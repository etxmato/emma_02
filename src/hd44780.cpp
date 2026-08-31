/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#define FOREGROUND 2
#define BACKGROUND 3

#include "main.h"
#include "hd44780.h"

HD44780::HD44780(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, Hd44780Configuration hd44780Configuration)
: Video(title, pos, size)
{
    clock_ = clock;
    videoNumber_ = hd44780Configuration.videoNumber;
    hd44780Configuration_ = hd44780Configuration;

    cols_ = hd44780Configuration.screenSize.x;   // 16
    rows_ = hd44780Configuration.screenSize.y;   // 2
    charW_ = hd44780Configuration.charSize.x;    // 5
    charH_ = hd44780Configuration.charSize.y;    // 8

    videoType_ = VIDEOXMLHD44780;
    colourIndex_ = COL_HD44780_FORE - 2;

    readCharRomFile(p_Main->getCharRomDir(), p_Main->getCharRomFile());

    // Initialize DDRAM with spaces (HD44780 reset state)
    memset(ddram_, 0x20, sizeof(ddram_));
    memset(cgram_, 0, sizeof(cgram_));

    fullScreenSet_ = false;
    zoom_ = zoom;

    double intPart;

#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    // Calculate pixel dimensions for the LCD display
    // Each character cell: charW_ pixels wide + 1 gap, charH_ pixels high + 1 gap
    int cellW = (charW_ + 1);
    int cellH = (charH_ + 1);
    int screenW = cols_ * cellW;
    int screenH = rows_ * cellH;

    videoWidth_ = screenW;
    videoHeight_ = screenH;

    screenCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    dcMemory.SelectObject(*screenCopyPointer);

    // The software framebuffer is enabled on ALL platforms (see video.h/
    // video.cpp); render per-pixel drawing through it (one DrawBitmap per
    // frame instead of per-pixel draws). The graphics context is only created
    // on macOS, where the non-framebuffer fallback draw path needs it.
    // On Linux wxGraphicsContext::Create(dcMemory) creates a Cairo context
    // bound to the screenCopyPointer Pixmap; leaving it unguarded caused a
    // BadDrawable X error on exit when that Pixmap was freed out of order.
#if defined(__WXMAC__)
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif
    enableFramebufferMac();

    videoScreenPointer = new VideoScreen(this, size, zoom, videoNumber_);

    // HD44780 reset state
    addressCounter_ = 0;
    activeRamIsDdram_ = true;
    displayOn_ = false;
    cursorOn_ = false;
    blinkOn_ = false;
    shiftOn_ = false;
    direction_ = 1;
    dataLen_ = 8;
    numLines_ = 1;
    charSize_ = 8;
    displayShift_ = 0;
    busyFlag_ = false;
    busyCycles_ = 0;
    cursorBlinkState_ = false;

    offsetX_ = 0;
    offsetY_ = 0;

    defineColours();
    backGround_ = BACKGROUND;

    setCycle();
    this->SetClientSize((videoWidth_ + 2 * borderX_[videoType_]) * zoom_, (videoHeight_ + 2 * borderY_[videoType_]) * zoom_);
    this->SetBackgroundColour(colour_[colourIndex_ + backGround_]);
}

HD44780::~HD44780()
{
    dcMemory.SelectObject(wxNullBitmap);
    delete screenCopyPointer;
    delete videoScreenPointer;
#if defined(__WXMAC__)
    delete gc;
#endif
}

void HD44780::configureHd44780()
{
    p_Main->configureMessage(&hd44780Configuration_.ioGroupVector, "HD44780");

    p_Computer->setOutType(&hd44780Configuration_.ioGroupVector, hd44780Configuration_.commandPort, "HD44780 command");
    p_Computer->setOutType(&hd44780Configuration_.ioGroupVector, hd44780Configuration_.dataPort, "HD44780 data");
    p_Computer->setInType(&hd44780Configuration_.ioGroupVector, hd44780Configuration_.statusPort, "HD44780 status");
    p_Computer->setInType(&hd44780Configuration_.ioGroupVector, hd44780Configuration_.dataReadPort, "HD44780 data read");

    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_HD44780, HD44780_CYCLE);
    p_Computer->setCycleType(CYCLE_TYPE_BLINK_HD44780, HD44780_BLINK_CYCLE);

    p_Main->message("");
}

void HD44780::initHd44780()
{
    cycleValue_ = cycleSize_;
    blinkValue_ = blinkSize_;
    reDraw_ = true;
    reBlit_ = false;
    newBackGround_ = false;
}

bool HD44780::ioGroup(int ioGroup)
{
    bool groupFound = false;
    
    if (hd44780Configuration_.ioGroupVector.size() == 0)
        groupFound = true;
    else
    {
        for (std::vector<int>::iterator ioGroupIterator = hd44780Configuration_.ioGroupVector.begin (); ioGroupIterator != hd44780Configuration_.ioGroupVector.end (); ++ioGroupIterator)
        {
            if (*ioGroupIterator == ioGroup)
                groupFound = true;
        }
    }
    return groupFound;
}

Byte HD44780::efHd44780()
{
    // Return busy flag status (active low or high depending on polarity)
    return (busyFlag_ ? 1 : 0) ^ hd44780Configuration_.ef.reverse;
}

void HD44780::writeCommand(Byte value, int showTrace)
{
    showTrace = p_Main->setHd44780Register(HD44780_COMMAND, value, showTrace);

    // HD44780 instruction register write (RS=0, R/W=0)
    if (value & 0x80)
    {
        // Set DDRAM address
        activeRamIsDdram_ = true;
        addressCounter_ = value & 0x7F;
        busyCycles_ = 10;
    }
    else if (value & 0x40)
    {
        // Set CGRAM address
        activeRamIsDdram_ = false;
        addressCounter_ = value & 0x3F;
        busyCycles_ = 10;
    }
    else if (value & 0x20)
    {
        // Function set
        dataLen_ = (value & 0x10) ? 8 : 4;
        numLines_ = (value & 0x08) ? 2 : 1;
        charSize_ = (value & 0x04) ? 10 : 8;
        if (numLines_ == 2) charSize_ = 8;  // 5x10 not available in 2-line mode
        busyCycles_ = 10;
    }
    else if (value & 0x10)
    {
        // Cursor or display shift
        int dir = (value & 0x04) ? 1 : -1;
        if (value & 0x08)
        {
            // Display shift
            displayShift_ += dir;
            if (displayShift_ >= 0x50) displayShift_ = 0;
            else if (displayShift_ < 0) displayShift_ = 0x4F;
        }
        else
        {
            // Cursor move
            updateAddressCounter(dir);
        }
        busyCycles_ = 10;
        reDraw_ = true;
    }
    else if (value & 0x08)
    {
        // Display on/off control
        displayOn_ = (value & 0x04) != 0;
        cursorOn_ = (value & 0x02) != 0;
        blinkOn_ = (value & 0x01) != 0;
        busyCycles_ = 10;
        reDraw_ = true;
    }
    else if (value & 0x04)
    {
        // Entry mode set
        direction_ = (value & 0x02) ? 1 : -1;
        shiftOn_ = (value & 0x01) != 0;
        busyCycles_ = 10;
    }
    else if (value & 0x02)
    {
        // Return home
        addressCounter_ = 0;
        activeRamIsDdram_ = true;
        displayShift_ = 0;
        busyCycles_ = 410;
        reDraw_ = true;
    }
    else if (value & 0x01)
    {
        // Clear display
        memset(ddram_, 0x20, sizeof(ddram_));
        addressCounter_ = 0;
        activeRamIsDdram_ = true;
        direction_ = 1;
        displayShift_ = 0;
        busyCycles_ = 410;
        reDraw_ = true;
    }

    busyFlag_ = true;
}

void HD44780::writeData(Byte value, int showTrace)
{
    showTrace = p_Main->setHd44780Register(HD44780_DATA, value, showTrace);
    // HD44780 data register write (RS=1, R/W=0)
    if (activeRamIsDdram_)
    {
        ddram_[addressCounter_ & 0x7F] = value;
        
        // Draw the character if it's in visible area
        int col, row;
        if (ddramAddressToPosition(addressCounter_, &col, &row))
            drawCharacter(col, row, value);
    }
    else
    {
        cgram_[addressCounter_ & 0x3F] = value;
        reDraw_ = true;  // CGRAM change may affect displayed characters
    }

    updateAddressCounter(direction_);
    if (shiftOn_)
    {
        displayShift_ += direction_;
        if (displayShift_ >= 0x50) displayShift_ = 0;
        else if (displayShift_ < 0) displayShift_ = 0x4F;
        reDraw_ = true;
    }

    busyFlag_ = true;
    busyCycles_ = 10;
}

Byte HD44780::readStatus()
{
    p_Main->setHd44780Register(HD44780_BUSYFLAG, (Byte)0, SHOW_ADDRESS_TRACE);
    // RS=0, R/W=1: returns busy flag + address counter
    // Memory-mapped mode: busy flag in bit 0 (as used by AMI gate array in PTC-701)
    if (hd44780Configuration_.statusPort.addressMode)
        return (busyFlag_ ? 0x01 : 0x00);

    return (busyFlag_ ? 0x80 : 0) | (addressCounter_ & 0x7F);
}

Byte HD44780::readData()
{
    p_Main->setHd44780Register(HD44780_ADDRESS, (Byte)0, SHOW_ADDRESS_TRACE);
    // RS=1, R/W=1: read data from DDRAM or CGRAM
    Byte data;
    if (activeRamIsDdram_)
        data = ddram_[addressCounter_ & 0x7F];
    else
        data = cgram_[addressCounter_ & 0x3F];

    updateAddressCounter(direction_);
    busyFlag_ = true;
    busyCycles_ = 10;
    return data;
}

void HD44780::cycleHd44780()
{
    // Count down busy flag
    if (busyFlag_)
    {
        busyCycles_--;
        if (busyCycles_ <= 0)
            busyFlag_ = false;
    }
}

void HD44780::blinkHd44780()
{
    blinkValue_--;
    if (blinkValue_ <= 0)
    {
        blinkValue_ = blinkSize_;
        videoSyncCount_++;

        if (changeScreenSize_)
            changeScreenSize();

        // Toggle cursor blink state
        cursorBlinkState_ = !cursorBlinkState_;

        copyScreen();

#ifndef __WXMAC__
        if (cursorOn_ || blinkOn_)
        {
            int col, row;
            if (ddramAddressToPosition(addressCounter_, &col, &row))
                drawCursor(col, row, cursorBlinkState_);
        }
#else
        p_Main->eventRefreshVideo(false, videoNumber_);
#endif
    }
}

void HD44780::setClock(double clock)
{
    clock_ = clock;
    reCycle_ = true;
}

void HD44780::setCycle()
{
    // HD44780 internal oscillator is ~270kHz
    // Blink rate is every 102400 cycles of the HD44780 internal clock
    // For Emma 02, we map this to CPU cycles
    // Approximate: blink every ~0.4 seconds
    float hd44780InternalClock = 270000.0f;  // Hz
    float blinkPeriodUs = (102400.0f / hd44780InternalClock) * 1000000.0f;  // microseconds
    float cpuCycleUs = (1.0f / (clock_ * 1000000.0f)) * 8.0f * 1000000.0f;  // microseconds per machine cycle
    
    blinkSize_ = (int)(blinkPeriodUs / cpuCycleUs);
    if (blinkSize_ < 1) blinkSize_ = 1;
    
    // Cycle for busy flag countdown - roughly 1 machine cycle per HD44780 clock tick
    cycleSize_ = (int)((1000000.0f / hd44780InternalClock) / cpuCycleUs);
    if (cycleSize_ < 1) cycleSize_ = 1;

    cycleValue_ = cycleSize_;
    blinkValue_ = blinkSize_;
    reCycle_ = false;
}

void HD44780::copyScreen()
{
    if (p_Main->isZoomEventOngoing())
        return;

    updateReColour();

    if (reCycle_)
        setCycle();

    if (reDraw_)
        drawScreen();

    finishCopyScreen();
}

void HD44780::drawScreen()
{
    // Fill background (LCD "off" color)
    setColour(colourIndex_ + backGround_);
    drawRectangle(0, 0, videoWidth_ + 2 * offsetX_, videoHeight_ + 2 * offsetY_);

    if (!displayOn_)
        return;

    // Draw all visible characters
    int lineSize = 80 / numLines_;  // 40 chars per line in 2-line mode
    
    for (int row = 0; row < rows_; row++)
    {
        for (int col = 0; col < cols_; col++)
        {
            int ddramAddr = row * 0x40 + ((col + displayShift_) % lineSize);
            Byte charCode = ddram_[ddramAddr & 0x7F];
            drawCharacter(col, row, charCode);
        }
    }
}

void HD44780::drawCharacter(int col, int row, Byte value)
{
    if (!displayOn_)
        return;

    int cellW = (charW_ + 1);
    int cellH = (charH_ + 1);
    int x = col * cellW + offsetX_;
    int y = row * cellH + offsetY_;

    // Clear character cell with background
    setColour(colourIndex_ + backGround_);
    drawRectangle(x, y, cellW, cellH);

    // Get font data
    const Byte *fontData;
    if (value < 0x10)
    {
        // CGRAM character (0x00-0x07 mirrored at 0x08-0x0F)
        fontData = cgram_ + (value & 0x07) * 8;
    }
    else
    {
        // CGROM character: 16 bytes per character
        fontData = cgrom_ + value * 16;
    }

    // Draw pixels
    setColour(colourIndex_ + FOREGROUND);
    for (int py = 0; py < charH_; py++)
    {
        Byte lineData = fontData[py];
        for (int px = 0; px < charW_; px++)
        {
            if (lineData & (0x10 >> px))  // HD44780 font is 5 pixels wide, bit 4 = leftmost
            {
                drawRectangle(x + px, y + py, 1, 1);
            }
        }
    }

    // Full-frame refresh on all platforms: the software framebuffer flushes
    // the whole plane each frame.
    reBlit_ = true;
}

void HD44780::drawCursor(int col, int row, bool status)
{
    if (!displayOn_)
        return;

    int cellW = (charW_ + 1);
    int cellH = (charH_ + 1);
    int x = col * cellW + offsetX_;
    int y = row * cellH + offsetY_;

    wxColour clr;

    if (blinkOn_ && status)
    {
        // Blink: fill entire character cell with foreground
        clr = colour_[colourIndex_ + FOREGROUND];
        videoScreenPointer->drawRectangle(clr, x * zoom_, y * zoom_, cellW * zoom_, cellH * zoom_);
    }
    else if (cursorOn_)
    {
        // Underline cursor at bottom row of character cell
        int cursorY = y + charH_ - 1;
        clr = status ? colour_[colourIndex_ + FOREGROUND] : colour_[colourIndex_ + backGround_];
        videoScreenPointer->drawRectangle(clr, x * zoom_, cursorY * zoom_, charW_ * zoom_, zoom_);
    }
}

void HD44780::drawCursor(wxDC &dc, int col, int row, bool status)
{
    if (!displayOn_)
        return;

    int cellW = (charW_ + 1);
    int cellH = (charH_ + 1);
    int x = col * cellW + offsetX_;
    int y = row * cellH + offsetY_;

    wxColour clr;

    if (blinkOn_ && status)
    {
        clr = colour_[colourIndex_ + FOREGROUND];
        dc.SetBrush(wxBrush(clr));
        dc.SetPen(wxPen(clr));
        dc.DrawRectangle(x, y, cellW, cellH);
    }
    else if (cursorOn_)
    {
        int cursorY = y + charH_ - 1;
        clr = status ? colour_[colourIndex_ + FOREGROUND] : colour_[colourIndex_ + backGround_];
        dc.SetBrush(wxBrush(clr));
        dc.SetPen(wxPen(clr));
        dc.DrawRectangle(x, cursorY, charW_, 1);
    }
}

bool HD44780::readCharRomFile(wxString romDir, wxString romFile)
{
    wxFFile inFile;
    size_t length;
    char buffer[4096];

    if (romFile == "")
    {
        p_Main->errorMessage("No HD44780 font filename specified");
        return false;
    }

    wxString fileName = romDir + romFile;

    if (inFile.Open(fileName, "rb"))
    {
        length = inFile.Read(buffer, 4096);
        for (size_t i = 0; i < length; i++)
        {
            cgrom_[i & 0xFFF] = (Byte)buffer[i];
        }
        inFile.Close();
        return true;
    }
    else
    {
        p_Main->errorMessage("Error reading " + fileName);
        return false;
    }
}

void HD44780::setFullScreen(bool fullScreenSet)
{
    fullScreenSet_ = fullScreenSet;
    ShowFullScreen(fullScreenSet);
}

void HD44780::onF3()
{
    fullScreenSet_ = !fullScreenSet_;
    p_Main->eventVideoSetFullScreen(fullScreenSet_, videoNumber_);
}

void HD44780::reBlit(wxDC &dc)
{
    if (!memoryDCvalid_)
        return;

    dc.Blit(0, 0, videoWidth_ + 2 * offsetX_, videoHeight_ + 2 * offsetY_, &dcMemory, 0, 0);

    if (extraBackGround_ && newBackGround_)
    {
        wxSize size = wxGetDisplaySize();

        dc.SetBrush(wxBrush(colour_[colourIndex_ + backGround_]));
        dc.SetPen(wxPen(colour_[colourIndex_ + backGround_]));

        int xStart = (int)((2 * offsetX_ + videoWidth_) * zoom_ * xZoomFactor_);
        dc.DrawRectangle(xStart, 0, size.x - xStart, size.y);

        int yStart = (int)((2 * offsetY_ + videoHeight_) * zoom_);
        dc.DrawRectangle(0, yStart, size.x, size.y - yStart);

        newBackGround_ = false;
    }

    if (displayOn_ && (cursorOn_ || blinkOn_))
    {
        int col, row;
        if (ddramAddressToPosition(addressCounter_, &col, &row))
            drawCursor(dc, col, row, cursorBlinkState_);
    }
}

// Helper: convert DDRAM address to visible column/row position
// Returns false if the address is not currently visible
bool HD44780::ddramAddressToPosition(Byte addr, int *col, int *row)
{
    int lineSize = 80 / numLines_;  // 40 in 2-line mode

    // Line 1: addresses 0x00-0x27
    if (addr <= 0x27)
    {
        *row = 0;
        int pos = (int)addr - displayShift_;
        if (pos < 0) pos += lineSize;
        if (pos >= lineSize) pos -= lineSize;
        if (pos >= 0 && pos < cols_)
        {
            *col = pos;
            return true;
        }
    }
    // Line 2: addresses 0x40-0x67
    else if (addr >= 0x40 && addr <= 0x67)
    {
        *row = 1;
        int pos = (int)(addr - 0x40) - displayShift_;
        if (pos < 0) pos += lineSize;
        if (pos >= lineSize) pos -= lineSize;
        if (pos >= 0 && pos < cols_)
        {
            *col = pos;
            return true;
        }
    }

    return false;
}

// Helper: update address counter with wrapping
void HD44780::updateAddressCounter(int dir)
{
    if (activeRamIsDdram_)
    {
        if (numLines_ == 1)
        {
            addressCounter_ = (addressCounter_ + dir) & 0x4F;
            if (addressCounter_ > 0x4F)
                addressCounter_ = 0;
        }
        else
        {
            // 2-line mode: wrap within each line
            if (addressCounter_ >= 0x40)
            {
                // Line 2
                if (dir > 0 && addressCounter_ >= 0x67)
                    addressCounter_ = 0x40;
                else if (dir < 0 && addressCounter_ < 0x40)
                    addressCounter_ = 0x67;
                else
                    addressCounter_ += dir;
            }
            else
            {
                // Line 1
                if (dir > 0 && addressCounter_ >= 0x27)
                    addressCounter_ = 0x00;
                else if (dir < 0 && (addressCounter_ == 0 || addressCounter_ > 0x7F))
                    addressCounter_ = 0x27;
                else
                    addressCounter_ += dir;
            }
        }
    }
    else
    {
        // CGRAM: wrap at 0x3F
        addressCounter_ = (addressCounter_ + dir) & 0x3F;
    }
}

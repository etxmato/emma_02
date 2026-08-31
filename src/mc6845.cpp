/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 ***                                                             ***
 *** 1802 Code based on elf emulator by Michael H Riley with     ***
 *** copyright as below                                          ***
 *******************************************************************
*/

/*
 *******************************************************************
 *** This software is copyright 2006 by Michael H Riley          ***
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
#include "mc6845.h"

MC6845::MC6845(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, Mc6845Configuration mc6845Configuration)
: Video(title, pos, size)
{
    charW_ = mc6845Configuration.charSize.x;
    clock_ = clock;
    videoNumber_ = mc6845Configuration.videoNumber;
    mc6845Configuration_ = mc6845Configuration;

    int regVal [16] = {94, mc6845Configuration.screenSize.x, 77, 8, 28, 2, mc6845Configuration.screenSize.y, 22, 0, mc6845Configuration.charSize.y, 20, 8, 0, 0, 3, 0xc0};

    windowSize_ = size;
    colourIndex_ = 0;

    videoType_ = VIDEOXML6845;
    colourIndex_ = COL_MC6845_FORE-2;

    interlaceOR_ = p_Main->getInterlace();
    readCharRomFile(p_Main->getCharRomDir(), p_Main->getCharRomFile());
    mc6845RamStart_ = mc6845Configuration.startRam; //p_Main->getConfigItem(elfTypeStr_+"/mc6845StartRam",0xE000l);
    mc6845RamEnd_ = mc6845Configuration.endRam; //p_Main->getConfigItem(elfTypeStr_+"/mc6845EndRam", 0xE7FFl);

    mc6845AddressRegister_ = mc6845Configuration.address; //p_Main->getConfigItem(elfTypeStr_+"/mc6845Address",0xE800l);
    mc6845DataRegister_ = mc6845Configuration.data; //p_Main->getConfigItem(elfTypeStr_+"/mc6845Data",0xE801l);

    switch (p_Main->getCpuStartupVideoRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<=mc6845Configuration.ramMask; i++) mc6845ram_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<=mc6845Configuration.ramMask; i++) mc6845ram_[i] = rand() % 0x100;
        break;
            
        case STARTUP_DYNAMIC:
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<=mc6845Configuration.ramMask; i++) mc6845ram_[i] = p_Computer->getDynamicByte(i);
        break;
    }
    
    fullScreenSet_ = false;
    zoom_ = zoom;

    double intPart;

#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    screenCopyPointer = new wxBitmap(size.x, size.y);
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
    cursorAddress_ = 0;
    startAddress_ = 0;
    cursorOn_ = false;
    cursorBlink_ = true;
    cursorBlinkOn_ = false;
    cursorBlinkTime_ = 16;
    charLine_ = 0;
    rows_ = 0;
    scanLine_ = 0;
    blink_ = cursorBlinkTime_;
    offsetX_ = 0;
    offsetY_ = 0;

    defineColours();
    backGround_ = BACKGROUND;

    for (int i=0; i<16; i++)
    {
        register_[i] = 0xff;
        writeAddressRegister6845(i, DO_NOT_SHOW_ANY_TRACE);
        writeData6845(regVal[i], DO_NOT_SHOW_ANY_TRACE);
    }
    setCycle();
    zoom_ = zoom;
    this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);
    this->SetBackgroundColour(colour_[colourIndex_+backGround_]);
}

MC6845::~MC6845()
{
    dcMemory.SelectObject(wxNullBitmap);
    delete screenCopyPointer;
    delete videoScreenPointer;
#if defined(__WXMAC__)
    delete gc;
#endif
}

void MC6845::configure6845()
{
    p_Main->configureMessage(&mc6845Configuration_.ioGroupVector, "MC6845");

    wxString printBuffer;
    printBuffer.Printf("	%04X-%04X: Video RAM", mc6845RamStart_, mc6845RamEnd_);
    p_Main->message(printBuffer);
    if (mc6845Configuration_.address != -1)
    {
        printBuffer.Printf("	%04X: CRTC address register, %04X: CRTC data register", mc6845AddressRegister_, mc6845DataRegister_);
        p_Main->message(printBuffer);
    }
    p_Computer->setEfType(&mc6845Configuration_.ioGroupVector, mc6845Configuration_.ef, "Display enable");
    p_Computer->setCycleType(CYCLE_TYPE_BLINK_MC6845, MC6845_BLINK_CYCLE);
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_MC6845, MC6845_CYCLE);

    p_Main->message("");
    
}

void MC6845::init6845()
{
    cycleValue6845_ = cycleSize6845_;
    blinkValue6845_ = blinkSize6845_;
    reDraw_ = true;
    reBlit_ = false;
    newBackGround_ = false;
}

Byte MC6845::ef6845()
{
    return ((cycleValue6845_ < nonDisplay6845_) ? 0 : 1)^mc6845Configuration_.ef.reverse;
}

void MC6845::writeAddressRegister6845(Byte value, int showTrace)
{
    registerIndex_ = value&0x1f;
    p_Main->setMc6845Register(MC6845_ADDRESS, (Byte)registerIndex_, showTrace);
}

Byte MC6845::readData6845(Word addr)
{
    if (addr != mc6845DataRegister_)  return 255;
    Byte Ret = 0;

    switch(registerIndex_)
    {
        case 14:
            Ret = register_[registerIndex_];
        break;

        case 15:
            Ret = register_[registerIndex_];
        break;
    }
    return Ret;
}

Byte MC6845::readDataDirect6845(Word WXUNUSED(addr))
{
    Byte Ret = 0;

    switch(registerIndex_)
    {
        case 14:
            Ret = register_[registerIndex_];
        break;

        case 15:
            Ret = register_[registerIndex_];
        break;
    }
    return Ret;
}

void MC6845::writeData6845(Byte value, int showTrace)
{
    showTrace = p_Main->setMc6845Register(MC6845_DATA, value, showTrace);
    writeRegister6845(registerIndex_, value, showTrace);
}

void MC6845::writeRegister6845(Byte regNumber, Byte value, int showTrace)
{
    switch(regNumber)
    {
        case 0:
            if ((value + 1) != Nht_)
            {
                Nht_= value + 1;
                reCycle_ = true;
            }
            p_Main->setMc6845Register(MC6845_R0, value, showTrace);
        break;

        case 1:
            if (value != charLine_)
            {
                charLine_ = value;
                videoWidth_ = charLine_*charW_;
                reDraw_ = true;
                setScreenSize();
                reCycle_ = true;

                resetScreenCopyPointer();
            }
            p_Main->setMc6845Register(MC6845_R1, value, showTrace);
        break;

        case 2:
            p_Main->setMc6845Register(MC6845_R2, value, showTrace);
        break;

        case 3:
            value &= 0xf;
            p_Main->setMc6845RegisterNibble(MC6845_R3, value, showTrace);
        break;

        case 4:
            value &= 0x7f;
            if ((value + 1) != Nvt_)
            {
                Nvt_= value + 1;
                reCycle_ = true;
            }
            p_Main->setMc6845Register(MC6845_R4, value, showTrace);
        break;

        case 5:
            value &= 0x1f;
            if (value != Nadj_)
            {
                Nadj_= value;
                reCycle_ = true;
            }
            p_Main->setMc6845Register(MC6845_R5, value, showTrace);
        break;

        case 6:
            value &= 0x7f;
            if (value != rows_)
            {
                rows_ = value;
                videoHeight_ = rows_*scanLine_*2;
                reDraw_ = true;
                setScreenSize();

                resetScreenCopyPointer();
            }
            p_Main->setMc6845Register(MC6845_R6, value, showTrace);
        break;

        case 7:
            value &= 0x7f;
            p_Main->setMc6845Register(MC6845_R7, value, showTrace);
        break;

        case 8:
            value &= 0x3;
            interlace_ = ((value&1) == 1);
            if (interlaceOR_ == true)
                interlace_ = 1;

            if ((value&0x3) == 3)
                videoM_ = 1;
            else
                videoM_ = 2;

            resetScreenCopyPointer();

            reDraw_ = true;
            p_Main->setMc6845RegisterNibble(MC6845_R8, value, showTrace);
        break;

        case 9:
            value &= 0x1f;
            if (value != (scanLine_-1))
            {
                scanLine_ = value + 1;
                videoHeight_ = rows_*scanLine_*2;
                reDraw_ = true;
                setScreenSize();
                reCycle_ = true;

                resetScreenCopyPointer();
            }
            p_Main->setMc6845Register(MC6845_R9, value, showTrace);
        break;

        case 10:
            value &= 0x7f;
            if (value != register_[regNumber])
            {
                cursorStartLine_ = value & 0x1f;
                if (cursorStartLine_ > 15)  cursorStartLine_ = 15;
                switch(value & 0x60)
                {
                    case 0x00:
                        cursorOn_ = true;
                        cursorBlink_ = false;
                        cursorBlinkTime_ = 16;
                        cursorBlinkOn_ = true;
                    break;

                    case 0x20:
                        cursorOn_ = false;
                    break;

                    case 0x40:
                        cursorOn_ = true;
                        cursorBlink_ = true;
                        cursorBlinkTime_ = 16;
                    break;

                    case 0x60:
                        cursorOn_ = true;
                        cursorBlink_ = true;
                        cursorBlinkTime_ = 32;
                    break;
                }
            }
            p_Main->setMc6845Register(MC6845_R10, value, showTrace);
        break;

        case 11:
            value &= 0x1f;
            if (value != register_[regNumber])
            {
                cursorEndLine_ = value;
                if (cursorEndLine_ > 15)  cursorEndLine_ = 15;
            }
            p_Main->setMc6845Register(MC6845_R11, value, showTrace);
        break;

        case 12:
            value &= 0x3f;
            if (value != register_[regNumber])
            {
                startAddress_ = (startAddress_ & 0xff) + (value << 8);
                reDraw_ = true;
            }
            p_Main->setMc6845Register(MC6845_R12_13, (Word)startAddress_, showTrace);
        break;

        case 13:
            if (value != register_[regNumber])
            {
                startAddress_ = (startAddress_ & 0x3f00) + value;
                reDraw_ = true;
            }
            p_Main->setMc6845Register(MC6845_R12_13, (Word)startAddress_, showTrace);
        break;

        case 14:
            value &= 0x3f;
            if (value != register_[regNumber])
            {
                draw6845(cursorAddress_& 0x7ff, mc6845ram_[cursorAddress_& mc6845Configuration_.ramMask]);
                cursorAddress_ = (cursorAddress_ & 0xff) +(value << 8);
            }
            p_Main->setMc6845Register(MC6845_R14_15, (Word)cursorAddress_, showTrace);
        break;

        case 15:
            if (value != register_[regNumber])
            {
                draw6845(cursorAddress_& 0x7ff, mc6845ram_[cursorAddress_& mc6845Configuration_.ramMask]);
                cursorAddress_ = (cursorAddress_ & 0x3f00) + value;
            }
            p_Main->setMc6845Register(MC6845_R14_15, (Word)cursorAddress_, showTrace);
        break;
    }
    register_[regNumber&0x1f] = value;
}

void MC6845::cycle6845()
{
    cycleValue6845_--;
    if (cycleValue6845_ <= 0)
        cycleValue6845_ = cycleSize6845_;
}

void MC6845::blink6845()
{
    blinkValue6845_--;
    if (blinkValue6845_ <= 0)
    {
        blinkValue6845_ = blinkSize6845_;
        videoSyncCount_++;
        if (changeScreenSize_)
            changeScreenSize();
        copyScreen();
        blink_--;
        if (blink_ <= 0)
        {
            blink_ = cursorBlinkTime_;
            if (cursorOn_ && cursorBlink_)
                    cursorBlinkOn_ = !cursorBlinkOn_;
        }
#ifndef __WXMAC__
        if (cursorOn_)
            drawCursor6845(cursorAddress_, cursorBlinkOn_);
#else
        p_Main->eventRefreshVideo(false, videoNumber_);
#endif
    }
}

void MC6845::setClock(double clock)
{
    clock_ = clock;
    reCycle_ = true;
}

void MC6845::setCycle()
{   // DOT = 14.31818
    float clockPeriod = (float)(1/14.318180 * charW_);
    float scanLinePeriod = (float)Nht_ * clockPeriod;
    float characterRowPeriod = (float)(scanLine_ + 1) * scanLinePeriod;
    int fieldTime = (int)((float)(Nvt_ * characterRowPeriod) + (Nadj_ * scanLinePeriod));

    cycleSize6845_ = (int)(scanLinePeriod / ((1/clock_) * 8));
    nonDisplay6845_ = cycleSize6845_ - (int)((charLine_ * clockPeriod) / ((1/clock_) * 8));
    blinkSize6845_ = (int)(fieldTime / ((1/clock_) * 8));

    cycleValue6845_ = cycleSize6845_;
    blinkValue6845_ = blinkSize6845_;
    reCycle_ = false;
}

Byte MC6845::read6845(Word addr)
{
     return mc6845ram_[addr & mc6845Configuration_.ramMask];
}

void MC6845::write6845(Word addr, Byte value)
{
    Word memoryStart = p_Computer->getDebugMemoryStart();
    if (addr >= memoryStart && addr<(memoryStart + 256))
        p_Main->updateDebugMemory(addr);
    draw6845(addr, value);
    p_Main->assLog(value);
}

Byte MC6845::read6845CharRom(Word addr)
{
     return mc6845CharRom_[addr&0x7ff];
}

void MC6845::write6845CharRom(Word addr, Byte value)
{
    mc6845CharRom_[addr&0x7ff] = value;
    reDraw_ = true;
}

void MC6845::copyScreen()
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

int MC6845::copyScreenHeight()
{
    return rows_*scanLine_*videoM_+2*offsetY_;
}

void MC6845::drawScreen()
{
    int addr = startAddress_;
    setColour(colourIndex_+backGround_);
    drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);
    for (int i=0; i<(charLine_*rows_); i++)
    {
        draw6845(addr, mc6845ram_[addr & mc6845Configuration_.ramMask]);

        addr++;
        addr &= 0x7ff;
    }
}

void MC6845::draw6845(Word addr, Byte value)
{
    mc6845ram_[addr & mc6845Configuration_.ramMask] = value;

    addr = (addr - startAddress_) & 0x7ff;

    int y = (addr/charLine_)*scanLine_*videoM_;
    int x = (addr%charLine_)*charW_;
    drawCharacter6845(x, y, value);
}

void MC6845::drawCharacter6845(wxCoord x, wxCoord y, Byte v)
{
    int line_byte, line;

    setColour(colourIndex_+backGround_);
    drawRectangle(x+offsetX_, y+offsetY_, charW_, scanLine_*videoM_);

    setColour(colourIndex_+FOREGROUND);

    if (v >= 0x80)
    {
        if (interlace_ & !(videoM_ == 1))
            drawRectangle(x+offsetX_, y+offsetY_+(scanLine_-1)*videoM_, charW_, 2);
        else
            drawRectangle(x+offsetX_, y+offsetY_+(scanLine_-1)*videoM_, charW_, 1);
    }

    // Full-frame refresh on all platforms: the software framebuffer flushes
    // the whole plane each frame.
    reBlit_ = true;

    line = 0;
    for (wxCoord j=y; j<y+(scanLine_-1)*videoM_; j+=videoM_)
    {
        line_byte = mc6845CharRom_[v*mc6845Configuration_.numberOfRomBytesPerChar+line];
        for (wxCoord i=x; i<x+charW_; i++)
        {
            if (line_byte & 128)
            {
                if (interlace_ & !(videoM_ == 1))
                    drawRectangle(i+offsetX_, j+offsetY_, 1, videoM_);
                else
                    drawPoint(i+offsetX_, j+offsetY_);
            }
            line_byte <<= 1;
        }
        line++;
    }
}

void MC6845::drawCursor6845(Word addr, bool status)
{
    Byte v;
    int line_byte, line;
    wxColour clr;

    addr = (addr - startAddress_) & 0x7ff;

    int y = (addr/charLine_)*scanLine_*videoM_;
    int x = (addr%charLine_)*charW_;

    v = mc6845ram_[addr & mc6845Configuration_.ramMask];
    line = cursorStartLine_;

    for (int yLine = y + cursorStartLine_*videoM_; yLine <= (y + cursorEndLine_*videoM_); yLine+=videoM_)
    {
        //if (yLine == (y + (scanLine_-1)*videoM_))
        if (cursorBlinkOn_)
        {
            if (v<0x80)
            {
                if (status)
                    clr = colour_[colourIndex_+FOREGROUND];
                else
                    clr = colour_[colourIndex_+backGround_];
            }
            else
            {
                if (status)
                    clr = colour_[colourIndex_+backGround_];
                else
                    clr = colour_[colourIndex_+FOREGROUND];
            }
            if (interlace_ & !(videoM_ == 1))
                videoScreenPointer->drawRectangle(clr, (x+offsetX_)*zoom_, (yLine+offsetY_)*zoom_, charW_*zoom_, 2*zoom_);
            else
                videoScreenPointer->drawRectangle(clr, (x+offsetX_)*zoom_, (yLine+offsetY_)*zoom_, charW_*zoom_, zoom_);
        }
        else
        {
            line_byte = mc6845CharRom_[v*mc6845Configuration_.numberOfRomBytesPerChar+line];
            for (wxCoord i=x; i<x+charW_; i++)
            {
                if (line_byte & 128)
                {
                    if (status)
                        clr = colour_[colourIndex_+backGround_];
                    else
                        clr = colour_[colourIndex_+FOREGROUND];
                }
                else
                {
                    if (status)
                        clr = colour_[colourIndex_+FOREGROUND];
                    else
                        clr = colour_[colourIndex_+backGround_];
                }
                if (interlace_ & !(videoM_ == 1))
                    videoScreenPointer->drawRectangle(clr, (i+offsetX_)*zoom_, (yLine+offsetY_)*zoom_, zoom_, 2*zoom_);
                else
                    videoScreenPointer->drawRectangle(clr, (i+offsetX_)*zoom_, (yLine+offsetY_)*zoom_, zoom_, zoom_);
                line_byte <<= 1;
            }
        }
        line++;
    }
}

void MC6845::drawCursor6845(wxDC &dc, Word addr, bool status)
{
    Byte v;
    int line_byte, line;
    wxColour clr;

    addr = (addr - startAddress_) & 0x7ff;

    int y = (addr/charLine_)*scanLine_*videoM_;
    int x = (addr%charLine_)*charW_;

    v = mc6845ram_[addr & mc6845Configuration_.ramMask];
    line = cursorStartLine_;
    
    for (int yLine = y + cursorStartLine_*videoM_; yLine <= (y + cursorEndLine_*videoM_); yLine+=videoM_)
    {
        if (cursorBlinkOn_)
        {
            if (v<0x80)
            {
                if (status)
                    clr = colour_[colourIndex_+FOREGROUND];
                else
                    clr = colour_[colourIndex_+backGround_];
            }
            else
            {
                if (status)
                    clr = colour_[colourIndex_+backGround_];
                else
                    clr = colour_[colourIndex_+FOREGROUND];
            }
            dc.SetBrush(wxBrush(clr));
            dc.SetPen(wxPen(clr));
            if (interlace_ & !(videoM_ == 1))
                dc.DrawRectangle(x+offsetX_, yLine+offsetY_, charW_, 2);
            else
                dc.DrawRectangle(x+offsetX_, yLine+offsetY_, charW_, 1);
        }
        else
        {
            line_byte = mc6845CharRom_[v*mc6845Configuration_.numberOfRomBytesPerChar+line];
            for (wxCoord i=x; i<x+charW_; i++)
            {
                if (line_byte & 128)
                {
                    if (status)
                        clr = colour_[colourIndex_+backGround_];
                    else
                        clr = colour_[colourIndex_+FOREGROUND];
                }
                else
                {
                    if (status)
                        clr = colour_[colourIndex_+FOREGROUND];
                    else
                        clr = colour_[colourIndex_+backGround_];
                }
                dc.SetBrush(wxBrush(clr));
                dc.SetPen(wxPen(clr));
                if (interlace_ & !(videoM_ == 1))
                {
                    dc.DrawRectangle(i+offsetX_, yLine+offsetY_, 1, 1);
                    dc.DrawRectangle(i+offsetX_, yLine+offsetY_+1, 1, 1);
                }
                else
                    dc.DrawRectangle(i+offsetX_, yLine+offsetY_, 1, 1);
                line_byte <<= 1;
            }
        }
        line++;
    }
#if defined(__linux__)
    this->Update();
#endif
}

void MC6845::setInterlace(bool status)
{
    interlace_ = status;
    if (status)
        register_[8] |= 1;
    else
        register_[8] &= 0xfe;
    reDraw_ = true;
}

bool MC6845::readCharRomFile(wxString romDir, wxString romFile)
{
    wxFFile inFile;
    size_t length, number;
    char buffer[2048];

    if (romFile == "")
    {
        p_Main->errorMessage("No font filename specified");
        return false;
    }

    wxString fileName = romDir + romFile;

    if (inFile.Open(fileName, "rb"))
    {
        length = inFile.Read(buffer, 2048);
        number = 0;
        for (size_t i=0; i<length; i++)
        {
            mc6845CharRom_[i&0x7ff] = (Byte)buffer[i];
            number++;
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

void MC6845::setFullScreen(bool fullScreenSet)
{
    fullScreenSet_ = fullScreenSet;
    ShowFullScreen(fullScreenSet);
}

void MC6845::onF3()
{
    fullScreenSet_ = !fullScreenSet_;
    p_Main->eventVideoSetFullScreen(fullScreenSet_, videoNumber_);
}

void MC6845::reBlit(wxDC &dc)
{
    if (!memoryDCvalid_)
        return;
    
    dc.Blit(0, 0, videoWidth_+2*offsetX_, rows_*scanLine_*videoM_+2*offsetY_, &dcMemory, 0, 0);
    
    if (extraBackGround_ && newBackGround_)
    {
        wxSize size = wxGetDisplaySize();

        dc.SetBrush(wxBrush(colour_[colourIndex_+backGround_]));
        dc.SetPen(wxPen(colour_[colourIndex_+backGround_]));

        int xStart = (int)((2*offsetX_+videoWidth_)*zoom_*xZoomFactor_);
        dc.DrawRectangle(xStart, 0, size.x-xStart, size.y);

        int yStart = (int)((2*offsetY_+videoHeight_)*zoom_);
        dc.DrawRectangle(0, yStart, size.x, size.y-yStart);

        newBackGround_ = false;
    }
    if (cursorOn_)
        drawCursor6845(dc, cursorAddress_, cursorBlinkOn_);
}

void MC6845::resetScreenCopyPointer()
{
    if (rows_ == 0 || scanLine_ == 0 || videoM_ == 0 || videoWidth_ == 0)
        return;
    
    dcMemory.SelectObject(wxNullBitmap);
    delete screenCopyPointer;
    
    screenCopyPointer = new wxBitmap(2*offsetX_+videoWidth_, 2*offsetY_+rows_*scanLine_*videoM_);
//    screenCopyPointer = new wxBitmap(videoWidth_, rows_*scanLine_*videoM_);
    dcMemory.SelectObject(*screenCopyPointer);

#ifdef __WXMAC__
    delete gc;
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif
}


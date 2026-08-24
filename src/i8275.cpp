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

#define C_NONE 0
#define C_RESET 1
#define C_START_DISPLAY 2
#define C_STOP_DISPLAY 3
#define C_LIGHT_PEN 4
#define C_LOAD_CURSOR 5
#define C_ENABLE_INT 6
#define C_DISABLE_INT 7
#define C_PRESET_COUNTERS 8

#define FOREGROUND8275 2
#define BACKGROUND8275 3
#define HIGHLIGHT8275 4

#include "main.h"
#include "i8275.h"

int const SSS [8] = {0, 7, 15, 23, 31, 39, 47, 55};
int const BB [4] = {1, 2, 4, 8};

BEGIN_EVENT_TABLE(i8275, wxFrame)
    EVT_CLOSE (i8275::onClose)
    EVT_SIZE(i8275::onSize)
END_EVENT_TABLE()

i8275::i8275(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, I8275Configuration i8275Configuration)
: Video(title, pos, size)
{
    clock_ = clock;
    colourIndex_ = 0;
    videoNumber_ = i8275Configuration.videoNumber;;
    i8275Configuration_ = i8275Configuration;

#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    videoType_ = VIDEOXMLI8275;
    colourIndex_ = COL_I8275_FORE-2;
    
    readCharRomFile(p_Main->getCharRomDir(), p_Main->getCharRomFile());
    interlace_ = p_Main->getInterlace();

    offsetX_ = 0;
    offsetY_ = 0;

    defineColours();
    backGround_ = BACKGROUND8275;

    fullScreenSet_ = false;
    zoom_ = zoom;

    double intPart;
    zoomFraction_ = (modf(zoom_, &intPart) != 0);

    burstCountCode_ = 8;
    burstSpaceCode_ = 0;
    lineCounterMode_ = 1;

    videoWidth_ = i8275Configuration_.screenSize.x*i8275Configuration_.charSize.x;
    videoHeight_ = i8275Configuration_.screenSize.y*i8275Configuration_.charSize.y*2;

    screenCopyPointer = new wxBitmap(videoWidth_*zoom_, videoHeight_*zoom_);
    dcMemory.SelectObject(*screenCopyPointer);

#if defined(__WXMAC__)
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
    enableFramebufferMac();
#endif

    setColour(colourIndex_+backGround_);
    drawRectangle(0, 0, videoWidth_, videoHeight_);

    videoScreenPointer = new VideoScreen(this, size, zoom, videoNumber_);

    setCycle();

    cycleValue8275_ = -1;
    cycleBlankValue8275_ = fullCycleSize8275_;
    dmaCycleValue8275_ = -1;
    retrace_ = false;
    screenLocation_ = 0;
    bufferLocation_ = 0;
    reverse_ = false;
    underline_ = false;
    highlight_ = false;
    gpa_ = 0;
    blink_ = false;
    blinkOn_ = false;
    cursorBlinkOn_ = true;
    cursorLine_ = false;
    cursorBlinking_ = false;
    attributeChange_ = false;
    blinkCount_ = 16;
    videoM_ = 2;
    status_ = 0;
    p_Main->setI8275Register(I8275_STATUS, status_, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setI8275SelectorValue(I8275_DISPLAY, false, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setI8275SelectorValue(I8275_INTERRUPT, false, DO_NOT_SHOW_ANY_TRACE);
    rowEf_ = 1;
    frameEf_ = 1;
    cursorCharPosition_ = 0;
    cursorRowPosition_ = 0;
    cursorAddress_ = 0;
    displayOn_ = false;
    dmaOn_ = false;
    interruptOn_ = false;
    switch (p_Main->getCpuStartupVideoRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<5120; i++)
                i8275ram_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<5120; i++)
                i8275ram_[i] = rand() % 0x100;
        break;
            
        case STARTUP_DYNAMIC:
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<5120; i++)
                i8275ram_[i] = p_Computer->getDynamicByte(i);
        break;
    }
    for (int i=0; i<5120; i++)
    {
        cursorReset_[i] = false;
        reverseScr_[i] = false;
        underlineScr_[i] = false;
        highlightScr_[i] = false;
        blinkScr_[i] = false;
        graphicLineScr_[i] = false;
        gpaScr_[i] = 0;
    }

    this->SetBackgroundColour(colour_[colourIndex_+backGround_]);
    this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);
    characterListPointer8275 = NULL;
}

i8275::~i8275()
{
    CharacterList8275 *temp;

    dcMemory.SelectObject(wxNullBitmap);
    delete screenCopyPointer;
    delete videoScreenPointer;
#if defined(__WXMAC__)
    delete gc;
#endif
    if (updateCharacter8275_ > 0)
    {
        while(characterListPointer8275 != NULL)
        {
            temp = characterListPointer8275;
            characterListPointer8275 = temp->nextCharacter;
            delete temp;
        }
    }
}

void i8275::configure8275()
{
    p_Main->configureMessage(&i8275Configuration_.ioGroupVector, "intel 8275");
    p_Computer->setOutType(&i8275Configuration_.ioGroupVector, i8275Configuration_.writeParameter, "write parameter");
    p_Computer->setOutType(&i8275Configuration_.ioGroupVector, i8275Configuration_.writeCommandOutput, "write command");
    p_Computer->setInType(&i8275Configuration_.ioGroupVector, i8275Configuration_.readParameter, "read parameter");
    p_Computer->setInType(&i8275Configuration_.ioGroupVector, i8275Configuration_.readStatus, "read status");
    p_Computer->setEfType(&i8275Configuration_.ioGroupVector, i8275Configuration_.efVerticalRetrace, "vertical retrace");
    p_Computer->setEfType(&i8275Configuration_.ioGroupVector, i8275Configuration_.efHorizontalRetrace, "horizontal retrace");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_I8275, I8275_CYCLE);
    
    p_Main->message("");
}

void i8275::init8275()
{
    reDraw_ = true;
    reBlit_ = false;
    reBlink_ = false;
    newBackGround_ = false;
    updateCharacter8275_ = 0;
}

Byte i8275::frameEf8275()
{
    return frameEf_^i8275Configuration_.efVerticalRetrace.reverse;
}

Byte i8275::rowEf8275()
{
    return rowEf_^i8275Configuration_.efHorizontalRetrace.reverse;
}

void i8275::setRowEf8275(Byte value)
{
    status_ &= 0xc4;
    rowEf_ = value;
    p_Main->setI8275Register(I8275_STATUS, status_, DO_NOT_SHOW_ANY_TRACE);
}

void i8275::pRegWrite(Byte value, int showTrace)
{
    if (command_ == 0)
    {
        status_ |= 0x8;
        showTrace = p_Main->setI8275Register(I8275_WRITE_PARAMETER, value, showTrace);
        p_Main->setI8275Register(I8275_STATUS, status_, DO_NOT_SHOW_ANY_TRACE);
        return;
    }

    showTrace = p_Main->setI8275Register(I8275_WRITE_PARAMETER, value, showTrace);
    switch (command_)
    {
        case C_RESET:
            switch (parameters_)
            {
                case 4:
                    spacedRows_ = ((value & 0x80) == 1);
                    showTrace = p_Main->setI8275SelectorValue(I8275_PARAMETER_S, spacedRows_, showTrace);
                    showTrace = setCharRow(value);
                break;

                case 3:
                    showTrace = setVerticalRetrace(value >> 6, showTrace);
                    showTrace = setRowsFrame(value, showTrace);
                break;

                case 2:
                    showTrace = setUnderline(value >> 4, showTrace);
                    showTrace = setLinesRow(value, showTrace);
                break;

                case 1:
                    lineCounterMode_ = (value & 0x80) >> 7;
                    showTrace = p_Main->setI8275SelectorValue(I8275_PARAMETER_M, lineCounterMode_ == 1, showTrace);
                    fieldAttributeMode_ = (value & 0x40) >> 6;
                    showTrace = p_Main->setI8275SelectorValue(I8275_PARAMETER_F, fieldAttributeMode_ == 1, showTrace);
                    showTrace = setCursorFormat(value >> 4, showTrace);
                    showTrace = setHorizontalRetrace(value, showTrace);
                break;
            }
            reCycle_ = true;
        break;

        case C_LOAD_CURSOR:
            switch (parameters_)
            {
                case 2:
                    showTrace = setCursorChar(value, showTrace);
                break;
                case 1:
                    showTrace = setCursorRow(value, showTrace);
                break;
            }
        break;
    }

    parameters_--;
    if (parameters_ < 0)
    {
        status_ |= 0x8;
        parameters_ = 0;
    }
    p_Main->setI8275Register(I8275_STATUS, status_, DO_NOT_SHOW_ANY_TRACE);
}

int i8275::setCharRow(Byte value, int showTrace)
{
    value = (value & 0x7f) + 1;
    if (value >= 80)  value = 80;
    i8275Configuration_.screenSize.x = value;
    videoWidth_ = i8275Configuration_.screenSize.x * i8275Configuration_.charSize.x;
    setScreenSize();
    reCycle_ = true;
    reDraw_ = true;

    return p_Main->setI8275Register(I8275_CHAR_ROW, value, showTrace);
}

int i8275::setVerticalRetrace(Byte value, int showTrace)
{
    verticalRetraceRowCount_ = (value & 0x3) + 1;
    reCycle_ = true;
    reDraw_ = true;

    return p_Main->setI8275RegisterNibble(I8275_VERTICAL_RETRACE, (Byte)verticalRetraceRowCount_, showTrace);
}

int i8275::setRowsFrame(Byte value, int showTrace)
{
    i8275Configuration_.screenSize.y = (value & 0x3f) + 1;
    videoHeight_ = i8275Configuration_.screenSize.y*i8275Configuration_.charSize.y*2;
    setScreenSize();
    reCycle_ = true;
    reDraw_ = true;

    return p_Main->setI8275Register(I8275_ROWS_FRAME, (Byte)i8275Configuration_.screenSize.y, showTrace);
}

int i8275::setUnderline(Byte value, int showTrace)
{
    underLinePlacement_ = value & 0xf;
    reCycle_ = true;
    reDraw_ = true;

    return p_Main->setI8275RegisterNibble(I8275_UNDERLINE, (Byte)(underLinePlacement_+1), showTrace);
}

int i8275::setLinesRow(Byte value, int showTrace)
{
    i8275Configuration_.charSize.y = (value & 0x0f) + 1;
    videoHeight_ = i8275Configuration_.screenSize.y*i8275Configuration_.charSize.y*2;
    setScreenSize();
    reCycle_ = true;
    reDraw_ = true;

    return p_Main->setI8275RegisterNibble(I8275_LINES_ROW, (Byte)i8275Configuration_.charSize.y, showTrace);
}

int i8275::setCursorFormat(Byte value, int showTrace)
{
    cursorBlinking_ = (((value & 0x2) >> 1) == 0);
    cursorBlinkOn_ = true;
    cursorLine_ = ((value & 0x1) == 1);
    showTrace = p_Main->setI8275SelectorValue(I8275_PARAMETER_CC, value & 0x3, showTrace);

    return p_Main->setI8275RegisterNibble(I8275_CURSOR_FORMAT, value & 0x3, showTrace);
}

int i8275::setHorizontalRetrace(Byte value, int showTrace)
{
    horizontalRetraceCount_ = ((value & 0x0f) + 1)*2;
    setScreenSize();
    reCycle_ = true;
    reDraw_ = true;

    return p_Main->setI8275Register(I8275_HORIZONTAL_RETRACE, (Byte)horizontalRetraceCount_, showTrace);
}

int i8275::setCursorChar(Byte value, int showTrace)
{
    cursorCharPosition_ = value;
    cursorAddress_ = cursorCharPosition_ + (cursorRowPosition_*80);

    return p_Main->setI8275Register(I8275_CURSOR_CHAR, (Byte)cursorCharPosition_, showTrace);
}

int i8275::setCursorRow(Byte value, int showTrace)
{
    cursorRowPosition_ = value;
    cursorAddress_ = cursorCharPosition_ + (cursorRowPosition_*80);

    return p_Main->setI8275Register(I8275_CURSOR_ROW, (Byte)cursorRowPosition_, showTrace);
}

void i8275::cRegWrite(Byte value, int showTrace)
{
    if (parameters_ != 0)
        status_ |= 0x8;

/*    if (value != 0x80)
    {
    p_Main->message("8275 Register_");
    p_Main->messageInt(value);
    }*/
    parameters_ = 0;
    command_ = C_NONE;

    showTrace = p_Main->setI8275Register(I8275_COMMAND, value, showTrace);

    if (value == 0) // RESET
    {
        command_ = C_RESET;
        parameters_ = 4;
        status_ &= 0xbf;
    }

    if (value == 0x40) // STOP_DISPLAY
    {
        displayOn_ = false;
        cycleValue8275_ = -1;
        dmaCycleValue8275_ = -1;
        status_ &= 0xfb;

        setColour(colourIndex_+backGround_);
        drawRectangle(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
        showTrace = p_Main->setI8275SelectorValue(I8275_DISPLAY, false, showTrace);
//        reBlit_ = true;
//        copyScreen();
    }

    if (value == 0x60) // LIGHT_PEN
    {
        command_ = C_LIGHT_PEN;
        parameters_ = 2;
    }

    if (value == 0x80) // LOAD_CURSOR
    {
        command_ = C_LOAD_CURSOR;
        parameters_ = 2;
    }

    if (value == 0xe0) // PRESET_COUNTERS
    {
        bufferLocation_ = 0;
        screenLocation_ = 0;
    }

    if ((value & 0xf0) == 0x20)  // START DISPLAY
    {
        displayOn_ = true;
        showTrace = setBurstSpaceCode(value >> 2, showTrace);
        showTrace = setDmaCyclesBurst(value, showTrace);
        bufferLocation_ = 0;
        screenLocation_ = 0;
        reCycle_ = true;
        cycleValue8275_ = rowCycleSize8275_;
//        p_Main->startTime();
        status_ |= 0x44;
        showTrace = p_Main->setI8275SelectorValue(I8275_DISPLAY, true, showTrace);
    }

    if ((value & 0xf0) == 0xA0)  // START INTERRUPT
    {
        interruptOn_ = true;
        showTrace = p_Main->setI8275SelectorValue(I8275_INTERRUPT, true, showTrace);
    }
    
    if ((value & 0xf0) == 0xC0)  // STOP INTERRUPT
    {
        interruptOn_ = false;
        showTrace = p_Main->setI8275SelectorValue(I8275_INTERRUPT, false, showTrace);
    }

    p_Main->setI8275Register(I8275_STATUS, status_, DO_NOT_SHOW_ANY_TRACE);
}

int i8275::setBurstSpaceCode(Byte value, int showTrace)
{
    burstSpaceCode_ = SSS[value & 07];
    reCycle_ = true;
    
    return p_Main->setI8275Register(I8275_BURST_SPACE_CODE, (Byte)burstSpaceCode_, showTrace);
}

int i8275::setDmaCyclesBurst(Byte value, int showTrace)
{
    burstCountCode_ = BB[(value & 0x3)];
    reCycle_ = true;

    return p_Main->setI8275RegisterNibble(I8275_DMA_CYCLES_BURST, (Byte)burstCountCode_, showTrace);
}

Byte i8275::pRegRead()
{
    p_Main->setI8275Register(I8275_READ_PARAMETER, (Byte)0, SHOW_ADDRESS_TRACE);
    return 0;
}

Byte i8275::sRegRead()
{
    dmaOn_ = true;
    p_Computer->requestInterrupt(INTERRUPT_TYPE_I8275_1, false, i8275Configuration_.picInterrupt);

    Byte ret = status_;
    status_ &= 0xc4;

    frameEf_ = 1;
    
    p_Main->setI8275Register(I8275_STATUS, ret);
    p_Main->setI8275Register(I8275_STATUS, status_, DO_NOT_SHOW_ANY_TRACE);
    return ret;
}

void i8275::cycle8275()
{
    if (!displayOn_)
    {
        cycleBlankValue8275_--;
        if (cycleBlankValue8275_ <= 0)
        {
            cycleBlankValue8275_ = fullCycleSize8275_;
            if (changeScreenSize_)
                changeScreenSize();

            reBlit_ = true;
            reDraw_ = false;

            setColour(colourIndex_+backGround_);
            drawRectangle(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);

            copyScreen();
            videoSyncCount_++;
            dmaCycleValue8275_ = -1;
            reDraw_ = true;
        }
        return;
    }
    cycleValue8275_--;
    if (cycleValue8275_ <= 0)
    {
        if (retrace_)
        {
            dmaCycleValue8275_ = dmaCycleSize8275_;
            bufferLocation_ = 0;
            retrace_ = false;
            cycleValue8275_ = rowCycleSize8275_;
        }
        else
        {
            if (dmaCycleValue8275_ > 0)
            {
                dmaCycleValue8275_ = -1;
                screenLocation_ = screenLocation_ + i8275Configuration_.screenSize.x - bufferLocation_;
                status_ |= 0x2;
                p_Main->setI8275Register(I8275_STATUS, status_, DO_NOT_SHOW_ANY_TRACE);
            }
            retrace_ = true;

            cycleValue8275_ = horizontalRetraceCycleSize8275_;
            if (screenLocation_ == (i8275Configuration_.screenSize.y*i8275Configuration_.screenSize.x))
            {
                attributeChange_ = false;
                reverse_ = false;
                underline_ = false;
                highlight_ = false;
                gpa_ = 0;
                blink_ = false;
                cycleValue8275_ = verticalRetraceCycleSize8275_;
                if (interruptOn_)
                    p_Computer->requestInterrupt(INTERRUPT_TYPE_I8275_1, true, i8275Configuration_.picInterrupt);
                frameEf_ = 0;
                screenLocation_ = 0;
                blinkCount_--;
                if (blinkCount_ <= 0)
                {
                    blinkCount_ = 16;
                    blinkOn_ = !blinkOn_;
                    reBlink_ = true;
                    if (cursorBlinking_)
                        cursorBlinkOn_ = !cursorBlinkOn_;
                }
                if (blinkCount_ == 8)
                    if (cursorBlinking_)
                    {
                        cursorBlinkOn_ = !cursorBlinkOn_;
                    }
                if (changeScreenSize_)
                    changeScreenSize();
                copyScreen();
                videoSyncCount_++;
            }
        }
    }

    if (!dmaOn_ || dmaCycleValue8275_ < 0)
        return;
//    if (p_Computer->getProgramCounter() == 0)
//    {
//        p_Main->eventShowTextMessage("DMA request while R0 is Program Counter");
//        return;
//    }
    dmaCycleValue8275_--;
    if (dmaCycleValue8275_ <= 0)
    {
        int i=1;
        while (((i <= burstCountCode_) || (dmaCycleSize8275_ == 0)) && (bufferLocation_ != i8275Configuration_.screenSize.x))
        {
            Byte value = p_Computer->pixieDmaOut();
            cycleValue8275_--;
            graphicLine_ = false;
            if ((value & 0xc0) == 0x80)
            {
                underline_ = ((value &0x20) == 0x20);
                reverse_ = ((value &0x10) == 0x10);
                blink_ = ((value &0x2) == 0x2);
                if (i8275Configuration_.gpaSwitched)
                    gpa_ = (value &0x8) << 8 | (value &0x4) << 10;
                else
                    gpa_ = (value &0xc) << 9;
                highlight_ = ((value &0x1) == 0x1);
            }
            if ((value & 0xc0) == 0xc0)
            {
                Byte attribute = (value &0x3c) >> 2;
                if (attribute <= 10)
                {
                    blink_ = ((value &0x2) == 0x2);
                    highlight_ = ((value &0x1) == 0x1);
                    graphicLine_ = true;
                }
            }
            if ((i8275ram_[screenLocation_&0x1FFF] != value) || (highlightScr_[screenLocation_&0x1FFF] != highlight_) || (gpaScr_[screenLocation_&0x1FFF] != gpa_) || (blinkScr_[screenLocation_&0x1FFF] != blink_) || (reverseScr_[screenLocation_&0x1FFF] != reverse_) || (underlineScr_[screenLocation_&0x1FFF] != underline_) || (graphicLineScr_[screenLocation_&0x1FFF] != graphicLine_) || (screenLocation_ == cursorAddress_) || cursorReset_[screenLocation_&0x1FFF])
            {
            /*    if ((value & 0xc0) == 0x80)
                {
                    attributeChange_ = true;
                    reDraw_ = true;
                }*/
                underlineScr_[screenLocation_&0x1FFF] = underline_;
                reverseScr_[screenLocation_&0x1FFF] = reverse_;
                blinkScr_[screenLocation_&0x1FFF] = blink_;
                gpaScr_[screenLocation_&0x1FFF] = gpa_;
                highlightScr_[screenLocation_&0x1FFF] = highlight_;
                graphicLineScr_[screenLocation_&0x1FFF] = graphicLine_;
                draw8275(screenLocation_, value);
            }

            i++;
            bufferLocation_++;
            screenLocation_++;
        }
        if (bufferLocation_ == i8275Configuration_.screenSize.x)
        {
            if ((status_ & 0x40) == 0x40)
            {
                status_ |= 0x20;
                p_Main->setI8275Register(I8275_STATUS, status_, DO_NOT_SHOW_ANY_TRACE);
                if (interruptOn_)
                    p_Computer->requestInterrupt(INTERRUPT_TYPE_I8275_4, true, i8275Configuration_.picInterruptHorizontal);
                rowEf_ = 0;
            }
            dmaCycleValue8275_ = -1;
        }
        else
            dmaCycleValue8275_ = dmaCycleSize8275_;
    }
}

void i8275::setClock(double clock)
{
    clock_ = clock;
    reCycle_ = true;
}

void i8275::setCycle()
{
    float clockPeriod = (float)1/i8275Configuration_.videoClock * i8275Configuration_.charSize.x;

    float characterCycleSize8275_ = (float)(clockPeriod * i8275Configuration_.charSize.y / ((1/clock_) * 8));
    horizontalRetraceCycleSize8275_ = (int) (characterCycleSize8275_ * horizontalRetraceCount_);
    float fullRowCycleSize8275_ = (float) (characterCycleSize8275_ * (i8275Configuration_.screenSize.x + horizontalRetraceCount_));
    rowCycleSize8275_ = (int) (characterCycleSize8275_ * i8275Configuration_.screenSize.x);
//    verticalRetraceCycleSize8275_ = (int) (fullRowCycleSize8275_ * verticalRetraceRowCount_);
    fullCycleSize8275_ = (int) (fullRowCycleSize8275_ * (i8275Configuration_.screenSize.y + verticalRetraceRowCount_));
    verticalRetraceCycleSize8275_ = (int) (fullCycleSize8275_ - (i8275Configuration_.screenSize.y * (horizontalRetraceCycleSize8275_ + rowCycleSize8275_)));
    dmaCycleSize8275_ = (float)(clockPeriod * burstSpaceCode_ / ((1/clock_) * 8));
       // dmaCycleSize8275_ = (int) (characterCycleSize8275_ * burstSpaceCode_); This is too slow, above might be more correct?

    reCycle_ = false;
}

Byte i8275::read8275CharRom(Word addr)
{
    return i8275CharRom_[addr&0x1FFF];
}

void i8275::write8275CharRom(Word addr, Byte value)
{
    addr &= 0x1FFF;
    i8275CharRom_[addr] = value;
    Word memoryStart = p_Computer->getDebugMemoryStart();
    if (addr >= memoryStart && addr<(memoryStart + 256))
        p_Main->updateDebugMemory(addr);
    reDraw_ = true;
}

Byte i8275::read8275VideoRam(Word addr)
{
    return i8275ram_[addr&0x1FFF];
}

void i8275::write8275VideoRam(Word addr, Byte value)
{
    addr &= 0x1FFF;
    i8275ram_[addr] = value;
    Word memoryStart = p_Computer->getDebugMemoryStart();
    if (addr >= memoryStart && addr<(memoryStart + 256))
        p_Main->updateDebugMemory(addr);
    reDraw_ = true;
}

void i8275::copyScreen()
{
    if (p_Main->isZoomEventOngoing())
        return;

    if (reColour_)
    {
        for (int i=0; i<numberOfColours_; i++)
        {
            colour_[i] = colourNew_[i];
            brushColour_[i] = brushColourNew_[i];
            penColour_[i] = penColourNew_[i];
        }
        for (int i=0; i<VIDEOXMLMAX; i++)
        {
            borderX_[i] = borderXNew_[i];
            borderY_[i] = borderYNew_[i];
        }
        setScreenSize();
        reDraw_ = true;
        reBlit_ = true;
        newBackGround_ = true;
        reColour_ = false;
    }

    if (reCycle_)
        setCycle();

    bool reBlitAfterReDraw = false;
    if (reDraw_)
    {
        drawScreen();
        reDraw_ = false;
        reBlitAfterReDraw = true;
    }

    if (reBlink_)
        blinkScreen8275();

#if defined(__WXMAC__)
    if (reBlit_ || reBlitAfterReDraw || reBlink_)
    {
        flushFramebufferMac();
        p_Main->eventRefreshVideo(false, videoNumber_);
        reBlit_ = false;
        reBlink_ = false;
    }
#else
    if (extraBackGround_ && newBackGround_)
        drawExtraBackground(colour_[colourIndex_+backGround_]);

    CharacterList8275 *temp;

    if (reBlit_ || reBlitAfterReDraw || reBlink_)
    {
        videoScreenPointer->blit(0, 0, videoWidth_+2*offsetX_, i8275Configuration_.screenSize.y*i8275Configuration_.charSize.y*videoM_+2*offsetY_, &dcMemory, 0, 0);
        reBlit_ = false;
        reBlink_ = false;
        if (updateCharacter8275_ > 0)
        {
            updateCharacter8275_ = 0;
            while(characterListPointer8275 != NULL)
            {
                temp = characterListPointer8275;
                characterListPointer8275 = temp->nextCharacter;
                delete temp;
            }
        }
    }
    if (updateCharacter8275_ > 0)
    {
        updateCharacter8275_ = 0;
        while(characterListPointer8275 != NULL)
        {
            videoScreenPointer->blit(offsetX_+(characterListPointer8275->x), offsetY_+(characterListPointer8275->y), i8275Configuration_.charSize.x, i8275Configuration_.charSize.y*videoM_, &dcMemory, offsetX_+characterListPointer8275->x, offsetY_+characterListPointer8275->y);
            temp = characterListPointer8275;
            characterListPointer8275 = temp->nextCharacter;
            delete temp;
        }
    }
#endif
}

void i8275::drawScreen()
{
    int addr = 0;

    setColour(colourIndex_+backGround_);
    drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);

    for (int i=0; i<(i8275Configuration_.screenSize.x*i8275Configuration_.screenSize.y); i++)
    {
        draw8275(addr, i8275ram_[addr&0x1FFF]);

        addr++;
        addr &= 0x7ff;
    }
}

void i8275::blinkScreen8275()
{
    int addr = 0;
    for (int i=0; i<(i8275Configuration_.screenSize.x*i8275Configuration_.screenSize.y); i++)
    {
        if (blinkScr_[addr&0x1FFF])
            draw8275(addr, i8275ram_[addr&0x1FFF]);
        addr++;
        addr &= 0x7ff;
    }
}

void i8275::draw8275(Word addr, Byte value)
{
    addr &= 0x1FFF;
    i8275ram_[addr] = value;
    Word memoryStart = p_Computer->getDebugMemoryStart();
    if (addr >= memoryStart && addr<(memoryStart + 256))
        p_Main->updateDebugMemory(addr);
    cursorReset_[addr] = (addr == cursorAddress_);

    int y = (addr/i8275Configuration_.screenSize.x)*i8275Configuration_.charSize.y*videoM_;
    int x = (addr%i8275Configuration_.screenSize.x)*i8275Configuration_.charSize.x;
    drawCharacter8275(x, y, value, cursorReset_[addr], addr);
}

void i8275::drawCharacter8275(wxCoord x, wxCoord y, Byte v, bool cursor, Word addr)
{
    int line_byte, line, drawLine;
    Byte graphicLineCodes[] =
    {
        0x3, 0x6, 0x9, 0xc, 0x7, 0xe, 0xb, 0xd, 0x5, 0xa, 0xf, 0xf
    };

#if defined(__WXMAC__) || defined(__linux__)
    reBlit_ = true;
#else
    if (zoomFraction_)
        reBlit_ = true;
    else
    {
        CharacterList8275 *temp = new CharacterList8275;
        temp->x = x;
        temp->y = y;
        temp->nextCharacter = characterListPointer8275;
        characterListPointer8275 = temp;
        updateCharacter8275_++;
        if (updateCharacter8275_ > 40)
            reBlit_ = true;
    }
#endif

    if ((v & 0xc0) == 0x80)
    {
    /*    underline_ = ((v &0x20) == 0x20);
        reverse_ = ((v &0x10) == 0x10);
        blink_ = ((v &0x2) == 0x2);
        highlight_ = ((v &0x1) == 0x1);
        gpa_ = (v &0xc) << 9;*/

        setColour(colourIndex_+backGround_);
        if ((cursor && cursorBlinkOn_) && !cursorLine_)
        {
            setColour(colourIndex_+FOREGROUND8275);
        }
        drawRectangle(x+offsetX_, y+offsetY_, i8275Configuration_.charSize.x, i8275Configuration_.charSize.y*videoM_);
        if ((cursor && cursorBlinkOn_) && cursorLine_)
        {
            setColour(colourIndex_+FOREGROUND8275);
            drawRectangle(x+offsetX_, y+offsetY_+(underLinePlacement_-1), i8275Configuration_.charSize.x, videoM_);
        }

        return;
    }

    if ((reverseScr_[addr&0x1FFF] && !((cursor && cursorBlinkOn_) && !cursorLine_)) || ((cursor && cursorBlinkOn_) && !cursorLine_ && !reverseScr_[addr&0x1FFF]))
    {
        if (highlightScr_[addr&0x1FFF])
        {
            setColour(colourIndex_+HIGHLIGHT8275);
        }
        else
        {
            setColour(colourIndex_+FOREGROUND8275);
        }
        drawRectangle(x+offsetX_, y+offsetY_, i8275Configuration_.charSize.x, i8275Configuration_.charSize.y*videoM_);

        setColour(colourIndex_+backGround_);
    }
    else
    {
        setColour(colourIndex_+backGround_);
        drawRectangle(x+offsetX_, y+offsetY_, i8275Configuration_.charSize.x, i8275Configuration_.charSize.y*videoM_);

        if (highlightScr_[addr&0x1FFF])
        {
            setColour(colourIndex_+HIGHLIGHT8275);
        }
        else
        {
            setColour(colourIndex_+FOREGROUND8275);
        }
    }

    if (blinkScr_[addr&0x1FFF] && blinkOn_)
    {
        return;
    }

    if (graphicLineScr_[addr&0x1FFF])
    {
        Byte attribute = (v &0x3c) >> 2;
        if ((graphicLineCodes[attribute&0x1FFF] & 1) == 1)
        {
            for (wxCoord i=x+(i8275Configuration_.charSize.x/2); i<x+i8275Configuration_.charSize.x; i++)
            {
                if (interlace_ & !(videoM_ == 1))
                    drawRectangle(i+offsetX_, y+underLinePlacement_+offsetY_, 1, videoM_);
                else
                    drawPoint(i+offsetX_, y+underLinePlacement_+offsetY_);
            }
        }
        if ((graphicLineCodes[attribute&0x1FFF] & 2) == 2)
        {
            for (wxCoord i=y+underLinePlacement_; i<y+i8275Configuration_.charSize.y*videoM_; i+=videoM_)
            {
                if (interlace_ & !(videoM_ == 1))
                    drawRectangle(x+(i8275Configuration_.charSize.x/2)+offsetX_, i+offsetY_, 1, videoM_);
                else
                    drawPoint(x+(i8275Configuration_.charSize.x/2)+offsetX_, i+offsetY_);
            }
        }
        if ((graphicLineCodes[attribute&0x1FFF] & 4) == 4)
        {
            for (wxCoord i=x; i<x+(i8275Configuration_.charSize.x/2); i++)
            {
                if (interlace_ & !(videoM_ == 1))
                    drawRectangle(i+offsetX_, y+underLinePlacement_+offsetY_, 1, videoM_);
                else
                    drawPoint(i+offsetX_, y+underLinePlacement_+offsetY_);
            }
        }
        if ((graphicLineCodes[attribute&0x1FFF] & 8) == 8)
        {
            for (wxCoord i=y; i<y+underLinePlacement_; i+=videoM_)
            {
                if (interlace_ & !(videoM_ == 1))
                    drawRectangle(x+(i8275Configuration_.charSize.x/2)+offsetX_, i+offsetY_, 1, videoM_);
                else
                    drawPoint(x+(i8275Configuration_.charSize.x/2)+offsetX_, i+offsetY_);
            }
        }
        return;
    }
    
    drawLine = 0;
    if (lineCounterMode_ == 0)
        line = 0;
    else
        line = i8275Configuration_.charSize.y-1;

    for (wxCoord j=y; j<y+(i8275Configuration_.charSize.y*videoM_); j+=videoM_)
    {
        if ((underLinePlacement_ > 7) && ((drawLine == 0) || (drawLine == (i8275Configuration_.charSize.y-1))))
            line_byte = 0;
        else
        {
            if (line >= 8)
                line_byte = i8275CharRom_[(v*8+line-8+0x400+gpaScr_[addr&0x1FFF])&0x1FFF];
            else
                line_byte = i8275CharRom_[(v*8+line+gpaScr_[addr&0x1FFF])&0x1FFF];
        }

        for (wxCoord i=x; i<x+i8275Configuration_.charSize.x; i++)
        {
            if ((line_byte & 1) || ((drawLine == underLinePlacement_) && ((underlineScr_[addr&0x1FFF] && !((cursor && cursorBlinkOn_) && cursorLine_)) || ((cursor && cursorBlinkOn_) && cursorLine_ && !underlineScr_[addr&0x1FFF]))))
            {
                if (interlace_ & !(videoM_ == 1))
                    drawRectangle(i+offsetX_, j+offsetY_, 1, videoM_);
                else
                    drawPoint(i+offsetX_, j+offsetY_);
            }
            line_byte >>= 1;
        }
        drawLine++;
        line++;
        if (line >= i8275Configuration_.charSize.y)
            line = 0;
    }
}

void i8275::setInterlace(bool status)
{
    interlace_ = status;
    reDraw_ = true;
}

bool i8275::readCharRomFile(wxString romDir, wxString romFile)
{
    wxFFile inFile;
    size_t length, number;
    char buffer[8192];

    for (size_t i=0; i<8192; i++)
    {
        i8275CharRom_[i] = 0;
    }

    if (romFile == "")
    {
        p_Main->errorMessage("No font filename specified");
        return false;
    }

    wxString fileName = romDir + romFile;

    if (!wxFile::Exists(fileName))
    {
        p_Main->errorMessage("File " + fileName + " not found");
        return false;
    }

    if (inFile.Open(fileName, _("rb")))
    {
        length = inFile.Read(buffer, 8192);
        number = 0;
        for (size_t i=0; i<length; i++)
        {
            i8275CharRom_[i&0x1FFF] = (Byte)buffer[i];
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

void i8275::setFullScreen(bool fullScreenSet)
{
    fullScreenSet_ = fullScreenSet;
    ShowFullScreen(fullScreenSet);
}

void i8275::onF3()
{
    fullScreenSet_ = !fullScreenSet_;
    p_Main->eventVideoSetFullScreen(fullScreenSet_, videoNumber_);
}


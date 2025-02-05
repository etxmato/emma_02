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
 f *******************************************************************
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

#include <wx/clipbrd.h>
#include "main.h"
#include "vt100.h"

#define XMODEM_ACK 6
#define XMODEM_NAK 21
#define XMODEM_CRC 67

#define YMODEM_HEADER 1
#define YMODEM_DATA 2
#define XMODEM_DATA 3
#define YMODEM_END_FRAME 4

#define UART_MCR_DTR 0
#define UART_MCR_RTS 1
#define UART_MCR_OUT1 2
#define UART_MCR_OUT2 3
#define UART_MCR_LOOP 4

#define UART_MSR_CTSD 0
#define UART_MSR_DSRD 1
#define UART_MSR_RID 2
#define UART_MSR_CDD 3
#define UART_MSR_CTS 4
#define UART_MSR_DSR 5
#define UART_MSR_RI 6
#define UART_MSR_CD 7

#define UART_LSR_DR 0
#define UART_LSR_OE 1
#define UART_LSR_PE 2
#define UART_LSR_FE 3
#define UART_LSR_BI 4
#define UART_LSR_THRE 5
#define UART_LSR_TRE 6
#define UART_LSR_FIFOE 7

int baudRateValue_[] =
{
    38400, 19200, 9600, 4800, 3600, 2400, 2000, 1800, 1200, 600, 300, 200, 150, 134, 110, 75, 50
};

int baudRateCode_[] =
{
    128, 120, 114, 104, 96, 88, 80, 72, 64, 56, 48, 40, 32, 24, 16, 8, 0
};

int baudRateFactor_[] =
{
    6,     13,    26,   52,   69,   120,  138,  153,  230,  460, 900, 1250, 1666, 1866, 2273, 3333, 5000
//    38400, 19200, 9600, 4800, 3600, 2400, 2000, 1800, 1200, 600, 300, 200,  150,  134,  110,  75,   50
};

Vt100::Vt100(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, ComputerConfiguration computerConfig, int uartNumber)
: Video(title, pos, size)
{
    currentComputerConfiguration = computerConfig;
    vtType_ = currentComputerConfiguration.videoTerminalConfiguration.type;
    clock_ = clock;
    uartNumber_ = uartNumber;
    
    videoScreenPointer = new VideoScreen(this, size, zoom, true, uartNumber);
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif
    line_ = "";

    colourIndex_ = 2;
    videoType_ = VIDEOVT;
    uartEf_ = false;
    uartControl_ = 0;
    uartStatus_ = 0x10;

    colourIndex_ = COL_VT_FORE;
    
    if (currentComputerConfiguration.videoTerminalConfiguration.show)
        readCharRomFile(p_Main->getVtCharRomDir(), p_Main->getVtCharRomFile());

    stretchDot_ = currentComputerConfiguration.videoTerminalConfiguration.stretchDot;
    serialLog_ = currentComputerConfiguration.videoTerminalConfiguration.serialLog;
    uart1854_ = currentComputerConfiguration.videoTerminalConfiguration.uart1854_defined;
    uart16450_ = currentComputerConfiguration.videoTerminalConfiguration.uart16450_defined;

    fullScreenSet_ = false;
    zoom_ = zoom;

    double intPart;
    zoomFraction_ = (modf(zoom_, &intPart) != 0);

    intensity_ = 1;
    
    defineColours();

    redFore_ = colour_[colourIndex_].Red();
    greenFore_ = colour_[colourIndex_].Green();
    blueFore_ = colour_[colourIndex_].Blue();
    redBack_ = colour_[colourIndex_+1].Red();
    greenBack_ = colour_[colourIndex_+1].Green();
    blueBack_ = colour_[colourIndex_+1].Blue();

    setForeColour();
    

    switch (vtType_)
    {
        case VT52:
            if (currentComputerConfiguration.videoTerminalConfiguration.charactersPerRow == 64)
                charactersPerRow_ = currentComputerConfiguration.videoTerminalConfiguration.charactersPerRow;
            else
                charactersPerRow_ = 80;
            SetUpFeature_ = currentComputerConfiguration.videoTerminalConfiguration.vt52SetUpFeature;
            tabs_ = "        T       T       T       T       T       T       T       T       T       T       T       T       T       T       T       T       T";
            charWidth_ = currentComputerConfiguration.videoTerminalConfiguration.characterWidth;
            rowsPerFrame_ = 24;
            linesPerCharacter_ = 11;
            heightFactor = 2;
            scrollStart_ = 0;
            scrollEnd_ = rowsPerFrame_;
        break;
        case VT100:
            SetUpFeature_ = currentComputerConfiguration.videoTerminalConfiguration.vt100SetUpFeature;
            tabs_ = p_Main->getConfigItem("Computer/VT100Tabs", "        T       T       T       T       T       T       T       T       T       T       T       T       T       T       T       T       T");
            answerBackMessage_ = p_Main->getConfigItem("Computer/VT100AnswerBack", "                    ");
            charactersPerRow_ = currentComputerConfiguration.videoTerminalConfiguration.charactersPerRow;
            charWidth_ = currentComputerConfiguration.videoTerminalConfiguration.characterWidth;
            rowsPerFrame_ = 25;
            linesPerCharacter_ = 10;
            heightFactor = 2;
            scrollStart_ = 0;
            scrollEnd_ = rowsPerFrame_;
        break;
/*        case VT220:
            charactersPerRow_ = 80;
            charWidth_ = 8;
            rowsPerFrame_ = 25;
            linesPerCharacter_ = 16;
            heightFactor = 1;
        break;*/
    }
    videoWidth_ = charactersPerRow_*charWidth_;
    videoHeight_ = rowsPerFrame_*linesPerCharacter_*heightFactor;

    if (wxGetKeyState(WXK_SCROLL))
        scroll_ = 0;
    else
        scroll_ = -1;

    setCycle();
    cycleBell_ = 0;
    cycleClick_ = 0;

    initScreen();
    videoScreenPointer->setRepeat(SetUpFeature_[VTREPEAT]);

    screenCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    dcMemory.SelectObject(*screenCopyPointer);

#if defined(__WXMAC__)
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

    pressedKey_ = 0;
    displayStart_ = 0;
    displayEnd_ = 0;
    escPosition_ = -1;
    cursorLine_ = 0;
    cursorPosition_ = 0;
    saveLine_ = 0;
    savePosition_ = 0;
    blinkCount_ = 30;
    savedCursorLine_ = 0;
    savedCursorPosition_ = 0;
    setUpMode_ = false;
    setUpModeB_ = false;
    answerBack_ = false;
    wait_ = 0;
    cursorBlinkOn_ = true;
    xOff_ = false;
    ctrlQpressed_ = false;
    smoothScroll_ = false;
    scrolling_ = 10;
    ShowCursor();

    highlight_ = false;
    underline_ = false;
    blink_ = false;
    blinkOn_ = false;
    reverse_ = false;

    // Modes
    AltKeypad_ = false;
    graphicsMode_ = false;
    scsMode_ = false;
    g0mode_ = 'A';
    g1mode_ = '0';
    cursorKeyMode_ = true;
    originMode_ = false;
    terminalSave_ = false;
    terminalLoad_ = false;
    sendPacket_ = false;
    terminalFileCdp18s020_ = false;
    terminalInputFileLine_ = "";
    clearToSend_ = true;

    if (vtType_ != EXTERNAL_TERMINAL && vtType_ != LOOP_BACK)
    {
        this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);
        this->SetBackgroundColour(colour_[colourIndex_+1]);
    }
    characterListPointer = NULL;
    offsetX_ = 0;
    offsetY_ = 0;
    reDraw_ = true;
    reBlit_ = false;
    reBlink_ = false;
    newBackGround_ = false;
    updateCharacter_ = false;
    tab_char = 8;

    if (serialLog_)
    {
        int num = 0;
        wxString number;
        wxString fileName = p_Main->getDataDir() + p_Main->getPathSep() + "terminal.log";
        while(wxFile::Exists(fileName))
        {
            num++;
            number.Printf("%d", num);
            fileName = p_Main->getDataDir() + p_Main->getPathSep() + "terminal." + number + ".log";
        }
        logFile_.Create(fileName);
    }
    serialOpen_ = false;
    
    uart_da_bit_ = 0;
    uart_fe_bit_ = 3;
    uart_tsre_bit_ = 6;
    uart_thre_bit_ = 7;
}

Vt100::~Vt100()
{
    if (serialLog_)
    {
        if (line_ != "")
            logFile_.AddLine(line_);
        logFile_.Write();
    }
    CharacterList *temp;
    dcMemory.SelectObject(wxNullBitmap);
    dcScroll.SelectObject(wxNullBitmap);
    delete screenCopyPointer;
    delete videoScreenPointer;
#if defined(__WXMAC__)
    delete gc;
#endif
    if (updateCharacter_)
    {
        while(characterListPointer != NULL)
        {
            temp = characterListPointer;
            characterListPointer = temp->nextCharacter;
            delete temp;
        }
    }
}

void Vt100::configure(VideoTerminalConfiguration videoTerminalConfiguration, AddressLocationConfiguration addressLocationConfiguration, wxString saveCommand)
{
    addressLocations_ = addressLocationConfiguration;
    saveCommand_ = saveCommand;
    
    wxString printBuffer;
    
    selectedBaudT_ = videoTerminalConfiguration.baudT;
    selectedBaudR_ = videoTerminalConfiguration.baudR;
    
    baudRateT_ = (int) ((((clock_ * 1000000) / 8) / baudRateValue_[selectedBaudT_])+videoTerminalConfiguration.baudCorrectionT);
    baudRateR_ = (int) ((((clock_ * 1000000) / 8) / baudRateValue_[selectedBaudR_])+videoTerminalConfiguration.baudCorrectionR);

    reverseEf_ = videoTerminalConfiguration.ef.reverse^1;

    if (uart1854_)
        configureUart1854(videoTerminalConfiguration);
    else
    {
        if (uart16450_)
            configureUart16450(videoTerminalConfiguration);
        else
        {
            reverseQ_ = videoTerminalConfiguration.reverseQ^1;
            
            dataReadyFlag_ = videoTerminalConfiguration.ef.flagNumber;

            if (reverseQ_) p_Computer->setFlipFlopQ(1);
            
            if (vtType_ == VT52)
                p_Main->configureMessage(&videoTerminalConfiguration.ioGroupVector, "VT52 terminal");
            else
                p_Main->configureMessage(&videoTerminalConfiguration.ioGroupVector, "VT100 terminal");
          
            printBuffer = "	Serial out: Q";
            if (reverseQ_ == 1)
                printBuffer = "	Serial out: reversed Q";
            p_Main->message(printBuffer);

            p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.output, VIDEO_TERMINAL_OUT, "vtEnable");
            p_Computer->setEfType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.ef, VIDEO_TERMINAL_EF, "serial input");
            p_Computer->setCycleType(CYCLE_TYPE_VIDEO_TERMINAL, VIDEO_TERMINAL_CYCLE);
        }
    }
    
    printBuffer.Printf("	Transmit baud rate: %d, receive baud rate: %d\n", baudRateValue_[selectedBaudT_], baudRateValue_[selectedBaudR_]);
    p_Main->message(printBuffer);
    
    vtEnabled_ = 1;
    vtCount_ = -1;
    vtOutCount_ = -1;
    vtOut_ = 0;
    vt100Ef_ = 1;
    elfRunCommand_ = 0;
    ctrlvText_ = 0;
}

void Vt100::configureUart1854(VideoTerminalConfiguration videoTerminalConfiguration)
{    
    if (vtType_ == VT52)
        p_Main->configureMessage(&videoTerminalConfiguration.ioGroupVector, "VT52 terminal with CDP1854/UART");
    else
        p_Main->configureMessage(&videoTerminalConfiguration.ioGroupVector, "VT100 terminal with CDP1854/UART");
    
    p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartOut, VT_UART1854_LOAD_TRANSMITTER_OUT, "load transmitter");
    p_Computer->setInType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartIn, VT_UART1854_READ_RECEIVER_IN, "read receiver");
    p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartControl, VT_UART1854_LOAD_CONTROL_OUT, "load control");
    p_Computer->setInType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartStatus, VT_UART1854_READ_STATUS_IN, "read status");
    if (currentComputerConfiguration.videoTerminalConfiguration.efInterrupt.flagNumber != -1)
        p_Computer->setEfType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.efInterrupt, VIDEO_TERMINAL_EF_INTERRUPT, "UART interrupt");
    if (currentComputerConfiguration.videoTerminalConfiguration.ef.flagNumber != -1)
    {
        p_Computer->setEfType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.ef, VIDEO_TERMINAL_EF, "serial input");
        uartEf_ = true;
    }
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_TERMINAL, VIDEO_TERMINAL_CYCLE);

    rs232_ = 0;
}

void Vt100::configureUart16450(VideoTerminalConfiguration videoTerminalConfiguration)
{
    uart16450_ = true;

    if (vtType_ == VT52)
        p_Main->message("Configuring VT52 terminal with 16450/550 UART");
    else
        p_Main->message("Configuring VT100 terminal with 16450/550 UART");

    p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartOut, UART16450_WRITE_REGISTER_OUT, "write selected");
    p_Computer->setOutType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartControl, UART16450_CONTROL_OUT, "register select");
    p_Computer->setInType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartIn, UART16450_READ_REGISTER_IN, "read selected");
    p_Computer->setInType(&videoTerminalConfiguration.ioGroupVector, videoTerminalConfiguration.uartStatus, UART16450_READ_STATUS_IN, "read status");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_TERMINAL, VIDEO_TERMINAL_CYCLE);

    rs232_ = 0;
    registerSelect_ = 0;
    modemControlRegister_ = 0;
    modemStatusRegister_ = 0;
    lineStatusRegister_ = 0xe0;
}

void Vt100::setTabChar(Byte value)
{
    tab_char = value;
}

Byte Vt100::ef()
{
    return (reverseEf_^vt100Ef_);
}

Byte Vt100::efInterrupt()
{
    return (currentComputerConfiguration.videoTerminalConfiguration.efInterrupt.reverse^vt100EfInterrupt_);
}

void Vt100::out(Byte value)
{
    if (value == 0x40) vtEnabled_ = 0;
    if (value == 0x80) vtEnabled_ = 1;
}

void Vt100::cycleVt()
{
    cycleValue_--;
    if (cycleValue_ <= 0)
    {
        if (!setUpMode_)
            getKey();
        else
        {
            if (pressedKey_ != 0)
            {
                if (!setUpModeB_)
                    setUpA(pressedKey_);
                else
                    setUpB(pressedKey_);
            }
            if (wait_ > 0)
            {
                wait_--;
                if (wait_ == 0)
                {
                    setUpModeB_ = false;
                    setupAScreen();
                    reDraw_ = true;
                }
            }
        }

        scrolling_--;
        if (scrolling_ < 0 || !SetUpFeature_[VTSMOOTHSCROLL])
        {
            scrolling_ = 10;
            
            if ((smoothScroll_ && scroll_ != 0 && SetUpFeature_[VTSMOOTHSCROLL]) || (scroll_ != 0 && !SetUpFeature_[VTSMOOTHSCROLL]) || ctrlQpressed_)
            {
                if (displayStart_ != displayEnd_)
                {
                    Display(getDisplay(), true);
                    while (displayBuffer_[displayStart_] != 10 && (displayStart_ != displayEnd_))
                    {
                        Display(getDisplay(), true);
                    }
                }
                if (scroll_ > 0)
                    scroll_--;
                if (displayStart_ == displayEnd_)
                {
                    if (smoothScroll_)
                        smoothScroll_ = false;
                    if (xOff_)
                    {
                        xOff_ = false;
                        videoScreenPointer->vtOut(17);
                    }
                }
            }
        }

        cycleValue_ = cycleSize_;
        if (changeScreenSize_)
        {
            changeScreenSize();
            changeScreenSize_ = false;
        }
//        if (zoomChanged_ != 0)
//        {
//            zoom_ = zoomChanged_;
//            double intPart;
//            zoomFraction_ = (modf(zoom_, &intPart) != 0);
//            this->SetClientSize((videoWidth_+2*borderX_)*zoom_, (videoHeight_+2*borderY_)*zoom_);
//            videoScreenPointer->SetClientSize((videoWidth_+2*borderX_)*zoom_, (videoHeight_+2*borderY_)*zoom_);
//
//            videoScreenPointer->setZoom(zoom_);
//            reBlit_ = true;
//            zoomChanged_ = 0;
//        }
        copyScreen();

        blinkCount_--;
        if (blinkCount_ <= 0)
        {
            blinkCount_ = 30;
            blinkOn_ = !blinkOn_;
            reBlink_ = true;
            cursorBlinkOn_ = !cursorBlinkOn_;
            ShowCursor();
        }
        if (blinkCount_ == 15)
        {
            cursorBlinkOn_ = !cursorBlinkOn_;
            ShowCursor();
        }
    }

    if (cycleBell_ > 0)
    {
        cycleBell_--;
        if (cycleBell_ == 0)
        {
            p_Computer->startTone(0, false);
            p_Computer->startTone(1, false);
        }
    }
    if (cycleClick_ > 0)
    {
        cycleClick_--;
        if (cycleClick_ == 0)
        {
            p_Computer->startTone(0, false);
            p_Computer->startTone(1, false);
        }
    }
    if (setUpMode_)
        return;

    if (uart1854_ || uart16450_)
    {
        if (vtOutCount_ > 0)
            uartVtOut();
        else
        {
            if (terminalLoad_ && !uart1854_)
                dataAvailableUart(1);
        }

        uartVtIn();
    }
    else  // if !uart
    {
        if (vtOutCount_ > 0)
            serialVtOut();
        else
        {
            bool dataReady = false;
            if (terminalLoad_ || (terminalSave_ && (protocol_ == TERM_XMODEM_SAVE || protocol_ == TERM_YMODEM_SAVE)))
            {
                if (vtOut_ == 0)
                {
                    if (p_Computer->isReadyToReceiveData(dataReadyFlag_-1))
                        dataReady = getTerminalLoadByte(&vtOut_);
                }
            }
            
            if ((vtOut_ != 0 || dataReady) && vtEnabled_)
            {
                vt100Ef_ = 0;
                parity_ = Parity(vtOut_);
                vtOutCount_ = baudRateT_;
                if (SetUpFeature_[VTBITS])
                    vtOutBits_ = 10;
                else
                    vtOutBits_ = 9;
                if (SetUpFeature_[VTPARITY])
                    vtOutBits_++;
                p_Computer->setGreenLed(vt100Ef_ ^ 1);
//                p_Main->message("start");
//                p_Main->eventMessageHex(vtOut_);
            }
        }
        serialVtIn();
    }
}

void Vt100::serialDataOutput(Byte transmitterHoldingRegister)
{
    if (terminalSave_ || terminalLoad_)
        Display(transmitterHoldingRegister, false);
    else
        Display(transmitterHoldingRegister & 0x7f, false);
}

Byte Vt100::readReceiverHoldingRegister()
{
    Byte loadByte = 0;
    if (terminalLoad_ || (terminalSave_ && (protocol_ == TERM_XMODEM_SAVE || protocol_ == TERM_YMODEM_SAVE)))
    {
        getTerminalLoadByte(&loadByte);
        return loadByte;
    }
    else
        return videoScreenPointer->getKey(0);
}

void Vt100::uartVtOut()
{
    vtOutCount_--;
    if (uartEf_)
    {
        if (vtOutCount_ <= 0)
        {
            if (vtOutBits_ == 10)
                vt100Ef_ = 0;
            else
            {
                vt100Ef_ = (vtOut_ & 1) ? 1 : 0;
                if (vtOutBits_ > 10)
                    vtOut_ = 0;
                else
                    vtOut_ = (vtOut_ >> 1) | 128;
            }
            vtOutCount_ = baudRateT_;
            if (vtOutBits_ == 2)
                vt100Ef_ = 1;
            if (--vtOutBits_ == 0)
            {
                vtOut_ = 0;
                dataAvailableUart(1);
                vtOutCount_ = -1;
                vtOutBits_=10;
            }
            if (vtOutBits_ == 11)
            {
                vt100Ef_ = 1;
                vtOutCount_ = -1;
                vtOutBits_=10;
            }
        }
    }
    else
    {
        if (vtOutCount_ == 0)
        {
            dataAvailableUart(1);
            vtOutCount_ = -1;
        }
    }
}

void Vt100::uartVtIn()
{

    vtCount_--;
    if (vtCount_ <= 0)
    {
        if (terminalSave_ || terminalLoad_)
        {
            if (uart1854_)
            {
                if (receivePacket_)
                    Display(rs232_, false);
            }
            else
            {
                if (receivePacket_ || rs232_ != 0)
                    Display(rs232_, false);
            }
        }
        else
        {
            if (rs232_ != 0)
                Display(rs232_ & 0x7f, false);
        }
        
        rs232_ = 0;
        p_Computer->thrStatusVt100(0);
        if (uartStatus_[uart_thre_bit_] == 1)
            uartStatus_[uart_tsre_bit_] = 1;
        uartStatus_[uart_thre_bit_] = 1;
        
        if (terminalSave_ && uart1854_)
            vtCount_ = baudRateR_ * 4;
        else
            vtCount_ = baudRateR_ * 9;
 //       uartInterrupt(); *** needed on Microboard or others?
    }
}

void Vt100::serialVtOut()
{
    vtOutCount_--;
    if (vtOutCount_ <= 0)
    { // input from terminal
        vt100Ef_ = (vtOut_ & 1) ? 1 : 0;
        vtOut_ = (vtOut_ >> 1) | 128;
        vtOutCount_ = baudRateT_;
        if (SetUpFeature_[VTPARITY])
        {
            if (vtOutBits_ == 3)
                vt100Ef_ = parity_;
            if (vtOutBits_ == 2)
                vt100Ef_ = 1;
        }
        else
        {
            if (vtOutBits_ == 2)
                vt100Ef_ = 1;
        }
        if (--vtOutBits_ == 0)
        {
            vtOut_ = 0;
            p_Computer->setNotReadyToReceiveData(dataReadyFlag_-1);
            vtOutCount_ = -1;
        }
//                else
//                    p_Main->messageInt(vt100Ef_);
        p_Computer->setGreenLed(vt100Ef_ ^ 1);
    }
}

void Vt100::serialVtIn()
{
    if (vtCount_ >= 0)
    { // output to terminal
        //wxString buffer;
        //buffer.Printf("%d", p_Computer->getFlipFlopQ());
        //p_Main->messageNoReturn(buffer);

        vtCount_--;
        if (vtCount_ <= 0)
        {
            //p_Main->messageInt(p_Computer->getFlipFlopQ());
            //p_Main->message("");
            if (SetUpFeature_[VTPARITY])
            {
                if (vtBits_ > 2)
                {
                    rs232_ >>= 1;
                    rs232_ |= (p_Computer->getFlipFlopQ() ^ reverseQ_) ? 0 : 128;
                }
                if (vtBits_ == 2)
                {
                    if (!SetUpFeature_[VTBITS])
                        rs232_ >>= 1;
                    if (Parity(rs232_) != p_Computer->getFlipFlopQ())
                        rs232_ = 2;
                }
            }
            else
            {
                if (vtBits_ > 1)
                {
                    rs232_ >>= 1;
                    rs232_ |= (p_Computer->getFlipFlopQ() ^ reverseQ_) ? 0 : 128;
                }
                if (vtBits_ == 1)
                {
                    if (!SetUpFeature_[VTBITS])
                        rs232_ >>= 1;
                }
            }
            vtCount_ = baudRateR_;
            if (--vtBits_ == 0)
            {
                vtCount_ = -1;
                if (terminalSave_ || terminalLoad_)
                    Display(rs232_, false);
                else
                    Display(rs232_ & 0x7f, false);
//                    p_Main->eventMessageHex(rs232_);
            }
        }
    }
}

bool Vt100::getTerminalLoadByte(Byte* value)
{
    char saveBuffer[3];
    size_t numberOfBytes = 0;
    bool dataReady = false;

    switch (protocol_)
    {
        case TERM_BIN:
            numberOfBytes = inputTerminalFile.Read(saveBuffer, 1);
        
            if (numberOfBytes == 1)
            {
                *value = saveBuffer[0];
                dataReady = true;
                previousByte_ = *value;
            }
        break;

        case TERM_YMODEM_SAVE:
            if (terminalAck_ & 0x80)
            {
                if ((xmodemBuffer_[3] == 0 && xmodemBuffer_[0] != 4) || xmodemBuffer_[0] == 24)
                {
                    terminalStopVt();
                    p_Main->stopTerminal();
                }
                else
                    outputTerminalFile.Close();
            }
            *value = terminalAck_ & 0x7f;

            if (*value == XMODEM_ACK && (xmodemBuffer_[1] == 0 || xmodemBuffer_[0] == 4) && terminalSave_)
            {
                terminalAck_ = XMODEM_CRC;
                if (uart1854_ || uart16450_)
                    dataAvailableUart(1);
            }
            
            dataReady = true;
            previousByte_ = *value;
            xmodemBufferSize_ = 133;
            xmodemBufferPointer_ = 0;
            xmodemBuffer_[xmodemBufferSize_-1] = 0;
            xmodemBuffer_[xmodemBufferSize_-2] = 0;
        break;

        case TERM_XMODEM_SAVE:
            if (terminalAck_ & 0x80)
            {
                terminalStopVt();
                p_Main->stopTerminal();
            }
            *value = terminalAck_ & 0x7f;

            dataReady = true;
            previousByte_ = *value;
            xmodemBufferPointer_ = 0;
            xmodemBuffer_[xmodemBufferSize_-1] = 0;
        break;

        case TERM_XMODEM_LOAD:
            if (xmodemBufferPointer_ == xmodemBufferSize_)
            {
                setSendPacket(false);
            }
            if (sendPacket_)
            {
                *value = xmodemBuffer_[xmodemBufferPointer_++];
                if (uart1854_)
                    uartStatus_[uart_da_bit_] = 1;
                if (xmodemBuffer_[0] == 4)
                {
                    if (useCrc_)
                        xmodemBufferPointer_ = 133;
                    else
                    {
                        if (!uart16450_)
                        {
                            dataAvailableUart(0);
                            setTerminalLoad(false);
                            p_Main->turboOff();
                            inputTerminalFile.Close();
                            p_Main->stopTerminal();
                        }
                    }
                }
                dataReady = true;
                previousByte_ = *value;
            }
            if (xmodemBufferPointer_ == xmodemBufferSize_)
            {
                if (uart1854_)
                {
                    setSendPacket(false);
                    dataAvailableUart(0);
                }
            }
        break;

        default:
            bool eof=false;
            int seek = -2;
            
            numberOfBytes = inputTerminalFile.Read(saveBuffer, 3);

            if (inputTerminalFile.Eof())
            {
                switch(numberOfBytes)
                {
                    case 0:
                        eof = true;
                    break;
                        
                    case 1:
                        if (saveBuffer[0] == 0xa)
                        {
                            saveBuffer[0] = 0xd;
                            eof = true;
                        }
                        seek = 0;
                    break;
                        
                    case 2:
                        if (saveBuffer[0] == 0xd && saveBuffer[1] == 0xa && !terminalFileCdp18s020_)
                            eof = true;
                        seek = -1;
                    break;
                }
            }
            
            if (saveBuffer[0] == 0xa && previousByte_ != 0xd)
                saveBuffer[0] = 0xd;
            
            if (eof)
            {
                setTerminalLoad(false);
                p_Main->turboOff();
                inputTerminalFile.Close();
                p_Main->stopTerminal();
            }
            else
            {
                inputTerminalFile.Seek(seek, wxFromCurrent);
                *value = saveBuffer[0];
                dataReady = true;
                previousByte_ = *value;
            }
        break;
    }
    return dataReady;
}

void Vt100::readBuffer()
{
    size_t numberOfBytes = 0;
    int crc;

    xmodemPacketNumber_++;
  
    if (useCrc_)
    {
        switch (p_Main->getPacketSize())
        {
            case 0:
                if (fileSize_ >= 1024)
                    xmodemBufferSize_ = 1029;
                else
                    xmodemBufferSize_ = 133;
            break;
            case 1:
                xmodemBufferSize_ = 133;
            break;
            case 2:
                xmodemBufferSize_ = 1029;
            break;
        }

        for (int i=0; i<(xmodemBufferSize_-2); i++)
            xmodemBuffer_[i] = 0x1a;
        xmodemBuffer_[xmodemBufferSize_-2] = 0;
        xmodemBuffer_[xmodemBufferSize_-1] = 0;
        numberOfBytes = inputTerminalFile.Read(&xmodemBuffer_[3], xmodemBufferSize_-5);
        fileSize_ -= (xmodemBufferSize_-5);
    }
    else
    {
        for (int i=0; i<(xmodemBufferSize_-1); i++)
            xmodemBuffer_[i] = 0x1a;
        xmodemBuffer_[xmodemBufferSize_-1] = 0;
        numberOfBytes = inputTerminalFile.Read(&xmodemBuffer_[3], xmodemBufferSize_-4);
    }

    if (numberOfBytes == 0)
        xmodemBuffer_[0] = 4;
    else
    {
        if (useCrc_)
        {
            if (xmodemBufferSize_ == 133)
                xmodemBuffer_[0] = 1;
            else
                xmodemBuffer_[0] = 2;

            crc = calcrc(&xmodemBuffer_[3], xmodemBufferSize_-5);
            xmodemBuffer_[xmodemBufferSize_-2] = (crc >> 8) & 0xff;
            xmodemBuffer_[xmodemBufferSize_-1] = crc & 0xff;
        }
        else
        {
            if (xmodemBufferSize_ == 132)
                xmodemBuffer_[0] = 1;
            else
                xmodemBuffer_[0] = 2;

            for (int i=3; i<(xmodemBufferSize_-1); i++)
                xmodemBuffer_[xmodemBufferSize_-1] += xmodemBuffer_[i];
        }
    }

    xmodemBuffer_[1] = xmodemPacketNumber_;
    xmodemBuffer_[2] = 255-xmodemPacketNumber_;
}

void Vt100::readFilename()
{
    int crc;

    for (int i=0; i<(xmodemBufferSize_-1); i++)
        xmodemBuffer_[i] = 0;
    xmodemBuffer_[xmodemBufferSize_-1] = 0;
    xmodemBuffer_[xmodemBufferSize_-2] = 0;
    
    size_t index = 0;

    xmodemBuffer_[index++] = 1;
    xmodemBuffer_[index++] = 0;
    xmodemBuffer_[index++] = 0xff;
    
    wxArrayString terminalFiles = p_Main->getTerminalFiles();
    for (size_t i=0; i<terminalFiles[xmodemFileNumber_-1].Len(); i++)
        xmodemBuffer_[index++] = terminalFiles[xmodemFileNumber_-1].GetChar(i);
    
    xmodemBuffer_[index++] = 0;

    size_t length = inputTerminalFile.Length();
    wxString lengthStr;
    lengthStr.Printf("%d", (int)length);
    
    for (size_t i=0; i<lengthStr.Len(); i++)
        xmodemBuffer_[index++] = lengthStr.GetChar(i);
    
    xmodemBuffer_[index++] = 0;

    crc = calcrc(&xmodemBuffer_[3], xmodemBufferSize_-5);
    xmodemBuffer_[xmodemBufferSize_-2] = (crc >> 8) & 0xff;
    xmodemBuffer_[xmodemBufferSize_-1] = crc & 0xff;
}

void Vt100::readEndFrame()
{
    int crc;

    xmodemBufferSize_ = 133;

    for (int i=0; i<(xmodemBufferSize_-1); i++)
        xmodemBuffer_[i] = 0;
    xmodemBuffer_[xmodemBufferSize_-1] = 0;
    xmodemBuffer_[xmodemBufferSize_-2] = 0;
    
    size_t index = 0;

    xmodemBuffer_[index++] = 1;
    xmodemBuffer_[index++] = 0;
    xmodemBuffer_[index++] = 0xff;
        
    crc = calcrc(&xmodemBuffer_[3], xmodemBufferSize_-5);
    xmodemBuffer_[xmodemBufferSize_-2] = (crc >> 8) & 0xff;
    xmodemBuffer_[xmodemBufferSize_-1] = crc & 0xff;
}

int Vt100::calcrc(char *ptr, int count)
{
    int  crc;
    char i;
    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}

void Vt100::switchQ(int value)
{
    if(uart1854_ || uart16450_)
        return;
    
    if (vtCount_ < 0)
    {
        if (value ^ reverseQ_)
        {
            vtCount_ = baudRateR_ + baudRateR_ / 2;
            if (SetUpFeature_[VTBITS])
                vtBits_ = 9;
            else
                vtBits_ = 8;
            if (SetUpFeature_[VTPARITY])
                vtBits_++;
            //                    p_Main->message("start");
            //                    p_Main->messageInt(p_Computer->getFlipFlopQ());
            rs232_ = 0;
        }
    }
}

void Vt100::setClock(double clock)
{
    clock_ = clock;
    reCycle_ = true;
}

void Vt100::setCycle()
{   
    if (SetUpFeature_[VTPOWER])
        cycleSize_ = (int) (((clock_ * 1000000) / 8) / 50);
    else
        cycleSize_ = (int) (((clock_ * 1000000) / 8) / 60);
    cycleSizeBell_ = (int) ((clock_ * 1000000) / 32);
    cycleSizeClick_ = (int) ((clock_ * 1000000) / 6000);

    cycleValue_ = cycleSize_;

    reCycle_ = false;
}

void Vt100::copyScreen()
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
        for (int i=0; i<10; i++)
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

    if (reDraw_)
        drawScreen();

    if (reBlink_)
        blinkScreen();

#if defined(__WXMAC__)
    if (reBlit_ || reDraw_ || reBlink_)
    {
        p_Main->eventRefreshVideo(true, uartNumber_);
        reBlit_ = false;
        reDraw_ = false;
        reBlink_ = false;
    }
#else
    if (extraBackGround_ && newBackGround_) 
        drawExtraBackground(colour_[colourIndex_+1]);

    CharacterList *temp;

    if (reBlit_ || reDraw_ || reBlink_)
    {
        videoScreenPointer->blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);
        reBlit_ = false;
        reDraw_ = false;
        reBlink_ = false;
        if (updateCharacter_)
        {
            updateCharacter_ = false;
            while(characterListPointer != NULL)
            {
                temp = characterListPointer;
                characterListPointer = temp->nextCharacter;
                delete temp;
            }
        }
    }
    if (updateCharacter_)
    {
        updateCharacter_ = false;
        while(characterListPointer != NULL)
        {
            videoScreenPointer->blit(offsetX_+(characterListPointer->x), offsetY_+(characterListPointer->y), charWidth_*(characterListPointer->doubleWidth), linesPerCharacter_*heightFactor, &dcMemory, offsetX_+characterListPointer->x, offsetY_+characterListPointer->y);
            temp = characterListPointer;
            characterListPointer = temp->nextCharacter;
            delete temp;
        }
    }
#endif
}

void Vt100::startXmlRun(bool load, wxString command)
{
    load_ = load;
    commandText_ = command;
    elfRunCommand_ = 1;
    if (commandText_ == "")
        elfRunCommand_++;
}

int Vt100::Parity(int value)
{
    int i;
    int par;
    par = 0;
    int numberOfBits;
    if (SetUpFeature_[VTBITS])
        numberOfBits = 8;
    else
        numberOfBits = 7;
    for (i=0; i<numberOfBits; i++)
    {
        if (value & 1) par++;
        value >>= 1;
    }
    if (SetUpFeature_[VTPARITYSENSE])
        return(par & 1);
    else
        return((par & 1) ^ 1);
}

void Vt100::ScrollUp()
{
    int x,y;
    for (y=scrollStart_; y<(scrollEnd_-1); y++)
    {
        doubleWidth_[y] = doubleWidth_[y+1];
        doubleHeight_[y] = doubleHeight_[y+1];
        startLine_[y] = startLine_[y+1];
        for (x=0; x<charactersPerRow_; x++)
        {
            reverseScr_[y][x] = reverseScr_[y+1][x];
            underlineScr_[y][x] = underlineScr_[y+1][x];
            highlightScr_[y][x] = highlightScr_[y+1][x];
            blinkScr_[y][x] = blinkScr_[y+1][x];
            writeMem(y, x, videoRam_[y+1][x]);
        }
    }
    for (x=0; x<charactersPerRow_; x++)
    {
        reverseScr_[scrollEnd_-1][x] = false;
        underlineScr_[scrollEnd_-1][x] = false;
        highlightScr_[scrollEnd_-1][x] = false;
        blinkScr_[scrollEnd_-1][x] = false;
        writeMem(scrollEnd_-1, x, 32);
    }
    doubleWidth_[scrollEnd_-1] = 1;
    doubleHeight_[scrollEnd_-1] = 1;
    startLine_[scrollEnd_-1] = 0;
    drawScreen();
}

void Vt100::ScrollDown()
{
    int x,y;
    for (y=scrollEnd_-1; y>scrollStart_; y--)
    {
        doubleWidth_[y] = doubleWidth_[y-1];
        doubleHeight_[y] = doubleHeight_[y-1];
        startLine_[y] = startLine_[y-1];
        for (x=0; x<charactersPerRow_; x++)
        {
            reverseScr_[y][x] = reverseScr_[y-1][x];
            underlineScr_[y][x] = underlineScr_[y-1][x];
            highlightScr_[y][x] = highlightScr_[y-1][x];
            blinkScr_[y][x] = blinkScr_[y-1][x];
            writeMem(y, x, videoRam_[y-1][x]);
        }
    }
    for (x=0; x<charactersPerRow_; x++)
    {
        reverseScr_[scrollStart_][x] = false;
        underlineScr_[scrollStart_][x] = false;
        highlightScr_[scrollStart_][x] = false;
        blinkScr_[scrollStart_][x] = false;
        writeMem(scrollStart_, x, 32);
    }
    doubleWidth_[scrollStart_] = 1;
    doubleHeight_[scrollStart_] = 1;
    startLine_[scrollStart_] = 0;
    drawScreen();
}

void Vt100::drawScreen()
{
    reDraw_ = true;
    setColour(colourIndex_+1);
    drawRectangle(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
    for (int y=0; y<rowsPerFrame_; y++)
    {
        for (int x=0; x<(charactersPerRow_/doubleWidth_[y]); x++)
        {
            drawCharacter(x, y, videoRam_[y][x], false);
        }
    }
}

void Vt100::blinkScreen()
{
    reBlink_ = true;
    for (int y=0; y<rowsPerFrame_; y++)
    {
        for (int x=0; x<(charactersPerRow_/doubleWidth_[y]); x++)
        {
            if (blinkScr_[y][x])
                drawCharacter(x, y, videoRam_[y][x], false);
        }
    }
}

void Vt100::drawCharacter(int pos, int line, Byte v, bool cursor)
{
    int line_byte;
    float drawLine;
    bool stretchDot;

    int x = pos * charWidth_ * doubleWidth_[line];
    int y = line * linesPerCharacter_ * heightFactor;
#if defined(__WXMAC__) || defined(__linux__)
    reBlit_ = true;
#else
    if (!(reDraw_ || reBlit_))
    {
        if (zoomFraction_)
            reBlit_ = true;
        else
        {
            CharacterList *temp = new CharacterList;
            temp->x = x;
            temp->y = y;
            temp->doubleWidth = doubleWidth_[line];
            temp->nextCharacter = characterListPointer;
            characterListPointer = temp;
            updateCharacter_ = true;
        }
    }
#endif

    if (reverseScr_[line][pos] ^ SetUpFeature_[VTREVERSESCREEN])
    {
        setColour(colourIndex_);
        drawRectangle(x+offsetX_, y+offsetY_, charWidth_*doubleWidth_[line], linesPerCharacter_*heightFactor);

        setColour(colourIndex_+1);
    }
    else
    {
        setColour(colourIndex_+1);
        drawRectangle(x+offsetX_, y+offsetY_, charWidth_*doubleWidth_[line], linesPerCharacter_*heightFactor);

        setColour(colourIndex_);
    }
    
    if (blinkScr_[line][pos] && blinkOn_)
    {
        return;
    }

    drawLine = startLine_[line];
    for (wxCoord j=y; j<y+(linesPerCharacter_*heightFactor); j+=heightFactor)
    {
//        if (highlightScr_[line][pos])
//            setColour(colourIndex_+2);
//        else
//            setColour(colourIndex_);
    
        stretchDot = false;
        line_byte =    charRom_[v*16+(int)drawLine];

        if (underlineScr_[line][pos] && (drawLine == 8))
            line_byte = 0xff;

        if (cursor && cursorBlinkOn_ && ((drawLine == 8) || SetUpFeature_[VTCURSORBLOCK] || (doubleHeight_[line] == 0.5 && startLine_[line] == 0 && drawLine == 4.5)))
        {
            line_byte ^= 0xff;
        }

        int bitNumber = 7;
        for (wxCoord i=x; i<x+(charWidth_*doubleWidth_[line]); i+=doubleWidth_[line])
        {
            if (((line_byte & 0x80)==0x80) || stretchDot)
            {
                if (SetUpFeature_[VTINTERLACE] && (heightFactor != 1))
                    drawRectangle(i+offsetX_, j+offsetY_, doubleWidth_[line], heightFactor);
                else if ((doubleWidth_[line] != 1))
                    drawRectangle(i+offsetX_, j+offsetY_, doubleWidth_[line], 1);
                else
                    drawPoint(i+offsetX_, j+offsetY_);
            }
            stretchDot = ((line_byte & 0x80)==0x80) & (stretchDot_ || highlightScr_[line][pos]);
            if (bitNumber > 0)
            {
                line_byte <<= 1;
                bitNumber--;
            }
        }
        drawLine+=doubleHeight_[line];
    }

}

void Vt100::setInterlace(bool status)
{
    SetUpFeature_[VTINTERLACE] = status;
    reDraw_ = true;
}

void Vt100::setStretchDot(bool status)
{
    stretchDot_ = status;
    reDraw_ = true;
}
/*
void Vt100::setZoom(double zoom)
{
    if (zoomChanged_ == 0)
        zoomChanged_ = zoom;
}*/

void Vt100::ShowCursor()
{
    if (cursorLine_ >= 0 && cursorPosition_ >= 0)
        drawCharacter(cursorPosition_, cursorLine_, videoRam_[cursorLine_][cursorPosition_], true);
}

void Vt100::RemoveCursor()
{
    if (cursorLine_ >= 0 && cursorPosition_ >= 0)
        drawCharacter(cursorPosition_, cursorLine_, videoRam_[cursorLine_][cursorPosition_], false);
}

char* Vt100::getInteger(char* buffer,int* dest)
{
    *dest = 0;
    while(*buffer >= '0' && *buffer <= '9')
    {
        *dest = *dest * 10 +(*buffer - '0');
        buffer++;
    }
    return buffer;
}

void Vt100::Vterr()
{
    escPosition_ = -1;
    
    if (!currentComputerConfiguration.videoTerminalConfiguration.escError)
        return;

    for (Word i=1; i<strlen(escBuffer_); i++) Display(escBuffer_[i], false);
}

void Vt100::setFullScreen(bool fullScreenSet)
{
    fullScreenSet_ = fullScreenSet;
    ShowFullScreen(fullScreenSet);
}

void Vt100::onF3()
{
    fullScreenSet_ = !fullScreenSet_;
    p_Main->eventVtSetFullScreen(fullScreenSet_, uartNumber_);
}

void Vt100::scrollLine()
{
    scroll_ = 1;
}

void Vt100::scrollScreen()
{
    scroll_ = rowsPerFrame_;
}

void Vt100::Display(int byt, bool forceDisplay)
{
    int oldPos, crc, pos;
    wxString character, newDir;
    wxString fileName, length;

    if ((serialLog_ && !forceDisplay))
    {
        if (byt > 31)
        {
            character.Printf("%c",byt);
            line_ = line_ + character;
        }
        else
        {
            if (byt == 0xa)
            {
                logFile_.AddLine(line_);
                line_ = "";
             }
        }
    }

    if (terminalSave_  || (terminalLoad_ && protocol_ == TERM_XMODEM_LOAD))
    {
        unsigned char buffer[1];
        buffer[0] = byt;
        if (terminalFileCdp18s020_)
        {
            switch (byt)
            {
                case 0:
                break;
                    
                case 0xa:
                    if (lastByte_ == -1)
                        lastByte_ = byt;
                    if (lastByte_ == 0xd)
                    {
                        outputTerminalFile.Write(buffer, 1);
                        lastByte_ = byt;
                    }
               break;
                    
                case 0xd:
                    if (lastByte_ != -1)
                    {
                        outputTerminalFile.Write(buffer, 1);
                        lastByte_ = byt;
                    }
                break;
                    
                case 0x2a:
                    terminalStopVt();
                    p_Main->stopTerminal();
                break;

                default:
                    outputTerminalFile.Write(buffer, 1);
                    lastByte_ = byt;
               break;

            }
        }
        else
        {
            switch (protocol_)
            {
                case TERM_YMODEM_SAVE:
                    switch (xmodemBufferPointer_)
                    {
                        case 0:
                            receivePacket_ = true;
                            if (byt == 2)
                                xmodemBufferSize_ = 1029;

                            if (byt == 4)
                            {
                                terminalAck_ = 0x86;
                                xmodemBufferPointer_ = 0;
                                if (uart1854_ || uart16450_)
                                    dataAvailableUart(1);
                            }

                            if (byt == 24)
                            {
                                terminalStopVt();
                                p_Main->stopTerminal();
                            }
                            else
                            {
                                xmodemBuffer_[xmodemBufferPointer_] = byt;
                                xmodemBufferPointer_++;
                            }
                        break;
                            
                        case 1:
                        case 2:
                            xmodemBuffer_[xmodemBufferPointer_] = byt;
                            xmodemBufferPointer_++;
                        break;
                            
                        case 131:
                            if (xmodemBufferSize_ != 133 && fileSize_ != 0)
                            {
                                outputTerminalFile.Write(buffer, 1);
                                fileSize_--;
                            }
                            xmodemBuffer_[xmodemBufferPointer_] = byt;
                            xmodemBufferPointer_++;
                        break;

                        case 132:
                            if (xmodemBufferSize_ == 133)
                            {
                                receivePacket_ = false;

                                crc = calcrc(&xmodemBuffer_[3], xmodemBufferSize_-5);
                                
                                if (xmodemBuffer_[xmodemBufferSize_-2] == (char) ((crc >> 8) & 0xff) && byt == (crc & 0xff))
                                {
                                    terminalAck_ = XMODEM_ACK;
                                    
                                    if (xmodemBuffer_[1] == 0)
                                    {
                                        fileName = "";
                                        length = "";
                                        pos = 3;
                                        
                                        while (xmodemBuffer_[pos] != 0)
                                            fileName.Append(xmodemBuffer_[pos++]);
                                        
                                        pos++;
                                        
                                        while (xmodemBuffer_[pos] != 0x20)
                                            length.Append(xmodemBuffer_[pos++]);
 
                                        if (!length.ToLong(&fileSize_, 10))
                                            fileSize_ = -1;

                                        if (!fileName.empty())
                                        {
                                            if (fileName.GetChar(0) == '/')
                                                fileName = fileName.Right(fileName.Len()-1);

                                            newDir = "";
                                            
                                            while (fileName.Find('/') != wxNOT_FOUND)
                                            {
                                                newDir = fileName.Left(fileName.Find('/')) + p_Main->getPathSep();
                                                fileName = fileName.Right(fileName.Len() - fileName.Find('/') - 1);
                                                
                                                if (!wxDir::Exists(directoryName_ + newDir))
                                                {
                                                    if (!wxFileName::Mkdir(directoryName_ + newDir))
                                                        newDir = "";
                                                }
                                            }
                                            
                                            if (wxFile::Exists(directoryName_ + newDir + fileName))
                                            {
                                                p_Main->eventShowMessageBox( fileName+" already exists.\n"+"Do you want to replace it?",
                                                                            "Confirm Save As", wxICON_EXCLAMATION | wxYES_NO);
                                                
                                                if (p_Main->getMessageBoxAnswer() == wxNO)
                                                {
                                                    fileName = p_Main->eventShowFileSelector( "Select the terminal file to save",
                                                                                              directoryName_, fileName,
                                                                                              "",
                                                                                              "All files (*.*)|*.*",
                                                                                              wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_OVERWRITE_PROMPT
                                                                                             );
                                                    if (!fileName)
                                                        terminalAck_ = XMODEM_NAK;

                                                    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
                                                    fileName = FullPath.GetFullName();
                                                    directoryName_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
                                                }
                                            }
                                            if (terminalAck_ != XMODEM_NAK)
                                            {
                                                if (!outputTerminalFile.Create(directoryName_ + newDir + fileName, true))
                                                    terminalAck_ = XMODEM_NAK;
                                            }
                                        }
                                        else
                                        {
                                            terminalAck_ = 0x86;
                                            xmodemBufferPointer_ = 0;
                                        }
                                    }
                                }
                                else
                                {
                                    terminalAck_ = XMODEM_NAK;
                                    xmodemBufferPointer_ = 0;
                                    xmodemBuffer_[xmodemBufferSize_-1] = 0;
                                }
                                if (uart1854_ || uart16450_)
                                    dataAvailableUart(1);
                            }
                            else
                            {
                                if (xmodemBuffer_[1] != 0 && fileSize_ != 0)
                                {
                                    outputTerminalFile.Write(buffer, 1);
                                    fileSize_--;
                                }
                                xmodemBuffer_[xmodemBufferPointer_] = byt;
                                xmodemBufferPointer_++;
                            }
                        break;
                            
                        case 1027:
                            xmodemBuffer_[xmodemBufferPointer_] = byt;
                            xmodemBufferPointer_++;
                        break;
                            
                        case 1028:
                            receivePacket_ = false;
                            crc = calcrc(&xmodemBuffer_[3], xmodemBufferSize_-5);
                            if (xmodemBuffer_[xmodemBufferSize_-2] == (char) ((crc >> 8) & 0xff) && byt == (crc & 0xff))
                                terminalAck_ = XMODEM_ACK;
                            else
                            {
                                terminalAck_ = XMODEM_NAK;
                                outputTerminalFile.Seek(outputTerminalFile.Tell()-(xmodemBufferSize_-5));
                                xmodemBufferPointer_ = 0;
                                xmodemBuffer_[xmodemBufferSize_-1] = 0;
                                xmodemBuffer_[xmodemBufferSize_-2] = 0;
                            }
                            if (uart1854_ || uart16450_)
                                dataAvailableUart(1);
                        break;

                        default:
                            if (xmodemBuffer_[1] != 0 && fileSize_ != 0)
                            {
                                outputTerminalFile.Write(buffer, 1);
                                fileSize_--;
                            }
                            xmodemBuffer_[xmodemBufferPointer_] = byt;
                            xmodemBufferPointer_++;
                        break;
                    }
                break;
                    
                case TERM_XMODEM_SAVE:
                    if (uart1854_)
                        receivePacket_ = false;
                    switch (xmodemBufferPointer_)
                    {
                        case 0:
                            if (!uart1854_)
                                receivePacket_ = true;
                            if (byt == 4)
                            {
                                terminalAck_ = 0x86;
                                if (uart1854_ || uart16450_)
                                    dataAvailableUart(1);
                            }
                            xmodemBuffer_[xmodemBufferPointer_] = byt;
                            xmodemBufferPointer_++;
                        break;
                            
                        case 1:
                        case 2:
                            xmodemBuffer_[xmodemBufferPointer_] = byt;
                            xmodemBufferPointer_++;
                        break;

                        case 131:
                            if (xmodemBufferSize_ == 132)
                            {
                                receivePacket_ = false;
                                if (xmodemBuffer_[xmodemBufferSize_-1] == (char) byt)
                                    terminalAck_ = XMODEM_ACK;
                                else
                                {
                                    terminalAck_ = XMODEM_NAK;
                                    outputTerminalFile.Seek(outputTerminalFile.Tell()-(xmodemBufferSize_-4));
                                    xmodemBufferPointer_ = 0;
                                    xmodemBuffer_[xmodemBufferSize_-1] = 0;
                                }
                                if (uart1854_ || uart16450_)
                                    dataAvailableUart(1);
                            }
                            else
                            {
                                outputTerminalFile.Write(buffer, 1);
                                xmodemBuffer_[xmodemBufferSize_-1] += byt;
                                xmodemBufferPointer_++;
                            }
                        break;
                            
                        default:
                            outputTerminalFile.Write(buffer, 1);
                            xmodemBuffer_[xmodemBufferPointer_] = byt;
                            xmodemBuffer_[xmodemBufferSize_-1] += byt;
                            xmodemBufferPointer_++;
                        break;
                    }
                break;

                case TERM_XMODEM_LOAD:
                    switch (byt)
                    {
                        case XMODEM_NAK:
                            useCrc_ = false;
                            xmodemBufferPointer_ = 0;
                            if (xmodemPacketNumber_ == 0)
                                readBuffer();
                            if (uart1854_)
                                dataAvailableUart(1);
                            setSendPacket(true);
                            sendingMode_ = XMODEM_DATA;
                        break;

                        case XMODEM_CRC:
                            switch (sendingMode_)
                            {
                                case YMODEM_HEADER:
                                    useCrc_ = true;
                                    xmodemBufferPointer_ = 0;
                                    xmodemBufferSize_++;
                                    readFilename();
                                    setSendPacket(true);
                                break;

                                case YMODEM_DATA:
                                    xmodemBufferPointer_ = 0;
                                    readBuffer();
                                    setSendPacket(true);
                                break;

                                case YMODEM_END_FRAME:
                                    xmodemBufferPointer_ = 0;
                                    readEndFrame();
                                    setSendPacket(true);
//                                    terminalLoad_ = false;
  //                                  Display(byt, false);
    //                                terminalLoad_ = true;
                                break;
                            }
                         break;

                        case XMODEM_ACK:
                            switch (sendingMode_)
                            {
                                case YMODEM_HEADER:
                                    sendingMode_ = YMODEM_DATA;
                                break;

                                case YMODEM_DATA:
                                    if (xmodemBuffer_[0] == 4)
                                    {
                                        if (xmodemFileNumber_ < p_Main->getNumberOfTerminalFiles())
                                        {
                                            inputTerminalFile.Close();
                                            xmodemFileNumber_++;
                                            fileName = p_Main->getTerminalPath((int)xmodemFileNumber_-1);
                                            if (!fileName.empty())
                                            {
                                                if (inputTerminalFile.Open(fileName, _("rb")))
                                                {
                                                    fileSize_ = inputTerminalFile.Length();
                                                    setTerminalLoad(true);
                                                    setSendPacket(false);
                                                    xmodemBufferSize_ = 132;
                                                    p_Computer->setNotReadyToReceiveData(dataReadyFlag_-1);
                                                    previousByte_ = 0;
                                                    xmodemBufferPointer_ = xmodemBufferSize_;
                                                    xmodemPacketNumber_  = 0;
                                                    sendingMode_ = YMODEM_HEADER;
                                                    return;
                                                }
                                            }
                                        }
                                        sendingMode_ = YMODEM_END_FRAME;
                                        return;
                                    }
                                    else
                                    {
                                        xmodemBufferPointer_ = 0;
                                        readBuffer();
                                        setSendPacket(true);
                                    }
                                break;

                                case XMODEM_DATA:
                                    xmodemBufferPointer_ = 0;
                                    readBuffer();
                                    if (uart1854_)
                                        dataAvailableUart(1);
                                    setSendPacket(true);
                                break;
                            }
                        break;
                     
                        case 0:
                        break;

                        default:
                            if (uart1854_ || uart16450_)
                            {
                                dataAvailableUart(0);
                                clearUartInterrupt();
                            }
                            setTerminalLoad(false);
                            p_Main->turboOff();
                            inputTerminalFile.Close();
                            p_Main->stopTerminal();
                        break;

                    }
                break;

                default:
                    outputTerminalFile.Write(buffer, 1);
            }
            return;
        }
    }

    if ((xOff_ || smoothScroll_ || (displayStart_ != displayEnd_)) && !forceDisplay)
    {
        displayBuffer(byt);
        if (byt == 17)
            ctrlQpressed_ = true;
        return;
    }

    RemoveCursor();
    if (escPosition_ >= 0)
    {
        if (escPosition_ > 20)
        {
            escPosition_ = -1;
            return;
        }
        escBuffer_[escPosition_++] = byt;
        escBuffer_[escPosition_] = 0;

        if (SetUpFeature_[VTANSI])
            escapeVT100(byt);
        else
            escapeVT52(byt);

        ShowCursor();
        return;
    }
    if (byt < 32 && byt != 2)
    {
        switch (byt)
        {
            case 7:
                bell();
            break;
            case 8:
                cursorPosition_ --;
                if (cursorPosition_ < 0) cursorPosition_ = 0;
            break;
            case 9:
                oldPos = cursorPosition_;
                if (vtType_ != VT100)
                {
                    if (cursorPosition_ > 71)
                    {
                        cursorPosition_ ++;
                        if (cursorPosition_ >= (charactersPerRow_/doubleWidth_[cursorLine_])) cursorPosition_ = (charactersPerRow_/doubleWidth_[cursorLine_]) - 1;
                    }
                    else
                        cursorPosition_ = (cursorPosition_ & 0xf8) + 8;
                }
                else
                {
                    if (cursorPosition_< (charactersPerRow_/doubleWidth_[cursorLine_]) - 1)
                        cursorPosition_++;
                    while ((tabs_.GetChar(cursorPosition_) == ' ') && (cursorPosition_< (charactersPerRow_/doubleWidth_[cursorLine_]) - 1))
                    {
                        cursorPosition_++;
                    }
                }
                if (cursorPosition_ >= (charactersPerRow_ - 8) && oldPos < (charactersPerRow_ - 8))
                    bell();
            break;
            case 10:
            case 11:
                if (cursorLine_ >= scrollEnd_-1)
                {
                    if (SetUpFeature_[VTAUTOXON] && !forceDisplay && (scroll_ == 0))
                    {
                        xOff_ = true;
                        videoScreenPointer->vtOut(19);
                        displayBuffer(10);
                    }
                    else if (SetUpFeature_[VTSMOOTHSCROLL] && !forceDisplay)
                    {
                        smoothScroll_ = true;
                        displayBuffer(10);
                    }
                    else
                    {
                        cursorLine_ = scrollEnd_-1;
                        ScrollUp();
                    }
                }
                else
                    cursorLine_++;
                if (SetUpFeature_[VTNEWLINE])
                    cursorPosition_ = 0;
            break;
            case 13:
                cursorPosition_ = 0;
            break;
            case 14:
                switch (g1mode_)
                {
                    case '0':
                        graphicsMode_ = true;
                        scsMode_ = false;
                    break;
                    case '1':
                        SetUpFeature_[VTUSASCII] = 1;
                        graphicsMode_ = false;
                        scsMode_ = true;
                    break;
                    case '2':
                        graphicsMode_ = true;
                        scsMode_ = true;
                    break;
                    case 'A':
                        SetUpFeature_[VTUSASCII] = 0;
                        graphicsMode_ = false;
                        scsMode_ = false;
                    break;
                    case 'B':
                        graphicsMode_ = false;
                        scsMode_ = false;
                        SetUpFeature_[VTUSASCII] = 1;
                    break;
                }
            break;
            case 15:
                switch (g0mode_)
                {
                    case '0':
                        graphicsMode_ = true;
                        scsMode_ = false;
                    break;
                    case '1':
                        SetUpFeature_[VTUSASCII] = 1;
                        graphicsMode_ = false;
                        scsMode_ = true;
                    break;
                    case '2':
                        graphicsMode_ = true;
                        scsMode_ = true;
                    break;
                    case 'A':
                        SetUpFeature_[VTUSASCII] = 0;
                        graphicsMode_ = false;
                        scsMode_ = false;
                    break;
                    case 'B':
                        graphicsMode_ = false;
                        scsMode_ = false;
                        SetUpFeature_[VTUSASCII] = 1;
                    break;
                }
            break;
            case 19:
                if (!p_Computer->isSaving())
                    xOff_ = true;
            break;
            case 25:
            case 26:
                escPosition_ = -1;
                byt = 2;
            break;
            case 27:
                escPosition_ = 0;
            break;
        }
        if (byt != 2)
        {
            ShowCursor();
            return;
        }    
    }
    if (byt == 0x7f) return;

    highlightScr_[cursorLine_][cursorPosition_] = highlight_;
    underlineScr_[cursorLine_][cursorPosition_] = underline_;
    blinkScr_[cursorLine_][cursorPosition_] = blink_;
    reverseScr_[cursorLine_][cursorPosition_] = reverse_;

    if (graphicsMode_ && (byt >= 94) && (byt <= 126))
    {
        if (byt < 95)
            byt = 0;
        else
            byt -= 95;
    }
    if (scsMode_)
        byt |= 0x80;
    if (!SetUpFeature_[VTUSASCII])
        if (byt == 35)
            byt = 30;


    drawCharacter(cursorPosition_, cursorLine_, byt, false);
    writeMem(cursorLine_, cursorPosition_, byt);
    if (++cursorPosition_ == (charactersPerRow_/doubleWidth_[cursorLine_]))
    {
        if (!SetUpFeature_[VTWRAPAROUND])
            cursorPosition_ = (charactersPerRow_/doubleWidth_[cursorLine_]) - 1;
        else
        {
            cursorPosition_ = 0;
            if (++cursorLine_ >= scrollEnd_)
            {
                cursorLine_ = scrollEnd_-1;
                ScrollUp();
            }
        }
    }

    ShowCursor();
}

void Vt100::escapeVT52(Byte byt)
{
    int newPos;

    switch (escPosition_)
    {
        case 1:
            switch (byt)
            {
                case 27:
                    escPosition_ = 0;
                break;

                case 'A':
                    cursorLine_ --;
                    if (cursorLine_ < 0) cursorLine_ = 0;
                    escPosition_ = -1;
                break;

                case 'B':
                    cursorLine_++;
                    if (cursorLine_ >= rowsPerFrame_) cursorLine_ = rowsPerFrame_-1;
                    escPosition_ = -1;
                break;

                case 'C':
                    cursorPosition_ ++;
                    if (cursorPosition_ >= charactersPerRow_) cursorPosition_ = charactersPerRow_ - 1;
                    escPosition_ = -1;
                break;

                case 'D':
                    cursorPosition_ --;
                    if (cursorPosition_ < 0) cursorPosition_ = 0;
                    escPosition_ = -1;
                break;

                case 'F':
                    graphicsMode_ = true;
                    escPosition_ = -1;
                break;

                case 'G':
                    graphicsMode_ = false;
                    escPosition_ = -1;
                break;

                case 'H':
                    cursorPosition_ = 0;
                    cursorLine_ = 0;
                    escPosition_ = -1;
                break;

                case 'I':
                    cursorLine_ --;
                    if (cursorLine_ < 0)
                    {
                        cursorLine_ = 0;
                        ScrollDown();
                    }
                    escPosition_ = -1;
                break;

                case 'J':
                    for (int i=cursorPosition_; i<charactersPerRow_; i++)
                        writeMem(cursorLine_, i, 32);
                    if (cursorLine_ <= rowsPerFrame_)
                    {
                        for (int y=cursorLine_+1; y<rowsPerFrame_; y++)
                            for (int x=0; x<charactersPerRow_; x++)
                                writeMem(y, x, 32);
                    }
                    escPosition_ = -1;
                    reDraw_ = true;
                break;

                case 'K':
                    for (int i=cursorPosition_; i<charactersPerRow_; i++)
                    {
                        writeMem(cursorLine_, i, 32);
                        drawCharacter(i, cursorLine_, 32, false);
                    }
                    escPosition_ = -1;
                break;

                case 'Y':
                break;

                case 'Z':
                    videoScreenPointer->vtOut(27);
                    videoScreenPointer->vtOut(0x2f);
                    if (vtType_ == VT100)
                        videoScreenPointer->vtOut('Z');
                    else
                        videoScreenPointer->vtOut('K');
                    escPosition_ = -1;
                break;

                case '[':
                    if (vtType_ == VT52)
                    {
                        SetUpFeature_[VTAUTOXON] = true;
                        escPosition_ = -1;
                    }
                    else
                        Vterr();
                break;

                case '\\':
                    if (vtType_ == VT52)
                    {
                        SetUpFeature_[VTAUTOXON] = false;
                        escPosition_ = -1;
                    }
                    else
                        Vterr();
                break;

                case '=':
                    AltKeypad_ = true;
                    escPosition_ = -1;
                break;

                case '>':
                    AltKeypad_ = false;
                    escPosition_ = -1;
                break;

                case '<':
                    if (vtType_ == VT100)
                        SetUpFeature_[VTANSI] = 1;
                break;

                default:
                    Vterr();
            }
        break;

        case 2:
            if (escBuffer_[0] != 'Y')
                Vterr();
        break;

        case 3:
            if (escBuffer_[0] != 'Y')
                Vterr();
            else
            {
                newPos = escBuffer_[1] - 0x20;
                if ((newPos <= rowsPerFrame_-1) && (newPos >= 0))
                    cursorLine_ = newPos;
                newPos = escBuffer_[2] - 0x20;
                if ((newPos <= charactersPerRow_ - 1) && (newPos >= 0))
                    cursorPosition_ = newPos;
                else 
                    cursorPosition_ = charactersPerRow_ - 1;
            }
            escPosition_ = -1;
        break;

        default:
            Vterr();
        break;
    }
}

void Vt100::escapeVT100(Byte byt)
{
    char *pBuffer;
    int i, x, y, bufPos;
    bool error;
    wxString buffer;

    switch (byt)
    {
        case '0':
        case '1':
        case '2':
            if (escBuffer_[0] == '(')
            {
                g0mode_ = byt;
                escPosition_ = -1;
            }
            if (escBuffer_[0] == ')')
            {
                g1mode_ = byt;
                escPosition_ = -1;
            }
        break;

        case '3':
            if ((escBuffer_[0] == '#') && (escBuffer_[1] == '3'))
            {
                doubleWidth_[cursorLine_] = 2;
                if (cursorPosition_ >= (charactersPerRow_/doubleWidth_[cursorLine_])) cursorPosition_ = (charactersPerRow_/doubleWidth_[cursorLine_]) - 1;
                doubleHeight_[cursorLine_] = 0.5;
                startLine_[cursorLine_] = 0;
                reDraw_ = true;
                escPosition_ = -1;
            }
        break;

        case '4':
            if ((escBuffer_[0] == '#') && (escBuffer_[1] == '4'))
            {
                doubleWidth_[cursorLine_] = 2;
                if (cursorPosition_ >= (charactersPerRow_/doubleWidth_[cursorLine_])) cursorPosition_ = (charactersPerRow_/doubleWidth_[cursorLine_]) - 1;
                doubleHeight_[cursorLine_] = 0.5;
                startLine_[cursorLine_] = linesPerCharacter_/2;
                reDraw_ = true;
                escPosition_ = -1;
            }
        break;

        case '5':
            if ((escBuffer_[0] == '#') && (escBuffer_[1] == '5'))
            {
                doubleWidth_[cursorLine_] = 1;
                doubleWidth_[cursorLine_] = 1;
                doubleHeight_[cursorLine_] = 1;
                reDraw_ = true;
                escPosition_ = -1;
            }
        break;

        case '6':
            if ((escBuffer_[0] == '#') && (escBuffer_[1] == '6'))
            {
                doubleWidth_[cursorLine_] = 2;
                if (cursorPosition_ >= (charactersPerRow_/doubleWidth_[cursorLine_])) cursorPosition_ = (charactersPerRow_/doubleWidth_[cursorLine_]) - 1;
                reDraw_ = true;
                escPosition_ = -1;
            }
        break;

        case '7':
            if (escBuffer_[0] == '7')
            {
                savedCursorLine_ = cursorLine_;
                savedCursorPosition_ = cursorPosition_;
            }
        break;

        case '8':
            switch (escBuffer_[0])
            {
                case '8':
                    cursorLine_ = savedCursorLine_;
                    cursorPosition_ = savedCursorPosition_;
                break;

                case '#':
                    for (int y_temp=0; y_temp<rowsPerFrame_; y_temp++)
                        for (int x_temp=0; x_temp<(charactersPerRow_/doubleWidth_[y_temp]); x_temp++)
                            writeMem(y_temp, x_temp, 'E');
                    reDraw_ = true;
                break;
            }
        break;

        case 'A':
            if (escBuffer_[0] == '[')
            {
                pBuffer = getInteger(&escBuffer_[1],&x);
                if (*pBuffer == byt)
                {
                    if (x == 0) x = 1;
                    cursorLine_ -= x;
                    if (originMode_)
                    {
                        if (cursorLine_ < scrollStart_) cursorLine_ = scrollStart_;
                    }
                    else
                    {
                        if (cursorLine_ < 0) cursorLine_ = 0;
                    }
                    escPosition_ = -1;
                }
                else
                    Vterr();
            }
            else
            {
                if (escBuffer_[0] == '(')
                {
                    g0mode_ = byt;
                    escPosition_ = -1;
                    return;
                }
                if (escBuffer_[0] == ')')
                {
                    g1mode_ = byt;
                    escPosition_ = -1;
                    return;
                }
                Vterr();
            }
        break;

        case 'B':
            if (escBuffer_[0] == '[')
            {
                pBuffer = getInteger(&escBuffer_[1],&x);
                if (*pBuffer == byt)
                {
                    if (x == 0) x = 1;
                    cursorLine_ += x;
                    if (originMode_)
                    {
                        if (cursorLine_ >= scrollEnd_) cursorLine_ = scrollEnd_-1;
                    }
                    else
                    {
                        if (cursorLine_ >= rowsPerFrame_) cursorLine_ = rowsPerFrame_-1;
                    }
                    escPosition_ = -1;
                }
                else
                    Vterr();
            }
            else
            {
                if (escBuffer_[0] == '(')
                {
                    g0mode_ = byt;
                    escPosition_ = -1;
                    return;
                }
                if (escBuffer_[0] == ')')
                {
                    g1mode_ = byt;
                    escPosition_ = -1;
                    return;
                }
                Vterr();
            }
        break;

        case 'C':
            if (escBuffer_[0] == '[')
            {
                pBuffer = getInteger(&escBuffer_[1],&x);
                if (*pBuffer == byt)
                {
                    if (x == 0) x = 1;
                    cursorPosition_ += x;
                    if (cursorPosition_ >= (charactersPerRow_/doubleWidth_[cursorLine_])) cursorPosition_ = (charactersPerRow_/doubleWidth_[cursorLine_]) - 1;
                    escPosition_ = -1;
                }
                else
                    Vterr();
            }
            else
                Vterr();
        break;

        case 'D':
            if (escBuffer_[0] == '[')
            {
                pBuffer = getInteger(&escBuffer_[1],&x);
                if (*pBuffer == byt)
                {
                    if (x == 0) x = 1;
                    cursorPosition_ -= x;
                    if (cursorPosition_ < 0) cursorPosition_ = 0;
                    escPosition_ = -1;
                }
                else
                    Vterr();
            }
            else if (escBuffer_[0] == 'D')
            {
                cursorLine_++;
                if (cursorLine_ >= scrollEnd_)
                {
                    cursorLine_ = scrollEnd_-1;
                    ScrollUp();
                }
                escPosition_ = -1;
            }
            else
                Vterr();
        break;

        case 'E':
            if (escBuffer_[0] == 'E')
            {
                cursorLine_++;
                if (cursorLine_ >= scrollEnd_)
                {
                    cursorLine_ = scrollEnd_-1;
                    ScrollUp();
                }
                cursorPosition_ = 0;
                escPosition_ = -1;
            }
            else
                Vterr();
        break;

        case 'H':
            if (escBuffer_[0] == '[')
            {
                if (escBuffer_[1] == 'H')
                {
                    cursorPosition_ = 0;
                    if (originMode_)
                        cursorLine_ = scrollStart_;
                    else
                        cursorLine_ = 0;
                    escPosition_ = -1;
                    return;
                }
                pBuffer = getInteger(&escBuffer_[1],&y);
                if (*pBuffer == ';')
                {
                    pBuffer = getInteger(pBuffer+1,&x);
                    if (*pBuffer == byt)
                    {
                        if (x != 0)  x--;
                        if (y != 0)  y--;
                        if (*pBuffer == byt)
                        {
                            cursorPosition_ = (x < (charactersPerRow_/doubleWidth_[cursorLine_])) ? x : (charactersPerRow_/doubleWidth_[cursorLine_]) - 1;
                            if (originMode_)
                                cursorLine_ = (scrollStart_ + y < scrollEnd_) ? scrollStart_ + y : scrollEnd_-1;
                            else
                                cursorLine_ = (y < rowsPerFrame_) ? y : rowsPerFrame_-1;
                            escPosition_ = -1;
                            return;
                        }
                    }
                }
            }
            if (escBuffer_[0] == 'H')
            {
                tabs_.SetChar(cursorPosition_, 'T');
                escPosition_ = -1;
                return;
            }
            Vterr();
        break;

        case 'J':
            if (escBuffer_[0] == '[')
            {
                if ((strcmp(escBuffer_,"[J") == 0) || (strcmp(escBuffer_,"[0J") == 0))
                {
                    for (i=cursorPosition_; i<(charactersPerRow_/doubleWidth_[cursorLine_]); i++)
                    {
                        writeMem(cursorLine_, i, 32);
                        reverseScr_[cursorLine_][i] = false;
                        underlineScr_[cursorLine_][i] = false;
                        highlightScr_[cursorLine_][i] = false;
                        blinkScr_[cursorLine_][i] = false;
                        drawCharacter(i, cursorLine_, 32, false);
                    }
                    for (y=cursorLine_+1; y<rowsPerFrame_; y++)
                    {
                        doubleWidth_[y] = 1;
                        doubleHeight_[y] = 1;

                        for (x=0; x<(charactersPerRow_/doubleWidth_[y]); x++)
                        {
                            writeMem(y, x, 32);
                            reverseScr_[y][x] = false;
                            underlineScr_[y][x] = false;
                            highlightScr_[y][x] = false;
                            blinkScr_[y][x] = false;
                            drawCharacter(x, y, 32, false);
                        }
                    }
                    escPosition_ = -1;
                }
                else if (strcmp(escBuffer_,"[1J") == 0)
                {
                    for (y=0; y<cursorLine_; y++)
                    {
                        doubleWidth_[y] = 1;
                        doubleHeight_[y] = 1;
                        for (x=0; x<(charactersPerRow_/doubleWidth_[y]); x++)
                        {
                            writeMem(y, x, 32);
                            reverseScr_[y][x] = false;
                            underlineScr_[y][x] = false;
                            highlightScr_[y][x] = false;
                            blinkScr_[y][x] = false;
                            drawCharacter(x, y, 32, false);
                        }
                    }
                    for (i=0; i<=cursorPosition_; i++)
                    {
                        writeMem(cursorLine_, i, 32);
                        reverseScr_[cursorLine_][i] = false;
                        underlineScr_[cursorLine_][i] = false;
                        highlightScr_[cursorLine_][i] = false;
                        blinkScr_[cursorLine_][i] = false;
                        drawCharacter(i, cursorLine_, 32, false);
                    }
                    escPosition_ = -1;
                }
                else if (strcmp(escBuffer_,"[2J") == 0)
                {
                    for (y=0; y<rowsPerFrame_; y++)
                    {
                        doubleWidth_[y] = 1;
                        doubleHeight_[y] = 1;
                        for (x=0; x<(charactersPerRow_/doubleWidth_[y]); x++)
                        {
                            writeMem(y, x, 32);
                            reverseScr_[y][x] = false;
                            underlineScr_[y][x] = false;
                            highlightScr_[y][x] = false;
                            blinkScr_[y][x] = false;
                            drawCharacter(x, y, 32, false);
                        }
                    }
                    escPosition_ = -1;
                }
                else Vterr();
            }
            else 
                Vterr();
        break;

        case 'K':
            if (byt == 'K' && escBuffer_[0] == '[')
            {
                if ((strcmp(escBuffer_,"[K") == 0) || (strcmp(escBuffer_,"[0K") == 0))
                {
                    for (i=cursorPosition_; i<(charactersPerRow_/doubleWidth_[cursorLine_]); i++)
                    {
                        writeMem(cursorLine_, i, 32);
                        reverseScr_[cursorLine_][i] = false;
                        underlineScr_[cursorLine_][i] = false;
                        highlightScr_[cursorLine_][i] = false;
                        blinkScr_[cursorLine_][i] = false;
                        drawCharacter(i, cursorLine_, 32, false);
                    }
                    escPosition_ = -1;
                }
                else if (strcmp(escBuffer_,"[1K") == 0)
                {
                    for (i=0; i<=cursorPosition_; i++)
                    {
                        writeMem(cursorLine_, i, 32);
                        reverseScr_[cursorLine_][i] = false;
                        underlineScr_[cursorLine_][i] = false;
                        highlightScr_[cursorLine_][i] = false;
                        blinkScr_[cursorLine_][i] = false;
                        drawCharacter(i, cursorLine_, 32, false);
                    }
                    escPosition_ = -1;
                }
                else if (strcmp(escBuffer_,"[2K") == 0)
                {
                    for (i=0; i<(charactersPerRow_/doubleWidth_[cursorLine_]); i++)
                    {
                        writeMem(cursorLine_, i, 32);
                        reverseScr_[cursorLine_][i] = false;
                        underlineScr_[cursorLine_][i] = false;
                        highlightScr_[cursorLine_][i] = false;
                        blinkScr_[cursorLine_][i] = false;
                        drawCharacter(i, cursorLine_, 32, false);
                    }
                    escPosition_ = -1;
                }
                else
                    Vterr();
            }
            else 
                Vterr();
        break;

        case 'M':
            if (escBuffer_[0] == 'M')
            {
                cursorLine_ --;
                if (cursorLine_ < scrollStart_)
                {
                    cursorLine_ = scrollStart_;
                    ScrollDown();
                }
                cursorPosition_ = 0;
                escPosition_ = -1;
            }
            else 
                Vterr();
        break;

        case 'Z':
        case 'c':
            if ((escBuffer_[0] == '[' && ((escBuffer_[1] == '0' && escBuffer_[2] == 'c') || escBuffer_[1] == 'c')) || escBuffer_[0] == 'Z')
            {
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('[');
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('1');
                videoScreenPointer->vtOut(';');
                videoScreenPointer->vtOut('2');
                videoScreenPointer->vtOut('c');
                escPosition_ = -1;
            }
            else if (escBuffer_[0] == 'c')
            {
                setupLoad();
                escPosition_ = -1;
            }
            else
                Vterr();
        break;

        case 'f':
            if (escBuffer_[0] == '[')
            {
                if (escBuffer_[1] == 'f')
                {
                    cursorPosition_ = 0;
                    if (originMode_)
                        cursorLine_ = scrollStart_;
                    else
                        cursorLine_ = 0;
                    escPosition_ = -1;
                    return;
                }
                pBuffer = getInteger(&escBuffer_[1],&y);
                if (*pBuffer == ';')
                {
                    pBuffer = getInteger(pBuffer+1,&x);
                    if (*pBuffer == byt)
                    {
                        if (x != 0)  x--;
                        if (y != 0)  y--;
                        if (*pBuffer == byt)
                        {
                            cursorPosition_ = (x < (charactersPerRow_/doubleWidth_[cursorLine_])) ? x : (charactersPerRow_/doubleWidth_[cursorLine_]) - 1;
                            if (originMode_)
                                cursorLine_ = (scrollStart_ + y < scrollEnd_) ? scrollStart_ + y : scrollEnd_-1;
                            else
                                cursorLine_ = (y < rowsPerFrame_) ? y : rowsPerFrame_-1;
                            escPosition_ = -1;
                            return;
                        }
                    }
                }
            }
            Vterr();
        break;

        case 'g':
            if (escBuffer_[0] == '[' && ((escBuffer_[1] == '0' && escBuffer_[2] == 'g') || escBuffer_[1] == 'g'))
            {
                tabs_.SetChar(cursorPosition_, ' ');
                escPosition_ = -1;
            }
            else if (escBuffer_[0] == '[' && (escBuffer_[1] == '3' && escBuffer_[2] == 'g'))
            {
                for (x=0; x<132; x++)
                {
                    tabs_.SetChar(cursorPosition_, ' ');
                    escPosition_ = -1;
                }
            }
            else
                Vterr();
        break;

        case 'h':
            error = true;
            if (escBuffer_[0] == '[')
            {
                bufPos = 1;
                while ((bufPos+2) <= escPosition_)
                {
                    error = true;
                    if ((escBuffer_[bufPos] == '?') && (escBuffer_[bufPos+2] == ';' || escBuffer_[bufPos+2] == 'h'))
                    {
                        switch (escBuffer_[bufPos+1])
                        {
                            case '1':
                                cursorKeyMode_ = true;
                                error = false;
                            break;

                            case '3':
                                charactersPerRow_ = 132;
                                charWidth_ = 8;
                                reDraw_ = true;
                                videoWidth_ = charactersPerRow_*charWidth_;
                                setScreenSize();
                                for (int y_temp=0; y_temp<rowsPerFrame_; y_temp++)
                                    for (int x_temp=0; x_temp<charactersPerRow_; x_temp++)
                                    {
                                        reverseScr_[y_temp][x_temp] = false;
                                        underlineScr_[y_temp][x_temp] = false;
                                        highlightScr_[y_temp][x_temp] = false;
                                        blinkScr_[y_temp][x_temp] = false;
                                        writeMem(y_temp, x_temp, 32);
                                    }
                                cursorPosition_ = 0;
                                cursorLine_ = 0;
                                error = false;
                            break;

                            case '5':
                                SetUpFeature_[VTREVERSESCREEN] = true;
                                reDraw_ = true;
                                error = false;
                            break;

                            case '6':
                                originMode_ = true;
                                cursorPosition_ = 0;
                                cursorLine_ = scrollStart_;                            
                                error = false;
                            break;

                            case '7':
                                SetUpFeature_[VTWRAPAROUND] = 1;
                                error = false;
                            break;

                            case '8':
                                SetUpFeature_[VTREPEAT] = 1;
                                videoScreenPointer->setRepeat(SetUpFeature_[VTREPEAT]);
                                error = false;
                            break;

                            case '9':
                                SetUpFeature_[VTINTERLACE] = 1;
                                reDraw_ = true;
                                error = false;
                            break;
                        }
                    }
                    if ((escBuffer_[bufPos] == '2') && (escBuffer_[bufPos+1] == '0') && (escBuffer_[bufPos+2] == ';' || escBuffer_[bufPos+2] == 'h'))
                    {
                        SetUpFeature_[VTNEWLINE] = 1;
                        error = false;
                    }
                    bufPos+=3;
                }
            }
            if (error)
                Vterr();
            else
                escPosition_ = -1;
        break;

        case 'l':
            error = true;
            if (escBuffer_[0] == '[')
            {
                bufPos = 1;
                while ((bufPos+2) <= escPosition_)
                {
                    error = true;
                    if ((escBuffer_[bufPos] == '?') && (escBuffer_[bufPos+2] == ';' || escBuffer_[bufPos+2] == 'l'))
                    {
                        switch (escBuffer_[bufPos+1])
                        {
                            case '1':
                                cursorKeyMode_ = false;
                                error = false;
                            break;

                            case '2':
                                SetUpFeature_[VTANSI] = 0;
                                error = false;
                            break;

                            case '3':
                                charactersPerRow_ = 80;
                                charWidth_ = 10;
                                videoWidth_ = charactersPerRow_*charWidth_;
                                reDraw_ = true;
                                setScreenSize();
                                for (int y_temp=0; y_temp<rowsPerFrame_; y_temp++)
                                    for (int x_temp =0; x_temp<charactersPerRow_; x_temp++)
                                    {
                                        reverseScr_[y_temp][x_temp] = false;
                                        underlineScr_[y_temp][x_temp] = false;
                                        highlightScr_[y_temp][x_temp] = false;
                                        blinkScr_[y_temp][x_temp] = false;
                                        writeMem(y_temp, x_temp, 32);
                                    }
                                cursorPosition_ = 0;
                                cursorLine_ = 0;
                                error = false;
                            break;

                            case '5':
                                SetUpFeature_[VTREVERSESCREEN] = false;
                                reDraw_ = true;
                                error = false;
                            break;

                            case '6':
                                originMode_ = true;
                                cursorPosition_ = 0;
                                cursorLine_ = scrollStart_;                            
                                error = false;
                            break;

                            case '7':
                                SetUpFeature_[VTWRAPAROUND] = 0;
                                error = false;
                            break;

                            case '8':
                                SetUpFeature_[VTREPEAT] = 0;
                                videoScreenPointer->setRepeat(SetUpFeature_[VTREPEAT]);
                                error = false;
                            break;

                            case '9':
                                SetUpFeature_[VTINTERLACE] = 0;
                                reDraw_ = true;
                                error = false;
                            break;
                        }
                    }
                    if ((escBuffer_[bufPos] == '2') && (escBuffer_[bufPos+1] == '0') && (escBuffer_[bufPos+2] == ';' || escBuffer_[bufPos+2] == 'l'))
                    {
                        SetUpFeature_[VTNEWLINE] = 0;
                        error = false;
                    }
                    bufPos+=3;
                }
            }
            if (error)
                Vterr();
            else
                escPosition_ = -1;
        break;

        case 'm':
            error = true;
            if (escBuffer_[0] == '[')
            {
                if (escBuffer_[1] == 'm')
                {
                    highlight_ = false;
                    underline_ = false;
                    blink_ = false;
                    reverse_ = false;
                    escPosition_ = -1;
                    return;
                }
                bufPos = 1;
                while ((bufPos+1) <= escPosition_)
                {
                    error = true;
                    if (escBuffer_[bufPos+1] == ';' || escBuffer_[bufPos+1] == 'm')
                    {
                        switch (escBuffer_[bufPos])
                        {
                            case '0':
                                highlight_ = false;
                                underline_ = false;
                                blink_ = false;
                                reverse_ = false;
                                error = false;
                            break;

                            case '1':
                                highlight_ = true;
                                error = false;
                            break;

                            case '4':
                                underline_ = true;
                                error = false;
                            break;

                            case '5':
                                blink_ = true;
                                error = false;
                            break;

                            case '7':
                                reverse_ = true;
                                error = false;
                            break;
                        }
                    }
                    bufPos+=2;
                }
            }
            if (error)
                Vterr();
            else
                escPosition_ = -1;
        break;

        case 'n':
            if ((escBuffer_[0] == '[') && (escBuffer_[2] == 'n')) 
            {
                switch (escBuffer_[1])
                {
                    case '5':
                        videoScreenPointer->vtOut(27);
                        videoScreenPointer->vtOut('[');
                        videoScreenPointer->vtOut('0');
                        videoScreenPointer->vtOut('n');
                        escPosition_ = -1;
                    return;

                    case '6':
                        videoScreenPointer->vtOut(27);
                        videoScreenPointer->vtOut('[');
                        if (originMode_)
                            buffer.Printf("%d", cursorLine_+1-scrollStart_);
                        else
                            buffer.Printf("%d", cursorLine_+1);
                        for (size_t bufPos_temp=0; bufPos_temp<buffer.Len(); bufPos_temp++)
                        {
                            videoScreenPointer->vtOut(buffer.GetChar(bufPos_temp));
                        }
                        videoScreenPointer->vtOut(';');
                        buffer.Printf("%d", cursorPosition_+1);
                        for (size_t bufPos_temp =0; bufPos_temp<buffer.Len(); bufPos_temp++)
                        {
                            videoScreenPointer->vtOut(buffer.GetChar(bufPos_temp));
                        }
                        videoScreenPointer->vtOut('R');
                        escPosition_ = -1;
                    return;
                }
            }
            Vterr();
        break;

        case 'r':
            if (escBuffer_[0] == '[')
            {
                if (escBuffer_[1] == 'r')
                {
                    scrollStart_ = 0;
                    scrollEnd_ = rowsPerFrame_;
                    
                    cursorPosition_ = 0;
                    cursorLine_ = 0;
                    escPosition_ = -1;
                    return;
                }
                pBuffer = getInteger(&escBuffer_[1],&y);
                if (*pBuffer == ';')
                {
                    pBuffer = getInteger(pBuffer+1,&x);
                    if (*pBuffer == byt && x > y)
                    {
                        scrollStart_ = y;
                        scrollEnd_ = x;
                        
                        cursorPosition_ = 0;
                        cursorLine_ = scrollStart_;
                        escPosition_ = -1;
                        return;
                    }
                }
            }
            Vterr();
        break;

        case 's':
            if (escBuffer_[0] == '[')
            {
                if (escPosition_ == 2)
                {
                    savePosition_ = cursorPosition_;
                    saveLine_ = cursorLine_;
                    escPosition_ = -1;
                }
                else Vterr();
            }
            else 
                Vterr();
        break;

        case 'u':
            if (escBuffer_[0] == '[')
            {
                if (escPosition_ == 2)
                {
                    cursorPosition_ = savePosition_;
                    cursorLine_ = saveLine_;
                    escPosition_ = -1;
                }
                else Vterr();
            }
            else 
                Vterr();
        break;

        case 'x':
            if ((escBuffer_[0] == '[') && ((escBuffer_[1] == 'x') || (escBuffer_[2] == 'x'))) 
            {
                switch (escBuffer_[1])
                {
                    case 'x':
                    case '0':
                    case '1':
                        videoScreenPointer->vtOut(27);
                        videoScreenPointer->vtOut('[');
                        videoScreenPointer->vtOut('3');
                        videoScreenPointer->vtOut(';');
                        videoScreenPointer->vtOut('1');
                        videoScreenPointer->vtOut(';');
                        videoScreenPointer->vtOut('1');
                        videoScreenPointer->vtOut(';');
                        buffer.Printf("%d", baudRateCode_[selectedBaudT_]);
                        for (size_t bufPos_temp=0; bufPos_temp<buffer.Len(); bufPos_temp++)
                        {
                            videoScreenPointer->vtOut(buffer.GetChar(bufPos_temp));
                        }
                        videoScreenPointer->vtOut(';');
                        buffer.Printf("%d", baudRateCode_[selectedBaudR_]);
                        for (size_t bufPos_temp =0; bufPos_temp<buffer.Len(); bufPos_temp++)
                        {
                            videoScreenPointer->vtOut(buffer.GetChar(bufPos_temp));
                        }
                        videoScreenPointer->vtOut('x');
                        escPosition_ = -1;
                    return;
                }
            }
            Vterr();
        break;

        case '=':
            if (escBuffer_[0] == '=')
            {
                AltKeypad_ = true;
                escPosition_ = -1;
            }
            else 
                Vterr();
        break;

        case '>':
            if (escBuffer_[0] == '>')
            {
                AltKeypad_ = false;
                escPosition_ = -1;
            }
            else 
                Vterr();
        break;

        case '[':
        case '9':
        case '#':
        case ';':
        case '?':
        case '(':
        case ')':
        break;

        default:
            Vterr();
        break;
    }
}

void Vt100::dataAvailable()
{
    if (!uart1854_ && !uart16450_)
        return;
    
    if (uartEf_)
        vtOutCount_ = baudRateT_ * 9;
    else
        vtOutCount_ = baudRateT_;
}

void Vt100::dataAvailable(Byte value)
{
    if (!uart1854_ && !uart16450_)
        return;
    
    vtOut_ = value;
    if (uartEf_)
    {
        if (vtOut_ == WXK_ESCAPE)
        {
            vtOut_ = 0;
            vtOutBits_ = (58240/baudRateT_)+11;
        }
        vtOutCount_ = baudRateT_ * 9;
    }
    else
        vtOutCount_ = baudRateT_;

    uartStatus_[uart_da_bit_] = 1;
}

void Vt100::dataAvailableUart(bool data)
{
    lineStatusRegister_[UART_LSR_DR] = data;
    uartStatus_[uart_da_bit_] = data;
    if (data)
        uartInterrupt();
}

void Vt100::framingError(bool data)
{
    uartStatus_[uart_fe_bit_] = data;
}

void Vt100::selectUart16450Register(Byte value)
{
    registerSelect_ = value &0x7;
}

void Vt100::uartCts(Byte value)
{
    switch (value)
    {
        case 1:
            if (terminalLoad_)
                dataAvailableUart(0);
            clearToSend_ = false;
        break;

        case 2:
            if (terminalLoad_ && sendPacket_)
                dataAvailableUart(1);
            clearToSend_ = true;
        break;
    }
}

void Vt100::uartOut(Byte value)
{
    rs232_ = value;
    p_Computer->thrStatusVt100(1);
    uartStatus_[uart_thre_bit_] = 0;
    uartStatus_[uart_tsre_bit_] = 0;
    receivePacket_ = true;
}

void Vt100::uart16450Out(Byte value)
{
    switch (registerSelect_)
    {
        case 0: // THR
            if (modemControlRegister_[UART_MCR_LOOP])
            {
                thr_ = value;
                lineStatusRegister_[UART_LSR_DR] = 1;
            }
            else
                uartOut(value);
        break;

        case 4: // MCR
            modemControlRegister_ = value;
            if (modemControlRegister_[UART_MCR_LOOP])
            {
                modemStatusRegister_[UART_MSR_CTS] = modemControlRegister_[UART_MCR_RTS];
                modemStatusRegister_[UART_MSR_DSR] = modemControlRegister_[UART_MCR_DTR];
                modemStatusRegister_[UART_MSR_RI] = modemControlRegister_[UART_MCR_OUT1];
                modemStatusRegister_[UART_MSR_CD] = modemControlRegister_[UART_MCR_OUT2];
            }
            else
                modemStatusRegister_ = 0;
        break;
    }
}

void Vt100::uartControl(Byte value)
{
    if ((value & 0x80) == 0x80)
    {
        uartInterrupt();
    }
    else
    {
        uartControl_ = value;
        clearUartInterrupt();
    }
    
    uartStatus_[uart_thre_bit_] = 1;

    if (terminalLoad_ && uartStatus_[uart_da_bit_] && clearToSend_)
        dataAvailableUart(1);
}

Byte Vt100::uartIn()
{
    framingError(0);
    dataAvailableUart(0);

    if (ctrlvText_ != 0)
    {
        videoScreenPointer->getKey(0);
        return checkCtrlvTextUart();
    }
    else
    {
        Byte loadByte = 0;
        if (terminalLoad_ || (terminalSave_ && (protocol_ == TERM_XMODEM_SAVE || protocol_ == TERM_YMODEM_SAVE)))
        {
            getTerminalLoadByte(&loadByte);
            return loadByte;
        }
        else
            return videoScreenPointer->getKey(0);
    }
}

Byte Vt100::uart16450In()
{
    switch (registerSelect_)
    {
        case 0: // RHR
            if (modemControlRegister_[UART_MCR_LOOP])
            {
                lineStatusRegister_[UART_LSR_DR] = 0;
                return thr_;
            }
            else
                return uartIn();
        break;

        case 5: // LSR
            return lineStatusRegister_.to_ulong();
        break;

        case 6: // MSR
            return modemStatusRegister_.to_ulong();
        break;

        default:
            return 0;
        break;
    }
}

Byte Vt100::uartStatus()
{
    clearUartInterrupt();
    return uartStatus_.to_ulong();
}

Byte Vt100::uartThreStatus()
{
    return uartStatus_[uart_thre_bit_];
}

void Vt100::thrStatusUart16450(bool data)
{
    lineStatusRegister_[UART_LSR_THRE] = !data;
    lineStatusRegister_[UART_LSR_TRE] = data;
}

void Vt100::uartInterrupt()
{
    if (uart16450_)
        return;
    
    if ((uartControl_ & 0x20) == 0x20 && currentComputerConfiguration.videoTerminalConfiguration.interrupt)
        p_Computer->requestInterrupt(INTERRUPT_TYPE_UART, true, currentComputerConfiguration.videoTerminalConfiguration.picInterrupt);
    if (currentComputerConfiguration.videoTerminalConfiguration.efInterrupt.flagNumber != -1)
        vt100EfInterrupt_ = 0;
}

void Vt100::clearUartInterrupt()
{
    if (uart16450_)
        return;
    
    p_Computer->requestInterrupt(INTERRUPT_TYPE_UART, false, currentComputerConfiguration.videoTerminalConfiguration.picInterrupt);
    if (currentComputerConfiguration.videoTerminalConfiguration.efInterrupt.flagNumber != -1)
        vt100EfInterrupt_ = 1;
}

void Vt100::getKey()
{
    if ((terminalLoad_ || terminalSave_) && !terminalFileCdp18s020_)
        return;
    
    if (!currentComputerConfiguration.videoTerminalConfiguration.show)
        return;
    
    if (vtOut_ <= 0)
    {
        if (elfRunCommand_ != 0)
        {
            checkXmlCommand();
        }
        else
        {
            if (ctrlvText_ != 0)
            {
                if (p_Computer->checkKeyInputAddress())
                    checkCtrlvText();
            }
            else if (vtOutCount_ == -1)
                vtOut_ = videoScreenPointer->getKey(vtOut_);
        }
        if (vtOut_ != 0 && cursorPosition_ == (charactersPerRow_ - 9) && vtOut_ != 8)
            bell();
        if (SetUpFeature_[VTLOCALECHO] && vtOut_ > 0)
            Display(vtOut_ & 0x7f, false);
    }
}

bool Vt100::checkInReleaseAddress(Word address)
{
    for (std::vector<Word>::iterator i = addressLocations_.inReleaseAddress.begin (); i != addressLocations_.inReleaseAddress.end (); ++i)
    {
        if (address == *i)
            return true;
    }
    return false;
}

void Vt100::checkCtrlvText()
{
    if (ctrlvText_ <= commandText_.Len())
    {
        vtOut_ = commandText_.GetChar(ctrlvText_ - 1);
        if (vtOut_ == 10)
            vtOut_ = 13;
        ctrlvText_++;
    }
    else
        ctrlvText_ = 0;
}

Byte Vt100::checkCtrlvTextUart()
{
    Byte key = 0;
    
    key = commandText_.GetChar(ctrlvText_ - 1);
    if (key == 10)
        key = 13;
    ctrlvText_++;

    if (ctrlvText_ <= commandText_.Len())
    {
        uartStatus_[uart_da_bit_] = 1;
        dataAvailableUart(1);
    }
    else
        ctrlvText_ = 0;

    return key;
}

void Vt100::checkXmlCommand()
{
    if (checkInReleaseAddress(p_Computer->getScratchpadRegister(p_Computer->getProgramCounter())))
        p_Computer->onInButtonRelease();

    if (p_Computer->checkKeyInputAddress())
    {
        if (elfRunCommand_ == 1 || commandText_ != "")
        {
            vtOut_ = commandText_.GetChar(0);
            commandText_ = commandText_.Right(commandText_.Len()-1);
            if (commandText_.Len() == 0)
                elfRunCommand_++;
        }
        else if (elfRunCommand_ == 2)
        {
            int saveExec = p_Main->pload();
            if (saveExec == -1)
                elfRunCommand_ = 0;
            else
            {
                if (p_Main->isBatchConvertActive())
                {
                    commandText_ = saveCommand_ + "\r";
                }
                else
                {
                    if (!load_)
                    {
                        if (saveExec == 0)
                            commandText_ = "RUN\r";
                        else
                        {
                            wxString buffer;
                            buffer.Printf("%04x", saveExec);
                            commandText_ = "PR CALL(@" + buffer + ")\r";
                        }
                    }
                    else
                        elfRunCommand_ = 0;
                }
                elfRunCommand_++;
            }
        }
        else
        {
            elfRunCommand_ = 0;
        }
    }
}

void Vt100::ResetVt()
{
    escPosition_ = -1;
    vtEnabled_ = 1;
}

void Vt100::ResetIo()
{
/*    xOff_ = false;
    escPosition_ = -1;
    vtEnabled_ = 1;
    vtCount_ = -1;
    vtOutCount_ = -1;
    vtOut_ = 0;
    vt100Ef_ = 1;
    elfRunCommand_ = 0;*/
}

void Vt100::setForceUCVt(bool status)
{
    videoScreenPointer->setForceUCVt(status);
}

void Vt100::setSendPacket(bool status)
{
    sendPacket_ = status;
    p_Computer->setSendPacket(status);
}

void Vt100::setTerminalSave(bool status)
{
    terminalSave_ = status;
    p_Computer->setTerminalSave(status);
}

void Vt100::terminalSaveVt(wxString fileName, int protocol)
{
    p_Main->turboOn();
    if (!fileName.empty())
    {
        if (outputTerminalFile.Create(fileName, true))
        {
            setTerminalSave(true);
            terminalLine_ = "";
            terminalAck_ = XMODEM_NAK;
            xmodemBuffer_[0] = 0;
            receivePacket_ = false;
            xmodemBufferSize_ = 132;
            protocol_ = protocol;
            if (!uart1854_ && !uart16450_)
                p_Computer->setNotReadyToReceiveData(dataReadyFlag_-1);
            else
                dataAvailableUart(1);
            previousByte_ = 0;
        }
    }
}

void Vt100::terminalYsSaveVt(wxString fileName, int protocol)
{
    p_Main->turboOn();

    setTerminalSave(true);
    terminalLine_ = "";
    terminalAck_ = XMODEM_CRC;
    useCrc_ = true;
    directoryName_ = fileName;
    xmodemBuffer_[0] = 0;
    receivePacket_ = false;
    xmodemBufferSize_ = 133;
    protocol_ = protocol;
    xmodemBufferPointer_ = 0;
    if (!uart1854_ && !uart16450_)
        p_Computer->setNotReadyToReceiveData(dataReadyFlag_-1);
    else
        dataAvailableUart(1);
    previousByte_ = 0;
}

void Vt100::terminalSaveCdp18s020Vt(wxString fileName, int protocol)
{
    if (!fileName.empty())
    {
        if (outputTerminalFile.Create(fileName, true))
        {
            setTerminalSave(true);
            protocol_ = protocol;
            terminalFileCdp18s020_ = true;
            lastByte_ = -1;
            terminalLine_ = "";
        }
    }
}

void Vt100::setTerminalLoad(bool status)
{
    terminalLoad_ = status;
    p_Computer->setTerminalLoad(status);
}

void Vt100::terminalLoadVt(wxString fileName, int protocol)
{
    p_Main->turboOn();
    if (!fileName.empty())
    {
        if (inputTerminalFile.Open(fileName, _("rb")))
        {
            fileSize_ = inputTerminalFile.Length();
            setTerminalLoad(true);
            setSendPacket(false);
            xmodemBufferSize_ = 132;
            protocol_ = protocol;
            if (!uart1854_ && !uart16450_)
                p_Computer->setNotReadyToReceiveData(dataReadyFlag_-1);
            previousByte_ = 0;
            xmodemBufferPointer_ = xmodemBufferSize_;
            xmodemPacketNumber_  = 0;
            xmodemFileNumber_ = 1;
            sendingMode_ = YMODEM_HEADER;
            receivePacket_ = false;
        }
    }
}

void Vt100::terminalLoadCdp18s020Vt(wxString fileName, int protocol)
{
    if (!fileName.empty())
    {
        if (inputTerminalFile.Open(fileName, _("rb")))
        {
            setTerminalLoad(true);
            terminalFileCdp18s020_ = true;
            protocol_ = protocol;
            p_Computer->setNotReadyToReceiveData(dataReadyFlag_-1);
            previousByte_ = 0;
        }
    }
}

void Vt100::terminalStopVt()
{
    p_Main->turboOff();
    terminalFileCdp18s020_ = false;
    if (terminalSave_)
    {
        setTerminalSave(false);
        outputTerminalFile.Close();
    }
    if (terminalLoad_)
    {
        setTerminalLoad(false);
        inputTerminalFile.Close();
    }
}

bool Vt100::readCharRomFile(wxString romDir, wxString romFile)
{
    wxFFile inFile;
    size_t length;
    char buffer[4096];

    for (size_t i=0; i<4096; i++)
    {
        charRom_[i] = 0;
    }

    if (romFile == "")
    {
        p_Main->errorMessage("No font filename specified");
        return false;
    }

    wxString fileName = romDir + romFile;

    if (!wxFile::Exists(fileName))
        p_Main->reInstallOnNotFound("VT Font");

    if (inFile.Open(fileName, "rb"))
    {
        length = inFile.Read(buffer, 4096);
        for (size_t i=0; i<length; i++)
            charRom_[i] = (Byte)buffer[i];
        inFile.Close();
        return true;
    }
    else
    {
        p_Main->errorMessage("Error reading " + fileName);
        return false;
    }
}

void Vt100::displayBuffer(int value)
{
    if ((displayEnd_ == displayStart_-1) || (displayEnd_ == (VTBUFFER - 1) && displayStart_ == 0))
    { 
        if (!xOff_)
        {
            xOff_ = true;
            videoScreenPointer->vtOut(19);
        }
        Display(getDisplay(), true);
        while (displayBuffer_[displayStart_] != 10 && (displayStart_ != displayEnd_))
        {
            Display(getDisplay(), true);
        }
    }
    displayBuffer_[displayEnd_++] = value;
    if (displayEnd_ == VTBUFFER) displayEnd_ = 0;
}

Byte Vt100::getDisplay()
{
    if (displayStart_ == displayEnd_)
    {
        return 0;
    }
    Byte ret = displayBuffer_[displayStart_++];
    if (displayStart_ == VTBUFFER) displayStart_ = 0;
    return ret;
}

void Vt100::keyDownPressed(wxKeyEvent& event)
{
    if (SetUpFeature_[VTANSI])
        keyDownVT100(event);
    else
        keyDownVT52(event);
}

void Vt100::keyDownVT52(wxKeyEvent& event)
{
    int key = event.GetKeyCode();
    click();
    if (setUpMode_)
    {
        if (key == p_Main->getFunctionKey(8))
        {
            setUpMode_ = false;
            initScreen();
            cursorLine_ = 0;
            cursorPosition_ = 0;
            reDraw_ = true;
            if (xOff_)
            {
                xOff_ = false;
                videoScreenPointer->vtOut(17);
            }
            return;
        }
        else
        {
            pressedShift_ = (event.GetModifiers() == wxMOD_SHIFT);
            pressedCtrl_ = (event.GetModifiers() == wxMOD_CONTROL);
            pressedKey_ = key;
            return;
        }
    }
    switch (key)
    {
#ifdef __WXMAC__
        case WXK_DELETE:
            videoScreenPointer->vtOut(tab_char);
        return;
#endif
        case WXK_BACK:
            videoScreenPointer->vtOut(tab_char);
        return;
        case WXK_UP:
        case WXK_NUMPAD_UP:
            videoScreenPointer->vtOut(27);
            videoScreenPointer->vtOut('A');
        return;
        case WXK_DOWN:
        case WXK_NUMPAD_DOWN:
            videoScreenPointer->vtOut(27);
            videoScreenPointer->vtOut('B');
        return;
        case WXK_LEFT:
        case WXK_NUMPAD_LEFT:
            videoScreenPointer->vtOut(27);
            videoScreenPointer->vtOut('D');
        return;
        case WXK_RIGHT:
        case WXK_NUMPAD_RIGHT:
            videoScreenPointer->vtOut(27);
            videoScreenPointer->vtOut('C');
        return;
        case WXK_SCROLL:
            if (event.GetModifiers() == wxMOD_SHIFT)
                scrollScreen();
            else
                scrollLine();
        return;
    }
    if (key == p_Main->getFunctionKey(8))
    {
        if (vtType_ == VT100)
        {
            if (!xOff_)
            {
                xOff_ = true;
                videoScreenPointer->vtOut(19);
            }
            setUpMode_ = true;
            setUpModeB_ = false;
            initScreen();
            setupAScreen();
            reDraw_ = true;
        }
        return;
    }
    if (AltKeypad_)
    {
        switch (key)
        {
            case WXK_NUMPAD0:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('p');
            return;
            case WXK_NUMPAD1:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('q');
            return;
            case WXK_NUMPAD2:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('r');
            return;
            case WXK_NUMPAD3:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('s');
            return;
            case WXK_NUMPAD4:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('t');
            return;
            case WXK_NUMPAD5:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('u');
            return;
            case WXK_NUMPAD6:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('v');
            return;
            case WXK_NUMPAD7:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('w');
            return;
            case WXK_NUMPAD8:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('x');
            return;
            case WXK_NUMPAD9:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('y');
            return;
            case WXK_NUMPAD_SUBTRACT:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('m');
            return;
            case WXK_NUMPAD_DIVIDE:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('l');
            return;
            case WXK_NUMPAD_DECIMAL:     
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('n');
            return;
            case WXK_NUMPAD_ENTER:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('?');
                videoScreenPointer->vtOut('M');
            return;
            case WXK_F1:
                if (event.GetModifiers() != wxMOD_SHIFT)
                    break;
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('P');
            return;
            case WXK_F2:
                if (event.GetModifiers() != wxMOD_SHIFT)
                    break;
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('Q');
            return;
            case WXK_F3:
                if (event.GetModifiers() != wxMOD_SHIFT)
                    break;
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('R');
            return;
            case WXK_F4:
                if (event.GetModifiers() != wxMOD_SHIFT)
                    break;
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('S');
            return;
        }
    }
    if (!p_Computer->keyDownPressed(key))
        event.Skip();
}

void Vt100::keyDownVT100(wxKeyEvent& event)
{
    int key = event.GetKeyCode();

    click();
    if (setUpMode_)
    {
        if (key == p_Main->getFunctionKey(8))
        {
            setUpMode_ = false;
            initScreen();
            cursorLine_ = 0;
            cursorPosition_ = 0;
            reDraw_ = true;
            if (xOff_)
            {
                xOff_ = false;
                videoScreenPointer->vtOut(17);
            }
            return;
        }
        else
        {
            pressedShift_ = (event.GetModifiers() == wxMOD_SHIFT);
            pressedKey_ = key;
            p_Computer->keyDownPressed(key);
            if (answerBack_)
                event.Skip();
            return;
        }
    }
    char cursorKeyChar;

    if (cursorKeyMode_)
        cursorKeyChar = '[';
    else
        cursorKeyChar = 'O';

    switch (key)
    {
        case WXK_BACK:
            videoScreenPointer->vtOut(tab_char);
        return;
            
        case 66:
            if (event.GetModifiers() == wxMOD_CONTROL)
            {
                for (size_t i=0; i<answerBackMessage_.Len()-1;i++)
                    videoScreenPointer->vtOut(answerBackMessage_.GetChar(i));
                return;
            }
        break;

        case WXK_RETURN:
            if (SetUpFeature_[VTNEWLINE])
            {
                videoScreenPointer->vtOut(13);
                videoScreenPointer->vtOut(10);
                return;
            }
        break;

        case WXK_UP:
        case WXK_NUMPAD_UP:
            videoScreenPointer->vtOut(27);
            videoScreenPointer->vtOut(cursorKeyChar);
            videoScreenPointer->vtOut('A');
        return;
        case WXK_DOWN:
        case WXK_NUMPAD_DOWN:
            videoScreenPointer->vtOut(27);
            videoScreenPointer->vtOut(cursorKeyChar);
            videoScreenPointer->vtOut('B');
        return;
        case WXK_LEFT:
        case WXK_NUMPAD_LEFT:
            videoScreenPointer->vtOut(27);
            videoScreenPointer->vtOut(cursorKeyChar);
            videoScreenPointer->vtOut('D');
        return;
        case WXK_RIGHT:
        case WXK_NUMPAD_RIGHT:
            videoScreenPointer->vtOut(27);
            videoScreenPointer->vtOut(cursorKeyChar);
            videoScreenPointer->vtOut('C');
        return;
        case WXK_SCROLL:
            if (scroll_ < 0)
                scroll_ = 0;
            else
                scroll_ = -1;
        return;
    }
    if (key == p_Main->getFunctionKey(8))
    {
        if (!xOff_)
        {
            xOff_ = true;
            videoScreenPointer->vtOut(19);
        }
        setUpMode_ = true;
        setUpModeB_ = false;
        initScreen();
        setupAScreen();
        reDraw_ = true;
    }
    if (AltKeypad_)
    {
        switch (key)
        {
            case WXK_NUMPAD0:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('p');
            return;
            case WXK_NUMPAD1:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('q');
            return;
            case WXK_NUMPAD2:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('r');
            return;
            case WXK_NUMPAD3:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('s');
            return;
            case WXK_NUMPAD4:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('t');
            return;
            case WXK_NUMPAD5:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('u');
            return;
            case WXK_NUMPAD6:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('v');
            return;
            case WXK_NUMPAD7:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('w');
            return;
            case WXK_NUMPAD8:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('x');
            return;
            case WXK_NUMPAD9:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('y');
            return;
            case WXK_NUMPAD_SUBTRACT:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('m');
            return;
            case WXK_NUMPAD_DIVIDE:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('l');
            return;
            case WXK_NUMPAD_DECIMAL:     
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('n');
            return;
            case WXK_NUMPAD_ENTER:
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('M');
            return;
            case WXK_F1:
                if (event.GetModifiers() != wxMOD_SHIFT)
                    break;
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('P');
            return;
            case WXK_F2:
                if (event.GetModifiers() != wxMOD_SHIFT)
                    break;
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('Q');
            return;
            case WXK_F3:
                if (event.GetModifiers() != wxMOD_SHIFT)
                    break;
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('R');
            return;
            case WXK_F4:
                if (event.GetModifiers() != wxMOD_SHIFT)
                    break;
                videoScreenPointer->vtOut(27);
                videoScreenPointer->vtOut('O');
                videoScreenPointer->vtOut('S');
            return;
        }
    }
    if (!p_Computer->keyDownPressed(key))
        event.Skip();
}

void Vt100::setUpA(int key)
{
    RemoveCursor();
    switch (key)
    {
        case WXK_UP:
        case WXK_NUMPAD_UP:
            intensity_+= 0.01;
            if (intensity_ > 1)  
                intensity_ = 1;
            else
            {
                reDraw_ = true;
                setForeColour();
            }
        break;
        case WXK_DOWN:
        case WXK_NUMPAD_DOWN:
            intensity_-= 0.01;
            if (intensity_ < 0.30) 
                intensity_ = 0.30;
            else
            {
                reDraw_ = true;
                setForeColour();
            }
        break;
        case WXK_LEFT:
        case WXK_NUMPAD_LEFT:
            cursorPosition_--;
            if (cursorPosition_ < 0) 
                cursorPosition_ = 0;
        break;
        case WXK_RIGHT:
        case WXK_NUMPAD_RIGHT:
        case WXK_SPACE:
            cursorPosition_++;
            if (cursorPosition_ >= charactersPerRow_) 
                cursorPosition_ = charactersPerRow_ - 1;
        break;
        case WXK_TAB:
            if (cursorPosition_< (charactersPerRow_ - 1))
                cursorPosition_++;
            while ((tabs_.GetChar(cursorPosition_) == ' ') && (cursorPosition_< (charactersPerRow_ - 1)))
            {
                cursorPosition_++;
            }
        break;
        case WXK_RETURN:
            cursorPosition_ = 0;
        break;
        case 50: // 2
            if (tabs_.GetChar(cursorPosition_) == ' ')
            {
                writeMem(cursorLine_, cursorPosition_, 'T');
                tabs_.SetChar(cursorPosition_, 'T');
                drawCharacter(cursorPosition_ , cursorLine_, 'T', false);
            }
            else
            {
                writeMem(cursorLine_, cursorPosition_, ' ');
                tabs_.SetChar(cursorPosition_, ' ');
                drawCharacter(cursorPosition_ , cursorLine_, 32, false);
            }
            cursorPosition_++;
            if (cursorPosition_ >= charactersPerRow_) 
                cursorPosition_ = charactersPerRow_ - 1;
        break;
        case 51: // 3
            for (int x=0; x<132; x++)
            {
                writeMem(cursorLine_, x, ' ');
                tabs_.SetChar(x, ' ');
                drawCharacter(x , cursorLine_, 32, false);
            }
        break;
        case 53: // 5
            setUpModeB_ = true;
            initScreen();
            setupBScreen();
            reDraw_ = true;
        break;
        case 57: //9
            if (charactersPerRow_ != 64)
            {
                if (charactersPerRow_ == 80)
                {
                    charactersPerRow_ = 132;
                    charWidth_ = 8;
                }
                else
                {
                    charactersPerRow_ = 80;
                    charWidth_ = 10;
                }
            }
            videoWidth_ = charactersPerRow_*charWidth_;
            reDraw_ = true;
            setScreenSize();
        break;
        case 82: //R
            if (pressedShift_)
            {
                initScreen();
                setupWaitScreen();
                wait_ = 20;
                setupLoad();
            }
        break;
        case 83: //S
            if (pressedShift_)
            {
                initScreen();
                setupWaitScreen();
                wait_ = 20;
                setupSave();
            }
        break;
    }
    ShowCursor();
    pressedKey_ = 0;
}

bool Vt100::charPressed(wxKeyEvent& event)
{
    int key = event.GetKeyCode();

    if (p_Main->getUseCtrlvKey())
    {
        if (key == p_Main->getCtrlvKey() && event.GetModifiers() == CTRL_V)
        {
            if (wxTheClipboard->Open())
            {
#ifndef __WXMAC__
                if (wxTheClipboard->IsSupported( wxDF_TEXT ))
                {
#endif
                    wxTextDataObject data;
                    wxTheClipboard->GetData( data );
                    commandText_ = data.GetText();
                    ctrlvText_ = 1;
#ifndef __WXMAC__
                }
#endif
                wxTheClipboard->Close();
                if (!uart1854_ && !uart16450_)
                    return true;
            }
        }
    }

//    click();
    if (setUpMode_)
    {
        if (setUpModeB_ && answerBack_)
        {
            RemoveCursor();
            if (key == answerBackDelimiter_)
            {
                answerBack_ = false;
                initScreen();
                setupBScreen();
                reDraw_ = true;
                p_Main->message(answerBackMessage_);
            }
            else
            {
                if (key < 128)
                {
                    if (cursorPosition_ == 2)
                        answerBackDelimiter_ = key;
                    else
                        answerBackMessage_.SetChar(cursorPosition_-3, key);
                    if (key < 32)  key = 1;
                    writeMem(23, cursorPosition_, key);
                    drawCharacter(cursorPosition_ , 23, key, false);
                    cursorPosition_++;
                    if (cursorPosition_ == 23) 
                        cursorPosition_ = 22;
                }
            }
            ShowCursor();
            pressedKey_ = 0;
            return true;
        }
    }
    return false;
}

void Vt100::setUpB(int key)
{
    if (answerBack_)
        return;

    int bit;
    RemoveCursor();
    switch (key)
    {
        case WXK_LEFT:
        case WXK_NUMPAD_LEFT:
            cursorPosition_--;
            if (cursorPosition_ < 0) 
                cursorPosition_ = 0;
        break;
        case WXK_RIGHT:
        case WXK_NUMPAD_RIGHT:
        case WXK_SPACE:
            cursorPosition_++;
            if (cursorPosition_ >= charactersPerRow_) 
                cursorPosition_ = charactersPerRow_ - 1;
        break;
        case WXK_TAB:
            if (cursorPosition_< (charactersPerRow_ - 1))
                cursorPosition_++;
            while ((tabs_.GetChar(cursorPosition_) == ' ') && (cursorPosition_< (charactersPerRow_ - 1)))
            {
                cursorPosition_++;
            }
        break;
        case WXK_RETURN:
            cursorPosition_ = 0;
        break;
        case 53: // 5
            setUpModeB_ = false;
            initScreen();
            setupAScreen();
            reDraw_ = true;
        break;
        case 54: // 6
            bit = 16;
            if (cursorPosition_ > 1 && cursorPosition_ < 6)  bit = cursorPosition_ - 2;
            if (cursorPosition_ > 9 && cursorPosition_ < 15)  bit = cursorPosition_ - 6;
            if (cursorPosition_ > 17 && cursorPosition_ < 22)  bit = cursorPosition_ - 10;
            if (cursorPosition_ > 25 && cursorPosition_ < 30)  bit = cursorPosition_ - 14;

            if (bit != 16)
            {
                bit = 15 - bit;
                if (SetUpFeature_[bit] == 1)
                {
                    SetUpFeature_[bit] = 0;
                    writeMem(24, cursorPosition_, 48);
                    drawCharacter(cursorPosition_ , 24, 48, false);
                }
                else
                {
                    SetUpFeature_[bit] = 1;
                    writeMem(24, cursorPosition_, 49);
                    drawCharacter(cursorPosition_ , 24, 49, false);
                }
            }
            videoScreenPointer->setRepeat(SetUpFeature_[VTREPEAT]);
            if (SetUpFeature_[VTPOWER])
                cycleSize_ = (int) (((clock_ * 1000000) / 8) / 50);
            else
                cycleSize_ = (int) (((clock_ * 1000000) / 8) / 60);
            reDraw_ = true;
        break;
        case 55: // 7
            selectedBaudT_--;
            if (selectedBaudT_ < 0)
                selectedBaudT_ = 15;
            baudRateT_ = baudRateFactor_[selectedBaudT_];
            setupBScreen();
            reDraw_ = true;
            p_Main->setBaud(selectedBaudR_, selectedBaudT_);
        break;
        case 56: // 8
            selectedBaudR_--;
            if (selectedBaudR_ < 0)
                selectedBaudR_ = 15;
            baudRateR_ = baudRateFactor_[selectedBaudR_];
            setupBScreen();
            reDraw_ = true;
            p_Main->setBaud(selectedBaudR_, selectedBaudT_);
        break;
        case 65: //A
            if (pressedShift_)
            {
                writeMem(23, 0, 'A');
                writeMem(23, 1, '=');
                answerBack_ = true;
                cursorPosition_ = 2;
                reDraw_ = true;
                answerBackMessage_ = "                    ";
                answerBackDelimiter_ = 0;
            }
        break;
        case 82: //R
            if (pressedShift_)
            {
                initScreen();
                setupWaitScreen();
                wait_ = 20;
                setupLoad();
            }
        break;
        case 83: //S
            if (pressedShift_)
            {
                initScreen();
                setupWaitScreen();
                wait_ = 20;
                setupSave();
            }
        break;
    }
    ShowCursor();
    pressedKey_ = 0;
    // 1 Scroll - repeat - screen reverse - cursor block line
    // 2 bell - keyklick - ansi/vt52 - xon/xoff
    // 3 US - wrap - newline - interlace
    // 4 parity sense - parity - bits per char - 60/50Hz
}

void Vt100::initScreen()
{
    for (int y=0; y<25; y++)
    {
        doubleWidth_[y] = 1;
        doubleHeight_[y] = 1;
        startLine_[y] = 0;
        for (int x=0; x<132; x++)
        {
            writeMem(y, x, 32);
            reverseScr_[y][x] = false;
            underlineScr_[y][x] = false;
            highlightScr_[y][x] = false;
            blinkScr_[y][x] = false;
        }
    }
}

void Vt100::setupAScreen()
{
    writeMem(0, 0, 'S');
    writeMem(0, 1, 'E');
    writeMem(0, 2, 'T');
    writeMem(0, 3, '-');
    writeMem(0, 4, 'U');
    writeMem(0, 5, 'P');
    writeMem(0, 6, ' ');
    writeMem(0, 7, 'A');
    writeMem(1, 0, 'S');
    writeMem(1, 1, 'E');
    writeMem(1, 2, 'T');
    writeMem(1, 3, '-');
    writeMem(1, 4, 'U');
    writeMem(1, 5, 'P');
    writeMem(1, 6, ' ');
    writeMem(1, 7, 'A');
    writeMem(2, 0, 'T');
    writeMem(2, 1, 'O');
    writeMem(2, 2, ' ');
    writeMem(2, 3, 'E');
    writeMem(2, 4, 'X');
    writeMem(2, 5, 'I');
    writeMem(2, 6, 'T');
    writeMem(2, 7, ' ');
    writeMem(2, 8, 'P');
    writeMem(2, 9, 'R');
    writeMem(2, 10, 'E');
    writeMem(2, 11, 'S');
    writeMem(2, 12, 'S');
    writeMem(2, 13, ' ');
    writeMem(2, 14, '"');
    writeMem(2, 15, 'S');
    writeMem(2, 16, 'E');
    writeMem(2, 17, 'T');
    writeMem(2, 18, '-');
    writeMem(2, 19, 'U');
    writeMem(2, 20, 'P');
    writeMem(2, 21, '"');

    for (int i=0; i<22; i++)
    {
        underlineScr_[2][i] = true;
    }
    bool reverse = true;
    for (int i=0; i<132; i++)
    {
        if (i%10 == 0) reverse = !reverse;
        writeMem(24, i, (i+1)%10+48);
        reverseScr_[24][i] = reverse;

        writeMem(23, i, tabs_.GetChar(i)); 
    }
    
    doubleWidth_[0] = doubleWidth_[1] = doubleWidth_[2] = 2;
    doubleHeight_[0] = doubleHeight_[1] = 0.5;
    startLine_[0] = 0;
    startLine_[1] = 5;

    cursorLine_ = 23;
    cursorPosition_ = 0;
}

void Vt100::setupBScreen()
{
    wxString buffer;

    writeMem(0, 0, 'S');
    writeMem(0, 1, 'E');
    writeMem(0, 2, 'T');
    writeMem(0, 3, '-');
    writeMem(0, 4, 'U');
    writeMem(0, 5, 'P');
    writeMem(0, 6, ' ');
    writeMem(0, 7, 'B');
    writeMem(1, 0, 'S');
    writeMem(1, 1, 'E');
    writeMem(1, 2, 'T');
    writeMem(1, 3, '-');
    writeMem(1, 4, 'U');
    writeMem(1, 5, 'P');
    writeMem(1, 6, ' ');
    writeMem(1, 7, 'B');
    writeMem(2, 0, 'T');
    writeMem(2, 1, 'O');
    writeMem(2, 2, ' ');
    writeMem(2, 3, 'E');
    writeMem(2, 4, 'X');
    writeMem(2, 5, 'I');
    writeMem(2, 6, 'T');
    writeMem(2, 7, ' ');
    writeMem(2, 8, 'P');
    writeMem(2, 9, 'R');
    writeMem(2, 10, 'E');
    writeMem(2, 11, 'S');
    writeMem(2, 12, 'S');
    writeMem(2, 13, ' ');
    writeMem(2, 14, '"');
    writeMem(2, 15, 'S');
    writeMem(2, 16, 'E');
    writeMem(2, 17, 'T');
    writeMem(2, 18, '-');
    writeMem(2, 19, 'U');
    writeMem(2, 20, 'P');
    writeMem(2, 21, '"');

    writeMem(24, 42, 'T');
    writeMem(24, 43, ' ');
    writeMem(24, 44, 'S');
    writeMem(24, 45, 'P');
    writeMem(24, 46, 'E');
    writeMem(24, 47, 'E');
    writeMem(24, 48, 'D');

    writeMem(24, 60, 'R');
    writeMem(24, 61, ' ');
    writeMem(24, 62, 'S');
    writeMem(24, 63, 'P');
    writeMem(24, 64, 'E');
    writeMem(24, 65, 'E');
    writeMem(24, 66, 'D');

    for (int i=0; i<22; i++)
    {
        underlineScr_[2][i] = true;
    }
    int x=0;
    int bit = 15;
    for (int i=0; i<4; i++)
    {
        writeMem(24, x, (i+1)+48);
        x+=2;
        for (int j=0; j<4; j++)
        {
            reverseScr_[24][x] = true;
            if (SetUpFeature_[bit]) 
                writeMem(24, x, 49);
            else
                writeMem(24, x, 48);
            x++;
            bit--;
        }
        x+=2;
    }

    buffer.Printf("%5d", baudRateValue_[selectedBaudT_]);
    for (size_t i=0; i<buffer.Len();i++)
    {
        writeMem(24, (int)(i+50), buffer.GetChar(i));
    }

    buffer.Printf("%5d", baudRateValue_[selectedBaudR_]);
    for (size_t i=0; i<buffer.Len();i++)
    {
        writeMem(24, (int)(i+68), buffer.GetChar(i));
    }

    doubleWidth_[0] = doubleWidth_[1] = doubleWidth_[2] = 2;
    doubleHeight_[0] = doubleHeight_[1] = 0.5;
    startLine_[0] = 0;
    startLine_[1] = 5;

    cursorLine_ = 23;
    cursorPosition_ = 0;
}

void Vt100::setupWaitScreen()
{
    writeMem(0, 0, 'W');
    writeMem(0, 1, 'A');
    writeMem(0, 2, 'I');
    writeMem(0, 3, 'T');
    writeMem(1, 0, 'W');
    writeMem(1, 1, 'A');
    writeMem(1, 2, 'I');
    writeMem(1, 3, 'T');
    doubleWidth_[0] = doubleWidth_[1] =  2;
    doubleHeight_[0] = doubleHeight_[1] = 0.5;
    startLine_[0] = 0;
    startLine_[1] = 5;
}

void Vt100::setupSave()
{
    p_Main->setConfigItem("Computer/VT100Tabs", tabs_);
    p_Main->setConfigItem("Computer/VT100AnswerBack", answerBackMessage_);

    p_Main->saveSetup(selectedBaudR_, selectedBaudT_, SetUpFeature_, charactersPerRow_, charWidth_);
    reDraw_ = true;
}

void Vt100::setupLoad()
{
    long value = currentComputerConfiguration.videoTerminalConfiguration.vt100DefaultSetUpFeature.to_ulong();
    SetUpFeature_ = p_Main->getConfigItem("Computer/VT100Setup/"+currentComputerConfiguration.xmlFileConfiguration.directory+currentComputerConfiguration.xmlFileConfiguration.fileName, value);
    tabs_ = p_Main->getConfigItem("Computer/VT100Tabs", "        T       T       T       T       T       T       T       T       T       T       T       T       T       T       T       T       T");
    answerBackMessage_ = p_Main->getConfigItem("Computer/VT100AnswerBack", "                    ");
    reDraw_ = true;
}

void Vt100::setForeColour()
{
/*    int blue, green;
    blue = intensity_-231;
    if (blue < 0)  blue = 0;
    green = intensity_-59;
    if (green < 0)  green = 0;*/
    colour_[colourIndex_] = wxColour(redFore_*intensity_, greenFore_*intensity_, blueFore_*intensity_);
    colour_[colourIndex_+1] = wxColour(redBack_*intensity_, greenBack_*intensity_, blueBack_*intensity_);
}

void Vt100::bell()
{
    if (SetUpFeature_[VTBELL])
    {
        if (currentComputerConfiguration.videoTerminalConfiguration.wavFileName != "")
            p_Computer->startWavSound(currentComputerConfiguration.videoTerminalConfiguration.wavDirectory + currentComputerConfiguration.videoTerminalConfiguration.wavFileName);
        else
        {
            p_Computer->setToneFrequency(0, currentComputerConfiguration.videoTerminalConfiguration.bellFrequency, true);
            p_Computer->setToneFrequency(1, currentComputerConfiguration.videoTerminalConfiguration.bellFrequency, true);
            cycleBell_ = cycleSizeBell_;
        }
    }
}

void Vt100::click()
{
    if (SetUpFeature_[VTKEYCLICK])
    {
        p_Computer->setToneFrequency(0, 800, true);
        p_Computer->setToneFrequency(1, 800, true);
        cycleClick_ = cycleSizeClick_;
    }
}

void Vt100::activateMainWindow()
{
    bool maximize = IsMaximized();
    Iconize(false);
    Raise();
    Show(true);
    Maximize(maximize);
}

void Vt100::writeMem(int y, int x, int value)
{
    videoRam_[y][x] = value;
    Word memoryStart = p_Computer->getDebugMemoryStart();
    if (((y*charactersPerRow_)+x) >= memoryStart && ((y*charactersPerRow_)+x)<(memoryStart + 256))
        p_Main->updateDebugMemory(((y*charactersPerRow_)+x));
}

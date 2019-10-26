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

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#if !wxUSE_COMBOCTRL
#error "Please set wxUSE_COMBOCTRL to 1 and rebuild the library."
#endif

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#include "main.h"

#include "cdp18s600.h"
#include "cdp18s640.h"
#include "pio.h"
#include "cdp1852.h"
#include "upd765.h"

Cdp18s600::Cdp18s600(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock, ElfConfiguration conf)
:V1870(title, pos, size, zoomLevel, computerType, clock)
{
    Cdp18s600Configuration = conf;
    computerTypeStr_ = p_Main->getRunningComputerStr();
    computerType_ = p_Main->getRunningComputerId();
    microboardType_ = p_Main->getMicroboardType(computerType_);
    Cdp18s600ClockSpeed_ = clock;
    lastAddress_ = 0;
   
    saveStarted_ = false;
    loadStarted_ = false;
    microDosRunning_ = false;
    resetHdData_ = true;
    printEf_ = 1;
    
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif
    
//    this->SetClientSize(size);
    
#if defined (__WXMAC__) || (__linux__)
    cdp18s640FramePointer = new Cdp18s640Frame(title, p_Main->getMainPos(computerType_), wxSize(310, 180));
#else
    cdp18s640FramePointer = new Cdp18s640Frame(title, p_Main->getMainPos(computerType_), wxSize(329, 180));
#endif
    cdp18s640FramePointer->init();

    pioMessage_ = "CDP1851 PIO";

    p_Printer = new Printer();
    p_Printer->init(p_Printer, computerTypeStr_, MS2000PRINTER);

    addressLatchCounter_ = 0;
}

Cdp18s600::~Cdp18s600()
{
    p_Printer->closeFrames();
    delete p_Printer;
    if (Cdp18s600Configuration.vtType != VTNONE)
    {
        p_Main->setVtPos(computerType_, vtPointer->GetPosition());
        vtPointer->Destroy();
    }
    if (Cdp18s600Configuration.vtExternal)
        delete p_Serial;
    
    if (cdp18s640FramePointer != NULL)
    {
        if (Cdp18s600Configuration.useElfControlWindows)
            p_Main->setMainPos(computerType_, cdp18s640FramePointer->GetPosition());
        
        cdp18s640FramePointer->Destroy();
    }

    delete cdp18s640FramePointer;

    if (pioFramePointer != NULL)
    {
        if (Cdp18s600Configuration.usePio)
            p_Main->setSecondFramePos(computerType_, pioFramePointer->GetPosition());
        
        pioFramePointer->Destroy();
    }
    if (Cdp18s600Configuration.useCdp18s660)
    {
        p_Main->setThirdFramePos(computerType_, pioFramePointer1->GetPosition());
        pioFramePointer1->Destroy();

        p_Main->setFourthFramePos(computerType_, pioFramePointer2->GetPosition());
        pioFramePointer2->Destroy();
    }
}

void Cdp18s600::onClose(wxCloseEvent&WXUNUSED(event) )
{
    p_Main->stopComputer();
}

void Cdp18s600::configureComputer()
{
    wxString messageStr;
    
    setCycleType(COMPUTERCYCLE, LEDCYCLE);

    inType_[1] = MS2000IOGROUP;
    inType_[2] = MS2000IO2;
    inType_[3] = MS2000IO3;
    inType_[4] = MS2000IO4;
    inType_[5] = MS2000IO5;
    inType_[6] = MS2000IO6;
    inType_[7] = MS2000IO7;
    outType_[1] = MS2000IOGROUP;
    outType_[2] = MS2000IO2;
    outType_[3] = MS2000IO3;
    outType_[4] = MS2000IO4;
    outType_[5] = MS2000IO5;
    outType_[6] = MS2000IO6;
    outType_[7] = MS2000IO7;
    
    efType_[1] = CDP18SEF1;
    efType_[2] = CDP18SEF2;
    efType_[3] = MS2000EF;

    efState_[1] = 1;
    efState_[2] = 1;
    efState_[3] = 1;

    if (computerType_ == MICROBOARD)
        p_Main->message("Configuring " + computerTypeStr_ + " with " + p_Main->getMicroboardTypeStr(microboardType_));
    else
        p_Main->message("Configuring " + computerTypeStr_);

    p_Main->message("	Output 1: set I/O group");
 
    p_Main->message("");

    if (p_Main->getPrinterStatus(computerType_))
	{
        messageStr.Printf("	I/O group %X: printer", Cdp18s600Configuration.printerGroup);
	    p_Main->message(messageStr);
	}

	if (Cdp18s600Configuration.useUart)
	{
        messageStr.Printf("	I/O group %X: video terminal", Cdp18s600Configuration.uartGroup);
	    p_Main->message(messageStr);
	}

    if (Cdp18s600Configuration.useTape)
        p_Main->message("	I/O group 2: tape");

    messageStr = "	I/O group 8: " + pioMessage_;
    if (Cdp18s600Configuration.useUpd765 && Cdp18s600Configuration.upd765Group == 8)
        messageStr += " & CDP18S651 (using uPD765)";
    p_Main->message(messageStr);

    if (Cdp18s600Configuration.useUpd765 && Cdp18s600Configuration.upd765Group != 8)
    {
        messageStr.Printf("	I/O group %X: CDP18S651 (using uPD765)", Cdp18s600Configuration.upd765Group);
        p_Main->message(messageStr);
    }

    p_Main->message("");
    
    if (Cdp18s600Configuration.vtType != VTNONE)
        configureVt();
    
    if (Cdp18s600Configuration.vtExternal)
    {
        p_Serial = new Serial(computerType_, Cdp18s600ClockSpeed_, Cdp18s600Configuration);
        p_Serial->configureMs2000(Cdp18s600Configuration.baudR, Cdp18s600Configuration.baudT);
    }
    
	if (p_Main->getPrinterStatus(computerType_))
    {
        messageStr.Printf("Configuring printer support on group %X", Cdp18s600Configuration.printerGroup);
        p_Main->message(messageStr);
		p_Main->message("	Output 6: write data, EF 1: data ready\n");
	}

    if (Cdp18s600Configuration.useTape)
    {
        p_Main->message("Configuring tape support on group 2");
        p_Main->message("	Output 4: tape motor, output 5: cassette out");
        p_Main->message("	EF 2: cassette in\n");
    }

    if (Cdp18s600Configuration.useUpd765)
        configureUpd765(Cdp18s600Configuration.fdcType_);

    configurePio();
    
    if (Cdp18s600Configuration.useCdp18s660)
        configureCdp18s660();
    
    resetCpu();
}

void Cdp18s600::configureVt()
{
    double zoom = p_Main->getZoomVt();
    if (Cdp18s600Configuration.vtType == VT52)
        vtPointer = new Vt100(computerTypeStr_ + " - VT 52", p_Main->getVtPos(computerType_), wxSize(640*zoom, 400*zoom), zoom, computerType_, Cdp18s600ClockSpeed_, Cdp18s600Configuration);
    else
        vtPointer = new Vt100(computerTypeStr_ + " - VT 100", p_Main->getVtPos(computerType_), wxSize(640*zoom, 400*zoom), zoom, computerType_, Cdp18s600ClockSpeed_, Cdp18s600Configuration);
    p_Vt100 = vtPointer;
    if (Cdp18s600Configuration.useUart)
        vtPointer->configureMs2000(Cdp18s600Configuration.baudR, Cdp18s600Configuration.baudT);
    else
        vtPointer->configureStandard(Cdp18s600Configuration.baudR, Cdp18s600Configuration.baudT, 4);
}

void Cdp18s600::configurePio()
{
#if defined (__WXMAC__) || (__linux__)
    pioFramePointer = new PioFrame("CDP1851 PIO", p_Main->getSecondFramePos(computerType_), wxSize(310, 180), 0);
#else
    pioFramePointer = new PioFrame("CDP1851 PIO", p_Main->getSecondFramePos(computerType_), wxSize(329, 180), 0);
#endif
    
    p_Main->message("Configuring CDP1851 PIO on group 8");
    p_Main->message("	Output 3: write to port A, output 2: write to port B");
    p_Main->message("	Input 3: read port A, input 2: read port B");
    p_Main->message("	Output 6: write control register, input 6: read status");
    p_Main->message("	EF 1: ARDY, EF 2: BRDY");
    p_Main->message("");
}

void Cdp18s600::configureCdp18s660()
{
    wxString messageStr;

#if defined (__WXMAC__) || (__linux__)
    pioFramePointer1 = new PioFrame("CDP18S660 PIO 1", p_Main->getThirdFramePos(computerType_), wxSize(310, 180), 1);
#else
    pioFramePointer1 = new PioFrame("CDP18S660 PIO 1", p_Main->getThirdFramePos(computerType_), wxSize(329, 180), 1);
#endif
    
    messageStr.Printf("Configuring CDP18S660 PIO 1 on group %X", Cdp18s600Configuration.cdp18s660Group1);
    p_Main->message(messageStr);
    p_Main->message("	Output 4: write to port A, output 6: write to port B");
    p_Main->message("	Input 4: read port A, input 6: read port B");
    p_Main->message("	Output 2: write control register, input 2: read status");
    p_Main->message("	EF 1: ARDY, EF 2: BRDY");
    p_Main->message("");

#if defined (__WXMAC__) || (__linux__)
    pioFramePointer2 = new PioFrame("CDP18S660 PIO 2", p_Main->getFourthFramePos(computerType_), wxSize(310, 180), 2);
#else
    pioFramePointer2 = new PioFrame("CDP18S660 PIO 2", p_Main->getFourthFramePos(computerType_), wxSize(329, 180), 2);
#endif
    
    messageStr.Printf("Configuring CDP18S660 PIO 2 on group %X", Cdp18s600Configuration.cdp18s660Group2);
    p_Main->message(messageStr);
    p_Main->message("	Output 4: write to port A, output 6: write to port B");
    p_Main->message("	Input 4: read port A, input 6: read port B");
    p_Main->message("	Output 2: write control register, input 2: read status");
    p_Main->message("	EF 1: ARDY, EF 2: BRDY");
    p_Main->message("");
}

void Cdp18s600::initComputer()
{
    if (Cdp18s600Configuration.usev1870)
        init1870();
  
    keyboardCode_ = 0;

    addressLatch_ = 0;
    setCpuMode(RESET); // CLEAR = 0, WAIT = 1, CLEAR LED ON, WAIT LED OFF, RUN LED OFF

    ioGroup_ = 0;
    cassetteEf_ = 0;

    cdpRunState_ = RESETSTATE;
}

void Cdp18s600::onRunButton()
{
    if (!singleStateStep_)
        addressLatch_ = 0x8000;
    if (idle_)
        dmaOut();
    onRun();
}

void Cdp18s600::onRunPButton()
{
    addressLatch_ = 0x0000;
    if (idle_)
        dmaOut();
    onRun();
}

void Cdp18s600::onRun()
{
//    if (!singleStateStep_)
//        resetCpu();
    
    if (cpuMode_ != RUN)
        resetEffectiveClock();
    
    addressLatchCounter_ = 64;
    setCpuMode(RUN); // CLEAR = 1, WAIT = 1, CLEAR LED OFF, WAIT LED OFF, RUN LED ON

    p_Main->eventUpdateTitle();
}

void Cdp18s600::autoBoot()
{
    addressLatch_ = p_Main->getBootAddress(computerTypeStr_, computerType_);
    setCpuMode(RUN); // CLEAR = 1, WAIT = 1, CLEAR LED OFF, WAIT LED OFF, RUN LED ON
}

void Cdp18s600::setAddressLatch(Word bootAddress)
{
    addressLatch_ = bootAddress;
}

void Cdp18s600::onSingleStep()
{
    singleStateStep_ = !singleStateStep_;
    if (singleStateStep_)
    {
        setMsValue_ = (int) p_Main->getLedTimeMs(computerType_);
        setLedMs(0);
    }
    else
        setLedMs(setMsValue_);
}

Byte Cdp18s600::ef(int flag)
{
    int defaultRet = defaultEf(flag);
    if (defaultRet != -1)
        return defaultRet;
    
    switch(efType_[flag])
    {
        case 0:
            return 1;
        break;
            
        case VT100EF:       // EF4
            if (p_Vt100 != NULL)
                return vtPointer->ef();
        break;
            
        case VTSERIALEF:
            if (p_Serial != NULL)
                return p_Serial->ef();
        break;
            
        case CDP18SEF1:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    return pioEfState_[0][1];
                break;
                    
                default:
                    return 1;
                break;
            }
        break;
            
        case CDP18SEF2:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    return pioEfState_[0][2];
                break;

                case IO_GRP_TAPE:
                    if (Cdp18s600Configuration.useTape)
                        return cassetteEf_;
                break;

                default:
                    return 1;
                break;
            }
        break;

        case MS2000EF:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                return efInterrupt();
            else
                return 1;
        break;

        default:
            return 1;
    }
    return 1;
}

int Cdp18s600::defaultEf(int flag)
{
    if  (ioGroup_ == Cdp18s600Configuration.v1870Group && Cdp18s600Configuration.usev1870)
    {
        if (flag == 1)
            return ef1_1870();
        
        if (flag == 3)
            return keyboardEf3_;
    }
    if  (ioGroup_ == Cdp18s600Configuration.printerGroup && p_Main->getPrinterStatus(computerType_))
    {
        if (flag == 1)
        {
            return printEf_;
        }
    }
    if  (ioGroup_ == Cdp18s600Configuration.cdp18s660Group1 && Cdp18s600Configuration.useCdp18s660)
    {
        if (flag == 1)
            return pioEfState_[1][1];
        if (flag == 2)
            return pioEfState_[1][2];
    }
    if  (ioGroup_ == Cdp18s600Configuration.cdp18s660Group2 && Cdp18s600Configuration.useCdp18s660)
    {
        if (flag == 1)
            return pioEfState_[2][1];
        if (flag == 2)
            return pioEfState_[2][2];
    }
    return -1;
}

void Cdp18s600::setEfState(int pioNumber, int number, Byte value)
{
    pioEfState_[pioNumber][number] = value;
}

Byte Cdp18s600::in(Byte port, Word WXUNUSED(address))
{
    Byte ret = 255;

    int defaultRet = defaultIn(port);
    if (defaultRet != -1)
        return defaultRet;
    
    switch(inType_[port])
    {
        case 0:
            ret = 255;
        break;
            
        case MS2000IOGROUP:
        break;
            
        case MS2000IO2:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    return pioFramePointer->readPortB();
                break;
            }
        break;
            
        case MS2000IO3:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    return pioFramePointer->readPortA();
                break;
            }
        break;
            
        case MS2000IO4:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                return inputMasterStatus();
        break;
            
        case MS2000IO5:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                return inputCommandStatus();
        break;
            
        case MS2000IO6:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    return pioFramePointer->readStatusRegister();
                break;
            }
        break;
            
        case MS2000IO7:
        break;
            
        default:
            ret = 255;
    }
    inValues_[port] = ret;
    return ret;
}

int Cdp18s600::defaultIn(Byte port)
{
    if  (ioGroup_ == Cdp18s600Configuration.uartGroup)
    {
        if (port == Cdp18s600Configuration.elfPortConf.uartOut)
        {
            if (p_Vt100 != NULL)
                return p_Vt100->uartIn();
            if (p_Serial != NULL)
                return p_Serial->uartIn();
        }
        if (port == Cdp18s600Configuration.elfPortConf.uartControl)
        {
            if (p_Vt100 != NULL)
                return p_Vt100->uartStatus();
            if (p_Serial != NULL)
                return p_Serial->uartStatus();
        }
    }
    if  (ioGroup_ == Cdp18s600Configuration.v1870Group && Cdp18s600Configuration.usev1870)
    {
        if (port == 3)
            return keyboardIn();
    }
    if  (ioGroup_ == Cdp18s600Configuration.cdp18s660Group1 && Cdp18s600Configuration.useCdp18s660)
    {
        if (port == 2)
            return pioFramePointer1->readStatusRegister();
        if (port == 4)
            return pioFramePointer1->readPortA();
        if (port == 6)
            return pioFramePointer1->readPortB();
    }
    if  (ioGroup_ == Cdp18s600Configuration.cdp18s660Group2 && Cdp18s600Configuration.useCdp18s660)
    {
        if (port == 2)
            return pioFramePointer2->readStatusRegister();
        if (port == 4)
            return pioFramePointer2->readPortA();
        if (port == 6)
            return pioFramePointer2->readPortB();
    }
    return -1;
}

void Cdp18s600::out(Byte port, Word address, Byte value)
{
    outValues_[port] = value;
    
    defaultOut(port, address, value);
    switch(outType_[port])
    {
        case 0:
            return;
        break;
            
        case MS2000IOGROUP:
            ioGroup_ = value;
        break;
            
        case MS2000IO2:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    pioFramePointer->writePortB(value);
                break;
            }
        break;
            
        case MS2000IO3:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    pioFramePointer->writePortA(value);
                break;
            }
        break;
            
        case MS2000IO4:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                outputDmaControl(value);
            switch (ioGroup_)
            {
                case IO_GRP_TAPE:
                    if (Cdp18s600Configuration.useTape)
                        tapeIo(value);
                break;
            }
        break;
            
        case MS2000IO5:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                outputCommand(value);
            switch (ioGroup_)
            {
                case IO_GRP_TAPE:
                    if (Cdp18s600Configuration.useTape)
                        psaveAmplitudeChange(value&1);
                break;
            }
        break;
            
        case MS2000IO6:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    pioFramePointer->writeControlRegister(value);
                break;
            }
        break;
            
        case MS2000IO7:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                outputDmaCounter(value);
        break;
    }
}

void Cdp18s600::defaultOut(Byte port, Word address, Byte value)
{
    if  (ioGroup_ == Cdp18s600Configuration.uartGroup)
    {
        if (port == Cdp18s600Configuration.elfPortConf.uartOut)
        {
            if (p_Vt100 != NULL)
                p_Vt100->uartOut(value);
            if (p_Serial != NULL)
                p_Serial->uartOut(value);
        }
        if (port == Cdp18s600Configuration.elfPortConf.uartControl)
        {
            if (p_Vt100 != NULL)
                p_Vt100->uartControl(value);
            if (p_Serial != NULL)
                p_Serial->uartControl(value);
        }
    }
    if  (ioGroup_ == Cdp18s600Configuration.v1870Group && Cdp18s600Configuration.usev1870)
    {
        switch (port)
        {
            case 2:
                p_Video->setInterruptEnable(value == 1);
                break;
                
            case 3:
                out3_1870(value);
                break;
                
            case 4:
                outValues_[port] = address;
                out4_1870(address);
                break;
                
            case 5:
                outValues_[port] = address;
                out5_1870(address);
                break;
                
            case 6:
                outValues_[port] = address;
                out6_1870(address);
                break;
                
            case 7:
                outValues_[port] = address;
                out7_1870(address);
                break;
        }
    }
    if  (ioGroup_ == Cdp18s600Configuration.printerGroup && p_Main->getPrinterStatus(computerType_))
    {
        if (port == 6)
            p_Printer->printerOut(value ^ 0xff);
    }
    if  (ioGroup_ == Cdp18s600Configuration.cdp18s660Group1 && Cdp18s600Configuration.useCdp18s660)
    {
        if (port == 2)
            pioFramePointer1->writeControlRegister(value);
        if (port == 4)
            pioFramePointer1->writePortA(value);
        if (port == 6)
            pioFramePointer1->writePortB(value);
    }
    if  (ioGroup_ == Cdp18s600Configuration.cdp18s660Group2 && Cdp18s600Configuration.useCdp18s660)
    {
        if (port == 2)
            pioFramePointer2->writeControlRegister(value);
        if (port == 4)
            pioFramePointer2->writePortA(value);
        if (port == 6)
            pioFramePointer2->writePortB(value);
    }
}

Byte Cdp18s600::keyboardIn()
{
    Byte ret;
    
    keyboardEf3_ = 1;
    ret = keyboardCode_;
    
    switch(ret)
    {
        case '@':ret = 0x20; break;
        case '#':ret = 0x23; break;
        case '\'': ret = 0x27; break;
        case '[':ret = 0x28; break;
        case ']':ret = 0x29; break;
        case ':':ret = 0x2a; break;
        case ';':ret = 0x2b; break;
        case '<':ret = 0x2c; break;
        case '=':ret = 0x2d; break;
        case '>':ret = 0x2e; break;
        case '\\':ret = 0x2f; break;
        case '.':ret = 0x3a; break;
        case ',':ret = 0x3b; break;
        case '(':ret = 0x3c; break;
        case '^':ret = 0x3d; break;
        case ')':ret = 0x3e; break;
        case '_':ret = 0x3f; break;
        case '?':ret = 0x40; break;
        case '+':ret = 0x5b; break;
        case '-':ret = 0x5c; break;
        case '*':ret = 0x5d; break;
        case '/':ret = 0x5e; break;
        case ' ':ret = 0x5f; break;
    }
    if (ret >= 0x90)  ret &= 0x7f;
    return ret;
}

void Cdp18s600::charEvent(int keycode)
{
    if (!Cdp18s600Configuration.usev1870)
        return;
    
    keyboardCode_ = keycode;
    keyboardEf3_ = 0;
}

bool Cdp18s600::keyDownExtended(int keycode, wxKeyEvent& event)
{
    if (!Cdp18s600Configuration.usev1870)
        return false;

    switch(keycode)
    {
        case WXK_RETURN:
            keyboardCode_ = 0x80;
            keyboardEf3_ = 0;
            return true;
        break;
            
        case WXK_NUMPAD_ENTER:
            keyboardCode_ = 0x80;
            keyboardEf3_ = 0;
            return true;
        break;
            
        case WXK_ESCAPE:
            keyboardCode_ = 0x81;
            keyboardEf3_ = 0;
            return true;
        break;
            
        case WXK_BACK:
            keyboardCode_ = 0x86;
            keyboardEf3_ = 0;
            return true;
        break;
            
        case WXK_DELETE:
            keyboardCode_ = 0x86;
            keyboardEf3_ = 0;
            return true;
        break;
            
        case WXK_LEFT:
            keyboardCode_ = 0x84;
            keyboardEf3_ = 0;
            return true;
        break;
            
        case WXK_RIGHT:
            keyboardCode_ = 0x83;
            keyboardEf3_ = 0;
            return true;
        break;
            
        case WXK_UP:
            keyboardCode_ = 0x82;
            keyboardEf3_ = 0;
            return true;
        break;
            
        case WXK_DOWN:
            keyboardCode_ = 0x85;
            keyboardEf3_ = 0;
            return true;
        break;
            
        case WXK_NUMPAD_ADD:
            if (event.GetModifiers() == wxMOD_SHIFT)
                keyboardCode_ = 0xfb;
            else
                keyboardCode_ = 0xdb;
            keyboardEf3_ = 0;
            return true;
        break;
            
        case WXK_NUMPAD_SUBTRACT:
            if (event.GetModifiers() == wxMOD_SHIFT)
                keyboardCode_ = 0xfc;
            else
                keyboardCode_ = 0xdc;
            keyboardEf3_ = 0;
            return true;
        break;
    }
    return false;
}

void Cdp18s600::keyClear()
{
    if (!Cdp18s600Configuration.usev1870)
        return;

    keyboardEf3_ = 1;
    keyboardCode_ = 0;
}

void Cdp18s600::tapeIo(Byte value)
{
    // 0x41 = TAPE 0, 0x81 = TAPE 1
    if (value == 0x41)
    {
        tapeNumber_ = "";
        if (!saveStarted_)
        {
            saveStarted_ = true;
            p_Main->startCassetteSave(0);
        }
        else
            restartTapeSave(TAPE_RECORD);
    }
    
    if (value == 0x81)
    {
        tapeNumber_ = "1";
        if (!saveStarted_)
        {
            saveStarted_ = true;
            p_Main->startCassetteSave(1);
        }
        else
            restartTapeSave(TAPE_RECORD1);
    }
    
    // 0x40 = TAPE 0, 0x80 = TAPE 1
    if (value == 0x40)
    {
        tapeNumber_ = "";
        if (!loadStarted_)
        {
            loadStarted_ = true;
            p_Main->startCassetteLoad(0);
        }
        else
            restartTapeLoad(TAPE_PLAY);
    }
    
    if (value == 0x80)
    {
        tapeNumber_ = "1";
        if (!loadStarted_)
        {
            loadStarted_ = true;
            p_Main->startCassetteLoad(1);
        }
        else
            restartTapeLoad(TAPE_PLAY1);
    }
    
    if (value == 0)
        pauseTape();
}

void Cdp18s600::switchQ(int value)
{
    cdp18s640FramePointer->setQLed(value);
    
    if (Cdp18s600Configuration.vtType != VTNONE)
        vtPointer->switchQ(value);
    
    if (Cdp18s600Configuration.vtExternal)
        p_Serial->switchQ(value);
}

void Cdp18s600::showCycleData(Byte val)
{
    cdp18s640FramePointer->showDataTil313Italic(val);
}

void Cdp18s600::showAddress(Word val)
{
    cdp18s640FramePointer->showAddressTil313Italic(val);
}

void Cdp18s600::showState(int state)
{
    cdp18s640FramePointer->setStateLed(SC0LED, state&1);
    cdp18s640FramePointer->setStateLed(SC1LED, (state&2)>>1);
}

void Cdp18s600::setCpuMode(int mode)
{
    int clear = (mode>>1)&1;
    int wait = mode&1;
    
    setWait(wait);
    setClear(clear);

    if (clear == 0)
    {
        pioFramePointer->reset();
        if (Cdp18s600Configuration.useCdp18s660)
        {
            pioFramePointer1->reset();
            pioFramePointer2->reset();
        }
    }

    cdp18s640FramePointer->setRunLed(clear & wait);

    wait ^= 1;
    clear ^= 1;

    cdp18s640FramePointer->setStateLed(CLEARLED, clear);
    cdp18s640FramePointer->setStateLed(WAITLED, wait);
}

void Cdp18s600::cycle(int type)
{
    switch(cycleType_[type])
    {
        case 0:
            return;
        break;
            
        case FDCCYCLE:
            cycleUpd765();
        break;
            
        case V1870CYCLE:
            cycle1870();
        break;
            
        case VT100CYCLE:
            vtPointer->cycleVt();
        break;
            
        case VTSERIALCYCLE:
            if (p_Serial != NULL)
                p_Serial->cycleVt();
        break;

        case LEDCYCLE:
            cycleLed();
            if (Cdp18s600Configuration.usePio)
                pioFramePointer->interruptCycle();
            if (Cdp18s600Configuration.useCdp18s660)
            {
                pioFramePointer1->interruptCycle();
                pioFramePointer2->interruptCycle();
            }
        break;
    }
}

void Cdp18s600::cycleLed()
{
    if (ledCycleValue_ > 0)
    {
        ledCycleValue_ --;
        if (ledCycleValue_ <= 0)
        {
            ledCycleValue_ = ledCycleSize_;
            cdp18s640FramePointer->ledTimeout();
			if (Cdp18s600Configuration.usePio)
				pioFramePointer->ledTimeout();
            if (Cdp18s600Configuration.useCdp18s660)
            {
                pioFramePointer1->ledTimeout();
                pioFramePointer2->ledTimeout();
            }
        }
    }
}

void Cdp18s600::startComputer()
{
    resetPressed_ = false;
    readRoms();

    if (computerType_ == MICROBOARD)
        configureCards();
    
    if (p_Vt100 != NULL)
        p_Vt100->Show(true);
    
    if (Cdp18s600Configuration.autoBoot)
        autoBoot();

    if (Cdp18s600Configuration.useUpd765)
        setDiskNames();
    
    if (Cdp18s600Configuration.usev1870)
    {
        if (configure1870Microboard(Cdp18s600Configuration.v1870Group, Cdp18s600Configuration.pageMemSize, Cdp18s600Configuration.v1870VideoMode, Cdp18s600Configuration.v1870InterruptMode))
        {
            readChargenFile(p_Main->getCharRomDir(MICROBOARD), p_Main->getCharRomFile(MICROBOARD));
        }

        Show(true);
        
        defineMemoryType(0xf400, 0xf7ff, CRAM1870);
        defineMemoryType(0xf800, 0xffff, PRAM1870);
    }
    
    p_Main->setSwName("");
    p_Main->updateTitle();
    
    cpuCycles_ = 0;
    p_Main->startTime();
    
    int ms = (int) p_Main->getLedTimeMs(computerType_);
    cdp18s640FramePointer->setLedMs(ms);
    
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((Cdp18s600ClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;

	if (Cdp18s600Configuration.usePio)
        startPio(ms);

    if (Cdp18s600Configuration.useCdp18s660)
        startCdp18s660(ms);

    if (Cdp18s600Configuration.useElfControlWindows)
        cdp18s640FramePointer->Show(true);

    threadPointer->Run();
}

void Cdp18s600::startPio(long ms)
{
    pioFramePointer->setLedMs(ms);
    pioFramePointer->Show(true);
}

void Cdp18s600::startCdp18s660(long ms)
{
    pioFramePointer1->setLedMs(ms);
    pioFramePointer1->Show(Cdp18s600Configuration.usePioWindow1Cdp18s660);

    pioFramePointer2->setLedMs(ms);
    pioFramePointer2->Show(Cdp18s600Configuration.usePioWindow2Cdp18s660);
}

void Cdp18s600::setDiskNames()
{
    if (p_Main->getDirectoryMode(Cdp18s600Configuration.fdcType_, 0))
        setDiskName(1, p_Main->getUpdFloppyDirSwitched(Cdp18s600Configuration.fdcType_, 0), "");
    else
    {
        wxString fileName = p_Main->getUpdFloppyFile(Cdp18s600Configuration.fdcType_, 0);
        if (fileName.Len() == 0)
            setDiskName(1, p_Main->getUpdFloppyDir(Cdp18s600Configuration.fdcType_, 0), "");
        else
            setDiskName(1, p_Main->getUpdFloppyDir(Cdp18s600Configuration.fdcType_, 0), p_Main->getUpdFloppyFile(Cdp18s600Configuration.fdcType_, 0));
    }
    
    if (p_Main->getDirectoryMode(Cdp18s600Configuration.fdcType_, 1))
        setDiskName(2, p_Main->getUpdFloppyDirSwitched(Cdp18s600Configuration.fdcType_, 1), "");
    else
    {
        wxString fileName = p_Main->getUpdFloppyFile(Cdp18s600Configuration.fdcType_, 1);
        if (fileName.Len() == 0)
            setDiskName(2, p_Main->getUpdFloppyDir(Cdp18s600Configuration.fdcType_, 1), "");
        else
            setDiskName(2, p_Main->getUpdFloppyDir(Cdp18s600Configuration.fdcType_, 1), p_Main->getUpdFloppyFile(Cdp18s600Configuration.fdcType_, 1));
    }
    
    if (p_Main->getDirectoryMode(Cdp18s600Configuration.fdcType_, 2))
        setDiskName(3, p_Main->getUpdFloppyDirSwitched(Cdp18s600Configuration.fdcType_, 2), "");
    else
    {
        wxString fileName = p_Main->getUpdFloppyFile(Cdp18s600Configuration.fdcType_, 2);
        if (fileName.Len() == 0)
            setDiskName(3, p_Main->getUpdFloppyDir(Cdp18s600Configuration.fdcType_, 2), "");
        else
            setDiskName(3, p_Main->getUpdFloppyDir(Cdp18s600Configuration.fdcType_, 2), p_Main->getUpdFloppyFile(Cdp18s600Configuration.fdcType_, 2));
    }
    
    if (p_Main->getDirectoryMode(Cdp18s600Configuration.fdcType_, 3))
        setDiskName(4, p_Main->getUpdFloppyDirSwitched(Cdp18s600Configuration.fdcType_, 3), "");
    else
    {
        wxString fileName = p_Main->getUpdFloppyFile(Cdp18s600Configuration.fdcType_, 3);
        if (fileName.Len() == 0)
            setDiskName(4, p_Main->getUpdFloppyDir(Cdp18s600Configuration.fdcType_, 3), "");
        else
            setDiskName(4, p_Main->getUpdFloppyDir(Cdp18s600Configuration.fdcType_, 3), p_Main->getUpdFloppyFile(Cdp18s600Configuration.fdcType_, 3));
    }
}

void Cdp18s600::readRoms()
{
    Conf configuration = p_Main->getConfiguration(MICROBOARD);
    
    p_Main->setMemoryMapCDP18S600(&configuration, -1, -1);
}

void Cdp18s600::configureCards()
{
    Conf configuration = p_Main->getConfiguration(MICROBOARD);

    for (int card=0; card<p_Main->getMicroboardMaxCard(computerType_); card++)
    {
        MicroMemoryConf microMemConf = p_Main->getMicroMemConf(card);
        switch (configuration.microboardType_[card+2])
        {
            case CARD_CDP18S620:
                p_Main->setMemoryMapCDP18S620(&configuration, microMemConf, card+2, -1);
            break;

            case CARD_CDP18S621:
                p_Main->setMemoryMapCDP18S621(&configuration, microMemConf, card+2, -1);
            break;

            case CARD_CDP18S623A:
                p_Main->setMemoryMapCDP18S623a(&configuration, microMemConf, card+2, -1);
            break;

            case CARD_CDP18S625:
                p_Main->setMemoryMapCDP18S625(&configuration, microMemConf, card+2, -1);
            break;

            case CARD_CDP18S626:
            case CARD_CDP18S628:
            case CARD_CDP18S629:
                p_Main->setMemoryMapCDP18S626(&configuration, microMemConf, card+2, -1);
            break;

            case CARD_CDP18S627:
                p_Main->setMemoryMapCDP18S627(&configuration, microMemConf, card+2, -1);
            break;

            case CARD_CDP18S640:
                p_Main->setMemoryMapCDP18S640(&configuration, microMemConf, card+2, -1);
            break;

            case CARD_CDP18S652:
                p_Main->setMemoryMapCDP18S652(&configuration, microMemConf, card+2, -1);
            break;

            case CARD_CDP18S660:
                p_Main->setMemoryMapCDP18S660(&configuration, microMemConf, card+2, -1);
            break;
        }
    }
}

void Cdp18s600::readMicro(int romNumber, Word startAddress, Word lastAddress )
{
    if (p_Main->getMicroChipDisable(computerType_, romNumber))
        return;
    
    switch (p_Main->getMicroChipMemory(computerType_, romNumber))
    {
        case MICRO_ROM:
            readProgramMicro(p_Main->getRomDir(computerType_, romNumber), p_Main->getRomFile(computerType_, romNumber), ROM, startAddress, lastAddress+1, NONAME);
            defineMemoryType(startAddress, lastAddress, ROM);
        break;

        case MICRO_RAM:
            readProgramMicro(p_Main->getRomDir(computerType_, romNumber), p_Main->getRomFile(computerType_, romNumber), RAM, startAddress, lastAddress+1, NONAME);
            defineMemoryType(startAddress, lastAddress, RAM);
            p_Main->assDefault("mycode", startAddress, lastAddress);
        break;

        case MICRO_CPURAM:
            readProgramMicro(p_Main->getRomDir(computerType_, romNumber), p_Main->getRomFile(computerType_, romNumber), CPURAM, startAddress, lastAddress+1, NONAME);
            defineMemoryType(startAddress, lastAddress, CPURAM);
            p_Main->assDefault("mycode", startAddress, lastAddress);
        break;
    }
}

void Cdp18s600::writeMemDataType(Word address, Byte type)
{
    if (address < 0x8000)
        address = (address | addressLatch_);

    switch (memoryType_[address/256])
    {
        case ROM:
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
        break;
            
        case MAPPEDROM:
            if (mainMemoryDataType_[address&0xfbff] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address&0xfbff] = type;
            }
        break;
            
        case MAPPEDRAM:
            if (mainMemoryDataType_[address&0xf3ff] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address&0xf3ff] = type;
            }
        break;
            
       case RAM:
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
        break;

        case CPURAM:
            if (cpuRamDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                cpuRamDataType_[address] = type;
            }
        break;

        case PRAM1870:
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
        break;
    }
}

Byte Cdp18s600::readMemDataType(Word address)
{
    if (address < 0x8000)
        address = (address | addressLatch_);

    switch (memoryType_[address/256])
    {
        case RAM:
        case ROM:
            return mainMemoryDataType_[address];
        break;

        case MAPPEDROM:
            return mainMemoryDataType_[address&0xfbff];
        break;
            
        case MAPPEDRAM:
            return mainMemoryDataType_[address&0xf3ff];
        break;
            
        case CPURAM:
            return cpuRamDataType_[address];
        break;
            
        case PRAM1870:
            return mainMemoryDataType_ [address];
        break;
    }
    return MEM_TYPE_UNDEFINED;
}

Byte Cdp18s600::readMem(Word address)
{
    address_ = address;
    return readMemDebug(address);
}

Byte Cdp18s600::readMemDebug(Word address)
{
//    if ((address & 0x8000) == 0x8000)
//        addressLatch_ = 0;
    
    if (address < 0x8000)
        address = (address | addressLatch_);
    
    switch (memoryType_[address / 256])
    {
        case UNDEFINED:
            return 255;
        break;
            
        case ROM:
        case RAM:
            return mainMemory_[address];
        break;
        
        case MAPPEDROM:
            return mainMemory_[address&0xfbff];
        break;

        case MAPPEDRAM:
            return mainMemory_[address&0xf3ff];
        break;
            
        case CPURAM:
            return cpuRam_[address&0xff];
        break;
            
        case PRAM1870:
            return readPram(address);
        break;
            
        case CRAM1870:
            return readCram(address);
        break;

        default:
            return 255;
        break;
    }
}

void Cdp18s600::writeMem(Word address, Byte value, bool writeRom)
{
    address_ = address;
    writeMemDebug(address, value, writeRom);
}

void Cdp18s600::writeMemDebug(Word address, Byte value, bool writeRom)
{
    switch (memoryType_[address/256])
    {
        case UNDEFINED:
        case ROM:
            if (writeRom)
                mainMemory_[address]=value;
        break;
            
        case RAM:
            if (mainMemory_[address]==value)
                return;
            mainMemory_[address]=value;
            if (address >= (memoryStart_) && address<((memoryStart_) +256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;

        case MAPPEDRAM:
            address &= 0xf3ff;
            if (mainMemory_[address]==value)
                return;
            mainMemory_[address]=value;
            if (address >= (memoryStart_) && address<((memoryStart_) +256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;

        case MAPPEDROM:
            if (writeRom)
                mainMemory_[address&0xfbff]=value;
        break;
            
        case CPURAM:
            if (cpuRam_[address&0xff]==value)
                return;
            cpuRam_[address&0xff]=value;
            p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;

        case PRAM1870:
            if (writeRom)
                mainMemory_[address]=value;
            else
                writePram(address, value);
        break;
            
        case CRAM1870:
            writeCram(address, value);
        break;
    }
}

void Cdp18s600::cpuInstruction()
{
    if (cpuMode_ == RUN)
    {
        if (addressLatchCounter_ > 0)
        {
            addressLatchCounter_--;
            if (addressLatchCounter_ == 0)
                addressLatch_ = 0;
        }
        cpuCycleStep();
        if (idle_ || cpuMode_ != RUN)
            cdp18s640FramePointer->setRunLed(0);
        else
            cdp18s640FramePointer->setRunLed(1);
    }
    else
    {
        machineCycle();
        machineCycle();
        if (resetPressed_)
            resetPressed();
    }
    if (interruptRequested_ == true && cpuState_ == STATE_FETCH_1)
        p_Computer->interrupt();
}

void Cdp18s600::resetPressed()
{
    resetCpu();
    init1870();
    initComputer();
    out5_1870(0x0080);

    p_Main->setSwName("");
    p_Main->eventUpdateTitle();

    setCpuMode(RESET); // CLEAR = 0, WAIT = 1, CLEAR LED ON, WAIT LED OFF, RUN LED OFF
    
    showCycleData(0);
    if (Cdp18s600Configuration.autoBoot)
        autoBoot();

    resetPressed_ = false;
}

void Cdp18s600::moveWindows()
{
    if (Cdp18s600Configuration.vtType != VTNONE)
        vtPointer->Move(p_Main->getVtPos(computerType_));
    if (Cdp18s600Configuration.usePio)
        pioFramePointer->Move(p_Main->getSecondFramePos(computerType_));
    if (Cdp18s600Configuration.useCdp18s660)
    {
        pioFramePointer->Move(p_Main->getThirdFramePos(computerType_));
        pioFramePointer->Move(p_Main->getFourthFramePos(computerType_));
    }
    if (Cdp18s600Configuration.useElfControlWindows)
        cdp18s640FramePointer->Move(p_Main->getMainPos(computerType_));
}

void Cdp18s600::setForceUpperCase(bool status)
{
    if (Cdp18s600Configuration.vtType != VTNONE)
        vtPointer->setForceUCVt(status);
}

void Cdp18s600::setBootRam(bool status)
{
    Cdp18s600Configuration.bootRam = status;
}

void Cdp18s600::updateTitle(wxString Title)
{
    if (Cdp18s600Configuration.vtType == VT52)
        vtPointer->SetTitle(computerTypeStr_ + " - VT 52"+Title);
    if (Cdp18s600Configuration.vtType == VT100)
        vtPointer->SetTitle(computerTypeStr_ + " - VT 100"+Title);
    if (Cdp18s600Configuration.useElfControlWindows)
        cdp18s640FramePointer->SetTitle(computerTypeStr_ + Title);
}

void Cdp18s600::checkComputerFunction()
{
    wxString buffer;
    
    switch (loadedProgram_)
    {
        case UT62:
            switch (scratchpadRegister_[programCounter_])
            {
                case 0x813e: // key input
                    if (saveStarted_)
                    {
                        stopPausedSave();
                        saveStarted_ = false;
                    }
                    
                    if (loadStarted_)
                    {
                        stopPausedLoad();
                        loadStarted_ = false;
                    }
                break;
                    
                case 0xb011:
                    cdpRunState_ = RESETSTATECW;
                break;
                
                case 0xb053:
                    cdpRunState_ = BASICSTATE;
                break;
                
                case 0xC076:    // RUN
                    cdpRunState_ = RUNSTATE;
                break;
                
                case 0xc79f:    // CALL
                    cdpRunState_ = RUNSTATE;
                break;
                
                case 0xB225:    // BYE
                    cdpRunState_ = RESETSTATE;
                break;
            }
        break;
    
        case UT71:
            switch (scratchpadRegister_[programCounter_])
            {
                case 0x95e5: // MicroDos key input
                    microDosRunning_ = true;
                    resetHdData_ = true;
                break;
                
                case 0x34ad: // BASIC COMPILER key input
                case 0x34ce: // BASIC COMPILER key input
                    microDosRunning_ = false;
                    resetHdData_ = false;
                break;
                
                case 0xd13: // BASIC1 key input
                    microDosRunning_ = false ;
                    resetHdData_ = true;
                break;
                
                case 0x813e: // key input
                    if (resetHdData_ && resetHdData_)
                    {
                        for (int drive = 0; drive < 4; drive++)
                        {
                            initializeCat(drive);
                            hdCommand_ = 0;
                            for (int cluster = FIRST_CLUSTER; cluster < (MAX_CLUSTER + BUFFER_CLUSTER); cluster++)
                            {
                                clusterInfo_[drive][cluster].filenameDefined = false;
                                clusterInfo_[drive][cluster].readCluster = true;
                                clusterInfo_[drive][cluster].sdwClusterDefined = false;
                            }
                        }
                        resetHdData_ = false;
                    }
                
                    if (saveStarted_)
                    {
                        stopPausedSave();
                        saveStarted_ = false;
                    }
                    
                    if (loadStarted_)
                    {
                        stopPausedLoad();
                        loadStarted_ = false;
                    }
                    
                    if (p_Vt100 != NULL)
                    {
                        if (microDosRunning_)
                            vtPointer->setTabChar(0x7f);
                        else
                            vtPointer->setTabChar(8);
                    }
                
                    microDosRunning_ = false;
                break;
            }
        break;
    }
}

void Cdp18s600::onReset()
{
    resetPressed_ = true;
}

void Cdp18s600::sleepComputer(long ms)
{
    threadPointer->Sleep(ms);
}

void Cdp18s600::setLedMs(long ms)
{
    cdp18s640FramePointer->setLedMs(ms);
    pioFramePointer->setLedMs(ms);
    if (Cdp18s600Configuration.useCdp18s660)
    {
        pioFramePointer1->setLedMs(ms);
        pioFramePointer2->setLedMs(ms);
    }
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((Cdp18s600ClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

void Cdp18s600::startComputerRun(bool load)
{
    if (p_Vt100 != NULL)
        vtPointer->startMcdsRun(load);
}

bool Cdp18s600::isComputerRunning()
{
    if (cdpRunState_ == RUNSTATE)
        return true;
    else
        return false;
}

void Cdp18s600::releaseButtonOnScreen(HexButton* buttonPointer, int WXUNUSED(buttonType))
{
    cdp18s640FramePointer->releaseButtonOnScreen(buttonPointer);
}

void Cdp18s600::releaseButtonOnScreen2(HexButton* buttonPointer, int WXUNUSED(buttonType), int pioNumber)
{
    switch (pioNumber)
    {
        case 0:
            pioFramePointer->releaseButtonOnScreen(buttonPointer);
        break;

        case 1:
            pioFramePointer1->releaseButtonOnScreen(buttonPointer);
        break;

        case 2:
            pioFramePointer2->releaseButtonOnScreen(buttonPointer);
        break;
    }
}

void Cdp18s600::activateMainWindow()
{
 /*   bool maximize = IsMaximized();
    Iconize(false);
    Raise();
    Show(true);
    Maximize(maximize);*/
}

void Cdp18s600::showPio(bool state)
{
	pioFramePointer->Show(state);
    if (state)
        pioFramePointer->refreshLeds();
}

void Cdp18s600::showCdp18s660Pio1(bool state)
{
    pioFramePointer1->Show(state);
    if (state)
        pioFramePointer1->refreshLeds();
}

void Cdp18s600::showCdp18s660Pio2(bool state)
{
    pioFramePointer2->Show(state);
    if (state)
        pioFramePointer2->refreshLeds();
}

void Cdp18s600::removePio(int pioNumber)
{
    switch (pioNumber)
    {
        case 0:
            Cdp18s600Configuration.usePio = false;
            p_Main->pioWindows(computerType_, false);
            showPio(false);
        break;

        case 1:
            Cdp18s600Configuration.usePioWindow1Cdp18s660  = false;
            p_Main->cdp18660WindowPio1(computerType_, false);
            pioFramePointer1->Show(false);
        break;

        case 2:
            Cdp18s600Configuration.usePioWindow2Cdp18s660 = false;
            p_Main->cdp18660WindowPio2(computerType_, false);
            pioFramePointer2->Show(false);
        break;
    }
}

void Cdp18s600::setHeaderTitle(const wxString& title)
{
    cdp18s640FramePointer->SetTitle(title);
}

void Cdp18s600::showControlWindow(bool state)
{
    cdp18s640FramePointer->Show(state);
}

Cdp18s601::Cdp18s601(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock, ElfConfiguration conf)
: Cdp18s600(title, pos, size, zoomLevel, computerType, clock, conf)
{

}

void Cdp18s601::configurePio()
{
#if defined (__WXMAC__) || (__linux__)
    pioFramePointer = new PioFrame("CDP1851 PIO", p_Main->getSecondFramePos(computerType_), wxSize(310, 180), 0);
#else
    pioFramePointer = new PioFrame("CDP1851 PIO", p_Main->getSecondFramePos(computerType_), wxSize(329, 180), 0);
#endif
    
    p_Main->message("Configuring CDP1851 PIO on group 8");
    if (microboardType_ == MICROBOARD_CDP18S603A || microboardType_ == MICROBOARD_CDP18S606  || microboardType_ == MICROBOARD_CDP18S608)
    {
        p_Main->message("	Output 4/5: write to port A, output 6/7: write to port B");
        p_Main->message("	Input 4/5: read port A, input 6/7: read port B");
        p_Main->message("	Output 2/3: write control register, input 2/3: read status");
    }
    else
    {
        p_Main->message("	Output 4: write to port A, output 6: write to port B");
        p_Main->message("	Input 4: read port A, input 6: read port B");
        p_Main->message("	Output 2: write control register, input 2: read status");
    }
    p_Main->message("	EF 1: ARDY, EF 2: BRDY");
    p_Main->message("");
}

Byte Cdp18s601::in(Byte port, Word WXUNUSED(address))
{
    Byte ret = 255;
    
    int defaultRet = defaultIn(port);
    if (defaultRet != -1)
        return defaultRet;
    
    switch(inType_[port])
    {
        case 0:
            ret = 255;
        break;
            
        case MS2000IOGROUP:
        break;
            
        case MS2000IO2:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    return pioFramePointer->readStatusRegister();
                break;
            }
        break;
            
        case MS2000IO3:
            switch (ioGroup_)
            {                    
                case IO_GRP_PIO:
                    if (microboardType_ == MICROBOARD_CDP18S603A || microboardType_ == MICROBOARD_CDP18S606  || microboardType_ == MICROBOARD_CDP18S608)
                        return pioFramePointer->readStatusRegister();
                break;
            }
        break;
            
        case MS2000IO4:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                return inputMasterStatus();
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    return pioFramePointer->readPortA();
                break;
            }
        break;
            
        case MS2000IO5:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                return inputCommandStatus();
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    if (microboardType_ == MICROBOARD_CDP18S603A || microboardType_ == MICROBOARD_CDP18S606  || microboardType_ == MICROBOARD_CDP18S608)
                        return pioFramePointer->readPortA();
                break;
            }
        break;
            
        case MS2000IO6:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    return pioFramePointer->readPortB();
                break;
            }
        break;
            
        case MS2000IO7:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    if (microboardType_ == MICROBOARD_CDP18S603A || microboardType_ == MICROBOARD_CDP18S606  || microboardType_ == MICROBOARD_CDP18S608)
                        return pioFramePointer->readPortB();
                break;
            }
        break;
    }
    inValues_[port] = ret;
    return ret;
}

void Cdp18s601::out(Byte port, Word address, Byte value)
{
    outValues_[port] = value;
    
    defaultOut(port, address, value);
    switch(outType_[port])
    {
        case 0:
            return;
        break;
            
        case MS2000IOGROUP:
            ioGroup_ = value;
        break;
            
        case MS2000IO2:
            switch (ioGroup_)
            {                   
                case IO_GRP_PIO:
                    pioFramePointer->writeControlRegister(value);
                break;
            }
        break;
            
        case MS2000IO3:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    if (microboardType_ == MICROBOARD_CDP18S603A || microboardType_ == MICROBOARD_CDP18S606  || microboardType_ == MICROBOARD_CDP18S608)
                        pioFramePointer->writeControlRegister(value);
                break;
            }
        break;
            
        case MS2000IO4:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                outputDmaControl(value);
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    pioFramePointer->writePortA(value);
                break;

                case IO_GRP_TAPE:
                    if (Cdp18s600Configuration.useTape)
                        tapeIo(value);
                break;
            }
        break;
            
        case MS2000IO5:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                outputCommand(value);
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    if (microboardType_ == MICROBOARD_CDP18S603A || microboardType_ == MICROBOARD_CDP18S606  || microboardType_ == MICROBOARD_CDP18S608)
                        pioFramePointer->writePortA(value);
                break;

                case IO_GRP_TAPE:
                    if (Cdp18s600Configuration.useTape)
                        psaveAmplitudeChange(value&1);
                break;
            }
        break;
            
        case MS2000IO6:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    pioFramePointer->writePortB(value);
                break;
            }
        break;
            
        case MS2000IO7:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                outputDmaCounter(value);
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    if (microboardType_ == MICROBOARD_CDP18S603A || microboardType_ == MICROBOARD_CDP18S606  || microboardType_ == MICROBOARD_CDP18S608)
                        pioFramePointer->writePortB(value);
                break;
            }
        break;
    }
}

void Cdp18s601::readRoms()
{
    Conf configuration = p_Main->getConfiguration(MICROBOARD);
    
    p_Main->setMemoryMapCDP18S601(&configuration, -1, -1);
}

Cdp18s602::Cdp18s602(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock, ElfConfiguration conf)
: Cdp18s600(title, pos, size, zoomLevel, computerType, clock, conf)
{
    pioFramePointer = NULL;
    pioMessage_ = "CDP1852";
}

Cdp18s602::~Cdp18s602()
{
    if (Cdp18s600Configuration.usePio)
        p_Main->setSecondFramePos(computerType_, cdp1852FramePointer->GetPosition());
    cdp1852FramePointer->Destroy();
}

void Cdp18s602::configurePio()
{
#if defined (__WXMAC__) || (__linux__)
    cdp1852FramePointer = new Cdp1852Frame("CDP1852", p_Main->getSecondFramePos(computerType_), wxSize(310, 180));
#else
    cdp1852FramePointer = new Cdp1852Frame("CDP1852", p_Main->getSecondFramePos(computerType_), wxSize(329, 180));
#endif
   
    if (microboardType_ == MICROBOARD_CDP18S605 || microboardType_ == MICROBOARD_CDP18S610)
    {
        Cdp18s600Configuration.usePio = false;
        return;
    }
    
    p_Main->message("Configuring CDP1852 on group 8");
    p_Main->message("	Output 2: write to port, input 2: read port");
    p_Main->message("	EF 3: STB");
    p_Main->message("");
}

void Cdp18s602::setCpuMode(int mode)
{
    int clear = (mode>>1)&1;
    int wait = mode&1;
    
    setWait(wait);
    setClear(clear);
    
    if (clear == 0)
        cdp1852FramePointer->reset();
    if (clear == 0)
    {
        if (Cdp18s600Configuration.useCdp18s660)
        {
            pioFramePointer1->reset();
            pioFramePointer2->reset();
        }
    }

    cdp18s640FramePointer->setRunLed(clear & wait);
    
    wait ^= 1;
    clear ^= 1;
    
    cdp18s640FramePointer->setStateLed(CLEARLED, clear);
    cdp18s640FramePointer->setStateLed(WAITLED, wait);
}

void Cdp18s602::cycle(int type)
{
    switch(cycleType_[type])
    {
        case 0:
            return;
            break;
            
        case FDCCYCLE:
            cycleUpd765();
        break;
            
        case VT100CYCLE:
            if (p_Vt100 != NULL)
                vtPointer->cycleVt();
        break;
            
        case VTSERIALCYCLE:
            if (p_Serial != NULL)
                p_Serial->cycleVt();
        break;
            
        case LEDCYCLE:
            cycleLed();
        break;
            
    }
}

void Cdp18s602::cycleLed()
{
    if (ledCycleValue_ > 0)
    {
        ledCycleValue_ --;
        if (ledCycleValue_ <= 0)
        {
            ledCycleValue_ = ledCycleSize_;
            cdp18s640FramePointer->ledTimeout();
            if (Cdp18s600Configuration.usePio)
                cdp1852FramePointer->ledTimeout();
        }
    }
}

Byte Cdp18s602::ef(int flag)
{
    int defaultRet = defaultEf(flag);
    if (defaultRet != -1)
        return defaultRet;
    
    switch(efType_[flag])
    {
        case 0:
            return 1;
        break;
            
        case VT100EF:       // EF4
            if (p_Vt100 != NULL)
                return vtPointer->ef();
        break;
            
        case VTSERIALEF:
            if (p_Serial != NULL)
                return p_Serial->ef();
        break;
            
        case CDP18SEF2:
            switch (ioGroup_)
            {
                case IO_GRP_TAPE:
                    if (Cdp18s600Configuration.useTape)
                        return cassetteEf_;
                break;
                    
                default:
                    return 1;
                break;
            }
        break;
            
        case MS2000EF:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                return efInterrupt();
            else
            {
                switch (ioGroup_)
                {
                    case IO_GRP_PIO:
                        if (Cdp18s600Configuration.usePio)
                            return pioEfState_[0][3];
                        else
                            return 1;
                    break;
                        
                    default:
                        return 1;
                    break;
                }
            }
        break;
            
        default:
            return 1;
    }
    return 1;
}

Byte Cdp18s602::in(Byte port, Word WXUNUSED(address))
{
    Byte ret;
    ret = 0;
    
    int defaultRet = defaultIn(port);
    if (defaultRet != -1)
        return defaultRet;
    
    switch(inType_[port])
    {
        case 0:
            ret = 255;
        break;
            
        case MS2000IOGROUP:
        break;
            
        case MS2000IO2:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    if (Cdp18s600Configuration.usePio)
                        return cdp1852FramePointer->readPort();
                break;
            }
        break;
            
        case MS2000IO3:
        break;
            
        case MS2000IO4:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                return inputMasterStatus();
        break;
            
        case MS2000IO5:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                return inputCommandStatus();
        break;
            
        case MS2000IO6:
        break;
            
        case MS2000IO7:
        break;
            
        default:
            ret = 255;
    }
    inValues_[port] = ret;
    return ret;
}

void Cdp18s602::out(Byte port, Word address, Byte value)
{
    outValues_[port] = value;
    
    defaultOut(port, address, value);
    switch(outType_[port])
    {
        case 0:
            return;
        break;
            
        case MS2000IOGROUP:
            ioGroup_ = value;
        break;
            
        case MS2000IO2:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    if (Cdp18s600Configuration.usePio)
                        cdp1852FramePointer->writePort(value);
                break;
            }
        break;
            
        case MS2000IO3:
        break;
            
        case MS2000IO4:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                outputDmaControl(value);
            switch (ioGroup_)
            {
                case IO_GRP_TAPE:
                    if (Cdp18s600Configuration.useTape)
                        tapeIo(value);
                break;
            }
        break;
            
        case MS2000IO5:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                outputCommand(value);
            switch (ioGroup_)
            {
                case IO_GRP_TAPE:
                    if (Cdp18s600Configuration.useTape)
                        psaveAmplitudeChange(value&1);
                break;
            }
        break;
            
        case MS2000IO6:
        break;
            
        case MS2000IO7:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                outputDmaCounter(value);
        break;
    }
}

void Cdp18s602::startPio(long ms)
{
    cdp1852FramePointer->setLedMs(ms);
    cdp1852FramePointer->Show(true);
}

void Cdp18s602::readRoms()
{
    Conf configuration = p_Main->getConfiguration(MICROBOARD);
    
    p_Main->setMemoryMapCDP18S602(&configuration, -1, -1);
}

void Cdp18s602::setLedMs(long ms)
{
    cdp18s640FramePointer->setLedMs(ms);
    cdp1852FramePointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((Cdp18s600ClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

void Cdp18s602::releaseButtonOnScreen2(HexButton* buttonPointer, int WXUNUSED(buttonType), int WXUNUSED(pioNumber))
{
    cdp1852FramePointer->releaseButtonOnScreen(buttonPointer);
}

void Cdp18s602::moveWindows()
{
    if (Cdp18s600Configuration.vtType != VTNONE)
        vtPointer->Move(p_Main->getVtPos(computerType_));
    if (Cdp18s600Configuration.usePio)
        cdp1852FramePointer->Move(p_Main->getSecondFramePos(computerType_));
    if (Cdp18s600Configuration.useElfControlWindows)
        cdp18s640FramePointer->Move(p_Main->getMainPos(computerType_));
}

void Cdp18s602::showPio(bool state)
{
    cdp1852FramePointer->Show(state);
    if (state)
        cdp1852FramePointer->refreshLeds();
}

Cdp18s603a::Cdp18s603a(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock, ElfConfiguration conf)
: Cdp18s601(title, pos, size, zoomLevel, computerType, clock, conf)
{
    
}

void Cdp18s603a::readRoms()
{
    Conf configuration = p_Main->getConfiguration(MICROBOARD);
    
    p_Main->setMemoryMapCDP18S603a(&configuration, -1, -1);
}

Cdp18s604b::Cdp18s604b(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock, ElfConfiguration conf)
: Cdp18s600(title, pos, size, zoomLevel, computerType, clock, conf)
{
    pioFramePointer = NULL;
    pioMessage_ = "CDP1852 & CD4536B";
}

Cdp18s604b::~Cdp18s604b()
{
    if (Cdp18s600Configuration.usePio)
        p_Main->setSecondFramePos(computerType_, cdp1852FramePointer->GetPosition());
    cdp1852FramePointer->Destroy();
}

void Cdp18s604b::configurePio()
{
#if defined (__WXMAC__) || (__linux__)
    cdp1852FramePointer = new Cdp1852Frame("CDP1852", p_Main->getSecondFramePos(computerType_), wxSize(310, 180));
#else
    cdp1852FramePointer = new Cdp1852Frame("CDP1852", p_Main->getSecondFramePos(computerType_), wxSize(329, 180));
#endif
    
    p_Main->message("Configuring CDP1852 on group 8");
    p_Main->message("	Output 2: write to port, input 2: read port");
    p_Main->message("	EF 3: STB");
    p_Main->message("");
    
    p_Main->message("Configuring CD4536B on group 8");
    p_Main->message("	Output 3: write control byte");
    p_Main->message("	EF 2: Timer out");
    p_Main->message("");
}

void Cdp18s604b::setCpuMode(int mode)
{
    int clear = (mode>>1)&1;
    int wait = mode&1;
    
    setWait(wait);
    setClear(clear);
    
    if (clear == 0)
        cdp1852FramePointer->reset();
    if (clear == 0)
    {
        if (Cdp18s600Configuration.useCdp18s660)
        {
            pioFramePointer1->reset();
            pioFramePointer2->reset();
        }
    }

    cdp18s640FramePointer->setRunLed(clear & wait);
    
    wait ^= 1;
    clear ^= 1;
    
    cdp18s640FramePointer->setStateLed(CLEARLED, clear);
    cdp18s640FramePointer->setStateLed(WAITLED, wait);
}

void Cdp18s604b::cycle(int type)
{
    switch(cycleType_[type])
    {
        case 0:
            return;
        break;
            
        case FDCCYCLE:
            cycleUpd765();
        break;
            
        case VT100CYCLE:
            if (p_Vt100 != NULL)
                vtPointer->cycleVt();
        break;
            
        case VTSERIALCYCLE:
            if (p_Serial != NULL)
                p_Serial->cycleVt();
        break;
            
        case LEDCYCLE:
            cycleLed();
        break;
            
    }
}

void Cdp18s604b::cycleLed()
{
    if (ledCycleValue_ > 0)
    {
        ledCycleValue_ --;
        if (ledCycleValue_ <= 0)
        {
            ledCycleValue_ = ledCycleSize_;
            cdp18s640FramePointer->ledTimeout();
            if (Cdp18s600Configuration.usePio)
                cdp1852FramePointer->ledTimeout();
        }
    }
    if (counterCycleValue_ > 0 && !counterPaused_)
    {
        counterCycleValue_ --;
        if (counterCycleValue_ <= 0)
        {
            if (counterSquare_)
            {
                efState_[2] = (efState_[2] ^ 0x1) & 0x1;
                counterCycleValue_ = counterCycleSize_/2;
            }
            else
            {
                efState_[2] = 0;
                counterCycleValue_ = counterCycleSize_;
            }
            p_Computer->interrupt();
        }
    }
}

Byte Cdp18s604b::ef(int flag)
{
    int defaultRet = defaultEf(flag);
    if (defaultRet != -1)
        return defaultRet;
    
    switch(efType_[flag])
    {
        case 0:
            return 1;
        break;
            
        case VT100EF:       // EF4
            if (p_Vt100 != NULL)
                return vtPointer->ef();
        break;
            
        case VTSERIALEF:
            if (p_Serial != NULL)
                return p_Serial->ef();
        break;
    
        case CDP18SEF2:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    return efState_[2];
                break;
                
                case IO_GRP_TAPE:
                    if (Cdp18s600Configuration.useTape)
                        return cassetteEf_;
                break;
                    
                default:
                    return 1;
                break;
            }
        break;

        case MS2000EF:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                return efInterrupt();
            else
            {
                switch (ioGroup_)
                {
                    case IO_GRP_PIO:
                        return pioEfState_[0][3];
                    break;
                    
                    default:
                        return 1;
                    break;
                }
            }
        break;
            
        default:
            return 1;
    }
    return 1;
}

Byte Cdp18s604b::in(Byte port, Word WXUNUSED(address))
{
    Byte ret;
    ret = 0;
    
    int defaultRet = defaultIn(port);
    if (defaultRet != -1)
        return defaultRet;
    
    switch(inType_[port])
    {
        case 0:
            ret = 255;
        break;
            
        case MS2000IOGROUP:
        break;
            
        case MS2000IO2:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    return cdp1852FramePointer->readPort();
                break;
            }
        break;
            
        case MS2000IO3:
        break;
            
        case MS2000IO4:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                return inputMasterStatus();
        break;
            
        case MS2000IO5:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                return inputCommandStatus();
        break;
            
        case MS2000IO6:
        break;
            
        case MS2000IO7:
        break;
            
        default:
            ret = 255;
    }
    inValues_[port] = ret;
    return ret;
}

void Cdp18s604b::out(Byte port, Word address, Byte value)
{
    outValues_[port] = value;
    
    defaultOut(port, address, value);
    switch(outType_[port])
    {
        case 0:
            return;
        break;
            
        case MS2000IOGROUP:
            ioGroup_ = value;
        break;
            
        case MS2000IO2:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    cdp1852FramePointer->writePort(value);
                break;
            }
        break;
            
        case MS2000IO3:
            switch (ioGroup_)
            {
                case IO_GRP_PIO:
                    counterControlByte(value);
                break;
            }
        break;
            
        case MS2000IO4:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                outputDmaControl(value);
            switch (ioGroup_)
            {
                case IO_GRP_TAPE:
                    if (Cdp18s600Configuration.useTape)
                        tapeIo(value);
                break;
            }
        break;
            
        case MS2000IO5:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                outputCommand(value);
            switch (ioGroup_)
            {
                case IO_GRP_TAPE:
                    if (Cdp18s600Configuration.useTape)
                        psaveAmplitudeChange(value&1);
                break;
            }
        break;
            
        case MS2000IO6:
        break;
            
        case MS2000IO7:
            if  (ioGroup_ == Cdp18s600Configuration.upd765Group)
                outputDmaCounter(value);
        break;
    }
}

void Cdp18s604b::counterControlByte(Byte value)
{
    efState_[2] = 1;

    if ((value & 0x10) == 0x10)
        counterCycleSize_ = 2 << (value&0xf);
    else
        counterCycleSize_ = 512 << (value&0xf);
    
    counterSquare_ = ((value & 0x30) == 0);

    counterCycleValue_ = counterCycleSize_ / 2;
    if ((value & 0x20) == 0x20) // Set
        counterCycleValue_ = counterCycleSize_ / 2;
    if ((value & 0x40) == 0x40) // Reset
        counterCycleValue_ = counterCycleSize_;

    counterPaused_ = ((value & 0x80) == 0x80);
}

void Cdp18s604b::startComputer()
{
    resetPressed_ = false;
    counterPaused_ = true;
    
    readRoms();
    
    if (computerType_ == MICROBOARD)
        configureCards();
    
    if (p_Vt100 != NULL)
        p_Vt100->Show(true);
    
    if (Cdp18s600Configuration.autoBoot)
        autoBoot();

    if (Cdp18s600Configuration.useUpd765)
        setDiskNames();
    
    if (Cdp18s600Configuration.usev1870)
    {
        if (configure1870Microboard(Cdp18s600Configuration.v1870Group, Cdp18s600Configuration.pageMemSize, Cdp18s600Configuration.v1870VideoMode, Cdp18s600Configuration.v1870InterruptMode))
        {
            readChargenFile(p_Main->getCharRomDir(MICROBOARD), p_Main->getCharRomFile(MICROBOARD));
        }
        
        Show(true);
        
        defineMemoryType(0xf400, 0xf7ff, CRAM1870);
        defineMemoryType(0xf800, 0xffff, PRAM1870);
    }
    
    p_Main->setSwName("");
    p_Main->updateTitle();
    
    cpuCycles_ = 0;
    p_Main->startTime();
    
    int ms = (int) p_Main->getLedTimeMs(computerType_);
    cdp18s640FramePointer->setLedMs(ms);
    
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((Cdp18s600ClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
    
    counterCycleSize_ = -1;
    
    if (Cdp18s600Configuration.usePio)
        startPio(ms);

    if (Cdp18s600Configuration.useElfControlWindows)
        cdp18s640FramePointer->Show(true);

    threadPointer->Run();
}

void Cdp18s604b::startPio(long ms)
{
    cdp1852FramePointer->setLedMs(ms);
    cdp1852FramePointer->Show(true);
}

void Cdp18s604b::readRoms()
{
    Conf configuration = p_Main->getConfiguration(MICROBOARD);
    
    p_Main->setMemoryMapCDP18S604b(&configuration, -1, -1);
}

void Cdp18s604b::writeMemDataType(Word address, Byte type)
{
    if (address < 0x8000)
        address = (address | addressLatch_);
    
    switch (memoryType_[address/256])
    {
        case ROM:
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
        break;
            
        case MAPPEDROM:
            if (mainMemoryDataType_[address&0xfdff] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address&0xfdff] = type;
            }
        break;
            
        case RAM:
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
        break;
    }
}

Byte Cdp18s604b::readMemDataType(Word address)
{
    if (address < 0x8000)
        address = (address | addressLatch_);
    
    switch (memoryType_[address/256])
    {
        case RAM:
        case ROM:
            return mainMemoryDataType_[address];
        break;
            
        case MAPPEDROM:
            return mainMemoryDataType_[address&0xfdff];
        break;
    }
    return MEM_TYPE_UNDEFINED;
}

Byte Cdp18s604b::readMemDebug(Word address)
{
    if ((address & 0x8000) == 0x8000)
        addressLatch_ = 0;
    
    if (address < 0x8000)
        address = (address | addressLatch_);
    
    switch (memoryType_[address / 256])
    {
        case UNDEFINED:
            return 255;
        break;
            
        case ROM:
        case RAM:
            return mainMemory_[address];
        break;
            
        case MAPPEDROM:
            return mainMemory_[address&0xfdff];
        break;
            
        default:
            return 255;
        break;
    }
}

void Cdp18s604b::writeMemDebug(Word address, Byte value, bool writeRom)
{
    switch (memoryType_[address/256])
    {
        case UNDEFINED:
        case ROM:
            if (writeRom)
                mainMemory_[address]=value;
        break;
            
        case RAM:
            if (mainMemory_[address]==value)
                return;
            mainMemory_[address]=value;
            if (address >= (memoryStart_) && address<((memoryStart_) +256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;
            
        case MAPPEDROM:
            if (writeRom)
                mainMemory_[address&0xfdff]=value;
        break;
    }
}

void Cdp18s604b::setLedMs(long ms)
{
    cdp18s640FramePointer->setLedMs(ms);
    cdp1852FramePointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((Cdp18s600ClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

void Cdp18s604b::releaseButtonOnScreen2(HexButton* buttonPointer, int WXUNUSED(buttonType), int WXUNUSED(pioNumber))
{
    cdp1852FramePointer->releaseButtonOnScreen(buttonPointer);
}

void Cdp18s604b::moveWindows()
{
    if (Cdp18s600Configuration.vtType != VTNONE)
        vtPointer->Move(p_Main->getVtPos(computerType_));
    if (Cdp18s600Configuration.usePio)
        cdp1852FramePointer->Move(p_Main->getSecondFramePos(computerType_));
    if (Cdp18s600Configuration.useElfControlWindows)
        cdp18s640FramePointer->Move(p_Main->getMainPos(computerType_));
}

void Cdp18s604b::showPio(bool state)
{
    cdp1852FramePointer->Show(state);
    if (state)
        cdp1852FramePointer->refreshLeds();
}

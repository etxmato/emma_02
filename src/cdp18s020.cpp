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

#include "main.h"
#include "cdp18s020.h"

#define SC0LED 0
#define SC1LED 1
#define WAITLED 2
#define CLEARLED 3

Cdp18s020Screen::Cdp18s020Screen(wxWindow *parent, const wxSize& size)
: Panel(parent, size)
{
}

Cdp18s020Screen::~Cdp18s020Screen()
{
    delete mainBitmapPointer;

#if defined (__WXMAC__)
    delete osx_text_runPButtonPointer;
    delete osx_text_runButtonPointer;
    delete osx_text_resetButtonPointer;
#else
    delete text_runPButtonPointer;
    delete text_runButtonPointer;
    delete text_resetButtonPointer;
#endif
    delete stepSwitchButton;

    for (int i=0; i<4; i++)
    {
        delete stateLedPointer[i];
        delete dipSwitchButton[i];
    }
    for (int i=0; i<24; i++)
    {
        delete ledPointer[i];
    }
    delete qLedPointer;
}

void Cdp18s020Screen::init()
{
    mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/dip_switch.png", wxBITMAP_TYPE_PNG);

    keyStart_ = 0;
    keyEnd_ = 0;
    lastKey_ = 0;
    
    wxClientDC dc(this);
    
    for (int i=8; i<24; i++)
    {
        ledPointer[i] = new Led(dc, 14+17*(23-i), 140, ELFLED);
        updateLed_[i]=true;
    }
    for (int i=0; i<8; i++)
    {
        ledPointer[i] = new Led(dc, 14+17*(7-i), 80, ELFLED);
        updateLed_[i]=true;
    }
    stateLedPointer[SC1LED] = new Led(dc, 167, 80, ELFLED);
    stateLedPointer[SC0LED] = new Led(dc, 184, 80, ELFLED);
    stateLedPointer[WAITLED] = new Led(dc, 227, 80, ELFLED);
    stateLedPointer[CLEARLED] = new Led(dc, 244, 80, ELFLED);
    qLedPointer = new Led(dc, 269, 80, ELFLED);

#if defined (__WXMAC__)
    osx_text_resetButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, 70, 30, "");
    osx_text_runPButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, 130, 30, "P");
    osx_text_runButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, 190, 30, "U");
#else
    text_resetButtonPointer = new wxButton(this, 3, "", wxPoint(70, 30), wxSize(25, 25), 0, wxDefaultValidator, "ResetButton");
    text_resetButtonPointer->SetToolTip("Reset");
    text_runPButtonPointer = new wxButton(this, 2, "P", wxPoint(130, 30), wxSize(25, 25), 0, wxDefaultValidator, "RunPButton");
    text_runPButtonPointer->SetToolTip("RUN-P");
    text_runButtonPointer = new wxButton(this, 1, "U", wxPoint(190, 30), wxSize(25, 25), 0, wxDefaultValidator, "RunButton");
    text_runButtonPointer->SetToolTip("RUN-U");
#endif
    stepSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 15, 30, "");
    dipSwitchButton[0] = new SwitchButton(dc, DIP_SWITCH_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 252, 25, "");
    dipSwitchButton[1] = new SwitchButton(dc, DIP_SWITCH_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 263, 25, "");
    dipSwitchButton[2] = new SwitchButton(dc, DIP_SWITCH_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 274, 25, "");
    dipSwitchButton[3] = new SwitchButton(dc, DIP_SWITCH_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 285, 25, "");

    this->connectKeyEvent(this);
}

void Cdp18s020Screen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);
    wxColourDatabase colour;

    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(0, 0, 310, 180);
#if defined(__WXMAC__)
	wxFont defaultFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
#else
	wxFont defaultFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
#endif

    dc.DrawBitmap(*mainBitmapPointer, 245, 13);
    wxString number;
    dc.SetTextForeground(*wxBLACK);
    dc.SetFont(defaultFont);
    
    dc.DrawText("STEP", 10, 8);
    dc.DrawText("RESET", 60, 8);
    dc.DrawText("RUN", 126, 8);
    dc.DrawText("RUN", 186, 8);

    dc.DrawText("DATA", 30, 62);
    dc.DrawText("BUS", 100, 62);
    dc.DrawText("STATUS", 180, 62);
    dc.DrawText("MEMORY", 45, 122);
    dc.DrawText("ADDRESS", 180, 122);

    dc.SetTextForeground(colour.Find("GREY"));
    dc.DrawText("15", 7, 152);
    dc.DrawText("8", 133, 152);
    dc.DrawText("7", 150, 152);
    dc.DrawText("0", 269, 152);
    
    dc.DrawText("SC1", 150, 92);
    dc.DrawText("SC0", 184, 92);
    dc.DrawText("W", 227, 92);
    dc.DrawText("C", 244, 92);
    dc.DrawText("Q", 269, 92);
    
    for (int i=0; i<8; i++)
    {
        number.Printf("%d", i);
        dc.DrawText(number, 14+17*(7-i), 92);
    }
    
    for (int i=0; i<24; i++)
        ledPointer[i]->onPaint(dc);
    for (int i=0; i<4; i++)
    {
        stateLedPointer[i]->onPaint(dc);
        dipSwitchButton[i]->onPaint(dc);
    }
    qLedPointer->onPaint(dc);

#if defined (__WXMAC__)
    osx_text_resetButtonPointer->onPaint(dc);
    osx_text_runButtonPointer->onPaint(dc);
    osx_text_runPButtonPointer->onPaint(dc);
#endif
    stepSwitchButton->onPaint(dc);
}

void Cdp18s020Screen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);

#if defined (__WXMAC__)
    if (osx_text_resetButtonPointer->onMousePress(dc, x, y))
        p_Computer->onReset();
    
    if (osx_text_runButtonPointer->onMousePress(dc, x, y))
        p_Computer->onRunButton();
    
    if (osx_text_runPButtonPointer->onMousePress(dc, x, y))
        p_Computer->onRunPButton();
#endif
}

void Cdp18s020Screen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
#if defined (__WXMAC__)
    osx_text_resetButtonPointer->onMouseRelease(dc, x, y);
    osx_text_runButtonPointer->onMouseRelease(dc, x, y);
    osx_text_runPButtonPointer->onMouseRelease(dc, x, y);
#endif
    if (stepSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onSingleStep();
    for (int i=0; i<4; i++)
        if (dipSwitchButton[i]->onMousePress(dc, x, y))
            p_Computer->onMpButton(i);
}

void Cdp18s020Screen::releaseButtonOnScreen(HexButton* buttonPoint)
{
    wxClientDC dc(this);
    
    buttonPoint->releaseButtonOnScreen(dc);
}

BEGIN_EVENT_TABLE(Cdp18s020, wxFrame)
    EVT_CLOSE (Cdp18s020::onClose)
    EVT_BUTTON(1, Cdp18s020::onRunButton)
    EVT_BUTTON(2, Cdp18s020::onRunButton)
    EVT_BUTTON(3, Cdp18s020::onResetButton)
END_EVENT_TABLE()

Cdp18s020::Cdp18s020(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	cdp18s020Configuration = conf;

	cdp18s020ClockSpeed_ = clock;
    
    this->SetClientSize(size);

    cdp18s020ScreenPointer = new Cdp18s020Screen(this, size);
    cdp18s020ScreenPointer->init();

}

Cdp18s020::~Cdp18s020()
{
    switch (loadedProgram_)
    {
        case NOPROGRAM:
        case FPBBOOT:
            p_Main->saveScrtValues("");
            break;
            
        case FPBBASIC:
            p_Main->saveScrtValues("FPBBASIC");
            break;
    }

    if (cdp18s020Configuration.vtType != VTNONE)
	{
		p_Main->setVtPos(CDP18S020, vtPointer->GetPosition());
		vtPointer->Destroy();
	}
    if (cdp18s020Configuration.vtExternal)
        delete p_Serial;
	p_Main->setMainPos(CDP18S020, GetPosition());
    delete cdp18s020ScreenPointer;
}

void Cdp18s020::onClose(wxCloseEvent&WXUNUSED(event) )
{
    p_Main->stopComputer();
}

void Cdp18s020::configureComputer()
{
    setCycleType(COMPUTERCYCLE, LEDCYCLE);

	p_Main->message("Configuring CDP18S020");
    p_Main->message("");

    if (cdp18s020Configuration.vtType != VTNONE)
	{
		double zoom = p_Main->getZoomVt();
        if (cdp18s020Configuration.vtType == VT52)
            vtPointer = new Vt100("CDP18S020 - VT 152", p_Main->getVtPos(CDP18S020), wxSize(640*zoom, 400*zoom), zoom, CDP18S020, cdp18s020ClockSpeed_, cdp18s020Configuration);
        else
            vtPointer = new Vt100("CDP18S020 - VT 100", p_Main->getVtPos(CDP18S020), wxSize(640*zoom, 400*zoom), zoom, CDP18S020, cdp18s020ClockSpeed_, cdp18s020Configuration);
		p_Vt100 = vtPointer;
        vtPointer->configureStandard(cdp18s020Configuration.baudR, cdp18s020Configuration.baudT, 4);
		vtPointer->Show(true);
	}

    if (cdp18s020Configuration.vtExternal)
    {
        p_Serial = new Serial(CDP18S020, cdp18s020ClockSpeed_, cdp18s020Configuration);
        p_Serial->configureStandard(cdp18s020Configuration.baudR, cdp18s020Configuration.baudT, 4);
    }

	resetCpu();
}

void Cdp18s020::initComputer()
{
    Show(p_Main->getUseCdp18s020ControlWindows());
    addressLatch_ = p_Main->getBootAddress("CDP18S020", computerType_);
    setClear(0);
    cdp18s020ScreenPointer->setStateLed(CLEARLED, 0);
	setWait(1);
    cdp18s020ScreenPointer->setStateLed(WAITLED, 1);

    for (int i=0; i<4; i++)
        mpButtonState_[i] = false;
    ef4State_ = 1;
}

void Cdp18s020::onRunButton(wxCommandEvent&WXUNUSED(event))
{
    onRunButton();
}

void Cdp18s020::onRunPButton(wxCommandEvent&WXUNUSED(event))
{
    onRunPButton();
}

void Cdp18s020::onRunButton()
{
    if (!singleStateStep_)
        addressLatch_ = 0x8000;
    onRun();
}

void Cdp18s020::onRunPButton()
{
    addressLatch_ = 0x0000;
    onRun();
}

void Cdp18s020::onRun()
{
    if (!singleStateStep_)
        resetCpu();
    
    if (cpuMode_ != RUN)
        resetEffectiveClock();

    setClear(1);
    cdp18s020ScreenPointer->setStateLed(CLEARLED, 1);
    setWait(1);
    cdp18s020ScreenPointer->setStateLed(WAITLED, 1);

    p_Main->eventUpdateTitle();
}

void Cdp18s020::autoBoot()
{
    setClear(1);
    cdp18s020ScreenPointer->setStateLed(CLEARLED, 1);
    if (cpuMode_ == RESET)
        showAddress(addressLatch_);
}

void Cdp18s020::setAddressLatch(Word bootAddress)
{
    addressLatch_ = bootAddress;
}

void Cdp18s020::onMpButton(int buttonNumber)
{
    mpButtonState_[buttonNumber] = !mpButtonState_[buttonNumber];
}

void Cdp18s020::onSingleStep()
{
    singleStateStep_ = !singleStateStep_;
    if (singleStateStep_)
    {
        setMsValue_ = (int) p_Main->getLedTimeMs(CDP18S020);
        setLedMs(0);
    }
    else
        setLedMs(setMsValue_);
}

Byte Cdp18s020::ef(int flag)
{
    if (flag == 4)
    {
        if (ef4State_ == 0)
            return ef4State_;
    }
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

        case VT100EF:
            return vtPointer->ef();
		break;

        case VTSERIALEF:
            return p_Serial->ef();
        break;
 
		default:
			return 1;
	}
}

Byte Cdp18s020::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Cdp18s020::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;

	switch(outType_[port])
	{
		case 0:
			return;
		break;

		case VT100OUT:
			vtPointer->out(value);
		break;

		case VTOUTSERIAL:
			p_Serial->out(value);
		break;
	}
}

void Cdp18s020::switchQ(int value)
{
    cdp18s020ScreenPointer->setQLed(value);

    if (cdp18s020Configuration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (cdp18s020Configuration.vtExternal)
        p_Serial->switchQ(value);
}

void Cdp18s020::showCycleData(Byte val)
{
    for (int i=0; i<8; i++)
    {
        cdp18s020ScreenPointer->setLed(i, val&1);
        val = val >> 1;
    }
}

void Cdp18s020::showAddress(Word val)
{
    for (int i=8; i<24; i++)
    {
        cdp18s020ScreenPointer->setLed(i, val&1);
        val = val >> 1;
    }
}

void Cdp18s020::showState(int state)
{
    cdp18s020ScreenPointer->setStateLed(SC0LED, state&1);
    cdp18s020ScreenPointer->setStateLed(SC1LED, (state&2)>>1);
}

void Cdp18s020::setWaitLed()
{
    cdp18s020ScreenPointer->setStateLed(WAITLED, 0);
}

void Cdp18s020::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

        case VT100CYCLE:
            vtPointer->cycleVt();
            break;
            
        case VTSERIALCYCLE:
            p_Serial->cycleVt();
        break;

        case LEDCYCLE:
            cycleLed();
        break;
    }
}

void Cdp18s020::cycleLed()
{
    if (ledCycleValue_ > 0)
    {
        ledCycleValue_ --;
        if (ledCycleValue_ <= 0)
        {
            ledCycleValue_ = ledCycleSize_;
            cdp18s020ScreenPointer->ledTimeout();
        }
    }
}

void Cdp18s020::startComputer()
{
	resetPressed_ = false;
	Word lastAddress;

    p_Main->checkAndReInstallMainRom(CDP18S020);
	readProgram(p_Main->getRomDir(CDP18S020, MAINROM1), p_Main->getRomFile(CDP18S020, MAINROM1), ROM, 0x8000, &lastAddress, NONAME);

    ramMask_ = ((1<<p_Main->getRamType(computerType_))<<8)-1;

    defineMemoryType(0, ramMask_, RAM);
    initRam(0, ramMask_);

    defineMemoryType(0x8c00, 0x8c1f, REGSTORAGE);
    initRam(0x8c00, 0x8c1f);
    
	p_Main->assDefault("mycode", 0, ramMask_);

	readProgram(p_Main->getRamDir(CDP18S020), p_Main->getRamFile(CDP18S020), NOCHANGE, 0, SHOWNAME);

    if (cdp18s020Configuration.autoBoot)
        autoBoot();
    
    p_Main->setSwName("");
	p_Main->updateTitle();

	cpuCycles_ = 0;
    p_Main->startTime();

    int ms = (int) p_Main->getLedTimeMs(CDP18S020);
    cdp18s020ScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((cdp18s020ClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
    
    cpuState_ = STATE_FETCH_1;
    
	threadPointer->Run();
}

void Cdp18s020::writeMemDataType(Word address, Byte type)
{
	if (address < 0x8000)
		address = (address | addressLatch_);

	switch (memoryType_[address/256])
	{
		case RAM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;

        case REGSTORAGE:
            if (address <= 0x8c1F)
            {
                if (mainMemoryDataType_[address] != type)
                {
                    p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                    mainMemoryDataType_[address] = type;
                }
            }
        break;
            
		case ROM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;
	}
}

Byte Cdp18s020::readMemDataType(Word address)
{
	if (address < 0x8000)
		address = (address | addressLatch_);

	switch (memoryType_[address/256])
	{
		case RAM:
			return mainMemoryDataType_[address];
		break;

        case REGSTORAGE:
            if (address <= 0x8c1F)
                return mainMemoryDataType_[address];
        break;

		case ROM:
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Cdp18s020::readMem(Word address)
{
    address_ = address;
    return readMemDebug(address);
}

Byte Cdp18s020::readMemDebug(Word address)
{
	if ((address & 0x8000) == 0x8000)
		addressLatch_ = 0;

	if (address < 0x8000)
		address = (address | addressLatch_);

	switch (memoryType_[address/256])
	{
		case RAM:
			return mainMemory_[address];
		break;

        case REGSTORAGE:
            if (address <= 0x8c1F)
                return mainMemory_[address];
            else
                return 255;
        break;

        case UNDEFINED:
			return 255;
		break;
	}
	return mainMemory_[address];
}

void Cdp18s020::writeMem(Word address, Byte value, bool writeRom)
{
    address_ = address;
    writeMemDebug(address, value, writeRom);
}

void Cdp18s020::writeMemDebug(Word address, Byte value, bool writeRom)
{
	switch (memoryType_[address/256])
	{
		case RAM:
            if (!mpButtonState_[(address>>10)&0x3])
            {
                if (mainMemory_[address]==value)
                    return;
                mainMemory_[address]=value;
                if (address >= memoryStart_ && address<(memoryStart_+256))
                    p_Main->updateDebugMemory(address);
                p_Main->updateAssTabCheck(address);
            }
		break;
            
        case REGSTORAGE:
            if (address <= 0x8c1F)
            {
                mainMemory_[address]=value;
                if (address >= memoryStart_ && address<(memoryStart_+256))
                    p_Main->updateDebugMemory(address);
                p_Main->updateAssTabCheck(address);
            }
        break;

		default:
			if (writeRom)
				mainMemory_[address]=value;
		break;
	}
}

void Cdp18s020::cpuInstruction()
{
    if (cpuMode_ == RUN)
    {
        cpuCycleStep();
    }
    else
    {
        if (resetPressed_)
            resetPressed();
    }
}

void Cdp18s020::resetPressed()
{
    resetCpu();
    cpuState_ = STATE_FETCH_1;
    setClear(0);
    cdp18s020ScreenPointer->setStateLed(CLEARLED, 0);
    setWait(1);
    cdp18s020ScreenPointer->setStateLed(WAITLED, 1);
    showCycleData(0);
    if (cdp18s020Configuration.autoBoot)
        autoBoot();
    resetPressed_ = false;
}

void Cdp18s020::moveWindows()
{
	if (cdp18s020Configuration.vtType != VTNONE)
		vtPointer->Move(p_Main->getVtPos(CDP18S020));
}

void Cdp18s020::updateTitle(wxString Title)
{
    if (cdp18s020Configuration.vtType == VT52)
        vtPointer->SetTitle("CDP18S020 - VT 52" + Title);
    if (cdp18s020Configuration.vtType == VT100)
        vtPointer->SetTitle("CDP18S020 - VT 100" + Title);
}

void Cdp18s020::onResetButton(wxCommandEvent&WXUNUSED(event))
{
    onReset();
}

void Cdp18s020::onReset()
{
	resetPressed_ = true;
}

void Cdp18s020::sleepComputer(long ms)
{
	threadPointer->Sleep(ms);
}

void Cdp18s020::setLedMs(long ms)
{
    cdp18s020ScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((cdp18s020ClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

void Cdp18s020::terminalSave(wxString fileName)
{
    if (cdp18s020Configuration.vtType != VTNONE)
        vtPointer->terminalSaveCdp18s020Vt(fileName);
}

void Cdp18s020::terminalLoad(wxString filePath, wxString fileName, bool binaryFile)
{
    if (cdp18s020Configuration.vtType != VTNONE)
        vtPointer->terminalLoadCdp18s020Vt(filePath, binaryFile);
}

void Cdp18s020::terminalStop()
{
    if (cdp18s020Configuration.vtType != VTNONE)
        vtPointer->terminalStopVt();
}

void Cdp18s020::releaseButtonOnScreen(HexButton* buttonPointer, int WXUNUSED(buttonType))
{
    cdp18s020ScreenPointer->releaseButtonOnScreen(buttonPointer);
}

void Cdp18s020::setForceUpperCase(bool status)
{
    if (cdp18s020Configuration.vtType != VTNONE)
        vtPointer->setForceUCVt(status);
}



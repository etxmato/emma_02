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
#include "velf.h"

VelfScreen::VelfScreen(wxWindow *parent, const wxSize& size)
: Panel(parent, size)
{
}

VelfScreen::~VelfScreen()
{
//    delete mainBitmapPointer;
    delete runSwitchButton;
    delete mpSwitchButton;
    delete velfSwitchButton;
    delete loadSwitchButton;
    delete inSwitchButton;
    
    for (int i=0; i<8; i++)
    {
        delete dataSwitchButton[i];
        delete ledPointer[i];
    }
    for (int i=0; i<2; i++)
    {
        delete dataPointer[i];
    }
}

void VelfScreen::init()
{
    keyStart_ = 0;
    keyEnd_ = 0;
    lastKey_ = 0;
    forceUpperCase_ = p_Main->getUpperCase(ELF);
    
    wxClientDC dc(this);
    
    for (int i=0; i<8; i++)
    {
        dataSwitchButton[i] = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 18+34*(7-i), 116, "");
        ledPointer[i] = new Led(dc, 24+34*(7-i), 100, ELFLED);
        dataSwitchState_[i]=0;
   }
    
    inSwitchButton = new SwitchButton(dc, PUSH_BUTTON, wxColour(255, 255, 255), BUTTON_UP, 18, 40, "");
    loadSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 52, 40, "");
    
    if (p_Main->getVelfMode() == 0)
    {
        velfSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_UP, 188, 40, "");
        runSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_UP, 256, 40, "");
    }
    else
    {
        velfSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 188, 40, "");
        runSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 256, 40, "");
    }
    mpSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 222, 40, "");

    for (int i=0; i<2; i++)
    {
        dataPointer[i] = new Til311();
        dataPointer[i]->init(dc, 104+i*25, 40);
    }
    updateData_ = true;

    this->connectKeyEvent(this);
}

void VelfScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);
    
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(0, 0, 310, 180);
#if defined(__WXMAC__)
	wxFont defaultFont(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
#else
	wxFont defaultFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
#endif

    wxString number;
    dc.SetTextForeground(*wxBLACK);
    for (int i=0; i<8; i++)
    {
        number.Printf("%d", i);
        dc.SetFont(defaultFont);
        dc.DrawText(number, 24+34*(7-i), 142);
    }

    dc.DrawText("IN", 20, 18);
    dc.DrawText("LOAD", 48, 18);
    dc.DrawText("VIP", 184, 18);
    dc.DrawText("ELF", 184, 66);
    dc.DrawText("MP", 222, 18);
    dc.DrawText("RUN", 256, 18);

    for (int i=0; i<2; i++)
    {
        dataPointer[i]->onPaint(dc);
    }
    
    inSwitchButton->onPaint(dc);
    runSwitchButton->onPaint(dc);
    mpSwitchButton->onPaint(dc);
    velfSwitchButton->onPaint(dc);
    loadSwitchButton->onPaint(dc);
    for (int i=0; i<8; i++)
    {
        dataSwitchButton[i]->onPaint(dc);
        ledPointer[i]->onPaint(dc);
    }
}

void VelfScreen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
    if (inSwitchButton->onMousePress(dc, x, y))
        p_Computer->onInButtonPress();
}

void VelfScreen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
    if (inSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onInButtonRelease();
    if (runSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onRun();
    if (mpSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onMpButton();
    if (velfSwitchButton->onMouseRelease(dc, x, y))
        p_Main->switchVelfMode();
    if (loadSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onLoadButton();
    for (int i=0; i<8; i++)
    {
        if (dataSwitchButton[i]->onMouseRelease(dc, x, y))
        {
            p_Computer->dataSwitch(i);

            if (dataSwitchState_[i])
                dataSwitchState_[i] = 0;
            else
                dataSwitchState_[i] = 1;

            ledPointer[i]->setStatus(dc, dataSwitchState_[i]);
        }
    }
}

BEGIN_EVENT_TABLE(Velf, wxFrame)
    EVT_CLOSE (Velf::onClose)
END_EVENT_TABLE()

Velf::Velf(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	vipConfiguration = conf;

	velfClockSpeed_ = clock;
	p_Printer = new Printer();
	p_Printer->initVip(p_Printer);
    
    this->SetClientSize(size);

    velfScreenPointer = new VelfScreen(this, size);
    velfScreenPointer->init();

}

Velf::~Velf()
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

    p_Printer->closeFrames();
	delete p_Printer;

    p_Main->setPixiePos(VELF, pixiePointer->GetPosition());
    pixiePointer->Destroy();

    if (vipConfiguration.vtType != VTNONE)
	{
		p_Main->setVtPos(VELF, vtPointer->GetPosition());
		vtPointer->Destroy();
	}
    if (vipConfiguration.vtExternal)
        delete p_Serial;
	p_Main->setMainPos(VELF, GetPosition());
    delete velfScreenPointer;
}

void Velf::onClose(wxCloseEvent&WXUNUSED(event) )
{
    p_Main->stopComputer();
}

void Velf::configureComputer()
{
	outType_[2] = VIPKEYOUT;
	outType_[4] = VIPOUT4;
	outType_[7] = VIPIIOUT7;
    inType_[4] = ELFIN;
	efType_[3] = VIPKEYEF;
    setCycleType(COMPUTERCYCLE, LEDCYCLE);

	p_Main->message("Configuring VELF");
	p_Main->message("	Output 2: hex key latch, output 4: display output");
	p_Main->message("	output 7: cassette on/off, input 4: data input");

	cycleType_[KEYCYCLE] = VIPIIKEYCYCLE;

	p_Main->message("	EF 2: cassette in, EF 3: hex keypad");
    p_Main->message("	EF 4: 0 when in button pressed\n");
    
	usePrinter_ = false;
	if (p_Main->getPrinterStatus(VELF))
	{
		outType_[3] = VIPOUT3;
		usePrinter_ = true;
		p_Main->message("Configuring Centronics P-1/PR-40 Printer");
		p_Main->message("	Output 3: latch, Q: strobe, EF 3: busy\n");
	}
    
    if (vipConfiguration.vtType != VTNONE)
	{
		double zoom = p_Main->getZoomVt();
        if (vipConfiguration.vtType == VT52)
            vtPointer = new Vt100("VELF - VT 152", p_Main->getVtPos(VELF), wxSize(640*zoom, 400*zoom), zoom, VELF, velfClockSpeed_, vipConfiguration);
        else
            vtPointer = new Vt100("VELF - VT 100", p_Main->getVtPos(VELF), wxSize(640*zoom, 400*zoom), zoom, VELF, velfClockSpeed_, vipConfiguration);
		p_Vt100 = vtPointer;
        vtPointer->configureStandard(vipConfiguration.baudR, vipConfiguration.baudT, 2);
		vtPointer->Show(true);
	}

    if (vipConfiguration.vtExternal)
    {
        p_Serial = new Serial(VELF, velfClockSpeed_, vipConfiguration);
        p_Serial->configureStandard(vipConfiguration.baudR, vipConfiguration.baudT, 2);
    }

    defineKeys();
	resetCpu();
}

void Velf::defineKeys()
{
	player2defined_ = false;
	p_Main->getDefaultHexKeys(VELF, "Velf", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
    inKey1_ = p_Main->getDefaultInKey1("Velf");
    inKey2_ = p_Main->getDefaultInKey2("Velf");

	if (p_Main->getConfigBool("/Velf/GameAuto", true))
		p_Main->loadKeyDefinition(p_Main->getRamFile(VELF), p_Main->getChip8SW(VELF), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");

	for (int i=0; i<512; i++)
	{
		keyDefinition[i].defined = false;
	}
	for (int i=0; i<16; i++)
	{
		if (keyDefA1_[i] != 0)
		{
			keyDefinition[keyDefA1_[i]].defined = true;
			keyDefinition[keyDefA1_[i]].player = 0;
			keyDefinition[keyDefA1_[i]].key = i;
		}
		if (keyDefA2_[i] != 0)
		{
			keyDefinition[keyDefA2_[i]].defined = true;
			keyDefinition[keyDefA2_[i]].player = 0;
			keyDefinition[keyDefA2_[i]].key = i;
		}
	}
	redefineKeys_ = false;

}

void Velf::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
	for (int i=0; i<512; i++)
	{
		keyDefinition[i].defined = false;
	}
	for (int i=0; i<16; i++)
	{
		keyDefA1_[i] = hexKeyDefA1[i];
		if (hexKeyDefA1[i] != 0)
		{
			keyDefinition[keyDefA1_[i]].defined = true;
			keyDefinition[keyDefA1_[i]].player = 0;
			keyDefinition[keyDefA1_[i]].key = i;
		}
		keyDefA2_[i] = hexKeyDefA2[i];
		if (hexKeyDefA2[i] != 0)
		{
			keyDefinition[keyDefA2_[i]].defined = true;
			keyDefinition[keyDefA2_[i]].player = 0;
			keyDefinition[keyDefA2_[i]].key = i;
		}
	}
}

void Velf::initComputer()
{
    Show(p_Main->getUseVelfControlWindows());
    if (p_Main->getVelfMode() == 0)
        addressLatch_ = 0x8000;
    else
        addressLatch_ = 0;
    runButtonState_ = 0;
    setClear(0);
	setWait(1);
	cassetteEf_ = 0;

    mpButtonState_ = 0;
    loadButtonState_ = 1;
    for (int i=0; i<8; i++)  dataSwitchState_[i]=0;

	vipKeyPort_ = 0;
	for (int i=0; i<16; i++)
	{
		vipKeyState_[0][i] = 0;
		vipKeyState_[1][i] = 0;
	}

    
    ef4State_ = 1;
	runPressed_ = false;
	stateQ_ = 0;
	printLatch_ = 0;

	vipRunCommand_ = 0;
	vipRunState_ = RESETSTATE;
}

void Velf::keyDown(int keycode)
{
	if (keyDefinition[keycode].defined)
		vipKeyState_[keyDefinition[keycode].player][keyDefinition[keycode].key] = 1;
}

void Velf::keyUp(int keycode)
{
	if (keyDefinition[keycode].defined)
		vipKeyState_[keyDefinition[keycode].player][keyDefinition[keycode].key] = 0;
}

void Velf::onInButtonPress()
{
    if (cpuMode_ == LOAD)
    {
        Byte value = getData();
        dmaIn(value);
        showData(value);
    }
    else
    {
        ef4State_ = 0;
    }
    velfScreenPointer->inSetState(BUTTON_DOWN);
}

void Velf::onInButtonRelease()
{
    ef4State_ = 1;
    velfScreenPointer->inSetState(BUTTON_UP);
}

int Velf::getMpButtonState()
{
    return mpButtonState_;
}

void Velf::onRun()
{
	runPressed_ = true;
}

void Velf::autoBoot()
{
    velfScreenPointer->runSetState(BUTTON_UP);
    runButtonState_ = 1;
    setClear(runButtonState_);
}

void Velf::onMpButton()
{
    if (mpButtonState_)
    {
        mpButtonState_ = 0;
    }
    else
    {
        mpButtonState_ = 1;
    }
}

void Velf::onLoadButton()
{
    if (!loadButtonState_)
    {
        loadButtonState_ = 1;
    }
    else
    {
        loadButtonState_ = 0;
    }
    setWait(loadButtonState_);
    if (cpuMode_ == RESET)
        velfScreenPointer->showAddress(0);
}

void Velf::dataSwitch(int i)
{
    if (dataSwitchState_[i])
        dataSwitchState_[i] = 0;
    else
        dataSwitchState_[i] = 1;
}

Byte Velf::ef(int flag)
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

		case PIXIEEF:
			return pixiePointer->efPixie();
		break;

		case VIPEF2:
			return cassetteEf_;
		break;

		case VIPKEYEF:
			return ef3();
		break;

        case VT100EF:
            if (isLoading() || realCassetteLoad_)
                return cassetteEf_;
            else
                return vtPointer->ef();
		break;

        case VTSERIALEF:
            if (isLoading() || realCassetteLoad_)
                return cassetteEf_;
            else
	            return p_Serial->ef();
        break;
 
		default:
			return 1;
	}
}

Byte Velf::ef3()
{
	return (vipKeyState_[0][vipKeyPort_]) ? 0 : 1;
}

Byte Velf::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

        case ELFIN:
            ret = getData();
        break;
            
		case PIXIEIN:
			ret = pixiePointer->inPixie();
		break;

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

Byte Velf::getData()
{
    return(dataSwitchState_[7]?128:0) +(dataSwitchState_[6]?64:0) +(dataSwitchState_[5]?32:0) +(dataSwitchState_[4]?16:0) +(dataSwitchState_[3]?8:0) +(dataSwitchState_[2]?4:0) +(dataSwitchState_[1]?2:0) +(dataSwitchState_[0]?1:0);
}

void Velf::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;

	switch(outType_[port])
	{
		case 0:
			return;
		break;

        case PIXIEOUT:
            pixiePointer->outPixie();
        break;
            
		case VIPKEYOUT:
			outVelf(value);
		break;

		case VIPOUT3:
			printLatch_ = value;
		break;

        case VIPOUT4:
            showData(value);
        break;
            
		case VT100OUT:
			vtPointer->out(value);
		break;

		case VTOUTSERIAL:
			p_Serial->out(value);
		break;

		case VIPIIOUT7:
			if (value == 1)
			{
				p_Main->startCassetteLoad(0);
				return;
			}
			if (value == 2)
			{
				p_Main->startCassetteSave(0);
				return;
			}
			p_Main->stopCassette();
		break;
	}
}

void Velf::outVelf(Byte value)
{
	vipKeyPort_ = value&0xf;
}

void Velf::switchQ(int value)
{
    if (vipConfiguration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (vipConfiguration.vtExternal)
        p_Serial->switchQ(value);

	if (!usePrinter_)  return;

	if (value == 0 && stateQ_ == 1 && printLatch_ != 0)
		p_Printer->printerOut(printLatch_);
	stateQ_ = value;
}

void Velf::showData(Byte val)
{
    velfScreenPointer->showData(val);
}

void Velf::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

		case PIXIECYCLE:
			pixiePointer->cyclePixie();
		break;

        case VT100CYCLE:
            vtPointer->cycleVt();
            break;
            
        case VTSERIALCYCLE:
            p_Serial->cycleVt();
        break;

		case VIPIIKEYCYCLE:
			cycleKey();
		break;

        case LEDCYCLE:
            cycleLed();
        break;
    }
}

void Velf::cycleKey()
{
	if (vipRunCommand_ != 0)
	{
		if (scratchpadRegister_[programCounter_] == 0x10)
		{
			if (vipRunCommand_ == 1)
			{
				vipKeyState_[0][1] = 1;
			}
		}
		if (scratchpadRegister_[programCounter_] == 0x17)
		{
			if (vipRunCommand_ == 1)
			{
				vipKeyState_[0][1] = 0;
			}
		}
	}
}

void Velf::cycleLed()
{
    if (ledCycleValue_ > 0)
    {
        ledCycleValue_ --;
        if (ledCycleValue_ <= 0)
        {
            ledCycleValue_ = ledCycleSize_;
            velfScreenPointer->ledTimeout();
        }
    }
}

void Velf::startComputer()
{
	resetPressed_ = false;
	Word lastAddress;

    double zoom = p_Main->getZoom();
    double scale = p_Main->getScale();
    pixiePointer = new Pixie( "VELF - CDP1861", p_Main->getPixiePos(VELF), wxSize(64*zoom*scale, 192*zoom), zoom, scale, VELF);
    p_Video = pixiePointer;

    if (wxFile::Exists(p_Main->getRomDir(VIP, MAINROM1) + "cosmac.ram"))
		readFile(p_Main->getRomDir(VIP, MAINROM1) + "cosmac.ram", RAM, 0xf00, 0x10000, NONAME);

    p_Main->checkAndReInstallMainRom(VELF);
	readProgram(p_Main->getRomDir(VELF, MAINROM1), p_Main->getRomFile(VELF, MAINROM1), ROM, 0x8000, &lastAddress, NONAME);

	defineMemoryType(0x0, 0x7FFF, RAM);
    initRam(0x0, 0x7FFF);
	p_Main->assDefault("mycode", 0, 0xFFF);

	readProgram(p_Main->getRamDir(VELF), p_Main->getRamFile(VELF), NOCHANGE, 0, SHOWNAME);
    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);

    if (pseudoType_ == "CHIP8")
		readProgram(p_Main->getChip8Dir(VELF), p_Main->getChip8SW(VELF), NOCHANGE, 0x200, SHOWNAME);
	else
	{
        if (pseudoType_ == "CHIP8X")
			readProgram(p_Main->getChip8Dir(VELF), p_Main->getChip8SW(VELF), NOCHANGE, 0x300, SHOWNAME);
		else
			readProgram(p_Main->getChip8Dir(VELF), p_Main->getChip8SW(VELF), NOCHANGE, 0x200, SHOWNAME);
	}


    if (vipConfiguration.autoBoot)
        autoBoot();
    else
        velfScreenPointer->runSetState(BUTTON_DOWN);
    
    pixiePointer->configurePixieVelf();
    pixiePointer->initPixie();
    pixiePointer->setZoom(zoom);
    pixiePointer->Show(true);

    p_Main->setSwName("");
	p_Main->updateTitle();

	cpuCycles_ = 0;
    p_Main->startTime();

    int ms = (int) p_Main->getLedTimeMs(VELF);
    velfScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((velfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
    
	threadPointer->Run();
}

void Velf::writeMemDataType(Word address, Byte type)
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

		case ROM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;
	}
}

Byte Velf::readMemDataType(Word address)
{
	if (address < 0x8000)
		address = (address | addressLatch_);

	switch (memoryType_[address/256])
	{
		case RAM:
			return mainMemoryDataType_[address];
		break;


		case ROM:
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Velf::readMem(Word address)
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

		case UNDEFINED:
			return 255;
		break;
	}
	return mainMemory_[address];
}

Byte Velf::readMemDebug(Word address)
{
    return readMem(address);
}

void Velf::writeMem(Word address, Byte value, bool writeRom)
{
	switch (memoryType_[address/256])
	{
		case RAM:
			if (mainMemory_[address]==value)
				return;
            if (!getMpButtonState())
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

void Velf::writeMemDebug(Word address, Byte value, bool writeRom)
{
    writeMem(address, value, writeRom);
}

void Velf::cpuInstruction()
{
	if (cpuMode_ == RUN)
	{
        cpuCycleStep();
		if (runPressed_)
		{
			setClear(0);
			p_Main->eventUpdateTitle();
            velfScreenPointer->runSetState(BUTTON_DOWN);
			runPressed_ = false;
		}
	}
	else
	{
		if (runPressed_)
		{
			setClear(1);
			p_Main->eventUpdateTitle();
            velfScreenPointer->runSetState(BUTTON_UP);
            if (p_Main->getVelfMode() == 0)
                addressLatch_ = 0x8000;
            else
                addressLatch_ = 0;
			pixiePointer->initPixie();
			runPressed_ = false;
		}
	}
}

void Velf::resetPressed()
{
    resetCpu();
    resetPressed_ = false;
    if (p_Main->getVelfMode() == 0)
        addressLatch_ = 0x8000;
    else
        addressLatch_ = 0;
    pixiePointer->initPixie();
    vipRunState_ = RESETSTATE;
}

void Velf::moveWindows()
{
    pixiePointer->Move(p_Main->getPixiePos(VELF));
	if (vipConfiguration.vtType != VTNONE)
		vtPointer->Move(p_Main->getVtPos(VELF));
}

void Velf::updateTitle(wxString Title)
{
    pixiePointer->SetTitle("VELF - CDP1861"+Title);
    if (vipConfiguration.vtType == VT52)
        vtPointer->SetTitle("VELF - VT 52" + Title);
    if (vipConfiguration.vtType == VT100)
        vtPointer->SetTitle("VELF - VT 100" + Title);
}

void Velf::onReset()
{
	resetPressed_ = true;
}

void Velf::checkComputerFunction()
{
    switch(scratchpadRegister_[programCounter_])
    {
        case 0x80c0:
        case 0x80ed:
            p_Main->stopCassette();
        break;
            
        case 0x8091:	// SAVE
            p_Main->startCassetteSave(0);
        break;
            
        case 0x80c2:	// LOAD
            p_Main->startCassetteLoad(0);
        break;
    }
}

void Velf::startComputerRun(bool load)
{
	load_ = load;
	if (vipRunState_ == RESETSTATE)
		vipRunCommand_ = 1;
	else
		vipRunCommand_ = 2;
}

void Velf::sleepComputer(long ms)
{
	threadPointer->Sleep(ms);
}

void Velf::setLedMs(long ms)
{
    velfScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((velfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}



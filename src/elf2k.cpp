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

#include "elf2k.h"

const int freq [16] = {0, 256, 128, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2};

Elf2KScreen::Elf2KScreen(wxWindow *parent, const wxSize& size)
: Panel(parent, size)
{
}

Elf2KScreen::~Elf2KScreen()
{
	delete mainBitmapPointer;

	delete powerSwitchButton;

	for (int i=0; i<4; i++)
	{
		delete addressPointer[i];
	}
	for (int i=0; i<2; i++)
	{
		delete dataPointer[i];
	}
 	delete runLedPointer;
 	delete loadLedPointer;
 	delete qLedPointer;
// 	delete sc0LedPointer;
//	delete sc1LedPointer;
}

void Elf2KScreen::init()
{
	keyStart_ = 0;
	keyEnd_ = 0;
	lastKey_ = 0;
	forceUpperCase_ = p_Main->getUpperCase(ELF2K);

	wxClientDC dc(this);
	wxString switchNumber;

	mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/Elf2K.png", wxBITMAP_TYPE_PNG);
	powerSwitchButton = new SwitchButton(dc, ELF2K_POWER_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 480, 71, "");

	for (int i=0; i<4; i++)
	{
		addressPointer[i] = new Til311();
		addressPointer[i]->init(dc, 147+i*35, 354);
	}
	updateAddress_ = true;
	for (int i=0; i<2; i++)
	{
		dataPointer[i] = new Til311();
		dataPointer[i]->init(dc, 301+i*36, 354);
	}
	updateData_ = true;
	runLedPointer = new Led(dc, 48, 309, ELF2KLED1);
	updateRunLed_ = true;
	loadLedPointer = new Led(dc, 51, 333, ELF2KLED2);
	updateLoadLed_ = true;
	qLedPointer = new Led(dc, 50, 354, ELF2KLED3);
	updateQLed_ = true;
//	sc0LedPointer = new Led(dc, 50, 379, ELF2K);
//	sc1LedPointer = new Led(dc, 50, 401, ELF2K);
	this->connectKeyEvent(this);
}

void Elf2KScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
	wxPaintDC dc(this);
	dc.DrawBitmap(*mainBitmapPointer, 0, 0);

	for (int i=0; i<4; i++)
	{
		addressPointer[i]->onPaint(dc);
	}
	for (int i=0; i<2; i++)
	{
		dataPointer[i]->onPaint(dc);
	}
	runLedPointer->onPaint(dc);
	loadLedPointer->onPaint(dc);
	qLedPointer->onPaint(dc);
	powerSwitchButton->onPaint(dc);
//	sc0LedPointer->onPaint(dc);
//	sc1LedPointer->onPaint(dc);
}

void Elf2KScreen::onMouseRelease(wxMouseEvent&event)
{
	int x, y;
	event.GetPosition(&x, &y);

	wxClientDC dc(this);

	if (powerSwitchButton->onMouseRelease(dc, x, y))
		p_Main->stopComputer();
}

BEGIN_EVENT_TABLE(Elf2K, wxFrame)
	EVT_CLOSE (Elf2K::onClose)
//	EVT_BUTTON(3, Elf2K::onPowerButton)
//	EVT_BUTTON(4, Elf2K::onPowerButton)
	EVT_TIMER(900, Elf2K::OnRtcTimer)
END_EVENT_TABLE()

Elf2K::Elf2K(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	elfConfiguration = conf;

	elfClockSpeed_ = clock;
	data_ = 0;
	lastAddress_ = 0;

	wxString switchNumber;

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	this->SetClientSize(size);

	elf2KScreenPointer = new Elf2KScreen(this, size);
	elf2KScreenPointer->init();

	rtcTimerPointer = new wxTimer(this, 900);
	cycleValue_ = -1;
	cycleSize_ = -1;
}

Elf2K::~Elf2K()
{
	saveRam();
	saveRtc();
	rtcTimerPointer->Stop();
	delete rtcTimerPointer;
	if (elfConfiguration.usePixie)
	{
		p_Main->setPixiePos(ELF2K, pixiePointer->GetPosition());
		pixiePointer->Destroy();
	}
	if (elfConfiguration.use8275)
	{
		p_Main->set8275Pos(ELF2K, i8275Pointer->GetPosition());
		i8275Pointer->Destroy();
	}
	if (elfConfiguration.vtType != VTNONE)
	{
		p_Main->setVtPos(ELF2K, vtPointer->GetPosition());
		vtPointer->Destroy();
	}
    if (elfConfiguration.vtExternal)
        delete p_Serial;
	p_Main->setMainPos(ELF2K, GetPosition());
	if (elfConfiguration.useSwitch)
	{
		p_Main->setElf2KswitchPos(p_Elf2Kswitch->GetPosition());
		p_Elf2Kswitch->Destroy();
	}
	if (elfConfiguration.useHex)
	{
		p_Main->setKeypadPos(ELF2K, p_Elf2Khex->GetPosition());
		p_Elf2Khex->Destroy();
	}
	delete elf2KScreenPointer;
}

void Elf2K::onClose(wxCloseEvent&WXUNUSED(event) )
{
	p_Main->stopComputer();
}

bool Elf2K::keyDownPressed(int key)
{
	switch(key)
	{
		case WXK_NUMPAD0:
		case WXK_NUMPAD1:
		case WXK_NUMPAD2:
		case WXK_NUMPAD3:
		case WXK_NUMPAD4:
		case WXK_NUMPAD5:
		case WXK_NUMPAD6:
		case WXK_NUMPAD7:
		case WXK_NUMPAD8:
		case WXK_NUMPAD9:
		case WXK_NUMPAD_DECIMAL:     
		case WXK_NUMPAD_ADD:
		case WXK_NUMPAD_DIVIDE:
		case WXK_NUMPAD_MULTIPLY:
		case WXK_NUMPAD_SUBTRACT:
		case WXK_NUMPAD_ENTER:
			if (elfConfiguration.UsePS2)
			{
				charEventPs2gpio(key);
				return true;
			}
		break;
	}
	if (elfConfiguration.useHexKeyboardEf3)
		checkHexKey(key);
	return false;
}

bool Elf2K::keyUpReleased(int key)
{
	if (key == inKey1_ || key == inKey2_)
	{
		onInButtonRelease();
		return true;
	}
	if (elfConfiguration.useHexKeyboardEf3)
		onHexKeyUp(key);
	return false;
}

void Elf2K::charEvent(int keycode)
{
	if (elfConfiguration.usePs2gpio)
		charEventPs2gpio(keycode);
}

void Elf2K::onInButtonPress()
{
	if (elfConfiguration.useSwitch)
		onInButtonPress(getData());
	else
		onInButtonPress(switches_);
}

void Elf2K::checkHexKey(int keycode)
{
	for (int i=0; i<16; i++)
	{
		if (keycode == keyDefA1_[i] || keycode == keyDefA2_[i])
		{
			onHexKeyDown(i);
		}
	}
}

void Elf2K::onHexKeyDown(int hexkey)
{
#if defined (__WXMAC__)
    if (ef3State_ == 0) // This is to avoid multiple key presses on OSX
        return;
#endif
    
	ef3State_ = 0;
	switches_ = ((switches_ << 4) & 0xf0) | hexkey;
}

void Elf2K::onHexKeyUp(int WXUNUSED(keycode))
{
	ef3State_ = 1;
}

void Elf2K::onRun()
{
	if (runButtonState_ == 1)
	{
		runButtonState_ = 0;
		setClear(runButtonState_);
		resetVideo();
	}
	else
	{
		runButtonState_ = 1;
		p_Main->startTime();
		setClear(runButtonState_);
		resetComputer();
	}

	if (elfConfiguration.useSwitch)
		p_Elf2Kswitch->setRunButtonUp(runButtonState_);
	if (elfConfiguration.useHex)
		p_Elf2Khex->setRunButtonUp(runButtonState_);

	p_Main->eventUpdateTitle();
	if (cpuMode_ == RESET)
		elf2KScreenPointer->showAddress(0);
}

void Elf2K::autoBoot()
{
	runButtonState_ = 1;
	if (elfConfiguration.useSwitch)
		p_Elf2Kswitch->setRunButtonUp(runButtonState_);
	if (elfConfiguration.useHex)
		p_Elf2Khex->setRunButtonUp(runButtonState_);

	setClear(runButtonState_);
	if (cpuMode_ == RESET)
		elf2KScreenPointer->showAddress(0);
}

Byte Elf2K::inPressed()
{
	return !inPressed_;
}

void Elf2K::onElf2KInButton()
{
    p_Elf2Khex->onElf2KInButton();
}

void Elf2K::onInButtonPress(Byte value)
{
	if (inPressed_ == true)  return;
	inPressed_ = true;
	if (getCpuMode() == LOAD)
	{
		dmaIn(value);
		showData(value);
	}
	if (elfConfiguration.useSwitch)
		p_Elf2Kswitch->setInButtonUp(false);
	if (elfConfiguration.useHex)
		p_Elf2Khex->setInButtonUp(false);
}

void Elf2K::onInButtonRelease()
{
	if (inPressed_ == false)  return;
	inPressed_ = false;
	if (elfConfiguration.useSwitch)
		p_Elf2Kswitch->setInButtonUp(true);
    if (elfConfiguration.useHex)
        p_Elf2Khex->setInButtonUp(true);
}
/*
void Elf2K::onButtonRelease(wxMouseEvent& event)
{
	p_Computer->onInButtonRelease();
	event.Skip();
}

void Elf2K::onButtonPress(wxMouseEvent& event)
{
	int x, y;
	event.GetPosition(&x, &y);

	if ((x >= 45) &&(x <= 75) &&(y >= 31) &&(y <= 61))
	{
		p_Computer->onInButtonPress(p_Computer->getData());
	}
	event.Skip();
}*/

void Elf2K::onLoadButton()
{
	if (!loadButtonState_)
	{
		loadButtonState_ = 1;
		clearBootstrap();
	}
	else
		loadButtonState_ = 0;

	if (elfConfiguration.useSwitch)
		p_Elf2Kswitch->setLoadButtonUp(!loadButtonState_);
	if (elfConfiguration.useHex)
		p_Elf2Khex->setLoadButtonUp(loadButtonState_);
	
	setWait(loadButtonState_);
	resetVideo();
	if (getCpuMode() == RESET)
		elf2KScreenPointer->showAddress(0);
}

void Elf2K::onMpButton()
{
	if (mpButtonState_)
		mpButtonState_ = 0;
	else
		mpButtonState_ = 1;

	if (elfConfiguration.useSwitch)
		p_Elf2Kswitch->setMpButtonUp(mpButtonState_);
	if (elfConfiguration.useHex)
		p_Elf2Khex->setMpButtonUp(!mpButtonState_);
}

void Elf2K::dataSwitch(int number)
{
	if (dataSwitchState_[number])
		dataSwitchState_[number] = 0;
	else
		dataSwitchState_[number] = 1;

	if (elfConfiguration.useSwitch)
		p_Elf2Kswitch->setDataSwitchUp(number, dataSwitchState_[number]);
}

Byte Elf2K::getData()
{
	return(dataSwitchState_[7]?128:0) +(dataSwitchState_[6]?64:0) +(dataSwitchState_[5]?32:0) +(dataSwitchState_[4]?16:0) +(dataSwitchState_[3]?8:0) +(dataSwitchState_[2]?4:0) +(dataSwitchState_[1]?2:0) +(dataSwitchState_[0]?1:0);
}

void Elf2K::configureComputer()
{
    efType_[1] = EF1UNDEFINED;
    efType_[2] = EF2UNDEFINED;
    efType_[3] = EF3UNDEFINED;

    int dummy[5];
    
	inType_[4] = ELF2KIN;
	outType_[4] = ELF2KOUT;
	outType_[7] = ELF2KGPIO;

	setCycleType(COMPUTERCYCLE, ELF2KCYCLE);
	p_Main->message("Configuring Elf2K");
	p_Main->message("	Output 4: display output, input 4: data input");
	p_Main->message("	Output 7: Write GPIO Control Register");
	if (elfConfiguration.useHexKeyboardEf3)
	{
		efType_[3] = ELF2KEF3;
		p_Main->message("	EF 3: 0 when hex button pressed");
	}
	if (efType_[4] == 0)
	{
		efType_[4] = ELF2KEF;
		p_Main->message("	EF 4: 0 when in button pressed");
	}
	p_Main->message("");

    inKey1_ = p_Main->getDefaultInKey1("Elf2K");
    inKey2_ = p_Main->getDefaultInKey2("Elf2K");
    p_Main->getDefaultHexKeys(ELF2K, "Elf2K", "A", keyDefA1_, keyDefA2_, dummy);

	runButtonState_ = 0;
	loadButtonState_ = 1;
	mpButtonState_ = 0;
	for (int i=0; i<8; i++)  dataSwitchState_[i]=0;

	resetCpu();
}

void Elf2K::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
	for (int i=0; i<16; i++)
	{
        keyDefA1_[i] = hexKeyDefA1[i];
        keyDefA2_[i] = hexKeyDefA2[i];
	}
}

void Elf2K::initComputer()
{
	Show(p_Main->getUseElfControlWindows(ELF2K));
	lastMode_ = UNDEFINDEDMODE;

	switches_ = 0;
	ef3State_ = 1;
	inPressed_ = false;
}

Byte Elf2K::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 0;
		break;

		case PIXIEEF:
			return pixiePointer->efPixie();
		break;

		case PS2GPIOEF:
			return efPs2gpio();
		break;

		case VT100EF:
			return vtPointer->ef();
		break;

        case VTSERIALEF:
            return p_Serial->ef();
        break;
 
		case I8275EF:
			return i8275Pointer->ef8275();
		break;

		case ELF2KEF:
			return inPressed();
		break;

		case ELF2KEF3:
			return ef3State_;
		break;

        case EF1UNDEFINED:
            return elfConfiguration.elfPortConf.ef1default;
        break;
            
        case EF2UNDEFINED:
            return elfConfiguration.elfPortConf.ef2default;
        break;
            
        case EF3UNDEFINED:
            return elfConfiguration.elfPortConf.ef3default;
        break;

        default:
			return 1;
	}
}

Byte Elf2K::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

		case PIXIEIN:
			ret = pixiePointer->inPixie();
		break;

		case I8275PREGREAD:
			ret = i8275Pointer->pRegRead();
		break;

		case I8275SREGREAD:
			ret = i8275Pointer->sRegRead();
		break;

		case PS2GPIOIN:
			ret = inPs2gpio();
		break;

		case ELF2KIN:
			ret = 0;
			if (elfConfiguration.useHexKeyboardEf3)
				ret = switches_;
			else
			{
				if (elfConfiguration.useSwitch)
					ret = getData();
				if (elfConfiguration.useHex)
					ret = p_Elf2Khex->getData();
			}
		break;

		case ELF2KDISKREADREGISTER:
			ret = inDisk();
		break;

		case ELF2KDISKREADSTATUS:
			ret = readDiskStatus();
		break;

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Elf2K::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;
	bootstrap_ = 0;

//	p_Main->messageInt(port);
//	p_Main->messageInt(value);

	switch(outType_[port])
	{
		case 0:
			return;
		break;

		case PIXIEOUT:
			pixiePointer->outPixie();
		break;

		case ELF2KGPIO:
			if ((value & 0x20) == 0x20) // SPEN
			{
				setSoundFollowQ (false);
				switch (value & 0x18)
				{
					case 0:
						toneElf2KOff();
					break;

					case 0x8:
						beepOn();
					break;

					case 0x10:
						setSoundFollowQ (true);
					break;

					case 0x18:
						toneElf2KOn();
					break;
				}
			}
			if ((value & 0x4) == 0x4) // PPIEN
			{
			}
		break;

		case I8275PREGWRITE:
			i8275Pointer->pRegWrite(value);
		break;

		case I8275CREGWRITE:
			i8275Pointer->cRegWrite(value);
		break;

		case VT100OUT:
			vtPointer->out(value);
		break;

		case VTOUTSERIAL:
			p_Serial->out(value);
		break;

		case ELF2KOUT:
			showData(value);
		break;

		case ELF2KDISKSELECTREGISTER:
			selectDiskRegister(value);
		break;

		case ELF2KDISKWRITEREGISTER:
			outDisk(value);
		break;
	}
}

void Elf2K::showData(Byte val)
{
	elf2KScreenPointer->showData(val);
}

void Elf2K::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

		case PS2GPIOCYCLE:
			cyclePs2gpio();
		break;

		case PIXIECYCLE:
			pixiePointer->cyclePixie();
		break;

		case I8275CYCLE:
			i8275Pointer->cycle8275();
		break;

		case VT100CYCLE:
			vtPointer->cycleVt();
		break;

        case VTSERIALCYCLE:
            p_Serial->cycleVt();
        break;

		case ELF2KDISKCYCLE:
			cycleDisk();
		break;

		case ELF2KCYCLE:
			cycleElf2K();
		break;
	}
}

void Elf2K::cycleElf2K()
{
	if (cycleValue_ > 0)
    {
        cycleValue_ --;
        if (cycleValue_ <= 0)
        {
            cycleValue_ = cycleSize_;
            rtcRam_[0xc] |= 0x40;
        }
    }
    if (ledCycleValue_ > 0)
    {
        ledCycleValue_ --;
        if (ledCycleValue_ <= 0)
        {
            ledCycleValue_ = ledCycleSize_;
            elf2KScreenPointer->ledTimeout();
        }
    }
}

void Elf2K::switchQ(int value)
{
	elf2KScreenPointer->setQLed(value);
    
    if (elfConfiguration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (elfConfiguration.vtExternal)
        p_Serial->switchQ(value);
}

void Elf2K::startComputer()
{
	resetPressed_ = false;

	if (elfConfiguration.useSwitch)
	{
		p_Elf2Kswitch = new Elf2Kswitch("Elf 2000 - Switch Panel", p_Main->getElf2KswitchPos(), wxSize(451, 166));
		p_Elf2Kswitch->Show(elfConfiguration.useSwitch);
	}
	if (elfConfiguration.useHex)
	{
		p_Elf2Khex = new Elf2Khex("Hex Panel", p_Main->getKeypadPos(ELF2K), wxSize(186, 126));
		p_Elf2Khex->Show(true);
	}

	bootstrap_ = 0;
	defineMemoryType(0, 0x7fff, RAM);
 
    p_Main->assDefault("mycode", 0, 0xFFF);

	readProgram(p_Main->getRomDir(ELF2K, MAINROM1), p_Main->getRomFile(ELF2K, MAINROM1), ROM, 0x8000, NONAME);
	loadRam();
	loadRtc();

	configureElfExtensions();
	scratchpadRegister_[0]=0;
	if (elfConfiguration.bootRam)
		bootstrap_ = 0;
	else
		bootstrap_ = 0x8000;
	autoBoot();

	if (elfConfiguration.vtType != VTNONE)
		setEf(4,0);

	p_Main->setSwName("");
	p_Main->updateTitle();

	cpuCycles_ = 0;
	p_Main->startTime();

	if (elfConfiguration.usePs2gpio)
		startPs2gpioKeyFile();
	rtcTimerPointer->Start(1000, wxTIMER_CONTINUOUS);
    
    int ms = (int) p_Main->getLedTimeMs(ELF2K);
    elf2KScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((elfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;

    if (p_Vt100 != NULL)
        p_Vt100->splashScreen();
    
    threadPointer->Run();
}

void Elf2K::writeMemDataType(Word address, Byte type)
{
	address = address | bootstrap_;
	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;
	}
}

Byte Elf2K::readMemDataType(Word address)
{
	address = address | bootstrap_;
	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Elf2K::readMem(Word addr)
{
	addr = addr | bootstrap_;
	address_ = addr; 
	elf2KScreenPointer->showAddress(address_);

	switch (memoryType_[addr/256])
	{
		case UNDEFINED:
			return 255;
		break;

		case ROM:
		case RAM:
			return mainMemory_[addr];
		break;

		default:
			return 255;
		break;
	}
}

void Elf2K::writeMem(Word addr, Byte value, bool writeRom)
{
	addr = addr | bootstrap_;
	address_ = addr; 
	elf2KScreenPointer->showAddress(address_);

	switch (memoryType_[addr/256])
	{
		case UNDEFINED:
		case ROM:
			if (writeRom)
				mainMemory_[addr]=value;
		break;

		case RAM:
			if (mainMemory_[addr]==value)
				return;
			if (elfConfiguration.useSwitch || elfConfiguration.useHex)
			{
				if (mpButtonState_ == 0)
				{
					mainMemory_[addr]=value;
					if (addr>= (memoryStart_ | bootstrap_) && addr<((memoryStart_ | bootstrap_)+256))
						p_Main->updateDebugMemory(addr);
					p_Main->updateAssTabCheck(addr);
				}
			}
			else
			{
				mainMemory_[addr]=value;
				if (addr>= (memoryStart_ | bootstrap_) && addr<((memoryStart_ | bootstrap_)+256))
					p_Main->updateDebugMemory(addr);
				p_Main->updateAssTabCheck(addr);
			}
		break;
	}
}

void Elf2K::cpuInstruction()
{
	if (cpuMode_ != lastMode_)
	{
		elf2KScreenPointer->setRunLed(0);
		elf2KScreenPointer->setLoadLed(0);
		switch(cpuMode_)
		{
			case LOAD: elf2KScreenPointer->setLoadLed(1); break;
			case RESET: break;
			case RUN: elf2KScreenPointer->setRunLed(1); break;
			case PAUSE: break;
		}
		lastMode_ = cpuMode_;
	}
	if (cpuMode_ == RUN)
	{
		if (steps_ != 0)
		{
			cycle0_=0;
			machineCycle();
			if (cycle0_ == 0) machineCycle();
			if (cycle0_ == 0 && steps_ != 0)
			{
				cpuCycle();
				cpuCycles_ += 2;
			}
			if (debugMode_)
				p_Main->showInstructionTrace();
		}
		else
			soundCycle();
		if (resetPressed_)
		{
			resetCpu();
			resetComputer();
			autoBoot();
			resetPressed_ = false;
			p_Main->setSwName("");
            p_Main->eventUpdateTitle();
			if (elfConfiguration.usePs2gpio)
				startPs2gpioKeyFile();
		}
		if (debugMode_)
			p_Main->cycleDebug();
	}
	else
	{
		machineCycle();
		machineCycle();
		cpuCycles_ = 0;
		p_Main->startTime();
		if (cpuMode_ == LOAD)
		{
			showData(readMem(address_));
            ledCycleValue_ = 1;
			threadPointer->Sleep(1);
		}
	}
}

void Elf2K::resetComputer()
{
	ElfConfiguration currentElfConfig = p_Main->getElfConfiguration(ELF2K);
	if (currentElfConfig.bootRam)
		bootstrap_ = 0;
	else
		bootstrap_ = 0x8000;
	if (currentElfConfig.clearRam)
	{
		p_Main->eventSetCheckBox("Elf2KClearRam", false);
		for (int i=0; i<0x8000; i++)
			mainMemory_[i] = 0;
	}
	if (currentElfConfig.clearRtc)
	{
		p_Main->eventSetCheckBox("Elf2KClearRtc", false);
		for (int i = 0; i<128; i++)
			rtcRam_[i] = 0xff;
		rtcRam_[0xa] = 0;
		rtcRam_[0xb] = 0x6;
		rtcRam_[0xc] = 0;
		rtcRam_[0xd] = 0x80;
	}
	resetVideo();
	if (currentElfConfig.usePs2gpio)
		resetPs2gpio();
}

void Elf2K::resetVideo()
{
	initComputer();
	if (elfConfiguration.use8275)
		i8275Pointer->cRegWrite(0x40);
}

void Elf2K::configureElfExtensions()
{
	wxString path, fileName1, fileName2;

	if (elfConfiguration.vtType != VTNONE)
	{
		double zoom = p_Main->getZoomVt();
        if (elfConfiguration.vtType == VT52)
            vtPointer = new Vt100("Elf 2000 - VT 52", p_Main->getVtPos(ELF2K), wxSize(640*zoom, 400*zoom), zoom, ELF2K, elfClockSpeed_, elfConfiguration);
        else
            vtPointer = new Vt100("Elf 2000 - VT 100", p_Main->getVtPos(ELF2K), wxSize(640*zoom, 400*zoom), zoom, ELF2K, elfClockSpeed_, elfConfiguration);
		p_Vt100 = vtPointer;
		vtPointer->configureVt2K(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.elfPortConf);
		vtPointer->Show(true);
	}

    if (elfConfiguration.vtExternal)
    {
        p_Serial = new Serial(MEMBER, elfClockSpeed_, elfConfiguration);
        p_Serial->configureVt2K(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.elfPortConf);
    }

    if (elfConfiguration.usePixie)
	{
		double zoom = p_Main->getZoom();
		double scale = p_Main->getScale();
		pixiePointer = new Pixie( "Elf 2000 - Pixie", p_Main->getPixiePos(ELF2K), wxSize(64*zoom*scale, 128*zoom), zoom, scale, ELF2K);
		p_Video = pixiePointer;
		pixiePointer->configurePixie(elfConfiguration.elfPortConf);
		pixiePointer->initPixie();
		pixiePointer->setZoom(zoom);
		pixiePointer->Show(true);
	}

	if (elfConfiguration.use8275)
	{
		double zoom = p_Main->getZoom();
		i8275Pointer = new i8275( "Elf 2000 - Intel 8275", p_Main->get8275Pos(ELF2K), wxSize(80*8*zoom, 24*10*2*zoom), zoom, ELF2K, elfClockSpeed_);
		p_Video = i8275Pointer;
		i8275Pointer->configure8275(elfConfiguration.elfPortConf);
		i8275Pointer->init8275();
		i8275Pointer->Show(true);
	}

	if (p_Main->getIdeFile(ELF2K) != "")
	{
		configureDisk(p_Main->getIdeDir(ELF2K) + p_Main->getIdeFile(ELF2K), p_Main->getIdeDir(ELF2K) + "disk2.ide", elfConfiguration.rtc, elfConfiguration.useUart, elfConfiguration.elfPortConf, elfConfiguration.use8275);
	}

	if (elfConfiguration.usePs2gpio)
		configurePs2gpioElf2K(ELF2K);
}

void Elf2K::moveWindows()
{
	if (elfConfiguration.usePixie)
		pixiePointer->Move(p_Main->getPixiePos(ELF2K));
	if (elfConfiguration.use8275)
		i8275Pointer->Move(p_Main->get8275Pos(ELF2K));
	if (elfConfiguration.vtType != VTNONE)
		vtPointer->Move(p_Main->getVtPos(ELF2K));
	if (elfConfiguration.useSwitch)
		p_Elf2Kswitch->Move(p_Main->getElf2KswitchPos());
	if (elfConfiguration.useHex)
		p_Elf2Khex->Move(p_Main->getKeypadPos(ELF2K));
}

void Elf2K::updateTitle(wxString Title)
{
	if (elfConfiguration.usePixie)
		pixiePointer->SetTitle("Elf 2000 - Pixie"+Title);
	if (elfConfiguration.use8275)
		i8275Pointer->SetTitle("Elf 2000 - Intel 8275"+Title);
    if (elfConfiguration.vtType == VT52)
		vtPointer->SetTitle("Elf 2000 - VT 52"+Title);
    if (elfConfiguration.vtType == VT100)
        vtPointer->SetTitle("Elf 2000 - VT 100"+Title);
	if (elfConfiguration.useSwitch)
		p_Elf2Kswitch->SetTitle("Elf 2000 - Switch Panel"+Title);
	if (elfConfiguration.useHex)
		p_Elf2Khex->SetTitle("Hex Panel"+Title);
}

void Elf2K::setForceUpperCase(bool status)
{
	if (elfConfiguration.vtType != VTNONE)
		vtPointer->setForceUCVt(status);
	if (elfConfiguration.usePs2gpio)
		setForceUpperCasePs2gpio(status);
}

void Elf2K::onReset()
{
	resetPressed_ = true;
}

void Elf2K::OnRtcTimer(wxTimerEvent&WXUNUSED(event))
{
	wxDateTime now = wxDateTime::Now();
	writeRtc(0, now.GetSecond());
//	p_Main->messageInt(now.GetSecond());
	writeRtc(2, now.GetMinute());
//	p_Main->messageInt(now.GetMinute());
	writeRtc(4, now.GetHour());
//	p_Main->messageInt(now.GetHour());
	writeRtc(6, now.GetWeekDay());
//	p_Main->messageInt(now.GetWeekDay());
	writeRtc(7, now.GetDay());
//	p_Main->messageInt(now.GetDay());
	writeRtc(8, now.GetMonth()+1);
//	p_Main->messageInt(now.GetMonth());
	writeRtc(9, now.GetYear()-1972);
//	p_Main->messageInt(now.GetYear());
}

void Elf2K::saveRam()
{
	ElfConfiguration currentElfConfig = p_Main->getElfConfiguration(ELF2K);
	if (!currentElfConfig.nvr)
		return;

	Byte value;
	wxFile outputFile;
	wxString fileName;

	fileName = p_Main->getMainDir()+"ramdump.bin";

	if (wxFile::Exists(fileName))
		outputFile.Open(fileName, wxFile::write);
	else
		outputFile.Create(fileName);
	for (long address = 0; address <= 0x7fff; address++)
	{
		value = mainMemory_[address];
		outputFile.Write(&value, 1);
	}
	outputFile.Close();
}

void Elf2K::loadRam()
{
	ElfConfiguration currentElfConfig = p_Main->getElfConfiguration(ELF2K);
	if (currentElfConfig.clearRam)
	{
		p_Main->eventSetCheckBox("Elf2KClearRam", false);
		for (int i=0; i<0x8000; i++)
			mainMemory_[i] = 0;
		return;
	}
	if (!currentElfConfig.nvr)
    {
        initRam(0, 0x7fff);
        return;
    }

	wxFFile inFile;
	size_t length;
	char buffer[0x8000];

	if (wxFile::Exists(p_Main->getMainDir()+"ramdump.bin"))
	{
		if (inFile.Open(p_Main->getMainDir()+"ramdump.bin", _("rb")))
		{
			length = inFile.Read(buffer, 0x8000);
			for (size_t i=0; i<length; i++)
				mainMemory_[i] = (Byte)buffer[i];
			inFile.Close();
		}
	}
    else
        initRam(0, 0x7fff);
}

void Elf2K::saveRtc()
{
	ElfConfiguration currentElfConfig = p_Main->getElfConfiguration(ELF2K);
	if (!currentElfConfig.rtc)
		return;

	Byte value;
	wxFile outputFile;

	wxString fileName = p_Main->getMainDir()+"rtcdump.bin";

	if (wxFile::Exists(fileName))
		outputFile.Open(fileName, wxFile::write);
	else
		outputFile.Create(fileName);
	for (int address = 0; address < 128; address++)
	{
		value = rtcRam_[address];
		outputFile.Write(&value, 1);
	}
	outputFile.Close();
}

void Elf2K::loadRtc()
{
	ElfConfiguration currentElfConfig = p_Main->getElfConfiguration(ELF2K);
	if (currentElfConfig.clearRtc)
	{
		p_Main->eventSetCheckBox("Elf2KClearRtc", false);
		for (int i = 0; i<128; i++)
			rtcRam_[i] = 0xff;
		rtcRam_[0xa] = 0;
		rtcRam_[0xb] = 0x6;
		rtcRam_[0xc] = 0;
		rtcRam_[0xd] = 0x80;
		return;
	}
	if (!currentElfConfig.rtc)
		return;

	wxFFile inFile;
	size_t length;
	char buffer[128];

	if (wxFile::Exists(p_Main->getMainDir()+"rtcdump.bin"))
	{
		if (inFile.Open(p_Main->getMainDir()+"rtcdump.bin", _("rb")))
		{
			length = inFile.Read(buffer, 128);
			for (size_t i=0; i<length; i++)
				rtcRam_[i] = (Byte)buffer[i];
			inFile.Close();
		}
	}
}

void Elf2K::removeElf2KSwitch() 
{
	elfConfiguration.useSwitch = false;
}

void Elf2K::removeElfHex() 
{
	elfConfiguration.useHex = false;
}

void Elf2K::showModules(bool useSwitch, bool useHex)
{
	if (elfConfiguration.useSwitch && !useSwitch)
	{
		p_Main->setElf2KswitchPos(p_Elf2Kswitch->GetPosition());
		p_Elf2Kswitch->Destroy();
		elfConfiguration.useSwitch = false;
	}
	if (elfConfiguration.useHex && !useHex)
	{
		p_Main->setKeypadPos(ELF2K, p_Elf2Khex->GetPosition());
		p_Elf2Khex->Destroy();
		elfConfiguration.useHex = false;
	}
	if (useSwitch && !elfConfiguration.useSwitch)
	{
		p_Elf2Kswitch = new Elf2Kswitch("Elf 2000 - Switch Panel", p_Main->getElf2KswitchPos(), wxSize(451, 166));
		p_Elf2Kswitch->Show(true);
		p_Elf2Kswitch->setRunButtonUp(runButtonState_);
		p_Elf2Kswitch->setLoadButtonUp(!loadButtonState_);
		p_Elf2Kswitch->setMpButtonUp(mpButtonState_);
		for (int i=0; i<8; i++)
		{
			p_Elf2Kswitch->setDataSwitchUp(i, dataSwitchState_[i]);
		}
		elfConfiguration.useSwitch = true;
	}
	if (useHex && !elfConfiguration.useHex )
	{
		p_Elf2Khex = new Elf2Khex("Hex Panel", p_Main->getKeypadPos(ELF2K), wxSize(186, 126));
		p_Elf2Khex->Show(true);
		p_Elf2Khex->setRunButtonUp(runButtonState_);
		p_Elf2Khex->setLoadButtonUp(loadButtonState_);
		p_Elf2Khex->setMpButtonUp(!mpButtonState_);
		elfConfiguration.useHex = true;
	}
}

Byte Elf2K::read8275CharRom(Word addr)
{
	if (elfConfiguration.use8275)
		return i8275Pointer->read8275CharRom(addr);
	else
		return 0;
}

void Elf2K::write8275CharRom(Word addr, Byte value)
{
	if (elfConfiguration.use8275)
		i8275Pointer->write8275CharRom(addr, value);
}

Byte Elf2K::read8275VideoRam(Word addr)
{
    if (elfConfiguration.use8275)
        return i8275Pointer->read8275VideoRam(addr);
    else
        return 0;
}

void Elf2K::write8275VideoRam(Word addr, Byte value)
{
    if (elfConfiguration.use8275)
        i8275Pointer->write8275VideoRam(addr, value);
}

void Elf2K::setElf2KDivider(Byte value)
{
	if (value == 0)
		cycleSize_ = -1;
	else
		cycleSize_ = (int) (((elfClockSpeed_ * 1000000) / 8) / freq [value]);
	cycleValue_ = cycleSize_;
}

void Elf2K::dataAvailable(bool data)
{
	dataAvailableUart(data);
}

void Elf2K::thrStatus(bool data)
{
	thrStatusUart(data);
}

void Elf2K::sleepComputer(long ms)
{
    threadPointer->Sleep(ms);
}

void Elf2K::setLedMs(long ms)
{
    elf2KScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((elfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

Byte Elf2K::getKey(Byte vtOut)
{
	Byte tempVtOut = elf2KScreenPointer->getKey(vtOut);

	if (tempVtOut == vtOut)
	{
		if (elfConfiguration.useSwitch)
			tempVtOut = p_Elf2Kswitch->getKey(vtOut);
		if (tempVtOut == vtOut)
		{
			if (elfConfiguration.useHex)
				tempVtOut = p_Elf2Khex->getKey(vtOut);
		}
	}
	return tempVtOut;
}

void Elf2K::activateMainWindow()
{
	bool maximize = IsMaximized();
	Iconize(false);
	Raise();
	Show(true);
	Maximize(maximize);
}

void Elf2K::releaseButtonOnScreen(HexButton* buttonPointer, int WXUNUSED(buttonType))
{
	if (elfConfiguration.useHex)
		p_Elf2Khex->releaseButtonOnScreen(buttonPointer);
}

void Elf2K::onNumberKeyDown(int i)
{
    if (elfConfiguration.useHex)
        p_Elf2Khex->onNumberKeyDown(i);
}



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
#include "vip.h"

Vip::Vip(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, double clock, int tempo, ElfConfiguration conf)
:Pixie(title, pos, size, zoom, zoomfactor, computerType)
{
	vipConfiguration = conf;

	clock_ = clock;
	p_Printer = new Printer();
	p_Printer->initVip(p_Printer);

	cycleSize_ = (int) (((clock_ * 1000000) / 8) / tempo);
}

Vip::~Vip()
{
	p_Printer->closeFrames();
	delete p_Printer;
	if (vipConfiguration.vtType != VTNONE)
	{
		p_Main->setVtPos(VIP, vtPointer->GetPosition());
		vtPointer->Destroy();
	}
    if (vipConfiguration.vtExternal)
        delete p_Serial;
	p_Main->setMainPos(VIP, GetPosition());
}

void Vip::setTempo(int tempo)
{
	cycleSize_ = (int) (((clock_ * 1000000) / 8) / tempo);
}

void Vip::configureComputer()
{
	outType_[2] = VIPKEYOUT;
	outType_[4] = VIPOUT4;
	outType_[7] = VIPIIOUT7;
	efType_[2] = VIPEF2;
	efType_[3] = VIPKEYEF;
    setCycleType(COMPUTERCYCLE, LEDCYCLE);

	vipSound_ = p_Main->getSound(VIP);
	cdp1862_ = p_Main->getVipVp590();
	vp580_ = p_Main->getVipVp580();

	p_Main->message("Configuring Cosmac VIP");
	if (vipSound_ == VIP_1864)
	{
		outType_[3] = VIPOUT3;
		p_Main->message("	Output 2: hex key latch, output 3: tone latch");
		p_Main->message("	output 4: address latch, output 7: cassette on/off");
	}
	else
	{
		p_Main->message("	Output 2: hex key latch, output 4: address latch");
		p_Main->message("	output 7: cassette on/off");
	}

	if (vipSound_ == VIP_SUPER2 || vipSound_ == VIP_SUPER4)
		cycleType_[COMPUTERCYCLE] = VP550CYCLE;

	cycleType_[KEYCYCLE] = VIPIIKEYCYCLE;

	if (vp580_ || cdp1862_)
	{
		p_Main->message("	EF 2: cassette in, EF 3: hex keypad A, EF 4: hex keypad B\n");
		efType_[4] = VIPKEYEF4;
	}
	else
		p_Main->message("	EF 2: cassette in, EF 3: hex keypad\n");

	usePrinter_ = false;
	if (p_Main->getPrinterStatus(VIP))
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
            vtPointer = new Vt100("Cosmac Vip - VT 52", p_Main->getVtPos(VIP), wxSize(640*zoom, 400*zoom), zoom, VIP, clock_, vipConfiguration);
        else
            vtPointer = new Vt100("Cosmac Vip - VT 100", p_Main->getVtPos(VIP), wxSize(640*zoom, 400*zoom), zoom, VIP, clock_, vipConfiguration);
		p_Vt100 = vtPointer;
		vtPointer->configureVip(vipConfiguration.baudR, vipConfiguration.baudT);
		vtPointer->Show(true);
	}

    if (vipConfiguration.vtExternal)
    {
        p_Serial = new Serial(VIP, clock_, vipConfiguration);
        p_Serial->configureVip(vipConfiguration.baudR, vipConfiguration.baudT);
    }

	defineKeys();
	resetCpu();
}

void Vip::defineKeys()
{
	player2defined_ = false;
	p_Main->getDefaultHexKeys(VIP, "Vip", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
	p_Main->getDefaultHexKeys(VIP, "Vip", "B", keyDefB1_, keyDefB2_, keyDefGameHexB_);

	if (p_Main->getConfigBool("/Vip/GameAuto", true))
		p_Main->loadKeyDefinition(p_Main->getRamFile(VIP), p_Main->getChip8SW(VIP), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");

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
		if (keyDefB1_[i] != 0)
		{
			keyDefinition[keyDefB1_[i]].defined = true;
			keyDefinition[keyDefB1_[i]].player = 1;
			keyDefinition[keyDefB1_[i]].key = i;
		}
		if (keyDefA2_[i] != 0)
		{
			keyDefinition[keyDefA2_[i]].defined = true;
			keyDefinition[keyDefA2_[i]].player = 0;
			keyDefinition[keyDefA2_[i]].key = i;
		}
		if (keyDefB2_[i] != 0)
		{
			keyDefinition[keyDefB2_[i]].defined = true;
			keyDefinition[keyDefB2_[i]].player = 1;
			keyDefinition[keyDefB2_[i]].key = i;
		}
	}
	redefineKeys_ = false;

}

void Vip::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
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

void Vip::reDefineKeysB(int hexKeyDefB1[], int hexKeyDefB2[])
{
	for (int i=0; i<16; i++)
	{
		keyDefB1_[i] = hexKeyDefB1[i];
		if (hexKeyDefB1[i] != 0)
		{
			keyDefinition[keyDefB1_[i]].defined = true;
			keyDefinition[keyDefB1_[i]].player = 1;
			keyDefinition[keyDefB1_[i]].key = i;
		}
		keyDefB2_[i] = hexKeyDefB2[i];
		if (hexKeyDefB2[i] != 0)
		{
			keyDefinition[keyDefB2_[i]].defined = true;
			keyDefinition[keyDefB2_[i]].player = 1;
			keyDefinition[keyDefB2_[i]].key = i;
		}
	}
}

void Vip::configureKeyboard() 
{
	keyboardEf_ = 1;
	keyboardValue_ = 0;

	p_Computer->setInType(3, KEYBRDIN);
	p_Computer->setEfType(4, KEYBRDEF); 

	wxString printBuffer;
	p_Main->message("Configuring Ascii Keyboard");

	printBuffer.Printf("	Input 3: read data, EF 4: data ready flag\n");
	p_Main->message(printBuffer);
}

void Vip::initComputer()
{
	setClear(1);
	setWait(1);
	cassetteEf_ = 0;

	vipKeyPort_ = 0;
	for (int i=0; i<16; i++)
	{
		vipKeyState_[0][i] = 0;
		vipKeyState_[1][i] = 0;
	}

	romMask_ = 0xFFFF;
	if (p_Main->getLatch())
		setLatch_ = 0xF000;
	else
		setLatch_ = 0x8000;

	addressLatch_ = setLatch_;
	runPressed_ = false;
	vp550IntOn_ = false;
	cycleValue_ = cycleSize_;
	colourMask_ = 0xff;
	stateQ_ = 0;
	printLatch_ = 0;

	vipRunCommand_ = 0;
	vipRunState_ = RESETSTATE;
}

void Vip::charEvent(int keycode)
{
	if (useKeyboard_)
	{
		keyboardValue_ = keycode;
		keyboardEf_ = 0;
	}
}

void Vip::keyDown(int keycode)
{
/*	if (redefineKeys_)
	{
		defineKeys();
	}*/
	if (keyDefinition[keycode].defined)
		vipKeyState_[keyDefinition[keycode].player][keyDefinition[keycode].key] = 1;
}

void Vip::keyUp(int keycode)
{
//	if (useKeyboard_)
//		keyboardEf_ = 1;
	if (keyDefinition[keycode].defined)
		vipKeyState_[keyDefinition[keycode].player][keyDefinition[keycode].key] = 0;
}

void Vip::onRun()
{
	runPressed_ = true;
}

Byte Vip::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

		case PIXIEEF:
			return efPixie();
		break;

		case KEYBRDEF:
			return keyboardEf_;
		break;

		case VIPEF2:
			return cassetteEf_;
		break;

		case VIPKEYEF:
			return ef3();
		break;

		case VIPKEYEF4:
			return ef4();
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

Byte Vip::ef3()
{
	return (vipKeyState_[0][vipKeyPort_]) ? 0 : 1;
}

Byte Vip::ef4()
{
	return (vipKeyState_[1][vipKeyPort_]) ? 0 : 1;
}

Byte Vip::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

		case PIXIEIN:
			ret = inPixie();
		break;

		case KEYBRDIN:
			ret = keyboardValue_;
			keyboardEf_ = 1;
			if (vipRunCommand_ != 0)
			{
				//keyboardEf_ = 1;
				if (keyboardValue_ == 13)
					vipRunCommand_ = 0;
			}
		break;

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Vip::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;

	switch(outType_[port])
	{
		case 0:
			return;
		break;

		case PIXIEOUT:
			outPixie();
		break;

		case PIXIEBACKGROUND:
			outPixieBackGround();
		break;

		case VIPKEYOUT:
			outVip(value);
		break;

		case VIPOUT3:
			printLatch_ = value;
			tone1864Latch(value);
		break;

		case VIPOUT4:
			addressLatch_ = 0;
		break;

		case VIPOUT5:
			outPixieBackGround();
		break;

		case VT100OUT:
			vtPointer->out(value);
		break;

		case VIPIIOUT7:
			if (value == 1)
			{
				p_Main->startCassetteLoad();
				return;
			}
			if (value == 2)
			{
				p_Main->startCassetteSave();
				return;
			}
			p_Main->stopCassette();
		break;
	}
}

void Vip::outVip(Byte value)
{
	vipKeyPort_ = value&0xf;
}

void Vip::switchQ(int value)
{
    if (vipConfiguration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (vipConfiguration.vtExternal)
        p_Serial->switchQ(value);

    if (!usePrinter_)  return;

	if (value == 0 && stateQ_ == 1 && printLatch_ != 0)
		p_Printer->printerOut(printLatch_);
//		p_Main->eventPrintDefault(printLatch_);
	stateQ_ = value;
}

void Vip::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

		case PIXIECYCLE:
			cyclePixie();
		break;

		case VP550CYCLE:
			cycleVP550();
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
    }
}

void Vip::cycleKey()
{
	if ((vipRunCommand_ != 0) && (keyboardEf_ == 1))
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
		if (scratchpadRegister_[programCounter_] == 0x08C6)
		{
			if (vipRunCommand_ == 1)
			{
				keyboardValue_ = 'C';
				keyboardEf_ = 0;
				vipRunCommand_++;
			}
			else if (vipRunCommand_ == 2)
			{
				int saveExec = p_Main->pload();
				if (saveExec == 1)
					vipRunCommand_ = 0;
				else
				{
					if (saveExec == 0)
						commandText_ = "run";
					else
					{
						wxString buffer;
						buffer.Printf("%04x", saveExec);
						commandText_ = "call(@" + buffer + ")";
					}
					keyboardValue_ = 0;
					keyboardEf_ = 0;
					vipRunCommand_++;
				}
			}
			else
			{
				if (load_)
					vipRunCommand_ = 0;
				else
				{
					if ((vipRunCommand_-2) <= commandText_.Len())
					{
						keyboardValue_ = commandText_.GetChar(vipRunCommand_-3);
						keyboardEf_ = 0;
						vipRunCommand_++;
					}
					else
					{
						keyboardValue_ = 13;
						keyboardEf_ = 0;
					}
				}
			}
		}
	}
}

void Vip::cycleVP550()
{
	if (vp550IntOn_)
	{
		cycleValue_--;
		if (cycleValue_ <= 0)
		{
			p_Computer->interrupt();
			cycleValue_ = cycleSize_;
		}
	}
}

void Vip::startComputer()
{
	resetPressed_ = false;
	Word lastAddress;

	p_Main->setSwName("");

	ramMask_ = (p_Main->getRamType(VIP) * 0x400) - 1;

	if (wxFile::Exists(p_Main->getRomDir(VIP, MAINROM1)+"cosmac.ram"))
		readFile(p_Main->getRomDir(VIP, MAINROM1)+"cosmac.ram", RAM, ramMask_-0xff, 0x10000, NONAME);
	if (p_Main->getLatch())
		readProgram(p_Main->getRomDir(VIP, MAINROM1), p_Main->getRomFile(VIP, MAINROM1), ROM, 0xF000, &lastAddress, NONAME);
	else
		readProgram(p_Main->getRomDir(VIP, MAINROM1), p_Main->getRomFile(VIP, MAINROM1), ROM, 0x8000, &lastAddress, NONAME);
    
	romMask_ = 0x7FFF;
    lastAddress &= 0x7FFF;

	if (ramMask_ <= 0xFFF)
		p_Main->assDefault("mycode", 0, ramMask_-0x100);
	else
		p_Main->assDefault("mycode", 0, 0xFFF);

    while ((lastAddress & 0x4000) == 0)
    {
        romMask_ = romMask_ >> 1;
        lastAddress = lastAddress << 1;
    }
    romMask_ |= 0x8000;

	defineMemoryType(0x0, ramMask_, RAM);
    initRam(0x0, ramMask_);

    for (int i=0x1000; i<0x8000; i+=0x1000)
		defineMemoryType(i, i+ramMask_, MAPPEDRAM);
	for (int i=0; i<p_Main->getVipVp570(); i++)
		defineMemoryType((i+1)*0x1000, (i+1)*0x1000+0xfff, VP570RAM);
	if (cdp1862_)
		defineMemoryType(0xc000, 0xdfff, COLOURRAM);


	ramMask_ |= 0xfff;
	readProgram(p_Main->getRamDir(VIP), p_Main->getRamFile(VIP), NOCHANGE, 0, SHOWNAME);
	if (mainMemory_[0x100] ==  0 && mainMemory_[0x1b] == 0x96 && mainMemory_[0x1c] == 0xb7)
	{
		chip8type_ = CHIP8;
		readProgram(p_Main->getChip8Dir(VIP), p_Main->getChip8SW(VIP), NOCHANGE, 0x200, SHOWNAME);
	}
	else
	{
		if (mainMemory_[0x100] ==  0x33 && mainMemory_[0x1b] == 0x96 && mainMemory_[0x1c] == 0xb7)
		{
			chip8type_ = CHIP8X;
			readProgram(p_Main->getChip8Dir(VIP), p_Main->getChip8SW(VIP), NOCHANGE, 0x300, SHOWNAME);
		}
		else
			readProgram(p_Main->getChip8Dir(VIP), p_Main->getChip8SW(VIP), NOCHANGE, 0x200, SHOWNAME);
	}

	double zoom = p_Main->getZoom();

	useKeyboard_ = false;
	if (p_Main->getUseKeyboard(VIP))
	{
		configureKeyboard();
		useKeyboard_ = true;
	}
	configurePixieVip();
	initPixie();
	setZoom(zoom);
	Show(true);
	setVipSound(vipSound_);

	p_Main->updateTitle();

	cpuCycles_ = 0;
	p_Main->startTime();
	
	p_Video->splashScreen();

	threadPointer->Run();
}

void Vip::writeMemDataType(Word address, Byte type)
{
	if (address < setLatch_)
		address = (address | addressLatch_);
	else
		address = address & romMask_;

	switch (memoryType_[address/256])
	{
		case RAM:
		case MAPPEDRAM:
			if (mainMemoryDataType_[address & ramMask_] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address & ramMask_] = type;
			}
		break;

		case VP570RAM:
		case ROM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;
	}
}

Byte Vip::readMemDataType(Word address)
{
	if (address < setLatch_)
		address = (address | addressLatch_);
	else
		address = address & romMask_;

	switch (memoryType_[address/256])
	{
		case RAM:
		case MAPPEDRAM:
			return mainMemoryDataType_[address & (ramMask_ | 0x8000)];
		break;

		case VP570RAM:
		case ROM:
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Vip::readMem(Word addr)
{
/*	if (addressLatch_ == 0)
	{
		if (addr < 0x200)
			addr += 0x8200;
	}
	else
	{*/ // Patch to get Eds home made VIP to work
//	if ((addr & 0x8000) == 0x8000)
//		addressLatch_ = 0;

	if (addr < setLatch_)
		addr = (addr | addressLatch_);
	else
		addr = addr & romMask_;
//	}
	switch (memoryType_[addr/256])
	{
		case RAM:
		case MAPPEDRAM:
			return mainMemory_[addr & (ramMask_ | 0x8000)];
		break;

		case VP570RAM:
			return mainMemory_[addr];
		break;

		case UNDEFINED:
			return 255;
		break;
	}
	return mainMemory_[addr];
}

void Vip::writeMem(Word addr, Byte value, bool writeRom)
{
	if (vipSound_ == VIP_SUPER4)
	{
		switch (addr)
		{
			case 0x4001:
				frequencySuper(2, value);
			return;
			case 0x4002:
				frequencySuper(3, value);
			return;
			case 0x4003:
				if (value & 0x4)
					octaveSuper(3, value);
				else
					octaveSuper(2, value);
			return;
			case 0x4010:
				amplitudeSuper(2, value);
			return;
			case 0x4020:
				amplitudeSuper(3, value);
			return;
			case 0x4030:
				vp550IntOn_ = (value == 1);
			return;
		}
	}
	switch (memoryType_[addr/256])
	{
		case RAM:
		case MAPPEDRAM:
			if (mainMemory_[addr & ramMask_]==value)
				return;
			mainMemory_[addr & ramMask_]=value;
			if ((addr & ramMask_) >= (memoryStart_ & ramMask_) && (addr & ramMask_)<((memoryStart_ & ramMask_)+256))
				p_Main->updateDebugMemory(addr);
			p_Main->updateAssTabCheck(addr & ramMask_);
		break;

		case VP570RAM:
			mainMemory_[addr]=value;
			if (addr >= memoryStart_ && addr <(memoryStart_+256))
				p_Main->updateDebugMemory(addr);
			p_Main->updateAssTabCheck(addr);
		break;

		case COLOURRAM:
			if ((addr >= 0xc000) && (addr < 0xd000))
				colourMask_ = 0xe7;
			else
				colourMask_ = 0xff; 
			colorMemory1864_[addr&colourMask_] = value & 0xf;
			if ((addr&colourMask_) >= memoryStart_ && (addr&colourMask_) <(memoryStart_+256))
				p_Main->updateDebugMemory(addr&colourMask_);
			if (addr >= memoryStart_ && addr<(memoryStart_ + 256))
				p_Main->updateDebugMemory(addr);
			p_Main->updateAssTabCheck(addr);
			useColour(colourMask_);
		break;

		default:
			if (writeRom)
				mainMemory_[addr]=value;
			else
			{
				if (vipSound_ == VIP_SUPER2 || vipSound_ == VIP_SUPER4)
				{
					switch (addr)
					{
						case 0x8001:
							frequencySuper(0, value);
						return;
						case 0x8002:
							frequencySuper(1, value);
						return;
						case 0x8003:
							if (value & 0x4)
								octaveSuper(1, value);
							else
								octaveSuper(0, value);
						return;
						case 0x8010:
							amplitudeSuper(0, value);
						return;
						case 0x8020:
							amplitudeSuper(1, value);
						return;
						case 0x8030:
							vp550IntOn_ = (value == 1);
						return;
					}
				}
			}
		break;
	}
}

Byte Vip::read1864ColorDirect(Word addr)
{
	return colorMemory1864_[addr] & 0xf;
}

void Vip::write1864ColorDirect(Word addr, Byte value)
{
	colorMemory1864_[addr] = value & 0xf;
}

void Vip::cpuInstruction()
{
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

		playSaveLoad();
		checkVipFunction();

		if (resetPressed_)
		{
			resetCpu();
			resetPressed_ = false;
			addressLatch_ = 0x8000;
			initPixie();
			vipRunState_ = RESETSTATE;
		}
		if (runPressed_)
		{
			setClear(0);
			p_Main->eventUpdateTitle();
			runPressed_ = false;
		}
		if (debugMode_)
			p_Main->cycleDebug();
		p_Main->cycleChip8Debug();
	}
	else
	{
		if (runPressed_)
		{
			setClear(1);
			p_Main->eventUpdateTitle();
			addressLatch_ = 0x8000;
			initPixie();
			runPressed_ = false;
		}
	}
}

void Vip::moveWindows()
{
	if (vipConfiguration.vtType != VTNONE)
		vtPointer->Move(p_Main->getVtPos(VIP));
}

void Vip::updateTitle(wxString Title)
{
    if (vipConfiguration.vtType == VT52)
        vtPointer->SetTitle("Cosmac VIP - VT 52" + Title);
    if (vipConfiguration.vtType == VT100)
        vtPointer->SetTitle("Cosmac VIP - VT 100" + Title);
}

void Vip::onReset()
{
	resetPressed_ = true;
}

void Vip::checkVipFunction()
{
	switch(scratchpadRegister_[programCounter_])
	{
		case 0xcc:
			if (loadedProgram_ == FPBBOOT || loadedProgram_ == FPBBASIC)
				p_Main->stopCassette();
		break;

		case 0x80c0:
		case 0x80ed:
			p_Main->stopCassette();
		break;

		case 0x87:	// FPB bootloader SAVE
			if (loadedProgram_ == FPBBOOT || loadedProgram_ == FPBBASIC)
				p_Main->startCassetteSave();
		break;

		case 0x8091:	// SAVE
			p_Main->startCassetteSave();
		break;

		case 0x2d:	// FPB bootloader LOAD
			if (loadedProgram_ == FPBBOOT || loadedProgram_ == FPBBASIC)
				p_Main->startCassetteLoad();
		break;

		case 0x80c2:	// LOAD
			p_Main->startCassetteLoad();
		break;

		case 0x1038:	// READY
			if (vipRunState_ != BASICSTATE && loadedProgram_ == FPBBASIC)
				vipRunState_ = BASICSTATE;
		break;

		case 0x1ADE:	// RUN
		case 0x2162:	// CALL
		case 0x2165:	// USR
			if (vipRunState_ != RUNSTATE && loadedProgram_ == FPBBASIC)
				vipRunState_ = RUNSTATE;
		break;
	}
}

void Vip::startComputerRun(bool load)
{
//	p_Main->pload();
	load_ = load;
	if (vipRunState_ == RESETSTATE)
		vipRunCommand_ = 1;
	else
		vipRunCommand_ = 2;
}

void Vip::sleepComputer(long ms)
{
	threadPointer->Sleep(ms);
}

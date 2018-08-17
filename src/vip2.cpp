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
#include "vip2.h"

VipII::VipII(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, double clock, int tempo)
:Pixie(title, pos, size, zoom, zoomfactor, computerType)
{
	clock_ = clock;
	p_Printer = new Printer();
	p_Printer->initVip(p_Printer);

	cycleSize_ = (int) (((clock_ * 1000000) / 8) / tempo);
}

VipII::~VipII()
{
	p_Printer->closeFrames();
	delete p_Printer;
	p_Main->setMainPos(VIPII, GetPosition());
}

void VipII::setTempo(int tempo)
{
	cycleSize_ = (int) (((clock_ * 1000000) / 8) / tempo);
}

void VipII::configureComputer()
{
	vipMode_ = false;
	outType_[2] = VIPKEYOUT;
	outType_[4] = VIPOUT4;
	outType_[7] = VIPIIOUT7;
	efType_[2] = VIPEF2;
	efType_[3] = VIPKEYEF;
    setCycleType(COMPUTERCYCLE, LEDCYCLE);

	p_Main->message("Configuring Cosmac VIP II");

	outType_[3] = VIPOUT3;

	cycleType_[COMPUTERCYCLE] = VIPIIKEYCYCLE;

	p_Main->message("	Output 2: hex key latch, output 3: tone latch");
	p_Main->message("	output 4: address latch, output 7: cassette on/off");

	p_Main->message("	EF 2: cassette in, EF 3: hex keypad A, EF 4: hex keypad B\n");
	efType_[4] = VIPKEYEF4;

	usePrinter_ = false;
/*	if (p_Main->getPrinterStatus(VIPII))
	{
		outType_[3] = VIPOUT3;
		usePrinter_ = true;
		p_Main->message("Configuring Centronics P-1/PR-40 Printer");
		p_Main->message("	Output 3: latch, Q: strobe, EF 3: busy\n");
	}*/
	defineKeys();
	resetCpu();
}

void VipII::defineKeys()
{
	player2defined_ = false;
	p_Main->getDefaultHexKeys(VIPII, "VipII", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
	p_Main->getDefaultHexKeys(VIPII, "VipII", "B", keyDefB1_, keyDefB2_, keyDefGameHexB_);

	if (p_Main->getConfigBool("/VipII/GameAuto", true))
		p_Main->loadKeyDefinition(p_Main->getRamFile(VIPII), p_Main->getChip8SW(VIPII), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
	
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

void VipII::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
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

void VipII::reDefineKeysB(int hexKeyDefB1[], int hexKeyDefB2[])
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

void VipII::configureKeyboard() 
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

void VipII::initComputer()
{
	setClear(1);
	setWait(1);
	cassetteEf_ = 0;
	oldCassetteEf_ = 1;

	vipKeyPort_ = 0;
	for (int i=0; i<16; i++)
	{
		vipKeyState_[0][i] = 0;
		vipKeyState_[1][i] = 0;
	}

	updateQLed_ = false;
	lastMode_ = UNDEFINDEDMODE;
//	addressLatch_ = 0x8000;
	runPressed_ = false;
	colourMask_ = 0x3ff;
	stateQ_ = 0;
	printLatch_ = 0;
	cycleKeyOn_ = false;
	cycleFlashOn_ = false;
	runPressedAtStartup_ = false;

	setCycle();
	cycleValue_ = cycleSize_;
	vipRunCommand_ = 0;
	vipRunState_ = RESETSTATE;
}

void VipII::charEvent(int keycode)
{
//	if (keyboardEf_ == 0) return;

	keyboardValue_ = keycode;
	keyboardEf_ = 0;
}

void VipII::keyDown(int keycode)
{
/*	if (redefineKeys_)
	{
		defineKeys();
	}*/
	if (keyDefinition[keycode].defined)
		vipKeyState_[keyDefinition[keycode].player][keyDefinition[keycode].key] = 1;
}

void VipII::keyUp(int keycode)
{
//	keyboardEf_ = 1;
	if (keyDefinition[keycode].defined)
		vipKeyState_[keyDefinition[keycode].player][keyDefinition[keycode].key] = 0;
}

void VipII::onRun()
{
	if (runPressedAtStartup_)
	{
		runPressedAtStartup_ = false;
		cycleKeyOn_ = false;
		cycleFlashOn_ = false;
		keyTimerExpired_ = false;
		return;
	}

	lastMode_ = cpuMode_;
	if (!vipMode_ && cpuMode_ != RESET)
	{
		cycleKeyOn_ = false;
		cycleFlashOn_ = false;
		updateLedStatus(0, true);

		if (keyTimerExpired_)
			runPressed_ = true;

		keyTimerExpired_ = false;
	}
	else
	{
		runPressed_ = true;
		cycleKeyOn_ = false;
		cycleFlashOn_ = false;
	}
}

void VipII::runPressed()
{
	if (cycleKeyOn_ || cycleFlashOn_)
		return;

	cycleValue_ = cycleSize_;
	keyTimerExpired_ = false;
	if (!vipMode_ && cpuMode_ != RESET)
	{
		cycleKeyOn_ = true;
		cycleFlashOn_ = false;
	}
}

void VipII::setCycle()
{
	cycleSize_ = (int)(2000000 / ((1/clock_) * 8));
}

Byte VipII::ef(int flag)
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

		default:
			return 1;
	}
}

Byte VipII::ef3()
{
	return (vipKeyState_[0][vipKeyPort_]) ? 0 : 1;
}

Byte VipII::ef4()
{
	return (vipKeyState_[1][vipKeyPort_]) ? 0 : 1;
}

Byte VipII::in(Byte port, Word WXUNUSED(address))
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

void VipII::out(Byte port, Word WXUNUSED(address), Byte value)
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

void VipII::outVip(Byte value)
{
	vipKeyPort_ = value&0xf;
}

void VipII::switchQ(int value)
{
	if (stateQ_ != value)
	{
		updateQLed_ = true;
		if (ms_ == 0)
			updateLedStatus(1, value == 1);
	}

	if (usePrinter_)  
	{
		if (value == 0 && stateQ_ == 1 && printLatch_ != 0)
			p_Printer->printerOut(printLatch_);
//			p_Main->eventPrintDefault(printLatch_);
	}
	stateQ_ = value;
}

void VipII::setLedMs(long ms)
{
	ms_ = ms;
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((clock_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

void VipII::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

		case PIXIECYCLE:
			cyclePixie();
		break;

		case VIPIIKEYCYCLE:
			cycleKey();
		break;
	}
}

void VipII::cycleKey()
{
	if (cycleKeyOn_)
	{
		cycleValue_--;
		if (cycleValue_ <= 0)
		{
			if (keyTimerExpired_)
			{
				keyTimerExpired_ = false;
				cycleKeyOn_ = false;
				cycleFlashOn_ = true;
				flashState_ = false;
				cycleValue_ = 0;
			}
			else
			{
				keyTimerExpired_ = true;
				cycleValue_ = cycleSize_;
			}
		}
	}
	if (cycleFlashOn_)
	{
		cycleValue_--;
		if (cycleValue_ <= 0)
		{
			updateLedStatus(0, flashState_);
			flashState_ = !flashState_;
			cycleValue_ = cycleSize_/5;
		}
	}
	if ((vipRunCommand_ != 0) && (keyboardEf_ == 1))
	{
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
//						vipRunCommand_ = 0;
					}
				}
			}
		}
	}
}

void VipII::cycleLed()
{
    if (ledCycleValue_ > 0)
    {
        ledCycleValue_ --;
        if (ledCycleValue_ <= 0)
        {
            ledCycleValue_ = ledCycleSize_;
            if (updateQLed_)
                updateLedStatus(1, stateQ_ == 1);
            if (cassetteEf_ != oldCassetteEf_)
            {
                oldCassetteEf_ = cassetteEf_;
                updateLedStatus(2, cassetteEf_ != 0);
            }
        }
    }
}

void VipII::startComputer()
{
	resetPressed_ = false;

	p_Main->setSwName("");

	readProgram(p_Main->getRomDir(VIPII, MAINROM2), p_Main->getRomFile(VIPII, MAINROM2), ROM, 0x8000, NONAME);

	runPressedAtStartup_ = p_Main->runPressed();
	if (runPressedAtStartup_) 
	{
		addressLatch_ = 0x8000;
		vipMode_ = true;
		defineMemoryType(0, 0x7fff, RAM);
        initRam(0x0, 0x7fff);
		p_Main->assDefault("mycode", 0, 0xFFF);
		readProgram(p_Main->getRamDir(VIPII), p_Main->getRamFile(VIPII), NOCHANGE, 0, SHOWNAME);
	}
	else
	{
		addressLatch_ = 0;
		vipMode_ = false;
		readProgram(p_Main->getRomDir(VIPII, MAINROM1), p_Main->getRomFile(VIPII, MAINROM1), ROM, 0, NONAME);
		defineMemoryType(0x400, 0x7ff, RAM);
        initRam(0x400, 0x7ff);
		defineMemoryType(0xc00, 0xfff, RAM);
        initRam(0xc00, 0xfff);
		defineMemoryType(0x4000, 0x7fff, RAM);
        initRam(0x4000, 0x7fff);
		p_Main->assDefault("mycode", 0x4200, 0x51FF);
		readProgram(p_Main->getRamDir(VIPII), p_Main->getRamFile(VIPII), NOCHANGE, 0x4000, SHOWNAME);
	}
	
	defineMemoryType(0xc000, 0xdfff, COLOURRAM);

	if (mainMemory_[0x100] ==  0 && mainMemory_[0x1b] == 0x96 && mainMemory_[0x1c] == 0xb7)
	{
		chip8type_ = CHIP8;
		readProgram(p_Main->getChip8Dir(VIPII), p_Main->getChip8SW(VIPII), NOCHANGE, 0x200, SHOWNAME);
	}
	else
	{
		if (mainMemory_[0x100] ==  0x33 && mainMemory_[0x1b] == 0x96 && mainMemory_[0x1c] == 0xb7)
		{
			chip8type_ = CHIP8X;
			readProgram(p_Main->getChip8Dir(VIPII), p_Main->getChip8SW(VIPII), NOCHANGE, 0x300, SHOWNAME);
		}
		else
			readProgram(p_Main->getChip8Dir(VIPII), p_Main->getChip8SW(VIPII), NOCHANGE, 0x200, SHOWNAME);
	}

    if (chip8type_ != CHIP_NONE)
        p_Main->definePseudoCommands(chip8type_);

	double zoom = p_Main->getZoom();

	configureKeyboard(); 
	configurePixieVipII();
	initPixie();
	setZoom(zoom);
	Show(true);

	if (vipMode_)
		setClear(0);

	p_Main->updateTitle();

    ms_ = (int) p_Main->getLedTimeMs(VIPII);
    if (ms_ == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((clock_ * 1000000) / 8) / 1000) * ms_;
    ledCycleValue_ = ledCycleSize_;
    
	cpuCycles_ = 0;
	p_Main->startTime();
	reDrawBar();
	threadPointer->Run();
}

void VipII::writeMemDataType(Word address, Byte type)
{
	if (address < 0x8000)
		address = (address | addressLatch_);
	else
		address = address & 0x81ff;

	if (address == 0)
		address = 0;
	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address]=type;
			}
		break;
	}
}

Byte VipII::readMemDataType(Word address)
{
	if (address < 0x8000)
		address = (address | addressLatch_);
	else
		address = address & 0x81ff;

	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte VipII::readMem(Word addr)
{
	if (addr < 0x8000)
		addr = (addr | addressLatch_);
	else
		addr = addr & 0x81ff;

	switch (memoryType_[addr/256])
	{
		case RAM:
			return mainMemory_[addr];
		break;

		case UNDEFINED:
			return 255;
		break;
	}
	return mainMemory_[addr];
}

void VipII::writeMem(Word addr, Byte value, bool writeRom)
{
	switch (memoryType_[addr/256])
	{
		case RAM:
			if (mainMemory_[addr]==value)
				return;
			mainMemory_[addr]=value;
			if (addr >= memoryStart_ && addr <(memoryStart_+256))
				p_Main->updateDebugMemory(addr);
			p_Main->updateAssTabCheck(addr);
		break;

		case COLOURRAM:
			if ((addr >= 0xc000) && (addr < 0xd000))
				colourMask_ = 0xe7;
			else
				colourMask_ = 0x3ff;  
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
		break;
	}
}

Byte VipII::read1864ColorDirect(Word addr)
{
	return colorMemory1864_[addr] & 0xf;
}

void VipII::write1864ColorDirect(Word addr, Byte value)
{
	colorMemory1864_[addr] = value & 0xf;
}

void VipII::cpuInstruction()
{
	if (cpuMode_ != lastMode_)
	{
		if (cpuMode_ == RUN)
			updateLedStatus(0, true);
		else
			updateLedStatus(0, false);
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

		playSaveLoad();
		checkVipFunction();

		if (resetPressed_)
		{
			runPressedAtStartup_ = p_Main->runPressed();
			if (runPressedAtStartup_) 
			{
				addressLatch_ = 0x8000;
				vipMode_ = true;
				defineMemoryType(0, 0x7fff, RAM);
				setClear(0);
				p_Main->eventUpdateTitle();
			}
			else 
			{
				addressLatch_ = 0;
				vipMode_ = false;
				readProgram(p_Main->getRomDir(VIPII, MAINROM1), p_Main->getRomFile(VIPII, MAINROM1), ROM, 0, NONAME);
				defineMemoryType(0x400, 0x7ff, RAM);
				defineMemoryType(0xc00, 0xfff, RAM);
				defineMemoryType(0x4000, 0x7fff, RAM);
			}
			resetCpu();
			resetPressed_ = false;
//			addressLatch_ = 0x8000;
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
		if (vipMode_) 
			p_Main->cyclePseudoDebug();
	}
	else
	{
		if (runPressed_)
		{
			setClear(1);
			p_Main->eventUpdateTitle();
			if (vipMode_) 
				addressLatch_ = 0x8000;
			initPixie();
			runPressed_ = false;
		}
	}
}

void VipII::onReset()
{
	resetPressed_ = true;
}

void VipII::checkVipFunction()
{
	switch(scratchpadRegister_[programCounter_])
	{
		case 0x80c0:
			p_Main->stopCassette();
		break;

		case 0x80ed:
			p_Main->stopCassette();
		break;

		case 0x1038:	// READY
			if (vipRunState_ != BASICSTATE)
				vipRunState_ = BASICSTATE;
		break;

		case 0x1ADE:	// RUN
		case 0x2162:	// CALL
		case 0x2165:	// USR
			if (vipRunState_ != RUNSTATE)
				vipRunState_ = RUNSTATE;
		break;

		case 0x8091:	// SAVE
			p_Main->startCassetteSave();
		break;

		case 0x80c2:	// LOAD
			p_Main->startCassetteLoad();
		break;

		case 0x409f:
		case 0x40a3: 
                p_Main->eventShowMessage(mainMemory_[scratchpadRegister_[programCounter_]+1]*256+mainMemory_[scratchpadRegister_[programCounter_]+2]);
		break; 
	}
}

void VipII::startComputerRun(bool load)
{
//	p_Main->pload();
	load_ = load;
	if (vipRunState_ == RESETSTATE)
		vipRunCommand_ = 1;
	else
		vipRunCommand_ = 2;
}

void VipII::sleepComputer(long ms)
{
	threadPointer->Sleep(ms);
}

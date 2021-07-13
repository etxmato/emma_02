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
#include "tmc1800.h"

Tmc1800::Tmc1800(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType)
:Pixie(title, pos, size, zoom, zoomfactor, computerType)
{
}

Tmc1800::~Tmc1800()
{
	p_Main->setMainPos(TMC1800, GetPosition());
}

void Tmc1800::configureComputer()
{
	outType_[2] = VIPKEYOUT;
	outType_[4] = VIPOUT4;
	efType_[2] = VIPEF2;
	efType_[3] = VIPKEYEF;

	p_Main->message("Configuring Telmac TMC-1800");
	p_Main->message("	Output 2: key latch, output 4: address latch");
	p_Main->message("	EF 2: cassette in, EF 3: keyboard\n");

	for (int i=0; i<64; i++)
        hexKeyDefA1_[i] = 0;
    for (int i=0; i<16; i++)
        hexKeyDefA2_[i] = 0;

    if (!p_Main->getUseKeyboard(TMC1800))
	{
        simDefA2_ = false;
        simDefB2_ = false;

        p_Main->getDefaultHexKeys(TMC1800, "TMC1800", "A", hexKeyDefA1_, hexKeyDefA2_, keyDefGameHexA_);

		if (p_Main->getConfigBool("/TMC1800/GameAuto", true))
			p_Main->loadKeyDefinition(p_Main->getRamFile(TMC1800), p_Main->getChip8SW(TMC1800), hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
	}
	else
	{
		for (int i=48; i<58; i++)
			hexKeyDefA1_[i-48] = i;
		for (int i=65; i<91; i++)
			hexKeyDefA1_[i-55] = i;
	}

	resetCpu();
}

void Tmc1800::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
	for (int i=0; i<16; i++)
	{
        hexKeyDefA1_[i] = hexKeyDefA1[i];
        hexKeyDefA2_[i] = hexKeyDefA2[i];
	}
}

void Tmc1800::initComputer()
{
	setClear(1);
	setWait(1);
	cassetteEf_ = 0;

	telmac1800KeyPort_ = 0;
	for (int i=0; i<64; i++)
		telmac1800KeyState_[i] = 0;

	addressLatch_ = 0x8000;
	runPressed_ = false;
}

void Tmc1800::keyDown(int keycode)
{
	for (int i = 0; i < 36; i++)
	{
		if (keycode == hexKeyDefA1_[i])
			telmac1800KeyState_[i] = 1;
	}
	for (int i = 0; i<16; i++)
	{
		if (keycode == hexKeyDefA2_[i])
            telmac1800KeyState_[i] = 1;
	}
}

void Tmc1800::keyUp(int keycode)
{
	for (int i=0; i<36; i++)
	{
		if (keycode == hexKeyDefA1_[i])
			telmac1800KeyState_[i] = 0;
	}
	for (int i = 0; i<16; i++)
	{
		if (keycode == hexKeyDefA2_[i])
            telmac1800KeyState_[i] = 0;
	}
}

void Tmc1800::onRun()
{
	runPressed_ = true;
}

Byte Tmc1800::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

		case PIXIEEF:
			return efPixie();
		break;

		case VIPEF2:
			return cassetteEf_;
		break;

		case VIPKEYEF:
			return ef3();
		break;

		default:
			return 1;
	}
}

Byte Tmc1800::ef3()
{
	return (telmac1800KeyState_[telmac1800KeyPort_]) ? 0 : 1;
}

Byte Tmc1800::in(Byte port, Word WXUNUSED(address))
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

		case PIXIEOUT:
			outPixie();
			return 0;
		break;

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Tmc1800::out(Byte port, Word WXUNUSED(address), Byte value)
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

		case VIPKEYOUT:
			outTMC1800(value);
		break;

		case VIPOUT4:
			addressLatch_ = 0;
		break;
	}
}

void Tmc1800::outTMC1800(Byte value)
{
	telmac1800KeyPort_ = value&0x3f;
}

void Tmc1800::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

		case PIXIECYCLE:
			cyclePixie();
		break;
	}
}

void Tmc1800::startComputer()
{
	resetPressed_ = false;

	p_Main->setSwName("");

    p_Main->checkAndReInstallMainRom(TMC1800);
	readProgram(p_Main->getRomDir(TMC1800, MAINROM1), p_Main->getRomFile(TMC1800, MAINROM1), ROM, 0x8000, NONAME);

	switch(p_Main->getRamType(TMC1800))
	{
		case 0:
			defineMemoryType(0x0, 0xfff, RAM);
            initRam(0x0, 0xfff);
			ramMask_ = 0xfff;
            p_Main->assDefault("mycode", 0, 0xEFF);
        break;

		case 1:
			defineMemoryType(0x0, 0x3fff, RAM);
            initRam(0x0, 0x3fff);
			ramMask_ = 0x3fff;
            p_Main->assDefault("mycode", 0, 0xFFF);
		break;

		case 2:
			defineMemoryType(0x0, 0x7fff, RAM);
            initRam(0x0, 0x7fff);
			ramMask_ = 0x7fff;
            p_Main->assDefault("mycode", 0, 0xFFF);
		break;
	}

	readProgram(p_Main->getRamDir(TMC1800), p_Main->getRamFile(TMC1800), NOCHANGE, 0, SHOWNAME);
	readProgram(p_Main->getChip8Dir(TMC1800), p_Main->getChip8SW(TMC1800), NOCHANGE, 0x200, SHOWNAME);
    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);

	double zoom = p_Main->getZoom();

	configurePixieTmc1800();
	initPixie();
	setZoom(zoom);
	Show(true);

	p_Main->updateTitle();


	cpuCycles_ = 0;
	instructionCounter_= 0;
	p_Main->startTime();

	threadPointer->Run();
}

void Tmc1800::writeMemDataType(Word address, Byte type)
{
	if (address < 0x8000)
		address = (address | addressLatch_) & (ramMask_ | 0x8000);

	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
                increaseExecutedMainMemory(address, type);
			}
		break;
	}
}

Byte Tmc1800::readMemDataType(Word address, uint64_t* executed)
{
	if (address < 0x8000)
		address = (address | addressLatch_) & (ramMask_ | 0x8000);
	else
		address = address & 0x81ff;

	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Tmc1800::readMem(Word address)
{
	if (address < 0x8000)
		address = (address | addressLatch_) & (ramMask_ | 0x8000);
	else
		address = address & 0x81ff;

	if (memoryType_[address/256] == UNDEFINED) return 255;
	return mainMemory_[address| addressLatch_];
}

Byte Tmc1800::readMemDebug(Word address)
{
    return readMem(address);
}

void Tmc1800::writeMem(Word address, Byte value, bool writeRom)
{
	if (!writeRom)
		address = address & ramMask_;

	if (mainMemory_[address]==value)
		return;
    
	if (!writeRom)
		if (memoryType_[address/256] != RAM)  return;

	mainMemory_[address]=value;
	if (writeRom)
		return;
	if (address >= (memoryStart_& ramMask_) && address<((memoryStart_& ramMask_) + 256))
		p_Main->updateDebugMemory(address);
	p_Main->updateAssTabCheck(address);
}

void Tmc1800::writeMemDebug(Word address, Byte value, bool writeRom)
{
    writeMem(address, value, writeRom);
}

void Tmc1800::cpuInstruction()
{
	if (cpuMode_ == RUN)
	{
        cpuCycleStep();
		if (runPressed_)
		{
			setClear(0);
			p_Main->eventUpdateTitle();
			runPressed_ = false;
		}
	}
	else
	{
		if (runPressed_)
		{
			setClear(1);
			addressLatch_ = 0x8000;
			initPixie();
			p_Main->eventUpdateTitle();
            resetEffectiveClock();
			runPressed_ = false;
		}
	}
}

void Tmc1800::resetPressed()
{
    resetCpu();
    resetPressed_ = false;
    addressLatch_ = 0x8000;
    initPixie();
}

void Tmc1800::onReset()
{
	resetPressed_ = true;
}

void Tmc1800::checkComputerFunction()
{
	switch(scratchpadRegister_[programCounter_])
	{
		case 0x80c0:
			p_Main->stopCassette();
		break;

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

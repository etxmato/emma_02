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
#include "nano.h"

Nano::Nano(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, Conf computerConf)
:Pixie(title, pos, size, zoom, zoomfactor, computerType)
{
    computerConfiguration = computerConf;
}

Nano::~Nano()
{
	p_Main->setMainPos(NANO, GetPosition());
}

void Nano::configureComputer()
{
	outType_[2] = VIPKEYOUT;
	outType_[4] = VIPOUT4;
	efType_[2] = VIPEF2;
	efType_[3] = VIPKEYEF;
	efType_[4] = NANOEF4;

	p_Main->message("Configuring Telmac Nano");
	if (p_Main->getSound(NANO) == 0)
		p_Main->message("	Output 2: key latch, output 4: address and CDP 1864 tone latch");
	else
		p_Main->message("	Output 2: key latch, output 4: address latch");

	p_Main->message("	EF 2: cassette in, EF 3: keyboard, EF 4: monitor enable\n");

	simDefA2_ = false;
	simDefB2_ = false;

	p_Main->getDefaultHexKeys(NANO, "Nano", "A", hexKeyDefA1_, hexKeyDefA2_, keyDefGameHexA_);

	if (p_Main->getConfigBool("/Nano/GameAuto", true))
		p_Main->loadKeyDefinition(p_Main->getRamFile(NANO), p_Main->getChip8SW(NANO), hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");

	resetCpu();
}

void Nano::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
	for (int i=0; i<16; i++)
	{
		hexKeyDefA1_[i] = hexKeyDefA1[i];
		hexKeyDefA2_[i] = hexKeyDefA2[i];
	}
}

void Nano::initComputer()
{
	setClear(1);
	setWait(1);
	cassetteEf_ = 0;

	NanoKeyPort_ = 0;
	for (int i=0; i<64; i++)
		NanoKeyState_[i] = 0;

	addressLatch_ = 0x8000;
	tabPressed_ = 0;
}

void Nano::keyDown(int keycode)
{
	for (int i=0; i<16; i++)
	{
		if (keycode == hexKeyDefA1_[i])
			NanoKeyState_[i] = 1;
		if (keycode == hexKeyDefA2_[i])
			NanoKeyState_[i] = 1;
	}
	if (keycode == 350)
	{
		resetPressed_ = true;
		tabPressed_ = 40;
	}
}

void Nano::keyUp(int keycode)
{
	for (int i=0; i<16; i++)
	{
		if (keycode == hexKeyDefA1_[i])
			NanoKeyState_[i] = 0;
		if (keycode == hexKeyDefA2_[i])
			NanoKeyState_[i] = 0;
	}
}

Byte Nano::ef(int flag)
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

		case NANOEF4:
			if (tabPressed_ > 0)
			{
				tabPressed_--;
				return 1;
			}
			else
				return 0;
		break;

		default:
			return 1;
	}
}

Byte Nano::ef3()
{
	return (NanoKeyState_[NanoKeyPort_]) ? 0 : 1;
}

Byte Nano::in(Byte port, Word WXUNUSED(address))
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

void Nano::out(Byte port, Word WXUNUSED(address), Byte value)
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
			outNano(value);
		break;

		case VIPOUT4:
			addressLatch_ = 0;
			tone1864Latch(value);
		break;
	}
}

void Nano::outNano(Byte value)
{
	NanoKeyPort_ = value&0xf;
}

void Nano::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

		case PIXIECYCLE:
			cyclePixieTelmac();
		break;
	}
}

void Nano::startComputer()
{
	resetPressed_ = false;
    p_Main->checkAndReInstallMainRom(NANO);
	readProgram(p_Main->getRomDir(NANO, MAINROM1), p_Main->getRomFile(NANO, MAINROM1), ROM, 0x8000, NONAME);

	defineMemoryType(0x0, 0xfff, RAM);
    initRam(0x0, 0xfff);
    p_Main->assDefault("mycode", 0, 0xEFF);

	readProgram(p_Main->getRamDir(NANO), p_Main->getRamFile(NANO), NOCHANGE, 0, SHOWNAME);
	readProgram(p_Main->getChip8Dir(NANO), p_Main->getChip8SW(NANO), NOCHANGE, 0x200, SHOWNAME);

    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);

	double zoom = p_Main->getZoom();

	configurePixieNano();
	initPixie();
	setZoom(zoom);
	Show(true);

	p_Main->updateTitle();

	cpuCycles_ = 0;
	instructionCounter_= 0;
	p_Main->startTime();

	threadPointer->Run();
}

void Nano::writeMemDataType(Word address, Byte type)
{
	if (address < 0x8000)
		address = (address | addressLatch_) & 0xfff;

	switch (memoryType_[address/256]&0xff)
	{
		case RAM:
		case ROM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
            increaseExecutedMainMemory(address, type);
		break;
	}
}

Byte Nano::readMemDataType(Word address, uint64_t* executed)
{
	if (address < 0x8000)
		address = (address | addressLatch_) & 0xfff;
	else
		address = address & 0x81ff;

	switch (memoryType_[address/256]&0xff)
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

Byte Nano::readMem(Word address)
{
	if (address < 0x8000)
		address = (address | addressLatch_) & 0x8fff;
	else
		address = address & 0x81ff;

	if ((memoryType_[address/256]&0xff) == UNDEFINED) return 255;
	return mainMemory_[address];
}

Byte Nano::readMemDebug(Word address)
{
    return readMem(address);
}

void Nano::writeMem(Word address, Byte value, bool writeRom)
{
	Word memStart;
	if (address < 0x8000)
		address = address & 0xfff;
	else
		address = address;

	if (memoryStart_ < 0x8000)
		memStart = memoryStart_ & 0xfff;
	else
		memStart = memoryStart_;

	if (mainMemory_[address]==value)
		return;
	if (!writeRom)
		if ((memoryType_[address/256]&0xff) != RAM)  return;

	mainMemory_[address]=value;
	if (writeRom)
		return;
	if (address>= memStart && address<(memStart+256))
		p_Main->updateDebugMemory(address);
	p_Main->updateAssTabCheck(address);
}

void Nano::writeMemDebug(Word address, Byte value, bool writeRom)
{
    writeMem(address, value, writeRom);
}

void Nano::cpuInstruction()
{
	if (cpuMode_ == RUN)
	{
        cpuCycleStep();
	}
}

void Nano::resetPressed()
{
    resetCpu();
    resetPressed_ = false;
    addressLatch_ = 0x8000;
    initPixie();
}

void Nano::onReset()
{
	resetPressed_ = true;
}

void Nano::checkComputerFunction()
{
	switch(scratchpadRegister_[programCounter_])
	{
		case 0x80c2:
			p_Main->stopCassette();
		break;

		case 0x80ef:
			p_Main->stopCassette();
		break;

		case 0x8091:	// SAVE
			p_Main->startCassetteSave(0);
		break;

		case 0x80c4:	// LOAD
			p_Main->startCassetteLoad(0);
		break;
	}
}

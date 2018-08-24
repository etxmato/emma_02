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
#include "tmc2000.h"

Tmc2000::Tmc2000(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType)
:Pixie(title, pos, size, zoom, zoomfactor, computerType)
{
}

Tmc2000::~Tmc2000()
{
	p_Main->setMainPos(TMC2000, GetPosition());
}

void Tmc2000::configureComputer()
{
	outType_[2] = VIPKEYOUT;
	outType_[4] = VIPOUT4;
	efType_[2] = VIPEF2;
	efType_[3] = VIPKEYEF;

	p_Main->message("Configuring Telmac TMC-2000");
	p_Main->message("	Output 2: key latch, output 4: address, colour and CDP 1864 tone latch");
	p_Main->message("	EF 2: cassette in, EF 3: keyboard\n");

	for (int i=0; i<64; i++)
		hexKeyDefA1_[i] = 0;
	for (int i = 0; i<16; i++)
		hexKeyDefA2_[i] = 0;

	if (!p_Main->getUseKeyboard(TMC2000))
	{
		simDefA2_ = false;
		simDefB2_ = false;

		p_Main->getDefaultHexKeys(TMC2000, "TMC2000", "A", hexKeyDefA1_, hexKeyDefA2_, keyDefGameHexA_);

		if (p_Main->getConfigBool("/TMC2000/GameAuto", true))
			p_Main->loadKeyDefinition(p_Main->getRamFile(TMC2000), p_Main->getChip8SW(TMC2000), hexKeyDefA1_, hexKeyDefB1_, hexKeyDefA2_, &simDefA2_, hexKeyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
	}
	else
	{
		for (int i=48; i<58; i++)
			hexKeyDefA1_[i - 48] = i;
		for (int i=65; i<91; i++)
			hexKeyDefA1_[i - 55] = i;
	}
	
	resetCpu();
}

void Tmc2000::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
	for (int i = 0; i < 16; i++)
	{
		hexKeyDefA1_[i] = hexKeyDefA1[i];
		hexKeyDefA2_[i] = hexKeyDefA2[i];
	}
}

void Tmc2000::initComputer()
{
	setClear(1);
	setWait(1);
	cassetteEf_ = 0;

	telmac2000KeyPort_ = 0;
	for (int i=0; i<64; i++)
		telmac2000KeyState_[i] = 0;

	addressLatch_ = 0x8000;
	colorLatch_ = false;
	runPressed_ = false;
}

void Tmc2000::keyDown(int keycode)
{
	for (int i=0; i<36; i++)
	{
		if (keycode == hexKeyDefA1_[i])
			telmac2000KeyState_[i] = 1;
	}
	for (int i = 0; i<16; i++)
	{
		if (keycode == hexKeyDefA2_[i])
			telmac2000KeyState_[i] = 1;
	}
}

void Tmc2000::keyUp(int keycode)
{
	for (int i=0; i<36; i++)
	{
		if (keycode == hexKeyDefA1_[i])
			telmac2000KeyState_[i] = 0;
	}
	for (int i = 0; i<16; i++)
	{
		if (keycode == hexKeyDefA2_[i])
			telmac2000KeyState_[i] = 0;
	}
}

void Tmc2000::onRun()
{
	runPressed_ = true;
}

Byte Tmc2000::ef(int flag)
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

Byte Tmc2000::ef3()
{
	return (telmac2000KeyState_[telmac2000KeyPort_]) ? 0 : 1;
}

Byte Tmc2000::in(Byte port, Word WXUNUSED(address))
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

void Tmc2000::out(Byte port, Word WXUNUSED(address), Byte value)
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
			outTMC2000(value);
		break;

		case VIPOUT4:
			if (value & 1)
				colorLatch_ = true;
			else
			{
				colorLatch_ = false;
				addressLatch_ = 0;
			}
			tone1864Latch(value);
		break;
	}
}

void Tmc2000::outTMC2000(Byte value)
{
	telmac2000KeyPort_ = value&0x3f;
}

void Tmc2000::cycle(int type)
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

void Tmc2000::startComputer()
{
	resetPressed_ = false;

	p_Main->setSwName("");

	readProgram(p_Main->getRomDir(TMC2000, MAINROM1), p_Main->getRomFile(TMC2000, MAINROM1), ROM, 0x8000, NONAME);

	switch(p_Main->getRamType(TMC2000))
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

	readProgram(p_Main->getRamDir(TMC2000), p_Main->getRamFile(TMC2000), NOCHANGE, 0, SHOWNAME);
	readProgram(p_Main->getChip8Dir(TMC2000), p_Main->getChip8SW(TMC2000), NOCHANGE, 0x200, SHOWNAME);
    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &pseudoLoaded_);

	double zoom = p_Main->getZoom();

	configurePixieTelmac();
	initPixie();
	setZoom(zoom);
	Show(true);

	p_Main->updateTitle();


	cpuCycles_ = 0;
	p_Main->startTime();

	threadPointer->Run();
}

void Tmc2000::writeMemDataType(Word address, Byte type)
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
			}
		break;
	}
}

Byte Tmc2000::readMemDataType(Word address)
{
	if (address < 0x8000)
		address = (address | addressLatch_) & (ramMask_ | 0x8000);
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

Byte Tmc2000::readMem(Word addr)
{
	if (addr < 0x8000)
		address_ = (addr | addressLatch_) & (ramMask_ | 0x8000);
	else
		address_ = addr & 0x81ff;

	if (memoryType_[address_/256] == UNDEFINED) return 255;
	return mainMemory_[address_| addressLatch_];
}

void Tmc2000::writeMem(Word addr, Byte value, bool writeRom)
{
	address_ = addr;

	if (!writeRom)
	{
		if (colorLatch_ && (addr >= 0x8000) && (addr < 0x8400))
		{
			colorMemory1864_[addr&0x3ff] = value &0xf;
			if ((addr&0x3ff) >= memoryStart_ && (addr&0x3ff) <(memoryStart_+256))
				p_Main->updateDebugMemory(addr&0x3ff);
			if (addr >= memoryStart_ && addr<(memoryStart_ + 256))
				p_Main->updateDebugMemory(addr);
			p_Main->updateAssTabCheck(addr);
			return;
		}
		address_ = addr & ramMask_;
	}
	else
		address_ = addr;

	if (mainMemory_[address_]==value)
		return;
	if (!writeRom)
		if (memoryType_[address_/256] != RAM)  return;

	mainMemory_[address_]=value;
	if (writeRom)
		return;
	if (address_ >= (memoryStart_& ramMask_) && address_<((memoryStart_& ramMask_) + 256))
		p_Main->updateDebugMemory(address_);
	p_Main->updateAssTabCheck(address_);
}

Byte Tmc2000::read1864ColorDirect(Word addr)
{
	return colorMemory1864_[addr] & 0xf;
}

void Tmc2000::write1864ColorDirect(Word addr, Byte value)
{
	colorMemory1864_[addr] = value & 0xf;
}

void Tmc2000::cpuInstruction()
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
		checkTMC2000Function();

		if (resetPressed_)
		{
			resetCpu();
			resetPressed_ = false;
			addressLatch_ = 0x8000;
			initPixie();
		}
		if (runPressed_)
		{
			setClear(0);
			p_Main->eventUpdateTitle();
			runPressed_ = false;
		}
		if (debugMode_)
			p_Main->cycleDebug();
		if (pseudoLoaded_ && cycle0_ == 0)
			p_Main->cyclePseudoDebug();
	}
	else
	{
		if (runPressed_)
		{
			setClear(1);
			addressLatch_ = 0x8000;
			initPixie();
			p_Main->eventUpdateTitle();
			runPressed_ = false;
		}
	}
}

void Tmc2000::onReset()
{
	resetPressed_ = true;
}

void Tmc2000::checkTMC2000Function()
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
			p_Main->startCassetteSave();
		break;

		case 0x80c2:	// LOAD
			p_Main->startCassetteLoad();
		break;
	}
}

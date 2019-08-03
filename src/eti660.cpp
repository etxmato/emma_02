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
#include "eti660.h"

Eti::Eti(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType)
:Pixie(title, pos, size, zoom, zoomfactor, computerType)
{
}

Eti::~Eti()
{
	p_Main->setMainPos(ETI, GetPosition());
}

void Eti::configureComputer()
{
	outType_[2] = ETIPIAOUT;
	inType_[2] = ETIPIAIN;
	outType_[3] = ETICOLOURRAM;
	outType_[4] = VIPOUT4;
	efType_[2] = VIPEF2;
	efType_[4] = VIPKEYEF;

	p_Main->message("Configuring ETI 660");
	p_Main->message("	Output/input 2: PIA, output 3: colour RAM");
	p_Main->message("	EF 2: cassette in, EF 4: step key\n");

    inKey1_ = p_Main->getDefaultInKey1("Eti");
    inKey2_ = p_Main->getDefaultInKey2("Eti");
	p_Main->getDefaultHexKeys(ETI, "Eti", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);

	if (p_Main->getConfigBool("/Eti/GameAuto", true))
		p_Main->loadKeyDefinition("", p_Main->getChip8SW(ETI), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");

	resetCpu();
	DataDirection_ = true;
}

void Eti::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
	for (int i=0; i<16; i++)
	{
		keyDefA1_[i] = hexKeyDefA1[i];
		keyDefA2_[i] = hexKeyDefA2[i];
	}
}

void Eti::initComputer()
{
	setClear(1);
	setWait(1);
	cassetteEf_ = 0;

	for (int i=0; i<16; i++)
		eti660KeyState_[i] = false;

	colorLatch_ = false;
	step_ = false;
	endSave_ = 0x600;
}

void Eti::keyDown(int keycode)
{
	for (int i=0; i<16; i++)
	{
		if (keycode == keyDefA1_[i])
			eti660KeyState_[i] = true;
		if (keycode == keyDefA2_[i])
			eti660KeyState_[i] = true;
	}

	if (keycode == 43)
		step_ = true;

	if (keycode == WXK_NUMPAD_ADD)
		step_ = true;

	if (keycode == inKey1_ || keycode == inKey2_)
		step_ = true;
}

void Eti::keyUp(int keycode)
{
	for (int i=0; i<16; i++)
	{
		if (keycode == keyDefA1_[i])
			eti660KeyState_[i] = false;
		if (keycode == keyDefA2_[i])
			eti660KeyState_[i] = false;
	}

	if (keycode == 43)
		step_ = false;

	if (keycode == WXK_NUMPAD_ADD)
		step_ = false;

	if (keycode == inKey1_ || keycode == inKey2_)
		step_ = false;
}

Byte Eti::ef(int flag)
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

Byte Eti::ef3()
{
	return step_;
}

Byte Eti::in(Byte port, Word address)
{
	Byte ret;
//	p_Main->messageInt(port);
//	p_Main->messageInt(address);

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

		case ETIPIAIN:
			ret = inEti(address);
		break;

		case PIXIEIN:
			p_Main->stopCassette();
			ret = inPixie();
		break;

		case PIXIEOUT:
			outPixie();
			return 0;
		break;

		default:
//			p_Main->messageInt(port);
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Eti::out(Byte port, Word address, Byte value)
{
	outValues_[port] = value;
	Word addr;
//	p_Main->messageInt(port);
//	p_Main->messageInt(value);

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

		case ETIPIAOUT:
			outEti(address, value);
		break;

		case ETICOLOURRAM:
//			p_Main->messageHex(address);
//			p_Main->messageHex(value);
//			colorMemory1864_[address&0xff] = value &0xf;
			addr = ((address >> 1) & 0xf8) + (address & 0x7);
			colorMemory1864_[addr] = value &0xf;
			if (addr >= memoryStart_ && addr <(memoryStart_+256))
				p_Main->updateDebugMemory(addr);
			return;
		break;

		case VIPOUT4:
			tone1864Latch(value);
		break;

//		default:
//			p_Main->messageInt(port);
	}
}

Byte Eti::inEti(Word address)
{
	Byte ret = 0xff;

	if ((address&0x3) == 0)
	{
		if (DataDirection_)	// Data Direction A
		{
//			p_Main->message("Data Direction Register: Input");
		}
		else				// Output Register
		{
			if ((outputKeyValue_&0x1) == 0)
			{
				if (eti660KeyState_[3])
					ret &= 0x7f;
				if (eti660KeyState_[2])
					ret &= 0xbf;
				if (eti660KeyState_[1])
					ret &= 0xdf;
				if (eti660KeyState_[0])
					ret &= 0xef;
			}

			if ((outputKeyValue_&0x2) == 0)
			{
				if (eti660KeyState_[7])
					ret &= 0x7f;
				if (eti660KeyState_[6])
					ret &= 0xbf;
				if (eti660KeyState_[5])
					ret &= 0xdf;
				if (eti660KeyState_[4])
					ret &= 0xef;
			}

			if ((outputKeyValue_&0x4) == 0)
			{
				if (eti660KeyState_[0xb])
					ret &= 0x7f;
				if (eti660KeyState_[0xa])
					ret &= 0xbf;
				if (eti660KeyState_[9])
					ret &= 0xdf;
				if (eti660KeyState_[8])
					ret &= 0xef;
			}

			if ((outputKeyValue_&0x8) == 0)
			{
				if (eti660KeyState_[0xf])
					ret &= 0x7f;
				if (eti660KeyState_[0xe])
					ret &= 0xbf;
				if (eti660KeyState_[0xd])
					ret &= 0xdf;
				if (eti660KeyState_[0xc])
					ret &= 0xef;
			}

			if ((outputKeyValue_&0x10) == 0)
			{
				if (eti660KeyState_[0xc])
					ret &= 0xf7;
				if (eti660KeyState_[8])
					ret &= 0xfb;
				if (eti660KeyState_[4])
					ret &= 0xfd;
				if (eti660KeyState_[0])
					ret &= 0xfe;
			}

			if ((outputKeyValue_&0x20) == 0)
			{
				if (eti660KeyState_[0xd])
					ret &= 0xf7;
				if (eti660KeyState_[9])
					ret &= 0xfb;
				if (eti660KeyState_[5])
					ret &= 0xfd;
				if (eti660KeyState_[1])
					ret &= 0xfe;
			}

			if ((outputKeyValue_&0x40) == 0)
			{
				if (eti660KeyState_[0xe])
					ret &= 0xf7;
				if (eti660KeyState_[0xa])
					ret &= 0xfb;
				if (eti660KeyState_[6])
					ret &= 0xfd;
				if (eti660KeyState_[2])
					ret &= 0xfe;
			}

			if ((outputKeyValue_&0x80) == 0)
			{
				if (eti660KeyState_[0xf])
					ret &= 0xf7;
				if (eti660KeyState_[0xb])
					ret &= 0xfb;
				if (eti660KeyState_[7])
					ret &= 0xfd;
				if (eti660KeyState_[3])
					ret &= 0xfe;
			}

			ret &= inputKeyLatch_;
			ret |= (outputKeyValue_&outputKeyLatch_);
//			p_Main->messageHex(ret);
		}
	}
//	else
//		p_Main->message("Other Input");
	return ret;
}

void Eti::outEti(Word address, Byte value)
{
	if ((address&0x3) == 1)	// Control register A
	{
		if ((value&0x4) == 0x4)
		{
//			p_Main->message("Data Direction Register Selected");
			initiateColour(true);
			DataDirection_ = true;
		}
		else
		{
//			p_Main->message("Output Register Selected");
			DataDirection_ = false;
		}
//		p_Main->messageHex(value);
	}

	if ((address&0x3) == 0)
	{
		if (DataDirection_)	// Data Direction A
		{
//			p_Main->message("Data Direction Register");
//			p_Main->messageHex(value);
			outputKeyLatch_ = value;
			inputKeyLatch_ = value^0xff;
		}
		else				// Output Register A
		{
//			p_Main->message("Output Register");
			outputKeyValue_ = value&outputKeyLatch_;
			outputKeyValue_ |= inputKeyLatch_;
//			p_Main->messageHex(outputKeyValue_);
		}
	}
}

void Eti::cycle(int type)
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

void Eti::startComputer()
{
	resetPressed_ = false;
	ramMask_ = 0xfff;

	p_Main->setSwName("");
	p_Main->updateTitle();

    p_Main->checkAndReInstallMainRom(ETI);
	readProgram(p_Main->getRomDir(ETI, MAINROM1), p_Main->getRomFile(ETI, MAINROM1), ROM, 0, NONAME);

	if (p_Main->getRamType(ETI) == 0)
	{
		defineMemoryType(0x0400, 0xfff, RAM);
		initRam(0x400, 0xfff);
        p_Main->assDefault("mycode", 0x400, 0xEFF);
    }
	else
	{
		for (int i=0x400; i<0x800; i++)
		{
			mainMemory_[i+0x800] = mainMemory_[i];
		}
		defineMemoryType(0x0c00, 0xfff, ROM);
		defineMemoryType(0x0400, 0xbff, RAM);
		initRam(0x400, 0xbff);
        p_Main->assDefault("mycode", 0x400, 0xAFF);
	}
//	mainMemory_[0x400] = 0;
//	mainMemory_[0x401] = 0;
//	mainMemory_[0x402] = 0;
//	mainMemory_[0x403] = 0;

    if (readMem(0x1d) == 0x56)
        readProgram(p_Main->getChip8Dir(ETI), p_Main->getChip8SW(ETI), NOCHANGE, 0x0700, SHOWNAME);
    else
        readProgram(p_Main->getChip8Dir(ETI), p_Main->getChip8SW(ETI), NOCHANGE, 0x0600, SHOWNAME);
	double zoom = p_Main->getZoom();

    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);

	configurePixieEti();
	initPixie();
	setZoom(zoom);
	Show(true);

	cpuCycles_ = 0;
	p_Main->startTime();

	threadPointer->Run();
}

void Eti::writeMemDataType(Word address, Byte type)
{
	address = address & ramMask_;
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

Byte Eti::readMemDataType(Word address)
{
	address = address & ramMask_;
	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Eti::readMem(Word address)
{
	address = address & ramMask_;

	if (memoryType_[address/256] == UNDEFINED) return 255;
	return mainMemory_[address];
}

Byte Eti::readMemDebug(Word address)
{
    return readMem(address);
}

void Eti::writeMem(Word address, Byte value, bool writeRom)
{
	address = address & ramMask_;
	if (mainMemory_[address]==value)
		return;

	if (!writeRom)
		if (memoryType_[address/256] != RAM)  return;

	if (address > endSave_)
		endSave_ = address;

	mainMemory_[address]=value;
	if (writeRom)
		return;
	if (address >= (memoryStart_& ramMask_) && address < ((memoryStart_& ramMask_) + 256))
		p_Main->updateDebugMemory(address);
}

void Eti::writeMemDebug(Word address, Byte value, bool writeRom)
{
    writeMem(address, value, writeRom);
}

Byte Eti::read1864ColorDirect(Word addr)
{
	return colorMemory1864_[addr] & 0xf;
}

void Eti::write1864ColorDirect(Word addr, Byte value)
{
	colorMemory1864_[addr] = value & 0xf;
}

void Eti::cpuInstruction()
{
    cpuCycleStep();
}

void Eti::resetPressed()
{
    resetCpu();
    resetPressed_ = false;
    initPixie();
    initiateColour(false);
}

void Eti::onReset()
{
	resetPressed_ = true;
}

void Eti::checkComputerFunction()
{
	switch(scratchpadRegister_[programCounter_])
	{
		case 0x018B:	// SAVE
			if (p_Main->getAutCassetteLoad())
			{
				writeMem(0x400, 0x06, false);
				writeMem(0x401, 0, false);
				p_Main->eventSaveStart(0x600);

				writeMem(0x402, (endSave_>>8) & 0xff, false);
				writeMem(0x403, endSave_&0xff, false);
				p_Main->eventSaveEnd(endSave_);

			}
			p_Main->startCassetteSave(0);
		break;

		case 0x0160:	// LOAD
			if (p_Main->getAutCassetteLoad())
			{
				writeMem(0x400, 0x06, false);
				writeMem(0x401, 0, false);
				p_Main->eventSaveStart(0x600);
			}
			p_Main->startCassetteLoad(0);
		break;
	}
}

void Eti::finishStopTape()
{
	Word end = scratchpadRegister_[0xe] - 1;
	if (p_Main->getAutCassetteLoad())
	{
		p_Computer->writeMem(0x402, (end>>8) & 0xff, false);
		p_Computer->writeMem(0x403, (end & 0xff), false);
	}
	p_Main->eventSaveEnd(end);
	resetPressed_ = true;
}

void Eti::keyClear()
{
	for (int i=0; i<16; i++)
		eti660KeyState_[i] = false;
}

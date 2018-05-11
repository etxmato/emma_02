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
#include "fred.h"

#define CHIP8_PC 5

Fred::Fred(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType)
:Pixie(title, pos, size, zoom, zoomfactor, computerType)
{
    ef1State_ = 1;
    
    keyPadActive_ = false;
    displayType_ = 0x3;
    
    currentTapeState_ = SOUND_DOWN;
    pulseLength_ = 0;
    lastSample_ = 0;
    pulseCount_ = 0;
    tapeInput_ = 0;
    minValue_ = 0;
    maxValue_ = 0;
    polarity_ = 0;
    bitNumber_ = -1;
}

Fred::~Fred()
{
	p_Main->setMainPos(FRED, GetPosition());
}

void Fred::configureComputer()
{
    inType_[0] = FREDINP0;
    outType_[1] = FREDIOGROUP;
    outType_[2] = FREDIO2;
    outType_[3] = FREDIO3;
    outType_[4] = FREDIO4;
	efType_[1] = FREDEF1;
    efType_[2] = FREDEF2;
    efType_[4] = FREDEF4;
    
	p_Main->message("Configuring FRED");
    p_Main->message("	Output 1: set I/O group\n");
    
    p_Main->message("	I/O group 1: keypad");
    p_Main->message("	I/O group 2: TV");
    p_Main->message("	I/O group 3: tape");
    
    p_Main->message("");
    
    p_Main->message("Configuring hex keypad support");
    p_Main->message("	Input 0: read byte");
    p_Main->message("	EF 1: data ready\n");
    
    p_Main->message("Configuring TV support");
    p_Main->message("	Output 2: display type 0 = 32x16, 1 = 32x32, 2 = 64x16, 3 = 64x32");
    
    p_Main->message("Configuring tape support");
    p_Main->message("	Output 2: bit 4 = program mode, bit 5 = direct mode, bit 6 = write mode");
    p_Main->message("	Output 3: bit 0 = 1 - run tape, bit 1 = 1 - sound on");
    p_Main->message("	Input 0: read byte");
    p_Main->message("	EF 1: data ready\n");
    p_Main->message("	EF 2: tape run/stop\n");
    p_Main->message("	EF 4: tape error\n");
    
    p_Main->getDefaultHexKeys(FRED, "FRED", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);

    if (p_Main->getConfigBool("/FRED/GameAuto", true))
        p_Main->loadKeyDefinition(p_Main->getRamFile(FRED), p_Main->getRamFile(ELF), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");

    resetCpu();
}

void Fred::reDefineKeys(int hexKeyDefA1[], int hexKeyDefA2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefA1_[i] = hexKeyDefA1[i];
        keyDefA2_[i] = hexKeyDefA2[i];
    }
}

void Fred::initComputer()
{
    setClear(1);
    setWait(1);
    
    for (int i=0; i<16; i++)
    {
        keyState_[i] = 0;
    }
    ioGroup_ = 0;
}

void Fred::keyDown(int keycode)
{
#if defined (__WXMAC__)
//    if (ef1State_ == 0) // This is to avoid multiple key presses on OSX
//        return;
#endif

    for (int i=0; i<16; i++)
    {
        if (keycode == keyDefA1_[i])
        {
            ef1State_ = 0;
            keyValue_ = i;
//            switches_ = ((switches_ << 4) & 0xf0) | i;
        }
        if (keycode == keyDefA2_[i])
        {
            ef1State_ = 0;
            keyValue_ = i;
//            switches_ = ((switches_ << 4) & 0xf0) | i;
        }
    }
}

void Fred::keyUp(int keycode)
{
    ef1State_ = 1;
}

Byte Fred::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

        case FREDEF1:
            return ef1();
        break;
            
		case FREDEF2:
			return ef2();
		break;

		default:
			return 1;
	}
}

Byte Fred::ef1()
{
    return ef1State_;
}

Byte Fred::ef2()
{
    return 1;
}

Byte Fred::in(Byte port, Word WXUNUSED(address))
{
	Byte ret = 255;

	switch(inType_[port])
	{
        case FREDINP0:
            ret = 255;
            switch (ioGroup_)
            {
                case IO_GRP_FRED_KEYPAD:
                    if (keyPadActive_)
                    {
                        ret = keyValue_;
                        ef1State_ = 1;
                    }
                break;
                case IO_GRP_FRED_TAPE:
                    ret = tapeInput_ & 0xff;
                    ef1State_ = 1;
                break;
            }
        break;
    }
	inValues_[port] = ret;
	return ret;
}

void Fred::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;

	switch(outType_[port])
	{
		case 0:
			return;
		break;

        case FREDIOGROUP:
            ioGroup_ = value;
        break;
            
        case FREDIO2:
            switch (ioGroup_)
            {
                case IO_GRP_FRED_KEYPAD:
                    if (value  == 1)
                        keyPadActive_ = true;
                    else
                        keyPadActive_ = false;
                break;
                    
                case IO_GRP_FRED_TV:
                    inPixie();
                    displayType_ = value;
                break;
                    
                case IO_GRP_FRED_TAPE:
					currentTapeState_ = SOUND_DOWN;
					pulseLength_ = 0;
					lastSample_ = 0;
					pulseCount_ = 0;
					tapeInput_ = 0;
                    minValue_ = 0;
                    maxValue_ = 0;
                    polarity_ = 0;
                    bitNumber_ = -1;
                break;
            }
        break;
    }
}

void Fred::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

		case PIXIECYCLE:
			cyclePixieFred(displayType_);
		break;
	}
}

void Fred::startComputer()
{
	resetPressed_ = false;

	p_Main->setSwName("");

    defineMemoryType(0x000, 0xffff, RAM);
//    for (int i=0x800; i<0xff00; i+=0x800)
//        defineMemoryType(i, i+0x7ff, MAPPEDRAM);
    initRam(0, 0xffff);

    readProgram(p_Main->getRamDir(FRED), p_Main->getRamFile(FRED), RAM, 0, NONAME);
    
    if (mainMemory_[0] == 0 && mainMemory_[0x2a] == 0xF8 && mainMemory_[0x100] == 0 && mainMemory_[0x210] == 0x52)
    {
        chip8baseVar_ = 0x100;
        chip8mainLoop_ = 0x13B;
        chip8type_ = CHIPFEL1;
    }
    if (chip8type_ != CHIP_NONE)
        p_Main->defineFelCommands_(chip8type_);

    p_Main->assDefault("fred", 0, 0x7FF);

	double zoom = p_Main->getZoom();

	configurePixieFred();
	initPixie();
	setZoom(zoom);
	Show(true);
	setWait(1);
	setClear(0);
	setWait(1);
	setClear(1);

	p_Main->updateTitle();

	cpuCycles_ = 0;
	p_Main->startTime();

	threadPointer->Run();
    if (mainMemory_[0] == 0)
        p_Computer->dmaOut(); // skip over IDL instruction, must be a RCA FRED COSMAC 1801 Game System
}

void Fred::writeMemDataType(Word address, Byte type)
{
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
            
		case MAPPEDRAM:
			address = address & 0x7ff;
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;
	}
}

Byte Fred::readMemDataType(Word address)
{
    switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
			return mainMemoryDataType_[address];
		break;

        case MAPPEDRAM:
            address = address & 0x7ff;
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Fred::readMem(Word addr)
{
	address_ = addr;

	switch (memoryType_[addr/256])
	{
		case UNDEFINED:
			return 255;
		break;

        case MAPPEDRAM:
			addr = addr & 0x7ff;
		break;
 	}
    return mainMemory_[addr];
}

void Fred::writeMem(Word addr, Byte value, bool writeRom)
{
	address_ = addr;

	switch (memoryType_[addr/256])
	{
		case RAM:
			if (mainMemory_[addr]==value)
				return;
			mainMemory_[addr]=value;
			if (addr>= memoryStart_ && addr<(memoryStart_+256))
				p_Main->updateDebugMemory(addr);
			p_Main->updateAssTabCheck(addr);
		break;
            
		case MAPPEDRAM:
			addr = addr & 0x7ff;
			if (mainMemory_[addr]==value)
				return;
			mainMemory_[addr]=value;
			if (addr>= memoryStart_ && addr<(memoryStart_+256))
				p_Main->updateDebugMemory(addr);
			p_Main->updateAssTabCheck(addr);
		break;

		default:
			if (writeRom)
				mainMemory_[addr]=value;
		break;
	}
}

void Fred::cpuInstruction()
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

		if (resetPressed_)
		{
			resetCpu();
			resetPressed_ = false;

            
            if (mainMemory_[0] == 0 && mainMemory_[0x2a] == 0xF8 && mainMemory_[0x100] == 0 && mainMemory_[0x210] == 0x52)
            {
                chip8baseVar_ = 0x100;
                chip8mainLoop_ = 0x13B;
                chip8type_ = CHIPFEL1;
            }
            
            setWait(1);
			setClear(0);
			setWait(1);
			setClear(1);
			initPixie();
            ioGroup_ = 0;
            if (mainMemory_[0] == 0)
                p_Computer->dmaOut(); // skip over IDL instruction, must be a RCA FRED COSMAC 1801 Game System
		}
		if (debugMode_)
			p_Main->cycleDebug();
        if (chip8type_ != CHIP_NONE)
            p_Main->cycleFredDebug();
	}
	else
	{
		initPixie();
		cpuCycles_ = 0;
		p_Main->startTime();
	}
}

void Fred::onReset()
{
	resetPressed_ = true;
}

void Fred::cassetteFred(short val)
{
    switch (currentTapeState_)
    {
        case SOUND_DOWN:
            if (val < lastSample_)
            {
                pulseLength_++;
                minValue_ = val;
            }
            else
            {
                if ((maxValue_ - minValue_) > 6000)
                {
                    if (pulseLength_ > 4)
                        pulseCount_++;
          //          else
            //            pulseCount_ = 0;
                    pulseLength_ = 0;
                }
                maxValue_ = minValue_;
                currentTapeState_ = SOUND_UP;
            }
        break;
        case SOUND_UP:
            if (val > lastSample_)
            {
                pulseLength_++;
                maxValue_ = val;
            }
            else
            {
                if ((maxValue_ - minValue_) > 6000)
                {
                    if (pulseLength_ > 4)
                        pulseCount_++;
          //          else
           //             pulseCount_ = 0;
                    pulseLength_ = 0;
                }
                minValue_ = maxValue_;
                currentTapeState_ = SOUND_DOWN;
            }
        break;
    }
    
    checkBit();
    lastSample_ = val;
}

void Fred::checkBit()
{
    if (pulseCount_ > 2 && pulseLength_ > 25)
  //  if (minValue_ !=0 && (maxValue_ - minValue_) < 500)
    {
        if (bitNumber_ == 8)
        {
            if (pulseCount_ > 6 && (polarity_ & 1) != 1)
                ef4State_ = 0;

            dmaIn(tapeInput_);
            
            ef1State_ = 0;
            tapeInput_ = 0;
            polarity_ = 0;
            bitNumber_ = -1;
        }
        else
        {
            if (bitNumber_ != -1)
            {
                if (pulseCount_ > 6)
                    tapeInput_ = (1 << bitNumber_) | tapeInput_;
            }
            if (pulseCount_ > 6)
                polarity_++;
            bitNumber_++;
        }
        pulseCount_ = 0;
        
    }
}

void Fred::cassetteFred(char val)
{
}



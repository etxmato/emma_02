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
}

Fred::~Fred()
{
	p_Main->setMainPos(FRED, GetPosition());
}

void Fred::configureComputer()
{
    inType_[5] = COINARCADEINPPAR5;
    inType_[6] = COINARCADEINPKEY6;
    outType_[3] = COINARCADEOUTTONE6;
    outType_[5] = COINARCADEOUTFREQ5;
    outType_[6] = COINARCADEOUTTONE6;
	efType_[1] = COINARCADEEF1;
    efType_[3] = COINARCADEEF3;
    efType_[4] = COINARCADEEF4;

    directionKey_ = 0;
    fireKeyA_ = 1;
    fireKeyB_ = 1;
    coinKey_ = 1;
    
	p_Main->message("Configuring FRED");
    p_Main->message("	EF1: fire player A, EF3: fire player B, EF4: coin");
    p_Main->message("	Input 5: parameter switch, input 6: direction keys & coin reset");
    p_Main->message("	Output 5: tone latch, output 3 & 6: tone on/off");

    keyDefCoin_ = p_Main->getDefaultCoinArcadeKeys(keyDefA_, keyDefB_);

	resetCpu();
}

void Fred::reDefineKeys(int keyDefA[], int keyDefB[], int coin)
{
	for (int i=0; i<4; i++)
	{
        keyDefA_[i] = keyDefA[i];
        keyDefB_[i] = keyDefB[i];
	}
    keyDefCoin_ = coin;
}

void Fred::keyDown(int keycode)
{
    if (keycode == keyDefCoin_)
        coinKey_ = 0;
    
    if (keycode == keyDefA_[KEY_UP])
        directionKey_ |= 0x02;
    if (keycode == keyDefA_[KEY_LEFT])
        directionKey_ |= 0x01;
    if (keycode == keyDefA_[KEY_RIGHT])
        directionKey_ |= 0x04;
    if (keycode == keyDefA_[KEY_DOWN])
        directionKey_ |= 0x08;
    if (keycode == keyDefA_[KEY_FIRE])
        fireKeyA_ = 0;
    
    if (keycode == keyDefB_[KEY_UP])
        directionKey_ |= 0x20;
    if (keycode == keyDefB_[KEY_LEFT])
        directionKey_ |= 0x10;
    if (keycode == keyDefB_[KEY_RIGHT])
        directionKey_ |= 0x40;
    if (keycode == keyDefB_[KEY_DOWN])
        directionKey_ |= 0x80;
    if (keycode == keyDefB_[KEY_FIRE])
        fireKeyB_ = 0;

    if (keycode == '1')
        directionKey_ |= 0x01;
    if (keycode == '2')
        directionKey_ |= 0x02;
    if (keycode == '3')
        directionKey_ |= 0x04;
    if (keycode == '4')
        directionKey_ |= 0x08;
}

void Fred::keyUp(int keycode)
{
//    if (keycode == keyDefCoin_)
//        coinKey_ = 1;
    
    if (keycode == keyDefA_[KEY_UP])
        directionKey_ &= 0xFD;
    if (keycode == keyDefA_[KEY_LEFT])
        directionKey_ &= 0xFE;
    if (keycode == keyDefA_[KEY_RIGHT])
        directionKey_ &= 0xFB;
    if (keycode == keyDefA_[KEY_DOWN])
        directionKey_ &= 0xF7;
    if (keycode == keyDefA_[KEY_FIRE])
        fireKeyA_ = 1;
    
    if (keycode == keyDefB_[KEY_UP])
        directionKey_ &= 0xDF;
    if (keycode == keyDefB_[KEY_LEFT])
        directionKey_ &= 0xEF;
    if (keycode == keyDefB_[KEY_RIGHT])
        directionKey_ &= 0xBF;
    if (keycode == keyDefB_[KEY_DOWN])
        directionKey_ &= 0x7F;
    if (keycode == keyDefB_[KEY_FIRE])
        fireKeyB_ = 1;

    if (keycode == '1')
        directionKey_ &= 0xFE;
    if (keycode == '2')
        directionKey_ &= 0xFD;
    if (keycode == '3')
        directionKey_ &= 0xFB;
    if (keycode == '4')
        directionKey_ &= 0xF7;
}

Byte Fred::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

        case COINARCADEEF1:
            return ef1();
        break;
            
		case COINARCADEEF3:
			return ef3();
		break;

		case COINARCADEEF4:
			return ef4();
		break;

		default:
			return 1;
	}
}

Byte Fred::ef1()
{
    return fireKeyA_;
}

Byte Fred::ef3()
{
    return fireKeyB_;
}

Byte Fred::ef4()
{
    return coinKey_;
}

Byte Fred::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

        case COINARCADEINPPAR5:
            ret = 0;    // COIN_ARCADE_PARAMETER_SWITCH = 0; 8 is test mode?
        break;
            
        case COINARCADEINPKEY6:
            coinKey_ = 1;
            ret = directionKey_;
        break;
            
		default:
			ret = 255;
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

		case PIXIEOUT:
			inPixie();
		break;
            
        case COINARCADEOUTFREQ4:
            tone1864Latch(value);
        break;
            
        case COINARCADEOUTFREQ5:
            tone1864Latch(value);
        break;
            
        case COINARCADEOUTTONE6:
            if (value != 0)
                tone1864On();
            else
                beepOff();
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
			cyclePixieCoinArcade();
		break;
	}
}

void Fred::startComputer()
{
	resetPressed_ = false;

	p_Main->setSwName("");

    defineMemoryType(0x000, 0x7ff, RAM);
    for (int i=0x800; i<0xff00; i+=0x800)
        defineMemoryType(i, i+0x7ff, MAPPEDRAM);
    initRam(0, 0x7ff);

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

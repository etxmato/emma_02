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
#include "coinarcade.h"

CoinArcade::CoinArcade(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType)
:Pixie(title, pos, size, zoom, zoomfactor, computerType)
{
}

CoinArcade::~CoinArcade()
{
	p_Main->setMainPos(COINARCADE, GetPosition());
}

void CoinArcade::configureComputer()
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
    
	p_Main->message("Configuring RCA Video Coin Arcade");
    p_Main->message("	EF1: fire player A, EF3: fire player B, EF4: coin");
    p_Main->message("	Input 5: parameter switch, input 6: direction keys & coin reset");
    p_Main->message("	Output 5: tone latch, output 3 & 6: tone on/off");

    keyDefCoin_ = p_Main->getDefaultCoinArcadeKeys(keyDefA_, keyDefB_);

	resetCpu();
}

void CoinArcade::reDefineKeys(int keyDefA[], int keyDefB[], int coin)
{
	for (int i=0; i<4; i++)
	{
        keyDefA_[i] = keyDefA[i];
        keyDefB_[i] = keyDefB[i];
	}
    keyDefCoin_ = coin;
}

void CoinArcade::keyDown(int keycode)
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

void CoinArcade::keyUp(int keycode)
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

Byte CoinArcade::ef(int flag)
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

Byte CoinArcade::ef1()
{
    return fireKeyA_;
}

Byte CoinArcade::ef3()
{
    return fireKeyB_;
}

Byte CoinArcade::ef4()
{
    return coinKey_;
}

Byte CoinArcade::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

        case COINARCADEINPPAR5:
            ret = 0x1;    // COIN_ARCADE_PARAMETER_SWITCH = 0; 8 is test mode?
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

void CoinArcade::out(Byte port, Word WXUNUSED(address), Byte value)
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

void CoinArcade::cycle(int type)
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

void CoinArcade::startComputer()
{
	resetPressed_ = false;

	p_Main->setSwName("");

    defineMemoryType(0x800, RAM);
    defineMemoryType(0x900, RAM);
    for (int i=0x1800; i<0xff00; i+=0x400)
	{
        defineMemoryType(i, MAPPEDRAM);
        defineMemoryType(i+0x100, MAPPEDRAM);
	}

    p_Main->checkAndReInstallMainRom(COINARCADE);
    readProgram(p_Main->getRomDir(COINARCADE, MAINROM1), p_Main->getRomFile(COINARCADE, MAINROM1), ROM, 0, NONAME);
    
    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);

    p_Main->assDefault("coinarcade", 0, 0x7FF);

    defineMemoryType(0x800, 0x9ff, RAM);
    initRam(0x800, 0x9ff);

	double zoom = p_Main->getZoom();

	configurePixieCoinArcade();
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

void CoinArcade::writeMemDataType(Word address, Byte type)
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
			address = (address & 0x1ff) | 0x800;
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;
	}
}

Byte CoinArcade::readMemDataType(Word address)
{
    switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
			return mainMemoryDataType_[address];
		break;

        case MAPPEDRAM:
			address = (address & 0x1ff) | 0x800;
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte CoinArcade::readMem(Word address)
{
	switch (memoryType_[address/256])
	{
		case UNDEFINED:
			return 255;
		break;

        case MAPPEDRAM:
			address = (address & 0x1ff) | 0x800;
		break;
 	}
    return mainMemory_[address];
}

Byte CoinArcade::readMemDebug(Word address)
{
    return readMem(address);
}

void CoinArcade::writeMem(Word address, Byte value, bool writeRom)
{
	switch (memoryType_[address/256])
	{
		case RAM:
			if (mainMemory_[address]==value)
				return;
			mainMemory_[address]=value;
			if (address>= memoryStart_ && address<(memoryStart_+256))
				p_Main->updateDebugMemory(address);
			p_Main->updateAssTabCheck(address);
		break;
            
		case MAPPEDRAM:
			address = (address & 0x1ff) | 0x800;
			if (mainMemory_[address]==value)
				return;
			mainMemory_[address]=value;
			if (address>= memoryStart_ && address<(memoryStart_+256))
				p_Main->updateDebugMemory(address);
			p_Main->updateAssTabCheck(address);
		break;

		default:
			if (writeRom)
				mainMemory_[address]=value;
		break;
	}
}

void CoinArcade::writeMemDebug(Word address, Byte value, bool writeRom)
{
    writeMem(address, value, writeRom);
}

void CoinArcade::cpuInstruction()
{
	if (cpuMode_ == RUN)
	{
        cpuCycleStep();
	}
	else
	{
		initPixie();
		cpuCycles_ = 0;
		p_Main->startTime();
	}
}

void CoinArcade::resetPressed()
{
    resetCpu();
    resetPressed_ = false;
    
    setWait(1);
    setClear(0);
    setWait(1);
    setClear(1);
    initPixie();
    if (mainMemory_[0] == 0)
        p_Computer->dmaOut(); // skip over IDL instruction, must be a RCA FRED COSMAC 1801 Game System
}

void CoinArcade::onReset()
{
	resetPressed_ = true;
}

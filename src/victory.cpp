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
#include "victory.h"

#define CHIP8_PC 5

Victory::Victory(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType)
:Pixie(title, pos, size, zoom, zoomfactor, computerType)
{
}

Victory::~Victory()
{
	p_Main->setMainPos(VICTORY, GetPosition());
}

void Victory::configureComputer()
{
	chip8baseVar_ = 0x8c0;
	chip8mainLoop_ = 0x6b;
	chip8type_ = CHIPST2;

	outType_[2] = STUDIOOUT;
	outType_[4] = VIPOUT4;
	victoryKeyPort_ = 0;
	efType_[3] = STUDIOEF3;
	efType_[4] = STUDIOEF4;

	for (int j=0; j<2; j++) for (int i=0; i<10; i++)
		victoryKeyState_[j][i] = 0;

	p_Main->message("Configuring Studio III / Victory MPT-02");
	p_Main->message("	Output 2: select port, EF 3: read selected port 1, EF4: read selected port 2\n");

	p_Main->getDefaultHexKeys(VICTORY, "Victory", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
	p_Main->getDefaultHexKeys(VICTORY, "Victory", "B", keyDefB1_, keyDefB2_, keyDefGameHexB_);

    gameAuto_ = p_Main->getConfigBool("/Victory/GameAuto", true);

	multiCart_ = p_Main->getUseMultiCart(VICTORY);
	disableSystemRom_ = p_Main->getDisableSystemRom(VICTORY);
	multiCartLsb_ = p_Main->getMultiCartLsb(VICTORY);
	multiCartMsb_ = p_Main->getMultiCartMsb(VICTORY);

	simDefA2_ = p_Main->getConfigBool("/Victory/DiagonalA2", true);
    simDefB2_ = p_Main->getConfigBool("/Victory/DiagonalB2", true);
    
	resetCpu();
}

void Victory::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
	for (int i=0; i<512; i++)
	{
		keyDefinition[i].defined = false;
	}
	for (int i=0; i<10; i++)
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

void Victory::reDefineKeysB(int hexKeyDefB1[], int hexKeyDefB2[])
{
	for (int i = 0; i<10; i++)
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

void Victory::keyDown(int keycode)
{
	if (keyDefinition[keycode].defined)
	{
		if (simDefA2_)
		{
			if (keycode == keyDefA2_[2])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[4]) == true)
				{
					victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key - 1] = 1;
					victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
					victoryKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[6]) == true)
				{
					victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key + 1] = 1;
					victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
					victoryKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
					return;
				}
			}
			if (keycode == keyDefA2_[4])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[2]) == true)
				{
					victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key - 1] = 1;
					victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
					victoryKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[8]) == true)
				{
					victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key - 1] = 1;
					victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
					victoryKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
					return;
				}
			}
			if (keycode == keyDefA2_[6])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[2]) == true)
				{
					victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key + 1] = 1;
					victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
					victoryKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[8]) == true)
				{
					victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key + 1] = 1;
					victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
					victoryKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
					return;
				}
			}
			if (keycode == keyDefA2_[8])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[4]) == true)
				{
					victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key - 1] = 1;
					victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
					victoryKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[6]) == true)
				{
					victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key + 1] = 1;
					victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
					victoryKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
					return;
				}
			}
		}

		if (simDefB2_)
		{
			if (keycode == keyDefB2_[2])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[4]) == true)
				{
					victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key - 1] = 1;
					victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
					victoryKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[6]) == true)
				{
					victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key + 1] = 1;
					victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
					victoryKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
					return;
				}
			}
			if (keycode == keyDefB2_[4])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[2]) == true)
				{
					victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key - 1] = 1;
					victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
					victoryKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[8]) == true)
				{
					victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key - 1] = 1;
					victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
					victoryKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
					return;
				}
			}
			if (keycode == keyDefB2_[6])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[2]) == true)
				{
					victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key + 1] = 1;
					victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
					victoryKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[8]) == true)
				{
					victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key + 1] = 1;
					victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
					victoryKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
					return;
				}
			}
			if (keycode == keyDefB2_[8])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[4]) == true)
				{
					victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key - 1] = 1;
					victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
					victoryKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[6]) == true)
				{
					victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key + 1] = 1;
					victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
					victoryKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
					return;
				}
			}
		}
		victoryKeyState_[keyDefinition[keycode].player][keyDefinition[keycode].key] = 1;
	}
}

void Victory::keyUp(int keycode)
{
	if (simDefA2_)
	{
        if (keycode == keyDefA2_[2] || keycode == keyDefA2_[4] || keycode == keyDefA2_[6] || keycode == keyDefA2_[8])
        {
            victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key-1] = 0;
            victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key+1] = 0;
            victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key-1] = 0;
            victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key+1] = 0;
            if (::wxGetKeyState((wxKeyCode)keyDefA2_[2]) == true)
                victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 1;
            if (::wxGetKeyState((wxKeyCode)keyDefA2_[4]) == true)
                victoryKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 1;
            if (::wxGetKeyState((wxKeyCode)keyDefA2_[6]) == true)
                victoryKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 1;
            if (::wxGetKeyState((wxKeyCode)keyDefA2_[8]) == true)
                victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 1;
        }
	}
	if (simDefB2_)
	{
		if (keycode == keyDefB2_[2] || keycode == keyDefB2_[4] || keycode == keyDefB2_[6] || keycode == keyDefB2_[8])
		{
			victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key - 1] = 0;
			victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key + 1] = 0;
			victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key - 1] = 0;
			victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key + 1] = 0;
			if (::wxGetKeyState((wxKeyCode)keyDefB2_[2]) == true)
				victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 1;
			if (::wxGetKeyState((wxKeyCode)keyDefB2_[4]) == true)
				victoryKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 1;
			if (::wxGetKeyState((wxKeyCode)keyDefB2_[6]) == true)
				victoryKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 1;
			if (::wxGetKeyState((wxKeyCode)keyDefB2_[8]) == true)
				victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 1;
		}
	}
	if (keyDefinition[keycode].defined)
		victoryKeyState_[keyDefinition[keycode].player][keyDefinition[keycode].key] = 0;
}

Byte Victory::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

		case PIXIEEF:
			return efPixie();
		break;

		case STUDIOEF3:
			return ef3();
		break;

		case STUDIOEF4:
			return ef4();
		break;

		default:
			return 1;
	}
}

Byte Victory::ef3()
{
	if (victoryKeyPort_<0 || victoryKeyPort_>9)
		return 1;
	return(victoryKeyState_[0][victoryKeyPort_]) ? 0 : 1;
}

Byte Victory::ef4()
{
	if (victoryKeyPort_<0 || victoryKeyPort_>9)
		return 1;
	return(victoryKeyState_[1][victoryKeyPort_]) ? 0 : 1;
}

Byte Victory::in(Byte port, Word WXUNUSED(address))
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

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Victory::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;

	switch(outType_[port])
	{
		case 0:
			return;
		break;

		case STUDIOOUT:
			outVictory(value);
		break;

		case VIPOUT4:
			tone1864Latch(value);
		break;

		case PIXIEBACKGROUND:
			outPixieBackGround();
		break;
	}
}

void Victory::outVictory(Byte value)
{
	victoryKeyPort_ = value & 0xf;
}

void Victory::cycle(int type)
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

void Victory::startComputer()
{
	resetPressed_ = false;

	p_Main->setSwName("");

	for (int i=0xc00; i<0xff00; i+=0x400)
	{
		defineMemoryType(i, MAPPEDRAM);
		defineMemoryType(i+0x100, MAPPEDRAM);
	}

    readProgram(p_Main->getRomDir(VICTORY, MAINROM1), p_Main->getRomFile(VICTORY, MAINROM1), ROM, 0, NONAME);

    if (multiCart_)
    {
        if (readMultiCartBinFile(p_Main->getRomDir(VICTORY, CARTROM), p_Main->getRomFile(VICTORY, CARTROM)))
        {
            if (disableSystemRom_)
                defineMemoryType(0, 0x7ff, MULTICART);
            else
                defineMemoryType(0x400, 0x7ff, MULTICART);
            defineMemoryType(0xc00, 0xfff, MULTICART);
            for (int i=0x1000; i<0xff00; i+=0x1000)
            {
                defineMemoryType(0+i, 0x7ff+i, MAPPEDMULTICART);
                defineMemoryType(0xc00+i, 0xfff+i, MAPPEDMULTICART);
            }
        }
        else
            multiCart_ = false;
    }
    
    if (readMem(0) == 0x90)
	{
		chip8baseVar_ = 0x8c0;
		chip8mainLoop_ = 0x6b;
		chip8type_ = CHIPST2;
        if (!multiCart_)
        {
            readSt2Program(VICTORY);
            p_Main->assDefault("victorycart", 0x400, 0x7FF);
        }
        else
        {
            p_Main->assDefault("victorycart_1", 0x400, 0x7FF);
            p_Main->assDefault("victorycart_2", 0xC00, 0xFFF);
        }
    }
	else
	{
		chip8baseVar_ = 0x8f0;
		chip8mainLoop_ = 0x1c;
		chip8type_ = CHIP8;
		if (!multiCart_)
			readProgram(p_Main->getRomDir(VICTORY, CARTROM), p_Main->getRomFile(VICTORY, CARTROM), ROM, 0x300, NONAME);
	}

    if (chip8type_ != CHIP_NONE)
        p_Main->definePseudoCommands(chip8type_);

//    if (testCartMemoryDefined_)
 //       readProgram(p_Main->getRomDir(VICTORY, MAINROM1), p_Main->getRomFile(VICTORY, MAINROM1), ROM, 0x2000, NONAME);
    
    defineMemoryType(0x800, 0x9ff, RAM);
    initRam(0x800, 0x9ff);
	defineMemoryType(0xa00, 0);
	defineMemoryType(0xb00, COLOURRAM);
    
    if (mainMemory_[0x400]==0x4 && mainMemory_[0x500]==0xab && mainMemory_[0x600]==0xf8 && mainMemory_[0x700]==0xd5)
    {
        for (int address=0x2000; address<0x4000; address+=0x800)
        {
            defineMemoryType(address, address+0x3ff, MAPPEDROM);
            defineMemoryType(address+0x400, address+0x7ff, CARTRIDGEROM);
        }
        defineMemoryType(0x4000, 0x7fff, CARTRIDGEROM);
    }

	double zoom = p_Main->getZoom();

	configurePixieVictory();
	initPixie();
	setZoom(zoom);
	Show(true);
	setWait(1);
	setClear(0);
	setWait(1);
	setClear(1);

	if (multiCart_)
	{
		wxString game;
        if (multiCartRom_[(0xa00 + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] < 32)
            game = p_Main->getMultiCartGame(multiCartMsb_, multiCartLsb_);
        else
            game = getMultiCartGame();
        if (gameAuto_)
			p_Main->loadKeyDefinition("", game, keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
		if (game.Find('.'))
			game = game.BeforeFirst('.');
		p_Main->setSwName(game);
	}
	else
	{
		if (gameAuto_)
			p_Main->loadKeyDefinition("", p_Main->getRomFile(VICTORY, CARTROM), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
	}

	reDefineKeysA(keyDefA1_, keyDefA2_);
	reDefineKeysB(keyDefB1_, keyDefB2_);

	p_Main->updateTitle();

	cpuCycles_ = 0;
	p_Main->startTime();

	threadPointer->Run();
}

void Victory::writeMemDataType(Word address, Byte type)
{
	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
        case MAPPEDROM:
        case CARTRIDGEROM:
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            break;
            
        case TESTCARTRIDGEROM:
            if (testCartRomDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                testCartRomDataType_[address] = type;
            }
        break;
            
		case MULTICART:
			if ((address < 0x400) && !disableSystemRom_)
			{
				if (mainMemoryDataType_[address] != type)
				{
					p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
					mainMemoryDataType_[address] = type;
				}
			}
			else
			{
				if (multiCartRomDataType_[(address + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] != type)
				{
					p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
					multiCartRomDataType_[(address + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] = type;
				}
			}
		break;

        case MAPPEDMULTICART:
            address = address & 0xfff;
            if ((address < 0x400) && !disableSystemRom_)
            {
                if (mainMemoryDataType_[address] != type)
                {
                    p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                    mainMemoryDataType_[address] = type;
                }
            }
            else
            {
                if (multiCartRomDataType_[(address + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] != type)
                {
                    p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                    multiCartRomDataType_[(address + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] = type;
                }
            }
        break;
            
		case MAPPEDRAM:
			address = (address & 0x1ff) | 0x800;
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address]=type;
			}
		break;
	}
}

Byte Victory::readMemDataType(Word address)
{
	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
        case MAPPEDROM:
        case CARTRIDGEROM:
            return mainMemoryDataType_[address];
        break;
            
        case TESTCARTRIDGEROM:
            return testCartRomDataType_[address];
        break;
            
		case MULTICART:
			if ((address < 0x400) && !disableSystemRom_)
				return mainMemoryDataType_[address];
			else
				return multiCartRomDataType_[(address + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_];
		break;

        case MAPPEDMULTICART:
            address = address & 0xfff;
            if ((address < 0x400) && !disableSystemRom_)
                return mainMemoryDataType_[address];
            else
                return multiCartRomDataType_[(address+multiCartLsb_*0x1000+multiCartMsb_*0x10000)&multiCartMask_];
        break;
            
		case MAPPEDRAM:
			address = (address & 0x1ff) | 0x800;
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Victory::readMem(Word addr)
{
	address_ = addr;

	switch (memoryType_[addr/256])
	{
		case UNDEFINED:
			return 255;
		break;

		case MULTICART:
			if ((addr < 0x400) && !disableSystemRom_)
				return mainMemory_[addr];
			else
				return multiCartRom_[(addr + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_];
		break;

        case MAPPEDMULTICART:
            addr = addr & 0xfff;
            if ((addr < 0x400) && !disableSystemRom_)
                return mainMemory_[addr];
            else
                return multiCartRom_[(addr+multiCartLsb_*0x1000+multiCartMsb_*0x10000)&multiCartMask_];
        break;
            
        case COLOURRAM:
			if ((addr & 0xff) < 0x40)
				return colorMemory1864_[addr&0xff] & 0xf;
			else
				return 255;
        break;
            
        case MAPPEDRAM:
			addr = (addr & 0x1ff) | 0x800;
		break;

        case CARTRIDGEROM:
  //          addr = (addr & 0x3ff) | 0x400;
        break;
            
        case TESTCARTRIDGEROM:
            return testCartRom_[addr];
        break;
            
        case MAPPEDROM:
            addr = (addr & 0x3ff);
        break;
    }

	return mainMemory_[addr];
}

void Victory::writeMem(Word addr, Byte value, bool writeRom)
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

		case MULTICART:
			if (writeRom)
			{
				if ((addr < 0x400) && !disableSystemRom_)
					mainMemory_[addr] = value;
				else
					multiCartRom_[(addr + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] = value;
			}
		break;

        case MAPPEDMULTICART:
            addr = addr & 0xfff;
            if (writeRom)
            {
                if ((addr < 0x400) && !disableSystemRom_)
                    mainMemory_[addr] = value;
                else
                    multiCartRom_[(addr + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] = value;
            }
        break;
            
		case COLOURRAM:
			if ((addr & 0xff) < 0x40)
			{
				colorMemory1864_[addr & 0xff] = value & 0xf;
				if ((addr & 0xff) >= memoryStart_ && (addr & 0xff) < (memoryStart_ + 256))
					p_Main->updateDebugMemory(addr & 0xff);
				if (addr >= memoryStart_ && addr < (memoryStart_ + 256))
					p_Main->updateDebugMemory(addr);
				p_Main->updateAssTabCheck(addr);
				useColour(7);
			}
		break;

		case MAPPEDRAM:
			addr = (addr & 0x1ff) | 0x800;
			if (mainMemory_[addr]==value)
				return;
			mainMemory_[addr]=value;
			if (addr>= memoryStart_ && addr<(memoryStart_+256))
				p_Main->updateDebugMemory(addr);
			p_Main->updateAssTabCheck(addr);
		break;

        case TESTCARTRIDGEROM:
            if (writeRom)
            {
                testCartRom_[addr] = value;
            }
        break;
            
		default:
			if (writeRom)
				mainMemory_[addr]=value;
		break;
	}
}

void Victory::cpuInstruction()
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
        checkFunction();

		if (resetPressed_)
		{
			resetCpu();
			resetPressed_ = false;
 
            p_Main->getDefaultHexKeys(VICTORY, "Victory", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
            p_Main->getDefaultHexKeys(VICTORY, "Victory", "B", keyDefB1_, keyDefB2_, keyDefGameHexB_);

            simDefA2_ = p_Main->getConfigBool("/Victory/DiagonalA2", true);
            simDefB2_ = p_Main->getConfigBool("/Victory/DiagonalB2", true);
            
            if (multiCart_)
			{
				wxString game;
                if (multiCartRom_[(0xa00 + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] < 32)
                    game = p_Main->getMultiCartGame(multiCartMsb_, multiCartLsb_);
                else
                    game = getMultiCartGame();
                if (gameAuto_)
					p_Main->loadKeyDefinition("", game, keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
				if (game.Find('.'))
					game = game.BeforeFirst('.');
				p_Main->setSwName(game);
                p_Main->eventUpdateTitle();
			}
			else
			{
				if (gameAuto_)
					p_Main->loadKeyDefinition("", p_Main->getRomFile(VICTORY, CARTROM), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
			}

            reDefineKeysA(keyDefA1_, keyDefA2_);
            reDefineKeysB(keyDefB1_, keyDefB2_);
            
			setWait(1);
			setClear(0);
			setWait(1);
			setClear(1);
			initPixie();
		}
		if (debugMode_)
			p_Main->cycleDebug();
		p_Main->cycleSt2Debug();
	}
	else
	{
		initPixie();
		cpuCycles_ = 0;
		p_Main->startTime();
	}
}

void Victory::onReset()
{
	resetPressed_ = true;
}

void Victory::checkFunction()
{
    if (!gameAuto_)
        return;

    bool buildInGameFound = false;
    if (chip8mainLoop_ == scratchpadRegister_[programCounter_])
    {
        if ((mainMemory_[0x40e] == 0xd1) && (mainMemory_[0x40f] == 0x1e) && (mainMemory_[0x410] == 0xd2) && (mainMemory_[0x411] == 0x4a))
        {
            switch(scratchpadRegister_[CHIP8_PC])
            {
                case 0x41e:
					p_Main->loadKeyDefinition("", "victorygrandpack1", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
					buildInGameFound = true;
                    buildInGame_ = 1;
                break;
                case 0x44a:
					p_Main->loadKeyDefinition("", "victorygrandpack2", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
					buildInGameFound = true;
                    buildInGame_ = 2;
                break;
                case 0x4e0:
					p_Main->loadKeyDefinition("", "victorygrandpack3", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
					buildInGameFound = true;
                    buildInGame_ = 3;
                break;
                case 0x41a:
					p_Main->loadKeyDefinition("", "victorygrandpack4", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
					buildInGameFound = true;
                    buildInGame_ = 4;
                break;
                case 0x41c:
					p_Main->loadKeyDefinition("", "victorygrandpack5", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
					buildInGameFound = true;
                    buildInGame_ = 5;
                break;
            }
        }
    }
    if (buildInGameFound)
    {
        reDefineKeysA(keyDefA1_, keyDefA2_);
        reDefineKeysB(keyDefB1_, keyDefB2_);
    }
}

void Victory::setMultiCartLsb(Byte lsb)
{
    multiCartLsb_ = lsb;
	resetPressed_ = true;
}

void Victory::setMultiCartMsb(Byte msb)
{
	multiCartMsb_ = msb;
	resetPressed_ = true;
}

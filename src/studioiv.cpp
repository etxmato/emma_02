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
#include "studioiv.h"

#define CHIP8_PC 5

StudioIV::StudioIV(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType)
:Pixie(title, pos, size, zoom, zoomfactor, computerType)
{
}

StudioIV::~StudioIV()
{
	p_Main->setMainPos(STUDIOIV, GetPosition());
}

void StudioIV::configureComputer()
{
//    chip8baseVar_ = 0x8c0;
//    chip8mainLoop_ = 0x6b;
//    chip8type_ = CHIPST2;
    
	outType_[2] = STUDIOOUT;
//	outType_[4] = VIPOUT4;
	victoryKeyPort_ = 0;
	efType_[3] = STUDIOEF3;
	efType_[4] = STUDIOEF4;

	for (int j=0; j<2; j++) for (int i=0; i<10; i++)
		victoryKeyState_[j][i] = 0;

	p_Main->message("Configuring Studio IV");
	p_Main->message("	Output 2: select port, EF 3: read selected port 1, EF4: read selected port 2\n");

	p_Main->getDefaultHexKeys(STUDIOIV, "StudioIV", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
	p_Main->getDefaultHexKeys(STUDIOIV, "StudioIV", "B", keyDefB1_, keyDefB2_, keyDefGameHexB_);

    gameAuto_ = p_Main->getConfigBool("/StudioIV/GameAuto", true);

	simDefA2_ = p_Main->getConfigBool("/StudioIV/DiagonalA2", true);
    simDefB2_ = p_Main->getConfigBool("/StudioIV/DiagonalB2", true);
    
	resetCpu();
}

void StudioIV::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
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

void StudioIV::reDefineKeysB(int hexKeyDefB1[], int hexKeyDefB2[])
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

void StudioIV::keyDown(int keycode)
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

void StudioIV::keyUp(int keycode)
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

Byte StudioIV::ef(int flag)
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

Byte StudioIV::ef3()
{
	if (victoryKeyPort_<0 || victoryKeyPort_>9)
		return 1;
	return(victoryKeyState_[0][victoryKeyPort_]) ? 0 : 1;
}

Byte StudioIV::ef4()
{
	if (victoryKeyPort_<0 || victoryKeyPort_>9)
		return 1;
	return(victoryKeyState_[1][victoryKeyPort_]) ? 0 : 1;
}

Byte StudioIV::in(Byte port, Word WXUNUSED(address))
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

void StudioIV::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;

	switch(outType_[port])
	{
		case 0:
			return;
		break;

		case STUDIOOUT:
			outStudioIV(value);
		break;

        case PIXIEOUT:
            inPixie();
        break;
            
//        case VIPOUT4:
//			tone1864Latch(value);
//		break;

		case PIXIEBACKGROUND:
			outPixieBackGround();
		break;
	}
}

void StudioIV::outStudioIV(Byte value)
{
	victoryKeyPort_ = value & 0xf;
}

void StudioIV::cycle(int type)
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

void StudioIV::startComputer()
{
	resetPressed_ = false;

	p_Main->setSwName("");

    readProgram(p_Main->getRomDir(STUDIOIV, MAINROM1), p_Main->getRomFile(STUDIOIV, MAINROM1), ROM, 0, NONAME);
    
    p_Main->assDefault("studioivcart_1", 0x800, 0xFFF);
    p_Main->assDefault("studioivcart_2", 0x1000, 0x17FF);

    defineMemoryType(0x2000, 0x3B00, RAM);
    initRam(0x2000, 0x3B00);
	defineMemoryType(0x2800, 0x2BFF, COLOURRAM);
    
	double zoom = p_Main->getZoom();

	configurePixieStudioIV();
	initPixie();
	setZoom(zoom);
	Show(true);
	setWait(1);
	setClear(0);
	setWait(1);
	setClear(1);

    if (gameAuto_)
        p_Main->loadKeyDefinition("", p_Main->getRomFile(STUDIOIV, CARTROM), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");

	reDefineKeysA(keyDefA1_, keyDefA2_);
	reDefineKeysB(keyDefB1_, keyDefB2_);

	p_Main->updateTitle();

	cpuCycles_ = 0;
	p_Main->startTime();

	threadPointer->Run();
}

void StudioIV::writeMemDataType(Word address, Byte type)
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

Byte StudioIV::readMemDataType(Word address)
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

Byte StudioIV::readMem(Word addr)
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
            addr = (addr & 0x3ff) | 0x400;
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

void StudioIV::writeMem(Word addr, Byte value, bool writeRom)
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

void StudioIV::cpuInstruction()
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
 
            p_Main->getDefaultHexKeys(STUDIOIV, "StudioIV", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
            p_Main->getDefaultHexKeys(STUDIOIV, "StudioIV", "B", keyDefB1_, keyDefB2_, keyDefGameHexB_);

            simDefA2_ = p_Main->getConfigBool("/StudioIV/DiagonalA2", true);
            simDefB2_ = p_Main->getConfigBool("/StudioIV/DiagonalB2", true);
            
            if (gameAuto_)
                p_Main->loadKeyDefinition("", p_Main->getRomFile(STUDIOIV, CARTROM), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");

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

void StudioIV::onReset()
{
	resetPressed_ = true;
}

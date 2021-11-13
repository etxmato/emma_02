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

/*
 *******************************************************************
 *** This software is copyright 2006 by Michael H Riley          ***
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
#include "visicom.h"

#define CHIP8_PC 5

Visicom::Visicom(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType)
:Pixie(title, pos, size, zoom, zoomfactor, computerType)
{
}

Visicom::~Visicom()
{
	p_Main->setMainPos(VISICOM, GetPosition());
}

void Visicom::configureComputer()
{
	outType_[2] = STUDIOOUT;
	studioKeyPort_ = 0;
	efType_[3] = STUDIOEF3;
	efType_[4] = STUDIOEF4;

	for (int j=0; j<2; j++) for (int i=0; i<10; i++)
		studioKeyState_[j][i] = 0;

	p_Main->message("Configuring Visicom COM-100");
	p_Main->message("	Output 2: select port, EF 3: read selected port 1, EF4: read selected port 2\n");

	p_Main->getDefaultHexKeys(VISICOM, "Visicom", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
	p_Main->getDefaultHexKeys(VISICOM, "Visicom", "B", keyDefB1_, keyDefB2_, keyDefGameHexB_);

    gameAuto_ = p_Main->getConfigBool("/Visicom/GameAuto", true);

    simDefA2_ = p_Main->getConfigBool("/Visicom/DiagonalA2", true);
    simDefB2_ = p_Main->getConfigBool("/Visicom/DiagonalB2", true);
    
	if (gameAuto_)
		p_Main->loadKeyDefinition("", p_Main->getRomFile(VISICOM, CARTROM), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");

    reDefineKeysA(keyDefA1_, keyDefA2_);
    reDefineKeysB(keyDefB1_, keyDefB2_);

	resetCpu();
}

void Visicom::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
	for (int i = 0; i<512; i++)
	{
		keyDefinition[i].defined = false;
	}
	for (int i = 0; i<10; i++)
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

void Visicom::reDefineKeysB(int hexKeyDefB1[], int hexKeyDefB2[])
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

void Visicom::keyDown(int keycode)
{
	if (keyDefinition[keycode].defined)
	{
		if (simDefA2_)
		{
			if (keycode == keyDefA2_[2])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[4]) == true)
				{
					studioKeyState_[0][keyDefinition[keyDefA2_[2]].key - 1] = 1;
					studioKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
					studioKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[6]) == true)
				{
					studioKeyState_[0][keyDefinition[keyDefA2_[2]].key + 1] = 1;
					studioKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
					studioKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
					return;
				}
			}
			if (keycode == keyDefA2_[4])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[2]) == true)
				{
					studioKeyState_[0][keyDefinition[keyDefA2_[2]].key - 1] = 1;
					studioKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
					studioKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[8]) == true)
				{
					studioKeyState_[0][keyDefinition[keyDefA2_[8]].key - 1] = 1;
					studioKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
					studioKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
					return;
				}
			}
			if (keycode == keyDefA2_[6])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[2]) == true)
				{
					studioKeyState_[0][keyDefinition[keyDefA2_[2]].key + 1] = 1;
					studioKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
					studioKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[8]) == true)
				{
					studioKeyState_[0][keyDefinition[keyDefA2_[8]].key + 1] = 1;
					studioKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
					studioKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
					return;
				}
			}
			if (keycode == keyDefA2_[8])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[4]) == true)
				{
					studioKeyState_[0][keyDefinition[keyDefA2_[8]].key - 1] = 1;
					studioKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
					studioKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefA2_[6]) == true)
				{
					studioKeyState_[0][keyDefinition[keyDefA2_[8]].key + 1] = 1;
					studioKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
					studioKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
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
					studioKeyState_[1][keyDefinition[keyDefB2_[2]].key - 1] = 1;
					studioKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
					studioKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[6]) == true)
				{
					studioKeyState_[1][keyDefinition[keyDefB2_[2]].key + 1] = 1;
					studioKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
					studioKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
					return;
				}
			}
			if (keycode == keyDefB2_[4])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[2]) == true)
				{
					studioKeyState_[1][keyDefinition[keyDefB2_[2]].key - 1] = 1;
					studioKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
					studioKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[8]) == true)
				{
					studioKeyState_[1][keyDefinition[keyDefB2_[8]].key - 1] = 1;
					studioKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
					studioKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
					return;
				}
			}
			if (keycode == keyDefB2_[6])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[2]) == true)
				{
					studioKeyState_[1][keyDefinition[keyDefB2_[2]].key + 1] = 1;
					studioKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
					studioKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[8]) == true)
				{
					studioKeyState_[1][keyDefinition[keyDefB2_[8]].key + 1] = 1;
					studioKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
					studioKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
					return;
				}
			}
			if (keycode == keyDefB2_[8])
			{
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[4]) == true)
				{
					studioKeyState_[1][keyDefinition[keyDefB2_[8]].key - 1] = 1;
					studioKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
					studioKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
					return;
				}
				if (::wxGetKeyState((wxKeyCode)keyDefB2_[6]) == true)
				{
					studioKeyState_[1][keyDefinition[keyDefB2_[8]].key + 1] = 1;
					studioKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
					studioKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
					return;
				}
			}
		}
		studioKeyState_[keyDefinition[keycode].player][keyDefinition[keycode].key] = 1;
	}
}

void Visicom::keyUp(int keycode)
{
	if (simDefA2_)
	{
        if (keycode == keyDefA2_[2] || keycode == keyDefA2_[4] || keycode == keyDefA2_[6] || keycode == keyDefA2_[8])
        {
            studioKeyState_[0][keyDefinition[keyDefA2_[2]].key-1] = 0;
            studioKeyState_[0][keyDefinition[keyDefA2_[2]].key+1] = 0;
            studioKeyState_[0][keyDefinition[keyDefA2_[8]].key-1] = 0;
            studioKeyState_[0][keyDefinition[keyDefA2_[8]].key+1] = 0;
            if (::wxGetKeyState((wxKeyCode)keyDefA2_[2]) == true)
                studioKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 1;
            if (::wxGetKeyState((wxKeyCode)keyDefA2_[4]) == true)
                studioKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 1;
            if (::wxGetKeyState((wxKeyCode)keyDefA2_[6]) == true)
                studioKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 1;
            if (::wxGetKeyState((wxKeyCode)keyDefA2_[8]) == true)
                studioKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 1;
        }
	}
	if (simDefB2_)
	{
		if (keycode == keyDefB2_[2] || keycode == keyDefB2_[4] || keycode == keyDefB2_[6] || keycode == keyDefB2_[8])
		{
			studioKeyState_[1][keyDefinition[keyDefB2_[2]].key - 1] = 0;
			studioKeyState_[1][keyDefinition[keyDefB2_[2]].key + 1] = 0;
			studioKeyState_[1][keyDefinition[keyDefB2_[8]].key - 1] = 0;
			studioKeyState_[1][keyDefinition[keyDefB2_[8]].key + 1] = 0;
			if (::wxGetKeyState((wxKeyCode)keyDefB2_[2]) == true)
				studioKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 1;
			if (::wxGetKeyState((wxKeyCode)keyDefB2_[4]) == true)
				studioKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 1;
			if (::wxGetKeyState((wxKeyCode)keyDefB2_[6]) == true)
				studioKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 1;
			if (::wxGetKeyState((wxKeyCode)keyDefB2_[8]) == true)
				studioKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 1;
		}
	}
	if (keyDefinition[keycode].defined)
		studioKeyState_[keyDefinition[keycode].player][keyDefinition[keycode].key] = 0;
}

Byte Visicom::ef(int flag)
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

Byte Visicom::ef3()
{
	if (studioKeyPort_<0 || studioKeyPort_>9)
		return 1;
    
    Byte returnValue = (studioKeyState_[0][studioKeyPort_]) ? 0 : 1;
    
    return returnValue;
}

Byte Visicom::ef4()
{
	if (studioKeyPort_<0 || studioKeyPort_>9)
		return 1;
    
    Byte returnValue = (studioKeyState_[1][studioKeyPort_]) ? 0 : 1;
    
    return returnValue;
}

Byte Visicom::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Visicom::out(Byte port, Word WXUNUSED(address), Byte value)
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

		case STUDIOOUT:
			outStudio(value);
		break;

		case VIPOUT5:
			outPixieBackGround();
		break;
	}
}

void Visicom::outStudio(Byte value)
{
//	while(value >= 0x10) value -= 0x10;
	studioKeyPort_ = value & 0xf;
}

void Visicom::cycle(int type)
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

void Visicom::startComputer()
{
	resetPressed_ = false;

	p_Main->setSwName("");

	for (int i=0x800; i<0xff00; i+=0x400)
	{
		defineMemoryType(i, MAPPEDRAM);
		defineMemoryType(i+0x100, MAPPEDRAM);
		defineMemoryType(i+0x300, MAPPEDRAM);
	}
    p_Main->checkAndReInstallMainRom(VISICOM);
	readProgram(p_Main->getRomDir(VISICOM, MAINROM1), p_Main->getRomFile(VISICOM, MAINROM1), ROM, 0, NONAME);
	readSt2Program(VISICOM, CARTRIDGEROM);
	defineMemoryType(0x800, 0xfff, CARTRIDGEROM);
    p_Main->assDefault("visicomcart", 0x800, 0xFFF);
	defineMemoryType(0x1000, 0x11ff, RAM);
    initRam(0x1000, 0x11ff);
	defineMemoryType(0x1300, 0x13ff, RAM);
    initRam(0x1300, 0x13ff);
	double zoom = p_Main->getZoom();
 
    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);
    
	configurePixieVisicom();
	initPixie();
	setZoom(zoom);
	Show(true);
	setWait(1);
	setClear(0);
	setWait(1);
	setClear(1);

	p_Main->updateTitle();

	cpuCycles_ = 0;
	instructionCounter_= 0;
	p_Main->startTime();

	threadPointer->Run();
}

void Visicom::writeMemDataType(Word address, Byte type)
{
	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
		case CARTRIDGEROM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
            increaseExecutedMainMemory(address, type);
		break;

		case MAPPEDRAM:
			address = (address & 0x1ff) | 0x800;
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
            increaseExecutedMainMemory(address, type);
		break;
	}
}

Byte Visicom::readMemDataType(Word address, uint64_t* executed)
{
	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
			return mainMemoryDataType_[address];
		break;

		case CARTRIDGEROM:
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address&0xfff];
			return mainMemoryDataType_[address&0xfff];
		break;

		case MAPPEDRAM:
			address = (address & 0x1ff) | 0x800;
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Visicom::readMem(Word address)
{
	switch (memoryType_[address/256])
	{
		case UNDEFINED:
			return 255;
		break;

		case CARTRIDGEROM:
			address = address&0xfff;
		break;

//		case CARTRIDGEROM:
//			address = (address & 0x3ff) | 0x400;
//		break;

		case MAPPEDRAM:
			address = (address & 0x3ff) | 0x1000;
		break;
	}

	return mainMemory_[address];
}

Byte Visicom::readMemDebug(Word address)
{
    return readMem(address);
}

void Visicom::writeMem(Word address, Byte value, bool writeRom)
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
			address = (address & 0x3ff) | 0x1000;
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

void Visicom::writeMemDebug(Word address, Byte value, bool writeRom)
{
    writeMem(address, value, writeRom);
}

void Visicom::cpuInstruction()
{
	if (cpuMode_ == RUN)
	{
        cpuCycleStep();
	}
	else
	{
		initPixie();
		cpuCycles_ = 0;
		instructionCounter_= 0;
		p_Main->startTime();
	}
}

void Visicom::resetPressed()
{
    resetCpu();
    resetPressed_ = false;
    
    p_Main->getDefaultHexKeys(VISICOM, "Visicom", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
    p_Main->getDefaultHexKeys(VISICOM, "Visicom", "B", keyDefB1_, keyDefB2_, keyDefGameHexB_);
    
    simDefA2_ = p_Main->getConfigBool("/Visicom/DiagonalA2", true);
    simDefB2_ = p_Main->getConfigBool("/Visicom/DiagonalB2", true);
    
    if (gameAuto_)
        p_Main->loadKeyDefinition("", p_Main->getRomFile(VISICOM, CARTROM), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
    
    reDefineKeysA(keyDefA1_, keyDefA2_);
    reDefineKeysB(keyDefB1_, keyDefB2_);
    
    setWait(1);
    setClear(0);
    setWait(1);
    setClear(1);
    initPixie();
}

void Visicom::onReset()
{
	resetPressed_ = true;
}

void Visicom::checkComputerFunction()
{
    if (!gameAuto_)
        return;
        
    bool buildInGameFound = false;
    if (chip8mainLoop_ == scratchpadRegister_[programCounter_])
    {
        switch(scratchpadRegister_[CHIP8_PC])
        {
            case 0x402:
				p_Main->loadKeyDefinition("", "visicombuildin1", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
				buildInGameFound = true;
                buildInGame_ = 1;
                break;
            case 0x603:
				p_Main->loadKeyDefinition("", "visicombuildin2", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
				buildInGameFound = true;
                buildInGame_ = 2;
                break;
            case 0x40c:
				p_Main->loadKeyDefinition("", "visicombuildin3", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
				buildInGameFound = true;
                buildInGame_ = 3;
                break;
            case 0x51d:
				p_Main->loadKeyDefinition("", "visicombuildin4", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
				buildInGameFound = true;
                buildInGame_ = 4;
                break;
            case 0x4ad:
				p_Main->loadKeyDefinition("", "visicombuildin7", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
				buildInGameFound = true;
                buildInGame_ = 7;
                break;
        }
    }
    if (buildInGameFound)
    {
        reDefineKeysA(keyDefA1_, keyDefA2_);
        reDefineKeysB(keyDefB1_, keyDefB2_);
	}
}

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
#include "studio2.h"

#define CHIP8_PC 5

Studio2::Studio2(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType)
:Pixie(title, pos, size, zoom, zoomfactor, computerType)
{
}

Studio2::~Studio2()
{
	p_Main->setMainPos(STUDIO, GetPosition());
}

void Studio2::configureComputer()
{
    buildInGame_ = 0;

    outType_[2] = STUDIOOUT;
	studioKeyPort_ = 0;
	efType_[3] = STUDIOEF3;
	efType_[4] = STUDIOEF4;

	for (int j=0; j<2; j++) for (int i=0; i<10; i++)
		studioKeyState_[j][i] = 0;

	p_Main->message("Configuring Studio II");
	p_Main->message("	Output 2: select port, EF 3: read selected port 1, EF4: read selected port 2\n");

    p_Main->getDefaultHexKeys(STUDIO, "Studio2", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
    p_Main->getDefaultHexKeys(STUDIO, "Studio2", "B", keyDefB1_, keyDefB2_, keyDefGameHexB_);

    gameAuto_ = p_Main->getConfigBool("/Studio2/GameAuto", true);
    
    simDefA2_ = p_Main->getConfigBool("/Studio2/DiagonalA2", true);
    simDefB2_ = p_Main->getConfigBool("/Studio2/DiagonalB2", true);

	multiCart_ = p_Main->getUseMultiCart(STUDIO);
	disableSystemRom_ = p_Main->getDisableSystemRom(STUDIO);
	multiCartLsb_ = p_Main->getMultiCartLsb(STUDIO);
	multiCartMsb_ = p_Main->getMultiCartMsb(STUDIO);

	resetCpu();
}

void Studio2::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
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

void Studio2::reDefineKeysB(int hexKeyDefB1[], int hexKeyDefB2[])
{
	for (int i=0; i<10; i++)
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

void Studio2::keyDown(int keycode)
{
	if (keyDefinition[keycode].defined)
    {
        if (simDefA2_)
        {
            if (keycode == keyDefA2_[2])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[4]) == true)
                {
                    studioKeyState_[0][keyDefinition[keyDefA2_[2]].key-1] = 1;
                    studioKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
                    studioKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[6]) == true)
                {
                    studioKeyState_[0][keyDefinition[keyDefA2_[2]].key+1] = 1;
                    studioKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
                    studioKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
                    return;
                }
            }
            if (keycode == keyDefA2_[4])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[2]) == true)
                {
                    studioKeyState_[0][keyDefinition[keyDefA2_[2]].key-1] = 1;
                    studioKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
                    studioKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[8]) == true)
                {
                    studioKeyState_[0][keyDefinition[keyDefA2_[8]].key-1] = 1;
                    studioKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
                    studioKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
                    return;
                }
            }
            if (keycode == keyDefA2_[6])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[2]) == true)
                {
                    studioKeyState_[0][keyDefinition[keyDefA2_[2]].key+1] = 1;
                    studioKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
                    studioKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[8]) == true)
                {
                    studioKeyState_[0][keyDefinition[keyDefA2_[8]].key+1] = 1;
                    studioKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
                    studioKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
                    return;
                }
            }
            if (keycode == keyDefA2_[8])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[4]) == true)
                {
                    studioKeyState_[0][keyDefinition[keyDefA2_[8]].key-1] = 1;
                    studioKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
                    studioKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[6]) == true)
                {
                    studioKeyState_[0][keyDefinition[keyDefA2_[8]].key+1] = 1;
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
                    studioKeyState_[1][keyDefinition[keyDefB2_[2]].key-1] = 1;
                    studioKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
                    studioKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[6]) == true)
                {
                    studioKeyState_[1][keyDefinition[keyDefB2_[2]].key+1] = 1;
                    studioKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
                    studioKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
                    return;
                }
            }
            if (keycode == keyDefB2_[4])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[2]) == true)
                {
                    studioKeyState_[1][keyDefinition[keyDefB2_[2]].key-1] = 1;
                    studioKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
                    studioKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[8]) == true)
                {
                    studioKeyState_[1][keyDefinition[keyDefB2_[8]].key-1] = 1;
                    studioKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
                    studioKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
                    return;
                }
            }
            if (keycode == keyDefB2_[6])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[2]) == true)
                {
                    studioKeyState_[1][keyDefinition[keyDefB2_[2]].key+1] = 1;
                    studioKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
                    studioKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[8]) == true)
                {
                    studioKeyState_[1][keyDefinition[keyDefB2_[8]].key+1] = 1;
                    studioKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
                    studioKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
                    return;
                }
            }
            if (keycode == keyDefB2_[8])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[4]) == true)
                {
                    studioKeyState_[1][keyDefinition[keyDefB2_[8]].key-1] = 1;
                    studioKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
                    studioKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[6]) == true)
                {
                    studioKeyState_[1][keyDefinition[keyDefB2_[8]].key+1] = 1;
                    studioKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
                    studioKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
                    return;
                }
            }
        }
        studioKeyState_[keyDefinition[keycode].player][keyDefinition[keycode].key] = 1;
    }
}

void Studio2::keyUp(int keycode)
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
            studioKeyState_[1][keyDefinition[keyDefB2_[2]].key-1] = 0;
            studioKeyState_[1][keyDefinition[keyDefB2_[2]].key+1] = 0;
            studioKeyState_[1][keyDefinition[keyDefB2_[8]].key-1] = 0;
            studioKeyState_[1][keyDefinition[keyDefB2_[8]].key+1] = 0;
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

Byte Studio2::ef(int flag)
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

Byte Studio2::ef3()
{
	if (studioKeyPort_<0 || studioKeyPort_>9)
		return 1;
	return(studioKeyState_[0][studioKeyPort_]) ? 0 : 1;
}

Byte Studio2::ef4()
{
	if (studioKeyPort_<0 || studioKeyPort_>9)
		return 1;
	return(studioKeyState_[1][studioKeyPort_]) ? 0 : 1;
}

void Studio2::switchQ(int value)
{
    if (value == 0)
    {
    }
    else
    {
    }
}

Byte Studio2::in(Byte port, Word WXUNUSED(address))
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

void Studio2::out(Byte port, Word WXUNUSED(address), Byte value)
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

		case STUDIOOUT:
			outStudio(value);
		break;
	}
}

void Studio2::outStudio(Byte value)
{
//	while(value >= 0x10) value -= 0x10;
	studioKeyPort_ = value & 0xf;
}

void Studio2::cycle(int type)
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

void Studio2::startComputer()
{
	resetPressed_ = false;

	p_Main->setSwName("");

    for (int i=0x800; i<0xff00; i+=0x400)
	{
        defineMemoryType(i, MAPPEDRAM);
        defineMemoryType(i+0x100, MAPPEDRAM);
	}

    p_Main->checkAndReInstallMainRom(STUDIO);
    readProgram(p_Main->getRomDir(STUDIO, MAINROM1), p_Main->getRomFile(STUDIO, MAINROM1), ROM, 0, NONAME);
    
    if (multiCart_)
    {
        if (readMultiCartBinFile(p_Main->getRomDir(STUDIO, CARTROM), p_Main->getRomFile(STUDIO, CARTROM)))
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
    
    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);

    if (pseudoType_ == "ST2")
    {
        if (!multiCart_)
        {
            readSt2Program(STUDIO);
            p_Main->assDefault("studiocart", 0x400, 0x7FF);
        }
        else
        {
            p_Main->assDefault("studiocart_1", 0x400, 0x7FF);
            p_Main->assDefault("studiocart_2", 0xC00, 0xFFF);
        }
    }
    else
    {
        if (!multiCart_)
            readProgram(p_Main->getRomDir(STUDIO, CARTROM), p_Main->getRomFile(STUDIO, CARTROM), ROM, 0x300, NONAME);
        p_Main->assDefault("studiocart_1", 0x400, 0x7FF);
        p_Main->assDefault("studiocart_2", 0xC00, 0xFFF);
	}

    defineMemoryType(0x800, 0x9ff, RAM);
    initRam(0x800, 0x9ff);

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

	configurePixieStudio2();
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
			p_Main->loadKeyDefinition("", p_Main->getRomFile(STUDIO, CARTROM), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
	}

	reDefineKeysA(keyDefA1_, keyDefA2_);
	reDefineKeysB(keyDefB1_, keyDefB2_);

	p_Main->updateTitle();

	cpuCycles_ = 0;
	p_Main->startTime();

	threadPointer->Run();
}

void Studio2::writeMemDataType(Word address, Byte type)
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
				mainMemoryDataType_[address] = type;
			}
		break;
	}
}

Byte Studio2::readMemDataType(Word address)
{
    switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
        case MAPPEDROM:
		case CARTRIDGEROM:
			return mainMemoryDataType_[address];
		break;

        case MULTICART:
			if ((address < 0x400) && !disableSystemRom_)
				return mainMemoryDataType_[address];
			else
				return multiCartRomDataType_[(address+multiCartLsb_*0x1000+multiCartMsb_*0x10000)&multiCartMask_];
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

Byte Studio2::readMem(Word address)
{
	switch (memoryType_[address/256])
	{
		case UNDEFINED:
			return 255;
		break;

        case MULTICART:
			if ((address < 0x400) && !disableSystemRom_)
				return mainMemory_[address];
			else
				return multiCartRom_[(address+multiCartLsb_*0x1000+multiCartMsb_*0x10000)&multiCartMask_];
        break;
            
        case MAPPEDMULTICART:
            address = address & 0xfff;
            if ((address < 0x400) && !disableSystemRom_)
                return mainMemory_[address];
            else
                return multiCartRom_[(address+multiCartLsb_*0x1000+multiCartMsb_*0x10000)&multiCartMask_];
        break;
            
        case MAPPEDRAM:
			address = (address & 0x1ff) | 0x800;
		break;
 
        case CARTRIDGEROM:
//            address = (address & 0x3ff) | 0x400;
        break;
            
        case MAPPEDROM:
            address = (address & 0x3ff);
        break;
	}
    return mainMemory_[address];
}

Byte Studio2::readMemDebug(Word address)
{
    return readMem(address);
}

void Studio2::writeMem(Word address, Byte value, bool writeRom)
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

        case MULTICART:
			if (writeRom)
			{
				if ((address < 0x400) && !disableSystemRom_)
					mainMemory_[address] = value;
				else
					multiCartRom_[(address + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] = value;
			}
        break;
            
        case MAPPEDMULTICART:
            address = address & 0xfff;
            if (writeRom)
            {
                if ((address < 0x400) && !disableSystemRom_)
                    mainMemory_[address] = value;
                else
                    multiCartRom_[(address + multiCartLsb_ * 0x1000 + multiCartMsb_ * 0x10000)&multiCartMask_] = value;
            }
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
//			else
//			{
//				if (address >= 0x400 && address < 0x800)
//				{
//					p_Main->eventShowMessage(scratchpadRegister_[5]);
//				}
//			}
		break;
	}
}

void Studio2::writeMemDebug(Word address, Byte value, bool writeRom)
{
    writeMem(address, value, writeRom);
}

void Studio2::cpuInstruction()
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

void Studio2::resetPressed()
{
    resetCpu();
    resetPressed_ = false;
    
    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);
    
    p_Main->getDefaultHexKeys(STUDIO, "Studio2", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
    p_Main->getDefaultHexKeys(STUDIO, "Studio2", "B", keyDefB1_, keyDefB2_, keyDefGameHexB_);
    
    simDefA2_ = p_Main->getConfigBool("/Studio2/DiagonalA2", true);
    simDefB2_ = p_Main->getConfigBool("/Studio2/DiagonalB2", true);
    
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
            p_Main->loadKeyDefinition("", p_Main->getRomFile(STUDIO, CARTROM), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
    }
    
    reDefineKeysA(keyDefA1_, keyDefA2_);
    reDefineKeysB(keyDefB1_, keyDefB2_);
    
    setWait(1);
    setClear(0);
    setWait(1);
    setClear(1);
    initPixie();
}

void Studio2::onReset()
{
	resetPressed_ = true;
}

void Studio2::checkComputerFunction()
{
    if (!gameAuto_)
        return;

    bool buildInGameFound = false;
    if (chip8mainLoop_ == scratchpadRegister_[programCounter_])
    {
        if ((mainMemory_[0x402] == 0xd1) && (mainMemory_[0x403] == 0x0e) && (mainMemory_[0x404] == 0xd2) && (mainMemory_[0x405] == 0x39))
        {
            switch(scratchpadRegister_[CHIP8_PC])
            {
                case 0x40e:
					p_Main->loadKeyDefinition("", "studio2buildin1", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
                    buildInGameFound = true;
                    buildInGame_ = 1;
                break;
                case 0x439:
					p_Main->loadKeyDefinition("", "studio2buildin2", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
                    buildInGameFound = true;
                    buildInGame_ = 2;
                break;
                case 0x48b:
					p_Main->loadKeyDefinition("", "studio2buildin3", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
                    buildInGameFound = true;
                    buildInGame_ = 3;
                break;
                case 0x48d:
					p_Main->loadKeyDefinition("", "studio2buildin4", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
                    buildInGameFound = true;
                    buildInGame_ = 4;
                break;
                case 0x48f:
					p_Main->loadKeyDefinition("", "studio2buildin5", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
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

void Studio2::setMultiCartLsb(Byte lsb)
{
    multiCartLsb_ = lsb;
    resetPressed_ = true;
}

void Studio2::setMultiCartMsb(Byte msb)
{
    multiCartMsb_ = msb;
    resetPressed_ = true;
}

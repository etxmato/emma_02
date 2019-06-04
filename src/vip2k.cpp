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
#include "vip2k.h"

Vip2K::Vip2K(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, double clock, ElfConfiguration conf)
:PixieVip2K(title, pos, size, zoom, zoomfactor, computerType)
{
	vipConfiguration = conf;
	clock_ = clock;
}

Vip2K::~Vip2K()
{
    switch (loadedProgram_)
    {
        case NOPROGRAM:
        case FPBBOOT:
            p_Main->saveScrtValues("");
            break;
            
        case FPBBASIC:
            p_Main->saveScrtValues("FPBBASIC");
            break;
    }
	if (vipConfiguration.vtType != VTNONE)
	{
		p_Main->setVtPos(VIP2K, vtPointer->GetPosition());
		vtPointer->Destroy();
	}
    if (vipConfiguration.vtExternal)
        delete p_Serial;
	p_Main->setMainPos(VIP2K, GetPosition());
}

void Vip2K::configureComputer()
{
    inType_[1] = VIP2KCOL1;
    inType_[2] = VIP2KCOL2;
    inType_[3] = VIP2KCOL3;
    inType_[4] = VIP2KCOL4;
    inType_[5] = VIP2KCOL5;
	efType_[2] = VIP2KEF2;
	efType_[3] = VIP2KEF3;

	p_Main->message("Configuring VIP2K Membership Card");
    
    p_Main->message("	Input 1-5: keyboard input keycol 1-5");
    p_Main->message("	EF 2: CTL, EF 3: SHIFT\n");

	if (vipConfiguration.vtType != VTNONE)
	{
		double zoom = p_Main->getZoomVt();
        if (vipConfiguration.vtType == VT52)
            vtPointer = new Vt100("Cosmac Vip 2K - VT 52", p_Main->getVtPos(VIP2K), wxSize(640*zoom, 400*zoom), zoom, VIP2K, clock_, vipConfiguration);
        else
            vtPointer = new Vt100("Cosmac Vip 2K - VT 100", p_Main->getVtPos(VIP2K), wxSize(640*zoom, 400*zoom), zoom, VIP2K, clock_, vipConfiguration);
		p_Vt100 = vtPointer;
        
        vtPointer->configureStandard(vipConfiguration.baudR, vipConfiguration.baudT, 4);
		vtPointer->Show(vipConfiguration.vtShow);
	}

    if (vipConfiguration.vtExternal)
    {
        p_Serial = new Serial(VIP2K, clock_, vipConfiguration);
        p_Serial->configureStandard(vipConfiguration.baudR, vipConfiguration.baudT, 4);
    }
    
	resetCpu();
}

void Vip2K::initComputer()
{
	setClear(1);
	setWait(1);
	cassetteEf_ = 0;

	for (int i=1; i<6; i++)
		vipKeyState_[i] = 0xff;

    shiftEf_ = 1;
    ctlEf_ = 1;

	runPressed_ = false;
	stateQ_ = 0;

	vipRunCommand_ = 0;
	vipRunState_ = RESETSTATE;
}

void Vip2K::keyDown(int keycode)
{
    switch (keycode)
    {
        case '1':
            vipKeyState_[5] &= 0xfe;
        break;
            
        case '2':
            vipKeyState_[4] &= 0xfe;
        break;

        case '3':
            vipKeyState_[3] &= 0xfe;
        break;

        case '4':
            vipKeyState_[2] &= 0xfe;
        break;

        case '5':
            vipKeyState_[1] &= 0xfe;
        break;
            
        case 'Q':
            vipKeyState_[5] &= 0xfd;
        break;
            
        case 'W':
            vipKeyState_[4] &= 0xfd;
        break;
            
        case 'E':
            vipKeyState_[3] &= 0xfd;
        break;
            
        case 'R':
            vipKeyState_[2] &= 0xfd;
        break;
            
        case 'T':
            vipKeyState_[1] &= 0xfd;
        break;

        case 'A':
            vipKeyState_[5] &= 0xfb;
        break;
            
        case 'S':
            vipKeyState_[4] &= 0xfb;
        break;
            
        case 'D':
            vipKeyState_[3] &= 0xfb;
        break;
            
        case 'F':
            vipKeyState_[2] &= 0xfb;
        break;
            
        case 'G':
            vipKeyState_[1] &= 0xfb;
        break;
            
        case 'Z':
            vipKeyState_[5] &= 0xf7;
        break;
            
        case 'X':
            vipKeyState_[4] &= 0xf7;
        break;
            
        case 'C':
            vipKeyState_[3] &= 0xf7;
        break;
            
        case 'V':
            vipKeyState_[2] &= 0xf7;
        break;
            
        case 'B':
            vipKeyState_[1] &= 0xf7;
        break;
            
        case '6':
            vipKeyState_[1] &= 0x7f;
        break;
            
        case '7':
            vipKeyState_[2] &= 0x7f;
        break;
            
        case '8':
            vipKeyState_[3] &= 0x7f;
        break;
            
        case '9':
            vipKeyState_[4] &= 0x7f;
        break;
            
        case '0':
            vipKeyState_[5] &= 0x7f;
        break;
            
        case 'Y':
            vipKeyState_[1] &= 0xbf;
        break;
            
        case 'U':
            vipKeyState_[2] &= 0xbf;
        break;
            
        case WXK_UP:
        case 'I':
            vipKeyState_[3] &= 0xbf;
        break;
            
        case 'O':
            vipKeyState_[4] &= 0xbf;
        break;
            
        case 'P':
            vipKeyState_[5] &= 0xbf;
        break;

        case 'H':
            vipKeyState_[1] &= 0xdf;
        break;
            
        case WXK_LEFT:
        case 'J':
            vipKeyState_[2] &= 0xdf;
        break;
            
        case WXK_RIGHT:
        case 'K':
            vipKeyState_[3] &= 0xdf;
        break;
            
        case 'L':
            vipKeyState_[4] &= 0xdf;
        break;
            
        case WXK_RETURN:
            vipKeyState_[5] &= 0xdf;
        break;
  
        case 'N':
            vipKeyState_[1] &= 0xef;
        break;
            
        case WXK_DOWN:
        case 'M':
            vipKeyState_[2] &= 0xef;
        break;
            
        case '.':
            vipKeyState_[3] &= 0xef;
        break;
            
        case WXK_SPACE:
            vipKeyState_[4] &= 0xef;
        break;
            
        case WXK_BACK:
            vipKeyState_[5] &= 0xef;
        break;
         
        case WXK_SHIFT:
            shiftEf_ = 0;
        break;

        case WXK_RAW_CONTROL:
            ctlEf_ = 0;
        break;
    }
}

void Vip2K::keyUp(int keycode)
{
    for (int i=1; i<6; i++)
        vipKeyState_[i] = 0xff;
    switch (keycode)
    {
        case WXK_SHIFT:
            shiftEf_ = 1;
        break;
            
        case WXK_RAW_CONTROL:
            ctlEf_ = 1;
        break;
    }
}

void Vip2K::onRun()
{
	runPressed_ = true;
}

Byte Vip2K::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

        case PIXIEEF:
            return efPixie();
        break;
            
		case VIP2KEF2: // CTL
			return ctlEf_;
		break;

		case VIP2KEF3: // SHIFT
			return shiftEf_;
		break;

		case VT100EF:
			return vtPointer->ef();
		break;

        case VTSERIALEF:
            return p_Serial->ef();
        break;
 
		default:
			return 1;
	}
}

Byte Vip2K::in(Byte port, Word WXUNUSED(address))
{
	Byte ret=255;
    Word address;

	switch(inType_[port])
	{
        case VIP2KCOL1:
            ret = vipKeyState_[1];
            if ((scratchpadRegister_[3] & 0xf000) == 0)
                vipKeyState_[1] = 0xff;
        break;
            
        case VIP2KCOL2:
            ret = vipKeyState_[2];
            if ((scratchpadRegister_[3] & 0xf000) == 0)
                vipKeyState_[2] = 0xff;
        break;
            
        case VIP2KCOL3:
            ret = vipKeyState_[3];
            if ((scratchpadRegister_[3] & 0xf000) == 0)
                vipKeyState_[3] = 0xff;
        break;
    
        case VIP2KCOL4:
            ret = vipKeyState_[4];
            if ((scratchpadRegister_[3] & 0xf000) == 0)
                vipKeyState_[4] = 0xff;
       break;
            
        case VIP2KCOL5:
            ret = vipKeyState_[5];
            if ((scratchpadRegister_[3] & 0xf000) == 0)
                vipKeyState_[5] = 0xff;
        break;
            
		case PIXIEIN:
            p_Main->stopAutoTerminal();
            ret = inPixie();
		break;

        case PIXIEOUT:
            outPixie();
            if (programCounter_ == 3)
            {
                if (mainMemory_[scratchpadRegister_[7]+0xb] == 0x4c && mainMemory_[scratchpadRegister_[7]+0xc] == 0x4f && mainMemory_[scratchpadRegister_[7]+0xd] == 0x41 && mainMemory_[scratchpadRegister_[7]+0xe] == 0x44)
                    p_Main->startAutoTerminalLoad(false);
                if (mainMemory_[scratchpadRegister_[7]+0xb] == 0x53 && mainMemory_[scratchpadRegister_[7]+0xc] == 0x41 && mainMemory_[scratchpadRegister_[7]+0xd] == 0x56 && mainMemory_[scratchpadRegister_[7]+0xe] == 0x45)
                    p_Main->startAutoTerminalSave();
             
                address = scratchpadRegister_[programCounter_];
                    
                while (mainMemory_[address] != 0xD4)
                    address--;
                    
                if (mainMemory_[address+0xe] == 0x4c && mainMemory_[address+0xf] == 0x4f && mainMemory_[address+0x10] == 0x41 && mainMemory_[address+0x11] == 0x44)
                    p_Main->startAutoTerminalLoad(true);
                    
                if (mainMemory_[address+0xe] == 0x53 && mainMemory_[address+0xf] == 0x41 && mainMemory_[address+0x10] == 0x56 && mainMemory_[address+0x11] == 0x45)
                    p_Main->startAutoTerminalSave();
            }
        break;
            
		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Vip2K::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;

	switch(outType_[port])
	{
		case 0:
			return;
		break;

        case PIXIEIN:
            inPixie();
        break;
            
        case PIXIEOUT:
            outPixie();
        break;
            
		case VT100OUT:
			vtPointer->out(value);
		break;

		case VTOUTSERIAL:
			p_Serial->out(value);
		break;
	}
}

void Vip2K::switchQ(int value)
{
    if (vipConfiguration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (vipConfiguration.vtExternal)
        p_Serial->switchQ(value);
}

void Vip2K::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

		case PIXIECYCLE:
			cyclePixie();
		break;

        case VT100CYCLE:
            vtPointer->cycleVt();
        break;
            
        case VTSERIALCYCLE:
            p_Serial->cycleVt();
        break;
    }
}

void Vip2K::startComputer()
{
	resetPressed_ = false;

	p_Main->setSwName("");

    if (readProgram(p_Main->getRomDir(VIP2K, MAINROM2), p_Main->getRomFile(VIP2K, MAINROM2), ROM, 0, NONAME))
	{
		for (int i=0; i<2048; i++)
    		sequencerMemory_[i] = mainMemory_[i];
	}
	else
	{
		for (int i=0; i<2048; i+=8)
		{
    		sequencerMemory_[i] = 0xb7;
    		sequencerMemory_[i+1] = 0xb7;
    		sequencerMemory_[i+2] = 0xa6;
    		sequencerMemory_[i+3] = 0xb7;
    		sequencerMemory_[i+4] = 0xb6;
    		sequencerMemory_[i+5] = 0xb7;
    		sequencerMemory_[i+6] = 0xb6;
    		sequencerMemory_[i+7] = 0xb7;
    		sequencerMemory_[i+8] = 0xb6;
		}
	}
    
    readProgram(p_Main->getRomDir(VIP2K, MAINROM1), p_Main->getRomFile(VIP2K, MAINROM1), ROM, 0, NONAME);
    
    p_Main->assDefault("mycode", 0x8000, 0xFFFF);

	defineMemoryType(0x8000, 0xFFFF, RAM);
    initRam(0x8000, 0xFFFF);

	readProgram(p_Main->getRamDir(VIP2K), p_Main->getRamFile(VIP2K), NOCHANGE, 0x8000, SHOWNAME);
    
    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);

    if (pseudoType_ == "CHIP8")
		readProgram(p_Main->getChip8Dir(VIP2K), p_Main->getChip8SW(VIP2K), NOCHANGE, 0x8200, SHOWNAME);
	else
	{
        if (pseudoType_ == "CHIP8X")
			readProgram(p_Main->getChip8Dir(VIP2K), p_Main->getChip8SW(VIP2K), NOCHANGE, 0x8300, SHOWNAME);
		else
			readProgram(p_Main->getChip8Dir(VIP2K), p_Main->getChip8SW(VIP2K), NOCHANGE, 0x8200, SHOWNAME);
	}

	double zoom = p_Main->getZoom();

    configurePixie();
	initPixie();
	setZoom(zoom);
	Show(true);

	p_Main->updateTitle();

	cpuCycles_ = 0;
	p_Main->startTime();
	
//	p_Video->splashScreen();

	threadPointer->Run();
}

void Vip2K::writeMemDataType(Word address, Byte type)
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
	}
}

Byte Vip2K::readMemDataType(Word address)
{
	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
			return mainMemoryDataType_[address];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Vip2K::readMem(Word addr)
{
    address_ = addr;

    switch (memoryType_[addr/256])
	{
		case RAM:
        case ROM:
			return mainMemory_[addr];
		break;

        default:
			return 255;
		break;
	}
}

void Vip2K::writeMem(Word addr, Byte value, bool writeRom)
{
    address_ = addr;

    switch (memoryType_[addr/256])
	{
		case RAM:
			if (mainMemory_[addr]==value)
				return;
			mainMemory_[addr]=value;
            if (addr >= memoryStart_ && addr<(memoryStart_ + 256))
                p_Main->updateDebugMemory(addr);
            p_Main->updateAssTabCheck(addr);
		break;

		default:
			if (writeRom)
				mainMemory_[addr]=value;
		break;
	}
}

void Vip2K::cpuInstruction()
{
	if (cpuMode_ == RUN)
	{
		if (steps_ != 0)
		{
			cycle0_=0;
            machineCycle();
//            machineCycle();
			if (cycle0_ == 0) machineCycle();
			if (cycle0_ == 0 && steps_ != 0)
//            if (steps_ != 0)
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
		checkVipFunction();

		if (resetPressed_)
		{
			resetCpu();
			resetPressed_ = false;
			initPixie();
			vipRunState_ = RESETSTATE;
            for (int i=1; i<6; i++)
            {
                vipKeyState_[i] = 0xff;
            }
            shiftEf_ = 1;
            ctlEf_ = 1;
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
			p_Main->eventUpdateTitle();
			initPixie();
			runPressed_ = false;
		}
	}
}

void Vip2K::moveWindows()
{
	if (vipConfiguration.vtType != VTNONE)
		vtPointer->Move(p_Main->getVtPos(VIP2K));
}

void Vip2K::updateTitle(wxString Title)
{
    if (vipConfiguration.vtType == VT52)
        vtPointer->SetTitle("VIP2K Membership Card - VT 52" + Title);
    if (vipConfiguration.vtType == VT100)
        vtPointer->SetTitle("VIP2K Membership Card - VT 100" + Title);
}

void Vip2K::onReset()
{
	resetPressed_ = true;
}

void Vip2K::startComputerRun(bool load)
{
	load_ = load;
    p_Main->pload();
}

void Vip2K::sleepComputer(long ms)
{
	threadPointer->Sleep(ms);
}

void Vip2K::terminalSave(wxString fileName)
{
    if (vipConfiguration.vtType != VTNONE)
        vtPointer->terminalSaveVt(fileName);
}

void Vip2K::terminalLoad(wxString filePath, wxString fileName, bool binaryFile)
{
    if (vipConfiguration.vtType != VTNONE)
        vtPointer->terminalLoadVt(filePath, binaryFile);
    
    fileName = fileName.Left(fileName.Len()-3)+"ch8";
    
    if (vipConfiguration.autoKeyDef == false)
        return;
    
    if (p_Main->loadKeyDefinition("", fileName, keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt"))
    {
        Word keyboardMapRAM = 0xFF00;
        Word keyboardMapROM = 0x7640;
        bool stop = false;
        
        while (!stop)
        {
            if (mainMemory_[keyboardMapROM] != 0)
                mainMemory_[keyboardMapRAM++] = mainMemory_[keyboardMapROM++];
            else
            {
                keyboardMapROM++;
                if (mainMemory_[keyboardMapROM] == 0)
                    stop = true;
                else
                {
                    int repeats = mainMemory_[keyboardMapROM++];
                    for (int i=0; i<repeats; i++)
                    {
                        mainMemory_[keyboardMapRAM++] = mainMemory_[keyboardMapROM];
                    }
                    keyboardMapROM++;
                }
            }
        }
    
        mainMemory_[0xFF11] = keyDefGameHexA_[0];
        mainMemory_[0xFF0A] = keyDefGameHexA_[1];
        mainMemory_[0xFF12] = keyDefGameHexA_[2];
        mainMemory_[0xFF0B] = keyDefGameHexA_[3];
        mainMemory_[0xFF1B] = keyDefGameHexA_[4];
    }
}

void Vip2K::setAutoKeyDef(bool autoKeyDef)
{
    vipConfiguration.autoKeyDef = autoKeyDef;
}

void Vip2K::terminalStop()
{
    if (vipConfiguration.vtType != VTNONE)
        vtPointer->terminalStopVt();
    
    
    for (int i=1; i<6; i++)
    {
        vipKeyState_[i] = 0xff;
    }
    shiftEf_ = 1;
    ctlEf_ = 1;
}

void Vip2K::checkVipFunction()
{
    
}

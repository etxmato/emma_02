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

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#include "main.h"

#include "mcds.h"
#include "upd765.h"

Mcds::Mcds(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	McdsConfiguration = conf;

	McdsClockSpeed_ = clock;
	lastAddress_ = 0;

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	this->SetClientSize(size);

    saveStarted_ = false;
    loadStarted_ = false;

    p_Printer = new Printer();
    p_Printer->init(p_Printer, "MCDS", MS2000PRINTER);
}

Mcds::~Mcds()
{
    p_Printer->closeFrames();
    delete p_Printer;
    if (McdsConfiguration.vtType != VTNONE)
	{
		p_Main->setVtPos(MCDS, vtPointer->GetPosition());
		vtPointer->Destroy();
	}
    if (McdsConfiguration.vtExternal)
        delete p_Serial;
	p_Main->setMainPos(MCDS, GetPosition());
}

void Mcds::onClose(wxCloseEvent&WXUNUSED(event) )
{
	p_Main->stopComputer();
}

void Mcds::configureComputer()
{
	inType_[1] = MS2000IOGROUP;
	inType_[2] = MS2000IO2;
	inType_[3] = MS2000IO3;
	inType_[4] = MS2000IO4;
	inType_[5] = MS2000IO5;
	inType_[6] = MS2000IO6;
	inType_[7] = MS2000IO7;
	outType_[1] = MS2000IOGROUP;
	outType_[2] = MS2000IO2;
	outType_[3] = MS2000IO3;
	outType_[4] = MS2000IO4;
	outType_[5] = MS2000IO5;
	outType_[6] = MS2000IO6;
	outType_[7] = MS2000IO7;

	efType_[2] = MS2000CASEF;
    
    p_Main->message("Configuring MCDS");
    p_Main->message("	Output 1: set I/O group, input 1: read I/O group");
    p_Main->message("	I/O group 1: video terminal & printer");
    p_Main->message("	I/O group 2: tape");
 
    p_Main->message("");
    
    if (McdsConfiguration.vtType != VTNONE)
    {
        double zoom = p_Main->getZoomVt();
        if (McdsConfiguration.vtType == VT52)
            vtPointer = new Vt100("MCDS - VT 52", p_Main->getVtPos(MCDS), wxSize(640*zoom, 400*zoom), zoom, MCDS, McdsClockSpeed_, McdsConfiguration, UART1);
        else
            vtPointer = new Vt100("MCDS - VT 100", p_Main->getVtPos(MCDS), wxSize(640*zoom, 400*zoom), zoom, MCDS, McdsClockSpeed_, McdsConfiguration, UART1);
        p_Vt100[UART1] = vtPointer;
        vtPointer->configureStandard(McdsConfiguration.baudR, McdsConfiguration.baudT, 4);
    }
    
    if (McdsConfiguration.vtExternal)
    {
        p_Serial = new Serial(MCDS, McdsClockSpeed_, McdsConfiguration);
        p_Serial->configureStandard(McdsConfiguration.baudR, McdsConfiguration.baudT, 4);
    }

    p_Main->message("Configuring printer support");
    p_Main->message("	Output 6: data out");
    p_Main->message("	EF 1: printer ready\n");

    p_Main->message("Configuring tape support");
    p_Main->message("	Output 4: tape motor, output 5: cassette out");
    p_Main->message("	EF 2: cassette in\n");

	resetCpu();
}

void Mcds::initComputer()
{
    setClear(1);
    setWait(1);
    
    ioGroup_ = 0;
    cassetteEf_ = 0;
    
    mcdsRunState_ = RESETSTATE;
}

Byte Mcds::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

        case MS2000PRINTEF: // EF1
            return 1;
        break;
            
        case MS2000CASEF:   // EF2
            return cassetteEf_;
        break;
            
        case VT100EF:       // EF4
            return vtPointer->ef();
        break;
            
        case VTSERIALEF:
            return p_Serial->ef();
        break;
 
		default:
			return 1;
	}
}

Byte Mcds::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;
	ret = 0;

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

		case MS2000IOGROUP:
			return ioGroup_;
		break;

        case MS2000IO2:
        break;
            
        case MS2000IO3:
        break;
            
        case MS2000IO4:
        break;
            
        case MS2000IO5: 
        break;
            
        case MS2000IO6:
        break;
            
        case MS2000IO7:
        break;
            
        default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Mcds::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;

	switch(outType_[port])
	{
		case 0:
			return;
		break;

        case MS2000IOGROUP:
            bootstrap_ = 0;
            ioGroup_ = value;
        break;

        case MS2000IO2:
        break;
            
        case MS2000IO3:
        break;
            
        case MS2000IO4:
            switch (ioGroup_)
            {
                case IO_GRP_TAPE:
                    // 0x41 = TAPE 0, 0x81 = TAPE 1
                    if (value == 0x41)
                    {
                        tapeNumber_ = "";
                        if (!saveStarted_)
                        {
                            saveStarted_ = true;
                            p_Main->startCassetteSave(0);
                        }
                        else
                            restartTapeSave(TAPE_RECORD);
                    }
                    
                    if (value == 0x81)
                    {
                        tapeNumber_ = "1";
                        if (!saveStarted_)
                        {
                            saveStarted_ = true;
                            p_Main->startCassetteSave(1);
                        }
                        else
                            restartTapeSave(TAPE_RECORD1);
                    }
                    
                    // 0x40 = TAPE 0, 0x80 = TAPE 1
                    if (value == 0x40)
                    {
                        tapeNumber_ = "";
                        if (!loadStarted_)
                        {
                            loadStarted_ = true;
                            p_Main->startCassetteLoad(0);
                        }
                        else
                            restartTapeLoad(TAPE_PLAY);
                    }

                    if (value == 0x80)
                    {
                        tapeNumber_ = "1";
                        if (!loadStarted_)
                        {
                            loadStarted_ = true;
                            p_Main->startCassetteLoad(1);
                        }
                        else
                            restartTapeLoad(TAPE_PLAY1);
                    }

                    if (value == 0)
                        pauseTape();
                break;
            }
		break;

		case MS2000IO5:    
            switch (ioGroup_)
            {
                case IO_GRP_TAPE:
                    psaveAmplitudeChange(value&1);
                break;
            }
		break;

        case MS2000IO6:
            switch (ioGroup_)
            {
                case IO_GRP_UART: // Printer
                    p_Printer->printerOut(value ^ 0xff);
                break;
                
            }
        break;
            
        case MS2000IO7:
		break;
    }
}

void Mcds::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

        case VT100CYCLE:
			vtPointer->cycleVt();
		break;

        case VTSERIALCYCLE:
            p_Serial->cycleVt();
        break;
	}
}

void Mcds::startComputer()
{
	resetPressed_ = false;

    defineMemoryType(0, 0x7fff, RAM);
    initRam(0, 0x7fff);
    
    defineMemoryType(0x8c00, 0x8fff, RAM);
    initRam(0x8c00, 0x8fff);
    
    p_Main->assDefault("mycode", 0, 0xFFF);

    p_Main->checkAndReInstallFile(MCDS, "UT ROM", MAINROM1);
    readProgram(p_Main->getRomDir(MCDS, MAINROM1), p_Main->getRomFile(MCDS, MAINROM1), ROM, 0x8000, NONAME);
    p_Main->checkAndReInstallFile(MCDS, "ASM ROM", MAINROM2);
    readProgram(p_Main->getRomDir(MCDS, MAINROM2), p_Main->getRomFile(MCDS, MAINROM2), ROM, 0x9000, NONAME);
    p_Main->checkAndReInstallFile(MCDS, "BAS ROM", MAINROM3);
    readProgram(p_Main->getRomDir(MCDS, MAINROM3), p_Main->getRomFile(MCDS, MAINROM3), ROM, 0xB000, NONAME);

    if (p_Vt100[UART1] != NULL)
		p_Vt100[UART1]->Show(true);

    if (McdsConfiguration.bootRam)
        bootstrap_ = 0;
    else
        bootstrap_ = 0x8000;
    
    p_Main->setSwName("");
	p_Main->updateTitle();

	cpuCycles_ = 0;
	p_Main->startTime();

    if (p_Vt100[UART1] != NULL)
        p_Vt100[UART1]->splashScreen();
    
    threadPointer->Run();
}

void Mcds::writeMemDataType(Word address, Byte type)
{
	address = address | bootstrap_;
	switch (memoryType_[address/256])
	{
		case ROM:
			if (mainMemoryDataType_[address] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address] = type;
			}
		break;

		case RAM:
			if (mainMemoryDataType_[address | bootstrap_] != type)
			{
				p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
				mainMemoryDataType_[address | bootstrap_] = type;
			}
		break;
	}
}

Byte Mcds::readMemDataType(Word address)
{
	address = address | bootstrap_;
	switch (memoryType_[address/256])
	{
		case RAM:
		case ROM:
			return mainMemoryDataType_[address | bootstrap_];
		break;
	}
	return MEM_TYPE_UNDEFINED;
}

Byte Mcds::readMem(Word address)
{
	address = address | bootstrap_;

	switch (memoryType_[address / 256])
	{
		case UNDEFINED:
			return 255;
		break;

		case ROM:
		case RAM:
			return mainMemory_[address | bootstrap_];
		break;

		default:
			return 255;
		break;
	}
}

Byte Mcds::readMemDebug(Word address)
{
    return readMem(address);
}

void Mcds::writeMem(Word address, Byte value, bool writeRom)
{
	address = address | bootstrap_;

	switch (memoryType_[address/256])
	{
		case UNDEFINED:
		case ROM:
			if (writeRom)
				mainMemory_[address]=value;
		break;

		case RAM:
			if (mainMemory_[address]==value)
				return;
			mainMemory_[address]=value;
			if (address >= (memoryStart_ | bootstrap_) && address<((memoryStart_ | bootstrap_ ) +256))
				p_Main->updateDebugMemory(address);
			p_Main->updateAssTabCheck(address);
		break;
	}
}

void Mcds::writeMemDebug(Word address, Byte value, bool writeRom)
{
    writeMem(address, value, writeRom);
}

void Mcds::cpuInstruction()
{
    cpuCycleStep();
}

void Mcds::resetPressed()
{
    resetCpu();
    initComputer();
    
    if (McdsConfiguration.bootRam)
        bootstrap_ = 0;
    else
        bootstrap_ = 0x8000;
    
    p_Main->setSwName("");
    p_Main->eventUpdateTitle();
    resetPressed_ = false;
}

void Mcds::moveWindows()
{
	if (McdsConfiguration.vtType != VTNONE)
		vtPointer->Move(p_Main->getVtPos(MCDS));
}

void Mcds::setForceUpperCase(bool status)
{
    if (McdsConfiguration.vtType != VTNONE)
        vtPointer->setForceUCVt(status);
}

void Mcds::setBootRam(bool status)
{
	McdsConfiguration.bootRam = status;
}

void Mcds::updateTitle(wxString Title)
{
    if (McdsConfiguration.vtType == VT52)
		vtPointer->SetTitle("MCDS - VT 52"+Title);
    if (McdsConfiguration.vtType == VT100)
        vtPointer->SetTitle("MCDS - VT 100"+Title);
}

void Mcds::checkComputerFunction()
{
    wxString buffer;

    switch (scratchpadRegister_[programCounter_])
    {
		case 0x813e: // key input
            if (saveStarted_)
            {
                stopPausedSave();
                saveStarted_ = false;
            }
            
            if (loadStarted_)
            {
                stopPausedLoad();
                loadStarted_ = false;
            }
		break;
            
        case 0xb011:
            mcdsRunState_ = RESETSTATECW;
        break;
    
        case BASICADDR_READY_MCDS:
            mcdsRunState_ = BASICSTATE;
        break;

        case BASICADDR_RUN_MCDS:	// RUN
            mcdsRunState_ = RUNSTATE;
        break;
            
        case BASICADDR_CALL_MCDS:	// CALL
            mcdsRunState_ = RUNSTATE;
        break;
            
        case 0xB225:	// BYE
            mcdsRunState_ = RESETSTATE;
        break;
            
/*        case 0x9f:
        case 0xa3:
            buffer.Printf("%04X", mainMemory_[scratchpadRegister_[programCounter_]+1]*256+mainMemory_[scratchpadRegister_[programCounter_]+2]);
            p_Main->eventShowTextMessage(buffer);
            buffer.Printf("%04X", mainMemory_[scratchpadRegister_[programCounter_]+5]*256+mainMemory_[scratchpadRegister_[programCounter_]+6]);
            p_Main->eventShowTextMessage(buffer);
        break;*/
    }
}

void Mcds::onReset()
{
	resetPressed_ = true;
}

void Mcds::sleepComputer(long ms)
{
    threadPointer->Sleep(ms);
}

void Mcds::startComputerRun(bool load)
{
    if (p_Vt100[UART1] != NULL)
		vtPointer->startMcdsRun(load);
}

bool Mcds::isComputerRunning()
{
    if (mcdsRunState_ == RUNSTATE)
        return true;
    else
        return false;
}

void Mcds::activateMainWindow()
{
	bool maximize = IsMaximized();
	Iconize(false);
	Raise();
	Show(true);
	Maximize(maximize);
}

void Mcds::switchQ(int value)
{
    if (McdsConfiguration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (McdsConfiguration.vtExternal)
        p_Serial->switchQ(value);
}


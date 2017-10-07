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

#include "ms2000.h"
#include "upd765.h"

Ms2000::Ms2000(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	ms2000Configuration = conf;

	ms2000ClockSpeed_ = clock;
	lastAddress_ = 0;

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	this->SetClientSize(size);

	cycleSize_ = (int)(1000 / ((1/1.75) * 8)); // ~1000 Hz on 1.75 CPU
	cycleValue_ = cycleSize_;
    
    saveStarted_ = false;
    loadStarted_ = false;
	microDosRunning_ = false;
    resetHdData_ = true;

    p_Printer = new Printer();
    p_Printer->initMS2000(p_Printer);
}

Ms2000::~Ms2000()
{
    p_Printer->closeFrames();
    delete p_Printer;
    if (ms2000Configuration.vtType != VTNONE)
	{
		p_Main->setVtPos(MS2000, vtPointer->GetPosition());
		vtPointer->Destroy();
	}
	p_Main->setMainPos(MS2000, GetPosition());
}

void Ms2000::onClose(wxCloseEvent&WXUNUSED(event) )
{
	p_Main->stopComputer();
}

void Ms2000::configureComputer()
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
    
	p_Main->message("Configuring MS2000");
    p_Main->message("	Output 1: set I/O group, input 1: read I/O group\n");

    p_Main->message("	I/O group 1: video terminal & printer");
    p_Main->message("	I/O group 2: tape");
    p_Main->message("	I/O group 8: 18S651 and uPD765");
 
    p_Main->message("");
    
    if (ms2000Configuration.vtType != VTNONE)
    {
        double zoom = p_Main->getZoomVt();
        if (ms2000Configuration.vtType == VT52)
            vtPointer = new Vt100("MS2000 - VT 52", p_Main->getVtPos(MS2000), wxSize(640*zoom, 400*zoom), zoom, MS2000, ms2000ClockSpeed_, ms2000Configuration);
        else
            vtPointer = new Vt100("MS2000 - VT 100", p_Main->getVtPos(MS2000), wxSize(640*zoom, 400*zoom), zoom, MS2000, ms2000ClockSpeed_, ms2000Configuration);
        p_Vt100 = vtPointer;
        vtPointer->configureMs2000(ms2000Configuration.baudR, ms2000Configuration.baudT);
    }
    
    p_Main->message("Configuring printer support");
    p_Main->message("	Output 6: data out");
    p_Main->message("	EF 1: printer ready\n");

    p_Main->message("Configuring tape support");
    p_Main->message("	Output 4: tape motor, output 5: cassette out");
    p_Main->message("	EF 2: cassette in\n");

    configureUpd765();
	resetCpu();
}

void Ms2000::initComputer()
{
    setClear(1);
    setWait(1);
    
    ioGroup_ = 0;
    cassetteEf_ = 0;
}

Byte Ms2000::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

        case MS2000PRINTEF: // EF1
            return 0;
        break;
            
        case MS2000CASEF:   // EF2
            return cassetteEf_;
        break;
            
		case MS2000EF:      // EF3
            return efInterrupt();
        break;
                    
        case VT100EF:       // EF4
            return vtPointer->ef();
        break;
            
        default:
			return 1;
	}
}

Byte Ms2000::in(Byte port, Word WXUNUSED(address))
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
            switch (ioGroup_)
            {
                case IO_GRP_UART:
                    return vtPointer->uartIn();
                break;
                
                default:
                    ret = 255;
            }
        break;
            
        case MS2000IO3:
            switch (ioGroup_)
            {
                case IO_GRP_UART:
                    return vtPointer->uartStatus();
                break;
                    
                default:
                    ret = 255;
            }
        break;
            
        case MS2000IO4:
            switch (ioGroup_)
            {
                case IO_GRP_UPD:
                    return inputMasterStatus();
                break;
                
                default:
                    ret = 255;
            }
        break;
            
        case MS2000IO5: 
            switch (ioGroup_)
            {
                case IO_GRP_UPD:
                    return inputCommandStatus();
                break;
                
                default:
                    ret = 255;
            }
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

void Ms2000::out(Byte port, Word WXUNUSED(address), Byte value)
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
            switch (ioGroup_)
            {
                case IO_GRP_UART:
                    vtPointer->uartOut(value);
                break;
            }
        break;
            
        case MS2000IO3:
            switch (ioGroup_)
            {
                case IO_GRP_UART:
                    vtPointer->uartControl(value);
                break;
            }
        break;
            
        case MS2000IO4:
            switch (ioGroup_)
            {
                case IO_GRP_TAPE:
                    // 0x41 = TAPE 0, 0x81 = TAPE 1
                    if (value == 0x41 || value == 0x81)
                    {
                        if (!saveStarted_)
                        {
                            saveStarted_ = true;
                            p_Main->startCassetteSave();
                        }
                        else
                            restartTapeSave();
                    }
                    
                    // 0x40 = TAPE 0, 0x80 = TAPE 1
                    if (value == 0x40 || value == 0x80)
                    {
                        if (!loadStarted_)
                        {
                            loadStarted_ = true;
                            p_Main->startCassetteLoad();
                        }
                        else
                            restartTapeLoad();
                    }

                    if (value == 0)
                        pauseTape();
                break;
 
                case IO_GRP_UPD:
                    outputDmaControl(value);
                break;
            }
		break;

		case MS2000IO5:    
            switch (ioGroup_)
            {
                case IO_GRP_TAPE:
                    psaveAmplitudeChange(value&1);
                break;
                    
                case IO_GRP_UPD:
                    outputCommand(value);
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
            switch (ioGroup_)
            {
                case IO_GRP_UPD:
                    outputDmaCounter(value);
                break;
            }
		break;
    }
}

void Ms2000::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;

        case FDCCYCLE:
            cycleUpd765();
        break;
            
        case VT100CYCLE:
			vtPointer->cycleVt();
		break;
	}
}

void Ms2000::startComputer()
{
	resetPressed_ = false;

    defineMemoryType(0, 0x7fff, RAM);
    initRam(0, 0x7fff);
 
    defineMemoryType(0x8800, 0xffff, RAM);
    initRam(0x8800, 0xffff);

    p_Main->assDefault("mycode", 0, 0xFFF);
    
	readProgram(p_Main->getRomDir(MS2000, MAINROM1), p_Main->getRomFile(MS2000, MAINROM1), ROM, 0x8000, NONAME);
    
    vtPointer->Show(true);

    if (ms2000Configuration.bootRam)
        bootstrap_ = 0;
    else
        bootstrap_ = 0x8000;
    
    if (p_Main->getDirectoryMode(0))
        setDiskName(1, p_Main->getMs2000FloppyDirSwitched(0), "");
    else
    {
        wxString fileName = p_Main->getMs2000FloppyFile(0);
        if (fileName.Len() == 0)
            setDiskName(1, p_Main->getMs2000FloppyDir(0), "");
        else
            setDiskName(1, p_Main->getMs2000FloppyDir(0), p_Main->getMs2000FloppyFile(0));
    }
    
    if (p_Main->getDirectoryMode(1))
        setDiskName(2, p_Main->getMs2000FloppyDirSwitched(1), "");
    else
    {
        wxString fileName = p_Main->getMs2000FloppyFile(1);
        if (fileName.Len() == 0)
            setDiskName(2, p_Main->getMs2000FloppyDir(1), "");
        else
            setDiskName(2, p_Main->getMs2000FloppyDir(1), p_Main->getMs2000FloppyFile(1));
    }
    
    if (p_Main->getDirectoryMode(2))
        setDiskName(3, p_Main->getMs2000FloppyDirSwitched(2), "");
    else
    {
        wxString fileName = p_Main->getMs2000FloppyFile(2);
        if (fileName.Len() == 0)
            setDiskName(3, p_Main->getMs2000FloppyDir(2), "");
        else
            setDiskName(3, p_Main->getMs2000FloppyDir(2), p_Main->getMs2000FloppyFile(2));
    }
    
    if (p_Main->getDirectoryMode(3))
        setDiskName(4, p_Main->getMs2000FloppyDirSwitched(3), "");
    else
    {
        wxString fileName = p_Main->getMs2000FloppyFile(3);
        if (fileName.Len() == 0)
            setDiskName(4, p_Main->getMs2000FloppyDir(3), "");
        else
            setDiskName(4, p_Main->getMs2000FloppyDir(3), p_Main->getMs2000FloppyFile(3));
    }
    
    p_Main->setSwName("");
	p_Main->updateTitle();

	cpuCycles_ = 0;
	p_Main->startTime();

    threadPointer->Run();
}

void Ms2000::writeMemDataType(Word address, Byte type)
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

Byte Ms2000::readMemDataType(Word address)
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

Byte Ms2000::readMem(Word addr)
{
	address_ = addr | bootstrap_;

	switch (memoryType_[addr / 256])
	{
		case UNDEFINED:
			return 255;
		break;

		case ROM:
		case RAM:
			return mainMemory_[addr | bootstrap_];
		break;

		default:
			return 255;
		break;
	}
}

void Ms2000::writeMem(Word addr, Byte value, bool writeRom)
{
	address_ = addr | bootstrap_;

	switch (memoryType_[addr/256])
	{
		case UNDEFINED:
		case ROM:
			if (writeRom)
				mainMemory_[addr]=value;
		break;

		case RAM:
			if (mainMemory_[address_]==value)
				return;
			mainMemory_[address_]=value;
			if (address_ >= (memoryStart_ | bootstrap_) && address_<((memoryStart_ | bootstrap_ ) +256))
				p_Main->updateDebugMemory(addr);
			p_Main->updateAssTabCheck(address_);
		break;
	}
}

void Ms2000::cpuInstruction()
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
    checkMs2000Function();
    
    if (resetPressed_)
    {
        resetCpu();
        initComputer();

        if (ms2000Configuration.bootRam)
            bootstrap_ = 0;
        else
            bootstrap_ = 0x8000;
        
        p_Main->setSwName("");
        p_Main->eventUpdateTitle();
        resetPressed_ = false;
    }
    if (debugMode_)
        p_Main->cycleDebug();

}

void Ms2000::moveWindows()
{
	if (ms2000Configuration.vtType != VTNONE)
		vtPointer->Move(p_Main->getVtPos(MS2000));
}

void Ms2000::setForceUpperCase(bool status)
{
    if (ms2000Configuration.vtType != VTNONE)
        vtPointer->setForceUCVt(status);
}

void Ms2000::setBootRam(bool status)
{
	ms2000Configuration.bootRam = status;
}

void Ms2000::updateTitle(wxString Title)
{
    if (ms2000Configuration.vtType == VT52)
		vtPointer->SetTitle("MS2000 - VT 52"+Title);
    if (ms2000Configuration.vtType == VT100)
        vtPointer->SetTitle("MS2000 - VT 100"+Title);
}

void Ms2000::checkMs2000Function()
{
    switch (scratchpadRegister_[programCounter_])
    {
		case 0x95e5: // MicroDos key input
			microDosRunning_ = true;
            resetHdData_ = true;
		break;

        case 0x34ad: // BASIC COMPILER key input
        case 0x34ce: // BASIC COMPILER key input
            microDosRunning_ = false;
            resetHdData_ = false;
        break;
            
        case 0xd13: // BASIC1 key input
			microDosRunning_ = false ;
            resetHdData_ = true;
        break;

		case 0x813e: // key input
            if (resetHdData_ && resetHdData_)
            {
                for (int drive = 0; drive < 4; drive++)
                {
                    initializeCat(drive);
                    hdCommand_ = 0;
                    for (int cluster = FIRST_CLUSTER; cluster < (MAX_CLUSTER + BUFFER_CLUSTER); cluster++)
                    {
                        clusterInfo_[drive][cluster].filenameDefined = false;
                        clusterInfo_[drive][cluster].readCluster = true;
                        clusterInfo_[drive][cluster].sdwClusterDefined = false;
                    }
                }
                resetHdData_ = false;
            }
            
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
            
            if (microDosRunning_)
                vtPointer->setTabChar(0x7f);
            else
                vtPointer->setTabChar(8);

            microDosRunning_ = false;
        break;
    }
}

void Ms2000::onReset()
{
	resetPressed_ = true;
}

void Ms2000::sleepComputer(long ms)
{
    threadPointer->Sleep(ms);
}

void Ms2000::activateMainWindow()
{
	bool maximize = IsMaximized();
	Iconize(false);
	Raise();
	Show(true);
	Maximize(maximize);
}

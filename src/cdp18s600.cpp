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

#include "cdp18s600.h"
#include "upd765.h"

Word romSize[]=
{
    0x7ff, 0xfff, 0x1fff
};

Cdp18s600::Cdp18s600(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    Cdp18s600Configuration = conf;
    
    Cdp18s600ClockSpeed_ = clock;
    lastAddress_ = 0;
    
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif
    
    this->SetClientSize(size);
    
    cycleSize_ = (int)(1000 / ((1/1.75) * 8)); // ~1000 Hz on 1.75 CPU
    cycleValue_ = cycleSize_;
}

Cdp18s600::~Cdp18s600()
{
    if (Cdp18s600Configuration.vtType != VTNONE)
    {
        p_Main->setVtPos(CDP18S600, vtPointer->GetPosition());
        vtPointer->Destroy();
    }
    if (Cdp18s600Configuration.vtExternal)
        delete p_Serial;
    p_Main->setMainPos(CDP18S600, GetPosition());
}

void Cdp18s600::onClose(wxCloseEvent&WXUNUSED(event) )
{
    p_Main->stopComputer();
}

void Cdp18s600::configureComputer()
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
    
    p_Main->message("Configuring CDP18S600");
    p_Main->message("    Output 1: set I/O group, input 1: read I/O group");
    p_Main->message("    I/O group 1: video terminal");
    
    p_Main->message("");
    
    if (Cdp18s600Configuration.vtType != VTNONE)
    {
        double zoom = p_Main->getZoomVt();
        if (Cdp18s600Configuration.vtType == VT52)
            vtPointer = new Vt100("CDP18S600 - VT 52", p_Main->getVtPos(CDP18S600), wxSize(640*zoom, 400*zoom), zoom, CDP18S600, Cdp18s600ClockSpeed_, Cdp18s600Configuration);
        else
            vtPointer = new Vt100("CDP18S600 - VT 100", p_Main->getVtPos(CDP18S600), wxSize(640*zoom, 400*zoom), zoom, CDP18S600, Cdp18s600ClockSpeed_, Cdp18s600Configuration);
        p_Vt100 = vtPointer;
        vtPointer->configureMs2000(Cdp18s600Configuration.baudR, Cdp18s600Configuration.baudT);
    }
    
    if (Cdp18s600Configuration.vtExternal)
    {
        p_Serial = new Serial(CDP18S600, Cdp18s600ClockSpeed_, Cdp18s600Configuration);
        p_Serial->configureMs2000(Cdp18s600Configuration.baudR, Cdp18s600Configuration.baudT);
    }
    
    resetCpu();
}

void Cdp18s600::initComputer()
{
    addressLatch_ = p_Main->getBootAddress("CDP18S600", computerType_);

    setClear(1);
    setWait(1);
    
    ioGroup_ = 0;
    
    mcdsRunState_ = RESETSTATE;
}

Byte Cdp18s600::ef(int flag)
{
    switch(efType_[flag])
    {
        case 0:
            return 1;
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

Byte Cdp18s600::in(Byte port, Word WXUNUSED(address))
{
    Byte ret;
    ret = 0;
    
    switch(inType_[port])
    {
        case 0:
            ret = 255;
        break;
            
        case MS2000IOGROUP:
        break;
            
        case MS2000IO2:
            switch (ioGroup_)
            {
                case IO_GRP1_UART:
                    if (Cdp18s600Configuration.uartGroup == 0)
                    {
                        if (p_Vt100 != NULL)
                            return p_Vt100->uartIn();
                        if (p_Serial != NULL)
                            return p_Serial->uartIn();
                    }
                break;
                
                case IO_GRP2_UART:
                    if (Cdp18s600Configuration.uartGroup == 1)
                    {
                        if (p_Vt100 != NULL)
                            return p_Vt100->uartIn();
                        if (p_Serial != NULL)
                            return p_Serial->uartIn();
                    }
                break;
                    
                default:
                    ret = 255;
            }
        break;
            
        case MS2000IO3:
            switch (ioGroup_)
            {
                case IO_GRP1_UART:
                    if (Cdp18s600Configuration.uartGroup == 0)
                    {
                        if (p_Vt100 != NULL)
                            return p_Vt100->uartStatus();
                        if (p_Serial != NULL)
                            return p_Serial->uartStatus();
                    }
                break;
                
                case IO_GRP2_UART:
                    if (Cdp18s600Configuration.uartGroup == 1)
                    {
                        if (p_Vt100 != NULL)
                            return p_Vt100->uartStatus();
                        if (p_Serial != NULL)
                            return p_Serial->uartStatus();
                    }
                break;
                    
                default:
                    ret = 255;
            }
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

void Cdp18s600::out(Byte port, Word WXUNUSED(address), Byte value)
{
    outValues_[port] = value;
    
    switch(outType_[port])
    {
        case 0:
            return;
        break;
            
        case MS2000IOGROUP:
            ioGroup_ = value;
        break;
            
        case MS2000IO2:
            switch (ioGroup_)
            {
                case IO_GRP1_UART:
                    if (Cdp18s600Configuration.uartGroup == 0)
                    {
                        if (p_Vt100 != NULL)
                            p_Vt100->uartOut(value);
                        if (p_Serial != NULL)
                            p_Serial->uartOut(value);
                    }
                break;

                case IO_GRP2_UART:
                    if (Cdp18s600Configuration.uartGroup == 1)
                    {
                        if (p_Vt100 != NULL)
                            p_Vt100->uartOut(value);
                        if (p_Serial != NULL)
                            p_Serial->uartOut(value);
                    }
                break;
            }
        break;
            
        case MS2000IO3:
            switch (ioGroup_)
            {
                case IO_GRP1_UART:
                    if (Cdp18s600Configuration.uartGroup == 0)
                    {
                        if (p_Vt100 != NULL)
                            p_Vt100->uartControl(value);
                        if (p_Serial != NULL)
                            p_Serial->uartControl(value);
                    }
                break;
                    
                case IO_GRP2_UART:
                    if (Cdp18s600Configuration.uartGroup == 1)
                    {
                        if (p_Vt100 != NULL)
                            p_Vt100->uartControl(value);
                        if (p_Serial != NULL)
                            p_Serial->uartControl(value);
                    }
                break;
            }
        break;
            
        case MS2000IO4:
        break;
            
        case MS2000IO5:
        break;
            
        case MS2000IO6:
        break;
            
        case MS2000IO7:
        break;
    }
}

void Cdp18s600::cycle(int type)
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

void Cdp18s600::startComputer()
{
    resetPressed_ = false;
        
//    p_Main->checkAndReInstallFile(CDP18S600, "ROM U21", U21ROM);
    Word currentRomSize = romSize[p_Main->getMicroChipType(CDP18S600, ONE_SOCKET)];
    int divider = (2 << (p_Main->getMicroChipType(CDP18S600, ONE_SOCKET)))/2;
    int location = p_Main->getMicroChipLocation(CDP18S600, ONE_SOCKET) / divider;
    Word startAddress = location * (currentRomSize+1);
    Word lastAddress = startAddress + currentRomSize;
    
    readMicro(U21ROM, startAddress, lastAddress);

//    p_Main->checkAndReInstallFile(CDP18S600, "ROM U20", U20ROM);
    currentRomSize = romSize[p_Main->getMicroChipType(CDP18S600, FOUR_SOCKET)];
    divider = (2 << (p_Main->getMicroChipType(CDP18S600, FOUR_SOCKET)))/2;
    location = p_Main->getMicroChipLocation(CDP18S600, FOUR_SOCKET) / divider;
    startAddress = location * (4*(currentRomSize+1));
    lastAddress = startAddress + currentRomSize;
    
    readMicro(U20ROM, startAddress, lastAddress);

//    p_Main->checkAndReInstallFile(CDP18S600, "ROM U19", U19ROM);
    startAddress += (currentRomSize+1);
    lastAddress = startAddress + currentRomSize;
    
    readMicro(U19ROM, startAddress, lastAddress);

//    p_Main->checkAndReInstallFile(CDP18S600, "ROM U18", U18ROM);
    startAddress += (currentRomSize+1);
    lastAddress = startAddress + currentRomSize;
    
    readMicro(U18ROM, startAddress, lastAddress);

//    p_Main->checkAndReInstallFile(CDP18S600, "ROM U17", U17ROM);
    startAddress += (currentRomSize+1);
    lastAddress = startAddress + currentRomSize;
    
    readMicro(U17ROM, startAddress, lastAddress);

    if (p_Vt100 != NULL)
        p_Vt100->Show(true);
    
    p_Main->setSwName("");
    p_Main->updateTitle();
    
    cpuCycles_ = 0;
    p_Main->startTime();
    
//    if (p_Vt100 != NULL)
//        p_Vt100->splashScreen();
    
    threadPointer->Run();
}

void Cdp18s600::readMicro(int romNumber, Word startAddress, Word lastAddress )
{
    if (p_Main->getMicroChipDisable(CDP18S600, romNumber))
        return;
    
    switch (p_Main->getMicroChipMemory(CDP18S600, romNumber))
    {
        case MICRO_ROM:
            readProgramMicro(p_Main->getRomDir(CDP18S600, romNumber), p_Main->getRomFile(CDP18S600, romNumber), ROM, startAddress, lastAddress+1, NONAME);
            defineMemoryType(startAddress, lastAddress, ROM);
        break;

        case MICRO_RAM:
            readProgramMicro(p_Main->getRomDir(CDP18S600, romNumber), p_Main->getRomFile(CDP18S600, romNumber), RAM, startAddress, lastAddress+1, NONAME);
            defineMemoryType(startAddress, lastAddress, RAM);
            p_Main->assDefault("mycode", startAddress, lastAddress);
        break;

        case MICRO_CPURAM:
            readProgramMicro(p_Main->getRomDir(CDP18S600, romNumber), p_Main->getRomFile(CDP18S600, romNumber), CPURAM, startAddress, lastAddress+1, NONAME);
            defineMemoryType(startAddress, lastAddress, CPURAM);
            p_Main->assDefault("mycode", startAddress, lastAddress);
        break;
    }
}

void Cdp18s600::writeMemDataType(Word address, Byte type)
{
    if (address < 0x8000)
        address = (address | addressLatch_);

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
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
        break;

        case CPURAM:
            if (cpuRamDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                cpuRamDataType_[address] = type;
            }
        break;
    }
}

Byte Cdp18s600::readMemDataType(Word address)
{
    if (address < 0x8000)
        address = (address | addressLatch_);

    switch (memoryType_[address/256])
    {
        case RAM:
        case ROM:
            return mainMemoryDataType_[address];
        break;

        case CPURAM:
            return cpuRamDataType_[address];
        break;
    }
    return MEM_TYPE_UNDEFINED;
}

Byte Cdp18s600::readMem(Word address)
{
    if ((address & 0x8000) == 0x8000)
        addressLatch_ = 0;
    
    if (address < 0x8000)
        address = (address | addressLatch_);

    switch (memoryType_[address / 256])
    {
        case UNDEFINED:
            return 255;
        break;
            
        case ROM:
        case RAM:
            return mainMemory_[address];
        break;
            
        case CPURAM:
            return cpuRam_[address&0xff];
        break;

        default:
            return 255;
        break;
    }
}

Byte Cdp18s600::readMemDebug(Word address)
{
    return readMem(address);
}

void Cdp18s600::writeMem(Word address, Byte value, bool writeRom)
{
    address = address;
    
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
            if (address >= (memoryStart_) && address<((memoryStart_) +256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;

        case CPURAM:
            if (cpuRam_[address&0xff]==value)
                return;
            cpuRam_[address&0xff]=value;
            p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;
    }
}

void Cdp18s600::writeMemDebug(Word address, Byte value, bool writeRom)
{
    writeMem(address, value, writeRom);
}

void Cdp18s600::cpuInstruction()
{
    cpuCycleStep();
}

void Cdp18s600::resetPressed()
{
    resetCpu();
    initComputer();
    
    p_Main->setSwName("");
    p_Main->eventUpdateTitle();
    resetPressed_ = false;
}

void Cdp18s600::moveWindows()
{
    if (Cdp18s600Configuration.vtType != VTNONE)
        vtPointer->Move(p_Main->getVtPos(CDP18S600));
}

void Cdp18s600::setForceUpperCase(bool status)
{
    if (Cdp18s600Configuration.vtType != VTNONE)
        vtPointer->setForceUCVt(status);
}

void Cdp18s600::setBootRam(bool status)
{
    Cdp18s600Configuration.bootRam = status;
}

void Cdp18s600::updateTitle(wxString Title)
{
    if (Cdp18s600Configuration.vtType == VT52)
        vtPointer->SetTitle("CDP18S600 - VT 52"+Title);
    if (Cdp18s600Configuration.vtType == VT100)
        vtPointer->SetTitle("CDP18S600 - VT 100"+Title);
}

void Cdp18s600::checkComputerFunction()
{
    wxString buffer;
    
    switch (scratchpadRegister_[programCounter_])
    {
        case 0xb011:
            mcdsRunState_ = RESETSTATECW;
        break;
            
        case 0xb053:
            mcdsRunState_ = BASICSTATE;
        break;
            
        case 0xC076:    // RUN
            mcdsRunState_ = RUNSTATE;
        break;
            
        case 0xc79f:    // CALL
            mcdsRunState_ = RUNSTATE;
        break;
            
        case 0xB225:    // BYE
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

void Cdp18s600::onReset()
{
    resetPressed_ = true;
}

void Cdp18s600::sleepComputer(long ms)
{
    threadPointer->Sleep(ms);
}

void Cdp18s600::startComputerRun(bool load)
{
    if (p_Vt100 != NULL)
        vtPointer->startMcdsRun(load);
}

bool Cdp18s600::isComputerRunning()
{
    if (mcdsRunState_ == RUNSTATE)
        return true;
    else
        return false;
}

void Cdp18s600::activateMainWindow()
{
    bool maximize = IsMaximized();
    Iconize(false);
    Raise();
    Show(true);
    Maximize(maximize);
}

void Cdp18s600::switchQ(int value)
{
    if (Cdp18s600Configuration.vtType != VTNONE)
        vtPointer->switchQ(value);
    
    if (Cdp18s600Configuration.vtExternal)
        p_Serial->switchQ(value);
}

void Cdp18s600::setAddressLatch(Word bootAddress)
{
    addressLatch_ = bootAddress;
}



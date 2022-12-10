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

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#include "main.h"
#include "pushbutton.h"

#include "pico.h"

#define OFF    1

BEGIN_EVENT_TABLE(Pico, wxFrame)
    EVT_CLOSE (Pico::onClose)
    EVT_TIMER(900, Elf::OnRtcTimer)
END_EVENT_TABLE()

Pico::Pico(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf, Conf computerConf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    computerConfiguration = computerConf;
    elfConfiguration = conf;
    elfClockSpeed_ = clock;

#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif
    
    this->SetClientSize(size);

    rtcTimerPointer = new wxTimer(this, 900);
    cycleValue_ = -1;
    cycleSize_ = -1;

    runningGame_ = "";
}

Pico::~Pico()
{
    saveRtc();
    rtcTimerPointer->Stop();
    delete rtcTimerPointer;
    if (elfConfiguration.usePixie)
    {
        p_Main->setPixiePos(PICO, pixiePointer->GetPosition());
        pixiePointer->Destroy();
    }
    if (elfConfiguration.useTMS9918)
    {
        p_Main->setTmsPos(PICO, tmsPointer->GetPosition());
        tmsPointer->Destroy();
    }
    if (elfConfiguration.use6845||elfConfiguration.useS100)
    {
        p_Main->set6845Pos(PICO, mc6845Pointer->GetPosition());
        mc6845Pointer->Destroy();
    }
    if (elfConfiguration.use6847)
    {
        p_Main->set6847Pos(PICO, mc6847Pointer->GetPosition());
        mc6847Pointer->Destroy();
    }
    if (elfConfiguration.use8275)
    {
        p_Main->set8275Pos(PICO, i8275Pointer->GetPosition());
        i8275Pointer->Destroy();
    }
    if (elfConfiguration.vtType != VTNONE)
    {
        p_Main->setVtPos(PICO, vtPointer->GetPosition());
        vtPointer->Destroy();
    }
    if (elfConfiguration.vtExternal)
        delete p_Serial;
    if (p_Main->getPrinterStatus(PICO))
    {
        p_Printer->closeFrames();
        delete p_Printer;
    }
    p_Main->setMainPos(PICO, GetPosition());
}

void Pico::onClose(wxCloseEvent&WXUNUSED(event) )
{
    p_Main->stopComputer();
}

void Pico::charEvent(int keycode)
{
    if (elfConfiguration.useKeyboard)
        charEventKeyboard(keycode);
}

bool Pico::keyDownPressed(int key)
{
    if (elfConfiguration.usePS2)
    {
        keyDownPs2(key);
            return true;
    }
    return false;
}

bool Pico::keyUpReleased(int key, wxKeyEvent&WXUNUSED(event))
{

    if (key == inKey1_ || key == inKey2_)
    {
        onInButtonRelease();
        return true;
    }
    onHexKeyUp(key);
    if (elfConfiguration.usePS2)
    {
        keyUpPs2(key);
        return true;
    }
    return false;
}

void Pico::configureComputer()
{
    efType_[1] = EF1UNDEFINED;
    efType_[2] = EF2UNDEFINED;
    efType_[3] = EF3UNDEFINED;

    setCycleType(COMPUTERCYCLE, LEDCYCLE);
    wxString printBuffer;

    p_Main->message("Configuring Pico/Elf V2");

    if (elfConfiguration.useTape && !elfConfiguration.useXmodem)
    {
        efType_[elfConfiguration.ioConfiguration.tapeEf] = ELF2EF2;
        printBuffer.Printf("    EF %d: cassette in", elfConfiguration.ioConfiguration.tapeEf);
        p_Main->message(printBuffer);
    }

    p_Main->message("");
    
    resetCpu();
}

void Pico::setPrinterEf()
{
    ef3State_ = 0;
}

void Pico::initComputer()
{
    cassetteEf_ = 0;
    ef3State_ = 1;
    ef4State_ = 1;
    ef3Button_ = 1;
    ef4Button_ = 1;
    elfRunState_ = RESETSTATE;
    bootstrap_ = 0;
}

Byte Pico::ef(int flag)
{
    switch (flag)
    {
        case 4:
            if (ef4State_ == 0)
                return ef4State_;
        break;
    }
    switch(efType_[flag])
    {
        case 0:
            return 1;
        break;

        case TMSINTERRUPT:
            return tmsPointer->readEf();
        break;
            
        case PIXIEEF:
            return pixiePointer->efPixie();
        break;

        case KEYBRDEF:
            return efKeyboard();
        break;

        case PS2GPIOEF:
            return efPs2gpio();
        break;

        case PS2EF:
            return efPs2();
        break;

        case FDCEF:
            return ef1793();
        break;

        case VT100EF:
            return vtPointer->ef();
        break;

        case VTSERIALEF:
            return p_Serial->ef();
        break;
 
        case MC6845EF:
            return mc6845Pointer->ef6845();
        break;

        case I8275EF:
            return i8275Pointer->ef8275();
        break;

        case ELF2EF2:
            return cassetteEf_;
        break;

        case ELFPRINTEREF:
            return ef3State_;
        break;

        case EF1UNDEFINED:
            return elfConfiguration.ioConfiguration.ef1default;
        break;

        case EF2UNDEFINED:
            return elfConfiguration.ioConfiguration.ef2default;
        break;

        case EF3UNDEFINED:
            return elfConfiguration.ioConfiguration.ef3default;
        break;

        default:
            return 1;
    }
}

Byte Pico::in(Byte port, Word WXUNUSED(address))
{
    Byte ret;

    switch(inType_[port])
    {
        case 0:
            ret = 255;
        break;

        case TMSDATAPORT:
            ret = tmsPointer->readVRAM();
        break;

        case TMSREGISTERPORT:
            ret = tmsPointer->readDataPort();
        break;

        case PIXIEIN:
            ret = pixiePointer->inPixie();
        break;

        case I8275PREGREAD:
            ret = i8275Pointer->pRegRead();
        break;

        case I8275SREGREAD:
            ret = i8275Pointer->sRegRead();
        break;

        case KEYBRDIN:
            ret = inKeyboard();
        break;

        case PS2GPIOIN:
            ret = inPs2gpio();
        break;

        case PS2IN:
            ret = inPs2();
        break;

        case FDCREADIN:
            ret = in1793();
        break;

        case IDEIN:
            ret = inIde();
        break;

        case PORTEXTIN:
            ret = inPortExtender();
        break;

        case UARTIN:
            return vtPointer->uartIn();
        break;

        case UARTINSERIAL:
            return p_Serial->uartIn();
        break;

        case UARTSTATUS:
            return vtPointer->uartStatus();
        break;

        case UARTSTATUSSERIAL:
            return p_Serial->uartStatus();
        break;

        case ELF2KDISKREADREGISTER:
            return inDisk();
        break;

        case ELF2KDISKREADSTATUS:
            return readDiskStatus();
        break;

        default:
            ret = 255;
    }
    inValues_[port] = ret;
    return ret;
}

void Pico::out(Byte port, Word WXUNUSED(address), Byte value)
{
    outValues_[port] = value;

    switch(outType_[port])
    {
        case 0:
            return;
        break;

//        case RCADISKOUT:
//            psave(value);
//        break;

        case TMSREGISTERPORT:
            tmsPointer->modeHighOut(value);
        break;

        case TMSDATAPORT:
            tmsPointer->writeVRAM(value);
        break;

        case PIXIEOUT:
            pixiePointer->outPixie();
        break;

        case MC6847OUT:
            mc6847Pointer->outMc6847(value);
        break;

        case I8275PREGWRITE:
            i8275Pointer->pRegWrite(value);
        break;

        case I8275CREGWRITE:
            i8275Pointer->cRegWrite(value);
        break;

        case VT100OUT:
            vtPointer->out(value);
        break;

        case VTOUTSERIAL:
            p_Serial->out(value);
        break;

        case PRINTEROUT:
            //p_Main->eventPrintDefault(value);
            if ((value & 0xfc) != 0)
                p_Printer->printerOut(value);
        break;

        case PS2OUT:
            outPs2(value);
        break;

        case FDCSELECTOUT:
            selectRegister1793(value);
        break;

        case FDCWRITEOUT:
            writeRegister1793(value);
        break;

        case IDESELECTOUT:
            selectIdeRegister(value);
        break;

        case IDEWRITEOUT:
            outIde(value);
        break;

        case PORTEXTSELECTOUT:
            selectPortExtender(value);
        break;

        case PORTEXTWRITEOUT:
            outPortExtender(value);
        break;

        case UARTOUT:
             vtPointer->uartOut(value);
        break;

        case UARTOUTSERIAL:
             p_Serial->uartOut(value);
        break;

        case UARTCONTROL:
            vtPointer->uartControl(value);
        break;
            
        case UARTCONTROLSERIAL:
            p_Serial->uartControl(value);
        break;
                        
        case ELF2KDISKSELECTREGISTER:
            selectDiskRegister(value);
        break;

        case ELF2KDISKWRITEREGISTER:
            outDisk(value);
        break;
    }
}

void Pico::cycle(int type)
{
    switch(cycleType_[type])
    {
        case 0:
            return;
        break;

        case KEYBRDCYCLE:
            cycleKeyboard();
        break;

        case PS2GPIOCYCLE:
            cyclePs2gpio();
        break;

        case PS2CYCLE:
            cyclePs2();
        break;

        case PIXIECYCLE:
            pixiePointer->cyclePixie();
        break;

        case TMSCYCLE:
            tmsPointer->cycleTms();
        break;

        case MC6847CYCLE:
            mc6847Pointer->cycle6847();
        break;

        case MC6845BLINK:
            mc6845Pointer->blink6845();
        break;

        case MC6845CYCLE:
            mc6845Pointer->cycle6845();
        break;

        case I8275CYCLE:
            i8275Pointer->cycle8275();
        break;

        case VT100CYCLE:
            vtPointer->cycleVt();
        break;

        case VTSERIALCYCLE:
            p_Serial->cycleVt();
        break;

        case FDCCYCLE:
            cycleFdc();
        break;

        case IDECYCLE:
            cycleIde();
        break;

        case LEDCYCLE:
            cycleLed();
        break;
    }
}

void Pico::cycleLed()
{
    if (cycleValue_ > 0)
    {
        cycleValue_ --;
        if (cycleValue_ <= 0)
        {
            cycleValue_ = cycleSize_;
            rtcRam_[0xc] |= 0x40;
            p_Main->updateDebugMemory(0xc);
        }
    }
}

void Pico::switchQ(int value)
{
    if (elfConfiguration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (elfConfiguration.vtExternal)
        p_Serial->switchQ(value);
}

void Pico::startComputer()
{
    resetPressed_ = false;

    defineMemoryType(0, 0x7fff, RAM);
    initRam(0, 0x7fff);

    p_Main->assDefault("mycode", 0, 0xfff);
    
    p_Main->checkAndReInstallFile(PICO, "ROM", MAINROM1);

    p_Main->enableDebugGuiMemory();

    Word offset = 0x8000;
    wxString fileName = p_Main->getRomFile(PICO, MAINROM1);
        
    readProgram(p_Main->getRomDir(PICO, MAINROM1), p_Main->getRomFile(PICO, MAINROM1), ROM, offset, NONAME);

    configureExtensions();
    startElfKeyFile("Pico");

    scratchpadRegister_[0] = 0;
    setClear(1);

    if (elfConfiguration.vtType != VTNONE)
        setEf(4,0);

    p_Main->setSwName("");
    p_Main->updateTitle();

    cpuCycles_ = 0;
    instructionCounter_= 0;
    p_Main->startTime();

    loadRtc();
    rtcCycle_ = 4;
    rtcTimerPointer->Start(250, wxTIMER_CONTINUOUS);
    
    if (p_Vt100[UART1] != NULL)
        p_Vt100[UART1]->splashScreen();
    else
        p_Video[VIDEOMAIN]->splashScreen();

    bootstrap_ = 0x8000;

    threadPointer->Run();
}

void Pico::writeMemDataType(Word address, Byte type)
{
    address = address | bootstrap_;
    switch (memoryType_[address/256]&0xff)
    {
        case ROM:
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            increaseExecutedMainMemory(address, type);
        break;

        case RAM:
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            increaseExecutedMainMemory(address, type);
        break;
    }
}

Byte Pico::readMemDataType(Word address, uint64_t* executed)
{
    address = address | bootstrap_;
    switch (memoryType_[address/256]&0xff)
    {
        case ROM:
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;

        case RAM:
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;
    }
    return MEM_TYPE_UNDEFINED;
}

Byte Pico::readMem(Word address)
{
    address_ = address;
    return readMemDebug(address_);
}

Byte Pico::readMemDebug(Word address)
{
    if ((address & 0x8000) == 0x8000)
        bootstrap_ = 0;

    address = address | bootstrap_;

    switch (memoryType_[address/256]&0xff)
    {
        case UNDEFINED:
            return 255;
        break;

        case ROM:
            return mainMemory_[address];
        break;

        case RAM:
            return mainMemory_[address];
        break;

        case MC6847RAM:
            return mc6847Pointer->read6847(address);
        break;

        case MC6845RAM:
            return mc6845Pointer->read6845(address & 0x7ff);
        break;

        case MC6845REGISTERS:
            return mc6845Pointer->readData6845(address);
        break;

        default:
            return 255;
        break;
    }
}

void Pico::writeMem(Word address, Byte value, bool writeRom)
{
    address_ = address;
    writeMemDebug(address_, value, writeRom);
}

void Pico::writeMemDebug(Word address, Byte value, bool writeRom)
{
    address = address | bootstrap_;

    if (loadedOs_ == ELFOS)
    {
        if (address == 0x400 && value >= 4 && value <= 128)
            loadedOs_ = ELFOS_4;
    }
        
    if (emsRamDefined_)
    {
        if (address>=0xc000 && address <=0xffff)
        {
            computerConfiguration.emsConfig_[0].page = value & 0x1f;
        }
    }
    if (elfConfiguration.ioConfiguration.mc6847OutputMode == 1 && address >= 0xff00)
        mc6847Pointer->outMc6847(value);

    switch (memoryType_[address/256]&0xff)
    {
        case MC6847RAM:
            mc6847Pointer->write(address, value);
            mainMemory_[address] = value;
            if (address >= memoryStart_ && address<(memoryStart_ + 256))
                p_Main->updateDebugMemory(address);
        break;

        case MC6845RAM:
            mc6845Pointer->write6845(address & 0x7ff, value);
        break;

        case MC6845REGISTERS:
            mc6845Pointer->writeRegister6845(address, value);
        break;
            
        case UNDEFINED:
        case ROM:
            if (writeRom)
                mainMemory_[address]=value;
        break;

        case RAM:
            if (mainMemory_[address]==value)
                return;
            mainMemory_[address]=value;
            if (address >= memoryStart_ && address<(memoryStart_ + 256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;
    }
}

void Pico::cpuInstruction()
{
    if (cpuMode_ == RUN)
    {
        cpuCycleStep();
    }
    else
    {
        machineCycle();
        machineCycle();
        cpuCycles_ = 0;
        instructionCounter_= 0;
        p_Main->startTime();
    }
}

void Pico::resetPressed()
{
    bootstrap_ = 0x8000;

    p_Main->stopTerminal();
    terminalStop();

    resetCpu();
    if (elfConfiguration.use8275)
        i8275Pointer->cRegWrite(0x40);

    scratchpadRegister_[0] = 0;
    setClear(1);

    resetPressed_ = false;
    elfRunState_ = RESETSTATE;
    p_Main->setSwName("");
    p_Main->eventUpdateTitle();
    startElfKeyFile("Pico");
}

void Pico::configureExtensions()
{
    wxString fileName, fileName2;

    computerConfiguration.numberOfVideoTypes_ = 0;
    if (elfConfiguration.vtType != VTNONE)
    {
        double zoom = p_Main->getZoomVt();
        if (elfConfiguration.vtType == VT52)
            vtPointer = new Vt100("Pico/Elf V2 - VT 52", p_Main->getVtPos(PICO), wxSize(800*zoom, 500*zoom), zoom, PICO, elfClockSpeed_, elfConfiguration, UART1);
        else
            vtPointer = new Vt100("Pico/Elf V2 - VT 100", p_Main->getVtPos(PICO), wxSize(800*zoom, 500*zoom), zoom, PICO, elfClockSpeed_, elfConfiguration, UART1);
        p_Vt100[UART1] = vtPointer;
        vtPointer->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.ioConfiguration);
        if (elfConfiguration.useUart16450)
            configureUart16450(elfConfiguration.ioConfiguration);
        vtPointer->Show(true);
        vtPointer->drawScreen();
    }

    if (elfConfiguration.vtExternal)
    {
        p_Serial = new Serial(PICO, elfClockSpeed_, elfConfiguration);
        p_Serial->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.ioConfiguration);
    }

    if (elfConfiguration.usePixie)
    {
        double zoom = p_Main->getZoom(VIDEOMAIN);
        double scale = p_Main->getScale();
        pixiePointer = new Pixie( "Pico/Elf V2 - Pixie", p_Main->getPixiePos(PICO), wxSize(64*zoom*scale, 128*zoom), zoom, scale, PICO, computerConfiguration.numberOfVideoTypes_);
        p_Video[computerConfiguration.numberOfVideoTypes_++] = pixiePointer;
        pixiePointer->configurePixie(elfConfiguration.ioConfiguration);
        pixiePointer->initPixie();
        pixiePointer->setZoom(zoom);
        pixiePointer->Show(true);
    }

    if (elfConfiguration.use6845)
    {
        double zoom = p_Main->getZoom(VIDEOMAIN);
        mc6845Pointer = new MC6845( "Pico/Elf V2 - MC6845", p_Main->get6845Pos(PICO), wxSize(64*8*zoom, 16*8*2*zoom), zoom, PICO, elfClockSpeed_, 8, elfConfiguration.ioConfiguration, computerConfiguration.numberOfVideoTypes_);
        p_Video[computerConfiguration.numberOfVideoTypes_++] = mc6845Pointer;
        mc6845Pointer->configure6845(elfConfiguration.ioConfiguration);
        mc6845Pointer->init6845();
        mc6845Pointer->Show(true);
    }

    if (elfConfiguration.useS100)
    {
        double zoom = p_Main->getZoom(VIDEOMAIN);
        mc6845Pointer = new MC6845( "Pico/Elf V2 - Quest Super Video", p_Main->get6845Pos(PICO), wxSize(64*7*zoom, 16*9*zoom), zoom, PICO, elfClockSpeed_, 7, elfConfiguration.ioConfiguration, computerConfiguration.numberOfVideoTypes_);
        p_Video[computerConfiguration.numberOfVideoTypes_++] = mc6845Pointer;
        mc6845Pointer->configureSuperVideo();
        mc6845Pointer->init6845();
        mc6845Pointer->Show(true);
    }

    if (elfConfiguration.use8275)
    {
        double zoom = p_Main->getZoom(VIDEOMAIN);
        i8275Pointer = new i8275( "Pico/Elf V2 - Intel 8275", p_Main->get8275Pos(PICO), wxSize(80*8*zoom, 24*10*2*zoom), zoom, PICO, elfClockSpeed_, computerConfiguration.numberOfVideoTypes_);
        p_Video[computerConfiguration.numberOfVideoTypes_++] = i8275Pointer;
        i8275Pointer->configure8275(elfConfiguration.ioConfiguration);
        i8275Pointer->init8275();
        i8275Pointer->Show(true);
    }

    if (elfConfiguration.use6847)
    {
        double zoom = p_Main->getZoom(VIDEOMAIN);
        mc6847Pointer = new mc6847( "Pico/Elf V2 - MC6847", p_Main->get6847Pos(PICO), wxSize(elfConfiguration.charLine*8*zoom, elfConfiguration.screenHeight6847*zoom), zoom, PICO, elfClockSpeed_, elfConfiguration.ioConfiguration, computerConfiguration.numberOfVideoTypes_);
        p_Video[computerConfiguration.numberOfVideoTypes_++] = mc6847Pointer;
        mc6847Pointer->configure(elfConfiguration.ioConfiguration);
        mc6847Pointer->init6847();
        mc6847Pointer->Show(true);
    }

    if (elfConfiguration.useTMS9918)
    {
        double zoom = p_Main->getZoom(VIDEOMAIN);
        tmsPointer = new Tms9918( "Pico/Elf V2 - TMS 9918", p_Main->getTmsPos(PICO), wxSize(320*zoom,240*zoom), zoom, PICO, elfClockSpeed_, computerConfiguration.numberOfVideoTypes_);
        p_Video[computerConfiguration.numberOfVideoTypes_++] = tmsPointer;
        tmsPointer->configure(elfConfiguration.ioConfiguration);
        tmsPointer->Show(true);
    }

    if (elfConfiguration.fdc1793Enabled)
    {
        configure1793(1, 40, 18, 256, 6256, PICO, elfConfiguration.ioConfiguration, true);
        resetFdc();
    }

    if (elfConfiguration.ideEnabled)
    {
        configureIde(p_Main->getIdeDir(PICO) + p_Main->getIdeFile(PICO), p_Main->getIdeDir(PICO) + "disk2.ide", elfConfiguration.ioConfiguration);
    }

    if (p_Main->getPrinterStatus(PICO))
    {
        p_Printer = new Printer();
        p_Printer->configureBasicPrinter(elfConfiguration.ioConfiguration);
        p_Printer->init(p_Printer, PRINTER_BASIC);
    }

    setQsound (elfConfiguration.qSound_);

    if (elfConfiguration.useKeyboard)
        configureKeyboard(PICO, elfConfiguration.ioConfiguration);

    if (elfConfiguration.usePS2)
        configurePs2(elfConfiguration.ps2Interrupt, elfConfiguration.ioConfiguration);
}

void Pico::moveWindows()
{
    if (elfConfiguration.usePixie)
        pixiePointer->Move(p_Main->getPixiePos(PICO));
    if (elfConfiguration.useTMS9918)
        tmsPointer->Move(p_Main->getTmsPos(PICO));
    if (elfConfiguration.use6845||elfConfiguration.useS100)
        mc6845Pointer->Move(p_Main->get6845Pos(PICO));
    if (elfConfiguration.use6847)
        mc6847Pointer->Move(p_Main->get6847Pos(PICO));
    if (elfConfiguration.use8275)
        i8275Pointer->Move(p_Main->get8275Pos(PICO));
    if (elfConfiguration.vtType != VTNONE)
        vtPointer->Move(p_Main->getVtPos(PICO));
}

void Pico::updateTitle(wxString Title)
{
    if (elfConfiguration.usePixie)
        pixiePointer->SetTitle("Pico/Elf V2 - Pixie"+Title);
    if (elfConfiguration.useTMS9918)
        tmsPointer->SetTitle("Pico/Elf V2 - TMS 9918"+Title);
    if (elfConfiguration.use6845)
        mc6845Pointer->SetTitle("Pico/Elf V2 - MC6845"+Title);
    if (elfConfiguration.useS100)
        mc6845Pointer->SetTitle("Pico/Elf V2 - Quest Super Video"+Title);
    if (elfConfiguration.use6847)
        mc6847Pointer->SetTitle("Pico/Elf V2 - MC6847"+Title);
    if (elfConfiguration.use8275)
        i8275Pointer->SetTitle("Pico/Elf V2 - Intel 8275"+Title);
    if (elfConfiguration.vtType == VT52)
        vtPointer->SetTitle("Pico/Elf V2 - VT 52"+Title);
    if (elfConfiguration.vtType == VT100)
        vtPointer->SetTitle("Pico/Elf V2 - VT 100"+Title);
}

void Pico::setForceUpperCase(bool status)
{
    if (elfConfiguration.vtType != VTNONE)
        vtPointer->setForceUCVt(status);
    if (elfConfiguration.useKeyboard)
        setForceUpperCaseKeyboard(status);
}

void Pico::onReset()
{
    resetPressed_ = true;
}

void Pico::sleepComputer(long ms)
{
    threadPointer->Sleep(ms);
}

Byte Pico::read6845CharRom(Word address)
{
    if (elfConfiguration.use6845||elfConfiguration.useS100)
        return mc6845Pointer->read6845CharRom(address);
    else
        return 0;
}

void Pico::write6845CharRom(Word address, Byte value)
{
    if (elfConfiguration.use6845||elfConfiguration.useS100)
        mc6845Pointer->write6845CharRom(address, value);
}

Byte Pico::read8275CharRom(Word address)
{
    if (elfConfiguration.use8275)
        return i8275Pointer->read8275CharRom(address);
    else
        return 0;
}

void Pico::write8275CharRom(Word address, Byte value)
{
    if (elfConfiguration.use8275)
        i8275Pointer->write8275CharRom(address, value);
}

Byte Pico::read8275VideoRam(Word address)
{
    if (elfConfiguration.use8275)
        return i8275Pointer->read8275VideoRam(address);
    else
        return 0;
}

void Pico::write8275VideoRam(Word address, Byte value)
{
    if (elfConfiguration.use8275)
        i8275Pointer->write8275VideoRam(address, value);
}

Byte Pico::read6847CharRom(Word address)
{
    if (elfConfiguration.use6847)
        return mc6847Pointer->read6847CharRom(address);
    else
        return 0;
}

void Pico::write6847CharRom(Word address, Byte value)
{
    if (elfConfiguration.use6847)
        mc6847Pointer->write6847CharRom(address, value);
}

int Pico::readDirect6847(Word address)
{
    if (elfConfiguration.use6847)
        return mc6847Pointer->readDirect6847(address);
    else
        return 0;
}

void Pico::writeDirect6847(Word address, int value)
{
    if (elfConfiguration.use6847)
        mc6847Pointer->writeDirect6847(address, value);
}

Word Pico::get6847RamMask()
{
     if (elfConfiguration.use6847)
        return mc6847Pointer->get6847RamMask();
    else
        return 0;
}

void Pico::activateMainWindow()
{
    bool maximize = IsMaximized();
    Iconize(false);
    Raise();
    Show(true);
    Maximize(maximize);
}

void Pico::OnRtcTimer(wxTimerEvent&WXUNUSED(event))
{
    rtcCycle_--;

    if (rtcCycle_ == 1)
        rtcRam_[0xa] |= 0x80;

    if (rtcCycle_ > 0)
        return;
    
    rtcRam_[0xa] &= 0x7f;
    rtcCycle_ = 4;

    wxDateTime now = wxDateTime::Now();
    
    writeRtc(0, now.GetSecond());
    writeRtc(2, now.GetMinute());
    writeRtc(4, now.GetHour());
    writeRtc(6, now.GetWeekDay());
    writeRtc(7, now.GetDay());
    writeRtc(8, now.GetMonth()+1);
    writeRtc(9, now.GetYear()-1972);

    rtcRam_[0xc] |= 0x10;
    p_Main->updateDebugMemory(0xa);
    p_Main->updateDebugMemory(0xc);
}

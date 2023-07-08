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

#include "hbelf.h"


ElfScreen::ElfScreen(wxWindow *parent, const wxSize& size, int tilType)
: Panel(parent, size, tilType)
{
}

ElfScreen::~ElfScreen()
{
    delete mainBitmapPointer;

    delete runSwitchButton;
    delete mpSwitchButton;
    delete powerSwitchButton;
    delete loadSwitchButton;
    delete inSwitchButton;

    for (int i=0; i<8; i++)
    {
        delete dataSwitchButton[i];
    }
    for (int i=0; i<4; i++)
    {
        delete efSwitchButton[i];
    }

    for (int i=0; i<4; i++)
        delete addressPointer[i];
        
    for (int i=0; i<2; i++)
        delete dataPointer[i];
    
     delete qLedPointer;
}

void ElfScreen::init()
{
    keyStart_ = 0;
    keyEnd_ = 0;
    lastKey_ = 0;
    forceUpperCase_ = p_Main->getUpperCase();

    wxClientDC dc(this);

    mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/elf.png", wxBITMAP_TYPE_PNG);

    runSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_DOWN, 190, 312, "");
    mpSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_DOWN, 150, 312, "");
    powerSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_DOWN, 313, 312, "");
    loadSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_DOWN, 75, 312, "");
    inSwitchButton = new SwitchButton(dc, PUSH_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_UP, 32, 312, "");
    
    qLedPointer = new Led(dc, 324, 250, ELFLED);
    updateQLed_ = true;

    for (int i=0; i<8; i++)
    {
        dataSwitchButton[i] = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_DOWN, 18+30*(7-i), 362, "");
    }
    for (int i=0; i<4; i++)
    {
        efSwitchButton[i] = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0x31, 0x31, 0x30), BUTTON_DOWN, 138+30*(3-i), 422, "");
    }
    for (int i=0; i<4; i++)
    {
        if (tilType_ == TIL311)
            addressPointer[i] = new Til311();
        else
            addressPointer[i] = new Til313Italic(false);

        addressPointer[i]->init(dc, 18+i*40, 226);
        updateAddress_ = true;
    }
    for (int i=0; i<2; i++)
    {
        if (tilType_ == TIL311)
            dataPointer[i] = new Til311();
        else
            dataPointer[i] = new Til313Italic(false);
        
        dataPointer[i]->init(dc, 218+i*40, 226);
        updateData_ = true;
    }
    this->connectKeyEvent(this);
}

void ElfScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.DrawBitmap(*mainBitmapPointer, 0, 0);

#if defined(__WXMAC__)
    rePaintLeds(dc);
#endif
        
    for (int i=0; i<4; i++)
    {
        efSwitchButton[i]->onPaint(dc);
    }

    for (int i=0; i<4; i++)
        addressPointer[i]->onPaint(dc);
    for (int i=0; i<2; i++)
        dataPointer[i]->onPaint(dc);

    qLedPointer->onPaint(dc);

    inSwitchButton->onPaint(dc);
    runSwitchButton->onPaint(dc);
    mpSwitchButton->onPaint(dc);
    powerSwitchButton->onPaint(dc);
    loadSwitchButton->onPaint(dc);
    for (int i=0; i<8; i++)
    {
        dataSwitchButton[i]->onPaint(dc);
    }
}

void ElfScreen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);

    wxClientDC dc(this);

    if (inSwitchButton->onMousePress(dc, x, y))
        p_Computer->onInButtonPress(p_Computer->getData());
}

void ElfScreen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);

    wxClientDC dc(this);

    if (inSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onInButtonRelease();
    if (runSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onRun();
    if (mpSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onMpButton();
    if (powerSwitchButton->onMouseRelease(dc, x, y))
        p_Main->stopComputer();
    if (loadSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onLoadButton();
    for (int i=0; i<8; i++)
    {
        if (dataSwitchButton[i]->onMouseRelease(dc, x, y))
            p_Computer->dataSwitch(i);
    }
    for (int i=0; i<4; i++)
    {
        if (efSwitchButton[i]->onMouseRelease(dc, x, y))
            p_Computer->efSwitch(i);
    }
}

BEGIN_EVENT_TABLE(Elf, wxFrame)
    EVT_CLOSE (Elf::onClose)
    EVT_TIMER(900, Elf::OnRtcTimer)
END_EVENT_TABLE()

Elf::Elf(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf, Conf computerConf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    computerConfiguration = computerConf;
    elfConfiguration = conf;
    elfClockSpeed_ = clock;

#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    this->SetClientSize(size);

    elfScreenPointer = new ElfScreen(this, size, elfConfiguration.tilType);
    elfScreenPointer->init();
    
    rtcTimerPointer = new wxTimer(this, 900);
    cycleValue_ = -1;
    cycleSize_ = -1;

    runningGame_ = "";
}

Elf::~Elf()
{
    saveRtc();
    rtcTimerPointer->Stop();
    delete rtcTimerPointer;
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
    {
        p_Main->setKeypadPos(ELF, keypadPointer->GetPosition());
        keypadPointer->Destroy();
    }
    if (elfConfiguration.useLedModule && !ledModuleClosed_)
    {
        p_Main->setLedModulePos(ledModulePointer->GetPosition());
        ledModulePointer->Destroy();
    }
    if (elfConfiguration.usePixie)
    {
        p_Main->setPixiePos(ELF, pixiePointer->GetPosition());
        pixiePointer->Destroy();
    }
    if (elfConfiguration.useTMS9918)
    {
        p_Main->setTmsPos(ELF, tmsPointer->GetPosition());
        tmsPointer->Destroy();
    }
    if (elfConfiguration.use6845||elfConfiguration.useS100)
    {
        p_Main->set6845Pos(ELF, mc6845Pointer->GetPosition());
        mc6845Pointer->Destroy();
    }
    if (elfConfiguration.use6847)
    {
        p_Main->set6847Pos(ELF, mc6847Pointer->GetPosition());
        mc6847Pointer->Destroy();
    }
    if (elfConfiguration.use8275)
    {
        p_Main->set8275Pos(ELF, i8275Pointer->GetPosition());
        i8275Pointer->Destroy();
    }
    if (elfConfiguration.vtType != VTNONE)
    {
        p_Main->setVtPos(ELF, vtPointer->GetPosition());
        vtPointer->Destroy();
    }
    if (elfConfiguration.vtExternal)
        delete p_Serial;
    if (p_Main->getPrinterStatus(ELF))
    {
        p_Printer->closeFrames();
        delete p_Printer;
    }
    p_Main->setMainPos(ELF, GetPosition());

    delete elfScreenPointer;
}

void Elf::showModules(bool status)
{
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->Show(status);
    if (elfConfiguration.useLedModule && !ledModuleClosed_)
        ledModulePointer->Show(status);
}

void Elf::onClose(wxCloseEvent&WXUNUSED(event) )
{
    p_Main->stopComputer();
}

void Elf::charEvent(int keycode)
{
    if (elfConfiguration.useKeyboard)
        charEventKeyboard(keycode);
}

bool Elf::keyDownPressed(int key)
{
    onHexKeyDown(key);
    if (elfConfiguration.usePS2)
    {
        keyDownPs2(key);
            return true;
    }
    return false;
}

bool Elf::keyUpReleased(int key, wxKeyEvent&WXUNUSED(event))
{
    if (key == inKey1_ || key == inKey2_)
    {
        onInButtonRelease();
        return true;
    }
    onHexKeyUp(key);
    if (elfConfiguration.useKeyboard)
        keyboardUp();
    if (elfConfiguration.usePS2)
    {
        keyUpPs2(key);
        return true;
    }
    return false;
}

void Elf::onInButtonPress()
{
    if (elfConfiguration.useHexKeyboard)
        onInButtonPress(switches_);
    else
        onInButtonPress(getData());
}

void Elf::onInButtonPress(Byte value)
{
    inPressed_ = true;
    if (cpuMode_ == LOAD)
    {
        dmaIn(value);
        showData(value);
    }
    else
    {
        efSwitchState_[3] = 1;
    }
    elfScreenPointer->inSetState(BUTTON_DOWN);
}

void Elf::onInButtonRelease()
{
    if (efSwitchState_[3] == 1)
    {
        inPressed_ = false;
        efSwitchState_[3] = 0;
    }
    elfScreenPointer->inSetState(BUTTON_UP);
}

void Elf::onHexKeyDown(int keycode)
{
#if defined (__WXMAC__)
    if (ef3State_ == 0) // This is to avoid multiple key presses on OSX
        return;
#endif
    
//    if (!elfConfiguration.useHexKeyboard)
//        return;
    for (int i=0; i<16; i++)
    {
        if (keycode == keyDefA1_[i])
        {
            ef3State_ = 0;
            switches_ = ((switches_ << 4) & 0xf0) | i;
            if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                keypadPointer->onNumberDown(i);
        }
        if (keycode == keyDefA2_[i])
        {
            ef3State_ = 0;
            switches_ = ((switches_ << 4) & 0xf0) | i;
            if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                keypadPointer->onNumberDown(i);
        }
    }
}

void Elf::onHexDown(int hex)
{
    ef3State_ = 0;
    switches_ = ((switches_ << 4) & 0xf0) | hex;
}

void Elf::onHexKeyUp(int keycode)
{
    if (elfConfiguration.useHexKeyboard)
    {
        for (int i=0; i<5; i++)
        {
            if (keyDefGameHexA_[i] <= 15)
            {
                if (keycode == keyDefA2_[keyDefGameHexA_[i]])
                    keyboardUp();
            }
        }
    }
        
    ef3State_ = 1;
}

void Elf::configureComputer()
{
    efType_[0][0][1] = EF1UNDEFINED;
    efType_[0][0][2] = EF2UNDEFINED;
    efType_[0][0][3] = EF3UNDEFINED;

    wxString printBuffer;

    setCycleType(COMPUTERCYCLE, LEDCYCLE);
    p_Main->message("Configuring Elf");
    printBuffer.Printf("    Output %d: display output, input %d: data input", elfConfiguration.ioConfiguration.hexOutput.portNumber, elfConfiguration.ioConfiguration.hexInput.portNumber);
    p_Main->message(printBuffer);

    p_Computer->setInType(elfConfiguration.ioConfiguration.hexInput.portNumber, ELFIN);
    p_Computer->setOutType(elfConfiguration.ioConfiguration.hexOutput.portNumber, ELFOUT);

    if (elfConfiguration.useEms)
    {
        if (elfConfiguration.emsType_ == RAM)
        {
            printBuffer.Printf("    Address C000-FFFF: EMS-512KB page select");
            p_Main->message(printBuffer);
        }
 
        printBuffer.Printf("    Output %d: EMS-512KB page select", elfConfiguration.ioConfiguration.emsOutput[0]);
        p_Computer->setOutType(elfConfiguration.ioConfiguration.emsOutput[0], EMSMAPPEROUT);
        p_Main->message(printBuffer);
    }
    if (elfConfiguration.useTape  && !elfConfiguration.useXmodem)
    {
        efType_[0][0][elfConfiguration.ioConfiguration.tapeEf] = ELF2EF2;
        printBuffer.Printf("    EF %d: cassette in", elfConfiguration.ioConfiguration.tapeEf);
        p_Main->message(printBuffer);
    }
    if (elfConfiguration.useHexKeyboardEf3)
    {
        printBuffer.Printf("    EF %d: 0 when hex button pressed", elfConfiguration.ioConfiguration.hexEf);
        p_Main->message(printBuffer);
    }

    p_Main->message("    EF 4: 0 when in button pressed");

    p_Main->message("");

    inKey1_ = p_Main->getDefaultInKey1("Elf");
    inKey2_ = p_Main->getDefaultInKey2("Elf");
    p_Main->getDefaultHexKeys(ELF, "Elf", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);

    if (p_Main->getConfigBool("/Elf/GameAuto", true))
        p_Main->loadKeyDefinition(p_Main->getRomFile(ELF, MAINROM1), p_Main->getRomFile(ELF, MAINROM2), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
    
    resetCpu();
}

void Elf::switchHexEf(bool state)
{
    elfConfiguration.useHexKeyboardEf3 = state;
}

void Elf::setPrinterEf()
{
    ef3State_ = 0;
}

void Elf::reLoadKeyDefinition(wxString fileName)
{
    runningGame_ = fileName;
    if (p_Main->getConfigBool("/Elf/GameAuto", true))
        p_Main->loadKeyDefinition("", fileName, keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
}

void Elf::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefA1_[i] = hexKeyDefA1[i];
        keyDefA2_[i] = hexKeyDefA2[i];
    }
}

void Elf::reDefineKeysB(int hexKeyDefB1[], int hexKeyDefB2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefB1_[i] = hexKeyDefB1[i];
        keyDefB2_[i] = hexKeyDefB2[i];
    }
}

void Elf::initComputer()
{
    Show(p_Main->getUseElfControlWindows(ELF));
    runButtonState_ = 0;
    for (int i=0; i<8; i++)  dataSwitchState_[i]=0;
    for (int i=0; i<4; i++)  efSwitchState_[i]=0;
    inPressed_ = false;
    mpButtonState_ = 0;
    loadButtonState_ = 1;
    cassetteEf_ = 0;

    switches_ = 0;
    ef3State_ = 1;
    hexKeypadClosed_ = false;
    ledModuleClosed_ = false;
    elfRunState_ = RESETSTATE;
    bootstrap_ = 0;
}

Byte Elf::ef(int flag)
{
    if (flag == 4)
    {
        if (inPressed_ == true)
            return 0;
    }
    if (elfConfiguration.useHexKeyboardEf3)
    {
        if (flag == elfConfiguration.ioConfiguration.hexEf)
            return ef3State_;
    }
    switch(efType_[0][0][flag])
    {
        case 0:
            return 1-efSwitchState_[flag-1];
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

        case FDC1793_EF:
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

        case BASIC_PRINT_EF:
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

Byte Elf::in(Byte port, Word WXUNUSED(address))
{
    Byte ret;

    switch(inType_[0][0][port])
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

        case ELFIN:
            if (elfConfiguration.useHexKeyboard)
                ret = switches_;
            else
                ret = getData();
        break;

        case FDC1793_READIN:
            ret = in1793();
        break;

        case IDEIN:
            ret = inIde();
        break;

        case PORTEXTIN:
            ret = inPortExtender();
        break;

        case UARTIN:
            ret = vtPointer->uartIn();
        break;

        case UARTINSERIAL:
            ret = p_Serial->uartIn();
        break;

        case UARTSTATUS:
            ret = vtPointer->uartStatus();
        break;

        case UARTSTATUSSERIAL:
            ret = p_Serial->uartStatus();
        break;

        case ELF2KDISKREADREGISTER:
            ret = inDisk();
        break;

        case ELF2KDISKREADSTATUS:
            ret = readDiskStatus();
        break;

        default:
            ret = 255;
    }
    inValues_[port] = ret;
    return ret;
}

Byte Elf::getData()
{
    return(dataSwitchState_[7]?128:0) +(dataSwitchState_[6]?64:0) +(dataSwitchState_[5]?32:0) +(dataSwitchState_[4]?16:0) +(dataSwitchState_[3]?8:0) +(dataSwitchState_[2]?4:0) +(dataSwitchState_[1]?2:0) +(dataSwitchState_[0]?1:0);
}

void Elf::out(Byte port, Word WXUNUSED(address), Byte value)
{
    outValues_[port] = value;

    switch(outType_[0][0][port])
    {
        case 0:
            return;
        break;

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

        case BASIC_PRINT_OUT:
//            p_Main->eventPrintDefault(value); *** Maybe this was needed on Mac or Linux? however it doesn't work on windows, i.e. no printer window will be opened.
            if ((value & 0xfc) != 0)
                p_Printer->printerOut(value);
        break;

        case LEDMODOUT:
            ledModulePointer->write(value);
        break;

        case PS2OUT:
            outPs2(value);
        break;

        case ELFOUT:
            showData(value);
        break;

        case FDC1793_SELECTOUT:
            selectRegister1793(value);
        break;

        case FDC1793_WRITEOUT:
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
            
        case EMSMAPPEROUT:
            setEmsPage(0, value);
        break;

        case ELF2KDISKSELECTREGISTER:
            selectDiskRegister(value);
        break;

        case ELF2KDISKWRITEREGISTER:
            outDisk(value);
        break;
    }
}

void Elf::showData(Byte val)
{
    elfScreenPointer->showData(val);
}

void Elf::cycle(int type)
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
//            cycleDisk(); not sure why this was here, looks wrong; maybe ended up here when trying to get Elf2K uart to work on Pico?
        break;

        case LEDCYCLE:
            cycleLed();
        break;
    }
}

void Elf::cycleLed()
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
    if (ledCycleValue_ > 0)
    {
        ledCycleValue_ --;
        if (ledCycleValue_ <= 0)
        {
            ledCycleValue_ = ledCycleSize_;
            elfScreenPointer->ledTimeout();
            if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                keypadPointer->ledTimeout();
            if (elfConfiguration.useLedModule && !ledModuleClosed_)
                ledModulePointer->ledTimeout();
        }
    }
}

void Elf::autoBoot()
{
    elfScreenPointer->runSetState(BUTTON_UP);
    runButtonState_ = 1;
    setClear(runButtonState_);
    if (cpuMode_ == RESET)  
    {
        elfScreenPointer->showAddress(0);
    }
}

void Elf::switchQ(int value)
{
    elfScreenPointer->setQLed(value);

    if (elfConfiguration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (elfConfiguration.vtExternal)
        p_Serial->switchQ(value);
}

int Elf::getMpButtonState()
{
    return mpButtonState_;
}

void Elf::onRun()
{
    if (elfConfiguration.bootStrap)
        bootstrap_ = 0x8000;

    if (runButtonState_)
    {
        elfScreenPointer->runSetState(BUTTON_DOWN);
        runButtonState_ = 0;
    }
    else
    {
        elfScreenPointer->runSetState(BUTTON_UP);
        runButtonState_ = 1;
        p_Main->startTime();
    }
    setClear(runButtonState_);
    p_Main->eventUpdateTitle();
    if (cpuMode_ == RESET)      
    {
        elfScreenPointer->showAddress(0);
    }
}

void Elf::onMpButton()
{
    if (mpButtonState_)
    {
        mpButtonState_ = 0;
    }
    else
    {
        mpButtonState_ = 1;
    }
}

void Elf::onLoadButton()
{
    if (!loadButtonState_)
    {
        loadButtonState_ = 1;
    }
    else
    {
        loadButtonState_ = 0;
    }
    setWait(loadButtonState_);
    if (cpuMode_ == RESET)  
    {
        elfScreenPointer->showAddress(0);
    }
}

void Elf::dataSwitch(int i)
{
    if (dataSwitchState_[i])
    {
        dataSwitchState_[i] = 0;
    }
    else
    {
        dataSwitchState_[i] = 1;
    }
}

void Elf::efSwitch(int i)
{
    if (efSwitchState_[i])
    {
        efSwitchState_[i] = 0;
    }
    else
    {
        efSwitchState_[i] = 1;
    }
    setEf(i+1, 1-efSwitchState_[i]);
}

void Elf::startComputer()
{
    resetPressed_ = false;

    if (elfConfiguration.usePortExtender)
        configurePortExt(elfConfiguration.ioConfiguration);

    ramStart_ = p_Main->getStartRam("Elf", ELF);
    Word ramEnd = p_Main->getEndRam("Elf", ELF);
    ramMask_ = 0x100;
    while (ramMask_ < (ramEnd + 1 - ramStart_))
        ramMask_ *= 2;
    ramMask_ -= 1;

    defineMemoryType(ramStart_, ramEnd, RAM);
    initRam(ramStart_, ramEnd);

    p_Main->assDefault("mycode", ramStart_&0xff00, ((ramEnd&0xff00)|0xff)&0xfff);
    
    if (p_Main->getLoadromMode(ELF, 0) == ROM)
        p_Main->checkAndReInstallFile(ELF, "ROM 1", MAINROM1);

    for (int i=ramMask_ + 1 + ramStart_; i<0x10000; i+=(ramMask_+1))
        defineMemoryType(i, i+(ramEnd - ramStart_), MAPPEDRAM);

    if (elfConfiguration.usePager)
    {
        allocPagerMemory();
        definePortExtForPager();

        defineMemoryType(0, 65535, PAGER);
    }
    if (elfConfiguration.useEms)
    {
        emsMemory_.resize(1);
        computerConfiguration.emsConfig_.resize(1);
        computerConfiguration.emsConfigNumber_ = 1;
        
        if (elfConfiguration.emsType_ == RAM)
        {
            computerConfiguration.emsConfig_[0].start = 0x8000;
            computerConfiguration.emsConfig_[0].end = 0xBFFF;
            computerConfiguration.emsConfig_[0].mask = 0x3FFF;
            computerConfiguration.emsConfig_[0].outputMask = 0x1f;
            computerConfiguration.emsConfig_[0].maskBits = 14;

            allocEmsMemorySegment(0);
            defineMemoryType(0x8000, 0xbfff, EMSMEMORY);
        }
        else
        {
            computerConfiguration.emsConfig_[0].start = 0x8000;
            computerConfiguration.emsConfig_[0].end = 0xFFFF;
            computerConfiguration.emsConfig_[0].mask = 0xFFFF;
            computerConfiguration.emsConfig_[0].outputMask = 0xFf;
            computerConfiguration.emsConfig_[0].maskBits = 15;

            readRomMapperBinFile(0, p_Main->getRomDir(ELF, MAINROM1)+p_Main->getRomFile(ELF, MAINROM1));
            defineMemoryType(0x8000, 0xffff, EMSMEMORY);
        }
    }
    
    p_Main->enableDebugGuiMemory();
    
    Word offset = 0;
    wxString fileName = p_Main->getRomFile(ELF, MAINROM1);
    if (fileName.Right(4) == ".ch8")
        offset = 0x200;
    
    if (elfConfiguration.bootStrap)
        offset = 0x8000;
    if (!elfConfiguration.useEms)
        readProgram(p_Main->getRomDir(ELF, MAINROM1), p_Main->getRomFile(ELF, MAINROM1), p_Main->getLoadromMode(ELF, 0), offset, NONAME);
  
    fileName = p_Main->getRomFile(ELF, MAINROM2);
    if (fileName.Right(4) == ".ch8")
        offset = 0x200;
    else
        offset = 0;
    readProgram(p_Main->getRomDir(ELF, MAINROM2), p_Main->getRomFile(ELF, MAINROM2), p_Main->getLoadromMode(ELF, 1), offset, NONAME);

    configureExtensions();
    startElfKeyFile("Elf");
    if (elfConfiguration.autoBoot)
    {
        scratchpadRegister_[0]=p_Main->getBootAddress("Elf", ELF);
        autoBoot();
    }

    if (elfConfiguration.vtType != VTNONE)
        setEf(4,0);

    p_Main->setSwName("");
    p_Main->updateTitle();
    address_ = 0;

    elfScreenPointer->showAddress(0);

    cpuCycles_ = 0;
    instructionCounter_= 0;
    p_Main->startTime();

    loadRtc();
    rtcCycle_ = 4;
    rtcTimerPointer->Start(250, wxTIMER_CONTINUOUS);

    int ms = (int) p_Main->getLedTimeMs(ELF);
    elfScreenPointer->setLedMs(ms);
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->setLedMs(ms);
    if (elfConfiguration.useLedModule && !ledModuleClosed_)
        ledModulePointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((elfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
    
    if (p_Vt100[UART1] != NULL)
        p_Vt100[UART1]->splashScreen();
    else
        p_Video[VIDEOMAIN]->splashScreen();

    if (elfConfiguration.bootStrap)
        bootstrap_ = 0x8000;

    threadPointer->Run();
}

void Elf::writeMemDataType(Word address, Byte type)
{
    address = address | bootstrap_;
    switch (memoryType_[address/256]&0xff)
    {
        case EMSMEMORY:
            switch (emsMemory_[0].memoryType_[((address & 0x7fff) |(computerConfiguration.emsConfig_[0].page << 15))/256])
            {
                case ROM:
                case RAM:
                    if (emsMemory_[0].dataType_[(long) ((address & 0x7fff) |(computerConfiguration.emsConfig_[0].page << 15))] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        emsMemory_[0].dataType_[(long) ((address & 0x7fff) |(computerConfiguration.emsConfig_[0].page << 15))] = type;
                    }
                    increaseExecutedEms(0, (long) ((address & 0x7fff) |(computerConfiguration.emsConfig_[0].page << 15)), type);
                break;
            }
        break;
            
        case ROM:
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address]=type;
            }
            increaseExecutedMainMemory(address, type);
        break;

        case MAPPEDRAM:
        case RAM:
            address = (address & ramMask_) + ramStart_;
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            increaseExecutedMainMemory(address, type);
        break;

        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>12) << 12) |(address &0xfff))/256])
            {
                case ROM:
                case RAM:
                    if (pagerMemoryDataType_[(getPager(address>>12) << 12) |(address &0xfff)] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        pagerMemoryDataType_[(getPager(address>>12) << 12) |(address &0xfff)] = type;
                    }
                    increaseExecutedMainMemory((getPager(address>>12) << 12) |(address &0xfff), type);
                break;
            }
        break;

    }
}

Byte Elf::readMemDataType(Word address, uint64_t* executed)
{
    address = address | bootstrap_;

    switch (memoryType_[address/256]&0xff)
    {
        case EMSMEMORY:
            switch (emsMemory_[0].memoryType_[((address & 0x7fff) |(computerConfiguration.emsConfig_[0].page << 15))/256])
            {
                case ROM:
                case RAM:
                    if (profilerCounter_ != PROFILER_OFF)
                        *executed = emsMemory_[0].executed_[(long) ((address & 0x7fff) |(computerConfiguration.emsConfig_[0].page << 15))];
                    return emsMemory_[0].dataType_[(long) ((address & 0x7fff) |(computerConfiguration.emsConfig_[0].page << 15))];
                break;
            }
       break;
            
        case ROM:
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;

        case MAPPEDRAM:
        case RAM:
            address = (address & ramMask_) + ramStart_;
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;

        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>12) << 12) |(address &0xfff))/256])
            {
                case ROM:
                case RAM:
                    if (profilerCounter_ != PROFILER_OFF)
                        *executed = pagerMemoryExecuted_[(getPager(address>>12) << 12) |(address &0xfff)];
                    return pagerMemoryDataType_[(getPager(address>>12) << 12) |(address &0xfff)];
                break;
            }
        break;
    }
    return MEM_TYPE_UNDEFINED;
}

Byte Elf::readMem(Word address)
{
    address_ = address;
    return readMemDebug(address_);
}

Byte Elf::readMemDebug(Word address)
{
    if ((address & 0x8000) == 0x8000)
        bootstrap_ = 0;

    address = address | bootstrap_;

    elfScreenPointer->showAddress(address);

    switch (memoryType_[address/256]&0xff)
    {
        case EMSMEMORY:
            switch (emsMemory_[0].memoryType_[((address & 0x7fff) |(computerConfiguration.emsConfig_[0].page << 15))/256])
            {
                case UNDEFINED:
                    return 255;
                break;
            
                case ROM:
                case RAM:
                    return emsMemory_[0].mainMem[(long) ((address & 0x7fff) |(computerConfiguration.emsConfig_[0].page << 15))];
                break;
            
                default:
                    return 255;
                break;
            }
        break;
            
        case UNDEFINED:
            return 255;
        break;

        case ROM:
            return mainMemory_[address];
        break;

        case MAPPEDRAM:
        case RAM:
            address = (address & ramMask_) + ramStart_;
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

        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>12) << 12) |(address &0xfff))/256])
            {
                case UNDEFINED:
                    return 255;
                break;

                case ROM:
                case RAM:
                    return pagerMemory_[(getPager(address>>12) << 12) |(address &0xfff)];
                break;

                default:
                    return 255;
                break;
            }
        break;

        default:
            return 255;
        break;
    }
}

void Elf::writeMem(Word address, Byte value, bool writeRom)
{
    address_ = address;
    writeMemDebug(address_, value, writeRom);
}

void Elf::writeMemDebug(Word address, Byte value, bool writeRom)
{
    address = address | bootstrap_;

    if (loadedOs_ == ELFOS)
    {
        if (address == 0x400 && value >= 4 && value <= 128)
            loadedOs_ = ELFOS_4;
    }
        
    elfScreenPointer->showAddress(address);

    if (emsRamDefined_)
    {
        if (address>=0xc000 && address <=0xffff)
        {
            computerConfiguration.emsConfig_[0].page = value & 0x1f;
        }
    }

    switch (memoryType_[address/256]&0xff)
    {
        case EMSMEMORY:
            switch (emsMemory_[0].memoryType_[((address & 0x7fff) |(computerConfiguration.emsConfig_[0].page << 15))/256])
            {
                case UNDEFINED:
                case ROM:
                    if (writeRom)
                        emsMemory_[00].mainMem[(long) ((address & 0x7fff) |(computerConfiguration.emsConfig_[0].page << 15))] = value;
                break;
            
                case RAM:
                    if (!getMpButtonState())
                    {
                        emsMemory_[0].mainMem[(long) ((address & 0x7fff) |(computerConfiguration.emsConfig_[0].page << 15))] = value;
                        if (address >= memoryStart_ && address<(memoryStart_ + 256))
                            p_Main->updateDebugMemory(address);
                        p_Main->updateAssTabCheck(address);
                    }
                break;
            }
        break;
            
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

        case MAPPEDRAM:
        case RAM:
            if (!getMpButtonState())
            {
                address = (address & ramMask_) + ramStart_;
                if (mainMemory_[address]==value)
                    return;
                mainMemory_[address]=value;
                if (address >= (memoryStart_& ramMask_) && address<((memoryStart_& ramMask_) + 256))
                    p_Main->updateDebugMemory(address);
                p_Main->updateAssTabCheck(address);
            }
        break;

        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>12) << 12) |(address &0xfff))/256])
            {
                case UNDEFINED:
                case ROM:
                    if (writeRom)
                        pagerMemory_[(getPager(address>>12) << 12) |(address &0xfff)] = value;
                break;

                case RAM:
                    if (!getMpButtonState())
                        pagerMemory_[(getPager(address>>12) << 12) |(address &0xfff)] = value;
                    if (address >= memoryStart_ && address<(memoryStart_ + 256))
                        p_Main->updateDebugMemory(address);
                    p_Main->updateAssTabCheck(address);
                break;
            }
        break;

    }
}

void Elf::cpuInstruction()
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
        if (cpuMode_ == LOAD)
        {
            showData(readMem(address_));
            ledCycleValue_ = 1;
            threadPointer->Sleep(1);
        }
    }
}

void Elf::resetPressed()
{
    if (!elfConfiguration.bootStrap)
        bootstrap_ = 0x8000;

    p_Main->stopTerminal();
    terminalStop();

    resetCpu();
     if (elfConfiguration.use8275)
        i8275Pointer->cRegWrite(0x40);
    if (elfConfiguration.autoBoot)
    {
        scratchpadRegister_[0]=p_Main->getBootAddress("Elf", ELF);
        autoBoot();
    }
    resetPressed_ = false;
    p_Main->setSwName("");
    p_Main->eventUpdateTitle();
    startElfKeyFile("Elf");
}

void Elf::configureExtensions()
{
    wxString fileName, fileName2;

    computerConfiguration.numberOfVideoTypes_ = 0;

    if (elfConfiguration.vtType != VTNONE)
    {
        double zoom = p_Main->getZoomVt();
        if (elfConfiguration.vtType == VT52)
            vtPointer = new Vt100("Elf - VT 52", p_Main->getVtPos(ELF), wxSize(640*zoom, 400*zoom), zoom, ELF, elfClockSpeed_, elfConfiguration, UART1);
        else
            vtPointer = new Vt100("Elf - VT 100", p_Main->getVtPos(ELF), wxSize(640*zoom, 400*zoom), zoom, ELF, elfClockSpeed_, elfConfiguration, UART1);
        p_Vt100[UART1] = vtPointer;
        vtPointer->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.ioConfiguration);
        if (elfConfiguration.useUart16450)
            configureUart16450(elfConfiguration.ioConfiguration);
        vtPointer->Show(true);
        vtPointer->drawScreen();
    }

    if (elfConfiguration.vtExternal)
    {
        p_Serial = new Serial(ELF, elfClockSpeed_, elfConfiguration);
        p_Serial->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.ioConfiguration);
    }

    if (elfConfiguration.usePixie)
    {
        double zoom = p_Main->getZoom(VIDEOMAIN);
        double scale = p_Main->getScale();
        pixiePointer = new Pixie( "Elf - Pixie", p_Main->getPixiePos(ELF), wxSize(64*zoom*scale, 128*zoom), zoom, scale, ELF, computerConfiguration.numberOfVideoTypes_);
        p_Video[computerConfiguration.numberOfVideoTypes_++] = pixiePointer;
        pixiePointer->configurePixie(elfConfiguration.ioConfiguration);
        pixiePointer->initPixie();
        pixiePointer->setZoom(zoom);
        pixiePointer->Show(true);
    }

    if (elfConfiguration.use6845)
    {
        double zoom = p_Main->getZoom(VIDEOMAIN);
        mc6845Pointer = new MC6845( "Elf - MC6845", p_Main->get6845Pos(ELF), wxSize(64*8*zoom, 16*8*zoom), zoom, ELF, elfClockSpeed_, 8, elfConfiguration.ioConfiguration, computerConfiguration.numberOfVideoTypes_);
        p_Video[computerConfiguration.numberOfVideoTypes_++] = mc6845Pointer;
        mc6845Pointer->configure6845(elfConfiguration.ioConfiguration);
        mc6845Pointer->init6845();
        mc6845Pointer->Show(true);
    }

    if (elfConfiguration.useS100)
    {
        double zoom = p_Main->getZoom(VIDEOMAIN);
        mc6845Pointer = new MC6845( "Elf - Quest Super Video", p_Main->get6845Pos(ELF), wxSize(64*7*zoom, 16*9*zoom), zoom, ELF, elfClockSpeed_, 7, elfConfiguration.ioConfiguration, computerConfiguration.numberOfVideoTypes_);
        p_Video[computerConfiguration.numberOfVideoTypes_++] = mc6845Pointer;
        mc6845Pointer->configureSuperVideo();
        mc6845Pointer->init6845();
        mc6845Pointer->Show(true);
    }

    if (elfConfiguration.use6847)
    {
        double zoom = p_Main->getZoom(VIDEOMAIN);
        mc6847Pointer = new mc6847( "Elf - MC6847", p_Main->get6847Pos(ELF), wxSize(elfConfiguration.charLine*8*zoom, elfConfiguration.screenHeight6847*zoom), zoom, ELF, elfClockSpeed_, elfConfiguration.ioConfiguration, computerConfiguration.numberOfVideoTypes_);
        p_Video[computerConfiguration.numberOfVideoTypes_++] = mc6847Pointer;
        mc6847Pointer->configure(elfConfiguration.ioConfiguration);
        mc6847Pointer->Show(true);
    }

    if (elfConfiguration.use8275)
    {
        double zoom = p_Main->getZoom(VIDEOMAIN);
        i8275Pointer = new i8275( "Elf - Intel 8275", p_Main->get8275Pos(ELF), wxSize(80*8*zoom, 24*10*2*zoom), zoom, ELF, elfClockSpeed_, computerConfiguration.numberOfVideoTypes_);
        p_Video[computerConfiguration.numberOfVideoTypes_++] = i8275Pointer;
        i8275Pointer->configure8275(elfConfiguration.ioConfiguration);
        i8275Pointer->init8275();
        i8275Pointer->Show(true);
    }

    if (elfConfiguration.useTMS9918)
    {
        double zoom = p_Main->getZoom(VIDEOMAIN);
        tmsPointer = new Tms9918( "Elf - TMS 9918", p_Main->getTmsPos(ELF), wxSize(320*zoom,240*zoom), zoom, ELF, elfClockSpeed_, computerConfiguration.numberOfVideoTypes_);
        p_Video[computerConfiguration.numberOfVideoTypes_++] = tmsPointer;
        tmsPointer->configure(elfConfiguration.ioConfiguration);
        tmsPointer->Show(true);
    }

    if (elfConfiguration.fdc1793Enabled)
    {
        configure1793(1, 40, 18, 256, 6256, ELF, elfConfiguration.ioConfiguration, true);
        resetFdc();
    }

    if (elfConfiguration.ideEnabled)
    {
        configureIde(p_Main->getIdeDir(ELF) + p_Main->getIdeFile(ELF), p_Main->getIdeDir(ELF) + "disk2.ide", elfConfiguration.ioConfiguration);
    }

    if (p_Main->getPrinterStatus(ELF))
    {
        p_Printer = new Printer();
        p_Printer->configureBasicPrinter(elfConfiguration.ioConfiguration);
        p_Printer->init(p_Printer, PRINTER_BASIC);
    }

    if (elfConfiguration.useLedModule)
    {
        ledModulePointer = new LedModule("Led Module", p_Main->getLedModulePos(), wxSize(172, 116), ELF);
        ledModulePointer->configure(elfConfiguration.ioConfiguration);
        ledModulePointer->Show(p_Main->getUseElfControlWindows(ELF));
    }

    setQsound (elfConfiguration.qSound_);

    if (elfConfiguration.useHexKeyboard)
    {
        keypadPointer = new Keypad("Keypad", p_Main->getKeypadPos(ELF), wxSize(172, 229), ELF);
        keypadPointer->Show(p_Main->getUseElfControlWindows(ELF));
        p_Main->message("Configuring Elf Keypad\n");
    }

    if (elfConfiguration.useKeyboard)
        configureKeyboard(ELF, elfConfiguration.ioConfiguration);

    if (elfConfiguration.usePS2)
        configurePs2(elfConfiguration.ps2Interrupt, elfConfiguration.ioConfiguration);
}

void Elf::moveWindows()
{
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->Move(p_Main->getKeypadPos(ELF));
    if (elfConfiguration.useLedModule)
        ledModulePointer->Move(p_Main->getLedModulePos());
    if (elfConfiguration.usePixie)
        pixiePointer->Move(p_Main->getPixiePos(ELF));
    if (elfConfiguration.useTMS9918)
        tmsPointer->Move(p_Main->getTmsPos(ELF));
    if (elfConfiguration.use6845||elfConfiguration.useS100)
        mc6845Pointer->Move(p_Main->get6845Pos(ELF));
    if (elfConfiguration.use6847)
        mc6847Pointer->Move(p_Main->get6847Pos(ELF));
    if (elfConfiguration.use8275)
        i8275Pointer->Move(p_Main->get8275Pos(ELF));
    if (elfConfiguration.vtType != VTNONE)
        vtPointer->Move(p_Main->getVtPos(ELF));
}

void Elf::updateTitle(wxString Title)
{
    if (elfConfiguration.usePixie)
        pixiePointer->SetTitle("Elf - Pixie"+Title);
    if (elfConfiguration.useTMS9918)
        tmsPointer->SetTitle("Elf - TMS 9918"+Title);
    if (elfConfiguration.use6845)
        mc6845Pointer->SetTitle("Elf - MC6845"+Title);
    if (elfConfiguration.useS100)
        mc6845Pointer->SetTitle("Elf - Quest Super Video"+Title);
    if (elfConfiguration.use6847)
        mc6847Pointer->SetTitle("Elf - MC6847"+Title);
    if (elfConfiguration.use8275)
        i8275Pointer->SetTitle("Elf - Intel 8275"+Title);
    if (elfConfiguration.vtType == VT52)
        vtPointer->SetTitle("Elf - VT 52"+Title);
    if (elfConfiguration.vtType == VT100)
        vtPointer->SetTitle("Elf - VT 100"+Title);
    if (elfConfiguration.useLedModule)
        ledModulePointer->SetTitle("Led Module"+Title);
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->SetTitle("Keypad"+Title);
}

void Elf::setForceUpperCase(bool status)
{
    if (elfConfiguration.vtType != VTNONE)
        vtPointer->setForceUCVt(status);
    if (elfConfiguration.useKeyboard)
        setForceUpperCaseKeyboard(status);
}

void Elf::onReset()
{
    resetPressed_ = true;
}

void Elf::sleepComputer(long ms)
{
    threadPointer->Sleep(ms);
}

void Elf::pauseComputer()
{
    threadPointer->Pause();
}

void Elf::resumeComputer()
{
    threadPointer->Resume();
}

Byte Elf::read8275CharRom(Word address)
{
    if (elfConfiguration.use8275)
        return i8275Pointer->read8275CharRom(address);
    else
        return 0;
}

Byte Elf::read8275VideoRam(Word address)
{
    if (elfConfiguration.use8275)
        return i8275Pointer->read8275VideoRam(address);
    else
        return 0;
}

void Elf::write8275VideoRam(Word address, Byte value)
{
    if (elfConfiguration.use8275)
        i8275Pointer->write8275VideoRam(address, value);
}

void Elf::write8275CharRom(Word address, Byte value)
{
    if (elfConfiguration.use8275)
        i8275Pointer->write8275CharRom(address, value);
}

Byte Elf::read6845CharRom(Word address)
{
    if (elfConfiguration.use6845||elfConfiguration.useS100)
        return mc6845Pointer->read6845CharRom(address);
    else
        return 0;
}

void Elf::write6845CharRom(Word address, Byte value)
{
    mc6845Pointer->write6845CharRom(address, value);
}

Byte Elf::read6847CharRom(Word address)
{
    if (elfConfiguration.use6847)
        return mc6847Pointer->read6847CharRom(address);
    else
        return 0;
}

void Elf::write6847CharRom(Word address, Byte value)
{
    if (elfConfiguration.use6847)
        mc6847Pointer->write6847CharRom(address, value);
}

int Elf::readDirect6847(Word address)
{
    if (elfConfiguration.use6847)
        return mc6847Pointer->readDirect6847(address);
    else
        return 0;
}

void Elf::writeDirect6847(Word address, int value)
{
    if (elfConfiguration.use6847)
        mc6847Pointer->writeDirect6847(address, value);
}

Word Elf::get6847RamMask()
{
     if (elfConfiguration.use6847)
        return mc6847Pointer->get6847RamMask();
    else
        return 0;
}

void Elf::setLedMs(long ms)
{
    elfScreenPointer->setLedMs(ms);
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->setLedMs(ms);
    if (elfConfiguration.useLedModule && !ledModuleClosed_)
        ledModulePointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((elfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

Byte Elf::getKey(Byte vtOut)
{
    Byte tempVtOut = elfScreenPointer->getKey(vtOut);

    if (tempVtOut == vtOut)
    {
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
            tempVtOut = keypadPointer->getKey(vtOut);
        if (tempVtOut == vtOut)
        {
            if (elfConfiguration.useLedModule && !ledModuleClosed_)
                tempVtOut = ledModulePointer->getKey(vtOut);
        }
    }
    return tempVtOut;
}

void Elf::activateMainWindow()
{
    bool maximize = IsMaximized();
    Iconize(false);
    Raise();
    Show(true);
    Maximize(maximize);
}

void Elf::releaseButtonOnScreen(HexButton* buttonPointer, int WXUNUSED(buttonType))
{
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->releaseButtonOnScreen(buttonPointer);
}

void Elf::onNumberKeyDown(int i)
{
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->onNumberKeyDown(i);
}

void Elf::onNumberKeyUp()
{
    ef3State_ = 1;
}

void Elf::refreshPanel()
{
    elfScreenPointer->refreshPanel();
    if (elfConfiguration.useLedModule && !ledModuleClosed_)
        ledModulePointer->refreshPanel();
    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->refreshPanel();

}

void Elf::OnRtcTimer(wxTimerEvent&WXUNUSED(event))
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

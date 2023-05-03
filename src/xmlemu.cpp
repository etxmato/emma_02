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

#include <math.h>

#include "main.h"
#include "pushbutton.h"

#include "xmlemu.h"

#define OFF    1

#define PRIDLE 0
#define PRW_START 1
#define PRW_STOP 2
#define PRBUSY 3

BEGIN_EVENT_TABLE(Xmlemu, wxFrame)
    EVT_CLOSE (Xmlemu::onClose)
    EVT_COMMAND(0, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(1, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(2, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(3, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(4, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(5, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(6, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(7, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(8, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(9, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(10, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(11, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(12, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(13, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(14, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(15, wxEVT_ButtonDownEvent, Xmlemu::onNumberKeyDown)
    EVT_COMMAND(0, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(1, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(2, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(3, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(4, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(5, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(6, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(7, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(8, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(9, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(10, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(11, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(12, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(13, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(14, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(15, wxEVT_ButtonUpEvent, Xmlemu::onNumberKeyUp)
    EVT_COMMAND(20, wxEVT_ButtonDownEvent, Xmlemu::onButtonPress)
    EVT_COMMAND(20, wxEVT_ButtonUpEvent, Xmlemu::onButtonRelease)
    EVT_BUTTON(21, Xmlemu::onRunButton)
    EVT_BUTTON(22, Xmlemu::onMpButton)
    EVT_BUTTON(24, Xmlemu::onLoadButton)
    EVT_BUTTON(25, Xmlemu::onPause)
    EVT_BUTTON(26, Xmlemu::onSingleStep)
    EVT_BUTTON(27, Xmlemu::onResetButton)
    EVT_BUTTON(28, Xmlemu::onMonitor)
    EVT_TIMER(900, Xmlemu::OnRtcTimer)
END_EVENT_TABLE()

Xmlemu::Xmlemu(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf, Conf computerConf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    computerConfiguration = computerConf;
    elfConfiguration = conf;
    elfClockSpeed_ = clock;

#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif
  
    p_Main->stopTerminal();

    this->SetClientSize(size);

    cycleValue_ = -1;
    cycleSize_ = -1;

    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            elfScreenPointer = new ElfScreen(this, size, elfConfiguration.tilType);
            elfScreenPointer->init();
        break;

        case PANEL_ELFII:
            elf2ScreenPointer = new Elf2Screen(this, size, elfConfiguration.tilType);
            elf2ScreenPointer->init();
        break;

        case PANEL_SUPER:
            superScreenPointer = new SuperScreen(this, size, elfConfiguration.tilType);
            superScreenPointer->init();
        break;

        case PANEL_ELF2K:
            elf2KScreenPointer = new Elf2KScreen(this, size);
            elf2KScreenPointer->init();
        break;

        case PANEL_COSMICOS:
            cosmicosScreenPointer = new CosmicosScreen(this, size);
            cosmicosScreenPointer->init();

            cycleSize_ = (int)(1000 / ((1/1.75) * 8)); // ~1000 Hz on 1.75 CPU
            cycleValue_ = cycleSize_;
        break;

        case PANEL_MICROTUTOR:
            microtutorScreenPointer = new MicrotutorScreen(this, size);
            microtutorScreenPointer->init();
        break;

        case PANEL_MICROTUTOR2:
            microtutor2ScreenPointer = new Microtutor2Screen(this, size);
            microtutor2ScreenPointer->init();
        break;
    }
    
    rtcTimerPointer = new wxTimer(this, 900);
    
    runningGame_ = "";
}

Xmlemu::~Xmlemu()
{
    p_Main->batchConvertStop();
    saveRtc();
    if (nvramDetails.size() > 0)
        saveNvRam();
    rtcTimerPointer->Stop();
    delete rtcTimerPointer;
    if (elfConfiguration.usePixie)
    {
        p_Main->setPixiePos(XML, pixiePointer->GetPosition());
        pixiePointer->Destroy();
    }
    if (elfConfiguration.use1864)
    {
        p_Main->setCdp1864Pos(XML, cdp1864Pointer->GetPosition());
        cdp1864Pointer->Destroy();
    }
    if (elfConfiguration.useTMS9918)
    {
        p_Main->setTmsPos(XML, tmsPointer->GetPosition());
        tmsPointer->Destroy();
    }
    if (elfConfiguration.use6845)
    {
        p_Main->set6845Pos(XML, mc6845Pointer->GetPosition());
        mc6845Pointer->Destroy();
    }
    if (elfConfiguration.use6847)
    {
        p_Main->set6847Pos(XML, mc6847Pointer->GetPosition());
        mc6847Pointer->Destroy();
    }
    if (elfConfiguration.use8275)
    {
        p_Main->set8275Pos(XML, i8275Pointer->GetPosition());
        i8275Pointer->Destroy();
    }
    if (elfConfiguration.usev1870)
    {
        p_Main->setV1870Pos(XML, vis1870Pointer->GetPosition());
        vis1870Pointer->Destroy();
    }
    if (elfConfiguration.useSN76430N)
    {
        p_Main->setV1870Pos(XML, sn76430nPointer->GetPosition());
        sn76430nPointer->Destroy();
    }
    if (elfConfiguration.vtType != VTNONE)
    {
        p_Main->setVtPos(XML, vtPointer->GetPosition());
        vtPointer->Destroy();
    }
    if (elfConfiguration.useBitKeypad)
    {
        if (elfConfiguration.ioConfiguration.bitKeypad[0].defined)
            delete bitkeypadPointer[0];
        if (elfConfiguration.ioConfiguration.bitKeypad[1].defined)
            delete bitkeypadPointer[1];
    }
    if (elfConfiguration.useCvKeypad)
    {
        if (elfConfiguration.ioConfiguration.cvKeypad.defined)
            delete cvkeypadPointer;
    }
    if (elfConfiguration.useLatchKeyboard)
        delete latchKeyboardPointer;
    if (elfConfiguration.useMatrixKeyboard)
        delete matrixKeyboardPointer;
    if (elfConfiguration.vtExternal)
        delete p_Serial;
    if (computerConfiguration.useBasicPrinter_ || computerConfiguration.useQSerialPrinter_)
    {
        p_Printer->closeFrames();
        delete p_Printer;
    }
    if (computerConfiguration.useParallelPrinter_)
    {
        p_PrinterParallel->closeFrames();
        delete p_PrinterParallel;
    }
    if (computerConfiguration.useSerialPrinter_)
    {
        p_PrinterSerial->closeFrames();
        delete p_PrinterSerial;
    }
    if (computerConfiguration.useThermalPrinter_)
    {
        p_PrinterThermal->closeFrames();
        delete p_PrinterThermal;
    }

    p_Main->setMainPos(XML, GetPosition());

    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
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
            delete elfScreenPointer;
        break;

        case PANEL_ELFII:
            delete elf2ScreenPointer;
        break;

        case PANEL_SUPER:
            delete superScreenPointer;
        break;
            
        case PANEL_ELF2K:
            if (elfConfiguration.useSwitch)
            {
                p_Main->setElf2KswitchPos(p_Elf2Kswitch->GetPosition());
                p_Elf2Kswitch->Destroy();
            }
            if (elfConfiguration.useHex)
            {
                p_Main->setKeypadPos(XML, p_Elf2Khex->GetPosition());
                p_Elf2Khex->Destroy();
            }
            delete elf2KScreenPointer;
        break;

        case PANEL_COSMICOS:
            if (elfConfiguration.useHex)
            {
                p_Main->setKeypadPos(XML, p_Cosmicoshex->GetPosition());
                p_Cosmicoshex->Destroy();
            }
            delete cosmicosScreenPointer;
        break;

        case PANEL_MICROTUTOR:
            delete microtutorScreenPointer;
        break;

        case PANEL_MICROTUTOR2:
            delete microtutor2ScreenPointer;
        break;
    }
}

void Xmlemu::showModules(bool status)
{
    if (elfConfiguration.panelType_ == PANEL_COSMICOS)
    {
        p_Cosmicoshex->Show(status);
        return;
    }

    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->Show(status);
    if (elfConfiguration.useLedModule && !ledModuleClosed_)
        ledModulePointer->Show(status);
}
    
void Xmlemu::showModules(bool status, bool useSwitch, bool useHex)
{
    if (useSwitch && !status)
    {
        p_Main->setElf2KswitchPos(p_Elf2Kswitch->GetPosition());
        p_Elf2Kswitch->Destroy();
        elfConfiguration.useSwitch = false;
    }
    if (useHex && !status)
    {
        p_Main->setKeypadPos(XML, p_Elf2Khex->GetPosition());
        p_Elf2Khex->Destroy();
        elfConfiguration.useHex = false;
    }
    if (useSwitch && status)
    {
        p_Elf2Kswitch = new Elf2Kswitch("Switch Panel", p_Main->getElf2KswitchPos(), wxSize(451, 166));
        p_Elf2Kswitch->Show(true);
        p_Elf2Kswitch->setRunButtonUp(runButtonState_);
        p_Elf2Kswitch->setLoadButtonUp(!loadButtonState_);
        p_Elf2Kswitch->setMpButtonUp(mpButtonState_);
        for (int i=0; i<8; i++)
        {
            p_Elf2Kswitch->setDataSwitchUp(i, dataSwitchState_[i]);
        }
        elfConfiguration.useSwitch = true;
    }
    if (useHex && status)
    {
        p_Elf2Khex = new Elf2Khex("Hex Panel", p_Main->getKeypadPos(XML), wxSize(186, 126));
        p_Elf2Khex->Show(true);
        p_Elf2Khex->setRunButtonUp(runButtonState_);
        p_Elf2Khex->setLoadButtonUp(loadButtonState_);
        p_Elf2Khex->setMpButtonUp(!mpButtonState_);
        elfConfiguration.useHex = true;
    }
}


void Xmlemu::onClose(wxCloseEvent&WXUNUSED(event) )
{
    p_Main->stopComputer();
}

void Xmlemu::charEvent(int keycode)
{
    if (elfConfiguration.useKeyboard)
        charEventKeyboard(keycode);
    if (elfConfiguration.useKeyb1871)
        charEventKeyb1871(keycode);
    if (elfConfiguration.useMatrixKeyboard)
        matrixKeyboardPointer->charEvent(keycode);
    if (elfConfiguration.usePs2gpio)
        charEventPs2gpio(keycode);
}

bool Xmlemu::keyDownPressed(int key)
{
    onHexKeyDown(key);
    if (elfConfiguration.gpioJp4)
    {
        switch(key)
        {
            case WXK_NUMPAD0:
            case WXK_NUMPAD1:
            case WXK_NUMPAD2:
            case WXK_NUMPAD3:
            case WXK_NUMPAD4:
            case WXK_NUMPAD5:
            case WXK_NUMPAD6:
            case WXK_NUMPAD7:
            case WXK_NUMPAD8:
            case WXK_NUMPAD9:
            case WXK_NUMPAD_DECIMAL:
            case WXK_NUMPAD_ADD:
            case WXK_NUMPAD_DIVIDE:
            case WXK_NUMPAD_MULTIPLY:
            case WXK_NUMPAD_SUBTRACT:
            case WXK_NUMPAD_ENTER:
                charEventPs2gpio(key);
                return true;
            break;
        }
    }
    if (elfConfiguration.usePS2)
    {
        keyDownPs2(key);
            return true;
    }
    return false;
}

bool Xmlemu::keyDownExtended(int keycode, wxKeyEvent& event)
{
    if (elfConfiguration.efButtons)
    {
        for (int ef=1; ef <=4; ef++)
        {
            if (elfConfiguration.ioConfiguration.efKey[ef].defined)
            {
                if (elfConfiguration.ioConfiguration.efKey[ef].value == keycode)
                    efKeyValue[ef] = elfConfiguration.ioConfiguration.efKeyPressed;
            }
        }
    }
    if (elfConfiguration.useBitKeypad)
    {
        if (elfConfiguration.ioConfiguration.bitKeypad[0].defined)
            bitkeypadPointer[0]->keyDown(keycode);
        if (elfConfiguration.ioConfiguration.bitKeypad[1].defined)
            bitkeypadPointer[1]->keyDown(keycode);
    }
    if (elfConfiguration.useCvKeypad)
    {
        if (elfConfiguration.ioConfiguration.cvKeypad.defined)
            cvkeypadPointer->keyDown(keycode, event);
    }
    if (elfConfiguration.useLatchKeyboard)
        latchKeyboardPointer->keyDown(keycode, event);
    if (elfConfiguration.useMatrixKeyboard)
        matrixKeyboardPointer->keyDownExtended(keycode, event);
    if (elfConfiguration.useKeyb1871)
        return keyDownExtended1871(keycode, event);
    return false;
}

bool Xmlemu::keyUpReleased(int key, wxKeyEvent& event)
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
    if (elfConfiguration.useKeyb1871)
        keyUp1871(key);
    if (elfConfiguration.efButtons)
    {
        for (int ef=1; ef <=4; ef++)
        {
            if (elfConfiguration.ioConfiguration.efKey[ef].defined)
            {
                if (elfConfiguration.ioConfiguration.efKey[ef].value == key)
                    efKeyValue[ef] = elfConfiguration.ioConfiguration.efKeyPressed | 0xff;
            }
        }

    }
    if (elfConfiguration.useBitKeypad)
    {
        if (elfConfiguration.ioConfiguration.bitKeypad[0].defined)
            bitkeypadPointer[0]->keyUp(key);
        if (elfConfiguration.ioConfiguration.bitKeypad[1].defined)
            bitkeypadPointer[1]->keyUp(key);
    }
    if (elfConfiguration.useCvKeypad)
    {
        if (elfConfiguration.ioConfiguration.cvKeypad.defined)
           cvkeypadPointer->keyUp(key, event);
    }
    if (elfConfiguration.useLatchKeyboard)
        latchKeyboardPointer->keyUp(key, event);
    if (elfConfiguration.useMatrixKeyboard)
        matrixKeyboardPointer->keyUpExtended(key, event);
    return false;
}

void Xmlemu::onButtonRelease(wxCommandEvent& event)
{
    onInButtonRelease();
    event.Skip();
}

void Xmlemu::onButtonPress(wxCommandEvent& event)
{
    onInButtonPress();
    event.Skip();
}

void Xmlemu::onInButtonPress()
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
        case PANEL_ELF2K:
            if (elfConfiguration.useHexKeyboard)
                onInButtonPress(switches_);
            else
                onInButtonPress(getData());
        break;

        case PANEL_COSMICOS:
            onInButtonPress(getData());
       break;

        default:
            if (cpuMode_ == LOAD)
            {
                Byte value = getData();
                dmaIn(value);
                showData(value);
            }
            else
            {
                inbuttonEfState_ = 0;
            }
        break;
    }
}

void Xmlemu::onInButtonPress(Byte value)
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
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
        break;

        case PANEL_ELF2K:
            if (inPressed_ == true)  return;
            inPressed_ = true;
            if (getCpuMode() == LOAD)
            {
                dmaIn(value);
                showData(value);
            }
            if (elfConfiguration.useSwitch)
                p_Elf2Kswitch->setInButtonUp(false);
            if (elfConfiguration.useHex)
                p_Elf2Khex->setInButtonUp(false);
        break;
            
        case PANEL_COSMICOS:
            if (inPressed_ == true)  return;
            inPressed_ = true;
            if (getCpuMode() == LOAD)
            {
                dmaIn(value);
                showData(value);
                for (int i=0; i<8; i++)
                {
                    dataSwitchState_[i] = 0;
                    cosmicosScreenPointer->setLed(i,0);
                }
            }
        break;
            
        case PANEL_MICROTUTOR:
            if (loadButtonState_ == 1)
                inPressed_ = true;
            if (getCpuMode() == LOAD)
            {
                dmaIn(value);
                showData(value);
            }
            microtutorScreenPointer->inSetState(BUTTON_DOWN);
        break;

        case PANEL_MICROTUTOR2:
            if (loadButtonState_ == 1)
                inPressed_ = true;
            if (getCpuMode() == LOAD)
            {
                dmaIn(value);
                showData(value);
            }
            microtutor2ScreenPointer->inSetState(BUTTON_DOWN);
        break;
    }
}

void Xmlemu::onElf2KInButton()
{
    p_Elf2Khex->onElf2KInButton();
}

void Xmlemu::onInButtonRelease()
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            if (efSwitchState_[3] == 1)
            {
                inPressed_ = false;
                efSwitchState_[3] = 0;
            }
            elfScreenPointer->inSetState(BUTTON_UP);
        break;

        case PANEL_ELF2K:
            if (inPressed_ == false)  return;
            inPressed_ = false;
            if (elfConfiguration.useSwitch)
                p_Elf2Kswitch->setInButtonUp(true);
            if (elfConfiguration.useHex)
                p_Elf2Khex->setInButtonUp(true);
        break;
            
        case PANEL_COSMICOS:
            if (inPressed_ == false)  return;
            inPressed_ = false;
        break;
            
        case PANEL_MICROTUTOR:
            if (loadButtonState_ == 1)
                inPressed_ = false;
            microtutorScreenPointer->inSetState(BUTTON_UP);
        break;

        case PANEL_MICROTUTOR2:
            if (loadButtonState_ == 1)
                inPressed_ = false;
            microtutor2ScreenPointer->inSetState(BUTTON_UP);
        break;

        default:
            inbuttonEfState_ = 1;
        break;
    }
}

void Xmlemu::configureComputer()
{
    wxString ioGroup;
    
    setEfType(1, EF1UNDEFINED);
    setEfType(2, EF2UNDEFINED);
    setEfType(3, EF3UNDEFINED);
    setEfType(4, EF4UNDEFINED);

    setCycleType(COMPUTERCYCLE, LEDCYCLE);
    wxString printBuffer;

    p_Main->message("Configuring " + p_Main->getRunningComputerText());
    if (elfConfiguration.panelType_ != PANEL_NONE)
    {
        printBuffer.Printf("    Output %d: til display output", elfConfiguration.ioConfiguration.hexOutput);
        
 //       if (elfConfiguration.ioConfiguration.hexOutput == elfConfiguration.ioConfiguration.bootStrapOut && elfConfiguration.ioConfiguration.bootStrapType == BOOTSTRAPIO)
 //           printBuffer += " & release bootstrap";
        p_Main->message(printBuffer);
        p_Computer->setOutType(elfConfiguration.ioConfiguration.hexOutput, ELF2OUT);
    }

    if (elfConfiguration.useHexKeyboard)
    {
        printBuffer.Printf("    input %d: data input", elfConfiguration.ioConfiguration.hexInput);
        p_Main->message(printBuffer);
    }

    if (elfConfiguration.useSwitch)
    {
        printBuffer.Printf("    input %d: hex keypad input", elfConfiguration.ioConfiguration.hexInput);
//        if (elfConfiguration.ioConfiguration.hexInput == elfConfiguration.ioConfiguration.bootStrapIn && elfConfiguration.ioConfiguration.bootStrapType == BOOTSTRAPIO)
//            printBuffer += " & release bootstrap";
        p_Main->message(printBuffer);
        p_Computer->setInType(elfConfiguration.ioConfiguration.hexInput, ELF2IN);
    }

    if (elfConfiguration.useEms)
    {
        elfConfiguration.ioConfiguration.emsOutput.resize(computerConfiguration.emsConfigNumber_);
        for (size_t emsNumber=0; emsNumber<computerConfiguration.emsConfigNumber_; emsNumber++)
        {
            if (elfConfiguration.ioConfiguration.emsOutput[emsNumber] == -1)
            {
                printBuffer.Printf("    Address %04X-%04X: EMS-512KB page select", computerConfiguration.emsConfig_[emsNumber].outputStart, computerConfiguration.emsConfig_[emsNumber].outputEnd);
            }
            else
            {
                printBuffer.Printf("    Output %d: EMS-512KB page select", elfConfiguration.ioConfiguration.emsOutput[emsNumber]);
                p_Computer->setOutType(elfConfiguration.ioConfiguration.emsOutput[emsNumber], EMSMAPPEROUT);
            }
            p_Main->message(printBuffer);
        }
    }
    if (elfConfiguration.useTape)
    {
        ioGroup = "";
        if (elfConfiguration.ioConfiguration.tapeIoGroup != -1)
            ioGroup.Printf(" (on group %X)", elfConfiguration.ioConfiguration.tapeIoGroup);

        p_Computer->setEfType(elfConfiguration.ioConfiguration.tapeIoGroup+1, elfConfiguration.ioConfiguration.tapeEf, TAPE_EF);
        printBuffer.Printf("    EF %d: cassette in"+ioGroup, elfConfiguration.ioConfiguration.tapeEf);
        p_Main->message(printBuffer);
    }
    if (elfConfiguration.useHexKeyboardEf3)
    {
        printBuffer.Printf("    EF %d: 0 when hex button pressed", elfConfiguration.ioConfiguration.hexEf);
        p_Main->message(printBuffer);
    }

    if (elfConfiguration.efButtons)
    {
        for (int ef=1; ef<=4; ef++)
        {
            if (elfConfiguration.ioConfiguration.efKey[ef].defined)
            {
                printBuffer.Printf("    EF %d: %d when key code %d is pressed", ef, elfConfiguration.ioConfiguration.efKeyPressed, elfConfiguration.ioConfiguration.efKey[ef].value);
                p_Main->message(printBuffer);
            }
        }
    }
    if (elfConfiguration.useInButton)
    {
        printBuffer.Printf("    EF %d: 0 when in button pressed", elfConfiguration.ioConfiguration.inEf);
        p_Main->message(printBuffer);
    }
    
    if (elfConfiguration.bootStrap)
    {
        printBuffer.Printf("    Bootstrap address: %04X", elfConfiguration.strapAddress);
        p_Main->message(printBuffer);
        
        switch (elfConfiguration.ioConfiguration.bootStrapType)
        {
            case BOOTSTRAPREAD:
                printBuffer.Printf("    Read address >= %04X: release bootstrap", elfConfiguration.strapAddress);
                p_Main->message(printBuffer);
            break;

            case BOOTSTRAPOUT:
//                p_Computer->setOutType(elfConfiguration.ioConfiguration.bootStrapOut, BOOTSTRAPDISABLE);
                printBuffer.Printf("    Output %d: release bootstrap", elfConfiguration.ioConfiguration.bootStrapOut);
                p_Main->message(printBuffer);
            break;

            case BOOTSTRAPIN:
//                p_Computer->setInType(elfConfiguration.ioConfiguration.bootStrapIn, BOOTSTRAPDISABLE);
                printBuffer.Printf("    Input %d: release bootstrap", elfConfiguration.ioConfiguration.bootStrapIn);
                p_Main->message(printBuffer);
            break;

            case BOOTSTRAPIO:
//                p_Computer->setInType(elfConfiguration.ioConfiguration.bootStrapIn, BOOTSTRAPDISABLE);
                printBuffer.Printf("    Input & output %d: release bootstrap", elfConfiguration.ioConfiguration.bootStrapIn);
                p_Main->message(printBuffer);
            break;
        }
    }
    
    hexModemOnStart = elfConfiguration.useHexModem;
    
    p_Main->message("");

    inKey1_ = p_Main->getDefaultInKey1("Xml");
    inKey2_ = p_Main->getDefaultInKey2("Xml");
    p_Main->getDefaultHexKeys(XML, "Xml", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);

    if (p_Main->getConfigBool("/ElfII/GameAuto", true))
        p_Main->loadKeyDefinition(computerConfiguration.memConfig_[0].dirname, computerConfiguration.memConfig_[0].filename, keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
    
    resetCpu();
}

void Xmlemu::switchHexEf(bool state)
{
    elfConfiguration.useHexKeyboardEf3 = state;
}

void Xmlemu::setPrinterEf()
{
    printerEfState_ = 0;
}

void Xmlemu::reLoadKeyDefinition(wxString fileName)
{
    runningGame_ = fileName;
    if (p_Main->getConfigBool("/ElfII/GameAuto", true))
        p_Main->loadKeyDefinition("", fileName, keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
}

void Xmlemu::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefA1_[i] = hexKeyDefA1[i];
        keyDefA2_[i] = hexKeyDefA2[i];
    }
}

void Xmlemu::reDefineKeysB(int hexKeyDefB1[], int hexKeyDefB2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefB1_[i] = hexKeyDefB1[i];
        keyDefB2_[i] = hexKeyDefB2[i];
    }
}

void Xmlemu::initComputer()
{
    Show(p_Main->getUseElfControlWindows(XML));

    mpButtonState_ = 0;
    loadButtonState_ = 1;
    runButtonState_ = 0;
    switches_ = 0;
    inbuttonEfState_ = 1;

    hexKeypadClosed_ = false;
    ledModuleClosed_ = false;
    monitor_ = false;

    for (int i=0; i<8; i++)  dataSwitchState_[i]=0;
    for (int i=0; i<4; i++)  efSwitchState_[i]=0;
    for (int ef=1; ef <=4; ef++)
        efKeyValue[ef] = 1;
    
    bootstrap_ = 0;

    pulseCount_ = 0;
    tapeRecording_ = false;
    tapeActivated_ = false;
    tapeEnd_ = false;
    startBytes_ = 2;
    zeroWaveCounter_ = -1;
    tapedataReady_ = 1;

    resetComputer();
}

void Xmlemu::resetComputer()
{
    inPressed_ = false;
    cassetteEf_ = 0;

    printerEfState_ = 1;
    hexEfState_ = 1;
    elfRunState_ = RESETSTATE;

    ioGroup_ = -1;
    qState_ = 0;
    
    lastMode_ = UNDEFINDEDMODE;
    
    thermalPrinting_ = false;
    thermalEF_ = 0;

    intCounter_ = elfConfiguration.intCycleValue;

    if (elfConfiguration.dmaOnFirstQ || elfConfiguration.dmaOnFirstOut != 0)
        dmaCounter_ = -1;
    else
        dmaCounter_ = elfConfiguration.dmaCycleValue;
    
    systemTime_ = wxDateTime::Now();
    xmlComputerTime_ = wxDateTime::Now();
    nvramWriteProtected_ = elfConfiguration.useNvRamMp;

    printerStatus_ = PRIDLE;
    batchInProgress_ = false;

    resetTape();

    if (elfConfiguration.usePs2gpio)
        resetPs2gpio();
    
    if (hexModemOnStart)
    {
        elfConfiguration.useHexModem = hexModemOnStart;
        elfConfiguration.useXmodem = false;
        p_Main->eventSetButtonLabel ("XmodemButtonXml", "HEX");

    }
}

Byte Xmlemu::ef(int flag)
{
    if (elfConfiguration.efButtons)
    {
        if (elfConfiguration.ioConfiguration.efKey[flag].defined)
            return efKeyValue[flag];
    }
    
    if (elfConfiguration.useHexKeyboardEf3)
    {
        if (flag == elfConfiguration.ioConfiguration.hexEf)
            return hexEfState_;
    }
    
    if (flag == elfConfiguration.ioConfiguration.inEf && elfConfiguration.useInButton)
    {
        switch (elfConfiguration.panelType_)
        {
            case PANEL_COSMAC:
            case PANEL_ELF2K:
            case PANEL_COSMICOS:
            case PANEL_MICROTUTOR:
            case PANEL_MICROTUTOR2:
                if (inPressed_ == true)
                    return 0;
            break;
                
            default:
                if (inbuttonEfState_ == 0)
                    return inbuttonEfState_;
            break;
        }
    }

    if (elfConfiguration.usev1870)
    {
        if  (elfConfiguration.ioConfiguration.v1870ioGroup == ioGroup_ || elfConfiguration.ioConfiguration.v1870ioGroup == -1)
        {
            if (elfConfiguration.ioConfiguration.v1870useVideoModeEf && elfConfiguration.ioConfiguration.v1870videoModeEf == flag)
            {
                if (p_Main->getVideoMode(XML) == NTSC)
                    return 0;
            }
            if (elfConfiguration.ioConfiguration.v1870ef == flag)
            {
                return vis1870Pointer->ef1_1870();
            }
        }
    }
    
    if (elfConfiguration.useMatrixKeyboard)
    {
        if  (elfConfiguration.ioConfiguration.keybMatrixIoGroup == ioGroup_ || elfConfiguration.ioConfiguration.keybMatrixIoGroup == -1)
        {
            return matrixKeyboardPointer->efKey(flag);
        }
    }
        
    switch(efType_[qState_][ioGroup_+1][flag])
    {
        case SB_EF:
            if (!nvramWriteProtected_)
                return usb_ef();
            else
                return cassetteEf_;
        break;

        case FDC1770_EF:
            if ((fdcRegisterSelect_&0x10) == 0x10)
                return ef1770();
            else
                return cassetteEf_;
        break;

        case FDC1793_EF:
            return ef1793();
        break;

        case  THERMAL_PRINT_EF:
            if (thermalPrinting_)
            {
                if (p_Main->getThermalEf())
                    return 1;
                else
                {
                    thermalEF_ = !thermalEF_;
                    return thermalEF_;
                }
            }
            else
                return cassetteEf_;
        break;

        case  BASICQ_PRINT_EF:
            if (printerStatus_ != PRIDLE)
                return true;
            else
                return cassetteEf_;
        break;

        case BASIC_PRINT_EF:
            return printerEfState_;
        break;

        case  MC6845EF:
            if (!isLoading())
                return mc6845Pointer->ef6845();
            else
                return cassetteEf_;
        break;

        case TAPE_EF:
            return cassetteEf_;
        break;

        case KEYB_LATCH_EF:
            return latchKeyboardPointer->ef();
        break;

        case PS2GPIOEF:
            return efPs2gpio();
        break;

        case TAPE_CV_EF:
            return tapedataReady_;
        break;

        case TAPE_CV_EF_OUT:
            return tapeHwReadyToReceive_;
        break;

        case CV_KEYPAD_EF:
            return cvkeypadPointer->ef();
        break;


        // Folowing I/O is not adapted to ioGroups
        case 0:
            if (elfConfiguration.panelType_ == PANEL_COSMAC)
                return 1-efSwitchState_[flag-1];
            else
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

        case PS2EF:
            return efPs2();
        break;

        case VT100EF:
            return vtPointer->ef();
        break;

        case VTSERIALEF:
            return p_Serial->ef();
        break;
 
        case I8275EF:
            return i8275Pointer->ef8275();
        break;

        case COMXEF2:
            return efKeybRepeat1871();
        break;

        case COMXEF3:
            return efKeyb1871();
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

        case EF4UNDEFINED:
            return elfConfiguration.ioConfiguration.ef4default;
        break;
            
        case COSMICOSREQ:
            if (elfConfiguration.use1864)
            {
                if (cdp1864Pointer->arePixieGraphicsOn())
                    return cdp1864Pointer->efPixie();
            }
            if (elfConfiguration.useHexKeyboardEf3 || elfConfiguration.useHex)
                return (p_Cosmicoshex->reqState() & p_Cosmicoshex->seqState());
            return 1;
        break;

        case COSMICOSDEC:
            if (isLoading() || realCassetteLoad_)
                return cassetteEf_;
            else
                return (p_Cosmicoshex->decState() & p_Cosmicoshex->seqState());
        break;

        case COSMICOSRET:
            return p_Cosmicoshex->retState();
        break;

        default:
            return 1;
    }
}

Byte Xmlemu::in(Byte port, Word address)
{
    Byte ret = 255;

    if (elfConfiguration.bootStrap)
    {
        if (port == elfConfiguration.ioConfiguration.bootStrapIn)
        {
            bootstrap_ = 0;
        }
    }
        
    switch (inType_[qState_][ioGroup_+1][port])
    {
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

        case UART16450_IN:
            ret = vtPointer->uart16450In();
        break;

        case UART16450_INSERIAL:
            ret = p_Serial->uart16450In();
        break;

        case UART16450_STATUSSERIAL:
        case UART16450_STATUS:
            ret = 255;
        break;

        case IDEIN:
            ret = inIde();
        break;

        case RTCIN:
            ret = inRtc();
        break;

        case IDEREADSTATUS:
            ret = readIdeStatus();
        break;

        case FDC1770_READIN:
            ret = readRegister1770();
            inValues_[port] = ret;
            return ret;
        break;

        case FDC1770_INTRQIN:
            ret = intrqStatus1770();
            inValues_[port] = ret;
            return ret;
        break;
            
        case FDC1793_READIN:
            ret = in1793();
            inValues_[port] = ret;
            return ret;
        break;

        case PARALLEL_PRINT_IN:
            ret = p_PrinterParallel->inParallel();
        break;

        case SERIAL_PRINT_IN:
            ret = p_PrinterSerial->inSerial();
        break;

        case THERMAL_PRINT_IN:
            ret = p_PrinterThermal->inThermal();
        break;
            
        case NVRAM_MP_IN:
            nvramWriteProtected_ = !nvramWriteProtected_;
            if (nvramWriteProtected_)
                return 1;
            else
                return 0;
        break;

        case SB_IN5:
            ret = usbIn();
        break;

        case SB_IN6:
            ret = usbIn6();
        break;

        case BIT_KEYPAD_IN_0:
            ret = bitkeypadPointer[0]->in();
        break;

        case BIT_KEYPAD_IN_1:
            ret = bitkeypadPointer[1]->in();
        break;

        case CV_KEYPAD_IN:
            ret = cvkeypadPointer->in();
        break;

        case KEYB_MATRIX_IN:
            ret = matrixKeyboardPointer->in(address);
        break;

        case PS2GPIOIN:
            ret = inPs2gpio();
        break;

        case CDP1864ENABLE:
            ret = cdp1864Pointer->inPixie();
        break;

        case CDP1864DISABLE:
            ret = 255;
            cdp1864Pointer->outPixie();
        break;

        case TAPE_CV_IN:
//            p_Main->startHwLoad();
            ret = lastTapeInpt_;
            tapedataReady_ = 1;
        break;


        // Folowing I/O is not adapted to ioGroups
        case TMSDATAPORT:
            ret = tmsPointer->readVRAM();
        break;

        case TMSREGISTERPORT:
            ret = tmsPointer->readDataPort();
        break;

        case PIXIEIN:
            ret = pixiePointer->inPixie();
        break;

        case PIXIEOUT:
            ret = 255;
            pixiePointer->outPixie();
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

        case PS2IN:
            ret = inPs2();
        break;

        case ELF2IN:
            switch (elfConfiguration.panelType_)
            {
                case PANEL_COSMAC:
                    if (elfConfiguration.useHexKeyboard)
                        ret = switches_;
                    else
                        ret = getData();
                break;

                case PANEL_ELF2K:
                    ret = 0;
                    if (elfConfiguration.useHexKeyboardEf3)
                        ret = switches_;
                    else
                    {
                        if (elfConfiguration.useSwitch)
                            ret = getData();
                        if (elfConfiguration.useHex)
                            ret = p_Elf2Khex->getData();
                    }
                break;

                case PANEL_MICROTUTOR:
                    inPressed_ = false;
                    ret = getData();
                break;

                case PANEL_MICROTUTOR2:
                    inPressed_ = false;
                    ret = getData();
 //                   bootstrap_ = 0;
                break;
                    
                default:
                    ret = getData();
                break;
            }
        break;

        case PORTEXTIN:
            ret = inPortExtender();
        break;

        case COMXIN:
            ret = inKeyb1871();
        break;

        case COMXDIAGIN1:
            if (diagRomActive_)
            {
                diagRomActive_ = false;
                if (elfConfiguration.usev1870)
                    p_Main->eventUpdateDiagLedStatus(1, diagRomActive_);
            }
            ret = getDiagInput();
        break;
            
        case COMXDIAGIN2:
            ret = 0;
            if (computerConfiguration.diagRomChecksum_ == 0)
                ret = 2;
            if (dmaCounter_ == -100)
                ret = ret ^ 4;
            if (computerConfiguration.diagFactory_ == 1)
                ret = ret ^ 8;
            // bit 1 ROM checksum
            // bit 2 IDEN itself
            // bit 3 keyboard debounce, repeat and IDEN
        break;
            
        case CIDELSAIN2:
            ret = inDip();
        break;

        case COSMICOSHEX:
            ret = p_Cosmicoshex->getKeyinput();
        break;

        case COSMICOS7SEG:
            segNumber_ = 0;
            ret = 255;
        break;
    }
   
    inValues_[port] = ret;
    return ret;
}

Byte Xmlemu::inDip()
{
    return elfConfiguration.ioConfiguration.dipValue;
}

Byte Xmlemu::getData()
{
    if (elfConfiguration.panelType_ == PANEL_COSMAC || elfConfiguration.panelType_ == PANEL_ELF2K || elfConfiguration.panelType_ == PANEL_MICROTUTOR || elfConfiguration.panelType_ == PANEL_MICROTUTOR2)
    {
        return(dataSwitchState_[7]?128:0) +(dataSwitchState_[6]?64:0) +(dataSwitchState_[5]?32:0) +(dataSwitchState_[4]?16:0) +(dataSwitchState_[3]?8:0) +(dataSwitchState_[2]?4:0) +(dataSwitchState_[1]?2:0) +(dataSwitchState_[0]?1:0);
    }
    else
        return switches_;
}

void Xmlemu::out(Byte port, Word address, Byte value)
{
    outValues_[port] = value;

    if (elfConfiguration.bootStrap)
    {
        if (port == elfConfiguration.ioConfiguration.bootStrapOut)
        {
            bootstrap_ = 0;
        }
    }

    if (elfConfiguration.useIoGroup)
    {
        if  (elfConfiguration.ioConfiguration.ioGroupOut == port)
            ioGroup_ = value & elfConfiguration.ioConfiguration.ioGroupMask;
    }

    if  (elfConfiguration.useDma)
        if (elfConfiguration.dmaOnFirstOut == port && dmaCounter_ == -1)
            dmaCounter_ = elfConfiguration.dmaCycleValue;

    switch (outType_[qState_][ioGroup_+1][port])
    {
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

        case UART16450_CONTROL:
             vtPointer->selectUart16450Register(value);
        break;

        case UART16450_CONTROLSERIAL:
            p_Serial->selectUart16450Register(value);
        break;

        case UART16450_OUT:
            vtPointer->uart16450Out(value);
        break;
            
        case UART16450_OUTSERIAL:
            p_Serial->uart16450Out(value);
        break;

        case IDESELECTOUT:
            selectIdeRegister(value);
        break;

        case IDEWRITEOUT:
            outIde(value);
        break;

        case RTCSELECT:
            selectRtcRegister(value);
        break;

        case RTCOUT:
            outRtc(value);
        break;

        case V1870OUT3:
            vis1870Pointer->out3_1870(value);
        break;
            
        case V1870OUT4:
            outValues_[port] = address;
            vis1870Pointer->out4_1870(address);
        break;
            
        case V1870OUT5:
            outValues_[port] = address;
            vis1870Pointer->out5_1870(address);
        break;
            
        case V1870OUT6:
            outValues_[port] = address;
            vis1870Pointer->out6_1870(address);
        break;
            
        case V1870OUT7:
            outValues_[port] = address;
            vis1870Pointer->out7_1870(address);
        break;

        case TELMACOUTREGISTER:
            vismacRegisterLatch_ = value;
        break;
            
        case TELMACOUTDATA:
            switch (vismacRegisterLatch_)
            {
                case 0x20:
                    vis1870Pointer->out2_1870(value);
                break;
                    
                case 0x30:
                    vis1870Pointer->out3_1870(value);
                break;
                
                case 0x40:
                    outValues_[port] = address;
                    vis1870Pointer->out4_1870(address);
                break;
                    
                case 0x50:
                    outValues_[port] = address;
                    vis1870Pointer->out5_1870(address);
                break;
                    
                case 0x60:
                    outValues_[port] = address;
                    vis1870Pointer->out6_1870(address);
                break;
                    
                case 0x70:
                    outValues_[port] = address;
                    vis1870Pointer->out7_1870(address);
                break;
            }
        break;

        case FDC1770_SELECTOUT:
            selectRegister1770(value);
        break;

        case FDC1770_WRITEOUT:
            writeRegister1770(value);
        break;

        case FDC1793_SELECTOUT:
            selectRegister1793(value);
        break;

        case FDC1793_WRITEOUT:
            writeRegister1793(value);
        break;

        case PARALLEL_PRINT_OUT:
            p_Main->eventPrintParallel(value);
        break;

        case SERIAL_PRINT_OUT:
            p_Main->eventPrintSerial(value);
        break;

        case THERMAL_PRINT_OUT:
            thermalPrinting_ = true;
            p_Main->eventPrintThermal(value, getFlipFlopQ());
        break;

        case BASIC_PRINT_OUT:
            if ((value & 0xfc) != 0)
                p_Printer->printerOut(value);
        break;

        case SB_OUT:
            usbOutNoEfCheck(value);
        break;

        case KEYB_LATCH_OUT:
            latchKeyboardPointer->out(value);
        break;
            
        case ELF2KGPIO:
            writeGpioControlRegister(value);
        break;

        case TAPE_CV_OUT:
       //     p_Main->startHwSave();
       //     tapeRecording_ = true;
            outSaveTapeHw(value);
        break;

        case BITSOUND_OUT:
            if ((value & elfConfiguration.ioConfiguration.bitSoundMask) == 1)
                toneElf2KOn();
            else
                toneElf2KOff();
        break;

        // Folowing I/O is not adapted to ioGroups
        case COMXOUT:
            slotOut(value);
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

        case PIXIEIN:
            pixiePointer->inPixie();
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

        case LEDMODOUT:
            ledModulePointer->write(value);
        break;

        case PS2OUT:
            outPs2(value);
        break;

        case ELF2OUT:
            switch (elfConfiguration.panelType_)
            {
                case PANEL_MICROTUTOR:
                    inPressed_ = false;
                break;

                case PANEL_MICROTUTOR2:
                    inPressed_ = false;
      //              bootstrap_ = 0;
                break;
                    
                default:
                break;
            }
            showData(value);
        break;

        case PORTEXTSELECTOUT:
            selectPortExtender(value);
        break;

        case PORTEXTWRITEOUT:
            outPortExtender(value);
        break;

        case EMSMAPPEROUT:
            for (size_t emsNumber=0; emsNumber<computerConfiguration.emsConfigNumber_; emsNumber++)
            {
                if (computerConfiguration.emsConfig_[emsNumber].outputEnd == port)
                    setEmsPage(emsNumber, value);
            }
        break;

        case COMXDIAGOUT1:
            if (computerConfiguration.diagFactory_ == 0)
                return;
            diagOut(value);
        break;

        case COSMICOSHEX:
            p_Cosmicoshex->keyLatch(value);
        break;

        case COSMICOS7SEG:
            p_Cosmicoshex->segUpdate(segNumber_, value);
            segNumber_++;
            segNumber_&=0x7;
        break;

        case COSMICOSTONE:
            tone1864Latch(value);
        break;
    }
}

void Xmlemu::printOutPecom(int q)
{
    if (!computerConfiguration.useQSerialPrinter_)
        return;
    
    cassetteEf_ = false;
    switch (printerStatus_)
    {
        case PRIDLE:
            if (q == 0)
                printerStatus_ = PRW_START;
        break;

        case PRW_START:
            if (q)
            {
                printerStatus_ = PRBUSY;
                p_Printer->startPecomChar();
                printBit_ = 0;
                printValue_ = 0;
            }
        break;

        case PRBUSY:
            if (q == 0)
                printValue_ += (1 <<(printBit_));
            if (printBit_ == 8)
            {
                p_Main->eventPrintPecom(printValue_);
                printerStatus_ = PRIDLE;
                if (elfConfiguration.useMatrixKeyboard)
                    matrixKeyboardPointer->clearReturn();
            }
            printBit_++;
            break;
    }
}

void Xmlemu::onXmlF4(bool WXUNUSED(forceStart))
{
    if (computerConfiguration.useBasicPrinter_)
    {
//        if  (elfConfiguration.ioConfiguration.printerIoGroup == ioGroup_ || elfConfiguration.ioConfiguration.printerIoGroup == -1 || forceStart)
//        {
        p_Printer->onF4();
        return;
//        }
    }

    if (computerConfiguration.useParallelPrinter_)
    {
//        if  (elfConfiguration.ioConfiguration.parallelPrinterIoGroup == ioGroup_ || elfConfiguration.ioConfiguration.parallelPrinterIoGroup == -1 || forceStart)
//        {
        p_PrinterParallel->onF4Parallel();
        return;
//        }
    }
    
    if (computerConfiguration.useSerialPrinter_)
    {
//        if  (elfConfiguration.ioConfiguration.serialPrinterIoGroup == ioGroup_ || elfConfiguration.ioConfiguration.serialPrinterIoGroup == -1 || forceStart)
//        {
            p_PrinterSerial->onF4Serial();
            return;
//        }
    }

    if (computerConfiguration.useQSerialPrinter_)
    {
        p_Printer->onF4();
        return;
    }

    if (computerConfiguration.useThermalPrinter_)
    {
//        if  (elfConfiguration.ioConfiguration.thermalPrinterIoGroup == ioGroup_ || elfConfiguration.ioConfiguration.thermalPrinterIoGroup == -1 || forceStart)
//        {
        p_PrinterThermal->onF4Thermal();
        return;
//        }
    }
}

void Xmlemu::slotOut(Byte value)
{
    Byte temp = slotShift(value, computerConfiguration.slotConfig_.slotOutputShift) & computerConfiguration.slotConfig_.slotOutputMask;
        
    if (computerConfiguration.slotConfig_.slotOutputNumber)
    {
        if (selectedSlot_ != temp)
        {
            p_Main->expLedOffEvent();
            selectedSlot_ = temp;
            p_Main->expLedOnEvent();
        }
    }
    else
    {
        Byte newSelectedSlot = 0;
        if (temp != 0)
        {
            while ((temp & 1) == 0)
            {
                newSelectedSlot++;
                temp = temp >> 1;
            }
        }
        if (selectedSlot_ != newSelectedSlot)
        {
            p_Main->expLedOffEvent();
            selectedSlot_ = newSelectedSlot;
            p_Main->expLedOnEvent();
        }
    }
    if (computerConfiguration.slotConfig_.useIoGroup)
        ioGroup_ = selectedSlot_;

    temp = slotShift(value, computerConfiguration.slotConfig_.bankOutputShift) & computerConfiguration.slotConfig_.bankOutputMask;

    if (computerConfiguration.slotConfig_.bankOutputNumber)
    {
        selectedBank_ = temp;
    }
    else
    {
        if (temp != 0)
        {
            selectedBank_ = 0;
            while ((temp & 1) == 0)
            {
                selectedBank_++;
                temp = temp >> 1;
            }
        }
    }
}

Byte Xmlemu::slotShift(Byte value, int shift)
{
    Byte circleBit;
    while (shift > 0)
    {
        circleBit = (value & 1) << 7;
        value = (value >> 1) | circleBit;
        shift--;
    }
    while (shift < 0)
    {
        circleBit = (value & 0x80) >> 7;
        value = (value << 1) | circleBit;
        shift++;
    }
    return value;
}

void Xmlemu::showData(Byte val)
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            if (elfConfiguration.tilType == TIL311)
                elfScreenPointer->showData(val);
            else
                elfScreenPointer->showDataTil313Italic(val);
        break;

        case PANEL_ELFII:
            if (elfConfiguration.tilType == TIL311)
                elf2ScreenPointer->showData(val);
            else
                elf2ScreenPointer->showDataTil313Italic(val);
        break;

        case PANEL_SUPER:
            if (elfConfiguration.tilType == TIL311)
                superScreenPointer->showData(val);
            else
                superScreenPointer->showDataTil313Italic(val);
        break;

        case PANEL_ELF2K:
            elf2KScreenPointer->showData(val);
        break;

        case PANEL_COSMICOS:
            cosmicosScreenPointer->showDataTil313(val);
        break;

        case PANEL_MICROTUTOR:
            microtutorScreenPointer->showData(val);
        break;

        case PANEL_MICROTUTOR2:
            microtutor2ScreenPointer->showDataTil313Italic(val);
        break;
    }
}

void Xmlemu::showCycleData(Byte val)
{
    if (singleStateStep_)
        showData(val);
}

void Xmlemu::cycle(int type)
{
    switch(cycleType_[type])
    {
        case 0:
            return;
        break;

        case KEYBRDCYCLE:
            cycleKeyboardXml();
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

        case CDP1864CYCLE:
            cdp1864Pointer->cyclePixieTelmac();
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

        case SN76430NCYCLE:
            sn76430nPointer->cycle();
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

        case V1870CYCLE:
            vis1870Pointer->cycle1870();
        break;
            
        case V1870BLINK:
            vis1870Pointer->blink1870();
        break;

        case COMXCYCLE:
            cycleKeyb1871();
        break;

        case THERMALCYCLE:
            p_PrinterThermal->cycleThermal();
        break;
            
        case DRAMDMA:
            cycleDma();
        break;
            
        case INTCLOCK:
            cycleInt();
        break;
            
        case LATCHKEYBCYCLE:
            latchKeyboardPointer->cycleKeybLatch();
        break;

        case COSMICOS7SEG:
            cycleValue_ --;
            if (cycleValue_ <= 0)
            {
                if (elfConfiguration.use1864)
                {
                    if (!cdp1864Pointer->arePixieGraphicsOn())
                        interrupt();
                }
                cycleValue_ = cycleSize_;
            }
        break;
    }
}

void Xmlemu::cycleDma()
{
    if (flipFlopQ_)
    {
        if (dmaCounter_ == -1)
            dmaCounter_ = elfConfiguration.dmaCycleValue;
    }

    if (dmaCounter_ > 0)
    {
        dmaCounter_--;
        if (dmaCounter_ <= 0)
        {
            dmaOut();
            dmaCounter_ = elfConfiguration.dmaCycleValue;
            if (!diagDmaLedOn_ && computerConfiguration.useDiagnosticBoard_)
            {
                diagDmaLedOn_ = true;
                if (elfConfiguration.usev1870)
                    p_Main->eventUpdateDiagLedStatus(2, diagDmaLedOn_);
            }
        }
    }
}

void Xmlemu::cycleInt()
{
    if (intCounter_ >= 0)
    {
        intCounter_--;
        if (intCounter_ == 0)
        {
            intCounter_ = elfConfiguration.intCycleValue;
            p_Computer->interrupt();
        }
    }
}

void Xmlemu::cycleLed()
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
            switch (elfConfiguration.panelType_)
            {
                case PANEL_COSMAC:
                    elfScreenPointer->ledTimeout();
                    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                        keypadPointer->ledTimeout();
                    if (elfConfiguration.useLedModule && !ledModuleClosed_)
                        ledModulePointer->ledTimeout();
                break;

                case PANEL_ELFII:
                    elf2ScreenPointer->ledTimeout();
                break;

                case PANEL_SUPER:
                    superScreenPointer->ledTimeout();
                break;

                case PANEL_ELF2K:
                    elf2KScreenPointer->ledTimeout();
                break;

                case PANEL_COSMICOS:
                    cosmicosScreenPointer->ledTimeout();
                    p_Cosmicoshex->ledTimeout();
                break;

                case PANEL_MICROTUTOR:
                    microtutorScreenPointer->ledTimeout();
                break;
                    
                case PANEL_MICROTUTOR2:
                    microtutor2ScreenPointer->ledTimeout();
                break;
            }
        }
        if (goCycleValue_ > 0)
        {
            goCycleValue_ --;
            if (goCycleValue_ <= 0)
            {
                goCycleValue_ = goCycleSize_;
                wxMouseState mouseState = wxGetMouseState ();
                
                if (mouseState.LeftIsDown())
                    setWait(1);
                else
                    goCycleValue_ = -1;
            }
        }
    }
}

void Xmlemu::setGoTimer()
{
    goCycleValue_ = goCycleSize_;
}

void Xmlemu::showState(int state)
{
    if (elfConfiguration.panelType_ != PANEL_SUPER)
        return;

    switch (state)
    {
        case STATE_FETCH:
            superScreenPointer->setLed(4, 1);
            superScreenPointer->setLed(5, 0);
        break;

        case STATE_EXECUTE:
            superScreenPointer->setLed(4, 0);
            superScreenPointer->setLed(5, 1);
        break;
            
        default:
            superScreenPointer->setLed(4, 0);
            superScreenPointer->setLed(5, 0);
        break;
    }
    superScreenPointer->setLed(6, 0);
    superScreenPointer->setLed(7, 0);
}

void Xmlemu::showDmaLed()
{
    if (elfConfiguration.panelType_ != PANEL_SUPER)
        return;

    superScreenPointer->setLed(6, 1);
}

void Xmlemu::showIntLed()
{
    if (elfConfiguration.panelType_ != PANEL_SUPER)
        return;

    superScreenPointer->setLed(7, 1);
}

void Xmlemu::autoBoot()
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            elfScreenPointer->runSetState(BUTTON_UP);
            runButtonState_ = 1;
            setClear(runButtonState_);
            if (cpuMode_ == RESET)
            {
                if (elfConfiguration.tilType == TIL311)
                    elfScreenPointer->showAddress(0);
                else
                    elfScreenPointer->showAddressTil313Italic(0);
            }
        break;

        case PANEL_ELFII:
            elf2ScreenPointer->runSetState(BUTTON_UP);
            runButtonState_ = 1;
            setClear(runButtonState_);
        break;

        case PANEL_SUPER:
            setClear(1);
            setWait(1);
        break;
            
        case PANEL_ELF2K:
            runButtonState_ = 1;
            if (elfConfiguration.useSwitch)
                p_Elf2Kswitch->setRunButtonUp(runButtonState_);
            if (elfConfiguration.useHex)
                p_Elf2Khex->setRunButtonUp(runButtonState_);

            setClear(runButtonState_);
            if (cpuMode_ == RESET)
                elf2KScreenPointer->showAddress(0);
        break;
            
        case PANEL_COSMICOS:
            runButtonState_ = 1;

            setClear(runButtonState_);
            segNumber_ = 0;
        break;
            
        case PANEL_MICROTUTOR:
            setClear(1);
        break;
            
        case PANEL_MICROTUTOR2:
            microtutor2ScreenPointer->runSetState(BUTTON_UP);
            runButtonState_ = 1;
            setClear(runButtonState_);
        break;
            
        default:
            runButtonState_ = 1;
            setClear(runButtonState_);
        break;
    }
}

void Xmlemu::switchQ(int value)
{
    if (elfConfiguration.useTapeHw && qState_ != value)
    {
        qState_ = value;
        if (qState_ == 0)
        {
            if (tapeActivated_ || tapeRecording_)
            {
                p_Main->turboOff();
                if (elfConfiguration.tape_stopDelay <= 0 || tapeRecording_)
                    p_Computer->pauseTape();
                else
                    pauseTapeCounter_ = (elfConfiguration.tape_stopDelay * sampleRate_) / 1000;
            }
        }
        else
        {
            pauseTapeCounter_ = 0;
            if (p_Main->getHwTapeState() == HW_TAPE_STATE_REC)
            {
                if (tapeRecording_)
                    restartHwTapeSave(TAPE_RECORD);
                else
                {
                    p_Main->startSaveCont(0, tapeCounter_);
                    tapeRecording_ = true;
                }
            }
            else
                startLoad(0, false);
        }
    }

    qState_ = value;
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            elfScreenPointer->setQLed(value);
        break;

        case PANEL_ELFII:
            elf2ScreenPointer->setQLed(value);
        break;

        case PANEL_SUPER:
            superScreenPointer->setQLed(value);
        break;

        case PANEL_ELF2K:
            elf2KScreenPointer->setQLed(value);
        break;

        case PANEL_MICROTUTOR2:
            microtutor2ScreenPointer->showDp313Italic(value == 1);
        break;

        case PANEL_COSMICOS:
            elfScreenPointer->setQLed(value);
        break;
    }

    if (elfConfiguration.vtType != VTNONE)
        vtPointer->switchQ(value);

    if (elfConfiguration.vtExternal)
        p_Serial->switchQ(value);
    
    if (computerConfiguration.useDiagnosticBoard_)
    {
        if (computerConfiguration.diagCassetteCables_ == 1)
            cassetteEf_ = value;
    }
}

int Xmlemu::getMpButtonState()
{
    return mpButtonState_;
}

void Xmlemu::onRunButton(wxCommandEvent&WXUNUSED(event))
{
    onRunButton();
}

void Xmlemu::onRunButton()
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMICOS:
            singleStateStep_ = false;
            resetEffectiveClock();
            setClear(1);
            setWait(1);
            p_Main->eventUpdateTitle();
        break;
            
        case PANEL_MICROTUTOR2:
            onRun();
        break;
            
        default:
            setClear(1);
            setWait(1);
            p_Main->eventUpdateTitle();
            
            if (cpuMode_ != RUN)
                resetEffectiveClock();
        break;
    }
}

void Xmlemu::onMouseRelease(wxMouseEvent&event)
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMICOS:
            cosmicosScreenPointer->onMouseRelease(event);
        break;
    }
}

void Xmlemu::onRunButtonPress()
{
    onRun();
    microtutorScreenPointer->runSetState(BUTTON_DOWN);
}

void Xmlemu::onRunButtonRelease()
{
    microtutorScreenPointer->runSetState(BUTTON_UP);
}

void Xmlemu::onRun()
{
    if (elfConfiguration.bootStrap)
        bootstrap_ = elfConfiguration.strapAddress;

    stopTape();
    
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
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
                if (elfConfiguration.tilType == TIL311)
                    elfScreenPointer->showAddress(0);
                else
                    elfScreenPointer->showAddressTil313Italic(0);
            }
        break;

        case PANEL_ELFII:
            if (runButtonState_)
            {
                elf2ScreenPointer->runSetState(BUTTON_DOWN);
                runButtonState_ = 0;
            }
            else
            {
                elf2ScreenPointer->runSetState(BUTTON_UP);
                runButtonState_ = 1;
                p_Main->startTime();
            }
            
            setClear(runButtonState_);
            p_Main->eventUpdateTitle();
        break;

        case PANEL_SUPER:
            if (getClear()==0)
            {
                setClear(1);
                setWait(1);
                p_Main->eventUpdateTitle();
                resetEffectiveClock();
            }
            else
            {
                setClear(0);
                setWait(1);
                p_Main->eventUpdateTitle();
                if (cpuMode_ == RESET)
                {
                    if (elfConfiguration.tilType == TIL311)
                        superScreenPointer->showAddress(0);
                    else
                        superScreenPointer->showAddressTil313Italic(0);
                }
                mpButtonState_ = 0;
                monitor_ = false;
            }
        break;

        case PANEL_ELF2K:
            if (runButtonState_)
            {
                runButtonState_ = 0;
                setClear(runButtonState_);
                resetVideo();
            }
            else
            {
                runButtonState_ = 1;
                p_Main->startTime();
                setClear(runButtonState_);
                resetComputer();
            }

            if (elfConfiguration.useSwitch)
                p_Elf2Kswitch->setRunButtonUp(runButtonState_);
            if (elfConfiguration.useHex)
                p_Elf2Khex->setRunButtonUp(runButtonState_);

            p_Main->eventUpdateTitle();
            if (cpuMode_ == RESET)
                elf2KScreenPointer->showAddress(0);
        break;

        case PANEL_COSMICOS:
            stopTape();
            if (getClear()==0)
            {
                setClear(1);
                setWait(1);
                p_Main->eventUpdateTitle();
                resetEffectiveClock();
            }
            else
            {
                setClear(0);
                setWait(1);
                segNumber_ = 0;
                cdp1864Pointer->setPixieGraphics(false);
                p_Main->eventUpdateTitle();
            }
        break;

        case PANEL_MICROTUTOR:
            showData(dmaOut());
            if (getCpuMode() != LOAD)
            {
                setClear(1);
                setWait(1);
                p_Main->eventUpdateTitle();
                p_Main->startTime();
            }
        break;

        case PANEL_MICROTUTOR2:
  //**          if (microtutorConfiguration.utilityMemory)
  //**              bootstrap_ = 0x8000;
            
            if (runButtonState_)
            {
                microtutor2ScreenPointer->runSetState(BUTTON_DOWN);
                runButtonState_ = 0;
            }
            else
            {
                microtutor2ScreenPointer->runSetState(BUTTON_UP);
                runButtonState_ = 1;
                p_Main->startTime();
            }
            setClear(runButtonState_);
            p_Main->eventUpdateTitle();
        break;
            
        default:
            if (getClear()==0)
            {
                setClear(1);
                setWait(1);
                p_Main->eventUpdateTitle();
                resetEffectiveClock();
            }
            else
            {
                setClear(0);
                setWait(1);
                p_Main->eventUpdateTitle();
            }
        break;
    }
}

void Xmlemu::onPause(wxCommandEvent&WXUNUSED(event))
{
    onPause();
}

void Xmlemu::onPause()
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMICOS:
            singleStateStep_ = true;
            setClear(1);
            setWait(0);
            cdp1864Pointer->setPixieGraphics(false);
        break;

        default:
            setWait(0);
            mpButtonState_ = 0;
        break;
    }
    p_Main->eventUpdateTitle();
}

void Xmlemu::onMpButton(wxCommandEvent&WXUNUSED(event))
{
    mpButtonState_ = 1;
}

void Xmlemu::onMpButton()
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_SUPER:
            mpButtonState_ = 1;
        break;

        case PANEL_ELF2K:
            if (mpButtonState_)
                mpButtonState_ = 0;
            else
                mpButtonState_ = 1;

            if (elfConfiguration.useSwitch)
                p_Elf2Kswitch->setMpButtonUp(mpButtonState_);
            if (elfConfiguration.useHex)
                p_Elf2Khex->setMpButtonUp(!mpButtonState_);
        break;

        default:
            if (mpButtonState_)
                mpButtonState_ = 0;
            else
                mpButtonState_ = 1;
        break;
    }
}

void Xmlemu::onMonitor(wxCommandEvent&WXUNUSED(event))
{
    onMonitor();
}

void Xmlemu::onMonitor()
{
    if (baseGiantBoard_ != 0x10000)
    {
        setClear(1);
        setWait(1);
        scratchpadRegister_[0] = baseGiantBoard_;
    }
    else monitor_ = true;
}

void Xmlemu::onLoadButton(wxCommandEvent&WXUNUSED(event))
{
    onLoadButton();
}

void Xmlemu::onLoadButton()
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            if (!loadButtonState_)
                loadButtonState_ = 1;
            else
                loadButtonState_ = 0;
            setWait(loadButtonState_);
            if (cpuMode_ == RESET)
            {
                if (elfConfiguration.tilType == TIL311)
                    elfScreenPointer->showAddress(0);
                else
                    elfScreenPointer->showAddressTil313Italic(0);
            }
        break;
            
        case PANEL_SUPER:
            if (cpuMode_ != LOAD)
            {
                if (elfConfiguration.tilType == TIL311)
                    superScreenPointer->showAddress(0);
                else
                    superScreenPointer->showAddressTil313Italic(0);
            }
            setClear(0);
            setWait(0);
        break;

        case PANEL_ELF2K:
            if (!loadButtonState_)
            {
                loadButtonState_ = 1;
                clearBootstrap();
            }
            else
                loadButtonState_ = 0;

            if (elfConfiguration.useSwitch)
                p_Elf2Kswitch->setLoadButtonUp(!loadButtonState_);
            if (elfConfiguration.useHex)
                p_Elf2Khex->setLoadButtonUp(loadButtonState_);
            
            setWait(loadButtonState_);
            resetVideo();
            if (getCpuMode() == RESET)
                elf2KScreenPointer->showAddress(0);
        break;

        case PANEL_COSMICOS:
            setClear(0);
            setWait(0);
            segNumber_ = 0;
            if (elfConfiguration.use1864)
            {
                cdp1864Pointer->outPixie();
                cdp1864Pointer->setPixieGraphics(false);
            }
        break;
            
        default:
            if (!loadButtonState_)
                loadButtonState_ = 1;
            else
                loadButtonState_ = 0;
            setWait(loadButtonState_);
        break;
    }
}

void Xmlemu::onClearButtonPress()
{
    setClear(0);
    resetCpu();

    microtutorScreenPointer->clearSetState(BUTTON_DOWN);
}

void Xmlemu::onClearButtonRelease()
{
    microtutorScreenPointer->clearSetState(BUTTON_UP);
}

void Xmlemu::onSingleStep(wxCommandEvent&WXUNUSED(event))
{
    onSingleStep();
}

void Xmlemu::onSingleStep()
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMICOS:
            setWait(1);
        break;

        default:
            singleStateStep_ = !singleStateStep_;
            if (singleStateStep_)
            {
                setMsValue_ = (int) p_Main->getLedTimeMs(SUPERELF);
                setLedMs(0);
            }
            else
                setLedMs(setMsValue_);
        break;
    }
}

void Xmlemu::onResetButton(wxCommandEvent&WXUNUSED(event))
{
    onResetButton();
}

void Xmlemu::onResetButton()
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMICOS:
            singleStateStep_ = false;
            lastMode_ = UNDEFINDEDMODE;
            setClear(0);
            setWait(1);
            segNumber_ = 0;
            cdp1864Pointer->setPixieGraphics(false);
        break;

        default:
            singleStateStep_ = false;
            lastMode_ = UNDEFINDEDMODE;
            setClear(0);
            setWait(1);
            if (cpuMode_ == RESET)
            {
                if (elfConfiguration.tilType == TIL311)
                    superScreenPointer->showAddress(0);
                else
                    superScreenPointer->showAddressTil313Italic(0);
            }
            mpButtonState_ = 0;
            monitor_ = false;
        break;
    }
}

void Xmlemu::dataSwitch(int i)
{
    if (dataSwitchState_[i])
    {
        dataSwitchState_[i] = 0;
    }
    else
    {
        dataSwitchState_[i] = 1;
    }
    if (elfConfiguration.useSwitch)
        p_Elf2Kswitch->setDataSwitchUp(i, dataSwitchState_[i]);
}

void Xmlemu::efSwitch(int i)
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

void Xmlemu::onNumberKeyDown(wxCommandEvent&event)
{
    int id = event.GetId();
    onNumberKeyDown(id);
}

void Xmlemu::onNumberKeyDown(int id)
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                keypadPointer->onNumberKeyDown(id);
            else
            {
                hexEfState_ = 0;
                switches_ = ((switches_ << 4) & 0xf0) | id;
            }
        break;
            
        case PANEL_ELF2K:
            if (elfConfiguration.useHex)
                p_Elf2Khex->onNumberKeyDown(id);
        break;
            
        default:
            hexEfState_ = 0;
            switches_ = ((switches_ << 4) & 0xf0) | id;
        break;
    }
}

void Xmlemu::onNumberKeyUp(wxCommandEvent&WXUNUSED(event))
{
    hexEfState_ = 1;
}

void Xmlemu::onNumberKeyUp()
{
    hexEfState_ = 1;
}

void Xmlemu::onHexKeyDown(int keycode)
{
#if defined (__WXMAC__)
    if (hexEfState_ == 0) // This is to avoid multiple key presses on OSX
        return;
#endif
    
    for (int i=0; i<16; i++)
    {
        if (keycode == keyDefA1_[i])
        {
            switch (elfConfiguration.panelType_)
            {
                case PANEL_COSMAC:
                    hexEfState_ = 0;
                    switches_ = ((switches_ << 4) & 0xf0) | i;
                    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                        keypadPointer->onNumberDown(i);
                break;

                case PANEL_COSMICOS:
                    p_Cosmicoshex->onNumberKeyPress(i);
                break;

                default:
                    hexEfState_ = 0;
                    switches_ = ((switches_ << 4) & 0xf0) | i;
                break;
            }
        }
        if (keycode == keyDefA2_[i])
        {
            switch (elfConfiguration.panelType_)
            {
                case PANEL_COSMAC:
                    hexEfState_ = 0;
                    switches_ = ((switches_ << 4) & 0xf0) | i;
                    if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                        keypadPointer->onNumberDown(i);
                break;

                case PANEL_COSMICOS:
                    p_Cosmicoshex->onNumberKeyPress(i);
                break;

                default:
                    hexEfState_ = 0;
                    switches_ = ((switches_ << 4) & 0xf0) | i;
                break;
            }
        }
    }
}

void Xmlemu::onHexDown(int hex)
{
    hexEfState_ = 0;
    switches_ = ((switches_ << 4) & 0xf0) | hex;
}

void Xmlemu::onHexKeyUp(int keycode)
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMICOS:
            for (int i=0; i<16; i++)
            {
                if (keycode == keyDefA1_[i] || keycode == keyDefA2_[i])
                    p_Cosmicoshex->onNumberKeyRelease(i);
            }
        break;

        default:
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

            hexEfState_ = 1;
        break;
    }
}

void Xmlemu::startComputer()
{
    configureMemory();
    
    resetPressed_ = false;
   
    p_Main->enableDebugGuiMemory();

    configureExtensions();
    
    if (!p_Main->getUseElfControlWindows(XML))
    {
        elfConfiguration.useSwitch = false;
        elfConfiguration.useHex = false;
    }
    
    switch (elfConfiguration.panelType_)
    {
        case PANEL_ELF2K:
            if (elfConfiguration.useSwitch)
            {
                p_Elf2Kswitch = new Elf2Kswitch("Switch Panel", p_Main->getElf2KswitchPos(), wxSize(451, 166));
                p_Elf2Kswitch->Show(elfConfiguration.useSwitch);
            }
            if (elfConfiguration.useHex)
            {
                p_Elf2Khex = new Elf2Khex("Hex Panel", p_Main->getKeypadPos(XML), wxSize(186, 126));
                p_Elf2Khex->Show(elfConfiguration.useHex);
            }
        break;

        case PANEL_COSMICOS:
            p_Elf2Khex = new Elf2Khex("Hex Panel", p_Main->getKeypadPos(XML), wxSize(186, 126));
            p_Elf2Khex->Show(elfConfiguration.useHex);
        break;
    }

    if (elfConfiguration.useKeyboard)
        startElfKeyFile("Xml");
    if (elfConfiguration.useKeyb1871)
        start1871KeyFile();
    if (elfConfiguration.useLatchKeyboard)
        latchKeyboardPointer->startKeyFile();
    if (elfConfiguration.useMatrixKeyboard)
        matrixKeyboardPointer->startKeyFile();
    if (elfConfiguration.usePs2gpio)
        startPs2gpioKeyFile();

    if (elfConfiguration.autoBoot)
    {
        scratchpadRegister_[0]=p_Main->getBootAddress("Xml", XML);
        autoBoot();
    }
    else
    {
        if (baseGiantBoard_ != 0x10000)
        {
            scratchpadRegister_[0] = baseGiantBoard_;
            autoBoot();
        }
    }

    if (elfConfiguration.tapeStart)
    {
        tapeActivated_ =  p_Main->startCassetteLoad(0);
//        if (tapeActivated_)
//            tapeRunSwitch_ = tapeRunSwitch_ | 1;
    }

    if (elfConfiguration.vtType != VTNONE)
        setEf(elfConfiguration.ioConfiguration.vt100Ef,0);

    p_Main->setSwName("");
    p_Main->updateTitle();
    address_ = 0;

    cpuCycles_ = 0;
    instructionCounter_= 0;
    p_Main->startTime();

    day_ = xmlComputerTime_.GetDay();
    month_ = xmlComputerTime_.GetMonth();
    year_ = xmlComputerTime_.GetYear();

    loadRtc();
    rtcCycle_ = 4;
    rtcTimerPointer->Start(250, wxTIMER_CONTINUOUS);

    int ms = (int) p_Main->getLedTimeMs(XML);
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            if (elfConfiguration.tilType == TIL311)
                elfScreenPointer->showAddress(0);
            else
                elfScreenPointer->showAddressTil313Italic(0);
            elfScreenPointer->setLedMs(ms);
            if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                keypadPointer->setLedMs(ms);
            if (elfConfiguration.useLedModule && !ledModuleClosed_)
                ledModulePointer->setLedMs(ms);
        break;

        case PANEL_ELFII:
            elf2ScreenPointer->setLedMs(ms);
        break;

        case PANEL_SUPER:
            if (elfConfiguration.tilType == TIL311)
                superScreenPointer->showAddress(address_);
            else
                superScreenPointer->showAddressTil313Italic(address_);
            superScreenPointer->setLedMs(ms);
        break;

        case PANEL_ELF2K:
            elf2KScreenPointer->setLedMs(ms);
        break;

        case PANEL_COSMICOS:
            cosmicosScreenPointer->setLedMs(ms);
            p_Cosmicoshex->setLedMs(ms);
        break;

        case PANEL_MICROTUTOR:
            microtutorScreenPointer->setLedMs(ms);
        break;

        case PANEL_MICROTUTOR2:
            microtutor2ScreenPointer->setLedMs(ms);
        break;
    }
    setForceUpperCase(elfConfiguration.forceUpperCase);

    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((elfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
    goCycleSize_ = (((elfClockSpeed_ * 1000000) / 8) / 1000) * 500;
    goCycleValue_ = -1;

    if (elfConfiguration.bootStrap)
        bootstrap_ = elfConfiguration.strapAddress;

    switch (elfConfiguration.ioConfiguration.statusBarType)
    {
        case STATUSBAR_COMX:
        case STATUSBAR_DIAG:
            if (elfConfiguration.usev1870)
            {
                vis1870Pointer->reDrawBar();
                vis1870Pointer->updateExpansionLed(true);
            }
        break;
    }

    threadPointer->Run();
}

void Xmlemu::loadRomRam(size_t configNumber)
{
    if (computerConfiguration.memConfig_[configNumber].filename != "")
    {
        if (computerConfiguration.memConfig_[configNumber].verifyFileExist)
            p_Main->checkAndReInstallFile(computerConfiguration.memConfig_[configNumber].dirname + computerConfiguration.memConfig_[configNumber].filename, XML, computerConfiguration.memConfig_[configNumber].filename);

        readProgram(computerConfiguration.memConfig_[configNumber].dirname, computerConfiguration.memConfig_[configNumber].filename, computerConfiguration.memConfig_[configNumber].type,  computerConfiguration.memConfig_[configNumber].start, NONAME); // type & 0xff causes loading ROM to end up without congif number in the higher 8 bit.
    }
}

void Xmlemu::writeMemDataType(Word address, Byte type)
{
    size_t number = (memoryType_[address / 256] >> 8);
    address = address | bootstrap_;
    
    switch (memoryType_[address/256]&0xff)
    {
        case EMSMEMORY:
            switch (emsMemory_[number].memoryType_[((address - computerConfiguration.emsConfig_[number].start) |(computerConfiguration.emsConfig_[number].page << computerConfiguration.emsConfig_[number].maskBits))/256])
            {
                case ROM:
                case RAM:
                    if (emsMemory_[number].dataType_[(long) ((address - computerConfiguration.emsConfig_[number].start) |(computerConfiguration.emsConfig_[number].page << computerConfiguration.emsConfig_[number].maskBits))] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        emsMemory_[number].dataType_[(long) ((address - computerConfiguration.emsConfig_[number].start) |(computerConfiguration.emsConfig_[number].page << computerConfiguration.emsConfig_[number].maskBits))] = type;
                    }
                    increaseExecutedEms(number, (long) ((address - computerConfiguration.emsConfig_[number].start) |(computerConfiguration.emsConfig_[number].page << computerConfiguration.emsConfig_[number].maskBits)), type);
                break;
            }
        break;
    
        case SLOTMEM:
            switch (slotMemoryType_[selectedSlot_][((selectedBank_&computerConfiguration.slotConfig_.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-computerConfiguration.slotConfig_.start)/256])
            {
                case ROM:
                case RAM:
                    if (slotMemoryDataType_[selectedSlot_][(selectedBank_&computerConfiguration.slotConfig_.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-computerConfiguration.slotConfig_.start] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        slotMemoryDataType_[selectedSlot_][(selectedBank_&computerConfiguration.slotConfig_.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-computerConfiguration.slotConfig_.start] = type;
                    }
                    increaseExecutedSlotMemory(selectedSlot_, (selectedBank_&computerConfiguration.slotConfig_.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-computerConfiguration.slotConfig_.start, type);
                break;
            }
        break;

        case MAPPEDROM:
        case ROM:
        case DIAGROM:
            if (elfConfiguration.giantBoardMapping)
            {
                if (address >= baseGiantBoard_)
                    address = (address & 0xff) | 0xf000;
            }
            else
            {
                if (computerConfiguration.memConfig_[number].useMemMask)
                address = (address & computerConfiguration.memConfig_[number].memMask) | computerConfiguration.memConfig_[number].start;
            }
            
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            increaseExecutedMainMemory(address, type);
        break;

        case MAINRAM:
        case MAPPEDRAM:
        case RAM:
        case NVRAM:
        case SN76430NRAM:
        case MC6847RAM:
            if (computerConfiguration.memConfig_[number].useMemMask)
            {
                address = (address & computerConfiguration.memConfig_[number].memMask) | computerConfiguration.memConfig_[number].start;
            }
            
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            increaseExecutedMainMemory(address, type);
        break;

        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_))/256])
            {
                case ROM:
                case RAM:
                    if (pagerMemoryDataType_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)] != type)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        pagerMemoryDataType_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)] = type;
                    }
                    increaseExecutedMainMemory((getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_), type);
                break;
            }
        break;

        case PRAM1870:
            if (elfConfiguration.ioConfiguration.v1870pageMemRom)
            {
                if (mainMemoryDataType_[address] != type)
                {
                    p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                    mainMemoryDataType_[address] = type;
                }
            }
            increaseExecutedMainMemory(address, type);
        break;
    }
}

Byte Xmlemu::readMemDataType(Word address, uint64_t* executed)
{
    address = address | bootstrap_;
    size_t number = (memoryType_[address / 256] >> 8);

    switch (memoryType_[address/256]&0xff)
    {
        case EMSMEMORY:
            switch (emsMemory_[number].memoryType_[((address - computerConfiguration.emsConfig_[number].start) |(computerConfiguration.emsConfig_[number].page << computerConfiguration.emsConfig_[number].maskBits))/256])
            {
                case ROM:
                case RAM:
                    if (profilerCounter_ != PROFILER_OFF)
                        *executed = emsMemory_[number].executed_[(long) ((address - computerConfiguration.emsConfig_[number].start) |(computerConfiguration.emsConfig_[number].page << computerConfiguration.emsConfig_[number].maskBits))];
                    return emsMemory_[number].dataType_[(long) ((address - computerConfiguration.emsConfig_[number].start) |(computerConfiguration.emsConfig_[number].page << computerConfiguration.emsConfig_[number].maskBits))];
                break;
            }
        break;

        case SLOTMEM:
            switch (slotMemoryType_[selectedSlot_][((selectedBank_&computerConfiguration.slotConfig_.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-computerConfiguration.slotConfig_.start)/256])
            {
                case ROM:
                case RAM:
                    if (profilerCounter_ != PROFILER_OFF)
                        *executed = slotMemoryExecuted_[selectedSlot_][(selectedBank_&computerConfiguration.slotConfig_.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-computerConfiguration.slotConfig_.start];
                    return slotMemoryDataType_[selectedSlot_][(selectedBank_&computerConfiguration.slotConfig_.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-computerConfiguration.slotConfig_.start];
                break;
            }
        break;

        case MAPPEDROM:
        case ROM:
        case DIAGROM:
            if (elfConfiguration.giantBoardMapping)
            {
                if (address >= baseGiantBoard_)
                    address = (address & 0xff) | 0xf000;
            }
            else
            {
                if (computerConfiguration.memConfig_[number].useMemMask)
                    address = (address & computerConfiguration.memConfig_[number].memMask) | computerConfiguration.memConfig_[number].start;
            }

            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;

        case MAINRAM:
        case MAPPEDRAM:
        case RAM:
        case NVRAM:
        case SN76430NRAM:
        case MC6847RAM:
            if (computerConfiguration.memConfig_[number].useMemMask)
            {
                address = (address & computerConfiguration.memConfig_[number].memMask) | computerConfiguration.memConfig_[number].start;
            }

            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;

        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_))/256])
            {
                case ROM:
                case RAM:
                    if (profilerCounter_ != PROFILER_OFF)
                        *executed = pagerMemoryExecuted_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)];
                    return pagerMemoryDataType_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)];
                break;
            }
        break;

        case PRAM1870:
            if (elfConfiguration.ioConfiguration.v1870pageMemRom)
            {
                if (profilerCounter_ != PROFILER_OFF)
                    *executed = mainMemoryExecuted_[address];
                return mainMemoryDataType_ [address];
            }
            else
                return MEM_TYPE_UNDEFINED;
        break;
    }
    return MEM_TYPE_UNDEFINED;
}

Byte Xmlemu::readMem(Word address)
{
    address_ = address;

    if (address >= elfConfiguration.strapAddress && elfConfiguration.ioConfiguration.bootStrapType == BOOTSTRAPREAD)
        bootstrap_ = 0;

    address = address | bootstrap_;

    return readMemDebug(address);
}

Byte Xmlemu::readMemDebug(Word address)
{
    wxDateTime systemNow;
    wxDateTime now;
    wxTimeSpan timeDiff;
    Byte value;
    int year;
    Byte high, low, rtcControl;
    
    Byte minimon[] = { 0xf8, 0xff, 0xa1, 0xe1, 0x6c, 0x64, 0xa3, 0x21,
        0x6c, 0x64, 0x3f, 0x07, 0x37, 0x0c, 0x3a, 0x11,
        0xd3, 0xe3, 0xf6, 0x33, 0x17, 0x7b, 0x6c, 0x64,
        0x23, 0x3f, 0x13, 0x37, 0x1b, 0x13, 0x30, 0x13 };
    
    for (size_t i=0; i<computerConfiguration.copyConfigNumber_; i++)
    {
        if (address >= computerConfiguration.copyConfig_[i].start && address <=   computerConfiguration.copyConfig_[i].end)
            if (computerConfiguration.copyConfig_[i].slot == selectedSlot_ || computerConfiguration.copyConfig_[i].slot == -1)
                return (readMemDebug(address + computerConfiguration.copyConfig_[i].copy));
    }
    
    if (elfConfiguration.use6845)
    {
        if  (elfConfiguration.ioConfiguration.mc6845IoGroup == ioGroup_ || elfConfiguration.ioConfiguration.mc6845IoGroup == -1)
        {
            if ((address&elfConfiguration.ioConfiguration.mc6845DataMask) == elfConfiguration.ioConfiguration.mc6845Data)
                return mc6845Pointer->readDataDirect6845(address);
            
            if (address >=elfConfiguration.ioConfiguration.mc6845StartRam && address <= elfConfiguration.ioConfiguration.mc6845EndRam)
                return mc6845Pointer->read6845(address & elfConfiguration.ioConfiguration.mc6845RamMask);
        }
    }

    if (elfConfiguration.useRtcM48T58)
    {
        if  (elfConfiguration.ioConfiguration.rtcIoGroup1 == ioGroup_ || elfConfiguration.ioConfiguration.rtcIoGroup1 == -1)
        {
            if (address > elfConfiguration.ioConfiguration.rtcControl && address <= elfConfiguration.ioConfiguration.rtcYear)
            {
                if ((mainMemory_[elfConfiguration.ioConfiguration.rtcControl]&0x40) != 0x40)
                {
                    rtcControl = 0;
                    systemNow = wxDateTime::Now();
                    timeDiff = systemNow.Subtract(systemTime_);

                    now = xmlComputerTime_;
                    now.Add(timeDiff);
                    
                    value = 0;
                    if (address == elfConfiguration.ioConfiguration.rtcSecond)
                    {
                        value = now.GetSecond();
                        rtcControl = mainMemory_[address] & 0x80;
                    }
                    if (address == elfConfiguration.ioConfiguration.rtcMinute)
                    {
                        value = now.GetMinute();
                    }
                    if (address == elfConfiguration.ioConfiguration.rtcHour)
                    {
                        value = now.GetHour();
                    }
                    if (address == elfConfiguration.ioConfiguration.rtcDay)
                    {
                        value = now.GetWeekDay();
                        rtcControl = mainMemory_[address] & 0x70;
                    }
                    if (address == elfConfiguration.ioConfiguration.rtcDate)
                    {
                        value = now.GetDay();
                        rtcControl = mainMemory_[address] & 0xc0;
                    }
                    if (address == elfConfiguration.ioConfiguration.rtcMonth)
                    {
                        value = now.GetMonth()+1;
                    }
                    if (address == elfConfiguration.ioConfiguration.rtcYear)
                    {
                        year = now.GetYear();
                        value = year % 100;
                    }

                    high = (int)(value/10)*16;
                    low = value - (int)(value/10)*10;
                    mainMemory_[address] = high + low + rtcControl;
                }
                return mainMemory_[address];
            }
        }
    }

    size_t number = (memoryType_[address / 256] >> 8);
        
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            if (elfConfiguration.tilType == TIL311)
                elfScreenPointer->showAddress(address);
            else
                elfScreenPointer->showAddressTil313Italic(address);
        break;

        case PANEL_SUPER:
            if (elfConfiguration.tilType == TIL311)
                superScreenPointer->showAddress(address_);
            else
                superScreenPointer->showAddressTil313Italic(address_);
        break;

        case PANEL_ELF2K:
            elf2KScreenPointer->showAddress(address_);
        break;
    }

    switch (memoryType_[address/256]&0xff)
    {
        case EMSMEMORY:
            switch (emsMemory_[number].memoryType_[((address - computerConfiguration.emsConfig_[number].start) |(computerConfiguration.emsConfig_[number].page << computerConfiguration.emsConfig_[number].maskBits))/256])
            {
                case UNDEFINED:
                    return 255;
                break;
                    
                case ROM:
                case RAM:
                    return emsMemory_[number].mainMem[(long) ((address - computerConfiguration.emsConfig_[number].start) |(computerConfiguration.emsConfig_[number].page << computerConfiguration.emsConfig_[number].maskBits))];
                break;
                    
                default:
                    return 255;
                break;
            }
        break;
            
        case SLOTMEM:
            switch (slotMemoryType_[selectedSlot_][((selectedBank_&computerConfiguration.slotConfig_.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-computerConfiguration.slotConfig_.start)/256])
            {
                case ROM:
                case RAM:
                    return slotMemory_[selectedSlot_][(selectedBank_&computerConfiguration.slotConfig_.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-computerConfiguration.slotConfig_.start];
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
        case MAPPEDROM:
            if (address <32 && monitor_) return minimon[address];
            
            if (elfConfiguration.giantBoardMapping)
            {
                if (address >= baseGiantBoard_)
                    address = (address & 0xff) | 0xf000;
            }
            else
            {
                if (computerConfiguration.memConfig_[number].useMemMask)
                    address = (address & computerConfiguration.memConfig_[number].memMask) | computerConfiguration.memConfig_[number].start;
            }
            
            return mainMemory_[address];
        break;

        case DIAGROM:
            if (diagRomActive_)
                return diagRomReplacement_[address];
            else
                return mainMemory_[address];
        break;
            
        case MAPPEDRAM:
        case MAINRAM:
        case RAM:
            if (address <32 && monitor_) return minimon[address];
            
            if (computerConfiguration.memConfig_[number].useMemMask)
            {
                address = (address & computerConfiguration.memConfig_[number].memMask) | computerConfiguration.memConfig_[number].start;
            }
            
            return mainMemory_[address];
        break;

        case SN76430NRAM:
            return mainMemory_[address];
        break;

        case MC6847RAM:
            return mc6847Pointer->read6847(address);
        break;

        case PRAM1870:
            if  (elfConfiguration.ioConfiguration.v1870ioGroup == ioGroup_ || elfConfiguration.ioConfiguration.v1870ioGroup == -1)
                return vis1870Pointer->readPram(address);
            else
                return mainMemory_[address];
        break;
            
        case CRAM1870:
            if  (elfConfiguration.ioConfiguration.v1870ioGroup == ioGroup_ || elfConfiguration.ioConfiguration.v1870ioGroup == -1)
                return vis1870Pointer->readCram(address);
            else
                return mainMemory_[address];
        break;
            
        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_))/256])
            {
                case UNDEFINED:
                    return 255;
                break;

                case ROM:
                case RAM:
                    if (address <32 && monitor_) return minimon[address];
                    return pagerMemory_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)];
                break;

                default:
                    return 255;
                break;
            }
        break;

        case NVRAM:
            return mainMemory_[address];
        break;
            
        default:
            return 255;
        break;
    }
}

void Xmlemu::writeMem(Word address, Byte value, bool writeRom)
{
    address_ = address;
    writeMemDebug(address, value, writeRom);
}

void Xmlemu::writeMemDebug(Word address, Byte value, bool writeRom)
{
    wxDateTime systemNow;
    wxDateTime now;
    wxTimeSpan timeDiff;
    Byte high, low;

    address = address | bootstrap_;
    size_t number = (memoryType_[address / 256] >> 8);
    
    if (loadedOs_ == ELFOS)
    {
        if (address == 0x400 && value >= 4 && value <= 128)
            loadedOs_ = ELFOS_4;
    }
        
    if (elfConfiguration.useRtcM48T58)
    {
        if  (elfConfiguration.ioConfiguration.rtcIoGroup1 == ioGroup_ || elfConfiguration.ioConfiguration.rtcIoGroup1 == -1)
        {
            if (address == elfConfiguration.ioConfiguration.rtcControl)
            {
                if (value == 0)
                {
                    systemNow = wxDateTime::Now();
                    timeDiff = systemNow.Subtract(systemTime_);

                    now = xmlComputerTime_;
                    now.Add(timeDiff);

                    now.SetDay(1);
                    now.SetYear(year_);
                    now.SetMonth(wxDateTime::Month(month_));
                    now.SetDay(day_);

                    xmlComputerTime_ = now.Subtract(timeDiff);
                }
            }
            if (address > elfConfiguration.ioConfiguration.rtcControl && address != elfConfiguration.ioConfiguration.rtcDay && address <= elfConfiguration.ioConfiguration.rtcYear)
            {
                if (nvramWriteProtected_)
                    return;
                if ((mainMemory_[elfConfiguration.ioConfiguration.rtcControl]&0x80) == 0x80)
                {
                    mainMemory_[address]=value;
                    
                    if (address == elfConfiguration.ioConfiguration.rtcSecond)
                        value &= 0x7f;
                    if (address == elfConfiguration.ioConfiguration.rtcMinute)
                        value &= 0x7f;
                    if (address == elfConfiguration.ioConfiguration.rtcHour)
                        value &= 0x3f;
                    if (address == elfConfiguration.ioConfiguration.rtcDate)
                        value &= 0x3f;
                    if (address == elfConfiguration.ioConfiguration.rtcMonth)
                        value &= 0x1f;

                    high = ((value&0xf0)/16)*10;
                    low = value&0xf;
                    value = high + low;

                    systemNow = wxDateTime::Now();
                    timeDiff = systemNow.Subtract(systemTime_);

                    now = xmlComputerTime_;
                    now.Add(timeDiff);

                    if (address == elfConfiguration.ioConfiguration.rtcSecond)
                        now.SetSecond(value);
                    if (address == elfConfiguration.ioConfiguration.rtcMinute)
                        now.SetMinute(value);
                    if (address == elfConfiguration.ioConfiguration.rtcHour)
                        now.SetHour(value);
                    if (address == elfConfiguration.ioConfiguration.rtcDate)
                        day_=value;
                    if (address == elfConfiguration.ioConfiguration.rtcMonth)
                        month_=value-1;
                    if (address == elfConfiguration.ioConfiguration.rtcYear)
                        year_=value+2000;

                    xmlComputerTime_ = now.Subtract(timeDiff);
                    return;
                }
            }
        }
    }
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            if (elfConfiguration.tilType == TIL311)
                elfScreenPointer->showAddress(address);
            else
                elfScreenPointer->showAddressTil313Italic(address);
        break;

        case PANEL_SUPER:
            if (elfConfiguration.tilType == TIL311)
                superScreenPointer->showAddress(address_);
            else
                superScreenPointer->showAddressTil313Italic(address_);
        break;

        case PANEL_ELF2K:
            elf2KScreenPointer->showAddress(address_);
        break;
    }

    if (computerConfiguration.emsConfigNumber_ != 0)
    {
        for (size_t num=0; num<computerConfiguration.emsConfigNumber_; num++)
        {
            if (elfConfiguration.ioConfiguration.emsOutput[num] == -1)
            {
                if (address>=computerConfiguration.emsConfig_[num].outputStart && address <=computerConfiguration.emsConfig_[num].outputEnd)
                {
                    computerConfiguration.emsConfig_[num].page = value & computerConfiguration.emsConfig_[num].outputMask;
                }
            }
        }
    }
    if (elfConfiguration.ioConfiguration.mc6847OutputMode == 1)
    {
        if (address>=elfConfiguration.ioConfiguration.mc6847OutputStart && address <=elfConfiguration.ioConfiguration.mc6847OutputEnd)
            mc6847Pointer->outMc6847(value);
    }

    if (elfConfiguration.use6845)
    {
        if  (elfConfiguration.ioConfiguration.mc6845IoGroup == ioGroup_ || elfConfiguration.ioConfiguration.mc6845IoGroup == -1)
        {
            if ((address&elfConfiguration.ioConfiguration.mc6845DataMask) == elfConfiguration.ioConfiguration.mc6845Data)
            {
                mc6845Pointer->writeData6845(value);
                return;
            }

            if ((address&elfConfiguration.ioConfiguration.mc6845AddressMask) == elfConfiguration.ioConfiguration.mc6845Address)
            {
                mc6845Pointer->writeRegister6845(value);
                return;
            }

            if (address >=elfConfiguration.ioConfiguration.mc6845StartRam && address <= elfConfiguration.ioConfiguration.mc6845EndRam)
            {
                mc6845Pointer->write6845(address & elfConfiguration.ioConfiguration.mc6845RamMask, value);
                return;
            }
        }
    }

    switch (memoryType_[address/256]&0xff)
    {
        case EMSMEMORY:
            switch (emsMemory_[number].memoryType_[((address - computerConfiguration.emsConfig_[number].start) |(computerConfiguration.emsConfig_[number].page << computerConfiguration.emsConfig_[number].maskBits))/256])
            {
                case UNDEFINED:
                case ROM:
                    if (writeRom)
                        emsMemory_[number].mainMem[(long) ((address - computerConfiguration.emsConfig_[number].start) |(computerConfiguration.emsConfig_[number].page << computerConfiguration.emsConfig_[number].maskBits))] = value;
                break;
                    
                case RAM:
                    if (!getMpButtonState())
                    {
                        emsMemory_[number].mainMem[(long) ((address - computerConfiguration.emsConfig_[number].start) |(computerConfiguration.emsConfig_[number].page << computerConfiguration.emsConfig_[number].maskBits))] = value;
                        if (address >= memoryStart_ && address<(memoryStart_ + 256))
                            p_Main->updateDebugMemory(address);
                        p_Main->updateAssTabCheck(address);
                    }
                break;
            }
        break;
       
        case SLOTMEM:
            switch (slotMemoryType_[selectedSlot_][((selectedBank_&computerConfiguration.slotConfig_.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-computerConfiguration.slotConfig_.start)/256])
            {
                case ROM:
                    if (writeRom)
                        slotMemory_[selectedSlot_][(selectedBank_&computerConfiguration.slotConfig_.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-computerConfiguration.slotConfig_.start] = value;
                break;

                case RAM:
                    slotMemory_[selectedSlot_][(selectedBank_&computerConfiguration.slotConfig_.slotInfo[selectedSlot_].bankOutputMaskInSlot)*slotSize_[selectedSlot_] + address-computerConfiguration.slotConfig_.start] = value;
                break;
            }
        break;

        case MC6847RAM:
            mc6847Pointer->write(address, value);
            mainMemory_[address] = value;
            if (address >= memoryStart_ && address<(memoryStart_ + 256))
                p_Main->updateDebugMemory(address);
        break;

        case SN76430NRAM:
            mainMemory_[address] = value;
            sn76430nPointer->writeRam(address, value);
        break;

        case UNDEFINED:
            if (writeRom)
                mainMemory_[address]=value;
        break;

        case MAPPEDROM:
        case ROM:
            if (address < 32 && monitor_) return;

            if (elfConfiguration.giantBoardMapping)
            {
                if (address >= baseGiantBoard_)
                    address = (address & 0xff) | 0xf000;
            }
            else
            {
                if (computerConfiguration.memConfig_[number].useMemMask)
                    address = (address & computerConfiguration.memConfig_[number].memMask) | computerConfiguration.memConfig_[number].start;
            }

            if (writeRom)
                mainMemory_[address]=value;
        break;

        case DIAGROM:
            if (writeRom)
            {
                if (diagRomActive_)
                    diagRomReplacement_[address]=value;
                else
                    mainMemory_[address]=value;
            }
        break;
            
        case MAPPEDRAM:
        case MAINRAM:
        case RAM:
            if (!getMpButtonState())
            {
                if (address < 32 && monitor_) return;
                
                if (computerConfiguration.memConfig_[number].useMemMask)
                {
                    
                    address = (address & computerConfiguration.memConfig_[number].memMask) | computerConfiguration.memConfig_[number].start;
                }
                
                if (mainMemory_[address]==value)
                    return;
                mainMemory_[address]=value;
                if (computerConfiguration.memConfig_[number].useMemMask)
                {
                    if (address >= (memoryStart_ & computerConfiguration.memConfig_[number].memMask) && address<((memoryStart_ & computerConfiguration.memConfig_[number].memMask) | 256))
                        p_Main->updateDebugMemory(address);
                }
                else
                {
                    if (address >= memoryStart_ && address<(memoryStart_ + 256))
                        p_Main->updateDebugMemory(address);
                }
                p_Main->updateAssTabCheck(address);
            }
            switch (address)
            {
                case 0x428b:
                    switch(value)
                    {
 //                       case 0x53:
 //                           fAndMBasicRunning_ = true;
 //                           p_Main->eventSetFandMBasicGui();
 //                       break;

                        case 0xc1:
                            thermalPrinting_ = true;
                        break;

                        case 0x14:
//                            stop6845();
//                            if (fAndMBasicRunning_ && comxKeyFileOpened_)
//                                keyboardEf3_ = 0;
//                            fAndMBasicRunning_ = false;
//                            p_Main->eventSetFandMBasicGui();
                            if (computerConfiguration.useParallelPrinter_)
                            {
                                p_Main->eventPrintParallelFinished();
                            }
                            if (computerConfiguration.useSerialPrinter_)
                            {
                                p_Main->eventPrintSerialFinished();
                            }
                            if (computerConfiguration.useThermalPrinter_)
                            {
                                p_Main->eventPrintThermalFinished();
                            }
                        break;
                    }
                break;

/*                case 0x41a3:
                    if (value == 0 && comxKeyFileOpened_ && !fAndMBasicRunning_ && comxRunState_ != LISTSTATE)
                    {
                        keyboardEf3_ = 0;
                    }
                break;*/
            }
        break;

        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_))/256])
            {
                case UNDEFINED:
                case ROM:
                    if (address < 32 && monitor_) return;
                    if (writeRom)
                        pagerMemory_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)] = value;
                break;

                case RAM:
                    if (address < 32 && monitor_) return;
                    if (!getMpButtonState())
                        pagerMemory_[(getPager(address>>computerConfiguration.pagerMaskBits_) << computerConfiguration.pagerMaskBits_) |(address &computerConfiguration.pagerMask_)] = value;
                    if (address >= memoryStart_ && address<(memoryStart_ + 256))
                        p_Main->updateDebugMemory(address);
                    p_Main->updateAssTabCheck(address);
                break;
            }
        break;

        case PRAM1870:
            if (writeRom && elfConfiguration.ioConfiguration.v1870pageMemRom)
                mainMemory_[address]=value;
            else
            {
                if  (elfConfiguration.ioConfiguration.v1870ioGroup == ioGroup_ || elfConfiguration.ioConfiguration.v1870ioGroup == -1)
                {
                    address = vis1870Pointer->writePram(address, value);
                    
                    if (address>= memoryStart_ && address<(memoryStart_+256))
                        p_Main->updateDebugMemory(address);
                }
                else
                {
                    if (ramGroupAtV1870_)
                    {
                        if (mainMemory_[address]==value)
                            return;
                        mainMemory_[address]=value;
                        if (address>= memoryStart_ && address<(memoryStart_+256))
                            p_Main->updateDebugMemory(address);
                        p_Main->updateAssTabCheck(address);
                    }
                }
            }
        break;
            
        case CRAM1870:
            if  (elfConfiguration.ioConfiguration.v1870ioGroup == ioGroup_ || elfConfiguration.ioConfiguration.v1870ioGroup == -1)
            {
                address = vis1870Pointer->writeCram(address, value);

                if (address>= memoryStart_ && address<(memoryStart_+256))
                    p_Main->updateDebugMemory(address);
            }
            else
            {
                if (ramGroupAtV1870_)
                {
                    if (mainMemory_[address]==value)
                        return;
                    mainMemory_[address]=value;
                    if (address>= memoryStart_ && address<(memoryStart_+256))
                        p_Main->updateDebugMemory(address);
                    p_Main->updateAssTabCheck(address);
                }
            }
        break;

        case NVRAM:
            if (nvramWriteProtected_)
                return;

            if (mainMemory_[address]==value)
                return;
            mainMemory_[address]=value;
            if (address>= memoryStart_ && address<(memoryStart_+256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;
    }
}

void Xmlemu::cpuInstruction()
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_SUPER:
            if (cpuMode_ != lastMode_)
            {
                superScreenPointer->setLed(0,0);
                superScreenPointer->setLed(1,0);
                superScreenPointer->setLed(2,0);
                superScreenPointer->setLed(3,0);
                switch(cpuMode_)
                {
                    case LOAD: superScreenPointer->setLed(0,1); break;
                    case RESET: superScreenPointer->setLed(1,1); break;
                    case RUN: superScreenPointer->setLed(2,1); break;
                    case PAUSE: superScreenPointer->setLed(3,1); break;
                }
                lastMode_ = cpuMode_;
            }
            
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
                }
            }
        break;

        case PANEL_MICROTUTOR:
            cpuCycleStep();
        break;

        case PANEL_ELF2K:
            if (cpuMode_ != lastMode_)
            {
                elf2KScreenPointer->setRunLed(0);
                elf2KScreenPointer->setLoadLed(0);
                switch(cpuMode_)
                {
                    case LOAD: elf2KScreenPointer->setLoadLed(1); break;
                    case RESET: break;
                    case RUN: elf2KScreenPointer->setRunLed(1); break;
                    case PAUSE: break;
                }
                lastMode_ = cpuMode_;
            }
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
        break;

        case PANEL_COSMICOS:
            if (cpuMode_ != lastMode_)
            {
                cosmicosScreenPointer->setRunLed(0);
                cosmicosScreenPointer->setLoadLed(0);
                cosmicosScreenPointer->setResetLed(0);
                cosmicosScreenPointer->setPauseLed(0);
                switch(cpuMode_)
                {
                    case LOAD: cosmicosScreenPointer->setLoadLed(1); break;
                    case RESET: cosmicosScreenPointer->setResetLed(1); break;
                    case RUN: cosmicosScreenPointer->setRunLed(1); break;
                    case PAUSE: cosmicosScreenPointer->setPauseLed(1); break;
                }
                lastMode_ = cpuMode_;
            }

            if (cpuMode_ == RUN)
            {
                cpuCycleStep();
            }
            else
            {
                cycleLed();
                cpuCycles_ = 0;
                instructionCounter_= 0;
                p_Main->startTime();
                if (cpuMode_ == LOAD)
                {
                    showData(readMem(address_));
                    ledCycleValue_ = 1;
                }
            }
        break;

        default:
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
                }
            }
        break;
    }
}

void Xmlemu::resetPressed()
{
    if (elfConfiguration.bootStrap)
        bootstrap_ = elfConfiguration.strapAddress;

    p_Main->stopTerminal();
    terminalStop();

    resetCpu();
    resetComputer();
    if (elfConfiguration.use8275)
        i8275Pointer->cRegWrite(0x40);
    if (elfConfiguration.usev1870)
    {
        vis1870Pointer->init1870();
        if (elfConfiguration.ioConfiguration.statusBarType != STATUSBAR_NONE)
            p_Main->v1870BarSizeEvent();
        
        if (computerConfiguration.useDiagnosticBoard_)
        {
            if (computerConfiguration.diagRomOn_ == 1)
                diagRomActive_ = true;
            else
                diagRomActive_ = false;
            p_Main->eventUpdateDiagLedStatus(1, diagRomActive_);
            diagDmaLedOn_ = false;
            p_Main->eventUpdateDiagLedStatus(2, diagDmaLedOn_);
            p_Main->eventUpdateDiagLedStatus(5, false);
        }

    }
    if (elfConfiguration.useLatchKeyboard)
        latchKeyboardPointer->resetKeybLatch();
    if (elfConfiguration.useMatrixKeyboard)
        matrixKeyboardPointer->resetKeyboard();
    if (elfConfiguration.autoBoot)
    {
        scratchpadRegister_[0]=p_Main->getBootAddress("Xml", XML);
        autoBoot();
    }
    else
    {
        if (baseGiantBoard_ != 0x10000)
        {
            scratchpadRegister_[0] = baseGiantBoard_;
            autoBoot();
        }
    }
    resetPressed_ = false;
    computerConfiguration.diagRomOn_ = (p_Main->isDiagOn(XML) == 1);

    elfRunState_ = RESETSTATE;
    p_Main->setSwName("");
    p_Main->eventUpdateTitle();
    
    if (elfConfiguration.useKeyboard)
        startElfKeyFile("Xml");
    if (elfConfiguration.useKeyb1871)
        start1871KeyFile();
    if (elfConfiguration.useLatchKeyboard)
        latchKeyboardPointer->startKeyFile();
    if (elfConfiguration.useMatrixKeyboard)
        matrixKeyboardPointer->startKeyFile();
    if (elfConfiguration.usePs2gpio)
        startPs2gpioKeyFile();

    ElfConfiguration currentElfConfig = p_Main->getElfConfiguration(computerType_);
    if (currentElfConfig.clearRam && currentElfConfig.useNvRam)
    {
        for (size_t i=0; i<nvramDetails.size(); i++)
        {
            for (long address = nvramDetails[i].start; address <= nvramDetails[i].end; address++)
                mainMemory_[address] = 0;
        }
        p_Main->eventSetCheckBox("XmlClearRam", false);
    }
    if (currentElfConfig.clearRtc && currentElfConfig.useRtcDS12887)
    {
        for (int i = 0; i<128; i++)
        {
            rtcRam_[i] = 0;
            p_Main->updateDebugMemory(i);
        }
        rtcRam_[0xa] = 0x20;
        rtcRam_[0xb] = 0x6;
        rtcRam_[0xc] = 0;
        rtcRam_[0xd] = 0x80;
        currentElfConfig.clearRtc = false;
        p_Main->setElfConfiguration(currentElfConfig);
        p_Main->eventSetCheckBox("XmlClearRtc", false);
    }
}

void Xmlemu::configureMemory()
{
    size_t memConfNumber = 0;
    Word mask;
    
    while (memConfNumber < computerConfiguration.memConfigNumber_)
    {
        switch (computerConfiguration.memConfig_[memConfNumber].type & 0xff)
        {
            case ROM:
                defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end, computerConfiguration.memConfig_[memConfNumber].type);

                if (computerConfiguration.memConfig_[memConfNumber].useMemMask)
                {
                    defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start + computerConfiguration.memConfig_[memConfNumber].memMask + 1, computerConfiguration.memConfig_[memConfNumber].end, MAPPEDROM + (computerConfiguration.memConfig_[memConfNumber].type & 0xff00));
                }

                loadRomRam(memConfNumber);
            break;
                
            case DIAGROM:
                defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end, computerConfiguration.memConfig_[memConfNumber].type);

                diagRomActive_ = true;
                loadRomRam(memConfNumber);
            break;
                
            case RAM:
                defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end, computerConfiguration.memConfig_[memConfNumber].type);
                
                if (computerConfiguration.memConfig_[memConfNumber].useMemMask)
                {
                    defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start + computerConfiguration.memConfig_[memConfNumber].memMask + 1, computerConfiguration.memConfig_[memConfNumber].end, MAPPEDRAM + (computerConfiguration.memConfig_[memConfNumber].type & 0xff00));
                }

                initRam(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end);
                loadRomRam(memConfNumber);
            break;

            case NVRAM:
                defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end, computerConfiguration.memConfig_[memConfNumber].type);
                
                initRam(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end);
                loadNvRam(memConfNumber);
            break;

            case MAINRAM:
                defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end, computerConfiguration.memConfig_[memConfNumber].type);
                
                if (computerConfiguration.memConfig_[memConfNumber].useMemMask)
                {
                    defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start + computerConfiguration.memConfig_[memConfNumber].memMask + 1, computerConfiguration.memConfig_[memConfNumber].end, MAPPEDRAM + (computerConfiguration.memConfig_[memConfNumber].type & 0xff00));
                }

                initRam(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end);

                p_Main->assDefault("mycode", computerConfiguration.memConfig_[memConfNumber].start&0xff00, (computerConfiguration.memConfig_[memConfNumber].start&0xff00)+0xfff);
                loadRomRam(memConfNumber);
            break;
                
            case PAGER:
                configurePortExt(elfConfiguration.ioConfiguration);
                
                allocPagerMemory(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end);
                definePortExtForPager(computerConfiguration.startPort_, computerConfiguration.endPort_);

                defineMemoryType(computerConfiguration.memConfig_[memConfNumber].start, computerConfiguration.memConfig_[memConfNumber].end, PAGER);
            break;
        }
        memConfNumber++;
    }

    size_t emsNumber = 0;
    
    while (emsNumber < computerConfiguration.emsConfigNumber_)
    {
        computerConfiguration.emsConfig_[emsNumber].maskBits = 16;
        computerConfiguration.emsConfig_[emsNumber].mask = 0xFFFF;
        mask = computerConfiguration.emsConfig_[emsNumber].end - computerConfiguration.emsConfig_[emsNumber].start;
        while ((mask & 0x8000) == 0)
        {
            computerConfiguration.emsConfig_[emsNumber].maskBits--;
            computerConfiguration.emsConfig_[emsNumber].mask = computerConfiguration.emsConfig_[emsNumber].mask >> 1;
            mask = mask << 1;
        }
        
        emsMemory_.resize(emsNumber+1);
        
        if (computerConfiguration.emsConfig_[emsNumber].emsType == ROM)
            readRomMapperBinFile(emsNumber, computerConfiguration.emsConfig_[emsNumber].dirname+computerConfiguration.emsConfig_[emsNumber].filename);
        else
            allocEmsMemorySegment(emsNumber);

        defineMemoryType(computerConfiguration.emsConfig_[emsNumber].start, computerConfiguration.emsConfig_[emsNumber].end, computerConfiguration.emsConfig_[emsNumber].type);
        emsNumber++;
    }
    
    if (elfConfiguration.giantBoardMapping)
        defineMemoryType(0xF000, 0xFFFF, ROM);
    
    if (computerConfiguration.slotConfig_.maxSlotNumber_ > 0)
    {
        defineMemoryType(computerConfiguration.slotConfig_.start, computerConfiguration.slotConfig_.end, SLOTMEM);
        
        for (int slot=0; slot<computerConfiguration.slotConfig_.maxSlotNumber_; slot++)
        {
            allocSlotMemory();
            selectedBank_ = 0;
            selectedSlot_ = slot;
            
            if (computerConfiguration.slotConfig_.slotInfo[slot].maxBankNumber_ > 1)
            {
                for (int bank=0; bank<computerConfiguration.slotConfig_.slotInfo[slot].maxBankNumber_; bank++)
                {
                    selectedBank_ = bank;

                    defineXmlBankMemoryType(slot, bank, 0, slotSize_[slot]-1, computerConfiguration.slotConfig_.slotInfo[slot].bankInfo[bank].type);

                    if (computerConfiguration.slotConfig_.slotInfo[slot].bankInfo[bank].filename != "")
                        readProgram(computerConfiguration.slotConfig_.slotInfo[slot].bankInfo[bank].dirname, computerConfiguration.slotConfig_.slotInfo[slot].bankInfo[bank].filename, SLOTMEM,  computerConfiguration.slotConfig_.start, NONAME);
                }
            }
            else
            {
                defineXmlSlotMemoryType(slot, 0, slotSize_[slot]-1, computerConfiguration.slotConfig_.slotInfo[slot].type);
                
                if (computerConfiguration.slotConfig_.slotInfo[slot].filename != "")
                    readProgram(computerConfiguration.slotConfig_.slotInfo[slot].dirname, computerConfiguration.slotConfig_.slotInfo[slot].filename, SLOTMEM,  computerConfiguration.slotConfig_.start, NONAME);
            }
        }
        if (computerConfiguration.slotConfig_.useIoGroup)
            ioGroup_ = selectedSlot_;
        setOutType(computerConfiguration.slotConfig_.output, COMXOUT);
    }
    selectedSlot_ = 0;
    selectedBank_ = 0;
}

void Xmlemu::configureExtensions()
{
    wxString fileName, fileName2, message;
    wxString ioGroup;
    wxString printBuffer;

    if (elfConfiguration.vtType != VTNONE)
    {
        double zoom = p_Main->getZoomVt();
        if (elfConfiguration.vtType == VT52)
            vtPointer = new Vt100(p_Main->getRunningComputerText() + " - VT 52", p_Main->getVtPos(XML), wxSize(800*zoom, 500*zoom), zoom, XML, elfClockSpeed_, elfConfiguration, UART1);
        else
            vtPointer = new Vt100(p_Main->getRunningComputerText() + " - VT 100", p_Main->getVtPos(XML), wxSize(800*zoom, 500*zoom), zoom, XML, elfClockSpeed_, elfConfiguration, UART1);
        p_Vt100[UART1] = vtPointer;
        vtPointer->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.ioConfiguration, computerConfiguration.addressLocations, computerConfiguration.saveCommand_);
        if (elfConfiguration.useUart16450)
            vtPointer->configureUart16450(elfConfiguration.ioConfiguration);
        vtPointer->Show(true);
        vtPointer->drawScreen();
    }

    if (elfConfiguration.vtExternal)
    {
        p_Serial = new Serial(XML, elfClockSpeed_, elfConfiguration);
        p_Serial->configure(elfConfiguration.baudR, elfConfiguration.baudT, elfConfiguration.ioConfiguration);
        if (elfConfiguration.useUart16450)
            p_Serial->configureUart16450(elfConfiguration.ioConfiguration);
    }

    if (elfConfiguration.usePixie)
    {
        double zoom = p_Main->getZoom(elfConfiguration.ioConfiguration.pixieVideoNumber);
        double scale = p_Main->getScale();
        pixiePointer = new Pixie(p_Main->getRunningComputerText() + " - Pixie", p_Main->getPixiePos(XML), wxSize(64*zoom*scale, 128*zoom), zoom, scale, XML, elfConfiguration.ioConfiguration.pixieVideoNumber);
        p_Video[elfConfiguration.ioConfiguration.pixieVideoNumber] = pixiePointer;
        if (elfConfiguration.ioConfiguration.pixieDoubleScreenIo)
            pixiePointer->configurePixieSuper(elfConfiguration.ioConfiguration);
        else
            pixiePointer->configurePixie(elfConfiguration.ioConfiguration);
        pixiePointer->initPixie();
        pixiePointer->setZoom(zoom);
        pixiePointer->Show(true);
    }

    if (elfConfiguration.use1864)
    {
        double zoom = p_Main->getZoom(elfConfiguration.ioConfiguration.cdp1864VideoNumber);
        double scale = p_Main->getScale();
        cdp1864Pointer = new Pixie(p_Main->getRunningComputerText() + " - CDP1864", p_Main->getCdp1864Pos(XML), wxSize(64*zoom*scale, 128*zoom), zoom, scale, XML, elfConfiguration.ioConfiguration.cdp1864VideoNumber);
        p_Video[elfConfiguration.ioConfiguration.cdp1864VideoNumber] = cdp1864Pointer;
        cdp1864Pointer->configureCdp1864(elfConfiguration.ioConfiguration);
        cdp1864Pointer->initPixie();
        cdp1864Pointer->setZoom(zoom);
        cdp1864Pointer->Show(true);
    }

    if (elfConfiguration.use6845)
    {
        double zoom = p_Main->getZoom(elfConfiguration.ioConfiguration.mc6845VideoNumber);
        mc6845Pointer = new MC6845(p_Main->getRunningComputerText() + " - MC6845", p_Main->get6845Pos(XML), wxSize(elfConfiguration.ioConfiguration.mc6845ScreenSize.x*elfConfiguration.ioConfiguration.mc6845CharSize.x*zoom, elfConfiguration.ioConfiguration.mc6845ScreenSize.y*(elfConfiguration.ioConfiguration.mc6845CharSize.y+1)*zoom), zoom, XML, elfClockSpeed_, elfConfiguration.ioConfiguration.mc6845CharSize, elfConfiguration.ioConfiguration, elfConfiguration.ioConfiguration.mc6845VideoNumber);
        p_Video[elfConfiguration.ioConfiguration.mc6845VideoNumber] = mc6845Pointer;
        mc6845Pointer->configure6845Xml(elfConfiguration.ioConfiguration);
        mc6845Pointer->init6845();
        mc6845Pointer->Show(true);
    }

    if (elfConfiguration.use8275)
    {
        double zoom = p_Main->getZoom(elfConfiguration.ioConfiguration.i8275VideoNumber);
        i8275Pointer = new i8275(p_Main->getRunningComputerText() + " - Intel 8275", p_Main->get8275Pos(XML), wxSize(80*8*zoom, 24*10*2*zoom), zoom, XML, elfClockSpeed_, elfConfiguration.ioConfiguration.i8275VideoNumber);
        p_Video[elfConfiguration.ioConfiguration.i8275VideoNumber] = i8275Pointer;
        i8275Pointer->configure8275(elfConfiguration.ioConfiguration);
        i8275Pointer->init8275();
        i8275Pointer->Show(true);
    }

    if (elfConfiguration.use6847)
    {
        double zoom = p_Main->getZoom(elfConfiguration.ioConfiguration.mc6847VideoNumber);
        mc6847Pointer = new mc6847(p_Main->getRunningComputerText() + " - MC6847", p_Main->get6847Pos(XML), wxSize(elfConfiguration.charLine*8*zoom, elfConfiguration.screenHeight6847*zoom), zoom, XML, elfClockSpeed_, elfConfiguration.ioConfiguration, elfConfiguration.ioConfiguration.mc6847VideoNumber);
        p_Video[elfConfiguration.ioConfiguration.mc6847VideoNumber] = mc6847Pointer;
        mc6847Pointer->configure(elfConfiguration.ioConfiguration);
        mc6847Pointer->init6847();
        mc6847Pointer->Show(true);
    }

    if (elfConfiguration.useTMS9918)
    {
        double zoom = p_Main->getZoom(elfConfiguration.ioConfiguration.tmsVideoNumber);
        tmsPointer = new Tms9918(p_Main->getRunningComputerText() + " - TMS 9918", p_Main->getTmsPos(XML), wxSize(320*zoom,240*zoom), zoom, XML, elfClockSpeed_, elfConfiguration.ioConfiguration.tmsVideoNumber);
        p_Video[elfConfiguration.ioConfiguration.tmsVideoNumber] = tmsPointer;
        tmsPointer->configure(elfConfiguration.ioConfiguration);
        tmsPointer->Show(true);
    }
    
    if (elfConfiguration.useSN76430N)
    {
        double zoom = p_Main->getZoom(elfConfiguration.ioConfiguration.SN76430NVideoNumber);
        sn76430nPointer = new SN76430N(p_Main->getRunningComputerText() + " - SN76430N", p_Main->getSN76430NPos(XML), wxSize(128*zoom,96*zoom), zoom, XML, elfClockSpeed_, 3.58, elfConfiguration.ioConfiguration, elfConfiguration.ioConfiguration.SN76430NVideoNumber);
        p_Video[elfConfiguration.ioConfiguration.SN76430NVideoNumber] = sn76430nPointer;
        sn76430nPointer->configure();
        sn76430nPointer->init();
        sn76430nPointer->Show(true);
        
        defineMemoryType(0x800, 0xFFF, SN76430NRAM);
    }


    if (elfConfiguration.usev1870)
    {
        double zoom = p_Main->getZoom(elfConfiguration.ioConfiguration.v1870VideoNumber);
        vis1870Pointer = new VIS1870(p_Main->getRunningComputerText() + " - VIS 1870", p_Main->getV1870Pos(XML), wxSize(240*zoom,216*zoom), zoom, XML, elfClockSpeed_, elfConfiguration.ioConfiguration.v1870clock, elfConfiguration.ioConfiguration, elfConfiguration.ioConfiguration.v1870VideoNumber);
        p_Video[elfConfiguration.ioConfiguration.v1870VideoNumber] = vis1870Pointer;
        if (vis1870Pointer->configure1870(readMemDebug(0xE000) != 0xFF, readMemDebug(0xC001)))
        {
            if (elfConfiguration.ioConfiguration.v1870charRomFormatTmc)
                vis1870Pointer->readChargenFileTmc(p_Main->getCharRomDir(XML), p_Main->getCharRomFile(XML));
            else
                vis1870Pointer->readChargenFile(p_Main->getCharRomDir(XML), p_Main->getCharRomFile(XML));
        }
        
        vis1870Pointer->init1870();
     
        efType_[0][0][elfConfiguration.ioConfiguration.v1870ef] = V1870EF;

        cycleType_[VIDEOCYCLE_V1870] = V1870CYCLE;
        if (elfConfiguration.ioConfiguration.v1870cursorBlink)
            cycleType_[BLINKCYCLE_V1870] = V1870BLINK;

        if (elfConfiguration.ioConfiguration.v1870outWrite == -1 && elfConfiguration.ioConfiguration.v1870outSelect == -1)
        {
            setOutType(elfConfiguration.ioConfiguration.v1870ioGroup+1, 3, V1870OUT3);
            setOutType(elfConfiguration.ioConfiguration.v1870ioGroup+1, 4, V1870OUT4);
            setOutType(elfConfiguration.ioConfiguration.v1870ioGroup+1, 5, V1870OUT5);
            setOutType(elfConfiguration.ioConfiguration.v1870ioGroup+1, 6, V1870OUT6);
            setOutType(elfConfiguration.ioConfiguration.v1870ioGroup+1, 7, V1870OUT7);
        }
        else
        {
            setOutType(elfConfiguration.ioConfiguration.v1870ioGroup+1, elfConfiguration.ioConfiguration.v1870outWrite, TELMACOUTDATA);
            setOutType(elfConfiguration.ioConfiguration.v1870ioGroup+1, elfConfiguration.ioConfiguration.v1870outSelect, TELMACOUTREGISTER);
        }

        ramGroupAtV1870_ = false;
        if ((memoryType_[0xf400/256]&0xff) == RAM)
            ramGroupAtV1870_ = true;
            
        defineMemoryType(0xf400, 0xf7ff, CRAM1870);
        defineMemoryType(0xf800, 0xffff, PRAM1870);
        vis1870Pointer->Show(true);
    }

    if (elfConfiguration.fdc1793Enabled)
    {
        configure1793(elfConfiguration.ioConfiguration.fdcSides, elfConfiguration.ioConfiguration.fdcTracks, elfConfiguration.ioConfiguration.fdcSectors, elfConfiguration.ioConfiguration.fdcSectorLength, elfConfiguration.ioConfiguration.fdcMaxFmtCount, XML, elfConfiguration.ioConfiguration, false);
        resetFdc();

        for (int disk=0; disk<4; disk++)
        {
            fileName = p_Main->getUpdFloppyFile(FDCTYPE_17XX, disk);
            if (fileName.Len() == 0)
                setFdcDiskname(disk+1, "");
            else
                setFdcDiskname(disk+1, p_Main->getUpdFloppyDir(FDCTYPE_17XX, disk)+p_Main->getUpdFloppyFile(FDCTYPE_17XX, disk));
        }
    }

    if (elfConfiguration.fdc1770Enabled)
    {
        configure1770(elfConfiguration.ioConfiguration.fdcSides, elfConfiguration.ioConfiguration.fdcTracks, elfConfiguration.ioConfiguration.fdcSectors, elfConfiguration.ioConfiguration.fdcSectorLength, elfConfiguration.ioConfiguration.fdcMaxFmtCount, XML, elfConfiguration.ioConfiguration);
        resetFdc();

        for (int disk=0; disk<4; disk++)
        {
            fileName = p_Main->getUpdFloppyFile(FDCTYPE_17XX, disk);
            if (fileName.Len() == 0)
                setFdcDiskname(disk+1, "");
            else
                setFdcDiskname(disk+1, p_Main->getUpdFloppyDir(FDCTYPE_17XX, disk)+p_Main->getUpdFloppyFile(FDCTYPE_17XX, disk));
        }
    }

    if (elfConfiguration.ideEnabled)
    {
        configureIde(p_Main->getIdeDir(XML) + p_Main->getIdeFile(XML), p_Main->getIdeDir(XML) + "disk2.ide", elfConfiguration.ioConfiguration);
    }

    if (elfConfiguration.useRtcDS12887)
    {
        configureRtc(elfConfiguration.ioConfiguration);
    }

    if (computerConfiguration.useBasicPrinter_)
    {
        p_Printer = new Printer();
        p_Printer->configureBasicPrinter(elfConfiguration.ioConfiguration);
        p_Printer->init(p_Printer, PRINTER_BASIC);
    }

    if (computerConfiguration.useParallelPrinter_)
    {
        p_PrinterParallel = new Printer();
        p_PrinterParallel->configureParallelPrinter(elfConfiguration.ioConfiguration);
        p_PrinterParallel->init(p_PrinterParallel, PRINTER_PARALLEL);
    }
    
    if (computerConfiguration.useSerialPrinter_)
    {
        p_PrinterSerial = new Printer();
        p_PrinterSerial->configureSerialPrinter(elfConfiguration.ioConfiguration);
        p_PrinterSerial->init(p_PrinterSerial, PRINTER_SERIAL);
    }
    
    if (computerConfiguration.useQSerialPrinter_)
    {
        p_Printer = new Printer();
        p_Printer->init(p_Printer, PRINTER_BASIC);
        p_Computer->setEfType(elfConfiguration.ioConfiguration.printerIoGroup +1, elfConfiguration.ioConfiguration.qSerialPrinterEf, BASICQ_PRINT_EF);
    }
    
    if (computerConfiguration.useThermalPrinter_)
    {
        p_PrinterThermal = new Printer();
        p_PrinterThermal->configureThermalPrinter(elfConfiguration.ioConfiguration);
        p_PrinterThermal->init(p_PrinterThermal, COMXTHPRINTER);
    }

    if (elfConfiguration.useLedModule)
    {
        ledModulePointer = new LedModule("Led Module", p_Main->getLedModulePos(), wxSize(172, 116), XML);
        ledModulePointer->configure(elfConfiguration.ioConfiguration);
        ledModulePointer->Show(p_Main->getUseElfControlWindows(XML));
    }

    setQsound (elfConfiguration.qSound_);

    if (elfConfiguration.useDip)
    {
        setInType(elfConfiguration.ioConfiguration.dipIn, CIDELSAIN2);

        p_Main->message("Configuring DIP switch");
        
        message.Printf("    Input %d, hax value: %02X\n", elfConfiguration.ioConfiguration.dipIn, elfConfiguration.ioConfiguration.dipValue);
        p_Main->message(message);
    }

    if (elfConfiguration.useDma)
        setCycleType(DRAMDMACYCLE, DRAMDMA);

    if (elfConfiguration.useInt)
        setCycleType(INTCYCLE, INTCLOCK);

    if (elfConfiguration.useNvRamMp)
        p_Computer->setInType(elfConfiguration.ioConfiguration.nvRamIoGroup+1, elfConfiguration.ioConfiguration.nvRamProtectIn, NVRAM_MP_IN);

    if (elfConfiguration.panelType_ == PANEL_COSMAC && elfConfiguration.useHexKeyboard)
    {
        keypadPointer = new Keypad("Keypad", p_Main->getKeypadPos(XML), wxSize(172, 229), ELF);
        keypadPointer->Show(p_Main->getUseElfControlWindows(XML));
        p_Main->message("Configuring Keypad Module\n");
    }

    if (elfConfiguration.useBitKeypad)
    {
        if (elfConfiguration.ioConfiguration.bitKeypad[0].defined)
        {
            bitkeypadPointer[0] = new BitKeypad();
            bitkeypadPointer[0]->configure(elfConfiguration.ioConfiguration, 0);
            p_Computer->setInType(elfConfiguration.ioConfiguration.bitKeypad[0].ioGroup+1, elfConfiguration.ioConfiguration.bitKeypad[0].inp , BIT_KEYPAD_IN_0);
        }
        if (elfConfiguration.ioConfiguration.bitKeypad[1].defined)
        {
            bitkeypadPointer[1] = new BitKeypad();
            bitkeypadPointer[1]->configure(elfConfiguration.ioConfiguration, 1);
            p_Computer->setInType(elfConfiguration.ioConfiguration.bitKeypad[1].ioGroup+1, elfConfiguration.ioConfiguration.bitKeypad[1].inp , BIT_KEYPAD_IN_1);
        }
    }
    
    if (elfConfiguration.useCvKeypad)
    {
        if (elfConfiguration.ioConfiguration.cvKeypad.defined)
        {
            cvkeypadPointer = new CvKeypad();
            cvkeypadPointer->configure(elfConfiguration.ioConfiguration);
            p_Computer->setInType(elfConfiguration.ioConfiguration.cvKeypad.ioGroup+1, elfConfiguration.ioConfiguration.cvKeypad.inp , CV_KEYPAD_IN);
            p_Computer->setEfType(elfConfiguration.ioConfiguration.cvKeypad.ioGroup+1, elfConfiguration.ioConfiguration.cvKeypad.ef , CV_KEYPAD_EF);
        }
    }

    if (elfConfiguration.useLatchKeyboard)
    {
        latchKeyboardPointer = new KeybLatch;
        latchKeyboardPointer->configure(elfConfiguration.ioConfiguration, computerConfiguration.saveCommand_);
        p_Computer->setOutType(elfConfiguration.ioConfiguration.keybLatchIoGroup +1, elfConfiguration.ioConfiguration.keybLatchOut, KEYB_LATCH_OUT);
        p_Computer->setEfType(elfConfiguration.ioConfiguration.keybLatchIoGroup +1, elfConfiguration.ioConfiguration.keybLatchEf, KEYB_LATCH_EF);
    }
    
    if (elfConfiguration.useMatrixKeyboard)
    {
        matrixKeyboardPointer = new KeybMatrix;
        matrixKeyboardPointer->configure(elfConfiguration.ioConfiguration, computerConfiguration.saveCommand_);
        p_Computer->setInType(elfConfiguration.ioConfiguration.keybMatrixIoGroup +1, elfConfiguration.ioConfiguration.keybMatrixIn, KEYB_MATRIX_IN);
    }

    if (elfConfiguration.useKeyboard)
        configureKeyboard(XML, elfConfiguration.ioConfiguration, computerConfiguration.addressLocations, computerConfiguration.saveCommand_);

    if (elfConfiguration.useKeyb1871)
        configureKeyb1871(XML, elfConfiguration.ioConfiguration, computerConfiguration.addressLocations, computerConfiguration.saveCommand_);

    if (elfConfiguration.usePS2)
        configurePs2(elfConfiguration.ps2Interrupt, elfConfiguration.ioConfiguration);
    
    if (elfConfiguration.usePs2gpio)
        configurePs2gpio(elfConfiguration.forceUpperCase, elfConfiguration.ioConfiguration);
    
    if (elfConfiguration.useUsbSb)
    {
        ioGroup = "";
        wxString printBuffer1 = "", printBuffer2 = "", printBuffer3 = "";
        if (elfConfiguration.ioConfiguration.usbSbIoGroup != -1)
            ioGroup.Printf(" on group %d", elfConfiguration.ioConfiguration.usbSbIoGroup);

        p_Computer->setEfType(elfConfiguration.ioConfiguration.usbSbIoGroup+1, elfConfiguration.ioConfiguration.usbSbEf, SB_EF);
        p_Computer->setInType(elfConfiguration.ioConfiguration.usbSbIn5.qValue, elfConfiguration.ioConfiguration.usbSbIoGroup+1, elfConfiguration.ioConfiguration.usbSbIn5.portNumber, SB_IN5);
        p_Computer->setInType(elfConfiguration.ioConfiguration.usbSbIn5.qValue, elfConfiguration.ioConfiguration.usbSbIoGroup+1, elfConfiguration.ioConfiguration.usbSbIn6.portNumber, SB_IN6);
        p_Computer->setOutType(elfConfiguration.ioConfiguration.usbSbOut.qValue, elfConfiguration.ioConfiguration.usbSbIoGroup+1, elfConfiguration.ioConfiguration.usbSbOut.portNumber, SB_OUT);

        p_Main->message("Configuring USB for COMX Super Board" + ioGroup);
        if (elfConfiguration.ioConfiguration.usbSbOut.qValue == -1)
            printBuffer1.Printf("    Output %d: UART out, ", elfConfiguration.ioConfiguration.usbSbOut.portNumber);
        else
        {
            printBuffer1.Printf("    Q = %d & output %d: UART out, ", elfConfiguration.ioConfiguration.usbSbOut.qValue, elfConfiguration.ioConfiguration.usbSbOut.portNumber);
        }

        if (elfConfiguration.ioConfiguration.usbSbIn5.qValue == -1)
            printBuffer2.Printf("input %d: UART in, ", elfConfiguration.ioConfiguration.usbSbIn5.portNumber);
        else
        {
            printBuffer2.Printf("Q = %d & input %d: UART in, ", elfConfiguration.ioConfiguration.usbSbIn5.qValue, elfConfiguration.ioConfiguration.usbSbIn5.portNumber);
        }

        if (elfConfiguration.ioConfiguration.usbSbIn6.qValue == -1)
            printBuffer3.Printf("input %d: reset ef", elfConfiguration.ioConfiguration.usbSbIn6.portNumber);
        else
        {
            printBuffer3.Printf("Q = %d & input %d: reset ef", elfConfiguration.ioConfiguration.usbSbIn6.qValue, elfConfiguration.ioConfiguration.usbSbIn5.portNumber);
        }

        p_Main->message(printBuffer1+printBuffer2+printBuffer3);
    }
    
    diagDmaLedOn_ = false;
    if (computerConfiguration.useDiagnosticBoard_)
    {
        setInType(elfConfiguration.ioConfiguration.diagIn1, COMXDIAGIN1);
        setInType(elfConfiguration.ioConfiguration.diagIn2, COMXDIAGIN2);
        setOutType(elfConfiguration.ioConfiguration.diagOut, COMXDIAGOUT1);
        
        p_Main->message("Configuring Diagnose Card");

        printBuffer.Printf("    Input %d, bit 1: debounce, bit 2: Step, bit 6: Abort, bit 7 Repeat", elfConfiguration.ioConfiguration.diagIn1);
        p_Main->message(printBuffer);

        printBuffer.Printf("    Input %d, bit 1: ROM Checksum, bit 2: IDEN, bit 3: Factory unit", elfConfiguration.ioConfiguration.diagIn2);
        p_Main->message(printBuffer);

        printBuffer.Printf("    Output %d, automated keyboard test", elfConfiguration.ioConfiguration.diagOut);
        p_Main->message(printBuffer);

        if (computerConfiguration.diagRomOn_ == 1)
            diagRomActive_ = true;
        else
            diagRomActive_ = false;

        if (elfConfiguration.usev1870)
        {
            p_Main->eventUpdateDiagLedStatus(1, diagRomActive_);
            p_Main->eventUpdateDiagLedStatus(2, diagDmaLedOn_);
        }
    }
    if (elfConfiguration.useTapeHw)
    {
        ioGroup = "";
        if (elfConfiguration.ioConfiguration.tapeIoGroup != -1)
            ioGroup.Printf(" (on group %X)", elfConfiguration.ioConfiguration.tapeIoGroup);

        p_Main->message("Configuring CyberVision Cassette" + ioGroup);
        
        p_Computer->setEfType(elfConfiguration.ioConfiguration.tapeIoGroup+1, elfConfiguration.ioConfiguration.tapeEfOut, TAPE_CV_EF_OUT);
        p_Computer->setEfType(elfConfiguration.ioConfiguration.tapeIoGroup+1, elfConfiguration.ioConfiguration.tapeEf, TAPE_CV_EF);
        printBuffer.Printf("    EF %d: write buffer empty, EF %d: data ready", elfConfiguration.ioConfiguration.tapeEfOut, elfConfiguration.ioConfiguration.tapeEf);
        p_Main->message(printBuffer);
        
        p_Computer->setOutType(elfConfiguration.ioConfiguration.tapeOut.qValue, elfConfiguration.ioConfiguration.tapeIoGroup+1, elfConfiguration.ioConfiguration.tapeOut.portNumber, TAPE_CV_OUT);
        p_Computer->setInType(elfConfiguration.ioConfiguration.tapeIn.qValue, elfConfiguration.ioConfiguration.tapeIoGroup+1, elfConfiguration.ioConfiguration.tapeIn.portNumber, TAPE_CV_IN);
        
        if (elfConfiguration.ioConfiguration.tapeOut.qValue == -1)
            printBuffer.Printf("    Output %d: write data", elfConfiguration.ioConfiguration.tapeOut.portNumber);
        else
        {
            printBuffer.Printf("    Q = %d & output %d: write data", elfConfiguration.ioConfiguration.tapeOut.qValue, elfConfiguration.ioConfiguration.tapeOut.portNumber);
        }
        p_Main->message(printBuffer);

        if (elfConfiguration.ioConfiguration.tapeIn.qValue == -1)
            printBuffer.Printf("    Input %d: read data", elfConfiguration.ioConfiguration.tapeIn.portNumber);
        else
        {
            printBuffer.Printf("    Q = %d & input %d: read data ", elfConfiguration.ioConfiguration.tapeIn.qValue, elfConfiguration.ioConfiguration.tapeIn.portNumber);
        }
        p_Main->message(printBuffer);
        p_Main->message("");

        p_Main->eventHwTapeStateChange(HW_TAPE_STATE_PLAY);
    }
    
    if (elfConfiguration.useBitSound)
    {
        ioGroup = "";
        if (elfConfiguration.ioConfiguration.bitSoundIoGroup != -1)
            ioGroup.Printf(" (on group %X)", elfConfiguration.ioConfiguration.bitSoundIoGroup);

        p_Main->message("Configuring sound" + ioGroup);
        
        p_Computer->setOutType(elfConfiguration.ioConfiguration.bitSoundOut.qValue, elfConfiguration.ioConfiguration.bitSoundIoGroup+1, elfConfiguration.ioConfiguration.bitSoundOut.portNumber, BITSOUND_OUT);
        
        if (elfConfiguration.ioConfiguration.bitSoundOut.qValue == -1)
            printBuffer.Printf("    Output %d using mask %d: tone wave", elfConfiguration.ioConfiguration.bitSoundOut.portNumber, elfConfiguration.ioConfiguration.bitSoundMask);
        else
        {
            printBuffer.Printf("    Q = %d & output %d using mask %d: tone wave", elfConfiguration.ioConfiguration.bitSoundOut.qValue, elfConfiguration.ioConfiguration.bitSoundOut.portNumber, elfConfiguration.ioConfiguration.bitSoundMask);
        }
        p_Main->message(printBuffer);
        p_Main->message("");
    }
}

void Xmlemu::moveWindows()
{
    if (elfConfiguration.panelType_ == PANEL_COSMAC && elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->Move(p_Main->getKeypadPos(ELF));
    if (elfConfiguration.panelType_ == PANEL_COSMAC && elfConfiguration.useLedModule)
        ledModulePointer->Move(p_Main->getLedModulePos());
    if (elfConfiguration.useSwitch)
        p_Elf2Kswitch->Move(p_Main->getElf2KswitchPos());
    if (elfConfiguration.useHex)
        p_Elf2Khex->Move(p_Main->getKeypadPos(XML));
    if (elfConfiguration.usePixie)
        pixiePointer->Move(p_Main->getPixiePos(XML));
    if (elfConfiguration.use1864)
        cdp1864Pointer->Move(p_Main->getCdp1864Pos(XML));
    if (elfConfiguration.useTMS9918)
        tmsPointer->Move(p_Main->getTmsPos(XML));
    if (elfConfiguration.use6845)
        mc6845Pointer->Move(p_Main->get6845Pos(XML));
    if (elfConfiguration.use6847)
        mc6847Pointer->Move(p_Main->get6847Pos(XML));
    if (elfConfiguration.use8275)
        i8275Pointer->Move(p_Main->get8275Pos(XML));
    if (elfConfiguration.usev1870)
        vis1870Pointer->Move(p_Main->getV1870Pos(XML));
    if (elfConfiguration.useSN76430N)
        sn76430nPointer->Move(p_Main->getSN76430NPos(XML));
    if (elfConfiguration.vtType != VTNONE)
        vtPointer->Move(p_Main->getVtPos(XML));
}

void Xmlemu::updateTitle(wxString Title)
{
    if (elfConfiguration.panelType_ == PANEL_COSMAC && elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
        keypadPointer->SetTitle("Keypad"+Title);
    if (elfConfiguration.panelType_ == PANEL_COSMAC && elfConfiguration.useLedModule)
        ledModulePointer->SetTitle("Led Module"+Title);
    if (elfConfiguration.usePixie)
        pixiePointer->SetTitle(p_Main->getRunningComputerText() + " - Pixie"+Title);
    if (elfConfiguration.use1864)
        cdp1864Pointer->SetTitle(p_Main->getRunningComputerText() + " - CDP1864"+Title);
    if (elfConfiguration.useTMS9918)
        tmsPointer->SetTitle(p_Main->getRunningComputerText() + " - TMS 9918"+Title);
    if (elfConfiguration.use6845)
        mc6845Pointer->SetTitle(p_Main->getRunningComputerText() + " - MC6845"+Title);
    if (elfConfiguration.use6847)
        mc6847Pointer->SetTitle(p_Main->getRunningComputerText() + " - MC6847"+Title);
    if (elfConfiguration.use8275)
        i8275Pointer->SetTitle(p_Main->getRunningComputerText() + " - Intel 8275"+Title);
    if (elfConfiguration.usev1870)
        vis1870Pointer->SetTitle(p_Main->getRunningComputerText() + " - VIS 1870"+Title);
    if (elfConfiguration.useSN76430N)
        sn76430nPointer->SetTitle(p_Main->getRunningComputerText() + " - SN76430N"+Title);
    if (elfConfiguration.vtType == VT52)
        vtPointer->SetTitle(p_Main->getRunningComputerText() + " - VT 52"+Title);
    if (elfConfiguration.vtType == VT100)
        vtPointer->SetTitle(p_Main->getRunningComputerText() + " - VT 100"+Title);
    if (elfConfiguration.useSwitch)
        p_Elf2Kswitch->SetTitle("Switch Panel"+Title);
    if (elfConfiguration.useHex)
    {
        switch (elfConfiguration.panelType_)
        {
            case PANEL_ELF2K:
                p_Elf2Khex->SetTitle("Hex Panel"+Title);
            break;
                
            case PANEL_COSMICOS:
                p_Cosmicoshex->SetTitle("Hex Panel"+Title);
            break;
        }
    }
}

void Xmlemu::setForceUpperCase(bool status)
{
    if (elfConfiguration.vtType != VTNONE)
        vtPointer->setForceUCVt(status);
    if (elfConfiguration.useKeyboard)
        setForceUpperCaseKeyboard(status);
    if (elfConfiguration.usePs2gpio)
        setForceUpperCasePs2gpio(status);
}

void Xmlemu::onReset()
{
    if (elfConfiguration.usev1870)
        p_Main->eventUpdateDiagLedStatus(5, true);
    resetPressed_ = true;
}

void Xmlemu::sleepComputer(long ms)
{
    threadPointer->Sleep(ms);
}

Byte Xmlemu::read6845CharRom(Word address)
{
    if (elfConfiguration.use6845)
        return mc6845Pointer->read6845CharRom(address);
    else
        return 0;
}

void Xmlemu::write6845CharRom(Word address, Byte value)
{
    if (elfConfiguration.use6845)
        mc6845Pointer->write6845CharRom(address, value);
}

Byte Xmlemu::read8275CharRom(Word address)
{
    if (elfConfiguration.use8275)
        return i8275Pointer->read8275CharRom(address);
    else
        return 0;
}

void Xmlemu::write8275CharRom(Word address, Byte value)
{
    if (elfConfiguration.use8275)
        i8275Pointer->write8275CharRom(address, value);
}

Byte Xmlemu::read8275VideoRam(Word address)
{
    if (elfConfiguration.use8275)
        return i8275Pointer->read8275VideoRam(address);
    else
        return 0;
}

void Xmlemu::write8275VideoRam(Word address, Byte value)
{
    if (elfConfiguration.use8275)
        i8275Pointer->write8275VideoRam(address, value);
}

Byte Xmlemu::read6847CharRom(Word address)
{
    if (elfConfiguration.use6847)
        return mc6847Pointer->read6847CharRom(address);
    else
        return 0;
}

void Xmlemu::write6847CharRom(Word address, Byte value)
{
    if (elfConfiguration.use6847)
        mc6847Pointer->write6847CharRom(address, value);
}

int Xmlemu::readDirect6847(Word address)
{
    if (elfConfiguration.use6847)
        return mc6847Pointer->readDirect6847(address);
    else
        return 0;
}

void Xmlemu::writeDirect6847(Word address, int value)
{
    if (elfConfiguration.use6847)
        mc6847Pointer->writeDirect6847(address, value);
}

Word Xmlemu::get6847RamMask()
{
     if (elfConfiguration.use6847)
        return mc6847Pointer->get6847RamMask();
    else
        return 0;
}

void Xmlemu::setLedMs(long ms)
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            elfScreenPointer->setLedMs(ms);
            if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                keypadPointer->setLedMs(ms);
            if (elfConfiguration.useLedModule && !ledModuleClosed_)
                ledModulePointer->setLedMs(ms);
        break;

        case PANEL_ELFII:
            elf2ScreenPointer->setLedMs(ms);
        break;

        case PANEL_SUPER:
            superScreenPointer->setLedMs(ms);
        break;

        case PANEL_ELF2K:
            elf2KScreenPointer->setLedMs(ms);
        break;

        case PANEL_COSMICOS:
            cosmicosScreenPointer->setLedMs(ms);
            p_Cosmicoshex->setLedMs(ms);
        break;

        case PANEL_MICROTUTOR:
            microtutorScreenPointer->setLedMs(ms);
        break;

        case PANEL_MICROTUTOR2:
            microtutor2ScreenPointer->setLedMs(ms);
        break;
    }
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((elfClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

Byte Xmlemu::getKey(Byte vtOut)
{
    Byte tempVtOut;
    
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            tempVtOut = elfScreenPointer->getKey(vtOut);

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
        break;

        case PANEL_ELFII:
            return elf2ScreenPointer->getKey(vtOut);
        break;

        case PANEL_SUPER:
            return superScreenPointer->getKey(vtOut);
        break;

        case PANEL_ELF2K:
            tempVtOut = elf2KScreenPointer->getKey(vtOut);

            if (tempVtOut == vtOut)
            {
                if (elfConfiguration.useSwitch)
                    tempVtOut = p_Elf2Kswitch->getKey(vtOut);
                if (tempVtOut == vtOut)
                {
                    if (elfConfiguration.useHex)
                        tempVtOut = p_Elf2Khex->getKey(vtOut);
                }
            }
            return tempVtOut;
        break;

        case PANEL_COSMICOS:
            tempVtOut = cosmicosScreenPointer->getKey(vtOut);

            if (tempVtOut == vtOut)
                return p_Cosmicoshex->getKey(vtOut);
            else
                return tempVtOut;
        break;
    }
    return 0;
}

void Xmlemu::activateMainWindow()
{
    bool maximize = IsMaximized();
    Iconize(false);
    Raise();
    Show(true);
    Maximize(maximize);
}

void Xmlemu::releaseButtonOnScreen(HexButton* buttonPointer, int buttonType)
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                keypadPointer->releaseButtonOnScreen(buttonPointer);
        break;

        case PANEL_ELFII:
            elf2ScreenPointer->releaseButtonOnScreen(buttonPointer);
        break;

        case PANEL_SUPER:
            superScreenPointer->releaseButtonOnScreen(buttonPointer);
        break;

        case PANEL_ELF2K:
            if (elfConfiguration.useHex)
                p_Elf2Khex->releaseButtonOnScreen(buttonPointer);
        break;

        case PANEL_COSMICOS:
            if (buttonType == PANEL_HEX_BUTTON || buttonType == PANEL_WIDE_HEX_BUTTON)
                p_Cosmicoshex->releaseButtonOnScreen(buttonPointer);
            else
                cosmicosScreenPointer->releaseButtonOnScreen(buttonPointer);
        break;
    }
}

void Xmlemu::refreshPanel()
{
    switch (elfConfiguration.panelType_)
    {
        case PANEL_COSMAC:
            elfScreenPointer->refreshPanel();
            if (elfConfiguration.useLedModule && !ledModuleClosed_)
                ledModulePointer->refreshPanel();
            if (elfConfiguration.useHexKeyboard && !hexKeypadClosed_)
                keypadPointer->refreshPanel();
        break;

        case PANEL_ELFII:
            elf2ScreenPointer->refreshPanel();
        break;

        case PANEL_SUPER:
            superScreenPointer->refreshPanel();
        break;

        case PANEL_ELF2K:
            elf2KScreenPointer->refreshPanel();
        break;

        case PANEL_COSMICOS:
            cosmicosScreenPointer->refreshPanel();
        break;

        case PANEL_MICROTUTOR:
            microtutorScreenPointer->refreshPanel();
        break;

        case PANEL_MICROTUTOR2:
            microtutor2ScreenPointer->refreshPanel();
        break;
    }
}

void Xmlemu::OnRtcTimer(wxTimerEvent&WXUNUSED(event))
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

void Xmlemu::writeRtc(int address, Byte value)
{
    if (address == 0xc || address == 0xd)
        return;
    
    if (address == 0xa)
        rtcRam_[address] = (rtcRam_[address] & 0x80) | (value & 0x7f);
    else
        rtcRam_[address] = value;
    p_Main->updateDebugMemory(address);
}

void Xmlemu::cid1Bit8(bool set)
{
    if (elfConfiguration.ioConfiguration.bitKeypad[0].defined)
        bitkeypadPointer[0]->cid1Bit8(set);
}

void Xmlemu::changeDiskName(int disk, wxString dirName, wxString fileName)
{
    if (elfConfiguration.fdc1770Enabled || elfConfiguration.fdc1793Enabled)
        setFdcDiskname(disk, dirName + fileName);
}

Byte Xmlemu::readPramDirect(Word address)
{
    return vis1870Pointer->readPramDirect(address);
}

Byte Xmlemu::readCramDirect(Word address)
{
    return vis1870Pointer->readCramDirect(address);
}

Byte Xmlemu::readColourRamDirect(Word address)
{
    return vis1870Pointer->readColourRamDirect(address);
};

void Xmlemu::writePramDirect(Word address, Byte value)
{
    vis1870Pointer->writePramDirect(address, value);
}

void Xmlemu::writeCramDirect(Word address, Byte value)
{
    vis1870Pointer->writeCramDirect(address, value);
}

void Xmlemu::writeColourRamDirect(Word address, Byte value)
{
    vis1870Pointer->writeColourRamDirect(address, value);
};

int Xmlemu::getMaxLinesPerChar()
{
    return vis1870Pointer->getMaxLinesPerChar();
}

Word Xmlemu::getPageMemorySize()
{
    return vis1870Pointer->getPageMemorySize();
}

Word Xmlemu::getCharMemorySize()
{
    return vis1870Pointer->getCharMemorySize();
}

Byte Xmlemu::getPcbMask()
{
    return vis1870Pointer->getPcbMask();
};

void Xmlemu::saveNvRam()
{
    Byte value;
    wxFile outputFile;
    
    for (size_t i=0; i<nvramDetails.size(); i++)
    {
        if (wxFile::Exists(nvramDetails[i].dirname+nvramDetails[i].filename))
            outputFile.Open(nvramDetails[i].dirname+nvramDetails[i].filename, wxFile::write);
        else
            outputFile.Create(nvramDetails[i].dirname+nvramDetails[i].filename);
        for (long address = nvramDetails[i].start; address <= nvramDetails[i].end; address++)
        {
            value = mainMemory_[address];
            outputFile.Write(&value, 1);
        }
        outputFile.Close();
    }
}

void Xmlemu::loadNvRam(size_t configNumber)
{
    NvramDetails newNvram;

    newNvram.start = computerConfiguration.memConfig_[configNumber].start;
    newNvram.end = computerConfiguration.memConfig_[configNumber].end;
    newNvram.dirname = computerConfiguration.memConfig_[configNumber].dirname;
    newNvram.filename = computerConfiguration.memConfig_[configNumber].filename;
    nvramDetails.push_back(newNvram);

    wxFFile inFile;
    size_t length = newNvram.end-newNvram.start+1;

    char* buffer = new char[length];

//    char buffer[length];

    if (wxFile::Exists(newNvram.dirname+newNvram.filename))
    {
        if (inFile.Open(newNvram.dirname+newNvram.filename, _("rb")))
        {
            length = inFile.Read(buffer, length);
            for (size_t i=0; i<length; i++)
                mainMemory_[i+newNvram.start] = (Byte)buffer[i];
            inFile.Close();
        }
    }

    delete[] buffer;
}

void Xmlemu::checkComputerFunction()
{
//#if defined (__linux__)
//    if (computerConfiguration.locationTrigger[scratchpadRegister_[programCounter_]].index[0] == 0xff)
//#else
    if (computerConfiguration.locationTrigger[scratchpadRegister_[programCounter_]].index[0] == -1)
//#endif
        return;

// ***   for (char trigger : computerConfiguration.locationTrigger[scratchpadRegister_[programCounter_]].index)
	for (std::vector<signed char>::iterator trigger = computerConfiguration.locationTrigger[scratchpadRegister_[programCounter_]].index.begin (); trigger != computerConfiguration.locationTrigger[scratchpadRegister_[programCounter_]].index.end (); ++trigger)
    {
        if (computerConfiguration.addressLocations.locationInfo[*trigger].checkAddressInfo.size() == 0)
            executeFunction(computerConfiguration.addressLocations.locationInfo[*trigger].type, computerConfiguration.addressLocations.locationInfo[*trigger].additionalAddress);
        else if (computerConfiguration.addressLocations.locationInfo[*trigger].checkAddressInfo.size() == 1)
        {
            if (readMemDebug(computerConfiguration.addressLocations.locationInfo[*trigger].checkAddressInfo[0].checkAddress) == computerConfiguration.addressLocations.locationInfo[*trigger].checkAddressInfo[0].checkValue)
                executeFunction(computerConfiguration.addressLocations.locationInfo[*trigger].type, computerConfiguration.addressLocations.locationInfo[*trigger].additionalAddress);
        }
        else
        {
            bool execute = true;
//#if defined (__WXMAC__)
//            for (CheckAddressInfo addressInfo : computerConfiguration.addressLocations.locationInfo[*trigger].checkAddressInfo)
//            {
//                if (readMemDebug(addressInfo.checkAddress) != addressInfo.checkValue)
//                    execute = false;
//            }
//#else
            for (std::vector<CheckAddressInfo>::iterator addressInfo = computerConfiguration.addressLocations.locationInfo[*trigger].checkAddressInfo.begin (); addressInfo != computerConfiguration.addressLocations.locationInfo[*trigger].checkAddressInfo.end (); ++addressInfo)
            {
                if (readMemDebug(addressInfo->checkAddress) != addressInfo->checkValue)
                    execute = false;
            }
//#endif
            if (execute)
                executeFunction(computerConfiguration.addressLocations.locationInfo[*trigger].type, computerConfiguration.addressLocations.locationInfo[*trigger].additionalAddress);
        }
    }
}

void Xmlemu::executeFunction(int function, Word additionalAddress)
{
    switch (function)
    {
        case INFO_RESET_STATE:
            elfRunState_ = RESETSTATE;
        break;

        case INFO_BASIC_STATE:
            elfRunState_ = BASICSTATE;
        break;

        case INFO_RUN_STATE:
            elfRunState_ = RUNSTATE;
        break;

        case INFO_STOP_CAS:
            p_Main->stopCassette();
        break;

        case INFO_START_CAS_SAVE:
            p_Main->startCassetteSave(0);
        break;

        case INFO_START_CAS_LOAD:
            p_Main->setSwName ("");
            p_Main->eventUpdateTitle();
            p_Main->startCassetteLoad(0);
        break;

        case INFO_START_CAS_DLOAD:
            p_Main->startCassetteLoad(0);
        break;

        case INFO_STOP_THERMAL:
            thermalPrinting_ = false;
        break;
            
        case INFO_SET_DOS_FILENAME:
            setDosFileName();
        break;

        case INFO_BATCH_START_SAVE:
            if (p_Main->isBatchConvertActive() && !batchInProgress_)
                batchConvert();
        break;
            
        case INFO_BATCH_END_SAVE:
            batchInProgress_ = false;
        break;

        case INFO_START_XMODEM_SAVE:
            p_Main->startAutoTerminalSave(TERM_XMODEM_SAVE);
        break;

        case INFO_START_XMODEM_LOAD:
            p_Main->startAutoTerminalLoad(TERM_XMODEM_LOAD);
        break;
            
        case INFO_START_YMODEM_SAVE:
            p_Main->startYsTerminalSave(TERM_YMODEM_SAVE);
        break;
            
        case INFO_CORRECT_CAPS:
            if (elfConfiguration.useLatchKeyboard)
                latchKeyboardPointer->checkCaps();
            if (elfConfiguration.useMatrixKeyboard)
                matrixKeyboardPointer->checkCaps();
        break;
            
        case INFO_ELFOS_BOOT:
            if (elfConfiguration.useHexModem)
            {
                elfConfiguration.useHexModem = false;
                elfConfiguration.useXmodem = true;
                p_Main->eventSetButtonLabel ("XmodemButtonXml", "XMODEM");
            }
        break;
            
        case INFO_START_CHIP8:
            chip8mainLoop_ = additionalAddress;
            activateElfOsChip8();
        break;

        case INFO_CV_TAPE_PLAY:
            if (readMem(scratchpadRegister_[programCounter_]) == 0x7b)
                p_Main->eventHwTapeStateChange(HW_TAPE_STATE_PLAY);
        break;
            
        case INFO_CV_TAPE_FF:
            if (readMem(scratchpadRegister_[programCounter_]) == 0x7b)
                p_Main->eventHwTapeStateChange(HW_TAPE_STATE_FF);
        break;
            
        case INFO_CV_TAPE_REC:
            if (readMem(scratchpadRegister_[programCounter_]) == 0x7b)
                p_Main->eventHwTapeStateChange(HW_TAPE_STATE_REC);
        break;
    }
}

void Xmlemu::startComputerRun(bool load)
{
    if (elfConfiguration.useKeyboard)
        startElfRun(load);
    if (elfConfiguration.useKeyb1871)
        start1871Run(load);
    if (elfConfiguration.useLatchKeyboard)
        latchKeyboardPointer->startLatchRun(load);
    if (elfConfiguration.useMatrixKeyboard)
        matrixKeyboardPointer->startRun(load);

    if (elfConfiguration.vtType != VTNONE)
    {
        if (cpuMode_ != RUN)
        {
            onInButtonPress();
            onRun();
            vtPointer->startElfRun(load, true);
        }
        else
            vtPointer->startElfRun(load, false);
    }
}

bool Xmlemu::isComputerRunning()
{
    if (elfRunState_ == RUNSTATE)
        return true;
    else
        return false;
}

void Xmlemu::terminalSave(wxString fileName, int protocol)
{
    if (elfConfiguration.vtType != VTNONE)
        vtPointer->terminalSaveVt(fileName, protocol);
}

void Xmlemu::terminalYsSave(wxString fileName, int protocol)
{
    if (elfConfiguration.vtType != VTNONE)
        vtPointer->terminalYsSaveVt(fileName, protocol);
}

void Xmlemu::terminalLoad(wxString filePath, wxString fileName, int protocol)
{
    if (elfConfiguration.vtType != VTNONE)
        vtPointer->terminalLoadVt(filePath, protocol);
}

void Xmlemu::terminalStop()
{
    if (elfConfiguration.vtType != VTNONE)
        vtPointer->terminalStopVt();
}

void Xmlemu::setDivider(Byte value)
{
    const int freq [16] = {0, 256, 128, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2};

    if (value == 0)
        cycleSize_ = -1;
    else
        cycleSize_ = (int) (((elfClockSpeed_ * 1000000) / 8) / freq [value]);
    cycleValue_ = cycleSize_;
}

void Xmlemu::dataAvailableVt100(bool data, int WXUNUSED(uartNumber))
{
    vtPointer->dataAvailableUart16450(data);
}

void Xmlemu::dataAvailableSerial(bool data)
{
    p_Serial->dataAvailableUart16450(data);
}

void Xmlemu::thrStatusVt100(bool data)
{
    vtPointer->thrStatusUart16450(data);
}

void Xmlemu::thrStatusSerial(bool data)
{
    p_Serial->thrStatusUart16450(data);
}

void Xmlemu::saveRtc()
{
    ElfConfiguration currentElfConfig = p_Main->getElfConfiguration(computerType_);

    if (!currentElfConfig.useRtcDS12887)
        return;

    Byte value;
    wxFile outputFile;

    wxString fileName = p_Main->getMainDir()+"rtcdump.bin";

    if (wxFile::Exists(fileName))
        outputFile.Open(fileName, wxFile::write);
    else
        outputFile.Create(fileName);
    for (int address = 0; address < 128; address++)
    {
        value = rtcRam_[address];
        outputFile.Write(&value, 1);
    }
    outputFile.Close();
}

void Xmlemu::loadRtc()
{
    ElfConfiguration currentElfConfig = p_Main->getElfConfiguration(computerType_);

    if (!currentElfConfig.useRtcDS12887)
        return;

    if (currentElfConfig.clearRtc)
    {
        for (int i = 0; i<128; i++)
        {
            rtcRam_[i] = 0;
            p_Main->updateDebugMemory(i);
        }
        rtcRam_[0xa] = 0x20;
        rtcRam_[0xb] = 0x6;
        rtcRam_[0xc] = 0;
        rtcRam_[0xd] = 0x80;
        currentElfConfig.clearRtc = false;
        p_Main->setElfConfiguration(currentElfConfig);
        p_Main->eventSetCheckBox("XmlClearRtc", false);
        return;
    }
    
    wxFFile inFile;
    size_t length;
    char buffer[128];

    if (wxFile::Exists(p_Main->getMainDir()+"rtcdump.bin"))
    {
        if (inFile.Open(p_Main->getMainDir()+"rtcdump.bin", _("rb")))
        {
            length = inFile.Read(buffer, 128);
            for (size_t i=0; i<length; i++)
            {
                rtcRam_[i] = (Byte)buffer[i];
                p_Main->updateDebugMemory(i);
            }
            inFile.Close();
        }
    }
}

Byte Xmlemu::readDirectRtc(Word address)
{
    return rtcRam_[address];
}

void Xmlemu::writeDirectRtc(Word address, Byte value)
{
    rtcRam_[address] = value;
}

void Xmlemu::setDosFileName()
{
    wxString name;
    name = "";
    Word address = 0xbc03;
    while ((mainMemory_[address] != 0xff) && (address < 0xbc15))
    {
        name = name + (char)mainMemory_[address];
        address++;
    }
    if (name[0] == 0 && name[1] == 0)
        return;

    p_Main->eventSetSwName(name);
    p_Main->setPloadFileName(name + p_Main->getRunningComputerPloadExtension());
    if (mainMemory_[0xbc17] == 2)
    {
        Word saveStart, saveEnd, saveExec;

        saveStart = mainMemory_[0xbc29] << 8;
        saveEnd = saveStart + (mainMemory_[0xbc25] << 8) + mainMemory_[0xbc26] - 4;
        saveExec = mainMemory_[0xbc29] << 8;
        p_Main->eventSetLocation(true, saveStart, saveEnd, saveExec);
    }
    else
        p_Main->eventSetLocation(false, 0, 0, 0);
}

void Xmlemu::cidelsaStatusBarDown(int keycode)
{
    switch(keycode)
    {
        case 'A':        // Coin shute A
        case 'B':        // Coin shute B
            for (int ef=1; ef <=4; ef++)
            {
                if (elfConfiguration.ioConfiguration.efKey[ef].defined)
                {
                    if (elfConfiguration.ioConfiguration.efKey[ef].value == keycode)
                        efKeyValue[ef] = 0;
                }
            }
        break;

        case '1':            // 1 Player
        case '2':            // 2 Players
            if (elfConfiguration.ioConfiguration.bitKeypad[0].defined)
                bitkeypadPointer[0]->keyDown(keycode);
        break;

        case WXK_SPACE:        // Fire
            if (elfConfiguration.ioConfiguration.bitKeypad[0].defined)
                bitkeypadPointer[0]->keyDown(keycode);
            if (elfConfiguration.usev1870)
                vis1870Pointer->focus();
        break;
    }
}

void Xmlemu::cidelsaStatusBarUp(int keycode)
{
    switch(keycode)
    {
        case 'B':        // Coin shute B
        case 'A':        // Coin shute A
            for (int ef=1; ef <=4; ef++)
            {
                if (elfConfiguration.ioConfiguration.efKey[ef].defined)
                {
                    if (elfConfiguration.ioConfiguration.efKey[ef].value == keycode)
                        efKeyValue[ef] = 1;
                }
            }
            if (elfConfiguration.usev1870)
                vis1870Pointer->focus();
        break;

        case '1':            // 1 Player
        case '2':            // 2 Players
        case WXK_SPACE:        // Fire
            if (elfConfiguration.ioConfiguration.bitKeypad[0].defined)
                bitkeypadPointer[0]->keyUp(keycode);
            if (elfConfiguration.usev1870)
                vis1870Pointer->focus();
        break;
    }
}

void Xmlemu::setBatchFileNumber(int number)
{
    batchFileNumber_ = number;
    batchInProgress_ = false;
        
    p_Main->eventSetConvertState(false);
}

void Xmlemu::batchConvert()
{
    wxString memAccessExtension = "." + p_Main->getRunningComputerPloadExtension();

    if (getBatchFile(memAccessExtension))
    {
        batchInProgress_ = true;
        
        p_Main->setPloadFileNameFull(p_Main->getBatchPath(batchFileNumber_+1) + p_Main->getBatchFile(batchFileNumber_+1));
        
        p_Main->pload();
        
        wxString filename = p_Main->getBatchFile(batchFileNumber_+1);
        filename = filename.Left(filename.Len()-memAccessExtension.Len()) + ".wav";
        
        p_Main->setWaveDir(XML, p_Main->getBatchPath(batchFileNumber_+1));
        p_Main->setWaveFile(XML, filename);
        
        startComputerRun(false);
//        load_ = false;
//        commandText_ = "psave";
//        comxRunCommand_ = 3;
    }
}

bool Xmlemu::getBatchFile(wxString memAccessExtension)
{
    wxString extension = "xxxxx";
    wxString filename, path;
    bool result = true;
    
    while (extension != memAccessExtension && batchFileNumber_ >= 0)
    {
        filename = p_Main->getBatchFile(batchFileNumber_);
//        path = p_Main->getBatchPath(batchFileNumber_);
        extension = filename.Right(memAccessExtension.Len());
        batchFileNumber_--;
    }
    
    if (extension != memAccessExtension )
    {
        result = false;
        p_Main->batchConvertStop();
        p_Main->eventSetConvertState(true);
    }

    return result;
}

void Xmlemu::closeKeyFile()
{
    if (elfConfiguration.useKeyboard)
        closeElfKeyFile();
    if (elfConfiguration.useKeyb1871)
        close1871KeyFile();
    if (elfConfiguration.useLatchKeyboard)
        latchKeyboardPointer->closeKeyFile();
    if (elfConfiguration.useMatrixKeyboard)
        matrixKeyboardPointer->closeKeyFile();
    if (elfConfiguration.usePs2gpio)
        closePs2gpioKeyFile();
}

void Xmlemu::resetV1870VideoModeEf()
{
    elfConfiguration.ioConfiguration.v1870useVideoModeEf = false;
}

void Xmlemu::activateElfOsChip8()
{
    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);
    
    Word address = scratchpadRegister_[0xA];
    
    fetchFileName(address, 100);
}

void Xmlemu::fetchFileName(Word address, size_t length)
{
    wxString fileName = "";
    Byte MemoryValue = readMem(address++);
    
    while (MemoryValue != 0 && length > 0)
    {
        if (MemoryValue >= 32 && MemoryValue < 128)
            fileName += wxString::FromAscii(MemoryValue);
        MemoryValue = readMem(address++);
        length--;
    }
    fileName.MakeLower();
    fileName.Trim();
    
    reLoadKeyDefinition(fileName);
    p_Main->eventSetSwName(fileName);
}

void Xmlemu::removeCosmicosHex()
{
    elfConfiguration.useHex = false;
    p_Cosmicoshex->Show(false);
}

void Xmlemu::startLoad(int tapeNumber, bool button)
{
    lastSampleInt16_ = 0;
    lastSampleChar_ = 0;
    lastSampleInt32_ = 0;
    pulseCount_ = 0;
    tapeInput_ = 0;
    polarity_ = 0;
    silenceCount_ = 0;
    bitNumber_ = -1;
    tapeError_ = 1;

    pulseCountStopTone_ = 2000;
    tapeFormat56_ = false;
    tapeFormatFixed_ = false;
    toneTime_ = 0;
    pauseTapeCounter_ = 0;

    if (tapeActivated_)
    {
        p_Main->turboOn();
        
        if (elfConfiguration.useTapeHw && elfConfiguration.useCvKeypad)
        {
            switch (p_Main->getHwTapeState())
            {
                case HW_TAPE_STATE_FF:
                    p_Computer->forwardTape(TAPE_FORWARD);
                break;

                default:
                    p_Computer->restartTapeLoad(TAPE_PLAY);
                break;
            }
        }
        else
            p_Computer->restartTapeLoad(TAPE_PLAY);
    }
    else
    {
        if (button)
            tapeActivated_ = p_Main->startLoad(tapeNumber);
        else
            tapeActivated_ = p_Main->startCassetteLoad(tapeNumber);
        
        if (elfConfiguration.useTapeHw && !tapeActivated_)
            p_Main->eventHwTapeStateChange(HW_TAPE_STATE_PLAY);
    }
    
//    tapeRunSwitch_ = tapeRunSwitch_ | 1;
}

void Xmlemu::cassetteXmlHw(wxInt32 val, long size)
{
    wxInt32 tape_threshold;
    if (useXmlThreshold_)
        tape_threshold = elfConfiguration.tape_threshold24Bit;
    else
        tape_threshold = threshold24_;

    stepCassetteCounter(size);
    
    if (!elfConfiguration.useTapeHw)
        return;
    
//    if ((tapeRunSwitch_&1) != 1)
//        return;

    wxInt32 difference;
    if (val < lastSampleInt32_)
        difference = lastSampleInt32_ - val;
    else
        difference = val - lastSampleInt32_;
    
    if (difference < tape_threshold)
        silenceCount_++;
    else
    {
        silenceCount_ = 0;
        toneTime_++;
    }
    if (pauseTapeCounter_ > 0)
    {
        pauseTapeCounter_--;
        if (pauseTapeCounter_ == 0)
            pauseTape();
    }

    switch (elfConfiguration.tapeFormat_)
    {
        case TAPE_FORMAT_AUTO:
        case TAPE_FORMAT_PM:
        case TAPE_FORMAT_56:
            if (lastSampleInt32_ <= 0)
            {
                if (val > 0 && silenceCount_ == 0)
                    pulseCount_++;
            }
            else
            {
                if (val < 0 && silenceCount_ == 0)
                    pulseCount_++;
            }
            
            if (pulseCount_ > pulseCountStopTone_ && silenceCount_ > 10 && elfConfiguration.stopTone)
            {
                p_Computer->pauseTape();
                p_Main->turboOff();
        //        tapeRunSwitch_ = tapeRunSwitch_ & 2;
            }
        break;
            
        case TAPE_FORMAT_CV:
            if (lastSampleInt32_ <= 0)
            {
                if (val > 0 && silenceCount_ == 0)
                {
                    pulseCount_++;
                    cassetteCyberVision();
                }
            }
            if (silenceCount_ > 10)
                startBytes_ = 2;
        break;
    }

    switch (elfConfiguration.tapeFormat_)
    {
        case TAPE_FORMAT_PM:
            tapeFormatFixed_ = true;
            cassettePm();
        break;
            
        case TAPE_FORMAT_56:
            pulseCountStopTone_ = 2000;
            tapeFormatFixed_ = true;
            tapeFormat56_ = true;
            cassette56();
        break;
            
        case TAPE_FORMAT_CV:
        break;

        default:
            if (pulseCount_ > 50 && pulseCount_ < 200 && silenceCount_ > 10)
            { // 5.2 & 6.2 tone format, if no tone is detected between 50 & 200 pulses at the start it is PM System format
                pulseCountStopTone_ = 2000;
                tapeFormatFixed_ = true;
                tapeFormat56_ = true;
//                if (computerType_ == FRED1)
//                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1", "-> 5.2/6.2 Tone");
//                else
//                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1_5", "-> 5.2/6.2 Tone");
            }
            if (tapeFormat56_)
                cassette56();
            else
                cassettePm();
        break;
    }
    
    lastSampleInt32_ = val;
}

void Xmlemu::cassetteXmlHw(wxInt16 val, long size)
{
    wxInt16 tape_threshold;
    if (useXmlThreshold_)
        tape_threshold = elfConfiguration.tape_threshold16Bit;
    else
        tape_threshold = threshold16_;

    stepCassetteCounter(size);
    
    if (!elfConfiguration.useTapeHw)
        return;
    
//    if ((tapeRunSwitch_&1) != 1)
//        return;

    wxInt16 difference;
    if (val < lastSampleInt16_)
        difference = lastSampleInt16_ - val;
    else
        difference = val - lastSampleInt16_;
    
    if (difference < tape_threshold)
        silenceCount_++;
    else
    {
        silenceCount_ = 0;
        toneTime_++;
    }
    if (pauseTapeCounter_ > 0)
    {
        pauseTapeCounter_--;
        if (pauseTapeCounter_ == 0)
            pauseTape();
    }
    
    switch (elfConfiguration.tapeFormat_)
    {
        case TAPE_FORMAT_AUTO:
        case TAPE_FORMAT_PM:
        case TAPE_FORMAT_56:
            if (lastSampleInt16_ <= 0)
            {
                if (val > 0 && silenceCount_ == 0)
                    pulseCount_++;
            }
            else
            {
                if (val < 0 && silenceCount_ == 0)
                    pulseCount_++;
            }
            
            if (pulseCount_ > pulseCountStopTone_ && silenceCount_ > 10 && elfConfiguration.stopTone)
            {
                p_Computer->pauseTape();
                p_Main->turboOff();
        //        tapeRunSwitch_ = tapeRunSwitch_ & 2;
            }
        break;
            
        case TAPE_FORMAT_CV:
            if (lastSampleInt16_ <= 0)
            {
                if (val > 0 && silenceCount_ == 0)
                {
                    pulseCount_++;
                    cassetteCyberVision();
                }
            }
            if (silenceCount_ > 10)
                startBytes_ = 2;
        break;
    }

    switch (elfConfiguration.tapeFormat_)
    {
        case TAPE_FORMAT_PM:
            tapeFormatFixed_ = true;
            cassettePm();
        break;
            
        case TAPE_FORMAT_56:
            pulseCountStopTone_ = 2000;
            tapeFormatFixed_ = true;
            tapeFormat56_ = true;
            cassette56();
        break;
            
        case TAPE_FORMAT_CV:
        break;

        default:
            if (pulseCount_ > 50 && pulseCount_ < 200 && silenceCount_ > 10)
            { // 5.2 & 6.2 tone format, if no tone is detected between 50 & 200 pulses at the start it is PM System format
                pulseCountStopTone_ = 2000;
                tapeFormatFixed_ = true;
                tapeFormat56_ = true;
//                if (computerType_ == FRED1)
//                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1", "-> 5.2/6.2 Tone");
//                else
//                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1_5", "-> 5.2/6.2 Tone");
            }
            if (tapeFormat56_)
                cassette56();
            else
                cassettePm();
        break;
    }
    
    lastSampleInt16_ = val;
}

void Xmlemu::cassetteXmlHw(char val, long size)
{
    char tape_threshold;
    if (useXmlThreshold_)
        tape_threshold = elfConfiguration.tape_threshold8Bit;
    else
        tape_threshold = threshold8_;
        
    stepCassetteCounter(size);

    if (!elfConfiguration.useTapeHw)
        return;

//    if ((tapeRunSwitch_&1) != 1)
//        return;

    char difference;
    if (val < lastSampleChar_)
        difference = lastSampleChar_ - val;
    else
        difference = val - lastSampleChar_;
    
    if (difference < tape_threshold)
        silenceCount_++;
    else
    {
        silenceCount_ = 0;
        toneTime_++;
    }
    if (pauseTapeCounter_ > 0)
    {
        pauseTapeCounter_--;
        if (pauseTapeCounter_ == 0)
            pauseTape();
    }

    switch (elfConfiguration.tapeFormat_)
    {
        case TAPE_FORMAT_AUTO:
        case TAPE_FORMAT_PM:
        case TAPE_FORMAT_56:
            if (lastSampleChar_ <= 0)
            {
                if (val > 0 && silenceCount_ == 0)
                    pulseCount_++;
            }
            else
            {
                if (val < 0 && silenceCount_ == 0)
                    pulseCount_++;
            }
            
            if (pulseCount_ > pulseCountStopTone_ && silenceCount_ > 10 && elfConfiguration.stopTone)
            {
                p_Computer->pauseTape();
                p_Main->turboOff();
        //        tapeRunSwitch_ = tapeRunSwitch_ & 2;
            }
        break;
            
        case TAPE_FORMAT_CV:
            if (lastSampleChar_ <= 0)
            {
                if (val > 0 && silenceCount_ == 0)
                {
                    pulseCount_++;
                    cassetteCyberVision();
                }
            }
            if (silenceCount_ > 10)
                startBytes_ = 2;
        break;
    }

    switch (elfConfiguration.tapeFormat_)
    {
        case TAPE_FORMAT_PM:
            tapeFormatFixed_ = true;
            cassettePm();
        break;
            
        case TAPE_FORMAT_56:
            pulseCountStopTone_ = 2000;
            tapeFormatFixed_ = true;
            tapeFormat56_ = true;
            cassette56();
        break;
            
        case TAPE_FORMAT_CV:
        break;

        default:
            if (pulseCount_ > 50 && pulseCount_ < 200 && silenceCount_ > 10)
            { // 5.2 & 6.2 tone format, if no tone is detected between 50 & 200 pulses at the start it is PM System format
                pulseCountStopTone_ = 2000;
                tapeFormatFixed_ = true;
                tapeFormat56_ = true;
//                if (computerType_ == FRED1)
//                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1", "-> 5.2/6.2 Tone");
//                else
//                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1_5", "-> 5.2/6.2 Tone");
            }
            if (tapeFormat56_)
                cassette56();
            else
                cassettePm();
        break;
    }

    lastSampleChar_ = val;
}

void Xmlemu::stepCassetteCounter(long step)
{
    tapeCounterStep_+=step;
    
    int stepSize=1000;
    
    if (p_Main->isTurboOn())
        stepSize = 100;
        
    if (p_Main->getHwTapeState() == HW_TAPE_STATE_FF)
        stepSize = 100;

    if (p_Main->isTurboOn() && p_Main->getHwTapeState() == HW_TAPE_STATE_FF)
        stepSize = 1;

    long long newPeriod = tapeCounterStep_*stepSize/sampleRate_;
    
    if (tapePeriod_ != newPeriod)
    {
        tapePeriod_ = newPeriod;
        int msec = tapePeriod_ % stepSize;
        int sec = (int) ((tapePeriod_/stepSize) % 60);
        int min = (int) ((tapePeriod_/stepSize) / 60);
        
        int numberOfSeconds = min*60+sec;
        
        if (numberOfSeconds != lastSec_ && p_Main->getHwTapeState() == HW_TAPE_STATE_FF)
        {
            int cva = p_Main->getPsaveData(11);
            float cvb = (float) p_Main->getPsaveData(12)/100;

            remainingForwardSpeed_ += sqrt(cva*cva+2*cvb*numberOfSeconds);
            forwardSpeed_ = (int)remainingForwardSpeed_;
            remainingForwardSpeed_ -= forwardSpeed_;
        }

        switch (stepSize)
        {
            case 1:
                tapeCounter_.Printf("%02d:%02d     ",min,sec);
            break;

            case 100:
                tapeCounter_.Printf("%02d:%02d:%02d ",min,sec,msec);
            break;
                
            default:
                tapeCounter_.Printf("%02d:%02d:%03d",min,sec,msec);
            break;
        }
        p_Main->eventSetStaticTextValue("CasCounterXml", tapeCounter_);
    }

}

void Xmlemu::cassetteCyberVision()
{
    float period = (float) (toneTime_ / pulseCount_) / sampleRate_;
    float freq = (float) 1 / period;
    
    if ((pulseCount_ == 1 && freq < elfConfiguration.tape_frequencyBorder) || pulseCount_ == 2)  // && silenceCount_ > 10)
    {
        if (bitNumber_ == -1)
        {
            if (pulseCount_ == 1)
            {
                if (freq < elfConfiguration.tape_frequencyBorder && elfConfiguration.tape_startBit == 1)
                    bitNumber_++;
                if (freq >= elfConfiguration.tape_frequencyBorder && elfConfiguration.tape_startBit == 0)
                    bitNumber_++;
            }
        }
        else
        {
            if (elfConfiguration.tape_stopBitIgnore)
            {
                if (bitNumber_ < elfConfiguration.tape_dataBits)
                {
                    tapeInput_ = (tapeInput_ << 1) | (pulseCount_&1);
                    bitNumber_++;
                    
                    if (startBytes_ > 0 && tapeInput_ !=0)
                    {
                        tapeInput_ = 0;
                        bitNumber_ = -1;
                        startBytes_ = 2;
                    }
                }
                if (bitNumber_ == elfConfiguration.tape_dataBits)
                {
                    lastTapeInpt_ = tapeInput_;
                    
                    if (startBytes_ > 0 && tapeInput_ == 0)
                        startBytes_--;
                    
                    tapedataReady_ = 0;
                    tapeInput_ = 0;
                    bitNumber_ = -1;
                }
            }
            else
            {
                if (bitNumber_ == elfConfiguration.tape_dataBits)
                {
                    bool stopBit = false;;
                    if (freq < elfConfiguration.tape_frequencyBorder && elfConfiguration.tape_stopBit == 1)
                        stopBit = true;
                    if (freq >= elfConfiguration.tape_frequencyBorder && elfConfiguration.tape_stopBit == 0)
                        stopBit = true;

                    if (stopBit)
                    {
                        lastTapeInpt_ = tapeInput_;
                        
                        if (startBytes_ > 0 && tapeInput_ == 0)
                            startBytes_--;
                        
                        tapedataReady_ = 0;
                        tapeInput_ = 0;
                        bitNumber_ = -1;
                    }
                }
                else
                {
                    tapeInput_ = (tapeInput_ << 1) | (pulseCount_&1);
                    bitNumber_++;
                    
                    if (startBytes_ > 0 && tapeInput_ !=0)
                    {
                        tapeInput_ = 0;
                        bitNumber_ = -1;
                        startBytes_ = 2;
                    }
                }
            }
        }
        
        pulseCount_ = 0;
        toneTime_ = 0;
    }
}

void Xmlemu::cassette56()
{
    float freq;
    wxString message;
    
    if (pulseCount_ > 50 && silenceCount_ > 10)
    {
        tapeFormatFixed_ = true;
        freq = (float) toneTime_ / (pulseCount_ - 1);
        if (bitNumber_ == 8)
        {
            if (freq <= fredFreq_ && (polarity_ & 1) != 1)
            {
                tapeError_ = 0;
//                fredScreenPointer->setErrorLed(1);
                
//                if (inpMode_ == INP_MODE_TAPE_DIRECT)
//                {
//                    message.Printf("Polarity issue at %04X", scratchpadRegister_[0]);
//                    p_Main->eventShowTextMessage(message);
//                }
//                if  (inpMode_ == INP_MODE_TAPE_PROGRAM)
//                {
//                    message.Printf("Polarity issue");
//                    p_Main->eventShowTextMessage(message);
//                }

//                if (inpMode_ == INP_MODE_TAPE_DIRECT)
//                    dmaIn(tapeInput_);
            }
            else
            {
//                if (inpMode_ == INP_MODE_TAPE_DIRECT)
//                    dmaIn(tapeInput_);
//                if  (inpMode_ == INP_MODE_TAPE_PROGRAM)
//                {
                    lastTapeInpt_ = tapeInput_;
                    tapedataReady_ = 0;
//                }
            }
        }
        else
        {
            if (bitNumber_ != -1)
            {
                if (freq <= fredFreq_)
                    tapeInput_ = (1 << bitNumber_) | tapeInput_;
            }
            if (freq <= fredFreq_)
                polarity_++;
        }
        
        if (bitNumber_ == 8)
        {
            polarity_ = 0;
            tapeInput_ = 0;
            bitNumber_ = -1;
        }
        else
            bitNumber_++;
        
        pulseCount_ = 0;
        toneTime_ = 0;
    }
}

void Xmlemu::cassettePm()
{
    wxString message;
    
    if (pulseCount_ > 1 && silenceCount_ > 10)
    {
        if (!tapeFormatFixed_)
        {
//            if (elfConfiguration.tapeFormat_ == TAPE_FORMAT_AUTO)
//            {
//                if (computerType_ == FRED1)
//                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1", "-> PM System");
//                else
//                    p_Main->eventSetStaticTextValue("CurrentTapeFormatTextFRED1_5", "-> PM SYSTEM");
//            }
            tapeFormatFixed_ = true;
        }
        if (bitNumber_ == 8)
        {
            if (pulseCount_ > 6 && (polarity_ & 1) != 1)
            {
                tapeError_ = 0;
//                fredScreenPointer->setErrorLed(1);
                
//                if (inpMode_ == INP_MODE_TAPE_DIRECT)
//                {
//                    message.Printf("Polarity issue at %04X", scratchpadRegister_[0]);
//                    p_Main->eventShowTextMessage(message);
//                }
//                if  (inpMode_ == INP_MODE_TAPE_PROGRAM)
//                {
//                    message.Printf("Polarity issue");
//                    p_Main->eventShowTextMessage(message);
//                }
                bitNumber_ = 0;
                polarity_ = 0;
                tapeInput_ = 0;
//                if (inpMode_ == INP_MODE_TAPE_DIRECT)
//                    dmaIn(tapeInput_);
            }
            else
            {
//                if (inpMode_ == INP_MODE_TAPE_DIRECT)
//                    dmaIn(tapeInput_);
//                if  (inpMode_ == INP_MODE_TAPE_PROGRAM)
//                {
                    lastTapeInpt_ = tapeInput_;
                    tapedataReady_ = 0;
//                }
            }
        }
        else
        {
            if (bitNumber_ != -1)
            {
                if (pulseCount_ > 6)
                    tapeInput_ = (1 << bitNumber_) | tapeInput_;
            }
            if (pulseCount_ > 6)
                polarity_++;
        }
        
        if (bitNumber_ == 8)
        {
            polarity_ = 0;
            tapeInput_ = 0;
            bitNumber_ = -1;
        }
        else
            bitNumber_++;
        
        pulseCount_ = 0;
        toneTime_ = 0;
    }
}

void Xmlemu::finishStopTape()
{
//    inpMode_ = INP_MODE_NONE;
//    tapeRunSwitch_ = tapeRunSwitch_ & 2;
    tapeActivated_ = false;
    tapeRecording_ = false;
    tapeEnd_ = true;
}

void Xmlemu::resetTape()
{
    tapeCounterStep_ = 0;
    tapeCounter_ = "00:00:000";
    tapePeriod_ = 0;
    forwardSpeed_ = 18;
    remainingForwardSpeed_ = 0;
    lastSec_ = -1;
    pauseTapeCounter_ = 0;

    finishStopTape();
    if (elfConfiguration.useTapeHw)
    {
        p_Computer->stopTape();
        p_Main->eventHwTapeStateChange(HW_TAPE_STATE_PLAY);
        p_Main->eventSetStaticTextValue("CasCounterXml", "00:00:000");
    }
}
/*
void MainElf::checkComputerFunction()
{
    if (emsRomDefined_)
    {
                
            case 0x7f11:    // ROM MAPPER MOVE
                Word address = scratchpadRegister_[0x8] - 9;
                fetchFileName(address, 9);
            break;
        }

    }
    
    if (baseGiantBoard_ != 0x10000)
    {
        if (scratchpadRegister_[programCounter_] == baseGiantBoard_ + 0x29)
            p_Main->stopCassette();

        if (scratchpadRegister_[programCounter_] == baseGiantBoard_ + 0x8d)
            p_Main->startCassetteSave(0);

        if (scratchpadRegister_[programCounter_] == baseGiantBoard_ + 0xcd)
            p_Main->startCassetteLoad(0);
    }

    if (baseQuestLoader_ != 0x10000)
    {
        if (scratchpadRegister_[programCounter_] == baseQuestLoader_ + 0x5d)
        {
            p_Main->eventSaveStart(scratchpadRegister_[8]);
            p_Main->eventSaveEnd(scratchpadRegister_[8]+scratchpadRegister_[9]-1);
        }
    }

    if (loadedOs_ == ELFOS)
    {
        if (scratchpadRegister_[programCounter_] == 0x7c40)
        {
            if ((mainMemory_[0x7c00] == 0x30) && (mainMemory_[0x7c01] == 0x06) && (mainMemory_[0x7c02] == 0x0c))
            {
                Word saveStart = (mainMemory_[0x7cae] << 8) + mainMemory_[0x7caf];
                Word saveEnd = saveStart + (mainMemory_[0x7cb0] << 8) + mainMemory_[0x7cb1];
                Word saveExec = (mainMemory_[0x7cb2] << 8) + mainMemory_[0x7cb3];
                p_Main->eventSetLocation(true, saveStart, saveEnd, saveExec);
            }
        }
        if (scratchpadRegister_[programCounter_] == 0)
        {
            if ((mainMemory_[0x2202] == 0xc0) && (mainMemory_[0x2203] == 0x28) && (mainMemory_[0x2204] == 0x65) && (mainMemory_[0x226f] == 0x52))
            {
                loadedProgram_ = RCABASIC3;
                basicExecAddress_[BASICADDR_READY] = BASICADDR_READY_RCA3;
                basicExecAddress_[BASICADDR_KEY_VT_RESTART] = BASICADDR_VT_RESTART_RCA;
                basicExecAddress_[BASICADDR_KEY_VT_INPUT] = BASICADDR_VT_INPUT_RCA;
                p_Main->eventEnableMemAccess(true);
            }
            if ((mainMemory_[0x2202] == 0xc0) && (mainMemory_[0x2203] == 0x28) && (mainMemory_[0x2204] == 0x65) && (mainMemory_[0x226f] == 0x55))
            {
                loadedProgram_ = RCABASIC4;
                basicExecAddress_[BASICADDR_READY] = BASICADDR_READY_RCA4;
                basicExecAddress_[BASICADDR_KEY_VT_RESTART] = BASICADDR_VT_RESTART_RCA;
                basicExecAddress_[BASICADDR_KEY_VT_INPUT] = BASICADDR_VT_INPUT_RCA;
                p_Main->eventEnableMemAccess(true);
            }
        }
        if (scratchpadRegister_[programCounter_] == 0x2300)
        {
            if ((mainMemory_[0x201c] == 0xd4) && (mainMemory_[0x201d] == 0x96) && (mainMemory_[0x201e] == 0xb7) && (mainMemory_[0x206f] == 0x05))
            {
                chip8mainLoop_ = 0x201d;
                activateElfOsChip8();
            }
            if ((mainMemory_[0x201f] == 0xd4) && (mainMemory_[0x2020] == 0x96) && (mainMemory_[0x2021] == 0xb7) && (mainMemory_[0x206f] == 0x05))
            {
                chip8mainLoop_ = 0x2020;
                activateElfOsChip8();
            }
        }
    }
    if (loadedOs_ == ELFOS_4)
    {
        if (scratchpadRegister_[programCounter_] == 0x7c40)
        {
            if ((mainMemory_[0x7c00] == 0x30) && (mainMemory_[0x7c01] == 0x06) && (mainMemory_[0x7c02] == 0x0c))
            {
                Word saveStart = (mainMemory_[0x7cae] << 8) + mainMemory_[0x7caf];
                Word saveEnd = saveStart + (mainMemory_[0x7cb0] << 8) + mainMemory_[0x7cb1];
                Word saveExec = (mainMemory_[0x7cb2] << 8) + mainMemory_[0x7cb3];
                p_Main->eventSetLocation(true, saveStart, saveEnd, saveExec);
            }
        }
        if (scratchpadRegister_[programCounter_] == 0)
        {
            if ((mainMemory_[0x2202] == 0xc0) && (mainMemory_[0x2203] == 0x28) && (mainMemory_[0x2204] == 0x65) && (mainMemory_[0x226f] == 0x52))
            {
                loadedProgram_ = RCABASIC3;
                basicExecAddress_[BASICADDR_READY] = BASICADDR_READY_RCA3;
                basicExecAddress_[BASICADDR_KEY_VT_RESTART] = BASICADDR_VT_RESTART_RCA;
                basicExecAddress_[BASICADDR_KEY_VT_INPUT] = BASICADDR_VT_INPUT_RCA;
                p_Main->eventEnableMemAccess(true);
            }
            if ((mainMemory_[0x2202] == 0xc0) && (mainMemory_[0x2203] == 0x28) && (mainMemory_[0x2204] == 0x65) && (mainMemory_[0x226f] == 0x55))
            {
                loadedProgram_ = RCABASIC4;
                basicExecAddress_[BASICADDR_READY] = BASICADDR_READY_RCA4;
                basicExecAddress_[BASICADDR_KEY_VT_RESTART] = BASICADDR_VT_RESTART_RCA;
                basicExecAddress_[BASICADDR_KEY_VT_INPUT] = BASICADDR_VT_INPUT_RCA;
                p_Main->eventEnableMemAccess(true);
            }
        }
        if (scratchpadRegister_[programCounter_] == 0x2300)
        {
            if ((mainMemory_[0x201c] == 0xd4) && (mainMemory_[0x201d] == 0x96) && (mainMemory_[0x201e] == 0xb7) && (mainMemory_[0x206f] == 0x05))
            {
                chip8mainLoop_ = 0x201d;
                activateElfOsChip8();
            }
            if ((mainMemory_[0x201f] == 0xd4) && (mainMemory_[0x2020] == 0x96) && (mainMemory_[0x2021] == 0xb7) && (mainMemory_[0x206f] == 0x05))
            {
                chip8mainLoop_ = 0x2020;
                activateElfOsChip8();
            }
        }
    }

    switch (loadedProgram_)
    {
        case NOPROGRAM:
            switch (scratchpadRegister_[programCounter_])
            {
                case 0x13D5:        // START CHUCKs MONITOR
                case 0xfc0b:        // START KEY INPUT ELFOS
                case 0x8000:        // START CHUCKs MONITOR
                    checkLoadedSoftware();
                break;
            }
            if (scratchpadRegister_[programCounter_] == p_Computer->getBasicExecAddr(BASICADDR_READY))
                checkLoadedSoftware();
        break;
 
        case RCABASIC4:
            switch (scratchpadRegister_[programCounter_])
            {
                case BASICADDR_READY_RCA4:
                    elfRunState_ = BASICSTATE;
                break;

                case BASICADDR_RUN_RCA4:
//                case BASICADDR_CALL_RCA4:
                    elfRunState_ = RUNSTATE;
                break;

        break;

        case GOLDMON:
            switch (scratchpadRegister_[programCounter_])
            {
                case 0xc5d3:    // SAVE ASCII MON
                case 0xc6c3:    // LOAD ASCII MON
                case 0xD237:    // SAVE
                case 0xD2cd:    // SAVE
                    p_Main->stopCassette();
                break;

                case 0xc571:    // SAVE ASCII MON
                case 0xd20c:    // SAVE
                    p_Main->startCassetteSave(0);
                break;

                case 0xc643:    // LOAD ASCII MON
                case 0xd28d:    // LOAD GOLD
                    p_Main->setSwName ("");
                    p_Main->eventUpdateTitle();
                    p_Main->startCassetteLoad(0);
                break;
            }
        break;

        case MONITOR_CHUCK_LOW:
            switch (scratchpadRegister_[programCounter_])
            {
                case 0xf4e:        // LOAD L
                    p_Main->startAutoTerminalLoad(TERM_HEX);
                break;

                case 0x1300:    // LOAD I
                    p_Main->startAutoTerminalLoad(TERM_HEX);
                break;
                    
                case 0x102D:    // SAVE S
                    if (scratchpadRegister_[7]==0x2fd)
                        p_Main->startAutoTerminalSave(TERM_HEX);
                break;
                    
                case 0x11EA:    // SAVE C
                    if (scratchpadRegister_[7]==0x2fd)
                        p_Main->startAutoTerminalSave(TERM_HEX);
                break;

                case 0x10C5:    // STOP SAVE S & C
                    p_Main->stopAutoTerminal();
                break;

            }
        break;

        case MONITOR_CHUCK_HIGH:
            switch (scratchpadRegister_[programCounter_])
            {
                case 0x8f4e:    // LOAD L
                    p_Main->startAutoTerminalLoad(TERM_HEX);
                break;

                case 0x9300:    // LOAD I
                    p_Main->startAutoTerminalLoad(TERM_HEX);
                break;
                    
                case 0x902D:    // SAVE S
                    if (scratchpadRegister_[7]==0x2fd)
                        p_Main->startAutoTerminalSave(TERM_HEX);
                break;

                case 0x91EA:    // SAVE C
                    if (scratchpadRegister_[7]==0x2fd)
                        p_Main->startAutoTerminalSave(TERM_HEX);
                break;
                    
                case 0x90C5:    // STOP SAVE S & C
                    p_Main->stopAutoTerminal();
                break;
            }
        break;
    }
}
*/

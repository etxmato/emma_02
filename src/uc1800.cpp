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
#include "pushbutton.h"

#include "uc1800.h"

Uc1800Screen::Uc1800Screen(wxWindow *parent, const wxSize& size)
: Panel(parent, size)
{
}

Uc1800Screen::~Uc1800Screen()
{
    delete mainBitmapPointer;

    delete runSwitchButton;
    delete loadSwitchButton;
    delete inSwitchButton;
    delete resetSwitchButton;
    delete stepSwitchButton;
    delete powerSwitchButton;

    for (int i=0; i<2; i++)
    {
        delete dataTil313PointerItalic[i];
        delete addressTil313PointerItalic[i+2];
        delete ledPointer[i];
    }
    delete readyLedPointer;
     delete qLedPointer;

#if defined (__WXMAC__)
     for (int i = 0; i<16; i++)
        delete osx_buttonPointer[i];
#else
    for (int i = 0; i<16; i++)
        delete buttonPointer[i];
#endif
}

void Uc1800Screen::init(bool powerButtonState)
{
    wxString buttonText;
    int x, y;

    numberOfTil313_ = 2;

    keyStart_ = 0;
    keyEnd_ = 0;
    lastKey_ = 0;

    wxClientDC dc(this);

    runSwitchButton = new SwitchButton(dc, PUSH_BUTTON_BLACK, wxColour(43, 71, 106), BUTTON_UP, 160, 114, "");
    loadSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(43, 71, 106), BUTTON_UP, 85, 188, "");
    inSwitchButton = new SwitchButton(dc, PUSH_BUTTON_BLACK, wxColour(43, 71, 106), BUTTON_UP, 160, 188, "");
    resetSwitchButton = new SwitchButton(dc, PUSH_BUTTON, wxColour(43, 71, 106), BUTTON_UP, 25, 114, "");
    stepSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(43, 71, 106), BUTTON_DOWN, 85, 114, "");
    powerSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(43, 71, 106), powerButtonState, 25, 188, "");

    for (int i=0; i<2; i++)
    {
        dataTil313PointerItalic[i] = new Til313Italic(false);
        dataTil313PointerItalic[i]->init(dc, 330+i*24, 18);
        updateDataTil313Italic_ = true;

        addressTil313PointerItalic[i+2] = new Til313Italic(false);
        addressTil313PointerItalic[i+2]->init(dc, 200+i*24, 18);
        updateAddressTil313Italic_ = true;

        ledPointer[i] = new Led(dc, 347+23*(1-i), 110, ELFLED);
        updateLed_[i]=true;
    }

    readyLedPointer = new Led(dc, 32, 245, ELFLED);
    updateReadyLed_ = true;

    qLedPointer = new Led(dc, 116, 44, ELFLED);

    mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/uc1800.png", wxBITMAP_TYPE_PNG);

    int xPos = 300;
    int yPos = 80;
    int xSize = 30;
    int ySize = 15;
#if defined(__WXMSW__)
    int spacing = 12;
#endif
#if defined(__linux__)
    ySize = 28;
    yPos = 46;
    int spacing = 4;
#endif

#if defined (__WXMAC__)
    for (int i = 0; i<16; i++)
    {
        buttonText.Printf("%01X", i);
        x = xPos + (i & 0x3)*(xSize + 2);
        y = (yPos + 3 * (ySize + 2)) + (int)(i / 4 * (ySize + 12));
        osx_buttonPointer[i] = new HexButton(dc, UC1800_HEX_BUTTON, x, y, buttonText);
    }
#else
    for (int i = 0; i<16; i++)
    {
        buttonText.Printf("%01X", i);
        x = xPos + (i & 0x3)*(xSize + 2);
        y = (yPos + 3 * (ySize + 2)) + (int)(i / 4 * (ySize + spacing));
        buttonPointer[i] = new PushButton(this, i, buttonText, wxPoint(x, y), wxSize(xSize, ySize), 0);
    }
#endif

    this->connectKeyEvent(this);
}

void Uc1800Screen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.DrawBitmap(*mainBitmapPointer, 0, 0);

#if defined(__WXMAC__)
    rePaintLeds(dc);
#endif

    for (int i=0; i<2; i++)
    {
        dataTil313PointerItalic[i]->onPaint(dc);
        addressTil313PointerItalic[i+2]->onPaint(dc);
        ledPointer[i]->onPaint(dc);
    }

    readyLedPointer->onPaint(dc);
      qLedPointer->onPaint(dc);
  
    runSwitchButton->onPaint(dc);
    loadSwitchButton->onPaint(dc);
    inSwitchButton->onPaint(dc);
    resetSwitchButton->onPaint(dc);
    stepSwitchButton->onPaint(dc);
    powerSwitchButton->onPaint(dc);

#if defined (__WXMAC__)
    for (int i = 0; i<16; i++)
        osx_buttonPointer[i]->onPaint(dc);
#endif
}

void Uc1800Screen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);

    wxClientDC dc(this);

    if (runSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onRunButtonRelease();
    if (loadSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onLoadButton();
    if (inSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onInButtonRelease();
    if (resetSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onResetButtonRelease();
    if (stepSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onSingleStep();
    if (powerSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onPowerButton();

#if defined (__WXMAC__)
    for (int i = 0; i<16; i++)
        if (osx_buttonPointer[i]->onMouseRelease(dc, x, y))
            p_Computer->onNumberKeyUp();
#endif
}

void Uc1800Screen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
    if (runSwitchButton->onMousePress(dc, x, y))
        p_Computer->onRunButtonPress();
    if (inSwitchButton->onMousePress(dc, x, y))
        p_Computer->onInButtonPress(p_Computer->getData());
    if (resetSwitchButton->onMousePress(dc, x, y))
        p_Computer->onResetButtonPress();
    if (powerSwitchButton->onMousePress(dc, x, y))
        p_Computer->onPowerButton();

#if defined (__WXMAC__)
    for (int i = 0; i<16; i++)
    {
        if (osx_buttonPointer[i]->onMousePress(dc, x, y))
        {
            p_Computer->onNumberKeyDown(i);
        }
    }
#endif
} 

void Uc1800Screen::releaseButtonOnScreen(HexButton* buttonPoint)
{
    wxClientDC dc(this);

    buttonPoint->releaseButtonOnScreen(dc);
}

BEGIN_EVENT_TABLE(Uc1800, wxFrame)
    EVT_CLOSE (Uc1800::onClose)
    EVT_COMMAND(0, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(1, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(2, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(3, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(4, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(5, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(6, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(7, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(8, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(9, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(10, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(11, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(12, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(13, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(14, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(15, wxEVT_ButtonDownEvent, Uc1800::onNumberKeyDown)
    EVT_COMMAND(0, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(1, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(2, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(3, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(4, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(5, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(6, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(7, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(8, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(9, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(10, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(11, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(12, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(13, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(14, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
    EVT_COMMAND(15, wxEVT_ButtonUpEvent, Uc1800::onNumberKeyUp)
END_EVENT_TABLE()

Uc1800::Uc1800(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf, Conf computerConf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    computerConfiguration = computerConf;
    uc1800Configuration = conf;
    uc1800ClockSpeed_ = clock;

#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    this->SetClientSize(size);

    powerButtonState_ = p_Main->getConfigBool("UC1800/PowerButtonState", true);

    uc1800ScreenPointer = new Uc1800Screen(this, size);
    uc1800ScreenPointer->init(powerButtonState_);
    setMsValue_ = (int) p_Main->getLedTimeMs(UC1800);
}

Uc1800::~Uc1800()
{
    resumeComputer();
    p_Main->setMainPos(UC1800, GetPosition());

    delete uc1800ScreenPointer;
}

void Uc1800::onClose(wxCloseEvent&WXUNUSED(event) )
{
    resumeComputer();
    p_Main->stopComputer();
}

void Uc1800::resumeComputer()
{
    if (!powerButtonState_)
    {
        threadPointer->Resume();
        powerButtonState_ = true;
    }
}

void Uc1800::onRunButtonPress()
{
    ef1State_ = 0;
    onRun();
    uc1800ScreenPointer->runSetState(BUTTON_DOWN);
}

void Uc1800::onRunButtonRelease()
{
    ef1State_ = 1;
    uc1800ScreenPointer->runSetState(BUTTON_UP);
}

void Uc1800::onRun() 
{
    if (singleStateStep_)
        return;
    
    if (cpuMode_ != RUN)
        resetEffectiveClock();

    if (cpuMode_ != LOAD)
    {
        setClear(1);
        setWait(1);
        p_Main->eventUpdateTitle();
    }
}

void Uc1800::onSingleStep()
{
    singleStateStep_ = !singleStateStep_;
//    if (singleStateStep_ && loadButtonState_ == 1)
//        setLedMs(0);
//    else
//        setLedMs(setMsValue_);
}

void Uc1800::onPowerButton()
{
    powerButtonState_ = !powerButtonState_;
    p_Main->setConfigBool("UC1800/PowerButtonState", powerButtonState_);
    if (powerButtonState_)
    {
        loadRam();
        powerLoad();
    }
    else
    {
        powerSave();
        saveRam();
    }
}

void Uc1800::powerSave()
{
    threadPointer->Pause();
    uc1800ScreenPointer->setLed(0, 0);
    uc1800ScreenPointer->setLed(1, 0);
    uc1800ScreenPointer->setReadyLed(1);
    uc1800ScreenPointer->turnOff313Italic(true);
    uc1800ScreenPointer->ledTimeout();
}

void Uc1800::powerLoad()
{
    threadPointer->Resume();
    uc1800ScreenPointer->turnOff313Italic(false);
    initComputer();
    setClear(0);
    setWait(1);
    uc1800ScreenPointer->ledTimeout();
}

void Uc1800::saveRam()
{
    Byte value;
    wxFile outputFile;
    wxString fileName;

    fileName = p_Main->getMainDir()+"ramdump.bin";

    if (wxFile::Exists(fileName))
        outputFile.Open(fileName, wxFile::write);
    else
        outputFile.Create(fileName);
    for (long address = 0; address <= 0xff; address++)
    {
        value = mainMemory_[address];
        outputFile.Write(&value, 1);
    }
    outputFile.Close();
}

void Uc1800::loadRam()
{
    wxFFile inFile;
    size_t length;
    char buffer[0x3ff];

    wxString dir = p_Main->getMainDir();
    if (wxFile::Exists(p_Main->getMainDir()+"ramdump.bin"))
    {
        if (inFile.Open(p_Main->getMainDir()+"ramdump.bin", _("rb")))
        {
            length = inFile.Read(buffer, 0x100);
            for (size_t i=0; i<length; i++)
                mainMemory_[i] = (Byte)buffer[i];
            inFile.Close();
        }
    }
}

void Uc1800::autoBoot()
{
    setClear(1);
}

bool Uc1800::keyDownPressed(int key)
{
    if (key == inKey2_)
    {
        onRunButtonPress();
        return true;
    }

    onHexKeyDown(key);
    return false;
}

bool Uc1800::keyUpReleased(int key, wxKeyEvent&WXUNUSED(event))
{
    if (key == inKey1_)
    {
        onInButtonRelease();
        return true;
    }
    if (key == inKey2_)
    {
        onRunButtonRelease();
        return true;
    }
    onHexKeyUp(key);
    return false;
}
void Uc1800::onInButtonPress()
{
    onInButtonPress(getData());
}

void Uc1800::onInButtonPress(Byte value)
{
    if (cpuMode_ == LOAD)
    {
        if (singleStateStep_)
        {
            singleStateStep_ = false;
            dmaIn(value);
            showData(value);
            singleStateStep_ = true;
        }
        else
        {
            dmaIn(value);
            showData(value);
        }
    }
    else
    {
        if (singleStateStep_)
        {
            setClear(1);
            setWait(1);
            p_Main->eventUpdateTitle();
        }
    }
    uc1800ScreenPointer->inSetState(BUTTON_DOWN);
}

void Uc1800::onInButtonRelease()
{
    uc1800ScreenPointer->inSetState(BUTTON_UP);
}

void Uc1800::onLoadButton()
{
    if (!loadButtonState_)
    {
        loadButtonState_ = 1;
        if (singleStateStep_)
            setLedMs(0);
    }
    else
    {
        loadButtonState_ = 0;
        if (singleStateStep_)
            setLedMs(setMsValue_);
    }
    setWait(loadButtonState_);
}

void Uc1800::onResetButtonPress()
{
    setClear(0);
    setWait(1);

    if (cpuMode_ == RESET)
    {
        uc1800ScreenPointer->showAddressTil313Italic(0);
    }

    uc1800ScreenPointer->resetSetState(BUTTON_DOWN);
}

void Uc1800::onResetButtonRelease()
{
    uc1800ScreenPointer->resetSetState(BUTTON_UP);
}

Byte Uc1800::getData()
{
    return keypadValue_;
}

void Uc1800::configureComputer()
{
    wxString printBuffer;

    inType_[1] = UC1800IN;
    outType_[1] = UC1800OUT;
    efType_[1] = UC1800EF;
    setCycleType(COMPUTERCYCLE, LEDCYCLE);
    
    p_Main->message("Configuring UC1800");
    p_Main->message("    Output 1: display output, input 1: data input");
    p_Main->message("    EF 1: 0 when start button pressed");
    p_Main->message("");

    inKey1_ = p_Main->getDefaultInKey1("UC1800");
    inKey2_ = p_Main->getDefaultInKey2("UC1800", 313);
    p_Main->getDefaultHexKeys(UC1800, "UC1800", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);

    resetCpu();
}

void Uc1800::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefA1_[i] = hexKeyDefA1[i];
        keyDefA2_[i] = hexKeyDefA2[i];
    }
}

void Uc1800::initComputer()
{
    Show(true);

    loadButtonState_ = 1;

    ef1State_ = 1;
    ef3State_ = 1;

    uc1800ScreenPointer->setLed(1, 1);
    uc1800ScreenPointer->setLed(0, 0);
    uc1800ScreenPointer->setReadyLed(0);

    keypadValue_ = 0;
    nextNybble_ = 'H';
}

Byte Uc1800::ef(int flag)
{
    switch(efType_[flag])
    {
        case 0:
            return 1;
        break;

        case UC1800EF:
            return ef1State_;
        break;

        default:
            return 1;
    }
}

Byte Uc1800::in(Byte port, Word WXUNUSED(address))
{
    Byte ret;
    ret = 0;

    switch(inType_[port])
    {
        case 0:
            ret = 255;
        break;

        case UC1800IN:
            ret = getData();
        break;

        default:
            ret = 255;
    }
    inValues_[port] = ret;
    return ret;
}

void Uc1800::out(Byte port, Word WXUNUSED(address), Byte value)
{
    outValues_[port] = value;

    switch(outType_[port])
    {
        case 0:
            return;
        break;

        case UC1800OUT:
            showData(value);
        break;
    }
}

void Uc1800::switchQ(int value)
{
    uc1800ScreenPointer->setQLed(value);
}

void Uc1800::showData(Byte val)
{
    uc1800ScreenPointer->showDataTil313Italic(val);
}

void Uc1800::showCycleData(Byte val)
{
    if (singleStateStep_)
        showData(val);
}

void Uc1800::cycle(int type)
{
    switch(cycleType_[type])
    {
        case 0:
            return;
        break;

        case LEDCYCLE:
            cycleLed();
        break;
    }
}

void Uc1800::cycleLed()
{
    ledCycleValue_ --;
    if (ledCycleValue_ <= 0)
    {
        ledCycleValue_ = ledCycleSize_;
        uc1800ScreenPointer->ledTimeout();
    }
}

void Uc1800::onNumberKeyDown(wxCommandEvent&event)
{
    int i = event.GetId();
    onNumberKeyDown(i);
}

void Uc1800::onNumberKeyDown(int i)
{
    ef3State_ = 0;
    keypadValue_= (nextNybble_ == 'H')?(keypadValue_&15)+(i<<4):(keypadValue_&0xf0)+i;
    if (nextNybble_ == 'H')
    {
        nextNybble_ = 'L';
        uc1800ScreenPointer->setLed(1, 0);
        uc1800ScreenPointer->setLed(0, 1);
    }
    else
    {
        nextNybble_ = 'H';
        uc1800ScreenPointer->setLed(1, 1);
        uc1800ScreenPointer->setLed(0, 0);
    }
}

void Uc1800::onNumberKeyUp(wxCommandEvent&WXUNUSED(event))
{
    ef3State_ = 1;
}

void Uc1800::onNumberKeyUp()
{
    ef3State_ = 1;
}

void Uc1800::onHexKeyDown(int keycode)
{
    if (ef3State_ == 0) // This is to avoid multiple key presses
        return;
    
    for (int i=0; i<16; i++)
    {
        if (keycode == keyDefA1_[i])
            onNumberKeyDown(i);
        if (keycode == keyDefA2_[i])
             onNumberKeyDown(i);
    }
}

void Uc1800::onHexKeyUp(int WXUNUSED(keycode))
{
    ef3State_ = 1;
}

void Uc1800::startComputer()
{
    resetPressed_ = false;

    defineMemoryType(0, 0xff, RAM);
    defineMemoryType(0x100, 0xffff, MAPPEDRAM);
    initRam(0, 0xff);

    p_Main->assDefault("mycode", 0, 0xA0);

    readProgram(p_Main->getRamDir(UC1800), p_Main->getRamFile(UC1800), RAM, 0, NONAME);

    if (uc1800Configuration.autoBoot)
    {
        scratchpadRegister_[0]=p_Main->getBootAddress("UC1800", UC1800);
        autoBoot();
    }

    p_Main->setSwName("");
    p_Main->updateTitle();

    address_ = 0;
    uc1800ScreenPointer->showAddressTil313Italic(address_);

    cpuCycles_ = 0;
    instructionCounter_= 0;
    p_Main->startTime();

    int ms = (int) p_Main->getLedTimeMs(UC1800);
    uc1800ScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((uc1800ClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
    
    threadPointer->Run();

    if (!powerButtonState_)
        powerSave();
}

void Uc1800::writeMemDataType(Word address, Byte type)
{
    switch (memoryType_[address/256]&0xff)
    {
        case RAM:
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            increaseExecutedMainMemory(address, type);
        break;
 
        case MAPPEDRAM:
            if (mainMemoryDataType_[address & 0xff] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address & 0xff] = type;
            }
            increaseExecutedMainMemory(address & 0xff, type);
        break;
    }
}

Byte Uc1800::readMemDataType(Word address, uint64_t* executed)
{
    switch (memoryType_[address/256]&0xff)
    {
        case RAM:
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;
            
        case MAPPEDRAM:
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address & 0xff];
            return mainMemoryDataType_[address & 0xff];
        break;
    }
    return mainMemoryDataType_[address];
}

Byte Uc1800::readMem(Word address)
{
    address_ = address;

    uc1800ScreenPointer->showAddressTil313Italic(address_);

    return readMemDebug(address_);
}

Byte Uc1800::readMemDebug(Word address)
{
    switch (memoryType_[address / 256]&0xff)
    {
        case UNDEFINED:
            return 255;
        break;

        case RAM:
            return mainMemory_[address];
        break;
            
        case MAPPEDRAM:
            return mainMemory_[address & 0xff];
        break;

        default:
            return 255;
        break;
    }
}

void Uc1800::writeMem(Word address, Byte value, bool writeRom)
{
    address_ = address;

    uc1800ScreenPointer->showAddressTil313Italic(address_);

    writeMemDebug(address_, value, writeRom);
}

void Uc1800::writeMemDebug(Word address, Byte value, bool writeRom)
{
    switch (memoryType_[address/256]&0xff)
    {
        case UNDEFINED:
            if (writeRom)
                mainMemory_[address]=value;
        break;

        case MAPPEDRAM:
            address &= 0xff;
            if (mainMemory_[address]==value)
                return;
            mainMemory_[address]=value;
            if (address >= memoryStart_ && address<(memoryStart_ +256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;

        case RAM:
            if (mainMemory_[address]==value)
                return;
            mainMemory_[address]=value;
            if (address >= memoryStart_ && address<(memoryStart_  +256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;
    }
}

void Uc1800::cpuInstruction()
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
        }
    }
}

void Uc1800::resetPressed()
{
    resetCpu();
    initComputer();
    if (uc1800Configuration.autoBoot)
    {
        scratchpadRegister_[0]=p_Main->getBootAddress("UC1800", UC1800);
        autoBoot();
    }
    
    resetPressed_ = false;
    p_Main->setSwName("");
    p_Main->eventUpdateTitle();
}

void Uc1800::onReset()
{
    resetPressed_ = true;
}

void Uc1800::setLedMs(long ms)
{
    uc1800ScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((uc1800ClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

void Uc1800::activateMainWindow()
{
    bool maximize = IsMaximized();
    Iconize(false);
    Raise();
    Show(true);
    Maximize(maximize);
}

void Uc1800::releaseButtonOnScreen(HexButton* buttonPointer, int WXUNUSED(buttonType))
{
    uc1800ScreenPointer->releaseButtonOnScreen(buttonPointer);
}

void Uc1800::refreshPanel()
{
    uc1800ScreenPointer->refreshPanel();
}

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

#include "microtutor2.h"

Microtutor2Screen::Microtutor2Screen(wxWindow *parent, const wxSize& size, int tilType)
: Panel(parent, size, tilType)
{
}

Microtutor2Screen::~Microtutor2Screen()
{
    delete mainBitmapPointer;

    delete runSwitchButton;
    delete loadSwitchButton;
    delete inSwitchButton;
    delete mpSwitchButton;
    
    for (int i=0; i<8; i++)
    {
        delete dataSwitchButton[i];
    }
    for (int i=0; i<2; i++)
    {
        delete dataPointer[i];
    }
}

void Microtutor2Screen::init()
{
    keyStart_ = 0;
    keyEnd_ = 0;
    lastKey_ = 0;

    wxClientDC dc(this);

    runSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(43, 71, 106), BUTTON_DOWN, 284, 30, "");
    loadSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(43, 71, 106), BUTTON_DOWN, 62, 30, "");
    inSwitchButton = new SwitchButton(dc, PUSH_BUTTON_BLACK, wxColour(43, 71, 106), BUTTON_UP, 25, 30, "");
    mpSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(43, 71, 106), BUTTON_DOWN, 247, 30, "");

    
    for (int i=0; i<8; i++)
        dataSwitchButton[i] = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(43, 71, 106), BUTTON_DOWN, 25+37*(7-i), 100, "");
    
    for (int i=0; i<2; i++)
    {
        dataPointer[i] = new Til313Italic(true);
        dataPointer[i]->init(dc, 139+i*28, 30);
    }

    mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/microtutor2.png", wxBITMAP_TYPE_PNG);
    
    this->connectKeyEvent(this);
}

void Microtutor2Screen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.DrawBitmap(*mainBitmapPointer, 0, 0);

#if defined(__WXMAC__)
    rePaintLeds(dc);
#endif
    
    for (int i=0; i<2; i++)
    {
        dataPointer[i]->onPaint(dc);
    }
    
    runSwitchButton->onPaint(dc);
    loadSwitchButton->onPaint(dc);
    inSwitchButton->onPaint(dc);
    mpSwitchButton->onPaint(dc);

    for (int i=0; i<8; i++)
    {
        dataSwitchButton[i]->onPaint(dc);
    }
}

void Microtutor2Screen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);

    wxClientDC dc(this);

    if (runSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onRunButton();
    if (loadSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onLoadButton();
    if (inSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onInButtonRelease();
    if (mpSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onMpButton();

    for (int i=0; i<8; i++)
    {
        if (dataSwitchButton[i]->onMouseRelease(dc, x, y))
            p_Computer->dataSwitch(i);
    }
}

void Microtutor2Screen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
    if (inSwitchButton->onMousePress(dc, x, y))
        p_Computer->onInButtonPress(p_Computer->getData());
}

BEGIN_EVENT_TABLE(Microtutor2, wxFrame)
    EVT_CLOSE (Microtutor2::onClose)
END_EVENT_TABLE()

Microtutor2::Microtutor2(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf, Conf computerConf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    computerConfiguration = computerConf;
    microtutorConfiguration = conf;
    microtutorClockSpeed_ = clock;

#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    this->SetClientSize(size);

    microtutorScreenPointer = new Microtutor2Screen(this, size, TIL313ITALIC);
    microtutorScreenPointer->init();
}

Microtutor2::~Microtutor2()
{
    p_Main->setMainPos(MICROTUTOR2, GetPosition());

    delete microtutorScreenPointer;
}

void Microtutor2::onClose(wxCloseEvent&WXUNUSED(event) )
{
    p_Main->stopComputer();
}

bool Microtutor2::keyUpReleased(int key, wxKeyEvent&WXUNUSED(event))
{
    if (key == inKey1_ || key == inKey2_)
    {
        onInButtonRelease();
        return true;
    }
    return false;
}

void Microtutor2::onRunButton()
{
    onRun();
}

void Microtutor2::onRun()
{
    if (microtutorConfiguration.utilityMemory)
        bootstrap_ = 0x8000;
    
    if (runButtonState_)
    {
        microtutorScreenPointer->runSetState(BUTTON_DOWN);
        runButtonState_ = 0;
    }
    else
    {
        microtutorScreenPointer->runSetState(BUTTON_UP);
        runButtonState_ = 1;
        p_Main->startTime();
    }
    setClear(runButtonState_);
    p_Main->eventUpdateTitle();
}

void Microtutor2::autoBoot()
{
    microtutorScreenPointer->runSetState(BUTTON_UP);
    runButtonState_ = 1;
    setClear(runButtonState_);
}

Byte Microtutor2::inPressed()
{
    if (inPressed_ == true)
        return 0;
    else
        return 1;
}

void Microtutor2::onInButtonPress(Byte value)
{
    if (loadButtonState_ == 1)
        inPressed_ = true;
    if (getCpuMode() == LOAD)
    {
        dmaIn(value);
        showData(value);
    }
    microtutorScreenPointer->inSetState(BUTTON_DOWN);
}

void Microtutor2::onInButtonRelease()
{
    if (loadButtonState_ == 1)
        inPressed_ = false;
//    showData(readMem((scratchpadRegister_[0]-1)&0xffff));
    microtutorScreenPointer->inSetState(BUTTON_UP);
}

void Microtutor2::onLoadButton()
{
    if (!loadButtonState_)
    {
        loadButtonState_ = 1;
    }
    else
    {
        loadButtonState_ = 0;
        resetCpu();
    }
    setWait(loadButtonState_);
}

void Microtutor2::onMpButton()
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

void Microtutor2::dataSwitch(int i)
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

Byte Microtutor2::getData()
{
    return(dataSwitchState_[7]?128:0) +(dataSwitchState_[6]?64:0) +(dataSwitchState_[5]?32:0) +(dataSwitchState_[4]?16:0) +(dataSwitchState_[3]?8:0) +(dataSwitchState_[2]?4:0) +(dataSwitchState_[1]?2:0) +(dataSwitchState_[0]?1:0);
}

void Microtutor2::configureComputer()
{
    inType_[0][0][4] = MICROTUTORIN;
    outType_[0][0][4] = MICROTUTOROUT;
    efType_[0][0][4] = MICROTUTOREF;
    setCycleType(COMPUTERCYCLE, LEDCYCLE);
    
    p_Main->message("Configuring Microtutor II");
    p_Main->message("    Output 4: display output, input 4: data input");
    if (microtutorConfiguration.utilityMemory)
        p_Main->message("    Output 4 & input 4: switch in E slot");
    p_Main->message("    EF 4: 0 when in button pressed");
    p_Main->message("");

    inKey1_ = p_Main->getDefaultInKey1("Microtutor2");
    inKey2_ = p_Main->getDefaultInKey2("Microtutor2");

    resetCpu();
}

void Microtutor2::initComputer()
{
    Show(true);

    loadButtonState_ = 1;
    mpButtonState_ = 0;
    runButtonState_ = 0;

    for (int i=0; i<8; i++)  dataSwitchState_[i]=0;
    
    switches_ = 0;
    inPressed_ = false;
}

Byte Microtutor2::ef(int flag)
{
    switch(efType_[0][0][flag])
    {
        case 0:
            return 1;
        break;

        case MICROTUTOREF:
            return inPressed();
        break;

        default:
            return 1;
    }
}

Byte Microtutor2::in(Byte port, Word WXUNUSED(address))
{
    Byte ret;
    ret = 0;

    switch(inType_[0][0][port])
    {
        case 0:
            ret = 255;
        break;

        case MICROTUTORIN:
            inPressed_ = false;
            ret = getData();
            bootstrap_ = 0;
        break;

        default:
            ret = 255;
    }
    inValues_[port] = ret;
    return ret;
}

void Microtutor2::out(Byte port, Word WXUNUSED(address), Byte value)
{
    outValues_[port] = value;

    switch(outType_[0][0][port])
    {
        case 0:
            return;
        break;

        case MICROTUTOROUT:
            inPressed_ = false;
            showData(value);
            bootstrap_ = 0;
        break;
    }
}

void Microtutor2::showData(Byte val)
{
    microtutorScreenPointer->showData(val);
}

void Microtutor2::switchQ(int value)
{
    microtutorScreenPointer->showDp313Italic(value == 1);
}

void Microtutor2::cycle(int type)
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

void Microtutor2::cycleLed()
{
    ledCycleValue_ --;
    if (ledCycleValue_ <= 0)
    {
        ledCycleValue_ = ledCycleSize_;
        microtutorScreenPointer->ledTimeout();
    }
}

void Microtutor2::startComputer()
{
    resetPressed_ = false;
   
    ramMask_ = ((1<<p_Main->getRamType(computerType_))<<8)-1;
    
    bootstrap_ = 0;
    if (microtutorConfiguration.utilityMemory)
    {
        defineMemoryType(0, ramMask_, RAM);
        defineMemoryType(ramMask_+1, 0x7fff, MAPPEDRAM);
        initRam(0, ramMask_);

        ramMask_ += 0x8000;
        defineMemoryType(0x8000, ramMask_, RAM);
        defineMemoryType(ramMask_+1, 0xffff, MAPPEDRAM);
        initRam(0x8000, ramMask_);
        
        readProgram(p_Main->getRomDir(MICROTUTOR2, MAINROM1), p_Main->getRomFile(MICROTUTOR2, MAINROM1), RAM, 0x8000, NONAME);
        readProgram(p_Main->getRomDir(MICROTUTOR2, MAINROM2), p_Main->getRomFile(MICROTUTOR2, MAINROM2), RAM, 0, NONAME);
        bootstrap_ = 0x8000;
    }
    else
    {
        defineMemoryType(0, ramMask_, RAM);
        defineMemoryType(ramMask_+1, 0xffff, MAPPEDRAM);
        initRam(0, ramMask_);
        readProgram(p_Main->getRomDir(MICROTUTOR2, MAINROM1), p_Main->getRomFile(MICROTUTOR2, MAINROM1), RAM, 0, NONAME);
    }

    p_Main->assDefault("mycode", 0, ramMask_);


    if (microtutorConfiguration.autoBoot)
    {
        scratchpadRegister_[0]=p_Main->getBootAddress("Microtutor2", MICROTUTOR2);
        autoBoot();
    }

    p_Main->setSwName("");
    p_Main->updateTitle();

    cpuCycles_ = 0;
    instructionCounter_= 0;
    p_Main->startTime();

    int ms = (int) p_Main->getLedTimeMs(MICROTUTOR2);
    microtutorScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((microtutorClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
    
    threadPointer->Run();
}

void Microtutor2::writeMemDataType(Word address, Byte type)
{
    switch (memoryType_[address/256]&0xff)
    {
        case RAM:
            address = address | bootstrap_;
            if (((address&0x8000) == 0x8000 && mpButtonState_ == 1) || (!microtutorConfiguration.utilityMemory && (mpButtonState_ == 1)))
                return;
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            increaseExecutedMainMemory(address, type);
        break;
            
        case MAPPEDRAM:
            address = (address & ramMask_) | bootstrap_;
            if (((address&0x8000) == 0x8000 && mpButtonState_ == 1) || (!microtutorConfiguration.utilityMemory && (mpButtonState_ == 1)))
                return;
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            increaseExecutedMainMemory(address, type);
        break;
    }
}

Byte Microtutor2::readMemDataType(Word address, uint64_t* executed)
{
    switch (memoryType_[address/256]&0xff)
    {
        case RAM:
            address = address | bootstrap_;
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;
            
        case MAPPEDRAM:
            address = (address & ramMask_) | bootstrap_;
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;
    }
    return mainMemoryDataType_[address];
}

Byte Microtutor2::readMem(Word address)
{
    switch (memoryType_[address / 256]&0xff)
    {
        case UNDEFINED:
            return 255;
        break;

        case RAM:
            address_ = address | bootstrap_;
            return mainMemory_[address_];
        break;
            
        case MAPPEDRAM:
            address_ = (address & ramMask_) | bootstrap_;
            return mainMemory_[address_];
        break;

        default:
            return 255;
        break;
    }
}

Byte Microtutor2::readMemDebug(Word address)
{
    switch (memoryType_[address / 256]&0xff)
    {
        case UNDEFINED:
            return 255;
        break;
            
        case RAM:
            address = address | bootstrap_;
            return mainMemory_[address];
        break;
            
        case MAPPEDRAM:
            address = (address & ramMask_) | bootstrap_;
            return mainMemory_[address];
        break;
            
        default:
            return 255;
        break;
    }
}

void Microtutor2::writeMem(Word address, Byte value, bool writeRom)
{
    switch (memoryType_[address/256]&0xff)
    {
        case UNDEFINED:
            if (writeRom)
                mainMemory_[address_]=value;
        break;

        case MAPPEDRAM:
            address_ = (address & ramMask_) | bootstrap_;
            if (((address_&0x8000) == 0x8000 && mpButtonState_ == 1) || (!microtutorConfiguration.utilityMemory && (mpButtonState_ == 1)))
                return;
            if (mainMemory_[address_]==value)
                return;
            mainMemory_[address_]=value;
            if (address_ >= memoryStart_ && address_<(memoryStart_ +256))
                p_Main->updateDebugMemory(address_);
            p_Main->updateAssTabCheck(address_);
        break;

        case RAM:
            address_ = address | bootstrap_;
            if (((address_&0x8000) == 0x8000 && mpButtonState_ == 1) || (!microtutorConfiguration.utilityMemory && (mpButtonState_ == 1)))
                return;
            if (mainMemory_[address_]==value)
                return;
            mainMemory_[address_]=value;
            if (address_ >= memoryStart_ && address_<(memoryStart_  +256))
                p_Main->updateDebugMemory(address_);
            p_Main->updateAssTabCheck(address_);
        break;
    }
}

void Microtutor2::writeMemDebug(Word address, Byte value, bool writeRom)
{
    switch (memoryType_[address/256]&0xff)
    {
        case UNDEFINED:
            if (writeRom)
                mainMemory_[address]=value;
        break;
            
        case MAPPEDRAM:
            address = (address & ramMask_) | bootstrap_;
            if (((address&0x8000) == 0x8000 && mpButtonState_ == 1) || (!microtutorConfiguration.utilityMemory && (mpButtonState_ == 1)))
                return;
            if (mainMemory_[address]==value)
                return;
            mainMemory_[address]=value;
            if (address >= memoryStart_ && address<(memoryStart_ +256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;
            
        case RAM:
            address = address | bootstrap_;
            if (((address&0x8000) == 0x8000 && mpButtonState_ == 1) || (!microtutorConfiguration.utilityMemory && (mpButtonState_ == 1)))
                return;
            if (mainMemory_[address]==value)
                return;
            mainMemory_[address]=value;
            if (address >= memoryStart_ && address<(memoryStart_  +256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;
    }
}

void Microtutor2::cpuInstruction()
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

void Microtutor2::resetPressed()
{
    resetCpu();
    initComputer();
    if (microtutorConfiguration.autoBoot)
    {
        scratchpadRegister_[0]=p_Main->getBootAddress("Microtutor II", MICROTUTOR2);
        autoBoot();
    }
    
    resetPressed_ = false;
    p_Main->setSwName("");
    p_Main->eventUpdateTitle();
}

void Microtutor2::onReset()
{
    resetPressed_ = true;
}

void Microtutor2::setLedMs(long ms)
{
    microtutorScreenPointer->setLedMs(ms);
    if (ms == 0)
        ledCycleSize_ = -1;
    else
        ledCycleSize_ = (((microtutorClockSpeed_ * 1000000) / 8) / 1000) * ms;
    ledCycleValue_ = ledCycleSize_;
}

void Microtutor2::activateMainWindow()
{
    bool maximize = IsMaximized();
    Iconize(false);
    Raise();
    Show(true);
    Maximize(maximize);
}

void Microtutor2::refreshPanel()
{
    microtutorScreenPointer->refreshPanel();
}

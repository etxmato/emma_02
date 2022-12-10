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
#include "cidelsa.h"

Cidelsa::Cidelsa(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock, Conf computerConf)
:V1870(title, pos, size, zoomLevel, computerType, clock, 0)
{
    computerConfiguration = computerConf;
}

Cidelsa::~Cidelsa()
{
    switch (cidelsaGame_)
    {
        case ALTAIR:
            p_Main->saveScrtValues("Altair");
        break;
            
        case DESTROYER2:
            p_Main->saveScrtValues("Destroyer2");
        break;
            
        case DESTROYER1:
            p_Main->saveScrtValues("Destroyer1");
        break;
            
        case DRACO:
            p_Main->saveScrtValues("Draco");
        break;
    }

    p_Main->setMainPos(CIDELSA, GetPosition());
}

bool Cidelsa::keyDownExtended(int keycode, wxKeyEvent&WXUNUSED(event))
{
    keyDown(keycode);
    return false;
}

void Cidelsa::keyDown(int keycode)
{
    switch(keycode)
    {
        case WXK_RETURN:    // Test mode
            cidEF2_ = 0;
        break;

        case 'B':        // Coin shute B
            cidEF3_ = 0;
        break;

        case 'A':        // Coin shute A
            cidEF4_ = 0;
        break;

        case '1':            // 1 Player
            if (cidelsaGame_ == DRACO)
                cid1_ |= 1;
            else
                cid1_ |= 2;
        break;

        case '2':            // 2 Players
            if (cidelsaGame_ == DRACO)
                cid1_ |= 2;
            else
                cid1_ |= 4;
        break;

        case 'T':
            if (cidelsaGame_ == DRACO)
                cid1_ |= 4;
        break;

        case WXK_LEFT:
            if (cidelsaGame_ == DRACO)
                cid4_ |= 128;
            else
                cid1_ |= 16;
        break;

        case WXK_RIGHT:
            if (cidelsaGame_ == DRACO)
                cid4_ |= 64;
            else
                cid1_ |= 8;
        break;

        case WXK_SPACE:        // Fire
            cid1_ |= 32;
        break;

        case WXK_UP:
            if (cidelsaGame_ == DRACO)
                cid4_ |= 16;
            if (cidelsaGame_ == ALTAIR)
                cid4_ |= 1;
        break;

        case WXK_DOWN:
            if (cidelsaGame_ == DRACO)
                cid4_ |= 32;
            if (cidelsaGame_ == ALTAIR)
                cid4_ |= 2;
        break;

        case WXK_INSERT:    // BUTTON2
            if (cidelsaGame_ == ALTAIR)
                cid4_ |= 4;
        break;

        case 'M':    // DOWN Player 2
        case WXK_NUMPAD2: 
        case WXK_NUMPAD_DOWN: 
            if (cidelsaGame_ == DRACO)
                cid4_ |= 2;
        break;

        case 'I':    // UP Player 2
        case WXK_NUMPAD8: 
        case WXK_NUMPAD_UP: 
            if (cidelsaGame_ == DRACO)
                cid4_ |= 1;
        break;

        case 'J':    // LEFT Player 2
        case WXK_NUMPAD4: 
        case WXK_NUMPAD_LEFT: 
            if (cidelsaGame_ == DRACO)
                cid4_ |= 8;
        break;

        case 'K':    // RIGHT Player 2
        case WXK_NUMPAD6:  
        case WXK_NUMPAD_RIGHT: 
            if (cidelsaGame_ == DRACO)
                cid4_ |= 4;
        break;
    }
}

void Cidelsa::keyUp(int keycode)
{
    switch(keycode)
    {
        case WXK_RETURN:    // Test mode
            cidEF2_ = 1;
        break;

        case 'B':        // Coin shute B
            cidEF3_ = 1;
        break;

        case 'A':        // Coin shute A
            cidEF4_ = 1;
        break;

        case '1':            // 1 Player
            if (cidelsaGame_ == DRACO)
                cid1_ &= 0xfe;
            else
                cid1_ &= 0xfd;
        break;

        case '2':            // 2 Players
            if (cidelsaGame_ == DRACO)
                cid1_ &= 0xfd;
            else
                cid1_ &= 0xfb;
        break;

        case 'T':
            if (cidelsaGame_ == DRACO)
                cid1_ &= 0xfb;
        break;

        case WXK_LEFT:
            if (cidelsaGame_ == DRACO)
                cid4_ &= 0x7f;
            else
                cid1_ &= 0xef;
        break;

        case WXK_RIGHT:
            if (cidelsaGame_ == DRACO)
                cid4_ &= 0xbf;
            else
                cid1_ &= 0xf7;
        break;

        case WXK_SPACE:        // Fire
            cid1_ &= 0xdf;
        break;

        case WXK_UP:
            if (cidelsaGame_ == DRACO)
                cid4_ &= 0xef;
            if (cidelsaGame_ == ALTAIR)
                cid4_ &= 0xfe;
        break;

        case WXK_DOWN:
            if (cidelsaGame_ == DRACO)
                cid4_ &= 0xdf;
            if (cidelsaGame_ == ALTAIR)
                cid4_ &= 0xfd;
        break;

        case WXK_INSERT:    // Button2
            if (cidelsaGame_ == ALTAIR)
                cid4_ &= 0xfb;
        break;

        case 'M':    // UP Player 2
        case WXK_NUMPAD2: 
        case WXK_NUMPAD_DOWN: 
            if (cidelsaGame_ == DRACO)
                cid4_ &= 0xfd;
        break;

        case 'I':    // DOWN Player 2
        case WXK_NUMPAD8: 
        case WXK_NUMPAD_UP: 
            if (cidelsaGame_ == DRACO)
                cid4_ &= 0xfe;
        break;

        case 'J':    // RIGHT Player 2
        case WXK_NUMPAD4: 
        case WXK_NUMPAD_LEFT: 
            if (cidelsaGame_ == DRACO)
                cid4_ &= 0xf7;
        break;

        case 'K':    // LEFT Player 2
        case WXK_NUMPAD6:  
        case WXK_NUMPAD_RIGHT: 
            if (cidelsaGame_ == DRACO)
                cid4_ &= 0xfb;
        break;
    }
}

void Cidelsa::configureComputer()
{
    inType_[1] = CIDELSAIN1;
    inType_[2] = CIDELSAIN2;
    inType_[4] = CIDELSAIN4;
    outType_[1] = CIDELSAOUT1;
    efType_[2] = BUTTON_EF2;
    efType_[3] = BUTTON_EF3;
    efType_[4] = BUTTON_EF4;

    p_Main->message("Configuring Cidelsa");
    p_Main->message("    Input 1: input A, input 2: dip settings, input 4: input B");
    p_Main->message("    Output 1: output A");
    p_Main->message("    EF 2: return(test mode), EF 3: shift(coin B), EF 4: tab(coin A)\n");

    cidEF2_ = 1;
    cidEF3_ = 1;
    cidEF4_ = 1;

    resetCpu();
}

void Cidelsa::initComputer()
{
    init1870();
    setClear(1);
    setWait(1);
}

Byte Cidelsa::ef(int flag)
{
    switch(efType_[flag])
    {
        case 0:
            return 1;
        break;

        case BUTTON_EF2:
            return ef2();
        break;

        case BUTTON_EF3:
            return ef3();
        break;

        case BUTTON_EF4:
            return ef4();
        break;

        case V1870EF:
            return ef1_1870();
        break;

        default:
            return 1;
    }
}

Byte Cidelsa::ef2()
{
    return cidEF2_;
}

Byte Cidelsa::ef3()
{
    return cidEF3_;
}

Byte Cidelsa::ef4()
{
    return cidEF4_;
}

Byte Cidelsa::in(Byte port, Word WXUNUSED(address))
{
    Byte ret;

    switch(inType_[port])
    {
        case 0:
            ret = 255;
        break;

        case CIDELSAIN1:
            ret = in1();
        break;

        case CIDELSAIN2:
            ret = in2();
        break;

        case CIDELSAIN4:
            ret = in4();
        break;

        default:
            ret = 255;
    }
    inValues_[port] = ret;
    return ret;
}

Byte Cidelsa::in1()
{
    return cid1_^0xff;
}

Byte Cidelsa::in2()
{
    if (cidelsaGame_ == DRACO)
        return cid2Draco_;
    else
        return cid2_;
}

Byte Cidelsa::in4()
{
    return cid4_^0xff;
}

void Cidelsa::out(Byte port, Word address, Byte value)
{
    switch(outType_[port])
    {
        case 0:
            return;
        break;

        case CIDELSAOUT1:
            outValues_[port] = value;
            out1(value);
        break;

        case V1870OUT3:
            outValues_[port] = value;
            out3_1870(value);
        break;

        case V1870OUT4:
            outValues_[port] = address;
            out4_1870(address);
        break;

        case V1870OUT5:
            outValues_[port] = address;
            out5_1870(address);
        break;

        case V1870OUT6:
            outValues_[port] = address;
            out6_1870(address);
        break;

        case V1870OUT7:
            outValues_[port] = address;
            out7_1870(address);
        break;
    }
}

void Cidelsa::out1(Byte value)
{
// Draco sound
    if (cidelsaGame_ != DRACO)
    {
        updateCidelsaLedStatus (0, (value&0x80) == 0x80);
        updateCidelsaLedStatus (1, (value&0x40) == 0x40);
        updateCidelsaLedStatus (2, (value&0x8) == 0x8);
        updateCidelsaLedStatus (3, (value&0x10) == 0x10);
        updateCidelsaLedStatus (4, (value&0x20) == 0x20);
    }
}

void Cidelsa::cycle(int type)
{
    switch(cycleType_[type])
    {
        case 0:
            return;
        break;

        case V1870CYCLE:
            cycle1870();
        break;
    }
}

void Cidelsa::startComputer()
{
    cidelsaGame_ = DRACO;
    cid2_ = p_Main->getIn2Value();
    cid2Draco_ = p_Main->getIn2ValueDraco();
    resetPressed_ = false;

    p_Main->setSwName("");
    p_Main->updateTitle();

    p_Main->checkAndReInstallMainRom(CIDELSA);
    readProgramCidelsa(p_Main->getRomDir(CIDELSA, MAINROM1), p_Main->getRomFile(CIDELSA, MAINROM1), ROM, 0, SHOWNAME);

    defineMemoryType(0xf400, 0xf7ff, CRAM1870);
    defineMemoryType(0xf800, 0xffff, PRAM1870);

    if (p_Main->getCidelsaHwConfiguration() == 0)
    {
        if (readMem(0xEc6) == 0x28 && readMem(0x1201) == 0x12 && readMem(0x1878) == 0xc3)
            cidelsaGame_ = ALTAIR;

        if (readMem(0xEc6) == 0x77 && readMem(0x1201) == 0xe0 && readMem(0x1878) == 0x52)
            cidelsaGame_ = DESTROYER1;

        if (readMem(0xEc6) == 0 && readMem(0x1201) == 0xe0 && readMem(0x1878) == 0x52)
            cidelsaGame_ = DESTROYER2;

        if (readMem(0xEc6) == 0xfb && readMem(0x1201) == 0xac && readMem(0x1878) == 0xfa)
            cidelsaGame_ = DRACO32;
    }
    else
        cidelsaGame_ = p_Main->getCidelsaHwConfiguration() - 1;
    
    switch (cidelsaGame_)
    {
        case ALTAIR:
            defineMemoryType(0x3100, 0x7fff, UNDEFINED);
            defineMemoryType(0x3000, 0x30ff, RAM);
            initRam(0x3000, 0x30ff);
            p_Main->assDefault("altairrom", 0, 0x2FFF);
            p_Main->setScrtValues(true, 4, 0x94, 5, 0xa6, "Altair");
        break;

        case ALTAIR24:
            cidelsaGame_ = ALTAIR;
            defineMemoryType(0x6100, 0x7fff, UNDEFINED);
            defineMemoryType(0x6000, 0x60ff, RAM);
            initRam(0x6000, 0x60ff);
            p_Main->assDefault("altairrom", 0, 0x5FFF);
            p_Main->setScrtValues(true, 4, 0x94, 5, 0xa6, "Altair");
        break;

        case DESTROYER1:
            defineMemoryType(0x2100, 0x7fff, UNDEFINED);
            defineMemoryType(0x2000, 0x20ff, RAM);
            initRam(0x2000, 0x20ff);
            p_Main->assDefault("destroyer_1_rom", 0, 0x1FFF);
            p_Main->setScrtValues(true, 4, 0x2b, 5, 0x3c, "Destroyer1");
        break;

        case DESTROYER2:
            defineMemoryType(0x3100, 0x7fff, UNDEFINED);
            defineMemoryType(0x3000, 0x30ff, RAM);
            initRam(0x3000, 0x30ff);
            p_Main->assDefault("destroyer_2_rom", 0, 0x1FFF);
            p_Main->setScrtValues(true, 4, 0x2b, 5, 0x3c, "Destroyer2");
        break;

        case DRACO:
            defineMemoryType(0x4400, 0x7fff, UNDEFINED);
            defineMemoryType(0x4000, 0x43ff, RAM);
            p_Main->assDefault("dracorom", 0, 0x3FFF);
            initRam(0x4000, 0x43ff);
            p_Main->setScrtValues(true, 4, 0x54, 5, 0x202, "Draco");
        break;

        case DRACO32:
            cidelsaGame_ = DRACO;
            defineMemoryType(0x8000, 0x83ff, RAM);
            p_Main->assDefault("dracorom", 0, 0x7FFF);
            initRam(0x8000, 0x83ff);
            p_Main->setScrtValues(true, 4, 0x54, 5, 0x202, "Draco");
        break;
    }
    
    cid4_ = 0;
    cid1_ = 0;

    configure1870Cidelsa();
    Show(true);

    cpuCycles_ = 0;
    instructionCounter_= 0;
    p_Main->startTime();

    threadPointer->Run();
}

void Cidelsa::writeMemDataType(Word address, Byte type)
{
    switch (memoryType_[address/256]&0xff)
    {
        case RAM:
        case ROM:
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            increaseExecutedMainMemory(address, type);
        break;
    }
}

Byte Cidelsa::readMemDataType(Word address, uint64_t* executed)
{
    switch (memoryType_[address/256]&0xff)
    {
        case RAM:
        case ROM:
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;
    }
    return MEM_TYPE_UNDEFINED;
}

Byte Cidelsa::readMem(Word address)
{
    switch (memoryType_[address/256]&0xff)
    {
        case UNDEFINED:
            return 255;
        break;

        case PRAM1870:
            return readPram(address);
        break;

        case CRAM1870:
            return readCram(address);
        break;

        case ROM:
        case RAM:
            return mainMemory_[address];
        break;

        default:
            return 255;
        break;
    }
}

Byte Cidelsa::readMemDebug(Word address)
{
    return readMem(address);
}

void Cidelsa::writeMem(Word address, Byte value, bool writeRom)
{
    switch (memoryType_[address/256]&0xff)
    {
        case UNDEFINED:
        case ROM:
            if (writeRom)
                mainMemory_[address]=value;
        break;

        case PRAM1870:
            writePram(address, value);
        break;

        case CRAM1870:
            writeCram(address, value);
 //           if (value == 0x37)
   //             p_Main->eventShowMessage(scratchpadRegister_[programCounter_]);
        break;

        case RAM:
            if (mainMemory_[address]==value)
                return;
            mainMemory_[address]=value;
            if (address>= memoryStart_ && address<(memoryStart_+256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;
    }
}

void Cidelsa::writeMemDebug(Word address, Byte value, bool writeRom)
{
    writeMem(address, value, writeRom);
}

void Cidelsa::cpuInstruction()
{
    cpuCycleStep();
}

void Cidelsa::resetPressed()
{
    resetCpu();
    resetPressed_ = false;
}

void Cidelsa::cid1Bit8(bool set)
{
    if (set)
        cid1_ |= 128;
    else
        cid1_ &= 0x7f;
}

void Cidelsa::onReset()
{
    resetPressed_ = true;
}

/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 ***                                                             ***
 *** 1802 Code based on elf emulator by Michael H Riley with     **
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

#include "wx/textfile.h"

#include "main.h"

int startCpuState1804_1805[] =
{
    STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1,  // 0x
    STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1,  // 1x
    STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805,  // 2x
    STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1,  // 3x
    STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1,  // 4x
    STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1,  // 5x
    STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805,  // 6x
    STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_EXECUTE_2_1805, STATE_FETCH_1, STATE_EXECUTE_4_1805, STATE_EXECUTE_2_1805, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_EXECUTE_2_1805, STATE_FETCH_1, STATE_FETCH_1, STATE_EXECUTE_2_1805,  // 7x
    STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805, STATE_EXECUTE_8_1805,  // 8x
    STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805, STATE_EXECUTE_6_1805,  // 9x
    STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805,  // Ax
    STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805, STATE_EXECUTE_2_1805,  // Bx
    STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805, STATE_EXECUTE_3_1805,  // Cx
    STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1,  // Dx
    STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1,  // Ex
    STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_EXECUTE_2_1805, STATE_FETCH_1, STATE_FETCH_1, STATE_EXECUTE_2_1805, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_FETCH_1, STATE_EXECUTE_2_1805, STATE_FETCH_1, STATE_FETCH_1, STATE_EXECUTE_2_1805,  // Fx
};

int numberOfCycles1802[] =
{
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 0x
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 1x
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 2x
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 3x
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 4x
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 5x
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 6x
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 7x
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 8x
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 9x
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // Ax
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // Bx
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // Cx
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // Dx
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // Ex
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // Fx
};

int numberOfCycles1805[] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // 0x
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // 1x
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  // 2x
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // 3x
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // 4x
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // 5x
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  // 6x
    3, 3, 3, 3, 4, 3, 6, 4, 3, 3, 3, 3, 4, 3, 3, 4,  // 7x
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  // 8x
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  // 9x
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  // Ax
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  // Bx
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  // Cx
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // Dx
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // Ex
    3, 3, 3, 3, 4, 3, 3, 4, 3, 3, 3, 3, 4, 3, 3, 4,  // Fx
};

Cdp1802::Cdp1802()
{
}

void Cdp1802::initCpu()
{
    cpuType_ = p_Main->getCpuType();

    if (p_Main->getCpuStartupRegisters() == STARTUP_ZEROED)
    {
        for (int i=1; i<16; i++)
            scratchpadRegister_[i] = 0;
        dataFlag_ = 0;
        accumulator_ = 0;
        registerT_ = 0;
        registerB_ = 0;
        counter_ = 0;
        ch_ = 0;
   }
    else
    {
        for (int i=1; i<16; i++)
            scratchpadRegister_[i] = rand() % 0x10000;
        dataFlag_ = rand() % 2;
        accumulator_ = rand() % 0x100;
        registerT_ = rand() % 0x100;
        registerB_ = rand() % 0x100;
        counter_ = rand() % 0x100;
        ch_ = rand() % 0x100;
   }
    efFlags_ = 0xf;

    cpuMode_ = RESET;
    clear_ = 0;
    wait_ = 1;
    initIo();
    steps_ = -1;

    for (int i=0; i<8; i++) outValues_[i] = 0;
    for (int i=0; i<8; i++) inValues_[i] = 0;

    debugMode_ = false;
    chip8DebugMode_ = false;
    trace_ = false;
    traceDma_ = false;
    traceInt_ = false;
    traceChip8Int_ = false;
    skipTrace_ = false;
    singleStateStep_ = false;
    interruptRequested_ = false;
    for (int type=0; type<INTERRUPT_TYPE_MAX; type++)
    {
        interruptRequested[type] = false;
        interruptRequestedCounter[type] = 0;
    }
    stopHiddenTrace_ = false;
    startHiddenTrace_ = false;
}

void Cdp1802::resetCpu()
{
    cpuState_ = STATE_FETCH_1;
    skipMachineCycleAfterIdle_=false;
    flipFlopQ_ = 0;
    interruptEnable_ = 1;
    cie_ = 1;
    xie_ = 1;
    ci_ = 0;
    xi_ = 0;
    dataPointer_ = 0;
    programCounter_ = 0;
    scratchpadRegister_[0] = 0;
    ctrPre_ = 32;
    tq_ = 0;
    ctrRunning_ = 0;
    ctrMode_ = 0;
    idle_ = 0;
    address_ = 0;
    colourMask_ = 0;
    bus_ = 0;
    for (int video=0; video<currentComputerConfiguration.numberOfVideoTypes_; video++)
    {
        if (p_Video[video] != NULL)
            p_Video[video]->reset();
    }
    for (int type=0; type<INTERRUPT_TYPE_MAX; type++)
    {
        interruptRequested[type] = false;
        interruptRequestedCounter[type] = 0;
    }
}

void Cdp1802::resetEffectiveClock()
{
    p_Main->startTime();
    cpuCycles_ = 0;
    instructionCounter_ = 0;
    for (int video=0; video<currentComputerConfiguration.numberOfVideoTypes_; video++)
    {
        if (p_Video[video] != NULL)
            p_Video[video]->resetVideoSyncCount();
    }
}

void Cdp1802::machineCycle()
{
    for (int i=1; i<5; i++)
    {
        setEf(i, p_Computer->ef(i));
    }

    for (int i=0; i<CYCLE_TYPE_MAX; i++)
    {
        p_Computer->cycle(i);
    }
    if (ctrRunning_ == 1)
    {
        switch (ctrMode_)
        {
            case 3:
                if (--ctrPre_ == 0)
                {
                    decCounter();
                    ctrPre_ = 32;
                }
            break;

            case 4:
                if ((efFlags_ & 1) == 1)
                    decCounter();
            break;

            case 5:
                if ((efFlags_ & 2) == 2)
                    decCounter();
            break;
        }
    }
    if (!p_Main->isZoomEventOngoing())
        soundCycle();
}

void Cdp1802::setCpuMode(int mode)
{
    clear_ = (mode>>1)&1;
    wait_ = mode&1;
    p_Computer->setMode();
}

void Cdp1802::setClear(int value)
{
    clear_= (value)?1:0;
    p_Computer->setMode();
}

void Cdp1802::setWait(int value)
{
    wait_= (value)?1:0;
    p_Computer->setMode();
}

Byte Cdp1802::dmaIn(Byte value)
{
    Byte dmaInValue = 0;
    p_Computer->showDmaLed();
    if (traceDma_)
    {
        wxString traceText;
        traceText.Printf("----  DMA in    R0=%04X", scratchpadRegister_[0]);
        p_Main->debugTrace(traceText);
    }
    
    if (cpuMode_ != RUN && cpuMode_ != LOAD && !currentComputerConfiguration.dmaInReset_defined)
        return dmaInValue;

    writeMem(scratchpadRegister_[0], value, false);
    dmaInValue = readMem(scratchpadRegister_[0]);
    address_ = scratchpadRegister_[0]++;
    idle_=0;
    cpuState_ = STATE_FETCH_1;
    cpuCycles_++;
    instructionCounter_++;
//    machineCycle(); // Using this will crash Elfs when tying in keys with Q sound on 'Hardware'
    if (singleStateStep_)
    {
        p_Computer->showCycleAddress(address_);
        p_Computer->showCycleData(value);
        singleStateStep();
    }
    return dmaInValue;
}

Byte Cdp1802::dmaOut()
{
    Byte ret;
    ret = 255;
    if (traceDma_)
    {
        wxString traceText;
        traceText.Printf("----  DMA out   R0=%04X", scratchpadRegister_[0]);
        p_Main->debugTrace(traceText);
    }
    ret=readMem(scratchpadRegister_[0]);
    address_ = scratchpadRegister_[0]++;
    idle_=0;
    cpuState_ = STATE_FETCH_1;
    cpuCycles_++;
    instructionCounter_++;
    machineCycle();
    
    if (singleStateStep_)
    {
        p_Computer->showCycleAddress(address_);
        p_Computer->showCycleData(ret);
        singleStateStep();
    }

    return ret;
}

Byte Cdp1802::pixieDmaOut(int *color, int colourType)
{
    p_Computer->showDmaLed();
    Byte ret;
    ret = 255;
    if (traceDma_)
    {
        wxString traceText;
        traceText.Printf("----  DMA out   R0=%04X", scratchpadRegister_[0]);
        p_Main->debugTrace(traceText);
    }
    ret=readMem(scratchpadRegister_[0]);

    switch (colourType)
    {
        case PIXIE_COLOR_ETI_1864:
            *color = colorMemory1864_[((scratchpadRegister_[0] >> 1) & 0xf8) + (scratchpadRegister_[0] & 0x7)] & 0x7;
        break;
            
        case PIXIE_COLOR_ETI_1862:
            *color = colorMemory1862_[((scratchpadRegister_[0] >> 1) & 0xf8) + (scratchpadRegister_[0] & 0x7)] & 0x7;
        break;
            
        case PIXIE_COLOR_VIP_1862:
            if (colourMask_ == 0)
                *color = 7;
            else
                *color = colorMemory1862_[scratchpadRegister_[0] & colourMask_] & 0x7;
        break;
            
        case PIXIE_COLOR_VIP_1864:
            if (colourMask_ == 0)
                *color = 7;
            else
                *color = colorMemory1864_[scratchpadRegister_[0] & colourMask_] & 0x7;
        break;

        case PIXIE_COLOR_VICTORY_1862:
            if (colourMask_ == 0)
                *color = 7;
            else
                *color = colorMemory1862_[((scratchpadRegister_[0] >> 2) & 0x38) + (scratchpadRegister_[0] & 0x7)] & 0x7;
        break;

        case PIXIE_COLOR_VICTORY_1864:
            if (colourMask_ == 0)
                *color = 7;
            else
                *color = colorMemory1864_[((scratchpadRegister_[0] >> 2) & 0x38) + (scratchpadRegister_[0] & 0x7)] & 0x7;
        break;

        case PIXIE_COLOR_STUDIOIV:
            *color = colorMemory1864_[(scratchpadRegister_[0]&0xf) +  ((scratchpadRegister_[0]&0x3c0) >> 2)] & 0x7;
        break;

        case PIXIE_COLOR_TMC2000_1862:
            *color = colorMemory1862_[scratchpadRegister_[0] & 0x3ff] & 0x7;
        break;

        case PIXIE_COLOR_TMC2000_1864:
            *color = colorMemory1864_[scratchpadRegister_[0] & 0x3ff] & 0x7;
        break;

        default:
            *color = 0;
        break;
    }

    address_ = scratchpadRegister_[0]++;
    idle_=0;
    cpuState_ = STATE_FETCH_1;
    cpuCycles_++;
    instructionCounter_++;
    soundCycle();

    if (singleStateStep_)
    {
        p_Computer->showCycleAddress(address_);
        p_Computer->showCycleData(ret);
        singleStateStep();
    }

    return ret;
}

void Cdp1802::visicomDmaOut(Byte *vram1, Byte *vram2)
{
    if (traceDma_)
    {
        wxString traceText;
        traceText.Printf("----  DMA out   R0=%04X", scratchpadRegister_[0]);
        p_Main->debugTrace(traceText);
    }
    *vram1 = readMem(scratchpadRegister_[0]);
    *vram2 = readMem(scratchpadRegister_[0]+0x200);

    scratchpadRegister_[0]++;
    idle_=0;
    cpuState_ = STATE_FETCH_1;
    cpuCycles_++;
    instructionCounter_++;
    soundCycle();
}

Byte Cdp1802::pixieDmaOut()
{
    p_Computer->showDmaLed();
    Byte ret;
    ret = 255;
    if (traceDma_)
    {
        wxString traceText;
        traceText.Printf("----  DMA out   R0=%04X", scratchpadRegister_[0]);
        p_Main->debugTrace(traceText);
    }
    ret=readMem(scratchpadRegister_[0]);
    address_ = scratchpadRegister_[0]++;
    idle_=0;
    cpuState_ = STATE_FETCH_1;
    cpuCycles_++;
    instructionCounter_++;
    soundCycle();

    if (singleStateStep_)
    {
        p_Computer->showCycleAddress(address_);
        p_Computer->showCycleData(ret);
        singleStateStep();
    }

    return ret;
}

void Cdp1802::decCounter()
{
    if (--counter_ == 0)
    {
        if (tq_)
        {
            flipFlopQ_ = (flipFlopQ_) ? 0 : 1;
        }
        ci_ = (cie_) ? 1 : 0;
        counter_ = ch_;
        if (ci_)
            interrupt();
    }
}

void Cdp1802::setEf(int flag,int value)
{
    if (flag == 1)
    {
        if (ctrRunning_)
        {
            switch (ctrMode_)
            {
                case 1:
                    if (value == 0 &&(efFlags_ & 1))
                        decCounter();
                break;
                case 4:
                    if (value == 0 &&(efFlags_ & 1))
                    {
                        ctrRunning_ = 0;
                        ctrMode_ = 0;
                        ci_ = (cie_) ? 1 : 0;
                        if (ci_)
                            interrupt();
                    }
                break;
            }
        }
        if (value) efFlags_ |= 1;
        else efFlags_ &= 0xe;
    }
    if (flag == 2)
    {
        if (ctrRunning_)
        {
            switch (ctrMode_)
            {
                case 2:
                    if (value == 0 &&(efFlags_ & 2))
                        decCounter();
                break;
                case 5:
                    if (value == 0 &&(efFlags_ & 1))
                    {
                        ctrRunning_ = 0;
                        ctrMode_ = 0;
                        ci_ = (cie_) ? 1 : 0;
                        if (ci_)
                            interrupt();
                    }
                break;
            }
        }
        if (value) efFlags_ |= 2;
        else efFlags_ &= 0xd;
    }
    if (flag == 3)
    {
        if (value) efFlags_ |= 4;
        else efFlags_ &= 0xb;
    }
    if (flag == 4)
    {
         if (value) efFlags_ |= 8;
        else efFlags_ &= 7;
    }
}

bool Cdp1802::interrupt()
{
    interruptRequested_ = false;
    p_Computer->showIntLed();
    
    if (p_Main->isDiagActive())
    {
        if (interruptEnable_ && (clear_ == 1))
        {
            p_Main->eventUpdateLedStatus(false, 3); //INT
            p_Main->eventUpdateLedStatus(true, 4); //INTACK
        }
        else
        {
            p_Main->eventUpdateLedStatus(true, 3); //INT
            p_Main->eventUpdateLedStatus(false, 4); //INTACK
        }
    }

    if (interruptEnable_ && clear_ == 1)
    {
        if (traceInt_)
        {
            p_Main->debugTrace("----  Interrupt");
        }
        if (traceChip8Int_)
        {
            p_Main->chip8DebugTrace("--- 1802 Interrupt");
        }
        registerT_= (dataPointer_<<4) | programCounter_;
        dataPointer_=2;
        programCounter_=1;
        interruptEnable_=0;
        cpuCycles_++;
        instructionCounter_++;
        machineCycle();
        
        cpuState_ = STATE_FETCH_1;
        
        if (singleStateStep_)
        {
            p_Computer->showCycleAddress(scratchpadRegister_[programCounter_]);
            p_Computer->showCycleData(0);
            singleStateStep();
        }
        idle_=0;
        return true;
    }
    idle_=0;
    return false;
}

void Cdp1802::requestInterrupt()
{
    interruptRequested_ = true;
}

void Cdp1802::requestInterrupt(int type, bool state, int picNumber)
{
    if (traceInt_)
    {
        if (type != INTERRUPT_TYPE_I8275_1 && type != INTERRUPT_TYPE_I8275_4)
        {
            if (state && !interruptRequested[type])
                p_Main->debugTrace("----  Int. request: " + interruptTypeList_[type]);
            if (!state && interruptRequested[type])
                p_Main->debugTrace("----  Int. cleared: " + interruptTypeList_[type]);
        }
    }
    interruptRequested[type]= state;
    picInterruptNumber[type]= picNumber;
    p_Computer->picInterruptRequest(type, state, picNumber);
}

void Cdp1802::pixieInterrupt()
{
    p_Computer->showIntLed();
    if (interruptEnable_)
    {
        if (traceInt_)
        {
            p_Main->debugTrace("----  Interrupt");
 //           p_Main->eventMessageHex(scratchpadRegister_[programCounter_]);
        }
        if (traceChip8Int_)
        {
            p_Main->chip8DebugTrace("--- 1802 Interrupt");
        }
        registerT_= (dataPointer_<<4) | programCounter_;
        dataPointer_=2;
        programCounter_=1;
        interruptEnable_=0;
        cpuCycles_++;
        instructionCounter_++;
//      Adding a 'machineCycle()' here will mess up the Pixie screens on at least the Elfs. The machineCycle is however done as part of the cpuCycleStep routine if cycle0 is set to 1.
        
        cpuState_ = STATE_FETCH_1;
        
        if (singleStateStep_)
            singleStateStep();
    }
    idle_=0;
}

void Cdp1802::cpuCycleExecute1_1805()
{
    wxString buffer;
    
    Byte i, n, df1;
    int tempWord;
    
    n = instructionCode_ & 15;
    i = instructionCode_>>4;
    bus_ = 0;
    
    switch(i)
    {
        case 2:
            if (cpuType_ == CPU1805)
            {
                switch (cpuState_)
                {
                    case STATE_EXECUTE_3_1805:
                        bus_=0;
                        address_=scratchpadRegister_[n];
                        scratchpadRegister_[n]--;
                    break;
                    case STATE_EXECUTE_2_1805:
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        bus_=readMem(scratchpadRegister_[programCounter_]++);
                        registerB_=bus_;
                    break;
                }
            }
            else
            {
                if (trace_)
                {
                    buffer.Printf("Illegal code");
                    traceBuffer_ = traceBuffer_ + buffer;
                }
                cpuState_ = STATE_FETCH_1;
            }
            break;
            
        case 6: // 1804
            switch (cpuState_)
            {
                case STATE_EXECUTE_3_1805:
                    bus_=readMem(scratchpadRegister_[dataPointer_]++);
                    registerB_=bus_;
                break;
                case STATE_EXECUTE_2_1805:
                    registerT_=registerB_;
                    bus_=readMem(scratchpadRegister_[dataPointer_]++);
                    registerB_=bus_;
                break;
            }
            break;
            
        case 7:
            if (cpuType_ == CPU1805)
            {
                switch(n)
                {
                    case 4:
                        bus_=readMem(scratchpadRegister_[dataPointer_]);
                        tempWord = accumulator_ + bus_ + dataFlag_;
                        dataFlag_ = (tempWord >> 8) & 0x1;
                        accumulator_ = tempWord & 255;
                    break;
                    case 6:
                        switch (cpuState_)
                        {
                            case STATE_EXECUTE_4_1805:
                                scratchpadRegister_[dataPointer_]--;
                                bus_=0;
                            break;
                            case STATE_EXECUTE_3_1805:
                                writeMem(scratchpadRegister_[dataPointer_]--, registerT_, false);
                                bus_=registerT_;
                            break;
                            case STATE_EXECUTE_2_1805:
                                bus_=accumulator_;
                                writeMem(scratchpadRegister_[dataPointer_]--, accumulator_, false);
                                df1= (dataFlag_) ? 128 : 0;
                                dataFlag_ = (accumulator_ & 1) ? 1 : 0;
                                accumulator_ = (accumulator_ >> 1) | df1;
                            break;
                        }
                        break;
                    case 7:
                        bus_ = readMem(scratchpadRegister_[dataPointer_]);
                        bus_ += (~dataFlag_&0x1);
                        tempWord = accumulator_+((~bus_)&0xff)+1;
                        dataFlag_ = (tempWord >> 8) & 0x1;
                        accumulator_ = tempWord & 255;
                    break;
                    case 0xc:
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        bus_=readMem(scratchpadRegister_[programCounter_]++);
                        tempWord = accumulator_ + bus_ + dataFlag_;
                        dataFlag_ = (tempWord >> 8) & 0x1;
                        accumulator_ = tempWord & 255;
                    break;
                    case 0xf:
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        bus_=readMem(scratchpadRegister_[programCounter_]++);
                        bus_ += (~dataFlag_&0x1);
                        tempWord = accumulator_+((~bus_)&0xff)+1;
                        dataFlag_ = (tempWord >> 8) & 0x1;
                        accumulator_ = tempWord & 255;
                    break;
                    default:
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        cpuState_ = STATE_FETCH_1;
                    break;
                }
            }
            else
            {
                if (trace_)
                {
                    buffer.Printf("Illegal code");
                    traceBuffer_ = traceBuffer_ + buffer;
                }
                cpuState_ = STATE_FETCH_1;
            }
            break;
            
        case 8: // 1804
            switch (cpuState_)
            {
                case STATE_EXECUTE_8_1805:
                    bus_=0;
                    address_=scratchpadRegister_[n];
                    registerT_=address_ & 0xff;
                    registerB_=address_ >> 8;
                break;
                case STATE_EXECUTE_7_1805:
                    writeMem(scratchpadRegister_[dataPointer_]--,registerT_, false);
                    bus_=registerT_;
                break;
                case STATE_EXECUTE_6_1805:
                    writeMem(scratchpadRegister_[dataPointer_]--,registerB_, false);
                    bus_=registerB_;
                break;
                case STATE_EXECUTE_5_1805:
                    bus_=0;
                    address_=scratchpadRegister_[programCounter_];
                    registerT_=address_ & 0xff;
                    registerB_=address_ >> 8;
                break;
                case STATE_EXECUTE_4_1805:
                    bus_=0;
                    scratchpadRegister_[n] = (registerB_ << 8) + registerT_;
                    address_=scratchpadRegister_[n];
                break;
                case STATE_EXECUTE_3_1805:
                    registerB_=readMem(scratchpadRegister_[n]++);
                    bus_=registerB_;
                break;
                case STATE_EXECUTE_2_1805:
                    registerT_=registerB_;
                    registerB_=readMem(scratchpadRegister_[n]++);
                    bus_=registerB_;
                break;
            }
            break;
            
        case 9: // 1804
            switch (cpuState_)
            {
                case STATE_EXECUTE_6_1805:
                    bus_=0;
                    address_=scratchpadRegister_[n];
                    registerT_=address_ & 0xff;
                    registerB_=address_ >> 8;
                break;
                case STATE_EXECUTE_5_1805:
                    bus_=0;
                    address_=scratchpadRegister_[dataPointer_];
                    scratchpadRegister_[dataPointer_]++;
                break;
                case STATE_EXECUTE_4_1805:
                    bus_=0;
                    scratchpadRegister_[programCounter_] = (registerB_ << 8) + registerT_;
                    address_=scratchpadRegister_[programCounter_];
                break;
                case STATE_EXECUTE_3_1805:
                    registerB_ = readMem(scratchpadRegister_[dataPointer_]++);
                    bus_=registerB_;
                break;
                case STATE_EXECUTE_2_1805:
                    registerT_=registerB_;
                    registerB_ = readMem(scratchpadRegister_[dataPointer_]);
                    bus_=registerB_;
                break;
            }
            break;
            
        case 0xa: // 1804
            switch (cpuState_)
            {
                case STATE_EXECUTE_3_1805:
                    bus_=0;
                    address_=scratchpadRegister_[n];
                    registerT_=address_ & 0xff;
                    registerB_=address_ >> 8;
                break;
                case STATE_EXECUTE_2_1805:
                    writeMem(scratchpadRegister_[dataPointer_]--,registerT_, false);
                    bus_=registerT_;
                break;
            }
            break;
            
        case 0xb: // 1804
            bus_=0;
            address_=scratchpadRegister_[n];
            registerT_=address_ & 0xff;
            registerB_=address_ >> 8;
            break;
            
        case 0xc:  // 1804
            switch (cpuState_)
            {
                case STATE_EXECUTE_3_1805:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    registerB_=readMem(scratchpadRegister_[programCounter_]++);
                    bus_=registerB_;
                break;
                case STATE_EXECUTE_2_1805:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    registerT_=registerB_;
                    registerB_=readMem(scratchpadRegister_[programCounter_]++);
                    bus_=registerB_;
                break;
            }
            break;
            
        case 0xf:
            if (cpuType_ == CPU1805)
            {
                switch(n)
                {
                    case 4:
                        bus_ = readMem(scratchpadRegister_[dataPointer_]);
                        tempWord = accumulator_ + bus_;
                        dataFlag_ = (tempWord >> 8) & 0x1;
                        accumulator_ = tempWord & 255;
                    break;
                    case 7:
                        bus_ = readMem(scratchpadRegister_[dataPointer_]);
                        tempWord = accumulator_+((~bus_)&0xff)+1;
                        dataFlag_ = (tempWord >> 8) & 0x1;
                        accumulator_ = tempWord & 255;
                    break;
                    case 0xc:
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        bus_ = readMem(scratchpadRegister_[programCounter_]++);
                        tempWord = accumulator_ + bus_;
                        dataFlag_ = (tempWord >> 8) & 0x1;
                        accumulator_ = tempWord & 255;
                    break;
                    case 0xf:
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        bus_ = readMem(scratchpadRegister_[programCounter_]++);
                        tempWord = accumulator_+((~bus_)&0xff)+1;
                        dataFlag_ = (tempWord >> 8) & 0x1;
                        accumulator_ = tempWord & 255;
                    break;
                    default:
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        cpuState_ = STATE_FETCH_1;
                    break;
                }
            }
            else
            {
                if (trace_)
                {
                    buffer.Printf("Illegal code");
                    traceBuffer_ = traceBuffer_ + buffer;
                }
                cpuState_ = STATE_FETCH_1;
            }
            break;
        default:
            if (trace_)
            {
                buffer.Printf("Illegal code");
                traceBuffer_ = traceBuffer_ + buffer;
                cpuState_ = STATE_FETCH_1;
            }
            break;
    }
}

void Cdp1802::cpuCycleExecute2_1805()
{
    wxString buffer;

    Byte i, n;
    int tempWord;
    tempWord = 0;
    uint64_t executed;

    n = instructionCode_ & 15;
    i = instructionCode_>>4;
    
    switch(i)
    {
        case 0:
            switch(n)
            {
                case 0: // 1804
                    ctrRunning_ = 0;
                    ctrPre_ = 32;
                    ctrMode_ = 0;
                     if (trace_)
                    {
                        buffer.Printf("STPC");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 1: // 1804
                    decCounter();
                    if (trace_)
                    {
                        buffer.Printf("DTC       CNTR=%02X", counter_);
     //                                     XXX       R
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 2: // 1804
                    ctrMode_ = 5;
                    ctrRunning_ = 1;
                    ctrPre_ = 32;
                    if (trace_)
                    {
                        buffer.Printf("SPM2");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 3: // 1804
                    ctrMode_ = 2;
                    ctrRunning_ = 1;
                    ctrPre_ = 32;
                    if (trace_)
                    {
                        buffer.Printf("SCM2");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 4: // 1804
                    ctrMode_ = 4;
                    ctrRunning_ = 1;
                    ctrPre_ = 32;
                    if (trace_)
                    {
                        buffer.Printf("SPM1");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 5: // 1804
                    ctrMode_ = 1;
                    ctrRunning_ = 1;
                    ctrPre_ = 32;
                    if (trace_)
                    {
                        buffer.Printf("SCM1");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 6: // 1804
                    bus_ = accumulator_;
                    ch_ = bus_;
                    if (!ctrRunning_)
                    {
                          counter_ = accumulator_;
                          ci_ = 0;
                        if (trace_)
                        {
                            buffer.Printf("LDC       CH/CNTR=%02X", ch_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        if (trace_)
                        {
                            buffer.Printf("LDC       CH=%02X", ch_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                break;
                case 7: // 1804
                    ctrMode_ = 3;
                    ctrRunning_ = 1;
                    ctrPre_ = 32;
                    if (trace_)
                    {
                        buffer.Printf("STM");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 8: // 1804
                    bus_ = counter_;
                    accumulator_ = bus_;
                    if (trace_)
                    {
                        buffer.Printf("GEC       D=%02X", accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 9: // 1804
                    tq_ = 1;
                    if (trace_)
                    {
                        buffer.Printf("ETQ");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 0xa: // 1804
                    xie_ = 1;
                    if (trace_)
                    {
                        buffer.Printf("XIE");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 0xb: // 1804
                    xie_ = 0;
                    if (trace_)
                    {
                        buffer.Printf("XID");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 0xc: // 1804
                    cie_ = 1;
                    if (trace_)
                    {
                        buffer.Printf("CIE");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 0xd: // 1804
                    cie_ = 0;
                    if (trace_)
                    {
                        buffer.Printf("CID");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                default:
                    if (trace_)
                    {
                        buffer.Printf("Illegal code");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
            }
            bus_ = 0;
            address_=scratchpadRegister_[n];
        break;

        case 2:
            if (cpuType_ == CPU1805)
            {
                bus_=readMem((Word) (scratchpadRegister_[programCounter_]++));

                if (scratchpadRegister_[n] != 0)
                    scratchpadRegister_[programCounter_] = (registerB_<<8) | bus_;

                if (trace_)
                {
                    buffer.Printf("DBNZ R%X,%02X%02X R%X=%04X", n, registerB_, bus_, n,scratchpadRegister_[n]);
                    traceBuffer_ = traceBuffer_ + buffer;
                }
            }
            else
            {
                if (trace_)
                {
                    buffer.Printf("Illegal code");
                    traceBuffer_ = traceBuffer_ + buffer;
                }
                bus_ = 0;
            }
        break;

        case 3:
            switch(n)
            {
                case 0xe: // 1804
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    if (ci_)
                    {
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                        ci_ = 0;
                    }
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        buffer.Printf("BCI  %02X",bus_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 0xf: // 1804
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    if (xi_)
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        buffer.Printf("BXI  %02X",bus_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                default:
                    if (trace_)
                    {
                        buffer.Printf("Illegal code");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                    bus_ = 0;
                break;
            }
         break;

        case 6: // 1804
            scratchpadRegister_[n] = (registerT_ << 8) | registerB_;
            if (trace_)
            {
                 buffer.Printf("RLXA R%X   R%X=%04X", n, n, scratchpadRegister_[n]);
                 traceBuffer_ = traceBuffer_ + buffer;
             }
            bus_ = 0;
            address_=scratchpadRegister_[n];
          break;

        case 7:
            if (cpuType_ == CPU1805)
            {
                switch(n)
                {
                    case 4:
                        tempWord = accumulator_ + (dataFlag_ << 8);
                        if ((tempWord & 0xf) > 9 || (tempWord & 0xf) < (bus_&0xf))
                            tempWord += 6;
                        if ((tempWord & 0xf0) > 0x90 || (tempWord & 0x100) == 0x100)
                        {
                            dataFlag_=1;
                            tempWord += 0x60;
                        }
                        accumulator_ = tempWord & 255;

                        if (trace_)
                        {
                            buffer.Printf("DADC      D=%02X", accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        bus_ = 0;
                        address_=scratchpadRegister_[programCounter_];
                    break;
                    case 6:
                        writeMemDebug(scratchpadRegister_[dataPointer_], accumulator_, false);
                        if (trace_)
                        {
                              buffer.Printf("DSAV      D=%02X", accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        bus_=accumulator_;
                        address_=scratchpadRegister_[dataPointer_];
                    break;
                    case 7:
                        accumulator_ -= (~dataFlag_&0x1);
                        if (dataFlag_ == 1)
                        {
                            if ((accumulator_ & 0xf) > ((~bus_)&0xf))
                                accumulator_ = (accumulator_ & 0xf0) + (((accumulator_ & 0x0f) + 0xa) & 0x0f);
                        }
                        else
                        {
                            if ((accumulator_ & 0xf) < ((~bus_)&0xf))
                            {
                                accumulator_ = ~accumulator_;
                                accumulator_ = (accumulator_ & 0xf0) + (((accumulator_ & 0x0f) + 0xa) & 0x0f);
                            }
                            else
                                accumulator_ = ~accumulator_;
                        }
                        if ((accumulator_ & 0xf) > 9)
                            accumulator_ += 6;
                        if ((accumulator_ & 0xf0) > 0x90 )
                        {
                            accumulator_ += 0x60;
                        }

                        if (trace_)  
                        {
                            buffer.Printf("DSMB      D=%02X", accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        bus_ = 0;
                        address_=scratchpadRegister_[programCounter_];
                    break;
                    case 0xc:
                        tempWord = accumulator_ + (dataFlag_ << 8);
                        if ((tempWord & 0xf) > 9 || (tempWord & 0xf) < (bus_&0xf))
                            tempWord += 6;
                        if ((tempWord & 0xf0) > 0x90 || (tempWord & 0x100) == 0x100)
                        {
                            dataFlag_=1;
                            tempWord += 0x60;
                        }
                        accumulator_ = tempWord & 255;

                        if (trace_)  
                        {
                            buffer.Printf("DACI %02X   D=%02X", bus_, accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        bus_ = 0;
                        address_=scratchpadRegister_[programCounter_];
                    break;
                    case 0xf:
                        accumulator_ -= (~dataFlag_&0x1);
                        if (dataFlag_ == 1)
                        {
                            if ((accumulator_ & 0xf) > ((~bus_)&0xf))
                                accumulator_ = (accumulator_ & 0xf0) + (((accumulator_ & 0x0f) + 0xa) & 0x0f);
                        }
                        else
                        {
                            if ((accumulator_ & 0xf) < ((~bus_)&0xf))
                            {
                                accumulator_ = ~accumulator_;
                                accumulator_ = (accumulator_ & 0xf0) + (((accumulator_ & 0x0f) + 0xa) & 0x0f);
                            }
                            else
                                accumulator_ = ~accumulator_;
                        }
                        if ((accumulator_ & 0xf) > 9)
                            accumulator_ += 6;
                        if ((accumulator_ & 0xf0) > 0x90 )
                        {
                            accumulator_ += 0x60;
                        }

                        if (trace_)  
                        {
                            buffer.Printf("DSBI %02X   D=%02X", bus_, accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        bus_ = 0;
                        address_=scratchpadRegister_[programCounter_];
                    break;
                    default:
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        bus_ = 0;
                    break;
                }
            }
             else
            {
                if (trace_)
                {
                    buffer.Printf("Illegal code");
                    traceBuffer_ = traceBuffer_ + buffer;
                }
                bus_ = 0;
            }
        break;

        case 8: // 1804
            scratchpadRegister_[programCounter_] = (registerT_ << 8) + registerB_;
            if (trace_)
            {
                 buffer.Printf("SCAL R%X,%04X", n, scratchpadRegister_[programCounter_]);
                 traceBuffer_ = traceBuffer_ + buffer;
             }
            address_=scratchpadRegister_[programCounter_];
            bus_ = 0;
          break;

        case 9: // 1804
            scratchpadRegister_[n] = (registerT_ << 8) + registerB_;
            if (trace_)
            {
                 buffer.Printf("SRET R%X   R%X(P)=%04X", n, programCounter_, scratchpadRegister_[programCounter_]);
                 traceBuffer_ = traceBuffer_ + buffer;
             }
            address_=scratchpadRegister_[n];
            bus_ = 0;
         break;

        case 0xa: // 1804
            bus_ = registerB_;
            writeMem(scratchpadRegister_[dataPointer_]--, bus_, false);
            if (trace_)
            {
                 buffer.Printf("RSXD R%X",n);
                 traceBuffer_ = traceBuffer_ + buffer;
             }
          break;

        case 0xb: // 1804
            scratchpadRegister_[dataPointer_] = (registerB_ << 8) + registerT_;
            if (trace_)
            {
                 buffer.Printf("RNX  R%X   R%X=%04X", n, dataPointer_, scratchpadRegister_[dataPointer_]);
                traceBuffer_ = traceBuffer_ + buffer;
             }
            address_=scratchpadRegister_[dataPointer_];
            bus_ = 0;
         break;

        case 0xc:  // 1804
            scratchpadRegister_[n] = (registerT_ << 8) + registerB_;
            if (trace_)
            {
                if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-3, &executed) == MEM_TYPE_OPCODE_RLDL)
                     buffer.Printf("RLDL R%X,%04X", n, scratchpadRegister_[n]);
                else
                     buffer.Printf("RLDI R%X,%04X", n, scratchpadRegister_[n]);
                 traceBuffer_ = traceBuffer_ + buffer;
             }
            address_=scratchpadRegister_[n];
            bus_ = 0;
          break;

        case 0xf:
            if (cpuType_ == CPU1805)
            {
                switch(n)
                {
                    case 4:
                        tempWord = accumulator_ + (dataFlag_ << 8);
                        if ((tempWord & 0xf) > 9 || (tempWord & 0xf) < (bus_&0xf))
                            tempWord += 6;
                        if ((tempWord & 0xf0) > 0x90 || (tempWord & 0x100) == 0x100)
                        {
                            dataFlag_=1;
                            tempWord += 0x60;
                        }
                        accumulator_ = tempWord & 255;

                        if (trace_)
                        {
                            buffer.Printf("DADD      D=%02X", accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        bus_ = 0;
                        address_=scratchpadRegister_[programCounter_];
                    break;
                    case 7:
                        accumulator_ -= (~dataFlag_&0x1);
                        if (dataFlag_ == 1)
                        {
                            if ((accumulator_ & 0xf) > ((~bus_)&0xf))
                                accumulator_ = (accumulator_ & 0xf0) + (((accumulator_ & 0x0f) + 0xa) & 0x0f);
                        }
                        else
                        {
                            if ((accumulator_ & 0xf) < ((~bus_)&0xf))
                            {
                                accumulator_ = ~accumulator_;
                                accumulator_ = (accumulator_ & 0xf0) + (((accumulator_ & 0x0f) + 0xa) & 0x0f);
                            }
                            else
                                accumulator_ = ~accumulator_;
                        }
                        if ((accumulator_ & 0xf) > 9)
                            accumulator_ += 6;
                        if ((accumulator_ & 0xf0) > 0x90 )
                        {
                            accumulator_ += 0x60;
                        }

                        if (trace_)
                        {
                            buffer.Printf("DSM       D=%02X", accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        bus_ = 0;
                        address_=scratchpadRegister_[programCounter_];
                    break;
                    case 0xc:
                        tempWord = accumulator_ + (dataFlag_ << 8);
                        if ((tempWord & 0xf) > 9 || (tempWord & 0xf) < (bus_&0xf))
                            tempWord += 6;
                        if ((tempWord & 0xf0) > 0x90 || (tempWord & 0x100) == 0x100)
                        {
                            dataFlag_=1;
                            tempWord += 0x60;
                        }
                        accumulator_ = tempWord & 255;

                        if (trace_)
                        {
                            buffer.Printf("DADI %02X   D=%02X", bus_, accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        bus_=0;
                        address_=scratchpadRegister_[programCounter_];
                    break;
                    case 0xf:
                        accumulator_ -= (~dataFlag_&0x1);
                        if (dataFlag_ == 1)
                        {
                            if ((accumulator_ & 0xf) > ((~bus_)&0xf))
                                accumulator_ = (accumulator_ & 0xf0) + (((accumulator_ & 0x0f) + 0xa) & 0x0f);
                        }
                        else
                        {
                            if ((accumulator_ & 0xf) < ((~bus_)&0xf))
                            {
                                accumulator_ = ~accumulator_;
                                accumulator_ = (accumulator_ & 0xf0) + (((accumulator_ & 0x0f) + 0xa) & 0x0f);
                            }
                            else
                                accumulator_ = ~accumulator_;
                        }
                        if ((accumulator_ & 0xf) > 9)
                            accumulator_ += 6;
                        if ((accumulator_ & 0xf0) > 0x90 )
                        {
                            accumulator_ += 0x60;
                        }

                        if (trace_)
                        {
                            buffer.Printf("DSMI %02X   D=%02X", bus_, accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        bus_=0;
                        address_=scratchpadRegister_[programCounter_];
                    break;
                    default:
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        bus_ = 0;
                    break;
                }
            }
            else
            {
                if (trace_)
                {
                    buffer.Printf("Illegal code");
                    traceBuffer_ = traceBuffer_ + buffer;
                }
                bus_ = 0;
            }
        break;
        default:
            if (trace_)
            {
                buffer.Printf("Illegal code");
                traceBuffer_ = traceBuffer_ + buffer;
            }
            bus_ = 0;
        break;
    }
    cpuCycleFinalize();
}

void Cdp1802::cpuCycleStep()
{
    int numberOfCycles;
    
    switch (cpuState_)
    {
        case STATE_FETCH_1:
            if (steps_ != 0)
            {
                if (instructionCode_ == 0x68 && (cpuType_ == CPU1805 || cpuType_ == CPU1804))
                    numberOfCycles = numberOfCycles1805[readMemDebug(scratchpadRegister_[programCounter_]+1)] - 1;
                else
                    numberOfCycles = numberOfCycles1802[readMemDebug(scratchpadRegister_[programCounter_], READ_FUNCTION_FREEZE_PIC_VECTOR | READ_FUNCTION_NOT_DEBUG)] - 1;
               
                cycle0_=0;
                machineCycle();
                cpuCycles_ ++;

                if (skipMachineCycleAfterIdle_)
                {
                    skipMachineCycleAfterIdle_=false;
                    cpuCycles_ ++;
                }
                else
                {
                    if (cycle0_ == 0)
                    {
                        while (numberOfCycles-- != 0)
                        {
                            machineCycle();
                            cpuCycles_ ++;
                        }
                    }
                }
                

                if (cycle0_ == 0 && steps_ != 0)
                {
                    cpuCycleFetch();
                }
            
                p_Computer->showState(STATE_FETCH);
            }
 //           else
  //              soundCycle();
        break;
            
        case STATE_FETCH_2:
            cpuCycleFetch2();
        break;
            
        case STATE_EXECUTE_1:
            cpuCycleExecute1();
            p_Computer->showState(STATE_EXECUTE);
        break;
            
        case STATE_EXECUTE_1_LBR:
            cpuCycleExecute1_LBR();
            p_Computer->showState(STATE_EXECUTE);
        break;
            
        case STATE_EXECUTE_2_LBR:
            cpuCycleExecute2_LBR();
        break;
            
        case STATE_EXECUTE_1_1805:
            cpuState_ = startCpuState1804_1805[instructionCode_];
            p_Computer->showState(STATE_EXECUTE);
            
            if (cpuState_ == STATE_FETCH_1)
                cpuCycleExecute2_1805();
            else
                cpuCycleExecute1_1805();
        break;
            
        case STATE_EXECUTE_2_1805:
        case STATE_EXECUTE_3_1805:
        case STATE_EXECUTE_4_1805:
        case STATE_EXECUTE_5_1805:
        case STATE_EXECUTE_6_1805:
        case STATE_EXECUTE_7_1805:
        case STATE_EXECUTE_8_1805:
            cpuState_--;
            
            if (cpuState_ == STATE_FETCH_1)
                cpuCycleExecute2_1805();
            else
                cpuCycleExecute1_1805();
        break;
    }
  
    if (cpuState_ == STATE_FETCH_1)
    {
        
        if (debugMode_)
        {
            p_Main->showInstructionTrace();
            p_Main->cycleDebug();
        }
        
        if (pseudoLoaded_)
            p_Main->cyclePseudoDebug();

        if (steps_ != 0)
            playSaveLoad();
        
        p_Computer->checkComputerFunction();
    }
    
    if (resetPressed_)
        p_Computer->resetPressed();
    
    p_Computer->showCycleAddress(address_);
    p_Computer->showCycleData(bus_);
    
    if (singleStateStep_)
        singleStateStep();
}

void Cdp1802::singleStateStep()
{
    setWait(0);
    setCpuMode(PAUSE);
    p_Computer->setGoTimer();
}

void Cdp1802::cpuCycleFetch()
{
    uint64_t executed;
    
    if (trace_)
        traceBuffer_.Printf("%04X: ",scratchpadRegister_[programCounter_]);
    
    instructionCode_=readMem(scratchpadRegister_[programCounter_]);
// ** address log
//    p_Main->addressLog(scratchpadRegister_[programCounter_]);
    bus_=instructionCode_;
    instructionCounter_++;

    Byte mem_type = p_Computer->readMemDataType(scratchpadRegister_[programCounter_], &executed);

    if (mem_type >= MEM_TYPE_OPCODE_RSHR)
    {
        switch (p_Computer->readMemDataType(scratchpadRegister_[programCounter_], &executed))
        {
            case MEM_TYPE_OPCODE_RSHR:
                if (instructionCode_ != 0x76)
                     mem_type = MEM_TYPE_OPCODE;
            break;
            case MEM_TYPE_OPCODE_RSHL:
                if (instructionCode_ != 0x7E)
                     mem_type = MEM_TYPE_OPCODE;
            break;
            case MEM_TYPE_OPCODE_BPZ:
                if (instructionCode_ != 0x33)
                     mem_type = MEM_TYPE_OPCODE;
            break;
            case MEM_TYPE_OPCODE_BGE:
                if (instructionCode_ != 0x33)
                     mem_type = MEM_TYPE_OPCODE;
            break;
            case MEM_TYPE_OPCODE_BM:
                if (instructionCode_ != 0x3b)
                     mem_type = MEM_TYPE_OPCODE;
            break;
            case MEM_TYPE_OPCODE_BL:
                if (instructionCode_ != 0x3b)
                     mem_type = MEM_TYPE_OPCODE;
            break;
            case MEM_TYPE_OPCODE_LSKP:
                if (instructionCode_ != 0xc8)
                     mem_type = MEM_TYPE_OPCODE;
            break;
            case MEM_TYPE_OPCODE_SKP:
                if (instructionCode_ != 0x38)
                     mem_type = MEM_TYPE_OPCODE;
            break;
            case MEM_TYPE_OPCODE_LBR_SLOT:
                if ((instructionCode_&0xf0) != 0xC0)
                     mem_type = MEM_TYPE_OPCODE;
           break;
            case MEM_TYPE_OPCODE_LDV:
            case MEM_TYPE_OPCODE_LDL:
            case MEM_TYPE_OPCODE_LDL_SLOT:
            case MEM_TYPE_OPCODE_LDRL:
            case MEM_TYPE_OPCODE_LDRL_SLOT:
                if (instructionCode_ != 0xF8)
                     mem_type = MEM_TYPE_OPCODE;
            break;
            case MEM_TYPE_OPERAND_LD_2:
            case MEM_TYPE_OPERAND_LDR_5:
                if ((instructionCode_&0xf0) != 0xB0)
                     mem_type = MEM_TYPE_OPCODE;
            break;
            case MEM_TYPE_OPERAND_LD_3:
            case MEM_TYPE_OPERAND_LDR_3:
                if (instructionCode_ != 0xF8)
                     mem_type = MEM_TYPE_OPCODE;
            break;
            case MEM_TYPE_OPERAND_LDR_2:
            case MEM_TYPE_OPERAND_LD_5:
                if ((instructionCode_&0xf0) != 0xA0)
                     mem_type = MEM_TYPE_OPCODE;
            break;
            case MEM_TYPE_OPCODE_RLDL:
                if (instructionCode_ != 68 && (readMem(scratchpadRegister_[programCounter_]+1)&0xf0) != 0xc0)
                     mem_type = MEM_TYPE_OPCODE;
            break;
        }
    }
    else
        mem_type = MEM_TYPE_OPCODE;
 
    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], mem_type);
    
    scratchpadRegister_[programCounter_]++;

    if (instructionCode_ == 0x68 && (cpuType_ == CPU1805 || cpuType_ == CPU1804))
        cpuState_ = STATE_FETCH_2;
    else
    {
        if ((instructionCode_ & 0xf0) == 0xc0 && cpuType_ > CPU1801)
            cpuState_ = STATE_EXECUTE_1_LBR;
        else
            cpuState_ = STATE_EXECUTE_1;
    }
}

void Cdp1802::cpuCycleFetch2()
{
    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
    instructionCode_ = readMem(scratchpadRegister_[programCounter_]++);
    bus_=instructionCode_;
    
    cpuState_ = STATE_EXECUTE_1_1805;
}

void Cdp1802::cpuCycleExecute1()
{
    wxString buffer;
    Byte i, n;
    int tempWord;
    Byte df1;
    uint64_t executed;

    stopHiddenTrace_ = false;
    startHiddenTrace_ = false;
    
    n = instructionCode_ & 15;
    i = instructionCode_>>4;

    switch(i)
    {
         case 0:
            if (cpuType_ == SYSTEM00)
            {
                bus_=readMem(scratchpadRegister_[n]);
                p_Computer->showDataLeds(bus_);
                if (trace_)
                {
                    buffer.Printf("IDL  R%X",n);
                    traceBuffer_ = traceBuffer_ + buffer;
                }
                if (idle_)
                    traceBuffer_ = ".";
                idle_=1;
            }
            else
            {
                if (n == 0)
                {
                    bus_=readMem(scratchpadRegister_[0]);
                    if (trace_)
                    {
                        traceBuffer_ = traceBuffer_ + "IDL";
                    }
                    if (idle_)
                        traceBuffer_ = ".";
                    idle_=1;
                }
                else
                {
                    if (cpuType_ == CPU1801)
                    {
                        bus_=0;
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        bus_=readMem(scratchpadRegister_[n]);
                        accumulator_=bus_;
                        if (trace_)
                        {
                            buffer.Printf("LDN  R%X   D=%02X",n,accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                }
            }
        break;

        case 1:
            address_=scratchpadRegister_[n];
            scratchpadRegister_[n]++;
            bus_=0;
            if (trace_)
            {
                buffer.Printf("INC  R%X   R%X=%04X",n,n,scratchpadRegister_[n]);
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;

        case 2:
            address_=scratchpadRegister_[n];
            scratchpadRegister_[n]--;
            bus_=0;
            if (trace_)
            {
                buffer.Printf("DEC  R%X   R%X=%04X",n,n,scratchpadRegister_[n]);
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;

        case 3:
            if (n > 7 && cpuType_ == SYSTEM00)
            {
                bus_=0;
                if (trace_)
                {
                    buffer.Printf("Illegal code");
                    traceBuffer_ = traceBuffer_ + buffer;
                }
                break;
            }
            switch(n)
            {
                case 0:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                    if (trace_)
                    {
                        buffer.Printf("BR   %02X",bus_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 1:
                    if (cpuType_ <= CPU1801)
                    {
                        if (cpuType_ == CPU1801)
                        {
                            bus_=0;
                            if (trace_)
                            {
                                buffer.Printf("Illegal code");
                                traceBuffer_ = traceBuffer_ + buffer;
                            }
                        }
                        else
                        {
                            p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                            bus_=readMem(scratchpadRegister_[programCounter_]);
                            p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                            if (accumulator_)
                            {
                                scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                            }
                            else
                                scratchpadRegister_[programCounter_]++;
                            if (trace_)
                            {
                                buffer.Printf("BNZ  %02X",bus_);
                                traceBuffer_ = traceBuffer_ + buffer;
                            }

                        }
                    }
                    else
                    {
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        bus_=readMem(scratchpadRegister_[programCounter_]);
                        p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                        if (flipFlopQ_)
                        {
                            scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                        }
                        else scratchpadRegister_[programCounter_]++;
                        if (trace_)
                        {
                            buffer.Printf("BQ   %02X",bus_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                break;
                case 2:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                    if (!accumulator_)
                    {
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    }
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        buffer.Printf("BZ   %02X",bus_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 3:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                    if (dataFlag_)
                    {
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    }
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-2, &executed) == MEM_TYPE_OPCODE_BPZ)
                                buffer.Printf("BPZ  %02X",bus_);
                        else
                        {
                            if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-2, &executed) == MEM_TYPE_OPCODE_BGE)
                                buffer.Printf("BGE  %02X",bus_);
                            else
                                buffer.Printf("BDF  %02X",bus_);
                        }
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 4:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                    readyToReceiveData[0] = true;
                    if (!(efFlags_ & 1))
                    {
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    }
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        buffer.Printf("B1   %02X",bus_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 5:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                    readyToReceiveData[1] = true;
                    if (!(efFlags_ & 2))
                    {
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    }
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        buffer.Printf("B2   %02X",bus_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 6:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                    readyToReceiveData[2] = true;
                    if (!(efFlags_ & 4))
                    {
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    }
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        buffer.Printf("B3   %02X",bus_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 7:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                    readyToReceiveData[3] = true;
                    if (!(efFlags_ & 8))
                    {
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    }
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        buffer.Printf("B4   %02X",bus_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 8:
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1, &executed) == MEM_TYPE_OPCODE_SKP)
                            buffer.Printf("SKP");
                        else
                            buffer.Printf("NBR");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 9:
                    if (cpuType_ <= CPU1801)
                    {
                        bus_=0;
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        bus_=readMem(scratchpadRegister_[programCounter_]);
                        p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                        if (!flipFlopQ_)
                        {
                            scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                        }
                        else
                            scratchpadRegister_[programCounter_]++;
                        if (trace_)
                        {
                            buffer.Printf("BNQ  %02X",bus_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                break;
                case 0xa:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                    if (accumulator_)
                    {
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    }
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        buffer.Printf("BNZ  %02X",bus_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 0xb:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                    if (!dataFlag_)
                    {
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    }
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-2, &executed) == MEM_TYPE_OPCODE_BM)
                                buffer.Printf("BM   %02X",bus_);
                        else
                        {
                            if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-2, &executed) == MEM_TYPE_OPCODE_BL)
                                buffer.Printf("BL   %02X",bus_);
                            else
                                buffer.Printf("BNF  %02X",bus_);
                        }
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 0xc:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                    readyToReceiveData[0] = true;
                    if (efFlags_ & 1)
                    {
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    }
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        buffer.Printf("BN1  %02X",bus_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 0xd:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                    readyToReceiveData[1] = true;
                    if (efFlags_ & 2)
                    {
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    }
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        buffer.Printf("BN2  %02X",bus_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 0xe:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                    readyToReceiveData[2] = true;
                    if (efFlags_ & 4)
                    {
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    }
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        buffer.Printf("BN3  %02X",bus_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 0xf:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]);
                    p_Computer->writeMemLabelType((scratchpadRegister_[programCounter_]&0xff00) | bus_, LABEL_TYPE_BRANCH);
                    readyToReceiveData[3] = true;
                    if (efFlags_ & 8)
                    {
                        scratchpadRegister_[programCounter_]= (scratchpadRegister_[programCounter_]&0xff00) | bus_;
                    }
                    else
                        scratchpadRegister_[programCounter_]++;
                    if (trace_)
                    {
                        buffer.Printf("BN4  %02X",bus_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
            }
        break;
        case 4:
            bus_=readMem(scratchpadRegister_[n]++);
            accumulator_=bus_;
            if (trace_)
            {
                buffer.Printf("LDA  R%X   D=M(%04X)=%02X",n,scratchpadRegister_[n]-1,accumulator_);
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 5:
            bus_=accumulator_;
            writeMem(scratchpadRegister_[n], accumulator_, false);
            if (trace_)
            {
                buffer.Printf("STR  R%X   M(%04X)=%02X",n,scratchpadRegister_[n],accumulator_);
                 traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 6:
            if (cpuType_ == SYSTEM00)
            {
                switch (n)
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        bus_ = readMem(scratchpadRegister_[dataPointer_]++);
                        p_Computer->out(n, scratchpadRegister_[dataPointer_]-1, bus_);
                        if (trace_)
                        {
                            buffer.Printf("OUT  %X    [%02X]",n,bus_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    break;
                        
                    case 8:
                        bus_= p_Computer->in((Byte)(n-8), scratchpadRegister_[dataPointer_]);
                        writeMem(scratchpadRegister_[dataPointer_], bus_, false);
//                      SYSTEM 00 doesn't load INP byte in D
                        if (trace_)
                        {
                            buffer.Printf("INP       M(%04X)=%02X", scratchpadRegister_[dataPointer_], bus_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    break;
                        
                    default:
                        bus_=0;
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    break;
                }
                break;
            }
            if (n == 0 && cpuType_ != CPU1801)
            {
                bus_=readMem(scratchpadRegister_[dataPointer_]);
                scratchpadRegister_[dataPointer_]++;
                if (trace_)
                {
                    buffer.Printf("IRX       R%X=%04X",dataPointer_,scratchpadRegister_[dataPointer_]);
                    traceBuffer_ = traceBuffer_ + buffer;
                }
                break;
            }
            if (n == 8 && (cpuType_ == CPU1805 || cpuType_ == CPU1804))
            {
                cpuCycleFetch2();
                cpuCycleExecute2_1805();
                return;
            }
            if (n <= 7)
            {
                bus_ = readMem(scratchpadRegister_[dataPointer_]++);
                p_Computer->out(n, scratchpadRegister_[dataPointer_]-1, bus_);
                if (p_Main->getLapTimeTrigger() == (LAPTIME_OUT - 1 + n))
                    p_Main->lapTime();
                if (trace_)
                {
                    if (currentComputerConfiguration.vis1870Configuration.defined)
                    {
                        if (currentComputerConfiguration.vis1870Configuration.outputWrite.portNumber[0] != -1)
                        {
                            if (n==5 && (p_Computer->getOutValue(7) != 0x20) && (p_Computer->getOutValue(7) != 0x30))
                                buffer.Printf("OUT  %X    [%04X]",n,scratchpadRegister_[dataPointer_]-1);
                            else
                                buffer.Printf("OUT  %X    [%02X]",n,bus_);
                        }
                        else
                        {
                            if (n>3)
                                buffer.Printf("OUT  %X    [%04X]",n,scratchpadRegister_[dataPointer_]-1);
                            else
                                buffer.Printf("OUT  %X    [%02X]",n,bus_);
                        }
                    }
                    else
                        buffer.Printf("OUT  %X    [%02X]",n,bus_);

                    traceBuffer_ = traceBuffer_ + buffer;
                }
                break;
            }
            bus_= p_Computer->in((Byte)(n-8), scratchpadRegister_[dataPointer_]);
            writeMem(scratchpadRegister_[dataPointer_], bus_, false);
            if (cpuType_ != CPU1801)  // 1801 doesn't load INP x byte in D
                accumulator_=bus_;
            if (trace_)
            {
                buffer.Printf("INP  %X    D=M(%04X)=%02X",n-8,scratchpadRegister_[dataPointer_], bus_);
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 7:
            switch(n)
            {
                 case 0:
                    bus_=readMem(scratchpadRegister_[dataPointer_]++);
                    programCounter_=bus_ & 15;
                    dataPointer_= (bus_>>4);
                    interruptEnable_=1;
                    if (trace_)
                    {
                        buffer.Printf("RET       P=R%X, X=R%X", programCounter_, dataPointer_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 1:
                    if (cpuType_ == SYSTEM00)
                    {
                        bus_=0;
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        bus_=readMem(scratchpadRegister_[dataPointer_]++);
                        programCounter_=bus_ & 15;
                        dataPointer_=bus_>>4;
                        interruptEnable_=0;
                        if (trace_)
                        {
                            buffer.Printf("DIS       P=R%X, X=R%X", programCounter_, dataPointer_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                break;
                case 2:
                    if (cpuType_ <= CPU1801)
                    {
                        bus_=0;
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        bus_ = readMem(scratchpadRegister_[dataPointer_]++);
                        accumulator_ = bus_;
                        if (trace_)
                        {
                            buffer.Printf("LDXA      D=M(%04X)=%02X", scratchpadRegister_[dataPointer_]-1, accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                break;
                case 3:
                    if (cpuType_ <= CPU1801)
                    {
                        bus_=0;
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        writeMem(scratchpadRegister_[dataPointer_]--, accumulator_, false);
                        bus_=accumulator_;
                        if (trace_)
                        {
                            buffer.Printf("STXD      M(%04X)=%02X",scratchpadRegister_[dataPointer_]+1,accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                break;
                case 4:
                    if (cpuType_ <= CPU1801)
                    {
                        bus_=0;
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        bus_ = readMem(scratchpadRegister_[dataPointer_]);
                        tempWord=accumulator_ + bus_ + dataFlag_;
                        if (tempWord>255)
                        {
                            accumulator_ = tempWord & 255;
                            dataFlag_=1;
                        }
                        else
                        {
                            accumulator_=tempWord;
                            dataFlag_=0;
                        }
                        if (trace_)
                        {
                            buffer.Printf("ADC       D=%02X", accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                break;
                case 5:
                    if (cpuType_ <= CPU1801)
                    {
                        bus_=0;
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        bus_ = readMem(scratchpadRegister_[dataPointer_]);
                        tempWord=bus_ +((~accumulator_)&0xff) + dataFlag_;
                        if (tempWord>255)
                        {
                            accumulator_ = tempWord&255;
                            dataFlag_=1;
                        }
                        else
                        {
                            accumulator_=tempWord&255;
                            dataFlag_=0;
                        }
                        if (trace_)
                        {
                            buffer.Printf("SDB       D=%02X", accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                 break;
                case 6:
                    bus_=0;
                    if (cpuType_ <= CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        df1= (dataFlag_) ? 128 : 0;
                        dataFlag_ = (accumulator_ & 1) ? 1 : 0;
                        accumulator_ = (accumulator_ >> 1) | df1;
                        if (trace_)
                        {
                            if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1, &executed) == MEM_TYPE_OPCODE_RSHR)
                                buffer.Printf("RSHR      D=%02X", accumulator_);
                            else
                                buffer.Printf("SHRC      D=%02X", accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        address_=scratchpadRegister_[dataPointer_];
                   }
                break;
                case 7:
                    if (cpuType_ <= CPU1801)
                    {
                        bus_=0;
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        bus_ = readMem(scratchpadRegister_[dataPointer_]);
                        tempWord=accumulator_ +((~bus_)&0xff) + dataFlag_;
                        if (tempWord>255)
                        {
                            accumulator_ = tempWord & 0xff;
                            dataFlag_=1;
                        }
                        else
                        {
                            accumulator_=tempWord&255;
                            dataFlag_=0;
                        }
                        if (trace_)
                        {
                            buffer.Printf("SMB       D=%02X", accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                break;
                case 8:
                    bus_ = registerT_;
                    writeMem(scratchpadRegister_[dataPointer_], registerT_, false);
                    if (trace_)
                    {
                        buffer.Printf("SAV       M(%04X)=%02X",scratchpadRegister_[dataPointer_], registerT_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 9:
                    if (cpuType_ <= CPU1801)
                    {
                        bus_=0;
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        registerT_= (dataPointer_<<4) | programCounter_;
                        bus_ = registerT_;
                        writeMem(scratchpadRegister_[2]--, registerT_, false);
                        dataPointer_=programCounter_;
                        if (trace_)
                        {
                            buffer.Printf("MARK");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                break;
                case 0xa:
                    bus_=0;
                    if (cpuType_ <= CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        flipFlopQ_=0;
                        if (trace_)
                        {
                            traceBuffer_ = traceBuffer_ + "REQ";
                        }
                        address_=scratchpadRegister_[programCounter_];
                        p_Computer->switchQ(0);
                        psaveAmplitudeChange(0);
                    }
                break;
                case 0xb:
                    bus_=0;
                    if (cpuType_ <= CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        flipFlopQ_=1;
                        if (trace_)
                        {
                            traceBuffer_ = traceBuffer_ + "SEQ";
                        }
                        address_=scratchpadRegister_[programCounter_];
                        p_Computer->switchQ(1);
                        if (p_Main->getLapTimeTrigger() == LAPTIME_Q)
                            p_Main->lapTime();
                        psaveAmplitudeChange(1);
                    }
                break;
                case 0xc:
                    if (cpuType_ <= CPU1801)
                    {
                        bus_=0;
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        bus_=readMem(scratchpadRegister_[programCounter_]++);
                        tempWord=accumulator_ + bus_ + dataFlag_;
                        if (tempWord>255)
                        {
                            accumulator_ = tempWord & 255;
                            dataFlag_=1;
                        }
                        else
                        {
                            accumulator_=tempWord;
                            dataFlag_=0;
                        }
                        if (trace_)
                        {
                            buffer.Printf("ADCI %02X   D=%02X", bus_, accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                break;
                case 0xd:
                    if (cpuType_ <= CPU1801)
                    {
                        bus_=0;
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        bus_=readMem(scratchpadRegister_[programCounter_]++);
                        tempWord=bus_ +((~accumulator_)&0xff) + dataFlag_;
                        if (tempWord>255)
                        {
                            accumulator_ = tempWord & 0xff;
                            dataFlag_=1;
                        }
                        else
                        {
                            accumulator_=tempWord&255;
                            dataFlag_=0;
                        }
                        if (trace_)
                        {
                            buffer.Printf("SDBI %02X   D=%02X", bus_, accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                break;
                case 0xe:
                    bus_=0;
                    if (cpuType_ <= CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        df1= (dataFlag_ & 1) ? 1: 0;
                        dataFlag_ = (accumulator_ & 128) ? 1 : 0;
                        accumulator_ = (accumulator_ << 1) | df1;
                        if (trace_)
                        {
                            if (p_Computer->readMemDataType(scratchpadRegister_[programCounter_]-1, &executed) == MEM_TYPE_OPCODE_RSHL)
                                buffer.Printf("RSHL      D=%02X", accumulator_);
                            else
                                buffer.Printf("SHLC      D=%02X", accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        address_=scratchpadRegister_[programCounter_];
                    }
                break;
                case 0xf:
                    if (cpuType_ <= CPU1801)
                    {
                        bus_=0;
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                        bus_=readMem(scratchpadRegister_[programCounter_]++);
            //            bus_= (~readMem(scratchpadRegister_[programCounter_]++)) & 0xff;
                        tempWord=accumulator_ + (~bus_ & 0xff) + dataFlag_;
                        if (tempWord>255)
                        {
                            accumulator_ = tempWord & 0xff;
                            dataFlag_=1;
                        }
                        else
                        {
                            accumulator_=tempWord&255;
                            dataFlag_=0;
                        }
                        if (trace_)
                        {
                            buffer.Printf("SMBI %02X   D=%02X", bus_, accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                break;
            }
        break;
        case 8:
            bus_= (scratchpadRegister_[n] & 255);
            accumulator_=bus_;
            if (trace_)
            {
                buffer.Printf("GLO  R%X   D=%02X", n, accumulator_);
                traceBuffer_ = traceBuffer_ + buffer;
            }
            address_=scratchpadRegister_[n];
        break;
        case 9:
            bus_= (scratchpadRegister_[n] >> 8);
            accumulator_=bus_;
            if (trace_)
            {
                buffer.Printf("GHI  R%X   D=%02X",n,accumulator_);
                traceBuffer_ = traceBuffer_ + buffer;
            }
            address_=scratchpadRegister_[n];
        break;
        case 0xa:
            bus_=accumulator_;
            scratchpadRegister_[n]= (scratchpadRegister_[n] & 0xff00) | accumulator_;
            if (trace_)
            {
                buffer.Printf("PLO  R%X   R%X=%04X ",n,n,scratchpadRegister_[n]);
                traceBuffer_ = traceBuffer_ + buffer;
            }
            address_=scratchpadRegister_[n];
//            if ((scratchpadRegister_[programCounter_]&0xff00) != 0xA900 && n == 0)
//            {
//                buffer.Printf("%04X: PLO  R%X   R%X=%04X ",scratchpadRegister_[programCounter_], n,n,scratchpadRegister_[n]);
//                p_Main->eventShowTextMessage(buffer);
//            }
        break;
        case 0xb:
            bus_=accumulator_;
            scratchpadRegister_[n]= (scratchpadRegister_[n] & 0x00ff) |(accumulator_<<8);
            if (trace_)
            {
                buffer.Printf("PHI  R%X   R%X=%04X ",n,n,scratchpadRegister_[n]);
                traceBuffer_ = traceBuffer_ + buffer;
            }
            address_=scratchpadRegister_[n];
//            if ((scratchpadRegister_[programCounter_]&0xff00) != 0xA900 && n == 0)
//            {
//                buffer.Printf("%04X: PHI  R%X   R%X=%04X ",scratchpadRegister_[programCounter_], n,n,scratchpadRegister_[n]);
//                p_Main->eventShowTextMessage(buffer);
//            }
        break;
        case 0xc:
            if (cpuType_ == SYSTEM00)
            {
                bus_=accumulator_;
                scratchpadRegister_[n]= (scratchpadRegister_[n] & 0xfff0) | (accumulator_&0xf);
                if (trace_)
                {
                    buffer.Printf("PNI  R%X   R%X=%04X ",n,n,scratchpadRegister_[n]);
                    traceBuffer_ = traceBuffer_ + buffer;
                }
                
            }
            else
            {
                bus_=0;
                if (trace_)
                {
                    buffer.Printf("Illegal code");
                    traceBuffer_ = traceBuffer_ + buffer;
                }
            }
        break;
        case 0xd:
            if (trace_)
            {
                buffer.Printf("SEP  R%X",n);
                traceBuffer_ = traceBuffer_ + buffer;
                
                if (p_Main->getDebugScrtMode())
                {
                    if (n == scrtProgramCounter_ && skipTrace_)
                    {
                        if (programCounter_ == p_Main->getDebugRetReg() && scratchpadRegister_[p_Main->getDebugRetReg()] == p_Main->getDebugRetAddress())
                            stopHiddenTrace_ = true;
                        if (programCounter_ == p_Main->getDebugCallReg() && scratchpadRegister_[p_Main->getDebugCallReg()] == p_Main->getDebugCallAddress())
                            stopHiddenTrace_ = true;
                    }
                    if (n == p_Main->getDebugCallReg() && scratchpadRegister_[n] == p_Main->getDebugCallAddress())
                    {
                        scrtProgramCounter_ = programCounter_;
                        startHiddenTrace_ = true;
                        buffer.Printf("   CALL %02X%02X", readMem(scratchpadRegister_[programCounter_]), readMem(scratchpadRegister_[programCounter_]+1));
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                    if (n == p_Main->getDebugRetReg() && scratchpadRegister_[n] == p_Main->getDebugRetAddress())
                    {
                        scrtProgramCounter_ = programCounter_;
                        startHiddenTrace_ = true;
                        buffer.Printf("   RETURN");
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                }
            }
            bus_=n+16*n;
            programCounter_=n;
            address_=scratchpadRegister_[n];
            p_Computer->writeMemLabelType(address_, LABEL_TYPE_SUB);
        break;
        case 0xe:
            bus_=n+16*n;
            dataPointer_=n;
            if (trace_)
            {
                buffer.Printf("SEX  R%X",n);
                traceBuffer_ = traceBuffer_ + buffer;
            }
            address_=scratchpadRegister_[n];
        break;
        case 0xf:
            if (n > 6 && cpuType_ == SYSTEM00)
            {
                bus_=0;
                if (trace_)
                {
                    buffer.Printf("Illegal code");
                    traceBuffer_ = traceBuffer_ + buffer;
                }
                break;
            }
            switch(n)
            {
                case 0:
                    bus_=readMem(scratchpadRegister_[dataPointer_]);
                    accumulator_=bus_;
                    if (trace_)
                    {
                        buffer.Printf("LDX       D=M(%04X)=%02X",scratchpadRegister_[dataPointer_],accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 1:
                    bus_=readMem(scratchpadRegister_[dataPointer_]);
                    accumulator_=bus_ | accumulator_;
                    if (trace_)
                    {
                        buffer.Printf("OR        D=%02X",accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 2:
                    bus_=readMem(scratchpadRegister_[dataPointer_]);
                    accumulator_=bus_ & accumulator_;
                    if (trace_) {
                        buffer.Printf("AND       D=%02X",accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 3:
                    bus_=readMem(scratchpadRegister_[dataPointer_]);
                    accumulator_=bus_ ^ accumulator_;
                    if (trace_)
                    {
                        buffer.Printf("XOR       D=%02X",accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 4:
                    bus_=readMem(scratchpadRegister_[dataPointer_]);
                    tempWord=accumulator_ + bus_;
                    if (tempWord>255)
                    {
                        accumulator_ = tempWord & 255;
                        dataFlag_=1;
                    }
                    else
                    {
                        accumulator_=tempWord;
                        dataFlag_=0;
                    }
                    if (trace_)
                    {
                        buffer.Printf("ADD       D=%02X",accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 5:
                    bus_=readMem(scratchpadRegister_[dataPointer_]);
                    tempWord=bus_ +((~accumulator_)&0xff)+1;
                    if (tempWord>255)
                    {
                        accumulator_ = tempWord & 0xff;
                        dataFlag_=1;
                    }
                    else
                    {
                        accumulator_=tempWord&255;
                        dataFlag_=0;
                    }
                    if (trace_)
                    {
                        buffer.Printf("SD        D=%02X", accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                 break;
                case 6:
                    bus_=0;
                    dataFlag_= (accumulator_ & 1)? 1 : 0;
                    accumulator_=accumulator_>>1;
                    if (trace_)
                    {
                        buffer.Printf("SHR       D=%02X",accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                    address_=scratchpadRegister_[dataPointer_];
                break;
                case 7:
                    bus_=readMem(scratchpadRegister_[dataPointer_]);
                    tempWord=accumulator_+((~bus_)&0xff)+1;
                    if (tempWord>255)
                    {
                        accumulator_ = tempWord & 0xff;
                        dataFlag_=1;
                    }
                    else
                    {
                        accumulator_=tempWord&255;
                        dataFlag_=0;
                    }
                    if (trace_)
                    {
                        buffer.Printf("SM        D=%02X", accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 8:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]++);
                    accumulator_=bus_;
                    if (trace_)
                    {
                        buffer.Printf("LDI  %02X   D=%02X", accumulator_, accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 9:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]++);
                    accumulator_ |= bus_;
                    if (trace_)
                    {
                        buffer.Printf("ORI  %02X   D=%02X", bus_, accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 10:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]++);
                    accumulator_ &= bus_;
                    if (trace_)
                    {
                        buffer.Printf("ANI  %02X   D=%02X",bus_,accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 11:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]++);
                    accumulator_ ^= bus_;
                    if (trace_)
                    {
                        buffer.Printf("XRI  %02X   D=%02X",bus_,accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 12:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]++);
                    tempWord=accumulator_ + bus_;
                    if (tempWord>255)
                    {
                        accumulator_ = tempWord & 255;
                        dataFlag_=1;
                    }
                    else
                    {
                        accumulator_=tempWord;
                        dataFlag_=0;
                    }
                    if (trace_)
                    {
                        buffer.Printf("ADI  %02X   D=%02X",bus_,accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                 break;
                case 13:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]++);
                    tempWord=bus_+((~accumulator_)&0xff)+1;
                    if (tempWord>255)
                    {
                        accumulator_ = tempWord&0xff;
                        dataFlag_=1;
                    }
                    else
                    {
                        accumulator_=tempWord&255;
                        dataFlag_=0;
                    }
                    if (trace_)
                    {
                        buffer.Printf("SDI  %02X   D=%02X",bus_, accumulator_);
                        traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
                case 14:
                    bus_=0;
                    if (cpuType_ == CPU1801)
                    {
                        if (trace_)
                        {
                            buffer.Printf("Illegal code");
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                    }
                    else
                    {
                        dataFlag_= (accumulator_ & 128)? 1 : 0;
                        accumulator_=accumulator_<<1;
                        if (trace_)
                        {
                            buffer.Printf("SHL       D=%02X",accumulator_);
                            traceBuffer_ = traceBuffer_ + buffer;
                        }
                        address_=scratchpadRegister_[programCounter_];
                    }
                break;
                case 15:
                    p_Computer->writeMemDataType(scratchpadRegister_[programCounter_], MEM_TYPE_OPERAND);
                    bus_=readMem(scratchpadRegister_[programCounter_]++);
                    tempWord=accumulator_+(~bus_&0xff)+1;
                    if (tempWord>255)
                    {
                        accumulator_ = tempWord&0xff;
                        dataFlag_=1;
                    }
                    else
                    {
                        accumulator_=tempWord&255;
                        dataFlag_=0;
                    }
                    if (trace_)
                    {
                        buffer.Printf("SMI  %02X   D=%02X",bus_,accumulator_);
    //                                     XXX       R
                         traceBuffer_ = traceBuffer_ + buffer;
                    }
                break;
            }
        break;
    }
    cpuCycleFinalize();
}

void Cdp1802::cpuCycleExecute1_LBR()
{
    Byte n = instructionCode_ & 0xf;

    address_ = scratchpadRegister_[programCounter_];
    bus_ = readMem(address_);

    switch(n)
    {
        case 0:
            setRegisterB(bus_);
            scratchpadRegister_[programCounter_]++;
        break;
            
        case 1:
            if (flipFlopQ_)
                setRegisterB(bus_);
            scratchpadRegister_[programCounter_]++;
        break;

        case 2:
            if (!accumulator_)
                setRegisterB(bus_);
            scratchpadRegister_[programCounter_]++;
        break;

        case 3:
            if (dataFlag_)
                setRegisterB(bus_);
            scratchpadRegister_[programCounter_]++;
        break;
            
        case 5:
            if (!flipFlopQ_)
                scratchpadRegister_[programCounter_]++;
        break;

        case 6:
            if (accumulator_)
                scratchpadRegister_[programCounter_]++;
        break;
            
        case 7:
            if (!dataFlag_)
                scratchpadRegister_[programCounter_]++;
        break;
            
        case 8:
            scratchpadRegister_[programCounter_]++;
        break;
        
        case 9:
            if (!flipFlopQ_)
                setRegisterB(bus_);
            scratchpadRegister_[programCounter_]++;
        break;
            
        case 0xa:
            if (accumulator_)
                setRegisterB(bus_);
            scratchpadRegister_[programCounter_]++;
        break;
            
        case 0xb:
            if (!dataFlag_)
                setRegisterB(bus_);
            scratchpadRegister_[programCounter_]++;
        break;

        case 0xc:
            if (interruptEnable_)
                scratchpadRegister_[programCounter_]++;
        break;
            
        case 0xd:
            if (flipFlopQ_)
                scratchpadRegister_[programCounter_]++;
        break;
            
        case 0xe:
            if (!accumulator_)
                scratchpadRegister_[programCounter_]++;
        break;
            
        case 0xf:
            if (dataFlag_)
                scratchpadRegister_[programCounter_]++;
        break;

    }
    cpuState_ = STATE_EXECUTE_2_LBR;
}

void Cdp1802::cpuCycleExecute2_LBR()
{
    wxString buffer;
    uint64_t executed;

    Byte n = instructionCode_ & 0xf;
    
    Word instructionAddress = address_-1;
    Word firstOperandAddress = address_;
    Word secondOperandAddress = address_+1;

    Byte highByteBranchAddress=bus_;
    Byte lowByteBranchAddress=readMem(secondOperandAddress);
    Word branchAddress=(registerB_<<8)|lowByteBranchAddress;

    switch(n)
    {
        case 0:
            if (p_Computer->readMemDataType(instructionAddress, &executed) != MEM_TYPE_OPCODE_LBR_SLOT)
            {
                p_Computer->writeMemDataType(firstOperandAddress, MEM_TYPE_OPERAND);
                p_Computer->writeMemDataType(secondOperandAddress, MEM_TYPE_OPERAND);
            }
            bus_=lowByteBranchAddress;
            scratchpadRegister_[programCounter_]=branchAddress;
            address_++;
            p_Computer->writeMemLabelType(branchAddress, LABEL_TYPE_JUMP);
            if (trace_)
            {
                buffer.Printf("LBR  %02X%02X",highByteBranchAddress,lowByteBranchAddress);
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 1:
            if (p_Computer->readMemDataType(instructionAddress, &executed) != MEM_TYPE_OPCODE_LBR_SLOT)
            {
                p_Computer->writeMemDataType(firstOperandAddress, MEM_TYPE_OPERAND);
                p_Computer->writeMemDataType(secondOperandAddress, MEM_TYPE_OPERAND);
            }
            bus_=lowByteBranchAddress;
            if (flipFlopQ_)
                scratchpadRegister_[programCounter_]=branchAddress;
            else
                scratchpadRegister_[programCounter_]++;
            address_++;
            p_Computer->writeMemLabelType(branchAddress, LABEL_TYPE_JUMP);
            if (trace_)
            {
                buffer.Printf("LBQ  %02X%02X",highByteBranchAddress,lowByteBranchAddress);
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 2:
            if (p_Computer->readMemDataType(instructionAddress, &executed) != MEM_TYPE_OPCODE_LBR_SLOT)
            {
                p_Computer->writeMemDataType(firstOperandAddress, MEM_TYPE_OPERAND);
                p_Computer->writeMemDataType(secondOperandAddress, MEM_TYPE_OPERAND);
            }
            bus_=lowByteBranchAddress;
            if (!accumulator_)
                scratchpadRegister_[programCounter_]=branchAddress;
            else
                scratchpadRegister_[programCounter_]++;
            address_++;
            p_Computer->writeMemLabelType(branchAddress, LABEL_TYPE_JUMP);
            if (trace_)
            {
                buffer.Printf("LBZ  %02X%02X",highByteBranchAddress,lowByteBranchAddress);
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 3:
            if (p_Computer->readMemDataType(instructionAddress, &executed) != MEM_TYPE_OPCODE_LBR_SLOT)
            {
                p_Computer->writeMemDataType(firstOperandAddress, MEM_TYPE_OPERAND);
                p_Computer->writeMemDataType(secondOperandAddress, MEM_TYPE_OPERAND);
            }
            bus_=lowByteBranchAddress;
            if (dataFlag_)
                scratchpadRegister_[programCounter_]=branchAddress;
            else
                scratchpadRegister_[programCounter_]++;
            address_++;
            p_Computer->writeMemLabelType(branchAddress, LABEL_TYPE_JUMP);
            if (trace_)
            {
                buffer.Printf("LBDF %02X%02X",highByteBranchAddress,lowByteBranchAddress);
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 4:
            if (trace_)
            {
                buffer.Printf("NOP");
                traceBuffer_ = traceBuffer_ + buffer;
            }
            address_--;
        break;
        case 5:
            if (!flipFlopQ_)
            {
                scratchpadRegister_[programCounter_]++;
                address_++;
                bus_=lowByteBranchAddress;
            }
            if (trace_)
            {
                buffer.Printf("LSNQ");
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 6:
            if (accumulator_)
            {
                scratchpadRegister_[programCounter_]++;
                address_++;
                bus_=lowByteBranchAddress;
            }
            if (trace_)
            {
                buffer.Printf("LSNZ");
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 7:
            if (!dataFlag_)
            {
                scratchpadRegister_[programCounter_]++;
                address_++;
                bus_=lowByteBranchAddress;
            }
            if (trace_)
            {
                buffer.Printf("LSNF");
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 8:
            scratchpadRegister_[programCounter_]++;
            address_++;
            bus_=lowByteBranchAddress;
            if (trace_)
            {
                if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_LSKP)
                    buffer.Printf("LSKP");
                else
                    buffer.Printf("NLBR");
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 9:
            if (p_Computer->readMemDataType(instructionAddress, &executed) != MEM_TYPE_OPCODE_LBR_SLOT)
            {
                p_Computer->writeMemDataType(firstOperandAddress, MEM_TYPE_OPERAND);
                p_Computer->writeMemDataType(secondOperandAddress, MEM_TYPE_OPERAND);
            }
            bus_=lowByteBranchAddress;
            if (!flipFlopQ_)
                scratchpadRegister_[programCounter_]=branchAddress;
            else
                scratchpadRegister_[programCounter_]++;
            address_++;
            p_Computer->writeMemLabelType(branchAddress, LABEL_TYPE_JUMP);
            if (trace_)
            {
                buffer.Printf("LBNQ %02X%02X",highByteBranchAddress,lowByteBranchAddress);
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 0xa:
            if (p_Computer->readMemDataType(instructionAddress, &executed) != MEM_TYPE_OPCODE_LBR_SLOT)
            {
                p_Computer->writeMemDataType(firstOperandAddress, MEM_TYPE_OPERAND);
                p_Computer->writeMemDataType(secondOperandAddress, MEM_TYPE_OPERAND);
            }
            bus_=lowByteBranchAddress;
            if (accumulator_)
                scratchpadRegister_[programCounter_]=branchAddress;
            else
                scratchpadRegister_[programCounter_]++;
            address_++;
            p_Computer->writeMemLabelType(branchAddress, LABEL_TYPE_JUMP);
            if (trace_)
            {
                buffer.Printf("LBNZ %02X%02X",highByteBranchAddress,lowByteBranchAddress);
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 0xb:
            if (p_Computer->readMemDataType(instructionAddress, &executed) != MEM_TYPE_OPCODE_LBR_SLOT)
            {
                p_Computer->writeMemDataType(firstOperandAddress, MEM_TYPE_OPERAND);
                p_Computer->writeMemDataType(secondOperandAddress, MEM_TYPE_OPERAND);
            }
            bus_=lowByteBranchAddress;
            if (!dataFlag_)
                scratchpadRegister_[programCounter_]=branchAddress;
            else
                scratchpadRegister_[programCounter_]++;
            address_++;
            p_Computer->writeMemLabelType(branchAddress, LABEL_TYPE_JUMP);
            if (trace_)
            {
                buffer.Printf("LBNF %02X%02X",highByteBranchAddress,lowByteBranchAddress);
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 0xc:
            if (interruptEnable_)
            {
                scratchpadRegister_[programCounter_]++;
                address_++;
                bus_=lowByteBranchAddress;
            }
            if (trace_)
            {
                buffer.Printf("LSIE");
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 0xd:
            if (flipFlopQ_)
            {
                scratchpadRegister_[programCounter_]++;
                address_++;
                bus_=lowByteBranchAddress;
            }
            if (trace_)
            {
                buffer.Printf("LSQ");
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 0xe:
            if (!accumulator_)
            {
                scratchpadRegister_[programCounter_]++;
                address_++;
                bus_=lowByteBranchAddress;
            }
            if (trace_)
            {
                buffer.Printf("LSZ");
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
        case 0xf:
            if (dataFlag_)
            {
                scratchpadRegister_[programCounter_]++;
                address_++;
                bus_=lowByteBranchAddress;
            }
            if (trace_)
            {
                buffer.Printf("LSDF");
                traceBuffer_ = traceBuffer_ + buffer;
            }
        break;
    }
    cpuCycleFinalize();
}

void Cdp1802::cpuCycleFinalize()
{
    if (!idle_)
    {
        if (trace_ && !skipTrace_)
            p_Main->debugTrace(traceBuffer_);

        cpuState_ = STATE_FETCH_1;
    }
    else
    {
        if (trace_ && !skipTrace_ && traceBuffer_ != ".")
            p_Main->debugTrace(traceBuffer_);

        machineCycle();
        cpuCycles_ ++;
        if (currentComputerConfiguration.vip2KVideoConfiguration.defined)
            skipMachineCycleAfterIdle_=true;

        if (cpuState_ != STATE_EXECUTE_1) 
            cpuState_ = STATE_FETCH_1;
        else
        {
            if (steps_ != 0)
                playSaveLoad();
        }
    }
    if (stopHiddenTrace_)
        skipTrace_ = false;
    if (startHiddenTrace_)
        skipTrace_ = true;
}

bool Cdp1802::readIntelFile(wxString fileName, int memoryType, long end, bool showFilename)
{
    wxTextFile inFile;
    wxString line, strValue;
    long count;
    long address;
    long value;
    int spaces;
    bool overloaded = false;
    Word start = 0xffff;
    Word last = 0;

    if (inFile.Open(fileName))
    {
        for (line=inFile.GetFirstLine(); !inFile.Eof(); line=inFile.GetNextLine())
        {
            line.Replace("O", "0");
            line.Replace("I", "1");
            line.Replace("l", "1");
            spaces = 0;
            int maxSpaces = 6;
            if (line.Len() < 6)  maxSpaces = (int)line.Len();
            for (int i=0; i<maxSpaces; i++) if (line[i] == 32) spaces++;
            if (spaces == 0)
            {
                strValue = line.Mid(1, 2);
                if (!strValue.ToLong(&count, 16))
                    count = 0;

                strValue = line.Mid(3, 4);
                strValue.ToLong(&address, 16);

                strValue = line.Mid(7, 2);
                strValue.ToLong(&value, 16);

                if (value == 1)
                {
                    inFile.Close();
                    if (overloaded)
                    {
                        wxString endStr;
                        endStr.Printf("%04X", (int)end);
                        p_Main->errorMessage("Attempt to load after address " + endStr);
                    }
                    setAddress(showFilename, start, last-1);
                    return true;
                }
                if (address < start)
                    start = address;
                for (int i=0; i<count; i++)
                {
                    strValue = line.Mid((i*2)+9, 2);
                    strValue.ToLong(&value, 16);
                    if ((memoryType&0xff) != NOCHANGE && (memoryType&0xff) != RAM)
                        defineMemoryType(address, memoryType);
        //            if (address < end)
                    writeMemDebug(address,(Byte)value, true);
        //            else
        //                overloaded = true;
                    address++;
                }
                if (address > last)
                    last = address;
            }
            else
            {
                strValue = line.Mid(1, 4);
                strValue.ToLong(&address, 16);
                for (size_t i=5; i<line.Len(); i++)
                {
                    if ((line[i] >= '0' && line [i] <= '9') ||
                        (line[i] >= 'A' && line [i] <= 'F') ||
                        (line[i] >= 'a' && line [i] <= 'f'))
                    {
                        strValue = line.Mid(i, 2);
                        if (strValue.ToLong(&value, 16))
                        {
                            value &= 255;
                            if ((memoryType&0xff) != NOCHANGE && (memoryType&0xff) != RAM)
                                defineMemoryType(address, memoryType);
            //                if (address < end)
                            writeMemDebug(address,(Byte)value, true);
            //                else
            //                    overloaded = true;
                            address++;
                            i++;
                        }
                    }
                }
            }
        }
        inFile.Close();
        if (overloaded)
        {
            wxString endStr;
            endStr.Printf("%04X", (int)end);
            p_Main->errorMessage("Attempt to load after address " + endStr);
        }
        setAddress(showFilename, start, last-1);
        return true;
    }
    else
    {
        p_Main->errorMessage("Error reading " + fileName);
        return false;
    }
}

bool Cdp1802::readIntelFile(wxString fileName, MemoryDefinition* memoryDefintion, long romSize)
{
    wxTextFile inFile;
    wxString line, strValue;
    long count;
    long address;
    long value;
    int spaces;
    Word start = 0xffff;
    Word last = 0;

    if (romSize != -1)
        memoryDefintion->data.resize(romSize);
    
    if (inFile.Open(fileName))
    {
        for (line=inFile.GetFirstLine(); !inFile.Eof(); line=inFile.GetNextLine())
        {
            line.Replace("O", "0");
            line.Replace("I", "1");
            line.Replace("l", "1");
            spaces = 0;
            int maxSpaces = 6;
            if (line.Len() < 6)  maxSpaces = (int)line.Len();
            for (int i=0; i<maxSpaces; i++) if (line[i] == 32) spaces++;
            if (spaces == 0)
            {
                strValue = line.Mid(1, 2);
                if (!strValue.ToLong(&count, 16))
                    count = 0;

                strValue = line.Mid(3, 4);
                strValue.ToLong(&address, 16);

                strValue = line.Mid(7, 2);
                strValue.ToLong(&value, 16);

                if (value == 1)
                {
                    inFile.Close();
                    memoryDefintion->mask = setRomMask(romSize);
                    return true;
                }
                if (address < start)
                    start = address;
                for (int i=0; i<count; i++)
                {
                    strValue = line.Mid((i*2)+9, 2);
                    strValue.ToLong(&value, 16);
                    if (address >= romSize)
                    {
                        romSize = address + 1;
                        memoryDefintion->data.resize(romSize);
                    }
                    memoryDefintion->data[address] = value;
                    address++;
                }
                if (address > last)
                    last = address;
            }
            else
            {
                strValue = line.Mid(1, 4);
                strValue.ToLong(&address, 16);
                for (size_t i=5; i<line.Len(); i++)
                {
                    if ((line[i] >= '0' && line [i] <= '9') ||
                        (line[i] >= 'A' && line [i] <= 'F') ||
                        (line[i] >= 'a' && line [i] <= 'f'))
                    {
                        strValue = line.Mid(i, 2);
                        if (strValue.ToLong(&value, 16))
                        {
                            value &= 255;
                            if (address >= romSize)
                            {
                                romSize = address + 1;
                                memoryDefintion->data.resize(romSize);
                            }
                            memoryDefintion->data[address] = value;
                            address++;
                            i++;
                        }
                    }
                }
            }
        }
        inFile.Close();
        memoryDefintion->mask = setRomMask(romSize);
        return true;
    }
    else
    {
        p_Main->errorMessage("Error reading " + fileName);
        return false;
    }
}

bool Cdp1802::readLstFile(wxString fileName, int memoryType, long end, bool showFilename)
{
    wxTextFile inFile;
    wxString line, strValue;
    long address = 0;
    long value;
    bool overloaded = false;
    Word start = 0xffff;
    Word last = 0;

    if (inFile.Open(fileName))
    {
        for (line=inFile.GetFirstLine(); !inFile.Eof(); line=inFile.GetNextLine())
        {
            line.Replace("O", "0");
            line.Replace("I", "1");
            line.Replace("l", "1");
            if (line.GetChar(0) != ' ')
            {
                strValue = line.Mid(0, 4);
                strValue.ToLong(&address, 16);
            }

            if (line.GetChar(4) != ' ')
            {
                size_t i=6;
                value = 0;
                Byte bit = 0x80;
                while ((line.GetChar(i) == ' ') || (line.GetChar(i) == 'x'))
                {
                    if (line.GetChar(i) == 'x')
                        value |= bit;
                    bit >>= 1;
                    i++;
                }
                if ((memoryType&0xff) != NOCHANGE && (memoryType&0xff) != RAM)
                    defineMemoryType(address, memoryType);
                writeMemDebug(address,(Byte)value, true);
                address++;
/*                size_t i=6;
                for (int j=0; j<10;j++)
                {
                    value = 0;
                    Byte bit = 0x80;
                    while ((line.GetChar(i) == '0') || (line.GetChar(i) == '1'))
                    {
                        if (line.GetChar(i) == '1')
                            value |= bit;
                        bit >>= 1;
                        i++;
                    }
                    defineMemoryType(address, memoryType);
                    writeMem(address,(Byte)value, true);
                    address++;
                    i+=2;
                }*/
            }
            else
            {
                size_t i=5;
                while (line.GetChar(i) != ' ')
                {
                    strValue = line.Mid(i, 2);
                    if (strValue.ToLong(&value, 16))
                    {
                        value &= 255;
                        if ((memoryType&0xff) != NOCHANGE && (memoryType&0xff) != RAM)
                            defineMemoryType(address, memoryType);
                        if (address < end)
                            writeMemDebug(address,(Byte)value, true);
                        else
                            overloaded = true;
                        address++;
                        i+=2;
                    }
                }
            }
        }
        inFile.Close();
        if (overloaded)
        {
            wxString endStr;
            endStr.Printf("%04X", (int)end);
            p_Main->errorMessage("Attempt to load after address " + endStr);
        }
        setAddress(showFilename, start, last);
        return true;
    }
    else
    {
        p_Main->errorMessage("Error reading " + fileName);
        return false;
    }
}

long Cdp1802::setRomMask(long romSize)
{
    long mask=1;
    while (mask < (romSize-1))
        mask = mask | (mask << 1);
    return mask;
}

void Cdp1802::saveIntelFile(wxString fileName, long start, long end)
{
    wxTextFile outputFile;
    wxString line, byteStr;
    int checkSum;

    if (!fileName.empty())
    {
        if (wxFile::Exists(fileName))
        {
            outputFile.Open(fileName);
            outputFile.Clear();
        }
        else
            outputFile.Create(fileName);

        int blockLength;
        while (start <= end)
        {
            blockLength = (int)(end-start+1);
            if (blockLength > 16)
                blockLength = 16;
            line.Printf(":%02X%04X%02X", blockLength, (int)start, 0x00);
            checkSum = blockLength+((start>>8)&0xff)+(start&0xff);
            for (int i = 0; i<blockLength; i++)
//            line.Printf(":%02X%04X%02X", 0x10, (int)start, 0x00);
//            checkSum = 0x10+((start>>8)&0xff)+(start&0xff);
//            for (int i = 0; i<16; i++)
            {
                checkSum += readMem(start);
                byteStr.Printf("%02X", readMem(start));
                line += byteStr;
                start++;
            }
            checkSum = ((checkSum ^ 0xff) + 1) & 0xff;
            byteStr.Printf("%02X", checkSum);
            line += byteStr;
            outputFile.AddLine(line);
        }
        outputFile.AddLine(":00000001FF");
        outputFile.Write();
    }
}

void Cdp1802::saveBinFile(wxString fileName, long start, long end)
{
    wxFile outputFile;
    size_t length;
    char buffer[65536];

    if (!fileName.empty())
    {
        if (outputFile.Create(fileName, true))
        {
            length = end-start+1;
            for (size_t i=0; i<length; i++)
            {
                buffer[i] = p_Computer->readMem(start);
                start++;
            }
            outputFile.Write(buffer, length);
            outputFile.Close();
        }
        else
        {
            p_Main->errorMessage("Error writing " + fileName);
            return;
        }
    }
}

bool Cdp1802::readBinFile(wxString fileName, int memoryType, Word address, long end, bool showFilename, bool showAddressPopup, Word specifiedStartAddress)
{
    wxFFile inFile;
    size_t length;
    char buffer[65535];
    bool overloaded = false;
    Word start;

    start = address;
    if (showAddressPopup && specifiedStartAddress != 0)
    {
        p_Main->eventShowAddressPopup(specifiedStartAddress);

        int answer = p_Main->getAddressPopupAnswer();
#if defined (__WXMAC__)
        if (answer == wxID_YES)
            start = specifiedStartAddress;
#else
        if (answer == wxID_NO)
            start = specifiedStartAddress;
#endif
        if (answer == wxID_CANCEL)
            return false;
    }

    address = start;
    
    if (inFile.Open(fileName, _("rb")))
    {
        length = inFile.Read(buffer, 65535);
        for (size_t i=0; i<length; i++)
        {
            if ((memoryType&0xff) != NOCHANGE && (memoryType&0xff) != RAM)
                defineMemoryType(address, memoryType);
            if (address < end)
                writeMemDebug(address,(Byte)buffer[i], true);
            else
                overloaded = true;
            address++;
        }
        inFile.Close();
        if (overloaded)
        {
            wxString endStr;
            endStr.Printf("%04X", (int)end);
            p_Main->errorMessage("Attempt to load after address " + endStr);
        }
        setAddress(showFilename, start, address-1);
        return true;
    }
    else
    {
        p_Main->errorMessage("Error reading " + fileName);
        return false;
    }
}

bool Cdp1802::readBinFile(wxString fileName, int memoryType, Word address, long end, LoadOffSet loadOffSet, bool showFilename, bool showAddressPopup, Word specifiedStartAddress)
{
    wxFFile inFile;
    size_t length;
    char buffer[65535];
    bool overloaded = false;
    Word start;

    start = address;
    if (showAddressPopup && specifiedStartAddress != 0)
    {
        p_Main->eventShowAddressPopup(specifiedStartAddress);

        int answer = p_Main->getAddressPopupAnswer();
#if defined (__WXMAC__)
        if (answer == wxID_YES)
            start = specifiedStartAddress;
#else
        if (answer == wxID_NO)
            start = specifiedStartAddress;
#endif
        if (answer == wxID_CANCEL)
            return false;
    }

    address = start;
    
    if (inFile.Open(fileName, _("rb")))
    {
        length = inFile.Read(buffer, 65535);
        for (size_t i=0; i<length; i++)
        {
            if ((memoryType&0xff) != NOCHANGE && (memoryType&0xff) != RAM)
                defineMemoryType(address, memoryType);
            if (address < end)
                writeMemDebug(address,(Byte)buffer[i], true);
            else
                overloaded = true;
            address++;
            
            if (address == loadOffSet.addressStart)
                address += loadOffSet.offSet;
        }
        inFile.Close();
        if (overloaded)
        {
            wxString endStr;
            endStr.Printf("%04X", (int)end);
            p_Main->errorMessage("Attempt to load after address " + endStr);
        }
        setAddress(showFilename, start, address-1);
        return true;
    }
    else
    {
        p_Main->errorMessage("Error reading " + fileName);
        return false;
    }
}

bool Cdp1802::readRomMapperBinFile(size_t emsNumber, wxString fileName)
{
    wxFFile inFile;
    wxFileOffset length=0;

    if (!inFile.Open(fileName, _("rb")))
    {
        p_Main->errorMessage("Error reading " + fileName);
        return false;
    }
    
    length = inFile.Length();
    allocRomMapperMemory(emsNumber, length);

    inFile.Read(emsMemory_[emsNumber].mainMem, (size_t)length);
    
    inFile.Close();
    return true;
}

bool Cdp1802::readMultiCartBinFile(wxString dirName, wxString fileName)
{
    wxFFile inFile;
    size_t length=0;
    
    if (!inFile.Open(dirName + fileName, _("rb")))
    {
        p_Main->errorMessage("Error reading " + dirName + fileName);
        return false;
    }
    
    wxFileName FullPath = wxFileName(dirName + fileName, wxPATH_NATIVE);
    wxString ext = FullPath.GetExt();
    
    if (fileName == "" || ext == "st2" || ext == "ch8")
    {
        p_Main->errorMessage("No multi-cart ROM detected in CART");
        return false;
    }
 
    length = (size_t)inFile.Length();
    size_t allocSize = allocMultiCartMemory(length);
    
    if (allocSize < length)
        length = allocSize;
    
    inFile.Read(multiCartRom_, (size_t)length);
    
    inFile.Close();
    return true;
}

void Cdp1802::setAddress(bool showFilename, Word start, Word end)
{
    if (showFilename)
    {
        wxString valueString;
        
        if (currentComputerConfiguration.addressLocationConfiguration.code_start_high != -1 && currentComputerConfiguration.addressLocationConfiguration.code_start_low != -1)
        {
            writeMem(currentComputerConfiguration.addressLocationConfiguration.code_start_high, (start>>8)&0xff, false);
            writeMem(currentComputerConfiguration.addressLocationConfiguration.code_start_low, start&0xff, false);
        }
        if (currentComputerConfiguration.addressLocationConfiguration.code_end_high != -1 && currentComputerConfiguration.addressLocationConfiguration.code_end_low != -1)
        {
            writeMem(currentComputerConfiguration.addressLocationConfiguration.code_end_high, (end>>8)&0xff, false);
            writeMem(currentComputerConfiguration.addressLocationConfiguration.code_end_low, end&0xff, false);
        }
        p_Main->eventSaveStart(start);
        p_Main->eventSaveEnd(end);
    }
}
 
bool Cdp1802::readProgram(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename)
{
    if (rom.Len() != 0)
    {
        return readFile(romDir+rom, memoryType, address, 0x10000, showFilename);
    }
    else return false;
}

bool Cdp1802::readProgram(wxString romDir, wxString rom, int memoryType, Word address, LoadOffSet loadOffSet, bool showFilename)
{
    if (rom.Len() != 0)
    {
        return readFile(romDir+rom, memoryType, address, 0x10000, loadOffSet, showFilename);
    }
    else return false;
}

bool Cdp1802::readProgramCidelsa(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename)
{
    bool ret;

    if ((rom == "des a 2.ic4") || (rom == "des b 2.ic5") || (rom == "des c 2.ic6") || (rom == "des d 2.ic7"))
    {
        ret = readFile(romDir + "des a 2.ic4", memoryType, address, 0x10000, showFilename);
        ret = ret | readFile(romDir + "des b 2.ic5", memoryType, address+0x800, 0x10000, showFilename);
        ret = ret | readFile(romDir + "des c 2.ic6", memoryType, address+0x1000, 0x10000, showFilename);
        ret = ret | readFile(romDir + "des d 2.ic7", memoryType, address+0x1800, 0x10000, showFilename);
        return ret;
    }

    if ((rom == "destryea_1") || (rom == "destryea_2") || (rom == "destryea_3") || (rom == "destryea_4"))
    {
        ret = readFile(romDir + "destryea_1", memoryType, address, 0x10000, showFilename);
        ret = ret | readFile(romDir + "destryea_2", memoryType, address+0x800, 0x10000, showFilename);
        ret = ret | readFile(romDir + "destryea_3" , memoryType, address+0x1000, 0x10000, showFilename);
        ret = ret | readFile(romDir + "destryea_4", memoryType, address+0x1800, 0x10000, showFilename);
        return ret;
    }

    if ((rom == "alt a 1.ic7") || (rom == "alt b 1.ic8") || (rom == "alt c 1.ic9") || (rom == "alt d 1.ic10") || (rom == "alt e 1.ic11") || (rom == "alt f 1.ic12"))
    {
        ret = readFile(romDir + "alt a 1.ic7", memoryType, address, 0x10000, showFilename);
        ret = ret | readFile(romDir + "alt b 1.ic8", memoryType, address+0x800, 0x10000, showFilename);
        ret = ret | readFile(romDir + "alt c 1.ic9" , memoryType, address+0x1000, 0x10000, showFilename);
        ret = ret | readFile(romDir + "alt d 1.ic10" , memoryType, address+0x1800, 0x10000, showFilename);
        ret = ret | readFile(romDir + "alt e 1.ic11" , memoryType, address+0x2000, 0x10000, showFilename);
        ret = ret | readFile(romDir + "alt f 1.ic12", memoryType, address+0x2800, 0x10000, showFilename);
        return ret;
    }

    if ((rom == "dra a 1.ic10") || (rom == "dra b 1.ic11") || (rom == "dra c 1.ic12") || (rom == "dra d 1.ic13") || (rom == "dra e 1.ic14") || (rom == "dra f 1.ic15") || (rom == "dra g 1.ic16") || (rom == "dra h 1.ic17"))
    {
        ret = readFile(romDir + "dra a 1.ic10", memoryType, address, 0x10000, showFilename);
        ret = ret | readFile(romDir + "dra b 1.ic11", memoryType, address+0x800, 0x10000, showFilename);
        ret = ret | readFile(romDir + "dra c 1.ic12" , memoryType, address+0x1000, 0x10000, showFilename);
        ret = ret | readFile(romDir + "dra d 1.ic13" , memoryType, address+0x1800, 0x10000, showFilename);
        ret = ret | readFile(romDir + "dra e 1.ic14" , memoryType, address+0x2000, 0x10000, showFilename);
        ret = ret | readFile(romDir + "dra f 1.ic15" , memoryType, address+0x2800, 0x10000, showFilename);
        ret = ret | readFile(romDir + "dra g 1.ic16" , memoryType, address+0x3000, 0x10000, showFilename);
        ret = ret | readFile(romDir + "dra h 1.ic17", memoryType, address+0x3800, 0x10000, showFilename);
        return ret;
    }

    return readProgram(romDir, rom, memoryType, address, showFilename);
}

bool Cdp1802::readProgramTmc600(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename)
{
    bool ret;

    if ((rom == "sb30") || (rom == "sb31") || (rom == "sb32") || (rom == "sb33") || (rom == "151182"))
    {
        ret = readFile(romDir + "sb30", memoryType, address, 0x10000, showFilename);
        ret = ret | readFile(romDir + "sb31", memoryType, address+0x1000, 0x10000, showFilename);
        ret = ret | readFile(romDir + "sb32", memoryType, address+0x2000, 0x10000, showFilename);
        ret = ret | readFile(romDir + "sb33", memoryType, address+0x3000, 0x10000, showFilename);
        return ret;
    }

    return readProgram(romDir, rom, memoryType, address, showFilename);
}

bool Cdp1802::readProgramPecom(wxString romDir, wxString rom, int memoryType, Word address, bool showFilename)
{
    bool ret;

    if ((rom == "pecom64-1.bin") || (rom == "pecom64-2.bin"))
    {
        ret = readFile(romDir + "pecom64-1.bin", memoryType, address, 0x10000, showFilename);
        ret = ret | readFile(romDir + "pecom64-2.bin", memoryType, address+0x4000, 0x10000, showFilename);
        return ret;
    }

    if ((rom == "170887-rom1.bin") || (rom == "170887-rom2.bin"))
    {
        ret = readFile(romDir + "170887-rom1.bin", memoryType, address, 0x10000, showFilename);
        ret = ret | readFile(romDir + "170887-rom2.bin", memoryType, address+0x4000, 0x10000, showFilename);
        return ret;
    }

    return readProgram(romDir, rom, memoryType, address, showFilename);
}

void Cdp1802::readSt2Program(wxString dirName, wxString fileName, int memoryType)
{
    readSt2Program(dirName+fileName, memoryType);
}

void Cdp1802::readSt2Program(wxString fileNameFull, int memoryType)
{
    wxFFile inFile;
    wxFileName swFullPath = wxFileName(fileNameFull, wxPATH_NATIVE);

    struct
    {
        char header[4];
        Byte numBlocks;
        Byte format;
        Byte video;
        Byte notUsed1;
        char author[2];
        char dumper[2];
        Byte notUsed2[4];
        char catalogue[10];
        Byte notUsed3[6];
        char title[32];
        Byte offsets[64];
        Byte notUsed4[128];
    } st2Header;

    Word address;
    
    if (fileNameFull.Len() != 0)
    {
        if (wxFile::Exists(fileNameFull))
        {
            if (inFile.Open(fileNameFull, _(_("rb"))))
            {
                inFile.Read(&st2Header, 256);
                if (st2Header.offsets[0] == 0 && st2Header.offsets[4] == 0x24)
                    allocTestCartMemory();
                for (int i=1; i<st2Header.numBlocks; i++)
                {
                    if (st2Header.offsets[0] == 0 && st2Header.offsets[4] == 0x24)
                    {
                        inFile.Read((&testCartRom_[st2Header.offsets[i-1] << 8]),256);
                        defineMemoryType(st2Header.offsets[i-1] << 8, TESTCARTRIDGEROM);
                    }
                    else
                    {
                        address = st2Header.offsets[i-1] << 8;
                        inFile.Read((&mainMemory_[address]),256);
                        if ((memoryType&0xff) != NOCHANGE)
                            defineMemoryType(st2Header.offsets[i-1] << 8, memoryType);
                    }
                }
                inFile.Close();
            }
            else
            {
                (void)wxMessageBox( "Error reading " + swFullPath.GetName(),  // Works correct, via p_Main->errorMessage it will NOT
                                    "Emma 02", wxICON_ERROR | wxOK );
            }
        }
        else
        {
            (void)wxMessageBox( "File " + swFullPath.GetName() + " not found", // Works correct, via p_Main->errorMessage it will NOT
                                "Emma 02", wxICON_ERROR | wxOK );
        }
    }
}

bool Cdp1802::readFile(wxString fileName, int memoryType, Word address, long end, bool showFilename)
{
    wxFFile inFile;
    char buffer[4];

    if (wxFile::Exists(fileName))
    {
        if (inFile.Open(fileName, _("rb")))
        {
            inFile.Read(buffer, 4);
            inFile.Close();

            if (showFilename)
            {
                wxFileName swFullPath = wxFileName(fileName, wxPATH_NATIVE); 
                p_Main->setSwName (swFullPath.GetName());
                p_Main->updateTitle();
            }

            if (buffer[0] == ':' || (buffer[0] == 0x0d && buffer[1] == 0x0a && buffer[2] == ':'))
                return readIntelFile(fileName, memoryType, end, showFilename);
            else if (buffer[0] == '0' && buffer[1] == '0' && buffer[2] == '0' && buffer[3] == '0')
                return readLstFile(fileName, memoryType, end, showFilename);
            else
                return readBinFile(fileName, memoryType, address, end, showFilename, false, 0);
        }
        else
        {
            p_Main->errorMessage("Error reading " + fileName);
            return false;
        }
    }
    else
    {
        p_Main->errorMessage("File " + fileName + " not found");
        return false;
    }
}

bool Cdp1802::readFile(wxString fileName, int memoryType, Word address, long end, LoadOffSet loadOffSet, bool showFilename)
{
    wxFFile inFile;
    char buffer[4];

    if (wxFile::Exists(fileName))
    {
        if (inFile.Open(fileName, _("rb")))
        {
            inFile.Read(buffer, 4);
            inFile.Close();

            if (showFilename)
            {
                wxFileName swFullPath = wxFileName(fileName, wxPATH_NATIVE);
                p_Main->setSwName (swFullPath.GetName());
                p_Main->updateTitle();
            }

            if (buffer[0] == ':' || (buffer[0] == 0x0d && buffer[1] == 0x0a && buffer[2] == ':'))
                return readIntelFile(fileName, memoryType, end, showFilename);
            else if (buffer[0] == '0' && buffer[1] == '0' && buffer[2] == '0' && buffer[3] == '0')
                return readLstFile(fileName, memoryType, end, showFilename);
            else
                return readBinFile(fileName, memoryType, address, end, loadOffSet, showFilename, false, 0);
        }
        else
        {
            p_Main->errorMessage("Error reading " + fileName);
            return false;
        }
    }
    else
    {
        p_Main->errorMessage("File " + fileName + " not found");
        return false;
    }
}

bool Cdp1802::readFile(wxString fileName, int memoryType, Word address, long end, bool showFilename, bool showAddressPopup, Word specifiedStartAddress)
{
    wxFFile inFile;
    char buffer[4];
    
    if (wxFile::Exists(fileName))
    {
        if (inFile.Open(fileName, _("rb")))
        {
            inFile.Read(buffer, 4);
            inFile.Close();
            
            if (showFilename)
            {
                wxFileName swFullPath = wxFileName(fileName, wxPATH_NATIVE);
                p_Main->setSwName(swFullPath.GetName());
                p_Main->updateTitle();
            }
            
            if (buffer[0] == ':')
                return readIntelFile(fileName, memoryType, end, showFilename);
            else if (buffer[0] == '0' && buffer[1] == '0' && buffer[2] == '0' && buffer[3] == '0')
                return readLstFile(fileName, memoryType, end, showFilename);
            else
                return readBinFile(fileName, memoryType, address, end, showFilename, showAddressPopup, specifiedStartAddress);
        }
        else
        {
            p_Main->errorMessage("Error reading " + fileName);
            return false;
        }
    }
    else
    {
        p_Main->errorMessage("File " + fileName + " not found");
        return false;
    }
}

void Cdp1802::setSteps(long steps)
{
    steps_ = steps;
};

void Cdp1802::setDebugMode (bool debugModeNew, bool debugChip8ModeNew, bool trace, bool traceDma, bool traceInt, bool traceChip8Int)
{
    if (debugMode_ && !debugModeNew)
    {
        trace_ = false;
        debugMode_ = false;
    }
    else
    {
        debugMode_ = debugModeNew;
        trace_ = trace;
        traceDma_ = traceDma;
        traceInt_ = traceInt;
    }

    if (chip8DebugMode_ && !debugChip8ModeNew)
    {
        chip8DebugMode_ = false;
    }
    else
    {
        chip8DebugMode_ = debugChip8ModeNew;
        traceChip8Int_ = traceChip8Int;
    }
}

void Cdp1802::debugTrace (wxString text)
{
    if (trace_ && !singleStateStep_)
        p_Main->debugTrace(text);
}

void Cdp1802::writeMemLabelType(Word address, Byte type)
{
    size_t emsNumber;
    
    switch (memoryType_[address / 256]&0xff)
    {
        case ROM:
        case NVRAM:
        case CARTRIDGEROM:
        case VP570RAM:
            address = (address | bootstrap_) & currentComputerConfiguration.memoryMask;
                    
            if (type > mainMemoryLabelType_[address] || type == 0)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryLabelType_[address] = type;
            }
        break;

        case RAM:
            address = (address | bootstrap_) & currentComputerConfiguration.memoryMask;

            if (type > mainMemoryLabelType_[address] || type == 0)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryLabelType_[address] = type;
            }
        break;
 
        case CPURAM:
            if (type > cpuRamLabelType_[address&0xff] || type == 0)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                cpuRamLabelType_[address&0xff] = type;
            }
        break;

        case MAPPEDRAM:
            address = (address | bootstrap_) & currentComputerConfiguration.memoryMask;

            if (type > mainMemoryLabelType_[address] || type == 0)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryLabelType_[address] = type;
            }
        break;
            
        case PRAM1870:
        case CRAM1870:
                if (epromSlot_ != 0xff || superSlot_ != 0xff)
                {
                    if (type > mainMemoryLabelType_[address] || type == 0)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        mainMemoryLabelType_[address] = type;
                    }
                }
        break;

        case EMSMEMORY:
            emsNumber = (memoryType_[address / 256] >> 8);

            switch (emsMemory_[emsNumber].memoryType_[((address - currentComputerConfiguration.emsMemoryConfiguration[emsNumber].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[emsNumber].page << currentComputerConfiguration.emsMemoryConfiguration[emsNumber].maskBits))/256])
            {
                case ROM:
                case RAM:
                    if (type > emsMemory_[emsNumber].labelType_[(long) ((address - currentComputerConfiguration.emsMemoryConfiguration[emsNumber].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[emsNumber].page << currentComputerConfiguration.emsMemoryConfiguration[emsNumber].maskBits))] || type == 0)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        emsMemory_[emsNumber].dataType_[(long) ((address - currentComputerConfiguration.emsMemoryConfiguration[emsNumber].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[emsNumber].page << currentComputerConfiguration.emsMemoryConfiguration[emsNumber].maskBits))] = type;
                    }
                break;
            }
        break;
            
        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask))/256])
            {
                case ROM:
                case RAM:
                    if (type > pagerMemoryLabelType_[(getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask)] || type == 0)
                    {
                        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                        pagerMemoryDataType_[(getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask)] = type;
                    }
                break;
            }
        break;
    }
}

Byte Cdp1802::readMemLabelType(Word address)
{
    size_t emsNumber;
    
    switch (memoryType_[address / 256]&0xff)
    {
        case ROM:
        case NVRAM:
        case CARTRIDGEROM:
        case VP570RAM:
             address = (address | bootstrap_) & currentComputerConfiguration.memoryMask;
            return mainMemoryLabelType_[address];
        break;
            
        case RAM:
            address = (address | bootstrap_) & currentComputerConfiguration.memoryMask;
            return mainMemoryLabelType_[address];
        break;
            
        case CPURAM:
            return cpuRamLabelType_[address&0xff];
        break;
            
        case MAPPEDRAM:
            address = (address | bootstrap_) & currentComputerConfiguration.memoryMask;
            return mainMemoryLabelType_[address];
        break;

        case PRAM1870:
        case CRAM1870:
                if (epromSlot_ != 0xff || superSlot_ != 0xff)
                    return mainMemoryLabelType_[address];
                else
                    return MEM_TYPE_UNDEFINED;
        break;

        case EMSMEMORY:
            emsNumber = (memoryType_[address / 256] >> 8);

            switch (emsMemory_[emsNumber].memoryType_[((address - currentComputerConfiguration.emsMemoryConfiguration[emsNumber].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[emsNumber].page << currentComputerConfiguration.emsMemoryConfiguration[emsNumber].maskBits))/256])
            {
                case ROM:
                case RAM:
                    return emsMemory_[emsNumber].labelType_[(long) ((address - currentComputerConfiguration.emsMemoryConfiguration[emsNumber].startAddress) |(currentComputerConfiguration.emsMemoryConfiguration[emsNumber].page << currentComputerConfiguration.emsMemoryConfiguration[emsNumber].maskBits))];
                break;
            }
        break;
            
        case PAGER:
            switch (pagerMemoryType_[((getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask))/256])
            {
                case ROM:
                case RAM:
                    return pagerMemoryLabelType_[(getPager(address>>currentComputerConfiguration.memoryMapperConfiguration.maskBits) << currentComputerConfiguration.memoryMapperConfiguration.maskBits) |(address &currentComputerConfiguration.memoryMapperConfiguration.mask)];
                break;
            }
        break;
    }
    return MEM_TYPE_UNDEFINED;
}

void Cdp1802::setHeaderTitle(const wxString& WXUNUSED(title))
{
}

void Cdp1802::updateTitle(wxString WXUNUSED(Title))
{
}

void Cdp1802::increaseExecutedMainMemory(long address, Byte type)
{
    if ((type == MEM_TYPE_OPCODE || type >= MEM_TYPE_OPCODE_RSHR) && profilerCounter_ != PROFILER_OFF)
    {
        if (mainMemoryExecuted_[address] < UINT64_MAX)
            mainMemoryExecuted_[address]++;
        p_Main->updateAssTabCheck(address);
    }
}

void Cdp1802::increaseExecutedExpansionRom(long address, Byte type)
{
    if ((type == MEM_TYPE_OPCODE || type >= MEM_TYPE_OPCODE_RSHR) && profilerCounter_ != PROFILER_OFF)
    {
        if (expansionRomExecuted_[address] < UINT64_MAX)
            expansionRomExecuted_[address]++;
        p_Main->updateAssTabCheck(address);
    }
}

void Cdp1802::increaseExecutedSlotMemory(int slot, long address, Byte type)
{
    if ((type == MEM_TYPE_OPCODE || type >= MEM_TYPE_OPCODE_RSHR) && profilerCounter_ != PROFILER_OFF)
    {
        if (slotMemoryExecuted_[slot][address] < UINT64_MAX)
            slotMemoryExecuted_[slot][address]++;
        p_Main->updateAssTabCheck(address);
    }
}

void Cdp1802::increaseExecutedMcrMemory(int map, long address, Byte type)
{
    if ((type == MEM_TYPE_OPCODE || type >= MEM_TYPE_OPCODE_RSHR) && profilerCounter_ != PROFILER_OFF)
    {
        if (mcrMemoryExecuted_[map][address] < UINT64_MAX)
            mcrMemoryExecuted_[map][address]++;
        p_Main->updateAssTabCheck(address);
    }
}

void Cdp1802::increaseExecutedExpansionRam(Word address, Byte type)
{
    if ((type == MEM_TYPE_OPCODE || type >= MEM_TYPE_OPCODE_RSHR) && profilerCounter_ != PROFILER_OFF)
    {
        if (expansionRamExecuted_[address] < UINT64_MAX)
            expansionRamExecuted_[address]++;
        p_Main->updateAssTabCheck(address);
    }
}

void Cdp1802::increaseExecutedExpansionEprom(Word address, Byte type)
{
    if ((type == MEM_TYPE_OPCODE || type >= MEM_TYPE_OPCODE_RSHR) && profilerCounter_ != PROFILER_OFF)
    {
        if (expansionEpromExecuted_[address] < UINT64_MAX)
            expansionEpromExecuted_[address]++;
        p_Main->updateAssTabCheck(address);
    }
}

void Cdp1802::increaseExecutedExpansionSuper(Word address, Byte type)
{
    if ((type == MEM_TYPE_OPCODE || type >= MEM_TYPE_OPCODE_RSHR) && profilerCounter_ != PROFILER_OFF)
    {
        if (expansionSuperExecuted_[address] < UINT64_MAX)
            expansionSuperExecuted_[address]++;
        p_Main->updateAssTabCheck(address);
    }
}

void Cdp1802::increaseExecutedCpuRam(Word address, Byte type)
{
    if ((type == MEM_TYPE_OPCODE || type >= MEM_TYPE_OPCODE_RSHR) && profilerCounter_ != PROFILER_OFF)
    {
        if (cpuRamExecuted_[address] < UINT64_MAX)
            cpuRamExecuted_[address]++;
        p_Main->updateAssTabCheck(address);
    }
}

void Cdp1802::increaseExecutedMultiCartRom(Word address, Byte type)
{
    if ((type == MEM_TYPE_OPCODE || type >= MEM_TYPE_OPCODE_RSHR) && profilerCounter_ != PROFILER_OFF)
    {
        if (multiCartRomExecuted_[address] < UINT64_MAX)
            multiCartRomExecuted_[address]++;
        p_Main->updateAssTabCheck(address);
    }
}

void Cdp1802::increaseExecutedTestCartRom(Word address, Byte type)
{
    if ((type == MEM_TYPE_OPCODE || type >= MEM_TYPE_OPCODE_RSHR) && profilerCounter_ != PROFILER_OFF)
    {
        if (testCartRomExecuted_[address] < UINT64_MAX)
            testCartRomExecuted_[address]++;
        p_Main->updateAssTabCheck(address);
    }
}

void Cdp1802::increaseExecutedEms(size_t emsNumber, long address, Byte type)
{
    if ((type == MEM_TYPE_OPCODE || type >= MEM_TYPE_OPCODE_RSHR) && profilerCounter_ != PROFILER_OFF)
    {
        if (emsMemory_[emsNumber].executed_[address] < UINT64_MAX)
            emsMemory_[emsNumber].executed_[address]++;
        p_Main->updateAssTabCheck(address);
    }
}

void Cdp1802::clearProfiler()
{
    if (profilerCounter_ == PROFILER_OFF)
        return;

    for (int i=0; i<65536; i++)
        mainMemoryExecuted_[i] = 0;
    for (int i=0; i<255; i++)
        cpuRamExecuted_[i] = 0;
    if (comxExpansionMemoryDefined_)
    {
        for (int i=0; i<32768; i++)
            expansionRomExecuted_[i] = 0;
        for (int i=0; i<32768; i++)
            expansionRamExecuted_[i] = 0;
        for (int i=0; i<40960; i++)
            expansionEpromExecuted_[i] = 0;
        for (int i=0; i<131072; i++)
            expansionSuperExecuted_[i] = 0;
    }
    for (std::vector<EmsMemory>::iterator emsMemory = emsMemory_.begin (); emsMemory != emsMemory_.end (); ++emsMemory)
    {
        for (wxUint32 i=0; i<emsSize_; i++)
            emsMemory->executed_[i] = 0;
    }
    if (multiCartMemoryDefined_)
    {
        for (wxUint32 i=0; i<1048576; i++)
            multiCartRomExecuted_[i] = 0;
    }
    if (pagerDefined_)
    {
        for (wxUint32 i=0; i<pagerSize_; i++)
            pagerMemoryExecuted_[i] = 0;
    }

}


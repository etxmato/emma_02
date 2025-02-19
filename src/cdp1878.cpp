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
#include "cdp1878.h"

#define MODE_NONE 0
#define MODE_1 1
#define MODE_2 2
#define MODE_3 3
#define MODE_4 4
#define MODE_5 5

Cdp1878Instance::Cdp1878Instance(int cdp1878Number)
{
    cdp1878Number_ = cdp1878Number;
    holdingRegisterA_ = 0;
    jamRegisterA_ = 0;
    controlA_ = 0;
    
    holdingRegisterB_ = 0;
    jamRegisterB_ = 0;
    controlB_ = 0;
    
    InterruptStatusRegister_ = 0;
    
    positiveGateLevelA_ = false;
    interruptEnabledA_ = false;
    startCounterA_ = false;
    freezeHoldingRegisterA_ = false;
    jamEnabledA_ = false;
    modeA_ = MODE_NONE;
    
    positiveGateLevelB_ = false;
    interruptEnabledB_ = false;
    startCounterB_ = false;
    freezeHoldingRegisterB_ = false;
    jamEnabledB_ = false;
    modeA_ = MODE_NONE;
}

void Cdp1878Instance::configureCdp1878(Cdp1878Configuration cdp1878Configuration)
{
    cdp1878Configuration_ = cdp1878Configuration;    
    wxString cdp1878NumberString = "";
    if (cdp1878Number_ > 0)
        cdp1878NumberString.Printf(" %d", cdp1878Number_);
    
    p_Main->configureMessage(&cdp1878Configuration.ioGroupVector, "CDP1878 Dual Counter-Timer" + cdp1878NumberString);

    p_Computer->setOutType(&cdp1878Configuration.ioGroupVector, cdp1878Configuration.counterHighA, "counter A high");
    p_Computer->setOutType(&cdp1878Configuration.ioGroupVector, cdp1878Configuration.counterLowA, "counter A low");
    p_Computer->setOutType(&cdp1878Configuration.ioGroupVector, cdp1878Configuration.controlA, "write control register A");
    p_Computer->setOutType(&cdp1878Configuration.ioGroupVector, cdp1878Configuration.counterHighB, "counter B high");
    p_Computer->setOutType(&cdp1878Configuration.ioGroupVector, cdp1878Configuration.counterLowB, "counter B low");
    p_Computer->setOutType(&cdp1878Configuration.ioGroupVector, cdp1878Configuration.controlB, "write control register B");
    p_Computer->setOutType(&cdp1878Configuration.ioGroupVector, cdp1878Configuration.interrupt, "read interrupt register");
    p_Computer->setEfType(&cdp1878Configuration.ioGroupVector, cdp1878Configuration.ef, "irq");
    p_Computer->setCycleType(CYCLE_TYPE_TIMER, TIMER_CYCLE);
 
    p_Main->message("");
}

bool Cdp1878Instance::ioGroupCdp1878(int ioGroup)
{
    bool groupFound = false;
    
    if (cdp1878Configuration_.ioGroupVector.size() == 0)
        groupFound = true;
    else
    {
        for (std::vector<int>::iterator ioGroupIterator = cdp1878Configuration_.ioGroupVector.begin (); ioGroupIterator != cdp1878Configuration_.ioGroupVector.end (); ++ioGroupIterator)
        {
            if (*ioGroupIterator == ioGroup)
                groupFound = true;
        }
    }
    return groupFound;
}

Byte Cdp1878Instance::efInterrupt()
{
    return (cdp1878Configuration_.ef.reverse^interruptEf_);
}

void Cdp1878Instance::writeCounterHighA(Byte value)
{
    jamRegisterA_ = (jamRegisterA_ & 0xff) | (value << 8);
}

void Cdp1878Instance::writeCounterLowA(Byte value)
{
    jamRegisterA_ = (jamRegisterA_ & 0xff00) | value;
}

void Cdp1878Instance::writeCounterHighB(Byte value)
{
    jamRegisterB_ = (jamRegisterB_ & 0xff) | (value << 8);
}

void Cdp1878Instance::writeCounterLowB(Byte value)
{
    jamRegisterB_ = (jamRegisterB_ & 0xff00) | value;
}

Byte Cdp1878Instance::readCounterHighA()
{
    return (holdingRegisterA_ & 0xFF00) >> 8;
}

Byte Cdp1878Instance::readCounterLowA()
{
    return holdingRegisterA_ & 0xFF;
}

Byte Cdp1878Instance::readCounterHighB()
{
    return (holdingRegisterB_ & 0xFF00) >> 8;
}

Byte Cdp1878Instance::readCounterLowB()
{
    return holdingRegisterB_ & 0xFF;
}

void Cdp1878Instance::writeControlA(Byte value)
{
    modeA_ = value & 0x7;
    positiveGateLevelA_ = ((value & 0x8) == 0x8);
    interruptEnabledA_ = ((value & 0x10) == 0x10);
    startCounterA_ = ((value & 0x20) == 0x20);
    freezeHoldingRegisterA_ = ((value & 0x40) == 0x40);
    jamEnabledA_ = ((value & 0x80) == 0x80);
}

void Cdp1878Instance::writeControlB(Byte value)
{
    modeB_ = value & 0x7;
    positiveGateLevelB_ = ((value & 0x8) == 0x8);
    interruptEnabledB_ = ((value & 0x10) == 0x10);
    startCounterB_ = ((value & 0x20) == 0x20);
    freezeHoldingRegisterB_ = ((value & 0x40) == 0x40);
    jamEnabledB_ = ((value & 0x80) == 0x80);
}

Byte Cdp1878Instance::readInterrupt()
{
    return InterruptStatusRegister_;
}

void Cdp1878Instance::cycle()
{
    if (cycleCounter_ > 0)
    {
        cycleCounter_--;
        if (cycleCounter_ == 0)
        {
        }
    }
}


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

Cdp1878Instance::Cdp1878Instance(int cdp1878Number)
{
    cdp1878Number_ = cdp1878Number;
    for (int counter=0; counter<2; counter++)
    {
        holdingRegister_[counter] = 0;
        counterRegister_[counter] = 0;
        jamRegister_[counter] = 0;
        control_[counter] = 0;

        positiveGateLevel_[counter] = false;
        interruptEnabled_[counter] = false;
        startCounter_[counter] = false;
        freezeHoldingRegister_[counter] = false;
        jamEnabled_[counter] = false;
        mode_[counter] = MODE_NONE;

        strobe_[counter] = false;
        pwmPhaseLsb_[counter] = true;
    }
    
    interruptEf_ = 1;
    interruptStatusRegister_ = 0;
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
    p_Computer->setInType(&cdp1878Configuration.ioGroupVector, cdp1878Configuration.interruptA, "read interrupt register");
    p_Computer->setOutType(&cdp1878Configuration.ioGroupVector, cdp1878Configuration.counterHighB, "counter B high");
    p_Computer->setOutType(&cdp1878Configuration.ioGroupVector, cdp1878Configuration.counterLowB, "counter B low");
    p_Computer->setOutType(&cdp1878Configuration.ioGroupVector, cdp1878Configuration.controlB, "write control register B");
    p_Computer->setInType(&cdp1878Configuration.ioGroupVector, cdp1878Configuration.interruptB, "read interrupt register");
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

void Cdp1878Instance::writeCounterHigh(int counter, Byte value)
{
    jamRegister_[counter] = (jamRegister_[counter] & 0xff) | (value << 8);
}

void Cdp1878Instance::writeCounterLow(int counter, Byte value)
{
    jamRegister_[counter] = (jamRegister_[counter] & 0xff00) | value;
}

Byte Cdp1878Instance::readCounterHigh(int counter)
{
    return (holdingRegister_[counter] & 0xFF00) >> 8;
}

Byte Cdp1878Instance::readCounterLow(int counter)
{
    return holdingRegister_[counter] & 0xFF;
}

void Cdp1878Instance::writeControl(int counter, Byte value)
{
    interruptEf_ = 1;
    p_Computer->clearInterrupt(INTERRUPT_TYPE_TIMER_A+counter, false, cdp1878Configuration_.picInterrupt);
    interruptStatusRegister_ = 0;
    
    if ((value & 0x7) != 0)
        mode_[counter] = value & 0x7;
    positiveGateLevel_[counter] = ((value & 0x8) == 0x8);
    interruptEnabled_[counter] = ((value & 0x10) == 0x10);
    startCounter_[counter] = ((value & 0x20) == 0x20);
    freezeHoldingRegister_[counter] = ((value & 0x40) == 0x40);
    jamEnabled_[counter] = ((value & 0x80) == 0x80);
    if (jamEnabled_[counter])
    {
        counterRegister_[counter] = jamRegister_[counter];
        counterRegisterLsb_[counter] = jamRegister_[counter] & 0xff;
        counterRegisterMsb_[counter] = jamRegister_[counter] >> 8;
    }
    pwmPhaseLsb_[counter] = true;
}

Byte Cdp1878Instance::readInterrupt()
{
    return interruptStatusRegister_;
}

void Cdp1878Instance::timeOut(int counter)
{
    if (strobe_[counter])
    {
        interruptEf_ = 1;
        p_Computer->clearInterrupt(INTERRUPT_TYPE_TIMER_A+counter, false, cdp1878Configuration_.picInterrupt);
        interruptStatusRegister_ ^= 0x80 >> counter;
        strobe_[counter] = false;
    }

    if (!startCounter_[counter] || !positiveGateLevel_[counter])
        return;
    
    switch (mode_[counter])
    {    
        case MODE_RATE:
            counterRegister_[counter]--;
            if (counterRegister_[counter] == 0xFFFF)
            {
                interrupt();
                counterRegister_[counter] = jamRegister_[counter];
            }
        break;
        
        case MODE_STROBE:
            counterRegister_[counter]--;
            if (counterRegister_[counter] == 0xFFFF)
                strobe_[counter] = true;
        break;
       
        case MODE_PWM_1:
        case MODE_PWM_2:
            if (pwmPhaseLsb_[counter])
            {
                counterRegisterLsb_[counter]--;
                if (counterRegisterLsb_[counter] == 0xff)
                {
                    counterRegisterLsb_[counter] = jamRegister_[counter] & 0xff;
                    pwmPhaseLsb_[counter] = false;
                }
            }
            else
            {
                counterRegisterMsb_[counter]--;
                if (counterRegisterLsb_[counter] == 0xff)
                {
                    counterRegisterMsb_[counter] = jamRegister_[counter] >> 8;
                    pwmPhaseLsb_[counter] = true;
                    interrupt();
                }
            }
            counterRegister_[counter] = (counterRegisterMsb_[counter] << 8) | counterRegisterLsb_[counter]
        break;

        default:
            counterRegister_[counter]--;
            if (counterRegister_[counter] == 0xFFFF)
                startCounter_[counter] = false;
        break;
    }
    if (!freezeHoldingRegister_[counter])
        holdingRegister_[counter] = counterRegister_[counter];
}

void Cdp1878Instance::interrupt()
{
    if (interruptEnabled_[counter])
    {
        interruptEf_ = 0;
        p_Computer->requestInterrupt(INTERRUPT_TYPE_TIMER_A+counter, true, cdp1878Configuration_.picInterrupt);
        interruptStatusRegister_ |= 0x80 >> counter;
    }
}



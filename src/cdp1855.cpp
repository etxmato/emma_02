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
#include "cdp1855.h"

Cdp1855Instance::Cdp1855Instance()
{
    status_ = 0;
    control_ = 0;
    command_ = 0;
    cycleCounter_ = 0;
    sequenceCounter_ = 0;
    numberOfMdu_ = 1;
}

void Cdp1855Instance::configureCdp1855(Cdp1855Configuration cdp1855Configuration)
{
    cdp1855Configuration_ = cdp1855Configuration;    
    p_Main->configureMessage(&cdp1855Configuration.ioGroupVector, "CDP1855 Multiply-Divide Unit");

    p_Computer->setOutType(&cdp1855Configuration.ioGroupVector, cdp1855Configuration.x, "write register x");
    p_Computer->setOutType(&cdp1855Configuration.ioGroupVector, cdp1855Configuration.y, "write register y");
    p_Computer->setOutType(&cdp1855Configuration.ioGroupVector, cdp1855Configuration.z, "write register z");
    p_Computer->setOutType(&cdp1855Configuration.ioGroupVector, cdp1855Configuration.control, "write cotnrol register");
    p_Computer->setInType(&cdp1855Configuration.ioGroupVector, cdp1855Configuration.x, "read register x");
    p_Computer->setInType(&cdp1855Configuration.ioGroupVector, cdp1855Configuration.y, "read register y");
    p_Computer->setInType(&cdp1855Configuration.ioGroupVector, cdp1855Configuration.z, "read register z");
    p_Computer->setInType(&cdp1855Configuration.ioGroupVector, cdp1855Configuration.status, "read overflow");
    p_Computer->setEfType(&cdp1855Configuration.ioGroupVector, cdp1855Configuration.ef, "Overflow");
    p_Computer->setCycleType(CYCLE_TYPE_MDU, MDU_CYCLE);
 
    p_Main->message("");
}

bool Cdp1855Instance::ioGroupCdp1855(int ioGroup)
{
    bool groupFound = false;
    
    if (cdp1855Configuration_.ioGroupVector.size() == 0)
        groupFound = true;
    else
    {
        for (std::vector<int>::iterator ioGroupIterator = cdp1855Configuration_.ioGroupVector.begin (); ioGroupIterator != cdp1855Configuration_.ioGroupVector.end (); ++ioGroupIterator)
        {
            if (*ioGroupIterator == ioGroup)
                groupFound = true;
        }
    }
    return groupFound;
}

void Cdp1855Instance::writeX(Byte value)
{
    x[getSequenceCounter()] = value;
}

void Cdp1855Instance::writeY(Byte value)
{
    y[getSequenceCounter()] = value;
}

void Cdp1855Instance::writeZ(Byte value)
{
    z[getSequenceCounter()] = value;
}
void Cdp1855Instance::writeControl(Byte value)
{
    int cycleValue[] = {9, 17, 25, 33};
    int preScalerCycleValue[] = {18, 68, 200, 264};

    command_ = value &0x3;
    if ((value & 0x4) == 0x4)
    {
        for (int mdu=0; mdu<numberOfMdu_; mdu++)
            z[mdu] = 0;
    }
    if ((value & 0x8) == 0x8)
    {
        for (int mdu=0; mdu<numberOfMdu_; mdu++)
            y[mdu] = 0;
    }
    numberOfMdu_ = ((value ^ 0x30) & 0x30) >> 4;
    if ((value & 0x40) == 0x40)
        sequenceCounter_ = 0;
    bool preScaler = ((value & 0x80) == 0x80);

    status_ = 0;
    
    if (preScaler)
        cycleCounter_ = preScalerCycleValue[numberOfMdu_];
    else
        cycleCounter_ = cycleValue[numberOfMdu_];
}

Byte Cdp1855Instance::readX()
{
    return x[getSequenceCounter()];
}

Byte Cdp1855Instance::readY()
{
    return y[getSequenceCounter()];
}

Byte Cdp1855Instance::readZ()
{
    return z[getSequenceCounter()];
}

Byte Cdp1855Instance::readStatus()
{
    return status_;
}

void Cdp1855Instance::cycle()
{
    if (cycleCounter_ > 0)
    {
        cycleCounter_--;
        if (cycleCounter_ == 0)
        {
            switch (command_)
            {
                case 0: // no operation 
                break;

                case 1: // multiply
                    multiply();
                break;

                case 2: // divide
                    divide();
                break;

                case 3: // illegal 
                break;
            }
            command_ = 0;
        }
    }
}

int Cdp1855Instance::getSequenceCounter()
{
    int returnValue = sequenceCounter_;
    sequenceCounter_++;
    if (sequenceCounter_ >= numberOfMdu_)
        sequenceCounter_ = 0;

    return returnValue;
}

void Cdp1855Instance::multiply()
{
    int result = (getRegValue(x) * getRegValue(z)) + getRegValue(y);
    result = setRegValue(z, result);
    setRegValue(y, result);
}

void Cdp1855Instance::divide()
{
    int divisor = getRegValue(x);
    int yVal = getRegValue(y);

    if (divisor <= yVal)
        status_ = 1;
    int dividend = (yVal << (8 * numberOfMdu_)) + getRegValue(z);

    int result = dividend / divisor;
    setRegValue(z, result);
    
    int remaining = dividend - (result * divisor);
    setRegValue(y, remaining);
}

int Cdp1855Instance::getRegValue(Byte reg[])
{
    int regValue = 0;
    for (int mdu=0; mdu<numberOfMdu_; mdu++)
    {
        regValue = regValue << 8;
        regValue |= reg[mdu];
    }
    return regValue;
}

int Cdp1855Instance::setRegValue(Byte reg[], int regValue)
{
    for (int mdu=numberOfMdu_-1; mdu>=0; mdu--)
    {
        reg[mdu] = regValue & 0xff;
        regValue = regValue >> 8;
    }
    return regValue;
}

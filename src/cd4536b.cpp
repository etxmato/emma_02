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

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "main.h"
#include "cd4536b.h"

Cd4536b::Cd4536b()
{
    efState_ = 1;
    counterCycleSize_ = -1;
    counterPaused_ = true;
}

Cd4536b::~Cd4536b()
{
}

void Cd4536b::Configure(Cd4536bConfiguration cd4536bIo, int number)
{
    p_Main->configureMessage(&cd4536bIo.ioGroupVector, "CD4536B");
    p_Computer->setOutType(&cd4536bIo.ioGroupVector, cd4536bIo.writeControl, "write control byte", number);
    p_Computer->setEfType(&cd4536bIo.ioGroupVector, cd4536bIo.ef, "Timer out", number);
    p_Computer->setCycleType(CYCLE_TYPE_CD3536B, CD4536B_CYCLE);
    
    p_Main->message("");
}

void Cd4536b::cycle()
{
    if (counterCycleValue_ > 0 && !counterPaused_)
    {
        counterCycleValue_ --;
        if (counterCycleValue_ <= 0)
        {
            if (counterSquare_)
            {
                efState_ = (efState_ ^ 0x1) & 0x1;
                counterCycleValue_ = counterCycleSize_/2;
            }
            else
            {
                efState_ = 0;
                counterCycleValue_ = counterCycleSize_;
            }
            p_Computer->interrupt();
        }
    }
}

Byte Cd4536b::ef()
{
    return efState_;
}

void Cd4536b::writeControl(Byte value)
{
    efState_ = 1;

    if ((value & 0x10) == 0x10)
        counterCycleSize_ = 2 << (value&0xf);
    else
        counterCycleSize_ = 512 << (value&0xf);
    
    counterSquare_ = ((value & 0x30) == 0);

    counterCycleValue_ = counterCycleSize_ / 2;
    if ((value & 0x20) == 0x20) // Set
        counterCycleValue_ = counterCycleSize_ / 2;
    if ((value & 0x40) == 0x40) // Reset
        counterCycleValue_ = counterCycleSize_;

    counterPaused_ = ((value & 0x80) == 0x80);
}



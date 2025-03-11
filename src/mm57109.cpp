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
#include "mm57109.h"

Mm57109Instance::Mm57109Instance()
{
    cycleCounter_ = 0;
    rdy_ = 1;
    hold_ = 1;
    mdc_ = 8;
    digitNumber_ = 0;
    floatingPointMode_ = true;
}

void Mm57109Instance::configureMm57109(Mm57109Configuration mm57109Configuration)
{
    mm57109Configuration_ = mm57109Configuration;    
    p_Main->configureMessage(&mm57109Configuration.ioGroupVector, "MM57109 Number Cruncher Unit");

    p_Computer->setOutType(&mm57109Configuration.ioGroupVector, mm57109Configuration.output, "write");
    p_Computer->setInType(&mm57109Configuration.ioGroupVector, mm57109Configuration.input, "read");
    p_Computer->setEfType(&mm57109Configuration.ioGroupVector, mm57109Configuration.ef, "rdy");
//    p_Computer->setCycleType(CYCLE_TYPE_MDU, MDU_CYCLE);
 
    p_Main->message("");
}

bool Mm57109Instance::ioGroupMm57109(int ioGroup)
{
    bool groupFound = false;
    
    if (mm57109Configuration_.ioGroupVector.size() == 0)
        groupFound = true;
    else
    {
        for (std::vector<int>::iterator ioGroupIterator = mm57109Configuration_.ioGroupVector.begin (); ioGroupIterator != mm57109Configuration_.ioGroupVector.end (); ++ioGroupIterator)
        {
            if (*ioGroupIterator == ioGroup)
                groupFound = true;
        }
    }
    return groupFound;
}

void Mm57109Instance::write(Byte value)
{
    OpCodes opCode = (OpCodes)(value & 0x3f);
    switch (opCode)
    {
        case OP_CODE_DIGIT_0:
        case OP_CODE_DIGIT_1:
        case OP_CODE_DIGIT_2:
        case OP_CODE_DIGIT_3:
        case OP_CODE_DIGIT_4:
        case OP_CODE_DIGIT_5:
        case OP_CODE_DIGIT_6:
        case OP_CODE_DIGIT_7:
        case OP_CODE_DIGIT_8:
        case OP_CODE_DIGIT_9:
            digitEntry(opCode);
        break;

        case OP_CODE_DP:
        case OP_CODE_EE:
        case OP_CODE_CS:
        case OP_CODE_PI:
        case OP_CODE_AIN:
        case OP_CODE_HALT:
        break;

        default:
            stopDigit();
        break;
    }
    lastOpCode_ = opCode;
}

Byte Mm57109Instance::read()
{
    return 0x40;
}

Byte Mm57109Instance::ef()
{
    return (mm57109Configuration_.ef.reverse^rdy_);
}

void Mm57109Instance::hold(Byte value)
{
    hold_ = value;
}

void Mm57109Instance::cycle()
{
    if (cycleCounter_ > 0)
    {
        cycleCounter_--;
        if (cycleCounter_ == 0)
        {
        }
    }
}

void Mm57109Instance::pushStack()
{
    if (lastOpCode != OP_CODE_ENTER)
    {
        registerT = registerZ;
        registerZ = registerY;
        registerY = registerX;
    }
    registerX.decimalPoint = 0;
    registerX.exponentSign = 0;
    registerX.mantissaSign = 0;
    for (int digit=0; digit<8; digit++)
        registerX.mantissaDigit[digit] = 0;
    for (int digit=0; digit<2; digit++)
        registerX.exponentDigit[digit] = 0;
}

void Mm57109Instance::digitEntry(Byte number)
{
    switch (digitNumber_)
    {
        case 0:
            pushStack();
        break;

        case 8:
        return;
    }
    registerX.mantissaDigit[digitNumber_++] = number;
}

void Mm57109Instance::stopDigitEntry()
{
    digitNumber_ = 0;
    
}



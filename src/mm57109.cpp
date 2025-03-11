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

factor[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000}

Mm57109Instance::Mm57109Instance()
{
    cycleCounter_ = 0;
    rdy_ = 1;
    hold_ = 1;
    mdc_ = 8;
    digitNumber_ = 0;
    dpNumber_ = -1;
    eeNumber_ = -1;
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
    switch (opCode) // first pass
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
            lastOpCode_ = opCode;
        return;

        case OP_CODE_DP:
            dpNumber_ = digitNumber_;
            lastOpCode_ = opCode;
        return;
            
        case OP_CODE_EE:
            eeNumber_ = 1;
            lastOpCode_ = opCode;
        return;
        
        case OP_CODE_CS:
        case OP_CODE_PI:
        case OP_CODE_AIN:
        case OP_CODE_HALT:
            lastOpCode_ = opCode;
        return;
    }
    stopDigitEntry();

    switch (opCode) // second pass
    {
        case OP_CODE_ENTER:
            pushStack();
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
}

void Mm57109Instance::clearX()
{
    inputRegisterX.decimalPoint = 0;
    inputRegisterX.exponentSign = 1;
    inputRegisterX.mantissaSign = 1;
    inputRegisterX.mantissa = 0;
    inputRegisterX.exponent = 0;

    registerX = 0;
}

void Mm57109Instance::digitEntry(int number)
{
    if (eeNumber_ == -1)
        mantissaEntry(number);
    else
        exponentEntry(number);
}

void Mm57109Instance::mantissaEntry(int number)
{
    switch (digitNumber_)
    {
        case 0:
            pushStack();
            clearX();
        break;

        case 8:
        return;
    }
    inputRegisterX.mantissa = inputRegisterX.mantissa + number * factor[digitNumber_++];
}

void Mm57109Instance::exponentEntry(int number)
{
    if (eeNumber > 1)
        return;

    inputRegisterX.exponent = inputRegisterX.exponent + number * factor[digitNumber_++];
}

void Mm57109Instance::stopDigitEntry()
{
    registerX = (double) inputRegisterX.mantissa / factor[dpNumber_];
 
    digitNumber_ = 0;
    dpNumber_ = -1;
    eeNumber_ = -1;
}



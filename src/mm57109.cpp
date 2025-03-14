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

int factor[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};

Mm57109Instance::Mm57109Instance()
{
    cycleCounter_ = 0;
    rdy_ = 1;
    hold_ = 1;
    mdc_ = 8;
    inputDigitNumber_ = 0;
    outputDigitNumber_ = 0;
    dpNumber_ = 0;
    eeNumber_ = -1;
    linkDigit_ = 0;
    outMode_ = 0x40;
    floatingPointMode_ = true;
    firstInstructionWord_ = true;
    cycleCounter_ = -1;
}

void Mm57109Instance::configureMm57109(Mm57109Configuration mm57109Configuration)
{
    mm57109Configuration_ = mm57109Configuration;    
    p_Main->configureMessage(&mm57109Configuration.ioGroupVector, "MM57109 Number Cruncher Unit");

    p_Computer->setOutType(&mm57109Configuration.ioGroupVector, mm57109Configuration.output, "write");
    p_Computer->setInType(&mm57109Configuration.ioGroupVector, mm57109Configuration.input, "read");
    p_Computer->setEfType(&mm57109Configuration.ioGroupVector, mm57109Configuration.ef, "rdy");
    p_Computer->setCycleType(CYCLE_TYPE_MDU, MM_CYCLE);
 
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
    if (firstInstructionWord_)
        firstInstrucionWord(value);
    else
        secondInstrucionWord(value);
}

void Mm57109Instance::firstInstrucionWord(Byte value)
{
    firstInstructionWord_ = true;

    OpCodes opCode = (OpCodes)(value & 0x3f);
    switch (opCode)
    {
        // Digit Entry commands:
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
            dpNumber_ = inputDigitNumber_;
        break;

        case OP_CODE_EE:
            eeNumber_ = 1;
        break;
            
        case OP_CODE_CS:
            if (eeNumber_ == 0)
                inputRegisterX.mantissaSign = -inputRegisterX.mantissaSign;
            else
                inputRegisterX.exponentSign = -inputRegisterX.exponentSign;
        break;

        case OP_CODE_PI:
            registerX = 3.1415927;
            convert(registerX);
            rdy_ = 0;
        break;

        case OP_CODE_HALT:
        case OP_CODE_NOP:
            // TO BE ADDED
        break;
        
        case OP_CODE_ENTER:
            stopDigitEntry();
            pushStack();
        break;
        
        // Math commands:
            
        case OP_CODE_PLUS:
        case OP_CODE_MINUS:
        case OP_CODE_TIMES:
        case OP_CODE_DIVIDE:
        case OP_CODE_YX:
        case OP_CODE_1X:
        case OP_CODE_SQRT:
        case OP_CODE_SQ:
            stopDigitEntry();
            mathOpCode_ = opCode;
            cycleCounter_ = 1;
        break;

        case OP_CODE_INV:
            stopDigitEntry();
            firstInstructionWord_ = false;
        break;

        // Move commands:

        case OP_CODE_ROLL:
            stopDigitEntry();
            rollStack();
        break;
        
        case OP_CODE_POP:
            stopDigitEntry();
            popStack();
        break;
        
        case OP_CODE_XEY:
            stopDigitEntry();
            exchangeXY();
        break;
        
        case OP_CODE_XEM:
            stopDigitEntry();
            exchangeXM();
        break;

        case OP_CODE_MS:
            stopDigitEntry();
            registerM = registerX;
        break;

        case OP_CODE_MR:
            stopDigitEntry();
            pushStack();
            registerX = registerM;
        break;

        case OP_CODE_LSH:
            stopDigitEntry();
            shiftLeft();
        break;

        case OP_CODE_RSH:
            stopDigitEntry();
            shiftRight();
        break;

        // Mode control commands:

        case OP_CODE_TOGM:
            stopDigitEntry();
            floatingPointMode_ = !floatingPointMode_;
        break;

        // Multi-digit commands:

        case OP_CODE_OUT:
            stopDigitEntry();
            firstInstructionWord_ = false;
        break;

        // Single-digit commands:
            
        case OP_CODE_AIN:
            // TO BE ADDED
        break;
                
    }
    lastOpCode_ = opCode;
}

void Mm57109Instance::secondInstrucionWord(Byte value)
{
    firstInstructionWord_ = true;

    OpCodes opCode = (OpCodes)(value & 0x3f);
    switch (opCode)
    {
        case OP_CODE_OUT:
            if (lastOpCode_ != OP_CODE_OUT)
                return;
            
            outputDigitNumber_ = 0;
            outMode_ = 0x90;
        break;
            
        case OP_CODE_PLUS:
        case OP_CODE_MINUS:
        case OP_CODE_TIMES:
        case OP_CODE_DIVIDE:
            if (lastOpCode_ != OP_CODE_INV)
                return;

            mathOpCode_ = opCode+OP_CODE_OFFSET;
            cycleCounter_ = 1;
        break;

        default:
        break;
    }
}
    
Byte Mm57109Instance::read()
{
    Byte returnValue = 0;
    if (outMode_ == 0x90)
        returnValue = outputRegister[outputDigitNumber_++];
    
    if (outputDigitNumber_ == 10)
    {
        rdy_ = 1;
        outputDigitNumber_ = 0;
        outMode_ = 0x40;
    }
    
    return returnValue | outMode_;
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
            switch (mathOpCode_) // math functions
            {
                // Math commands:
                    
                case OP_CODE_PLUS:
                    registerX = registerY + registerX;
                    convert(registerX);
                    popStack();
                break;

                case OP_CODE_PLUS_INV:
                    registerM = registerM + registerX;
                    convert(registerM);
                break;

                case OP_CODE_MINUS:
                    registerX = registerY - registerX;
                    convert(registerX);
                    popStack();
                break;
                
                case OP_CODE_MINUS_INV:
                    registerM = registerM - registerX;
                    convert(registerM);
                break;
                    
                case OP_CODE_TIMES:
                    registerX = registerY * registerX;
                    convert(registerX);
                    popStack();
                break;
                
                case OP_CODE_TIMES_INV:
                    registerM = registerM * registerX;
                    convert(registerM);
                break;
                
                case OP_CODE_DIVIDE:
                    registerX = registerY / registerX;
                    convert(registerX);
                    popStack();
                break;
                
                case OP_CODE_DIVIDE_INV:
                    registerM = registerM / registerX;
                    convert(registerM);
                break;
                
                case OP_CODE_YX:
                    registerX = pow(registerY, registerX);
                    convert(registerX);
                    popStack();
                break;

                case OP_CODE_1X:
                    registerX = 1 / registerX;
                    convert(registerX);
                break;
                    
                case OP_CODE_SQRT:
                    registerX = sqrt(registerX);
                    convert(registerX);
                break;
                    
                case OP_CODE_SQ:
                    registerX = pow(registerX, 2);
                    convert(registerX);
                break;

                default:
                break;
            }
        }
        rdy_ = 0;
        cycleCounter_ = -1;
    }
}

void Mm57109Instance::pushStack()
{
    registerT = registerZ;
    registerZ = registerY;
    registerY = registerX;
}

void Mm57109Instance::popStack()
{
    registerY = registerZ;
    registerZ = registerT;
    registerT = 0;
}

void Mm57109Instance::rollStack()
{
    double saveX = registerX;

    registerX = registerY;
    registerY = registerZ;
    registerZ = registerT;
    registerT = saveX;
}

void Mm57109Instance::exchangeXY()
{
    double saveX = registerX;
    registerX = registerY;
    registerY = saveX;
}

void Mm57109Instance::exchangeXM()
{
    double saveX = registerX;
    registerX = registerM;
    registerM = saveX;
}

void Mm57109Instance::shiftLeft()
{
    registerX = registerX * 10;
    int intPartRegister = (int) registerX;
    int numberOfDigits = count_digit(intPartRegister);

  //  int removeDigit =
}

void Mm57109Instance::shiftRight()
{
    registerX = registerX / 10;
    int intPartRegister = (int) registerX;
    int numberOfDigits = count_digit(intPartRegister);

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

void Mm57109Instance::convert(double reg)
{
    if (reg < 0)
    {
        outputRegister[0] = 0x8;
        reg = -reg;
    }
    else
        outputRegister[0] = 0;

    int intPartRegister = abs((int) reg);
    int numberOfDigits = count_digit(intPartRegister);
    if (numberOfDigits > 8)
        numberOfDigits = 8;
    outputRegister[1] = 12-numberOfDigits;
    intPartRegister = (double) reg * pow(10, 8-numberOfDigits);
    for (int digit=9; digit>1; digit--)
    {
        outputRegister[digit] = intPartRegister % 10;
        intPartRegister = intPartRegister / 10;
    }
}

int Mm57109Instance::count_digit(int number) 
{
    if (number == 0)
        return 1;
    else
        return int(log10(number) + 1);
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
    switch (inputDigitNumber_)
    {
        case 0:
            if (lastOpCode_ != OP_CODE_ENTER)
                pushStack();
            clearX();
        break;

        case 8:
        return;
    }
    inputRegisterX.mantissa = number + inputRegisterX.mantissa * 10;
    inputDigitNumber_++;
}

void Mm57109Instance::exponentEntry(int number)
{
    if (eeNumber_ > 1)
        return;

    inputRegisterX.exponent = number + inputRegisterX.exponent * 10;
    eeNumber_++;
}

void Mm57109Instance::stopDigitEntry()
{
    if (inputDigitNumber_ == 0)
        return;
    
    if (floatingPointMode_)
    {
        if (dpNumber_ != 0)
            eeNumber_ = dpNumber_-inputDigitNumber_;
        else
            eeNumber_ = 0;
    }

    registerX = (double) inputRegisterX.mantissa * pow(10, eeNumber_);
 
    inputDigitNumber_ = 0;
    dpNumber_ = 0;
    eeNumber_ = -1;
}



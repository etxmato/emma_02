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

Mm57109Instance::Mm57109Instance(double cpuClock, double ncuClock)
{
    inputDigitNumber_ = 0;
    outputDigitNumber_ = 0;
    dpNumber_ = -1;
    eeNumber_ = -1;
    linkDigit_ = 0;
    outMode_ = 0x40;
    firstInstructionWord_ = true;
    cycleCounter_ = -1;
    
    int cycleTime = 1 / ncuClock;

    masterClear();
}

void Mm57109Instance::masterClear()
{
    rdy_ = 1;
    hold_ = 1;
    clearX();
    registerForOutput = 0;
    registerY = 0;
    registerZ = 0;
    registerT = 0;
    registerM = 0;
    mdc_ = 8;
    floatingPointMode_ = true;
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
            convert(registerX);
        break;

        case OP_CODE_EE:
            eeNumber_ = 0;
        break;
            
        case OP_CODE_CS:
            if (eeNumber_ == -1)
                inputRegisterX.mantissaSign = -inputRegisterX.mantissaSign;
            else
                inputRegisterX.exponentSign = -inputRegisterX.exponentSign;
            convert(registerX);
        break;

        case OP_CODE_PI:
            registerX = 3.1415927;
            convert(registerX);
        break;

        case OP_CODE_HALT:
            // TO BE ADDED
        break;

        case OP_CODE_NOP:
            stopDigitEntry();
            // TO BE ADDED
        break;
        
        case OP_CODE_ENTER:
            stopDigitEntry();
            pushStack();
            rdy_ = 0;
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
        case OP_CODE_10X:
        case OP_CODE_EX:
        case OP_CODE_LN:
        case OP_CODE_LOG:
        case OP_CODE_SIN:
        case OP_CODE_COS:
        case OP_CODE_TAN:
        case OP_CODE_DTR:
        case OP_CODE_RTD:
            stopDigitEntry();
            mathOpCode_ = opCode;
            cycleCounter_ = 100;
        break;

        // Move commands:

        case OP_CODE_ROLL:
            stopDigitEntry();
            rollStack();
            convert(registerX);
        break;
        
        case OP_CODE_POP:
            stopDigitEntry();
            registerX = registerY;
            popStack();
            convert(registerX);
        break;
        
        case OP_CODE_XEY:
            stopDigitEntry();
            exchangeXY();
            convert(registerX);
        break;
        
        case OP_CODE_XEM:
            stopDigitEntry();
            exchangeXM();
            convert(registerX);
        break;

        case OP_CODE_MS:
            stopDigitEntry();
            registerM = registerX;
        break;

        case OP_CODE_MR:
            stopDigitEntry();
            pushStack();
            registerX = registerM;
            convert(registerX);
        break;

        case OP_CODE_LSH:
            stopDigitEntry();
            shiftLeft();
            convert(registerX);
        break;

        case OP_CODE_RSH:
            stopDigitEntry();
            shiftRight();
            convert(registerX);
        break;

        // Clear commands:
        
        case OP_CODE_MCLR:
            stopDigitEntry();
            masterClear();
            convert(0);
        break;
   
        case OP_CODE_ECLR:
            stopDigitEntry();
            // TO BE ADDED
        break;
        
        // Multi-digit commands:

        case OP_CODE_IN:
            stopDigitEntry();
            // TO BE ADDED
        break;
        
        case OP_CODE_OUT:
            stopDigitEntry();
            firstInstructionWord_ = false;
        break;

        // Single-digit commands:
            
        case OP_CODE_AIN:
            stopDigitEntry();
            // TO BE ADDED
        break;
                
        // Mode control commands:

        case OP_CODE_TOGM:
            stopDigitEntry();
            floatingPointMode_ = !floatingPointMode_;
        break;

        case OP_CODE_SMDC:
            stopDigitEntry();
            firstInstructionWord_ = false;
        break;

        case OP_CODE_INV:
            stopDigitEntry();
            firstInstructionWord_ = false;
        break;
    }
    lastOpCode_ = opCode;
}

void Mm57109Instance::secondInstrucionWord(Byte value)
{
    firstInstructionWord_ = true;

    OpCodes opCode = (OpCodes)(value & 0x3f);
    switch (lastOpCode_)
    {
        case OP_CODE_OUT:
            if (opCode == OP_CODE_OUT)
            {
                outputDigitNumber_ = 0;
                outMode_ = 0x90;
            }
            else
                firstInstrucionWord(value);
        break;

        case OP_CODE_INV:
            invCommand(opCode);
        break;

        case OP_CODE_SMDC:
            mdc_ = value;
            if (mdc_ > 8)
                mdc_ = 8;
        break;

        default:
            firstInstrucionWord(value);
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

                case OP_CODE_10X:
                    registerX = pow(10, registerX);
                    convert(registerX);
                break;

                case OP_CODE_EX:
                    registerX = pow(2.7182818, registerX);
                    convert(registerX);
                break;

                case OP_CODE_LN:
                    registerX = log(registerX);
                    convert(registerX);
                break;

                case OP_CODE_LOG:
                    registerX = log10(registerX);
                    convert(registerX);
                break;

                case OP_CODE_SIN:
                    registerX = sin(registerX * 3.1415927/180);
                    convert(registerX);
                break;

                case OP_CODE_SIN_INV:
                    registerX = asin(registerX) * 180/3.1415927;
                    convert(registerX);
                break;

                case OP_CODE_COS:
                    registerX = cos(registerX * 3.1415927/180);
                    convert(registerX);
                break;

                case OP_CODE_COS_INV:
                    registerX = acos(registerX) * 180/3.1415927;
                    convert(registerX);
                break;

                case OP_CODE_TAN:
                    registerX = tan(registerX * 3.1415927/180);
                    convert(registerX);
                break;

                case OP_CODE_TAN_INV:
                    registerX = atan(registerX) * 180/3.1415927;
                    convert(registerX);
                break;

                case OP_CODE_DTR:
                    registerX = registerX * 3.1415927/180;
                    convert(registerX);
                break;
                
                case OP_CODE_RTD:
                    registerX = registerX * 180/3.1415927;
                    convert(registerX);
                break;

                default:
                break;
            }
            cycleCounter_ = -1;
        }
        rdy_ = 0;
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
    lastOpCode_ = OP_CODE_ENTER;
    mantissaConvert(registerX);
    
    linkDigit_ = outputRegister[2];
    inputDigitNumber_ = 0;
    for (int i=3; i<10; i++)
        mantissaEntry(outputRegister[i]);

    if (outputRegister[0] == 8)
        inputRegisterX.mantissaSign = -1;
    
    registerX = (double) inputRegisterX.mantissa * pow(10, count_digit(inputRegisterX.mantissa)-outputRegister[1]-3);
    registerX *= inputRegisterX.mantissaSign;
}

void Mm57109Instance::shiftRight()
{
    lastOpCode_ = OP_CODE_ENTER;
    mantissaConvert(registerX);
    
    inputDigitNumber_ = 0;
    mantissaEntry(linkDigit_);
    for (int i=2; i<9; i++)
        mantissaEntry(outputRegister[i]);

    if (outputRegister[0] == 8)
        inputRegisterX.mantissaSign = -1;
    
    registerX = (double) inputRegisterX.mantissa * pow(10, count_digit(inputRegisterX.mantissa)-outputRegister[1]-3);
    registerX *= inputRegisterX.mantissaSign;

    linkDigit_ = 0;
    
/*    registerX = registerX / 10;

    int intPartRegister = (int) registerX;
    int numberOfDigits = count_digit(intPartRegister);*/

}

void Mm57109Instance::invCommand(Byte value)
{
    switch (value)
    {
        case OP_CODE_PLUS:
        case OP_CODE_MINUS:
        case OP_CODE_TIMES:
        case OP_CODE_DIVIDE:
        case OP_CODE_SIN:
        case OP_CODE_COS:
        case OP_CODE_TAN:
            mathOpCode_ = value+OP_CODE_OFFSET;
            cycleCounter_ = 1;
        break;

        default: 
            firstInstrucionWord(value);
        break;
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

void Mm57109Instance::convert(double reg)
{
    if (floatingPointMode_)
        mantissaConvert(reg);
    else
        exponentConvert(reg);
}

void Mm57109Instance::mantissaConvert(double reg)
{
    if (reg < 0)
    {
        outputRegister[0] = 8;
        reg = -reg;
    }
    else
        outputRegister[0] = 0;

    int intPartRegister = (int) reg;
    int numberOfDigits = count_digit(intPartRegister);
    if (numberOfDigits > mdc_)
        numberOfDigits = mdc_;
    outputRegister[1] = 12-numberOfDigits;
    intPartRegister = (double) reg * pow(10, mdc_-numberOfDigits);
    for (int digit=9; digit>1; digit--)
    {
        outputRegister[digit] = intPartRegister % 10;
        intPartRegister /= 10;
    }
}

void Mm57109Instance::exponentConvert(double reg)
{
    outputRegister[3] = 0x9b;
    int intPartRegister = (int) reg;
    int numberOfDigits = count_digit(intPartRegister);
    
    if (numberOfDigits == 0)
        exponentConvertBelowOne(reg);
    else
        exponentConvertAboveOne(reg, numberOfDigits);
}

void Mm57109Instance::exponentConvertAboveOne(double reg, int numberOfDigits)
{
    outputRegister[0] = ((numberOfDigits-1)/10) % 10;
    outputRegister[1] = (numberOfDigits-1) % 10;
    outputRegister[2] = 0;

    reg = reg / pow(10, numberOfDigits-mdc_);
    int intPartRegister = (int) reg;

    for (int digit=11; digit>3; digit--)
    {
        outputRegister[digit] = intPartRegister % 10;
        intPartRegister = intPartRegister / 10;
    }
}

void Mm57109Instance::exponentConvertBelowOne(double reg)
{
    int exponent = 0;
    int intPartRegister = (int) reg;
    while (intPartRegister == 0)
    {
        exponent++;
        reg *= 10;
        intPartRegister = (int) reg;
    }
    outputRegister[0] = (exponent/10) % 10;
    outputRegister[1] = exponent % 10;
    outputRegister[2] = 8;

    for (int digit=4; digit<12; digit++)
    {
        outputRegister[digit] = intPartRegister % 10;
        reg *= 10;
        intPartRegister = (int) reg;
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
    calculateDigitEntry();
    convert(registerX);
}

void Mm57109Instance::calculateDigitEntry()
{
    if (floatingPointMode_)
    {
        if (dpNumber_ != -1)
            inputRegisterX.exponent = dpNumber_-inputDigitNumber_;
        else
            inputRegisterX.exponent = 0;
    }

    registerX = (double) inputRegisterX.mantissa * pow(10, inputRegisterX.exponent);
    registerX *= inputRegisterX.mantissaSign;
}

void Mm57109Instance::mantissaEntry(int number)
{
    if (inputDigitNumber_== 0)
    {
            if (lastOpCode_ != OP_CODE_ENTER)
                pushStack();
            clearX();
    }

    if (inputDigitNumber_== mdc_)
        return;

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
    
//    calculateDigitEntry();
    
    inputDigitNumber_ = 0;
    dpNumber_ = -1;
    eeNumber_ = -1;
}



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
#include <math.h>

int factor[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};

int intructionCycleTime[] =
{
    238,	//  OP_CODE_DIGIT_0
    238,    //  OP_CODE_DIGIT_1
    238,    //  OP_CODE_DIGIT_2
    238,    //  OP_CODE_DIGIT_3
    238,    //  OP_CODE_DIGIT_4
    238,    //  OP_CODE_DIGIT_5
    238,    //  OP_CODE_DIGIT_6
    238,    //  OP_CODE_DIGIT_7
    238,    //  OP_CODE_DIGIT_8
    238,    //  OP_CODE_DIGIT_9
    152,    //  OP_CODE_DP
    151,    //  OP_CODE_EE
    166,    //  OP_CODE_CS
    1312,	//  OP_CODE_PI
    284,	//  OP_CODE_AIN
    134,	//  OP_CODE_HALT
    208,	//  OP_CODE_TJC
    278, 	//  OP_CODE_TX0
    197, 	//  OP_CODE_TXLT0
    277, 	//  OP_CODE_TXF
    191, 	//  OP_CODE_TERR
    186, 	//  OP_CODE_JMP
    583, 	//  OP_CODE_OUT
    395, 	//  OP_CODE_IN
    163, 	//  OP_CODE_SMDC
    2314, 	//  OP_CODE_IBNZ
    2314, 	//  OP_CODE_DBNZ
    812, 	//  OP_CODE_XEM
    839, 	//  OP_CODE_MS
    1385, 	//  OP_CODE_MR
    168, 	//  OP_CODE_LSH
    173, 	//  OP_CODE_RSH
    166, 	//  OP_CODE_INV
    552, 	//  OP_CODE_ENTER
    157, 	//  OP_CODE_TOGM
    905, 	//  OP_CODE_ROLL
    56200, 	//  OP_CODE_SIN
    56200, 	//  OP_CODE_COS
    35000, 	//  OP_CODE_TAN
    163, 	//  OP_CODE_SF1
    185, 	//  OP_CODE_PF1
    163, 	//  OP_CODE_SF2
    185, 	//  OP_CODE_PF2
    163, 	//  OP_CODE_ECLR
    9600, 	//  OP_CODE_RTD
    9600, 	//  OP_CODE_DTR
    448, 	//  OP_CODE_POP
    734, 	//  OP_CODE_MCLR
    652, 	//  OP_CODE_XEY
    30800, 	//  OP_CODE_EX
    27400, 	//  OP_CODE_10X
    3000, 	//  OP_CODE_SQ
    7000, 	//  OP_CODE_SQRT
    24800, 	//  OP_CODE_LN
    30700, 	//  OP_CODE_LOG
    4500, 	//  OP_CODE_1X
    55400, 	//  OP_CODE_YX
    2200, 	//  OP_CODE_PLUS
    2200, 	//  OP_CODE_MINUS
    3200, 	//  OP_CODE_TIMES
    7800, 	//  OP_CODE_DIVIDE
    130, 	//  OP_CODE_PRW1
    130, 	//  OP_CODE_PRW2
    122, 	//  OP_CODE_NOP
    54000, 	//  OP_CODE_SIN_INV
    54000, 	//  OP_CODE_COS_INV
    30200, 	//  OP_CODE_TAN_INV
    1700, 	//  OP_CODE_PLUS_INV
    1700, 	//  OP_CODE_MINUS_INV
    2700, 	//  OP_CODE_TIMES_INV
    7300, 	//  OP_CODE_DIVIDE_INV
};

Mm57109Instance::Mm57109Instance()
{
    inputDigitNumber_ = 0;
    outputDigitNumber_ = 0;
    dpNumber_ = -1;
    eeNumber_ = -1;
    linkDigit_ = 0;
    firstInstructionWord_ = true;
    instructionCycleCounter_ = -1;
    rdyCycleCounter_ =  -1;
    f1CycleCounter_ =  -1;
    f2CycleCounter_ =  -1;
    for (int led=0; led<2; led++)
        mathLedStatus[led] = 0;
    moreDigitsThanMdc_ = false;
}

void Mm57109Instance::masterClear()
{
    dataReady_ = 0;
    error_ = 0;
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
    
    masterClear();
    rdy_ = 0x40;
}

void Mm57109Instance::setSpeedFactor(double cpuClock, double ncuClock)
{
    double ncuCycleTime = 4 / ncuClock;
    double cpuCyleTime = 8 / cpuClock;
    
    speedFactor_ = ncuCycleTime / cpuCyleTime;
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

void Mm57109Instance::output(Byte value)
{
    hold_ = 0;
    rdy_ = 0;

    if (firstInstructionWord_)
        firsInstructionWord((OpCodes)(value & 0x3f));
    else
        secondInstrucionWord((OpCodes)(value & 0x3f));
    
    hold_ = 1;
}

Byte Mm57109Instance::input()
{
    Byte returnValue = 0;
    
    if (dataReady_ == 1 && rdy_ == 0 && error_ == 0)
        returnValue = outputRegister[outputDigitNumber_++];
    
    if ((outputDigitNumber_ == 10 && floatingPointMode_) || (outputDigitNumber_ == 12 && !floatingPointMode_))
    {
        dataReady_ = 0;
        outputDigitNumber_ = 0;
        readyPulse();
    }
        
    return returnValue | rdy_ | error_ | 0x90;
}

void Mm57109Instance::firsInstructionWord(OpCodes opCode)
{
    firstInstructionWord_ = true;

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
        case OP_CODE_DP:
        case OP_CODE_EE:
        case OP_CODE_CS:
        case OP_CODE_PI:
        case OP_CODE_HALT:
        case OP_CODE_AIN:
            mathOpCode_ = opCode;
            setExecutionTimeNumberEntry();
        break;
        
        // Non digit entry commands:
            
        case OP_CODE_ENTER:
        case OP_CODE_NOP:
        case OP_CODE_PLUS:
        case OP_CODE_MINUS:
        case OP_CODE_TIMES:
        case OP_CODE_YX:
        case OP_CODE_SQ:
        case OP_CODE_10X:
        case OP_CODE_EX:
        case OP_CODE_DTR:
        case OP_CODE_RTD:
        case OP_CODE_XEM:
        case OP_CODE_MS:
        case OP_CODE_MR:
        case OP_CODE_LSH:
        case OP_CODE_RSH:
        case OP_CODE_ROLL:
        case OP_CODE_POP:
        case OP_CODE_XEY:
        case OP_CODE_MCLR:
        case OP_CODE_ECLR:
        case OP_CODE_TOGM:
        case OP_CODE_TJC:
        case OP_CODE_TX0:
        case OP_CODE_TXLT0:
        case OP_CODE_TXF:
        case OP_CODE_TERR:
        case OP_CODE_JMP:
        case OP_CODE_IBNZ:
        case OP_CODE_DBNZ:
        case OP_CODE_SF1:
        case OP_CODE_PF1:
        case OP_CODE_SF2:
        case OP_CODE_PF2:
        case OP_CODE_PRW1:
        case OP_CODE_PRW2:
            executeNonDigitCommand(opCode);
        break;

        case OP_CODE_LN:
        case OP_CODE_LOG:
            if (registerX <= 0)
                error_ = 0x20;
            executeNonDigitCommand(opCode);
        break;

        case OP_CODE_SQRT:
            if (registerX < 0)
                error_ = 0x20;
            executeNonDigitCommand(opCode);
        break;

        case OP_CODE_SIN:
        case OP_CODE_COS:
            if (abs(registerX) >= 9000)
                error_ = 0x20;
            executeNonDigitCommand(opCode);
        break;

        case OP_CODE_TAN:
            if (abs(registerX) >= 9000 || cos(registerX) == 0)
                error_ = 0x20;
            executeNonDigitCommand(opCode);
        break;

        case OP_CODE_DIVIDE:
        case OP_CODE_1X:
            if (registerX == 0)
                error_ = 0x20;
            executeNonDigitCommand(opCode);
        break;
                        
        // 2 byte commands:
        
        case OP_CODE_IN:
            stopDigitEntry();
            inputDigitNumber_ = 0;
            if (lastOpCode_ != OP_CODE_ENTER)
                pushStack();
            firstInstructionWord_ = false;
            readyPulse();
        break;
            
        case OP_CODE_OUT:
        case OP_CODE_SMDC:
        case OP_CODE_INV:
            stopDigitEntry();
            firstInstructionWord_ = false;
            readyPulse();
        break;

        case OP_CODE_SIN_INV:
        case OP_CODE_COS_INV:
        case OP_CODE_TAN_INV:
        case OP_CODE_PLUS_INV:
        case OP_CODE_MINUS_INV:
        case OP_CODE_TIMES_INV:
        case OP_CODE_DIVIDE_INV:
        break;
    }
    lastOpCode_ = opCode;
}

void Mm57109Instance::setExecutionTimeNumberEntry()
{
    int extraCycles = 67;
    if (lastOpCode_ != OP_CODE_ENTER)
        extraCycles = 282;
    
    if (inputDigitNumber_== 0)
        instructionCycleCounter_ = (intructionCycleTime[mathOpCode_] + extraCycles) * speedFactor_;
    else
        instructionCycleCounter_ = intructionCycleTime[mathOpCode_] * speedFactor_;
}

void Mm57109Instance::setExecutionTimeNoNumberEntry()
{
    if (inputDigitNumber_== 0)
        instructionCycleCounter_ = intructionCycleTime[mathOpCode_] * speedFactor_;
    else
        instructionCycleCounter_ = (intructionCycleTime[mathOpCode_] + 1003) * speedFactor_;
}

void Mm57109Instance::executeNonDigitCommand(OpCodes opCode)
{
    stopDigitEntry();
    mathOpCode_ = opCode;
    setExecutionTimeNoNumberEntry();
}

void Mm57109Instance::secondInstrucionWord(OpCodes opCode)
{
    firstInstructionWord_ = true;

    switch (lastOpCode_)
    {
        // Multi-digit commands:

        case OP_CODE_OUT:
            if (moreDigitsThanMdc_)
            {
                error_ = 0x20;
                moreDigitsThanMdc_ = false;
            }
            else
            {
                outputDigitNumber_ = 0;
                dataReady_ = 1;
            }
            mathOpCode_ = lastOpCode_;
        break;

        case OP_CODE_IN:
            mathOpCode_ = lastOpCode_;
        break;

        // Mode control commands:

        case OP_CODE_SMDC:
        case OP_CODE_INV:
            checkInvError(opCode);
            mathOpCode_ = lastOpCode_;
            secondOpCode_ = opCode;
            setExecutionTimeNoNumberEntry();
        break;

        default:
            firsInstructionWord(opCode);
        break;
    }
}
   
void Mm57109Instance::checkInvError(OpCodes opCode)
{
    switch (opCode)
    {
        case OP_CODE_SIN:
        case OP_CODE_COS:
            if (abs(registerX) >= 9000)
                error_ = 0x20;
            if (abs(registerX) <= pow(10, -50))
                error_ = 0x20;
        break;
            
        case OP_CODE_DIVIDE:
            if (registerX == 0)
                error_ = 0x20;
        break;

        default:
        break;
    }
}

void Mm57109Instance::invCommand(OpCodes opCode)
{
    switch (opCode)
    {
        case OP_CODE_SIN:
            mathOpCode_ = OP_CODE_SIN_INV;
            setExecutionTimeNoNumberEntry();
        break;
            
        case OP_CODE_COS:
            mathOpCode_ = OP_CODE_COS_INV;
            setExecutionTimeNoNumberEntry();
        break;
            
        case OP_CODE_TAN:
            mathOpCode_ = OP_CODE_TAN_INV;
            setExecutionTimeNoNumberEntry();
        break;

        case OP_CODE_PLUS:
            mathOpCode_ = OP_CODE_PLUS_INV;
            setExecutionTimeNoNumberEntry();
        break;

        case OP_CODE_MINUS:
            mathOpCode_ = OP_CODE_MINUS_INV;
            setExecutionTimeNoNumberEntry();
        break;

        case OP_CODE_TIMES:
            mathOpCode_ = OP_CODE_TIMES_INV;
            setExecutionTimeNoNumberEntry();
        break;

        case OP_CODE_DIVIDE:
            mathOpCode_ = OP_CODE_DIVIDE_INV;
            setExecutionTimeNoNumberEntry();
        break;

        default:
            firsInstructionWord(opCode);
        break;
    }
}

void Mm57109Instance::cycle()
{
    if (rdyCycleCounter_ > 0)
    {
        rdyCycleCounter_--;
        if (rdyCycleCounter_ == 0 && hold_ == 0)
            rdy_ = 0;
    }
    if (f1CycleCounter_ > 0)
    {
        f1CycleCounter_--;
        if (f1CycleCounter_ == 0)
            mathLedStatus[0] ^= 1;
        p_Computer->setMathLed(0, mathLedStatus[0]);
    }
    if (f2CycleCounter_ > 0)
    {
        f2CycleCounter_--;
        if (f2CycleCounter_ == 0)
            mathLedStatus[1] ^= 1;
        p_Computer->setMathLed(1, mathLedStatus[1]);
    }
    if (instructionCycleCounter_ > 0)
    {
        instructionCycleCounter_--;
        if (instructionCycleCounter_ == 0)
        {
            switch (mathOpCode_) // math functions
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
                    digitEntry(mathOpCode_);
                break;

                case OP_CODE_DP:
                    dpNumber_ = inputDigitNumber_;
                    calculateDigitEntry();
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
                    calculateDigitEntry();
                    convert(registerX);
                break;

                case OP_CODE_PI:
                    registerX = 3.1415927;
                    convert(registerX);
                break;

                case OP_CODE_NOP:
                break;

                case OP_CODE_HALT:
                    hold_ = 1;
                break;

                case OP_CODE_ENTER:
                    pushStack();
               break;

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

                // Move commands:

                case OP_CODE_ROLL:
                    rollStack();
                    convert(registerX);
                break;
                
                case OP_CODE_POP:
                    registerX = registerY;
                    popStack();
                    convert(registerX);
                break;
                
                case OP_CODE_XEY:
                    exchangeXY();
                    convert(registerX);
                break;
                
                case OP_CODE_XEM:
                    exchangeXM();
                    convert(registerX);
                break;

                case OP_CODE_MS:
                    registerM = registerX;
                break;

                case OP_CODE_MR:
                    pushStack();
                    registerX = registerM;
                    convert(registerX);
                break;

                case OP_CODE_LSH:
                    shiftLeft();
                    convert(registerX);
                break;

                case OP_CODE_RSH:
                    shiftRight();
                    convert(registerX);
                break;

                // Clear commands:
                
                case OP_CODE_MCLR:
                    masterClear();
                    convert(0);
                break;
           
                case OP_CODE_ECLR:
                    error_ = 0;
                break;
                    
                // Branch commands:

                case OP_CODE_TJC:
                case OP_CODE_TX0:
                case OP_CODE_TXLT0:
                case OP_CODE_TXF:
                case OP_CODE_TERR:
                case OP_CODE_JMP:
                case OP_CODE_IBNZ:
                case OP_CODE_DBNZ:
                    // NOT IMPLEMENTED
                break;

                // Flags:
                case OP_CODE_SF1:
                    mathLedStatus[0] = 1;
                    p_Computer->setMathLed(0, mathLedStatus[0]);
                break;
                    
                case OP_CODE_PF1:
                    f1Pulse(1);
               break;
                    
                case OP_CODE_SF2:
                    mathLedStatus[1] = 1;
                    p_Computer->setMathLed(1, mathLedStatus[1]);
                break;
                    
                case OP_CODE_PF2:
                    f2Pulse(1);
                break;
                    
                case OP_CODE_PRW1:
                    // NOT IMPLEMENTED
                break;

                case OP_CODE_PRW2:
                    // NOT IMPLEMENTED
                break;

                // Multi-digit commands:

                case OP_CODE_IN:
                    digitEntry(mathOpCode_ & 0xf);
                    if ((inputDigitNumber_ == 10 && floatingPointMode_) || (inputDigitNumber_ == 12 && !floatingPointMode_))
                        inputDigitNumber_ = 0;
                    else
                        firstInstructionWord_ = false;
                break;
                
                case OP_CODE_OUT:
                    outputDigitNumber_ = 0;
                return;

                // Single-digit commands:
                    
                case OP_CODE_AIN:
                    // NOT IMPLEMENTED
                    // mantissaEntry(mathOpCode_ & 0xf);
                    // calculateDigitEntry();
                    // convert(registerX);
                    // f2Pulse(0);
                break;
                            
                // Mode control commands:

                case OP_CODE_TOGM:
                    floatingPointMode_ = !floatingPointMode_;
                break;

                case OP_CODE_SMDC:
                    mdc_ = secondOpCode_;
                    if (mdc_ > 8)
                        mdc_ = 8;
                break;

                case OP_CODE_INV:
                    invCommand(secondOpCode_);
                return; // return instead of break - as invCommand routine has set a new instructionCycleCounter_ value

                default:
                break;
            }
            instructionCycleCounter_ = -1;
            readyPulse();
        }
    }
}

Byte Mm57109Instance::ef()
{
    Byte ef = (error_ >> 5) | dataReady_;
    return (mm57109Configuration_.ef.reverse^ef);
}

void Mm57109Instance::readyPulse()
{
    rdy_ = 0x40;
    rdyCycleCounter_ = 8 * speedFactor_;
}

void Mm57109Instance::f1Pulse(int state)
{
    mathLedStatus[0] = state;
    p_Computer->setMathLed(0, mathLedStatus[0]);
    f1CycleCounter_ = 4 * speedFactor_;
}

void Mm57109Instance::f2Pulse(int state)
{
    mathLedStatus[1] = state;
    p_Computer->setMathLed(1, mathLedStatus[1]);
    f2CycleCounter_ = 4 * speedFactor_;
}
void Mm57109Instance::hold(Byte value)
{
    hold_ = value;
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
    
    registerX = powerMultiplification(inputRegisterX.mantissa, count_digit(inputRegisterX.mantissa)-outputRegister[1]-2);
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
    
    registerX = powerMultiplification(inputRegisterX.mantissa, count_digit(inputRegisterX.mantissa)-outputRegister[1]-3);
    registerX *= inputRegisterX.mantissaSign;

    linkDigit_ = 0;
}

double Mm57109Instance::powerMultiplification(unsigned int mantissa, int exponent)
{
    double result;
    
    int power = pow(10, abs(exponent));
    if (exponent < 0)
        result = (double) mantissa / power;
    else
        result = (double) mantissa * power;
    return result;
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
    wxString outputFormat;
    outputFormat.Printf("%.8f", reg); // format 12345678.12345678 or -12345678.12345678

    if (outputFormat.GetChar(0) == '-')
    {
        outputRegister[0] = 8;     // digit value is negative
        outputFormat = outputFormat.Right(outputFormat.Len()-1);
    }
    else
        outputRegister[0] = 0;     // digit value is positive

    if (outputFormat.GetChar(0) == '0' && reg != 0)
        outputFormat = outputFormat.Right(outputFormat.Len()-1);

    wxString digitsBeforeDigitalPoint;
    if (outputFormat.Find('.'))
        digitsBeforeDigitalPoint = outputFormat.Before('.');
    else
        digitsBeforeDigitalPoint = outputFormat.Before(',');

    size_t numberOfDigitsBeforeDigitalPoint = digitsBeforeDigitalPoint.Len();
    if (numberOfDigitsBeforeDigitalPoint > mdc_)
        moreDigitsThanMdc_ = true;
    outputRegister[1] = 12 - numberOfDigitsBeforeDigitalPoint;

    size_t charNumber = 0;
    for (int digit = 2; digit < mdc_+2; digit++)             // get digits
    {
        if (outputFormat.GetChar(charNumber) < 0x30)
            charNumber++;
        outputRegister[digit] = outputFormat.GetChar(charNumber++) - (wxUniChar)0x30;
    }
    for (int digit = mdc_+2; digit < 10; digit++)
        outputRegister[digit] = 0;
    /*

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
//    intPartRegister = (intPartRegister + 5)/10;
    for (int digit=9; digit>mdc_+1; digit--)
        outputRegister[digit] = 0;
    for (int digit=mdc_+1; digit>1; digit--)
    {
        outputRegister[digit] = intPartRegister % 10;
        intPartRegister /= 10;
    }
    if (intPartRegister != 0)
        moreDigitsThanMdc_ = true;*/
}

void Mm57109Instance::exponentConvert(double reg)
{
    wxString outputFormat;
    outputFormat.Printf("%1.7e", reg); // format 1.2345678e+12 or -1.2345678e+12

    size_t charNumber = 0;

    if (outputFormat.GetChar(charNumber) == '-')
    {
        outputRegister[2] = 8;     // digit value is negative
        charNumber++;
    }
    else
        outputRegister[2] = 0;     // digit value is positive

    outputRegister[3] = 0x9b;      // digital point is not used - i.e. always 0x9b
    
    outputRegister[4] = outputFormat.GetChar(charNumber++) - (wxUniChar)0x30;
    charNumber++;                  // skip the digital point as it is always in the same position

    for (int digit = 5; digit < mdc_+4; digit++)             // get digit 2 to 8
        outputRegister[digit] = outputFormat.GetChar(charNumber++) - (wxUniChar)0x30;
    for (int digit = mdc_+4; digit < 12; digit++)
        outputRegister[digit] = 0;

    charNumber++;                  // skip 'e' character

    if (outputFormat.GetChar(charNumber++) == '-')
        outputRegister[2] |= 1;    // exponent value is negative

    size_t exponent = 0;
    while (charNumber < outputFormat.Len())
    {
        exponent *= 10;
        exponent += outputFormat.GetChar(charNumber++) - (wxUniChar)0x30;
    }
    
    outputRegister[0] = (exponent/10) % 10;
    outputRegister[1] = exponent % 10;

    if (exponent > 99)
        error_ = 0x20;
}

int Mm57109Instance::count_digit(uint64_t number)
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

    registerX = (double) inputRegisterX.mantissa * pow(10, inputRegisterX.exponent*inputRegisterX.exponentSign);
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



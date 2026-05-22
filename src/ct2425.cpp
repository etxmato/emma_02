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
#include "ct2425.h"

Ct2425::Ct2425()
{
    gatesAndLedsRegister_ = 0;
    sensorStatus_ = 0;
    coinStatus_ = 0;
    sensorClear_ = 0xff;
    setSensors();
    coinState_ = NO_COIN;
    upperEscrowCoins_ = 0;
    lowerEscrowCoins_ = 0;

    sensorCycleValue_ = -1;
    
    spmState_ = 1;
    spmCycleValue_ = -1;
    spmCycleValueOn_ = -1;

    for (int gate=0; gate <= 5; gate++)
    {
        gateStatus_[gate].direction = GATE_STOPPED;
        gateStatus_[gate].state = GATE_INACTIVE;
        gateStatus_[gate].cycleValue_ = -1;
    }
}

Ct2425::~Ct2425()
{
}

void Ct2425::Configure(Ct2425Configuration ct2425Io, int number, double clockSpeed)
{
    ct2425Io_ = ct2425Io;
    spmCycleSize_ = (int) ((clockSpeed * 1000000 * ct2425Io_.spm_interval) / 8);
    spmPulseCycleSize_ = (int) (((clockSpeed * 1000000 * ct2425Io_.spm_pulse) / 8)/1000);
    
    ignoreLedCycleSize_ = (int) (((clockSpeed * 1000000 * ct2425Io_.ignore_led) / 8)/1000000);
    sensorBefore_1_CycleSize_ = (int) (((clockSpeed * 1000000 * ct2425Io_.before_1) / 8)/1000);
    sensorBlock_1_CycleSize_ = (int) (((clockSpeed * 1000000 * ct2425Io_.block_1) / 8)/1000);
    sensorBetween_1_2_CycleSize_ = (int) (((clockSpeed * 1000000 * ct2425Io_.between_1_2) / 8)/1000);
    sensorBlock_2_CycleSize_ = (int) (((clockSpeed * 1000000 * ct2425Io_.block_2) / 8)/1000);
    sensorBetween_2_x_CycleSize_ = (int) (((clockSpeed * 1000000 * ct2425Io_.between_2_x) / 8)/1000);
    sensorAfter_gate_5_6_CycleSize_ = (int) (((clockSpeed * 1000000 * ct2425Io_.after_gate_5_6) / 8)/1000);
    sensorBlock_5_CycleSize_ = (int) (((clockSpeed * 1000000 * ct2425Io_.block_5) / 8)/1000);
    
    p_Main->configureMessage(&ct2425Io.ioGroupVector, "CT Payphone");
    p_Computer->setOutType(&ct2425Io.ioGroupVector, ct2425Io.validators, "set validators", number);
    p_Computer->setOutType(&ct2425Io.ioGroupVector, ct2425Io.gates, "set gates and leds", number);
    p_Computer->setOutType(&ct2425Io.ioGroupVector, ct2425Io.control, "set control", number);
    p_Computer->setInType(&ct2425Io.ioGroupVector, ct2425Io.sensors, "read sensors", number);
    p_Computer->setInType(&ct2425Io.ioGroupVector, ct2425Io.coins, "read coin value & status", number);
    p_Computer->setEfType(&ct2425Io.ioGroupVector, ct2425Io.efSpm, "SPM");
    p_Computer->setCycleType(CYCLE_TYPE_CT2425, CT2425_IO_CYCLE);
    
    p_Main->message("");
    
    p_Main->setCt2425Register(CT2425_GATES, 0, DO_NOT_SHOW_ANY_TRACE);

    p_Main->setCt2425SelectorValue(CT2425_GATE1, false, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setCt2425SelectorValue(CT2425_GATE2, false, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setCt2425SelectorValue(CT2425_GATE3, false, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setCt2425SelectorValue(CT2425_GATE4, false, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setCt2425SelectorValue(CT2425_GATE5, false, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setCt2425SelectorValue(CT2425_GATE6, false, DO_NOT_SHOW_ANY_TRACE);

    p_Main->setCt2425SelectorValue(CT2425_SENSOR1, true, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setCt2425SelectorValue(CT2425_SENSOR2, true, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setCt2425SelectorValue(CT2425_SENSOR3, true, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setCt2425SelectorValue(CT2425_SENSOR4, true, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setCt2425SelectorValue(CT2425_SENSOR5, true, DO_NOT_SHOW_ANY_TRACE);

    p_Main->setCt2425Register(CT2425_LOWER_ESCROW, 0, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setCt2425Register(CT2425_UPPER_ESCROW, 0, DO_NOT_SHOW_ANY_TRACE);
}

void Ct2425::cycle()
{
    int showTrace = SHOW_ADDRESS_TRACE;
    
    if (spmCycleValue_ > 0)
    {
        spmCycleValue_ --;
        if (spmCycleValue_ <= 0)
        {
            spmState_ = 0;
            showTrace = p_Main->setCt2425SelectorValue(CT2425_SPM, false, showTrace);
            spmCycleValue_ = spmCycleSize_;
            spmCycleValueOn_ = spmPulseCycleSize_;
        }
    }
    
    if (spmCycleValueOn_ > 0)
    {
        spmCycleValueOn_ --;
        if (spmCycleValueOn_ <= 0)
            spmState_ = 1;
    }

    if (sensorCycleValue_ > 0)
    {
        sensorCycleValue_ --;
        if (sensorCycleValue_ <= 0)
        {
            sensorCycleValue_ = -1;

            switch (coinState_)
            {
                case BEFORE_SENSOR_1:
                    sensorClear_[ct2425Io_.sensor_after_gate_1] = SENSOR_BLOCKED;
                    coinState_ = AT_SENSOR_1;
                    sensorCycleValue_ = sensorBlock_1_CycleSize_;
                    showTrace = setSensors(showTrace);
                    showTrace = p_Main->setCt2425SelectorValue(CT2425_SENSOR1, false, showTrace);
                break;
                    
                case AT_SENSOR_1:
                    sensorClear_[ct2425Io_.sensor_after_gate_1] = SENSOR_CLEAR;
                    coinState_ = AFTER_SENSOR_1;
                    sensorCycleValue_ = sensorBetween_1_2_CycleSize_;
                    showTrace = setSensors(showTrace);
                    showTrace = p_Main->setCt2425SelectorValue(CT2425_SENSOR1, true, showTrace);
                break;

                case AFTER_SENSOR_1:
                    if (gateStatus_[ct2425Io_.gate_second].state == GATE_ACTIVE)
                    {
                        sensorClear_[ct2425Io_.sensor_after_gate_2] = SENSOR_BLOCKED;
                        coinState_ = AT_SENSOR_2;
                        sensorCycleValue_ = sensorBlock_2_CycleSize_;
                        showTrace = setSensors(showTrace);
                        showTrace = p_Main->setCt2425SelectorValue(CT2425_SENSOR2, false, showTrace);
                    }
                    if (gateStatus_[ct2425Io_.gate_second].state == GATE_INACTIVE)
                    {
                        coinState_ = AT_SENSOR_4;
                        upperEscrowCoins_++;
                        sensorClear_[ct2425Io_.sensor_upper_escrow] = SENSOR_BLOCKED;
                        showTrace = setSensors(showTrace);
                        showTrace = p_Main->setCt2425SelectorValue(CT2425_SENSOR4, false, showTrace);
                        showTrace = p_Main->setCt2425Register(CT2425_UPPER_ESCROW, upperEscrowCoins_, showTrace);
                    }
                break;

                case AT_SENSOR_2:
                    sensorClear_[ct2425Io_.sensor_after_gate_2] = SENSOR_CLEAR;
                    coinState_ = AFTER_SENSOR_2;
                    sensorCycleValue_ = sensorBetween_2_x_CycleSize_;
                    showTrace = setSensors(showTrace);
                    showTrace = p_Main->setCt2425SelectorValue(CT2425_SENSOR2, true, showTrace);
                break;

                case AFTER_SENSOR_2:
                    coinState_ = AT_SENSOR_3;
                    lowerEscrowCoins_++;
                    sensorClear_[ct2425Io_.sensor_lower_escrow] = SENSOR_BLOCKED;
                    showTrace = setSensors(showTrace);
                    showTrace = p_Main->setCt2425SelectorValue(CT2425_SENSOR3, false, showTrace);
                    showTrace = p_Main->setCt2425Register(CT2425_LOWER_ESCROW, lowerEscrowCoins_, showTrace);
                break;

                case AFTER_GATE_5_6:
                    coinState_ = AT_SENSOR_5;
                    sensorCycleValue_ = sensorBlock_5_CycleSize_;
                    sensorClear_[ct2425Io_.sensor_return] = SENSOR_BLOCKED;
                    showTrace = setSensors(showTrace);
                    showTrace = p_Main->setCt2425SelectorValue(CT2425_SENSOR5, false, showTrace);
                break;

                case AT_SENSOR_5:
                    sensorClear_[ct2425Io_.sensor_return] = SENSOR_CLEAR;
                    showTrace = setSensors(showTrace);
                    showTrace = p_Main->setCt2425SelectorValue(CT2425_SENSOR5, true, showTrace);
                break;
            }
        }
    }
    
    for (int gate=0; gate <= 5; gate++)
    {
        if (gateStatus_[gate].cycleValue_ > 0)
        {
            gateStatus_[gate].cycleValue_ --;
            if (gateStatus_[gate].cycleValue_ <= 0)
            {
                gateStatus_[gate].cycleValue_ = -1;
                
                if (gateStatus_[gate].direction != GATE_STOPPED)
                {
                    if (gateStatus_[gate].direction == GATE_GOING_ACTIVE)
                    {
                        gateStatus_[gate].state = GATE_ACTIVE;
                        
                        if (gate == 5 && !gatesTraced_)
                            showTrace = p_Main->setCt2425Register(CT2425_GATES, (Byte)gatesAndLedsRegister_.to_ulong(), showTrace);
                        showTrace = p_Main->setCt2425SelectorValue(CT2425_GATE1+gate, gateStatus_[gate].state, showTrace);
                        if (gate == ct2425Io_.gate_first)
                        {
                            coinState_ = BEFORE_SENSOR_1;
                            sensorCycleValue_ = sensorBefore_1_CycleSize_;
                        }
                        if (gate == ct2425Io_.gate_upper_escrow_cash || gate == ct2425Io_.gate_upper_escrow_return)
                        {
                            upperEscrowCoins_--;
                            if (upperEscrowCoins_ < 0)
                                upperEscrowCoins_ = 0;
                            showTrace = p_Main->setCt2425Register(CT2425_UPPER_ESCROW, upperEscrowCoins_, showTrace);
                            if (upperEscrowCoins_ == 0)
                            {
                                sensorClear_[ct2425Io_.sensor_upper_escrow] = SENSOR_CLEAR;
                                showTrace = setSensors(showTrace);
                                showTrace = p_Main->setCt2425SelectorValue(CT2425_SENSOR4, true, showTrace);
                            }
                        }
                        if (gate == ct2425Io_.gate_lower_escrow_cash || gate == ct2425Io_.gate_lower_escrow_return)
                        {
                            lowerEscrowCoins_--;
                            if (lowerEscrowCoins_ < 0)
                                lowerEscrowCoins_ = 0;
                            showTrace = p_Main->setCt2425Register(CT2425_LOWER_ESCROW, lowerEscrowCoins_, showTrace);
                            if (lowerEscrowCoins_ == 0)
                            {
                                sensorClear_[ct2425Io_.sensor_lower_escrow] = SENSOR_CLEAR;
                                showTrace = setSensors(showTrace);
                                showTrace = p_Main->setCt2425SelectorValue(CT2425_SENSOR3, true, showTrace);
                            }
                        }
                        if (gate == ct2425Io_.gate_lower_escrow_return || gate == ct2425Io_.gate_upper_escrow_return)
                        {
                            coinState_ = AFTER_GATE_5_6;
                            sensorCycleValue_ = sensorAfter_gate_5_6_CycleSize_;
                        }
                    }
                    if (gateStatus_[gate].direction == GATE_GOING_INACTIVE)
                    {
                        gateStatus_[gate].state = GATE_INACTIVE;
                        if (gate == 5 && !gatesTraced_)
                            showTrace = p_Main->setCt2425Register(CT2425_GATES, (Byte)gatesAndLedsRegister_.to_ulong(), showTrace);
                        showTrace = p_Main->setCt2425SelectorValue(CT2425_GATE1+gate, gateStatus_[gate].state, showTrace);
                    }
                }
            }
        }
    }
}

Byte Ct2425::efSpm()
{
    return spmState_;
}


int Ct2425::writeRegisterCt2425(Byte registerIndex, Byte value, int showTrace)
{
    switch (registerIndex)
    {
        case CT2425_GATES:
            setGatesAndLeds(value, showTrace);
        break;

        case CT2425_VALIDATOR:
            setValidator(value, showTrace);
        break;

        case CT2425_CONTROL:
            setControl(value, showTrace);
        break;
    }
    return showTrace;
}

void Ct2425::answerCall()
{
    spmState_ = 0;
    p_Main->setCt2425SelectorValue(CT2425_SPM, false, SHOW_ADDRESS_TRACE);
    spmCycleValue_ = spmCycleSize_;
    spmCycleValueOn_ = spmPulseCycleSize_;
}

void Ct2425::insertCoin(Byte value, int showTrace)
{
    coinStatus_ = 0;
    if (value >= 0xa && value <= 0xf)
    {
        value -= 0xa;
        coinStatus_ = 1 << value;
        coinStatus_[COIN_DAV] = 1;
    }
    
    Byte coin = 0;
    Byte coinStatus = coinStatus_.to_ulong() & 0x3f;
    while (coinStatus != 0)
    {
        coin++;
        coinStatus = coinStatus >> 1;
    }
//    showTrace = p_Main->timeTrace(-1, -1, CT2425_COINS, -1, true, showTrace);
    if (coin != 0)
        showTrace = p_Main->setCt2425SelectorValue(CT2425_LAST_COIN, coin, showTrace);
    showTrace = p_Main->setCt2425Register(CT2425_COINS, (int)coinStatus_.to_ulong(), showTrace);
}

void Ct2425::setValidator(Byte value, int showTrace)
{
    p_Main->setCt2425Register(CT2425_VALIDATOR, value, showTrace);
    if (value & 0x4)
    {
        coinStatus_ = 0;
        p_Main->setCt2425Register(CT2425_COINS, (int)coinStatus_.to_ulong(), showTrace);
    }
}

/*
 Bit 0: Gate 1 - 50, 100 => upper escrow to gate 4/6
 Bit 1: Gate 2 - 10, 20 => lower escrow to gate 3/5
 Bit 2: Gate 3
 Bit 3: Gate 4
 Bit 4: Gate 5
 But 5: Sensor LEDs 3 & 4 and Gate 6.
 Bit 6: Sensor LEDs 1 & 2 and Sensor 5 LED
*/

void Ct2425::setGatesAndLeds(Byte value, int showTrace)
{
    p_Main->setCt2425Register(CT2425_GATES_AND_LEDS, value, showTrace);
    
    showTrace = setGates(value, showTrace);
    showTrace = setLeds(value, showTrace);
    
    gatesAndLedsRegister_ = value;
    setSensors(showTrace);
}

int Ct2425::setGates(Byte value, int showTrace)
{
    bitset<8> newGates = value;
    gatesTraced_ = false;

    for (int gate=0; gate <= 5; gate++)
    {
        if (newGates[gate] != gatesAndLedsRegister_[gate])
        {
            if (gate != 5 && !gatesTraced_)
            {
                showTrace = p_Main->setCt2425Register(CT2425_GATES, value & 0x3f, showTrace);
                gatesTraced_ = true;
            }

            gateStatus_[gate].direction = GATE_STOPPED;
            if (newGates[gate] == 1 && gateStatus_[gate].state != GATE_ACTIVE)
                gateStatus_[gate].direction = GATE_GOING_ACTIVE;
            if (newGates[gate] == 0 && gateStatus_[gate].state != GATE_INACTIVE)
                gateStatus_[gate].direction = GATE_GOING_INACTIVE;
            gateStatus_[gate].cycleValue_ = ignoreLedCycleSize_;
        }
    }
    return showTrace;
}

int Ct2425::setLeds(Byte value, int showTrace)
{
    bitset<8> newLeds_ = value;

    for (int led=SENSOR_LED_3_4; led <= SENSOR_LED_1_2_5; led++)
    {
        if (newLeds_[led] != gatesAndLedsRegister_[led])
            showTrace = p_Main->setCt2425Register(CT2425_LEDS, value & 0x60, showTrace);
    }
    return showTrace;
}

void Ct2425::setControl(Byte value, int showTrace)
{
    p_Main->setCt2425Register(CT2425_CONTROL, value, showTrace);
    if ((value & 0x80) == 0)
    {
        gatesAndLedsRegister_ = 0;
        sensorStatus_ = 0;
        coinStatus_ = 0;
        sensorClear_ = 0xff;
        setSensors();
        coinState_ = NO_COIN;

        sensorCycleValue_ = -1;

        for (int gate=0; gate <= 5; gate++)
        {
            gateStatus_[gate].direction = GATE_STOPPED;
            gateStatus_[gate].state = GATE_INACTIVE;
            gateStatus_[gate].cycleValue_ = -1;
        }
        p_Computer->resetCpu();
        p_Computer->resetComputer();
    }
    if ((value & 0x40) == 0)
    {
        if (lowerEscrowCoins_ == 0 && upperEscrowCoins_ == 0)
        {
            spmState_ = 1;
            spmCycleValue_ = -1;
            spmCycleValueOn_ = -1;
        }
    }
}

/*
 Bit 5: Sensor 1 & 3
 Bit 6: Sensor 2 & 4
 Bit 7: Sensor 5
 */

Byte Ct2425::readSensors()
{
    setSensors();
    return sensorStatus_.to_ulong();
}

int Ct2425::setSensors(int showTrace)
{
    sensorStatus_[SENSOR_1_3] = 0;
    
    if (gatesAndLedsRegister_[SENSOR_LED_1_2_5] && sensorClear_[ct2425Io_.sensor_after_gate_1])
        sensorStatus_[SENSOR_1_3] = 1;
    
    if (gatesAndLedsRegister_[SENSOR_LED_3_4] && sensorClear_[ct2425Io_.sensor_lower_escrow])
        sensorStatus_[SENSOR_1_3] = 1;
        
    sensorStatus_[SENSOR_2_4] = 0;
    
    if (gatesAndLedsRegister_[SENSOR_LED_1_2_5] && sensorClear_[ct2425Io_.sensor_after_gate_2])
        sensorStatus_[SENSOR_2_4] = 1;

    if (gatesAndLedsRegister_[SENSOR_LED_3_4] && sensorClear_[ct2425Io_.sensor_upper_escrow])
        sensorStatus_[SENSOR_2_4] = 1;

    sensorStatus_[SENSOR_5] = 0;
    if (gatesAndLedsRegister_[SENSOR_LED_1_2_5] && sensorClear_[5])
        sensorStatus_[SENSOR_5] = 1;

    return p_Main->setCt2425Register(CT2425_SENSORS, (int)sensorStatus_.to_ulong(), showTrace);
}

Byte Ct2425::readCoins()
{
    return coinStatus_.to_ulong();
}




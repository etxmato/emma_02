#ifndef CT2425_H
#define CT2425_H

#include "computerconfig.h"

// GATE & LED BITS
#define SENSOR_LED_3_4 5
#define SENSOR_LED_1_2_5 6

// SENSOR BITS
#define SENSOR_1_3 5
#define SENSOR_2_4 6
#define SENSOR_5 7

// COIN STATUS
#define COIN_DAV 6

// GATE STATUS
#define GATE_GOING_INACTIVE 0
#define GATE_GOING_ACTIVE 1
#define GATE_STOPPED 2
#define GATE_INACTIVE false
#define GATE_ACTIVE true

// SENSOR STATUS
#define SENSOR_BLOCKED 0
#define SENSOR_CLEAR 1

#define SENSOR_BLOCK_TIME 2000

// COIN STATE
enum
{
    NO_COIN,
    BEFORE_SENSOR_1,
    AT_SENSOR_1,
    AFTER_SENSOR_1,
    AT_SENSOR_2,
    AFTER_SENSOR_2,
    AT_SENSOR_3,
    AT_SENSOR_4,
    AFTER_GATE_5_6,
    AT_SENSOR_5,
};

class GateStatus
{
public:
    int direction;
    bool state;
    int cycleValue_;
};

class Ct2425
{
public:
    Ct2425();
    ~Ct2425();

    void Configure(Ct2425Configuration ct2425Io, int number, double clockSpeed);
    void cycle();
    Byte efSpm();
    int writeRegisterCt2425(Byte registerIndex, Byte value, int showTrace);
    void answerCall();
    void insertCoin(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    void setGatesAndLeds(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setGates(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setLeds(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    void setControl(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    void setValidator(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    Byte readSensors();
    int setSensors(int showTrace = SHOW_ADDRESS_TRACE);
    Byte readCoins();

protected:
    bitset<8> gatesAndLedsRegister_;
    bitset<8> sensorStatus_;
    bitset<8> coinStatus_;
    bitset<8> sensorClear_;

    Ct2425Configuration ct2425Io_;
    
    int coinState_;
    int upperEscrowCoins_;
    int lowerEscrowCoins_;
    bool gatesTraced_;
    
    GateStatus gateStatus_[6];

    int sensorCycleValue_;
    int ignoreLedCycleSize_;
    int sensorBefore_1_CycleSize_;
    int sensorBlock_1_CycleSize_;
    int sensorBetween_1_2_CycleSize_;
    int sensorBlock_2_CycleSize_;
    int sensorBetween_2_x_CycleSize_;
    int sensorAfter_gate_5_6_CycleSize_;
    int sensorBlock_5_CycleSize_;
    
    int spmCycleSize_;
    int spmPulseCycleSize_;

    int spmCycleValue_;
    int spmCycleValueOn_;
    Byte spmState_;
};


#endif    // CT2425_H

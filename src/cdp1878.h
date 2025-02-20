#ifndef CDP1878_H
#define CDP1878_H

#define MODE_NONE 0
#define MODE_TIMEOUT 1
#define MODE_STROBE 2
#define MODE_ONESHOT 3
#define MODE_RATE 4
#define MODE_PWM_1 5
#define MODE_UNDEFINED 6
#define MODE_PWM_2 7

class Cdp1878Instance
{
public:
    Cdp1878Instance(int cdp1878Number);
    ~Cdp1878Instance() {};
    
    void configureCdp1878(Cdp1878Configuration cdp1878Configuration);
    bool ioGroupCdp1878(int ioGroup);
    
    Byte efInterrupt();
    void writeCounterHigh(int counter, Byte value);
    void writeCounterLow(int counter, Byte value);
    Byte readCounterHigh(int counter);
    Byte readCounterLow(int counter);
    void writeControl(int counter, Byte value);
    Byte readInterrupt();
    Byte ef();
    void timeOut(int counter);
    void interrupt();
    
private:
    Cdp1878Configuration cdp1878Configuration_;
    int cdp1878Number_;

    Byte control_[2];
    Word holdingRegister_[2];
    Word counterRegister_[2];
    Word jamRegister_[2];
    
    Byte mode_[2];
    bool positiveGateLevel_[2];
    bool interruptEnabled_[2];
    bool startCounter_[2];
    bool freezeHoldingRegister_[2];
    bool jamEnabled_[2];

    bool strobe_[2];
    bool pwmPhaseLsb_[2];
    Byte counterRegisterLsb_[2];
    Byte counterRegisterMsb_[2];

    Byte interruptStatusRegister_;
    Byte interruptEf_;

    int cycleCounter_;
};

#endif  // CDP1878_H

#ifndef CDP1878_H
#define CDP1878_H

class Cdp1878Instance
{
public:
    Cdp1878Instance(int cdp1878Number);
    ~Cdp1878Instance() {};
    
    void configureCdp1878(Cdp1878Configuration cdp1878Configuration);
    bool ioGroupCdp1878(int ioGroup);
    
    Byte efInterrupt();
    void writeCounterHighA(Byte value);
    void writeCounterLowA(Byte value);
    void writeCounterHighB(Byte value);
    void writeCounterLowB(Byte value);
    Byte readCounterHighA();
    Byte readCounterLowA();
    Byte readCounterHighB();
    Byte readCounterLowB();
    void writeControlA(Byte value);
    void writeControlB(Byte value);
    Byte readInterrupt();
    Byte ef();
    void cycle();
    
private:
    Cdp1878Configuration cdp1878Configuration_;
    int cdp1878Number_;

    Byte controlA_;
    Word holdingRegisterA_;
    Word jamRegisterA_;
    
    Byte modeA_;
    bool positiveGateLevelA_;
    bool interruptEnabledA_;
    bool startCounterA_;
    bool freezeHoldingRegisterA_;
    bool jamEnabledA_;

    Byte controlB_;
    Word holdingRegisterB_;
    Word jamRegisterB_;
    
    Byte modeB_;
    bool positiveGateLevelB_;
    bool interruptEnabledB_;
    bool startCounterB_;
    bool freezeHoldingRegisterB_;
    bool jamEnabledB_;

    Byte InterruptStatusRegister_;
    Byte interruptEf_;

    int cycleCounter_;
};

#endif  // CDP1878_H

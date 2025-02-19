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
    void writeCounterHigh(int counter, Byte value);
    void writeCounterLow(int counter, Byte value);
    Byte readCounterHigh(int counter);
    Byte readCounterLow(int counter);
    void writeControl(int counter, Byte value);
    Byte readInterrupt();
    Byte ef();
    void cycle();
    
private:
    Cdp1878Configuration cdp1878Configuration_;
    int cdp1878Number_;

    Byte control[2]_;
    Word holdingRegister[2]_;
    Word counterRegister[2]_;
    Word jamRegister[2]_;
    
    Byte mode[2]_;
    bool positiveGateLevel[2]_;
    bool interruptEnabled[2]_;
    bool startCounter[2]_;
    bool freezeHoldingRegister[2]_;
    bool jamEnabled[2]_;

    Byte InterruptStatusRegister_;
    Byte interruptEf_;

    int cycleCounter_;
};

#endif  // CDP1878_H

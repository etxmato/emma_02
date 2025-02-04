#ifndef CDP1854_H
#define CDP1854_H

class Cdp1854Instance
{
public:
    Cdp1854Instance(int cdp1854Number);
    ~Cdp1854Instance() {};
    
    void configureCdp1854(Cdp1854Configuration cdp1854Configuration, double clock);
    bool ioGroupCdp1854(int ioGroup);
    
    void uartCts(Byte value);
    void writeControlRegister(Byte value);
    Byte readStatusRegister();
    void writeTransmitterHoldingRegister(Byte value);
    Byte readReceiverHoldingRegister_();
    Byte efSerialDataInput();
    Byte efInterrupt();
    void cycle();
    
    Byte threStatus();
    void interrupt();
    void clearInterrupt();
    
    void serialDataInput();
    void writeTransmitterShiftRegister_();
    Byte uartThreStatus();
    void framingError(bool data);
    void dataAvailable();
    void dataAvailable(Byte value);
    void dataAvailableUart(bool data);
    
private:
    int cdp1854Number_;
    
    Cdp1854Configuration cdp1854Configuration_;
    
    bitset<8> controlRegister_;
    bitset<8> statusRegister_;
    Byte serialDataInputEf_;
    bool useSdi_;
    Byte interruptEf_;

    Byte transmitterHoldingRegister;
    bool clearToSend_;
    
    int baudRateT_;
    int baudRateR_;

    long serialDataOutputCount_;
    int vtOutBits_;
    long serialDataInputCount_;
    Byte receiverHoldingRegister_;
};

#endif  // CDP1854_H

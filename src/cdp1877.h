#ifndef CDP1877_H
#define CDP1877_H

class Cdp1877Instance
{
public:
    Cdp1877Instance(int cdp1877Number);
    ~Cdp1877Instance() {};
  
    void configureCdp1877(Cdp1877Configuration cdp1877Configuration);
    bool ioGroupCdp1877(int ioGroup);

    void requestInterrupt(int type, bool state, int picNumber);
    int findInterrupt();
    void devWrite(Word address, Byte value);
    void writeMask(Byte value);
    void writePage(Byte value);
    void writeControl(Byte value);
    int devRead(Word address, int function);
    bool readMien() {return mien_;};
    Byte readStatus();
    Byte readPolling();
    Byte readVector(Word address, int function);

private:
    int cdp1877Number_;

    Cdp1877Configuration cdp1877Configuration_;
    
    bitset<8> mask_;
    Byte page_;
    Byte interval_;
    Byte vectorAddressLow_;
    bitset<8> status_;
    
    int vectorByteCounter_;
    int interruptType[8];
    
    bool mien_;
};


#endif  // CDP1877_H

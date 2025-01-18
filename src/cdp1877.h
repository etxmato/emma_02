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


#endif  // CDP1855_H

#ifndef CDP1855_H
#define CDP1855_H

class Cdp1855Instance
{
public:
    Cdp1855Instance(int cdp1855Number);
    ~Cdp1855Instance() {};
  
    void configureCdp1855(Cdp1855Configuration cdp1855Configuration);
    bool ioGroupCdp1855(int ioGroup);

    void writeX(Byte value);
    void writeY(Byte value);
    void writeZ(Byte value);
    void writeStatus(Byte value);
    Byte readX();
    Byte readY();
    Byte readZ();
    Byte readControl();
    
    void multiply();
    void divide();

private:
    int cdp1855Number_;

    Cdp1855Configuration cdp1855Configuration_;
    
    Byte x_;
    Byte y_;
    Byte z_;
    Byte control_;
    Byte status_;
};


#endif  // CDP1855_H

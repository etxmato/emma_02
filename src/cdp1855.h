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
    void writeControl(Byte value);
    Byte readX();
    Byte readY();
    Byte readZ();
    Byte readStatus();
    void cycle();
    int getSequenceCounter();
    void multiply();
    void divide();
    int getRegValue(Byte reg[]);
    int setRegValue(Byte reg[], int regValue);

private:
    int cdp1855Number_;

    Cdp1855Configuration cdp1855Configuration_;
    
    Byte x[4];
    Byte y[4];
    Byte z[4];
    Byte control_;
    Byte status_;
    Byte command_;
    int sequenceCounter_;
    int numberOfMdu_;
    int cycleCounter_;
};


#endif  // CDP1855_H

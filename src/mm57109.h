#ifndef MM57109_H
#define MM57109_H

struct NcuRegister
{
    unsigned int mantissaDigit[8] : 4;
    unsigned int exponentDigit[2] : 4;
    unsigned int decimalPoint : 4;
    unsigned int mantissaSign : 4;
    unsigned int exponentSign : 4;
};

class Mm57109Instance
{
public:
    Mm57109Instance();
    ~Mm57109Instance() {};
    
    void configureMm57109(Mm57109Configuration mm57109Configuration);
    bool ioGroupMm57109(int ioGroup);
    
    void write(Byte value);
    Byte read();
    Byte ef();
    void cycle();
    
private:
    Mm57109Configuration mm57109Configuration_;
    
    int cycleCounter_;
    
    Register registerX;
    Register registerY;
    Register registerZ;
    Register registerT;
    Register registerM;

    Byte rdy_;
};


#endif  // MM57109_H

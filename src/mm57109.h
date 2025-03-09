#ifndef MM57109_H
#define MM57109_H

struct RegisterX
{
    unsigned int nibbleX : 4;
    unsigned int nibbleY : 4;
    unsigned int nibbleZ : 4;
    unsigned int nibbleT : 4;
    unsigned int nibbleM0 : 4;
    unsigned int nibbleM1 : 4;
    unsigned int nibbleM2 : 4;
    unsigned int nibbleM3 : 4;
    unsigned int nibbleM4 : 4;
    unsigned int nibbleM5 : 4;
    unsigned int nibbleM6 : 4;
    unsigned int nibbleM7 : 4;
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
    void cycle();
    
private:
    Mm57109Configuration mm57109Configuration_;
    
    int cycleCounter_;
    
    RegisterX registerX;
};


#endif  // MM57109_H

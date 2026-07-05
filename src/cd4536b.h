#ifndef CD4536B_H
#define CD4536B_H

#include "computerconfig.h"

class Cd4536b
{
public:
    Cd4536b();
    ~Cd4536b();

    void Configure(Cd4536bConfiguration cd4536bIo, int number);
    void cycle();
    Byte ef();
    void writeControl(Byte value);

protected:
    Byte efState_;

    int counterCycleValue_;
    int counterCycleSize_;
    bool counterPaused_;
    bool counterSquare_;
};


#endif    // CD4536B_H

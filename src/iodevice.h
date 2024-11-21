
#ifndef IODEVICE_H
#define IODEVICE_H

#include "computerconfig.h"

class IoDevice 
{
public:
    IoDevice();
    ~IoDevice() {};

    void initIo();
    void setCycleType(int number, int outCycleType);

    void setEfType(int number, int efType, int itemNumber = 0);
    void setEfType(vector<int>* ioGroup, int number, int efType, bool excludeIoGroup, int itemNumber = 0);
    void setEfType(vector<int>* ioGroup, EfFlag efFlag, int efType, wxString message, int itemNumber = 0);
    void setEfType(vector<int>* ioGroup, EfFlag efFlag, wxString message, int itemNumber = 0);

    void setInType(int number, int inType, int itemNumber = 0);
    void setInType(vector<int>* ioGroup, int number, int inType, int itemNumber = 0);
    void setInType(int q, vector<int>* ioGroup, int number, int inType, int itemNumber = 0);
    void setInType(vector<int>* ioGroup, IoPort port, int inType, wxString message, int itemNumber = 0);
    void setInType(vector<int>* ioGroup, IoPort port, wxString message, int itemNumber = 0);

    void setOutType(int number, int outType, int itemNumber = 0);
    void setOutType(vector<int>* ioGroup, int number, int outType, int itemNumber = 0);
    void setOutType(int q, vector<int>* ioGroup, int number, int outType, int itemNumber = 0);
    void setOutType(vector<int>* ioGroup, IoPort port, int outType, wxString message, int itemNumber = 0);
    void setOutType(vector<int>* ioGroup, IoPort port, wxString message, int itemNumber = 0);

protected:
    int efType_[2][257][5];
    int efItemNumber_[2][257][5];
    int inType_[2][257][8];
    int inItemNumber_[2][257][8];
    int outType_[2][257][8];
    int outItemNumber_[2][257][8];
    
    int cycleType_[CYCLE_TYPE_MAX];

private:

};

#endif  // IODEVICE_H

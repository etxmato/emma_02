
#ifndef IODEVICE_H
#define IODEVICE_H

#include "computerconfig.h"

class OutputConfiguration
{
public:
    int type[2][257][8];
    int itemNumber[2][257][8];
};

class InputConfiguration
{
public:
    int type[2][257][8];
    int itemNumber[2][257][8];
};

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
    vector<OutputConfiguration> outputConfiguration;
    vector<InputConfiguration> inputConfiguration;

    int efType_[2][257][5];
    int efItemNumber_[2][257][5];
    
    int cycleType_[CYCLE_TYPE_MAX];

private:
    size_t getInputConfiguration(int q, int group, int number);
    size_t getOutputConfiguration(int q, int group, int number);
};

#endif  // IODEVICE_H

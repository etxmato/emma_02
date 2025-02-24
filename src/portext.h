#define PAGEROUT 1

#ifndef PORTEXT_H
#define PORTEXT_H

class PortExt 
{
public:
    PortExt();
    ~PortExt() {};

    void configurePortExt(MemoryMapperConfiguration memoryMapperConfiguration);
    void definePortExtForPager();
    void definePortExtForPager(Byte startPort, Byte endPort);
    Byte inPortExtender();
    void outPortExtender(Byte value);

    void selectPortExtender(Byte value);

private:
    int extPortsIn_[256];
    int extPortsOut_[256];

    int extPort_;
};

#endif  // PORTEXT_H

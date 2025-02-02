#ifndef CDP1854_H
#define CDP1854_H

class Cdp1854Instance
{
public:
    Cdp1854Instance(int cdp1854Number);
    ~Cdp1854Instance() {};
  
    void configureCdp1854(Cdp1854Configuration cdp1854Configuration);
    bool ioGroupCdp1854(int ioGroup);

    void cts(Byte value);
    void out(Byte value);
    void control(Byte value);
    Byte in(); 
    Byte status();
    Byte threStatus();
    void interrupt();
    void clearInterrupt();

private:
    int cdp1854Number_;

    Cdp1854Configuration cdp1854Configuration_;
    
};


#endif  // CDP1854_H
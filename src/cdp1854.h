#ifndef CDP1854_H
#define CDP1854_H

class Cdp1854Instance
{
public:
    Cdp1854Instance(int cdp1854Number);
    ~Cdp1854Instance() {};
  
    void configureCdp1854(Cdp1854Configuration cdp1854Configuration);
    bool ioGroupCdp1854(int ioGroup);

private:
    int cdp1854Number_;

    Cdp1854Configuration cdp1854Configuration_;
    
};
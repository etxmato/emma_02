#ifndef DIP_H
#define DIP_H

class DipInstance
{
public:
    DipInstance(int dipNumber);
    ~DipInstance() {};
  
    void configureDip(DipConfiguration dipConfiguration);
    bool ioGroupDip(int ioGroup);
    
    Byte getValue();
    bool isAddressValid(Word address);
    bool isAddressMode();

private:
    int dipNumber_;

    DipConfiguration dipConfiguration_;
    
};


#endif  // DIP_H

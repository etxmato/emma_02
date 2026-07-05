#ifndef PS2GPIO_H
#define PS2GPIO_H

class Ps2gpio 
{
public:
    Ps2gpio ();
    ~Ps2gpio () {};

    void configurePs2gpio(bool forceUpperCase, GpioPs2KeyboardConfiguration gpioPs2KeyboardConfiguration);
    void charEventPs2gpio(int keycode);
    void addKeyToBuffer(int value, int keycode);

    Byte efPs2gpio();
    Byte inPs2gpio();
    void cyclePs2gpio();

    void startPs2gpioKeyFile();
    void closePs2gpioKeyFile();
    void setForceUpperCasePs2gpio(bool status);
    void resetPs2gpio();
    void writeGpioControlRegister(Byte value);

private:
    GpioPs2KeyboardConfiguration gpioPs2KeyboardConfiguration_;

    Byte ps2Buffer_[256];
    int ps2KeyStart_;
    int ps2KeyEnd_;

    int keyboardValue_;
    Byte keyboardEf_;
    int keyCycles_;
    Byte lastKeyCode_;

    wxFile elfKeyFile_;
    bool elfKeyFileOpen_;
    bool forceUpperCase_;
    bool startUp_;
};

#endif  // PS2GPIO_H

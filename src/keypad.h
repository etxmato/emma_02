#ifndef KEYPAD_H
#define KEYPAD_H

#include "led.h"

class EtiKeypad
{
public:
    EtiKeypad ();
    ~EtiKeypad () {};

    void configure(EtiKeypadConfiguration etiKeypadConfiguration, int keyDefA1[], int keyDefA2[]);
    
    void keyDown(int keycode, wxKeyEvent& event);
    void keyUp(int keycode, wxKeyEvent& event);
    void onInButtonPress();
    void onInButtonRelease();

    Byte ef();
    Byte in(Word address);
    bool out(Word address, Byte value);
    void keyClear();

private:
    EtiKeypadConfiguration etiKeypadConfiguration_;
    
    int keyDefA1_[16];
    int keyDefA2_[16];

    bool eti660KeyState_[16];
    bool DataDirection_;
    Byte outputKeyLatch_;
    Byte inputKeyLatch_;
    Byte outputKeyValue_;

    int stepEf_;
};


#endif  // KEYPAD_H

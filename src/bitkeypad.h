#ifndef BITKEYPAD_H
#define BITKEYPAD_H

class BitKeypad
{
public:
    BitKeypad ();
    ~BitKeypad () {};

    void configure(BitKeypadConfiguration bitKeypadConfiguration);
    
    void keyDown(int keycode);
    bool keyDownCtrlV(int keycode);
    void keyUp(int keycode);
    void keyUpCtrlV(int keycode);
    void keysUp();

    Byte in();
    void cid1Bit8(bool set);
    
private:
    BitKeypadConfiguration bitKeypadConfiguration_;

    Byte inputValue_;
    int lastKey_;
};

#endif  // BITKEYPAD_H

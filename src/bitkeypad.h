#ifndef BITKEYPAD_H
#define BITKEYPAD_H

class BitKeypad
{
public:
    BitKeypad ();
    ~BitKeypad () {};

    void configure(IoConfiguration portConf, int pad);
    
    void keyDown(int keycode);
    bool keyDownCtrlV(int keycode);
    void keyUp(int keycode);
    void keyUpCtrlV(int keycode);
    void keysUp();

    Byte in();
    void cid1Bit8(bool set);
    
private:
    IoConfiguration ioConfiguration_;

    Byte inputValue_;
    int lastKey_;
    int pad_;
};

#endif  // BITKEYPAD_H

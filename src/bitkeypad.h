#ifndef BITKEYPAD_H
#define BITKEYPAD_H

class BitKeypad
{
public:
    BitKeypad ();
    ~BitKeypad () {};

    void configure(IoConfiguration portConf, int pad);
    
    void keyDown(int keycode);
    void keyUp(int keycode);

    Byte in();
    void cid1Bit8(bool set);

private:
    IoConfiguration ioConfiguration_;

    Byte inputValue_;
    int pad_;
};

#endif  // BITKEYPAD_H

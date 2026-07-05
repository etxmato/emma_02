#ifndef KEYPADFRED_H
#define KEYPADFRED_H

class KeypadFred
{
public:
    KeypadFred ();
    ~KeypadFred () {};

    void configure(FredKeypadConfiguration fredKeypadConfiguration, int keyDefA1[], int keyDefA2[]);
    
    void keyDown(int keycode);
    void keyUp(int keycode);
    void keyFound(int key);
    void cycle();
    void setInputMode(int inpMode);

    Byte efHexFireA();
    Byte efFireB();
    Byte efCoin();
    Byte inHex();
    Byte inCoin();
    void setKeyPress(Byte value);

private:
    FredKeypadConfiguration fredKeypadConfiguration_;
    
    wxKeyCode keyCode_;
    int keyCycles_;
    char nextNybble_;
    Byte shiftPressed_;

    Byte efState_;
    Byte keyValue_;

    int keyDefA1_[16];
    int keyDefA2_[16];
    
    int inpMode_;

    Byte directionKey_;
    Byte fireKeyA_;
    Byte fireKeyB_;
    Byte coinKey_;

    int keyDefA_[5];
    int keyDefB_[5];
    int keyDefCoin_;
};

#endif  // KEYPADFRED_H

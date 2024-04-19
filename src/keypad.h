#ifndef KEYPAD_H
#define KEYPAD_H

#include "led.h"

class KeypadScreen: public Panel
{
public:
    KeypadScreen(wxWindow *parent, const wxSize& size, int tilType);
    ~KeypadScreen();

    void init(int computerType);
    void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
    void releaseButtonOnScreen(HexButton* buttonPointer);

private:
};

class Keypad : public wxFrame
{
public:
    Keypad(const wxString& title, const wxPoint& pos, const wxSize& size, int computerType);
    ~Keypad();

    void onClose(wxCloseEvent&WXUNUSED(event));
    void onButtonRelease(wxCommandEvent& event);
    void onButtonPress(wxCommandEvent& event);
    void onNumberKeyDown(int i);
    void onNumberKeyDown(wxCommandEvent&event);
    void onNumberDown(int hex);
    void onNumberKeyUp(wxCommandEvent&event);
    void ledTimeout();
    void setLedMs(long ms);
    Byte getKey(Byte vtOut);
    void releaseButtonOnScreen(HexButton* buttonPointer);
    void refreshPanel();

private:
    class KeypadScreen *keypadScreenPointer;

    int computerType_;
    Byte keypadValue_;
    char nextNybble_;

    DECLARE_EVENT_TABLE()
};

class EtiKeypad
{
public:
    EtiKeypad ();
    ~EtiKeypad () {};

    void configure(IoConfiguration portConf, int keyDefA1[], int keyDefA2[]);
    
    void keyDown(int keycode, wxKeyEvent& event);
    void keyUp(int keycode, wxKeyEvent& event);
    void onInButtonPress();
    void onInButtonRelease();

    Byte ef();
    Byte in(Word address);
    bool out(Word address, Byte value);
    void keyClear();

private:
    IoConfiguration ioConfiguration_;

    int keyDefA1_[16];
    int keyDefA2_[16];

    bool eti660KeyState_[16];
    bool DataDirection_;
    Byte outputKeyLatch_;
    Byte inputKeyLatch_;
    Byte outputKeyValue_;

    bool step_;
};


#endif  // KEYPAD_H

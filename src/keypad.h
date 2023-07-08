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

#endif  // KEYPAD_H

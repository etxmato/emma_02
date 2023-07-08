#ifndef COSMICOSHEX_H
#define COSMICOSHEX_H

#include "led.h"
#include "cdp1802.h"

class CosmicoshexScreen : public Panel
{
public:
    CosmicoshexScreen(wxWindow *parent, const wxSize& size, int tilType);
    ~CosmicoshexScreen();

    void init();
    void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
    void releaseButtonOnScreen(HexButton* buttonPointer);

    Byte reqState();
    Byte seqState();
    Byte decState();
    Byte retState();

private:
    Byte reqState_;
    Byte seqState_;
    Byte decState_;
    Byte retState_;
};

class Cosmicoshex : public wxFrame
{
public:
    Cosmicoshex(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~Cosmicoshex();

    void onClose(wxCloseEvent&event);
    void onNumberKeyPressEvent(wxCommandEvent&event);
    void onNumberKeyReleaseEvent(wxCommandEvent&event);
    void onNumberKeyPress(int key);
    void onNumberKeyRelease(int key);
    void keyLatch(int value);
    void segUpdate(int number, int value);
    void segOff();
    Byte getKeyinput();

    void onReqButtonPress(wxCommandEvent& event);
    void onSeqButtonPress(wxCommandEvent& event);
    void onDecButtonPress(wxCommandEvent& event);
    void onRetButtonPress(wxCommandEvent& event);
    void onReqButtonRelease(wxCommandEvent& event);
    void onSeqButtonRelease(wxCommandEvent& event);
    void onDecButtonRelease(wxCommandEvent& event);
    void onRetButtonRelease(wxCommandEvent& event);

    Byte reqState();
    Byte seqState(); 
    Byte decState(); 
    Byte retState(); 

    void ledTimeout();
    void setLedMs(long ms);
    Byte getKey(Byte vtOut);
    void releaseButtonOnScreen(HexButton* buttonPointer);
    void refreshPanel();

private:
    class CosmicoshexScreen *cosmicoshexScreenPointer;

    Byte reqState_;
    Byte seqState_;
    Byte decState_;
    Byte retState_;

    Byte keypadValue_[5];
    int keyLatch_;

    DECLARE_EVENT_TABLE()
};

#endif  // COSMICOSHEX_H

#ifndef VIPIISTATUSBAR_H
#define VIPIISTATUSBAR_H

#define LED_SIZE_X 12
#define LED_SIZE_Y 7

class VipIIStatusBar : public wxStatusBar
{
public:
    VipIIStatusBar(wxWindow *parent);
    ~VipIIStatusBar();

    void initVipIIBar(bool runLed);
    void updateLedStatus(int card, bool status);
    void reDrawBar();

private:
    void displayText();
    void displayLeds();
    void deleteBitmaps();
    void updateStatusBarText();

#if defined(__linux__) || defined(__WXMAC__)
    wxBitmapButton *ledBitmapPointers [4];
#else
    wxButton *ledBitmapPointers [4];
#endif
    bool ledStatus [4];

    wxBitmap *ledOffPointer;
    wxBitmap *ledOnPointer;
    wxMask *maskOn;
    wxMask *maskOff;

    int led_pos_y_;
    int statusBarElementMeasure0_;
    int statusBarElementMeasure1_;
    int ledSpacing_;
    wxString leaderString_;


    bool ledsDefined_;
};

#endif  //_vipIIstatusbar_H_

#ifndef STATUSBAR_H
#define STATUSBAR_H

#define LED_SIZE_X 12
#define LED_SIZE_Y 7

class EmmaStatusBar : public wxStatusBar
{
public:
    EmmaStatusBar(wxWindow *parent);
    ~EmmaStatusBar();

    virtual void init(bool WXUNUSED(expansionConfiguration_defined)) {};
    virtual void configure(int WXUNUSED(ledOut)) {};
    virtual void updateLedStatus(bool WXUNUSED(status), int WXUNUSED(card), int WXUNUSED(i = 0)) {};
    virtual void reDrawBar() {};
    virtual void deleteBitmaps() {};

protected:
    wxBitmap *ledOffPointer;
    wxBitmap *ledOnPointer;
};

class ComxStatusBar : public EmmaStatusBar
{
public:
    ComxStatusBar(wxWindow *parent);
    ~ComxStatusBar();

    void init(bool expansionConfiguration_defined);
    void updateLedStatus(bool status, int card, int i);
    void reDrawBar();
    void deleteBitmaps();

private:
    void onStatusLed(wxCommandEvent& event);
    void onSlotLed(wxCommandEvent& event);
    void displayText();
    void displayLeds();
    void updateStatusBarText();
    wxString getStatusBarTextXml(int slot, wxRect rect);

#if defined(__linux__) || defined(__WXMAC__)
    wxBitmapButton *ledBitmapPointers [4][2];
#else
    wxButton *ledBitmapPointers [4][2];
#endif

    wxBitmap *ledDisabledPointer;

    bool expansionConfiguration_defined_;
    bool ledsDefined_;
    bool statusLedUpdate_;
    bool slotLedUpdate_;
    bool ledStatus_[4];
    
    int led_pos_y_;
    int ledPosX1_;
    int ledPosX2_;
    int lastLedPosX_;
    
    int offsetField_;
    int ledSpacing_;

    int ledOffsetX_;
    
    int statusBarElementMeasure[5];
    wxString leaderString_;

    DECLARE_EVENT_TABLE()
};

#define CIDELSA_LED_SIZE_X 12
#define CIDELSA_LED_SIZE_Y 7

class CidelsaStatusBar : public EmmaStatusBar
{
public:
    CidelsaStatusBar(wxWindow *parent);
    ~CidelsaStatusBar();

    void init(bool expansionConfiguration_defined);
    void configure(int ledOut);
    void updateLedStatus(bool status, int number, int i = 0);
    void reDrawBar();
    void deleteBitmaps();

private:
    void onButtonPress1(wxCommandEvent& event);
    void onButtonRelease1(wxCommandEvent& event);
    void onButtonPress2(wxCommandEvent& event);
    void onButtonRelease2(wxCommandEvent& event);
    void onButtonPressA(wxCommandEvent& event);
    void onButtonReleaseA(wxCommandEvent& event);
    void onButtonPressB(wxCommandEvent& event);
    void onButtonReleaseB(wxCommandEvent& event);
    void onButtonPressFire(wxCommandEvent& event);
    void onButtonReleaseFire(wxCommandEvent& event);
    void displayText();
    void displayLeds();

#if defined(__linux__) || defined(__WXMAC__)
    wxBitmapButton *ledBitmapPointers [5];
#else
    wxButton *ledBitmapPointers [5];
#endif

    bool status_[5];

    bool ledsDefined_;
    
    int led_pos_y_;
    int statusBarElementMeasure0_;
    int statusBarElementMeasure1_;
    int ledSpacing_;
    wxString leaderString_;

    DECLARE_EVENT_TABLE()
};

#define DIAG_LED_SIZE_X 12
#define DIAG_LED_SIZE_Y 12

#define NUMBER_OF_DIAG_LEDS 6

class DiagStatusBar : public EmmaStatusBar
{
public:
    DiagStatusBar(wxWindow *parent);
    ~DiagStatusBar();

    void init(bool expansionConfiguration_defined);
    void updateLedStatus(bool status, int led, int i = 0);
    void reDrawBar();
    void deleteBitmaps();

private:
    void displayText();
    void displayLeds();

#if defined(__linux__) || defined(__WXMAC__)
    wxBitmapButton *diagLedPointer [NUMBER_OF_DIAG_LEDS];
#else
    wxButton *diagLedPointer [NUMBER_OF_DIAG_LEDS];
#endif
    
    wxMask *maskOff;
    wxMask *maskOn;
    
    int led_pos_y_;
    int statusBarElementMeasure0_;
    int statusBarElementMeasure1_;
    wxString leaderString_;

    bool ledsDefined_;
    bool ledStatus_[NUMBER_OF_DIAG_LEDS];

};

class VipIIStatusBar : public wxStatusBar
{
public:
    VipIIStatusBar(wxWindow *parent);
    ~VipIIStatusBar();

    void initVipIIBar(bool runLed);
    void updateLedStatus(bool status, int card, int i = 0);
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

#endif  // STATUSBAR_H

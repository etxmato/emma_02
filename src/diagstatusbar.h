#ifndef DIAGSTATUSBAR_H
#define DIAGSTATUSBAR_H

#define DIAG_LED_SIZE_X 12
#define DIAG_LED_SIZE_Y 12

#define NUMBER_OF_DIAG_LEDS 6

class DiagStatusBar : public wxStatusBar
{
public:
	DiagStatusBar(wxWindow *parent);
	~DiagStatusBar();

	void initDiagBar();
	void updateLedStatus(int led, bool status);
	void reDrawBar();

private:
	void displayText();
	void displayLeds();
	void deleteBitmaps();

#if defined(__linux__) || defined(__WXMAC__)
    wxBitmapButton *ledPointer [NUMBER_OF_DIAG_LEDS];
#else
    wxButton *ledPointer [NUMBER_OF_DIAG_LEDS];
#endif
    
	wxBitmap *ledOffPointer;
	wxBitmap *ledOnPointer;

    wxMask *maskOff;
    wxMask *maskOn;
    
    int linux_led_pos_y_;
    int statusBarElementMeasure0_;
    int statusBarElementMeasure1_;
    wxString leaderString_;

	bool ledsDefined_;
	bool ledStatus_[NUMBER_OF_DIAG_LEDS];

};

#endif  //_diagstatusbar_H_

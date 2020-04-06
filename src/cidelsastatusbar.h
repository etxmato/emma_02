#ifndef CIDELSASTATUSBAR_H
#define CIDELSASTATUSBAR_H

#define CIDELSA_LED_SIZE_X 12
#define CIDELSA_LED_SIZE_Y 7

class CidelsaStatusBar : public wxStatusBar
{
public:
	CidelsaStatusBar(wxWindow *parent);
	~CidelsaStatusBar();

	void initCidelsaBar();
	void updateLedStatus(int number, bool status);
	void reDrawBar();
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

private:
	void displayText();
	void displayLeds();
	void deleteBitmaps();

#if defined(__linux__) || defined(__WXMAC__)
	wxBitmapButton *ledBitmapPointers [5];
#else
	wxButton *ledBitmapPointers [5];
#endif

	wxBitmap *ledOffPointer;
	wxBitmap *ledOnPointer;
	bool status_[5];

	bool ledsDefined_;
    
    int linux_led_pos_y_;
    int statusBarElementMeasure0_;
    int statusBarElementMeasure1_;
    int ledSpacing_;
    wxString leaderString_;

    DECLARE_EVENT_TABLE()
};

#endif  //_cidelsastatusbar_H_

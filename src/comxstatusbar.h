#ifndef COMXSTATUSBAR_H
#define COMXSTATUSBAR_H

#define LED_SIZE_X 12
#define LED_SIZE_Y 7

class ComxStatusBar : public wxStatusBar
{
public:
	ComxStatusBar(wxWindow *parent);
	~ComxStatusBar();

	void initComxBar(bool expansionRomLoaded, int expansionTypeCard0);
	void updateLedStatus(int card, int i, bool status);
	void reDrawBar();
	void onStatusLed(wxCommandEvent& event);
	void onSlotLed(wxCommandEvent& event);

private:
	void displayText();
	void displayLeds();
	void deleteBitmaps();
	void updateStatusBarText();

#if defined(__linux__) || defined(__WXMAC__)
	wxBitmapButton *ledBitmapPointers [4][2];
#else
	wxButton *ledBitmapPointers [4][2];
#endif

	wxBitmap *ledOffPointer;
	wxBitmap *ledOnPointer;
	wxBitmap *ledDisabledPointer;

	bool expansionRomLoaded_;
	int expansionTypeCard0_;
	bool ledsDefined_;
	bool statusLedUpdate_;
	bool slotLedUpdate_;
	bool ledStatus_[4];
    
    int linux_led_pos_y_;
    int statusBarElementMeasure[4];
    wxString leaderString_;

    DECLARE_EVENT_TABLE()
};

#endif  //_comxstatusbar_H_

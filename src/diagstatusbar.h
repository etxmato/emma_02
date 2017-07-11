#ifndef DIAGSTATUSBAR_H
#define DIAGSTATUSBAR_H

#define DIAG_LED_SIZE_X 12
#define DIAG_LED_SIZE_Y 12

class DiagStatusBar : public wxStatusBar
{
public:
	DiagStatusBar(wxWindow *parent);
	~DiagStatusBar();

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

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
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

    DECLARE_EVENT_TABLE()
};

#endif  //_diagstatusbar_H_

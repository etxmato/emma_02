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

#endif  //_comxstatusbar_H_

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
	void updateLedStatus(int led, bool status);
	void reDrawBar();

private:
	void displayText();
	void displayLeds();
	void deleteBitmaps();

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
    wxBitmapButton *ledPointer [5];
#else
    wxButton *ledPointer [5];
#endif
    
	wxBitmap *ledOffPointer;
	wxBitmap *ledOnPointer;

    wxMask *maskLedOff;
    wxMask *maskLedOn;
    
	bool ledsDefined_;
	bool ledStatus_[5];

};

#endif  //_diagstatusbar_H_

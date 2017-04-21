#ifndef VIPIISTATUSBAR_H
#define VIPIISTATUSBAR_H

#define LED_SIZE_X 12
#define LED_SIZE_Y 7

class VipIIStatusBar : public wxStatusBar
{
public:
	VipIIStatusBar(wxWindow *parent);
	~VipIIStatusBar();

	void initVipIIBar();
	void updateLedStatus(int card, bool status);
	void reDrawBar();

private:
	void displayText();
	void displayLeds();
	void deleteBitmaps();
	void updateStatusBarText();

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
	wxBitmapButton *ledBitmapPointers [4];
#else
	wxButton *ledBitmapPointers [4];
#endif
	bool ledStatus [4];

	wxBitmap *ledOffPointer;
	wxBitmap *ledOnPointer;
	wxMask *maskOn;
	wxMask *maskOff;

	bool ledsDefined_;
};

#endif  //_vipIIstatusbar_H_

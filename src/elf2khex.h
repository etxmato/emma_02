#ifndef ELF2KHEX_H
#define ELF2KHEX_H

#include "cdp1802.h"

class Elf2KHexScreen : public Panel
{
public:
	Elf2KHexScreen(wxWindow *parent, const wxSize& size);
	~Elf2KHexScreen();

	void init();
	void onPaint(wxPaintEvent&event);
	void onMousePress(wxMouseEvent& event);
	void onMouseRelease(wxMouseEvent& event);
	void releaseButtonOnScreen(HexButton* buttonPointer);

	void inUp();
	void inDown();
	void runUp();
	void runDown();
	void mpUp();
	void mpDown();
	void loadUp();
	void loadDown();

private:
#ifndef __WXMAC__
	wxBitmap *inUpBitmapPointer;
	wxBitmap *inDownBitmapPointer;
	wxBitmap *runUpBitmapPointer;
	wxBitmap *runDownBitmapPointer;
	wxBitmap *loadUpBitmapPointer;
	wxBitmap *loadDownBitmapPointer;
	wxBitmap *mpUpBitmapPointer;
	wxBitmap *mpDownBitmapPointer;
	wxBitmap *resetUpBitmapPointer;
	wxBitmap *resetDownBitmapPointer;
#endif
};

class Elf2Khex : public wxFrame
{
public:
	Elf2Khex(const wxString& title, const wxPoint& pos, const wxSize& size);
	~Elf2Khex();

	void onClose(wxCloseEvent&WXUNUSED(event));
	void onPaint(wxPaintEvent&event);
	void onNumberKey(wxCommandEvent&event);
	void onNumberKeyDown(int i);
	Byte getData();
	void setRunButtonUp(int up);
	void setInButtonUp(bool up);
	void setLoadButtonUp(int up);
	void setMpButtonUp(int up);

	void onRunButton(wxCommandEvent& event);
	void onMpButton(wxCommandEvent& event);
	void onResetButton(wxCommandEvent& event);
	void onInButton(wxCommandEvent& event);
	void onElf2KInButton();
	void onLoadButton(wxCommandEvent& event);
	void onInButtonTimer(wxTimerEvent& event);
	Byte getKey(Byte vtOut);
	void releaseButtonOnScreen(HexButton* buttonPointer);

private:
	wxTimer *inButtonTimerPointer;
	class Elf2KHexScreen *elf2KHexScreenPointer;

	Byte keypadValue_;
	char nextNybble_;

	DECLARE_EVENT_TABLE()
};

#endif  // ELF2KHEX_H
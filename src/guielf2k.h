#ifndef GUIELF2K_H
#define GUIELF2K_H

#include "guims2000.h"
#include "elfconfiguration.h"

class GuiElf2K: public GuiMS2000
{
public:

	GuiElf2K(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir);
	~GuiElf2K() {};

	void readElf2KConfig();
    void writeElf2KDirConfig();
	void writeElf2KConfig();
    void readElf2KWindowConfig();
    void writeElf2KWindowConfig();

	void onElf2KVideoType(wxCommandEvent& event);
	void onElf2KKeyboard(wxCommandEvent& event);
	void onElf2KBaudR(wxCommandEvent& event);
	void onElf2KBaudT(wxCommandEvent& event);
	void onElf2KUart(wxCommandEvent& event);
	void onElf2KForceUpperCase(wxCommandEvent& event);
	void onElf2KControlWindows(wxCommandEvent& event);
	void onElf2KRtc(wxCommandEvent& event);
	void onElf2KNvr(wxCommandEvent& event);
	void onElf2KSwitch(wxCommandEvent& event);
	void onElf2KHex(wxCommandEvent& event);
	void onElf2KClock(wxCommandEvent& event);
	void onBootRam(wxCommandEvent& event);
	void onClearRtc(wxCommandEvent& event);

	void setElf2KKeyboard(int Selection);
	void setElf2KVideoType(int Selection);

	wxPoint getElf2KswitchPos();
	void setElf2KswitchPos(wxPoint position);

	void setBaudChoiceElf2K();

protected:
	int switchX_, switchY_;

private:
    wxPoint position_;

	DECLARE_EVENT_TABLE()
};

#endif // GUIELF2K_H

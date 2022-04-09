#ifndef GUIMEMBERSHIP_H
#define GUIMEMBERSHIP_H

#include "guistudio2.h"
#include "elfconfiguration.h"

class GuiMembership: public GuiStudio2
{
public:

	GuiMembership(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiMembership() {};

	void readMembershipConfig();
    void writeMembershipDirConfig();
    void writeMembershipConfig();
    void readMembershipWindowConfig();
    void writeMembershipWindowConfig();

	void onMembershipBaudR(wxCommandEvent& event);
	void onMembershipBaudT(wxCommandEvent& event);
	void onMembershipForceUpperCase(wxCommandEvent& event);
	void onMembershipControlWindows(wxCommandEvent& event);
	void onMembershipClock(wxCommandEvent& event);
	void onRam(wxCommandEvent& event);
    void onIo(wxCommandEvent& event);
    void onFront(wxCommandEvent& event);
	void onRomEvent(wxCommandEvent& event);
	void onRom();
	int getLoadromModeMembership();
	void onNvrMembership(wxCommandEvent&event);

protected:
	int loadromMode_;

private:

    DECLARE_EVENT_TABLE()
};

#endif // GUIMEMBERSHIP_H

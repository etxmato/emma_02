#ifndef GUINETRONICS_H
#define GUINETRONICS_H

#include "guipico.h"
#include "elfconfiguration.h"

class GuiNetronics: public GuiPico
{
public:

	GuiNetronics(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiNetronics() {};

    void readNetronicsConfig();
	void writeNetronicsDirConfig();
	void writeNetronicsConfig();
    void readNetronicsWindowConfig();
    void writeNetronicsWindowConfig();

	void onMemoryNetronics(wxCommandEvent& event);
	void onNetronicsControlWindows(wxCommandEvent& event);
	void onRom1(wxCommandEvent& event);
	void onRom2(wxCommandEvent& event);

    int getLoadromModeNetronics(int num);
	void setMemoryNetronics(wxString elfTypeStr, int elfType, int Selection);

private:

	int loadromMode_[2];

	wxBitmap tapeOffBitmap;
	wxBitmap tapeOnBitmap;

	DECLARE_EVENT_TABLE()
};

#endif // GUINETRONICS_H

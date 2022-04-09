#ifndef GUIPICO_H
#define GUIPICO_H

#include "guielf.h"
#include "elfconfiguration.h"

class GuiPico: public GuiElf
{
public:

	GuiPico(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiPico() {};

    void readPicoConfig();
	void writePicoDirConfig();
	void writePicoConfig();
    void readPicoWindowConfig();
    void writePicoWindowConfig();

	void onMemoryPico(wxCommandEvent& event);
	void onPicoControlWindows(wxCommandEvent& event);
	void onRom1(wxCommandEvent& event);
	void onRom2(wxCommandEvent& event);

    int getLoadromModePico(int num);
	void setMemoryPico(wxString elfTypeStr, int elfType, int Selection);

private:

	int loadromMode_[2];

	wxBitmap tapeOffBitmap;
	wxBitmap tapeOnBitmap;

	DECLARE_EVENT_TABLE()
};

#endif // GUIPICO_H

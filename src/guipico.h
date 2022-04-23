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
    void onVideoTypePico(wxCommandEvent& event);
    void setVideoTypePico(int Selection);
    void onDiskTypePico(wxCommandEvent&event);
    void setDiskTypePico(int Selection);
    void onPicoRtc(wxCommandEvent&event);
    void onPicoClearRtc(wxCommandEvent&event);

private:
	wxBitmap tapeOffBitmap;
	wxBitmap tapeOnBitmap;

	DECLARE_EVENT_TABLE()
};

#endif // GUIPICO_H

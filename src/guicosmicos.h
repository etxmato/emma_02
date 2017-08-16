#ifndef GUICOSMICOS_H
#define GUICOSMICOS_H

#include "guimicrotutor.h"
#include "elfconfiguration.h"

class GuiCosmicos: public GuiMicrotutor
{
public:

	GuiCosmicos(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiCosmicos() {};

	void readCosmicosConfig();
    void writeCosmicosDirConfig();
    void writeCosmicosConfig();
    void readCosmicosWindowConfig();
    void writeCosmicosWindowConfig();

	void onCosmicosVideoType(wxCommandEvent& event);
	void onCosmicosKeyboard(wxCommandEvent& event);
	void onCosmicosBaudT(wxCommandEvent& event);
	void onCosmicosForceUpperCase(wxCommandEvent& event);
	void onCosmicosControlWindows(wxCommandEvent& event);
	void onCosmicosHex(wxCommandEvent& event);
	void onCosmicosClock(wxCommandEvent& event);
	void onRam(wxSpinEvent& event);

	void setCosmicosKeyboard(int Selection);
	void setCosmicosVideoType(int Selection);

	void setBaudChoiceCosmicos();

protected:
	int switchX_, switchY_;

private:
    wxPoint position_;

	DECLARE_EVENT_TABLE()
};

#endif // GUICOSMICOS_H

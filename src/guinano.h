#ifndef GUINANO_H
#define GUINANO_H

#include "guicidelsa.h"

class GuiNano: public GuiCidelsa
{
public:

	GuiNano(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiNano() {};

	void readNanoConfig();
    void writeNanoDirConfig();
    void writeNanoConfig();
    void readNanoWindowConfig();
    void writeNanoWindowConfig();

	void onNanoSound(wxCommandEvent&event);

private:

	DECLARE_EVENT_TABLE()
};

#endif // GUINANO_H

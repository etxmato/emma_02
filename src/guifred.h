#ifndef GUIFRED_H
#define GUIFRED_H

#include "guivip.h"

class GuiFred: public GuiVip
{
public:

	GuiFred(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiFred() {};

    void readFredConfig();
    void writeFredDirConfig();
    void writeFredConfig();
    void readFredWindowConfig();
    void writeFredWindowConfig();
    void onFredControlWindows(wxCommandEvent&event);
    bool getUseControlWindows();

private:

	DECLARE_EVENT_TABLE()
};

#endif // GUIFRED_H

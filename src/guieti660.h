#ifndef GUIETI660_H
#define GUIETI660_H

#include "guinano.h"

class GuiEti: public GuiNano
{
public:

	GuiEti(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiEti() {};

	void readEtiConfig();
    void writeEtiDirConfig();
    void writeEtiConfig();
    void readEtiWindowConfig();
    void writeEtiWindowConfig();

private:

	DECLARE_EVENT_TABLE()
};

#endif // GUIETI660_H

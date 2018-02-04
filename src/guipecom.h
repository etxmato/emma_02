#ifndef GUIPECOM_H
#define GUIPECOM_H

#include "guimain.h"

class GuiPecom : public GuiMain
{
public:

	GuiPecom(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiPecom() {};

	void readPecomConfig();
    void writePecomDirConfig();
    void writePecomConfig();
    void readPecomWindowConfig();
    void writePecomWindowConfig();

private:

	DECLARE_EVENT_TABLE()
};

#endif // GUIPECOM_H

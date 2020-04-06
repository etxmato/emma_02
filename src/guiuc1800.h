#ifndef GUIUC1800_H
#define GUIUC1800_H

#include "guimembership.h"
#include "elfconfiguration.h"
 
class GuiUc1800: public GuiMembership
{
public:

	GuiUc1800(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiUc1800() {};

	void readUc1800Config();
    void writeUc1800DirConfig();
    void writeUc1800Config();
    void readUc1800WindowConfig();
    void writeUc1800WindowConfig();

	void onUc1800Clock(wxCommandEvent& event);
	void onUC1800HexOutput(wxSpinEvent&event);
	void onUC1800HexInput(wxSpinEvent&event);

protected:
	int switchX_, switchY_;

private:
    wxPoint position_;

	DECLARE_EVENT_TABLE()
};

#endif // GUIUC1800_H

#ifndef GUIMICROTUTOR_H
#define GUIMICROTUTOR_H

#include "guimembership.h"
#include "elfconfiguration.h"

class GuiMicrotutor: public GuiMembership
{
public:

	GuiMicrotutor(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiMicrotutor() {};

	void readMicrotutorConfig();
    void writeMicrotutorDirConfig();
    void writeMicrotutorConfig();
    void readMicrotutorWindowConfig();
    void writeMicrotutorWindowConfig();

	void onMicrotutorClock(wxCommandEvent& event);

protected:
	int switchX_, switchY_;

private:
    wxPoint position_;

	DECLARE_EVENT_TABLE()
};

#endif // GUIMICROTUTOR_H

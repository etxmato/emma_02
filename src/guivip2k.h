#ifndef GUIVIP2K_H
#define GUIVIP2K_H

#include "guivelf.h"

class GuiVip2K: public GuiVelf
{
public:

    GuiVip2K(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode_, wxString dataDir, wxString iniDir);
    ~GuiVip2K() {};

    void readVip2KConfig();
    void writeVip2KDirConfig();
    void writeVip2KConfig();
    void readVip2KWindowConfig();
    void writeVip2KWindowConfig();

    void onVipBaudR(wxCommandEvent& event);
    void onVipBaudT(wxCommandEvent& event);
    void onVipUart(wxCommandEvent& event);
    void onVtShow(wxCommandEvent&event);
    void onAutoKeyDef(wxCommandEvent&event);

private:

    DECLARE_EVENT_TABLE()
};

#endif // GUIVIP2K_H

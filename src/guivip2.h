#ifndef GUIVIPII_H
#define GUIVIPII_H

#include "xmlparser.h"

DECLARE_EVENT_TYPE(STATUS_BAR_PIXIE, 810)

class GuiVipII: public XmlParser
{
public:

    GuiVipII(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode_, wxString dataDir, wxString iniDir);
    ~GuiVipII() {};

    void pixieBarSize(wxCommandEvent &event);
    void pixieBarSizeEvent();

private:

    DECLARE_EVENT_TABLE()
};

#endif // GUIVIPII_H

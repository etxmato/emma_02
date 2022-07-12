#ifndef GUIDIY_H
#define GUIDIY_H

#include "guipico.h"
#include "elfconfiguration.h"

class GuiDiy: public GuiPico
{
public:

    GuiDiy(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiDiy() {};

    void readNetronicsConfig();
    void writeNetronicsDirConfig();
    void writeNetronicsConfig();
    void readNetronicsWindowConfig();
    void writeNetronicsWindowConfig();

    void onNetronicsControlWindows(wxCommandEvent& event);

    void onMainRamDiy(wxCommandEvent& event);
    void onMainRamTextDiy(wxCommandEvent& event);
    void onMainXmlDiy(wxCommandEvent& event);
    void onMainXmlTextDiy(wxCommandEvent& event);

private:    
    wxBitmap tapeOffBitmap;
    wxBitmap tapeOnBitmap;

    DECLARE_EVENT_TABLE()
};

#endif // GUIDIY_H

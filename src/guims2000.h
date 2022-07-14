#ifndef GUIMS2000_H
#define GUIMS2000_H

#include "guimcds.h"
#include "elfconfiguration.h"

class GuiMS2000: public GuiMcds
{
public:

    GuiMS2000(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiMS2000() {};

    void readMS2000Config();
    void writeMS2000DirConfig();
    void writeMS2000Config();
    void readMS2000WindowConfig();
    void writeMS2000WindowConfig();

    void onMS2000BaudR(wxCommandEvent& event);
    void onMS2000BaudT(wxCommandEvent& event);
    void onMS2000Uart(wxCommandEvent& event);
    void onMS2000Clock(wxCommandEvent& event);
    void onMS2000ForceUpperCase(wxCommandEvent& event);
    void onBootRam(wxCommandEvent&event);
    
protected:

private:
    
    DECLARE_EVENT_TABLE()
};

#endif // GUIMS2000_H

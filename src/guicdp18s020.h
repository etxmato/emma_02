#ifndef GUICDP18S020_H
#define GUICDP18S020_H

#include "guicdp18s600.h"

class GuiCdp18s020: public GuiCdp18s600
{
public:

    GuiCdp18s020(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode_, wxString dataDir, wxString iniDir);
    ~GuiCdp18s020() {};

    void readCdp18s020Config();
    void writeCdp18s020DirConfig();
    void writeCdp18s020Config();
    void readCdp18s020WindowConfig();
    void writeCdp18s020WindowConfig();

    void onRamSWCdp18s020(wxCommandEvent&event);
    void onLatch(wxCommandEvent&event);

    int getLatch() {return latch_;};

    void onCdp18s020BaudR(wxCommandEvent& event);
    void onCdp18s020BaudT(wxCommandEvent& event);
    void onCdp18s020ControlWindows(wxCommandEvent& event);
    bool getUseCdp18s020ControlWindows();
    void onAutoBoot(wxCommandEvent& event);
    void onAutoBootType(wxCommandEvent& event);
    void onCdp18s020ForceUpperCase(wxCommandEvent&event);

private:
    
    bool latch_;

    DECLARE_EVENT_TABLE()
};

#endif // GUICDP18S020_H

#ifndef GUIVELF_H
#define GUIVELF_H

#include "guicdp18s020.h"

class GuiVelf: public GuiCdp18s020
{
public:

    GuiVelf(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode_, wxString dataDir, wxString iniDir);
    ~GuiVelf() {};

    void readVelfConfig();
    void writeVelfDirConfig();
    void writeVelfConfig();
    void readVelfWindowConfig();
    void writeVelfWindowConfig();

    void onRamSWVelf(wxCommandEvent&event);
    void onLatch(wxCommandEvent&event);

    int getLatch() {return latch_;};

    void onVelfBaudR(wxCommandEvent& event);
    void onVelfBaudT(wxCommandEvent& event);
    void onVelfControlWindows(wxCommandEvent& event);
    bool getUseVelfControlWindows();
    int getVelfMode();
    void switchVelfMode();
    void onAutoBoot(wxCommandEvent& event);

private:

    bool latch_;

    DECLARE_EVENT_TABLE()
};

#endif // GUIVELF_H

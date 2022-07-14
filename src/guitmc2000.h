#ifndef GUITMC2000_H
#define GUITMC2000_H

#include "guieti660.h"

class GuiTMC2000: public GuiEti
{
public:

    GuiTMC2000(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiTMC2000() {};

    void readTMC2000Config();
    void writeTMC2000DirConfig();
    void writeTMC2000Config();
    void readTMC2000WindowConfig();
    void writeTMC2000WindowConfig();
    void readTMC1800Config();
    void writeTMC1800DirConfig();
    void writeTMC1800Config();
    void readTMC1800WindowConfig();
    void writeTMC1800WindowConfig();

private:

    DECLARE_EVENT_TABLE()
};

#endif // GUITMC2000_H

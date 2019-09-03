#ifndef GUICDP18S600_H
#define GUICDP18S600_H

#include "guitmc2000.h"
#include "elfconfiguration.h"

class GuiCdp18s600: public GuiTMC2000
{
public:
    
    GuiCdp18s600(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiCdp18s600() {};
    
    void readCdp18s600Config(int cdpType, wxString cdpTypeStr);
    void writeCdp18s600DirConfig(int cdpType, wxString cdpTypeStr);
    void writeCdp18s600Config(int cdpType, wxString cdpTypeStr);
    void readCdp18s600WindowConfig(int cdpType, wxString cdpTypeStr);
    void writeCdp18s600WindowConfig(int cdpType, wxString cdpTypeStr);
    
    void onCdp18s600BaudT(wxCommandEvent& event);
    void onCdp18s600Clock(wxCommandEvent& event);
    void onCdp18s600ForceUpperCase(wxCommandEvent& event);
    
    void onOneSocketBank(wxCommandEvent&event);
    void onFourSocketBank(wxCommandEvent&event);

    void onRomU21(wxCommandEvent& event);
    void onRomU21Text(wxCommandEvent& event);
    void onRomU20(wxCommandEvent& event);
    void onRomU20Text(wxCommandEvent& event);
    void onRomU19(wxCommandEvent& event);
    void onRomU19Text(wxCommandEvent& event);
    void onRomU18(wxCommandEvent& event);
    void onRomU18Text(wxCommandEvent& event);
    void onRomU17(wxCommandEvent& event);
    void onRomU17Text(wxCommandEvent& event);

    void onOneSocketSetup(wxCommandEvent& event);
    void setOneSocketState(wxString cdpTypeStr);
    void onCdp18s600ControlWindows(wxCommandEvent&event);
    void onPioWindows(wxCommandEvent&event);
    void pioWindows(bool state);
    bool getUseCdp18s600ControlWindows();
    bool getUsePioWindows();
    void setRamlabel(int romNumber, wxString romString, wxString cdpTypeStr, wxString label);
    void onFourSocketSetup(wxCommandEvent& event);
    void onRomSocketSetup(wxCommandEvent& event);
    void onRom603ASocketSetup(wxCommandEvent& event);
    void setFourSocketState(int cdpType, wxString cdpTypeStr);
    void onAutoBoot(wxCommandEvent&event);
    void onAutoBootType(wxCommandEvent&event);

protected:
    
private:
    wxPoint position_;
    wxString floppyDirSwitchedCdp18s600_[4];
    wxString floppyDirCdp18s600_[4];
    wxString floppyCdp18s600_[4];
    
    bool directoryMode_[4];

    DECLARE_EVENT_TABLE()
};

#endif // GUICDP18S600_H


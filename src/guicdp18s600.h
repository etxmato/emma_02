#ifndef GUICDP18S600_H
#define GUICDP18S600_H

#include "guitmc2000.h"
#include "elfconfiguration.h"

#define MICRO_MAXROM 4

class MicroMemoryConf
{
public:
    wxString ramDir_;
    wxString ram_;

    wxString romDir_[MICRO_MAXROM];
    wxString rom_[MICRO_MAXROM];
    int memType[MICRO_MAXROM];
  
    wxString chipBlockRam_;
    wxString chipBlockRom_[2];

    int memLocation_[2];
    int socketSize_;
    
    int inhibit64_;
    int inhibit32Low_;
    int inhibit32High_;

    bool useCdp18s626_;
    bool useCdp18s629_;
    bool useCdp18s652_;
};

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
    void onFour604BSocketBank(wxCommandEvent&event);
    int convert604BChipType(int type);
    void on604BRamBlock(wxCommandEvent&event);
    void on604BRomBlock(wxCommandEvent&event);
    void one604BSocketBankGui();
    void four604BSocketBankGui();

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
    void pioWindows(int computerType, bool state);
    bool getUseCdp18s600ControlWindows();
    bool getUsePioWindows();
    void setRamlabel(int romNumber, wxString romString, wxString cdpTypeStr, wxString label);
    void onFourSocketSetup(wxCommandEvent& event);
    void onRomSocketSetup(wxCommandEvent& event);
    void onRom603ASocketSetup(wxCommandEvent& event);
    void onMicroboardCard1Setup(wxCommandEvent& event);
    void onMicroboardCard2Setup(wxCommandEvent& event);
    void onMicroboardCard3Setup(wxCommandEvent& event);
    void onMicroboardCard4Setup(wxCommandEvent& event);
    void onMicroboardCard5Setup(wxCommandEvent& event);
    void onMicroboardType1(wxCommandEvent&event);
    void onMicroboardType2(wxCommandEvent&event);
    void onMicroboardType3(wxCommandEvent&event);
    void onMicroboardType4(wxCommandEvent&event);
    void onMicroboardType5(wxCommandEvent&event);
    void setCardType();
    void setTapeGui();
    void setFourSocketState(int cdpType, wxString cdpTypeStr);
    void onAutoBoot(wxCommandEvent&event);
    void onAutoBootType(wxCommandEvent&event);
    MicroMemoryConf getMicroMemConf(int card) {return microMemConf[card];};
    void setMicroMemConfiguration(int card, MicroMemoryConf microMemoryConf) { microMemConf[card-2] = microMemoryConf;};

protected:
    
private:
    wxPoint position_;

    MicroMemoryConf microMemConf[4];

    DECLARE_EVENT_TABLE()
};

#endif // GUICDP18S600_H


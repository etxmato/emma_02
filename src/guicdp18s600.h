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

    int memLocation_[3];
    int socketSize_[2];
  
    bool inhibitBlock_[2][4];

    int inhibit64_;
    int inhibit32Low_;
    int inhibit32High_;

    bool disableCardMemory_;

    bool useCdp18s620_;
    bool useCdp18s621_;
    bool useCdp18s623a_;
    bool useCdp18s625_;
    bool useCdp18s626_;
    bool useCdp18s627_;
    bool useCdp18s628_;
    bool useCdp18s629_;
    bool useCdp18s640_;
    bool useCdp18s652_;
    bool useCdp18s661_;
    bool useCdp18s660_;
};

class GuiCdp18s600: public GuiTMC2000
{
public:
    
    GuiCdp18s600(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiCdp18s600() {};
    
    void readCdp18s600Config();
    void writeCdp18s600DirConfig();
    void writeCdp18s600Config();
    void readCdp18s600WindowConfig();
    void writeCdp18s600WindowConfig();
    
    void onCdp18s600BaudT(wxCommandEvent& event);
    void onCdp18s600Clock(wxCommandEvent& event);
    void onCdp18s600ForceUpperCase(wxCommandEvent& event);
    
    int convert604BChipType(int type);

    void onPioWindows(wxCommandEvent&event);
    void pioWindows(int computerType, bool state);
    bool getUseCdp18s600ControlWindows();
    bool getUsePioWindows();
    void setRamlabel(int romNumber, wxString romString, wxString cdpTypeStr, wxString label);
    void onMicroboardCard1Setup(wxCommandEvent& event);
    void onMicroboardCard2Setup(wxCommandEvent& event);
    void onMicroboardCard3Setup(wxCommandEvent& event);
    void onMicroboardCard4Setup(wxCommandEvent& event);
    void onMicroboardCardSetup(wxCommandEvent& event);
    void onMicroboardType1(wxCommandEvent&event);
    void onMicroboardType2(wxCommandEvent&event);
    void onMicroboardType3(wxCommandEvent&event);
    void onMicroboardType4(wxCommandEvent&event);
    void onMicroboardCard(wxCommandEvent&event);
    void setCardMax();
    void setCardType();
    wxString checkBoardType(int card, wxString cardstring, wxString oldStr, bool boardControlValue);
    void setChoiceColor(wxString cardstring, bool error);
    void setTapeGui();
    void onAutoBoot(wxCommandEvent&event);
    void onAutoBootType(wxCommandEvent&event);
    MicroMemoryConf getMicroMemConf(int card) {return microMemConf[card];};
    void setMicroMemConfiguration(int card, MicroMemoryConf microMemoryConf) { microMemConf[card-2] = microMemoryConf;};

    int getMicroboardType(int computerType) {return conf[computerType].microboardType_[1];};
    int getMicroboardMaxCard(int computerType) {return conf[computerType].microboardMaxCards_;};
    wxString getMicroboardTypeStr(int boardType);
protected:
    
private:
    wxPoint position_;

    vector<MicroMemoryConf> microMemConf;

//    MicroMemoryConf microMemConf[24];

    DECLARE_EVENT_TABLE()
};

#endif // GUICDP18S600_H


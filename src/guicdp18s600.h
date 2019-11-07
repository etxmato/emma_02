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
    void cdp18660WindowPio1(int computerType, bool state);
    void cdp18660WindowPio2(int computerType, bool state);
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
    void onMicroboardType5(wxCommandEvent&event);
    void clearConfigName();
    void setCardMax(Conf* config);
    void setCardType();
    void checkAllBoardTypes(Conf* config, ElfConfiguration* elfConfig);
    void checkAllBoardTypes(Conf* config, ElfConfiguration* elfConfig, MicroMemoryConf microMemoryConfCardx, int cardx);
    void setMemoryMap(Conf* config, long start, long end, int card, int boardType);
    void setMemoryMap(Conf* config, long start, long end, int card, int boardType, long inhibitStart, long inhibitEnd);
    void setMemoryMapCDP18S600(Conf* config, int card, int boardType);
    void setMemoryMapCDP18S601(Conf* config, int card, int boardType);
    void setMemoryMapCDP18S602(Conf* config, int card, int boardType);
    void setMemoryMapCDP18S603a(Conf* config, int card, int boardType);
    void setMemoryMapCDP18S604b(Conf* config, int card, int boardType);
    void setMemoryMapCDP18S620(Conf* config, MicroMemoryConf memConf, int card, int boardType);
    void setMemoryMapCDP18S621(Conf* config, MicroMemoryConf memConf, int card, int boardType);
    void setMemoryMapCDP18S623a(Conf* config, MicroMemoryConf memConf, int card, int boardType);
    void setMemoryMapCDP18S625(Conf* config, MicroMemoryConf memConf, int card, int boardType);
    void setMemoryMapCDP18S626(Conf* config, MicroMemoryConf memConf, int card, int boardType);
    void setMemoryMapCDP18S627(Conf* config, MicroMemoryConf memConf, int card, int boardType);
    void setMemoryMapCDP18S640(Conf* config, MicroMemoryConf memConf, int card, int boardType);
    void setMemoryMapCDP18S652(Conf* config, MicroMemoryConf memConf, int card, int boardType);
    void setMemoryMapCDP18S660(Conf* config, MicroMemoryConf memConf, int card, int boardType);
    void setInhibitBlock(MicroMemoryConf memConf, Word* startAddress, Word* socketSize, Word* inhibitStart, Word* inhibitStop, int bank);
    void checkBoardType(Conf* config, int card, wxString cardstring, int oldCard, wxString oldStr, bool boardControlValue);
    void setButtonColor(wxString cardstring, bool error);
    void setTapeGui();
    void onAutoBoot(wxCommandEvent&event);
    void onAutoBootType(wxCommandEvent&event);
    MicroMemoryConf getMicroMemConf(int card) {return microMemConf[card-1];};
    void setMicroMemConfiguration(int card, MicroMemoryConf microMemoryConf) { microMemConf[card-1] = microMemoryConf;};

    int getMicroboardType(int computerType) {return conf[computerType].microboardType_[0];};
    int getMicroboardMaxCard(int computerType) {return conf[computerType].microboardMaxCards_;};
    wxString getMicroboardTypeStr(int boardType);
    
protected:
    
private:
    wxPoint position_;

    vector<MicroMemoryConf> microMemConf;

    DECLARE_EVENT_TABLE()
};

#endif // GUICDP18S600_H


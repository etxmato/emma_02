#ifndef GUICOMX_H
#define GUICOMX_H

#include "guixml.h"

DECLARE_EVENT_TYPE(STATUS_LED_ON, 801)
DECLARE_EVENT_TYPE(STATUS_LED_OFF, 802)
DECLARE_EVENT_TYPE(STATUS_BAR_1870, 803)
DECLARE_EVENT_TYPE(EXP_LED_ON, 805)
DECLARE_EVENT_TYPE(EXP_LED_OFF, 806)
DECLARE_EVENT_TYPE(KILL_COMPUTER, 809)

class GuiComx: public GuiXml
{
public:

    GuiComx(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiComx();

    void setLocation(bool state, Word saveStart, Word saveEnd, Word saveExec);
    void onSBDialog(wxCommandEvent& event);
    void readSbConfig();
    void writeSbConfig();
    void onDiagDialog(wxCommandEvent& event);

    void statusLedOn(wxCommandEvent &event);
    void statusLedOff(wxCommandEvent &event);
    void statusLedOnDirect();
    void statusLedOffDirect();
    void v1870BarSize(wxCommandEvent &event);
    void expLedOn(wxCommandEvent &event);
    void expLedOff(wxCommandEvent &event);
    void expLedOnDirect();
    void expLedOffDirect();

    wxString getPL80Data(int item);
    void setPL80Data(int item, wxString data);
    void setFandMBasicGui();
    bool getUseExpansionRam() {return useExpansionRam_;};
    wxString getSbEmail() {return sbEmail_;}; 
    wxString getSbPlayer() {return sbPlayer_;}; 
    wxString getSbLocation() {return sbLocation_;}; 
    wxString getSbUrlHome() {return sbUrlHome_;}; 
    wxString getSbUrlBookMark(int number) {return sbUrlBookMark_[number];};
    wxString getSbRootDirectory() {return sbRootDirectory_;}; 
    wxString getSbRomDirectory(int number) {return SBRomDir_[number];};
    wxString getSbRom(int number) {return SBRom_[number];};
    wxString getEpromRomDirectory(int number) {return EpromRomDir_[number];};
    wxString getEpromRom(int number) {return EpromRom_[number];};

    int getSbCdRoot() {return sbCdRoot_;};
    int getSbBackup() {return sbBackup_;}; 
    int getSbBackupSys() {return sbBackupSys_;}; 
    int getSbCaseFile() {return sbCaseFile_;}; 
    int getSbCaseDir() {return sbCaseDir_;}; 
    int getSbFwVersion() {return sbFwVersion_;};
    bool isDiagActive() { return computerConfiguration.diagnosticBoardConfiguration.defined; };
  
    void setSbEmail(wxString sbEmail) { sbEmail_ = sbEmail; };
    void setSbPlayer(wxString sbPlayer) { sbPlayer_ = sbPlayer; };
    void setSbLocation(wxString sbLocation) { sbLocation_ = sbLocation; };
    void setSbUrlHome(wxString urlHome); 
    void setSbRootDirectory(wxString sbRootDirectory) { sbRootDirectory_ = sbRootDirectory; };

    void setSbCdRoot(int sdCdRoot) { sbCdRoot_ = sdCdRoot; };
    void getSbBackup(int sbBackup) { sbBackup_ = sbBackup; };
    void getSbBackupSys(int sbBackupSys) { sbBackupSys_ = sbBackupSys; };
    void getSbCaseFile(int sbCaseFile) { sbCaseFile_ = sbCaseFile; };
    void setSbCaseDir(int sbCaseDir) { sbCaseDir_ = sbCaseDir; };
    void setSbFwVersion(int version);

    void setComxStatusLedOn(bool status) { isStatusLedOn_ = status; };
    void setComxExpLedOn(bool status) {isExpLedOn_ = status;};

    void statusLedOnEvent();
    void statusLedOffEvent();
    void expLedOnEvent();
    void expLedOffEvent();
    void v1870BarSizeEvent();

    wxString getUrlBookMark(int number) {return sbUrlBookMark_[number];}; 
    void setSbUrlBookMark(int number, wxString urlBookMark);
    void setUrlBookMark(int number, wxString urlBookMark);

    size_t getNumberOfAlias() {return numberOfAlias_;}; 
    wxString getAlias(size_t number); 
    wxString getAliasEmail(size_t number); 
    void setAlias(long number, wxString aliasNew, wxString aliasEmailNew);
    void deleteAlias(int aliasDel);

protected:
    wxString sbPlayer_;
    wxString sbLocation_;
    wxString sbEmail_;

    vector<wxString> alias;
    vector<wxString> aliasEmail;
    size_t numberOfAlias_;

private:
    wxString Pl80Data_[3];
    bool useExpansionRam_;
    int expansionRamSlot_;
    wxString saveCardText_;

    wxString sbUrlHome_;
    wxString sbUrlBookMark_[10];
    wxString sbRootDirectory_;
    wxString DiagPalRomDir_[2];
    wxString DiagPalRom_[2];
    wxString DiagNtscRomDir_[2];
    wxString DiagNtscRom_[2];
    wxString SBRomDir_[12];
    wxString SBRom_[12];
    wxString EpromRomDir_[5];
    wxString EpromRom_[5];

    int sbCdRoot_;
    int sbBackup_;
    int sbBackupSys_;
    int sbCaseFile_;
    int sbCaseDir_;
    int sbFwVersion_;
    
    bool isStatusLedOn_;
    bool isExpLedOn_;
    
    DECLARE_EVENT_TABLE()
};

#endif // GUICOMX_H

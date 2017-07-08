#ifndef GUICOMX_H
#define GUICOMX_H

#include "guielf.h"

DECLARE_EVENT_TYPE(OPEN_COMX_PRINTER_WINDOW, 800)
DECLARE_EVENT_TYPE(STATUS_LED_ON, 801)
DECLARE_EVENT_TYPE(STATUS_LED_OFF, 802)
DECLARE_EVENT_TYPE(STATUS_BAR_1870, 803)
DECLARE_EVENT_TYPE(EXP_LED_ON, 805)
DECLARE_EVENT_TYPE(EXP_LED_OFF, 806)
DECLARE_EVENT_TYPE(KILL_COMPUTER, 809)

class GuiComx: public GuiElf
{
public:

	GuiComx(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir);
	~GuiComx();

	void readComxConfig();
    void readSbConfig();
    void readInitialComxConfig();
    void writeSbConfig();
    void writeComxDirConfig();
	void writeComxConfig();
    void readComxWindowConfig();
    void writeComxWindowConfig();

	void onExpansionRom(wxCommandEvent& event);
	void onExpansionRomText(wxCommandEvent& event);
	void onCard1Rom(wxCommandEvent& event);
	void onCard1RomText(wxCommandEvent& event);
	void onCard2Rom(wxCommandEvent& event);
	void onCard2RomText(wxCommandEvent& event);
	void onCard3Rom(wxCommandEvent& event);
	void onCard3RomText(wxCommandEvent& event);
	void onCard4Rom(wxCommandEvent& event);
	void onCard4RomText(wxCommandEvent& event);
	void onComxDisk1(wxCommandEvent& event);
	void onComxDiskText1(wxCommandEvent& event);
	void onComxDiskEject1(wxCommandEvent& event);
	void onComxDisk2(wxCommandEvent& event);
	void onComxDiskText2(wxCommandEvent& event);
	void onComxDiskEject2(wxCommandEvent& event);
	void onComxPrintFileText(wxCommandEvent& event);
	void onComxPrintMode(wxCommandEvent& event);
	void onComxPrintButton(wxCommandEvent& event);
	void onComxVideoMode(wxCommandEvent& event);
	void onComxExpansionRam(wxCommandEvent& event);
	void onComxExpansionRamSlot(wxSpinEvent&event);
	void setLocation(bool state, Word saveStart, Word saveEnd, Word saveExec);
	void onEpromDialog(wxCommandEvent& event);
	void onSBDialog(wxCommandEvent& event);
	void onSbActive(wxCommandEvent& event);
	void diagSbChange();
	void onDiagActive(wxCommandEvent& event);

	void statusLedOn(wxCommandEvent &event);
	void statusLedOff(wxCommandEvent &event);
	void statusLedOnDirect();
	void statusLedOffDirect();
	void v1870BarSize(wxCommandEvent &event);
	void expLedOn(wxCommandEvent &event);
	void expLedOff(wxCommandEvent &event);
	void expLedOnDirect();
	void expLedOffDirect();
	void openComxPrinterFrame(wxCommandEvent &event);

	wxString getFloppyDir(int drive);
	wxString getFloppyFile(int drive);
	wxString getPL80Data(int item);
	void setPL80Data(int item, wxString data);
	void enableComxGui(bool status);
	void setComxExpRamSlot();
	void resetCardText();
	bool isSaving();
	void setComxPrintMode();
	void onComxF4();
	void setFandMBasicGui();
	void enableDiskRomGui(bool DiskRom);
	void setExtRomStatus(bool expansionRomLoaded);
	void setComxPrintMode(int mode);
	int getComxPrintMode();
	int getExpansionRamSlot() {return expansionRamSlot_;};
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

	void setSbEmail(wxString sbEmail) { sbEmail_ = sbEmail; };
	void setSbPlayer(wxString sbPlayer) { sbPlayer_ = sbPlayer; };
	void setSbLocation(wxString sbLocation) { sbLocation_ = sbLocation; };
	void setSbUrlHome(wxString urlHome); 
	void setSbRootDirectory(wxString sbRootDirectory) { sbRootDirectory_ = sbRootDirectory; };
    void setSbRomDirectory(int number, wxString directory);
    void setSbRom(int number, wxString filename);
    void setEpromRomDirectory(int number, wxString directory);
    void setEpromRom(int number, wxString filename);

	void setSbCdRoot(int sdCdRoot) { sbCdRoot_ = sdCdRoot; };
	void getSbBackup(int sbBackup) { sbBackup_ = sbBackup; };
	void getSbBackupSys(int sbBackupSys) { sbBackupSys_ = sbBackupSys; };
	void getSbCaseFile(int sbCaseFile) { sbCaseFile_ = sbCaseFile; };
	void setSbCaseDir(int sbCaseDir) { sbCaseDir_ = sbCaseDir; };
	void setSbFwVersion(int version);

	void setComxStatusLedOn(bool status) { isComxStatusLedOn_ = status; };
	void setComxExpLedOn (bool status) {isComxExpLedOn_ = status;};

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
    void onLogComx(wxCommandEvent&event);

protected:
	bool expansionRomLoaded_;
	bool diskRomLoaded_;
	wxString sbPlayer_;
	wxString sbLocation_;
	wxString sbEmail_;

	vector<wxString> alias;
	vector<wxString> aliasEmail;
	size_t numberOfAlias_;

private:
	wxString Pl80Data_[3];
	wxString floppyDir_[2];
	wxString floppy_[2];
	bool useExpansionRam_;
	int expansionRamSlot_;
	wxString saveCardText_;

	wxString sbUrlHome_;
	wxString sbUrlBookMark_[10];
	wxString sbRootDirectory_;
	wxString DiagRomDir_[2];
	wxString DiagRom_[2];
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

	int comxPrintMode_;
	bool isComxStatusLedOn_;
	bool isComxExpLedOn_;

	DECLARE_EVENT_TABLE()
};

#endif // GUICOMX_H

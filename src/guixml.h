#ifndef GUIDIY_H
#define GUIDIY_H

#include "guivip2.h"
#include "computerconfig.h"

class GuiXml: public GuiVipII
{
public:

    GuiXml(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiXml() {};

    void readXmlConfig();
    void writeXmlDirConfig();
    void writeXmlConfig();
    void readXmlWindowConfig();
    void writeXmlWindowConfig();
    void readDefaultVtConfig();
    void writeDefaultVtConfig();
    bool showSplashScreen();
    void hideSplashScreen();

    void onKeyFile(wxCommandEvent& event);
    void onKeyFileText(wxCommandEvent& event);
    void onKeyFileEject(wxCommandEvent& event);
    void onScreenDumpFile(wxCommandEvent& event);
    void onScreenDumpFileText(wxCommandEvent& event);
    void onXmodem(wxCommandEvent& event);
    void onXmodemText(wxCommandEvent& event);
    void onXmodemEject(wxCommandEvent& event);
    void startAutoTerminalSave(int protocol);
    void startTerminalSave(int protocol);
    void onCassette(wxCommandEvent& event);
    void onCassetteFileSelector();
    void onCassette1(wxCommandEvent& event);
    void onCassetteEject(wxCommandEvent& event);
    void onCassette1Eject(wxCommandEvent& event);
    void onCassetteText(wxCommandEvent& event);
    void onCassette1Text(wxCommandEvent& event);
    void onDisk0(wxCommandEvent& event);
    void onDisk1(wxCommandEvent& event);
    void onDisk2(wxCommandEvent& event);
    void onDisk3(wxCommandEvent& event);
    void onDisk(int diskNumber);
    void onDiskText0(wxCommandEvent& event);
    void onDiskText1(wxCommandEvent& event);
    void onDiskText2(wxCommandEvent& event);
    void onDiskText3(wxCommandEvent& event);
    void onDiskText(int diskNumber, wxString eventString);
    void onDiskEject0(wxCommandEvent& event);
    void onDiskEject1(wxCommandEvent& event);
    void onDiskEject2(wxCommandEvent& event);
    void onDiskEject3(wxCommandEvent& event);
    void onDiskEject(int diskNumber);
    void onDiskDirSwitch0(wxCommandEvent& event);
    void onDiskDirSwitch1(wxCommandEvent& event);
    void onDiskDirSwitch2(wxCommandEvent& event);
    void onDiskDirSwitch3(wxCommandEvent& event);
    void onDiskDirSwitch(int diskNumber);

    void onXmlControlWindows(wxCommandEvent& event);

    void onLedTimerXml(wxCommandEvent&event);
    void onAutoLoadXml(wxCommandEvent&event);
    void onTurboXml(wxCommandEvent&event);
    void onUseLocationXml(wxCommandEvent&event);
    void onRomRam0Xml(wxCommandEvent& event);
    void onRomRam1Xml(wxCommandEvent& event);
    void romRamXml(int romRamButton, wxString romRamButtonString);
    void onRomRom0TextXml(wxCommandEvent& event);
    void onRomRom1TextXml(wxCommandEvent& event);
    void romRomTextXml(int romRamButton, wxString romRamButtonString);
    void onRomRom0ComboXml(wxCommandEvent& event);
    void onRomRom1ComboXml(wxCommandEvent& event);
    void romRomComboXml(int romRamButton, wxString romRamButtonString);
    void onMainXmlXml(wxCommandEvent& event);
    void onMainXmlTextXml(wxCommandEvent& event);
    void onMainXmlComboXml(wxCommandEvent& event);
    void onMainDirComboXml(wxCommandEvent& event);
    void setXmlDirFileGui();
    void setXmlDropDown();
    void setRomRamDropDown(int romRamButton, wxString romRamButtonString);
    void setXmlDirDropDown();
    void setXmlSubDir(wxString directory);
    void setPrintModeXml();
    void setRomRamButtonOrder();
    void setXmlGui();
    void onVideoNumber(wxCommandEvent&event);
    void onPrintFile(wxCommandEvent& event);
    void onXmlPrintFileText(wxCommandEvent&event);
    void onXmlPrintMode(wxCommandEvent&event);
    void onXmlPrintButton(wxCommandEvent& event);
    void onXmlF4(bool forceStart);
    void onXmlBaudR(wxCommandEvent&event);
    void onXmlBaudT(wxCommandEvent&event);
    void onTempo(wxScrollEvent&event);

    void setNvRamDisable(bool status){computerConfiguration.nvRamConfiguration.disable = status;};
    bool getNvRamDisable(){return computerConfiguration.nvRamConfiguration.disable;};
    
    int getRomRamButton0() {return romRamButton0_;};
    int getRomRamButton1() {return romRamButton1_;};
    wxString getComputerString() {return dirNameList_[xmlDirComboSelection];};

    bool getUpperCase() {return computerConfiguration.forceUpperCase;};
    int getPacketSize() {return computerConfiguration.videoTerminalConfiguration.packetSize;};
    bool showFrontPanel() {return computerConfiguration.frontPanelConfiguration[PANEL_MAIN].show;};

protected:
    int romRamButton0_;
    int romRamButton1_;
    wxString xmlDirComboString;

private:
    wxArrayString dirNameList_;
    wxArrayString dirNameListDefaultFile_;
    wxArrayString dirNameListGui_;

    size_t xmlFileComboSelection;
    size_t xmlDirComboSelection;

    wxArrayString romRamFileNameListGui_[2];
    wxArrayString romRamDirNameListGui_[2];
    size_t romRamFileComboSelection[2];

    bool dropdownUpdateOngoing_;
    
    DECLARE_EVENT_TABLE()
};

#endif // GUIDIY_H

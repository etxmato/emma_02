#ifndef GUIMAIN_H
#define GUIMAIN_H

#include "wx/xml/xml.h"
#include "computerconfig.h"

#define NO_COMPUTER 34
#define LAST_ELF_TYPE 19

#define MAINROM1 0

// Studio II
#define CARTROM 1

// Elf
#define MAINROM2 1

// MSDC
#define MAINROM3 2

// Telmac 600
//#define EXPROM 1

// Comx
#define EXPROM 1
#define CARTROM1 2
#define CARTROM2 3
#define CARTROM3 4
#define CARTROM4 5

// CDP18S600
#define U21ROM 0
#define U20ROM 1
#define U19ROM 2
#define U18ROM 3
#define U17ROM 4

#define ONE_SOCKET 0
#define FOUR_SOCKET 1

#define FOUR_SOCKET_ROM1 1
#define FOUR_SOCKET_ROM2 2

#define XU27ROM 1
#define XU26ROM 2
#define XU25ROM 3
#define XU24ROM 4

#define XU23ROM 0
#define XU22ROM 1
#define XU21ROM 2
#define XU20ROM 3

#define CDP626_SEC1 0
#define CDP626_SEC2 1
#define CDP626_SEC3 2
#define CDP626_SEC4 3

//MICROBOARD CARD TYPES
#define CARD_EMPTY 0
#define CARD_CDP18S620 1
#define CARD_CDP18S621 2
#define CARD_CDP18S623A 3
#define CARD_CDP18S625 4
#define CARD_CDP18S626 5
#define CARD_CDP18S627 6
#define CARD_CDP18S628 7
#define CARD_CDP18S629 8
#define CARD_CDP18S640 9
#define CARD_CDP18S641 10
#define CARD_CDP18S646 11
#define CARD_CDP18S651 12
#define CARD_CDP18S652 13
#define CARD_CDP18S660 14
#define CARD_CDP18S661B 15
#define CARD_CDP18S661V1 16
#define CARD_CDP18S661V3 17
#define CARD_LAST 17

#define FDCTYPE_MS2000 0
#define FDCTYPE_17XX 0
#define FDCTYPE_MICROBOARD 1
#define FDCTYPE_MAX 2

// CDP18S626
#define MEM_SECTION0 0
#define MEM_SECTION1 1
#define MEM_SECTION2 2
#define MEM_SECTION3 3

// CDP18S652
#define ROM_SOCKET1 0
#define ROM_SOCKET2 1
#define ROM_SOCKET3 2
#define ROM_SOCKET4 3

#define VIPII_ED 0
#define VIPII_RCA 1

#define GUI_COL_BLACK 0
#define GUI_COL_BLUE 1
#define GUI_COL_PINK 2
#define GUI_COL_RED 3
#define GUI_COL_ORANGE 4
#define GUI_COL_PURPLE 5
#define GUI_COL_STEEL 6
#define GUI_COL_GREY 7
#define GUI_COL_GREEN 8
#define GUI_COL_MAX 9

DECLARE_EVENT_TYPE(OPEN_PRINTER_WINDOW, 800) 

class ComputerInfo
{
public:
    wxString gui;
    wxString name;
    wxString configuration;
//    wxString extension;
    wxString ploadExtension;
};

class GuiMain : public wxFrame
{
public:

    GuiMain(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiMain() {};

    WindowInfo getWinSizeInfo(wxString appDir, wxString fontSizeString);

    void readElfPortConfig(int elfType, wxString elfTypeStr);
    void writeElfPortConfig(int elfType, wxString elfTypeStr);

    wxString readConfigDir(const wxString& key, const wxString& defVal = wxEmptyString);
    void writeConfigDir(const wxString& key, const wxString& value);

    void onMainRom1(wxCommandEvent& event);
    void onMainRom1Text(wxCommandEvent& event);

    void onMainRom2(wxCommandEvent& event);
    void onMainRom2Text(wxCommandEvent& event);

    void onMainRom3(wxCommandEvent& event);
    void onMainRom3Text(wxCommandEvent& event);

    void onCartRom(wxCommandEvent& event);
    void onCartRomText(wxCommandEvent& event);

    void onRamSW(wxCommandEvent& event);
    void onRamSWText(wxCommandEvent& event);

    void onChip8SW(wxCommandEvent& event);
    void onChip8SWText(wxCommandEvent& event);
    void onEjectChip8SW(wxCommandEvent& event);
    void onPrintFile(wxCommandEvent& event);
    void onPrintFileText(wxCommandEvent& event);
    void onPrintButton(wxCommandEvent& event);
    void onPrintMode(wxCommandEvent& event);
    void openPrinterFrame(wxCommandEvent &event);
    void onF4();
    void setPrintMode();
    int getPrintMode();
    void onIde(wxCommandEvent& event);
    void onIdeEject(wxCommandEvent& event);
    void onIdeText(wxCommandEvent& event);
    void onCharRom(wxCommandEvent& event);
    void onCharRomText(wxCommandEvent& event);
    void onKeyFile(wxCommandEvent& event);
    void onKeyFileText(wxCommandEvent& event);
    void onKeyFileEject(wxCommandEvent& event);
    void onVT100(wxCommandEvent& event);
    void onZoom(wxSpinEvent& event);
    void onZoom(int direction);
    void onZoomValue(wxCommandEvent& event);
    void onZoomVt(wxSpinEvent& event);
    void onZoomValueVt(wxCommandEvent& event);
    void onFullScreen(wxCommandEvent&event);
    void onInterlace(wxCommandEvent& event);
    void onUpperCase(wxCommandEvent& event);
    void onStretchDot(wxCommandEvent& event);
    void onScreenDumpFile(wxCommandEvent& event);
    void onScreenDumpFileText(wxCommandEvent& event);
    void onScreenDump(wxCommandEvent& event);
    void onDp(wxCommandEvent& event);
    void onVolume(wxScrollEvent&event);
    void onCassette(wxCommandEvent& event);
    void onCassetteFileDialog();
    void onCassetteFileSelector();
    void onCassette1(wxCommandEvent& event);
    void onXmodem(wxCommandEvent& event);
    void onTerminalFile(wxCommandEvent& event);
    void onCassetteEject(wxCommandEvent& event);
    void onCassette1Eject(wxCommandEvent& event);
    void onCassetteText(wxCommandEvent& event);
    void onCassette1Text(wxCommandEvent& event);
    void onXmodemText(wxCommandEvent& event);
    void onXmodemEject(wxCommandEvent& event);
    void onAutoLoad(wxCommandEvent& event);
    void onRealCas(wxCommandEvent& event);
    void onWavFile(wxCommandEvent& event);
    void onTurbo(wxCommandEvent&event);
    void onTurboClock(wxCommandEvent& event);
    void onUseLocation(wxCommandEvent& event);
    void onCassetteLoad(wxCommandEvent& event);
    void onCassetteForward(wxCommandEvent& event);
    void onCassetteRewind(wxCommandEvent& event);
    void onCassetteSave(wxCommandEvent& event);
    void onCassetteStop(wxCommandEvent& event);
    void onCassetteLoad1(wxCommandEvent& event);
    void onCassetteSave1(wxCommandEvent& event);
    void onCassettePause(wxCommandEvent& event);
    void startHwSave();
    void startHwLoad();
    void onKeyboard(wxCommandEvent& event);
    void onPsave(wxString fileName);
    void onDataSaveButton(wxCommandEvent& event);
    void onLoadRunButton(wxCommandEvent& event);
    void onLoadButton(wxCommandEvent& event);
    void onSaveStart(wxCommandEvent& event);
    void onSaveEnd(wxCommandEvent& event);
    void onSaveExec(wxCommandEvent& event);
    void runSoftware(bool load);
    void onLoad(bool load);
    void onSaveButton(wxCommandEvent& event);
    void onClock(wxCommandEvent& event);
    void onBaudR(wxCommandEvent& event);
    void onBaudT(wxCommandEvent& event);
    void onClearRam(wxCommandEvent& event);
    void onClearRtc(wxCommandEvent& event);
    void onBootAddress(wxCommandEvent& event);
    long getBootAddress(wxString computerTypeStr, int computerType);
    void onChoiceRam(wxCommandEvent&event);
    void onChoiceCpu(wxCommandEvent&event);

    int getHwTapeState() {return hwTapeState_;}

    void onPsaveMenu(wxCommandEvent& event);
    void onVtSetup(wxCommandEvent& event);
    void onBeepFrequency(wxCommandEvent&event);

    wxString convertLocale(wxString input);

    bool toDouble(wxString stringName, double* result);
    wxString getMainDir() {return conf[runningComputer_].mainDir_;};
    bool getGuiMode() {return mode_.gui;};

    wxString getRomDir(int computerType, int romType) {return conf[computerType].romDir_[romType];};
    wxString getRamDir(int computerType) {return conf[computerType].ramDir_;};
    wxString getChip8Dir(int computerType){return conf[computerType].chip8SWDir_;};
    wxString getIdeDir(int computerType) {return conf[computerType].ideDir_;};
    wxString getCharRomDir(int computerType) {return conf[computerType].charRomDir_;};
    wxString getVt52CharRomDir(int computerType) {return elfConfiguration[computerType].vt52CharRomDir_;};
    wxString getVt100CharRomDir(int computerType) {return elfConfiguration[computerType].vt100CharRomDir_;};
    wxString getWaveDir(int computerType) {return conf[computerType].wavFileDir_[0];};
    void setWaveDir(int computerType, wxString fileDir) {conf[computerType].wavFileDir_[0] = fileDir;};
    wxArrayString getTerminalPaths(int computerType) {return conf[computerType].terminalPaths_;}
    wxString getTerminalPath(int computerType, int filenumber) {return conf[computerType].terminalPaths_[filenumber];}
    wxArrayString getTerminalFiles(int computerType) {return conf[computerType].terminalFiles_;}
    size_t getNumberOfTerminalFiles(int computerType) {return conf[computerType].numberOfTerminalFiles_;}
    bool getAutCassetteLoad() {return conf[runningComputer_].autoCassetteLoad_;};
    bool getPrinterStatus(int computerType) {return conf[computerType].printerOn_;};
    PixieGraphics getPixieGraphics(int computerType) {return elfConfiguration[computerType].ioConfiguration.pixieGraphics;};
    int getVideoHeight(int computerType) {return elfConfiguration[computerType].ioConfiguration.videoHeight;};
    int getVideoWidth(int computerType) {return elfConfiguration[computerType].ioConfiguration.videoWidth;};
    int getFrontPanelRevision() {return elfConfiguration[runningComputer_].frontType;};

    wxString getSplashDialog(int computerType) {return conf[computerType].splashDialog_;};
    wxString getSplashText(int computerType) {return conf[computerType].splashText_;};

    wxString getSelectedComputerStr() {return computerInfo[selectedComputer_].gui;};
    wxString getRunningComputerStr() {return computerInfo[runningComputer_].gui;};
    wxString getSelectedComputerText() {return computerInfo[selectedComputer_].name;};
    wxString getRunningComputerText() {return computerInfo[runningComputer_].name;};
    wxString getRunningComputerPloadExtension() {return computerInfo[runningComputer_].ploadExtension;};
    int getSelectedComputerId() {return selectedComputer_;};
    int getRunningComputerId() {return runningComputer_;};
    int getVolume(int computerType) {return conf[computerType].volume_;};
    int getVideoMode(int computerType) {return conf[computerType].videoMode_;};
    int getComputerVersion(int computerType) {return conf[computerType].computerVersion_;};
    bool getInterlace(int computerType) {return conf[computerType].interlace_;};
    bool getStretchDot(int computerType) {return conf[computerType].stretchDot_;};
    int getSound(int computerType) {return conf[computerType].soundType_;};
    int getRamType(int computerType) {return conf[computerType].ramType_;};
    bool getUseKeyboard(int computerType) {return conf[computerType].useKeyboard_;}; 
    bool getUseLoadLocation(int computerType) {return conf[computerType].useLoadLocation_;}; 
    wxString getSaveStartString(int computerType) {return conf[computerType].saveStartString_;}; 
    wxString getSaveEndString(int computerType) {return conf[computerType].saveEndString_;}; 
    wxString getSaveExecString(int computerType) {return conf[computerType].saveExecString_;}; 
    bool getUseMultiCart(int computerType) { return conf[computerType].multiCart_; };
    bool getDisableSystemRom(int computerType) { return conf[computerType].disableSystemRom_; };
    Byte getMultiCartLsb(int computerType) {return conf[computerType].lsb_;};
    Byte getMultiCartMsb(int computerType) {return conf[computerType].msb_;};
    int getFdcType(int computerType) {return elfConfiguration[computerType].fdcType_;};
    int getBarLedPosX1() {return windowInfo.ledPosX1;};
    int getBarLedPosX2() {return windowInfo.ledPosX2;};
    int getBarLedSpacing() {return windowInfo.ledSpacing;};
    int getBarLedPosY() {return windowInfo.ledPosY;};
    int getBarLedPosDiagY() {return windowInfo.ledPosDiagY;};
    int getBarLedPosVip2Y() {return windowInfo.ledPosVip2Y;};
    int getStatusBarElementMeasure(int number) {return windowInfo.statusBarElementMeasure[number];};
    wxString getBarLeader() {return windowInfo.statusBarLeader;};
    wxString getBarLeaderCidelsa() {return windowInfo.statusBarLeaderCidelsa;};
    int getPrintX() {return printX_;};

    wxString getLedTime(int computerType) {return conf[computerType].ledTime_;}; 
    long getLedTimeMs(int computerType) {return conf[computerType].ledTimeMs_;}; 

    wxString getRomFile(int computerType, int romType) {return conf[computerType].rom_[romType];};
    wxString getRamFile(int computerType) {return conf[computerType].ram_;};
    wxString getChip8SW(int computerType){return conf[computerType].chip8SW_;};
    wxString getIdeFile(int computerType) {return conf[computerType].ide_;};
    wxString getCharRomFile(int computerType) {return conf[computerType].charRom_;};
    wxString getVt100CharRomFile(int computerType) {return elfConfiguration[computerType].vt100CharRom_;};
    wxString getVt52CharRomFile(int computerType) {return elfConfiguration[computerType].vt52CharRom_;};
    wxString getWaveFile(int computerType) {return conf[computerType].wavFile_[0];};
    void setWaveFile(int computerType, wxString fileName) {conf[computerType].wavFile_[0] = fileName;};
    wxString getWaveFile1(int computerType) {return conf[computerType].wavFile_[1];};
    wxString getKeyFile();
    wxString getKeyFileDir();
    wxString getScreenDumpFile();
    wxString getPrintFile();
    ElfConfiguration getElfConfiguration(int computer);
    ElfConfiguration getElfConfiguration();
    void setElfConfiguration(ElfConfiguration elfConf);
    void setSerialPorts(wxString port);

    void setConfiguration(int computerType, Conf configuration) { conf[computerType] = configuration;};
    Conf getConfiguration(int computerType){ return conf[computerType];};
    void setElfConfiguration(int computerType, ElfConfiguration elfConf) { elfConfiguration[computerType] = elfConf;};
    int getMicroChipType(int computerType, int romType) {return conf[computerType].microChipType_[romType];};
    void setMicroChipType(int computerType, int romType, int chipType) { conf[computerType].microChipType_[romType] = chipType;};
    int getMicroChipLocation(int computerType, int romType) {return conf[computerType].microChipLocation_[romType];};
    void setMicroChipLocation(int computerType, int romType, int chipLocation) { conf[computerType].microChipLocation_[romType] = chipLocation;};
    int getMicroChipMemory(int computerType, int romType) {return conf[computerType].microChipMemory_[romType];};
    void setMicroChipMemory(int computerType, int romType, int memType) { conf[computerType].microChipMemory_[romType] = memType;};
    bool getMicroChipDisable(int computerType, int romType) {return conf[computerType].microChipDisable_[romType];};
    void setMicroChipDisable(int computerType, int romType, bool disableSocket) { conf[computerType].microChipDisable_[romType] = disableSocket;};
    wxString getMicroChipBlock(int computerType, int romType) {return conf[computerType].microChipBlock_[romType];};

    long getBitValue(wxString reference);
    long get8BitValue(wxString reference);
    long get12BitValue(wxString reference);
    long get16BitValue(wxString reference);

    void setClock(int computerType);
    void setBeepFrequency(int computerType);
    void setClockRate();
    void setPloadFileName(wxString fileName) {conf[runningComputer_].loadFileName_ = fileName;};
    void setPloadFileNameFull(wxString fileName) {conf[runningComputer_].loadFileNameFull_ = fileName;};

    int getConfigItem(wxString Item, long DefaultValue);
    void setConfigItem(wxString Item, int Value);
    void setConfigItem(wxString Item, long Value);
    wxString getConfigItem(wxString Item, wxString DefaultValue);
    void setConfigItem(wxString Item, wxString Value);
    bool getConfigBool(wxString Item, bool DefaultValue);
    void setConfigBool(wxString Item, bool Value);
    int getChoiceSelection(wxString info);
    void setSpinCtrl(wxString info, int value);
    void setTextCtrl(wxString info, wxString value);
    void setComboBox(wxString info, wxString value);
    void setCheckBox(wxString info, bool status);
//    int showMessageBox(wxString message, wxString caption, int style);
//    wxString showFileSelector(wxString message, wxString default_path, wxString default_filename, wxString default_extension, wxString wildcard, int flags, wxWindow *parent);
    void onHexKeyDef(wxCommandEvent&event);
    void onColoursDef(wxCommandEvent&event);

    void setScale(wxString scale);
    double getScale();
    double getZoom(int videoNumber);
    double getZoomVt();
    wxSize getMainSize(int computerType);
    void setMainSize(int computerType, wxSize size);
    wxSize get6845Size(int computerType);
    void set6845Size(int computerType, wxSize size);
    wxPoint getMainPos(int computerType);
    void setMainPos(int computerType, wxPoint position);
    wxPoint getPixiePos(int computerType);
    void setPixiePos(int computerType, wxPoint position);
    wxPoint getCdp1864Pos(int computerType);
    void setCdp1864Pos(int computerType, wxPoint position);
    wxPoint getVip2KPos(int computerType);
    void setVip2KPos(int computerType, wxPoint position);
    wxPoint getTmsPos(int computerType);
    void setTmsPos(int computerType, wxPoint position);
    wxPoint getVtPos(int computerType);
    void setVtPos(int computerType, wxPoint position);
    wxPoint getVtUart2Pos(int computerType);
    void setVtUart2Pos(int computerType, wxPoint position);
    wxPoint get6845Pos(int computerType);
    void set6845Pos(int computerType, wxPoint position);
    wxPoint get6847Pos(int computerType);
    void set6847Pos(int computerType, wxPoint position);
    wxPoint get8275Pos(int computerType);
    void set8275Pos(int computerType, wxPoint position);
    wxPoint getKeypadPos(int computerType);
    void setKeypadPos(int computerType, wxPoint position);
    wxPoint getSecondFramePos(int computerType);
    void setSecondFramePos(int computerType, wxPoint position);
    wxPoint getThirdFramePos(int computerType);
    void setThirdFramePos(int computerType, wxPoint position);
    wxPoint getFourthFramePos(int computerType);
    void setFourthFramePos(int computerType, wxPoint position);
    wxPoint getV1870Pos(int computerType);
    void setV1870Pos(int computerType, wxPoint position);
    wxPoint getSN76430NPos(int computerType);
    void setSN76430NPos(int computerType, wxPoint position);
    
    wxString getDataDir() {return dataDir_;};
    wxString getApplicationDir() {return applicationDirectory_;};
    void setDataDir(wxString dataDir) {dataDir_ = dataDir;};

    int pload();
    bool startCassetteLoad(int tapeNumber);
    bool startLoad(int tapeNumber);
    void stopCassette();
    void startCassetteSave(int tapeNumber);
    void startSaveNew(int tapeNumber);
    bool startSaveCont(int tapeNumber, wxString tapeCounterStr);
    bool startSave(int tapeNumber, wxString messageStr, bool cont);
    void onTerminalSave(wxCommandEvent&event);
    void onTerminalLoad(wxCommandEvent&event);
    void onTerminalStop(wxCommandEvent&event);
    void startAutoTerminalLoad(int protocol);
    void startTerminalLoad(int protocol);
    void stopAutoTerminal();
    void stopTerminal();
    void startAutoTerminalSave(int protocol);
    void startTerminalSave(int protocol);
    void startYsTerminalSave(int protocol);
    void turboOn();
    void turboOff();
    void enableStartButtonGui(bool status);
    void enableLocationGui();
    void enableMemAccessGui(bool status);
    void enableTapeGui(bool status, int computerType);
    void enableLoadGui(bool status);
    void setTapeState(int tapeState, wxString tapeNumber);
    void turboGui(wxString computerString);
    void setComputerInfo(int id, wxString gui, wxString name, wxString ploadExtension);
    void setScreenInfo(int id, int start, int end, wxString colour[]);
    void setScreenInfo(int id, int start, int end, wxString colour[], int numberVideo, int borderX[], int borderY[]);
    void setVtType(wxString elfTypeStr, int elfType, int Selection, bool GuiChange);
    void setBaudChoice(int computerType);
    void setRealCas(int computerType);
    void setRealCas2(int computerType);
    void setRealCasOff(int computerType);
    void setPrinterState(int computerType);
    void setBaud(int baudR, int baudT);
    void saveSetup(int baudR, int baudT, bitset<32> setupFeature, int vtCharactersPerRow, int charWidth);
    ScreenInfo getScreenInfo(int id);
    bool isFullScreenFloat() {return fullScreenFloat_;};
    void onFullScreenFloat(wxCommandEvent&event);
    void correctZoomAndValue(int computerType, wxString computerTypeString, bool setSpin, int videoNumber);
    void correctZoom(int computerType, wxString computerTypeString, bool setSpin, int videoNumber);
    void correctZoomVtAndValue(int computerType, wxString computerTypeString, bool setSpin);
    void correctZoomVt(int computerType, wxString computerTypeString, bool setSpin);
    void onLedTimer(wxCommandEvent&event);
    int getCpuType();
    int getCpuStartupRegisters() {return cpuStartupRegisters_;};
    int getCpuStartupRam() {return cpuStartupRam_;};
    int getCpuStartupVideoRam() {return cpuStartupVideoRam_;};
    bool isComputerRunning() {return computerRunning_;};
    bool useNumPad() {return useNumPad_;};
    bool checkWavFile(wxString fileName);

    bool showSoundError();
    void hideSoundError();
    bool showSplashScreen();
    void hideSplashScreen();
    
    bool repairIde();
    
    void onUpdDisk0(wxCommandEvent& event);
    void onUpdDiskText0(wxCommandEvent& event);
    void onUpdDiskEject0(wxCommandEvent& event);
    void onUpdDiskDirSwitch0(wxCommandEvent& event);
    void onUpdDisk1(wxCommandEvent& event);
    void onUpdDiskText1(wxCommandEvent& event);
    void onUpdDiskEject1(wxCommandEvent& event);
    void onUpdDiskDirSwitch1(wxCommandEvent& event);
    void onUpdDisk2(wxCommandEvent& event);
    void onUpdDiskText2(wxCommandEvent& event);
    void onUpdDiskEject2(wxCommandEvent& event);
    void onUpdDiskDirSwitch2(wxCommandEvent& event);
    void onUpdDisk3(wxCommandEvent& event);
    void onUpdDiskText3(wxCommandEvent& event);
    void onUpdDiskEject3(wxCommandEvent& event);
    void onUpdDiskDirSwitch3(wxCommandEvent& event);
    bool getDirectoryMode(int fdcType, int drive);
    void setDirectoryMode(int fdcType, int drive, bool state);
    wxString getUpdFloppyDirSwitched(int fdcType, int drive);
    wxString getUpdFloppyDir(int fdcType, int drive);
    wxString getUpdFloppyFile(int fdcType, int drive);
    void setUpdFloppyGui(int drive, int computerType);

    int getMessageBoxAnswer() {return messageBoxAnswer_;};
    wxColour getGuiTextColour(int colour) {return guiTextColour[colour];};

    int isDiagOn(int computer);
    int getDiagRomChecksum(int computer) {return conf[computer].diagRomChecksum_;};
    int getDiagFactory(int computer) {return conf[computer].diagFactory_;};
    void setDiagRomChecksum(int computer, int diagRomChecksum) { conf[computer].diagRomChecksum_ = diagRomChecksum;};
    void setDiagFactory(int computer, int diagFactory) { conf[computer].diagFactory_ = diagFactory;};
    int getDiagCassetteCables(int computer) {return conf[computer].diagCassetteCables_;};
    void setDiagCassetteCables(int computer, int diagCassetteCables) { conf[computer].diagCassetteCables_ = diagCassetteCables;};

    void onBatchConvertStart(wxCommandEvent&event);
    void batchConvertStop();
    void onBatchFileDialog(wxCommandEvent& event);
    wxArrayString getBatchPaths() {return batchPaths_;}
    wxString getBatchPath(int filenumber) {return batchPaths_[filenumber];}
    wxArrayString getBatchFiles() {return batchFiles_;}
    wxString getBatchFile(int filenumber) {return batchFiles_[filenumber];}
    size_t getNumberOfBatchFiles() {return numberOfBatchFiles_;}
    bool isBatchConvertActive() {return batchConvertActive_;};
    bool isTurboOn() {return turboOn_;};

    void downloadWavFiles(int computer);
    void checkWavFileDownload(int computer, bool downloadIfMissing = false);

protected:
    Mode mode_;

    Vip *p_Vip;
    VipII *p_Vip2;
    Vip2K *p_Vip2K;
    Velf *p_Velf;
    Cdp18s020 *p_Cdp18s020;
    Cdp18s600 *p_Cdp18s600;
    Cdp18s601 *p_Cdp18s601;
    Cdp18s602 *p_Cdp18s602;
    Cdp18s603a *p_Cdp18s603a;
    Cdp18s604b *p_Cdp18s604b;
    Rcasbc *p_Rcasbc;
    Nano *p_Nano;
    Tmc1800 *p_Tmc1800;
    Tmc2000 *p_Tmc2000;
    Studio2 *p_Studio2;
    CoinArcade *p_CoinArcade;
    Fred *p_Fred;
    Visicom *p_Visicom;
    Victory *p_Victory;
    StudioIV *p_StudioIV;
    Eti *p_Eti;
    Cidelsa *p_Cidelsa;
    Comx *p_Comx;
    Tmc600 *p_Tmc600;
    Pecom *p_Pecom;
    Elf2 *p_Elf2;
    Super *p_Super;
    Xmlemu *p_Xmlemu;
    Pico *p_Pico;
    Elf2K *p_Elf2K;
    Ms2000 *p_Ms2000;
    Mcds *p_Mcds;
    Cosmicos *p_Cosmicos;
    Membership *p_Membership;
    Uc1800 *p_Uc1800;
    Microtutor *p_Microtutor;
    Microtutor2 *p_Microtutor2;
    Elf *p_Elf;
    wxConfigBase *regPointer;
    wxMenu *configurationMenu;
    wxMenu *configurationDeleteMenu;
    
    bool configurationMenuOn_;
    size_t configurationMenuInfoNumber_;
    vector<ConfigurationMenuInfo> configurationMenuInfo_;
    
    ElfConfiguration elfConfiguration[LAST_ELF_TYPE+1];
    wxConfigBase *configPointer;
    Byte portExtender_;

    Conf conf[NO_COMPUTER];
    ComputerInfo computerInfo[NO_COMPUTER];
    ScreenInfo screenInfo[NO_COMPUTER];

    wxString comxPalClock_;
    wxString comxNtscClock_;
    wxString elf2KPixieClock_;
    wxString elf2K8275Clock_;

    wxString applicationDirectory_;
    wxString pathSeparator_;
    int mainWindowX_, mainWindowY_;
    int offsetX_;
    int offsetY_;
    int fontSize_;
    wxString fontSizeString_;

    bool guiInitialized_;
    int elfChoice_;
    int rcaChoice_;
    int debuggerChoice_;
    int studioChoice_;
    int telmacChoice_;
    int selectedComputer_;
    int runningComputer_;
    bool fullScreenFloat_;
    bool useNumPad_;
    int cpuType_;
    int defaultCpuType_;
    int cpuStartupRegisters_;
    int cpuStartupRam_;
    int cpuStartupVideoRam_;
    wxString keyboardType_;
    wxString keyboardTypeMenuItem_;

    bool useCtrlvKey_;
    bool useExitKey_;

    WindowInfo windowInfo;
    wxString workingDir_;
    wxString dataDir_;
    wxString iniDir_;
    int psaveData_[15];
    wxSize defaultGuiSize_;

    bool debugMode_;
    bool chip8DebugMode_;
    int zoomPosition_;
    int memoryDisplay_;

    int tapeState_;
    bool zoomEventOngoing_;
    bool fullScreenEventOngoing_;

    bool computerRunning_;

    wxStaticText *clockText[NO_COMPUTER];
    FloatEdit *clockTextCtrl[NO_COMPUTER];
    wxStaticText *mhzText[NO_COMPUTER];
    wxButton *startButton[NO_COMPUTER];
    wxButton *stopButton[NO_COMPUTER];

    wxTimer *tapePauseTimerPointer;
    wxTimer *traceTimeoutPointer;
    wxTimer *keyDebounceTimeoutPointer;
    wxTimer *vuPointer;
    wxTimer *directAssPointer;
    wxTimer *guiSizeTimeoutPointer;
    wxTimer *guiRedrawBarTimeOutPointer;
    bool guiSizeTimerStarted_;

    bool slotLedUpdate_;

    wxBitmap ejectBitmap;
 
    int messageBoxAnswer_;
    wxString fileSelectorAnswer_;
    bool fileSelectorCancel_;
    int messageAddressPopupAnswer_;

    wxSize clientSize_;
    bool sizeChanged_;
    
    bool terminalSave_;
    bool terminalLoad_;

    wxString floppyDirSwitched_[FDCTYPE_MAX][4];
    wxString floppyDir_[FDCTYPE_MAX][4];
    wxString floppy_[FDCTYPE_MAX][4];
    
    bool directoryMode_[FDCTYPE_MAX][4];

    int printX_;
    
    wxString warningText_;
    
    wxDateTime oldXmlDate_;
    wxString oldXmlFileName_;
    
    bool darkMode_;
    wxColour guiBackGround_;
    wxColour guiTextColour[GUI_COL_MAX];

    int hwTapeState_;
    wxBitmap rewindBlackBitmap;
    wxBitmap rewindGreenBitmap;
    wxBitmap rewindDarkGreenBitmap;
    wxBitmap forwardBlackBitmap;
    wxBitmap forwardGreenBitmap;
    wxBitmap forwardDarkGreenBitmap;
    wxBitmap playBlackBitmap;
    wxBitmap playDarkGreenBitmap;
    wxBitmap playGreenBitmap;
    wxBitmap recOffBitmap;
    wxBitmap recOnBitmap;
    wxBitmap recPressedBitmap;

private:
    wxBitmap realCasOffBitmap;
    wxBitmap realCasOnBitmap;
    wxBitmap printerOffBitmap;
    wxBitmap printerOnBitmap;
    wxBitmap pauseOffBitmap;
    wxBitmap pauseOnBitmap;

    double savedSpeed_;
    bool turboOn_;

    bool batchConvertActive_;
    wxString batchSaveWavFileDir_;
    wxString batchSaveWavFile_;

    wxArrayString batchPaths_;
    wxArrayString batchFiles_;
    size_t numberOfBatchFiles_;

    DECLARE_EVENT_TABLE()
};

#endif // GUIMAIN_H

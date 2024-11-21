#ifndef GUIMAIN_H
#define GUIMAIN_H

#include "wx/xml/xml.h"
#include "computerconfig.h"

#define NO_COMPUTER 1
#define DELETED_COMPUTERS 0

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

#define FDCTYPE_17XX 0
#define FDCTYPE_UPD765 1
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
#define GUI_COL_WHITE 9
#define GUI_COL_MAX 10

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

    wxString readConfigDir(const wxString& key, const wxString& defVal = wxEmptyString);
    void writeConfigDir(const wxString& key, const wxString& value);

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
    void onScreenDump(wxCommandEvent& event);
    void onVolume(wxScrollEvent&event);
    void onAutoLoad(wxCommandEvent& event);
    void onRealCas(wxCommandEvent& event);
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
    void onPsave(wxString fileName);
    void onDataSaveButton(wxCommandEvent& event);
    void onLoadRunButton(wxCommandEvent& event);
    void onLoadButton(wxCommandEvent& event);
    void onSaveStart(wxCommandEvent& event);
    void onSaveEnd(wxCommandEvent& event);
    void onSaveExec(wxCommandEvent& event);
    void getSoftware(wxString computer, wxString type, wxString software);
    void runSoftware(bool load);
    void onLoad(bool load);
    void onSaveButton(wxCommandEvent& event);
    void onClock(wxCommandEvent& event);
    void onBaudR(wxCommandEvent& event);
    void onBaudT(wxCommandEvent& event);
    void onClearRam(wxCommandEvent& event);
    void onClearRtc(wxCommandEvent& event);
    void onChoiceCpu(wxCommandEvent&event);

    bool getClearRamState() {return computerConfiguration.clearRam;};
    bool getClearRtcState() {return computerConfiguration.clearRtc;};
    void resetClearRamState();
    void resetClearRtcState();
    int getHwTapeState() {return hwTapeState_;};

    void onPsaveMenu(wxCommandEvent& event);
    void onVtSetup(wxCommandEvent& event);

    wxString convertLocale(wxString input);

    bool toDouble(wxString stringName, double* result);
    wxString getMainDir() {return computerConfiguration.mainDir_;};
    bool getGuiMode() {return mode_.gui;};

    wxString getIdeDir() {return computerConfiguration.ideFileConfiguration.directory;};
    wxString getCharRomDir() {return computerConfiguration.characterRomConfiguration.directory;};
    wxString getVtCharRomDir() {return computerConfiguration.videoTerminalConfiguration.vtCharRomDirectory;};
    void setWaveDir(wxString fileDir) {computerConfiguration.wavConfiguration[0].directory = fileDir;};
    wxString getTerminalPath(int filenumber) {return computerConfiguration.videoTerminalConfiguration.terminalPaths[filenumber];}
    wxArrayString getTerminalFiles() {return computerConfiguration.videoTerminalConfiguration.terminalFiles;}
    size_t getNumberOfTerminalFiles() {return computerConfiguration.videoTerminalConfiguration.numberOfTerminalFiles;}
    int getIoMask(size_t number) {return computerConfiguration.ioMask[number];};

    wxString getSplashDialog() {return computerConfiguration.splashConfiguration.dialog;};
    wxString getSplashText() {return computerConfiguration.splashConfiguration.text;};

    wxString getSelectedComputerText() {return computerInfo.name;};
    wxString getRunningComputerText() {return computerInfo.name;};
    wxString getRunningComputerPloadExtension() {return computerInfo.ploadExtension;};
    int getVolume() {return computerConfiguration.soundConfiguration.volume;};
    bool getInterlace() {return computerConfiguration.interlace_;};
    bool getUseLoadLocation() {return computerConfiguration.memAccessConfiguration.useLocation;};
    wxString getSaveStartString() {return computerConfiguration.memAccessConfiguration.saveStartString;}; 
    wxString getSaveEndString() {return computerConfiguration.memAccessConfiguration.saveEndString;}; 
    wxString getSaveExecString() {return computerConfiguration.memAccessConfiguration.saveExecString;}; 
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

    wxString getIdeFile() {return computerConfiguration.ideFileConfiguration.fileName;};
    wxString getCharRomFile() {return computerConfiguration.characterRomConfiguration.fileName;};
    wxString getVtCharRomFile() {return computerConfiguration.videoTerminalConfiguration.vtCharRomFileName;};
    wxString getXmodemFile() {return computerConfiguration.videoTerminalConfiguration.xmodemFileName;};
    wxString getXmodemFileFullStr() {return computerConfiguration.videoTerminalConfiguration.xmodemFullFileName;};
    void setWaveFile(wxString fileName) {computerConfiguration.wavConfiguration[0].fileName = fileName;};
    wxString getWaveFile(int number) {return computerConfiguration.wavConfiguration[number].fileName;};
    wxString getKeyFile();
    wxString getKeyFileDir();
    wxString getScreenDumpFile();
    wxString getPrintFile();

    void setConfiguration(ComputerConfiguration computerConfig) { computerConfiguration = computerConfig;};
    ComputerConfiguration getConfiguration(){ return computerConfiguration;};

    long getBitValue(wxString reference);
    long get8BitValue(wxString reference);
    long get12BitValue(wxString reference);
    long get16BitValue(wxString reference);

    void setClock();
    void setClockRate();
    void setPloadFileName(wxString fileName) {computerConfiguration.memAccessConfiguration.fileName = fileName;};
    void setPloadFullFileName(wxString fileName) {computerConfiguration.memAccessConfiguration.fullFileName = fileName;};

    int getConfigItem(wxString Item, long DefaultValue);
    void setConfigItem(wxString Item, int Value);
    void setConfigItem(wxString Item, long Value);
    wxString getConfigItem(wxString Item, wxString DefaultValue);
    void setConfigItem(wxString Item, wxString Value);
    bool getConfigBool(wxString Item, bool DefaultValue);
    void setConfigBool(wxString Item, bool Value);
    int getChoiceSelection(wxString info);
    void setTextCtrl(wxString info, wxString value);
    void setCheckBox(wxString info, bool status);
    void onHexKeyDef(wxCommandEvent&event);

    double getZoom(int videoNumber);
    double getZoomVt();
    wxPoint getMainPos();
    void setMainPos(wxPoint position);
    wxPoint getCoinPos();
    void setCoinPos(wxPoint position);
    wxPoint getPixiePos();
    void setPixiePos(wxPoint position);
    wxPoint getCdp1864Pos();
    void setCdp1864Pos(wxPoint position);
    wxPoint getSt4Pos();
    void setSt4Pos(wxPoint position);
    wxPoint getVip2KPos();
    void setVip2KPos(wxPoint position);
    wxPoint getFredPos();
    void setFredPos(wxPoint position);
    wxPoint getTmsPos();
    void setTmsPos(wxPoint position);
    wxPoint getVtPos();
    void setVtPos(wxPoint position);
    wxPoint get6845Pos();
    void set6845Pos(wxPoint position);
    wxPoint get6847Pos();
    void set6847Pos(wxPoint position);
    wxPoint get8275Pos();
    void set8275Pos(wxPoint position);
    wxPoint getV1870Pos();
    void setV1870Pos(wxPoint position);
    wxPoint getSN76430NPos();
    void setSN76430NPos(wxPoint position);
    void setCdp1851Pos(wxPoint position, int number);
    void setCdp1852Pos(wxPoint position, int number);

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
    void startYsTerminalSave(int protocol);
    void turboOn();
    void turboOff();
    void enableStartButtonGui(bool status);
    void enableLocationGui();
    void enableMemAccessGui(bool status);
    void enableTapeGui(bool status);
    void enableLoadGui(bool status);
    void setTapeState(int tapeState, wxString tapeNumber);
    void turboGui();
    void setComputerInfo(wxString name, wxString ploadExtension);
    void setScreenInfo(int start, int end, wxString colour[]);
    void setScreenInfo(int start, int end, wxString colour[], int numberVideo, int borderX[], int borderY[]);
    void setVtType(int Selection, bool GuiChange);
    void setBaudChoice();
    void setRealCas();
    void setRealCas2();
    void setRealCasOff();
    void setBaud(int baudR, int baudT);
    void saveSetup(int baudR, int baudT, bitset<32> setupFeature, int charactersPerRow, int charWidth);
    ScreenInfo getScreenInfo();
    bool isFullScreenFloat() {return fullScreenFloat_;};
    void onFullScreenFloat(wxCommandEvent&event);
    void correctZoomAndValue(bool setSpin, int videoNumber);
    void correctZoom(bool setSpin, int videoNumber);
    void correctZoomVtAndValue(bool setSpin);
    void correctZoomVt(bool setSpin);
    int getCpuType();
    void setCpuType(wxString type);
    bool isXmlRomRamOptionGui() {return XmlRomRamOptionGui_;};
    int getCpuStartupRegisters() {return cpuStartupRegisters_;};
    int getCpuStartupRam() {return cpuStartupRam_;};
    int getCpuStartupVideoRam() {return cpuStartupVideoRam_;};
    bool isComputerRunning() {return computerRunning_;};
    bool useNumPad() {return useNumPad_;};
    bool checkWavFile(wxString fileName);

    bool showSoundError();
    void hideSoundError();
        
    bool getDirectoryMode(int fdcType, int drive);
    void setDirectoryMode(int fdcType, int drive, bool state);
    wxString getUpdFloppyDirSwitched(int fdcType, int drive);
    wxString getUpdFloppyDir(int fdcType, int drive);
    wxString getUpdFloppyFile(int fdcType, int drive);
    void setUpdFloppyGui(int drive);

    int getMessageBoxAnswer() {return messageBoxAnswer_;};
    wxColour getGuiTextColour(int colour) {return guiTextColour[colour];};

    bool isDiagOn();

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

    bool isTapeHwFred() {return (computerConfiguration.hwTapeConfiguration.format != TAPE_FORMAT_CV && computerConfiguration.hwTapeConfiguration.defined);}
    bool isTapeHwCybervision() {return (computerConfiguration.hwTapeConfiguration.format == TAPE_FORMAT_CV && computerConfiguration.hwTapeConfiguration.defined);}

    void downloadWavFiles();
    void checkWavFileDownload(bool downloadIfMissing = false);

protected:
    Mode mode_;

    wxConfigBase *regPointer;
    wxMenu *configurationMenu;
    wxMenu *configurationDeleteMenu;
    
    bool configurationMenuOn_;
    size_t configurationMenuInfoNumber_;
    vector<ConfigurationMenuInfo> configurationMenuInfo_;
    
    wxConfigBase *configPointer;
    Byte portExtender_;

    ComputerConfiguration computerConfiguration;
    ComputerInfo computerInfo;
    ScreenInfo screenInfo;

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
    int debuggerChoice_;
    int selectedTab_;
    bool fullScreenFloat_;
    bool useNumPad_;
    int cpuType_;
    int cpuTypeXml_;
    int defaultCpuType_;
    int cpuStartupRegisters_;
    int cpuStartupRam_;
    int cpuStartupVideoRam_;
    wxString keyboardType_;
    wxString keyboardTypeMenuItem_;
    
    bool XmlRomRamOptionGui_;

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

    wxStaticText *clockText;
    FloatEdit *clockTextCtrl;
    wxStaticText *mhzText;
    wxButton *startButton;
    wxButton *stopButton;

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

    bool turboOn_;

private:
    wxBitmap realCasOffBitmap;
    wxBitmap realCasOnBitmap;
    wxBitmap printerOffBitmap;
    wxBitmap printerOnBitmap;
    wxBitmap pauseOffBitmap;
    wxBitmap pauseOnBitmap;

    double savedSpeed_;

    bool batchConvertActive_;
    wxString batchSaveWavFileDir_;
    wxString batchSaveWavFile_;

    wxArrayString batchPaths_;
    wxArrayString batchFiles_;
    size_t numberOfBatchFiles_;

    DECLARE_EVENT_TABLE()
};

#endif // GUIMAIN_H

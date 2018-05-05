#ifndef GUIMAIN_H
#define GUIMAIN_H

#define NO_COMPUTER 26
#define LAST_ELF_TYPE 10

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

#define MAXROM 6

DECLARE_EVENT_TYPE(OPEN_PRINTER_WINDOW, 811) 

class ConfigurationMenuInfo
{
public:
    wxString fileName;
    wxString menuName;
    wxString subMenuName;
};

class ConfigurationSubMenuInfo
{
public:
    wxString subMenuName;
    wxMenu *subMenu;
    wxMenu *deleteSubMenu;
};

class Conf
{
public:
	wxString mainDir_;

	wxString romDir_[MAXROM];
	wxString rom_[MAXROM];
	wxString ramDir_;
	wxString ram_;
	wxString chip8SWDir_;
	wxString chip8SW_;
	wxString wavFileDir_;
	wxString wavFile_;
	wxString charRomDir_;
	wxString charRom_;
	wxString vtCharRomDir_;
	wxString vtCharRom_;
	wxString ideDir_;
	wxString ide_;
    wxString keyFileDir_;
	wxString keyFile_;
	wxString screenDumpFileDir_;
	wxString screenDumpFile_;
	wxString printFileDir_;
	wxString printFile_;
    wxString configurationDir_;
    
    ConfigurationInfo configurationInfo_;
    
    wxMenu *configurationMenu;
    wxMenu *configurationDeleteMenu;
    
    size_t configurationSubMenuInfoNumber_;
    vector<ConfigurationSubMenuInfo> configurationSubMenuInfo_;

	int printMode_;

	wxString loadFileName_;
	wxString loadFileNameFull_;

	char pLoadSaveName_[4];
	Word defus_;
	Word eop_;
	Word string_;
	Word arrayValue_;
	Word eod_;
	Word basicRamAddress_;

	long bootAddress_;

	int volume_;
	int tempo_;
	int soundType_;
	double clockSpeed_;
	int beepFrequency_;
	wxString ledTime_;
	long ledTimeMs_;
	wxString clock_;
	wxString zoom_;
	wxString xScale_;
	wxString zoomVt_;
	int fdcCpms_;
    int videoMode_;
    int velfMode_;
	int ramType_;
	bool useKeyboard_;

	bool printerOn_;
	bool useLoadLocation_;
	bool autoCassetteLoad_;
	bool turbo_;
	bool interlace_;
	bool sbActive_;
	bool diagActive_;
	int diagOn_;
	bool videoLog_;
	bool stretchDot_;
	bool realCassetteLoad_;
	wxString turboClock_;

    bool multiCart_;
	bool disableSystemRom_;
	Byte lsb_;
    Byte msb_;

    int pixieX_, pixieY_;
	int tmsX_, tmsY_;
	int vtX_, vtY_;
	int mc6845X_, mc6845Y_;
	int mc6847X_, mc6847Y_;
	int i8275X_, i8275Y_;
	int mainX_, mainY_;
	int keypadX_, keypadY_;
	
	int sizeX_;
	int sizeY_;
	int sizeNTSCX_;
	int sizeNTSCY_;
	int sizeX6845_;
	int sizeY6845_;

	Word saveStart_;
	Word saveEnd_;
	Word saveExec_;

	wxString saveStartString_;
	wxString saveEndString_;
	wxString saveExecString_;
};

class ComputerInfo
{
public:
	wxString gui;
	wxString name;
//	wxString extension;
	wxString ploadExtension;
};

class GuiMain : public wxFrame
{
public:

	GuiMain(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiMain() {};

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
	void onVtCharRom(wxCommandEvent& event);
	void onVtCharRomText(wxCommandEvent& event);
	void onKeyFile(wxCommandEvent& event);
	void onKeyFileText(wxCommandEvent& event);
	void onKeyFileEject(wxCommandEvent& event);
    void onVT100(wxCommandEvent& event);
	void onZoomUp(wxSpinEvent& event);
	void onZoomDown(wxSpinEvent& event);
	void onZoomValue(wxCommandEvent& event);
	void changeZoom(double zoom);
	void onZoomUpVt(wxSpinEvent& event);
	void onZoomDownVt(wxSpinEvent& event);
	void onZoomValueVt(wxCommandEvent& event);
	void changeZoomVt(double zoom);
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
	void onTerminalFile(wxCommandEvent& event);
	void onCassetteEject(wxCommandEvent& event);
	void onCassetteText(wxCommandEvent& event);
	void onAutoLoad(wxCommandEvent& event);
	void onRealCas(wxCommandEvent& event);
	void onWavFile(wxCommandEvent& event);
	void onTurbo(wxCommandEvent&event);
	void onTurboClock(wxCommandEvent& event);
	void onUseLocation(wxCommandEvent& event);
	void onCassetteLoad(wxCommandEvent& event);
	void onCassetteSave(wxCommandEvent& event);
	void onCassetteStop(wxCommandEvent& event);
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
	void onBootAddress(wxCommandEvent& event);
	long getBootAddress(wxString computerTypeStr, int computerType);
	void onChoiceRam(wxCommandEvent&event);

	void onPsaveMenu(wxCommandEvent& event);
	void onVtSetup(wxCommandEvent& event);
	void onBeepFrequency(wxCommandEvent&event);

    wxString getMainDir() {return conf[runningComputer_].mainDir_;};
	bool getGuiMode() {return mode_.gui;};

	wxString getRomDir(int computerType, int romType) {return conf[computerType].romDir_[romType];};
	wxString getRamDir(int computerType) {return conf[computerType].ramDir_;};
	wxString getChip8Dir(int computerType){return conf[computerType].chip8SWDir_;};
	wxString getIdeDir(int computerType) {return conf[computerType].ideDir_;};
	wxString getCharRomDir(int computerType) {return conf[computerType].charRomDir_;};
	wxString getVtCharRomDir(int computerType) {return conf[computerType].vtCharRomDir_;};
	wxString getWaveDir(int computerType) {return conf[computerType].wavFileDir_;};
	bool getAutCassetteLoad() {return conf[runningComputer_].autoCassetteLoad_;};
	bool getPrinterStatus(int computerType) {return conf[computerType].printerOn_;};

	wxString getSelectedComputerStr() {return computerInfo[selectedComputer_].gui;};
	wxString getRunningComputerStr() {return computerInfo[runningComputer_].gui;};
	wxString getSelectedComputerText() {return computerInfo[selectedComputer_].name;};
	int getSelectedComputerId() {return selectedComputer_;};
	int getRunningComputerId() {return runningComputer_;};
	int getVideoMode(int computerType) {return conf[computerType].videoMode_;};
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
    
	wxString getLedTime(int computerType) {return conf[computerType].ledTime_;}; 
	long getLedTimeMs(int computerType) {return conf[computerType].ledTimeMs_;}; 

	wxString getRomFile(int computerType, int romType) {return conf[computerType].rom_[romType];};
	wxString getRamFile(int computerType) {return conf[computerType].ram_;};
	wxString getChip8SW(int computerType){return conf[computerType].chip8SW_;};
	wxString getIdeFile(int computerType) {return conf[computerType].ide_;};
	wxString getCharRomFile(int computerType) {return conf[computerType].charRom_;};
	wxString getVtCharRomFile(int computerType) {return conf[computerType].vtCharRom_;};
	wxString getWaveFile(int computerType) {return conf[computerType].wavFile_;};
	wxString getKeyFile();
	wxString getKeyFileDir();
	wxString getScreenDumpFile();
	wxString getPrintFile();
	ElfConfiguration getElfConfiguration(int computer);
    ElfConfiguration getElfConfiguration();
    void setElfConfiguration(ElfConfiguration elfConf);
    void setSerialPorts(wxString port);

	long getBitValue(wxString reference);
	long get8BitValue(wxString reference);
	long get12BitValue(wxString reference);
	long get16BitValue(wxString reference);

	void setClock(int computerType);
	void setBeepFrequency(int computerType);
	void setClockRate();
	void setPloadFileName(wxString fileName) {conf[runningComputer_].loadFileName_ = fileName;};

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
//	int showMessageBox(wxString message, wxString caption, int style);
//	wxString showFileSelector(wxString message, wxString default_path, wxString default_filename, wxString default_extension, wxString wildcard, int flags, wxWindow *parent);
	void onHexKeyDef(wxCommandEvent&event);
	void onColoursDef(wxCommandEvent&event);

	void setScale(wxString scale);
	double getScale();
	double getZoom();
	double getZoomVt();
	wxSize getMainSize(int computerType);
	void setMainSize(int computerType, wxSize size);
	wxSize get6845Size(int computerType);
	void set6845Size(int computerType, wxSize size);
	wxPoint getMainPos(int computerType);
	void setMainPos(int computerType, wxPoint position);
	wxPoint getPixiePos(int computerType);
	void setPixiePos(int computerType, wxPoint position);
	wxPoint getTmsPos(int computerType);
	void setTmsPos(int computerType, wxPoint position);
	wxPoint getVtPos(int computerType);
	void setVtPos(int computerType, wxPoint position);
	wxPoint get6845Pos(int computerType);
	void set6845Pos(int computerType, wxPoint position);
	wxPoint get6847Pos(int computerType);
	void set6847Pos(int computerType, wxPoint position);
	wxPoint get8275Pos(int computerType);
	void set8275Pos(int computerType, wxPoint position);
	wxPoint getKeypadPos(int computerType);
	void setKeypadPos(int computerType, wxPoint position);

	wxString getDataDir() {return dataDir_;};
	wxString getApplicationDir() {return applicationDirectory_;};
	void setDataDir(wxString dataDir) {dataDir_ = dataDir;};

	int pload();
	void startCassetteLoad();
	void startLoad();
	void stopCassette();
	void startCassetteSave();
    void startSave();
	void onTerminalSave(wxCommandEvent&event);
	void onTerminalLoad(wxCommandEvent&event);
	void onTerminalStop(wxCommandEvent&event);
    void startAutoTerminalLoad();
    void startTerminalLoad();
    void stopAutoTerminal();
    void stopTerminal();
    void startAutoTerminalSave();
    void startTerminalSave();
	void turboOn();
	void turboOff();
	void enableStartButtonGui(bool status);
	void enableLocationGui();
	void enableMemAccessGui(bool status);
	void enableTapeGui(bool status, int computerType);
	void enableLoadGui(bool status);
	void setTapeState(int tapeState);
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
	ScreenInfo getScreenInfo(int id);
	bool isFullScreenFloat() {return fullScreenFloat_;};
	void onFullScreenFloat(wxCommandEvent&event);
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
    
protected:
	Mode mode_;

	Vip *p_Vip;
	VipII *p_Vip2;
    Velf *p_Velf;
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
    Elf2K *p_Elf2K;
	Ms2000 *p_Ms2000;
	Mcds *p_Mcds;
	Cosmicos *p_Cosmicos;
	Membership *p_Membership;
	Microtutor *p_Microtutor;
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
	int ubuntuOffsetX_;

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

	bool useExitKey_;

	WindowInfo windowInfo;
	wxString workingDir_;
	wxString dataDir_;
	wxString iniDir_;
	int psaveData_[LAST_ELF_TYPE+1];
    wxSize defaultGuiSize_;

	bool debugMode_;
	bool chip8DebugMode_;
	bool zoomTextValueChanged_;
	int memoryDisplay_;

	int tapeState_;

	bool computerRunning_;

	wxStaticText *clockText[NO_COMPUTER];
	FloatEdit *clockTextCtrl[NO_COMPUTER];
	wxStaticText *mhzText[NO_COMPUTER];
    wxButton *startButton[NO_COMPUTER];
    wxButton *stopButton[NO_COMPUTER];

    wxTimer *traceTimeoutPointer;
    wxTimer *keyDebounceTimeoutPointer;
    wxTimer *vuPointer;

	bool slotLedUpdate_;

    wxBitmap ejectBitmap;
 
	int messageBoxAnswer_;
	wxString fileSelectorAnswer_;
	int messageAddressPopupAnswer_;

    bool terminalSave_;
    bool terminalLoad_;

private:
	wxBitmap playBlackBitmap;
	wxBitmap playGreenBitmap;
	wxBitmap recOffBitmap;
	wxBitmap recOnBitmap;
	wxBitmap realCasOffBitmap;
	wxBitmap realCasOnBitmap;
	wxBitmap printerOffBitmap;
    wxBitmap printerOnBitmap;

    wxPoint position_[LAST_ELF_TYPE+1];

	double savedSpeed_;
	bool turboOn_;

	DECLARE_EVENT_TABLE()
};

#endif // GUIMAIN_H

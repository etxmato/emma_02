#ifndef COMP_H
#define COMP_H

#include "til311.h"
#include "til313.h"
#include "til313.full.h"
#include "led.h"

#define ELF_HEX_BUTTON 0
#define COSMICOS_HEX_BUTTON 1
#define PANEL_HEX_BUTTON 2
#define PANEL_WIDE_HEX_BUTTON 3
#define ELF2K_RESET_BUTTON 4

#define VERTICAL_BUTTON 0
#define HORIZONTAL_BUTTON 1
#define ELF2K_POWER_BUTTON 2
#define PUSH_BUTTON_BLACK 3
#define VERTICAL_BUTTON_RED 4
#define PUSH_BUTTON 5
#define ELF2K_LOAD_BUTTON 6
#define ELF2K_MP_BUTTON 7
#define ELF2K_RUN_BUTTON 8
#define ELF2K_IN_BUTTON 9

#define BUTTON_UP true
#define BUTTON_DOWN false
#define BUTTON_RIGHT true
#define BUTTON_LEFT false
#define BASICADDR_MAX 4

wxDEFINE_EVENT(COMPUTER_MSG, guiEvent);

#define guiEventHandler(func) (&func)

#define EVT_COMPUTER_MSG(id, func) \
wx__DECLARE_EVT1(COMPUTER_MSG, id, (&func))


class HexButton : public wxEvtHandler

{
public:
    HexButton(wxDC& dc, int type, wxCoord x, wxCoord y, wxString label);
    ~HexButton();
    void onPaint(wxDC& dc);
    bool onMousePress(wxDC& dc, wxCoord x, wxCoord y);
    bool onMouseRelease(wxDC& dc, wxCoord x, wxCoord y);
	void OnTimer(wxTimerEvent& event);
	void releaseButtonOnScreen(wxDC& dc);

private:
    wxBitmap *upBitmapPointer;
    wxBitmap *downBitmapPointer;
    
    wxCoord x_;
    wxCoord y_;
    bool state_;
    
    int buttonType_;
    wxTimer *focusTimer;

    DECLARE_EVENT_TABLE()
};

class SwitchButton
{
public:
    SwitchButton(wxDC& dc, int type, wxColour bkgrClr, bool state, wxCoord x, wxCoord y, wxString label);
    ~SwitchButton();
    void onPaint(wxDC& dc);
    bool onMousePress(wxDC& dc, wxCoord x, wxCoord y);
    bool onMouseRelease(wxDC& dc, wxCoord x, wxCoord y);
    void setState(wxDC& dc, bool state);
    
private:
    wxBitmap *upBitmapPointer;
    wxBitmap *downBitmapPointer;
    
    wxMask *maskUp;
    wxMask *maskDown;
    
    wxCoord x_;
    wxCoord y_;
    bool state_;
    int type_;
};

class RunComputer : public wxThread
{
public:
	RunComputer() {};
	virtual void *Entry();
};

class Panel : public wxWindow
{
public:
	Panel(wxWindow *parent, const wxSize& size);
	virtual ~Panel();

	virtual void init();
	virtual void init(int computerType);
	void connectKeyEvent(wxWindow* pclComponent);
	virtual void onPaint(wxPaintEvent&event);
	void onChar(wxKeyEvent&event);
	void vtOut(int value);
	void onKeyDown(wxKeyEvent&event);
	void onKeyUp(wxKeyEvent&event);
	Byte getKey(Byte vtOut);
	virtual void onMousePress(wxMouseEvent& event);
	virtual void onMouseRelease(wxMouseEvent& event);
	void ledTimeout();
	void setLedMs(long ms);

    void setReadyLed(int status);
    void updateReadyLed(wxDC& dc);
    void setStopLed(int status);
    void updateStopLed(wxDC& dc);
    void setErrorLed(int status);
    void updateErrorLed(wxDC& dc);
	void setQLed(int status);
	void updateQLed(wxDC& dc);
	void setResetLed(int status);
	void updateResetLed(wxDC& dc);
	void setPauseLed(int status);
	void updatePauseLed(wxDC& dc);
	void setRunLed(int status);
	void updateRunLed(wxDC& dc);
	void setLoadLed(int status);
	void updateLoadLed(wxDC& dc);
	void setLed(int i, int status);
	void updateLed(wxDC& dc, int i);
	void showData(Byte value);
	void updateData(wxDC& dc);
	void showDataTil313(Byte value);
	void updateDataTil313(wxDC& dc);
	void showSeg(int number, Byte value);
	void updateSeg(wxDC& dc, int number);
    void showAddress(Word address);
    void showAddressTil313(Word address);
    void updateAddress(wxDC& dc);
    void updateAddressTil313(wxDC& dc);
	virtual void inUp();
	virtual void inDown();
    virtual void inSetState(bool state);
    virtual void clearSetState(bool state);
    virtual void waitSetState(bool state);
    virtual void runUp();
    virtual void runDown();
	virtual void runSetState(bool state);
	virtual void mpUp();
	virtual void mpDown();
	virtual void mpSetState(bool state);
	void powerUp();
	void powerDown();
	virtual void loadUp();
	virtual void loadDown();
	virtual void loadSetState(bool state);
	void dataUp(int number);
	void dataDown(int number);
	virtual void dataSetState(int number, bool state);
	void efUp(int number);
	void efDown(int number);

protected:
	wxBitmap *mainBitmapPointer;

#if wxCHECK_VERSION(2, 9, 0)
	wxButton *inButtonPointer;
	wxButton *resetButtonPointer;
	wxButton *runButtonPointer;
	wxButton *mpButtonPointer;
	wxButton *powerButtonPointer;
	wxButton *loadButtonPointer;
	wxButton *ramButtonPointer;
	wxButton *dataSwitchPointer[8];
	wxButton *efSwitchPointer[4];
#else
	wxBitmapButton *inButtonPointer;
	wxBitmapButton *resetButtonPointer;
	wxBitmapButton *runButtonPointer;
	wxBitmapButton *mpButtonPointer;
	wxBitmapButton *powerButtonPointer;
	wxBitmapButton *loadButtonPointer;
	wxBitmapButton *ramButtonPointer;
	wxBitmapButton *dataSwitchPointer[8];
	wxBitmapButton *efSwitchPointer[4];
#endif
	wxButton *buttonPointer[16];
	wxButton *pauseButtonPointer;
	wxButton *stepButtonPointer;
	wxButton *monitorButtonPointer;
	wxButton *clearButtonPointer;
	wxButton *enterButtonPointer;
	wxButton *reqButtonPointer;
	wxButton *seqButtonPointer;
	wxButton *decButtonPointer;
	wxButton *retButtonPointer;
	wxButton *push_inButtonPointer;
    wxButton *text_readButtonPointer;
    wxButton *text_resetButtonPointer;
	wxButton *text_loadButtonPointer;
	wxButton *text_runButtonPointer;
	wxButton *text_mpButtonPointer;
	wxButton *text_dataSwitchPointer[8];

	HexButton *osx_pauseButtonPointer;
	HexButton *osx_monitorButtonPointer;
	HexButton *osx_stepButtonPointer;
	HexButton *osx_push_inButtonPointer;
    HexButton *osx_text_readButtonPointer;
    HexButton *osx_text_resetButtonPointer;
    HexButton *osx_text_loadButtonPointer;
    HexButton *osx_text_runButtonPointer;
    HexButton *osx_text_mpButtonPointer;
    HexButton *osx_text_dataSwitchPointer[8];
	HexButton *osx_buttonPointer[16];
    HexButton *osx_clearButtonPointer;
    HexButton *osx_enterButtonPointer;
	HexButton *osx_reqButtonPointer;
	HexButton *osx_seqButtonPointer;
	HexButton *osx_decButtonPointer;
	HexButton *osx_retButtonPointer;

    SwitchButton *powerSwitchButton;
    SwitchButton *resetSwitchButton;
    SwitchButton *runSwitchButton;
	SwitchButton *loadSwitchButton;
	SwitchButton *mpSwitchButton;
	SwitchButton *ramSwitchButton;
    SwitchButton *inSwitchButton;
    SwitchButton *readSwitchButton;
    SwitchButton *cardSwitchButton;
    SwitchButton *clearSwitchButton;
    SwitchButton *waitSwitchButton;
    SwitchButton *velfSwitchButton;
	SwitchButton *dataSwitchButton[8];
	SwitchButton *efSwitchButton[4];

    wxBitmap *hexButtonPointer;
    wxBitmap *upBitmapPointer;
	wxBitmap *downBitmapPointer;
	wxBitmap *pushUpBitmapPointer;
	wxBitmap *pushDownBitmapPointer;

	wxMask *maskUp;
	wxMask *maskDown;

	Til311 *addressPointer[4];
	Til311 *dataPointer[2];
    Til313 *addressTil313Pointer[4];
	Til313 *dataTil313Pointer[2];
	Til313full *segPointer[8];

    int tilType_;
    
	Word addressStatus;
	Byte dataStatus;
	Byte dataTil313Status;
	Byte segStatus[8];

    Led *readyLedPointer;
    Led *stopLedPointer;
    Led *errorLedPointer;
	Led *qLedPointer;
	Led *resetLedPointer;
	Led *pauseLedPointer;
	Led *runLedPointer;
	Led *loadLedPointer;
    Led *ledPointer[8];

    int readyLedStatus;
    int stopLedStatus;
    int errorLedStatus;
	int qLedStatus;
	int resetLedStatus;
	int pauseLedStatus;
	int runLedStatus;
	int loadLedStatus;
	int ledStatus[8];

    bool updateReadyLed_;
    bool updateStopLed_;
    bool updateErrorLed_;
	bool updateQLed_;
	bool updateResetLed_;
	bool updatePauseLed_;
	bool updateRunLed_;
	bool updateLoadLed_;
	bool updateLed_[8];
    bool updateAddress_;
    bool updateAddressTil313_;
	bool updateData_;
	bool updateDataTil313_;
	bool updateSeg_[8];

	long ms_;

	char keyBuffer_[26];
	int  keyStart_;
	int  keyEnd_;
	int lastKey_;
	bool repeat_;
	bool forceUpperCase_;

private:
	bool functionKeyReleaseTwo_;

	DECLARE_EVENT_TABLE()
};

class Computer
{
public:
	Computer();
	~Computer() {};

	virtual bool isComputerRunning();
	virtual void startComputerRun(bool load);
	virtual void onReset();
	virtual void onRun();
    virtual void keyDown(int keycode);
	virtual bool keyDownExtended(int keycode, wxKeyEvent& event);
	virtual bool keyDownPressed(int keycode);
	virtual void keyUp(int keycode);
	virtual void keyUpExtended(int keycode, wxKeyEvent& event);
	virtual bool keyUpReleased(int keycode);
	virtual void charEvent(int keycode);
	virtual void onButtonRelease(wxCommandEvent& event);
	virtual void onButtonPress(wxCommandEvent& event);
	virtual void cid1Bit8(bool set);
	virtual int getComxExpansionType(int card);
    virtual void sleepComputer(long ms);
    virtual void pauseComputer();
    virtual void resumeComputer();
	virtual void checkCaps();
	virtual void finishStopTape();
	virtual void printOutPecom(int q);
	virtual void setElf2KDivider(Byte value);
	virtual void removeElf2KSwitch();
	virtual void removeElfHex();
	virtual void removeCosmicosHex();
	virtual void removeElfLedModule(); 
	virtual void showData(Byte val);
	virtual void resetVideo();
	virtual void resetComputer();
	virtual void clearBootstrap();
	virtual void onInButtonRelease();
    virtual void onElf2KInButton() {};
	virtual void onInButtonPress();
	virtual void onInButtonPress(Byte value);
	virtual void realCassette(short val);
	virtual void cassette(short val);
	virtual void cassette(char val);
	virtual void cassetteFred(short val);
	virtual void cassetteFred(char val);
	virtual void keyClear();
	virtual void startComputer();
	virtual void initComputer();
	virtual void configureComputer();
    virtual void onReadButton();
    virtual void onCardButton();
    virtual void onRunButton();
    virtual void onRunButtonPress() {};
    virtual void onRunButtonRelease() {};
    virtual void onRunButton(wxCommandEvent& event);
	virtual void onPause();
	virtual void onPause(wxCommandEvent& event);
	virtual void onMonitor();
	virtual void onMonitor(wxCommandEvent& event);
	virtual void onSingleStep();
	virtual void onSingleStep(wxCommandEvent& event);
    virtual void onMpButton();
    virtual void onMpButton(wxCommandEvent& event);
    virtual void onWaitButton();
    virtual void onClearButton();
    virtual void onClearButtonPress() {};
    virtual void onClearButtonRelease() {};
    virtual void onClearButton(wxCommandEvent& event);
	virtual void onResetButton();
    virtual void onResetButton(wxCommandEvent& event);
    virtual void onLoadButton();
    virtual void onLoadButton(wxCommandEvent& event);
    virtual void dataButton(int i);
	virtual void onNumberKeyRelease(int i);
    virtual void onNumberKeyPress(int i);
    virtual void onNumberKeyDown(int i);
	virtual void onRamButton();
	virtual void efSwitch(int number);
	virtual void dataSwitch(int number);
	virtual Byte getData();
	virtual void onHexDown(int hex);
	virtual void onHexKeyUp(int keycode);
	int getLoadedProgram() {return loadedProgram_;};
	int getLoadedOs() {return loadedOs_;};
	virtual int getRunState();
	virtual void checkLoadedSoftware();
	virtual void dataAvailable(bool data);
	virtual void thrStatus(bool data);
	virtual void setTempo(int tempo);
	virtual void switchQ(int value);
	int getGaugeValue() {return gaugeValue_;};
	void resetGaugeValue() {gaugeValue_ = 0;};
	void setTapePolarity(Byte polarity) {tapePolarity_ = polarity;};
	void setDebugMemoryStart(Word address) {memoryStart_ = address;};
	Word getDebugMemoryStart() {return memoryStart_;};
	void setConversionType(int convTypeWav, int convType) {conversionTypeWav_ = convTypeWav; conversionType_ = convType;};
	int getBasicExecAddr(int type){return basicExecAddress_[type];};
	virtual bool isEpromBoardLoaded() { return false; };
	virtual bool isSuperBoardLoaded() { return false; };
	void reDefineInKey(int inKey1, int inKey2);
    int getInKey1() {return inKey1_;};
    int getInKey2() {return inKey2_;};
	virtual void reDefineKeys();
    virtual void reDefineKeys(int *, int *, int);
	virtual void reDefineKeysA(int *, int *, int *);
	virtual void reDefineKeysB(int *, int *, int *);
	virtual void reDefineKeysA(int *, int *);
	virtual void reDefineKeysB(int *, int *);
	virtual long getCpuCycles() { return 0; };
	void setQsound(int status) {qSound_ = status;};

	virtual void onNumberKeyDown(wxCommandEvent& event);
	virtual void onNumberKeyUp(wxCommandEvent& event);
	virtual void ledTimeout();
	virtual void setLedMs(long ms);
    virtual void showDataLeds(Byte value);
	virtual Byte getKey(Byte vtOut);
	virtual void activateMainWindow();
	bool getAudioInStatus() {return audioIn_;};
	Word getChip8baseVar() {return chip8baseVar_;};
	Word getChip8MainLoop() {return chip8mainLoop_;};
	wxString getPseudoType() {return pseudoType_;};
	void showChip8Registers();
	virtual void writeMemDataType(Word address, Byte type);
	virtual Byte readMemDataType(Word address);
	virtual int getAds(int i){return i;};
	virtual int getAdi(int i){return i;};

	void writeDebugFileExit(wxString dir, wxString name, Word start, Word end);
	void writeDebugFile(wxString dir, wxString name, Word start, Word end);
	void readDebugFile(wxString dir, wxString name, wxString number, Word start);

    virtual void terminalSave(wxString fileName);
    virtual void terminalLoad(wxString filePath, wxString fileName, bool binaryFile);
    virtual void terminalStop();
    
	virtual void deleteAllBackup(wxString dir, bool sub);
	virtual void onBackupYes(wxString dir, bool sub);
    virtual int getBuildInGame(){return 0;};

	virtual void releaseButtonOnScreen(HexButton* buttonPointer, int buttonType);
    virtual void reLoadKeyDefinition(wxString fileName) {};
    virtual void setPrinterEf() {};
    virtual wxString getRunningGame() {return "";};
    virtual void setGreenLed(int status);

    virtual void setMultiCartLsb(Byte lsb);
	virtual void setMultiCartMsb(Byte msb); 
    virtual void setDisableSystemRom(bool disableSystemRom);
    virtual void setAutoKeyDef(bool autoKeyDef);
    virtual int getDmaCounter() {return 0;};

protected:
	RunComputer *threadPointer;


	int baseGiantBoard_;
	int baseQuestLoader_;
	int loadedProgram_;
	int loadedOs_;

	Byte cassetteEf_;
	Byte oldCassetteEf_;
	short gaugeValue_;
	short maxTapeInput_;
	short lastTapeInput_;
	Byte tapePolarity_;
	int conversionType_;
	int conversionTypeWav_;
	int basicExecAddress_[BASICADDR_MAX];
	int qSound_;
	bool resetPressed_;
	bool audioIn_;
	bool audioOut_;
	Word memoryStart_;
	Word chip8baseVar_;
	Word chip8mainLoop_;
    bool chip8register12bit_;

	wxString pseudoType_;
    bool pseudoLoaded_;

    int inKey1_;
    int inKey2_;

private:
	int chip8Register[16];

	int counter_;
	bool sign_;
	int period_;
};

#endif	// COMP_H

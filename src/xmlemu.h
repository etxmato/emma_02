#ifndef XMLEMU_H
#define XMLEMU_H

#include "cdp1802.h"
#include "fdc.h"
#include "ide.h"
#include "keyboard.h"
#include "ps2gpio.h"
#include "portext.h"
#include "ps2.h"
#include "vt100.h"
#include "serial.h"

#include "tms9918.h"
#include "pixie.h"
#include "mc6847.h"
#include "mc6845.h"
#include "i8275.h"
#include "vis1870.h"
#include "sn76430n.h"
#include "keyb1871.h"
#include "bitkeypad.h"
#include "cvkeypad.h"
#include "keypadfred.h"
#include "keyblatch.h"
#include "keybmatrix.h"
#include "printer.h"
#include "keypad.h"
#include "joycard.h"
#include "usb.h"
#include "rtc.h"
#include "pio.h"
#include "cdp1852.h"
#include "cdp1854.h"
#include "cdp1855.h"
#include "cdp1877.h"
#include "cd4536b.h"
#include "upd765.h"
#include "ay-3-8912.h"

class NvramDetails
{
public:
    wxString dirname;
    wxString filename;
    Word start;
    Word end;
    bool mcr;
    int mcrMemNumber;
};

class Computer : public wxFrame, public Cdp1802, public Fdc, public Ide, public Keyboard, public Keyb1871, public PortExt, public Ps2, public Ps2gpio, public Joycard, public Usbcard, public RtcCDP1879, public RtcDs12788, public Upd765
{
public:
    Computer(const wxString& title, double clock, int tempo, ComputerConfiguration computerConfig);
    Computer() {};
    ~Computer();

    void onClose(wxCloseEvent&event);
    void resumeComputer();
    void charEvent(int keycode);
    bool keyDownPressed(int keycode);
    bool keyDownExtended(int keycode, wxKeyEvent& event);
    bool keyUpReleased(int keycode);
    bool keyUpReleased(int keycode, wxKeyEvent& event);

    void onRunButtonPress(wxCommandEvent&event);
    void onRunButtonPress0(wxCommandEvent&event);
    void onRunButtonPress(bool run0);
    void onRunButtonRelease();
    void onRun();
    void onButtonRelease(wxCommandEvent& event);
    void onButtonPress(wxCommandEvent& event);
    void onSwitchButtonPress(wxCommandEvent& event);
    void onInButtonPress(bool switchButton);
    void onInButtonRelease();
    void onHexKeyDown(int keycode);
    void onHexDown(int hex);
    void onHexKeyUp(int keycode);
    void onClearButton(wxCommandEvent&event);
    void onClearButton();
    void onClearSwitch();

    void configureComputer();
    void setPrinterEf();
    void reLoadKeyDefinition(wxString fileName);
    void reDefineKeysA(int *, int *);
    void reDefineKeysB(int *, int *);
    void initComputer();
    void resetComputer();
    Byte ef(int flag);
    Byte in(Byte port, Word address);
    Byte inDip();
    Byte getData(bool switchButton);
    void out(Byte port, Word address, Byte value);
    void cycle(int type);
    void cycleCd();
    void cycleVP550();
    void cycleBitKeyPad();
    void cycleDma();
    void cycleInt();
    void picInterruptRequest(int type, bool state, int picNumber);
    void cycleLed();
    void printOutPecom(int q);
    void onXmlF4(bool forceStart);
    void slotOut(Byte value);
    Byte slotShift(Byte value, int shift);
    void showData(Byte value);
    void showCycleData(Byte val);
    void showCycleAddress(Word val);
    void setGoTimer();
    void setMode();
    void showState(int state);
    void showDmaLed();
    void showIntLed();
    void showStatusLed(int led, int status);
    void updateStatusBarLedStatus(bool status, int led);

    void autoBoot();
    void dmaOnBoot();
    void switchQ(int value);
    int getMpButtonState();
    void onWaitButton(wxCommandEvent&event);
    void onWaitButton();
    void onStopButton(wxCommandEvent&event);
    void onPowerButton(wxCommandEvent&event);
    void onPowerButton();
    void powerOff();
    void powerOn();
    void runPressed();
    void onPause(wxCommandEvent&event);
    void onPause();
    void onMpButton(wxCommandEvent&event);
    void onMpButtonMulti(wxCommandEvent&event);
    void onMpButton();
    void onMpSuperButton(wxCommandEvent&event);
    void onMpSuperButton();
    void onMpButton(int buttonNumber);
    void onEmsButton(int buttonNumber, bool up);
    void onEmsButton(wxCommandEvent&event);
    void setMultiCartGame();
    void onRamButton(wxCommandEvent&event);
    void onRamButton();
    void onMonitor(wxCommandEvent&event);
    void onMonitor();
    void onNanoMonitor(wxCommandEvent&event);
    void onNanoMonitor();
    void onLoadButton(wxCommandEvent&event);
    void onLoadButton(bool pushButton);
    void onClearResetButtonPress();
    void onClearButtonRelease();
    void onSingleStep(wxCommandEvent&event);
    void onSingleStep();
    void onResetButton(wxCommandEvent&event);
    void onResetButton();
//    void onResetButtonPress();
//    void onResetButtonRelease();
    void onReadButton(wxCommandEvent&event);
    void onReadButton();
    void onCardButtonSwitch(wxCommandEvent&event);
    void onCardButtonSwitch();
    void updateCardReadStatus();
    void dataSwitch(int i);
    void efSwitch(int i);

    void onNumberKeyDown(int i);
    void onNumberKeyDown(wxCommandEvent& event);
    void onNumberKeyUp(wxCommandEvent& event);
    void onNumberKeyUp();
    void onEfKeyDown(int ef);
    void onEfKeyUp(int ef);

    void startComputer();
    void loadRomRam(size_t configNumber);
    void writeMemDataType(Word address, Byte type);
    Byte readMemDataType(Word address, uint64_t* executed);
    Byte readMem(Word address);
    void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address, int function = 0);
    void writeMemDebug(Word address, Byte value, bool writeRom);
    void cpuInstruction();
    void resetPressed();
    void showDataLeds(Byte value);
    void configureMemory();
    void configureExtensions();
    void configureVideoExtensions();
    void configureV1870Extension();
    void configureKeyboardExtensions();
    void configureDiskExtensions();
    void configurePrinterExtensions();
    void configureSoundExtensions();
    void configureTapeExtensions();
    void configureCdp1863();
    void moveWindows();
    void updateTitle(wxString Title);
    void releaseButtonOnScreen1851(HexButton* buttonPointer, int buttonType, int pioNumber);
    void releaseButtonOnScreen1852(HexButton* buttonPointer, int buttonType, int pioNumber);
    void showCdp1851(int pioNumber, bool state);
    void showCdp1852(int pioNumber, bool state);
    void removeCdp1851(int pioNumber);
    void removeCdp1852(int pioNumber);
    void setForceUpperCase(bool status);
    void keyClear();
    void onReset();
    void sleepComputer(long ms);

    Byte getTmsMemory(int address) {return tmsPointer->getTmsMemory(address);};
    void setTmsMemory(int address, Byte value) {tmsPointer->setTmsMemory(address, value);};
    Byte read8275CharRom(Word address);
    void write8275CharRom(Word address, Byte value);
    Byte read8275VideoRam(Word address);
    void write8275VideoRam(Word address, Byte value);
    Byte read1862ColorDirect(Word address);
    void write1862ColorDirect(Word address, Byte value);
    Byte read1864ColorDirect(Word address);
    void write1864ColorDirect(Word address, Byte value);
    Byte readSt4ColorDirect(Word address);
    void writeSt4ColorDirect(Word address, Byte value);
    Byte read6845CharRom(Word address);
    void write6845CharRom(Word address, Byte value);
    Byte read6847CharRom(Word address);
    void write6847CharRom(Word address, Byte value);
    int readDirect6847(Word address); 
    Word get6847RamMask();
    void writeDirect6847(Word address, int value); 
    void setLedMs(long ms);
    void setLedMsTemp(long ms);
    Byte getKey(Byte vtOut);
    void activateMainWindow();
    void releaseButtonOnScreen(HexButton* buttonPointer);
    void setGreenLed(int status);
    void refreshPanel();
    void showPanel();
    void OnRtcTimer(wxTimerEvent& event);
    void writeRtc(int address, Byte value);
    void cid1Bit8(bool set);
    void changeDiskName(int disk, wxString dirName, wxString fileName);

    Byte readPramDirect(Word address);
    Byte readCramDirect(Word address);
    Byte readColourRamDirect(Word address);
    Byte readGraphicRamDirect(Word address);
    void writePramDirect(Word address, Byte value);
    void writeCramDirect(Word address, Byte value);
    void writeColourRamDirect(Word address, Byte value);
    void writeGraphicRamDirect(Word address, Byte value);
    int getMaxLinesPerChar();
    Word getPageMemorySize();
    Word getCharMemorySize();
    Word getGraphicMemorySize();
    Byte getPcbMask();
    void saveNvRam();
    void loadNvRam(size_t configNumber);

    void checkComputerFunction();
    void checkRegFunction(char trigger);
    void executeFunction(int function, Word additionalAddress);
    bool checkKeyInputAddress(Word address);
    bool checkKeyInputAddress();

    void startComputerRun(bool load);
    
    void ctrlvText(wxString text);
    void ctrlvTextXml(wxString text);
    int getCtrlvChar();
    int getCtrlvChar(bool increase);
    int getCtrlvCharTmc();
    size_t getCtrlvCharNum() {return ctrlvTextCharNum_;};
    void ctrlvTextCharNumPlusOne();

    int getRunState() {return elfRunState_;};
    bool isComputerRunning();
    bool isFAndMBasicRunning();
    wxString getRunningGame(){return runningGame_;};

    void terminalSave(wxString fileName, int protocol);
    void terminalYsSave(wxString fileName, int protocol);
    void terminalLoad(wxString filePath, wxString fileName, int protocol);
    void terminalStop();
    void setDivider(Byte value);
    void dataAvailableVt100(bool data, int uartNumber);
    void thrStatusVt100(bool data);
    void thrStatusSerial(bool data);
    void saveRtc();
    void loadRtc();
    Byte readDirectRtc(Word address);
    void writeDirectRtc(Word address, Byte value);
    void setDosFileName();
    
    int getSelectedSlot() {return selectedSlot_;};
    void setSelectedSlot(int slot);

    int getSelectedBank() {return selectedBank_;};
    void setSelectedBank(int bank);

    void cidelsaStatusBarDown(int keycode);
    void cidelsaStatusBarUp(int keycode);

    void setBatchFileNumber(int number);
    void batchConvert();
    bool getBatchFile(wxString memAccessExtension);

    void closeKeyFile();
    void resetV1870VideoModeEf();

    void activateElfOsChip8();
    void fetchFileName(Word address, size_t length);
    
    void startLoad(int tapeNumber, bool button);
    void cassette(wxInt32 val);
    void cassette(wxInt16 val);
    void cassette(char val);
    void realCassette(short val);
    void cassetteXmlHw(wxInt32 val, long size);
    void cassetteXmlHw(wxInt16 val, long size);
    void cassetteXmlHw(char val, long size);
    void stepCassetteCounter(long step);
    void cassetteCyberVision();
    void cassette56();
    void cassettePm();
    void startRecording(int tapeNumber);
    void finishStopTape();
    void resetTape();
    void tapeIo(Byte value);
    void onDataSwitch(wxCommandEvent&event);
    void onCardButton(wxCommandEvent&event);
    void cardButton(int cardValue);
    void setTempo(int tempo);

    int getGaugeValue() {return gaugeValue_;};
    void resetGaugeValue() {gaugeValue_ = 0;};
    void setTapePolarity(Byte polarity) {tapePolarity_ = polarity;};
    void setConversionType(int convTypeWav, int convType) {conversionTypeWav_ = convTypeWav; conversionType_ = convType;};

    bool isAudioChannelLeft() {return currentComputerConfiguration.hwTapeConfiguration.audioChannelLeft;};
    bool isDataChannelLeft() {return currentComputerConfiguration.hwTapeConfiguration.dataChannelLeft;};
    int getFrequency0() {return currentComputerConfiguration.hwTapeConfiguration.frequency0;};
    int getFrequency1() {return currentComputerConfiguration.hwTapeConfiguration.frequency1;};
    int getStartBit() {return currentComputerConfiguration.hwTapeConfiguration.startBit;};
    int getDataBits() {return currentComputerConfiguration.hwTapeConfiguration.dataBits;};
    int getStopBit() {return currentComputerConfiguration.hwTapeConfiguration.stopBit;};
    int getIoGroup() {return ioGroup_;};

    void onBackupYes(wxString dir, bool sub);
    void setEfKeyValue(int ef, Byte value);
    
    Word getChip8baseVar() {return chip8baseVar_;};
    Word getChip8MainLoop() {return chip8mainLoop_;};
    wxString getPseudoType() {return pseudoType_;};
    void showChip8Registers();

    void reDefineInKey(int inKey1, int inKey2);
    int getInKey1() {return inKey1_;};
    int getInKey2() {return inKey2_;};

    void setDebugMemoryStart(Word address) {memoryStart_ = address;};
    Word getDebugMemoryStart() {return memoryStart_;};
    void writeDebugFile(wxString dir, wxString name, Word start, Word end);
    void readDebugFile(wxString dir, wxString name, wxString number, Word start);
    
    void switchBootStrap();
    Byte getTilHexFont(Word address, int segNumber);

    void setThumbSwitch(Byte value) {thumbSwitchValue_ = value;};
    bool serialDataOutput(int connection, Byte transmitterHoldingRegister);
    Byte readReceiverHoldingRegister();
    void setSendPacket(bool status);
    void setTerminalLoad(bool status);
    void setTerminalSave(bool status);

private:
    RunComputer *threadPointer;
    wxString title_;
    
    vector<PanelFrame *> panelPointer;
    int numberOfFrontPanels_;

    vector<PioFrame *> cdp1851FramePointer;
    int numberOfCdp1851Frames_;

    vector<Cdp1854Instance *> cdp1854InstancePointer;
    int numberOfCdp1854Instances_;
    int cdp1854Vt100Connection_;

    Cdp1855Instance *cdp1855InstancePointer;
    
    AY_3_8912Instance *ay_3_8912InstancePointer;

    vector<Cdp1877Instance *> cdp1877InstancePointer;
    int numberOfCdp1877Instances_;

    vector<Cdp1852Frame *> cdp1852FramePointer;
    int numberOfCdp1852Frames_;

    vector<Cd4536b *> cd4536bPointer;
    int numberOfCd4536b_;

    Tms9918 *tmsPointer;
    SN76430N *sn76430nPointer;
    Pixie *coinPointer;
    Pixie *pixiePointer;
    Pixie *cdp1864Pointer;
    PixieStudioIV *st4VideoPointer;
    PixieVip2K *vip2KVideoPointer;
    PixieFred *fredVideoPointer;
    MC6845 *mc6845Pointer;
    mc6847 *mc6847Pointer;
    i8275 *i8275Pointer;
    VIS1870 *vis1870Pointer;
    BitKeypad *bitkeypadPointer[MAX_BITKEYPADS];
    CvKeypad *cvkeypadPointer;
    EtiKeypad *etikeypadPointer;
    KeypadFred *fredkeypadPointer;
    KeybLatch *latchKeyPointer[MAX_LATCHKEYPADS];
    KeybMatrix *matrixKeyboardPointer;
    Vt100 *vtPointer;

    bool videoNumber_;
    int thumbSwitchValue_;

    int ledCycleValue_;
    int ledCycleSize_;
    long ledTimeMs_;
    bool updateQLed_;
    bool updateTapeLed_;

    bool cycleKeyOn_;
    bool cycleFlashOn_;
    bool flashState_;
    bool runReleased_;
    int flashCycleValue_;
    bool vipMode_;

    int goCycleValue_;
    int goCycleSize_;

    Byte switches_;
    bool powerButtonState_;
    int waitButtonState_;
    int runButtonState_;
    int mpButtonState_;
    bool mpButtonState[4];
    bool mpSuperButtonActive_;
    bool nvRamDisable_;
    int loadButtonState_;
    Byte inbuttonEfState_;
    Byte hexEfState_;
    char nextNybble_;
    Byte printerEfState_;
    int qLedStatus_;

    Byte tapeRunSwitch_;
    bool cardSwitchOn_;
    bool readSwitchOn_;
    int inpMode_;

    int keyDefA1_[16];
    int keyDefB1_[16];
    int keyDefA2_[16];
    int keyDefB2_[16];
    
    bool simDefA2_;
    bool simDefB2_;
    
    int keyDefGameHexA_[5];
    int keyDefGameHexB_[5];
    
    bool inPressed_;
    int dataSwitchState_[8];
    int efSwitchState_[4];

    Byte lastMode_;
    bool monitor_;
    int nanoMonitor_;
    
    Byte vismacRegisterLatch_;
    
    bool thermalPrinting_;
    Byte thermalEF_;

    int selectedMap_;
    int selectedSlot_;
    int selectedBank_;
    int ioGroup_;

    int dmaCounter_;
    int intCounter_;

    wxDateTime systemTime_;
    wxDateTime xmlComputerTime_;
    int day_, month_, year_;
    bool nvramWriteProtected_;
    
    vector<NvramDetails> nvramDetails;

    int elfRunState_;
    bool runPressed_;

    int cycleValue_;
    int cycleSize_;
    double computerClockSpeed_;

    int multiTilCycleValue_;
    int multiTilCycleSize_;

    int soundTempoCycleValue_;
    int soundTempoCycleSize_;
    int vipIIRunCycleSize_;

    wxString runningGame_;

    wxTimer *rtcTimerPointer;
    int rtcCycle_;

    bool diagRomActive_;
    bool diagDmaLedOn_;

    int batchFileNumber_;
    bool batchInProgress_;

    int printerStatus_;
    int printBit_;
    int printValue_;

    bool ramGroupAtV1870_;
        
    bool hexModemOnStart;

    int segNumber_;
    int segValue_;
    int qState_;

    bool tapeActivated_;
    long pauseTapeCounter_;
    int zeroWaveCounter_;

    wxInt32 lastSampleInt32_;
    wxInt16 lastSampleInt16_;
    char lastSampleChar_;
    int pulseCount_;
    Byte tapeInput_;
    Byte lastTapeInpt_;
    Byte polarity_;
    int bitNumber_;
    int silenceCount_;
    bool tapeEnd_;
    int toneTime_;
    Byte tapeError_;
    Byte tapedataReady_;
    int lastSec_;
    long tapeFinished_;
    Byte cassetteEf_;
    Byte oldCassetteEf_;

    int pulseCountStopTone_;
    bool tapeFormat56_;
    bool tapeFormatFixed_;
    int startBytes_;
    
    Byte tapePolarity_;
    short gaugeValue_;
    int conversionType_;
    int conversionTypeWav_;
    wxInt32 maxTapeInputInt32_;
    wxInt16 maxTapeInputInt16_;
    char maxTapeInputChar_;
    wxInt32 lastTapeInputInt32_;
    wxInt16 lastTapeInputInt16_;
    char lastTapeInputChar_;

    bool saveStarted_;
    bool loadStarted_;

    int bitKeypadValue_;
    int lastBitKeyPad_;
    int lastLatchKeyPad_;

    wxString tapeCounter_;
    Word endSave_;

    int colourMask1862_;
    bool colourLatch_;

    bool vp550IntOn_;
    bool runPressedAtStartup_;
    Byte printLatch_;

    bool specifiedSoftwareLoaded_;
    int emsButton_[2];
    
    int adInputNumber_;
    int adOutputFunction_;
    int adiValue_;
    int adiChannel_;
    
    int addressLatchCounter_;

    bool fAndMBasicRunning_;
    
    Byte efKeyValue[5];
    Word chip8baseVar_;
    Word chip8mainLoop_;
    int chip8Register[16];
    bool chip8register12bit_;
    wxString pseudoType_;

    wxString ctrlvTextStr_;
    size_t ctrlvTextCharNum_;

    int inKey1_;
    int inKey2_;
    
    Word memoryStart_;

    DECLARE_EVENT_TABLE()
};

#endif  // XMLEMU_H

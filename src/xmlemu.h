#ifndef XMLEMU_H
#define XMLEMU_H

#include "elf.h"
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
#include "ledmodule.h"
#include "printer.h"
#include "elfconfiguration.h"
#include "hbelf.h"
#include "elf2.h"
#include "super.h"
#include "microtutor.h"
#include "microtutor2.h"
#include "elf2k.h"
#include "velf.h"
#include "fred.h"
#include "cosmicos.h"
#include "joycard.h"
#include "usb.h"
#include "rtc.h"

class NvramDetails
{
public:
    wxString dirname;
    wxString filename;
    Word start;
    Word end;
};

class Xmlemu : public wxFrame, public Cdp1802, public Fdc, public Ide, public Keyboard, public Keyb1871, public PortExt, public Ps2, public Ps2gpio, public Joycard, public Usbcard, public Rtc
{
public:
    Xmlemu(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, int tempo, ElfConfiguration conf, Conf computerConf);
    Xmlemu() {};
    ~Xmlemu();

    void onClose(wxCloseEvent&event);
    void resumeComputer();
    void showModules(bool status);
    void showModules(bool status, bool useSwitch, bool useHex);
    void removeElfHex() {hexKeypadClosed_ = true; elfConfiguration.useHex = false;};
    void removeElf2KSwitch() {elfConfiguration.useSwitch = false;};
    void removeElfLedModule() {ledModuleClosed_ = true;};
    void charEvent(int keycode);
    bool keyDownPressed(int keycode);
    bool keyDownExtended(int keycode, wxKeyEvent& event);
    bool keyUpReleased(int keycode, wxKeyEvent& event);

    void onRun();
    void onButtonRelease(wxCommandEvent& event);
    void onButtonPress(wxCommandEvent& event);
    void onInButtonPress();
    void onInButtonPress(Byte value);
    void onElf2KInButton();
    void onInButtonRelease();
    void onHexKeyDown(int keycode);
    void onHexDown(int hex);
    void onHexKeyUp(int keycode);
    void onNumberKeyPress(int key);
    void onNumberKeyRelease(int key);
    void onClearButton();

    void configureComputer();
    void switchHexEf(bool state);
    void setPrinterEf();
    void reLoadKeyDefinition(wxString fileName);
    void reDefineKeysA(int *, int *);
    void reDefineKeysB(int *, int *);
    void initComputer();
    void resetComputer();
    Byte ef(int flag);
    Byte in(Byte port, Word address);
    Byte inDip();
    Byte getData();
    void out(Byte port, Word address, Byte value);
    void cycle(int type);
    void cycleVP550();
    void cycleBitKeyPad();
    void cycleDma();
    void cycleInt();
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
    void updateStatusBarLedStatus(int led, bool status);

    void autoBoot();
    void switchQ(int value);
    int getMpButtonState();
    void onWaitButton();
    void onPowerButton(wxCommandEvent&event);
    void onPowerButton();
    void powerOff();
    void powerOn();
    void runPressed();
    void onRunButton(wxCommandEvent&event);
    void onRunButton0(wxCommandEvent&event);
    void onRunButton(bool run0);
    void onRunButtonPress();
    void onRunButtonRelease();
    void onMouseRelease(wxMouseEvent&event);
    void onPause(wxCommandEvent&event);
    void onPause();
    void onMpButton(wxCommandEvent&event);
    void onMpButtonMulti(wxCommandEvent&event);
    void onMpButton();
    void onMpButton(int buttonNumber);
    void onEmsButton(int buttonNumber, bool up);
    void onEmsButton(wxCommandEvent&event);
    void setMultiCartGame();
    void onRamButton();
    void onMonitor(wxCommandEvent&event);
    void onMonitor();
    void onLoadButton(wxCommandEvent&event);
    void onLoadButton();
    void onClearButtonPress();
    void onClearButtonRelease();
    void onSingleStep(wxCommandEvent&event);
    void onSingleStep();
    void onResetButton(wxCommandEvent&event);
    void onResetButton();
    void onResetButtonPress();
    void onResetButtonRelease();
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

    void startComputer();
    void loadRomRam(size_t configNumber);
    void writeMemDataType(Word address, Byte type);
    Byte readMemDataType(Word address, uint64_t* executed);
    Byte readMem(Word address);
    void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
    void cpuInstruction();
    void resetPressed();
    void configureMemory();
    void configureExtensions();
    void configureCdp1863();
    void moveWindows();
    void updateTitle(wxString Title);
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
    void releaseButtonOnScreen(HexButton* buttonPointer, int buttonType);
    void setGreenLed(int status);
    void refreshPanel();
    void OnRtcTimer(wxTimerEvent& event);
    void writeRtc(int address, Byte value);
    void cid1Bit8(bool set);
    void changeDiskName(int disk, wxString dirName, wxString fileName);

    Byte readPramDirect(Word address);
    Byte readCramDirect(Word address);
    Byte readColourRamDirect(Word address);
    void writePramDirect(Word address, Byte value);
    void writeCramDirect(Word address, Byte value);
    void writeColourRamDirect(Word address, Byte value);
    int getMaxLinesPerChar();
    Word getPageMemorySize();
    Word getCharMemorySize();
    Byte getPcbMask();
    void saveNvRam();
    void loadNvRam(size_t configNumber);

    void checkComputerFunction();
    void checkRegFunction(char trigger);
    void executeFunction(int function, Word additionalAddress);
    bool checkKeyInputAddress(Word address);

    void startComputerRun(bool load);
    int getRunState() {return elfRunState_;};
    bool isComputerRunning();
    wxString getRunningGame(){return runningGame_;};

    void terminalSave(wxString fileName, int protocol);
    void terminalYsSave(wxString fileName, int protocol);
    void terminalLoad(wxString filePath, wxString fileName, int protocol);
    void terminalStop();
    void setDivider(Byte value);
    void dataAvailableVt100(bool data, int uartNumber);
    void dataAvailableSerial(bool data);
    void thrStatusVt100(bool data);
    void thrStatusSerial(bool data);
    void saveRtc();
    void loadRtc();
    Byte readDirectRtc(Word address);
    void writeDirectRtc(Word address, Byte value);
    void setDosFileName();
    
    int getSelectedSlot() {return selectedSlot_;};
    void setSelectedSlot(int slot) {selectedSlot_ = slot;};

    void cidelsaStatusBarDown(int keycode);
    void cidelsaStatusBarUp(int keycode);

    void setBatchFileNumber(int number);
    void batchConvert();
    bool getBatchFile(wxString memAccessExtension);

    void closeKeyFile();
    void resetV1870VideoModeEf();

    void activateElfOsChip8();
    void fetchFileName(Word address, size_t length);

    void removeCosmicosHex();
    
    void startLoad(int tapeNumber, bool button);
    void cassette(wxInt32 val);
    void cassette(wxInt16 val);
    void cassette(char val);
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
    void onCardButton(wxCommandEvent&event);
    void cardButton(int cardValue);
    void setTempo(int tempo);

    bool isAudioChannelLeft() {return elfConfiguration.tape_audioChannelLeft;};
    bool isDataChannelLeft() {return elfConfiguration.tape_dataChannelLeft;};
    int getFrequency0() {return elfConfiguration.tape_frequency0;};
    int getFrequency1() {return elfConfiguration.tape_frequency1;};
    int getStartBit() {return elfConfiguration.tape_startBit;};
    int getDataBits() {return elfConfiguration.tape_dataBits;};
    int getStopBit() {return elfConfiguration.tape_stopBit;};
    
private:
    class ElfScreen *elfScreenPointer;
    class Elf2Screen *elf2ScreenPointer;
    class SuperScreen *superScreenPointer;
    class Elf2KScreen *elf2KScreenPointer;
    class MemberScreen *memberScreenPointer;
    class MicrotutorScreen *microtutorScreenPointer;
    class Microtutor2Screen *microtutor2ScreenPointer;
    class CosmicosScreen *cosmicosScreenPointer;
    class VelfScreen *velfScreenPointer;
    class Uc1800Screen *uc1800ScreenPointer;

    Tms9918 *tmsPointer;
    SN76430N *sn76430nPointer;
    Pixie *coinPointer;
    Pixie *pixiePointer;
    Pixie *cdp1862Pointer;
    Pixie *cdp1864Pointer;
    PixieVip2K *vip2KVideoPointer;
    PixieFred *fredVideoPointer;
    MC6845 *mc6845Pointer;
    mc6847 *mc6847Pointer;
    i8275 *i8275Pointer;
    VIS1870 *vis1870Pointer;
    Keypad *keypadPointer;
    BitKeypad *bitkeypadPointer[MAX_BITKEYPADS];
    CvKeypad *cvkeypadPointer;
    EtiKeypad *etikeypadPointer;
    KeypadFred *fredkeypadPointer;
    KeybLatch *latchKeyPointer[MAX_LATCHKEYPADS+1];
    KeybMatrix *matrixKeyboardPointer;
    LedModule *ledModulePointer;
    Vt100 *vtPointer;

    bool use6845_;
    
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
    bool ledModuleClosed_;
    bool hexKeypadClosed_;
    int dataSwitchState_[8];
    int efSwitchState_[4];

    Byte lastMode_;
    bool monitor_;
    
    Byte vismacRegisterLatch_;
    
    bool thermalPrinting_;
    Byte thermalEF_;

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
    double elfClockSpeed_;

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

    Elf2Kswitch *p_Elf2Kswitch;
    Elf2Khex *p_Elf2Khex;
    
    Cosmicoshex *p_Cosmicoshex;
    
    bool hexModemOnStart;

    int segNumber_;
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

    int pulseCountStopTone_;
    bool tapeFormat56_;
    bool tapeFormatFixed_;
    int startBytes_;
    
    int bitKeypadValue_;
    int lastBitKeyPad_;
    int lastLatchKeyPad_;

    wxString tapeCounter_;
    Word endSave_;

    int colourMask1862_;

    bool vp550IntOn_;
    bool runPressedAtStartup_;
    Byte printLatch_;

    bool specifiedSoftwareLoaded_;
    int emsButton_[2];
    
    DECLARE_EVENT_TABLE()
};

#endif  // XMLEMU_H

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
    Xmlemu(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf, Conf computerConf);
    Xmlemu() {};
    ~Xmlemu();

    void onClose(wxCloseEvent&WXUNUSED(event));
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
    void cycleDma();
    void cycleInt();
    void cycleLed();
    void printOutPecom(int q);
    void onXmlF4(bool forceStart);
    void slotOut(Byte value);
    Byte slotShift(Byte value, int shift);
    void showData(Byte value);
    void showCycleData(Byte val);
    void setGoTimer();
    void showState(int state);
    void showDmaLed();
    void showIntLed();

    void autoBoot();
    void switchQ(int value);
    int getMpButtonState();
    void onRunButton(wxCommandEvent&WXUNUSED(event));
    void onRunButton();
    void onRunButtonPress();
    void onRunButtonRelease();
    void onMouseRelease(wxMouseEvent&event);
    void onPause(wxCommandEvent&WXUNUSED(event));
    void onPause();
    void onMpButton(wxCommandEvent&WXUNUSED(event));
    void onMpButton();
    void onMonitor(wxCommandEvent&WXUNUSED(event));
    void onMonitor();
    void onLoadButton(wxCommandEvent&WXUNUSED(event));
    void onLoadButton();
    void onClearButtonPress();
    void onClearButtonRelease();
    void onSingleStep(wxCommandEvent&WXUNUSED(event));
    void onSingleStep();
    void onResetButton(wxCommandEvent&WXUNUSED(event));
    void onResetButton();
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
    void moveWindows();
    void updateTitle(wxString Title);
    void setForceUpperCase(bool status);
    void onReset();
    void sleepComputer(long ms);

    Byte getTmsMemory(int address) {return tmsPointer->getTmsMemory(address);};
    void setTmsMemory(int address, Byte value) {tmsPointer->setTmsMemory(address, value);};
    Byte read8275CharRom(Word address);
    void write8275CharRom(Word address, Byte value);
    Byte read8275VideoRam(Word address);
    void write8275VideoRam(Word address, Byte value);
    Byte read6845CharRom(Word address);
    void write6845CharRom(Word address, Byte value);
    Byte read6847CharRom(Word address);
    void write6847CharRom(Word address, Byte value);
    int readDirect6847(Word address); 
    Word get6847RamMask();
    void writeDirect6847(Word address, int value); 
    void setLedMs(long ms);
    Byte getKey(Byte vtOut);
    void activateMainWindow();
    void releaseButtonOnScreen(HexButton* buttonPointer, int buttonType);
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
    void executeFunction(int function, Word additionalAddress);

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
    void cassetteXmlHw(uint32_t val, long size);
    void cassetteXmlHw(short val, long size);
    void cassetteXmlHw(char val, long size);
    void stepCassetteCounter(long step);
    void cassetteCyberVision();
    void cassette56();
    void cassettePm();
    void finishStopTape();
    
    bool isAudioChannelLeft() {return elfConfiguration.audioChannelLeft;};
    bool isDataChannelLeft() {return elfConfiguration.dataChannelLeft;};

private:
    class ElfScreen *elfScreenPointer;
    class Elf2Screen *elf2ScreenPointer;
    class SuperScreen *superScreenPointer;
    class Elf2KScreen *elf2KScreenPointer;
    class MicrotutorScreen *microtutorScreenPointer;
    class Microtutor2Screen *microtutor2ScreenPointer;
    class CosmicosScreen *cosmicosScreenPointer;

    Tms9918 *tmsPointer;
    SN76430N *sn76430nPointer;
    Pixie *pixiePointer;
    Pixie *cdp1864Pointer;
    MC6845 *mc6845Pointer;
    mc6847 *mc6847Pointer;
    i8275 *i8275Pointer;
    VIS1870 *vis1870Pointer;
    Keypad *keypadPointer;
    BitKeypad *bitkeypadPointer[2];
    CvKeypad *cvkeypadPointer;
    KeybLatch *latchKeyboardPointer;
    KeybMatrix *matrixKeyboardPointer;
    LedModule *ledModulePointer;
    Vt100 *vtPointer;

    int ledCycleValue_;
    int ledCycleSize_;
    int setMsValue_;

    int goCycleValue_;
    int goCycleSize_;

    Byte switches_;
    int runButtonState_;
    int mpButtonState_;
    int loadButtonState_;
    Byte inbuttonEfState_;
    Byte hexEfState_;
    Byte printerEfState_;

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

    Byte efKeyValue[5];

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
    int cycleValue_;
    int cycleSize_;
    double elfClockSpeed_;

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
    bool tapeRecording_;
    int zeroWaveCounter_;

    short lastSample_;
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
    long long tapeCounterStep_;
    long long tapePeriod_;
    int lastSec_;

    int pulseCountStopTone_;
    bool tapeFormat56_;
    bool tapeFormatFixed_;
    int startBytes_;

    DECLARE_EVENT_TABLE()
};

#endif  // XMLEMU_H

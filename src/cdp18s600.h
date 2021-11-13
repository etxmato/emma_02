#ifndef CDP18S600_H
#define CDP18S600_H

#include "cdp1802.h"
#include "vt100.h"
#include "serial.h"
#include "upd765.h"
#include "elfconfiguration.h"

#define IO_GRP1_UART 1
#define IO_GRP2_UART 2
#define IO_GRP_PIO 8

class Cdp18s600 : public Upd765, public V1870
{
public:
    Cdp18s600(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock, ElfConfiguration conf);
//    Cdp18s600() {};
    virtual ~Cdp18s600();
    
    void onClose(wxCloseEvent&WXUNUSED(event));
    
    virtual void configureComputer();
    virtual void configureVt();
    virtual void configurePio();
    void configureCdp18s660();
    void initComputer();
    
    void onRunButton();
    void onRunPButton();
    void onRun();
    virtual void autoBoot();
    void setAddressLatch(Word bootAddress);
    void onSingleStep();

    virtual Byte ef(int flag);
    int defaultEf(int flag);
    void setEfState(int pioNumber, int number, Byte value);
    virtual Byte in(Byte port, Word address);
    int defaultIn(Byte port);
    virtual void out(Byte port, Word address, Byte value);
    void defaultOut(Byte port, Word address, Byte value);
    Byte keyboardIn();
    void charEvent(int keycode);
    bool keyDownExtended(int keycode, wxKeyEvent& event);
    void keyUp(int keycode);
    void keyClear();
    void tapeIo(Byte value);
    void switchQ(int value);
    void showCycleData(Byte val);
    void showAddress(Word val);
    void showState(int state);
    virtual void setCpuMode(int mode);
    virtual void cycle(int type);
    virtual void cycleLed();
    void cycleKeyInput();

    virtual void startComputer();
    virtual void startPio(long ms);
    void startCdp18s660(long ms);
    void setDiskNames();
    virtual void readRoms();
    void configureCards();
    void readMicro(int romNumber, Word startAddress, Word lastAddress);
    virtual void writeMemDataType(Word address, Byte type);
    virtual Byte readMemDataType(Word address, uint64_t* executed);
    Byte readMem(Word address);
    void writeMem(Word address, Byte value, bool writeRom);
    virtual Byte readMemDebug(Word address);
    virtual void writeMemDebug(Word address, Byte value, bool writeRom);
    void cpuInstruction();
    void resetPressed();
    virtual void moveWindows();
    void setForceUpperCase(bool status);
    void setBootRam(bool status);
    virtual void updateTitle(wxString Title);
    void onReset();
    void sleepComputer(long ms);
    virtual void setLedMs(long ms);
    void startComputerRun(bool load);
    int getRunState() {return cdpRunState_;};
    bool isComputerRunning();
    void checkComputerFunction();

    void setCdp18s600ClockSpeed(double clock) {Cdp18s600ClockSpeed_ = clock;};
    void releaseButtonOnScreen(HexButton* buttonPointer, int buttonType);
    virtual void releaseButtonOnScreen2(HexButton* buttonPointer, int buttonType, int pioNumber);
    void activateMainWindow();
    virtual void showPio(bool state);
    void showCdp18s660Pio1(bool state);
    void showCdp18s660Pio2(bool state);
    void removePio(int pioNumber);
    void setHeaderTitle(const wxString& title);
    void showControlWindow(bool state);
    void refreshPanel();

protected:
    Byte efState_[5];
    Byte pioEfState_[3][5];
    ElfConfiguration Cdp18s600Configuration;
    
    wxString computerTypeStr_;
    wxString pioMessage_;
    double Cdp18s600ClockSpeed_;
    Vt100 *vtPointer1;
    Vt100 *vtPointer2;
    int ioGroup_;
    class PioFrame *pioFramePointer;
    class PioFrame *pioFramePointer1;
    class PioFrame *pioFramePointer2;
    class Cdp1852Frame *cdp1852FramePointer;
    int ledCycleValue_;
    int ledCycleSize_;
    class Cdp18s640Frame *cdp18s640FramePointer;
    
    int microboardType_;
    Byte keyboardEf3_;
    Byte printEf_;
    bool keyDown_;

    int addressLatchCounter_;

private:
    Word lastAddress_;
    
    int setMsValue_;
    int cdpRunState_;
    bool saveStarted_;
    bool loadStarted_;
    bool microDosRunning_;
    bool resetHdData_;

    wxString tapeNumber_;

    int keyboardCode_;
    int secondKeyboardCodes[5];

    bool load_;
    size_t microRunCommand_;
    wxString commandText_;
};

class Cdp18s601 : public Cdp18s600
{
public:
    Cdp18s601(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock, ElfConfiguration conf);

    virtual void configurePio();
    Byte in(Byte port, Word address);
    void out(Byte port, Word address, Byte value);
    void readRoms();

private:

};

class Cdp18s602 : public Cdp18s600
{
public:
    Cdp18s602(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock, ElfConfiguration conf);
    ~Cdp18s602();
    
    void configurePio();
    void startPio(long ms);
    Byte ef(int flag);
    Byte in(Byte port, Word address);
    void out(Byte port, Word address, Byte value);
    void readRoms();
    
    void setCpuMode(int mode);
    void cycle(int type);
    void cycleLed();
    void setLedMs(long ms);
    void releaseButtonOnScreen2(HexButton* buttonPointer, int buttonType, int pioNumber);
    void moveWindows();
    void showPio(bool state);
    
private:
};

class Cdp18s603a : public Cdp18s601
{
public:
    Cdp18s603a(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock, ElfConfiguration conf);
    
    void readRoms();
    
private:
    
};

class Cdp18s604b : public Cdp18s600
{
public:
    Cdp18s604b(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock, ElfConfiguration conf);
    ~Cdp18s604b();
    
    void configurePio();
    void startPio(long ms);
    Byte ef(int flag);
    Byte in(Byte port, Word address);
    void out(Byte port, Word address, Byte value);
    void counterControlByte(Byte value);
    void startComputer();
    void readRoms();
    
    void writeMemDataType(Word address, Byte type);
    Byte readMemDataType(Word address, uint64_t* executed);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
    void setCpuMode(int mode);
    void cycle(int type);
    void cycleLed();
    void setLedMs(long ms);
    void releaseButtonOnScreen2(HexButton* buttonPointer, int buttonType, int pioNumber);
    void moveWindows();
    void showPio(bool state);

private:
    int counterCycleValue_;
    int counterCycleSize_;
    bool counterPaused_;
    bool counterSquare_;
};

class Rcasbc : public Cdp18s600
{
public:
    Rcasbc(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock, ElfConfiguration conf);
    ~Rcasbc();
    
    void configureVt();
    void configureComputer();
    void configurePio();
    Byte ef(int flag);
    Byte in(Byte port, Word address);
    void out(Byte port, Word address, Byte value);
    void startComputer();

    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);

    void readRoms();
    void autoBoot();
    void dataAvailableVt100(bool data, int uartNumber);
    void dataAvailable2Vt100(bool data);
    void dataAvailableSerial(bool data);
    void updateTitle(wxString Title);

private:
    bool uart1Reset_;
    int uart1ModeWordNumber_;
    bool uart2Reset_;
    int uart2ModeWordNumber_;
    
    Byte lastUart1In_;
    Byte lastUart2In_;
};


#endif  // CDP18S600_H

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

class Cdp18s600 : public wxFrame, public Cdp1802
{
public:
    Cdp18s600(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);
    Cdp18s600() {};
    ~Cdp18s600();
    
    void onClose(wxCloseEvent&WXUNUSED(event));
    
    virtual void configureComputer();
    void initComputer();
    
    void onRunButton(wxCommandEvent&event);
    void onRunPButton(wxCommandEvent&event);
    void onResetButton(wxCommandEvent&event);
    void onRunButton();
    void onRunPButton();
    void onRun();
    void autoBoot();
    void setAddressLatch(Word bootAddress);
    void onSingleStep();

    Byte ef(int flag);
    void setEfState(int number, Byte value);
    virtual Byte in(Byte port, Word address);
    virtual void out(Byte port, Word address, Byte value);
    void switchQ(int value);
    void showCycleData(Byte val);
    void showAddress(Word val);
    void showState(int state);
    void setCpuMode(int mode);
    void cycle(int type);
    void cycleLed();

    void startComputer();
    virtual void readRoms();
    void readMicro(int romNumber, Word startAddress, Word lastAddress);
    void writeMemDataType(Word address, Byte type);
    Byte readMemDataType(Word address);
    Byte readMem(Word address);
    void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
    void cpuInstruction();
    void resetPressed();
    void moveWindows();
    void setForceUpperCase(bool status);
    void setBootRam(bool status);
    void updateTitle(wxString Title);
    void onReset();
    void sleepComputer(long ms);
    void setLedMs(long ms);
    void startComputerRun(bool load);
    int getRunState() {return mcdsRunState_;};
    bool isComputerRunning();
    void checkComputerFunction();
    
    void setCdp18s600ClockSpeed(double clock) {Cdp18s600ClockSpeed_ = clock;};
    void releaseButtonOnScreen(HexButton* buttonPointer, int buttonType);
    void releaseButtonOnScreen2(HexButton* buttonPointer, int buttonType);
    void activateMainWindow();
	void showPio(bool state);
    void removePio();

protected:
    Byte efState_[5];
    ElfConfiguration Cdp18s600Configuration;
    wxString computerTypeStr_;
    double Cdp18s600ClockSpeed_;
    Vt100 *vtPointer;
    int ioGroup_;
    class PioFrame *pioFramePointer;

private:
    Word lastAddress_;
    
    int cycleValue_;
    int cycleSize_;
   
    int ledCycleValue_;
    int ledCycleSize_;
    int setMsValue_;

    int mcdsRunState_;
    
    class Cdp18s640Screen *cdp18s640ScreenPointer;
    
    DECLARE_EVENT_TABLE()
};

class Cdp18s601 : public Cdp18s600
{
public:
    Cdp18s601(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);

    void configureComputer();
    Byte in(Byte port, Word address);
    void out(Byte port, Word address, Byte value);
    void readRoms();

private:

};

class Cdp18s603a : public Cdp18s600
{
public:
    Cdp18s603a(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);
    
    void configureComputer();
    Byte in(Byte port, Word address);
    void out(Byte port, Word address, Byte value);
    void readRoms();
    
private:
    
};


#endif  // CDP18S600_H

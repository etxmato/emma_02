#ifndef CDP18S600_H
#define CDP18S600_H

#include "cdp1802.h"
#include "vt100.h"
#include "serial.h"
#include "upd765.h"
#include "elfconfiguration.h"

#define IO_GRP1_UART 1
#define IO_GRP2_UART 2

class Cdp18s600 : public wxFrame, public Cdp1802
{
public:
    Cdp18s600(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);
    Cdp18s600() {};
    ~Cdp18s600();
    
    void onClose(wxCloseEvent&WXUNUSED(event));
    
    void configureComputer();
    void initComputer();
    Byte ef(int flag);
    Byte in(Byte port, Word address);
    void out(Byte port, Word address, Byte value);
    void cycle(int type);
    
    void startComputer();
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
    void startComputerRun(bool load);
    int getRunState() {return mcdsRunState_;};
    bool isComputerRunning();
    void checkComputerFunction();
    
    void setCdp18s600ClockSpeed(double clock) {Cdp18s600ClockSpeed_ = clock;};
    void activateMainWindow();
    void switchQ(int value);
    void setAddressLatch(Word bootAddress);

private:
    Vt100 *vtPointer;
    
    ElfConfiguration Cdp18s600Configuration;
    
    double Cdp18s600ClockSpeed_;
    Word lastAddress_;
    
    int ioGroup_;
    
    int cycleValue_;
    int cycleSize_;
    
    int mcdsRunState_;
};

#endif  // CDP18S600_H

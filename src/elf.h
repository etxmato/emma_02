#ifndef MAINELF_H
#define MAINELF_H

#include "cdp1802.h"
#include "fdc.h"
#include "ide.h"
#include "keyboard.h"
#include "ps2gpio.h"
#include "portext.h"
#include "printer.h"
#include "ps2.h"
#include "elf2kdisk.h"
#include "vt100.h"
#include "serial.h"

class MainElf : public Cdp1802, public Fdc, public Ide, public Keyboard, public PortExt, public Ps2, public Ps2gpio, public Elf2KDisk
{			
public:
	MainElf();
	~MainElf();

	void checkComputerFunction();
    void activateElfOsChip8();
    void fetchFileName(Word address, size_t length);

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
    void thrStatus(bool data);
    void saveRtc();
    void loadRtc();
    Byte readDirectRtc(Word address);
    void writeDirectRtc(Word address, Byte value);

protected:
	Vt100 *vtPointer;
    
	int elfRunState_;
    int cycleValue_;
    int cycleSize_;
    double elfClockSpeed_;

    wxString runningGame_;

    wxTimer *rtcTimerPointer;
    int rtcCycle_;

private:

};

#endif  // MAINELF_H

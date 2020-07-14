#ifndef MS2000_H
#define MS2000_H

#include "cdp1802.h"
#include "vt100.h"
#include "serial.h"
#include "upd765.h"
#include "elfconfiguration.h"

#define IO_GRP_UART 1
#define IO_GRP_TAPE 2 // OUT 5 = Q, OUT 4 0=STOP TAPE and 81=START TAPE
#define IO_GRP_UPD 8

DECLARE_EVENT_TYPE(ON_UART_MS2000, 809)

class Ms2000 : public wxFrame, public Upd765, public Cdp1802
{
public:
	Ms2000(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);
	Ms2000() {};
	~Ms2000();

	void onClose(wxCloseEvent&WXUNUSED(event));

	void configureComputer();
	void initComputer();
	Byte ef(int flag);
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void cycle(int type);

	void startComputer();
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
    void checkComputerFunction();

	void setMs2000ClockSpeed(double clock) {ms2000ClockSpeed_ = clock;};
	void activateMainWindow();
    void switchQ(int value);

private:
	Vt100 *vtPointer;

	ElfConfiguration ms2000Configuration;

	double ms2000ClockSpeed_;
	Word lastAddress_;

    int ioGroup_;

    bool saveStarted_;
    bool loadStarted_;
	bool microDosRunning_;
    bool resetHdData_;
    
};

#endif  // MS2000_H

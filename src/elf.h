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
#include "vt100.h"
#include "serial.h"

class MainElf : public Cdp1802, public Fdc, public Ide, public Keyboard, public PortExt, public Ps2, public Ps2gpio
{			
public:
	MainElf();
	~MainElf();

	void checkElfFunction();
    void activateElfOsChip8();
    void fetchFileName(Word address, size_t length);

	void startComputerRun(bool load);
	int getRunState() {return elfRunState_;};
	bool isComputerRunning();
    wxString getRunningGame(){return runningGame_;};

protected:
	Vt100 *vtPointer;
    
	int elfRunState_;

    wxString runningGame_;

private:

};

#endif  // MAINELF_H

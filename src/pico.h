#ifndef PICO_H
#define PICO_H

#include "elf.h"
#include "tms9918.h"
#include "pixie.h"
#include "mc6847.h"
#include "mc6845.h"
#include "i8275.h"
#include "ledmodule.h"
#include "printer.h"
#include "elfconfiguration.h"

class Pico : public wxFrame, public MainElf
{
public:
	Pico(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf, Conf computerConf);
	Pico() {};
	~Pico();

	void onClose(wxCloseEvent&WXUNUSED(event));
	void charEvent(int keycode);
	bool keyDownPressed(int keycode);
	bool keyUpReleased(int keycode);

	void configureComputer();
    void setPrinterEf();
	void initComputer();
	Byte ef(int flag);
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void cycle(int type);
    void cycleLed();

	void switchQ(int value);

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address, uint64_t* executed);
	Byte readMem(Word address);
	void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
	void cpuInstruction();
    void resetPressed();
	void configureElfExtensions();
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
	void activateMainWindow();
    void OnRtcTimer(wxTimerEvent& event);

private:
    Tms9918 *tmsPointer;
	Pixie *pixiePointer;
	MC6845 *mc6845Pointer;
	mc6847 *mc6847Pointer;
	i8275 *i8275Pointer;

	Byte ef4State_;
	Byte ef3State_;
    Byte ef3Button_;
    Byte ef4Button_;
	
	long offset_;

	DECLARE_EVENT_TABLE()
};

#endif  // PICO_H

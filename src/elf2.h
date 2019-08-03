#ifndef ELF2_H
#define ELF2_H

#include "elf.h"
#include "tms9918.h"
#include "pixie.h"
#include "mc6847.h"
#include "mc6845.h"
#include "i8275.h"
#include "ledmodule.h"
#include "printer.h"
#include "elfconfiguration.h"

class Elf2Screen : public Panel
{
public:
	Elf2Screen(wxWindow *parent, const wxSize& size, int tilType);
	~Elf2Screen();

	void init();
	void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
	void onMouseRelease(wxMouseEvent& event);
    void releaseButtonOnScreen(HexButton* buttonPointer);

private:
};

class Elf2 : public wxFrame, public MainElf 
{
public:
	Elf2(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);
	Elf2() {};
	~Elf2();

	void onClose(wxCloseEvent&WXUNUSED(event));
	void charEvent(int keycode);
	bool keyDownPressed(int keycode);
	bool keyUpReleased(int keycode);

	void onRun();
	void onButtonRelease(wxCommandEvent& event);
	void onButtonPress(wxCommandEvent& event);
	void onInButtonPress();
	void onInButtonRelease();
	void onHexKeyDown(int keycode);
	void onHexKeyUp(int keycode);

	void configureComputer();
    void switchHexEf(bool state);
    void setPrinterEf();
    void reLoadKeyDefinition(wxString fileName);
	void reDefineKeysA(int *, int *);
	void initComputer();
	Byte ef(int flag);
	Byte in(Byte port, Word address);
	Byte getData();
	void out(Byte port, Word address, Byte value);
	void cycle(int type);
    void cycleLed();
	void showData(Byte value);

	void autoBoot();
	void switchQ(int value);
	int getMpButtonState();
	void onMpButton();
	void onLoadButton();
    void onNumberKeyDown(int i);
	void onNumberKeyDown(wxCommandEvent& event);
    void onNumberKeyUp(wxCommandEvent& event);
    void onNumberKeyUp();

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address);
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
	void setLedMs(long ms);
	Byte getKey(Byte vtOut);
	void activateMainWindow();
    void releaseButtonOnScreen(HexButton* buttonPointer, int buttonType);

private:
	class Elf2Screen *elf2ScreenPointer;

	Tms9918 *tmsPointer;
	Pixie *pixiePointer;
	MC6845 *mc6845Pointer;
	mc6847 *mc6847Pointer;
	i8275 *i8275Pointer;

    int ledCycleValue_;
    int ledCycleSize_;

    Byte switches_;
	int runButtonState_;
	int mpButtonState_;
	int loadButtonState_;
	Byte ef4State_;
	Byte ef3State_;
    Byte ef3Button_;
    Byte ef4Button_;

    int keyDefA1_[16];
    int keyDefB1_[16];
    int keyDefA2_[16];
    int keyDefB2_[16];
    
    bool simDefA2_;
    bool simDefB2_;
    
	int keyDefGameHexA_[5];
	int keyDefGameHexB_[5];
	
	double elfClockSpeed_;

	long offset_;

	DECLARE_EVENT_TABLE()
};

#endif  // ELF2_H

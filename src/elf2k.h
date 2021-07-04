#ifndef ELF2K_H
#define ELF2K_H

#include "elf2kswitch.h"
#include "elf2khex.h"
#include "cdp1802.h"
#include "elf2kdisk.h"
#include "tms9918.h"
#include "pixie.h"
#include "mc6847.h"
#include "mc6845.h"
#include "keyboard.h"
#include "keypad.h"
#include "ps2gpio.h"
#include "ledmodule.h"
#include "vt100.h"
#include "serial.h"
#include "portext.h"
#include "printer.h"
#include "ps2.h"

DECLARE_EVENT_TYPE(ON_UART_ELF2K, 808) 

class Elf2KScreen : public Panel
{
public:
	Elf2KScreen(wxWindow *parent, const wxSize& size);
	~Elf2KScreen();

	void init();
	void onPaint(wxPaintEvent&event);
	void onMouseRelease(wxMouseEvent& event);

private:
};

class Elf2K : public wxFrame, public Cdp1802, public Elf2KDisk, public Ps2gpio
{
public:
	Elf2K(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);
	Elf2K() {};
	~Elf2K();

	void onClose(wxCloseEvent&event);
	bool keyDownPressed(int keycode);
	bool keyUpReleased(int keycode);
	void charEvent(int keycode);

	void onLoadButton();
	void onMpButton();
	void dataSwitch(int number);
	Byte getData();

	void onRun();
	void autoBoot();
	Byte inPressed();
	void onInButtonPress(Byte value);
	void onInButtonRelease();
	void onInButtonPress();
    void onElf2KInButton();
	void checkHexKey(int keycode);
	void onHexKeyDown(int hexkey);
	void onHexKeyUp(int keycode);

	void configureComputer();
	void reDefineKeysA(int *, int *);
	void initComputer();
	Byte ef(int flag);
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void showData(Byte value);
	void cycle(int type);
	void cycleElf2K();

	void resetComputer();
	void resetVideo();
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
	Byte read8275CharRom(Word address);
	void write8275CharRom(Word address, Byte value);
    Byte read8275VideoRam(Word address);
    void write8275VideoRam(Word address, Byte value);
	void OnRtcTimer(wxTimerEvent& event);
	void saveRam();
	void loadRam();
	void saveRtc();
	void loadRtc();

	void removeElf2KSwitch();
	void removeElfHex();
	void showModules(bool useSwitch, bool useHex);
	void clearBootstrap() {bootstrap_ = 0;};
	void setDivider(Byte value);
	void setElf2KClockSpeed(double clock) {elfClockSpeed_ = clock;};
    void dataAvailableVt100(bool data, int uartNumber);
    void dataAvailableSerial(bool data);
	void thrStatus(bool data);
    void sleepComputer(long ms);
	void setLedMs(long ms);
	Byte getKey(Byte vtOut);
	void activateMainWindow();
	void releaseButtonOnScreen(HexButton* buttonPointer, int buttonType);
    void onNumberKeyDown(int i);
    void refreshPanel();
    void terminalSave(wxString fileName, int protocol);
    void terminalLoad(wxString filePath, wxString fileName, int protocol);
    void terminalStop();
    void checkComputerFunction();
    void activateElfOsChip8();
    void fetchFileName(Word address, size_t length);

private:
	class Elf2KScreen *elf2KScreenPointer;
	Pixie *pixiePointer;
	i8275 *i8275Pointer;
	Vt100 *vtPointer;

    wxTimer *rtcTimerPointer;
    int rtcCycle_;

	Elf2Kswitch *p_Elf2Kswitch;
	Elf2Khex *p_Elf2Khex;

	Byte ef3State_;
	Byte switches_;
	Byte data_;
	Byte lastMode_;

    int keyDefA1_[16];
    int keyDefA2_[16];
    
	double elfClockSpeed_;
	Word lastAddress_;

	int cycleValue_;
    int cycleSize_;
    int ledCycleValue_;
    int ledCycleSize_;

	int runButtonState_;
	bool inPressed_;
	int loadButtonState_;
	int mpButtonState_;
	int dataSwitchState_[8];

	DECLARE_EVENT_TABLE()
};

#endif  // ELF2K_H

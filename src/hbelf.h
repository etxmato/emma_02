#ifndef ELF_H
#define ELF_H

#include <wx/dcclient.h> 

#include "elf.h"
#include "tms9918.h"
#include "pixie.h"
#include "mc6847.h"
#include "mc6845.h"
#include "i8275.h"
#include "keypad.h"
#include "ledmodule.h"
#include "printer.h"
#include "elfconfiguration.h"

class ElfScreen : public Panel
{
public:
	ElfScreen(wxWindow *parent, const wxSize& size, int tilType);
	~ElfScreen();

	void init();
	void onPaint(wxPaintEvent&event);
	void onMousePress(wxMouseEvent& event);
	void onMouseRelease(wxMouseEvent& event);
private:

};

class Elf : public wxFrame,  public MainElf
{
public:
	Elf(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf, Conf computerConf);
	Elf() {};
	~Elf();

	void onClose(wxCloseEvent&WXUNUSED(event));
	bool keyDownPressed(int keycode);
	bool keyUpReleased(int keycode);
	void charEvent(int keycode);

	void onRun();
	void onInButtonPress();
	void onInButtonPress(Byte value);
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
	Byte ef(int flag);
	Byte in(Byte port, Word address);
	Byte getData();
	void out(Byte port, Word address, Byte value);
	void showData(Byte value);
	void cycle(int type);
    void cycleLed();

	void autoBoot();
	void switchQ(int value);
	int getMpButtonState();
	void onMpButton();
	void onLoadButton();
	void dataSwitch(int i);
	void efSwitch(int i);

	void startComputer();
	void removeElfLedModule() {ledModuleClosed_ = true;};
	void showModules(bool status);
	void removeElfHex() {hexKeypadClosed_ = true;};
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
    void pauseComputer();
    void resumeComputer();

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
    void onNumberKeyDown(int i);
    void onNumberKeyUp();
    void refreshPanel();
    void OnRtcTimer(wxTimerEvent& event);

private:
	class ElfScreen *elfScreenPointer;

	Tms9918 *tmsPointer;
	Pixie *pixiePointer;
	MC6845 *mc6845Pointer;
	mc6847 *mc6847Pointer;
	i8275 *i8275Pointer;
	Keypad *keypadPointer;
	LedModule *ledModulePointer;

    int ledCycleValue_;
    int ledCycleSize_;
    
	int runButtonState_;
	int mpButtonState_;
	int loadButtonState_;
	int dataSwitchState_[8];
	int efSwitchState_[4];

	bool inPressed_;
	Byte ef3State_;

	Byte switches_;
	Byte data_;
	Word lastAddress_;

	bool ledModuleClosed_;
	bool hexKeypadClosed_;

    int keyDefA1_[16];
    int keyDefB1_[16];
    int keyDefA2_[16];
    int keyDefB2_[16];
 
    bool simDefA2_;
    bool simDefB2_;
    
	int keyDefGameHexA_[5];
	int keyDefGameHexB_[5];

	DECLARE_EVENT_TABLE()
};

#endif  // ELF_H

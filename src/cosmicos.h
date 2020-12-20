#ifndef COSMICOS_H
#define COSMICOS_H

#include "til313.h"
#include "cosmicoshex.h"
#include "led.h"
#include "cdp1802.h"
#include "pixie.h"
#include "keyboard.h"
#include "keypad.h"
#include "vt100.h"
#include "serial.h"
#include "printer.h"
#include "elfconfiguration.h"

class CosmicosScreen : public Panel
{
public:
	CosmicosScreen(wxWindow *parent, const wxSize& size);
	~CosmicosScreen();

	void init();
	void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
    void releaseButtonOnScreen(HexButton* buttonPointer);
    
private:
};

class Cosmicos : public wxFrame, public Cdp1802
{
public:
	Cosmicos(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);
	Cosmicos() {};
	~Cosmicos();

	void onClose(wxCloseEvent&WXUNUSED(event));
	bool keyDownPressed(int keycode);
	bool keyUpReleased(int keycode);
	void onButtonRelease(wxCommandEvent& event);
	void onButtonPress(wxCommandEvent& event);

    void onNumberKeyPress(int key);
    void onNumberKeyRelease(int key);
    void onSingleStep(wxCommandEvent&WXUNUSED(event));
    void onSingleStep();
    void onPause(wxCommandEvent&WXUNUSED(event));
    void onPause();
    void onResetButton(wxCommandEvent&WXUNUSED(event));
    void onResetButton();
    void onRunButton(wxCommandEvent& event);
    void onRunButton();
    void onLoadButton(wxCommandEvent& event);
    void onLoadButton();
    void onClearButton(wxCommandEvent& event);
    void onClearButton();
	void onMpButton();
	void onRamButton();
	void onMouseRelease(wxMouseEvent& event);
	Byte getData();

    void dataButton(wxCommandEvent& event);
    void dataButton(int i);

	void onRun();
	void autoBoot();
	Byte inPressed();
	void onInButtonPress(Byte value);
	void onInButtonRelease();
	void onInButtonPress();
	void onHexKeyDown(int keycode);
	void onHexKeyUp(int keycode);

	void configureComputer();
	void reDefineKeysA(int *, int *);
	void initComputer();
	Byte ef(int flag);
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void switchQ(int value);
	void showData(Byte value);
	void cycle(int type);
    void cycleLed();

	void onPowerButton(wxCommandEvent& event);

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
	void checkComputerFunction();

	void removeCosmicosHex();
	void showModules(bool useHex);
	void setCosmicosClockSpeed(double clock) {cosmicosClockSpeed_ = clock;};
	void saveRam();
	void loadRam();
	void setLedMs(long ms);
	Byte getKey(Byte vtOut);
	void activateMainWindow();
    void releaseButtonOnScreen(HexButton* buttonPointer, int buttonType);
    void refreshPanel();

private:
	class CosmicosScreen *cosmicosScreenPointer;

	Pixie *pixiePointer;
	Vt100 *vtPointer;

	Cosmicoshex *p_Cosmicoshex;
	Keypad *keypadPointer;

	ElfConfiguration cosmicosConfiguration;

	int qState_;
	Byte switches_;
	Byte data_;
	Byte lastMode_;

    int keyDefA1_[16];
    int keyDefB1_[16];
    int keyDefA2_[16];
    int keyDefB2_[16];
    
	bool simDefA2_;
    bool simDefB2_;

	int keyDefGameHexA_[5];
	int keyDefGameHexB_[5];

	double cosmicosClockSpeed_;
	Word lastAddress_;

	int runButtonState_;
	bool inPressed_;
	int loadButtonState_;
	int mpButtonState_;
	int ramButtonState_;
	int dataSwitchState_[8];
	int segNumber_;

	Word mainRamAddress_;

	int cycleValue_;
	int cycleSize_;
    int ledCycleValue_;
    int ledCycleSize_;
	bool pixieOn_;

	DECLARE_EVENT_TABLE()
};

#endif  // COSMICOS_H

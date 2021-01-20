#ifndef Uc1800_H
#define Uc1800_H
    
#include "til311.h"
#include "cdp1802.h"
#include "elfconfiguration.h"

class Uc1800Screen : public Panel
{
public:
	Uc1800Screen(wxWindow *parent, const wxSize& size);
	~Uc1800Screen();

	void init(bool powerButtonState);
	void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
   	void releaseButtonOnScreen(HexButton* buttonPointer);
 
private:
};

class Uc1800 : public wxFrame, public Cdp1802
{
public:
	Uc1800(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);
	Uc1800() {};
	~Uc1800();

	void onClose(wxCloseEvent&WXUNUSED(event));
    void resumeComputer();

    void onRunButtonPress();
    void onRunButtonRelease();
    void onLoadButton();
    void onResetButtonPress();
    void onResetButtonRelease();
	Byte getData();

	bool keyDownPressed(int keycode);
	bool keyUpReleased(int keycode);

	void onRun();
	void onSingleStep();
	void onPowerButton();
	void powerSave();
	void powerLoad();
	void saveRam();
	void loadRam();
	void autoBoot();
	void onInButtonPress();
	void onInButtonPress(Byte value);
	void onInButtonRelease();

	void reDefineKeysA(int *, int *);

    void configureComputer();
	void initComputer();
	Byte ef(int flag);
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void switchQ(int value);
	void showData(Byte value);
    void showCycleData(Byte val);
	void cycle(int type);
    void cycleLed();

	void onPowerButton(wxCommandEvent& event);
	void onNumberKeyDown(wxCommandEvent& event);
	void onNumberKeyDown(int i);
    void onNumberKeyUp(wxCommandEvent& event);
    void onNumberKeyUp();
	void onHexKeyDown(int keycode);
	void onHexKeyUp(int keycode);

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address);
	Byte readMem(Word address);
	void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
	void cpuInstruction();
    void resetPressed();
	void onReset();

	void setUc1800ClockSpeed(double clock) {uc1800ClockSpeed_ = clock;};
	void setLedMs(long ms);
	void activateMainWindow();
    void releaseButtonOnScreen(HexButton* buttonPointer, int buttonType);
    void refreshPanel();

private:
	class Uc1800Screen *uc1800ScreenPointer;

	ElfConfiguration uc1800Configuration;

    int ledCycleValue_;
    int ledCycleSize_;
    int setMsValue_;

    Byte keypadValue_;
	char nextNybble_;

	Byte data_;
	Byte ef1State_;
	Byte ef3State_;

	int keyDefA1_[16];
    int keyDefA2_[16];
        
	int keyDefGameHexA_[5];

	double uc1800ClockSpeed_;

	int loadButtonState_;
	bool powerButtonState_;

	Byte lastMode_;

	DECLARE_EVENT_TABLE()
};

#endif  // Uc1800_H

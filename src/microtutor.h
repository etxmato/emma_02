#ifndef MICROTUTOR_H
#define MICROTUTOR_H

#include "til311.h"
#include "cdp1802.h"
#include "elfconfiguration.h"

class MicrotutorScreen : public Panel
{
public:
	MicrotutorScreen(wxWindow *parent, const wxSize& size);
	~MicrotutorScreen();

	void init();
	void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
    
private:
};

class Microtutor : public wxFrame, public Cdp1802
{
public:
	Microtutor(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);
	Microtutor() {};
	~Microtutor();

	void onClose(wxCloseEvent&WXUNUSED(event));
	bool keyUpReleased(int keycode);

    void onRunButtonPress();
    void onRunButtonRelease();
    void onLoadButton();
    void dataSwitch(int i);
    void onClearButtonPress();
    void onClearButtonRelease();
	Byte getData();

    void onRun();
	void autoBoot();
	Byte inPressed();
	void onInButtonPress(Byte value);
	void onInButtonRelease();

    void configureComputer();
	void initComputer();
	Byte ef(int flag);
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void showData(Byte value);
	void cycle(int type);
    void cycleLed();

	void onPowerButton(wxCommandEvent& event);

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address);
	Byte readMem(Word addr);
	void writeMem(Word addr, Byte value, bool writeRom);
	void cpuInstruction();
	void onReset();

	void setMicrotutorClockSpeed(double clock) {microtutorClockSpeed_ = clock;};
	void setLedMs(long ms);
	void activateMainWindow();

private:
	class MicrotutorScreen *microtutorScreenPointer;

	ElfConfiguration microtutorConfiguration;

    int ledCycleValue_;
    int ledCycleSize_;

    Byte switches_;
	Byte data_;

	double microtutorClockSpeed_;

	bool inPressed_;
	int loadButtonState_;
	int dataSwitchState_[8];

	DECLARE_EVENT_TABLE()
};

#endif  // MICROTUTOR_H

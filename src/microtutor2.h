#ifndef MICROTUTOR2_H
#define MICROTUTOR2_H

#include "til313.h"
#include "cdp1802.h"
#include "elfconfiguration.h"

class Microtutor2Screen : public Panel
{
public:
	Microtutor2Screen(wxWindow *parent, const wxSize& size);
	~Microtutor2Screen();

	void init();
	void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
    
private:
};

class Microtutor2 : public wxFrame, public Cdp1802
{
public:
	Microtutor2(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);
	Microtutor2() {};
	~Microtutor2();

	void onClose(wxCloseEvent&WXUNUSED(event));
	bool keyUpReleased(int keycode);

    void onRunButton();
    void onLoadButton();
    void dataSwitch(int i);
    void onMpButton();
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
    void switchQ(int value);
	void cycle(int type);
    void cycleLed();

	void onPowerButton(wxCommandEvent& event);

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address, uint64_t* executed);
	Byte readMem(Word address);
	void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
	void cpuInstruction();
    void resetPressed();
	void onReset();

	void setMicrotutorClockSpeed(double clock) {microtutorClockSpeed_ = clock;};
	void setLedMs(long ms);
	void activateMainWindow();
    void refreshPanel();

private:
	class Microtutor2Screen *microtutorScreenPointer;

	ElfConfiguration microtutorConfiguration;

    int ledCycleValue_;
    int ledCycleSize_;

    Byte switches_;
	Byte data_;

	double microtutorClockSpeed_;

	bool inPressed_;
	int loadButtonState_;
    int mpButtonState_;
    int runButtonState_;
	int dataSwitchState_[8];

	DECLARE_EVENT_TABLE()
};

#endif  // MICROTUTOR2_H

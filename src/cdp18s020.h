#ifndef CDP18S020_H
#define CDP18S020_H

#include "cdp1802.h"
#include "pixie.h"
#include "keyboard.h"
#include "elfconfiguration.h"
#include "vt100.h"

class Cdp18s020Screen : public Panel
{
public:
    Cdp18s020Screen(wxWindow *parent, const wxSize& size);
    ~Cdp18s020Screen();
    
    void init();
    void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
    void releaseButtonOnScreen(HexButton* buttonPointer);
private:
};

class Cdp18s020 : public wxFrame, public Cdp1802
{
public:
	Cdp18s020(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);
	~Cdp18s020();

    void onClose(wxCloseEvent&WXUNUSED(event));

    void configureComputer();
	void configureKeyboard();
	void initComputer();

    void onRunButton(wxCommandEvent&event);
    void onRunPButton(wxCommandEvent&event);
    void onRunButton();
    void onRunPButton();
    void onRun();
    void autoBoot();
    
    void onMpButton(int buttonNumber);
    void onSingleStep();

	Byte ef(int flag);
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void switchQ(int value);
    void showCycleData(Byte val);
    void showAddress(Word val);
    void showState(int state);
    void setCpuMode(int mode);
	void cycle(int type);
    void cycleLed();

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
	void updateTitle(wxString Title);
    void onResetButton(wxCommandEvent&event);
	void onReset();
	void sleepComputer(long ms);

    void setLedMs(long ms);

    void terminalSave(wxString fileName);
    void terminalLoad(wxString filePath, wxString fileName, bool binaryFile);
    void terminalStop();
    
    void releaseButtonOnScreen(HexButton* buttonPointer, int buttonType);
    void setAddressLatch(Word bootAddress);
    void setForceUpperCase(bool status);
    void refreshPanel();

private:
	ElfConfiguration cdp18s020Configuration;
	Vt100 *vtPointer;

    int ledCycleValue_;
    int ledCycleSize_;
    int setMsValue_;

    Byte ef4State_;
    bool mpButtonState_[4];
    
    double cdp18s020ClockSpeed_;
    class Cdp18s020Screen *cdp18s020ScreenPointer;

    int addressLatchCounter_;
    
    DECLARE_EVENT_TABLE()
};

#endif  // CDP18S020_H

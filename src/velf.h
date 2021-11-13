#ifndef VELF_H
#define VELF_H

#include "cdp1802.h"
#include "pixie.h"
#include "keyboard.h"
#include "elfconfiguration.h"

class VelfScreen : public Panel
{
public:
    VelfScreen(wxWindow *parent, const wxSize& size);
    ~VelfScreen();
    
    void init();
    void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
private:
    int dataSwitchState_[8];   
};

class Velf : public wxFrame, public Cdp1802
{
public:
	Velf(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);
	~Velf();

    void onClose(wxCloseEvent&WXUNUSED(event));

    void configureComputer();
	void configureKeyboard();
	void initComputer();
	void keyDown(int keycode);
	void keyUp(int keycode);
    void onInButtonPress();
    void onInButtonRelease();

    void onRun();
    void autoBoot();

	Byte ef(int flag);
    Byte ef3();
	Byte in(Byte port, Word address);
    Byte getData();
	void out(Byte port, Word address, Byte value);
	void outVelf(Byte value);
	void switchQ(int value);
	void cycle(int type);
    void showData(Byte value);
	void cycleKey();
    void cycleLed();

    int getMpButtonState();
    void onMpButton();
    void onLoadButton();
    void dataSwitch(int i);

    void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address, uint64_t* executed);
	Byte readMem(Word address);
	void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
	void cpuInstruction();
    void resetPressed();
	void moveWindows();
	void updateTitle(wxString Title);
	void onReset();
    void checkComputerFunction();
	void startComputerRun(bool load);
	void sleepComputer(long ms);

	void defineKeys();
	void reDefineKeysA(int *, int *);

    void setLedMs(long ms);
    void refreshPanel();
    
private:
	ElfConfiguration vipConfiguration;
	Vt100 *vtPointer;

    int ledCycleValue_;
    int ledCycleSize_;

    int runButtonState_;
    int mpButtonState_;
    int loadButtonState_;
    int dataSwitchState_[8];

    Byte vipKeyPort_;
	Byte vipKeyState_[2][16];
	bool runPressed_;
    Byte ef4State_;

	int keyDefA1_[16];
	int keyDefB1_[16];
	int keyDefA2_[16];
	int keyDefB2_[16];

	bool simDefA2_;
	bool simDefB2_;
	
	int keyDefGameHexA_[5];
	int keyDefGameHexB_[5];

	bool redefineKeys_;

	Byte printLatch_;
	int stateQ_;
	bool usePrinter_;
	bool player2defined_;

	KeyDef keyDefinition[512];

	size_t vipRunCommand_;
	wxString commandText_; 
	int vipRunState_;
	bool load_;
    
    double velfClockSpeed_;
    Pixie *pixiePointer;
    bool pixieCycle_;
    class VelfScreen *velfScreenPointer;

    DECLARE_EVENT_TABLE()
};

#endif  // VELF_H

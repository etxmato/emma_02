#ifndef VIP_H
#define VIP_H

#include "cdp1802.h"
#include "pixie.h"
#include "keyboard.h"
#include "elfconfiguration.h"

DECLARE_EVENT_TYPE(ON_UART_VIP, 810)

class Vip : public Cdp1802, public Pixie
{
public:
	Vip(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, double clock, int tempo, ElfConfiguration conf);
	~Vip();

	void configureComputer();
	void configureKeyboard();
	void initComputer();
	void charEvent(int keycode);
	void keyDown(int keycode);
	void keyUp(int keycode);

	void onRun();

	Byte ef(int flag);
	Byte ef3();
	Byte ef4();
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void outVip(Byte value);
	void switchQ(int value);
	void cycle(int type);
	void cycleVP550();
	void cycleKey();

	void startComputer();
	void setTempo(int tempo);
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
    void finishStopTape();

	Byte read1864ColorDirect(Word address);
	void write1864ColorDirect(Word address, Byte value);
	void defineKeys();
	void reDefineKeysA(int *, int *);
	void reDefineKeysB(int *, int *);

private:
	ElfConfiguration vipConfiguration;
	Vt100 *vtPointer;

	Byte vipKeyPort_;
	Byte vipKeyState_[2][16];
	bool runPressed_;

	bool cdp1862_;
	bool vp580_;
	bool vp550IntOn_;

	int keyDefA1_[16];
	int keyDefB1_[16];
	int keyDefA2_[16];
	int keyDefB2_[16];
    int inKey1_;
    int inKey2_;

	bool simDefA2_;
	bool simDefB2_;
	
	int keyDefGameHexA_[5];
	int keyDefGameHexB_[5];

	int cycleValue_;
	int cycleSize_;
	int colourMask_;
	bool useKeyboard_;
	bool redefineKeys_;

	char keyboardValue_;
	Byte keyboardEf_;
	Byte printLatch_;
	int stateQ_;
	bool usePrinter_;
	bool player2defined_;

	KeyDef keyDefinition[512];

	size_t vipRunCommand_;
	wxString commandText_; 
	int vipRunState_;
	bool load_;
    
    int tapeFinished_;
};

#endif  // VIP_H

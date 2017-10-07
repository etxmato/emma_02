#ifndef VIPII_H
#define VIPII_H

#include "cdp1802.h"
#include "pixie.h"
#include "keyboard.h"

class VipII : public Cdp1802, public Pixie
{
public:
	VipII(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, double clock, int tempo);
	~VipII();

	void configureComputer();
	void configureKeyboard();
	void initComputer();
	void charEvent(int keycode);
	void keyDown(int keycode);
	void keyUp(int keycode);

	void onRun();
	void runPressed();
	void setCycle();

	Byte ef(int flag);
	Byte ef3();
	Byte ef4();
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void outVip(Byte value);
	void switchQ(int value);
	void setLedMs(long ms);
	void cycle(int type);
	void cycleKey();
    void cycleLed();

	void startComputer();
	void setTempo(int tempo);
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address);
	Byte readMem(Word addr);
	void writeMem(Word addr, Byte value, bool writeRom);
	void cpuInstruction();
	void onReset();
	void checkVipFunction();
	void startComputerRun(bool load);
	void sleepComputer(long ms);

	Byte read1864ColorDirect(Word addr);
	void write1864ColorDirect(Word addr, Byte value);
	void defineKeys();
	void reDefineKeysA(int *, int *);
	void reDefineKeysB(int *, int *);

private:
	Byte lastMode_;
	Byte vipKeyPort_;
	Byte vipKeyState_[2][16];
	bool runPressed_;
	bool keyTimerExpired_;
	bool vipMode_;
	bool runPressedAtStartup_;

	bool cycleKeyOn_;
	bool cycleFlashOn_;
	bool flashState_;
	int cycleValue_;
	int cycleSize_;

    int ledCycleValue_;
    int ledCycleSize_;

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

	int colourMask_;
	bool redefineKeys_;

	char keyboardValue_;
	Byte keyboardEf_;
	Byte printLatch_;
	int stateQ_;
	bool updateQLed_;
	long ms_;
	bool usePrinter_;
	bool player2defined_;

	KeyDef keyDefinition[512];

	size_t vipRunCommand_;
	wxString commandText_; 
	int vipRunState_;
	bool load_;

};

#endif  // VIP_H

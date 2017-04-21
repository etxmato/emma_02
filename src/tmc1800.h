#ifndef TELMAC1800_H
#define TELMAC1800_H

#include "cdp1802.h"
#include "pixie.h"

class Tmc1800 : public Cdp1802, public Pixie
{
public:
	Tmc1800(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType);
	~Tmc1800();

	void configureComputer();
	void reDefineKeysA(int *, int *);
	void initComputer();
	void keyDown(int keycode);
	void keyUp(int keycode);

	void onRun();

	Byte ef(int flag);
	Byte ef3();
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void outTMC1800(Byte value);
	void cycle(int type);

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address);
	Byte readMem(Word addr);
	void writeMem(Word addr, Byte value, bool writeRom);
	void cpuInstruction();
	void onReset();
	void checkTMC1800Function();

private:
	Byte telmac1800KeyPort_;
	Byte telmac1800KeyState_[64];
	bool runPressed_;

	int hexKeyDefA1_[64];
    int hexKeyDefA2_[16];
    int hexKeyDefB1_[16];
    int hexKeyDefB2_[16];
    int inKey1_;
    int inKey2_;

    bool simDefA2_;
    bool simDefB2_;

	int keyDefGameHexA_[5];
	int keyDefGameHexB_[5];
};

#endif  // TELMAC1800_H

#ifndef ETI660_H
#define ETI660_H

#include "cdp1802.h"
#include "pixie.h"

class Eti : public Cdp1802, public Pixie
{
public:
	Eti(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType);
	~Eti();

	void configureComputer();
	void reDefineKeysA(int *, int *);
	void initComputer();
	void keyDown(int keycode);
	void keyUp(int keycode);

	Byte ef(int flag);
	Byte ef3();
	Byte in(Byte port, Word address);
	Byte inEti(Word address);
	void out(Byte port, Word address, Byte value);
	void outEti(Word address, Byte value);
	void cycle(int type);

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address);
	Byte readMem(Word address);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
	void writeMem(Word address, Byte value, bool writeRom);
	void cpuInstruction();
	void onReset();
	void checkEtiFunction();
	void finishStopTape();
	void keyClear();

	Byte read1864ColorDirect(Word address);
	void write1864ColorDirect(Word address, Byte value);

private:
	bool eti660KeyState_[16];
	bool colorLatch_;

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

	bool DataDirection_;
	Byte outputKeyLatch_;
	Byte inputKeyLatch_;
	Byte outputKeyValue_;
	Word endSave_;

	bool step_;
};

#endif  // ETI660_H

#ifndef CIDELSA_H
#define CIDELSA_H

#include "v1870.h"

#define ALTAIR 0
#define DESTROYER1 1
#define DESTROYER2 2
#define DRACO 3

class Cidelsa : public V1870
{
public:
	Cidelsa(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock);
	~Cidelsa();

	bool keyDownExtended(int keycode, wxKeyEvent& event);
	void keyDown(int keycode);
	void keyUp(int keycode);

	void configureComputer();
	void initComputer();
	Byte ef(int flag);
	Byte ef2();
	Byte ef3();
	Byte ef4();
	Byte in(Byte port, Word address);
	Byte in1();
	Byte in2();
	Byte in4();
	void out(Byte port, Word address, Byte value);
	void out1(Byte value);
	void cycle(int type);

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address);
	Byte readMem(Word address);
	void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
	void cpuInstruction();
	void cid1Bit8(bool set);
	void onReset();

	void setIn2Value(Byte value) {cid2_ = value;};
	void setIn2ValueDraco(Byte value) {cid2Draco_ = value;};

private:
	Byte cid1_;
	Byte cid2_;
	Byte cid2Draco_;
	Byte cid4_;
	Byte cidEF2_;
	Byte cidEF3_;
	Byte cidEF4_;

};

#endif  // CIDELSA_H

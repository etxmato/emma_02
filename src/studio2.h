#ifndef STUDIO2_H
#define STUDIO2_H

#include "cdp1802.h"
#include "pixie.h"

class KeyDef
{
public:
	bool defined;
	int player;
	int key;
};

class Studio2 : public Cdp1802, public Pixie
{
public:
	Studio2(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType);
	~Studio2();

	void configureComputer();
	void reDefineKeysA(int *, int *);
    void reDefineKeysB(int *, int *);
	void keyDown(int keycode);
	void keyUp(int keycode);

	Byte ef(int flag);
	Byte ef3();
	Byte ef4();
    void switchQ(int value);
    Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void outStudio(Byte value);
	void cycle(int type);

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address);
	Byte readMem(Word address);
	void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
	void cpuInstruction();
	void onReset();
    void checkFunction();
    int getBuildInGame(){return buildInGame_;};

    void setMultiCartLsb(Byte lsb);
    void setMultiCartMsb(Byte msb);
	void setDisableSystemRom(bool disableSystemRom) { disableSystemRom_ = disableSystemRom;};

private:
	int studioKeyPort_;
	Byte studioKeyState_[2][10];

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

	KeyDef keyDefinition[512];

    int buildInGame_;
    bool gameAuto_;
    
    bool multiCart_;
};

#endif  // STUDO2_H

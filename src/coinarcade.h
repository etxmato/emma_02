#ifndef COINARCADE_H
#define COINARCADE_H
 
#include "cdp1802.h"
#include "pixie.h"

#define PLAYER_A 0
#define PLAYER_B 1

#define KEY_UP 0
#define KEY_LEFT 1
#define KEY_RIGHT 2
#define KEY_DOWN 3
#define KEY_FIRE 4
#define KEY_COIN 5

class CoinArcade : public Cdp1802, public Pixie
{
public:
	CoinArcade(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType);
	~CoinArcade();

	void configureComputer();
	void reDefineKeys(int *, int *, int coin);
	void keyDown(int keycode);
	void keyUp(int keycode);

	Byte ef(int flag);
    Byte ef1();
    Byte ef3();
	Byte ef4();
    Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void cycle(int type);

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address);
	Byte readMem(Word addr);
	void writeMem(Word addr, Byte value, bool writeRom);
	void cpuInstruction();
	void onReset();

private:
	int keyDefA_[5];
	int keyDefB_[5];
    int keyDefCoin_;
    
    Byte directionKey_;
    Byte fireKeyA_;
    Byte fireKeyB_;
    Byte coinKey_;
};

#endif  // COINARCADE_H

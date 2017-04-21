#ifndef TMS9918_H
#define TMS9918_H

#include "video.h"

class TileList
{
public:
	wxCoord x;
	wxCoord y;
	int size;
	TileList *nextTile;
};

class Tms9918 : public Video
{
public:

	Tms9918(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock);
	~Tms9918();

	void configure(ElfPortConfiguration elfPortConf);
	void modeHighOut(Byte value);
	void modeLowOut(Byte value);
	void cycleTms();

	void setClock(double clock);
	void setCycle();
	void copyScreen();
	void drawTile(Word tile);
	void drawScreen();

	Byte getTmsMemory(int address) {return tmsMemory_[address];}
	void setTmsMemory(int address, Byte value) {tmsMemory_[address] = value;}

	void setFullScreen(bool fullScreenSet);
	void onF3();

private:
	TileList *tileListPointer;

	Byte tmsMemory_[16384];
	int computerType_;
	Byte registers_[8];
	Byte mode_;
	Word nameAddress_;
	Word colorAddress_;
	Word patternAddress_;
	Word currentAddress_;
	Byte toggle_;
	Byte value_;

	wxCoord backGroundX_;
	wxCoord backGroundY_;
	int updateTile_;


	int cycleValue_;
	int cycleSize_;
};

#endif  // TMS9918_H

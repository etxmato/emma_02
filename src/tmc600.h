#ifndef TELMAC_H
#define TELMAC_H

#include "v1870.h"

#define TELMACPRINT 1
#define TELMACTEMP 2
#define TELMACSELECT 3

class Tmc600 : public V1870
{
public:
	Tmc600(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock);
	~Tmc600();

    void reDefineKeys();
	void charEvent(int keycode);
	bool keyDownExtended(int keycode, wxKeyEvent& event);
	void keyUpExtended(int keycode, wxKeyEvent& event);
	void keyDownFile();
	void keyUpFile();
	void pressKey();
	void releaseKey();

	void onRun();

	void configureComputer();
	void initComputer();
	Byte ef(int flag);
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void cycle(int type);
	void cycleTelmac();
    int getCtrlvChar();

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address);
	Byte readMem(Word address);
	void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
	void cpuInstruction();
    void resetPressed();
	void onReset();
	void checkComputerFunction();
	void startTelmacKeyFile();
	void closeTelmacKeyFile();
	void checkCaps();
	void switchCaps();
	void sleepComputer(long ms);
	void setAds(int i, int val){adsArray_[i] = val;};
	int getAds(int i){return adsArray_[i];};
	void setAdi(int i, int val){adiArray_[i] = val;};
	int getAdi(int i){return adiArray_[i];};
	void startComputerRun(bool load);
	bool isComputerRunning();

private:
	Byte keyValue_[9];
	int adiArray_[16];
	int adsArray_[16];
    int inputPortFile[256];
    int bitNumberFile[256];
   	int keyDefNormal_[21];
   	int keyDefShift_[21];
	Byte keyLatch_;

	bool capsPressed_;
	bool capsPressedOnStart_;
    bool shiftPressed_;
	bool ctrlPressed_;
	bool altPressed_;
	bool capsCorrection_;
	Byte vismacRegisterLatch;

	bool ctrlAltLeft_;

	wxFile telmacKeyFile_;
	bool telmacKeyFileOpened_;
	bool keyDown_;
	unsigned char keyboardCode_;
	unsigned char nextKeyboardCode_;
	int cycleValue_;

	wxString commandText_; 
	size_t telmacRunCommand_; 
	int telmacRunState_;
	bool load_;

	int output3Function_;
	int inputNumber_;
	int adiValue_;
	int adiChannel_;
	int clockValue_;
	int clockSize_;
};

#endif  // TELMAC_H

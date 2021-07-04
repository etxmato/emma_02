#ifndef PECOM_H
#define PECOM_H

#include "v1870.h"

#define PECOMPRINT 1
#define PECOMTEMP 2
#define PECOMSELECT 3

class Pecom : public V1870
{
public:
	Pecom(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock);
	~Pecom();

	void charEvent(int keycode);
	bool keyDownExtended(int keycode, wxKeyEvent& event);
	void keyUpExtended(int keycode, wxKeyEvent& event);
	void keyDownFile();
	void keyUpFile();

	void configureComputer();
	void initComputer();
	Byte ef(int flag);
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void cycle(int type);
	void cyclePecom();

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address, uint64_t* executed);
	Byte readMem(Word address);
	void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
	void cpuInstruction();
    void resetPressed();
	void onReset();
	void printOutPecom(int q);
	void checkComputerFunction();
	void startPecomKeyFile();
	void closePecomKeyFile();
	void sleepComputer(long ms);
	bool isComputerRunning();
	void startComputerRun(bool load);

private:
	Byte keyValue_[26];

	Byte ctrlEf1_;
	Byte shiftEf2_;
	Byte shiftEf3_;
	Byte escEf4_;

    bool videoRAM_;
    bool extendedRAM_;
    
//	bool capsPressedOnStart_;

	wxFile pecomKeyFile_;
	bool pecomKeyFileOpened_;
	bool keyDown_;
    int keyboardCode_;
    int secondKeyboardCodes[5];
	int keyboardCount_;

	int cycleValue_;

	int dmaCounter_;
	int printerStatus_;
	int cycleCount_;

	wxString commandText_; 
	size_t pecomRunCommand_; 
	int pecomRunState_;
	bool load_;

    int printBit_;
	int printValue_;
    
    bool pecom32_;
};

#endif  // PECOM_H

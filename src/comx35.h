#ifndef COMX35_H
#define COMX35_H

#include "expansion.h"

class Comx : public Expansion
{ 
public:
	Comx(const wxString& title, const wxPoint& pos, const wxSize& size, double zoomLevel, int computerType, double clock);
	~Comx();

	void configureComputer();
	void initComputer();
	Byte ef(int flag);
	Byte ef2();
	Byte ef3();
	Byte ef4();
	Byte in();
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void cycle(int type);
	void cycleComx();
    void cycleKeyboard();

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address);
	Byte readMem(Word address);
	void writeMem(Word address, Byte value, bool writeRom);
    Byte readMemDebug(Word address);
    void writeMemDebug(Word address, Byte value, bool writeRom);
	void cpuInstruction();
    void resetPressed();

	void charEvent(int keycode);
    bool keyDownExtended(int keycode, wxKeyEvent& event);
    bool keyCheck(int keycode, int modifiers);
	void keyUp(int keycode);
	void keyClear();

	void checkComputerFunction();
	void sleepComputer(long ms);
	void startComputerRun(bool load);
	void startComxKeyFile();
	void closeComxKeyFile();
	void onReset();
	bool isFAndMBasicRunning();
	bool isComputerRunning();
	void setDosFileName(int address);
	void saveRam();
	void loadRam();
    void switchQ(int value);

    int getDmaCounter() {return dmaCounter_;};

private:
	Byte keyboardEf2_;
	Byte keyboardEf3_;

	Byte qMode_;

	Byte lastKeyCode_;
    int keyboardCode_;
    int secondKeyboardCodes[5];
	wxKeyCode previousKeyCode_;

    int keyCycles_;
    int rawKeyCode_;
    wxFile keyLogFilePc_;
    wxFile keyLogFile1802_;
//    wxFile keyLogFileCycle_;

	int dmaCounter_;
	int debounceCounter_;
	wxFile comxKeyFile_;
	bool comxKeyFileOpened_;
	size_t comxRunCommand_;
    size_t ctrlvText_;
	wxString commandText_; 
	int comxRunState_;

	Byte thermalEF_;

	bool load_;
	bool fAndMBasicRunning_;
	bool nvramWriteProtected_;

    bool diagRomActive_;
	bool diagDmaLedOn_;
    
	wxDateTime systemTime_;
	wxDateTime comxTime_;
	int day_, month_, year_;
};

#endif  // COMX35_H

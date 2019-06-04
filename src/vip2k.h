#ifndef VIP2K_H
#define VIP2K_H

#include "cdp1802.h"
#include "pixie.h"
#include "keyboard.h"
#include "elfconfiguration.h"

DECLARE_EVENT_TYPE(ON_UART_VIP2K, 811)

class Vip2K : public Cdp1802, public PixieVip2K
{
public:
	Vip2K(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, double clock, ElfConfiguration conf);
	~Vip2K();

	void configureComputer();
	void configureKeyboard();
	void initComputer();
    void keyDown(int keycode);
	void keyUp(int keycode);

	void onRun();

	Byte ef(int flag);
	Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void switchQ(int value);
	void cycle(int type);
	void cycleKey();

	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address);
	Byte readMem(Word addr);
	void writeMem(Word addr, Byte value, bool writeRom);
	void cpuInstruction();
	void moveWindows();
	void updateTitle(wxString Title);
	void onReset();
	void checkVipFunction();
	void startComputerRun(bool load);
	void sleepComputer(long ms);

    void terminalSave(wxString fileName);
    void terminalLoad(wxString filePath, wxString fileName, bool binaryFile);
    void terminalStop();
    
    void setAutoKeyDef(bool autoKeyDef);
    
private:
	ElfConfiguration vipConfiguration;
	Vt100 *vtPointer;

	Byte vipKeyState_[6];
    Byte shiftEf_;
    Byte ctlEf_;
	bool runPressed_;

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

	int cycleValue_;
	int cycleSize_;
	bool redefineKeys_;

	char keyboardValue_;
	Byte keyboardEf_;
	int stateQ_;
	bool player2defined_;

	KeyDef keyDefinition[512];

	size_t vipRunCommand_;
	wxString commandText_; 
	int vipRunState_;
	bool load_;
};

#endif  // VIP2K_H

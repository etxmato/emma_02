#ifndef FRED_H
#define FRED_H
 
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

#define IO_GRP_FRED_KEYPAD 1
#define IO_GRP_FRED_TV 2
#define IO_GRP_FRED_TAPE 3

class FredScreen : public Panel
{
public:
    FredScreen(wxWindow *parent, const wxSize& size);
    ~FredScreen();
    
    void init();
    void onPaint(wxPaintEvent&event);
    void onMousePress(wxMouseEvent& event);
    void onMouseRelease(wxMouseEvent& event);
    void releaseButtonOnScreen(HexButton* buttonPointer);

private:
    int dataSwitchState_[8];
};

class Fred : public wxFrame, public Cdp1802
{
public:
	Fred(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf);
	~Fred();

    void onClose(wxCloseEvent&WXUNUSED(event));

    void configureComputer();
    void initComputer();
	void reDefineKeys(int *, int *);
	void keyDown(int keycode);
	void keyUp(int keycode);
    void cycleKeyboard();

	Byte ef(int flag);
    Byte ef1();
    Byte ef2();
    Byte ef4();
    Byte in(Byte port, Word address);
	void out(Byte port, Word address, Byte value);
	void cycle(int type);
    void cycleLed();
    void setLedMs(long ms);

	void onRunButton(wxCommandEvent&event);
    void onRunButton();
    void autoBoot();
    void onReadButton();
    void startLoad(bool button);
    void onCardButton();
    
	void startComputer();
	void writeMemDataType(Word address, Byte type);
	Byte readMemDataType(Word address);
	Byte readMem(Word addr);
	void writeMem(Word addr, Byte value, bool writeRom);
	void cpuInstruction();

	void onResetButton(wxCommandEvent&event);
	void onReset();
    void sleepComputer(long ms);
    void resetFred();
	
	void cassetteFred(short val);
	void cassetteFred(char val);
    void finishStopTape();
    
    void moveWindows();
    void updateTitle(wxString Title);

    void releaseButtonOnScreen(HexButton* buttonPointer, int buttonType);
    void showDataLeds(Byte value);
    void checkFredFunction();
    
private:
    PixieFred *pixiePointer;
    class FredScreen *fredScreenPointer;
    ElfConfiguration fredConfiguration;

    int ledCycleValue_;
    int ledCycleSize_;
    
    double fredClockSpeed_;

    Byte keyState_[16];

    int keyDefA1_[16];
    int keyDefB1_[16];
    int keyDefA2_[16];
    int keyDefB2_[16];
    
    bool simDefA2_;
    bool simDefB2_;
    
    int keyDefGameHexA_[5];
    int keyDefGameHexB_[5];

    wxKeyCode keyCode_;
    int keyCycles_;
   
    Byte ef1State_;
    Byte ef1StateTape_;
    Byte ef4State_;
    Byte keyValue_;

	Byte tapeRunSwitch_;
    bool cardSwitchOn_;
    bool readSwitchOn_;
    bool tapeActivated_;
    bool tapeRecording_;
    int zeroWaveCounter_;

	int inpMode_;

    int ioGroup_;
	
    short lastSample_;
    char lastSampleChar_;
    int pulseCount_;
    Byte tapeInput_;
	Byte lastTapeInpt_;
    Byte polarity_;
    int bitNumber_;
    int silenceCount_;
	bool tapeEnd_;

    DECLARE_EVENT_TABLE()
};

#endif  // FRED_H

#ifndef VT100_H
#define VT100_H

#include <bitset>
using namespace std;

#define VTBUFFER 1024

class Vt100 : public Video
{
public:

	Vt100(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock, ElfConfiguration elfConfiguration);
	~Vt100();

	void configure(int selectedBaudR, int selectedBaudT, ElfPortConfiguration elfPortConf);
	void configureStandard(int selectedBaudR, int selectedBaudT, int dataReadyFlag);
	void configureUart(ElfPortConfiguration elfPortConf);
    void configureMs2000(int selectedBaudR, int selectedBaudT);
    void setTabChar(Byte value);
    void configureVt2K(int SelectedBaudR, int SelectedBaudT, ElfPortConfiguration elfPortConf);
    void configureQandEfPolarity(int ef, bool vtEnable);
	Byte ef();
	void out(Byte value);
	void cycleVt();
	void setInterlace(bool status);
	void setStretchDot(bool status);

    void switchQ(int value);
	void setClock(double clock);
	void setCycle();
	void copyScreen();
	int Parity(int value); 
	void ScrollUp(); 
	void ScrollDown(); 
	void drawScreen(); 
	void blinkScreen(); 
	void drawCharacter(int pos, int line, Byte value, bool cursor);
//    void setZoom(double zoom);
    void ShowCursor();
	void RemoveCursor(); 
	char* getInteger(char* buffer,int* dest); 
	void Vterr(); 
	void Display(int byt, bool forceDisplay);
	void escapeVT52(Byte byt);
	void escapeVT100(Byte byt);
    void dataAvailable();
    void dataAvailable(Byte value);
	void framingError(bool data);
	void uartOut(Byte value); 
	void uartControl(Byte value); 
	Byte uartIn(); 
    Byte uartStatus();
    Byte uartThreStatus();
	void getKey();
	void checkElfCommand();
	void checkMcdsCommand();
	void ResetVt();
	void ResetIo();
	void setForceUCVt(bool status);
    void terminalSaveVt(wxString fileName);
    void terminalSaveCdp18s020Vt(wxString fileName);
    void terminalLoadVt(wxString fileNamee, bool binaryFile);
    void terminalLoadCdp18s020Vt(wxString fileNamee, bool binaryFile);
    void terminalStopVt();
    void startElfRun(bool load, bool overRide);
    void startMcdsRun(bool load);
    bool readCharRomFile(int computerType, wxString romDir, wxString FileRef);
	void setFullScreen(bool fullScreenSet);
	void onF3();
	void scrollLine();
	void scrollScreen();
	void displayBuffer(int value);
	Byte getDisplay();
	bool charPressed(wxKeyEvent& event);
	void keyDownPressed(wxKeyEvent& event);
	void keyUpPressed() {pressedKey_ = 0;};
	void keyDownVT52(wxKeyEvent& event);
	void keyDownVT100(wxKeyEvent& event);
	void setUpA(int key);
	void setUpB(int key);
	void initScreen();
	void setupAScreen();
	void setupBScreen();
	void setupWaitScreen();
	void setupSave();
	void setupLoad();
	void setForeColour();
	void bell();
	void click();

	Byte getVtMemory(int address) {return videoRam_[address / charactersPerRow_][address % charactersPerRow_];};
	void setVtMemory(int address, Byte value) {videoRam_[address / charactersPerRow_][address % charactersPerRow_] = value;reDraw_ = true;};
	void activateMainWindow();
	void writeMem(int y, int x, int value);

private:
    ElfConfiguration elfConfiguration_;

    CharacterList *characterListPointer;
	wxString line_;
	wxString terminalLine_;
	wxTextFile logFile_;

	int computerType_;
	wxString computerTypeStr_;
	int vtType_;
	double intensity_;
	unsigned char redFore_;
	unsigned char greenFore_;
	unsigned char blueFore_;
	unsigned char redBack_;
	unsigned char greenBack_;
	unsigned char blueBack_;
	
	wxMemoryDC dcScroll;

	int cycleValue_;
	int cycleSize_;
	int cycleBell_;
	int cycleSizeBell_;
	int cycleClick_;
	int cycleSizeClick_;

	bool stretchDot_;
	bool updateCharacter_;
	bool graphicsMode_;
	bool scsMode_;
	Byte g0mode_;
	Byte g1mode_;
	bool xOff_;
	bool ctrlQpressed_;
	int scroll_;
	bool setUpMode_;
	bool setUpModeB_;
	bool answerBack_;
	int answerBackDelimiter_;
	wxString answerBackMessage_;
	int wait_;

	bool cursorKeyMode_;
	bool AltKeypad_;
	bool originMode_;

	int charactersPerRow_;
	int charWidth_;
	int rowsPerFrame_;
	int linesPerCharacter_;
	int heightFactor;

	long vtCount_;
	int baudRateT_;
	int selectedBaudT_;
	int baudRateR_;
	int selectedBaudR_;
	int vtBits_;
	Byte rs232_;
	int vtOutBits_;
	long vtOutCount_;
    Byte vtOut_;

	int cursorLine_;
	int cursorPosition_;
	int savedCursorLine_;
	int savedCursorPosition_;
	char escBuffer_[256];
	int escPosition_;
	int saveLine_;
	int savePosition_;
	int scrollStart_;
	int scrollEnd_;
	int scrolling_;
	bool smoothScroll_;
	int parity_;
	int blinkCount_;
	bool cursorBlinkOn_;

	Byte videoRam_[25][132];

	bool highlightScr_[25][132];
	bool underlineScr_[25][132];
	bool blinkScr_[25][132];
	bool reverseScr_[25][132];
	int doubleWidth_[25];
	float doubleHeight_[25];
	int startLine_[25];
	wxString tabs_;

	bool highlight_;
	bool underline_;
	bool blink_;
	bool reverse_;
	bool blinkOn_;
	bool reBlink_;

	Byte charRom_[4096];
	Byte vt100Ef_;
	Byte vtEnabled_;

	int reverseEf_;
	bool serialLog_;
	int reverseQ_;
    int lastByte_;

	bool load_;
    size_t elfRunCommand_;
    size_t mcdsRunCommand_;
    wxString commandText_;

	Byte uartControl_;
	bitset<8> uartStatus_;
	bool uart_;
    bool serialOpen_;

	char displayBuffer_[VTBUFFER];
	int  displayStart_;
	int  displayEnd_;
	int pressedKey_;
	bool pressedShift_;
	bool pressedCtrl_;
	
	bitset<32> SetUpFeature_;
    
    int colourIndex_;

    bool terminalSave_;
    bool terminalLoad_;
    bool terminalFileCdp18s020_;
    bool binaryFile_;
    wxFile outputTerminalFile;
    wxFFile inputTerminalFile;
    wxString terminalInputFileLine_;
	int dataReadyFlag_;
    Byte previousByte_;
    
    int tab_char;
};

#endif  // VT100_H

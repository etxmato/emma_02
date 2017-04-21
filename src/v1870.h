#ifndef V1870_H
#define V1870_H

#include "comxstatusbar.h"
#include "cidelsastatusbar.h"
#include "cdp1802.h"
#include "video.h"

class CharacterList
{
public:
	wxCoord x;
	wxCoord y;
	CharacterList *nextCharacter;
	int doubleWidth;
};

class V1870 : public Video, public Cdp1802
{
public:

	V1870(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock);
	~V1870();

	void onFocus(wxFocusEvent&event);
	void focus();

	void configure1870Pecom();
	void configure1870Comx(bool expansionRomLoaded, int expansionTypeCard0);
	void configure1870Telmac();
	void configure1870Cidelsa();
	void configure6845();
	void stop6845();
	void init1870();
	Byte ef1_1870();
	Byte ef6845();
	void out2_1870(Byte value);
	void out3_1870(Byte value);
	void out4_1870(Word address);
	void out5_1870(Word address);
	void out6_1870(Word address);
	void out7_1870(Word address);
	void cycle1870();
	void cycle6845();
	void blink1870();
	void blink6845();

	void writePram(Word address, Byte v);
	void writeCram(Word address, Byte v);
    void writeCramDirect(Word address, Byte v);
    void writeColourRamDirect(Word address, Byte v);
	void writePramDirect(Word address, Byte v);
	Byte readPram(Word address);
	Byte readCram(Word address);
    Byte readCramDirect(Word address);
    Byte readColourRamDirect(Word address);
	Byte readPramDirect(Word address);
	void set1870(wxString Register, long value);

	void copyScreen();
	void reDrawBar();
	void updateExpansionLed(bool status);
	void updateStatusLed(bool status);
	void updateCidelsaLedStatus(int number, bool status);
	bool readChargenFile(wxString romDir, wxString romFile);
	Word getPageMemoryMask()  {return pageMemoryMask_;};
	Word getCharacterMemoryMask()  {return pcbMask_*16+15;};

	void writeRegister6845(Byte value);
	Byte readData6845();
	void writeData6845(Byte value);
	Byte read6845(Word addr);
	void write6845(Word addr, Byte value);
	Byte read6845CharRom(Word addr);
	void write6845CharRom(Word addr, Byte value);
	void copyScreen6845();
	void reDrawScreen6845();
	void drawScreen6845();
	void draw6845(Word addr, Byte value);
	void drawCharacter6845(int x, int y, Byte value);
	void drawCursor6845(Word addr, bool status);
	void v1870BarSize();
	void setInterlace(bool status);
	void setClock(double clock);
	void setCycle();
	void setFullScreen(bool fullScreenSet);
	void onF3();

protected:
	int videoMode_;
	int cidelsaGame_;
	bool mc6845started_;

private:
	void drawScreen();
	void drawCharacter(wxCoord x, wxCoord y, Byte v, int address);
	void drawCharacterAndBackground(wxCoord x, wxCoord y, Byte v, int address);
	void drawLine(wxCoord x, wxCoord y, Byte v, Byte pcb, int address);
	void drawBackgroundLine(wxCoord x, wxCoord y);
	void drawPoint(wxCoord x, wxCoord y);
	void updateLedStatus(int card, int i, bool status);

	CidelsaStatusBar *cidelsaStatusBarPointer;
	ComxStatusBar *comxStatusBarPointer;
	wxBitmap *screenScrollCopyPointer;
	CharacterList *characterListPointer;
	CharacterList *characterListPointer6845;

	wxMemoryDC dcScroll;

	int save_v1870X_;
	int save_v1870Y_;
	int computerType_;

	int cycleValue_;
	int cycleSize_;
	int blinkValue_;
	int blinkSize_;
	int cycleValue6845_;
	int cycleSize6845_;
	int nonDisplay6845_;
	int blinkValue6845_;
	int blinkSize6845_;

	Byte pageMemory_[4096];
	Byte vismacColorRam_[4096];
	Byte characterMemory_[4096];
	Byte v1870pcb_[4096];

	Word register3_;
	Word register4_;
	Word register5_;
	Word register6_;
	Word register7_;

	int cmemAccessMode_;
	int charactersPerRow_;
	int linesPerCharacters_;
	int rowsPerScreen_;
	int CmaMask_;
	int pageMemoryMask_;
	int pcbMask_;
	int backgroundColour_;
	int pixelWidth_;
	int pixelHeight_;
	int displayOff_;
	int colourFormatControl_;
	int maxPageMemory_;
	int preDisplayPeriod_;
	int nonDisplayPeriodEnd_;
	int displayPeriodEnd_;
	int displayPeriod_;
	Byte ef1Value_;
	bool nonDisplay_;
	Byte efNonDisplay_;
	Byte efDisplay_;
	bool updateCharacter_;
	wxSize windowSize_;
	wxSize saveWindowSize_;

	Byte vismacColorLatch_;
	bool vismacBlink_;

	int Nht_;
	int charLine_;
	int Nvt_;
	int Nadj_;
	int rows_;
	bool interlace_;
	bool interlaceGUI_;
	int videoM_;
	Byte register_[32];
	int registerIndex_;
	int startAddress_;
	int cursorAddress_;
	int cursorStartLine_;
	int cursorEndLine_;
	int scanLine_;
	bool cursorOn_;
	bool cursorBlink_;
	bool cursorBlinkOn_;
	int blink_;
	int cursorBlinkTime_;
	int updateCharacter6845_;

	DECLARE_EVENT_TABLE()
};

#endif  // V1870_H

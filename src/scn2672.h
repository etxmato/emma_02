#ifndef Scn2672_H
#define Scn2672_H

#include "video.h"

#define SCN2672_ROM_LINES 16
#define SCN2672_FIRST_ROM_LINE 0
#define SCN267_ROM_MASK 0xfff

class Scn2672 : public Video
{
public:
    Scn2672(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, Scn2672Configuration scn2672Configuration, Crt8002Configuration crt8002Configuration);
    ~Scn2672();

    void configureScn2672();
    void configureCrt8002();
    void initScn2672();
    void cycleScn2672();
    void blinkScn2672();

    void setClock(double clock);
    void setCycle();
    int writeInitializationRegisterScn2672(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int writeInitializationRegisterScn2672(Byte registerIndex, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int writeRegisterScn2672(Byte registerIndex, Word value, int showTrace = SHOW_ADDRESS_TRACE);
    void checkIfInitializationRegistersAreSet(int currentRegister);
    int writeRegisterCrt8002(Byte registerIndex, Word value, int showTrace = SHOW_ADDRESS_TRACE);
    void setCharacterAndCursurMode();
    void setVideoHeight();
    void setVideoWidth();
    void setInterlace(bool status);
    void setForcedInterlace();
    void calculateHorizontalFrontPorch();
    int writeCommandScn2672(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int instantaneousCommand(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int delayedCommand(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int writeScreenStartScn2672(Word value, int showTrace = SHOW_ADDRESS_TRACE);
    int writeScreenStartHighScn2672(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int writeScreenStartLowScn2672(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    Byte readScreenStartHighScn2672(int showTrace = SHOW_ADDRESS_TRACE);
    Byte readScreenStartLowScn2672(int showTrace = SHOW_ADDRESS_TRACE);
    int writeCursorScn2672(Word value, int showTrace = SHOW_ADDRESS_TRACE);
    int writeCursorHighScn2672(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int writeCursorLowScn2672(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    Byte readCursorHighScn2672(int showTrace = SHOW_ADDRESS_TRACE);
    Byte readCursorLowScn2672(int showTrace = SHOW_ADDRESS_TRACE);
    int writePointerScn2672(Word value, int showTrace = SHOW_ADDRESS_TRACE);
    int writePointerHighScn2672(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int writePointerLowScn2672(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    Byte readDataScn2672(int showTrace = SHOW_ADDRESS_TRACE);
    void writeDataScn2672(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    void writeScanlinesScn2672(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    void writeCharWidthScn2672(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    void writeScreenRowsScn2672(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    void writeCharPerRowScn2672(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    Byte readAttribute(int showTrace = SHOW_ADDRESS_TRACE);
    Byte readAttributeScreen1(int showTrace = SHOW_ADDRESS_TRACE);
    void writeAttribute(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    void writeAttributeScreen1(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    Byte readStatusScn2672(int showTrace = SHOW_ADDRESS_TRACE);
    Byte readInterruptScn2672(int showTrace = SHOW_ADDRESS_TRACE);
    Byte readInterruptStatusRegister();
    Byte readScn2672Ram(Word addr);
    void writeScn2672Ram(Word addr, Byte value);
    Byte readScn2672CharRom(Word addr);
    void writeScn2672CharRom(Word addr, Byte value);
    void drawScreen();
    void drawNextLine();
	void drawLine();
	bool isEndScreen();
	void setStartScreen();
	void drawCursor(wxCoord x);
    void drawCharacterScn2672(int x, int y, Byte value);
    bool readCharRomFile(wxString romDir, wxString romFile);
    void setFullScreen(bool fullScreenSet);
    void onF3();
    void reBlit(wxDC &dc);
    void resetScreenCopyPointer();

private:
    Scn2672Configuration scn2672Configuration_;
    Crt8002Configuration crt8002Configuration_;

    Byte scn2672ram_[32768];
    int scn2672display_[256][128];
    Byte scn2672displayAttribute_[256][128];
    Byte scn2672CharRom_[SCN267_ROM_MASK+1];

    int registerIndex_;
	Word displayStart_;
	Word displayEnd_;
    Word screenStart_;
    Word cursorAddress_;
    Word pointerAddress_;

    bitset<6> statusRegister_;
    bitset<5> interruptRegister_;
    Byte displayBuffer_;
    bitset<8> attributeType_;
    Byte attributeTypeScreen1_;
    Byte lastAttributeType_;
    
    bool cursorOn_;
    bool displayOn_;

    int lineCycleValueScn2672_; 
    int lineCycleSizeScn2672_;
    int characterLineCycleValueScn2672_; 
    int characterLineCycleSizeScn2672_;
    int nonDisplayScn2672_;
    int cursorBlinkValueScn2672_;
    int cursorBlinkSizeScn2672_;
    int characterBlinkValueScn2672_;
    int characterBlinkSizeScn2672_;

    int verticalRetraceCycleValueScn2672_;
    int verticalRetraceCycleSizeScn2672_;

    Word curentRamAddress_;
    wxCoord currentY_;
    int currentLine_;
    bool reDrawOnNextCycle_;

    wxSize windowSize_;
    
    int equalizingConstant_;
    int activeCharactersPerRow_;
    int horizontalSyncWidth_;
    int horizontalBackPorch_;
    int horizontalFrontPorch_;

    int scanLinesPerCharacterRow_;
	int underLineScanLine_;
	bool doubleHeightCharacter_;

    int characterRowsPerScreen_;
    int verticalBackPorch_;
    int verticalFrontPorch_;
    
    int splitScreenInterruptRow_;
    Byte initializationRegister[11];
    
    int interlace_;
    bool interlaceOR_;
    int cursorStartLine_;
    int cursorEndLine_;
    
    bool cursorBlink_;
    bool cursorBlinkOn_;
    int cursorBlinkTimeValue_;
    int cursorBlinkTimeSize_;
    
    bool characterBlinkOn_;
    int characterBlinkTimeValue_;
    int characterBlinkTimeSize_;

    bool interruptReady_;
    bool interruptSplitScreen_;
    bool interruptLineZero_;
    bool interruptVerticalBlank_;

    bool setVideoConfiguration_;
    bool initializationRegisterLoaded[11];
    bool initializationRegistersLoaded_;

    DECLARE_EVENT_TABLE()
};

#endif  // Scn2672_H

#ifndef I8275_H
#define I8275_H

#include "video.h"

class CharacterList8275
{
public:
    wxCoord x;
    wxCoord y;
    CharacterList8275 *nextCharacter;
};

class i8275 : public Video
{
public:

    i8275(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock);
    ~i8275();

    void configure8275(ElfPortConfiguration elfPortConf);
    void init8275();
    void cycle8275();
    Byte ef8275();

    void setClock(double clock);
    void setCycle();
    void pRegWrite(Byte value);
    void cRegWrite(Byte value);
    Byte pRegRead();
    Byte sRegRead();
    Byte read8275CharRom(Word addr);
    void write8275CharRom(Word addr, Byte value);
    Byte read8275VideoRam(Word addr);
    void write8275VideoRam(Word addr, Byte value);
    void copyScreen();
    void drawScreen();
    void blinkScreen8275();
    void draw8275(Word addr, Byte value);
    void drawCharacter8275(int x, int y, Byte value, bool cursor, Word addr);
    void setInterlace(bool status);
    bool readCharRomFile(wxString romDir, wxString romFile);
    void setFullScreen(bool fullScreenSet);
    void onF3();

private:
    Byte i8275ram_[5120];
    bool cursorReset_[5120];
    bool reverseScr_[5120];
    bool underlineScr_[5120];
    bool highlightScr_[5120];
    int gpaScr_[5120];
    bool blinkScr_[5120];
    Byte i8275CharRom_[8192];

    CharacterList8275 *characterListPointer8275;

    int computerType_;

    int cycleValue8275_;
    int cycleBlankValue8275_;
    int dmaCycleValue8275_;

    int dmaCycleSize8275_;
    int rowCycleSize8275_;
    int horizontalRetraceCycleSize8275_;
    int verticalRetraceCycleSize8275_;
    int fullCycleSize8275_;

    bool reBlink_;

    bool interlace_;
    int videoM_;
    int updateCharacter8275_;

    int screenLocation_;
    int bufferLocation_;
    int parameters_;
    int command_;
    Byte status_;
    bool spacedRows_;
    int horizontalCharactersPerRow_;
    int verticalRetraceRowCount_;
    int verticalRowsPerFrame_;
    int underLinePlacement_;
    int linesPerCharacterRow_;
    int lineCounterMode_;
    int fieldAttributeMode_;
    bool cursorBlinking_;
    bool cursorLine_;
    int horizontalRetraceCount_;
    int burstSpaceCode_;
    int burstCountCode_;
    bool retrace_;
    Byte ef_;

    bool reverse_;
    bool underline_;
    bool highlight_;
    int gpa_;
    bool blink_;
    bool blinkOn_;
    bool attributeChange_;

    int blinkCount_;
    bool cursorBlinkOn_;
    int cursorCharPosition_;
    int cursorRowPosition_;
    int cursorAddress_;

    DECLARE_EVENT_TABLE()
};

#endif  // I8275_H

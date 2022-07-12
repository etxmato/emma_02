#ifndef MC6845_H
#define MC6845_H

#include "video.h"

class CharacterList6845
{
public:
    wxCoord x;
    wxCoord y;
    CharacterList6845 *nextCharacter;    
};

class MC6845 : public Video
{
public:

    MC6845(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock, int charW, ElfPortConfiguration elfPortConf);
    ~MC6845();

    void configure6845(ElfPortConfiguration elfPortConf);
    void configureSuperVideo();
    void init6845();
    Byte ef6845();
    void cycle6845();
    void blink6845();

    void setClock(double clock);
    void setCycle();
    void writeRegister6845(Word addr, Byte value);
    Byte readData6845(Word addr);
    void writeData6845(Byte value);
    Byte read6845(Word addr);
    void write6845(Word addr, Byte value);
    Byte read6845CharRom(Word addr);
    void write6845CharRom(Word addr, Byte value);
    void copyScreen();
    void drawScreen();
    void draw6845(Word addr, Byte value);
    void drawCharacter6845(int x, int y, Byte value);
    void drawCursor6845(Word addr, bool status);
    void setInterlace(bool status);
    bool readCharRomFile(wxString romDir, wxString romFile);
    void setFullScreen(bool fullScreenSet);
    void onF3();
    void reBlit(wxDC &dc);

private:
    Byte mc6845ram_[2048];
    Byte mc6845CharRom_[2048];

    CharacterList6845 *characterListPointer6845;
    Word mc6845RamStart_;
    Word mc6845RamEnd_;
    Word mc6845AddressRegister_;
    Word mc6845DataRegister_;

    int computerType_;
    wxString elfTypeStr_;

    int cycleValue6845_; 
    int cycleSize6845_;
    int nonDisplay6845_;
    int blinkValue6845_;
    int blinkSize6845_;

    wxSize windowSize_;

    int Nht_;
    int charLine_;
    int Nvt_;
    int Nadj_;
    int rows_;
    bool interlace_;
    bool interlaceOR_;
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

    int charW_;
};

#endif  // MC6845_H

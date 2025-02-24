#ifndef MC6847_H
#define MC6847_H

#include "video.h"

class CharacterList6847
{
public:
    wxCoord x;
    wxCoord y;
    CharacterList6847 *nextCharacter;
};

class mc6847 : public Video
{
public:

    mc6847(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, Mc6847Configuration mc6847Configuration);
    ~mc6847();

    void configure();
    void init6847();
    void setMCBit(int bit, int selection);
    void cycle6847();
    void outMc6847(Byte value);
    void setGraphicMode();
    void copyScreen();
    void drawScreen();

    void setClock(double clock);
    void setCycle();
    void write(Word addr, Byte value); 
    Byte read6847(Word addr); 
    int readDirect6847(Word addr); 
    void writeDirect6847(Word addr, int value); 
    Byte read6847CharRom(Word addr);
    void write6847CharRom(Word addr, Byte value);
    Word get6847RamMask();
    void draw(Word addr);
    void drawCharacter(int x, int y, int value);
    void drawGraphic(int x, int y, int value);
    bool readCharRomFile(wxString romDir, wxString romFile);
    void setFullScreen(bool fullScreenSet);
    void onF3();

private:
    CharacterList6847 *characterListPointer6847;
    Mc6847Configuration mc6847Configuration_;

    wxString elfTypeStr_;

    Word mc6847RamStart_;
    Word mc6847RamMask_;
    Byte mc6847CharRom_[2048];
    int mc6847ram_[6144];
    int charHeight_;
    int charWidth_;
    int elementWidth_;
    int charLine_;
    int rows_;
//    int screenHeight_;
//    int screenWidth_;
    int addLine_;

    int cycleValue_;
    int cycleSize_;
    int nonDisplay_;
    int updateCharacter_;
    int graphicMode_;
    int outLatch_;

    int invBit_;
    int extBit_;
    int cssBit_;
    int asBit_;
    int agBit_;
    int gm0Bit_;
    int gm1Bit_;
    int gm2Bit_;

    int inv_;
    int ext_;
    int css_;
    int as_;
    int ag_;
    int gm0_;
    int gm1_;
    int gm2_;
};

#endif  // MC6847_H

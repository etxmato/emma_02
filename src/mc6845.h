#ifndef MC6845_H
#define MC6845_H

#include "video.h"

class MC6845 : public Video
{
public:
    MC6845(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, Mc6845Configuration mc6845Configuration);
    ~MC6845();

    void configure6845();
    void init6845();
    Byte ef6845();
    void cycle6845();
    void blink6845();

    void setClock(double clock);
    void setCycle();
    void writeAddressRegister6845(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    Byte readData6845(Word addr);
    Byte readDataDirect6845(Word addr);
    void writeData6845(Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    void writeRegister6845(Byte regNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    Byte read6845(Word addr);
    void write6845(Word addr, Byte value);
    Byte read6845CharRom(Word addr);
    void write6845CharRom(Word addr, Byte value);
    void copyScreen();
    void drawScreen();
    void draw6845(Word addr, Byte value);
    void drawCharacter6845(int x, int y, Byte value);
    void drawCursor6845(Word addr, bool status);
    void drawCursor6845(wxDC &dc, Word addr, bool status);
    void setInterlace(bool status);
    bool readCharRomFile(wxString romDir, wxString romFile);
    void setFullScreen(bool fullScreenSet);
    void onF3();
    void reBlit(wxDC &dc);
    void resetScreenCopyPointer();

private:
    Mc6845Configuration mc6845Configuration_;
    
    Byte mc6845ram_[16384];
    Byte mc6845CharRom_[2048];

    Word mc6845RamStart_;
    Word mc6845RamEnd_;
    Word mc6845AddressRegister_;
    Word mc6845DataRegister_;

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

    int charW_;
};

#endif  // MC6845_H

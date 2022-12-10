#ifndef VIS1870_H
#define VIS1870_H

#include "comxstatusbar.h"
#include "diagstatusbar.h"
#include "cidelsastatusbar.h"
#include "cdp1802.h"
#include "video.h"

#define INT_MODE1 0
#define INT_MODE2 1
#define INT_MODE3 2
#define INT_MODE4 3
#define INT_NONE 4

class VIS1870 : public Video
{
public:

    VIS1870(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock, double videoClock, IoConfiguration ioConfiguration, int videoNumber);
    ~VIS1870();

    void onFocus(wxFocusEvent&event);
    void focus();

    bool configure1870(bool expansionRomLoaded, int expansionTypeCard0);
    void v1870BarSize();
    void init1870();
    Byte ef1_1870();
    void out2_1870(Byte value);
    void out3_1870(Byte value);
    void out4_1870(Word address);
    void out5_1870(Word address);
    void out6_1870(Word address);
    void out7_1870(Word address);
    void cycle1870();
    void blink1870();

    int writePram(Word address, Byte v);
    int writeCram(Word address, Byte v);
    void writeColourRamDirect(Word address, Byte v);
    Byte readCramDirect(Word address);
    void writeCramDirect(Word address, Byte value);
    Byte readPram(Word address);
    Byte readCram(Word address);
    Byte readColourRamDirect(Word address);

    void copyScreen();
    void reDrawBar();
    void updateExpansionLed(bool status);
    void updateStatusLed(bool status);
    void updateDiagLedStatus(int led, bool status);
    void updateCidelsaLedStatus(int number, bool status);
    bool readChargenFileTmc(wxString romDir, wxString romFile);
    bool readChargenFile(wxString romDir, wxString romFile);

    void setClock(double clock);
    void setCycle();
    void setFullScreen(bool fullScreenSet);
    void onF3();

    void reBlit(wxDC &dc);
    bool getReturnVideoMode() {return ioConfiguration_.v1870useVideoModeEf;};

protected:
    int videoMode_;
    int cidelsaGame_;
    int computerType_;

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
    DiagStatusBar *diagStatusBarPointer;

    wxBitmap *screenScrollCopyPointer;
    CharacterList *characterListPointer;

    wxMemoryDC dcScroll;

    int save_v1870X_;
    int save_v1870Y_;

    int cycleValue_;
    int cycleSize_;
    int blinkValue_;
    int blinkSize_;

    Byte vismacColorRam_[4096];
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

    bool interlace_;

    int videoM_;
    int registerIndex_;
    int cursorStartLine_;
    int cursorEndLine_;
    double videoClock_;

    int tempCompType_;
    
    IoConfiguration ioConfiguration_;
    
    DECLARE_EVENT_TABLE()
};

#endif  // VIS1870_H

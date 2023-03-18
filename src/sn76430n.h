#ifndef SN76430N_H
#define SN76430N_H

#include "sn76430n.h"
#include "video.h"

class SN76430N : public Video
{
public:

    SN76430N(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock, double videoClock, IoConfiguration ioConfiguration, int videoNumber);
    ~SN76430N();

    void focus();

    void configure();
    void init();
    void cycle();

    void writeRam(Word address, Byte value);

    void copyScreen();

    void setClock(double clock);
    void setCycle();
    void setFullScreen(bool fullScreenSet);
    void onF3();

    void reBlit(wxDC &dc);

private:
    void drawScreen();
    void drawCharacter(wxCoord x, wxCoord y, Byte value);
    
    CharacterList *characterListPointer;

    int cycleValue_;
    int cycleSize_;

    double videoClock_;

    bool updateCharacter_;
    wxSize saveWindowSize_;
    
    IoConfiguration ioConfiguration_;
    
    DECLARE_EVENT_TABLE()
};

#endif  // SN76430N_H

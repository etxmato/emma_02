#ifndef SN76430N_H
#define SN76430N_H

#include "sn76430n.h"
#include "video.h"

class SN76430N : public Video
{
public:

    SN76430N(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, double videoClock, Sn76430NConfiguration sn76430NConfiguration);
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
        
    DECLARE_EVENT_TABLE()
};

#endif  // SN76430N_H

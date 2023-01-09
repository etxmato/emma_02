#ifndef PIXIE_H
#define PIXIE_H

#include "video.h"
#include "vip2statusbar.h"

class PlotList
{
public: 
    wxCoord x;
    wxCoord y;
    int width;
    int height;
    wxPen penClr;
    PlotList *nextPlot;
};

class Pixie : public Video
{
public:
    Pixie(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, int videoNumber);
    ~Pixie();

    void reset();
    void configurePixie(IoConfiguration portConf);
    virtual void configurePixie() {};
    void configurePixieSuper(IoConfiguration portConf);
    void configurePixieStudio2();
    void configurePixieCoinArcade();
    void configurePixieVisicom();
    void configurePixieVip();
    void configurePixieVelf();
    void configurePixieVipII(bool runLed);
    void configurePixieTmc1800();
    void configurePixieTelmac();
    void configurePixieVictory();
    void configurePixieEti();
    void initiateColour(bool colour);
    void configurePixieNano();
    void configurePixieCosmicos();
    void initPixie(); 
    Byte efPixie();
    virtual Byte inPixie();
    virtual void outPixie();
    virtual void outPixieBackGround();
    virtual void cyclePixie();
    void cyclePixieCoinArcade();
    void cyclePixieTelmac();
    void dmaEnable();
    
    virtual void copyScreen();
    void drawBackgroundLine();
    virtual void drawScreen();
    void plot(int x, int y, int c, int color);
    void plot(int x, int y, int width, int height, int c, int color);

    void setFullScreen(bool fullScreenSet);
    void onF3();
    void pixieBarSize();
    void reDrawBar();
    void updateLedStatus(int led, bool status);
    void setInterlace(bool status);
    void reBlit(wxDC &dc);

protected:
    PlotList *plotListPointer;
    Byte pbacking_[384][208];
    Byte color_[384][208];
    Byte bgColor_[312]; 
    bool bgChanged;
    bool interlace_;
    Byte pixieEf_;

    bool graphicsOn_;
    long graphicsNext_;
    long graphicsMode_;
 
    int colourMask_;
    int backGroundInit_;

    Byte vidInt_;
    bool studioIVFactor_;

    int interruptGraphicsMode_;
    int startGraphicsMode_;
    int endGraphicsMode_;
    int endScreen_;

    int videoMode_;
    int highRes_;
    int graphicsX_;

    int updatePlot_;

private:
    wxString runningComputer_;
    VipIIStatusBar *vipIIStatusBarPointer;

    int computerType_;
    Byte vidCycle_;

};

class PixieFred : public Pixie
{
public:
    PixieFred(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, int videoNumber);

    void configurePixie();
    void drawScreen();
    void cyclePixie();
    void setDisplayType(int displayType);
  
private:
    int displayType_;
    int xInterlace_, yInterlace_, xNonInterlace_, yNonInterlace_;

};

class PixieVip2K : public Pixie
{
public:
    PixieVip2K(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, int videoNumber);
    
    void configurePixie();
    void drawScreen();
    void cyclePixie();
    void executeSequencer(Byte sequencerValue);
    Byte inPixie();
    void outPixie();

private:
    int additionalBottomPalLines_;
    int additionalTopPalLines_;
    
    Word sequencerAddress_;
    int scanLine_;
    int scanByte_;
    int viewableLines_;
    int totalNumberOfLines_;
    int sizeTopBox_;
    int sizeBottomBox1_;
    int sizeBottomBox2_;
};

class PixieStudioIV : public Pixie
{
public:
    PixieStudioIV(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, int videoNumber);

    void configurePixie();
    void drawScreen();
    void cyclePixie();
    void outPixie();
    void outPixieBackGround();
    void outPixieStudioIV(int value);
    void switchVideoMode(int videoMode);

private:
};

class PixieEti : public Pixie
{
public:
    PixieEti(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, int videoNumber);

    void configurePixie();
    void copyScreen();
    void drawScreen();
    void cyclePixie();
    void outPixie();
    void outPixieBackGround();

private:
};

#endif  // PIXIE_H

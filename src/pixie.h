#ifndef PIXIE_H
#define PIXIE_H

#include "video.h"
#include "statusbar.h"

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
    Pixie(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int videoNumber, int videoType, CDP1861Configuration cdp1861Configuration, CDP1862Configuration cdp1862Configuration, CDP1864Configuration cdp1864Configuration, int bootStrapType);
    ~Pixie();

    void reset();
    void configurePixie();
    void configurePixieIn();
    void configurePixieOut();
    void configurePixieSuper();
    void configurePixieCoinArcade(CoinConfiguration coinConfiguration);
    void initiateColour(bool colour);
    void configureCdp1862(bool autoBoot);
    void configureCdp1864(CDP1864Configuration cdp1864Configuration);
    void initPixie();
    virtual Byte efPixie();
    virtual Byte efCdp1864();
    virtual Byte inPixie();
    virtual void outPixie();
    void outCdp1862BackGround();
    void outCdp1862BackGround(Byte background);
    void outCdp1864BackGround();
    void outCdp1864BackGround(Byte background);
    virtual void cyclePixie();
    void cyclePixieCoinArcade();
    void cyclePixieCdp1864();
    
    virtual void copyScreen();
    void drawBackgroundLine();
    virtual void drawScreen();
    void plot(int x, int y, int c, int color);
    void plot(int x, int y, int width, int height, int c, int color);

    // Software-framebuffer rendering overrides (see pixie.cpp for details).
    // On macOS these write into a wxImage framebuffer instead of issuing
    // per-pixel wxGraphicsContext (CoreGraphics) calls; on Windows/Linux
    // they simply delegate to the base Video implementation so those
    // rendering paths are unchanged.
    virtual void setColour(int clr);
    virtual void drawPoint(wxCoord x, wxCoord y);
    virtual void drawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);

#if defined(__WXMAC__)
    void drawFullFrameMac();
#endif

    void setFullScreen(bool fullScreenSet);
    void onF3();
    void pixieBarSize();
    void reDrawBar();
    void updateVipLedStatus(bool status, int led);
    void setInterlace(bool status);
    void reBlit(wxDC &dc);

protected:
    CDP1861Configuration cdp1861Configuration_;
    CDP1862Configuration cdp1862Configuration_;
    CDP1864Configuration cdp1864Configuration_;

    PlotList *plotListPointer;
    Byte pbacking_[384][208];
    Byte color_[384][208];
    Byte bgColor_[312]; 
    bool bgChanged;
    bool interlace_;
    Byte pixieEf_;

    long graphicsNext_;
    long graphicsMode_;
 
    int colourMask_;
    int backGroundInit_;

    Byte vidInt_;
    bool studioIVFactor_;

//    PixieGraphics cdp1861Configuration_.pixieGraphics;

//    int p_videoMode_;
    int highRes_;
    int graphicsX_;

    int updatePlot_;
    int colourType_;

private:
    VipIIStatusBar *vipIIStatusBarPointer;

    Byte vidCycle_;

#if defined(__WXMAC__)
    // macOS software framebuffer: all pixie drawing writes here first,
    // then the finished frame is blitted to dcMemory with a single
    // gc->DrawBitmap in drawFullFrameMac() (called from copyScreen()).
    void ensureFramebufferMac();
    wxImage *macFrameImage_;
    wxColour macCurrentColour_;
#endif

};

class PixieFred : public Pixie
{
public:
    PixieFred(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int videoNumber, int videoType, CDP1861Configuration cdp1861Configuration, CDP1862Configuration cdp1862Configuration, CDP1864Configuration cdp1864Configuration, int bootStrapType);

    void configureFredVideo(FredVideoConfiguration fredVideoConfiguration);
    void drawScreen();
    void cyclePixie();
	void enableScreen(bool enable, int showTrace = SHOW_ADDRESS_TRACE);
    void setDisplayType(int displayType, int showTrace = SHOW_ADDRESS_TRACE);
  
private:
    int displayType_;
    int xInterlace_, yInterlace_, xNonInterlace_, yNonInterlace_;

};

class PixieVip2K : public Pixie
{
public:
    PixieVip2K(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int videoNumber, int videoType, CDP1861Configuration cdp1861Configuration, CDP1862Configuration cdp1862Configuration, CDP1864Configuration cdp1864Configuration, int bootStrapType);

    void configureVip2K(Vip2KVideoConfiguration vip2KVideoConfiguration);
    void drawScreen();
    void cyclePixie();
    void executeSequencer(Byte sequencerValue);
    Byte efPixie();
    Byte inPixie();
    void outPixie();

private:
    Vip2KVideoConfiguration vip2KVideoConfiguration_;
    
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
    PixieStudioIV(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int videoNumber, int videoType, CDP1861Configuration cdp1861Configuration, CDP1862Configuration cdp1862Configuration, CDP1864Configuration cdp1864Configuration, int bootStrapType);

    void configureSt4(Studio4VideoConfiguration studio4VideoConfiguration);
    void drawScreen();
    void cyclePixie();
    Byte efPixie();
    void outPixie();
    void dmaEnable(int showTrace = SHOW_ADDRESS_TRACE);
    int st4BackGround(int value, int showTrace = SHOW_ADDRESS_TRACE);
    int enableScreen(bool enable, int showTrace = SHOW_ADDRESS_TRACE);
    int st4PalNtsc(int palNtsc, int showTrace = SHOW_ADDRESS_TRACE);
    int st4WhiteForeground(bool white, int showTrace = SHOW_ADDRESS_TRACE);
    void outPixieStudioIV(int value, int showTrace = SHOW_ADDRESS_TRACE);
    void switchVideoMode(int videoMode);

private:
    Studio4VideoConfiguration studio4VideoConfiguration_;
    
    int videoMode_;
};

#endif  // PIXIE_H

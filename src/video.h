#ifndef VIDEO_H
#define VIDEO_H

#include "splash.h"
#include "definition.h"

class VideoScreen : public wxWindow
{
public:
    VideoScreen(wxWindow *parent, const wxSize& size, double zoom, int videoNumber);
    VideoScreen(wxWindow *parent, const wxSize& size, double zoom, int videoNumber, double xZoomFactor);
    VideoScreen(wxWindow *parent, const wxSize& size, double zoom, int videoNumber, double xZoomFactor, bool vipiiRcaMode);
    VideoScreen(wxWindow *parent, const wxSize& size, double zoom, bool vt100, int uartNumber);
    ~VideoScreen() {};

    void onPaint(wxPaintEvent&event);
    void onChar(wxKeyEvent&event);
    void vtOut(int value);
    void onKeyDown(wxKeyEvent&event);
    void onKeyUp(wxKeyEvent&event);
    Byte getKey(Byte vtOut); 
    bool isVt() {return vt100_;};

    void blit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height, wxDC *source, wxCoord xsrc, wxCoord ysrc);
    void refreshVideo();
    void drawLine(wxCoord x, wxCoord y, wxCoord width, wxCoord height, wxPen penCclr);
    void drawExtraBackground(wxColour clr, int width, int height, wxCoord offsetX, wxCoord offsetY);
    void drawRectangle(wxColour clr, int x, int y, wxCoord width, wxCoord height);
    void setForceUCVt(bool status);
    void disableScreen(wxColour clr, int xSize, int ySize);
    void setZoom(double zoom);
    void setRepeat(bool status) {repeat_ = status;};
    void setScale(double scaleX) {xZoomFactor_ = scaleX;};

private:
    bool vipiiRcaMode_;
    double zoom_;
    double xZoomFactor_;

    int width_;
    int height_;

    bool vt100_;
    int uartNumber_;
    int videoNumber_;
    char keyBuffer_[26];
    int  keyStart_;
    int  keyEnd_;
    int lastKey_;
    bool repeat_;
    bool forceUpperCase_;
    
    DECLARE_EVENT_TABLE()
};

class Video : public wxFrame
{
public:
    Video(const wxString& title, const wxPoint& pos, const wxSize& size);
    virtual ~Video();

    virtual void onClose(wxCloseEvent&event );
    void onSize(wxSizeEvent&event);
    virtual void reset();

    virtual void setInterlace(bool status);
    virtual void setStretchDot(bool status);
    virtual void setReBlit();
    long getVideoSyncCount();
    void resetVideoSyncCount();
    virtual void focus();
    virtual void updateComxExpansionLed(bool status);
    virtual void updateComxStatusLed(bool status);
    virtual void dataAvailable();
    virtual void dataAvailable(Byte value);
    virtual void dataAvailableUart(bool data);
    virtual void framingError(bool data);
    bool isFullScreenSet() {return fullScreenSet_;};
    virtual void setFullScreen(bool fullScreenSet);
    virtual Byte getVtMemory(int address);
    virtual void setVtMemory(int address, Byte value);
    virtual bool charPressed(wxKeyEvent& event);
    virtual void keyDownPressed(wxKeyEvent& event);
    virtual void keyUpPressed();
    virtual void uartOut(Byte value); 
    virtual void uartControl(Byte value); 
    virtual Byte uartIn(); 
    virtual Byte uartStatus(); 
    virtual void ResetIo();

    void defineColours();
    void setColour(int colourNumber, wxString colour);
    void setBorder(int videoNumber, int borderX, int boderY);
    void setScale(double scaleX);
    void reColour() {reColour_ = true;};

    virtual void setScreenSize();
    virtual void changeScreenSize();
    virtual void onF3();
    virtual void onF5();
    double getZoom() {return zoom_;};
    virtual void setZoom(double zoom);
    virtual void drawExtraBackground(wxColour clr);
    virtual void copyScreen();
    virtual void setCycle() {};
    virtual void drawScreen() {};
    virtual void blinkScreen() {};
    virtual void reDrawBar();
    virtual void updateStatusLed(bool status, int card, int i = 0);
    virtual void setClock(double clock);
    void activateMainWindow();

    virtual void setColour(int clr);
    void setColour(wxColour clr);
    virtual void drawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    virtual void drawPoint(wxCoord x, wxCoord y);
    void setColourMutex(int clr);
    void drawRectangleMutex(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    void drawPointMutex(wxCoord x, wxCoord y);
    void splashScreen();
 
    virtual Byte readPramDirect(Word WXUNUSED(address)) {return 0;};
    virtual Byte readCramDirect(Word WXUNUSED(address)) {return 0;};
    virtual void writeCramDirect(Word WXUNUSED(address), Byte WXUNUSED(v)) {};
    virtual void writePramDirect(Word WXUNUSED(address), Byte WXUNUSED(v)) {};
    Word getPageMemorySize()  {return pageMemorySize_;};
    Word getCharMemorySize()  {return charMemorySize_;};
    Word getGraphicMemorySize()  {return graphicMemorySize_;};
    void setInterruptEnable(bool status) {interruptEnabled_ = status;};
    Byte getPcbMask()  {return pcbMask_;};
    int getMaxLinesPerChar()  {return maxLinesPerCharacters_;};
    virtual Byte readColourRamDirect(Word WXUNUSED(address)) {return 0;};
    virtual void writeColourRamDirect(Word WXUNUSED(address), Byte WXUNUSED(v)) {};
    virtual Byte readGraphicRamDirect(Word WXUNUSED(address)) {return 0;};
    virtual void writeGraphicRamDirect(Word WXUNUSED(address), Byte WXUNUSED(v)) {};

    void refreshVideo();
    void setClientSize(wxSize size);

    virtual void reBlit(wxDC &dc);
    virtual bool isMc6845running() {return false;};
    bool arePixieGraphicsOn() {return graphicsOn_;};
    void setPixieGraphics(bool status) {graphicsOn_ = status;};
    virtual void pixieBarSize() {};

protected:
    Sn76430NConfiguration sn76430NConfiguration_;

    // Shared pieces of Video::copyScreen(). updateReColour() applies any
    // pending colour/border changes (set via reColour()) and marks the
    // screen for a full redraw. The virtual hooks let individual video
    // types vary the few per-chip differences: eventRefreshScreen() posts
    // the async refresh event (vt100 posts it with isVt=true and
    // uartNumber_, the other video types with isVt=false and videoNumber_);
    // copyScreenHeight() and copyScreenBackgroundColour() define the blit
    // geometry and the extra-background colour.
    void updateReColour();
    virtual void eventRefreshScreen();
    virtual void applyScaleFactor();
    virtual int copyScreenHeight() {return videoHeight_+2*offsetY_;};
    virtual wxColour copyScreenBackgroundColour() {return colour_[colourIndex_+backGround_];};
    
    Byte pageMemory_[4096];
    Byte characterMemory_[4096];
    Byte graphicMemory_[8192];
    
    int pcbMask_;
    int pageMemoryMask_;
    Word pageMemorySize_;
    Word charMemorySize_;
    Word graphicMemorySize_;
    bool charMemoryIsRom_;
    Word romAddress_;

    wxColour colour_[COL_MAX];
    wxPen penColour_[COL_MAX];
    wxBrush brushColour_[COL_MAX];
    int borderX_[VIDEOXMLMAX];
    int borderY_[VIDEOXMLMAX];

    wxColour colourNew_[COL_MAX];
    wxPen penColourNew_[COL_MAX];
    wxBrush brushColourNew_[COL_MAX];
    int borderXNew_[VIDEOXMLMAX];
    int borderYNew_[VIDEOXMLMAX];

    wxMemoryDC dcMemory;
    wxImage *screenImage;

    wxMemoryDC dcMemoryMainAndSpritePlane;
    
    wxMemoryDC dcMemoryMainPlane;
    
    wxMemoryDC dcMemorySpritePlane;
        
    double clock_;

    int videoType_;
    int numberOfColours_;
    int backGround_;
    bool reBlit_;
    bool reDraw_;
    bool reBlink_;
    bool reDrawMultiColor_;
    bool reColour_;
    bool reCycle_;
    bool newBackGround_;
    bool extraBackGround_;

    class VideoScreen *videoScreenPointer;
    wxBitmap *screenCopyPointer;
    wxBitmap *mainAndSpritePlanePointer;
    wxBitmap *mainPlanePointer;
    wxBitmap *spritePlanePointer;
    wxBitmap *screenFilePointer;
    bool changeScreenSize_;
    double zoomChanged_;
    bool memoryDCvalid_;
    bool memoryDCBeingUsed_;

    double zoom_;
    double xZoomFactor_;
    double xZoomFactorNew_;

    int videoWidth_;
    int videoHeight_;
    int destinationWidth_;
    int destinationHeight_;
    wxCoord offsetX_;
    wxCoord offsetY_;

    bool fullScreenSet_;
    long videoSyncCount_;
    
    bool interruptEnabled_;
    int maxLinesPerCharacters_;
    bool v1870Configured_;

    int uartNumber_;
    int colourIndex_;
    int videoNumber_;

    bool graphicsOn_;
    
// Software framebuffer (see video.cpp). All per-pixel drawing
    // funnels through the virtual setColour / drawPoint / drawRectangle
    // (and the setColourMutex / drawPointMutex / drawRectangleMutex
    // variants, which delegate to them). They write into a plain wxImage
    // software framebuffer instead of issuing per-pixel drawing calls,
    // which is what kept the pixie-family displays below real-time and
    // starved the audio ring. Once per video frame, copyScreen() calls
    // flushFramebuffer() to push the touched planes into their backing
    // memory DCs with one DrawBitmap each.
    //
    // There is one framebuffer per rendering plane, matching the base
    // Video memory DCs: [0]=dcMemory, [1]=dcMemoryMainPlane,
    // [2]=dcMemorySpritePlane. Single-plane video types (Pixie,
    // MC6847, VIS1870, ...) only ever use plane 0.
    //
    // Every Video subclass uses this path (each enables it at construction);
    // there is no non-framebuffer fallback draw path left, and no graphics
    // context is created on any platform.
    void flushFramebuffer();
    void copyFramebuffer(int fromPlane, int toPlane);
    void setMacPlane(int plane);
    void ensureFramebuffer(int plane = 0);
    void drawPointFramebuffer(wxCoord x, wxCoord y);
    void drawRectangleFramebuffer(wxCoord x, wxCoord y, wxCoord width, wxCoord height);

    wxImage *macFrameImage_[3];
    wxColour macCurrentColour_[3];
    int  macPlaneId_;
    bool macPlaneDirty_[3];
    
private:
    SplashScreen *splashScreen_;
    
    DECLARE_EVENT_TABLE()
};

#endif    // VIDEO_H

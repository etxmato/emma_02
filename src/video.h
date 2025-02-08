#ifndef VIDEO_H
#define VIDEO_H

#include "splash.h"
#include "definition.h"

class CharacterList
{
public:
    wxCoord x;
    wxCoord y;
    CharacterList *nextCharacter;
    int doubleWidth;
};

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
    ~Video() {};

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
    virtual void reDrawBar();
    virtual void updateStatusLed(bool status, int card, int i = 0);
    virtual void setClock(double clock);
    void activateMainWindow();

    void setColour(int clr);
    void setColour(wxColour clr);
    void drawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    void drawPoint(wxCoord x, wxCoord y);
    void setColourMutex(int clr);
    void drawRectangleMutex(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    void drawPointMutex(wxCoord x, wxCoord y);
    void splashScreen();
 
    Byte readPramDirect(Word address);
    virtual Byte readCramDirect(Word address);
    virtual void writeCramDirect(Word address, Byte v);
    void writePramDirect(Word address, Byte v);
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
    wxGraphicsContext *gc;
    wxImage *screenImage;

    wxMemoryDC dcMemoryMainAndSpritePlane;
    wxGraphicsContext *gcMainAndSpritePlane;
    
    wxMemoryDC dcMemoryMainPlane;
    wxGraphicsContext *gcMainPlane;
    
    wxMemoryDC dcMemorySpritePlane;
    wxGraphicsContext *gcSpritePlane;
        
    double clock_;

    int videoType_;
    int numberOfColours_;
    int backGround_;
    bool reBlit_;
    bool reDraw_;
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
    
    double zoom_;
    bool zoomFraction_;
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
    
private:
    SplashScreen *splashScreen_;
    
    DECLARE_EVENT_TABLE()
};

#endif    // VIDEO_H

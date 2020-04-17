#ifndef VIDEO_H
#define VIDEO_H

#include "splash.h"

class VideoScreen : public wxWindow
{
public:
	VideoScreen(wxWindow *parent, const wxSize& size, double zoom, int computerType);
	VideoScreen(wxWindow *parent, const wxSize& size, double zoom, int computerType, double xZoomFactor);
	VideoScreen(wxWindow *parent, const wxSize& size, double zoom, int computerType, bool vt100);
	~VideoScreen() {};

	void onPaint(wxPaintEvent&event);
	void onChar(wxKeyEvent&event);
	void vtOut(int value);
	void onKeyDown(wxKeyEvent&event);
	void onKeyUp(wxKeyEvent&event);
	Byte getKey(Byte vtOut); 
	bool isVt() {return vt100_;};

	void blit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height, wxDC *source, wxCoord xsrc, wxCoord ysrc);
    void blitDirect(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height, wxDC *source, wxCoord xsrc, wxCoord ysrc);
	void drawLine(wxCoord x, wxCoord y, wxCoord width, wxCoord height, wxPen penCclr);
	void drawExtraBackground(wxColour clr, int width, int height, wxCoord offsetX, wxCoord offsetY);
	void drawRectangle(wxColour clr, int x, int y, wxCoord width, wxCoord height);
	void setForceUCVt(bool status);
	void disableScreen(wxColour clr, int xSize, int ySize);
	void setZoom(double zoom);
	void setRepeat(bool status) {repeat_ = status;};
	void setScale(double scaleX) {xZoomFactor_ = scaleX;};

private:
	double zoom_;
	double xZoomFactor_;
	int computerType_;

	int width_;
	int height_;

	bool vt100_;
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
	virtual void updateStatusLed(bool status);
    virtual void dataAvailable();
    virtual void dataAvailable(Byte value);
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

	void defineColours(int type);
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
	virtual void updateDiagLedStatus(int led, bool status);
 
    Byte readPramDirect(Word address);
    Byte readCramDirect(Word address);
    void writeCramDirect(Word address, Byte v);
    void writePramDirect(Word address, Byte v);
    Word getPageMemorySize()  {return pageMemorySize_;};
    Word getCharMemorySize()  {return charMemorySize_;};
    void setInterruptEnable(bool status) {interruptEnabled_ = status;};
    Byte getPcbMask()  {return pcbMask_;};
    int getMaxLinesPerChar()  {return maxLinesPerCharacters_;};
    void blitDirect(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height, wxDC *source, wxCoord xsrc, wxCoord ysrc);

protected:
    Byte pageMemory_[4096];
    Byte characterMemory_[4096];
    int pcbMask_;
    int pageMemoryMask_;
    Word pageMemorySize_;
    Word charMemorySize_;
    bool charMemoryIsRom_;
    Word romAddress_;

	wxColour colour_[67];
	wxPen penColour_[67];
	wxBrush brushColour_[67];
	int borderX_[10];
	int borderY_[10];

	wxColour colourNew_[67];
	wxPen penColourNew_[67];
	wxBrush brushColourNew_[67];
	int borderXNew_[10];
	int borderYNew_[10];

	wxMemoryDC dcMemory;
	wxGraphicsContext *gc;
	wxImage *screenImage;

	int computerType_;
	double clock_;

	int videoType_;
	int numberOfColours_;
	int backGround_;
	bool reBlit_;
	bool reDraw_;
	bool reColour_;
	bool reCycle_;
	bool newBackGround_;
	bool extraBackGround_;

    class VideoScreen *videoScreenPointer;
	wxBitmap *screenCopyPointer;
	wxBitmap *screenFilePointer;
	bool changeScreenSize_;
	double zoomChanged_;

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

private:
    SplashScreen *splashScreen_;
    
	DECLARE_EVENT_TABLE()
};

#endif	// VIDEO_H

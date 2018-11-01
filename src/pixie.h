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
	Pixie(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType);
	~Pixie();

	void reset();
	void configurePixie(ElfPortConfiguration portConf);
	void configurePixieStudio2();
    void configurePixieFred();
    void configurePixieCoinArcade();
    void configurePixieVisicom();
    void configurePixieVip();
    void configurePixieVip2K();
	void configurePixieVelf();
	void configurePixieVipII();
	void configurePixieTmc1800();
	void configurePixieTelmac();
    void configurePixieStudioIV();
    void configurePixieVictory();
	void configurePixieEti();
	void initiateColour(bool colour);
	void configurePixieNano();
	void configurePixieCosmicos();
	void initPixie(); 
	Byte efPixie();
	Byte inPixie();
	void outPixie(); 
    void outPixieBackGround();
    void outPixieStudioIV(int value);
    void switchVideoMode(int videoMode);
    void cyclePixie();
    void cyclePixieVip2K();
    void executeSequencer(Byte sequencerValue);
    void cyclePixieStudioIV();
    void cyclePixieCoinArcade();
	void cyclePixieTelmac();
    void dmaEnable();
    
	void copyScreen();
	virtual void drawScreen();
	void plot(int x, int y, int c, int color);
    void plot(int x, int y, int width, int height, int c, int color);

	void setFullScreen(bool fullScreenSet);
	void onF3();
	void pixieBarSize();
	void reDrawBar();
	void updateLedStatus(int led, bool status);
    void setInterlace(bool status);

protected:
    Byte pbacking_[384][192];
    Byte color_[384][192];
    bool interlace_;

    bool graphicsOn_;
    long graphicsNext_;
    long graphicsMode_;
 
    int colourMask_;

    Byte vidInt_;

private:
	PlotList *plotListPointer;
	wxString runningComputer_;
	VipIIStatusBar *vipIIStatusBarPointer;

	int computerType_;
	Byte vidCycle_;

	int graphicsX_;
	Byte pixieEf_;

	int backGroundInit_;

	int updatePlot_;
	int highRes_;
    
    bool studioIVFactor_;
    int interruptGraphicsMode_;
    int startGraphicsMode_;
    int endGraphicsMode_;
    int endScreen_;

    int videoMode_;
    
    Word sequencerAddress_;
    int scanLine_;
    int scanByte_;
};

class PixieFred : public Pixie
{
public:
    PixieFred(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType);

    void drawScreen();
    void cyclePixieFred();
    void setDisplayType(int displayType);
  
private:
    int displayType_;
    int xInterlace_, yInterlace_, xNonInterlace_, yNonInterlace_;

};

#endif  // PIXIE_H

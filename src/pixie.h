#ifndef PIXIE_H
#define PIXIE_H

#include "video.h"
#include "vip2statusbar.h"

class PlotList
{
public: 
	wxCoord x;
	wxCoord y;
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
    void cyclePixieStudioIV();
    void cyclePixieCoinArcade();
	void cyclePixieTelmac();
    void dmaEnable();

	void copyScreen();
	void drawScreen();
	void plot(int x, int y, int c, int color);

	void setFullScreen(bool fullScreenSet);
	void onF3();
	void pixieBarSize();
	void reDrawBar();
	void updateLedStatus(int led, bool status);

private:
	PlotList *plotListPointer;
	wxString runningComputer_;
	VipIIStatusBar *vipIIStatusBarPointer;

	int computerType_;
	Byte pbacking_[128][192];
	Byte color_[128][192];
	Byte vidInt_;
	Byte vidCycle_;

	bool graphicsOn_;
	long graphicsMode_;
	long graphicsNext_;
	int graphicsX_;
	Byte pixieEf_;

	int backGroundInit_;

	int updatePlot_;
	int colourMask_;
	int highRes_;
    
    bool studioIVFactor_;
    int interruptGraphicsMode_;
    int startGraphicsMode_;
    int endGraphicsMode_;
    int endScreen_;

    int videoMode_;
};

#endif  // PIXIE_H

/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 ***                                                             ***
 *** 1802 Code based on elf emulator by Michael H Riley with     ***
 *** copyright as below                                          ***
 *******************************************************************
*/

/*
 *******************************************************************
 *** This software is copyright 2006 by Michael H Riley          ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#include "wx/frame.h"
#include "wx/graphics.h"

#include "main.h"
#include "pixie.h"
#include "vip2statusbar.h"

Pixie::Pixie(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType)
: Video(title, pos, size)
{
	computerType_ = computerType;
	videoType_ = VIDEOPIXIE;
	runningComputer_ = p_Main->getRunningComputerStr();

	if (!p_Main->isFullScreenFloat())
		xZoomFactor_ = (int) xZoomFactor_;

	if ((computerType == VIP && p_Main->getVipHighRes()))
	{
		highRes_ = 2;
		xZoomFactor_ = zoomfactor/highRes_;
	}
	else
	{
		highRes_ = 1;
		xZoomFactor_ = zoomfactor;
	}
	graphicsX_ = 8*highRes_;
    videoWidth_ = 64*highRes_;
	backGroundInit_ = 1;
	backGround_ = 1;

	this->SetClientSize(size);

	videoScreenPointer = new VideoScreen(this, size, zoom, computerType, xZoomFactor_);

	if ((computerType_ == TMC2000) || (computerType_ == NANO) || (computerType_ == COSMICOS) || (computerType_ == ETI) || (computerType_ == VICTORY))
		videoHeight_ = 192;
	else
		videoHeight_ = 128;

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif
	if (computerType_ == VIPII)
	{
		vipIIStatusBarPointer = new VipIIStatusBar(this);
		SetStatusBar(vipIIStatusBarPointer);
	}

	screenCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
	dcMemory.SelectObject(*screenCopyPointer);
	
#if defined(__WXMAC__)
	gc = wxGraphicsContext::Create(dcMemory);
	gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

	fullScreenSet_ = false;
	zoom_ = zoom;

	double intPart;
	zoomFraction_ = (modf(zoom_, &intPart) != 0);

	offsetX_ = 0;
	offsetY_ = 0;
    
    defineColours(computerType_);

	plotListPointer = NULL;
}

Pixie::~Pixie()
{
	PlotList *temp;

	dcMemory.SelectObject(wxNullBitmap);
	delete screenCopyPointer;
	delete videoScreenPointer;
#if defined(__WXMAC__)
	delete gc;
#endif
	if (updatePlot_ > 0)
	{
		while(plotListPointer != NULL)
		{
			temp = plotListPointer;
			plotListPointer = temp->nextPlot;
			delete temp;
		}
	}
	if (computerType_ == VIPII)
		delete vipIIStatusBarPointer;
}

void Pixie::reset()
{
	graphicsOn_ = false;
    videoScreenPointer->disableScreen(colour_[backGround_], videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
}

void Pixie::configurePixie(ElfPortConfiguration portConf)
{
	wxString runningComp = p_Main->getRunningComputerStr();

	p_Computer->setOutType(portConf.pixieOutput, PIXIEOUT);
	p_Computer->setCycleType(VIDEOCYCLE, PIXIECYCLE);
	p_Computer->setInType(portConf.pixieInput, PIXIEIN);
	p_Computer->setEfType(portConf.pixieEf, PIXIEEF);

	backGroundInit_ = 1;
	colourMask_ = 0;

	wxString printBuffer;
	p_Main->message("Configuring CDP 1861");

	printBuffer.Printf("	Output %d: disable graphics, input %d: enable graphics, EF %d: in frame indicator\n", portConf.pixieOutput, portConf.pixieInput, portConf.pixieEf);
	p_Main->message(printBuffer);
}

void Pixie::configurePixieStudio2()
{
	p_Computer->setOutType(1, PIXIEOUT);
	p_Computer->setCycleType(VIDEOCYCLE, PIXIECYCLE);
	p_Computer->setInType(1, PIXIEIN);
	p_Computer->setEfType(1, PIXIEEF);

	backGroundInit_ = 1;
	colourMask_ = 0;

	p_Main->message("Configuring CDP 1861");
	p_Main->message("	Output 1: disable graphics, input 1: enable graphics, EF 1: in frame indicator\n");
}

void Pixie::configurePixieCoinArcade()
{
    p_Computer->setOutType(2, PIXIEOUT);
    p_Computer->setCycleType(VIDEOCYCLE, PIXIECYCLE);
    p_Computer->setInType(1, PIXIEIN);
    p_Computer->setEfType(2, PIXIEEF);
  
    backGroundInit_ = 1;
    colourMask_ = 0;
    
    p_Main->message("	Output 1: enable graphics, input 1: enable graphics\n");
}

void Pixie::configurePixieVisicom()
{
	p_Computer->setOutType(1, PIXIEOUT);
	p_Computer->setCycleType(VIDEOCYCLE, PIXIECYCLE);
	p_Computer->setEfType(1, PIXIEEF);

	backGroundInit_ = 0;
	colourMask_ = 0; 

	p_Main->message("Configuring CDP 1861");
	p_Main->message("	Output 1: enable graphics, EF 1: in frame indicator\n");
}

void Pixie::configurePixieVip()
{
	p_Computer->setOutType(1, PIXIEOUT);
	p_Computer->setCycleType(VIDEOCYCLE, PIXIECYCLE);
	p_Computer->setInType(1, PIXIEIN);
	p_Computer->setEfType(1, PIXIEEF);

	if (p_Main->getVipVp590())
	{
		backGroundInit_ = 8;
		colourMask_ = 0;
		p_Computer->setOutType(5, VIPOUT5);

		p_Main->message("Configuring CDP 1861/1862");
		p_Main->message("	Output 1: disable graphics, input 1: enable graphics, EF 1: in frame indicator");
		p_Main->message("	Output 5: step background colour\n");
	}
	else
	{
		backGroundInit_ = 9;
		colourMask_ = 7;

		p_Main->message("Configuring CDP 1861");
		p_Main->message("	Output 1: disable graphics, input 1: enable graphics, EF 1: in frame indicator\n");
	}
}

void Pixie::configurePixieVelf()
{
	p_Computer->setOutType(1, PIXIEOUT);
	p_Computer->setCycleType(VIDEOCYCLE, PIXIECYCLE);
	p_Computer->setInType(1, PIXIEIN);
	p_Computer->setEfType(1, PIXIEEF);

	backGroundInit_ = 1;
	colourMask_ = 0;

	p_Main->message("Configuring CDP 1861");
	p_Main->message("	Output 1: disable graphics, input 1: enable graphics, EF 1: in frame indicator\n");
}

void Pixie::configurePixieVipII()
{
	p_Computer->setOutType(1, PIXIEOUT);
	p_Computer->setCycleType(VIDEOCYCLE, PIXIECYCLE);
	p_Computer->setInType(1, PIXIEIN);
	p_Computer->setEfType(1, PIXIEEF); 

	backGroundInit_ = 8;
	colourMask_ = 0; 
	p_Computer->setOutType(5, VIPOUT5);

	p_Main->message("Configuring CDP 1862");
	p_Main->message("	Output 1: disable graphics, input 1: enable graphics, EF 1: in frame indicator");
	p_Main->message("	Output 5: step background colour\n");

	vipIIStatusBarPointer->initVipIIBar();
}

void Pixie::configurePixieTmc1800()
{
	p_Computer->setCycleType(VIDEOCYCLE, PIXIECYCLE);
	p_Computer->setInType(1, PIXIEIN);
	p_Computer->setInType(4, PIXIEOUT);
	p_Computer->setEfType(1, PIXIEEF);

	backGroundInit_ = 1;
	colourMask_ = 0;

	p_Main->message("Configuring CDP 1861");
	p_Main->message("	Input 1: enable graphics, input 4: disable graphics");
	p_Main->message("	EF 1: in frame indicator\n");
}

void Pixie::configurePixieTelmac()
{
	p_Computer->setOutType(1, PIXIEBACKGROUND);
	p_Computer->setCycleType(VIDEOCYCLE, PIXIECYCLE);
	p_Computer->setInType(1, PIXIEIN);
	p_Computer->setInType(4, PIXIEOUT);
	p_Computer->setEfType(1, PIXIEEF);

	backGroundInit_ = 8;
	colourMask_ = 0;

	p_Main->message("Configuring CDP 1864");

	p_Main->message("	Output 1: switch background colour");
	p_Main->message("	Input 1: enable graphics, input 4: disable graphics");
	p_Main->message("	EF 1: in frame indicator\n");
}

void Pixie::configurePixieVictory()
{
	p_Computer->setOutType(1, PIXIEBACKGROUND);
	p_Computer->setCycleType(VIDEOCYCLE, PIXIECYCLE);
	p_Computer->setInType(1, PIXIEIN);
	p_Computer->setEfType(1, PIXIEEF);

	backGroundInit_ = 8;
	colourMask_ = 0;

	p_Main->message("Configuring CDP 1864");

	p_Main->message("	Output 1: switch background colour, output 4: tone latch");
	p_Main->message("	Input 1: enable graphics");
	p_Main->message("	EF 1: in frame indicator\n");
}

void Pixie::configurePixieNano()
{
	p_Computer->setOutType(1, PIXIEOUT);
	p_Computer->setCycleType(VIDEOCYCLE, PIXIECYCLE);
	p_Computer->setInType(1, PIXIEIN);
	p_Computer->setInType(4, PIXIEOUT);
	p_Computer->setEfType(1, PIXIEEF);

	backGroundInit_ = 1;
	colourMask_ = 0;

	p_Main->message("Configuring CDP 1864");
	p_Main->message("	Input 1: enable graphics, input 4: disable graphics");
	p_Main->message("	EF 1: in frame indicator\n");

	if (p_Main->getSound(NANO) == 0)
		p_Computer->setSoundFollowQ(false);
	else
		p_Computer->setSoundFollowQ(true);
}

void Pixie::configurePixieCosmicos()
{
	p_Computer->setCycleType(VIDEOCYCLE, PIXIECYCLE);
	p_Computer->setOutType(2, PIXIEIN);
	p_Computer->setInType(1, PIXIEIN);
	p_Computer->setInType(2, PIXIEOUT);
	p_Computer->setEfType(3, COSMICOSREQ);

	backGroundInit_ = 1;
	colourMask_ = 0;

	p_Main->message("Configuring CDP 1864");
	p_Main->message("	Q=0: Input 1: enable graphics, input 2: disable graphics");
	p_Main->message("	Q=1: Output 2: tone latch");
	p_Main->message("	EF 3: in frame indicator (when graphics enabled)\n");

}

void Pixie::configurePixieEti()
{
	p_Computer->setOutType(1, PIXIEBACKGROUND);
	p_Computer->setCycleType(VIDEOCYCLE, PIXIECYCLE);
	p_Computer->setInType(1, PIXIEIN);
	p_Computer->setInType(4, PIXIEOUT);
	p_Computer->setEfType(1, PIXIEEF);

	backGroundInit_ = 8;
	colourMask_ = 7;

	p_Main->message("Configuring CDP 1864");

	p_Main->message("	Output 1: switch background colour, output 4: tone latch");
	p_Main->message("	Input 1: enable graphics, input 4: disable graphics");
	p_Main->message("	EF 1: in frame indicator\n");
}

void Pixie::initiateColour(bool colour)
{
	if (colour)
		colourMask_ = 0;
	else
		colourMask_ = 7;
}

void Pixie::initPixie()
{
	vidInt_ = 0;
	vidCycle_ = 0;
	pixieEf_ = 1;

	for (int x=0; x<128; x++) for (int y=0; y<192; y++)
	{
		pbacking_[x][y] = 0;
		color_[x][y] = 0;
	}

	this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_*xZoomFactor_, (videoHeight_+2*borderY_[videoType_])*zoom_);

	graphicsOn_ = false;
	graphicsNext_ = 0;
	graphicsMode_ = 0;

	newBackGround_ = false;
	reBlit_ = true;
	reDraw_ = false;
	updatePlot_ = 0;
	backGround_ = backGroundInit_;

	changeScreenSize();
	videoScreenPointer->disableScreen(colour_[backGround_], videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
    drawScreen();
}

Byte Pixie::efPixie()
{
	return pixieEf_;
}

Byte Pixie::inPixie()
{
	graphicsOn_ = true;
	reBlit_ = true;
	return 0;
}

void Pixie::outPixie()
{
	graphicsOn_ = false;
	if (computerType_ == ETI)
		videoScreenPointer->disableScreen(colour_[backGround_], videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
}

void Pixie::outPixieBackGround()
{
	graphicsOn_ = true;
	backGround_++;
	if (backGround_ == 12)  backGround_ = 8;
	newBackGround_ = true;
	reBlit_ = true;
	drawScreen();
}

void Pixie::cyclePixie()
{
	int j;
	Byte v, vram1, vram2;
	int color;

	if (graphicsNext_ == 0)
	{
		p_Computer->debugTrace("----  H.Sync");
		graphicsMode_++;
		if (graphicsMode_ == 60) pixieEf_ = 0;
		if (graphicsMode_ == 64) pixieEf_ = 1;
		if (graphicsMode_ == 188) pixieEf_ = 0;
		if (graphicsMode_ == 192) pixieEf_ = 1;
		if (graphicsMode_ >= 262)
		{
			if (changeScreenSize_)
			{
				changeScreenSize();
				if (!fullScreenSet_)
					p_Main->pixieBarSizeEvent();
				changeScreenSize_ = false;
			}
			graphicsMode_ = 0;
			copyScreen();
			videoSyncCount_++;
		}

	}
	if (graphicsNext_ == 2)
	{
		if (graphicsMode_ == 62)
		{
			if (graphicsOn_)
			{
				p_Computer->pixieInterrupt();
				vidInt_ = 1;
				p_Computer->setCycle0();
			}
			else vidInt_ = 0;
		}
	}
	if (graphicsMode_ >= 64 && graphicsMode_ <=191 && graphicsOn_ && vidInt_ == 1 && graphicsNext_ >=4 && graphicsNext_ < (4+graphicsX_))
	{
		j = 0;
		while(graphicsNext_ >= 4 && graphicsNext_ < (4+graphicsX_))
		{
			graphicsNext_ ++;
			if (computerType_ == VISICOM)
			{
				p_Computer->visicomDmaOut(&vram1, &vram2);
				for (int i=0; i<8; i++)
				{
					color = 0;
					if (vram1 & 128)	color |= 1;
					if (vram2 & 128)	color |= 2;
					plot(j+i, (int)graphicsMode_-64, 1, color);
					vram1 <<= 1;
					vram2 <<= 1;
				}
			}
			else
			{
				v = p_Computer->pixieDmaOut(&color);
				for (int i=0; i<8; i++)
				{
					plot(j+i, (int)graphicsMode_-64,(v & 128) ? 1 : 0, (color|colourMask_)&7);
					if (graphicsX_ == 16)
						plot(j+i, (int)graphicsMode_-63,(v & 128) ? 1 : 0, (color|colourMask_)&7);
					v <<= 1;
				}
			}
			j += 8;
		}
		if (graphicsX_ == 16)
			graphicsMode_++;
		p_Computer->setCycle0();
		graphicsNext_ -= 1;
	}
	graphicsNext_ += 1;
	if (graphicsNext_ > (5+graphicsX_))
		graphicsNext_ = 0;
}

void Pixie::cyclePixieCoinArcade()
{
    int j;
    Byte v;
    int color;
    
    if (graphicsNext_ == 0)
    {
        p_Computer->debugTrace("----  H.Sync");
        graphicsMode_++;
        if (graphicsMode_ >= 262)
        {
            if (changeScreenSize_)
            {
                changeScreenSize();
                if (!fullScreenSet_)
                    p_Main->pixieBarSizeEvent();
                changeScreenSize_ = false;
            }
            graphicsMode_ = 0;
            copyScreen();
            videoSyncCount_++;
        }
    }
    if (graphicsNext_ == 2)
    {
        if (graphicsMode_ == 62)
        {
            if (graphicsOn_)
            {
                p_Computer->pixieInterrupt();
                vidInt_ = 1;
                p_Computer->setCycle0();
            }
            else vidInt_ = 0;
        }
    }
    if (graphicsMode_ >= 72 && graphicsMode_ <=199 && graphicsOn_ && vidInt_ == 1 && graphicsNext_ >=4 && graphicsNext_ < (4+graphicsX_))
    {
        j = 0;
        while(graphicsNext_ >= 4 && graphicsNext_ < (4+graphicsX_))
        {
            graphicsNext_ ++;
            v = p_Computer->pixieDmaOut(&color);
            for (int i=0; i<8; i++)
            {
                plot(j+i, (int)graphicsMode_-64,(v & 128) ? 1 : 0, (color|colourMask_)&7);
                plot(j+i, (int)graphicsMode_-63,(v & 128) ? 1 : 0, (color|colourMask_)&7);
                plot(j+i, (int)graphicsMode_-62,(v & 128) ? 1 : 0, (color|colourMask_)&7);
                plot(j+i, (int)graphicsMode_-61,(v & 128) ? 1 : 0, (color|colourMask_)&7);
                v <<= 1;
            }
            j += 8;
        }
        graphicsMode_+=3;
        p_Computer->setCycle0();
        graphicsNext_ -= 1;
    }
    graphicsNext_ += 1;
    if (graphicsNext_ > (5+graphicsX_))
        graphicsNext_ = 0;
}

void Pixie::cyclePixieTelmac()
{
	int j;
	Byte v;
	int color;

	if (graphicsNext_ == 0)
	{
		p_Computer->debugTrace("----  H.Sync");
		graphicsMode_++;
		if (graphicsMode_ == 72) pixieEf_ = 0;
		if (graphicsMode_ == 76) pixieEf_ = 1;
		if (graphicsMode_ == 264) pixieEf_ = 0;
		if (graphicsMode_ == 268) pixieEf_ = 1;
		if (graphicsMode_ >= 312)
		{
			if (changeScreenSize_)
			{
				changeScreenSize();
				if (!fullScreenSet_)
					p_Main->pixieBarSizeEvent();
				changeScreenSize_ = false;
			}
			graphicsMode_ = 0;
			copyScreen();
			videoSyncCount_++;
		}
	}
	if (graphicsNext_ == 2)
	{
		if (graphicsMode_ == 74)
		{
			if (graphicsOn_)
			{
				p_Computer->pixieInterrupt();
				vidInt_ = 1;
				p_Computer->setCycle0();
			}
			else vidInt_ = 0;
		}
	}
	if (graphicsMode_ >= 76 && graphicsMode_ <=267 && graphicsOn_ && vidInt_ == 1 && graphicsNext_ >=4 && graphicsNext_ <=11)
	{
		j = 0;
		while(graphicsNext_ >= 4 && graphicsNext_ <= 11)
		{
			graphicsNext_ ++;
			v = p_Computer->pixieDmaOut(&color);
			for (int i=0; i<8; i++)
			{
				plot(j+i, (int)graphicsMode_-76, (v & 128) ? 1 : 0, (color|colourMask_)&7);
				v <<= 1;
			}
			j += 8;
		}
		p_Computer->setCycle0();
		graphicsNext_ -= 1;
	}
	graphicsNext_ += 1;
	if (graphicsNext_ > 13)
		graphicsNext_ = 0;
}

void Pixie::copyScreen()
{
	PlotList *temp;

	if (!graphicsOn_ && (computerType_ == ETI))  return;

	if (reColour_)
	{
		for (int i=0; i<numberOfColours_; i++)
		{
			colour_[i] = colourNew_[i];
			brushColour_[i] = brushColourNew_[i];
			penColour_[i] = penColourNew_[i];
		}
		for (int i=0; i<10; i++)
		{
			borderX_[i] = borderXNew_[i];
			borderY_[i] = borderYNew_[i];
		}
		xZoomFactor_ = xZoomFactorNew_/highRes_;
		videoScreenPointer->setScale(xZoomFactor_);
		setScreenSize();
		reDraw_ = true;
		reBlit_ = true;
		newBackGround_ = true;
		reColour_ = false;
	}

	if (reDraw_)
		drawScreen();

	if (extraBackGround_ && newBackGround_) 
		drawExtraBackground(colour_[backGround_]);

	if (reBlit_ || reDraw_ )
	{
		videoScreenPointer->blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);
		reBlit_ = false;
		reDraw_ = false;
		if (updatePlot_ > 0)
		{
			updatePlot_ = 0;
			while(plotListPointer != NULL)
			{
				temp = plotListPointer;
				plotListPointer = temp->nextPlot;
				delete temp;
			}
		}
	}
	if (updatePlot_ > 0)
	{
		updatePlot_ = 0;
		while(plotListPointer != NULL)
		{
			videoScreenPointer->blit(offsetX_+ plotListPointer->x, offsetY_+plotListPointer->y, 1, 1, &dcMemory, offsetX_+plotListPointer->x, offsetY_+plotListPointer->y);
			temp = plotListPointer;
			plotListPointer = temp->nextPlot;
			delete temp;
		}
	}
}

void Pixie::drawScreen()
{
	int v;
	int color;

	setColour(backGround_);
	drawRectangle(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);

	for (int x=0; x<videoWidth_; x++)
	{
		for (int y=0; y<videoHeight_; y++)
		{
			v = pbacking_[x][y];
			color = color_[x][y];
			if (v)
			{
				setColour(color);
				drawPoint(x+offsetX_, y+offsetY_);
			}
		}
	}
}

void Pixie::plot(int x, int y, int c, int color)
{
	wxPen penClr;

	if (pbacking_[x][y] == c)
		if (!c || (color_[x][y] == color))  return;

	color_[x][y] = color;
	pbacking_[x][y] = c;

	if (c)
	{
		setColour(color);
		penClr = penColour_[color];
	}
	else
	{
		setColour(backGround_);
		penClr = penColour_[backGround_];
	}
	drawPoint(x+offsetX_, y+offsetY_);

#if defined(__WXMAC__) || defined(__linux__)
    reBlit_ = true;
#else
	if (reBlit_)  return;
	if (zoomFraction_)
		reBlit_ = true;
	else
	{
		PlotList *temp = new PlotList;
		temp->x = x;
		temp->y = y;
		temp->penClr = penClr;
		temp->nextPlot = plotListPointer;
		plotListPointer = temp;
		updatePlot_++;
		if (updatePlot_ > 40)
			reBlit_ = true;
	}
#endif
}

void Pixie::setFullScreen(bool fullScreenSet)
{
	fullScreenSet_ = fullScreenSet;
	ShowFullScreen(fullScreenSet);
}

void Pixie::onF3()
{
	fullScreenSet_ = !fullScreenSet_;
	p_Main->eventVideoSetFullScreen(fullScreenSet_);
}

void Pixie::pixieBarSize()
{
	if (computerType_ == VIPII)
		vipIIStatusBarPointer->reDrawBar();
}

void Pixie::reDrawBar()
{
	if (computerType_ == VIPII)
		vipIIStatusBarPointer->reDrawBar();
}

void Pixie::updateLedStatus(int led, bool status)
{
	if (computerType_ == VIPII)
		vipIIStatusBarPointer->updateLedStatus(led, status);
}



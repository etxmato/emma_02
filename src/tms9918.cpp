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

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
#include "app_icon.xpm"
#endif

#include "wx/dcbuffer.h"
#include "main.h"
#include "tms9918.h"

Tms9918::Tms9918(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock)
: Video(title, pos, size)
{
	computerType_ = computerType;
	zoom_ = zoom;
	clock_ = clock;

	double intPart;
	zoomFraction_ = (modf(zoom_, &intPart) != 0);

	defineColours(computerType_);
	videoType_ = VIDEOTMS;

	videoScreenPointer = new VideoScreen(this, size, zoom, computerType);

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	screenCopyPointer = new wxBitmap(320, 240);
	dcMemory.SelectObject(*screenCopyPointer);

#if defined(__WXMAC__)
	gc = wxGraphicsContext::Create(dcMemory);
	gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

    switch (p_Main->getCpuStartupVideoRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<16384; i++) tmsMemory_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<16384; i++) tmsMemory_[i] = rand() % 0x100;
        break;
            
        case STARTUP_DYNAMIC:
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<16384; i++) tmsMemory_[i] = p_Computer->getDynamicByte(i);
        break;
    }
    
    switch (p_Main->getCpuStartupRegisters())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<8; i++) registers_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<8; i++) registers_[i] = rand() % 0x100;
        break;
    }
    
	mode_ = 0;
	nameAddress_ = 0;
	colorAddress_ = 0;
	patternAddress_ = 0;
	currentAddress_ = 0;
	toggle_ = 0;

	offsetX_ = 0;
	offsetY_ = 0;
	backGroundX_ = 0;
	backGroundY_ = 0;

	setCycle();

	cycleValue_ = cycleSize_;
	newBackGround_ = false;
	fullScreenSet_ = false;
	updateTile_ = 0;
	tileListPointer = NULL;

	videoWidth_ = 256;
	videoHeight_ = 192;

	this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);
	changeScreenSize();
}

Tms9918::~Tms9918()
{
	TileList *temp;

	dcMemory.SelectObject(wxNullBitmap);
	delete screenCopyPointer;
	delete videoScreenPointer;
#if defined(__WXMAC__)
	delete gc;
#endif
	if (updateTile_ > 0)
	{
		while(tileListPointer != NULL)
		{
			temp = tileListPointer;
			tileListPointer = temp->nextTile;
			delete temp;
		}
	}
}

void Tms9918::configure(ElfPortConfiguration elfPortConf)
{
//	int highOutput, lowOutput;

	wxString runningComp = p_Main->getRunningComputerStr();

//	highOutput = p_Main->getConfigItem(runningComp +"/TmsModeHighOutput", 5l);
//	lowOutput = p_Main->getConfigItem(runningComp +"/TmsModeLowOutput", 6l);

	p_Computer->setOutType(elfPortConf.tmsModeHighOutput, TMSHIGHOUT);
	p_Computer->setOutType(elfPortConf.tmsModeLowOutput, TMSLOWOUT);
	p_Computer->setCycleType(VIDEOCYCLE, TMSCYCLE);

	wxString printBuffer;
	p_Main->message("Configuring TMS 9918");

	printBuffer.Printf("	Output %d: register port, output %d: data port\n", elfPortConf.tmsModeHighOutput, elfPortConf.tmsModeLowOutput);
	p_Main->message(printBuffer);
}

void Tms9918::modeHighOut(Byte value)
{
	if (toggle_)
	{
		if ((value & 0xf8) == 0x80 )
		{
			registers_[value & 7] = value_;
			nameAddress_ = (registers_[2] & 0xf) << 10;
			colorAddress_ = registers_[3] << 6;
			patternAddress_ = (registers_[4] & 0x7) << 11;
			mode_ = 0; // Graphics I
			if (registers_[0] & 2) mode_ = 1; // Graphics II
			if (registers_[1] & 8) mode_ = 2; // Multicolor
			if (registers_[1] & 16) mode_ = 4; // Text
			if (mode_ == 1)
			{
				colorAddress_ = (registers_[3] & 128) ? 0x2000 : 0;
				patternAddress_ = (registers_[4] & 4) ? 0x2000 : 0;
			}
			reBlit_ = true;
			drawScreen();
		}
		if ((value & 0xc0) == 0x40)
		{
			currentAddress_ = value_ +((value & 0x3f) << 8);
		}
		toggle_ = 0;
	}
	else
	{
		value_ = value;
		toggle_ = 1;
	}
}

void Tms9918::modeLowOut(Byte value)
{
	int  p;
	Word addr;

	addr = currentAddress_;
	tmsMemory_[currentAddress_++] = value;
	Word memoryStart = p_Computer->getDebugMemoryStart();
	if (addr >= memoryStart && addr<(memoryStart + 256))
		p_Main->updateDebugMemory(addr);

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
				wxMutexGuiEnter();
#endif
	switch(mode_)
	{
		case 0:
			if (addr >= nameAddress_ && addr < nameAddress_+768)
			{
				drawTile(addr - nameAddress_);
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
				wxMutexGuiLeave();
#endif
				return;
			}
			if (addr >= colorAddress_ && addr < colorAddress_+32)
			{
				p = addr - colorAddress_;
				for (int i=0; i<768; i++)
					if ((tmsMemory_[nameAddress_+i] >> 3) == p) drawTile(i);
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
				wxMutexGuiLeave();
#endif
				return;
			}
			if (addr >= patternAddress_ && addr < patternAddress_+2048)
			{
				p = (addr - patternAddress_) >> 3;
				for (int i=0; i<768; i++)
					if (tmsMemory_[nameAddress_+i] == p) drawTile(i);
			}
		break;

		case 1:
			if (addr >= nameAddress_ && addr < nameAddress_+768)
			{
				drawTile(addr - nameAddress_);
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
				wxMutexGuiLeave();
#endif
				return;
			}
			if (addr >= colorAddress_ && addr < colorAddress_+2048)
			{
				p = addr - colorAddress_;
				for (int i=0; i<256; i++)
					if (tmsMemory_[nameAddress_+i] == p) drawTile(i);
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
				wxMutexGuiLeave();
#endif
				return;
			}
			if (addr >= colorAddress_+2048 && addr < colorAddress_+4096)
			{
				p = addr -(colorAddress_-2048);
				for (int i=256; i<512; i++)
					if (tmsMemory_[nameAddress_+i] == p) drawTile(i);
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
				wxMutexGuiLeave();
#endif
				return;
			}
			if (addr >= colorAddress_+4096 && addr < colorAddress_+6144)
			{
				p = addr -(colorAddress_-4096);
				for (int i=512; i<768; i++)
					if (tmsMemory_[nameAddress_+i] == p) drawTile(i);
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
				wxMutexGuiLeave();
#endif
				return;
			}
			if (addr >= patternAddress_ && addr < patternAddress_+2048)
			{
				p = (addr -(patternAddress_-2048)) >> 3;
				for (int i=0; i<256; i++)
					if (tmsMemory_[nameAddress_+i] == p) drawTile(i);
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
				wxMutexGuiLeave();
#endif
				return;
			}
			if (addr >= patternAddress_+2048 && addr < patternAddress_+4096)
			{
				p = (addr -(patternAddress_-2048)) >> 3;
				for (int i=256; i<512; i++)
					if (tmsMemory_[nameAddress_+i] == p) drawTile(i);
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
				wxMutexGuiLeave();
#endif
				return;
			}
			if (addr >= patternAddress_+4096 && addr < patternAddress_+6144)
			{
				p = (addr -(patternAddress_-4096)) >> 3;
				for (int i=512; i<768; i++)
					if (tmsMemory_[nameAddress_+i] == p) drawTile(i);
			}
		break;

		case 4:
			if (addr >= nameAddress_ && addr < nameAddress_+960)
			{
				drawTile(addr - nameAddress_);
			}
		break;
	}
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	wxMutexGuiLeave();
#endif
}

void Tms9918::cycleTms()
{
	cycleValue_ --;
	if (cycleValue_ == 0)
	{
		cycleValue_ = cycleSize_;
		copyScreen();
		videoSyncCount_++;
		if (changeScreenSize_)
		{
			changeScreenSize();
			changeScreenSize_ = false;
		}
	}
}

void Tms9918::setClock(double clock)
{
	clock_ = clock;
	reCycle_ = true;
}

void Tms9918::setCycle()
{
	float clockPeriod = (float)(1/5.37);
	float fieldTime = (float) clockPeriod * 342 * 262;

	cycleSize_ = (int)(fieldTime / ((1/clock_) * 8));
}

void Tms9918::copyScreen()
{
	TileList *temp;

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
		setScreenSize();
		reDraw_ = true;
		reBlit_ = true;
		newBackGround_ = true;
		reColour_ = false;
	}

	if (reCycle_)
		setCycle();

	if (reDraw_)
		drawScreen();

	if (extraBackGround_ && newBackGround_) 
		drawExtraBackground(colour_[(registers_[7] & 0xf)+16]);

	if (reBlit_ || reDraw_)
	{
		videoScreenPointer->blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);
		reBlit_ = false;
		reDraw_ = false;
		if (updateTile_ > 0)
		{
			updateTile_ = 0;
			while(tileListPointer != NULL)
			{
				temp = tileListPointer;
				tileListPointer = temp->nextTile;
				delete temp;
			}
		}
	}
	if (updateTile_ > 0)
	{
		updateTile_ = 0;
		while(tileListPointer != NULL)
		{
			videoScreenPointer->blit(offsetX_+ tileListPointer->x-backGroundX_, offsetY_+tileListPointer->y-backGroundY_, tileListPointer->size, 8, &dcMemory, offsetX_+tileListPointer->x, offsetY_+tileListPointer->y);
			temp = tileListPointer;
			tileListPointer = temp->nextTile;
			delete temp;
		}
	}
}

void Tms9918::drawTile(Word tile)
{
	int p;
	int c;
	int b;
	int cl;
	int x,y, size;
	Word ofs;

	p = tmsMemory_[nameAddress_+tile];
	switch(mode_)
	{
		case 0:
			c = tmsMemory_[colorAddress_ +(p >> 3)];
			x = (tile % 32)*8;
			y = (tile / 32)*8;
			size = 8;

			for (int py=0; py<8; py++)
			{
				b = tmsMemory_[patternAddress_+p*8+py];
				for (int px=0; px<8; px++)
				{
					cl = (b & 128) ? c>>4 : c & 0xf;
					if (cl == 0) cl = registers_[7] & 0xf;
					if (cl == 0) cl = 1;
					setColourMutex(cl+16);
					drawPoint(x+px+offsetX_, y+py+offsetY_);
					b = (b << 1) & 0xff;
				}
			}
		break;

		case 1:
			x = tile % 32;
			y = tile / 32;
			size = 8;

			ofs = 0;
			if (y>7) ofs = 2048;
			if (y>15) ofs = 4096;

			x = x*8;
			y = y*8;

			for (int py=0; py<8; py++)
			{
				b = tmsMemory_[ofs + patternAddress_ + p*8 + py];
				c = tmsMemory_[ofs + colorAddress_ + p*8 + py];
				for (int px=0; px<8; px++)
				{
					cl = (b & 128) ? c>>4 : c & 0xf;
					if (cl == 0) cl = registers_[7] & 0xf;
					if (cl == 0) cl = 1;
 					setColourMutex(cl+16);
					drawPoint(x+px+offsetX_, y+py+offsetY_);
					b = (b << 1) & 0xff;
				}
			}
		break;

		case 4:
			c = registers_[7];
			x = (tile % 40)*6+8;
			y = (tile / 40)*8;
			size = 6;

			setColourMutex((c & 0xf)+16);
			drawRectangle(x+offsetX_, y+offsetY_, 6, 8);

			for (int py=0; py<8; py++)
			{
				b = tmsMemory_[patternAddress_+p*8+py];
				for (int px=0; px<6; px++)
				{
					if (b & 128)
					{
						cl = (c>>4) & 0xf;
						if (cl == 0) cl = registers_[7] & 0xf;
						setColourMutex(cl+16);
						drawPoint(x+px+offsetX_, y+py+offsetY_);
					}
					b = (b << 1) & 0xff;
				}
			}
		break;

		default:
			x = 0;
			y = 0;
			size = 0;
		break;
	}
#if defined(__WXMAC__) || defined(__linux__)
    reBlit_ = true;
#else
	if (reBlit_)  return;
	if (zoomFraction_)
		reBlit_ = true;
	else
	{
		TileList *temp = new TileList;
		temp->x = x;
		temp->y = y;
		temp->size = size;
		temp->nextTile = tileListPointer;
		tileListPointer = temp;
		updateTile_++;
		if (updateTile_ > 40)
			reBlit_ = true;
	}
#endif
}

void Tms9918::drawScreen()
{
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	wxMutexGuiEnter();
#endif
	int cl = registers_[7] & 0xf;
	setColourMutex(cl+16);
	drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);
	if (mode_ == 0)
	{
		for (int x=0; x<768; x++)
			drawTile(x);
	}
	if (mode_ == 1)
	{
		for (int x=0; x<768; x++)
			drawTile(x);
	}
	if (mode_ == 4)
	{
		for (int x=0; x<960; x++)
			drawTile(x);
	}
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	wxMutexGuiLeave();
#endif
}

void Tms9918::setFullScreen(bool fullScreenSet)
{
	fullScreenSet_ = fullScreenSet;
	ShowFullScreen(fullScreenSet);
}

void Tms9918::onF3()
{
	fullScreenSet_ = !fullScreenSet_;
	p_Main->eventVideoSetFullScreen(fullScreenSet_);
}


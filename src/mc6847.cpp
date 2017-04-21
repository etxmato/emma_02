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

#include "main.h"
#include "mc6847.h"

#define MC_INV 1
#define MC_EXT 2
#define MC_CSS 3
#define MC_AS 4
#define MC_AG 5
#define MC_GM0 6
#define MC_GM1 7
#define MC_GM2 8

#define TEXTCOL 6
#define GRAPHIC 8
#define BACKGROUND 5

mc6847::mc6847(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock, ElfPortConfiguration elfPortConf)
: Video(title, pos, size)
{
	computerType_ = computerType;
	clock_ = clock;

	switch(computerType_)
	{
		case ELF:
			elfTypeStr_ = "Elf";
		break;

		case ELFII:
			elfTypeStr_ = "ElfII";
		break;

		case SUPERELF:
			elfTypeStr_ = "SuperElf";
		break;
	}
	readCharRomFile(p_Main->getCharRomDir(computerType_), p_Main->getCharRomFile(computerType_));
    mc6847RamStart_ = elfPortConf.mc6847StartRam; //p_Main->getConfigItem(elfTypeStr_+"/mc6847StartRam",0xE000l);
//	Word end = p_Main->getConfigItem(elfTypeStr_+"/mc6847EndRam", 0xE3FFl);
	p_Computer->defineMemoryType(mc6847RamStart_, elfPortConf.mc6847EndRam, MC6847RAM);
	mc6847RamMask_ = elfPortConf.mc6847EndRam - mc6847RamStart_;

    switch (p_Main->getCpuStartupVideoRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<=mc6847RamMask_; i++) mc6847ram_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<=mc6847RamMask_; i++) mc6847ram_[i] = rand() % 0x10000;
        break;
            
        case STARTUP_DYNAMIC:
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<=mc6847RamMask_; i++) mc6847ram_[i] = p_Computer->getDynamicWord(i);
        break;
    }
    

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	videoWidth_ = size.GetWidth() / zoom;
	videoHeight_ = size.GetHeight() / zoom;

	charHeight_ = 12;
	charWidth_ = 8;

	charLine_ = videoWidth_ / charWidth_;
	rows_ = videoHeight_ / charHeight_;

	fullScreenSet_ = false;

	if (videoHeight_ == 192)
		addLine_ = 1;
	else
		addLine_ = 2;

	videoHeight_ = rows_*charHeight_*addLine_;

	offsetX_ = 0;
	offsetY_ = 0;
	reDraw_ = true;
	reBlit_ = false;
	newBackGround_ = false;
	updateCharacter_ = 0;

//	for (int i=0; i<(rows_*charLine_); i++)
//		mc6847ram_[i] = 32;

	screenCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
	dcMemory.SelectObject(*screenCopyPointer);

#if defined(__WXMAC__)
	gc = wxGraphicsContext::Create(dcMemory);
	gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

	videoScreenPointer = new VideoScreen(this, size, zoom, computerType);

	setCycle();

	defineColours(computerType_);
	backGround_ = BACKGROUND;
	videoType_ = VIDEO6847;

	cycleValue_ = cycleSize_;
	zoom_ = zoom;

	double intPart;
	zoomFraction_ = (modf(zoom_, &intPart) != 0);

	characterListPointer6847 = NULL;
	ag_ = 0;
	inv_ = 0;
	ext_ = 0;
	css_ = 0;
	as_ = 0;
	gm0_ = 0;
	gm1_ = 0;
	gm2_ = 0;
	outLatch_ = 0;

	this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);
}

mc6847::~mc6847()
{
	CharacterList6847 *temp;

	dcMemory.SelectObject(wxNullBitmap);
	delete screenCopyPointer;
	delete videoScreenPointer;
#if defined(__WXMAC__)
	delete gc;
#endif
	if (updateCharacter_ > 0)
	{
		while(characterListPointer6847 != NULL)
		{
			temp = characterListPointer6847;
			characterListPointer6847 = temp->nextCharacter;
			delete temp;
		}
	}
}

void mc6847::drawScreen()
{
	setColour(backGround_);
	drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);
	for (int addr=0; addr<(rows_*charLine_); addr++)
		draw(addr);
}

void mc6847::configure(ElfPortConfiguration elfPortConf)
{
//	int mc6847Out;
	wxString printBuffer;

//    mc6847Out = p_Main->getConfigItem(elfTypeStr_+"/MC6847Output", 5l);

	invBit_ = 16;
	extBit_ = 16;
	cssBit_ = 16;
	asBit_ = 16;
	agBit_ = 16;
	gm0Bit_ = 16;
	gm1Bit_ = 16;
	gm2Bit_ = 16;
	
    setMCBit(15, elfPortConf.mc6847b7); //p_Main->getConfigItem(elfTypeStr_+"/MC6847-B7", 0l));
	setMCBit(14, elfPortConf.mc6847b6); //p_Main->getConfigItem(elfTypeStr_+"/MC6847-B6", 0l));
	setMCBit(13, elfPortConf.mc6847b5); //p_Main->getConfigItem(elfTypeStr_+"/MC6847-B5", 0l));
	setMCBit(12, elfPortConf.mc6847b4); //p_Main->getConfigItem(elfTypeStr_+"/MC6847-B4", 0l));
	setMCBit(11, elfPortConf.mc6847b3); //p_Main->getConfigItem(elfTypeStr_+"/MC6847-B3", 3l));
	setMCBit(10, elfPortConf.mc6847b2); //p_Main->getConfigItem(elfTypeStr_+"/MC6847-B2", 4l));
	setMCBit(9, elfPortConf.mc6847b1); //p_Main->getConfigItem(elfTypeStr_+"/MC6847-B1", 6l));
	setMCBit(8, elfPortConf.mc6847b0); //p_Main->getConfigItem(elfTypeStr_+"/MC6847-B0", 5l));
	setMCBit(7, elfPortConf.mc6847dd7); //p_Main->getConfigItem(elfTypeStr_+"/MC6847-DD7", 1l));
	setMCBit(6, elfPortConf.mc6847dd6); //p_Main->getConfigItem(elfTypeStr_+"/MC6847-DD6", 0l));

	p_Computer->setOutType(elfPortConf.mc6847Output, MC6847OUT);

	p_Computer->setCycleType(VIDEOCYCLE, MC6847CYCLE);

	p_Main->message("Configuring MC6847");

	printBuffer.Printf("	Output %d: video mode\n", elfPortConf.mc6847Output);
	p_Main->message(printBuffer);
}

void mc6847::init6847()
{
	cycleValue_ = cycleSize_;
	reDraw_ = true;
	reBlit_ = false;
	newBackGround_ = false;
	updateCharacter_ = 0;
}

void mc6847::setMCBit(int bit, int selection)
{
	switch (selection)
	{
		case MC_INV:
			invBit_ = bit;
		break;

		case MC_EXT:
			extBit_ = bit;
		break;

		case MC_CSS:
			cssBit_ = bit;
		break;

		case MC_AS:
			asBit_ = bit;
		break;

		case MC_AG:
			agBit_ = bit;
		break;

		case MC_GM0:
			gm0Bit_ = bit;
		break;

		case MC_GM1:
			gm1Bit_ = bit;
		break;

		case MC_GM2:
			gm2Bit_ = bit;
		break;
	}
}

void mc6847::outMc6847(Byte v)
{
	int newMode, value;

	value = v * 256;

	newMode = (((value >> agBit_) & 1) == 1);
	if (newMode != ag_)
		reDraw_ = true;
	ag_ = newMode;

	inv_ = (value >> invBit_) & 1;
	ext_ = (value >> extBit_) & 1;
	css_ = (value >> cssBit_) & 1;
	as_ = (value >> asBit_) & 1;
	gm0_ = (value >> gm0Bit_) & 1;
	gm1_ = (value >> gm1Bit_) & 1;
	gm2_ = (value >> gm2Bit_) & 1;

	newMode = gm0_ + (gm1_ << 1) + (gm2_ << 2);
	if (newMode != graphicMode_)
		reDraw_ = true;
	graphicMode_ = newMode;

	if (ag_ == 0)
	{
		charHeight_ = 12;
		charWidth_ = 8;
	}
	else
	{
		switch(graphicMode_)
		{
			case 0:
				charHeight_ = 3;
				charWidth_ = 16;
				elementWidth_ = 4;
			break;

			case 1:
				charHeight_ = 3;
				charWidth_ = 16;
				elementWidth_ = 2;
			break;

			case 2:
				charHeight_ = 3;
				charWidth_ = 8;
				elementWidth_ = 2;
			break;

			case 3:
				charHeight_ = 2;
				charWidth_ = 16;
				elementWidth_ = 2;
			break;

			case 4:
				charHeight_ = 2;
				charWidth_ = 8;
				elementWidth_ = 2;
			break;

			case 5:
				charHeight_ = 1;
				charWidth_ = 16;
				elementWidth_ = 2;
			break;

			case 6:
				charHeight_ = 1;
				charWidth_ = 8;
				elementWidth_ = 2;
			break;

			case 7:
				charHeight_ = 1;
				charWidth_ = 8;
				elementWidth_ = 1;
			break;
		}
	}
	charLine_ = screenWidth_ / charWidth_;
	rows_ = screenHeight_ / charHeight_;

	videoWidth_ = charLine_*charWidth_;
	videoHeight_ = rows_*charHeight_*addLine_;

	outLatch_ = value & 0xf00;
}

void mc6847::cycle6847()
{
	cycleValue_--;
	if (cycleValue_ == (cycleSize_ - (int)(nonDisplay_/2)))
	{
		if (changeScreenSize_)
		{
			changeScreenSize();
			changeScreenSize_ = false;
		}
		copyScreen();
		videoSyncCount_++;
	}
	if (cycleValue_ <= 0)
	{
		cycleValue_ = cycleSize_;
	}
}

void mc6847::copyScreen()
{
	CharacterList6847 *temp;

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
		drawExtraBackground(colour_[backGround_]);

	if (reBlit_ || reDraw_)
	{
		videoScreenPointer->blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);
		reBlit_ = false;
		reDraw_ = false;
		if (updateCharacter_ > 0)
		{
			updateCharacter_ = 0;
			while(characterListPointer6847 != NULL)
			{
				temp = characterListPointer6847;
				characterListPointer6847 = temp->nextCharacter;
				delete temp;
			}
		}
	}
	if (updateCharacter_ > 0)
	{
		updateCharacter_ = 0;
		while(characterListPointer6847 != NULL)
		{
			videoScreenPointer->blit(offsetX_+(characterListPointer6847->x), offsetY_+(characterListPointer6847->y), charWidth_, charHeight_*addLine_, &dcMemory, offsetX_+characterListPointer6847->x, offsetY_+characterListPointer6847->y);
			temp = characterListPointer6847;
			characterListPointer6847 = temp->nextCharacter;
			delete temp;
		}
	}
}

void mc6847::setClock(double clock)
{
	clock_ = clock;
	reCycle_ = true;
}

void mc6847::setCycle()
{
	// DOT 3.579
	int fieldTime = (int)(262*(227.5*1/3.579));
	cycleSize_ = (int)(fieldTime / ((1/clock_) * 8));
	nonDisplay_ = (int) ((float)cycleSize_ / 262 * 96);
	reCycle_ = false;
}

void mc6847::write(Word addr, Byte value)
{
	mc6847ram_[addr-mc6847RamStart_] = value + outLatch_;
	Word memoryStart = p_Computer->getDebugMemoryStart();
	if ((addr-mc6847RamStart_) >= memoryStart && (addr-mc6847RamStart_)<(memoryStart + 256))
		p_Main->updateDebugMemory(addr-mc6847RamStart_);
	draw(addr-mc6847RamStart_);
}

Byte mc6847::read6847(Word addr)
{
	return (mc6847ram_[addr-mc6847RamStart_] & 0xff);
}

int mc6847::readDirect6847(Word addr)
{
	if (addr > mc6847RamMask_)
		return ((mc6847ram_[addr - mc6847RamMask_ - 1] >> 8) & 0xf);
	else
		return (mc6847ram_[addr] &0xff);
}

void mc6847::writeDirect6847(Word addr, int value)
{
	if (addr > mc6847RamMask_)
	{
		mc6847ram_[addr - mc6847RamMask_ - 1] = (mc6847ram_[addr - mc6847RamMask_ - 1]&0x0ff) | ((value&0xf) << 8);
		draw(addr - mc6847RamMask_ - 1);
	}
	else
	{
		mc6847ram_[addr] = (mc6847ram_[addr]&0xf00) | value;
		draw(addr);
	}
}

Byte mc6847::read6847CharRom(Word addr)
{
	return mc6847CharRom_[addr];
}

void mc6847::write6847CharRom(Word addr, Byte value)
{
	mc6847CharRom_[addr] = value;
	Word memoryStart = p_Computer->getDebugMemoryStart();
	if (addr >= memoryStart && addr<(memoryStart + 256))
		p_Main->updateDebugMemory(addr);
	reDraw_ = true;
}

Word mc6847::get6847RamMask()
{
 	return mc6847RamMask_;
}

void mc6847::draw(Word addr)
{
	int y = (addr/charLine_)*charHeight_*addLine_;
	int x = (addr%charLine_)*charWidth_;
	if (ag_ == 1)
		drawGraphic(x, y, mc6847ram_[addr]);
	else
		drawCharacter(x, y, mc6847ram_[addr]);
}

void mc6847::drawCharacter(wxCoord x, wxCoord y, int v)
{
	int line_byte, line;
	wxColour backGroundClr;
	wxColour foreGroundClr;

	if (cssBit_ < 12) 
		css_ = (v >> cssBit_) & 1;
	if (asBit_ < 12) 
		as_ = (v >> asBit_) & 1;
	if (invBit_ < 12) 
		inv_ = (v >> invBit_) & 1;
	if (extBit_ < 12) 
		ext_ = (v >> extBit_) & 1;

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	if (!wxIsMainThread())
		wxMutexGuiEnter();
#endif

	if (as_ == 0)
	{
		if (inv_)
		{
			backGroundClr = colour_[TEXTCOL+css_];
			foreGroundClr = colour_[backGround_];
		}
		else
		{
			backGroundClr = colour_[backGround_];
			foreGroundClr = colour_[TEXTCOL+css_];
		}

		if (ext_ == 1)
			v &= 0x7f;
		else
			v &= 0x3f;

		setColour(backGroundClr);
		drawRectangle(x+offsetX_, y+offsetY_, charWidth_, charHeight_*addLine_);

		setColour(foreGroundClr);

		line = 0;
		for (wxCoord j=y; j<y+charHeight_*addLine_; j+=addLine_)
		{
			if (ext_ == 1)
				line_byte = mc6847CharRom_[v*charHeight_+ line + 0x200];
			else
			{
				if ((line == 0) || (line == 1) || (line == 9) || (line == 10) || (line == 11))
					line_byte = 0;
				else
					line_byte = mc6847CharRom_[v*8 + line - 2];
			}
			for (wxCoord i=x; i<x+charWidth_; i++)
			{
				if (line_byte & 128)
				{
					drawPoint(i+offsetX_, j+offsetY_);
				}
				line_byte <<= 1;
			}
			line++;
		}
	}
	else
	{
		if (ext_ == 0)
		{
			foreGroundClr = colour_[GRAPHIC +((v >> 4) & 0x7)];

			v &= 0x0f;
			for (wxCoord j=y; j<y+charHeight_*addLine_; j+=6)
			{
				for (wxCoord i=x; i<x+charWidth_; i+=4)
				{
					if (v & 8)
					{
						setColour(foreGroundClr);
					}
					else
					{
						setColour(backGround_);
					}
					drawRectangle(i+offsetX_, j+offsetY_, 4, 6*addLine_);
					v = v << 1;
				}
			}
		}
		else
		{
			foreGroundClr = colour_[GRAPHIC+(((v >> 6) & 0x3) + (css_ << 2))];

			v &= 0x3f;
			for (wxCoord j=y; j<y+charHeight_*addLine_; j+=4)
			{
				for (wxCoord i=x; i<x+charWidth_; i+=4)
				{
					if (v & 0x20)
					{
						setColour(foreGroundClr);
					}
					else
					{
						setColour(backGround_);
					}
					drawRectangle(i+offsetX_, j+offsetY_, 4, 4*addLine_);
					v = v << 1;
				}
			}
		}
	}
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	if (!wxIsMainThread())
		wxMutexGuiLeave();
#endif
#if defined(__WXMAC__) || defined(__linux__)
    reBlit_ = true;
#else
	if (zoomFraction_)
		reBlit_ = true;
	else
	{
		CharacterList6847 *temp = new CharacterList6847;
		temp->x = x;
		temp->y = y;
		temp->nextCharacter = characterListPointer6847;
		characterListPointer6847 = temp;
		updateCharacter_++;
		if (updateCharacter_ > 40)
			reBlit_ = true;

	}
#endif
}

void mc6847::drawGraphic(wxCoord x, wxCoord y, int v)
{
	wxColour clr;

	if (css_ < 12) 
		css_ = (v >> cssBit_) & 1;

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	if (!wxIsMainThread())
		wxMutexGuiEnter();
#endif
	if (gm0_ == 0)
	{
		for (wxCoord i=x; i<x+charWidth_; i+=elementWidth_)
		{
			clr = colour_[GRAPHIC+(((v & 0xc0) >> 6) + (css_ << 2))];
			setColour(clr);
			drawRectangle(i+offsetX_, y+offsetY_, elementWidth_, charHeight_*addLine_);
			v = v << 2;
		}
	}
	else
	{
		for (wxCoord i=x; i<x+charWidth_; i+=elementWidth_)
		{
			if ((v & 0x80) == 0)
				clr = colour_[backGround_];
			else
				clr = colour_[GRAPHIC+(css_ << 2)];
			setColour(clr);
			if ((elementWidth_ == 1) && (addLine_ == 1))
				drawPoint(i+offsetX_, y+offsetY_);
			else
				drawRectangle(i+offsetX_, y+offsetY_, elementWidth_, charHeight_*addLine_);
			v = v << 1;
		}
	}

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	if (!wxIsMainThread())
		wxMutexGuiLeave();
#endif
//#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
//		reBlit_ = true;
//#else
		CharacterList6847 *temp = new CharacterList6847;
		temp->x = x;
		temp->y = y;
		temp->nextCharacter = characterListPointer6847;
		characterListPointer6847 = temp;
		updateCharacter_++;
		if (updateCharacter_ > 40)
			reBlit_ = true;
//#endif
}

bool mc6847::readCharRomFile(wxString romDir, wxString romFile)
{
	wxFFile inFile;
	size_t length, number;
	char buffer[2048];

	if (romFile == "")
	{
		p_Main->errorMessage("No font filename specified");
		return false;
	}

	wxString fileName = romDir + romFile;

	for (size_t i=0x000; i<0x800; i++)
	{
		mc6847CharRom_[i] = 0;
	}
	if (inFile.Open(fileName, "rb"))
	{
		length = inFile.Read(buffer, 2048);
		number = 0;
		for (size_t i=0; i<length; i++)
		{
			mc6847CharRom_[i] = (Byte)buffer[i];
			number++;
		}
		inFile.Close();
/*		int addr=0;
		for (int c=0; c<64; c++)
			for (int l=0; l<12; l++)
			{
				if ((l!=0) && (l!=1) && (l!=10) && (l!=11))
				{
					mc6847CharRom_[addr] = mc6847CharRom_[c*12+l+0x200]>>1;
					addr++;
				}
			}*/
		return true;
	}
	else
	{
		p_Main->errorMessage("Error reading " + fileName);
		return false;
	}
}

void mc6847::setFullScreen(bool fullScreenSet)
{
	fullScreenSet_ = fullScreenSet;
	ShowFullScreen(fullScreenSet);
}

void mc6847::onF3()
{
	fullScreenSet_ = !fullScreenSet_;
	p_Main->eventVideoSetFullScreen(fullScreenSet_);
}


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

#define FOREGROUND 2
#define BACKGROUND 3

#include "main.h"
#include "mc6845.h"

MC6845::MC6845(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock, int charW, ElfPortConfiguration elfPortConf)
: Video(title, pos, size)
{
	charW_ = charW;
	clock_ = clock;

	int regVal [16] = {94, 64, 77, 8, 28, 2, 16, 22, 0, 8, 20, 8, 0, 0, 3, 0xc0}; 

	windowSize_ = size;
	computerType_ = computerType;
	characterListPointer6845 = NULL;

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
	interlaceOR_ = p_Main->getInterlace(computerType_);
	readCharRomFile(p_Main->getCharRomDir(computerType_), p_Main->getCharRomFile(computerType_));
    mc6845RamStart_ = elfPortConf.mc6845StartRam; //p_Main->getConfigItem(elfTypeStr_+"/mc6845StartRam",0xE000l);
	mc6845RamEnd_ = elfPortConf.mc6845EndRam; //p_Main->getConfigItem(elfTypeStr_+"/mc6845EndRam", 0xE7FFl);
	Word mc6845RamSize = mc6845RamEnd_ - mc6845RamStart_ + 1;
	p_Computer->defineMemoryType(mc6845RamStart_, mc6845RamEnd_, MC6845RAM);

    mc6845AddressRegister_ = elfPortConf.mc6845Address; //p_Main->getConfigItem(elfTypeStr_+"/mc6845Address",0xE800l);
	mc6845DataRegister_ = elfPortConf.mc6845Data; //p_Main->getConfigItem(elfTypeStr_+"/mc6845Data",0xE801l);

    switch (p_Main->getCpuStartupVideoRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<mc6845RamSize; i++) mc6845ram_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<mc6845RamSize; i++) mc6845ram_[i] = rand() % 0x100;
        break;
            
        case STARTUP_DYNAMIC:
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<mc6845RamSize; i++) mc6845ram_[i] = p_Computer->getDynamicByte(i);
        break;
    }
    
	fullScreenSet_ = false;
	zoom_ = zoom;

	double intPart;
	zoomFraction_ = (modf(zoom_, &intPart) != 0);

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	screenCopyPointer = new wxBitmap(size.x, size.y);
	dcMemory.SelectObject(*screenCopyPointer);

#if defined(__WXMAC__)
	gc = wxGraphicsContext::Create(dcMemory);
	gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

	videoScreenPointer = new VideoScreen(this, size, zoom, computerType);
	cursorAddress_ = 0;
	startAddress_ = 0;
	cursorOn_ = false;
	cursorBlink_ = true;
	cursorBlinkOn_ = false;
	cursorBlinkTime_ = 16;
	charLine_ = 0;
	rows_ = 0;
	scanLine_ = 0;
	blink_ = cursorBlinkTime_;
	offsetX_ = 0;
	offsetY_ = 0;
	videoHeight_ = 288;
	videoWidth_ = 512;
	scanLine_ = 9;

	defineColours(computerType_);
	backGround_ = BACKGROUND;
	videoType_ = VIDEO6845;

	for (int i=0; i<16; i++)
	{
		register_[i] = 0xff;
		writeRegister6845(mc6845AddressRegister_, i);
		writeData6845(regVal[i]);
	}
	setCycle();
	zoom_ = zoom;
	this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);
	this->SetBackgroundColour(colour_[backGround_]);
}

MC6845::~MC6845()
{
	CharacterList6845 *temp;

	dcMemory.SelectObject(wxNullBitmap);
	delete screenCopyPointer;
	delete videoScreenPointer;
#if defined(__WXMAC__)
	delete gc;
#endif
	if (updateCharacter6845_ > 0)
	{
		while(characterListPointer6845 != NULL)
		{
			temp = characterListPointer6845;
			characterListPointer6845 = temp->nextCharacter;
			delete temp;
		}
	}
}

void MC6845::configure6845(ElfPortConfiguration elfPortConf)
{
	p_Computer->defineMemoryType(mc6845AddressRegister_, MC6845REGISTERS);
	p_Computer->defineMemoryType(mc6845DataRegister_, MC6845REGISTERS);

	p_Computer->setCycleType(BLINKCYCLE, MC6845BLINK);
	p_Computer->setCycleType(VIDEOCYCLE, MC6845CYCLE);

//	int efPort = p_Main->getConfigItem(elfTypeStr_+"/mc6845Ef", 2l);
	p_Computer->setEfType(elfPortConf.mc6845Ef, MC6847EF);

	p_Main->message("Configuring MC6845");

	wxString printBuffer;
	printBuffer.Printf("	EF %d: Display enable", elfPortConf.mc6845Ef);
	p_Main->message(printBuffer);
	printBuffer.Printf("	%04X-%04X: Video RAM", mc6845RamStart_, mc6845RamEnd_);
	p_Main->message(printBuffer);
	printBuffer.Printf("	%04X: CRTC address register, %04X: CRTC data register\n", mc6845AddressRegister_, mc6845DataRegister_);
	p_Main->message(printBuffer);
}

void MC6845::configureSuperVideo()
{
	p_Computer->setCycleType(BLINKCYCLE, MC6845BLINK);
	p_Computer->setCycleType(VIDEOCYCLE, MC6845CYCLE);

	p_Main->message("Configuring MC6845");

	wxString printBuffer;
	printBuffer.Printf("	%04X-%04X: Video RAM\n", mc6845RamStart_, mc6845RamEnd_);
	p_Main->message(printBuffer);
}

void MC6845::init6845()
{
	cycleValue6845_ = cycleSize6845_;
	blinkValue6845_ = blinkSize6845_;
	reDraw_ = true;
	reBlit_ = false;
	newBackGround_ = false;
	updateCharacter6845_ = 0;
}

Byte MC6845::ef6845()
{
	return(cycleValue6845_ < nonDisplay6845_) ? 0 : 1;
}

void MC6845::writeRegister6845(Word addr, Byte value)
{
	if (addr == mc6845DataRegister_)  writeData6845(value);
	if (addr != mc6845AddressRegister_)  return;

	registerIndex_ = value&0x1f;
}

Byte MC6845::readData6845(Word addr)
{
	if (addr != mc6845DataRegister_)  return 255;
	Byte Ret = 0;

	switch(registerIndex_)
	{
		case 14:
			Ret = register_[registerIndex_];
		break;

		case 15:
			Ret = register_[registerIndex_];
		break;
	}
	return Ret;
}

void MC6845::writeData6845(Byte value)
{
	switch(registerIndex_)
	{
		case 0:
			if ((value + 1) != Nht_)
			{
				Nht_= value + 1;
				reCycle_ = true;
			}
		break;

		case 1:
			if (value != charLine_)
			{
				charLine_ = value;
				videoWidth_ = charLine_*charW_;
				reDraw_ = true;
				setScreenSize();
				reCycle_ = true;
			}
		break;

		case 4:
			if ((value + 1) != Nvt_)
			{
				Nvt_= value + 1;
				reCycle_ = true;
			}
		break;

		case 5:
			if (value != Nadj_)
			{
				Nadj_= value;
				reCycle_ = true;
			}
		break;

		case 6:
			if (value != rows_)
			{
				rows_ = value;
				videoHeight_ = rows_*scanLine_*2; 
				reDraw_ = true;
				setScreenSize();
			}
		break;

		case 8:
			interlace_ = ((value&1) == 1);
			if (interlaceOR_ == true)
				interlace_ = 1;

			if ((value&0x3) == 3)
				videoM_ = 1;
			else
				videoM_ = 2;

			reDraw_ = true;
		break;

		case 9:
			if (value != (scanLine_-1))
			{
				value &= 0x1f;
				scanLine_ = value + 1;
				videoHeight_ = rows_*scanLine_*2; 
				reDraw_ = true;
				setScreenSize();
				reCycle_ = true;
			}
		break;

		case 10:
			if ((value&0x7f) != register_[registerIndex_])
			{
				value &= 0x7f;
				cursorStartLine_ = value & 0x1f;
				if (cursorStartLine_ > 15)  cursorStartLine_ = 15;
				switch(value & 0x60)
				{
					case 0x00:
						cursorOn_ = true;
						cursorBlink_ = false;
						cursorBlinkTime_ = 16;
					break;

					case 0x20:
						cursorOn_ = false;
					break;

					case 0x40:
						cursorOn_ = true;
						cursorBlink_ = true;
						cursorBlinkTime_ = 16;
					break;

					case 0x60:
						cursorOn_ = true;
						cursorBlink_ = true;
						cursorBlinkTime_ = 32;
					break;
				}
			}
		break;

		case 11:
			if ((value&0x1f) != register_[registerIndex_])
			{
				value &= 0x1f;
				cursorEndLine_ = value;
				if (cursorEndLine_ > 15)  cursorEndLine_ = 15;
			}
		break;

		case 12:
			if ((value&0x3f) != register_[registerIndex_])
			{
				value &= 0x3f;
				startAddress_ = (startAddress_ & 0xff) + (value << 8);
				reDraw_ = true;
			}
		break;

		case 13:
			if (value != register_[registerIndex_])
			{
				startAddress_ = (startAddress_ & 0x3f00) + value;
				reDraw_ = true;
			}
		break;

		case 14:
			if ((value&0x3f) != register_[registerIndex_])
			{
				value &= 0x3f;
				draw6845(cursorAddress_& 0x7ff, mc6845ram_[cursorAddress_& 0x7ff]);
				cursorAddress_ = (cursorAddress_ & 0xff) +(value << 8);
			}
		break;

		case 15:
			if (value != register_[registerIndex_])
			{
				draw6845(cursorAddress_& 0x7ff, mc6845ram_[cursorAddress_& 0x7ff]);
				cursorAddress_ = (cursorAddress_ & 0x3f00) + value;
			}
		break;
	}
	register_[registerIndex_] = value;
}

void MC6845::cycle6845()
{
	cycleValue6845_--;
	if (cycleValue6845_ <= 0)
		cycleValue6845_ = cycleSize6845_;
}

void MC6845::blink6845()
{
	blinkValue6845_--;
	if (blinkValue6845_ <= 0)
	{
		blinkValue6845_ = blinkSize6845_;
		copyScreen();
		videoSyncCount_++;
		if (changeScreenSize_)
		{
			changeScreenSize();
			changeScreenSize_ = false;
		}
		blink_--;
		if (blink_ <= 0)
		{
			blink_ = cursorBlinkTime_;
			if (cursorOn_)
			{
				if (cursorBlinkOn_ || !cursorBlink_)
					cursorBlinkOn_ = false;
				else
					cursorBlinkOn_ = true;
			}
		}
		if (cursorOn_)
			drawCursor6845(cursorAddress_, cursorBlinkOn_);
	}
}

void MC6845::setClock(double clock)
{
	clock_ = clock;
	reCycle_ = true;
}

void MC6845::setCycle()
{   // DOT = 14.31818
	float clockPeriod = (float)(1/14.318180 * charW_);
	float scanLinePeriod = (float)Nht_ * clockPeriod;
	float characterRowPeriod = (float)(scanLine_ + 1) * scanLinePeriod;
	int fieldTime = (int)((float)(Nvt_ * characterRowPeriod) + (Nadj_ * scanLinePeriod));

	cycleSize6845_ = (int)(scanLinePeriod / ((1/clock_) * 8));
	nonDisplay6845_ = cycleSize6845_ - (int)((charLine_ * clockPeriod) / ((1/clock_) * 8));
	blinkSize6845_ = (int)(fieldTime / ((1/clock_) * 8));

	cycleValue6845_ = cycleSize6845_;
	blinkValue6845_ = blinkSize6845_;
	reCycle_ = false;
}

Byte MC6845::read6845(Word addr)
{
 	return mc6845ram_[addr];
}

void MC6845::write6845(Word addr, Byte value)
{
	Word memoryStart = p_Computer->getDebugMemoryStart();
	if (addr >= memoryStart && addr<(memoryStart + 256))
		p_Main->updateDebugMemory(addr);
	draw6845(addr, value);
}

Byte MC6845::read6845CharRom(Word addr)
{
 	return mc6845CharRom_[addr];
}

void MC6845::write6845CharRom(Word addr, Byte value)
{
	mc6845CharRom_[addr] = value;
	reDraw_ = true;
}

void MC6845::copyScreen()
{
    if (p_Main->isZoomEventOngoing())
        return;

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

#if defined(__WXMAC__)
    if (reBlit_ || reDraw_)
    {
        p_Main->eventRefreshVideo(false, 0);
        reBlit_ = false;
        reDraw_ = false;
    }
#else
    if (extraBackGround_ && newBackGround_)
        drawExtraBackground(colour_[backGround_]);

    CharacterList6845 *temp;

    if (reBlit_ || reDraw_)
	{
		videoScreenPointer->blit(0, 0, videoWidth_+2*offsetX_, rows_*scanLine_*videoM_+2*offsetY_, &dcMemory, 0, 0);
		reBlit_ = false;
		reDraw_ = false;
		if (updateCharacter6845_ > 0)
		{
			updateCharacter6845_ = 0;
			while(characterListPointer6845 != NULL)
			{
				temp = characterListPointer6845;
				characterListPointer6845 = temp->nextCharacter;
				delete temp;
			}
		}
	}
	if (updateCharacter6845_ > 0)
	{
		updateCharacter6845_ = 0;
		while(characterListPointer6845 != NULL)
		{
			videoScreenPointer->blit(offsetX_+(characterListPointer6845->x), offsetY_+(characterListPointer6845->y), charW_, scanLine_*videoM_, &dcMemory, offsetX_+characterListPointer6845->x, offsetY_+characterListPointer6845->y);
			temp = characterListPointer6845;
			characterListPointer6845 = temp->nextCharacter;
			delete temp;
		}
	}
#endif
}

void MC6845::drawScreen()
{
	int addr = startAddress_;
	setColour(backGround_);
	drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);
	for (int i=0; i<(charLine_*rows_); i++)
	{
		draw6845(addr, mc6845ram_[addr]);

		addr++;
		addr &= 0x7ff;
	}
}

void MC6845::draw6845(Word addr, Byte value)
{
	mc6845ram_[addr] = value;

	addr = (addr - startAddress_) & 0x7ff;

	int y = (addr/charLine_)*scanLine_*videoM_;
 	int x = (addr%charLine_)*charW_;
 	drawCharacter6845(x, y, value);
}

void MC6845::drawCharacter6845(wxCoord x, wxCoord y, Byte v)
{
	int line_byte, line;

    setColour(backGround_);
	drawRectangle(x+offsetX_, y+offsetY_, charW_, scanLine_*videoM_);

	setColour(FOREGROUND);

	if (v >= 0x80)
	{
		if (interlace_ & !(videoM_ == 1))
			drawRectangle(x+offsetX_, y+offsetY_+(scanLine_-1)*videoM_, charW_, 2);
		else
			drawRectangle(x+offsetX_, y+offsetY_+(scanLine_-1)*videoM_, charW_, 1);
	}

#if defined(__WXMAC__) || defined(__linux__)
    reBlit_ = true;
#else
	if (zoomFraction_)
		reBlit_ = true;
	else
	{
		CharacterList6845 *temp = new CharacterList6845;
		temp->x = x;
		temp->y = y;
		temp->nextCharacter = characterListPointer6845;
		characterListPointer6845 = temp;
		updateCharacter6845_++;
		if (updateCharacter6845_++ > 40)
			reBlit_ = true;
	}
#endif

	line = 0;
	for (wxCoord j=y; j<y+(scanLine_-1)*videoM_; j+=videoM_)
	{
		line_byte =	mc6845CharRom_[v*8+line];
		for (wxCoord i=x; i<x+charW_; i++)
		{
			if (line_byte & 128)
			{
				if (interlace_ & !(videoM_ == 1))
					drawRectangle(i+offsetX_, j+offsetY_, 1, videoM_);
				else
					drawPoint(i+offsetX_, j+offsetY_);
			}
			line_byte <<= 1;
		}
		line++;
	}
}

void MC6845::drawCursor6845(Word addr, bool status)
{
	Byte v;
	int line_byte, line;
	wxColour clr;

	addr = (addr - startAddress_) & 0x7ff;

	int y = (addr/charLine_)*scanLine_*videoM_;
 	int x = (addr%charLine_)*charW_;

	v = mc6845ram_[addr];
	line = cursorStartLine_;

    for (int yLine = y + cursorStartLine_*videoM_; yLine <= (y + cursorEndLine_*videoM_); yLine+=videoM_)
	{
		if (yLine == (y + (scanLine_-1)*videoM_))
		{
			if (v<0x80)
			{
				if (status)
					clr = colour_[FOREGROUND];
				else
					clr = colour_[backGround_];
			}
			else
			{
				if (status)
					clr = colour_[backGround_];
				else
					clr = colour_[FOREGROUND];
			}
			if (interlace_ & !(videoM_ == 1))
				videoScreenPointer->drawRectangle(clr, (x+offsetX_)*zoom_, (yLine+offsetY_)*zoom_, charW_*zoom_, 2*zoom_);
			else
				videoScreenPointer->drawRectangle(clr, (x+offsetX_)*zoom_, (yLine+offsetY_)*zoom_, charW_*zoom_, zoom_);
		}
		else
		{
			line_byte =	mc6845CharRom_[v*8+line];
			for (wxCoord i=x; i<x+charW_; i++)
			{
				if (line_byte & 128)
				{
					if (status)
						clr = colour_[backGround_];
					else
						clr = colour_[FOREGROUND];
				}
				else
				{
					if (status)
						clr = colour_[FOREGROUND];
					else
						clr = colour_[backGround_];
				}
				if (interlace_ & !(videoM_ == 1))
					videoScreenPointer->drawRectangle(clr, (i+offsetX_)*zoom_, (yLine+offsetY_)*zoom_, zoom_, 2*zoom_);
				else
					videoScreenPointer->drawRectangle(clr, (i+offsetX_)*zoom_, (yLine+offsetY_)*zoom_, zoom_, zoom_);
				line_byte <<= 1;
			}
		}
		line++;
	}
}

void MC6845::setInterlace(bool status)
{
	interlace_ = status;
	if (status)
		register_[8] |= 1;
	else
		register_[8] &= 0xfe;
	reDraw_ = true;
}

bool MC6845::readCharRomFile(wxString romDir, wxString romFile)
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

	if (inFile.Open(fileName, "rb"))
	{
		length = inFile.Read(buffer, 2048);
		number = 0;
		for (size_t i=0; i<length; i++)
		{
			mc6845CharRom_[i] = (Byte)buffer[i];
			number++;
		}
		inFile.Close();
		return true;
	}
	else
	{
		p_Main->errorMessage("Error reading " + fileName);
		return false;
	}
}

void MC6845::setFullScreen(bool fullScreenSet)
{
	fullScreenSet_ = fullScreenSet;
	ShowFullScreen(fullScreenSet);
}

void MC6845::onF3()
{
	fullScreenSet_ = !fullScreenSet_;
	p_Main->eventVideoSetFullScreen(fullScreenSet_);
}

void MC6845::reBlit(wxDC &dc)
{
    if (!memoryDCvalid_)
        return;
    
    dc.Blit(0, 0, videoWidth_+2*offsetX_, rows_*scanLine_*videoM_+2*offsetY_, &dcMemory, 0, 0);
    
    if (extraBackGround_ && newBackGround_)
    {
        wxSize size = wxGetDisplaySize();

        dc.SetBrush(wxBrush(colour_[backGround_]));
        dc.SetPen(wxPen(colour_[backGround_]));

        int xStart = (int)((2*offsetX_+videoWidth_)*zoom_*xZoomFactor_);
        dc.DrawRectangle(xStart, 0, size.x-xStart, size.y);

        int yStart = (int)((2*offsetY_+videoHeight_)*zoom_);
        dc.DrawRectangle(0, yStart, size.x, size.y-yStart);

        newBackGround_ = false;
    }
}


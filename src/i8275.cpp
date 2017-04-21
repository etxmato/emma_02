/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
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

#define I8275CHARW 8

#define C_NONE 0
#define C_RESET 1
#define C_START_DISPLAY 2
#define C_STOP_DISPLAY 3
#define C_LIGHT_PEN 4
#define C_LOAD_CURSOR 5
#define C_ENABLE_INT 6
#define C_DISABLE_INT 7
#define C_PRESET_COUNTERS 8

#define FOREGROUND8275 2
#define BACKGROUND8275 3
#define HIGHLIGHT8275 4

#include "main.h"
#include "i8275.h"

int const SSS [8] = {0, 7, 15, 23, 31, 39, 47, 55};
int const BB [4] = {1, 2, 4, 8};

BEGIN_EVENT_TABLE(i8275, wxFrame)
	EVT_CLOSE (i8275::onClose)
	EVT_SIZE(i8275::onSize)
END_EVENT_TABLE()

i8275::i8275(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock)
: Video(title, pos, size)
{
	computerType_ = computerType;
	clock_ = clock;

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	videoType_ = VIDEOI8275;
	readCharRomFile(p_Main->getCharRomDir(computerType_), p_Main->getCharRomFile(computerType_));
	interlace_ = p_Main->getInterlace(computerType_);

	if (computerType_ == ELF2K)
		videoType_ = VIDEO2KI8275;

	offsetX_ = 0;
	offsetY_ = 0;

	defineColours(computerType_);
	backGround_ = BACKGROUND8275;

	fullScreenSet_ = false;
	zoom_ = zoom;

	double intPart;
	zoomFraction_ = (modf(zoom_, &intPart) != 0);

	horizontalCharactersPerRow_ = 80;
	verticalRowsPerFrame_ = 24;
	linesPerCharacterRow_ = 10;
	verticalRetraceRowCount_ = 2;
	horizontalRetraceCount_ = 16;
	burstCountCode_ = 8;
	burstSpaceCode_ = 0;
	lineCounterMode_ = 1;

	videoWidth_ = horizontalCharactersPerRow_*I8275CHARW;
	videoHeight_ = verticalRowsPerFrame_*linesPerCharacterRow_*2;

	screenCopyPointer = new wxBitmap(videoWidth_*zoom_, videoHeight_*zoom_);
	dcMemory.SelectObject(*screenCopyPointer);

#if defined(__WXMAC__)
	gc = wxGraphicsContext::Create(dcMemory);
	gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

	setColour(backGround_);
	drawRectangle(0, 0, videoWidth_, videoHeight_);

	videoScreenPointer = new VideoScreen(this, size, zoom, computerType);

	setCycle();

	cycleValue8275_ = -1;
	cycleBlankValue8275_ = fullCycleSize8275_;
	dmaCycleValue8275_ = -1;
	retrace_ = false;
	screenLocation_ = 0;
	bufferLocation_ = 0;
	reverse_ = false;
	underline_ = false;
	highlight_ = false;
	gpa_ = 0;
	blink_ = false;
	blinkOn_ = false;
	cursorBlinkOn_ = true;
	cursorLine_ = false;
	cursorBlinking_ = false;
	attributeChange_ = false;
	blinkCount_ = 16;
	videoM_ = 2;
	status_ = 0;
	ef_ = 1;
	cursorCharPosition_ = 0;
	cursorRowPosition_ = 0;
	cursorAddress_ = 0;
    switch (p_Main->getCpuStartupVideoRam())
    {
        case STARTUP_ZEROED:
            for (int i=0; i<5120; i++)
                i8275ram_[i] = 0;
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<5120; i++)
                i8275ram_[i] = rand() % 0x100;
        break;
            
        case STARTUP_DYNAMIC:
            p_Computer->setDynamicRandomByte();
            for (int i=0; i<5120; i++)
                i8275ram_[i] = p_Computer->getDynamicByte(i);
        break;
    }
	for (int i=0; i<5120; i++)
	{
		cursorReset_[i] = false;
		reverseScr_[i] = false;
		underlineScr_[i] = false;
		highlightScr_[i] = false;
		blinkScr_[i] = false;
		gpaScr_[i] = 0;
	}

	this->SetBackgroundColour(colour_[backGround_]);
	this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);
	characterListPointer8275 = NULL;
}

i8275::~i8275()
{
	CharacterList8275 *temp;

	dcMemory.SelectObject(wxNullBitmap);
	delete screenCopyPointer;
	delete videoScreenPointer;
#if defined(__WXMAC__)
	delete gc;
#endif
	if (updateCharacter8275_ > 0)
	{
		while(characterListPointer8275 != NULL)
		{
			temp = characterListPointer8275;
			characterListPointer8275 = temp->nextCharacter;
			delete temp;
		}
	}
}

void i8275::configure8275(ElfPortConfiguration elfPortConf)
{
	wxString runningComp = p_Main->getRunningComputerStr();

//	int i8275WriteCommand = p_Main->getConfigItem(runningComp+"/I8275WriteCommand", 5l);
//	int i8275ReadStatus = p_Main->getConfigItem(runningComp+"/I8275ReadStatus", 5l);
//	int i8275WriteParameter = p_Main->getConfigItem(runningComp+"/I8275WriteParameter", 1l);
//	int i8275ReadParameter = p_Main->getConfigItem(runningComp+"/I8275ReadParameter", 1l);
//	int i8275VerticalRetrace = p_Main->getConfigItem(runningComp+"/I8275VerticalRetrace", 1l);

	p_Computer->setCycleType(VIDEOCYCLE, I8275CYCLE);
	p_Computer->setOutType(elfPortConf.i8275WriteParameter, I8275PREGWRITE);
	p_Computer->setOutType(elfPortConf.i8275WriteCommand, I8275CREGWRITE);
	p_Computer->setInType(elfPortConf.i8275ReadParameter, I8275PREGREAD);
	p_Computer->setInType(elfPortConf.i8275ReadStatus, I8275SREGREAD);
	p_Computer->setEfType(elfPortConf.i8275VerticalRetrace, I8275EF);

	p_Main->message("Configuring intel 8275");

	wxString printBuffer;
	printBuffer.Printf("	Output %d write command, input %d read status", elfPortConf.i8275WriteCommand, elfPortConf.i8275ReadStatus);
	p_Main->message(printBuffer);
	printBuffer.Printf("	Output %d write parameter, input %d read parameter", elfPortConf.i8275WriteParameter, elfPortConf.i8275ReadParameter);
	p_Main->message(printBuffer);
	printBuffer.Printf("	EF %d: vertical retrace\n", elfPortConf.i8275VerticalRetrace);
	p_Main->message(printBuffer);
}

void i8275::init8275()
{
	reDraw_ = true;
	reBlit_ = false;
	reBlink_ = false;
	newBackGround_ = false;
	updateCharacter8275_ = 0;

}

Byte i8275::ef8275()
{
	return ef_;
}

void i8275::pRegWrite(Byte value)
{
	if (command_ == 0)
	{
		status_ |= 0x8;
		return;
	}

	switch (command_)
	{
		case C_RESET:
			switch (parameters_)
			{
				case 4:
					spacedRows_ = ((value & 0x80) == 1);
					value = (value & 0x7f) + 1;
					if (value >= 80)  value = 80;
					horizontalCharactersPerRow_ = value;
					videoWidth_ = horizontalCharactersPerRow_ * I8275CHARW;
					setScreenSize();
//					p_Main->message("Column:");
//					p_Main->messageInt(horizontalCharactersPerRow_);
				break;

				case 3:
					verticalRetraceRowCount_ = ((value & 0xc0) >> 6) + 1;
					verticalRowsPerFrame_ = (value & 0x3f) + 1;
					videoHeight_ = verticalRowsPerFrame_*linesPerCharacterRow_*2;
					setScreenSize();
//					p_Main->message("Rows:");
//					p_Main->messageInt(verticalRowsPerFrame_);
//					p_Main->message("Vertical Retrace Row Count:");
//					p_Main->messageInt(verticalRetraceRowCount_);
				break;

				case 2:
					underLinePlacement_ = ((value & 0xf0) >> 4) + 1;
					linesPerCharacterRow_ = (value & 0x0f) + 1;
					videoHeight_ = verticalRowsPerFrame_*linesPerCharacterRow_*2;
					setScreenSize();
//					p_Main->message("Under Line Placement");
//					p_Main->messageInt(underLinePlacement_);
//					p_Main->message("ScanLine:");
//					p_Main->messageInt(linesPerCharacterRow_);
				break;

				case 1:
					lineCounterMode_ = (value & 0x80) >> 7;
					fieldAttributeMode_ = (value & 0x40) >> 6;
					cursorBlinking_ = (((value & 0x20) >> 4) == 0);
					cursorBlinkOn_ = true;
					cursorLine_ = (((value & 0x10) >> 4) == 1);
					horizontalRetraceCount_ = ((value & 0x0f) + 1)*2;
					setScreenSize();
//					p_Main->message("Line Counter Mode:");
//					p_Main->messageInt(lineCounterMode_);
//					p_Main->message("Field Attribute Mode:");
//					p_Main->messageInt(fieldAttributeMode_);
//					p_Main->message("Horizontal Retrace Count:");
//					p_Main->messageInt(horizontalRetraceCount_);
				break;
			}
			reCycle_ = true;
		break;

		case C_LOAD_CURSOR:
			switch (parameters_)
			{
				case 2:
					cursorCharPosition_ = value;
					cursorAddress_ = cursorCharPosition_ + (cursorRowPosition_*80);
				break;
				case 1:
					cursorRowPosition_ = value;
					cursorAddress_ = cursorCharPosition_ + (cursorRowPosition_*80);
				break;
			}
		break;
	}

	parameters_--;
	if (parameters_ < 0)
	{
		status_ |= 0x8;
		parameters_ = 0;
	}
}

void i8275::cRegWrite(Byte value)
{
	if (parameters_ != 0)
		status_ |= 0x8;

/*	if (value != 0x80)
	{
	p_Main->message("8275 Register_");
	p_Main->messageInt(value);
	}*/
	parameters_ = 0;
	command_ = C_NONE;

	if (value == 0) // RESET
	{
		command_ = C_RESET;
		parameters_ = 4;
		status_ &= 0xbf;
	}

	if (value == 0x40) // STOP_DISPLAY
	{
		cycleValue8275_ = -1;
		dmaCycleValue8275_ = -1;
		status_ &= 0xfb;

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	if (!wxIsMainThread())
		wxMutexGuiEnter();
#endif
		setColour(backGround_);
		drawRectangle(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	if (!wxIsMainThread())
		wxMutexGuiLeave();
#endif
//		reBlit_ = true;
//		copyScreen();
	}

	if (value == 0x60) // LIGHT_PEN
	{
		command_ = C_LIGHT_PEN;
		parameters_ = 2;
	}

	if (value == 0x80) // LOAD_CURSOR
	{
		command_ = C_LOAD_CURSOR;
		parameters_ = 2;
	}

	if (value == 0xe0) // PRESET_COUNTERS
	{
		bufferLocation_ = 0;
		screenLocation_ = 0;
	}

	if ((value & 0xf0) == 0x20)  // START DISPLAY
	{
		burstSpaceCode_ = SSS[(value & 0x1c) >> 2];
		burstCountCode_ = BB[(value & 0x3)];
//		p_Main->message("Burst Space Code:");
//		p_Main->messageInt(burstSpaceCode_);
//		p_Main->message("Burst Count Code:");
//		p_Main->messageInt(burstCountCode_);
		bufferLocation_ = 0;
		screenLocation_ = 0;
		reCycle_ = true;
		cycleValue8275_ = rowCycleSize8275_;
//		p_Main->startTime();
		status_ |= 0x44;
	}

}

Byte i8275::pRegRead()
{
	return 0;
}

Byte i8275::sRegRead()
{
	Byte ret = status_;
	status_ &= 0xc4;

    ef_ = 1;
	return ret;
}

void i8275::cycle8275()
{
	if (cycleValue8275_ < 0) 
	{
		cycleBlankValue8275_--;
		if (cycleBlankValue8275_ <= 0)
		{
			cycleBlankValue8275_ = fullCycleSize8275_;
			if (changeScreenSize_)
			{
				changeScreenSize();
				changeScreenSize_ = false;
			}
			reBlit_ = true;
			reDraw_ = false;
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	if (!wxIsMainThread())
		wxMutexGuiEnter();
#endif
			setColour(backGround_);
			drawRectangle(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	if (!wxIsMainThread())
		wxMutexGuiLeave();
#endif
			copyScreen();
			videoSyncCount_++;
			reDraw_ = true;
		}
		return;
	}
	cycleValue8275_--;
	if (cycleValue8275_ <= 0)
	{
		if (retrace_)
		{
			dmaCycleValue8275_ = dmaCycleSize8275_;
			bufferLocation_ = 0;
			retrace_ = false;
			cycleValue8275_ = rowCycleSize8275_;
		}
		else
		{
			if (dmaCycleValue8275_ > 0)
			{
				dmaCycleValue8275_ = -1;
				screenLocation_ = screenLocation_ + horizontalCharactersPerRow_ - bufferLocation_;
				status_ |= 0x2;
			}
			retrace_ = true;
			if ((status_ & 0x40) == 0x40)
			{
				status_ |= 0x20;
				p_Computer->pixieInterrupt();
				p_Computer->setCycle0();
			}

			cycleValue8275_ = horizontalRetraceCycleSize8275_;
			if (screenLocation_ == (verticalRowsPerFrame_*horizontalCharactersPerRow_))
			{
				attributeChange_ = false;
				reverse_ = false;
				underline_ = false;
				highlight_ = false;
				gpa_ = 0;
				blink_ = false;
				cycleValue8275_ = verticalRetraceCycleSize8275_;
				ef_ = 0;
				screenLocation_ = 0;
				blinkCount_--;
				if (blinkCount_ <= 0)
				{
					blinkCount_ = 16;
					blinkOn_ = !blinkOn_;
					reBlink_ = true;
					if (cursorBlinking_)
						cursorBlinkOn_ = !cursorBlinkOn_;
				}
				if (blinkCount_ == 8)
					if (cursorBlinking_)
					{
						cursorBlinkOn_ = !cursorBlinkOn_;
					}
				if (changeScreenSize_)
				{
					changeScreenSize();
					changeScreenSize_ = false;
				}
				copyScreen();
				videoSyncCount_++;
			}
		}
	}

	if (dmaCycleValue8275_ < 0)  return;
	dmaCycleValue8275_--;
	if (dmaCycleValue8275_ <= 0)
	{
		int i=1;
		while (((i <= burstCountCode_) || (dmaCycleSize8275_ == 0)) && (bufferLocation_ != horizontalCharactersPerRow_))
		{
			Byte value = p_Computer->pixieDmaOut();
			cycleValue8275_--;
			if ((value & 0xc0) == 0x80)
			{
				underline_ = ((value &0x20) == 0x20);
				reverse_ = ((value &0x10) == 0x10);
				blink_ = ((value &0x2) == 0x2);
				gpa_ = (value &0xc) << 9;
				highlight_ = ((value &0x1) == 0x1);
			}
			if ((i8275ram_[screenLocation_] != value) || (highlightScr_[screenLocation_] != highlight_) || (gpaScr_[screenLocation_] != gpa_) || (blinkScr_[screenLocation_] != blink_) || (reverseScr_[screenLocation_] != reverse_) || (underlineScr_[screenLocation_] != underline_) || (screenLocation_ == cursorAddress_) || cursorReset_[screenLocation_])
			{
			/*	if ((value & 0xc0) == 0x80)
				{
					attributeChange_ = true;
			//		reDraw_ = true;
				}*/
				underlineScr_[screenLocation_] = underline_;
				reverseScr_[screenLocation_] = reverse_;
				blinkScr_[screenLocation_] = blink_;
				gpaScr_[screenLocation_] = gpa_;
				highlightScr_[screenLocation_] = highlight_;
				draw8275(screenLocation_, value);
			}

			i++;
			bufferLocation_++;
			screenLocation_++;
		}
		if (bufferLocation_ == horizontalCharactersPerRow_)
			dmaCycleValue8275_ = -1;
		else
			dmaCycleValue8275_ = dmaCycleSize8275_;
	}
}

void i8275::setClock(double clock)
{
	clock_ = clock;
	reCycle_ = true;
}

void i8275::setCycle()
{   // DOT = 12
	float clockPeriod = (float)1/12 * 8;

	float characterCycleSize8275_ = (float)(clockPeriod * linesPerCharacterRow_ / ((1/clock_) * 8));
	horizontalRetraceCycleSize8275_ = (int) (characterCycleSize8275_ * horizontalRetraceCount_);
	float fullRowCycleSize8275_ = (float) (characterCycleSize8275_ * (horizontalCharactersPerRow_ + horizontalRetraceCount_));
	rowCycleSize8275_ = (int) (characterCycleSize8275_ * horizontalCharactersPerRow_);
//	verticalRetraceCycleSize8275_ = (int) (fullRowCycleSize8275_ * verticalRetraceRowCount_);
	fullCycleSize8275_ = (int) (fullRowCycleSize8275_ * (verticalRowsPerFrame_ + verticalRetraceRowCount_));
	verticalRetraceCycleSize8275_ = (int) (fullCycleSize8275_ - (verticalRowsPerFrame_ * (horizontalRetraceCycleSize8275_ + rowCycleSize8275_)));
	dmaCycleSize8275_ = (int) (characterCycleSize8275_ * burstSpaceCode_);

	reCycle_ = false;
}

Byte i8275::read8275CharRom(Word addr)
{
    return i8275CharRom_[addr];
}

void i8275::write8275CharRom(Word addr, Byte value)
{
    i8275CharRom_[addr] = value;
	Word memoryStart = p_Computer->getDebugMemoryStart();
	if (addr >= memoryStart && addr<(memoryStart + 256))
		p_Main->updateDebugMemory(addr);
	reDraw_ = true;
}

Byte i8275::read8275VideoRam(Word addr)
{
    return i8275ram_[addr];
}

void i8275::write8275VideoRam(Word addr, Byte value)
{
    i8275ram_[addr] = value;
	Word memoryStart = p_Computer->getDebugMemoryStart();
	if (addr >= memoryStart && addr<(memoryStart + 256))
		p_Main->updateDebugMemory(addr);
	reDraw_ = true;
}

void i8275::copyScreen()
{
	CharacterList8275 *temp;

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

    bool reBlitAfterReDraw = false;
	if (reDraw_)
    {
		drawScreen();
    	reDraw_ = false;
        reBlitAfterReDraw = true;
    }

	if (reBlink_)
		blinkScreen8275();

	if (extraBackGround_ && newBackGround_) 
		drawExtraBackground(colour_[backGround_]);

    if (reBlit_ || reBlitAfterReDraw || reBlink_)
	{
		videoScreenPointer->blit(0, 0, videoWidth_+2*offsetX_, verticalRowsPerFrame_*linesPerCharacterRow_*videoM_+2*offsetY_, &dcMemory, 0, 0);

        reBlit_ = false;
		reBlink_ = false;
		if (updateCharacter8275_ > 0)
		{
			updateCharacter8275_ = 0;
			while(characterListPointer8275 != NULL)
			{
				temp = characterListPointer8275;
				characterListPointer8275 = temp->nextCharacter;
				delete temp;
			}
		}
	}
	if (updateCharacter8275_ > 0)
	{
		updateCharacter8275_ = 0;
		while(characterListPointer8275 != NULL)
		{
			videoScreenPointer->blit(offsetX_+(characterListPointer8275->x), offsetY_+(characterListPointer8275->y), I8275CHARW, linesPerCharacterRow_*videoM_, &dcMemory, offsetX_+characterListPointer8275->x, offsetY_+characterListPointer8275->y);
			temp = characterListPointer8275;
			characterListPointer8275 = temp->nextCharacter;
			delete temp;
		}
	}
}

void i8275::drawScreen()
{
	int addr = 0;
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	if (!wxIsMainThread())
		wxMutexGuiEnter();
#endif
	setColour(backGround_);
	drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	if (!wxIsMainThread())
		wxMutexGuiLeave();
#endif
	for (int i=0; i<(horizontalCharactersPerRow_*verticalRowsPerFrame_); i++)
	{
		draw8275(addr, i8275ram_[addr]);

		addr++;
		addr &= 0x7ff;
	}
}

void i8275::blinkScreen8275()
{
	int addr = 0;
	for (int i=0; i<(horizontalCharactersPerRow_*verticalRowsPerFrame_); i++)
	{
		if (blinkScr_[addr])
			draw8275(addr, i8275ram_[addr]);
		addr++;
		addr &= 0x7ff;
	}
}

void i8275::draw8275(Word addr, Byte value)
{
	i8275ram_[addr] = value;
	Word memoryStart = p_Computer->getDebugMemoryStart();
	if (addr >= memoryStart && addr<(memoryStart + 256))
		p_Main->updateDebugMemory(addr);
	cursorReset_[addr] = (addr == cursorAddress_);

	int y = (addr/horizontalCharactersPerRow_)*linesPerCharacterRow_*videoM_;
 	int x = (addr%horizontalCharactersPerRow_)*I8275CHARW;
 	drawCharacter8275(x, y, value, cursorReset_[addr], addr);
}

void i8275::drawCharacter8275(wxCoord x, wxCoord y, Byte v, bool cursor, Word addr)
{
	int line_byte, line, drawLine;

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	if (!wxIsMainThread())
		wxMutexGuiEnter();
#endif
#if defined(__WXMAC__) || defined(__linux__)
	reBlit_ = true;
#else
	if (zoomFraction_)
		reBlit_ = true;
	else
	{
		CharacterList8275 *temp = new CharacterList8275;
		temp->x = x;
		temp->y = y;
		temp->nextCharacter = characterListPointer8275;
		characterListPointer8275 = temp;
		updateCharacter8275_++;
		if (updateCharacter8275_ > 40)
			reBlit_ = true;
	}
#endif

	if ((v & 0xc0) == 0x80)
	{
	/*	underline_ = ((v &0x20) == 0x20);
		reverse_ = ((v &0x10) == 0x10);
		blink_ = ((v &0x2) == 0x2);
		highlight_ = ((v &0x1) == 0x1);
		gpa_ = (v &0xc) << 9;*/

		setColour(backGround_);
		if ((cursor && cursorBlinkOn_) && !cursorLine_)
		{
			setColour(FOREGROUND8275);
		}
		drawRectangle(x+offsetX_, y+offsetY_, I8275CHARW, linesPerCharacterRow_*videoM_);
		if ((cursor && cursorBlinkOn_) && cursorLine_)
		{
			setColour(FOREGROUND8275);
			drawRectangle(x+offsetX_, y+offsetY_+(underLinePlacement_-2), I8275CHARW, videoM_);
		}
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
		if (!wxIsMainThread())
			wxMutexGuiLeave();
#endif
		return;
	}

	if ((reverseScr_[addr] && !((cursor && cursorBlinkOn_) && !cursorLine_)) || ((cursor && cursorBlinkOn_) && !cursorLine_ && !reverseScr_[addr]))
	{
		if (highlightScr_[addr])
		{
			setColour(HIGHLIGHT8275);
		}
		else
		{
			setColour(FOREGROUND8275);
		}
		drawRectangle(x+offsetX_, y+offsetY_, I8275CHARW, linesPerCharacterRow_*videoM_);

		setColour(backGround_);
	}
	else
	{
		setColour(backGround_);
		drawRectangle(x+offsetX_, y+offsetY_, I8275CHARW, linesPerCharacterRow_*videoM_);

		if (highlightScr_[addr])
		{
			setColour(HIGHLIGHT8275);
		}
		else
		{
			setColour(FOREGROUND8275);
		}
	}

	if (blinkScr_[addr] && blinkOn_)
	{
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
		if (!wxIsMainThread())
			wxMutexGuiLeave();
#endif
		return;
	}

	drawLine = 0;
	if (lineCounterMode_ == 0)
		line = 0;
	else
		line = linesPerCharacterRow_-1;

	for (wxCoord j=y; j<y+(linesPerCharacterRow_*videoM_); j+=videoM_)
	{
		if ((underLinePlacement_ > 7) && ((drawLine == 0) || (drawLine == (linesPerCharacterRow_-1))))
			line_byte = 0;
		else
		{
			if (line >= 8)
				line_byte =	i8275CharRom_[v*8+line-8+0x400+gpaScr_[addr]];
			else
				line_byte =	i8275CharRom_[v*8+line+gpaScr_[addr]];
		}

		for (wxCoord i=x; i<x+I8275CHARW; i++)
		{
			if ((line_byte & 1) || ((drawLine == underLinePlacement_) && ((underlineScr_[addr] && !((cursor && cursorBlinkOn_) && cursorLine_)) || ((cursor && cursorBlinkOn_) && cursorLine_ && !underlineScr_[addr]))))
			{
				if (interlace_ & !(videoM_ == 1))
					drawRectangle(i+offsetX_, j+offsetY_, 1, videoM_);
				else
					drawPoint(i+offsetX_, j+offsetY_);
			}
			line_byte >>= 1;
		}
		drawLine++;
		line++;
		if (line >= linesPerCharacterRow_)
			line = 0;
	}

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	if (!wxIsMainThread())
		wxMutexGuiLeave();
#endif
}

void i8275::setInterlace(bool status)
{
	interlace_ = status;
	reDraw_ = true;
}

bool i8275::readCharRomFile(wxString romDir, wxString romFile)
{
	wxFFile inFile;
	size_t length, number;
	char buffer[8192];

	for (size_t i=0; i<8192; i++)
	{
		i8275CharRom_[i] = 0;
	}

	if (romFile == "")
	{
		p_Main->errorMessage("No font filename specified");
		return false;
	}

	wxString fileName = romDir + romFile;

	if (!wxFile::Exists(fileName))
	{
		p_Main->errorMessage("File " + fileName + " not found");
		return false;
	}

	if (inFile.Open(fileName, _("rb")))
	{
		length = inFile.Read(buffer, 8192);
		number = 0;
		for (size_t i=0; i<length; i++)
		{
			i8275CharRom_[i] = (Byte)buffer[i];
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

void i8275::setFullScreen(bool fullScreenSet)
{
	fullScreenSet_ = fullScreenSet;
	ShowFullScreen(fullScreenSet);
}

void i8275::onF3()
{
	fullScreenSet_ = !fullScreenSet_;
	p_Main->eventVideoSetFullScreen(fullScreenSet_);
}


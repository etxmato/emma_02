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

#include "wx/dcbuffer.h"
#include "wx/rawbmp.h"
#include "main.h"
#include "tms9918.h"

#define TMS_GRAPHICS_I 0
#define TMS_GRAPHICS_II 1
#define TMS_MULTICOLOR 2
#define TMS_TEXT 4

#define SPRITE_8_8 0
#define SPRITE_8_8_MAG 1
#define SPRITE_16_16 2
#define SPRITE_16_16_MAG 3

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

    mainAndSpritePlanePointer = new wxBitmap(320, 240);
    dcMemoryMainAndSpritePlane.SelectObject(*mainAndSpritePlanePointer);

    mainPlanePointer = new wxBitmap(320, 240);
    dcMemoryMainPlane.SelectObject(*mainPlanePointer);

    spritePlanePointer = new wxBitmap(320, 240, 32);
    dcMemorySpritePlane.SelectObject(*spritePlanePointer);

#if defined(__WXMAC__)
	gc = wxGraphicsContext::Create(dcMemory);
	gc->SetAntialiasMode(wxANTIALIAS_NONE);
    
    gcMainAndSpritePlane = wxGraphicsContext::Create(dcMemoryMainAndSpritePlane);
    gcMainAndSpritePlane->SetAntialiasMode(wxANTIALIAS_NONE);

    gcMainPlane = wxGraphicsContext::Create(dcMemoryMainPlane);
    gcMainPlane->SetAntialiasMode(wxANTIALIAS_NONE);

    gcSpritePlane = wxGraphicsContext::Create(dcMemorySpritePlane);
    gcSpritePlane->SetAntialiasMode(wxANTIALIAS_NONE);
#endif
    
    mode_ = TMS_GRAPHICS_I;
    nameAddress_ = 0;
    colorAddress_ = 0;
    patternAddress_ = 0;
    currentAddress_ = 0;
    spriteAttributeTableAddress_ = 0;
    spritePatternTableAddress_ = 0;
    statusRegister_ = 0;

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
            for (int i=0; i<8; i++) writeRegister(i, 0);
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<8; i++) writeRegister(i, rand() % 0x100);
        break;
    }
        
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
    reDrawSprites_ = true;
    disableScreen_ = true;

	videoWidth_ = 256;
	videoHeight_ = 192;

	this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);
    
#if defined(__linux__) || defined (__WXMSW__)
    setColourMutex(backgroundColor_+16);
    drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);
#endif
    
    setColourMutexMainPlane(backgroundColor_+16);
    drawRectangleMainPlane(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);
}

Tms9918::~Tms9918()
{
	TileList *temp;

    dcMemory.SelectObject(wxNullBitmap);
    dcMemoryMainPlane.SelectObject(wxNullBitmap);

    delete screenCopyPointer;
    delete mainPlanePointer;
    delete spritePlanePointer;
	delete videoScreenPointer;
#if defined(__WXMAC__)
    delete gc;
    delete gcMainPlane;
    delete gcSpritePlane;
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
    changeScreenSize();
//	int highOutput, lowOutput;

	wxString runningComp = p_Main->getRunningComputerStr();

//	highOutput = p_Main->getConfigItem(runningComp +"/TmsModeHighOutput", 5l);
//	lowOutput = p_Main->getConfigItem(runningComp +"/TmsModeLowOutput", 6l);

	p_Computer->setOutType(elfPortConf.tmsModeHighOutput, TMSREGISTERPORT);
	p_Computer->setOutType(elfPortConf.tmsModeLowOutput, TMSDATAPORT);
    p_Computer->setInType(elfPortConf.tmsModeLowOutput, TMSDATAPORT);
    p_Computer->setInType(elfPortConf.tmsModeHighOutput, TMSREGISTERPORT);
    if (elfPortConf.tmsInterrupt > 0)
        p_Computer->setEfType(elfPortConf.tmsInterrupt, TMSINTERRUPT);

	p_Computer->setCycleType(VIDEOCYCLE, TMSCYCLE);

	wxString printBuffer;
	p_Main->message("Configuring TMS 9918");

	printBuffer.Printf("	Output %d: register port, input/output %d: data port\n", elfPortConf.tmsModeHighOutput, elfPortConf.tmsModeLowOutput);
	p_Main->message(printBuffer);
}

Byte Tms9918::readEf()
{
    Byte returnValue = ((statusRegister_&0x80) >> 7) ^ 0x1;
    
    return returnValue;
}

Byte Tms9918::readDataPort()
{
    Byte returnValue = statusRegister_;
    
    statusRegister_ = statusRegister_ & 0x1f;
    return returnValue;
}

Byte Tms9918::readVRAM()
{
    return tmsMemory_[currentAddress_++];
}

void Tms9918::modeHighOut(Byte value)
{
	if (toggle_)
	{
		if ((value & 0xf8) == 0x80 )
		{
            writeRegister(value & 7, value_);
            reBlit_ = true;
            reDraw_ = true;
		}
		if ((value & 0xc0) == 0x40)
		{
			currentAddress_ = value_ +((value & 0x3f) << 8);
            reBlit_ = true;
    //        reDraw_ = true; //** test code!
		}
		toggle_ = 0;
	}
	else
	{
		value_ = value;
		toggle_ = 1;
	}
}

void Tms9918::writeVRAM(Byte value)
{
	int  p;
	Word addr;
    int row;

    addr = currentAddress_;
    if (value == tmsMemory_[currentAddress_])
    {
        currentAddress_++;
        return;
    }
    
	tmsMemory_[currentAddress_++] = value;
	Word memoryStart = p_Computer->getDebugMemoryStart();
	if (addr >= memoryStart && addr<(memoryStart + 256))
		p_Main->updateDebugMemory(addr);
    
	switch(mode_)
	{
		case TMS_GRAPHICS_I:
            if (addr >= nameAddress_ && addr < nameAddress_+0x300)
			{
				drawTile(addr - nameAddress_);
				return;
			}
            if (addr >= spriteAttributeTableAddress_ && addr < (spriteAttributeTableAddress_ + 0x80))
            {
                reDrawSprites_ = true;
                return;
            }
            if (addr >= spritePatternTableAddress_ && addr < (spritePatternTableAddress_ + 0x800))
            {
                reDrawSprites_ = true;
                return;
            }
            if (addr >= colorAddress_ && addr < (colorAddress_+0x20))
			{
				p = addr - colorAddress_;
				for (int i=0; i<768; i++)
					if ((tmsMemory_[nameAddress_+i] >> 3) == p) drawTile(i);
				return;
			}
			if (addr >= patternAddress_ && addr < (patternAddress_+0x800))
			{
				p = (addr - patternAddress_) >> 3;
				for (int i=0; i<768; i++)
					if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
                return;
			}
		break;

		case TMS_GRAPHICS_II:
			if (addr >= nameAddress_ && addr < (nameAddress_+0x300))
			{
				drawTile(addr - nameAddress_);
				return;
			}
            if (addr >= spriteAttributeTableAddress_ && addr < (spriteAttributeTableAddress_ + 0x80))
            {
                reDrawSprites_ = true;
                return;
            }
            if (addr >= spritePatternTableAddress_ && addr < (spritePatternTableAddress_ + 0x800))
            {
                reDrawSprites_ = true;
                return;
            }
			if (addr >= colorAddress_ && addr < (colorAddress_+0x800))
			{
				p = (addr - colorAddress_) >> 3;
				for (int i=0; i<256; i++)
                    if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
				return;
			}
			if (addr >= (colorAddress_+0x800) && addr < (colorAddress_+0x1000))
			{
				p = (addr -(colorAddress_+0x800)) >> 3;
				for (int i=256; i<512; i++)
                    if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
				return;
			}
			if (addr >= (colorAddress_+0x1000) && addr < (colorAddress_+0x1800))
			{
				p = (addr -(colorAddress_+0x1000)) >> 3;
				for (int i=512; i<768; i++)
                    if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
				return;
			}
			if (addr >= patternAddress_ && addr < (patternAddress_+0x800))
			{
				p = (addr - patternAddress_) >> 3;
				for (int i=0; i<256; i++)
                    if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
				return;
			}
			if (addr >= (patternAddress_+0x800) && addr < (patternAddress_+0x1000))
			{
				p = (addr -(patternAddress_+0x800)) >> 3;
				for (int i=256; i<512; i++)
                    if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
				return;
			}
			if (addr >= (patternAddress_+0x1000) && addr < (patternAddress_+0x1800))
			{
				p = (addr -(patternAddress_+0x1000)) >> 3;
				for (int i=512; i<768; i++)
                    if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
                return;
			}
		break;

		case TMS_TEXT:
			if (addr >= nameAddress_ && addr < nameAddress_+960)
			{
				drawTile(addr - nameAddress_);
			}
		break;
            
        case TMS_MULTICOLOR:
            if (addr >= nameAddress_ && addr < nameAddress_+768)
            {
                drawTile(addr - nameAddress_);
                return;
            }
            if (addr >= spriteAttributeTableAddress_ && addr < (spriteAttributeTableAddress_ + 0x80))
            {
                reDrawSprites_ = true;
                return;
            }
            if (addr >= spritePatternTableAddress_ && addr < (spritePatternTableAddress_ + 0x800))
            {
                reDrawSprites_ = true;
                return;
            }
           if (addr >= patternAddress_ && addr < (patternAddress_+0x800))
            {
                p = (addr - patternAddress_) & 0x7FE;

                for (int i=0; i<768; i++)
                {
                    row = ((i / 32) * 2) % 4;
                    if (tmsMemory_[nameAddress_+i] == ((p - row) / 8)) drawTilePatternUpdate(i, addr);
                }
                return;
            }
        break;
    }
}

void Tms9918::cycleTms()
{
	cycleValue_ --;
	if (cycleValue_ == 0)
	{
		cycleValue_ = cycleSize_;
        copyScreen();
		videoSyncCount_++;
        if (enableInterrupt_)
            statusRegister_ = statusRegister_ | 0x80;
		if (changeScreenSize_)
		{
			changeScreenSize();
			changeScreenSize_ = false;
		}
	}
}

void Tms9918::writeRegister(Byte reg, Byte value)
{
    registers_[reg] = value_;
    
    mode_ = TMS_GRAPHICS_I; // Graphics I
    if (registers_[0] & 2) mode_ = TMS_GRAPHICS_II; // Graphics II
    if (registers_[1] & 8) mode_ = TMS_MULTICOLOR; // Multicolor
    if (registers_[1] & 16) mode_ = TMS_TEXT; // Text

    switch (reg)
    {
        case 0:
        break;

        case 1:
            enableInterrupt_ = ((value_ & 0x20) == 0x20);
            spriteSize16_ = ((value_ & 0x2) == 0x2);
            spriteMagnify_ = ((value_ & 0x1) == 0x1);
            disableScreen_ = ((value_ & 0x40) != 0x40);
            spriteSelect_ = value & 0x3;
            
            if (spriteMagnify_)
                spriteMagnifyFactor_ = 16;
            else
                spriteMagnifyFactor_ = 8;

            switch (spriteSelect_)
            {
                case SPRITE_8_8:
                    spriteSize_ = 8;
                break;

                case SPRITE_8_8_MAG:
                    spriteSize_ = 16;
                break;
                    
                case SPRITE_16_16:
                    spriteSize_ = 16;
                break;

                case SPRITE_16_16_MAG:
                    spriteSize_ = 32;
                break;
            }
        break;

        case 2:
            nameAddress_ = (registers_[2] & 0xf) << 10;
        break;

        case 3:
        break;

        case 4:
        break;

        case 5:
            spriteAttributeTableAddress_ = (value_ & 0x7f) << 7;
        break;

        case 6:
            spritePatternTableAddress_ = (value_ & 0x7) << 11;
       break;

        case 7:
            textColor_ = (value_ & 0xf0) >> 4;
            backgroundColor_ = value_ & 0xf;
       break;
    }
    
    if (mode_ == TMS_GRAPHICS_II)
    {
        colorAddress_ = (registers_[3] & 128) ? 0x2000 : 0;
        patternAddress_ = (registers_[4] & 4) ? 0x2000 : 0;
    }
    else
    {
        colorAddress_ = registers_[3] << 6;
        patternAddress_ = (registers_[4] & 0x7) << 11;
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

    reCycle_ = false;
}

void Tms9918::copyScreen()
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

    if (reDrawSprites_ && mode_ != TMS_TEXT)
        drawSprites();

#if defined(__WXMAC__)
    if (reBlit_ || reDraw_ || reDrawSprites_)
    {
        dcMemoryMainAndSpritePlane.Blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemoryMainPlane, 0, 0);
        if (mode_ != TMS_TEXT)
            dcMemoryMainAndSpritePlane.Blit(offsetX_, offsetY_, videoWidth_, videoHeight_, &dcMemorySpritePlane, offsetX_, offsetY_);
        dcMemory.Blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemoryMainAndSpritePlane, 0, 0);

        p_Main->eventRefreshVideo(false, 0);
        reBlit_ = false;
        reDraw_ = false;
    }
#else
	if (extraBackGround_ && newBackGround_)
		drawExtraBackground(colour_[backgroundColor_+16]);

    TileList *temp;

	if (reBlit_ || reDraw_ || reDrawSprites_)
	{
        if (mode_ != TMS_TEXT)
            dcMemory.Blit(offsetX_, offsetY_, videoWidth_, videoHeight_, &dcMemorySpritePlane, offsetX_, offsetY_);

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
        if (mode_ != TMS_TEXT)
            dcMemory.Blit(offsetX_, offsetY_, videoWidth_, videoHeight_, &dcMemorySpritePlane, offsetX_, offsetY_);

        updateTile_ = 0;
		while(tileListPointer != NULL)
		{
			videoScreenPointer->blit(offsetX_+ tileListPointer->x-backGroundX_, offsetY_+tileListPointer->y-backGroundY_, tileListPointer->size, 8, &dcMemory, offsetX_+tileListPointer->x, offsetY_+tileListPointer->y);
			temp = tileListPointer;
			tileListPointer = temp->nextTile;
			delete temp;
		}
	}
#endif
}

void Tms9918::drawSprites()
{
    if (mode_ == TMS_TEXT)
        return;
   
    Word spriteAttributeTableAddress = spriteAttributeTableAddress_;
    Word spritePatternTableAddress;
    wxCoord y;
    wxCoord x;
    Byte namePointer;
    Byte color;
    bool earlyClock;

    for (int i=0; i<192; i++)
    {
        numberOfSpritesOnline_[i] = 0;
        scanLineMap_[i].reset();
    }

#if defined(__WXMAC__)
    dcMemorySpritePlane.SelectObject(wxNullBitmap);
    delete spritePlanePointer;
    delete gcSpritePlane;
    
    spritePlanePointer = new wxBitmap(320, 240);
    dcMemorySpritePlane.SelectObject(*spritePlanePointer);

    gcSpritePlane = wxGraphicsContext::Create(dcMemorySpritePlane);
    gcSpritePlane->SetAntialiasMode(wxANTIALIAS_NONE);
#else
    dcMemorySpritePlane.Blit(offsetX_, offsetY_, videoWidth_, videoHeight_, &dcMemoryMainPlane, offsetX_, offsetY_);
#endif

    while (tmsMemory_[spriteAttributeTableAddress] != 0xD0 && spriteAttributeTableAddress < (spriteAttributeTableAddress_+128))
    {
        y = tmsMemory_[spriteAttributeTableAddress++];
        x = tmsMemory_[spriteAttributeTableAddress++];
        namePointer = tmsMemory_[spriteAttributeTableAddress++];
        earlyClock = ((tmsMemory_[spriteAttributeTableAddress] & 0x80) == 0x80);
        color = tmsMemory_[spriteAttributeTableAddress++] & 0xf;
        
        spritePatternTableAddress = spritePatternTableAddress_ + namePointer * 8;

#if defined(__WXMAC__)
        gcSpritePlane->SetBrush(brushColour_[color+16]);
        gcSpritePlane->SetPen(penColour_[color+16]);
#else
        dcMemorySpritePlane.SetBrush(brushColour_[color+16]);
        dcMemorySpritePlane.SetPen(penColour_[color+16]);
#endif
        int numberOfLines = 8;
        
        if (spriteSize_ == 16)
            numberOfLines = 16;
        
        if (spriteMagnify_)
            numberOfLines *= 2;

        if (y >= 0xe0)
        {
            int hiddenLines = ((y&0x1f) ^ 0x1f) + 1;
            if (spriteMagnify_)
                spritePatternTableAddress = spritePatternTableAddress + hiddenLines / 2;
            else
                spritePatternTableAddress = spritePatternTableAddress + hiddenLines;
            numberOfLines = numberOfLines - hiddenLines;
            if (numberOfLines < 0)
                return;
            y = 0;
        }
        
        lastSpriteX_[namePointer] = x;
        lastSpriteY_[namePointer] = y;

        if (spriteMagnify_)
            drawSpriteMagnify(namePointer, spritePatternTableAddress, x, y, numberOfLines - spriteMagnifyFactor_, earlyClock);
        else
            drawSprite(namePointer, spritePatternTableAddress, x, y, numberOfLines - spriteMagnifyFactor_, earlyClock);

        if (spriteSize_ == 16)
        {
            int addition = numberOfLines-spriteMagnifyFactor_;
            if (numberOfLines < spriteMagnifyFactor_)
                addition = 0;

            if (spriteMagnify_)
            {
                drawSpriteMagnify(namePointer, spritePatternTableAddress+addition/2, x, y+addition, numberOfLines, earlyClock);
            }
            else
                drawSprite(namePointer, spritePatternTableAddress+addition, x, y+addition, numberOfLines, earlyClock);
        }
    }
}

void Tms9918::drawSprite(Byte namePointer, Word spritePatternTableAddress, wxCoord x, wxCoord y, int numberOfLines, bool earlyClock)
{
    if (numberOfLines < 0)
        return;
 
    if (numberOfLines > spriteMagnifyFactor_)
        numberOfLines = spriteMagnifyFactor_;
    
    Word pattern;
    
    wxCoord scanLine=y;

    if (earlyClock)
        x -= 32;
    
    while (scanLine < (y+numberOfLines) && scanLine < 192)
    {
        numberOfSpritesOnline_[scanLine]++;
        
        if (numberOfSpritesOnline_[scanLine] <= 4)
        {
            pattern = tmsMemory_[spritePatternTableAddress];
            
            for (wxCoord pixel=x; pixel < (x+8); pixel++)
            {
                if ((pattern & 0x80) == 0x80)
                {
                    if (pixel >= 0 && pixel <= 255)
                    {
                        if (scanLineMap_[scanLine].test(pixel))
                            statusRegister_ |= 0x20;
                        else
                        {
                            scanLineMap_[scanLine].set(pixel);
                            drawPointSpritePlane(pixel+offsetX_, scanLine+offsetY_);
                        }
                    }
                }
                
                pattern = pattern << 1;
            }
            if (spriteSize_ == 16)
            {
                pattern = tmsMemory_[spritePatternTableAddress+16];
                for (wxCoord pixel=(x+8); pixel < (x+16); pixel++)
                {
                    if ((pattern & 0x80) == 0x80)
                    {
                        if (pixel >= 0 && pixel <= 255)
                        {
                            if (scanLineMap_[scanLine].test(pixel))
                                statusRegister_ |= 0x20;
                            else
                            {
                                scanLineMap_[scanLine].set(pixel);
                                drawPointSpritePlane(pixel+offsetX_, scanLine+offsetY_);
                            }
                        }
                    }
                    
                    pattern = pattern << 1;
                }
            }
        }
        else
        {
            if ((statusRegister_ & 0x40) == 0)
            {
                statusRegister_ |= 0x40;
                statusRegister_ &= 0xe0;
                statusRegister_ |= namePointer;
            }
        }
        spritePatternTableAddress++;
        scanLine++;
    }
}

void Tms9918::drawSpriteMagnify(Byte namePointer, Word spritePatternTableAddress, wxCoord x, wxCoord y, int numberOfLines, bool earlyClock)
{
    if (numberOfLines < 0)
        return;
 
    if (numberOfLines > spriteMagnifyFactor_)
    {
        if ((numberOfLines&0x1) == 0x1)
            spritePatternTableAddress++;
        numberOfLines = spriteMagnifyFactor_;
    }
    
    bool unevenLines = false;
    if ((numberOfLines&0x1) == 0x1)
        unevenLines = true;
    
    Word pattern;
    
    wxCoord scanLine=y;

    if (earlyClock)
        x -= 32;
    
    while (scanLine < (y+numberOfLines) && scanLine < 192)
    {
        if (!unevenLines)
        {
            numberOfSpritesOnline_[scanLine]++;
            
            if (numberOfSpritesOnline_[scanLine] <= 4)
            {
                pattern = tmsMemory_[spritePatternTableAddress];
                
                for (wxCoord pixel=x; pixel < (x+16); pixel++)
                {
                    if ((pattern & 0x80) == 0x80)
                    {
                        if (pixel >= 0 && pixel <= 255)
                        {
                            if (scanLineMap_[scanLine].test(pixel))
                                statusRegister_ |= 0x20;
                            else
                            {
                                scanLineMap_[scanLine].set(pixel);
                                drawPointSpritePlane(pixel+offsetX_, scanLine+offsetY_);
                            }
                        }
                        pixel++;
                        if (pixel >= 0 && pixel <= 255)
                        {
                            if (scanLineMap_[scanLine].test(pixel))
                                statusRegister_ |= 0x20;
                            else
                            {
                                scanLineMap_[scanLine].set(pixel);
                                drawPointSpritePlane(pixel+offsetX_, scanLine+offsetY_);
                            }
                        }
                    }
                    else
                        pixel++;
                    
                    pattern = pattern << 1;
                }
                if (spriteSize_ == 16)
                {
                    pattern = tmsMemory_[spritePatternTableAddress+16];
                    for (wxCoord pixel=(x+16); pixel < (x+32); pixel++)
                    {
                        if ((pattern & 0x80) == 0x80)
                        {
                            if (pixel >= 0 && pixel <= 255)
                            {
                                if (scanLineMap_[scanLine].test(pixel))
                                    statusRegister_ |= 0x20;
                                else
                                {
                                    scanLineMap_[scanLine].set(pixel);
                                    drawPointSpritePlane(pixel+offsetX_, scanLine+offsetY_);
                                }
                            }
                            pixel++;
                            if (pixel >= 0 && pixel <= 255)
                            {
                                if (scanLineMap_[scanLine].test(pixel))
                                    statusRegister_ |= 0x20;
                                else
                                {
                                    scanLineMap_[scanLine].set(pixel);
                                    drawPointSpritePlane(pixel+offsetX_, scanLine+offsetY_);
                                }
                            }
                        }
                        else
                            pixel++;

                        pattern = pattern << 1;
                    }
                }
            }
            else
            {
                if ((statusRegister_ & 0x40) == 0)
                {
                    statusRegister_ |= 0x40;
                    statusRegister_ &= 0xe0;
                    statusRegister_ |= namePointer;
                }
            }
            
            scanLine++;
        }
        else
            unevenLines = false;
        
        if (scanLine < 192)
        {
            numberOfSpritesOnline_[scanLine]++;
            
            if (numberOfSpritesOnline_[scanLine] <= 4)
            {
                pattern = tmsMemory_[spritePatternTableAddress];
                
                for (wxCoord pixel=x; pixel < (x+16); pixel++)
                {
                    if ((pattern & 0x80) == 0x80)
                    {
                        if (pixel >= 0 && pixel <= 255)
                        {
                            if (scanLineMap_[scanLine].test(pixel))
                                statusRegister_ |= 0x20;
                            else
                            {
                                scanLineMap_[scanLine].set(pixel);
                                drawPointSpritePlane(pixel+offsetX_, scanLine+offsetY_);
                            }
                        }
                        pixel++;
                        if (pixel >= 0 && pixel <= 255)
                        {
                            if (scanLineMap_[scanLine].test(pixel))
                                statusRegister_ |= 0x20;
                            else
                            {
                                scanLineMap_[scanLine].set(pixel);
                                drawPointSpritePlane(pixel+offsetX_, scanLine+offsetY_);
                            }
                        }
                    }
                    else
                        pixel++;

                    pattern = pattern << 1;
                }
                if (spriteSize_ == 16)
                {
                    pattern = tmsMemory_[spritePatternTableAddress+16];
                    for (wxCoord pixel=(x+16); pixel < (x+32); pixel++)
                    {
                        if ((pattern & 0x80) == 0x80)
                        {
                            if (pixel >= 0 && pixel <= 255)
                            {
                                if (scanLineMap_[scanLine].test(pixel))
                                    statusRegister_ |= 0x20;
                                else
                                {
                                    scanLineMap_[scanLine].set(pixel);
                                    drawPointSpritePlane(pixel+offsetX_, scanLine+offsetY_);
                                }
                            }
                            pixel++;
                            if (pixel >= 0 && pixel <= 255)
                            {
                                if (scanLineMap_[scanLine].test(pixel))
                                    statusRegister_ |= 0x20;
                                else
                                {
                                    scanLineMap_[scanLine].set(pixel);
                                    drawPointSpritePlane(pixel+offsetX_, scanLine+offsetY_);
                                }
                            }
                        }
                        else
                            pixel++;

                        pattern = pattern << 1;
                    }
                }
            }
            else
            {
                if ((statusRegister_ & 0x40) == 0)
                {
                    statusRegister_ |= 0x40;
                    statusRegister_ &= 0xe0;
                    statusRegister_ |= namePointer;
                }
            }
            spritePatternTableAddress++;
            scanLine++;
        }
    }
}

void Tms9918::drawPointMainPlane(wxCoord x, wxCoord y)
{
#if defined(__WXMAC__)
    gcMainPlane->DrawRectangle(x, y, 0, 0);
#else
    dcMemoryMainPlane.DrawPoint(x, y);
#endif
}

void Tms9918::drawPointSpritePlane(wxCoord x, wxCoord y)
{
#if defined(__WXMAC__)
    gcSpritePlane->DrawRectangle(x, y, 0, 0);
#else
    dcMemorySpritePlane.DrawPoint(x, y);
#endif
}

void Tms9918::drawRectangleMainPlane(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
#if defined(__WXMAC__)
    gcMainPlane->DrawRectangle(x, y, width-1, height-1);
#else
    dcMemoryMainPlane.DrawRectangle(x, y, width, height);
#endif
}

void Tms9918::setColourMutexMainPlane(int clr)
{
#if defined(__WXMAC__)
    gcMainPlane->SetBrush(brushColour_[clr]);
    gcMainPlane->SetPen(penColour_[clr]);
#else
    dcMemoryMainPlane.SetBrush(brushColour_[clr]);
    dcMemoryMainPlane.SetPen(penColour_[clr]);
#endif
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
		case TMS_GRAPHICS_I:
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
					if (cl == 0) cl = backgroundColor_;
					if (cl == 0) cl = 1;
#if defined(__linux__) || defined (__WXMSW__)
					setColourMutex(cl+16);
                    drawPoint(x+px+offsetX_, y+py+offsetY_);
#endif
                    setColourMutexMainPlane(cl+16);
                    drawPointMainPlane(x+px+offsetX_, y+py+offsetY_);
					b = (b << 1) & 0xff;
				}
			}
		break;

		case TMS_GRAPHICS_II:
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
					if (cl == 0) cl = backgroundColor_;
					if (cl == 0) cl = 1;
#if defined(__linux__) || defined (__WXMSW__)
 					setColourMutex(cl+16);
                    drawPoint(x+px+offsetX_, y+py+offsetY_);
#endif
                    setColourMutexMainPlane(cl+16);
                    drawPointMainPlane(x+px+offsetX_, y+py+offsetY_);
					b = (b << 1) & 0xff;
				}
			}
		break;

		case TMS_TEXT:
			x = (tile % 40)*6+8;
			y = (tile / 40)*8;
			size = 6;

            setColourMutexMainPlane(backgroundColor_+16);
            drawRectangleMainPlane(x+offsetX_, y+offsetY_, 6, 8);

			for (int py=0; py<8; py++)
			{
				b = tmsMemory_[patternAddress_+p*8+py];
				for (int px=0; px<6; px++)
				{
					if (b & 128)
					{
						cl = textColor_;
						if (cl == 0) cl = backgroundColor_;
#if defined(__linux__) || defined (__WXMSW__)
                        setColourMutex(cl+16);
                        drawPoint(x+px+offsetX_, y+py+offsetY_);
#endif
                        setColourMutexMainPlane(cl+16);
                        drawPointMainPlane(x+px+offsetX_, y+py+offsetY_);
					}
					b = (b << 1) & 0xff;
				}
			}
		break;

        case TMS_MULTICOLOR:
            x = (tile % 32);
            y = (tile / 32);

            b =  2 * (y % 4) + p * 8;

            cl = (tmsMemory_[b] & 0xf0) >> 4;
            
#if defined(__linux__) || defined (__WXMSW__)
            setColourMutex(cl+16);
            drawRectangle(x * 8 + offsetX_, y * 8 + offsetY_, 4 ,4);
#endif
            setColourMutexMainPlane(cl+16);
            drawRectangleMainPlane(x * 8 + offsetX_, y * 8 + offsetY_, 4 ,4);
            
            cl = tmsMemory_[b] & 0xf;
            
#if defined(__linux__) || defined (__WXMSW__)
            setColourMutex(cl+16);
            drawRectangle(x * 8 + 4 + offsetX_, y * 8 + offsetY_, 4 ,4);
#endif
            setColourMutexMainPlane(cl+16);
            drawRectangleMainPlane(x * 8 + 4 + offsetX_, y * 8 + offsetY_, 4 ,4);

            b++;
  
            cl = (tmsMemory_[b] & 0xf0) >> 4;

#if defined(__linux__) || defined (__WXMSW__)
            setColourMutex(cl+16);
            drawRectangle(x * 8 + offsetX_, y * 8 + 4 + offsetY_, 4 ,4);
#endif
            setColourMutexMainPlane(cl+16);
            drawRectangleMainPlane(x * 8 + offsetX_, y * 8 + 4 + offsetY_, 4 ,4);
            
            cl = tmsMemory_[b] & 0xf;
            
#if defined(__linux__) || defined (__WXMSW__)
            setColourMutex(cl+16);
            drawRectangle(x * 8 + 4 + offsetX_, y * 8 + 4 + offsetY_, 4 ,4);
#endif
            setColourMutexMainPlane(cl+16);
            drawRectangleMainPlane(x * 8 + 4 + offsetX_, y * 8 + 4 + offsetY_, 4 ,4);
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

void Tms9918::drawTilePatternUpdate(Word tile, Word address)
{
    int p;
    int c;
    int b;
    int cl;
    int pytemp;
    int x,y, size;
    Word ofs;

    p = tmsMemory_[nameAddress_+tile];
    pytemp = address & 0x7;
    
    switch(mode_)
    {
        case TMS_GRAPHICS_I:
            c = tmsMemory_[colorAddress_ +(p >> 3)];
            x = (tile % 32)*8;
            y = (tile / 32)*8;
            size = 8;


            b = tmsMemory_[address];
            for (int px=0; px<8; px++)
            {
                cl = (b & 128) ? c>>4 : c & 0xf;
                if (cl == 0) cl = backgroundColor_;
                if (cl == 0) cl = 1;
#if defined(__linux__) || defined (__WXMSW__)
                setColourMutex(cl+16);
                drawPoint(x+px+offsetX_, y+pytemp+offsetY_);
#endif
                setColourMutexMainPlane(cl+16);
                drawPointMainPlane(x+px+offsetX_, y+pytemp+offsetY_);
                b = (b << 1) & 0xff;
            }
        break;

        case TMS_GRAPHICS_II:
            x = tile % 32;
            y = tile / 32;
            size = 8;

            ofs = 0;
            if (y>7) ofs = 2048;
            if (y>15) ofs = 4096;

            x = x*8;
            y = y*8;

            b = tmsMemory_[ofs + patternAddress_ + p*8 + pytemp];
            c = tmsMemory_[ofs + colorAddress_ + p*8 + pytemp];
            for (int px=0; px<8; px++)
            {
                cl = (b & 128) ? c>>4 : c & 0xf;
                if (cl == 0) cl = backgroundColor_;
                if (cl == 0) cl = 1;
#if defined(__linux__) || defined (__WXMSW__)
                setColourMutex(cl+16);
                drawPoint(x+px+offsetX_, y+pytemp+offsetY_);
#endif
                setColourMutexMainPlane(cl+16);
                drawPointMainPlane(x+px+offsetX_, y+pytemp+offsetY_);
                b = (b << 1) & 0xff;
            }
        break;

        case TMS_MULTICOLOR:
            x = (tile % 32);
            y = (tile / 32);

            b =  2 * (y % 4) + p * 8;
            
            if ((address & 1) == 0)
            {
                cl = (tmsMemory_[b] & 0xf0) >> 4;
                
#if defined(__linux__) || defined (__WXMSW__)
                setColourMutex(cl+16);
                drawRectangle(x * 8 + offsetX_, y * 8 + offsetY_, 4 ,4);
#endif
                setColourMutexMainPlane(cl+16);
                drawRectangleMainPlane(x * 8 + offsetX_, y * 8 + offsetY_, 4 ,4);
                
                cl = tmsMemory_[b] & 0xf;
                
#if defined(__linux__) || defined (__WXMSW__)
                setColourMutex(cl+16);
                drawRectangle(x * 8 + 4 + offsetX_, y * 8 + offsetY_, 4 ,4);
#endif
                setColourMutexMainPlane(cl+16);
                drawRectangleMainPlane(x * 8 + 4 + offsetX_, y * 8 + offsetY_, 4 ,4);
            }
            else
            {
                b++;
      
                cl = (tmsMemory_[b] & 0xf0) >> 4;

#if defined(__linux__) || defined (__WXMSW__)
                setColourMutex(cl+16);
                drawRectangle(x * 8 + offsetX_, y * 8 + 4 + offsetY_, 4 ,4);
#endif
                setColourMutexMainPlane(cl+16);
                drawRectangleMainPlane(x * 8 + offsetX_, y * 8 + 4 + offsetY_, 4 ,4);
                
                cl = tmsMemory_[b] & 0xf;
                
#if defined(__linux__) || defined (__WXMSW__)
                setColourMutex(cl+16);
                drawRectangle(x * 8 + 4 + offsetX_, y * 8 + 4 + offsetY_, 4 ,4);
#endif
                setColourMutexMainPlane(cl+16);
                drawRectangleMainPlane(x * 8 + 4 + offsetX_, y * 8 + 4 + offsetY_, 4 ,4);
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
#if defined(__WXMAC__)
	setColourMutexMainPlane(backgroundColor_+16);
    drawRectangleMainPlane(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);
#else
    setColourMutex(backgroundColor_+16);
    drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);
#endif
//    drawRectangleMain(0, 0, videoWidth_ + 2*offsetX_, offsetY_);
//    drawRectangle(0, videoHeight_ + offsetY_, videoWidth_ + 2*offsetX_, offsetY_);
//    drawRectangle(0, offsetY_, offsetX_, videoHeight_);
//    drawRectangle(videoWidth_ + offsetX_, offsetY_, offsetX_, videoHeight_);
	if (mode_ == TMS_GRAPHICS_I)
	{
		for (int x=0; x<768; x++)
			drawTile(x);
	}
	if (mode_ == TMS_GRAPHICS_II)
	{
		for (int x=0; x<768; x++)
			drawTile(x);
	}
	if (mode_ == TMS_TEXT)
	{
		for (int x=0; x<960; x++)
			drawTile(x);
	}
    if (mode_ == TMS_MULTICOLOR)
    {
        for (int x=0; x<768; x++)
            drawTile(x);
    }
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

void Tms9918::reBlit(wxDC &dc)
{
    if (!memoryDCvalid_)
        return;
 
    if (disableScreen_)
    {
        dc.SetUserScale(zoom_*xZoomFactor_, zoom_);
        dc.SetBrush(wxBrush(colour_[backgroundColor_+16]));
        dc.SetPen(wxPen(colour_[backgroundColor_+16]));
        dc.DrawRectangle(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
        return;
    }

    dc.Blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);

    if (extraBackGround_ && newBackGround_)
    {
        wxSize size = wxGetDisplaySize();

        dc.SetBrush(wxBrush(colour_[backgroundColor_+16]));
        dc.SetPen(wxPen(colour_[backgroundColor_+16]));

        int xStart = (int)((2*offsetX_+videoWidth_)*zoom_*xZoomFactor_);
        dc.DrawRectangle(xStart, 0, size.x-xStart, size.y);

        int yStart = (int)((2*offsetY_+videoHeight_)*zoom_);
        dc.DrawRectangle(0, yStart, size.x, size.y-yStart);

        newBackGround_ = false;
    }
}

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

#define SPRITE_8_8 0
#define SPRITE_8_8_MAG 1
#define SPRITE_16_16 2
#define SPRITE_16_16_MAG 3

Tms9918::Tms9918(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, TmsConfiguration tmsConfiguration)
: Video(title, pos, size)
{
    zoom_ = zoom;
    clock_ = clock;
    colourIndex_ = 0;
    tmsConfiguration_ = tmsConfiguration;
    videoNumber_ = tmsConfiguration_.videoNumber;

    videoType_ = VIDEOXMLTMS;
    colourIndex_ = COL_TMS_TRANSPARANT-16;

    double intPart;
    zoomFraction_ = (modf(zoom_, &intPart) != 0);

    defineColours();

    videoScreenPointer = new VideoScreen(this, size, zoom, videoNumber_);

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

    // The software framebuffer is enabled on ALL platforms (see video.h/
    // video.cpp); render per-pixel drawing through it (one DrawBitmap per
    // frame instead of per-pixel draws). Plane 0 (dcMemory) is only composited
    // via Blit, so the single plane-0 graphics context below is all the macOS
    // non-framebuffer fallback draw path needs. On Linux
    // wxGraphicsContext::Create(dcMemory) creates a Cairo context bound to the
    // screenCopyPointer Pixmap; leaving it unguarded caused a BadDrawable X
    // error on exit when that Pixmap was freed out of order.
#if defined(__WXMAC__)
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif
    enableFramebufferMac();   // software framebuffer (planes 1 + 2)
    
    mode_ = TMS_GRAPHICS_I;
    nameAddress_ = 0;
    colorAddress_ = 0;
    patternAddress_ = 0;
    currentWriteAddress_ = 0;
    currentReadAddress_ = 0;
    spriteAttributeTableAddress_ = 0;
    spritePatternTableAddress_ = 0;
    statusRegister_ = 0;
    videoMemoryMask_  = 0xfff;
    disableScreen_ = true;
    enableInterrupt_ = false;
    spriteSize16_ = 0;
    spriteMagnify_ = 0;
    spriteSelect_ = 0;

    p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setTms9918Register(TMS9918_FIFTH_SPRITE, (Byte)(statusRegister_ & 0x1f), DO_NOT_SHOW_ANY_TRACE);
    p_Main->setTms9918SelectorValue(TMS9918_MEMORY_SIZE_LARGE, videoMemoryMask_ == 0x3fff, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setTms9918SelectorValue(TMS9918_DISPLAY_ENABLE, !disableScreen_, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setTms9918SelectorValue(TMS9918_INTERRUPT_ENABLE, enableInterrupt_, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setTms9918DisplayMode(mode_, DO_NOT_SHOW_ANY_TRACE);
    p_Main->setTms9918SelectorValue(TMS9918_SPRITE_SIZE, spriteSelect_, DO_NOT_SHOW_ANY_TRACE);

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
            for (int i=0; i<8; i++) writeRegister(i, 0, DO_NOT_SHOW_ANY_TRACE);
        break;
            
        case STARTUP_RANDOM:
            for (int i=0; i<8; i++) writeRegister(i, rand() % 0x100, DO_NOT_SHOW_ANY_TRACE);
        break;
    }
        
    toggle_ = false;

    for (int x=0; x<512; x+=8)
    {
        for (int y=0; y<384; y+=8)
        {
            multiColour_[x][y] = 0;
            multiColour_[x+1][y] = 0;
            multiColour_[x][y+1] = 0;
            multiColour_[x+1][y+1] = 0;
        }
    }
    reDrawMultiColor_ = false;
    
    offsetX_ = 0;
    offsetY_ = 0;

    setCycle();

    cycleValue_ = cycleSize_;
    newBackGround_ = false;
    fullScreenSet_ = false;
    reDrawSprites_ = false;

    videoWidth_ = 256;
    videoHeight_ = 192;

    this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);

    setColourMutexMainPlane(colourIndex_+backgroundColor_+16);
    drawRectangleMainPlane(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);
}

Tms9918::~Tms9918()
{
    dcMemory.SelectObject(wxNullBitmap);
    dcMemoryMainAndSpritePlane.SelectObject(wxNullBitmap);
    dcMemoryMainPlane.SelectObject(wxNullBitmap);
    dcMemorySpritePlane.SelectObject(wxNullBitmap);

    delete screenCopyPointer;
    delete mainAndSpritePlanePointer;
    delete mainPlanePointer;
    delete spritePlanePointer;
    delete videoScreenPointer;
#if defined(__WXMAC__)
    delete gc;
#endif
}

void Tms9918::configure()
{
    changeScreenSize();

    p_Main->configureMessage(&tmsConfiguration_.ioGroupVector, "TMS 9918");
    p_Computer->setOutType(&tmsConfiguration_.ioGroupVector, tmsConfiguration_.registerOutput, "register port");
    p_Computer->setInType(&tmsConfiguration_.ioGroupVector, tmsConfiguration_.registerOutput, "register port");
    p_Computer->setOutType(&tmsConfiguration_.ioGroupVector, tmsConfiguration_.dataOutput, "data port");
    p_Computer->setInType(&tmsConfiguration_.ioGroupVector, tmsConfiguration_.dataOutput, "data port");
    p_Computer->setEfType(&tmsConfiguration_.ioGroupVector, tmsConfiguration_.efInterrupt, "interrupt");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_TMS9918, TMS_CYCLE);
    
    p_Main->message("");
}

Byte Tms9918::readEf()
{
    Byte returnValue = (((statusRegister_&0x80) >> 7) ^ 0x1) ^ tmsConfiguration_.efInterrupt.reverse;
    
    return returnValue;
}

Byte Tms9918::readDataPort()
{
    Byte returnValue = statusRegister_;
    
    statusRegister_ = statusRegister_ & 0x1f;
    p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_);
    return returnValue;
}

Byte Tms9918::readVRAM()
{
    Byte returnValue = tmsMemory_[currentReadAddress_++];
    currentReadAddress_ &= videoMemoryMask_;
    int showTrace = p_Main->setTms9918Register(TMS9918_DATA, returnValue);
    p_Main->setTms9918Register(TMS9918_READ_ADDRESS, currentReadAddress_, showTrace);
    return returnValue;
}

void Tms9918::modeHighOut(Byte value, int showTrace)
{
    if (toggle_)
    {
        showTrace = p_Main->setTms9918Register(TMS9918_REGISTER, value, showTrace);
        if ((value & 0xf8) == 0x80 )
        {
            writeRegister(value & 7, value_, showTrace);
            reBlit_ = true;
            reDraw_ = true;
        }
        if ((value & 0xc0) == 0)
        {
            currentReadAddress_ = (value_ +((value & 0x3f) << 8)) & videoMemoryMask_;
            p_Main->setTms9918Register(TMS9918_READ_ADDRESS, currentReadAddress_, showTrace);
        }
        if ((value & 0xc0) == 0x40)
        {
            currentWriteAddress_ = (value_ +((value & 0x3f) << 8)) & videoMemoryMask_;
            p_Main->setTms9918Register(TMS9918_WRITE_ADDRESS, currentWriteAddress_, showTrace);
            reBlit_ = true;
    //        reDraw_ = true; //** test code!
        }
        toggle_ = false;
    }
    else
    {
        p_Main->setTms9918RegisterValue(TMS9918_REGISTER, value, showTrace);
        value_ = value;
        toggle_ = true;
    }
}

void Tms9918::writeVRAM(Byte value, int showTrace)
{
    showTrace = p_Main->setTms9918Register(TMS9918_DATA, value, showTrace);
    int  p;
    Word addr;

    addr = currentWriteAddress_;
    if (value == tmsMemory_[currentWriteAddress_])
    {
        currentWriteAddress_++;
        currentWriteAddress_ &= videoMemoryMask_;
        p_Main->setTms9918Register(TMS9918_WRITE_ADDRESS, currentWriteAddress_, showTrace);
        return;
    }
    
    tmsMemory_[currentWriteAddress_++] = value;
    currentWriteAddress_ &= videoMemoryMask_;
    p_Main->setTms9918Register(TMS9918_WRITE_ADDRESS, currentWriteAddress_, showTrace);
    Word memoryStart = p_Computer->getDebugMemoryStart();
    if (addr >= memoryStart && addr<(memoryStart + 256))
        p_Main->updateDebugMemory(addr);
    
    switch(mode_)
    {
        case TMS_GRAPHICS_I:
            if (addr >= nameAddress_ && addr < nameAddress_+0x300)
            {
                drawTile(addr - nameAddress_);
            }
            if (addr >= spriteAttributeTableAddress_ && addr < (spriteAttributeTableAddress_ + 0x80))
            {
                reDrawSprites_ = true;
            }
            if (addr >= spritePatternTableAddress_ && addr < (spritePatternTableAddress_ + 0x800))
            {
                reDrawSprites_ = true;
            }
            if (addr >= colorAddress_ && addr < (colorAddress_+0x20))
            {
                p = addr - colorAddress_;
                for (int i=0; i<768; i++)
                    if ((tmsMemory_[nameAddress_+i] >> 3) == p) drawTile(i);
            }
            if (addr >= patternAddress_ && addr < (patternAddress_+0x800))
            {
                p = (addr - patternAddress_) >> 3;
                for (int i=0; i<768; i++)
                    if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
            }
        break;

        case TMS_GRAPHICS_II:
            if (addr >= nameAddress_ && addr < (nameAddress_+0x300))
            {
                drawTile(addr - nameAddress_);
            }
            if (addr >= spriteAttributeTableAddress_ && addr < (spriteAttributeTableAddress_ + 0x80))
            {
                reDrawSprites_ = true;
            }
            if (addr >= spritePatternTableAddress_ && addr < (spritePatternTableAddress_ + 0x800))
            {
                reDrawSprites_ = true;
            }
            if (addr >= colorAddress_ && addr < (colorAddress_+0x800))
            {
                p = (addr - colorAddress_) >> 3;
                for (int i=0; i<256; i++)
                    if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
            }
            if (addr >= (colorAddress_+0x800) && addr < (colorAddress_+0x1000))
            {
                p = (addr -(colorAddress_+0x800)) >> 3;
                for (int i=256; i<512; i++)
                    if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
            }
            if (addr >= (colorAddress_+0x1000) && addr < (colorAddress_+0x1800))
            {
                p = (addr -(colorAddress_+0x1000)) >> 3;
                for (int i=512; i<768; i++)
                    if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
            }
            if (addr >= patternAddress_ && addr < (patternAddress_+0x800))
            {
                p = (addr - patternAddress_) >> 3;
                for (int i=0; i<256; i++)
                    if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
            }
            if (addr >= (patternAddress_+0x800) && addr < (patternAddress_+0x1000))
            {
                p = (addr -(patternAddress_+0x800)) >> 3;
                for (int i=256; i<512; i++)
                    if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
            }
            if (addr >= (patternAddress_+0x1000) && addr < (patternAddress_+0x1800))
            {
                p = (addr -(patternAddress_+0x1000)) >> 3;
                for (int i=512; i<768; i++)
                    if (tmsMemory_[nameAddress_+i] == p) drawTilePatternUpdate(i, addr);
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
            }
            if (addr >= spriteAttributeTableAddress_ && addr < (spriteAttributeTableAddress_ + 0x80))
            {
                reDrawSprites_ = true;
            }
            if (addr >= spritePatternTableAddress_ && addr < (spritePatternTableAddress_ + 0x800))
            {
                reDrawSprites_ = true;
            }
           if (addr >= patternAddress_ && addr < (patternAddress_+0x800))
            {
                reDrawMultiColor_ = true;
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
        videoSyncCount_++;
        if (enableInterrupt_)
        {
            statusRegister_ = statusRegister_ | 0x80;
            p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
        }
        if (changeScreenSize_)
            changeScreenSize();
        copyScreen();
    }
}

void Tms9918::writeRegister(Byte reg, Byte value, int showTrace)
{
    switch (reg)
    {
        case 0:
            registers_[reg] = value & 0x3;
            mode_ = setDisplayMode();
            showTrace = p_Main->setTms9918RegisterNibble(TMS9918_R0, registers_[reg], showTrace);
            p_Main->setTms9918DisplayMode(mode_, showTrace);
        break;

        case 1:
            registers_[reg] = value;
            mode_ = setDisplayMode();
            spriteMagnify_ = ((value & 0x1) == 0x1);
            spriteSize16_ = ((value & 0x2) == 0x2);
            enableInterrupt_ = ((value & 0x20) == 0x20);
            disableScreen_ = ((value & 0x40) != 0x40);
            if ((value & 0x80) == 0x80)
                videoMemoryMask_  = 0x3fff;
            else
                videoMemoryMask_  = 0xfff;
            spriteSelect_ = value & 0x3;

            switch (spriteSelect_)
            {
                case SPRITE_8_8:
                    spriteSize_ = 8;
                    spriteMagnifyFactor_ = 0;
                break;

                case SPRITE_8_8_MAG:
                    spriteSize_ = 16;
                    spriteMagnifyFactor_ = 0;
                break;
                    
                case SPRITE_16_16:
                    spriteSize_ = 16;
                    spriteMagnifyFactor_ = 8;
                break;

                case SPRITE_16_16_MAG:
                    spriteSize_ = 32;
                    spriteMagnifyFactor_ = 16;
                break;
            }
            showTrace = p_Main->setTms9918Register(TMS9918_R1, registers_[reg], showTrace);
            showTrace = p_Main->setTms9918SelectorValue(TMS9918_SPRITE_SIZE, spriteSelect_, showTrace);
            showTrace = p_Main->setTms9918DisplayMode(mode_, showTrace);
            showTrace = p_Main->setTms9918SelectorValue(TMS9918_INTERRUPT_ENABLE, enableInterrupt_, showTrace);
            showTrace = p_Main->setTms9918SelectorValue(TMS9918_DISPLAY_ENABLE, !disableScreen_, showTrace);
            showTrace = p_Main->setTms9918SelectorValue(TMS9918_MEMORY_SIZE_LARGE, videoMemoryMask_ == 0x3fff, showTrace);
        break;

        case 2:
            nameAddress_ = (value & 0xf) << 10;
            p_Main->setTms9918Register(TMS9918_R2, nameAddress_, showTrace);
        break;

        case 3:
            if (mode_ == TMS_GRAPHICS_II)
                colorAddress_ = (value & 128) ? 0x2000 : 0;
            else
                colorAddress_ = value << 6;
            p_Main->setTms9918Register(TMS9918_R3, colorAddress_, showTrace);
        break;

        case 4:
            if (mode_ == TMS_GRAPHICS_II)
                patternAddress_ = (value & 4) ? 0x2000 : 0;
            else
                patternAddress_ = (value & 0x7) << 11;
            p_Main->setTms9918Register(TMS9918_R4, patternAddress_, showTrace);
        break;

        case 5:
            spriteAttributeTableAddress_ = (value & 0x7f) << 7;
            p_Main->setTms9918Register(TMS9918_R5, spriteAttributeTableAddress_, showTrace);
        break;

        case 6:
            spritePatternTableAddress_ = (value & 0x7) << 11;
            p_Main->setTms9918Register(TMS9918_R6, spritePatternTableAddress_, showTrace);
       break;

        case 7:
            textColor_ = (value & 0xf0) >> 4;
            backgroundColor_ = value & 0xf;
            p_Main->setTms9918Register(TMS9918_R7, value, showTrace);
       break;
    }
}

void Tms9918::writeRegisterDirect(Byte reg, Word value, int showTrace)
{
    switch (reg)
    {
        case 0:
            registers_[reg] = value & 0x3;
            mode_ = setDisplayMode();
            showTrace = p_Main->setTms9918RegisterNibble(TMS9918_R0, registers_[reg], showTrace);
            p_Main->setTms9918DisplayMode(mode_, showTrace);
            reDraw_ = true;
        break;

        case 1:
            registers_[reg] = value & 0xff;
            mode_ = setDisplayMode();
            showTrace = p_Main->setTms9918Register(TMS9918_R1, registers_[reg], showTrace);
            p_Main->setTms9918DisplayMode(mode_, showTrace);
            enableInterrupt_ = ((value & 0x20) == 0x20);
            spriteSize16_ = ((value & 0x2) == 0x2);
            spriteMagnify_ = ((value & 0x1) == 0x1);
            disableScreen_ = ((value & 0x40) != 0x40);
            if ((value & 0x80) == 0x80)
                videoMemoryMask_  = 0x3fff;
            else
                videoMemoryMask_  = 0xfff;
            showTrace = p_Main->setTms9918SelectorValue(TMS9918_DISPLAY_ENABLE, !disableScreen_, showTrace);
            showTrace = p_Main->setTms9918SelectorValue(TMS9918_INTERRUPT_ENABLE, enableInterrupt_, showTrace);
            showTrace = p_Main->setTms9918SelectorValue(TMS9918_MEMORY_SIZE_LARGE, videoMemoryMask_ == 0x3fff, showTrace);
            spriteSelect_ = value & 0x3;

            switch (spriteSelect_)
            {
                case SPRITE_8_8:
                    spriteSize_ = 8;
                    spriteMagnifyFactor_ = 0;
                break;

                case SPRITE_8_8_MAG:
                    spriteSize_ = 16;
                    spriteMagnifyFactor_ = 0;
                break;
                    
                case SPRITE_16_16:
                    spriteSize_ = 16;
                    spriteMagnifyFactor_ = 8;
                break;

                case SPRITE_16_16_MAG:
                    spriteSize_ = 32;
                    spriteMagnifyFactor_ = 16;
                break;
            }
            p_Main->setTms9918SelectorValue(TMS9918_SPRITE_SIZE, spriteSelect_);
            reDraw_ = true;
        break;

        case 2:
            nameAddress_ = value & 0x3c00;
            p_Main->setTms9918Register(TMS9918_R2, nameAddress_, showTrace);
            reDraw_ = true;
        break;

        case 3:
            colorAddress_ = value & 0x3fc0;
            p_Main->setTms9918Register(TMS9918_R3, colorAddress_, showTrace);
            reDraw_ = true;
        break;

        case 4:
            patternAddress_ = value & 0x3800;
            p_Main->setTms9918Register(TMS9918_R4, patternAddress_, showTrace);
            reDraw_ = true;
        break;

        case 5:
            spriteAttributeTableAddress_ = value & 0x3f80;
            p_Main->setTms9918Register(TMS9918_R5, spriteAttributeTableAddress_, showTrace);
            reDrawSprites_ = true;
        break;

        case 6:
            spritePatternTableAddress_ = value & 0x3800;
            p_Main->setTms9918Register(TMS9918_R6, spritePatternTableAddress_, showTrace);
            reDrawSprites_ = true;
       break;

        case 7:
            textColor_ = (value & 0xf0) >> 4;
            backgroundColor_ = value & 0xf;
            p_Main->setTms9918Register(TMS9918_R7, value, showTrace);
            reDraw_ = true;
       break;
    }
}

Byte Tms9918::setDisplayMode()
{
    Byte mode = TMS_GRAPHICS_I; // Graphics I
    if (registers_[0] & 2)
        mode = TMS_GRAPHICS_II; // Graphics II
    if (registers_[1] & 8)
        mode = TMS_MULTICOLOR; // Multicolor
    if (registers_[1] & 16)
        mode = TMS_TEXT; // Text

    return mode;
}

void Tms9918::setStatus(Byte value)
{
    statusRegister_ = value;
    
    p_Main->setTms9918Register(TMS9918_STATUS, value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Tms9918::setFifthSprite(Byte value)
{
    statusRegister_ &= 0xe0;
    statusRegister_ |= (value & 0x1f);

    p_Main->setTms9918Register(TMS9918_FIFTH_SPRITE, (Byte)(value & 0x1f), DO_NOT_SHOW_ADDRESS_TRACE);
    p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Tms9918::setDisplayModeDirect(Byte value)
{
    reDraw_ = true;
    mode_ = value & 0x7;
    registers_[0] &= 0xfd;
    registers_[1] &= 0xe7;
    switch (mode_)
    {
        case TMS_GRAPHICS_II:
            registers_[0] |= 0x2;
        break;

        case TMS_MULTICOLOR:
            registers_[1] |= 0x8;
        break;

        case TMS_TEXT:
            registers_[1] |= 0x10;
        break;
    }
    p_Main->setTms9918DisplayMode(mode_, DO_NOT_SHOW_ADDRESS_TRACE);
    p_Main->setTms9918RegisterNibble(TMS9918_R0, registers_[0], DO_NOT_SHOW_ADDRESS_TRACE);
    p_Main->setTms9918Register(TMS9918_R1, registers_[1], DO_NOT_SHOW_ADDRESS_TRACE);
    p_Main->setTms9918Register(TMS9918_DISPLAY_MODE, value, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Tms9918::setCurrentReadAddress(Word value)
{
    currentReadAddress_ = value & videoMemoryMask_;
    p_Main->setTms9918Register(TMS9918_READ_ADDRESS, currentReadAddress_, DO_NOT_SHOW_ADDRESS_TRACE);
}

void Tms9918::setCurrentWriteAddress(Word value)
{
    currentWriteAddress_ = value & videoMemoryMask_;
    p_Main->setTms9918Register(TMS9918_WRITE_ADDRESS, currentWriteAddress_, DO_NOT_SHOW_ADDRESS_TRACE);
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
        for (int i=0; i<VIDEOXMLMAX; i++)
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
    else
    {
        if (reDrawMultiColor_)
            drawScreenMultiColor();
    }

    if (reDrawSprites_ && mode_ != TMS_TEXT)
        drawSprites();

    // The software framebuffer is flushed into the plane DCs identically on
    // every platform; only how dcMemory reaches the window differs. macOS
    // posts an async refresh (onPaint -> reBlit(dc), which also paints the
    // extra background); Windows/Linux draw the extra background and blit the
    // client DC directly from the emulation thread here.
    if (reBlit_ || reDraw_ || reDrawSprites_)
    {
        flushFramebufferMac();
        dcMemoryMainAndSpritePlane.Blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemoryMainPlane, 0, 0);
        if (mode_ != TMS_TEXT)
            dcMemoryMainAndSpritePlane.Blit(offsetX_, offsetY_, videoWidth_, videoHeight_, &dcMemorySpritePlane, offsetX_, offsetY_);
        dcMemory.Blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemoryMainAndSpritePlane, 0, 0);
#if defined(__WXMAC__)
        p_Main->eventRefreshVideo(false, videoNumber_);
#else
        if (extraBackGround_ && newBackGround_)
            drawExtraBackground(colour_[colourIndex_+backgroundColor_+16]);
        videoScreenPointer->blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);
#endif
        reBlit_ = false;
        reDraw_ = false;
    }
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

    // Seed the sprite plane framebuffer (plane 2) with the current main-plane
    // content (memory-to-memory). This replaces the per-frame bitmap recreate
    // + blit, which can only read flushed (previous-frame) DC content
    // mid-frame.
    copyFramebufferMac(1, 2);

    while (tmsMemory_[spriteAttributeTableAddress] != 0xD0 && spriteAttributeTableAddress < (spriteAttributeTableAddress_+128))
    {
        y = tmsMemory_[spriteAttributeTableAddress++];
        x = tmsMemory_[spriteAttributeTableAddress++];
        namePointer = tmsMemory_[spriteAttributeTableAddress++];
        earlyClock = ((tmsMemory_[spriteAttributeTableAddress] & 0x80) == 0x80);
        color = tmsMemory_[spriteAttributeTableAddress++] & 0xf;
        
        spritePatternTableAddress = spritePatternTableAddress_ + namePointer * 8;

        setMacPlane(2);
        Video::setColour(brushColour_[colourIndex_+color+16].GetColour());
        setMacPlane(0);
        int numberOfLines = 8;
        
        if ((spriteSize_ == 16 && !spriteMagnify_) || (spriteSize_ == 32 && spriteMagnify_))
        {
            numberOfLines = 16;
        }
        
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

        if ((spriteSize_ == 16 && !spriteMagnify_) || (spriteSize_ == 32 && spriteMagnify_))
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
 
    if (numberOfLines > spriteMagnifyFactor_ && spriteMagnifyFactor_ != 0)
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
                        {
                            statusRegister_ |= 0x20;
                            p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
                        }
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
                            {
                                statusRegister_ |= 0x20;
                                p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
                            }
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
                p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
                p_Main->setTms9918Register(TMS9918_FIFTH_SPRITE, (Byte)(statusRegister_ & 0x1f), DO_NOT_SHOW_ANY_TRACE);
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
        if (spriteMagnifyFactor_ != 0)
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
                            {
                                statusRegister_ |= 0x20;
                                p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
                            }
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
                            {
                                statusRegister_ |= 0x20;
                                p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
                            }
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
                if (spriteSize_ == 32)
                {
                    pattern = tmsMemory_[spritePatternTableAddress+16];
                    for (wxCoord pixel=(x+16); pixel < (x+32); pixel++)
                    {
                        if ((pattern & 0x80) == 0x80)
                        {
                            if (pixel >= 0 && pixel <= 255)
                            {
                                if (scanLineMap_[scanLine].test(pixel))
                                {
                                    statusRegister_ |= 0x20;
                                    p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
                                }
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
                                {
                                    statusRegister_ |= 0x20;
                                    p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
                                }
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
                    p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
                    p_Main->setTms9918Register(TMS9918_FIFTH_SPRITE, (Byte)(statusRegister_ & 0x1f), DO_NOT_SHOW_ANY_TRACE);
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
                            {
                                statusRegister_ |= 0x20;
                                p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
                            }
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
                            {
                                statusRegister_ |= 0x20;
                                p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
                            }
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
                if (spriteSize_ == 32)
                {
                    pattern = tmsMemory_[spritePatternTableAddress+16];
                    for (wxCoord pixel=(x+16); pixel < (x+32); pixel++)
                    {
                        if ((pattern & 0x80) == 0x80)
                        {
                            if (pixel >= 0 && pixel <= 255)
                            {
                                if (scanLineMap_[scanLine].test(pixel))
                                {
                                    statusRegister_ |= 0x20;
                                    p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
                                }
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
                                {
                                    statusRegister_ |= 0x20;
                                    p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
                                }
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
                    p_Main->setTms9918Register(TMS9918_STATUS, statusRegister_, DO_NOT_SHOW_ANY_TRACE);
                    p_Main->setTms9918Register(TMS9918_FIFTH_SPRITE, (Byte)(statusRegister_ & 0x1f), DO_NOT_SHOW_ANY_TRACE);
                }
            }
            spritePatternTableAddress++;
            scanLine++;
        }
    }
}

void Tms9918::drawPointMainPlane(wxCoord x, wxCoord y)
{
    setMacPlane(1);
    Video::drawPoint(x, y);
    setMacPlane(0);
}

void Tms9918::drawPointSpritePlane(wxCoord x, wxCoord y)
{
    setMacPlane(2);
    Video::drawPoint(x, y);
    setMacPlane(0);
}

void Tms9918::drawRectangleMainPlane(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    setMacPlane(1);
    Video::drawRectangle(x, y, width, height);
    setMacPlane(0);
}

void Tms9918::setColourMutexMainPlane(int clr)
{
    setMacPlane(1);
    Video::setColour(clr);
    setMacPlane(0);
}

void Tms9918::drawTile(Word tile)
{
    int p;
    int c;
    int b;
    int cl;
    int x=0, y=0;
    Word ofs;

    p = tmsMemory_[nameAddress_+tile];
    switch(mode_)
    {
        case TMS_GRAPHICS_I:
            c = tmsMemory_[colorAddress_ +(p >> 3)];
            x = (tile % 32)*8;
            y = (tile / 32)*8;

            for (int py=0; py<8; py++)
            {
                b = tmsMemory_[patternAddress_+p*8+py];
                for (int px=0; px<8; px++)
                {
                    cl = (b & 128) ? c>>4 : c & 0xf;
                    if (cl == 0) cl = backgroundColor_;
                    if (cl == 0) cl = 1;
                    setColourMutexMainPlane(colourIndex_+cl+16);
                    drawPointMainPlane(x+px+offsetX_, y+py+offsetY_);
                    b = (b << 1) & 0xff;
                }
            }
        break;

        case TMS_GRAPHICS_II:
            x = tile % 32;
            y = tile / 32;

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
                    setColourMutexMainPlane(colourIndex_+cl+16);
                    drawPointMainPlane(x+px+offsetX_, y+py+offsetY_);
                    b = (b << 1) & 0xff;
                }
            }
        break;

        case TMS_TEXT:
            x = (tile % 40)*6+8;
            y = (tile / 40)*8;

            setColourMutexMainPlane(colourIndex_+backgroundColor_+16);
            drawRectangleMainPlane(x+offsetX_, y+offsetY_, 6, 8);

            for (int py=0; py<8; py++)
            {
                b = tmsMemory_[patternAddress_+p*8+py];
                for (int px=0; px<6; px++)
                {
                    if (b & 128)
                        cl = textColor_;
                    else
                        cl = backgroundColor_;
                    
                    if (cl == 0) cl = backgroundColor_;
                    setColourMutexMainPlane(colourIndex_+cl+16);
                    drawPointMainPlane(x+px+offsetX_, y+py+offsetY_);
                    b = (b << 1) & 0xff;
                }
            }
        break;

        case TMS_MULTICOLOR:
            x = (tile % 32) * 8;
            y = (tile / 32);

            b =  2 * (y % 4) + p * 8;
            y *= 8;
            
            cl = (tmsMemory_[patternAddress_+b] & 0xf0) >> 4;
            
            multiColour_[x][y] = cl;
            setColourMutexMainPlane(colourIndex_+cl+16);
            drawRectangleMainPlane(x+offsetX_, y+offsetY_, 4 ,4);
            
            cl = tmsMemory_[patternAddress_+b] & 0xf;
            
            multiColour_[x+1][y] = cl;
            setColourMutexMainPlane(colourIndex_+cl+16);
            drawRectangleMainPlane(x+offsetX_ + 4, y+offsetY_, 4 ,4);

            b++;
  
            cl = (tmsMemory_[patternAddress_+b] & 0xf0) >> 4;

            multiColour_[x][y+1] = cl;
            setColourMutexMainPlane(colourIndex_+cl+16);
            drawRectangleMainPlane(x+offsetX_, y+offsetY_ + 4, 4 ,4);
            cl = tmsMemory_[patternAddress_+b] & 0xf;
            
            multiColour_[x+1][y+1] = cl;
            setColourMutexMainPlane(colourIndex_+cl+16);
            drawRectangleMainPlane(x+offsetX_ + 4, y+offsetY_ + 4, 4 ,4);
        break;

        default:
            x = 0;
            y = 0;
        break;
    }
    // Full-frame refresh on all platforms: the software framebuffer flushes
    // the whole plane each frame.
    reBlit_ = true;
}

void Tms9918::drawTileMultiColor(Word tile)
{
    int p;
    int b;
    int cl;
    int x=0, y=0;

    p = tmsMemory_[nameAddress_+tile];

    x = (tile % 32) * 8;
    y = (tile / 32);

    b =  2 * (y % 4) + p * 8;
    y *= 8;
    
    cl = (tmsMemory_[patternAddress_+b] & 0xf0) >> 4;
    
    if (multiColour_[x][y] != cl)
    {
        multiColour_[x][y] = cl;
        setColourMutexMainPlane(colourIndex_+cl+16);
        drawRectangleMainPlane(x+offsetX_, y+offsetY_, 4 ,4);
    }
    
    cl = tmsMemory_[patternAddress_+b] & 0xf;
    
    if (multiColour_[x+1][y] != cl)
    {
        multiColour_[x+1][y] = cl;
        setColourMutexMainPlane(colourIndex_+cl+16);
        drawRectangleMainPlane(x+offsetX_ + 4, y+offsetY_, 4 ,4);
    }
    b++;

    cl = (tmsMemory_[patternAddress_+b] & 0xf0) >> 4;

    if (multiColour_[x][y+1] != cl)
    {
        multiColour_[x][y+1] = cl;
        setColourMutexMainPlane(colourIndex_+cl+16);
        drawRectangleMainPlane(x+offsetX_, y+offsetY_ + 4, 4 ,4);
    }
    cl = tmsMemory_[patternAddress_+b] & 0xf;
    
    if (multiColour_[x+1][y+1] != cl)
    {
        multiColour_[x+1][y+1] = cl;
        setColourMutexMainPlane(colourIndex_+cl+16);
        drawRectangleMainPlane(x+offsetX_ + 4, y+offsetY_ + 4, 4 ,4);
    }
    reBlit_ = true;
}

void Tms9918::drawTilePatternUpdate(Word tile, Word address)
{
    int p;
    int c;
    int b;
    int cl;
    int pytemp;
    int x=0, y=0;
    Word ofs;

    p = tmsMemory_[nameAddress_+tile];
    pytemp = address & 0x7;
    
    switch(mode_)
    {
        case TMS_GRAPHICS_I:
            c = tmsMemory_[colorAddress_ +(p >> 3)];
            x = (tile % 32)*8;
            y = (tile / 32)*8;

            b = tmsMemory_[address];
            for (int px=0; px<8; px++)
            {
                cl = (b & 128) ? c>>4 : c & 0xf;
                if (cl == 0) cl = backgroundColor_;
                if (cl == 0) cl = 1;
                setColourMutexMainPlane(colourIndex_+cl+16);
                drawPointMainPlane(x+px+offsetX_, y+pytemp+offsetY_);
                b = (b << 1) & 0xff;
            }
        break;

        case TMS_GRAPHICS_II:
            x = tile % 32;
            y = tile / 32;

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
                setColourMutexMainPlane(colourIndex_+cl+16);
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
                cl = (tmsMemory_[patternAddress_+b] & 0xf0) >> 4;
                
                setColourMutexMainPlane(colourIndex_+cl+16);
                drawRectangleMainPlane(x * 8 + offsetX_, y * 8 + offsetY_, 4 ,4);
                
                cl = tmsMemory_[patternAddress_+b] & 0xf;
                
                setColourMutexMainPlane(colourIndex_+cl+16);
                drawRectangleMainPlane(x * 8 + 4 + offsetX_, y * 8 + offsetY_, 4 ,4);
            }
            else
            {
                b++;
      
                cl = (tmsMemory_[patternAddress_+b] & 0xf0) >> 4;

                setColourMutexMainPlane(colourIndex_+cl+16);
                drawRectangleMainPlane(x * 8 + offsetX_, y * 8 + 4 + offsetY_, 4 ,4);
                
                cl = tmsMemory_[patternAddress_+b] & 0xf;
                
                setColourMutexMainPlane(colourIndex_+cl+16);
                drawRectangleMainPlane(x * 8 + 4 + offsetX_, y * 8 + 4 + offsetY_, 4 ,4);
            }
        break;

        default:
            x = 0;
            y = 0;
        break;
    }
    // Full-frame refresh on all platforms: the software framebuffer flushes
    // the whole plane each frame.
    reBlit_ = true;
}

void Tms9918::drawScreen()
{
    setColourMutexMainPlane(colourIndex_+backgroundColor_+16);
    drawRectangleMainPlane(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);
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

void Tms9918::drawScreenMultiColor()
{
    for (int x=0; x<768; x++)
        drawTileMultiColor(x);
}

void Tms9918::setFullScreen(bool fullScreenSet)
{
    fullScreenSet_ = fullScreenSet;
    ShowFullScreen(fullScreenSet);
}

void Tms9918::onF3()
{
    fullScreenSet_ = !fullScreenSet_;
    p_Main->eventVideoSetFullScreen(fullScreenSet_, videoNumber_);
}

void Tms9918::reBlit(wxDC &dc)
{
    if (!memoryDCvalid_)
        return;
 
    if (disableScreen_)
    {
        dc.SetUserScale(zoom_*xZoomFactor_, zoom_);
        dc.SetBrush(wxBrush(colour_[colourIndex_+backgroundColor_+16]));
        dc.SetPen(wxPen(colour_[colourIndex_+backgroundColor_+16]));
        dc.DrawRectangle(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
        return;
    }

    dc.Blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);

    if (extraBackGround_ && newBackGround_)
    {
        wxSize size = wxGetDisplaySize();

        dc.SetBrush(wxBrush(colour_[colourIndex_+backgroundColor_+16]));
        dc.SetPen(wxPen(colour_[colourIndex_+backgroundColor_+16]));

        int xStart = (int)((2*offsetX_+videoWidth_)*zoom_*xZoomFactor_);
        dc.DrawRectangle(xStart, 0, size.x-xStart, size.y);

        int yStart = (int)((2*offsetY_+videoHeight_)*zoom_);
        dc.DrawRectangle(0, yStart, size.x, size.y-yStart);

        newBackGround_ = false;
    }
}

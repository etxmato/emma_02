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

#include "wx/graphics.h"
#include "wx/dcgraph.h"

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#define EF_REFRESH 0
#define EF_NONDISPLAY 1

#include "main.h"
#include "sn76430n.h"

BEGIN_EVENT_TABLE(SN76430N, wxFrame)
    EVT_CLOSE (SN76430N::onClose)
    EVT_SIZE(SN76430N::onSize)
END_EVENT_TABLE()

SN76430N::SN76430N(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, int computerType, double clock, double videoClock, IoConfiguration ioConfiguration, int videoNumber)
: Video(title, pos, size)
{
    clock_ = clock;
    videoClock_ = videoClock;
    ioConfiguration_ = ioConfiguration;
    videoType_ = VIDEOXMLSN76430N;
    videoNumber_ = videoNumber;

    computerType_ = computerType;
 
    colourIndex_ = COL_SN76430N_WHITE;

    fullScreenSet_ = false;
    zoom_ = zoom;

    videoWidth_ = 128;
    videoHeight_ = 96;
    
    double intPart;
    zoomFraction_ = (modf(zoom_, &intPart) != 0);
    
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    offsetX_ = 0;
    offsetY_ = 0;
        

    videoScreenPointer = new VideoScreen(this, wxSize(videoWidth_, videoHeight_), zoom, computerType, videoNumber_);
    
    setCycle();
    defineColours(computerType_);

    screenCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    dcMemory.SelectObject(*screenCopyPointer);
    
#if defined(__WXMAC__)
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

    this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);
    this->SetBackgroundColour(COL_SN76430N_BLACK);
    characterListPointer = NULL;
}

SN76430N::~SN76430N()
{
    CharacterList *temp;

    dcMemory.SelectObject(wxNullBitmap);
    delete videoScreenPointer;
    delete screenCopyPointer;
#if defined(__WXMAC__)
    delete gc;
#endif
    if (updateCharacter_)
    {
        while(characterListPointer != NULL)
        {
            temp = characterListPointer;
            characterListPointer = temp->nextCharacter;
            delete temp;
        }
    }
}

void SN76430N::focus()
{
    this->SetFocus();
    videoScreenPointer->SetFocus();
}

void SN76430N::configure()
{
    p_Computer->setCycleType(VIDEOCYCLE_SN76430N, SN76430NCYCLE);

    p_Main->message("Configuring Video SN76430N\n");
}

void SN76430N::init()
{
    cycleValue_ = cycleSize_;
    changeScreenSize_ = false;
    reDraw_ = true;
    reBlit_ = false;
    newBackGround_ = false;
    extraBackGround_ = false;
    updateCharacter_ = false;
}

void SN76430N::cycle()
{
    cycleValue_--;
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

void SN76430N::writeRam(Word address, Byte value)
{    
    int x = (address & 0x3f)*2;
    int y = ((address & 0x7c0) >> 6)*3;
    drawCharacter(x, y, value);
}

void SN76430N::copyScreen()
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
        p_Main->eventRefreshVideo(false, videoNumber_);
        reBlit_ = false;
        reDraw_ = false;
    }
#else
    if (extraBackGround_ && newBackGround_)
        drawExtraBackground(colour_[COL_SN76430N_BLACK]);

    CharacterList *temp;

    if (reBlit_ || reDraw_)
    {
        videoScreenPointer->blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);
        reBlit_ = false;
        reDraw_ = false;
        if (updateCharacter_)
        {
            updateCharacter_ = false;
            while(characterListPointer != NULL)
            {
                temp = characterListPointer;
                characterListPointer = temp->nextCharacter;
                delete temp;
            }
        }
    }
#endif
}

void SN76430N::drawScreen()
{
    setColour(COL_SN76430N_BLACK);
    drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);

    for (Word address=0x800; address<=0xfff; address++)
    {
        int x = (address & 0x3f)*2;
        int y = ((address & 0x7c0) >> 6)*3;
        drawCharacter(x, y, p_Computer->readMemDebug(address));
    }
}

void SN76430N::drawCharacter(wxCoord x, wxCoord y, Byte value)
{
    setColour(COL_SN76430N_BLACK);
    drawRectangle(x+offsetX_, y+offsetY_, 2, 3);

    setColour(colourIndex_ + (value >> 6));

    if (value & 0x10)
        drawPointMutex(x + offsetX_, y + offsetY_);
    if (value & 0x20)
        drawPointMutex(x + 1 + offsetX_, y + offsetY_);
    if (value & 0x4)
        drawPointMutex(x + offsetX_, y + 1 + offsetY_);
    if (value & 0x8)
        drawPointMutex(x + 1 + offsetX_, y + 1 + offsetY_);
    if (value & 0x1)
        drawPointMutex(x + offsetX_, y + 2 + offsetY_);
    if (value & 0x2)
        drawPointMutex(x + 1 + offsetX_, y + 2 + offsetY_);

    reBlit_ = true;
}

void SN76430N::setClock(double clock)
{
    clock_ = clock;
    reCycle_ = true;
}

void SN76430N::setCycle()
{
    float clockPeriod;
    int fieldTime;

    clockPeriod = (float)((1/videoClock_) * 6);
    fieldTime = clockPeriod * 50 * 96;

    cycleSize_ = (int)(fieldTime / ((1/clock_) * 8));
    cycleValue_ = cycleSize_;
    reCycle_ = false;
}

void SN76430N::setFullScreen(bool fullScreenSet)
{
    fullScreenSet_ = fullScreenSet;
    ShowFullScreen(fullScreenSet);
}

void SN76430N::onF3()
{
    fullScreenSet_ = !fullScreenSet_;
    p_Main->eventVideoSetFullScreen(fullScreenSet_, videoNumber_);
}

void SN76430N::reBlit(wxDC &dc)
{
    if (!memoryDCvalid_)
        return;

    dc.Blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);

    if (extraBackGround_ && newBackGround_)
    {
        wxSize size = wxGetDisplaySize();

        dc.SetBrush(wxBrush(colour_[COL_SN76430N_BLACK]));
        dc.SetPen(wxPen(colour_[COL_SN76430N_BLACK]));

        int xStart = (int)((2*offsetX_+videoWidth_)*zoom_*xZoomFactor_);
        dc.DrawRectangle(xStart, 0, size.x-xStart, size.y);

        int yStart = (int)((2*offsetY_+videoHeight_)*zoom_);
        dc.DrawRectangle(0, yStart, size.x, size.y-yStart);

        newBackGround_ = false;
    }
}


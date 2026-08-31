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

SN76430N::SN76430N(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double clock, double videoClock, Sn76430NConfiguration sn76430NConfiguration)
: Video(title, pos, size)
{
    clock_ = clock;
    videoClock_ = videoClock;
    videoType_ = VIDEOXMLSN76430N;
    sn76430NConfiguration_ = sn76430NConfiguration;
    videoNumber_ = sn76430NConfiguration.videoNumber;
 
    colourIndex_ = COL_SN76430N_WHITE;

    fullScreenSet_ = false;
    zoom_ = zoom;

    videoWidth_ = 128;
    videoHeight_ = 96;
    
    double intPart;
    
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    offsetX_ = 0;
    offsetY_ = 0;
        
    videoScreenPointer = new VideoScreen(this, wxSize(videoWidth_, videoHeight_), zoom, sn76430NConfiguration_.videoNumber);
    
    setCycle();
    defineColours();

    screenCopyPointer = new wxBitmap(videoWidth_, videoHeight_);
    dcMemory.SelectObject(*screenCopyPointer);

    // The software framebuffer is enabled on ALL platforms (see video.h/
    // video.cpp); render per-pixel drawing through it (one DrawBitmap per
    // frame instead of per-pixel draws). The graphics context is only created
    // on macOS, where the non-framebuffer fallback draw path needs it.
    // On Linux wxGraphicsContext::Create(dcMemory) creates a Cairo context
    // bound to the screenCopyPointer Pixmap; leaving it unguarded caused a
    // BadDrawable X error on exit when that Pixmap was freed out of order.
#if defined(__WXMAC__)
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif
    enableFramebufferMac();

    this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_, (videoHeight_+2*borderY_[videoType_])*zoom_);
    this->SetBackgroundColour(COL_SN76430N_BLACK);
}

SN76430N::~SN76430N()
{
    dcMemory.SelectObject(wxNullBitmap);
    delete videoScreenPointer;
    delete screenCopyPointer;
#if defined(__WXMAC__)
    delete gc;
#endif
}

void SN76430N::focus()
{
    this->SetFocus();
    videoScreenPointer->SetFocus();
}

void SN76430N::configure()
{
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_SN76430N, SN76430N_CYCLE);

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
}

void SN76430N::cycle()
{
    cycleValue_--;
    if (cycleValue_ == 0)
    {
        cycleValue_ = cycleSize_;
        videoSyncCount_++;
        if (changeScreenSize_)
            changeScreenSize();
        copyScreen();
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

    updateReColour();

    if (reCycle_)
        setCycle();

    if (reDraw_)
        drawScreen();

    finishCopyScreen();
}

wxColour SN76430N::copyScreenBackgroundColour()
{
    return colour_[COL_SN76430N_BLACK];
}

void SN76430N::drawScreen()
{
    setColour(COL_SN76430N_BLACK);
    drawRectangle(0, 0, videoWidth_ + 2*offsetX_, videoHeight_ + 2*offsetY_);

    for (Word address=sn76430NConfiguration_.startRam; address<=sn76430NConfiguration_.endRam; address++)
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
    fieldTime = clockPeriod * 50 * 96 * 2;

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
    p_Main->eventVideoSetFullScreen(fullScreenSet_, sn76430NConfiguration_.videoNumber);
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


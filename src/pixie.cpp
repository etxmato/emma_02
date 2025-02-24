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
#include "statusbar.h"

Pixie::Pixie(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int videoNumber, int videoType, CDP1861Configuration cdp1861Configuration, CDP1862Configuration cdp1862Configuration, int bootStrapType)
: Video(title, pos, size)
{
    cdp1861Configuration_ = cdp1861Configuration;
    cdp1862Configuration_ = cdp1862Configuration;
    videoNumber_ = videoNumber;
    
    videoType_ = videoType;
    highRes_ = 1;
    xZoomFactor_ = zoomfactor;

    switch (videoType)
    {
        case VIDEOXML1864:
            colourIndex_ = COL_CDP1864_WHITE;
            backGroundInit_ = COL_CDP1864_BACK_BLUE-COL_CDP1864_WHITE;
        break;
            
        case VIDEOSTUDIOIV:
            colourIndex_ = COL_ST4_BLACK;
            backGroundInit_ = COL_ST4_BACK_BLACK-COL_ST4_BLACK;
            studioIVFactor_ = true;
            highRes_ = 2;
            xZoomFactor_ = zoomfactor/highRes_;
        break;

        default:
            if (cdp1862Configuration.defined)
            {
                colourIndex_ = COL_CDP1862_WHITE;
                backGroundInit_ = COL_CDP1862_BACK_BLUE-COL_CDP1862_WHITE;
            }
            else
            {
                colourIndex_ = COL_PIXIE_FORE;
                backGroundInit_ = COL_PIXIE_BACK;
            }
            if (cdp1861Configuration_.colorType == PIXIE_COLOR_VISICOM)
                colourIndex_ = COL_PIXIE_BACK;
            if (cdp1861Configuration_.highRes)
            {
                highRes_ = 2;
                xZoomFactor_ = zoomfactor/highRes_;
            }
        break;
    }

    interlace_ = p_Main->getInterlace();

    videoHeight_ = 128;

//    cdp1861Configuration_.pixieGraphics.interrupt = 74;
//    cdp1861Configuration_.pixieGraphics.start = 76;
//    cdp1861Configuration_.pixieGraphics.end = 267;
//    cdp1861Configuration_.pixieGraphics.screenend = 312;
    
//    cdp1861Configuration_.pixieGraphics = p_Main->getPixieGraphics(XML);
    videoHeight_ = cdp1861Configuration.pixieGraphics.videoHeight;
    videoWidth_ = cdp1861Configuration.pixieGraphics.videoWidth;
    
    if (!p_Main->isFullScreenFloat())
        xZoomFactor_ = (int) xZoomFactor_;
    
    graphicsX_ = 8*highRes_;
    videoWidth_ = videoWidth_*highRes_;

    backGround_ = backGroundInit_;

    this->SetClientSize(size);

    videoScreenPointer = new VideoScreen(this, size, zoom, videoNumber_, xZoomFactor_, (cdp1861Configuration_.statusBarType == STATUSBAR_VIP2 && bootStrapType != BOOTSTRAPRUN));

#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    if (cdp1861Configuration_.statusBarType == STATUSBAR_VIP2)
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
    
    defineColours();

    plotListPointer = NULL;
    colourType_ = cdp1861Configuration_.colorType;
    bgChanged = false;
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
    if (cdp1861Configuration_.statusBarType == STATUSBAR_VIP2)
        delete vipIIStatusBarPointer;
}

void Pixie::reset()
{
    graphicsOn_ = false;
#if defined(__WXMAC__)
    p_Main->eventRefreshVideo(false, videoNumber_);
#else
    videoScreenPointer->disableScreen(colour_[colourIndex_+backGround_], videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
#endif
}

void Pixie::configurePixie()
{
    p_Main->configureMessage(&cdp1861Configuration_.ioGroupVector, "CDP 1861");
    p_Computer->setOutType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.output, "disable graphics");
    p_Computer->setInType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.input, "enable graphics");
    p_Computer->setEfType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.ef, "in frame indicator");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_CDP1861, CDP1861_CYCLE);

    p_Main->message("");

    if (cdp1861Configuration_.colorType == PIXIE_COLOR_VISICOM)
        backGroundInit_ = 0;
    else
        backGroundInit_ = 1;
    colourMask_ = 0;
}

void Pixie::configurePixieIn()
{
    p_Main->configureMessage(&cdp1861Configuration_.ioGroupVector, "CDP 1861");
    p_Computer->setInType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.output, "disable graphics");
    p_Computer->setInType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.input, "enable graphics");
    p_Computer->setEfType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.ef, "in frame indicator");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_CDP1861, CDP1861_CYCLE);

    p_Main->message("");

    if (cdp1861Configuration_.colorType == PIXIE_COLOR_VISICOM)
        backGroundInit_ = 0;
    else
        backGroundInit_ = 1;
    colourMask_ = 0;
}

void Pixie::configurePixieOut()
{
    p_Main->configureMessage(&cdp1861Configuration_.ioGroupVector, "CDP 1861");
    p_Computer->setOutType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.output, "disable graphics");
    p_Computer->setOutType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.input, "enable graphics");
    p_Computer->setEfType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.ef, "in frame indicator");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_CDP1861, CDP1861_CYCLE);

    p_Main->message("");

    if (cdp1861Configuration_.colorType == PIXIE_COLOR_VISICOM)
        backGroundInit_ = 0;
    else
        backGroundInit_ = 1;
    colourMask_ = 0;
}

void Pixie::configurePixieSuper()
{
    p_Main->configureMessage(&cdp1861Configuration_.ioGroupVector, "CDP 1861");
    p_Computer->setInType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.output, "disable graphics");
    p_Computer->setOutType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.output, "disable graphics");
    p_Computer->setInType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.input, "enable graphics");
    p_Computer->setOutType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.input, "enable graphics");
    p_Computer->setEfType(&cdp1861Configuration_.ioGroupVector, cdp1861Configuration_.ef, "in frame indicator");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_CDP1861, CDP1861_CYCLE);

    p_Main->message("");

    backGroundInit_ = 1;
    colourMask_ = 0;
}

void Pixie::configurePixieCoinArcade(CoinConfiguration coinConfiguration)
{
    p_Main->configureMessage(&coinConfiguration.ioGroupVector, "Coin Arcade Video");
    p_Computer->setOutType(&coinConfiguration.ioGroupVector, coinConfiguration.output, "enable graphics");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_COIN, COIN_VIDEO_CYCLE);

    p_Main->message("");
    
    backGroundInit_ = 1;
    colourMask_ = 0;
}

void Pixie::configureCdp1862(bool autoBoot)
{
    wxString printBuffer = "";

    p_Main->configureMessage(&cdp1862Configuration_.ioGroupVector, "CDP 1862");

    p_Computer->setOutType(&cdp1862Configuration_.ioGroupVector, cdp1862Configuration_.background, "switch background colour");
    if (cdp1862Configuration_.colorMemory.portNumber[0] != -1)
    {
        printBuffer.Printf("color RAM (mask %02X)", p_Main->getIoMask(CDP1862_COLORRAM_OUT));
        p_Computer->setOutType(&cdp1862Configuration_.ioGroupVector, cdp1862Configuration_.colorMemory, printBuffer);
    }
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_CDP1861, CDP1862_CYCLE);

    p_Main->message("");

    backGroundInit_ = COL_CDP1862_BACK_BLUE-COL_CDP1862_WHITE;
    colourMask_ = 0;
    colourType_ = cdp1862Configuration_.colorType;
    
    if (cdp1861Configuration_.statusBarType == STATUSBAR_VIP2)
        vipIIStatusBarPointer->initVipIIBar(autoBoot);
}

void Pixie::configureCdp1864(CDP1864Configuration cdp1864Configuration)
{
    wxString printBuffer = "";

    p_Main->configureMessage(&cdp1864Configuration.ioGroupVector, "CDP 1864");

    p_Computer->setInType(&cdp1864Configuration.ioGroupVector, cdp1864Configuration.enable, "enable graphics");
    p_Computer->setInType(&cdp1864Configuration.ioGroupVector, cdp1864Configuration.disable, "disable graphics");
    p_Computer->setOutType(&cdp1864Configuration.ioGroupVector, cdp1864Configuration.background, "switch background colour");
    if (cdp1864Configuration.colorMemory.portNumber[0] != -1)
    {
        printBuffer.Printf("color RAM (mask %02X)", p_Main->getIoMask(CDP1864_COLORRAM_OUT));
        p_Computer->setOutType(&cdp1864Configuration.ioGroupVector, cdp1864Configuration.colorMemory, printBuffer);
    }
    if (cdp1864Configuration.colorLatch)
        p_Computer->setOutType(&cdp1864Configuration.ioGroupVector, cdp1864Configuration.toneLatch, "tone & colour latch");
    else
        p_Computer->setOutType(&cdp1864Configuration.ioGroupVector, cdp1864Configuration.toneLatch, "tone latch");
        
    printBuffer.Printf("	EF %d: in frame indicator\n", cdp1864Configuration.ef);
    p_Main->message(printBuffer);

    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_CDP1864, CDP1864_CYCLE);

    backGroundInit_ = COL_CDP1864_BACK_BLUE-COL_CDP1864_WHITE;
    colourMask_ = 0;
    colourType_ = cdp1864Configuration.colorType;
    
    p_Computer->setTonePeriod(0, 32 *(128 + 1), false);
    p_Computer->setTonePeriod(1, 32 *(128 + 1), false);
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

    for (int x=0; x<384; x++) for (int y=0; y<208; y++)
    {
        pbacking_[x][y] = 0;
        color_[x][y] = 0;
    }
    for (int graphicsMode = 0; graphicsMode < cdp1861Configuration_.pixieGraphics.screenend; graphicsMode++)
    {
         bgColor_[graphicsMode] = backGroundInit_;
    }
    
    if (wxIsMainThread())
        this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_*xZoomFactor_, (videoHeight_+2*borderY_[videoType_])*zoom_);
    else
        p_Main->eventSetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_*xZoomFactor_, (videoHeight_+2*borderY_[videoType_])*zoom_, DON_T_CALL_CHANGE_SCREEN_SIZE, false, videoNumber_);

    graphicsOn_ = false;
    graphicsNext_ = 0;
    graphicsMode_ = 0;

    newBackGround_ = false;
    reBlit_ = true;
    reDraw_ = false;
    updatePlot_ = 0;
    backGround_ = backGroundInit_;

    changeScreenSize();
#if defined(__WXMAC__)
    p_Main->eventRefreshVideo(false, videoNumber_);
#else
    videoScreenPointer->disableScreen(colour_[colourIndex_+backGround_], videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
#endif
    drawScreen();
}

Byte Pixie::efPixie()
{
    return pixieEf_^cdp1861Configuration_.ef.reverse;
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
    reBlit_ = true;
}

void Pixie::outPixieBackGround()
{
    graphicsOn_ = true;
    backGround_++;
    if (backGround_ == 12)  backGround_ = 8;
    newBackGround_ = true;
    reBlit_ = true;
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
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.interrupt-2)
            pixieEf_ = 0;
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.start)
            pixieEf_ = 1;
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.start+videoHeight_-4)
            pixieEf_ = 0;
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.start+videoHeight_)
            pixieEf_ = 1;
        if (graphicsMode_ >= cdp1861Configuration_.pixieGraphics.screenend)
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
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.interrupt)
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
    if (graphicsNext_ == 4)
         drawBackgroundLine();
    if (graphicsMode_ >= cdp1861Configuration_.pixieGraphics.start && graphicsMode_ <=cdp1861Configuration_.pixieGraphics.end && graphicsOn_ && vidInt_ == 1 && graphicsNext_ >=4 && graphicsNext_ < (4+graphicsX_))
    {
        j = 0;
        while(graphicsNext_ >= 4 && graphicsNext_ < (4+graphicsX_))
        {
            graphicsNext_ ++;
            if (colourType_ == PIXIE_COLOR_VISICOM)
            {
                p_Computer->visicomDmaOut(&vram1, &vram2);
                for (int i=0; i<8; i++)
                {
                    color = 0;
                    if (vram1 & 128)    color |= 1;
                    if (vram2 & 128)    color |= 2;
                    plot(j+i, (int)graphicsMode_-cdp1861Configuration_.pixieGraphics.start, 1, color);
                    vram1 <<= 1;
                    vram2 <<= 1;
                }
            }
            else
            {
                v = p_Computer->pixieDmaOut(&color, colourType_);
                for (int i=0; i<8; i++)
                {
                    plot(j+i, (int)graphicsMode_-cdp1861Configuration_.pixieGraphics.start,(v & 128) ? 1 : 0, (color|colourMask_)&7);
                    if (graphicsX_ == 16)
                        plot(j+i, (int)graphicsMode_-cdp1861Configuration_.pixieGraphics.start+1,(v & 128) ? 1 : 0, (color|colourMask_)&7);
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

void Pixie::dmaEnable()
{
    Byte v;
    int color;

    for(int j=0; j<128; j+=8)
    {
        v = p_Computer->pixieDmaOut(&color, colourType_);
        for (int i=0; i<8; i++)
        {
            plot(j+i, (int)graphicsMode_ - cdp1861Configuration_.pixieGraphics.start,(v & 128) ? 1 : 0, (color|colourMask_)&7);
            v <<= 1;
        }
    }
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
        if (graphicsMode_ >= cdp1861Configuration_.pixieGraphics.screenend)
        {
            if (changeScreenSize_)
            {
                changeScreenSize();
                changeScreenSize_ = false;
            }
            graphicsMode_ = 0;
            copyScreen();
            videoSyncCount_++;
        }
    }
    if (graphicsNext_ == 2)
    {
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.interrupt)
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
    if (graphicsMode_ >= cdp1861Configuration_.pixieGraphics.start && graphicsMode_ <=cdp1861Configuration_.pixieGraphics.end && graphicsOn_ && vidInt_ == 1 && graphicsNext_ >=4 && graphicsNext_ < 12)
    {
        j = 0;
        while(graphicsNext_ >= 4 && graphicsNext_ < 12)
        {
            graphicsNext_ ++;
            v = p_Computer->pixieDmaOut(&color, colourType_);
            for (int i=0; i<8; i++)
            {
                plot(j+i, (int)graphicsMode_-cdp1861Configuration_.pixieGraphics.start,(v & 128) ? 1 : 0, (color|colourMask_)&7);
                plot(j+i, (int)graphicsMode_-cdp1861Configuration_.pixieGraphics.start+1,(v & 128) ? 1 : 0, (color|colourMask_)&7);
                plot(j+i, (int)graphicsMode_-cdp1861Configuration_.pixieGraphics.start+2,(v & 128) ? 1 : 0, (color|colourMask_)&7);
                plot(j+i, (int)graphicsMode_-cdp1861Configuration_.pixieGraphics.start+3,(v & 128) ? 1 : 0, (color|colourMask_)&7);
                v <<= 1;
            }
            j += 8;
        }
        graphicsMode_+=3;
        p_Computer->setCycle0();
        graphicsNext_ -= 1;
    }
    graphicsNext_ += 1;
    if (graphicsNext_ > 13)
        graphicsNext_ = 0;
}

void Pixie::cyclePixieCdp1864()
{
    int j;
    Byte v;
    int color;

    if (graphicsNext_ == 0)
    {
        p_Computer->debugTrace("----  H.Sync");
        graphicsMode_++;
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.interrupt-2) pixieEf_ = 0;
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.start) pixieEf_ = 1;
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.start+videoHeight_-4) pixieEf_ = 0;
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.start+videoHeight_) pixieEf_ = 1;
        if (graphicsMode_ >= cdp1861Configuration_.pixieGraphics.screenend)
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
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.interrupt)
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
    if (graphicsNext_ == 4)
         drawBackgroundLine();
    if (graphicsMode_ >= cdp1861Configuration_.pixieGraphics.start && graphicsMode_ <=cdp1861Configuration_.pixieGraphics.end && graphicsOn_ && vidInt_ == 1 && graphicsNext_ >=4 && graphicsNext_ <=11)
    {
        j = 0;
        while(graphicsNext_ >= 4 && graphicsNext_ <= 11)
        {
            graphicsNext_ ++;
            v = p_Computer->pixieDmaOut(&color, colourType_);
            for (int i=0; i<8; i++)
            {
                plot(j+i, (int)graphicsMode_-cdp1861Configuration_.pixieGraphics.start, (v & 128) ? 1 : 0, (color|colourMask_)&7);
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
    if (p_Main->isZoomEventOngoing())
        return;

//    if (!graphicsOn_) // TO BE CHECKED - for ETI
//        return;

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

#if defined(__WXMAC__)
    if (reBlit_ || reDraw_)
    {
        p_Main->eventRefreshVideo(false, videoNumber_);
        reBlit_ = false;
        reDraw_ = false;
    }
#else
    if (extraBackGround_ && newBackGround_)
        drawExtraBackground(colour_[colourIndex_+backGround_]);

    PlotList *temp;

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
            videoScreenPointer->blit(offsetX_+ plotListPointer->x, offsetY_+plotListPointer->y, plotListPointer->width, plotListPointer->height, &dcMemory, offsetX_+plotListPointer->x, offsetY_+plotListPointer->y);
            temp = plotListPointer;
            plotListPointer = temp->nextPlot;
            delete temp;
        }
    }
#endif
}

void Pixie::drawScreen()
{
    int v;
    int color;

    if (offsetY_ > cdp1861Configuration_.pixieGraphics.start)
    {
        setColour(colourIndex_+bgColor_[cdp1861Configuration_.pixieGraphics.screenend-1]);
        drawRectangle(0, 0, videoWidth_+2*offsetX_, offsetY_ - cdp1861Configuration_.pixieGraphics.start);
        drawRectangle(0, cdp1861Configuration_.pixieGraphics.screenend, videoWidth_+2*offsetX_, cdp1861Configuration_.pixieGraphics.screenend + cdp1861Configuration_.pixieGraphics.start - offsetY_);
    }

    for (int graphicsMode = 0; graphicsMode < cdp1861Configuration_.pixieGraphics.screenend; graphicsMode++)
    {
        setColour(colourIndex_+bgColor_[graphicsMode]);
        drawRectangle(0, graphicsMode - cdp1861Configuration_.pixieGraphics.start + offsetY_, videoWidth_ + offsetX_ * 2, 1);
    }

    for (int x=0; x<videoWidth_; x++)
    {
        for (int y=0; y<videoHeight_; y++)
        {
            v = pbacking_[x][y];
            color = color_[x][y];
            if (v)
            {
                setColour(colourIndex_+color);
                drawPoint(x+offsetX_, y+offsetY_);
            }
        }
    }
}

void Pixie::drawBackgroundLine()
{
    wxPen penClr;

    bgChanged = (backGround_ != bgColor_[graphicsMode_]);
    if (!bgChanged)
        return;
    bgColor_[graphicsMode_] = backGround_;

    setColour(colourIndex_+backGround_);
    penClr = penColour_[colourIndex_+backGround_];

    int y = (int)graphicsMode_ - cdp1861Configuration_.pixieGraphics.start + offsetY_;
    if (y >= 0)
        drawRectangle(0, y, videoWidth_ + offsetX_ * 2, 1);

    reBlit_ = true;
}

void Pixie::plot(int x, int y, int c, int color)
{
    wxPen penClr;

    if (!bgChanged)
        if (pbacking_[x][y] == c)
            if (!c || (color_[x][y] == color))  
                return;

    color_[x][y] = color;
    pbacking_[x][y] = c;

    if (c)
    {
        setColour(colourIndex_+color);
#if defined(__WXMAC__) || defined(__linux__)
        penClr = penColour_[color];
#endif
    }
    else
    {
        setColour(colourIndex_+backGround_);
#if defined(__WXMAC__) || defined(__linux__)
        penClr = penColour_[backGround_+colourIndex_];
#endif
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
        temp->width = 1;
        temp->height = 1;
        temp->penClr = penClr;
        temp->nextPlot = plotListPointer;
        plotListPointer = temp;
        updatePlot_++;
        if (updatePlot_ > 40)
            reBlit_ = true;
    }
#endif
}

void Pixie::plot(int x, int y, int width, int height, int c, int color)
{
    wxPen penClr;
   
    if (pbacking_[x][y] == c)
        if (!c || (color_[x][y] == color))  return;
    
    color_[x][y] = color;
    pbacking_[x][y] = c;

    if (c)
    {
        setColour(colourIndex_+color);
        penClr = penColour_[color];
    }
    else
    {
        setColour(colourIndex_+backGround_);
        penClr = penColour_[backGround_+colourIndex_];
    }
    drawRectangle(x+offsetX_, y+offsetY_, width, height);
    
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
        temp->width = width;
        temp->height = height;
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
    p_Main->eventVideoSetFullScreen(fullScreenSet_, videoNumber_);
}

void Pixie::pixieBarSize()
{
    if (cdp1861Configuration_.statusBarType == STATUSBAR_VIP2)
        vipIIStatusBarPointer->reDrawBar();
}

void Pixie::reDrawBar()
{
    if (cdp1861Configuration_.statusBarType == STATUSBAR_VIP2)
        vipIIStatusBarPointer->reDrawBar();
}

void Pixie::updateVipLedStatus(bool status, int led)
{
    if (cdp1861Configuration_.statusBarType == STATUSBAR_VIP2)
        vipIIStatusBarPointer->updateLedStatus(status, led);
}

void Pixie::setInterlace(bool status)
{
    interlace_ = status;
    reDraw_ = true;
}

void Pixie::reBlit(wxDC &dc)
{
    if (!memoryDCvalid_)
        return;
    
    dc.Blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);
    
    if (extraBackGround_ && newBackGround_)
    {
        wxSize size = wxGetDisplaySize();

        dc.SetBrush(wxBrush(colour_[colourIndex_+0]));
        dc.SetPen(wxPen(colour_[colourIndex_+0]));

        int xStart = (int)((2*offsetX_+videoWidth_)*zoom_*xZoomFactor_);
        dc.DrawRectangle(xStart, 0, size.x-xStart, size.y);

        int yStart = (int)((2*offsetY_+videoHeight_)*zoom_);
        dc.DrawRectangle(0, yStart, size.x, size.y-yStart);

        newBackGround_ = false;
    }
    if (!graphicsOn_)
    {
        dc.SetUserScale(zoom_*xZoomFactor_, zoom_);
        dc.SetBrush(wxBrush(colour_[colourIndex_+backGround_]));
        dc.SetPen(wxPen(colour_[colourIndex_+backGround_]));
        dc.DrawRectangle(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
    }
}

PixieFred::PixieFred(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int videoNumber, int videoType, CDP1861Configuration cdp1861Configuration, CDP1862Configuration cdp1862Configuration, int bootStrapType)
: Pixie(title, pos, size, zoom, zoomfactor, videoNumber, videoType, cdp1861Configuration, cdp1862Configuration, bootStrapType)
{
    displayType_ = 3;
    setDisplayType(displayType_);
    colourIndex_ = COL_PIXIE_FORE;
    videoNumber_ = videoNumber;
}

void PixieFred::configureFredVideo(FredVideoConfiguration fredVideoConfiguration)
{
    p_Main->configureMessage(&fredVideoConfiguration.ioGroupVector, "FRED Video");
    p_Computer->setOutType(&fredVideoConfiguration.ioGroupVector, fredVideoConfiguration.output, "bit 0-2 background colour, bit 3 white foreground, bit 4-5 enable graphics, bit 6 PAL/NTSC");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_FRED, FRED_VIDEO_CYCLE);

    p_Main->message("");

    backGroundInit_ = 1;
    colourMask_ = 0;
}

void PixieFred::drawScreen()
{
    int v;
    int color;
    
    setColour(colourIndex_+backGround_);
    drawRectangle(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
    
    for (int x=0; x<videoWidth_; x+=xInterlace_)
    {
        for (int y=0; y<videoHeight_; y+=yInterlace_)
        {
            v = pbacking_[x][y];
            color = color_[x][y];
            if (v)
            {
                setColour(colourIndex_+color);
                if (interlace_)
                    drawRectangle(x+offsetX_, y+offsetY_, xInterlace_, yInterlace_);
                else
                    drawRectangle(x+offsetX_, y+offsetY_, xNonInterlace_, yNonInterlace_);
            }
        }
    }
}

void PixieFred::setDisplayType(int displayType)
{
    displayType_ = displayType;
    if ((displayType & 0x2) == 0)
    {
        xInterlace_ = 6;
        xNonInterlace_ = 3;
        if ((displayType & 0x1) == 0)
        {
            yInterlace_ = 8;
            yNonInterlace_ = 4;
        }
        else
        {
            yInterlace_ = 4;
            yNonInterlace_ = 2;
        }
    }
    else
    {
        xInterlace_ = 3;
        xNonInterlace_ = 2;
        if ((displayType & 0x1) == 0)
        {
            yInterlace_ = 8;
            yNonInterlace_ = 4;
        }
        else
        {
            yInterlace_ = 4;
            yNonInterlace_ = 2;
        }
    }
}

void PixieFred::cyclePixie()
{
    int j;
    Byte v;
    int color;

    if (graphicsNext_ == 0)
    {
        p_Computer->debugTrace("----  H.Sync");
        graphicsMode_++;
        if (graphicsMode_ >= 64)
        {
            if (changeScreenSize_)
            {
                changeScreenSize();
                changeScreenSize_ = false;
            }
            graphicsMode_ = 0;
            copyScreen();
            videoSyncCount_++;
        }
        
        if (graphicsMode_ == 1)
        {
            if (graphicsOn_)
            {
                p_Computer->pixieInterrupt();
                vidInt_ = 1;
                p_Computer->setCycle0();
            }
            else vidInt_ = 0;
        }
        
        int numberOfDmaInstructions = 8;
        if ((displayType_ & 0x2) == 0)
            numberOfDmaInstructions = 4;
        
        int lastGraphicsMode = 48;
        if ((displayType_ & 0x1) == 0)
            lastGraphicsMode = 32;
        
        if (graphicsMode_ >= 16 && graphicsMode_ < lastGraphicsMode && graphicsOn_ && vidInt_ == 1)
        {
            j = 0;
            for (int dma=0; dma<numberOfDmaInstructions; dma++)
            {
                graphicsNext_ += 1;
                v = p_Computer->pixieDmaOut(&color, colourType_);
                int line = (int)graphicsMode_-16;
                if (interlace_)
                {
                    for (int i=0; i<(8*xInterlace_); i+=xInterlace_)
                    {
                        plot(j+i, line*yInterlace_, xInterlace_, yInterlace_, (v & 128) ? 1 : 0, (color|colourMask_)&7);
                        v <<= 1;
                    }
                    j += (8*xInterlace_);
                }
                else
                {
                    for (int i=0; i<(8*xInterlace_); i+=xInterlace_)
                    {
                        plot(j+i, line*yInterlace_, xNonInterlace_, yNonInterlace_, (v & 128) ? 1 : 0, (color|colourMask_)&7);
                        v <<= 1;
                    }
                    j += (8*xInterlace_);
                }
            }
            p_Computer->setCycle0();
            graphicsNext_ -= 1;
        }
    }
    graphicsNext_ += 1;
    if (graphicsNext_ > 31)
        graphicsNext_ = 0;
}

PixieVip2K::PixieVip2K(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int videoNumber, int videoType, CDP1861Configuration cdp1861Configuration, CDP1862Configuration cdp1862Configuration, int bootStrapType)
: Pixie(title, pos, size, zoom, zoomfactor, videoNumber, videoType, cdp1861Configuration, cdp1862Configuration, bootStrapType)
{
    colourIndex_ = COL_PIXIE_FORE;
    videoNumber_ = videoNumber;
}

void PixieVip2K::configureVip2K(Vip2KVideoConfiguration vip2KVideoConfiguration)
{
    vip2KVideoConfiguration_ = vip2KVideoConfiguration;
    
    p_Main->configureMessage(&vip2KVideoConfiguration.ioGroupVector, "VIP2K Video");

    p_Computer->setInType(&vip2KVideoConfiguration.ioGroupVector, vip2KVideoConfiguration.input, "enable graphics");
    if (vip2KVideoConfiguration.doubleScreenIo)
        p_Computer->setOutType(&vip2KVideoConfiguration.ioGroupVector, vip2KVideoConfiguration.input, "enable graphics");
    p_Computer->setOutType(&vip2KVideoConfiguration.ioGroupVector, vip2KVideoConfiguration.output, "disable graphics");
    if (vip2KVideoConfiguration.doubleScreenIo)
        p_Computer->setInType(&vip2KVideoConfiguration.ioGroupVector, vip2KVideoConfiguration.output, "disable graphics");
    p_Computer->setEfType(&vip2KVideoConfiguration.ioGroupVector, vip2KVideoConfiguration.ef, "in frame indicator");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_VIP2K, VIP2K_VIDEO_CYCLE);

    p_Main->message("");

    backGroundInit_ = 1;
    colourMask_ = 0;

    sequencerAddress_ = 0;
    scanLine_ = 0;
    totalNumberOfLines_ = 0;
    sizeTopBox_ = 0;
    sizeBottomBox1_ = 0;
    sizeBottomBox2_ = 0;
    additionalTopPalLines_ = 0;
    additionalBottomPalLines_ = 0;
    viewableLines_ = 0;
    scanByte_ = 0;
}

void PixieVip2K::drawScreen()
{
    int v;
    int color;
    
    setColour(colourIndex_+backGround_);
    drawRectangle(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
    
    int videoHeight = videoHeight_ - additionalBottomPalLines_;
    
    for (int x=0; x<videoWidth_; x++)
    {
        for (int y=additionalTopPalLines_; y<videoHeight; y++)
        {
            v = pbacking_[x][y];
            color = color_[x][y];
            if (v)
            {
                setColour(colourIndex_+color);
                drawPoint(x+offsetX_, y+offsetY_);
            }
        }
    }
}

void PixieVip2K::cyclePixie()
{
    int continue_loop = true;
    
    while (continue_loop)
    {
        executeSequencer(p_Computer->readSequencer(sequencerAddress_));
        scanByte_++;
        
        if ((scanByte_&1) == 0)
            sequencerAddress_++;
        
        if (scanByte_ == 30)
            scanLine_++;
        
        if (scanLine_ > totalNumberOfLines_)
            totalNumberOfLines_ = scanLine_;
        
        if (scanByte_ >= 32)
            scanByte_ = 0;
        
        if ((p_Computer->readSequencer(sequencerAddress_) & 0x2) != 0)
            continue_loop = false;
    }
}

void PixieVip2K::executeSequencer(Byte sequencerValue)
{
    Byte v;
    int x;
        
    if ((sequencerValue & 0x2) == 0)
    {
        v = p_Computer->pixieDmaOut();
        
        if (scanByte_ == 0)
            viewableLines_++;
        
        if (scanLine_ >= 66 && scanLine_ < (videoHeight_+66))
        {
            switch (scanByte_)
            {
                case 30:
                    v <<= 5;
                    for (int i=0; i<3; i++)
                    {
                        plot(i, scanLine_-66+additionalTopPalLines_,(v & 128) ? 1 : 0, 0);
                        v <<= 1;
                    }
                    break;
                    
                case 31:
                    for (int i=3; i<11; i++)
                    {
                        plot(i, scanLine_-66+additionalTopPalLines_,(v & 128) ? 1 : 0, 0);
                        v <<= 1;
                    }
                    break;
                    
                case 23:
                    for (int i=195; i<198; i++)
                    {
                        plot(i, scanLine_-66+additionalTopPalLines_,(v & 128) ? 1 : 0, 0);
                        v <<= 1;
                    }
                    break;
                    
                default:
                    x = scanByte_*8 + 11;
                    for (int i=0; i<8; i++)
                    {
                        plot(x+i, scanLine_-66+additionalTopPalLines_,(v & 128) ? 1 : 0, 0);
                        v <<= 1;
                    }
                    break;
            }
        }
    }
    
    if (sequencerValue & 0x80)
        sequencerAddress_ |= 0x2000;
    else
    {
        sequencerAddress_ &= 0x5fff;
        if ((sequencerValue & 0x10) == 0 && scanByte_ == 30)
        {
            if (scanLine_ > sizeBottomBox2_)
                sizeBottomBox2_ = scanLine_;
        }
    }
    
    if (sequencerValue & 0x40)
    {
        sequencerAddress_ &= 0x4000;
        viewableLines_+=5;
        
        if ((scanLine_ - sizeBottomBox2_ - sizeTopBox_ - 192) > sizeBottomBox1_)
        {
            sizeBottomBox1_ = scanLine_ - sizeBottomBox2_ - sizeTopBox_ - 192;
            
            additionalTopPalLines_ = sizeTopBox_ - 28;
            if (additionalTopPalLines_ < 0)
                additionalTopPalLines_ = 0;
            
            additionalBottomPalLines_ = sizeBottomBox1_ + sizeBottomBox2_ - 50;
            if (additionalBottomPalLines_ < 0)
                additionalBottomPalLines_ = 0;
        }
        
        if (videoHeight_ != viewableLines_ && viewableLines_ > 100)
        {
            videoHeight_ = viewableLines_ + additionalTopPalLines_ + additionalBottomPalLines_;
            
            p_Main->eventSetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_*xZoomFactor_, (videoHeight_+2*borderY_[videoType_])*zoom_, DON_T_CALL_CHANGE_SCREEN_SIZE, false, videoNumber_);
            changeScreenSize_ = true;
        }
        
        if (changeScreenSize_)
        {
            changeScreenSize();
            changeScreenSize_ = false;
        }
        
        scanLine_ = 0;
        viewableLines_ = 0;
        copyScreen();
        videoSyncCount_++;
    }
    
    pixieEf_ = (sequencerValue & 0x20) >> 5;
    
    if ((sequencerValue & 0x4) == 0)
    {
        if (vidInt_ == 0)
        {
            if ((scanLine_ - sizeBottomBox2_) > sizeTopBox_)
                sizeTopBox_ = scanLine_ - sizeBottomBox2_;
            
            p_Computer->pixieInterrupt();
            vidInt_ = 1;
            p_Computer->setCycle0();
        }
    }
    else
        vidInt_ = 0;
}

Byte PixieVip2K::efPixie()
{
    return pixieEf_^vip2KVideoConfiguration_.ef.reverse;
}

Byte PixieVip2K::inPixie()
{
    graphicsOn_ = true;
    reBlit_ = true;
    sequencerAddress_ |= 0x4000;
    return 0;
}

void PixieVip2K::outPixie()
{
    graphicsOn_ = false;
#if defined(__WXMAC__)
    p_Main->eventRefreshVideo(false, videoNumber_);
#else
    videoScreenPointer->disableScreen(colour_[colourIndex_+backGround_], videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
#endif
    sequencerAddress_ &= 0x3fff;
}

PixieStudioIV::PixieStudioIV(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int videoNumber, int videoType, CDP1861Configuration cdp1861Configuration, CDP1862Configuration cdp1862Configuration, int bootStrapType)
: Pixie(title, pos, size, zoom, zoomfactor, videoNumber, videoType, cdp1861Configuration, cdp1862Configuration, bootStrapType)
{
}

void PixieStudioIV::configureSt4(Studio4VideoConfiguration studio4VideoConfiguration)
{
    studio4VideoConfiguration_ = studio4VideoConfiguration;
    
    p_Main->configureMessage(&studio4VideoConfiguration.ioGroupVector, "RCA Studio IV Video Chip");
    p_Computer->setOutType(&studio4VideoConfiguration.ioGroupVector, studio4VideoConfiguration.output, "bit 0-2 background colour, bit 3 white foreground, bit 4-5 enable graphics, bit 6 PAL/NTSC");
    p_Computer->setOutType(&studio4VideoConfiguration.ioGroupVector, studio4VideoConfiguration.dmaEnable, "enable DMA");
    p_Computer->setEfType(&studio4VideoConfiguration.ioGroupVector, studio4VideoConfiguration.ef, "in frame indicator");
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_ST4, STUDIOIV_VIDEO_CYCLE);

    p_Main->message("");

    colourMask_ = 0;
    colourType_ = PIXIE_COLOR_STUDIOIV;
}

void PixieStudioIV::cyclePixie()
{
    studioIVFactor_ = !studioIVFactor_;
    if (studioIVFactor_ || !graphicsOn_)
        return;
    
    if (graphicsNext_ == 0)
    {
        p_Computer->debugTrace("----  H.Sync");
        graphicsMode_++;
        if (graphicsMode_ >= cdp1861Configuration_.pixieGraphics.screenend)
        {
            if (changeScreenSize_)
            {
                changeScreenSize();
                changeScreenSize_ = false;
            }
            graphicsMode_ = 0;
            copyScreen();
            videoSyncCount_++;
        }
    }
    
    if (graphicsNext_ == 18)
    {
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.interrupt)
        {
            p_Computer->pixieInterrupt();
            p_Computer->setCycle0();
        }
    }
    if (graphicsMode_ >= cdp1861Configuration_.pixieGraphics.start && graphicsMode_ <=cdp1861Configuration_.pixieGraphics.end && graphicsNext_ >=4 && graphicsNext_ <= 19)
    {
        graphicsNext_ = 19;
        p_Computer->setCycle0();
    }
    graphicsNext_ += 1;
    if (graphicsNext_ > 21)
        graphicsNext_ = 0;
}

void PixieStudioIV::drawScreen()
{
    int v;
    int color;

    setColour(colourIndex_+backGround_);
    drawRectangle(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);

    for (int x=0; x<videoWidth_; x++)
    {
        for (int y=0; y<videoHeight_; y++)
        {
            v = pbacking_[x][y];
            color = color_[x][y];
            if (v)
            {
                setColour(colourIndex_+color);
                drawPoint(x+offsetX_, y+offsetY_);
            }
        }
    }
}

Byte PixieStudioIV::efPixie()
{
    return pixieEf_^studio4VideoConfiguration_.ef.reverse;
}

void PixieStudioIV::outPixie()
{
    graphicsOn_ = false;
#if defined(__WXMAC__)
    p_Main->eventRefreshVideo(false, videoNumber_);
#else
    videoScreenPointer->disableScreen(colour_[colourIndex_+backGround_], videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
#endif
}

void PixieStudioIV::outPixieBackGround()
{
    graphicsOn_ = true;
    backGround_++;
    if (backGround_ == 12)  backGround_ = 8;
    newBackGround_ = true;
    reBlit_ = true;
    drawScreen();
}

void PixieStudioIV::outPixieStudioIV(int value)
{
    int newBackground = (value &0x7) + 8;
    
    if (newBackground != backGround_)
    {
        backGround_ = newBackground;
        newBackGround_ = true;
        reBlit_ = true;
        drawScreen();
    }
    
    if ((value & 0x8) == 0x8)
    {
        if (colourMask_ != 7)
        {
            reBlit_ = true;
            drawScreen();

        }
        colourMask_ = 7;
    }
    else
    {
        if (colourMask_ != 0)
        {
            reBlit_ = true;
            drawScreen();
        }
        colourMask_ = 0;
    }
    
    if ((value & 0x30) == 0x30)
        inPixie();
    else
        outPixie();
    
//           if ((value&0x40) == 0x40)
//               switchVideoMode(PAL);
//           else
//               switchVideoMode(NTSC);
}

/*
void PixieStudioIV::switchVideoMode(int videoMode)
{
    if (videoMode == p_videoMode_)
        return;
    
    p_videoMode_ = videoMode;
    
    if (p_videoMode_ == PAL)
    {
        cdp1861Configuration_.pixieGraphics.interrupt = 74;
        cdp1861Configuration_.pixieGraphics.start = 76;
        cdp1861Configuration_.pixieGraphics.end = 267;
        cdp1861Configuration_.pixieGraphics.screenend = 312;
        videoHeight_ = 192;
    }
    else
    {
        cdp1861Configuration_.pixieGraphics.interrupt = 62;
        cdp1861Configuration_.pixieGraphics.start = 64;
        cdp1861Configuration_.pixieGraphics.end = 191;
        cdp1861Configuration_.pixieGraphics.screenend = 262;
        videoHeight_ = 128;
    }
    p_Main->eventSetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_*xZoomFactor_, (videoHeight_+2*borderY_[videoType_])*zoom_, DON_T_CALL_CHANGE_SCREEN_SIZE, false, videoNumber_);
}*/

PixieEti::PixieEti(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int videoNumber, int videoType, CDP1861Configuration cdp1861Configuration, CDP1862Configuration cdp1862Configuration, int bootStrapType)
: Pixie(title, pos, size, zoom, zoomfactor, videoNumber, videoType, cdp1861Configuration, cdp1862Configuration, bootStrapType)
{
}

void PixieEti::copyScreen()
{
    if (p_Main->isZoomEventOngoing())
        return;

    if (!graphicsOn_)
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

#if defined(__WXMAC__)
    if (reBlit_ || reDraw_)
    {
        p_Main->eventRefreshVideo(false, videoNumber_);
        reBlit_ = false;
        reDraw_ = false;
    }
#else
    if (extraBackGround_ && newBackGround_)
        drawExtraBackground(colour_[colourIndex_+backGround_]);

    PlotList *temp;

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
            videoScreenPointer->blit(offsetX_+ plotListPointer->x, offsetY_+plotListPointer->y, plotListPointer->width, plotListPointer->height, &dcMemory, offsetX_+plotListPointer->x, offsetY_+plotListPointer->y);
            temp = plotListPointer;
            plotListPointer = temp->nextPlot;
            delete temp;
        }
    }
#endif
}

void PixieEti::drawScreen()
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
                setColour(colourIndex_+color);
                drawPoint(x+offsetX_, y+offsetY_);
            }
        }
    }
}

void PixieEti::cyclePixie()
{
    int j;
    Byte v;
    int color;

    if (graphicsNext_ == 0)
    {
        p_Computer->debugTrace("----  H.Sync");
        graphicsMode_++;
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.interrupt-2) pixieEf_ = 0;
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.start) pixieEf_ = 1;
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.start+videoHeight_-4) pixieEf_ = 0;
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.start+videoHeight_) pixieEf_ = 1;
        if (graphicsMode_ >= cdp1861Configuration_.pixieGraphics.screenend)
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
        if (graphicsMode_ == cdp1861Configuration_.pixieGraphics.interrupt)
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
    if (graphicsMode_ >= cdp1861Configuration_.pixieGraphics.start && graphicsMode_ <=cdp1861Configuration_.pixieGraphics.end && graphicsOn_ && vidInt_ == 1 && graphicsNext_ >=4 && graphicsNext_ <=11)
    {
        j = 0;
        while(graphicsNext_ >= 4 && graphicsNext_ <= 11)
        {
            graphicsNext_ ++;
            v = p_Computer->pixieDmaOut(&color, colourType_);
            for (int i=0; i<8; i++)
            {
                plot(j+i, (int)graphicsMode_-cdp1861Configuration_.pixieGraphics.start, (v & 128) ? 1 : 0, (color|colourMask_)&7);
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

void PixieEti::outPixie()
{
    graphicsOn_ = false;
#if defined(__WXMAC__)
    p_Main->eventRefreshVideo(false, videoNumber_);
#else
    videoScreenPointer->disableScreen(colour_[colourIndex_+backGround_], videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
#endif
}

void PixieEti::outPixieBackGround()
{
    graphicsOn_ = true;
    backGround_++;
    if (backGround_ == 12)  backGround_ = 8;
    newBackGround_ = true;
    reBlit_ = true;
    drawScreen();
}


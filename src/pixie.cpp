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

Pixie::Pixie(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, int videoNumber)
: Video(title, pos, size)
{
    computerType_ = computerType;
    colourIndex_ = 0;
    videoNumber_ = videoNumber;
    
    if (computerType_ == XML)
    {
        videoType_ = VIDEOXMLPIXIE;
        colourIndex_ = COL_PIXIE_FORE;
    }
    else
    {
        videoType_ = VIDEOPIXIE;
    }
    
    runningComputer_ = p_Main->getRunningComputerStr();
    interlace_ = p_Main->getInterlace(computerType_);

    if ((computerType_ == TMC2000) || (computerType_ == NANO) || (computerType_ == COSMICOS) || (computerType_ == ETI) || (computerType_ == VICTORY) || (computerType_ == VIP2K))
        videoHeight_ = 192;
    else
        videoHeight_ = 128;

    pixieGraphics_.interrupt = 74;
    pixieGraphics_.start = 76;
    pixieGraphics_.end = 267;
    pixieGraphics_.screenend = 312;
    videoWidth_ = 64;
    
    switch (computerType_)
    {
        case VICTORY:
            videoHeight_ = 192;
            videoMode_ = p_Main->getStudioVideoMode(computerType_);
            
            if (videoMode_ != PAL)
            {
                pixieGraphics_.interrupt = 62;
                pixieGraphics_.start = 64;
                pixieGraphics_.end = 191;
                pixieGraphics_.screenend = 262;
                videoHeight_ = 128;
            }
        break;

        case STUDIOIV:
            videoMode_ = p_Main->getStudioVideoMode(computerType_);
            
            if (videoMode_ == PAL)
            {
                pixieGraphics_.interrupt = 32;
                pixieGraphics_.start = 34;
                pixieGraphics_.end = 225;
                pixieGraphics_.screenend = 262;

                videoHeight_ = 192;
            }
            else
            {
                pixieGraphics_.interrupt = 38;
                pixieGraphics_.start = 40;
                pixieGraphics_.end = 167;
                pixieGraphics_.screenend = 208;
                
                videoHeight_ = 128;
            }
            studioIVFactor_ = true;
        break;
            
        case ELF:
        case ELFII:
        case ELF2K:
        case PICO:
        case STUDIO:
        case SUPERELF:
        case TMC1800:
        case VELF:
        case VIP:
        case VIPII:
        case VISICOM:
            pixieGraphics_.interrupt = 62;
            pixieGraphics_.start = 64;
            pixieGraphics_.end = 191;
            pixieGraphics_.screenend = 262;
            videoHeight_ = 128;
        break;
            
        case XML:
            pixieGraphics_ = p_Main->getPixieGraphics(XML);
            videoHeight_ = p_Main->getVideoHeight(XML);
            videoWidth_ = p_Main->getVideoWidth(XML);
        break;
    }
    
    if ( (computerType == VIP && p_Main->getVipHighRes()) || (computerType_ == STUDIOIV))
    {
        highRes_ = 2;
        xZoomFactor_ = zoomfactor/highRes_;
    }
    else
    {
        highRes_ = 1;
        xZoomFactor_ = zoomfactor;
    }
    
    if (!p_Main->isFullScreenFloat())
        xZoomFactor_ = (int) xZoomFactor_;
    
    graphicsX_ = 8*highRes_;
    videoWidth_ = videoWidth_*highRes_;
    if (computerType == FRED1 || computerType == FRED1_5)
        videoWidth_ = videoWidth_*highRes_*3;

    if (computerType == VIP2K)
    {
        videoHeight_ = 200;
        videoWidth_ = 198;
        studioIVFactor_ = true;
    }
    
    backGroundInit_ = 1;
    backGround_ = 1;

    this->SetClientSize(size);

    videoScreenPointer = new VideoScreen(this, size, zoom, computerType, videoNumber_, xZoomFactor_);

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

Pixie::Pixie(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, int videoNumber, int videoType)
: Video(title, pos, size)
{
    computerType_ = computerType;
    colourIndex_ = 0;
    videoNumber_ = videoNumber;
    
    videoType_ = videoType;
    colourIndex_ = COL_PIXIE_FORE;
    
    runningComputer_ = p_Main->getRunningComputerStr();
    interlace_ = p_Main->getInterlace(computerType_);

    videoHeight_ = 128;

    pixieGraphics_.interrupt = 74;
    pixieGraphics_.start = 76;
    pixieGraphics_.end = 267;
    pixieGraphics_.screenend = 312;
    
    pixieGraphics_ = p_Main->getPixieGraphics(XML);
    videoHeight_ = p_Main->getVideoHeight(XML);
    videoWidth_ = p_Main->getVideoWidth(XML);
    
    if ( (computerType == VIP && p_Main->getVipHighRes()) || (computerType_ == STUDIOIV))
    {
        highRes_ = 2;
        xZoomFactor_ = zoomfactor/highRes_;
    }
    else
    {
        highRes_ = 1;
        xZoomFactor_ = zoomfactor;
    }
    
    if (!p_Main->isFullScreenFloat())
        xZoomFactor_ = (int) xZoomFactor_;
    
    graphicsX_ = 8*highRes_;
    videoWidth_ = videoWidth_*highRes_;
    if (computerType == FRED1 || computerType == FRED1_5)
        videoWidth_ = videoWidth_*highRes_*3;

    backGroundInit_ = 1;
    backGround_ = 1;

    this->SetClientSize(size);

    videoScreenPointer = new VideoScreen(this, size, zoom, computerType, videoNumber_, xZoomFactor_);

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
#if defined(__WXMAC__)
    p_Main->eventRefreshVideo(false, videoNumber_);
#else
    videoScreenPointer->disableScreen(colour_[colourIndex_+backGround_], videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
#endif
}

void Pixie::configurePixie(IoConfiguration portConf)
{
    p_Computer->setOutType(portConf.pixieOutput, PIXIEOUT);
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setInType(portConf.pixieInput, PIXIEIN);
    p_Computer->setEfType(portConf.pixieEf, PIXIEEF);

    backGroundInit_ = 1;
    colourMask_ = 0;

    wxString printBuffer;
    p_Main->message("Configuring CDP 1861");

    printBuffer.Printf("    Output %d: disable graphics, input %d: enable graphics, EF %d: in frame indicator\n", portConf.pixieOutput, portConf.pixieInput, portConf.pixieEf);
    p_Main->message(printBuffer);
}

void Pixie::configurePixieSuper(IoConfiguration portConf)
{
    p_Computer->setOutType(portConf.pixieOutput, PIXIEOUT);
    p_Computer->setInType(portConf.pixieOutput, PIXIEOUT);
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setOutType(portConf.pixieInput, PIXIEIN);
    p_Computer->setInType(portConf.pixieInput, PIXIEIN); 
    p_Computer->setEfType(portConf.pixieEf, PIXIEEF);

    backGroundInit_ = 1;
    colourMask_ = 0;

    wxString printBuffer;
    p_Main->message("Configuring CDP 1861");

    printBuffer.Printf("    Output/input %d: disable graphics, Output/input %d: enable graphics, EF %d: in frame indicator\n", portConf.pixieOutput, portConf.pixieInput, portConf.pixieEf);
    p_Main->message(printBuffer);
}

void Pixie::configurePixieStudio2()
{
    p_Computer->setOutType(1, PIXIEOUT);
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setInType(1, PIXIEIN);
    p_Computer->setEfType(1, PIXIEEF);

    backGroundInit_ = 1;
    colourMask_ = 0;

    p_Main->message("Configuring CDP 1861");
    p_Main->message("    Output 1: disable graphics, input 1: enable graphics, EF 1: in frame indicator\n");
}

void Pixie::configurePixieCoinArcade()
{
    p_Computer->setOutType(2, PIXIEOUT);
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setEfType(2, PIXIEEF);
  
    backGroundInit_ = 1;
    colourMask_ = 0;
    
    p_Main->message("    Output 1: enable graphics\n");
}

void Pixie::configurePixieVisicom()
{
    p_Computer->setOutType(1, PIXIEOUT);
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setEfType(1, PIXIEEF);

    backGroundInit_ = 0;
    colourMask_ = 0; 

    p_Main->message("Configuring CDP 1861");
    p_Main->message("    Output 1: enable graphics, EF 1: in frame indicator\n");
}

void Pixie::configurePixieVip()
{
    p_Computer->setOutType(1, PIXIEOUT);
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setInType(1, PIXIEIN);
    p_Computer->setEfType(1, PIXIEEF);

    if (p_Main->getVipVp590())
    {
        backGroundInit_ = 8;
        colourMask_ = 0;
        p_Computer->setOutType(5, VIPOUT5);

        p_Main->message("Configuring CDP 1861/1862");
        p_Main->message("    Output 1: disable graphics, input 1: enable graphics, EF 1: in frame indicator");
        p_Main->message("    Output 5: step background colour\n");
    }
    else
    {
        backGroundInit_ = 9;
        colourMask_ = 7;

        p_Main->message("Configuring CDP 1861");
        p_Main->message("    Output 1: disable graphics, input 1: enable graphics, EF 1: in frame indicator\n");
    }
}

void Pixie::configurePixieVelf()
{
    p_Computer->setOutType(1, PIXIEOUT);
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setInType(1, PIXIEIN);
    p_Computer->setEfType(1, PIXIEEF);

    backGroundInit_ = 1;
    colourMask_ = 0;

    p_Main->message("Configuring CDP 1861");
    p_Main->message("    Output 1: disable graphics, input 1: enable graphics, EF 1: in frame indicator\n");
}

void Pixie::configurePixieVipII(bool runLed)
{
    p_Computer->setOutType(1, PIXIEOUT);
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setInType(1, PIXIEIN);
    p_Computer->setEfType(1, PIXIEEF); 

    backGroundInit_ = 8;
    colourMask_ = 0; 
    p_Computer->setOutType(5, VIPOUT5);

    p_Main->message("Configuring CDP 1862");
    p_Main->message("    Output 1: disable graphics, input 1: enable graphics, EF 1: in frame indicator");
    p_Main->message("    Output 5: step background colour\n");

    vipIIStatusBarPointer->initVipIIBar(runLed);
}

void Pixie::configurePixieTmc1800()
{
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setInType(1, PIXIEIN);
    p_Computer->setInType(4, PIXIEOUT);
    p_Computer->setEfType(1, PIXIEEF);

    backGroundInit_ = 1;
    colourMask_ = 0;

    p_Main->message("Configuring CDP 1861");
    p_Main->message("    Input 1: enable graphics, input 4: disable graphics");
    p_Main->message("    EF 1: in frame indicator\n");
}

void Pixie::configurePixieTelmac()
{
    p_Computer->setOutType(1, PIXIEBACKGROUND);
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setInType(1, PIXIEIN);
    p_Computer->setInType(4, PIXIEOUT);
    p_Computer->setEfType(1, PIXIEEF);

    backGroundInit_ = 8;
    colourMask_ = 0;

    p_Main->message("Configuring CDP 1864");

    p_Main->message("    Output 1: switch background colour");
    p_Main->message("    Input 1: enable graphics, input 4: disable graphics");
    p_Main->message("    EF 1: in frame indicator\n");
}

void Pixie::configurePixieVictory()
{
    p_Computer->setOutType(1, PIXIEBACKGROUND);
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setInType(1, PIXIEIN);
    p_Computer->setEfType(1, PIXIEEF);
    
    backGroundInit_ = 8;
    colourMask_ = 0;
    
    p_Main->message("Configuring CDP 1864");
    
    p_Main->message("    Output 1: switch background colour, output 4: tone latch");
    p_Main->message("    Input 1: enable graphics");
    p_Main->message("    EF 1: in frame indicator\n");
}

void Pixie::configurePixieNano()
{
    p_Computer->setOutType(1, PIXIEOUT);
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setInType(1, PIXIEIN);
    p_Computer->setInType(4, PIXIEOUT);
    p_Computer->setEfType(1, PIXIEEF);

    backGroundInit_ = 1;
    colourMask_ = 0;

    p_Main->message("Configuring CDP 1864");
    p_Main->message("    Input 1: enable graphics, input 4: disable graphics");
    p_Main->message("    EF 1: in frame indicator\n");

    if (p_Main->getSound(NANO) == 0)
        p_Computer->setSoundFollowQ(false);
    else
        p_Computer->setSoundFollowQ(true);
}

void Pixie::configurePixieCosmicos()
{
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setOutType(2, CDP1864TONE);
    p_Computer->setInType(1, PIXIEIN);
    p_Computer->setInType(2, PIXIEOUT);
    p_Computer->setEfType(3, COSMICOSREQ);

    backGroundInit_ = 1;
    colourMask_ = 0;

    p_Main->message("Configuring CDP 1864");
    p_Main->message("    Q=0: Input 1: enable graphics, input 2: disable graphics");
    p_Main->message("    Q=1: Output 2: tone latch");
    p_Main->message("    EF 3: in frame indicator (when graphics enabled)\n");
}

void Pixie::configureCdp1864(IoConfiguration portConf)
{
    wxString ioGroup = "", printBuffer = "";

    int ioGroupNum = portConf.cdp1864IoGroup + 1;

    p_Computer->setCycleType(VIDEOCYCLE_CDP1864, CDP1864CYCLE);
    
    if (portConf.cdp1864IoGroup != -1)
        ioGroup.Printf(" on group %d", portConf.cdp1864IoGroup);
    
    p_Main->message("Configuring CDP 1864" + ioGroup);

    if (portConf.cdp1864enable.portNumber != -1)
    {
        if (portConf.cdp1864enable.qValue == -1)
            printBuffer.Printf("    Input %d: enable graphics", portConf.cdp1864enable.portNumber);
        else
            printBuffer.Printf("    Q = %d & input %d: enable graphics", portConf.cdp1864enable.qValue,  portConf.cdp1864enable.portNumber);
        p_Main->message(printBuffer);
            
        p_Computer->setInType(portConf.cdp1864enable.qValue, ioGroupNum, portConf.cdp1864enable.portNumber, CDP1864ENABLE);
    }
    if (portConf.cdp1864disable.portNumber != -1)
    {
        if (portConf.cdp1864disable.qValue == -1)
            printBuffer.Printf("    Input %d: disable graphics", portConf.cdp1864disable.portNumber);
        else
            printBuffer.Printf("    Q = %d & input %d: disable graphics", portConf.cdp1864disable.qValue,  portConf.cdp1864disable.portNumber);
        p_Main->message(printBuffer);

        p_Computer->setInType(portConf.cdp1864disable.qValue, ioGroupNum, portConf.cdp1864disable.portNumber, CDP1864DISABLE);
    }

    if (portConf.cdp1864toneLatch.portNumber != -1)
    {
        if (portConf.cdp1864toneLatch.qValue == -1)
            printBuffer.Printf("    Output %d: tone latch", portConf.cdp1864toneLatch.portNumber);
        else
            printBuffer.Printf("    Q = %d & output %d: tone latch", portConf.cdp1864toneLatch.qValue,  portConf.cdp1864toneLatch.portNumber);
        p_Main->message(printBuffer);

        p_Computer->setOutType(portConf.cdp1864toneLatch.qValue, ioGroupNum, portConf.cdp1864toneLatch.portNumber, CDP1864TONE);
    }
        
    printBuffer.Printf("    EF %d: in frame indicator\n", portConf.cdp1864Ef);
    p_Main->message(printBuffer);

    backGroundInit_ = 1;
    colourMask_ = 0;
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
    for (int graphicsMode = 0; graphicsMode < pixieGraphics_.screenend; graphicsMode++)
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
    if (computerType_ == VIPII || computerType_ == VIP)
    {
#if defined(__WXMAC__)
        p_Main->eventRefreshVideo(false, videoNumber_);
#else
        videoScreenPointer->disableScreen(colour_[colourIndex_+backGround_], videoWidth_+2*offsetX_, videoHeight_+2*offsetY_);
#endif
    }
}

void Pixie::outPixieBackGround()
{
    graphicsOn_ = true;
    backGround_++;
    if (backGround_ == 12)  backGround_ = 8;
    newBackGround_ = true;
    reBlit_ = true;
//  drawScreen();
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
        if (graphicsMode_ == pixieGraphics_.interrupt-2) pixieEf_ = 0;
        if (graphicsMode_ == pixieGraphics_.start) pixieEf_ = 1;
        if (graphicsMode_ == pixieGraphics_.start+videoHeight_-4) pixieEf_ = 0;
        if (graphicsMode_ == pixieGraphics_.start+videoHeight_) pixieEf_ = 1;
        if (graphicsMode_ >= pixieGraphics_.screenend)
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
        if (graphicsMode_ == pixieGraphics_.interrupt)
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
    if (graphicsMode_ >= pixieGraphics_.start && graphicsMode_ <=pixieGraphics_.end && graphicsOn_ && vidInt_ == 1 && graphicsNext_ >=4 && graphicsNext_ < (4+graphicsX_))
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
                    if (vram1 & 128)    color |= 1;
                    if (vram2 & 128)    color |= 2;
                    plot(j+i, (int)graphicsMode_-pixieGraphics_.start, 1, color);
                    vram1 <<= 1;
                    vram2 <<= 1;
                }
            }
            else
            {
                v = p_Computer->pixieDmaOut(&color);
                for (int i=0; i<8; i++)
                {
                    plot(j+i, (int)graphicsMode_-pixieGraphics_.start,(v & 128) ? 1 : 0, (color|colourMask_)&7);
                    if (graphicsX_ == 16)
                        plot(j+i, (int)graphicsMode_-pixieGraphics_.start+1,(v & 128) ? 1 : 0, (color|colourMask_)&7);
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
        v = p_Computer->pixieDmaOut(&color);
        for (int i=0; i<8; i++)
        {
            plot(j+i, (int)graphicsMode_ - pixieGraphics_.start,(v & 128) ? 1 : 0, (color|colourMask_)&7);
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
        if (graphicsMode_ >= 322)
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
    if (graphicsMode_ >= 72 && graphicsMode_ <=199 && graphicsOn_ && vidInt_ == 1 && graphicsNext_ >=4 && graphicsNext_ < 12)
    {
        j = 0;
        while(graphicsNext_ >= 4 && graphicsNext_ < 12)
        {
            graphicsNext_ ++;
            v = p_Computer->pixieDmaOut(&color);
            for (int i=0; i<8; i++)
            {
                plot(j+i, (int)graphicsMode_-72,(v & 128) ? 1 : 0, (color|colourMask_)&7);
                plot(j+i, (int)graphicsMode_-71,(v & 128) ? 1 : 0, (color|colourMask_)&7);
                plot(j+i, (int)graphicsMode_-70,(v & 128) ? 1 : 0, (color|colourMask_)&7);
                plot(j+i, (int)graphicsMode_-69,(v & 128) ? 1 : 0, (color|colourMask_)&7);
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

void Pixie::cyclePixieTelmac()
{
    int j;
    Byte v;
    int color;

    if (graphicsNext_ == 0)
    {
        p_Computer->debugTrace("----  H.Sync");
        graphicsMode_++;
        if (graphicsMode_ == pixieGraphics_.interrupt-2) pixieEf_ = 0;
        if (graphicsMode_ == pixieGraphics_.start) pixieEf_ = 1;
        if (graphicsMode_ == pixieGraphics_.start+videoHeight_-4) pixieEf_ = 0;
        if (graphicsMode_ == pixieGraphics_.start+videoHeight_) pixieEf_ = 1;
        if (graphicsMode_ >= pixieGraphics_.screenend)
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
        if (graphicsMode_ == pixieGraphics_.interrupt)
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
    if (graphicsMode_ >= pixieGraphics_.start && graphicsMode_ <=pixieGraphics_.end && graphicsOn_ && vidInt_ == 1 && graphicsNext_ >=4 && graphicsNext_ <=11)
    {
        j = 0;
        while(graphicsNext_ >= 4 && graphicsNext_ <= 11)
        {
            graphicsNext_ ++;
            v = p_Computer->pixieDmaOut(&color);
            for (int i=0; i<8; i++)
            {
                plot(j+i, (int)graphicsMode_-pixieGraphics_.start, (v & 128) ? 1 : 0, (color|colourMask_)&7);
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

    if (offsetY_ > pixieGraphics_.start)
    {
        setColour(colourIndex_+bgColor_[pixieGraphics_.screenend-1]);
        drawRectangle(0, 0, videoWidth_+2*offsetX_, offsetY_ - pixieGraphics_.start);
        drawRectangle(0, pixieGraphics_.screenend, videoWidth_+2*offsetX_, pixieGraphics_.screenend + pixieGraphics_.start - offsetY_);
    }

    for (int graphicsMode = 0; graphicsMode < pixieGraphics_.screenend; graphicsMode++)
    {
        setColour(colourIndex_+bgColor_[graphicsMode]);
        drawRectangle(0, graphicsMode - pixieGraphics_.start + offsetY_, videoWidth_ + offsetX_ * 2, 1);
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

    int y = (int)graphicsMode_ - pixieGraphics_.start + offsetY_;
    if (y >= 0)
        drawRectangle(0, y, videoWidth_ + offsetX_ * 2, 1);

    reBlit_ = true;
}

void Pixie::plot(int x, int y, int c, int color)
{
    wxPen penClr;

    if (!bgChanged)
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



PixieFred::PixieFred(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, int videoNumber)
: Pixie(title, pos, size, zoom, zoomfactor, computerType, videoNumber)
{
    displayType_ = 3;
    setDisplayType(displayType_);
    colourIndex_ = 0;
    videoNumber_ = videoNumber;
}

void PixieFred::configurePixie()
{
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    
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
                v = p_Computer->pixieDmaOut(&color);
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

PixieVip2K::PixieVip2K(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, int videoNumber)
: Pixie(title, pos, size, zoom, zoomfactor, computerType, videoNumber)
{
    colourIndex_ = 0;
    videoNumber_ = videoNumber;
}

PixieVip2K::PixieVip2K(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, int videoNumber, int videoType)
: Pixie(title, pos, size, zoom, zoomfactor, computerType, videoNumber, videoType)
{
    colourIndex_ = 0;
    videoNumber_ = videoNumber;
}

void PixieVip2K::configurePixie()
{
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setInType(6, PIXIEIN);
    p_Computer->setInType(7, PIXIEOUT);
    p_Computer->setOutType(6, PIXIEIN);
    p_Computer->setOutType(7, PIXIEOUT);
    p_Computer->setEfType(1, PIXIEEF);
    
    backGroundInit_ = 1;
    colourMask_ = 0;
    
    p_Main->message("Configuring VIP2K Video");
    p_Main->message("    Input 6: enable graphics, input 7: disable graphics, EF 1: DMA indicator");
    p_Main->message("    Output 6: enable graphics, ouptut 7: disable graphics\n");
    
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

void PixieVip2K::configureVip2K(IoConfiguration portConf)
{
    p_Computer->setCycleType(VIDEOCYCLE_VIP2K, VIP2KVIDEOCYCLE);
    p_Computer->setOutType(portConf.vip2KOutput, VIP2KVIDEODISABLE);
    p_Computer->setInType(portConf.vip2KInput, VIP2KVIDEOENABLE);

    wxString printBuffer;
    p_Main->message("Configuring VIP2K Video");

    if (portConf.vip2KDoubleScreenIo)
    {
        p_Computer->setInType(portConf.vip2KOutput, VIP2KVIDEODISABLE);
        p_Computer->setOutType(portConf.vip2KInput, VIP2KVIDEOENABLE);
        printBuffer.Printf("    Output/input %d: disable graphics, Output/input %d: enable graphics, EF %d: in frame indicator\n", portConf.vip2KOutput, portConf.vip2KInput, portConf.vip2KEf);
    }
    else
    {
        printBuffer.Printf("    Output %d: disable graphics, input %d: enable graphics, EF %d: in frame indicator\n", portConf.vip2KOutput, portConf.vip2KInput, portConf.vip2KEf);
    }
    p_Main->message(printBuffer);

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

PixieStudioIV::PixieStudioIV(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, int videoNumber)
: Pixie(title, pos, size, zoom, zoomfactor, computerType, videoNumber)
{
}

void PixieStudioIV::configurePixie()
{
    p_Computer->setOutType(4, PIXIEOUT);
    p_Computer->setOutType(6, PIXIEOUT);
    p_Computer->setOutType(5, STUDIOIVDMA);
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setEfType(1, PIXIEEF);

    backGroundInit_ = 9;
    colourMask_ = 0;

    p_Main->message("Configuring RCA Studio IV Video Chip");

    p_Main->message("    Output 4/6: bit 0-2 background colour, bit 3 white foreground");
    p_Main->message("    Output 4/6: bit 4-5 enable graphics, bit 6 PAL/NTSC");
    p_Main->message("    Output 5, enable DMA");
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
        if (graphicsMode_ >= pixieGraphics_.screenend)
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
        if (graphicsMode_ == pixieGraphics_.interrupt)
        {
            p_Computer->pixieInterrupt();
            p_Computer->setCycle0();
        }
    }
    if (graphicsMode_ >= pixieGraphics_.start && graphicsMode_ <=pixieGraphics_.end && graphicsNext_ >=4 && graphicsNext_ <= 19)
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

void PixieStudioIV::switchVideoMode(int videoMode)
{
    if (videoMode == videoMode_)
        return;
    
    videoMode_ = videoMode;
    
    if (videoMode_ == PAL)
    {
        pixieGraphics_.interrupt = 74;
        pixieGraphics_.start = 76;
        pixieGraphics_.end = 267;
        pixieGraphics_.screenend = 312;
        videoHeight_ = 192;
    }
    else
    {
        pixieGraphics_.interrupt = 62;
        pixieGraphics_.start = 64;
        pixieGraphics_.end = 191;
        pixieGraphics_.screenend = 262;
        videoHeight_ = 128;
    }
    p_Main->eventSetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_*xZoomFactor_, (videoHeight_+2*borderY_[videoType_])*zoom_, DON_T_CALL_CHANGE_SCREEN_SIZE, false, videoNumber_);
}

PixieEti::PixieEti(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, int videoNumber)
: Pixie(title, pos, size, zoom, zoomfactor, computerType, videoNumber)
{
}

void PixieEti::configurePixie()
{
    p_Computer->setOutType(1, PIXIEBACKGROUND);
    p_Computer->setCycleType(VIDEOCYCLE_PIXIE, PIXIECYCLE);
    p_Computer->setInType(1, PIXIEIN);
    p_Computer->setInType(4, PIXIEOUT);
    p_Computer->setEfType(1, PIXIEEF);

    backGroundInit_ = 8;
    colourMask_ = 7;

    p_Main->message("Configuring CDP 1864");

    p_Main->message("    Output 1: switch background colour, output 4: tone latch");
    p_Main->message("    Input 1: enable graphics, input 4: disable graphics");
    p_Main->message("    EF 1: in frame indicator\n");
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
        if (graphicsMode_ == pixieGraphics_.interrupt-2) pixieEf_ = 0;
        if (graphicsMode_ == pixieGraphics_.start) pixieEf_ = 1;
        if (graphicsMode_ == pixieGraphics_.start+videoHeight_-4) pixieEf_ = 0;
        if (graphicsMode_ == pixieGraphics_.start+videoHeight_) pixieEf_ = 1;
        if (graphicsMode_ >= pixieGraphics_.screenend)
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
        if (graphicsMode_ == pixieGraphics_.interrupt)
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
    if (graphicsMode_ >= pixieGraphics_.start && graphicsMode_ <=pixieGraphics_.end && graphicsOn_ && vidInt_ == 1 && graphicsNext_ >=4 && graphicsNext_ <=11)
    {
        j = 0;
        while(graphicsNext_ >= 4 && graphicsNext_ <= 11)
        {
            graphicsNext_ ++;
            v = p_Computer->pixieDmaOut(&color);
            for (int i=0; i<8; i++)
            {
                plot(j+i, (int)graphicsMode_-pixieGraphics_.start, (v & 128) ? 1 : 0, (color|colourMask_)&7);
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


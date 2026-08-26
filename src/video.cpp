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

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/clipbrd.h>
#include "wx/frame.h"
#include "wx/graphics.h"

#include "main.h"
#include "video.h"

BEGIN_EVENT_TABLE(VideoScreen, wxWindow)
    EVT_PAINT(VideoScreen::onPaint)
    EVT_CHAR(VideoScreen::onChar)
    EVT_KEY_DOWN(VideoScreen::onKeyDown)
    EVT_KEY_UP(VideoScreen::onKeyUp)
END_EVENT_TABLE()

VideoScreen::VideoScreen(wxWindow *parent, const wxSize& size, double zoom, int videoNumber)
: wxWindow(parent, wxID_ANY, wxDefaultPosition, size)
{
    vipiiRcaMode_ = false;
    zoom_ = zoom;
    xZoomFactor_ = 1;
    vt100_ = false;
    videoNumber_ = videoNumber;
}

VideoScreen::VideoScreen(wxWindow *parent, const wxSize& size, double zoom, int videoNumber, double xZoomFactor)
: wxWindow(parent, wxID_ANY, wxDefaultPosition, size)
{
    vipiiRcaMode_ = false;
    zoom_ = zoom;
    xZoomFactor_ = xZoomFactor;
    vt100_ = false;
    videoNumber_ = videoNumber;
}

VideoScreen::VideoScreen(wxWindow *parent, const wxSize& size, double zoom, int videoNumber, double xZoomFactor, bool vipiiRcaMode)
: wxWindow(parent, wxID_ANY, wxDefaultPosition, size)
{
    vipiiRcaMode_ = vipiiRcaMode;
    zoom_ = zoom;
    xZoomFactor_ = xZoomFactor;
    vt100_ = false;
    videoNumber_ = videoNumber;
}

VideoScreen::VideoScreen(wxWindow *parent, const wxSize& size, double zoom, bool vt100, int uartNumber)
: wxWindow(parent, wxID_ANY, wxDefaultPosition, size)
{
    vipiiRcaMode_ = false;
    zoom_ = zoom;
    xZoomFactor_ = 1;
    vt100_ = vt100;
    uartNumber_ = uartNumber;

    keyStart_ = 0;
    keyEnd_ = 0;
    lastKey_ = 0;
    
    forceUpperCase_ = p_Main->getUpperCase();
}

void VideoScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    if (p_Main->emuClosing())
        return;
    
    wxPaintDC dcWindow(this);
#ifdef __WXMAC__
    dcWindow.SetUserScale((double)zoom_*xZoomFactor_, zoom_);
#endif
    if (vt100_)
    {
#ifdef __WXMAC__
        if (p_Vt100[uartNumber_] != NULL)
            p_Vt100[uartNumber_]->reBlit(dcWindow);
#else
        if (p_Vt100[uartNumber_] != NULL)
            p_Vt100[uartNumber_]->setReBlit();
#endif
    }
    else
    {
#ifdef __WXMAC__
        p_Video[videoNumber_]->reBlit(dcWindow);
#else
        p_Video[videoNumber_]->setReBlit();
#endif
    }
}

void VideoScreen::onChar(wxKeyEvent& event)
{
    int key = event.GetKeyCode();
    if (vt100_)
    {
        if (p_Vt100[uartNumber_]->charPressed(event))
            return;
        if (forceUpperCase_ && key >= 'a' && key <= 'z')
            key -= 32;
        if (key > 255) key = 0;
        if (key !=0 && key < 128)
        {
            vtOut(key);
        }
    }
    else
    {
        if (p_Main->getUseCtrlvKey())
        {
            if (key == p_Main->getCtrlvKey() && event.GetModifiers() == CTRL_V)
            {
                if (wxTheClipboard->Open())
                {
#ifndef __WXMAC__
                    if (wxTheClipboard->IsSupported( wxDF_TEXT ))
                    {
#endif
                        wxTextDataObject data;
                        wxTheClipboard->GetData( data );
                        p_Computer->ctrlvTextXml(data.GetText());
#ifndef __WXMAC__
                    }
#endif
                    wxTheClipboard->Close();
                }
                return;
            }
        }
    }
    p_Computer->charEvent(event, key);
}

void VideoScreen::vtOut(int value)
{
    if (keyEnd_ != keyStart_-1 ||(keyEnd_ ==25 && keyStart_ != 0))
    {
        keyBuffer_[keyEnd_++] = value;
        if (keyEnd_ == 26) keyEnd_ = 0;
//        p_Vt100[uartNumber_]->dataAvailable(value);
        p_Computer->dataAvailable(value, uartNumber_);
        if (value == 27) p_Vt100[uartNumber_]->framingError(1);
    }
}

void VideoScreen::onKeyDown(wxKeyEvent& event)
{
    int keycode;
    keycode = event.GetKeyCode();
    
    if (vt100_)
    {
        if (p_Main->checkFunctionKey(event))
            return;
        if (keycode == lastKey_)
        {
            if (!repeat_) return;

            if (event.GetModifiers() == wxMOD_CONTROL)
                return;

            switch (keycode)
            {
                case WXK_HOME:
                case WXK_ESCAPE:
                case WXK_SCROLL:
                case WXK_TAB:
                case WXK_RETURN:
                    return;
                break;
            }
        }
        lastKey_ = keycode;
        p_Vt100[uartNumber_]->keyDownPressed(event);
    }
    else
    {
        if (p_Main->checkFunctionKey(event))
            return;
        if (!p_Computer->keyDownExtended(keycode, event))
        {
            if (!p_Computer->keyDownPressed(event.GetKeyCode()))
                event.Skip();
        }
    }
}

void VideoScreen::onKeyUp(wxKeyEvent& event)
{
    p_Main->onKeyUp(event);
    if (vt100_)
    {
        lastKey_ = 0;
        p_Vt100[uartNumber_]->keyUpPressed();
        if (!p_Computer->keyUpReleased(event.GetKeyCode(), event))
            event.Skip();
    }
    else
    {
        lastKey_ = 0;
        if (!p_Computer->keyUpReleased(event.GetKeyCode(), event))
            event.Skip();
    }
}

Byte VideoScreen::getKey(Byte vtOut)
{
    if (keyStart_ == keyEnd_)
        return p_Computer->getKey(vtOut);
    vtOut = keyBuffer_[keyStart_++];
    if (keyStart_ == 26) keyStart_ = 0;
    if (keyStart_ != keyEnd_)
    {
//        p_Vt100[uartNumber_]->dataAvailable(vtOut);
        p_Computer->dataAvailable(vtOut, uartNumber_);
    }
    return vtOut;
}

void VideoScreen::blit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height, wxDC *source, wxCoord xsrc, wxCoord ysrc)
{
    wxClientDC dcWindow(this);

    dcWindow.SetUserScale((double)zoom_*xZoomFactor_, zoom_);
    dcWindow.Blit(xdest, ydest, width, height, source, xsrc, ysrc);
}

void VideoScreen::refreshVideo()
{
    this->Refresh();
    this->Update();
}

void VideoScreen::drawExtraBackground(wxColour clr, int width, int height, wxCoord offsetX, wxCoord offsetY)
{
    wxClientDC dcWindow(this);
    wxSize size = wxGetDisplaySize();

    dcWindow.SetBrush(wxBrush(clr));
    dcWindow.SetPen(wxPen(clr));

    int xStart = (int)((2*offsetX+width)*zoom_*xZoomFactor_);
    dcWindow.DrawRectangle(xStart, 0, size.x-xStart, size.y);

    int yStart = (int)((2*offsetY+height)*zoom_);
    dcWindow.DrawRectangle(0, yStart, size.x, size.y-yStart);
}

void VideoScreen::drawRectangle(wxColour clr, int x, int y, wxCoord width, wxCoord height)
{
    wxClientDC dcWindow(this);

    dcWindow.SetBrush(wxBrush(clr));
    dcWindow.SetPen(wxPen(clr));
    dcWindow.DrawRectangle(x, y, width, height);
}

void VideoScreen::disableScreen(wxColour clr, int xSize, int ySize)
{
    wxClientDC dcWindow(this);
    dcWindow.SetUserScale(zoom_*xZoomFactor_, zoom_);
    dcWindow.SetBrush(wxBrush(clr));
    dcWindow.SetPen(wxPen(clr));
    dcWindow.DrawRectangle(0, 0, xSize, ySize);
}

void VideoScreen::setForceUCVt(bool status)
{
    forceUpperCase_ = status;
}

void VideoScreen::setZoom(double zoom)
{
    zoom_ = zoom;
}

BEGIN_EVENT_TABLE(Video, wxFrame)
    EVT_CLOSE (Video::onClose)
    EVT_SIZE(Video::onSize)
END_EVENT_TABLE()

Video::Video(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size) 
{
    xZoomFactor_ = 1;
    zoomChanged_ = 0;
    videoType_ = 0;
    videoSyncCount_ = 0;
//    memoryDCvalid_ = true;
    memoryDCvalid_ = false;
    colourIndex_ = 0;
    offsetX_ = 0;
    offsetY_ = 0;

    // Graphics contexts are created by the subclasses (and by
    // changeScreenSize / resetScreenCopyPointer). NULL-init here so the
    // framebuffer flush guards (flushFramebufferMac) are well-defined for
    // the main/sprite plane contexts before their first assignment.
    gc = NULL;
    gcMainAndSpritePlane = NULL;
    gcMainPlane = NULL;
    gcSpritePlane = NULL;

// Software framebuffer is off by default; video types that need it
    // (pixie family first, others in later phases) call enableFramebufferMac().
    // NOTE: framebuffer state is now cross-platform (see video.h).
    macFramebufferEnabled_ = false;
    macPlaneId_ = 0;
    macFrameImage_[0] = macFrameImage_[1] = macFrameImage_[2] = NULL;
    macCurrentColour_[0] = macCurrentColour_[1] = macCurrentColour_[2] = *wxBLACK;
    macPlaneDirty_[0] = macPlaneDirty_[1] = macPlaneDirty_[2] = false;
}

Video::~Video()
{
    delete macFrameImage_[0];
    delete macFrameImage_[1];
    delete macFrameImage_[2];
}

void Video::onClose(wxCloseEvent&WXUNUSED(event) )
{
    p_Main->stopComputer();
}

void Video::onSize(wxSizeEvent&event)
{
    event.Skip();
    changeScreenSize_ = true;
}

void Video::reset()
{
}

void Video::setInterlace(bool WXUNUSED(status))
{
}

void Video::setStretchDot(bool status)
{
    if (status)
        p_Main->message("Illegal call to stretch dot set");
    else
        p_Main->message("Illegal call to stretch dot reset");
}

void Video::setReBlit()
{
    newBackGround_ = true;
    reBlit_ = true;
}

long Video::getVideoSyncCount()
{
    return videoSyncCount_;
}

void Video::resetVideoSyncCount()
{
    videoSyncCount_ = 0;
}

void Video::focus()
{
    p_Main->message("Illegal call to set CDP 1870 focus");
}

void Video::updateComxStatusLed(bool WXUNUSED(status))
{
    p_Main->message("Illegal call to update status led");
}

void Video::updateComxExpansionLed(bool WXUNUSED(status))
{
    p_Main->message("Illegal call to update expansion led");
}

void Video::dataAvailable()
{
    p_Main->message("Illegal call to vt-100 data available");
}

void Video::dataAvailable(Byte WXUNUSED(value))
{
    p_Main->message("Illegal call to vt-100 data available");
}

void Video::dataAvailableUart(bool WXUNUSED(value))
{
    p_Main->message("Illegal call to vt-100 data available uart");
}

void Video::framingError(bool WXUNUSED(data))
{
    p_Main->message("Illegal call to vt-100 framing error");
}

void Video::setFullScreen(bool WXUNUSED(fullScreenSet))
{
}

void Video::setVtMemory(int WXUNUSED(address), Byte WXUNUSED(value))
{
}

Byte Video::getVtMemory(int WXUNUSED(address))
{
    return 0;
}

bool Video::charPressed(wxKeyEvent& WXUNUSED(event))
{
    return false;
}

void Video::keyDownPressed(wxKeyEvent& WXUNUSED(event))
{
}

void Video::keyUpPressed()
{
}

void Video::uartOut(Byte WXUNUSED(value))
{
}

Byte Video::uartIn()
{
    return 0;
}

void Video::uartControl(Byte WXUNUSED(value))
{
}

Byte Video::uartStatus()
{
    return 0;
}


void Video::defineColours()
{
    wxString colour, button;

    ScreenInfo screenInfo = p_Main->getScreenInfo();
    numberOfColours_ = screenInfo.number;
    reColour_ = false;
    for (int i=screenInfo.start; i<screenInfo.number; i++)
    {
        colour.Printf("%d", i);
        colour.Trim(false);
        colour_[i] = wxColour(p_Main->getConfigItem("Xml/Colour"+colour, screenInfo.defaultColour[i]));
        brushColour_[i] = wxBrush(colour_[i]);
        penColour_[i] = wxPen(colour_[i], 1);
    }
    for (int i=0; i<VIDEOXMLMAX; i++)
    {
        button.Printf("%d", i);
        button.Trim(false);
        borderX_[i] = p_Main->getConfigItem("Xml/BorderX"+button, screenInfo.borderX[i]);
        borderY_[i] = p_Main->getConfigItem("Xml/BorderY"+button, screenInfo.borderY[i]);
    }
}

void Video::setColour(int colourNumber, wxString colour)
{
    colourNew_[colourNumber] = wxColour(colour);
    brushColourNew_[colourNumber] = wxBrush(colourNew_[colourNumber]);
    penColourNew_[colourNumber] = wxPen(colourNew_[colourNumber], 1);
}

void Video::setBorder(int videoNumber, int borderX, int borderY)
{
    borderXNew_[videoNumber] = borderX;
    borderYNew_[videoNumber] = borderY;
}

void Video::setScale(double scaleX)
{
    xZoomFactorNew_ = scaleX;
}

void Video::ResetIo()
{
}

void Video::setScreenSize()
{
    if (fullScreenSet_)
        return;
    if (wxIsMainThread())
        SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_*xZoomFactor_, (videoHeight_+2*borderY_[videoType_])*zoom_);
    else
    {
        if (videoScreenPointer->isVt())
            p_Main->eventSetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_*xZoomFactor_, (videoHeight_+2*borderY_[videoType_])*zoom_, CALL_CHANGE_SCREEN_SIZE, true, uartNumber_);
        else
            p_Main->eventSetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_*xZoomFactor_, (videoHeight_+2*borderY_[videoType_])*zoom_, CALL_CHANGE_SCREEN_SIZE, false, videoNumber_);
    }
}

void Video::changeScreenSize()
{
    if (p_Main->isZoomEventOngoing())
        return;

    changeScreenSize_ = false;
    memoryDCvalid_ = false;

    double zoomx, zoomy;

    wxSize size;
    if (wxIsMainThread())
    {
        if (videoScreenPointer->isVt())
            size = p_Vt100[uartNumber_]->GetClientSize();
        else
            size = p_Video[videoNumber_]->GetClientSize();
    }
    else
    {
        if (videoScreenPointer->isVt())
            size = p_Main->eventGetClientSize(true, uartNumber_);
        else
            size = p_Main->eventGetClientSize(false, videoNumber_);
    }
    destinationWidth_ = size.x;
    destinationHeight_ = size.y;
    
    if (p_Main->isFullScreenFloat())
    {
        zoomx = (double)destinationWidth_/(double)((videoWidth_+2*borderX_[videoType_])*xZoomFactor_);
        zoomy = (double)destinationHeight_/(double)(videoHeight_+2*borderY_[videoType_]);
    }
    else
    {
        zoomx = (int)(destinationWidth_/((videoWidth_+2*borderX_[videoType_])*xZoomFactor_));
        zoomy = (int)(destinationHeight_/(videoHeight_+2*borderY_[videoType_]));
    }
    if (zoomx <= zoomy)
        zoom_ = zoomx;
    else
        zoom_ = zoomy;
    offsetX_ = (destinationWidth_/(zoom_*xZoomFactor_) - videoWidth_) / 2;
    offsetY_ = (destinationHeight_/zoom_ - videoHeight_) / 2;

    extraBackGround_ = false;
    if ((int)((2*offsetX_+videoWidth_)*zoom_*xZoomFactor_) != destinationWidth_)
         extraBackGround_ = true;
    if ((int)((2*offsetY_+videoHeight_)*zoom_) != destinationHeight_)
         extraBackGround_ = true;

    videoScreenPointer->setZoom(zoom_);
    if (wxIsMainThread())
        SetClientSize(destinationWidth_, destinationHeight_);
    else
    {
        if (videoScreenPointer->isVt())
            p_Main->eventSetClientSize(destinationWidth_, destinationHeight_, DON_T_CALL_CHANGE_SCREEN_SIZE, true, uartNumber_);
        else
            p_Main->eventSetClientSize(destinationWidth_, destinationHeight_, DON_T_CALL_CHANGE_SCREEN_SIZE, false, videoNumber_);
    }

    dcMemory.SelectObject(wxNullBitmap);
    delete screenCopyPointer;
    screenCopyPointer = new wxBitmap(2*offsetX_+videoWidth_, 2*offsetY_+videoHeight_);
    
    dcMemory.SelectObject(*screenCopyPointer);
    
#ifdef __WXMAC__
    // Native graphics context, used by the non-framebuffer fallback draw path
    // on macOS (setColour / drawPoint / drawRectangle). The software-framebuffer
    // flush (flushFramebufferMac) composites through dcMemory.DrawBitmap and
    // does NOT use gc, so no graphics context is needed on Windows/Linux.
    delete gc;
    gc = wxGraphicsContext::Create(dcMemory);
    gc->SetAntialiasMode(wxANTIALIAS_NONE);
#endif

    double intPart;
    zoomFraction_ = (modf(zoom_, &intPart) != 0);

#ifndef __linux__ // Looks like reDrawing on zooming will (sometimes) crash on linux
    reDraw_ = true;
    newBackGround_ = true;
#endif

    memoryDCvalid_ = true;

    if (wxIsMainThread())
    {
        if (videoScreenPointer->isVt())
            p_Main->zoomEventVt(zoom_);
        else
            p_Main->zoomEvent(zoom_, videoNumber_);
    }
    else
        p_Main->eventZoom(zoom_, videoNumber_, videoScreenPointer->isVt());
}

void Video::onF3()
{
}

void Video::onF5()
{
//    if (videoType_ == VIDEO1870 && !v1870Configured_)
//        return;
        
    int num = 0;
    wxFile screenDump;
    wxString number;
    wxString fileName;

    fileName = p_Main->getScreenDumpFile();

    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    wxString Name = FullPath.GetName();
    wxString Path = FullPath.GetPath();
    wxString Ext = FullPath.GetExt();

    wxMemoryDC dcFile;

    screenFilePointer = new wxBitmap((2*offsetX_+videoWidth_)*xZoomFactor_, 2*offsetY_+videoHeight_);
    dcFile.SelectObject(*screenFilePointer);
    dcFile.SetUserScale(xZoomFactor_, 1);
    dcFile.Blit(0, 0, (2*offsetX_+videoWidth_)*xZoomFactor_, 2*offsetY_+videoHeight_, &dcMemory, 0, 0);

    while(wxFile::Exists(fileName))
    {
        num++;
        number.Printf("%d", num);
        fileName = Path + p_Main->getPathSep() + Name + "." + number + "." + Ext;
    }
    screenDump.Create(fileName);
    if (Ext == "bmp")
    {
        screenFilePointer->SaveFile(fileName, wxBITMAP_TYPE_BMP);
        delete screenFilePointer;
        return;
    }
    if (Ext == "jpeg")
    {
        screenFilePointer->SaveFile(fileName, wxBITMAP_TYPE_JPEG);
        delete screenFilePointer;
        return;
    }
    if (Ext == "png")
    {
        screenFilePointer->SaveFile(fileName, wxBITMAP_TYPE_PNG);
        delete screenFilePointer;
        return;
    }
    if (Ext == "pcx")
    {
        screenFilePointer->SaveFile(fileName, wxBITMAP_TYPE_PCX);
        delete screenFilePointer;
        return;
    }
    screenFilePointer->SaveFile(fileName, wxBITMAP_TYPE_BMP);
    delete screenFilePointer;
}

void Video::setZoom(double zoom)
{
    zoom_ = zoom;

    double intPart;
    zoomFraction_ = (modf(zoom_, &intPart) != 0);

    if (fullScreenSet_)
    {
        this->SetClientSize(destinationWidth_, destinationHeight_);
        videoScreenPointer->SetClientSize(destinationWidth_, destinationHeight_);
    }
    else
    {
        this->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_*xZoomFactor_, (videoHeight_+2*borderY_[videoType_])*zoom_);
        videoScreenPointer->SetClientSize((videoWidth_+2*borderX_[videoType_])*zoom_*xZoomFactor_, (videoHeight_+2*borderY_[videoType_])*zoom_);
    }

    videoScreenPointer->setZoom(zoom_);
#ifndef __linux__
    reBlit_ = true;
#endif
}

void Video::reBlit(wxDC &dc)
{
    if (!memoryDCvalid_)
        return;
    
    dc.Blit(0, 0, videoWidth_+2*offsetX_, videoHeight_+2*offsetY_, &dcMemory, 0, 0);
    
    if (extraBackGround_ && newBackGround_)
    {
        wxSize size = wxGetDisplaySize();

        dc.SetBrush(wxBrush(colour_[colourIndex_+backGround_]));
        dc.SetPen(wxPen(colour_[colourIndex_+backGround_]));

        int xStart = (int)((2*offsetX_+videoWidth_)*zoom_*xZoomFactor_);
        dc.DrawRectangle(xStart, 0, size.x-xStart, size.y);

        int yStart = (int)((2*offsetY_+videoHeight_)*zoom_);
        dc.DrawRectangle(0, yStart, size.x, size.y-yStart);

        newBackGround_ = false;
    }
}

void Video::drawExtraBackground(wxColour clr)
{
    videoScreenPointer->drawExtraBackground(clr, videoWidth_, videoHeight_, offsetX_, offsetY_);
    newBackGround_ = false;
}

void Video::copyScreen()
{
}

void Video::reDrawBar()
{
}
 
void Video::updateStatusLed(bool WXUNUSED(status), int WXUNUSED(card), int WXUNUSED(i))
{
}

void Video::setClock(double WXUNUSED(clock))
{
}

void Video::activateMainWindow()
{
    bool maximize = IsMaximized();
    Iconize(false);
    Raise();
    Show(true);
    Maximize(maximize);
}

void Video::setColour(int clr)
{
//    if (p_Main->isZoomEventOngoingButNotFullScreen())
//        return;
    if (macFramebufferEnabled_)
    {
        // Deferred rendering: remember the colour only, do NOT touch gc.
        // The colour_[]/brushColour_[]/penColour_[] arrays are already up
        // to date (defineColours / reColour_).
        macCurrentColour_[macPlaneId_] = brushColour_[clr].GetColour();
        return;
    }
#if defined(__WXMAC__)
    gc->SetBrush(brushColour_[clr]);
    gc->SetPen(penColour_[clr]);
#else
    dcMemory.SetBrush(brushColour_[clr]);
    dcMemory.SetPen(penColour_[clr]);
#endif
}

void Video::setColour(wxColour clr)
{
    //    if (p_Main->isZoomEventOngoingButNotFullScreen())
    //        return;
    if (macFramebufferEnabled_)
    {
        macCurrentColour_[macPlaneId_] = clr;
        return;
    }
#if defined(__WXMAC__)
    gc->SetBrush(wxBrush(clr));
    gc->SetPen(wxPen(clr));
#else
    dcMemory.SetBrush(wxBrush(clr));
    dcMemory.SetPen(wxPen(clr));
#endif
}

void Video::drawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    if (macFramebufferEnabled_)
    {
        drawRectangleFramebufferMac(x, y, width, height);
        return;
    }
#if defined(__WXMAC__)
    gc->DrawRectangle(x, y, width-1, height-1);
#else
    dcMemory.DrawRectangle(x, y, width, height);
#endif
}

void Video::drawPoint(wxCoord x, wxCoord y)
{
    if (macFramebufferEnabled_)
    {
        drawPointFramebufferMac(x, y);
        return;
    }
#if defined(__WXMAC__)
    gc->DrawRectangle(x, y, 0, 0);
#else
    dcMemory.DrawPoint(x, y);
#endif
}

void Video::setColourMutex(int clr)
{
    // Delegates to the base Video::setColour so the macOS software framebuffer
    // (when enabled) is used here too - VIS1870 / TMS9918 / SN76430N draw
    // through these variants. With the framebuffer disabled (default) this
    // is byte-for-byte identical to the old direct gc/dcMemory code.
    //
    // IMPORTANT: the call must be QUALIFIED (Video::setColour), not an
    // unqualified virtual call. VIS1870 overrides drawPoint() (same signature,
    // implicitly virtual) and routes it back through drawPointMutex(), so an
    // unqualified virtual call from here would recurse forever
    // (drawPoint -> drawPointMutex -> drawPoint -> ...).
    Video::setColour(clr);
}

void Video::drawRectangleMutex(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    Video::drawRectangle(x, y, width, height);
}

void Video::drawPointMutex(wxCoord x, wxCoord y)
{
    Video::drawPoint(x, y);
}

void Video::enableFramebufferMac()
{
    macFramebufferEnabled_ = true;
}

void Video::setMacPlane(int plane)
{
    macPlaneId_ = plane;
}

void Video::drawPointFramebufferMac(wxCoord x, wxCoord y)
{
    ensureFramebufferMac(macPlaneId_);
    int w = macFrameImage_[macPlaneId_]->GetWidth();
    int h = macFrameImage_[macPlaneId_]->GetHeight();
    if (x < 0 || x >= w || y < 0 || y >= h)
        return;
    unsigned char* data = macFrameImage_[macPlaneId_]->GetData();
    int offset = ((int)y * w + (int)x) * 3;
    data[offset]     = macCurrentColour_[macPlaneId_].Red();
    data[offset + 1] = macCurrentColour_[macPlaneId_].Green();
    data[offset + 2] = macCurrentColour_[macPlaneId_].Blue();
    macPlaneDirty_[macPlaneId_] = true;
}

void Video::drawRectangleFramebufferMac(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    ensureFramebufferMac(macPlaneId_);
    int w = macFrameImage_[macPlaneId_]->GetWidth();
    int h = macFrameImage_[macPlaneId_]->GetHeight();
    // Match the base macOS gc->DrawRectangle(x, y, width-1, height-1)
    // semantics so the framebuffer produces the identical image.
    //
    // NOTE on the macOS gc geometry: the CoreGraphics / wxGraphicsContext
    // rectangle fill endpoint is INCLUSIVE, so gc->DrawRectangle(x, y, w, h)
    // fills (w+1) x (h+1) pixels. That is why the base mac path passes
    // (width-1, height-1): it fills exactly width x height pixels. It also
    // explains drawPoint, which issues gc->DrawRectangle(x, y, 0, 0) and
    // must produce a single 1x1 pixel. So the exclusive limits below are
    // x+width and y+height (filling x..x+width-1, y..y+height-1).
    int x2 = (int)x + (int)width;        // exclusive x limit
    int y2 = (int)y + (int)height;       // exclusive y limit
    int cx0 = wxMax((int)x, 0);
    int cy0 = wxMax((int)y, 0);
    int cx1 = wxMin(x2, w);
    int cy1 = wxMin(y2, h);
    if (cx1 <= cx0 || cy1 <= cy0)
        return;
    unsigned char r = macCurrentColour_[macPlaneId_].Red();
    unsigned char g = macCurrentColour_[macPlaneId_].Green();
    unsigned char b = macCurrentColour_[macPlaneId_].Blue();
    unsigned char* data = macFrameImage_[macPlaneId_]->GetData();
    for (int iy = cy0; iy < cy1; iy++)
    {
        int row = iy * w;
        for (int ix = cx0; ix < cx1; ix++)
        {
            int offset = (row + ix) * 3;
            data[offset]     = r;
            data[offset + 1] = g;
            data[offset + 2] = b;
        }
    }
    macPlaneDirty_[macPlaneId_] = true;
}

void Video::ensureFramebufferMac(int plane)
{
    int w = 2 * offsetX_ + videoWidth_;
    int h = 2 * offsetY_ + videoHeight_;
    if (macFrameImage_[plane] != NULL && macFrameImage_[plane]->GetWidth() == w && macFrameImage_[plane]->GetHeight() == h)
        return;
    // Framebuffer must match the dcMemory/screenCopyPointer size, which is
    // recreated on window size / zoom changes (Video::changeScreenSize).
    delete macFrameImage_[plane];
    macFrameImage_[plane] = new wxImage(w, h);
    macFrameImage_[plane]->SetRGB(wxRect(0, 0, w, h), 0, 0, 0);   // clear to black
    macPlaneDirty_[plane] = true;
}

void Video::flushFramebufferMac()
{
    for (int plane = 0; plane < 3; plane++)
    {
        if (!macPlaneDirty_[plane] || macFrameImage_[plane] == NULL)
            continue;
        wxMemoryDC *dcPlane = &dcMemory;
        if (plane == 1)
            dcPlane = &dcMemoryMainPlane;
        else if (plane == 2)
            dcPlane = &dcMemorySpritePlane;
        // Planes 1/2 belong to the multi-plane types (TMS9918) and are only
        // ever dirty when their backing bitmaps exist.
        if (plane == 1 && mainPlanePointer == NULL)
            continue;
        if (plane == 2 && spritePlanePointer == NULL)
            continue;
        // ONE bitmap draw into the plane's memory DC replaces the per-pixel
        // writes. Previously this went through the graphics context
        // (gcPlane->DrawBitmap + Flush), but a Cairo-backed memory-DC graphics
        // context (Linux wxGTK) does not reliably propagate back to the
        // underlying bitmap -- see wxCairoContext::wxCairoContext(const
        // wxMemoryDC&), whose GTK branch reuses GetCairoContext()/
        // gdk_cairo_create() instead of mapping the bitmap data directly (as
        // the MSW branch does with cairo_image_surface_create_for_data).
        // wxMemoryDC::DrawBitmap is deterministic on every platform and needs
        // no graphics context.
        wxBitmap bitmap(*macFrameImage_[plane]);
        dcPlane->DrawBitmap(bitmap, 0, 0);
        macPlaneDirty_[plane] = false;
    }
}

void Video::copyFramebufferMac(int fromPlane, int toPlane)
{
    ensureFramebufferMac(fromPlane);
    ensureFramebufferMac(toPlane);
    wxImage *src = macFrameImage_[fromPlane];
    wxImage *dst = macFrameImage_[toPlane];
    // Both planes use the same size (2*offsetX_+videoWidth_ x
    // 2*offsetY_+videoHeight_), so a straight memcpy seeds the target plane
    // with the source plane's content (e.g. the TMS9918 sprite plane gets the
    // full main-plane image before sprites are drawn on top). fromPlane keeps
    // its own dirty state; only the target is marked dirty.
    memcpy(dst->GetData(), src->GetData(),
           (size_t)dst->GetWidth() * dst->GetHeight() * 3);
    macPlaneDirty_[toPlane] = true;
}

void Video::splashScreen()
{
    if (p_Main->showSplashScreen())
        splashScreen_ = new SplashScreen(this);
}

void Video::refreshVideo()
{
    videoScreenPointer->refreshVideo();
}

void Video::setClientSize(wxSize size)
{
    this->SetClientSize(size);
}


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
        dcWindow.SetUserScale((double)zoom_*xZoomFactor_, zoom_);
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
    p_Computer->charEvent(key);
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
    memoryDCvalid_ = true;
    colourIndex_ = 0;
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
    for (int i=0; i<screenInfo.numberVideo; i++)
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
        SetClientSize(destinationWidth_, destinationHeight_);
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
    if (videoType_ == VIDEO1870 && !v1870Configured_)
        return;
        
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
#if defined(__WXMAC__)
    gc->DrawRectangle(x, y, width-1, height-1);
#else
    dcMemory.DrawRectangle(x, y, width, height);
#endif
}

void Video::drawPoint(wxCoord x, wxCoord y)
{
#if defined(__WXMAC__)
    gc->DrawRectangle(x, y, 0, 0);
#else
    dcMemory.DrawPoint(x, y);
#endif
}

void Video::setColourMutex(int clr)
{
    //    if (p_Main->isZoomEventOngoingButNotFullScreen())
    //        return;
#if defined(__WXMAC__)
    gc->SetBrush(brushColour_[clr]);
    gc->SetPen(penColour_[clr]);
#else
    dcMemory.SetBrush(brushColour_[clr]);
    dcMemory.SetPen(penColour_[clr]);
#endif
}

void Video::drawRectangleMutex(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
#if defined(__WXMAC__)
    gc->DrawRectangle(x, y, width-1, height-1);
#else
    dcMemory.DrawRectangle(x, y, width, height);
#endif
}

void Video::drawPointMutex(wxCoord x, wxCoord y)
{
#if defined(__WXMAC__)
    gc->DrawRectangle(x, y, 0, 0);
#else
    dcMemory.DrawPoint(x, y);
#endif
}

void Video::splashScreen()
{
    if (p_Main->showSplashScreen())
        splashScreen_ = new SplashScreen(this);
}

Byte Video::readPramDirect(Word address)
{
    reDraw_ = true;
    return pageMemory_[address];
}

Byte Video::readCramDirect(Word address)
{
    reDraw_ = true;
    return characterMemory_[address];
}

void Video::writeCramDirect(Word address, Byte value)
{
    characterMemory_[address] = value;
    reDraw_ = true;
}

void Video::writePramDirect(Word address, Byte value)
{
    pageMemory_[address] = value;
    reDraw_ = true;
}

void Video::refreshVideo()
{
    videoScreenPointer->refreshVideo();
}

void Video::setClientSize(wxSize size)
{
    this->SetClientSize(size);
}


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

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#if !wxUSE_COMBOCTRL
    #error "Please set wxUSE_COMBOCTRL to 1 and rebuild the library."
#endif

#include "main.h"
#include "video_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(VideoConfig, XmlBase)
    EVT_TOGGLEBUTTON(XRCID("VideoTraceButton"), VideoConfig::onVideoTrace)
    EVT_BUTTON(XRCID("VideoClearButton"), VideoConfig::onVideoClear)
END_EVENT_TABLE()

VideoConfig::VideoConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: XmlBase(title, pos, size, mode, dataDir, iniDir)
{
    videoTraceString_ = "";
    videoTrace_ = false;
}

VideoConfig::~VideoConfig()
{
}

void VideoConfig::updateVideoPanel()
{
#if defined(__linux__)
    if (!wxIsMainThread())
        wxMutexGuiEnter();
#endif

    if (computerConfiguration.vis1870Configuration.defined)
        p_Main->updateVis1870Panel();

    if (computerConfiguration.tmsConfiguration.defined)
        p_Main->updateTms9918Panel();

    if (computerConfiguration.i8275Configuration.defined)
        p_Main->updateI8275Panel();

    if (computerConfiguration.scn2672Configuration.defined)
        p_Main->updateScn2672Panel();
    
    if (computerConfiguration.mc6845Configuration.defined)
        p_Main->updateMc6845Panel();
    
    if (computerConfiguration.mc6847Configuration.defined)
        p_Main->updateMc6847Panel();
    
    if (computerConfiguration.cdp1861Configuration.defined)
        p_Main->updatePixiePanel();
    
    if (computerConfiguration.coinConfiguration.defined)
        p_Main->updateCoinVideoPanel();
    
    if (computerConfiguration.cdp1862Configuration.defined)
        p_Main->updateCdp1862Panel();
    
    if (computerConfiguration.cdp1864Configuration.defined)
        p_Main->updateCdp1864Panel();

    if (computerConfiguration.fredVideoConfiguration.defined)
        p_Main->updateFredVideoPanel();
    
    if (computerConfiguration.vip2KVideoConfiguration.defined)
        p_Main->updateVip2KVideoPanel();
    
    if (computerConfiguration.studio4VideoConfiguration.defined)
        p_Main->updateSt4VideoPanel();
		
    if (computerConfiguration.sn76430NConfiguration.defined)
        p_Main->updateSn76430NPanel();
    
#if defined(__linux__)
    if (!wxIsMainThread())
        wxMutexGuiLeave();
#endif
}

void VideoConfig::videoConfigInit()
{
    videoConfigGuiPosition_ = XRCCTRL(*this, "PanelNoVideo", wxPanel)->GetPosition();
    videoConfigWidth_ = 0;
 
    XRCCTRL(*this, "PanelNoVideo", wxPanel)->Show();
    XRCCTRL(*this, "VideoTraceWindow", wxTextCtrl)->Hide();
    XRCCTRL(*this, "VideoTraceButton", wxToggleButton)->Hide();
    XRCCTRL(*this, "VideoClearButton", wxButton)->Hide();
}

void VideoConfig::videoConfigUsed(wxString panelName)
{
    XRCCTRL(*this, "PanelNoVideo", wxPanel)->Hide();
    XRCCTRL(*this, "VideoTraceWindow", wxTextCtrl)->Show();
    XRCCTRL(*this, "VideoTraceButton", wxToggleButton)->Show();
    XRCCTRL(*this, "VideoClearButton", wxButton)->Show();
	
    XRCCTRL(*this, panelName, wxPanel)->Show();

    wxSize configSize = XRCCTRL(*this, panelName, wxPanel)->GetSize();
    XRCCTRL(*this, panelName, wxPanel)->SetPosition(wxPoint(videoConfigGuiPosition_.x, videoConfigGuiPosition_.y));
    videoConfigGuiPosition_.y += configSize.y;
    if (configSize.x > videoConfigWidth_)
        videoConfigWidth_ = configSize.x;
}

void VideoConfig::showTraceText(wxString function, wxString address, wxString value, int showTrace)
{
    if (showTrace == DO_NOT_SHOW_ANY_TRACE)
        return;
    
    wxString addressString = "      ";
    if (showTrace == SHOW_ADDRESS_TRACE)
        addressString.Printf("%04X: ", p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
    videoTrace(addressString + function + " " + address + "=" + value);
}

void VideoConfig::showTraceText(wxString function, wxString value, int showTrace)
{
    if (showTrace == DO_NOT_SHOW_ANY_TRACE)
        return;
   
    int count = function.Find("\n");
    if (count != wxNOT_FOUND)
        function = function.Left(count);

    wxString addressString = "      ";
    if (showTrace == SHOW_ADDRESS_TRACE)
        addressString.Printf("%04X: ", p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
    videoTrace(addressString + function + "=" + value);
}

void VideoConfig::showTraceTextRead(wxString function, wxString value, int showTrace)
{
    if (showTrace == DO_NOT_SHOW_ANY_TRACE)
        return;
   
    int count = function.Find("\n");
    if (count != wxNOT_FOUND)
        function = function.Left(count);

    wxString addressString = "      ";
    if (showTrace == SHOW_ADDRESS_TRACE)
        addressString.Printf("%04X: ", p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
    videoTrace(addressString + "D=" + function + "=" + value);
}

void VideoConfig::showTraceText(wxString function, int showTrace)
{
    if (showTrace == DO_NOT_SHOW_ANY_TRACE)
        return;
    
    int count = function.Find("\n");
    if (count != wxNOT_FOUND)
        function = function.Left(count);
    
    count = function.Find("(");
    if (count != wxNOT_FOUND)
        function = function.Left(count);

    wxString addressString = "      ";
    if (showTrace == SHOW_ADDRESS_TRACE)
        addressString.Printf("%04X: ", p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
    videoTrace(addressString + function);
}

void VideoConfig::showNotRunning()
{
    videoTraceWindowPointer->SetInsertionPointEnd();
    videoTraceWindowPointer->WriteText("No computer running\n");
}

void VideoConfig::videoTrace(wxString buffer)
{
#if defined(__WXMAC__) || defined(__linux__)
    videoTraceString_ = videoTraceString_ + buffer + "\n";
#else
    videoTraceWindowPointer->AppendText(buffer+"\n");
#endif
}

void VideoConfig::onVideoTrace(wxCommandEvent& WXUNUSED(event))
{
    videoTrace_ = !videoTrace_;
}

void VideoConfig::onVideoClear(wxCommandEvent& WXUNUSED(event))
{
    videoTraceWindowPointer->Clear();
}

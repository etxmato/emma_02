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

BEGIN_EVENT_TABLE(VideoConfig, Ct2425Config)
    EVT_TOGGLEBUTTON(XRCID("VideoTraceButton"), VideoConfig::onVideoTrace)
    EVT_TOGGLEBUTTON(XRCID("VideoTimeButton"), VideoConfig::onVideoTime)
    EVT_BUTTON(XRCID("VideoClearButton"), VideoConfig::onVideoClear)
END_EVENT_TABLE()

VideoConfig::VideoConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: Ct2425Config(title, pos, size, mode, dataDir, iniDir)
{
    restartVideoTime_ = true;
    videoTraceString_ = "";
    videoTrace_ = false;
    videoTime_ = true;
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
    
    if (computerConfiguration.crt8002Configuration.defined)
        p_Main->updateCrt8002Panel();
    
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
    if (!mode_.gui)
        return;

    videoConfigGuiPosition_ = XRCCTRL(*this, "PanelNoVideo", wxPanel)->GetPosition();
    videoConfigWidth_ = 0;
 
    XRCCTRL(*this, "PanelNoVideo", wxPanel)->Show();
    XRCCTRL(*this, "VideoTraceWindow", wxTextCtrl)->Hide();
    XRCCTRL(*this, "VideoTraceButton", wxToggleButton)->Hide();
    XRCCTRL(*this, "VideoTimeButton", wxToggleButton)->Hide();
    XRCCTRL(*this, "VideoClearButton", wxButton)->Hide();
}

void VideoConfig::videoConfigUsed(wxString panelName)
{
    if (!mode_.gui)
        return;

    XRCCTRL(*this, "PanelNoVideo", wxPanel)->Hide();
    XRCCTRL(*this, "VideoTraceWindow", wxTextCtrl)->Show();
    XRCCTRL(*this, "VideoTraceButton", wxToggleButton)->Show();
    XRCCTRL(*this, "VideoTimeButton", wxToggleButton)->Show();
    XRCCTRL(*this, "VideoTimeButton", wxToggleButton)->SetValue(videoTime_);
    XRCCTRL(*this, "VideoClearButton", wxButton)->Show();
	
    XRCCTRL(*this, panelName, wxPanel)->Show();

    wxSize configSize = XRCCTRL(*this, panelName, wxPanel)->GetSize();
    XRCCTRL(*this, panelName, wxPanel)->SetPosition(wxPoint(videoConfigGuiPosition_.x, videoConfigGuiPosition_.y));
    videoConfigGuiPosition_.y += configSize.y;
    if (configSize.x > videoConfigWidth_)
        videoConfigWidth_ = configSize.x;
}

void VideoConfig::showVideoTraceText(wxString function, wxString address, wxString value, int showTrace)
{
    if (!mode_.gui)
        return;

    if (showTrace == DO_NOT_SHOW_ANY_TRACE)
        return;
    
    wxString addressString = "      ";
    if (showTrace == SHOW_ADDRESS_TRACE)
    {
        if (videoTime_)
            videoTimeTrace();
        else
            addressString.Printf("%04X: ", p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
    }
    videoTrace(addressString + function + " " + address + "=" + value);
}

void VideoConfig::showVideoTraceText(wxString function, wxString value, int showTrace)
{
    if (!mode_.gui)
        return;

    if (showTrace == DO_NOT_SHOW_ANY_TRACE)
        return;
   
    int count = function.Find("\n");
    if (count != wxNOT_FOUND)
        function = function.Left(count);

    wxString addressString = "      ";
    if (showTrace == SHOW_ADDRESS_TRACE)
    {
        if (videoTime_)
            videoTimeTrace();
        else
            addressString.Printf("%04X: ", p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
    }
    videoTrace(addressString + function + "=" + value);
}

void VideoConfig::showVideoTraceTextRead(wxString function, wxString value, int showTrace)
{
    if (!mode_.gui)
        return;

    if (showTrace == DO_NOT_SHOW_ANY_TRACE)
        return;
   
    int count = function.Find("\n");
    if (count != wxNOT_FOUND)
        function = function.Left(count);

    wxString addressString = "      ";
    if (showTrace == SHOW_ADDRESS_TRACE)
    {
        if (videoTime_)
            videoTimeTrace();
        else
            addressString.Printf("%04X: ", p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
    }
    videoTrace(addressString + "D=" + function + "=" + value);
}

void VideoConfig::showVideoTraceText(wxString function, int showTrace)
{
    if (!mode_.gui)
        return;

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
    {
        if (videoTime_)
            videoTimeTrace();
        else
            addressString.Printf("%04X: ", p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
    }
    videoTrace(addressString + function);
}

void VideoConfig::videoTimeTrace()
{
    wxLongLong timeInterval = p_Main->traceTime(restartVideoTime_);
    restartVideoTime_ = false;

    wxString print_buffer;
    int h,m,s, millis;

    millis = timeInterval.GetLo();
    s = (int)(millis/1000);
    h = s / 3600;
    s -= (h * 3600);
    m = s / 60;
    s -= (m * 60);
    millis -= (h * 3600000);
    millis -= (m * 60000);
    millis -= (s * 1000);

    print_buffer.Printf("Time = %02d:%02d:%03d", m, s, millis);
    
    wxString addressString;
    addressString.Printf("%04X: ", p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
    videoTrace(addressString + print_buffer);
}


void VideoConfig::showVideoNotRunning()
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

void VideoConfig::onVideoTime(wxCommandEvent& WXUNUSED(event))
{
    videoTime_ = !videoTime_;
}

void VideoConfig::onVideoClear(wxCommandEvent& WXUNUSED(event))
{
    videoTraceWindowPointer->Clear();
}

long VideoConfig::getVideoRegisterValue(wxString registerReference)
{
    if (!computerRunning_)
    {
        p_Main->showVideoNotRunning();
        return -1;
    }

    return get16BitValue(registerReference);
}


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
#include "other_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(OtherConfig, XmlBase)
    EVT_TOGGLEBUTTON(XRCID("OtherTraceButton"), OtherConfig::onOtherTrace)
    EVT_BUTTON(XRCID("OtherClearButton"), OtherConfig::onOtherClear)
    EVT_TOGGLEBUTTON(XRCID("OtherTimeButton"), OtherConfig::onOtherTime)
END_EVENT_TABLE()

OtherConfig::OtherConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: XmlBase(title, pos, size, mode, dataDir, iniDir)
{
    restartOtherTime_ = true;
    otherTraceString_ = "";
    otherTrace_ = false;
    otherTime_ = true;
}

OtherConfig::~OtherConfig()
{
}

void OtherConfig::updateOtherPanel()
{
#if defined(__linux__)
    if (!wxIsMainThread())
        wxMutexGuiEnter();
#endif
    
    if (numberOfCt2425_> 0)
        p_Main->updateCt2425Panel();
#if defined(__linux__)
    if (!wxIsMainThread())
        wxMutexGuiLeave();
#endif
}

void OtherConfig::otherConfigInit()
{
    if (!mode_.gui)
        return;

    otherConfigGuiPosition_ = XRCCTRL(*this, "PanelNoOther", wxPanel)->GetPosition();
    otherConfigWidth_ = 0;
 
    XRCCTRL(*this, "PanelNoOther", wxPanel)->Show();
    XRCCTRL(*this, "OtherTraceWindow", wxTextCtrl)->Hide();
    XRCCTRL(*this, "OtherTraceButton", wxToggleButton)->Hide();
    XRCCTRL(*this, "OtherClearButton", wxButton)->Hide();
    XRCCTRL(*this, "OtherTimeButton", wxToggleButton)->Hide();
}

void OtherConfig::otherConfigUsed(wxString panelName)
{
    if (!mode_.gui)
        return;

    XRCCTRL(*this, "PanelNoOther", wxPanel)->Hide();
    XRCCTRL(*this, "OtherTraceWindow", wxTextCtrl)->Show();
    XRCCTRL(*this, "OtherTraceButton", wxToggleButton)->Show();
    XRCCTRL(*this, "OtherClearButton", wxButton)->Show();
    XRCCTRL(*this, "OtherTimeButton", wxToggleButton)->Show();
    XRCCTRL(*this, "OtherTimeButton", wxToggleButton)->SetValue(otherTime_);

    XRCCTRL(*this, panelName, wxPanel)->Show();

    wxSize configSize = XRCCTRL(*this, panelName, wxPanel)->GetSize();
    XRCCTRL(*this, panelName, wxPanel)->SetPosition(wxPoint(otherConfigGuiPosition_.x, otherConfigGuiPosition_.y));
    otherConfigGuiPosition_.y += configSize.y;
    if (configSize.x > otherConfigWidth_)
        otherConfigWidth_ = configSize.x;
}

void OtherConfig::showOtherTraceText(wxString function, wxString address, wxString value, int showTrace)
{
    if (!mode_.gui)
        return;

    if (showTrace == DO_NOT_SHOW_ANY_TRACE)
        return;
    
    wxString addressString = "      ";
    if (showTrace == SHOW_ADDRESS_TRACE)
    {
        if (otherTime_)
            otherTimeTrace();
        else
            addressString.Printf("%04X: ", p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
    }
    otherTrace(addressString + function + " " + address + "=" + value);
}

void OtherConfig::showOtherTraceText(wxString function, wxString value, int showTrace)
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
        if (otherTime_)
            otherTimeTrace();
        else
            addressString.Printf("%04X: ", p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
    }
    otherTrace(addressString + function + "=" + value);
}

void OtherConfig::showOtherTraceTextRead(wxString function, wxString value, int showTrace)
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
        if (otherTime_)
            otherTimeTrace();
        else
            addressString.Printf("%04X: ", p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
    }
    otherTrace(addressString + "D=" + function + "=" + value);
}

void OtherConfig::showOtherTraceText(wxString function, int showTrace)
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
        if (otherTime_)
            otherTimeTrace();
        else
            addressString.Printf("%04X: ", p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
    }
    otherTrace(addressString + function);
}

void OtherConfig::otherTimeTrace()
{
    wxLongLong timeInterval = p_Main->traceTime(restartOtherTime_);
    restartOtherTime_ = false;

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
    otherTrace(addressString + print_buffer);
}

void OtherConfig::showOtherNotRunning()
{
    otherTraceWindowPointer->SetInsertionPointEnd();
    otherTraceWindowPointer->WriteText("No computer running\n");
}

void OtherConfig::otherTrace(wxString buffer)
{
#if defined(__WXMAC__) || defined(__linux__)
    otherTraceString_ = otherTraceString_ + buffer + "\n";
#else
    otherTraceWindowPointer->AppendText(buffer+"\n");
#endif
}

void OtherConfig::onOtherTrace(wxCommandEvent& WXUNUSED(event))
{
    otherTrace_ = !otherTrace_;
}

void OtherConfig::onOtherClear(wxCommandEvent& WXUNUSED(event))
{
    otherTraceWindowPointer->Clear();
}

void OtherConfig::onOtherTime(wxCommandEvent& WXUNUSED(event))
{
    otherTime_ = !otherTime_;
}

long OtherConfig::getOtherRegisterValue(wxString registerReference)
{
    if (!computerRunning_)
    {
        p_Main->showOtherNotRunning();
        return -1;
    }

    return get16BitValue(registerReference);
}


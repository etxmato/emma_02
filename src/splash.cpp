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

#include "splash.h"
#include "main.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(SplashScreen, wxDialog)
    EVT_BUTTON(XRCID("NoShow"), SplashScreen::noShow)
    EVT_BUTTON(XRCID("NoClose"), SplashScreen::noClose)
    EVT_TIMER(1000, SplashScreen::onTimer)
    EVT_CLOSE (SplashScreen::onClose)
END_EVENT_TABLE()

SplashScreen::SplashScreen(wxWindow *parent)
{
    wxString dialog;
    vt100_ = false;
    
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"splash_" + p_Main->getFontSize() + ".xrc");
    dialog = p_Main->getSplashDialog();
    wxXmlResource::Get()->LoadDialog(this, parent, dialog);
    
    if (dialog == "DEFAULT")
    {
        XRCCTRL(*this,"SplashText",wxStaticText)->SetLabel(p_Main->getSplashText());
        XRCCTRL(*this,"DEFAULT",wxDialog)->DoLayoutAdaptation();
    }
    Show(true);
    
    timerPointer = new wxTimer(this, 1000);
    timerPointer->Start(6000, wxTIMER_ONE_SHOT);
}

SplashScreen::~SplashScreen()
{
    timerPointer->Stop();
    delete timerPointer;
}

void SplashScreen::noShow(wxCommandEvent&WXUNUSED(event))
{
    p_Main->hideSplashScreen();
    Show(false);
}

void SplashScreen::noClose(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this,"NoClose",wxButton)->Enable(false);
    timerPointer->Stop();
}

void SplashScreen::onClose(wxCloseEvent&WXUNUSED(event))
{
    Show(false);
}

void SplashScreen::onTimer(wxTimerEvent&WXUNUSED(event))
{
    Show(false);
}

BEGIN_EVENT_TABLE(CompletedSplashScreen, wxDialog)
    EVT_TIMER(1000, CompletedSplashScreen::onTimer)
    EVT_CLOSE (CompletedSplashScreen::onClose)
END_EVENT_TABLE()

CompletedSplashScreen::CompletedSplashScreen(wxWindow *parent)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"splash_" + p_Main->getFontSize() + ".xrc");
    wxXmlResource::Get()->LoadDialog(this, parent, "COMPLETED");
    Show(true);
    
    timerPointer = new wxTimer(this, 1000);
    timerPointer->Start(6000, wxTIMER_ONE_SHOT);
}

CompletedSplashScreen::~CompletedSplashScreen()
{
    timerPointer->Stop();
    delete timerPointer;
}

void CompletedSplashScreen::onClose(wxCloseEvent&WXUNUSED(event))
{
    Show(false);
}

void CompletedSplashScreen::onTimer(wxTimerEvent&WXUNUSED(event))
{
    Show(false);
}

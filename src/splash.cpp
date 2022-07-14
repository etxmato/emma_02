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
    EVT_TIMER(1000, SplashScreen::onTimer)
    EVT_CLOSE (SplashScreen::onClose)
END_EVENT_TABLE()

SplashScreen::SplashScreen(wxWindow *parent)
{
    wxString computerStr = p_Main->getRunningComputerStr();
    int computer = p_Main->getRunningComputerId();
    ElfConfiguration currentElfConfig;
    vt100_ = false;
    
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"splash_" + p_Main->getFontSize() + ".xrc");
    switch (computer)
    {
        case ELF:
        case ELFII:
        case SUPERELF:
        case DIY:
        case PICO:
            switch (p_Computer->getLoadedProgram())
            {
                case SUPERBASICV1:
                    currentElfConfig = p_Main->getElfConfiguration(computer);
                    if (currentElfConfig.vtType != VTNONE)
                        wxXmlResource::Get()->LoadDialog(this, parent, "SBV1_VT");
                    else
                        wxXmlResource::Get()->LoadDialog(this, parent, "SBV1_SUPER");
                    Show(true);
                break;
                    
                case SUPERBASICV3:
                    wxXmlResource::Get()->LoadDialog(this, parent, "SBV3_SUPER");
                    Show(true);
                break;

                case RCABASIC3:
                    wxXmlResource::Get()->LoadDialog(this, parent, "RCA_BASIC");
                    Show(true);
                break;
                 
                case MINIMON:
                case GOLDMON:
                    wxXmlResource::Get()->LoadDialog(this, parent, "Monitor");
                    Show(true);
                break;
                    
                case TINYBASIC:
                    currentElfConfig = p_Main->getElfConfiguration(computer);
                    if (currentElfConfig.vtType != VTNONE)
                        wxXmlResource::Get()->LoadDialog(this, parent, "TINY_VT");
                    else
                        wxXmlResource::Get()->LoadDialog(this, parent, "TINY");
                    Show(true);
                break;
                    
                default:
                    currentElfConfig = p_Main->getElfConfiguration(computer);
                    if (p_Computer->getLoadedOs() != NOOS && currentElfConfig.vtType != VTNONE)
                        wxXmlResource::Get()->LoadDialog(this, parent, "ELFOS");
                    Show(true);
                break;
            }
            break;
            
        case ELF2K:
            currentElfConfig = p_Main->getElfConfiguration(computer);
            if (currentElfConfig.vtType != VTNONE)
            {
                wxXmlResource::Get()->LoadDialog(this, parent, "Elf2K_VT");
                Show(true);
            }
        break;
            
       default:
            wxXmlResource::Get()->LoadDialog(this, parent, computerStr);
            Show(true);
        break;
    }
    
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

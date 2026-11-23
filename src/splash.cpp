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

#include "splash.h"
#include "main.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(SplashScreen, wxDialog)
    EVT_BUTTON(XRCID("NoShow"), SplashScreen::noShow)
    EVT_BUTTON(XRCID("NoClose"), SplashScreen::noClose)
    EVT_TIMER(1000, SplashScreen::onTimer)
    EVT_CLOSE (SplashScreen::onClose)
END_EVENT_TABLE()

#ifdef __WXMSW__
#if wxCHECK_VERSION(3, 1, 0)
static void ApplySplashDarkTitleBar(wxWindow* splashWin)
{
    if (!wxSystemSettings::GetAppearance().IsDark())
        return;

    HWND hwnd = (HWND)splashWin->GetHWND();
    if (hwnd == NULL)
        return;

    HMODULE dwmLib = ::LoadLibraryW(L"dwmapi.dll");
    if (dwmLib == NULL)
        return;

    typedef HRESULT (WINAPI *DwmSetWindowAttribute_t)(HWND, DWORD, LPCVOID, DWORD);
    DwmSetWindowAttribute_t setAttr = (DwmSetWindowAttribute_t)::GetProcAddress(dwmLib, "DwmSetWindowAttribute");
    if (setAttr != NULL)
    {
        BOOL useDark = TRUE;
        // DWMWA_USE_IMMERSIVE_DARK_MODE is 20 on Windows 10 20H1+ / 11, 19 on earlier builds
        if (FAILED(setAttr(hwnd, 20, &useDark, sizeof(useDark))))
            setAttr(hwnd, 19, &useDark, sizeof(useDark));
    }
    ::FreeLibrary(dwmLib);
}
#endif
#endif

SplashScreen::SplashScreen(wxWindow *parent)
    : wxDialog(parent, wxID_ANY, "Emma 02 - Info", wxDefaultPosition, wxDefaultSize,
               wxCAPTION|wxSTAY_ON_TOP|wxCLOSE_BOX|wxTAB_TRAVERSAL)
{
    vt100_ = false;
    noCloseButton_ = NULL;
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);

    // The splash window is built in code; splash_<fontsize>.xrc is no longer
    // loaded. Same text scaling as before: 11 (standard) uses the system font,
    // 12/14 set the point size on the dialog, the children inherit it.
    long fontSize = 11;
    p_Main->getFontSize().ToLong(&fontSize);
    if (fontSize != 11)
    {
        wxFont font = GetFont();
        font.SetPointSize((int)fontSize);
        SetFont(font);
    }

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(this, wxID_ANY, p_Main->getSplashText()), 0, (int)wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    noCloseButton_ = new wxButton(this, XRCID("NoClose"), "Don't close");
    buttonSizer->Add(noCloseButton_, 1, (int)wxGROW, 5);
    buttonSizer->AddSpacer(5);
    buttonSizer->Add(new wxButton(this, XRCID("NoShow"), "Don't show again"), 1, (int)wxGROW, 5);
    sizer->Add(buttonSizer, 0, (int)wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    SetSizerAndFit(sizer);
    Centre();

#ifdef __WXMSW__
    // Force the dark immersive title bar (wx pushes dark mode onto the frame's
    // controls, but a dialog created in code can keep the light title bar /
    // border).
#if wxCHECK_VERSION(3, 1, 0)
    ApplySplashDarkTitleBar(this);
#endif
#endif

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
    if (noCloseButton_ != NULL)
        noCloseButton_->Enable(false);
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

CompletedSplashScreen::CompletedSplashScreen(wxWindow *parent, const wxString& message)
    : wxDialog(parent, wxID_ANY, "Emma 02 - Info", wxDefaultPosition, wxDefaultSize,
               wxCAPTION|wxSTAY_ON_TOP|wxCLOSE_BOX|wxTAB_TRAVERSAL)
{
    timerPointer = NULL;
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);

    // Same text scaling as SplashScreen above: standard uses the system font,
    // large uses the point size of the selected font size.
    long fontSize = 11;
    p_Main->getFontSize().ToLong(&fontSize);
    if (fontSize != 11)
    {
        wxFont font = GetFont();
        font.SetPointSize((int)fontSize);
        SetFont(font);
    }

    wxString text = message;
    if (text.empty())
        text = "Re-install completed!";

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(this, wxID_ANY, text), 0, (int)wxALIGN_CENTER_HORIZONTAL|wxALL, 10);
    SetSizerAndFit(sizer);
    Centre();

#ifdef __WXMSW__
#if wxCHECK_VERSION(3, 1, 0)
    ApplySplashDarkTitleBar(this);
#endif
#endif

    Show(true);
    Raise();
    SetFocus();

    timerPointer = new wxTimer(this, 1000);
    timerPointer->Start(6000, wxTIMER_ONE_SHOT);
}

CompletedSplashScreen::~CompletedSplashScreen()
{
    if ( timerPointer != NULL )
    {
        timerPointer->Stop();
        delete timerPointer;
    }
}

void CompletedSplashScreen::onClose(wxCloseEvent&WXUNUSED(event))
{
    Show(false);
}

void CompletedSplashScreen::onTimer(wxTimerEvent&WXUNUSED(event))
{
    Show(false);
}

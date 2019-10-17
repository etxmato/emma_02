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
#include "cdp18s640.h"

#define SC0LED 0
#define SC1LED 1
#define WAITLED 2
#define CLEARLED 3

Cdp18s640Screen::Cdp18s640Screen(wxWindow *parent, const wxSize& size)
: Panel(parent, size)
{
}

Cdp18s640Screen::~Cdp18s640Screen()
{
#if defined (__WXMAC__)
    delete osx_text_runPButtonPointer;
    delete osx_text_runButtonPointer;
    delete osx_text_resetButtonPointer;
#else
    delete text_runPButtonPointer;
    delete text_runButtonPointer;
    delete text_resetButtonPointer;
#endif
    delete stepSwitchButton;

    for (int i=0; i<4; i++)
    {
        delete stateLedPointer[i];
    }
    for (int i=0; i<4; i++)
        delete addressTil313PointerItalic[i];
    for (int i=0; i<2; i++)
        delete dataTil313PointerItalic[i];
    delete qLedPointer;
    delete runLedPointer;
}

void Cdp18s640Screen::init()
{
    keyStart_ = 0;
    keyEnd_ = 0;
    lastKey_ = 0;
    
    wxClientDC dc(this);
    
    for (int i=0; i<4; i++)
    {
        addressTil313PointerItalic[i] = new Til313Italic(false);
        addressTil313PointerItalic[i]->init(dc, 45+i*28, 8);
        updateAddressTil313Italic_ = true;
    }
    for (int i=0; i<2; i++)
    {
        dataTil313PointerItalic[i] = new Til313Italic(false);
        dataTil313PointerItalic[i]->init(dc, 205+i*28,8);
        updateDataTil313Italic_ = true;
    }
    stateLedPointer[SC1LED] = new Led(dc, 50, 72, ELFLED);
    stateLedPointer[SC0LED] = new Led(dc, 88, 72, ELFLED);
    stateLedPointer[WAITLED] = new Led(dc, 126, 72, ELFLED);
    stateLedPointer[CLEARLED] = new Led(dc, 164, 72, ELFLED);
    qLedPointer = new Led(dc, 202, 72, ELFLED);
    runLedPointer = new Led(dc, 240, 72, ELFLED);

#if defined (__WXMAC__)
    osx_text_resetButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, 50, 126, "");
    osx_text_runButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, 110, 126, "U");
    osx_text_runPButtonPointer = new HexButton(dc, COSMICOS_HEX_BUTTON, 170, 126, "P");
#else
    text_resetButtonPointer = new wxButton(this, 3, "", wxPoint(50, 126), wxSize(25, 25), 0, wxDefaultValidator, "ResetButton");
    text_resetButtonPointer->SetToolTip("Reset");
    text_runButtonPointer = new wxButton(this, 1, "U", wxPoint(110, 126), wxSize(25, 25), 0, wxDefaultValidator, "RunButton");
    text_runButtonPointer->SetToolTip("RUN-U");
    text_runPButtonPointer = new wxButton(this, 2, "P", wxPoint(170, 126), wxSize(25, 25), 0, wxDefaultValidator, "RunPButton");
    text_runPButtonPointer->SetToolTip("RUN-P");
#endif
    stepSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 230, 126, "");

    this->connectKeyEvent(this);
}

void Cdp18s640Screen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);
    wxColourDatabase colour;

    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(0, 0, 310, 180);
#if defined(__WXMAC__)
	wxFont defaultFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
#else
	wxFont defaultFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
#endif

    dc.SetTextForeground(*wxBLACK);
    dc.SetFont(defaultFont);
    
    dc.DrawText("RESET", 36, 152);
    dc.DrawText("RUN", 106, 152);
    dc.DrawText("RUN", 166, 152);
    dc.DrawText("CONT", 219, 152);
    dc.DrawText("STEP", 224, 108);

    dc.DrawText("BUS", 218, 43);
    dc.DrawText("ADDRESS", 65, 43);

    dc.SetTextForeground(colour.Find("GREY"));
    
    dc.DrawText("SC1", 40, 84);
    dc.DrawText("SC0", 78, 84);
    dc.DrawText("WT", 119, 84);
    dc.DrawText("CLR", 154, 84);
    dc.DrawText("Q", 201, 84);
    dc.DrawText("RUN", 229, 84);

    addressTil313PointerItalic[3]->onPaint(dc);
    addressTil313PointerItalic[2]->onPaint(dc);
    addressTil313PointerItalic[1]->onPaint(dc);
    addressTil313PointerItalic[0]->onPaint(dc);
    dataTil313PointerItalic[1]->onPaint(dc);
    dataTil313PointerItalic[0]->onPaint(dc);
    for (int i=0; i<4; i++)
    {
        stateLedPointer[i]->onPaint(dc);
    }
    qLedPointer->onPaint(dc);
    runLedPointer->onPaint(dc);

#if defined (__WXMAC__)
    osx_text_resetButtonPointer->onPaint(dc);
    osx_text_runButtonPointer->onPaint(dc);
    osx_text_runPButtonPointer->onPaint(dc);
#endif
    stepSwitchButton->onPaint(dc);
}

void Cdp18s640Screen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);

#if defined (__WXMAC__)
    if (osx_text_resetButtonPointer->onMousePress(dc, x, y))
        p_Computer->onReset();
    
    if (osx_text_runButtonPointer->onMousePress(dc, x, y))
        p_Computer->onRunButton();
    
    if (osx_text_runPButtonPointer->onMousePress(dc, x, y))
        p_Computer->onRunPButton();
#endif
}

void Cdp18s640Screen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
#if defined (__WXMAC__)
    osx_text_resetButtonPointer->onMouseRelease(dc, x, y);
    osx_text_runButtonPointer->onMouseRelease(dc, x, y);
    osx_text_runPButtonPointer->onMouseRelease(dc, x, y);
#endif
    if (stepSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onSingleStep();
}

void Cdp18s640Screen::releaseButtonOnScreen(HexButton* buttonPoint)
{
    wxClientDC dc(this);
    
    buttonPoint->releaseButtonOnScreen(dc);
}

BEGIN_EVENT_TABLE(Cdp18s640Frame, wxFrame)
    EVT_CLOSE (Cdp18s640Frame::onClose)
    EVT_BUTTON(1, Cdp18s640Frame::onRunButton)
    EVT_BUTTON(2, Cdp18s640Frame::onRunPButton)
    EVT_BUTTON(3, Cdp18s640Frame::onResetButton)
END_EVENT_TABLE()

Cdp18s640Frame::Cdp18s640Frame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    cdp18s640ScreenPointer = new Cdp18s640Screen(this, size);
    cdp18s640ScreenPointer->init();
    
    this->SetClientSize(size);
    
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif
}

Cdp18s640Frame::~Cdp18s640Frame()
{
    delete cdp18s640ScreenPointer;
}

void Cdp18s640Frame::onClose(wxCloseEvent&WXUNUSED(event))
{
    Show(false);
}

void Cdp18s640Frame::onRunButton(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->onRunButton();
}

void Cdp18s640Frame::onRunPButton(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->onRunPButton();
}

void Cdp18s640Frame::onResetButton(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->onReset();
}




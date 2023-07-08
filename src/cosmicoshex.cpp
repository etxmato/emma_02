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

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#include "main.h"
#include "cosmicoshex.h"
#include "pushbutton.h"

CosmicoshexScreen::CosmicoshexScreen(wxWindow *parent, const wxSize& size, int tilType)
: Panel(parent, size, tilType)
{
    reqState_ = 1;
    seqState_ = 1;
    decState_ = 1;
    retState_ = 1;
}

CosmicoshexScreen::~CosmicoshexScreen()
{
    for (int i=0; i<8; i++)
    {
        delete segPointer[i];
    }
#if defined (__WXMAC__)
    for (int i=0;i<16;i++)
        delete osx_buttonPointer[i];
    delete osx_reqButtonPointer;
    delete osx_seqButtonPointer;
    delete osx_decButtonPointer;
    delete osx_retButtonPointer;
#else
    for (int i=0;i<16;i++)
        delete buttonPointer[i];
    delete reqButtonPointer;
    delete seqButtonPointer;
    delete decButtonPointer;
    delete retButtonPointer;
#endif
}

void CosmicoshexScreen::init()
{
    keyStart_ = 0;
    keyEnd_ = 0;
    lastKey_ = 0;
    forceUpperCase_ = p_Main->getUpperCase();

    wxClientDC dc(this);
    wxString buttonText;
    int x, y;

    dc.SetBackground(wxBrush(wxColour(255,255,255)));
    dc.DrawRectangle(0, 0, 185, 160);

#if defined(__linux__)
    wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    SetFont(defaultFont);
#endif
#if defined(__WXMAC__)
    wxFont defaultFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    SetFont(defaultFont);
#endif

    for (int i=0; i<8; i++)
    {
        segPointer[7-i] = new Til313full();
        segPointer[7-i]->init(dc, 3+i*22, 3);
    }

#if defined (__WXMAC__)
    for (int i=0; i<16; i++)
    {
        buttonText.Printf("%01X", i);
        x = 37+(i&0x3)*30;
        y = 126 -(int)i/4*30;
        osx_buttonPointer[i] = new HexButton(dc, PANEL_HEX_BUTTON, x, y, buttonText);
    }
    osx_seqButtonPointer = new HexButton(dc, PANEL_WIDE_HEX_BUTTON, 3, 36, "SEQ");
    osx_reqButtonPointer = new HexButton(dc, PANEL_WIDE_HEX_BUTTON, 3, 66, "REQ");
    osx_decButtonPointer = new HexButton(dc, PANEL_WIDE_HEX_BUTTON, 3, 96, "DEC");
    osx_retButtonPointer = new HexButton(dc, PANEL_WIDE_HEX_BUTTON, 3, 126, "RET");
#else
    for (int i=0;i<16;i++)
    {
        buttonText.Printf("%01X", i);
        x = 37+(i&0x3)*30;
        y = 126 -(int)i/4*30;
        buttonPointer[i] = new PushButton(this, i, buttonText, wxPoint(x, y), wxSize(30, 30), 0);
    }
    seqButtonPointer = new PushButton(this, 20, "SEQ", wxPoint(3, 36), wxSize(34, 30), 0);
    reqButtonPointer = new PushButton(this, 21, "REQ", wxPoint(3, 66), wxSize(34, 30), 0);
    decButtonPointer = new PushButton(this, 22, "DEC", wxPoint(3, 96), wxSize(34, 30), 0);
    retButtonPointer = new PushButton(this, 23, "RET", wxPoint(3, 126), wxSize(34, 30), 0);
#endif
    this->connectKeyEvent(this);
}

void CosmicoshexScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.SetBackground(wxBrush(wxColour(255,255,255)));
    dc.DrawRectangle(0, 0, 185, 160);

#if defined(__WXMAC__)
    rePaintLeds(dc);
#endif

    for (int i=0; i<8; i++)
    {
        segPointer[i]->onPaint(dc);
    }
#if defined (__WXMAC__)
    osx_seqButtonPointer->onPaint(dc);
    osx_reqButtonPointer->onPaint(dc);
    osx_decButtonPointer->onPaint(dc);
    osx_retButtonPointer->onPaint(dc);
    for (int i = 0; i<16; i++)
        osx_buttonPointer[i]->onPaint(dc);
#endif
}

void CosmicoshexScreen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
#if defined (__WXMAC__)
    if (osx_seqButtonPointer->onMouseRelease(dc, x, y))
        seqState_ = 1;
    if (osx_reqButtonPointer->onMouseRelease(dc, x, y))
        reqState_ = 1;
    if (osx_decButtonPointer->onMouseRelease(dc, x, y))
        decState_ = 1;
    if (osx_retButtonPointer->onMouseRelease(dc, x, y))
        retState_ = 1;
    for (int i = 0; i<16; i++)
    {
        if (osx_buttonPointer[i]->onMouseRelease(dc, x, y))
            p_Computer->onNumberKeyRelease(i);
    }
#endif
}

void CosmicoshexScreen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
#if defined (__WXMAC__)
    if (osx_seqButtonPointer->onMousePress(dc, x, y))
        seqState_ = 0;
    if (osx_reqButtonPointer->onMousePress(dc, x, y))
        reqState_ = 0;
    if (osx_decButtonPointer->onMousePress(dc, x, y))
        decState_ = 0;
    if (osx_retButtonPointer->onMousePress(dc, x, y))
        retState_ = 0;
    for (int i = 0; i<16; i++)
    {
        if (osx_buttonPointer[i]->onMousePress(dc, x, y))
            p_Computer->onNumberKeyPress(i);
    }
#endif
}

void CosmicoshexScreen::releaseButtonOnScreen(HexButton* buttonPoint)
{
    wxClientDC dc(this);
    
    buttonPoint->releaseButtonOnScreen(dc);
}

Byte CosmicoshexScreen::retState()
{
    return retState_;
}

Byte CosmicoshexScreen::reqState()
{
    return reqState_;
}

Byte CosmicoshexScreen::seqState()
{
    return seqState_;
}

Byte CosmicoshexScreen::decState()
{
    return decState_;
}

BEGIN_EVENT_TABLE(Cosmicoshex, wxFrame)
    EVT_CLOSE (Cosmicoshex::onClose)
    EVT_COMMAND(0, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(0, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(1, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(1, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(2, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(2, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(3, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(3, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(4, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(4, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(5, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(5, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(6, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(6, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(7, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(7, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(8, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(8, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(9, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(9, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(10, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(10, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(11, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(11, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(12, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(12, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(13, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(13, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(14, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(14, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(15, wxEVT_ButtonDownEvent, Cosmicoshex::onNumberKeyPressEvent)
    EVT_COMMAND(15, wxEVT_ButtonUpEvent, Cosmicoshex::onNumberKeyReleaseEvent)
    EVT_COMMAND(20, wxEVT_ButtonDownEvent, Cosmicoshex::onSeqButtonPress)
    EVT_COMMAND(20, wxEVT_ButtonUpEvent, Cosmicoshex::onSeqButtonRelease)
    EVT_COMMAND(21, wxEVT_ButtonDownEvent, Cosmicoshex::onReqButtonPress)
    EVT_COMMAND(21, wxEVT_ButtonUpEvent, Cosmicoshex::onReqButtonRelease)
    EVT_COMMAND(22, wxEVT_ButtonDownEvent, Cosmicoshex::onDecButtonPress)
    EVT_COMMAND(22, wxEVT_ButtonUpEvent, Cosmicoshex::onDecButtonRelease)
    EVT_COMMAND(23, wxEVT_ButtonDownEvent, Cosmicoshex::onRetButtonPress)
    EVT_COMMAND(23, wxEVT_ButtonUpEvent, Cosmicoshex::onRetButtonRelease)
END_EVENT_TABLE()

Cosmicoshex::Cosmicoshex(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    cosmicoshexScreenPointer = new CosmicoshexScreen(this, size, TILNONE);
    cosmicoshexScreenPointer->init();

    keypadValue_[0] = 0;
    keypadValue_[1] = 0;
    keypadValue_[2] = 0;
    keypadValue_[3] = 0;
    keypadValue_[4] = 0;
    reqState_ = 1;
    seqState_ = 1;
    decState_ = 1;
    retState_ = 1;
    this->SetClientSize(size);
}

Cosmicoshex::~Cosmicoshex()
{
    delete cosmicoshexScreenPointer;
}

void Cosmicoshex::onClose(wxCloseEvent&WXUNUSED(event))
{
    p_Computer->removeCosmicosHex();
}

void Cosmicoshex::keyLatch(int value)
{
    keyLatch_ = 0;
    switch (value)
    {
        case 1: keyLatch_ = 1; break;
        case 2: keyLatch_ = 2; break;
        case 4: keyLatch_ = 3; break;
        case 8: keyLatch_ = 4; break;
    }
}

void Cosmicoshex::onNumberKeyPressEvent(wxCommandEvent&event)
{
    int i = event.GetId();
    onNumberKeyPress(i);
}

void Cosmicoshex::onNumberKeyPress(int key)
{
    switch (key&3)
    {
        case 0: keypadValue_[(key>>2)+1] = 1; break;
        case 1: keypadValue_[(key>>2)+1] = 2; break;
        case 2: keypadValue_[(key>>2)+1] = 4; break;
        case 3: keypadValue_[(key>>2)+1] = 6; break;
    }
}

void Cosmicoshex::onNumberKeyReleaseEvent(wxCommandEvent&event)
{
    int i = event.GetId();
    onNumberKeyRelease(i);
}

void Cosmicoshex::onNumberKeyRelease(int key)
{
    keypadValue_[(key>>2)+1] = 0;
}

Byte Cosmicoshex::getKeyinput()
{ 
    return keypadValue_[keyLatch_];
}

void Cosmicoshex::onReqButtonPress(wxCommandEvent&WXUNUSED(event))
{
    reqState_ = 0;
}

void Cosmicoshex::onSeqButtonPress(wxCommandEvent&WXUNUSED(event))
{
    seqState_ = 0;
}

void Cosmicoshex::onDecButtonPress(wxCommandEvent&WXUNUSED(event))
{
    decState_ = 0;
}

void Cosmicoshex::onRetButtonPress(wxCommandEvent&WXUNUSED(event))
{
    retState_ = 0;
}

void Cosmicoshex::onReqButtonRelease(wxCommandEvent&WXUNUSED(event))
{
    reqState_ = 1;
}

void Cosmicoshex::onSeqButtonRelease(wxCommandEvent&WXUNUSED(event))
{
    seqState_ = 1;
}

void Cosmicoshex::onDecButtonRelease(wxCommandEvent&WXUNUSED(event))
{
    decState_ = 1;
}

void Cosmicoshex::onRetButtonRelease(wxCommandEvent&WXUNUSED(event))
{
    retState_ = 1;
}

Byte Cosmicoshex::retState()
{
#if defined (__WXMAC__)
    return cosmicoshexScreenPointer->retState();
#else
    return retState_;
#endif
}

Byte Cosmicoshex::reqState()
{
#if defined (__WXMAC__)
    return cosmicoshexScreenPointer->reqState();
#else
    return reqState_;
#endif
}

Byte Cosmicoshex::seqState()
{
#if defined (__WXMAC__)
    return cosmicoshexScreenPointer->seqState();
#else
    return seqState_;
#endif
}

Byte Cosmicoshex::decState()
{
#if defined (__WXMAC__)
    return cosmicoshexScreenPointer->decState();
#else
    return decState_;
#endif
}

void Cosmicoshex::segUpdate(int number, int value)
{
    cosmicoshexScreenPointer->showSeg(number, value);
}

void Cosmicoshex::segOff()
{
    for (int i=0; i<8; i++)
        cosmicoshexScreenPointer->showSeg(i, 0);
}

void Cosmicoshex::ledTimeout()
{
    cosmicoshexScreenPointer->ledTimeout();
}

void Cosmicoshex::setLedMs(long ms)
{
    cosmicoshexScreenPointer->setLedMs(ms);
}

Byte Cosmicoshex::getKey(Byte vtOut)
{
    return cosmicoshexScreenPointer->getKey(vtOut);
}

void Cosmicoshex::releaseButtonOnScreen(HexButton* buttonPointer)
{
    cosmicoshexScreenPointer->releaseButtonOnScreen(buttonPointer);
}

void Cosmicoshex::refreshPanel()
{
    cosmicoshexScreenPointer->refreshPanel();
}


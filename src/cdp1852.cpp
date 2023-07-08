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
#include "cdp1852.h"

Cdp1852Screen::Cdp1852Screen(wxWindow *parent, const wxSize& size, int tilType)
: Panel(parent, size, tilType)
{
}

Cdp1852Screen::~Cdp1852Screen()
{
    for (int i=0; i<8; i++)
    {
        delete ledPointer[i];
        delete dataSwitchButton[i];
    }
#if defined (__WXMAC__)
    delete osx_stbButtonPointer;
#else
    delete text_stbButtonPointer;
#endif
}

void Cdp1852Screen::init()
{
    wxClientDC dc(this);
    
    for (int i=0; i<8; i++)
    {
        ledPointer[i] = new Led(dc, 56+27*(7-i), 60, PIOLED);
        updateLed_[i]=true;
        setLed(i, 0);
        dataSwitchButton[i] = new SwitchButton(dc, PIO_VERTICAL_BUTTON, wxColour(255, 255, 255), BUTTON_DOWN, 50+27*(7-i), 94, "");
        ioSwitchState_[i] = 0;
    }
    outPutValue_ = 0;
    inPutValue_ = 0;

#if defined (__WXMAC__)
    osx_stbButtonPointer = new HexButton2(dc, PIO_HEX_BUTTON, 55, 144, "", 0);
#else
    text_stbButtonPointer = new wxButton(this, 1, "", wxPoint(55, 144), wxSize(25, 25), 0, wxDefaultValidator, "StbButton");
    text_stbButtonPointer->SetToolTip("STB");
#endif

    this->connectKeyEvent(this);
}

void Cdp1852Screen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);
    wxColourDatabase colour;
    wxString number;

    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(0, 0, 329, 219);
#if defined(__WXMAC__)
    wxFont defaultFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
#else
    wxFont defaultFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
#endif

    dc.SetTextForeground(*wxBLACK);
    dc.SetFont(defaultFont);
    
    for (int i=0; i<8; i++)
    {
        number.Printf("%d", i);
        dc.DrawText(number, 56+27*(7-i), 26);
    }
    dc.DrawText("OUT:", 10, 56);
    dc.DrawText("INP:", 10, 96);
    dc.DrawText("INT:", 10, 146);

    dc.SetTextForeground(colour.Find("GREY"));
    
#if defined (__WXMAC__)
    rePaintLeds(dc);
#endif

    for (int i=0; i<8; i++)
    {
        ledPointer[i]->onPaint(dc);
        dataSwitchButton[i]->onPaint(dc);
    }

#if defined (__WXMAC__)
    osx_stbButtonPointer->onPaint(dc);
#endif
}

void Cdp1852Screen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);

    if (osx_stbButtonPointer->onMousePress(dc, x, y))
        onStbButton();
}

void Cdp1852Screen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
    for (int i=0; i<8; i++)
    {
        if (dataSwitchButton[i]->onMouseRelease(dc, x, y))
            ioSwitch(i);
    }
#if defined (__WXMAC__)
    osx_stbButtonPointer->onMouseRelease(dc, x, y);
#endif
}

void Cdp1852Screen::releaseButtonOnScreen(HexButton* buttonPoint)
{
    wxClientDC dc(this);

    buttonPoint->releaseButtonOnScreen(dc);
}

void Cdp1852Screen::onStbButton()
{
    p_Computer->setEfState(0, 3, 0);
   
    p_Computer->requestInterrupt();
}

void Cdp1852Screen::ioSwitch(int i)
{
    if (ioSwitchState_[i])
        ioSwitchState_[i] = 0;
    else
        ioSwitchState_[i] = 1;
    
    Byte bitmask = (1 << i) ^ 0xff;
    outPutValue_ &= bitmask;
    outPutValue_ |=  ioSwitchState_[i] << i;
}

void Cdp1852Screen::reset()
{
    wxClientDC dc(this);

    outPutValue_ = 0;
    inPutValue_ = 0;
    for (int i=0; i<8; i++)
    {
        dataSwitchButton[i]->setState(dc, 0);
        ioSwitchState_[i] = 0;
    }
}

void Cdp1852Screen::writePort(Byte value)
{
    outPutValue_ = value;
    
    for (int i=0; i<8; i++)
    {
        setLed(i, value&1);
        value = value >> 1;
    }
}

Byte Cdp1852Screen::readPort()
{
    p_Computer->setEfState(0, 3, 1);
 
    return outPutValue_;
}

void Cdp1852Screen::refreshLeds()
{
    wxClientDC dc(this);

    for (int i=0; i<8; i++)
        refreshLed(dc, i);
}

BEGIN_EVENT_TABLE(Cdp1852Frame, wxFrame)
    EVT_CLOSE (Cdp1852Frame::onClose)
    EVT_BUTTON(1, Cdp1852Frame::onStbButton)
END_EVENT_TABLE()

Cdp1852Frame::Cdp1852Frame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    cdp1852ScreenPointer = new Cdp1852Screen(this, size, TILNONE);
    cdp1852ScreenPointer->init();
    
    this->SetClientSize(size);
    
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif
}

Cdp1852Frame::~Cdp1852Frame()
{
    delete cdp1852ScreenPointer;
}

void Cdp1852Frame::onClose(wxCloseEvent&WXUNUSED(event))
{
    p_Computer->removePio(0);
}

void Cdp1852Frame::onStbButton(wxCommandEvent&WXUNUSED(event))
{
    cdp1852ScreenPointer->onStbButton();
};



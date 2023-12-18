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

#include "elf2kswitch.h"

Elf2KswitchScreen::Elf2KswitchScreen(wxWindow *parent, const wxSize& size, int tilType)
: Panel(parent, size, tilType)
{
}

Elf2KswitchScreen::~Elf2KswitchScreen()
{
    delete mainBitmapPointer;

    delete runSwitchButton;
    delete mpSwitchButton;
    delete loadSwitchButton;
    delete inSwitchButton;
    for (int i=0; i<8; i++)
    {
        delete dataSwitchButton[i];
    }
}

void Elf2KswitchScreen::init()
{
    keyStart_ = 0;
    keyEnd_ = 0;
    lastKey_ = 0;
    forceUpperCase_ = p_Main->getUpperCase();

    wxClientDC dc(this);
    wxString switchNumber;

    mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/elf2kswitches.png", wxBITMAP_TYPE_PNG);

    runSwitchButton = new SwitchButton(dc, SWITCH_BUTTON_VERTICAL, wxColour(255, 255, 255), BUTTON_DOWN, 380, 31, "");
    mpSwitchButton = new SwitchButton(dc, SWITCH_BUTTON_VERTICAL, wxColour(255, 255, 255), BUTTON_DOWN, 332, 31, "");
    loadSwitchButton = new SwitchButton(dc, SWITCH_BUTTON_VERTICAL, wxColour(255, 255, 255), BUTTON_DOWN, 93, 31, "");
    inSwitchButton = new SwitchButton(dc, PUSH_BUTTON_ROUND_RED, wxColour(255, 255, 255), BUTTON_UP, 45, 31, "");

    for (int i=0; i<8; i++)
    {
        dataSwitchButton[i] = new SwitchButton(dc, SWITCH_BUTTON_VERTICAL, wxColour(255, 255, 255), BUTTON_DOWN, 45+48*(7-i), 111, "");
    }
    this->connectKeyEvent(this);
}

void Elf2KswitchScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.DrawBitmap(*mainBitmapPointer, 0, 0);
    inSwitchButton->onPaint(dc);
    runSwitchButton->onPaint(dc);
    mpSwitchButton->onPaint(dc);
    loadSwitchButton->onPaint(dc);
    for (int i=0; i<8; i++)
    {
        dataSwitchButton[i]->onPaint(dc);
    }
}

void Elf2KswitchScreen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);

    wxClientDC dc(this);

    if (inSwitchButton->onMousePress(dc, x, y))
        p_Computer->onInButtonPress(p_Computer->getData());
}

void Elf2KswitchScreen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);

    wxClientDC dc(this);

    if (inSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onInButtonRelease();
    if (runSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onRun();
    if (mpSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onMpButton();
    if (loadSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onLoadButton();
    for (int i=0; i<8; i++)
    {
        if (dataSwitchButton[i]->onMouseRelease(dc, x, y))
            p_Computer->dataSwitch(i);
    }
}

BEGIN_EVENT_TABLE(Elf2Kswitch, wxFrame)
    EVT_CLOSE (Elf2Kswitch::onClose)
END_EVENT_TABLE()

Elf2Kswitch::Elf2Kswitch(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    this->SetClientSize(size);

    elf2KswitchScreenPointer = new Elf2KswitchScreen(this, size, TILNONE);
    elf2KswitchScreenPointer->init();
}

Elf2Kswitch::~Elf2Kswitch()
{
    p_Main->setElf2KswitchPos(GetPosition());

    delete elf2KswitchScreenPointer;
}

void Elf2Kswitch::onClose(wxCloseEvent&WXUNUSED(event) )
{
    p_Computer->removeElf2KSwitch();
    Destroy();
}

void Elf2Kswitch::setRunButtonUp(int up)
{
    elf2KswitchScreenPointer->runSetState(up == 1);
}

void Elf2Kswitch::setInButtonUp(bool up)
{
    elf2KswitchScreenPointer->inSetState(up);
}

void Elf2Kswitch::setLoadButtonUp(int up)
{
    elf2KswitchScreenPointer->loadSetState(up == 1);
}

void Elf2Kswitch::setMpButtonUp(int up)
{
    elf2KswitchScreenPointer->mpSetState(up == 1);
}

void Elf2Kswitch::setDataSwitchUp(int number, int up)
{
    elf2KswitchScreenPointer->dataSetState(number, up == 1);
}

Byte Elf2Kswitch::getKey(Byte vtOut)
{
    return elf2KswitchScreenPointer->getKey(vtOut);
}


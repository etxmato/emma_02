/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 ***                                                             ***
 *** 1802 Code based on elf emulator by Michael H Riley with     ***
 *** copyright as below                                          ***
 *******************************************************************
*/

/*
 *******************************************************************
 *** This software is copyright 2006 by Michael H Riley          ***
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
#include "pushbutton.h"
#include "keypad.h"

KeypadScreen::KeypadScreen(wxWindow *parent, const wxSize& size)
: Panel(parent, size)
{
}

KeypadScreen::~KeypadScreen()
{
	delete mainBitmapPointer;
	for (int i=0;i<2;i++) 
	{
		delete ledPointer[i];
	}
#if defined (__WXMAC__)
    delete osx_push_inButtonPointer;
    for (int i=0;i<16;i++)
        delete osx_buttonPointer[i];
#else
	delete push_inButtonPointer;
	for (int i=0;i<16;i++)
		delete buttonPointer[i];
#endif
}

void KeypadScreen::init(int computerType)
{
	keyStart_ = 0;
	keyEnd_ = 0;
	lastKey_ = 0;
	forceUpperCase_ = p_Main->getUpperCase(ELF);

	wxClientDC dc(this);
	wxString buttonText;
	int x, y;

	mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/hexpad.png", wxBITMAP_TYPE_PNG);

	for (int i=0;i<2;i++) 
	{
		ledPointer[i] = new Led(dc, 20+96*(1-i),174, computerType);
		updateLed_[i] = true;
	}
	ledPointer[1]->setStatus(dc, 1);

#if defined (__WXMAC__)
    osx_push_inButtonPointer = new HexButton(dc, ELF_HEX_BUTTON, 8, 11, "IN");
    for (int i=0; i<16; i++)
    {
        buttonText.Printf("%01X", i);
        x = 8+(i&0x3)*32;
        y = 139 -(int)i/4*32;
        osx_buttonPointer[i] = new HexButton(dc, ELF_HEX_BUTTON, x, y, buttonText);
    }
#else
	push_inButtonPointer = new PushButton(this, 20, "IN", wxPoint(8, 11), wxSize(30, 30), 0);
	for (int i=0;i<16;i++)
	{
		buttonText.Printf("%01X", i);
		x = 8+(i&0x3)*32;
		y = 139 -(int)i/4*32;
		buttonPointer[i] = new PushButton(this, i, buttonText, wxPoint(x, y), wxSize(30, 30), 0);
	}
#endif
	this->connectKeyEvent(this);
}

void KeypadScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
	wxPaintDC dc(this);
	dc.DrawBitmap(*mainBitmapPointer, 0, 0);

	for (int i=0;i<2;i++) 
	{
		ledPointer[i]->onPaint(dc);
	}
#if defined (__WXMAC__)
    osx_push_inButtonPointer->onPaint(dc);
    for (int i = 0; i<16; i++)
        osx_buttonPointer[i]->onPaint(dc);
#endif
}

void KeypadScreen::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
#if defined (__WXMAC__)
    if (osx_push_inButtonPointer->onMouseRelease(dc, x, y))
        p_Computer->onInButtonRelease();
    for (int i = 0; i<16; i++)
        if (osx_buttonPointer[i]->onMouseRelease(dc, x, y))
            p_Computer->onNumberKeyUp();
#endif
}

void KeypadScreen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
#if defined (__WXMAC__)
    if (osx_push_inButtonPointer->onMousePress(dc, x, y))
        p_Computer->onInButtonPress();
    
    for (int i = 0; i<16; i++)
    {
        if (osx_buttonPointer[i]->onMousePress(dc, x, y))
        {
            p_Computer->onNumberKeyDown(i);
        }
    }
#endif
}

void KeypadScreen::releaseButtonOnScreen(HexButton* buttonPoint)
{
    wxClientDC dc(this);
    
    buttonPoint->releaseButtonOnScreen(dc);
}

BEGIN_EVENT_TABLE(Keypad, wxFrame)
	EVT_CLOSE (Keypad::onClose)
	EVT_COMMAND(20, wxEVT_ButtonDownEvent, Keypad::onButtonPress)
	EVT_COMMAND(20, wxEVT_ButtonUpEvent, Keypad::onButtonRelease)
	EVT_COMMAND(0, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(1, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(2, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(3, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(4, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(5, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(6, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(7, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(8, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(9, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(10, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(11, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(12, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(13, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(14, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(15, wxEVT_ButtonDownEvent, Keypad::onNumberKeyDown)
	EVT_COMMAND(0, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(1, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(2, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(3, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(4, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(5, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(6, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(7, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(8, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(9, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(10, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(11, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(12, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(13, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(14, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
	EVT_COMMAND(15, wxEVT_ButtonUpEvent, Keypad::onNumberKeyUp)
END_EVENT_TABLE()

Keypad::Keypad(const wxString& title, const wxPoint& pos, const wxSize& size, int computerType)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	computerType_ = computerType;

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	keypadScreenPointer = new KeypadScreen(this, size);
	keypadScreenPointer->init(computerType);
	keypadScreenPointer->setLed(1, 1);
	keypadScreenPointer->setLed(0, 0);

	keypadValue_ = 0;
	nextNybble_ = 'H';
	this->SetClientSize(size);
}

Keypad::~Keypad()
{
	delete keypadScreenPointer;
}

void Keypad::onClose(wxCloseEvent&WXUNUSED(event))
{
	p_Computer->removeElfHex();
	Destroy();
}

void Keypad::onNumberKeyDown(wxCommandEvent&event)
{
    int i = event.GetId();
    onNumberKeyDown(i);
}

void Keypad::onNumberKeyDown(int i)
{
    wxClientDC dc(this);
    
    keypadValue_= (nextNybble_ == 'H')?(keypadValue_&15)+(i<<4):(keypadValue_&0xf0)+i;
    if (nextNybble_ == 'H')
    {
        nextNybble_ = 'L';
        keypadScreenPointer->setLed(1, 0);
        keypadScreenPointer->setLed(0, 1);
    }
    else
    {
        nextNybble_ = 'H';
        keypadScreenPointer->setLed(1, 1);
        keypadScreenPointer->setLed(0, 0);
    }
    p_Computer->onHexDown(i);
}

void Keypad::onNumberDown(int hex)
{
	wxClientDC dc(this);

	keypadValue_= (nextNybble_ == 'H')?(keypadValue_&15)+(hex<<4):(keypadValue_&0xf0)+hex;
	if (nextNybble_ == 'H')
	{
		nextNybble_ = 'L';
		keypadScreenPointer->setLed(1, 0);
		keypadScreenPointer->setLed(0, 1);
	}
	else
	{
		nextNybble_ = 'H';
		keypadScreenPointer->setLed(1, 1);
		keypadScreenPointer->setLed(0, 0);
	}
}

void Keypad::onNumberKeyUp(wxCommandEvent&event)
{
    int i = event.GetId();
	p_Computer->onHexKeyUp(i);
}

void Keypad::onButtonRelease(wxCommandEvent& event)
{
	p_Computer->onInButtonRelease();
	event.Skip();
}

void Keypad::onButtonPress(wxCommandEvent& event)
{
	p_Computer->onInButtonPress(keypadValue_);
	event.Skip();
}

void Keypad::ledTimeout()
{
	keypadScreenPointer->ledTimeout();
}

void Keypad::setLedMs(long ms)
{
	keypadScreenPointer->setLedMs(ms);
}

Byte Keypad::getKey(Byte vtOut)
{
	return keypadScreenPointer->getKey(vtOut);
}

void Keypad::releaseButtonOnScreen(HexButton* buttonPointer)
{
    keypadScreenPointer->releaseButtonOnScreen(buttonPointer);
}

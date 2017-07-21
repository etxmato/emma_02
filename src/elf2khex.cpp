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

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
#include "app_icon.xpm"
#endif

#include "main.h"
#include "elf2khex.h"

Elf2KHexScreen::Elf2KHexScreen(wxWindow *parent, const wxSize& size)
: Panel(parent, size)
{
}

Elf2KHexScreen::~Elf2KHexScreen()
{
#if defined (__WXMAC__)
	for (int i = 0; i<16; i++)
		delete osx_buttonPointer[i];
	delete runSwitchButton;
	delete mpSwitchButton;
	delete loadSwitchButton;
	delete osx_text_resetButtonPointer;
	delete inSwitchButton;
#else
	for (int i = 0; i<16; i++)
		delete buttonPointer[i];
	delete runButtonPointer;
	delete mpButtonPointer;
	delete loadButtonPointer;
	delete resetButtonPointer;
	delete inButtonPointer;
	delete resetUpBitmapPointer;
	delete resetDownBitmapPointer;
	delete loadUpBitmapPointer;
	delete loadDownBitmapPointer;
	delete mpUpBitmapPointer;
	delete mpDownBitmapPointer;
	delete runUpBitmapPointer;
	delete runDownBitmapPointer;
	delete inUpBitmapPointer;
	delete inDownBitmapPointer;
#endif
}

void Elf2KHexScreen::init()
{
	keyStart_ = 0;
	keyEnd_ = 0;
	lastKey_ = 0;
	forceUpperCase_ = p_Main->getUpperCase(ELF2K);

	wxClientDC dc(this);
	wxString buttonText;
	int x, y;

	dc.SetBackground(wxBrush(wxColour(255,255,255)));
	dc.DrawRectangle(0, 0, 186, 126);

#ifndef __WXMAC__
	resetUpBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/resetButtonUp.png", wxBITMAP_TYPE_PNG);
	resetDownBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/resetButtonDown.png", wxBITMAP_TYPE_PNG);
	loadUpBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/loadButtonUp.png", wxBITMAP_TYPE_PNG);
	loadDownBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/loadButtonDown.png", wxBITMAP_TYPE_PNG);
	mpUpBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/mpButtonUp.png", wxBITMAP_TYPE_PNG);
	mpDownBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/mpButtonDown.png", wxBITMAP_TYPE_PNG);
	runUpBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/runButtonUp.png", wxBITMAP_TYPE_PNG);
	runDownBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/runButtonDown.png", wxBITMAP_TYPE_PNG);
	inUpBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/inButtonUp.png", wxBITMAP_TYPE_PNG);
	inDownBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/inButtonDown.png", wxBITMAP_TYPE_PNG);
#endif

	wxSize buttonSize;
	int offSetX, offSetY;

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	buttonSize.x = 30;
	buttonSize.y = 30;
	offSetX = -4;
	offSetY = -4;
#endif
#if defined (__WXMAC__)
    buttonSize.x = 30;
    buttonSize.y = 30;
    offSetX = 0;
    offSetY = 0;
#endif
#if defined (__WXMSW__)
    buttonSize.x = 30;
    buttonSize.y = 30;
    offSetX = 0;
    offSetY = 0;
#endif

#if wxCHECK_VERSION(2, 9, 0) 
#if defined (__WXMAC__)
	powerSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(0xbd, 0xb2, 0xa5), BUTTON_UP, 464, 42, "");
	runSwitchButton = new SwitchButton(dc, ELF2K_RUN_BUTTON, wxColour(0xbd, 0xb2, 0xa5), BUTTON_UP, 153 + offSetX, 63 + offSetY, "G");
	mpSwitchButton = new SwitchButton(dc, ELF2K_MP_BUTTON, wxColour(0xbd, 0xb2, 0xa5), BUTTON_UP, 123+offSetX, 63+offSetY, "M");
	loadSwitchButton = new SwitchButton(dc, ELF2K_LOAD_BUTTON, wxColour(0xbd, 0xb2, 0xa5), BUTTON_UP, 153+offSetX, 33+offSetY, "L");
    osx_text_resetButtonPointer = new HexButton(dc, ELF2K_RESET_BUTTON, 153+offSetX, 3+offSetY, "R");
	inSwitchButton = new SwitchButton(dc, ELF2K_IN_BUTTON, wxColour(0xbd, 0xb2, 0xa5), BUTTON_UP, 123+offSetX, 93+offSetY, "I");
#else
	runButtonPointer = new wxButton(this, 20, wxEmptyString, wxPoint(153 + offSetX, 63 + offSetY), buttonSize, wxBU_EXACTFIT);
	runButtonPointer->SetBitmap(*runUpBitmapPointer);
	mpButtonPointer = new wxButton(this, 21, wxEmptyString, wxPoint(123+offSetX, 63+offSetY), buttonSize, wxBU_EXACTFIT);
	mpButtonPointer->SetBitmap(*mpUpBitmapPointer);
	loadButtonPointer = new wxButton(this, 22, wxEmptyString, wxPoint(153+offSetX, 33+offSetY), buttonSize, wxBU_EXACTFIT);
	loadButtonPointer->SetBitmap(*loadUpBitmapPointer);
	resetButtonPointer = new wxButton(this, 23, wxEmptyString, wxPoint(153+offSetX, 3+offSetY), buttonSize, wxBU_EXACTFIT);
	resetButtonPointer->SetBitmap(*resetUpBitmapPointer);
	inButtonPointer = new wxButton(this, 24, wxEmptyString, wxPoint(123+offSetX, 93+offSetY), buttonSize, wxBU_EXACTFIT);
	inButtonPointer->SetBitmap(*inUpBitmapPointer);
#endif
#else
	runButtonPointer = new wxBitmapButton(this, 20, *runUpBitmapPointer, wxPoint(153+offSetX, 63+offSetY), buttonSize, wxBU_EXACTFIT);
	mpButtonPointer = new wxBitmapButton(this, 21, *mpUpBitmapPointer, wxPoint(123+offSetX, 63+offSetY+), buttonSize, wxBU_EXACTFIT);
	loadButtonPointer = new wxBitmapButton(this, 22, *loadUpBitmapPointer, wxPoint(153+offSetX, 33+offSetY), buttonSize, wxBU_EXACTFIT);
	resetButtonPointer = new wxBitmapButton(this, 23, *resetUpBitmapPointer, wxPoint(153+offSetX, 3+offSetY), buttonSize, wxBU_EXACTFIT);
	inButtonPointer = new wxBitmapButton(this, 24, *inUpBitmapPointer, wxPoint(123+offSetX, 93+offSetY), buttonSize, wxBU_EXACTFIT);
#endif

#if defined (__WXMAC__)
	for (int i = 0; i<16; i++)
	{
		buttonText.Printf("%01X", i);
		x = 3+(i&0x3)*30;
		y = 93 -(int)i/4*30;
		osx_buttonPointer[i] = new HexButton(dc, PANEL_HEX_BUTTON, x, y, buttonText);
	}
#else
	for (int i = 0; i<16; i++)
	{
		buttonText.Printf("%01X", i);
		x = 3+(i&0x3)*30;
		y = 93 -(int)i/4*30;
		buttonPointer[i] = new wxButton(this, i, buttonText, wxPoint(x+offSetX, y+offSetY), buttonSize);
	}
#endif
	this->connectKeyEvent(this);
}

void Elf2KHexScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
	wxPaintDC dc(this);
	dc.SetBackground(wxBrush(wxColour(255,255,255)));
	dc.DrawRectangle(0, 0, 186, 126);
#if defined (__WXMAC__)
	for (int i = 0; i<16; i++)
		osx_buttonPointer[i]->onPaint(dc);
	runSwitchButton->onPaint(dc);
	mpSwitchButton->onPaint(dc);
	loadSwitchButton->onPaint(dc);
	osx_text_resetButtonPointer->onPaint(dc);
	inSwitchButton->onPaint(dc);
#endif
}

void Elf2KHexScreen::onMouseRelease(wxMouseEvent&event)
{
	int x, y;
	event.GetPosition(&x, &y);

	wxClientDC dc(this);

#if defined (__WXMAC__)
	for (int i = 0; i<16; i++)
		osx_buttonPointer[i]->onMouseRelease(dc, x, y);
	runSwitchButton->onMouseRelease(dc, x, y);
	mpSwitchButton->onMouseRelease(dc, x, y);
	loadSwitchButton->onMouseRelease(dc, x, y);
	osx_text_resetButtonPointer->onMouseRelease(dc, x, y);
	inSwitchButton->onMouseRelease(dc, x, y);
#endif
}

void Elf2KHexScreen::onMousePress(wxMouseEvent&event)
{
	int x, y;
	event.GetPosition(&x, &y);

	wxClientDC dc(this);

#if defined (__WXMAC__)
	for (int i = 0; i<16; i++)
	{
		if (osx_buttonPointer[i]->onMousePress(dc, x, y))
			p_Computer->onNumberKeyDown(i);
	}
	if (runSwitchButton->onMousePress(dc, x, y))
		p_Computer->onRun();
	if (mpSwitchButton->onMousePress(dc, x, y))
		p_Computer->onMpButton();
	if (loadSwitchButton->onMousePress(dc, x, y))
		p_Computer->onLoadButton();
	if (osx_text_resetButtonPointer->onMousePress(dc, x, y))
		p_Computer->onReset();
	if (inSwitchButton->onMousePress(dc, x, y))
		p_Computer->onElf2KInButton();
#endif
}

void Elf2KHexScreen::releaseButtonOnScreen(HexButton* buttonPoint)
{
	wxClientDC dc(this);

	buttonPoint->releaseButtonOnScreen(dc);
}

void Elf2KHexScreen::inUp()
{
#if defined (__WXMAC__)
    wxClientDC dc(this);
    inSwitchButton->setState(dc, BUTTON_UP);
#else
#if wxCHECK_VERSION(2, 9, 0)
	inButtonPointer->SetBitmap(*inUpBitmapPointer);
#else
	inButtonPointer->SetBitmapLabel(*inUpBitmapPointer);
#endif
#endif
}

void Elf2KHexScreen::inDown()
{
#if defined (__WXMAC__)
    wxClientDC dc(this);
    inSwitchButton->setState(dc, BUTTON_DOWN);
#else
#if wxCHECK_VERSION(2, 9, 0)
	inButtonPointer->SetBitmap(*inDownBitmapPointer);
#else
	inButtonPointer->SetBitmapLabel(*inDownBitmapPointer);
#endif
#endif
}

void Elf2KHexScreen::runUp()
{
#if defined (__WXMAC__)
    wxClientDC dc(this);
    runSwitchButton->setState(dc, BUTTON_UP);
#else
#if wxCHECK_VERSION(2, 9, 0)
	runButtonPointer->SetBitmap(*runUpBitmapPointer);
#else
	runButtonPointer->SetBitmapLabel(*runUpBitmapPointer);
#endif
#endif
}

void Elf2KHexScreen::runDown()
{
#if defined (__WXMAC__)
    wxClientDC dc(this);
    runSwitchButton->setState(dc, BUTTON_DOWN);
#else
#if wxCHECK_VERSION(2, 9, 0)
	runButtonPointer->SetBitmap(*runDownBitmapPointer);
#else
	runButtonPointer->SetBitmapLabel(*runDownBitmapPointer);
#endif
#endif
}

void Elf2KHexScreen::mpUp()
{
#if defined (__WXMAC__)
    wxClientDC dc(this);
    mpSwitchButton->setState(dc, BUTTON_UP);
#else
#if wxCHECK_VERSION(2, 9, 0)
	mpButtonPointer->SetBitmap(*mpUpBitmapPointer);
#else
	mpButtonPointer->SetBitmapLabel(*mpUpBitmapPointer);
#endif
#endif
}

void Elf2KHexScreen::mpDown()
{
#if defined (__WXMAC__)
    wxClientDC dc(this);
    mpSwitchButton->setState(dc, BUTTON_DOWN);
#else
#if wxCHECK_VERSION(2, 9, 0)
	mpButtonPointer->SetBitmap(*mpDownBitmapPointer);
#else
	mpButtonPointer->SetBitmapLabel(*mpDownBitmapPointer);
#endif
#endif
}

void Elf2KHexScreen::loadUp()
{
#if defined (__WXMAC__)
    wxClientDC dc(this);
    loadSwitchButton->setState(dc, BUTTON_UP);
#else
#if wxCHECK_VERSION(2, 9, 0)
	loadButtonPointer->SetBitmap(*loadUpBitmapPointer);
#else
	loadButtonPointer->SetBitmapLabel(*loadUpBitmapPointer);
#endif
#endif
}

void Elf2KHexScreen::loadDown()
{
#if defined (__WXMAC__)
    wxClientDC dc(this);
    loadSwitchButton->setState(dc, BUTTON_DOWN);
#else
#if wxCHECK_VERSION(2, 9, 0)
	loadButtonPointer->SetBitmap(*loadDownBitmapPointer);
#else
	loadButtonPointer->SetBitmapLabel(*loadDownBitmapPointer);
#endif
#endif
}

BEGIN_EVENT_TABLE(Elf2Khex, wxFrame)
	EVT_CLOSE (Elf2Khex::onClose)
	EVT_BUTTON(0, Elf2Khex::onNumberKey)
	EVT_BUTTON(1, Elf2Khex::onNumberKey)
	EVT_BUTTON(2, Elf2Khex::onNumberKey)
	EVT_BUTTON(3, Elf2Khex::onNumberKey)
	EVT_BUTTON(4, Elf2Khex::onNumberKey)
	EVT_BUTTON(5, Elf2Khex::onNumberKey)
	EVT_BUTTON(6, Elf2Khex::onNumberKey)
	EVT_BUTTON(7, Elf2Khex::onNumberKey)
	EVT_BUTTON(8, Elf2Khex::onNumberKey)
	EVT_BUTTON(9, Elf2Khex::onNumberKey)
	EVT_BUTTON(10, Elf2Khex::onNumberKey)
	EVT_BUTTON(11, Elf2Khex::onNumberKey)
	EVT_BUTTON(12, Elf2Khex::onNumberKey)
	EVT_BUTTON(13, Elf2Khex::onNumberKey)
	EVT_BUTTON(14, Elf2Khex::onNumberKey)
	EVT_BUTTON(15, Elf2Khex::onNumberKey)
	EVT_BUTTON(20, Elf2Khex::onRunButton)
	EVT_BUTTON(21, Elf2Khex::onMpButton)
	EVT_BUTTON(22, Elf2Khex::onLoadButton)
	EVT_BUTTON(23, Elf2Khex::onResetButton)
	EVT_BUTTON(24, Elf2Khex::onInButton)
	EVT_TIMER(950, Elf2Khex::onInButtonTimer)
END_EVENT_TABLE()

Elf2Khex::Elf2Khex(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif


	elf2KHexScreenPointer = new Elf2KHexScreen(this, size);
	elf2KHexScreenPointer->init();

	keypadValue_ = 0;
	nextNybble_ = 'H';
	this->SetClientSize(size);

	inButtonTimerPointer = new wxTimer(this, 950);
}

Elf2Khex::~Elf2Khex()
{
	delete elf2KHexScreenPointer;

	inButtonTimerPointer->Stop();
	delete inButtonTimerPointer;
}

void Elf2Khex::onClose(wxCloseEvent&WXUNUSED(event))
{
	p_Computer->removeElfHex();
	Destroy();
}

void Elf2Khex::onNumberKey(wxCommandEvent&event)
{
	int i = event.GetId();
	onNumberKeyDown(i);
}

void Elf2Khex::onNumberKeyDown(int i)
{
	wxClientDC dc(this);

	keypadValue_= (nextNybble_ == 'H')?(keypadValue_&15)+(i<<4):(keypadValue_&0xf0)+i;
	if (nextNybble_ == 'H')
	{
		nextNybble_ = 'L';
	}
	else
	{
		nextNybble_ = 'H';
	}
	elf2KHexScreenPointer->inDown();
}

Byte Elf2Khex::getData()
{
	return keypadValue_;
}

void Elf2Khex::onInButton(wxCommandEvent&WXUNUSED(event))
{
	onElf2KInButton();
}

void Elf2Khex::onElf2KInButton()
{
	inButtonTimerPointer->Start(50, wxTIMER_ONE_SHOT );
	p_Computer->onInButtonPress(keypadValue_);
}

void Elf2Khex::onInButtonTimer(wxTimerEvent&WXUNUSED(event))
{
	p_Computer->onInButtonRelease();
}

void Elf2Khex::onRunButton(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->onRun();
}

void Elf2Khex::onLoadButton(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->onLoadButton();
}

void Elf2Khex::onMpButton(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->onMpButton();
}

void Elf2Khex::setRunButtonUp(int up)
{
	if (up == 1)
		elf2KHexScreenPointer->runDown();
	else
		elf2KHexScreenPointer->runUp();
}

void Elf2Khex::setInButtonUp(bool up)
{
	if (up)
		elf2KHexScreenPointer->inUp();
	else
		elf2KHexScreenPointer->inDown();
}

void Elf2Khex::setLoadButtonUp(int up)
{
	if (up)
		elf2KHexScreenPointer->loadUp();
	else
		elf2KHexScreenPointer->loadDown();
}

void Elf2Khex::setMpButtonUp(int up)
{
	if (up)
		elf2KHexScreenPointer->mpUp();
	else
		elf2KHexScreenPointer->mpDown();
}

void Elf2Khex::onResetButton(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->onReset();
}

Byte Elf2Khex::getKey(Byte vtOut)
{
	return elf2KHexScreenPointer->getKey(vtOut);
}

void Elf2Khex::releaseButtonOnScreen(HexButton* buttonPointer)
{
	elf2KHexScreenPointer->releaseButtonOnScreen(buttonPointer);
}


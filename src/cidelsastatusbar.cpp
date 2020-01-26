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

#include "main.h"
#include "cidelsastatusbar.h"
#include "pushbutton.h"

BEGIN_EVENT_TABLE(CidelsaStatusBar, wxStatusBar)
	EVT_COMMAND(0, wxEVT_ButtonDownEvent, CidelsaStatusBar::onButtonPressA)
	EVT_COMMAND(0, wxEVT_ButtonUpEvent, CidelsaStatusBar::onButtonReleaseA)
	EVT_COMMAND(1, wxEVT_ButtonDownEvent, CidelsaStatusBar::onButtonPressB)
	EVT_COMMAND(1, wxEVT_ButtonUpEvent, CidelsaStatusBar::onButtonReleaseB)
	EVT_COMMAND(2, wxEVT_ButtonDownEvent, CidelsaStatusBar::onButtonPress1)
	EVT_COMMAND(2, wxEVT_ButtonUpEvent, CidelsaStatusBar::onButtonRelease1)
	EVT_COMMAND(3, wxEVT_ButtonDownEvent, CidelsaStatusBar::onButtonPress2)
	EVT_COMMAND(3, wxEVT_ButtonUpEvent, CidelsaStatusBar::onButtonRelease2)
	EVT_COMMAND(4, wxEVT_ButtonDownEvent, CidelsaStatusBar::onButtonPressFire)
	EVT_COMMAND(4, wxEVT_ButtonUpEvent, CidelsaStatusBar::onButtonReleaseFire)
END_EVENT_TABLE()

CidelsaStatusBar::CidelsaStatusBar(wxWindow *parent)
: wxStatusBar(parent, wxID_ANY, 0)
{
	ledOffPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/comxledoff.png", wxBITMAP_TYPE_PNG);
	ledOnPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/comxledon.png", wxBITMAP_TYPE_PNG);

    ledsDefined_ = false;
    linux_led_pos_y_ = p_Main->getBarLedPosY();

    for (int i=0; i<5; i++)
		status_[i] = false;
}

CidelsaStatusBar::~CidelsaStatusBar()
{
	delete ledOffPointer;
	delete ledOnPointer;
	deleteBitmaps();
}

void CidelsaStatusBar::onButtonPress1(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->keyDown('1');
}

void CidelsaStatusBar::onButtonRelease1(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->keyUp('1');
	p_Video->focus();
}

void CidelsaStatusBar::onButtonPress2(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->keyDown('2');
}

void CidelsaStatusBar::onButtonRelease2(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->keyUp('2');
	p_Video->focus();
}

void CidelsaStatusBar::onButtonPressA(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->keyDown('A');
}

void CidelsaStatusBar::onButtonReleaseA(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->keyUp('A');
	p_Video->focus();
}

void CidelsaStatusBar::onButtonPressB(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->keyDown('B');
}

void CidelsaStatusBar::onButtonReleaseB(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->keyUp('B');
	p_Video->focus();
}

void CidelsaStatusBar::onButtonPressFire(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->keyDown(WXK_SPACE);
	p_Video->focus();
}

void CidelsaStatusBar::onButtonReleaseFire(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->keyUp(WXK_SPACE);
	p_Video->focus();
}

void CidelsaStatusBar::initCidelsaBar()
{
	SetFieldsCount(5);
	displayLeds();
	displayText();
}

void CidelsaStatusBar::updateLedStatus(int number, bool status)
{
	if (status)
	{
		if (!status_[number])
		{
#if wxCHECK_VERSION(2, 9, 0)
			ledBitmapPointers [number]->SetBitmap(*ledOnPointer);
#else
			ledBitmapPointers [number]->SetBitmapLabel(*ledOnPointer);
#endif
		}
	}
	else
	{
		if (status_[number])
		{
#if wxCHECK_VERSION(2, 9, 0)
			ledBitmapPointers [number]->SetBitmap(*ledOffPointer);
#else
			ledBitmapPointers [number]->SetBitmapLabel(*ledOffPointer);
#endif
		}
	}
	status_[number] = status;
}

void CidelsaStatusBar::displayText()
{
#if defined(__linux__)
    wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    SetFont(defaultFont);
#endif
	wxRect rect;
	this->GetFieldRect (1, rect);

	wxString leader;
#if defined(__linux__)
	leader = "     ";
#else
#if defined(__WXMAC__)
	leader = "     ";
#else
	leader = "     ";
#endif
#endif

	if (rect.GetWidth() < 70)
	{
		SetStatusText(leader + "S A", 0);
		SetStatusText(leader + "S B", 1);
		SetStatusText(leader + "1 P", 2);
		SetStatusText(leader + "2 P", 3);
		SetStatusText(leader + "Fire", 4);
	}
	else
	{
		SetStatusText(leader + "Shute A", 0);
		SetStatusText(leader + "Shute B", 1);
		SetStatusText(leader + "1 Player", 2);
		SetStatusText(leader + "2 Players", 3);
		SetStatusText(leader + "Fire", 4);
	}
}

void CidelsaStatusBar::displayLeds()
{
	deleteBitmaps();
	wxRect rect;
	this->GetFieldRect (1, rect);

	for (int number = 0; number < 5; number++)
	{
#if defined(__linux__)
        ledBitmapPointers [number] = new PushBitmapButton(this, number, *ledOnPointer,
							         wxPoint(number*(rect.GetWidth()+1)+3+number*3, linux_led_pos_y_), wxSize(-1, -1),
							         wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMAC__)
        ledBitmapPointers [number] = new PushBitmapButton(this, number, *ledOnPointer,
                                                          wxPoint(number*(rect.GetWidth()+1)+3+number*3, 4), wxSize(-1, -1),
                                                          wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMSW__)
		ledBitmapPointers [number] = new PushButton(this, number, wxEmptyString, 
							         wxPoint(number*(rect.GetWidth()+1)+3, 9), wxSize(CIDELSA_LED_SIZE_X, CIDELSA_LED_SIZE_Y),
							         wxBORDER_NONE);
#endif
		if (status_[number])
#if wxCHECK_VERSION(2, 9, 0)
			ledBitmapPointers [number]->SetBitmap(*ledOnPointer);
#else
			ledBitmapPointers [number]->SetBitmapLabel(*ledOnPointer);
#endif
		else
#if wxCHECK_VERSION(2, 9, 0)
			ledBitmapPointers [number]->SetBitmap(*ledOffPointer);
#else
			ledBitmapPointers [number]->SetBitmapLabel(*ledOffPointer);
#endif
	}
	ledsDefined_ = true;
}

void CidelsaStatusBar::deleteBitmaps()
{
	if (!ledsDefined_)  return;
	for (int number = 0; number <5; number++)
	{
		delete ledBitmapPointers [number];
	}
	ledsDefined_ = false;
}

void CidelsaStatusBar::reDrawBar()
{
	displayText();
	deleteBitmaps();
	displayLeds();
}

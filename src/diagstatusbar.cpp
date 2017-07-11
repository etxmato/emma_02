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
#include "diagstatusbar.h"
#include "pushbutton.h"

BEGIN_EVENT_TABLE(DiagStatusBar, wxStatusBar)
	EVT_COMMAND(1, wxEVT_ButtonDownEvent, DiagStatusBar::onStatusLed)
	EVT_COMMAND(3, wxEVT_ButtonDownEvent, DiagStatusBar::onStatusLed)
	EVT_COMMAND(5, wxEVT_ButtonDownEvent, DiagStatusBar::onStatusLed)
	EVT_COMMAND(7, wxEVT_ButtonDownEvent, DiagStatusBar::onStatusLed)
	EVT_COMMAND(0, wxEVT_ButtonDownEvent, DiagStatusBar::onSlotLed)
	EVT_COMMAND(2, wxEVT_ButtonDownEvent, DiagStatusBar::onSlotLed)
	EVT_COMMAND(4, wxEVT_ButtonDownEvent, DiagStatusBar::onSlotLed)
	EVT_COMMAND(6, wxEVT_ButtonDownEvent, DiagStatusBar::onSlotLed)
END_EVENT_TABLE()

DiagStatusBar::DiagStatusBar(wxWindow *parent)
: wxStatusBar(parent, wxID_ANY, 0)
{
    ledOffPointer = new wxBitmap(p_Main->getApplicationDir() + "images/ledoff.png", wxBITMAP_TYPE_PNG);
    ledOnPointer = new wxBitmap(p_Main->getApplicationDir() + "images/ledon.png", wxBITMAP_TYPE_PNG);
    ledDisabledPointer = new wxBitmap(p_Main->getApplicationDir() + "images/ledoff.png", wxBITMAP_TYPE_PNG);
	ledsDefined_ = false;
	statusLedUpdate_ = true;
	slotLedUpdate_ = true;
}

DiagStatusBar::~DiagStatusBar()
{
	delete ledOffPointer;
	delete ledOnPointer;
	delete ledDisabledPointer;
	deleteBitmaps();
}

void DiagStatusBar::initComxBar(bool expansionRomLoaded, int expansionTypeCard0)
{
	expansionRomLoaded_ = expansionRomLoaded;
	expansionTypeCard0_ = expansionTypeCard0;
	SetFieldsCount(5);
	displayLeds();
	displayText();
}

void DiagStatusBar::updateLedStatus(int card, int i, bool status)
{
	if (!statusLedUpdate_ && (i == 1))
		return;

	if (i == 0)
		ledStatus_[card] = status;

	if (!slotLedUpdate_ && (i == 0))
		return;

	if (status)
	{
#if wxCHECK_VERSION(2, 9, 0)
		ledBitmapPointers [card][i]->SetBitmap(*ledOnPointer);
#else
		ledBitmapPointers [card][i]->SetBitmapLabel(*ledOnPointer);
#endif
	}
	else
	{
#if wxCHECK_VERSION(2, 9, 0)
		ledBitmapPointers [card][i]->SetBitmap(*ledOffPointer);
#else
		ledBitmapPointers [card][i]->SetBitmapLabel(*ledOffPointer);
#endif
	}
}

void DiagStatusBar::displayText()
{
	wxRect rect;
	this->GetFieldRect (1, rect);
	if (expansionRomLoaded_)
	{
		if (rect.GetWidth() < 80)
			SetStatusText("E-Box", 0);
		else
			SetStatusText("Expansion Box", 0);
	}
	else
	{
		if (expansionTypeCard0_ != COMXEMPTY)
		{
			if (rect.GetWidth() < 80)
				SetStatusText("E-Card", 0);
			else
				SetStatusText("Expansion Card", 0);
		}
	}
}

void DiagStatusBar::displayLeds()
{
	deleteBitmaps();

	wxRect rect;
	int number = 0;
	this->GetFieldRect (1, rect);

	for (int card = 0; card < 4; card++)
	{
		for (int i = 0; i < 2; i++)
		{
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
			ledBitmapPointers [card][i] = new PushBitmapButton(this, number, *ledOffPointer,
							         wxPoint((card+1)*((int)rect.GetWidth()+1)+i*14+16+(card*3), 4), wxSize(-1, -1), // check 2 on ubuntu
							         wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#else
#if defined(__WXMAC__)
			ledBitmapPointers [card][i] = new PushBitmapButton(this, number, *ledOffPointer,
							         wxPoint((card+1)*((int)rect.GetWidth()+1)+i*14+19+(card*3), 4), wxSize(-1, -1), 
							         wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#else
			ledBitmapPointers [card][i] = new PushButton(this, number, wxEmptyString,
							         wxPoint((card+1)*(rect.GetWidth()+1)+i*14+16, 9), wxSize(DIAG_LED_SIZE_X, DIAG_LED_SIZE_Y),
							         wxBORDER_NONE);
			ledBitmapPointers [card][i]->SetBitmap(*ledOffPointer);
#endif
#endif
			if (i == 1)
			{
				if (statusLedUpdate_)
#if wxCHECK_VERSION(2, 9, 0)
					ledBitmapPointers [card][i]->SetBitmap(*ledOffPointer);
#else
					ledBitmapPointers [card][1]->SetBitmapLabel(*ledOffPointer);
#endif
				else
#if wxCHECK_VERSION(2, 9, 0)
					ledBitmapPointers [card][i]->SetBitmap(*ledDisabledPointer);
#else
					ledBitmapPointers [card][1]->SetBitmapLabel(*ledDisabledPointer);
#endif
			}
			else
			{
				ledStatus_[card] = false;
				if (slotLedUpdate_)
#if wxCHECK_VERSION(2, 9, 0)
					ledBitmapPointers [card][i]->SetBitmap(*ledOffPointer);
#else
					ledBitmapPointers [card][1]->SetBitmapLabel(*ledOffPointer);
#endif
				else
#if wxCHECK_VERSION(2, 9, 0)
					ledBitmapPointers [card][i]->SetBitmap(*ledDisabledPointer);
#else
					ledBitmapPointers [card][1]->SetBitmapLabel(*ledDisabledPointer);
#endif
			}
			if (p_Computer->getComxExpansionType(card) == COMXEMPTY)
				ledBitmapPointers[card][i]->Hide();
			if ((!expansionRomLoaded_) &&(card >0))
				ledBitmapPointers[card][i]->Hide();
			number++;
		}
	}
	ledsDefined_ = true;
}

void DiagStatusBar::deleteBitmaps()
{
	if (!ledsDefined_)  return;
	for (int card = 0; card < 4; card++)
	{
		for (int i = 0; i < 2; i++)
		{
			delete ledBitmapPointers [card][i];
		}
	}
	ledsDefined_ = false;
}

void DiagStatusBar::reDrawBar()
{
	displayText();
	updateStatusBarText();
	displayLeds();
}

void DiagStatusBar::onStatusLed(wxCommandEvent&WXUNUSED(event))
{
	statusLedUpdate_ = !statusLedUpdate_;
	p_Main->setStatusLedUpdate(statusLedUpdate_);
	if (statusLedUpdate_)
	{
		for (int slot = 0; slot < 4; slot++)
		{
#if wxCHECK_VERSION(2, 9, 0)
			ledBitmapPointers [slot][1]->SetBitmap(*ledOffPointer);
#else
			ledBitmapPointers [slot][1]->SetBitmapLabel(*ledOffPointer);
#endif
		}
	}
	else
	{
		for (int slot = 0; slot < 4; slot++)
		{
#if wxCHECK_VERSION(2, 9, 0)
			ledBitmapPointers [slot][1]->SetBitmap(*ledDisabledPointer);
#else
			ledBitmapPointers [slot][1]->SetBitmapLabel(*ledDisabledPointer);
#endif
		}

	}
}

void DiagStatusBar::onSlotLed(wxCommandEvent&WXUNUSED(event))
{
	slotLedUpdate_ = !slotLedUpdate_;
	p_Main->setSlotLedUpdate(slotLedUpdate_);
	if (slotLedUpdate_)
	{
		for (int slot = 0; slot < 4; slot++)
		{
			if (ledStatus_[slot])
			{
#if wxCHECK_VERSION(2, 9, 0)
				ledBitmapPointers [slot][0]->SetBitmap(*ledOnPointer);
#else
				ledBitmapPointers [slot][0]->SetBitmapLabel(*ledOnPointer);
#endif
			}
			else
			{
#if wxCHECK_VERSION(2, 9, 0)
				ledBitmapPointers [slot][0]->SetBitmap(*ledOffPointer);
#else
				ledBitmapPointers [slot][0]->SetBitmapLabel(*ledOffPointer);
#endif
			}
		}
	}
	else
	{
		for (int slot = 0; slot < 4; slot++)
		{
#if wxCHECK_VERSION(2, 9, 0)
			ledBitmapPointers [slot][0]->SetBitmap(*ledDisabledPointer);
#else
			ledBitmapPointers [slot][0]->SetBitmapLabel(*ledDisabledPointer);
#endif
		}

	}
}

void DiagStatusBar::updateStatusBarText()
{
	wxString buf;
	wxString leader;

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
	leader = "%d:         ";
#else
	leader = "%d:           ";
#endif

	wxRect rect;
	this->GetFieldRect (1, rect);

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	SetFont(defaultFont);
#endif
#if defined(__WXMAC__)
	wxFont defaultFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	SetFont(defaultFont);
#endif

	for (int slot = 0; slot < 4; slot++)
	{
		if (!((slot > 0) && !expansionRomLoaded_))
		{
			switch(p_Computer->getComxExpansionType(slot))
			{
				case COMXRAM:
					if (rect.GetWidth() < 70)
						buf.Printf("%d:", slot+1);
					else
						if (rect.GetWidth() < 100)
							buf.Printf(leader + "32K", slot+1);
						else
							buf.Printf(leader + "Ram 32K", slot+1);
					SetStatusText(buf, slot+1);
				break;

				case COMXFLOP:
					if (rect.GetWidth() < 70)
						buf.Printf("%d:", slot+1);
					else
						buf.Printf(leader + "FDC", slot+1);
					SetStatusText(buf, slot+1);
				break;

				case COMXDIAG:
					if (rect.GetWidth() < 70)
						buf.Printf("%d:", slot + 1);
					else
						buf.Printf(leader + "Diag", slot + 1);
					SetStatusText(buf, slot + 1);
				break;

				case COMXPRINTER:
					if (rect.GetWidth() < 70)
						buf.Printf("%d:", slot+1);
					else
						if (rect.GetWidth() < 90)
							buf.Printf(leader + "Pr", slot+1);
						else
							buf.Printf(leader + "Printer", slot+1);
					SetStatusText(buf, slot+1);
				break;

				case COMXRS232:
					if (rect.GetWidth() < 70)
						buf.Printf("%d:", slot+1);
					else
						if (rect.GetWidth() < 90)
							buf.Printf(leader + "232", slot+1);
						else
							buf.Printf(leader + "RS232", slot+1);
					SetStatusText(buf, slot+1);
				break;

				case COMXTHPRINTER:
					if (rect.GetWidth() < 70)
						buf.Printf("%d:", slot+1);
					else
						if (rect.GetWidth() < 90)
							buf.Printf(leader + "Th", slot+1);
						else
							if (rect.GetWidth() < 130)
								buf.Printf(leader + "Thermal", slot+1);
							else
								buf.Printf(leader + "Thermal Printer", slot+1);
					SetStatusText(buf, slot+1);
				break;

				case COMX80COLUMN:
					if (rect.GetWidth() < 70)
						buf.Printf("%d:", slot+1);
					else
						if (rect.GetWidth() < 90)
							buf.Printf(leader + "80", slot+1);
						else
							if (rect.GetWidth() < 110)
								buf.Printf(leader + "80 COL", slot+1);
							else
								buf.Printf(leader + "80 Column", slot+1);
					SetStatusText(buf, slot+1);
				break;

				case COMXJOY:
					if (rect.GetWidth() < 70)
						buf.Printf("%d:", slot+1);
					else
						if (rect.GetWidth() < 95)
							buf.Printf(leader + "Joy", slot+1);
						else
							if (rect.GetWidth() < 120)
								buf.Printf(leader + "JoyCard", slot+1);
							else
								buf.Printf(leader + "F&M JoyCard", slot+1);
					SetStatusText(buf, slot+1);
				break;
				
				case COMXEPROMBOARD:
					if (rect.GetWidth() < 70)
						buf.Printf("%d:", slot+1);
					else
						if (rect.GetWidth() < 95)
							buf.Printf(leader + "E-B", slot+1);
						else
							if (rect.GetWidth() < 120)
								buf.Printf(leader + "Eprom-B", slot+1);
							else
								if (rect.GetWidth() < 150)
									buf.Printf(leader + "Eprom Board", slot+1);
								else
									buf.Printf(leader + "F&M Eprom Board", slot+1);
					SetStatusText(buf, slot+1);
				break;

				case NETWORK:
					if (rect.GetWidth() < 70)
						buf.Printf("%d:", slot+1);
					else
						if (rect.GetWidth() < 95)
							buf.Printf(leader + "NW", slot+1);
						else
							buf.Printf(leader + "Network", slot+1);
					SetStatusText(buf, slot+1);
				break;

				case COMXSUPERBOARD:
					if (rect.GetWidth() < 70)
						buf.Printf("%d:", slot+1);
					else
						if (rect.GetWidth() < 95)
							buf.Printf(leader + "USB", slot+1);
						else
							if (rect.GetWidth() < 120)
								buf.Printf(leader + "USB-B", slot+1);
							else
								buf.Printf(leader + "USB Board", slot+1);
					SetStatusText(buf, slot+1);
				break;
			}
		}
	}
}

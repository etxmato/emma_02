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

DiagStatusBar::DiagStatusBar(wxWindow *parent)
: wxStatusBar(parent, wxID_ANY, 0)
{
    wxBitmap *ledOffBitmap;
    wxBitmap *ledOnBitmap;
    
    ledOffBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/diagledoff.png", wxBITMAP_TYPE_PNG);
    ledOnBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/diagledon.png", wxBITMAP_TYPE_PNG);
	ledsDefined_ = false;

    wxColour maskColour(255, 0, 255);
    
    maskLedOff = new wxMask (*ledOffBitmap, maskColour);
    ledOffBitmap->SetMask(maskLedOff);
    maskLedOn = new wxMask (*ledOnBitmap, maskColour);
    ledOnBitmap->SetMask(maskLedOn);
    
    ledOffPointer = new wxBitmap(ledOffBitmap->GetWidth(), ledOffBitmap->GetHeight());
    ledOnPointer = new wxBitmap(ledOnBitmap->GetWidth(), ledOnBitmap->GetHeight());
    
    wxMemoryDC memDC(*ledOffPointer);
    
    memDC.SetBackground(*wxTheBrushList->FindOrCreateBrush(wxColour(236, 236, 236)));
    memDC.Clear();
    memDC.DrawBitmap(*ledOffBitmap, 0, 0, true);
    memDC.SelectObject(wxNullBitmap);
    
    memDC.SelectObject(*ledOnPointer);
    memDC.SetBackground(*wxTheBrushList->FindOrCreateBrush(wxColour(236, 236, 236)));
    memDC.Clear();
    memDC.DrawBitmap(*ledOnBitmap, 0, 0, true);
    memDC.SelectObject(wxNullBitmap);
    
    delete ledOffBitmap;
    delete ledOnBitmap;

    for (int i=0; i<NUMBER_OF_DIAG_LEDS; i++)
        ledStatus_[i] = false;
}

DiagStatusBar::~DiagStatusBar()
{
	delete ledOffPointer;
	delete ledOnPointer;
    deleteBitmaps();
}

void DiagStatusBar::initDiagBar()
{
	SetFieldsCount(NUMBER_OF_DIAG_LEDS);
	displayLeds();
	displayText();
}

void DiagStatusBar::updateLedStatus(int led, bool status)
{
    if (!ledsDefined_)
        return;
    
    if (status == ledStatus_[led])
        return;
    
	ledStatus_[led] = status;

	if (status)
	{
#if wxCHECK_VERSION(2, 9, 0)
		ledPointer [led]->SetBitmap(*ledOnPointer);
#else
		ledPointer [led]->SetBitmapLabel(*ledOnPointer);
#endif
	}
	else
	{
#if wxCHECK_VERSION(2, 9, 0)
		ledPointer [led]->SetBitmap(*ledOffPointer);
#else
		ledPointer [led]->SetBitmapLabel(*ledOffPointer);
#endif
	}
}

void DiagStatusBar::displayText()
{
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
    wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    SetFont(defaultFont);
#endif
#if defined(__WXMAC__)
    wxFont defaultFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    SetFont(defaultFont);
#endif

    wxRect rect;
    this->GetFieldRect (1, rect);
    
    if (rect.GetWidth() < 70)
    {
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
        SetStatusText("    V++", 0);
        SetStatusText("    EXT", 1);
        SetStatusText("    DM", 2);
        SetStatusText("    INT", 3);
        SetStatusText("    I-A", 4);
        SetStatusText("    CLR", 5);
#else
        SetStatusText("     V++", 0);
        SetStatusText("     EXT", 1);
        SetStatusText("     DM", 2);
        SetStatusText("     INT", 3);
        SetStatusText("     I-A", 4);
        SetStatusText("     CLR", 5);
#endif
    }
    else
    {
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
        SetStatusText("    V++", 0);
        SetStatusText("    EXT ROM", 1);
        SetStatusText("    DMA ACK", 2);
        SetStatusText("    INTERRUPT", 3);
        SetStatusText("    INT ACK", 4);
        SetStatusText("    CLEAR", 5);
#else
        SetStatusText("     V++", 0);
        SetStatusText("     EXT ROM", 1);
        SetStatusText("     DMA ACK", 2);
        SetStatusText("     INTERRUPT", 3);
        SetStatusText("     INT ACK", 4);
        SetStatusText("     CLEAR", 5);
#endif
    }
}

void DiagStatusBar::displayLeds()
{
    ledsDefined_ = false;
    deleteBitmaps();

	wxRect rect;
	this->GetFieldRect (1, rect);

    for (int led = 0; led < NUMBER_OF_DIAG_LEDS; led++)
    {
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
        ledPointer [led] = new PushBitmapButton(this, led, *ledOffPointer, wxPoint(led*((int)rect.GetWidth()+1)+(led*3)+2, 2), wxSize(-1, -1), wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#else
        ledPointer [led] = new PushButton(this, led, wxEmptyString, wxPoint(led*((int)rect.GetWidth()+1)+led+2, 4), wxSize(DIAG_LED_SIZE_X, DIAG_LED_SIZE_Y), wxBORDER_NONE);
#endif
        if (ledStatus_[led])
#if wxCHECK_VERSION(2, 9, 0)
        ledPointer [led]->SetBitmap(*ledOnPointer);
#else
        ledPointer [led]->SetBitmapLabel(*ledOnPointer);
#endif
        else
#if wxCHECK_VERSION(2, 9, 0)
        ledPointer [led]->SetBitmap(*ledOffPointer);
#else
        ledPointer [led]->SetBitmapLabel(*ledOffPointer);
#endif
    }
    ledsDefined_ = true;
}

void DiagStatusBar::deleteBitmaps()
{
	if (!ledsDefined_)  return;
  	for (int led = 0; led < NUMBER_OF_DIAG_LEDS; led++)
	{
        delete ledPointer [led];
	}
    ledsDefined_ = false;
}

void DiagStatusBar::reDrawBar()
{
    displayText();
    displayLeds();
}
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
#include "vip2statusbar.h"
#include "pushbutton.h"

VipIIStatusBar::VipIIStatusBar(wxWindow *parent)
: wxStatusBar(parent, wxID_ANY, 0)
{
    wxString linuxExtension = "";
#if defined (__linux__)
    linuxExtension = "_linux";
#endif

    wxColour white(255, 255, 255);
	ledOffPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/ledoff.png", wxBITMAP_TYPE_PNG);
	ledOnPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/ledon.png", wxBITMAP_TYPE_PNG);
	maskOn = new wxMask(*ledOnPointer, white);
	maskOff = new wxMask(*ledOnPointer, white);
	ledOnPointer->SetMask(maskOn);
	ledOffPointer->SetMask(maskOff);
	ledsDefined_ = false;
    
    WindowInfo windowInfo = getWinSizeInfo();
    linux_led_pos_y_ = -1;
    
    if (windowInfo.operatingSystem == OS_LINUX_FEDORA)
        linux_led_pos_y_ = 4;
}

VipIIStatusBar::~VipIIStatusBar()
{
	delete ledOffPointer;
	delete ledOnPointer;
	deleteBitmaps();
}

void VipIIStatusBar::initVipIIBar()
{
	SetFieldsCount(3);
	for (int led = 0; led < 3; led++)
		ledStatus[led] = false;
	displayLeds();
	displayText();
}

void VipIIStatusBar::updateLedStatus(int led, bool status)
{
	if (!ledsDefined_)
		return;

	ledStatus[led] = status;
	if (status)
	{
#if wxCHECK_VERSION(2, 9, 0)
		ledBitmapPointers [led]->SetBitmap(*ledOnPointer);
#else
		ledBitmapPointers [led]->SetBitmapLabel(*ledOnPointer);
#endif
	}
	else
	{
#if wxCHECK_VERSION(2, 9, 0)
		ledBitmapPointers [led]->SetBitmap(*ledOffPointer);
#else
		ledBitmapPointers [led]->SetBitmapLabel(*ledOffPointer);
#endif
	}
}

void VipIIStatusBar::displayText()
{
#if defined(__linux__)
    wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    SetFont(defaultFont);
#endif
	SetStatusText("   RUN", 0);
	SetStatusText("       Q", 1);
	SetStatusText(" TAPE", 2);
}

void VipIIStatusBar::displayLeds()
{
	deleteBitmaps();

	wxRect rect;
	this->GetFieldRect (1, rect);

	for (int led = 0; led < 3; led++)
	{
#if defined(__linux__)
		if (ledStatus[led])
			ledBitmapPointers [led] = new PushBitmapButton(this, led, *ledOnPointer,
									 wxPoint(led*((int)rect.GetWidth()+1)+40+(led*3), linux_led_pos_y_), wxSize(-1, -1),
									 wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
		else
			ledBitmapPointers [led] = new PushBitmapButton(this, led, *ledOffPointer,
									 wxPoint(led*((int)rect.GetWidth()+1)+40+(led*3), linux_led_pos_y_), wxSize(-1, -1),
									 wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMAC__)
		if (ledStatus[led])
			ledBitmapPointers [led] = new PushBitmapButton(this, led, *ledOnPointer,
									 wxPoint(led*((int)rect.GetWidth()+1)+43+(led*3), 3), wxSize(-1, -1),
									 wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
		else
			ledBitmapPointers [led] = new PushBitmapButton(this, led, *ledOffPointer,
									 wxPoint(led*((int)rect.GetWidth()+1)+43+(led*3), 3), wxSize(-1, -1),
									 wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMSW__)
		ledBitmapPointers [led] = new PushButton(this, led, wxEmptyString,
								 wxPoint(led*(rect.GetWidth()+1)+40, 6), wxSize(12, 12), 
								 wxBORDER_NONE);

        if (ledStatus[led])
			ledBitmapPointers [led]->SetBitmap(*ledOnPointer);
		else
			ledBitmapPointers [led]->SetBitmap(*ledOffPointer);
#endif
	}
	ledsDefined_ = true;
}

void VipIIStatusBar::deleteBitmaps()
{
	if (!ledsDefined_)  return;
	for (int led = 0; led < 3; led++)
	{
		delete ledBitmapPointers [led];
	}
	ledsDefined_ = false;
}

void VipIIStatusBar::reDrawBar()
{
	displayText();
	displayLeds();
}

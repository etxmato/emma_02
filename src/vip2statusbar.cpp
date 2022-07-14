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
    
    leaderString_ = p_Main->getBarLeaderCidelsa();
    statusBarElementMeasure0_ = p_Main->getStatusBarElementMeasure(0);
    statusBarElementMeasure1_ = p_Main->getStatusBarElementMeasure(1);
    ledSpacing_ = p_Main->getBarLedSpacing();

    linux_led_pos_y_ = p_Main->getBarLedPosVip2Y();
}

VipIIStatusBar::~VipIIStatusBar()
{
    delete ledOffPointer;
    delete ledOnPointer;
    deleteBitmaps();
}

void VipIIStatusBar::initVipIIBar(bool runLed)
{
    SetFieldsCount(3);
    ledStatus[0] = runLed;
    for (int led = 1; led < 3; led++)
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
    wxRect rect;
    this->GetFieldRect (1, rect);

    if (rect.GetWidth() < statusBarElementMeasure1_)
    {
        SetStatusText(leaderString_ + "R", 0);
        SetStatusText(leaderString_ + "Q", 1);
        SetStatusText(leaderString_ + "T", 2);
    }
    else
    {
        SetStatusText(leaderString_ + "RUN", 0);
        SetStatusText(leaderString_ + "Q", 1);
        SetStatusText(leaderString_ + "TAPE", 2);
    }
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
                                     wxPoint(led*((int)rect.GetWidth()+ledSpacing_)+2+(led*3), linux_led_pos_y_), wxSize(-1, -1),
                                     wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
        else
            ledBitmapPointers [led] = new PushBitmapButton(this, led, *ledOffPointer,
                                     wxPoint(led*((int)rect.GetWidth()+ledSpacing_)+2+(led*3), linux_led_pos_y_), wxSize(-1, -1),
                                     wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMAC__)
        if (ledStatus[led])
            ledBitmapPointers [led] = new PushBitmapButton(this, led, *ledOnPointer,
                                     wxPoint(led*((int)rect.GetWidth()+ledSpacing_)+2+(led*3), 2), wxSize(-1, -1),
                                     wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
        else
            ledBitmapPointers [led] = new PushBitmapButton(this, led, *ledOffPointer,
                                     wxPoint(led*((int)rect.GetWidth()+ledSpacing_)+2+(led*3), 2), wxSize(-1, -1),
                                     wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMSW__)
        ledBitmapPointers [led] = new PushButton(this, led, wxEmptyString,
                                 wxPoint(led*(rect.GetWidth()+ledSpacing_)+2, 6), wxSize(12, 12), 
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

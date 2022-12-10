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
    wxString linuxExtension = "";
#if defined (__linux__)
    linuxExtension = "_linux";
#endif
    
    wxColour white(255, 0, 255);
    ledOffPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/diagledoff.png", wxBITMAP_TYPE_PNG);
    ledOnPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/diagledon.png", wxBITMAP_TYPE_PNG);
    maskOn = new wxMask(*ledOnPointer, white);
    maskOff = new wxMask(*ledOnPointer, white);
    ledOnPointer->SetMask(maskOn);
    ledOffPointer->SetMask(maskOff);
    ledsDefined_ = false;
    
    led_pos_y_ = p_Main->getBarLedPosDiagY();
    leaderString_ = p_Main->getBarLeaderCidelsa();
    statusBarElementMeasure0_ = p_Main->getStatusBarElementMeasure(0);
    statusBarElementMeasure1_ = p_Main->getStatusBarElementMeasure(1);
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
    for (int i=0; i<NUMBER_OF_DIAG_LEDS; i++)
        ledStatus_[i] = false;
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
#if defined(__linux__)
    wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    SetFont(defaultFont);
#endif
    wxRect rect;
    this->GetFieldRect (1, rect);
    
    if (rect.GetWidth() < statusBarElementMeasure0_)
    {
        SetStatusText("", 0);
        SetStatusText("", 1);
        SetStatusText("", 2);
        SetStatusText("", 3);
        SetStatusText("", 4);
        SetStatusText("", 5);
    }
    else
    {
        if (rect.GetWidth() < statusBarElementMeasure1_)
        {
            SetStatusText(leaderString_ + "V++", 0);
            SetStatusText(leaderString_ + "EXT", 1);
            SetStatusText(leaderString_ + "DM", 2);
            SetStatusText(leaderString_ + "INT", 3);
            SetStatusText(leaderString_ + "I-A", 4);
            SetStatusText(leaderString_ + "CLR", 5);
        }
        else
        {
            SetStatusText(leaderString_ + "V++", 0);
            SetStatusText(leaderString_ + "EXT ROM", 1);
            SetStatusText(leaderString_ + "DMA ACK", 2);
            SetStatusText(leaderString_ + "INT", 3);
            SetStatusText(leaderString_ + "INT ACK", 4);
            SetStatusText(leaderString_ + "CLEAR", 5);
        }
    }
}

void DiagStatusBar::displayLeds()
{
    deleteBitmaps();

    wxRect rect;
    this->GetFieldRect (1, rect);

    for (int led = 0; led < NUMBER_OF_DIAG_LEDS; led++)
    {
#if defined(__linux__)
       if (ledStatus_[led])
           ledPointer [led] = new PushBitmapButton(this, led, *ledOnPointer, wxPoint(led*((int)rect.GetWidth()+1)+(led*3)+2, led_pos_y_), wxSize(-1, -1), wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
        else
            ledPointer [led] = new PushBitmapButton(this, led, *ledOffPointer, wxPoint(led*((int)rect.GetWidth()+1)+(led*3)+2, led_pos_y_), wxSize(-1, -1), wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMAC__)
        if (ledStatus_[led])
            ledPointer [led] = new PushBitmapButton(this, led, *ledOnPointer, wxPoint(led*((int)rect.GetWidth()+1)+(led*3)+2, 2), wxSize(-1, -1), wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
        else
            ledPointer [led] = new PushBitmapButton(this, led, *ledOffPointer, wxPoint(led*((int)rect.GetWidth()+1)+(led*3)+2, 2), wxSize(-1, -1), wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMSW__)
        ledPointer [led] = new PushButton(this, led, wxEmptyString, wxPoint(led*((int)rect.GetWidth()+1)+led+2, 4), wxSize(DIAG_LED_SIZE_X, DIAG_LED_SIZE_Y), wxBORDER_NONE);

        if (ledStatus_[led])
            ledPointer [led]->SetBitmap(*ledOnPointer);
        else
            ledPointer [led]->SetBitmap(*ledOffPointer);
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

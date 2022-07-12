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
#include "comxstatusbar.h"
#include "pushbutton.h"

BEGIN_EVENT_TABLE(ComxStatusBar, wxStatusBar)
    EVT_COMMAND(1, wxEVT_ButtonDownEvent, ComxStatusBar::onStatusLed)
    EVT_COMMAND(3, wxEVT_ButtonDownEvent, ComxStatusBar::onStatusLed)
    EVT_COMMAND(5, wxEVT_ButtonDownEvent, ComxStatusBar::onStatusLed)
    EVT_COMMAND(7, wxEVT_ButtonDownEvent, ComxStatusBar::onStatusLed)
    EVT_COMMAND(0, wxEVT_ButtonDownEvent, ComxStatusBar::onSlotLed)
    EVT_COMMAND(2, wxEVT_ButtonDownEvent, ComxStatusBar::onSlotLed)
    EVT_COMMAND(4, wxEVT_ButtonDownEvent, ComxStatusBar::onSlotLed)
    EVT_COMMAND(6, wxEVT_ButtonDownEvent, ComxStatusBar::onSlotLed)
END_EVENT_TABLE()

ComxStatusBar::ComxStatusBar(wxWindow *parent)
: wxStatusBar(parent, wxID_ANY, 0)
{
    ledOffPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/comxledoff.png", wxBITMAP_TYPE_PNG);
    ledOnPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/comxledon.png", wxBITMAP_TYPE_PNG);
    ledDisabledPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/comxleddisabled.png", wxBITMAP_TYPE_PNG);

    ledsDefined_ = false;
    statusLedUpdate_ = true;
    slotLedUpdate_ = true;
        
    linux_led_pos_y_ = p_Main->getBarLedPosY();
    ledPosX1_ = p_Main->getBarLedPosX1();
    ledPosX2_ = p_Main->getBarLedPosX2();
    ledSpacing_ = p_Main->getBarLedSpacing();

    offsetField_ = 1;

    ledOffsetX_ = ledPosX2_;
    lastLedPosX_ = -1;

    leaderString_ = p_Main->getBarLeader();
    
    for (int i=0; i<5; i++)
        statusBarElementMeasure[i] = p_Main->getStatusBarElementMeasure(i);
}

ComxStatusBar::~ComxStatusBar()
{
    delete ledOffPointer;
    delete ledOnPointer;
    delete ledDisabledPointer;
    deleteBitmaps();
}

void ComxStatusBar::initComxBar(bool expansionRomLoaded, int expansionTypeCard0)
{
    expansionRomLoaded_ = expansionRomLoaded;
    expansionTypeCard0_ = expansionTypeCard0;
    SetFieldsCount(5);
    displayLeds();
    displayText();
}

void ComxStatusBar::updateLedStatus(int card, int i, bool status)
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

void ComxStatusBar::displayText()
{
#if defined(__linux__)
    wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    SetFont(defaultFont);
#endif
    wxRect rect;
    this->GetFieldRect (1, rect);
    if (expansionRomLoaded_)
    {
        if (rect.GetWidth() < statusBarElementMeasure[0])
            SetStatusText("EB", 0);
        else
        {
            if (rect.GetWidth() < statusBarElementMeasure[2])
                SetStatusText("E-Box", 0);
            else
                SetStatusText("Expansion Box", 0);
        }
    }
    else
    {
        if (expansionTypeCard0_ != COMXEMPTY)
        {
            if (rect.GetWidth() < statusBarElementMeasure[0])
                SetStatusText("EC", 0);
            else
            {
                if (rect.GetWidth() < statusBarElementMeasure[2])
                    SetStatusText("E-Card", 0);
                else
                    SetStatusText("Expansion Card", 0);
            }
        }
    }
}

void ComxStatusBar::displayLeds()
{
    deleteBitmaps();

    wxRect rect;
    int number = 0;
    this->GetFieldRect (1, rect);

    for (int card = 0; card < 4; card++)
    {
        for (int i = 0; i < 2; i++)
        {
#if defined(__linux__)
            ledBitmapPointers [card][i] = new PushBitmapButton(this, number, *ledOffPointer,
                                     wxPoint((card+offsetField_)*((int)rect.GetWidth()+ledSpacing_)+i*14+ledOffsetX_+(card*3), linux_led_pos_y_), wxSize(-1, -1),
                                     wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMAC__)
            ledBitmapPointers [card][i] = new PushBitmapButton(this, number, *ledOffPointer,
                                     wxPoint((card+offsetField_)*((int)rect.GetWidth()+ledSpacing_)+i*14+ledOffsetX_+(card*3), 4), wxSize(-1, -1),
                                     wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMSW__)
            ledBitmapPointers [card][i] = new PushButton(this, number, wxEmptyString,
                                     wxPoint((card+offsetField_)*(rect.GetWidth()+ledSpacing_)+i*14+ledOffsetX_, 9), wxSize(LED_SIZE_X, LED_SIZE_Y),
                                     wxBORDER_NONE);
            ledBitmapPointers [card][i]->SetBitmap(*ledOffPointer);
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

void ComxStatusBar::deleteBitmaps()
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

void ComxStatusBar::reDrawBar()
{
    displayText();
    updateStatusBarText();
    displayLeds();
}

void ComxStatusBar::onStatusLed(wxCommandEvent&WXUNUSED(event))
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

void ComxStatusBar::onSlotLed(wxCommandEvent&WXUNUSED(event))
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

void ComxStatusBar::updateStatusBarText()
{
    wxString buf;

    wxRect rect;
    this->GetFieldRect (1, rect);

#if defined(__linux__)
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
            if (rect.GetWidth() < statusBarElementMeasure[1])
            {
                if (rect.GetWidth() < statusBarElementMeasure[0])
                {
                    if (lastLedPosX_ != ledPosX1_)
                    {
                        lastLedPosX_ = ledPosX1_;
                        ledOffsetX_ = ledPosX1_;
                        offsetField_ = 1;
                        displayLeds();
                    }
                    buf = "";
                }
                else
                {
                    if (lastLedPosX_ != ledPosX2_)
                    {
                        lastLedPosX_ = ledPosX2_;
                        ledOffsetX_ = ledPosX2_;
                        offsetField_ = 1;
                        displayLeds();
                    }
                    buf.Printf("%d:", slot+1);
                }
            }
            else
            {
                if (lastLedPosX_ != ledPosX2_)
                {
                    lastLedPosX_ = ledPosX2_;
                    ledOffsetX_ = ledPosX2_;
                    offsetField_ = 1;
                    displayLeds();
                }
                switch(p_Computer->getComxExpansionType(slot))
                {
                    case COMXRAM:
                        if (rect.GetWidth() < statusBarElementMeasure[3])
                            buf.Printf(leaderString_ + "32K", slot+1);
                        else
                            buf.Printf(leaderString_ + "Ram 32K", slot+1);
                    break;

                    case COMXFLOP:
                        buf.Printf(leaderString_ + "FDC", slot+1);
                    break;

                    case COMXDIAG:
                        buf.Printf(leaderString_ + "Diag", slot + 1);
                    break;

                    case COMXPRINTER:
                        if (rect.GetWidth() < statusBarElementMeasure[2])
                            buf.Printf(leaderString_ + "Pr", slot+1);
                        else
                            buf.Printf(leaderString_ + "Printer", slot+1);
                    break;

                    case COMXRS232:
                        if (rect.GetWidth() < statusBarElementMeasure[2])
                            buf.Printf(leaderString_ + "232", slot+1);
                        else
                            buf.Printf(leaderString_ + "RS232", slot+1);
                    break;

                    case COMXTHPRINTER:
                        if (rect.GetWidth() < statusBarElementMeasure[2])
                            buf.Printf(leaderString_ + "T-P", slot+1);
                        else
                            if (rect.GetWidth() < statusBarElementMeasure[4])
                                buf.Printf(leaderString_ + "Thermal", slot+1);
                            else
                                buf.Printf(leaderString_ + "Thermal Printer", slot+1);
                    break;

                    case COMX80COLUMN:
                        if (rect.GetWidth() < statusBarElementMeasure[2])
                            buf.Printf(leaderString_ + "80", slot+1);
                        else
                            if (rect.GetWidth() < statusBarElementMeasure[3])
                                buf.Printf(leaderString_ + "80 COL", slot+1);
                            else
                                buf.Printf(leaderString_ + "80 Column", slot+1);
                    break;

                    case COMXJOY:
                        if (rect.GetWidth() < statusBarElementMeasure[2])
                            buf.Printf(leaderString_ + "Joy", slot+1);
                        else
                            if (rect.GetWidth() < statusBarElementMeasure[4])
                                buf.Printf(leaderString_ + "JoyCard", slot+1);
                            else
                                buf.Printf(leaderString_ + "F&M JoyCard", slot+1);
                    break;
                    
                    case COMXEPROMBOARD:
                        if (rect.GetWidth() < statusBarElementMeasure[2])
                            buf.Printf(leaderString_ + "E-B", slot+1);
                        else
                            if (rect.GetWidth() < statusBarElementMeasure[4])
                                buf.Printf(leaderString_ + "Eprom", slot+1);
                            else
                                buf.Printf(leaderString_ + "F&M Eprom Board", slot+1);
                    break;

                    case NETWORK:
                        if (rect.GetWidth() < statusBarElementMeasure[2])
                            buf.Printf(leaderString_ + "NW", slot+1);
                        else
                            buf.Printf(leaderString_ + "Network", slot+1);
                    break;

                    case COMXSUPERBOARD:
                        if (rect.GetWidth() < statusBarElementMeasure[3])
                            buf.Printf(leaderString_ + "USB", slot+1);
                        else
                            buf.Printf(leaderString_ + "USB Board", slot+1);
                    break;
                }
            }
            SetStatusText(buf, slot+1);
        }
    }
}

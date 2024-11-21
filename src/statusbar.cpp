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
#include "statusbar.h"
#include "pushbutton.h"

EmmaStatusBar::EmmaStatusBar(wxWindow *parent)
: wxStatusBar(parent, wxID_ANY, 0)
{
    
}

EmmaStatusBar::~EmmaStatusBar()
{
    delete ledOffPointer;
    delete ledOnPointer;
    deleteBitmaps();
}

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
: EmmaStatusBar(parent)
{
    ledOffPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/comxledoff.png", wxBITMAP_TYPE_PNG);
    ledOnPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/comxledon.png", wxBITMAP_TYPE_PNG);
    ledDisabledPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/comxleddisabled.png", wxBITMAP_TYPE_PNG);

    ledsDefined_ = false;
    statusLedUpdate_ = true;
    slotLedUpdate_ = true;
        
#if defined(__WXMSW__)
    led_pos_y_ = 9;
#else
    led_pos_y_ = p_Main->getBarLedPosY();
#endif
    ledPosX1_ = p_Main->getBarLedPosX1();
    ledPosX2_ = p_Main->getBarLedPosX2();
    ledSpacing_ = 2; //p_Main->getBarLedSpacing();

    offsetField_ = 1;

    ledOffsetX_ = ledPosX2_;
    lastLedPosX_ = -1;

    leaderString_ = p_Main->getBarLeader();
    
    for (int i=0; i<5; i++)
        statusBarElementMeasure[i] = p_Main->getStatusBarElementMeasure(i);
}

ComxStatusBar::~ComxStatusBar()
{
    delete ledDisabledPointer;
}

void ComxStatusBar::init(bool expansionConfiguration_defined)
{
    expansionConfiguration_defined_ = expansionConfiguration_defined;
    SetFieldsCount(5);
    displayLeds();
    displayText();
}

void ComxStatusBar::updateLedStatus(bool status, int card, int i)
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
    if (expansionConfiguration_defined_)
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
                                     wxPoint((card+offsetField_)*((int)rect.GetWidth()+ledSpacing_)+i*14+ledOffsetX_+(card*3), led_pos_y_), wxSize(-1, -1),
                                     wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMAC__)
            ledBitmapPointers [card][i] = new PushBitmapButton(this, number, *ledOffPointer,
                                     wxPoint((card+offsetField_)*((int)rect.GetWidth()+ledSpacing_)+i*14+ledOffsetX_+(card*3), led_pos_y_), wxSize(-1, -1),
                                     wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMSW__)
            ledBitmapPointers [card][i] = new PushButton(this, number, wxEmptyString,
                                     wxPoint((card+offsetField_)*(rect.GetWidth()+ledSpacing_)+i*14+ledOffsetX_, led_pos_y_), wxSize(LED_SIZE_X, LED_SIZE_Y),
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
            if (p_Computer->readMemDebug(0xC001) == COMXEMPTY)
                ledBitmapPointers[card][i]->Hide();
            if ((!expansionConfiguration_defined_) &&(card >0))
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
        if (!((slot > 0) && !expansionConfiguration_defined_))
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
                buf = getStatusBarTextXml(slot, rect);
            }
            SetStatusText(buf, slot+1);
        }
    }
}

wxString ComxStatusBar::getStatusBarTextXml(int slot, wxRect rect)
{
    wxString buf;
    int currentSlot;

    currentSlot = p_Computer->getSelectedSlot();
    p_Computer->setSelectedSlot(slot);
    
    if (p_Computer->getXmlSlotMemoryType(slot, 0) == RAM)
    {
        if (rect.GetWidth() < statusBarElementMeasure[3])
            buf.Printf(leaderString_ + "32K", slot+1);
        else
            buf.Printf(leaderString_ + "Ram 32K", slot+1);
    }
    else
    {
        switch(p_Computer->readMemDebug(0xC001))
        {
            case COMXFLOP:
                buf.Printf(leaderString_ + "FDC", slot+1);
            break;

            case COMXDIAG:
                buf.Printf(leaderString_ + "Diag", slot + 1);
            break;

            case PRINTER_PARALLEL:
                if (p_Computer->readMemDebug(0xC014) == 'P')
                {
                    if (rect.GetWidth() < statusBarElementMeasure[2])
                        buf.Printf(leaderString_ + "Pr", slot+1);
                    else
                        buf.Printf(leaderString_ + "Printer", slot+1);
                }
                if (p_Computer->readMemDebug(0xC014) == 'R')
                {
                    if (rect.GetWidth() < statusBarElementMeasure[2])
                        buf.Printf(leaderString_ + "232", slot+1);
                    else
                        buf.Printf(leaderString_ + "RS232", slot+1);
                }
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
    p_Computer->setSelectedSlot(currentSlot);
    return buf;
}

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
: EmmaStatusBar(parent)
{
    ledOffPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/comxledoff.png", wxBITMAP_TYPE_PNG);
    ledOnPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/comxledon.png", wxBITMAP_TYPE_PNG);

    ledsDefined_ = false;
    led_pos_y_ = p_Main->getBarLedPosY();

    for (int i=0; i<5; i++)
        status_[i] = false;

    leaderString_ = p_Main->getBarLeaderCidelsa();
    statusBarElementMeasure0_ = p_Main->getStatusBarElementMeasure(0);
    statusBarElementMeasure1_ = p_Main->getStatusBarElementMeasure(1);
    ledSpacing_ = 2; //p_Main->getBarLedSpacing();
}

CidelsaStatusBar::~CidelsaStatusBar()
{
}

void CidelsaStatusBar::onButtonPress1(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->cidelsaStatusBarDown('1');
}

void CidelsaStatusBar::onButtonRelease1(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->cidelsaStatusBarUp('1');
}

void CidelsaStatusBar::onButtonPress2(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->cidelsaStatusBarDown('2');
}

void CidelsaStatusBar::onButtonRelease2(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->cidelsaStatusBarUp('2');
}

void CidelsaStatusBar::onButtonPressA(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->cidelsaStatusBarDown('A');
}

void CidelsaStatusBar::onButtonReleaseA(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->cidelsaStatusBarUp('A');
}

void CidelsaStatusBar::onButtonPressB(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->cidelsaStatusBarDown('B');
}

void CidelsaStatusBar::onButtonReleaseB(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->cidelsaStatusBarUp('B');
}

void CidelsaStatusBar::onButtonPressFire(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->cidelsaStatusBarDown(WXK_SPACE);
}

void CidelsaStatusBar::onButtonReleaseFire(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->cidelsaStatusBarUp(WXK_SPACE);

}

void CidelsaStatusBar::init(bool WXUNUSED(expansionConfiguration_defined))
{
    SetFieldsCount(5);
    displayLeds();
    displayText();
}

void CidelsaStatusBar::configure(int ledOut)
{
    if (ledOut != -1)
    {
        wxString message;
        p_Main->message("Configuring Statusbar");
        message.Printf(" Output %d: led on/off\n", ledOut);
        p_Main->message(message);
        p_Computer->setOutType(ledOut, LED_CIDELSA_OUT);
    }
}

void CidelsaStatusBar::updateLedStatus(bool status, int number, int WXUNUSED(i))
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

    if (rect.GetWidth() < statusBarElementMeasure0_)
    {
        SetStatusText("", 0);
        SetStatusText("", 1);
        SetStatusText("", 2);
        SetStatusText("", 3);
        SetStatusText("", 4);
    }
    else
    {
        if (rect.GetWidth() < statusBarElementMeasure1_)
        {
            SetStatusText(leaderString_ + "S A", 0);
            SetStatusText(leaderString_ + "S B", 1);
            SetStatusText(leaderString_ + "1 P", 2);
            SetStatusText(leaderString_ + "2 P", 3);
            SetStatusText(leaderString_ + "Fire", 4);
        }
        else
        {
            SetStatusText(leaderString_ + "Shute A", 0);
            SetStatusText(leaderString_ + "Shute B", 1);
            SetStatusText(leaderString_ + "1 Player", 2);
            SetStatusText(leaderString_ + "2 Players", 3);
            SetStatusText(leaderString_ + "Fire", 4);
        }
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
                                     wxPoint(number*(rect.GetWidth()+ledSpacing_)+3+number*3, led_pos_y_), wxSize(-1, -1),
                                     wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMAC__)
        ledBitmapPointers [number] = new PushBitmapButton(this, number, *ledOnPointer,
                                                          wxPoint(number*(rect.GetWidth()+ledSpacing_)+3+number*3, led_pos_y_), wxSize(-1, -1),
                                                          wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMSW__)
        ledBitmapPointers [number] = new PushButton(this, number, wxEmptyString,
                                     wxPoint(number*(rect.GetWidth()+ledSpacing_)+3, 9), wxSize(CIDELSA_LED_SIZE_X, CIDELSA_LED_SIZE_Y),
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

DiagStatusBar::DiagStatusBar(wxWindow *parent)
: EmmaStatusBar (parent)
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
}

void DiagStatusBar::init(bool WXUNUSED(expansionConfiguration_defined))
{
    SetFieldsCount(NUMBER_OF_DIAG_LEDS);
    for (int i=0; i<NUMBER_OF_DIAG_LEDS; i++)
        ledStatus_[i] = false;
    displayLeds();
    displayText();
}

void DiagStatusBar::updateLedStatus(bool status, int led, int WXUNUSED(i))
{
    if (!ledsDefined_)
        return;
    
    if (status == ledStatus_[led])
        return;
    
    ledStatus_[led] = status;

    if (status)
    {
#if wxCHECK_VERSION(2, 9, 0)
        diagLedPointer [led]->SetBitmap(*ledOnPointer);
#else
        diagLedPointer [led]->SetBitmapLabel(*ledOnPointer);
#endif
    }
    else
    {
#if wxCHECK_VERSION(2, 9, 0)
        diagLedPointer [led]->SetBitmap(*ledOffPointer);
#else
        diagLedPointer [led]->SetBitmapLabel(*ledOffPointer);
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
           diagLedPointer [led] = new PushBitmapButton(this, led, *ledOnPointer, wxPoint(led*((int)rect.GetWidth()+1)+(led*3)+2, led_pos_y_), wxSize(-1, -1), wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
        else
            diagLedPointer [led] = new PushBitmapButton(this, led, *ledOffPointer, wxPoint(led*((int)rect.GetWidth()+1)+(led*3)+2, led_pos_y_), wxSize(-1, -1), wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMAC__)
        if (ledStatus_[led])
            diagLedPointer [led] = new PushBitmapButton(this, led, *ledOnPointer, wxPoint(led*((int)rect.GetWidth()+1)+(led*3)+2, 2), wxSize(-1, -1), wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
        else
            diagLedPointer [led] = new PushBitmapButton(this, led, *ledOffPointer, wxPoint(led*((int)rect.GetWidth()+1)+(led*3)+2, 2), wxSize(-1, -1), wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMSW__)
        diagLedPointer [led] = new PushButton(this, led, wxEmptyString, wxPoint(led*((int)rect.GetWidth()+1)+led+2, 4), wxSize(DIAG_LED_SIZE_X, DIAG_LED_SIZE_Y), wxBORDER_NONE);
#endif

        if (ledStatus_[led])
        {
    #if wxCHECK_VERSION(2, 9, 0)
            diagLedPointer [led]->SetBitmap(*ledOnPointer);
    #else
            diagLedPointer [led]->SetBitmapLabel(*ledOnPointer);
    #endif
        }
        else
        {
    #if wxCHECK_VERSION(2, 9, 0)
            diagLedPointer [led]->SetBitmap(*ledOffPointer);
    #else
            diagLedPointer [led]->SetBitmapLabel(*ledOffPointer);
    #endif
        }
    }
    ledsDefined_ = true;
}

void DiagStatusBar::deleteBitmaps()
{
    if (!ledsDefined_)  return;
      for (int led = 0; led < NUMBER_OF_DIAG_LEDS; led++)
    {
        delete diagLedPointer [led];
    }
    ledsDefined_ = false;
}

void DiagStatusBar::reDrawBar()
{
    displayText();
    displayLeds();
}

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
    ledSpacing_ = 2; //p_Main->getBarLedSpacing();

    led_pos_y_ = p_Main->getBarLedPosVip2Y();
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

void VipIIStatusBar::updateLedStatus(bool status, int led, int WXUNUSED(i))
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
                                     wxPoint(led*((int)rect.GetWidth()+ledSpacing_)+2+(led*3), led_pos_y_), wxSize(-1, -1),
                                     wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
        else
            ledBitmapPointers [led] = new PushBitmapButton(this, led, *ledOffPointer,
                                     wxPoint(led*((int)rect.GetWidth()+ledSpacing_)+2+(led*3), led_pos_y_), wxSize(-1, -1),
                                     wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMAC__)
        if (ledStatus[led])
            ledBitmapPointers [led] = new PushBitmapButton(this, led, *ledOnPointer,
                                     wxPoint(led*((int)rect.GetWidth()+ledSpacing_)+2+(led*3), led_pos_y_), wxSize(-1, -1),
                                     wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
        else
            ledBitmapPointers [led] = new PushBitmapButton(this, led, *ledOffPointer,
                                     wxPoint(led*((int)rect.GetWidth()+ledSpacing_)+2+(led*3), led_pos_y_), wxSize(-1, -1),
                                     wxNO_BORDER | wxBU_EXACTFIT | wxBU_TOP);
#endif
#if defined(__WXMSW__)
        ledBitmapPointers [led] = new PushButton(this, led, wxEmptyString,
                                 wxPoint(led*(rect.GetWidth()+ledSpacing_)+2, 6), wxSize(12, 12),
                                 wxBORDER_NONE);
#endif

        if (ledStatus[led])
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

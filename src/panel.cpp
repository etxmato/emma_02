/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** wFthout express written permission from the author.         ***
 ***                                                             ***
 *** 1802 Code based on elf emulator by Michael H Riley with     ***
 *** copyright as below                                          ***
 *******************************************************************
*/

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/frame.h"
#include "wx/dir.h"

#include "main.h"
#include "panel.h"
#include <memory>


BEGIN_EVENT_TABLE(HexButton, wxEvtHandler )
    EVT_TIMER(wxID_ANY, HexButton::OnTimer)
END_EVENT_TABLE()

HexButton::HexButton(wxDC& dc, int type, wxCoord x, wxCoord y, wxString label)
{
    wxBitmap *upBitmap;
    wxBitmap *downBitmap;
    wxBitmap *disabledBitmap;
    wxSize buttonSize;
    
    buttonType_ = type;
    switch (buttonType_)
    {
        case PUSH_BUTTON_RED:
            upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/resetButtonUp.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/resetButtonDown.png", wxBITMAP_TYPE_PNG);
            buttonSize.x = 30;
            buttonSize.y = 30;
        break;
            
        case PUSH_BUTTON_ORANGE:
            upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/runButtonUp.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/runButtonDown.png", wxBITMAP_TYPE_PNG);
            buttonSize.x = 30;
            buttonSize.y = 30;
        break;

        case PUSH_BUTTON_YELLOW:
            upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/mpButtonUp.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/mpButtonDown.png", wxBITMAP_TYPE_PNG);
            buttonSize.x = 30;
            buttonSize.y = 30;
        break;
            
        case PUSH_BUTTON_GREEN:
            upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/inButtonUp.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/inButtonDown.png", wxBITMAP_TYPE_PNG);
            buttonSize.x = 30;
            buttonSize.y = 30;
        break;
            
        case PUSH_BUTTON_BLUE:
            upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/loadButtonUp.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/loadButtonDown.png", wxBITMAP_TYPE_PNG);
            buttonSize.x = 30;
            buttonSize.y = 30;
        break;
            
        case PUSH_BUTTON_SMALL:
        case PUSH_BUTTON_PIO:
            upBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button_small.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button_small_pushed.png", wxBITMAP_TYPE_PNG);
            buttonSize.x = 26;
            buttonSize.y = 26;
        break;
            
        case PUSH_BUTTON_RECTANGLE_SMALL:
            upBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button_uc1800.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button_uc1800_pushed.png", wxBITMAP_TYPE_PNG);
            buttonSize.x = 26;
            buttonSize.y = 20;
        break;
            
        case PUSH_BUTTON_RECTANGLE:
            upBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button_wide.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button_wide_pushed.png", wxBITMAP_TYPE_PNG);
            buttonSize.x = 34;
            buttonSize.y = 30;
        break;
            
        default:
            upBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button_pushed.png", wxBITMAP_TYPE_PNG);
            buttonSize.x = 30;
            buttonSize.y = 30;
        break;
   }
    upBitmapPointer = new wxBitmap(buttonSize);
    downBitmapPointer = new wxBitmap(buttonSize);
    
    wxMemoryDC memDC(*upBitmapPointer);
    wxSize labelSize = memDC.GetTextExtent(label);
    wxCoord xPosition = (buttonSize.x - labelSize.x)/2;
    wxCoord yPosition = (buttonSize.y - labelSize.y)/2;
    memDC.Clear();
    memDC.DrawBitmap(*upBitmap, 0, 0);
    if (buttonType_ >= PUSH_BUTTON_RED && buttonType_ <= PUSH_BUTTON_BLUE)
        memDC.SetTextForeground(*wxWHITE);
    memDC.DrawText(label, xPosition, yPosition);
    memDC.SelectObject(wxNullBitmap);
    
    memDC.SelectObject(*downBitmapPointer);
    memDC.Clear();
    memDC.DrawBitmap(*downBitmap, 0, 0);
    if (buttonType_ >= PUSH_BUTTON_RED && buttonType_ <= PUSH_BUTTON_BLUE)
        memDC.SetTextForeground(*wxWHITE);
    memDC.DrawText(label, xPosition, yPosition);
    memDC.SelectObject(wxNullBitmap);
    
    if (type == PUSH_BUTTON_PIO)
    {
        disabledBitmapPointer = new wxBitmap(buttonSize);
        
        disabledBitmap = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + "/hex_button_small_disabled.png", wxBITMAP_TYPE_PNG);
        
        memDC.SelectObject(*disabledBitmapPointer);
        memDC.Clear();
        memDC.DrawBitmap(*disabledBitmap, 0, 0);
        memDC.SetTextForeground(*wxLIGHT_GREY);
        memDC.DrawText(label, xPosition, yPosition);
        memDC.SelectObject(wxNullBitmap);

        delete disabledBitmap;
    }

    delete upBitmap;
    delete downBitmap;
    
    enabled_ = true;
    
    x_ = x;
    y_ = y;
    
    state_ = BUTTON_UP;
    focusTimer = new wxTimer(this, wxID_ANY);

    dc.DrawBitmap(*upBitmapPointer, x, y);
}

HexButton::~HexButton()
{
    delete upBitmapPointer;
    delete downBitmapPointer;
    delete focusTimer;

    if (buttonType_ == PUSH_BUTTON_PIO)
        delete disabledBitmapPointer;
}

void HexButton::onPaint(wxDC& dc)
{
    if (enabled_)
    {
        if (state_ == BUTTON_UP)
        {
            dc.DrawBitmap(*upBitmapPointer, x_, y_);
        }
        else
        {
            dc.DrawBitmap(*downBitmapPointer, x_, y_);
        }
    }
    else
    {
        dc.DrawBitmap(*disabledBitmapPointer, x_, y_);
    }
}

bool HexButton::onMousePress(wxDC& dc, wxCoord x, wxCoord y)
{
    focusTimer->Stop();
    if ((x >= x_) &&(x <= (x_+30)) &&(y >= y_) &&(y <= (y_+30)))
    {
        state_ = BUTTON_DOWN;
        dc.DrawBitmap(*downBitmapPointer, x_, y_);
        return true;
    }
    return false;
}

bool HexButton::onMouseRelease(wxDC& WXUNUSED(dc), wxCoord x, wxCoord y)
{
    if ((x >= x_) &&(x <= (x_+30)) &&(y >= y_) &&(y <= (y_+30)))
    {
        focusTimer->Start(50, wxTIMER_ONE_SHOT);
        return true;
    }
    return false;
}

void HexButton::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    state_ = BUTTON_UP;
    p_Computer->releaseButtonOnScreen(this);
}

void HexButton::releaseButtonOnScreen(wxDC& dc)
{
    dc.DrawBitmap(*upBitmapPointer, x_, y_);
}

void HexButton::enable(wxDC& dc, bool enabled)
{
    enabled_ = enabled;
    if (enabled_)
    {
        if (state_ == BUTTON_UP)
        {
            dc.DrawBitmap(*upBitmapPointer, x_, y_);
        }
        else
        {
            dc.DrawBitmap(*downBitmapPointer, x_, y_);
        }
    }
    else
    {
        dc.DrawBitmap(*disabledBitmapPointer, x_, y_);
    }
}

BEGIN_EVENT_TABLE(HexButtonCdp1851, wxEvtHandler )
    EVT_TIMER(wxID_ANY, HexButtonCdp1852::OnTimer)
END_EVENT_TABLE()

HexButtonCdp1851::HexButtonCdp1851(wxDC& dc, int type, wxCoord x, wxCoord y, wxString label, int pioNumber)
: HexButton(dc, type, x, y, label)
{
    pioNumber_ = pioNumber;
}

void HexButtonCdp1851::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    state_ = BUTTON_UP;
    p_Computer->releaseButtonOnScreen1851(this, buttonType_, pioNumber_);
}

BEGIN_EVENT_TABLE(HexButtonCdp1852, wxEvtHandler )
    EVT_TIMER(wxID_ANY, HexButtonCdp1852::OnTimer)
END_EVENT_TABLE()

HexButtonCdp1852::HexButtonCdp1852(wxDC& dc, int type, wxCoord x, wxCoord y, wxString label, int pioNumber)
: HexButton(dc, type, x, y, label)
{
    pioNumber_ = pioNumber;
}

void HexButtonCdp1852::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    state_ = BUTTON_UP;
    p_Computer->releaseButtonOnScreen1852(this, buttonType_, pioNumber_);
}

SwitchButton::SwitchButton(wxDC& dc, int type, wxColour bkgrClr, bool state, wxCoord x, wxCoord y, wxString label)
{
    wxBitmap *upBitmap;
    wxBitmap *downBitmap;

    wxString linuxExtension = "";
#if defined (__linux__)
    linuxExtension = "_linux";
#endif
    
    buttonSize_.x = 30;
    buttonSize_.y = 30;
    buttonStartX_ = 0;
    buttonStartY_ = 0;
    
    enabled_ = true;

    switch (type)
    {
        case SWITCH_BUTTON_VERTICAL:
            upBitmapPointer = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swup.png", wxBITMAP_TYPE_PNG);
            downBitmapPointer = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swdown.png", wxBITMAP_TYPE_PNG);
            buttonSize_.x = 22;
            buttonSize_.y = 22;
            buttonStartX_ = 3;
            buttonStartY_ = 3;
        break;

        case SWITCH_BUTTON_VERTICAL_RED:
        case SWITCH_BUTTON_VERTICAL_PIO:
            upBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swupred.png", wxBITMAP_TYPE_PNG);
            downBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swdownred.png", wxBITMAP_TYPE_PNG);
            buttonSize_.x = 22;
            buttonSize_.y = 22;
            buttonStartX_ = 3;
            buttonStartY_ = 3;
        break;

        case SWITCH_BUTTON_VERTICAL_GREEN:
            upBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swupgreen.png", wxBITMAP_TYPE_PNG);
            downBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swdowngreen.png", wxBITMAP_TYPE_PNG);
            buttonSize_.x = 22;
            buttonSize_.y = 22;
            buttonStartX_ = 3;
            buttonStartY_ = 3;
        break;

        case SWITCH_BUTTON_HORIZONTAL:
            upBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/swright.png", wxBITMAP_TYPE_PNG);
            downBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/swleft.png", wxBITMAP_TYPE_PNG);
        break;

        case PUSH_BUTTON_ROUND_RED:
            upBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/pushup.png", wxBITMAP_TYPE_PNG);
            downBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/pushdown.png", wxBITMAP_TYPE_PNG);
        break;

        case PUSH_BUTTON_ROUND_RED_LARGE:
            upBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/pushupLarge.png", wxBITMAP_TYPE_PNG);
            downBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/pushdownLarge.png", wxBITMAP_TYPE_PNG);
        break;

        case PUSH_BUTTON_ROUND_GREEN_LARGE:
            upBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/pushupLargeGreen.png", wxBITMAP_TYPE_PNG);
            downBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/pushdownLargeGreen.png", wxBITMAP_TYPE_PNG);
        break;

        case PUSH_BUTTON_ROUND_BLACK:
            upBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/pushupblack.png", wxBITMAP_TYPE_PNG);
            downBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/pushdownblack.png", wxBITMAP_TYPE_PNG);
        break;

        case ELF2K_POWER_BUTTON:
            upBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/Elf2Kon.png", wxBITMAP_TYPE_PNG);
            downBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/Elf2Koff.png", wxBITMAP_TYPE_PNG);
        break;

        case ELF2K_LOAD_BUTTON:
        case SWITCH_BUTTON_BLUE:
            upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/loadButtonUp.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/loadButtonDown.png", wxBITMAP_TYPE_PNG);
            buttonSize_.x = 30;
            buttonSize_.y = 30;
        break;

        case ELF2K_MP_BUTTON:
        case SWITCH_BUTTON_YELLOW:
            upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/mpButtonUp.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/mpButtonDown.png", wxBITMAP_TYPE_PNG);
            buttonSize_.x = 30;
            buttonSize_.y = 30;
        break;

        case ELF2K_RUN_BUTTON:
        case SWITCH_BUTTON_ORANGE:
            upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/runButtonUp.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/runButtonDown.png", wxBITMAP_TYPE_PNG);
            buttonSize_.x = 30;
            buttonSize_.y = 30;
        break;

        case ELF2K_IN_BUTTON:
        case SWITCH_BUTTON_SQUARE_GREEN:
            upBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/inButtonUp.png", wxBITMAP_TYPE_PNG);
            downBitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/inButtonDown.png", wxBITMAP_TYPE_PNG);
            buttonSize_.x = 30;
            buttonSize_.y = 30;
        break;

        case DIP_SWITCH_BUTTON:
            upBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/dip_switch_on.png", wxBITMAP_TYPE_PNG);
            downBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/dip_switch_off.png", wxBITMAP_TYPE_PNG);
            buttonSize_.x = 8;
            buttonSize_.y = 20;
        break;

        case THUMB_MINUS_BUTTON:
            upBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/thumb-minus.png", wxBITMAP_TYPE_PNG);
            downBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/thumb-minus_down.png", wxBITMAP_TYPE_PNG);
            buttonSize_.x = 18;
            buttonSize_.y = 14;
        break;
            
        case THUMB_PLUS_BUTTON:
            upBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/thumb-plus.png", wxBITMAP_TYPE_PNG);
            downBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/thumb-plus_down.png", wxBITMAP_TYPE_PNG);
            buttonSize_.x = 18;
            buttonSize_.y = 14;
        break;

        default:
            upBitmapPointer = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swup.png", wxBITMAP_TYPE_PNG);
            downBitmapPointer = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swdown.png", wxBITMAP_TYPE_PNG);
        break;
    }
    
    if (type == PUSH_BUTTON_ROUND_BLACK)
        type = PUSH_BUTTON_ROUND_RED;

    wxColour maskColour(255, 0, 255);

    if (type >= SWITCH_BUTTON_ORANGE && type <= SWITCH_BUTTON_BLUE)
    {
        upBitmapPointer = new wxBitmap(buttonSize_);

        wxMemoryDC memDC(*upBitmapPointer);
        wxSize labelSize = memDC.GetTextExtent(label);
        wxCoord xPosition = (buttonSize_.x - labelSize.x)/2;
        wxCoord yPosition = (buttonSize_.y - labelSize.y)/2;
        memDC.Clear();
        memDC.DrawBitmap(*upBitmap, 0, 0);
        memDC.SetTextForeground(*wxWHITE);
        memDC.DrawText(label, xPosition, yPosition);
        memDC.SelectObject(wxNullBitmap);
        
        downBitmapPointer = new wxBitmap(buttonSize_);

        memDC.SelectObject(*downBitmapPointer);
        memDC.Clear();
        memDC.DrawBitmap(*downBitmap, 0, 0);
        memDC.SetTextForeground(*wxWHITE);
        memDC.DrawText(label, xPosition, yPosition);
        memDC.SelectObject(wxNullBitmap);
        
        delete upBitmap;
        delete downBitmap;
    }
    else
    {
        maskUp = new wxMask (*upBitmapPointer, maskColour);
        upBitmapPointer->SetMask(maskUp);
        maskDown = new wxMask (*downBitmapPointer, maskColour);
        downBitmapPointer->SetMask(maskDown);
    }

    if (type == SWITCH_BUTTON_VERTICAL_PIO)
    {
        disabledUpBitmapPointer = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swdisabledup.png", wxBITMAP_TYPE_PNG);
        disabledDownBitmapPointer = new wxBitmap (p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/swdisableddown.png", wxBITMAP_TYPE_PNG);
        
        disabledUpBitmapPointer->SetMask(maskUp);
        disabledDownBitmapPointer->SetMask(maskDown);
    }

    x_ = x;
    y_ = y;
    state_ = state;
    type_ = type;

    if (state == BUTTON_UP)
        dc.DrawBitmap(*upBitmapPointer, x, y, true);
    else
        dc.DrawBitmap(*downBitmapPointer, x, y, true);
}

SwitchButton::~SwitchButton()
{
    delete upBitmapPointer;
    delete downBitmapPointer;
    if (type_ == SWITCH_BUTTON_VERTICAL_PIO)
    {
 //       delete disabledUpBitmapPointer;
 //       delete disabledDownBitmapPointer;
    }
}

void SwitchButton::onPaint(wxDC& dc)
{
    if (enabled_)
    {
        if (state_ == BUTTON_UP)
            dc.DrawBitmap(*upBitmapPointer, x_, y_, true);
        else
            dc.DrawBitmap(*downBitmapPointer, x_, y_, true);
    }
    else
    {
        if (state_ == BUTTON_UP)
            dc.DrawBitmap(*disabledUpBitmapPointer, x_, y_, true);
        else
            dc.DrawBitmap(*disabledDownBitmapPointer, x_, y_, true);
    }
}

bool SwitchButton::onMousePress(wxDC& dc, wxCoord x, wxCoord y)
{
    if ((x >= (x_+buttonStartX_)) &&(x <= (x_+buttonSize_.x)) &&(y >= (y_+buttonStartY_)) &&(y <= (y_+buttonSize_.y)))
    {
        if (type_ >= PUSH_BUTTON)
        {
            state_ = !state_;
            if (state_ == BUTTON_UP)
                dc.DrawBitmap(*upBitmapPointer, x_, y_, true);
            else
                dc.DrawBitmap(*downBitmapPointer, x_, y_, true);
        }
        return true;
    }
    return false;
}

bool SwitchButton::onMouseRelease(wxDC& dc, wxCoord x, wxCoord y)
{
    if ((x >= (x_+buttonStartX_)) &&(x <= (x_+buttonSize_.x)) &&(y >= (y_+buttonStartY_)) &&(y <= (y_+buttonSize_.y)))
    {
        state_ = !state_;
        if (state_ == BUTTON_UP)
            dc.DrawBitmap(*upBitmapPointer, x_, y_, true);
        else
            dc.DrawBitmap(*downBitmapPointer, x_, y_, true);
        return true;
    }
    return false;
}

void SwitchButton::setState(wxDC& dc, bool state)
{
    state_ = state;
    if (enabled_)
    {
        if (state_ == BUTTON_UP)
        {
            dc.DrawBitmap(*upBitmapPointer, x_, y_, true);
        }
        else
        {
            dc.DrawBitmap(*downBitmapPointer, x_, y_, true);
        }
    }
    else
    {
        if (state_ == BUTTON_UP)
        {
            dc.DrawBitmap(*disabledUpBitmapPointer, x_, y_, true);
        }
        else
        {
            dc.DrawBitmap(*disabledDownBitmapPointer, x_, y_, true);
        }
    }
}

void SwitchButton::enable(wxDC& dc, bool enabled)
{
    enabled_ = enabled;
    if (enabled_)
    {
        if (state_ == BUTTON_UP)
        {
            dc.DrawBitmap(*upBitmapPointer, x_, y_, true);
        }
        else
        {
            dc.DrawBitmap(*downBitmapPointer, x_, y_, true);
        }
    }
    else
    {
        if (state_ == BUTTON_UP)
        {
            dc.DrawBitmap(*disabledUpBitmapPointer, x_, y_, true);
        }
        else
        {
            dc.DrawBitmap(*disabledDownBitmapPointer, x_, y_, true);
        }
    }
}

RotButton::RotButton(wxDC& dc, int state, wxCoord x, wxCoord y, int type)
{
    wxBitmap *bitmap;
    wxString linuxExtension = "";
#if defined (__linux__)
    linuxExtension = "_linux";
#endif
    
    buttonSize_.x = 60;
    buttonSize_.y = 60;
    buttonStartX_ = 0;
    buttonStartY_ = 0;
    
    wxString number;
   
    state_ = state & 0xf;

    for (int i=0; i<16; i++)
    {
        number.Printf("%01X", i);
        bitmap = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/16_pos_rot_"+ number +".png", wxBITMAP_TYPE_PNG);
        
        bitmapPointer[i] = new wxBitmap(bitmap->GetWidth(), bitmap->GetHeight());

        wxMemoryDC memDC(*bitmapPointer[i]);
        memDC.Clear();
        memDC.DrawBitmap(*bitmap, 0, 0, true);
        memDC.SelectObject(wxNullBitmap);
        
        delete bitmap;
    }
    
    x_ = x;
    y_ = y;

    dc.DrawBitmap(*bitmapPointer[state_], x, y);
}

RotButton::~RotButton()
{
}

void RotButton::onPaint(wxDC& dc)
{
    dc.DrawBitmap(*bitmapPointer[state_], x_, y_);
}

bool RotButton::onMouseLeftRelease(wxDC& dc, wxCoord x, wxCoord y)
{
    if ((x >= (x_+buttonStartX_)) &&(x <= (x_+buttonSize_.x)) &&(y >= (y_+buttonStartY_)) &&(y <= (y_+buttonSize_.y)))
    {
        state_++;
        state_ &= 0xf;
        dc.DrawBitmap(*bitmapPointer[state_], x_, y_);
        return true;
    }
    return false;
}

bool RotButton::onMouseRightRelease(wxDC& dc, wxCoord x, wxCoord y)
{
    if ((x >= (x_+buttonStartX_)) &&(x <= (x_+buttonSize_.x)) &&(y >= (y_+buttonStartY_)) &&(y <= (y_+buttonSize_.y)))
    {
        state_--;
        state_ &= 0xf;
        dc.DrawBitmap(*bitmapPointer[state_], x_, y_);
        return true;
    }
    return false;
}

void RotButton::setState(wxDC& dc, int state)
{
    state_ = state & 0xf;
    dc.DrawBitmap(*bitmapPointer[state_], x_, y_);
}

void *RunComputer::Entry()
{
    while(!p_Main->emuClosing())
    {
        p_Computer->cpuInstruction();
    }
    wxCommandEvent event(KILL_COMPUTER, 809);
    event.SetEventObject(p_Main);
    wxPostEvent(p_Main, event);
    return NULL;
}

BEGIN_EVENT_TABLE(Panel, wxWindow)
    EVT_PAINT(Panel::onPaint)
    EVT_CHAR(Panel::onChar)
    EVT_KEY_DOWN(Panel::onKeyDown)
    EVT_KEY_UP(Panel::onKeyUp)
    EVT_LEFT_DOWN(Panel::onMousePress)
    EVT_LEFT_UP(Panel::onMouseRelease)
    EVT_RIGHT_UP(Panel::onMouseRightRelease)

    EVT_SPINCTRL(0x70, Panel::onAdiChannel)
    EVT_TEXT(0x70, Panel::onAdiChannelText)
    EVT_SPINCTRL(0x71, Panel::onAdiVolt)
    EVT_TEXT(0x71, Panel::onAdiVoltText)

    EVT_SPINCTRL(0x72, Panel::onAdsChannel)
    EVT_TEXT(0x72, Panel::onAdsChannelText)
    EVT_SPINCTRL(0x73, Panel::onAdsVolt)
    EVT_TEXT(0x74, Panel::onAdsVoltText)

END_EVENT_TABLE()

Panel::Panel(wxWindow *parent, const wxSize& size)
: wxWindow(parent, wxID_ANY, wxDefaultPosition, size)
{
    updateQLed_ = false;
    updateReadyLed_ = false;
    updateStopLed_ = false;
    updatePowerLed_ = false;
    updateErrorLed_ = false;
    for (int i=0; i<MAX_DATA_TIL; i++)
    {
        tilDataPointerDefined[i] = false;
        nibbleLedPointerDefined[i] = false;
        nibbleLedStatus[i] = 0;
        updateNibbleLed_[i] = false;
        updateDp313_[i] = false;
        dpStatus[i] = false;
    }
    for (int i=0; i<MAX_BIT_LEDS; i++)
    {
        ledPointerDefined[i] = false;
        ledStatus[i] = 0;
        updateLed_[i] = false;
    }
    for (int i=0; i<MAX_ADDRESS_TIL; i++)
    {
        tilAddressPointerDefined[i] = false;
    }
    for (int i=0; i<MAX_CPU_STATE_LEDS; i++)
    {
        stateLedPointerDefined[i] = false;
        stateLedStatus[i] = 0;
        updateStateLed_[i] = false;
    }
    for (int i=0; i<MAX_MULTI_TIL; i++)
    {
        segStatus[i] = 0;
        updateSeg_[i] = false;
        tilMultiPointerDefined[i] = false;
        updateMulti_[i] = false;
    }
    updateAddress_ = false;
    updateData_ = false;

    qLedStatus = 0;
    readyLedStatus = 0;
    stopLedStatus = 0;
    powerLedStatus = 0;
    errorLedStatus = 0;
    addressStatus = 0;
    dataStatus = 0;
    ms_ = 100;

    functionKeyReleaseTwo_ = false;
    xmlButtonDefined_ = false;
}

Panel::~Panel()
{
    for (std::vector<GuiItemConfiguration>::iterator button = guiItemConfiguration.begin (); button != guiItemConfiguration.end (); ++button)
    {
        switch (button->type)
        {
            case SWITCH_BUTTON_VERTICAL:
            case SWITCH_BUTTON_VERTICAL_RED:
            case SWITCH_BUTTON_VERTICAL_GREEN:
            case SWITCH_BUTTON_HORIZONTAL:
            case PUSH_BUTTON_ROUND_RED:
            case PUSH_BUTTON_ROUND_BLACK:
            case PUSH_BUTTON_ROUND_RED_LARGE:
            case PUSH_BUTTON_ROUND_GREEN_LARGE:
            case ELF2K_POWER_BUTTON:
            case SWITCH_BUTTON_ORANGE:
            case SWITCH_BUTTON_YELLOW:
            case SWITCH_BUTTON_SQUARE_GREEN:
            case SWITCH_BUTTON_BLUE:
            case THUMB_MINUS_BUTTON:
            case THUMB_PLUS_BUTTON:
                delete button->switchButton;
            break;

            case DIP_SWITCH_BUTTON:
                delete button->dipSwitchButton;
            break;

            case ROT_SWITCH_BUTTON:
                delete button->rotButton;
            break;

            case ADI_SPINCTRL:
                delete spinCtrlAdi;
            break;

            case ADI_VOLT_SPINCTRL:
                delete spinCtrlAdiVolt;
            break;

            case ADS_SPINCTRL:
                delete spinCtrlAds;
            break;

            case ADS_VOLT_SPINCTRL:
                delete spinCtrlAdsVolt;
            break;

            case PUSH_BUTTON:
            case PUSH_BUTTON_SMALL:
            case PUSH_BUTTON_RECTANGLE:
            case PUSH_BUTTON_RECTANGLE_SMALL:
#if defined (__WXMAC__)
                delete button->hexButton;
#else
                delete button->windowsButton;
#endif
            break;

            case PUSH_BUTTON_RED:
            case PUSH_BUTTON_ORANGE:
            case PUSH_BUTTON_YELLOW:
            case PUSH_BUTTON_GREEN:
            case PUSH_BUTTON_BLUE:
            case PUSH_BUTTON_PIO:
                delete button->hexButton;
            break;

            case LED_SMALL_RED:
            case LED_REAL_RED:
            case LED_REAL_GREEN:
            case LED_REAL_ORANGE:
            case LED_LARGE_RED:
            case LED_LARGE_GREEN:
            case LED_LARGE_ORANGE:
            case LED_LARGE_COLOR:
            case LED_SMALL_RED_DISABLE:
                switch (button->function)
                {
                    case LED_FUNC_POWER:
                        delete powerLedPointer;
                    break;
                    case LED_FUNC_STOP:
                        delete stopLedPointer;
                    break;
                    case LED_FUNC_READY:
                        delete readyLedPointer;
                    break;
                    case LED_FUNC_ERROR:
                        delete errorLedPointer;
                    break;
                    case LED_FUNC_Q:
                        delete qLedPointer;
                    break;
                    case LED_FUNC_NIBBLE:
                        delete nibbleLedPointer[button->value];
                    break;
                    case LED_FUNC_BIT:
                    case LED_FUNC_BUTTON:
                    case LED_FUNC_SWITCH:
                        delete ledPointer[button->value];
                    break;
                    case LED_FUNC_ADDRESS:
                        delete ledPointer[button->value+8];
                    break;
                    case LED_FUNC_CPUSTATE:
                        delete stateLedPointer[button->value];
                    break;
                }
            break;

            case TIL_311:
            case TIL_313:
            case TIL_313_ITALIC:
                switch (button->function)
                {
                    case TIL_DATA:
                        delete dataPointer[button->value];
                    break;

                    case TIL_ADDRESS:
                        delete addressPointer[button->value];
                    break;
                        
                    case TIL_MULTI:
                        delete multiPointer[button->value];
                    break;
                }
            break;
                
            case PANEL_PNG:
                delete button->bitmapPointer;
            break;
        }
    }
}

void Panel::init(vector<GuiItemConfiguration> buttonConfig, wxSize panelSize, int picInterruptNumber)
{    
    picInterruptNumber_ = picInterruptNumber;

    readyLedPointerDefined = false;
    stopLedPointerDefined = false;
    powerLedPointerDefined = false;
    errorLedPointerDefined = false;
    qLedPointerDefined = false;
    for (int i=0; i<MAX_DATA_TIL; i++)
    {
        tilDataPointerDefined[i] = false;
        nibbleLedPointerDefined[i] = false;
    }
    for (int i=0; i<MAX_BIT_LEDS; i++)
        ledPointerDefined[i] = false;
    for (int i=0; i<MAX_CPU_STATE_LEDS; i++)
        stateLedPointerDefined[i] = false;
    for (int i=0; i<MAX_ADDRESS_TIL; i++)
    {
        tilAddressPointerDefined[i] = false;
        stateLedPointerDefined[i] = false;
    }
    for (int i=0; i<16; i++)
    {
        adiArray_[i]=0;
        adsArray_[i]=0;
    }
    for (int i=0; i<MAX_THUMB_VALUE; i++)
        thumbValue[i] = 0;

    keyStart_ = 0;
    keyEnd_ = 0;
    lastKey_ = 0;
    forceUpperCase_ = p_Main->getUpperCase();
    
    guiItemConfiguration = buttonConfig;
    panelSize_ = panelSize;
    runButtonDefined = false;
    powerButtonDefined = false;
    waitButtonDefined = false;
    clearButtonDefined = false;
    clearRunButtonDefined = false;
    clearResetButtonDefined = false;
    inButtonDefined = false;
    spinCtrlAdiDefined = false;
    spinCtrlAdiVoltDefined = false;
    spinCtrlAdsDefined = false;
    spinCtrlAdsVoltDefined = false;
    
    wxClientDC dc(this);
        
    for (std::vector<GuiItemConfiguration>::iterator button = guiItemConfiguration.begin (); button != guiItemConfiguration.end (); ++button)
    {
        xmlButtonDefined_ = true;
                
        switch (button->type)
        {
            case SWITCH_BUTTON_VERTICAL:
            case SWITCH_BUTTON_VERTICAL_RED:
            case SWITCH_BUTTON_VERTICAL_GREEN:
            case SWITCH_BUTTON_HORIZONTAL:
            case PUSH_BUTTON_ROUND_RED:
            case PUSH_BUTTON_ROUND_BLACK:
            case PUSH_BUTTON_ROUND_RED_LARGE:
            case PUSH_BUTTON_ROUND_GREEN_LARGE:
            case ELF2K_POWER_BUTTON:
            case SWITCH_BUTTON_ORANGE:
            case SWITCH_BUTTON_YELLOW:
            case SWITCH_BUTTON_SQUARE_GREEN:
            case SWITCH_BUTTON_BLUE:
            case THUMB_MINUS_BUTTON:
            case THUMB_PLUS_BUTTON:
                button->switchButton = new SwitchButton(dc, button->type, wxColour(255, 255, 255), button->initup, button->position.x, button->position.y, button->label);
                switch (button->function)
                {
                    case BUTTON_FUNC_RUN:
                        runButton = button;
                        runButtonDefined = true;
                    break;

                    case BUTTON_FUNC_POWER:
                        powerButton = button;
                        powerButtonDefined = true;
                    break;

                    case BUTTON_FUNC_WAIT:
                        waitButton = button;
                        waitButtonDefined = true;
                    break;

                    case BUTTON_FUNC_CLEAR:
                        clearButton = button;
                        clearButtonDefined = true;
                    break;

                    case BUTTON_FUNC_CLEAR_RUN:
                        clearRunButton = button;
                        clearRunButtonDefined = true;
                    break;

                    case BUTTON_FUNC_CLEAR_RESET:
                        clearResetButton = button;
                        clearResetButtonDefined = true;
                    break;

                    case BUTTON_FUNC_IN:
                    case BUTTON_FUNC_IN_SWITCH:
                    case BUTTON_FUNC_IN_INTERRUPT:
                        inButton = button;
                        inButtonDefined = true;
                    break;

                    case BUTTON_FUNC_THUMB_PLUS:
                    case BUTTON_FUNC_THUMB_MINUS:
                        p_Computer->setThumbSwitch(0);
                    break;
                }
            break;

            case DIP_SWITCH_BUTTON:
                button->dipSwitchButton = new SwitchButton(dc, button->type, wxColour(255, 255, 255), button->initup, button->position.x, button->position.y, "");
            break;
                
            case ROT_SWITCH_BUTTON:
                button->rotButton = new RotButton(dc, 0, button->position.x, button->position.y, button->type);
            break;

            case ADI_SPINCTRL:
                spinCtrlAdiDefined = false;
                spinCtrlAdi = new wxSpinCtrl(this, 0x70, wxEmptyString, button->position, wxDefaultSize, wxSP_ARROW_KEYS, button->rangeLow, button->rangeHigh);
            break;

            case ADI_VOLT_SPINCTRL:
                spinCtrlAdiVoltDefined = false;
                spinCtrlAdiVolt = new wxSpinCtrl(this, 0x71, wxEmptyString, button->position, wxDefaultSize, wxSP_ARROW_KEYS, button->rangeLow, button->rangeHigh);
            break;

            case ADS_SPINCTRL:
                spinCtrlAdsDefined = false;
                spinCtrlAds = new wxSpinCtrl(this, 0x72, wxEmptyString, button->position, wxDefaultSize, wxSP_ARROW_KEYS, button->rangeLow, button->rangeHigh);
            break;

            case ADS_VOLT_SPINCTRL:
                spinCtrlAdsVoltDefined = false;
                spinCtrlAdsVolt = new wxSpinCtrl(this, 0x73, wxEmptyString, button->position, wxDefaultSize, wxSP_ARROW_KEYS, button->rangeLow, button->rangeHigh);
            break;
                
            case PUSH_BUTTON:
            case PUSH_BUTTON_SMALL:
            case PUSH_BUTTON_RECTANGLE:
            case PUSH_BUTTON_RECTANGLE_SMALL:
#if defined (__WXMAC__)
                button->hexButton = new HexButton(dc, button->type, button->position.x, button->position.y, button->label);
#else
                button->windowsButton = new wxButton(this, button->value, button->label, button->position, button->size, 0, wxDefaultValidator, "");
#endif
            break;

            case PUSH_BUTTON_RED:
            case PUSH_BUTTON_ORANGE:
            case PUSH_BUTTON_YELLOW:
            case PUSH_BUTTON_GREEN:
            case PUSH_BUTTON_BLUE:
            case PUSH_BUTTON_PIO:
                button->hexButton = new HexButton(dc, button->type, button->position.x, button->position.y, button->label);
            break;

            case LED_SMALL_RED:
            case LED_REAL_RED:
            case LED_REAL_GREEN:
            case LED_REAL_ORANGE:
            case LED_LARGE_RED:
            case LED_LARGE_GREEN:
            case LED_LARGE_ORANGE:
            case LED_LARGE_COLOR:
            case LED_SMALL_RED_DISABLE:
                switch (button->function)
                {
                    case LED_FUNC_POWER:
                        powerLedPointer = new Led(dc, button->position.x, button->position.y, button->type, button->reversePol);
                        powerLedPointerDefined = true;
                    break;
                    case LED_FUNC_STOP:
                        stopLedPointer = new Led(dc, button->position.x, button->position.y, button->type, button->reversePol);
                        stopLedPointerDefined = true;
                    break;
                    case LED_FUNC_READY:
                        readyLedPointer = new Led(dc, button->position.x, button->position.y, button->type, button->reversePol);
                        readyLedPointerDefined = true;
                    break;
                    case LED_FUNC_ERROR:
                        errorLedPointer = new Led(dc, button->position.x, button->position.y, button->type, button->reversePol);
                        errorLedPointerDefined = true;
                    break;
                    case LED_FUNC_Q:
                        qLedPointer = new Led(dc, button->position.x, button->position.y, button->type, button->reversePol);
                        qLedPointerDefined = true;
                    break;
                    case LED_FUNC_NIBBLE:
                        if (button->value >= 2)
                            button->value = 2-1;
                        nibbleLedPointer[button->value] = new Led(dc, button->position.x, button->position.y, button->type, button->reversePol);
                        nibbleLedPointerDefined[button->value] = true;
                    break;
                    case LED_FUNC_BIT:
                    case LED_FUNC_BUTTON:
                    case LED_FUNC_SWITCH:
                        if (button->value >= MAX_BIT_LEDS)
                            button->value = MAX_BIT_LEDS -1;
                        ledPointer[button->value] = new Led(dc, button->position.x, button->position.y, button->type, button->reversePol);
                        ledPointerDefined[button->value] = true;
                    break;
                    case LED_FUNC_ADDRESS:
                        if ((button->value+8) >= MAX_BIT_LEDS)
                            button->value = MAX_BIT_LEDS - 9;
                        ledPointer[button->value+8] = new Led(dc, button->position.x, button->position.y, button->type, button->reversePol);
                        ledPointerDefined[button->value+8] = true;
                    break;
                    case LED_FUNC_CPUSTATE:
                        if (button->value >= MAX_CPU_STATE_LEDS)
                            button->value = MAX_CPU_STATE_LEDS -1;
                        stateLedPointer[button->value] = new Led(dc, button->position.x, button->position.y, button->type, button->reversePol);
                        stateLedPointerDefined[button->value] = true;
                    break;
                }
            break;
                
            case TIL_311:
                switch (button->function)
                {
                    case TIL_DATA:
                        dataPointer[button->value] = new Til311();
                        dataPointer[button->value]->init(dc, button->position.x, button->position.y);
                        tilDataPointerDefined[button->value] = true;
                        updateData_ = true;
                    break;

                    case TIL_ADDRESS:
                        addressPointer[button->value] = new Til311();
                        addressPointer[button->value]->init(dc, button->position.x, button->position.y);
                        tilAddressPointerDefined[button->value]=true;
                        updateAddress_ = true;
                    break;

                    case TIL_MULTI:
                        multiPointer[button->value] = new Tilfull(FULL_TIL_313);
                        multiPointer[button->value]->init(dc, button->position.x, button->position.y);
                        tilMultiPointerDefined[button->value] = true;
                        updateMulti_[button->value] = true;
                    break;
                }
            break;
                
            case TIL_313:
                switch (button->function)
                {
                    case TIL_DATA:
                        dataPointer[button->value] = new Til313();
                        dataPointer[button->value]->init(dc, button->position.x, button->position.y);
                        tilDataPointerDefined[button->value] = true;
                        updateData_ = true;
                    break;

                    case TIL_ADDRESS:
                        addressPointer[button->value] = new Til313();
                        addressPointer[button->value]->init(dc, button->position.x, button->position.y);
                        tilAddressPointerDefined[button->value]=true;
                        updateAddress_ = true;
                    break;

                    case TIL_MULTI:
                        multiPointer[button->value] = new Tilfull(FULL_TIL_313);
                        multiPointer[button->value]->init(dc, button->position.x, button->position.y);
                        tilMultiPointerDefined[button->value] = true;
                        updateMulti_[button->value] = true;
                    break;
                }
            break;

            case TIL_313_ITALIC:
                switch (button->function)
                {
                    case TIL_DATA:
                        dataPointer[button->value] = new Til313Italic(false);
                        dataPointer[button->value]->init(dc, button->position.x, button->position.y);
                        tilDataPointerDefined[button->value] = true;
                        updateData_ = true;
                    break;

                    case TIL_ADDRESS:
                        addressPointer[button->value] = new Til313Italic(false);
                        addressPointer[button->value]->init(dc, button->position.x, button->position.y);
                        tilAddressPointerDefined[button->value]=true;
                        updateAddress_ = true;
                    break;
                        
                    case TIL_MULTI:
                        multiPointer[button->value] = new Tilfull(FULL_TIL_313_ITALIC);
                        multiPointer[button->value]->init(dc, button->position.x, button->position.y);
                        tilMultiPointerDefined[button->value] = true;
                        updateMulti_[button->value] = true;
                    break;
                }
            break;

            case PANEL_PNG:
                if (button->useImageDir)
                    button->bitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + p_Main->getPathSep() + button->fileName, wxBITMAP_TYPE_PNG);
                else
                    button->bitmapPointer = new wxBitmap(button->dirName + button->fileName, wxBITMAP_TYPE_PNG);
            break;
        }
    }
    
    this->connectKeyEvent(this);
}

void Panel::init()
{
}

void Panel::init(int WXUNUSED(computerType))
{
}

void Panel::connectKeyEvent(wxWindow* pclComponent)
{
  if(pclComponent)
  {
    pclComponent->Connect(wxID_ANY,
                          wxEVT_KEY_DOWN,
                          wxKeyEventHandler(Panel::onKeyDown),
                          (wxObject*) NULL,
                          this);
    pclComponent->Connect(wxID_ANY,
                          wxEVT_KEY_UP,
                          wxKeyEventHandler(Panel::onKeyUp),
                          (wxObject*) NULL,
                          this);
    pclComponent->Connect(wxID_ANY,
                          wxEVT_CHAR,
                          wxKeyEventHandler(Panel::onChar),
                          (wxObject*) NULL,
                          this);

    wxWindowListNode* pclNode = pclComponent->GetChildren().GetFirst();
    while(pclNode)
    {
      wxWindow* pclChild = pclNode->GetData();
      this->connectKeyEvent(pclChild);
     
      pclNode = pclNode->GetNext();
    }
  }
}
 
void Panel::onPaint(wxPaintEvent&WXUNUSED(event))
{
    wxPaintDC dc(this);

#if defined(__WXMAC__)
    rePaintLeds(dc);
#endif

    dc.SetPen(p_Main->getGuiTextColour(GUI_COL_WHITE));
    dc.SetBrush(p_Main->getGuiTextColour(GUI_COL_WHITE));
    dc.DrawRectangle(0, 0, panelSize_.x, panelSize_.y);

    for (std::vector<GuiItemConfiguration>::iterator button = guiItemConfiguration.begin (); button != guiItemConfiguration.end (); ++button)
    {
        switch (button->type)
        {
            case PUSH_BUTTON_ROUND_RED:
            case PUSH_BUTTON_ROUND_BLACK:
            case PUSH_BUTTON_ROUND_RED_LARGE:
            case PUSH_BUTTON_ROUND_GREEN_LARGE:
            case SWITCH_BUTTON_VERTICAL:
            case SWITCH_BUTTON_VERTICAL_RED:
            case SWITCH_BUTTON_VERTICAL_GREEN:
            case SWITCH_BUTTON_HORIZONTAL:
            case ELF2K_POWER_BUTTON:
            case SWITCH_BUTTON_ORANGE:
            case SWITCH_BUTTON_YELLOW:
            case SWITCH_BUTTON_SQUARE_GREEN:
            case SWITCH_BUTTON_BLUE:
            case THUMB_MINUS_BUTTON:
            case THUMB_PLUS_BUTTON:
                button->switchButton->onPaint(dc);
            break;

            case DIP_SWITCH_BUTTON:
                button->dipSwitchButton->onPaint(dc);
            break;
                
            case ROT_SWITCH_BUTTON:
                button->rotButton->onPaint(dc);
            break;

            case PUSH_BUTTON:
            case PUSH_BUTTON_SMALL:
            case PUSH_BUTTON_RECTANGLE:
            case PUSH_BUTTON_RECTANGLE_SMALL:
#if defined (__WXMAC__)
                button->hexButton->onPaint(dc);
#endif
            break;

            case PUSH_BUTTON_RED:
            case PUSH_BUTTON_ORANGE:
            case PUSH_BUTTON_YELLOW:
            case PUSH_BUTTON_GREEN:
            case PUSH_BUTTON_BLUE:
            case PUSH_BUTTON_PIO:
                button->hexButton->onPaint(dc);
            break;

            case LED_SMALL_RED:
            case LED_REAL_RED:
            case LED_REAL_GREEN:
            case LED_REAL_ORANGE:
            case LED_LARGE_RED:
            case LED_LARGE_GREEN:
            case LED_LARGE_ORANGE:
            case LED_LARGE_COLOR:
            case LED_SMALL_RED_DISABLE:
                switch (button->function)
                {
                    case LED_FUNC_POWER:
                        powerLedPointer->onPaint(dc);
                    break;
                    case LED_FUNC_STOP:
                        stopLedPointer->onPaint(dc);
                    break;
                    case LED_FUNC_READY:
                        readyLedPointer->onPaint(dc);
                    break;
                    case LED_FUNC_ERROR:
                        errorLedPointer->onPaint(dc);
                    break;
                    case LED_FUNC_Q:
                        qLedPointer->onPaint(dc);
                    break;
                    case LED_FUNC_NIBBLE:
                        nibbleLedPointer[button->value]->onPaint(dc);
                    break;
                    case LED_FUNC_BIT:
                    case LED_FUNC_BUTTON:
                    case LED_FUNC_SWITCH:
                        ledPointer[button->value]->onPaint(dc);
                    break;
                    case LED_FUNC_ADDRESS:
                        ledPointer[button->value+8]->onPaint(dc);
                    break;
                    case LED_FUNC_CPUSTATE:
                        stateLedPointer[button->value]->onPaint(dc);
                    break;
                }
            break;

            case PANEL_PNG:
                dc.DrawBitmap(*button->bitmapPointer, button->position.x, button->position.y);
            break;

            case PANEL_TEXT:
            case THUMB_TEXT:
                dc.SetTextForeground(p_Main->getGuiTextColour(button->color));
                wxFont defaultFont(button->textSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
                dc.SetFont(defaultFont);
                if (button->type == PANEL_TEXT)
                    dc.DrawText(button->label, button->position.x, button->position.y);
                else
                {
                    wxString value ="0";
                    if (button->value < MAX_THUMB_VALUE)
                        value.Printf("%01X", thumbValue[button->value]);
                    dc.DrawText(value, button->position.x, button->position.y);
                }
            break;
        }
    }
    for (std::vector<GuiItemConfiguration>::iterator button = guiItemConfiguration.begin (); button != guiItemConfiguration.end (); ++button)
    {
        switch (button->type)
        {
            case TIL_311:
            case TIL_313:
            case TIL_313_ITALIC:
                switch (button->function)
                {
                    case TIL_DATA:
                        dataPointer[button->value]->onPaint(dc);
                    break;

                    case TIL_ADDRESS:
                        addressPointer[button->value]->onPaint(dc);
                    break;

                    case TIL_MULTI:
                        multiPointer[button->value]->onPaint(dc);
                    break;
                }
            break;
        }
    }
}

void Panel::refreshPanel()
{
    this->Refresh();
    this->Update();
}

void Panel::onChar(wxKeyEvent& event)
{
    if (p_Vt100[UART1] != NULL)
    {
        int key = event.GetKeyCode();
        if (!p_Vt100[UART1]->charPressed(event))
        {    
            if (forceUpperCase_ && key >= 'a' && key <= 'z')
                key -= 32;
            if (key > 255) key = 0;
            if (key !=0 && key < 128)
            {
                vtOut(key);
            }
        }
    }
    p_Computer->charEvent(event.GetKeyCode());
}

void Panel::vtOut(int value)
{
    if (keyEnd_ != keyStart_-1 ||(keyEnd_ ==25 && keyStart_ != 0))
    {
        keyBuffer_[keyEnd_++] = value;
        if (keyEnd_ == 26) keyEnd_ = 0;
//        p_Vt100[UART1]->dataAvailable();
        p_Computer->dataAvailable(UART1);
        if (value == 27) p_Vt100[UART1]->framingError(1);
    }
}

void Panel::onKeyDown(wxKeyEvent& event)
{
    int keycode;
    keycode = event.GetKeyCode();

    if (p_Vt100[UART1] != NULL)
    {
        if (keycode == lastKey_)
        {
            if (repeat_)
            {
                if (event.GetModifiers() != wxMOD_CONTROL || keycode != WXK_HOME || keycode != WXK_ESCAPE || keycode != WXK_SCROLL || keycode != WXK_TAB || keycode != WXK_RETURN)
                {
                    lastKey_ = keycode;
                    p_Vt100[UART1]->keyDownPressed(event);
                }
            }
        }
        else
        {
            lastKey_ = keycode;
            p_Vt100[UART1]->keyDownPressed(event);
        }
    }
    if (p_Main->checkFunctionKey(event))
        return;
    if (!p_Computer->keyDownExtended(keycode, event))
    {
        if (!p_Computer->keyDownPressed(keycode))
            event.Skip();
    }
}

void Panel::onKeyUp(wxKeyEvent& event)
{
    if (!functionKeyReleaseTwo_)
        p_Main->onKeyUp(event);
    functionKeyReleaseTwo_ = !functionKeyReleaseTwo_;

    if (p_Vt100[UART1] != NULL)
    {
        lastKey_ = 0;
        p_Vt100[UART1]->keyUpPressed();
    }
    if (!p_Computer->keyUpReleased(event.GetKeyCode(), event))
        event.Skip();
}

Byte Panel::getKey(Byte vtOut)
{
    if (keyStart_ == keyEnd_)
        return vtOut;
    vtOut = keyBuffer_[keyStart_++];
    if (keyStart_ == 26) keyStart_ = 0;
    if (keyStart_ != keyEnd_)
//        p_Vt100[UART1]->dataAvailable();
        p_Computer->dataAvailable(UART1);
    return vtOut;
}

void Panel::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);

    for (std::vector<GuiItemConfiguration>::iterator button = guiItemConfiguration.begin (); button != guiItemConfiguration.end (); ++button)
    {
        switch (button->type)
        {
            case PUSH_BUTTON_ROUND_RED:
            case PUSH_BUTTON_ROUND_BLACK:
            case PUSH_BUTTON_ROUND_RED_LARGE:
            case PUSH_BUTTON_ROUND_GREEN_LARGE:
            case SWITCH_BUTTON_VERTICAL:
            case SWITCH_BUTTON_VERTICAL_RED:
            case SWITCH_BUTTON_VERTICAL_GREEN:
            case SWITCH_BUTTON_HORIZONTAL:
            case ELF2K_POWER_BUTTON:
            case SWITCH_BUTTON_ORANGE:
            case SWITCH_BUTTON_YELLOW:
            case SWITCH_BUTTON_SQUARE_GREEN:
            case SWITCH_BUTTON_BLUE:
            case THUMB_MINUS_BUTTON:
            case THUMB_PLUS_BUTTON:
                if (button->switchButton->onMousePress(dc, x, y))
                    executeMousePressFunction(button);
            break;
                
            case PUSH_BUTTON:
            case PUSH_BUTTON_SMALL:
            case PUSH_BUTTON_RECTANGLE:
            case PUSH_BUTTON_RECTANGLE_SMALL:
#if defined (__WXMAC__)
                if (button->hexButton->onMousePress(dc, x, y))
                    executeMousePressFunction(button);
#endif
            break;

            case PUSH_BUTTON_RED:
            case PUSH_BUTTON_ORANGE:
            case PUSH_BUTTON_YELLOW:
            case PUSH_BUTTON_GREEN:
            case PUSH_BUTTON_BLUE:
            case PUSH_BUTTON_PIO:
                if (button->hexButton->onMousePress(dc, x, y))
                    executeMousePressFunction(button);
            break;
        }
    }
}

void Panel::onMouseRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);

    for (std::vector<GuiItemConfiguration>::iterator button = guiItemConfiguration.begin (); button != guiItemConfiguration.end (); ++button)
    {
        switch (button->type)
        {
            case PUSH_BUTTON_ROUND_RED:
            case PUSH_BUTTON_ROUND_BLACK:
            case PUSH_BUTTON_ROUND_RED_LARGE:
            case PUSH_BUTTON_ROUND_GREEN_LARGE:
            case SWITCH_BUTTON_VERTICAL:
            case SWITCH_BUTTON_VERTICAL_RED:
            case SWITCH_BUTTON_VERTICAL_GREEN:
            case SWITCH_BUTTON_HORIZONTAL:
            case ELF2K_POWER_BUTTON:
            case SWITCH_BUTTON_ORANGE:
            case SWITCH_BUTTON_YELLOW:
            case SWITCH_BUTTON_SQUARE_GREEN:
            case SWITCH_BUTTON_BLUE:
                if (button->switchButton->onMouseRelease(dc, x, y))
                    executeMouseReleaseFunction(button);
            break;

            case DIP_SWITCH_BUTTON:
                if (button->dipSwitchButton->onMousePress(dc, x, y))
                    executeMouseReleaseFunction(button);
            break;
                
            case ROT_SWITCH_BUTTON:
                if (button->rotButton->onMouseLeftRelease(dc, x, y))
                    executeMouseReleaseFunction(button);
            break;

            case PUSH_BUTTON:
            case PUSH_BUTTON_SMALL:
            case PUSH_BUTTON_RECTANGLE:
            case PUSH_BUTTON_RECTANGLE_SMALL:
#if defined (__WXMAC__)
                if (button->hexButton->onMouseRelease(dc, x, y))
                    executeMouseReleaseFunction(button);
#endif
            break;

            case PUSH_BUTTON_RED:
            case PUSH_BUTTON_ORANGE:
            case PUSH_BUTTON_BLUE:
            case PUSH_BUTTON_YELLOW:
            case PUSH_BUTTON_GREEN:
            case PUSH_BUTTON_PIO:
                if (button->hexButton->onMouseRelease(dc, x, y))
                    executeMouseReleaseFunction(button);
            break;
        }
    }
}

void Panel::onMouseRightRelease(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);

    for (std::vector<GuiItemConfiguration>::iterator button = guiItemConfiguration.begin (); button != guiItemConfiguration.end (); ++button)
    {
        switch (button->type)
        {
            case ROT_SWITCH_BUTTON:
                if (button->rotButton->onMouseRightRelease(dc, x, y))
                    executeMouseRightReleaseFunction(button);
            break;

            case SWITCH_BUTTON_VERTICAL:
            case SWITCH_BUTTON_HORIZONTAL:
                if (button->switchButton->onMouseRelease(dc, x, y))
                    executeMouseReleaseFunction(button);
            break;
        }
    }
}

void Panel::executeMousePressFunction(std::vector<GuiItemConfiguration>::iterator button)
{
    if (!button->actOnPress)
        return;
    
    switch (button->function)
    {
        case BUTTON_FUNC_IN:
            p_Computer->onInButtonPress(IN_BUTTON_PUSH);
        break;

        case BUTTON_FUNC_IN_SWITCH:
            p_Computer->onInButtonPress(IN_BUTTON_SWITCH);
        break;
            
        case BUTTON_FUNC_IN_INTERRUPT:
            p_Computer->onInButtonPress(IN_BUTTON_PUSH);
            p_Computer->requestInterrupt(INTERRUPT_TYPE_INPUT, true, picInterruptNumber_);
        break;

        case BUTTON_FUNC_HEX:
            p_Computer->charEvent(button->value);
            p_Computer->onNumberKeyDown(button->value);
        break;

        case BUTTON_FUNC_EF:
            if (button->rangeLow != -1)
            {
                for (int ef=button->rangeLow; ef<=button->rangeHigh; ef++)
                    p_Computer->onEfKeyDown(ef);
            }
            else
                p_Computer->onEfKeyDown(button->value);
        break;

        case BUTTON_FUNC_RUN:
            p_Computer->onRunButtonPress(false);
        break;

        case BUTTON_FUNC_CLEAR_RESET:
            p_Computer->onClearResetButtonPress();
        break;

        case BUTTON_FUNC_THUMB_PLUS:
            if (button->value < MAX_THUMB_VALUE)
            {
                thumbValue[button->value]++;
                thumbValue[button->value] &= 0xf;
            }
            p_Computer->setThumbSwitch((thumbValue[1] << 4) + thumbValue[0]);
        break;

        case BUTTON_FUNC_THUMB_MINUS:
            if (button->value < MAX_THUMB_VALUE)
            {
                thumbValue[button->value]--;
                thumbValue[button->value] &= 0xf;
            }
            p_Computer->setThumbSwitch((thumbValue[1] << 4) + thumbValue[0]);
        break;

        default:
            executeMouseLeftFunction(button);
        break;
    }
}

void Panel::executeMouseReleaseFunction(std::vector<GuiItemConfiguration>::iterator button)
{
    if (!button->actOnRelease)
        return;
    
    switch (button->function)
    {
        case BUTTON_FUNC_IN:
        case BUTTON_FUNC_IN_SWITCH:
        case BUTTON_FUNC_IN_INTERRUPT:
            p_Computer->onInButtonRelease();
        break;
            
        case BUTTON_FUNC_HEX:
            p_Computer->keyUpReleased(button->value);
            p_Computer->onNumberKeyUp();
        break;

        case BUTTON_FUNC_EF:
            if (button->rangeLow != -1)
            {
                for (int ef=button->rangeLow; ef<=button->rangeHigh; ef++)
                    p_Computer->onEfKeyUp(ef);
            }
            else
                p_Computer->onEfKeyUp(button->value);
        break;

        case BUTTON_FUNC_RUN:
            if (button->type >= PUSH_BUTTON)
                p_Computer->onRunButtonRelease();
            else
                p_Computer->onRunButtonPress(false);
        break;

        case BUTTON_FUNC_CLEAR_RESET:
            p_Computer->onClearButtonRelease();
        break;

        default:
            executeMouseLeftFunction(button);
        break;
    }
}

void Panel::executeMouseLeftFunction(std::vector<GuiItemConfiguration>::iterator button)
{
    switch (button->function)
    {
        case BUTTON_FUNC_CARD:
            p_Computer->cardButton(button->value);
        break;

        case BUTTON_FUNC_LOAD:
            p_Computer->onLoadButton(button->type >= PUSH_BUTTON);
        break;

        case BUTTON_FUNC_READ:
            p_Computer->onReadButton();
        break;

        case BUTTON_FUNC_CARDSWITCH:
            p_Computer->onCardButtonSwitch();
        break;

        case BUTTON_FUNC_NANO_MONITOR:
            p_Computer->onNanoMonitor();
        break;

        case BUTTON_FUNC_STOP:
            p_Main->stopComputer();
        break;

        case BUTTON_FUNC_POWER:
            p_Computer->onPowerButton();
        break;

        case BUTTON_FUNC_WAIT:
            p_Computer->onWaitButton();
        break;

        case BUTTON_FUNC_RESET:
            p_Computer->onResetButton();
        break;
            
        case BUTTON_FUNC_SUPER_MONITOR:
            p_Computer->onMonitor();
        break;

        case BUTTON_FUNC_CLEAR:
            p_Computer->onClearButton();
        break;

        case BUTTON_FUNC_CLEAR_RUN:
            p_Computer->onClearSwitch();
        break;

        case BUTTON_FUNC_STEP:
            p_Computer->onSingleStep();
        break;

        case BUTTON_FUNC_VELF:
            p_Computer->switchBootStrap();
        break;

        case BUTTON_FUNC_BIT:
            p_Computer->dataSwitch(button->value&0x7);
        break;
            
        case BUTTON_FUNC_EF_SWITCH:
            p_Computer->efSwitch((button->value-1)&0x3);
        break;

        case BUTTON_FUNC_MP:
            p_Computer->onMpButton();
        break;

        case BUTTON_FUNC_SUPER_MP:
            p_Computer->onMpSuperButton();
        break;

        case BUTTON_FUNC_NVRAM_DISABLE:
            p_Computer->onRamButton();
        break;
            
        case BUTTON_FUNC_DIPMP:
            p_Computer->onMpButton(button->value&0x3);
        break;

        case BUTTON_FUNC_EMS:
            p_Computer->onEmsButton(button->value, true);
        break;

        case BUTTON_FUNC_PAUSE:
            p_Computer->onPause();
        break;
            
        case BUTTON_FUNC_RUN0:
            p_Computer->onRunButtonPress(true);
        break;

        default:
        break;
    }
}

void Panel::executeMouseRightReleaseFunction(std::vector<GuiItemConfiguration>::iterator button)
{
    if (!button->actOnRelease)
        return;
    
    switch (button->function)
    {
        case BUTTON_FUNC_EMS:
            p_Computer->onEmsButton(button->value, false);
        break;

        default:
        break;
    }
}

void Panel::ledTimeout()
{
#if defined(__WXMAC__)
    p_Main->eventRefreshPanel();
#else
    wxClientDC dc(this);
    rePaintLeds(dc);
#endif
}

void Panel::rePaintLeds(wxDC& dc)
{
    updateReadyLed(dc);
    updateStopLed(dc);
    updatePowerLed(dc);
    updateErrorLed(dc);
    updateQLed(dc);
    for (int i=0; i<2; i++)
    {
        if (nibbleLedPointerDefined[i])
            updateNibbleLed(dc, i);
    }
    for (int i=0; i<MAX_BIT_LEDS; i++)
    {
        if (ledPointerDefined[i])
            updateLed(dc, i);
    }
    for (int i=0; i<MAX_CPU_STATE_LEDS; i++)
    {
        if (stateLedPointerDefined[i])
            updateStateLed(dc, i);
    }
    for (int i=0; i<MAX_MULTI_TIL; i++)
    {
        updateSeg(dc, i);
        updateMulti(dc, i);
    }
    updateData(dc);
    updateDp313Italic(dc);
    updateAddress(dc);
}

void Panel::setLedMs(long ms)
{
    ms_ = ms;
}

void Panel::updateReadyLed(wxDC& dc)
{
    if (updateReadyLed_)
    {
        readyLedPointer->setStatus(dc, readyLedStatus);
        updateReadyLed_ = false;
    }
}

void Panel::setReadyLed(int status)
{
    if (!readyLedPointerDefined)
        return;
    
    if (readyLedStatus != status)
    {
        readyLedStatus = status;
        updateReadyLed_ = true;
        if (ms_ == 0)
        {
#if defined(__WXMAC__)
            p_Main->eventRefreshPanel();
#else
            wxClientDC dc(this);
            updateReadyLed(dc);
#endif
        }
    }
}

void Panel::setStopLed(int status)
{
    if (!stopLedPointerDefined)
        return;
    
    if (stopLedStatus != status)
    {
        stopLedStatus = status;
        updateStopLed_ = true;
        if (ms_ == 0)
        {
#if defined(__WXMAC__)
            p_Main->eventRefreshPanel();
#else
            wxClientDC dc(this);
            updateStopLed(dc);
#endif
        }
    }
}

void Panel::updateStopLed(wxDC& dc)
{
    if (updateStopLed_)
    {
        stopLedPointer->setStatus(dc, stopLedStatus);
        updateStopLed_ = false;
    }
}

void Panel::setPowerLed(int status)
{
    if (!powerLedPointerDefined)
        return;
    
    if (powerLedStatus != status)
    {
        powerLedStatus = status;
        updatePowerLed_ = true;
        if (ms_ == 0)
        {
#if defined(__WXMAC__)
            p_Main->eventRefreshPanel();
#else
            wxClientDC dc(this);
            updatePowerLed(dc);
#endif
        }
    }
}

void Panel::updatePowerLed(wxDC& dc)
{
    if (updatePowerLed_)
    {
        powerLedPointer->setStatus(dc, powerLedStatus);
        updatePowerLed_ = false;
    }
}

void Panel::setErrorLed(int status)
{
    if (!errorLedPointerDefined)
        return;
    
    if (errorLedStatus != status)
    {
        errorLedStatus = status;
        updateErrorLed_ = true;
        if (ms_ == 0)
        {
#if defined(__WXMAC__)
            p_Main->eventRefreshPanel();
#else
            wxClientDC dc(this);
            updateErrorLed(dc);
#endif
        }
    }
}

void Panel::updateErrorLed(wxDC& dc)
{
    if (updateErrorLed_)
    {
        errorLedPointer->setStatus(dc, errorLedStatus);
        updateErrorLed_ = false;
    }
}

void Panel::setQLed(int status)
{
    if (!qLedPointerDefined)
        return;
    
    if (qLedStatus != status)
    {
        qLedStatus = status;
        updateQLed_ = true;
        if (ms_ == 0)
        {
#if defined(__WXMAC__)
            p_Main->eventRefreshPanel();
#else
            wxClientDC dc(this);
            updateQLed(dc);
#endif
        }
    }
}

void Panel::updateQLed(wxDC& dc)
{
    if (updateQLed_)
    {
        qLedPointer->setStatus(dc, qLedStatus);
        updateQLed_ = false;
    }
}

void Panel::setLed(int i, int status)
{
    if (!ledPointerDefined[i])
        return;

    if (ledStatus[i] != status)
    {
        ledStatus[i] = status;
        updateLed_[i] = true;
        if (ms_ == 0)
        {
#if defined(__WXMAC__)
            p_Main->eventRefreshPanel();
#else
            wxClientDC dc(this);
            updateLed(dc, i);
#endif
        }
    }
}

void Panel::updateLed(wxDC& dc, int i)
{
    if (updateLed_[i])
    {
        ledPointer[i]->setStatus(dc, ledStatus[i]);
        updateLed_[i] = false;
    }
}

void Panel::refreshLed(wxDC& dc, int i)
{
    ledPointer[i]->setStatus(dc, ledStatus[i]);
    updateLed_[i] = false;
}

void Panel::setNibbleLed(int i, int status)
{
    if (!nibbleLedPointerDefined[i])
        return;
    
    if (nibbleLedStatus[i] != status)
    {
        nibbleLedStatus[i] = status;
        updateNibbleLed_[i] = true;
        if (ms_ == 0)
        {
#if defined(__WXMAC__)
            p_Main->eventRefreshPanel();
#else
            wxClientDC dc(this);
            updateNibbleLed(dc, i);
#endif
        }
    }
}

void Panel::updateNibbleLed(wxDC& dc, int i)
{
    if (updateNibbleLed_[i])
    {
        nibbleLedPointer[i]->setStatus(dc, nibbleLedStatus[i]);
        updateNibbleLed_[i] = false;
    }
}

void Panel::setStateLed(int i, int status)
{
    if (!stateLedPointerDefined[i])
        return;

    if (stateLedStatus[i] != status)
    {
        stateLedStatus[i] = status;
        updateStateLed_[i] = true;
        if (ms_ == 0)
        {
#if defined(__WXMAC__)
            p_Main->eventRefreshPanel();
#else
            wxClientDC dc(this);
            updateStateLed(dc, i);
#endif
        }
    }
}

void Panel::updateStateLed(wxDC& dc, int i)
{
    if (updateStateLed_[i])
    {
        stateLedPointer[i]->setStatus(dc, stateLedStatus[i]);
        updateStateLed_[i] = false;
    }
}

void Panel::showData(Byte value)
{
    if (dataStatus != value)
    {
        dataStatus = value;
        updateData_ = true;
        if (ms_ == 0)
        {
#if defined(__WXMAC__)
            p_Main->eventRefreshPanel();
#else
            wxClientDC dc(this);
            updateData(dc);
#endif
        }
    }
}

void Panel::updateData(wxDC& dc)
{
    if (updateData_)
    {
        if (tilDataPointerDefined[0])
            dataPointer[0]->update(dc,(dataStatus>>4)&15);
        if (tilDataPointerDefined[1])
            dataPointer[1]->update(dc, dataStatus&15);
        updateData_ = false;
    }
}

void Panel::showDp313Italic(bool status, int i)
{
    if (!tilDataPointerDefined[i])
        return;

    if (dpStatus[i] != status)
    {
        dpStatus[i] = status;
        updateDp313_[i] = true;
        if (ms_ == 0)
        {
#if defined(__WXMAC__)
            p_Main->eventRefreshPanel();
#else
            wxClientDC dc(this);
            updateDp313Italic(dc);
#endif
        }
    }
}

void Panel::turnOff313Italic(bool status)
{
    wxClientDC dc(this);
    if (tilDataPointerDefined[0])
        dataPointer[0]->turnOff(dc, status);
    if (tilDataPointerDefined[1])
        dataPointer[1]->turnOff(dc, status);
    if (tilAddressPointerDefined[0])
        addressPointer[0]->turnOff(dc, status);
    if (tilAddressPointerDefined[1])
        addressPointer[1]->turnOff(dc, status);
    if (tilAddressPointerDefined[2])
        addressPointer[2]->turnOff(dc, status);
    if (tilAddressPointerDefined[3])
        addressPointer[3]->turnOff(dc, status);
}

void Panel::updateDp313Italic(wxDC& dc)
{
    for (int i=0; i<MAX_DATA_TIL; i++)
    {
        if (updateDp313_[i])
        {
            dataPointer[i]->dp(dc, dpStatus[i]);
            updateDp313_[i] = false;
        }
    }
}

void Panel::showSeg(int number, Byte value)
{
    if (segStatus[number] != value)
    {
        segStatus[number] = value;
        updateSeg_[number] = true;
        if (ms_ == 0)
        {
#if defined(__WXMAC__)
            p_Main->eventRefreshPanel();
#else
            wxClientDC dc(this);
            updateSeg(dc, number);
#endif
        }
    }
}

void Panel::updateSeg(wxDC& dc, int number)
{
    if (updateSeg_[number])
    {
        segPointer[number]->update(dc, segStatus[number], number);
        updateSeg_[number] = false;
    }
}

void Panel::showMulti(int number, Byte value)
{
    if (!tilMultiPointerDefined[number])
        return;
    
    if (multiStatus[number] != value)
    {
        multiStatus[number] = value;
        updateMulti_[number] = true;
        if (ms_ == 0)
        {
#if defined(__WXMAC__)
            p_Main->eventRefreshPanel();
#else
            wxClientDC dc(this);
            updateMulti(dc, number);
#endif
        }
    }
}

void Panel::updateMulti(wxDC& dc, int number)
{
    if (updateMulti_[number])
    {
        if (tilMultiPointerDefined[number])
            multiPointer[number]->update(dc, multiStatus[number], number);
        updateMulti_[number] = false;
    }
}

void Panel::showAddress(Word address)
{
    if (addressStatus != address)
    {
        addressStatus = address;
        updateAddress_ = true;
        if (ms_ == 0)
        {
#if defined(__WXMAC__)
            p_Main->eventRefreshPanel();
#else
            wxClientDC dc(this);
            updateAddress(dc);
#endif
        }
    }
}

void Panel::updateAddress(wxDC& dc)
{
    if (updateAddress_)
    {
        if (tilAddressPointerDefined[0])
            addressPointer[0]->update(dc, addressStatus>>12);
        if (tilAddressPointerDefined[1])
            addressPointer[1]->update(dc,(addressStatus>>8)&15);
        if (tilAddressPointerDefined[2])
            addressPointer[2]->update(dc,(addressStatus>>4)&15);
        if (tilAddressPointerDefined[3])
            addressPointer[3]->update(dc, addressStatus&15);
        updateAddress_ = false;
    }
}

void Panel::inUp()
{
}

void Panel::inDown()
{
}

void Panel::inSetState(bool state)
{
    if (!inButtonDefined)
        return;

    wxClientDC dc(this);
    inButton->switchButton->setState(dc, state);
}

void Panel::clearSetState(bool state)
{
    if (!clearButtonDefined)
        return;

    wxClientDC dc(this);
    clearButton->switchButton->setState(dc, state);
}

void Panel::clearResetSetState(bool state)
{
    if (!clearResetButtonDefined)
        return;

    wxClientDC dc(this);
    clearResetButton->switchButton->setState(dc, state);
}

//void Panel::resetSetState(bool state)
//{
//#if defined(__WXMAC__)
//    p_Main->eventRefreshPanel();
//#else
//    wxClientDC dc(this);
//    resetSwitchButton->setState(dc, state);
//#endif
//}

void Panel::waitSetState(bool state)
{
    if (!waitButtonDefined)
        return;
    
    wxClientDC dc(this);
    waitButton->switchButton->setState(dc, state);
}

void Panel::runSetState(bool state)
{
    if (!runButtonDefined)
        return;

    wxClientDC dc(this);
    runButton->switchButton->setState(dc, state);
}

void Panel::powerSetState(bool state)
{
    if (powerButtonDefined)
        return;
    
    wxClientDC dc(this);
    powerButton->switchButton->setState(dc, state);
}

void Panel::releaseButtonOnScreen(HexButton* buttonPoint)
{
    wxClientDC dc(this);
    
    buttonPoint->releaseButtonOnScreen(dc);
}

void Panel::onAdsChannel(wxSpinEvent&event)
{
    if (spinCtrlAdsVoltDefined)
        spinCtrlAdsVolt->SetValue(adsArray_[event.GetPosition()]);
}

void Panel::onAdsVolt(wxSpinEvent&event)
{
    if (spinCtrlAdsDefined)
        adsArray_[spinCtrlAds->GetValue()] = event.GetPosition();
}

void Panel::onAdsChannelText(wxCommandEvent&WXUNUSED(event))
{
    if (spinCtrlAdsVoltDefined && spinCtrlAdsDefined)
        spinCtrlAdsVolt->SetValue(adsArray_[spinCtrlAds->GetValue()]);
}

void Panel::onAdsVoltText(wxCommandEvent&WXUNUSED(event))
{
    if (spinCtrlAdsVoltDefined && spinCtrlAdsDefined)
        adsArray_[spinCtrlAds->GetValue()] = spinCtrlAdsVolt->GetValue();
}

void Panel::onAdiChannel(wxSpinEvent&event)
{
    if (spinCtrlAdiVoltDefined)
        spinCtrlAdiVolt->SetValue(adiArray_[event.GetPosition()]);
}

void Panel::onAdiVolt(wxSpinEvent&event)
{
    if (spinCtrlAdiDefined)
        adiArray_[spinCtrlAdi->GetValue()] = event.GetPosition();
}

void Panel::onAdiChannelText(wxCommandEvent&WXUNUSED(event))
{
    if (spinCtrlAdiVoltDefined && spinCtrlAdiDefined)
        spinCtrlAdiVolt->SetValue(adiArray_[spinCtrlAdi->GetValue()]);
}

void Panel::onAdiVoltText(wxCommandEvent&WXUNUSED(event))
{
    if (spinCtrlAdiVoltDefined && spinCtrlAdiDefined)
        adiArray_[spinCtrlAdi->GetValue()] = spinCtrlAdiVolt->GetValue();
}

void Panel::setAds(int i, int val)
{
    if (spinCtrlAdsDefined)
        adsArray_[i] = val;
}

bool Panel::isAdsDefined()
{
    return spinCtrlAdsDefined;
}

int Panel::getAds(int i)
{
    return adsArray_[i];
}

void Panel::setAdi(int i, int val)
{
    if (spinCtrlAdiDefined)
        adiArray_[i] = val;
}

bool Panel::isAdiDefined()
{
    return spinCtrlAdiDefined;
}

int Panel::getAdi(int i)
{
    return adiArray_[i];
}

BEGIN_EVENT_TABLE(PanelFrame, wxFrame)
    EVT_CLOSE (PanelFrame::onClose)
END_EVENT_TABLE()

PanelFrame::PanelFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    panelPointer = new Panel(this, size);
    
    this->SetClientSize(size);
    
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif
}

PanelFrame::~PanelFrame()
{
    delete panelPointer;
}

void PanelFrame::onClose(wxCloseEvent&event)
{
    p_Computer->onClose(event);
}


/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
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

#include "main.h"
#include "til313.h"

Til313::Til313()
{
    displayedNumber_ = 0;
    x_ = 0;
    y_ = 0;

    til313BitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/til313.png", wxBITMAP_TYPE_PNG);
    
    dcMemory.SelectObject(*til313BitmapPointer);
}

Til313::~Til313()
{
    delete til313BitmapPointer;
}

void Til313::init(wxDC& dc, int x, int y)
{
    x_ = x;
    y_ = y;

    dc.Blit(x_, y_, 25 , 33, &dcMemory, 0, 0);
}

void Til313::onPaint(wxDC& dc)
{
    int ox = (displayedNumber_ & 3) * 26;
    int oy = ((displayedNumber_>>2) & 3) * 34;
    dc.Blit(x_, y_, 25 , 33, &dcMemory, ox, oy);
}
 
void Til313::update(wxDC& dc, int NewNumber) 
{
    if (displayedNumber_ == NewNumber)  return;

    int ox = (NewNumber & 3) * 26;
    int oy = ((NewNumber>>2) & 3) * 34;
    dc.Blit(x_, y_, 25 , 33, &dcMemory, ox, oy);

    displayedNumber_ = NewNumber;
}

Til313Italic::Til313Italic(bool upsideDown)
{
    displayedNumber_ = 0;
    x_ = 0;
    y_ = 0;
    
    if (upsideDown)
    {
        til313BitmapPointer_led_off = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/til313_upside_down_led_off.png", wxBITMAP_TYPE_PNG);
        til313BitmapPointer_led_on = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/til313_upside_down_led_on.png", wxBITMAP_TYPE_PNG);
    }
    else
    {
        til313BitmapPointer_led_off = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/til313_led_off.png", wxBITMAP_TYPE_PNG);
        til313BitmapPointer_led_on = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/til313_led_on.png", wxBITMAP_TYPE_PNG);
        til313BitmapPointer_off = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/til313_off.png", wxBITMAP_TYPE_PNG);
    }

    dcMemory.SelectObject(*til313BitmapPointer_led_off);
}

Til313Italic::~Til313Italic()
{
    delete til313BitmapPointer_led_off;
    delete til313BitmapPointer_led_on;
}

void Til313Italic::init(wxDC& dc, int x, int y)
{
    x_ = x;
    y_ = y;
    
    dc.Blit(x_, y_, 28 , 36, &dcMemory, 0, 0);
}

void Til313Italic::onPaint(wxDC& dc)
{
    int ox = (displayedNumber_ & 3) * 29;
    int oy = ((displayedNumber_>>2) & 3) * 37;
    dc.Blit(x_, y_, 28 , 36, &dcMemory, ox, oy);
}

void Til313Italic::update(wxDC& dc, int NewNumber)
{
    if (displayedNumber_ == NewNumber)  return;
    
    int ox = (NewNumber & 3) * 29;
    int oy = ((NewNumber>>2) & 3) * 37;
    dc.Blit(x_, y_, 28 , 36, &dcMemory, ox, oy);
    
    displayedNumber_ = NewNumber;
}

void Til313Italic::dp(wxDC& dc, bool status)
{
    if (status)
        dcMemory.SelectObject(*til313BitmapPointer_led_on);
    else
        dcMemory.SelectObject(*til313BitmapPointer_led_off);
    
    int ox = (displayedNumber_ & 3) * 29;
    int oy = ((displayedNumber_>>2) & 3) * 37;
    dc.Blit(x_, y_, 28 , 36, &dcMemory, ox, oy);
}

void Til313Italic::turnOff(wxDC& dc, bool status)
{
    if (status)
    {
        dcMemory.SelectObject(*til313BitmapPointer_off);
        displayedNumber_ = 0;
    }
    else
        dcMemory.SelectObject(*til313BitmapPointer_led_off);
    
    int ox = (displayedNumber_ & 3) * 29;
    int oy = ((displayedNumber_>>2) & 3) * 37;
    dc.Blit(x_, y_, 28 , 36, &dcMemory, ox, oy);
}

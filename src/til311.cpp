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

/*
 *******************************************************************
 *** This software is copyright 2006 by Michael H Riley          ***
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
#include "til311.h"

Til311::Til311()
{
    displayedNumber_ = 0;
    x_ = 0;
    y_ = 0;

    til311BitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/til311.png", wxBITMAP_TYPE_PNG);
    dcMemory.SelectObject(*til311BitmapPointer);
}

Til311::~Til311()
{
    delete til311BitmapPointer;
}

void Til311::init(wxDC& dc, int x, int y)
{
    x_ = x;
    y_ = y;

    dc.Blit(x_, y_, 20 , 32, &dcMemory, 0, 0);
}

void Til311::onPaint(wxDC& dc)
{
    int ox = (displayedNumber_ & 3) * 21;
    int oy = ((displayedNumber_>>2) & 3) * 33;
    dc.Blit(x_, y_, 20 , 32, &dcMemory, ox, oy);
}
 
void Til311::update(wxDC& dc, int NewNumber) 
{
    if (displayedNumber_ == NewNumber)  return;

    int ox = (NewNumber & 3) * 21;
    int oy = ((NewNumber>>2) & 3) * 33;
    dc.Blit(x_, y_, 20 , 32, &dcMemory, ox, oy);

    displayedNumber_ = NewNumber;
}

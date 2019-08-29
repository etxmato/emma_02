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

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#if !wxUSE_COMBOCTRL
    #error "Please set wxUSE_COMBOCTRL to 1 and rebuild the library."
#endif

#include "main.h"
#include "led.h"

Led::Led(wxDC& dc, int x, int y, int ledType)
{
	status_ = 0;
	ledType_ = ledType;

	x_ = x;
	y_ = y;

	wxColour pink(255, 0, 255);
	wxColour white(255, 255, 255);

    wxString linuxExtension = "";
#if defined (__linux__)
    linuxExtension = "_linux";
#endif
    
	switch(ledType)
	{
		case ELF2KLED1:
			ledOnBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/Elf2Kredledon.png", wxBITMAP_TYPE_PNG);
			ledOffBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/Elf2Kredledoff.png", wxBITMAP_TYPE_PNG);
			maskOn = new wxMask(*ledOnBitmapPointer, pink);
			maskOff = new wxMask(*ledOnBitmapPointer, pink);
		break;

		case ELF2KLED2:
			ledOnBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/Elf2Kgreenledon.png", wxBITMAP_TYPE_PNG);
			ledOffBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/Elf2Kgreenledoff.png", wxBITMAP_TYPE_PNG);
			maskOn = new wxMask(*ledOnBitmapPointer, pink);
			maskOff = new wxMask(*ledOnBitmapPointer, pink);
		break;

		case ELF2KLED3:
			ledOnBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/Elf2Korangeledon.png", wxBITMAP_TYPE_PNG);
			ledOffBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/Elf2Korangeledoff.png", wxBITMAP_TYPE_PNG);
			maskOn = new wxMask(*ledOnBitmapPointer, pink);
			maskOff = new wxMask(*ledOnBitmapPointer, pink);
		break;

		case ELFLED:
		case ELFIILED:
		case SUPERELFLED:
		case COSMICOSLED:
			ledOnBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/ledon.png", wxBITMAP_TYPE_PNG);
			ledOffBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/ledoff.png", wxBITMAP_TYPE_PNG);
			maskOn = new wxMask(*ledOnBitmapPointer, white);
			maskOff = new wxMask(*ledOnBitmapPointer, white);
		break;

		case MEMBERLED:
			ledOnBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/largeledon.png", wxBITMAP_TYPE_PNG);
			ledOffBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/largeledoff.png", wxBITMAP_TYPE_PNG);
			maskOn = new wxMask(*ledOnBitmapPointer, white);
			maskOff = new wxMask(*ledOnBitmapPointer, white);
		break;

		case MEMBERLEDGREEN:
			ledOnBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/largeledredon.png", wxBITMAP_TYPE_PNG);
            ledOnGreenBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/largeledgreenon.png", wxBITMAP_TYPE_PNG);
            ledOnOrangeBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/largeledorangeon.png", wxBITMAP_TYPE_PNG);
			ledOffBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/largeledgreenoff.png", wxBITMAP_TYPE_PNG);
			maskOn = new wxMask(*ledOnBitmapPointer, white);
			maskOff = new wxMask(*ledOnBitmapPointer, white);
		break;

        case PIOLED:
            ledOnBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/ledon.png", wxBITMAP_TYPE_PNG);
            ledOffBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/ledoff.png", wxBITMAP_TYPE_PNG);
            ledOnGreenBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/leddisabled.png", wxBITMAP_TYPE_PNG);
            ledOnOrangeBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + linuxExtension + "/leddisabled.png", wxBITMAP_TYPE_PNG);
            maskOn = new wxMask(*ledOnBitmapPointer, white);
            maskOff = new wxMask(*ledOnBitmapPointer, white);
        break;
    }

	if (ledType == MEMBERLEDGREEN || ledType == PIOLED)
	{
		ledOnBitmapPointer->SetMask(maskOn);
        ledOnGreenBitmapPointer->SetMask(maskOn);
        ledOnOrangeBitmapPointer->SetMask(maskOn);
		ledOffBitmapPointer->SetMask(maskOff);
		dc.DrawBitmap(*ledOnBitmapPointer, x_, y_, true);
	}
	else
	{
		ledOnBitmapPointer->SetMask(maskOn);
		ledOffBitmapPointer->SetMask(maskOff);
		dc.DrawBitmap(*ledOnBitmapPointer, x_, y_, true);
	}
}

Led::~Led()
{
    if (ledType_ == MEMBERLEDGREEN || ledType_ == PIOLED)
	{
		delete ledOnBitmapPointer;
//		delete ledOnGreenBitmapPointer;
		delete ledOffBitmapPointer;
	}
	else
	{
		delete ledOnBitmapPointer;
		delete ledOffBitmapPointer;
	}
}

void Led::onPaint(wxDC& dc)
{
    if (ledType_ == MEMBERLEDGREEN || ledType_ == PIOLED)
	{
		switch (status_)
		{
			case 0:
				dc.DrawBitmap(*ledOffBitmapPointer, x_, y_, true);
			break;
			case 1:
				dc.DrawBitmap(*ledOnBitmapPointer, x_, y_, true);
			break;
			case 2:
				dc.DrawBitmap(*ledOnGreenBitmapPointer, x_, y_, true);
			break;
			case 3:
				dc.DrawBitmap(*ledOnOrangeBitmapPointer, x_, y_, true);
			break;
		}
	}
	else
	{
		if (status_)
			dc.DrawBitmap(*ledOnBitmapPointer, x_, y_, true);
		else
			dc.DrawBitmap(*ledOffBitmapPointer, x_, y_, true);
	}
}

void Led::setStatus(wxDC& dc, int status)
{
	status_ = status;
	onPaint(dc);
}



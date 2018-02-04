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
#include "til313.full.h"

int xPos[] =
{
	6, 16, 16, 6, 3, 3, 6, 19
};

int yPos[] =
{
	3, 5, 17, 27, 17, 5, 15, 27
};

int width[] =
{
	10, 3, 3, 10, 3, 3, 10, 3
};

int height[] =
{
	3, 11, 11, 3, 11, 11, 3, 3
};

Til313full::Til313full()
{
	displayedNumber_ = 0;
	x_ = 0;
	y_ = 0;

	tilBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/til313.full.png", wxBITMAP_TYPE_PNG);
	tilCopyBitmapPointer = new wxBitmap(25, 33);
	dcMemory.SelectObject(*tilBitmapPointer);
	dcCopy.SelectObject(*tilCopyBitmapPointer);
	dcCopy.Blit(0, 0, 25 , 33, &dcMemory, 0, 33);
}

Til313full::~Til313full()
{
	delete tilBitmapPointer;
	delete tilCopyBitmapPointer;
}

void Til313full::init(wxDC& dc, int x, int y)
{
	x_ = x;
	y_ = y;

	dc.Blit(x_, y_, 25 , 33, &dcCopy, 0, 0);
}

void Til313full::onPaint(wxDC& dc)
{
	dc.Blit(x_, y_, 25 , 33, &dcCopy, 0, 0);
}
 
void Til313full::update(wxDC& dc, Byte NewNumber) 
{
	if (displayedNumber_ == NewNumber)  return;

	int newN = NewNumber;
	for (int i=0; i<8; i++)
	{
		if ((displayedNumber_ & 1) != (newN & 1))
		{
			if (newN & 1) // ON
			{
				dcCopy.Blit(xPos[i], yPos[i], width[i], height[i], &dcMemory, xPos[i], yPos[i]);
			}
			else // OFF
			{
				dcCopy.Blit(xPos[i], yPos[i], width[i], height[i], &dcMemory, xPos[i], yPos[i]+33);
			}
		}
		newN = newN >> 1;
		displayedNumber_ = displayedNumber_ >> 1;
	}
	dc.Blit(x_, y_, 25 , 33, &dcCopy, 0, 0);
	displayedNumber_ = NewNumber;
}

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

int xPos1[] =
{
     6, 16, 16,  6,  3,  3,  6, 19,
    10, 20, 18,  6,  4,  6,  7, 22,
};

int yPos1[] =
{
     3,  5, 17, 27, 17,  5, 15, 27,
     2,  6, 20, 31, 19,  5, 17, 30,
};

int width1[] =
{
    10,  3,  3, 10,  3,  3, 10,  3,
    12,  5,  4, 12,  4,  4, 15,  4,
};

int height1[] =
{
     3, 11, 11,  3, 11, 11,  3,  3,
     4, 11, 12,  3, 12, 12,  2,  4,
};

int xPos2[] =
{
     0,  0,  0,  0,  0,  0,  0,  0,
     9, 22, 20,  5,  4, 10,  9,  0,
};

int yPos2[] =
{
     0,  0,  0,  0,  0,  0,  0,  0,
     3,  5, 19, 32, 31,  6, 16,  0,
};

int width2[] =
{
     0,  0,  0,  0,  0,  0,  0,  0,
    14,  3,  3, 15,  2,  1, 11,  0,
};

int height2[] =
{
     0,  0,  0,  0,  0,  0,  0,  0,
     2, 13,  1,  2,  1,  1,  4,  0,
};

int segWidth[] =
{
    25, 28
};

int segHeight[] =
{
    33, 36
};

wxString fileName[] =
{
    "/til313.full.png",
    "/til313.italic.full.png",
};

Tilfull::Tilfull(int type)
{
    type_ = type;
    displayedNumber_ = 0;
    x_ = 0;
    y_ = 0;

    tilBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + fileName[type_], wxBITMAP_TYPE_PNG);
    tilCopyBitmapPointer = new wxBitmap(segWidth[type_], segHeight[type_]);
    dcMemory.SelectObject(*tilBitmapPointer);
    dcCopy.SelectObject(*tilCopyBitmapPointer);
    dcCopy.Blit(0, 0, segWidth[type_], segHeight[type_], &dcMemory, 0, segHeight[type_]);
}

Tilfull::~Tilfull()
{
    delete tilBitmapPointer;
    delete tilCopyBitmapPointer;
}

void Tilfull::init(wxDC& dc, int x, int y)
{
    x_ = x;
    y_ = y;

    dc.Blit(x_, y_, segWidth[type_], segHeight[type_], &dcCopy, 0, 0);
}

void Tilfull::onPaint(wxDC& dc)
{
    dc.Blit(x_, y_, segWidth[type_], segHeight[type_], &dcCopy, 0, 0);
}
 
void Tilfull::update(wxDC& dc, Byte NewNumber, int segNumber)
{
    NewNumber = p_Computer->getTilHexFont(NewNumber, segNumber);
    if (displayedNumber_ == NewNumber)  return;

    int newN = NewNumber;
    
    for (int i=0; i<8; i++)
    {
        int tileIndex = i+(type_*8);
        if ((displayedNumber_ & 1) != (newN & 1))
        {
            if (newN & 1) // ON
            {
                dcCopy.Blit(xPos1[tileIndex], yPos1[tileIndex], width1[tileIndex], height1[tileIndex], &dcMemory, xPos1[tileIndex], yPos1[tileIndex]);
                if (width2[tileIndex] != 0)
                    dcCopy.Blit(xPos2[tileIndex], yPos2[tileIndex], width2[tileIndex], height2[tileIndex], &dcMemory, xPos2[tileIndex], yPos2[tileIndex]);
            }
            else // OFF
            {
                dcCopy.Blit(xPos1[tileIndex], yPos1[tileIndex], width1[tileIndex], height1[tileIndex], &dcMemory, xPos1[tileIndex], yPos1[tileIndex]+33);
                if (width2[tileIndex] != 0)
                    dcCopy.Blit(xPos2[tileIndex], yPos2[tileIndex], width2[tileIndex], height2[tileIndex], &dcMemory, xPos2[tileIndex], yPos2[tileIndex]+33);
            }
        }
        newN = newN >> 1;
        displayedNumber_ = displayedNumber_ >> 1;
    }
    dc.Blit(x_, y_, segWidth[type_] , segHeight[type_], &dcCopy, 0, 0);
    displayedNumber_ = NewNumber;
}


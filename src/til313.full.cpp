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
 
void Tilfull::update(wxDC& dc, Word NewNumber, int segNumber)
{
    NewNumber = p_Computer->getTilHexFont(NewNumber, segNumber);
    if (displayedNumber_ == NewNumber)  return;

    Word newN = NewNumber;
    
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

TilMan2815::TilMan2815()
{
    displayedNumber_ = 0xFFFF;
    x_ = 0;
    y_ = 0;
    tilBitmapPointer = new wxBitmap(28, 36);
    dcMemory.SelectObject(*tilBitmapPointer);
    drawSegments(0);
}

TilMan2815::~TilMan2815()
{
    delete tilBitmapPointer;
}

void TilMan2815::init(wxDC& dc, int x, int y)
{
    x_ = x;
    y_ = y;
    dc.Blit(x_, y_, 28, 36, &dcMemory, 0, 0);
}

void TilMan2815::onPaint(wxDC& dc)
{
    dc.Blit(x_, y_, 28, 36, &dcMemory, 0, 0);
}

void TilMan2815::drawSegments(Word segMask)
{
    dcMemory.SetBackground(wxBrush(wxColour(0, 0, 0)));
    dcMemory.Clear();

    wxColour onColor(255, 35, 35);
    wxColour offColor(40, 10, 10);

    auto getBrush = [&](int bit) -> wxBrush {
        return wxBrush((segMask & (1 << bit)) ? onColor : offColor);
    };
    auto getPen = [&](int bit) -> wxPen {
        return wxPen((segMask & (1 << bit)) ? onColor : offColor);
    };

    auto drawRectSeg = [&](int bit, int x, int y, int w, int h) {
        dcMemory.SetBrush(getBrush(bit));
        dcMemory.SetPen(getPen(bit));
        dcMemory.DrawRectangle(x, y, w, h);
    };

    auto drawQuadSeg = [&](int bit, wxPoint pts[4]) {
        dcMemory.SetBrush(getBrush(bit));
        dcMemory.SetPen(getPen(bit));
        dcMemory.DrawPolygon(4, pts);
    };

    // MAN2815 14-segment + DP. On this machine G1 and G2 are driven from a SINGLE
    // combined middle bit (bit 7), and the whole character is mirror-imaged (the
    // firmware's font is horizontally mirrored), so the geometry below swaps the
    // left/right verticals & diagonals so letters render the correct way round.
    // Bit 0: dp (Decimal Point) - lower left after mirror
    drawRectSeg(0, 2, 29, 3, 3);
    // Bit 1: A (Top)
    drawRectSeg(1, 7, 3, 13, 2);
    // Bit 2: B -> drawn upper-LEFT (mirrored)
    drawRectSeg(2, 5, 5, 2, 10);
    // Bit 3: C -> drawn lower-LEFT (mirrored)
    drawRectSeg(3, 5, 19, 2, 10);
    // Bit 4: D (Bottom)
    drawRectSeg(4, 7, 29, 13, 2);
    // Bit 5: E -> drawn lower-RIGHT (mirrored)
    drawRectSeg(5, 20, 19, 2, 10);
    // Bit 6: F -> drawn upper-RIGHT (mirrored)
    drawRectSeg(6, 20, 5, 2, 10);
    // Bit 7: G (Middle) - single combined bar (G1 & G2 driven together)
    drawRectSeg(7, 7, 16, 13, 2);

    // High byte 14-segment additions (mirrored left/right):
    // Bit 8: h -> upper-RIGHT diag (mirrored)
    wxPoint hPts[4] = { wxPoint(20, 5), wxPoint(18, 5), wxPoint(15, 14), wxPoint(17, 14) };
    drawQuadSeg(8, hPts);

    // Bit 9: j -> lower-RIGHT diag (mirrored)
    wxPoint jPts[4] = { wxPoint(17, 19), wxPoint(15, 19), wxPoint(18, 28), wxPoint(20, 28) };
    drawQuadSeg(9, jPts);

    // Bit 10: k (Upper Center Vert)
    drawRectSeg(10, 13, 5, 1, 10);

    // Bit 11: m (Lower Center Vert)
    drawRectSeg(11, 13, 19, 1, 10);

    // Bit 12: n -> upper-LEFT diag (mirrored)
    wxPoint nPts[4] = { wxPoint(9, 5), wxPoint(7, 5), wxPoint(10, 14), wxPoint(12, 14) };
    drawQuadSeg(12, nPts);

    // Bit 13: p -> lower-LEFT diag (mirrored)
    wxPoint pPts[4] = { wxPoint(12, 19), wxPoint(10, 19), wxPoint(7, 28), wxPoint(9, 28) };
    drawQuadSeg(13, pPts);
}

void TilMan2815::update(wxDC& dc, Word NewNumber, int segNumber)
{
    // Raw 14-segment + DP pattern (bits 0-13). Do NOT pass through getTilHexFont,
    // which truncates to a Byte and would discard the diagonal/center segments (8+).
    NewNumber &= 0x3FFF;
    if (displayedNumber_ == NewNumber)  return;

    displayedNumber_ = NewNumber;
    drawSegments(displayedNumber_);
    dc.Blit(x_, y_, 28, 36, &dcMemory, 0, 0);
}


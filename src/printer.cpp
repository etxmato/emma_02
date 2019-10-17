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

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#include "wx/file.h"
#include "wx/ffile.h"
#include "wx/dcbuffer.h"

#include "main.h"
#include "printer.h"

#define THERMALCYCLEVALUE 12

ComxPrintout::ComxPrintout(const wxString& title, Printer *pointer)
:wxPrintout(title)
{
	printerPointer = pointer;
}

bool ComxPrintout::OnPrintPage(int page)
{
	int pageWidth, pageHeight;

	pageWidth = 960;
	pageHeight = 1408;

	wxDC *dc = GetDC();
	dc->SetAxisOrientation(true, false);

	if (dc)
	{
		FitThisSizeToPageMargins(wxSize(pageWidth, pageHeight), *p_PageSetupData);
		printerPointer->completePage(*dc, page);
		return true;
	}
	else
		return false;

}

bool ComxPrintout::OnBeginDocument(int startPage, int endPage)
{
	if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;
	return true;
}

void ComxPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
	*minPage = 1;
	*maxPage = printerPointer->getNumberofPages();
	*selPageFrom = 1;
	*selPageTo = 2;
}

bool ComxPrintout::HasPage(int pageNum)
{
	return(pageNum <= printerPointer->getNumberofPages());
}

BEGIN_EVENT_TABLE(PrinterCanvas, wxScrolledWindow)
	EVT_PAINT(PrinterCanvas::onPaint)
	EVT_CLOSE(PrinterCanvas::onClose)
END_EVENT_TABLE()

PrinterCanvas::PrinterCanvas(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString& title)
:wxScrolledWindow(parent, id, pos, size, style, title)
{
	this->SetClientSize(size);

	paperWidth_ = size.GetWidth();

	SetScrollRate(10, 10);
	SetVirtualSize(paperWidth_, 1408);
	SetBackgroundColour(*wxWHITE);
	plotterNumberOfSets_ = 0;

	plotterLineSpace_ = 10;
	plotterTextMode_ = true;
	plotterX_ = 0;
	plotterY_ = 39;
	plotterOriginX_ = 0;
	plotterOriginY_ = 39;
	plotterAngle_ = 0;
	plotterColour_ = 1;
	plotterLineScale_ = 0;
	plotterSolidLine_ = true;
	plotterCharSize_ = 1;
	plotterTextSize_ = 1;
	plotterCharHeight_ = 12 + plotterLineSpace_;
	plotterCharWidth_ = 12;
	plotterGraphCharHeight_ = 12 + plotterLineSpace_;
	plotterGraphCharWidth_ = 12;
	plotterCharSet_ = 0;
	maxY_ = 1408;

	matrixX_ = 0;
	matrixY_ = 0;
	matrixCharHeight_ = 10;
	matrixCharWidth_ = 10;
	pageSize_ = 1408;

	thermalX_ = -1;
	thermalY_ = 0;
	thermalCommand_ = STANDBY;
	thermalCycleValue_ = THERMALCYCLEVALUE;
	thermalLineFeed_ = 20;

	printerBitmapPointer = new wxBitmap(paperWidth_, maxY_);
	dcPreview.SelectObject(*printerBitmapPointer);

	printFontPointer = new wxFont(12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

}

PrinterCanvas::~PrinterCanvas()
{
	dcPreview.SelectObject(wxNullBitmap);
	delete printerBitmapPointer;
	delete printFontPointer;
}

void PrinterCanvas::onClose(wxCloseEvent&WXUNUSED(event))
{
	Destroy();
}

void PrinterCanvas::onPaint(wxPaintEvent&WXUNUSED(event))
{
	wxBufferedPaintDC dc(this);

	wxSize clientSize = GetClientSize();
	int xbegin, ybegin;
	GetViewStart(&xbegin, &ybegin);
	int xunit, yunit;
	GetScrollPixelsPerUnit(&xunit, &yunit);
	xbegin*=xunit;
	ybegin*=yunit;

	dc.Blit(0, 0, clientSize.GetWidth(), clientSize.GetHeight(), &dcPreview, xbegin, ybegin);
}

void PrinterCanvas::init()
{
	dcPreview.SetPen(*wxBLACK_PEN);
	dcPreview.SetTextForeground(*wxBLACK);
	dcPreview.SetBrush(wxBrush(*wxWHITE_BRUSH));
	dcPreview.SetPen(wxPen(*wxWHITE_PEN));
	dcPreview.DrawRectangle(0, 0, paperWidth_, maxY_);

	for (int i=0; i<16; i+=2)
	{
		plotterDash_[i][0] = i+1;
		plotterDash_[i][1] = i+1;
		plotterDash_[i][2] = i+1;
		plotterDash_[i][3] = i+1;
	}
	for (int i=1; i<16; i+=2)
	{
		plotterDash_[i][0] = i+1;
		plotterDash_[i][1] = 2;
		plotterDash_[i][2] = 2;
		plotterDash_[i][3] = 2;
	}
	dcPreview.SetPen(*wxBLACK_PEN);
}

Byte PrinterCanvas::inThermal()
{
	if (thermalX_ > 0)
		return 0;
	else
		return 4;
}

void PrinterCanvas::outThermal(Byte value, Byte Qflag)
{
//	if (p_Computer->getFlipFlopQ())
	if (Qflag)
	{
		if (value & 0x8)  // Step ?
		{
//			thermalCommand_ = RIGHT;
			thermalX_++;
//			p_Main->messageInt(thermalX_);
			if (thermalX_ == 512)
				thermalLineFeed_ = 9;
		}
		if (value == 0x12)
		{
//			thermalCommand_ = LEFT;
//			p_Main->message("LEFT 9");
			thermalX_ = -1;
			thermalY_ += thermalLineFeed_;
			checkThermalY();
//			return;
		}
		if (value == 0x20) // LF / START / RIGHT ?
		{
//			p_Main->message("20");
		}
		if ((value&0x7f) == 0x21)
		{
//			thermalCommand_ = BREAK;
			if (value & 0x80) // Line 8
				thermalLine2();
		}
//		p_Main->messageInt(value);
	}
	else
	{
		if (value != 0) // Line 0-7
			thermalLine(value);
	}
}

bool PrinterCanvas::cycleThermal(bool ef4)
{
	thermalCycleValue_ --;
	if (thermalCycleValue_ > 0)  return ef4;

	thermalCycleValue_ = THERMALCYCLEVALUE;

	return !ef4;
/*
	switch(thermalCommand_)
	{
		case STANDBY:
			return;
		break;

		case RIGHT:
			thermalX_++;
		break;

		case LEFT:
			if (thermalX_ >= 0)
			{
				thermalX_--;
				if (thermalX_ == -1)
				{
					thermalY_ += 20;
					checkThermalY();
					thermalCommand_ = STANDBY;
				}
			}
		break;
	}*/
}

void PrinterCanvas::readPlotterRom(wxString DirName, wxString fileName)
{
	wxFFile inFile;
	char buffer[4096];
	wxString pathName;

	pathName = DirName + fileName;

	for (int j=0; j<256; j++)
		plotterCharacterSets_[0][j] = 0xffff;

	if (fileName.Len() != 0)
	{
		if (wxFile::Exists(pathName))
		{
			if (inFile.Open(pathName, _("rb")))
			{
				int Set = 0;
				int charNr = 32;
				plotterCharacterSets_[Set][charNr] = 0;
				charNr++;

				inFile.Read(buffer, 4096);
				for (size_t i=0; i<0xc00; i++)
				{
					plotterRom_[i] = (Byte)buffer[i];
					if ((plotterRom_[i]&0x8) == 8)
					{
						if ((Byte)buffer[i+1] != 0)
						{
							plotterCharacterSets_[Set][charNr] = (int)i+1;
							charNr++;
						}
					}
				}
				plotterCharacterSets_[Set][charNr-1] = 0xffff;
				inFile.Close();
			}
		}
	}
}

void PrinterCanvas::readPlotterRomExtension(wxString DirName, wxString fileName)
{
	wxFFile inFile;
	size_t length;
	char buffer[8192];
	wxString pathName;

	pathName = DirName + fileName;

	for (int i=1; i<8; i++)
		for (int j=0; j<256; j++)
			plotterCharacterSets_[i][j] = 0xffff;

	if (fileName.Len() != 0)
	{
		if (wxFile::Exists(pathName))
		{
			if (inFile.Open(pathName, _("rb")))
			{
				length = inFile.Read(buffer, 8192);
				for (size_t i=0; i<length; i++)
				{
					plotterRom_[i+0x1000] = (Byte)buffer[i];
				}
				inFile.Close();

				plotterNumberOfSets_ = plotterRom_[0x1000]&07;

				for (int Set=1; Set <= plotterNumberOfSets_; Set++)
				{
					int charNr = 32;
					int Start = plotterRom_[0x1001 +((Set-1)*6)] << 8;
					Start += (plotterRom_[0x1002 +((Set-1)*6)] + 0x1000);

					int address = 0x1000;

					while(address == 0x1000)
					{
						address = plotterRom_[Start] << 8;
						address += (plotterRom_[Start + 1] + 0x1000);
						Start +=2;
					}

					int stop = plotterRom_[0x1001 +((Set)*6)] << 8;
					stop += (plotterRom_[0x1002 +((Set)*6)] + 0x100C);
					if (Set == plotterNumberOfSets_)  stop = 0x3000;

					plotterCharacterSets_[Set][charNr] = address;
					charNr++;
					address++;

					while((address < stop) &&(plotterRom_[address] != 0xff))
					{
						if ((plotterRom_[address-1]&0x8) == 8)
						{
							plotterCharacterSets_[Set][charNr] = address;
							charNr++;
						}
						address++;
					}
				}
			}
		}
	}
}

void PrinterCanvas::completePage(wxDC& dc, int page)
{
	dc.Blit(0, 0, paperWidth_, pageSize_, &dcPreview, 0, pageSize_*(page-1));
}

int PrinterCanvas::getNumberofPages()
{
	return(int) ((maxY_-1)/pageSize_) + 1;
}

void PrinterCanvas::thermalLine(Byte value)
{
	for (int y=thermalY_; y<thermalY_+16; y+=2)
	{
		if (value & 1)
		{
			dcPreview.DrawRectangle(thermalX_&0xffe, y, 2, 2);
		}
		value = value >> 1;
	}

	wxSize clientSize = GetClientSize();
	int xbegin, ybegin;
	GetViewStart(&xbegin, &ybegin);
	int xunit, yunit;
	GetScrollPixelsPerUnit(&xunit, &yunit);
	xbegin*=xunit;
	ybegin*=yunit;

	wxClientDC dc(this);
	dc.Blit(0, 0, clientSize.GetWidth(), clientSize.GetHeight(), &dcPreview, xbegin, ybegin);
}

void PrinterCanvas::thermalLine2()
{
	dcPreview.DrawRectangle(thermalX_, thermalY_+16, 2, 2);

	wxSize clientSize = GetClientSize();
	int xbegin, ybegin;
	GetViewStart(&xbegin, &ybegin);
	int xunit, yunit;
	GetScrollPixelsPerUnit(&xunit, &yunit);
	xbegin*=xunit;
	ybegin*=yunit;

	wxClientDC dc(this);
	dc.Blit(0, 0, clientSize.GetWidth(), clientSize.GetHeight(), &dcPreview, xbegin, ybegin);
}

void PrinterCanvas::plotterLine(wxString printline)
{
	printLine_ = printline;
	plotterTextDrawn_ = false;

	plotterStringPosition_ = 0;
	while(plotterStringPosition_ <(int)printLine_.Len())
	{
		if (plotterTextMode_)
		{
			doText();
		}
		else
		{
			doGraphics();
		}
	}
	if (plotterTextDrawn_)
	{
		plotterY_ = plotterY_+plotterCharHeight_;
		plotterX_ = 0;
		checkXandY();
	}
	wxSize clientSize = GetClientSize();
	int xbegin, ybegin;
	GetViewStart(&xbegin, &ybegin);
	int xunit, yunit;
	GetScrollPixelsPerUnit(&xunit, &yunit);
	xbegin*=xunit;
	ybegin*=yunit;

	wxClientDC dc(this);
	dc.Blit(0, 0, clientSize.GetWidth(), clientSize.GetHeight(), &dcPreview, xbegin, ybegin);
}

void PrinterCanvas::setFont(int font)
{
	delete printFontPointer;
	switch (font)
	{
		case 0:
			printFontPointer = new wxFont(12, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		break;
		case 1:
			printFontPointer = new wxFont(12, wxFONTFAMILY_DECORATIVE  , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		break;
		case 2:
			printFontPointer = new wxFont(12, wxFONTFAMILY_ROMAN  , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		break;
		case 3:
			printFontPointer = new wxFont(12, wxFONTFAMILY_SCRIPT   , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		break;
		case 4:
			printFontPointer = new wxFont(12, wxFONTFAMILY_SWISS   , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		break;
		case 5:
			printFontPointer = new wxFont(12, wxFONTFAMILY_MODERN   , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		break;
		case 6:
			printFontPointer = new wxFont(12, wxFONTFAMILY_TELETYPE   , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		break;
	}
}

void PrinterCanvas::matrixLine(wxString printline)
{
	printLine_ = printline;
	char printCharacter;

//	wxFont exactFont(12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	matrixStringPosition_ = 0;
	dcPreview.SetFont(*printFontPointer);
//	dcPreview.DrawText(printLine_, 0, matrixY_);
//	dcPreview.GetTextExtent(printLine_, &matrixCharWidth_, &matrixCharHeight_);

	while(matrixStringPosition_ <(int)printLine_.Len())
	{
		printCharacter = printLine_.GetChar(matrixStringPosition_);
		dcPreview.GetTextExtent(printCharacter, &matrixCharWidth_, &matrixCharHeight_);
		checkX();
		dcPreview.DrawText(printCharacter, matrixX_, matrixY_);
		matrixStringPosition_++;
		matrixX_ += matrixCharWidth_;
	}
	matrixX_ = 0;
	checkY();
	wxSize clientSize = GetClientSize();
	int xbegin, ybegin;
	GetViewStart(&xbegin, &ybegin);
	int xunit, yunit;
	GetScrollPixelsPerUnit(&xunit, &yunit);
	xbegin*=xunit;
	ybegin*=yunit;

	wxClientDC dc(this);
	dc.Blit(0, 0, clientSize.GetWidth(), clientSize.GetHeight(), &dcPreview, xbegin, ybegin);
}

void PrinterCanvas::doGraphics()
{
	wxPen pen;

	int destinationX;
	int destinationY;

 	if (plotterSolidLine_)
		pen.SetStyle(wxSOLID);
	else
	{
		pen.SetStyle(wxUSER_DASH);
		pen.SetDashes(4, plotterDash_[plotterLineScale_]);
	}

	switch(plotterColour_)
	{
		case 1:
			pen.SetColour(*wxBLACK);
			dcPreview.SetTextForeground(*wxBLACK);
		break;

		case 2:
			pen.SetColour(*wxRED);
			dcPreview.SetTextForeground(*wxRED);
		break;

		case 3:
			pen.SetColour(*wxGREEN);
			dcPreview.SetTextForeground(*wxGREEN);
		break;

		case 4:
			pen.SetColour(*wxBLUE);
			dcPreview.SetTextForeground(*wxBLUE);
		break;
            
        default:
            pen.SetColour(*wxBLACK);
            dcPreview.SetTextForeground(*wxBLACK);
        break;
	}
	dcPreview.SetPen(pen);

#if wxCHECK_VERSION(2, 9, 0)
	switch(printLine_[plotterStringPosition_].GetValue())
#else
	switch(printLine_[plotterStringPosition_])
#endif
	{
		case 'B':
			plotterStringPosition_++;
			plotterLineScale_ = readInt();
		break;

		case 'H':
			plotterStringPosition_++;
			plotterX_ = plotterOriginX_;
			plotterY_ = plotterOriginY_;
		break;

		case 'D':
			plotterStringPosition_++;
			destinationX = plotterOriginX_ + readInt();
			plotterStringPosition_++;
			destinationY = plotterOriginY_ - readInt();
			dcPreview.DrawLine(plotterX_, plotterY_, destinationX, destinationY);
			plotterX_ = destinationX;
			plotterY_ = destinationY;
			checkXandY();
		break;

		case 'I':
			plotterStringPosition_++;
			destinationX = readInt();
			plotterStringPosition_++;
			destinationY = - readInt();
			dcPreview.DrawLine(plotterX_, plotterY_, plotterX_+destinationX, plotterY_+destinationY);
			plotterX_ = plotterX_+destinationX;
			plotterY_ = plotterY_+destinationY;
			checkXandY();
		break;

		case 'J':
			plotterStringPosition_++;
			plotterColour_ = readInt();
		break;

		case 'L':
			plotterStringPosition_++;
			if (readInt() == 0)
				plotterSolidLine_ = true;
			else
				plotterSolidLine_ = false;
		break;

		case 'M':
			plotterStringPosition_++;
			plotterX_ = plotterOriginX_ + readInt();
			plotterStringPosition_++;
			plotterY_ = plotterOriginY_ - readInt();
			checkXandY();
		break;

		case 'O':
			plotterStringPosition_++;
			plotterOriginX_ = plotterX_;
			plotterOriginY_ = plotterY_;
		break;

		case 'P':
			pen.SetStyle(wxSOLID);
			dcPreview.SetPen(pen);
			plotterStringPosition_++;
			while(plotterStringPosition_ <(int)printLine_.Len())
			{
				printCharacter(printLine_.GetChar(plotterStringPosition_));
				plotterStringPosition_++;
			}
		break;

		case 'Q':
			plotterStringPosition_++;
			plotterAngle_ = readInt();
		break;

		case 'R':
			plotterStringPosition_++;
			plotterX_ += readInt();
			plotterStringPosition_++;
			plotterY_ -= readInt();
			checkXandY();
		break;

		case 'S':
			plotterStringPosition_++;
			plotterCharSize_ = readInt() + 1;
			if (plotterCharSize_ <= 0)  plotterCharSize_ = 1;
			plotterGraphCharWidth_ = 12 * plotterCharSize_;
			plotterGraphCharHeight_ = (12 * plotterCharSize_) + plotterLineSpace_;
		break;

		case 'T':
			plotterTextMode_ = true;
			plotterStringPosition_++;
			plotterX_ = 0;
		break;

		case 'X':
			plotterStringPosition_++;
			plotterStringPosition_ = (int)printLine_.Len();
		break;

		default:
			plotterStringPosition_++;
		break;

	}
}

void PrinterCanvas::doText()
{
	wxColourDatabase penColour;

	switch(plotterColour_)
	{
		case 1:
			dcPreview.SetPen(*wxBLACK_PEN);
			dcPreview.SetTextForeground(*wxBLACK);
		break;

		case 2:
			dcPreview.SetPen(*wxRED_PEN);
			dcPreview.SetTextForeground(*wxRED);
		break;

		case 3:
			dcPreview.SetPen(*wxGREEN_PEN);
			dcPreview.SetTextForeground(*wxGREEN);
		break;

		case 4:
			dcPreview.SetPen(wxPen(penColour.Find("BLUE")));
			dcPreview.SetTextForeground(*wxBLUE);
		break;
	}
#if wxCHECK_VERSION(2, 9, 0)
	switch(printLine_[plotterStringPosition_].GetValue())
#else
	switch(printLine_[plotterStringPosition_])
#endif
	{
		case 8:
			plotterStringPosition_++;
			plotterX_ = plotterX_-plotterCharWidth_;
		break;

		case 10:
			plotterStringPosition_++;
			plotterY_ = plotterY_+plotterCharHeight_;
			checkXandY();
		break;

		case 11:
			plotterStringPosition_++;
			plotterY_ = plotterY_-plotterCharHeight_;
		break;

		case 12:
			plotterStringPosition_++;
			plotterCharSet_ = readInt();
			if (plotterCharSet_ > plotterNumberOfSets_)  plotterCharSet_ = 0;
		break;

		case 13:
			plotterStringPosition_++;
			plotterY_ = plotterY_+plotterCharHeight_;
			plotterX_ = 0;
			checkXandY();
		break;

		case 14:
			plotterStringPosition_++;
			plotterTextSize_ = 2;
			plotterY_ = plotterY_+plotterCharHeight_;
			plotterX_ = 0;
			checkXandY();
			plotterCharHeight_ = 24+plotterLineSpace_;
			plotterCharWidth_ = 24;
		break;

		case 15:
			plotterStringPosition_++;
			plotterColour_++;
			if (plotterColour_ == 5)  plotterColour_ = 1;
		break;

		case 18:
			plotterTextMode_ = false;
			plotterStringPosition_++;
		break;

		case 20:
			plotterStringPosition_++;
			plotterTextSize_ = 1;
			plotterCharHeight_ = 12+plotterLineSpace_;
			plotterCharWidth_ = 12;
		break;

		case 27:
			plotterStringPosition_++;
			plotterLineSpace_ = readInt() + 5;
			plotterCharHeight_ = (12*plotterTextSize_)+plotterLineSpace_;
			plotterStringPosition_ = (int)printLine_.Len();
		break;

		default:
			printCharacter(printLine_.GetChar(plotterStringPosition_));
			if (plotterX_ >= paperWidth_)
			{
				plotterX_ = 0;
				plotterY_ = plotterY_+plotterCharHeight_;
			}
			checkXandY();
			plotterStringPosition_++;
			plotterTextDrawn_ = true;
		break;
	}
}

void PrinterCanvas::printCharacter(Byte CharNumber)
{
	Byte directionConvertor [8] = {0, 4, 6, 2, 7, 1, 5, 3};
	int steps;
	Byte direction;
	int drawMode;

	int address = plotterCharacterSets_[plotterCharSet_][CharNumber];

	if (address == 0xffff)  return;

	int saveY = plotterY_;
	int saveX = plotterX_;
	while((plotterRom_[address] & 0x8) == 0)
	{
		direction = directionConvertor[(plotterRom_[address] & 0x70) >> 4];
		if (plotterTextMode_)
			steps = (plotterRom_[address] & 0x7)*plotterTextSize_;
		else
		{
			steps = (plotterRom_[address] & 0x7)*plotterCharSize_;
			direction = (direction + plotterAngle_*2) & 0x7;
		}
		drawMode = plotterRom_[address] & 0x80;
		switch(direction)
		{
			case 0:
				drawCharLine(plotterX_, plotterY_, plotterX_+steps, plotterY_, drawMode);
			break;
			case 4:
				drawCharLine(plotterX_, plotterY_, plotterX_-steps, plotterY_, drawMode);
			break;
			case 6:
				drawCharLine(plotterX_, plotterY_, plotterX_, plotterY_-steps, drawMode);
			break;
			case 2:
				drawCharLine(plotterX_, plotterY_, plotterX_, plotterY_+steps, drawMode);
			break;
			case 7:
				drawCharLine(plotterX_, plotterY_, plotterX_+steps, plotterY_-steps, drawMode);
			break;
			case 1:
				drawCharLine(plotterX_, plotterY_, plotterX_+steps, plotterY_+steps, drawMode);
			break;
			case 5:
				drawCharLine(plotterX_, plotterY_, plotterX_-steps, plotterY_-steps, drawMode);
			break;
			case 3:
				drawCharLine(plotterX_, plotterY_, plotterX_-steps, plotterY_+steps, drawMode);
			break;
		}
		address++;
	}
	if (plotterTextMode_)
	{
			plotterY_ = saveY;
			plotterX_ = saveX + plotterCharWidth_;
	}
	else
	{
		switch(plotterAngle_)
		{
			case 0:
				plotterY_ = saveY;
				plotterX_ = saveX + plotterGraphCharWidth_;
			break;

			case 1:
				plotterY_ = saveY + plotterGraphCharWidth_;
				plotterX_ = saveX;
			break;

			case 2:
				plotterY_ = saveY;
				plotterX_ = saveX - plotterGraphCharWidth_;
			break;

			case 3:
				plotterY_ = saveY - plotterGraphCharWidth_;
				plotterX_ = saveX;
			break;
		}
	}
}

void PrinterCanvas::drawCharLine(int x, int y, int xDest, int yDest, int drawmode)
{
	if (drawmode == 0)
		dcPreview.DrawLine(x, y, xDest, yDest);
	plotterX_ = xDest;
	plotterY_ = yDest;
}

int PrinterCanvas::readInt()
{
	size_t digits;
	long value;
	wxString stringValue;

	digits = 0;
	while(((plotterStringPosition_+digits) < printLine_.Len()) &&
		   (((printLine_[plotterStringPosition_+digits] >= 0x30) && (printLine_[plotterStringPosition_+digits] <= 0x39))  ||
		    (printLine_[plotterStringPosition_+digits] == 0x2d) ) )
	{
		digits++;
	}
	stringValue = printLine_.Mid(plotterStringPosition_, digits);
	if (!stringValue.ToLong(&value))
		value = 0;
	plotterStringPosition_ += (int)digits;
	return (int)value;
}

void PrinterCanvas::checkXandY()
{
	if (plotterX_ >= paperWidth_)  plotterX_ = paperWidth_-1;
	if (plotterX_ < 0)  plotterX_ = 0;
	if (plotterY_ >= (maxY_ - 200))
	{
		PrinterImage = printerBitmapPointer->ConvertToImage();
		PrinterImage = PrinterImage.Resize(wxSize(paperWidth_, maxY_ + pageSize_), wxPoint(0,0), 255, 255, 255);

		dcPreview.SelectObject(wxNullBitmap);
		delete printerBitmapPointer;
		printerBitmapPointer = new wxBitmap(PrinterImage);
		dcPreview.SelectObject(*printerBitmapPointer);

		maxY_ += pageSize_;
		SetVirtualSize(paperWidth_, maxY_);
	}
	if (plotterY_ < 0)  plotterY_ = 0;
}

void PrinterCanvas::changeSize(int x)
{
	paperWidth_ = x;
	PrinterImage = printerBitmapPointer->ConvertToImage();
	PrinterImage.SetMaskColour(255, 255, 255);
	PrinterImage = PrinterImage.Resize(wxSize(paperWidth_, maxY_), wxPoint(0,0), 255, 255, 255);

	dcPreview.SelectObject(wxNullBitmap);
	delete printerBitmapPointer;
	printerBitmapPointer = new wxBitmap(PrinterImage);
	dcPreview.SelectObject(*printerBitmapPointer);

	SetVirtualSize(paperWidth_, maxY_);
}

void PrinterCanvas::checkX()
{
	if ((matrixX_ + matrixCharWidth_) > paperWidth_)
	{
		matrixX_ = 0;
		checkY();
	}
}

void PrinterCanvas::checkY()
{
	if ((matrixY_ +(matrixCharHeight_*2)) >= maxY_)
	{
		if (maxY_ == 1408)  pageSize_ = matrixY_;

		PrinterImage = printerBitmapPointer->ConvertToImage();
		PrinterImage = PrinterImage.Resize(wxSize(paperWidth_, maxY_ + pageSize_), wxPoint(0,0), 255, 255, 255);

		dcPreview.SelectObject(wxNullBitmap);
		delete printerBitmapPointer;
		printerBitmapPointer = new wxBitmap(PrinterImage);
		dcPreview.SelectObject(*printerBitmapPointer);

		maxY_ += pageSize_;
		SetVirtualSize(paperWidth_, maxY_);
	}
	matrixY_ = matrixY_+matrixCharHeight_;
}

void PrinterCanvas::checkThermalY()
{
	if ((thermalY_ + 20) >= maxY_)
	{
		if (maxY_ == 1408)  pageSize_ = thermalY_;

		PrinterImage = printerBitmapPointer->ConvertToImage();
		PrinterImage = PrinterImage.Resize(wxSize(paperWidth_, maxY_ + pageSize_), wxPoint(0,0), 255, 255, 255);

		dcPreview.SelectObject(wxNullBitmap);
		delete printerBitmapPointer;
		printerBitmapPointer = new wxBitmap(PrinterImage);
		dcPreview.SelectObject(*printerBitmapPointer);

		maxY_ += pageSize_;
		SetVirtualSize(paperWidth_, maxY_);
	}
}

void PrinterCanvas::printFinished()
{
	thermalLineFeed_ = 20;
}

BEGIN_EVENT_TABLE(PrinterFrame, wxFrame)
	EVT_CLOSE(PrinterFrame::onClose)
	EVT_TOOL(wxID_PRINT, PrinterFrame::onPrint)
	EVT_TOOL(wxID_PREVIEW, PrinterFrame::onPreview)
	EVT_TOOL(wxID_PRINT_SETUP, PrinterFrame::onPageSetup)
	EVT_BUTTON(PRINTER_PLOTTER, PrinterFrame::onPrinterPlotter)
	EVT_BUTTON(PLOTTERROM, PrinterFrame::onRomButton)
	EVT_BUTTON(PLOTTEREXT, PrinterFrame::onExtButton)
	EVT_TEXT(PLOTTERROMTEXT, PrinterFrame::onPlotterRomText)
	EVT_COMBOBOX(PLOTTERROMTEXT, PrinterFrame::onPlotterRomText)
	EVT_TEXT(PLOTTEREXTTEXT, PrinterFrame::onPlotterRomExtensionText)
	EVT_COMBOBOX( PLOTTEREXTTEXT, PrinterFrame::onPlotterRomExtensionText)
	EVT_CHOICE( PRINTERFONT, PrinterFrame::onFont)
	EVT_CHOICE( PRINTERROWS, PrinterFrame::onRows)
END_EVENT_TABLE()

PrinterFrame::PrinterFrame(const wxString& title, const wxPoint& pos, const wxSize& size, int printerType)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	WindowInfo windowInfo = getWinSizeInfo();

	plotterRomDir_ = p_Main->getPL80Data(0);
	plotterRomFile_ = p_Main->getPL80Data(1);
	plotterRomExtensionFile_ = p_Main->getPL80Data(2);

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	switch (printerType)
	{
		case COMXPRINTER:
		case COMXRS232:
			if (p_Main->getComxPrintMode() == COMXPRINTPRINTER) 
				paperWidth_ = p_Main->getConfigItem("PrinterPaperWidth", 960l);
			else
				paperWidth_ = 960;
		break;
	
		case COMXTHPRINTER:
			paperWidth_ = 480;
		break;
	
		default:
			paperWidth_ = p_Main->getConfigItem("PrinterPaperWidth", 960l);
		break;
	}

	this->SetClientSize(wxSize(paperWidth_ + windowInfo.xPrint, size.GetHeight()));

	printerCanvasPointer = new PrinterCanvas( this, wxID_ANY, pos, size, wxSUNKEN_BORDER, title);

	printerCanvasPointer->readPlotterRom(plotterRomDir_, plotterRomFile_);
	printerCanvasPointer->readPlotterRomExtension(plotterRomDir_, plotterRomExtensionFile_);
	printerType_ = printerType;
}

PrinterFrame::~PrinterFrame()
{
	printerCanvasPointer->Destroy();
}

void PrinterFrame::onClose(wxCloseEvent&WXUNUSED(event))
{
	switch(printerType_)
	{
		case TELMACPRINTER:
        case PECOMPRINTER:
        case MS2000PRINTER:
        case VIPPRINTER:
		case ELFPRINTER:
			p_Printer->closePreviewWindow();
		break;

		case COMXPRINTER:
			p_Main->setPL80Data(0, plotterRomDir_);
			p_Main->setPL80Data(1, plotterRomFile_);
			p_Main->setPL80Data(2, plotterRomExtensionFile_);
			p_PrinterParallel->closePreviewWindow();
		break;

		case COMXTHPRINTER:
			p_PrinterThermal->closePreviewWindow();
		break;

		case COMXRS232:
			p_Main->setPL80Data(0, plotterRomDir_);
			p_Main->setPL80Data(1, plotterRomFile_);
			p_Main->setPL80Data(2, plotterRomExtensionFile_);
			p_PrinterSerial->closePreviewWindow();
		break;
	}

	Destroy();
}

void PrinterFrame::init()
{
	printerCanvasPointer->init();
}

Byte PrinterFrame::inThermal()
{
	return printerCanvasPointer->inThermal();
}

void PrinterFrame::outThermal(Byte value, Byte Qflag)
{
	printerCanvasPointer->outThermal(value, Qflag);
}

bool PrinterFrame::cycleThermal(bool ef4)
{
	return printerCanvasPointer->cycleThermal(ef4);
}

void PrinterFrame::onFont(wxCommandEvent &event)
{
	printerCanvasPointer->setFont(event.GetSelection());
}

void PrinterFrame::onRows(wxCommandEvent &event)
{
	WindowInfo windowInfo = getWinSizeInfo();
	switch (event.GetSelection())
	{
		case 0:
			paperWidth_ = 400;
		break;

		case 1:
			paperWidth_ = 800;
		break;

		case 2:
			paperWidth_ = 960;
		break;

		case 3:
			paperWidth_ = 1200;
		break;
	}
	p_Main->setConfigItem("PrinterPaperWidth", paperWidth_);
	printerCanvasPointer->changeSize(paperWidth_);
	this->SetClientSize(wxSize(paperWidth_ + windowInfo.xPrint, 1408/4*3));
}

void PrinterFrame::onPrinterPlotter(wxCommandEvent&WXUNUSED(event))
{
	WindowInfo windowInfo = getWinSizeInfo();
	if (p_Main->getComxPrintMode() != COMXPRINTPRINTER)
	{
		p_Main->setComxPrintMode(COMXPRINTPRINTER);
		SetTitle("COMX Printer Output");
		enablePlotterGui(false);
		paperWidth_ = p_Main->getConfigItem("PrinterPaperWidth", 960l);
	}
	else
	{
		p_Main->setComxPrintMode(COMXPRINTPLOTTER);
		SetTitle("COMX Plotter Output");
		enablePlotterGui(true);
		paperWidth_ = 960;
	}
	printerCanvasPointer->changeSize(paperWidth_);
	this->SetClientSize(wxSize(paperWidth_ + windowInfo.xPrint, 1408/4*3));
}

void PrinterFrame::onPlotterRomExtensionText(wxCommandEvent &WXUNUSED(event) )
{
    wxString newFileName = getPlotterRomExtensionFile();
	if (newFileName != "")
    {
        plotterRomExtensionFile_ = newFileName;
		printerCanvasPointer->readPlotterRomExtension(plotterRomDir_, plotterRomExtensionFile_);
    }
}

void PrinterFrame::onPlotterRomText(wxCommandEvent &WXUNUSED(event) )
{
    wxString newFileName = getPlotterRomFile();
    if (newFileName != "")
    {
        plotterRomFile_ = newFileName;
		printerCanvasPointer->readPlotterRom(plotterRomDir_, plotterRomFile_);
    }
}

void PrinterFrame::onRomButton(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the PL80 ROM file to load",
                               plotterRomDir_, plotterRomFile_,
                               "",
                               wxString::Format
                              (
                                   "Binary File|*.bin;*.rom;|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);

	plotterRomFile_ = FullPath.GetFullName();
	plotterRomDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	showPlotterRomFile(plotterRomFile_);
	printerCanvasPointer->readPlotterRom(plotterRomDir_, plotterRomFile_);
}

void PrinterFrame::onExtButton(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the PL80 Character ROM file to load",
                               plotterRomDir_, plotterRomExtensionFile_,
                               "",
                               wxString::Format
                              (
                                   "Binary File|*.bin;*.rom;|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);

	plotterRomExtensionFile_ = FullPath.GetFullName();
	plotterRomDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	showPlotterRomExtensionFile(plotterRomExtensionFile_);
	printerCanvasPointer->readPlotterRomExtension(plotterRomDir_, plotterRomExtensionFile_);
}

void PrinterFrame::onPrint(wxCommandEvent&WXUNUSED(event))
{
	switch(printerType_)
	{
		case TELMACPRINTER:
		case PECOMPRINTER:
        case MS2000PRINTER:
		case VIPPRINTER:
		case ELFPRINTER:
			p_Printer->printOut();
		break;

		case COMXPRINTER:
			p_PrinterParallel->printOut();
		break;

		case COMXTHPRINTER:
			p_PrinterThermal->printOut();
		break;

		case COMXRS232:
			p_PrinterSerial->printOut();
		break;
	}
}

void PrinterFrame::onPreview(wxCommandEvent&WXUNUSED(event))
{
	switch(printerType_)
	{
		case TELMACPRINTER:
		case PECOMPRINTER:
        case MS2000PRINTER:
		case VIPPRINTER:
		case ELFPRINTER:
			p_Printer->printPreview();
		break;

		case COMXPRINTER:
			p_PrinterParallel->printPreview();
		break;

		case COMXTHPRINTER:
			p_PrinterThermal->printPreview();
		break;

		case COMXRS232:
			p_PrinterSerial->printPreview();
		break;
	}
}

void PrinterFrame::onPageSetup(wxCommandEvent&WXUNUSED(event))
{
	p_Main->pageSetup();
}

void PrinterFrame::matrixLine(wxString printline)
{
	printerCanvasPointer->matrixLine(printline);
}

void PrinterFrame::plotterLine(wxString printline)
{
	printerCanvasPointer->plotterLine(printline);
}

void PrinterFrame::printFinished()
{
	printerCanvasPointer->printFinished();
}

void PrinterFrame::completePage(wxDC& dc, int page)
{
	printerCanvasPointer->completePage(dc, page);
}

int PrinterFrame::getNumberofPages()
{
	return printerCanvasPointer->getNumberofPages();
}

void PrinterFrame::enablePlotterGui(bool status)
{
	switch(printerType_)
	{
		case TELMACPRINTER:
		case PECOMPRINTER:
        case MS2000PRINTER:
		case VIPPRINTER:
		case ELFPRINTER:
			p_Printer->enablePlotterGui(status);
		break;

		case COMXPRINTER:
			p_PrinterParallel->enablePlotterGui(status);
		break;

		case COMXTHPRINTER:
			p_PrinterThermal->enablePlotterGui(status);
		break;

		case COMXRS232:
			p_PrinterSerial->enablePlotterGui(status);
		break;
	}
}

void PrinterFrame::showPlotterRomFile(wxString fileName)
{
	switch(printerType_)
	{
		case TELMACPRINTER:
		case PECOMPRINTER:
        case MS2000PRINTER:
		case VIPPRINTER:
		case ELFPRINTER:
			p_Printer->showPlotterRomFile(fileName);
		break;

		case COMXPRINTER:
			p_PrinterParallel->showPlotterRomFile(fileName);
		break;

		case COMXTHPRINTER:
			p_PrinterThermal->showPlotterRomFile(fileName);
		break;

		case COMXRS232:
			p_PrinterSerial->showPlotterRomFile(fileName);
		break;
	}
}

void PrinterFrame::showPlotterRomExtensionFile(wxString fileName)
{
	switch(printerType_)
	{
		case TELMACPRINTER:
		case PECOMPRINTER:
        case MS2000PRINTER:
		case VIPPRINTER:
		case ELFPRINTER:
			p_Printer->showPlotterRomExtensionFile(fileName);
		break;

		case COMXPRINTER:
			p_PrinterParallel->showPlotterRomExtensionFile(fileName);
		break;

		case COMXTHPRINTER:
			p_PrinterThermal->showPlotterRomExtensionFile(fileName);
		break;

		case COMXRS232:
			p_PrinterSerial->showPlotterRomExtensionFile(fileName);
		break;
	}
}

wxString PrinterFrame::getPlotterRomFile()
{
	switch(printerType_)
	{
		case TELMACPRINTER:
		case PECOMPRINTER:
        case MS2000PRINTER:
		case VIPPRINTER:
		case ELFPRINTER:
			return p_Printer->getPlotterRomFile();
		break;

		case COMXPRINTER:
			return p_PrinterParallel->getPlotterRomFile();
		break;

		case COMXTHPRINTER:
			return p_PrinterThermal->getPlotterRomFile();
		break;

		case COMXRS232:
			return p_PrinterSerial->getPlotterRomFile();
		break;
	}
	return "";
}

wxString PrinterFrame::getPlotterRomExtensionFile()
{
	switch(printerType_)
	{
		case TELMACPRINTER:
		case PECOMPRINTER:
        case MS2000PRINTER:
		case VIPPRINTER:
		case ELFPRINTER:
			return p_Printer->getPlotterRomExtensionFile();
		break;

		case COMXPRINTER:
			return p_PrinterParallel->getPlotterRomExtensionFile();
		break;

		case COMXTHPRINTER:
			return p_PrinterThermal->getPlotterRomExtensionFile();
		break;

		case COMXRS232:
			return p_PrinterSerial->getPlotterRomExtensionFile();
		break;
	}
	return "";
}

Printer::Printer()
{
	parallelFrameOpen_ = false;
	serialFrameOpen_ = false;
	thermalFrameOpen_ = false;
	printerFrameOpen_ = false;
	printFileOpen_ = false;
}

Printer::~Printer()
{
	if (printFileOpen_)
		printFile_.Close();
}

void Printer::configureElfPrinter(ElfPortConfiguration portConf)
{
//	int output;

	wxString runningComp = p_Main->getRunningComputerStr();

//	output = p_Main->getConfigItem(runningComp +"/PrinterOutput", 7l);
	p_Computer->setOutType(portConf.printerOutput, PRINTEROUT);

	wxString printBuffer;
	p_Main->message("Configuring Printer");

	if (portConf.printerEf != 0)
	{
		p_Computer->setEfType(portConf.printerEf, ELFPRINTEREF); 
		printBuffer.Printf("	Output %d: write data, EF %d: data ready\n", portConf.printerOutput, portConf.printerEf);
	}
	else
		printBuffer.Printf("	Output %d: write data\n", portConf.printerOutput);
	p_Main->message(printBuffer);
}

void Printer::configureThermalPrinter()
{
	p_Computer->setCycleType(PRINTCYCLE, THERMALCYCLE);
}

void Printer::init(Printer *pointer, wxString computerName, int printerType)
{
	printerPointer = pointer;
	computerName_ = computerName;
	printerType_ = printerType;

	printMode_ = p_Main->getPrintMode();
	printerFrameOpen_ = false;
	setPrintfileName(p_Main->getPrintFile());
	line_ = "";
}

void Printer::initComx(Printer *pointer)
{
	printerPointer = pointer;
	computerName_ = "COMX-35";
	comxPrintMode_ = p_Main->getComxPrintMode();

	printEscape_ = false;
	setPrintfileName(p_Main->getPrintFile());
	printStarted_ = false;
	thermalEf4_ = false;
	parallelFrameOpen_ = false;
	serialFrameOpen_ = false;
	thermalFrameOpen_ = false;
	line_ = "";
}

void Printer::initTelmac(Printer *pointer)
{
	printerPointer = pointer;
	computerName_ = "Telmac";
	printerType_ = TELMACPRINTER;

	printMode_ = p_Main->getPrintMode();
	printerFrameOpen_ = false;
	setPrintfileName(p_Main->getPrintFile());
}

void Printer::initPecom(Printer *pointer)
{
    printerPointer = pointer;
    computerName_ = "Pecom";
    printerType_ = PECOMPRINTER;
    
    printMode_ = p_Main->getPrintMode();
    printerFrameOpen_ = false;
    setPrintfileName(p_Main->getPrintFile());
    line_ = "";
}

void Printer::initMS2000(Printer *pointer)
{
    printerPointer = pointer;
    computerName_ = "MS2000";
    printerType_ = MS2000PRINTER;
    
    printMode_ = p_Main->getPrintMode();
    printerFrameOpen_ = false;
    setPrintfileName(p_Main->getPrintFile());
    line_ = "";
}

void Printer::initVip(Printer *pointer)
{
	printerPointer = pointer;
	computerName_ = "Vip";
	printerType_ = VIPPRINTER;

	printMode_ = p_Main->getPrintMode();
	printerFrameOpen_ = false;
	setPrintfileName(p_Main->getPrintFile());
}

void Printer::initElf(Printer *pointer, wxString computerName)
{
	printerPointer = pointer;
	computerName_ = computerName;
	printerType_ = ELFPRINTER;

	printMode_ = p_Main->getPrintMode();
	printerFrameOpen_ = false;
	setPrintfileName(p_Main->getPrintFile());
}

Byte Printer::inParallel()
{
	if (comxPrintMode_ == PRINTFILE)
		return 0x6;
	else
	{
		if ((parallelFrameOpen_)  || printStarted_)
			return 0x6;
		else
		{
			wxCommandEvent event(OPEN_COMX_PRINTER_WINDOW, 800);
			event.SetEventObject(this);
			wxPostEvent(p_Main, event);
			while(!parallelFrameOpen_)
				p_Computer->sleepComputer(1000);
			return 0x6;
		}
	}
}

Byte Printer::inSerial()
{
	printValue_ = 0;
	bit_ = 0;
	dataBits_ = p_Computer->getRam(0x41b2) & 0xf;
	stopBit_ = dataBits_ + 1;
	if (p_Computer->getRam(0x41b2) & 0x30)
		stopBit_++;

	if (comxPrintMode_ != PRINTFILE && !printStarted_)
	{
		if (!serialFrameOpen_)
		{
			wxCommandEvent event(OPEN_COMX_PRINTER_WINDOW, 800);
			event.SetEventObject(this);
			wxPostEvent(p_Main, event);
			while(!serialFrameOpen_)
				p_Computer->sleepComputer(1000);
		}
	}
	return 0;
}

Byte Printer::inThermal()
{
	if (thermalFrameOpen_)
		return printerFramePointer->inThermal();
	else
	{
		wxCommandEvent event(OPEN_COMX_PRINTER_WINDOW, 800);
		event.SetEventObject(this);
		wxPostEvent(p_Main, event);
		while(!thermalFrameOpen_)
			p_Computer->sleepComputer(1000);
		return printerFramePointer->inThermal();
	}
}

void Printer::openPrintFile()
{
	if (!wxFile::Exists(printFileFullPath_))
	{
		printFile_.Create(printFileFullPath_);
		newPrintFile_ = false;
		printFileOpen_ = false;
	}
	if (newPrintFile_)
	{
		newPrintFile_ = false;
		wxFileName FullPath = wxFileName(printFileFullPath_, wxPATH_NATIVE);
		wxString Name = FullPath.GetName();
		wxString pathName = FullPath.GetPath();
		wxString Ext = FullPath.GetExt();
		int num = 0;
		wxString Number;

		while(wxFile::Exists(printFileFullPath_))
		{
			num++;
			Number.Printf("%d", num);
			printFileFullPath_ = pathName + p_Main->getPathSep() + Name + "." + Number + "." + Ext;
		}
		if (printFileOpen_)
			printFile_.Close();
		printFile_.Create(printFileFullPath_);
		printFileOpen_ = false;
	}
	if (!printFile_.IsOpened())
	{
		printFile_.Open(printFileFullPath_, wxFile::write_append);
		printFileOpen_ = true;
	}
}

void Printer::printerOut(Byte value)
{
	wxString outputBuffer;

	if ((value & 0x80) !=  0)  return;
	if (printMode_ != PRINTFILE)
	{
		if (!printerFrameOpen_)
		{
			wxCommandEvent event(OPEN_PRINTER_WINDOW, 811);
			event.SetEventObject(this);
			wxPostEvent(p_Main, event);
			while(!printerFrameOpen_)
				p_Computer->sleepComputer(1000);
		}

		if (value == 0xd)
		{
			printerFramePointer->matrixLine(line_);
			line_ = "";
		}
		else
		{
			outputBuffer.Printf("%c",value);
			line_ = line_ + outputBuffer;
		}
	}
	else
	{
		if (printFileName_.Len() != 0)
		{
			openPrintFile();
			outputBuffer.Printf("%c",value);
			if (printFile_.IsOpened())
				printFile_.Write(outputBuffer);
		}
	}
    p_Computer->setPrinterEf();
}

void Printer::startPecomChar()
{
	printValue_ = 0;
	bit_ = 0;

	if (printMode_ != PRINTFILE)
	{
		if (!printerFrameOpen_)
		{
			wxCommandEvent event(OPEN_PRINTER_WINDOW, 811);
			event.SetEventObject(this);
			wxPostEvent(p_Main, event);
			while(!printerFrameOpen_)
				p_Computer->sleepComputer(1000);
		}
	}
}

void Printer::outParallel(Byte value)
{
	wxString outputBuffer;

	if (!printStarted_ &&(parallelFrameOpen_ || serialFrameOpen_))
		disablePrintPlotter();

	printStarted_ = true;
	if (printEscape_)
	{
		printEscape_ = false;
		return;
	}
	switch(value)
	{
		case '@':
			if (comxPrintMode_ != PRINTFILE)
				value = 219;
		break;

		case 0x1:
			value = 0xb6;
		break;

		case 0xa:
			return;
		break;

		case 0x1b:
			if (comxPrintMode_ != COMXPRINTPLOTTER)
			{
				printEscape_ = true;
				return;
			}
		break;

		case 0x8:
			if (comxPrintMode_ != COMXPRINTPLOTTER)
			{
				printEscape_ = true;
				return;
			}
		break;
	}

	p_Main->statusLedOnEvent();

	if (parallelFrameOpen_ || serialFrameOpen_)
	{
		if (value == 0xd)
		{
			if (comxPrintMode_ == COMXPRINTPRINTER)
				printerFramePointer->matrixLine(line_);
			else
				printerFramePointer->plotterLine(line_);
			line_ = "";
		}
		else
		{
            if (value <= 0x7f)
            {
                outputBuffer.Printf("%c",value);
                line_ = line_ + outputBuffer;
            }
		}
	}
	if (comxPrintMode_ == PRINTFILE)
	{
		if (printFileName_.Len() != 0)
		{
			openPrintFile();
            if (value <= 0x7f)
            {
                outputBuffer.Printf("%c",value);
                if (printFile_.IsOpened())
                    printFile_.Write(outputBuffer);
            }
		}
	}

	p_Main->statusLedOffEvent();
}

void Printer::outSerial(Byte value)
{
	if (bit_ == stopBit_)  outParallel(printValue_);
	if ((bit_ > 0) &&(bit_ <= dataBits_))
	{
		if (value == 0)
			printValue_ += (1 <<(bit_ -1));
	}
	bit_++;
}

void Printer::outThermal(Byte value, Byte Qflag)
{
	printStarted_ = true;

	if (thermalFrameOpen_)
		printerFramePointer->outThermal(value, Qflag);
}

void Printer::cycleThermal()
{
	if (thermalFrameOpen_)
	{
		thermalEf4_ = printerFramePointer->cycleThermal(thermalEf4_);
	}
	else
		thermalEf4_ = false;
}

void Printer::setComxPrintMode(int mode)
{
	comxPrintMode_ = mode;
}

void Printer::setPrintMode(int mode)
{
	printMode_ = mode;
}

bool Printer::getThermalEf4()
{
	return thermalEf4_;
}

void Printer::closeFrames()
{
	if (parallelFrameOpen_ || serialFrameOpen_ || thermalFrameOpen_ || printerFrameOpen_)
	{
		printerFramePointer->Destroy();
		parallelFrameOpen_ = false;
		serialFrameOpen_ = false;
		thermalFrameOpen_ = false;
		printerFrameOpen_ = false;
	}
}

void Printer::closePreviewWindow()
{
	if (parallelFrameOpen_ || serialFrameOpen_ || thermalFrameOpen_ || printerFrameOpen_)
	{
		parallelFrameOpen_ = false;
		serialFrameOpen_ = false;
		thermalFrameOpen_ = false;
		printerFrameOpen_ = false;
	}
}

void Printer::onF4Parallel()
{
	printerType_ = COMXPRINTER;

	if (!parallelFrameOpen_)
	{
		onComxF4();
	}
	parallelFrameOpen_ = true;
}

void Printer::onF4Serial()
{
	printerType_ = COMXRS232;

	if (!serialFrameOpen_)
	{
		onComxF4();
	}
	serialFrameOpen_ = true;
}

void Printer::onF4()
{
	int paperWidth;
	wxString choices [6];
	if (!printerFrameOpen_)
	{
		paperWidth = p_Main->getConfigItem("PrinterPaperWidth", 960l);
		printerFramePointer = new PrinterFrame( computerName_+" Printer Output", wxPoint(-1, -1), wxSize(paperWidth, 1408/4*3), printerType_);
		printerFramePointer->CreateToolBar(wxTB_HORZ_TEXT);

		fontTextPointer = new wxStaticText(printerFramePointer->GetToolBar(), -1, " Font:");
		rowsTextPointer = new wxStaticText(printerFramePointer->GetToolBar(), -1, " Characters:");

		choices [0] = "Default";
		choices [1] = "Decorative";
		choices [2] = "Roman";
		choices [3] = "Script";
		choices [4] = "Swiss";
		choices [5] = "Teletype";
#if defined(__WXMAC__)
		printerFontPointer = new wxChoice(printerFramePointer->GetToolBar(), PRINTERFONT, wxPoint(-1, -1), wxSize(85, -1), 6, choices, 0, wxDefaultValidator, "Font");
#else
		printerFontPointer = new wxChoice(printerFramePointer->GetToolBar(), PRINTERFONT, wxPoint(-1, -1), wxSize(75, -1), 6, choices, 0, wxDefaultValidator, "Font");
#endif
		printerFontPointer->SetToolTip("Specify Printer Font");
		printerFontPointer->SetSelection(5);

		choices [0] = "40";
		choices [1] = "80";
		choices [2] = "96";
		choices [3] = "120";
#if defined(__WXMAC__)
		printerRowsPointer = new wxChoice(printerFramePointer->GetToolBar(), PRINTERROWS, wxPoint(-1, -1), wxSize(55, -1), 4, choices, 0, wxDefaultValidator, "Rows");
#else
		printerRowsPointer = new wxChoice(printerFramePointer->GetToolBar(), PRINTERROWS, wxPoint(-1, -1), wxSize(42, -1), 4, choices, 0, wxDefaultValidator, "Rows");
#endif
		printerRowsPointer->SetToolTip("Specify number of Characters per line");
		switch (paperWidth)
		{
			case 400:
				printerRowsPointer->SetSelection(0);
			break;
			case 800:
				printerRowsPointer->SetSelection(1);
			break;
			case 960:
				printerRowsPointer->SetSelection(2);
			break;
			case 1200:
				printerRowsPointer->SetSelection(3);
			break;
		}

		printerFramePointer->GetToolBar()->AddControl(fontTextPointer);
		printerFramePointer->GetToolBar()->AddControl(printerFontPointer);
		printerFramePointer->GetToolBar()->AddControl(rowsTextPointer);
		printerFramePointer->GetToolBar()->AddControl(printerRowsPointer);
#ifndef __WXMAC__
		printerFramePointer->GetToolBar()->AddSeparator();
#endif
		printerFramePointer->GetToolBar()->AddTool(wxID_PRINT, "Print", wxBitmap(p_Main->getApplicationDir()+IMAGES_FOLDER + "/print.png", wxBITMAP_TYPE_PNG), wxNullBitmap, wxITEM_NORMAL , "Send output to Printer");
		printerFramePointer->GetToolBar()->AddTool(wxID_PREVIEW, "Preview", wxBitmap(p_Main->getApplicationDir()+IMAGES_FOLDER + "/preview.png", wxBITMAP_TYPE_PNG), wxNullBitmap,  wxITEM_NORMAL, "Send output to Preview Window");
		printerFramePointer->GetToolBar()->AddTool(wxID_PRINT_SETUP, "Setup", wxBitmap(p_Main->getApplicationDir()+IMAGES_FOLDER + "/page_setup.png", wxBITMAP_TYPE_PNG), wxNullBitmap,  wxITEM_NORMAL, "Specify Preview and Printout page details");

#if defined(__linux__)
		wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		fontTextPointer->SetFont(defaultFont);
		rowsTextPointer->SetFont(defaultFont);
		printerFontPointer->SetFont(defaultFont);
		printerRowsPointer->SetFont(defaultFont);
		printerFramePointer->GetToolBar()->SetFont(defaultFont);
#endif
#if defined(__WXMAC__)
		wxFont defaultFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		fontTextPointer->SetFont(defaultFont);
		rowsTextPointer->SetFont(defaultFont);
		printerFontPointer->SetFont(defaultFont);
		printerRowsPointer->SetFont(defaultFont);
		printerFramePointer->GetToolBar()->SetFont(defaultFont);
#endif

		printerFramePointer->GetToolBar()->Realize();

		printerFramePointer->Show(true);
		printerFramePointer->init();
		enableToolbar(true);
        printerFrameOpen_ = true;
	}
}

void Printer::onF4Thermal()
{
	printerType_ = COMXTHPRINTER;

	if (!thermalFrameOpen_)
	{
#if defined(__linux__) || defined(__WXMAC__)
		printerFramePointer = new PrinterFrame( "COMX Thermal Printer Output", wxPoint(-1, -1), wxSize(480, 1408/4*3), COMXTHPRINTER);
#else
		printerFramePointer = new PrinterFrame( "COMX Thermal Printer Output", wxPoint(-1, -1), wxSize(480, 1408/4*3), COMXTHPRINTER);
#endif
		printerFramePointer->CreateToolBar(wxTB_HORZ_TEXT);

		printerFramePointer->GetToolBar()->AddTool(wxID_PRINT, "Print", wxBitmap(p_Main->getApplicationDir()+IMAGES_FOLDER + "/print.png", wxBITMAP_TYPE_PNG), wxNullBitmap, wxITEM_NORMAL , "Send output to Printer");
		printerFramePointer->GetToolBar()->AddTool(wxID_PREVIEW, "Preview", wxBitmap(p_Main->getApplicationDir()+IMAGES_FOLDER + "/preview.png", wxBITMAP_TYPE_PNG), wxNullBitmap,  wxITEM_NORMAL, "Send output to Preview Window");
		printerFramePointer->GetToolBar()->AddTool(wxID_PRINT_SETUP, "Setup", wxBitmap(p_Main->getApplicationDir()+IMAGES_FOLDER + "/page_setup.png", wxBITMAP_TYPE_PNG), wxNullBitmap,  wxITEM_NORMAL, "Specify Preview and Printout page details");

#if defined(__linux__)
		wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		printerFramePointer->GetToolBar()->SetFont(defaultFont);
#endif
#if defined(__WXMAC__)
		wxFont defaultFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		printerFramePointer->GetToolBar()->SetFont(defaultFont);
#endif

		printerFramePointer->GetToolBar()->Realize();

		printerFramePointer->Show(true);
		printerFramePointer->init();
		enableToolbar(false);

		thermalFrameOpen_ = true;
	}
}

void Printer::onComxF4()
{
	wxString choices[6];
	int paperWidth;

	paperWidth = p_Main->getConfigItem("PrinterPaperWidth", 960l);
	if (comxPrintMode_ == COMXPRINTPRINTER)
	{
		printerFramePointer = new PrinterFrame( "COMX Printer Output", wxPoint(-1, -1), wxSize(paperWidth, 1408/4*3), printerType_);
	}
	else
	{
		printerFramePointer = new PrinterFrame( "COMX Plotter Output", wxPoint(-1, -1), wxSize(960, 1408/4*3), printerType_);
	}
	printerFramePointer->CreateToolBar(wxTB_HORZ_TEXT);
	if (comxPrintMode_ == COMXPRINTPRINTER)
#if defined(__WXMAC__)
		printerPlotterButtonPointer = new wxButton(printerFramePointer->GetToolBar(), PRINTER_PLOTTER, "->PLOTTER", wxPoint(-1, -1), wxSize(75, 25));
#else
		printerPlotterButtonPointer = new wxButton(printerFramePointer->GetToolBar(), PRINTER_PLOTTER, "->PLOTTER", wxPoint(-1, -1), wxSize(60, -1));
#endif
	else
#if defined(__WXMAC__)
		printerPlotterButtonPointer = new wxButton(printerFramePointer->GetToolBar(), PRINTER_PLOTTER, "->PRINTER", wxPoint(-1, -1), wxSize(75, 25));
#else
		printerPlotterButtonPointer = new wxButton(printerFramePointer->GetToolBar(), PRINTER_PLOTTER, "->PRINTER", wxPoint(-1, -1), wxSize(60, -1));
#endif
	printerPlotterButtonPointer->SetToolTip("Switch output PRINTER <-> PLOTTER");

	choices [0] = " ";
	choices [1] = "pl80.bin";
#if defined(__WXMAC__)
	plotterRomPointer = new wxButton(printerFramePointer->GetToolBar(), PLOTTERROM, "ROM:", wxPoint(-1, -1), wxSize(60, 25));
#else
	plotterRomPointer = new wxButton(printerFramePointer->GetToolBar(), PLOTTERROM, "ROM", wxPoint(-1, -1), wxSize(42, -1));
#endif
	plotterRomPointer->SetToolTip("Browse for Plotter ROM File");
#if defined(__WXMAC__)
	plotterRomTextPointer = new wxComboBox(printerFramePointer->GetToolBar(), PLOTTERROMTEXT, p_Main->getPL80Data(1), wxPoint(-1, -1), wxSize(110, -1), 2, choices);
#else
	plotterRomTextPointer = new wxComboBox(printerFramePointer->GetToolBar(), PLOTTERROMTEXT, p_Main->getPL80Data(1), wxPoint(-1, -1), wxSize(100, -1), 2, choices);
#endif
	plotterRomTextPointer->SetToolTip("Specify Plotter ROM File");
	plotterRomTextPointer->SetSelection(1);

	choices [0] = " ";
	choices [1] = "pl80.it.em.ou.bin";
	choices [2] = "pl80.tiny.bin";
#if defined(__WXMAC__)
	plotterExtensionRomPointer = new wxButton(printerFramePointer->GetToolBar(), PLOTTEREXT, "CHAR:", wxPoint(-1, -1), wxSize(60, 25));
#else
	plotterExtensionRomPointer = new wxButton(printerFramePointer->GetToolBar(), PLOTTEREXT, "CHAR", wxPoint(-1, -1), wxSize(42, -1));
#endif
	plotterExtensionRomPointer->SetToolTip("Browse for Plotter Character ROM File");
#if defined(__WXMAC__)
	plotterExtensionRomTextPointer = new wxComboBox(printerFramePointer->GetToolBar(), PLOTTEREXTTEXT, p_Main->getPL80Data(2), wxPoint(-1, -1), wxSize(130, -1), 3, choices);
#else
	plotterExtensionRomTextPointer = new wxComboBox(printerFramePointer->GetToolBar(), PLOTTEREXTTEXT, p_Main->getPL80Data(2), wxPoint(-1, -1), wxSize(120, -1), 3, choices);
#endif
	plotterExtensionRomTextPointer->SetToolTip("Specify Plotter Character ROM File");
	plotterExtensionRomTextPointer->SetSelection(1);

	fontTextPointer = new wxStaticText(printerFramePointer->GetToolBar(), -1, " Font:");
	rowsTextPointer = new wxStaticText(printerFramePointer->GetToolBar(), -1, " Characters:");

	choices [0] = "Default";
	choices [1] = "Decorative";
	choices [2] = "Roman";
	choices [3] = "Script";
	choices [4] = "Swiss";
	choices [5] = "Teletype";
#if defined(__WXMAC__)
	printerFontPointer = new wxChoice(printerFramePointer->GetToolBar(), PRINTERFONT, wxPoint(-1, -1), wxSize(85, -1), 6, choices, 0, wxDefaultValidator, "Font");
#else
	printerFontPointer = new wxChoice(printerFramePointer->GetToolBar(), PRINTERFONT, wxPoint(-1, -1), wxSize(75, -1), 6, choices, 0, wxDefaultValidator, "Font");
#endif
	printerFontPointer->SetToolTip("Specify Printer Font");
	printerFontPointer->SetSelection(5);

	choices [0] = "40";
	choices [1] = "80";
	choices [2] = "96";
	choices [3] = "120";
#if defined(__WXMAC__)
	printerRowsPointer = new wxChoice(printerFramePointer->GetToolBar(), PRINTERROWS, wxPoint(-1, -1), wxSize(55, -1), 4, choices, 0, wxDefaultValidator, "Rows");
#else
	printerRowsPointer = new wxChoice(printerFramePointer->GetToolBar(), PRINTERROWS, wxPoint(-1, -1), wxSize(42, -1), 4, choices, 0, wxDefaultValidator, "Rows");
#endif
	printerRowsPointer->SetToolTip("Specify number of Characters per line");
	switch (paperWidth)
	{
		case 400:
			printerRowsPointer->SetSelection(0);
		break;
		case 800:
			printerRowsPointer->SetSelection(1);
		break;
		case 960:
			printerRowsPointer->SetSelection(2);
		break;
		case 1200:
			printerRowsPointer->SetSelection(3);
		break;
	}

	printerFramePointer->GetToolBar()->AddControl(fontTextPointer);
	printerFramePointer->GetToolBar()->AddControl(printerFontPointer);
	printerFramePointer->GetToolBar()->AddControl(rowsTextPointer);
	printerFramePointer->GetToolBar()->AddControl(printerRowsPointer);
#ifndef __WXMAC__
	printerFramePointer->GetToolBar()->AddSeparator();
#endif
	printerFramePointer->GetToolBar()->AddTool(wxID_PRINT, "Print", wxBitmap(p_Main->getApplicationDir()+IMAGES_FOLDER + "/print.png", wxBITMAP_TYPE_PNG), "Send output to Printer", wxITEM_NORMAL);
	printerFramePointer->GetToolBar()->AddTool(wxID_PREVIEW, "Preview", wxBitmap(p_Main->getApplicationDir()+IMAGES_FOLDER + "/preview.png", wxBITMAP_TYPE_PNG), wxNullBitmap,  wxITEM_NORMAL, "Send output to Preview Window");
	printerFramePointer->GetToolBar()->AddTool(wxID_PRINT_SETUP, "Setup", wxBitmap(p_Main->getApplicationDir()+IMAGES_FOLDER + "/page_setup.png", wxBITMAP_TYPE_PNG), wxNullBitmap,  wxITEM_NORMAL, "Specify Preview and Printout page details");
	printerFramePointer->GetToolBar()->AddControl(printerPlotterButtonPointer);
#ifndef __WXMAC__
	printerFramePointer->GetToolBar()->AddSeparator();
#endif
    printerFramePointer->GetToolBar()->AddControl(plotterRomPointer);
	printerFramePointer->GetToolBar()->AddControl(plotterRomTextPointer);
	printerFramePointer->GetToolBar()->AddControl(plotterExtensionRomPointer);
	printerFramePointer->GetToolBar()->AddControl(plotterExtensionRomTextPointer);
	printerFramePointer->GetToolBar()->SetBackgroundColour(wxColour(230,230,230));

#if defined(__linux__)
	wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	printerPlotterButtonPointer->SetFont(defaultFont);
	plotterRomPointer->SetFont(defaultFont);
	plotterRomTextPointer->SetFont(defaultFont);
	plotterExtensionRomPointer->SetFont(defaultFont);
	plotterExtensionRomTextPointer->SetFont(defaultFont);
	fontTextPointer->SetFont(defaultFont);
	rowsTextPointer->SetFont(defaultFont);
	printerFontPointer->SetFont(defaultFont);
	printerRowsPointer->SetFont(defaultFont);
	printerFramePointer->GetToolBar()->SetFont(defaultFont);
#endif
#if defined(__WXMAC__)
	wxFont defaultFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	printerPlotterButtonPointer->SetFont(defaultFont);
	plotterRomPointer->SetFont(defaultFont);
	plotterRomTextPointer->SetFont(defaultFont);
	plotterExtensionRomPointer->SetFont(defaultFont);
	plotterExtensionRomTextPointer->SetFont(defaultFont);
	fontTextPointer->SetFont(defaultFont);
	rowsTextPointer->SetFont(defaultFont);
	printerFontPointer->SetFont(defaultFont);
	printerRowsPointer->SetFont(defaultFont);
	printerFramePointer->GetToolBar()->SetFont(defaultFont);
#endif

	printerFramePointer->GetToolBar()->Realize();

	printerFramePointer->Show(true);
	printerFramePointer->init();
	enableToolbar(false);
	if (comxPrintMode_ == COMXPRINTPRINTER)
		enablePlotterGui(false);
	else
		enablePlotterGui(true);
    printerFrameOpen_ = true;
}

void Printer::printFinished()
{
	if (!printStarted_) return;

	printStarted_ = false;
	if (parallelFrameOpen_ || serialFrameOpen_ || thermalFrameOpen_)
	{
		enableToolbar(true);
		if (thermalFrameOpen_)
			printerFramePointer->printFinished();
	}
	else
	{
		setPrintfileName(p_Main->getPrintFile());
	}
}

void Printer::enableToolbar(bool status)
{
	printerFramePointer->GetToolBar()->EnableTool(wxID_PRINT, status);
	printerFramePointer->GetToolBar()->EnableTool(wxID_PREVIEW, status);
	printerFramePointer->GetToolBar()->EnableTool(wxID_PRINT_SETUP, status);
}

void Printer::enablePlotterGui(bool status)
{
	plotterRomPointer->Show(status);
	plotterRomTextPointer->Show(status);
	plotterExtensionRomPointer->Show(status);
	plotterExtensionRomTextPointer->Show(status);
	printerFontPointer->Show(!status);
	printerRowsPointer->Show(!status);
	fontTextPointer->Show(!status);
	rowsTextPointer->Show(!status);
	if (status)
		printerPlotterButtonPointer->SetLabel("->Printer");
	else
		printerPlotterButtonPointer->SetLabel("->Plotter");
}

void Printer::disablePrintPlotter()
{
	printerPlotterButtonPointer->Enable(false);
}

void Printer::showPlotterRomFile(wxString fileName)
{
	plotterRomTextPointer->SetValue(fileName);
}

wxString Printer::getPlotterRomExtensionFile()
{
	if (!printerFrameOpen_)
		return "";
	return plotterExtensionRomTextPointer->GetValue();
}

wxString Printer::getPlotterRomFile()
{
	if (!printerFrameOpen_)
		return "";
	return plotterRomTextPointer->GetValue();
}

void Printer::showPlotterRomExtensionFile(wxString fileName)
{
	plotterExtensionRomTextPointer->SetValue(fileName);
}

void Printer::completePage(wxDC& dc, int page)
{
	printerFramePointer->completePage(dc, page);
}

int Printer::getNumberofPages()
{
	return printerFramePointer->getNumberofPages();
}

void Printer::setPrintfileName(wxString fileName)
{
	printFileFullPath_ = fileName;
	newPrintFile_ = true;

	wxFileName FullPath = wxFileName(printFileFullPath_, wxPATH_NATIVE);
	printFileName_ = FullPath.GetFullName();
}

void Printer::printOut()
{
	wxPrintDialogData printDialogData(* PrintDataPointer);

	wxPrinter printer(& printDialogData);
	ComxPrintout printout(computerName_ + " Printout", printerPointer);

	if (!printer.Print(NULL, &printout, true ))
	{
		if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
			wxMessageBox("There was a problem printing.\nPerhaps your current printer is not set correctly?", "Printing", wxOK);
		else
			wxMessageBox("You canceled printing", "Printing", wxOK);
	}
	else
	{
		(*PrintDataPointer) = printer.GetPrintDialogData().GetPrintData();
	}
}

void Printer::printPreview()
{
	wxPrintDialogData printDialogData(* PrintDataPointer);
	wxPrintPreview *preview = new wxPrintPreview(new ComxPrintout(computerName_ + " Printout", printerPointer), new ComxPrintout(computerName_ + " Printout", printerPointer), & printDialogData);

	if (!preview->Ok())
	{
		delete preview;
		wxMessageBox("There was a problem previewing.\nPerhaps your current printer is not set correctly?", "Previewing", wxOK);
		return;
	}

	wxPreviewFrame *frame = new wxPreviewFrame(preview, printerFramePointer, computerName_ + " Printer Preview", wxPoint(-1, -1), wxSize(600, 650));
	frame->Centre(wxBOTH);
	frame->Initialize();
	frame->Show();
}

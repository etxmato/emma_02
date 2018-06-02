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
#include "colour.h"
#include "wx/spinctrl.h"

#include "wx/xrc/xmlres.h"             

BEGIN_EVENT_TABLE(ColourDialog, wxDialog)
   EVT_BUTTON(XRCID("ColourSave"), ColourDialog::onSaveButton)
   EVT_BUTTON(XRCID("ColourDefault1"), ColourDialog::onDefault1)
   EVT_BUTTON(XRCID("ColourDefault2"), ColourDialog::onDefault2)
   EVT_CHOICE(XRCID("ColourMonitor1"), ColourDialog::onMonitor1)
   EVT_CHOICE(XRCID("ColourMonitor2"), ColourDialog::onMonitor2)
   EVT_BUTTON(XRCID("ColourGray"), ColourDialog::onGray)
   EVT_BUTTON(XRCID("ColourCol"), ColourDialog::onCol)
   EVT_BUTTON(XRCID("ColourTest"), ColourDialog::onTest)

   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton0"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton1"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton2"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton3"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton4"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton5"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton6"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton7"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton8"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton9"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton10"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton11"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton12"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton13"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton14"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton15"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton16"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton17"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton18"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton19"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton20"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton21"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton22"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton23"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton24"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton25"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton26"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton27"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton28"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton29"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton30"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton31"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton32"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton33"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton34"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton35"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton36"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton37"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton38"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton39"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton40"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton41"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton42"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton43"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton44"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton45"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton46"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton47"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton48"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton49"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton50"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton51"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton52"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton53"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton54"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton55"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton56"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton57"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton58"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton59"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton60"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton61"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton62"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton63"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton64"), ColourDialog::onColourChanged)
   EVT_COLOURPICKER_CHANGED(XRCID("ColourButton65"), ColourDialog::onColourChanged)

   EVT_SPINCTRL(XRCID("BorderY0"), ColourDialog::onBorder)
   EVT_SPINCTRL(XRCID("BorderX0"), ColourDialog::onBorder)
   EVT_SPINCTRL(XRCID("BorderY1"), ColourDialog::onBorder)
   EVT_SPINCTRL(XRCID("BorderY2"), ColourDialog::onBorder)
   EVT_SPINCTRL(XRCID("BorderX2"), ColourDialog::onBorder)
   EVT_SPINCTRL(XRCID("BorderY3"), ColourDialog::onBorder)
   EVT_SPINCTRL(XRCID("BorderX3"), ColourDialog::onBorder)
   EVT_SPINCTRL(XRCID("BorderY4"), ColourDialog::onBorder)
   EVT_SPINCTRL(XRCID("BorderX4"), ColourDialog::onBorder)
   EVT_SPINCTRL(XRCID("BorderY5"), ColourDialog::onBorder)
   EVT_SPINCTRL(XRCID("BorderX5"), ColourDialog::onBorder)
   EVT_SPINCTRL(XRCID("BorderY6"), ColourDialog::onBorder)
   EVT_SPINCTRL(XRCID("BorderX6"), ColourDialog::onBorder)

   EVT_TEXT(XRCID("BorderY0"), ColourDialog::onBorderText)
   EVT_TEXT(XRCID("BorderX0"), ColourDialog::onBorderText)
   EVT_TEXT(XRCID("BorderY1"), ColourDialog::onBorderText)
   EVT_TEXT(XRCID("BorderX1Text"), ColourDialog::onBorderText)
   EVT_TEXT(XRCID("BorderY2"), ColourDialog::onBorderText)
   EVT_TEXT(XRCID("BorderX2"), ColourDialog::onBorderText)
   EVT_TEXT(XRCID("BorderY3"), ColourDialog::onBorderText)
   EVT_TEXT(XRCID("BorderX3"), ColourDialog::onBorderText)
   EVT_TEXT(XRCID("BorderY4"), ColourDialog::onBorderText)
   EVT_TEXT(XRCID("BorderX4"), ColourDialog::onBorderText)
   EVT_TEXT(XRCID("BorderY5"), ColourDialog::onBorderText)
   EVT_TEXT(XRCID("BorderX5"), ColourDialog::onBorderText)
   EVT_TEXT(XRCID("BorderY6"), ColourDialog::onBorderText)
   EVT_TEXT(XRCID("BorderX6"), ColourDialog::onBorderText)

   EVT_SPIN_UP(XRCID("BorderX1Spin"), ColourDialog::onBorderX1Up)
   EVT_SPIN_DOWN(XRCID("BorderX1Spin"), ColourDialog::onBorderX1Down)
   EVT_TEXT_ENTER(XRCID("BorderX1Text"), ColourDialog::onBorderX1TextEnter)

   EVT_TEXT(XRCID("ScaleXText"), ColourDialog::onBorderText)
   EVT_TEXT_ENTER(XRCID("ScaleXText"), ColourDialog::onScaleXTextEnter)

END_EVENT_TABLE()

ColourDialog::ColourDialog(wxWindow* parent)
{
	wxString button;
	computerTypeStr_ = p_Main->getSelectedComputerStr();
	computerType_ = p_Main->getSelectedComputerId();
	colourChanged_ = true;
//	wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	screenInfo_ = p_Main->getScreenInfo(computerType_);

	if (computerType_ == VICTORY)
		 screenInfo_.number -= 12;

	wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"colour.xrc");
	if (computerType_ == ELFII || computerType_ == SUPERELF)
		wxXmlResource::Get()->LoadDialog(this, parent, wxT("ColourDialogElf"));
	else
		wxXmlResource::Get()->LoadDialog(this, parent, wxT("ColourDialog"+computerTypeStr_));
//	XRCCTRL(*this, "ColourText", wxStaticText)->SetFont(font);
	colourChanged_ = false;
	
	if (p_Video == NULL && p_Vt100 == NULL)
		XRCCTRL(*this, "ColourTest", wxButton)->Hide();

	XRCCTRL(*this, "ColourTest", wxButton)->Enable(false);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(false);

	wxString scaleString;
	if (computerType_ == ELF || computerType_ == ELFII || computerType_ == SUPERELF || computerType_ == ELF2K || computerType_ == MS2000 || computerType_ == MCDS || computerType_ == TMC1800 || computerType_ == STUDIO || computerType_ == COINARCADE || computerType_ == VISICOM || computerType_ == VIP || computerType_ == VIPII || computerType_ == VELF)
	{
		scaleString = p_Main->getConfigItem(computerTypeStr_+"/Window_Scale_Factor_X", "3");
		XRCCTRL(*this, "ScaleXText", wxTextCtrl)->ChangeValue(scaleString);
	}
	if (computerType_ == COSMICOS || computerType_ == TMC2000 || computerType_ == ETI || computerType_ == VICTORY || computerType_ == STUDIOIV || computerType_ == NANO)
	{
		scaleString = p_Main->getConfigItem(computerTypeStr_+"/Window_Scale_Factor_X", "4");
		XRCCTRL(*this, "ScaleXText", wxTextCtrl)->ChangeValue(scaleString);
	}
    if (computerType_ == FRED)
    {
        scaleString = p_Main->getConfigItem(computerTypeStr_+"/Window_Scale_Factor_X", "1");
        XRCCTRL(*this, "ScaleXText", wxTextCtrl)->ChangeValue(scaleString);
    }

	double scaleValue;
	if (scaleString.ToDouble(&scaleValue))
	{
		scaleValue = (int)scaleValue;
	}
	else scaleValue = 3;

	for (int i=screenInfo_.start; i<screenInfo_.number; i++)
	{
		button.Printf("%d", i);
		button.Trim(false);
		XRCCTRL(*this, "ColourButton"+button, wxColourPickerCtrl)->SetColour(p_Main->getConfigItem(computerTypeStr_+"/Colour"+button, screenInfo_.defaultColour[i]));
	}
	for (int i=0; i<screenInfo_.numberVideo; i++)
	{
		button.Printf("%d", i);
		button.Trim(false);
		if (i == 1)
		{
			wxString valueString;
			valueString.Printf("%i", p_Main->getConfigItem(computerTypeStr_+"/BorderX"+button, screenInfo_.borderX[i])*(int)scaleValue);
			XRCCTRL(*this, "BorderX1Text", wxTextCtrl)->ChangeValue(valueString);
		}
		else
			XRCCTRL(*this, "BorderX"+button, wxSpinCtrl)->SetValue(p_Main->getConfigItem(computerTypeStr_+"/BorderX"+button, screenInfo_.borderX[i]));
		XRCCTRL(*this, "BorderY"+button, wxSpinCtrl)->SetValue(p_Main->getConfigItem(computerTypeStr_+"/BorderY"+button, screenInfo_.borderY[i]));
	}
}

ColourDialog::~ColourDialog()
{
	if (!colourChanged_)
		return;
	if (p_Video != NULL || p_Vt100 != NULL)
	{
		wxColour colour;
		wxString button;
		int borderX;
		int borderY;
		for (int i=screenInfo_.start; i<screenInfo_.number; i++)
		{
			button.Printf("%d", i);
			button.Trim(false);
			colour = p_Main->getConfigItem(computerTypeStr_+"/Colour"+button, screenInfo_.defaultColour[i]);
			if (p_Video != NULL)
				p_Video->setColour(i, colour.GetAsString(wxC2S_HTML_SYNTAX));
			if (p_Vt100 != NULL)
				p_Vt100->setColour(i, colour.GetAsString(wxC2S_HTML_SYNTAX));
		}
		for (int i=0; i<screenInfo_.numberVideo; i++)
		{
			button.Printf("%d", i);
			button.Trim(false);
			borderX = p_Main->getConfigItem(computerTypeStr_+"/BorderX"+button, screenInfo_.borderX[i]);
			borderY = p_Main->getConfigItem(computerTypeStr_+"/BorderY"+button, screenInfo_.borderY[i]);
			if (p_Video != NULL)
				p_Video->setBorder(i, borderX, borderY);
			if (p_Vt100 != NULL)
				p_Vt100->setBorder(i, borderX, borderY);
		}
		if (computerType_ == COSMICOS || computerType_ == TMC2000 || computerType_ == ETI || computerType_ == VICTORY || computerType_ == STUDIOIV || computerType_ == NANO || computerType_ == VIP || computerType_ == VIPII || computerType_ == VELF || computerType_ == ELF || computerType_ == ELFII || computerType_ == SUPERELF || computerType_ == ELF2K || computerType_ == MS2000 || computerType_ == MCDS || computerType_ == TMC1800 || computerType_ == STUDIO || computerType_ == COINARCADE || computerType_ == FRED || computerType_ == VISICOM)
		{
			double scale;
			wxString scaleString = p_Main->getConfigItem(computerTypeStr_+"/Window_Scale_Factor_X", "");
			if (scaleString.ToDouble(&scale))
			{
				if (!p_Main->isFullScreenFloat())
				{
					scale = (int)scale;
					scaleString.Printf("%i", (int)scale);
				}
				if (scale < 1)
				{
					scale = 1;
					scaleString.Printf("%i", (int)scale);

				}
				if (scale > 5)
				{
					scale = 5;
					scaleString.Printf("%i", (int)scale);
				}
				p_Main->setConfigItem(computerTypeStr_+"/Window_Scale_Factor_X", scaleString);
				if (p_Video != NULL)
					p_Video->setScale(scale);
				p_Main->setScale(scaleString);
			}
		}
		if (p_Video != NULL)
			p_Video->reColour();
		if (p_Vt100 != NULL)
			p_Vt100->reColour();
	}
}

void ColourDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
	wxColour colour;
	long border;
	wxString button, scaleString;
	for (int i=screenInfo_.start; i<screenInfo_.number; i++)
	{
		button.Printf("%d", i);
		button.Trim(false);
		colour = XRCCTRL(*this, "ColourButton"+button, wxColourPickerCtrl)->GetColour();
		p_Main->setConfigItem(computerTypeStr_+"/Colour"+button, colour.GetAsString(wxC2S_HTML_SYNTAX));
	}

	if (computerType_ == COSMICOS || computerType_ == TMC2000 || computerType_ == ETI || computerType_ == VICTORY || computerType_ == STUDIOIV || computerType_ == NANO || computerType_ == VIP || computerType_ == VIPII || computerType_ == VELF || computerType_ == ELF || computerType_ == ELFII || computerType_ == SUPERELF || computerType_ == ELF2K || computerType_ == MS2000 || computerType_ == MCDS || computerType_ == TMC1800 || computerType_ == STUDIO || computerType_ == COINARCADE || computerType_ == FRED || computerType_ == VISICOM)
	{
		double scale;
		scaleString = XRCCTRL(*this, "ScaleXText", wxTextCtrl)->GetValue();
		if (scaleString.ToDouble(&scale))
		{
			if (!p_Main->isFullScreenFloat())
				scaleString.Printf("%i", (int)scale);
			if (scale < 1)
			{
				scale = 1;
				scaleString.Printf("%i", (int)scale);
			}
			if (scale > 5)
			{
				scale = 5;
				scaleString.Printf("%i", (int)scale);
			}

			p_Main->setConfigItem(computerTypeStr_+"/Window_Scale_Factor_X", scaleString);
			p_Main->setScale(scaleString);
		}
	}
	double scaleValue;
	if (scaleString.ToDouble(&scaleValue))
	{
		scaleValue = (int)scaleValue;
	}
	else scaleValue = 3;

	for (int i=0; i<screenInfo_.numberVideo; i++)
	{
		button.Printf("%d", i);
		button.Trim(false);
		if (i == 1)
		{
			wxString valueString = XRCCTRL(*this, "BorderX1Text", wxTextCtrl)->GetValue();
			valueString.ToLong(&border);
			border /= scaleValue;
		}
		else
			border = XRCCTRL(*this, "BorderX"+button, wxSpinCtrl)->GetValue();
		p_Main->setConfigItem(computerTypeStr_+"/BorderX"+button, border);
		border = XRCCTRL(*this, "BorderY"+button, wxSpinCtrl)->GetValue();
		p_Main->setConfigItem(computerTypeStr_+"/BorderY"+button, border);
	}
	EndModal( wxID_OK );
}

void ColourDialog::onTest( wxCommandEvent& WXUNUSED(event) )
{
	wxString scaleString;
	if (p_Video != NULL || p_Vt100 != NULL)
	{
		wxColour colour;
		wxString button;
		long borderX;
		int borderY;
		for (int i=screenInfo_.start; i<screenInfo_.number; i++)
		{
			button.Printf("%d", i);
			button.Trim(false);
			colour = XRCCTRL(*this, "ColourButton"+button, wxColourPickerCtrl)->GetColour();
			if (p_Video != NULL)
				p_Video->setColour(i, colour.GetAsString(wxC2S_HTML_SYNTAX));
			if (p_Vt100 != NULL)
				p_Vt100->setColour(i, colour.GetAsString(wxC2S_HTML_SYNTAX));
		}

		if (computerType_ == COSMICOS || computerType_ == TMC2000 || computerType_ == ETI || computerType_ == VICTORY || computerType_ == STUDIOIV || computerType_ == NANO || computerType_ == VIP || computerType_ == VIPII || computerType_ == VELF || computerType_ == ELF || computerType_ == ELFII || computerType_ == SUPERELF || computerType_ == ELF2K || computerType_ == MS2000 || computerType_ == MCDS || computerType_ == TMC1800 || computerType_ == STUDIO || computerType_ == COINARCADE || computerType_ == FRED || computerType_ == VISICOM)
		{
			double scale;
			scaleString = XRCCTRL(*this, "ScaleXText", wxTextCtrl)->GetValue();
			if (scaleString.ToDouble(&scale))
			{
				if (!p_Main->isFullScreenFloat())
				{
					scale = (int)scale;
					scaleString.Printf("%i", (int)scale);
				}
				if (scale < 1)
				{
					scale = 1;
					scaleString.Printf("%i", (int)scale);
				}
				if (scale > 5)
				{
					scale = 5;
					scaleString.Printf("%i", (int)scale);
				}
				XRCCTRL(*this, "ScaleXText", wxTextCtrl)->ChangeValue(scaleString);
				if (p_Video != NULL)
					p_Video->setScale(scale);
				p_Main->setScale(scaleString);
			}
		}
		double scaleValue;
		if (scaleString.ToDouble(&scaleValue))
		{
			scaleValue = (int)scaleValue;
		}
		else scaleValue = 3;

		for (int i=0; i<screenInfo_.numberVideo; i++)
		{
			button.Printf("%d", i);
			button.Trim(false);
			if (i == 1)
			{
				wxString valueString = XRCCTRL(*this, "BorderX1Text", wxTextCtrl)->GetValue();
				valueString.ToLong(&borderX);
				borderX /= scaleValue;
			}
			else
				borderX = XRCCTRL(*this, "BorderX"+button, wxSpinCtrl)->GetValue();
			borderY = XRCCTRL(*this, "BorderY"+button, wxSpinCtrl)->GetValue();
			if (p_Video != NULL)
				p_Video->setBorder(i, (int)borderX, borderY);
			if (p_Vt100 != NULL)
				p_Vt100->setBorder(i, (int)borderX, borderY);
		}
		if (p_Video != NULL)
			p_Video->reColour();
		if (p_Vt100 != NULL)
			p_Vt100->reColour();
	}
}

void ColourDialog::onDefault1( wxCommandEvent& WXUNUSED(event) )
{
	colourChanged_ = true;
	XRCCTRL(*this, "ColourTest", wxButton)->Enable(true);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(true);
	screenInfo_ = p_Main->getScreenInfo(computerType_);

	if (computerType_ == VICTORY)
		 screenInfo_.number -= 12;

	wxString scaleString = "3";
	if (computerType_ == ELF || computerType_ == ELFII || computerType_ == SUPERELF || computerType_ == ELF2K || computerType_ == MS2000 || computerType_ == MCDS || computerType_ == TMC1800 || computerType_ == STUDIO || computerType_ == COINARCADE || computerType_ == FRED || computerType_ == VISICOM || computerType_ == VIP || computerType_ == VIPII || computerType_ == VELF)
	{
		XRCCTRL(*this, "ScaleXText", wxTextCtrl)->ChangeValue("3");
	}
	if (computerType_ == COSMICOS || computerType_ == TMC2000 || computerType_ == ETI || computerType_ == VICTORY || computerType_ == STUDIOIV || computerType_ == NANO)
	{
		XRCCTRL(*this, "ScaleXText", wxTextCtrl)->ChangeValue("4");
		scaleString = "4";
	}
	double scaleValue;
	if (scaleString.ToDouble(&scaleValue))
	{
		scaleValue = (int)scaleValue;
	}
	else scaleValue = 3;
	wxString button;

	for (int i=screenInfo_.start; i<screenInfo_.number; i++)
	{
		button.Printf("%d", i);
		button.Trim(false);
		XRCCTRL(*this, "ColourButton"+button, wxColourPickerCtrl)->SetColour(screenInfo_.defaultColour[i]);
	}
	for (int i=0; i<screenInfo_.numberVideo; i++)
	{
		button.Printf("%d", i);
		button.Trim(false);
		if (i == 1)
		{
			wxString valueString;
			valueString.Printf("%i", (int)(screenInfo_.borderX[i]*scaleValue));
			XRCCTRL(*this, "BorderX1Text", wxTextCtrl)->ChangeValue(valueString);
		}
		else
			XRCCTRL(*this, "BorderX"+button, wxSpinCtrl)->SetValue(screenInfo_.borderX[i]);
		XRCCTRL(*this, "BorderY"+button, wxSpinCtrl)->SetValue(screenInfo_.borderY[i]);
	}
}

void ColourDialog::onDefault2( wxCommandEvent& WXUNUSED(event) )
{
	colourChanged_ = true;
	XRCCTRL(*this, "ColourTest", wxButton)->Enable(true);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(true);
	screenInfo_ = p_Main->getScreenInfo(computerType_);

	wxString button;
	for (int i=screenInfo_.start+12; i<screenInfo_.number; i++)
	{
		button.Printf("%d", i-12);
		button.Trim(false);
		XRCCTRL(*this, "ColourButton"+button, wxColourPickerCtrl)->SetColour(screenInfo_.defaultColour[i]);
	}
	 screenInfo_.number -= 12;
}

void ColourDialog::onMonitor1( wxCommandEvent&event)
{
	int colourIndex = 2;

	if (computerType_ == VIP)
		colourIndex = 12;

	colourChanged_ = true;
	XRCCTRL(*this, "ColourTest", wxButton)->Enable(true);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(true);
	switch(event.GetSelection())
	{
		case 0:
		return;
		case 1:
			screenInfo_.defaultColour[colourIndex] = "#00ff00";
			screenInfo_.defaultColour[colourIndex+1] = "#004000";
			screenInfo_.defaultColour[colourIndex+2] = "#00ff00";
		break;
		case 2:
			screenInfo_.defaultColour[colourIndex] = "#ffc418";
			screenInfo_.defaultColour[colourIndex+1] = "#401000";
			screenInfo_.defaultColour[colourIndex+2] = "#ffc418";
		break;
		case 3:
			screenInfo_.defaultColour[colourIndex] = "#ffffff";
			screenInfo_.defaultColour[colourIndex+1] = "#000000";
			screenInfo_.defaultColour[colourIndex+2] = "#ffffff";
		break;
		case 4:
			screenInfo_.defaultColour[colourIndex] = "#00ff00";
			screenInfo_.defaultColour[colourIndex+1] = "#000000";
			screenInfo_.defaultColour[colourIndex+2] = "#00ff00";
		break;
		case 5:
			screenInfo_.defaultColour[colourIndex] = "#ffc418";
			screenInfo_.defaultColour[colourIndex+1] = "#000000";
			screenInfo_.defaultColour[colourIndex+2] = "#ffc418";
		break;
	}
	wxString button;
	for (int i=colourIndex; i<colourIndex+3; i++)
	{
		button.Printf("%d", i);
		button.Trim(false);
		XRCCTRL(*this, "ColourButton"+button, wxColourPickerCtrl)->SetColour(screenInfo_.defaultColour[i]);
	}
}

void ColourDialog::onMonitor2( wxCommandEvent&event)
{
	colourChanged_ = true;
	XRCCTRL(*this, "ColourTest", wxButton)->Enable(true);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(true);
	switch(event.GetSelection())
	{
		case 0:
		return;
		case 1:
			screenInfo_.defaultColour[64] = "#00ff00";
			screenInfo_.defaultColour[65] = "#004000";
		break;
		case 2:
			screenInfo_.defaultColour[64] = "#ffc418";
			screenInfo_.defaultColour[65] = "#401000";
		break;
		case 3:
			screenInfo_.defaultColour[64] = "#ffffff";
			screenInfo_.defaultColour[65] = "#000000";
		break;
		case 4:
			screenInfo_.defaultColour[64] = "#00ff00";
			screenInfo_.defaultColour[65] = "#000000";
		break;
		case 5:
			screenInfo_.defaultColour[64] = "#ffc418";
			screenInfo_.defaultColour[65] = "#000000";
		break;
	}
	wxString button;
	for (int i=64; i<66; i++)
	{
		button.Printf("%d", i);
		button.Trim(false);
		XRCCTRL(*this, "ColourButton"+button, wxColourPickerCtrl)->SetColour(screenInfo_.defaultColour[i]);
	}
}

void ColourDialog::onGray( wxCommandEvent& WXUNUSED(event) )
{
	colourChanged_ = true;
	XRCCTRL(*this, "ColourTest", wxButton)->Enable(true);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(true);
	screenInfo_.defaultColour[16] = "#000000";
	screenInfo_.defaultColour[17] = "#000000";
	screenInfo_.defaultColour[18] = "#181818";
	screenInfo_.defaultColour[19] = "#303030";
	screenInfo_.defaultColour[20] = "#404040";
	screenInfo_.defaultColour[21] = "#505050";
	screenInfo_.defaultColour[22] = "#606060";
	screenInfo_.defaultColour[23] = "#707070";
	screenInfo_.defaultColour[24] = "#808080";
	screenInfo_.defaultColour[25] = "#909090";
	screenInfo_.defaultColour[26] = "#a0a0a0";
	screenInfo_.defaultColour[27] = "#b0b0b0";
	screenInfo_.defaultColour[28] = "#c0c0c0";
	screenInfo_.defaultColour[29] = "#d0d0d0";
	screenInfo_.defaultColour[30] = "#e0e0e0";
	screenInfo_.defaultColour[31] = "#ffffff";
	wxString button;
	for (int i=16; i<31; i++)
	{
		button.Printf("%d", i);
		button.Trim(false);
		XRCCTRL(*this, "ColourButton"+button, wxColourPickerCtrl)->SetColour(screenInfo_.defaultColour[i]);
	}
}

void ColourDialog::onCol( wxCommandEvent& WXUNUSED(event) )
{
	colourChanged_ = true;
	XRCCTRL(*this, "ColourTest", wxButton)->Enable(true);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(true);
	screenInfo_.defaultColour[16] = "#000000";
	screenInfo_.defaultColour[17] = "#000000";
	screenInfo_.defaultColour[18] = "#007f00";
	screenInfo_.defaultColour[19] = "#00ff00";
	screenInfo_.defaultColour[20] = "#00003f";
	screenInfo_.defaultColour[21] = "#0000ff";
	screenInfo_.defaultColour[22] = "#3f0000";
	screenInfo_.defaultColour[23] = "#007f7f";
	screenInfo_.defaultColour[24] = "#7f0000";
	screenInfo_.defaultColour[25] = "#ff0000";
	screenInfo_.defaultColour[26] = "#7f7f00";
	screenInfo_.defaultColour[27] = "#ffff00";
	screenInfo_.defaultColour[28] = "#003f00";
	screenInfo_.defaultColour[29] = "#7f007f";
	screenInfo_.defaultColour[30] = "#7f7f7f";
	screenInfo_.defaultColour[31] = "#ffffff";
	wxString button;
	for (int i=16; i<31; i++)
	{
		button.Printf("%d", i);
		button.Trim(false);
		XRCCTRL(*this, "ColourButton"+button, wxColourPickerCtrl)->SetColour(screenInfo_.defaultColour[i]);
	}
}

void ColourDialog::onColourChanged( wxColourPickerEvent& WXUNUSED(event) )
{
	colourChanged_ = true;
	XRCCTRL(*this, "ColourTest", wxButton)->Enable(true);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(true);
}

void ColourDialog::onBorder( wxSpinEvent& WXUNUSED(event)  )
{
	colourChanged_ = true;
	XRCCTRL(*this, "ColourTest", wxButton)->Enable(true);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(true);
}

void ColourDialog::onBorderX1Up( wxSpinEvent& WXUNUSED(event) )
{
	long value;
	wxString valueString = XRCCTRL(*this, "BorderX1Text", wxTextCtrl)->GetValue();
	valueString.ToLong(&value);

	wxString scaleString = XRCCTRL(*this, "ScaleXText", wxTextCtrl)->GetValue();
	double scaleValue;

	if (scaleString.ToDouble(&scaleValue))
	{
		scaleValue = (int)scaleValue;
	}
	else scaleValue = 3;

	value += scaleValue;
	if (value >= 100)
	{
		value = 100;
		int floorValue = value / scaleValue;
		floorValue *= scaleValue;
	} 

	valueString.Printf("%ld", value);
	XRCCTRL(*this, "BorderX1Text", wxTextCtrl)->ChangeValue(valueString);

	colourChanged_ = true;
	XRCCTRL(*this, "ColourTest", wxButton)->Enable(true);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(true);
}

void ColourDialog::onBorderX1Down( wxSpinEvent& WXUNUSED(event) )
{
	long value;
	wxString valueString = XRCCTRL(*this, "BorderX1Text", wxTextCtrl)->GetValue();
	valueString.ToLong(&value);

	wxString scaleString = XRCCTRL(*this, "ScaleXText", wxTextCtrl)->GetValue();
	double scaleValue;

	if (scaleString.ToDouble(&scaleValue))
	{
		scaleValue = (int)scaleValue;
	}
	else scaleValue = 3;

	value -= scaleValue;
	if (value <= 0)  value = 0;

	valueString.Printf("%ld", value);
	XRCCTRL(*this, "BorderX1Text", wxTextCtrl)->ChangeValue(valueString);

	colourChanged_ = true;
	XRCCTRL(*this, "ColourTest", wxButton)->Enable(true);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(true);
}

void ColourDialog::onBorderX1TextEnter( wxCommandEvent& WXUNUSED(event) )
{
	long value;
	wxString valueString = XRCCTRL(*this, "BorderX1Text", wxTextCtrl)->GetValue();
	valueString.ToLong(&value);

	wxString scaleString = XRCCTRL(*this, "ScaleXText", wxTextCtrl)->GetValue();
	double scaleValue;

	if (scaleString.ToDouble(&scaleValue))
	{
		scaleValue = (int)scaleValue;
	}
	else scaleValue = 3;

	int floorValue = value / scaleValue;
	floorValue *= scaleValue;
	
	if (floorValue <= 0)  floorValue = 0;
	if (floorValue >= 100)  floorValue = 100;
	if (value - floorValue >= (int)(ceil(scaleValue/2)))  floorValue +=scaleValue; 

	valueString.Printf("%i", floorValue);
	XRCCTRL(*this, "BorderX1Text", wxTextCtrl)->ChangeValue(valueString);

	colourChanged_ = true;
	XRCCTRL(*this, "ColourTest", wxButton)->Enable(true);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(true);
}

void ColourDialog::onBorderText( wxCommandEvent& WXUNUSED(event) )
{
	if (colourChanged_)  return;
	colourChanged_ = true;
	XRCCTRL(*this, "ColourTest", wxButton)->Enable(true);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(true);
}

void ColourDialog::onScaleXTextEnter( wxCommandEvent& WXUNUSED(event) )
{
	wxString valueString = XRCCTRL(*this, "ScaleXText", wxTextCtrl)->GetValue();
	double value;

	if (valueString.ToDouble(&value))
	{
		if (!p_Main->isFullScreenFloat())
		{
			valueString.Printf("%i", (int)value);
		}
		if (value < 1)
		{
			value = 1;
			valueString.Printf("%i", (int)value);
		}
		if (value > 5)
		{
			value = 5;
			valueString.Printf("%i", (int)value);
		}
		XRCCTRL(*this, "ScaleXText", wxTextCtrl)->ChangeValue(valueString);
	}

	colourChanged_ = true;
	XRCCTRL(*this, "ColourTest", wxButton)->Enable(true);
	XRCCTRL(*this, "ColourSave", wxButton)->Enable(true);
}

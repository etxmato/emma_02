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
#include "guidiag.h"

#include "wx/xrc/xmlres.h"             

BEGIN_EVENT_TABLE(DiagDialog, wxDialog)
    EVT_BUTTON(XRCID("SBSave"), DiagDialog::onSaveButton)
	EVT_BUTTON(XRCID("DIAG_ROM_0_Button"), DiagDialog::onDiagPal0Button)
	EVT_BUTTON(XRCID("DIAG_ROM_1_Button"), DiagDialog::onDiagPal1Button)
	EVT_BUTTON(XRCID("DIAG_NTSC_ROM_0_Button"), DiagDialog::onDiagNtsc0Button)
	EVT_BUTTON(XRCID("DIAG_NTSC_ROM_1_Button"), DiagDialog::onDiagNtsc1Button)
	END_EVENT_TABLE()

DiagDialog::DiagDialog(wxWindow* parent)
{
	wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"diag_" + p_Main->getFontSize() + ".xrc");
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("DiagDialog"));

    for (int i=0; i<2; i++)
    {
		DiagPalRomDir_[i] = p_Main->getDiagPalRomDirectory(i);
		DiagNtscRomDir_[i] = p_Main->getDiagNtscRomDirectory(i);
	}

	XRCCTRL(*this, "DIAG_ROM_0", wxComboBox)->SetValue(p_Main->getDiagPalRom(0));
	XRCCTRL(*this, "DIAG_ROM_1", wxComboBox)->SetValue(p_Main->getDiagPalRom(1));
	XRCCTRL(*this, "DIAG_NTSC_ROM_0", wxComboBox)->SetValue(p_Main->getDiagNtscRom(0));
	XRCCTRL(*this, "DIAG_NTSC_ROM_1", wxComboBox)->SetValue(p_Main->getDiagNtscRom(1));


    XRCCTRL(*this, "DIAG_CHECKSUM", wxChoice)->SetSelection(p_Main->getDiagRomChecksum());
    XRCCTRL(*this, "DIAG_FACTORY", wxChoice)->SetSelection(p_Main->getDiagFactory());
    XRCCTRL(*this, "DIAG_CASSETTE", wxChoice)->SetSelection(p_Main->getDiagCassetteCables());
}

void DiagDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
	wxString number;

    p_Main->setDiagRomChecksum(XRCCTRL(*this, "DIAG_CHECKSUM", wxChoice)->GetSelection());
    p_Main->setDiagFactory(XRCCTRL(*this, "DIAG_FACTORY", wxChoice)->GetSelection());
    p_Main->setDiagCassetteCables(XRCCTRL(*this, "DIAG_CASSETTE", wxChoice)->GetSelection());
	for (int i = 0; i<2; i++)
	{
		number.Printf("%01d", i);
		p_Main->setDiagPalRomDirectory(i, DiagPalRomDir_[i]);
		p_Main->setDiagPalRom(i, XRCCTRL(*this, "DIAG_ROM_" + number, wxComboBox)->GetValue());
		number.Printf("%01d", i);
		p_Main->setDiagNtscRomDirectory(i, DiagNtscRomDir_[i]);
		p_Main->setDiagNtscRom(i, XRCCTRL(*this, "DIAG_NTSC_ROM_" + number, wxComboBox)->GetValue());
	}

	p_Main->diagSbChange();

	EndModal( wxID_OK );
}


void DiagDialog::onDiagPal0Button(wxCommandEvent& WXUNUSED(event))
{
	DiagPalButton(0, "main ROM replacement");
}

void DiagDialog::onDiagPal1Button(wxCommandEvent& WXUNUSED(event))
{
	DiagPalButton(1, "ROM C000-D7FF");
}

void DiagDialog::onDiagNtsc0Button(wxCommandEvent& WXUNUSED(event))
{
	DiagNtscButton(0, "main ROM replacement");
}

void DiagDialog::onDiagNtsc1Button(wxCommandEvent& WXUNUSED(event))
{
	DiagNtscButton(1, "ROM C000-D7FF");
}

void DiagDialog::DiagPalButton(int number, wxString textMessage)
{
	wxString stringNumber;
	wxString fileName;

	stringNumber.Printf("%01d", number);
	fileName = wxFileSelector( "Select the PAL Diag " + textMessage +" file to load",
                               DiagPalRomDir_[number], XRCCTRL(*this, "DIAG_ROM_" + stringNumber, wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                    "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	DiagPalRomDir_[number] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	XRCCTRL(*this, "DIAG_ROM_" + stringNumber, wxComboBox)->SetValue(FullPath.GetFullName());
}

void DiagDialog::DiagNtscButton(int number, wxString textMessage)
{
	wxString stringNumber;
	wxString fileName;

	stringNumber.Printf("%01d", number);
	fileName = wxFileSelector("Select the NTSC Diag " + textMessage + " file to load",
		DiagNtscRomDir_[number], XRCCTRL(*this, "DIAG_NTSC_ROM_" + stringNumber, wxComboBox)->GetValue(),
		"",
		wxString::Format
		(
			"Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s",
			wxFileSelectorDefaultWildcardStr,
			wxFileSelectorDefaultWildcardStr
			),
		wxFD_OPEN | wxFD_CHANGE_DIR | wxFD_PREVIEW,
		this
		);
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	DiagNtscRomDir_[number] = FullPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	XRCCTRL(*this, "DIAG_NTSC_ROM_" + stringNumber, wxComboBox)->SetValue(FullPath.GetFullName());
}

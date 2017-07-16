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
    EVT_BUTTON(XRCID("DIAG_ROM_0_Button"), DiagDialog::onDiag0Button)
    EVT_BUTTON(XRCID("DIAG_ROM_1_Button"), DiagDialog::onDiag1Button)
END_EVENT_TABLE()

DiagDialog::DiagDialog(wxWindow* parent)
{
	wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"diag.xrc");
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("DiagDialog"));

    for (int i=0; i<2; i++)
    {
        DiagRomDir_[i] = p_Main->getDiagRomDirectory(i);
    }

	XRCCTRL(*this, "DIAG_ROM_0", wxComboBox)->SetValue(p_Main->getDiagRom(0));
	XRCCTRL(*this, "DIAG_ROM_1", wxComboBox)->SetValue(p_Main->getDiagRom(1));

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
    for (int i=0; i<2; i++)
	{
		number.Printf("%01d",i);
        p_Main->setDiagRomDirectory(i, DiagRomDir_[i]);
        p_Main->setDiagRom(i, XRCCTRL(*this, "DIAG_ROM_"+number, wxComboBox)->GetValue());
	}

	EndModal( wxID_OK );
}


void DiagDialog::onDiag0Button(wxCommandEvent& WXUNUSED(event) )
{
	DiagButton(0, "main ROM replacement");
}

void DiagDialog::onDiag1Button(wxCommandEvent& WXUNUSED(event) )
{
	DiagButton(1, "ROM C000-D7FF");
}

void DiagDialog::DiagButton(int number, wxString textMessage)
{
	wxString stringNumber;
	wxString fileName;

	stringNumber.Printf("%01d", number);
	fileName = wxFileSelector( "Select the Diag " + textMessage +" file to load",
                               DiagRomDir_[number], XRCCTRL(*this, "DIAG_ROM_" + stringNumber, wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                    "Binary File|*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	DiagRomDir_[number] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	XRCCTRL(*this, "DIAG_ROM_" + stringNumber, wxComboBox)->SetValue(FullPath.GetFullName());
}

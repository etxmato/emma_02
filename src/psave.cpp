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
#include "psave.h"

#include "wx/xrc/xmlres.h"             
#include <wx/spinctrl.h>

BEGIN_EVENT_TABLE(PsaveDialog, wxDialog)
    EVT_BUTTON(XRCID("PsaveSave"), PsaveDialog::onSaveButton)
END_EVENT_TABLE()

PsaveDialog::PsaveDialog(wxWindow* parent)
{
	wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"cassette.xrc");
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("PsaveDialog"));

	XRCCTRL(*this, "PsaveVolume", wxSpinCtrl)->SetValue(p_Main->getPsaveData(0));
	XRCCTRL(*this, "PsaveBitRate", wxChoice)->SetSelection(p_Main->getPsaveData(1));
	XRCCTRL(*this, "PsaveBitsPerSample", wxChoice)->SetSelection(p_Main->getPsaveData(2));
	XRCCTRL(*this, "ConversionTypeWav", wxChoice)->SetSelection(p_Main->getPsaveData(3));
	XRCCTRL(*this, "Channel", wxChoice)->SetSelection(p_Main->getPsaveData(4));
	XRCCTRL(*this, "ConversionTypeCassette", wxChoice)->SetSelection(p_Main->getPsaveData(7));
	XRCCTRL(*this, "Playback", wxCheckBox)->SetValue(p_Main->getPsaveData(5) != 0);
	XRCCTRL(*this, "ReversedPolarity", wxCheckBox)->SetValue(p_Main->getPsaveData(6) != 0);

}

void PsaveDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
	if (p_Main->isSaving())
		return;

	p_Main->setPsaveData(0, XRCCTRL(*this, "PsaveVolume", wxSpinCtrl)->GetValue());
	p_Main->setPsaveData(1, XRCCTRL(*this, "PsaveBitRate", wxChoice)->GetSelection());
	p_Main->setPsaveData(2, XRCCTRL(*this, "PsaveBitsPerSample", wxChoice)->GetSelection());
	p_Main->setPsaveData(3, XRCCTRL(*this, "ConversionTypeWav", wxChoice)->GetSelection());
	p_Main->setPsaveData(4, XRCCTRL(*this, "Channel", wxChoice)->GetSelection());
	p_Main->setPsaveData(7, XRCCTRL(*this, "ConversionTypeCassette", wxChoice)->GetSelection());
	p_Main->setPsaveData(5, XRCCTRL(*this, "Playback", wxCheckBox)->GetValue());
	p_Main->setPsaveData(6, XRCCTRL(*this, "ReversedPolarity", wxCheckBox)->GetValue());

	EndModal( wxID_OK );
}


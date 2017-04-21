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
#include "breakpoints.h"

#include "wx/xrc/xmlres.h"             

BEGIN_EVENT_TABLE(BreakPointsDialog, wxDialog)
    EVT_BUTTON(XRCID("CloseBreakPoint"), BreakPointsDialog::onClose)
END_EVENT_TABLE()

BreakPointsDialog::BreakPointsDialog(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("BreakPointDialog"));
}

void BreakPointsDialog::onClose( wxCommandEvent& WXUNUSED(event) )
{
	EndModal( wxID_OK );
}

 

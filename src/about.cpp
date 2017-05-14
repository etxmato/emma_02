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

#include "about.h"
#include "main.h"

#include "wx/xrc/xmlres.h"             

BEGIN_EVENT_TABLE(MyAboutDialog, wxDialog)
    EVT_BUTTON(XRCID("AboutOk"), MyAboutDialog::onOkButton)
END_EVENT_TABLE()

MyAboutDialog::MyAboutDialog(wxWindow* parent)
{
	wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"about.xrc");
	wxXmlResource::Get()->LoadDialog(this, parent, _T("AboutDialog"));
	XRCCTRL(*this, "EmmaVersion", wxStaticText)->SetLabel("Emma 02 - V"+p_Main->getConfigItem("/Main/Version", EMMA_VERSION));
}

void MyAboutDialog::onOkButton(wxCommandEvent& WXUNUSED(event))
{
	EndModal(wxID_OK);
}

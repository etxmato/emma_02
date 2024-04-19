/////////////////////////////////////////////////////////////////////////////
// Name:        popupcomx.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     Saturday, 08 July 2023 at 21:39:51
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "popupcomx.h"

////@begin XPM images

////@end XPM images


/*!
 * wxDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog, wxDialog )


/*!
 * wxDialog event table definition
 */

BEGIN_EVENT_TABLE( wxDialog, wxDialog )

////@begin wxDialog event table entries
////@end wxDialog event table entries

END_EVENT_TABLE()


/*!
 * wxDialog constructors
 */

wxDialog::wxDialog()
{
    Init();
}

wxDialog::wxDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * wxDialog creator
 */

bool wxDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end wxDialog creation
    return true;
}


/*!
 * wxDialog destructor
 */

wxDialog::~wxDialog()
{
////@begin wxDialog destruction
////@end wxDialog destruction
}


/*!
 * Member initialisation
 */

void wxDialog::Init()
{
////@begin wxDialog member initialisation
////@end wxDialog member initialisation
}


/*!
 * Control creation for wxDialog
 */

void wxDialog::CreateControls()
{    
////@begin wxDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), wxT("SBV1_VT")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
////@end wxDialog content construction

    // Create custom windows not generated automatically here.
////@begin wxDialog content initialisation
////@end wxDialog content initialisation
}


/*!
 * Should we show tooltips?
 */

bool wxDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxDialog icon retrieval
}

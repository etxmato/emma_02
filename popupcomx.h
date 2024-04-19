/////////////////////////////////////////////////////////////////////////////
// Name:        popupcomx.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     Saturday, 08 July 2023 at 21:39:51
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _POPUPCOMX_H_
#define _POPUPCOMX_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/xrc/xmlres.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define SBV1_VT 10005
#define SYMBOL_WXDIALOG_STYLE wxCAPTION|wxSTAY_ON_TOP|wxCLOSE_BOX|wxBORDER_THEME|wxTAB_TRAVERSAL
#define SYMBOL_WXDIALOG_TITLE _("Emma 02 - Info")
#define SYMBOL_WXDIALOG_IDNAME SBV1_VT
#define SYMBOL_WXDIALOG_SIZE wxDefaultSize
#define SYMBOL_WXDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * wxDialog class declaration
 */

class wxDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( wxDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    wxDialog();
    wxDialog( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_POSITION, const wxSize& size = SYMBOL_WXDIALOG_SIZE, long style = SYMBOL_WXDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXDIALOG_IDNAME, const wxString& caption = SYMBOL_WXDIALOG_TITLE, const wxPoint& pos = SYMBOL_WXDIALOG_POSITION, const wxSize& size = SYMBOL_WXDIALOG_SIZE, long style = SYMBOL_WXDIALOG_STYLE );

    /// Destructor
    ~wxDialog();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxDialog event handler declarations
////@end wxDialog event handler declarations

////@begin wxDialog member function declarations
    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxDialog member variables
////@end wxDialog member variables
};

#endif
    // _POPUPCOMX_H_

/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     Monday, 01 September 2025 at 22:28:53
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DIALOG_H_
#define _DIALOG_H_


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
#define ID_WXDIALOG 10000
#define SYMBOL_WXDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_WXDIALOG_TITLE _("Dialog")
#define SYMBOL_WXDIALOG_IDNAME ID_WXDIALOG
#define SYMBOL_WXDIALOG_SIZE wxSize(400, 300)
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
    // _DIALOG_H_

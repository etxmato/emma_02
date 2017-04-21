#ifndef COLOURDLG_H
#define COLOURDLG_H

#include "wx/clrpicker.h"

class ColourDialog : public wxDialog
{
public:

    ColourDialog(wxWindow* parent);
	~ColourDialog();

private:
    void onSaveButton( wxCommandEvent &event );
    void onDefault1( wxCommandEvent &event );
    void onDefault2( wxCommandEvent &event );
    void onMonitor1( wxCommandEvent &event );
    void onMonitor2( wxCommandEvent &event );
    void onGray( wxCommandEvent &event );
    void onCol( wxCommandEvent &event );
    void onTest( wxCommandEvent &event );
    void onColourChanged( wxColourPickerEvent &event );
    void onBorder( wxSpinEvent &event );
    void onBorderX1Up( wxSpinEvent &event );
    void onBorderX1Down( wxSpinEvent &event );
    void onBorderX1TextEnter( wxCommandEvent &event );
    void onBorderText( wxCommandEvent &event );

    void onScaleXTextEnter( wxCommandEvent &event );

	wxString computerTypeStr_;
	int computerType_;
	ScreenInfo screenInfo_;
	bool colourChanged_;

    DECLARE_EVENT_TABLE()

};

#endif  // COLOURDLG_H

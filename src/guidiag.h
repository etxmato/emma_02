#ifndef DIAGDLG_H
#define DIAGDLG_H

class DiagDialog : public wxDialog
{
public:
    DiagDialog (wxWindow* parent);
    ~DiagDialog (){};

private:
    void onSaveButton( wxCommandEvent &event);
    void onDiagPal0Button(wxCommandEvent &event);
    void onDiagPal1Button(wxCommandEvent &event);
    void DiagPalButton(int number, wxString textMessage);
    void onDiagNtsc0Button(wxCommandEvent &event);
    void onDiagNtsc1Button(wxCommandEvent &event);
    void DiagNtscButton(int number, wxString textMessage);

    wxString DiagPalRomDir_[2];
    wxString DiagNtscRomDir_[2];

    DECLARE_EVENT_TABLE()

};

#endif  // DIAGDLG_H

#ifndef DIAGDLG_H
#define DIAGDLG_H

class DiagDialog : public wxDialog
{
public:
    DiagDialog (wxWindow* parent);
    ~DiagDialog (){};

private:
    void onSaveButton( wxCommandEvent &event);
	void onDiag0Button(wxCommandEvent &event);
	void onDiag1Button(wxCommandEvent &event);
	void DiagButton(int number, wxString textMessage);

	wxString DiagRomDir_[2];

    DECLARE_EVENT_TABLE()

};

#endif  // DIAGDLG_H

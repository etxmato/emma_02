#ifndef EPROMDLG_H
#define EPROMDLG_H

class EpromDialog : public wxDialog
{
public:

    EpromDialog (wxWindow* parent);
    ~EpromDialog (){};

private:
    void onSaveButton( wxCommandEvent &event );
    void onEprom0Button( wxCommandEvent &event );
    void onEprom1Button( wxCommandEvent &event );
    void onEprom2Button( wxCommandEvent &event );
    void onEprom3Button( wxCommandEvent &event );
    void onEprom4Button( wxCommandEvent &event );
    void epromButton( int number );


    wxString epromRomDir_[5];

    DECLARE_EVENT_TABLE()

};

#endif  // EPROMDLG_H

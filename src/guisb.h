#ifndef SBDLG_H
#define SBDLG_H

class SBDialog : public wxDialog
{
public:

    SBDialog (wxWindow* parent);
    ~SBDialog (){};

private:
	// Rom spec
    void onSaveButton( wxCommandEvent &event );
    void onBMChoice( wxCommandEvent &event );
    void onBMStore( wxCommandEvent &event );
    void onAliasChoice( wxCommandEvent &event );
    void onAliasStore( wxCommandEvent &event );
    void onAliasDel( wxCommandEvent &event );
    void onSB0Button( wxCommandEvent &event );
    void onSB1Button( wxCommandEvent &event );
    void onSB2Button( wxCommandEvent &event );
    void onSB3Button( wxCommandEvent &event );
    void onSB4Button( wxCommandEvent &event );
    void onSB5Button( wxCommandEvent &event );
    void onSB6Button( wxCommandEvent &event );
    void onSB7Button( wxCommandEvent &event );
    void onSB8Button( wxCommandEvent &event );
    void onSB9Button( wxCommandEvent &event );
    void onSB10Button( wxCommandEvent &event );
    void SBButton( int number , wxString textMessage);
    void onOnlineBMChange( wxCommandEvent &event );
    void onOnlineAliasChange( wxCommandEvent &event );

    wxString SBRomDir_[12];

	// USB Settings spec
	void onRoot(wxCommandEvent &event);

	// Backup specs
    void onYesButton( wxCommandEvent &event );
	void onNoButton(wxCommandEvent &event);

	void listBackup(wxFileName dir, wxString name, wxString ext, int flags);
	void listAllBackup(wxString dir);
	void onListBackup();
	void onSub(wxCommandEvent& event);
	void onFolder(wxCommandEvent& event);

	bool first_;
	bool sub_;

	wxString urlBookMark_[10];
	int bmNumber_;
	int aliasNumber_;
	wxString emailAddress_;

	wxString rootDir_;

    DECLARE_EVENT_TABLE()

};

#endif  // SBDLG_H

#ifndef DATADIRDLG_H
#define DATADIRDLG_H

class DatadirDialog : public wxDialog
{
public:
    DatadirDialog(wxWindow* parent);
    ~DatadirDialog(){};

private:
    void onMoveButton( wxCommandEvent &event );
    void onSetButton( wxCommandEvent &event );
	bool copyTree( wxFileName* source, wxFileName* destination );

    DECLARE_EVENT_TABLE()

};

#endif  // DATADIRDLG_H

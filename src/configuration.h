#ifndef CONFIGURATIONDLG_H
#define CONFIGURATIONDLG_H

class ConfigurationDialog : public wxDialog
{
public:
    ConfigurationDialog(wxWindow* parent, ConfigurationInfo configurationInfo, vector<ConfigurationSubMenuInfo> configurationSubMenuInfo, size_t configurationSubMenuInfoNumber);
    ~ConfigurationDialog(){};

private:
    void onSaveButton( wxCommandEvent &event );
    void onMenuName( wxCommandEvent &event);
    void onSubMenuName( wxCommandEvent &event);

    wxString fileNameMenuPart_;
    wxString fileNameSubMenuPart_;
    wxString fileName_;
    ConfigurationInfo oldConfigurationInfo_;

    DECLARE_EVENT_TABLE()

};

#endif  // CONFIGURATIONDLG_H

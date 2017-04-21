#ifndef DEVICEDLG_H
#define DEVICEDLG_H

class DevicePortsDialog : public wxDialog
{
public:
    DevicePortsDialog(wxWindow* parent);
    ~DevicePortsDialog() {};

private:
    void onSaveButton(wxCommandEvent &event);
	void reset6847Item(int dontCheck, int num);
	void onMC6847DD6(wxCommandEvent& event);
	void onMC6847DD7(wxCommandEvent& event);
	void onMC6847B7(wxCommandEvent& event);
	void onMC6847B6(wxCommandEvent& event);
	void onMC6847B5(wxCommandEvent& event);
	void onMC6847B4(wxCommandEvent& event);
	void onMC6847B3(wxCommandEvent& event);
	void onMC6847B2(wxCommandEvent& event);
	void onMC6847B1(wxCommandEvent& event);
	void onMC6847B0(wxCommandEvent& event);

	wxString elfTypeStr_;
    ElfConfiguration elfConfiguration;
    
    DECLARE_EVENT_TABLE()

};

#endif  // DEVICEDLG_H

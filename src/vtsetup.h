#ifndef VTSETUPDLG_H
#define VTSETUPDLG_H

class VtSetupDialog : public wxDialog
{
public:
    VtSetupDialog(wxWindow* parent);
    ~VtSetupDialog() {};

private:
	void onSaveButton(wxCommandEvent &event);
	void onVtWavFile(wxCommandEvent &event);
	void onVtWavFileButton(wxCommandEvent &event);
	void onVtWavFileEject(wxCommandEvent &event);
	void onVtCharRom(wxCommandEvent& event);
	void onVtCharRomText(wxCommandEvent& event);
    void listPorts();
    void onUart1854(wxCommandEvent&event);
    void onUart16450(wxCommandEvent&event);

 	wxString computerTypeStr_;
	int computerType_;
	bitset<32> SetUpFeature_;
	ElfConfiguration elfConfiguration_;

    bool originalUartValue_;
    
	DECLARE_EVENT_TABLE()

};

#endif  // VTSETUPDLG_H

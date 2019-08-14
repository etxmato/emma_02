#ifndef MICROSETUPDLG_H
#define MICROSETUPDLG_H

class MicroOneSocketSetupDialog : public wxDialog
{
public:
    MicroOneSocketSetupDialog(wxWindow* parent);
    ~MicroOneSocketSetupDialog() {};

private:
	void onSaveButton(wxCommandEvent &event);
    
    int ramSize_;
	DECLARE_EVENT_TABLE()

};

class MicroFourSocketSetupDialog : public wxDialog
{
public:
    MicroFourSocketSetupDialog(wxWindow* parent);
    ~MicroFourSocketSetupDialog() {};
    
private:
    void onSaveButton(wxCommandEvent &event);
    void onLocation(wxCommandEvent& event);

    int ramSize_;
    DECLARE_EVENT_TABLE()
    
};

#endif  // MICROSETUPDLG_H

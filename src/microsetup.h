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

class MicroRomSocketSetupDialog : public wxDialog
{
public:
    MicroRomSocketSetupDialog(wxWindow* parent);
    ~MicroRomSocketSetupDialog() {};
    
private:
    void onSaveButton(wxCommandEvent &event);
    void onLocationXu25(wxCommandEvent& event);
    void onLocationXu27(wxCommandEvent& event);
    void onLocationXu(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
    
};

class MicroRom603ASocketSetupDialog : public wxDialog
{
public:
    MicroRom603ASocketSetupDialog(wxWindow* parent);
    ~MicroRom603ASocketSetupDialog() {};
    
private:
    void onSaveButton(wxCommandEvent &event);
    void onLocationXu25(wxCommandEvent& event);
    void onLocationXu27(wxCommandEvent& event);
    void onLocationXu(wxCommandEvent& event);
    
    DECLARE_EVENT_TABLE()
    
};

#endif  // MICROSETUPDLG_H

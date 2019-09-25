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

class MicroboardSetupDialog : public wxDialog
{
public:
    MicroboardSetupDialog(wxWindow* parent, Conf configuration);
    ~MicroboardSetupDialog() {};
    
    void setConfiguration(Conf configuration) { configuration_ = configuration;};

private:
    void onSaveButton(wxCommandEvent &event);
    void onRomU21(wxCommandEvent& event);
    void onRomU21Text(wxCommandEvent& event);
    void onRomU20(wxCommandEvent& event);
    void onRomU20Text(wxCommandEvent& event);
    void onRomU19(wxCommandEvent& event);
    void onRomU19Text(wxCommandEvent& event);
    void onRomU18(wxCommandEvent& event);
    void onRomU18Text(wxCommandEvent& event);
    void onRomU17(wxCommandEvent& event);
    void onRomU17Text(wxCommandEvent& event);

    void setOneSocketState();
    void setFourSocketState();
    void setRamlabel(int romNumber, wxString romString);

    void onFour604BSocketBank(wxCommandEvent&event);
    int convert604BChipType(int type);
    void on604BRamBlock(wxCommandEvent&event);
    void on604BRomBlock(wxCommandEvent&event);
    void one604BSocketBankGui();
    void four604BSocketBankGui();

    void onOneSocketBank(wxCommandEvent&event);
    void onFourSocketBank(wxCommandEvent&event);

    void onOne600SocketSetup(wxCommandEvent& event);
    void onFour600SocketSetup(wxCommandEvent& event);
    void onFour601SocketSetup(wxCommandEvent& event);

    Conf configuration_;
    int selectedComputer_;
    wxString selectedComputerName_;
    wxString selectedComputerStr_;

    DECLARE_EVENT_TABLE()
    
};

class One600SocketSetupDialog : public wxDialog
{
public:
    One600SocketSetupDialog(MicroboardSetupDialog* parent, Conf configuration);
    ~One600SocketSetupDialog() {};
    
private:
    void onSaveButton(wxCommandEvent &event);
    
    int ramSize_;
    Conf configuration_;
    MicroboardSetupDialog* parent_;
    
    DECLARE_EVENT_TABLE()
    
};

class Four600SocketSetupDialog : public wxDialog
{
public:
    Four600SocketSetupDialog(MicroboardSetupDialog* parent, Conf configuration);
    ~Four600SocketSetupDialog() {};
    
private:
    void onSaveButton(wxCommandEvent &event);
    void onLocation(wxCommandEvent& event);
    
    int ramSize_;
    Conf configuration_;
    MicroboardSetupDialog* parent_;

    DECLARE_EVENT_TABLE()
    
};

class Four601SocketSetupDialog : public wxDialog
{
public:
    Four601SocketSetupDialog(MicroboardSetupDialog* parent, Conf configuration);
    ~Four601SocketSetupDialog() {};
    
private:
    void onSaveButton(wxCommandEvent &event);
    void onLocationXu25(wxCommandEvent& event);
    void onLocationXu27(wxCommandEvent& event);
    void onLocationXu(wxCommandEvent& event);
    
    Conf configuration_;
    MicroboardSetupDialog* parent_;

    DECLARE_EVENT_TABLE()
    
};

class MicroboardCardSetupDialog : public wxDialog
{
public:
    MicroboardCardSetupDialog(wxWindow* parent, Conf configuration, ElfConfiguration elfConfigurationMicro, MicroMemoryConf microMemConfigutation, int cardNumber);
    ~MicroboardCardSetupDialog() {};
    
private:
    void onSaveButton(wxCommandEvent &event);
    void onFdcIoGroupMicroboard(wxCommandEvent&event);
    void onSocketBankMicroboard(wxCommandEvent&event);
    void setInhibit();
    void onLowHighMicroboard(wxCommandEvent&event);

    void onRamSec1(wxCommandEvent& event);
    void onRamSec1Text(wxCommandEvent& event);

    void onRomSec1(wxCommandEvent& event);
    void onRomSec1Text(wxCommandEvent& event);
    void onRomSec2(wxCommandEvent& event);
    void onRomSec2Text(wxCommandEvent& event);
    void onRomSec3(wxCommandEvent& event);
    void onRomSec3Text(wxCommandEvent& event);
    void onRomSec4(wxCommandEvent& event);
    void onRomSec4Text(wxCommandEvent& event);

    void onRamBlock(wxCommandEvent&event);
    void ramSocketBankGui();
    void onRom1Block(wxCommandEvent&event);
    void rom1SocketBankGui();
    void onRom2Block(wxCommandEvent&event);
    void rom2SocketBankGui();

    Conf configuration_;
    ElfConfiguration elfConfiguration_;
    MicroMemoryConf microMemConfigutation_;
    int cardNumber_;
    int selectedComputer_;
    wxString selectedComputerName_;
    wxString selectedComputerStr_;
    
    DECLARE_EVENT_TABLE()
    
};

#endif  // MICROSETUPDLG_H

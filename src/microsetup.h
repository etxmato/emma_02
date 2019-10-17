#ifndef MICROSETUPDLG_H
#define MICROSETUPDLG_H

class MicroboardSetupDialog : public wxDialog
{
public:
    MicroboardSetupDialog(wxWindow* parent, Conf configuration, ElfConfiguration elfConfiguration);
    ~MicroboardSetupDialog() {};
    
    void setConfiguration(Conf configuration) { configuration_ = configuration;};

private:
    void onSaveButton(wxCommandEvent &event);
    void onFourSocketBankDisable(wxCommandEvent&event);
    
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

    void onFour602SocketBank(wxCommandEvent&event);
    void on602RamBlock(wxCommandEvent&event);
    void on602RomBlock(wxCommandEvent&event);
    void one602SocketBankGui();
    void four602SocketBankGui();

    void onOneSocketBank(wxCommandEvent&event);
    void onFourSocketBank(wxCommandEvent&event);
    void onUartGroup(wxCommandEvent&event);

    void onOne600SocketSetup(wxCommandEvent& event);
    void onFour600SocketSetup(wxCommandEvent& event);
    void onFour601SocketSetup(wxCommandEvent& event);

    Conf configuration_;
    ElfConfiguration elfConfiguration_;
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
  
    void setConfiguration(MicroMemoryConf microMemConfigutation) { microMemConfigutation_ = microMemConfigutation;};

private:
    void onSaveButton(wxCommandEvent &event);
    void onFdcIoGroupMicroboard(wxCommandEvent&event);
    void onUartIoGroupMicroboard(wxCommandEvent&event);
    void onV1870IoGroupMicroboard(wxCommandEvent&event);
    void onPio1IoGroupMicroboard(wxCommandEvent&event);
    void onPio2IoGroupMicroboard(wxCommandEvent&event);
    void onSocket626BankMicroboard(wxCommandEvent&event);
    void setInhibit();
    void onLowHighMicroboard(wxCommandEvent&event);
    void onVideoMode(wxCommandEvent&event);
    void setVideoMode();

    void onRamSec1(wxCommandEvent& event);
    void onRamSec1Text(wxCommandEvent& event);
    void onCharacterRom(wxCommandEvent& event);
    void onCharacterRomText(wxCommandEvent& event);

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

    void on621Location0(wxCommandEvent& event);
    void on621Location1(wxCommandEvent& event);
    void setMemoryInhibit621(int bank);
    
    void onRom1Block625(wxCommandEvent&event);
    void rom1SocketBankGui625();
    void onRom2Block625(wxCommandEvent&event);
    void rom2SocketBankGui625();
    void onSocket1BankMicroboard(wxCommandEvent&event);
    void onSocket2BankMicroboard(wxCommandEvent&event);
    void setMemoryInhibit625(int bank);
    void onVolume(wxScrollEvent&event);
    void onPrintMode(wxCommandEvent&event);
    void setPrintMode();
    void onPrintFile(wxCommandEvent&event);
    void onPrintFileText(wxCommandEvent&event);
    void onMicroRom660SocketSetup(wxCommandEvent&event);
    void onSocket660BankMicroboard(wxCommandEvent&event);

    Conf configuration_;
    ElfConfiguration elfConfiguration_;
    MicroMemoryConf microMemConfigutation_;
    int cardNumber_;
    int selectedComputer_;
    int runningComputer_;
    wxString selectedComputerName_;
    wxString selectedComputerStr_;
    
    wxString romText_[4];

    DECLARE_EVENT_TABLE()
    
};

class MicroboardAdditionalCardSetupDialog : public wxDialog
{
public:
    MicroboardAdditionalCardSetupDialog(wxWindow* parent, Conf configuration, ElfConfiguration elfConfigurationMicro);
    ~MicroboardAdditionalCardSetupDialog() {};
    
private:
    void onSaveButton(wxCommandEvent &event);

    void onMicroboardCardSetup(wxCommandEvent& event);
    void onMicroboardType(wxCommandEvent&event);

    void checkAllBoardTypes();
    wxString checkBoardType(int card, wxString cardstring, wxString oldStr, bool boardControlValue);
    void setChoiceColor(wxString cardstring, bool error);
    void microboardCardSetup(int card);
    
    Conf configuration_;
    ElfConfiguration elfConfiguration_;
    int selectedComputer_;
    int runningComputer_;
    wxString selectedComputerName_;
    wxString selectedComputerStr_;

    DECLARE_EVENT_TABLE()
    
};

class MicroRom660SocketSetupDialog : public wxDialog
{
public:
    MicroRom660SocketSetupDialog(MicroboardCardSetupDialog* parent, MicroMemoryConf microMemConfigutation);
    ~MicroRom660SocketSetupDialog() {};
    
private:
    void onSaveButton(wxCommandEvent &event);
    void onLocationXu21(wxCommandEvent& event);
    void onLocationXu23(wxCommandEvent& event);
    void onLocationXu(wxCommandEvent& event);

    MicroMemoryConf microMemConfigutation_;
    MicroboardCardSetupDialog* parent_;

    DECLARE_EVENT_TABLE()
};

#endif  // MICROSETUPDLG_H

#ifndef POPUPDLG_H
#define POPUPDLG_H

class PopupDialog : public wxDialog
{
public:

    PopupDialog (wxWindow* parent);
    ~PopupDialog ();

    void init();

    void setLocation(bool state, wxString saveStart, wxString saveEnd, wxString saveExec);
    void setLocation(bool state);
    void setStartLocation(wxString saveStart);
    void setEndLocation(wxString saveEnd);
    void setTape1Type(bool useTape);

private:
    void onOk(wxCommandEvent&event);

    void onXmlControlWindows(wxCommandEvent& event);

    void onCassette(wxCommandEvent& event);
    void onCassetteText(wxCommandEvent& event);
    void onCassetteEject(wxCommandEvent& event);
    void onCassette1(wxCommandEvent& event);
    void onCassetteText1(wxCommandEvent& event);
    void onCassetteEject1(wxCommandEvent& event);

    void onXmodem(wxCommandEvent& event);
    void onXmodemEject(wxCommandEvent& event);
    void onXmodemText(wxCommandEvent& event);

    void onLoadRunButton(wxCommandEvent& event);
    void onLoadButton(wxCommandEvent& event);
    void onSaveButton(wxCommandEvent& event);
    void onDataSaveButton(wxCommandEvent& event);

    void onSaveStart(wxCommandEvent& event);
    void onSaveEnd(wxCommandEvent& event);
    void onSaveExec(wxCommandEvent& event);
    void onUseLocation(wxCommandEvent& event);

    void onDisk0(wxCommandEvent& event);
    void onDiskText0(wxCommandEvent& event);
    void onDiskEject0(wxCommandEvent& event);
    void onDiskDirSwitch0(wxCommandEvent& event);
    void onDisk1(wxCommandEvent& event);
    void onDiskText1(wxCommandEvent& event);
    void onDiskEject1(wxCommandEvent& event);
    void onDiskDirSwitch1(wxCommandEvent& event);
    void onDisk2(wxCommandEvent& event);
    void onDiskText2(wxCommandEvent& event);
    void onDiskEject2(wxCommandEvent& event);
    void onDiskDirSwitch2(wxCommandEvent& event);
    void onDisk3(wxCommandEvent& event);
    void onDiskText3(wxCommandEvent& event);
    void onDiskEject3(wxCommandEvent& event);
    void onDiskDirSwitch3(wxCommandEvent& event);

    ComputerConfiguration currentComputerConfiguration;

    wxString getFdcName(int drive);
    void setDirSwitch(int drive);
    void setUpdFloppyGui(int drive);

    bool loadingGui_;

    DECLARE_EVENT_TABLE()

};

#endif  // POPUPDLG_H

#ifndef POPUPDLG_H
#define POPUPDLG_H

class PopupDialog : public wxDialog
{
public:

    PopupDialog (wxWindow* parent);
    ~PopupDialog ();

	void init();

	void setLocation(bool state, wxString saveStart, wxString saveEnd, wxString saveExec);
	void setStartLocation(wxString saveStart);
	void setEndLocation(wxString saveEnd);
	void setTapeType(bool useTape);
	void enableMemAccessGui(bool status);
	void enableIoGui();

private:
	void onOk(wxCommandEvent&event);
	void onFocus(wxActivateEvent&event);

	void onComxDisk1(wxCommandEvent&event);
	void onComxDiskText1(wxCommandEvent&event);
	void onComxDiskEject1(wxCommandEvent&event);
	void onComxDisk2(wxCommandEvent&event);
	void onComxDiskText2(wxCommandEvent&event);
	void onComxDiskEject2(wxCommandEvent&event);

	void onElf2KControlWindows(wxCommandEvent& event);
	void onElf2KSwitch(wxCommandEvent& event);
	void onElf2KHex(wxCommandEvent& event);

	void onCosmicosControlWindows(wxCommandEvent& event);
	void onCosmicosHex(wxCommandEvent& event);

    void onElfControlWindows(wxCommandEvent& event);
    void onVelfControlWindows(wxCommandEvent& event);

	void onMembershipControlWindows(wxCommandEvent& event);

	void onTelmacAdsChannel(wxSpinEvent&event);
	void onTelmacAdsVolt(wxSpinEvent&event);
	void onTelmacAdsChannelText(wxCommandEvent& event);
	void onTelmacAdsVoltText(wxCommandEvent& event);
	void onTelmacAdiChannel(wxSpinEvent&event);
	void onTelmacAdiVolt(wxSpinEvent&event);
	void onTelmacAdiChannelText(wxCommandEvent& event);
	void onTelmacAdiVoltText(wxCommandEvent& event);

	void onCassette(wxCommandEvent& event);
	void onCassetteText(wxCommandEvent& event);
	void onCassetteEject(wxCommandEvent& event);

	void onLoadRunButton(wxCommandEvent& event);
	void onLoadButton(wxCommandEvent& event);
	void onSaveButton(wxCommandEvent& event);
	void onDataSaveButton(wxCommandEvent& event);

	void onSaveStart(wxCommandEvent& event);
	void onSaveEnd(wxCommandEvent& event);
	void onSaveExec(wxCommandEvent& event);
	void onUseLocation(wxCommandEvent& event);

	void onMs2000Disk0(wxCommandEvent& event);
	void onMs2000DiskText0(wxCommandEvent& event);
	void onMs2000DiskEject0(wxCommandEvent& event);
	void onMS2000DiskDirSwitch0(wxCommandEvent& event);
	void onMs2000Disk1(wxCommandEvent& event);
	void onMs2000DiskText1(wxCommandEvent& event);
	void onMs2000DiskEject1(wxCommandEvent& event);
	void onMS2000DiskDirSwitch1(wxCommandEvent& event);
	void onMs2000Disk2(wxCommandEvent& event);
	void onMs2000DiskText2(wxCommandEvent& event);
	void onMs2000DiskEject2(wxCommandEvent& event);
	void onMS2000DiskDirSwitch2(wxCommandEvent& event);
	void onMs2000Disk3(wxCommandEvent& event);
	void onMs2000DiskText3(wxCommandEvent& event);
	void onMs2000DiskEject3(wxCommandEvent& event);
	void onMS2000DiskDirSwitch3(wxCommandEvent& event);
    
    wxString getFdcName(int drive);
	void setDirSwitch(int drive);
    
	wxString computerStr_;
	int computer_;

	bool loadingGui_;

	wxBitmap tapeOffBitmap;
	wxBitmap tapeOnBitmap;

	DECLARE_EVENT_TABLE()

};

#endif  // POPUPDLG_H

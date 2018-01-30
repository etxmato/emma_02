#ifndef GUIMS2000_H
#define GUIMS2000_H

#include "guimcds.h"
#include "elfconfiguration.h"

class GuiMS2000: public GuiMcds
{
public:

	GuiMS2000(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiMS2000() {};

	void readMS2000Config();
    void writeMS2000DirConfig();
	void writeMS2000Config();
    void readMS2000WindowConfig();
    void writeMS2000WindowConfig();

	void onMS2000BaudR(wxCommandEvent& event);
	void onMS2000BaudT(wxCommandEvent& event);
	void onMS2000Uart(wxCommandEvent& event);
	void onMS2000Clock(wxCommandEvent& event);
    void onMS2000ForceUpperCase(wxCommandEvent& event);
    void onBootRam(wxCommandEvent&event);
    
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
    bool getDirectoryMode(int drive);
    void setDirectoryMode(int drive, bool state);
    wxString getMs2000FloppyDirSwitched(int drive);
    wxString getMs2000FloppyDir(int drive);
    wxString getMs2000FloppyFile(int drive);

protected:

private:
    wxPoint position_;
    wxString floppyDirSwitchedMs2000_[4];
    wxString floppyDirMs2000_[4];
    wxString floppyMs2000_[4];
    
    bool directoryMode_[4];
    
	DECLARE_EVENT_TABLE()
};

#endif // GUIMS2000_H

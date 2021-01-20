#ifndef GUICIDELSA_H
#define GUICIDELSA_H

#include "guitmc600.h"

class GuiCidelsa : public GuiTelmac
{
public:

	GuiCidelsa(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~GuiCidelsa() {};

	void readCidelsaConfig();
    void writeCidelsaDirConfig();
    void writeCidelsaConfig();
    void readCidelsaWindowConfig();
    void writeCidelsaWindowConfig();

	void onCidelsaDifficulty(wxCommandEvent&event);
	void onCidelsaBonus(wxCommandEvent&event);
	void onCidelsaLives(wxCommandEvent&event);
	void onCidelsaCoin(wxCommandEvent&event);
	void onCidelsaDifficultyDraco(wxCommandEvent&event);
	void onCidelsaBonusDraco(wxCommandEvent&event);
	void onCidelsaLivesDraco(wxCommandEvent&event);
	void onCidelsaCoinDraco(wxCommandEvent&event);
	void onCidelsaDracoDouble(wxCommandEvent&event);
    void onCidelsaHw(wxCommandEvent&event);

	Byte getIn2Value() {return in2Value_;};
	Byte getIn2ValueDraco() {return in2ValueDraco_;};
    Byte getCidelsaHwConfiguration() {return hwConfiguration_;};

protected:
	Byte in2Value_;
	Byte in2ValueDraco_;
    Byte hwConfiguration_;

private:
	
	DECLARE_EVENT_TABLE()
};

#endif // GUICIDELSA_H

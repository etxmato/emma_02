#ifndef GUIVIP_H
#define GUIVIP_H

#include "guivip2.h"

class GuiVip: public GuiVipII
{
public:

	GuiVip(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode_, wxString dataDir);
	~GuiVip() {};

	void readVipConfig();
    void writeVipDirConfig();
    void writeVipConfig();
    void readVipWindowConfig();
    void writeVipWindowConfig();

	void onTempo(wxScrollEvent&event);
	void onSound(wxCommandEvent&event);
	void onRamSWVip(wxCommandEvent&event);
	void onRamSWText(wxCommandEvent&event);
	void onVipRam(wxSpinEvent& event);
	void onVipVp570(wxSpinEvent& event);
	void onVipVp580(wxCommandEvent& event);
	void onVipVp590(wxCommandEvent& event);
	void onVipHighRes(wxCommandEvent&event);
	void onVipStereo(wxCommandEvent&event);
	void onLatch(wxCommandEvent&event);

	void setSoundGui(int sound);
	int getVipVp570() {return vipVp570_;};
	bool getVipVp580() {return vipVp580_;};
	bool getVipVp590() {return vipVp590_;}; 
	int getVipStereo() {return stereo_;};
	int getLatch() {return latch_;};
	int getVipHighRes() {return highRes_;};

	void onVipBaudR(wxCommandEvent& event);
	void onVipBaudT(wxCommandEvent& event);
	void setBaudChoiceVip();

private:
    wxPoint position_;

    int vipVp570_;
	bool vipVp580_;
	bool vipVp590_;
	bool highRes_;
	bool stereo_;
	bool latch_;

	DECLARE_EVENT_TABLE()
};

#endif // GUIVIP_H

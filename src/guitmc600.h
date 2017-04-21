#ifndef GUITELMAC_H
#define GUITELMAC_H

#include "guipecom.h"

class GuiTelmac : public GuiPecom
{
public:

	GuiTelmac(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir);
	~GuiTelmac() {};

	void readTelmacConfig();
    void writeTelmacDirConfig();
    void writeTelmacConfig();
    void readTelmacWindowConfig();
    void writeTelmacWindowConfig();

	void onTelmacExpRom(wxCommandEvent& event);
	void onTelmacExpRomText(wxCommandEvent& event);
	void onRealTimeClockTmc600(wxCommandEvent& event);
	void onTelmacAdsChannel(wxSpinEvent&event);
	void onTelmacAdsVolt(wxSpinEvent&event);
	void onTelmacAdsChannelText(wxCommandEvent& event);
	void onTelmacAdsVoltText(wxCommandEvent& event);
	void onTelmacAdiChannel(wxSpinEvent&event);
	void onTelmacAdiVolt(wxSpinEvent&event);
	void onTelmacAdiChannelText(wxCommandEvent& event);
	void onTelmacAdiVoltText(wxCommandEvent& event);
	void enableIoGui();
    void onKeyDef(wxCommandEvent& event);

	bool getRealTimeClock() {return realTimeClock_;};

private:
	bool realTimeClock_;

	DECLARE_EVENT_TABLE()
};

#endif // GUITELMAC_H

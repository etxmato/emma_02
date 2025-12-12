#ifndef COIN_VIDEO_CONFIG_H
#define COIN_VIDEO_CONFIG_H

#include "sn76430n_config.h"

class CoinVideoConfig: public Sn76430NConfig
{
public:

    CoinVideoConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~CoinVideoConfig();

    void coinVideoConfigInit();
    void updateCoinVideoPanel();
    int setCoinVideoRegister(int registerNumber, bool value, int showTrace = SHOW_ADDRESS_TRACE);

protected:
    void parseXml_CoinVideo(wxXmlNode &node);

private:
    void CoinVideoEnable(wxCommandEvent&event);
    
    bool coinConfigRegisterValue[COIN_VIDEO_NUMBER_OF_REGISTERS];
    bool lastCoinConfigRegisterValue[COIN_VIDEO_NUMBER_OF_REGISTERS];

    DECLARE_EVENT_TABLE()
};

#endif // COIN_VIDEO_CONFIG_H

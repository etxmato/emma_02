#ifndef PIXIE_CONFIG_H
#define PIXIE_CONFIG_H

#include "coin_video_config.h"

class PixieConfig: public CoinVideoConfig
{
public:

    PixieConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~PixieConfig();

    void pixieConfigInit();
    void updatePixiePanel();
    int setPixieRegister(int registerNumber, bool value, int showTrace = SHOW_ADDRESS_TRACE);

protected:
    void parseXml_PixieVideo(wxXmlNode &node);

private:
    void PixieEnable(wxCommandEvent&event);
    void PixieDisable(wxCommandEvent&event);

    bool pixieConfigRegisterValue[PIXIE_NUMBER_OF_REGISTERS];
    bool lastPixieConfigRegisterValue[PIXIE_NUMBER_OF_REGISTERS];

    DECLARE_EVENT_TABLE()
};

#endif // PIXIE_CONFIG_H

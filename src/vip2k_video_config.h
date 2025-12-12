#ifndef VIP2K_VIDEO_CONFIG_H
#define VIP2K_VIDEO_CONFIG_H

#include "studioiv_video_config.h"

class Vip2KVideoConfig: public St4VideoConfig
{
public:

    Vip2KVideoConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Vip2KVideoConfig();

    void vip2KVideoConfigInit();
    void updateVip2KVideoPanel();
    int setVip2KVideoRegister(int registerNumber, bool value, int showTrace = SHOW_ADDRESS_TRACE);

protected:
    void parseXml_Vip2KVideo(wxXmlNode &node);

private:
    void Vip2KVideoEnable(wxCommandEvent&event);
    void Vip2KVideoDisable(wxCommandEvent&event);

    bool vip2KConfigRegisterValue[VIP2K_VIDEO_NUMBER_OF_REGISTERS];
    bool lastVip2KConfigRegisterValue[VIP2K_VIDEO_NUMBER_OF_REGISTERS];

    DECLARE_EVENT_TABLE()
};

#endif // VIP2K_VIDEO_CONFIG_H

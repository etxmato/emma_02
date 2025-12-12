#ifndef SN76430N_CONFIG_H
#define SN76430N_CONFIG_H

#include "cdp1862_config.h"

class Sn76430NConfig: public Cdp1862Config
{
public:

    Sn76430NConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Sn76430NConfig();

    void sn76430NConfigInit();
    void updateSn76430NPanel();

protected:
    void parseXml_SN76430NVideo(wxXmlNode &node);

private:
    
    DECLARE_EVENT_TABLE()
};

#endif // SN76430N

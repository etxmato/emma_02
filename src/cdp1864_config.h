#ifndef CDP1864_CONFIG_H
#define CDP1864_CONFIG_H

#include "fred_video_config.h"

class Cdp1864Config: public FredVideoConfig
{
public:

    Cdp1864Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Cdp1864Config();

    void cdp1864ConfigInit();
    void updateCdp1864Panel();
    int setCdp1864Register(int registerNumber, bool value, int showTrace = SHOW_ADDRESS_TRACE);
    int setCdp1864Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setCdp1864RegisterNible(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setCdp1864RegisterNible(int registerNumber, Word address, Byte value, int showTrace = SHOW_ADDRESS_TRACE);

protected:
    void parseXml_Cdp1864Video(wxXmlNode &node);

private:
    void Cdp1864Enable(wxCommandEvent&event);
    void Cdp1864Disable(wxCommandEvent&event);
    void Cdp1864Background(wxCommandEvent&event);
    void Cdp1864ColorToneLatch(wxCommandEvent&event);

    bool cdp1864ConfigRegisterValue[CDP1864_NUMBER_OF_RADIOBUTTONS];
    bool lastCdp1864ConfigRegisterValue[CDP1864_NUMBER_OF_RADIOBUTTONS];

    wxString cdp1864ConfigRegisterValueString[CDP1864_NUMBER_OF_REGISTERS];
    wxString lastCdp1864ConfigRegisterValueString[CDP1864_NUMBER_OF_REGISTERS];

    DECLARE_EVENT_TABLE()
};

#endif // CDP1864

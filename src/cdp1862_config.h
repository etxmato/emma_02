#ifndef CDP1862_CONFIG_H
#define CDP1862_CONFIG_H

#include "cdp1864_config.h"

class Cdp1862Config: public Cdp1864Config
{
public:

    Cdp1862Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Cdp1862Config();

    void cdp1862ConfigInit();
    void updateCdp1862Panel();
    int setCdp1862Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setCdp1862RegisterNible(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setCdp1862RegisterNible(int registerNumber, Word address, Byte value, int showTrace = SHOW_ADDRESS_TRACE);

protected:
    void parseXml_Cdp1862Video(wxXmlNode &node);

private:
    void Cdp1862Background(wxCommandEvent&event);
    
    wxString cdp1862ConfigRegisterValueString[CDP1862_NUMBER_OF_REGISTERS];
    wxString lastCdp1862ConfigRegisterValueString[CDP1862_NUMBER_OF_REGISTERS];

    DECLARE_EVENT_TABLE()
};

#endif // CDP1862

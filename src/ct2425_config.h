#ifndef CT2425_CONFIG_H
#define CT2425_CONFIG_H

#include "other_config.h"

class Ct2425Config: public OtherConfig
{
public:

    Ct2425Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Ct2425Config();

    void ct2425ConfigInit();
    void updateCt2425Panel();
    int setCt2425Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setCt2425SelectorValue(int selectorNumber, int selectorValue, int showTrace = SHOW_ADDRESS_TRACE);
    int setCt2425SelectorValue(int selectorNumber, bool selectorValue, int showTrace = SHOW_ADDRESS_TRACE);
    void setCoinLogText(wxString filename);
    wxString getCoinLogText(int value);

protected:
    void parseXml_Ct2425Coins(wxXmlNode &node);
    void parseXml_Ct2425(wxXmlNode &node);

private:
    void ct2425Register(wxCommandEvent&event);

    wxString ct2425ConfigRegisterValueString[CT2425_NUMBER_OF_REGISTERS];
    wxString lastCt2425ConfigRegisterValueString[CT2425_NUMBER_OF_REGISTERS];
    
    char ct2425ConfigSelector[CT2425_NUMBER_OF_SELECTORS];
    char updateCt2425ConfigSelector[CT2425_NUMBER_OF_SELECTORS];

    bool currentInterLace_;
    
    DECLARE_EVENT_TABLE()
};

#endif // CT2425_CONFIG_H

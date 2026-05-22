#ifndef CRT2425_CONFIG_H
#define CRT2425_CONFIG_H

#include "mc6845_config.h"

class Crt2425Config: public Mc6845Config
{
public:

    Crt2425Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Crt2425Config();

    void crt2425ConfigInit();
    void updateCrt2425Panel();
    int setCrt2425Register(int registerNumber, Word value, int showTrace = SHOW_ADDRESS_TRACE);
    int readCrt2425Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int readCrt2425RegisterSetData(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setCrt2425Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setCrt2425Register(int registerNumber, bool value, int showTrace = SHOW_ADDRESS_TRACE);
    int setCrt2425RegisterNibble(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    bool isCrt2425TraceChecked(int registerNumber);

protected:
    void parseXml_CoinMechanism(wxXmlNode &node);

private:
    void Crt2425Register(wxCommandEvent&event);

    wxString crt2425ConfigRegisterValueString[CRT2425_NUMBER_OF_REGISTERS];
    wxString lastCrt2425ConfigRegisterValueString[CRT2425_NUMBER_OF_REGISTERS];

    bool currentInterLace_;
    
    DECLARE_EVENT_TABLE()
};

#endif // CRT2425_CONFIG_H

#ifndef CRT8002_CONFIG_H
#define CRT8002_CONFIG_H

#include "mc6845_config.h"

class Crt8002Config: public Mc6845Config
{
public:

    Crt8002Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Crt8002Config();

    void crt8002ConfigInit();
    void updateCrt8002Panel();
    int setCrt8002Register(int registerNumber, Word value, int showTrace = SHOW_ADDRESS_TRACE);
    int readCrt8002Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int readCrt8002RegisterSetData(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setCrt8002Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setCrt8002RegisterNibble(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    bool isCrt8002TraceChecked(int registerNumber);
 //   int setCrt8002SelectorValue(int selectorNumber, int selectorValue, int showTrace = SHOW_ADDRESS_TRACE);

protected:
    void parseXml_Crt8002Video(wxXmlNode &node);

private:
    void Crt8002Register(wxCommandEvent&event);

    wxString crt8002ConfigRegisterValueString[CRT8002_NUMBER_OF_REGISTERS];
    wxString lastCrt8002ConfigRegisterValueString[CRT8002_NUMBER_OF_REGISTERS];

    bool currentInterLace_;
    
    DECLARE_EVENT_TABLE()
};

#endif // CRT8002_CONFIG_H

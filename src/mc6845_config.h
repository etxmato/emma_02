#ifndef MC6845_CONFIG_H
#define MC6845_CONFIG_H

#include "mc6847_config.h"

class Mc6845Config: public Mc6847Config
{
public:

    Mc6845Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Mc6845Config();

    void mc6845ConfigInit();
    void updateMc6845Panel();
    int setMc6845Register(int registerNumber, Word value, int showTrace = SHOW_ADDRESS_TRACE);
    int setMc6845Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setMc6845RegisterNibble(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    bool isMc6845TraceChecked(int registerNumber);

protected:
    void parseXml_MC6845Video(wxXmlNode &node);

private:
    void Mc6845Address(wxCommandEvent&event);
    void Mc6845Data(wxCommandEvent&event);
    void Mc6845RegisterByte(wxCommandEvent&event);
    void Mc6845RegisterWord(wxCommandEvent&event);

    wxString mc6845ConfigRegisterValueString[MC6845_NUMBER_OF_REGISTERS];
    wxString lastMc6845ConfigRegisterValueString[MC6845_NUMBER_OF_REGISTERS];

    bool currentInterLace_;
    
    DECLARE_EVENT_TABLE()
};

#endif // MC6845_CONFIG_H

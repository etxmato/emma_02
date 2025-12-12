#ifndef MC6847_CONFIG_H
#define MC6847_CONFIG_H

#include "pixie_config.h"

class Mc6847Config: public PixieConfig
{
public:

    Mc6847Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Mc6847Config();

    void mc6847ConfigInit();
    void updateMc6847Panel();
    void setBitValue(int bitNumber, wxString registerId, wxString function);
    void setGmValue(int bitNumber0, int bitNumber1, int bitNumber2, wxString registerId);
    int setMc6847Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setMc6847RegisterNibble(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    bool isMc6847TraceChecked(int registerNumber);
    
protected:
    void parseXml_MC6847Video(wxXmlNode &node);

private:
    void Mc6847VideoMode(wxCommandEvent&event);
    void setOutMc6847(Byte value, int invBit);
    void Mc6847Inv(wxCommandEvent&event);
    void Mc6847Ext(wxCommandEvent&event);
    void Mc6847Css(wxCommandEvent&event);
    void Mc6847As(wxCommandEvent&event);
    void Mc6847Ag(wxCommandEvent&event);
    void Mc6847Gm(wxCommandEvent&event);

    Byte mc6847ConfigRegisterValueByte[MC6847_NUMBER_OF_REGISTERS];
    wxString mc6847ConfigRegisterValueString[MC6847_NUMBER_OF_REGISTERS];
    wxString lastMc6847ConfigRegisterValueString[MC6847_NUMBER_OF_REGISTERS];

    DECLARE_EVENT_TABLE()
};

#endif // MC6847_CONFIG_H

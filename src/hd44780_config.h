#ifndef HD44780_CONFIG_H
#define HD44780_CONFIG_H

#include "mc6845_config.h"

class Hd44780Config: public Mc6845Config
{
public:

    Hd44780Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Hd44780Config();

    void hd44780ConfigInit();
    void updateHd44780Panel();
    int setHd44780Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    bool isHd44780TraceChecked(int registerNumber);

protected:
    void parseXml_HD44780Video(wxXmlNode &node);

private:
    void Hd44780Command(wxCommandEvent&event);
    void Hd44780Data(wxCommandEvent&event);

    wxString hd44780ConfigRegisterValueString[HD44780_NUMBER_OF_REGISTERS];
    wxString lastHd44780ConfigRegisterValueString[HD44780_NUMBER_OF_REGISTERS];

    DECLARE_EVENT_TABLE()
};

#endif // HD44780_CONFIG_H

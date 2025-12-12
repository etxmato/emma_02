#ifndef TMS9918_CONFIG_H
#define TMS9918_CONFIG_H

#include "i8275_config.h"

class Tms9918Config: public I8275Config
{
public:

    Tms9918Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Tms9918Config();

    void tms9918ConfigInit();
    void updateTms9918Panel();
    int setTms9918Register(int registerNumber, Word value, int showTrace = SHOW_ADDRESS_TRACE);
    int setTms9918Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setTms9918RegisterNibble(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setTms9918RegisterValue(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setTms9918DisplayMode(Byte mode, int showTrace = SHOW_ADDRESS_TRACE);
    int setTms9918SelectorValue(int selectorNumber, int selectorValue, int showTrace = SHOW_ADDRESS_TRACE);
    int setTms9918SelectorValue(int selectorNumber, bool selectorValue, int showTrace = SHOW_ADDRESS_TRACE);

protected:
    void parseXml_TMS9918Video(wxXmlNode &node);

private:
    void Tms9918Register(wxCommandEvent&event);
    void Tms9918Data(wxCommandEvent&event);
    void Tms9918RegisterNibble(wxCommandEvent&event);
    void Tms9918RegisterByte(wxCommandEvent&event);
    void Tms9918RegisterWord(wxCommandEvent&event);
    void Tms9918Status(wxCommandEvent&event);
    void Tms9918FifthSprite(wxCommandEvent&event);
    void Tms9918DisplayMode(wxCommandEvent&event);
    void Tms9918CurrentReadAddress(wxCommandEvent&event);
    void Tms9918CurrentWriteAddress(wxCommandEvent&event);

    wxString tms9918ConfigRegisterValueString[TMS9918_NUMBER_OF_REGISTERS];
    wxString lastTms9918ConfigRegisterValueString[TMS9918_NUMBER_OF_REGISTERS];

    char tms9918ConfigSelector[TMS9918_NUMBER_OF_SELECTORS];
    char updateTms9918ConfigSelector[TMS9918_NUMBER_OF_SELECTORS];
    
    DECLARE_EVENT_TABLE()
};

#endif // TMS9918_CONFIG_H

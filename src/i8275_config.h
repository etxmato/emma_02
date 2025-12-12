#ifndef I8275_CONFIG_H
#define I8275_CONFIG_H

#include "scn2672_config.h"

class I8275Config: public Scn2672Config
{
public:

    I8275Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~I8275Config();

    void i8275ConfigInit();
    void updateI8275Panel();
    int setI8275Register(int registerNumber, Word value, int showTrace = SHOW_ADDRESS_TRACE);
    int setI8275Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setI8275RegisterNibble(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setI8275RegisterValue(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setI8275SelectorValue(int selectorNumber, int selectorValue, int showTrace = SHOW_ADDRESS_TRACE);
    int setI8275SelectorValue(int selectorNumber, bool selectorValue, int showTrace = SHOW_ADDRESS_TRACE);

protected:
    void parseXml_Intel8275Video(wxXmlNode &node);

private:
    void I8275Command(wxCommandEvent&event);
    void I8275WriteParameter(wxCommandEvent&event);
    void I8275Status(wxCommandEvent&event);

    void I8275CharRow(wxCommandEvent&event);
    void I8275VerticalRetrace(wxCommandEvent&event);
    void I8275RowsFrame(wxCommandEvent&event);
    void I8275Underline(wxCommandEvent&event);
    void I8275LinesRow(wxCommandEvent&event);
    void I8275CursorFormat(wxCommandEvent&event);
    void I8275HorizontalRetrace(wxCommandEvent&event);
    void I8275BurstSpaceCode(wxCommandEvent&event);
    void I8275DmaCyclesBurst(wxCommandEvent&event);
    void I8275CursorChar(wxCommandEvent&event);
    void I8275CursorRow(wxCommandEvent&event);

    wxString i8275ConfigRegisterValueString[I8275_NUMBER_OF_REGISTERS];
    wxString lastI8275ConfigRegisterValueString[I8275_NUMBER_OF_REGISTERS];

    char i8275ConfigSelector[I8275_NUMBER_OF_SELECTORS];
    char updateI8275ConfigSelector[I8275_NUMBER_OF_SELECTORS];
    
    DECLARE_EVENT_TABLE()
};

#endif // I8275_CONFIG_H

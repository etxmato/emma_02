#ifndef SCN2672_CONFIG_H
#define SCN2672_CONFIG_H

#include "mc6845_config.h"

class Scn2672Config: public Mc6845Config
{
public:

    Scn2672Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Scn2672Config();

    void scn2672ConfigInit();
    void crt8002ConfigInit();
    void updateScn2672Panel();
    int setScn2672Register(int registerNumber, Word value, int showTrace = SHOW_ADDRESS_TRACE);
    int readScn2672Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int readScn2672RegisterSetData(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setScn2672Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setScn2672RegisterNibble(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    bool isScn2672TraceChecked(int registerNumber);
    int setScn2672SelectorValue(int selectorNumber, int selectorValue, int showTrace = SHOW_ADDRESS_TRACE);

protected:
    void parseXml_Scn2672Video(wxXmlNode &node);
    void parseXml_Crt8002Video(wxXmlNode &node);

private:
    void Scn2672InitReg(wxCommandEvent&event);
    void Scn2672Command(wxCommandEvent&event);
    void Scn2672ScreenStart(wxCommandEvent&event);
    void Scn2672Cursor(wxCommandEvent&event);
    void Scn2672Pointer(wxCommandEvent&event);
    void Scn2672Data(wxCommandEvent&event);
    void Scn2672RegisterByte(wxCommandEvent&event);

    wxString scn2672ConfigRegisterValueString[SCN2672_NUMBER_OF_REGISTERS];
    wxString lastScn2672ConfigRegisterValueString[SCN2672_NUMBER_OF_REGISTERS];

    char scn2672ConfigSelector[SCN2672_NUMBER_OF_SELECTORS];
    char updateScn2672ConfigSelector[SCN2672_NUMBER_OF_SELECTORS];

    bool currentInterLace_;
    
    DECLARE_EVENT_TABLE()
};

#endif // SCN2672_CONFIG_H

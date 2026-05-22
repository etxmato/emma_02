#ifndef SCN2672_CONFIG_H
#define SCN2672_CONFIG_H

#include "crt8002_config.h"

class Scn2672Config: public Crt8002Config
{
public:

    Scn2672Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Scn2672Config();

    void scn2672ConfigInit();
    void updateScn2672Panel();
    int readScn2672Register(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int readScn2672RegisterSetData(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setScn2672RegisterWord(int registerNumber, Word value, int showTrace = SHOW_ADDRESS_TRACE);
    int setScn2672Register12Bit(int registerNumber, Word value, int showTrace);
    int setScn2672RegisterByte(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setScn2672RegisterNibble(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    bool isScn2672TraceChecked(int registerNumber);
    int setScn2672SelectorValue(int selectorNumber, int selectorValue, int showTrace = SHOW_ADDRESS_TRACE);

protected:
    int setScn2672Register(int registerNumber, int showTrace);
    void parseXml_Scn2672Video(wxXmlNode &node);

private:
/*    void Scn2672InitReg(wxCommandEvent&event);
    void Scn2672Command(wxCommandEvent&event);
    void Scn2672ScreenStart(wxCommandEvent&event);
    void Scn2672Cursor(wxCommandEvent&event);
    void Scn2672Pointer(wxCommandEvent&event);
    void Scn2672Data(wxCommandEvent&event);
    void Scn2672Scanlines(wxCommandEvent&event);
    void Scn2672CharWidth(wxCommandEvent&event);
    void Scn2672ScreenRows(wxCommandEvent&event);
    void Scn2672CharPerRow(wxCommandEvent&event);*/
    void Scn2672Register(wxCommandEvent&event);

    wxString scn2672ConfigRegisterValueString[SCN2672_NUMBER_OF_REGISTERS];
    wxString lastScn2672ConfigRegisterValueString[SCN2672_NUMBER_OF_REGISTERS];

    char scn2672ConfigSelector[SCN2672_NUMBER_OF_SELECTORS];
    char updateScn2672ConfigSelector[SCN2672_NUMBER_OF_SELECTORS];

    bool currentInterLace_;
    
    DECLARE_EVENT_TABLE()
};

#endif // SCN2672_CONFIG_H

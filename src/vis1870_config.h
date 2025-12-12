#ifndef VIS1870_CONFIG_H
#define VIS1870_CONFIG_H

#include "tms9918_config.h"

class Vis1870Config: public Tms9918Config
{
public:

    Vis1870Config(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~Vis1870Config();

    void visConfigInit();
    void updateVis1870Panel();
    int setVisRegister(int registerNumber, Word value, int showTrace = SHOW_ADDRESS_TRACE);
    int setVisRegister(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);

protected:
    void parseXml_VisVideo(wxXmlNode &node);

private:
    void VisIntReset(wxCommandEvent&event);
    void VisIntEnable(wxCommandEvent&event);
    void VisR2(wxCommandEvent&event);
    void VisR3(wxCommandEvent&event);
    void VisR4(wxCommandEvent&event);
    void VisR5_1(wxCommandEvent&event);
    void VisR5_0(wxCommandEvent&event);
    void VisR6(wxCommandEvent&event);
    void VisR7(wxCommandEvent&event);

    Word vis1870ConfigRegisterValue[VIS_NUMBER_OF_REGISTERS];
    wxString vis1870ConfigRegisterValueString[VIS_NUMBER_OF_REGISTERS];
    wxString lastVis1870ConfigRegisterValueString[VIS_NUMBER_OF_REGISTERS];
    
    bool registerSelectActive_;

    DECLARE_EVENT_TABLE()
};

#endif // VIS1870_CONFIG_H

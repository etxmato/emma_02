#ifndef ST4_VIDEO_CONFIG_H
#define ST4_VIDEO_CONFIG_H

#include "video_config.h"

class St4VideoConfig: public VideoConfig
{
public:

    St4VideoConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~St4VideoConfig();

    void st4VideoConfigInit();
    void updateSt4VideoPanel();
    int setSt4VideoRegister(int registerNumber, bool value, int showTrace = SHOW_ADDRESS_TRACE);
    int setSt4VideoRegister(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setSt4VideoRegisterNibleBackground(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);
    int setSt4SelectorValue(int selectorNumber, bool selectorValue, int showTrace = SHOW_ADDRESS_TRACE);
    
protected:
    void parseXml_St4Video(wxXmlNode &node);

private:
    void St4VideoEnable(wxCommandEvent&event);
    void St4VideoDisable(wxCommandEvent&event);
    void St4VideoMode(wxCommandEvent&event);
    void St4VideoBackground(wxCommandEvent&event);

    bool st4ConfigRegisterValueBool[ST4_VIDEO_NUMBER_OF_RADIOBUTTONS];
    bool lastSt4ConfigRegisterValueBool[ST4_VIDEO_NUMBER_OF_RADIOBUTTONS];

    Byte st4ConfigRegisterValueByte[ST4_VIDEO_NUMBER_OF_REGISTERS];
    wxString st4ConfigRegisterValueString[ST4_VIDEO_NUMBER_OF_REGISTERS];
    wxString lastSt4ConfigRegisterValueString[ST4_VIDEO_NUMBER_OF_REGISTERS];

    char st4ConfigSelector[TMS9918_NUMBER_OF_SELECTORS];
    char updateSt4ConfigSelector[TMS9918_NUMBER_OF_SELECTORS];

    DECLARE_EVENT_TABLE()
};

#endif // ST4_VIDEO_CONFIG_H

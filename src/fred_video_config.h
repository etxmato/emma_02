#ifndef FRED_VIDEO_CONFIG_H
#define FRED_VIDEO_CONFIG_H

#include "vip2k_video_config.h"

class FredVideoConfig: public Vip2KVideoConfig
{
public:

    FredVideoConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~FredVideoConfig();

    void fredVideoConfigInit();
    void updateFredVideoPanel();
    int setFredVideoRegister(int registerNumber, bool value, int showTrace = SHOW_ADDRESS_TRACE);
    int setFredVideoRegisterNible(int registerNumber, Byte value, int showTrace = SHOW_ADDRESS_TRACE);

protected:
    void parseXml_FredVideo(wxXmlNode &node);

private:
    void FredVideoEnable(wxCommandEvent&event);
    void FredVideoDisable(wxCommandEvent&event);
    void FredVideoType(wxCommandEvent&event);

    bool fredConfigRegisterValue[FRED_VIDEO_NUMBER_OF_RADIOBUTTONS];
    bool lastFredConfigRegisterValue[FRED_VIDEO_NUMBER_OF_RADIOBUTTONS];

    wxString fredConfigRegisterValueString[FRED_VIDEO_NUMBER_OF_REGISTERS];
    wxString lastFredConfigRegisterValueString[FRED_VIDEO_NUMBER_OF_REGISTERS];

    DECLARE_EVENT_TABLE()
};

#endif // FRED_VIDEO

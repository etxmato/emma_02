#ifndef GUIDIY_H
#define GUIDIY_H

#include "guipico.h"
#include "elfconfiguration.h"

class GuiXml: public GuiPico
{
public:

    GuiXml(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiXml() {};

    void readXmlConig();
    void writeXmlDirConfig();
    void writeXmlConfig();
    void readXmlWindowConfig();
    void writeXmlWindowConfig();

    void onXmlControlWindows(wxCommandEvent& event);

    void onMainRamXml(wxCommandEvent& event);
    void onMainRamTextXml(wxCommandEvent& event);
    void onMainXmlXml(wxCommandEvent& event);
    void onMainXmlTextXml(wxCommandEvent& event);
    void setPrintModeXml();
    void setXmlGui();
    void onVideoNumber(wxCommandEvent&event);
    void onXmlPrintFileText(wxCommandEvent&event);
    void onXmlPrintMode(wxCommandEvent&event);
    void onXmlPrintButton(wxCommandEvent& event);
    void onXmlF4(bool forceStart);

private:    
    wxBitmap tapeOffBitmap;
    wxBitmap tapeOnBitmap;

    DECLARE_EVENT_TABLE()
};

#endif // GUIDIY_H

#ifndef GUIDIY_H
#define GUIDIY_H

#include "guipico.h"
#include "elfconfiguration.h"

class GuiXml: public GuiPico
{
public:

    GuiXml(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiXml() {};

    void readXmlConfig();
    void writeXmlDirConfig();
    void writeXmlConfig();
    void readXmlWindowConfig();
    void writeXmlWindowConfig();

    void onXmlControlWindows(wxCommandEvent& event);

    void onRomRam0Xml(wxCommandEvent& event);
    void onRomRam1Xml(wxCommandEvent& event);
    void romRamXml(int romRamButton, wxString romRamButtonString);
    void onRomRom0TextXml(wxCommandEvent& event);
    void onRomRom1TextXml(wxCommandEvent& event);
    void romRomTextXml(int romRamButton, wxString romRamButtonString);
    void onRomRom0ComboXml(wxCommandEvent& event);
    void onRomRom1ComboXml(wxCommandEvent& event);
    void romRomComboXml(int romRamButton, wxString romRamButtonString);
    void onMainXmlXml(wxCommandEvent& event);
    void onMainXmlTextXml(wxCommandEvent& event);
    void onMainXmlComboXml(wxCommandEvent& event);
    void onMainDirComboXml(wxCommandEvent& event);
    void setXmlDirFileGui();
    void setXmlDropDown();
    void setRomRamDropDown(int romRamButton, wxString romRamButtonString);
    void setXmlDirDropDown();
    void setXmlSubDir(wxString directory);
    void setPrintModeXml();
    void setRomRamButtonOrder();
    void setXmlGui();
    void onVideoNumber(wxCommandEvent&event);
    void onXmlPrintFileText(wxCommandEvent&event);
    void onXmlPrintMode(wxCommandEvent&event);
    void onXmlPrintButton(wxCommandEvent& event);
    void onXmlF4(bool forceStart);
    void onXmlBaudR(wxCommandEvent&event);
    void onXmlBaudT(wxCommandEvent&event);
    void onTempo(wxScrollEvent&event);

    void setNvRamDisable(int computer, bool status){elfConfiguration[computer].nvRamDisable = status;};
    bool getNvRamDisbale(int computer){return elfConfiguration[computer].nvRamDisable;};
    
    int getRomRamButton0() {return romRamButton0_;};
    int getRomRamButton1() {return romRamButton1_;};

protected:
    int romRamButton0_;
    int romRamButton1_;

private:
    wxBitmap tapeOffBitmap;
    wxBitmap tapeOnBitmap;

    wxArrayString dirNameList_;
    wxArrayString dirNameListDefaultFile_;
    wxArrayString dirNameListGui_;

    size_t xmlFileComboSelection;
    size_t xmlDirComboSelection;
    wxString xmlDirComboString;

    wxArrayString romRamFileNameListGui_[2];
    wxArrayString romRamDirNameListGui_[2];
    size_t romRamFileComboSelection[2];

    bool dropdownUpdateOngoing_;
    
    DECLARE_EVENT_TABLE()
};

#endif // GUIDIY_H

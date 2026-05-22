#ifndef XMLBASE_H
#define XMLBASE_H

#include "guimain.h"
#include "computerconfig.h"

class XmlBase: public GuiMain
{
public:

    XmlBase(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~XmlBase() {};

protected:
    EfFlag init_EfFlag();
    IoPort init_IoPort();
    
    void xmlBaseConfigInit();
    void parseXml_pixieGraphics(wxXmlNode &node);
    void disableIoPortConfig(wxString registerIdentifier);
    void disableIoPortConfigRadio(wxString registerIdentifier);
    void setIoPortConfig(IoPort ioport, wxString registerIdentifier, wxString checkBoxLabel, wxString directionLabel, bool sixteenBit = false, wxString qLabel = "-");
    void setIoPortConfig(wxString registerIdentifier, wxString checkBoxLabel, wxString directionLabel, wxString portLabel = "-", wxString qLabel = "-");
    void setIoPortConfigRadio(IoPort ioport, wxString registerIdentifier, wxString checkBoxLabel, wxString label);
    void setEfFlagConfig(EfFlag efflag, wxString text, wxString flagId);
    IoPort parseXml_IoPort(wxXmlNode &node, int ioDefinition = 0, Byte defaultMask = 0xff);
    EfFlag parseXml_EfFlag(wxXmlNode &node, int ioDefinition = 0);
    long parseXml_Number(wxXmlNode &node);
    long parseXml_Number(wxXmlNode &node, wxString attribute);
    int parseXml_Mc6847BitSetup(wxXmlNode &node, int bitNumber, wxString childName);
    bool parseXml_Range(wxXmlNode &node, long *start, long *end);
    long getHexDec(wxString numberString);
    long getNextHexDec(wxString *numberString);
    bool parseXml_Size(wxXmlNode &node, long *width, long *height);
    wxString getDoubleString(wxString doubleSting, wxString tag, double max, wxString maxStr);
    double getDouble(wxString doubleSting, wxString tag, double max, wxString maxStr, bool allowZero);
    int textToColorCode(wxString color, bool returnMinusOnNotFound = false);
    wxColour textToWxColour(wxString color);
    long getButtonNumber(wxString idReference);

private:
    size_t guiItemConfigNumber_;
    size_t memConfigNumber_;

    int defaultFrontPanelX_, defaultFrontPanelY_;
        
    DECLARE_EVENT_TABLE()
};

#endif // XMLBASE_H

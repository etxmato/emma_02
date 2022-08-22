#ifndef GUIXML_H
#define GUIXML_H

#include "guimain.h"
#include "elfconfiguration.h"

class GuiXml: public GuiMain
{
public:

    GuiXml(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~GuiXml() {};

    void parseXmlFile(int computer, wxString xmlDir, wxString xmlFile);

private:
    void parseXml_System(int computer, wxXmlNode &node);
    void parseXml_Bootstrap(int computer, wxXmlNode &node);
    void parseXml_IdeDisk(int computer, wxXmlNode &node);
    void parseXml_FdcDisk(int computer, wxXmlNode &node);
    void parseXml_PixieVideo(int computer, wxXmlNode &node);
    void parseXml_MC6845Video(int computer, wxXmlNode &node);
    void parseXml_MC6847Video(int computer, wxXmlNode &node);
    void parseXml_TMS9918Video(int computer, wxXmlNode &node);
    void parseXml_Intel8275Video(int computer, wxXmlNode &node);
    void parseXml_SuperVideo(int computer, wxXmlNode &node);
    void parseXml_Ps2Keyboard (int computer, wxXmlNode &node);
    void parseXml_AsciiKeyboard (int computer, wxXmlNode &node);
    void parseXml_ElfHexKeypad (int computer, wxXmlNode &node);
    void parseXml_SerialVt (int computer, wxXmlNode &node);
    void parseXml_UartVt (int computer, wxXmlNode &node, bool uart16450);
    void parseXml_Printer (int computer, wxXmlNode &node);
    void parseXml_Cassette (int computer, wxXmlNode &node);
    void parseXml_QSound (int computer, wxXmlNode &node);
    void parseXml_Panel (int computer, wxXmlNode &node);
    void parseXml_Memory(int computer, wxXmlNode &node);
    void parseXml_RomRam(int computer, wxXmlNode &node, int type, size_t configNumber);
    void parseXml_Ems(int computer, wxXmlNode &node, int type, size_t configNumber);
    void parseXml_portExt(int computer, wxXmlNode &node, int type, size_t configNumber);
    void parseXml_Mc6857BitSetup(int computer, wxXmlNode &node, int bitNumber, wxString childName);
    long parseXml_Number(wxXmlNode &node);
    long parseXml_Number(wxXmlNode &node, wxString attribute);
    bool parseXml_Range(wxXmlNode &node, long *start, long *end);
    long getHexDec(wxString numberString);
    wxString getDoubleString(wxString doubleSting, wxString tag, double max, wxString maxStr);

    DECLARE_EVENT_TABLE()
};

#endif // GUIXML_H

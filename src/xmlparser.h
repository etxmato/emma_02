#ifndef XMLFILEPARSER_H
#define XMLFILEPARSER_H

#include "guimain.h"
#include "elfconfiguration.h"

class XmlParser: public GuiMain
{
public:

    XmlParser(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~XmlParser() {};

    void parseXmlFile(int computer, wxString xmlDir, wxString xmlFile);

private:
    void parseXml_System(int computer, wxXmlNode &node);
    void parseXml_Basic(int computer, wxXmlNode &node);
    void parseXml_Locations(int computer, wxXmlNode &node);
    void parseXml_Bootstrap(int computer, wxXmlNode &node);
    void parseXml_IdeDisk(int computer, wxXmlNode &node);
    void parseXml_FdcDisk(int computer, wxXmlNode &node);
    void parseXml_PixieVideo(int computer, wxXmlNode &node);
    void parseXml_MC6845Video(int computer, wxXmlNode &node);
    void parseXml_MC6847Video(int computer, wxXmlNode &node);
    void parseXml_TMS9918Video(int computer, wxXmlNode &node);
    void parseXml_Intel8275Video(int computer, wxXmlNode &node);
    void parseXml_VisVideo(int computer, wxXmlNode &node);
    void parseXml_Ps2Keyboard (int computer, wxXmlNode &node);
    void parseXml_AsciiKeyboard (int computer, wxXmlNode &node);
    void parseXml_1871Keyboard (int computer, wxXmlNode &node);
    void parseXml_LatchKeyboard (int computer, wxXmlNode &node);
    void parseXml_MatrixKeyboard (int computer, wxXmlNode &node);
    void parseXml_ElfKeypad (int computer, wxXmlNode &node);
    void parseXml_BitKeypad (int computer, int padnumber, wxXmlNode &node);
    void parseXml_EfButtons (int computer, wxXmlNode &node);
    void parseXml_FrontPanel (int computer, wxXmlNode &node);
    void parseXml_SerialVt (int computer, wxXmlNode &node);
    void parseXml_UartVt (int computer, wxXmlNode &node, bool uart16450);
    void parseXml_Printer (int computer, wxXmlNode &node, int printerType);
    void parseXml_Cassette (int computer, wxXmlNode &node);
    void parseXml_Xmodem (int computer, wxXmlNode &node);
    void parseXml_KeyFile (int computer, wxXmlNode &node);
    void parseXml_VideoDump (int computer, wxXmlNode &node);
    void parseXml_MemAccess (int computer, wxXmlNode &node);
    void parseXml_Diag (int computer, wxXmlNode &node);
    void parseXml_BatchWav (int computer, wxXmlNode &node);
    void parseXml_QSound (int computer, wxXmlNode &node);
    void parseXml_Dip (int computer, wxXmlNode &node);
    void parseXml_IoGroup (int computer, wxXmlNode &node);
    void parseXml_Panel (int computer, wxXmlNode &node);
    void parseXml_Memory(int computer, wxXmlNode &node);
    void parseXml_RomRam(int computer, wxXmlNode &node, int type, size_t configNumber);
    void parseXml_Ems(int computer, wxXmlNode &node, int type, size_t configNumber);
    void parseXml_Slot(int computer, wxXmlNode &node, size_t maxSlots);
    void parseXml_Bank(int computer, wxXmlNode &node, int slot, size_t maxBanks);
    void parseXml_SlotRomRam(int computer, wxXmlNode &node, int slot, int type);
    void parseXml_BankRomRam(int computer, wxXmlNode &node, int slot, int bank, int type);
    void parseXml_portExt(int computer, wxXmlNode &node, int type, size_t configNumber);
    void parseXml_Mc6857BitSetup(int computer, wxXmlNode &node, int bitNumber, wxString childName);
    void parseXml_RtcM58T58(int computer, wxXmlNode &node);
    void parseXml_UsbSuperBoard(int computer, wxXmlNode &node);
    void parseXml_nvRamMp(int computer, wxXmlNode &node);
    long parseXml_Number(wxXmlNode &node);
    long parseXml_Number(wxXmlNode &node, wxString attribute);
    bool parseXml_Range(wxXmlNode &node, long *start, long *end);
    long getHexDec(wxString numberString);
    long getNextHexDec(wxString *numberString);
    bool parseXml_Size(wxXmlNode &node, long *width, long *height);
    wxString getDoubleString(wxString doubleSting, wxString tag, double max, wxString maxStr);
    double getDouble(wxString doubleSting, wxString tag, double max, wxString maxStr);

    bool batchFileDirDefined_;
    bool memAccessDirDefined_;
    bool keyFileDirDefined_;
    bool videoDumpDirDefined_;
    bool charRomDirDefined_;
    bool vt52CharRomDirDefined_;
    bool vt100CharRomDirDefined_;
    bool vtWavFileDirDefined_;
    bool printFileDirDefined_;
    bool ideFileDirDefined_;
    bool wavFileDirDefined[2];
    bool xmodemFileDirDefined;
    bool floppyDirDefined[FDCTYPE_MAX][4];

    DECLARE_EVENT_TABLE()
};

#endif // XMLFILEPARSER_H
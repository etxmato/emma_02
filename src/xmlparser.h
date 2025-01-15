#ifndef XMLFILEPARSER_H
#define XMLFILEPARSER_H

#include "guimain.h"
#include "computerconfig.h"

class XmlParser: public GuiMain
{
public:

    XmlParser(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~XmlParser() {};

    void parseXmlFile(wxString xmlDir, wxString xmlFile);

private:
    void parseXml_System(wxXmlNode &node);
    void parseXml_Basic(wxXmlNode &node);
    void parseXml_Locations(wxXmlNode &node);
    void parseXml_Bootstrap(wxXmlNode &node);
    void parseXml_IdeDisk(wxXmlNode &node);
    void parseXml_FdcDisk(wxXmlNode &node);
    void parseXml_Upd765(wxXmlNode &node);
    void parseXml_CoinVideo(wxXmlNode &node);
    void parseXml_PixieVideo(wxXmlNode &node);
    void parseXml_1862Video(wxXmlNode &node);
    void parseXml_1864Video(wxXmlNode &node);
    void parseXml_St4Video(wxXmlNode &node);
    void parseXml_Vip2KVideo(wxXmlNode &node);
    void parseXml_fredVideo(wxXmlNode &node);
    void parseXml_pixieGraphics(wxXmlNode &node);
    void parseXml_MC6845Video(wxXmlNode &node);
    void parseXml_MC6847Video(wxXmlNode &node);
    void parseXml_TMS9918Video(wxXmlNode &node);
    void parseXml_Intel8275Video(wxXmlNode &node);
    void parseXml_VisVideo(wxXmlNode &node);
    void parseXml_SN76430NVideo(wxXmlNode &node);
    void parseXml_Ps2Keyboard(wxXmlNode &node);
    void parseXml_Gpio(wxXmlNode &node);
    void parseXml_AsciiKeyboard(wxXmlNode &node);
    void parseXml_1871Keyboard(wxXmlNode &node);
    void parseXml_LatchKeyboard(wxXmlNode &node);
    void parseXml_MatrixKeyboard(wxXmlNode &node);
    void parseXml_Vip2KKeyboard(wxXmlNode &node);
    void parseXml_ElfKeypad(wxXmlNode &node);
    void parseXml_BitKeypad(int padnumber, wxXmlNode &node);
    void parseXml_CvKeypad(wxXmlNode &node);
    void parseXml_LatchKeypad(int pad, wxXmlNode &node);
    void parseXml_FredKeypad(wxXmlNode &node);
    void parseXml_EtiKeypad(wxXmlNode &node);
    void parseXml_EfButtons(wxXmlNode &node);
    void parseXml_AdConvertor(wxXmlNode &node);
    void parseXml_FrontPanel(wxXmlNode &node, int frontNumber);
    void parseXml_FrontPanelItem(wxXmlNode &node, int frontNumber);
    void parseXml_Cdp1877(wxXmlNode &node, bool windowOn);
    void parseXml_Cdp1851(wxXmlNode &node, bool windowOn);
    void parseXml_Cdp1852(wxXmlNode &node, bool windowOn);
    void parseXml_Cd4536b(wxXmlNode &node);
    void parseXml_SerialVt(wxXmlNode &node);
    void parseXml_UartVt(wxXmlNode &node, bool uart16450);
    void parseXml_Printer(wxXmlNode &node, int printerType);
    void parseXml_SwCassette(wxXmlNode &node);
    void parseXml_HwCassette(wxXmlNode &node);
    void parseXml_Xmodem(wxXmlNode &node);
    void parseXml_KeyFile(wxXmlNode &node);
    void parseXml_Splash(wxXmlNode &node);
    void parseXml_VideoDump(wxXmlNode &node);
    void parseXml_MemAccess(wxXmlNode &node);
    void parseXml_Gui(wxXmlNode &node);
    void parseXml_Diag(wxXmlNode &node);
    void parseXml_Debugger(wxXmlNode &node);
    void parseXml_Scrt(wxXmlNode &node);
    void parseXml_Assembler(wxXmlNode &node);
    void parseXml_BatchWav(wxXmlNode &node);
    void parseXml_QSound(wxXmlNode &node);
    void parseXml_OutBitSound(wxXmlNode &node);
    void parseXml_Cdp1863Sound(wxXmlNode &node);
    void parseXml_Dip(wxXmlNode &node);
    void parseXml_IoGroup(wxXmlNode &node);
    void parseXml_Flipflop(wxXmlNode &node);
    void parseXml_Panel(wxXmlNode &node);
    void parseXml_Memory(wxXmlNode &node);
    void setMemMask(size_t configNumber, long mask);
    size_t getMemConfig(wxString type);
    void setCartType(size_t configNumber, wxString type);
    void parseXml_RomRam(wxXmlNode &node, int type, size_t configNumber);
    void parseXml_Ems(wxXmlNode &node, int type, size_t configNumber);
    void parseXml_Slot(wxXmlNode &node, int maxSlots);
    void parseXml_Bank(wxXmlNode &node, int slot, int maxBanks);
    void parseXml_SlotRomRam(wxXmlNode &node, int slot, int type);
    void parseXml_BankRomRam(wxXmlNode &node, int slot, int bank, int type);
    void parseXml_Mcr(wxXmlNode &node, int maxMaps);
    void parseXml_Map(wxXmlNode &node, int mapNum);
    void parseXml_MapRomRam(wxXmlNode &node, int mapNum, int memNum, int type);
    void parseXml_MapIO(wxXmlNode &node, int memNum);
    void parseXml_portExt(wxXmlNode &node, int type, size_t configNumber);
    void parseXml_Mc6857BitSetup(wxXmlNode &node, int bitNumber, wxString childName);
    void parseXml_RtcCdp1879(wxXmlNode &node);
    void parseXml_RtcM48T58(wxXmlNode &node);
    void parseXml_RtcDS12887(wxXmlNode &node);
    void parseXml_UsbSuperBoard(wxXmlNode &node);
    void parseXml_nvRamMp(wxXmlNode &node);
    IoPort parseXml_IoPort(wxXmlNode &node, int ioDefinition = 0, Byte defaultMask = 0xff);
    IoPort init_IoPort();
    EfFlag parseXml_EfFlag(wxXmlNode &node, int ioDefinition = 0);
    EfFlag init_EfFlag();
    long parseXml_Number(wxXmlNode &node);
    long parseXml_Number(wxXmlNode &node, wxString attribute);
    bool parseXml_Range(wxXmlNode &node, long *start, long *end);
    long getHexDec(wxString numberString);
    long getNextHexDec(wxString *numberString);
    bool parseXml_Size(wxXmlNode &node, long *width, long *height);
    wxString getDoubleString(wxString doubleSting, wxString tag, double max, wxString maxStr);
    double getDouble(wxString doubleSting, wxString tag, double max, wxString maxStr, bool allowZero);

    size_t guiItemConfigNumber_;
    size_t memConfigNumber_;

    int defaultFrontPanelX_, defaultFrontPanelY_;
    
    DECLARE_EVENT_TABLE()
};

#endif // XMLFILEPARSER_H

#ifndef XMLFILEPARSER_H
#define XMLFILEPARSER_H

#include "vis1870_config.h"
#include "computerconfig.h"

class XmlParser: public Vis1870Config
{
public:

    XmlParser(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
    ~XmlParser() {};

    void parseXmlFile(wxString xmlDir, wxString xmlFile);

private:
    void parseXml_System(wxXmlNode &node);
    void parseXml_ModKeys(wxXmlNode &node);
    int getModifier(wxString modText);
    void parseXml_Basic(wxXmlNode &node);
    void parseXml_Locations(wxXmlNode &node);
    void parseXml_Bootstrap(wxXmlNode &node);
    void parseXml_IdeDisk(wxXmlNode &node);
    void parseXml_FdcDisk(wxXmlNode &node);
    void parseXml_Upd765(wxXmlNode &node);
    void parseXml_Tu58Disk(wxXmlNode &node);
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
    void parseXml_FrontPanelItem(wxXmlNode &node, int frontNumber, wxPoint origin);
    void parseXml_Cdp1851_Printer(wxXmlNode &node);
    void parseXml_Cdp1851_PrinterPort(wxXmlNode &node, int port);
    int get_Cdp1851_Printer_function(wxString function);
    void parseXml_Cdp1851(wxXmlNode &node, bool windowOn, int connection);
    void parseXml_Cdp1852(wxXmlNode &node, bool windowOn);
    void parseXml_Cdp1854(wxXmlNode &node, int connection);
    void parseXml_Cdp1855(wxXmlNode &node);
    void parseXml_Cdp1877(wxXmlNode &node);
    void parseXml_AmiIntController(wxXmlNode &node);
    void parseXml_Cdp1878(wxXmlNode &node);
    void parseXml_Cd4536b(wxXmlNode &node);
    void parseXml_Mm57109(wxXmlNode &node);
    void parseXml_Scn2671(wxXmlNode &node, int connection);
    void parseXml_SerialVt(wxXmlNode &node);
    void parseXml_Scn2671Vt(wxXmlNode &node);
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
    void parseXml_Sep(wxXmlNode &node, SepConfiguration *sep);
    void parseXml_TraceInstruction(wxXmlNode &node);
    void parseXml_BatchWav(wxXmlNode &node);
    void parseXml_QSound(wxXmlNode &node);
    void parseXml_StudioSound(wxXmlNode &node);
    void parseXml_OutBitSound(wxXmlNode &node);
    void parseXml_CoinSound(wxXmlNode &node);
    void parseXml_Cdp1863Sound(wxXmlNode &node);
    void parseXml_AY_3_8912Sound(wxXmlNode &node);
    void getChannels(wxString channel, wxString side, int *one, int *two);
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
    void parseXml_RtcCdp1879(wxXmlNode &node);
    void parseXml_RtcM48T58(wxXmlNode &node);
    void parseXml_RtcDS12887(wxXmlNode &node);
    void parseXml_RtcMM58174(wxXmlNode &node);
    void parseXml_UsbSuperBoard(wxXmlNode &node);
    void parseXml_nvRamMp(wxXmlNode &node);

    size_t guiItemConfigNumber_;
    size_t memConfigNumber_;

    int defaultFrontPanelX_, defaultFrontPanelY_;
        
    DECLARE_EVENT_TABLE()
};

#endif // XMLFILEPARSER_H

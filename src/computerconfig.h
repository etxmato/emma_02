#ifndef COMPUTERCONFIG_H
#define COMPUTERCONFIG_H

#define MAXROM 6

class ConfigurationMenuInfo
{
public:
    wxString fileName;
    wxString menuName;
    wxString subMenuName;
};

class ConfigurationSubMenuInfo
{
public:
    wxString subMenuName;
    wxMenu *subMenu;
    wxMenu *deleteSubMenu;
};

class MemConfig
{
public:
    wxString dirname;
    wxString filename;
    bool verifyFileExist;
    Word start;
    Word end;
    Word memMask;
    Word outputStart;
    Word outputEnd;
    int type;
};

class EmsConfig
{
public:
    wxString dirname;
    wxString filename;
    Word start;
    Word end;
    Word mask;
    Word outputStart;
    Word outputEnd;
    Byte outputMask;
    int maskBits;
    int type;
    int emsType;
    Byte page;
};

class Conf
{
public:
    wxString mainDir_;

    size_t memConfigNumber_;
    vector<MemConfig> memConfig_;
    
    size_t emsConfigNumber_;
    vector<EmsConfig> emsConfig_;

    wxString romDir_[MAXROM];
    wxString rom_[MAXROM];
    wxString ramDir_;
    wxString ram_;
    wxString chip8SWDir_;
    wxString chip8SW_;
    wxString wavFileDir_[2];
    wxString wavFile_[2];
    wxString batchFileDir_;
    wxString batchFile_;
    wxString charRomDir_;
    wxString charRom_;
    wxString ideDir_;
    wxString ide_;
    wxString keyFileDir_;
    wxString keyFile_;
    wxString screenDumpFileDir_;
    wxString screenDumpFile_;
    wxString printFileDir_;
    wxString printFile_;
    wxString configurationDir_;
    wxString xmlDir_;
    wxString xmlFile_;

    wxArrayString terminalPaths_;
    wxArrayString terminalFiles_;
    size_t numberOfTerminalFiles_;

    ConfigurationInfo configurationInfo_;
    
    wxMenu *configurationMenu;
    wxMenu *configurationDeleteMenu;
    
    size_t configurationSubMenuInfoNumber_;
    vector<ConfigurationSubMenuInfo> configurationSubMenuInfo_;

    int printMode_;

    wxString loadFileName_;
    wxString loadFileNameFull_;

    char pLoadSaveName_[4];
    Word defus_;
    Word eop_;
    Word string_;
    Word arrayValue_;
    Word eod_;
    Word basicRamAddress_;

    long bootAddress_;
    bool autoBoot;
    int autoBootType;

    int volume_;
    int tempo_;
    int soundType_;
    double clockSpeed_;
    int beepFrequency_;
    wxString ledTime_;
    long ledTimeMs_;
    wxString clock_;
    wxString zoom_;
    wxString xScale_;
    wxString zoomVt_;
    int fdcCpms_;
    int videoMode_;
    int velfMode_;
    int ramType_;
    int computerVersion_;
    bool useKeyboard_;

    bool printerOn_;
    bool useLoadLocation_;
    bool autoCassetteLoad_;
    bool turbo_;
    bool interlace_;
    bool dram_;
    bool st2020Active_;
    bool sbActive_;
    bool diagActive_;
    int diagOn_;
    bool videoLog_;
    bool stretchDot_;
    bool realCassetteLoad_;
    wxString turboClock_;

    bool multiCart_;
    bool disableSystemRom_;
    Byte lsb_;
    Byte msb_;

    int v1870X_, v1870Y_;
    int pixieX_, pixieY_;
    int tmsX_, tmsY_;
    int vtX_, vtY_;
    int vtUart2X_, vtUart2Y_;
    int mc6845X_, mc6845Y_;
    int mc6847X_, mc6847Y_;
    int i8275X_, i8275Y_;
    int mainX_, mainY_;
    int keypadX_, keypadY_;
    int secondFrameX_, secondFrameY_;
    int thirdFrameX_, thirdFrameY_;
    int fourthFrameX_, fourthFrameY_;

    int sizeX_;
    int sizeY_;
    int sizeNTSCX_;
    int sizeNTSCY_;
    int sizeX6845_;
    int sizeY6845_;

    Word saveStart_;
    Word saveEnd_;
    Word saveExec_;

    wxString saveStartString_;
    wxString saveEndString_;
    wxString saveExecString_;
    
    bool scrtMode_;
    long debugCallReg_;
    long debugCallAddress_;
    long debugRetReg_;
    long debugRetAddress_;

    int gameId_;
    
    wxString errorDoubleBoard_[24];
    wxString errorMemoryOverlapp_[24];
    int memoryMapType_[256];
    int memoryMapCard_[256];

    int microboardMaxCards_;
    int microboardType_[24];
    wxString microboardTypeStr_[24];
    int microChipType_[2];
    int microChipLocation_[3];
    wxString microChipBlock_[2];
    int microChipMemory_[MAXROM];
    bool microChipDisable_[MAXROM];
    
    int pagerMaskBits_;
    Word pagerMask_;
    Byte startPort_;
    Byte endPort_;
};

#endif // COMPUTERCONFIG_H

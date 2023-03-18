#ifndef COMPUTERCONFIG_H
#define COMPUTERCONFIG_H

#include "definition.h"

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
    bool useMemMask;
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

class CopyConfig
{
public:
    Word start;
    Word end;
    Word copy;
    int slot;
};

class BankInfo
{
public:
    wxString dirname;
    wxString filename;
    int type;
};

class SlotInfo
{
public:
    wxString dirname;
    wxString filename;
    int type;
    
    int maxBankNumber_;
    vector<BankInfo> bankInfo;
    Byte bankOutputMaskInSlot;
};

class SlotConfig
{
public:
    int maxSlotNumber_;
    vector<SlotInfo> slotInfo;
    Word start;
    Word end;
    Byte output;
    Byte slotOutputMask;
    bool slotOutputNumber;
    int slotOutputShift;
    Byte bankOutputMask;
    int bankOutputShift;
    bool bankOutputNumber;
    bool useIoGroup;
};

class CheckAddressInfo
{
public:
    Word checkAddress;
    Byte checkValue;
};

class LocationInfo
{
public:
    vector<CheckAddressInfo> checkAddressInfo;
    Word additionalAddress;
    int type;
};

enum
{
    INFO_RESET_STATE = 0,
    INFO_BASIC_STATE,
    INFO_RUN_STATE,
    INFO_STOP_THERMAL,
    INFO_STOP_CAS,
    INFO_START_CAS_SAVE,
    INFO_START_CAS_LOAD,
    INFO_START_CAS_DLOAD,
    INFO_SET_DOS_FILENAME,
    INFO_BATCH_START_SAVE,
    INFO_BATCH_END_SAVE,
    INFO_START_XMODEM_SAVE,
    INFO_START_XMODEM_LOAD,
    INFO_START_YMODEM_SAVE,
    INFO_CORRECT_CAPS,
    INFO_ELFOS_BOOT,
    INFO_START_CHIP8
};

class Locations
{
public:
    vector<Word> keyInputAddress;
    vector<LocationInfo> locationInfo;
};

class LocationTrigger
{
public:
    vector<signed char> index;
};

class Conf
{
public:
    wxString mainDir_;

    size_t memConfigNumber_;
    vector<MemConfig> memConfig_;
    
    size_t emsConfigNumber_;
    vector<EmsConfig> emsConfig_;

    size_t copyConfigNumber_;
    vector<CopyConfig> copyConfig_;
    
    SlotConfig slotConfig_;
    Locations addressLocations;
    vector<LocationTrigger> locationTrigger;
    
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
    wxString xmodemFileDir_;
    wxString xmodemFile_;

    bool ramFileFromGui_;

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
    wxString zoom_[VIDEOXMLMAX];
    wxString videoName_[VIDEOXMLMAX];
    int videoNumber_;
    wxString xScale_;
    wxString zoomVt_;
    int fdcCpms_;
    int videoMode_;
    int velfMode_;
    int ramType_;
    int computerVersion_;
    bool useKeyboard_;

    bool printerOn_;
    bool useBasicPrinter_;
    bool useParallelPrinter_;
    bool useSerialPrinter_;
    bool useQSerialPrinter_;
    bool useThermalPrinter_;
    bool useLoadLocation_;
    bool autoCassetteLoad_;
    bool turbo_;
    bool interlace_;
    bool dram_;
    bool st2020Active_;
    bool sbActive_;
    
    bool useDiagnosticBoard_;
    int diagRomChecksum_;
    int diagFactory_;
    int diagCassetteCables_;

    bool useBatchWav_;
    wxString saveCommand_;

    int diagRomOn_;
    bool videoLog_;
    bool stretchDot_;
    bool realCassetteLoad_;
    wxString turboClock_;

    bool multiCart_;
    bool disableSystemRom_;
    Byte lsb_;
    Byte msb_;

    int numberOfVideoTypes_;
    int v1870X_, v1870Y_;
    int pixieX_, pixieY_;
    int cdp1864X_, cdp1864Y_;
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
    int SN76430NX_, SN76430NY_;

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

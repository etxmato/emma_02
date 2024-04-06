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

class GuiItemConfig
{
public:
    int type;           // PUSH_BUTTON_ROUND_RED or SWITCHBUTTON
    int function;       // card, reset, run etc.
    bool initup;        // BUTTON_DOWN (false) or BUTTON_UP (true)
    wxString label;
    wxPoint position;
    wxSize size;
    int value;
    int rangeLow;
    int rangeHigh;
    int textSize;
    int color;
    bool useImageDir;
    wxString fileName;
    wxString dirName;
    
    wxBitmap *bitmapPointer;

    SwitchButton *switchButton;
    SwitchButton *dipSwitchButton;
    RotButton *rotButton;

    HexButton *hexButton;
    wxButton *windowsButton;
};

class LoadOffSet
{
public:
    Word addressStart;
    Word offSet;
};

class MemConfig
{
public:
    wxArrayString pulldownDir;
    wxArrayString pulldownMask;
    wxArrayString pulldownExclude;
    wxArrayString pulldownExclude2;
    wxString dirname;
    wxString filename;
    wxString filename2;
    wxString dumpFilename;
    bool verifyFileExist;
    Word start;
    Word end;
    Word memMask;
    bool useMemMask;
    Word outputStart;
    Word outputEnd;
    int type;
    LoadOffSet loadOffSet;
    int cartType;
    
    vector<Word> mappingList;
};

class EmsConfig
{
public:
    wxString dirname;
    wxString filename;
    wxString filename2;
    Word start;
    Word end;
    Word start2;
    Word end2;
    Word start3;
    Word end3;
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
    
    Word ramStart[4];
    Word ramEnd[4];

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

class CheckRegInfo
{
public:
    Byte checkReg;
    Word checkValue;
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
    vector<CheckRegInfo> checkRegInfo;
    Word additionalAddress;
    int type;
};

enum
{
    INFO_RESET_STATE = 0,
    INFO_UT_STATE,
    INFO_BASIC_STATE,
    INFO_BASIC_STATE_CW,
    INFO_RUN_STATE,
    INFO_STOP_THERMAL,
    INFO_STOP_CAS,
    INFO_STOP_CAS_MICRO,
    INFO_START_CAS_SAVE,
    INFO_START_CAS_LOAD,
    INFO_START_CAS_DLOAD,
    INFO_SET_DOS_FILENAME,
    INFO_BATCH_START_SAVE,
    INFO_BATCH_END_SAVE,
    INFO_START_XMODEM_SAVE,
    INFO_START_XMODEM_LOAD,
    INFO_START_YMODEM_SAVE,
    INFO_START_HEXMODEM_SAVE,
    INFO_START_HEXMODEM_LOAD,
    INFO_START_BINMODEM_SAVE,
    INFO_START_BINMODEM_LOAD,
    INFO_STOP_MODEM,
    INFO_CORRECT_CAPS,
    INFO_ELFOS_BOOT,
    INFO_START_CHIP8,
    INFO_CV_TAPE_PLAY,
    INFO_CV_TAPE_FF,
    INFO_CV_TAPE_REC,
    INFO_FRED_TAPE_REC,
    INFO_CHECK_SW,
    INFO_STUDIO_BUILD_IN_GAME,
    INFO_VICTORY_BUILD_IN_GAME,
    INFO_VISICOM_BUILD_IN_GAME,
    INFO_SET_PSEUDO_AM4KBAS2020,
    INFO_SET_PSEUDO_AM4KBAS1978,
};

class Locations
{
public:
    vector<Word> keyInputAddress;
    vector<Word> inReleaseAddress;
    int code_start;
    int code_start_high;
    int code_start_low;
    int code_end_high;
    int code_end_low;
    int reg_end;
    int reg_end_adjust;
    vector<LocationInfo> locationInfo;
};

class AssemblerInfo
{
public:
    wxString dir;
    bool dirDefined;
    wxString file;
    bool fileDefined;
    int code_start;
    int code_end;
    int end;
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
    int multicartEmsNumber_;

    size_t copyConfigNumber_;
    vector<CopyConfig> copyConfig_;
    
    size_t ramPartConfigNumber_;
    vector<CopyConfig> ramPartConfig_;

    vector<GuiItemConfig> guiItemConfig_;
    
    SlotConfig slotConfig_;
    Locations addressLocations;
    vector<LocationTrigger> locationTrigger;
    
    wxString stateCommand_[LAST_COMMAND_STATE];

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
    wxString xmlMainDir_;
    wxString xmlSubDir_;
    wxString xmlDir_;
    wxString xmlFile_;
    wxString xmodemFileDir_;
    wxString xmodemFile_;
    wxString sequencerDir_;
    wxString sequencerFile_;
    wxString splashDialog_;
    wxString splashText_;

    vector<AssemblerInfo> assemblerInfo_;
    
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
    bool defusDefined_;
    bool eopDefined_;
    bool stringDefined_;
    bool arrayValueDefined_;
    bool eodDefined_;
    bool basicRamAddressDefined_;

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
    int stereo_;
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
    bool useCentronicsPrinter_;
    bool useThermalPrinter_;
    bool useLoadLocation_;
    bool autoCassetteLoad_;
    bool turbo_;
    bool interlace_;
    bool dram_;
    bool st2020Active_;
    bool sbActive_;
    bool useSplashScreen_;

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
    int coinX_, coinY_;
    int pixieX_, pixieY_;
    int cdp1864X_, cdp1864Y_;
    int st4X_, st4Y_;
    int vip2KX_, vip2KY_;
    int fredX_, fredY_;
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

    int defv1870X_, defv1870Y_;
    int defCoinX_, defCoinY_;
    int defPixieX_, defPixieY_;
    int defCdp1864X_, defCdp1864Y_;
    int defSt4X_, defSt4Y_;
    int defVip2KX_, defVip2KY_;
    int defFredX_, defFredY_;
    int defTmsX_, defTmsY_;
    int defVtX_, defVtY_;
    int defMc6845X_, defMc6845Y_;
    int defMc6847X_, defMc6847Y_;
    int defi8275X_, defi8275Y_;
    int defSN76430NX_, defSN76430NY_;
    int defaultFrontPanelX_, defaultFrontPanelY_;

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

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

class LoadOffSet
{
public:
    Word addressStart;
    Word offSet;
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

class MapMemInfo
{
public:
//    wxString dirname;
//    wxString filename;
    int type;
    int memNumber;
    Word start;
    Word end;
};

class MapInfo
{
public:
    bool defined;
    vector<MapMemInfo> mapMemInfo;
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

class EfButton
{
public:
    int value;
    vector<int> flag;
};

class EfKey
{
public:
    int value;
    bool defined;
    bool mod;
    int resetInp;
    wxString modString;
};

class BitKey
{
public:
    int value;
    int bitMaskPressed;
    int bitMaskReleased;
};

class BitModKey
{
public:
    int value;
    int ef;
    int mod;
    int bitMaskPressed;
    int bitMaskReleased;
};
    
class DiagonalKeys
{
public:
    int mainKey;
    int key1;
    int key2;
};

class WordRange
{
public:
    Word start;
    Word end;
};

class IoPort
{
public:
    int ioDefinition;
    vector<int> portNumber;
    int qValue;
    Byte mask;
    Word addressMask;
    bool addressMode;
};

class EfFlag
{
public:
    int ioDefinition;
    int flagNumber;
    int qValue;
    int reverse;
    
    bool excludeIoGroup;
};

class KeyDefinition
{
public:
    Word keyValue;
    Byte bitMaskPressed;
    Byte bitMaskNotPressed;
    Byte ctrlValue;
    Byte shiftValue;
};

class PixieGraphics
{
public:
    int interrupt;
    int start;
    int end;
    int screenend;
    int videoHeight;
    int videoWidth;
};

enum
{
    INFO_RESET_STATE = 0,
    INFO_UT_STATE,
    INFO_BASIC_STATE,
    INFO_BASIC_STATE_CW,
    INFO_RUN_STATE,
    INFO_START_THERMAL,
    INFO_STOP_THERMAL,
    INFO_START_FM_BASIC,
    INFO_STOP_FM_BASIC,
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

class WriteAddress
{
public:
    Word address;
    int value;
    int function;
};

class ReadAddress
{
public:
    Word address;
    int function;
};

class IoGroupConfiguration
{
public:
    vector <int> ioGroupVector;
};

class IoGroupDefineConfiguration : public IoGroupConfiguration
{
public:
    bool defined;
};

class DefineConfiguration
{
public:
    bool defined;
};

class WordDefineConfiguration : public DefineConfiguration
{
public:
    Word value;
};

class MainIoGroupConfiguration
{
public:
    bool defined;
    
    IoPort output;
    IoPort disable;
    IoPort input;
    
    Byte disableBitMask;
};

class ModKeyConfiguration : public DefineConfiguration
{
public:
    vector<int> resetKey;
    vector<int> stopKey;
    
    Word macModifier;
    Word windowsModifier;
    Word linuxModifier;
};

// Video configuration class definitions:

class VideoConfiguration : public IoGroupDefineConfiguration
{
public:
    int videoNumber;
    int defaultX, defaultY;
    int x, y;
};

class CDP1861Configuration : public VideoConfiguration
{
public:
    IoPort input;
    IoPort output;
    EfFlag ef;
    
    int xScale;
    int doubleScreenIo;
    int efScreenOn;
    int highRes;
    int colorType;
    PixieGraphics pixieGraphics;
    
    int statusBarType;
};

class CDP1862Configuration : public VideoConfiguration
{
public:
    IoPort background;
    IoPort colorMemory;
    
    int startRam;
    int endRam;
    Word ramMask;
    int colorType;
};

class CDP1864Configuration: public VideoConfiguration
{
public:
    IoPort enable;
    IoPort disable;
    IoPort toneLatch;
    IoPort background;
    IoPort colorMemory;
    EfFlag ef;

    int xScale;
    bool screenOn;
    int colorType;
    int startRam;
    int endRam;
    Word ramMask;
    bool colorLatch;
};

class CoinConfiguration : public VideoConfiguration
{
public:
    IoPort output;
    
    int xScale;
};

class Studio4VideoConfiguration : public VideoConfiguration
{
public:
    IoPort dmaEnable;
    IoPort output;
    EfFlag ef;
    
    int xScale;
    int startRam;
    int endRam;
};

class Vip2KVideoConfiguration : public VideoConfiguration
{
public:
    IoPort output;
    IoPort input;
    EfFlag ef;

    wxString sequencerDirectory;
    wxString sequencerFile;

    double xScale;
    bool doubleScreenIo;
};

class FredVideoConfiguration : public VideoConfiguration
{
public:
    IoPort output;
    
    int xScale;
};

class Vis1870Configuration : public VideoConfiguration
{
public:
    IoPort outputWrite;
    IoPort outputSelect;
    IoPort outputInterruptEnable;
    IoPort outputInterruptReset;
    EfFlag ef;
    
    double videoClock;
    int interruptMode;
    int picInterrupt;
    int videoModeEf;
    bool useVideoModeEf;
    bool rotateScreen;
    bool useBlockWrite;
    bool cursorBlink;
    int qGroup;
    
    int videoMode;

    Word pageMemSize;
    bool pageMemIsRom;
    bool pageMemExcludeIoGroup;
    int maxScreenLines;
    Word graphicMemSize;
    bool graphicMemExcludeIoGroup;
    Word charMemSize;
    bool charMemIsRom;
    bool charMemExcludeIoGroup;
    Word charRomStart;
    bool charRomIsTmcFormat;
    int charLines;
    int maxCharLines;
    int colorRamType;
    
    Byte pcbMask;
    bool cmaMaskFixed;
    
    int statusBarType;
    int statusBarLedOut;
    bool expansionConfiguration_defined;
};

class TmsConfiguration : public VideoConfiguration
{
public:
    IoPort registerOutput;
    IoPort dataOutput;
    EfFlag efInterrupt;
};

class I8275Configuration : public VideoConfiguration
{
public:
    IoPort writeCommandOutput;
    IoPort readStatus;
    IoPort writeParameter;
    IoPort readParameter;
    EfFlag efVerticalRetrace;
    EfFlag efHorizontalRetrace;

    double videoClock;
    wxSize charSize;
    wxSize screenSize;
    bool gpaSwitched;
    int picInterrupt;
    int picInterruptHorizontal;
};

class Sn76430NConfiguration : public VideoConfiguration
{
public:
    Word startRam;
    Word endRam;
};

class Scn2672Configuration : public VideoConfiguration
{
public:
    IoPort initializationRegister;
    IoPort command;
    IoPort screenStart;
    IoPort cursor;
    IoPort pointer;
    IoPort data;
    IoPort status;
    IoPort interrupt;

	bool videoMode;
	double videoClock;
    int picInterrupt;
	
    double xScale;

    wxSize charSize;
    wxSize screenSize;
};

class Attribute
{
public:
    int bitNumber;
    bool reversePolarity;
};

class Crt8002Configuration : public VideoConfiguration
{
public:
    IoPort attribute;
    IoPort attributeScreen1;
    
    Attribute reverse;
    Attribute blink;
    Attribute graphic_ms0;
    Attribute graphic_ms1;
    Attribute underline;
    Attribute strikeThru;
    Attribute blank;
    
    int underlineLine1;
    int underlineLine2;
    int strikeThruLine1;
    int strikeThruLine2;
};

class Mc6845Configuration : public VideoConfiguration
{
public:
    EfFlag ef;
    
    Word startRam;
    Word endRam;
    Word ramMask;
    
    int address;
    int addressMask;
    int data;
    int dataMask;
    
    wxSize charSize;
    wxSize screenSize;
};

class Mc6847Configuration : public VideoConfiguration
{
public:
    IoPort output;

    int outputMode;
    Word outputStart;
    Word outputEnd;

    Word startRam;
    Word endRam;
    
    int screenHeight;

    bool forceHighAg;
    bool forceHighAs;
    bool forceHighExt;
    bool forceHighGm2;
    bool forceHighGm1;
    bool forceHighGm0;
    bool forceHighCss;
    bool forceHighInv;
    
    int b7;
    int b6;
    int b5;
    int b4;
    int b3;
    int b2;
    int b1;
    int b0;
    int dd7;
    int dd6;
    
    int inv;
    int ext;
    int css;
    int as;
    int ag;
    int gm0;
    int gm1;
    int gm2;
};

// Video Terminal configuration class definitions:

class TerminalInterfaceSetting
{
public:
    bitset<32> vt52SetUpFeature;
    bitset<32> vt100SetUpFeature;

    int charactersPerRow;
    
    double stopBit;
    
    wxString vtCharRomDirectory;
    wxString vtCharRomFileName;
    
    wxString wavDirectory;
    wxString wavFileName;
    
    bool uart1854_defined;
    bool uart16450_defined;
    bool efReverse;
    bool parity;
    bool paritySense;
    
    int reverseQ;
    int bitsPerCharacter;
    wxString serialPort;
};

class VideoTerminalConfiguration : public VideoConfiguration
{
public:
    bool scn2671_defined;
    bool xModem_defined;
    bool hexModem_defined;
    bool hexModem_cdp18s020;

    IoPort output;
    IoPort qOutput;
    IoPort uartOut;
    IoPort uartIn;
    IoPort uartControl;
    IoPort uartStatus;
    EfFlag ef;
    EfFlag efInterrupt;

    bool threUnchangedAtControl;
    Byte qOutputBitMask;
    
    wxString zoom;
    int type;
    bool external;
    bool loop_back;
    bool show;
    bool interrupt;
    bool externalBlockingWrite;
    int picInterrupt;

    int characterWidth;
    bool stretchDot;
    
    int baudR;
    int baudT;
    double baudCorrectionR;
    double baudCorrectionT;

    bool serialLog;
    bool escError;
    int bellFrequency;
    int packetSize;
    
    wxString xmodemDirectory;
    wxString xmodemFileName;
    wxString xmodemFullFileName;
    wxArrayString terminalPaths;
    wxArrayString terminalFiles;
    size_t numberOfTerminalFiles;

    bool specifiedInXml[XML_VT_MAX];
    
    TerminalInterfaceSetting terminalInterfaceSetting[2];
    int selectedTerminalSetting;
    
    int defaultCharacterWidth;
    int defaultBellFrequency;
    
    Byte backSpaceCharacter;
};

// Sound configuration class definitions:

class SoundConfiguration
{
public:
    int type;
    int volume;
    int tempo;
    int stereo;
    int beepFrequency;
    int targetBeepFrequency;
    int decay;
};

class CDP1863Configuration : public IoGroupConfiguration
{
public:
    IoPort toneLatch;
    IoPort toneSwitch1;
    IoPort toneSwitch2;
};

class AY_3_8912Configuration : public IoGroupDefineConfiguration
{
public:
    IoPort registerAddressAy1;
    IoPort registerAddressAy2;
    IoPort dataAddress;
    
    int channel_a_1;
    int channel_a_2;
    int channel_b_1;
    int channel_b_2;
    int channel_c_1;
    int channel_c_2;
};

class BitSoundConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
};

// Disk configuration class definitions:

class Upd765Configuration : public IoGroupDefineConfiguration
{
public:
    IoPort dmaControl;
    IoPort dmaCount;
    IoPort readStatus;
    IoPort writeCommand;
    IoPort readCommand;
    EfFlag efInterrupt;
    
    int cyclesPerMilliSecond;
};

class IdeConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort selectOutput;
    IoPort writeOutput;
    IoPort status;
    IoPort input;
    
    int tracks;
    int heads;
    int sectors;
    int picInterrupt;
};

class FdcConfiguration : public IoGroupConfiguration
{
public:
    bool wd1770_defined;
    bool wd1793_defined;
    
    IoPort selectOutput;
    IoPort writeOutput;
    IoPort readInput;
    IoPort intrqInput;
    EfFlag ef;
    
    int drives;
    int tracks;
    int heads;
    int sectors;
    int sectorLength;
    int maxFmtCount;
    int clearAddress;
    
    int cyclesPerMilliSecond;
};

class Tu58Configuration : public IoGroupDefineConfiguration
{
public:
    int numberOfBlocks[4];
};

// Keyboard configuration class definitions:

class AsciiKeyboardConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort input;
    EfFlag ef;

};

class Cdp1871Configuration : public IoGroupDefineConfiguration
{
public:
    IoPort input;
    EfFlag ef;
    EfFlag repeatEf;

};

class Ps2KeyboardConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort input;
    IoPort output;
    EfFlag ef;

    bool interrupt;
};

class GpioPs2KeyboardConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort input;
    IoPort output;
    EfFlag ef;

    wxString startupString;
    int textKey[LAST_MATRIX_TEXT_KEY];

    bool jp4;
    bool interrupt;
    int picInterrupt;
    int beepFrequency;
};

class MatrixKeyboardConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort input;
    IoPort output;
    Byte efKey[LAST_MATRIX_EF_KEY];
    Byte efKeyRev[LAST_MATRIX_EF_KEY];

    int keyValue[256];
    Byte bitValue[256];
    Byte shiftValue[256];
    Byte ctrlValue[256];
    KeyDefinition textKey[LAST_MATRIX_TEXT_KEY];
    Byte keyPressed;
    bool useAddress;
};

class BitKeypadConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort input;

    wxString number;
    bool repeat;
    vector<BitKey> bitkey;
    vector<BitModKey> bitModkey;
    int numberOfKeys;
    int numberOfModKeys;
    Byte bitKeyPressed;
};

class CvKeypadConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort input;
    EfFlag ef;

    int keydef[256];
    int textKey[LAST_MATRIX_TEXT_KEY];
    int modKey;
    int modKeyIgnore;
    Word padMask;
};

class EtiKeypadConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort input;
    IoPort output;
    EfFlag ef;
};

class FredKeypadConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
    IoPort inputKey;
    IoPort inputCoin;
    EfFlag efKey;
    EfFlag efFireB;
    EfFlag efCoin;

    int input_mode;
    int keypad_mode;
    bool coinArcadeControl;
    int ef_mode;
};

class KeyLatchConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
    EfFlag ef;

    wxString padNumberStr;
    int pc[512];
    int pcShift[512];
    int pcForceShift[512];
    int pcForceNoShift[512];
    int pcMapShift[512];
    int pcMap[512];
    vector<DiagonalKeys> diagonalKeys;

    int altRight;
    int altLeft;
    int ctrlRight;
    int ctrlLeft;
    int shift;
    int caps;
    int switchAltCtrl;
    bool loadAutoConfig;
};

class EfButtonsConfiguration : public DefineConfiguration
{
public:
    EfKey key[5];
    vector<EfButton> efButton;
    Byte keyPressed;
    bool multiMode;
};

// SCN2671 configuration class definitions:

class Scn2671Configuration : public IoGroupConfiguration
{
public:
    IoPort command;
    IoPort isr;
    IoPort cmr;
    IoPort txhr;
    IoPort rxhr;
    IoPort brr;
    IoPort csr;
    IoPort imr;
    IoPort kmr;
    IoPort khr;
    IoPort ksr;
    IoPort commandMisc;
        
    bool interrupt;
    int picInterrupt;
    
    int baudR;
    int baudT;
    double baudCorrectionR;
    double baudCorrectionT;
    
    int connection;
    
    KeyDefinition textKey[LAST_MATRIX_TEXT_KEY];
    bool vt4801coding;
};

// CDP1854 configuration class definitions:

class Cdp1854Configuration : public IoGroupConfiguration
{
public:
    IoPort out;
    IoPort in;
    IoPort control;
    IoPort status;
    EfFlag ef;
    EfFlag efInterrupt;
    
    bool interrupt;
    bool threUnchangedAtControl;
    int picInterrupt;
    
    int baudR;
    int baudT;
    double baudCorrectionR;
    double baudCorrectionT;
    
    int connection;
};

// CDP1855 configuration class definitions:

class Cdp1855Configuration : public IoGroupDefineConfiguration
{
public:
    IoPort x;
    IoPort y;
    IoPort z;
    IoPort control;
    IoPort status;
    
    EfFlag ef;
    int maxNumberOfMdu;
};

// CDP1877 configuration class definitions:

class Cdp1877Configuration : public IoGroupConfiguration
{
public:
    IoPort readStatus;
    IoPort readPolling;
    IoPort readVector;
    IoPort writeMask;
    IoPort writeControl;
    IoPort writePage;
    
    IoPort readIrq;
    IoPort writeMien;
};

// PIO configuration class definitions:

class Cdp1851Configuration : public IoGroupConfiguration
{
public:
    IoPort writePortA;
    IoPort writePortB;
    IoPort readPortA;
    IoPort readPortB;
    IoPort writeControl;
    IoPort readStatus;
    EfFlag efaRdy;
    EfFlag efbRdy;
    EfFlag efIrq;
    
    bool windowOpen;
    wxPoint pos;
    wxPoint defaultPos;
    int picInterrupt;
    
    Byte initPortA;
    Byte initPortB;
    
    int connection;
};

class Cdp1852Configuration : public IoGroupConfiguration
{
public:
    IoPort writePort;
    IoPort readPort;
    EfFlag efStb;
    
    bool windowOpen;
    wxPoint pos;
    wxPoint defaultPos;
    
    int picInterrupt;
};

// Timer configuration class definitions:

class Cdp1878Configuration : public IoGroupDefineConfiguration
{
public:
    IoPort counterHighA;
    IoPort counterLowA;
    IoPort controlA;
    IoPort counterHighB;
    IoPort counterLowB;
    IoPort controlB;
    IoPort interruptA;
    IoPort interruptB;
    EfFlag ef;
    
    double clockA;
    double clockB;
    int picInterrupt;
};

class Cd4536bConfiguration : public IoGroupConfiguration
{
public:
    IoPort writeControl;
    EfFlag ef;
};

// MM57109 configuration class definitions:

class Mm57109Configuration : public IoGroupDefineConfiguration
{
public:
    IoPort input;
    IoPort output;

    EfFlag ef;
    double clock;
};

// Printer configuration class definitions:

class BasicPrinterConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
    EfFlag ef;
    
    Byte reversePolarityOutput;
};

class ParallelPrinterConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
    IoPort input;
};

class SerialPrinterConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
    IoPort input;
    
    int bits;
    int parity;
};

class QSerialPrinterConfiguration : public IoGroupDefineConfiguration
{
public:
    EfFlag ef;
};

class CentronicsPrinterConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
    EfFlag ef;
};

class ThermalPrinterConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
    IoPort input;
    EfFlag ef;
};

class Cdp1851PrinterIo
{
public:
    Byte active;
    int bitNumber;
    int portNumber;
};

class Cdp1851PrinterConfiguration : public IoGroupDefineConfiguration
{
public:
    double busyCycleTime;
    double ackCycleTime;
    
    int portFunction[2];
    int portRdyFunction[2];
    int portStrobeFunction[2];
    
    Byte ackMask;
    Byte busyMask;
    Byte selectInMask;
    Byte errorMask;
    Byte paperOutMask;

    Cdp1851PrinterIo init;
    Cdp1851PrinterIo selectOut;
    Cdp1851PrinterIo strobe;
    Cdp1851PrinterIo autoLf;
    Cdp1851PrinterIo busy;
    Cdp1851PrinterIo ack;
    Cdp1851PrinterIo selectIn;
    Cdp1851PrinterIo error;
    Cdp1851PrinterIo paperOut;
};

// Memory configuration class definitions:

class MemoryConfiguration
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
    bool checkFwList;
    Word start;
    Word end;
    Word memMask;
    bool useMemMask;
    int type;
    LoadOffSet loadOffSet;
    int cartType;
    bool mcr;
    int mcrMemNumber;

    vector<Word> mappingList;
};

class MemoryMapperConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort selectOutput;
    IoPort writeOutput;
    IoPort input;
    
    int maskBits;
    Word mask;
    Byte startPort;
    Byte endPort;
};

class EmsMemoryConfiguration : public IoGroupConfiguration
{
public:
    IoPort output;

    wxString dirname;
    wxString filename;
    vector <WordRange> range;
    Word startAddress;
    Word mask;
    Word outputStart;
    Word outputEnd;
    int maskBits;
    int type;
    int emsType;
    Byte page;
};

class NvRamConfiguration : public DefineConfiguration
{
public:
    bool disable;
};

class NvRamMpConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort input;
    
    bool followMpSwitch;
};

class MemoryPartConfiguration
{
public:
    Word start;
    Word end;
    Word copy;
    int slot;
};

class SlotConfiguration
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
    bool banksInUse_;
};

class McrConfiguration
{
public:
    IoPort output;
    IoPort bbat;

    int maxMapNumber_;
    vector<MapInfo> mapInfo;
    Word start;
    Word end;
    
    Word ioStart;
    Word ioEnd;
    int ioMemNumber;
};

// HEX & LED Display and Keypad configuration class definitions:

class HexDisplayConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
    IoPort input;
    EfFlag ef;
    EfFlag inEf;
    EfFlag inEfElf;
    EfFlag startEf;
    
    bool oneKeyIn;
};

class MultiSegDisplayConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort segOutput;
    IoPort segInput;
    EfFlag segEf;

    bool multitil[MAX_MULTI_TIL];
    int multitilAddressLine[MAX_MULTI_TIL];
    int multitilAddressLineWait;
    int multitilAddressLineHigh;

    int multiTilNumber;
    int cycleValue;

    wxString tilFontDirectory;
    wxString tilFontFile;
    bool reversePolarity;
};

class LedDisplayConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
    
    bool outtil[8][2];
    bool outtilDpQ[8][2];
    bool datatil[MAX_DATA_TIL];
    bool datatilDpQ[MAX_DATA_TIL];
    bool addresstil[MAX_ADDRESS_TIL];
    
    bool followButton;
    bool followSwitches;
    bool showDataOnLoad;
    bool showDataOnCycle;
    bool showAddressOnCycle;
};

// Tape configuration class definitions:

class SwTapeConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
    IoPort outputQ;
    EfFlag ef;
    EfFlag efRun;
    
    bool twoDecks;
    bool keyClear;
    
    int stopDelay;
    int endDelay;
    bool flipq;
};

class HwTapeConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
    IoPort input;
    IoPort outputMode;
    IoPort outputSound;
    EfFlag ef;
    EfFlag efRun;
    EfFlag efBufferEmpty;
    EfFlag efError;

    bool twoDecks;
    
    int format;
    int stopDelay;
    
    int frequencyBorder;
    int frequency0;
    int frequency1;
    
    int startBit;
    int stopBit;
    bool stopBitIgnore;
    int dataBits;
    
    bool audioChannelLeft;
    bool dataChannelLeft;

    bool stopTone;
    bool startOnReset;

    char threshold8Bit;
    wxInt16 threshold16Bit;
    wxInt32 threshold24Bit;
};

// Boot and Bootstrap configuration class definition:

class BootstrapConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
    IoPort input;

    Word address;
    int type;

    int timer;
};

class AutoBootConfiguration : public DefineConfiguration
{
public:
    long address;
    int dmaOutOnBoot;
    int dmaOutOnBootIfMem0is0;
};

// RTC configuration class definitions:

class RtcM48t58Configuration : public IoGroupDefineConfiguration
{
public:
    Word control;
    Word second;
    Word minute;
    Word hour;
    Word day;
    Word date;
    Word month;
    Word year;
};

class RtcCdp1879Configuration : public IoGroupDefineConfiguration
{
public:
    EfFlag ef;

    Word base;
    Word freeze;
    Word control;
    Word second;
    Word minute;
    Word hour;
    Word date;
    Word month;
    
    bool interrupt;
    int picInterrupt;
};

class RtcDs12887Configuration : public IoGroupDefineConfiguration
{
public:
    IoPort outputSelectPort;
    IoPort outputWritePort;
    IoPort input;
};

// DIP configuration class definition:

class DipConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort input;
    
    int value;
};

// SuperBoard configuration class definition:

class SuperBoardConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort input5;
    IoPort input6;
    IoPort output;
    EfFlag ef;
};

// DiagBoard configuration class definition:

class DiagnosticBoardConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort input1;
    IoPort input2;
    IoPort output;
    
    int checksum;
    int factory;
    int cassetteCables;
    
    bool active;
};

// Address Location configuration class definitions:

class AddressLocationConfiguration
{
public:
    vector<Word> keyInputAddress;
    vector<Word> inReleaseAddress;
    vector<WriteAddress> writeAddress;
    vector<ReadAddress> readAddress;
    int code_start;
    int code_start_high;
    int code_start_low;
    int code_end_high;
    int code_end_low;
    int reg_end;
    int reg_end_adjust;
    vector<LocationInfo> locationInfo;
};

class LocationTriggerConfigurations
{
public:
    vector<signed char> index;
};

// Assembler & Debugger class definitions:

class AssemblerConfiguration
{
public:
    bool directoryName_defined;
    bool fileName_defined;

    wxString directoryName;
    wxString fileName;

    int code_start;
    int code_end;
    int end;
};

class DebuggerConfiguration
{
public:
    bool mode;
    long callRegister;
    long callAddress;
    long returnRegister;
    long returnAddress;
    
    bool videoLog_active;
    bool videoLog_defined;
};

class TraceInstructionSepDetails
{
public:
	int sepType;
	Byte registerValue;
	wxString stringValue;
	Byte offset;
};

class CheckDetails
{
public:
	Byte mask;
	Byte value;
};

class SepConfiguration
{
public:
    Word sepAddress;
    Byte sepRegister;
    int bytes;
	CheckDetails pcByte;
    CheckDetails preByte;
	vector<TraceInstructionSepDetails> details;
};

// File & directory configuration class definitions:

class FileConfiguration
{
public:
    wxString directory;
    wxString fileName;
};

class WavConfiguration : public FileConfiguration {};

class BatchConfiguration : public FileConfiguration {};

class CharacterRomConfiguration : public FileConfiguration {};

class KeyFileConfiguration : public FileConfiguration {};

class ScreenDumpFileConfiguration : public FileConfiguration {};

class PrinterFileConfiguration : public FileConfiguration {};

class XmlFileConfiguration : public FileConfiguration
{
public:
    wxString mainDirectory;
    wxString subDirectory;
};

// Other configuration class definitions:

class Ct2425Configuration : public IoGroupConfiguration
{
public:
    IoPort validators;
    IoPort gates;
    IoPort control;
    IoPort sensors;
    IoPort coins;
    EfFlag efSpm;

    int ignore_led;
    int before_1;
    int block_1;
    int between_1_2;
    int block_2;
    int between_2_x;
    int after_gate_5_6;
    int block_5;
    int spm_interval;
    int spm_pulse;

    int sensor_after_gate_1;
    int sensor_after_gate_2;
    int sensor_upper_escrow;
    int sensor_lower_escrow;
    int sensor_return;
    
    int gate_first;
    int gate_second;
    int gate_upper_escrow_cash;
    int gate_lower_escrow_cash;
    int gate_upper_escrow_return;
    int gate_lower_escrow_return;
};

class Ct2425CoinConfiguration
{
public:
    wxString filename;
    wxString coinString[6];
};

class DefaultEfConfiguration
{
public:
    int ef1;
    int ef2;
    int ef3;
    int ef4;
};

class MonitorConfiguration : public IoGroupDefineConfiguration
{
public:
    EfFlag ef;
};

class AdConvertorConfiguration : public IoGroupDefineConfiguration
{
public:
    IoPort output;
    IoPort input;
    
    int addressStart;
    int addressEnd;
};

class BasicConfiguration
{
public:
    char pLoadSaveName[4];
    WordDefineConfiguration defus;
    WordDefineConfiguration eop;
    WordDefineConfiguration string;
    WordDefineConfiguration array;
    WordDefineConfiguration eod;
    WordDefineConfiguration ramAddress;
};

class DmaConfiguration : public DefineConfiguration
{
public:
    int cycleValue;
    
    bool startOnFirstQ;
    int startOnFirstOut;
};

class InterruptConfiguration : public DefineConfiguration
{
public:
    int cycleValue;
};

class GuiItemConfiguration
{
public:
    int type;           // PUSH_BUTTON_ROUND_RED or SWITCHBUTTON
    int function;       // card, reset, run etc.
    bool actOnRelease;
    bool actOnPress;
    bool initup;        // BUTTON_DOWN (false) or BUTTON_UP (true)
    wxString label;
    wxString label2;
    wxPoint position;
    wxSize size;
    int value;
    IoPort input;
    IoPort output;
    IoPort tilOutput;
    int rangeLow;
    int rangeHigh;
    int textSize;
    int systemColor1;
    int systemColor2;
    bool useImageDir;
    wxString fileName;
    wxString dirName;
    bool reversePol;
    
    wxColour color1;
    wxColour color2;
    
    wxBitmap *bitmapPointer;

    SwitchButton *switchButton;
    SwitchButton *dipSwitchButton;
    RotButton *rotButton;

    HexButton *hexButton;
    wxButton *windowsButton;
};

class MainFrontPanelConfiguration
{
public:
    bool dataSwitchButtons;
    bool dataPushButtons;
    
    bool powerButtonDefined;
    bool waitButtonDefined;
    bool nibbleLeds;
    
    int xBorder, yBorder;
};

class FrontPanelConfiguration : public DefineConfiguration
{
public:
    bool show;
    int version;
    wxSize size;
    
    wxPoint pos;
    wxPoint defaultPos;
    int posType;
    int picInterrupt;
    
    int frontPanelNumberBitText;

    vector<GuiItemConfiguration> guiItemConfiguration;
};

class GiantBoardConfiguration : public DefineConfiguration
{
public:
    Word base;
};

class MemAccessConfiguration
{
public:
    wxString directory;
    wxString fileName;
    wxString fullFileName;

    bool useLocation;
    Word saveStart;
    Word saveEnd;
    Word saveExec;
    wxString saveStartString;
    wxString saveEndString;
    wxString saveExecString;
};

class SplashConfiguration
{
public:
    wxString dialog;
    wxString text;
};

class FlipflopConfiguration: public IoGroupDefineConfiguration
{
public:
    IoPort output;
    EfFlag ef;
    
    int function[4];
};

class ComputerConfiguration
{
public:
    MainIoGroupConfiguration ioGroupConfiguration;
    
    // FucntionKey configurations
    ModKeyConfiguration modKeyConfiguration;

    // Video configurations
    CDP1861Configuration cdp1861Configuration;
    CDP1862Configuration cdp1862Configuration;
    CDP1864Configuration cdp1864Configuration;
    CoinConfiguration coinConfiguration;
    Vip2KVideoConfiguration vip2KVideoConfiguration;
    FredVideoConfiguration fredVideoConfiguration;
    Studio4VideoConfiguration studio4VideoConfiguration;
    TmsConfiguration tmsConfiguration;
    I8275Configuration i8275Configuration;
    Sn76430NConfiguration sn76430NConfiguration;
    Scn2672Configuration scn2672Configuration;
    Crt8002Configuration crt8002Configuration;
    Mc6845Configuration mc6845Configuration;
    Mc6847Configuration mc6847Configuration;
    Vis1870Configuration vis1870Configuration;
    
    // Terminal configurations
    VideoTerminalConfiguration videoTerminalConfiguration;
    
    // Sound configuration class definitions:
    SoundConfiguration soundConfiguration;
    CDP1863Configuration cdp1863Configuration;
    AY_3_8912Configuration ay_3_8912Configuration;
    BitSoundConfiguration bitSoundConfiguration;
    
    // Disk configurations
    Upd765Configuration upd765Configuration;
    IdeConfiguration ideConfiguration;
    FdcConfiguration fdcConfiguration;
    Tu58Configuration tu58Configuration;

    // Keyboard configurations
    AsciiKeyboardConfiguration asciiKeyboardConfiguration;
    Cdp1871Configuration cdp1871Configuration;
    Ps2KeyboardConfiguration ps2KeyboardConfiguration;
    GpioPs2KeyboardConfiguration gpioPs2KeyboardConfiguration;
    MatrixKeyboardConfiguration matrixKeyboardConfiguration;
    CvKeypadConfiguration cvKeypadConfiguration;
    EtiKeypadConfiguration etiKeypadConfiguration;
    FredKeypadConfiguration fredKeypadConfiguration;
    BitKeypadConfiguration bitKeypadConfiguration[MAX_BITKEYPADS];
    KeyLatchConfiguration keyLatchConfiguration[MAX_LATCHKEYPADS];
    EfButtonsConfiguration efButtonsConfiguration;

    // PIO configurations:
    vector<Cdp1851Configuration> cdp1851Configuration;
    vector<Cdp1852Configuration> cdp1852Configuration;
    
    // UART configurations:
    vector<Cdp1854Configuration> cdp1854Configuration;
    vector<Scn2671Configuration> scn2671Configuration;
    
    // MDC configurations:
    Cdp1855Configuration cdp1855Configuration;
    Mm57109Configuration mm57109Configuration;

    // PIC configurations:
    vector<Cdp1877Configuration> cdp1877Configuration;
    
    // Timer configurations:
    vector<Cdp1878Configuration> cdp1878Configuration;
    vector<Cd4536bConfiguration> cd4536bConfiguration;
    
    // Printer configurations:
    BasicPrinterConfiguration basicPrinterConfiguration;
    ParallelPrinterConfiguration parallelPrinterConfiguration;
    SerialPrinterConfiguration serialPrinterConfiguration;
    QSerialPrinterConfiguration qSerialPrinterConfiguration;
    CentronicsPrinterConfiguration centronicsPrinterConfiguration;
    ThermalPrinterConfiguration thermalPrinterConfiguration;
    Cdp1851PrinterConfiguration cdp1851PrinterConfiguration;
    
    // Memory configurations:
    MemoryMapperConfiguration memoryMapperConfiguration;
    vector<EmsMemoryConfiguration> emsMemoryConfiguration;
    vector<MemoryConfiguration> memoryConfiguration;
    NvRamConfiguration nvRamConfiguration;
    NvRamMpConfiguration nvRamMpConfiguration;
    vector<MemoryPartConfiguration> memoryCopyConfiguration;
    vector<MemoryPartConfiguration> memoryRamPartConfiguration;
    SlotConfiguration slotConfiguration;
    McrConfiguration mcrConfiguration;

    // HEX & LED Display configurations:
    HexDisplayConfiguration hexDisplayConfiguration;
    MultiSegDisplayConfiguration multiSegDisplayConfiguration;
    LedDisplayConfiguration ledDisplayConfiguration;
    
    // Tape configurations:
    SwTapeConfiguration swTapeConfiguration;
    HwTapeConfiguration hwTapeConfiguration;
    
    // Boot and Bootstrap configuration:
    BootstrapConfiguration bootstrapConfiguration;
    AutoBootConfiguration autoBootConfiguration;
    
    // RTC configurations:
    RtcCdp1879Configuration rtcCdp1879Configuration;
    RtcM48t58Configuration rtcM48t58Configuration;
    RtcDs12887Configuration rtcDs12887Configuration;
    
    // DIP configurations:
    vector<DipConfiguration> dipConfiguration;
//    int numberOfDipInstances_;

    // SuperBoard configurations:
    SuperBoardConfiguration superBoardConfiguration;
    
    // DiagBoard configurations:
    DiagnosticBoardConfiguration diagnosticBoardConfiguration;
   
    // Address Location configurations:
    AddressLocationConfiguration addressLocationConfiguration;
    vector<LocationTriggerConfigurations> locationTriggerConfiguration;

    // Assembler & Debugger configurations:
    vector<AssemblerConfiguration> assemblerConfiguration;
    DebuggerConfiguration debuggerConfiguration;
    vector<SepConfiguration> sepConfiguration;

    // File & directory configurations:
    WavConfiguration wavConfiguration[2];
    BatchConfiguration batchConfiguration;
    CharacterRomConfiguration characterRomConfiguration;
    KeyFileConfiguration keyFileConfiguration;
    ScreenDumpFileConfiguration screenDumpFileConfiguration;
    PrinterFileConfiguration printerFileConfiguration;
    XmlFileConfiguration xmlFileConfiguration;

    // Other configurations:
    vector<Ct2425Configuration> ct2425Configuration;
    vector<Ct2425CoinConfiguration> ct2425CoinConfiguration;
    DefaultEfConfiguration defaultEfConfiguration;
    MonitorConfiguration monitorConfiguration;
    AdConvertorConfiguration adConvertorConfiguration;
    BasicConfiguration basicConfiguration;
    DmaConfiguration dmaConfiguration;
    InterruptConfiguration interruptConfiguration;
    MainFrontPanelConfiguration mainFrontPanelConfiguration;
    vector<FrontPanelConfiguration> frontPanelConfiguration;
    GiantBoardConfiguration giantBoardConfiguration;
    MemAccessConfiguration memAccessConfiguration;
    SplashConfiguration splashConfiguration;
    FlipflopConfiguration flipflopConfiguration;

    // Folder and file location configuration items
    wxString mainDir_;
        
    // Keyboard and Keypad related configuration items
    bool bitKeypadEf[5];
    wxString keyPadDefinitionFile;
    int runPressType;
    int resetPressType;
    int stepPressType;
    bool useHexKeyboard;
    bool forceUpperCase;
    int numberOfPadKeys;
    wxString defaultKeyDefinition[2];

    // CPU and I/O related configuration items
    wxString clock_;
    double clockSpeed_;
    wxString stateCommand_[LAST_COMMAND_STATE];
    Byte ioMask[LAST_IO_DEFINITION];
    bool f12reset;
    
    // GUI related configuration items
    wxString ledTime_;
    long ledTimeMs_;
    bool useSplashScreen_;
    bool clearRam;
    bool clearRtc;
            
    // Memory related configuration items
    int multicartEmsNumber_;
    int memoryMask;
    
    // DMA related configuration items
    bool dmaInReset_defined;
    
    // Cassette related configuration items
    bool autoCassetteLoad_;
    bool turbo_;
    bool useBatchWav_;
    wxString saveCommand_;
    bool realCassetteLoad_;
    wxString turboClock_;
    
    // FDC related configuration items
    int fdcType_;
    
    // Printer related configuration items
    int printMode_;
    bool printerOn_;
    
    // Video related configuration items
    int videoNumber_;
    int numberOfVideoTypes_;
    wxString videoName_[VIDEOXMLMAX];
    wxString zoom_[VIDEOXMLMAX];
    bool interlace_;
};

#endif // COMPUTERCONFIG_H

#ifndef ELFCONFIG_H
#define ELFCONFIG_H

#define BOOTSTRAPREAD 0
#define BOOTSTRAPOUT 1
#define BOOTSTRAPIN 2
#define BOOTSTRAPIO 3
#define BOOTSTRAPNONE 4

class EfKey
{
public:
    int value;
    bool defined;
};

class BitKey
{
public:
    int value;
    int bitMaskPressed;
    int bitMaskReleased;
};

class BitKeypadDetails
{
public:
    wxString number;
    bool defined;
    int inp;
    int ioGroup;
    vector<BitKey> bitkey;
    int numberOfKeys;
};

class CvKeypadDetails
{
public:
    bool defined;
    int inp;
    int ef;
    bool reversed;
    int ioGroup;
    int keydef[256];
};

class IoPort
{
public:
    int portNumber;
    int qValue;
};

class KeyDefinition
{
public:
    Word keyValue;
    Byte bitMaskPressed;
    Byte bitMaskNotPressed;
};

class IoConfiguration
{
public:
    int ef1default;
    int ef2default;
    int ef3default;
    int ef4default;

    bool pixieDoubleScreenIo;
    int pixieInput;
    int pixieOutput;
    int pixieEf;
    int pixieVideoNumber;

    IoPort cdp1864enable;
    IoPort cdp1864disable;
    IoPort cdp1864toneLatch;
    int cdp1864Ef;
    int cdp1864VideoNumber;
    int cdp1864IoGroup;

    int portExtenderSelectOutput;
    int portExtenderWriteOutput;
    int portExtenderInput;

    int ideSelectOutput;
    int ideWriteOutput;
    int ideStatus;
    int ideInput;
    int ideTracks;
    int ideHeads;
    int ideSectors;
    int ideIoGroup;

    IoPort fdcSelectOutput;
    IoPort fdcWriteOutput;
    IoPort fdcReadInput;
    IoPort fdcIntrqInput;
    int fdcEf;
    int fdcDrives;
    int fdcSides;
    int fdcTracks;
    int fdcSectors;
    int fdcSectorLength;
    int fdcMaxFmtCount;
    int fdcIoGroup;

    int keyboardInput;
    int keyboardEf;
    int keyboardRepeatEf;

    int ps2KeyboardInput;
    int ps2KeyboardOutput;
    int ps2KeyboardEf;
    
    int gpioInput;
    int gpioOutput;
    int gpioEf;
    int gpioIoGroup;

    int printerIoGroup;
    int printerOutput;
    int printerEf;
    
    int parallelPrinterIoGroup;
    int parallelPrinterOutput;
    int parallelPrinterInput;

    int serialPrinterIoGroup;
    int serialPrinterOutput;
    int serialPrinterInput;
    int serialPrinterBits;
    int serialPrinterParity;

    int qSerialPrinterIoGroup;
    int qSerialPrinterEf;
    
    int thermalPrinterIoGroup;
    int thermalPrinterOutput;
    int thermalPrinterInput;
    int thermalPrinterEf;

    vector<int> emsOutput;

    int vt100Output;
    int vt100Ef;
    int vt100ReverseEf;
    int vt100ReverseQ;

    int uartOut;
    int uartIn;
    int uartControl;
    int uartStatus;
    int uartIoGroup;

    int tmsModeHighOutput;
    int tmsModeLowOutput;
    int tmsInterrupt;
    int tmsVideoNumber;

    int i8275WriteCommand;
    int i8275ReadStatus;
    int i8275WriteParameter;
    int i8275ReadParameter;
    int i8275VerticalRetrace;
    int i8275VideoNumber;

    int SN76430NVideoNumber;

    int led_Module_Output;

    int mc6847OutputMode;
    int mc6847Output;
    Word mc6847OutputStart;
    Word mc6847OutputEnd;
    int mc6847b7;
    int mc6847b6;
    int mc6847b5;
    int mc6847b4;
    int mc6847b3;
    int mc6847b2;
    int mc6847b1;
    int mc6847b0;
    int mc6847dd7;
    int mc6847dd6;
    int mc6847VideoNumber;

    bool forceHighAg;
    bool forceHighAs;
    bool forceHighExt;
    bool forceHighGm2;
    bool forceHighGm1;
    bool forceHighGm0;
    bool forceHighCss;
    bool forceHighInv;

    int mc6847StartRam;
    int mc6847EndRam;

    int mc6845StartRam;
    int mc6845EndRam;
    Word mc6845RamMask;
    int mc6845Address;
    int mc6845AddressMask;
    int mc6845Data;
    int mc6845DataMask;
    int mc6845Ef;
    wxSize mc6845CharSize;
    wxSize mc6845ScreenSize;
    int mc6845IoGroup;
    int mc6845VideoNumber;

    int hexOutput;
    int hexInput;
    int hexEf;
    
    int inEf;
    
    int bootStrapIn;
    int bootStrapOut;
    int bootStrapType;
    
    int dipIn;
    int dipValue;

    int ioGroupOut;
    Byte ioGroupMask;

    int tapeEf;
    IoPort tapeIn;
    int tapeEfOut;
    IoPort tapeOut;
    int tapeIoGroup;

    int v1870ioGroup;
    int v1870interruptMode;
    int v1870videoMode;
    int v1870pageMemSize;
    bool v1870pageMemRom;
    int v1870charMemSize;
    bool v1870charMemRom;
    bool v1870charRomFormatTmc;
    int v1870pcbMask;
    int v1870maxCharLines;
    int v1870maxScreenLines;
    int v1870ef;
    bool v1870efRev;
    int v1870videoModeEf;
    bool v1870useVideoModeEf;
    double v1870clock;
    Word v1870charRomStart;
    bool v1870cmaMaskFixed;
    bool v1870rotate;
    int v1870colorRamType;
    bool v1870cursorBlink;
    int v1870outWrite;
    int v1870outSelect;
    int v1870VideoNumber;

    int statusBarType;

    IoPort bitSoundOut;
    int bitSoundIoGroup;
    int bitSoundMask;

    EfKey efKey[5];
    Byte efKeyPressed;
    
    BitKeypadDetails bitKeypad[2];
    Byte bitKeyPressed;
    
    CvKeypadDetails cvKeypad;
    int CvKeypadTextKey[LAST_MATRIX_TEXT_KEY];

    int nvRamProtectIn;
    int nvRamIoGroup;
    
    Word rtcControl;
    Word rtcSecond;
    Word rtcMinute;
    Word rtcHour;
    Word rtcDay;
    Word rtcDate;
    Word rtcMonth;
    Word rtcYear;
    int rtcOut;
    int rtcIn;
    int rtcSelect;
    int rtcIoGroup1;
    int rtcIoGroup2;

    IoPort usbSbIn5;
    IoPort usbSbIn6;
    IoPort usbSbOut;
    int usbSbEf;
    int usbSbIoGroup;
    
    int diagIn1;
    int diagIn2;
    int diagOut;
    
    int keybLatchPc[512];
    int keybLatchPcShift[512];
    bool useKeyDefShift;
    Byte keybLatchPressed;
    int keybLatchIoGroup;
    int keybLatchOut;
    int keybLatchOutMask;
    int keybLatchEf;
    int keybLatchAltRight;
    int keybLatchAltLeft;
    int keybLatchCtrlRight;
    int keybLatchCtrlLeft;
    int keybLatchShift;
    int keybLatchCaps;
    int keybLatchSwitch;
    
    int keybMatrixKeyValue[256];
    Byte keybMatrixBitValue[256];
    Byte keybMatrixShiftValue[256];
    Byte keybMatrixCtrlValue[256];
    int keybMatrixIn;
    int keybMatrixInMask;
    int keybMatrixIoGroup;
    Byte keybMatrixEfKey[LAST_MATRIX_EF_KEY];
    bool keybMatrixEfKeyRev[LAST_MATRIX_EF_KEY];
    KeyDefinition keybMatrixTextKey[LAST_MATRIX_TEXT_KEY];
    Byte keybMatrixPressed;
};

class ElfConfiguration
{
public:
    bool usePixie;
    bool use1864;
    bool useS100;
    bool use6845;
    bool use6847;
    int charLine;
    int screenHeight6847;
    bool useTMS9918;
    bool use8275;
    bool usev1870;
    bool useSN76430N;
    bool useNvRamMp;
    bool useNvRam;
    bool useRtcM48T58;
    bool useRtcDS12887;
    bool useUsbSb;

    bool useDip;
    bool useIoGroup;

    bool usePio;
    bool useCdp18s660;
    bool usePioWindow1Cdp18s660;
    bool usePioWindow2Cdp18s660;
    bool useUpd765;
    int upd765Group;
    int printerGroup;
    int cdp18s660Group1;
    int cdp18s660Group2;
    int fdcType_;
    int vtType;
    int vtCharactersPerRow;
    int vt100CharWidth;
    int vt52CharWidth;
    bool vtExternal;
    bool serialLog;
    bool escError;
    int bellFrequency_;
    wxString vtWavFileDir_;
    wxString vtWavFile_;
    wxString vt100CharRomDir_;
    wxString vt100CharRom_;
    wxString vt52CharRomDir_;
    wxString vt52CharRom_;
    wxString serialPort_;
    bool useUart;
    bool useUart16450;
    int uartGroup;
    int baudR;
    int baudT;
    bool autoBoot;
    int autoBootType;
    bool stopTone;
    bool utilityMemory;
    bool tapeStart;
    int tape_threshold8Bit;
    int tape_threshold16Bit;
    int tape_frequencyBorder;
    int tape_frequency0;
    int tape_frequency1;
    int tape_startBit;
    int tape_stopBit;
    int tape_dataBits;
    int tape_stopDelay;
    bool tape_audioChannelLeft;
    bool tape_dataChannelLeft;
    bool tape_revInput;
    bool rtc;
    bool nvr;
    bool useSwitch;
    bool useHex;
    bool useTape;
    bool useTape1;
    bool useTapeHw;
    bool useXmodem;
    bool useHexModem;
    int packetSize;
    bool vtShow;
    bool autoKeyDef;

    int keyboardType;
    bool useHexKeyboard;
    bool useHexKeyboardEf3;
    bool useInButton;
    bool useKeyb1871;
    bool useKeyboard;
    bool useBitKeypad;
    bool useCvKeypad;
    bool useLatchKeyboard;
    bool useMatrixKeyboard;
    bool usePS2;
    bool ps2Interrupt;
    bool gpioJp4;
    bool usePs2gpio;
    bool useDma;
    bool useInt;

    int intCycleValue;
    
    int dmaCycleValue;
    bool dmaOnFirstQ;
    int dmaOnFirstOut;

    int diskType;
    bool ideEnabled;
    bool fdc1793Enabled;
    bool fdc1770Enabled;

    int memoryType;
    bool usePortExtender;
    bool usePager;
    bool useEms;
    int emsType_;
    bool useLedModule;
    bool useElfControlWindows;
    bool showAddress;

    bool bootStrap;
    Word strapAddress;
    
    bool bootRam;
    bool clearRam;
    bool clearRtc;

    int ioType;
    int frontType;

    bool giantBoardMapping;
    bool efButtons;

    bool forceUpperCase;
    int qSound_;
    int tilType;
    
    bool useBitSound;

    bool vtEf;
    bool vtQ;
    bitset<32> vt52SetUpFeature_;
    bitset<32> vt100SetUpFeature_;
    bitset<32> vtExternalSetUpFeature_;

    int tapeFormat_;
    bool coinArcadeControl_;
    
    int panelType_;
    
    IoConfiguration ioConfiguration;
};

#endif    // ELFCONFIG_H

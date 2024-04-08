/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#if !wxUSE_COMBOCTRL
    #error "Please set wxUSE_COMBOCTRL to 1 and rebuild the library."
#endif

#include "wx/xrc/xmlres.h"
#include "wx/spinctrl.h"

#include "main.h"
#include "xmlparser.h"

BEGIN_EVENT_TABLE(XmlParser, XmlParser)

END_EVENT_TABLE()

wxString textKeyList[]=
{
    "return",
    "end",
    "home",
    "down",
    "left",
    "right",
    "up",
    "esc",
    "back",
    "undefined"
};

XmlParser::XmlParser(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiMain(title, pos, size, mode, dataDir, iniDir)
{
}

void XmlParser::clearXmlData(int computer)
{    
    wxString number, type;
    
    conf[computer].keyFile_ = configPointer->Read("/Xmlemu/Key_File", "");
    conf[computer].keyFileDir_ = readConfigDir("/Dir/Xmlemu/Key_File", conf[computer].mainDir_);
    conf[computer].screenDumpFile_ = configPointer->Read("/Xmlemu/Video_Dump_File", "screendump.png");
    conf[computer].screenDumpFileDir_ = readConfigDir("/Dir/Xmlemu/Video_Dump_File", conf[computer].mainDir_);
    conf[computer].printFile_ = configPointer->Read("/Xmlemu/Print_File", "printerout.txt");
    conf[computer].printFileDir_ = readConfigDir("/Dir/Xmlemu/Print_File", conf[computer].mainDir_);
    conf[computer].xmodemFile_ = configPointer->Read("/Xmlemu/Xmodem_File", "");
    conf[computer].xmodemFileDir_ = readConfigDir("/Dir/Xmlemu/Xmodem_File", conf[computer].mainDir_);

    for (int fdcType = 0; fdcType<FDCTYPE_MAX; fdcType++)
    {
        type.Printf("%d", fdcType);
        
        for (int disk=0; disk<4; disk++)
        {
            number.Printf("%d", disk);

            floppy_[fdcType][disk] = configPointer->Read("/Xmlemu/FDC" + number + "_File_" + type, "");
            floppyDir_[fdcType][disk] = readConfigDir("/Dir/Xmlemu/FDC" + number + "_File_" + type, conf[computer].mainDir_);
        }
    }
    for (int tape=0; tape<2; tape++)
    {
        number.Printf("%d", tape);
        
        conf[computer].wavFile_[tape] = configPointer->Read("/Xmlemu/Wav_File" + number, "");
        conf[computer].wavFileDir_[tape] = readConfigDir("/Dir/Xmlemu/Wav_File" + number, conf[computer].mainDir_);
    }
}

void XmlParser::parseXmlFile(int computer, wxString xmlDir, wxString xmlFile)
{
    wxString tagList[]=
    {
        "system",
        "bootstrap",
        "frontpanel",
        "memory",
        "disk",
        "video",
        "keyboard",
        "keypad",
        "bitkeypad",
        "xlkeypad",
        "vt",
        "printer",
        "cassette",
        "xmodem",
        "hexmodem",
        "efbuttons",
        "sound",
        "dip",
        "iogroup",
        "rtc",
        "usb",
        "keyfile",
        "splash",
        "videodump",
        "memaccess",
        "gui",
        "basic",
        "locations",
        "diagnostic",
        "batchwav",
        "gpio",
        "cdp1851",
        "cdp1852",
        "cd4536b",
        "ad_convertor",
        "debugger",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_SYSTEM,
        TAG_BOOTSTRAP,
        TAG_PANEL,
        TAG_MEMORY,
        TAG_DISK,
        TAG_VIDEO,
        TAG_KEYBOARD,
        TAG_KEYPAD,
        TAG_BITKEYPAD,
        TAG_CVKEYPAD,
        TAG_VT,
        TAG_PRINTER,
        TAG_CASSETTE,
        TAG_XMODEM,
        TAG_HEXMODEM,
        TAG_EFBUTTONS,
        TAG_SOUND,
        TAG_DIP,
        TAG_IOGROUP,
        TAG_RTC,
        TAG_USB,
        TAG_KEYFILE,
        TAG_SPLASH,
        TAG_VIDEODUMP,
        TAG_MEMACCESS,
        TAG_GUI,
        TAG_BASIC,
        TAG_LOCATIONS,
        TAG_DIAG,
        TAG_BATCHWAV,
        TAG_GPIO,
        TAG_CDP1851,
        TAG_CDP1852,
        TAG_CD4536B,
        TAG_AD_CONVERTOR,
        TAG_DEBUGGER,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    if (!wxFile::Exists(xmlDir + xmlFile) || xmlFile == "" || xmlDir == "")
    {
        conf[computer].memConfigNumber_ = 2;
        conf[computer].memConfig_.resize(2);
        conf[computer].memConfig_[0].filename = "";
        conf[computer].memConfig_[1].filename = "";
        elfConfiguration[computer].fdcType_ = FDCTYPE_17XX;
        return;
    }
        
    wxFileName xmlFileName = wxFileName(xmlDir + xmlFile);
    wxDateTime newDate = xmlFileName.GetModificationTime();
    
    if (oldXmlFileName_ == xmlDir + xmlFile)
    {
        
        if (newDate.IsEqualTo(oldXmlDate_))
            return;
    }

/*    wxString computerFolder = xmlDir;
    wxString configName = xmlFile;
    size_t pathSep, dot;
    
    if (computerFolder.Right(1) == pathSeparator_)
        computerFolder = computerFolder.Left(computerFolder.Len()-1);
    
    pathSep = computerFolder.find_last_of(pathSeparator_) + 1;
    computerFolder = computerFolder.Right(computerFolder.Len()-pathSep);
    
    dot = configName.find_last_of(".");
    configName = configName.Left(dot);*/
  
    setCpuType("");

    computerInfo[computer].gui = "Xml";
    
    elfConfiguration[computer].autoBoot = false;
    elfConfiguration[computer].f12reset = true;
    elfConfiguration[computer].dmaOnBoot = false;
    elfConfiguration[computer].dmaOnBoot0 = false;
    conf[computer].bootAddress_ = 0;
    elfConfiguration[computer].ioConfiguration.efMonitor = -1;

    elfConfiguration[computer].bootStrap = false;
    elfConfiguration[computer].useDma = false;
    elfConfiguration[computer].useInt = false;
    elfConfiguration[computer].ioConfiguration.statusBarType = STATUSBAR_NONE;
    elfConfiguration[computer].ioConfiguration.statusBarLedOut = -1;

    elfConfiguration[computer].ioConfiguration.keyPadDefinitionFile = "keydefinition.txt";
    elfConfiguration[computer].ioConfiguration.keypadCheckMemConfig = 0;  // Check RAM

    elfConfiguration[computer].memoryMask = 0xffff;
    conf[computer].memConfigNumber_ = 2;
    conf[computer].memConfig_.resize(2);
    
    conf[computer].memConfig_[0].filename = "";
    conf[computer].memConfig_[0].start = 0;
    conf[computer].memConfig_[0].type = UNDEFINED;
    conf[computer].memConfig_[0].cartType = CART_BIN;
    conf[computer].memConfig_[1].filename = "";
    conf[computer].memConfig_[1].start = 0;
    conf[computer].memConfig_[1].type = UNDEFINED;
    conf[computer].memConfig_[1].cartType = CART_NONE;
    conf[computer].mainDir_ =  dataDir_;
    conf[computer].emsConfigNumber_ = 0;
    conf[computer].copyConfigNumber_ = 0;
    conf[computer].ramPartConfigNumber_ = 0;
    conf[computer].slotConfig_.maxSlotNumber_ = 0;
    conf[computer].useDiagnosticBoard_ = false;
    conf[computer].useBatchWav_ = false;
    elfConfiguration[computer].useNvRamMp = false;
    elfConfiguration[computer].useNvRam = false;
//    elfConfiguration[computer].nvRamDisableDefault = false;
    elfConfiguration[computer].useRtcM48T58 = false;
    elfConfiguration[computer].useRtcDS12887 = false;
    elfConfiguration[computer].useUsbSb = false;
    elfConfiguration[computer].usePager = false;
    elfConfiguration[computer].usePortExtender = false;
    elfConfiguration[computer].useEms = false;
    conf[computer].multicartEmsNumber_ = -1;
    elfConfiguration[computer].fdc1793Enabled = false;
    elfConfiguration[computer].fdc1770Enabled = false;
    elfConfiguration[computer].useUpd765 = false;
    elfConfiguration[computer].ideEnabled = false;
    elfConfiguration[computer].ioConfiguration.fdcIoGroup = -1;
    elfConfiguration[computer].ioConfiguration.ideIoGroup = -1;

    conf[computer].numberOfVideoTypes_ = 0;
    elfConfiguration[computer].useCoinVideo = false;
    elfConfiguration[computer].usePixie = false;
    elfConfiguration[computer].ioConfiguration.pixieEfScreenOn = false;
    elfConfiguration[computer].ioConfiguration.pixieHighRes = false;
    elfConfiguration[computer].ioConfiguration.pixieColorType = PIXIE_COLOR_DEFAULT;
    elfConfiguration[computer].ioConfiguration.videoWidth = 64;
    elfConfiguration[computer].ioConfiguration.cdp1862ColorType = PIXIE_COLOR_DEFAULT;
    elfConfiguration[computer].ioConfiguration.use1862 = false;
    elfConfiguration[computer].use1864 = false;
    elfConfiguration[computer].ioConfiguration.cdp1864EfScreenOn = false;
    elfConfiguration[computer].ioConfiguration.cdp1864ColorType = PIXIE_COLOR_DEFAULT;
    elfConfiguration[computer].useSt4Video = false;
    elfConfiguration[computer].useVip2KVideo = false;
    elfConfiguration[computer].useFredVideo = false;
    conf[computer].xScale_ = "3";
    elfConfiguration[computer].ioConfiguration.pixieDoubleScreenIo = PIXIE_IN_OUT;
    elfConfiguration[computer].use6845 = false;
    elfConfiguration[computer].use6847 = false;
    elfConfiguration[computer].useTMS9918 = false;
    elfConfiguration[computer].use8275 = false;
    elfConfiguration[computer].useSN76430N = false;
    elfConfiguration[computer].usev1870 = false;
    elfConfiguration[computer].ioConfiguration.v1870useVideoModeEf = false;
    elfConfiguration[computer].ioConfiguration.v1870useBlockWrite = true;
    conf[computer].interlace_ = false;

    elfConfiguration[computer].useHexKeyboard = false;
    elfConfiguration[computer].useHexKeyboardEf3 = false;
    elfConfiguration[computer].useInButton = false;
    for (int pad=0; pad<MAX_BITKEYPADS; pad++)
        elfConfiguration[computer].ioConfiguration.bitKeypad[pad].defined = false;

    elfConfiguration[computer].useMatrixKeyboard = false;
    elfConfiguration[computer].ioConfiguration.keybMatrixIn = -1;
    elfConfiguration[computer].ioConfiguration.keybMatrixOut = -1;
    elfConfiguration[computer].ioConfiguration.keybMatrixAddressMode = false;
    elfConfiguration[computer].useKeyb1871 = false;
    elfConfiguration[computer].useKeyboard = false;
    elfConfiguration[computer].usePS2 = false;
    elfConfiguration[computer].ps2Interrupt = false;
    elfConfiguration[computer].usePs2gpio = false;
    elfConfiguration[computer].forceUpperCase = false;
    elfConfiguration[computer].useBitKeypad = false;
    for (int pad=0; pad<MAX_LATCHKEYPADS; pad++)
    {
        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].defined = false;
        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].outPort = -1;
    }
    elfConfiguration[computer].ioConfiguration.fredKeypad.defined= false;
    elfConfiguration[computer].ioConfiguration.etiKeypad.defined= false;
    elfConfiguration[computer].ioConfiguration.gpioIoGroup = -1;
    elfConfiguration[computer].ioConfiguration.keyboardIoGroup = -1;
    elfConfiguration[computer].useAdConvertor = false;
    
    elfConfiguration[computer].vtType = VTNONE;
    elfConfiguration[computer].vtShow = true;
    elfConfiguration[computer].ioConfiguration.vt100ReverseQ = -1;
    elfConfiguration[computer].ioConfiguration.vt100Ef = 4;
    elfConfiguration[computer].vtExternal = false;
    elfConfiguration[computer].useUart = false;
    elfConfiguration[computer].useUart16450 = false;
    elfConfiguration[computer].baudR = 4;
    elfConfiguration[computer].baudT = 4;
    elfConfiguration[computer].serialPort_ = "";
    elfConfiguration[computer].ioConfiguration.uartIoGroup = -1;
    
    elfConfiguration[computer].ioConfiguration.rtcIoGroup1 = -1;
    elfConfiguration[computer].ioConfiguration.rtcIoGroup2 = -1;

    conf[computer].autoCassetteLoad_ = true;
    conf[computer].turbo_ = true;
    conf[computer].ledTime_ = "100";

    memAccessDirDefined_ = false;
    conf[computer].ramDir_ = conf[computer].mainDir_ ;
    charRomDirDefined_ = false;
    conf[computer].charRom_ = "";
    conf[computer].charRomDir_ = conf[computer].mainDir_ ;
    batchFileDirDefined_ = false;
    conf[computer].batchFile_ = "";
    conf[computer].batchFileDir_ = conf[computer].mainDir_ ;
    ideFileDirDefined_ = false;
    conf[computer].ide_ = "";
    conf[computer].ideDir_ = conf[computer].mainDir_ ;

    keyFileDirDefined_ = false;
    keyFileDefined_ = false;
    keyFileDir = conf[computer].keyFileDir_;
    videoDumpDirDefined_ = false;
    videoDumpFileDefined_ = false;
    videoDumpFileDir = conf[computer].screenDumpFileDir_;
    printFileDirDefined_ = false;
    printFileDefined_ = false;
    printFileDir = conf[computer].printFileDir_;
    xmodemFileDirDefined_ = false;
    xmodemFileDefined_ = false;
    xmodemFileDir = conf[computer].xmodemFileDir_;
    sequencerFileDirDefined_ = false;
    conf[computer].sequencerFile_ = "2716-ntsc.hex";
    conf[computer].sequencerDir_ = conf[computer].mainDir_;
    conf[computer].useSplashScreen_ = false;

    for (int fdcType = 0; fdcType<FDCTYPE_MAX; fdcType++)
    {
        for (int disk=0; disk<4; disk++)
        {
            floppyDirDefined[fdcType][disk] = false;
            floppyDefined[fdcType][disk] = false;
            floppyFileDir[fdcType][disk] = floppyDir_[fdcType][disk];
            directoryMode_[fdcType][disk] = false;
        }
    }
    for (int tape=0; tape<2; tape++)
    {
        wavFileDirDefined[tape] = false;
        wavFileDefined[tape] = false;
        wavFileDir[tape] = conf[computer].wavFileDir_[tape];
    }
    for (int mem=0; mem<2; mem++)
        memDirDefined[mem] = false;
    
    elfConfiguration[computer].fdcType_ = FDCTYPE_17XX;

    conf[computer].locationTrigger.resize(65536);
    for (int address=0; address<65536; address++)
    {
        conf[computer].locationTrigger[address].index.resize(1);
        conf[computer].locationTrigger[address].index[0] = -1;
    }
    
    conf[computer].addressLocations.locationInfo.clear();
    conf[computer].addressLocations.keyInputAddress.clear();
    conf[computer].addressLocations.inReleaseAddress.clear();
    conf[computer].assemblerInfo_.clear();
    elfConfiguration[computer].ioConfiguration.diagonalKeys.clear();
    elfConfiguration[computer].ioConfiguration.cdp1851.clear();
    elfConfiguration[computer].ioConfiguration.cdp1852.clear();
    elfConfiguration[computer].ioConfiguration.cd4536bIo.clear();
    conf[computer].memConfig_[0].pulldownDir.Clear();
    conf[computer].memConfig_[0].pulldownMask.Clear();
    conf[computer].memConfig_[0].pulldownExclude.Clear();
    conf[computer].memConfig_[0].pulldownExclude2.Clear();
    conf[computer].memConfig_[1].pulldownDir.Clear();
    conf[computer].memConfig_[1].pulldownMask.Clear();
    conf[computer].memConfig_[1].pulldownExclude.Clear();
    conf[computer].memConfig_[1].pulldownExclude2.Clear();

    vt52CharRomDirDefined_ = false;
    elfConfiguration[computer].vt52CharRom_ = "vt52.a.bin";
    elfConfiguration[computer].vt52CharRomDir_ = conf[computer].mainDir_ ;
    vt100CharRomDirDefined_ = false;
    elfConfiguration[computer].vt100CharRom_ = "vt100.bin";
    elfConfiguration[computer].vt100CharRomDir_ = conf[computer].mainDir_ ;
    vtWavFileDirDefined_ = false;
    elfConfiguration[computer].vtWavFile_ = "";
    elfConfiguration[computer].vtWavFileDir_ = conf[computer].mainDir_ ;

    conf[computer].addressLocations.code_start = -1;
    conf[computer].addressLocations.code_start_high = -1;
    conf[computer].addressLocations.code_start_low = -1;
    conf[computer].addressLocations.code_end_high = -1;
    conf[computer].addressLocations.code_end_low = -1;
    conf[computer].addressLocations.reg_end = -1;
    conf[computer].printerOn_ = false;
    conf[computer].useBasicPrinter_ = false;
    conf[computer].useParallelPrinter_ = false;
    conf[computer].useCentronicsPrinter_ = false;
    conf[computer].useSerialPrinter_ = false;
    conf[computer].useThermalPrinter_ = false;
    conf[computer].useQSerialPrinter_ = false;
    elfConfiguration[computer].ioConfiguration.printerEf = 0;
    elfConfiguration[computer].ioConfiguration.serialPrinterInput = -1;
    elfConfiguration[computer].ioConfiguration.parallelPrinterInput = -1;
    elfConfiguration[computer].ioConfiguration.thermalPrinterInput = -1;

    elfConfiguration[computer].useXmodem = false;
    elfConfiguration[computer].useHexModem = false;
    elfConfiguration[computer].useTape = false;
    elfConfiguration[computer].useTape1 = false;
    elfConfiguration[computer].useTapeMicro = false;
    elfConfiguration[computer].useTapeHw = false;

    conf[computer].soundType_ = SOUND_OFF;
    conf[computer].stereo_ = 1;

    elfConfiguration[computer].efButtons = false;
    elfConfiguration[computer].flexPanel_ = false;
    elfConfiguration[computer].panelType_ = PANEL_NONE;
    elfConfiguration[computer].useLedModule = false;
    elfConfiguration[computer].useSwitch = false;
    elfConfiguration[computer].useHex = false;
    elfConfiguration[computer].useElfControlWindows = false;

    elfConfiguration[computer].giantBoardMapping = false;
    elfConfiguration[computer].ioConfiguration.bootStrapType = BOOTSTRAPNONE;

    elfConfiguration[computer].ioConfiguration.hexOutput = init_IoPort();
    elfConfiguration[computer].ioConfiguration.hexInput = init_IoPort();

    elfConfiguration[computer].useDip = false;
    elfConfiguration[computer].useIoGroup = false;

    elfConfiguration[computer].useCvKeypad = false;
    elfConfiguration[computer].ioConfiguration.cvKeypad.defined = false;

    elfConfiguration[computer].useBitSound = false;
    elfConfiguration[computer].tilType = TILNONE;
    
    conf[computer].defVtX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[computer].defVtY_ = mainWindowY_;
    conf[computer].defCoinX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[computer].defCoinY_ = mainWindowY_;
    conf[computer].defPixieX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[computer].defPixieY_ = mainWindowY_;
    conf[computer].defCdp1864X_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[computer].defCdp1864Y_ = mainWindowY_;
    conf[computer].defSt4X_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[computer].defSt4Y_ = mainWindowY_;
    conf[computer].defVip2KX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[computer].defVip2KY_ = mainWindowY_;
    conf[computer].defFredX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[computer].defFredY_ = mainWindowY_;
    conf[computer].defMc6845X_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[computer].defMc6845Y_ = mainWindowY_;
    conf[computer].defMc6847X_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[computer].defMc6847Y_ = mainWindowY_;
    conf[computer].defTmsX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[computer].defTmsY_ = mainWindowY_;
    conf[computer].defi8275X_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[computer].defi8275Y_ = mainWindowY_;
    conf[computer].defv1870X_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[computer].defv1870Y_ = mainWindowY_;
    conf[computer].defSN76430NX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[computer].defSN76430NY_ = mainWindowY_;
    conf[computer].defaultFrontPanelX_ = mainWindowX_+windowInfo.xBorder2;
    conf[computer].defaultFrontPanelY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;

    configPointer->Read("/Xml/DebugScrt", &conf[computer].scrtMode_, true);
    conf[computer].debugCallReg_ = configPointer->Read("/Xml/DebugCallReg", 4);
    conf[computer].debugCallAddress_ = configPointer->Read("/Xml/DebugCallAddress", -1);
    conf[computer].debugRetReg_ = configPointer->Read("/Xml/DebugRetReg", 5);
    conf[computer].debugRetAddress_ = configPointer->Read("/Xml/DebugRetAddress", -1);

    elfConfiguration[computer].ioConfiguration.qLed = false;
    elfConfiguration[computer].ioConfiguration.runLed = false;
    elfConfiguration[computer].ioConfiguration.errorLed = false;
    elfConfiguration[computer].ioConfiguration.readyLed = false;
    elfConfiguration[computer].ioConfiguration.stopLed = false;
    elfConfiguration[computer].ioConfiguration.showDataOnLoad = false;
    elfConfiguration[computer].ioConfiguration.showDataOnCycle = false;
    elfConfiguration[computer].ioConfiguration.runPressType = RUN_TYPE_DEFAULT;
    elfConfiguration[computer].ioConfiguration.runPressType0 = RUN_TYPE_DEFAULT;
    elfConfiguration[computer].ioConfiguration.resetPressType = RESET_TYPE_DEFAULT;
    for (int bit=0; bit<MAX_BIT_LEDS; bit++)
        elfConfiguration[computer].ioConfiguration.bitLed[bit] = false;
    for (int bit=0; bit<MAX_CPU_STATE_LEDS; bit++)
        elfConfiguration[computer].ioConfiguration.cpuStatusLed[bit] = false;
    for (int til=0; til<MAX_DATA_TIL; til++)
        elfConfiguration[computer].ioConfiguration.datatil[til] = false;
    for (int til=0; til<MAX_ADDRESS_TIL; til++)
        elfConfiguration[computer].ioConfiguration.addresstil[til] = false;

    for (int ef=1; ef<5; ef++)
        elfConfiguration[computer].ioConfiguration.bitKeypadEf[ef] = false;

    oldXmlFileName_ = xmlDir + xmlFile;
    oldXmlDate_ = newDate;

    int tagTypeInt;
    wxString padLetter, padNumber;

    warningText_ = "";
    
    wxXmlDocument doc;
    
    if (!doc.Load(xmlDir + xmlFile))
        return;
    
    // start processing the XML file
    if (doc.GetRoot()->GetName() != "emmaconfig")
        return;
    
    wxXmlNode *child = doc.GetRoot()->GetChildren();
    while (child)
    {
        wxString childName = child->GetName();
        
        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_SYSTEM:
                parseXml_System (computer, *child);
            break;
                
            case TAG_BASIC:
                parseXml_Basic (computer, *child);
            break;
                
            case TAG_LOCATIONS:
                parseXml_Locations (computer, *child);
            break;
                
            case TAG_BOOTSTRAP:
                parseXml_Bootstrap (computer, *child);
            break;
                
            case TAG_PANEL:
                elfConfiguration[computer].useElfControlWindows = false;
                if (child->GetAttribute("init") == "on")
                    elfConfiguration[computer].useElfControlWindows = true;
                if (child->HasAttribute("type"))
                {
                    if (child->GetAttribute("type") == "io")
                    {
                        elfConfiguration[computer].useElfControlWindows = false;
                        parseXml_FrontPanel (computer, *child);
                    }
                    if (child->GetAttribute("type") == "cosmac")
                    {
                        elfConfiguration[computer].tilType = TIL311;
                        elfConfiguration[computer].panelType_ = PANEL_COSMAC;
                        parseXml_FrontPanel (computer, *child);
                    }
                    if (child->GetAttribute("type") == "netronics")
                    {
                        elfConfiguration[computer].tilType = TIL311;
                        elfConfiguration[computer].panelType_ = PANEL_ELFII;
                        parseXml_FrontPanel (computer, *child);
                    }
                    if (child->GetAttribute("type") == "quest")
                    {
                        elfConfiguration[computer].tilType = TIL313ITALIC;
                        elfConfiguration[computer].panelType_ = PANEL_SUPER;
                        parseXml_FrontPanel (computer, *child);
                    }
                    if (child->GetAttribute("type") == "elf2k")
                    {
                        elfConfiguration[computer].tilType = TIL311;
                        elfConfiguration[computer].panelType_ = PANEL_ELF2K;
                        parseXml_FrontPanel (computer, *child);
                    }
                    if (child->GetAttribute("type") == "cosmicos")
                    {
                        elfConfiguration[computer].tilType = TIL313;
                        elfConfiguration[computer].panelType_ = PANEL_COSMICOS;
                        parseXml_FrontPanel (computer, *child);
                    }
                    if (child->GetAttribute("type") == "velf")
                    {
                        elfConfiguration[computer].tilType = TIL311;
                        elfConfiguration[computer].panelType_ = PANEL_VELF;
                        parseXml_FrontPanel (computer, *child);
                    }
                    if (child->GetAttribute("type") == "member" || child->GetAttribute("type") == "member_j")
                    {
                        elfConfiguration[computer].frontType = FRONT_TYPE_J;
                        elfConfiguration[computer].panelType_ = PANEL_MEMBER;
                        parseXml_FrontPanel (computer, *child);
                    }
                    if (child->GetAttribute("type") == "member_b")
                    {
                        elfConfiguration[computer].frontType = FRONT_TYPE_B;
                        elfConfiguration[computer].panelType_ = PANEL_MEMBER;
                        parseXml_FrontPanel (computer, *child);
                    }
                    if (child->GetAttribute("type") == "member_c")
                    {
                        elfConfiguration[computer].frontType = FRONT_TYPE_C;
                        elfConfiguration[computer].panelType_ = PANEL_MEMBER;
                        parseXml_FrontPanel (computer, *child);
                    }
                    if (child->GetAttribute("type") == "member_i")
                    {
                        elfConfiguration[computer].frontType = FRONT_TYPE_I;
                        elfConfiguration[computer].panelType_ = PANEL_MEMBER;
                        parseXml_FrontPanel (computer, *child);
                    }
                    if (child->GetAttribute("type") == "microtutor")
                    {
                        elfConfiguration[computer].tilType = TIL311;
                        elfConfiguration[computer].panelType_ = PANEL_MICROTUTOR;
                        parseXml_FrontPanel (computer, *child);
                    }
                    if (child->GetAttribute("type")  == "microtutor2" || child->GetAttribute("type")  == "microtutorII")
                    {
                        elfConfiguration[computer].tilType = TIL313ITALIC;
                        elfConfiguration[computer].panelType_ = PANEL_MICROTUTOR2;
                        parseXml_FrontPanel (computer, *child);
                    }
                    if (child->GetAttribute("type") == "uc1800")
                    {
                        elfConfiguration[computer].tilType = TIL313ITALIC;
                        elfConfiguration[computer].panelType_ = PANEL_UC1800;
                        parseXml_FrontPanel (computer, *child);
                    }
                }
                else
                {
                    elfConfiguration[computer].panelType_ = PANEL_XML;
                    elfConfiguration[computer].tilType = TILNONE;
                    parseXml_FrontPanel (computer, *child);
                }
            break;
                
            case TAG_MEMORY:
                if (child->HasAttribute("mask"))
                    elfConfiguration[computer].memoryMask = (int) parseXml_Number(*child, "mask");
                parseXml_Memory (computer, *child);
            break;

            case TAG_DISK:
                elfConfiguration[computer].ioConfiguration.fdcDrives = (int) ((parseXml_Number(*child, "drives")-1)&0x3) + 1;
                if (child->GetAttribute("type") == "upd765")
                    parseXml_Upd765 (computer, *child);
                if (child->GetAttribute("type") == "1793")
                {
                    elfConfiguration[computer].fdc1793Enabled = true;
                    parseXml_FdcDisk (computer, *child);
                }
                if (child->GetAttribute("type") == "1770")
                {
                    elfConfiguration[computer].fdc1770Enabled = true;
                    parseXml_FdcDisk (computer, *child);
                }
                if (child->GetAttribute("type") == "ide")
                    parseXml_IdeDisk (computer, *child);
            break;
                
            case TAG_VIDEO:
                if (child->GetAttribute("type") == "coin")
                    parseXml_CoinVideo (computer, *child);
                if (child->GetAttribute("type") == "pixie" || child->GetAttribute("type") == "cdp1861" || child->GetAttribute("type") == "1861")
                    parseXml_PixieVideo (computer, *child);
                if (child->GetAttribute("type") == "cdp1862" || child->GetAttribute("type") == "1862")
                    parseXml_1862Video (computer, *child);
                if (child->GetAttribute("type") == "cdp1864" || child->GetAttribute("type") == "1864")
                    parseXml_1864Video (computer, *child);
                if (child->GetAttribute("type") == "vip2k")
                    parseXml_Vip2KVideo (computer, *child);
                if (child->GetAttribute("type") == "fred")
                    parseXml_fredVideo (computer, *child);
                if (child->GetAttribute("type") == "mc6845")
                    parseXml_MC6845Video (computer, *child);
                if (child->GetAttribute("type") == "mc6847")
                    parseXml_MC6847Video (computer, *child);
                if (child->GetAttribute("type") == "tms9918")
                    parseXml_TMS9918Video (computer, *child);
                if (child->GetAttribute("type") == "intel8275")
                    parseXml_Intel8275Video (computer, *child);
                if (child->GetAttribute("type") == "1870")
                    parseXml_VisVideo (computer, *child);
                if (child->GetAttribute("type") == "sn76430n")
                    parseXml_SN76430NVideo (computer, *child);
                if (child->GetAttribute("type") == "st4")
                    parseXml_St4Video (computer, *child);
            break;

            case TAG_KEYBOARD:
                if (child->GetAttribute("type") == "ps2")
                {
                   elfConfiguration[computer].ps2Interrupt = false;
                   parseXml_Ps2Keyboard (computer, *child);
                }
                if (child->GetAttribute("type") == "ps2-interrupt")
                {
                   elfConfiguration[computer].ps2Interrupt = true;
                   parseXml_Ps2Keyboard (computer, *child);
                }
                if (child->GetAttribute("type") == "ascii")
                   parseXml_AsciiKeyboard (computer, *child);
                if (child->GetAttribute("type") == "1871")
                   parseXml_1871Keyboard (computer, *child);
                if (child->GetAttribute("type") == "latch")
                    parseXml_LatchKeyboard (computer, *child);
                if (child->GetAttribute("type") == "matrix")
                    parseXml_MatrixKeyboard (computer, *child);
            break;

            case TAG_GPIO:
                parseXml_Gpio (computer, *child);
            break;
                
            case TAG_CDP1851:
                parseXml_Cdp1851 (computer, *child, child->GetAttribute("init") == "on");
            break;

            case TAG_CDP1852:
                parseXml_Cdp1852 (computer, *child, child->GetAttribute("init") == "on");
            break;

            case TAG_CD4536B:
                parseXml_Cd4536b (computer, *child);
            break;

            case TAG_AD_CONVERTOR:
                parseXml_AdConvertor (computer, *child);
            break;

            case TAG_KEYPAD:
                if (child->GetAttribute("type") == "elf")
                    parseXml_ElfKeypad (computer, *child);
                if (child->GetAttribute("type") == "cosmicos")
                    parseXml_CosmicosKeypad (computer, *child);
                if (child->GetAttribute("type") == "latch")
                {
                    if (child->HasAttribute("pad"))
                    {
                        for (int pad=1; pad<MAX_LATCHKEYPADS; pad++)
                        {
                            padLetter.Printf("%c", pad+97-1);
                            padNumber.Printf("%c", pad+49-1);
                            if (child->GetAttribute("pad") == padLetter || child->GetAttribute("pad") == padNumber)
                            {
                                elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].padNumberStr = " " + child->GetAttribute("pad");
                                elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].padNumberStr = elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].padNumberStr.Capitalize();
                                parseXml_LatchKeypad (computer, pad, *child);
                            }
                        }
                    }
                    else
                    {
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[1].padNumberStr = "";
                        parseXml_LatchKeypad (computer, 1, *child);
                    }
                }
                if (child->GetAttribute("type") == "fred")
                    parseXml_FredKeypad (computer, *child);
                if (child->GetAttribute("type") == "hug" || child->GetAttribute("type") == "eti")
                    parseXml_EtiKeypad (computer, *child);
            break;

            case TAG_BITKEYPAD:
                for (int pad=0; pad<MAX_BITKEYPADS; pad++)
                {
                    padLetter.Printf("%c", pad+97);
                    padNumber.Printf("%c", pad+49);
                    if (child->GetAttribute("pad") == padLetter || child->GetAttribute("pad") == padNumber)
                    {
                        elfConfiguration[computer].ioConfiguration.bitKeypad[pad].number = child->GetAttribute("pad");
                        elfConfiguration[computer].ioConfiguration.bitKeypad[pad].number = elfConfiguration[computer].ioConfiguration.bitKeypad[pad].number.Capitalize();
                        parseXml_BitKeypad (computer, pad, *child);
                    }
               }
            break;

            case TAG_CVKEYPAD:
                parseXml_CvKeypad (computer, *child);
            break;

            case TAG_VT:
                if (child->GetAttribute("type") == "vt100")
                    elfConfiguration[computer].vtType = VT100;
                if (child->GetAttribute("type") == "vt52")
                    elfConfiguration[computer].vtType = VT52;
                if (child->GetAttribute("type") == "external")
                {
                    elfConfiguration[computer].vtExternal = true;
                    elfConfiguration[computer].vtType = VTNONE;
                }
                if (child->GetAttribute("connection") == "serial")
                    parseXml_SerialVt (computer, *child);
                if (child->GetAttribute("connection") == "uart1854")
                {
                    elfConfiguration[computer].useUart = true;
                    parseXml_UartVt (computer, *child, false);
                }
                if (child->GetAttribute("connection") == "uart16450")
                {
                    elfConfiguration[computer].useUart16450 = true;
                    parseXml_UartVt (computer, *child, true);
                }
            break;

            case TAG_PRINTER:
                if (child->GetAttribute("type") == "")
                {
                    conf[computer].useBasicPrinter_ = true;
                    parseXml_Printer (computer, *child, PRINTER_BASIC);
                }
                if (child->GetAttribute("type") == "q")
                {
                    conf[computer].useQSerialPrinter_ = true;
                    parseXml_Printer (computer, *child, PRINTER_SERIAL_Q);
                }
                if (child->GetAttribute("type") == "centronics")
                {
                    conf[computer].useCentronicsPrinter_ = true;
                    parseXml_Printer (computer, *child, PRINTER_CENTRONICS);
                }
                if (child->GetAttribute("type") == "parallel")
                {
                    conf[computer].useParallelPrinter_ = true;
                    parseXml_Printer (computer, *child, PRINTER_PARALLEL);
                }
                if (child->GetAttribute("type") == "serial")
                {
                    conf[computer].useSerialPrinter_ = true;
                    parseXml_Printer (computer, *child, PRINTER_SERIAL);
                }
                if (child->GetAttribute("type") == "thermal")
                {
                    conf[computer].useThermalPrinter_ = true;
                    parseXml_Printer (computer, *child, COMXTHPRINTER);
                }
            break;

            case TAG_CASSETTE:
                if (child->GetAttribute("decks") == "2")
                    elfConfiguration[computer].useTape1 = true;
                if (child->GetAttribute("type") == "micro")
                    elfConfiguration[computer].useTapeMicro = true;
                if (child->GetAttribute("type") == "cv")
                {
                    elfConfiguration[computer].useTapeHw = true;
                    elfConfiguration[computer].tapeFormat_ = TAPE_FORMAT_CV;
                    parseXml_Cassette (computer, *child);
                }
                else
                {
                    if (child->GetAttribute("type") == "fred")
                    {
                        elfConfiguration[computer].useTapeHw = true;
                        elfConfiguration[computer].tapeFormat_ = TAPE_FORMAT_AUTO;
                        parseXml_Cassette (computer, *child);
                    }
                    else
                    {
                        elfConfiguration[computer].useTape = true;
                        parseXml_Cassette (computer, *child);
                    }
                }
            break;

            case TAG_SOUND:
                if (child->GetAttribute("type") == "q")
                    parseXml_QSound (computer, *child);
                if (child->GetAttribute("type") == "studio")
                    conf[computer].soundType_ = SOUND_STUDIO;
                if (child->GetAttribute("type") == "bit")
                    parseXml_OutBitSound (computer, *child);
                if (child->GetAttribute("type") == "cdp1863" || child->GetAttribute("type") == "1863")
                    parseXml_Cdp1863Sound (computer, *child);
                if (child->GetAttribute("type") == "vp550")
                    conf[computer].soundType_ = SOUND_SUPER_VP550;
                if (child->GetAttribute("type") == "vp551")
                    conf[computer].soundType_ = SOUND_SUPER_VP551;
                if (child->GetAttribute("stereo") == "on")
                    conf[computer].stereo_ = 2;
            break;

            case TAG_XMODEM:
                elfConfiguration[computer].useXmodem = true;
                parseXml_Xmodem (computer, *child);
            break;

            case TAG_HEXMODEM:
                elfConfiguration[computer].useHexModem = true;
                parseXml_Xmodem (computer, *child);
            break;

            case TAG_EFBUTTONS:
                parseXml_EfButtons (computer, *child);
            break;

            case TAG_DIP:
                parseXml_Dip (computer, *child);
            break;

            case TAG_IOGROUP:
                parseXml_IoGroup (computer, *child);
            break;
                
            case TAG_RTC:
                if (child->GetAttribute("type") == "m48t58")
                    parseXml_RtcM58T58 (computer, *child);
                if (child->GetAttribute("type") == "ds12887")
                    parseXml_RtcDS12887 (computer, *child);
            break;

            case TAG_USB:
                if (child->GetAttribute("type") == "superboard")
                    parseXml_UsbSuperBoard (computer, *child);
            break;

            case TAG_KEYFILE:
                parseXml_KeyFile (computer, *child);
            break;
                
            case TAG_SPLASH:
                parseXml_Splash (computer, *child);
            break;
                
            case TAG_VIDEODUMP:
                parseXml_VideoDump (computer, *child);
            break;
                
            case TAG_MEMACCESS:
                parseXml_MemAccess (computer, *child);
            break;
                
            case TAG_GUI:
                parseXml_Gui (computer, *child);
            break;
                
            case TAG_DIAG:
                parseXml_Diag (computer, *child);
            break;
                
            case TAG_BATCHWAV:
                parseXml_BatchWav (computer, *child);
            break;
        
            case TAG_DEBUGGER:
                parseXml_Debugger (computer, *child);
            break;
                                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
       
        child = child->GetNext();
    }
    
    if (warningText_ != "")
        wxMessageBox(warningText_, "Warning list xml parser", wxICON_EXCLAMATION);
    
    if (!memAccessDirDefined_)
        conf[computer].ramDir_ = conf[computer].mainDir_ ;
    if (!batchFileDirDefined_)
        conf[computer].batchFileDir_ = conf[computer].mainDir_ ;
    if (!ideFileDirDefined_)
        conf[computer].ideDir_ = conf[computer].mainDir_ ;
    if (!sequencerFileDirDefined_)
        conf[computer].sequencerDir_ = conf[computer].mainDir_ ;
    if (!charRomDirDefined_)
        conf[computer].charRomDir_ = conf[computer].mainDir_ ;
    if (!vt52CharRomDirDefined_)
        elfConfiguration[computer].vt52CharRomDir_ = conf[computer].mainDir_ ;
    if (!vt100CharRomDirDefined_)
        elfConfiguration[computer].vt100CharRomDir_ = conf[computer].mainDir_ ;
    if (!vtWavFileDirDefined_)
        elfConfiguration[computer].vtWavFileDir_ = conf[computer].mainDir_ ;

    if (keyFileDir.Left(conf[computer].mainDir_.Len()) != conf[computer].mainDir_)
    {
        if (!keyFileDirDefined_)
            conf[computer].keyFileDir_ = conf[computer].mainDir_;
        if (!keyFileDefined_)
            conf[computer].keyFile_ = "";
    }
    if (videoDumpFileDir.Left(conf[computer].mainDir_.Len()) != conf[computer].mainDir_)
    {
        if (!videoDumpDirDefined_)
            conf[computer].screenDumpFileDir_ = conf[computer].mainDir_;
        if (!videoDumpFileDefined_)
            conf[computer].screenDumpFile_ = "screendump.png";
    }
    
    for (std::vector<AssemblerInfo>::iterator assemblerInfo = conf[computer].assemblerInfo_.begin (); assemblerInfo != conf[computer].assemblerInfo_.end (); ++assemblerInfo)
    {
        if (!assemblerInfo->dirDefined)
            assemblerInfo->dir = conf[computer].mainDir_;
        if (!assemblerInfo->fileDefined)
            assemblerInfo->file = "";
    }
    if (printFileDir.Left(conf[computer].mainDir_.Len()) != conf[computer].mainDir_)
    {
        if (!printFileDirDefined_)
            conf[computer].printFileDir_ = conf[computer].mainDir_;
        if (!printFileDefined_)
            conf[computer].printFile_ = "printerout.txt";
    }
    if (xmodemFileDir.Left(conf[computer].mainDir_.Len()) != conf[computer].mainDir_)
    {
        if (!xmodemFileDirDefined_)
            conf[computer].xmodemFileDir_ = conf[computer].mainDir_;
        if (!xmodemFileDefined_)
            conf[computer].xmodemFile_ = "";
    }
    for (int fdcType = 0; fdcType<FDCTYPE_MAX; fdcType++)
    {
        for (int disk=0; disk<4; disk++)
        {
            if (floppyFileDir[fdcType][disk].Left(conf[computer].mainDir_.Len()) != conf[computer].mainDir_)
            {
                if (!floppyDirDefined[fdcType][disk])
                    floppyDir_[fdcType][disk] = conf[computer].mainDir_;
                if (!floppyDefined[fdcType][disk])
                    floppy_[fdcType][disk] = "";
            }
        }
    }
    for (int tape=0; tape<2; tape++)
    {
        if (wavFileDir[tape].Left(conf[computer].mainDir_.Len()) != conf[computer].mainDir_)
        {
            if (!wavFileDirDefined[tape])
                conf[computer].wavFileDir_[tape] = conf[computer].mainDir_;
            if (!wavFileDefined[tape])
                conf[computer].wavFile_[tape] = "";
        }
    }
    for (int mem=0; mem<2; mem++)
    {
        if (conf[computer].memConfig_[mem].dirname.Left(conf[computer].mainDir_.Len()) != conf[computer].mainDir_)
        {
            if (!memDirDefined[mem])
                conf[computer].memConfig_[mem].dirname = conf[computer].mainDir_;
        }
    }
}

void XmlParser::parseXml_System(int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "name",
        "config",
        "cpu",
        "clock",
        "boot",
        "f12",
        "init",
        "monitor",
        "dirname",
        "dma",
        "int",
        "statusbar",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_NAME,
        TAG_CONFIG,
        TAG_CPU,
        TAG_CLOCK,
        TAG_BOOT,
        TAG_F12,
        TAG_INIT,
        TAG_MONITOR,
        TAG_DIRNAME,
        TAG_DMA,
        TAG_INT,
        TAG_STATUSBAR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    elfConfiguration[computer].dmaOnFirstQ = false;
    elfConfiguration[computer].dmaOnFirstOut = 0;
    computerInfo[computer].name = "";
    computerInfo[computer].configuration = "";
    
    wxString guiName;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_NAME:
                computerInfo[computer].name = child->GetNodeContent();
         /*       if (mode_.gui)
                {
                    if (computerInfo[computer].configuration == "")
                        guiName = computerInfo[computer].name;
                    else
                        guiName = computerInfo[computer].name + " - " + computerInfo[computer].configuration;
                    guiName.Replace("&","&&");

                    XRCCTRL(*this,"ConfigTextXml", wxStaticText)->SetLabel(guiName);
                }*/
            break;

            case TAG_CONFIG:
                computerInfo[computer].configuration = child->GetNodeContent();
            break;
                
            case TAG_CPU:
                setCpuType(child->GetNodeContent());
            break;

            case TAG_CLOCK:
                conf[computer].clock_ = getDoubleString(child->GetNodeContent(), childName, 500, "500");
            break;
                
            case TAG_BOOT:
                elfConfiguration[computer].autoBoot = true;
                if (child->GetAttribute("start") == "no")
                    elfConfiguration[computer].autoBoot = false;
                if (child->GetAttribute("dma") == "on")
                    elfConfiguration[computer].dmaOnBoot = true;
                if (child->GetAttribute("dma_zero") == "on")
                    elfConfiguration[computer].dmaOnBoot0 = true;

                conf[computer].bootAddress_ = getHexDec(child->GetNodeContent());
            break;
                
            case TAG_F12:
                if (child->GetNodeContent() == "stop")
                    elfConfiguration[computer].f12reset = false;
            break;
                
            case TAG_INIT:
                if (child->GetAttribute("type") == "ef1")
                    elfConfiguration[computer].ioConfiguration.ef1default = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "ef2")
                    elfConfiguration[computer].ioConfiguration.ef2default = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "ef3")
                    elfConfiguration[computer].ioConfiguration.ef3default = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "ef4")
                    elfConfiguration[computer].ioConfiguration.ef4default = (int)parseXml_Number(*child);
            break;

            case TAG_MONITOR:
                elfConfiguration[computer].ioConfiguration.efMonitor = (int)parseXml_Number(*child);
            break;

            case TAG_DIRNAME:
                conf[computer].mainDir_ =  dataDir_ + child->GetNodeContent();
                if (conf[computer].mainDir_.Right(1) != pathSeparator_)
                    conf[computer].mainDir_ += pathSeparator_;
            break;
                
            case TAG_DMA:
                elfConfiguration[computer].useDma = true;
                if (child->GetAttribute("out") == "q")
                    elfConfiguration[computer].dmaOnFirstQ = true;
                else
                    elfConfiguration[computer].dmaOnFirstOut = (int)parseXml_Number(*child, "out");
                elfConfiguration[computer].dmaCycleValue = (int)parseXml_Number(*child);
            break;

            case TAG_INT:
                elfConfiguration[computer].useInt = true;
                elfConfiguration[computer].intCycleValue = (int)parseXml_Number(*child);
            break;

            case TAG_STATUSBAR:
                if (child->GetNodeContent() == "comx")
                    elfConfiguration[computer].ioConfiguration.statusBarType = STATUSBAR_COMX;
                if (child->GetNodeContent() == "diag")
                    elfConfiguration[computer].ioConfiguration.statusBarType = STATUSBAR_DIAG;
                if (child->GetNodeContent() == "cidelsa")
                {
                    elfConfiguration[computer].ioConfiguration.statusBarType = STATUSBAR_CIDELSA;
                    if (child->HasAttribute("out"))
                        elfConfiguration[computer].ioConfiguration.statusBarLedOut = (int)parseXml_Number(*child, "out");
                }
                if (child->GetNodeContent() == "vip2")
                    elfConfiguration[computer].ioConfiguration.statusBarType = STATUSBAR_VIP2;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
    if (mode_.gui)
    {
        if (computerInfo[computer].configuration == "")
            guiName = computerInfo[computer].name;
        else
            guiName = computerInfo[computer].configuration;
        guiName.Replace("&","&&");

        XRCCTRL(*this,"ConfigTextXml", wxStaticText)->SetLabel(guiName);
    }
}

void XmlParser::parseXml_Basic(int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "pload",
        "id",
        "defus",
        "eop",
        "string",
        "array",
        "eod",
        "ram",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_PLOAD,
        TAG_ID,
        TAG_DEFUS,
        TAG_EOP,
        TAG_STRING,
        TAG_ARRAY,
        TAG_EOD,
        TAG_RAM,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString ploadUpper;

    conf[computer].defusDefined_ = false;
    conf[computer].eopDefined_ = false;
    conf[computer].stringDefined_ = false;
    conf[computer].arrayValueDefined_ = false;
    conf[computer].eodDefined_ = false;
    conf[computer].basicRamAddressDefined_ = false;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
      
            case TAG_PLOAD:
                computerInfo[computer].ploadExtension = child->GetNodeContent();
                ploadUpper = computerInfo[computer].ploadExtension;
                ploadUpper.MakeUpper();
                for (int i=0; i<4; i++)
                {
                    if (i < (int)ploadUpper.Len())
                        conf[computer].pLoadSaveName_[i] = ploadUpper.GetChar(i);
                    else
                        conf[computer].pLoadSaveName_[i] = ' ';
                }
            break;

            case TAG_ID:
                ploadUpper = child->GetNodeContent();
                ploadUpper.MakeUpper();
                for (int i=0; i<4; i++)
                {
                    if (i < (int)ploadUpper.Len())
                        conf[computer].pLoadSaveName_[i] = ploadUpper.GetChar(i);
                    else
                        conf[computer].pLoadSaveName_[i] = ' ';
                }
            break;

            case TAG_DEFUS:
                conf[computer].defusDefined_ = true;
                conf[computer].defus_ = (int)parseXml_Number(*child);
            break;

            case TAG_EOP:
                conf[computer].eopDefined_ = true;
                conf[computer].eop_ = (int)parseXml_Number(*child);
            break;

            case TAG_STRING:
                conf[computer].stringDefined_ = true;
                conf[computer].string_ = (int)parseXml_Number(*child);
            break;

            case TAG_ARRAY:
                conf[computer].arrayValueDefined_ = true;
                conf[computer].arrayValue_ = (int)parseXml_Number(*child);
            break;

            case TAG_EOD:
                conf[computer].eodDefined_ = true;
                conf[computer].eod_ = (int)parseXml_Number(*child);
            break;

            case TAG_RAM:
                conf[computer].basicRamAddressDefined_ = true;
                conf[computer].basicRamAddress_ = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Locations(int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "key_input",
        "in_release",
        "code_start",
        "code_start_high",
        "code_start_low",
        "code_end_high",
        "code_end_low",
        "reg_end",
        "reset_state",
        "ut_state",
        "basic_state",
        "basic_state_cw",
        "run_state",
        "stop_thermal",
        "stop_cas",
        "stop_cas_micro",
        "start_cas_save",
        "start_cas_load",
        "start_cas_dload",
        "set_dos_filename",
        "batch_start_save",
        "batch_end_save",
        "start_xmodem_save",
        "start_xmodem_load",
        "start_ymodem_save",
        "start_hexmodem_save",
        "start_hexmodem_load",
        "start_binmodem_save",
        "start_binmodem_load",
        "stop_modem",
        "correct_caps",
        "elfosboot",
        "start_chip8",
        "cv_tape_play",
        "cv_tape_ff",
        "cv_tape_rec",
        "fred_tape_rec",
        "check_sw",
        "st2_buildin_game",
        "vic_buildin_game",
        "vis_buildin_game",
        "set_pseudo_am4kbas2020",
        "set_pseudo_am4kbas1978",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_KEYINPUT,
        TAG_IN_RELEASE,
        TAG_CODE_START,
        TAG_CODE_START_HIGH,
        TAG_CODE_START_LOW,
        TAG_CODE_END_HIGH,
        TAG_CODE_END_LOW,
        TAG_REG_END,
        TAG_RESET_STATE,
        TAG_UT_STATE,
        TAG_BASIC_STATE,
        TAG_BASIC_STATE_CW,
        TAG_RUN_STATE,
        TAG_STOP_THERMAL,
        TAG_STOP_CAS,
        TAG_STOP_CAS_MICRO,
        TAG_START_CAS_SAVE,
        TAG_START_CAS_LOAD,
        TAG_START_CAS_DLOAD,
        TAG_SET_DOS_FILENAME,
        TAG_BATCH_START_SAVE,
        TAG_BATCH_END_SAVE,
        TAG_START_XMODEM_SAVE,
        TAG_START_XMODEM_LOAD,
        TAG_START_YMODEM_SAVE,
        TAG_START_HEXMODEM_SAVE,
        TAG_START_HEXMODEM_LOAD,
        TAG_START_BINMODEM_SAVE,
        TAG_START_BINMODEM_LOAD,
        TAG_STOP_MODEM,
        TAG_CORRECT_CAPS,
        TAG_ELFOS_BOOT,
        TAG_START_CHIP8,
        TAG_CV_TAPE_PLAY,
        TAG_CV_TAPE_FF,
        TAG_CV_TAPE_REC,
        TAG_FRED_TAPE_REC,
        TAG_CHECK_SW,
        TAG_STUDIO_BUILD_IN_GAME,
        TAG_VICTORY_BUILD_IN_GAME,
        TAG_VISICOM_BUILD_IN_GAME,
        TAG_SET_PSEUDO_AM4KBAS2020,
        TAG_SET_PSEUDO_AM4KBAS1978,
        TAG_COMMENT,
        TAG_UNDEFINED,
    };
    
    int tagTypeInt;
    wxString addressLocations, addressValues;
    Word address;
    LocationInfo newInfo;
    CheckAddressInfo newAddressInfo;
    CheckRegInfo newRegInfo;
    char newtrigger;
        
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
      
            case TAG_KEYINPUT:
                addressLocations = child->GetNodeContent();
                while (addressLocations != "")
                {
                    address = getNextHexDec(&addressLocations);
                    if (address != 0)
                        conf[computer].addressLocations.keyInputAddress.push_back(address);
                }
            break;

            case TAG_IN_RELEASE:
                addressLocations = child->GetNodeContent();
                while (addressLocations != "")
                {
                    address = getNextHexDec(&addressLocations);
                    if (address != 0)
                        conf[computer].addressLocations.inReleaseAddress.push_back(address);
                }
            break;

            case TAG_CODE_START:
                conf[computer].addressLocations.code_start = (int)getHexDec(child->GetNodeContent());
            break;
                
            case TAG_CODE_START_HIGH:
                conf[computer].addressLocations.code_start_high = (int)getHexDec(child->GetNodeContent());
            break;
                
            case TAG_CODE_START_LOW:
                conf[computer].addressLocations.code_start_low = (int)getHexDec(child->GetNodeContent());
            break;

            case TAG_CODE_END_HIGH:
                conf[computer].addressLocations.code_end_high = (int)getHexDec(child->GetNodeContent());
            break;
                
            case TAG_CODE_END_LOW:
                conf[computer].addressLocations.code_end_low = (int)getHexDec(child->GetNodeContent());
            break;
                
            case TAG_REG_END:
                conf[computer].addressLocations.reg_end = (int)getHexDec(child->GetNodeContent());
                conf[computer].addressLocations.reg_end_adjust = 0;
                if (child->HasAttribute("adjust"))
                    conf[computer].addressLocations.reg_end_adjust = (int)parseXml_Number(*child, "adjust");
            break;

            case TAG_RESET_STATE:
            case TAG_UT_STATE:
            case TAG_BASIC_STATE:
            case TAG_BASIC_STATE_CW:
            case TAG_RUN_STATE:
            case TAG_STOP_THERMAL:
            case TAG_STOP_CAS:
            case TAG_STOP_CAS_MICRO:
            case TAG_START_CAS_SAVE:
            case TAG_START_CAS_LOAD:
            case TAG_START_CAS_DLOAD:
            case TAG_SET_DOS_FILENAME:
            case TAG_BATCH_START_SAVE:
            case TAG_BATCH_END_SAVE:
            case TAG_START_XMODEM_SAVE:
            case TAG_START_XMODEM_LOAD:
            case TAG_START_YMODEM_SAVE:
            case TAG_START_HEXMODEM_SAVE:
            case TAG_START_HEXMODEM_LOAD:
            case TAG_START_BINMODEM_SAVE:
            case TAG_START_BINMODEM_LOAD:
            case TAG_STOP_MODEM:
            case TAG_CORRECT_CAPS:
            case TAG_CV_TAPE_PLAY:
            case TAG_CV_TAPE_FF:
            case TAG_CV_TAPE_REC:
            case TAG_FRED_TAPE_REC:
            case TAG_ELFOS_BOOT:
            case TAG_START_CHIP8:
            case TAG_CHECK_SW:
            case TAG_STUDIO_BUILD_IN_GAME:
            case TAG_VICTORY_BUILD_IN_GAME:
            case TAG_VISICOM_BUILD_IN_GAME:
            case TAG_SET_PSEUDO_AM4KBAS2020:
            case TAG_SET_PSEUDO_AM4KBAS1978:
                newInfo.type = tagTypeInt-TAG_RESET_STATE;

                newInfo.additionalAddress = getHexDec(child->GetAttribute("info"));

                if (child->HasAttribute("address"))
                {
                    addressLocations = child->GetAttribute("address");
                    addressValues = child->GetAttribute("value");
                    while (addressLocations != "")
                    {
                        newAddressInfo.checkAddress = getNextHexDec(&addressLocations);
                        newAddressInfo.checkValue = getNextHexDec(&addressValues);

                        if (newAddressInfo.checkAddress != 0)
                            newInfo.checkAddressInfo.push_back(newAddressInfo);
                    }
                }

                if (child->HasAttribute("reg"))
                {
                    addressLocations = child->GetAttribute("reg");
                    addressValues = child->GetAttribute("value");
                    while (addressLocations != "")
                    {
                        newRegInfo.checkReg = getNextHexDec(&addressLocations);
                        newRegInfo.checkValue = getNextHexDec(&addressValues);

                        if (newRegInfo.checkReg != 0)
                            newInfo.checkRegInfo.push_back(newRegInfo);
                    }
                }

                if (child->HasAttribute("command"))
                {
                    switch (tagTypeInt)
                    {
                        case TAG_RESET_STATE:
                            conf[computer].stateCommand_[RESETSTATE] = child->GetAttribute("command");
                        break;
                        case TAG_UT_STATE:
                            conf[computer].stateCommand_[UTSTATE] = child->GetAttribute("command");
                        break;
                        case TAG_BASIC_STATE:
                            conf[computer].stateCommand_[BASICSTATE] = child->GetAttribute("command");
                        break;
                        case TAG_BASIC_STATE_CW:
                            conf[computer].stateCommand_[BASICSTATECW] = child->GetAttribute("command");
                        break;
                    }
                }

                newtrigger = conf[computer].addressLocations.locationInfo.size();

                conf[computer].addressLocations.locationInfo.push_back(newInfo);

                addressLocations = child->GetNodeContent();
                while (addressLocations != "")
                {
                    address = getNextHexDec(&addressLocations);
                    if (address != 0)
                    {
//#if defined (__linux__)
//                        if (conf[computer].locationTrigger[address].index[0] == 0xff)
//#else
                        if (conf[computer].locationTrigger[address].index[0] == -1)
//#endif
                            conf[computer].locationTrigger[address].index[0] = newtrigger;
                        else
                            conf[computer].locationTrigger[address].index.push_back(newtrigger);
                    }
                }
                
                newInfo.checkAddressInfo.clear();
                newInfo.checkRegInfo.clear();
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Bootstrap(int computer, wxXmlNode &node)
{
    int number;
    
    wxString tagList[]=
    {
        "address",
        "in",
        "out",
        "io",
        "run",
        "timer",
        "read",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_ADDRESS,
        TAG_IN,
        TAG_OUT,
        TAG_IO,
        TAG_RUN,
        TAG_TIMER,
        TAG_READ,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    elfConfiguration[computer].bootStrap = true;
    elfConfiguration[computer].ioConfiguration.bootStrapType = BOOTSTRAPREAD;
    elfConfiguration[computer].ioConfiguration.bootStrapIn = -1;
    elfConfiguration[computer].ioConfiguration.bootStrapOut = -1;
    elfConfiguration[computer].ioConfiguration.bootStrapOut2 = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_ADDRESS:
                elfConfiguration[computer].strapAddress = getHexDec(child->GetNodeContent());
            break;

            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.bootStrapType = BOOTSTRAPOUT;
                if (elfConfiguration[computer].ioConfiguration.bootStrapOut == -1)
                    elfConfiguration[computer].ioConfiguration.bootStrapOut =  (int)parseXml_Number(*child);
                else
                    elfConfiguration[computer].ioConfiguration.bootStrapOut2 =  (int)parseXml_Number(*child);
            break;
                
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.bootStrapType = BOOTSTRAPIN;
                elfConfiguration[computer].ioConfiguration.bootStrapIn = (int)parseXml_Number(*child);
            break;
 
            case TAG_IO:
                number = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.bootStrapType = BOOTSTRAPIO;
                elfConfiguration[computer].ioConfiguration.bootStrapIn = number;
                elfConfiguration[computer].ioConfiguration.bootStrapOut = number;
            break;

            case TAG_RUN:
                elfConfiguration[computer].ioConfiguration.bootStrapType = BOOTSTRAPRUN;
            break;

            case TAG_TIMER:
                elfConfiguration[computer].ioConfiguration.bootStrapType = BOOTSTRAPTIMER;
                elfConfiguration[computer].ioConfiguration.bootStrapTimer = (int)parseXml_Number(*child);
            break;

            case TAG_READ:
                elfConfiguration[computer].ioConfiguration.bootStrapType = BOOTSTRAPREAD;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_IdeDisk(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].ideEnabled = true;
    elfConfiguration[computer].ioConfiguration.ideStatus = -1;

    wxString tagList[]=
    {
        "filename",
        "dirname",
        "out",
        "in",
        "tracks",
        "heads",
        "sectors",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_OUT,
        TAG_IN,
        TAG_TRACKS,
        TAG_HEADS,
        TAG_SECTORS,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    elfConfiguration[computer].ioConfiguration.ideTracks = 2048;
    elfConfiguration[computer].ioConfiguration.ideHeads = 16;
    elfConfiguration[computer].ioConfiguration.ideSectors = 64;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_FILENAME:
                conf[computer].ide_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                ideFileDirDefined_ = true;
                conf[computer].ideDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].ideDir_.Right(1) != pathSeparator_)
                    conf[computer].ideDir_ += pathSeparator_;
            break;

            case TAG_IN:
                if (child->GetAttribute("type") == "status")
                    elfConfiguration[computer].ioConfiguration.ideStatus = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "read")
                    elfConfiguration[computer].ioConfiguration.ideInput = (int)parseXml_Number(*child);
            break;

           case TAG_OUT:
                if (child->GetAttribute("type") == "select")
                    elfConfiguration[computer].ioConfiguration.ideSelectOutput = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "write")
                    elfConfiguration[computer].ioConfiguration.ideWriteOutput = (int)parseXml_Number(*child);
            break;
                
            case TAG_TRACKS:
                elfConfiguration[computer].ioConfiguration.ideTracks = (int)parseXml_Number(*child);
            break;

            case TAG_HEADS:
                elfConfiguration[computer].ioConfiguration.ideHeads = (int)parseXml_Number(*child);
            break;

            case TAG_SECTORS:
                elfConfiguration[computer].ioConfiguration.ideSectors = (int)parseXml_Number(*child);
            break;

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.ideIoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_FdcDisk(int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "sides",
        "tracks",
        "sectors",
        "sectorlength",
        "maxfmtcount",
        "iogroup",
        "filename",
        "dirname",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_SIDES,
        TAG_TRACKS,
        TAG_SECTORS,
        TAG_SECTOR_LENGTH,
        TAG_MAX_FMT_COUNT,
        TAG_IOGROUP,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt, diskNumber;
    
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_IN:
                if (child->GetAttribute("type") == "read")
                {
                    elfConfiguration[computer].ioConfiguration.fdcReadInput.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.fdcReadInput.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.fdcReadInput.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.fdcReadInput.qValue = 0;
                }
                if (child->GetAttribute("type") == "intrq")
                {
                    elfConfiguration[computer].ioConfiguration.fdcIntrqInput.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.fdcIntrqInput.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.fdcIntrqInput.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.fdcIntrqInput.qValue = 0;
                }
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "select")
                {
                    elfConfiguration[computer].ioConfiguration.fdcSelectOutput.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.fdcSelectOutput.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.fdcSelectOutput.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.fdcSelectOutput.qValue = 0;
                }
                if (child->GetAttribute("type") == "write")
                {
                    elfConfiguration[computer].ioConfiguration.fdcWriteOutput.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.fdcWriteOutput.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.fdcWriteOutput.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.fdcWriteOutput.qValue = 0;
                }
            break;
                
            case TAG_EF:
                    elfConfiguration[computer].ioConfiguration.fdcEf = (int)parseXml_Number(*child);
            break;
                
            case TAG_SIDES:
                elfConfiguration[computer].ioConfiguration.fdcSides = (int)parseXml_Number(*child);
            break;

            case TAG_TRACKS:
                elfConfiguration[computer].ioConfiguration.fdcTracks = (int)parseXml_Number(*child);;
            break;

            case TAG_SECTORS:
                elfConfiguration[computer].ioConfiguration.fdcSectors = (int)parseXml_Number(*child);
            break;

            case TAG_SECTOR_LENGTH:
                elfConfiguration[computer].ioConfiguration.fdcSectorLength = (int)parseXml_Number(*child);
            break;

            case TAG_MAX_FMT_COUNT:
                elfConfiguration[computer].ioConfiguration.fdcMaxFmtCount = (int)parseXml_Number(*child);
            break;

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.fdcIoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_FILENAME:
                diskNumber = (int)parseXml_Number(*child, "disk");
                floppy_[FDCTYPE_17XX][diskNumber] = child->GetNodeContent();
                floppyDefined[FDCTYPE_17XX][diskNumber] = true;
            break;

            case TAG_DIRNAME:
                diskNumber = (int)parseXml_Number(*child, "disk");
                floppyDirDefined[FDCTYPE_17XX][diskNumber] = true;
                floppyDir_[FDCTYPE_17XX][diskNumber] = dataDir_ + child->GetNodeContent();
                if (floppyDir_[FDCTYPE_17XX][diskNumber].Right(1) != pathSeparator_)
                    floppyDir_[FDCTYPE_17XX][diskNumber] += pathSeparator_;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Upd765(int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "iogroup",
        "filename",
        "dirname",
        "hdname",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_IOGROUP,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_HDNAME,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    elfConfiguration[computer].fdcType_ = FDCTYPE_MICROBOARD;
    elfConfiguration[computer].useUpd765 = true;
    elfConfiguration[computer].ioConfiguration.upd765Io.readStatus = init_IoPort();
    elfConfiguration[computer].ioConfiguration.upd765Io.readCommand = init_IoPort();
    elfConfiguration[computer].ioConfiguration.upd765Io.dmaControl = init_IoPort();
    elfConfiguration[computer].ioConfiguration.upd765Io.dmaCount = init_IoPort();
    elfConfiguration[computer].ioConfiguration.upd765Io.writeCommand = init_IoPort();
    
    int tagTypeInt, diskNumber;
    
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_IN:
                if (child->GetAttribute("type") == "status")
                    elfConfiguration[computer].ioConfiguration.upd765Io.readStatus = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "command")
                    elfConfiguration[computer].ioConfiguration.upd765Io.readCommand = parseXml_IoPort(*child);
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "control")
                    elfConfiguration[computer].ioConfiguration.upd765Io.dmaControl = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "count")
                    elfConfiguration[computer].ioConfiguration.upd765Io.dmaCount = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "command")
                    elfConfiguration[computer].ioConfiguration.upd765Io.writeCommand = parseXml_IoPort(*child);
            break;
                
            case TAG_EF:
                    elfConfiguration[computer].ioConfiguration.upd765Io.efInterrupt  = (int)parseXml_Number(*child);
            break;
                
            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.upd765Io.ioGroup = (int)parseXml_Number(*child);
            break;

            case TAG_FILENAME:
                diskNumber = (int)parseXml_Number(*child, "disk");
                floppy_[FDCTYPE_MICROBOARD][diskNumber] = child->GetNodeContent();
                floppyDefined[FDCTYPE_MICROBOARD][diskNumber] = true;
            break;

            case TAG_DIRNAME:
                diskNumber = (int)parseXml_Number(*child, "disk");
                floppyDirDefined[FDCTYPE_MICROBOARD][diskNumber] = true;
                floppyDir_[FDCTYPE_MICROBOARD][diskNumber] = dataDir_ + child->GetNodeContent();
                if (floppyDir_[FDCTYPE_MICROBOARD][diskNumber].Right(1) != pathSeparator_)
                    floppyDir_[FDCTYPE_MICROBOARD][diskNumber] += pathSeparator_;
            break;

            case TAG_HDNAME:
                diskNumber = (int)parseXml_Number(*child, "disk");
                floppyDirSwitched_[FDCTYPE_MICROBOARD][diskNumber] = conf[computer].mainDir_ + child->GetNodeContent();
                if (floppyDirSwitched_[FDCTYPE_MICROBOARD][diskNumber].Right(1) != pathSeparator_)
                    floppyDirSwitched_[FDCTYPE_MICROBOARD][diskNumber] += pathSeparator_;
                if (child->GetAttribute("default") == "yes")
                    directoryMode_[FDCTYPE_MICROBOARD][diskNumber] = true;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_CoinVideo(int computer, wxXmlNode &node)
{
    if (!elfConfiguration[computer].useCoinVideo)
    {
        elfConfiguration[computer].ioConfiguration.coinVideoNumber = conf[computer].numberOfVideoTypes_;
        conf[computer].numberOfVideoTypes_++;
    }

    elfConfiguration[computer].useCoinVideo = true;
    elfConfiguration[computer].ioConfiguration.coinIoGroup = -1;

    wxString tagList[]=
    {
        "out",
        "zoom",
        "border",
        "pos",
        "xscale",
        "color",
        "graphics",
        "iogroup",
        "height",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_XSCALE,
        TAG_COLOR,
        TAG_GRAPHICS,
        TAG_IOGROUP,
        TAG_HEIGHT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos, xScale = 3;
    wxString color, scale, position;

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.coinVideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.coinVideoNumber] = "Coin Video";

    elfConfiguration[computer].ioConfiguration.pixieGraphics.interrupt = 62;
    elfConfiguration[computer].ioConfiguration.pixieGraphics.start = 72;
    elfConfiguration[computer].ioConfiguration.pixieGraphics.end = 199;
    elfConfiguration[computer].ioConfiguration.pixieGraphics.screenend = 322;
    elfConfiguration[computer].ioConfiguration.videoHeight = 128;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.coinOutput = (int)parseXml_Number(*child);
            break;
                                
            case TAG_ZOOM:
                conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.coinVideoNumber] = child->GetNodeContent();
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOCOIN] = (int)width/xScale;
                    screenInfo[computer].borderY[VIDEOCOIN] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defCoinX_;
                if (ypos != -1)
                    ypos += conf[computer].defCoinY_;
                conf[computer].defCoinX_ = xpos;
                conf[computer].defCoinY_ = ypos;
            break;

            case TAG_XSCALE:
                screenInfo[computer].borderX[VIDEOCOIN] = screenInfo[computer].borderX[VIDEOCOIN] * xScale;
                conf[computer].xScale_ = convertLocale(child->GetNodeContent());
                xScale = (int)parseXml_Number(*child);
                screenInfo[computer].borderX[VIDEOCOIN] = (int)screenInfo[computer].borderX[VIDEOCOIN] / xScale;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo[computer].defaultColour[COL_PIXIE_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo[computer].defaultColour[COL_PIXIE_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.coinIoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_GRAPHICS:
                parseXml_pixieGraphics (computer, *child);
            break;

            case TAG_HEIGHT:
                elfConfiguration[computer].ioConfiguration.videoHeight = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_PixieVideo(int computer, wxXmlNode &node)
{
    if (!elfConfiguration[computer].usePixie)
    {
        elfConfiguration[computer].ioConfiguration.pixieVideoNumber = conf[computer].numberOfVideoTypes_;
        conf[computer].numberOfVideoTypes_++;
    }

    elfConfiguration[computer].usePixie = true;

    wxString tagList[]=
    {
        "io",
        "out",
        "in",
        "ef",
        "zoom",
        "border",
        "pos",
        "xscale",
        "color",
        "colortype",
        "graphics",
        "highres",
        "height",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IO,
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_XSCALE,
        TAG_COLOR,
        TAG_COLOR_TYPE,
        TAG_GRAPHICS,
        TAG_HIGH_RES,
        TAG_HEIGHT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos, xScale = 3;
    wxString color, scale, position;

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.pixieVideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.pixieVideoNumber] = "1861 Pixie";

    elfConfiguration[computer].ioConfiguration.pixieGraphics.interrupt = 62;
    elfConfiguration[computer].ioConfiguration.pixieGraphics.start = 64;
    elfConfiguration[computer].ioConfiguration.pixieGraphics.end = 191;
    elfConfiguration[computer].ioConfiguration.pixieGraphics.screenend = 262;
    elfConfiguration[computer].ioConfiguration.videoHeight = 128;
    
    elfConfiguration[computer].ioConfiguration.pixieInput = -1;
    elfConfiguration[computer].ioConfiguration.pixieOutput = -1;
    elfConfiguration[computer].ioConfiguration.pixieEf = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IO:
                if (child->GetAttribute("type") == "on")
                    elfConfiguration[computer].ioConfiguration.pixieInput = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "off")
                    elfConfiguration[computer].ioConfiguration.pixieOutput = (int)parseXml_Number(*child);
                elfConfiguration[computer].ioConfiguration.pixieDoubleScreenIo = PIXIE_DOUBLE;
            break;
                
            case TAG_IN:
                if (child->HasAttribute("type"))
                {
                    elfConfiguration[computer].ioConfiguration.pixieDoubleScreenIo = PIXIE_IN_IN;
                    if (child->GetAttribute("type") == "on")
                        elfConfiguration[computer].ioConfiguration.pixieInput = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "off")
                        elfConfiguration[computer].ioConfiguration.pixieOutput = (int)parseXml_Number(*child);
                }
                else
                    elfConfiguration[computer].ioConfiguration.pixieInput = (int)parseXml_Number(*child);
            break;
                
            case TAG_OUT:
                if (child->HasAttribute("type"))
                {
                    elfConfiguration[computer].ioConfiguration.pixieDoubleScreenIo = PIXIE_OUT_OUT;
                    if (child->GetAttribute("type") == "on")
                        elfConfiguration[computer].ioConfiguration.pixieInput = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "off")
                        elfConfiguration[computer].ioConfiguration.pixieOutput = (int)parseXml_Number(*child);
                }
                else
                    elfConfiguration[computer].ioConfiguration.pixieOutput = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "on")
                    elfConfiguration[computer].ioConfiguration.pixieEfScreenOn = true;
                elfConfiguration[computer].ioConfiguration.pixieEf = (int)parseXml_Number(*child);
            break;
                
            case TAG_ZOOM:
                conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.pixieVideoNumber] = child->GetNodeContent();
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOXMLPIXIE] = (int)width/xScale;
                    screenInfo[computer].borderY[VIDEOXMLPIXIE] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defPixieX_;
                if (ypos != -1)
                    ypos += conf[computer].defPixieY_;
                conf[computer].defPixieX_ = xpos;
                conf[computer].defPixieY_ = ypos;
            break;

            case TAG_XSCALE:
                screenInfo[computer].borderX[VIDEOXMLPIXIE] = screenInfo[computer].borderX[VIDEOXMLPIXIE] * xScale;
                conf[computer].xScale_ = convertLocale(child->GetNodeContent());
                xScale = (int)parseXml_Number(*child);
                screenInfo[computer].borderX[VIDEOXMLPIXIE] = (int)screenInfo[computer].borderX[VIDEOXMLPIXIE] / xScale;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo[computer].defaultColour[COL_PIXIE_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "fore1")
                    screenInfo[computer].defaultColour[COL_PIXIE_VISICOM_COL1].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "fore2")
                    screenInfo[computer].defaultColour[COL_PIXIE_VISICOM_COL2].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "fore3")
                    screenInfo[computer].defaultColour[COL_PIXIE_VISICOM_COL3].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo[computer].defaultColour[COL_PIXIE_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_COLOR_TYPE:
                if (child->GetNodeContent() == "vis")
                    elfConfiguration[computer].ioConfiguration.pixieColorType = PIXIE_COLOR_VISICOM;
            break;

            case TAG_GRAPHICS:
                parseXml_pixieGraphics (computer, *child);
            break;

            case TAG_HIGH_RES:
                elfConfiguration[computer].ioConfiguration.pixieHighRes = true;
            break;

            case TAG_HEIGHT:
                elfConfiguration[computer].ioConfiguration.videoHeight = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_1862Video(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].ioConfiguration.use1862 = true;

    wxString tagList[]=
    {
        "out",
        "colorram",
        "colortype",
        "color",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_COLOR_RAM,
        TAG_COLOR_TYPE,
        TAG_COLOR,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    long start, end;
    int tagTypeInt;
    int red, green, blue;
    wxString color, scale, position;
    bool whiteDefined = false, blackDefined = false;

    elfConfiguration[computer].ioConfiguration.cdp1862background.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.cdp1862IoGroup = -1;
    elfConfiguration[computer].ioConfiguration.cdp1862colorMemory.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.cdp1862StartRam = -1;
    elfConfiguration[computer].ioConfiguration.cdp1862EndRam = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_COLOR_RAM:
                if (!parseXml_Range(*child, &start, &end))
                {
                    warningText_ += "No CDP1862 Colour RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    elfConfiguration[computer].ioConfiguration.cdp1862StartRam = (int)start;
                    elfConfiguration[computer].ioConfiguration.cdp1862EndRam = (int)end;
                }
            break;

            case TAG_COLOR_TYPE:
                if (child->GetNodeContent() == "vip")
                    elfConfiguration[computer].ioConfiguration.cdp1862ColorType = PIXIE_COLOR_VIP_1862;
                if (child->GetNodeContent() == "eti" || child->GetNodeContent() == "hug" || child->GetNodeContent() == "hec")
                    elfConfiguration[computer].ioConfiguration.cdp1862ColorType = PIXIE_COLOR_ETI_1862;
                if (child->GetNodeContent() == "vic")
                {
                    elfConfiguration[computer].ioConfiguration.cdp1862ColorType = PIXIE_COLOR_VICTORY_1862;
                    if (whiteDefined && blackDefined)
                    {
                        color = screenInfo[computer].defaultColour[COL_CDP1862_BLACK];
                        screenInfo[computer].defaultColour[COL_CDP1862_BLACK] = screenInfo[computer].defaultColour[COL_CDP1862_WHITE];
                        screenInfo[computer].defaultColour[COL_CDP1862_WHITE] = color;
                    }
                    else
                    {
                        if (whiteDefined)
                            screenInfo[computer].defaultColour[COL_CDP1862_BLACK] = screenInfo[computer].defaultColour[COL_CDP1862_WHITE];
                        if (blackDefined)
                            screenInfo[computer].defaultColour[COL_CDP1862_WHITE] = screenInfo[computer].defaultColour[COL_CDP1862_BLACK];
                    }
                }

                if (child->GetNodeContent() == "tmc")
                    elfConfiguration[computer].ioConfiguration.cdp1862ColorType = PIXIE_COLOR_TMC2000_1862;
                if (child->GetNodeContent() == "cos")
                    elfConfiguration[computer].ioConfiguration.cdp1862ColorType = PIXIE_COLOR_DEFAULT;
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "back")
                {
                    elfConfiguration[computer].ioConfiguration.cdp1862background.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.cdp1862background.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.cdp1862background.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.cdp1862background.qValue = 0;
                }
                if (child->GetAttribute("type") == "color_ram")
                {
                    elfConfiguration[computer].ioConfiguration.cdp1862colorMemory.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.cdp1862colorMemory.mask = 0xff;
                    if (child->HasAttribute("mask"))
                        elfConfiguration[computer].ioConfiguration.cdp1862colorMemory.mask = (int)parseXml_Number(*child, "mask");
                    elfConfiguration[computer].ioConfiguration.cdp1862colorMemory.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.cdp1862colorMemory.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.cdp1862colorMemory.qValue = 0;
                }
            break;
                
            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo[computer].defaultColour[COL_CDP1862_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo[computer].defaultColour[COL_CDP1862_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);

                if (child->GetAttribute("type") == "red")
                    screenInfo[computer].defaultColour[COL_CDP1862_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "blue")
                    screenInfo[computer].defaultColour[COL_CDP1862_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "magenta")
                    screenInfo[computer].defaultColour[COL_CDP1862_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "green")
                    screenInfo[computer].defaultColour[COL_CDP1862_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "yellow")
                    screenInfo[computer].defaultColour[COL_CDP1862_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "cyan")
                    screenInfo[computer].defaultColour[COL_CDP1862_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_blue")
                    screenInfo[computer].defaultColour[COL_CDP1862_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_black")
                    screenInfo[computer].defaultColour[COL_CDP1862_BACK_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_green")
                    screenInfo[computer].defaultColour[COL_CDP1862_BACK_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_red")
                    screenInfo[computer].defaultColour[COL_CDP1862_BACK_RED].Printf("#%02X%02X%02X", red, green, blue);

                if (elfConfiguration[computer].ioConfiguration.cdp1862ColorType == PIXIE_COLOR_VICTORY_1862)
                {
                    if (child->GetAttribute("type") == "white")
                        screenInfo[computer].defaultColour[COL_CDP1862_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                    if (child->GetAttribute("type") == "black")
                        screenInfo[computer].defaultColour[COL_CDP1862_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                }
                else
                {
                    if (child->GetAttribute("type") == "white")
                    {
                        whiteDefined = true;
                        screenInfo[computer].defaultColour[COL_CDP1862_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                    }
                    if (child->GetAttribute("type") == "black")
                    {
                        blackDefined = true;
                        screenInfo[computer].defaultColour[COL_CDP1862_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                    }
                }
            break;
                
            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.cdp1862IoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_1864Video(int computer, wxXmlNode &node)
{
    if (!elfConfiguration[computer].use1864)
    {
        elfConfiguration[computer].ioConfiguration.cdp1864VideoNumber = conf[computer].numberOfVideoTypes_;
        conf[computer].numberOfVideoTypes_++;
    }

    elfConfiguration[computer].use1864 = true;

    wxString tagList[]=
    {
        "io",
        "out",
        "in",
        "ef",
        "zoom",
        "border",
        "pos",
        "xscale",
        "colortype",
        "color",
        "colorram",
        "iogroup",
        "graphics",
        "height",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IO,
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_XSCALE,
        TAG_COLOR_TYPE,
        TAG_COLOR,
        TAG_COLOR_RAM,
        TAG_IOGROUP,
        TAG_GRAPHICS,
        TAG_HEIGHT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    long start, end;
    int red, green, blue, xpos, ypos, xScale = 4;
    wxString color, scale, position;
    bool whiteDefined = false, blackDefined = false;

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.cdp1864VideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.cdp1864VideoNumber] = "CDP1864";
    elfConfiguration[computer].ioConfiguration.cdp1864disable.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.cdp1864enable.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.cdp1864colorMemory.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.cdp1864background.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.cdp1864IoGroup = -1;
    elfConfiguration[computer].ioConfiguration.cdp1864StartRam = -1;
    elfConfiguration[computer].ioConfiguration.cdp1864RamMask = 0xff;
    elfConfiguration[computer].ioConfiguration.cdp1864ColorLatch = false;
    elfConfiguration[computer].ioConfiguration.cdp1864EndRam = -1;

    elfConfiguration[computer].ioConfiguration.pixieGraphics.interrupt = 62;
    elfConfiguration[computer].ioConfiguration.pixieGraphics.start = 64;
    elfConfiguration[computer].ioConfiguration.pixieGraphics.end = 191;
    elfConfiguration[computer].ioConfiguration.pixieGraphics.screenend = 262;
    elfConfiguration[computer].ioConfiguration.videoHeight = 192;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                if (child->GetAttribute("type") == "off")
                {
                    elfConfiguration[computer].ioConfiguration.cdp1864disable.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.cdp1864disable.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.cdp1864disable.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.cdp1864disable.qValue = 0;
                }
                if (child->GetAttribute("type") == "on")
                {
                    elfConfiguration[computer].ioConfiguration.cdp1864enable.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.cdp1864enable.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.cdp1864enable.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.cdp1864enable.qValue = 0;
                }
            break;
                
            case TAG_COLOR_TYPE:
                if (child->GetNodeContent() == "vip")
                    elfConfiguration[computer].ioConfiguration.cdp1864ColorType = PIXIE_COLOR_VIP_1864;
                if (child->GetNodeContent() == "eti" || child->GetNodeContent() == "hug" || child->GetNodeContent() == "hec")
                    elfConfiguration[computer].ioConfiguration.cdp1864ColorType = PIXIE_COLOR_ETI_1864;
                if (child->GetNodeContent() == "vic")
                {
                    elfConfiguration[computer].ioConfiguration.cdp1864ColorType = PIXIE_COLOR_VICTORY_1864;
                    if (whiteDefined && blackDefined)
                    {
                        color = screenInfo[computer].defaultColour[COL_CDP1864_BLACK];
                        screenInfo[computer].defaultColour[COL_CDP1864_BLACK] = screenInfo[computer].defaultColour[COL_CDP1864_WHITE];
                        screenInfo[computer].defaultColour[COL_CDP1864_WHITE] = color;
                    }
                    else
                    {
                        if (whiteDefined)
                            screenInfo[computer].defaultColour[COL_CDP1864_BLACK] = screenInfo[computer].defaultColour[COL_CDP1864_WHITE];
                        if (blackDefined)
                            screenInfo[computer].defaultColour[COL_CDP1864_WHITE] = screenInfo[computer].defaultColour[COL_CDP1864_BLACK];
                    }
                }
                if (child->GetNodeContent() == "tmc")
                    elfConfiguration[computer].ioConfiguration.cdp1864ColorType = PIXIE_COLOR_TMC2000_1864;
                if (child->GetNodeContent() == "cos")
                    elfConfiguration[computer].ioConfiguration.cdp1864ColorType = PIXIE_COLOR_DEFAULT;
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "back")
                {
                    elfConfiguration[computer].ioConfiguration.cdp1864background.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.cdp1864background.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.cdp1864background.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.cdp1864background.qValue = 0;
                }
                if (child->GetAttribute("type") == "tone")
                {
                    conf[computer].soundType_ = SOUND_1863_1864;
                    elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.qValue = 0;
                }
                if (child->GetAttribute("type") == "color_ram")
                {
                    elfConfiguration[computer].ioConfiguration.cdp1864colorMemory.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.cdp1864colorMemory.mask = 0xff;
                    if (child->HasAttribute("mask"))
                        elfConfiguration[computer].ioConfiguration.cdp1864colorMemory.mask = (int)parseXml_Number(*child, "mask");
                    elfConfiguration[computer].ioConfiguration.cdp1864colorMemory.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.cdp1864colorMemory.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.cdp1864colorMemory.qValue = 0;
                }
                if (child->GetAttribute("type") == "color_tone")
                {
                    elfConfiguration[computer].ioConfiguration.cdp1864ColorLatch = true;
                    conf[computer].soundType_ = SOUND_1863_1864;
                    elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.qValue = 0;
                }
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "on")
                    elfConfiguration[computer].ioConfiguration.cdp1864EfScreenOn = true;
                elfConfiguration[computer].ioConfiguration.cdp1864Ef = (int)parseXml_Number(*child);
            break;
                
            case TAG_COLOR_RAM:
                if (!parseXml_Range(*child, &start, &end))
                {
                    warningText_ += "No CDP1864 Colour RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    elfConfiguration[computer].ioConfiguration.cdp1864StartRam = (int)start;
                    elfConfiguration[computer].ioConfiguration.cdp1864EndRam = (int)end;
                }
                if (child->HasAttribute("mask"))
                    elfConfiguration[computer].ioConfiguration.cdp1864RamMask = (int)parseXml_Number(*child, "mask") & 0x3ff;
            break;

            case TAG_ZOOM:
                conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.cdp1864VideoNumber] = child->GetNodeContent();
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOXML1864] = (int)width/xScale;
                    screenInfo[computer].borderY[VIDEOXML1864] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defCdp1864X_;
                if (ypos != -1)
                    ypos += conf[computer].defCdp1864Y_;
                conf[computer].defCdp1864X_ = xpos;
                conf[computer].defCdp1864Y_ = ypos;
            break;

            case TAG_XSCALE:
                screenInfo[computer].borderX[VIDEOXML1864] = screenInfo[computer].borderX[VIDEOXML1864] * xScale;
                conf[computer].xScale_ = convertLocale(child->GetNodeContent());
                xScale = (int)parseXml_Number(*child);
                screenInfo[computer].borderX[VIDEOXML1864] = (int)screenInfo[computer].borderX[VIDEOXML1864] / xScale;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo[computer].defaultColour[COL_CDP1864_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo[computer].defaultColour[COL_CDP1864_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);

                if (child->GetAttribute("type") == "red")
                    screenInfo[computer].defaultColour[COL_CDP1864_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "blue")
                    screenInfo[computer].defaultColour[COL_CDP1864_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "magenta")
                    screenInfo[computer].defaultColour[COL_CDP1864_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "green")
                    screenInfo[computer].defaultColour[COL_CDP1864_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "yellow")
                    screenInfo[computer].defaultColour[COL_CDP1864_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "cyan")
                    screenInfo[computer].defaultColour[COL_CDP1864_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_blue")
                    screenInfo[computer].defaultColour[COL_CDP1864_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_black")
                    screenInfo[computer].defaultColour[COL_CDP1864_BACK_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_green")
                    screenInfo[computer].defaultColour[COL_CDP1864_BACK_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_red")
                    screenInfo[computer].defaultColour[COL_CDP1864_BACK_RED].Printf("#%02X%02X%02X", red, green, blue);

                if (elfConfiguration[computer].ioConfiguration.cdp1864ColorType == PIXIE_COLOR_VICTORY_1864)
                {
                    if (child->GetAttribute("type") == "white")
                        screenInfo[computer].defaultColour[COL_CDP1864_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                    if (child->GetAttribute("type") == "black")
                        screenInfo[computer].defaultColour[COL_CDP1864_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                }
                else
                {
                    if (child->GetAttribute("type") == "white")
                    {
                        whiteDefined = true;
                        screenInfo[computer].defaultColour[COL_CDP1864_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                    }
                    if (child->GetAttribute("type") == "black")
                    {
                        blackDefined = true;
                        screenInfo[computer].defaultColour[COL_CDP1864_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                    }
                }
            break;
                
            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.cdp1864IoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_GRAPHICS:
                parseXml_pixieGraphics (computer, *child);
            break;

            case TAG_HEIGHT:
                elfConfiguration[computer].ioConfiguration.videoHeight = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_St4Video(int computer, wxXmlNode &node)
{
    if (!elfConfiguration[computer].useSt4Video)
    {
        elfConfiguration[computer].ioConfiguration.st4VideoNumber = conf[computer].numberOfVideoTypes_;
        conf[computer].numberOfVideoTypes_++;
    }

    elfConfiguration[computer].useSt4Video = true;

    wxString tagList[]=
    {
        "out",
        "ef",
        "zoom",
        "border",
        "pos",
        "xscale",
        "color",
        "colorram",
        "iogroup",
        "graphics",
        "height",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_EF,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_XSCALE,
        TAG_COLOR,
        TAG_COLOR_RAM,
        TAG_IOGROUP,
        TAG_GRAPHICS,
        TAG_HEIGHT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    long start, end;
    int red, green, blue, xpos, ypos, xScale = 4;
    wxString color, scale, position;

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.st4VideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.st4VideoNumber] = "Studio IV";
    elfConfiguration[computer].ioConfiguration.st4VideoDmaEnable.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.st4VideoOut1.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.st4VideoOut2.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.st4VideoEf = -1;
    elfConfiguration[computer].ioConfiguration.st4VideoIoGroup = -1;
    elfConfiguration[computer].ioConfiguration.st4VideoStartRam = -1;
    elfConfiguration[computer].ioConfiguration.st4VideoEndRam = -1;

    elfConfiguration[computer].ioConfiguration.pixieGraphics.interrupt = 62;
    elfConfiguration[computer].ioConfiguration.pixieGraphics.start = 64;
    elfConfiguration[computer].ioConfiguration.pixieGraphics.end = 191;
    elfConfiguration[computer].ioConfiguration.pixieGraphics.screenend = 262;
    elfConfiguration[computer].ioConfiguration.videoHeight = 192;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                if (child->GetAttribute("type") == "color")
                {
                    if (elfConfiguration[computer].ioConfiguration.st4VideoOut1.portNumber != -1)
                    {
                        elfConfiguration[computer].ioConfiguration.st4VideoOut2.qValue = -1;
                        elfConfiguration[computer].ioConfiguration.st4VideoOut2.portNumber = (int)parseXml_Number(*child);
                        if (child->GetAttribute("q") == "1")
                            elfConfiguration[computer].ioConfiguration.st4VideoOut2.qValue = 1;
                        if (child->GetAttribute("q") == "0")
                            elfConfiguration[computer].ioConfiguration.st4VideoOut2.qValue = 0;
                    }
                    else
                    {
                        elfConfiguration[computer].ioConfiguration.st4VideoOut1.qValue = -1;
                        elfConfiguration[computer].ioConfiguration.st4VideoOut1.portNumber = (int)parseXml_Number(*child);
                        if (child->GetAttribute("q") == "1")
                            elfConfiguration[computer].ioConfiguration.st4VideoOut1.qValue = 1;
                        if (child->GetAttribute("q") == "0")
                            elfConfiguration[computer].ioConfiguration.st4VideoOut1.qValue = 0;
                    }
                }
                if (child->GetAttribute("type") == "dma")
                {
                    elfConfiguration[computer].ioConfiguration.st4VideoDmaEnable.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.st4VideoDmaEnable.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.st4VideoDmaEnable.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.st4VideoDmaEnable.qValue = 0;
                }
            break;
                
            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.st4VideoEf = (int)parseXml_Number(*child);
            break;
                
            case TAG_COLOR_RAM:
                if (!parseXml_Range(*child, &start, &end))
                {
                    warningText_ += "No Studio IV Colour RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    elfConfiguration[computer].ioConfiguration.st4VideoStartRam = (int)start;
                    elfConfiguration[computer].ioConfiguration.st4VideoEndRam = (int)end;
                }
            break;

            case TAG_ZOOM:
                conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.st4VideoNumber] = child->GetNodeContent();
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOSTUDIOIV] = (int)width/xScale;
                    screenInfo[computer].borderY[VIDEOSTUDIOIV] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defSt4X_;
                if (ypos != -1)
                    ypos += conf[computer].defSt4Y_;
                conf[computer].defSt4X_ = xpos;
                conf[computer].defSt4Y_ = ypos;
            break;

            case TAG_XSCALE:
                screenInfo[computer].borderX[VIDEOSTUDIOIV] = screenInfo[computer].borderX[VIDEOSTUDIOIV] * xScale;
                conf[computer].xScale_ = convertLocale(child->GetNodeContent());
                xScale = (int)parseXml_Number(*child);
                screenInfo[computer].borderX[VIDEOSTUDIOIV] = (int)screenInfo[computer].borderX[VIDEOSTUDIOIV] / xScale;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "black")
                    screenInfo[computer].defaultColour[COL_ST4_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "red")
                    screenInfo[computer].defaultColour[COL_ST4_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "blue")
                    screenInfo[computer].defaultColour[COL_ST4_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "magenta")
                    screenInfo[computer].defaultColour[COL_ST4_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "green")
                    screenInfo[computer].defaultColour[COL_ST4_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "yellow")
                    screenInfo[computer].defaultColour[COL_ST4_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "cyan")
                    screenInfo[computer].defaultColour[COL_ST4_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "white")
                    screenInfo[computer].defaultColour[COL_ST4_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_black")
                    screenInfo[computer].defaultColour[COL_ST4_BACK_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_red")
                    screenInfo[computer].defaultColour[COL_ST4_BACK_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_blue")
                    screenInfo[computer].defaultColour[COL_ST4_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_magenta")
                    screenInfo[computer].defaultColour[COL_ST4_BACK_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_green")
                    screenInfo[computer].defaultColour[COL_ST4_BACK_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_yellow")
                    screenInfo[computer].defaultColour[COL_ST4_BACK_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_cyan")
                    screenInfo[computer].defaultColour[COL_ST4_BACK_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_white")
                    screenInfo[computer].defaultColour[COL_ST4_BACK_WHITE].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.st4VideoIoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_GRAPHICS:
                parseXml_pixieGraphics (computer, *child);
            break;

            case TAG_HEIGHT:
                elfConfiguration[computer].ioConfiguration.videoHeight = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Vip2KVideo(int computer, wxXmlNode &node)
{
    if (!elfConfiguration[computer].useVip2KVideo)
    {
        elfConfiguration[computer].ioConfiguration.vip2KVideoNumber = conf[computer].numberOfVideoTypes_;
        conf[computer].numberOfVideoTypes_++;
    }

    elfConfiguration[computer].useVip2KVideo = true;

    wxString tagList[]=
    {
        "io",
        "out",
        "in",
        "ef",
        "zoom",
        "border",
        "pos",
        "xscale",
        "color",
        "height",
        "width",
        "filename",
        "dirname",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IO,
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_XSCALE,
        TAG_COLOR,
        TAG_HEIGHT,
        TAG_WIDTH,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, scale, position;

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.vip2KVideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.vip2KVideoNumber] = "VIP2K";
    elfConfiguration[computer].ioConfiguration.videoHeight = 198;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IO:
                if (child->GetAttribute("type") == "on")
                    elfConfiguration[computer].ioConfiguration.vip2KInput = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "off")
                    elfConfiguration[computer].ioConfiguration.vip2KOutput = (int)parseXml_Number(*child);
                elfConfiguration[computer].ioConfiguration.vip2KDoubleScreenIo = true;
            break;
                
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.vip2KInput = (int)parseXml_Number(*child);
            break;
                
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.vip2KOutput = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                    elfConfiguration[computer].ioConfiguration.vip2KEf = (int)parseXml_Number(*child);
            break;
                
            case TAG_ZOOM:
                conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.vip2KVideoNumber] = child->GetNodeContent();
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOVIP2K] = (int)width;
                    screenInfo[computer].borderY[VIDEOVIP2K] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defVip2KX_;
                if (ypos != -1)
                    ypos += conf[computer].defVip2KY_;
                conf[computer].defVip2KX_ = xpos;
                conf[computer].defVip2KY_ = ypos;
            break;

            case TAG_XSCALE:
                conf[computer].xScale_ = convertLocale(child->GetNodeContent());
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo[computer].defaultColour[COL_PIXIE_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo[computer].defaultColour[COL_PIXIE_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_HEIGHT:
                elfConfiguration[computer].ioConfiguration.videoHeight = (int)parseXml_Number(*child);
            break;

            case TAG_WIDTH:
                elfConfiguration[computer].ioConfiguration.videoWidth = (int)parseXml_Number(*child);
            break;

            case TAG_FILENAME:
                conf[computer].sequencerFile_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                sequencerFileDirDefined_ = true;
                conf[computer].sequencerDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].sequencerDir_.Right(1) != pathSeparator_)
                    conf[computer].sequencerDir_ += pathSeparator_;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_fredVideo(int computer, wxXmlNode &node)
{
    if (!elfConfiguration[computer].useFredVideo)
    {
        elfConfiguration[computer].ioConfiguration.fredVideoNumber = conf[computer].numberOfVideoTypes_;
        conf[computer].numberOfVideoTypes_++;
    }

    elfConfiguration[computer].useFredVideo = true;

    wxString tagList[]=
    {
        "out",
        "iogroup",
        "zoom",
        "border",
        "pos",
        "xscale",
        "color",
        "height",
        "width",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IOGROUP,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_XSCALE,
        TAG_COLOR,
        TAG_HEIGHT,
        TAG_WIDTH,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, scale, position;

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.fredVideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.fredVideoNumber] = "FRED";
    elfConfiguration[computer].ioConfiguration.videoHeight = 128;
    elfConfiguration[computer].ioConfiguration.videoWidth = 192;
    conf[computer].xScale_ = "1";

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.fredVideoOutput = (int)parseXml_Number(*child);
            break;
                
            case TAG_ZOOM:
                conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.fredVideoNumber] = child->GetNodeContent();
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOFRED] = (int)width;
                    screenInfo[computer].borderY[VIDEOFRED] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defFredX_;
                if (ypos != -1)
                    ypos += conf[computer].defFredY_;
                conf[computer].defFredX_ = xpos;
                conf[computer].defFredY_ = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo[computer].defaultColour[COL_PIXIE_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo[computer].defaultColour[COL_PIXIE_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_XSCALE:
                conf[computer].xScale_ = convertLocale(child->GetNodeContent());
            break;

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.fredVideoIoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_HEIGHT:
                elfConfiguration[computer].ioConfiguration.videoHeight = (int)parseXml_Number(*child);
            break;

            case TAG_WIDTH:
                elfConfiguration[computer].ioConfiguration.videoWidth = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_pixieGraphics(int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "interrupt",
        "start",
        "end",
        "screenend",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_INTERRUPT,
        TAG_START,
        TAG_END,
        TAG_SCREENEND,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_INTERRUPT:
                elfConfiguration[computer].ioConfiguration.pixieGraphics.interrupt = (int)parseXml_Number(*child);
            break;

            case TAG_START:
                elfConfiguration[computer].ioConfiguration.pixieGraphics.start = (int)parseXml_Number(*child);
            break;
                
            case TAG_END:
                elfConfiguration[computer].ioConfiguration.pixieGraphics.end = (int)parseXml_Number(*child);
            break;
 
            case TAG_SCREENEND:
                elfConfiguration[computer].ioConfiguration.pixieGraphics.screenend = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_MC6845Video(int computer, wxXmlNode &node)
{
    long start, end;
    long width, height;

    if (!elfConfiguration[computer].use6845)
    {
        elfConfiguration[computer].ioConfiguration.mc6845VideoNumber = conf[computer].numberOfVideoTypes_;
        conf[computer].numberOfVideoTypes_++;
    }

    elfConfiguration[computer].use6845 = true;

    wxString tagList[]=
    {
        "ram",
        "font",
        "dirname",
        "out",
        "ef",
        "interlace",
        "iogroup",
        "zoom",
        "screen",
        "char",
        "border",
        "pos",
        "color",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_RAM,
        TAG_FONT,
        TAG_DIRNAME,
        TAG_OUT,
        TAG_EF,
        TAG_INTERLACE,
        TAG_IOGROUP,
        TAG_ZOOM,
        TAG_SCREEN,
        TAG_CHAR,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    elfConfiguration[computer].charLine = 64;
    elfConfiguration[computer].ioConfiguration.mc6845Address = -1;
    elfConfiguration[computer].ioConfiguration.mc6845AddressMask = 0xFFFF;
    elfConfiguration[computer].ioConfiguration.mc6845Data = -1;
    elfConfiguration[computer].ioConfiguration.mc6845DataMask = 0xFFFF;
    elfConfiguration[computer].ioConfiguration.mc6845Ef = -1;
    elfConfiguration[computer].ioConfiguration.mc6845IoGroup = -1;
    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.mc6845VideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.mc6845VideoNumber] = "MC6845";
    elfConfiguration[computer].ioConfiguration.mc6845CharSize.x = 8;
    elfConfiguration[computer].ioConfiguration.mc6845CharSize.y = 8;
    elfConfiguration[computer].ioConfiguration.mc6845ScreenSize.x = 64;
    elfConfiguration[computer].ioConfiguration.mc6845ScreenSize.y = 16;

    int tagTypeInt;
    Word mask;
    int red, green, blue, xpos, ypos;
    wxString color, position;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_RAM:
                if (!parseXml_Range(*child, &start, &end))
                {
                    warningText_ += "No mc6845 RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    elfConfiguration[computer].ioConfiguration.mc6845StartRam = (int)start;
                    elfConfiguration[computer].ioConfiguration.mc6845EndRam = (int)end;
                    mask = (Word)end - start;
                    
                    elfConfiguration[computer].ioConfiguration.mc6845RamMask = 0x3FFF;
                    while ((mask & 0x2000) == 0)
                    {
                        elfConfiguration[computer].ioConfiguration.mc6845RamMask = elfConfiguration[computer].ioConfiguration.mc6845RamMask >> 1;
                        mask = mask << 1;
                    }
                }
             break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "register")
                {
                    elfConfiguration[computer].ioConfiguration.mc6845Address = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.mc6845AddressMask = (int)parseXml_Number(*child, "mask");
                    if (elfConfiguration[computer].ioConfiguration.mc6845AddressMask == 0)
                        elfConfiguration[computer].ioConfiguration.mc6845AddressMask = 0xFFFF;
                }
                if (child->GetAttribute("type") == "data")
                {
                    elfConfiguration[computer].ioConfiguration.mc6845Data = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.mc6845DataMask = (int)parseXml_Number(*child, "mask");
                    if (elfConfiguration[computer].ioConfiguration.mc6845DataMask == 0)
                        elfConfiguration[computer].ioConfiguration.mc6845DataMask = 0xFFFF;
                }
           break;
                
            case TAG_FONT:
                conf[computer].charRom_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                charRomDirDefined_ = true;
                conf[computer].charRomDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].charRomDir_.Right(1) != pathSeparator_)
                    conf[computer].charRomDir_ += pathSeparator_;
            break;

            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.mc6845Ef = (int)parseXml_Number(*child);
            break;
                
            case TAG_INTERLACE:
                conf[computer].interlace_ = true;
            break;
                
            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.mc6845IoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_ZOOM:
                conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.mc6845VideoNumber] = child->GetNodeContent();
            break;

            case TAG_CHAR:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect char size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    elfConfiguration[computer].ioConfiguration.mc6845CharSize.x = (int)width;
                    elfConfiguration[computer].ioConfiguration.mc6845CharSize.y = (int)height;
                }
            break;

            case TAG_SCREEN:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect screen size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    elfConfiguration[computer].ioConfiguration.mc6845ScreenSize.x = (int)width;
                    elfConfiguration[computer].ioConfiguration.mc6845ScreenSize.y = (int)height;
                }
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOXML6845] = (int)width;
                    screenInfo[computer].borderY[VIDEOXML6845] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defMc6845X_;
                if (ypos != -1)
                    ypos += conf[computer].defMc6845Y_;
                conf[computer].defMc6845X_ = xpos;
                conf[computer].defMc6845Y_ = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo[computer].defaultColour[COL_MC6845_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo[computer].defaultColour[COL_MC6845_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_MC6847Video(int computer, wxXmlNode &node)
{
    long start, end;
    long width, height;

    if (!elfConfiguration[computer].use6847)
    {
        elfConfiguration[computer].ioConfiguration.mc6847VideoNumber = conf[computer].numberOfVideoTypes_;
        conf[computer].numberOfVideoTypes_++;
    }
    
    elfConfiguration[computer].use6847 = true;

    wxString tagList[]=
    {
        "ram",
        "out",
        "font",
        "dirname",
        "inv",
        "ext",
        "css",
        "as",
        "ag",
        "gm0",
        "gm1",
        "gm2",
        "zoom",
        "border",
        "pos",
        "color",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_RAM,
        TAG_OUT,
        TAG_FONT,
        TAG_DIRNAME,
        TAG_INV,
        TAG_EXT,
        TAG_CSS,
        TAG_AS,
        TAG_AG,
        TAG_GM0,
        TAG_GM1,
        TAG_GM2,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    int red, green, blue, xpos, ypos;
    wxString color, position;

    elfConfiguration[computer].ioConfiguration.mc6847b7 = 0;
    elfConfiguration[computer].ioConfiguration.mc6847b6 = 0;
    elfConfiguration[computer].ioConfiguration.mc6847b5 = 0;
    elfConfiguration[computer].ioConfiguration.mc6847b4 = 0;
    elfConfiguration[computer].ioConfiguration.mc6847b3 = 0;
    elfConfiguration[computer].ioConfiguration.mc6847b2 = 0;
    elfConfiguration[computer].ioConfiguration.mc6847b1 = 0;
    elfConfiguration[computer].ioConfiguration.mc6847b0 = 0;
    elfConfiguration[computer].ioConfiguration.mc6847dd7 = 0;
    elfConfiguration[computer].ioConfiguration.mc6847dd6 = 0;

    elfConfiguration[computer].ioConfiguration.forceHighAg = false;
    elfConfiguration[computer].ioConfiguration.forceHighAs = false;
    elfConfiguration[computer].ioConfiguration.forceHighExt = false;
    elfConfiguration[computer].ioConfiguration.forceHighGm2 = false;
    elfConfiguration[computer].ioConfiguration.forceHighGm1 = false;
    elfConfiguration[computer].ioConfiguration.forceHighGm0 = false;
    elfConfiguration[computer].ioConfiguration.forceHighCss = false;
    elfConfiguration[computer].ioConfiguration.forceHighInv = false;

    elfConfiguration[computer].charLine = 32;
    elfConfiguration[computer].screenHeight6847 = 192;
    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.mc6847VideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.mc6847VideoNumber] = "MC6847";

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_RAM:
                if (!parseXml_Range(*child, &start, &end))
                {
                    warningText_ += "No mc6847 RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    elfConfiguration[computer].ioConfiguration.mc6847StartRam = (int)start;
                    elfConfiguration[computer].ioConfiguration.mc6847EndRam = (int)end;
                }
            break;
                
            case TAG_OUT:
                if (!parseXml_Range(*child, &start, &end))
                {
                    elfConfiguration[computer].ioConfiguration.mc6847OutputMode = 0;
                    elfConfiguration[computer].ioConfiguration.mc6847Output = start & 07;
                }
                else
                {
                    elfConfiguration[computer].ioConfiguration.mc6847OutputMode = 1;
                    elfConfiguration[computer].ioConfiguration.mc6847OutputStart = (Word) start;
                    elfConfiguration[computer].ioConfiguration.mc6847OutputEnd = (Word) end;
                }
            break;
 
            case TAG_FONT:
                conf[computer].charRom_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                charRomDirDefined_ = true;
                conf[computer].charRomDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].charRomDir_.Right(1) != pathSeparator_)
                    conf[computer].charRomDir_ += pathSeparator_;
            break;

            case TAG_INV:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].ioConfiguration.forceHighInv = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_EXT:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].ioConfiguration.forceHighExt = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_CSS:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].ioConfiguration.forceHighCss = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_AS:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].ioConfiguration.forceHighAs = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_AG:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].ioConfiguration.forceHighAg = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_GM0:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].ioConfiguration.forceHighGm0 = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_GM1:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].ioConfiguration.forceHighGm1 = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_GM2:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].ioConfiguration.forceHighGm2 = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_ZOOM:
                conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.mc6847VideoNumber] = child->GetNodeContent();
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOXML6847] = (int)width;
                    screenInfo[computer].borderY[VIDEOXML6847] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defMc6847X_;
                if (ypos != -1)
                    ypos += conf[computer].defMc6847Y_;
                conf[computer].defMc6847X_ = xpos;
                conf[computer].defMc6847Y_ = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "text_back")
                    screenInfo[computer].defaultColour[COL_MC6847_TEXT_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "text_green")
                    screenInfo[computer].defaultColour[COL_MC6847_TEXT_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "text_orange")
                    screenInfo[computer].defaultColour[COL_MC6847_TEXT_ORANGE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_green")
                    screenInfo[computer].defaultColour[COL_MC6847_GRAPH_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_yellow")
                    screenInfo[computer].defaultColour[COL_MC6847_GRAPH_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_blue")
                    screenInfo[computer].defaultColour[COL_MC6847_GRAPH_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_red")
                    screenInfo[computer].defaultColour[COL_MC6847_GRAPH_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_buff")
                    screenInfo[computer].defaultColour[COL_MC6847_GRAPH_BUFF].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_cyan")
                    screenInfo[computer].defaultColour[COL_MC6847_GRAPH_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_magenta")
                    screenInfo[computer].defaultColour[COL_MC6847_GRAPH_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_orange")
                    screenInfo[computer].defaultColour[COL_MC6847_GRAPH_ORANGE].Printf("#%02X%02X%02X", red, green, blue);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_TMS9918Video(int computer, wxXmlNode &node)
{
    if (!elfConfiguration[computer].useTMS9918)
    {
        elfConfiguration[computer].ioConfiguration.tmsVideoNumber = conf[computer].numberOfVideoTypes_;
        conf[computer].numberOfVideoTypes_++;
    }

    elfConfiguration[computer].useTMS9918 = true;

    wxString tagList[]=
    {
        "out",
        "ef",
        "zoom",
        "border",
        "pos",
        "color",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_EF,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, position;

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.tmsVideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.tmsVideoNumber] = "TMS 9918";
    elfConfiguration[computer].ioConfiguration.tmsInterrupt = 0;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                if (child->GetAttribute("type") == "register")
                    elfConfiguration[computer].ioConfiguration.tmsModeHighOutput = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "data")
                    elfConfiguration[computer].ioConfiguration.tmsModeLowOutput = (int)parseXml_Number(*child);
           break;
                
            case TAG_EF:
                    elfConfiguration[computer].ioConfiguration.tmsInterrupt = (int)parseXml_Number(*child);
            break;
                
            case TAG_ZOOM:
                conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.tmsVideoNumber] = child->GetNodeContent();
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOXMLTMS] = (int)width;
                    screenInfo[computer].borderY[VIDEOXMLTMS] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defTmsX_;
                if (ypos != -1)
                    ypos += conf[computer].defTmsY_;
                conf[computer].defTmsX_ = xpos;
                conf[computer].defTmsY_ = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "transparant")
                    screenInfo[computer].defaultColour[COL_TMS_TRANSPARANT].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "black")
                    screenInfo[computer].defaultColour[COL_TMS_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "medium_green")
                    screenInfo[computer].defaultColour[COL_TMS_MEDIUM_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "light_green")
                    screenInfo[computer].defaultColour[COL_TMS_LIGHT_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "dark_blue")
                    screenInfo[computer].defaultColour[COL_TMS_DARK_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "light_blue")
                    screenInfo[computer].defaultColour[COL_TMS_LIGHT_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "dark_red")
                    screenInfo[computer].defaultColour[COL_TMS_DARK_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "aqua_blue")
                    screenInfo[computer].defaultColour[COL_TMS_AQUA_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "medium_red")
                    screenInfo[computer].defaultColour[COL_TMS_MEDIUM_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "light_red")
                    screenInfo[computer].defaultColour[COL_TMS_LIGHT_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "dark_yellow")
                    screenInfo[computer].defaultColour[COL_TMS_DARK_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "light_yellow")
                    screenInfo[computer].defaultColour[COL_TMS_LIGHT_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "dark_green")
                    screenInfo[computer].defaultColour[COL_TMS_DARK_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "purple")
                    screenInfo[computer].defaultColour[COL_TMS_PURPLE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "gray")
                    screenInfo[computer].defaultColour[COL_TMS_GRAY].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "white")
                    screenInfo[computer].defaultColour[COL_TMS_WHITE].Printf("#%02X%02X%02X", red, green, blue);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Intel8275Video(int computer, wxXmlNode &node)
{
    if (!elfConfiguration[computer].use8275)
    {
        elfConfiguration[computer].ioConfiguration.i8275VideoNumber = conf[computer].numberOfVideoTypes_;
        conf[computer].numberOfVideoTypes_++;
    }

    elfConfiguration[computer].use8275 = true;

    wxString tagList[]=
    {
        "font",
        "dirname",
        "in",
        "out",
        "ef",
        "interlace",
        "zoom",
        "border",
        "pos",
        "color",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FONT,
        TAG_DIRNAME,
        TAG_IN,
        TAG_OUT,
        TAG_EF,
        TAG_INTERLACE,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.i8275VideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.i8275VideoNumber] = "Intel 8275";

    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, position;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                if (child->GetAttribute("type") == "status")
                    elfConfiguration[computer].ioConfiguration.i8275ReadStatus = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "parameter")
                    elfConfiguration[computer].ioConfiguration.i8275ReadParameter = (int)parseXml_Number(*child);
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "command")
                    elfConfiguration[computer].ioConfiguration.i8275WriteCommand  = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "parameter")
                    elfConfiguration[computer].ioConfiguration.i8275WriteParameter  = (int)parseXml_Number(*child);
            break;
                
            case TAG_FONT:
                conf[computer].charRom_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                charRomDirDefined_ = true;
                conf[computer].charRomDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].charRomDir_.Right(1) != pathSeparator_)
                    conf[computer].charRomDir_ += pathSeparator_;
            break;

            case TAG_EF:
                    elfConfiguration[computer].ioConfiguration.i8275VerticalRetrace = (int)parseXml_Number(*child);
            break;
                
            case TAG_INTERLACE:
                conf[computer].interlace_ = true;
            break;
                
            case TAG_ZOOM:
                conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.i8275VideoNumber] = child->GetNodeContent();
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOXMLI8275] = (int)width;
                    screenInfo[computer].borderY[VIDEOXMLI8275] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defi8275X_;
                if (ypos != -1)
                    ypos += conf[computer].defi8275Y_;
                conf[computer].defi8275X_ = xpos;
                conf[computer].defi8275Y_ = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo[computer].defaultColour[COL_I8275_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo[computer].defaultColour[COL_I8275_BACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "high")
                    screenInfo[computer].defaultColour[COL_I8275_HIGH].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_VisVideo(int computer, wxXmlNode &node)
{    
    Byte ctone[8] = { 0, 28, 77, 105, 150, 194, 227, 255 };
    Byte red[8]   = {   0,   0,   0,   0, 255, 255, 255, 255 };
    Byte green[8] = {   0, 255,   0, 255,   0, 255,   0, 255 };
    Byte blue[8]  = {   0,   0, 255, 255,   0,   0, 255, 255 };

    if (!elfConfiguration[computer].usev1870)
    {
        elfConfiguration[computer].ioConfiguration.v1870VideoNumber = conf[computer].numberOfVideoTypes_;
        conf[computer].numberOfVideoTypes_++;
    }

    elfConfiguration[computer].usev1870 = true;

    wxString tagList[]=
    {
        "pagesize",
        "charsize",
        "charrom",
        "filename",
        "dirname",
        "pcbmask",
        "maxcharlines",
        "maxscreenlines",
        "mode",
        "int",
        "ef",
        "clock",
        "cmamaskfixed",
        "rotate",
        "colorram",
        "out",
        "zoom",
        "border",
        "pos",
        "color",
        "ctone",
        "cursorblink",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_PAGE,
        TAG_CHAR,
        TAG_CHARROM,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_PCB,
        TAG_CHARLINES,
        TAG_SCREENLINES,
        TAG_MODE,
        TAG_INT,
        TAG_EF,
        TAG_CLOCK,
        TAG_CMAMASK,
        TAG_ROTATE,
        TAG_COLORRAM,
        TAG_OUT,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_CTONE,
        TAG_CURSORBLINK,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    int tagTypeInt;
    long width, height;
    int number, xpos, ypos;
    wxString color, position;

    elfConfiguration[computer].ioConfiguration.v1870pageMemSize = 0x3ff;
    elfConfiguration[computer].ioConfiguration.v1870pageMemRom =false;
    elfConfiguration[computer].ioConfiguration.v1870charMemSize =0x7ff;
    elfConfiguration[computer].ioConfiguration.v1870charMemRom = false;
    elfConfiguration[computer].ioConfiguration.v1870charRomFormatTmc = false;
    elfConfiguration[computer].ioConfiguration.v1870pcbMask = 0x7f;
    elfConfiguration[computer].ioConfiguration.v1870maxCharLines = 16;
    elfConfiguration[computer].ioConfiguration.v1870maxScreenLines = 24;
    elfConfiguration[computer].ioConfiguration.v1870interruptMode = INT_NONE;
    elfConfiguration[computer].ioConfiguration.v1870ef = 1;
    elfConfiguration[computer].ioConfiguration.v1870clock = 5.626;
    elfConfiguration[computer].ioConfiguration.v1870charRomStart = 0;
    elfConfiguration[computer].ioConfiguration.v1870cmaMaskFixed = false;
    elfConfiguration[computer].ioConfiguration.v1870videoModeEf = 2;
    elfConfiguration[computer].ioConfiguration.v1870rotate = false;
    elfConfiguration[computer].ioConfiguration.v1870colorRamType = CR_NONE;
    elfConfiguration[computer].ioConfiguration.v1870efRev = false;
    elfConfiguration[computer].ioConfiguration.v1870outWrite = -1;
    elfConfiguration[computer].ioConfiguration.v1870outSelect = -1;
    elfConfiguration[computer].ioConfiguration.v1870ioGroup = -1;
    elfConfiguration[computer].ioConfiguration.v1870cursorBlink = false;
    elfConfiguration[computer].ioConfiguration.v1870outIntEnable = init_IoPort();

    conf[computer].videoMode_ = PAL;
    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.v1870VideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.v1870VideoNumber] = "VIS 1870";

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_PAGE:
                if (child->GetAttribute("type") == "rom")
                    elfConfiguration[computer].ioConfiguration.v1870pageMemRom = true;
                elfConfiguration[computer].ioConfiguration.v1870pageMemSize =  (int)parseXml_Number(*child);
            break;
                                
            case TAG_CHAR:
                if (child->GetAttribute("type") == "rom")
                    elfConfiguration[computer].ioConfiguration.v1870charMemRom = true;
                elfConfiguration[computer].ioConfiguration.v1870charMemSize =  (int)parseXml_Number(*child);
            break;

            case TAG_CHARROM:
                elfConfiguration[computer].ioConfiguration.v1870charRomStart = (int)parseXml_Number(*child);
            break;

            case TAG_FILENAME:
                if (child->GetAttribute("format") == "tmc")
                    elfConfiguration[computer].ioConfiguration.v1870charRomFormatTmc = true;
                conf[computer].charRom_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                charRomDirDefined_ = true;
                conf[computer].charRomDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].charRomDir_.Right(1) != pathSeparator_)
                    conf[computer].charRomDir_ += pathSeparator_;
            break;

            case TAG_PCB:
                elfConfiguration[computer].ioConfiguration.v1870pcbMask =  (int)parseXml_Number(*child);
            break;

            case TAG_CHARLINES:
                elfConfiguration[computer].ioConfiguration.v1870maxCharLines =  (int)parseXml_Number(*child);
            break;

            case TAG_SCREENLINES:
                elfConfiguration[computer].ioConfiguration.v1870maxScreenLines =  (int)parseXml_Number(*child);
            break;

            case TAG_MODE:
                if (child->GetNodeContent() == "ntsc")
                    conf[computer].videoMode_ = NTSC;
            break;

            case TAG_INT:
                elfConfiguration[computer].ioConfiguration.v1870interruptMode =  (int)parseXml_Number(*child) - 1;
            break;

            case TAG_EF:
                if (child->GetAttribute("type") == "display")
                {
                    elfConfiguration[computer].ioConfiguration.v1870ef = (int)parseXml_Number(*child);
                    if (child->GetAttribute("pol") == "rev")
                        elfConfiguration[computer].ioConfiguration.v1870efRev = true;
                }
                if (child->GetAttribute("type") == "mode")
                {
                    elfConfiguration[computer].ioConfiguration.v1870videoModeEf = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.v1870useVideoModeEf = true;
                }
                if (child->GetAttribute("write") == "enable")
                {
                    elfConfiguration[computer].ioConfiguration.v1870useBlockWrite = false;
                }
            break;
                
            case TAG_CLOCK:
                elfConfiguration[computer].ioConfiguration.v1870clock = getDouble(child->GetNodeContent(), childName, 500, "500");
            break;

            case TAG_CMAMASK:
                elfConfiguration[computer].ioConfiguration.v1870cmaMaskFixed = true;
            break;
                
            case TAG_ROTATE:
                elfConfiguration[computer].ioConfiguration.v1870rotate = true;
            break;

            case TAG_COLORRAM:
                if (child->GetAttribute("type") == "cidelsa")
                    elfConfiguration[computer].ioConfiguration.v1870colorRamType = CR_CIDELSA;
                if (child->GetAttribute("type") == "tmc")
                    elfConfiguration[computer].ioConfiguration.v1870colorRamType = CR_TMC600;
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "write")
                    elfConfiguration[computer].ioConfiguration.v1870outWrite = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "select")
                    elfConfiguration[computer].ioConfiguration.v1870outSelect = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "int")
                    elfConfiguration[computer].ioConfiguration.v1870outIntEnable = parseXml_IoPort(*child);
            break;

            case TAG_ZOOM:
                conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.v1870VideoNumber] = child->GetNodeContent();
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOXML1870] = (int)width;
                    screenInfo[computer].borderY[VIDEOXML1870] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defv1870X_;
                if (ypos != -1)
                    ypos += conf[computer].defv1870Y_;
                conf[computer].defv1870X_ = xpos;
                conf[computer].defv1870Y_ = ypos;
            break;

            case TAG_COLOR:
                number = (int)parseXml_Number(*child, "num");
                color = child->GetNodeContent();
                red[number] = getNextHexDec(&color) & 0xff;
                green[number] = getNextHexDec(&color) & 0xff;
                blue[number] = getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "mask")
                {
                    if (number == 0)
                    {
                        for (int i = 0; i < 8; i++)
                        {
                            screenInfo[computer].defaultColour[i*8 + number].Printf("#%02X%02X%02X", (red[number] ^ 0xff) & (ctone[i] ^ 0xff), (green[number] ^ 0xff) & (ctone[i] ^ 0xff), (blue[number] ^ 0xff) & (ctone[i] ^ 0xff));
                        }
                    }
                    else
                    {
                        for (int i = 0; i < 8; i++)
                        {
                            screenInfo[computer].defaultColour[i*8 + number].Printf("#%02X%02X%02X", red[number] & ctone[i], green[number] & ctone[i], blue[number] & ctone[i]);
                        }
                    }
                }
                else
                    screenInfo[computer].defaultColour[number+56].Printf("#%02X%02X%02X", red[number], green[number], blue[number]);
            break;

            case TAG_CTONE:
                number = (int)parseXml_Number(*child, "num");
                ctone[number] = (int)parseXml_Number(*child);
                
                screenInfo[computer].defaultColour[number*8].Printf("#%02X%02X%02X", (red[0] ^ 0xff) & (ctone[number] ^0xff), (green[0] ^ 0xff) & (ctone[number] ^0xff), (blue[0] ^ 0xff) & (ctone[number] ^0xff));
                for (int i = 1; i < 8; i++)
                    screenInfo[computer].defaultColour[number*8 + i].Printf("#%02X%02X%02X", red[i] & ctone[number], green[i] & ctone[number], blue[i] & ctone[number]);
            break;

            case TAG_CURSORBLINK:
                elfConfiguration[computer].ioConfiguration.v1870cursorBlink = true;
            break;

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.v1870ioGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_SN76430NVideo(int computer, wxXmlNode &node)
{
    if (!elfConfiguration[computer].useSN76430N)
    {
        elfConfiguration[computer].ioConfiguration.SN76430NVideoNumber = conf[computer].numberOfVideoTypes_;
        conf[computer].numberOfVideoTypes_++;
    }

    elfConfiguration[computer].useSN76430N = true;

    wxString tagList[]=
    {
        "zoom",
        "border",
        "pos",
        "color",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.SN76430NVideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.SN76430NVideoNumber] = "SN76430N";

    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, position;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_ZOOM:
                conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.SN76430NVideoNumber] = child->GetNodeContent();
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOXMLSN76430N] = (int)width;
                    screenInfo[computer].borderY[VIDEOXMLSN76430N] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defSN76430NX_;
                if (ypos != -1)
                    ypos += conf[computer].defSN76430NY_;
                conf[computer].defSN76430NX_ = xpos;
                conf[computer].defSN76430NY_ = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "white")
                    screenInfo[computer].defaultColour[COL_SN76430N_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "yellow")
                    screenInfo[computer].defaultColour[COL_SN76430N_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "green")
                    screenInfo[computer].defaultColour[COL_SN76430N_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "red")
                    screenInfo[computer].defaultColour[COL_SN76430N_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "black")
                    screenInfo[computer].defaultColour[COL_SN76430N_BLACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Ps2Keyboard (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].defined = false;
    elfConfiguration[computer].useMatrixKeyboard = false;
    elfConfiguration[computer].useKeyb1871 = false;
    elfConfiguration[computer].useKeyboard = false;
    elfConfiguration[computer].usePS2 = true;
    elfConfiguration[computer].usePs2gpio = false;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "caps",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_CAPS,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.ps2KeyboardInput = (int)parseXml_Number(*child);
            break;
                
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.ps2KeyboardOutput = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.ps2KeyboardEf = (int)parseXml_Number(*child);
            break;
 
            case TAG_CAPS:
                elfConfiguration[computer].forceUpperCase = true;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Gpio (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].defined = false;
    elfConfiguration[computer].useMatrixKeyboard = false;
    elfConfiguration[computer].useKeyb1871 = false;
    elfConfiguration[computer].useKeyboard = false;
    elfConfiguration[computer].usePS2 = false;
    elfConfiguration[computer].usePs2gpio = true;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "caps",
        "jp4",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_CAPS,
        TAG_JP4,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    elfConfiguration[computer].ioConfiguration.gpioInput = -1;
    elfConfiguration[computer].ioConfiguration.gpioOutput = -1;
    elfConfiguration[computer].ioConfiguration.gpioEf = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.gpioInput = (int)parseXml_Number(*child);
            break;
                                
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.gpioOutput = (int)parseXml_Number(*child);
            break;

            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.gpioEf = (int)parseXml_Number(*child);
            break;
 
            case TAG_JP4:
                elfConfiguration[computer].gpioJp4 = true;
            break;

            case TAG_CAPS:
                elfConfiguration[computer].forceUpperCase = true;
            break;

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.gpioIoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_AsciiKeyboard (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].defined = false;
    elfConfiguration[computer].useMatrixKeyboard = false;
    elfConfiguration[computer].useKeyb1871 = false;
    elfConfiguration[computer].useKeyboard = true;
    elfConfiguration[computer].usePS2 = false;
    elfConfiguration[computer].ps2Interrupt = false;
    elfConfiguration[computer].usePs2gpio = false;

    wxString tagList[]=
    {
        "in",
        "ef",
        "caps",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_EF,
        TAG_CAPS,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.keyboardInput = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.keyboardEf = (int)parseXml_Number(*child);
            break;
                
            case TAG_CAPS:
                elfConfiguration[computer].forceUpperCase = true;
            break;

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.keyboardIoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_1871Keyboard(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].defined = false;
    elfConfiguration[computer].useMatrixKeyboard = false;
    elfConfiguration[computer].useKeyb1871 = true;
    elfConfiguration[computer].useKeyboard = false;
    elfConfiguration[computer].usePS2 = false;
    elfConfiguration[computer].ps2Interrupt = false;
    elfConfiguration[computer].usePs2gpio = false;

    wxString tagList[]=
    {
        "in",
        "ef",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_EF,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.keyboardInput = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "rpt")
                    elfConfiguration[computer].ioConfiguration.keyboardRepeatEf = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "data")
                    elfConfiguration[computer].ioConfiguration.keyboardEf = (int)parseXml_Number(*child);
            break;
                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_LatchKeyboard(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].defined = true;
    elfConfiguration[computer].useMatrixKeyboard = false;
    elfConfiguration[computer].useKeyb1871 = false;
    elfConfiguration[computer].useKeyboard = false;
    elfConfiguration[computer].usePS2 = false;
    elfConfiguration[computer].ps2Interrupt = false;
    elfConfiguration[computer].usePs2gpio = false;

    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].IoGroup = -1;

    wxString tagList[]=
    {
        "out",
        "ef",
        "keydef",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_EF,
        TAG_KEYDEF,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED,
    };

    int tagTypeInt, keyValue;
    wxString keyText;

    for (int i=0; i<512; i++)
    {
        elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].pc[i] = -1;
        elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].pcShift[i] = -1;
        elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].pcForceShift[i] = -1;
        elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].pcForceNoShift[i] = -1;
    }
    
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].altRight = -1;
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].altLeft = -1;
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].ctrlRight = -1;
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].ctrlLeft = -1;
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].shift = -1;
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].switchAltCtrl = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].outPort = (int)parseXml_Number(*child);
                elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].mask = (int)parseXml_Number(*child, "mask");
            break;
                
            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].ef = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].pressed = 0;
                else
                    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].pressed = 1;
            break;
                
            case TAG_KEYDEF:
                if (child->GetAttribute("type") == "")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                    {
                        if (child->HasAttribute("out"))
                            elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].pc[keyValue] = (int)parseXml_Number(*child, "out");
                    }
                    if (child->HasAttribute("map"))
                    {
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].pcMap[(int)parseXml_Number(*child, "map")] = (int)parseXml_Number(*child)&0x1ff;
                    }
                }
                
                if (child->GetAttribute("type") == "text")
                {
                    keyText = child->GetNodeContent();
                    if (keyText == "alt_right")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].altRight = (int)parseXml_Number(*child, "out");
                    if (keyText == "alt_left")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].altLeft = (int)parseXml_Number(*child, "out");
                    if (keyText == "ctrl_right")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].ctrlRight = (int)parseXml_Number(*child, "out");
                    if (keyText == "ctrl_left")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].ctrlLeft = (int)parseXml_Number(*child, "out");
                    if (keyText == "shift")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].shift = (int)parseXml_Number(*child, "out");
                    if (keyText == "switch")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].switchAltCtrl = (int)parseXml_Number(*child, "out");
                    if (keyText == "caps")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].caps = (int)parseXml_Number(*child, "out");
                }

                if (child->GetAttribute("type") == "force")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                        if (child->HasAttribute("out"))
                            elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].pcForceShift[keyValue] = (int)parseXml_Number(*child, "out");
                }

                if (child->GetAttribute("type") == "force_noshift")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                        if (child->HasAttribute("out"))
                            elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].pcForceNoShift[keyValue] = (int)parseXml_Number(*child, "out");
                }

                if (child->GetAttribute("type") == "shift")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                        if (child->HasAttribute("out"))
                            elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].pcShift[keyValue] = (int)parseXml_Number(*child, "out");
                    if (child->HasAttribute("map"))
                    {
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].pcMapShift[(int)parseXml_Number(*child, "map")] = (int)parseXml_Number(*child)&0x1ff;
                    }
                }
            break;

/*            case TAG_KEYDEFSHIFT:
                keyValue = (int)parseXml_Number(*child)&0x1ff;
                if (keyValue != 0)
                    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].pcShift[keyValue] = (int)parseXml_Number(*child, "out");
            break;*/

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].IoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_MatrixKeyboard(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[0].defined = false;
    elfConfiguration[computer].useMatrixKeyboard = true;
    elfConfiguration[computer].useKeyb1871 = false;
    elfConfiguration[computer].useKeyboard = false;
    elfConfiguration[computer].usePS2 = false;
    elfConfiguration[computer].ps2Interrupt = false;
    elfConfiguration[computer].usePs2gpio = false;

    elfConfiguration[computer].ioConfiguration.keybMatrixIoGroup = -1;
    elfConfiguration[computer].ioConfiguration.keybMatrixPressed = 0xff;
    elfConfiguration[computer].ioConfiguration.keybMatrixInMask = 0xff;
    
    wxString tagList[]=
    {
        "in",
        "out",
        "keydef",
        "keydeftext",
        "ef1",
        "ef2",
        "ef3",
        "ef4",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_OUT,
        TAG_KEYDEF,
        TAG_KEYDEFTEXT,
        TAG_EF1,
        TAG_EF2,
        TAG_EF3,
        TAG_EF4,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    wxString efKeyList[]=
    {
        "ctrl",
        "shift",
        "caps",
        "esc",
        "undefined"
    };

    int tagTypeInt, keyValue, textEfKeyInt;
    wxString keyText, efText;
    
    for (int i=0; i<255; i++)
    {
        elfConfiguration[computer].ioConfiguration.keybMatrixKeyValue[i] = -1;
        elfConfiguration[computer].ioConfiguration.keybMatrixBitValue[i] = 0;
        elfConfiguration[computer].ioConfiguration.keybMatrixShiftValue[i] = 1;
        elfConfiguration[computer].ioConfiguration.keybMatrixCtrlValue[i] = 1;
    }
    for (int i=0; i<LAST_MATRIX_EF_KEY; i++)
    {
        elfConfiguration[computer].ioConfiguration.keybMatrixEfKey[i] = 0;
        elfConfiguration[computer].ioConfiguration.keybMatrixEfKeyRev[i] = false;
    }

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.keybMatrixIn = (int)parseXml_Number(*child);
                if (elfConfiguration[computer].ioConfiguration.keybMatrixIn > 7)
                    elfConfiguration[computer].ioConfiguration.keybMatrixAddressMode = true;
                if (child->HasAttribute("mask"))
                    elfConfiguration[computer].ioConfiguration.keybMatrixInMask = (int)parseXml_Number(*child, "mask");
                
                if (child->GetAttribute("pol") == "rev")
                    elfConfiguration[computer].ioConfiguration.keybMatrixPressed = 0;
            break;
            
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.keybMatrixOut = (int)parseXml_Number(*child);
                if (elfConfiguration[computer].ioConfiguration.keybMatrixOut > 7)
                    elfConfiguration[computer].ioConfiguration.keybMatrixAddressMode = true;
            break;

            case TAG_KEYDEF:
                if (child->GetAttribute("type") == "")
                {
                    keyValue = (int)parseXml_Number(*child)&0xff;
                    elfConfiguration[computer].ioConfiguration.keybMatrixKeyValue[keyValue] = (int)parseXml_Number(*child, "address");
                    elfConfiguration[computer].ioConfiguration.keybMatrixBitValue[keyValue] = 1 << (int)parseXml_Number(*child, "bit");
                }
                if (child->GetAttribute("type") == "shift")
                {
                    keyValue = (int)parseXml_Number(*child)&0xff;
                    elfConfiguration[computer].ioConfiguration.keybMatrixKeyValue[keyValue] = (int)parseXml_Number(*child, "address");
                    elfConfiguration[computer].ioConfiguration.keybMatrixBitValue[keyValue] = 1 << (int)parseXml_Number(*child, "bit");
                    elfConfiguration[computer].ioConfiguration.keybMatrixShiftValue[keyValue] = 0;
                }
                if (child->GetAttribute("type") == "ctrl")
                {
                    keyValue = (int)parseXml_Number(*child)&0xff;
                    elfConfiguration[computer].ioConfiguration.keybMatrixKeyValue[keyValue] = (int)parseXml_Number(*child, "address");
                    elfConfiguration[computer].ioConfiguration.keybMatrixBitValue[keyValue] = 1 << (int)parseXml_Number(*child, "bit");
                    elfConfiguration[computer].ioConfiguration.keybMatrixCtrlValue[keyValue] = 0;
                }
                if (child->GetAttribute("type") == "text")
                {
                    keyText = child->GetNodeContent();
                    textEfKeyInt = 0;
                    while (textKeyList[textEfKeyInt] != "undefined")
                    {
                        if (keyText == textKeyList[textEfKeyInt])
                        {
                            elfConfiguration[computer].ioConfiguration.keybMatrixTextKey[textEfKeyInt].keyValue = (int)parseXml_Number(*child, "address");
                            elfConfiguration[computer].ioConfiguration.keybMatrixTextKey[textEfKeyInt].bitMaskPressed = 1 << (int)parseXml_Number(*child, "bit");
                            elfConfiguration[computer].ioConfiguration.keybMatrixTextKey[textEfKeyInt].bitMaskNotPressed = elfConfiguration[computer].ioConfiguration.keybMatrixTextKey[textEfKeyInt].bitMaskPressed ^ 0xff;
                        }
                        textEfKeyInt++;
                    }
                }
            break;
                
            case TAG_EF1:
            case TAG_EF2:
            case TAG_EF3:
            case TAG_EF4:
                keyText = child->GetNodeContent();
                textEfKeyInt = 0;
                while (efKeyList[textEfKeyInt] != "undefined")
                {
                    if (keyText == efKeyList[textEfKeyInt])
                    {
                        elfConfiguration[computer].ioConfiguration.keybMatrixEfKey[textEfKeyInt] = tagTypeInt-TAG_EF1+1;
                        if (child->GetAttribute("pol") == "rev")
                            elfConfiguration[computer].ioConfiguration.keybMatrixEfKeyRev[textEfKeyInt] = true;
                    }
                    textEfKeyInt++;
                }
            break;
                
            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.keybMatrixIoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_ElfKeypad (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].useHexKeyboard = true;

    wxString tagList[]=
    {
        "in",
        "ef",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_EF,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    if (elfConfiguration[computer].panelType_ == PANEL_ELF2K)
    {
        elfConfiguration[computer].useHex = true;
        elfConfiguration[computer].useSwitch = false;
    }
    
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.hexInput.portNumber = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "ready")
                {
                    elfConfiguration[computer].useHexKeyboardEf3 = true;
                    elfConfiguration[computer].ioConfiguration.hexEf = (int)parseXml_Number(*child);
                }
                if (child->GetAttribute("type") == "in")
                {
                    elfConfiguration[computer].useInButton = true;
                    elfConfiguration[computer].ioConfiguration.inEf = (int)parseXml_Number(*child);
                }
                break;
                                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_CosmicosKeypad (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].panelType_ = PANEL_COSMICOS;
    elfConfiguration[computer].useHex = true;
    elfConfiguration[computer].useHexKeyboard = true;

    wxString tagList[]=
    {
        "in",
        "out",
        "ef",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_OUT,
        TAG_EF,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                if (child->GetAttribute("type") == "hex")
                    elfConfiguration[computer].ioConfiguration.hexCosmicosHexInput = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "seg")
                    elfConfiguration[computer].ioConfiguration.hexCosmicosSegInput = (int)parseXml_Number(*child);
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "hex")
                    elfConfiguration[computer].ioConfiguration.hexCosmicosHexOutput = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "seg")
                    elfConfiguration[computer].ioConfiguration.hexCosmicosSegOutput = (int)parseXml_Number(*child);
            break;

            case TAG_EF:
                if (child->GetAttribute("type") == "ret")
                    elfConfiguration[computer].ioConfiguration.hexCosmicosEfRet = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "dec")
                    elfConfiguration[computer].ioConfiguration.hexCosmicosEfDec = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "req")
                    elfConfiguration[computer].ioConfiguration.hexCosmicosEfReq = (int)parseXml_Number(*child);
            break;
                                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_BitKeypad (int computer, int padnumber, wxXmlNode &node)
{
    elfConfiguration[computer].useBitKeypad = true;
    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].defined = true;
    
    wxString tagList[]=
    {
        "in",
        "bit0",
        "bit1",
        "bit2",
        "bit3",
        "bit4",
        "bit5",
        "bit6",
        "bit7",
        "bit",
        "pressed",
        "repeat",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_BIT0,
        TAG_BIT1,
        TAG_BIT2,
        TAG_BIT3,
        TAG_BIT4,
        TAG_BIT5,
        TAG_BIT6,
        TAG_BIT7,
        TAG_BIT,
        TAG_PRESSED,
        TAG_REPEAT,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString bitNumber;
    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitKeyPressed = 0;
    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys = 0;
    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfModKeys = 0;
    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].ioGroup = -1;
    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].repeat = true;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].inp = (int)parseXml_Number(*child);
            break;

            case TAG_BIT0:
            case TAG_BIT1:
            case TAG_BIT2:
            case TAG_BIT3:
            case TAG_BIT4:
            case TAG_BIT5:
            case TAG_BIT6:
            case TAG_BIT7:
                if (child->HasAttribute("ef"))
                {
                    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitModkey.resize(elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfModKeys+1);
                    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitModkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfModKeys].value = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitModkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfModKeys].ef = (int)parseXml_Number(*child, "ef");
                    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitModkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfModKeys].bitMaskPressed = 1 << (tagTypeInt - TAG_BIT0);
                    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitModkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfModKeys].bitMaskReleased = elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitModkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfModKeys].bitMaskPressed ^ 0xff;
                    elfConfiguration[computer].ioConfiguration.bitKeypadEf[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitModkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfModKeys].ef] = true;
                    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfModKeys++;
                }
                else
                {
                    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey.resize(elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys+1);
                    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys].value = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys].bitMaskPressed = 1 << (tagTypeInt - TAG_BIT0);
                    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys].bitMaskReleased = elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys].bitMaskPressed ^ 0xff;
                    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys++;
                }
            break;
                
            case TAG_BIT:
                elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey.resize(elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys+1);
                
                bitNumber = child->GetNodeContent();
                elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys].value = (int)parseXml_Number(*child, "key");
                elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys].bitMaskPressed = 0;
                
                while (bitNumber != "")
                    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys].bitMaskPressed |= (1 << (int)getNextHexDec(&bitNumber));
                    
                elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys].bitMaskReleased = elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys].bitMaskPressed ^ 0xff;
                
                elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys++;
            break;

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].ioGroup = (int)parseXml_Number(*child);
            break;

            case TAG_PRESSED:
                elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitKeyPressed = (int)parseXml_Number(*child);
            break;

            case TAG_REPEAT:
                if (child->GetNodeContent() == "off")
                    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].repeat = false;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_CvKeypad (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].useCvKeypad = true;
    elfConfiguration[computer].ioConfiguration.cvKeypad.defined = true;
    
    wxString tagList[]=
    {
        "in",
        "ef",
        "pad",
        "keydef",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_EF,
        TAG_PAD,
        TAG_KEYDEF,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    wxString padModList[]=
    {
        "shift",
        "control",
        "alt",
        "cmd",
        "caps",
        "undefined"
    };

    int padModValueList[]=
    {
        wxMOD_SHIFT,
        wxMOD_CONTROL,
        wxMOD_ALT,
        wxMOD_CMD,
        WXK_CAPITAL,
        0
    };

    int padModIgnoreValueList[]=
    {
        WXK_SHIFT,
        WXK_CONTROL,
        WXK_ALT,
        WXK_COMMAND,
        WXK_CAPITAL,
        0
    };

    int tagTypeInt, keyValue, textEfKeyInt;
    wxString keyText, efText;
    wxString bitNumber;
    elfConfiguration[computer].ioConfiguration.cvKeypad.ioGroup = -1;
    elfConfiguration[computer].ioConfiguration.cvKeypad.reversed = false;
    elfConfiguration[computer].ioConfiguration.CvKeypadPadMask = 0;
    elfConfiguration[computer].ioConfiguration.CvKeypadModKey = 0;

    for (int i=0; i<255; i++)
        elfConfiguration[computer].ioConfiguration.cvKeypad.keydef[i] = -1;
    
    for (int i=0; i<LAST_MATRIX_TEXT_KEY; i++)
        elfConfiguration[computer].ioConfiguration.CvKeypadTextKey[i] = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.cvKeypad.inp = (int)parseXml_Number(*child);
            break;

            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.cvKeypad.ef = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    elfConfiguration[computer].ioConfiguration.cvKeypad.reversed = true;
            break;

            case TAG_KEYDEF:
                if (child->GetAttribute("type") == "")
                {
                    keyValue = (int)parseXml_Number(*child)&0xff;
                    elfConfiguration[computer].ioConfiguration.cvKeypad.keydef[keyValue] = (int)parseXml_Number(*child, "value");
                }
                if (child->GetAttribute("type") == "text")
                {
                    keyText = child->GetNodeContent();
                    textEfKeyInt = 0;
                    while (textKeyList[textEfKeyInt] != "undefined")
                    {
                        if (keyText == textKeyList[textEfKeyInt])
                            elfConfiguration[computer].ioConfiguration.CvKeypadTextKey[textEfKeyInt] = (int)parseXml_Number(*child, "value");
                        textEfKeyInt++;
                    }
                }
            break;
                
            case TAG_PAD:
                elfConfiguration[computer].ioConfiguration.CvKeypadPadMask = (int)parseXml_Number(*child, "mask");
                keyText = child->GetNodeContent();
                textEfKeyInt = 0;
                while (padModList[textEfKeyInt] != "undefined")
                {
                    if (keyText == padModList[textEfKeyInt])
                    {
                        elfConfiguration[computer].ioConfiguration.CvKeypadModKey = padModValueList[textEfKeyInt];
                        elfConfiguration[computer].ioConfiguration.CvKeypadModKeyIgnore = padModIgnoreValueList[textEfKeyInt];
                    }
                    textEfKeyInt++;
                }
            break;
                
            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.cvKeypad.ioGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_LatchKeypad(int computer, int pad, wxXmlNode &node)
{
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].defined = true;
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].IoGroup = -1;

    wxString tagList[]=
    {
        "out",
        "ef",
        "keydef",
        "diagonal",
        "auto",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_EF,
        TAG_KEYDEF,
        TAG_DIAGONAL,
        TAG_AUTO,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED,
    };

    int tagTypeInt, keyValue;
    wxString keyText;
    DiagonalKeys diagonalKeys;

//    for (int i=0; i<16; i++)
//    {
//        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pc[i] = i;
//    }
    for (int i=0; i<512; i++)
    {
        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pc[i] = -1;
        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pcShift[i] = -1;
        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pcForceShift[i] = -1;
        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pcForceNoShift[i] = -1;
        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pcMapShift[i] = -1;
        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pcMap[i] = -1;
    }
    
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].ef = -1;
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].altRight = -1;
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].altLeft = -1;
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].ctrlRight = -1;
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].ctrlLeft = -1;
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].shift = -1;
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].switchAltCtrl = -1;
    elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].loadAutoConfig = false;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].outPort = (int)parseXml_Number(*child);
                elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].mask = (int)parseXml_Number(*child, "mask");
            break;
                
            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].ef = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pressed = 0;
                else
                    elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pressed = 1;
            break;
                
            case TAG_KEYDEF:
                if (child->GetAttribute("type") == "")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                    {
                        if (child->HasAttribute("out"))
                            elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pc[keyValue] = (int)parseXml_Number(*child, "out");
                    }
                    if (child->HasAttribute("map"))
                    {
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pcMap[(int)parseXml_Number(*child, "map")] = (int)parseXml_Number(*child)&0x1ff;
                    }
                }
                
                if (child->GetAttribute("type") == "text")
                {
                    keyText = child->GetNodeContent();
                    if (keyText == "alt_right")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].altRight = (int)parseXml_Number(*child, "out");
                    if (keyText == "alt_left")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].altLeft = (int)parseXml_Number(*child, "out");
                    if (keyText == "ctrl_right")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].ctrlRight = (int)parseXml_Number(*child, "out");
                    if (keyText == "ctrl_left")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].ctrlLeft = (int)parseXml_Number(*child, "out");
                    if (keyText == "shift")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].shift = (int)parseXml_Number(*child, "out");
                    if (keyText == "switch")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].switchAltCtrl = (int)parseXml_Number(*child, "out");
                    if (keyText == "caps")
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].caps = (int)parseXml_Number(*child, "out");
                }

                if (child->GetAttribute("type") == "file")
                {
                    elfConfiguration[computer].ioConfiguration.keyPadDefinitionFile = child->GetNodeContent();
                    if (child->GetAttribute("check") == "rom")
                        elfConfiguration[computer].ioConfiguration.keypadCheckMemConfig = 1;  // Check ROM
                }

                if (child->GetAttribute("type") == "force")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                        if (child->HasAttribute("out"))
                            elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pcForceShift[keyValue] = (int)parseXml_Number(*child, "out");
                }

                if (child->GetAttribute("type") == "shift")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                        if (child->HasAttribute("out"))
                            elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pcShift[keyValue] = (int)parseXml_Number(*child, "out");
                    if (child->HasAttribute("map"))
                    {
                        elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].pcMapShift[(int)parseXml_Number(*child, "map")] = (int)parseXml_Number(*child)&0x1ff;
                    }
                }
            break;
                
            case TAG_DIAGONAL:
                diagonalKeys.mainKey = (int)parseXml_Number(*child);
                diagonalKeys.key1 = (int)parseXml_Number(*child, "key1");
                diagonalKeys.key2 = (int)parseXml_Number(*child, "key2");
                elfConfiguration[computer].ioConfiguration.diagonalKeys.push_back(diagonalKeys);
            break;

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].IoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_AUTO:
                if (child->GetNodeContent() == "on")
                    elfConfiguration[computer].ioConfiguration.keyLatchDetails[pad].loadAutoConfig = true;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_FredKeypad(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].ioConfiguration.fredKeypad.defined= true;
    elfConfiguration[computer].ioConfiguration.fredKeypad.ioGroup = -1;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "mode",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_MODE,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED,
    };

    int tagTypeInt;
    elfConfiguration[computer].ioConfiguration.fredKeypad.inpKey = 0;
    elfConfiguration[computer].ioConfiguration.fredKeypad.input_mode = 1;
    elfConfiguration[computer].ioConfiguration.fredKeypad.efKey = 1;
    elfConfiguration[computer].ioConfiguration.fredKeypad.ef_mode = 1;
    elfConfiguration[computer].ioConfiguration.fredKeypad.coinArcadeControl = false;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.fredKeypad.output = (int)parseXml_Number(*child);
            break;
                
            case TAG_IN:
                if (child->GetAttribute("type") == "key")
                {
                    elfConfiguration[computer].ioConfiguration.fredKeypad.inpKey = (int)parseXml_Number(*child);
                    if (child->HasAttribute("mode"))
                    {
                        elfConfiguration[computer].ioConfiguration.fredKeypad.input_mode = (int)parseXml_Number(*child, "mode");
                    }
                }
                if (child->GetAttribute("type") == "coin")
                {
                    elfConfiguration[computer].ioConfiguration.fredKeypad.coinArcadeControl = true;
                    elfConfiguration[computer].ioConfiguration.fredKeypad.inpCoin = (int)parseXml_Number(*child);
                }
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "key")
                {
                    elfConfiguration[computer].ioConfiguration.fredKeypad.efKey = (int)parseXml_Number(*child);
                    if (child->HasAttribute("mode"))
                    {
                        elfConfiguration[computer].ioConfiguration.fredKeypad.ef_mode = (int)parseXml_Number(*child, "mode");
                    }
                }
                if (child->GetAttribute("type") == "frieb")
                {
                    elfConfiguration[computer].ioConfiguration.fredKeypad.efFireB = (int)parseXml_Number(*child);
                }
                if (child->GetAttribute("type") == "coin")
                {
                    elfConfiguration[computer].ioConfiguration.fredKeypad.coinArcadeControl = true;
                    elfConfiguration[computer].ioConfiguration.fredKeypad.efCoin = (int)parseXml_Number(*child);
                }
            break;
                
            case TAG_MODE:
                if (child->GetNodeContent() == "hex")
                {
                    elfConfiguration[computer].ioConfiguration.fredKeypad.keypad_mode = FRED_HEX_MODE;
                }
                if (child->GetNodeContent() == "byte")
                {
                    elfConfiguration[computer].ioConfiguration.fredKeypad.keypad_mode = FRED_BYTE_MODE;
                }
                if (child->GetNodeContent() == "p-hex")
                {
                    elfConfiguration[computer].ioConfiguration.fredKeypad.keypad_mode = FRED_HEX_PULSE_MODE;
                }
            break;

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.fredKeypad.ioGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_EtiKeypad(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].ioConfiguration.etiKeypad.defined= true;
    elfConfiguration[computer].ioConfiguration.etiKeypad.ioGroup = -1;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED,
    };

    int tagTypeInt;
    elfConfiguration[computer].ioConfiguration.etiKeypad.out = 2;
    elfConfiguration[computer].ioConfiguration.etiKeypad.inp = 2;
    elfConfiguration[computer].ioConfiguration.etiKeypad.ef = 4;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.etiKeypad.out  = (int)parseXml_Number(*child);
            break;
                
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.etiKeypad.inp  = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.etiKeypad.ef  = (int)parseXml_Number(*child);
            break;
                
            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.etiKeypad.ioGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_EfButtons (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].efButtons = true;
    elfConfiguration[computer].ioConfiguration.efKeyPressed = 0;
    for (int i=1; i<5; i++)
    {
        elfConfiguration[computer].ioConfiguration.efKey[i].defined = false;
        elfConfiguration[computer].ioConfiguration.efKey[i].mod = false;
    }

    wxString tagList[]=
    {
        "ef1",
        "ef2",
        "ef3",
        "ef4",
        "pressed",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_EF1,
        TAG_EF2,
        TAG_EF3,
        TAG_EF4,
        TAG_PRESSED,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_EF1:
            case TAG_EF2:
            case TAG_EF3:
            case TAG_EF4:
                if (child->GetAttribute("type") == "mod")
                {
                    elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].modString = child->GetNodeContent();
                    if (elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].modString == "alt")
                    {
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].defined = true;
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].mod = true;
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].value = wxMOD_ALT;
                    }
                    if (elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].modString == "control")
                    {
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].defined = true;
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].mod = true;
#ifdef __WXMAC__
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].value = wxMOD_RAW_CONTROL;
#else
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].value = wxMOD_CONTROL;
#endif
                    }
                    if (elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].modString == "shift")
                    {
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].defined = true;
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].mod = true;
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].value = wxMOD_SHIFT;
                    }
                    if (elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].modString == "altgr")
                    {
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].defined = true;
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].mod = true;
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].value = wxMOD_ALTGR;
                    }
                    if (elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].modString == "meta")
                    {
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].defined = true;
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].mod = true;
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].value = wxMOD_META;
                    }
                    if (elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].modString == "command")
                    {
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].defined = true;
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].mod = true;
                        elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].value = wxMOD_CONTROL;
                    }
                }
                else
                {
                    elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].resetInp = -1;
                    if (child->GetAttribute("type") == "reset")
                    {
                        if (child->HasAttribute("in"))
                            elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].resetInp = (int)parseXml_Number(*child, "in");
                    }
                    elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].defined = true;
                    elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].value = (int)parseXml_Number(*child);
                }
            break;
 
            case TAG_PRESSED:
                elfConfiguration[computer].ioConfiguration.efKeyPressed = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_AdConvertor (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].useAdConvertor = true;

    wxString tagList[]=
    {
        "out",
        "in",
        "address",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_ADDRESS,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long start, end;

    elfConfiguration[computer].ioConfiguration.adConvertorAddressStart = -1;
    elfConfiguration[computer].ioConfiguration.adConvertorAddressEnd = -1;
    elfConfiguration[computer].ioConfiguration.adConvertorIn = -1;
    elfConfiguration[computer].ioConfiguration.adConvertorOut = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.adConvertorIn = (int)parseXml_Number(*child);
            break;
                                
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.adConvertorOut = (int)parseXml_Number(*child);
            break;

            case TAG_ADDRESS:
                if (!parseXml_Range(*child, &start, &end))
                {
                    elfConfiguration[computer].ioConfiguration.adConvertorAddressStart = (Word) start;
                    elfConfiguration[computer].ioConfiguration.adConvertorAddressEnd = (Word) start;
                }
                else
                {
                    elfConfiguration[computer].ioConfiguration.adConvertorAddressStart = (Word) start;
                    elfConfiguration[computer].ioConfiguration.adConvertorAddressEnd = (Word) end;
                }
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_FrontPanel (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "til",
        "led",
        "switch",
        "hex",
        "item",
        "size",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_TIL,
        TAG_LED,
        TAG_SWITCH,
        TAG_HEX,
        TAG_ITEM,
        TAG_SIZE,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    int xSize, ySize;
    wxString position;
    
    guiItemConfigNumber_ = 0;

    if (elfConfiguration[computer].useHexKeyboard && elfConfiguration[computer].panelType_ == PANEL_ELF2K)
    {
        elfConfiguration[computer].useHex = true;
        elfConfiguration[computer].useSwitch = false;
    }
    
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.hexOutput.portNumber = (int)parseXml_Number(*child);
                if (child->HasAttribute("mask"))
                    elfConfiguration[computer].ioConfiguration.hexOutput.mask = (Byte)parseXml_Number(*child, "mask");
            break;
                
            case TAG_IN:
                if (elfConfiguration[computer].panelType_ != PANEL_COSMAC && elfConfiguration[computer].panelType_ != PANEL_ELF2K && elfConfiguration[computer].panelType_ != PANEL_COSMICOS)
                    elfConfiguration[computer].useHexKeyboard = true;
                elfConfiguration[computer].ioConfiguration.hexInput.portNumber = (int)parseXml_Number(*child);
                if (child->HasAttribute("mask"))
                    elfConfiguration[computer].ioConfiguration.hexInput.mask = (Byte)parseXml_Number(*child, "mask");
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "ready")
                {
                    if (elfConfiguration[computer].panelType_ != PANEL_COSMAC && elfConfiguration[computer].panelType_ != PANEL_ELF2K)
                        elfConfiguration[computer].useHexKeyboardEf3 = true;
                    elfConfiguration[computer].ioConfiguration.hexEf = (int)parseXml_Number(*child);
                }
                if (child->GetAttribute("type") == "in")
                {
                    elfConfiguration[computer].useInButton = true;
                    elfConfiguration[computer].ioConfiguration.inEf = (int)parseXml_Number(*child);
                }
                if (child->GetAttribute("type") == "start")
                {
                    elfConfiguration[computer].useStartButton = true;
                    elfConfiguration[computer].ioConfiguration.startEf = (int)parseXml_Number(*child);
                }
            break;

            case TAG_TIL:
                if (child->GetNodeContent() == "311")
                    elfConfiguration[computer].tilType = TIL311;
                else
                {
                    elfConfiguration[computer].tilType = TIL313;
                    if (child->GetAttribute("font") == "italic")
                        elfConfiguration[computer].tilType = TIL313ITALIC;
                }
            break;
                
            case TAG_LED:
                if (elfConfiguration[computer].panelType_ == PANEL_COSMAC)
                {
                    elfConfiguration[computer].useLedModule = true;
                    elfConfiguration[computer].ioConfiguration.led_Module_Output = (int)parseXml_Number(*child);
                }
            break;

            case TAG_SWITCH:
                if (elfConfiguration[computer].panelType_ == PANEL_ELF2K)
                    elfConfiguration[computer].useSwitch = true;
                elfConfiguration[computer].useHex = false;
            break;

            case TAG_ITEM:
                parseXml_FrontPanelItem (computer, *child);
            break;

            case TAG_SIZE:
                position = child->GetNodeContent();
                xSize = (int)getNextHexDec(&position);
                ySize = (int)getNextHexDec(&position);
                elfConfiguration[computer].panelSize_ = wxSize(xSize, ySize);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_FrontPanelItem (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].flexPanel_ = true;

    wxString tagList[]=
    {
        "type",
        "value",
        "range",
        "function",
        "label",
        "init",
        "pos",
        "size",
        "filename",
        "dirname",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_TYPE,
        TAG_VALUE,
        TAG_RANGE,
        TAG_FUNCTION,
        TAG_LABEL,
        TAG_INIT,
        TAG_POS,
        TAG_SIZE,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    wxString itemFunctionList[]=
    {
        "in",
        "run",
        "mp",
        "run0",
        "load",
        "pause",
        "step",
        "reset",
        "monitor",
        "power",
        "read",
        "nano_monitor",
        "cardswitch",
        "card",
        "dipmp",
        "ems",
        "tmc_ad",
        "stopled",
        "readyled",
        "errorled",
        "qled",
        "resetled",
        "pauseled",
        "runled",
        "loadled",
        "bitled",
        "addressled",
        "cpustateled",
        "datatil",
        "addresstil",
        "undefined"
    };

    int itemFunctionValueList[]=
    {
        BUTTON_FUNC_IN,
        BUTTON_FUNC_RUN,
        BUTTON_FUNC_MP,
        BUTTON_FUNC_RUN0,
        BUTTON_FUNC_LOAD,
        BUTTON_FUNC_PAUSE,
        BUTTON_FUNC_STEP,
        BUTTON_FUNC_RESET,
        BUTTON_FUNC_MONITOR,
        BUTTON_FUNC_POWER,
        BUTTON_FUNC_READ,
        BUTTON_FUNC_NANO_MONITOR,
        BUTTON_FUNC_CARDSWITCH,
        BUTTON_FUNC_CARD,
        BUTTON_FUNC_DIPMP,
        BUTTON_FUNC_EMS,
        BUTTON_FUNC_TMC_AD,
        LED_FUNC_STOP,
        LED_FUNC_READY,
        LED_FUNC_ERROR,
        LED_FUNC_Q,
        LED_FUNC_RESET,
        LED_FUNC_PAUSE,
        LED_FUNC_RUN,
        LED_FUNC_LOAD,
        LED_FUNC_BIT,
        LED_FUNC_ADDRESS,
        LED_FUNC_CPUSTATE,
        TIL_DATA,
        TIL_ADDRESS,
        0
    };

    int tagTypeInt, itemFunctionId;
    int xpos, ypos;
    int xSize, ySize;
    int rangeLow, rangeHigh;
    wxString position, size, range, itemFunctionText;

    conf[computer].guiItemConfig_.resize(guiItemConfigNumber_+1);
    conf[computer].guiItemConfig_[guiItemConfigNumber_].type = SWITCH_BUTTON_VERTICAL;
    conf[computer].guiItemConfig_[guiItemConfigNumber_].value = -1;
    conf[computer].guiItemConfig_[guiItemConfigNumber_].function = -1;
    conf[computer].guiItemConfig_[guiItemConfigNumber_].label = "";
    conf[computer].guiItemConfig_[guiItemConfigNumber_].position = wxPoint(0,0);
    conf[computer].guiItemConfig_[guiItemConfigNumber_].textSize = 12;
    conf[computer].guiItemConfig_[guiItemConfigNumber_].initup = BUTTON_DOWN;
    conf[computer].guiItemConfig_[guiItemConfigNumber_].dirName = conf[computer].mainDir_;
    conf[computer].guiItemConfig_[guiItemConfigNumber_].useImageDir = false;
    
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_TYPE:
                if (child->GetNodeContent() == "switch")
                {
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].type = SWITCH_BUTTON_VERTICAL;
                    if (child->HasAttribute("direction"))
                    {
                        if (child->GetAttribute("direction") == "horizontal")
                            conf[computer].guiItemConfig_[guiItemConfigNumber_].type = SWITCH_BUTTON_HORIZONTAL;
                    }
                }
                
                if (child->GetNodeContent() == "dipswitch")
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].type = DIP_SWITCH_BUTTON;

                if (child->GetNodeContent() == "rot")
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].type = ROT_SWITCH_BUTTON;

                if (child->GetNodeContent() == "spin_adi")
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].type = ADI_SPINCTRL;

                if (child->GetNodeContent() == "spin_adi_volt")
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].type = ADI_VOLT_SPINCTRL;

                if (child->GetNodeContent() == "spin_ads")
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].type = ADS_SPINCTRL;

                if (child->GetNodeContent() == "spin_ads_volt")
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].type = ADS_VOLT_SPINCTRL;

                if (child->GetNodeContent() == "push")
                {
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].type = PUSH_BUTTON;
                    if (child->HasAttribute("size"))
                    {
                        if (child->GetAttribute("size") == "small")
                            conf[computer].guiItemConfig_[guiItemConfigNumber_].type = PUSH_BUTTON_SMALL;
                    }
                    
                    if (child->HasAttribute("form"))
                    {
                        if (child->GetAttribute("form") == "rectangle")
                        {
                            if (conf[computer].guiItemConfig_[guiItemConfigNumber_].type == PUSH_BUTTON_SMALL)
                                conf[computer].guiItemConfig_[guiItemConfigNumber_].type = PUSH_BUTTON_RECTANGLE_SMALL;
                            else
                                conf[computer].guiItemConfig_[guiItemConfigNumber_].type = PUSH_BUTTON_RECTANGLE;
                        }
                        if (child->GetAttribute("form") == "round")
                        {
                            conf[computer].guiItemConfig_[guiItemConfigNumber_].type = PUSH_BUTTON_ROUND_BLACK;
                        }
                    }
                    
                    if (child->HasAttribute("color"))
                    {
                        if (child->GetAttribute("color") == "red")
                        {
                            if(conf[computer].guiItemConfig_[guiItemConfigNumber_].type == PUSH_BUTTON)
                                conf[computer].guiItemConfig_[guiItemConfigNumber_].type = PUSH_BUTTON_RED;
                            if(conf[computer].guiItemConfig_[guiItemConfigNumber_].type == PUSH_BUTTON_ROUND_BLACK)
                                conf[computer].guiItemConfig_[guiItemConfigNumber_].type = PUSH_BUTTON_ROUND_RED;
                        }
                    }
                }

                if (child->GetNodeContent() == "text")
                {
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].color = GUI_COL_BLACK;
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].type = PANEL_TEXT;
                    if (child->HasAttribute("color"))
                    {
                        if (child->GetAttribute("color") == "grey")
                        {
                            conf[computer].guiItemConfig_[guiItemConfigNumber_].color = GUI_COL_GREY;
                        }
                    }
                }

                if (child->GetNodeContent() == "png")
                {
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].type = PANEL_PNG;
                }
                
                if (child->GetNodeContent() == "led")
                {
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].type = LED_SMALL_RED;
                    if (child->GetAttribute("form") == "realred")
                        conf[computer].guiItemConfig_[guiItemConfigNumber_].type = LED_REAL_RED;
                    if (child->GetAttribute("form") == "realgreen")
                        conf[computer].guiItemConfig_[guiItemConfigNumber_].type = LED_REAL_GREEN;
                    if (child->GetAttribute("form") == "realorange")
                        conf[computer].guiItemConfig_[guiItemConfigNumber_].type = LED_REAL_ORANGE;
                    if (child->GetAttribute("form") == "large")
                        conf[computer].guiItemConfig_[guiItemConfigNumber_].type = LED_LARGE_RED;
                    if (child->GetAttribute("form") == "largegreen")
                        conf[computer].guiItemConfig_[guiItemConfigNumber_].type = LED_LARGE_GREEN;
                    if (child->GetAttribute("form") == "disable")
                        conf[computer].guiItemConfig_[guiItemConfigNumber_].type = LED_SMALL_RED_DISABLE;
                }

                if (child->GetNodeContent() == "til")
                {
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].type = TIL_311;
                    if (child->GetAttribute("form") == "313")
                    {
                        conf[computer].guiItemConfig_[guiItemConfigNumber_].type = TIL_313;
                        if (child->GetAttribute("font") == "italic")
                            conf[computer].guiItemConfig_[guiItemConfigNumber_].type = TIL_313_ITALIC;
                    }
                }
            break;

            case TAG_VALUE:
                conf[computer].guiItemConfig_[guiItemConfigNumber_].value = (int)parseXml_Number(*child);
                if (conf[computer].guiItemConfig_[guiItemConfigNumber_].function >= BUTTON_FUNC_IN && conf[computer].guiItemConfig_[guiItemConfigNumber_].function < BUTTON_FUNC_CARD)
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].value = conf[computer].guiItemConfig_[guiItemConfigNumber_].function+20;
                
                switch (conf[computer].guiItemConfig_[guiItemConfigNumber_].function)
                {
                    case BUTTON_FUNC_DIPMP:
                        conf[computer].guiItemConfig_[guiItemConfigNumber_].value += 0x50;
                    break;

                    case BUTTON_FUNC_EMS:
                        conf[computer].guiItemConfig_[guiItemConfigNumber_].value += 0x60;
                    break;

                    case BUTTON_FUNC_TMC_AD:
                        conf[computer].guiItemConfig_[guiItemConfigNumber_].value += 0x70;
                    break;

                    case BUTTON_FUNC_CARD:
                        conf[computer].guiItemConfig_[guiItemConfigNumber_].value += 0x100;
                    break;

                    case LED_FUNC_BIT:
                        if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value >= 8)
                            conf[computer].guiItemConfig_[guiItemConfigNumber_].value = 0;
                        if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value != -1)
                            elfConfiguration[computer].ioConfiguration.bitLed[conf[computer].guiItemConfig_[guiItemConfigNumber_].value] = true;
                    break;

                    case LED_FUNC_ADDRESS:
                        if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value >= MAX_BIT_LEDS - 8)
                            conf[computer].guiItemConfig_[guiItemConfigNumber_].value = 0;
                        if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value != -1)
                            elfConfiguration[computer].ioConfiguration.bitLed[conf[computer].guiItemConfig_[guiItemConfigNumber_].value+8] = true;
                    break;

                    case LED_FUNC_CPUSTATE:
                        if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value >= MAX_CPU_STATE_LEDS)
                            conf[computer].guiItemConfig_[guiItemConfigNumber_].value = 0;
                        if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value != -1)
                            elfConfiguration[computer].ioConfiguration.cpuStatusLed[conf[computer].guiItemConfig_[guiItemConfigNumber_].value] = true;
                    break;
                        
                    case TIL_DATA:
                        if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value >= MAX_DATA_TIL)
                            conf[computer].guiItemConfig_[guiItemConfigNumber_].value = 0;
                        if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value != -1)
                            elfConfiguration[computer].ioConfiguration.datatil[conf[computer].guiItemConfig_[guiItemConfigNumber_].value] = true;
                    break;

                    case TIL_ADDRESS:
                        if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value >= MAX_ADDRESS_TIL)
                            conf[computer].guiItemConfig_[guiItemConfigNumber_].value = 0;
                        if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value != -1)
                            elfConfiguration[computer].ioConfiguration.addresstil[conf[computer].guiItemConfig_[guiItemConfigNumber_].value] = true;
                    break;
                }
            break;
                
            case TAG_RANGE:
                range = child->GetNodeContent();
                rangeLow = (int)getNextHexDec(&range);
                rangeHigh = (int)getNextHexDec(&range);
                if (rangeLow > rangeHigh)
                {
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].rangeLow = rangeHigh;
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].rangeHigh = rangeLow;
                }
                else
                {
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].rangeLow = rangeLow;
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].rangeHigh = rangeHigh;
                }
            break;

            case TAG_FUNCTION:
                itemFunctionText = child->GetNodeContent();
                itemFunctionId = 0;
                while (itemFunctionList[itemFunctionId] != "undefined")
                {
                    if (itemFunctionText == itemFunctionList[itemFunctionId])
                    {
                        conf[computer].guiItemConfig_[guiItemConfigNumber_].function = itemFunctionValueList[itemFunctionId];
                        if (itemFunctionId >= BUTTON_FUNC_IN && itemFunctionId < BUTTON_FUNC_CARD)
                            conf[computer].guiItemConfig_[guiItemConfigNumber_].value = itemFunctionId+20;
                        switch (itemFunctionId)
                        {
                            case BUTTON_FUNC_DIPMP:
                                conf[computer].guiItemConfig_[guiItemConfigNumber_].value += 0x50;
                            break;

                            case BUTTON_FUNC_EMS:
                                conf[computer].guiItemConfig_[guiItemConfigNumber_].value += 0x60;
                            break;

                            case BUTTON_FUNC_TMC_AD:
                                conf[computer].guiItemConfig_[guiItemConfigNumber_].value += 0x70;
                            break;

                            case BUTTON_FUNC_CARD:
                                conf[computer].guiItemConfig_[guiItemConfigNumber_].value += 0x100;
                            break;

                            case BUTTON_FUNC_RUN:
                                if (child->GetAttribute("type") == "reset")
                                    elfConfiguration[computer].ioConfiguration.runPressType = RUN_TYPE_RESET;
                                if (child->GetAttribute("type") == "boot")
                                    elfConfiguration[computer].ioConfiguration.runPressType = RUN_TYPE_BOOT;
                            break;

                            case BUTTON_FUNC_RUN0:
                                if (child->GetAttribute("type") == "reset")
                                    elfConfiguration[computer].ioConfiguration.runPressType = RUN_TYPE_RESET;
                                if (child->GetAttribute("type") == "boot")
                                    elfConfiguration[computer].ioConfiguration.runPressType = RUN_TYPE_BOOT;
                            break;

                            case BUTTON_FUNC_RESET:
                                if (child->GetAttribute("type") == "full")
                                    elfConfiguration[computer].ioConfiguration.resetPressType = RESET_TYPE_FULL;
                            break;

                            case LED_FUNC_Q:
                                elfConfiguration[computer].ioConfiguration.qLed = true;
                            break;

                            case LED_FUNC_ERROR:
                                elfConfiguration[computer].ioConfiguration.errorLed = true;
                            break;

                            case LED_FUNC_RUN:
                                elfConfiguration[computer].ioConfiguration.runLed = true;
                            break;

                            case LED_FUNC_READY:
                                elfConfiguration[computer].ioConfiguration.readyLed = true;
                            break;

                            case LED_FUNC_STOP:
                                elfConfiguration[computer].ioConfiguration.stopLed = true;
                            break;

                            case LED_FUNC_BIT:
                                if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value != -1)
                                    elfConfiguration[computer].ioConfiguration.bitLed[conf[computer].guiItemConfig_[guiItemConfigNumber_].value] = true;
                                if (child->GetAttribute("load") == "show")
                                    elfConfiguration[computer].ioConfiguration.showDataOnLoad = true;
                                if (child->GetAttribute("cycle") == "show")
                                    elfConfiguration[computer].ioConfiguration.showDataOnCycle = true;
                            break;

                            case LED_FUNC_ADDRESS:
                                if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value != -1)
                                    elfConfiguration[computer].ioConfiguration.bitLed[conf[computer].guiItemConfig_[guiItemConfigNumber_].value+8] = true;
                                if (child->GetAttribute("cycle") == "show")
                                    elfConfiguration[computer].ioConfiguration.showAddressOnCycle = true;
                            break;

                            case LED_FUNC_CPUSTATE:
                                if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value != -1)
                                    elfConfiguration[computer].ioConfiguration.cpuStatusLed[conf[computer].guiItemConfig_[guiItemConfigNumber_].value] = true;
                            break;

                            case TIL_DATA:
                                if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value != -1)
                                    elfConfiguration[computer].ioConfiguration.datatil[conf[computer].guiItemConfig_[guiItemConfigNumber_].value] = true;
                                if (child->GetAttribute("cycle") == "show")
                                    elfConfiguration[computer].ioConfiguration.showDataOnCycle = true;
                            break;

                            case TIL_ADDRESS:
                                if (conf[computer].guiItemConfig_[guiItemConfigNumber_].value != -1)
                                    elfConfiguration[computer].ioConfiguration.addresstil[conf[computer].guiItemConfig_[guiItemConfigNumber_].value] = true;
                                if (child->GetAttribute("cycle") == "show")
                                    elfConfiguration[computer].ioConfiguration.showAddressOnCycle = true;
                            break;
                        }
                    }
                    itemFunctionId++;
                }
            break;

            case TAG_LABEL:
                conf[computer].guiItemConfig_[guiItemConfigNumber_].label = child->GetNodeContent();
                if (child->HasAttribute("size"))
#if defined (__WXMAC__)
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].textSize = (int)parseXml_Number(*child, "size")+4;
#else
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].textSize = (int)parseXml_Number(*child, "size");
#endif
            break;
                
            case TAG_INIT:
                if (child->GetNodeContent() == "up")
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].initup = BUTTON_UP;
            break;
                
            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                conf[computer].guiItemConfig_[guiItemConfigNumber_].position = wxPoint(xpos, ypos);
            break;

            case TAG_SIZE:
                size = child->GetNodeContent();
                xSize = (int)getNextHexDec(&size);
                ySize = (int)getNextHexDec(&size);
                conf[computer].guiItemConfig_[guiItemConfigNumber_].size = wxSize(xSize, ySize);
            break;

            case TAG_COMMENT:
            break;

            case TAG_FILENAME:
                if (child->GetAttribute("dirname") == "images")
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].useImageDir = true;
                conf[computer].guiItemConfig_[guiItemConfigNumber_].fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                conf[computer].guiItemConfig_[guiItemConfigNumber_].dirName = dataDir_ + child->GetNodeContent();
                if (conf[computer].guiItemConfig_[guiItemConfigNumber_].dirName != pathSeparator_)
                    conf[computer].guiItemConfig_[guiItemConfigNumber_].dirName += pathSeparator_;
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
    guiItemConfigNumber_++;
}

void XmlParser::parseXml_Cdp1851 (int computer, wxXmlNode &node, bool windowOn)
{
    Cdp1851 cdp1851;
    
    cdp1851.windowOpen = windowOn;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "pos",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_POS,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString position;
    
    cdp1851.writePortA = init_IoPort();
    cdp1851.writePortB = init_IoPort();
    cdp1851.readPortA = init_IoPort();
    cdp1851.readPortB = init_IoPort();

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                if (child->GetAttribute("type") == "a")
                    cdp1851.writePortA = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "b")
                    cdp1851.writePortB = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "control")
                    cdp1851.writeControl = parseXml_IoPort(*child);
            break;
                
            case TAG_IN:
                if (child->GetAttribute("type") == "a")
                    cdp1851.readPortA = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "b")
                    cdp1851.readPortB = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "status")
                    cdp1851.readStatus = parseXml_IoPort(*child);
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "a")
                    cdp1851.efaRdy = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "b")
                    cdp1851.efbRdy = (int)parseXml_Number(*child);
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                cdp1851.defaultPos.x = (int)getNextHexDec(&position);
                cdp1851.defaultPos.y = (int)getNextHexDec(&position);
                if (cdp1851.defaultPos.x != -1)
                    cdp1851.defaultPos.x += conf[computer].defaultFrontPanelX_;
                if (cdp1851.defaultPos.y != -1)
                    cdp1851.defaultPos.y += conf[computer].defaultFrontPanelY_;
            break;

            case TAG_IOGROUP:
                cdp1851.ioGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
    elfConfiguration[computer].ioConfiguration.cdp1851.push_back(cdp1851);
}

void XmlParser::parseXml_Cdp1852 (int computer, wxXmlNode &node, bool windowOn)
{
    Cdp1852 cdp1852;
    
    cdp1852.windowOpen = windowOn;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "pos",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_POS,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString position;
    
    cdp1852.writePort = init_IoPort();
    cdp1852.readPort = init_IoPort();
    cdp1852.efStb = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                cdp1852.writePort = parseXml_IoPort(*child);
            break;
                
            case TAG_IN:
                cdp1852.readPort = parseXml_IoPort(*child);
            break;
                
            case TAG_EF:
                cdp1852.efStb = (int)parseXml_Number(*child);
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                cdp1852.defaultPos.x = (int)getNextHexDec(&position);
                cdp1852.defaultPos.y = (int)getNextHexDec(&position);
                if (cdp1852.defaultPos.x != -1)
                    cdp1852.defaultPos.x += conf[computer].defaultFrontPanelX_;
                if (cdp1852.defaultPos.y != -1)
                    cdp1852.defaultPos.y += conf[computer].defaultFrontPanelY_;
            break;

            case TAG_IOGROUP:
                cdp1852.ioGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
    elfConfiguration[computer].ioConfiguration.cdp1852.push_back(cdp1852);
}

void XmlParser::parseXml_Cd4536b (int computer, wxXmlNode &node)
{
    Cd4536bIo cd4536bIo;
    
    wxString tagList[]=
    {
        "out",
        "ef",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_EF,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString position;
    
    cd4536bIo.writeControl = init_IoPort();
    cd4536bIo.ef = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                cd4536bIo.writeControl = parseXml_IoPort(*child);
            break;
                
            case TAG_EF:
                cd4536bIo.ef = (int)parseXml_Number(*child);
            break;

            case TAG_IOGROUP:
                cd4536bIo.ioGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
    elfConfiguration[computer].ioConfiguration.cd4536bIo.push_back(cd4536bIo);
}

void XmlParser::parseXml_SerialVt (int computer, wxXmlNode &node)
{
    int baudRateValue_[] =
    {
        38400, 19200, 9600, 4800, 3600, 2400, 2000, 1800, 1200, 600, 300, 200, 150, 134, 110, 75, 50
    };

    wxString tagList[]=
    {
        "font",
        "wav",
        "dirname",
        "out",
        "ef",
        "q",
        "serialport",
        "timeout",
        "baud",
        "characters",
        "border",
        "pos",
        "color",
        "caps",
        "show",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FONT,
        TAG_WAV,
        TAG_DIRNAME,
        TAG_OUT,
        TAG_EF,
        TAG_Q,
        TAG_SERIALPORT,
        TAG_TIMEOUT,
        TAG_BAUD,
        TAG_CHARACTERS,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_CAPS,
        TAG_SHOW,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    int baud;
    int number, red, green, blue, xpos, ypos;
    long width, height;
    wxString color, position;

    elfConfiguration[computer].ioConfiguration.vt100Output = -1;
    
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_FONT:
                if (elfConfiguration[computer].vtType == VT52)
                    elfConfiguration[computer].vt52CharRom_ = child->GetNodeContent();
                if (elfConfiguration[computer].vtType == VT100)
                    elfConfiguration[computer].vt100CharRom_ = child->GetNodeContent();
            break;

            case TAG_WAV:
                elfConfiguration[computer].vtWavFile_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                if (child->GetAttribute("type") == "font")
                {
                    if (elfConfiguration[computer].vtType == VT52)
                    {
                        vt52CharRomDirDefined_ = true;
                        elfConfiguration[computer].vt52CharRomDir_ = dataDir_ + child->GetNodeContent();
                        if (elfConfiguration[computer].vt52CharRomDir_.Right(1) != pathSeparator_)
                            elfConfiguration[computer].vt52CharRomDir_ += pathSeparator_;
                    }
                    if (elfConfiguration[computer].vtType == VT100)
                    {
                        vt100CharRomDirDefined_ = true;
                        elfConfiguration[computer].vt100CharRomDir_ = dataDir_ + child->GetNodeContent();
                        if (elfConfiguration[computer].vt100CharRomDir_.Right(1) != pathSeparator_)
                            elfConfiguration[computer].vt100CharRomDir_ += pathSeparator_;
                    }
                }
                if (child->GetAttribute("type") == "wav")
                {
                    vtWavFileDirDefined_ = true;
                    elfConfiguration[computer].vtWavFileDir_ = dataDir_ + child->GetNodeContent();
                    if (elfConfiguration[computer].vtWavFileDir_.Right(1) != pathSeparator_)
                        elfConfiguration[computer].vtWavFileDir_ += pathSeparator_;
                }
            break;

            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.vt100Output = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.vt100Ef = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    elfConfiguration[computer].ioConfiguration.vt100ReverseEf = 0;
                else
                    elfConfiguration[computer].ioConfiguration.vt100ReverseEf = 1;
            break;
                
            case TAG_Q:
                if (child->GetAttribute("pol") == "rev")
                    elfConfiguration[computer].ioConfiguration.vt100ReverseQ = 1;
                else
                    elfConfiguration[computer].ioConfiguration.vt100ReverseQ = 0;
            break;
                
            case TAG_SERIALPORT:
                elfConfiguration[computer].serialPort_ = child->GetNodeContent();
            break;

            case TAG_TIMEOUT:
                elfConfiguration[computer].serialPortTimeout_ = (int)parseXml_Number(*child);
            break;

            case TAG_BAUD:
                baud = (int)parseXml_Number(*child);
                number = 0;
                while (baud < baudRateValue_[number] && baudRateValue_[number] != 50)
                    number++;
                if (child->GetAttribute("type") == "receive")
                elfConfiguration[computer].baudR = number;
                if (child->GetAttribute("type") == "transmit")
                elfConfiguration[computer].baudT = number;
            break;

            case TAG_CHARACTERS:
                if (child->GetNodeContent() == "132")
                {
                    elfConfiguration[computer].vtCharactersPerRow = 132;
                    elfConfiguration[computer].vt100CharWidth = 8;
                }
                else
                {
                    elfConfiguration[computer].vtCharactersPerRow = 80;
                    elfConfiguration[computer].vt100CharWidth = 10;
                }
            break;
                
            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOVT] = (int)width;
                    screenInfo[computer].borderY[VIDEOVT] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defVtX_;
                if (ypos != -1)
                    ypos += conf[computer].defVtY_;
                conf[computer].defVtX_ = xpos;
                conf[computer].defVtY_ = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo[computer].defaultColour[COL_VT_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo[computer].defaultColour[COL_VT_BACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "high")
                    screenInfo[computer].defaultColour[COL_VT_HIGH].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_CAPS:
                elfConfiguration[computer].forceUpperCase = true;
            break;

            case TAG_SHOW:
                if (child->GetNodeContent() == "no")
                    elfConfiguration[computer].vtShow = false;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_UartVt (int computer, wxXmlNode &node, bool uart16450)
{
    int baudRateValue_[] =
    {
        38400, 19200, 9600, 4800, 3600, 2400, 2000, 1800, 1200, 600, 300, 200, 150, 134, 110, 75, 50
    };

    wxString tagList[]=
    {
        "font",
        "wav",
        "dirname",
        "in",
        "out",
        "ef",
        "serialport",
        "timeout",
        "baud",
        "characters",
        "border",
        "pos",
        "color",
        "caps",
        "show",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FONT,
        TAG_WAV,
        TAG_DIRNAME,
        TAG_IN,
        TAG_OUT,
        TAG_EF,
        TAG_SERIALPORT,
        TAG_TIMEOUT,
        TAG_BAUD,
        TAG_CHARACTERS,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_CAPS,
        TAG_SHOW,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    int baud;
    int number, red, green, blue, xpos, ypos;
    long width, height;
    wxString color, position;
    
    elfConfiguration[computer].ioConfiguration.vt100Ef = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_FONT:
                if (elfConfiguration[computer].vtType == VT52)
                    elfConfiguration[computer].vt52CharRom_ = child->GetNodeContent();
                if (elfConfiguration[computer].vtType == VT100)
                    elfConfiguration[computer].vt100CharRom_ = child->GetNodeContent();
            break;

            case TAG_WAV:
                elfConfiguration[computer].vtWavFile_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                if (child->GetAttribute("type") == "font")
                {
                    if (elfConfiguration[computer].vtType == VT52)
                    {
                        vt52CharRomDirDefined_ = true;
                        elfConfiguration[computer].vt52CharRomDir_ = dataDir_ + child->GetNodeContent();
                        if (elfConfiguration[computer].vt52CharRomDir_.Right(1) != pathSeparator_)
                            elfConfiguration[computer].vt52CharRomDir_ += pathSeparator_;
                    }
                    if (elfConfiguration[computer].vtType == VT100)
                    {
                        vt100CharRomDirDefined_ = true;
                        elfConfiguration[computer].vt100CharRomDir_ = dataDir_ + child->GetNodeContent();
                        if (elfConfiguration[computer].vt100CharRomDir_.Right(1) != pathSeparator_)
                            elfConfiguration[computer].vt100CharRomDir_ += pathSeparator_;
                    }
                }
                if (child->GetAttribute("type") == "wav")
                {
                    vtWavFileDirDefined_ = true;
                    elfConfiguration[computer].vtWavFileDir_ = dataDir_ + child->GetNodeContent();
                    if (elfConfiguration[computer].vtWavFileDir_.Right(1) != pathSeparator_)
                        elfConfiguration[computer].vtWavFileDir_ += pathSeparator_;
                }
            break;
                
            case TAG_IN:
                if (uart16450)
                {
                    if (child->GetAttribute("type") == "port")
                        elfConfiguration[computer].ioConfiguration.uartIn = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "status")
                        elfConfiguration[computer].ioConfiguration.uartStatus = (int)parseXml_Number(*child);
                }
                else
                {
                    if (child->GetAttribute("type") == "register")
                        elfConfiguration[computer].ioConfiguration.uartIn = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "status")
                        elfConfiguration[computer].ioConfiguration.uartStatus = (int)parseXml_Number(*child);
                }
            break;

            case TAG_OUT:
                if (uart16450)
                {
                    if (child->GetAttribute("type") == "port")
                        elfConfiguration[computer].ioConfiguration.uartOut = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "select")
                        elfConfiguration[computer].ioConfiguration.uartControl = (int)parseXml_Number(*child);
                }
                else
                {
                    if (child->GetAttribute("type") == "register")
                        elfConfiguration[computer].ioConfiguration.uartOut = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "control")
                        elfConfiguration[computer].ioConfiguration.uartControl = (int)parseXml_Number(*child);
                }
            break;
                
            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.vt100Ef = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    elfConfiguration[computer].ioConfiguration.vt100ReverseEf = 0;
                else
                    elfConfiguration[computer].ioConfiguration.vt100ReverseEf = 1;
            break;
                
            case TAG_SERIALPORT:
                elfConfiguration[computer].serialPort_ = child->GetNodeContent();
            break;

            case TAG_TIMEOUT:
                elfConfiguration[computer].serialPortTimeout_ = (int)parseXml_Number(*child);
            break;

            case TAG_BAUD:
                baud = (int)parseXml_Number(*child);
                number = 0;
                while (baud < baudRateValue_[number] && baudRateValue_[number] != 50)
                    number++;
                if (child->GetAttribute("type") == "receive")
                elfConfiguration[computer].baudR = number;
                if (child->GetAttribute("type") == "transmit")
                elfConfiguration[computer].baudT = number;
            break;

            case TAG_CHARACTERS:
                if (child->GetNodeContent() == "132")
                {
                    elfConfiguration[computer].vtCharactersPerRow = 132;
                    elfConfiguration[computer].vt100CharWidth = 8;
                }
                else
                {
                    elfConfiguration[computer].vtCharactersPerRow = 80;
                    elfConfiguration[computer].vt100CharWidth = 10;
                }
            break;
                
            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo[computer].borderX[VIDEOVT] = (int)width;
                    screenInfo[computer].borderY[VIDEOVT] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += conf[computer].defVtX_;
                if (ypos != -1)
                    ypos += conf[computer].defVtY_;
                conf[computer].defVtX_ = xpos;
                conf[computer].defVtY_ = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo[computer].defaultColour[COL_VT_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo[computer].defaultColour[COL_VT_BACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "high")
                    screenInfo[computer].defaultColour[COL_VT_HIGH].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.uartIoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_CAPS:
                elfConfiguration[computer].forceUpperCase = true;
            break;

            case TAG_SHOW:
                if (child->GetNodeContent() == "no")
                    elfConfiguration[computer].vtShow = false;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Printer (int computer, wxXmlNode &node, int printerType)
{
    wxString tagList[]=
    {
        "iogroup",
        "out",
        "in",
        "ef",
        "filename",
        "dirname",
        "bits",
        "parity",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IOGROUP,
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_BITS,
        TAG_PARITY,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt, ioValue;

    conf[computer].printerOn_ = true;
    elfConfiguration[computer].ioConfiguration.printerIoGroup = -1;
    elfConfiguration[computer].ioConfiguration.parallelPrinterIoGroup = -1;
    elfConfiguration[computer].ioConfiguration.serialPrinterIoGroup = -1;
    elfConfiguration[computer].ioConfiguration.thermalPrinterIoGroup = -1;
    elfConfiguration[computer].ioConfiguration.qSerialPrinterIoGroup = -1;
    elfConfiguration[computer].ioConfiguration.centronicsPrinterIoGroup = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IOGROUP:
                ioValue = (int)parseXml_Number(*child);
                switch (printerType)
                {
                    case PRINTER_BASIC:
                        elfConfiguration[computer].ioConfiguration.printerIoGroup = ioValue;
                    break;

                    case PRINTER_PARALLEL:
                        elfConfiguration[computer].ioConfiguration.parallelPrinterIoGroup = ioValue;
                    break;

                    case PRINTER_SERIAL:
                        elfConfiguration[computer].ioConfiguration.serialPrinterIoGroup = ioValue;
                    break;
 
                    case COMXTHERMAL:
                        elfConfiguration[computer].ioConfiguration.thermalPrinterIoGroup = ioValue;
                    break;

                    case PRINTER_SERIAL_Q:
                        elfConfiguration[computer].ioConfiguration.qSerialPrinterIoGroup = ioValue;
                    break;

                    case PRINTER_CENTRONICS:
                        elfConfiguration[computer].ioConfiguration.centronicsPrinterIoGroup = ioValue;
                    break;
                }
            break;

            case TAG_OUT:
                ioValue = (int)parseXml_Number(*child);
                switch (printerType)
                {
                    case PRINTER_BASIC:
                        elfConfiguration[computer].ioConfiguration.printerOutput = ioValue;
                    break;

                    case PRINTER_PARALLEL:
                        elfConfiguration[computer].ioConfiguration.parallelPrinterOutput = ioValue;
                    break;

                    case PRINTER_SERIAL:
                        elfConfiguration[computer].ioConfiguration.serialPrinterOutput = ioValue;
                    break;
 
                    case COMXTHERMAL:
                        elfConfiguration[computer].ioConfiguration.thermalPrinterOutput = ioValue;
                    break;

                    case PRINTER_CENTRONICS:
                        elfConfiguration[computer].ioConfiguration.centronicsPrinterOutput = ioValue;
                    break;
                }
            break;
                
            case TAG_IN:
                ioValue = (int)parseXml_Number(*child);
                switch (printerType)
                {
                    case PRINTER_PARALLEL:
                        elfConfiguration[computer].ioConfiguration.parallelPrinterInput = ioValue;
                    break;

                    case PRINTER_SERIAL:
                        elfConfiguration[computer].ioConfiguration.serialPrinterInput = ioValue;
                    break;
 
                    case COMXTHERMAL:
                        elfConfiguration[computer].ioConfiguration.thermalPrinterInput = ioValue;
                    break;
                }
            break;
                
            case TAG_EF:
                ioValue = (int)parseXml_Number(*child);
                switch (printerType)
                {
                    case PRINTER_BASIC:
                        elfConfiguration[computer].ioConfiguration.printerEf = ioValue;
                    break;

                    case COMXTHERMAL:
                        elfConfiguration[computer].ioConfiguration.thermalPrinterEf = ioValue;
                    break;

                    case PRINTER_SERIAL_Q:
                        elfConfiguration[computer].ioConfiguration.qSerialPrinterEf = ioValue;
                    break;

                    case PRINTER_CENTRONICS:
                        elfConfiguration[computer].ioConfiguration.centronicsPrinterEf = ioValue;
                    break;
                }
            break;

            case TAG_FILENAME:
                printFileDefined_ = true;
                conf[computer].printFile_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                printFileDirDefined_ = true;
                conf[computer].printFileDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].printFileDir_.Right(1) != pathSeparator_)
                    conf[computer].printFileDir_ += pathSeparator_;
            break;

            case TAG_BITS:
                elfConfiguration[computer].ioConfiguration.serialPrinterBits = (int)parseXml_Number(*child);
                if (elfConfiguration[computer].ioConfiguration.serialPrinterBits < 5)
                    elfConfiguration[computer].ioConfiguration.serialPrinterBits = 5;
                if (elfConfiguration[computer].ioConfiguration.serialPrinterBits > 8)
                    elfConfiguration[computer].ioConfiguration.serialPrinterBits = 8;
            break;

            case TAG_PARITY:
                elfConfiguration[computer].ioConfiguration.serialPrinterParity = PRINTER_PARITY_NONE;
                if (child->GetNodeContent() == "odd")
                    elfConfiguration[computer].ioConfiguration.serialPrinterParity = PRINTER_PARITY_ODD;
                if (child->GetNodeContent() == "even")
                    elfConfiguration[computer].ioConfiguration.serialPrinterParity = PRINTER_PARITY_EVEN;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Cassette (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "ef",
        "in",
        "out",
        "freq",
        "threshold",
        "stoptone",
        "bootstart",
        "startbit",
        "databits",
        "stopbit",
        "stopdelay",
        "endtapedelay",
        "filename",
        "dirname",
        "iogroup",
        "channel",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_EF,
        TAG_IN,
        TAG_OUT,
        TAG_FREQ,
        TAG_THRESHOLD,
        TAG_STOPTONE,
        TAG_BOOTSTART,
        TAG_STARTBIT,
        TAG_DATABITS,
        TAG_STOPBIT,
        TAG_STOPDELAY,
        TAG_ENDTAPEDELAY,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_IOGROUP,
        TAG_CHANNEL,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt, cassetteNumber;

    elfConfiguration[computer].ioConfiguration.tapeIoGroup = -1;
    elfConfiguration[computer].ioConfiguration.tapeIn = init_IoPort();
    elfConfiguration[computer].ioConfiguration.tapeOut = init_IoPort();
    elfConfiguration[computer].ioConfiguration.tapeQOut = init_IoPort();

    elfConfiguration[computer].stopTone = false;
    elfConfiguration[computer].tapeStart = false;
    elfConfiguration[computer].tape_revInput = false;
    elfConfiguration[computer].tape_startBit = 1;
    elfConfiguration[computer].tape_dataBits = 8;
    elfConfiguration[computer].tape_stopBit = 0;
    elfConfiguration[computer].tape_stopBitIgnore = false;
    elfConfiguration[computer].tape_stopDelay = 0;
    elfConfiguration[computer].tape_frequency0 = 4000;
    elfConfiguration[computer].tape_frequency1 = 2000;
    elfConfiguration[computer].tape_frequencyBorder = 3000;
    elfConfiguration[computer].tape_threshold8Bit = 10;
    elfConfiguration[computer].tape_threshold16Bit = 120;
    elfConfiguration[computer].tape_threshold24Bit = 1000;
    elfConfiguration[computer].tape_audioChannelLeft = true;
    elfConfiguration[computer].tape_dataChannelLeft = true;
    elfConfiguration[computer].ioConfiguration.tapeEfRun = -1;
    cassetteNumber = 0;

    wxXmlNode *child = node.GetChildren();
    
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_EF:
                if (!child->HasAttribute("type"))
                    elfConfiguration[computer].ioConfiguration.tapeEf = (int)parseXml_Number(*child);
                else
                {
                    if (child->GetAttribute("type") == "out")
                        elfConfiguration[computer].ioConfiguration.tapeEfOut = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "in")
                        elfConfiguration[computer].ioConfiguration.tapeEf = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "run")
                        elfConfiguration[computer].ioConfiguration.tapeEfRun = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "error")
                        elfConfiguration[computer].ioConfiguration.tapeEfError = (int)parseXml_Number(*child);
                }
            break;

            case TAG_IN:
                if (child->HasAttribute("mode"))
                    elfConfiguration[computer].ioConfiguration.tapeMode = (int)parseXml_Number(*child, "mode");

                elfConfiguration[computer].ioConfiguration.tapeIn = parseXml_IoPort(*child);
            break;

            case TAG_OUT:
                if (child->HasAttribute("type"))
                {
                    if (child->GetAttribute("type") == "mode")
                        elfConfiguration[computer].ioConfiguration.tapeOutMode = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "sound")
                    {
                        elfConfiguration[computer].ioConfiguration.tapeOutSound = (int)parseXml_Number(*child);
                        conf[computer].soundType_ = SOUND_Q_SW;
                    }
                    if (child->GetAttribute("type") == "q")
                        elfConfiguration[computer].ioConfiguration.tapeQOut = parseXml_IoPort(*child);
                }
                else
                {
                    elfConfiguration[computer].ioConfiguration.tapeOut = parseXml_IoPort(*child);
                }
            break;

            case TAG_FREQ:
                if (child->GetAttribute("type") == "0")
                    elfConfiguration[computer].tape_frequency0 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "1")
                    elfConfiguration[computer].tape_frequency1 = (int)parseXml_Number(*child);
                elfConfiguration[computer].tape_frequencyBorder = (elfConfiguration[computer].tape_frequency1 + elfConfiguration[computer].tape_frequency0) / 2;
            break;

            case TAG_THRESHOLD:
                if (child->GetAttribute("type") == "8bit")
                    elfConfiguration[computer].tape_threshold8Bit = (char)parseXml_Number(*child);
                if (child->GetAttribute("type") == "16bit")
                    elfConfiguration[computer].tape_threshold16Bit  = (wxInt16)parseXml_Number(*child);
                if (child->GetAttribute("type") == "24bit")
                    elfConfiguration[computer].tape_threshold24Bit  = (wxInt32)parseXml_Number(*child);
            break;

            case TAG_STARTBIT:
                elfConfiguration[computer].tape_startBit = (int)parseXml_Number(*child);
            break;

            case TAG_DATABITS:
                elfConfiguration[computer].tape_dataBits = (int)parseXml_Number(*child);
            break;

            case TAG_STOPBIT:
                elfConfiguration[computer].tape_stopBit = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "ignore")
                    elfConfiguration[computer].tape_stopBitIgnore = true;
            break;

            case TAG_STOPDELAY:
                elfConfiguration[computer].tape_stopDelay = (int)parseXml_Number(*child);
            break;

            case TAG_ENDTAPEDELAY:
                elfConfiguration[computer].end_tape_delay = (int)parseXml_Number(*child);
            break;

            case TAG_STOPTONE:
                elfConfiguration[computer].stopTone = true;
            break;
            
            case TAG_BOOTSTART:
                elfConfiguration[computer].tapeStart = true;
            break;
            
            case TAG_FILENAME:
                if (child->GetAttribute("cassette") == "1")
                    cassetteNumber = 1;
                conf[computer].wavFile_[cassetteNumber] = child->GetNodeContent();
                wavFileDefined[cassetteNumber] = true;
            break;

            case TAG_DIRNAME:
                if (child->GetAttribute("cassette") == "1")
                    cassetteNumber = 1;
                wavFileDirDefined[cassetteNumber] = true;
                conf[computer].wavFileDir_[cassetteNumber] = dataDir_ + child->GetNodeContent();
                if (conf[computer].wavFileDir_[cassetteNumber].Right(1) != pathSeparator_)
                    conf[computer].wavFileDir_[cassetteNumber] += pathSeparator_;
            break;

            case TAG_CHANNEL:
                if (child->GetAttribute("type") == "audio")
                {
                    if (child->GetNodeContent() == "right")
                        elfConfiguration[computer].tape_audioChannelLeft = false;
                }
                if (child->GetAttribute("type") == "data")
                {
                    if (child->GetNodeContent() == "right")
                        elfConfiguration[computer].tape_dataChannelLeft = false;
                }
            break;

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.tapeIoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Xmodem (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "filename",
        "dirname",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_FILENAME:
                xmodemFileDefined_ = true;
                conf[computer].xmodemFile_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                xmodemFileDirDefined_ = true;
                conf[computer].xmodemFileDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].xmodemFileDir_.Right(1) != pathSeparator_)
                    conf[computer].xmodemFileDir_ += pathSeparator_;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_KeyFile (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "filename",
        "dirname",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_FILENAME:
                keyFileDefined_ = true;
                conf[computer].keyFile_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                keyFileDirDefined_ = true;
                conf[computer].keyFileDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].keyFileDir_.Right(1) != pathSeparator_)
                    conf[computer].keyFileDir_ += pathSeparator_;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Splash (int computer, wxXmlNode &node)
{
    conf[computer].useSplashScreen_ = true;

    wxString tagList[]=
    {
        "dialog",
        "text",
        "mac",
        "windows",
        "linux",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_DIALOG,
        TAG_TEXT,
        TAG_MAC,
        TAG_WINDOWS,
        TAG_LINUX,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    conf[computer].splashDialog_ = "DEFAULT";
    
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_DIALOG:
                conf[computer].splashDialog_ = child->GetNodeContent();
            break;
                
            case TAG_TEXT:
                conf[computer].splashText_ = child->GetNodeContent();
                conf[computer].splashText_.Replace("\r", "\n");
                conf[computer].splashText_.Replace("&","&&");
            break;
                
            case TAG_MAC:
#ifdef __WXMAC__
                conf[computer].splashText_ = child->GetNodeContent();
                conf[computer].splashText_.Replace("\r", "\n");
                conf[computer].splashText_.Replace("&","&&");
#endif
            break;

            case TAG_WINDOWS:
#ifdef __WXMSW__
                conf[computer].splashText_ = child->GetNodeContent();
                conf[computer].splashText_.Replace("\r", "\n");
                conf[computer].splashText_.Replace("&","&&");
#endif
            break;

            case TAG_LINUX:
#ifdef __linux__
                conf[computer].splashText_ = child->GetNodeContent();
                conf[computer].splashText_.Replace("\r", "\n");
                conf[computer].splashText_.Replace("&","&&");
#endif
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_VideoDump (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "filename",
        "dirname",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_FILENAME:
                videoDumpFileDefined_ = true;
                conf[computer].screenDumpFile_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                videoDumpDirDefined_ = true;
                conf[computer].screenDumpFileDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].screenDumpFileDir_.Right(1) != pathSeparator_)
                    conf[computer].screenDumpFileDir_ += pathSeparator_;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_MemAccess (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "dirname",
        "location",
        "start",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_DIRNAME,
        TAG_LOCATION,
        TAG_START,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_DIRNAME:
                memAccessDirDefined_ = true;
                conf[computer].ramDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].ramDir_.Right(1) != pathSeparator_)
                    conf[computer].ramDir_ += pathSeparator_;
            break;

            case TAG_LOCATION:
                if (child->GetNodeContent() == "off")
                    conf[computer].useLoadLocation_ = false;
                if (child->GetNodeContent() == "on")
                    conf[computer].useLoadLocation_ = true;
            break;

            case TAG_START:
                conf[computer].saveStart_ = (int)parseXml_Number(*child);
                conf[computer].saveStartString_.Printf("%04X", conf[computer].saveStart_);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Gui (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "memaccess",
        "ledtimer",
        "auto",
        "turbo",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_MEMACCESS,
        TAG_LED_TIMER,
        TAG_AUTO,
        TAG_TURBO,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_MEMACCESS:
                parseXml_MemAccess (computer, *child);
            break;

            case TAG_LED_TIMER:
                conf[computer].ledTime_ = child->GetNodeContent();
            break;

            case TAG_AUTO:
                if (child->GetNodeContent() == "off")
                    conf[computer].autoCassetteLoad_ = false;
                else
                    conf[computer].autoCassetteLoad_ = true;
            break;

            case TAG_TURBO:
                if (child->GetNodeContent() == "off")
                    conf[computer].turbo_ = false;
                else
                    conf[computer].turbo_ = true;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Diag (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "checksum",
        "factory",
        "cassette",
        "in",
        "out",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_CHECKSUM,
        TAG_FACTORY,
        TAG_CASSETTE,
        TAG_IN,
        TAG_OUT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    conf[computer].useDiagnosticBoard_ = true;
    conf[computer].diagRomChecksum_ = 1;
    conf[computer].diagFactory_ = 1;
    conf[computer].diagCassetteCables_ = 0;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_CHECKSUM:
                if (child->GetNodeContent() == "off")
                    conf[computer].diagRomChecksum_ = 0;
            break;

            case TAG_FACTORY:
                if (child->GetNodeContent() == "no")
                    conf[computer].diagFactory_ = 0;
            break;

            case TAG_CASSETTE:
                if (child->GetNodeContent() == "disconnected")
                    conf[computer].diagCassetteCables_ = 1;
            break;

            case TAG_IN:
                if (child->GetAttribute("type") == "1")
                    elfConfiguration[computer].ioConfiguration.diagIn1 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "2")
                    elfConfiguration[computer].ioConfiguration.diagIn2 = (int)parseXml_Number(*child);
            break;

            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.diagOut = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Debugger (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "scrt",
        "assembler",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_SCRT,
        TAG_ASSEMBLER,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_SCRT:
                conf[computer].scrtMode_ = (child->GetAttribute("mode") == "on");
                    parseXml_Scrt (computer, *child);
            break;

            case TAG_ASSEMBLER:
                parseXml_Assembler (computer, *child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Scrt (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "call",
        "return",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_CALL,
        TAG_RETURN,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_CALL:
                if (child->HasAttribute("reg"))
                    conf[computer].debugCallReg_ = (Byte)parseXml_Number(*child, "reg");
                conf[computer].debugCallAddress_ = (int)parseXml_Number(*child);
            break;

            case TAG_RETURN:
                if (child->HasAttribute("reg"))
                    conf[computer].debugRetReg_ = (Byte)parseXml_Number(*child, "reg");
                conf[computer].debugRetAddress_ = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Assembler (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "filename",
        "dirname",
        "code_start",
        "code_end",
        "end",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_CODE_START,
        TAG_CODE_END,
        TAG_END,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    
    AssemblerInfo assemblerInfo;
    
    assemblerInfo.dir = conf[computer].mainDir_;
    assemblerInfo.dirDefined = false;
    assemblerInfo.file = "";
    assemblerInfo.fileDefined = false;
    assemblerInfo.code_start = -1;
    assemblerInfo.code_end = -1;
    assemblerInfo.end = -1;
    
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_FILENAME:
                assemblerInfo.fileDefined = true;
                assemblerInfo.file = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                assemblerInfo.dirDefined = true;
                assemblerInfo.dir = dataDir_ + child->GetNodeContent();
                if (assemblerInfo.dir.Right(1) != pathSeparator_)
                    assemblerInfo.dir += pathSeparator_;
            break;

            case TAG_CODE_START:
                assemblerInfo.code_start = (int)parseXml_Number(*child);
            break;

            case TAG_CODE_END:
                assemblerInfo.code_end = (int)parseXml_Number(*child);
                if (assemblerInfo.end == -1)
                    assemblerInfo.end = assemblerInfo.code_end;
            break;

            case TAG_END:
                assemblerInfo.end = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        child = child->GetNext();
    }
    conf[computer].assemblerInfo_.push_back(assemblerInfo);
}

void XmlParser::parseXml_BatchWav (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "command",
        "dirname",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_SAVE,
        TAG_DIRNAME,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    conf[computer].useBatchWav_ = true;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_SAVE:
                conf[computer].saveCommand_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                batchFileDirDefined_ = true;
                conf[computer].batchFileDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].batchFileDir_.Right(1) != pathSeparator_)
                    conf[computer].batchFileDir_ += pathSeparator_;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_QSound (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "hw",
        "sw",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_HW,
        TAG_SW,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    conf[computer].beepFrequency_ = 250;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_HW:
                conf[computer].soundType_ = SOUND_EXT_BEEPER;
                conf[computer].beepFrequency_ = (int)parseXml_Number(*child);
            break;

            case TAG_SW:
                conf[computer].soundType_ = SOUND_Q_SW;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_OutBitSound (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "out",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    elfConfiguration[computer].ioConfiguration.bitSoundIoGroup = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                elfConfiguration[computer].useBitSound = true;
                
                elfConfiguration[computer].ioConfiguration.bitSoundOut.qValue = -1;
                elfConfiguration[computer].ioConfiguration.bitSoundOut.portNumber = (int)parseXml_Number(*child);
                if (child->GetAttribute("q") == "1")
                    elfConfiguration[computer].ioConfiguration.bitSoundOut.qValue = 1;
                if (child->GetAttribute("q") == "0")
                    elfConfiguration[computer].ioConfiguration.bitSoundOut.qValue = 0;
             
                elfConfiguration[computer].ioConfiguration.bitSoundMask = 1;
                elfConfiguration[computer].ioConfiguration.bitSoundMask = (int)parseXml_Number(*child, "mask");
            break;

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.bitSoundIoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Cdp1863Sound (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "out",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    elfConfiguration[computer].ioConfiguration.cdp1863IoGroup = -1;
    elfConfiguration[computer].ioConfiguration.cdp1863toneSwitch1.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.cdp1863toneSwitch2.portNumber = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                conf[computer].soundType_ = SOUND_1863_1864;

                if (child->GetAttribute("type") == "switch")
                {
                    if (elfConfiguration[computer].ioConfiguration.cdp1863toneSwitch1.portNumber != -1)
                    {
                        conf[computer].soundType_ = SOUND_1863_NOQ;
                        elfConfiguration[computer].ioConfiguration.cdp1863toneSwitch2.qValue = -1;
                        elfConfiguration[computer].ioConfiguration.cdp1863toneSwitch2.portNumber = (int)parseXml_Number(*child);
                        if (child->GetAttribute("q") == "1")
                            elfConfiguration[computer].ioConfiguration.cdp1863toneSwitch2.qValue = 1;
                        if (child->GetAttribute("q") == "0")
                            elfConfiguration[computer].ioConfiguration.cdp1863toneSwitch2.qValue = 0;
                    }
                    else
                    {
                        conf[computer].soundType_ = SOUND_1863_NOQ;
                        elfConfiguration[computer].ioConfiguration.cdp1863toneSwitch1.qValue = -1;
                        elfConfiguration[computer].ioConfiguration.cdp1863toneSwitch1.portNumber = (int)parseXml_Number(*child);
                        if (child->GetAttribute("q") == "1")
                            elfConfiguration[computer].ioConfiguration.cdp1863toneSwitch1.qValue = 1;
                        if (child->GetAttribute("q") == "0")
                            elfConfiguration[computer].ioConfiguration.cdp1863toneSwitch1.qValue = 0;
                    }
                }
                else
                {
                    elfConfiguration[computer].ioConfiguration.cdp1863toneLatch.qValue = -1;
                    elfConfiguration[computer].ioConfiguration.cdp1863toneLatch.portNumber = (int)parseXml_Number(*child);
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.cdp1863toneLatch.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.cdp1863toneLatch.qValue = 0;
                }
            break;

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.cdp1863IoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Dip (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].useDip = true;

    wxString tagList[]=
    {
        "in",
        "value",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_VALUE,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    
    elfConfiguration[computer].ioConfiguration.dipIn = 2;
    elfConfiguration[computer].ioConfiguration.dipValue = 0xf;
    
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.dipIn = (int)parseXml_Number(*child);
            break;

            case TAG_VALUE:
                elfConfiguration[computer].ioConfiguration.dipValue = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_IoGroup(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].useIoGroup = true;

    wxString tagList[]=
    {
        "out",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    
    elfConfiguration[computer].ioConfiguration.ioGroupOut = 1;
    elfConfiguration[computer].ioConfiguration.ioGroupMask = 0x1f;
    
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.ioGroupOut = (int)parseXml_Number(*child);
                if (child->HasAttribute("mask"))
                    elfConfiguration[computer].ioConfiguration.ioGroupMask = parseXml_Number(*child, "mask");
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Panel (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "in",
        "out",
        "ef",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_OUT,
        TAG_EF,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.hexOutput.portNumber = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.printerEf = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}
 
void XmlParser::parseXml_Memory(int computer, wxXmlNode &node)
{
    Word memMask;
    
    wxString tagList[]=
    {
        "rom",
        "diagrom",
        "ram",
        "nvram",
        "ems",
        "mapper",
        "giant",
        "copy",
        "rampart",
        "slot",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_ROM,
        TAG_DIAG,
        TAG_RAM,
        TAG_NVRAM,
        TAG_EMS,
        TAG_MAPPER,
        TAG_GIANT,
        TAG_COPY,
        TAG_RAMPART,
        TAG_SLOT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt, romRamButton;
    size_t configNumber;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_ROM:
                configNumber = getMemConfig(computer, child->GetAttribute("type"));
                setCartType(computer, configNumber, child->GetAttribute("cart"));
                parseXml_RomRam (computer, *child, (int)(ROM + 256*configNumber), configNumber);
                setMemMask(computer, configNumber, parseXml_Number(*child, "mask"));
            break;

            case TAG_DIAG:
                if (child->GetAttribute("type") == "on")
                    conf[computer].diagRomOn_ = 1;
                else
                    conf[computer].diagRomOn_ = 0;
                conf[computer].memConfig_.resize(conf[computer].memConfigNumber_+1);
                parseXml_RomRam (computer, *child, (int)(DIAGROM + 256*conf[computer].memConfigNumber_), conf[computer].memConfigNumber_);
                conf[computer].memConfig_[conf[computer].memConfigNumber_].useMemMask = false;
                conf[computer].memConfigNumber_++;
            break;

            case TAG_RAM:
                configNumber = getMemConfig(computer, child->GetAttribute("type"));
                setCartType(computer, configNumber, child->GetAttribute("cart"));
                parseXml_RomRam (computer, *child, (int)(RAM + 256*configNumber), configNumber);
                setMemMask(computer, configNumber, parseXml_Number(*child, "mask"));
            break;

            case TAG_NVRAM:
                elfConfiguration[computer].useNvRam = true;
                if (child->GetAttribute("default") == "off")
                    elfConfiguration[computer].nvRamDisable = true;
                
                configNumber = getMemConfig(computer, child->GetAttribute("type"));
                setCartType(computer, configNumber, child->GetAttribute("cart"));
                parseXml_RomRam (computer, *child, (int)(NVRAM + 256*configNumber), configNumber);
                setMemMask(computer, configNumber, parseXml_Number(*child, "mask"));
            break;

            case TAG_EMS:
                conf[computer].emsConfig_.resize(conf[computer].emsConfigNumber_+1);
                elfConfiguration[computer].ioConfiguration.emsOutput.resize(conf[computer].emsConfigNumber_+1);

                conf[computer].emsConfig_[conf[computer].emsConfigNumber_].emsType = RAM;

                if (child->GetAttribute("type") == "ROM")
                    conf[computer].emsConfig_[conf[computer].emsConfigNumber_].emsType = ROM;
                
                if (child->GetAttribute("type") == "multicart")
                {
                    conf[computer].emsConfig_[conf[computer].emsConfigNumber_].emsType = ROM;
                    conf[computer].multicartEmsNumber_ = (int)conf[computer].emsConfigNumber_;
                }

                elfConfiguration[computer].useEms = true;
                parseXml_Ems (computer, *child, (int)(EMSMEMORY + 256*conf[computer].emsConfigNumber_), conf[computer].emsConfigNumber_);
				conf[computer].emsConfigNumber_++;
            break;

            case TAG_MAPPER:
                conf[computer].memConfig_.resize(conf[computer].memConfigNumber_+1);
                elfConfiguration[computer].usePager = true;
                elfConfiguration[computer].usePortExtender = true;
                
                memMask = parseXml_Number(*child, "mask");
                if (memMask != 0)
                    memMask = 0xffff;
                memMask  |= 0xff;

                conf[computer].pagerMaskBits_ = 16;
                conf[computer].pagerMask_ = 0xFFFF;
                while ((memMask & 0x8000) == 0)
                {
                    conf[computer].pagerMaskBits_--;
                    conf[computer].pagerMask_ = conf[computer].pagerMask_ >> 1;
                    memMask = memMask << 1;
                }

                parseXml_portExt (computer, *child, PAGER, conf[computer].memConfigNumber_);
                conf[computer].memConfigNumber_++;
            break;

            case TAG_GIANT:
                elfConfiguration[computer].giantBoardMapping = true;
            break;

            case TAG_COPY:
                conf[computer].copyConfig_.resize(++conf[computer].copyConfigNumber_);
                conf[computer].copyConfig_[conf[computer].copyConfigNumber_-1].start = parseXml_Number(*child, "start");
                conf[computer].copyConfig_[conf[computer].copyConfigNumber_-1].end = parseXml_Number(*child, "end");
                conf[computer].copyConfig_[conf[computer].copyConfigNumber_-1].slot = (int)parseXml_Number(*child, "slot");
                conf[computer].copyConfig_[conf[computer].copyConfigNumber_-1].copy = (int)parseXml_Number(*child);
            break;

            case TAG_RAMPART:
                conf[computer].ramPartConfig_.resize(++conf[computer].ramPartConfigNumber_);
                conf[computer].ramPartConfig_[conf[computer].ramPartConfigNumber_-1].start = parseXml_Number(*child, "start");
                conf[computer].ramPartConfig_[conf[computer].ramPartConfigNumber_-1].end = parseXml_Number(*child, "end");
            break;

            case TAG_SLOT:
                parseXml_Slot (computer, *child, (int)parseXml_Number(*child, "max"));
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::setMemMask(int computer, size_t configNumber, long mask)
{
    conf[computer].memConfig_[configNumber].memMask = mask;
    if (conf[computer].memConfig_[configNumber].memMask != 0)
    {
        conf[computer].memConfig_[configNumber].useMemMask = true;
        conf[computer].memConfig_[configNumber].memMask |= 0xff;
    }
    else
        conf[computer].memConfig_[configNumber].useMemMask = false;
}

size_t XmlParser::getMemConfig(int computer, wxString type)
{
    size_t configNumber;

    if (type == "gui")
    {
        configNumber = 0;
        if ((conf[computer].memConfig_[configNumber].type & 0xff) != UNDEFINED)
            configNumber = 1;
    }
    else
    {
        configNumber = conf[computer].memConfigNumber_++;
        conf[computer].memConfig_.resize(configNumber+1);
    }
    
    return configNumber;
}

void XmlParser::setCartType(int computer, size_t configNumber, wxString type)
{
    if (type == "st2")
        conf[computer].memConfig_[configNumber].cartType = CART_ST2;

    if (type == "ch8")
        conf[computer].memConfig_[configNumber].cartType = CART_CH8;
}

void XmlParser::parseXml_RomRam(int computer, wxXmlNode &node, int type, size_t configNumber)
{
    wxString tagList[]=
    {
        "start",
        "end",
        "filename",
        "dump",
        "dirname",
        "pulldown",
        "mp",
        "map",
        "load_offset",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_START,
        TAG_END,
        TAG_FILENAME,
        TAG_DUMP,
        TAG_DIRNAME,
        TAG_PULLDOWN,
        TAG_MP,
        TAG_MAP,
        TAG_LOAD_OFFSET,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString addressMap, pulldownString;
    Word address;

    conf[computer].memConfig_[configNumber].start = 0;
    conf[computer].memConfig_[configNumber].end = 0;
    conf[computer].memConfig_[configNumber].memMask = 0;
    if (configNumber != 0 && configNumber != 1)
        conf[computer].memConfig_[configNumber].dirname = conf[computer].mainDir_ ;
    conf[computer].memConfig_[configNumber].filename = "";
    conf[computer].memConfig_[configNumber].filename2 = "";
    conf[computer].memConfig_[configNumber].type = type;
    conf[computer].memConfig_[configNumber].loadOffSet.addressStart = 0;
    conf[computer].memConfig_[configNumber].loadOffSet.offSet = 0;

    if ((type & 0xff) == NVRAM)
        conf[computer].memConfig_[configNumber].dumpFilename = "ramdump.bin";

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_START:
                conf[computer].memConfig_[configNumber].start = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                conf[computer].memConfig_[configNumber].end = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_FILENAME:
                if (conf[computer].memConfig_[configNumber].filename != "")
                    conf[computer].memConfig_[configNumber].filename2 = child->GetNodeContent();
                else
                    conf[computer].memConfig_[configNumber].filename = child->GetNodeContent();
                
                conf[computer].memConfig_[configNumber].verifyFileExist = (child->GetAttribute("verify") == "true");
            break;

            case TAG_DUMP:
                conf[computer].memConfig_[configNumber].dumpFilename = child->GetNodeContent();
            break;

           case TAG_DIRNAME:
                if (configNumber == 0 || configNumber == 1)
                    memDirDefined[configNumber] = true;
                conf[computer].memConfig_[configNumber].dirname =  dataDir_ + child->GetNodeContent();
                if (conf[computer].memConfig_[configNumber].dirname.Right(1) != pathSeparator_)
                {
                    conf[computer].memConfig_[configNumber].dirname += pathSeparator_;
                }
            break;

            case TAG_PULLDOWN:
                if (configNumber == 0 || configNumber == 1)
                    memDirDefined[configNumber] = true;
                pulldownString = dataDir_ + child->GetNodeContent();
                if (pulldownString.Right(1) != pathSeparator_)
                    pulldownString += pathSeparator_;
                conf[computer].memConfig_[configNumber].pulldownDir.Add(pulldownString);
                conf[computer].memConfig_[configNumber].dirname = pulldownString;
                
                pulldownString = "";
                if (child->HasAttribute("mask"))
                    pulldownString = child->GetAttribute("mask");
                conf[computer].memConfig_[configNumber].pulldownMask.Add(pulldownString);

                pulldownString = "";
                if (child->HasAttribute("exclude"))
                    pulldownString = child->GetAttribute("exclude");
                if (child->HasAttribute("exclude1"))
                    pulldownString = child->GetAttribute("exclude1");
                conf[computer].memConfig_[configNumber].pulldownExclude.Add(pulldownString);

                pulldownString = "";
                if (child->HasAttribute("exclude2"))
                    pulldownString = child->GetAttribute("exclude2");
                conf[computer].memConfig_[configNumber].pulldownExclude2.Add(pulldownString);
            break;

            case TAG_MP:
                if (node.GetName() == "nvram")
                    parseXml_nvRamMp(computer, *child);
            break;

            case TAG_MAP:
                addressMap = child->GetNodeContent();
                while (addressMap != "")
                {
                    address = getNextHexDec(&addressMap);
                    if (address != 0)
                        conf[computer].memConfig_[configNumber].mappingList.push_back(address);
                }
            break;
              
            case TAG_LOAD_OFFSET:
                conf[computer].memConfig_[configNumber].loadOffSet.addressStart = parseXml_Number(*child, "address") & 0xffff;
                conf[computer].memConfig_[configNumber].loadOffSet.offSet = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Ems(int computer, wxXmlNode &node, int type, size_t configNumber)
{
    long start, end;
    
    wxString tagList[]=
    {
        "start",
        "end",
        "start2",
        "end2",
        "start3",
        "end3",
        "mask",
        "filename",
        "dirname",
        "out",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_START,
        TAG_END,
        TAG_START2,
        TAG_END2,
        TAG_START3,
        TAG_END3,
        TAG_MASK,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_OUT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    conf[computer].emsConfig_[configNumber].start = 0;
    conf[computer].emsConfig_[configNumber].end = 0;
    conf[computer].emsConfig_[configNumber].start2 = 0;
    conf[computer].emsConfig_[configNumber].end2 = 0;
    conf[computer].emsConfig_[configNumber].start3 = 0;
    conf[computer].emsConfig_[configNumber].end3 = 0;
    conf[computer].emsConfig_[configNumber].outputStart = 0;
    conf[computer].emsConfig_[configNumber].outputEnd = 0;
    conf[computer].emsConfig_[configNumber].outputMask = 0xFF;
    conf[computer].emsConfig_[configNumber].filename = "";
    conf[computer].emsConfig_[configNumber].dirname = conf[computer].mainDir_ ;
    conf[computer].emsConfig_[configNumber].type = type;
    elfConfiguration[computer].ioConfiguration.emsOutput[configNumber] = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_START:
                conf[computer].emsConfig_[configNumber].start = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                conf[computer].emsConfig_[configNumber].end = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_START2:
                conf[computer].emsConfig_[configNumber].start2 = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END2:
                conf[computer].emsConfig_[configNumber].end2 = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_START3:
                conf[computer].emsConfig_[configNumber].start3 = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END3:
                conf[computer].emsConfig_[configNumber].end3 = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_FILENAME:
                conf[computer].emsConfig_[configNumber].filename = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                conf[computer].emsConfig_[configNumber].dirname  = dataDir_ + child->GetNodeContent();
                if (conf[computer].emsConfig_[configNumber].dirname.Right(1) != pathSeparator_)
                {
                    conf[computer].emsConfig_[configNumber].dirname += pathSeparator_;
                }
            break;

            case TAG_OUT:
                if (!parseXml_Range(*child, &start, &end))
                    elfConfiguration[computer].ioConfiguration.emsOutput[configNumber] = start & 0x7;
                else
                {
                    conf[computer].emsConfig_[configNumber].outputStart = (Word) start;
                    conf[computer].emsConfig_[configNumber].outputEnd = (Word) end;
                }
                conf[computer].emsConfig_[configNumber].outputMask = parseXml_Number(*child, "mask");
                if (conf[computer].emsConfig_[configNumber].outputMask == 0)
                    conf[computer].emsConfig_[configNumber].outputMask = 0xff;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Slot(int computer, wxXmlNode &node, int maxSlots)
{
    if (maxSlots <= 0)
        maxSlots = 1;

    size_t bitNumber = maxSlots;
    conf[computer].slotConfig_.slotOutputMask = 1;
    while (--bitNumber > 0)
        conf[computer].slotConfig_.slotOutputMask = 1 + (conf[computer].slotConfig_.slotOutputMask << 1);

    wxString tagList[]=
    {
        "start",
        "end",
        "out",
        "slotshift",
        "bankshift",
        "rom",
        "ram",
        "iogroup",
        "bank",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_START,
        TAG_END,
        TAG_OUT,
        TAG_SLOTSHIFT,
        TAG_BANKSHIFT,
        TAG_ROM,
        TAG_RAM,
        TAG_IOGROUP,
        TAG_BANK,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    Word mask;
    
    conf[computer].slotConfig_.maxSlotNumber_ = maxSlots;
    conf[computer].slotConfig_.slotInfo.resize(maxSlots);
    conf[computer].slotConfig_.useIoGroup = false;
    conf[computer].slotConfig_.slotOutputNumber = false;
    conf[computer].slotConfig_.bankOutputNumber = true;
    conf[computer].slotConfig_.bankOutputMask = 0;

    for (int slot=0; slot<maxSlots; slot++)
    {
        conf[computer].slotConfig_.slotInfo[slot].type = UNDEFINED;
        conf[computer].slotConfig_.slotInfo[slot].maxBankNumber_ = 1;
    }

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_START:
                conf[computer].slotConfig_.start = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                conf[computer].slotConfig_.end = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_OUT:
                conf[computer].slotConfig_.output = parseXml_Number(*child);
            break;

            case TAG_SLOTSHIFT:
                conf[computer].slotConfig_.slotOutputShift = (int)parseXml_Number(*child);
                conf[computer].slotConfig_.slotOutputNumber = (child->GetAttribute("type") == "number");
                if (conf[computer].slotConfig_.slotOutputNumber)
                {
                    if (maxSlots > 255)
                        maxSlots = 255;
                    
                    mask = maxSlots - 1;
                   
                    conf[computer].slotConfig_.slotOutputMask = 0xFF;
                    while ((mask & 0x80) == 0)
                    {
                        conf[computer].slotConfig_.slotOutputMask = conf[computer].slotConfig_.slotOutputMask >> 1;
                        mask = mask << 1;
                    }
                }
                else
                {
                    if (maxSlots > 8)
                    {
                        maxSlots = 8;
                    
                        conf[computer].slotConfig_.maxSlotNumber_ = maxSlots;
                        conf[computer].slotConfig_.slotInfo.resize(maxSlots);
                    }
                }
            break;

            case TAG_BANKSHIFT:
                conf[computer].slotConfig_.bankOutputShift = (int)parseXml_Number(*child);
                conf[computer].slotConfig_.bankOutputNumber = (child->GetAttribute("type") == "number");
                conf[computer].slotConfig_.bankOutputMask = (int)parseXml_Number(*child, "mask");
            break;

            case TAG_ROM:
                parseXml_SlotRomRam(computer, *child, (int)parseXml_Number(*child, "slot") , ROM);
            break;

            case TAG_RAM:
                parseXml_SlotRomRam(computer, *child, (int)parseXml_Number(*child, "slot"), RAM);
            break;

            case TAG_IOGROUP:
                conf[computer].slotConfig_.useIoGroup = true;
            break;

            case TAG_BANK:
                parseXml_Bank(computer, *child, (int)parseXml_Number(*child, "slot"), (int)parseXml_Number(*child, "max"));
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_Bank(int computer, wxXmlNode &node, int slot, int maxBanks)
{
    if (maxBanks <= 0)
        maxBanks = 1;

    if (conf[computer].slotConfig_.bankOutputNumber)
    {
        if (maxBanks > 255)
            maxBanks = 255;

        Word mask = maxBanks - 1;
        
        conf[computer].slotConfig_.slotInfo[slot].bankOutputMaskInSlot = 0xFF;
        while ((mask & 0x80) == 0)
        {
            conf[computer].slotConfig_.slotInfo[slot].bankOutputMaskInSlot = conf[computer].slotConfig_.slotInfo[slot].bankOutputMaskInSlot >> 1;
            mask = mask << 1;
        }
    }
    else
    {
        if (maxBanks > 8)
            maxBanks = 8;

        size_t bitNumber = maxBanks;
        conf[computer].slotConfig_.slotInfo[slot].bankOutputMaskInSlot = 1;
        while (--bitNumber > 0)
            conf[computer].slotConfig_.slotInfo[slot].bankOutputMaskInSlot = 1 + (conf[computer].slotConfig_.slotInfo[slot].bankOutputMaskInSlot << 1);
    }

    wxString tagList[]=
    {
        "out",
        "rom",
        "ram",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_ROM,
        TAG_RAM,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    
    conf[computer].slotConfig_.slotInfo[slot].maxBankNumber_ = maxBanks;
    conf[computer].slotConfig_.slotInfo[slot].bankInfo.resize(maxBanks);

    for (int bank=0; bank<maxBanks; bank++)
        conf[computer].slotConfig_.slotInfo[slot].bankInfo[bank].type = UNDEFINED;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_ROM:
                parseXml_BankRomRam(computer, *child, slot, (int)parseXml_Number(*child, "bank") , ROM);
            break;

            case TAG_RAM:
                parseXml_BankRomRam(computer, *child, slot, (int)parseXml_Number(*child, "bank"), RAM);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_SlotRomRam(int computer, wxXmlNode &node, int slot, int type)
{
    if (slot >= conf[computer].slotConfig_.maxSlotNumber_)
        return;
    
    conf[computer].slotConfig_.slotInfo[slot].type = type;
    
    wxString tagList[]=
    {
        "filename",
        "dirname",
        "ramstart",
        "ramend",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_START,
        TAG_END,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    
    conf[computer].slotConfig_.slotInfo[slot].filename = "";
    conf[computer].slotConfig_.slotInfo[slot].dirname = conf[computer].mainDir_ ;
    for (int ramPart = 0; ramPart<4; ramPart++)
    {
        conf[computer].slotConfig_.slotInfo[slot].ramStart[ramPart] = 0;
        conf[computer].slotConfig_.slotInfo[slot].ramEnd[ramPart] = 0;
    }

    int ramPart;
    
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_FILENAME:
                conf[computer].slotConfig_.slotInfo[slot].filename = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                conf[computer].slotConfig_.slotInfo[slot].dirname  = dataDir_ + child->GetNodeContent();
                if (conf[computer].slotConfig_.slotInfo[slot].dirname.Right(1) != pathSeparator_)
                {
                    conf[computer].slotConfig_.slotInfo[slot].dirname += pathSeparator_;
                }
            break;

            case TAG_START:
                ramPart = 0;
                if (child->HasAttribute("number"))
                    ramPart = (int)parseXml_Number(*child, "number")&3;
                conf[computer].slotConfig_.slotInfo[slot].ramStart[ramPart] = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                ramPart = 0;
                if (child->HasAttribute("number"))
                    ramPart = (int)parseXml_Number(*child, "number")&3;
                conf[computer].slotConfig_.slotInfo[slot].ramEnd[ramPart] = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_BankRomRam(int computer, wxXmlNode &node, int slot, int bank, int type)
{
    if (slot >= conf[computer].slotConfig_.maxSlotNumber_)
        return;

    if (bank >= conf[computer].slotConfig_.slotInfo[slot].maxBankNumber_)
        return;

    conf[computer].slotConfig_.slotInfo[slot].bankInfo[bank].type = type;
    
    wxString tagList[]=
    {
        "filename",
        "dirname",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    
    conf[computer].slotConfig_.slotInfo[slot].bankInfo[bank].filename = "";
    conf[computer].slotConfig_.slotInfo[slot].bankInfo[bank].dirname = conf[computer].mainDir_ ;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_FILENAME:
                conf[computer].slotConfig_.slotInfo[slot].bankInfo[bank].filename = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                conf[computer].slotConfig_.slotInfo[slot].bankInfo[bank].dirname  = dataDir_ + child->GetNodeContent();
                if (conf[computer].slotConfig_.slotInfo[slot].bankInfo[bank].dirname.Right(1) != pathSeparator_)
                {
                    conf[computer].slotConfig_.slotInfo[slot].bankInfo[bank].dirname += pathSeparator_;
                }
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_portExt(int computer, wxXmlNode &node, int type, size_t configNumber)
{
    wxString tagList[]=
    {
        "out",
        "in",
        "start",
        "end",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_START,
        TAG_END,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    conf[computer].memConfig_[configNumber].start = 0;
    conf[computer].memConfig_[configNumber].end = 0;
    conf[computer].memConfig_[configNumber].type = type;

    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_START:
                conf[computer].memConfig_[configNumber].start = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                conf[computer].memConfig_[configNumber].end = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.portExtenderInput = (int)parseXml_Number(*child);
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "select")
                    elfConfiguration[computer].ioConfiguration.portExtenderSelectOutput = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "write")
                    elfConfiguration[computer].ioConfiguration.portExtenderWriteOutput = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_RtcM58T58(int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "iogroup",
        "control",
        "second",
        "minute",
        "hour",
        "day",
        "date",
        "month",
        "year",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IOGROUP,
        TAG_CONTROL,
        TAG_SECOND,
        TAG_MINUTE,
        TAG_HOUR,
        TAG_DAY,
        TAG_DATE,
        TAG_MONTH,
        TAG_YEAR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    elfConfiguration[computer].useRtcM48T58 = true;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.rtcIoGroup1 = (int)parseXml_Number(*child);
            break;

            case TAG_CONTROL:
                elfConfiguration[computer].ioConfiguration.rtcControl = (int)parseXml_Number(*child);
            break;

            case TAG_SECOND:
                elfConfiguration[computer].ioConfiguration.rtcSecond = (int)parseXml_Number(*child);
            break;

            case TAG_MINUTE:
                elfConfiguration[computer].ioConfiguration.rtcMinute = (int)parseXml_Number(*child);
            break;

            case TAG_HOUR:
                elfConfiguration[computer].ioConfiguration.rtcHour = (int)parseXml_Number(*child);
            break;

            case TAG_DAY:
                elfConfiguration[computer].ioConfiguration.rtcDay = (int)parseXml_Number(*child);
            break;

            case TAG_DATE:
                elfConfiguration[computer].ioConfiguration.rtcDate = (int)parseXml_Number(*child);
            break;

            case TAG_MONTH:
                elfConfiguration[computer].ioConfiguration.rtcMonth = (int)parseXml_Number(*child);
            break;

            case TAG_YEAR:
                elfConfiguration[computer].ioConfiguration.rtcYear = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_RtcDS12887(int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "iogroup",
        "in",
        "out",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IOGROUP,
        TAG_IN,
        TAG_OUT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString iogroup;
    
    elfConfiguration[computer].useRtcDS12887 = true;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                elfConfiguration[computer].ioConfiguration.rtcIoGroup1 = (int)getNextHexDec(&iogroup) & 0xff;
                if (iogroup != "")
                    elfConfiguration[computer].ioConfiguration.rtcIoGroup2 = (int)getNextHexDec(&iogroup) & 0xff;
            break;

            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.rtcIn = (int)parseXml_Number(*child);
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "port")
                    elfConfiguration[computer].ioConfiguration.rtcOut = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "select")
                    elfConfiguration[computer].ioConfiguration.rtcSelect = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}


void XmlParser::parseXml_UsbSuperBoard(int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "iogroup",
        "in",
        "out",
        "ef",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IOGROUP,
        TAG_IN,
        TAG_OUT,
        TAG_EF,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    elfConfiguration[computer].useUsbSb = true;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.usbSbIoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_IN:
                if (child->GetAttribute("type") == "ef")
                {
                    elfConfiguration[computer].ioConfiguration.usbSbIn6.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.usbSbIn6.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.usbSbIn6.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.usbSbIn6.qValue = 0;
                }
                if (child->GetAttribute("type") == "usb")
                {
                    elfConfiguration[computer].ioConfiguration.usbSbIn5.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.usbSbIn5.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.usbSbIn5.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.usbSbIn5.qValue = 0;
                }
            break;

            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.usbSbOut.portNumber = (int)parseXml_Number(*child);
                elfConfiguration[computer].ioConfiguration.usbSbOut.qValue = -1;
                if (child->GetAttribute("q") == "1")
                    elfConfiguration[computer].ioConfiguration.usbSbOut.qValue = 1;
                if (child->GetAttribute("q") == "0")
                    elfConfiguration[computer].ioConfiguration.usbSbOut.qValue = 0;
            break;

            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.usbSbEf = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void XmlParser::parseXml_nvRamMp(int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "iogroup",
        "in",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IOGROUP,
        TAG_IN,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    elfConfiguration[computer].useNvRamMp = true;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.nvRamIoGroup = (int)parseXml_Number(*child);
            break;

            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.nvRamProtectIn = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

IoPort XmlParser::parseXml_IoPort(wxXmlNode &node)
{
    IoPort ioPort;
    
    ioPort.portNumber = (int)parseXml_Number(node);
    ioPort.qValue = -1;
    if (node.GetAttribute("q") == "1")
        ioPort.qValue = 1;
    if (node.GetAttribute("q") == "0")
        ioPort.qValue = 0;

    ioPort.mask = 0xff;
    if (node.HasAttribute("mask"))
        ioPort.mask = (Byte)parseXml_Number(node, "mask");

    ioPort.xorMask = 0;
    if (node.HasAttribute("xor"))
        ioPort.xorMask = (Byte)parseXml_Number(node, "xor");

    return ioPort;
}

IoPort XmlParser::init_IoPort()
{
    IoPort ioPort;

    ioPort.portNumber = -1;
    ioPort.qValue = -1;
    ioPort.mask = 0xff;

    return ioPort;
}

long XmlParser::parseXml_Number(wxXmlNode &node)
{
    return getHexDec(node.GetNodeContent());
}

void XmlParser::parseXml_Mc6857BitSetup(int computer, wxXmlNode &node,  int bitNumber, wxString childName)
{
    wxString bitList[]=
    {
        "",
        "b0",
        "b1",
        "b2",
        "b3",
        "b4",
        "b5",
        "b6",
        "b7",
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "dd6",
        "dd7",
        "dd8",
        "dd9",
        "dd10",
        "dd11",
        "comment",
        "undefined"
    };
    
    enum
    {
        BIT_NONE,
        BIT_B0,
        BIT_B1,
        BIT_B2,
        BIT_B3,
        BIT_B4,
        BIT_B5,
        BIT_B6,
        BIT_B7,
        BIT_0,
        BIT_1,
        BIT_2,
        BIT_3,
        BIT_4,
        BIT_5,
        BIT_6,
        BIT_7,
        BIT_DD6,
        BIT_DD7,
        BIT_DD8,
        BIT_DD9,
        BIT_DD10,
        BIT_DD11,
        TAG_COMMENT,
        BIT_UNDEFINED
    };

    int bitTypeInt = 0;

    wxString bitType = node.GetNodeContent();
    
    while (bitTypeInt != BIT_UNDEFINED && bitList[bitTypeInt] != bitType)
        bitTypeInt++;
    
    switch (bitTypeInt)
    {
        case BIT_NONE:
        break;

        case BIT_0:
        case BIT_B0:
        case BIT_DD8:
            elfConfiguration[computer].ioConfiguration.mc6847b0 =  bitNumber;
        break;

        case BIT_1:
        case BIT_B1:
        case BIT_DD9:
            elfConfiguration[computer].ioConfiguration.mc6847b1 =  bitNumber;
        break;

        case BIT_2:
        case BIT_B2:
        case BIT_DD10:
            elfConfiguration[computer].ioConfiguration.mc6847b2 =  bitNumber;
        break;

        case BIT_3:
        case BIT_B3:
        case BIT_DD11:
            elfConfiguration[computer].ioConfiguration.mc6847b3 =  bitNumber;
        break;

        case BIT_4:
        case BIT_B4:
            elfConfiguration[computer].ioConfiguration.mc6847b4 =  bitNumber;
        break;

        case BIT_5:
        case BIT_B5:
            elfConfiguration[computer].ioConfiguration.mc6847b5 =  bitNumber;
        break;

        case BIT_6:
        case BIT_B6:
            elfConfiguration[computer].ioConfiguration.mc6847b6 =  bitNumber;
        break;

        case BIT_7:
        case BIT_B7:
            elfConfiguration[computer].ioConfiguration.mc6847b7 =  bitNumber;
        break;

        case BIT_DD6:
            elfConfiguration[computer].ioConfiguration.mc6847dd6 =  bitNumber;
        break;

        case BIT_DD7:
            elfConfiguration[computer].ioConfiguration.mc6847dd7 =  bitNumber;
        break;
            
        case TAG_COMMENT:
        break;

        default:
            warningText_ += "Unkown mc6847 bit number";
            warningText_ += childName;
            warningText_ += "\n";
        break;
    }

}

long XmlParser::parseXml_Number(wxXmlNode &node, wxString attribute)
{
    return getHexDec(node.GetAttribute(attribute));
}

bool XmlParser::parseXml_Range(wxXmlNode &node, long *start, long *end)
{
    wxString numberString1, numberString2;

    numberString1 = node.GetNodeContent();
    int dash = numberString1.Find("-");
    int x = numberString1.Find("x");

    if (dash == wxNOT_FOUND && x == wxNOT_FOUND)
    {
        *start = parseXml_Number(node);
        return false;
    }
    
    if (dash == wxNOT_FOUND)
    {
        *start = parseXml_Number(node);
        *end = *start;
        return true;
    }
    numberString2 = numberString1.Right(numberString1.Len()-(dash+1));
    numberString1 = numberString1.Left(dash);
    
    *start = getHexDec(numberString1);
    *end = getHexDec(numberString2);

    return true;
}

long XmlParser::getHexDec(wxString numberString)
{
    int base;
    long number;

    if (numberString.Left(2) == "0x")
    {
        base = 16;
        numberString = numberString.Right(numberString.Len()-2);
    }
    else
        base = 10;
    
    if (!numberString.ToLong(&number, base))
        number = 0;

    return number;
}

long XmlParser::getNextHexDec(wxString *numberString)
{
    int base;
    long number;
    wxString nextNumberString;

    nextNumberString = *numberString;
    nextNumberString.Trim(false);
    numberString->Trim(false);
    
    if (nextNumberString.Left(2) == "0x")
    {
        base = 16;
        nextNumberString = nextNumberString.Right(nextNumberString.Len()-2);
        *numberString = nextNumberString;
    }
    else
        base = 10;
    
    int comma = nextNumberString.Find(",");
    if (comma == wxNOT_FOUND)
    {
        int cr = nextNumberString.Find("\n");
        if (cr == wxNOT_FOUND)
            *numberString = "";
        else
        {
            nextNumberString = nextNumberString.Left(cr);
            nextNumberString.Trim();
            *numberString = numberString->Right(numberString->Len()-(cr+1));
        }
    }
    else
    {
        nextNumberString = nextNumberString.Left(comma);
        nextNumberString.Trim();
        *numberString = numberString->Right(numberString->Len()-(comma+1));
    }
    
    if (!nextNumberString.ToLong(&number, base))
        number = 0;

    return number;
}

bool XmlParser::parseXml_Size(wxXmlNode &node, long *width, long *height)
{
    wxString numberString1, numberString2;
    numberString1 = node.GetNodeContent();

    int x = numberString1.Find("x");
    if (x == wxNOT_FOUND)
        return false;

    numberString2 = numberString1.Right(numberString1.Len()-(x+1));
    numberString1 = numberString1.Left(x);

    *width = getHexDec(numberString1);
    *height = getHexDec(numberString2);

    return true;
}

wxString XmlParser::getDoubleString(wxString doubleString, wxString tag, double max, wxString maxStr)
{
    double floatValue;

    if (!toDouble(doubleString, (double*)&floatValue))
    {
        warningText_ += tag;
        warningText_ += " not specified in MHz\n";
        return "1.0";
    }
    if (floatValue == 0)
    {
        warningText_ += tag;
        warningText_ += " should not be 0\n";
        return "1.0";
    }
    if (floatValue >= max)
    {
        warningText_ += tag;
        warningText_ += " should be < ";
        warningText_ += maxStr;
        warningText_ += "\n";
        return "1.0";
    }
    return doubleString;
}

double XmlParser::getDouble(wxString doubleString, wxString tag, double max, wxString maxStr)
{
    double floatValue;

    if (!toDouble(doubleString, (double*)&floatValue))
    {
        warningText_ += tag;
        warningText_ += " not specified in MHz\n";
        return 1;
    }
    if (floatValue == 0)
    {
        warningText_ += tag;
        warningText_ += " should not be 0\n";
        return 1;
    }
    if (floatValue >= max)
    {
        warningText_ += tag;
        warningText_ += " should be < ";
        warningText_ += maxStr;
        warningText_ += "\n";
        return 1;
    }
    return floatValue;
}

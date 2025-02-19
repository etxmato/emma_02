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

BEGIN_EVENT_TABLE(XmlParser, GuiMain)

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
    "pause",
    "menu",
    "tab",
    "numpad_enter",
    "undefined"
};

int parserBaudRateValue_[] =
{
    38400, 19200, 9600, 4800, 3600, 2400, 2000, 1800, 1200, 600, 300, 200, 150, 134, 110, 75, 50
};

XmlParser::XmlParser(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiMain(title, pos, size, mode, dataDir, iniDir)
{
}

void XmlParser::parseXmlFile(wxString xmlDir, wxString xmlFile)
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
        "flipflop",
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
        "cdp1854",
        "cdp1855",
        "cdp1877",
        "cdp1878",
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
        TAG_FLIPFLOP,
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
        TAG_CDP1854,
        TAG_CDP1855,
        TAG_CDP1877,
        TAG_CDP1878,
        TAG_CD4536B,
        TAG_AD_CONVERTOR,
        TAG_DEBUGGER,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    if (!wxFile::Exists(xmlDir + xmlFile) || xmlFile == "" || xmlDir == "")
    {
        memConfigNumber_ = 2;
        computerConfiguration.memoryConfiguration.resize(memConfigNumber_);
        computerConfiguration.memoryConfiguration[0].filename = "";
        computerConfiguration.memoryConfiguration[1].filename = "";
        computerConfiguration.fdcType_ = FDCTYPE_17XX;
        computerConfiguration.frontPanelConfiguration.resize(1);
        return;
    }
        
    wxFileName xmlFileName = wxFileName(xmlDir + xmlFile);
    wxDateTime newDate = xmlFileName.GetModificationTime();
    
    if (oldXmlFileName_ == xmlDir + xmlFile)
    {
        if (newDate.IsEqualTo(oldXmlDate_))
            return;
    }

    memConfigNumber_ = 2;
    computerConfiguration.memoryConfiguration.resize(memConfigNumber_);
    computerConfiguration.memoryConfiguration[0].filename = "";
    computerConfiguration.memoryConfiguration[1].filename = "";
    computerConfiguration.fdcType_ = FDCTYPE_17XX;

    setCpuType("");
    
    computerInfo.ploadExtension = "";
    computerConfiguration.autoBootConfiguration.defined = false;
    computerConfiguration.f12reset = true;
    computerConfiguration.autoBootConfiguration.dmaOutOnBoot = false;
    computerConfiguration.autoBootConfiguration.dmaOutOnBootIfMem0is0 = false;
    computerConfiguration.autoBootConfiguration.address = 0;
    computerConfiguration.monitorConfiguration.defined = false;

    computerConfiguration.bootstrapConfiguration.defined = false;
    computerConfiguration.dmaConfiguration.defined = false;
    computerConfiguration.dmaInReset_defined = false;
    computerConfiguration.interruptConfiguration.defined = false;
    
    for (int io=0; io<LAST_IO_DEFINITION; io++)
        computerConfiguration.ioMask[io] = 0xff;

    computerConfiguration.vis1870Configuration.statusBarType = STATUSBAR_NONE;
    computerConfiguration.vis1870Configuration.statusBarLedOut = -1;

    computerConfiguration.keyPadDefinitionFile = "keydefinition.txt";

    computerConfiguration.memoryMask = 0xffff;
    
    computerConfiguration.memoryConfiguration[0].start = 0;
    computerConfiguration.memoryConfiguration[0].type = UNDEFINED;
    computerConfiguration.memoryConfiguration[0].cartType = CART_BIN;
    computerConfiguration.memoryConfiguration[1].start = 0;
    computerConfiguration.memoryConfiguration[1].type = UNDEFINED;
    computerConfiguration.memoryConfiguration[1].cartType = CART_NONE;
    computerConfiguration.slotConfiguration.maxSlotNumber_ = 0;
    computerConfiguration.slotConfiguration.banksInUse_ = false;
    computerConfiguration.slotConfiguration.slotInfo.clear();
    computerConfiguration.mcrConfiguration.mapInfo.clear();
    computerConfiguration.mcrConfiguration.maxMapNumber_ = 0;
    computerConfiguration.mcrConfiguration.output = init_IoPort();
    computerConfiguration.mcrConfiguration.bbat = init_IoPort();
    computerConfiguration.diagnosticBoardConfiguration.defined = false;
    computerConfiguration.useBatchWav_ = false;
    computerConfiguration.nvRamMpConfiguration.defined = false;
    computerConfiguration.nvRamMpConfiguration.followMpSwitch = false;
    computerConfiguration.nvRamConfiguration.defined = false;
//    computerConfiguration.nvRamDisableDefault = false;
    computerConfiguration.rtcCdp1879Configuration.defined = false;
    computerConfiguration.rtcM48t58Configuration.defined = false;
    computerConfiguration.rtcDs12887Configuration.defined = false;
    computerConfiguration.superBoardConfiguration.defined = false;
    computerConfiguration.memoryMapperConfiguration.defined = false;
    computerConfiguration.multicartEmsNumber_ = -1;
    computerConfiguration.fdcConfiguration.wd1793_defined = false;
    computerConfiguration.fdcConfiguration.wd1770_defined = false;
    computerConfiguration.upd765Configuration.defined = false;
    computerConfiguration.ideConfiguration.defined = false;

    computerConfiguration.numberOfVideoTypes_ = 0;
    computerConfiguration.coinConfiguration.defined = false;
    
    computerConfiguration.cdp1861Configuration.statusBarType = STATUSBAR_NONE;
    computerConfiguration.cdp1861Configuration.defined = false;
    computerConfiguration.cdp1861Configuration.efScreenOn = false;
    computerConfiguration.cdp1861Configuration.highRes = false;
    computerConfiguration.cdp1861Configuration.colorType = PIXIE_COLOR_DEFAULT;
    computerConfiguration.cdp1861Configuration.doubleScreenIo = PIXIE_IN_OUT;

    computerConfiguration.cdp1861Configuration.pixieGraphics.videoWidth = 64;
    computerConfiguration.cdp1862Configuration.colorType = PIXIE_COLOR_DEFAULT;
    computerConfiguration.cdp1862Configuration.defined = false;
    computerConfiguration.cdp1864Configuration.defined = false;
    computerConfiguration.cdp1864Configuration.screenOn = false;
    computerConfiguration.cdp1864Configuration.colorType = PIXIE_COLOR_DEFAULT;
    computerConfiguration.tu58Configuration.defined = false;

    computerConfiguration.studio4VideoConfiguration.defined = false;
    computerConfiguration.vip2KVideoConfiguration.defined = false;
    computerConfiguration.fredVideoConfiguration.defined = false;
    computerConfiguration.mc6845Configuration.defined = false;
    computerConfiguration.mc6847Configuration.defined = false;
    computerConfiguration.tmsConfiguration.defined = false;
    computerConfiguration.i8275Configuration.defined = false;
    computerConfiguration.sn76430NConfiguration.defined = false;
    computerConfiguration.vis1870Configuration.defined = false;
    computerConfiguration.vis1870Configuration.useVideoModeEf = false;
    computerConfiguration.vis1870Configuration.useBlockWrite = true;
    computerConfiguration.interlace_ = false;

    computerConfiguration.useHexKeyboard = false;
    for (int pad=0; pad<MAX_BITKEYPADS; pad++)
        computerConfiguration.bitKeypadConfiguration[pad].defined = false;

    computerConfiguration.matrixKeyboardConfiguration.defined = false;
    computerConfiguration.matrixKeyboardConfiguration.input = init_IoPort();
    computerConfiguration.matrixKeyboardConfiguration.output = init_IoPort();
    computerConfiguration.cdp1871Configuration.defined = false;
    computerConfiguration.asciiKeyboardConfiguration.defined = false;
    computerConfiguration.ps2KeyboardConfiguration.defined = false;
    computerConfiguration.ps2KeyboardConfiguration.interrupt = false;
    computerConfiguration.gpioPs2KeyboardConfiguration.defined = false;
    computerConfiguration.gpioPs2KeyboardConfiguration.interrupt = false;
    computerConfiguration.forceUpperCase = false;
    for (int pad=0; pad<MAX_LATCHKEYPADS; pad++)
    {
        computerConfiguration.keyLatchConfiguration[pad].defined = false;
        computerConfiguration.keyLatchConfiguration[pad].output = init_IoPort();
        computerConfiguration.keyLatchConfiguration[pad].diagonalKeys.clear();
    }
    computerConfiguration.fredKeypadConfiguration.defined= false;
    computerConfiguration.etiKeypadConfiguration.defined= false;
    computerConfiguration.adConvertorConfiguration.defined = false;
    computerConfiguration.ay_3_8912Configuration.defined = false;

    computerConfiguration.videoTerminalConfiguration.type = VTNONE;
    computerConfiguration.videoTerminalConfiguration.show = true;
    computerConfiguration.videoTerminalConfiguration.reverseQ = -1;
    computerConfiguration.videoTerminalConfiguration.efInterrupt = init_EfFlag();
    computerConfiguration.videoTerminalConfiguration.ef = init_EfFlag();
    computerConfiguration.videoTerminalConfiguration.ef.flagNumber = 4;
    computerConfiguration.videoTerminalConfiguration.external = false;
    computerConfiguration.videoTerminalConfiguration.loop_back = false;
    computerConfiguration.videoTerminalConfiguration.uart1854_defined = false;
    computerConfiguration.videoTerminalConfiguration.uart16450_defined = false;
    computerConfiguration.videoTerminalConfiguration.baudR = 4;
    computerConfiguration.videoTerminalConfiguration.baudT = 4;
    computerConfiguration.videoTerminalConfiguration.baudCorrectionR = 0.5;
    computerConfiguration.videoTerminalConfiguration.baudCorrectionT = 0.5;
    computerConfiguration.videoTerminalConfiguration.serialPort = "";
    
    computerConfiguration.autoCassetteLoad_ = true;
    computerConfiguration.turbo_ = true;
    computerConfiguration.ledTime_ = "100";
    computerConfiguration.memAccessConfiguration.useLocation = false;
    computerConfiguration.memAccessConfiguration.saveStart = 0;
    computerConfiguration.memAccessConfiguration.saveStartString = "0";
    computerConfiguration.characterRomConfiguration.fileName = "";
    computerConfiguration.batchConfiguration.fileName = "";

    computerConfiguration.keyFileConfiguration.fileName = "";
    computerConfiguration.printerFileConfiguration.fileName = "printerout.txt";
    computerConfiguration.screenDumpFileConfiguration.fileName = "screendump.png";
    computerConfiguration.videoTerminalConfiguration.xmodemFileName = "";

    computerConfiguration.vip2KVideoConfiguration.sequencerFile = "2716-ntsc.hex";
    computerConfiguration.useSplashScreen_ = false;
    computerConfiguration.debuggerConfiguration.videoLog_active = false;
    computerConfiguration.debuggerConfiguration.videoLog_defined = false;

    
    computerConfiguration.locationTriggerConfiguration.resize(65536);
    for (int address=0; address<65536; address++)
    {
        computerConfiguration.locationTriggerConfiguration[address].index.resize(1);
        computerConfiguration.locationTriggerConfiguration[address].index[0] = -1;
    }
    
    computerConfiguration.addressLocationConfiguration.locationInfo.clear();
    computerConfiguration.addressLocationConfiguration.keyInputAddress.clear();
    computerConfiguration.addressLocationConfiguration.writeAddress.clear();
    computerConfiguration.addressLocationConfiguration.inReleaseAddress.clear();
    computerConfiguration.assemblerConfiguration.clear();
    computerConfiguration.cdp1851Configuration.clear();
    computerConfiguration.cdp1852Configuration.clear();
    computerConfiguration.cdp1854Configuration.clear();
    computerConfiguration.cdp1877Configuration.clear();
    computerConfiguration.cdp1878Configuration.clear();
    computerConfiguration.cd4536bConfiguration.clear();
    computerConfiguration.memoryRamPartConfiguration.clear();
    computerConfiguration.memoryCopyConfiguration.clear();
    computerConfiguration.emsMemoryConfiguration.clear();
    computerConfiguration.memoryConfiguration[0].pulldownDir.Clear();
    computerConfiguration.memoryConfiguration[0].pulldownMask.Clear();
    computerConfiguration.memoryConfiguration[0].pulldownExclude.Clear();
    computerConfiguration.memoryConfiguration[0].pulldownExclude2.Clear();
    computerConfiguration.memoryConfiguration[1].pulldownDir.Clear();
    computerConfiguration.memoryConfiguration[1].pulldownMask.Clear();
    computerConfiguration.memoryConfiguration[1].pulldownExclude.Clear();
    computerConfiguration.memoryConfiguration[1].pulldownExclude2.Clear();

    computerConfiguration.videoTerminalConfiguration.vtCharRomFileName = "vt100.bin";
    computerConfiguration.videoTerminalConfiguration.wavFileName = "";

    computerConfiguration.addressLocationConfiguration.code_start = -1;
    computerConfiguration.addressLocationConfiguration.code_start_high = -1;
    computerConfiguration.addressLocationConfiguration.code_start_low = -1;
    computerConfiguration.addressLocationConfiguration.code_end_high = -1;
    computerConfiguration.addressLocationConfiguration.code_end_low = -1;
    computerConfiguration.addressLocationConfiguration.reg_end = -1;
    computerConfiguration.printerOn_ = false;
    computerConfiguration.basicPrinterConfiguration.defined = false;
    computerConfiguration.parallelPrinterConfiguration.defined = false;
    computerConfiguration.centronicsPrinterConfiguration.defined = false;
    computerConfiguration.serialPrinterConfiguration.defined = false;
    computerConfiguration.thermalPrinterConfiguration.defined = false;
    computerConfiguration.qSerialPrinterConfiguration.defined = false;
    computerConfiguration.cdp1855Configuration.defined = false;

    computerConfiguration.videoTerminalConfiguration.xModem_defined = false;
    computerConfiguration.videoTerminalConfiguration.hexModem_defined = false;
    computerConfiguration.swTapeConfiguration.defined = false;
    computerConfiguration.swTapeConfiguration.twoDecks = false;
    computerConfiguration.hwTapeConfiguration.defined = false;
    computerConfiguration.hwTapeConfiguration.twoDecks = false;
    computerConfiguration.hwTapeConfiguration.defined = false;

    computerConfiguration.soundConfiguration.type = SOUND_OFF;
    computerConfiguration.soundConfiguration.stereo = 1;
    computerConfiguration.soundConfiguration.beepFrequency = 250;

    computerConfiguration.efButtonsConfiguration.efButton.clear();
    computerConfiguration.efButtonsConfiguration.defined = false;
    computerConfiguration.ledDisplayConfiguration.defined = false;
    
    computerConfiguration.frontPanelConfiguration.resize(1);
    computerConfiguration.frontPanelConfiguration[PANEL_MAIN].defined = false;
    computerConfiguration.frontPanelConfiguration[PANEL_MAIN].show = false;
    computerConfiguration.frontPanelConfiguration[PANEL_MAIN].size.x = 0;
    computerConfiguration.frontPanelConfiguration[PANEL_MAIN].size.y = 0;
    
    computerConfiguration.mainFrontPanelConfiguration.nibbleLeds = false;
    computerConfiguration.mainFrontPanelConfiguration.dataSwitchButtons = false;
    computerConfiguration.mainFrontPanelConfiguration.dataPushButtons = false;
    computerConfiguration.mainFrontPanelConfiguration.powerButtonDefined = false;
    computerConfiguration.mainFrontPanelConfiguration.waitButtonDefined = false;
    computerConfiguration.mainFrontPanelConfiguration.xBorder = windowInfo.xBorder2;
    computerConfiguration.mainFrontPanelConfiguration.yBorder = windowInfo.yBorder2;

    computerConfiguration.giantBoardConfiguration.defined = false;
    computerConfiguration.bootstrapConfiguration.type = BOOTSTRAPNONE;

    computerConfiguration.hexDisplayConfiguration.output = init_IoPort();
    computerConfiguration.hexDisplayConfiguration.input = init_IoPort();
    computerConfiguration.hexDisplayConfiguration.defined = false;
    computerConfiguration.multiSegDisplayConfiguration.multiTilNumber = 0;
    computerConfiguration.multiSegDisplayConfiguration.tilFontFile = "";
    computerConfiguration.multiSegDisplayConfiguration.reversePolarity = false;
    computerConfiguration.multiSegDisplayConfiguration.multitilAddressLineWait = -1;
    computerConfiguration.multiSegDisplayConfiguration.multitilAddressLineHigh = -1;

    computerConfiguration.flipflopConfiguration.output = init_IoPort();
    computerConfiguration.flipflopConfiguration.ef = init_EfFlag();

    computerConfiguration.dipConfiguration.defined = false;
    computerConfiguration.ioGroupConfiguration.defined = false;

    computerConfiguration.cvKeypadConfiguration.defined = false;

    computerConfiguration.bitSoundConfiguration.defined = false;
    
    computerConfiguration.videoTerminalConfiguration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.videoTerminalConfiguration.defaultY = mainWindowY_;
    computerConfiguration.coinConfiguration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.coinConfiguration.defaultY = mainWindowY_;
    computerConfiguration.cdp1861Configuration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.cdp1861Configuration.defaultY = mainWindowY_;
    computerConfiguration.cdp1864Configuration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.cdp1864Configuration.defaultY = mainWindowY_;
    computerConfiguration.studio4VideoConfiguration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.studio4VideoConfiguration.defaultY = mainWindowY_;
    computerConfiguration.vip2KVideoConfiguration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.vip2KVideoConfiguration.defaultY = mainWindowY_;
    computerConfiguration.fredVideoConfiguration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.fredVideoConfiguration.defaultY = mainWindowY_;
    computerConfiguration.mc6845Configuration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.mc6845Configuration.defaultY = mainWindowY_;
    computerConfiguration.mc6847Configuration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.mc6847Configuration.defaultY = mainWindowY_;
    computerConfiguration.tmsConfiguration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.tmsConfiguration.defaultY = mainWindowY_;
    computerConfiguration.i8275Configuration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.i8275Configuration.defaultY = mainWindowY_;
    computerConfiguration.vis1870Configuration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.vis1870Configuration.defaultY = mainWindowY_;
    computerConfiguration.sn76430NConfiguration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.sn76430NConfiguration.defaultY = mainWindowY_;
    defaultFrontPanelX_ = mainWindowX_+windowInfo.xBorder2;
    defaultFrontPanelY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
        
    computerConfiguration.videoTerminalConfiguration.vt52DefaultSetUpFeature = 0x4092l;
    computerConfiguration.videoTerminalConfiguration.vt100DefaultSetUpFeature = 0xcad2l;
    computerConfiguration.videoTerminalConfiguration.vtExternalDefaultSetUpFeature = 0xcad2l;
    computerConfiguration.videoTerminalConfiguration.defaultBellFrequency = 800;

    configPointer->Read("/Xml/DebugScrt", &computerConfiguration.debuggerConfiguration.mode, true);
    computerConfiguration.debuggerConfiguration.callRegister = configPointer->Read("/Xml/DebugCallReg", 4);
    computerConfiguration.debuggerConfiguration.callAddress = configPointer->Read("/Xml/DebugCallAddress", -1);
    computerConfiguration.debuggerConfiguration.returnRegister = configPointer->Read("/Xml/DebugRetReg", 5);
    computerConfiguration.debuggerConfiguration.returnAddress = configPointer->Read("/Xml/DebugRetAddress", -1);

    computerConfiguration.ledDisplayConfiguration.showDataOnLoad = false;
    computerConfiguration.ledDisplayConfiguration.followSwitches = false;
    computerConfiguration.ledDisplayConfiguration.followButton = false;
    computerConfiguration.ledDisplayConfiguration.showDataOnCycle = false;
    for (int til=0; til<MAX_DATA_TIL; til++)
    {
        computerConfiguration.ledDisplayConfiguration.datatil[til] = false;
        computerConfiguration.ledDisplayConfiguration.datatilDpQ[til] = false;
    }
    for (int til=0; til<MAX_ADDRESS_TIL; til++)
        computerConfiguration.ledDisplayConfiguration.addresstil[til] = false;
    for (int til=0; til<MAX_MULTI_TIL; til++)
    {
        computerConfiguration.multiSegDisplayConfiguration.multitil[til] = false;
        computerConfiguration.multiSegDisplayConfiguration.multitilAddressLine[til] = -1;
    }

    computerConfiguration.runPressType = RUN_TYPE_DEFAULT;
    computerConfiguration.resetPressType = RESET_TYPE_DEFAULT;
    computerConfiguration.stepPressType = STEP_TYPE_DEFAULT;

    for (int ef=1; ef<5; ef++)
        computerConfiguration.bitKeypadEf[ef] = false;

    oldXmlFileName_ = xmlDir + xmlFile;
    oldXmlDate_ = newDate;
    computerConfiguration.mainDir_ = dataDir_;

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
    
    tagTypeInt = 0;
    bool continueLoop = true;
    while (child && continueLoop)
    {
        wxString childName = child->GetName();
        
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        if (tagTypeInt == TAG_SYSTEM)
        {
            parseXml_System (*child);
            continueLoop = false;
        }
    }

    computerConfiguration.memAccessConfiguration.directory = computerConfiguration.mainDir_ ;
    computerConfiguration.characterRomConfiguration.directory = computerConfiguration.mainDir_ ;
    computerConfiguration.batchConfiguration.directory = computerConfiguration.mainDir_ ;
    computerConfiguration.keyFileConfiguration.directory = computerConfiguration.mainDir_;
    computerConfiguration.printerFileConfiguration.directory = computerConfiguration.mainDir_;
    computerConfiguration.screenDumpFileConfiguration.directory = computerConfiguration.mainDir_;
    computerConfiguration.videoTerminalConfiguration.xmodemDirectory = computerConfiguration.mainDir_;
    computerConfiguration.vip2KVideoConfiguration.sequencerDirectory = computerConfiguration.mainDir_;
    computerConfiguration.multiSegDisplayConfiguration.tilFontDirectory = computerConfiguration.mainDir_;
    computerConfiguration.videoTerminalConfiguration.vtCharRomDirectory = computerConfiguration.mainDir_ ;
    computerConfiguration.videoTerminalConfiguration.wavDirectory = computerConfiguration.mainDir_ ;
    for (int fdcType = 0; fdcType<FDCTYPE_MAX; fdcType++)
    {
        for (int disk=0; disk<4; disk++)
        {
            floppy_[fdcType][disk] = "";
            floppyDir_[fdcType][disk] = computerConfiguration.mainDir_;
            directoryMode_[fdcType][disk] = false;
        }
    }
    for (int tape=0; tape<2; tape++)
    {
        computerConfiguration.wavConfiguration[tape].fileName = "";
        computerConfiguration.wavConfiguration[tape].directory = computerConfiguration.mainDir_;
    }
    for (int mem=0; mem<2; mem++)
        computerConfiguration.memoryConfiguration[mem].dirname = computerConfiguration.mainDir_;

    child = doc.GetRoot()->GetChildren();
    while (child)
    {
        wxString childName = child->GetName();
        int uartConnection = UART_CONNECTION_NONE;
        
        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_SYSTEM:
     //           parseXml_System (*child);
            break;
                
            case TAG_BASIC:
                parseXml_Basic (*child);
            break;
                
            case TAG_LOCATIONS:
                parseXml_Locations (*child);
            break;
                
            case TAG_BOOTSTRAP:
                parseXml_Bootstrap (*child);
            break;
                
            case TAG_PANEL:
                if (computerConfiguration.frontPanelConfiguration[PANEL_MAIN].defined)
                {
                    computerConfiguration.frontPanelConfiguration.resize(computerConfiguration.frontPanelConfiguration.size()+1);
                    parseXml_FrontPanel (*child, (int)computerConfiguration.frontPanelConfiguration.size()-1);
                }
                else
                {
                    if (child->GetAttribute("type") == "io")
                        parseXml_FrontPanel (*child, PANEL_MAIN);
                    else
                    {
                        if (child->GetAttribute("init") == "on")
                            computerConfiguration.frontPanelConfiguration[PANEL_MAIN].show = true;
                        
                        computerConfiguration.frontPanelConfiguration[PANEL_MAIN].defined = true;
                        parseXml_FrontPanel (*child, PANEL_MAIN);
                    }
                }
            break;
                
            case TAG_MEMORY:
                if (child->HasAttribute("mask"))
                    computerConfiguration.memoryMask = (int) parseXml_Number(*child, "mask");
                parseXml_Memory (*child);
            break;

            case TAG_DISK:
                computerConfiguration.fdcConfiguration.drives = (int) ((parseXml_Number(*child, "drives")-1)&0x3) + 1;
                if (child->GetAttribute("type") == "upd765")
                    parseXml_Upd765 (*child);
                if (child->GetAttribute("type") == "1793")
                {
                    computerConfiguration.fdcConfiguration.wd1793_defined = true;
                    parseXml_FdcDisk (*child);
                }
                if (child->GetAttribute("type") == "1770")
                {
                    computerConfiguration.fdcConfiguration.wd1770_defined = true;
                    parseXml_FdcDisk (*child);
                }
                if (child->GetAttribute("type") == "ide")
                    parseXml_IdeDisk (*child);
                if (child->GetAttribute("type") == "tu58")
                    parseXml_Tu58Disk (*child);
            break;
                
            case TAG_VIDEO:
                if (child->GetAttribute("type") == "coin")
                    parseXml_CoinVideo (*child);
                if (child->GetAttribute("type") == "pixie" || child->GetAttribute("type") == "cdp1861" || child->GetAttribute("type") == "1861")
                    parseXml_PixieVideo (*child);
                if (child->GetAttribute("type") == "cdp1862" || child->GetAttribute("type") == "1862")
                    parseXml_1862Video (*child);
                if (child->GetAttribute("type") == "cdp1864" || child->GetAttribute("type") == "1864")
                    parseXml_1864Video (*child);
                if (child->GetAttribute("type") == "vip2k")
                    parseXml_Vip2KVideo (*child);
                if (child->GetAttribute("type") == "fred")
                    parseXml_fredVideo (*child);
                if (child->GetAttribute("type") == "mc6845")
                    parseXml_MC6845Video (*child);
                if (child->GetAttribute("type") == "mc6847")
                    parseXml_MC6847Video (*child);
                if (child->GetAttribute("type") == "tms9918")
                    parseXml_TMS9918Video (*child);
                if (child->GetAttribute("type") == "intel8275")
                    parseXml_Intel8275Video (*child);
                if (child->GetAttribute("type") == "1870")
                    parseXml_VisVideo (*child);
                if (child->GetAttribute("type") == "sn76430n")
                    parseXml_SN76430NVideo (*child);
                if (child->GetAttribute("type") == "st4")
                    parseXml_St4Video (*child);
            break;

            case TAG_KEYBOARD:
                if (child->GetAttribute("type") == "ps2")
                {
                   computerConfiguration.ps2KeyboardConfiguration.interrupt = false;
                   parseXml_Ps2Keyboard (*child);
                }
                if (child->GetAttribute("type") == "ps2-interrupt")
                {
                   computerConfiguration.ps2KeyboardConfiguration.interrupt = true;
                   parseXml_Ps2Keyboard (*child);
                }
                if (child->GetAttribute("type") == "ps2-gpio")
                {
                   parseXml_Gpio (*child);
                }
                if (child->GetAttribute("type") == "ps2-89C4051")
                {
                   parseXml_Gpio (*child);
                }
                if (child->GetAttribute("type") == "ascii")
                   parseXml_AsciiKeyboard (*child);
                if (child->GetAttribute("type") == "1871")
                   parseXml_1871Keyboard (*child);
                if (child->GetAttribute("type") == "latch")
                    parseXml_LatchKeyboard (*child);
                if (child->GetAttribute("type") == "matrix")
                    parseXml_MatrixKeyboard (*child);
            break;

            case TAG_CDP1851:
                parseXml_Cdp1851 (*child, child->GetAttribute("init") == "on");
            break;

            case TAG_CDP1852:
                parseXml_Cdp1852 (*child, child->GetAttribute("init") == "on");
            break;

            case TAG_CDP1854:
                if (child->GetAttribute("connection") == "tu58")
                    uartConnection = UART_CONNECTION_TU58;
                if (child->GetAttribute("connection") == "vt1802")
                    uartConnection = UART_CONNECTION_VT1802;
                if (child->GetAttribute("connection") == "vis1802")
                    uartConnection = UART_CONNECTION_VIS1802;
                if (child->GetAttribute("connection") == "vt100")
                    uartConnection = UART_CONNECTION_VT100;
                parseXml_Cdp1854 (*child, uartConnection);
            break;

            case TAG_CDP1855:
                computerConfiguration.cdp1855Configuration.maxNumberOfMdu =  (int)parseXml_Number(*child, "num");
                parseXml_Cdp1855 (*child);
            break;

            case TAG_CDP1877:
                parseXml_Cdp1877 (*child);
            break;

            case TAG_CDP1878:
                parseXml_Cdp1878 (*child);
            break;

            case TAG_CD4536B:
                parseXml_Cd4536b (*child);
            break;

            case TAG_AD_CONVERTOR:
                parseXml_AdConvertor (*child);
            break;

            case TAG_KEYPAD:
                if (child->GetAttribute("type") == "elf")
                    parseXml_ElfKeypad (*child);
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
                                computerConfiguration.keyLatchConfiguration[pad].padNumberStr = " " + child->GetAttribute("pad");
                                computerConfiguration.keyLatchConfiguration[pad].padNumberStr = computerConfiguration.keyLatchConfiguration[pad].padNumberStr.Capitalize();
                                parseXml_LatchKeypad (pad, *child);
                            }
                        }
                    }
                    else
                    {
                        computerConfiguration.keyLatchConfiguration[1].padNumberStr = "";
                        parseXml_LatchKeypad (1, *child);
                    }
                }
                if (child->GetAttribute("type") == "fred")
                    parseXml_FredKeypad (*child);
                if (child->GetAttribute("type") == "hug" || child->GetAttribute("type") == "eti")
                    parseXml_EtiKeypad (*child);
                if (child->GetAttribute("type") == "cv")
                    parseXml_CvKeypad (*child);
                if (child->GetAttribute("type") == "bit")
                {
                    for (int pad=0; pad<MAX_BITKEYPADS; pad++)
                    {
                        padLetter.Printf("%c", pad+97);
                        padNumber.Printf("%c", pad+49);
                        if (child->GetAttribute("pad") == padLetter || child->GetAttribute("pad") == padNumber)
                        {
                            computerConfiguration.bitKeypadConfiguration[pad].number = child->GetAttribute("pad");
                            computerConfiguration.bitKeypadConfiguration[pad].number = computerConfiguration.bitKeypadConfiguration[pad].number.Capitalize();
                            parseXml_BitKeypad (pad, *child);
                        }
                   }
                }
            break;

            case TAG_BITKEYPAD:
                for (int pad=0; pad<MAX_BITKEYPADS; pad++)
                {
                    padLetter.Printf("%c", pad+97);
                    padNumber.Printf("%c", pad+49);
                    if (child->GetAttribute("pad") == padLetter || child->GetAttribute("pad") == padNumber)
                    {
                        computerConfiguration.bitKeypadConfiguration[pad].number = child->GetAttribute("pad");
                        computerConfiguration.bitKeypadConfiguration[pad].number = computerConfiguration.bitKeypadConfiguration[pad].number.Capitalize();
                        parseXml_BitKeypad (pad, *child);
                    }
               }
            break;

            case TAG_CVKEYPAD:
                parseXml_CvKeypad (*child);
            break;

            case TAG_VT:
                if (child->GetAttribute("type") == "vt100")
                    computerConfiguration.videoTerminalConfiguration.type = VT100;
                if (child->GetAttribute("type") == "vt52")
                {
                    computerConfiguration.videoTerminalConfiguration.type = VT52;
                    computerConfiguration.videoTerminalConfiguration.vtCharRomFileName = "vt52.a.bin";
                }
                if (child->GetAttribute("type") == "external")
                {
                    computerConfiguration.videoTerminalConfiguration.external = true;
                    computerConfiguration.videoTerminalConfiguration.type = VTNONE;
                }
                if (child->GetAttribute("type") == "loop_back")
                {
                    computerConfiguration.videoTerminalConfiguration.loop_back = true;
                    computerConfiguration.videoTerminalConfiguration.type = VTNONE;
                }
                if (child->GetAttribute("connection") == "serial")
                    parseXml_SerialVt (*child);
                if (child->GetAttribute("connection") == "uart1854")
                {
                    computerConfiguration.videoTerminalConfiguration.uart1854_defined = true;
                    parseXml_UartVt (*child, false);
                }
                if (child->GetAttribute("connection") == "uart16450")
                {
                    computerConfiguration.videoTerminalConfiguration.uart16450_defined = true;
                    parseXml_UartVt (*child, true);
                }
            break;

            case TAG_PRINTER:
                if (child->GetAttribute("type") == "")
                {
                    computerConfiguration.basicPrinterConfiguration.defined = true;
                    parseXml_Printer (*child, PRINTER_BASIC);
                }
                if (child->GetAttribute("type") == "q")
                {
                    computerConfiguration.qSerialPrinterConfiguration.defined = true;
                    parseXml_Printer (*child, PRINTER_SERIAL_Q);
                }
                if (child->GetAttribute("type") == "centronics")
                {
                    computerConfiguration.centronicsPrinterConfiguration.defined = true;
                    parseXml_Printer (*child, PRINTER_CENTRONICS);
                }
                if (child->GetAttribute("type") == "parallel")
                {
                    computerConfiguration.parallelPrinterConfiguration.defined = true;
                    parseXml_Printer (*child, PRINTER_PARALLEL);
                }
                if (child->GetAttribute("type") == "serial")
                {
                    computerConfiguration.serialPrinterConfiguration.defined = true;
                    parseXml_Printer (*child, PRINTER_SERIAL);
                }
                if (child->GetAttribute("type") == "thermal")
                {
                    computerConfiguration.thermalPrinterConfiguration.defined = true;
                    parseXml_Printer (*child, COMXTHPRINTER);
                }
            break;

            case TAG_CASSETTE:
                if (child->GetAttribute("type") == "cv")
                {
                    if (child->GetAttribute("decks") == "2")
                        computerConfiguration.hwTapeConfiguration.twoDecks = true;
                    computerConfiguration.hwTapeConfiguration.defined = true;
                    computerConfiguration.hwTapeConfiguration.format = TAPE_FORMAT_CV;
                    parseXml_HwCassette (*child);
                }
                else
                {
                    if (child->GetAttribute("type") == "fred")
                    {
                        if (child->GetAttribute("decks") == "2")
                            computerConfiguration.hwTapeConfiguration.twoDecks = true;
                        computerConfiguration.hwTapeConfiguration.defined = true;
                        computerConfiguration.hwTapeConfiguration.format = TAPE_FORMAT_AUTO;
                        parseXml_HwCassette (*child);
                    }
                    else
                    {
                        if (child->GetAttribute("decks") == "2")
                            computerConfiguration.swTapeConfiguration.twoDecks = true;
                        computerConfiguration.swTapeConfiguration.defined = true;
                        parseXml_SwCassette (*child);
                    }
                }
            break;

            case TAG_SOUND:
                if (child->GetAttribute("type") == "q")
                    parseXml_QSound (*child);
                if (child->GetAttribute("type") == "studio")
                    parseXml_StudioSound (*child);
                if (child->GetAttribute("type") == "bit")
                    parseXml_OutBitSound (*child);
                if (child->GetAttribute("type") == "cdp1863" || child->GetAttribute("type") == "1863")
                    parseXml_Cdp1863Sound (*child);
                if (child->GetAttribute("type") == "8912")
                    parseXml_AY_3_8912Sound (*child);
                if (child->GetAttribute("type") == "vp550")
                    computerConfiguration.soundConfiguration.type = SOUND_SUPER_VP550;
                if (child->GetAttribute("type") == "vp551")
                    computerConfiguration.soundConfiguration.type = SOUND_SUPER_VP551;
                if (child->GetAttribute("stereo") == "on")
                    computerConfiguration.soundConfiguration.stereo = 2;
            break;

            case TAG_XMODEM:
                computerConfiguration.videoTerminalConfiguration.xModem_defined = true;
                parseXml_Xmodem (*child);
            break;

            case TAG_HEXMODEM:
                computerConfiguration.videoTerminalConfiguration.hexModem_defined = true;
                parseXml_Xmodem (*child);
            break;

            case TAG_EFBUTTONS:
                parseXml_EfButtons (*child);
            break;

            case TAG_DIP:
                parseXml_Dip (*child);
            break;

            case TAG_IOGROUP:
                parseXml_IoGroup (*child);
            break;
          
            case TAG_FLIPFLOP:
                parseXml_Flipflop (*child);
            break;

            case TAG_RTC:
                if (child->GetAttribute("type") == "cdp1879")
                    parseXml_RtcCdp1879 (*child);
                if (child->GetAttribute("type") == "m48t58")
                    parseXml_RtcM48T58 (*child);
                if (child->GetAttribute("type") == "ds12887")
                    parseXml_RtcDS12887 (*child);
            break;

            case TAG_USB:
                if (child->GetAttribute("type") == "superboard")
                    parseXml_UsbSuperBoard (*child);
            break;

            case TAG_KEYFILE:
                parseXml_KeyFile (*child);
            break;
                
            case TAG_SPLASH:
                parseXml_Splash (*child);
            break;
                
            case TAG_VIDEODUMP:
                parseXml_VideoDump (*child);
            break;
                
            case TAG_MEMACCESS:
                parseXml_MemAccess (*child);
            break;
                
            case TAG_GUI:
                parseXml_Gui (*child);
            break;
                
            case TAG_DIAG:
                parseXml_Diag (*child);
            break;
                
            case TAG_BATCHWAV:
                parseXml_BatchWav (*child);
            break;
        
            case TAG_DEBUGGER:
                parseXml_Debugger (*child);
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
    
    for (std::vector<AssemblerConfiguration>::iterator assemblerInfo = computerConfiguration.assemblerConfiguration.begin (); assemblerInfo != computerConfiguration.assemblerConfiguration.end (); ++assemblerInfo)
    {
        if (!assemblerInfo->directoryName_defined)
            assemblerInfo->directoryName = computerConfiguration.mainDir_;
        if (!assemblerInfo->fileName_defined)
            assemblerInfo->fileName = "";
    }
    
    if (computerConfiguration.mainFrontPanelConfiguration.dataSwitchButtons == computerConfiguration.mainFrontPanelConfiguration.dataPushButtons)
    {
        computerConfiguration.mainFrontPanelConfiguration.dataSwitchButtons = false;
        computerConfiguration.mainFrontPanelConfiguration.dataPushButtons = true;
    }

    int numberOfFrontPanels_ = 0;
    
    wxPoint position, lastPosition = wxPoint(mainWindowX_, mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    wxSize lastSize = computerConfiguration.frontPanelConfiguration[PANEL_MAIN].size;
    for (std::vector<FrontPanelConfiguration>::iterator frontPanel = computerConfiguration.frontPanelConfiguration.begin (); frontPanel != computerConfiguration.frontPanelConfiguration.end (); ++frontPanel)
    {
        switch (frontPanel->posType)
        {
            case POS_TYPE_GRID:
                position.x = lastPosition.x + lastSize.x * frontPanel->defaultPos.x;
                if (frontPanel->defaultPos.x > 0)
                    position.x += computerConfiguration.mainFrontPanelConfiguration.xBorder;
                position.y = lastPosition.y + lastSize.y * frontPanel->defaultPos.y;
                if (frontPanel->defaultPos.y > 0)
                    position.y += computerConfiguration.mainFrontPanelConfiguration.yBorder;
            break;

            case POS_TYPE_REAL:
                position = frontPanel->defaultPos;
            break;

            default: // POS_TYPE_RELATIVE:
                position.x = mainWindowX_ + frontPanel->defaultPos.x;
                position.y = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder + frontPanel->defaultPos.y;
            break;

        }
        frontPanel->defaultPos.x = position.x;
        frontPanel->defaultPos.y = position.y;
        
        lastSize = frontPanel->size;
        lastPosition = position;
        numberOfFrontPanels_++;
    }
}

void XmlParser::parseXml_System(wxXmlNode &node)
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
        "dma_in",
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
        TAG_DMA_IN,
        TAG_INT,
        TAG_STATUSBAR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    computerConfiguration.dmaConfiguration.startOnFirstQ = false;
    computerConfiguration.dmaConfiguration.startOnFirstOut = 0;
    computerInfo.name = "";
    computerInfo.configuration = "";
    computerConfiguration.monitorConfiguration.ef = init_EfFlag();
    computerConfiguration.monitorConfiguration.ioGroupVector.clear();

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
                computerInfo.name = child->GetNodeContent();
            break;

            case TAG_CONFIG:
                computerInfo.configuration = child->GetNodeContent();
            break;
                
            case TAG_CPU:
                setCpuType(child->GetNodeContent());
            break;

            case TAG_CLOCK:
                computerConfiguration.clock_ = getDoubleString(child->GetNodeContent(), childName, 500, "500");
            break;
                
            case TAG_BOOT:
                computerConfiguration.autoBootConfiguration.defined = true;
                if (child->GetAttribute("start") == "no")
                 computerConfiguration.autoBootConfiguration.defined = false;
                if (child->GetAttribute("dma") == "out")
                    computerConfiguration.autoBootConfiguration.dmaOutOnBoot = true;
                if (child->GetAttribute("dma") == "zero")
                    computerConfiguration.autoBootConfiguration.dmaOutOnBootIfMem0is0 = true;

                computerConfiguration.autoBootConfiguration.address = getHexDec(child->GetNodeContent());
            break;
                
            case TAG_F12:
                if (child->GetNodeContent() == "stop")
                    computerConfiguration.f12reset = false;
            break;
                
            case TAG_INIT:
                if (child->GetAttribute("ef") == "1")
                    computerConfiguration.defaultEfConfiguration.ef1 = (int)parseXml_Number(*child);
                if (child->GetAttribute("ef") == "2")
                    computerConfiguration.defaultEfConfiguration.ef2 = (int)parseXml_Number(*child);
                if (child->GetAttribute("ef") == "3")
                    computerConfiguration.defaultEfConfiguration.ef3 = (int)parseXml_Number(*child);
                if (child->GetAttribute("ef") == "4")
                    computerConfiguration.defaultEfConfiguration.ef4 = (int)parseXml_Number(*child);
            break;

            case TAG_MONITOR:
                computerConfiguration.monitorConfiguration.defined = true;
                computerConfiguration.monitorConfiguration.ef = parseXml_EfFlag(*child, NANO_MONITOR_EF);
            break;

            case TAG_DIRNAME:
                computerConfiguration.mainDir_ = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.mainDir_.Right(1) != pathSeparator_)
                    computerConfiguration.mainDir_ += pathSeparator_;
            break;
                
            case TAG_DMA:
                computerConfiguration.dmaConfiguration.defined = true;
                if (child->GetAttribute("out") == "q")
                    computerConfiguration.dmaConfiguration.startOnFirstQ = true;
                else
                    computerConfiguration.dmaConfiguration.startOnFirstOut = (int)parseXml_Number(*child, "out");
                computerConfiguration.dmaConfiguration.cycleValue = (int)parseXml_Number(*child);
            break;

            case TAG_DMA_IN:
                if (child->GetNodeContent() == "reset")
                    computerConfiguration.dmaInReset_defined = true;
            break;

            case TAG_INT:
                computerConfiguration.interruptConfiguration.defined = true;
                computerConfiguration.interruptConfiguration.cycleValue = (int)parseXml_Number(*child);
            break;

            case TAG_STATUSBAR:
                if (child->GetNodeContent() == "comx")
                    computerConfiguration.vis1870Configuration.statusBarType = STATUSBAR_COMX;
                if (child->GetNodeContent() == "diag")
                    computerConfiguration.vis1870Configuration.statusBarType = STATUSBAR_DIAG;
                if (child->GetNodeContent() == "cidelsa")
                {
                    computerConfiguration.vis1870Configuration.statusBarType = STATUSBAR_CIDELSA;
                    if (child->HasAttribute("out"))
                        computerConfiguration.vis1870Configuration.statusBarLedOut = (int)parseXml_Number(*child, "out");
                }
                if (child->GetNodeContent() == "vip2")
                    computerConfiguration.cdp1861Configuration.statusBarType = STATUSBAR_VIP2;
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
        if (computerInfo.configuration == "")
            guiName = computerInfo.name;
        else
            guiName = computerInfo.configuration;
        guiName.Replace("&","&&");

        XRCCTRL(*this,"ConfigTextXml", wxStaticText)->SetLabel(guiName);
    }
}

void XmlParser::parseXml_Basic(wxXmlNode &node)
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

    computerConfiguration.basicConfiguration.defus.defined = false;
    computerConfiguration.basicConfiguration.eop.defined = false;
    computerConfiguration.basicConfiguration.string.defined = false;
    computerConfiguration.basicConfiguration.array.defined = false;
    computerConfiguration.basicConfiguration.eod.defined = false;
    computerConfiguration.basicConfiguration.ramAddress.defined = false;

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
                computerInfo.ploadExtension = child->GetNodeContent();
                ploadUpper = computerInfo.ploadExtension;
                ploadUpper.MakeUpper();
                for (int i=0; i<4; i++)
                {
                    if (i < (int)ploadUpper.Len())
                        computerConfiguration.basicConfiguration.pLoadSaveName[i] = ploadUpper.GetChar(i);
                    else
                        computerConfiguration.basicConfiguration.pLoadSaveName[i] = ' ';
                }
            break;

            case TAG_ID:
                ploadUpper = child->GetNodeContent();
                ploadUpper.MakeUpper();
                for (int i=0; i<4; i++)
                {
                    if (i < (int)ploadUpper.Len())
                        computerConfiguration.basicConfiguration.pLoadSaveName[i] = ploadUpper.GetChar(i);
                    else
                        computerConfiguration.basicConfiguration.pLoadSaveName[i] = ' ';
                }
            break;

            case TAG_DEFUS:
                computerConfiguration.basicConfiguration.defus.defined = true;
                computerConfiguration.basicConfiguration.defus.value = (int)parseXml_Number(*child);
            break;

            case TAG_EOP:
                computerConfiguration.basicConfiguration.eop.defined = true;
                computerConfiguration.basicConfiguration.eop.value = (int)parseXml_Number(*child);
            break;

            case TAG_STRING:
                computerConfiguration.basicConfiguration.string.defined = true;
                computerConfiguration.basicConfiguration.string.value = (int)parseXml_Number(*child);
            break;

            case TAG_ARRAY:
                computerConfiguration.basicConfiguration.array.defined = true;
                computerConfiguration.basicConfiguration.array.value = (int)parseXml_Number(*child);
            break;

            case TAG_EOD:
                computerConfiguration.basicConfiguration.eod.defined = true;
                computerConfiguration.basicConfiguration.eod.value = (int)parseXml_Number(*child);
            break;

            case TAG_RAM:
                computerConfiguration.basicConfiguration.ramAddress.defined = true;
                computerConfiguration.basicConfiguration.ramAddress.value = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_Locations(wxXmlNode &node)
{
    wxString tagList[]=
    {
        "key_input",
        "write_address",
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
        "start_thermal",
        "stop_thermal",
        "start_fm_basic",
        "stop_fm_basic",
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
        TAG_WRITE_ADDRESS,
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
        TAG_START_THERMAL,
        TAG_STOP_THERMAL,
        TAG_START_FM_BASIC,
        TAG_STOP_FM_BASIC,
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
    wxString addressLocationConfiguration, addressValues;
    Word address;
    LocationInfo newInfo;
    CheckAddressInfo newAddressInfo;
    CheckRegInfo newRegInfo;
    char newtrigger;
    WriteAddress writeAddress;
        
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
                addressLocationConfiguration = child->GetNodeContent();
                while (addressLocationConfiguration != "")
                {
                    address = getNextHexDec(&addressLocationConfiguration);
                    if (address != 0)
                        computerConfiguration.addressLocationConfiguration.keyInputAddress.push_back(address);
                }
            break;

            case TAG_WRITE_ADDRESS:
                writeAddress.address = (Word)getHexDec(child->GetNodeContent());
                writeAddress.value = (Byte)parseXml_Number(*child, "value");
                
                if (child->GetAttribute("function") == "debug")
                    writeAddress.function = WRITE_ADDRESS_DEBUG;

                computerConfiguration.addressLocationConfiguration.writeAddress.push_back(writeAddress);
            break;
                
            case TAG_IN_RELEASE:
                addressLocationConfiguration = child->GetNodeContent();
                while (addressLocationConfiguration != "")
                {
                    address = getNextHexDec(&addressLocationConfiguration);
                    if (address != 0)
                        computerConfiguration.addressLocationConfiguration.inReleaseAddress.push_back(address);
                }
            break;

            case TAG_CODE_START:
                computerConfiguration.addressLocationConfiguration.code_start = (int)getHexDec(child->GetNodeContent());
            break;
                
            case TAG_CODE_START_HIGH:
                computerConfiguration.addressLocationConfiguration.code_start_high = (int)getHexDec(child->GetNodeContent());
            break;
                
            case TAG_CODE_START_LOW:
                computerConfiguration.addressLocationConfiguration.code_start_low = (int)getHexDec(child->GetNodeContent());
            break;

            case TAG_CODE_END_HIGH:
                computerConfiguration.addressLocationConfiguration.code_end_high = (int)getHexDec(child->GetNodeContent());
            break;
                
            case TAG_CODE_END_LOW:
                computerConfiguration.addressLocationConfiguration.code_end_low = (int)getHexDec(child->GetNodeContent());
            break;
                
            case TAG_REG_END:
                computerConfiguration.addressLocationConfiguration.reg_end = (int)getHexDec(child->GetNodeContent());
                computerConfiguration.addressLocationConfiguration.reg_end_adjust = 0;
                if (child->HasAttribute("adjust"))
                    computerConfiguration.addressLocationConfiguration.reg_end_adjust = (int)parseXml_Number(*child, "adjust");
            break;

            case TAG_RESET_STATE:
            case TAG_UT_STATE:
            case TAG_BASIC_STATE:
            case TAG_BASIC_STATE_CW:
            case TAG_RUN_STATE:
            case TAG_START_THERMAL:
            case TAG_STOP_THERMAL:
            case TAG_START_FM_BASIC:
            case TAG_STOP_FM_BASIC:
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
                    addressLocationConfiguration = child->GetAttribute("address");
                    addressValues = child->GetAttribute("value");
                    while (addressLocationConfiguration != "")
                    {
                        newAddressInfo.checkAddress = getNextHexDec(&addressLocationConfiguration);
                        newAddressInfo.checkValue = getNextHexDec(&addressValues);

                        if (newAddressInfo.checkAddress != 0)
                            newInfo.checkAddressInfo.push_back(newAddressInfo);
                    }
                }

                if (child->HasAttribute("reg"))
                {
                    addressLocationConfiguration = child->GetAttribute("reg");
                    addressValues = child->GetAttribute("value");
                    while (addressLocationConfiguration != "")
                    {
                        newRegInfo.checkReg = getNextHexDec(&addressLocationConfiguration);
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
                            computerConfiguration.stateCommand_[RESETSTATE] = child->GetAttribute("command");
                        break;
                        case TAG_UT_STATE:
                            computerConfiguration.stateCommand_[UTSTATE] = child->GetAttribute("command");
                        break;
                        case TAG_BASIC_STATE:
                            computerConfiguration.stateCommand_[BASICSTATE] = child->GetAttribute("command");
                        break;
                        case TAG_BASIC_STATE_CW:
                            computerConfiguration.stateCommand_[BASICSTATECW] = child->GetAttribute("command");
                        break;
                    }
                }

                newtrigger = computerConfiguration.addressLocationConfiguration.locationInfo.size();

                computerConfiguration.addressLocationConfiguration.locationInfo.push_back(newInfo);

                addressLocationConfiguration = child->GetNodeContent();
                while (addressLocationConfiguration != "")
                {
                    address = getNextHexDec(&addressLocationConfiguration);
                    if (address != 0)
                    {
//#if defined (__linux__)
//                        if (computerConfiguration.locationTriggerConfiguration[address].index[0] == 0xff)
//#else
                        if (computerConfiguration.locationTriggerConfiguration[address].index[0] == -1)
//#endif
                            computerConfiguration.locationTriggerConfiguration[address].index[0] = newtrigger;
                        else
                            computerConfiguration.locationTriggerConfiguration[address].index.push_back(newtrigger);
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

void XmlParser::parseXml_Bootstrap(wxXmlNode &node)
{
    int number;
    
    wxString tagList[]=
    {
        "address",
        "in",
        "out",
        "q",
        "io",
        "run",
        "timer",
        "read",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_ADDRESS,
        TAG_IN,
        TAG_OUT,
        TAG_Q,
        TAG_IO,
        TAG_RUN,
        TAG_TIMER,
        TAG_READ,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.bootstrapConfiguration.ioGroupVector.clear();
    computerConfiguration.bootstrapConfiguration.defined = true;
    computerConfiguration.bootstrapConfiguration.output = init_IoPort();
    computerConfiguration.bootstrapConfiguration.input = init_IoPort();
    computerConfiguration.bootstrapConfiguration.type = BOOTSTRAPREAD;

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
                computerConfiguration.bootstrapConfiguration.address = getHexDec(child->GetNodeContent());
            break;

            case TAG_OUT:
                computerConfiguration.bootstrapConfiguration.type = BOOTSTRAPOUT;
                computerConfiguration.bootstrapConfiguration.output =  parseXml_IoPort(*child);
            break;
          
            case TAG_Q:
                computerConfiguration.bootstrapConfiguration.type = BOOTSTRAPQ;
            break;

            case TAG_IN:
                computerConfiguration.bootstrapConfiguration.type = BOOTSTRAPIN;
                computerConfiguration.bootstrapConfiguration.input = parseXml_IoPort(*child);
            break;
 
            case TAG_IO:
                number = (int)parseXml_Number(*child);
                    computerConfiguration.bootstrapConfiguration.type = BOOTSTRAPIO;
                computerConfiguration.bootstrapConfiguration.input = parseXml_IoPort(*child);
                computerConfiguration.bootstrapConfiguration.output = parseXml_IoPort(*child);
            break;

            case TAG_RUN:
                computerConfiguration.bootstrapConfiguration.type = BOOTSTRAPRUN;
            break;

            case TAG_TIMER:
                if (child->GetAttribute("type") == "onin")
                    computerConfiguration.bootstrapConfiguration.type = BOOTSTRAPTIMER_ONIN;
                else
                    computerConfiguration.bootstrapConfiguration.type = BOOTSTRAPTIMER;
                computerConfiguration.bootstrapConfiguration.timer = (int)parseXml_Number(*child);
            break;

            case TAG_READ:
                computerConfiguration.bootstrapConfiguration.type = BOOTSTRAPREAD;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.bootstrapConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.bootstrapConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_IdeDisk(wxXmlNode &node)
{
    computerConfiguration.ideConfiguration.defined = true;
    computerConfiguration.fdcType_ = FDCTYPE_TU58_IDE;

    wxString tagList[]=
    {
        "filename",
        "dirname",
        "out",
        "in",
        "int",
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
        TAG_INTERRUPT,
        TAG_TRACKS,
        TAG_HEADS,
        TAG_SECTORS,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt, driveNumber=0;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.ideConfiguration.ioGroupVector.clear();
    computerConfiguration.ideConfiguration.tracks = 2048;
    computerConfiguration.ideConfiguration.heads = 16;
    computerConfiguration.ideConfiguration.sectors = 64;
    
    computerConfiguration.ideConfiguration.status = init_IoPort();
    computerConfiguration.ideConfiguration.input = init_IoPort();
    computerConfiguration.ideConfiguration.selectOutput = init_IoPort();
    computerConfiguration.ideConfiguration.writeOutput = init_IoPort();

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
                driveNumber = (int)parseXml_Number(*child, "drive");
                floppy_[FDCTYPE_TU58_IDE][driveNumber] = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                driveNumber = (int)parseXml_Number(*child, "drive");
                floppyDir_[FDCTYPE_TU58_IDE][driveNumber] = dataDir_ + child->GetNodeContent();
                if (floppyDir_[FDCTYPE_TU58_IDE][driveNumber].Right(1) != pathSeparator_)
                    floppyDir_[FDCTYPE_TU58_IDE][driveNumber] += pathSeparator_;
            break;

            case TAG_IN:
                if (child->GetAttribute("type") == "status")
                    computerConfiguration.ideConfiguration.status = parseXml_IoPort(*child, IDE_READ_STATUS_IN);
                if (child->GetAttribute("type") == "read")
                    computerConfiguration.ideConfiguration.input = parseXml_IoPort(*child, IDE_READ_PORT_IN);
            break;

           case TAG_OUT:
                if (child->GetAttribute("type") == "select")
                    computerConfiguration.ideConfiguration.selectOutput = parseXml_IoPort(*child, IDE_SELECT_PORT_OUT);
                if (child->GetAttribute("type") == "write")
                    computerConfiguration.ideConfiguration.writeOutput = parseXml_IoPort(*child, IDE_WRITE_PORT_OUT);
            break;
                
            case TAG_INTERRUPT:
                computerConfiguration.ideConfiguration.picInterrupt = (int)parseXml_Number(*child);
            break;

            case TAG_TRACKS:
                computerConfiguration.ideConfiguration.tracks = (int)parseXml_Number(*child);
            break;

            case TAG_HEADS:
                computerConfiguration.ideConfiguration.heads = (int)parseXml_Number(*child);
            break;

            case TAG_SECTORS:
                computerConfiguration.ideConfiguration.sectors = (int)parseXml_Number(*child);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.ideConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.ideConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_FdcDisk(wxXmlNode &node)
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
        "clearaddress",
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
        TAG_CLEAR_ADDRESS,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt, diskNumber;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.fdcConfiguration.ioGroupVector.clear();
    computerConfiguration.fdcConfiguration.clearAddress = -1;
    computerConfiguration.fdcConfiguration.readInput = init_IoPort();
    computerConfiguration.fdcConfiguration.intrqInput = init_IoPort();
    computerConfiguration.fdcConfiguration.selectOutput = init_IoPort();
    computerConfiguration.fdcConfiguration.writeOutput = init_IoPort();

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
                    computerConfiguration.fdcConfiguration.readInput = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "intrq")
                    computerConfiguration.fdcConfiguration.intrqInput = parseXml_IoPort(*child);
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "select")
                    computerConfiguration.fdcConfiguration.selectOutput = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "write")
                    computerConfiguration.fdcConfiguration.writeOutput = parseXml_IoPort(*child);
            break;
                
            case TAG_EF:
                    computerConfiguration.fdcConfiguration.ef = parseXml_EfFlag(*child);
            break;
                
            case TAG_SIDES:
                computerConfiguration.fdcConfiguration.heads = (int)parseXml_Number(*child);
            break;

            case TAG_TRACKS:
                computerConfiguration.fdcConfiguration.tracks = (int)parseXml_Number(*child);;
            break;

            case TAG_SECTORS:
                computerConfiguration.fdcConfiguration.sectors = (int)parseXml_Number(*child);
            break;

            case TAG_SECTOR_LENGTH:
                computerConfiguration.fdcConfiguration.sectorLength = (int)parseXml_Number(*child);
            break;

            case TAG_MAX_FMT_COUNT:
                computerConfiguration.fdcConfiguration.maxFmtCount = (int)parseXml_Number(*child);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.fdcConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.fdcConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_FILENAME:
                diskNumber = (int)parseXml_Number(*child, "disk");
                floppy_[FDCTYPE_17XX][diskNumber] = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                diskNumber = (int)parseXml_Number(*child, "disk");
                floppyDir_[FDCTYPE_17XX][diskNumber] = dataDir_ + child->GetNodeContent();
                if (floppyDir_[FDCTYPE_17XX][diskNumber].Right(1) != pathSeparator_)
                    floppyDir_[FDCTYPE_17XX][diskNumber] += pathSeparator_;
            break;
                
            case TAG_CLEAR_ADDRESS:
                computerConfiguration.fdcConfiguration.clearAddress = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_Upd765(wxXmlNode &node)
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

    int tagTypeInt, diskNumber;
    
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.fdcType_ = FDCTYPE_UPD765;
    computerConfiguration.upd765Configuration.ioGroupVector.clear();
    computerConfiguration.upd765Configuration.defined = true;
    computerConfiguration.upd765Configuration.readStatus = init_IoPort();
    computerConfiguration.upd765Configuration.readCommand = init_IoPort();
    computerConfiguration.upd765Configuration.dmaControl = init_IoPort();
    computerConfiguration.upd765Configuration.dmaCount = init_IoPort();
    computerConfiguration.upd765Configuration.writeCommand = init_IoPort();
    computerConfiguration.upd765Configuration.efInterrupt = EfFlag();
    
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
                    computerConfiguration.upd765Configuration.readStatus = parseXml_IoPort(*child, UPD765_READ_STATUS_IN);
                if (child->GetAttribute("type") == "command")
                    computerConfiguration.upd765Configuration.readCommand = parseXml_IoPort(*child, UPD765_READ_COMMAND_IN);
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "control")
                    computerConfiguration.upd765Configuration.dmaControl = parseXml_IoPort(*child, UPD765_DMA_CONTROL_OUT);
                if (child->GetAttribute("type") == "count")
                    computerConfiguration.upd765Configuration.dmaCount = parseXml_IoPort(*child, UPD765_DMA_COUNT_OUT);
                if (child->GetAttribute("type") == "command")
                    computerConfiguration.upd765Configuration.writeCommand = parseXml_IoPort(*child, UPD765_WRITE_COMMAND_OUT);
            break;
                
            case TAG_EF:
                computerConfiguration.upd765Configuration.efInterrupt  = parseXml_EfFlag(*child, UPD765_EF);
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.upd765Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.upd765Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_FILENAME:
                diskNumber = (int)parseXml_Number(*child, "disk");
                floppy_[FDCTYPE_UPD765][diskNumber] = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                diskNumber = (int)parseXml_Number(*child, "disk");
                floppyDir_[FDCTYPE_UPD765][diskNumber] = dataDir_ + child->GetNodeContent();
                if (floppyDir_[FDCTYPE_UPD765][diskNumber].Right(1) != pathSeparator_)
                    floppyDir_[FDCTYPE_UPD765][diskNumber] += pathSeparator_;
            break;

            case TAG_HDNAME:
                diskNumber = (int)parseXml_Number(*child, "disk");
                floppyDirSwitched_[FDCTYPE_UPD765][diskNumber] = computerConfiguration.mainDir_ + child->GetNodeContent();
                if (floppyDirSwitched_[FDCTYPE_UPD765][diskNumber].Right(1) != pathSeparator_)
                    floppyDirSwitched_[FDCTYPE_UPD765][diskNumber] += pathSeparator_;
                if (child->GetAttribute("default") == "yes")
                    directoryMode_[FDCTYPE_UPD765][diskNumber] = true;
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

void XmlParser::parseXml_Tu58Disk(wxXmlNode &node)
{
    computerConfiguration.tu58Configuration.defined = true;
    computerConfiguration.fdcType_ = FDCTYPE_TU58_IDE;

    wxString tagList[]=
    {
        "filename",
        "dirname",
        "blocks",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_BLOCKS,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt, diskNumber = 0;
    wxString driveNumberStr;
    
    for (int drive=0; drive<2; drive++)
        computerConfiguration.tu58Configuration.numberOfBlocks[drive] = 0;
    
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
                diskNumber = (int)parseXml_Number(*child, "disk");
                floppy_[FDCTYPE_TU58_IDE][diskNumber+2] = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                diskNumber = (int)parseXml_Number(*child, "disk");
                floppyDir_[FDCTYPE_TU58_IDE][diskNumber+2] = dataDir_ + child->GetNodeContent();
                if (floppyDir_[FDCTYPE_TU58_IDE][diskNumber+2].Right(1) != pathSeparator_)
                    floppyDir_[FDCTYPE_TU58_IDE][diskNumber+2] += pathSeparator_;
            break;

            case TAG_BLOCKS:
                diskNumber = (int)parseXml_Number(*child, "disk");
                computerConfiguration.tu58Configuration.numberOfBlocks[diskNumber] = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_CoinVideo(wxXmlNode &node)
{
    if (!computerConfiguration.coinConfiguration.defined)
    {
        computerConfiguration.coinConfiguration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.coinConfiguration.defined = true;

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
    int red, green, blue, xpos, ypos;
    wxString color, scale, position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.coinConfiguration.xScale = 3;
    computerConfiguration.coinConfiguration.ioGroupVector.clear();
    computerConfiguration.zoom_[computerConfiguration.coinConfiguration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.coinConfiguration.videoNumber] = "Coin Video";

    computerConfiguration.cdp1861Configuration.pixieGraphics.interrupt = 62;
    computerConfiguration.cdp1861Configuration.pixieGraphics.start = 72;
    computerConfiguration.cdp1861Configuration.pixieGraphics.end = 199;
    computerConfiguration.cdp1861Configuration.pixieGraphics.screenend = 322;
    computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = 128;

    computerConfiguration.coinConfiguration.output = init_IoPort();

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
                computerConfiguration.coinConfiguration.output = parseXml_IoPort(*child, COIN_VIDEO_ENABLE_OUT);
            break;
                                
            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.coinConfiguration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOCOIN] = (int)width/computerConfiguration.coinConfiguration.xScale;
                    screenInfo.borderY[VIDEOCOIN] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.coinConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.coinConfiguration.defaultY;
                computerConfiguration.coinConfiguration.defaultX = xpos;
                computerConfiguration.coinConfiguration.defaultY = ypos;
            break;

            case TAG_XSCALE:
                screenInfo.borderX[VIDEOCOIN] = (int)screenInfo.borderX[VIDEOCOIN] * computerConfiguration.coinConfiguration.xScale;
                computerConfiguration.coinConfiguration.xScale = getDouble(child->GetNodeContent(), childName, -1, "", false);
                screenInfo.borderX[VIDEOCOIN] = (int)screenInfo.borderX[VIDEOCOIN] / computerConfiguration.coinConfiguration.xScale;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_PIXIE_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_PIXIE_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.coinConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.coinConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_GRAPHICS:
                parseXml_pixieGraphics (*child);
            break;

            case TAG_HEIGHT:
                computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_PixieVideo(wxXmlNode &node)
{
    if (!computerConfiguration.cdp1861Configuration.defined)
    {
        computerConfiguration.cdp1861Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.cdp1861Configuration.defined = true;

    wxString tagList[]=
    {
        "io",
        "out",
        "in",
        "ef",
        "iogroup",
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
        TAG_IOGROUP,
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
    int red, green, blue, xpos, ypos;
    wxString color, scale, position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.cdp1861Configuration.xScale = 3;
    computerConfiguration.cdp1861Configuration.ioGroupVector.clear();
    computerConfiguration.zoom_[computerConfiguration.cdp1861Configuration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.cdp1861Configuration.videoNumber] = "1861 Pixie";

    computerConfiguration.cdp1861Configuration.pixieGraphics.interrupt = 62;
    computerConfiguration.cdp1861Configuration.pixieGraphics.start = 64;
    computerConfiguration.cdp1861Configuration.pixieGraphics.end = 191;
    computerConfiguration.cdp1861Configuration.pixieGraphics.screenend = 262;
    computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = 128;
    
    computerConfiguration.cdp1861Configuration.input = init_IoPort();
    computerConfiguration.cdp1861Configuration.output = init_IoPort();
    computerConfiguration.cdp1861Configuration.ef = init_EfFlag();

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
                if (child->HasAttribute("type"))
                {
                    if (child->GetAttribute("type") == "on")
                        computerConfiguration.cdp1861Configuration.input = parseXml_IoPort(*child, CDP1861_ENABLE_IN);
                    if (child->GetAttribute("type") == "off")
                        computerConfiguration.cdp1861Configuration.output = parseXml_IoPort(*child, CDP1861_DISABLE_OUT);
                }
                else
                    computerConfiguration.cdp1861Configuration.input = parseXml_IoPort(*child, CDP1861_ENABLE_IN);
                computerConfiguration.cdp1861Configuration.doubleScreenIo = PIXIE_DOUBLE;
            break;
                
            case TAG_IN:
                if (child->HasAttribute("type"))
                {
                    computerConfiguration.cdp1861Configuration.doubleScreenIo = PIXIE_IN_IN;
                    if (child->GetAttribute("type") == "on")
                        computerConfiguration.cdp1861Configuration.input = parseXml_IoPort(*child, CDP1861_ENABLE_IN);
                    if (child->GetAttribute("type") == "off")
                        computerConfiguration.cdp1861Configuration.output = parseXml_IoPort(*child, CDP1861_DISABLE_OUT);
                }
                else
                    computerConfiguration.cdp1861Configuration.input = parseXml_IoPort(*child, CDP1861_ENABLE_IN);
            break;
                
            case TAG_OUT:
                if (child->HasAttribute("type"))
                {
                    computerConfiguration.cdp1861Configuration.doubleScreenIo = PIXIE_OUT_OUT;
                    if (child->GetAttribute("type") == "on")
                        computerConfiguration.cdp1861Configuration.input = parseXml_IoPort(*child, CDP1861_ENABLE_IN);
                    if (child->GetAttribute("type") == "off")
                        computerConfiguration.cdp1861Configuration.output = parseXml_IoPort(*child, CDP1861_DISABLE_OUT);
                }
                else
                    computerConfiguration.cdp1861Configuration.output = parseXml_IoPort(*child, CDP1861_DISABLE_OUT);
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "on")
                    computerConfiguration.cdp1861Configuration.efScreenOn = true;
                computerConfiguration.cdp1861Configuration.ef = parseXml_EfFlag(*child, CDP1861_IN_FRAME_EF);
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.cdp1861Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.cdp1861Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.cdp1861Configuration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOXMLPIXIE] = (int)width/computerConfiguration.cdp1861Configuration.xScale;
                    screenInfo.borderY[VIDEOXMLPIXIE] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.cdp1861Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.cdp1861Configuration.defaultY;
                computerConfiguration.cdp1861Configuration.defaultX = xpos;
                computerConfiguration.cdp1861Configuration.defaultY = ypos;
            break;

            case TAG_XSCALE:
                screenInfo.borderX[VIDEOXMLPIXIE] = (int)screenInfo.borderX[VIDEOXMLPIXIE] * computerConfiguration.cdp1861Configuration.xScale;
                computerConfiguration.cdp1861Configuration.xScale = getDouble(child->GetNodeContent(), childName, -1, "", false);
                screenInfo.borderX[VIDEOXMLPIXIE] = (int)screenInfo.borderX[VIDEOXMLPIXIE] / computerConfiguration.cdp1861Configuration.xScale;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_PIXIE_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "fore1")
                    screenInfo.defaultColour[COL_PIXIE_VISICOM_COL1].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "fore2")
                    screenInfo.defaultColour[COL_PIXIE_VISICOM_COL2].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "fore3")
                    screenInfo.defaultColour[COL_PIXIE_VISICOM_COL3].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_PIXIE_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_COLOR_TYPE:
                if (child->GetNodeContent() == "vis")
                    computerConfiguration.cdp1861Configuration.colorType = PIXIE_COLOR_VISICOM;
            break;

            case TAG_GRAPHICS:
                parseXml_pixieGraphics (*child);
            break;

            case TAG_HIGH_RES:
                computerConfiguration.cdp1861Configuration.highRes = true;
            break;

            case TAG_HEIGHT:
                computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_1862Video(wxXmlNode &node)
{
    computerConfiguration.cdp1862Configuration.defined = true;

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
    bool whiteDefined = false, blackDefined = false;
    wxString color, scale, position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.cdp1862Configuration.ioGroupVector.clear();
    computerConfiguration.cdp1862Configuration.background = init_IoPort();
    computerConfiguration.cdp1862Configuration.colorMemory = init_IoPort();
    computerConfiguration.cdp1862Configuration.startRam = -1;
    computerConfiguration.cdp1862Configuration.endRam = -1;

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
                    computerConfiguration.cdp1862Configuration.startRam = (int)start;
                    computerConfiguration.cdp1862Configuration.endRam = (int)end;
                }
                if (child->HasAttribute("mask"))
                    computerConfiguration.ioMask[CDP1862_COLORRAM_OUT] = (int)parseXml_Number(*child, "mask") & 0xff;
            break;

            case TAG_COLOR_TYPE:
                if (child->GetNodeContent() == "vip")
                    computerConfiguration.cdp1862Configuration.colorType = PIXIE_COLOR_VIP_1862;
                if (child->GetNodeContent() == "eti" || child->GetNodeContent() == "hug" || child->GetNodeContent() == "hec")
                    computerConfiguration.cdp1862Configuration.colorType = PIXIE_COLOR_ETI_1862;
                if (child->GetNodeContent() == "vic" || child->GetNodeContent() == "vip")
                {
                    if (child->GetNodeContent() == "vip")
                        computerConfiguration.cdp1862Configuration.colorType = PIXIE_COLOR_VIP_1862;
                    else
                        computerConfiguration.cdp1862Configuration.colorType = PIXIE_COLOR_VICTORY_1862;
                    if (whiteDefined && blackDefined)
                    {
                        color = screenInfo.defaultColour[COL_CDP1862_BLACK];
                        screenInfo.defaultColour[COL_CDP1862_BLACK] = screenInfo.defaultColour[COL_CDP1862_WHITE];
                        screenInfo.defaultColour[COL_CDP1862_WHITE] = color;
                    }
                    else
                    {
                        if (whiteDefined)
                            screenInfo.defaultColour[COL_CDP1862_BLACK] = screenInfo.defaultColour[COL_CDP1862_WHITE];
                        if (blackDefined)
                            screenInfo.defaultColour[COL_CDP1862_WHITE] = screenInfo.defaultColour[COL_CDP1862_BLACK];
                    }
                }

                if (child->GetNodeContent() == "tmc")
                    computerConfiguration.cdp1862Configuration.colorType = PIXIE_COLOR_TMC2000_1862;
                if (child->GetNodeContent() == "cos")
                    computerConfiguration.cdp1862Configuration.colorType = PIXIE_COLOR_DEFAULT;
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "back")
                    computerConfiguration.cdp1862Configuration.background = parseXml_IoPort(*child, CDP1862_BACKGROUND_OUT);
                if (child->GetAttribute("type") == "color_ram")
                    computerConfiguration.cdp1862Configuration.colorMemory = parseXml_IoPort(*child, CDP1862_COLORRAM_OUT);
            break;
                
            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_CDP1862_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_CDP1862_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);

                if (child->GetAttribute("type") == "red")
                    screenInfo.defaultColour[COL_CDP1862_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "blue")
                    screenInfo.defaultColour[COL_CDP1862_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "magenta")
                    screenInfo.defaultColour[COL_CDP1862_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "green")
                    screenInfo.defaultColour[COL_CDP1862_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "yellow")
                    screenInfo.defaultColour[COL_CDP1862_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "cyan")
                    screenInfo.defaultColour[COL_CDP1862_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_blue")
                    screenInfo.defaultColour[COL_CDP1862_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_black")
                    screenInfo.defaultColour[COL_CDP1862_BACK_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_green")
                    screenInfo.defaultColour[COL_CDP1862_BACK_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_red")
                    screenInfo.defaultColour[COL_CDP1862_BACK_RED].Printf("#%02X%02X%02X", red, green, blue);

                if (computerConfiguration.cdp1862Configuration.colorType == PIXIE_COLOR_VICTORY_1862 || computerConfiguration.cdp1862Configuration.colorType == PIXIE_COLOR_VIP_1862)
                {
                    if (child->GetAttribute("type") == "white")
                        screenInfo.defaultColour[COL_CDP1862_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                    if (child->GetAttribute("type") == "black")
                        screenInfo.defaultColour[COL_CDP1862_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                }
                else
                {
                    if (child->GetAttribute("type") == "white")
                    {
                        whiteDefined = true;
                        screenInfo.defaultColour[COL_CDP1862_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                    }
                    if (child->GetAttribute("type") == "black")
                    {
                        blackDefined = true;
                        screenInfo.defaultColour[COL_CDP1862_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                    }
                }
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.cdp1862Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.cdp1862Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_1864Video(wxXmlNode &node)
{
    if (!computerConfiguration.cdp1864Configuration.defined)
    {
        computerConfiguration.cdp1864Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.cdp1864Configuration.defined = true;

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
    long width, height, start, end;
    int red, green, blue, xpos, ypos;
    bool whiteDefined = false, blackDefined = false;
    wxString color, scale, position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.cdp1864Configuration.xScale = 4;
    computerConfiguration.cdp1864Configuration.ioGroupVector.clear();
    computerConfiguration.zoom_[computerConfiguration.cdp1864Configuration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.cdp1864Configuration.videoNumber] = "CDP1864";
    computerConfiguration.cdp1864Configuration.disable = init_IoPort();
    computerConfiguration.cdp1864Configuration.enable = init_IoPort();
    computerConfiguration.cdp1864Configuration.toneLatch = init_IoPort();
    computerConfiguration.cdp1864Configuration.colorMemory = init_IoPort();
    computerConfiguration.cdp1864Configuration.background = init_IoPort();
    computerConfiguration.cdp1864Configuration.startRam = -1;
    computerConfiguration.cdp1864Configuration.ramMask = 0xff;
    computerConfiguration.cdp1864Configuration.colorLatch = false;
    computerConfiguration.cdp1864Configuration.endRam = -1;

    computerConfiguration.cdp1861Configuration.pixieGraphics.interrupt = 62;
    computerConfiguration.cdp1861Configuration.pixieGraphics.start = 64;
    computerConfiguration.cdp1861Configuration.pixieGraphics.end = 191;
    computerConfiguration.cdp1861Configuration.pixieGraphics.screenend = 262;
    computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = 192;

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
                if (child->HasAttribute("type"))
                {
                    if (child->GetAttribute("type") == "off")
                        computerConfiguration.cdp1864Configuration.disable = parseXml_IoPort(*child, CDP1864_DISABLE_IN);
                    if (child->GetAttribute("type") == "on")
                        computerConfiguration.cdp1864Configuration.enable = parseXml_IoPort(*child, CDP1864_ENABLE_IN);
                }
                else
                    computerConfiguration.cdp1864Configuration.enable = parseXml_IoPort(*child, CDP1864_ENABLE_IN);
            break;
                
            case TAG_COLOR_TYPE:
                if (child->GetNodeContent() == "eti" || child->GetNodeContent() == "hug" || child->GetNodeContent() == "hec")
                    computerConfiguration.cdp1864Configuration.colorType = PIXIE_COLOR_ETI_1864;
                if (child->GetNodeContent() == "vic" || child->GetNodeContent() == "vip")
                {
                    if (child->GetNodeContent() == "vip")
                        computerConfiguration.cdp1864Configuration.colorType = PIXIE_COLOR_VIP_1864;
                    else
                        computerConfiguration.cdp1864Configuration.colorType = PIXIE_COLOR_VICTORY_1864;
                    if (whiteDefined && blackDefined)
                    {
                        color = screenInfo.defaultColour[COL_CDP1864_BLACK];
                        screenInfo.defaultColour[COL_CDP1864_BLACK] = screenInfo.defaultColour[COL_CDP1864_WHITE];
                        screenInfo.defaultColour[COL_CDP1864_WHITE] = color;
                    }
                    else
                    {
                        if (whiteDefined)
                            screenInfo.defaultColour[COL_CDP1864_BLACK] = screenInfo.defaultColour[COL_CDP1864_WHITE];
                        if (blackDefined)
                            screenInfo.defaultColour[COL_CDP1864_WHITE] = screenInfo.defaultColour[COL_CDP1864_BLACK];
                    }
                }
                if (child->GetNodeContent() == "tmc")
                    computerConfiguration.cdp1864Configuration.colorType = PIXIE_COLOR_TMC2000_1864;
                if (child->GetNodeContent() == "cos")
                    computerConfiguration.cdp1864Configuration.colorType = PIXIE_COLOR_DEFAULT;
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "back")
                    computerConfiguration.cdp1864Configuration.background = parseXml_IoPort(*child, CDP1864_BACKGROUND_OUT);
                if (child->GetAttribute("type") == "tone")
                {
                    computerConfiguration.soundConfiguration.type = SOUND_1863_1864;
                    computerConfiguration.cdp1864Configuration.toneLatch = parseXml_IoPort(*child, CDP1864_TONE_LATCH_OUT);
                }
                if (child->GetAttribute("type") == "color_ram")
                    computerConfiguration.cdp1864Configuration.colorMemory = parseXml_IoPort(*child, CDP1864_COLORRAM_OUT);
                if (child->GetAttribute("type") == "color_tone")
                {
                    computerConfiguration.cdp1864Configuration.colorLatch = true;
                    computerConfiguration.soundConfiguration.type = SOUND_1863_1864;
                    computerConfiguration.cdp1864Configuration.toneLatch = parseXml_IoPort(*child, CDP1864_TONE_LATCH_OUT);
                }
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "on")
                    computerConfiguration.cdp1864Configuration.screenOn = true;
                computerConfiguration.cdp1864Configuration.ef = (int)parseXml_Number(*child);
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
                    computerConfiguration.cdp1864Configuration.startRam = (int)start;
                    computerConfiguration.cdp1864Configuration.endRam = (int)end;
                    computerConfiguration.cdp1864Configuration.ramMask = (end - start) & 0xff;
                }
                if (child->HasAttribute("mask"))
                    computerConfiguration.cdp1864Configuration.colorMemory.mask = (int)parseXml_Number(*child, "mask") & 0xff;
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.cdp1864Configuration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOXML1864] = (int)width/computerConfiguration.cdp1864Configuration.xScale;
                    screenInfo.borderY[VIDEOXML1864] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.cdp1864Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.cdp1864Configuration.defaultY;
                computerConfiguration.cdp1864Configuration.defaultX = xpos;
                computerConfiguration.cdp1864Configuration.defaultY = ypos;
            break;

            case TAG_XSCALE:
                screenInfo.borderX[VIDEOXML1864] = (int)screenInfo.borderX[VIDEOXML1864] * computerConfiguration.cdp1864Configuration.xScale;
                computerConfiguration.cdp1864Configuration.xScale = getDouble(child->GetNodeContent(), childName, -1, "", false);
                screenInfo.borderX[VIDEOXML1864] = (int)screenInfo.borderX[VIDEOXML1864] / computerConfiguration.cdp1864Configuration.xScale;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_CDP1864_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_CDP1864_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);

                if (child->GetAttribute("type") == "red")
                    screenInfo.defaultColour[COL_CDP1864_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "blue")
                    screenInfo.defaultColour[COL_CDP1864_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "magenta")
                    screenInfo.defaultColour[COL_CDP1864_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "green")
                    screenInfo.defaultColour[COL_CDP1864_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "yellow")
                    screenInfo.defaultColour[COL_CDP1864_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "cyan")
                    screenInfo.defaultColour[COL_CDP1864_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_blue")
                    screenInfo.defaultColour[COL_CDP1864_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_black")
                    screenInfo.defaultColour[COL_CDP1864_BACK_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_green")
                    screenInfo.defaultColour[COL_CDP1864_BACK_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_red")
                    screenInfo.defaultColour[COL_CDP1864_BACK_RED].Printf("#%02X%02X%02X", red, green, blue);

                if (computerConfiguration.cdp1864Configuration.colorType == PIXIE_COLOR_VICTORY_1864 || computerConfiguration.cdp1862Configuration.colorType == PIXIE_COLOR_VIP_1864)
                {
                    if (child->GetAttribute("type") == "white")
                        screenInfo.defaultColour[COL_CDP1864_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                    if (child->GetAttribute("type") == "black")
                        screenInfo.defaultColour[COL_CDP1864_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                }
                else
                {
                    if (child->GetAttribute("type") == "white")
                    {
                        whiteDefined = true;
                        screenInfo.defaultColour[COL_CDP1864_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                    }
                    if (child->GetAttribute("type") == "black")
                    {
                        blackDefined = true;
                        screenInfo.defaultColour[COL_CDP1864_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                    }
                }
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.cdp1864Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.cdp1864Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_GRAPHICS:
                parseXml_pixieGraphics (*child);
            break;

            case TAG_HEIGHT:
                computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_St4Video(wxXmlNode &node)
{
    if (!computerConfiguration.studio4VideoConfiguration.defined)
    {
        computerConfiguration.studio4VideoConfiguration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.studio4VideoConfiguration.defined = true;

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
    int red, green, blue, xpos, ypos;
    wxString color, scale, position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.studio4VideoConfiguration.xScale = 4;
    computerConfiguration.studio4VideoConfiguration.ioGroupVector.clear();
    computerConfiguration.zoom_[computerConfiguration.studio4VideoConfiguration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.studio4VideoConfiguration.videoNumber] = "Studio IV";
    computerConfiguration.studio4VideoConfiguration.dmaEnable = init_IoPort();
    computerConfiguration.studio4VideoConfiguration.output = init_IoPort();
    computerConfiguration.studio4VideoConfiguration.ef = init_EfFlag();
    computerConfiguration.studio4VideoConfiguration.startRam = -1;
    computerConfiguration.studio4VideoConfiguration.endRam = -1;

    computerConfiguration.cdp1861Configuration.pixieGraphics.interrupt = 62;
    computerConfiguration.cdp1861Configuration.pixieGraphics.start = 64;
    computerConfiguration.cdp1861Configuration.pixieGraphics.end = 191;
    computerConfiguration.cdp1861Configuration.pixieGraphics.screenend = 262;
    computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = 192;

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
                    computerConfiguration.studio4VideoConfiguration.output = parseXml_IoPort(*child, STUDIOIV_VIDEO_OUT);
                if (child->GetAttribute("type") == "dma")
                    computerConfiguration.studio4VideoConfiguration.dmaEnable = parseXml_IoPort(*child, STUDIOIV_VIDEO_DMA_ENABLE_OUT);
            break;
                
            case TAG_EF:
                computerConfiguration.studio4VideoConfiguration.ef = parseXml_EfFlag(*child);
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
                    computerConfiguration.studio4VideoConfiguration.startRam = (int)start;
                    computerConfiguration.studio4VideoConfiguration.endRam = (int)end;
                }
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.studio4VideoConfiguration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOSTUDIOIV] = (int)width/computerConfiguration.studio4VideoConfiguration.xScale;
                    screenInfo.borderY[VIDEOSTUDIOIV] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.studio4VideoConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.studio4VideoConfiguration.defaultY;
                computerConfiguration.studio4VideoConfiguration.defaultX = xpos;
                computerConfiguration.studio4VideoConfiguration.defaultY = ypos;
            break;

            case TAG_XSCALE:
                screenInfo.borderX[VIDEOSTUDIOIV] = (int)screenInfo.borderX[VIDEOSTUDIOIV] * computerConfiguration.studio4VideoConfiguration.xScale;
                computerConfiguration.studio4VideoConfiguration.xScale = getDouble(child->GetNodeContent(), childName, -1, "", false);
                screenInfo.borderX[VIDEOSTUDIOIV] = (int)screenInfo.borderX[VIDEOSTUDIOIV] / computerConfiguration.studio4VideoConfiguration.xScale;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "black")
                    screenInfo.defaultColour[COL_ST4_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "red")
                    screenInfo.defaultColour[COL_ST4_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "blue")
                    screenInfo.defaultColour[COL_ST4_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "magenta")
                    screenInfo.defaultColour[COL_ST4_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "green")
                    screenInfo.defaultColour[COL_ST4_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "yellow")
                    screenInfo.defaultColour[COL_ST4_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "cyan")
                    screenInfo.defaultColour[COL_ST4_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "white")
                    screenInfo.defaultColour[COL_ST4_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_black")
                    screenInfo.defaultColour[COL_ST4_BACK_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_red")
                    screenInfo.defaultColour[COL_ST4_BACK_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_blue")
                    screenInfo.defaultColour[COL_ST4_BACK_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_magenta")
                    screenInfo.defaultColour[COL_ST4_BACK_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_green")
                    screenInfo.defaultColour[COL_ST4_BACK_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_yellow")
                    screenInfo.defaultColour[COL_ST4_BACK_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_cyan")
                    screenInfo.defaultColour[COL_ST4_BACK_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back_white")
                    screenInfo.defaultColour[COL_ST4_BACK_WHITE].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.studio4VideoConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.studio4VideoConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_GRAPHICS:
                parseXml_pixieGraphics (*child);
            break;

            case TAG_HEIGHT:
                computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_Vip2KVideo(wxXmlNode &node)
{
    if (!computerConfiguration.vip2KVideoConfiguration.defined)
    {
        computerConfiguration.vip2KVideoConfiguration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.vip2KVideoConfiguration.defined = true;

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
        "iogroup",
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
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, scale, position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.vip2KVideoConfiguration.xScale = 1.5;
    computerConfiguration.vip2KVideoConfiguration.ioGroupVector.clear();
    computerConfiguration.zoom_[computerConfiguration.vip2KVideoConfiguration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.vip2KVideoConfiguration.videoNumber] = "VIP2K";
    computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = 198;
    computerConfiguration.vip2KVideoConfiguration.doubleScreenIo = false;
    computerConfiguration.vip2KVideoConfiguration.output = init_IoPort();
    computerConfiguration.vip2KVideoConfiguration.input = init_IoPort();
    computerConfiguration.vip2KVideoConfiguration.ef = init_EfFlag();

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
                if (child->HasAttribute("type"))
                {
                    if (child->GetAttribute("type") == "on")
                        computerConfiguration.vip2KVideoConfiguration.input = parseXml_IoPort(*child, VIP2K_VIDEO_ENABLE_IN);
                    if (child->GetAttribute("type") == "off")
                        computerConfiguration.vip2KVideoConfiguration.output = parseXml_IoPort(*child, VIP2K_VIDEO_DISABLE_OUT);
                }
                else
                    computerConfiguration.vip2KVideoConfiguration.input = parseXml_IoPort(*child, VIP2K_VIDEO_ENABLE_IN);
                computerConfiguration.vip2KVideoConfiguration.doubleScreenIo = true;
            break;
                
            case TAG_IN:
                if (child->HasAttribute("type"))
                {
                    if (child->GetAttribute("type") == "off")
                        computerConfiguration.vip2KVideoConfiguration.input = parseXml_IoPort(*child, VIP2K_VIDEO_DISABLE_OUT);
                    if (child->GetAttribute("type") == "on")
                        computerConfiguration.vip2KVideoConfiguration.input = parseXml_IoPort(*child, VIP2K_VIDEO_ENABLE_IN);
                }
                else
                    computerConfiguration.vip2KVideoConfiguration.input = parseXml_IoPort(*child, VIP2K_VIDEO_ENABLE_IN);
            break;
                
            case TAG_OUT:
                if (child->HasAttribute("type"))
                {
                    if (child->GetAttribute("type") == "off")
                        computerConfiguration.vip2KVideoConfiguration.input = parseXml_IoPort(*child, VIP2K_VIDEO_DISABLE_OUT);
                    if (child->GetAttribute("type") == "on")
                        computerConfiguration.vip2KVideoConfiguration.input = parseXml_IoPort(*child, VIP2K_VIDEO_ENABLE_IN);
                }
                else
                    computerConfiguration.vip2KVideoConfiguration.output = parseXml_IoPort(*child, VIP2K_VIDEO_DISABLE_OUT);
            break;
                
            case TAG_EF:
                computerConfiguration.vip2KVideoConfiguration.ef = parseXml_EfFlag(*child);
            break;
                
            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.vip2KVideoConfiguration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOVIP2K] = (int)width/computerConfiguration.vip2KVideoConfiguration.xScale;
                    screenInfo.borderY[VIDEOVIP2K] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.vip2KVideoConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.vip2KVideoConfiguration.defaultY;
                computerConfiguration.vip2KVideoConfiguration.defaultX = xpos;
                computerConfiguration.vip2KVideoConfiguration.defaultY = ypos;
            break;

            case TAG_XSCALE: 
                screenInfo.borderX[VIDEOVIP2K] = (int)screenInfo.borderX[VIDEOVIP2K] * computerConfiguration.vip2KVideoConfiguration.xScale;
                computerConfiguration.vip2KVideoConfiguration.xScale = getDouble(child->GetNodeContent(), childName, -1, "", false);
                screenInfo.borderX[VIDEOVIP2K] = (int)screenInfo.borderX[VIDEOVIP2K] / computerConfiguration.vip2KVideoConfiguration.xScale;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_PIXIE_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_PIXIE_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_HEIGHT:
                computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = (int)parseXml_Number(*child);
            break;

            case TAG_WIDTH:
                computerConfiguration.cdp1861Configuration.pixieGraphics.videoWidth = (int)parseXml_Number(*child);
            break;

            case TAG_FILENAME:
                computerConfiguration.vip2KVideoConfiguration.sequencerFile = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.vip2KVideoConfiguration.sequencerDirectory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.vip2KVideoConfiguration.sequencerDirectory.Right(1) != pathSeparator_)
                    computerConfiguration.vip2KVideoConfiguration.sequencerDirectory += pathSeparator_;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.vip2KVideoConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.vip2KVideoConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_fredVideo(wxXmlNode &node)
{
    if (!computerConfiguration.fredVideoConfiguration.defined)
    {
        computerConfiguration.fredVideoConfiguration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.fredVideoConfiguration.defined = true;

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
    wxString color, scale, position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.fredVideoConfiguration.xScale = 1;
    computerConfiguration.fredVideoConfiguration.ioGroupVector.clear();
    computerConfiguration.zoom_[computerConfiguration.fredVideoConfiguration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.fredVideoConfiguration.videoNumber] = "FRED";
    computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = 128;
    computerConfiguration.cdp1861Configuration.pixieGraphics.videoWidth = 192;

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
                computerConfiguration.fredVideoConfiguration.output = parseXml_IoPort(*child, FRED_VIDEO_TYPE_OUT);
            break;
                
            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.fredVideoConfiguration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOFRED] = (int)width/computerConfiguration.fredVideoConfiguration.xScale;
                    screenInfo.borderY[VIDEOFRED] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.fredVideoConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.fredVideoConfiguration.defaultY;
                computerConfiguration.fredVideoConfiguration.defaultX = xpos;
                computerConfiguration.fredVideoConfiguration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_PIXIE_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_PIXIE_BACK].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_XSCALE:
                screenInfo.borderX[VIDEOFRED] = (int)screenInfo.borderX[VIDEOFRED] * computerConfiguration.fredVideoConfiguration.xScale;
                computerConfiguration.fredVideoConfiguration.xScale = getDouble(child->GetNodeContent(), childName, -1, "", false);
                screenInfo.borderX[VIDEOFRED] = (int)screenInfo.borderX[VIDEOFRED] / computerConfiguration.fredVideoConfiguration.xScale;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.fredVideoConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.fredVideoConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_HEIGHT:
                computerConfiguration.cdp1861Configuration.pixieGraphics.videoHeight = (int)parseXml_Number(*child);
            break;

            case TAG_WIDTH:
                computerConfiguration.cdp1861Configuration.pixieGraphics.videoWidth = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_pixieGraphics(wxXmlNode &node)
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
                computerConfiguration.cdp1861Configuration.pixieGraphics.interrupt = (int)parseXml_Number(*child);
            break;

            case TAG_START:
                computerConfiguration.cdp1861Configuration.pixieGraphics.start = (int)parseXml_Number(*child);
            break;
                
            case TAG_END:
                computerConfiguration.cdp1861Configuration.pixieGraphics.end = (int)parseXml_Number(*child);
            break;
 
            case TAG_SCREENEND:
                computerConfiguration.cdp1861Configuration.pixieGraphics.screenend = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_MC6845Video(wxXmlNode &node)
{
    long start, end;
    long width, height;

    if (!computerConfiguration.mc6845Configuration.defined)
    {
        computerConfiguration.mc6845Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.mc6845Configuration.defined = true;

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

    computerConfiguration.mc6845Configuration.address = -1;
    computerConfiguration.mc6845Configuration.addressMask = 0xFFFF;
    computerConfiguration.mc6845Configuration.data = -1;
    computerConfiguration.mc6845Configuration.dataMask = 0xFFFF;
    computerConfiguration.mc6845Configuration.ef = init_EfFlag();
    computerConfiguration.zoom_[computerConfiguration.mc6845Configuration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.mc6845Configuration.videoNumber] = "MC6845";
    computerConfiguration.mc6845Configuration.charSize.x = 8;
    computerConfiguration.mc6845Configuration.charSize.y = 8;
    computerConfiguration.mc6845Configuration.screenSize.x = 64;
    computerConfiguration.mc6845Configuration.screenSize.y = 16;
    computerConfiguration.mc6845Configuration.ioGroupVector.clear();

    int tagTypeInt;
    Word mask;
    int red, green, blue, xpos, ypos;
    wxString color, position, iogroup;
    size_t ioGroupNumber = 0;

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
                    computerConfiguration.mc6845Configuration.startRam = (Word)start;
                    computerConfiguration.mc6845Configuration.endRam = (Word)end;
                    mask = (Word)end - start;
                    
                    computerConfiguration.mc6845Configuration.ramMask = 0x3FFF;
                    while ((mask & 0x2000) == 0)
                    {
                        computerConfiguration.mc6845Configuration.ramMask = computerConfiguration.mc6845Configuration.ramMask >> 1;
                        mask = mask << 1;
                    }
                }
             break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "register")
                {
                    computerConfiguration.mc6845Configuration.address = (int)parseXml_Number(*child);
                    computerConfiguration.mc6845Configuration.addressMask = (int)parseXml_Number(*child, "mask");
                    if (computerConfiguration.mc6845Configuration.addressMask == 0)
                        computerConfiguration.mc6845Configuration.addressMask = 0xFFFF;
                }
                if (child->GetAttribute("type") == "data")
                {
                    computerConfiguration.mc6845Configuration.data = (int)parseXml_Number(*child);
                    computerConfiguration.mc6845Configuration.dataMask = (int)parseXml_Number(*child, "mask");
                    if (computerConfiguration.mc6845Configuration.dataMask == 0)
                        computerConfiguration.mc6845Configuration.dataMask = 0xFFFF;
                }
           break;
                
            case TAG_FONT:
                computerConfiguration.characterRomConfiguration.fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.characterRomConfiguration.directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.characterRomConfiguration.directory.Right(1) != pathSeparator_)
                    computerConfiguration.characterRomConfiguration.directory += pathSeparator_;
            break;

            case TAG_EF:
                computerConfiguration.mc6845Configuration.ef = parseXml_EfFlag(*child, MC6845_EF);
            break;
                
            case TAG_INTERLACE:
                computerConfiguration.interlace_ = true;
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.mc6845Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.mc6845Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.mc6845Configuration.videoNumber] = child->GetNodeContent();
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
                    computerConfiguration.mc6845Configuration.charSize.x = (int)width;
                    computerConfiguration.mc6845Configuration.charSize.y = (int)height;
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
                    computerConfiguration.mc6845Configuration.screenSize.x = (int)width;
                    computerConfiguration.mc6845Configuration.screenSize.y = (int)height;
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
                    screenInfo.borderX[VIDEOXML6845] = (int)width;
                    screenInfo.borderY[VIDEOXML6845] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.mc6845Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.mc6845Configuration.defaultY;
                computerConfiguration.mc6845Configuration.defaultX = xpos;
                computerConfiguration.mc6845Configuration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_MC6845_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_MC6845_BACK].Printf("#%02X%02X%02X", red, green, blue);
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

void XmlParser::parseXml_MC6847Video(wxXmlNode &node)
{
    long start, end;
    long width, height;

    if (!computerConfiguration.mc6847Configuration.defined)
    {
        computerConfiguration.mc6847Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }
    
    computerConfiguration.mc6847Configuration.defined = true;

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
        "iogroup",
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
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    int red, green, blue, xpos, ypos;
    wxString color, position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.mc6847Configuration.ioGroupVector.clear();
    computerConfiguration.mc6847Configuration.b7 = 0;
    computerConfiguration.mc6847Configuration.b6 = 0;
    computerConfiguration.mc6847Configuration.b5 = 0;
    computerConfiguration.mc6847Configuration.b4 = 0;
    computerConfiguration.mc6847Configuration.b3 = 0;
    computerConfiguration.mc6847Configuration.b2 = 0;
    computerConfiguration.mc6847Configuration.b1 = 0;
    computerConfiguration.mc6847Configuration.b0 = 0;
    computerConfiguration.mc6847Configuration.dd7 = 0;
    computerConfiguration.mc6847Configuration.dd6 = 0;

    computerConfiguration.mc6847Configuration.forceHighAg = false;
    computerConfiguration.mc6847Configuration.forceHighAs = false;
    computerConfiguration.mc6847Configuration.forceHighExt = false;
    computerConfiguration.mc6847Configuration.forceHighGm2 = false;
    computerConfiguration.mc6847Configuration.forceHighGm1 = false;
    computerConfiguration.mc6847Configuration.forceHighGm0 = false;
    computerConfiguration.mc6847Configuration.forceHighCss = false;
    computerConfiguration.mc6847Configuration.forceHighInv = false;

    computerConfiguration.mc6847Configuration.screenHeight = 192;
    computerConfiguration.zoom_[computerConfiguration.mc6847Configuration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.mc6847Configuration.videoNumber] = "MC6847";

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
                    computerConfiguration.mc6847Configuration.startRam = (int)start;
                    computerConfiguration.mc6847Configuration.endRam = (int)end;
                }
            break;
                
            case TAG_OUT:
                if (!parseXml_Range(*child, &start, &end))
                {
                    computerConfiguration.mc6847Configuration.outputMode = 0;
                    computerConfiguration.mc6847Configuration.output = parseXml_IoPort(*child, MC6847_OUT);
                }
                else
                {
                    computerConfiguration.mc6847Configuration.outputMode = 1;
                    computerConfiguration.mc6847Configuration.outputStart = (Word) start;
                    computerConfiguration.mc6847Configuration.outputEnd = (Word) end;
                }
            break;
 
            case TAG_FONT:
                computerConfiguration.characterRomConfiguration.fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.characterRomConfiguration.directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.characterRomConfiguration.directory.Right(1) != pathSeparator_)
                    computerConfiguration.characterRomConfiguration.directory += pathSeparator_;
            break;

            case TAG_INV:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighInv = true;
                parseXml_Mc6857BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_EXT:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighExt = true;
                parseXml_Mc6857BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_CSS:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighCss = true;
                parseXml_Mc6857BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_AS:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighAs = true;
                parseXml_Mc6857BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_AG:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighAg = true;
                parseXml_Mc6857BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_GM0:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighGm0 = true;
                parseXml_Mc6857BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_GM1:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighGm1 = true;
                parseXml_Mc6857BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_GM2:
                if (child->GetAttribute("init") == "high")
                    computerConfiguration.mc6847Configuration.forceHighGm2 = true;
                parseXml_Mc6857BitSetup(*child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.mc6847Configuration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOXML6847] = (int)width;
                    screenInfo.borderY[VIDEOXML6847] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.mc6847Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.mc6847Configuration.defaultY;
                computerConfiguration.mc6847Configuration.defaultX = xpos;
                computerConfiguration.mc6847Configuration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "text_back")
                    screenInfo.defaultColour[COL_MC6847_TEXT_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "text_green")
                    screenInfo.defaultColour[COL_MC6847_TEXT_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "text_orange")
                    screenInfo.defaultColour[COL_MC6847_TEXT_ORANGE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_green")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_yellow")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_blue")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_red")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_buff")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_BUFF].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_cyan")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_CYAN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_magenta")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_MAGENTA].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "graph_orange")
                    screenInfo.defaultColour[COL_MC6847_GRAPH_ORANGE].Printf("#%02X%02X%02X", red, green, blue);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.mc6847Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.mc6847Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_TMS9918Video(wxXmlNode &node)
{
    if (!computerConfiguration.tmsConfiguration.defined)
    {
        computerConfiguration.tmsConfiguration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.tmsConfiguration.defined = true;

    wxString tagList[]=
    {
        "out",
        "ef",
        "zoom",
        "border",
        "pos",
        "color",
        "iogroup",
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
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.tmsConfiguration.ioGroupVector.clear();
    computerConfiguration.zoom_[computerConfiguration.tmsConfiguration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.tmsConfiguration.videoNumber] = "TMS 9918";
    computerConfiguration.tmsConfiguration.efInterrupt = init_EfFlag();
    computerConfiguration.tmsConfiguration.registerOutput = init_IoPort();
    computerConfiguration.tmsConfiguration.dataOutput = init_IoPort();

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
                    computerConfiguration.tmsConfiguration.registerOutput = parseXml_IoPort(*child, TMS_REGISTER_PORT_OUT);
                if (child->GetAttribute("type") == "data")
                    computerConfiguration.tmsConfiguration.dataOutput = parseXml_IoPort(*child, TMS_DATA_PORT_OUT);
           break;
                
            case TAG_EF:
                    computerConfiguration.tmsConfiguration.efInterrupt = parseXml_EfFlag(*child, TMS_INTERRUPT_EF);
            break;
                
            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.tmsConfiguration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOXMLTMS] = (int)width;
                    screenInfo.borderY[VIDEOXMLTMS] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.tmsConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.tmsConfiguration.defaultY;
                computerConfiguration.tmsConfiguration.defaultX = xpos;
                computerConfiguration.tmsConfiguration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "transparant")
                    screenInfo.defaultColour[COL_TMS_TRANSPARANT].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "black")
                    screenInfo.defaultColour[COL_TMS_BLACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "medium_green")
                    screenInfo.defaultColour[COL_TMS_MEDIUM_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "light_green")
                    screenInfo.defaultColour[COL_TMS_LIGHT_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "dark_blue")
                    screenInfo.defaultColour[COL_TMS_DARK_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "light_blue")
                    screenInfo.defaultColour[COL_TMS_LIGHT_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "dark_red")
                    screenInfo.defaultColour[COL_TMS_DARK_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "aqua_blue")
                    screenInfo.defaultColour[COL_TMS_AQUA_BLUE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "medium_red")
                    screenInfo.defaultColour[COL_TMS_MEDIUM_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "light_red")
                    screenInfo.defaultColour[COL_TMS_LIGHT_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "dark_yellow")
                    screenInfo.defaultColour[COL_TMS_DARK_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "light_yellow")
                    screenInfo.defaultColour[COL_TMS_LIGHT_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "dark_green")
                    screenInfo.defaultColour[COL_TMS_DARK_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "purple")
                    screenInfo.defaultColour[COL_TMS_PURPLE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "gray")
                    screenInfo.defaultColour[COL_TMS_GRAY].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "white")
                    screenInfo.defaultColour[COL_TMS_WHITE].Printf("#%02X%02X%02X", red, green, blue);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.tmsConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.tmsConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_Intel8275Video(wxXmlNode &node)
{
    if (!computerConfiguration.i8275Configuration.defined)
    {
        computerConfiguration.i8275Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.i8275Configuration.defined = true;

    wxString tagList[]=
    {
        "font",
        "dirname",
        "in",
        "out",
        "ef",
        "int",
        "interlace",
        "zoom",
        "border",
        "pos",
        "color",
        "clock",
        "char",
        "screen",
        "iogroup",
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
        TAG_INTERRUPT,
        TAG_INTERLACE,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_CLOCK,
        TAG_CHAR,
        TAG_SCREEN,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    computerConfiguration.zoom_[computerConfiguration.i8275Configuration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.i8275Configuration.videoNumber] = "Intel 8275";
    computerConfiguration.i8275Configuration.writeCommandOutput = init_IoPort();
    computerConfiguration.i8275Configuration.readStatus = init_IoPort();
    computerConfiguration.i8275Configuration.writeParameter = init_IoPort();
    computerConfiguration.i8275Configuration.readParameter = init_IoPort();
    computerConfiguration.i8275Configuration.efVerticalRetrace = init_EfFlag();
    computerConfiguration.i8275Configuration.efHorizontalRetrace = init_EfFlag();
    computerConfiguration.i8275Configuration.ioGroupVector.clear();
    computerConfiguration.i8275Configuration.charSize.x = 8;
    computerConfiguration.i8275Configuration.charSize.y = 10;
    computerConfiguration.i8275Configuration.screenSize.x = 80;
    computerConfiguration.i8275Configuration.screenSize.y = 24;
    computerConfiguration.i8275Configuration.videoClock = 12;
    computerConfiguration.i8275Configuration.gpaSwitched = false;

    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, position, iogroup;
    size_t ioGroupNumber = 0;

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
                    computerConfiguration.i8275Configuration.readStatus = parseXml_IoPort(*child, I8275_READ_STATUS_IN);
                if (child->GetAttribute("type") == "parameter")
                    computerConfiguration.i8275Configuration.readParameter = parseXml_IoPort(*child, I8275_READ_PARAMETER_IN);
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "command")
                    computerConfiguration.i8275Configuration.writeCommandOutput = parseXml_IoPort(*child, I8275_WRITE_COMMAND_OUT);
                if (child->GetAttribute("type") == "parameter")
                    computerConfiguration.i8275Configuration.writeParameter  = parseXml_IoPort(*child, I8275_WRITE_PARAMETER_OUT);
            break;
                
            case TAG_FONT:
                computerConfiguration.characterRomConfiguration.fileName = child->GetNodeContent();
                if (child->GetAttribute("gpa") == "switched")
                    computerConfiguration.i8275Configuration.gpaSwitched = true;
            break;

            case TAG_DIRNAME:
                computerConfiguration.characterRomConfiguration.directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.characterRomConfiguration.directory.Right(1) != pathSeparator_)
                    computerConfiguration.characterRomConfiguration.directory += pathSeparator_;
            break;

            case TAG_EF:
                if (child->GetAttribute("type") == "horizontal")
                    computerConfiguration.i8275Configuration.efHorizontalRetrace = parseXml_EfFlag(*child, I8275_HORIZONTAL_EF);
                else
                    computerConfiguration.i8275Configuration.efVerticalRetrace = parseXml_EfFlag(*child, I8275_VERTICAL_EF);
            break;
                                
            case TAG_INTERRUPT:
                if (child->GetAttribute("type") == "horizontal")
                    computerConfiguration.i8275Configuration.picInterrupt = (int)parseXml_Number(*child);
                else
                    computerConfiguration.i8275Configuration.picInterruptHorizontal = (int)parseXml_Number(*child);
            break;

            case TAG_INTERLACE:
                computerConfiguration.interlace_ = true;
            break;
                
            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.i8275Configuration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOXMLI8275] = (int)width;
                    screenInfo.borderY[VIDEOXMLI8275] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.i8275Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.i8275Configuration.defaultY;
                computerConfiguration.i8275Configuration.defaultX = xpos;
                computerConfiguration.i8275Configuration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_I8275_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_I8275_BACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "high")
                    screenInfo.defaultColour[COL_I8275_HIGH].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.i8275Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.i8275Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_CLOCK:
                computerConfiguration.i8275Configuration.videoClock = getDouble(child->GetNodeContent(), childName, 500, "500", false);
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
                    computerConfiguration.i8275Configuration.charSize.x = (int)width;
                    computerConfiguration.i8275Configuration.charSize.y = (int)height;
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
                    computerConfiguration.i8275Configuration.screenSize.x = (int)width;
                    computerConfiguration.i8275Configuration.screenSize.y = (int)height;
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

void XmlParser::parseXml_VisVideo(wxXmlNode &node)
{    
    Byte ctone[8] = { 0, 28, 77, 105, 150, 194, 227, 255 };
    Byte red[8]   = {   0,   0,   0,   0, 255, 255, 255, 255 };
    Byte green[8] = {   0, 255,   0, 255,   0, 255,   0, 255 };
    Byte blue[8]  = {   0,   0, 255, 255,   0,   0, 255, 255 };

    if (!computerConfiguration.vis1870Configuration.defined)
    {
        computerConfiguration.vis1870Configuration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.vis1870Configuration.defined = true;

    wxString tagList[]=
    {
        "pagesize",
        "charsize",
        "graphicsize",
        "charrom",
        "filename",
        "dirname",
        "pcbmask",
        "charlines",
        "maxcharlines",
        "maxscreenlines",
        "mode",
        "int_mode",
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
        "log",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_PAGE,
        TAG_CHAR,
        TAG_GRAPHIC,
        TAG_CHARROM,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_PCB,
        TAG_CHARLINES,
        TAG_MAX_CHARLINES,
        TAG_SCREENLINES,
        TAG_MODE,
        TAG_INT_MODE,
        TAG_INTERRUPT,
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
        TAG_LOG,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    int tagTypeInt;
    long width, height;
    int number, xpos, ypos;
    wxString color, position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.vis1870Configuration.ioGroupVector.clear();
    computerConfiguration.vis1870Configuration.pageMemSize = 0x3ff;
    computerConfiguration.vis1870Configuration.pageMemIsRom =false;
    computerConfiguration.vis1870Configuration.graphicMemSize = 0;
    computerConfiguration.vis1870Configuration.charMemSize =0x7ff;
    computerConfiguration.vis1870Configuration.charMemIsRom = false;
    computerConfiguration.vis1870Configuration.charRomIsTmcFormat = false;
    computerConfiguration.vis1870Configuration.pcbMask = 0x7f;
    computerConfiguration.vis1870Configuration.maxCharLines = 16;
    computerConfiguration.vis1870Configuration.maxScreenLines = 24;
    computerConfiguration.vis1870Configuration.interruptMode = INT_NONE;
    computerConfiguration.vis1870Configuration.videoClock = 5.626;
    computerConfiguration.vis1870Configuration.charRomStart = 0;
    computerConfiguration.vis1870Configuration.cmaMaskFixed = false;
    computerConfiguration.vis1870Configuration.videoModeEf = 2;
    computerConfiguration.vis1870Configuration.rotateScreen = false;
    computerConfiguration.vis1870Configuration.colorRamType = CR_NONE;
    computerConfiguration.vis1870Configuration.ef = init_EfFlag();
    computerConfiguration.vis1870Configuration.outputWrite = init_IoPort();
    computerConfiguration.vis1870Configuration.outputSelect = init_IoPort();
    computerConfiguration.vis1870Configuration.cursorBlink = false;
    computerConfiguration.vis1870Configuration.outputInterruptEnable = init_IoPort();
    computerConfiguration.vis1870Configuration.outputInterruptReset = init_IoPort();
    computerConfiguration.vis1870Configuration.pageMemExcludeIoGroup = false;
    computerConfiguration.vis1870Configuration.charMemExcludeIoGroup = false;
    computerConfiguration.vis1870Configuration.graphicMemExcludeIoGroup = false;
    computerConfiguration.vis1870Configuration.charLines = -1;
    
    computerConfiguration.vis1870Configuration.videoMode = PAL;
    computerConfiguration.zoom_[computerConfiguration.vis1870Configuration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.vis1870Configuration.videoNumber] = "VIS 1870";

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
                    computerConfiguration.vis1870Configuration.pageMemIsRom = true;
                if (child->GetAttribute("iogroup") == "no")
                    computerConfiguration.vis1870Configuration.pageMemExcludeIoGroup = true;
                computerConfiguration.vis1870Configuration.pageMemSize = (int)parseXml_Number(*child) & 0xfff;
            break;
                                
            case TAG_CHAR:
                if (child->GetAttribute("type") == "rom")
                    computerConfiguration.vis1870Configuration.charMemIsRom = true;
                if (child->GetAttribute("iogroup") == "no")
                    computerConfiguration.vis1870Configuration.charMemExcludeIoGroup = true;
                computerConfiguration.vis1870Configuration.charMemSize =  (int)parseXml_Number(*child) & 0xfff;
            break;

            case TAG_GRAPHIC:
                if (child->GetAttribute("iogroup") == "no")
                    computerConfiguration.vis1870Configuration.graphicMemExcludeIoGroup = true;
                computerConfiguration.vis1870Configuration.graphicMemSize =  (int)parseXml_Number(*child) & 0x1fff;
            break;

            case TAG_CHARROM:
                computerConfiguration.vis1870Configuration.charRomStart = (int)parseXml_Number(*child);
            break;

            case TAG_FILENAME:
                if (child->GetAttribute("format") == "tmc")
                    computerConfiguration.vis1870Configuration.charRomIsTmcFormat = true;
                computerConfiguration.characterRomConfiguration.fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.characterRomConfiguration.directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.characterRomConfiguration.directory.Right(1) != pathSeparator_)
                    computerConfiguration.characterRomConfiguration.directory += pathSeparator_;
            break;

            case TAG_PCB:
                computerConfiguration.vis1870Configuration.pcbMask =  (int)parseXml_Number(*child);
            break;

            case TAG_CHARLINES:
                computerConfiguration.vis1870Configuration.charLines = (int)parseXml_Number(*child);
            break;

            case TAG_MAX_CHARLINES:
                computerConfiguration.vis1870Configuration.maxCharLines = (int)parseXml_Number(*child);
            break;

            case TAG_SCREENLINES:
                computerConfiguration.vis1870Configuration.maxScreenLines = (int)parseXml_Number(*child);
            break;

            case TAG_MODE:
                if (child->GetNodeContent() == "ntsc")
                    computerConfiguration.vis1870Configuration.videoMode = NTSC;
            break;

            case TAG_INT_MODE:
                computerConfiguration.vis1870Configuration.interruptMode =  (int)parseXml_Number(*child) - 1;
            break;

            case TAG_INTERRUPT:
                computerConfiguration.vis1870Configuration.picInterrupt =  (int)parseXml_Number(*child) - 1;
            break;

            case TAG_EF:
                if (child->GetAttribute("type") == "display")
                    computerConfiguration.vis1870Configuration.ef = parseXml_EfFlag(*child, VIS1870_EF);
                if (child->GetAttribute("type") == "mode")
                {
                    computerConfiguration.vis1870Configuration.videoModeEf = (int)parseXml_Number(*child);
                    computerConfiguration.vis1870Configuration.useVideoModeEf = true;
                }
                if (child->GetAttribute("write") == "enable")
                {
                    computerConfiguration.vis1870Configuration.useBlockWrite = false;
                }
            break;
                
            case TAG_CLOCK:
                computerConfiguration.vis1870Configuration.videoClock = getDouble(child->GetNodeContent(), childName, 500, "500", false);
            break;

            case TAG_CMAMASK:
                computerConfiguration.vis1870Configuration.cmaMaskFixed = true;
            break;
                
            case TAG_ROTATE:
                computerConfiguration.vis1870Configuration.rotateScreen = true;
            break;

            case TAG_COLORRAM:
                if (child->GetAttribute("type") == "cidelsa")
                    computerConfiguration.vis1870Configuration.colorRamType = CR_CIDELSA;
                if (child->GetAttribute("type") == "tmc")
                    computerConfiguration.vis1870Configuration.colorRamType = CR_TMC600;
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "write")
                    computerConfiguration.vis1870Configuration.outputWrite = parseXml_IoPort(*child, VIS1870_TELMAC_DATA_OUT);
                if (child->GetAttribute("type") == "select")
                    computerConfiguration.vis1870Configuration.outputSelect = parseXml_IoPort(*child, VIS1870_TELMAC_REGISTER_OUT);
                if (child->GetAttribute("type") == "int")
                    computerConfiguration.vis1870Configuration.outputInterruptEnable = parseXml_IoPort(*child, VIS1870_OUT2);
                if (child->GetAttribute("type") == "int_reset")
                    computerConfiguration.vis1870Configuration.outputInterruptReset = parseXml_IoPort(*child, VIS1870_INT_RESET);
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.vis1870Configuration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOXML1870] = (int)width;
                    screenInfo.borderY[VIDEOXML1870] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.vis1870Configuration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.vis1870Configuration.defaultY;
                computerConfiguration.vis1870Configuration.defaultX = xpos;
                computerConfiguration.vis1870Configuration.defaultY = ypos;
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
                            screenInfo.defaultColour[i*8 + number].Printf("#%02X%02X%02X", (red[number] ^ 0xff) & (ctone[i] ^ 0xff), (green[number] ^ 0xff) & (ctone[i] ^ 0xff), (blue[number] ^ 0xff) & (ctone[i] ^ 0xff));
                        }
                    }
                    else
                    {
                        for (int i = 0; i < 8; i++)
                        {
                            screenInfo.defaultColour[i*8 + number].Printf("#%02X%02X%02X", red[number] & ctone[i], green[number] & ctone[i], blue[number] & ctone[i]);
                        }
                    }
                }
                else
                    screenInfo.defaultColour[number+56].Printf("#%02X%02X%02X", red[number], green[number], blue[number]);
            break;

            case TAG_CTONE:
                number = (int)parseXml_Number(*child, "num");
                ctone[number] = (int)parseXml_Number(*child);
                
                screenInfo.defaultColour[number*8].Printf("#%02X%02X%02X", (red[0] ^ 0xff) & (ctone[number] ^0xff), (green[0] ^ 0xff) & (ctone[number] ^0xff), (blue[0] ^ 0xff) & (ctone[number] ^0xff));
                for (int i = 1; i < 8; i++)
                    screenInfo.defaultColour[number*8 + i].Printf("#%02X%02X%02X", red[i] & ctone[number], green[i] & ctone[number], blue[i] & ctone[number]);
            break;

            case TAG_CURSORBLINK:
                computerConfiguration.vis1870Configuration.cursorBlink = true;
            break;

            case TAG_LOG:
                computerConfiguration.debuggerConfiguration.videoLog_defined = true;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.vis1870Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.vis1870Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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
    if (computerConfiguration.vis1870Configuration.charLines == -1)
    {
        if (computerConfiguration.vis1870Configuration.videoMode == PAL)
            computerConfiguration.vis1870Configuration.charLines = 9;
        else
            computerConfiguration.vis1870Configuration.charLines = 8;
    }
}

void XmlParser::parseXml_SN76430NVideo(wxXmlNode &node)
{
    if (!computerConfiguration.sn76430NConfiguration.defined)
    {
        computerConfiguration.sn76430NConfiguration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.sn76430NConfiguration.defined = true;

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

    computerConfiguration.zoom_[computerConfiguration.sn76430NConfiguration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.sn76430NConfiguration.videoNumber] = "SN76430N";

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
                computerConfiguration.zoom_[computerConfiguration.sn76430NConfiguration.videoNumber] = child->GetNodeContent();
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
                    screenInfo.borderX[VIDEOXMLSN76430N] = (int)width;
                    screenInfo.borderY[VIDEOXMLSN76430N] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.sn76430NConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.sn76430NConfiguration.defaultY;
                computerConfiguration.sn76430NConfiguration.defaultX = xpos;
                computerConfiguration.sn76430NConfiguration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "white")
                    screenInfo.defaultColour[COL_SN76430N_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "yellow")
                    screenInfo.defaultColour[COL_SN76430N_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "green")
                    screenInfo.defaultColour[COL_SN76430N_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "red")
                    screenInfo.defaultColour[COL_SN76430N_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "black")
                    screenInfo.defaultColour[COL_SN76430N_BLACK].Printf("#%02X%02X%02X", red, green, blue);
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

void XmlParser::parseXml_Ps2Keyboard(wxXmlNode &node)
{
    computerConfiguration.keyLatchConfiguration[0].defined = false;
    computerConfiguration.matrixKeyboardConfiguration.defined = false;
    computerConfiguration.cdp1871Configuration.defined = false;
    computerConfiguration.asciiKeyboardConfiguration.defined = false;
    computerConfiguration.ps2KeyboardConfiguration.defined = true;
    computerConfiguration.gpioPs2KeyboardConfiguration.defined = false;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "caps",
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
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.ps2KeyboardConfiguration.ioGroupVector.clear();
    computerConfiguration.ps2KeyboardConfiguration.input = init_IoPort();
    computerConfiguration.ps2KeyboardConfiguration.output = init_IoPort();
    computerConfiguration.ps2KeyboardConfiguration.ef = init_EfFlag();

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
                computerConfiguration.ps2KeyboardConfiguration.input = parseXml_IoPort(*child, PS2_KEYBOARD_IN);
            break;
                
            case TAG_OUT:
                computerConfiguration.ps2KeyboardConfiguration.output = parseXml_IoPort(*child, PS2_KEYBOARD_OUT);
            break;
                
            case TAG_EF:
                computerConfiguration.ps2KeyboardConfiguration.ef = parseXml_EfFlag(*child, PS2_KEYBOARD_EF);
            break;
 
            case TAG_CAPS:
                computerConfiguration.forceUpperCase = true;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.ps2KeyboardConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.ps2KeyboardConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_Gpio(wxXmlNode &node)
{
    computerConfiguration.keyLatchConfiguration[0].defined = false;
    computerConfiguration.matrixKeyboardConfiguration.defined = false;
    computerConfiguration.cdp1871Configuration.defined = false;
    computerConfiguration.asciiKeyboardConfiguration.defined = false;
    computerConfiguration.ps2KeyboardConfiguration.defined = false;
    computerConfiguration.gpioPs2KeyboardConfiguration.defined = true;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "keydef",
        "caps",
        "frequency",
        "jp4",
        "startup",
        "int",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_KEYDEF,
        TAG_CAPS,
        TAG_FREQ,
        TAG_JP4,
        TAG_STARTUP,
        TAG_INTERRUPT,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString iogroup, keyText;
    size_t ioGroupNumber = 0, textEfKeyInt;

    computerConfiguration.gpioPs2KeyboardConfiguration.ioGroupVector.clear();
    computerConfiguration.gpioPs2KeyboardConfiguration.input = init_IoPort();
    computerConfiguration.gpioPs2KeyboardConfiguration.output = init_IoPort();
    computerConfiguration.gpioPs2KeyboardConfiguration.ef = init_EfFlag();

    for (int i=0; i<LAST_MATRIX_TEXT_KEY; i++)
        computerConfiguration.gpioPs2KeyboardConfiguration.textKey[i] = -1;

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
                computerConfiguration.gpioPs2KeyboardConfiguration.input = parseXml_IoPort(*child, PS2_GPIO_READ_DATA_IN);
            break;
                                
            case TAG_OUT:
                computerConfiguration.gpioPs2KeyboardConfiguration.output = parseXml_IoPort(*child, PS2_GPIO_WRITE_CONTROL_OUT);
            break;

            case TAG_EF:
                computerConfiguration.gpioPs2KeyboardConfiguration.ef = parseXml_EfFlag(*child, PS2_GPIO_EF);
            break;
 
            case TAG_JP4:
                computerConfiguration.gpioPs2KeyboardConfiguration.jp4 = true;
            break;

            case TAG_STARTUP:
                computerConfiguration.gpioPs2KeyboardConfiguration.startupString = child->GetNodeContent();
            break;

            case TAG_INTERRUPT:
                computerConfiguration.gpioPs2KeyboardConfiguration.interrupt = true;
                computerConfiguration.gpioPs2KeyboardConfiguration.picInterrupt = (int)parseXml_Number(*child);
            break;

            case TAG_CAPS:
                computerConfiguration.forceUpperCase = true;
            break;

            case TAG_FREQ:
                computerConfiguration.gpioPs2KeyboardConfiguration.beepFrequency = (int)parseXml_Number(*child);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.gpioPs2KeyboardConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.gpioPs2KeyboardConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_KEYDEF:
                if (child->GetAttribute("type") == "text")
                {
                    keyText = child->GetNodeContent();
                    textEfKeyInt = 0;
                    while (textKeyList[textEfKeyInt] != "undefined")
                    {
                        if (keyText == textKeyList[textEfKeyInt])
                        {
                            if (!child->HasAttribute("os"))
                                computerConfiguration.gpioPs2KeyboardConfiguration.textKey[textEfKeyInt] = (int)parseXml_Number(*child, "value");
#ifdef __WXMAC__
                            if (child->GetAttribute("os") == "mac")
                                computerConfiguration.gpioPs2KeyboardConfiguration.textKey[textEfKeyInt] = (int)parseXml_Number(*child, "value");
#endif
#ifdef __WXMSW__
                            if (child->GetAttribute("os") == "windows")
                                computerConfiguration.gpioPs2KeyboardConfiguration.textKey[textEfKeyInt] = (int)parseXml_Number(*child, "value");
#endif
#ifdef __linux__
                            if (child->GetAttribute("os") == "linux")
                                computerConfiguration.gpioPs2KeyboardConfiguration.textKey[textEfKeyInt] = (int)parseXml_Number(*child, "value");
#endif
                        }
                        textEfKeyInt++;
                    }
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

void XmlParser::parseXml_AsciiKeyboard(wxXmlNode &node)
{
    computerConfiguration.keyLatchConfiguration[0].defined = false;
    computerConfiguration.matrixKeyboardConfiguration.defined = false;
    computerConfiguration.cdp1871Configuration.defined = false;
    computerConfiguration.asciiKeyboardConfiguration.defined = true;
    computerConfiguration.ps2KeyboardConfiguration.defined = false;
    computerConfiguration.ps2KeyboardConfiguration.interrupt = false;
    computerConfiguration.gpioPs2KeyboardConfiguration.defined = false;

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
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.asciiKeyboardConfiguration.ioGroupVector.clear();
    computerConfiguration.asciiKeyboardConfiguration.input = init_IoPort();
    computerConfiguration.asciiKeyboardConfiguration.ef = init_EfFlag();

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
                computerConfiguration.asciiKeyboardConfiguration.input = parseXml_IoPort(*child, ASCII_KEYBOARD_IN);
            break;
                
            case TAG_EF:
                computerConfiguration.asciiKeyboardConfiguration.ef = parseXml_EfFlag(*child, ASCII_KEYBOARD_EF);
            break;
                
            case TAG_CAPS:
                computerConfiguration.forceUpperCase = true;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.asciiKeyboardConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.asciiKeyboardConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_1871Keyboard(wxXmlNode &node)
{
    computerConfiguration.keyLatchConfiguration[0].defined = false;
    computerConfiguration.matrixKeyboardConfiguration.defined = false;
    computerConfiguration.cdp1871Configuration.defined = true;
    computerConfiguration.asciiKeyboardConfiguration.defined = false;
    computerConfiguration.ps2KeyboardConfiguration.defined = false;
    computerConfiguration.ps2KeyboardConfiguration.interrupt = false;
    computerConfiguration.gpioPs2KeyboardConfiguration.defined = false;

    wxString tagList[]=
    {
        "in",
        "ef",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_EF,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    int tagTypeInt;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.cdp1871Configuration.ioGroupVector.clear();
    computerConfiguration.cdp1871Configuration.input = init_IoPort();
    computerConfiguration.cdp1871Configuration.repeatEf = init_EfFlag();
    computerConfiguration.cdp1871Configuration.ef = init_EfFlag();

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
                computerConfiguration.cdp1871Configuration.input = parseXml_IoPort(*child, CDP1871_IN);
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "rpt")
                    computerConfiguration.cdp1871Configuration.repeatEf = parseXml_EfFlag(*child, CDP1871_REPEAT_EF);
                if (child->GetAttribute("type") == "data" || child->GetAttribute("type") == "ready" )
                    computerConfiguration.cdp1871Configuration.ef = parseXml_EfFlag(*child, CDP1871_EF);
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.cdp1871Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.cdp1871Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_LatchKeyboard(wxXmlNode &node)
{
    computerConfiguration.keyLatchConfiguration[0].defined = true;
    computerConfiguration.matrixKeyboardConfiguration.defined = false;
    computerConfiguration.cdp1871Configuration.defined = false;
    computerConfiguration.asciiKeyboardConfiguration.defined = false;
    computerConfiguration.ps2KeyboardConfiguration.defined = false;
    computerConfiguration.ps2KeyboardConfiguration.interrupt = false;
    computerConfiguration.gpioPs2KeyboardConfiguration.defined = false;

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
    wxString keyText, iogroup;
    size_t ioGroupNumber = 0;
    
    for (int i=0; i<512; i++)
    {
        computerConfiguration.keyLatchConfiguration[0].pc[i] = -1;
        computerConfiguration.keyLatchConfiguration[0].pcShift[i] = -1;
        computerConfiguration.keyLatchConfiguration[0].pcForceShift[i] = -1;
        computerConfiguration.keyLatchConfiguration[0].pcForceNoShift[i] = -1;
    }
    
    computerConfiguration.keyLatchConfiguration[0].ioGroupVector.clear();
    computerConfiguration.keyLatchConfiguration[0].ef = init_EfFlag();
    computerConfiguration.keyLatchConfiguration[0].output = init_IoPort();
    computerConfiguration.keyLatchConfiguration[0].altRight = -1;
    computerConfiguration.keyLatchConfiguration[0].altLeft = -1;
    computerConfiguration.keyLatchConfiguration[0].ctrlRight = -1;
    computerConfiguration.keyLatchConfiguration[0].ctrlLeft = -1;
    computerConfiguration.keyLatchConfiguration[0].shift = -1;
    computerConfiguration.keyLatchConfiguration[0].switchAltCtrl = -1;

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
                computerConfiguration.keyLatchConfiguration[0].output = parseXml_IoPort(*child, LATCH_KEYBOARD_OUT);
            break;
                
            case TAG_EF:
                computerConfiguration.keyLatchConfiguration[0].ef = parseXml_EfFlag(*child, LATCH_KEYBOARD_EF);
            break;
                
            case TAG_KEYDEF:
                if (child->GetAttribute("type") == "")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                    {
                        if (child->HasAttribute("out"))
                            computerConfiguration.keyLatchConfiguration[0].pc[keyValue] = (int)parseXml_Number(*child, "out");
                    }
                    if (child->HasAttribute("map"))
                    {
                        computerConfiguration.keyLatchConfiguration[0].pcMap[(int)parseXml_Number(*child, "map")] = (int)parseXml_Number(*child)&0x1ff;
                    }
                }
                
                if (child->GetAttribute("type") == "text")
                {
                    keyText = child->GetNodeContent();
                    if (keyText == "alt_right")
                        computerConfiguration.keyLatchConfiguration[0].altRight = (int)parseXml_Number(*child, "out");
                    if (keyText == "alt_left")
                        computerConfiguration.keyLatchConfiguration[0].altLeft = (int)parseXml_Number(*child, "out");
                    if (keyText == "ctrl_right")
                        computerConfiguration.keyLatchConfiguration[0].ctrlRight = (int)parseXml_Number(*child, "out");
                    if (keyText == "ctrl_left")
                        computerConfiguration.keyLatchConfiguration[0].ctrlLeft = (int)parseXml_Number(*child, "out");
                    if (keyText == "shift")
                        computerConfiguration.keyLatchConfiguration[0].shift = (int)parseXml_Number(*child, "out");
                    if (keyText == "switch")
                        computerConfiguration.keyLatchConfiguration[0].switchAltCtrl = (int)parseXml_Number(*child, "out");
                    if (keyText == "caps")
                        computerConfiguration.keyLatchConfiguration[0].caps = (int)parseXml_Number(*child, "out");
                }

                if (child->GetAttribute("type") == "force")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                        if (child->HasAttribute("out"))
                            computerConfiguration.keyLatchConfiguration[0].pcForceShift[keyValue] = (int)parseXml_Number(*child, "out");
                }

                if (child->GetAttribute("type") == "force_noshift")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                        if (child->HasAttribute("out"))
                            computerConfiguration.keyLatchConfiguration[0].pcForceNoShift[keyValue] = (int)parseXml_Number(*child, "out");
                }

                if (child->GetAttribute("type") == "shift")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                        if (child->HasAttribute("out"))
                            computerConfiguration.keyLatchConfiguration[0].pcShift[keyValue] = (int)parseXml_Number(*child, "out");
                    if (child->HasAttribute("map"))
                    {
                        computerConfiguration.keyLatchConfiguration[0].pcMapShift[(int)parseXml_Number(*child, "map")] = (int)parseXml_Number(*child)&0x1ff;
                    }
                }
            break;

/*            case TAG_KEYDEFSHIFT:
                keyValue = (int)parseXml_Number(*child)&0x1ff;
                if (keyValue != 0)
                    computerConfiguration.keyLatchConfiguration[0].pcShift[keyValue] = (int)parseXml_Number(*child, "out");
            break;*/

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.keyLatchConfiguration[0].ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.keyLatchConfiguration[0].ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_MatrixKeyboard(wxXmlNode &node)
{
    computerConfiguration.keyLatchConfiguration[0].defined = false;
    computerConfiguration.matrixKeyboardConfiguration.defined = true;
    computerConfiguration.cdp1871Configuration.defined = false;
    computerConfiguration.asciiKeyboardConfiguration.defined = false;
    computerConfiguration.ps2KeyboardConfiguration.defined = false;
    computerConfiguration.ps2KeyboardConfiguration.interrupt = false;
    computerConfiguration.gpioPs2KeyboardConfiguration.defined = false;

    computerConfiguration.matrixKeyboardConfiguration.keyPressed = 0xff;

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
    wxString keyText, efText, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.matrixKeyboardConfiguration.ioGroupVector.clear();
    for (int i=0; i<255; i++)
    {
        computerConfiguration.matrixKeyboardConfiguration.keyValue[i] = -1;
        computerConfiguration.matrixKeyboardConfiguration.bitValue[i] = 0;
        computerConfiguration.matrixKeyboardConfiguration.shiftValue[i] = 1;
        computerConfiguration.matrixKeyboardConfiguration.ctrlValue[i] = 1;
    }
    for (int i=0; i<LAST_MATRIX_EF_KEY; i++)
    {
        computerConfiguration.matrixKeyboardConfiguration.efKey[i] = 0;
        computerConfiguration.matrixKeyboardConfiguration.efKeyRev[i] = false;
    }
    computerConfiguration.matrixKeyboardConfiguration.useAddress = false;
    

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
                computerConfiguration.matrixKeyboardConfiguration.input = parseXml_IoPort(*child, MATRIX_KEYBOARD_IN);
                
                if (child->GetAttribute("useaddress") == "yes")
                    computerConfiguration.matrixKeyboardConfiguration.useAddress = true;

                if (child->GetAttribute("pol") == "rev")
                    computerConfiguration.matrixKeyboardConfiguration.keyPressed = 0;
            break;
            
            case TAG_OUT:
                computerConfiguration.matrixKeyboardConfiguration.output = parseXml_IoPort(*child, MATRIX_KEYBOARD_OUT);
            break;

            case TAG_KEYDEF:
                if (child->GetAttribute("type") == "")
                {
                    keyValue = (int)parseXml_Number(*child)&0xff;
                    computerConfiguration.matrixKeyboardConfiguration.keyValue[keyValue] = (int)parseXml_Number(*child, "address");
                    if (child->HasAttribute("bit"))
                        computerConfiguration.matrixKeyboardConfiguration.bitValue[keyValue] = 1 << (int)parseXml_Number(*child, "bit");
                    if (child->HasAttribute("mask"))
                        computerConfiguration.matrixKeyboardConfiguration.bitValue[keyValue] = parseXml_Number(*child, "mask");
                }
                if (child->GetAttribute("type") == "shift")
                {
                    keyValue = (int)parseXml_Number(*child)&0xff;
                    computerConfiguration.matrixKeyboardConfiguration.keyValue[keyValue] = (int)parseXml_Number(*child, "address");
                    if (child->HasAttribute("bit"))
                        computerConfiguration.matrixKeyboardConfiguration.bitValue[keyValue] = 1 << (int)parseXml_Number(*child, "bit");
                    if (child->HasAttribute("mask"))
                        computerConfiguration.matrixKeyboardConfiguration.bitValue[keyValue] = parseXml_Number(*child, "mask");
                    computerConfiguration.matrixKeyboardConfiguration.shiftValue[keyValue] = 0;
                }
                if (child->GetAttribute("type") == "ctrl")
                {
                    keyValue = (int)parseXml_Number(*child)&0xff;
                    computerConfiguration.matrixKeyboardConfiguration.keyValue[keyValue] = (int)parseXml_Number(*child, "address");
                    if (child->HasAttribute("bit"))
                        computerConfiguration.matrixKeyboardConfiguration.bitValue[keyValue] = 1 << (int)parseXml_Number(*child, "bit");
                    if (child->HasAttribute("mask"))
                        computerConfiguration.matrixKeyboardConfiguration.bitValue[keyValue] = parseXml_Number(*child, "mask");
                    computerConfiguration.matrixKeyboardConfiguration.ctrlValue[keyValue] = 0;
                }
                if (child->GetAttribute("type") == "text")
                {
                    keyText = child->GetNodeContent();
                    textEfKeyInt = 0;
                    while (textKeyList[textEfKeyInt] != "undefined")
                    {
                        if (keyText == textKeyList[textEfKeyInt])
                        {
                            computerConfiguration.matrixKeyboardConfiguration.textKey[textEfKeyInt].keyValue = (int)parseXml_Number(*child, "address");
                            if (child->HasAttribute("bit"))
                                computerConfiguration.matrixKeyboardConfiguration.textKey[textEfKeyInt].bitMaskPressed = 1 << (int)parseXml_Number(*child, "bit");
                            if (child->HasAttribute("mask"))
                                computerConfiguration.matrixKeyboardConfiguration.textKey[textEfKeyInt].bitMaskPressed = parseXml_Number(*child, "mask");
                            computerConfiguration.matrixKeyboardConfiguration.textKey[textEfKeyInt].bitMaskNotPressed = computerConfiguration.matrixKeyboardConfiguration.textKey[textEfKeyInt].bitMaskPressed ^ 0xff;
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
                        computerConfiguration.matrixKeyboardConfiguration.efKey[textEfKeyInt] = tagTypeInt-TAG_EF1+1;
                        if (child->GetAttribute("pol") == "rev")
                            computerConfiguration.matrixKeyboardConfiguration.efKeyRev[textEfKeyInt] = true;
                    }
                    textEfKeyInt++;
                }
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.matrixKeyboardConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.matrixKeyboardConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_ElfKeypad(wxXmlNode &node)
{
    computerConfiguration.useHexKeyboard = true;
    computerConfiguration.hexDisplayConfiguration.defined = true;

    wxString tagList[]=
    {
        "in",
        "ef",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_EF,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.hexDisplayConfiguration.ioGroupVector.clear();
    computerConfiguration.hexDisplayConfiguration.ef = init_EfFlag();
    computerConfiguration.hexDisplayConfiguration.inEf = init_EfFlag();
    computerConfiguration.hexDisplayConfiguration.inEfElf = init_EfFlag();

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
                computerConfiguration.hexDisplayConfiguration.input = parseXml_IoPort(*child, HEX_KEY_IN);
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "data" || child->GetAttribute("type") == "ready" )
                    computerConfiguration.hexDisplayConfiguration.ef = parseXml_EfFlag(*child, HEX_KEY_EF);
                if (child->GetAttribute("type") == "in")
                    computerConfiguration.hexDisplayConfiguration.inEf = parseXml_EfFlag(*child);
                if (child->GetAttribute("type") == "in_elf")
                    computerConfiguration.hexDisplayConfiguration.inEfElf = parseXml_EfFlag(*child);
            break;
                                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.hexDisplayConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.hexDisplayConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_BitKeypad(int padnumber, wxXmlNode &node)
{
    computerConfiguration.bitKeypadConfiguration[padnumber].defined = true;
    
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
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.bitKeypadConfiguration[padnumber].ioGroupVector.clear();
    computerConfiguration.bitKeypadConfiguration[padnumber].bitKeyPressed = 0;
    computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys = 0;
    computerConfiguration.bitKeypadConfiguration[padnumber].numberOfModKeys = 0;
    computerConfiguration.bitKeypadConfiguration[padnumber].repeat = true;
    computerConfiguration.bitKeypadConfiguration[padnumber].input = init_IoPort();

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
                computerConfiguration.bitKeypadConfiguration[padnumber].input = parseXml_IoPort(*child, BIT_KEYPAD_IN_0+padnumber);
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
                    computerConfiguration.bitKeypadConfiguration[padnumber].bitModkey.resize(computerConfiguration.bitKeypadConfiguration[padnumber].numberOfModKeys+1);
                    computerConfiguration.bitKeypadConfiguration[padnumber].bitModkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfModKeys].value = (int)parseXml_Number(*child);
                    computerConfiguration.bitKeypadConfiguration[padnumber].bitModkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfModKeys].ef = (int)parseXml_Number(*child, "ef");
                    computerConfiguration.bitKeypadConfiguration[padnumber].bitModkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfModKeys].bitMaskPressed = 1 << (tagTypeInt - TAG_BIT0);
                    computerConfiguration.bitKeypadConfiguration[padnumber].bitModkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfModKeys].bitMaskReleased = computerConfiguration.bitKeypadConfiguration[padnumber].bitModkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfModKeys].bitMaskPressed ^ 0xff;
                    computerConfiguration.bitKeypadEf[computerConfiguration.bitKeypadConfiguration[padnumber].bitModkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfModKeys].ef] = true;
                    computerConfiguration.bitKeypadConfiguration[padnumber].numberOfModKeys++;
                }
                else
                {
                    computerConfiguration.bitKeypadConfiguration[padnumber].bitkey.resize(computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys+1);
                    computerConfiguration.bitKeypadConfiguration[padnumber].bitkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys].value = (int)parseXml_Number(*child);
                    computerConfiguration.bitKeypadConfiguration[padnumber].bitkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys].bitMaskPressed = 1 << (tagTypeInt - TAG_BIT0);
                    computerConfiguration.bitKeypadConfiguration[padnumber].bitkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys].bitMaskReleased = computerConfiguration.bitKeypadConfiguration[padnumber].bitkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys].bitMaskPressed ^ 0xff;
                    computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys++;
                }
            break;
                
            case TAG_BIT:
                computerConfiguration.bitKeypadConfiguration[padnumber].bitkey.resize(computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys+1);
                
                bitNumber = child->GetNodeContent();
                computerConfiguration.bitKeypadConfiguration[padnumber].bitkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys].value = (int)parseXml_Number(*child, "key");
                computerConfiguration.bitKeypadConfiguration[padnumber].bitkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys].bitMaskPressed = 0;
                
                while (bitNumber != "")
                    computerConfiguration.bitKeypadConfiguration[padnumber].bitkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys].bitMaskPressed |= (1 << (int)getNextHexDec(&bitNumber));
                    
                computerConfiguration.bitKeypadConfiguration[padnumber].bitkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys].bitMaskReleased = computerConfiguration.bitKeypadConfiguration[padnumber].bitkey[computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys].bitMaskPressed ^ 0xff;
                
                computerConfiguration.bitKeypadConfiguration[padnumber].numberOfKeys++;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.bitKeypadConfiguration[padnumber].ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.bitKeypadConfiguration[padnumber].ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_PRESSED:
                computerConfiguration.bitKeypadConfiguration[padnumber].bitKeyPressed = (int)parseXml_Number(*child);
            break;

            case TAG_REPEAT:
                if (child->GetNodeContent() == "off")
                    computerConfiguration.bitKeypadConfiguration[padnumber].repeat = false;
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

void XmlParser::parseXml_CvKeypad(wxXmlNode &node)
{
    computerConfiguration.cvKeypadConfiguration.defined = true;
    
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
    wxString keyText, efText, bitNumber, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.cvKeypadConfiguration.ioGroupVector.clear();
    computerConfiguration.cvKeypadConfiguration.input = init_IoPort();
    computerConfiguration.cvKeypadConfiguration.ef = init_EfFlag();
    computerConfiguration.cvKeypadConfiguration.padMask = 0;
    computerConfiguration.cvKeypadConfiguration.modKey = 0;

    for (int i=0; i<255; i++)
        computerConfiguration.cvKeypadConfiguration.keydef[i] = -1;
    
    for (int i=0; i<LAST_MATRIX_TEXT_KEY; i++)
        computerConfiguration.cvKeypadConfiguration.textKey[i] = -1;

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
                computerConfiguration.cvKeypadConfiguration.input = parseXml_IoPort(*child, CV_KEYPAD_READ_DATA_IN);
            break;

            case TAG_EF:
                computerConfiguration.cvKeypadConfiguration.ef = parseXml_EfFlag(*child, CV_KEYPAD_DATA_READY_EF);
            break;

            case TAG_KEYDEF:
                if (child->GetAttribute("type") == "")
                {
                    keyValue = (int)parseXml_Number(*child)&0xff;
                    computerConfiguration.cvKeypadConfiguration.keydef[keyValue] = (int)parseXml_Number(*child, "value");
                }
                if (child->GetAttribute("type") == "text")
                {
                    keyText = child->GetNodeContent();
                    textEfKeyInt = 0;
                    while (textKeyList[textEfKeyInt] != "undefined")
                    {
                        if (keyText == textKeyList[textEfKeyInt])
                            computerConfiguration.cvKeypadConfiguration.textKey[textEfKeyInt] = (int)parseXml_Number(*child, "value");
                        textEfKeyInt++;
                    }
                }
            break;
                
            case TAG_PAD:
                computerConfiguration.cvKeypadConfiguration.padMask = (int)parseXml_Number(*child, "mask");
                keyText = child->GetNodeContent();
                textEfKeyInt = 0;
                while (padModList[textEfKeyInt] != "undefined")
                {
                    if (keyText == padModList[textEfKeyInt])
                    {
                        computerConfiguration.cvKeypadConfiguration.modKey = padModValueList[textEfKeyInt];
                        computerConfiguration.cvKeypadConfiguration.modKeyIgnore = padModIgnoreValueList[textEfKeyInt];
                    }
                    textEfKeyInt++;
                }
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.cvKeypadConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.cvKeypadConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_LatchKeypad(int pad, wxXmlNode &node)
{
    computerConfiguration.keyLatchConfiguration[pad].defined = true;

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
    wxString keyText, iogroup;
    size_t ioGroupNumber = 0;
    DiagonalKeys diagonalKeys;

//    for (int i=0; i<16; i++)
//    {
//        computerConfiguration.keyLatchConfiguration[pad].pc[i] = i;
//    }
    for (int i=0; i<512; i++)
    {
        computerConfiguration.keyLatchConfiguration[pad].pc[i] = -1;
        computerConfiguration.keyLatchConfiguration[pad].pcShift[i] = -1;
        computerConfiguration.keyLatchConfiguration[pad].pcForceShift[i] = -1;
        computerConfiguration.keyLatchConfiguration[pad].pcForceNoShift[i] = -1;
        computerConfiguration.keyLatchConfiguration[pad].pcMapShift[i] = -1;
        computerConfiguration.keyLatchConfiguration[pad].pcMap[i] = -1;
    }
    
    computerConfiguration.keyLatchConfiguration[pad].ioGroupVector.clear();
    computerConfiguration.keyLatchConfiguration[pad].ef = init_EfFlag();
    computerConfiguration.keyLatchConfiguration[pad].output = init_IoPort();
    computerConfiguration.keyLatchConfiguration[pad].altRight = -1;
    computerConfiguration.keyLatchConfiguration[pad].altLeft = -1;
    computerConfiguration.keyLatchConfiguration[pad].ctrlRight = -1;
    computerConfiguration.keyLatchConfiguration[pad].ctrlLeft = -1;
    computerConfiguration.keyLatchConfiguration[pad].shift = -1;
    computerConfiguration.keyLatchConfiguration[pad].switchAltCtrl = -1;
    computerConfiguration.keyLatchConfiguration[pad].loadAutoConfig = false;

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
                computerConfiguration.keyLatchConfiguration[pad].output = parseXml_IoPort(*child, LATCH_KEYPAD_OUT1+pad-1);
                if (computerConfiguration.keyLatchConfiguration[1].output.portNumber[0] == computerConfiguration.keyLatchConfiguration[2].output.portNumber[0])
                {
                    computerConfiguration.keyLatchConfiguration[1].output.ioDefinition = LATCH_KEYPAD_DOUBLE_OUT;
                    computerConfiguration.keyLatchConfiguration[2].output.ioDefinition = LATCH_KEYPAD_DOUBLE_OUT;
                }
                else
            break;
                
            case TAG_EF:
                computerConfiguration.keyLatchConfiguration[pad].ef = parseXml_EfFlag(*child, LATCH_KEYPAD_EF1+pad-1);
            break;
                
            case TAG_KEYDEF:
                if (child->GetAttribute("type") == "")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                    {
                        if (child->HasAttribute("out"))
                            computerConfiguration.keyLatchConfiguration[pad].pc[keyValue] = (int)parseXml_Number(*child, "out");
                    }
                    if (child->HasAttribute("map"))
                    {
                        computerConfiguration.keyLatchConfiguration[pad].pcMap[(int)parseXml_Number(*child, "map")] = (int)parseXml_Number(*child)&0x1ff;
                    }
                }
                
                if (child->GetAttribute("type") == "text")
                {
                    keyText = child->GetNodeContent();
                    if (keyText == "alt_right")
                        computerConfiguration.keyLatchConfiguration[pad].altRight = (int)parseXml_Number(*child, "out");
                    if (keyText == "alt_left")
                        computerConfiguration.keyLatchConfiguration[pad].altLeft = (int)parseXml_Number(*child, "out");
                    if (keyText == "ctrl_right")
                        computerConfiguration.keyLatchConfiguration[pad].ctrlRight = (int)parseXml_Number(*child, "out");
                    if (keyText == "ctrl_left")
                        computerConfiguration.keyLatchConfiguration[pad].ctrlLeft = (int)parseXml_Number(*child, "out");
                    if (keyText == "shift")
                        computerConfiguration.keyLatchConfiguration[pad].shift = (int)parseXml_Number(*child, "out");
                    if (keyText == "switch")
                        computerConfiguration.keyLatchConfiguration[pad].switchAltCtrl = (int)parseXml_Number(*child, "out");
                    if (keyText == "caps")
                        computerConfiguration.keyLatchConfiguration[pad].caps = (int)parseXml_Number(*child, "out");
                }

                if (child->GetAttribute("type") == "file")
                {
                    computerConfiguration.keyPadDefinitionFile = child->GetNodeContent();
                }

                if (child->GetAttribute("type") == "force")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                        if (child->HasAttribute("out"))
                            computerConfiguration.keyLatchConfiguration[pad].pcForceShift[keyValue] = (int)parseXml_Number(*child, "out");
                }

                if (child->GetAttribute("type") == "shift")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                        if (child->HasAttribute("out"))
                            computerConfiguration.keyLatchConfiguration[pad].pcShift[keyValue] = (int)parseXml_Number(*child, "out");
                    if (child->HasAttribute("map"))
                    {
                        computerConfiguration.keyLatchConfiguration[pad].pcMapShift[(int)parseXml_Number(*child, "map")] = (int)parseXml_Number(*child)&0x1ff;
                    }
                }
            break;
                
            case TAG_DIAGONAL:
                diagonalKeys.mainKey = (int)parseXml_Number(*child);
                diagonalKeys.key1 = (int)parseXml_Number(*child, "key1");
                diagonalKeys.key2 = (int)parseXml_Number(*child, "key2");
                computerConfiguration.keyLatchConfiguration[pad].diagonalKeys.push_back(diagonalKeys);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.keyLatchConfiguration[pad].ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.keyLatchConfiguration[pad].ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_AUTO:
                if (child->GetNodeContent() == "on")
                    computerConfiguration.keyLatchConfiguration[pad].loadAutoConfig = true;
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

void XmlParser::parseXml_FredKeypad(wxXmlNode &node)
{
    computerConfiguration.fredKeypadConfiguration.defined= true;

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
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.fredKeypadConfiguration.ioGroupVector.clear();
    computerConfiguration.fredKeypadConfiguration.input_mode = 1;
    computerConfiguration.fredKeypadConfiguration.ef_mode = 1;
    computerConfiguration.fredKeypadConfiguration.coinArcadeControl = false;
    computerConfiguration.fredKeypadConfiguration.output = init_IoPort();
    computerConfiguration.fredKeypadConfiguration.inputKey = init_IoPort();
    computerConfiguration.fredKeypadConfiguration.inputKey.portNumber[0] = 0;
    computerConfiguration.fredKeypadConfiguration.inputCoin = init_IoPort();
    computerConfiguration.fredKeypadConfiguration.efKey = init_EfFlag();
    computerConfiguration.fredKeypadConfiguration.efKey.flagNumber = 1;
    computerConfiguration.fredKeypadConfiguration.efFireB = init_EfFlag();
    computerConfiguration.fredKeypadConfiguration.efCoin = init_EfFlag();

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
                computerConfiguration.fredKeypadConfiguration.output = parseXml_IoPort(*child, KEY_FRED_MODE_OUT);
            break;
                
            case TAG_IN:
                if (child->GetAttribute("type") == "key")
                {
                    computerConfiguration.fredKeypadConfiguration.inputKey = parseXml_IoPort(*child, KEY_TAPE_FRED_READ_DATA_IN);
                    if (child->HasAttribute("mode"))
                    {
                        computerConfiguration.fredKeypadConfiguration.input_mode = (int)parseXml_Number(*child, "mode");
                    }
                }
                if (child->GetAttribute("type") == "coin")
                {
                    computerConfiguration.fredKeypadConfiguration.coinArcadeControl = true;
                    computerConfiguration.fredKeypadConfiguration.inputCoin = parseXml_IoPort(*child, KEY_FRED_COIN_ARCADE_IN);
                }
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "key")
                {
                    computerConfiguration.fredKeypadConfiguration.efKey = parseXml_EfFlag(*child, KEY_TAPE_FRED_EF);
                    if (child->HasAttribute("mode"))
                    {
                        computerConfiguration.fredKeypadConfiguration.ef_mode = (int)parseXml_Number(*child, "mode");
                    }
                }
                if (child->GetAttribute("type") == "fireb")
                {
                    computerConfiguration.fredKeypadConfiguration.efFireB = parseXml_EfFlag(*child, KEY_FRED_COIN_ARCADE_EF);
                }
                if (child->GetAttribute("type") == "coin" || child->GetAttribute("type") == "error")
                {
                    computerConfiguration.fredKeypadConfiguration.efCoin = parseXml_EfFlag(*child, TAPE_ERROR_FRED_EF);
                }
            break;
                
            case TAG_MODE:
                if (child->GetNodeContent() == "hex")
                {
                    computerConfiguration.fredKeypadConfiguration.keypad_mode = FRED_HEX_MODE;
                }
                if (child->GetNodeContent() == "byte")
                {
                    computerConfiguration.fredKeypadConfiguration.keypad_mode = FRED_BYTE_MODE;
                }
                if (child->GetNodeContent() == "p-hex")
                {
                    computerConfiguration.fredKeypadConfiguration.keypad_mode = FRED_HEX_PULSE_MODE;
                }
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.fredKeypadConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.fredKeypadConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_EtiKeypad(wxXmlNode &node)
{
    computerConfiguration.etiKeypadConfiguration.defined= true;

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
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.etiKeypadConfiguration.ioGroupVector.clear();
    computerConfiguration.etiKeypadConfiguration.output = init_IoPort();
    computerConfiguration.etiKeypadConfiguration.output.portNumber[0] = 2;
    computerConfiguration.etiKeypadConfiguration.input = init_IoPort();
    computerConfiguration.etiKeypadConfiguration.input.portNumber[0] = 2;
    computerConfiguration.etiKeypadConfiguration.ef = init_EfFlag();
    computerConfiguration.etiKeypadConfiguration.ef.flagNumber = 4;

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
                computerConfiguration.etiKeypadConfiguration.output  = parseXml_IoPort(*child, ETI_KEYPAD_OUT);
            break;
                
            case TAG_IN:
                computerConfiguration.etiKeypadConfiguration.input  = parseXml_IoPort(*child, ETI_KEYPAD_IN);
            break;
                
            case TAG_EF:
                computerConfiguration.etiKeypadConfiguration.ef  = parseXml_EfFlag(*child, ETI_KEYPAD_EF);
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.etiKeypadConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.etiKeypadConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_EfButtons(wxXmlNode &node)
{
    computerConfiguration.efButtonsConfiguration.defined = true;
    computerConfiguration.efButtonsConfiguration.keyPressed = 0;
    for (int i=1; i<5; i++)
    {
        computerConfiguration.efButtonsConfiguration.key[i].defined = false;
        computerConfiguration.efButtonsConfiguration.key[i].mod = false;
    }

    wxString tagList[]=
    {
        "ef1",
        "ef2",
        "ef3",
        "ef4",
        "keydef",
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
        TAG_KEYDEF,
        TAG_PRESSED,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString efString;
    int efNumber, efButtonNumber = 0;

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
                    computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].modString = child->GetNodeContent();
                    if (computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].modString == "alt")
                    {
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].defined = true;
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].mod = true;
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].value = wxMOD_ALT;
                    }
                    if (computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].modString == "control")
                    {
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].defined = true;
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].mod = true;
#ifdef __WXMAC__
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].value = wxMOD_RAW_CONTROL;
#else
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].value = wxMOD_CONTROL;
#endif
                    }
                    if (computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].modString == "shift")
                    {
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].defined = true;
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].mod = true;
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].value = wxMOD_SHIFT;
                    }
                    if (computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].modString == "altgr")
                    {
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].defined = true;
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].mod = true;
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].value = wxMOD_ALTGR;
                    }
                    if (computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].modString == "meta")
                    {
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].defined = true;
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].mod = true;
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].value = wxMOD_META;
                    }
                    if (computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].modString == "command")
                    {
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].defined = true;
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].mod = true;
                        computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].value = wxMOD_CONTROL;
                    }
                }
                else
                {
                    computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].resetInp = -1;
                    if (child->GetAttribute("type") == "reset")
                    {
                        if (child->HasAttribute("in"))
                            computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].resetInp = (int)parseXml_Number(*child, "in");
                    }
                    computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].defined = true;
                    computerConfiguration.efButtonsConfiguration.key[tagTypeInt-TAG_EF1+1].value = (int)parseXml_Number(*child);
                }
            break;
 
            case TAG_KEYDEF:
                computerConfiguration.efButtonsConfiguration.efButton.resize(efButtonNumber+1);
                computerConfiguration.efButtonsConfiguration.efButton[efButtonNumber].value = (int)parseXml_Number(*child, "key");
                efString = child->GetNodeContent();
                efNumber = 0;
                while (efString != "")
                {
                    computerConfiguration.efButtonsConfiguration.efButton[efButtonNumber].flag.resize(efNumber+1);
                    computerConfiguration.efButtonsConfiguration.efButton[efButtonNumber].flag[efNumber] = (int)getNextHexDec(&efString) & 0x7;
                    computerConfiguration.efButtonsConfiguration.key[computerConfiguration.efButtonsConfiguration.efButton[efButtonNumber].flag[efNumber++]].defined = true;
                }
                efButtonNumber++;
            break;
                
            case TAG_PRESSED:
                computerConfiguration.efButtonsConfiguration.keyPressed = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_AdConvertor(wxXmlNode &node)
{
    computerConfiguration.adConvertorConfiguration.defined = true;

    wxString tagList[]=
    {
        "out",
        "in",
        "address",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_ADDRESS,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long start, end;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.adConvertorConfiguration.ioGroupVector.clear();
    computerConfiguration.adConvertorConfiguration.addressStart = -1;
    computerConfiguration.adConvertorConfiguration.addressEnd = -1;
    computerConfiguration.adConvertorConfiguration.output = init_IoPort();
    computerConfiguration.adConvertorConfiguration.input = init_IoPort();
	IoPort addressPort;

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
                if (child->GetAttribute("type") == "adi")
                {
                	addressPort = parseXml_IoPort(*child);
                	if (addressPort.portNumber.size() == 1)
                	{
                	computerConfiguration.adConvertorConfiguration.addressStart = addressPort.portNumber[0];
                    computerConfiguration.adConvertorConfiguration.addressEnd = addressPort.portNumber[0];
                	}
                	else 
                	{
                	computerConfiguration.adConvertorConfiguration.addressStart = addressPort.portNumber[0];
                    computerConfiguration.adConvertorConfiguration.addressEnd = addressPort.portNumber[1];
                	}
                }
                else
                	computerConfiguration.adConvertorConfiguration.input = parseXml_IoPort(*child, AD_CONVERTOR_IN);
            break;
                                
            case TAG_OUT:
                computerConfiguration.adConvertorConfiguration.output = parseXml_IoPort(*child, AD_CONVERTOR_OUT);
            break;

            case TAG_ADDRESS:
                if (!parseXml_Range(*child, &start, &end))
                {
                    computerConfiguration.adConvertorConfiguration.addressStart = (Word) start;
                    computerConfiguration.adConvertorConfiguration.addressEnd = (Word) start;
                }
                else
                {
                    computerConfiguration.adConvertorConfiguration.addressStart = (Word) start;
                    computerConfiguration.adConvertorConfiguration.addressEnd = (Word) end;
                }
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.adConvertorConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.adConvertorConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_FrontPanel(wxXmlNode &node, int frontNumber)
{
    computerConfiguration.hexDisplayConfiguration.defined = true;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "int",
        "cycle",
        "filename",
        "dirname",
        "item",
        "size",
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
        TAG_INTERRUPT,
        TAG_CYCLE,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_ITEM,
        TAG_SIZE,
        TAG_POS,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    int xSize, ySize;
    wxString position, iogroup;
    size_t ioGroupNumber = 0;

    guiItemConfigNumber_ = 0;
    
    computerConfiguration.multiSegDisplayConfiguration.segOutput = init_IoPort();
    computerConfiguration.multiSegDisplayConfiguration.segInput = init_IoPort();
    computerConfiguration.multiSegDisplayConfiguration.segEf = init_EfFlag();

    computerConfiguration.hexDisplayConfiguration.ioGroupVector.clear();
    computerConfiguration.hexDisplayConfiguration.ef = init_EfFlag();
    computerConfiguration.hexDisplayConfiguration.inEf = init_EfFlag();
    computerConfiguration.hexDisplayConfiguration.inEfElf = init_EfFlag();
    computerConfiguration.hexDisplayConfiguration.startEf = init_EfFlag();
    computerConfiguration.ledDisplayConfiguration.output = init_IoPort();

    computerConfiguration.frontPanelConfiguration[frontNumber].defaultPos = wxPoint(0, 0);
    computerConfiguration.frontPanelConfiguration[frontNumber].posType = POS_TYPE_RELATIVE;
    
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
                if (child->GetAttribute("type") == "multitil")
                    computerConfiguration.multiSegDisplayConfiguration.segOutput = parseXml_IoPort(*child, MULTI_TIL_DISPLAY_OUT);
                else
                    computerConfiguration.hexDisplayConfiguration.output = parseXml_IoPort(*child, HEX_DISPLAY_OUT);
            break;
                
            case TAG_IN:
                if (child->GetAttribute("type") == "multitil")
                    computerConfiguration.multiSegDisplayConfiguration.segInput = parseXml_IoPort(*child, MULTI_TIL_DISPLAY_IN);
                else
                {
                    if (child->GetAttribute("type") == "direct")
                    {
                        computerConfiguration.hexDisplayConfiguration.input = parseXml_IoPort(*child, SWITCHES_IN_DIRECT);
                    }
                    else
                    {
                        computerConfiguration.useHexKeyboard = true;
                        computerConfiguration.hexDisplayConfiguration.input = parseXml_IoPort(*child, HEX_KEY_IN);
                    }
                }
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "data" || child->GetAttribute("type") == "ready" )
                    computerConfiguration.hexDisplayConfiguration.ef = parseXml_EfFlag(*child, HEX_KEY_EF);
                if (child->GetAttribute("type") == "in")
                    computerConfiguration.hexDisplayConfiguration.inEf = parseXml_EfFlag(*child);
                if (child->GetAttribute("type") == "in_elf")
                    computerConfiguration.hexDisplayConfiguration.inEfElf = parseXml_EfFlag(*child);
                if (child->GetAttribute("type") == "start")
                    computerConfiguration.hexDisplayConfiguration.startEf = parseXml_EfFlag(*child);
                if (child->GetAttribute("type") == "multitil")
                    computerConfiguration.multiSegDisplayConfiguration.segEf = parseXml_EfFlag(*child, MULTI_TIL_DISPLAY_EF);

            break;

            case TAG_INTERRUPT:
                computerConfiguration.frontPanelConfiguration[frontNumber].picInterrupt = (int)parseXml_Number(*child);
            break;

            case TAG_CYCLE:
                if (child->GetAttribute("type") == "multitil")
                    computerConfiguration.multiSegDisplayConfiguration.cycleValue = (int)parseXml_Number(*child);
            break;
                
            case TAG_ITEM:
                parseXml_FrontPanelItem (*child, frontNumber);
            break;

            case TAG_SIZE:
                position = child->GetNodeContent();
                xSize = (int)getNextHexDec(&position);
                ySize = (int)getNextHexDec(&position);
                computerConfiguration.frontPanelConfiguration[frontNumber].size = wxSize(xSize, ySize);
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                computerConfiguration.frontPanelConfiguration[frontNumber].defaultPos.x = (int)getNextHexDec(&position);
                computerConfiguration.frontPanelConfiguration[frontNumber].defaultPos.y = (int)getNextHexDec(&position);
                if (child->GetAttribute("type") == "grid")
                    computerConfiguration.frontPanelConfiguration[frontNumber].posType = POS_TYPE_GRID;
                if (child->GetAttribute("type") == "real")
                    computerConfiguration.frontPanelConfiguration[frontNumber].posType = POS_TYPE_REAL;
            break;

            case TAG_FILENAME:
                if (child->HasAttribute("pol"))
                {
                    if (child->GetAttribute("pol") == "rev")
                        computerConfiguration.multiSegDisplayConfiguration.reversePolarity = true;
                }
                if (child->HasAttribute("high"))
                    computerConfiguration.multiSegDisplayConfiguration.multitilAddressLineHigh = (int)parseXml_Number(*child, "high");
                if (child->HasAttribute("wait"))
                    computerConfiguration.multiSegDisplayConfiguration.multitilAddressLineWait = (int)parseXml_Number(*child, "wait");

                computerConfiguration.multiSegDisplayConfiguration.tilFontFile = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.multiSegDisplayConfiguration.tilFontDirectory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.multiSegDisplayConfiguration.tilFontDirectory.Right(1) != pathSeparator_)
                    computerConfiguration.multiSegDisplayConfiguration.tilFontDirectory += pathSeparator_;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.hexDisplayConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.hexDisplayConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_FrontPanelItem(wxXmlNode &node, int frontNumber)
{
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
        "super_monitor",
        "super_mp",
        "stop",
        "read",
        "nano_monitor",
        "cardswitch",
        "power",
        "wait",
        "clear",
        "clear_run",
        "clear_reset",
        "nvram",
        "in_int",
        "in_switch",
        "card",
        "dipmp",
        "ems",
        "tmc_ad",
        "bitswitch",
        "efswitch",
        "velf",
        "hex",
        "ef",
        "thumb_minus",
        "thumb_plus",
        "powerled",
        "stopled",
        "readyled",
        "errorled",
        "qled",
        "resetled",
        "pauseled",
        "runled",
        "loadled",
        "bitled",
        "switchled",
        "buttonled",
        "addressled",
        "nibbleled",
        "fetchled",
        "executeled",
        "waitled",
        "clearled",
        "dmaled",
        "interruptled",
        "diskled",
        "cpustateled",
        "datatil",
        "addresstil",
        "multitil",
        "undefined"
    };

    int itemFunctionValueList[]=
    {
        BUTTON_FUNC_IN,             // 0
        BUTTON_FUNC_RUN,            // 1
        BUTTON_FUNC_MP,             // 2
        BUTTON_FUNC_RUN0,           // 3
        BUTTON_FUNC_LOAD,
        BUTTON_FUNC_PAUSE,          // 5
        BUTTON_FUNC_STEP,
        BUTTON_FUNC_RESET,          // 7
        BUTTON_FUNC_SUPER_MONITOR,
        BUTTON_FUNC_SUPER_MP,
        BUTTON_FUNC_STOP,           // 9
        BUTTON_FUNC_READ,
        BUTTON_FUNC_NANO_MONITOR,   // 11
        BUTTON_FUNC_CARDSWITCH,
        BUTTON_FUNC_POWER,
        BUTTON_FUNC_WAIT,           // 14
        BUTTON_FUNC_CLEAR,
        BUTTON_FUNC_CLEAR_RUN,
        BUTTON_FUNC_CLEAR_RESET,    // 17
        BUTTON_FUNC_NVRAM_DISABLE,
        BUTTON_FUNC_IN_INTERRUPT,
        BUTTON_FUNC_IN_SWITCH,
        BUTTON_FUNC_CARD,
        BUTTON_FUNC_DIPMP,          // 21
        BUTTON_FUNC_EMS,
        BUTTON_FUNC_TMC_AD,
        BUTTON_FUNC_BIT,            // 24
        BUTTON_FUNC_EF_SWITCH,      // 25
        BUTTON_FUNC_VELF,
        BUTTON_FUNC_HEX,
        BUTTON_FUNC_EF,
        BUTTON_FUNC_THUMB_MINUS,
        BUTTON_FUNC_THUMB_PLUS,
        LED_FUNC_POWER,             // 28
        LED_FUNC_STOP,
        LED_FUNC_READY,
        LED_FUNC_ERROR,             // 31
        LED_FUNC_Q,
        LED_FUNC_RESET,
        LED_FUNC_PAUSE,             // 34
        LED_FUNC_RUN,
        LED_FUNC_LOAD,
        LED_FUNC_BIT,               // 37
        LED_FUNC_SWITCH,
        LED_FUNC_BUTTON,
        LED_FUNC_ADDRESS,
        LED_FUNC_NIBBLE,
        LED_FUNC_FETCH,
        LED_FUNC_EXECUTE,
        LED_FUNC_WAIT,
        LED_FUNC_CLEAR,
        LED_FUNC_DMA,
        LED_FUNC_INTERRUPT,
        LED_FUNC_DISK,
        LED_FUNC_CPUSTATE,
        TIL_DATA,
        TIL_ADDRESS,
        TIL_MULTI,
        0
    };

    int tagTypeInt, itemFunctionId;
    int xpos, ypos;
    int xSize, ySize;
    int rangeLow, rangeHigh;
    int addressLine = -1;
    bool dpQvalue = false;
    wxString position, size, range, itemFunctionText;

    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration.resize(guiItemConfigNumber_+1);
    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = -1;
    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = -1;
    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].rangeLow = -1;
    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].rangeHigh = -1;
    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function = -1;
    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnRelease = false;
    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnPress = false;
    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].label = "";
    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].position = wxPoint(0,0);
    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].textSize = 12;
    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].initup = BUTTON_DOWN;
    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].dirName = computerConfiguration.mainDir_;
    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].useImageDir = false;
    
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
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnRelease = true;
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = SWITCH_BUTTON_VERTICAL;
                    if (child->HasAttribute("direction"))
                    {
                        if (child->GetAttribute("direction") == "horizontal")
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = SWITCH_BUTTON_HORIZONTAL;
                    }
                    if (child->HasAttribute("form"))
                    {
                        if (child->GetAttribute("form") == "elf2k_power")
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = ELF2K_POWER_BUTTON;
                        if (child->GetAttribute("form") == "square")
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = SWITCH_BUTTON_SQUARE_GREEN;
                    }
                }
                
                if (child->GetNodeContent() == "dipswitch")
                {
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnRelease = true;
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = DIP_SWITCH_BUTTON;
                }

                if (child->GetNodeContent() == "rot")
                {
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnRelease = true;
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = ROT_SWITCH_BUTTON;
                }

                if (child->GetNodeContent() == "thumb_minus")
                {
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnPress = true;
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = THUMB_MINUS_BUTTON;
                }
                
                if (child->GetNodeContent() == "thumb_plus")
                {
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnPress = true;
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = THUMB_PLUS_BUTTON;
                }
                
                if (child->GetNodeContent() == "spin_adi")
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = ADI_SPINCTRL;

                if (child->GetNodeContent() == "spin_adi_volt")
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = ADI_VOLT_SPINCTRL;

                if (child->GetNodeContent() == "spin_ads")
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = ADS_SPINCTRL;

                if (child->GetNodeContent() == "spin_ads_volt")
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = ADS_VOLT_SPINCTRL;

                if (child->GetNodeContent() == "push")
                {
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnPress = true;
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON;
                    if (child->HasAttribute("size"))
                    {
                        if (child->GetAttribute("size") == "small")
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON_SMALL;
                        if (child->GetAttribute("size") == "large")
                        {
                            if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == PUSH_BUTTON_GREEN)
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON_ROUND_GREEN_LARGE;
                            else
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON_ROUND_RED_LARGE;
                        }
                    }

                    if (child->HasAttribute("form"))
                    {
                        if (child->GetAttribute("form") == "rectangle")
                        {
                            if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == PUSH_BUTTON_SMALL)
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON_RECTANGLE_SMALL;
                            else
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON_RECTANGLE;
                        }
                        if (child->GetAttribute("form") == "round")
                        {
                            if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type != PUSH_BUTTON_ROUND_GREEN_LARGE && computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type != PUSH_BUTTON_ROUND_RED_LARGE)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON_ROUND_BLACK;
                        }
                    }
                }

                if (child->HasAttribute("color"))
                {
                    if (child->GetAttribute("color") == "red")
                    {
                        if(computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == PUSH_BUTTON)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON_RED;
                        if(computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == PUSH_BUTTON_ROUND_BLACK)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON_ROUND_RED;
                        if(computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == SWITCH_BUTTON_VERTICAL)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = SWITCH_BUTTON_VERTICAL_RED;
                    }
                    if (child->GetAttribute("color") == "blue")
                    {
                        if(computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == PUSH_BUTTON)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON_BLUE;
                        if(computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == SWITCH_BUTTON_VERTICAL)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = SWITCH_BUTTON_BLUE;
                    }
                    if (child->GetAttribute("color") == "orange")
                    {
                        if(computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == PUSH_BUTTON)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON_ORANGE;
                        if(computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == SWITCH_BUTTON_VERTICAL)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = SWITCH_BUTTON_ORANGE;
                    }
                    if (child->GetAttribute("color") == "yellow")
                    {
                        if(computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == PUSH_BUTTON)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON_YELLOW;
                        if(computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == SWITCH_BUTTON_VERTICAL)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = SWITCH_BUTTON_YELLOW;
                    }
                    if (child->GetAttribute("color") == "green")
                    {
                        if(computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == PUSH_BUTTON)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON_GREEN;
                        if(computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == PUSH_BUTTON_ROUND_RED_LARGE)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PUSH_BUTTON_ROUND_GREEN_LARGE;
             //           if(computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == SWITCH_BUTTON_VERTICAL)
             //               computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = SWITCH_BUTTON_SQUARE_GREEN;
                        if(computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type == SWITCH_BUTTON_VERTICAL)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = SWITCH_BUTTON_VERTICAL_GREEN;
                    }
                }

                if (child->GetNodeContent() == "text")
                {
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].color = GUI_COL_BLACK;
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PANEL_TEXT;
                    if (child->HasAttribute("color"))
                    {
                        if (child->GetAttribute("color") == "grey")
                        {
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].color = GUI_COL_GREY;
                        }
                        if (child->GetAttribute("color") == "white")
                        {
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].color = GUI_COL_WHITE;
                        }
                    }
                }

                if (child->GetNodeContent() == "thumb_text")
                {
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].color = GUI_COL_BLACK;
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = THUMB_TEXT;
                    if (child->HasAttribute("color"))
                    {
                        if (child->GetAttribute("color") == "grey")
                        {
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].color = GUI_COL_GREY;
                        }
                        if (child->GetAttribute("color") == "white")
                        {
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].color = GUI_COL_WHITE;
                        }
                    }
                }

                if (child->GetNodeContent() == "png")
                {
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = PANEL_PNG;
                }
                
                if (child->GetNodeContent() == "led")
                {
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = LED_SMALL_RED;
                    if (child->GetAttribute("form") == "realred")
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = LED_REAL_RED;
                    if (child->GetAttribute("form") == "realgreen")
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = LED_REAL_GREEN;
                    if (child->GetAttribute("form") == "realorange")
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = LED_REAL_ORANGE;
                    if (child->GetAttribute("form") == "large")
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = LED_LARGE_RED;
                    if (child->GetAttribute("form") == "largegreen")
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = LED_LARGE_GREEN;
                    if (child->GetAttribute("form") == "largeorange")
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = LED_LARGE_ORANGE;
                    if (child->GetAttribute("form") == "largecolor")
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = LED_LARGE_COLOR;
                    if (child->GetAttribute("form") == "disable")
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = LED_SMALL_RED_DISABLE;
                }

                if (child->GetNodeContent() == "til")
                {
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = TIL_311;
                    if (child->GetAttribute("form") == "313")
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = TIL_313;
                    if (child->GetAttribute("font") == "italic")
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type = TIL_313_ITALIC;
                }

                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type >= SWITCH_BUTTON_VERTICAL && computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type < SWITCH_BUTTON_VERTICAL_PIO && computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function == BUTTON_FUNC_BIT)
                    computerConfiguration.mainFrontPanelConfiguration.dataSwitchButtons = true;
                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type >= PUSH_BUTTON && computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function == BUTTON_FUNC_BIT)
                    computerConfiguration.mainFrontPanelConfiguration.dataPushButtons = true;
            break;

            case TAG_VALUE:
                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = (int)parseXml_Number(*child);
                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function >= BUTTON_FUNC_IN && computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function < BUTTON_FUNC_CARD)
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function;
                
                if (child->HasAttribute("address"))
                    addressLine = (int)parseXml_Number(*child, "address");

                switch (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function)
                {
                    case BUTTON_FUNC_DIPMP:
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += 0x50;
                    break;

                    case BUTTON_FUNC_EMS:
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += 0x60;
                    break;

                    case BUTTON_FUNC_TMC_AD:
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += 0x70;
                    break;

                    case BUTTON_FUNC_BIT:
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += 0x80;
                    break;

                    case BUTTON_FUNC_EF_SWITCH:
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += 0x90;
                    break;
                        
                    case BUTTON_FUNC_CARD:
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += 0x100;
                    break;

                    case LED_FUNC_BIT:
                        if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value >= 8)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = 0;
                        if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value != -1)
                    break;

                    case LED_FUNC_ADDRESS:
                        if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value >= MAX_BIT_LEDS - 8)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = 0;
                        if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value != -1)
                    break;

                    case LED_FUNC_CPUSTATE:
                        if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value >= MAX_CPU_STATE_LEDS)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = 0;
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += SC0LED;
                    break;
                        
                    case TIL_DATA:
                        if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value >= MAX_DATA_TIL)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = 0;
                        if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value != -1)
                        {
                            computerConfiguration.ledDisplayConfiguration.datatil[computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value] = true;
                            computerConfiguration.ledDisplayConfiguration.datatilDpQ[computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value] = dpQvalue;
                        }
                    break;

                    case TIL_ADDRESS:
                        if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value >= MAX_ADDRESS_TIL)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = 0;
                        if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value != -1)
                            computerConfiguration.ledDisplayConfiguration.addresstil[computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value] = true;
                    break;

                    case TIL_MULTI:
                        if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value >= MAX_MULTI_TIL)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = 0;
                        if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value != -1)
                            computerConfiguration.multiSegDisplayConfiguration.multitil[computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value] = true;
                        if (addressLine != -1)
                            computerConfiguration.multiSegDisplayConfiguration.multitilAddressLine[computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value] = addressLine;
                    break;
                }
            break;
                
            case TAG_RANGE:
                range = child->GetNodeContent();
                rangeLow = (int)getNextHexDec(&range);
                rangeHigh = (int)getNextHexDec(&range);
                if (rangeLow > rangeHigh)
                {
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].rangeLow = rangeHigh;
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].rangeHigh = rangeLow;
                }
                else
                {
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].rangeLow = rangeLow;
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].rangeHigh = rangeHigh;
                }
            break;

            case TAG_FUNCTION:
                itemFunctionText = child->GetNodeContent();
                itemFunctionId = 0;
                while (itemFunctionList[itemFunctionId] != "undefined")
                {
                    if (itemFunctionText == itemFunctionList[itemFunctionId])
                    {
                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function = itemFunctionValueList[itemFunctionId];
                        if (itemFunctionId >= BUTTON_FUNC_IN && itemFunctionId < BUTTON_FUNC_CARD)
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = itemFunctionId+20;

                        computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].reversePol = child->GetAttribute("pol") == "rev";

                        switch (itemFunctionId)
                        {
                            case BUTTON_FUNC_IN:
                            case BUTTON_FUNC_IN_SWITCH:
                            case BUTTON_FUNC_IN_INTERRUPT:
                            case BUTTON_FUNC_EF:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnRelease = true;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnPress = true;
                            break;

                            case BUTTON_FUNC_HEX:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnRelease = true;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnPress = true;
                                computerConfiguration.mainFrontPanelConfiguration.dataPushButtons = true;
                            break;

                            case BUTTON_FUNC_POWER:
                                computerConfiguration.mainFrontPanelConfiguration.powerButtonDefined = true;
                            break;
                                
                            case BUTTON_FUNC_WAIT:
                                computerConfiguration.mainFrontPanelConfiguration.waitButtonDefined = true;
                            break;
                                
                            case BUTTON_FUNC_DIPMP:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += 0x50;
                            break;

                            case BUTTON_FUNC_EMS:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += 0x60;
                            break;

                            case BUTTON_FUNC_TMC_AD:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += 0x70;
                            break;

                            case BUTTON_FUNC_BIT:
                                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type >= SWITCH_BUTTON_VERTICAL && computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type < SWITCH_BUTTON_VERTICAL_PIO)
                                    computerConfiguration.mainFrontPanelConfiguration.dataSwitchButtons = true;
                                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].type >= PUSH_BUTTON)
                                    computerConfiguration.mainFrontPanelConfiguration.dataPushButtons = true;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += 0x80;
                            break;

                            case BUTTON_FUNC_EF_SWITCH:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += 0x8f;
                            break;

                            case BUTTON_FUNC_CARD:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += 0x100;
                            break;

                            case BUTTON_FUNC_RUN:
                                if (child->GetAttribute("type") == "reset")
                                    computerConfiguration.runPressType = RUN_TYPE_RESET;
                                if (child->GetAttribute("type") == "boot")
                                    computerConfiguration.runPressType = RUN_TYPE_BOOT;
                                if (child->GetAttribute("type") == "switch")
                                    computerConfiguration.runPressType = RUN_TYPE_SWITCH;
                                if (child->GetAttribute("type") == "velf")
                                    computerConfiguration.runPressType = RUN_TYPE_VELF;
                                if (child->GetAttribute("type") == "uc1800")
                                    computerConfiguration.runPressType = RUN_TYPE_UC1800;
                                if (child->GetAttribute("type") == "microtutor")
                                    computerConfiguration.runPressType = RUN_TYPE_MICROTUTOR;
                            break;

                            case BUTTON_FUNC_RUN0:
                                if (child->GetAttribute("type") == "reset")
                                    computerConfiguration.runPressType = RUN_TYPE_RESET;
                                if (child->GetAttribute("type") == "boot")
                                    computerConfiguration.runPressType = RUN_TYPE_BOOT;
                            break;

                            case BUTTON_FUNC_RESET:
                                if (child->GetAttribute("type") == "full")
                                    computerConfiguration.resetPressType = RESET_TYPE_FULL;
                            break;
                                
                            case BUTTON_FUNC_STEP:
                                if (child->GetAttribute("type") == "cosmicos")
                                    computerConfiguration.stepPressType = STEP_TYPE_COSMICOS;
                            break;

                            case LED_FUNC_LOAD:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = LOADLED;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function = LED_FUNC_CPUSTATE;
                            break;

                            case LED_FUNC_RESET:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = RESETLED;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function = LED_FUNC_CPUSTATE;
                            break;

                            case LED_FUNC_PAUSE:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = PAUSELED;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function = LED_FUNC_CPUSTATE;
                            break;

                            case LED_FUNC_RUN:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = RUNLED;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function = LED_FUNC_CPUSTATE;
                            break;

                            case LED_FUNC_FETCH:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = SC0LED;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function = LED_FUNC_CPUSTATE;
                            break;

                            case LED_FUNC_EXECUTE:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = SC1LED;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function = LED_FUNC_CPUSTATE;
                            break;

                            case LED_FUNC_WAIT:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = WAITLED;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function = LED_FUNC_CPUSTATE;
                            break;

                            case LED_FUNC_CLEAR:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = CLEARLED;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function = LED_FUNC_CPUSTATE;
                            break;

                            case LED_FUNC_DMA:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = DMALED;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function = LED_FUNC_CPUSTATE;
                            break;
                                
                            case LED_FUNC_DISK:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = DISKLED;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function = LED_FUNC_CPUSTATE;
                            break;
                                
                            case LED_FUNC_INTERRUPT:
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value = INTERRUPTLED;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].function = LED_FUNC_CPUSTATE;
                            break;
                                
                            case LED_FUNC_NIBBLE:
                                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value != -1)
                                    computerConfiguration.mainFrontPanelConfiguration.nibbleLeds = true;
                            break;

                            case LED_FUNC_BIT:
                                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value != -1)
                                if (child->GetAttribute("load") == "show")
                                    computerConfiguration.ledDisplayConfiguration.showDataOnLoad = true;
                                if (child->GetAttribute("cycle") == "show")
                                    computerConfiguration.ledDisplayConfiguration.showDataOnCycle = true;
                            break;

                            case LED_FUNC_BUTTON:
                                computerConfiguration.ledDisplayConfiguration.followButton = true;
                            break;
                                
                            case LED_FUNC_SWITCH:
                                computerConfiguration.ledDisplayConfiguration.followSwitches = true;
                            break;
                                
                            case LED_FUNC_ADDRESS:
                                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value != -1)
                                if (child->GetAttribute("cycle") == "show")
                                    computerConfiguration.ledDisplayConfiguration.showAddressOnCycle = true;
                            break;

                            case LED_FUNC_CPUSTATE:
                                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value != -1)
                                {
                                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value += SC0LED;
                                }
                            break;

                            case TIL_DATA:
                                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value != -1)
                                {
                                    computerConfiguration.ledDisplayConfiguration.datatil[computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value] = true;
                                    if (child->GetAttribute("dp") == "q")
                                        computerConfiguration.ledDisplayConfiguration.datatilDpQ[computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value] = true;
                                }
                                else
                                {
                                    if (child->GetAttribute("dp") == "q")
                                        dpQvalue = true;
                                }
                                if (child->GetAttribute("cycle") == "show")
                                    computerConfiguration.ledDisplayConfiguration.showDataOnCycle = true;
                            break;

                            case TIL_ADDRESS:
                                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value != -1)
                                    computerConfiguration.ledDisplayConfiguration.addresstil[computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value] = true;
                                if (child->GetAttribute("cycle") == "show")
                                    computerConfiguration.ledDisplayConfiguration.showAddressOnCycle = true;
                            break;

                            case TIL_MULTI:
                                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value != -1)
                                    computerConfiguration.multiSegDisplayConfiguration.multitil[computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value] = true;
                                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value > computerConfiguration.multiSegDisplayConfiguration.multiTilNumber)
                                {
                                    computerConfiguration.multiSegDisplayConfiguration.multiTilNumber = computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].value;
                                    if (addressLine != -1)
                                        computerConfiguration.multiSegDisplayConfiguration.multitilAddressLine[computerConfiguration.multiSegDisplayConfiguration.multiTilNumber] = addressLine;
                                }
                            break;
                        }
                        if (child->HasAttribute("action"))
                        {
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnPress = false;
                            computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnRelease = false;
                            if (child->GetAttribute("action") == "press")
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnPress = true;
                            if (child->GetAttribute("action") == "release")
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnRelease = true;
                            if (child->GetAttribute("action") == "both")
                            {
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnPress = true;
                                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].actOnRelease = true;
                            }
                        }
                    }
                    itemFunctionId++;
                }
            break;

            case TAG_LABEL:
                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].label = child->GetNodeContent();
                if (child->HasAttribute("size"))
#if defined (__WXMAC__)
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].textSize = (int)parseXml_Number(*child, "size")+4;
#else
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].textSize = (int)parseXml_Number(*child, "size");
#endif
            break;
                
            case TAG_INIT:
                if (child->GetNodeContent() == "up")
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].initup = BUTTON_UP;
            break;
                
            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].position = wxPoint(xpos, ypos);
            break;

            case TAG_SIZE:
                size = child->GetNodeContent();
                xSize = (int)getNextHexDec(&size);
                ySize = (int)getNextHexDec(&size);
                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].size = wxSize(xSize, ySize);
            break;

            case TAG_COMMENT:
            break;

            case TAG_FILENAME:
                if (child->GetAttribute("dirname") == "images")
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].useImageDir = true;
                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].dirName = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].dirName != pathSeparator_)
                    computerConfiguration.frontPanelConfiguration[frontNumber].guiItemConfiguration[guiItemConfigNumber_].dirName += pathSeparator_;
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

void XmlParser::parseXml_Cdp1851(wxXmlNode &node, bool windowOn)
{
    Cdp1851Configuration cdp1851;
    
    cdp1851.windowOpen = windowOn;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "pos",
        "init",
        "iogroup",
        "int",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_POS,
        TAG_INIT,
        TAG_IOGROUP,
        TAG_INTERRUPT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString position, iogroup;
    size_t ioGroupNumber = 0;

    cdp1851.ioGroupVector.clear();
    cdp1851.writePortA = init_IoPort();
    cdp1851.writePortB = init_IoPort();
    cdp1851.readPortA = init_IoPort();
    cdp1851.readPortB = init_IoPort();
    cdp1851.efaRdy = init_EfFlag();
    cdp1851.efbRdy = init_EfFlag();
    cdp1851.efIrq = init_EfFlag();
    cdp1851.initPortA = 0;
    cdp1851.initPortB = 0;

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
                    cdp1851.writePortA = parseXml_IoPort(*child, CDP1851_WRITE_A_OUT);
                if (child->GetAttribute("type") == "b")
                    cdp1851.writePortB = parseXml_IoPort(*child, CDP1851_WRITE_B_OUT);
                if (child->GetAttribute("type") == "control")
                    cdp1851.writeControl = parseXml_IoPort(*child, CDP1851_WRITE_CONTROL_OUT);
            break;
                
            case TAG_IN:
                if (child->GetAttribute("type") == "a")
                    cdp1851.readPortA = parseXml_IoPort(*child, CDP1851_READ_A_OUT);
                if (child->GetAttribute("type") == "b")
                    cdp1851.readPortB = parseXml_IoPort(*child, CDP1851_READ_B_OUT);
                if (child->GetAttribute("type") == "status")
                    cdp1851.readStatus = parseXml_IoPort(*child, CDP1851_READ_STATUS_IN);
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "a")
                    cdp1851.efaRdy = parseXml_EfFlag(*child, CDP1851_A_EF);
                if (child->GetAttribute("type") == "b")
                    cdp1851.efbRdy = parseXml_EfFlag(*child, CDP1851_B_EF);
                if (child->GetAttribute("type") == "irq")
                    cdp1851.efIrq = parseXml_EfFlag(*child, CDP1851_IRQ);
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                cdp1851.defaultPos.x = (int)getNextHexDec(&position);
                cdp1851.defaultPos.y = (int)getNextHexDec(&position);
                if (cdp1851.defaultPos.x != -1)
                    cdp1851.defaultPos.x += defaultFrontPanelX_;
                if (cdp1851.defaultPos.y != -1)
                    cdp1851.defaultPos.y += defaultFrontPanelY_;
            break;

            case TAG_INIT:
                if (child->GetAttribute("type") == "a")
                    cdp1851.initPortA = (Byte)parseXml_Number(*child);
                if (child->GetAttribute("type") == "b")
                    cdp1851.initPortB = (Byte)parseXml_Number(*child);
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    cdp1851.ioGroupVector.resize(ioGroupNumber+1);
                    cdp1851.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_INTERRUPT:
                cdp1851.picInterrupt = (int)parseXml_Number(*child);
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
    computerConfiguration.cdp1851Configuration.push_back(cdp1851);
}

void XmlParser::parseXml_Cdp1852(wxXmlNode &node, bool windowOn)
{
    Cdp1852Configuration cdp1852;
    
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
    wxString position, iogroup;
    size_t ioGroupNumber = 0;

    cdp1852.ioGroupVector.clear();
    cdp1852.writePort = init_IoPort();
    cdp1852.readPort = init_IoPort();
    cdp1852.efStb = init_EfFlag();

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
                cdp1852.writePort = parseXml_IoPort(*child, CDP1852_WRITE_OUT);
            break;
                
            case TAG_IN:
                cdp1852.readPort = parseXml_IoPort(*child, CDP1852_READ_IN);
            break;
                
            case TAG_EF:
                cdp1852.efStb = parseXml_EfFlag(*child, CDP1852_EF);
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                cdp1852.defaultPos.x = (int)getNextHexDec(&position);
                cdp1852.defaultPos.y = (int)getNextHexDec(&position);
                if (cdp1852.defaultPos.x != -1)
                    cdp1852.defaultPos.x += defaultFrontPanelX_;
                if (cdp1852.defaultPos.y != -1)
                    cdp1852.defaultPos.y += defaultFrontPanelY_;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    cdp1852.ioGroupVector.resize(ioGroupNumber+1);
                    cdp1852.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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
    computerConfiguration.cdp1852Configuration.push_back(cdp1852);
}

void XmlParser::parseXml_Cdp1854(wxXmlNode &node, int connection)
{
    Cdp1854Configuration cdp1854;

    wxString tagList[]=
    {
        "in",
        "out",
        "ef",
        "int",
        "serialport",
        "baud",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_OUT,
        TAG_EF,
        TAG_INTERRUPT,
        TAG_SERIALPORT,
        TAG_BAUD,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    int baud, number;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    cdp1854.ioGroupVector.clear();
    cdp1854.in = init_IoPort();
    cdp1854.status = init_IoPort();
    cdp1854.out = init_IoPort();
    cdp1854.control = init_IoPort();
    cdp1854.ef = init_EfFlag();
    cdp1854.efInterrupt = init_EfFlag();
    cdp1854.interrupt = false;

    cdp1854.baudR = 4;
    cdp1854.baudT = 4;
    cdp1854.baudCorrectionR = 0.5;
    cdp1854.baudCorrectionT = 0.5;
    cdp1854.connection = connection;

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
                if (child->GetAttribute("type") == "register")
                    cdp1854.in = parseXml_IoPort(*child, UART1854_READ_RECEIVER_IN);
                if (child->GetAttribute("type") == "status")
                    cdp1854.status = parseXml_IoPort(*child, UART1854_READ_STATUS_IN);
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "register")
                    cdp1854.out = parseXml_IoPort(*child, UART1854_LOAD_TRANSMITTER_OUT);
                if (child->GetAttribute("type") == "control")
                    cdp1854.control = parseXml_IoPort(*child, UART1854_LOAD_CONTROL_OUT);
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "int")
                    cdp1854.efInterrupt = parseXml_EfFlag(*child, UART1854_EF_INTERRUPT);
                else
                    cdp1854.ef = parseXml_EfFlag(*child, UART1854_EF);
            break;
                
            case TAG_INTERRUPT:
                cdp1854.interrupt = true;
                cdp1854.picInterrupt = (int)parseXml_Number(*child);
            break;

/*            case TAG_SERIALPORT:
                cdp1854.serialPort = child->GetNodeContent();
            break;*/

            case TAG_BAUD:
                baud = (int)parseXml_Number(*child);
                number = 0;
                while (baud < parserBaudRateValue_[number] && parserBaudRateValue_[number] != 50)
                    number++;
                if (child->GetAttribute("type") == "receive")
                    cdp1854.baudR = number;
                if (child->GetAttribute("type") == "transmit")
                    cdp1854.baudT = number;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    cdp1854.ioGroupVector.resize(ioGroupNumber+1);
                    cdp1854.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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
    
    computerConfiguration.cdp1854Configuration.push_back(cdp1854);
}


void XmlParser::parseXml_Cdp1855(wxXmlNode &node)
{
    computerConfiguration.cdp1855Configuration.defined = true;
    
    wxString tagList[]=
    {
        "out",
        "in",
        "io",
        "ef",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_IO,
        TAG_EF,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.cdp1855Configuration.ioGroupVector.clear();
    computerConfiguration.cdp1855Configuration.x = init_IoPort();
    computerConfiguration.cdp1855Configuration.y = init_IoPort();
    computerConfiguration.cdp1855Configuration.z = init_IoPort();
    computerConfiguration.cdp1855Configuration.control = init_IoPort();
    computerConfiguration.cdp1855Configuration.status = init_IoPort();
    computerConfiguration.cdp1855Configuration.ef = init_EfFlag();

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
                if (child->GetAttribute("type") == "control")
                    computerConfiguration.cdp1855Configuration.control = parseXml_IoPort(*child, MDU_CONTROL);
             break;
                
            case TAG_IN:
                if (child->GetAttribute("type") == "status")
                    computerConfiguration.cdp1855Configuration.status = parseXml_IoPort(*child, MDU_STATUS);
            break;

            case TAG_IO:
                if (child->GetAttribute("type") == "x")
                    computerConfiguration.cdp1855Configuration.x = parseXml_IoPort(*child, MDU_X);
                if (child->GetAttribute("type") == "y")
                    computerConfiguration.cdp1855Configuration.y = parseXml_IoPort(*child, MDU_Y);
                if (child->GetAttribute("type") == "z")
                    computerConfiguration.cdp1855Configuration.z = parseXml_IoPort(*child, MDU_Z);
            break;

            case TAG_EF:
                computerConfiguration.cdp1855Configuration.ef = parseXml_EfFlag(*child, MDU_EF);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.cdp1855Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.cdp1855Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_Cdp1877(wxXmlNode &node)
{
    Cdp1877Configuration cdp1877;
    
    wxString tagList[]=
    {
        "out",
        "in",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString position, iogroup;
    size_t ioGroupNumber = 0;

    cdp1877.ioGroupVector.clear();
    cdp1877.writeMien = init_IoPort();
    cdp1877.readStatus = init_IoPort();
    cdp1877.readPolling = init_IoPort();
    cdp1877.readVector = init_IoPort();
    cdp1877.readIrq = init_IoPort();
    cdp1877.writeMask = init_IoPort();
    cdp1877.writeControl = init_IoPort();
    cdp1877.writePage = init_IoPort();

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
                if (child->GetAttribute("type") == "mien")
                    cdp1877.writeMien = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "mask")
                    cdp1877.writeMask = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "control")
                    cdp1877.writeControl = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "page")
                    cdp1877.writePage = parseXml_IoPort(*child);
            break;
                
            case TAG_IN:
                if (child->GetAttribute("type") == "irq")
                    cdp1877.readIrq = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "status")
                    cdp1877.readStatus = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "polling")
                    cdp1877.readPolling = parseXml_IoPort(*child);
                if (child->GetAttribute("type") == "vector")
                    cdp1877.readVector = parseXml_IoPort(*child);
            break;
                

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    cdp1877.ioGroupVector.resize(ioGroupNumber+1);
                    cdp1877.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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
    computerConfiguration.cdp1877Configuration.push_back(cdp1877);
}

void XmlParser::parseXml_Cdp1878(wxXmlNode &node)
{
    Cdp1878Configuration cdp1878;

    cdp1878.defined = true;
    
    wxString tagList[]=
    {
        "out",
        "in",
        "io",
        "ef",
        "clock",
        "int",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_IO,
        TAG_EF,
        TAG_CLOCK,
        TAG_INTERRUPT,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString position, iogroup;
    size_t ioGroupNumber = 0;

    cdp1878.ioGroupVector.clear();
    cdp1878.counterHighA = init_IoPort();
    cdp1878.counterLowA = init_IoPort();
    cdp1878.counterHighB = init_IoPort();
    cdp1878.counterLowB = init_IoPort();
    cdp1878.controlA = init_IoPort();
    cdp1878.controlB = init_IoPort();
    cdp1878.interruptA = init_IoPort();
    cdp1878.interruptB = init_IoPort();
    cdp1878.ef = init_EfFlag();

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
                if (child->GetAttribute("counter") == "a")
                {
                    if (child->GetAttribute("type") == "control")
                        cdp1878.controlA = parseXml_IoPort(*child, TIMER_CONTROL_A);
                }
                if (child->GetAttribute("counter") == "b")
                {
                    if (child->GetAttribute("type") == "control")
                        cdp1878.controlB = parseXml_IoPort(*child, TIMER_CONTROL_B);
                }
             break;
                
            case TAG_IN:
                if (child->GetAttribute("counter") == "a")
                {
                    if (child->GetAttribute("type") == "interrupt")
                        cdp1878.interruptA = parseXml_IoPort(*child, TIMER_INTERRUPT);
                }
                if (child->GetAttribute("counter") == "b")
                {
                    if (child->GetAttribute("type") == "interrupt")
                        cdp1878.interruptB = parseXml_IoPort(*child, TIMER_INTERRUPT);
                }
            break;

            case TAG_IO:
                if (child->GetAttribute("counter") == "a")
                {
                    if (child->GetAttribute("type") == "low")
                        cdp1878.counterLowA = parseXml_IoPort(*child, TIMER_COUNTER_LOW_A);
                    if (child->GetAttribute("type") == "high")
                        cdp1878.counterHighA = parseXml_IoPort(*child, TIMER_COUNTER_HIGH_A);
                }
                if (child->GetAttribute("counter") == "b")
                {
                    if (child->GetAttribute("type") == "low")
                        cdp1878.counterLowB = parseXml_IoPort(*child, TIMER_COUNTER_LOW_B);
                    if (child->GetAttribute("type") == "high")
                        cdp1878.counterHighB = parseXml_IoPort(*child, TIMER_COUNTER_HIGH_B);
                }
            break;

            case TAG_EF:
                cdp1878.ef = parseXml_EfFlag(*child, TIMER_EF);
            break;

            case TAG_CLOCK:
                if (child->GetAttribute("counter") == "a")
                    cdp1878.clockA = getDouble(child->GetNodeContent(), childName, 500, "500", false);
                if (child->GetAttribute("counter") == "b")
                    cdp1878.clockB = getDouble(child->GetNodeContent(), childName, 500, "500", false);
            break;
                
            case TAG_INTERRUPT:
                cdp1878.picInterrupt = (int)parseXml_Number(*child);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    cdp1878.ioGroupVector.resize(ioGroupNumber+1);
                    cdp1878.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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
    computerConfiguration.cdp1878Configuration.push_back(cdp1878);
}

void XmlParser::parseXml_Cd4536b(wxXmlNode &node)
{
    Cd4536bConfiguration cd4536bIo;
    
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
    wxString iogroup;
    size_t ioGroupNumber = 0;

    cd4536bIo.ioGroupVector.clear();
    cd4536bIo.writeControl = init_IoPort();
    cd4536bIo.ef = init_EfFlag();

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
                cd4536bIo.writeControl = parseXml_IoPort(*child, CD4536B_WRITE_OUT);
            break;
                
            case TAG_EF:
                cd4536bIo.ef = parseXml_EfFlag(*child, CD4536B_EF);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    cd4536bIo.ioGroupVector.resize(ioGroupNumber+1);
                    cd4536bIo.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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
    computerConfiguration.cd4536bConfiguration.push_back(cd4536bIo);
}

void XmlParser::parseXml_SerialVt(wxXmlNode &node)
{
    wxString tagList[]=
    {
        "font",
        "wav",
        "dirname",
        "out",
        "ef",
        "q",
        "serialport",
        "baud",
        "characters",
        "power",
        "bits",
        "parity",
        "parity_sense",
        "interlace",
        "newline",
        "wrap",
        "shift3",
        "xon_xoff",
        "ansi_vt52",
        "key_click",
        "bell",
        "frequency",
        "cursor",
        "screen",
        "repeat",
        "scroll",
        "echo",
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
        TAG_OUT,
        TAG_EF,
        TAG_Q,
        TAG_SERIALPORT,
        TAG_BAUD,
        TAG_CHARACTERS,
        TAG_POWER,
        TAG_BITS,
        TAG_PARITY,
        TAG_PARITY_SENSE,
        TAG_INTERLACE,
        TAG_NEW_LINE,
        TAG_WRAP_AROUND,
        TAG_SHIFT_3,
        TAG_XON_XOFF,
        TAG_ANSI_VT52,
        TAG_KEY_CLICK,
        TAG_BELL,
        TAG_FREQUENCY,
        TAG_CURSOR,
        TAG_SCREEN,
        TAG_REPEAT,
        TAG_SCROLL,
        TAG_ECHO,
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
    int baud, number, red, green, blue, xpos, ypos;
    long width, height;
    wxString color, position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.videoTerminalConfiguration.ioGroupVector.clear();
    computerConfiguration.videoTerminalConfiguration.output = init_IoPort();
    computerConfiguration.videoTerminalConfiguration.defaultCharactersPerRow = 80;
    computerConfiguration.videoTerminalConfiguration.defaultCharacterWidth = 10;

    bitset<32> SetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.type == VT52)
        SetUpFeature = computerConfiguration.videoTerminalConfiguration.vt52DefaultSetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.type == VT100)
        SetUpFeature = computerConfiguration.videoTerminalConfiguration.vt100DefaultSetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.external)
        SetUpFeature = computerConfiguration.videoTerminalConfiguration.vtExternalDefaultSetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.loop_back)
        SetUpFeature = computerConfiguration.videoTerminalConfiguration.vtLoopBackDefaultSetUpFeature;

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
                computerConfiguration.videoTerminalConfiguration.vtCharRomFileName = child->GetNodeContent();
            break;

            case TAG_WAV:
                computerConfiguration.videoTerminalConfiguration.wavFileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                if (child->GetAttribute("type") == "font")
                {
                    computerConfiguration.videoTerminalConfiguration.vtCharRomDirectory = dataDir_ + child->GetNodeContent();
                    if (computerConfiguration.videoTerminalConfiguration.vtCharRomDirectory.Right(1) != pathSeparator_)
                        computerConfiguration.videoTerminalConfiguration.vtCharRomDirectory += pathSeparator_;
                }
                if (child->GetAttribute("type") == "wav")
                {
                    computerConfiguration.videoTerminalConfiguration.wavDirectory = dataDir_ + child->GetNodeContent();
                    if (computerConfiguration.videoTerminalConfiguration.wavDirectory.Right(1) != pathSeparator_)
                        computerConfiguration.videoTerminalConfiguration.wavDirectory += pathSeparator_;
                }
            break;

            case TAG_OUT:
                computerConfiguration.videoTerminalConfiguration.output = parseXml_IoPort(*child);
            break;
                
            case TAG_EF:
                computerConfiguration.videoTerminalConfiguration.ef = parseXml_EfFlag(*child);
            break;
                
            case TAG_Q:
                if (child->GetAttribute("pol") == "rev")
                    computerConfiguration.videoTerminalConfiguration.reverseQ = 0;
                else
                    computerConfiguration.videoTerminalConfiguration.reverseQ = 1;
            break;
                
            case TAG_SERIALPORT:
                computerConfiguration.videoTerminalConfiguration.serialPort = child->GetNodeContent();
            break;

            case TAG_BAUD:
                baud = (int)parseXml_Number(*child);
                number = 0;
                while (baud < parserBaudRateValue_[number] && parserBaudRateValue_[number] != 50)
                    number++;
                if (child->GetAttribute("type") == "receive")
                {
                    computerConfiguration.videoTerminalConfiguration.baudR = number;
                    if (child->HasAttribute("correction"))
                        computerConfiguration.videoTerminalConfiguration.baudCorrectionR = getDouble(child->GetAttribute("correction"), childName, -1, "", true);
                }
                if (child->GetAttribute("type") == "transmit")
                {
                    computerConfiguration.videoTerminalConfiguration.baudT = number;
                    if (child->HasAttribute("correction"))
                        computerConfiguration.videoTerminalConfiguration.baudCorrectionT = getDouble(child->GetAttribute("correction"), childName, -1, "", true);
                }
            break;

            case TAG_CHARACTERS:
                if (child->GetNodeContent() == "132")
                {
                    computerConfiguration.videoTerminalConfiguration.defaultCharactersPerRow = 132;
                    computerConfiguration.videoTerminalConfiguration.defaultCharacterWidth = 8;
                }
                if (child->GetNodeContent() == "64")
                {
                    computerConfiguration.videoTerminalConfiguration.defaultCharactersPerRow = 64;
                    computerConfiguration.videoTerminalConfiguration.defaultCharacterWidth = 10;
                }
            break;

            case TAG_POWER:
                if (child->GetNodeContent() == "60")
                    SetUpFeature[VTPOWER] = 0;
                if (child->GetNodeContent() == "50")
                    SetUpFeature[VTPOWER] = 1;
            break;
                
            case TAG_BITS:
                if (child->GetNodeContent() == "7")
                    SetUpFeature[VTBITS] = 0;
                if (child->GetNodeContent() == "8")
                    SetUpFeature[VTBITS] = 1;
            break;
                
            case TAG_PARITY:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTPARITY] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTPARITY] = 1;
            break;
                
            case TAG_PARITY_SENSE:
                if (child->GetNodeContent() == "0dd")
                    SetUpFeature[VTPARITYSENSE] = 0;
                if (child->GetNodeContent() == "even")
                    SetUpFeature[VTPARITYSENSE] = 1;
            break;
                
            case TAG_INTERLACE:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTINTERLACE] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTINTERLACE] = 1;
            break;
                
            case TAG_NEW_LINE:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTNEWLINE] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTNEWLINE] = 1;
            break;
                
            case TAG_WRAP_AROUND:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTWRAPAROUND] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTWRAPAROUND] = 1;
            break;

            case TAG_SHIFT_3:
                if (child->GetNodeContent() == "uk")
                    SetUpFeature[VTUSASCII] = 0;
                if (child->GetNodeContent() == "us")
                    SetUpFeature[VTUSASCII] = 1;
            break;
                
            case TAG_XON_XOFF:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTAUTOXON] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTAUTOXON] = 1;
            break;

            case TAG_ANSI_VT52:
                if (child->GetNodeContent() == "vt52")
                    SetUpFeature[VTANSI] = 0;
                if (child->GetNodeContent() == "ansi")
                    SetUpFeature[VTANSI] = 1;
            break;

            case TAG_KEY_CLICK:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTKEYCLICK] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTKEYCLICK] = 1;
            break;

            case TAG_BELL:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTBELL] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTBELL] = 1;
            break;

            case TAG_FREQUENCY:
                computerConfiguration.videoTerminalConfiguration.defaultBellFrequency = (int)parseXml_Number(*child);
            break;

            case TAG_CURSOR:
                if (child->GetNodeContent() == "line")
                    SetUpFeature[VTCURSORBLOCK] = 0;
                if (child->GetNodeContent() == "block")
                    SetUpFeature[VTCURSORBLOCK] = 1;
            break;

            case TAG_SCREEN:
                if (child->GetNodeContent() == "dark")
                    SetUpFeature[VTREVERSESCREEN] = 0;
                if (child->GetNodeContent() == "light")
                    SetUpFeature[VTREVERSESCREEN] = 1;
            break;

            case TAG_REPEAT:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTREPEAT] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTREPEAT] = 1;
            break;

            case TAG_SCROLL:
                if (child->GetNodeContent() == "jump")
                    SetUpFeature[VTSMOOTHSCROLL] = 0;
                if (child->GetNodeContent() == "smooth")
                    SetUpFeature[VTSMOOTHSCROLL] = 1;
            break;

            case TAG_ECHO:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTLOCALECHO] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTLOCALECHO] = 1;
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
                    screenInfo.borderX[VIDEOVT] = (int)width;
                    screenInfo.borderY[VIDEOVT] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.videoTerminalConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.videoTerminalConfiguration.defaultY;
                computerConfiguration.videoTerminalConfiguration.defaultX = xpos;
                computerConfiguration.videoTerminalConfiguration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_VT_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_VT_BACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "high")
                    screenInfo.defaultColour[COL_VT_HIGH].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_CAPS:
                computerConfiguration.forceUpperCase = true;
            break;

            case TAG_SHOW:
                if (child->GetNodeContent() == "no")
                    computerConfiguration.videoTerminalConfiguration.show = false;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.videoTerminalConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.videoTerminalConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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
    
    if (computerConfiguration.videoTerminalConfiguration.type == VT52)
        computerConfiguration.videoTerminalConfiguration.vt52DefaultSetUpFeature = SetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.type == VT100)
        computerConfiguration.videoTerminalConfiguration.vt100DefaultSetUpFeature = SetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.external)
        computerConfiguration.videoTerminalConfiguration.vtExternalDefaultSetUpFeature = SetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.loop_back)
        computerConfiguration.videoTerminalConfiguration.vtLoopBackDefaultSetUpFeature = SetUpFeature;
}

void XmlParser::parseXml_UartVt(wxXmlNode &node, bool uart16450)
{
    wxString tagList[]=
    {
        "font",
        "wav",
        "dirname",
        "in",
        "out",
        "ef",
        "int",
        "serialport",
        "baud",
        "characters",
        "power",
        "bits",
        "parity",
        "parity_sense",
        "interlace",
        "newline",
        "wrap",
        "shift3",
        "xon_xoff",
        "ansi_vt52",
        "key_click",
        "bell",
        "frequency",
        "cursor",
        "screen",
        "repeat",
        "scroll",
        "echo",
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
        TAG_INTERRUPT,
        TAG_SERIALPORT,
        TAG_BAUD,
        TAG_CHARACTERS,
        TAG_POWER,
        TAG_BITS,
        TAG_PARITY,
        TAG_PARITY_SENSE,
        TAG_INTERLACE,
        TAG_NEW_LINE,
        TAG_WRAP_AROUND,
        TAG_SHIFT_3,
        TAG_XON_XOFF,
        TAG_ANSI_VT52,
        TAG_KEY_CLICK,
        TAG_BELL,
        TAG_FREQUENCY,
        TAG_CURSOR,
        TAG_SCREEN,
        TAG_REPEAT,
        TAG_SCROLL,
        TAG_ECHO,
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
    int baud, number, red, green, blue, xpos, ypos;
    long width, height;
    wxString color, position, iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.videoTerminalConfiguration.ioGroupVector.clear();
    computerConfiguration.videoTerminalConfiguration.ef = init_EfFlag();
    computerConfiguration.videoTerminalConfiguration.efInterrupt = init_EfFlag();
    computerConfiguration.videoTerminalConfiguration.defaultCharactersPerRow = 80;
    computerConfiguration.videoTerminalConfiguration.defaultCharacterWidth = 10;
    computerConfiguration.videoTerminalConfiguration.interrupt = false;
    computerConfiguration.videoTerminalConfiguration.uartOut = init_IoPort();
    computerConfiguration.videoTerminalConfiguration.uartControl = init_IoPort();
    computerConfiguration.videoTerminalConfiguration.uartIn = init_IoPort();
    computerConfiguration.videoTerminalConfiguration.uartStatus = init_IoPort();

    bitset<32> SetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.type == VT52)
        SetUpFeature = computerConfiguration.videoTerminalConfiguration.vt52DefaultSetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.type == VT100)
        SetUpFeature = computerConfiguration.videoTerminalConfiguration.vt100DefaultSetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.external)
        SetUpFeature = computerConfiguration.videoTerminalConfiguration.vtExternalDefaultSetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.loop_back)
        SetUpFeature = computerConfiguration.videoTerminalConfiguration.vtLoopBackDefaultSetUpFeature;

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
                computerConfiguration.videoTerminalConfiguration.vtCharRomFileName = child->GetNodeContent();
            break;

            case TAG_WAV:
                computerConfiguration.videoTerminalConfiguration.wavFileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                if (child->GetAttribute("type") == "font")
                {
                    computerConfiguration.videoTerminalConfiguration.vtCharRomDirectory = dataDir_ + child->GetNodeContent();
                    if (computerConfiguration.videoTerminalConfiguration.vtCharRomDirectory.Right(1) != pathSeparator_)
                        computerConfiguration.videoTerminalConfiguration.vtCharRomDirectory += pathSeparator_;
                }
                if (child->GetAttribute("type") == "wav")
                {
                    computerConfiguration.videoTerminalConfiguration.wavDirectory = dataDir_ + child->GetNodeContent();
                    if (computerConfiguration.videoTerminalConfiguration.wavDirectory.Right(1) != pathSeparator_)
                        computerConfiguration.videoTerminalConfiguration.wavDirectory += pathSeparator_;
                }
            break;
                
            case TAG_IN:
                if (uart16450)
                {
                    if (child->GetAttribute("type") == "port" || child->GetAttribute("type") == "read")
                        computerConfiguration.videoTerminalConfiguration.uartIn = parseXml_IoPort(*child);
                    if (child->GetAttribute("type") == "status")
                        computerConfiguration.videoTerminalConfiguration.uartStatus = parseXml_IoPort(*child);
                }
                else
                {
                    if (child->GetAttribute("type") == "register")
                        computerConfiguration.videoTerminalConfiguration.uartIn = parseXml_IoPort(*child);
                    if (child->GetAttribute("type") == "status")
                        computerConfiguration.videoTerminalConfiguration.uartStatus = parseXml_IoPort(*child);
                }
            break;

            case TAG_OUT:
                if (uart16450)
                {
                    if (child->GetAttribute("type") == "port" || child->GetAttribute("type") == "write")
                        computerConfiguration.videoTerminalConfiguration.uartOut = parseXml_IoPort(*child);
                    if (child->GetAttribute("type") == "select")
                        computerConfiguration.videoTerminalConfiguration.uartControl = parseXml_IoPort(*child);
                }
                else
                {
                    if (child->GetAttribute("type") == "register")
                        computerConfiguration.videoTerminalConfiguration.uartOut = parseXml_IoPort(*child);
                    if (child->GetAttribute("type") == "control")
                        computerConfiguration.videoTerminalConfiguration.uartControl = parseXml_IoPort(*child);
                }
            break;
                
            case TAG_EF:
                if (child->GetAttribute("type") == "int")
                    computerConfiguration.videoTerminalConfiguration.efInterrupt = parseXml_EfFlag(*child);
                else
                    computerConfiguration.videoTerminalConfiguration.ef = parseXml_EfFlag(*child);
            break;
                
            case TAG_INTERRUPT:
                computerConfiguration.videoTerminalConfiguration.interrupt = true;
                computerConfiguration.videoTerminalConfiguration.picInterrupt = (int)parseXml_Number(*child);
            break;

            case TAG_SERIALPORT:
                computerConfiguration.videoTerminalConfiguration.serialPort = child->GetNodeContent();
            break;

            case TAG_BAUD:
                baud = (int)parseXml_Number(*child);
                number = 0;
                while (baud < parserBaudRateValue_[number] && parserBaudRateValue_[number] != 50)
                    number++;
                if (child->GetAttribute("type") == "receive")
                    computerConfiguration.videoTerminalConfiguration.baudR = number;
                if (child->GetAttribute("type") == "transmit")
                    computerConfiguration.videoTerminalConfiguration.baudT = number;
            break;

            case TAG_CHARACTERS:
                if (child->GetNodeContent() == "132")
                {
                    computerConfiguration.videoTerminalConfiguration.defaultCharactersPerRow = 132;
                    computerConfiguration.videoTerminalConfiguration.defaultCharacterWidth = 8;
                }
                if (child->GetNodeContent() == "64")
                {
                    computerConfiguration.videoTerminalConfiguration.defaultCharactersPerRow = 64;
                    computerConfiguration.videoTerminalConfiguration.defaultCharacterWidth = 10;
                }
            break;
                
            case TAG_POWER:
                if (child->GetNodeContent() == "60")
                    SetUpFeature[VTPOWER] = 0;
                if (child->GetNodeContent() == "50")
                    SetUpFeature[VTPOWER] = 1;
            break;
                
            case TAG_BITS:
                if (child->GetNodeContent() == "7")
                    SetUpFeature[VTBITS] = 0;
                if (child->GetNodeContent() == "8")
                    SetUpFeature[VTBITS] = 1;
            break;
                
            case TAG_PARITY:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTPARITY] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTPARITY] = 1;
            break;
                
            case TAG_PARITY_SENSE:
                if (child->GetNodeContent() == "0dd")
                    SetUpFeature[VTPARITYSENSE] = 0;
                if (child->GetNodeContent() == "even")
                    SetUpFeature[VTPARITYSENSE] = 1;
            break;
                
            case TAG_INTERLACE:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTINTERLACE] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTINTERLACE] = 1;
            break;
                
            case TAG_NEW_LINE:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTNEWLINE] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTNEWLINE] = 1;
            break;
                
            case TAG_WRAP_AROUND:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTWRAPAROUND] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTWRAPAROUND] = 1;
            break;

            case TAG_SHIFT_3:
                if (child->GetNodeContent() == "uk")
                    SetUpFeature[VTUSASCII] = 0;
                if (child->GetNodeContent() == "us")
                    SetUpFeature[VTUSASCII] = 1;
            break;
                
            case TAG_XON_XOFF:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTAUTOXON] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTAUTOXON] = 1;
            break;

            case TAG_ANSI_VT52:
                if (child->GetNodeContent() == "vt52")
                    SetUpFeature[VTANSI] = 0;
                if (child->GetNodeContent() == "ansi")
                    SetUpFeature[VTANSI] = 1;
            break;

            case TAG_KEY_CLICK:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTKEYCLICK] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTKEYCLICK] = 1;
            break;

            case TAG_BELL:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTBELL] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTBELL] = 1;
            break;

            case TAG_FREQUENCY:
                computerConfiguration.videoTerminalConfiguration.defaultBellFrequency = (int)parseXml_Number(*child);
            break;

            case TAG_CURSOR:
                if (child->GetNodeContent() == "line")
                    SetUpFeature[VTCURSORBLOCK] = 0;
                if (child->GetNodeContent() == "block")
                    SetUpFeature[VTCURSORBLOCK] = 1;
            break;

            case TAG_SCREEN:
                if (child->GetNodeContent() == "dark")
                    SetUpFeature[VTREVERSESCREEN] = 0;
                if (child->GetNodeContent() == "light")
                    SetUpFeature[VTREVERSESCREEN] = 1;
            break;

            case TAG_REPEAT:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTREPEAT] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTREPEAT] = 1;
            break;

            case TAG_SCROLL:
                if (child->GetNodeContent() == "jump")
                    SetUpFeature[VTSMOOTHSCROLL] = 0;
                if (child->GetNodeContent() == "smooth")
                    SetUpFeature[VTSMOOTHSCROLL] = 1;
            break;

            case TAG_ECHO:
                if (child->GetNodeContent() == "off")
                    SetUpFeature[VTLOCALECHO] = 0;
                if (child->GetNodeContent() == "on")
                    SetUpFeature[VTLOCALECHO] = 1;
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
                    screenInfo.borderX[VIDEOVT] = (int)width;
                    screenInfo.borderY[VIDEOVT] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.videoTerminalConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.videoTerminalConfiguration.defaultY;
                computerConfiguration.videoTerminalConfiguration.defaultX = xpos;
                computerConfiguration.videoTerminalConfiguration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "fore")
                    screenInfo.defaultColour[COL_VT_FORE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "back")
                    screenInfo.defaultColour[COL_VT_BACK].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "high")
                    screenInfo.defaultColour[COL_VT_HIGH].Printf("#%02X%02X%02X", red, green, blue);
            break;
                
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.videoTerminalConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.videoTerminalConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_CAPS:
                computerConfiguration.forceUpperCase = true;
            break;

            case TAG_SHOW:
                if (child->GetNodeContent() == "no")
                    computerConfiguration.videoTerminalConfiguration.show = false;
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
    
    if (computerConfiguration.videoTerminalConfiguration.type == VT52)
        computerConfiguration.videoTerminalConfiguration.vt52DefaultSetUpFeature = SetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.type == VT100)
        computerConfiguration.videoTerminalConfiguration.vt100DefaultSetUpFeature = SetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.external)
        computerConfiguration.videoTerminalConfiguration.vtExternalDefaultSetUpFeature = SetUpFeature;
    if (computerConfiguration.videoTerminalConfiguration.loop_back)
        computerConfiguration.videoTerminalConfiguration.vtLoopBackDefaultSetUpFeature = SetUpFeature;
}

void XmlParser::parseXml_Printer(wxXmlNode &node, int printerType)
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
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.printerOn_ = true;
    computerConfiguration.serialPrinterConfiguration.bits = 8;
    computerConfiguration.serialPrinterConfiguration.parity = PRINTER_PARITY_NONE;

    switch (printerType)
    {
        case PRINTER_BASIC:
            computerConfiguration.basicPrinterConfiguration.output = init_IoPort();
            computerConfiguration.basicPrinterConfiguration.ef = init_EfFlag();
            computerConfiguration.basicPrinterConfiguration.ioGroupVector.clear();
        break;

        case PRINTER_PARALLEL:
            computerConfiguration.parallelPrinterConfiguration.output = init_IoPort();
            computerConfiguration.parallelPrinterConfiguration.input = init_IoPort();
            computerConfiguration.parallelPrinterConfiguration.ioGroupVector.clear();
        break;

        case PRINTER_SERIAL:
            computerConfiguration.serialPrinterConfiguration.output = init_IoPort();
            computerConfiguration.serialPrinterConfiguration.input = init_IoPort();
            computerConfiguration.serialPrinterConfiguration.ioGroupVector.clear();
        break;

        case COMXTHERMAL:
            computerConfiguration.thermalPrinterConfiguration.output = init_IoPort();
            computerConfiguration.thermalPrinterConfiguration.input = init_IoPort();
            computerConfiguration.thermalPrinterConfiguration.ef = init_EfFlag();
            computerConfiguration.thermalPrinterConfiguration.ioGroupVector.clear();
        break;

        case PRINTER_SERIAL_Q:
            computerConfiguration.qSerialPrinterConfiguration.ef = init_EfFlag();
            computerConfiguration.qSerialPrinterConfiguration.ioGroupVector.clear();
        break;

        case PRINTER_CENTRONICS:
            computerConfiguration.centronicsPrinterConfiguration.output = init_IoPort();
            computerConfiguration.centronicsPrinterConfiguration.ef = init_EfFlag();
            computerConfiguration.centronicsPrinterConfiguration.ioGroupVector.clear();
        break;
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
            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    ioValue = (int)getNextHexDec(&iogroup) & 0xff;
                    switch (printerType)
                    {
                        case PRINTER_BASIC:
                            computerConfiguration.basicPrinterConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                            computerConfiguration.basicPrinterConfiguration.ioGroupVector[ioGroupNumber++] = ioValue;
                        break;

                        case PRINTER_PARALLEL:
                            computerConfiguration.parallelPrinterConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                            computerConfiguration.parallelPrinterConfiguration.ioGroupVector[ioGroupNumber++] = ioValue;
                        break;

                        case PRINTER_SERIAL:
                            computerConfiguration.serialPrinterConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                            computerConfiguration.serialPrinterConfiguration.ioGroupVector[ioGroupNumber++] = ioValue;
                        break;
     
                        case COMXTHERMAL:
                            computerConfiguration.thermalPrinterConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                            computerConfiguration.thermalPrinterConfiguration.ioGroupVector[ioGroupNumber++] = ioValue;
                        break;

                        case PRINTER_SERIAL_Q:
                            computerConfiguration.qSerialPrinterConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                            computerConfiguration.qSerialPrinterConfiguration.ioGroupVector[ioGroupNumber++] = ioValue;
                        break;

                        case PRINTER_CENTRONICS:
                            computerConfiguration.centronicsPrinterConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                            computerConfiguration.centronicsPrinterConfiguration.ioGroupVector[ioGroupNumber++] = ioValue;
                        break;
                    }
                }
            break;

            case TAG_OUT:
                switch (printerType)
                {
                    case PRINTER_BASIC:
                        computerConfiguration.basicPrinterConfiguration.output = parseXml_IoPort(*child, BASIC_PRINTER_OUT);
                    break;

                    case PRINTER_PARALLEL:
                        computerConfiguration.parallelPrinterConfiguration.output = parseXml_IoPort(*child, PARALLEL_PRINTER_OUT);
                    break;

                    case PRINTER_SERIAL:
                        computerConfiguration.serialPrinterConfiguration.output = parseXml_IoPort(*child, SERIAL_PRINTER_OUT);
                    break;
 
                    case COMXTHERMAL:
                        computerConfiguration.thermalPrinterConfiguration.output = parseXml_IoPort(*child, THERMAL_PRINTER_OUT);
                    break;

                    case PRINTER_CENTRONICS:
                        computerConfiguration.centronicsPrinterConfiguration.output = parseXml_IoPort(*child, CENTRONICS_PRINTER_OUT);
                    break;
                }
            break;
                
            case TAG_IN:
                ioValue = (int)parseXml_Number(*child);
                switch (printerType)
                {
                    case PRINTER_PARALLEL:
                        computerConfiguration.parallelPrinterConfiguration.input = parseXml_IoPort(*child, PARALLEL_PRINTER_IN);
                    break;

                    case PRINTER_SERIAL:
                        computerConfiguration.serialPrinterConfiguration.input = parseXml_IoPort(*child, SERIAL_PRINTER_IN);
                    break;
 
                    case COMXTHERMAL:
                        computerConfiguration.thermalPrinterConfiguration.input = parseXml_IoPort(*child, THERMAL_PRINTER_IN);
                    break;
                }
            break;
                
            case TAG_EF:
                switch (printerType)
                {
                    case PRINTER_BASIC:
                        computerConfiguration.basicPrinterConfiguration.ef = parseXml_EfFlag(*child, BASIC_PRINTER_EF);
                    break;

                    case COMXTHERMAL:
                        computerConfiguration.thermalPrinterConfiguration.ef = parseXml_EfFlag(*child, THERMAL_PRINTER_EF);
                    break;

                    case PRINTER_SERIAL_Q:
                        computerConfiguration.qSerialPrinterConfiguration.ef = parseXml_EfFlag(*child, BASIC_Q_PRINTER_EF);
                    break;

                    case PRINTER_CENTRONICS:
                        computerConfiguration.centronicsPrinterConfiguration.ef = parseXml_EfFlag(*child, CENTRONICS_PRINTER_EF);
                    break;
                }
            break;

            case TAG_FILENAME:
                computerConfiguration.printerFileConfiguration.fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.printerFileConfiguration.directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.printerFileConfiguration.directory.Right(1) != pathSeparator_)
                    computerConfiguration.printerFileConfiguration.directory += pathSeparator_;
            break;

            case TAG_BITS:
                computerConfiguration.serialPrinterConfiguration.bits = (int)parseXml_Number(*child);
                if (computerConfiguration.serialPrinterConfiguration.bits < 5)
                    computerConfiguration.serialPrinterConfiguration.bits = 5;
                if (computerConfiguration.serialPrinterConfiguration.bits > 8)
                    computerConfiguration.serialPrinterConfiguration.bits = 8;
            break;

            case TAG_PARITY:
                computerConfiguration.serialPrinterConfiguration.parity = PRINTER_PARITY_NONE;
                if (child->GetNodeContent() == "odd")
                    computerConfiguration.serialPrinterConfiguration.parity = PRINTER_PARITY_ODD;
                if (child->GetNodeContent() == "even")
                    computerConfiguration.serialPrinterConfiguration.parity = PRINTER_PARITY_EVEN;
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

void XmlParser::parseXml_SwCassette(wxXmlNode &node)
{
    wxString tagList[]=
    {
        "ef",
        "out",
        "stopdelay",
        "endtapedelay",
        "filename",
        "dirname",
        "iogroup",
        "keyclear",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_EF,
        TAG_OUT,
        TAG_STOPDELAY,
        TAG_ENDTAPEDELAY,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_IOGROUP,
        TAG_KEYCLEAR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt, cassetteNumber;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.swTapeConfiguration.ioGroupVector.clear();
    computerConfiguration.swTapeConfiguration.output = init_IoPort();
    computerConfiguration.swTapeConfiguration.outputQ = init_IoPort();
    computerConfiguration.swTapeConfiguration.ef = init_EfFlag();
    computerConfiguration.swTapeConfiguration.efRun = init_EfFlag();

    computerConfiguration.swTapeConfiguration.stopDelay = 0;
    computerConfiguration.swTapeConfiguration.endDelay = 0;
    computerConfiguration.swTapeConfiguration.keyClear = false;
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
                    computerConfiguration.swTapeConfiguration.ef = parseXml_EfFlag(*child, TAPE_EF);
                else
                {
                    if (child->GetAttribute("type") == "in")
                        computerConfiguration.swTapeConfiguration.ef = parseXml_EfFlag(*child, TAPE_EF);
                    if (child->GetAttribute("type") == "run")
                        computerConfiguration.swTapeConfiguration.efRun = parseXml_EfFlag(*child, TAPE_RUNNING_EF);
                }
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "q")
                    computerConfiguration.swTapeConfiguration.outputQ = parseXml_IoPort(*child, TAPE_Q_OUT);
                if (child->GetAttribute("type") == "micro")
                    computerConfiguration.swTapeConfiguration.output = parseXml_IoPort(*child, TAPE_MICRO_OUT);
                else
                    computerConfiguration.swTapeConfiguration.output = parseXml_IoPort(*child, TAPE_ON_OFF_OUT);
            break;

            case TAG_STOPDELAY:
                computerConfiguration.swTapeConfiguration.stopDelay = (int)parseXml_Number(*child);
            break;

            case TAG_ENDTAPEDELAY:
                computerConfiguration.swTapeConfiguration.endDelay = (int)parseXml_Number(*child);
            break;

            case TAG_FILENAME:
                if (child->GetAttribute("cassette") == "0")
                    cassetteNumber = 0;
                if (child->GetAttribute("cassette") == "1")
                    cassetteNumber = 1;
                computerConfiguration.wavConfiguration[cassetteNumber].fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                if (child->GetAttribute("cassette") == "0")
                    cassetteNumber = 0;
                if (child->GetAttribute("cassette") == "1")
                    cassetteNumber = 1;
                computerConfiguration.wavConfiguration[cassetteNumber].directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.wavConfiguration[cassetteNumber].directory.Right(1) != pathSeparator_)
                    computerConfiguration.wavConfiguration[cassetteNumber].directory += pathSeparator_;
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.swTapeConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.swTapeConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_KEYCLEAR:
                computerConfiguration.swTapeConfiguration.keyClear = true;
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

void XmlParser::parseXml_HwCassette(wxXmlNode &node)
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
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_IOGROUP,
        TAG_CHANNEL,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt, cassetteNumber;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.hwTapeConfiguration.ioGroupVector.clear();
    computerConfiguration.hwTapeConfiguration.output = init_IoPort();
    computerConfiguration.hwTapeConfiguration.outputMode = init_IoPort();
    computerConfiguration.hwTapeConfiguration.outputMode = init_IoPort();
    computerConfiguration.hwTapeConfiguration.outputSound = init_IoPort();
    computerConfiguration.hwTapeConfiguration.input = init_IoPort();
    computerConfiguration.hwTapeConfiguration.ef = init_EfFlag();
    computerConfiguration.hwTapeConfiguration.efRun = init_EfFlag();
    computerConfiguration.hwTapeConfiguration.efError = init_EfFlag();

    computerConfiguration.hwTapeConfiguration.stopTone = false;
    computerConfiguration.hwTapeConfiguration.startOnReset = false;
    computerConfiguration.hwTapeConfiguration.startBit = 1;
    computerConfiguration.hwTapeConfiguration.dataBits = 8;
    computerConfiguration.hwTapeConfiguration.stopBit = 0;
    computerConfiguration.hwTapeConfiguration.stopBitIgnore = false;
    computerConfiguration.hwTapeConfiguration.stopDelay = 0;
    computerConfiguration.hwTapeConfiguration.frequency0 = 4000;
    computerConfiguration.hwTapeConfiguration.frequency1 = 2000;
    computerConfiguration.hwTapeConfiguration.frequencyBorder = 3000;
    computerConfiguration.hwTapeConfiguration.threshold8Bit = 10;
    computerConfiguration.hwTapeConfiguration.threshold16Bit = 120;
    computerConfiguration.hwTapeConfiguration.threshold24Bit = 1000;
    computerConfiguration.hwTapeConfiguration.audioChannelLeft = true;
    computerConfiguration.hwTapeConfiguration.dataChannelLeft = true;
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
                {
                    if (computerConfiguration.hwTapeConfiguration.format == TAPE_FORMAT_CV)
                        computerConfiguration.hwTapeConfiguration.ef = parseXml_EfFlag(*child, TAPE_CV_DATA_READY_EF);
                    else
                        computerConfiguration.hwTapeConfiguration.ef = parseXml_EfFlag(*child, KEY_TAPE_FRED_EF);
                }
                else
                {
                    if (child->GetAttribute("type") == "out")
                        computerConfiguration.hwTapeConfiguration.efBufferEmpty = parseXml_EfFlag(*child, TAPE_CV_BUFFER_EMPTY_EF);
                    if (child->GetAttribute("type") == "in")
                    {
                        if (computerConfiguration.hwTapeConfiguration.format == TAPE_FORMAT_CV)
                            computerConfiguration.hwTapeConfiguration.ef = parseXml_EfFlag(*child, TAPE_CV_DATA_READY_EF);
                        else
                            computerConfiguration.hwTapeConfiguration.ef = parseXml_EfFlag(*child, KEY_TAPE_FRED_EF);
                    }
                    if (child->GetAttribute("type") == "run")
                        computerConfiguration.hwTapeConfiguration.efRun = parseXml_EfFlag(*child, TAPE_CONTROL_FRED_EF);
                    if (child->GetAttribute("type") == "error")
                        computerConfiguration.hwTapeConfiguration.efError = parseXml_EfFlag(*child, TAPE_ERROR_FRED_EF);
                }
            break;

            case TAG_IN:
                if (computerConfiguration.hwTapeConfiguration.defined)
                {
                    if (computerConfiguration.hwTapeConfiguration.format == TAPE_FORMAT_CV)
                        computerConfiguration.hwTapeConfiguration.input = parseXml_IoPort(*child, TAPE_CV_READ_DATA_IN);
                    else
                        computerConfiguration.hwTapeConfiguration.input = parseXml_IoPort(*child, KEY_TAPE_FRED_READ_DATA_IN);
                }
            break;

            case TAG_OUT:
                if (child->HasAttribute("type"))
                {
                    if (child->GetAttribute("type") == "mode")
                        computerConfiguration.hwTapeConfiguration.outputMode = parseXml_IoPort(*child, TAPE_MODE_FRED_OUT);
                    if (child->GetAttribute("type") == "sound")
                    {
                        computerConfiguration.hwTapeConfiguration.outputSound = parseXml_IoPort(*child, TAPE_SOUND_FRED_OUT);
                        computerConfiguration.soundConfiguration.type = SOUND_Q_SW;
                    }
                }
                else
                {
                    if (computerConfiguration.hwTapeConfiguration.format == TAPE_FORMAT_CV)
                        computerConfiguration.hwTapeConfiguration.output = parseXml_IoPort(*child, TAPE_CV_WRITE_DATA_OUT);
                }
            break;

            case TAG_FREQ:
                if (child->GetAttribute("type") == "0")
                    computerConfiguration.hwTapeConfiguration.frequency0 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "1")
                    computerConfiguration.hwTapeConfiguration.frequency1 = (int)parseXml_Number(*child);
                computerConfiguration.hwTapeConfiguration.frequencyBorder = (computerConfiguration.hwTapeConfiguration.frequency1 + computerConfiguration.hwTapeConfiguration.frequency0) / 2;
            break;

            case TAG_THRESHOLD:
                if (child->GetAttribute("type") == "8bit")
                    computerConfiguration.hwTapeConfiguration.threshold8Bit = (char)parseXml_Number(*child);
                if (child->GetAttribute("type") == "16bit")
                    computerConfiguration.hwTapeConfiguration.threshold16Bit  = (wxInt16)parseXml_Number(*child);
                if (child->GetAttribute("type") == "24bit")
                    computerConfiguration.hwTapeConfiguration.threshold24Bit  = (wxInt32)parseXml_Number(*child);
            break;

            case TAG_STARTBIT:
                computerConfiguration.hwTapeConfiguration.startBit = (int)parseXml_Number(*child);
            break;

            case TAG_DATABITS:
                computerConfiguration.hwTapeConfiguration.dataBits = (int)parseXml_Number(*child);
            break;

            case TAG_STOPBIT:
                computerConfiguration.hwTapeConfiguration.stopBit = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "ignore")
                    computerConfiguration.hwTapeConfiguration.stopBitIgnore = true;
            break;

            case TAG_STOPDELAY:
                computerConfiguration.hwTapeConfiguration.stopDelay = (int)parseXml_Number(*child);
            break;

            case TAG_STOPTONE:
                computerConfiguration.hwTapeConfiguration.stopTone = true;
            break;
            
            case TAG_BOOTSTART:
                computerConfiguration.hwTapeConfiguration.startOnReset = true;
            break;
            
            case TAG_FILENAME:
                if (child->GetAttribute("cassette") == "1")
                    cassetteNumber = 1;
                computerConfiguration.wavConfiguration[cassetteNumber].fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                if (child->GetAttribute("cassette") == "1")
                    cassetteNumber = 1;
                computerConfiguration.wavConfiguration[cassetteNumber].directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.wavConfiguration[cassetteNumber].directory.Right(1) != pathSeparator_)
                    computerConfiguration.wavConfiguration[cassetteNumber].directory += pathSeparator_;
            break;

            case TAG_CHANNEL:
                if (child->GetAttribute("type") == "audio")
                {
                    if (child->GetNodeContent() == "right")
                        computerConfiguration.hwTapeConfiguration.audioChannelLeft = false;
                }
                if (child->GetAttribute("type") == "data")
                {
                    if (child->GetNodeContent() == "right")
                        computerConfiguration.hwTapeConfiguration.dataChannelLeft = false;
                }
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.hwTapeConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.hwTapeConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_Xmodem(wxXmlNode &node)
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
                computerConfiguration.videoTerminalConfiguration.xmodemFileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.videoTerminalConfiguration.xmodemDirectory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.videoTerminalConfiguration.xmodemDirectory.Right(1) != pathSeparator_)
                    computerConfiguration.videoTerminalConfiguration.xmodemDirectory += pathSeparator_;
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

void XmlParser::parseXml_KeyFile(wxXmlNode &node)
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
                computerConfiguration.keyFileConfiguration.fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.keyFileConfiguration.directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.keyFileConfiguration.directory.Right(1) != pathSeparator_)
                    computerConfiguration.keyFileConfiguration.directory += pathSeparator_;
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

void XmlParser::parseXml_Splash(wxXmlNode &node)
{
    computerConfiguration.useSplashScreen_ = true;

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
    computerConfiguration.splashConfiguration.dialog = "DEFAULT";
    
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_DIALOG:
                computerConfiguration.splashConfiguration.dialog = child->GetNodeContent();
            break;
                
            case TAG_TEXT:
                computerConfiguration.splashConfiguration.text = child->GetNodeContent();
                computerConfiguration.splashConfiguration.text.Replace("\r", "\n");
                computerConfiguration.splashConfiguration.text.Replace("&","&&");
            break;
                
            case TAG_MAC:
#ifdef __WXMAC__
                computerConfiguration.splashConfiguration.text = child->GetNodeContent();
                computerConfiguration.splashConfiguration.text.Replace("\r", "\n");
                computerConfiguration.splashConfiguration.text.Replace("&","&&");
#endif
            break;

            case TAG_WINDOWS:
#ifdef __WXMSW__
                computerConfiguration.splashConfiguration.text = child->GetNodeContent();
                computerConfiguration.splashConfiguration.text.Replace("\r", "\n");
                computerConfiguration.splashConfiguration.text.Replace("&","&&");
#endif
            break;

            case TAG_LINUX:
#ifdef __linux__
                computerConfiguration.splashConfiguration.text = child->GetNodeContent();
                computerConfiguration.splashConfiguration.text.Replace("\r", "\n");
                computerConfiguration.splashConfiguration.text.Replace("&","&&");
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

void XmlParser::parseXml_VideoDump(wxXmlNode &node)
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
                computerConfiguration.screenDumpFileConfiguration.fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.screenDumpFileConfiguration.directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.screenDumpFileConfiguration.directory.Right(1) != pathSeparator_)
                    computerConfiguration.screenDumpFileConfiguration.directory += pathSeparator_;
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

void XmlParser::parseXml_MemAccess(wxXmlNode &node)
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
                computerConfiguration.memAccessConfiguration.directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.memAccessConfiguration.directory.Right(1) != pathSeparator_)
                    computerConfiguration.memAccessConfiguration.directory += pathSeparator_;
            break;

            case TAG_LOCATION:
                if (child->GetNodeContent() == "off")
                    computerConfiguration.memAccessConfiguration.useLocation = false;
                if (child->GetNodeContent() == "on")
                    computerConfiguration.memAccessConfiguration.useLocation = true;
            break;

            case TAG_START:
                computerConfiguration.memAccessConfiguration.saveStart = (int)parseXml_Number(*child);
                computerConfiguration.memAccessConfiguration.saveStartString.Printf("%04X", computerConfiguration.memAccessConfiguration.saveStart);
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

void XmlParser::parseXml_Gui(wxXmlNode &node)
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
                parseXml_MemAccess (*child);
            break;

            case TAG_LED_TIMER:
                computerConfiguration.ledTime_ = child->GetNodeContent();
            break;

            case TAG_AUTO:
                if (child->GetNodeContent() == "off")
                    computerConfiguration.autoCassetteLoad_ = false;
                else
                    computerConfiguration.autoCassetteLoad_ = true;
            break;

            case TAG_TURBO:
                if (child->GetNodeContent() == "off")
                    computerConfiguration.turbo_ = false;
                else
                    computerConfiguration.turbo_ = true;
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

void XmlParser::parseXml_Diag(wxXmlNode &node)
{
    wxString tagList[]=
    {
        "checksum",
        "factory",
        "cassette",
        "in",
        "out",
        "iogroup",
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
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.diagnosticBoardConfiguration.ioGroupVector.clear();
    computerConfiguration.diagnosticBoardConfiguration.defined = true;
    computerConfiguration.diagnosticBoardConfiguration.checksum = 1;
    computerConfiguration.diagnosticBoardConfiguration.factory = 1;
    computerConfiguration.diagnosticBoardConfiguration.cassetteCables = 0;
    computerConfiguration.diagnosticBoardConfiguration.output = init_IoPort();
    computerConfiguration.diagnosticBoardConfiguration.input1 = init_IoPort();
    computerConfiguration.diagnosticBoardConfiguration.input2 = init_IoPort();

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
                    computerConfiguration.diagnosticBoardConfiguration.checksum = 0;
            break;

            case TAG_FACTORY:
                if (child->GetNodeContent() == "no")
                    computerConfiguration.diagnosticBoardConfiguration.factory = 0;
            break;

            case TAG_CASSETTE:
                if (child->GetNodeContent() == "disconnected")
                    computerConfiguration.diagnosticBoardConfiguration.cassetteCables = 1;
            break;

            case TAG_IN:
                if (child->GetAttribute("type") == "1")
                    computerConfiguration.diagnosticBoardConfiguration.input1 = parseXml_IoPort(*child, COMX_DIAGNOSTIC_BOARD_IN1);
                if (child->GetAttribute("type") == "2")
                    computerConfiguration.diagnosticBoardConfiguration.input2 = parseXml_IoPort(*child, COMX_DIAGNOSTIC_BOARD_IN2);
            break;

            case TAG_OUT:
                computerConfiguration.diagnosticBoardConfiguration.output = parseXml_IoPort(*child, COMX_DIAGNOSTIC_BOARD_OUT);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.diagnosticBoardConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.diagnosticBoardConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_Debugger(wxXmlNode &node)
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
                computerConfiguration.debuggerConfiguration.mode = (child->GetAttribute("mode") == "on");
                    parseXml_Scrt (*child);
            break;

            case TAG_ASSEMBLER:
                parseXml_Assembler (*child);
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

void XmlParser::parseXml_Scrt(wxXmlNode &node)
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
                    computerConfiguration.debuggerConfiguration.callRegister = (Byte)parseXml_Number(*child, "reg");
                computerConfiguration.debuggerConfiguration.callAddress = (int)parseXml_Number(*child);
            break;

            case TAG_RETURN:
                if (child->HasAttribute("reg"))
                    computerConfiguration.debuggerConfiguration.returnRegister = (Byte)parseXml_Number(*child, "reg");
                computerConfiguration.debuggerConfiguration.returnAddress = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_Assembler(wxXmlNode &node)
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
    
    AssemblerConfiguration assemblerInfo;
    
    assemblerInfo.directoryName = computerConfiguration.mainDir_;
    assemblerInfo.directoryName_defined = false;
    assemblerInfo.fileName = "";
    assemblerInfo.fileName_defined = false;
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
                assemblerInfo.fileName_defined = true;
                assemblerInfo.fileName = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                assemblerInfo.directoryName_defined = true;
                assemblerInfo.directoryName = dataDir_ + child->GetNodeContent();
                if (assemblerInfo.directoryName.Right(1) != pathSeparator_)
                    assemblerInfo.directoryName += pathSeparator_;
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
    computerConfiguration.assemblerConfiguration.push_back(assemblerInfo);
}

void XmlParser::parseXml_BatchWav(wxXmlNode &node)
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
    computerConfiguration.useBatchWav_ = true;

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
                computerConfiguration.saveCommand_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.batchConfiguration.directory = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.batchConfiguration.directory.Right(1) != pathSeparator_)
                    computerConfiguration.batchConfiguration.directory += pathSeparator_;
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

void XmlParser::parseXml_QSound(wxXmlNode &node)
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

    computerConfiguration.soundConfiguration.beepFrequency = 250;

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
                computerConfiguration.soundConfiguration.type = SOUND_EXT_BEEPER;
                computerConfiguration.soundConfiguration.beepFrequency = (int)parseXml_Number(*child);
            break;

            case TAG_SW:
                computerConfiguration.soundConfiguration.type = SOUND_Q_SW;
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

void XmlParser::parseXml_StudioSound(wxXmlNode &node)
{
    computerConfiguration.soundConfiguration.type = SOUND_STUDIO;

    wxString tagList[]=
    {
        "frequency",
        "decay",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FREQ,
        TAG_DECAY,
        TAG_SW,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    computerConfiguration.soundConfiguration.beepFrequency = 650;
    computerConfiguration.soundConfiguration.targetBeepFrequency = 320;
    computerConfiguration.soundConfiguration.decay = 420000;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_FREQ:
                if (child->GetAttribute("type") == "start")
                    computerConfiguration.soundConfiguration.beepFrequency = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "end")
                    computerConfiguration.soundConfiguration.targetBeepFrequency = (int)parseXml_Number(*child);
            break;

            case TAG_DECAY:
                computerConfiguration.soundConfiguration.decay = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_OutBitSound(wxXmlNode &node)
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
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.bitSoundConfiguration.ioGroupVector.clear();
    computerConfiguration.bitSoundConfiguration.output = init_IoPort();
    
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
                computerConfiguration.bitSoundConfiguration.defined = true;
                
                computerConfiguration.bitSoundConfiguration.output = parseXml_IoPort(*child, BIT_SOUND_OUT, 1);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.bitSoundConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.bitSoundConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_Cdp1863Sound(wxXmlNode &node)
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
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.cdp1863Configuration.ioGroupVector.clear();
    computerConfiguration.cdp1863Configuration.toneLatch = init_IoPort();
    computerConfiguration.cdp1863Configuration.toneSwitch1 = init_IoPort();
    computerConfiguration.cdp1863Configuration.toneSwitch2 = init_IoPort();

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
                computerConfiguration.soundConfiguration.type = SOUND_1863_1864;

                if (child->GetAttribute("type") == "switch")
                {
                    if (computerConfiguration.cdp1863Configuration.toneSwitch1.portNumber[0] != -1)
                    {
                        computerConfiguration.soundConfiguration.type = SOUND_1863_NOQ;
                        computerConfiguration.cdp1863Configuration.toneSwitch2 = parseXml_IoPort(*child, CDP1863_TONE_SWITCH_OUT2);
                    }
                    else
                    {
                        computerConfiguration.soundConfiguration.type = SOUND_1863_NOQ;
                        computerConfiguration.cdp1863Configuration.toneSwitch1 = parseXml_IoPort(*child, CDP1863_TONE_SWITCH_OUT1);
                    }
                }
                else
                    computerConfiguration.cdp1863Configuration.toneLatch = parseXml_IoPort(*child, CDP1863_TONE_LATCH_OUT);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.cdp1863Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.cdp1863Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_AY_3_8912Sound(wxXmlNode &node)
{
    computerConfiguration.ay_3_8912Configuration.defined = true;
    computerConfiguration.soundConfiguration.type = SOUND_AY_3_8912;
    wxString tagList[]=
    {
        "out",
        "io",
        "channel",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IO,
        TAG_CHANNEL,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.ay_3_8912Configuration.ioGroupVector.clear();
    computerConfiguration.ay_3_8912Configuration.registerAddressAy1 = init_IoPort();
    computerConfiguration.ay_3_8912Configuration.registerAddressAy2 = init_IoPort();
    computerConfiguration.ay_3_8912Configuration.dataAddress = init_IoPort();
    computerConfiguration.ay_3_8912Configuration.channel_a_1 = 0;
    computerConfiguration.ay_3_8912Configuration.channel_a_2 = -1;
    computerConfiguration.ay_3_8912Configuration.channel_b_1 = -1;
    computerConfiguration.ay_3_8912Configuration.channel_b_2 = 3;
    computerConfiguration.ay_3_8912Configuration.channel_c_1 = 4;
    computerConfiguration.ay_3_8912Configuration.channel_c_2 = 5;

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
                {
                    if (child->HasAttribute("ay"))
                    {
                        if (child->GetAttribute("ay") == "1")
                            computerConfiguration.ay_3_8912Configuration.registerAddressAy1 = parseXml_IoPort(*child, AY_3_8912_REGISTER_ADDRESS_1);
                        if (child->GetAttribute("ay") == "2")
                            computerConfiguration.ay_3_8912Configuration.registerAddressAy2 = parseXml_IoPort(*child, AY_3_8912_REGISTER_ADDRESS_2);
                    }
                    else
                        computerConfiguration.ay_3_8912Configuration.registerAddressAy1 = parseXml_IoPort(*child, AY_3_8912_REGISTER_ADDRESS_1);
                }
            break;

            case TAG_IO:
                if (child->GetAttribute("type") == "data")
                    computerConfiguration.ay_3_8912Configuration.dataAddress = parseXml_IoPort(*child, AY_3_8912_DATA);
            break;
          
            case TAG_CHANNEL:
                if (child->GetAttribute("type") == "a")
                {
                    getChannels("a", child->GetNodeContent(), &computerConfiguration.ay_3_8912Configuration.channel_a_1, &computerConfiguration.ay_3_8912Configuration.channel_a_2);
                }
                if (child->GetAttribute("type") == "b")
                {
                    getChannels("b", child->GetNodeContent(), &computerConfiguration.ay_3_8912Configuration.channel_b_1, &computerConfiguration.ay_3_8912Configuration.channel_b_2);
                }
                if (child->GetAttribute("type") == "c")
                {
                    getChannels("c", child->GetNodeContent(), &computerConfiguration.ay_3_8912Configuration.channel_c_1, &computerConfiguration.ay_3_8912Configuration.channel_c_2);
                }
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.ay_3_8912Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.ay_3_8912Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::getChannels(wxString channel, wxString side, int *one, int *two)
{
    *one = 0;
    *two = 1;
    if (channel == "b")
    {
        *one = *one + 2;
        *two = *two + 2;
    }
    if (channel == "c")
    {
        *one = *one + 4;
        *two = *two + 4;
    }
    if (side == "left")
        *two = -1;
    if (side == "right")
        *one = -1;
}

void XmlParser::parseXml_Dip(wxXmlNode &node)
{
    computerConfiguration.dipConfiguration.defined = true;

    wxString tagList[]=
    {
        "in",
        "value",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_VALUE,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.dipConfiguration.ioGroupVector.clear();
    computerConfiguration.dipConfiguration.input = init_IoPort();
    computerConfiguration.dipConfiguration.input.portNumber[0] = 2;
    computerConfiguration.dipConfiguration.value = 0xf;
    
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
                computerConfiguration.dipConfiguration.input = parseXml_IoPort(*child, DIP_IN);
            break;

            case TAG_VALUE:
                computerConfiguration.dipConfiguration.value = (int)parseXml_Number(*child);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.dipConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.dipConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_IoGroup(wxXmlNode &node)
{
    computerConfiguration.ioGroupConfiguration.defined = true;

    wxString tagList[]=
    {
        "out",
        "in",
        "io",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_IO,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    
    computerConfiguration.ioGroupConfiguration.output = init_IoPort();
    computerConfiguration.ioGroupConfiguration.input = init_IoPort();
    computerConfiguration.ioGroupConfiguration.output.portNumber[0] = 1;
    computerConfiguration.ioGroupConfiguration.output.mask = 0x1f;
    computerConfiguration.ioGroupConfiguration.input.mask = 0x1f;

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
                computerConfiguration.ioGroupConfiguration.output = parseXml_IoPort(*child);
            break;

            case TAG_IN:
                computerConfiguration.ioGroupConfiguration.input = parseXml_IoPort(*child);
            break;

            case TAG_IO:
                computerConfiguration.ioGroupConfiguration.output = parseXml_IoPort(*child);
                computerConfiguration.ioGroupConfiguration.input = computerConfiguration.ioGroupConfiguration.output;
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

void XmlParser::parseXml_Flipflop(wxXmlNode &node)
{
    computerConfiguration.flipflopConfiguration.defined = true;

    wxString tagList[]=
    {
        "out",
        "ef",
        "function",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_EF,
        TAG_FUNCTION,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    for (int number=0; number<4; number++)
        computerConfiguration.flipflopConfiguration.function[number] = FLIPFLOP_NONE;

    int tagTypeInt, number = 0;
    
    computerConfiguration.flipflopConfiguration.output = init_IoPort();
    computerConfiguration.flipflopConfiguration.ef = init_EfFlag();

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
                computerConfiguration.flipflopConfiguration.output = parseXml_IoPort(*child, FLIPFLOP_OUT);
            break;

            case TAG_EF:
                computerConfiguration.flipflopConfiguration.ef = parseXml_EfFlag(*child, FLIPFLOP_EF);
            break;

            case TAG_FUNCTION:
                if (child->HasAttribute("number"))
                    number = (int)parseXml_Number(*child, "number")&3;
                if (child->GetNodeContent() == "iogroup")
                    computerConfiguration.flipflopConfiguration.function[number] = FLIPFLOP_IOGROUP;
                if (child->GetNodeContent() == "vis_mode")
                    computerConfiguration.flipflopConfiguration.function[number] = FLIPFLOP_VIS_MODE;
                if (child->GetNodeContent() == "rs232_cts")
                    computerConfiguration.flipflopConfiguration.function[number] = FLIPFLOP_RS232_CTS;
                if (child->GetNodeContent() == "vis_pcb")
                    computerConfiguration.flipflopConfiguration.function[number] = FLIPFLOP_VIS_PCB;
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

void XmlParser::parseXml_Panel(wxXmlNode &node)
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
                computerConfiguration.hexDisplayConfiguration.output = parseXml_IoPort(*child, HEX_DISPLAY_OUT);
            break;
                
            case TAG_EF:
                computerConfiguration.basicPrinterConfiguration.ef = parseXml_EfFlag(*child, BASIC_PRINTER_EF);
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
 
void XmlParser::parseXml_Memory(wxXmlNode &node)
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
        "mcr",
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
        TAG_MCR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    size_t configNumber;
    size_t emsConfigNumber = 0;
    size_t copyConfigNumber = 0;
    size_t ramPartConfigNumber = 0;

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
                configNumber = getMemConfig(child->GetAttribute("type"));
                setCartType(configNumber, child->GetAttribute("cart"));
                parseXml_RomRam (*child, (int)(ROM + 256*configNumber), configNumber);
                setMemMask(configNumber, parseXml_Number(*child, "mask"));
            break;

            case TAG_DIAG:
                computerConfiguration.diagnosticBoardConfiguration.active = child->GetAttribute("type") == "on";
                computerConfiguration.memoryConfiguration.resize(memConfigNumber_+1);
                parseXml_RomRam (*child, (int)(DIAGROM + 256*memConfigNumber_), memConfigNumber_);
                computerConfiguration.memoryConfiguration[memConfigNumber_].useMemMask = false;
                memConfigNumber_++;
            break;

            case TAG_RAM:
                configNumber = getMemConfig(child->GetAttribute("type"));
                setCartType(configNumber, child->GetAttribute("cart"));
                parseXml_RomRam (*child, (int)(RAM + 256*configNumber), configNumber);
                setMemMask(configNumber, parseXml_Number(*child, "mask"));
            break;

            case TAG_NVRAM:
                computerConfiguration.nvRamConfiguration.disable = false;
                computerConfiguration.nvRamConfiguration.defined = true;
                if (child->GetAttribute("default") == "off")
                    computerConfiguration.nvRamConfiguration.disable = true;
                
                configNumber = getMemConfig(child->GetAttribute("type"));
                setCartType(configNumber, child->GetAttribute("cart"));
                parseXml_RomRam (*child, (int)(NVRAM + 256*configNumber), configNumber);
                setMemMask(configNumber, parseXml_Number(*child, "mask"));
            break;

            case TAG_EMS:
                computerConfiguration.emsMemoryConfiguration.resize(emsConfigNumber+1);
                computerConfiguration.emsMemoryConfiguration[emsConfigNumber].emsType = RAM;

                if (child->GetAttribute("type") == "ROM")
                    computerConfiguration.emsMemoryConfiguration[emsConfigNumber].emsType = ROM;
                
                if (child->GetAttribute("type") == "multicart")
                {
                    computerConfiguration.emsMemoryConfiguration[emsConfigNumber].emsType = ROM;
                    computerConfiguration.multicartEmsNumber_ = (int)emsConfigNumber;
                }

                parseXml_Ems (*child, (int)(EMSMEMORY + 256*emsConfigNumber), emsConfigNumber);
				emsConfigNumber++;
            break;

            case TAG_MAPPER:
                computerConfiguration.memoryConfiguration.resize(memConfigNumber_+1);
                computerConfiguration.memoryMapperConfiguration.defined = true;
                
                memMask = parseXml_Number(*child, "mask");
                if (memMask != 0)
                    memMask = 0xffff;
                memMask  |= 0xff;

                computerConfiguration.memoryMapperConfiguration.maskBits = 16;
                computerConfiguration.memoryMapperConfiguration.mask = 0xFFFF;
                while ((memMask & 0x8000) == 0)
                {
                    computerConfiguration.memoryMapperConfiguration.maskBits--;
                    computerConfiguration.memoryMapperConfiguration.mask = computerConfiguration.memoryMapperConfiguration.mask >> 1;
                    memMask = memMask << 1;
                }

                parseXml_portExt (*child, PAGER, memConfigNumber_);
                memConfigNumber_++;
            break;

            case TAG_GIANT:
                computerConfiguration.giantBoardConfiguration.defined = true;
                computerConfiguration.giantBoardConfiguration.base = (int)parseXml_Number(*child);
            break;

            case TAG_COPY:
                computerConfiguration.memoryCopyConfiguration.resize(++copyConfigNumber);
                computerConfiguration.memoryCopyConfiguration[copyConfigNumber-1].start = parseXml_Number(*child, "start");
                computerConfiguration.memoryCopyConfiguration[copyConfigNumber-1].end = parseXml_Number(*child, "end");
                computerConfiguration.memoryCopyConfiguration[copyConfigNumber-1].slot = (int)parseXml_Number(*child, "slot");
                computerConfiguration.memoryCopyConfiguration[copyConfigNumber-1].copy = (int)parseXml_Number(*child);
            break;

            case TAG_RAMPART:
                computerConfiguration.memoryRamPartConfiguration.resize(++ramPartConfigNumber);
                computerConfiguration.memoryRamPartConfiguration[ramPartConfigNumber-1].start = parseXml_Number(*child, "start");
                computerConfiguration.memoryRamPartConfiguration[ramPartConfigNumber-1].end = parseXml_Number(*child, "end");
            break;

            case TAG_SLOT:
                computerConfiguration.vis1870Configuration.expansionConfiguration_defined = true;
                parseXml_Slot (*child, (int)parseXml_Number(*child, "max"));
            break;

            case TAG_MCR:
                parseXml_Mcr (*child, (int)parseXml_Number(*child, "max"));
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

void XmlParser::setMemMask(size_t configNumber, long mask)
{
    computerConfiguration.memoryConfiguration[configNumber].memMask = mask;
    if (computerConfiguration.memoryConfiguration[configNumber].memMask != 0)
    {
        computerConfiguration.memoryConfiguration[configNumber].useMemMask = true;
        computerConfiguration.memoryConfiguration[configNumber].memMask |= 0xff;
    }
    else
        computerConfiguration.memoryConfiguration[configNumber].useMemMask = false;
}

size_t XmlParser::getMemConfig(wxString type)
{
    size_t configNumber;

    if (type == "gui")
    {
        configNumber = 0;
        if ((computerConfiguration.memoryConfiguration[configNumber].type & 0xff) != UNDEFINED)
            configNumber = 1;
    }
    else
    {
        configNumber = memConfigNumber_++;
        computerConfiguration.memoryConfiguration.resize(configNumber+1);
    }
    
    return configNumber;
}

void XmlParser::setCartType(size_t configNumber, wxString type)
{
    if (type == "st2")
        computerConfiguration.memoryConfiguration[configNumber].cartType = CART_ST2;

    if (type == "ch8")
        computerConfiguration.memoryConfiguration[configNumber].cartType = CART_CH8;
}

void XmlParser::parseXml_RomRam(wxXmlNode &node, int type, size_t configNumber)
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
        "mcr",
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
        TAG_MCR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString addressMap, pulldownString;
    Word address, mask, endMask;

    computerConfiguration.memoryConfiguration[configNumber].start = 0;
    computerConfiguration.memoryConfiguration[configNumber].end = 0;
    computerConfiguration.memoryConfiguration[configNumber].memMask = 0;
    if (configNumber != 0 && configNumber != 1)
        computerConfiguration.memoryConfiguration[configNumber].dirname = computerConfiguration.mainDir_ ;
    computerConfiguration.memoryConfiguration[configNumber].filename = "";
    computerConfiguration.memoryConfiguration[configNumber].filename2 = "";
    computerConfiguration.memoryConfiguration[configNumber].type = type;
    computerConfiguration.memoryConfiguration[configNumber].loadOffSet.addressStart = 0;
    computerConfiguration.memoryConfiguration[configNumber].loadOffSet.offSet = 0;
    computerConfiguration.memoryConfiguration[configNumber].mcr = false;

    if ((type & 0xff) == NVRAM)
        computerConfiguration.memoryConfiguration[configNumber].dumpFilename = "ramdump.bin";

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
                computerConfiguration.memoryConfiguration[configNumber].start = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                computerConfiguration.memoryConfiguration[configNumber].end = parseXml_Number(*child) & 0xffff;
                if (computerConfiguration.memoryConfiguration[configNumber].mcr)
                {
                    endMask = 0xFFFF;
                    mask = computerConfiguration.memoryConfiguration[configNumber].end;
                    while ((mask & 0x8000) == 0)
                    {
                        endMask = endMask >> 1;
                        mask = mask << 1;
                    }
                    computerConfiguration.memoryConfiguration[configNumber].end = endMask;
                }
            break;

            case TAG_FILENAME:
                if (computerConfiguration.memoryConfiguration[configNumber].filename != "")
                    computerConfiguration.memoryConfiguration[configNumber].filename2 = child->GetNodeContent();
                else
                    computerConfiguration.memoryConfiguration[configNumber].filename = child->GetNodeContent();
                
                computerConfiguration.memoryConfiguration[configNumber].verifyFileExist = (child->GetAttribute("verify") == "true");
            break;

            case TAG_DUMP:
                computerConfiguration.memoryConfiguration[configNumber].dumpFilename = child->GetNodeContent();
            break;

           case TAG_DIRNAME:
                computerConfiguration.memoryConfiguration[configNumber].dirname =  dataDir_ + child->GetNodeContent();
                if (computerConfiguration.memoryConfiguration[configNumber].dirname.Right(1) != pathSeparator_)
                {
                    computerConfiguration.memoryConfiguration[configNumber].dirname += pathSeparator_;
                }
            break;

            case TAG_PULLDOWN:
                pulldownString = dataDir_ + child->GetNodeContent();
                if (pulldownString.Right(1) != pathSeparator_)
                    pulldownString += pathSeparator_;
                computerConfiguration.memoryConfiguration[configNumber].pulldownDir.Add(pulldownString);
                computerConfiguration.memoryConfiguration[configNumber].dirname = pulldownString;
                
                pulldownString = "";
                if (child->HasAttribute("mask"))
                    pulldownString = child->GetAttribute("mask");
                computerConfiguration.memoryConfiguration[configNumber].pulldownMask.Add(pulldownString);

                pulldownString = "";
                if (child->HasAttribute("exclude"))
                    pulldownString = child->GetAttribute("exclude");
                if (child->HasAttribute("exclude1"))
                    pulldownString = child->GetAttribute("exclude1");
                computerConfiguration.memoryConfiguration[configNumber].pulldownExclude.Add(pulldownString);

                pulldownString = "";
                if (child->HasAttribute("exclude2"))
                    pulldownString = child->GetAttribute("exclude2");
                computerConfiguration.memoryConfiguration[configNumber].pulldownExclude2.Add(pulldownString);
            break;

            case TAG_MP:
                if (child->GetNodeContent() == "switch")
                    computerConfiguration.nvRamMpConfiguration.followMpSwitch = true;
                else
                    if (node.GetName() == "nvram")
                        parseXml_nvRamMp(*child);
            break;

            case TAG_MAP:
                addressMap = child->GetNodeContent();
                while (addressMap != "")
                {
                    address = getNextHexDec(&addressMap);
                    if (address != 0)
                        computerConfiguration.memoryConfiguration[configNumber].mappingList.push_back(address);
                }
            break;
              
            case TAG_MCR:
                computerConfiguration.memoryConfiguration[configNumber].mcr = true;
                if (computerConfiguration.memoryConfiguration[configNumber].end != 0)
                {
                    endMask = 0xFFFF;
                    mask = computerConfiguration.memoryConfiguration[configNumber].end;
                    while ((mask & 0x8000) == 0)
                    {
                        endMask = endMask >> 1;
                        mask = mask << 1;
                    }
                    computerConfiguration.memoryConfiguration[configNumber].end = endMask;
                }
            break;
              
            case TAG_LOAD_OFFSET:
                computerConfiguration.memoryConfiguration[configNumber].loadOffSet.addressStart = parseXml_Number(*child, "address") & 0xffff;
                computerConfiguration.memoryConfiguration[configNumber].loadOffSet.offSet = parseXml_Number(*child) & 0xffff;
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

void XmlParser::parseXml_Ems(wxXmlNode &node, int type, size_t configNumber)
{
    long start, end;
    
    wxString tagList[]=
    {
        "range",
        "start",
        "end",
        "mask",
        "filename",
        "dirname",
        "out",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_RANGE,
        TAG_START,
        TAG_END,
        TAG_MASK,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_OUT,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.emsMemoryConfiguration[configNumber].ioGroupVector.clear();
    computerConfiguration.emsMemoryConfiguration[configNumber].outputStart = 0;
    computerConfiguration.emsMemoryConfiguration[configNumber].outputEnd = 0;
    computerConfiguration.emsMemoryConfiguration[configNumber].filename = "";
    computerConfiguration.emsMemoryConfiguration[configNumber].dirname = computerConfiguration.mainDir_ ;
    computerConfiguration.emsMemoryConfiguration[configNumber].type = type;
    computerConfiguration.emsMemoryConfiguration[configNumber].output = init_IoPort();

    size_t rangeNumber = 0;
    Word mask;
    bool startEndReserved = false;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_RANGE:
                computerConfiguration.emsMemoryConfiguration[configNumber].range.resize(rangeNumber+1);
                
                if (!parseXml_Range(*child, &start, &end))
                    computerConfiguration.emsMemoryConfiguration[configNumber].range[rangeNumber].end = (Word) start & 0xffff;
                else
                    computerConfiguration.emsMemoryConfiguration[configNumber].range[rangeNumber].end = (Word) end & 0xffff;
                computerConfiguration.emsMemoryConfiguration[configNumber].range[rangeNumber].start = (Word) start & 0xffff;
                rangeNumber++;
            break;

            case TAG_START:
                if (!startEndReserved)
                    computerConfiguration.emsMemoryConfiguration[configNumber].range.resize(rangeNumber+1);
                startEndReserved = !startEndReserved;
                
                computerConfiguration.emsMemoryConfiguration[configNumber].range[rangeNumber].start = (int)parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                if (!startEndReserved)
                    computerConfiguration.emsMemoryConfiguration[configNumber].range.resize(rangeNumber+1);
                startEndReserved = !startEndReserved;
                
                computerConfiguration.emsMemoryConfiguration[configNumber].range[rangeNumber].end = (int)parseXml_Number(*child) & 0xffff;
            break;

            case TAG_FILENAME:
                computerConfiguration.emsMemoryConfiguration[configNumber].filename = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.emsMemoryConfiguration[configNumber].dirname  = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.emsMemoryConfiguration[configNumber].dirname.Right(1) != pathSeparator_)
                {
                    computerConfiguration.emsMemoryConfiguration[configNumber].dirname += pathSeparator_;
                }
            break;

            case TAG_OUT:
                if (!parseXml_Range(*child, &start, &end))
                    computerConfiguration.emsMemoryConfiguration[configNumber].output = parseXml_IoPort(*child);
                else
                {
                    if (child->HasAttribute("mask"))
                        computerConfiguration.emsMemoryConfiguration[configNumber].output.mask = parseXml_Number(*child, "mask");
                    computerConfiguration.emsMemoryConfiguration[configNumber].outputStart = (Word) start;
                    computerConfiguration.emsMemoryConfiguration[configNumber].outputEnd = (Word) end;
                }
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.emsMemoryConfiguration[configNumber].ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.emsMemoryConfiguration[configNumber].ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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
    
    computerConfiguration.emsMemoryConfiguration[configNumber].page = 0;
    computerConfiguration.emsMemoryConfiguration[configNumber].maskBits = 16;
    computerConfiguration.emsMemoryConfiguration[configNumber].mask = 0xFFFF;
    
    end = 0;
    start = 0;
    
    if (computerConfiguration.emsMemoryConfiguration[configNumber].range.size() != 0)
        start = computerConfiguration.emsMemoryConfiguration[configNumber].range[0].start;
    for (std::vector<WordRange>::iterator range = computerConfiguration.emsMemoryConfiguration[configNumber].range.begin (); range != computerConfiguration.emsMemoryConfiguration[configNumber].range.end (); ++range)
    {
        if (range->end > end)
            end = range->end;
        if (range->start < start)
            start = range->start;
    }
    computerConfiguration.emsMemoryConfiguration[configNumber].startAddress = start;

    mask = end - start;

    while ((mask & 0x8000) == 0)
    {
        computerConfiguration.emsMemoryConfiguration[configNumber].maskBits--;
        computerConfiguration.emsMemoryConfiguration[configNumber].mask = computerConfiguration.emsMemoryConfiguration[configNumber].mask >> 1;
        mask = mask << 1;
    }
}

void XmlParser::parseXml_Slot(wxXmlNode &node, int maxSlots)
{
    if (maxSlots <= 0)
        maxSlots = 1;

    size_t bitNumber = maxSlots;
    computerConfiguration.slotConfiguration.slotOutputMask = 1;
    while (--bitNumber > 0)
        computerConfiguration.slotConfiguration.slotOutputMask = 1 + (computerConfiguration.slotConfiguration.slotOutputMask << 1);

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
    
    computerConfiguration.slotConfiguration.maxSlotNumber_ = maxSlots;
    computerConfiguration.slotConfiguration.slotInfo.resize(maxSlots);
    computerConfiguration.slotConfiguration.useIoGroup = false;
    computerConfiguration.slotConfiguration.slotOutputNumber = false;
    computerConfiguration.slotConfiguration.bankOutputNumber = true;
    computerConfiguration.slotConfiguration.bankOutputMask = 0;

    for (int slot=0; slot<maxSlots; slot++)
    {
        computerConfiguration.slotConfiguration.slotInfo[slot].type = UNDEFINED;
        computerConfiguration.slotConfiguration.slotInfo[slot].maxBankNumber_ = 1;
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
                computerConfiguration.slotConfiguration.start = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                computerConfiguration.slotConfiguration.end = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_OUT:
                computerConfiguration.slotConfiguration.output = parseXml_Number(*child);
            break;

            case TAG_SLOTSHIFT:
                computerConfiguration.slotConfiguration.slotOutputShift = (int)parseXml_Number(*child);
                computerConfiguration.slotConfiguration.slotOutputNumber = (child->GetAttribute("type") == "number");
                if (computerConfiguration.slotConfiguration.slotOutputNumber)
                {
                    if (maxSlots > 255)
                        maxSlots = 255;
                    
                    mask = maxSlots - 1;
                   
                    computerConfiguration.slotConfiguration.slotOutputMask = 0xFF;
                    while ((mask & 0x80) == 0)
                    {
                        computerConfiguration.slotConfiguration.slotOutputMask = computerConfiguration.slotConfiguration.slotOutputMask >> 1;
                        mask = mask << 1;
                    }
                }
                else
                {
                    if (maxSlots > 8)
                    {
                        maxSlots = 8;
                    
                        computerConfiguration.slotConfiguration.maxSlotNumber_ = maxSlots;
                        computerConfiguration.slotConfiguration.slotInfo.resize(maxSlots);
                    }
                }
            break;

            case TAG_BANKSHIFT:
                computerConfiguration.slotConfiguration.bankOutputShift = (int)parseXml_Number(*child);
                computerConfiguration.slotConfiguration.bankOutputNumber = (child->GetAttribute("type") == "number");
                computerConfiguration.slotConfiguration.bankOutputMask = (int)parseXml_Number(*child, "mask");
            break;

            case TAG_ROM:
                parseXml_SlotRomRam(*child, (int)parseXml_Number(*child, "slot") , ROM);
            break;

            case TAG_RAM:
                parseXml_SlotRomRam(*child, (int)parseXml_Number(*child, "slot"), RAM);
            break;

            case TAG_IOGROUP:
                computerConfiguration.slotConfiguration.useIoGroup = true;
            break;

            case TAG_BANK:
                parseXml_Bank(*child, (int)parseXml_Number(*child, "slot"), (int)parseXml_Number(*child, "max"));
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

void XmlParser::parseXml_Bank(wxXmlNode &node, int slot, int maxBanks)
{
    computerConfiguration.slotConfiguration.banksInUse_ = true;

    if (maxBanks <= 0)
        maxBanks = 1;

    if (computerConfiguration.slotConfiguration.bankOutputNumber)
    {
        if (maxBanks > 255)
            maxBanks = 255;

        Word mask = maxBanks - 1;
        
        computerConfiguration.slotConfiguration.slotInfo[slot].bankOutputMaskInSlot = 0xFF;
        while ((mask & 0x80) == 0)
        {
            computerConfiguration.slotConfiguration.slotInfo[slot].bankOutputMaskInSlot = computerConfiguration.slotConfiguration.slotInfo[slot].bankOutputMaskInSlot >> 1;
            mask = mask << 1;
        }
    }
    else
    {
        if (maxBanks > 8)
            maxBanks = 8;

        size_t bitNumber = maxBanks;
        computerConfiguration.slotConfiguration.slotInfo[slot].bankOutputMaskInSlot = 1;
        while (--bitNumber > 0)
            computerConfiguration.slotConfiguration.slotInfo[slot].bankOutputMaskInSlot = 1 + (computerConfiguration.slotConfiguration.slotInfo[slot].bankOutputMaskInSlot << 1);
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
    
    computerConfiguration.slotConfiguration.slotInfo[slot].maxBankNumber_ = maxBanks;
    computerConfiguration.slotConfiguration.slotInfo[slot].bankInfo.resize(maxBanks);

    for (int bank=0; bank<maxBanks; bank++)
        computerConfiguration.slotConfiguration.slotInfo[slot].bankInfo[bank].type = UNDEFINED;

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
                parseXml_BankRomRam(*child, slot, (int)parseXml_Number(*child, "bank") , ROM);
            break;

            case TAG_RAM:
                parseXml_BankRomRam(*child, slot, (int)parseXml_Number(*child, "bank"), RAM);
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

void XmlParser::parseXml_SlotRomRam(wxXmlNode &node, int slot, int type)
{
    if (slot >= computerConfiguration.slotConfiguration.maxSlotNumber_)
        return;
    
    computerConfiguration.slotConfiguration.slotInfo[slot].type = type;
    
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
    
    computerConfiguration.slotConfiguration.slotInfo[slot].filename = "";
    computerConfiguration.slotConfiguration.slotInfo[slot].dirname = computerConfiguration.mainDir_ ;
    for (int ramPart = 0; ramPart<4; ramPart++)
    {
        computerConfiguration.slotConfiguration.slotInfo[slot].ramStart[ramPart] = 0;
        computerConfiguration.slotConfiguration.slotInfo[slot].ramEnd[ramPart] = 0;
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
                computerConfiguration.slotConfiguration.slotInfo[slot].filename = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.slotConfiguration.slotInfo[slot].dirname  = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.slotConfiguration.slotInfo[slot].dirname.Right(1) != pathSeparator_)
                {
                    computerConfiguration.slotConfiguration.slotInfo[slot].dirname += pathSeparator_;
                }
            break;

            case TAG_START:
                ramPart = 0;
                if (child->HasAttribute("number"))
                    ramPart = (int)parseXml_Number(*child, "number")&3;
                computerConfiguration.slotConfiguration.slotInfo[slot].ramStart[ramPart] = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                ramPart = 0;
                if (child->HasAttribute("number"))
                    ramPart = (int)parseXml_Number(*child, "number")&3;
                computerConfiguration.slotConfiguration.slotInfo[slot].ramEnd[ramPart] = parseXml_Number(*child) & 0xffff;
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

void XmlParser::parseXml_BankRomRam(wxXmlNode &node, int slot, int bank, int type)
{
    if (slot >= computerConfiguration.slotConfiguration.maxSlotNumber_)
        return;

    if (bank >= computerConfiguration.slotConfiguration.slotInfo[slot].maxBankNumber_)
        return;

    computerConfiguration.slotConfiguration.slotInfo[slot].bankInfo[bank].type = type;
    
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
    
    computerConfiguration.slotConfiguration.slotInfo[slot].bankInfo[bank].filename = "";
    computerConfiguration.slotConfiguration.slotInfo[slot].bankInfo[bank].dirname = computerConfiguration.mainDir_ ;

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
                computerConfiguration.slotConfiguration.slotInfo[slot].bankInfo[bank].filename = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                computerConfiguration.slotConfiguration.slotInfo[slot].bankInfo[bank].dirname  = dataDir_ + child->GetNodeContent();
                if (computerConfiguration.slotConfiguration.slotInfo[slot].bankInfo[bank].dirname.Right(1) != pathSeparator_)
                {
                    computerConfiguration.slotConfiguration.slotInfo[slot].bankInfo[bank].dirname += pathSeparator_;
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

void XmlParser::parseXml_Mcr(wxXmlNode &node, int maxMaps)
{
    if (maxMaps <= 0)
        maxMaps = 1;

    size_t bitNumber = maxMaps;
    computerConfiguration.mcrConfiguration.output.mask = 1;
    while (--bitNumber > 0)
        computerConfiguration.mcrConfiguration.output.mask = 1 + (computerConfiguration.mcrConfiguration.output.mask << 1);

    wxString tagList[]=
    {
        "start",
        "end",
        "out",
        "in",
        "map",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_START,
        TAG_END,
        TAG_OUT,
        TAG_IN,
        TAG_MAP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    
    computerConfiguration.mcrConfiguration.maxMapNumber_ = maxMaps;
    computerConfiguration.mcrConfiguration.mapInfo.resize(maxMaps);
    
    for (int i=0; i<maxMaps; i++)
        computerConfiguration.mcrConfiguration.mapInfo[i].defined = false;

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
                computerConfiguration.mcrConfiguration.start = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                computerConfiguration.mcrConfiguration.end = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "mcr")
                    computerConfiguration.mcrConfiguration.output = parseXml_IoPort(*child);
            break;

            case TAG_IN:
                if (child->GetAttribute("type") == "bbat")
                    computerConfiguration.mcrConfiguration.bbat = parseXml_IoPort(*child);
            break;

            case TAG_MAP:
                parseXml_Map(*child, (int)parseXml_Number(*child, "map_num"));
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

void XmlParser::parseXml_Map(wxXmlNode &node, int mapNum)
{
    if (mapNum >= computerConfiguration.mcrConfiguration.maxMapNumber_)
        return;
        
    computerConfiguration.mcrConfiguration.mapInfo[mapNum].defined = true;

    wxString tagList[]=
    {
        "rom",
        "ram",
        "nvram",
        "io",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_ROM,
        TAG_RAM,
        TAG_NVRAM,
        TAG_IO,
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
            case TAG_ROM:
                parseXml_MapRomRam(*child, mapNum, (int)parseXml_Number(*child, "mem_num"), ROM);
            break;

            case TAG_RAM:
                parseXml_MapRomRam(*child, mapNum, (int)parseXml_Number(*child, "mem_num"), RAM);
            break;
                
            case TAG_NVRAM:
                parseXml_MapRomRam(*child, mapNum, (int)parseXml_Number(*child, "mem_num"), NVRAM);
            break;
                
            case TAG_IO:
                parseXml_MapIO(*child, (int)parseXml_Number(*child, "mem_num"));
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

void XmlParser::parseXml_MapRomRam(wxXmlNode &node, int mapNum, int memNum, int type)
{
    wxString tagList[]=
    {
        "start",
        "end",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_START,
        TAG_END,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long segmentNumber = computerConfiguration.mcrConfiguration.mapInfo[mapNum].mapMemInfo.size();

    computerConfiguration.mcrConfiguration.mapInfo[mapNum].mapMemInfo.resize(segmentNumber+1);
    computerConfiguration.mcrConfiguration.mapInfo[mapNum].mapMemInfo[segmentNumber].type = type;
    computerConfiguration.mcrConfiguration.mapInfo[mapNum].mapMemInfo[segmentNumber].memNumber = memNum;

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
                computerConfiguration.mcrConfiguration.mapInfo[mapNum].mapMemInfo[segmentNumber].start = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                computerConfiguration.mcrConfiguration.mapInfo[mapNum].mapMemInfo[segmentNumber].end = parseXml_Number(*child) & 0xffff;
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

void XmlParser::parseXml_MapIO(wxXmlNode &node, int memNum)
{
    wxString tagList[]=
    {
        "start",
        "end",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_START,
        TAG_END,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    computerConfiguration.mcrConfiguration.ioMemNumber = memNum;

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
                computerConfiguration.mcrConfiguration.ioStart = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                computerConfiguration.mcrConfiguration.ioEnd = parseXml_Number(*child) & 0xffff;
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

void XmlParser::parseXml_portExt(wxXmlNode &node, int type, size_t configNumber)
{
    wxString tagList[]=
    {
        "out",
        "in",
        "start",
        "end",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_START,
        TAG_END,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.memoryConfiguration[configNumber].start = 0;
    computerConfiguration.memoryConfiguration[configNumber].end = 0;
    computerConfiguration.memoryConfiguration[configNumber].type = type;
    computerConfiguration.memoryMapperConfiguration.ioGroupVector.clear();
    computerConfiguration.memoryMapperConfiguration.selectOutput = init_IoPort();
    computerConfiguration.memoryMapperConfiguration.writeOutput = init_IoPort();
    computerConfiguration.memoryMapperConfiguration.input = init_IoPort();

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
                computerConfiguration.memoryConfiguration[configNumber].start = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                computerConfiguration.memoryConfiguration[configNumber].end = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_IN:
                computerConfiguration.memoryMapperConfiguration.input = parseXml_IoPort(*child, PORT_EXTENDER_IN);
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "select")
                    computerConfiguration.memoryMapperConfiguration.selectOutput = parseXml_IoPort(*child, PORT_EXTENDER_SELECT_OUT);
                if (child->GetAttribute("type") == "write")
                    computerConfiguration.memoryMapperConfiguration.writeOutput = parseXml_IoPort(*child, PORT_EXTENDER_WRITE_OUT);
            break;

            case TAG_IOGROUP:
                iogroup = child->GetNodeContent();
                while (iogroup != "")
                {
                    computerConfiguration.memoryMapperConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.memoryMapperConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
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

void XmlParser::parseXml_RtcCdp1879(wxXmlNode &node)
{
    wxString tagList[]=
    {
        "iogroup",
        "ef",
        "int",
        "base",
        "control",
        "second",
        "minute",
        "hour",
        "date",
        "month",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IOGROUP,
        TAG_EF,
        TAG_INTERRUPT,
        TAG_BASE,
        TAG_CONTROL,
        TAG_SECOND,
        TAG_MINUTE,
        TAG_HOUR,
        TAG_DATE,
        TAG_MONTH,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.rtcCdp1879Configuration.ioGroupVector.clear();
    computerConfiguration.rtcCdp1879Configuration.defined = true;
    computerConfiguration.rtcCdp1879Configuration.ef = init_EfFlag();
    computerConfiguration.rtcCdp1879Configuration.interrupt = false;

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
                while (iogroup != "")
                {
                    computerConfiguration.rtcCdp1879Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.rtcCdp1879Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_EF:
                computerConfiguration.rtcCdp1879Configuration.ef = parseXml_EfFlag(*child, RTC_EF);
            break;

            case TAG_INTERRUPT:
                computerConfiguration.rtcCdp1879Configuration.interrupt = true;
                computerConfiguration.rtcCdp1879Configuration.picInterrupt = (int)parseXml_Number(*child);
            break;

            case TAG_BASE:
                computerConfiguration.rtcCdp1879Configuration.base = (int)parseXml_Number(*child);
                computerConfiguration.rtcCdp1879Configuration.freeze = computerConfiguration.rtcCdp1879Configuration.base + 1;
                computerConfiguration.rtcCdp1879Configuration.second = computerConfiguration.rtcCdp1879Configuration.base + 2;
                computerConfiguration.rtcCdp1879Configuration.minute = computerConfiguration.rtcCdp1879Configuration.base + 3;
                computerConfiguration.rtcCdp1879Configuration.hour = computerConfiguration.rtcCdp1879Configuration.base + 4;
                computerConfiguration.rtcCdp1879Configuration.date = computerConfiguration.rtcCdp1879Configuration.base + 5;
                computerConfiguration.rtcCdp1879Configuration.month = computerConfiguration.rtcCdp1879Configuration.base + 6;
                computerConfiguration.rtcCdp1879Configuration.control = computerConfiguration.rtcCdp1879Configuration.base + 7;
            break;

            case TAG_CONTROL:
                computerConfiguration.rtcCdp1879Configuration.control = (int)parseXml_Number(*child);
            break;

            case TAG_SECOND:
                computerConfiguration.rtcCdp1879Configuration.second = (int)parseXml_Number(*child);
            break;

            case TAG_MINUTE:
                computerConfiguration.rtcCdp1879Configuration.minute = (int)parseXml_Number(*child);
            break;

            case TAG_HOUR:
                computerConfiguration.rtcCdp1879Configuration.hour = (int)parseXml_Number(*child);
            break;

            case TAG_DATE:
                computerConfiguration.rtcCdp1879Configuration.date = (int)parseXml_Number(*child);
            break;

            case TAG_MONTH:
                computerConfiguration.rtcCdp1879Configuration.month = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_RtcM48T58(wxXmlNode &node)
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
    wxString iogroup;
    size_t ioGroupNumber = 0;

    computerConfiguration.rtcM48t58Configuration.ioGroupVector.clear();
    computerConfiguration.rtcM48t58Configuration.defined = true;

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
                while (iogroup != "")
                {
                    computerConfiguration.rtcM48t58Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.rtcM48t58Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_CONTROL:
                computerConfiguration.rtcM48t58Configuration.control = (int)parseXml_Number(*child);
            break;

            case TAG_SECOND:
                computerConfiguration.rtcM48t58Configuration.second = (int)parseXml_Number(*child);
            break;

            case TAG_MINUTE:
                computerConfiguration.rtcM48t58Configuration.minute = (int)parseXml_Number(*child);
            break;

            case TAG_HOUR:
                computerConfiguration.rtcM48t58Configuration.hour = (int)parseXml_Number(*child);
            break;

            case TAG_DAY:
                computerConfiguration.rtcM48t58Configuration.day = (int)parseXml_Number(*child);
            break;

            case TAG_DATE:
                computerConfiguration.rtcM48t58Configuration.date = (int)parseXml_Number(*child);
            break;

            case TAG_MONTH:
                computerConfiguration.rtcM48t58Configuration.month = (int)parseXml_Number(*child);
            break;

            case TAG_YEAR:
                computerConfiguration.rtcM48t58Configuration.year = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_RtcDS12887(wxXmlNode &node)
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
    size_t ioGroupNumber = 0;
    
    computerConfiguration.rtcDs12887Configuration.ioGroupVector.clear();
    computerConfiguration.rtcDs12887Configuration.defined = true;
    computerConfiguration.rtcDs12887Configuration.outputSelectPort = init_IoPort();
    computerConfiguration.rtcDs12887Configuration.outputWritePort = init_IoPort();
    computerConfiguration.rtcDs12887Configuration.input = init_IoPort();

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
                while (iogroup != "")
                {
                    computerConfiguration.rtcDs12887Configuration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.rtcDs12887Configuration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_IN:
                computerConfiguration.rtcDs12887Configuration.input = parseXml_IoPort(*child, RTC_READ_PORT_IN);
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "select")
                    computerConfiguration.rtcDs12887Configuration.outputSelectPort = parseXml_IoPort(*child, RTC_SELECT_PORT_OUT);
                if (child->GetAttribute("type") == "write")
                    computerConfiguration.rtcDs12887Configuration.outputWritePort = parseXml_IoPort(*child, RTC_WRITE_PORT_OUT);
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


void XmlParser::parseXml_UsbSuperBoard(wxXmlNode &node)
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
    wxString iogroup;
    size_t ioGroupNumber = 0;
    
    computerConfiguration.superBoardConfiguration.ioGroupVector.clear();
    computerConfiguration.superBoardConfiguration.defined = true;
    computerConfiguration.superBoardConfiguration.input5 = init_IoPort();
    computerConfiguration.superBoardConfiguration.input6 = init_IoPort();
    computerConfiguration.superBoardConfiguration.output = init_IoPort();
    computerConfiguration.superBoardConfiguration.ef = init_EfFlag();

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
                while (iogroup != "")
                {
                    computerConfiguration.superBoardConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.superBoardConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_IN:
                if (child->GetAttribute("type") == "ef")
                    computerConfiguration.superBoardConfiguration.input6 = parseXml_IoPort(*child, SUPERBOARD_RESET_EF_IN);
                if (child->GetAttribute("type") == "usb")
                    computerConfiguration.superBoardConfiguration.input5 = parseXml_IoPort(*child, SUPERBOARD_UART_IN);
            break;

            case TAG_OUT:
                computerConfiguration.superBoardConfiguration.output = parseXml_IoPort(*child, SUPERBOARD_UART_OUT);
            break;

            case TAG_EF:
                computerConfiguration.superBoardConfiguration.ef = parseXml_EfFlag(*child, SUPERBOARD_EF);
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

void XmlParser::parseXml_nvRamMp(wxXmlNode &node)
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
    wxString iogroup;
    size_t ioGroupNumber = 0;
    
    computerConfiguration.nvRamMpConfiguration.ioGroupVector.clear();
    computerConfiguration.nvRamMpConfiguration.defined = true;

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
                while (iogroup != "")
                {
                    computerConfiguration.nvRamMpConfiguration.ioGroupVector.resize(ioGroupNumber+1);
                    computerConfiguration.nvRamMpConfiguration.ioGroupVector[ioGroupNumber++] = (int)getNextHexDec(&iogroup) & 0xff;
                }
            break;

            case TAG_IN:
                computerConfiguration.nvRamMpConfiguration.input = parseXml_IoPort(*child, NVRAM_MEMORY_PROTECT_IN);
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

IoPort XmlParser::parseXml_IoPort(wxXmlNode &node, int ioDefinition, Byte defaultMask)
{
    IoPort ioPort;
    
    wxString ports = node.GetNodeContent();
    long portNumber;
    ioPort.portNumber.clear();
    ioPort.addressMode = false;

    while (ports != "")
    {
        portNumber = getNextHexDec(&ports);
        ioPort.portNumber.push_back((int)portNumber);
    }
    if (ioPort.portNumber.size() == 0)
    {
        ioPort.portNumber.resize(1);
        ioPort.portNumber[0] = -1;
    }

    ioPort.qValue = -1;
    if (node.GetAttribute("q") == "1")
        ioPort.qValue = 1;
    if (node.GetAttribute("q") == "0")
        ioPort.qValue = 0;

    ioPort.mask = defaultMask;
    if (node.HasAttribute("mask"))
        ioPort.mask = (Byte)parseXml_Number(node, "mask");

    if (ioPort.portNumber[0] > 7 || node.GetAttribute("type") == "address")
        ioPort.addressMode = true;
    else
    {
        ioPort.ioDefinition = 0;
        if (ioDefinition != 0)
        {
            computerConfiguration.ioMask[ioDefinition] = ioPort.mask;
            ioPort.ioDefinition = ioDefinition;
        }
    }

    ioPort.addressMask = 0xffff;
    if (node.HasAttribute("addressmask"))
        ioPort.addressMask = (Word)parseXml_Number(node, "addressmask");

    return ioPort;
}

IoPort XmlParser::init_IoPort()
{
    IoPort ioPort;

    ioPort.portNumber.clear();
    ioPort.portNumber.resize(1);
    ioPort.portNumber[0] = -1;
    ioPort.qValue = -1;
    ioPort.mask = 0xff;
    ioPort.addressMask = 0xffff;
    ioPort.addressMode = false;
    ioPort.ioDefinition = 0;

    return ioPort;
}

EfFlag XmlParser::parseXml_EfFlag(wxXmlNode &node, int ioDefinition)
{
    EfFlag efFlag;
    
    efFlag.ioDefinition = 0;
    efFlag.excludeIoGroup = false;

    efFlag.flagNumber = (int)parseXml_Number(node);
    efFlag.qValue = -1;
    if (node.GetAttribute("q") == "1")
        efFlag.qValue = 1;
    if (node.GetAttribute("q") == "0")
        efFlag.qValue = 0;
    
    if (ioDefinition != 0)
        efFlag.ioDefinition = ioDefinition;

    if (node.GetAttribute("pol") == "rev")
        efFlag.reverse = 1;
    else
        efFlag.reverse = 0;

    if (node.GetAttribute("iogroup") == "no")
        efFlag.excludeIoGroup = true;

    return efFlag;
}

EfFlag XmlParser::init_EfFlag()
{
    EfFlag efFlag;

    efFlag.flagNumber = -1;
    efFlag.qValue = -1;
    efFlag.reverse = 0;
    efFlag.ioDefinition = 0;
    efFlag.excludeIoGroup = false;

    return efFlag;
}


long XmlParser::parseXml_Number(wxXmlNode &node)
{
    return getHexDec(node.GetNodeContent());
}

void XmlParser::parseXml_Mc6857BitSetup(wxXmlNode &node,  int bitNumber, wxString childName)
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
            computerConfiguration.mc6847Configuration.b0 =  bitNumber;
        break;

        case BIT_1:
        case BIT_B1:
        case BIT_DD9:
            computerConfiguration.mc6847Configuration.b1 =  bitNumber;
        break;

        case BIT_2:
        case BIT_B2:
        case BIT_DD10:
            computerConfiguration.mc6847Configuration.b2 =  bitNumber;
        break;

        case BIT_3:
        case BIT_B3:
        case BIT_DD11:
            computerConfiguration.mc6847Configuration.b3 =  bitNumber;
        break;

        case BIT_4:
        case BIT_B4:
            computerConfiguration.mc6847Configuration.b4 =  bitNumber;
        break;

        case BIT_5:
        case BIT_B5:
            computerConfiguration.mc6847Configuration.b5 =  bitNumber;
        break;

        case BIT_6:
        case BIT_B6:
            computerConfiguration.mc6847Configuration.b6 =  bitNumber;
        break;

        case BIT_7:
        case BIT_B7:
            computerConfiguration.mc6847Configuration.b7 =  bitNumber;
        break;

        case BIT_DD6:
            computerConfiguration.mc6847Configuration.dd6 =  bitNumber;
        break;

        case BIT_DD7:
            computerConfiguration.mc6847Configuration.dd7 =  bitNumber;
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
		comma = nextNumberString.Find("-");
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
        warningText_ += " incorrect number format\n";
        return "1.0";
    }
    if (floatValue == 0)
    {
        warningText_ += tag;
        warningText_ += " should not be 0\n";
        return "1.0";
    }
    if (floatValue >= max && max != -1)
    {
        warningText_ += tag;
        warningText_ += " should be < ";
        warningText_ += maxStr;
        warningText_ += "\n";
        return "1.0";
    }
    return doubleString;
}

double XmlParser::getDouble(wxString doubleString, wxString tag, double max, wxString maxStr, bool allowZero)
{
    double floatValue;

    if (!toDouble(doubleString, (double*)&floatValue))
    {
        warningText_ += tag;
        warningText_ += " incorrect number format\n";
        return 1;
    }
    if (floatValue == 0 && !allowZero)
    {
        warningText_ += tag;
        warningText_ += " should not be 0\n";
        return 1;
    }
    if (floatValue >= max && max != -1)
    {
        warningText_ += tag;
        warningText_ += " should be < ";
        warningText_ += maxStr;
        warningText_ += "\n";
        return 1;
    }
    return floatValue;
}

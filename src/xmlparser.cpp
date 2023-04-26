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
        "videodump",
        "memaccess",
        "basic",
        "locations",
        "diagnostic",
        "batchwav",
        "gpio",
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
        TAG_VIDEODUMP,
        TAG_MEMACCESS,
        TAG_BASIC,
        TAG_LOCATIONS,
        TAG_DIAG,
        TAG_BATCHWAV,
        TAG_GPIO,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    if (!wxFile::Exists(xmlDir + xmlFile))
    {
        conf[computer].memConfigNumber_ = 1;
        conf[computer].memConfig_.resize(1);
        conf[computer].memConfig_[0].filename = "";
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
    
    computerInfo[computer].gui = "Xml";
    
    elfConfiguration[computer].autoBoot = false;
    conf[computer].bootAddress_ = 0;

    elfConfiguration[computer].bootStrap = false;
    elfConfiguration[computer].useDma = false;
    elfConfiguration[computer].useInt = false;
    elfConfiguration[computer].ioConfiguration.statusBarType = STATUSBAR_NONE;
    
    conf[computer].memConfigNumber_ = 1;
    conf[computer].memConfig_.resize(1);
    conf[computer].memConfig_[0].filename = "";
    conf[computer].ramFileFromGui_ = true;
    conf[computer].mainDir_ =  dataDir_;
    conf[computer].emsConfigNumber_ = 0;
    conf[computer].copyConfigNumber_ = 0;
    conf[computer].slotConfig_.maxSlotNumber_ = 0;
    conf[computer].useDiagnosticBoard_ = false;
    conf[computer].useBatchWav_ = false;
    elfConfiguration[computer].useNvRamMp = false;
    elfConfiguration[computer].useNvRam = false;
    elfConfiguration[computer].useRtcM48T58 = false;
    elfConfiguration[computer].useRtcDS12887 = false;
    elfConfiguration[computer].useUsbSb = false;
    elfConfiguration[computer].usePager = false;
    elfConfiguration[computer].usePortExtender = false;
    elfConfiguration[computer].useEms = false;
    elfConfiguration[computer].fdc1793Enabled = false;
    elfConfiguration[computer].fdc1770Enabled = false;
    elfConfiguration[computer].ideEnabled = false;
    elfConfiguration[computer].ioConfiguration.fdcIoGroup = -1;
    elfConfiguration[computer].ioConfiguration.ideIoGroup = -1;

    conf[computer].numberOfVideoTypes_ = 0;
    elfConfiguration[computer].usePixie = false;
    elfConfiguration[computer].ioConfiguration.pixieDoubleScreenIo = false;
    elfConfiguration[computer].use6845 = false;
    elfConfiguration[computer].use6847 = false;
    elfConfiguration[computer].useTMS9918 = false;
    elfConfiguration[computer].use8275 = false;
    elfConfiguration[computer].useSN76430N = false;
    elfConfiguration[computer].usev1870 = false;
    elfConfiguration[computer].ioConfiguration.v1870useVideoModeEf = false;
    conf[computer].interlace_ = false;

    elfConfiguration[computer].useHexKeyboard = false;
    elfConfiguration[computer].useHexKeyboardEf3 = false;
    elfConfiguration[computer].useInButton = false;
    elfConfiguration[computer].useLatchKeyboard = false;
    elfConfiguration[computer].useMatrixKeyboard = false;
    elfConfiguration[computer].useKeyb1871 = false;
    elfConfiguration[computer].useKeyboard = false;
    elfConfiguration[computer].usePS2 = false;
    elfConfiguration[computer].ps2Interrupt = false;
    elfConfiguration[computer].usePs2gpio = false;
    elfConfiguration[computer].forceUpperCase = false;
    elfConfiguration[computer].useBitKeypad = false;
    elfConfiguration[computer].ioConfiguration.gpioIoGroup = -1;

    elfConfiguration[computer].vtType = VTNONE;
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
    
    elfConfiguration[computer].fdcType_ = FDCTYPE_17XX;

    conf[computer].locationTrigger.resize(65536);
    for (int address=0; address<65536; address++)
    {
        conf[computer].locationTrigger[address].index.resize(1);
        conf[computer].locationTrigger[address].index[0] = -1;
    }
    
    conf[computer].addressLocations.locationInfo.clear();
    
    vt52CharRomDirDefined_ = false;
    elfConfiguration[computer].vt52CharRom_ = "vt52.a.bin";
    elfConfiguration[computer].vt52CharRomDir_ = conf[computer].mainDir_ ;
    vt100CharRomDirDefined_ = false;
    elfConfiguration[computer].vt100CharRom_ = "vt100.bin";
    elfConfiguration[computer].vt100CharRomDir_ = conf[computer].mainDir_ ;
    vtWavFileDirDefined_ = false;
    elfConfiguration[computer].vtWavFile_ = "";
    elfConfiguration[computer].vtWavFileDir_ = conf[computer].mainDir_ ;

    conf[computer].printerOn_ = false;
    conf[computer].useBasicPrinter_ = false;
    conf[computer].useParallelPrinter_ = false;
    conf[computer].useSerialPrinter_ = false;
    conf[computer].useThermalPrinter_ = false;
    conf[computer].useQSerialPrinter_ = false;
    elfConfiguration[computer].ioConfiguration.printerEf = 0;
    elfConfiguration[computer].ioConfiguration.serialPrinterInput = -1;
    elfConfiguration[computer].ioConfiguration.parallelPrinterInput = -1;
    elfConfiguration[computer].ioConfiguration.thermalPrinterInput = -1;

    elfConfiguration[computer].useXmodem = false;
    elfConfiguration[computer].useTape = false;
    elfConfiguration[computer].useTape1 = false;
    elfConfiguration[computer].useTapeHw = false;
    
    elfConfiguration[computer].qSound_ = QSOUNDOFF;

    elfConfiguration[computer].efButtons = false;
    elfConfiguration[computer].panelType_ = PANEL_NONE;
    elfConfiguration[computer].useLedModule = false;
    elfConfiguration[computer].useSwitch = false;
    elfConfiguration[computer].useHex = false;
    elfConfiguration[computer].useElfControlWindows = false;

    elfConfiguration[computer].giantBoardMapping = false;
    elfConfiguration[computer].ioConfiguration.bootStrapType = BOOTSTRAPNONE;

    elfConfiguration[computer].ioConfiguration.hexOutput = -1;
    elfConfiguration[computer].ioConfiguration.hexInput = -1;

    elfConfiguration[computer].useDip = false;
    elfConfiguration[computer].useIoGroup = false;

    elfConfiguration[computer].ioConfiguration.bitKeypad[0].defined = false;
    elfConfiguration[computer].ioConfiguration.bitKeypad[1].defined = false;

    elfConfiguration[computer].useBitSound = false;

    oldXmlFileName_ = xmlDir + xmlFile;
    oldXmlDate_ = newDate;

    int tagTypeInt;

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
                if (child->GetAttribute("type") == "")
                {
                    elfConfiguration[computer].useElfControlWindows = false;
                    parseXml_FrontPanel (computer, *child);
                }
                if (child->GetAttribute("type") == "cosmac")
                {
                    elfConfiguration[computer].panelType_ = PANEL_COSMAC;
                    parseXml_FrontPanel (computer, *child);
                }
                if (child->GetAttribute("type") == "netronics")
                {
                    elfConfiguration[computer].panelType_ = PANEL_ELFII;
                    parseXml_FrontPanel (computer, *child);
                }
                if (child->GetAttribute("type") == "quest")
                {
                    elfConfiguration[computer].panelType_ = PANEL_SUPER;
                    parseXml_FrontPanel (computer, *child);
                }
                if (child->GetAttribute("type") == "elf2k")
                {
                    elfConfiguration[computer].panelType_ = PANEL_ELF2K;
                    parseXml_FrontPanel (computer, *child);
                }
                if (child->GetAttribute("type") == "cosmicos")
                {
                    elfConfiguration[computer].panelType_ = PANEL_COSMICOS;
                    parseXml_FrontPanel (computer, *child);
                }
                if (child->GetAttribute("type") == "microtutor")
                {
                    elfConfiguration[computer].panelType_ = PANEL_MICROTUTOR;
                    parseXml_FrontPanel (computer, *child);
                }
                if (child->GetAttribute("type")  == "microtutor2" || child->GetAttribute("type")  == "microtutorII")
                {
                    elfConfiguration[computer].panelType_ = PANEL_MICROTUTOR2;
                    parseXml_FrontPanel (computer, *child);
                }
            break;
                
            case TAG_MEMORY:
                parseXml_Memory (computer, *child);
            break;

            case TAG_DISK:
                elfConfiguration[computer].ioConfiguration.fdcDrives = (int) ((parseXml_Number(*child, "drives")-1)&0x3) + 1;
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
                if (child->GetAttribute("type") == "pixie")
                    parseXml_PixieVideo (computer, *child);
                if (child->GetAttribute("type") == "1864")
                    parseXml_1864Video (computer, *child);
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
                
            case TAG_KEYPAD:
                if (child->GetAttribute("type") == "elf")
                    parseXml_ElfKeypad (computer, *child);
            break;

            case TAG_BITKEYPAD:
                if (child->GetAttribute("pad") == "a" || child->GetAttribute("pad") == "1")
                {
                    elfConfiguration[computer].ioConfiguration.bitKeypad[0].number = child->GetAttribute("pad");
                    elfConfiguration[computer].ioConfiguration.bitKeypad[0].number = elfConfiguration[computer].ioConfiguration.bitKeypad[0].number.Capitalize();
                    parseXml_BitKeypad (computer, 0, *child);
                }
                if (child->GetAttribute("pad") == "b" || child->GetAttribute("pad") == "2")
                {
                    elfConfiguration[computer].ioConfiguration.bitKeypad[1].number = child->GetAttribute("pad");
                    elfConfiguration[computer].ioConfiguration.bitKeypad[1].number = elfConfiguration[computer].ioConfiguration.bitKeypad[1].number.Capitalize();
                    parseXml_BitKeypad (computer, 1, *child);
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
                if (child->GetAttribute("type") == "cv")
                {
                    elfConfiguration[computer].useTapeHw = true;
                    elfConfiguration[computer].tapeFormat_ = TAPE_FORMAT_CV;
                    parseXml_Cassette (computer, *child);
                }
                else
                {
                    elfConfiguration[computer].useTape = true;
                    parseXml_Cassette (computer, *child);
                }
            break;

            case TAG_SOUND:
                if (child->GetAttribute("type") == "q")
                    parseXml_QSound (computer, *child);
                if (child->GetAttribute("type") == "bit")
                    parseXml_OutBitSound (computer, *child);

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
                
            case TAG_VIDEODUMP:
                parseXml_VideoDump (computer, *child);
            break;
                
            case TAG_MEMACCESS:
                parseXml_MemAccess (computer, *child);
            break;
                
            case TAG_DIAG:
                parseXml_Diag (computer, *child);
            break;
                
            case TAG_BATCHWAV:
                parseXml_BatchWav (computer, *child);
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
}

void XmlParser::parseXml_System(int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "name",
        "clock",
        "boot",
        "init",
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
        TAG_CLOCK,
        TAG_BOOT,
        TAG_INIT,
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
                guiName = computerInfo[computer].name;
                guiName.Replace("&","&&");
                if (mode_.gui)
                    XRCCTRL(*this,"ConfigTextXml", wxStaticText)->SetLabel(guiName);
            break;

            case TAG_CLOCK:
                conf[computer].clock_ = getDoubleString(child->GetNodeContent(), childName, 500, "500");
            break;
                
            case TAG_BOOT:
                elfConfiguration[computer].autoBoot = true;
                conf[computer].bootAddress_ = getHexDec(child->GetNodeContent());
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
                    elfConfiguration[computer].ioConfiguration.statusBarType = STATUSBAR_CIDELSA;
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
                conf[computer].defus_ = (int)parseXml_Number(*child);
            break;

            case TAG_EOP:
                conf[computer].eop_ = (int)parseXml_Number(*child);
            break;

            case TAG_STRING:
                conf[computer].string_ = (int)parseXml_Number(*child);
            break;

            case TAG_ARRAY:
                conf[computer].arrayValue_ = (int)parseXml_Number(*child);
            break;

            case TAG_EOD:
                conf[computer].eod_ = (int)parseXml_Number(*child);
            break;

            case TAG_RAM:
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
        "reset_state",
        "basic_state",
        "run_state",
        "stop_thermal",
        "stop_cas",
        "start_cas_save",
        "start_cas_load",
        "start_cas_dload",
        "set_dos_filename",
        "batch_start_save",
        "batch_end_save",
        "start_xmodem_save",
        "start_xmodem_load",
        "start_ymodem_save",
        "correct_caps",
        "elfosboot",
        "start_chip8",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_KEYINPUT,
        TAG_RESET_STATE,
        TAG_BASIC_STATE,
        TAG_RUN_STATE,
        TAG_STOP_THERMAL,
        TAG_STOP_CAS,
        TAG_START_CAS_SAVE,
        TAG_START_CAS_LOAD,
        TAG_START_CAS_DLOAD,
        TAG_SET_DOS_FILENAME,
        TAG_BATCH_START_SAVE,
        TAG_BATCH_END_SAVE,
        TAG_START_XMODEM_SAVE,
        TAG_START_XMODEM_LOAD,
        TAG_START_YMODEM_SAVE,
        TAG_CORRECT_CAPS,
        TAG_ELFOS_BOOT,
        TAG_START_CHIP8,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString addressLocations, addressValues;
    Word address;
    LocationInfo newInfo;
    CheckAddressInfo newAddressInfo;
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

            case TAG_RESET_STATE:
            case TAG_BASIC_STATE:
            case TAG_RUN_STATE:
            case TAG_STOP_THERMAL:
            case TAG_STOP_CAS:
            case TAG_START_CAS_SAVE:
            case TAG_START_CAS_LOAD:
            case TAG_START_CAS_DLOAD:
            case TAG_SET_DOS_FILENAME:
            case TAG_BATCH_START_SAVE:
            case TAG_BATCH_END_SAVE:
            case TAG_START_XMODEM_SAVE:
            case TAG_START_XMODEM_LOAD:
            case TAG_START_YMODEM_SAVE:
            case TAG_CORRECT_CAPS:
            case TAG_ELFOS_BOOT:
            case TAG_START_CHIP8:
                newInfo.type = tagTypeInt-TAG_RESET_STATE;

                newInfo.additionalAddress = getHexDec(child->GetAttribute("info"));

                addressLocations = child->GetAttribute("address");
                addressValues = child->GetAttribute("value");
                while (addressLocations != "")
                {
                    newAddressInfo.checkAddress = getNextHexDec(&addressLocations);
                    newAddressInfo.checkValue = getNextHexDec(&addressValues);

                    if (newAddressInfo.checkAddress != 0)
                        newInfo.checkAddressInfo.push_back(newAddressInfo);
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
        "comment",
        "undefined"
    };

    enum
    {
        TAG_ADDRESS,
        TAG_IN,
        TAG_OUT,
        TAG_IO,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    elfConfiguration[computer].bootStrap = true;
    elfConfiguration[computer].ioConfiguration.bootStrapType = BOOTSTRAPREAD;
    elfConfiguration[computer].ioConfiguration.bootStrapIn = -1;
    elfConfiguration[computer].ioConfiguration.bootStrapOut = -1;
    
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
                elfConfiguration[computer].ioConfiguration.bootStrapOut =  (int)parseXml_Number(*child);
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
                elfConfiguration[computer].ioConfiguration.ideTracks = (int)parseXml_Number(*child);;
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
        "color",
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
        TAG_COLOR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue;
    wxString color;

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.pixieVideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.pixieVideoNumber] = "1861 Pixie";

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
                elfConfiguration[computer].ioConfiguration.pixieDoubleScreenIo = true;
            break;
                
            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.pixieInput = (int)parseXml_Number(*child);
            break;
                
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.pixieOutput = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
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
                    screenInfo[computer].borderX[VIDEOXMLPIXIE] = (int)width/3;
                    screenInfo[computer].borderY[VIDEOXMLPIXIE] = (int)height;
                }
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
        "color",
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
        TAG_COLOR,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue;
    wxString color;

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.cdp1864VideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.cdp1864VideoNumber] = "CDP1864";
    elfConfiguration[computer].ioConfiguration.cdp1864disable.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.cdp1864enable.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.cdp1864IoGroup = -1;

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
                else
                {
                    elfConfiguration[computer].ioConfiguration.cdp1864enable.portNumber = (int)parseXml_Number(*child);
                    elfConfiguration[computer].ioConfiguration.cdp1864enable.qValue = -1;
                    if (child->GetAttribute("q") == "1")
                        elfConfiguration[computer].ioConfiguration.cdp1864enable.qValue = 1;
                    if (child->GetAttribute("q") == "0")
                        elfConfiguration[computer].ioConfiguration.cdp1864enable.qValue = 0;
                }
            break;
                
            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.portNumber = (int)parseXml_Number(*child);
                elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.qValue = -1;
                if (child->GetAttribute("q") == "1")
                    elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.qValue = 1;
                if (child->GetAttribute("q") == "0")
                    elfConfiguration[computer].ioConfiguration.cdp1864toneLatch.qValue = 0;
            break;
                
            case TAG_EF:
                    elfConfiguration[computer].ioConfiguration.cdp1864Ef = (int)parseXml_Number(*child);
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
                    screenInfo[computer].borderX[VIDEOXML1864] = (int)width/3;
                    screenInfo[computer].borderY[VIDEOXML1864] = (int)height;
                }
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
                elfConfiguration[computer].ioConfiguration.cdp1864IoGroup = (int)parseXml_Number(*child);
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
    int red, green, blue;
    wxString color;

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
        TAG_COLOR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    int red, green, blue;
    wxString color;

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
                if (child->GetAttribute("type") == "mode")
                {
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
        TAG_COLOR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    long width, height;
    int red, green, blue;
    wxString color;

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.tmsVideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.tmsVideoNumber] = "TMS 9918";

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
        TAG_COLOR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.i8275VideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.i8275VideoNumber] = "Intel 8275";

    int tagTypeInt;
    long width, height;
    int red, green, blue;
    wxString color;

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
        TAG_COLOR,
        TAG_CTONE,
        TAG_CURSORBLINK,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    int tagTypeInt;
    long width, height;
    int number;
    wxString color;

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
        "color",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_ZOOM,
        TAG_BORDER,
        TAG_COLOR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    conf[computer].zoom_[elfConfiguration[computer].ioConfiguration.SN76430NVideoNumber] = "2.00";
    conf[computer].videoName_[elfConfiguration[computer].ioConfiguration.SN76430NVideoNumber] = "SN76430N";

    int tagTypeInt;
    long width, height;
    int red, green, blue;
    wxString color;

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
    elfConfiguration[computer].useLatchKeyboard = false;
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
    elfConfiguration[computer].useLatchKeyboard = false;
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
    elfConfiguration[computer].useLatchKeyboard = false;
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
        "comment",
        "undefined"
    };

    enum
    {
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
                elfConfiguration[computer].ioConfiguration.keyboardInput = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.keyboardEf = (int)parseXml_Number(*child);
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

void XmlParser::parseXml_1871Keyboard(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].useLatchKeyboard = false;
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
    elfConfiguration[computer].useLatchKeyboard = true;
    elfConfiguration[computer].useMatrixKeyboard = false;
    elfConfiguration[computer].useKeyb1871 = false;
    elfConfiguration[computer].useKeyboard = false;
    elfConfiguration[computer].usePS2 = false;
    elfConfiguration[computer].ps2Interrupt = false;
    elfConfiguration[computer].usePs2gpio = false;

    elfConfiguration[computer].ioConfiguration.keybLatchIoGroup = -1;

    wxString tagList[]=
    {
        "out",
        "ef",
        "keydef",
        "keydefshift",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_EF,
        TAG_KEYDEF,
        TAG_KEYDEFSHIFT,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED,
    };

    int tagTypeInt, keyValue;
    wxString keyText;

    for (int i=0; i<512; i++)
    {
        elfConfiguration[computer].ioConfiguration.keybLatchPc[i] = -1;
        elfConfiguration[computer].ioConfiguration.keybLatchPcShift[i] = -1;
    }
    
    elfConfiguration[computer].ioConfiguration.keybLatchAltRight = -1;
    elfConfiguration[computer].ioConfiguration.keybLatchAltLeft = -1;
    elfConfiguration[computer].ioConfiguration.keybLatchCtrlRight = -1;
    elfConfiguration[computer].ioConfiguration.keybLatchCtrlLeft = -1;
    elfConfiguration[computer].ioConfiguration.keybLatchShift = -1;
    elfConfiguration[computer].ioConfiguration.keybLatchSwitch = -1;
    elfConfiguration[computer].ioConfiguration.useKeyDefShift = false;

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
                elfConfiguration[computer].ioConfiguration.keybLatchOut = (int)parseXml_Number(*child);
                elfConfiguration[computer].ioConfiguration.keybLatchOutMask = (int)parseXml_Number(*child, "mask");
            break;
                
            case TAG_EF:
                elfConfiguration[computer].ioConfiguration.keybLatchEf = (int)parseXml_Number(*child);
                if (child->GetAttribute("pol") == "rev")
                    elfConfiguration[computer].ioConfiguration.keybLatchPressed = 0;
                else
                    elfConfiguration[computer].ioConfiguration.keybLatchPressed = 1;
            break;
                
            case TAG_KEYDEF:
                if (child->GetAttribute("type") == "")
                {
                    keyValue = (int)parseXml_Number(*child)&0x1ff;
                    if (keyValue != 0)
                    {
                        elfConfiguration[computer].ioConfiguration.keybLatchPc[keyValue] = (int)parseXml_Number(*child, "out");
                        elfConfiguration[computer].ioConfiguration.keybLatchPcShift[keyValue] = (int)parseXml_Number(*child, "out");
                    }
                }
                
                if (child->GetAttribute("type") == "text")
                {
                    keyText = child->GetNodeContent();
                    if (keyText == "alt_right")
                        elfConfiguration[computer].ioConfiguration.keybLatchAltRight = (int)parseXml_Number(*child, "out");
                    if (keyText == "alt_left")
                        elfConfiguration[computer].ioConfiguration.keybLatchAltLeft = (int)parseXml_Number(*child, "out");
                    if (keyText == "ctrl_right")
                        elfConfiguration[computer].ioConfiguration.keybLatchCtrlRight = (int)parseXml_Number(*child, "out");
                    if (keyText == "ctrl_left")
                        elfConfiguration[computer].ioConfiguration.keybLatchCtrlLeft = (int)parseXml_Number(*child, "out");
                    if (keyText == "shift")
                        elfConfiguration[computer].ioConfiguration.keybLatchShift = (int)parseXml_Number(*child, "out");
                    if (keyText == "switch")
                        elfConfiguration[computer].ioConfiguration.keybLatchSwitch = (int)parseXml_Number(*child, "out");
                    if (keyText == "caps")
                        elfConfiguration[computer].ioConfiguration.keybLatchCaps = (int)parseXml_Number(*child, "out");
                }
            break;

            case TAG_KEYDEFSHIFT:
                elfConfiguration[computer].ioConfiguration.useKeyDefShift = true;
                keyValue = (int)parseXml_Number(*child)&0x1ff;
                if (keyValue != 0)
                    elfConfiguration[computer].ioConfiguration.keybLatchPcShift[keyValue] = (int)parseXml_Number(*child, "out");
            break;

            case TAG_IOGROUP:
                elfConfiguration[computer].ioConfiguration.keybLatchIoGroup = (int)parseXml_Number(*child);
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
    elfConfiguration[computer].useLatchKeyboard = false;
    elfConfiguration[computer].useMatrixKeyboard = true;
    elfConfiguration[computer].useKeyb1871 = false;
    elfConfiguration[computer].useKeyboard = false;
    elfConfiguration[computer].usePS2 = false;
    elfConfiguration[computer].ps2Interrupt = false;
    elfConfiguration[computer].usePs2gpio = false;

    elfConfiguration[computer].ioConfiguration.keybMatrixIoGroup = -1;
    elfConfiguration[computer].ioConfiguration.keybMatrixPressed = 0xff;
    
    wxString tagList[]=
    {
        "in",
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
                elfConfiguration[computer].ioConfiguration.keybMatrixInMask = (int)parseXml_Number(*child, "mask");
                
                if (child->GetAttribute("pol") == "rev")
                    elfConfiguration[computer].ioConfiguration.keybMatrixPressed = 0;
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
                            elfConfiguration[computer].ioConfiguration.keybMatrixTextKey[textEfKeyInt].bitMaskPressed = (int)parseXml_Number(*child, "bit") + 1;
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
                elfConfiguration[computer].ioConfiguration.hexInput = (int)parseXml_Number(*child);
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
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    wxString bitNumber;
    elfConfiguration[computer].ioConfiguration.bitKeyPressed = 0;
    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys = 0;
    elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].ioGroup = -1;

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
                elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey.resize(elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys+1);
                elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys].value = (int)parseXml_Number(*child);
                elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys].bitMaskPressed = 1 << (tagTypeInt - TAG_BIT0);
                elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys].bitMaskReleased = elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].bitkey[elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys].bitMaskPressed ^ 0xff;
                elfConfiguration[computer].ioConfiguration.bitKeypad[padnumber].numberOfKeys++;
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
                elfConfiguration[computer].ioConfiguration.bitKeyPressed = (int)parseXml_Number(*child);
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
        "keydef",
        "iogroup",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_EF,
        TAG_KEYDEF,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt, keyValue, textEfKeyInt;
    wxString keyText, efText;
    wxString bitNumber;
    elfConfiguration[computer].ioConfiguration.cvKeypad.ioGroup = -1;
    elfConfiguration[computer].ioConfiguration.cvKeypad.reversed = false;

    for (int i=0; i<255; i++)
    {
        elfConfiguration[computer].ioConfiguration.cvKeypad.keydef[i] = -1;
    }
    for (int i=0; i<LAST_MATRIX_TEXT_KEY; i++)
    {
        elfConfiguration[computer].ioConfiguration.CvKeypadTextKey[i] = -1;
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

void XmlParser::parseXml_EfButtons (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].efButtons = true;
    elfConfiguration[computer].ioConfiguration.efKeyPressed = 0;
    elfConfiguration[computer].ioConfiguration.efKey[1].defined = false;
    elfConfiguration[computer].ioConfiguration.efKey[2].defined = false;
    elfConfiguration[computer].ioConfiguration.efKey[3].defined = false;
    elfConfiguration[computer].ioConfiguration.efKey[4].defined = false;

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
                elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].defined = true;
                elfConfiguration[computer].ioConfiguration.efKey[tagTypeInt-TAG_EF1+1].value = (int)parseXml_Number(*child);
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
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    elfConfiguration[computer].tilType = TIL311;
    
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
                elfConfiguration[computer].ioConfiguration.hexOutput = (int)parseXml_Number(*child);
            break;
                
            case TAG_IN:
                if (elfConfiguration[computer].panelType_ != PANEL_COSMAC && elfConfiguration[computer].panelType_ != PANEL_ELF2K)
                    elfConfiguration[computer].useHexKeyboard = true;
                elfConfiguration[computer].ioConfiguration.hexInput = (int)parseXml_Number(*child);
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
            break;

            case TAG_TIL:
                if (child->GetNodeContent() == "311")
                    elfConfiguration[computer].tilType = TIL311;
                else
                    elfConfiguration[computer].tilType = TIL313;
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
        "baud",
        "characters",
        "border",
        "color",
        "caps",
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
        TAG_BORDER,
        TAG_COLOR,
        TAG_CAPS,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    int baud;
    int number, red, green, blue;
    long width, height;
    wxString color;

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
        "serialport",
        "baud",
        "characters",
        "border",
        "color",
        "caps",
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
        TAG_SERIALPORT,
        TAG_BAUD,
        TAG_CHARACTERS,
        TAG_BORDER,
        TAG_COLOR,
        TAG_CAPS,
        TAG_IOGROUP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    int baud;
    int number, red, green, blue;
    long width, height;
    wxString color;
    
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
                
            case TAG_SERIALPORT:
                elfConfiguration[computer].serialPort_ = child->GetNodeContent();
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

    elfConfiguration[computer].ioConfiguration.tapeIoGroup = -1;
    elfConfiguration[computer].ioConfiguration.tapeIn.portNumber = -1;
    elfConfiguration[computer].ioConfiguration.tapeOut.portNumber = -1;

    elfConfiguration[computer].stopTone = false;
    elfConfiguration[computer].tapeStart = false;
    elfConfiguration[computer].tape_revInput = false;
    elfConfiguration[computer].tape_startBit = 1;
    elfConfiguration[computer].tape_dataBits = 8;
    elfConfiguration[computer].tape_stopBit = 0;
    elfConfiguration[computer].tape_stopDelay = 100;
    elfConfiguration[computer].tape_frequency0 = 4000;
    elfConfiguration[computer].tape_frequency1 = 2000;
    elfConfiguration[computer].tape_frequencyBorder = 3000;
    elfConfiguration[computer].tape_threshold8Bit = 10;
    elfConfiguration[computer].tape_threshold16Bit = 500;
    elfConfiguration[computer].tape_audioChannelLeft = true;
    elfConfiguration[computer].tape_dataChannelLeft = true;
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
                if (child->GetAttribute("type") == "out")
                    elfConfiguration[computer].ioConfiguration.tapeEfOut = (int)parseXml_Number(*child);
                else
                    elfConfiguration[computer].ioConfiguration.tapeEf = (int)parseXml_Number(*child);
            break;

            case TAG_IN:
                elfConfiguration[computer].ioConfiguration.tapeIn.qValue = -1;
                if (child->GetAttribute("q") == "1")
                    elfConfiguration[computer].ioConfiguration.tapeIn.qValue = 1;
                if (child->GetAttribute("q") == "0")
                    elfConfiguration[computer].ioConfiguration.tapeIn.qValue = 0;

                elfConfiguration[computer].ioConfiguration.tapeIn.portNumber = (int)parseXml_Number(*child);
            break;

            case TAG_OUT:
                elfConfiguration[computer].ioConfiguration.tapeOut.qValue = -1;
                if (child->GetAttribute("q") == "1")
                    elfConfiguration[computer].ioConfiguration.tapeOut.qValue = 1;
                if (child->GetAttribute("q") == "0")
                    elfConfiguration[computer].ioConfiguration.tapeOut.qValue = 0;

                elfConfiguration[computer].ioConfiguration.tapeOut.portNumber = (int)parseXml_Number(*child);
            break;

            case TAG_FREQ:
                if (child->GetAttribute("type") == "0")
                    elfConfiguration[computer].tape_frequency0 = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "1")
                    elfConfiguration[computer].tape_frequency1 = (int)parseXml_Number(*child);
                elfConfiguration[computer].tape_frequencyBorder = (elfConfiguration[computer].tape_frequency1 + elfConfiguration[computer].tape_frequency0) / 2;
                if (elfConfiguration[computer].tape_frequency0 > elfConfiguration[computer].tape_frequency1)
                    elfConfiguration[computer].tape_revInput = true;
            break;

            case TAG_THRESHOLD:
                if (child->GetAttribute("type") == "8bit")
                    elfConfiguration[computer].tape_threshold8Bit = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "16bit")
                    elfConfiguration[computer].tape_threshold16Bit  = (int)parseXml_Number(*child);
            break;

            case TAG_STARTBIT:
                elfConfiguration[computer].tape_startBit = (int)parseXml_Number(*child);
            break;

            case TAG_DATABITS:
                elfConfiguration[computer].tape_dataBits = (int)parseXml_Number(*child);
            break;

            case TAG_STOPBIT:
                elfConfiguration[computer].tape_stopBit = (int)parseXml_Number(*child);
            break;

            case TAG_STOPDELAY:
                elfConfiguration[computer].tape_stopDelay = (int)parseXml_Number(*child);
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
            case TAG_DIRNAME:
                memAccessDirDefined_ = true;
                conf[computer].ramDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].ramDir_.Right(1) != pathSeparator_)
                    conf[computer].ramDir_ += pathSeparator_;
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
                elfConfiguration[computer].qSound_ = QSOUNDEXT;
                conf[computer].beepFrequency_ = (int)parseXml_Number(*child);
            break;

            case TAG_SW:
                elfConfiguration[computer].qSound_ = QSOUNDSW;
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
                elfConfiguration[computer].ioConfiguration.hexOutput = (int)parseXml_Number(*child);
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
        "mainram",
        "ram",
        "nvram",
        "ems",
        "mapper",
        "giant",
        "copy",
        "slot",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_ROM,
        TAG_DIAG,
        TAG_MAINRAM,
        TAG_RAM,
        TAG_NVRAM,
        TAG_EMS,
        TAG_MAPPER,
        TAG_GIANT,
        TAG_COPY,
        TAG_SLOT,
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
                conf[computer].memConfig_.resize(conf[computer].memConfigNumber_+1);
                parseXml_RomRam (computer, *child, (int)(ROM + 256*conf[computer].memConfigNumber_), conf[computer].memConfigNumber_);
                conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask = parseXml_Number(*child, "mask");
                if (conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask != 0)
                {
                    conf[computer].memConfig_[conf[computer].memConfigNumber_].useMemMask = true;
                    conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask |= 0xff;
                }
                else
                    conf[computer].memConfig_[conf[computer].memConfigNumber_].useMemMask = false;
                conf[computer].memConfigNumber_++;
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

            case TAG_MAINRAM:
                parseXml_RomRam (computer, *child, MAINRAM, 0);
                conf[computer].memConfig_[0].memMask = parseXml_Number(*child, "mask");
                if (conf[computer].memConfig_[0].memMask != 0)
                {
                    conf[computer].memConfig_[0].useMemMask = true;
                    conf[computer].memConfig_[0].memMask |= 0xff;
                }
                else
                    conf[computer].memConfig_[0].useMemMask = false;
            break;

            case TAG_RAM:
                conf[computer].memConfig_.resize(conf[computer].memConfigNumber_+1);
                parseXml_RomRam (computer, *child, (int)(RAM + 256*conf[computer].memConfigNumber_), conf[computer].memConfigNumber_);
                conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask = parseXml_Number(*child, "mask");
                if (conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask != 0)
                {
                    conf[computer].memConfig_[conf[computer].memConfigNumber_].useMemMask = true;
                    conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask |= 0xff;
                }
                else
                    conf[computer].memConfig_[conf[computer].memConfigNumber_].useMemMask = false;
                conf[computer].memConfigNumber_++;
            break;

            case TAG_NVRAM:
                elfConfiguration[computer].useNvRam = true;
                conf[computer].memConfig_.resize(conf[computer].memConfigNumber_+1);
                parseXml_RomRam (computer, *child, (int)(NVRAM + 256*conf[computer].memConfigNumber_), conf[computer].memConfigNumber_);
                conf[computer].memConfig_[conf[computer].memConfigNumber_].useMemMask = false;

                conf[computer].memConfigNumber_++;
            break;

            case TAG_EMS:
                conf[computer].emsConfig_.resize(conf[computer].emsConfigNumber_+1);
                elfConfiguration[computer].ioConfiguration.emsOutput.resize(conf[computer].emsConfigNumber_+1);

                if (child->GetAttribute("type") == "ROM")
                    conf[computer].emsConfig_[conf[computer].emsConfigNumber_].emsType = ROM;
                else
                    conf[computer].emsConfig_[conf[computer].emsConfigNumber_].emsType = RAM;

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

void XmlParser::parseXml_RomRam(int computer, wxXmlNode &node, int type, size_t configNumber)
{
    wxString tagList[]=
    {
        "start",
        "end",
        "filename",
        "dirname",
        "mp",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_START,
        TAG_END,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_MP,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    conf[computer].memConfig_[configNumber].start = 0;
    conf[computer].memConfig_[configNumber].end = 0;
    conf[computer].memConfig_[configNumber].memMask = 0;
    conf[computer].memConfig_[configNumber].filename = "";
    conf[computer].memConfig_[configNumber].dirname = conf[computer].mainDir_ ;
    conf[computer].memConfig_[configNumber].type = type;

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
                if (node.GetName() == "mainram")
                    conf[computer].ramFileFromGui_ = false;

                conf[computer].memConfig_[configNumber].filename = child->GetNodeContent();
                
                conf[computer].memConfig_[configNumber].verifyFileExist = (child->GetAttribute("verify") == "true");
            break;

            case TAG_DIRNAME:
                conf[computer].memConfig_[configNumber].dirname =  dataDir_ + child->GetNodeContent();
                if (conf[computer].memConfig_[configNumber].dirname.Right(1) != pathSeparator_)
                {
                    conf[computer].memConfig_[configNumber].dirname += pathSeparator_;
                }
            break;

            case TAG_MP:
                if (node.GetName() == "nvram")
                    parseXml_nvRamMp(computer, *child);
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
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_OUT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    conf[computer].emsConfig_[configNumber].start = 0;
    conf[computer].emsConfig_[configNumber].end = 0;
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
                    elfConfiguration[computer].ioConfiguration.emsOutput[configNumber] = start & 07;
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
    
    conf[computer].slotConfig_.slotInfo[slot].filename = "";
    conf[computer].slotConfig_.slotInfo[slot].dirname = conf[computer].mainDir_ ;

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
        *numberString = "";
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

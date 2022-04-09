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
#include "guicdp18s600.h"
#include "microsetup.h"

Word guiRomSize[]=
{
    0x7ff, 0xfff, 0x1fff
};

Word guiRomSizeCDP18S601[]=
{
    0x3ff, 0x7ff
};

int inhibit[5][12][2]=
{
    {
        {0xFFFF, 0x0000}, {0xE000, 0xEFFF}, {0xF000, 0xFFFF}, {0xE000, 0xFFFF}, {0xE000, 0xE3FF}, {0xE400, 0xE7FF}, {0xE800, 0xEBFF}, {0xEC00, 0xEFFF}, {0xF000, 0xF3FF}, {0xF400, 0xF7FF}, {0xF800, 0xFBFF}, {0xFC00, 0xFFFF}
    },
    {
        {0xFFFF, 0x0000}, {0x7000, 0x77FF}, {0x7800, 0x7FFF}, {0x7000, 0x7FFF}, {0x7000, 0x73FF}, {0x7400, 0x77FF}, {0x7800, 0x7BFF}, {0x7C00, 0x7FFF}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
    },
    {
        {0xFFFF, 0x0000}, {0xF000, 0xF7FF}, {0xF800, 0xFFFF}, {0xF000, 0xFFFF}, {0xF000, 0xF3FF}, {0xF400, 0xF7FF}, {0xF800, 0xFBFF}, {0xFC00, 0xFFFF}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
    },
    {
        {0xFFFF, 0x0000}, {0x0000, 0x07FF}, {0x0800, 0x0FFF}, {0x0000, 0x0FFF}, {0x0000, 0x03FF}, {0x0400, 0x07FF}, {0x0800, 0x0BFF}, {0x0C00, 0x0FFF}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
    },
    {
        {0xFFFF, 0x0000}, {0x8000, 0x87FF}, {0x8800, 0x8FFF}, {0x8000, 0x8FFF}, {0x8000, 0x83FF}, {0x8400, 0x87FF}, {0x8800, 0x8BFF}, {0x8C00, 0x8FFF}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
    }
};

wxString microBoardStr[]=
{
    "CDP18S600",
    "CDP18S601",
    "CDP18S602",
    "CDP18S603",
    "CDP18S603A",
    "CDP18S604B",
    "CDP18S605",
    "CDP18S606",
    "CDP18S607",
    "CDP18S608",
    "CDP18S609",
    "CDP18S610",
    "RCASBC",
    "CDP18S620",
    "CDP18S621",
    "CDP18S623A",
    "CDP18S625",
    "CDP18S626",
    "CDP18S627",
    "CDP18S628",
    "CDP18S629",
    "CDP18S640",
    "CDP18S641",
    "CDP18S646",
    "CDP18S651",
    "CDP18S652",
    "CDP18S660",
    "CDP18S661B",
    "CDP18S661V1",
    "CDP18S661V3"
};

int cardBoardNumber[]= // Keep same order as in GUI, new board get next number
{
	0,
	1, // CARD_CDP18S620,
	2, // CARD_CDP18S621,
	3, // CARD_CDP18S623A,
	4, // CARD_CDP18S625,
	5, // CARD_CDP18S626,
	6, // CARD_CDP18S627,
	7, // CARD_CDP18S628,
	8, // CARD_CDP18S629,
	9, // CARD_CDP18S640,
	10, // CARD_CDP18S641,
    15, // CARD_CDP18S646,
	11, // CARD_CDP18S651,
    12, // CARD_CDP18S652,
    16, // CARD_CDP18S660,
	13, // CARD_CDP18S661B,
    17, // CARD_CDP18S661V1,
    14, // CARD_CDP18S661V3,
};

int cardBoardId[]= // Order should NOT change add new one in the bottom
{
	0, // empty
	CARD_CDP18S620, // CARD_CDP18S620
	CARD_CDP18S621, // CARD_CDP18S621
	CARD_CDP18S623A, // CARD_CDP18S623A
	CARD_CDP18S625, // CARD_CDP18S625,
	CARD_CDP18S626, // CARD_CDP18S626,
	CARD_CDP18S627, // CARD_CDP18S627,
	CARD_CDP18S628, // CARD_CDP18S628,
	CARD_CDP18S629, // CARD_CDP18S629,
	CARD_CDP18S640, // CARD_CDP18S640,
	CARD_CDP18S641, // CARD_CDP18S641,
	CARD_CDP18S651, // CARD_CDP18S651,
	CARD_CDP18S652, // CARD_CDP18S652,
	CARD_CDP18S661B, // CARD_CDP18S661B,
	CARD_CDP18S661V3, // CARD_CDP18S661V3,
    CARD_CDP18S646,  // CARD_CDP18S646
    CARD_CDP18S660,  // CARD_CDP18S660
    CARD_CDP18S661V1, // CARD_CDP18S661V1,
	// new card etc..
};

BEGIN_EVENT_TABLE(GuiCdp18s600, GuiTMC2000)

    EVT_BUTTON(XRCID("Card1Microboard"), GuiCdp18s600::onMicroboardCard1Setup)
    EVT_BUTTON(XRCID("Card2Microboard"), GuiCdp18s600::onMicroboardCard2Setup)
    EVT_BUTTON(XRCID("Card3Microboard"), GuiCdp18s600::onMicroboardCard3Setup)
    EVT_BUTTON(XRCID("Card4Microboard"), GuiCdp18s600::onMicroboardCard4Setup)
    EVT_BUTTON(XRCID("Card5Microboard"), GuiCdp18s600::onMicroboardCardSetup)
    EVT_CHOICE(XRCID("Card1ChoiceMicroboard"), GuiCdp18s600::onMicroboardType1)
    EVT_CHOICE(XRCID("Card2ChoiceMicroboard"), GuiCdp18s600::onMicroboardType2)
    EVT_CHOICE(XRCID("Card3ChoiceMicroboard"), GuiCdp18s600::onMicroboardType3)
    EVT_CHOICE(XRCID("Card4ChoiceMicroboard"), GuiCdp18s600::onMicroboardType4)
    EVT_CHOICE(XRCID("Card5ChoiceMicroboard"), GuiCdp18s600::onMicroboardType5)

    EVT_CHOICE(XRCID("VTTypeMicroboard"), GuiMain::onVT100)
    EVT_SPIN_UP(XRCID("ZoomSpinVtMicroboard"), GuiMain::onZoomVt)
    EVT_SPIN_DOWN(XRCID("ZoomSpinVtMicroboard"), GuiMain::onZoomVt)
    EVT_TEXT(XRCID("ZoomValueVtMicroboard"), GuiMain::onZoomValueVt)
    EVT_BUTTON(XRCID("FullScreenF3Microboard"), GuiMain::onFullScreen)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonMicroboard"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileMicroboard"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileMicroboard"), GuiMain::onScreenDumpFileText)
    EVT_BUTTON(XRCID("ScreenDumpF5Microboard"), GuiMain::onScreenDump)

    EVT_CHECKBOX(XRCID("MicroboardForceUC"), GuiCdp18s600::onCdp18s600ForceUpperCase)
    EVT_BUTTON(XRCID("VtSetupMicroboard"), GuiMain::onVtSetup)
    EVT_CHECKBOX(XRCID("StretchDotMicroboard"), GuiMain::onStretchDot)
    EVT_BUTTON(XRCID("ColoursMicroboard"), Main::onColoursDef)

    EVT_BUTTON(XRCID("SaveButtonMicroboard"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonMicroboard"), GuiMain::onLoadButton)
    EVT_BUTTON(XRCID("RunButtonMicroboard"), GuiMain::onLoadRunButton)
    EVT_BUTTON(XRCID("DsaveButtonMicroboard"), GuiMain::onDataSaveButton)

    EVT_CHECKBOX(XRCID("UseLocationMicroboard"), GuiMain::onUseLocation)
    EVT_TEXT(XRCID("SaveStartMicroboard"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndMicroboard"), GuiMain::onSaveEnd)

    EVT_CHOICE(XRCID("VTBaudTChoiceMicroboard"), GuiCdp18s600::onCdp18s600BaudT)

    EVT_CHECKBOX(XRCID("AutoBootMicroboard"), GuiCdp18s600::onAutoBoot)
    EVT_CHOICE(XRCID("AutoBootTypeMicroboard"), GuiCdp18s600::onAutoBootType)

    EVT_TEXT(XRCID("ShowAddressMicroboard"), GuiMain::onLedTimer)
    EVT_CHECKBOX(XRCID("PioMicroboard"), GuiCdp18s600::onPioWindows)

    EVT_BUTTON(XRCID("FDC0_ButtonMicroboard"), GuiMain::onUpdDisk0)
    EVT_TEXT(XRCID("FDC0_FileMicroboard"), GuiMain::onUpdDiskText0)
    EVT_BUTTON(XRCID("Eject_FDC0Microboard"), GuiMain::onUpdDiskEject0)
    EVT_BUTTON(XRCID("FDC0_SwitchMicroboard"), GuiMain::onUpdDiskDirSwitch0)
    EVT_BUTTON(XRCID("FDC1_ButtonMicroboard"), GuiMain::onUpdDisk1)
    EVT_TEXT(XRCID("FDC1_FileMicroboard"), GuiMain::onUpdDiskText1)
    EVT_BUTTON(XRCID("Eject_FDC1Microboard"), GuiMain::onUpdDiskEject1)
    EVT_BUTTON(XRCID("FDC1_SwitchMicroboard"), GuiMain::onUpdDiskDirSwitch1)
    EVT_BUTTON(XRCID("FDC2_ButtonMicroboard"), GuiMain::onUpdDisk2)
    EVT_TEXT(XRCID("FDC2_FileMicroboard"), GuiMain::onUpdDiskText2)
    EVT_BUTTON(XRCID("Eject_FDC2Microboard"), GuiMain::onUpdDiskEject2)
    EVT_BUTTON(XRCID("FDC2_SwitchMicroboard"), GuiMain::onUpdDiskDirSwitch2)
    EVT_BUTTON(XRCID("FDC3_ButtonMicroboard"), GuiMain::onUpdDisk3)
    EVT_TEXT(XRCID("FDC3_FileMicroboard"), GuiMain::onUpdDiskText3)
    EVT_BUTTON(XRCID("Eject_FDC3Microboard"), GuiMain::onUpdDiskEject3)
    EVT_BUTTON(XRCID("FDC3_SwitchMicroboard"), GuiMain::onUpdDiskDirSwitch3)

    EVT_BUTTON(XRCID("CasButtonMicroboard"), GuiMain::onCassette)
    EVT_BUTTON(XRCID("EjectCasMicroboard"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileMicroboard"), GuiMain::onCassetteText)

    EVT_BUTTON(XRCID("CasButton1Microboard"), GuiMain::onCassette1)
    EVT_BUTTON(XRCID("EjectCas1Microboard"), GuiMain::onCassette1Eject)
    EVT_TEXT(XRCID("WavFile1Microboard"), GuiMain::onCassette1Text)

    EVT_CHECKBOX(XRCID("TurboMicroboard"), GuiMain::onTurbo)
    EVT_TEXT(XRCID("TurboClockMicroboard"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadMicroboard"), GuiMain::onAutoLoad)
    EVT_BUTTON(XRCID("CasLoadMicroboard"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasSaveMicroboard"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasStopMicroboard"), GuiMain::onCassetteStop)
    EVT_BUTTON(XRCID("RealCasLoadMicroboard"), GuiMain::onRealCas)

    EVT_BUTTON(XRCID("CasLoad1Microboard"), GuiMain::onCassetteLoad1)
    EVT_BUTTON(XRCID("CasSave1Microboard"), GuiMain::onCassetteSave1)
    EVT_BUTTON(XRCID("CasStop1Microboard"), GuiMain::onCassetteStop)

END_EVENT_TABLE()

GuiCdp18s600::GuiCdp18s600(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode_, wxString dataDir, wxString iniDir)
: GuiTMC2000(title, pos, size, mode_, dataDir, iniDir)
{
}

void GuiCdp18s600::readCdp18s600Config()
{
    wxString cardNumberStr;
 
    readElfPortConfig(MICROBOARD, "Microboard");

    selectedComputer_ = MICROBOARD;
    elfConfiguration[MICROBOARD].fdcType_ = FDCTYPE_MICROBOARD;
    
    conf[MICROBOARD].loadFileNameFull_ = "";
    conf[MICROBOARD].loadFileName_ = "";
    
    conf[MICROBOARD].pLoadSaveName_[0] = 'S';
    conf[MICROBOARD].pLoadSaveName_[1] = 'U';
    conf[MICROBOARD].pLoadSaveName_[2] = 'P';
    conf[MICROBOARD].pLoadSaveName_[3] = 'E';
    
    conf[MICROBOARD].defus_ = 0x81;
    conf[MICROBOARD].eop_ = 0x83;
    conf[MICROBOARD].string_ = 0x92;
    conf[MICROBOARD].arrayValue_ = 0x94;
    conf[MICROBOARD].eod_ = 0x99;
    conf[MICROBOARD].basicRamAddress_ = 0x200;

    conf[MICROBOARD].saveEndString_ = "";
    conf[MICROBOARD].saveExecString_ = "";
    
    conf[MICROBOARD].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Microboard" + pathSeparator_;
    
    conf[MICROBOARD].mainDir_ = readConfigDir("/Dir/Microboard/Main", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].romDir_[U21ROM] = readConfigDir("/Dir/Microboard/Main_Rom_File21", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].romDir_[U20ROM] = readConfigDir("/Dir/Microboard/Main_Rom_File20", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].romDir_[U19ROM] = readConfigDir("/Dir/Microboard/Main_Rom_File19", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].romDir_[U18ROM] = readConfigDir("/Dir/Microboard/Main_Rom_File18", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].romDir_[U17ROM] = readConfigDir("/Dir/Microboard/Main_Rom_File17", dataDir_ + "Microboard" + pathSeparator_);
    elfConfiguration[MICROBOARD].vtCharRomDir_ = readConfigDir("/Dir/Microboard/Vt_Font_Rom_File", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].keyFileDir_ = readConfigDir("/Dir/Microboard/Key_File", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].screenDumpFileDir_ = readConfigDir("/Dir/Microboard/Video_Dump_File", dataDir_ + "Microboard" + pathSeparator_);
    
    conf[MICROBOARD].screenDumpFile_ = configPointer->Read("/Microboard/Video_Dump_File", "screendump.png");
    
    configPointer->Read("/Microboard/SerialLog", &elfConfiguration[MICROBOARD].serialLog, false);
    configPointer->Read("/Microboard/Enable_Vt_Stretch_Dot", &conf[MICROBOARD].stretchDot_, false);
    configPointer->Read("/Microboard/Enable_Vt_External", &elfConfiguration[MICROBOARD].vtExternal, false);
    
    wxString defaultClock;
    wxString defaultBlock;

    elfConfiguration[MICROBOARD].useUpd765 = false;
    elfConfiguration[MICROBOARD].upd765Group = (int)configPointer->Read("/Microboard/Upd765Group", 8l);
    elfConfiguration[MICROBOARD].printerGroup = (int)configPointer->Read("/Microboard/PrinterGroup", 1l);
    elfConfiguration[MICROBOARD].uartGroup = (int)configPointer->Read("/Microboard/UartGroup", 1l);
    elfConfiguration[MICROBOARD].elfPortConf.uartOut = (int)configPointer->Read("/Microboard/UartOut", 2l);
    elfConfiguration[MICROBOARD].elfPortConf.uartControl = (int)configPointer->Read("/Microboard/UartControl", 3l);
    elfConfiguration[MICROBOARD].v1870Group = (int)configPointer->Read("/Microboard/V1870Group", 0x80l);
    elfConfiguration[MICROBOARD].cdp18s660Group1 = (int)configPointer->Read("/Microboard/Pio1Group", 0x10l);
    elfConfiguration[MICROBOARD].cdp18s660Group2 = (int)configPointer->Read("/Microboard/Pio2Group", 0x20l);

    elfConfiguration[MICROBOARD].vtType = (int)configPointer->Read("/Microboard/VT_Type", 0l);
    floppyDir_[elfConfiguration[MICROBOARD].fdcType_][0] = readConfigDir("/Dir/Microboard/FDC0_File", dataDir_ + "Microboard" + pathSeparator_);
    floppyDir_[elfConfiguration[MICROBOARD].fdcType_][1] = readConfigDir("/Dir/Microboard/FDC1_File", dataDir_ + "Microboard" + pathSeparator_);
    floppyDir_[elfConfiguration[MICROBOARD].fdcType_][2] = readConfigDir("/Dir/Microboard/FDC2_File", dataDir_ + "Microboard" + pathSeparator_);
    floppyDir_[elfConfiguration[MICROBOARD].fdcType_][3] = readConfigDir("/Dir/Microboard/FDC3_File", dataDir_ + "Microboard" + pathSeparator_);
    floppyDirSwitched_[elfConfiguration[MICROBOARD].fdcType_][0] = readConfigDir("/Dir/Microboard/FDC0_File_Switched", dataDir_ + "Microboard" + pathSeparator_ + "Software" + pathSeparator_);
    floppyDirSwitched_[elfConfiguration[MICROBOARD].fdcType_][1] = readConfigDir("/Dir/Microboard/FDC1_File_Switched", dataDir_ + "Microboard" + pathSeparator_ + "Software" + pathSeparator_);
    floppyDirSwitched_[elfConfiguration[MICROBOARD].fdcType_][2] = readConfigDir("/Dir/Microboard/FDC2_File_Switched", dataDir_ + "Microboard" + pathSeparator_ + "PLM" + pathSeparator_);
    floppyDirSwitched_[elfConfiguration[MICROBOARD].fdcType_][3] = readConfigDir("/Dir/Microboard/FDC3_File_Switched", dataDir_ + "Microboard" + pathSeparator_ + "BASIC" + pathSeparator_);
    conf[MICROBOARD].charRomDir_ = readConfigDir("/Dir/Microboard/Font_Rom_File", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].printFileDir_ = readConfigDir("/Dir/Microboard/Print_File", dataDir_ + "Microboard" + pathSeparator_);
    floppy_[elfConfiguration[MICROBOARD].fdcType_][0] = configPointer->Read("/Microboard/FDC0_File", "microdos.img");
    floppy_[elfConfiguration[MICROBOARD].fdcType_][1] = configPointer->Read("/Microboard/FDC1_File", "PLM 1800.img");
    floppy_[elfConfiguration[MICROBOARD].fdcType_][2] = configPointer->Read("/Microboard/FDC2_File", "basic1.img");
    floppy_[elfConfiguration[MICROBOARD].fdcType_][3] = configPointer->Read("/Microboard/FDC3_File", "");
    configPointer->Read("/Microboard/DirectoryMode_0", &directoryMode_[elfConfiguration[MICROBOARD].fdcType_][0], false);
    configPointer->Read("/Microboard/DirectoryMode_1", &directoryMode_[elfConfiguration[MICROBOARD].fdcType_][1], false);
    configPointer->Read("/Microboard/DirectoryMode_2", &directoryMode_[elfConfiguration[MICROBOARD].fdcType_][2], false);
    configPointer->Read("/Microboard/DirectoryMode_3", &directoryMode_[elfConfiguration[MICROBOARD].fdcType_][3], false);
    conf[MICROBOARD].printFile_ = configPointer->Read("/Microboard/Print_File", "printerout.txt");

    conf[MICROBOARD].microboardType_[0] = cardBoardId[(int)configPointer->Read("/Microboard/MicroboardType0", 0l)];
    conf[MICROBOARD].microboardType_[1] = cardBoardId[(int)configPointer->Read("/Microboard/MicroboardType1", 14l)];

    for (int card=2; card<24; card++)
    {
        cardNumberStr.Printf("%d",card);
        conf[MICROBOARD].microboardType_[card] = cardBoardId[(int)configPointer->Read("/Microboard/MicroboardType"+ cardNumberStr, 0l)];
    }
    
    setCardMax(&conf[MICROBOARD]);

    elfConfiguration[MICROBOARD].baudR = (int)configPointer->Read("/Microboard/Vt_Baud_Receive", 0l);
    elfConfiguration[MICROBOARD].baudT = (int)configPointer->Read("/Microboard/Vt_Baud_Transmit", 0l);
    defaultClock.Printf("%1.4f", 4.9152);

    conf[MICROBOARD].rom_[U21ROM] = configPointer->Read("/Microboard/Main_Rom_File21", "");
    conf[MICROBOARD].rom_[U20ROM] = configPointer->Read("/Microboard/Main_Rom_File20", "FULL_microboard_pal2.hex");
    conf[MICROBOARD].rom_[U19ROM] = configPointer->Read("/Microboard/Main_Rom_File19", "");
    conf[MICROBOARD].rom_[U18ROM] = configPointer->Read("/Microboard/Main_Rom_File18", "");
    conf[MICROBOARD].rom_[U17ROM] = configPointer->Read("/Microboard/Main_Rom_File17", "");

    conf[MICROBOARD].microChipType_[ONE_SOCKET] = (int)configPointer->Read("/Microboard/MicroChipTypeOneSocket", 2l);
    conf[MICROBOARD].microChipType_[FOUR_SOCKET] = (int)configPointer->Read("/Microboard/MicroChipTypeFourSocket", 2l);

    conf[MICROBOARD].microChipMemory_[U21ROM] = (int)configPointer->Read("/Microboard/MicroChipMemoryU21", 1l);
    configPointer->Read("/Microboard/MicroChiDisableU21", &conf[MICROBOARD].microChipDisable_[U21ROM], false);
    
    conf[MICROBOARD].microChipMemory_[U20ROM] = (int)configPointer->Read("/Microboard/MicroChipMemoryU20", 1l);
    configPointer->Read("/Microboard/MicroChiDisableU20", &conf[MICROBOARD].microChipDisable_[U20ROM], false);
    
    conf[MICROBOARD].microChipMemory_[U19ROM] = (int)configPointer->Read("/Microboard/MicroChipMemoryU19", 1l);
    configPointer->Read("/Microboard/MicroChiDisableU19", &conf[MICROBOARD].microChipDisable_[U19ROM], false);
    
    conf[MICROBOARD].microChipMemory_[U18ROM] = (int)configPointer->Read("/Microboard/MicroChipMemoryU18", 1l);
    configPointer->Read("/Microboard/MicroChiDisableU18", &conf[MICROBOARD].microChipDisable_[U18ROM], false);
    
    conf[MICROBOARD].microChipMemory_[U17ROM] = (int)configPointer->Read("/Microboard/MicroChipMemoryU17", 1l);
    configPointer->Read("/Microboard/MicroChiDisableU17", &conf[MICROBOARD].microChipDisable_[U17ROM], false);

    conf[MICROBOARD].microChipLocation_[ONE_SOCKET] = (int)configPointer->Read("/Microboard/MicroChipLocationOneSocket", 16l);
    conf[MICROBOARD].microChipLocation_[FOUR_SOCKET_ROM1] = (int)configPointer->Read("/Microboard/MicroChipLocationRom1Socket", 0l);
    conf[MICROBOARD].microChipLocation_[FOUR_SOCKET_ROM2] = (int)configPointer->Read("/Microboard/MicroChipLocationRom2Socket", 8l);

    defaultBlock.Printf("%d", 16);
    conf[MICROBOARD].microChipBlock_[ONE_SOCKET] = configPointer->Read("/Microboard/MicroBlockOneSocket", defaultBlock);
    
    defaultBlock.Printf("%d", 0);
    conf[MICROBOARD].microChipBlock_[FOUR_SOCKET] = configPointer->Read("/Microboard/MicroBlockFourSocket", defaultBlock);
    
    configPointer->Read("/Microboard/Pio_Windows", &elfConfiguration[MICROBOARD].usePio, false);
    configPointer->Read("/Microboard/Pio1_Windows", &elfConfiguration[MICROBOARD].usePioWindow1Cdp18s660, true);
    configPointer->Read("/Microboard/Pio2_Windows", &elfConfiguration[MICROBOARD].usePioWindow2Cdp18s660, true);
    
    microMemConf[0].rom_[MEM_SECTION1] = configPointer->Read("/Microboard/Card0RomFile1", "ut71.bin");

    for (int card=0; card<conf[MICROBOARD].microboardMaxCards_; card++)
    {
        cardNumberStr.Printf("%d",card);
        microMemConf[card].memType[MEM_SECTION0] = (int)configPointer->Read("/Microboard/Card" + cardNumberStr+"MemType0", 0l);
        microMemConf[card].memType[MEM_SECTION1] = (int)configPointer->Read("/Microboard/Card" + cardNumberStr+"MemType1", 0l);
        microMemConf[card].memType[MEM_SECTION2] = (int)configPointer->Read("/Microboard/Card" + cardNumberStr+"MemType2", 0l);
        microMemConf[card].memType[MEM_SECTION3] = (int)configPointer->Read("/Microboard/Card" + cardNumberStr+"MemType3", 0l);
        microMemConf[card].memLocation_[0] = (int)configPointer->Read("/Microboard/Card" + cardNumberStr+"MemLocation0", 0l);
        microMemConf[card].memLocation_[1] = (int)configPointer->Read("/Microboard/Card" + cardNumberStr+"MemLocation1", 0l);
        microMemConf[card].memLocation_[2] = (int)configPointer->Read("/Microboard/Card" + cardNumberStr+"MemLocation2", 0l);
        defaultBlock.Printf("%d", 0);
        microMemConf[card].chipBlockRam_ = configPointer->Read("/Microboard/Card" + cardNumberStr+"ChipBlockRam", defaultBlock);
        microMemConf[card].chipBlockRom_[0] = configPointer->Read("/Microboard/Card" + cardNumberStr+"ChipBlockRom0", defaultBlock);
        microMemConf[card].chipBlockRom_[1] = configPointer->Read("/Microboard/Card" + cardNumberStr+"ChipBlockRom1", defaultBlock);
        microMemConf[card].socketSize_[0] = (int)configPointer->Read("/Microboard/Card" + cardNumberStr+"Socket0Size", 0l);
        microMemConf[card].socketSize_[1] = (int)configPointer->Read("/Microboard/Card" + cardNumberStr+"Socket1Size", 0l);
        configPointer->Read("/Microboard/Card" + cardNumberStr+"MemInhibitBlock00", &microMemConf[card].inhibitBlock_[0][0], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr+"MemInhibitBlock01", &microMemConf[card].inhibitBlock_[0][1], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr+"MemInhibitBlock02", &microMemConf[card].inhibitBlock_[0][2], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr+"MemInhibitBlock03", &microMemConf[card].inhibitBlock_[0][3], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr+"MemInhibitBlock10", &microMemConf[card].inhibitBlock_[1][0], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr+"MemInhibitBlock11", &microMemConf[card].inhibitBlock_[1][1], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr+"MemInhibitBlock12", &microMemConf[card].inhibitBlock_[1][2], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr+"MemInhibitBlock13", &microMemConf[card].inhibitBlock_[1][3], false);
        microMemConf[card].inhibit64_ = (int)configPointer->Read("/Microboard/Card" + cardNumberStr+"MemInhibit64", 0l);
        microMemConf[card].inhibit32Low_ = (int)configPointer->Read("/Microboard/Card" + cardNumberStr+"MemInhibit32Low", 0l);
        microMemConf[card].inhibit32High_ = (int)configPointer->Read("/Microboard/Card" + cardNumberStr+"MemInhibit32High", 0l);
        microMemConf[card].ram_ = configPointer->Read("/Microboard/Card" + cardNumberStr+"RamFile0", "");
        microMemConf[card].rom_[MEM_SECTION0] = configPointer->Read("/Microboard/Card" + cardNumberStr+"RomFile0", "");
        if (card != 0)
            microMemConf[card].rom_[MEM_SECTION1] = configPointer->Read("/Microboard/Card" + cardNumberStr+"RomFile1", "");
        microMemConf[card].rom_[MEM_SECTION2] = configPointer->Read("/Microboard/Card" + cardNumberStr+"RomFile2", "");
        microMemConf[card].rom_[MEM_SECTION3] = configPointer->Read("/Microboard/Card" + cardNumberStr+"RomFile3", "");
        microMemConf[card].ramDir_ = readConfigDir("/Dir/Microboard/Card" + cardNumberStr+"RamFile0", dataDir_ + "Microboard" + pathSeparator_);
        microMemConf[card].romDir_[MEM_SECTION0] = readConfigDir("/Dir/Microboard/Card" + cardNumberStr+"RomFile0", dataDir_ + "Microboard" + pathSeparator_);
        microMemConf[card].romDir_[MEM_SECTION1] = readConfigDir("/Dir/Microboard/Card" + cardNumberStr+"RomFile1", dataDir_ + "Microboard" + pathSeparator_);
        microMemConf[card].romDir_[MEM_SECTION2] = readConfigDir("/Dir/Microboard/Card" + cardNumberStr+"RomFile2", dataDir_ + "Microboard" + pathSeparator_);
        microMemConf[card].romDir_[MEM_SECTION3] = readConfigDir("/Dir/Microboard/Card" + cardNumberStr+"RomFile3", dataDir_ + "Microboard" + pathSeparator_);
        configPointer->Read("/Microboard/Card" + cardNumberStr+"DisableMemory", &microMemConf[card].disableCardMemory_, false);
    }
   
    setCardType();
    
    conf[MICROBOARD].wavFileDir_[0] = readConfigDir("/Dir/Microboard/Wav_File", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].wavFileDir_[1] = readConfigDir("/Dir/Microboard/Wav_File1", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].wavFile_[0] = configPointer->Read("/Microboard/Wav_File", "");
    conf[MICROBOARD].wavFile_[1] = configPointer->Read("/Microboard/Wav_File1", "");
    conf[MICROBOARD].turboClock_ = configPointer->Read("/Microboard/Turbo_Clock_Speed", "15");
    configPointer->Read("/Microboard/Enable_Turbo_Cassette", &conf[MICROBOARD].turbo_, true);
    configPointer->Read("/Microboard/Enable_Auto_Cassette", &conf[MICROBOARD].autoCassetteLoad_, true);
    configPointer->Read("/Microboard/Enable_Real_Cassette", &conf[MICROBOARD].realCassetteLoad_, false);

    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 2.0);
    conf[MICROBOARD].zoom_ = convertLocale(configPointer->Read("/Microboard/Zoom", defaultZoom));
    conf[MICROBOARD].volume_ = (int)configPointer->Read("/Microboard/Volume", 25l);
    elfConfiguration[MICROBOARD].pageMemSize = (int)configPointer->Read("/Microboard/PageMemSize", 0l);
    conf[MICROBOARD].charRom_ = configPointer->Read("/Microboard/Font_Rom_File", "character.bin");
    elfConfiguration[MICROBOARD].v1870InterruptMode = (int)configPointer->Read("/Microboard/V1870InterruptMode", 1l);
    elfConfiguration[MICROBOARD].v1870VideoMode = (int)configPointer->Read("/Microboard/V1870VideoMode", 1l);
    conf[MICROBOARD].printMode_ = (int)configPointer->Read("/Microboard/Print_Mode", 1l);

    elfConfiguration[MICROBOARD].elfPortConf.keyboardEf = (int)configPointer->Read("/Microboard/KeyboardEF", 2l);
    elfConfiguration[MICROBOARD].keyboardType = (int)configPointer->Read("/Microboard/KeyboardType", 0l);
    
    elfConfiguration[MICROBOARD].bellFrequency_ = (int)configPointer->Read("/Microboard/Bell_Frequency", 800);
    elfConfiguration[MICROBOARD].vt52SetUpFeature_ = configPointer->Read("/Microboard/VT52Setup", 0x00004092l);
    elfConfiguration[MICROBOARD].vt100SetUpFeature_ = configPointer->Read("/Microboard/VT100Setup", 0x0000cad2l);
    elfConfiguration[MICROBOARD].vtExternalSetUpFeature_ = configPointer->Read("/Microboard/VTExternalSetup", 0x0000cad2l);
    elfConfiguration[MICROBOARD].serialPort_ = configPointer->Read("/Microboard/VtSerialPortChoice", "");
    elfConfiguration[MICROBOARD].vtEf = false;
    elfConfiguration[MICROBOARD].vtQ = true;

    elfConfiguration[MICROBOARD].vtCharactersPerRow = (int)configPointer->Read("/Microboard/VT100CharPerRow", 80);
    elfConfiguration[MICROBOARD].vt100CharWidth = (int)configPointer->Read("/Microboard/VT100CharWidth", 10);
    elfConfiguration[MICROBOARD].vt52CharWidth = (int)configPointer->Read("/Microboard/VT52CharWidth", 9);

    configPointer->Read("/Microboard/Force_Uppercase", &elfConfiguration[MICROBOARD].forceUpperCase, true);

    defaultZoom.Printf("%2.2f", 1.0);
    conf[MICROBOARD].zoomVt_ = convertLocale(configPointer->Read("/Microboard/Vt_Zoom", defaultZoom));
    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[MICROBOARD].xScale_ = convertLocale(configPointer->Read("/Microboard/Window_Scale_Factor_X", defaultScale));
    
    conf[MICROBOARD].clock_ = convertLocale(configPointer->Read("/Microboard/Clock_Speed", defaultClock));
    
    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[MICROBOARD].ledTime_ = configPointer->Read("/Microboard/Led_Update_Frequency", defaultTimer);
    
    configPointer->Read("/Microboard/UseLoadLocation", &conf[MICROBOARD].useLoadLocation_, false);
    
    setVtType("Microboard", MICROBOARD, elfConfiguration[MICROBOARD].vtType, false);
    
    elfConfiguration[MICROBOARD].vtCharRom_ = configPointer->Read("/Microboard/Vt_Font_Rom_File", "vt100.bin");
    
    configPointer->Read("/Microboard/Enable_Auto_Boot", &elfConfiguration[MICROBOARD].autoBoot, true);
    elfConfiguration[MICROBOARD].autoBootType = (int)configPointer->Read("/Microboard/AutoBootType", 1l);
    if (elfConfiguration[MICROBOARD].autoBootType == 0)
        conf[MICROBOARD].bootAddress_ = 0x8000;
    else
        conf[MICROBOARD].bootAddress_ = 0;

    long value;
    conf[MICROBOARD].saveStartString_ = configPointer->Read("/Microboard/SaveStart", "0");
    if (!conf[MICROBOARD].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[MICROBOARD].saveStart_ = value;

    if (mode_.gui)
    {
        XRCCTRL(*this, "ScreenDumpFileMicroboard", wxComboBox)->SetValue(conf[MICROBOARD].screenDumpFile_);
        
        if (elfConfiguration[MICROBOARD].vtExternal)
            XRCCTRL(*this, "VTTypeMicroboard", wxChoice)->SetSelection(EXTERNAL_TERMINAL);
        else
            XRCCTRL(*this, "VTTypeMicroboard", wxChoice)->SetSelection(elfConfiguration[MICROBOARD].vtType);
        XRCCTRL(*this, "MicroboardForceUC", wxCheckBox)->SetValue(elfConfiguration[MICROBOARD].forceUpperCase);
        
        XRCCTRL(*this, "VTBaudRChoiceMicroboard", wxChoice)->SetSelection(elfConfiguration[MICROBOARD].baudR);
        XRCCTRL(*this, "VTBaudTChoiceMicroboard", wxChoice)->SetSelection(elfConfiguration[MICROBOARD].baudT);
        
        correctZoomAndValue(MICROBOARD, "Microboard", SET_SPIN);
        correctZoomVtAndValue(MICROBOARD, "Microboard", SET_SPIN);

        XRCCTRL(*this, "StretchDotMicroboard", wxCheckBox)->SetValue(conf[MICROBOARD].stretchDot_);
        
        if (clockTextCtrl[MICROBOARD] != NULL)
            clockTextCtrl[MICROBOARD]->ChangeValue(conf[MICROBOARD].clock_);
        
        XRCCTRL(*this, "UseLocationMicroboard", wxCheckBox)->SetValue(conf[MICROBOARD].useLoadLocation_);
        
        for (int drive=0; drive < 4; drive++)
            setUpdFloppyGui(drive, MICROBOARD);

        XRCCTRL(*this, "Card1ChoiceMicroboard", wxChoice)->SetSelection(conf[MICROBOARD].microboardType_[0]);
        XRCCTRL(*this, "Card2ChoiceMicroboard", wxChoice)->SetSelection(conf[MICROBOARD].microboardType_[1]);
        XRCCTRL(*this, "Card3ChoiceMicroboard", wxChoice)->SetSelection(conf[MICROBOARD].microboardType_[2]);
        XRCCTRL(*this, "Card4ChoiceMicroboard", wxChoice)->SetSelection(conf[MICROBOARD].microboardType_[3]);
        XRCCTRL(*this, "Card5ChoiceMicroboard", wxChoice)->SetSelection(conf[MICROBOARD].microboardType_[4]);

        XRCCTRL(*this, "TurboClockMicroboard", wxTextCtrl)->SetValue(conf[MICROBOARD].turboClock_);
        XRCCTRL(*this, "TurboMicroboard", wxCheckBox)->SetValue(conf[MICROBOARD].turbo_);
        turboGui("Microboard");
        XRCCTRL(*this, "AutoCasLoadMicroboard", wxCheckBox)->SetValue(conf[MICROBOARD].autoCassetteLoad_);

        XRCCTRL(*this, "WavFileMicroboard", wxTextCtrl)->SetValue(conf[MICROBOARD].wavFile_[0]);
        XRCCTRL(*this, "WavFile1Microboard", wxTextCtrl)->SetValue(conf[MICROBOARD].wavFile_[1]);
        setTapeGui();

        XRCCTRL(*this, "PioMicroboard", wxCheckBox)->SetValue(elfConfiguration[MICROBOARD].usePio);

        XRCCTRL(*this, "ShowAddressMicroboard", wxTextCtrl)->ChangeValue(conf[MICROBOARD].ledTime_);
        XRCCTRL(*this,"ShowAddressMicroboard", wxTextCtrl)->Enable(elfConfiguration[MICROBOARD].useElfControlWindows);
        XRCCTRL(*this, "AutoBootMicroboard", wxCheckBox)->SetValue(elfConfiguration[MICROBOARD].autoBoot);
        XRCCTRL(*this, "AutoBootTypeMicroboard", wxChoice)->SetSelection(elfConfiguration[MICROBOARD].autoBootType);

        XRCCTRL(*this,"AddressText1Microboard", wxStaticText)->Enable(elfConfiguration[MICROBOARD].useElfControlWindows);
        XRCCTRL(*this,"AddressText2Microboard", wxStaticText)->Enable(elfConfiguration[MICROBOARD].useElfControlWindows);
        
        XRCCTRL(*this,"ConfigTextMicroboard", wxStaticText)->SetLabel(configPointer->Read("/Microboard/ConfigName", "CDP18S600"));

        if (conf[MICROBOARD].saveStart_ != 0)
            XRCCTRL(*this, "SaveStartMicroboard", wxTextCtrl)->SetValue(conf[MICROBOARD].saveStartString_);
    }
}

void GuiCdp18s600::writeCdp18s600DirConfig()
{
    wxString cardNumberStr;
    
    writeConfigDir("/Dir/Microboard/Main", conf[MICROBOARD].mainDir_);
    writeConfigDir("/Dir/Microboard/Main_Rom_File21", conf[MICROBOARD].romDir_[U21ROM]);
    writeConfigDir("/Dir/Microboard/Main_Rom_File20", conf[MICROBOARD].romDir_[U20ROM]);
    writeConfigDir("/Dir/Microboard/Main_Rom_File19", conf[MICROBOARD].romDir_[U19ROM]);
    writeConfigDir("/Dir/Microboard/Main_Rom_File18", conf[MICROBOARD].romDir_[U18ROM]);
    writeConfigDir("/Dir/Microboard/Main_Rom_File18", conf[MICROBOARD].romDir_[U17ROM]);
    writeConfigDir("/Dir/Microboard/Vt_Font_Rom_File", elfConfiguration[MICROBOARD].vtCharRomDir_);
    writeConfigDir("/Dir/Microboard/Key_File", conf[MICROBOARD].keyFileDir_);
    writeConfigDir("/Dir/Microboard/Video_Dump_File", conf[MICROBOARD].screenDumpFileDir_);

    writeConfigDir("/Dir/Microboard/FDC0_File", floppyDir_[elfConfiguration[MICROBOARD].fdcType_][0]);
    writeConfigDir("/Dir/Microboard/FDC1_File", floppyDir_[elfConfiguration[MICROBOARD].fdcType_][1]);
    writeConfigDir("/Dir/Microboard/FDC2_File", floppyDir_[elfConfiguration[MICROBOARD].fdcType_][2]);
    writeConfigDir("/Dir/Microboard/FDC3_File", floppyDir_[elfConfiguration[MICROBOARD].fdcType_][3]);
    writeConfigDir("/Dir/Microboard/FDC0_File_Switched", floppyDirSwitched_[elfConfiguration[MICROBOARD].fdcType_][0]);
    writeConfigDir("/Dir/Microboard/FDC1_File_Switched", floppyDirSwitched_[elfConfiguration[MICROBOARD].fdcType_][1]);
    writeConfigDir("/Dir/Microboard/FDC2_File_Switched", floppyDirSwitched_[elfConfiguration[MICROBOARD].fdcType_][2]);
    writeConfigDir("/Dir/Microboard/FDC3_File_Switched", floppyDirSwitched_[elfConfiguration[MICROBOARD].fdcType_][3]);
    writeConfigDir("/Dir/Microboard/Wav_File", conf[MICROBOARD].wavFileDir_[0]);
    writeConfigDir("/Dir/Microboard/Wav_File1", conf[MICROBOARD].wavFileDir_[1]);
    writeConfigDir("/Dir/Microboard/Font_Rom_File", conf[MICROBOARD].charRomDir_);
    writeConfigDir("/Dir/Microboard/Print_File", conf[MICROBOARD].printFileDir_);

    for (int card=0; card<conf[MICROBOARD].microboardMaxCards_; card++)
    {
        cardNumberStr.Printf("%d",card);
        writeConfigDir("/Dir/Microboard/Card" + cardNumberStr+"RamFile0", microMemConf[card].ramDir_);
        writeConfigDir("/Dir/Microboard/Card" + cardNumberStr+"RomFile0", microMemConf[card].romDir_[MEM_SECTION0]);
        writeConfigDir("/Dir/Microboard/Card" + cardNumberStr+"RomFile1", microMemConf[card].romDir_[MEM_SECTION1]);
        writeConfigDir("/Dir/Microboard/Card" + cardNumberStr+"RomFile2", microMemConf[card].romDir_[MEM_SECTION2]);
        writeConfigDir("/Dir/Microboard/Card" + cardNumberStr+"RomFile3", microMemConf[card].romDir_[MEM_SECTION3]);
    }
}

void GuiCdp18s600::writeCdp18s600Config()
{
    wxString cardNumberStr;
    
    writeElfPortConfig(MICROBOARD, "Microboard");
    
    configPointer->Write("/Microboard/Main_Rom_File21", conf[MICROBOARD].rom_[U21ROM]);
    configPointer->Write("/Microboard/Main_Rom_File20", conf[MICROBOARD].rom_[U20ROM]);
    configPointer->Write("/Microboard/Main_Rom_File19", conf[MICROBOARD].rom_[U19ROM]);
    configPointer->Write("/Microboard/Main_Rom_File18", conf[MICROBOARD].rom_[U18ROM]);
    configPointer->Write("/Microboard/Main_Rom_File17", conf[MICROBOARD].rom_[U17ROM]);
    configPointer->Write("/Microboard/Vt_Font_Rom_File", elfConfiguration[MICROBOARD].vtCharRom_);
    configPointer->Write("/Microboard/Video_Dump_File", conf[MICROBOARD].screenDumpFile_);
    configPointer->Write("/Microboard/VtSerialPortChoice", elfConfiguration[MICROBOARD].serialPort_);
    
    configPointer->Write("/Microboard/SerialLog", elfConfiguration[MICROBOARD].serialLog);
    configPointer->Write("/Microboard/Bell_Frequency", elfConfiguration[MICROBOARD].bellFrequency_);
    configPointer->Write("/Microboard/VT_Type", elfConfiguration[MICROBOARD].vtType);
    
    long value = elfConfiguration[MICROBOARD].vt52SetUpFeature_.to_ulong();
    configPointer->Write("/Microboard/VT52Setup", value);
    value = elfConfiguration[MICROBOARD].vt100SetUpFeature_.to_ulong();
    configPointer->Write("/Microboard/VT100Setup", value);
    value = elfConfiguration[MICROBOARD].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("/Microboard/VTExternalSetup", value);
    configPointer->Write("/Microboard/Uart", elfConfiguration[MICROBOARD].useUart);
    configPointer->Write("/Microboard/VT100CharPerRow", elfConfiguration[MICROBOARD].vtCharactersPerRow);
    configPointer->Write("/Microboard/VT100CharWidth", elfConfiguration[MICROBOARD].vt100CharWidth);
    configPointer->Write("/Microboard/VT52CharWidth", elfConfiguration[MICROBOARD].vt52CharWidth);

    configPointer->Write("/Microboard/Vt_Baud_Receive", elfConfiguration[MICROBOARD].baudR);
    configPointer->Write("/Microboard/Vt_Baud_Transmit", elfConfiguration[MICROBOARD].baudT);
    configPointer->Write("/Microboard/Vt_Zoom", conf[MICROBOARD].zoomVt_);
    configPointer->Write("/Microboard/Force_Uppercase", elfConfiguration[MICROBOARD].forceUpperCase);
    configPointer->Write("/Microboard/Enable_Vt_Stretch_Dot", conf[MICROBOARD].stretchDot_);
    configPointer->Write("/Microboard/Enable_Vt_External", elfConfiguration[MICROBOARD].vtExternal);
    configPointer->Write("/Microboard/Led_Update_Frequency", conf[MICROBOARD].ledTime_);
    configPointer->Write("/Microboard/UseLoadLocation", conf[MICROBOARD].useLoadLocation_);
    configPointer->Write("/Microboard/SaveStart", conf[MICROBOARD].saveStartString_);

    configPointer->Write("/Microboard/MicroChipLocationOneSocket", conf[MICROBOARD].microChipLocation_[ONE_SOCKET]);
    configPointer->Write("/Microboard/MicroChipTypeOneSocket", conf[MICROBOARD].microChipType_[ONE_SOCKET]);
    configPointer->Write("/Microboard/MicroChipTypeFourSocket", conf[MICROBOARD].microChipType_[FOUR_SOCKET]);

    configPointer->Write("/Microboard/FDC0_File", floppy_[elfConfiguration[MICROBOARD].fdcType_][0]);
    configPointer->Write("/Microboard/FDC1_File", floppy_[elfConfiguration[MICROBOARD].fdcType_][1]);
    configPointer->Write("/Microboard/FDC2_File", floppy_[elfConfiguration[MICROBOARD].fdcType_][2]);
    configPointer->Write("/Microboard/FDC3_File", floppy_[elfConfiguration[MICROBOARD].fdcType_][3]);
    configPointer->Write("/Microboard/DirectoryMode_0", directoryMode_[elfConfiguration[MICROBOARD].fdcType_][0]);
    configPointer->Write("/Microboard/DirectoryMode_1", directoryMode_[elfConfiguration[MICROBOARD].fdcType_][1]);
    configPointer->Write("/Microboard/DirectoryMode_2", directoryMode_[elfConfiguration[MICROBOARD].fdcType_][2]);
    configPointer->Write("/Microboard/DirectoryMode_3", directoryMode_[elfConfiguration[MICROBOARD].fdcType_][3]);

    configPointer->Write("/Microboard/MicroChipLocationFourSocket", conf[MICROBOARD].microChipLocation_[FOUR_SOCKET]);
    configPointer->Write("/Microboard/MicroChipLocationRom1Socket", conf[MICROBOARD].microChipLocation_[FOUR_SOCKET_ROM1]);
    configPointer->Write("/Microboard/MicroChipLocationRom2Socket", conf[MICROBOARD].microChipLocation_[FOUR_SOCKET_ROM2]);

    if (mode_.gui)
        configPointer->Write("/Microboard/ConfigName", XRCCTRL(*this,"ConfigTextMicroboard", wxStaticText)->GetLabel());

    configPointer->Write("/Microboard/MicroBlockOneSocket", conf[MICROBOARD].microChipBlock_[ONE_SOCKET]);
    configPointer->Write("/Microboard/MicroBlockFourSocket", conf[MICROBOARD].microChipBlock_[FOUR_SOCKET]);
    configPointer->Write("/Microboard/MicroChipMemoryU21", conf[MICROBOARD].microChipMemory_[U21ROM]);
    configPointer->Write("/Microboard/MicroChiDisableU21", conf[MICROBOARD].microChipDisable_[U21ROM]);
    
    configPointer->Write("/Microboard/MicroChipMemoryU20", conf[MICROBOARD].microChipMemory_[U20ROM]);
    configPointer->Write("/Microboard/MicroChiDisableU20", conf[MICROBOARD].microChipDisable_[U20ROM]);
    
    configPointer->Write("/Microboard/MicroChipMemoryU19", conf[MICROBOARD].microChipMemory_[U19ROM]);
    configPointer->Write("/Microboard/MicroChiDisableU19", conf[MICROBOARD].microChipDisable_[U19ROM]);
    
    configPointer->Write("/Microboard/MicroChipMemoryU18", conf[MICROBOARD].microChipMemory_[U18ROM]);
    configPointer->Write("/Microboard/MicroChiDisableU18", conf[MICROBOARD].microChipDisable_[U18ROM]);
    
    configPointer->Write("/Microboard/MicroChipMemoryU17", conf[MICROBOARD].microChipMemory_[U17ROM]);
    configPointer->Write("/Microboard/MicroChiDisableU17", conf[MICROBOARD].microChipDisable_[U17ROM]);

    for (int card=0; card<24; card++)
    {
        cardNumberStr.Printf("%d",card);
        configPointer->Write("/Microboard/MicroboardType"+cardNumberStr, cardBoardNumber[conf[MICROBOARD].microboardType_[card]]);
    }
    
    for (int card=0; card<conf[MICROBOARD].microboardMaxCards_; card++)
    {
        cardNumberStr.Printf("%d",card);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemType0", microMemConf[card].memType[MEM_SECTION0]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemType1", microMemConf[card].memType[MEM_SECTION1]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemType2", microMemConf[card].memType[MEM_SECTION2]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemType3", microMemConf[card].memType[MEM_SECTION3]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemLocation0", microMemConf[card].memLocation_[0]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemLocation1", microMemConf[card].memLocation_[1]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemLocation2", microMemConf[card].memLocation_[2]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"ChipBlockRam", microMemConf[card].chipBlockRam_);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"ChipBlockRom0", microMemConf[card].chipBlockRom_[0]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"ChipBlockRom1", microMemConf[card].chipBlockRom_[1]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"Socket0Size", microMemConf[card].socketSize_[0]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"Socket1Size", microMemConf[card].socketSize_[1]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemInhibitBlock00", microMemConf[card].inhibitBlock_[0][0]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemInhibitBlock01", microMemConf[card].inhibitBlock_[0][1]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemInhibitBlock02", microMemConf[card].inhibitBlock_[0][2]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemInhibitBlock03", microMemConf[card].inhibitBlock_[0][3]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemInhibitBlock10", microMemConf[card].inhibitBlock_[1][0]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemInhibitBlock11", microMemConf[card].inhibitBlock_[1][1]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemInhibitBlock12", microMemConf[card].inhibitBlock_[1][2]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemInhibitBlock13", microMemConf[card].inhibitBlock_[1][3]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemInhibit64", microMemConf[card].inhibit64_);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemInhibit32Low", microMemConf[card].inhibit32Low_);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"MemInhibit32High", microMemConf[card].inhibit32High_);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"RamFile0", microMemConf[card].ram_);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"RomFile0", microMemConf[card].rom_[MEM_SECTION0]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"RomFile1", microMemConf[card].rom_[MEM_SECTION1]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"RomFile2", microMemConf[card].rom_[MEM_SECTION2]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"RomFile3", microMemConf[card].rom_[MEM_SECTION3]);
        configPointer->Write("/Microboard/Card" + cardNumberStr+"DisableMemory", microMemConf[card].disableCardMemory_);
    }
    configPointer->Write("/Microboard/Wav_File", conf[MICROBOARD].wavFile_[0]);
    configPointer->Write("/Microboard/Wav_File1", conf[MICROBOARD].wavFile_[1]);
    configPointer->Write("/Microboard/Turbo_Clock_Speed", conf[MICROBOARD].turboClock_);
    configPointer->Write("/Microboard/Enable_Turbo_Cassette", conf[MICROBOARD].turbo_);
    configPointer->Write("/Microboard/Enable_Auto_Cassette", conf[MICROBOARD].autoCassetteLoad_);
    configPointer->Write("/Microboard/Enable_Real_Cassette", conf[MICROBOARD].realCassetteLoad_);

    configPointer->Write("/Microboard/Zoom", conf[MICROBOARD].zoom_);
    configPointer->Write("/Microboard/Volume", conf[MICROBOARD].volume_);
    configPointer->Write("/Microboard/PageMemSize", elfConfiguration[MICROBOARD].pageMemSize);
    configPointer->Write("/Microboard/Font_Rom_File", conf[MICROBOARD].charRom_);
    configPointer->Write("/Microboard/V1870InterruptMode", elfConfiguration[MICROBOARD].v1870InterruptMode);
    configPointer->Write("/Microboard/V1870VideoMode", elfConfiguration[MICROBOARD].v1870VideoMode);
    configPointer->Write("/Microboard/Print_Mode", conf[MICROBOARD].printMode_);
    configPointer->Write("/Microboard/Print_File", conf[MICROBOARD].printFile_);

	configPointer->Write("/Microboard/KeyboardEF", elfConfiguration[MICROBOARD].elfPortConf.keyboardEf);
	configPointer->Write("/Microboard/KeyboardType", elfConfiguration[MICROBOARD].keyboardType);

    configPointer->Write("/Microboard/Upd765Group", elfConfiguration[MICROBOARD].upd765Group);
    configPointer->Write("/Microboard/PrinterGroup", elfConfiguration[MICROBOARD].printerGroup);
    configPointer->Write("/Microboard/UartGroup", elfConfiguration[MICROBOARD].uartGroup);
    configPointer->Write("/Microboard/UartOut", elfConfiguration[MICROBOARD].elfPortConf.uartOut);
    configPointer->Write("/Microboard/UartControl", elfConfiguration[MICROBOARD].elfPortConf.uartControl);
    configPointer->Write("/Microboard/Pio_Windows", elfConfiguration[MICROBOARD].usePio);
    configPointer->Write("/Microboard/Pio1_Windows", elfConfiguration[MICROBOARD].usePioWindow1Cdp18s660);
    configPointer->Write("/Microboard/Pio2_Windows", elfConfiguration[MICROBOARD].usePioWindow2Cdp18s660);
    configPointer->Write("/Microboard/V1870Group", elfConfiguration[MICROBOARD].v1870Group);
    configPointer->Write("/Microboard/Pio1Group", elfConfiguration[MICROBOARD].cdp18s660Group1);
    configPointer->Write("/Microboard/Pio2Group", elfConfiguration[MICROBOARD].cdp18s660Group2);

    configPointer->Write("/Microboard/Enable_Auto_Boot", elfConfiguration[MICROBOARD].autoBoot);
    configPointer->Write("/Microboard/AutoBootType", elfConfiguration[MICROBOARD].autoBootType);

    configPointer->Write("/Microboard/Clock_Speed", conf[MICROBOARD].clock_);
}

void GuiCdp18s600::readCdp18s600WindowConfig()
{
    conf[MICROBOARD].vtX_ = (int)configPointer->Read("/Microboard/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[MICROBOARD].vtY_ = (int)configPointer->Read("/Microboard/Window_Position_Vt_Y", mainWindowY_);
    conf[MICROBOARD].vtUart2X_ = (int)configPointer->Read("/Microboard/Window_Position_VtUart2_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[MICROBOARD].vtUart2Y_ = (int)configPointer->Read("/Microboard/Window_Position_VtUart2_Y", mainWindowY_ + 530);
    conf[MICROBOARD].mainX_ = (int)configPointer->Read("/Microboard/Window_Position_X", mainWindowX_);
    conf[MICROBOARD].mainY_ = (int)configPointer->Read("/Microboard/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    conf[MICROBOARD].secondFrameX_ = (int)configPointer->Read("/Microboard/Window_Position_SecondFrame_X", mainWindowX_ + 310 + windowInfo.xBorder2);
    conf[MICROBOARD].secondFrameY_ = (int)configPointer->Read("/Microboard/Window_Position_SecondFrame_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    conf[MICROBOARD].thirdFrameY_ = (int)configPointer->Read("/Microboard/Window_Position_ThirdFrame_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
#if defined (__WXMAC__) || (__linux__)
    conf[MICROBOARD].thirdFrameX_ = (int)configPointer->Read("/Microboard/Window_Position_ThirdFrame_X", mainWindowX_ + 620 + windowInfo.xBorder2);
    conf[MICROBOARD].fourthFrameX_ = (int)configPointer->Read("/Microboard/Window_Position_FourthFrame_X", mainWindowX_ + 930 + windowInfo.xBorder2);
#else
    conf[MICROBOARD].thirdFrameX_ = (int)configPointer->Read("/Microboard/Window_Position_ThirdFrame_X", mainWindowX_ + 639 + windowInfo.xBorder2);
    conf[MICROBOARD].fourthFrameX_ = (int)configPointer->Read("/Microboard/Window_Position_FourthFrame_X", mainWindowX_ + 968 + windowInfo.xBorder2);
#endif

    conf[MICROBOARD].fourthFrameY_ = (int)configPointer->Read("/Microboard/Window_Position_FourthFrame_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    conf[MICROBOARD].v1870X_ = (int)configPointer->Read("/Microboard/Window_Position_v1870_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[MICROBOARD].v1870Y_ = (int)configPointer->Read("/Microboard/Window_Position_v1870_Y", mainWindowY_);
}

void GuiCdp18s600::writeCdp18s600WindowConfig()
{
    if (conf[MICROBOARD].vtX_ > 0)
        configPointer->Write("/Microboard/Window_Position_Vt_X", conf[MICROBOARD].vtX_);
    if (conf[MICROBOARD].vtY_ > 0)
        configPointer->Write("/Microboard/Window_Position_Vt_Y", conf[MICROBOARD].vtY_);
    if (conf[MICROBOARD].vtUart2X_ > 0)
        configPointer->Write("/Microboard/Window_Position_VtUart2_X", conf[MICROBOARD].vtUart2X_);
    if (conf[MICROBOARD].vtUart2Y_ > 0)
        configPointer->Write("/Microboard/Window_Position_VtUart2_Y", conf[MICROBOARD].vtUart2Y_);
    if (conf[MICROBOARD].mainX_ > 0)
        configPointer->Write("/Microboard/Window_Position_X", conf[MICROBOARD].mainX_);
    if (conf[MICROBOARD].mainY_ > 0)
        configPointer->Write("/Microboard/Window_Position_Y", conf[MICROBOARD].mainY_);
    if (conf[MICROBOARD].secondFrameX_ > 0)
        configPointer->Write("/Microboard/Window_Position_SecondFrame_X", conf[MICROBOARD].secondFrameX_);
    if (conf[MICROBOARD].secondFrameY_ > 0)
        configPointer->Write("/Microboard/Window_Position_SecondFrame_Y", conf[MICROBOARD].secondFrameY_);
    if (conf[MICROBOARD].thirdFrameX_ > 0)
        configPointer->Write("/Microboard/Window_Position_ThirdFrame_X", conf[MICROBOARD].thirdFrameX_);
    if (conf[MICROBOARD].thirdFrameY_ > 0)
        configPointer->Write("/Microboard/Window_Position_ThirdFrame_Y", conf[MICROBOARD].thirdFrameY_);
    if (conf[MICROBOARD].fourthFrameX_ > 0)
        configPointer->Write("/Microboard/Window_Position_FourthFrame_X", conf[MICROBOARD].fourthFrameX_);
    if (conf[MICROBOARD].fourthFrameY_ > 0)
        configPointer->Write("/Microboard/Window_Position_FourthFrame_Y", conf[MICROBOARD].fourthFrameY_);
    if (conf[MICROBOARD].v1870X_ > 0)
        configPointer->Write("/Microboard/Window_Position_v1870_X", conf[MICROBOARD].v1870X_);
    if (conf[MICROBOARD].v1870Y_ > 0)
        configPointer->Write("/Microboard/Window_Position_v1870_Y", conf[MICROBOARD].v1870Y_);
}

void GuiCdp18s600::onCdp18s600BaudT(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].baudT = event.GetSelection();
    elfConfiguration[selectedComputer_].baudR = event.GetSelection();
    XRCCTRL(*this, "VTBaudRChoice"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(elfConfiguration[selectedComputer_].baudR);
}

void GuiCdp18s600::onCdp18s600ForceUpperCase(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].forceUpperCase = event.IsChecked();
    if (runningComputer_ == selectedComputer_)
    {
        p_Computer->setForceUpperCase(event.IsChecked());
    }
}

int GuiCdp18s600::convert604BChipType(int type)
{
    if (type == 0)
        type = 1;
    
    type = 1 << (type-1);
    return type;
}

void GuiCdp18s600::setRamlabel(int romNumber, wxString romString, wxString cdpTypeStr, wxString label)
{
    wxString buttonString = "RomButton" + romString + cdpTypeStr;
    wxString comboString = "MainRom" + romString + cdpTypeStr;
    wxString typeString = "ROM ";
    
    switch (conf[selectedComputer_].microChipMemory_[romNumber])
    {
        case MICRO_RAM:
            typeString = "RAM ";
        break;
            
        case MICRO_CPURAM:
            typeString = "CRAM ";
        break;
    }
    XRCCTRL(*this, buttonString, wxButton)->SetLabel(typeString + label);

    if (conf[selectedComputer_].microChipDisable_[romNumber])
    {
        XRCCTRL(*this, buttonString, wxButton)->Disable();
        XRCCTRL(*this, comboString, wxComboBox)->Disable();
    }
    else
    {
        XRCCTRL(*this, buttonString, wxButton)->Enable();
        XRCCTRL(*this, comboString, wxComboBox)->Enable();
    }
}

void GuiCdp18s600::onMicroboardCard1Setup(wxCommandEvent&WXUNUSED(event))
{
    MicroboardSetupDialog MicroboardSetupDialog(this, conf[selectedComputer_], elfConfiguration[selectedComputer_]);
    MicroboardSetupDialog.ShowModal();

    setCardType();
}

void GuiCdp18s600::onMicroboardCard2Setup(wxCommandEvent&WXUNUSED(event))
{
    MicroboardCardSetupDialog MicroboardCardSetupDialog(this, conf[selectedComputer_], elfConfiguration[selectedComputer_], microMemConf[0], 1);
    MicroboardCardSetupDialog.ShowModal();

    setCardType();
}

void GuiCdp18s600::onMicroboardCard3Setup(wxCommandEvent&WXUNUSED(event))
{
    MicroboardCardSetupDialog MicroboardCardSetupDialog(this, conf[selectedComputer_], elfConfiguration[selectedComputer_], microMemConf[1], 2);
    MicroboardCardSetupDialog.ShowModal();

    setCardType();
}

void GuiCdp18s600::onMicroboardCard4Setup(wxCommandEvent&WXUNUSED(event))
{
    MicroboardCardSetupDialog MicroboardCardSetupDialog(this, conf[selectedComputer_], elfConfiguration[selectedComputer_], microMemConf[2], 3);
    MicroboardCardSetupDialog.ShowModal();

    setCardType();
}

void GuiCdp18s600::onMicroboardCardSetup(wxCommandEvent&WXUNUSED(event))
{
    MicroboardAdditionalCardSetupDialog MicroboardAdditionalCardSetupDialog(this, conf[selectedComputer_], elfConfiguration[selectedComputer_]);
    MicroboardAdditionalCardSetupDialog.ShowModal();

    XRCCTRL(*this, "Card5ChoiceMicroboard", wxChoice)->SetSelection(conf[MICROBOARD].microboardType_[4]);

    setCardType();
}

void GuiCdp18s600::onMicroboardType1(wxCommandEvent&event)
{
    int selection = event.GetSelection();
    
    if (conf[selectedComputer_].microboardType_[0] != selection)
    {
        conf[selectedComputer_].microboardType_[0] = selection;
        clearConfigName();
        setCardType();
    }
}

void GuiCdp18s600::onMicroboardType2(wxCommandEvent&event)
{
    int selection = event.GetSelection();
    
    if (conf[selectedComputer_].microboardType_[1] != selection)
    {
        conf[selectedComputer_].microboardType_[1] = selection;
        clearConfigName();
        setCardType();
    }
}

void GuiCdp18s600::onMicroboardType3(wxCommandEvent&event)
{
    int selection = event.GetSelection();
    
    if (conf[selectedComputer_].microboardType_[2] != selection)
    {
        conf[selectedComputer_].microboardType_[2] = selection;
        clearConfigName();
        setCardType();
    }
}

void GuiCdp18s600::onMicroboardType4(wxCommandEvent&event)
{
    int selection = event.GetSelection();
    
    if (conf[selectedComputer_].microboardType_[3] != selection)
    {
        conf[selectedComputer_].microboardType_[3] = selection;
        clearConfigName();
        setCardType();
    }
}

void GuiCdp18s600::onMicroboardType5(wxCommandEvent&event)
{
    int selection = event.GetSelection();
    
    if (conf[selectedComputer_].microboardType_[4] != selection)
    {
        conf[selectedComputer_].microboardType_[4] = selection;
        clearConfigName();
        setCardType();
    }
}

void GuiCdp18s600::clearConfigName()
{
    XRCCTRL(*this, "ConfigTextMicroboard", wxStaticText)->SetLabel("");
}

void GuiCdp18s600::setCardMax(Conf* config)
{
    config->microboardMaxCards_ = 4;
    for (int card=4; card<24; card++)
    {
        if (config->microboardType_[card] != CARD_EMPTY)
            config->microboardMaxCards_ = card;
    }
    
    microMemConf.resize(config->microboardMaxCards_);
}

void GuiCdp18s600::setCardType()
{
    wxString cardStr;

    checkAllBoardTypes(&conf[MICROBOARD], &elfConfiguration[MICROBOARD]);
    
    if (mode_.gui)
    {
        XRCCTRL(*this, "PioMicroboard", wxCheckBox)->Enable(true);
        XRCCTRL(*this, "VTTypeMicroboard", wxChoice)->Enable(true);
        XRCCTRL(*this, "ZoomTextMicroboard", wxStaticText)->Enable(false);
        XRCCTRL(*this, "ZoomValueMicroboard", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "ZoomSpinMicroboard", wxSpinButton)->Enable(false);

        XRCCTRL(*this, "Card2ChoiceMicroboard", wxChoice)->Enable(true);
        XRCCTRL(*this, "Card3ChoiceMicroboard", wxChoice)->Enable(true);
        XRCCTRL(*this, "Card4ChoiceMicroboard", wxChoice)->Enable(true);
        XRCCTRL(*this, "Card5ChoiceMicroboard", wxChoice)->Enable(true);
        XRCCTRL(*this, "Card5Microboard", wxButton)->Enable(true);

        switch (conf[MICROBOARD].microboardType_[0])
        {
            case MICROBOARD_CDP18S604B:
            case MICROBOARD_CDP18S609:
                XRCCTRL(*this, "VTTypeMicroboard", wxChoice)->Enable(false);
                XRCCTRL(*this, "VTTypeMicroboard", wxChoice)->SetSelection(VTNONE);
            break;
                
            case MICROBOARD_CDP18S605:
            case MICROBOARD_CDP18S610:
                XRCCTRL(*this, "PioMicroboard", wxCheckBox)->Enable(false);
            break;

            case RCASBC:
//                XRCCTRL(*this, "Card2ChoiceMicroboard", wxChoice)->Enable(false);
//                XRCCTRL(*this, "Card3ChoiceMicroboard", wxChoice)->Enable(false);
//                XRCCTRL(*this, "Card4ChoiceMicroboard", wxChoice)->Enable(false);
//                XRCCTRL(*this, "Card5ChoiceMicroboard", wxChoice)->Enable(false);
//                XRCCTRL(*this, "Card5Microboard", wxButton)->Enable(false);
            break;
        }
    
        setButtonColor("1", (conf[MICROBOARD].errorMemoryOverlapp_[0] != "" && conf[MICROBOARD].errorMemoryOverlapp_[0].Right(4) != "card") || conf[MICROBOARD].errorDoubleBoard_[0] != "");

        for (int card=1; card<=4; card++)
        {
            cardStr.Printf("%d", card+1);
//           if (conf[selectedComputer_].microboardType_[0] != RCASBC || card < 4)
            if (card < 4)
                XRCCTRL(*this, "Card"+cardStr + "Microboard", wxButton)->Enable(true);
            
            setButtonColor(cardStr, (conf[MICROBOARD].errorMemoryOverlapp_[card] != ""  && conf[MICROBOARD].errorMemoryOverlapp_[card].Right(4) != "card") || conf[MICROBOARD].errorDoubleBoard_[card] != "");

            switch (conf[MICROBOARD].microboardType_[card])
            {
                case CARD_EMPTY:
                    if (card != 4)
                        XRCCTRL(*this, "Card"+cardStr + "Microboard", wxButton)->Enable(false);
                break;
                    
                case CARD_CDP18S641:
                    XRCCTRL(*this, "VTTypeMicroboard", wxChoice)->Enable(true);
                    if (elfConfiguration[MICROBOARD].vtExternal)
                        XRCCTRL(*this, "VTTypeMicroboard", wxChoice)->SetSelection(EXTERNAL_TERMINAL);
                    else
                        XRCCTRL(*this, "VTTypeMicroboard", wxChoice)->SetSelection(elfConfiguration[MICROBOARD].vtType);
                break;

                case CARD_CDP18S661B:
                    XRCCTRL(*this, "ZoomTextMicroboard", wxStaticText)->Enable(true);
                    XRCCTRL(*this, "ZoomValueMicroboard", wxTextCtrl)->Enable(true);
                    XRCCTRL(*this, "ZoomSpinMicroboard", wxSpinButton)->Enable(true);
                break;

                case CARD_CDP18S661V1:
                case CARD_CDP18S661V3:
                    XRCCTRL(*this, "ZoomTextMicroboard", wxStaticText)->Enable(true);
                    XRCCTRL(*this, "ZoomValueMicroboard", wxTextCtrl)->Enable(true);
                    XRCCTRL(*this, "ZoomSpinMicroboard", wxSpinButton)->Enable(true);
                break;
            }
        }
        
        for (int drive=0; drive < 4; drive++)
            setUpdFloppyGui(drive, MICROBOARD);
        
        setTapeGui();
        setVtType(computerInfo[MICROBOARD].gui, MICROBOARD, elfConfiguration[MICROBOARD].vtType, false);
    }
}

void GuiCdp18s600::checkAllBoardTypes(Conf* config, ElfConfiguration* elfConfig)
{
    checkAllBoardTypes(config, elfConfig, microMemConf[0], -1);
}

void GuiCdp18s600::checkAllBoardTypes(Conf* config, ElfConfiguration* elfConfig, MicroMemoryConf microMemoryConfCardx, int cardx)
{
    long block;
    Word socketSize;
    wxString cardStr;
    int chipType;
    
    int vtType = elfConfig->vtType;
    for (int i=0; i<256; i++)
    {
        config->memoryMapType_[i] = -1;
        config->memoryMapCard_[i] = -1;
    }

    elfConfig->useUart = false;
    config->errorDoubleBoard_[0] = "";
    config->errorMemoryOverlapp_[0] = "";
    config->microboardTypeStr_[0] = microBoardStr[config->microboardType_[0]];
    switch (config->microboardType_[0])
    {
        case MICROBOARD_CDP18S600:
            if (config->microChipType_[ONE_SOCKET]>2)
                config->microChipType_[ONE_SOCKET] = 0;
            
            if (config->microChipType_[FOUR_SOCKET]>2)
                config->microChipType_[FOUR_SOCKET] = 0;

            elfConfig->useUart = true;
            elfConfig->elfPortConf.uartOut = 2;
            elfConfig->elfPortConf.uartControl = 3;
            setMemoryMapCDP18S600(config, 0, config->microboardType_[0]);
        break;
            
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S606:
            if (config->microChipType_[FOUR_SOCKET]>1)
                config->microChipType_[FOUR_SOCKET] = 0;

            elfConfig->useUart = false;
            setMemoryMapCDP18S601(config, 0, config->microboardType_[0]);
        break;

        case MICROBOARD_CDP18S602:
        case MICROBOARD_CDP18S605:
        case MICROBOARD_CDP18S607:
        case MICROBOARD_CDP18S610:
            if (config->microChipType_[FOUR_SOCKET]>1)
                config->microChipType_[FOUR_SOCKET] = 0;
            
            if (!config->microChipBlock_[ONE_SOCKET].ToLong(&block))
            {
                config->microChipBlock_[ONE_SOCKET] = "0";
                block = 0;
            }
            while ((block*0x800) > 0xffff)
            {
                block = (long)(block / 2);
                config->microChipBlock_[ONE_SOCKET].Printf("%d", (int)block);
            }

            chipType = (config->microChipType_[FOUR_SOCKET]+1)*2;
            if (!config->microChipBlock_[FOUR_SOCKET].ToLong(&block))
            {
                config->microChipBlock_[FOUR_SOCKET] = "0";
                block = 0;
            }
            while ((block*chipType*0x400) > 0xffff || (block*chipType*0x400+chipType*0x400-1) > 0xffff)
            {
                block = (long)(block / 2);
                config->microChipBlock_[FOUR_SOCKET].Printf("%d", (int)block);
            }

            elfConfig->useUart = true;
            elfConfig->elfPortConf.uartOut = 2;
            elfConfig->elfPortConf.uartControl = 3;
            setMemoryMapCDP18S602(config, 0, config->microboardType_[0]);
        break;
            
        case MICROBOARD_CDP18S603:
        case MICROBOARD_CDP18S603A:
        case MICROBOARD_CDP18S608:
            if (config->microChipType_[FOUR_SOCKET]>1)
                config->microChipType_[FOUR_SOCKET] = 0;

            elfConfig->useUart = false;
            setMemoryMapCDP18S603a(config, 0, config->microboardType_[0]);
        break;
            
        case MICROBOARD_CDP18S604B:
        case MICROBOARD_CDP18S609:
            if (!config->microChipBlock_[ONE_SOCKET].ToLong(&block))
            {
                config->microChipBlock_[ONE_SOCKET] = "0";
                block = 0;
            }
            while ((block*0x400) > 0xffff)
            {
                block = (long)(block / 2);
                config->microChipBlock_[ONE_SOCKET].Printf("%d", (int)block);
            }

            chipType = convert604BChipType(config->microChipType_[FOUR_SOCKET]);
            if (!config->microChipBlock_[FOUR_SOCKET].ToLong(&block))
            {
                config->microChipBlock_[FOUR_SOCKET] = "0";
                block = 0;
            }
            while ((block*chipType*0x400) > 0xffff || (block*chipType*0x400+chipType*0x400-1) > 0xffff)
            {
                block = (long)(block / 2);
                config->microChipBlock_[FOUR_SOCKET].Printf("%d", (int)block);
            }

            elfConfig->vtType = 0;
            elfConfig->useUart = false;
            setMemoryMapCDP18S604b(config, 0, config->microboardType_[0]);
        break;
            
        case RCASBC:
            elfConfig->useUart = true;
            setMemoryMapRcasbc(config, 0, config->microboardType_[0]);
        break;
    }

    elfConfig->useElfControlWindows = false;
    elfConfig->useUpd765 = false;
    elfConfig->useTape = false;
    elfConfig->usev1870 = false;
    elfConfig->useCdp18s660 = false;
    config->printerOn_ = false;

    wxString controlWindowCardStr="", uartCardStr="1", printerCardStr="", upd765CardStr="", tapeCardStr="", v1870CardStr="", pioCardStr="";
    int controlWindowCard=-1, uartCard=0, printerCard=-1, upd765Card=-1, tapeCard=-1, v1870Card=-1, pioCard=-1;

    MicroMemoryConf microMemConfig;
    
    for (int card=1; card<=config->microboardMaxCards_; card++)
    {
        config->microboardTypeStr_[card] = microBoardStr[config->microboardType_[card]+MICROBOARD_LAST];
        
        microMemConfig = getMicroMemConf(card);
        
        config->errorDoubleBoard_[card] = "";
        config->errorMemoryOverlapp_[card] = "";
        cardStr.Printf("%d", card+1);

        switch (config->microboardType_[card])
        {
            case CARD_CDP18S620:
                if (microMemConfig.memLocation_[0] > 15)
                    microMemConfig.memLocation_[0] = 0;

                if (card == cardx)
                    setMemoryMapCDP18S620(config, microMemoryConfCardx, cardx, config->microboardType_[card]+MICROBOARD_LAST);
                else
                    setMemoryMapCDP18S620(config, microMemConfig, card, config->microboardType_[card]+MICROBOARD_LAST);
            break;
                
            case CARD_CDP18S621:
                if (microMemConfig.memLocation_[0] > 3)
                    microMemConfig.memLocation_[0] = 0;
                microMemConfig.socketSize_[0] = 2;

                if (card == cardx)
                    setMemoryMapCDP18S621(config, microMemoryConfCardx, cardx, config->microboardType_[card]+MICROBOARD_LAST);
                else
                    setMemoryMapCDP18S621(config, microMemConfig, card, config->microboardType_[card]+MICROBOARD_LAST);
            break;
                
            case CARD_CDP18S623A:
                if (microMemConfig.memLocation_[0] > 7)
                    microMemConfig.memLocation_[0] = 0;
                    
                if (card == cardx)
                    setMemoryMapCDP18S623a(config, microMemoryConfCardx, cardx, config->microboardType_[card]+MICROBOARD_LAST);
                else
                    setMemoryMapCDP18S623a(config, microMemConfig, card, config->microboardType_[card]+MICROBOARD_LAST);
            break;
                
            case CARD_CDP18S625:
                if (!microMemConfig.chipBlockRom_[0].ToLong(&block))
                {
                    microMemConfig.chipBlockRom_[0] = "0";
                    block = 0;
                }
                socketSize = (1 << (microMemConfig.socketSize_[0]))*0x400;
                while ((block*socketSize*4) > 0xffff || (block*socketSize*4+(socketSize*4)-1) > 0xffff)
                {
                    block = (long)(block / 2);
                    microMemConfig.chipBlockRom_[0].Printf("%d", (int)block);
                }
                
                if (!microMemConfig.chipBlockRom_[1].ToLong(&block))
                {
                    microMemConfig.chipBlockRom_[1] = "0";
                    block = 0;
                }
                socketSize = (1 << (microMemConfig.socketSize_[1]))*0x400;
                while ((block*socketSize*4) > 0xffff || (block*socketSize*4+(socketSize*4)-1) > 0xffff)
                {
                    block = (long)(block / 2);
                    microMemConfig.chipBlockRom_[1].Printf("%d", (int)block);
                }

                if (card == cardx)
                    setMemoryMapCDP18S625(config, microMemoryConfCardx, cardx, config->microboardType_[card]+MICROBOARD_LAST);
                else
                    setMemoryMapCDP18S625(config, microMemConfig, card, config->microboardType_[card]+MICROBOARD_LAST);
            break;
                
            case CARD_CDP18S626:
                if (microMemConfig.memLocation_[0] > 1)
                    microMemConfig.memLocation_[0] = 0;
                if (microMemConfig.socketSize_[0] > 1)
                    microMemConfig.socketSize_[0] = 0;

                if (card == cardx)
                    setMemoryMapCDP18S626(config, microMemoryConfCardx, cardx, config->microboardType_[card]+MICROBOARD_LAST);
                else
                    setMemoryMapCDP18S626(config, microMemConfig, card, config->microboardType_[card]+MICROBOARD_LAST);
            break;
 
            case CARD_CDP18S627:
                if (microMemConfig.memLocation_[0] > 15)
                    microMemConfig.memLocation_[0] = 0;

                if (card == cardx)
                    setMemoryMapCDP18S627(config, microMemoryConfCardx, cardx, config->microboardType_[card]+MICROBOARD_LAST);
                else
                    setMemoryMapCDP18S627(config, microMemConfig, card, config->microboardType_[card]+MICROBOARD_LAST);
            break;

            case CARD_CDP18S628:
            case CARD_CDP18S629:
                if (microMemConfig.memLocation_[0] > 1)
                    microMemConfig.memLocation_[0] = 0;
                
                if (card == cardx)
                    setMemoryMapCDP18S626(config, microMemoryConfCardx, cardx, config->microboardType_[card]+MICROBOARD_LAST);
                else
                    setMemoryMapCDP18S626(config, microMemConfig, card, config->microboardType_[card]+MICROBOARD_LAST);
                microMemConfig.socketSize_[0] = 0;
            break;
                
            case CARD_CDP18S640:
                checkBoardType(config, card, cardStr, controlWindowCard, controlWindowCardStr, elfConfig->useElfControlWindows);
                if (card == cardx)
                    setMemoryMapCDP18S640(config, microMemoryConfCardx, cardx, config->microboardType_[card]+MICROBOARD_LAST);
                else
                    setMemoryMapCDP18S640(config, microMemConfig, card, config->microboardType_[card]+MICROBOARD_LAST);

                controlWindowCardStr = cardStr;
                controlWindowCard = card;
                elfConfig->useElfControlWindows = true;
            break;

            case CARD_CDP18S641:
                checkBoardType(config, card, cardStr, uartCard, uartCardStr, elfConfig->useUart);

                uartCardStr = cardStr;
                uartCard = card;
                elfConfig->useUart = true;
                elfConfig->vtType = vtType;
            break;
          
            case CARD_CDP18S646:
                checkBoardType(config, card, cardStr, printerCard, printerCardStr, config->printerOn_);

                printerCardStr = cardStr;
                printerCard = card;
                config->printerOn_ = true;
            break;

            case CARD_CDP18S651:
                checkBoardType(config, card, cardStr, upd765Card, upd765CardStr, elfConfig->useUpd765);

                upd765CardStr = cardStr;
                upd765Card = card;
                elfConfig->useUpd765 = true;
            break;

            case CARD_CDP18S652:
                checkBoardType(config, card, cardStr, tapeCard, tapeCardStr, elfConfig->useTape);
                if (card == cardx)
                    setMemoryMapCDP18S652(config, microMemoryConfCardx, cardx, config->microboardType_[card]+MICROBOARD_LAST);
                else
                    setMemoryMapCDP18S652(config, microMemConfig, card, config->microboardType_[card]+MICROBOARD_LAST);

                tapeCardStr = cardStr;
                tapeCard = card;
                elfConfig->useTape = true;
            break;
                
            case CARD_CDP18S660:
                if (microMemConfig.socketSize_[1] > 1)
                    microMemConfig.socketSize_[1] = 0;

                checkBoardType(config, card, cardStr, pioCard, pioCardStr, elfConfig->useCdp18s660);
                if (card == cardx)
                    setMemoryMapCDP18S660(config, microMemoryConfCardx, cardx, config->microboardType_[card]+MICROBOARD_LAST);
                else
                    setMemoryMapCDP18S660(config, microMemConfig, card, config->microboardType_[card]+MICROBOARD_LAST);

                pioCardStr = cardStr;
                pioCard = card;
                elfConfig->useCdp18s660 = true;
            break;

            case CARD_CDP18S661B:
                checkBoardType(config, card, cardStr, v1870Card, v1870CardStr, elfConfig->usev1870);

                v1870CardStr = cardStr;
                v1870Card = card;
                elfConfig->usev1870 = true;
                config->videoMode_ = NTSC;
            break;

            case CARD_CDP18S661V1:
                checkBoardType(config, card, cardStr, v1870Card, v1870CardStr, elfConfig->usev1870);
                
                v1870CardStr = cardStr;
                v1870Card = card;
                elfConfig->usev1870 = true;
                config->videoMode_ = NTSC;
                if (elfConfig->v1870VideoMode > 2)
                    elfConfig->v1870VideoMode = 1;
            break;
                
            case CARD_CDP18S661V3:
                checkBoardType(config, card, cardStr, v1870Card, v1870CardStr, elfConfig->usev1870);

                v1870CardStr = cardStr;
                v1870Card = card;
                elfConfig->usev1870 = true;
                config->videoMode_ = PAL;
                if (elfConfig->v1870VideoMode > 2)
                    elfConfig->v1870VideoMode = 1;
            break;
        }
        setMicroMemConfiguration(card, microMemConfig);
    }
}

void GuiCdp18s600::setLoadMemoryMap(wxString romDir, wxString rom, int memoryType1, int memoryType2, long startAddress, long lastAddress, long inhibitStart, long inhibitEnd, Conf* config, int card, int boardType)
{
    if (rom == "" && memoryType1 == ROM)
        return;
    
    if (boardType == -1)
        p_Computer->readProgramMicro(romDir, rom, memoryType1, memoryType2, startAddress, lastAddress, inhibitStart, inhibitEnd);
    else
        setMemoryMap(config, startAddress, lastAddress-1, card, boardType, inhibitStart, inhibitEnd);
}

void GuiCdp18s600::setLoadMemoryMap(wxString romDir, wxString rom, int memoryType1, int memoryType2, int memoryType3, long startAddress, long lastAddress, long inhibitStart, long inhibitEnd, Conf* config, int card, int boardType, int memType)
{
    if (rom == "" && memoryType1 == ROM)
        return;
    
    if (boardType == -1)
    {
        p_Computer->readProgramMicro(romDir, rom, memoryType1, memoryType2, startAddress, lastAddress, inhibitStart, inhibitEnd);
        
        if (memType == 0)
            p_Computer->defineMemoryType(startAddress+0x400, lastAddress+0x3ff, memoryType3);
    }
    else
        setMemoryMap(config, startAddress, lastAddress-1, card, boardType, inhibitStart, inhibitEnd);
}

void GuiCdp18s600::setMemoryMap(Conf* config, long start, long end, int card, int boardType)
{
    setMemoryMap(config, start, end, card, boardType, 0xFFFF, 0);
}

void GuiCdp18s600::setMemoryMap(Conf* config, long start, long end, int card, int boardType, long inhibitStart, long inhibitEnd)
{
    wxString message1, message2;
    Word address = start;
    start = (start / 256) & 0xff;
    end = (end / 256) & 0xff;
    for (long i=start; i<=end; i++)
    {
        if (!(address >= inhibitStart && address <= inhibitEnd))
        {
            if (config->memoryMapType_[i] != -1) // && config->memoryMapCard_[i] != card)
            {
                if (config->memoryMapCard_[i] == card)
                {
                    message1.Printf("Warning: Memory overlap on this card");
                    if (config->errorMemoryOverlapp_[card] == "")
                        config->errorMemoryOverlapp_[card] = message1;
                }
                else
                {
                    message1.Printf("Error: Memory overlap with " + microBoardStr[config->memoryMapType_[i]] + " in slot %d", config->memoryMapCard_[i]+1);
                    config->errorMemoryOverlapp_[card] = message1;
                    message2.Printf("Error: Memory overlap with " + microBoardStr[boardType] + " in slot %d", card+1);
                    config->errorMemoryOverlapp_[config->memoryMapCard_[i]] = message2;
                }
            }
            config->memoryMapType_[i] = boardType;
                config->memoryMapCard_[i] = card;
        }
        address += 256;
    }
}

void GuiCdp18s600::setMemoryMapCDP18S600(Conf* config, int card, int boardType)
{
    Word currentRomSize = guiRomSize[config->microChipType_[ONE_SOCKET]];
    int divider = (2 << (config->microChipType_[ONE_SOCKET]))/2;
    int location = config->microChipLocation_[ONE_SOCKET] / divider;
    Word startAddress = location * (currentRomSize+1);
    Word lastAddress = startAddress + currentRomSize;
    
    if (boardType == -1)
    {
        p_Cdp18s600->readMicro(U21ROM, startAddress, lastAddress);
        p_Main->checkAndReInstallFile(MICROBOARD, "ROM U20", U20ROM);
    }
    else
    {
        if (!config->microChipDisable_[U21ROM] && config->rom_[U21ROM] != "")
            setMemoryMap(config, startAddress, lastAddress, card, boardType);
    }

    currentRomSize = guiRomSize[config->microChipType_[FOUR_SOCKET]];
    divider = (2 << (config->microChipType_[FOUR_SOCKET]))/2;
    location = config->microChipLocation_[FOUR_SOCKET] / divider;
    startAddress = location * (4*(currentRomSize+1));
    lastAddress = startAddress + currentRomSize;
    
    if (boardType == -1)
        p_Cdp18s600->readMicro(U20ROM, startAddress, lastAddress);
    else
    {
        if (!config->microChipDisable_[U20ROM] && config->rom_[U20ROM] != "")
            setMemoryMap(config, startAddress, lastAddress, card, boardType);
    }

    startAddress += (currentRomSize+1);
    lastAddress = startAddress + currentRomSize;
    
    if (boardType == -1)
        p_Cdp18s600->readMicro(U19ROM, startAddress, lastAddress);
    else
    {
        if (!config->microChipDisable_[U19ROM] && config->rom_[U19ROM] != "")
			setMemoryMap(config, startAddress, lastAddress, card, boardType);
    }
    
    startAddress += (currentRomSize+1);
    lastAddress = startAddress + currentRomSize;
    
    if (boardType == -1)
        p_Cdp18s600->readMicro(U18ROM, startAddress, lastAddress);
    else
    {
        if (!config->microChipDisable_[U18ROM] && config->rom_[U18ROM] != "")
			setMemoryMap(config, startAddress, lastAddress, card, boardType);
    }
    
    startAddress += (currentRomSize+1);
    lastAddress = startAddress + currentRomSize;
    
    if (boardType == -1)
        p_Cdp18s600->readMicro(U17ROM, startAddress, lastAddress);
    else
    {
        if (!config->microChipDisable_[U17ROM] && config->rom_[U17ROM] != "")
			setMemoryMap(config, startAddress, lastAddress, card, boardType);
    }
}

void GuiCdp18s600::setMemoryMapCDP18S601(Conf* config, int card, int boardType)
{
    Word currentRomSize = 0xfff;
    int ramLocation = config->microChipType_[ONE_SOCKET];
    Word startAddress = ramLocation * 0x1000;
    Word lastAddress = startAddress + currentRomSize;
    
    if (boardType == -1)
    {
        p_Computer->readProgramMicro(config->romDir_[U21ROM], config->rom_[U21ROM], RAM, startAddress, lastAddress+1, NONAME);
        p_Computer->defineMemoryType(startAddress, lastAddress, RAM);
    }
    else
        setMemoryMap(config, startAddress, lastAddress, card, boardType);

    if (config->microChipType_[FOUR_SOCKET] == 1)
    { // 2Kx8 for ROM SOCKET
        currentRomSize = guiRomSizeCDP18S601[config->microChipMemory_[XU27ROM]];
        int romLocation = config->microChipLocation_[FOUR_SOCKET_ROM1];
        startAddress = romLocation * 0x1000;
        lastAddress = startAddress + currentRomSize;
        
        if (romLocation != ramLocation)
        {
			setLoadMemoryMap(config->romDir_[XU27ROM], config->rom_[XU27ROM], ROM, ROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType);

            if (config->microChipMemory_[XU27ROM] == 0)
            {
                if (boardType == -1)
                    p_Computer->defineMemoryType(startAddress+0x400, lastAddress+0x400, MAPPEDROM);
                else
                    setMemoryMap(config, startAddress+0x400, lastAddress+0x400, card, boardType);
            }
        }
        else
        {
            if (boardType == -1)
                p_Computer->readProgramMicro(config->romDir_[XU27ROM], config->rom_[XU27ROM], NOCHANGE, startAddress, lastAddress+1, NONAME);
            else
                setMemoryMap(config, startAddress, lastAddress, card, boardType);
        }
        
        currentRomSize = guiRomSizeCDP18S601[config->microChipMemory_[XU26ROM]];
        startAddress += 0x800;
        lastAddress = startAddress + currentRomSize;
        
        if (romLocation != ramLocation)
        {
			setLoadMemoryMap(config->romDir_[XU26ROM], config->rom_[XU26ROM], ROM, ROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType);

            if (config->microChipMemory_[XU26ROM] == 0)
            {
                if (boardType == -1)
                    p_Computer->defineMemoryType(startAddress+0x400, lastAddress+0x400, MAPPEDROM);
                else
                    setMemoryMap(config, startAddress+0x400, lastAddress+0x400, card, boardType);
            }
        }
        else
        {
            if (boardType == -1)
                p_Computer->readProgramMicro(config->romDir_[XU26ROM], config->rom_[XU26ROM], NOCHANGE, startAddress, lastAddress+1, NONAME);
            else
                setMemoryMap(config, startAddress, lastAddress, card, boardType);
        }

        p_Main->checkAndReInstallFile(MICROBOARD, "ROM XU25", XU25ROM);
        currentRomSize = guiRomSizeCDP18S601[config->microChipMemory_[XU25ROM]];
        int location = config->microChipLocation_[FOUR_SOCKET_ROM2];
        startAddress = location * 0x1000;
        lastAddress = startAddress + currentRomSize;
        
        if (location != ramLocation && location != romLocation)
        {
			setLoadMemoryMap(config->romDir_[XU25ROM], config->rom_[XU25ROM], ROM, ROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType);

            if (config->microChipMemory_[XU25ROM] == 0 && config->rom_[XU25ROM] != "")
            {
                if (boardType == -1)
                    p_Computer->defineMemoryType(startAddress+0x400, lastAddress+0x400, MAPPEDROM);
                else
                    setMemoryMap(config, startAddress+0x400, lastAddress+0x400, card, boardType);
            }
        }
        else
        {
            if (boardType == -1)
            {
                if (location != ramLocation)
				{
					if (config->rom_[XU25ROM] != "")
						p_Computer->readProgramMicro(config->romDir_[XU25ROM], config->rom_[XU25ROM], ROM, startAddress, lastAddress+1, NONAME);
				}
                else
                    p_Computer->readProgramMicro(config->romDir_[XU25ROM], config->rom_[XU25ROM], NOCHANGE, startAddress, lastAddress+1, NONAME);
            }
            else
			{
				if (config->rom_[XU25ROM] != "")
	                setMemoryMap(config, startAddress, lastAddress, card, boardType);
			}
        }
        
        currentRomSize = guiRomSizeCDP18S601[config->microChipMemory_[XU24ROM]];
        startAddress += 0x800;
        lastAddress = startAddress + currentRomSize;
        
        if (location != ramLocation && location != romLocation)
        {
			setLoadMemoryMap(config->romDir_[XU24ROM], config->rom_[XU24ROM], ROM, ROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType);

            if (config->microChipMemory_[XU24ROM] == 0 && config->rom_[XU24ROM] != "")
            {
                if (boardType == -1)
                    p_Computer->defineMemoryType(startAddress+0x400, lastAddress+0x400, MAPPEDROM);
                else
                    setMemoryMap(config, startAddress+0x400, lastAddress+0x400, card, boardType);
            }
        }
        else
        {
            if (boardType == -1)
            {
                if (location != ramLocation)
				{
					if (config->rom_[XU24ROM] != "")
						p_Computer->readProgramMicro(config->romDir_[XU24ROM], config->rom_[XU24ROM], ROM, startAddress, lastAddress+1, NONAME);
				}
                else
                    p_Computer->readProgramMicro(config->romDir_[XU24ROM], config->rom_[XU24ROM], NOCHANGE, startAddress, lastAddress+1, NONAME);
            }
            else
			{
				if (config->rom_[XU24ROM] != "")
					setMemoryMap(config, startAddress, lastAddress, card, boardType);
			}
        }
    }
    else
    { // 1Kx8 for ROM SOCKET
        currentRomSize = 0x3ff;
        int romLocation = config->microChipLocation_[FOUR_SOCKET_ROM2];
        startAddress = romLocation * 0x1000;
        lastAddress = startAddress + currentRomSize;
        
        if (boardType == -1)
        {
            if (romLocation != ramLocation)
            {
				if (config->rom_[XU25ROM] != "")
				{
					p_Computer->readProgramMicro(config->romDir_[XU25ROM], config->rom_[XU25ROM], ROM, startAddress, lastAddress+1, NONAME);
					p_Computer->defineMemoryType(startAddress, lastAddress, ROM);
				}
            }
            else
                p_Computer->readProgramMicro(config->romDir_[XU25ROM], config->rom_[XU25ROM], NOCHANGE, startAddress, lastAddress+1, NONAME);
        }
        else
		{
			if (config->rom_[XU25ROM] != "")
	            setMemoryMap(config, startAddress, lastAddress, card, boardType);
		}

        startAddress += 0x400;
        lastAddress = startAddress + currentRomSize;
        
        if (boardType == -1)
        {
            if (romLocation != ramLocation)
            {
				if (config->rom_[XU27ROM] != "")
				{
					p_Computer->readProgramMicro(config->romDir_[XU27ROM], config->rom_[XU27ROM], ROM, startAddress, lastAddress+1, NONAME);
					p_Computer->defineMemoryType(startAddress, lastAddress, ROM);
				}
            }
            else
                p_Computer->readProgramMicro(config->romDir_[XU27ROM], config->rom_[XU27ROM], NOCHANGE, startAddress, lastAddress+1, NONAME);
        }
        else
        {
			if (config->rom_[XU27ROM] != "")
				setMemoryMap(config, startAddress, lastAddress, card, boardType);
		}

        startAddress += 0x400;
        lastAddress = startAddress + currentRomSize;
        
        if (boardType == -1)
        {
            if (romLocation != ramLocation)
            {
				if (config->rom_[XU24ROM] != "")
				{
	                p_Computer->readProgramMicro(config->romDir_[XU24ROM], config->rom_[XU24ROM], ROM, startAddress, lastAddress+1, NONAME);
		            p_Computer->defineMemoryType(startAddress, lastAddress, ROM);
				}
            }
            else
                p_Computer->readProgramMicro(config->romDir_[XU24ROM], config->rom_[XU24ROM], NOCHANGE, startAddress, lastAddress+1, NONAME);
        }
        else
		{
			if (config->rom_[XU24ROM] != "")
				setMemoryMap(config, startAddress, lastAddress, card, boardType);
		}

        startAddress += 0x400;
        lastAddress = startAddress + currentRomSize;
        
        if (boardType == -1)
        {
            if (romLocation != ramLocation)
            {
				if (config->rom_[XU26ROM] != "")
				{
	                p_Computer->readProgramMicro(config->romDir_[XU26ROM], config->rom_[XU26ROM], ROM, startAddress, lastAddress+1, NONAME);
	                p_Computer->defineMemoryType(startAddress, lastAddress, ROM);
				}
            }
            else
                p_Computer->readProgramMicro(config->romDir_[XU26ROM], config->rom_[XU26ROM], NOCHANGE, startAddress, lastAddress+1, NONAME);
        }
        else
        {
			if (config->rom_[XU26ROM] != "")
				setMemoryMap(config, startAddress, lastAddress, card, boardType);
		}
    }
}

void GuiCdp18s600::setMemoryMapCDP18S602(Conf* config, int card, int boardType)
{
    wxString ramBlockStr = config->microChipBlock_[ONE_SOCKET];
    long ramBlock;
    ramBlockStr.ToLong(&ramBlock);
    
    Word startAddress = ramBlock * 0x800;
    Word lastAddress = startAddress + 0x7ff;
    
    if (boardType == -1)
    {
        p_Computer->readProgramMicro(config->romDir_[U21ROM], config->rom_[U21ROM], RAM, startAddress, lastAddress+1, NONAME);
        p_Computer->defineMemoryType(startAddress, lastAddress, RAM);
    }
    else
        setMemoryMap(config, startAddress, lastAddress, card, boardType);

    if (config->microChipDisable_[FOUR_SOCKET])
        return;
    
    int chipType = config->microChipType_[FOUR_SOCKET]+1;
    Word currentRomSize = chipType*0x400;
    
    wxString romBlockStr = config->microChipBlock_[FOUR_SOCKET];
    long romBlock;
    romBlockStr.ToLong(&romBlock);
    
    startAddress = romBlock * (currentRomSize*2);
    lastAddress = startAddress + currentRomSize - 1;
    
	setLoadMemoryMap(config->romDir_[U20ROM], config->rom_[U20ROM], ROM, ROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType);

    startAddress += currentRomSize;
    lastAddress = startAddress + currentRomSize - 1;
    
	setLoadMemoryMap(config->romDir_[U19ROM], config->rom_[U19ROM], ROM, ROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType);
}

void GuiCdp18s600::setMemoryMapCDP18S603a(Conf* config, int card, int boardType)
{
    Word currentRomSize = 0x3ff;
    int ramLocation = config->microChipType_[ONE_SOCKET];
    Word startAddress = ramLocation * 0x1000;
    Word lastAddress = startAddress + currentRomSize;
    
    if (boardType == -1)
    {
        p_Computer->readProgramMicro(config->romDir_[U21ROM], config->rom_[U21ROM], RAM, startAddress, lastAddress+1, NONAME);
        p_Computer->defineMemoryType(startAddress, lastAddress, RAM);
        p_Computer->defineMemoryType(lastAddress+1, startAddress+0xfff, MAPPEDRAM);
    }
    else
        setMemoryMap(config, startAddress, lastAddress, card, boardType);

    if (config->microChipType_[FOUR_SOCKET] == 1)
    { // 2Kx8 for ROM SOCKET
        currentRomSize = guiRomSizeCDP18S601[config->microChipMemory_[XU27ROM]];
        int romLocation = config->microChipLocation_[FOUR_SOCKET_ROM1];
        startAddress = romLocation * 0x1000;
        lastAddress = startAddress + currentRomSize;
        
        if (boardType == -1)
        {
            if (romLocation != ramLocation)
            {
				if (config->rom_[XU27ROM] != "")
				{
					p_Computer->readProgramMicro(config->romDir_[XU27ROM], config->rom_[XU27ROM], ROM, startAddress, lastAddress+1, NONAME);
					p_Computer->defineMemoryType(startAddress, lastAddress, ROM);
                
					if (config->microChipMemory_[XU27ROM] == 0)
						p_Computer->defineMemoryType(startAddress+0x400, lastAddress+0x400, MAPPEDROM);
				}
            }
            else
                p_Computer->readProgramMicro(config->romDir_[XU27ROM], config->rom_[XU27ROM], NOCHANGE, startAddress, lastAddress+1, NONAME);
        }
        else
        {
			if (config->rom_[XU27ROM] != "")
	            setMemoryMap(config, startAddress, lastAddress, card, boardType);
		}

        currentRomSize = guiRomSizeCDP18S601[config->microChipMemory_[XU26ROM]];
        startAddress += 0x800;
        lastAddress = startAddress + currentRomSize;
        
        if (boardType == -1)
        {
            if (romLocation != ramLocation)
            {
				if (config->rom_[XU26ROM] != "")
				{
					p_Computer->readProgramMicro(config->romDir_[XU26ROM], config->rom_[XU26ROM], ROM, startAddress, lastAddress+1, NONAME);                
					p_Computer->defineMemoryType(startAddress, lastAddress, ROM);
	                
					if (config->microChipMemory_[XU26ROM] == 0)
						p_Computer->defineMemoryType(startAddress+0x400, lastAddress+0x400, MAPPEDROM);
				}
            }
            else
                p_Computer->readProgramMicro(config->romDir_[XU26ROM], config->rom_[XU26ROM], NOCHANGE, startAddress, lastAddress+1, NONAME);
        }
        else
        {
			if (config->rom_[XU26ROM] != "")
				setMemoryMap(config, startAddress, lastAddress, card, boardType);
		}

        p_Main->checkAndReInstallFile(MICROBOARD, "ROM XU25", XU25ROM);
        currentRomSize = guiRomSizeCDP18S601[config->microChipMemory_[XU25ROM]];
        int location = config->microChipLocation_[FOUR_SOCKET_ROM2];
        startAddress = location * 0x1000;
        lastAddress = startAddress + currentRomSize;
        
        if (boardType == -1)
        {
            if (location != ramLocation && location != romLocation)
            {
				if (config->rom_[XU25ROM] != "")
				{
					p_Computer->readProgramMicro(config->romDir_[XU25ROM], config->rom_[XU25ROM], ROM, startAddress, lastAddress+1, NONAME);                
					p_Computer->defineMemoryType(startAddress, lastAddress, ROM);
	                
					if (config->microChipMemory_[XU25ROM] == 0)
						p_Computer->defineMemoryType(startAddress+0x400, lastAddress+0x400, MAPPEDROM);
				}
            }
            else
            {
                if (location != ramLocation)
				{
					if (config->rom_[XU25ROM] != "")
                    p_Computer->readProgramMicro(config->romDir_[XU25ROM], config->rom_[XU25ROM], ROM, startAddress, lastAddress+1, NONAME);
				}
                else
                    p_Computer->readProgramMicro(config->romDir_[XU25ROM], config->rom_[XU25ROM], NOCHANGE, startAddress, lastAddress+1, NONAME);
            }
        }
        else
        {
			if (config->rom_[XU25ROM] != "")
	            setMemoryMap(config, startAddress, lastAddress, card, boardType);
		}

        currentRomSize = guiRomSizeCDP18S601[config->microChipMemory_[XU24ROM]];
        startAddress += 0x800;
        lastAddress = startAddress + currentRomSize;
        
        if (boardType == -1)
        {
            if (location != ramLocation && location != romLocation)
            {
				if (config->rom_[XU24ROM] != "")
				{
					p_Computer->readProgramMicro(config->romDir_[XU24ROM], config->rom_[XU24ROM], ROM, startAddress, lastAddress+1, NONAME);
					p_Computer->defineMemoryType(startAddress, lastAddress, ROM);
	                
					if (config->microChipMemory_[XU24ROM] == 0)
						p_Computer->defineMemoryType(startAddress+0x400, lastAddress+0x400, MAPPEDROM);
				}
            }
            else
            {
                if (location != ramLocation)
				{
					if (config->rom_[XU24ROM] != "")
						p_Computer->readProgramMicro(config->romDir_[XU24ROM], config->rom_[XU24ROM], ROM, startAddress, lastAddress+1, NONAME);
				}
                else
                    p_Computer->readProgramMicro(config->romDir_[XU24ROM], config->rom_[XU24ROM], NOCHANGE, startAddress, lastAddress+1, NONAME);
            }
        }
        else
		{
			if (config->rom_[XU24ROM] != "")
	            setMemoryMap(config, startAddress, lastAddress, card, boardType);
		}
    }
    else
    { // 1Kx8 for ROM SOCKET
        currentRomSize = 0x3ff;
        int romLocation = config->microChipLocation_[FOUR_SOCKET_ROM2];
        startAddress = romLocation * 0x1000;
        lastAddress = startAddress + currentRomSize;
        
        if (boardType == -1)
        {
            if (romLocation != ramLocation)
            {
				if (config->rom_[XU25ROM] != "")
				{
					p_Computer->readProgramMicro(config->romDir_[XU25ROM], config->rom_[XU25ROM], ROM, startAddress, lastAddress+1, NONAME);
					p_Computer->defineMemoryType(startAddress, lastAddress, ROM);
				}
            }
            else
                p_Computer->readProgramMicro(config->romDir_[XU25ROM], config->rom_[XU25ROM], NOCHANGE, startAddress, lastAddress+1, NONAME);
        }
        else
        {
			if (config->rom_[XU25ROM] != "")
            setMemoryMap(config, startAddress, lastAddress, card, boardType);
		}

        startAddress += 0x400;
        lastAddress = startAddress + currentRomSize;
        
		setLoadMemoryMap(config->romDir_[XU27ROM], config->rom_[XU27ROM], ROM, ROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType);

        startAddress += 0x400;
        lastAddress = startAddress + currentRomSize;
        
		setLoadMemoryMap(config->romDir_[XU24ROM], config->rom_[XU24ROM], ROM, ROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType);

        startAddress += 0x400;
        lastAddress = startAddress + currentRomSize;
        
		setLoadMemoryMap(config->romDir_[XU26ROM], config->rom_[XU26ROM], ROM, ROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType);
    }
}

void GuiCdp18s600::setMemoryMapCDP18S604b(Conf* config, int card, int boardType)
{
    wxString ramBlockStr = config->microChipBlock_[ONE_SOCKET];
    long ramBlock;
    ramBlockStr.ToLong(&ramBlock);
    
    Word startAddress = ramBlock * 0x400;
    Word lastAddress = startAddress + 0x3ff;
    
    if (boardType == -1)
    {
        p_Computer->readProgramMicro(config->romDir_[U21ROM], config->rom_[U21ROM], RAM, startAddress, lastAddress+1, NONAME);
        p_Computer->defineMemoryType(startAddress, lastAddress, RAM);
    }
    else
        setMemoryMap(config, startAddress, lastAddress, card, boardType);

    if (config->rom_[U20ROM] == "")
        return;

    int chipType = config->microChipType_[FOUR_SOCKET];
    int convertedChipType = p_Main->convert604BChipType(chipType);
    Word currentRomSize = convertedChipType*0x400;
    
    wxString romBlockStr = config->microChipBlock_[FOUR_SOCKET];
    long romBlock;
    romBlockStr.ToLong(&romBlock);
    
    startAddress = romBlock * currentRomSize;
    
    if (chipType == 0)
        lastAddress = startAddress + 0x1ff;
    else
        lastAddress = startAddress + currentRomSize - 1;
    
	if (config->rom_[U20ROM] != "")
	{
		if (boardType == -1)
		{
			p_Computer->readProgramMicro(config->romDir_[U20ROM], config->rom_[U20ROM], ROM, startAddress, lastAddress+1, NONAME);
			if (chipType == 0)
			{
				p_Computer->defineMemoryType(startAddress, lastAddress, ROM);
				p_Computer->defineMemoryType(lastAddress+1, lastAddress+0x200, MAPPEDROM);
			}
			else
				p_Computer->defineMemoryType(startAddress, lastAddress, ROM);
		}
		else
			setMemoryMap(config, startAddress, lastAddress, card, boardType);
	}
}

void GuiCdp18s600::setMemoryMapRcasbc(Conf* config, int card, int boardType)
{
    if (boardType == -1)
    {
        p_Computer->readProgramMicro(config->romDir_[U21ROM], config->rom_[U21ROM], RAM, 0x8000, 0xA000, NONAME);
        p_Computer->defineMemoryType(0x8000, 0x9fff, RAM);
    }
    else
        setMemoryMap(config, 0x8000, 0x9fff, card, boardType);
    
    if (config->rom_[U20ROM] == "")
        return;

    if (boardType == -1)
    {
        p_Computer->readProgramMicro(config->romDir_[U20ROM], config->rom_[U20ROM], ROM, 0, 0x8000, NONAME);
        p_Computer->defineMemoryType(0, 0x7fff, ROM);
    }
    else
        setMemoryMap(config, 0, 0x7fff, card, boardType);
}

void GuiCdp18s600::setMemoryMapCDP18S620(Conf* config, MicroMemoryConf memConf, int card, int boardType)
{
    Word startAddress, socketSize;

    startAddress = 0x1000 * memConf.memLocation_[0];
    socketSize = 0x1000;

    if (boardType == -1)
        p_Computer->readProgramMicro(memConf.romDir_[0], memConf.rom_[0], RAM, RAM, startAddress, startAddress+socketSize, 0xFFFF, 0);
    else
        setMemoryMap(config, startAddress, startAddress+socketSize-1, card, boardType);
}

void GuiCdp18s600::setMemoryMapCDP18S621(Conf* config, MicroMemoryConf memConf, int card, int boardType)
{
    Word startAddress, socketSize;
    Word inhibitStart = 0xFFFF, inhibitStop = 0;

    startAddress = 0x4000 * memConf.memLocation_[0];
    socketSize = 0x4000;
    
    setInhibitBlock(memConf, &startAddress, &socketSize, &inhibitStart, &inhibitStop, 0);
    
    if (!memConf.inhibitBlock_[0][0] || !memConf.inhibitBlock_[0][1] || !memConf.inhibitBlock_[0][2] || !memConf.inhibitBlock_[0][3])
    {
        if (boardType == -1)
            p_Computer->readProgramMicro(memConf.romDir_[0], memConf.rom_[0], RAM, RAM, startAddress, startAddress+socketSize, inhibitStart, inhibitStop);
        else
            setMemoryMap(config, startAddress, startAddress+socketSize-1, card, boardType, inhibitStart, inhibitStop);
    }
}

void GuiCdp18s600::setMemoryMapCDP18S623a(Conf* config, MicroMemoryConf memConf, int card, int boardType)
{
    Word startAddress, socketSize;

    startAddress = 0x2000 * memConf.memLocation_[0];
    socketSize = 0x2000;
    
    if (boardType == -1)
        p_Computer->readProgramMicro(memConf.romDir_[0], memConf.rom_[0], RAM, RAM, startAddress, startAddress+socketSize, 0xFFFF, 0);
    else
        setMemoryMap(config, startAddress, startAddress+socketSize-1, card, boardType);
}

void GuiCdp18s600::setMemoryMapCDP18S625(Conf* config, MicroMemoryConf memConf, int card, int boardType)
{
    Word startAddress, socketSize;
    Word inhibitStart = 0xFFFF, inhibitStop = 0;
    
    long block;
    memConf.chipBlockRom_[0].ToLong(&block);
    
    socketSize = (1 << (memConf.socketSize_[0]))*0x1000;
    startAddress = block * socketSize;
    
    setInhibitBlock(memConf, &startAddress, &socketSize, &inhibitStart, &inhibitStop, 0);
    
    if (!memConf.inhibitBlock_[0][0] || !memConf.inhibitBlock_[0][1] || !memConf.inhibitBlock_[0][2] || !memConf.inhibitBlock_[0][3])
    {
        setLoadMemoryMap(memConf.romDir_[0], memConf.rom_[0], ROM, ROM, startAddress, startAddress+socketSize, inhibitStart, inhibitStop, config, card, boardType);
    }
    
    inhibitStart = 0xFFFF; inhibitStop = 0;
    memConf.chipBlockRom_[1].ToLong(&block);
    
    socketSize = (1 << (memConf.socketSize_[1]))*0x1000;
    startAddress = block * socketSize;
    
    setInhibitBlock(memConf, &startAddress, &socketSize, &inhibitStart, &inhibitStop, 1);
    
    if (!memConf.inhibitBlock_[1][0] || !memConf.inhibitBlock_[1][1] || !memConf.inhibitBlock_[1][2] || !memConf.inhibitBlock_[1][3])
    {
        setLoadMemoryMap(memConf.romDir_[1], memConf.rom_[1], ROM, ROM, startAddress, startAddress+socketSize, inhibitStart, inhibitStop, config, card, boardType);
    }
}

void GuiCdp18s600::setMemoryMapCDP18S626(Conf* config, MicroMemoryConf memConf, int card, int boardType)
{
    Word startAddress, socketSize;

    startAddress = 0x8000 * memConf.memLocation_[0];
    socketSize = 0x2000 * (memConf.socketSize_[0] + 1);
//    if ((startAddress + socketSize) > 0x10000)
//    {
//        memConf.memLocation_[0] = 0;
//        memConf.socketSize_[0] = 0;
//        p_Main->setMicroMemConfiguration(card, memConf);
//    }

    for (int section = 0; section<4; section++)
    {
        int inhibitLocation, inhibitType;
        if (memConf.socketSize_[0] == 1)
        {
            inhibitLocation = memConf.inhibit64_;
            inhibitType = 0;
        }
        else
        {
            if (memConf.memLocation_[0] == 0)
            {
                inhibitLocation = memConf.inhibit32Low_;
                inhibitType = 1;
                if (config->microboardType_[card] == CARD_CDP18S628)
                    inhibitType = 3;
            }
            else
            {
                inhibitLocation = memConf.inhibit32High_;
                inhibitType = 2;
                if (config->microboardType_[card] == CARD_CDP18S628)
                    inhibitType = 4;
            }
        }
        
        if (config->microboardType_[card] == CARD_CDP18S629)
        {
            int memType1 = memConf.memType[section]+1;
            int memType2 = memConf.memType[section]+1;
            if (memConf.memType[section] == 2)
            {
                memType1 = RAM;
                memType2 = ROM;
            }
            if (boardType == -1)
                p_Computer->readProgramMicro(memConf.romDir_[section], memConf.rom_[section], memType1, memType2, startAddress, startAddress+socketSize, inhibit[inhibitType][inhibitLocation][0], inhibit[inhibitType][inhibitLocation][1]);
            else
                setMemoryMap(config, startAddress, startAddress+socketSize-1, card, boardType, inhibit[inhibitType][inhibitLocation][0], inhibit[inhibitType][inhibitLocation][1]);
        }
        else
        {
            setLoadMemoryMap(memConf.romDir_[section], memConf.rom_[section], memConf.memType[section]+1, memConf.memType[section]+1, startAddress, startAddress+socketSize, inhibit[inhibitType][inhibitLocation][0], inhibit[inhibitType][inhibitLocation][1], config, card, boardType);
        }
        startAddress += socketSize;
    }
}

void GuiCdp18s600::setMemoryMapCDP18S627(Conf* config, MicroMemoryConf memConf, int card, int boardType)
{
    Word startAddress, socketSize;

    startAddress = 0x1000 * memConf.memLocation_[0];
    socketSize = 0x1000;
    
    setLoadMemoryMap(memConf.romDir_[0], memConf.rom_[0], ROM, ROM, startAddress, startAddress+socketSize, 0xFFFF, 0, config, card, boardType);
}

void GuiCdp18s600::setMemoryMapCDP18S640(Conf* config, MicroMemoryConf memConf, int card, int boardType)
{
    Word startAddress, socketSize;

    startAddress = 0x8C00;
    socketSize = 0x400;
    
    if (boardType == -1)
        p_Computer->readProgramMicro(memConf.romDir_[0], memConf.rom_[0], RAM, RAM, startAddress, startAddress+socketSize, 0xFFFF, 0);
    else
        setMemoryMap(config, startAddress, startAddress+socketSize-1, card, boardType);

    startAddress = 0x8000;
    socketSize = 0x800;
    
    setLoadMemoryMap(memConf.romDir_[1], memConf.rom_[1], ROM, ROM, startAddress, startAddress+socketSize, 0xFFFF, 0, config, card, boardType);
}

void GuiCdp18s600::setMemoryMapCDP18S652(Conf* config, MicroMemoryConf memConf, int card, int boardType)
{
    Word startAddress, socketSize;

    if (!memConf.disableCardMemory_)
    {
        if (memConf.socketSize_[0] == 0)
        {
            startAddress = 0x8c00;
            socketSize = 0x400;
        }
        else
        {
            startAddress = 0x8800;
            socketSize = 0x800;
        }

        if (boardType == -1)
            p_Computer->readProgramMicro(memConf.ramDir_, memConf.ram_, RAM, RAM, startAddress, startAddress+socketSize, 0xFFFF, 0);
        else
            setMemoryMap(config, startAddress, startAddress+socketSize-1, card, boardType);

        setLoadMemoryMap(memConf.romDir_[CDP626_SEC1], memConf.rom_[CDP626_SEC1], ROM, ROM, 0x8000, 0x8800, 0xFFFF, 0, config, card, boardType);

        setLoadMemoryMap(memConf.romDir_[CDP626_SEC2], memConf.rom_[CDP626_SEC2], ROM, ROM, 0x9000, 0xa800, 0xFFFF, 0, config, card, boardType);
       
        setLoadMemoryMap(memConf.romDir_[CDP626_SEC3], memConf.rom_[CDP626_SEC3], ROM, ROM, 0xB000, 0xdfff, 0xFFFF, 0, config, card, boardType);

        setLoadMemoryMap(memConf.romDir_[CDP626_SEC4], memConf.rom_[CDP626_SEC4], ROM, ROM, 0xE000, 0xf000, 0xFFFF, 0, config, card, boardType);
    }
}

void GuiCdp18s600::setMemoryMapCDP18S660(Conf* config, MicroMemoryConf memConf, int card, int boardType)
{
    Word socketSize;
    Word currentRomSize, lastAddress;
    Word startAddress = memConf.memLocation_[ONE_SOCKET] * 0x800;

    socketSize = 0x800;
    
    if (boardType == -1)
        p_Computer->readProgramMicro(memConf.ramDir_, memConf.ram_, RAM, RAM, startAddress, startAddress+socketSize, 0xFFFF, 0);
    else
        setMemoryMap(config, startAddress, startAddress+socketSize-1, card, boardType);

    if (memConf.socketSize_[FOUR_SOCKET] == 1)
    { // 2Kx8 for ROM SOCKET
        int romLocation = memConf.memLocation_[FOUR_SOCKET_ROM1];
        
        currentRomSize = guiRomSizeCDP18S601[memConf.memType[XU23ROM]];
        startAddress = romLocation * 0x1000;
        lastAddress = startAddress + currentRomSize;
        
        setLoadMemoryMap(memConf.romDir_[XU23ROM], memConf.rom_[XU23ROM], ROM, ROM, MAPPEDROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType, memConf.memType[XU23ROM]);
        
        currentRomSize = guiRomSizeCDP18S601[memConf.memType[XU22ROM]];
        startAddress += 0x800;
        lastAddress = startAddress + currentRomSize;
        
        setLoadMemoryMap(memConf.romDir_[XU22ROM], memConf.rom_[XU22ROM], ROM, ROM, MAPPEDROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType, memConf.memType[XU22ROM]);

        romLocation = memConf.memLocation_[FOUR_SOCKET_ROM2];
        
        currentRomSize = guiRomSizeCDP18S601[memConf.memType[XU21ROM]];
        startAddress = romLocation * 0x1000;
        lastAddress = startAddress + currentRomSize;
        
        setLoadMemoryMap(memConf.romDir_[XU21ROM], memConf.rom_[XU21ROM], ROM, ROM, MAPPEDROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType, memConf.memType[XU21ROM]);

        currentRomSize = guiRomSizeCDP18S601[memConf.memType[XU20ROM]];
        startAddress += 0x800;
        lastAddress = startAddress + currentRomSize;
        
        setLoadMemoryMap(memConf.romDir_[XU20ROM], memConf.rom_[XU20ROM], ROM, ROM, MAPPEDROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType, memConf.memType[XU20ROM]);
    }
    else
    { // 1Kx8 for ROM SOCKET
        int romLocation = memConf.memLocation_[FOUR_SOCKET_ROM2];
        
        currentRomSize = 0x3ff;
        startAddress = romLocation * 0x1000;
        lastAddress = startAddress + currentRomSize;
        
        setLoadMemoryMap(memConf.romDir_[XU23ROM], memConf.rom_[XU23ROM], ROM, ROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType);

        startAddress += 0x400;
        lastAddress = startAddress + currentRomSize;
        
        setLoadMemoryMap(memConf.romDir_[XU21ROM], memConf.rom_[XU21ROM], ROM, ROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType);

        startAddress += 0x400;
        lastAddress = startAddress + currentRomSize;
        
        setLoadMemoryMap(memConf.romDir_[XU22ROM], memConf.rom_[XU22ROM], ROM, ROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType);

        startAddress += 0x400;
        lastAddress = startAddress + currentRomSize;
        
        setLoadMemoryMap(memConf.romDir_[XU20ROM], memConf.rom_[XU20ROM], ROM, ROM, startAddress, lastAddress+1, 0xFFFF, 0, config, card, boardType);
    }
}

void GuiCdp18s600::setInhibitBlock(MicroMemoryConf memConf, Word* startAddress, Word* socketSize, Word* inhibitStart, Word* inhibitStop, int bank)
{
    Word chipSize = *socketSize/4;
    
    if (memConf.inhibitBlock_[bank][1])
    {
        *inhibitStart = *startAddress + chipSize;
        *inhibitStop = *inhibitStart + chipSize - 1;
    }
    if (memConf.inhibitBlock_[bank][2])
    {
        *inhibitStart = *startAddress + (chipSize*2);
        *inhibitStop = *inhibitStart + chipSize - 1;
    }
    if (memConf.inhibitBlock_[bank][1] && memConf.inhibitBlock_[bank][2])
    {
        *inhibitStart = *startAddress + chipSize - 1;
        *inhibitStop = *inhibitStart + (chipSize*2) - 1;
    }
    
    if (memConf.inhibitBlock_[bank][0])
    {
        *startAddress += chipSize;
        *socketSize -= chipSize;
    }
    if (memConf.inhibitBlock_[bank][3])
        *socketSize -= chipSize;
}

void GuiCdp18s600::checkBoardType(Conf* config, int card, wxString cardstring, int oldCard, wxString oldStr, bool boardControlValue)
{
    if (boardControlValue == true)
    {
        config->errorDoubleBoard_[card] = "Error: Same board as in slot "+oldStr+", only 1 " + conf[MICROBOARD].microboardTypeStr_[card] + " supported";
        if (oldCard == 0)
            config->errorDoubleBoard_[card] = "Error: UART included in CPU board, only 1 UART supported";
        
        if (cardstring.Left(1) == "0")
            cardstring = cardstring.Right(1);
        
        if (oldCard > 0)
            config->errorDoubleBoard_[oldCard] = "Error: Same board as in slot "+cardstring+" only 1 " + conf[MICROBOARD].microboardTypeStr_[oldCard] + " supported";
    }
}

void GuiCdp18s600::setButtonColor(wxString cardstring, bool error)
{
    wxColour colour;
#ifdef __WXMAC__
    if (!error)
    {
        colour = XRCCTRL(*this, "Card1Choice" + computerInfo[MICROBOARD].gui, wxChoice)->GetBackgroundColour();
        XRCCTRL(*this, "Card" + cardstring + computerInfo[MICROBOARD].gui, wxButton)->SetBackgroundColour(colour);
    }
    else
        XRCCTRL(*this, "Card" + cardstring + computerInfo[MICROBOARD].gui, wxButton)->SetBackgroundColour(*wxRED);
#else
    if (!error)
    {
        colour = XRCCTRL(*this, "Card1Choice" + computerInfo[MICROBOARD].gui, wxChoice)->GetForegroundColour();
        XRCCTRL(*this, "Card" + cardstring + computerInfo[MICROBOARD].gui, wxButton)->SetForegroundColour(colour);
    }
    else
        XRCCTRL(*this, "Card" + cardstring + computerInfo[MICROBOARD].gui, wxButton)->SetForegroundColour(*wxRED);
#endif
    XRCCTRL(*this, "Card" + cardstring + computerInfo[MICROBOARD].gui, wxButton)->Refresh();
}

void GuiCdp18s600::setTapeGui()
{
    XRCCTRL(*this, "WavFileMicroboard", wxTextCtrl)->Enable(elfConfiguration[MICROBOARD].useTape);
    XRCCTRL(*this, "WavFile1Microboard", wxTextCtrl)->Enable(elfConfiguration[MICROBOARD].useTape);

    XRCCTRL(*this, "CasButtonMicroboard", wxButton)->Enable(elfConfiguration[MICROBOARD].useTape);
    XRCCTRL(*this, "CasButton1Microboard", wxButton)->Enable(elfConfiguration[MICROBOARD].useTape);

    XRCCTRL(*this, "EjectCasMicroboard", wxBitmapButton)->Enable(elfConfiguration[MICROBOARD].useTape);
    XRCCTRL(*this, "EjectCas1Microboard", wxBitmapButton)->Enable(elfConfiguration[MICROBOARD].useTape);
    
    XRCCTRL(*this, "TurboMicroboard", wxCheckBox)->Enable(elfConfiguration[MICROBOARD].useTape);
    XRCCTRL(*this, "AutoCasLoadMicroboard", wxCheckBox)->Enable(elfConfiguration[MICROBOARD].useTape);
    XRCCTRL(*this, "TurboClockMicroboard", wxTextCtrl)->Enable(elfConfiguration[MICROBOARD].useTape);
    XRCCTRL(*this, "TurboMhzTextMicroboard", wxStaticText)->Enable(elfConfiguration[MICROBOARD].useTape);

}

void GuiCdp18s600::onPioWindows(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].usePio = event.IsChecked();
    
    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddress" + computerInfo[selectedComputer_].gui,wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].usePio | elfConfiguration[selectedComputer_].usePioWindow1Cdp18s660 | elfConfiguration[selectedComputer_].usePioWindow2Cdp18s660);
        XRCCTRL(*this,"AddressText1" + computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].usePio | elfConfiguration[selectedComputer_].usePioWindow1Cdp18s660 | elfConfiguration[selectedComputer_].usePioWindow2Cdp18s660);
        XRCCTRL(*this,"AddressText2" + computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].usePio | elfConfiguration[selectedComputer_].usePioWindow1Cdp18s660 | elfConfiguration[selectedComputer_].usePioWindow2Cdp18s660);
    }
    
    if (runningComputer_ == selectedComputer_)
        p_Computer->showPio(elfConfiguration[selectedComputer_].usePio);
}

void GuiCdp18s600::pioWindows(int computerType, bool state)
{
    elfConfiguration[computerType].usePio = state;
    XRCCTRL(*this, "Pio" + computerInfo[computerType].gui, wxCheckBox)->SetValue(elfConfiguration[computerType].usePio);
}

void GuiCdp18s600::cdp18660WindowPio1(int computerType, bool state)
{
    elfConfiguration[computerType].usePioWindow1Cdp18s660 = state;
}

void GuiCdp18s600::cdp18660WindowPio2(int computerType, bool state)
{
    elfConfiguration[computerType].usePioWindow2Cdp18s660 = state;
}

bool GuiCdp18s600::getUseCdp18s600ControlWindows()
{
    return elfConfiguration[selectedComputer_].useElfControlWindows;
}

bool GuiCdp18s600::getUsePioWindows()
{
    return elfConfiguration[selectedComputer_].usePio;
}

void GuiCdp18s600::onAutoBoot(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].autoBoot = event.IsChecked();
}

void GuiCdp18s600::onAutoBootType(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].autoBootType = event.GetSelection();
    if (elfConfiguration[selectedComputer_].autoBootType == 0)
        conf[selectedComputer_].bootAddress_ = 0x8000;
    else
        conf[selectedComputer_].bootAddress_ = 0;
    
//    if (runningComputer_ == selectedComputer_)
//        p_Computer->setAddressLatch(conf[selectedComputer_].bootAddress_);
}

wxString GuiCdp18s600::getMicroboardTypeStr(int boardType)
{
    return microBoardStr[boardType];
}



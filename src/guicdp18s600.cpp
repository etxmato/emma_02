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
    "CDP18S610"
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
	// new card etc..
};

BEGIN_EVENT_TABLE(GuiCdp18s600, GuiTMC2000)

    EVT_BUTTON(XRCID("Card1Microboard"), GuiCdp18s600::onMicroboardCard1Setup)
    EVT_BUTTON(XRCID("Card2Microboard"), GuiCdp18s600::onMicroboardCard2Setup)
    EVT_BUTTON(XRCID("Card3Microboard"), GuiCdp18s600::onMicroboardCard3Setup)
    EVT_BUTTON(XRCID("Card4Microboard"), GuiCdp18s600::onMicroboardCard4Setup)
    EVT_BUTTON(XRCID("AdditionalCardsMicroboard"), GuiCdp18s600::onMicroboardCardSetup)
    EVT_CHOICE(XRCID("Card1ChoiceMicroboard"), GuiCdp18s600::onMicroboardType1)
    EVT_CHOICE(XRCID("Card2ChoiceMicroboard"), GuiCdp18s600::onMicroboardType2)
    EVT_CHOICE(XRCID("Card3ChoiceMicroboard"), GuiCdp18s600::onMicroboardType3)
    EVT_CHOICE(XRCID("Card4ChoiceMicroboard"), GuiCdp18s600::onMicroboardType4)
    EVT_CHOICE(XRCID("CardMaxMicroboard"), GuiCdp18s600::onMicroboardCard)

    EVT_TEXT(XRCID("VtCharRomMicroboard"), GuiMain::onVtCharRomText)
    EVT_COMBOBOX(XRCID("VtCharRomMicroboard"), GuiMain::onVtCharRomText)
    EVT_BUTTON(XRCID("VtCharRomButtonMicroboard"), GuiMain::onVtCharRom)

    EVT_TEXT(XRCID("KeyFileMicroboard"), GuiMain::onKeyFileText)
    EVT_BUTTON(XRCID("KeyFileButtonMicroboard"), GuiMain::onKeyFile)
    EVT_BUTTON(XRCID("EjectKeyFileMicroboard"), GuiMain::onKeyFileEject)

    EVT_CHOICE(XRCID("VTTypeMicroboard"), GuiMain::onVT100)
    EVT_SPIN_UP(XRCID("ZoomSpinVtMicroboard"), GuiMain::onZoomUpVt)
    EVT_SPIN_DOWN(XRCID("ZoomSpinVtMicroboard"), GuiMain::onZoomDownVt)
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
    position_.x = 0;
}

void GuiCdp18s600::readCdp18s600Config()
{
    wxString cardNumberStr;
    
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
    
    conf[MICROBOARD].saveStartString_ = "";
    conf[MICROBOARD].saveEndString_ = "";
    conf[MICROBOARD].saveExecString_ = "";
    
    conf[MICROBOARD].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Microboard" + pathSeparator_;
    
    conf[MICROBOARD].mainDir_ = readConfigDir("/Dir/Microboard/Main", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].romDir_[U21ROM] = readConfigDir("/Dir/Microboard/Main_Rom_File21", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].romDir_[U20ROM] = readConfigDir("/Dir/Microboard/Main_Rom_File20", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].romDir_[U19ROM] = readConfigDir("/Dir/Microboard/Main_Rom_File19", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].romDir_[U18ROM] = readConfigDir("/Dir/Microboard/Main_Rom_File18", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].romDir_[U17ROM] = readConfigDir("/Dir/Microboard/Main_Rom_File17", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].vtCharRomDir_ = readConfigDir("/Dir/Microboard/Vt_Font_Rom_File", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].keyFileDir_ = readConfigDir("/Dir/Microboard/Key_File", dataDir_ + "Microboard" + pathSeparator_);
    conf[MICROBOARD].screenDumpFileDir_ = readConfigDir("/Dir/Microboard/Video_Dump_File", dataDir_ + "Microboard" + pathSeparator_);
    
    conf[MICROBOARD].keyFile_ = configPointer->Read("/Microboard/Key_File", "");
    conf[MICROBOARD].screenDumpFile_ = configPointer->Read("/Microboard/Video_Dump_File", "screendump.png");
    
    getConfigBool("/Microboard/SerialLog", false);
    
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

    elfConfiguration[MICROBOARD].vtType = (int)configPointer->Read("/Microboard/VT_Type", 2l);
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

    conf[MICROBOARD].microboardMaxCards_ = (int)configPointer->Read("/Microboard/MicroboardMaxCards", 4l);
    microMemConf.resize(conf[MICROBOARD].microboardMaxCards_);
    
    setCardMax();

    elfConfiguration[MICROBOARD].baudR = (int)configPointer->Read("/Microboard/Vt_Baud_Receive", 0l);
    elfConfiguration[MICROBOARD].baudT = (int)configPointer->Read("/Microboard/Vt_Baud_Transmit", 0l);
    defaultClock.Printf("%1.4f", 4.9152);

    conf[MICROBOARD].rom_[U21ROM] = configPointer->Read("/Microboard/Main_Rom_File21", "");
    conf[MICROBOARD].rom_[U20ROM] = configPointer->Read("/Microboard/Main_Rom_File20", "");
    conf[MICROBOARD].rom_[U19ROM] = configPointer->Read("/Microboard/Main_Rom_File19", "");
    conf[MICROBOARD].rom_[U18ROM] = configPointer->Read("/Microboard/Main_Rom_File18", "");
    conf[MICROBOARD].rom_[U17ROM] = configPointer->Read("/Microboard/Main_Rom_File17", "");

    conf[MICROBOARD].microChipType_[ONE_SOCKET] = (int)configPointer->Read("/Microboard/MicroChipTypeOneSocket", 2l);
    conf[MICROBOARD].microChipType_[FOUR_SOCKET] = (int)configPointer->Read("/Microboard/MicroChipTypeFourSocket", 2l);

    conf[MICROBOARD].microChipMemory_[U21ROM] = (int)configPointer->Read("/Microboard/MicroChipMemoryU21", 1l);
    configPointer->Read("/Microboard/MicroChiDisableU21", &conf[MICROBOARD].microChipDisable_[U21ROM], false);
    
    conf[MICROBOARD].microChipMemory_[U20ROM] = (int)configPointer->Read("/Microboard/MicroChipMemoryU20", 0l);
    configPointer->Read("/Microboard/MicroChiDisableU20", &conf[MICROBOARD].microChipDisable_[U20ROM], false);
    
    conf[MICROBOARD].microChipMemory_[U19ROM] = (int)configPointer->Read("/Microboard/MicroChipMemoryU19", 1l);
    configPointer->Read("/Microboard/MicroChiDisableU19", &conf[MICROBOARD].microChipDisable_[U19ROM], false);
    
    conf[MICROBOARD].microChipMemory_[U18ROM] = (int)configPointer->Read("/Microboard/MicroChipMemoryU18", 0l);
    configPointer->Read("/Microboard/MicroChiDisableU18", &conf[MICROBOARD].microChipDisable_[U18ROM], false);
    
    conf[MICROBOARD].microChipMemory_[U17ROM] = (int)configPointer->Read("/Microboard/MicroChipMemoryU17", 0l);
    configPointer->Read("/Microboard/MicroChiDisableU17", &conf[MICROBOARD].microChipDisable_[U17ROM], false);

    conf[MICROBOARD].microChipLocation_[ONE_SOCKET] = (int)configPointer->Read("/Microboard/MicroChipLocationOneSocket", 20l);
    conf[MICROBOARD].microChipLocation_[FOUR_SOCKET_ROM1] = (int)configPointer->Read("/Microboard/MicroChipLocationRom1Socket", 0l);
    conf[MICROBOARD].microChipLocation_[FOUR_SOCKET_ROM2] = (int)configPointer->Read("/Microboard/MicroChipLocationRom2Socket", 8l);

    defaultBlock.Printf("%d", 16);
    conf[MICROBOARD].microChipBlock_[ONE_SOCKET] = configPointer->Read("/Microboard/MicroBlockOneSocket", defaultBlock);
    
    defaultBlock.Printf("%d", 0);
    conf[MICROBOARD].microChipBlock_[FOUR_SOCKET] = configPointer->Read("/Microboard/MicroBlockFourSocket", defaultBlock);
    
    configPointer->Read("/Microboard/Pio_Windows", &elfConfiguration[MICROBOARD].usePio, true);

    conf[MICROBOARD].microboardType_[1] = cardBoardId[(int)configPointer->Read("/Microboard/MicroboardType0", 0l)];
    conf[MICROBOARD].microboardType_[2] = cardBoardId[(int)configPointer->Read("/Microboard/MicroboardType9", 9l)];
    
    microMemConf[0].rom_[MEM_SECTION1] = configPointer->Read("/Microboard/Card0RomFile1", "ut71.bin");

    for (int card=0; card<conf[MICROBOARD].microboardMaxCards_; card++)
    {
        cardNumberStr.Printf("%d",card);
        if (card != 0 && card != 1)
            conf[MICROBOARD].microboardType_[card+1] = cardBoardId[(int)configPointer->Read("/Microboard/MicroboardType"+ cardNumberStr, 0l)];
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
        configPointer->Read("/Microboard/Card" + cardNumberStr + "/MemInhibitBlock00", &microMemConf[card].inhibitBlock_[0][0], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr + "/MemInhibitBlock01", &microMemConf[card].inhibitBlock_[0][1], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr + "/MemInhibitBlock02", &microMemConf[card].inhibitBlock_[0][2], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr + "/MemInhibitBlock03", &microMemConf[card].inhibitBlock_[0][3], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr + "/MemInhibitBlock10", &microMemConf[card].inhibitBlock_[1][0], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr + "/MemInhibitBlock11", &microMemConf[card].inhibitBlock_[1][1], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr + "/MemInhibitBlock12", &microMemConf[card].inhibitBlock_[1][2], false);
        configPointer->Read("/Microboard/Card" + cardNumberStr + "/MemInhibitBlock13", &microMemConf[card].inhibitBlock_[1][3], false);
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
    conf[MICROBOARD].zoom_ = configPointer->Read("/Microboard/Zoom", defaultZoom);
    conf[MICROBOARD].volume_ = (int)configPointer->Read("/Microboard/Volume", 25l);
    elfConfiguration[MICROBOARD].pageMemSize = (int)configPointer->Read("/Microboard/PageMemSize", 0l);
    conf[MICROBOARD].charRom_ = configPointer->Read("/Microboard/Font_Rom_File", "character.bin");
    elfConfiguration[MICROBOARD].v1870InterruptMode = (int)configPointer->Read("/Microboard/V1870InterruptMode", 1l);
    elfConfiguration[MICROBOARD].v1870VideoMode = (int)configPointer->Read("/Microboard/V1870VideoMode", 1l);
    conf[MICROBOARD].printMode_ = (int)configPointer->Read("/Microboard/Print_Mode", 1l);
    
    elfConfiguration[MICROBOARD].bellFrequency_ = (int)configPointer->Read("/Microboard/Bell_Frequency", 800);
    elfConfiguration[MICROBOARD].vt52SetUpFeature_ = configPointer->Read("/Microboard/VT52Setup", 0x00004092l);
    elfConfiguration[MICROBOARD].vt100SetUpFeature_ = configPointer->Read("/Microboard/VT100Setup", 0x0000cad2l);
    elfConfiguration[MICROBOARD].vtExternalSetUpFeature_ = configPointer->Read("/Microboard/VTExternalSetup", 0x0000cad2l);
    elfConfiguration[MICROBOARD].serialPort_ = configPointer->Read("/Microboard/VtSerialPortChoice", "");
    elfConfiguration[MICROBOARD].vtEf = false;
    elfConfiguration[MICROBOARD].vtQ = true;

    configPointer->Read("/Microboard/Force_Uppercase", &elfConfiguration[MICROBOARD].forceUpperCase, true);

    defaultZoom.Printf("%2.2f", 1.0);
    conf[MICROBOARD].zoomVt_ = configPointer->Read("/Microboard/Vt_Zoom", defaultZoom);
    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[MICROBOARD].xScale_ = configPointer->Read("/Microboard/Window_Scale_Factor_X", defaultScale);
    
    conf[MICROBOARD].clock_ = configPointer->Read("/Microboard/Clock_Speed", defaultClock);
    
    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[MICROBOARD].ledTime_ = configPointer->Read("/Microboard/Led_Update_Frequency", defaultTimer);
    
    conf[MICROBOARD].useLoadLocation_ = false;
    
    setVtType("Microboard", MICROBOARD, elfConfiguration[MICROBOARD].vtType, false);
    
    conf[MICROBOARD].vtCharRom_ = configPointer->Read("/Microboard/Vt_Font_Rom_File", "vt100.bin");
    
    configPointer->Read("/Microboard/Enable_Auto_Boot", &elfConfiguration[MICROBOARD].autoBoot, true);
    elfConfiguration[MICROBOARD].autoBootType = (int)configPointer->Read("/Microboard/AutoBootType", 0l);
    if (elfConfiguration[MICROBOARD].autoBootType == 0)
        conf[MICROBOARD].bootAddress_ = 0x8000;
    else
        conf[MICROBOARD].bootAddress_ = 0;

    if (mode_.gui)
    {
        XRCCTRL(*this, "VtCharRomMicroboard", wxComboBox)->SetValue(conf[MICROBOARD].vtCharRom_);
        XRCCTRL(*this, "KeyFileMicroboard", wxTextCtrl)->SetValue(conf[MICROBOARD].keyFile_);
        XRCCTRL(*this, "ScreenDumpFileMicroboard", wxComboBox)->SetValue(conf[MICROBOARD].screenDumpFile_);
        
        XRCCTRL(*this, "VTTypeMicroboard", wxChoice)->SetSelection(elfConfiguration[MICROBOARD].vtType);
        XRCCTRL(*this, "MicroboardForceUC", wxCheckBox)->SetValue(elfConfiguration[MICROBOARD].forceUpperCase);
        
        XRCCTRL(*this, "VTBaudRChoiceMicroboard", wxChoice)->SetSelection(elfConfiguration[MICROBOARD].baudR);
        XRCCTRL(*this, "VTBaudTChoiceMicroboard", wxChoice)->SetSelection(elfConfiguration[MICROBOARD].baudT);
        
        XRCCTRL(*this, "ZoomValueVtMicroboard", wxTextCtrl)->ChangeValue(conf[MICROBOARD].zoomVt_);
        XRCCTRL(*this, "StretchDotMicroboard", wxCheckBox)->SetValue(conf[MICROBOARD].stretchDot_);
        
        clockTextCtrl[MICROBOARD]->ChangeValue(conf[MICROBOARD].clock_);
        
        XRCCTRL(*this, "UseLocationMicroboard", wxCheckBox)->SetValue(conf[MICROBOARD].useLoadLocation_);
        
        for (int drive=0; drive < 4; drive++)
            setUpdFloppyGui(drive);

        XRCCTRL(*this, "CardMaxMicroboard", wxChoice)->SetSelection((conf[MICROBOARD].microboardMaxCards_/4)-1);

        XRCCTRL(*this, "Card1ChoiceMicroboard", wxChoice)->SetSelection(conf[MICROBOARD].microboardType_[1]);
        XRCCTRL(*this, "Card2ChoiceMicroboard", wxChoice)->SetSelection(conf[MICROBOARD].microboardType_[2]);
        XRCCTRL(*this, "Card3ChoiceMicroboard", wxChoice)->SetSelection(conf[MICROBOARD].microboardType_[3]);
        XRCCTRL(*this, "Card4ChoiceMicroboard", wxChoice)->SetSelection(conf[MICROBOARD].microboardType_[4]);

        XRCCTRL(*this, "TurboClockMicroboard", wxTextCtrl)->SetValue(conf[MICROBOARD].turboClock_);
        XRCCTRL(*this, "TurboMicroboard", wxCheckBox)->SetValue(conf[MICROBOARD].turbo_);
        turboGui("Microboard");
        XRCCTRL(*this, "AutoCasLoadMicroboard", wxCheckBox)->SetValue(conf[MICROBOARD].autoCassetteLoad_);

        XRCCTRL(*this, "WavFileMicroboard", wxTextCtrl)->SetValue(conf[MICROBOARD].wavFile_[0]);
        XRCCTRL(*this, "WavFile1Microboard", wxTextCtrl)->SetValue(conf[MICROBOARD].wavFile_[1]);
        setTapeGui();

        XRCCTRL(*this, "ZoomValueMicroboard", wxTextCtrl)->ChangeValue(conf[MICROBOARD].zoom_);

        XRCCTRL(*this, "PioMicroboard", wxCheckBox)->SetValue(elfConfiguration[MICROBOARD].usePio);

        XRCCTRL(*this, "ShowAddressMicroboard", wxTextCtrl)->ChangeValue(conf[MICROBOARD].ledTime_);
        XRCCTRL(*this,"ShowAddressMicroboard", wxTextCtrl)->Enable(elfConfiguration[MICROBOARD].useElfControlWindows);
        XRCCTRL(*this, "AutoBootMicroboard", wxCheckBox)->SetValue(elfConfiguration[MICROBOARD].autoBoot);
        XRCCTRL(*this, "AutoBootTypeMicroboard", wxChoice)->SetSelection(elfConfiguration[MICROBOARD].autoBootType);

        XRCCTRL(*this,"AddressText1Microboard", wxStaticText)->Enable(elfConfiguration[MICROBOARD].useElfControlWindows);
        XRCCTRL(*this,"AddressText2Microboard", wxStaticText)->Enable(elfConfiguration[MICROBOARD].useElfControlWindows);
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
    writeConfigDir("/Dir/Microboard/Vt_Font_Rom_File", conf[MICROBOARD].vtCharRomDir_);
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
        configPointer->Write("/Dir/Microboard/Card" + cardNumberStr+"RamFile0", microMemConf[card].ramDir_);
        configPointer->Write("/Dir/Microboard/Card" + cardNumberStr+"RomFile0", microMemConf[card].romDir_[MEM_SECTION0]);
        configPointer->Write("/Dir/Microboard/Card" + cardNumberStr+"RomFile1", microMemConf[card].romDir_[MEM_SECTION1]);
        configPointer->Write("/Dir/Microboard/Card" + cardNumberStr+"RomFile2", microMemConf[card].romDir_[MEM_SECTION2]);
        configPointer->Write("/Dir/Microboard/Card" + cardNumberStr+"RomFile3", microMemConf[card].romDir_[MEM_SECTION3]);
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
    configPointer->Write("/Microboard/Vt_Font_Rom_File", conf[MICROBOARD].vtCharRom_);
    configPointer->Write("/Microboard/Key_File", conf[MICROBOARD].keyFile_);
    configPointer->Write("/Microboard/Video_Dump_File", conf[MICROBOARD].screenDumpFile_);
    configPointer->Write("/Microboard/VtSerialPortChoice", elfConfiguration[MICROBOARD].serialPort_);
    
    configPointer->Write("/Microboard/Bell_Frequency", elfConfiguration[MICROBOARD].bellFrequency_);
    configPointer->Write("/Microboard/VT_Type", elfConfiguration[MICROBOARD].vtType);
    
    long value = elfConfiguration[MICROBOARD].vt52SetUpFeature_.to_ulong();
    configPointer->Write("/Microboard/VT52Setup", value);
    value = elfConfiguration[MICROBOARD].vt100SetUpFeature_.to_ulong();
    configPointer->Write("/Microboard/VT100Setup", value);
    value = elfConfiguration[MICROBOARD].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("/Microboard/VTExternalSetup", value);
    configPointer->Write("/Microboard/Uart", elfConfiguration[MICROBOARD].useUart);

    configPointer->Write("/Microboard/Vt_Baud_Receive", elfConfiguration[MICROBOARD].baudR);
    configPointer->Write("/Microboard/Vt_Baud_Transmit", elfConfiguration[MICROBOARD].baudT);
    configPointer->Write("/Microboard/Vt_Zoom", conf[MICROBOARD].zoomVt_);
    configPointer->Write("/Microboard/Force_Uppercase", elfConfiguration[MICROBOARD].forceUpperCase);
    configPointer->Write("/Microboard/Enable_Vt_Stretch_Dot", conf[MICROBOARD].stretchDot_);
    configPointer->Write("/Microboard/Enable_Vt_External", elfConfiguration[MICROBOARD].vtExternal);
    configPointer->Write("/Microboard/Led_Update_Frequency", conf[MICROBOARD].ledTime_);


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

    configPointer->Write("/Microboard/MicroboardMaxCards", conf[MICROBOARD].microboardMaxCards_);

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


    for (int card=0; card<conf[MICROBOARD].microboardMaxCards_; card++)
    {
        cardNumberStr.Printf("%d",card);
        configPointer->Write("/Microboard/MicroboardType"+cardNumberStr, cardBoardNumber[conf[MICROBOARD].microboardType_[card+1]]);
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

    configPointer->Write("/Microboard/Upd765Group", elfConfiguration[MICROBOARD].upd765Group);
    configPointer->Write("/Microboard/PrinterGroup", elfConfiguration[MICROBOARD].printerGroup);
    configPointer->Write("/Microboard/UartGroup", elfConfiguration[MICROBOARD].uartGroup);
    configPointer->Write("/Microboard/UartOut", elfConfiguration[MICROBOARD].elfPortConf.uartOut);
    configPointer->Write("/Microboard/UartControl", elfConfiguration[MICROBOARD].elfPortConf.uartControl);
    configPointer->Write("/Microboard/Pio_Windows", elfConfiguration[MICROBOARD].usePio);
    configPointer->Write("/Microboard/V1870Group", elfConfiguration[MICROBOARD].v1870Group);
    configPointer->Write("/Microboard/Pio1Group", elfConfiguration[MICROBOARD].cdp18s660Group1);
    configPointer->Write("/Microboard/Pio2Group", elfConfiguration[MICROBOARD].cdp18s660Group2);

    configPointer->Write("/Microboard/Enable_Auto_Boot", elfConfiguration[MICROBOARD].autoBoot);
    configPointer->Write("/Microboard/AutoBootType", elfConfiguration[MICROBOARD].autoBootType);

    configPointer->Write("/Microboard/Clock_Speed", conf[MICROBOARD].clock_);
}

void GuiCdp18s600::readCdp18s600WindowConfig()
{
    conf[MICROBOARD].vtX_ = (int)configPointer->Read("/Microboard/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX);
    conf[MICROBOARD].vtY_ = (int)configPointer->Read("/Microboard/Window_Position_Vt_Y", mainWindowY_);
    conf[MICROBOARD].mainX_ = (int)configPointer->Read("/Microboard/Window_Position_X", mainWindowX_);
    conf[MICROBOARD].mainY_ = (int)configPointer->Read("/Microboard/Window_Position_Y", mainWindowY_+windowInfo.mainwY);
    conf[MICROBOARD].secondFrameX_ = (int)configPointer->Read("/Microboard/Window_Position_SecondFrame_X", mainWindowX_ + 310);
    conf[MICROBOARD].secondFrameY_ = (int)configPointer->Read("/Microboard/Window_Position_SecondFrame_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    conf[MICROBOARD].thirdFrameX_ = (int)configPointer->Read("/Microboard/Window_Position_ThirdFrame_X", mainWindowX_ + 610);
    conf[MICROBOARD].thirdFrameY_ = (int)configPointer->Read("/Microboard/Window_Position_ThirdFrame_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    conf[MICROBOARD].fourthFrameX_ = (int)configPointer->Read("/Microboard/Window_Position_FourthFrame_X", mainWindowX_ + 910);
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
}

void GuiCdp18s600::onMicroboardCard2Setup(wxCommandEvent&WXUNUSED(event))
{
    MicroboardCardSetupDialog MicroboardCardSetupDialog(this, conf[selectedComputer_], elfConfiguration[selectedComputer_], microMemConf[0], 2);
    MicroboardCardSetupDialog.ShowModal();
}

void GuiCdp18s600::onMicroboardCard3Setup(wxCommandEvent&WXUNUSED(event))
{
    MicroboardCardSetupDialog MicroboardCardSetupDialog(this, conf[selectedComputer_], elfConfiguration[selectedComputer_], microMemConf[1], 3);
    MicroboardCardSetupDialog.ShowModal();
}

void GuiCdp18s600::onMicroboardCard4Setup(wxCommandEvent&WXUNUSED(event))
{
    MicroboardCardSetupDialog MicroboardCardSetupDialog(this, conf[selectedComputer_], elfConfiguration[selectedComputer_], microMemConf[2], 4);
    MicroboardCardSetupDialog.ShowModal();
}

void GuiCdp18s600::onMicroboardCardSetup(wxCommandEvent&WXUNUSED(event))
{
    MicroboardAdditionalCardSetupDialog MicroboardAdditionalCardSetupDialog(this, conf[selectedComputer_], elfConfiguration[selectedComputer_]);
    MicroboardAdditionalCardSetupDialog.ShowModal();

    setCardType();
}

void GuiCdp18s600::onMicroboardType1(wxCommandEvent&event)
{
    conf[selectedComputer_].microboardType_[1] = event.GetSelection();
    setCardType();
}

void GuiCdp18s600::onMicroboardType2(wxCommandEvent&event)
{
    conf[selectedComputer_].microboardType_[2] = event.GetSelection();
    setCardType();
}

void GuiCdp18s600::onMicroboardType3(wxCommandEvent&event)
{
    conf[selectedComputer_].microboardType_[3] = event.GetSelection();
    setCardType();
}

void GuiCdp18s600::onMicroboardType4(wxCommandEvent&event)
{
    conf[selectedComputer_].microboardType_[4] = event.GetSelection();
    setCardType();
}

void GuiCdp18s600::onMicroboardCard(wxCommandEvent&event)
{
    conf[selectedComputer_].microboardMaxCards_ = (event.GetSelection() + 1) * 4;
    microMemConf.resize(conf[selectedComputer_].microboardMaxCards_);

    setCardMax();
}

void GuiCdp18s600::setCardMax()
{
    wxString label;
    if (conf[selectedComputer_].microboardMaxCards_ == 4)
        label = "";
    else
        label.Printf("C 5-%d", conf[selectedComputer_].microboardMaxCards_);
    XRCCTRL(*this, "AdditionalCards" + computerInfo[selectedComputer_].gui, wxButton)->SetLabel(label);
    XRCCTRL(*this, "AdditionalCards" + computerInfo[selectedComputer_].gui, wxButton)->Enable(conf[selectedComputer_].microboardMaxCards_ != 4);
}

void GuiCdp18s600::setCardType()
{
    wxString cardStr;
    int vtType = elfConfiguration[selectedComputer_].vtType;
    
    elfConfiguration[selectedComputer_].useUart = false;
    if (mode_.gui)
    {
        XRCCTRL(*this, "Pio" + computerInfo[selectedComputer_].gui, wxCheckBox)->Enable(true);
        XRCCTRL(*this, "VTType" + computerInfo[selectedComputer_].gui, wxChoice)->Enable(true);
        XRCCTRL(*this, "ZoomText" + computerInfo[selectedComputer_].gui, wxStaticText)->Enable(false);
        XRCCTRL(*this, "ZoomValue" + computerInfo[selectedComputer_].gui, wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "ZoomSpin" + computerInfo[selectedComputer_].gui, wxSpinButton)->Enable(false);
   }
    switch (conf[selectedComputer_].microboardType_[1])
    {
        case MICROBOARD_CDP18S600:
        case MICROBOARD_CDP18S602:
        case MICROBOARD_CDP18S607:
            elfConfiguration[selectedComputer_].useUart = true;
			elfConfiguration[selectedComputer_].elfPortConf.uartOut = 2;
			elfConfiguration[selectedComputer_].elfPortConf.uartControl = 3;
        break;
            
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S603:
        case MICROBOARD_CDP18S603A:
        case MICROBOARD_CDP18S606:
        case MICROBOARD_CDP18S608:
            elfConfiguration[selectedComputer_].useUart = false;
        break;

        case MICROBOARD_CDP18S604B:
        case MICROBOARD_CDP18S609:
            elfConfiguration[selectedComputer_].vtType = 0;
            elfConfiguration[selectedComputer_].useUart = false;
            if (mode_.gui)
            {
                XRCCTRL(*this, "VTType" + computerInfo[selectedComputer_].gui, wxChoice)->Enable(false);
                XRCCTRL(*this, "VTType" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VTNONE);
            }
        break;

        case MICROBOARD_CDP18S605:
        case MICROBOARD_CDP18S610:
            elfConfiguration[selectedComputer_].useUart = true;
			elfConfiguration[selectedComputer_].elfPortConf.uartOut = 2;
			elfConfiguration[selectedComputer_].elfPortConf.uartControl = 3;
            if (mode_.gui)
                XRCCTRL(*this, "Pio" + computerInfo[selectedComputer_].gui, wxCheckBox)->Enable(false);
        break;
    }

    elfConfiguration[selectedComputer_].useElfControlWindows = false;
    elfConfiguration[selectedComputer_].useUpd765 = false;
    elfConfiguration[selectedComputer_].useTape = false;
    elfConfiguration[selectedComputer_].usev1870 = false;
    elfConfiguration[selectedComputer_].useCdp18s660 = false;
    conf[selectedComputer_].printerOn_ = false;

    wxString controlWindowCardStr="", uartCardStr="", printerCardStr="", upd765CardStr="", tapeCardStr="", v1870CardStr="", pioCardStr="";

    for (int card=2; card<=conf[selectedComputer_].microboardMaxCards_; card++)
    {
        cardStr.Printf("%d", card);
        
        if (mode_.gui && card <=4)
        {
            XRCCTRL(*this, "Card"+cardStr + computerInfo[selectedComputer_].gui, wxButton)->Enable(true);
            setChoiceColor(cardStr, false);
        }
        microMemConf[card-2].useCdp18s620_ = false;
        microMemConf[card-2].useCdp18s621_ = false;
        microMemConf[card-2].useCdp18s623a_ = false;
        microMemConf[card-2].useCdp18s625_ = false;
        microMemConf[card-2].useCdp18s626_ = false;
        microMemConf[card-2].useCdp18s627_ = false;
        microMemConf[card-2].useCdp18s628_ = false;
        microMemConf[card-2].useCdp18s629_ = false;
        microMemConf[card-2].useCdp18s640_ = false;
        microMemConf[card-2].useCdp18s652_ = false;
        microMemConf[card-2].useCdp18s660_ = false;
        microMemConf[card-2].useCdp18s661_ = false;

        switch (conf[selectedComputer_].microboardType_[card])
        {
            case CARD_EMPTY:
                if (mode_.gui && card <=4)
                    XRCCTRL(*this, "Card"+cardStr + computerInfo[selectedComputer_].gui, wxButton)->Enable(false);
            break;
                
            case CARD_CDP18S620:
                microMemConf[card-2].useCdp18s620_ = true;
            break;
                
            case CARD_CDP18S621:
                microMemConf[card-2].useCdp18s621_ = true;
                microMemConf[card-2].socketSize_[0] = 2;
                if (microMemConf[card-2].memLocation_[0] > 3)
                    microMemConf[card-2].memLocation_[0] = 0;
            break;
                
            case CARD_CDP18S623A:
                microMemConf[card-2].useCdp18s623a_ = true;
            break;
                
            case CARD_CDP18S625:
                microMemConf[card-2].useCdp18s625_ = true;
            break;
                
            case CARD_CDP18S626:
                microMemConf[card-2].useCdp18s626_ = true;
            break;
 
            case CARD_CDP18S627:
                microMemConf[card-2].useCdp18s627_ = true;
            break;

            case CARD_CDP18S628:
                microMemConf[card-2].useCdp18s628_ = true;
                microMemConf[card-2].socketSize_[0] = 0;
            break;

            case CARD_CDP18S629:
                microMemConf[card-2].useCdp18s629_ = true;
                microMemConf[card-2].socketSize_[0] = 0;
            break;
                
            case CARD_CDP18S640:
                controlWindowCardStr = checkBoardType(card, cardStr, controlWindowCardStr, elfConfiguration[selectedComputer_].useElfControlWindows);

                elfConfiguration[selectedComputer_].useElfControlWindows = true;
                microMemConf[card-2].useCdp18s640_ = true;
            break;

            case CARD_CDP18S641:
                uartCardStr = checkBoardType(card, cardStr, uartCardStr, elfConfiguration[selectedComputer_].useUart);

                elfConfiguration[selectedComputer_].useUart = true;
                elfConfiguration[selectedComputer_].vtType = vtType;
                if (mode_.gui)
                {
                    XRCCTRL(*this, "VTType" + computerInfo[selectedComputer_].gui, wxChoice)->Enable(true);
                    XRCCTRL(*this, "VTType" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(vtType);
                }
            break;
          
            case CARD_CDP18S646:
                printerCardStr = checkBoardType(card, cardStr, printerCardStr, conf[selectedComputer_].printerOn_);

                conf[selectedComputer_].printerOn_ = true;
            break;

            case CARD_CDP18S651:
                upd765CardStr = checkBoardType(card, cardStr, upd765CardStr, elfConfiguration[selectedComputer_].useUpd765);

                elfConfiguration[selectedComputer_].useUpd765 = true;
            break;

            case CARD_CDP18S652:
                tapeCardStr = checkBoardType(card, cardStr, tapeCardStr, elfConfiguration[selectedComputer_].useTape);

                microMemConf[card-2].useCdp18s652_ = true;
                elfConfiguration[selectedComputer_].useTape = true;
                microMemConf[card-2].socketSize_[0] = 0;
            break;
                
            case CARD_CDP18S660:
                pioCardStr = checkBoardType(card, cardStr, pioCardStr, elfConfiguration[selectedComputer_].useCdp18s660);

                elfConfiguration[selectedComputer_].useCdp18s660 = true;
                microMemConf[card-2].useCdp18s660_ = true;
            break;

            case CARD_CDP18S661B:
                v1870CardStr = checkBoardType(card, cardStr, v1870CardStr, elfConfiguration[selectedComputer_].usev1870);

                microMemConf[card-2].useCdp18s661_ = true;
                elfConfiguration[selectedComputer_].usev1870 = true;
                conf[selectedComputer_].videoMode_ = NTSC;
				if (mode_.gui)
				{
					XRCCTRL(*this, "ZoomText" + computerInfo[selectedComputer_].gui, wxStaticText)->Enable(true);
					XRCCTRL(*this, "ZoomValue" + computerInfo[selectedComputer_].gui, wxTextCtrl)->Enable(true);
					XRCCTRL(*this, "ZoomSpin" + computerInfo[selectedComputer_].gui, wxSpinButton)->Enable(true);
			   }
            break;

            case CARD_CDP18S661V3:
                v1870CardStr = checkBoardType(card, cardStr, v1870CardStr, elfConfiguration[selectedComputer_].usev1870);

                microMemConf[card-2].useCdp18s661_ = true;
                elfConfiguration[selectedComputer_].usev1870 = true;
                conf[selectedComputer_].videoMode_ = PAL;
				if (mode_.gui)
				{
					XRCCTRL(*this, "ZoomText" + computerInfo[selectedComputer_].gui, wxStaticText)->Enable(true);
					XRCCTRL(*this, "ZoomValue" + computerInfo[selectedComputer_].gui, wxTextCtrl)->Enable(true);
					XRCCTRL(*this, "ZoomSpin" + computerInfo[selectedComputer_].gui, wxSpinButton)->Enable(true);
                }
                if (elfConfiguration[selectedComputer_].v1870VideoMode > 2)
                    elfConfiguration[selectedComputer_].v1870VideoMode = 1;
            break;
        }
    }
  
    for (int drive=0; drive < 4; drive++)
        setUpdFloppyGui(drive);
    
    setTapeGui();
    setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, elfConfiguration[selectedComputer_].vtType, false);
}

wxString GuiCdp18s600::checkBoardType(int card, wxString cardstring, wxString oldStr, bool boardControlValue)
{
    wxString ret ="";
    
    if (boardControlValue == true)
    {
        if (card < 5)
            setChoiceColor(cardstring, true);
        if (oldStr != "")
            setChoiceColor(oldStr, true);
    }
    if (card < 5)
        ret = cardstring;
    
    return ret;
}

void GuiCdp18s600::setChoiceColor(wxString cardstring, bool error)
{
    wxColour colour;
#ifdef __WXMAC__
    if (!error)
    {
        colour = XRCCTRL(*this, "Card1Choice" + computerInfo[selectedComputer_].gui, wxChoice)->GetBackgroundColour();
        XRCCTRL(*this, "Card" + cardstring + "Choice" + computerInfo[selectedComputer_].gui, wxChoice)->SetBackgroundColour(colour);
    }
    else
        XRCCTRL(*this, "Card" + cardstring + "Choice" + computerInfo[selectedComputer_].gui, wxChoice)->SetBackgroundColour(*wxRED);
#else
    if (!error)
    {
        colour = XRCCTRL(*this, "Card1Choice" + computerInfo[selectedComputer_].gui, wxChoice)->GetForegroundColour();
        XRCCTRL(*this, "Card" + cardstring + "Choice" + computerInfo[selectedComputer_].gui, wxChoice)->SetForegroundColour(colour);
    }
    else
        XRCCTRL(*this, "Card" + cardstring + "Choice" + computerInfo[selectedComputer_].gui, wxChoice)->SetForegroundColour(*wxRED);
#endif
    XRCCTRL(*this, "Card" + cardstring + "Choice" + computerInfo[selectedComputer_].gui, wxChoice)->Refresh();
}

void GuiCdp18s600::setTapeGui()
{
    XRCCTRL(*this, "WavFileMicroboard", wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useTape);
    XRCCTRL(*this, "WavFile1Microboard", wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useTape);

    XRCCTRL(*this, "CasButtonMicroboard", wxButton)->Enable(elfConfiguration[selectedComputer_].useTape);
    XRCCTRL(*this, "CasButton1Microboard", wxButton)->Enable(elfConfiguration[selectedComputer_].useTape);

    XRCCTRL(*this, "EjectCasMicroboard", wxBitmapButton)->Enable(elfConfiguration[selectedComputer_].useTape);
    XRCCTRL(*this, "EjectCas1Microboard", wxBitmapButton)->Enable(elfConfiguration[selectedComputer_].useTape);
    
    XRCCTRL(*this, "TurboMicroboard", wxCheckBox)->Enable(elfConfiguration[selectedComputer_].useTape);
    XRCCTRL(*this, "AutoCasLoadMicroboard", wxCheckBox)->Enable(elfConfiguration[selectedComputer_].useTape);
    XRCCTRL(*this, "TurboClockMicroboard", wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useTape);
    XRCCTRL(*this, "TurboMhzTextMicroboard", wxStaticText)->Enable(elfConfiguration[selectedComputer_].useTape);

}

void GuiCdp18s600::onPioWindows(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].usePio = event.IsChecked();
    
    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddress" + computerInfo[selectedComputer_].gui,wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].usePio);
        XRCCTRL(*this,"AddressText1" + computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].usePio);
        XRCCTRL(*this,"AddressText2" + computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].usePio);
    }
    
    if (runningComputer_ == selectedComputer_)
        p_Computer->showPio(elfConfiguration[selectedComputer_].usePio);
}

void GuiCdp18s600::pioWindows(int computerType, bool state)
{
    elfConfiguration[computerType].usePio = state;
    XRCCTRL(*this, "Pio" + computerInfo[computerType].gui, wxCheckBox)->SetValue(elfConfiguration[computerType].usePio);
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


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
#include "wx/tglbtn.h"
#include "wx/utils.h"

#include "main.h"
#include "guicomx.h"
#include "printer.h"
#include "guieprom.h"
#include "guisb.h"
#include "guidiag.h"

DEFINE_EVENT_TYPE(STATUS_LED_ON)
DEFINE_EVENT_TYPE(STATUS_LED_OFF)
DEFINE_EVENT_TYPE(EXP_LED_ON)
DEFINE_EVENT_TYPE(EXP_LED_OFF)
DEFINE_EVENT_TYPE(STATUS_BAR_1870)

BEGIN_EVENT_TABLE(GuiComx, GuiElf)

	EVT_TEXT(XRCID("MainRomComx"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomComx"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonComx"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("ExpRomComx"), GuiComx::onExpansionRomText)
	EVT_COMBOBOX(XRCID("ExpRomComx"), GuiComx::onExpansionRomText)
	EVT_BUTTON(XRCID("ExpRomButtonComx"), GuiComx::onExpansionRom)

	EVT_TEXT(XRCID("Cart1RomComx"), GuiComx::onCard1RomText)
	EVT_COMBOBOX(XRCID("Cart1RomComx"), GuiComx::onCard1RomText)
	EVT_BUTTON(XRCID("Cart1RomButtonComx"), GuiComx::onCard1Rom)

	EVT_TEXT(XRCID("Cart2RomComx"), GuiComx::onCard2RomText)
	EVT_COMBOBOX(XRCID("Cart2RomComx"), GuiComx::onCard2RomText)
	EVT_BUTTON(XRCID("Cart2RomButtonComx"), GuiComx::onCard2Rom)

	EVT_TEXT(XRCID("Cart3RomComx"), GuiComx::onCard3RomText)
	EVT_COMBOBOX(XRCID("Cart3RomComx"), GuiComx::onCard3RomText)
	EVT_BUTTON(XRCID("Cart3RomButtonComx"), GuiComx::onCard3Rom)

	EVT_TEXT(XRCID("Cart4RomComx"), GuiComx::onCard4RomText)
	EVT_COMBOBOX(XRCID("Cart4RomComx"), GuiComx::onCard4RomText)
	EVT_BUTTON(XRCID("Cart4RomButtonComx"), GuiComx::onCard4Rom)

	EVT_BUTTON(XRCID("Disk1ButtonComx"), GuiComx::onComxDisk1)
	EVT_TEXT(XRCID("Disk1FileComx"), GuiComx::onComxDiskText1)
	EVT_BUTTON(XRCID("EjectDisk1Comx"), GuiComx::onComxDiskEject1)
	EVT_BUTTON(XRCID("Disk2ButtonComx"), GuiComx::onComxDisk2)
	EVT_TEXT(XRCID("Disk2FileComx"), GuiComx::onComxDiskText2)
	EVT_BUTTON(XRCID("EjectDisk2Comx"), GuiComx::onComxDiskEject2)
	EVT_BUTTON(XRCID("PrintFileButtonComx"), GuiMain::onPrintFile)
	EVT_TEXT(XRCID("PrintFileComx"), GuiComx::onComxPrintFileText)
	EVT_CHOICE(XRCID("PrintModeComx"), GuiComx::onComxPrintMode)
	EVT_BUTTON(XRCID("PrintButtonComx"), GuiComx::onComxPrintButton)

	EVT_BUTTON(XRCID("ScreenDumpFileButtonComx"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileComx"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileComx"), GuiMain::onScreenDumpFileText)

	EVT_BUTTON(XRCID("ScreenDumpF5Comx"), GuiMain::onScreenDump)
	EVT_BUTTON(XRCID("CasButtonComx"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasComx"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileComx"), GuiMain::onCassetteText)
	EVT_BUTTON(XRCID("KeyFileButtonComx"), GuiMain::onKeyFile)
	EVT_TEXT(XRCID("KeyFileComx"), GuiMain::onKeyFileText)
	EVT_BUTTON(XRCID("EjectKeyFileComx"), GuiMain::onKeyFileEject)
	EVT_CHOICE(XRCID("VidModeComx"), GuiComx::onComxVideoMode)
	EVT_SPIN_UP(XRCID("ZoomSpinComx"), GuiMain::onZoomUp)
	EVT_SPIN_DOWN(XRCID("ZoomSpinComx"), GuiMain::onZoomDown)
	EVT_TEXT(XRCID("ZoomValueComx"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3Comx"), GuiMain::onFullScreen)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeComx"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeComx"), GuiMain::onVolume)
	EVT_CHECKBOX(XRCID("InterlaceComx"), GuiMain::onInterlace)
	EVT_CHECKBOX(XRCID("ExpRamComx"), GuiComx::onComxExpansionRam)
	EVT_SPINCTRL(XRCID("ExpRamSlotComx"), GuiComx::onComxExpansionRamSlot)
	EVT_CHECKBOX(XRCID("UseLocationComx"), GuiMain::onUseLocation)

	EVT_TEXT(XRCID("SaveStartComx"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndComx"), GuiMain::onSaveEnd)
	EVT_TEXT(XRCID("SaveExecComx"), GuiMain::onSaveExec)

	EVT_BUTTON(XRCID("SaveButtonComx"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonComx"), GuiMain::onLoadButton)
	EVT_BUTTON(XRCID("RunButtonComx"), GuiMain::onLoadRunButton)
	EVT_BUTTON(XRCID("DsaveButtonComx"), GuiMain::onDataSaveButton)

	EVT_CHECKBOX(XRCID("TurboComx"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockComx"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadComx"), GuiMain::onAutoLoad)
	EVT_BUTTON(XRCID("CasLoadComx"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSaveComx"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopComx"), GuiMain::onCassetteStop)
	EVT_BUTTON(XRCID("RealCasLoadComx"), GuiMain::onRealCas)
	EVT_TOGGLEBUTTON(XRCID("ComxWavFile"), GuiMain::onWavFile)
	EVT_BUTTON(XRCID("ColoursComx"), Main::onColoursDef)

	EVT_COMMAND(wxID_ANY, OPEN_COMX_PRINTER_WINDOW, GuiComx::openComxPrinterFrame)
	EVT_COMMAND(wxID_ANY, STATUS_LED_ON, GuiComx::statusLedOn)
	EVT_COMMAND(wxID_ANY, STATUS_LED_OFF, GuiComx::statusLedOff)
	EVT_COMMAND(wxID_ANY, STATUS_BAR_1870, GuiComx::v1870BarSize)
	EVT_COMMAND(wxID_ANY, EXP_LED_ON, GuiComx::expLedOn)
	EVT_COMMAND(wxID_ANY, EXP_LED_OFF, GuiComx::expLedOff)

	EVT_BUTTON(XRCID("EpromComx"), GuiComx::onEpromDialog)
	EVT_BUTTON(XRCID("SbSetupComx"), GuiComx::onSBDialog)
	EVT_BUTTON(XRCID("DiagSetupComx"), GuiComx::onDiagDialog)
	EVT_CHECKBOX(XRCID("SbActiveComx"), GuiComx::onSbActive)
	EVT_CHECKBOX(XRCID("DiagActiveComx"), GuiComx::onDiagActive)
	EVT_CHOICE(XRCID("DiagOnComx"), GuiComx::onDiagOn)

    EVT_CHECKBOX(XRCID("LogComx"), GuiComx::onLogComx)

	END_EVENT_TABLE()

GuiComx::GuiComx(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir)
: GuiElf(title, pos, size, mode, dataDir)
{
	conf[COMX].loadFileNameFull_ = "";
	conf[COMX].loadFileName_ = "";

	conf[COMX].pLoadSaveName_[0] = 'C';
	conf[COMX].pLoadSaveName_[1] = 'O';
	conf[COMX].pLoadSaveName_[2] = 'M';
	conf[COMX].pLoadSaveName_[3] = 'X';

	conf[COMX].defus_ = 0x4281;
	conf[COMX].eop_ = 0x4283;
	conf[COMX].string_ = 0x4292;
	conf[COMX].arrayValue_ = 0x4294;
	conf[COMX].eod_ = 0x4299;
	conf[COMX].basicRamAddress_ = 0x4400;

	conf[COMX].saveStartString_ = "";
	conf[COMX].saveEndString_ = "";
	conf[COMX].saveExecString_ = "";
}

GuiComx::~GuiComx()
{
}

void GuiComx::readComxConfig()
{
	selectedComputer_ = COMX;

	configPointer->Read("/Comx/Enable_SB", &conf[COMX].sbActive_, true);
	configPointer->Read("/Comx/Enable_DIAG", &conf[COMX].diagActive_, false);
	conf[COMX].diagOn_ = (int)configPointer->Read("/Comx/Enable_DIAG_ON", 1l);

    conf[COMX].configurationDir_ = dataDir_ + "Configurations" + pathSeparator_ + "Comx" + pathSeparator_;

    conf[COMX].mainDir_ = readConfigDir("/Dir/Comx/Main", dataDir_ + "Comx" + pathSeparator_);

    conf[COMX].romDir_[MAINROM1] = readConfigDir("/Dir/Comx/Main_Rom_File", dataDir_ + "Comx" + pathSeparator_);
    conf[COMX].rom_[MAINROM1] = configPointer->Read("/Comx/Main_Rom_File", "comx35.1.1.bin");
    conf[COMX].romDir_[EXPROM] = readConfigDir("/Dir/Comx/Expansion_Rom_File", dataDir_ + "Comx" + pathSeparator_);
    conf[COMX].rom_[EXPROM] = configPointer->Read("/Comx/Expansion_Rom_File", "f&m.expansion.3.2.bin");
    conf[COMX].romDir_[CARTROM4] = readConfigDir("/Dir/Comx/Card_4_Rom_File", dataDir_ + "Comx" + pathSeparator_);
    conf[COMX].rom_[CARTROM4] = configPointer->Read("/Comx/Card_4_Rom_File", "f&m.eprom.board.1.2.bin");

    conf[COMX].romDir_[CARTROM1] = readConfigDir("/Dir/Comx/Card_1_Rom_File", dataDir_ + "Comx" + pathSeparator_);
	conf[COMX].romDir_[CARTROM2] = readConfigDir("/Dir/Comx/Card_2_Rom_File", dataDir_ + "Comx" + pathSeparator_);
	conf[COMX].romDir_[CARTROM3] = readConfigDir("/Dir/Comx/Card_3_Rom_File", dataDir_ + "Comx" + pathSeparator_);
	Pl80Data_[0] = readConfigDir("/Dir/Comx/PL80_Rom_Files", dataDir_ + "Comx" + pathSeparator_);
	conf[COMX].ramDir_ = readConfigDir("/Dir/Comx/Software_File", dataDir_ + "Comx" + pathSeparator_);
	floppyDir_[0] = readConfigDir("/Dir/Comx/Floppy_1_File", dataDir_ + "Comx" + pathSeparator_ + "Disks" + pathSeparator_);
	floppyDir_[1] = readConfigDir("/Dir/Comx/Floppy_2_File", dataDir_ + "Comx" + pathSeparator_ + "Disks" + pathSeparator_);
	conf[COMX].keyFileDir_ = readConfigDir("/Dir/Comx/Key_File", dataDir_ + "Comx" + pathSeparator_);
	conf[COMX].printFileDir_ = readConfigDir("/Dir/Comx/Print_File", dataDir_ + "Comx" + pathSeparator_);
	conf[COMX].screenDumpFileDir_ = readConfigDir("/Dir/Comx/Video_Dump_File", dataDir_ + "Comx" + pathSeparator_);
	conf[COMX].wavFileDir_ = readConfigDir("/Dir/Comx/Wav_File", dataDir_ + "Comx" + pathSeparator_);

	conf[COMX].rom_[CARTROM1] = configPointer->Read("/Comx/Card_1_Rom_File", "fdc.bin");
	conf[COMX].rom_[CARTROM2] = configPointer->Read("/Comx/Card_2_Rom_File", "f&m.printer.1.2.bin");
	conf[COMX].rom_[CARTROM3] = configPointer->Read("/Comx/Card_3_Rom_File", "80column.1.1.bin");
	Pl80Data_[1] = configPointer->Read("/Comx/PL80_Rom_File", "pl80.bin");
	Pl80Data_[2] = configPointer->Read("/Comx/PL80_Extension_Rom_File", "pl80.it.em.ou.bin");
	floppy_[0] = configPointer->Read("/Comx/Floppy_1_File", "dos.1.4+f&m.disk.tools.img");
	floppy_[1] = configPointer->Read("/Comx/Floppy_2_File", "f&m-heijmans.sw.img");
	conf[COMX].keyFile_ = configPointer->Read("/Comx/Key_File", "");
	conf[COMX].printFile_ = configPointer->Read("/Comx/Print_File", "printerout.txt");
	conf[COMX].screenDumpFile_ = configPointer->Read("/Comx/Video_Dump_File", "screendump.png");
	conf[COMX].wavFile_ = configPointer->Read("/Comx/Wav_File", "");

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[COMX].zoom_ = configPointer->Read("/Comx/Zoom", defaultZoom);
	wxString defaultClock;
	defaultClock.Printf("%1.3f", 2.813);
	comxPalClock_ = configPointer->Read("/Comx/Clock_Speed_When_Using_Pal", defaultClock);
	defaultClock.Printf("%1.3f", 2.835);
	comxNtscClock_ = configPointer->Read("/Comx/Clock_Speed_When_Using_Ntsc", defaultClock);
	conf[COMX].videoMode_ = (int)configPointer->Read("/Comx/Video_Mode", 0l);
	if (conf[COMX].videoMode_ == PAL)
		conf[COMX].clock_ = comxPalClock_;
	else
		conf[COMX].clock_ = comxNtscClock_;

	conf[COMX].turboClock_ = configPointer->Read("/Comx/Turbo_Clock_Speed", "15");
	conf[COMX].volume_ = (int)configPointer->Read("/Comx/Volume", 25l);
	expansionRamSlot_ = (int)configPointer->Read("/Comx/Ram_Card_Slot", 4l);
	comxPrintMode_ = (int)configPointer->Read("/Comx/Print_Mode", 1l);
	configPointer->Read("/Comx/Use_Ram_Card", &useExpansionRam_, false);
	configPointer->Read("/Comx/Video_Log", &conf[COMX].videoLog_, false);
	configPointer->Read("/Comx/Expansion_Rom_Loaded", &expansionRomLoaded_, true);
	configPointer->Read("/Comx/Enable_Turbo_Cassette", &conf[COMX].turbo_, true);
	configPointer->Read("/Comx/Enable_Auto_Cassette", &conf[COMX].autoCassetteLoad_, true);
	configPointer->Read("/Comx/Enable_Real_Cassette", &conf[COMX].realCassetteLoad_, false);
	configPointer->Read("/Comx/Enable_80_Column_Interlace", &conf[COMX].interlace_, true);
	configPointer->Read("/Comx/Disk_Rom_Loaded", &diskRomLoaded_, true);
	conf[COMX].useLoadLocation_ = false;

    wxString number;
    for (int i=0; i<5; i++)
    {
        number.Printf("%1d",i);
        EpromRomDir_[i] = readConfigDir("/Dir/Comx/Epromdir" + number, dataDir_ + "Comx" + pathSeparator_ + "F&M EPROM Switchboard" + pathSeparator_);
    }
    EpromRom_[0] = getConfigItem("/Comx/Eprom0", "f&m.utilities.bin");
    EpromRom_[1] = getConfigItem("/Comx/Eprom1", "disk.utilities.bin");
    EpromRom_[2] = getConfigItem("/Comx/Eprom2", "f&m.monitor.english.bin");
    EpromRom_[3] = getConfigItem("/Comx/Eprom3", "f&m.basic.bin");
    EpromRom_[4] = getConfigItem("/Comx/Eprom4", "tennismania.bin");
    
	readSbConfig();
	setRealCas(COMX);

	DiagRom_[0] = getConfigItem("/Comx/DIAG_ROM_0", "diag1.bin");
	DiagRom_[1] = getConfigItem("/Comx/DIAG_ROM_1", "diag2.bin");
	DiagRomDir_[0] = readConfigDir("/Dir/Comx/DIAG_ROM_0", dataDir_ + "Comx" + pathSeparator_);
	DiagRomDir_[1] = readConfigDir("/Dir/Comx/DIAG_ROM_1", dataDir_ + "Comx" + pathSeparator_);
    diagRomChecksum_ = (int)configPointer->Read("/Comx/DiagRomChecksum", 1l);
    diagFactory_ = (int)configPointer->Read("/Comx/DiagFactoryUnit", 1l);
    diagCassetteCables_ = (int)configPointer->Read("/Comx/DiagCassetteCables", 1l);

/*    if (conf[COMX].diagActive_)
	{
		conf[COMX].romDir_[EXPROM] = DiagRomDir_[0];
		conf[COMX].rom_[EXPROM] = DiagRom_[0];
        conf[COMX].romDir_[CARTROM1] = DiagRomDir_[1];
		conf[COMX].rom_[CARTROM1] = DiagRom_[1];
	}*/
	diagSbChange();

	if (mode_.gui)
	{
		XRCCTRL(*this, "TurboClockComx", wxTextCtrl)->SetValue(conf[COMX].turboClock_);
		XRCCTRL(*this, "VolumeComx", wxSlider)->SetValue(conf[COMX].volume_);

		XRCCTRL(*this, "MainRomComx", wxComboBox)->SetValue(conf[COMX].rom_[MAINROM1]);
		XRCCTRL(*this, "ExpRomComx", wxComboBox)->SetValue(conf[COMX].rom_[EXPROM]);
		XRCCTRL(*this, "Cart1RomComx", wxComboBox)->SetValue(conf[COMX].rom_[CARTROM1]);
		XRCCTRL(*this, "Cart2RomComx", wxComboBox)->SetValue(conf[COMX].rom_[CARTROM2]);
		XRCCTRL(*this, "Cart3RomComx", wxComboBox)->SetValue(conf[COMX].rom_[CARTROM3]);
		XRCCTRL(*this, "Cart4RomComx", wxComboBox)->SetValue(conf[COMX].rom_[CARTROM4]);
		XRCCTRL(*this, "Disk1FileComx", wxTextCtrl)->SetValue(floppy_[0]);
		XRCCTRL(*this, "Disk2FileComx", wxTextCtrl)->SetValue(floppy_[1]);
		XRCCTRL(*this, "KeyFileComx", wxTextCtrl)->SetValue(conf[COMX].keyFile_);
		XRCCTRL(*this, "PrintFileComx", wxTextCtrl)->SetValue(conf[COMX].printFile_);
		XRCCTRL(*this, "ScreenDumpFileComx", wxComboBox)->SetValue(conf[COMX].screenDumpFile_);

		XRCCTRL(*this, "TurboComx", wxCheckBox)->SetValue(conf[COMX].turbo_);
		turboGui("Comx");
		XRCCTRL(*this, "AutoCasLoadComx", wxCheckBox)->SetValue(conf[COMX].autoCassetteLoad_);

		XRCCTRL(*this, "InterlaceComx", wxCheckBox)->SetValue(conf[COMX].interlace_);
		XRCCTRL(*this, "SbActiveComx", wxCheckBox)->SetValue(conf[COMX].sbActive_);
		XRCCTRL(*this, "DiagActiveComx", wxCheckBox)->SetValue(conf[COMX].diagActive_);
		XRCCTRL(*this, "DiagOnComx", wxChoice)->SetSelection(conf[COMX].diagOn_);

        XRCCTRL(*this, "LogComx", wxCheckBox)->SetValue(conf[COMX].videoLog_);

		XRCCTRL(*this, "ExpRamComx", wxCheckBox)->SetValue(useExpansionRam_);
		XRCCTRL(*this, "ExpRamSlotComx", wxSpinCtrl)->SetValue(expansionRamSlot_);

		XRCCTRL(*this, "Cart2RomButtonComx", wxButton)->Enable(expansionRomLoaded_ );
		XRCCTRL(*this, "Cart2RomComx", wxComboBox)->Enable(expansionRomLoaded_ );
		XRCCTRL(*this, "Cart3RomButtonComx", wxButton)->Enable(expansionRomLoaded_ );
		XRCCTRL(*this, "Cart3RomComx", wxComboBox)->Enable(expansionRomLoaded_ );
		XRCCTRL(*this, "Cart4RomButtonComx", wxButton)->Enable(expansionRomLoaded_ );
		XRCCTRL(*this, "Cart4RomComx", wxComboBox)->Enable(expansionRomLoaded_ );

		XRCCTRL(*this,"MainRomComx", wxComboBox)->Enable(!conf[COMX].sbActive_);
		XRCCTRL(*this,"RomButtonComx", wxButton)->Enable(!conf[COMX].sbActive_);
		XRCCTRL(*this,"ExpRomComx", wxComboBox)->Enable(!(conf[COMX].sbActive_ || conf[COMX].diagActive_));
		XRCCTRL(*this,"ExpRomButtonComx", wxButton)->Enable(!(conf[COMX].sbActive_ || conf[COMX].diagActive_));
        XRCCTRL(*this,"Cart1RomComx", wxComboBox)->Enable(!conf[COMX].diagActive_);
        XRCCTRL(*this,"Cart1RomButtonComx", wxButton)->Enable(!conf[COMX].diagActive_);
        XRCCTRL(*this,"Cart4RomComx", wxComboBox)->Enable(!conf[COMX].sbActive_);
        XRCCTRL(*this,"Cart4RomButtonComx", wxButton)->Enable(!conf[COMX].sbActive_);

        XRCCTRL(*this, "ExpRamSlotComx", wxSpinCtrl)->SetRange(1, 4);
		if (conf[COMX].sbActive_)
            XRCCTRL(*this, "ExpRamSlotComx", wxSpinCtrl)->SetRange(1, 3);
        else
            XRCCTRL(*this, "ExpRamSlotComx", wxSpinCtrl)->SetRange(1, 4);
        
		XRCCTRL(*this, "ExpRamSlotComx", wxSpinCtrl)->Enable(useExpansionRam_);
		if (useExpansionRam_)
			setComxExpRamSlot();
		XRCCTRL(*this, "Disk1ButtonComx", wxButton)->Enable(diskRomLoaded_);
		XRCCTRL(*this, "Disk1FileComx", wxTextCtrl)->Enable(diskRomLoaded_);
		XRCCTRL(*this, "EjectDisk1Comx", wxButton)->Enable(diskRomLoaded_ );
		XRCCTRL(*this, "Disk2ButtonComx", wxButton)->Enable(diskRomLoaded_);
		XRCCTRL(*this, "Disk2FileComx", wxTextCtrl)->Enable(diskRomLoaded_);
		XRCCTRL(*this, "EjectDisk2Comx", wxButton)->Enable(diskRomLoaded_ );

		XRCCTRL(*this, "WavFileComx", wxTextCtrl)->SetValue(conf[COMX].wavFile_);
		XRCCTRL(*this, "VidModeComx", wxChoice)->SetSelection(conf[COMX].videoMode_);
		if (conf[COMX].videoMode_ == PAL)
			clockTextCtrl[COMX]->ChangeValue(comxPalClock_);
		else
			clockTextCtrl[COMX]->ChangeValue(comxNtscClock_);

		XRCCTRL(*this, "ZoomValueComx", wxTextCtrl)->ChangeValue(conf[COMX].zoom_);
		XRCCTRL(*this, "PrintModeComx", wxChoice)->SetSelection(comxPrintMode_);
		setComxPrintMode();

		XRCCTRL(*this, "UseLocationComx", wxCheckBox)->SetValue(conf[COMX].useLoadLocation_);
	}
}

void GuiComx::readSbConfig()
{
    sbRootDirectory_ = readConfigDir("/Dir/Comx/RootDirectory", dataDir_ + "Comx" + pathSeparator_);
    wxString number;
    for (int i=0; i<11; i++)
    {
        number.Printf("%02d",i);
        SBRomDir_[i] = readConfigDir("/Dir/Comx/SB_ROM_" + number, dataDir_ + "Comx" + pathSeparator_ + "Usb FW" + pathSeparator_);
    }
    
    SBRom_[0] = getConfigItem("/Comx/SB_ROM_00", "sb.0000.bin");
    SBRom_[1] = getConfigItem("/Comx/SB_ROM_01", "sb.e000.bin");
    SBRom_[2] = getConfigItem("/Comx/SB_ROM_02", "sb.f800.bin");
    SBRom_[3] = getConfigItem("/Comx/SB_ROM_03", "sb.c000-0.bin");
    SBRom_[4] = getConfigItem("/Comx/SB_ROM_04", "sb.c000-1.bin");
    SBRom_[5] = getConfigItem("/Comx/SB_ROM_05", "sb.c000-2.bin");
    SBRom_[6] = getConfigItem("/Comx/SB_ROM_06", "sb.c000-3.bin");
    SBRom_[7] = getConfigItem("/Comx/SB_ROM_07", "sb.c000-4.bin");
    SBRom_[8] = getConfigItem("/Comx/SB_ROM_08", "sb.c000-5.bin");
    SBRom_[9] = getConfigItem("/Comx/SB_ROM_09", "sb.c000-6.bin");
    SBRom_[10] = getConfigItem("/Comx/SB_ROM_10", "sb.c000-7.bin");

    sbCdRoot_ = (int)configPointer->Read("/Comx/CdRoot", 0l);
    sbBackup_ = (int)configPointer->Read("/Comx/Backup", 0l);
    sbBackupSys_ = (int)configPointer->Read("/Comx/BackupSys", 1l);
    sbCaseFile_ = (int)configPointer->Read("/Comx/CaseFile", 0l);
    sbCaseDir_ = (int)configPointer->Read("/Comx/CaseDir", 2l);
    
/*    if (conf[COMX].sbActive_)
    {
        conf[COMX].romDir_[MAINROM1] = SBRomDir_[0];
        conf[COMX].rom_[MAINROM1] = SBRom_[0];
        conf[COMX].romDir_[EXPROM] = SBRomDir_[1];
        conf[COMX].rom_[EXPROM] = SBRom_[1];
        conf[COMX].romDir_[CARTROM4] = SBRomDir_[2];
        conf[COMX].rom_[CARTROM4] = SBRom_[2];
    }*/
}

void GuiComx::readInitialComxConfig()
{
    if (useExpansionRam_)
        resetCardText();
    
    wxString name=wxGetUserName();
    sbPlayer_ = configPointer->Read("/Comx/PlayerName", name);
    sbLocation_ = configPointer->Read("/Comx/PlayerLocation", "");
    sbEmail_ = configPointer->Read("/Comx/Email", "");
    sbUrlHome_ = configPointer->Read("/Comx/HomePage", "http://www.emma02.hobby-site.com");
    
    sbUrlBookMark_[0] = configPointer->Read("/Comx/BookMark0", "http://www.comx35.com/comx35sbhs/hs2.php");
    sbUrlBookMark_[1] = configPointer->Read("/Comx/BookMark1", "http://www.comx35.com/comx35.html");
    sbUrlBookMark_[2] = configPointer->Read("/Comx/BookMark2", "http://www.comx35.com/superboard.html");
    sbUrlBookMark_[3] = configPointer->Read("/Comx/BookMark3", "http://www.emma02.hobby-site.com/comx.htm");
    sbUrlBookMark_[4] = configPointer->Read("/Comx/BookMark4", "http://www.emma02.hobby-site.com/basic_comx.htm");
    sbUrlBookMark_[5] = configPointer->Read("/Comx/BookMark5", "http://www.elf-emulation.com/");
    sbUrlBookMark_[6] = configPointer->Read("/Comx/BookMark6", "http://www.cosmacelf.com/");
    sbUrlBookMark_[7] = configPointer->Read("/Comx/BookMark7", "http://www.sparetimegizmos.com/");
    sbUrlBookMark_[8] = configPointer->Read("/Comx/BookMark8", "http://www.cosmacvip.com/");
    sbUrlBookMark_[9] = configPointer->Read("/Comx/BookMark9", "http://www.comxclub.hobby-site.com/");
    
    numberOfAlias_ = 0;
    wxString aliasStr, aliasEmailStr;
    
    numberOfAlias_ = configPointer->Read("/Comx/AliasNumberOf", 0l);
    alias.resize(numberOfAlias_);
    aliasEmail.resize(numberOfAlias_);
    
    for (size_t number=0; number < numberOfAlias_; number++)
    {
        aliasStr.Printf("/Comx/Alias%d", (int)number);
        aliasEmailStr.Printf("/Comx/AliasEmail%d", (int)number);
        
        alias[number] = configPointer->Read(aliasStr);
        aliasEmail[number] = configPointer->Read(aliasEmailStr);
    }

    sbFwVersion_ = (int)configPointer->Read("/Comx/FwVersion", 0l);
}

void GuiComx::writeSbConfig()
{
    writeConfigDir("/Dir/Comx/RootDirectory", sbRootDirectory_);
    wxString number;
    for (int i=0; i<11; i++)
    {
        number.Printf("%02d",i);
        writeConfigDir("/Dir/Comx/SB_ROM_" + number, SBRomDir_[i]);
        setConfigItem("/Comx/SB_ROM_"+number, SBRom_[i]);
    }
    
    configPointer->Write("/Comx/CdRoot", sbCdRoot_);
    configPointer->Write("/Comx/Backup", sbBackup_);
    configPointer->Write("/Comx/BackupSys", sbBackupSys_);
    configPointer->Write("/Comx/CaseFile", sbCaseFile_);
    configPointer->Write("/Comx/CaseDir", sbCaseDir_);
}

void GuiComx::writeComxDirConfig()
{
        if (!conf[COMX].sbActive_)
    {
        writeConfigDir("/Dir/Comx/Main_Rom_File", conf[COMX].romDir_[MAINROM1]);
        writeConfigDir("/Dir/Comx/Expansion_Rom_File", conf[COMX].romDir_[EXPROM]);
        writeConfigDir("/Dir/Comx/Card_4_Rom_File", conf[COMX].romDir_[CARTROM4]);
    }
    writeConfigDir("/Dir/Comx/Main", conf[COMX].mainDir_);
    writeConfigDir("/Dir/Comx/Card_1_Rom_File", conf[COMX].romDir_[CARTROM1]);
    writeConfigDir("/Dir/Comx/Card_2_Rom_File", conf[COMX].romDir_[CARTROM2]);
    writeConfigDir("/Dir/Comx/Card_3_Rom_File", conf[COMX].romDir_[CARTROM3]);
    writeConfigDir("/Dir/Comx/PL80_Rom_Files", Pl80Data_[0]);
    writeConfigDir("/Dir/Comx/Software_File", conf[COMX].ramDir_);
    writeConfigDir("/Dir/Comx/Floppy_1_File", floppyDir_[0]);
    writeConfigDir("/Dir/Comx/Floppy_2_File", floppyDir_[1]);
    writeConfigDir("/Dir/Comx/Key_File", conf[COMX].keyFileDir_);
    writeConfigDir("/Dir/Comx/Print_File", conf[COMX].printFileDir_);
    writeConfigDir("/Dir/Comx/Video_Dump_File", conf[COMX].screenDumpFileDir_);
    writeConfigDir("/Dir/Comx/Wav_File", conf[COMX].wavFileDir_);
	writeConfigDir("/Dir/Comx/DIAG_ROM_0", DiagRomDir_[0]);
	writeConfigDir("/Dir/Comx/DIAG_ROM_1", DiagRomDir_[1]);
}

void GuiComx::writeComxConfig()
{
    if (!conf[COMX].sbActive_ && !conf[COMX].diagActive_)
    {
        configPointer->Write("/Comx/Main_Rom_File", conf[COMX].rom_[MAINROM1]);
        configPointer->Write("/Comx/Expansion_Rom_File", conf[COMX].rom_[EXPROM]);
        configPointer->Write("/Comx/Card_1_Rom_File", conf[COMX].rom_[CARTROM1]);
        configPointer->Write("/Comx/Card_4_Rom_File", conf[COMX].rom_[CARTROM4]);
    }
    
    if (conf[COMX].sbActive_)
    {
        configPointer->Write("/Comx/Card_1_Rom_File", conf[COMX].rom_[CARTROM1]);
    }
    
    if (conf[COMX].diagActive_)
    {
        configPointer->Write("/Comx/Main_Rom_File", conf[COMX].rom_[MAINROM1]);
        configPointer->Write("/Comx/Card_4_Rom_File", conf[COMX].rom_[CARTROM4]);
    }
    
	configPointer->Write("/Comx/Card_2_Rom_File", conf[COMX].rom_[CARTROM2]);
	configPointer->Write("/Comx/Card_3_Rom_File", conf[COMX].rom_[CARTROM3]);
	configPointer->Write("/Comx/PL80_Rom_File", Pl80Data_[1]);
	configPointer->Write("/Comx/PL80_Extension_Rom_File", Pl80Data_[2]);
	configPointer->Write("/Comx/Floppy_1_File", floppy_[0]);
	configPointer->Write("/Comx/Floppy_2_File", floppy_[1]);
	configPointer->Write("/Comx/Key_File", conf[COMX].keyFile_);
	configPointer->Write("/Comx/Print_File", conf[COMX].printFile_);
	configPointer->Write("/Comx/Video_Dump_File", conf[COMX].screenDumpFile_);
	configPointer->Write("/Comx/Wav_File", conf[COMX].wavFile_);

	configPointer->Write("/Comx/Clock_Speed_When_Using_Pal", comxPalClock_);
	configPointer->Write("/Comx/Clock_Speed_When_Using_Ntsc", comxNtscClock_);
	configPointer->Write("/Comx/Turbo_Clock_Speed", conf[COMX].turboClock_);
	configPointer->Write("/Comx/Volume", conf[COMX].volume_);
	configPointer->Write("/Comx/Enable_Turbo_Cassette", conf[COMX].turbo_);
	configPointer->Write("/Comx/Video_Mode", conf[COMX].videoMode_);
	configPointer->Write("/Comx/Zoom", conf[COMX].zoom_);
	configPointer->Write("/Comx/Enable_Auto_Cassette", conf[COMX].autoCassetteLoad_);
	configPointer->Write("/Comx/Enable_Real_Cassette", conf[COMX].realCassetteLoad_);
	configPointer->Write("/Comx/Enable_80_Column_Interlace", conf[COMX].interlace_);
	configPointer->Write("/Comx/Enable_SB", conf[COMX].sbActive_);
	configPointer->Write("/Comx/Enable_DIAG", conf[COMX].diagActive_);
	configPointer->Write("/Comx/Enable_DIAG_ON", conf[COMX].diagOn_);
	configPointer->Write("/Comx/Use_Ram_Card", useExpansionRam_);
    configPointer->Write("/Comx/Video_Log", conf[COMX].videoLog_);
	configPointer->Write("/Comx/Ram_Card_Slot", expansionRamSlot_);
	configPointer->Write("/Comx/Expansion_Rom_Loaded", expansionRomLoaded_);
	configPointer->Write("/Comx/Disk_Rom_Loaded", diskRomLoaded_);

	configPointer->Write("/Comx/Print_Mode", comxPrintMode_);

    wxString number;
    for (int i=0; i<5; i++)
    {
        number.Printf("%1d",i);
        writeConfigDir("/Dir/Comx/Epromdir" + number, EpromRomDir_[i]);
        setConfigItem("/Comx/Eprom"+number, EpromRom_[i]);
    }
    
	setConfigItem("/Comx/DIAG_ROM_0", DiagRom_[0]);
	setConfigItem("/Comx/DIAG_ROM_1", DiagRom_[1]);
    configPointer->Write("/Comx/DiagRomChecksum", diagRomChecksum_);
    configPointer->Write("/Comx/DiagFactoryUnit", diagFactory_);
    configPointer->Write("/Comx/DiagCassetteCables", diagCassetteCables_);

	writeSbConfig();
}

void GuiComx::readComxWindowConfig()
{
    conf[COMX].mainX_ = (int)configPointer->Read("/Comx/Window_Position_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[COMX].mainY_ = (int)configPointer->Read("/Comx/Window_Position_Y", mainWindowY_);
    conf[COMX].mc6845X_ = (int)configPointer->Read("/Comx/Window_Position_MC6845_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[COMX].mc6845Y_ = (int)configPointer->Read("/Comx/Window_Position_MC6845_Y", mainWindowY_);
    conf[COMX].sizeX_ = (int)configPointer->Read("/Comx/Window_Size_X", 480);
    conf[COMX].sizeY_ = (int)configPointer->Read("/Comx/Window_Size_Y", 432);
    conf[COMX].sizeNTSCX_ = (int)configPointer->Read("/Comx/Window_Size_Ntsc_X", 480);
    conf[COMX].sizeNTSCY_ = (int)configPointer->Read("/Comx/Window_Size_Ntsc_Y", 384);
    conf[COMX].sizeX6845_ = (int)configPointer->Read("/Comx/Window_Size_6845_X", 640);
    conf[COMX].sizeY6845_ = (int)configPointer->Read("/Comx/Window_Size_6845_Y", 432);
}

void GuiComx::writeComxWindowConfig()
{
    if (conf[COMX].mainX_ > 0)
        configPointer->Write("/Comx/Window_Position_X", conf[COMX].mainX_);
    if (conf[COMX].mainY_ > 0)
        configPointer->Write("/Comx/Window_Position_Y", conf[COMX].mainY_);
    if (conf[COMX].mc6845X_ > 0)
        configPointer->Write("/Comx/Window_Position_MC6845_X", conf[COMX].mc6845X_);
    if (conf[COMX].mc6845Y_ > 0)
        configPointer->Write("/Comx/Window_Position_MC6845_Y", conf[COMX].mc6845Y_);
    configPointer->Write("/Comx/Window_Size_X", conf[COMX].sizeX_);
    configPointer->Write("/Comx/Window_Size_Y", conf[COMX].sizeY_);
    configPointer->Write("/Comx/Window_Size_Ntsc_X", conf[COMX].sizeNTSCX_);
    configPointer->Write("/Comx/Window_Size_Ntsc_Y", conf[COMX].sizeNTSCY_);
    configPointer->Write("/Comx/Window_Size_6845_X", conf[COMX].sizeX6845_);
    configPointer->Write("/Comx/Window_Size_6845_Y", conf[COMX].sizeY6845_);
}

void GuiComx::onExpansionRom(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the Expansion ROM file to load",
                               conf[COMX].romDir_[EXPROM], XRCCTRL(*this, "ExpRomComx", wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                    "Binary File|*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
	{
		expansionRomLoaded_ = false;
		XRCCTRL(*this, "Cart2RomButtonComx", wxButton)->Enable(false);
		XRCCTRL(*this, "Cart2RomComx", wxComboBox)->Enable(false);
		XRCCTRL(*this, "Cart3RomButtonComx", wxButton)->Enable(false);
		XRCCTRL(*this, "Cart3RomComx", wxComboBox)->Enable(false);
		XRCCTRL(*this, "Cart4RomButtonComx", wxButton)->Enable(false);
		XRCCTRL(*this, "Cart4RomComx", wxComboBox)->Enable(false);
		return;
	}

	expansionRomLoaded_ = true;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[COMX].romDir_[EXPROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[COMX].rom_[EXPROM] = FullPath.GetFullName();

	XRCCTRL(*this, "ExpRomComx", wxComboBox)->SetValue(conf[COMX].rom_[EXPROM]);
	if ((expansionRamSlot_ != 2) || !useExpansionRam_)
	{
		XRCCTRL(*this, "Cart2RomButtonComx", wxButton)->Enable(true);
		XRCCTRL(*this, "Cart2RomComx", wxComboBox)->Enable(true);
	}
	if ((expansionRamSlot_ != 3) || !useExpansionRam_)
	{
		XRCCTRL(*this, "Cart3RomButtonComx", wxButton)->Enable(true);
		XRCCTRL(*this, "Cart3RomComx", wxComboBox)->Enable(true);
	}
	if ((expansionRamSlot_ != 4) || !useExpansionRam_)
	{
		XRCCTRL(*this, "Cart4RomButtonComx", wxButton)->Enable(true);
		XRCCTRL(*this, "Cart4RomComx", wxComboBox)->Enable(true);
	}
}

void GuiComx::onExpansionRomText(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].rom_[EXPROM] = XRCCTRL(*this, "ExpRomComx", wxComboBox)->GetValue();
}

void GuiComx::onCard1Rom(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the Cartridge 1 ROM file to load",
                               conf[COMX].romDir_[CARTROM1], XRCCTRL(*this, "Cart1RomComx", wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                    "Binary File|*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[COMX].romDir_[CARTROM1] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[COMX].rom_[CARTROM1] = FullPath.GetFullName();

	XRCCTRL(*this, "Cart1RomComx", wxComboBox)->SetValue(conf[COMX].rom_[CARTROM1]);
}

void GuiComx::onCard1RomText(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].rom_[CARTROM1] = XRCCTRL(*this, "Cart1RomComx", wxComboBox)->GetValue();
}

void GuiComx::onCard2Rom(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the Cartridge 2 ROM file to load",
                               conf[COMX].romDir_[CARTROM2], XRCCTRL(*this, "Cart2RomComx", wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                    "Binary File|*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[COMX].romDir_[CARTROM2] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[COMX].rom_[CARTROM2] = FullPath.GetFullName();

	XRCCTRL(*this, "Cart2RomComx", wxComboBox)->SetValue(conf[COMX].rom_[CARTROM2]);
}

void GuiComx::onCard2RomText(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].rom_[CARTROM2] = XRCCTRL(*this, "Cart2RomComx", wxComboBox)->GetValue();
}

void GuiComx::onCard3Rom(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the Cartridge 3 ROM file to load",
                               conf[COMX].romDir_[CARTROM3], XRCCTRL(*this, "Cart3RomComx", wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                    "Binary File|*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[COMX].romDir_[CARTROM3] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[COMX].rom_[CARTROM3] = FullPath.GetFullName();

	XRCCTRL(*this, "Cart3RomComx", wxComboBox)->SetValue(conf[COMX].rom_[CARTROM3]);
}

void GuiComx::onCard3RomText(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].rom_[CARTROM3] = XRCCTRL(*this, "Cart3RomComx", wxComboBox)->GetValue();
}

void GuiComx::onCard4Rom(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the Cartridge 4 ROM file to load",
                               conf[COMX].romDir_[CARTROM4], XRCCTRL(*this, "Cart4RomComx", wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                    "Binary File|*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[COMX].romDir_[CARTROM4] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[COMX].rom_[CARTROM4] = FullPath.GetFullName();

	XRCCTRL(*this, "Cart4RomComx", wxComboBox)->SetValue(conf[COMX].rom_[CARTROM4]);
}

void GuiComx::onCard4RomText(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].rom_[CARTROM4] = XRCCTRL(*this, "Cart4RomComx", wxComboBox)->GetValue();
}

void GuiComx::onComxDisk1(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the Disk 1 file to load",
                               floppyDir_[0], floppy_[0],
                               "img",
                               wxString::Format
                              (
                                    "Disk Image (*.img)|*.img|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	floppyDir_[0] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	floppy_[0] = FullPath.GetFullName();

	if (mode_.gui)
		XRCCTRL(*this, "Disk1FileComx", wxTextCtrl)->SetValue(floppy_[0]);
}

void GuiComx::onComxDiskText1(wxCommandEvent&event)
{
	floppy_[0] = event.GetString();
	if ((runningComputer_ == COMX) && diskRomLoaded_)
	{
		if (floppy_[0].Len() == 0)
			p_Comx->setDiskName(1, "");
		else
			p_Comx->setDiskName(1, floppyDir_[0] + floppy_[0]);
	}
}

void GuiComx::onComxDiskEject1(wxCommandEvent& WXUNUSED(event) )
{
	floppy_[0] = "";
	if (mode_.gui)
		XRCCTRL(*this, "Disk1FileComx", wxTextCtrl)->SetValue(floppy_[0]);
}

void GuiComx::onComxDisk2(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the Disk 2 file to load",
                               floppyDir_[1], floppy_[1],
                               "img",
                               wxString::Format
                              (
                                    "Disk Image (*.img)|*.img|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	floppyDir_[1] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	floppy_[1] = FullPath.GetFullName();

	if (mode_.gui)
		XRCCTRL(*this, "Disk2FileComx", wxTextCtrl)->SetValue(floppy_[1]);
}

void GuiComx::onComxDiskText2(wxCommandEvent&event)
{
	floppy_[1] = event.GetString();
	if ((runningComputer_ == COMX) && diskRomLoaded_)
	{
		if (floppy_[1].Len() == 0)
			p_Comx->setDiskName(2, "");
		else
			p_Comx->setDiskName(2, floppyDir_[1] + floppy_[1]);
	}
}

void GuiComx::onComxDiskEject2(wxCommandEvent& WXUNUSED(event) )
{
	floppy_[1] = "";
	if (mode_.gui)
		XRCCTRL(*this, "Disk2FileComx", wxTextCtrl)->SetValue(floppy_[1]);
}

void GuiComx::onComxPrintFileText(wxCommandEvent&WXUNUSED(event))
{
	conf[COMX].printFile_ = XRCCTRL(*this, "PrintFileComx", wxTextCtrl)->GetValue();

	if (runningComputer_ == COMX)
	{
		if (p_Comx->isPrintRomLoaded(COMXPARALLEL))
		{
			if (conf[COMX].printFile_.Len() == 0)
				p_PrinterParallel->setPrintfileName("");
			else
				p_PrinterParallel->setPrintfileName(conf[COMX].printFileDir_ + conf[COMX].printFile_);
		}
		if (p_Comx->isPrintRomLoaded(COMXSERIAL))
		{
			if (conf[COMX].printFile_.Len() == 0)
				p_PrinterSerial->setPrintfileName("");
			else
				p_PrinterSerial->setPrintfileName(conf[COMX].printFileDir_ + conf[COMX].printFile_);
		}
		if (p_Comx->isPrintRomLoaded(COMXTHERMAL))
		{
			if (conf[COMX].printFile_.Len() == 0)
				p_PrinterThermal->setPrintfileName("");
			else
				p_PrinterThermal->setPrintfileName(conf[COMX].printFileDir_ + conf[COMX].printFile_);
		}
	}
}

void GuiComx::onComxPrintMode(wxCommandEvent&event)
{
	comxPrintMode_ = event.GetSelection();
	setComxPrintMode();

	if (runningComputer_ == COMX)
	{
		if (p_Comx->isPrintRomLoaded(COMXPARALLEL))
		{
			p_PrinterParallel->setComxPrintMode(comxPrintMode_);
		}
		if (p_Comx->isPrintRomLoaded(COMXSERIAL))
		{
			p_PrinterSerial->setComxPrintMode(comxPrintMode_);
		}
	}
}

void GuiComx::onComxPrintButton(wxCommandEvent&WXUNUSED(event))
{
	onComxF4();
}

void GuiComx::onComxVideoMode(wxCommandEvent&event)
{
	conf[COMX].videoMode_ = event.GetSelection();
	switch(conf[COMX].videoMode_)
	{
		case PAL:
			clockTextCtrl[COMX]->ChangeValue(comxPalClock_);
		break;

		case NTSC:
			clockTextCtrl[COMX]->ChangeValue(comxNtscClock_);
		break;
	}
}

void GuiComx::onComxExpansionRam(wxCommandEvent&event)
{
	useExpansionRam_ = event.IsChecked();
	XRCCTRL(*this, "ExpRamSlotComx", wxSpinCtrl)->Enable(useExpansionRam_);
	if (useExpansionRam_)
    {
        if (expansionRamSlot_ == 4 && useExpansionRam_)
        {
            expansionRamSlot_ = 3;
            XRCCTRL(*this, "ExpRamSlotComx", wxSpinCtrl)->SetValue(expansionRamSlot_);
       }
		setComxExpRamSlot();
    }
	else
		resetCardText();
}

void GuiComx::onComxExpansionRamSlot(wxSpinEvent&event)
{
	resetCardText();

	expansionRamSlot_ = event.GetPosition();

	setComxExpRamSlot();
}

void GuiComx::statusLedOnEvent()
{
	if (wxIsMainThread())
	{
		statusLedOnDirect();
		return;
	}
	wxCommandEvent event(STATUS_LED_ON, 801);
	event.SetEventObject(this);
	wxPostEvent(this, event);
	while(!isComxStatusLedOn_)
		p_Computer->sleepComputer(1);
}

void GuiComx::statusLedOffEvent()
{
	if (wxIsMainThread())
	{
		statusLedOffDirect();
		return;
	}
	wxCommandEvent event(STATUS_LED_OFF, 802);
	event.SetEventObject(this);
	wxPostEvent(this, event);
	while(isComxStatusLedOn_)
		p_Computer->sleepComputer(1);
}

void GuiComx::expLedOnEvent()
{
	if (wxIsMainThread() || !slotLedUpdate_)
	{
		expLedOnDirect();
		return;
	}
	if (isComxExpLedOn_)  return;
	wxCommandEvent event(EXP_LED_ON, 805);
	event.SetEventObject(this);
	wxPostEvent(this, event);
	while(!isComxExpLedOn_)
		p_Computer->sleepComputer(1);
}

void GuiComx::expLedOffEvent()
{
	if (wxIsMainThread() || !slotLedUpdate_)
	{
		expLedOffDirect();
		return;
	}
	if (!isComxExpLedOn_)  return;
	wxCommandEvent event(EXP_LED_OFF, 806);
	event.SetEventObject(this);
	wxPostEvent(this, event);
	while(isComxExpLedOn_)
		p_Computer->sleepComputer(1);
}

void GuiComx::v1870BarSizeEvent()
{
	wxCommandEvent event(STATUS_BAR_1870, 803);
	event.SetEventObject(this);
	wxPostEvent(this, event);
}

wxString GuiComx::getFloppyDir(int drive)
{
	return floppyDir_[drive];
}

wxString GuiComx::getFloppyFile(int drive)
{
	return floppy_[drive];
}

wxString GuiComx::getPL80Data(int item)
{
	return Pl80Data_[item];
}

void GuiComx::setPL80Data(int item, wxString data)
{
	Pl80Data_[item] = data;
}

void GuiComx::enableComxGui(bool status)
{
	if (useExpansionRam_)
	{
		XRCCTRL(*this, "ExpRamSlotComx", wxSpinCtrl)->Enable(status);
		switch(expansionRamSlot_)
		{
			case 1:
				XRCCTRL(*this, "Cart1RomComx", wxComboBox)->Enable(false);
				XRCCTRL(*this, "Cart1RomButtonComx", wxButton)->Enable(false);
			break;

			case 2:
				XRCCTRL(*this, "Cart2RomComx", wxComboBox)->Enable(false);
				XRCCTRL(*this, "Cart2RomButtonComx", wxButton)->Enable(false);
			break;

			case 3:
				XRCCTRL(*this, "Cart3RomComx", wxComboBox)->Enable(false);
				XRCCTRL(*this, "Cart3RomButtonComx", wxButton)->Enable(false);
			break;

			case 4:
				XRCCTRL(*this, "Cart4RomComx", wxComboBox)->Enable(false);
				XRCCTRL(*this, "Cart4RomButtonComx", wxButton)->Enable(false);
			break;
		}
	}
}

void GuiComx::statusLedOn(wxCommandEvent&WXUNUSED(event))
{
	if (runningComputer_ != COMX)  return;
	p_Comx->updateStatusLed(true);
	isComxStatusLedOn_ = true;
}

void GuiComx::statusLedOnDirect()
{
	if (runningComputer_ != COMX)  return;
	p_Comx->updateStatusLed(true);
	isComxStatusLedOn_ = true;
}

void GuiComx::statusLedOff(wxCommandEvent&WXUNUSED(event))
{
	if (runningComputer_ != COMX)  return;
	p_Comx->updateStatusLed(false);
	isComxStatusLedOn_ = false;
}

void GuiComx::statusLedOffDirect()
{
	if (runningComputer_ != COMX)  return;
	p_Comx->updateStatusLed(false);
	isComxStatusLedOn_ = false;
}

void GuiComx::expLedOn(wxCommandEvent&WXUNUSED(event))
{
	if (runningComputer_ != COMX)  return;
	p_Comx->updateExpansionLed(true);
	isComxExpLedOn_ = true;
}

void GuiComx::expLedOnDirect()
{
	if (runningComputer_ != COMX)  return;
	p_Comx->updateExpansionLed(true);
	isComxExpLedOn_ = true;
}

void GuiComx::expLedOff(wxCommandEvent&WXUNUSED(event))
{
	if (runningComputer_ != COMX)  return;
	p_Comx->updateExpansionLed(false);
	isComxExpLedOn_ = false;
}

void GuiComx::expLedOffDirect()
{
	if (runningComputer_ != COMX)  return;
	p_Comx->updateExpansionLed(false);
	isComxExpLedOn_ = false;
}

void GuiComx::v1870BarSize(wxCommandEvent&WXUNUSED(event))
{
	if (runningComputer_ == COMX)
		p_Comx->v1870BarSize();
	if (runningComputer_ == CIDELSA)
		p_Cidelsa->v1870BarSize();
}

void GuiComx::setComxExpRamSlot()
{
	if (!mode_.gui)  return;

	switch(expansionRamSlot_)
	{
		case 1:
			saveCardText_ = XRCCTRL(*this, "Cart1RomComx", wxComboBox)->GetValue();
			XRCCTRL(*this, "Cart1RomComx", wxComboBox)->SetValue("Ram Card");
			XRCCTRL(*this, "Cart1RomComx", wxComboBox)->Enable(false);
			XRCCTRL(*this, "Cart1RomButtonComx", wxButton)->Enable(false);
		break;

		case 2:
			saveCardText_ = XRCCTRL(*this, "Cart2RomComx", wxComboBox)->GetValue();
			XRCCTRL(*this, "Cart2RomComx", wxComboBox)->SetValue("Ram Card");
			XRCCTRL(*this, "Cart2RomComx", wxComboBox)->Enable(false);
			XRCCTRL(*this, "Cart2RomButtonComx", wxButton)->Enable(false);
		break;

		case 3:
			saveCardText_ = XRCCTRL(*this, "Cart3RomComx", wxComboBox)->GetValue();
			XRCCTRL(*this, "Cart3RomComx", wxComboBox)->SetValue("Ram Card");
			XRCCTRL(*this, "Cart3RomComx", wxComboBox)->Enable(false);
			XRCCTRL(*this, "Cart3RomButtonComx", wxButton)->Enable(false);
		break;

		case 4:
			saveCardText_ = XRCCTRL(*this, "Cart4RomComx", wxComboBox)->GetValue();
			XRCCTRL(*this, "Cart4RomComx", wxComboBox)->SetValue("Ram Card");
			XRCCTRL(*this, "Cart4RomComx", wxComboBox)->Enable(false);
			XRCCTRL(*this, "Cart4RomButtonComx", wxButton)->Enable(false);
		break;
	}
}

void GuiComx::resetCardText()
{
	if (!mode_.gui)  return;
	switch(expansionRamSlot_)
	{
		case 1:
			XRCCTRL(*this, "Cart1RomComx", wxComboBox)->SetValue(saveCardText_);
			XRCCTRL(*this, "Cart1RomComx", wxComboBox)->Enable(true);
			XRCCTRL(*this, "Cart1RomButtonComx", wxButton)->Enable(true);
		break;

		case 2:
			XRCCTRL(*this, "Cart2RomComx", wxComboBox)->SetValue(saveCardText_);
			if (expansionRomLoaded_)
			{
				XRCCTRL(*this, "Cart2RomComx", wxComboBox)->Enable(true);
				XRCCTRL(*this, "Cart2RomButtonComx", wxButton)->Enable(true);
			}
		break;

		case 3:
			XRCCTRL(*this, "Cart3RomComx", wxComboBox)->SetValue(saveCardText_);
			if (expansionRomLoaded_)
			{
				XRCCTRL(*this, "Cart3RomComx", wxComboBox)->Enable(true);
				XRCCTRL(*this, "Cart3RomButtonComx", wxButton)->Enable(true);
			}
		break;

		case 4:
			XRCCTRL(*this, "Cart4RomComx", wxComboBox)->SetValue(saveCardText_);
			if (expansionRomLoaded_)
			{
				XRCCTRL(*this, "Cart4RomComx", wxComboBox)->Enable(true);
				XRCCTRL(*this, "Cart4RomButtonComx", wxButton)->Enable(true);
			}
		break;
	}
}

bool GuiComx::isSaving()
{
	if (runningComputer_ == COMX)
		return p_Comx->isSaving();
	else
		return false;
}

void GuiComx::setComxPrintMode()
{
	if (!mode_.gui)  return;
	if (comxPrintMode_ == PRINTFILE)
	{
		XRCCTRL(*this, "PrintFileButtonComx", wxButton)->Enable(true);
		XRCCTRL(*this, "PrintFileComx", wxTextCtrl)->Enable(true);
	}
	else
	{
		XRCCTRL(*this, "PrintFileButtonComx", wxButton)->Enable(false);
		XRCCTRL(*this, "PrintFileComx", wxTextCtrl)->Enable(false);
	}
}

void GuiComx::openComxPrinterFrame(wxCommandEvent&WXUNUSED(event))
{
	if (runningComputer_ != COMX)  return;
	onComxF4();
}

void GuiComx::onComxF4()
{
	if (runningComputer_ == COMX)
	{
		p_Comx->onComxF4();
		if (comxPrintMode_ == PRINTFILE)
		{
			comxPrintMode_ = COMXPRINTPLOTTER;
			if (mode_.gui)  
				XRCCTRL(*this, "PrintModeComx", wxChoice)->SetSelection(comxPrintMode_);
			setComxPrintMode();
		}
	}
}

void GuiComx::setFandMBasicGui()
{
	if (!mode_.gui)  return;
	if (p_Comx->isFAndMBasicRunning())
	{
		XRCCTRL(*this, "RunButtonComx", wxButton)->SetLabel("F&&M RUN");
		XRCCTRL(*this, "SaveButtonComx", wxButton)->SetLabel("F&&M SAVE");
		XRCCTRL(*this, "LoadButtonComx", wxButton)->SetLabel("F&&M LOAD");
	}
	else
	{
		XRCCTRL(*this, "RunButtonComx", wxButton)->SetLabel("RUN");
		XRCCTRL(*this, "SaveButtonComx", wxButton)->SetLabel("SAVE");
		XRCCTRL(*this, "LoadButtonComx", wxButton)->SetLabel("LOAD");
	}
}

void GuiComx::enableDiskRomGui(bool DiskRom)
{
	diskRomLoaded_ = DiskRom;
	if (!mode_.gui)  return;
	XRCCTRL(*this, "Disk1FileComx", wxTextCtrl)->Enable(diskRomLoaded_);
	XRCCTRL(*this, "Disk1ButtonComx", wxButton)->Enable(diskRomLoaded_);
	XRCCTRL(*this, "EjectDisk1Comx", wxButton)->Enable(diskRomLoaded_);
	XRCCTRL(*this, "Disk2FileComx", wxTextCtrl)->Enable(diskRomLoaded_);
	XRCCTRL(*this, "Disk2ButtonComx", wxButton)->Enable(diskRomLoaded_);
	XRCCTRL(*this, "EjectDisk2Comx", wxButton)->Enable(diskRomLoaded_);
}

void GuiComx::setExtRomStatus(bool expansionRomLoaded)
{
	expansionRomLoaded_ = expansionRomLoaded;
}

void GuiComx::setComxPrintMode(int mode)
{
	comxPrintMode_ = mode;
}

int GuiComx::getComxPrintMode()
{
	return comxPrintMode_;
}

void GuiComx::onEpromDialog(wxCommandEvent&WXUNUSED(event))
{
	EpromDialog EpromDialog(this);
 	EpromDialog.ShowModal();
}

void GuiComx::onSBDialog(wxCommandEvent&WXUNUSED(event))
{
	SBDialog SBDialog(this);
	SBDialog.ShowModal();
}

void GuiComx::onDiagDialog(wxCommandEvent&WXUNUSED(event))
{
	DiagDialog DiagDialog(this);
	DiagDialog.ShowModal();
}

void GuiComx::onSbActive(wxCommandEvent&event)
{
	conf[COMX].sbActive_ = event.IsChecked();
	conf[COMX].diagActive_ = false;
	diagSbChange();
}

void GuiComx::onDiagActive(wxCommandEvent&event)
{
	conf[COMX].diagActive_ = event.IsChecked();
	conf[COMX].sbActive_ = false;
	diagSbChange();
}

void GuiComx::onDiagOn(wxCommandEvent&event)
{
	conf[COMX].diagOn_ = event.GetSelection();
}

void GuiComx::diagSbChange()
{
	if (mode_.gui)
	{
		XRCCTRL(*this, "DiagActiveComx", wxCheckBox)->SetValue(conf[COMX].diagActive_);
		XRCCTRL(*this, "SbActiveComx", wxCheckBox)->SetValue(conf[COMX].sbActive_);

		XRCCTRL(*this, "MainRomComx", wxComboBox)->Enable(!conf[COMX].sbActive_);
		XRCCTRL(*this, "RomButtonComx", wxButton)->Enable(!conf[COMX].sbActive_);
		XRCCTRL(*this, "ExpRomComx", wxComboBox)->Enable(!(conf[COMX].diagActive_ || conf[COMX].sbActive_));
		XRCCTRL(*this, "ExpRomButtonComx", wxButton)->Enable(!(conf[COMX].diagActive_ || conf[COMX].sbActive_));
		XRCCTRL(*this, "Cart1RomComx", wxComboBox)->Enable(!conf[COMX].diagActive_);
		XRCCTRL(*this, "Cart1RomButtonComx", wxButton)->Enable(!conf[COMX].diagActive_);
		XRCCTRL(*this, "Cart4RomComx", wxComboBox)->Enable(!conf[COMX].sbActive_);
		XRCCTRL(*this, "Cart4RomButtonComx", wxButton)->Enable(!conf[COMX].sbActive_);
		XRCCTRL(*this, "DiagOnComx", wxChoice)->Enable(conf[COMX].diagActive_);
		XRCCTRL(*this, "ExpRamComx", wxCheckBox)->Enable(!conf[COMX].diagActive_);
	}

	conf[COMX].romDir_[MAINROM1] = readConfigDir("/Dir/Comx/Main_Rom_File", dataDir_ + "Comx" + pathSeparator_);
	conf[COMX].rom_[MAINROM1] = configPointer->Read("/Comx/Main_Rom_File", "comx35.1.1.bin");
	conf[COMX].romDir_[EXPROM] = readConfigDir("/Dir/Comx/Expansion_Rom_File", dataDir_ + "Comx" + pathSeparator_);
	conf[COMX].rom_[EXPROM] = configPointer->Read("/Comx/Expansion_Rom_File", "f&m.expansion.3.2.bin");
	conf[COMX].romDir_[CARTROM1] = readConfigDir("/Dir/Comx/Card_1_Rom_File", dataDir_ + "Comx" + pathSeparator_);
	conf[COMX].rom_[CARTROM1] = configPointer->Read("/Comx/Card_1_Rom_File", "fdc.bin");
	conf[COMX].romDir_[CARTROM4] = readConfigDir("/Dir/Comx/Card_4_Rom_File", dataDir_ + "Comx" + pathSeparator_);
	conf[COMX].rom_[CARTROM4] = configPointer->Read("/Comx/Card_4_Rom_File", "f&m.eprom.board.1.2.bin");

	if (conf[COMX].sbActive_)
	{
		conf[COMX].romDir_[MAINROM1] = SBRomDir_[0];
		conf[COMX].rom_[MAINROM1] = SBRom_[0];
		conf[COMX].romDir_[EXPROM] = SBRomDir_[1];
		conf[COMX].rom_[EXPROM] = SBRom_[1];
		conf[COMX].romDir_[CARTROM4] = SBRomDir_[2];
		conf[COMX].rom_[CARTROM4] = SBRom_[2];
		if (expansionRamSlot_ == 4 && useExpansionRam_)
		{
			expansionRamSlot_ = 3;
			useExpansionRam_ = false;
			if (mode_.gui)
				XRCCTRL(*this, "ExpRamComx", wxCheckBox)->SetValue(useExpansionRam_);
		}
		if (mode_.gui)
		{
			XRCCTRL(*this, "Cart2RomComx", wxComboBox)->Enable(true);
			XRCCTRL(*this, "Cart2RomButtonComx", wxButton)->Enable(true);
			XRCCTRL(*this, "Cart3RomComx", wxComboBox)->Enable(true);
			XRCCTRL(*this, "Cart3RomButtonComx", wxButton)->Enable(true);
		}
    }

	if (conf[COMX].diagActive_)
	{
		conf[COMX].romDir_[EXPROM] = DiagRomDir_[0];
		conf[COMX].rom_[EXPROM] = DiagRom_[0];
        conf[COMX].romDir_[CARTROM1] = DiagRomDir_[1];
		conf[COMX].rom_[CARTROM1] = DiagRom_[1];

		if (mode_.gui)
		{
			XRCCTRL(*this, "Cart2RomComx", wxComboBox)->Enable(false);
			XRCCTRL(*this, "Cart2RomButtonComx", wxButton)->Enable(false);
			XRCCTRL(*this, "Cart3RomComx", wxComboBox)->Enable(false);
			XRCCTRL(*this, "Cart3RomButtonComx", wxButton)->Enable(false);
			XRCCTRL(*this, "Cart4RomComx", wxComboBox)->Enable(false);
			XRCCTRL(*this, "Cart4RomButtonComx", wxButton)->Enable(false);
		}
	}

	if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomComx", wxComboBox)->SetValue(conf[COMX].rom_[MAINROM1]);
		XRCCTRL(*this, "ExpRomComx", wxComboBox)->SetValue(conf[COMX].rom_[EXPROM]);
		XRCCTRL(*this, "Cart1RomComx", wxComboBox)->SetValue(conf[COMX].rom_[CARTROM1]);
		XRCCTRL(*this, "Cart4RomComx", wxComboBox)->SetValue(conf[COMX].rom_[CARTROM4]);
		if (conf[COMX].sbActive_)
			XRCCTRL(*this, "ExpRamSlotComx", wxSpinCtrl)->SetRange(1, 3);
		else
            XRCCTRL(*this, "ExpRamSlotComx", wxSpinCtrl)->SetRange(1, 4);
    }
}

void GuiComx::setSbUrlHome(wxString urlHome)
{
	sbUrlHome_ = urlHome;
	p_Main->setConfigItem("/Comx/HomePage", sbUrlHome_);
} 

void GuiComx::setSbFwVersion(int version)
{
	sbFwVersion_ = version;
	p_Main->setConfigItem("/Comx/FwVersion", sbFwVersion_);
} 

void GuiComx::setSbUrlBookMark(int number, wxString urlBookMark)
{
    sbUrlBookMark_[number] = urlBookMark;
    
    wxString numberStr;
    numberStr.Printf("/Comx/BookMark%d",number);
    p_Main->setConfigItem(numberStr, sbUrlBookMark_[number]);
} 

void GuiComx::setUrlBookMark(int number, wxString urlBookMark)
{
	sbUrlBookMark_[number] = urlBookMark;
}

void GuiComx::setSbRomDirectory(int number, wxString directory)
{
	SBRomDir_[number] = directory;
}

void GuiComx::setSbRom(int number, wxString filename)
{
	SBRom_[number] = filename;
}

void GuiComx::setDiagRomDirectory(int number, wxString directory)
{
	DiagRomDir_[number] = directory;
}

void GuiComx::setDiagRom(int number, wxString filename)
{
	DiagRom_[number] = filename;
}

void GuiComx::setEpromRomDirectory(int number, wxString directory)
{
    EpromRomDir_[number] = directory;
}

void GuiComx::setEpromRom(int number, wxString filename)
{
    EpromRom_[number] = filename;
}

void GuiComx::setAlias(long number, wxString aliasNew, wxString aliasEmailNew)
{
	if (number == -1)
	{
		number = numberOfAlias_;
		numberOfAlias_++;
		alias.resize(numberOfAlias_);
		aliasEmail.resize(numberOfAlias_);
	}

	alias[number] = aliasNew;
	aliasEmail[number] = aliasEmailNew;
}

void GuiComx::deleteAlias(int aliasDel)
{
	for (int number = aliasDel+1; number<(int)numberOfAlias_; number++)
	{
		alias[aliasDel] = alias[number];
		aliasEmail[aliasDel] = aliasEmail[number];
		aliasDel++;
	}
	numberOfAlias_--,
	alias.resize(numberOfAlias_);
	aliasEmail.resize(numberOfAlias_);
}

wxString GuiComx::getAlias(size_t number) 
{
	if (number >= numberOfAlias_)
		return "";
	else
		return alias[number];
}

wxString GuiComx::getAliasEmail(size_t number) 
{
	if (number >= numberOfAlias_)
		return "";
	else
		return aliasEmail[number];
}

void GuiComx::onLogComx(wxCommandEvent&event)
{
    conf[selectedComputer_].videoLog_ = event.IsChecked();
    if (conf[selectedComputer_].videoLog_)
        p_Main->AssInitLog();
    else
        p_Main->stopAssLog();
}


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
#include "guinetronics.h"

BEGIN_EVENT_TABLE(GuiNetronics, GuiPico)

	EVT_TEXT(XRCID("MainRomNetronics"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomNetronics"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonNetronics"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("IdeFileNetronics"), GuiMain::onIdeText)
	EVT_BUTTON(XRCID("IDE_ButtonNetronics"), GuiMain::onIde)
	EVT_BUTTON(XRCID("Eject_IDENetronics"), GuiMain::onIdeEject)

	EVT_TEXT(XRCID("KeyFileNetronics"), GuiMain::onKeyFileText)
	EVT_BUTTON(XRCID("KeyFileButtonNetronics"), GuiMain::onKeyFile)
	EVT_BUTTON(XRCID("EjectKeyFileNetronics"), GuiMain::onKeyFileEject)

	EVT_CHOICE(XRCID("VTTypeNetronics"), GuiMain::onVT100)
	EVT_CHOICE(XRCID("QsoundNetronics"), GuiElf::onQsound)
	EVT_SPIN_UP(XRCID("ZoomSpinNetronics"), GuiMain::onZoom)
	EVT_SPIN_DOWN(XRCID("ZoomSpinNetronics"), GuiMain::onZoom)
	EVT_TEXT(XRCID("ZoomValueNetronics"), GuiMain::onZoomValue)
	EVT_SPIN_UP(XRCID("ZoomSpinVtNetronics"), GuiMain::onZoomVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtNetronics"), GuiMain::onZoomVt)
	EVT_TEXT(XRCID("ZoomValueVtNetronics"), GuiMain::onZoomValueVt)
	EVT_CHECKBOX(XRCID("InterlaceNetronics"), GuiMain::onInterlace)
	EVT_CHECKBOX(XRCID("StretchDotNetronics"), GuiMain::onStretchDot)
	EVT_BUTTON(XRCID("ScreenDumpFileButtonNetronics"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileNetronics"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileNetronics"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5Netronics"), GuiMain::onScreenDump)
	EVT_BUTTON(XRCID("SaveButtonNetronics"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("DP_ButtonNetronics"), GuiMain::onDp)
	EVT_BUTTON(XRCID("VtSetupNetronics"), GuiMain::onVtSetup)
	EVT_BUTTON(XRCID("LoadButtonNetronics"), GuiMain::onLoadButton)
	EVT_BUTTON(XRCID("RunButtonNetronics"), GuiMain::onLoadRunButton)
	EVT_BUTTON(XRCID("DsaveButtonNetronics"), GuiMain::onDataSaveButton)
	EVT_CHECKBOX(XRCID("UseLocationNetronics"), GuiMain::onUseLocation)
	EVT_TEXT(XRCID("BeepFrequencyNetronics"), GuiMain::onBeepFrequency)
	EVT_BUTTON(XRCID("Rom1Netronics"), GuiNetronics::onRom1)
	EVT_TEXT(XRCID("MainRom2Netronics"), GuiMain::onMainRom2Text)
	EVT_COMBOBOX(XRCID("MainRom2Netronics"), GuiMain::onMainRom2Text)
	EVT_BUTTON(XRCID("RomButton2Netronics"), GuiMain::onMainRom2)

	EVT_BUTTON(XRCID("Rom2Netronics"), GuiNetronics::onRom2)

	EVT_TEXT(XRCID("CharRomNetronics"), GuiMain::onCharRomText)
	EVT_COMBOBOX(XRCID("CharRomNetronics"), GuiMain::onCharRomText)
	EVT_BUTTON(XRCID("CharRomButtonNetronics"), GuiMain::onCharRom)

	EVT_TEXT(XRCID("StartRamNetronics"), GuiElf::onStartRam)
	EVT_TEXT(XRCID("EndRamNetronics"), GuiElf::onEndRam)
	EVT_CHOICE(XRCID("VideoTypeNetronics"), GuiElf::onVideoType)
	EVT_CHOICE(XRCID("KeyboardNetronics"), GuiElf::onElfKeyboard)
	EVT_CHECKBOX(XRCID("UpperCaseNetronics"), GuiElf::onForceUpperCase)
    EVT_CHECKBOX(XRCID("GiantNetronics"), GuiElf::onGiantBoardMapping)
    EVT_CHECKBOX(XRCID("EFButtonsNetronics"), GuiElf::onEfButtons)

	EVT_CHOICE(XRCID("DiskTypeNetronics"), GuiElf::onDiskType)
	EVT_CHOICE(XRCID("MemoryNetronics"), GuiNetronics::onMemoryNetronics)
	EVT_BUTTON(XRCID("FullScreenF3Netronics"), GuiMain::onFullScreen)

	EVT_TEXT(XRCID("PrintFileNetronics"), GuiMain::onPrintFileText)
	EVT_BUTTON(XRCID("PrintButtonNetronics"), GuiMain::onPrintButton)
	EVT_CHOICE(XRCID("PrintModeNetronics"), GuiMain::onPrintMode)
	EVT_BUTTON(XRCID("PrintFileButtonNetronics"), GuiMain::onPrintFile)
		
	EVT_CHOICE(XRCID("VTBaudTChoiceNetronics"), GuiMain::onBaudT)
	EVT_CHOICE(XRCID("VTBaudRChoiceNetronics"), GuiMain::onBaudR)

	EVT_COMMAND(wxID_ANY, OPEN_PRINTER_WINDOW, GuiMain::openPrinterFrame) 

	EVT_CHECKBOX(XRCID("PortExtNetronics"), GuiElf::onUsePortExtender)
    EVT_CHECKBOX(XRCID("ControlWindowsNetronics"), GuiNetronics::onNetronicsControlWindows)
	EVT_CHECKBOX(XRCID("AutoBootNetronics"), GuiElf::onAutoBoot)
	EVT_TEXT(XRCID("BootAddressNetronics"), GuiNetronics::onBootAddress)
	EVT_BUTTON(XRCID("KeyMapNetronics"), Main::onHexKeyDef)

	EVT_BUTTON(XRCID("CasButtonNetronics"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasNetronics"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileNetronics"), GuiMain::onCassetteText)
	EVT_BUTTON(XRCID("RealCasLoadNetronics"), GuiMain::onRealCas)
	EVT_BUTTON(XRCID("CasLoadNetronics"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSaveNetronics"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopNetronics"), GuiMain::onCassetteStop)
	EVT_CHECKBOX(XRCID("TurboNetronics"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockNetronics"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadNetronics"), GuiMain::onAutoLoad)
	EVT_BUTTON(XRCID("TapeNetronics"), GuiElf::onTape)

	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeNetronics"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeNetronics"), GuiMain::onVolume)

    EVT_BUTTON(XRCID("ColoursNetronics"), Main::onColoursDef)
	EVT_TEXT(XRCID("ShowAddressNetronics"), GuiMain::onLedTimer)
	EVT_TEXT(XRCID("SaveStartNetronics"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndNetronics"), GuiMain::onSaveEnd)
	EVT_TEXT(XRCID("SaveExecNetronics"), GuiMain::onSaveExec)
    EVT_CHECKBOX(XRCID("BootStrapNetronics"), GuiElf::onBootStrap)
    EVT_CHECKBOX(XRCID("HexEfNetronics"), GuiElf::onHexEf)
    EVT_CHOICE(XRCID("TilTypeNetronics"), GuiElf::onTilType)

	END_EVENT_TABLE()

GuiNetronics::GuiNetronics(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiPico(title, pos, size, mode, dataDir, iniDir)
{
	tapeOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/tick.png", wxBITMAP_TYPE_PNG);
	tapeOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/minus.png", wxBITMAP_TYPE_PNG);
}

void GuiNetronics::readNetronicsConfig()
{
    return;  //*** to be removed
	bool romMode;
    
    int elfType = NETRONICS;
    wxString elfTypeStr = "Netronics";
    
	selectedComputer_ = elfType;

    readElfPortConfig(elfType, elfTypeStr);
    
	conf[elfType].volume_ = 0;

    conf[elfType].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Netronics" + pathSeparator_;
    conf[elfType].mainDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Main", dataDir_ + "Netronics" + pathSeparator_);
	conf[elfType].romDir_[MAINROM1] = readConfigDir("/Dir/"+elfTypeStr+"/Main_Rom_1_File", dataDir_ + "Netronics" + pathSeparator_);
	conf[elfType].romDir_[MAINROM2] = readConfigDir("/Dir/"+elfTypeStr+"/Main_Rom_2_File", dataDir_ + "Netronics" + pathSeparator_);
	conf[elfType].charRomDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Font_Rom_File", dataDir_ + "Netronics" + pathSeparator_);
	elfConfiguration[elfType].vtCharRomDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Vt_Font_Rom_File", dataDir_ + "Netronics" + pathSeparator_);
	conf[elfType].ramDir_ = readConfigDir("/Dir/"+elfTypeStr+"/SWD", dataDir_ + "Netronics" + pathSeparator_);
	conf[elfType].ideDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Ide_File", dataDir_ + "Netronics" + pathSeparator_);
	conf[elfType].keyFileDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Key_File", dataDir_ + "Netronics" + pathSeparator_);
	conf[elfType].printFileDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Print_File", dataDir_ + "Netronics" + pathSeparator_);
	conf[elfType].screenDumpFileDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Video_Dump_File", dataDir_ + "Netronics" + pathSeparator_);
	conf[elfType].wavFileDir_[0] = readConfigDir("/Dir/"+elfTypeStr+"/Wav_File", dataDir_ + "Netronics" + pathSeparator_);
	elfConfiguration[elfType].vtWavFileDir_ = readConfigDir("/Dir/" + elfTypeStr + "/Vt_Wav_File", dataDir_ + "Netronics" + pathSeparator_);

	conf[elfType].videoMode_ = (int)configPointer->Read(elfTypeStr+"/Video_Type", 0l);

	conf[elfType].rom_[MAINROM1] = configPointer->Read(elfTypeStr+"/Main_Rom_1_File", "bios.1.0.4.rom");
	conf[elfType].rom_[MAINROM2] = configPointer->Read(elfTypeStr+"/Main_Rom_2_File", "");
	conf[elfType].ide_ = configPointer->Read(elfTypeStr+"/Ide_File", "vt100.ide");
	conf[elfType].keyFile_ = configPointer->Read(elfTypeStr+"/Key_File", "");
	conf[elfType].printFile_ = configPointer->Read(elfTypeStr+"/Print_File", "printerout.txt");
	conf[elfType].screenDumpFile_ = configPointer->Read(elfTypeStr+"/Video_Dump_File", "screendump.png");
	conf[elfType].wavFile_[0] = configPointer->Read(elfTypeStr+"/Wav_File", "");
    conf[elfType].terminalFiles_.Add(conf[elfType].wavFile_[0]);
    conf[elfType].terminalPaths_.Add(conf[elfType].wavFileDir_[0]+conf[elfType].wavFile_[0]);
    conf[elfType].numberOfTerminalFiles_ = 1;
	elfConfiguration[elfType].vtWavFile_ = configPointer->Read(elfTypeStr + "/Vt_Wav_File", "");
    elfConfiguration[elfType].serialPort_ = configPointer->Read(elfTypeStr + "/VtSerialPortChoice", "");

	conf[elfType].saveEndString_ = "";
	conf[elfType].saveExecString_ = "";

	elfConfiguration[elfType].qSound_ = (int)configPointer->Read(elfTypeStr+"/Enable_Q_Sound", 1l);
	elfConfiguration[elfType].vtType = (int)configPointer->Read(elfTypeStr+"/VT_Type", 2l);
    elfConfiguration[elfType].vt52SetUpFeature_ = configPointer->Read(elfTypeStr+"/VT52Setup", 0x00004092l);
    elfConfiguration[elfType].vt100SetUpFeature_ = configPointer->Read(elfTypeStr+"/VT100Setup", 0x0000ca52l);
    elfConfiguration[elfType].vtExternalSetUpFeature_ = configPointer->Read(elfTypeStr+"/VTExternalSetup", 0x0000ca52l);
	elfConfiguration[elfType].baudR = (int)configPointer->Read(elfTypeStr+"/Vt_Baud_Receive", 4l);
	elfConfiguration[elfType].baudT = (int)configPointer->Read(elfTypeStr+"/Vt_Baud_Transmit", 4l);
    
    elfConfiguration[elfType].vtCharactersPerRow = (int)configPointer->Read(elfTypeStr+"/VT100CharPerRow", 80);
    elfConfiguration[elfType].vt100CharWidth = (int)configPointer->Read(elfTypeStr+"/VT100CharWidth", 10);
    elfConfiguration[elfType].vt52CharWidth = (int)configPointer->Read(elfTypeStr+"/VT52CharWidth", 9);

	elfConfiguration[elfType].diskType = (int)configPointer->Read(elfTypeStr+"/Disk_Type", 2l);
    elfConfiguration[elfType].keyboardType = (int)configPointer->Read(elfTypeStr+"/Keyboard_Type", 0l);
	elfConfiguration[elfType].memoryType = (int)configPointer->Read(elfTypeStr+"/Memory_Type", 0l);

	elfConfiguration[elfType].bellFrequency_ = (int)configPointer->Read(elfTypeStr + "/Bell_Frequency", 800);
    configPointer->Read(elfTypeStr+"/UseHexEf", &elfConfiguration[elfType].useHexKeyboardEf3, false);
    configPointer->Read(elfTypeStr+"/SerialLog", &elfConfiguration[elfType].serialLog, false);
    configPointer->Read(elfTypeStr+"/ESCError", &elfConfiguration[elfType].escError, false);
    configPointer->Read(elfTypeStr+"/Uart", &elfConfiguration[elfType].useUart, false);
    configPointer->Read(elfTypeStr+"/Uart16450", &elfConfiguration[elfType].useUart16450, false);
    configPointer->Read(elfTypeStr+"/ClearRtc", &elfConfiguration[elfType].clearRtc, false);
	configPointer->Read(elfTypeStr+"/Enable_Auto_Boot", &elfConfiguration[elfType].autoBoot, true);
    configPointer->Read(elfTypeStr+"/Force_Uppercase", &elfConfiguration[elfType].forceUpperCase, true);
	configPointer->Read(elfTypeStr+"/Enable_Printer", &conf[elfType].printerOn_, false);
	configPointer->Read(elfTypeStr+"/Enable_Extended_Ports", &elfConfiguration[elfType].usePortExtender, false);
	configPointer->Read(elfTypeStr+"/Open_Control_Windows", &elfConfiguration[elfType].useElfControlWindows, false);
	configPointer->Read(elfTypeStr+"/Enable_Interlace", &conf[elfType].interlace_, true);
	configPointer->Read(elfTypeStr+"/Enable_Vt_Stretch_Dot", &conf[elfType].stretchDot_, false);
    configPointer->Read(elfTypeStr+"/Enable_Vt_External", &elfConfiguration[elfType].vtExternal, false);
	conf[elfType].printMode_ = (int)configPointer->Read(elfTypeStr+"/Print_Mode", 1l);

    configPointer->Read(elfTypeStr+"/GiantBoardMapping", &elfConfiguration[elfType].giantBoardMapping, false);
    configPointer->Read(elfTypeStr+"/EfButtons", &elfConfiguration[elfType].efButtons, false);
    elfConfiguration[elfType].tilType = (int)configPointer->Read(elfTypeStr+"/TilType", 1l);
    configPointer->Read(elfTypeStr+"/BootStrap", &elfConfiguration[elfType].bootStrap, false);

    wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[elfType].zoom_ = convertLocale(configPointer->Read(elfTypeStr+"/Zoom", defaultZoom));
	defaultZoom.Printf("%2.2f", 1.0);
	conf[elfType].zoomVt_ = convertLocale(configPointer->Read(elfTypeStr+"/Vt_Zoom", defaultZoom));
	wxString defaultScale;
	defaultScale.Printf("%i", 3);
	conf[elfType].xScale_ = convertLocale(configPointer->Read(elfTypeStr+"/Window_Scale_Factor_X", defaultScale));

	wxString defaultClock;
	defaultClock.Printf("%1.2f", 1.76);
	conf[elfType].clock_ = convertLocale(configPointer->Read(elfTypeStr+"/Clock_Speed", defaultClock));
	wxString defaultTimer;
	defaultTimer.Printf("%d", 100);
	conf[elfType].ledTime_ = configPointer->Read(elfTypeStr+"/Led_Update_Frequency", defaultTimer);

	conf[elfType].beepFrequency_ = (int)configPointer->Read(elfTypeStr+"/Beep_Frequency", 250);

	configPointer->Read(elfTypeStr+"/Enable_Turbo_Cassette", &conf[elfType].turbo_, true);
	conf[elfType].turboClock_ = configPointer->Read(elfTypeStr+"/Turbo_Clock_Speed", "15");
	configPointer->Read(elfTypeStr+"/Enable_Auto_Cassette", &conf[elfType].autoCassetteLoad_, true);
    configPointer->Read(elfTypeStr+"/Enable_Cassette", &elfConfiguration[elfType].useTape, false);
    configPointer->Read(elfTypeStr+"/Enable_Xmodem", &elfConfiguration[elfType].useXmodem, false);
    elfConfiguration[elfType].packetSize = (int)configPointer->Read(elfTypeStr+"/Ymodem_PacketSize", 0l);
	configPointer->Read(elfTypeStr+"/Enable_Real_Cassette", &conf[elfType].realCassetteLoad_, false);
	conf[elfType].volume_ = (int)configPointer->Read(elfTypeStr+"/Volume", 25l);

	long value; 
	wxString bootAddress = configPointer->Read(elfTypeStr+"/Boot_Address", "FF00");
	if (!bootAddress.ToLong(&value, 16))
		value = 0x9000;
	conf[elfType].bootAddress_ = value;

	wxString startRam = configPointer->Read(elfTypeStr+"/Ram_Start_Address", "0");
	if (!startRam.ToLong(&value, 16))
		value = 0;
	startRam_[elfType] = value;

	wxString endRam = configPointer->Read(elfTypeStr+"/Ram_End_Address", "FFFF");
	if (!endRam.ToLong(&value, 16))
		value = 0xffff;
	endRam_[elfType] = value;

    conf[elfType].saveStartString_ = configPointer->Read(elfTypeStr+"/SaveStart", "0");
    if (!conf[elfType].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[elfType].saveStart_ = value;

    if (mode_.gui)
		setBaudChoice(elfType);

	setVtType(elfTypeStr, elfType, elfConfiguration[elfType].vtType, false);
	setVideoType(elfTypeStr, elfType, conf[elfType].videoMode_);

	conf[elfType].charRom_ = configPointer->Read(elfTypeStr+"/Font_Rom_File", "super.video.bin");
	elfConfiguration[elfType].vtCharRom_ = configPointer->Read(elfTypeStr+"/Vt_Font_Rom_File", "vt100.bin");

    configPointer->Read(elfTypeStr+"/UseLoadLocation", &conf[elfType].useLoadLocation_, false);

	if (mode_.gui)
	{
        setPrinterState(elfType);
        XRCCTRL(*this, "MainRom"+elfTypeStr, wxComboBox)->SetValue(conf[elfType].rom_[MAINROM1]);
        XRCCTRL(*this, "MainRom2"+elfTypeStr, wxComboBox)->SetValue(conf[elfType].rom_[MAINROM2]);
        XRCCTRL(*this, "CharRom"+elfTypeStr, wxComboBox)->SetValue(conf[elfType].charRom_);
        XRCCTRL(*this, "IdeFile"+elfTypeStr, wxTextCtrl)->SetValue(conf[elfType].ide_);
        XRCCTRL(*this, "KeyFile"+elfTypeStr, wxTextCtrl)->SetValue(conf[elfType].keyFile_);
        XRCCTRL(*this, "PrintFile"+elfTypeStr, wxTextCtrl)->SetValue(conf[elfType].printFile_);
        XRCCTRL(*this, "ScreenDumpFile"+elfTypeStr, wxComboBox)->SetValue(conf[elfType].screenDumpFile_);
        XRCCTRL(*this, "WavFile"+elfTypeStr, wxTextCtrl)->SetValue(conf[elfType].wavFile_[0]);
    
        XRCCTRL(*this, "Qsound"+elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].qSound_);
        if (elfConfiguration[elfType].vtExternal)
            XRCCTRL(*this, "VTType"+elfTypeStr, wxChoice)->SetSelection(EXTERNAL_TERMINAL);
        else
            XRCCTRL(*this, "VTType"+elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].vtType);
        XRCCTRL(*this, "BeepFrequency"+elfTypeStr, wxTextCtrl)->Enable(elfConfiguration[elfType].qSound_ == QSOUNDEXT);

		XRCCTRL(*this, "VTBaudRChoice" + elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].baudR);
		XRCCTRL(*this, "VTBaudTChoice" + elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].baudT);
//		XRCCTRL(*this, "VTBaudRChoice" + elfTypeStr, wxChoice)->Enable((elfConfiguration[elfType].vtType != VTNONE) && elfConfiguration[elfType].useUart);
        XRCCTRL(*this, "BeepFrequencyText"+elfTypeStr, wxStaticText)->Enable(elfConfiguration[elfType].qSound_ == QSOUNDEXT);
        XRCCTRL(*this, "BeepFrequencyTextHz"+elfTypeStr, wxStaticText)->Enable(elfConfiguration[elfType].qSound_ == QSOUNDEXT);
//        XRCCTRL(*this, "VTBaudRText" + elfTypeStr, wxStaticText)->Enable((elfConfiguration[elfType].vtType != VTNONE) && elfConfiguration[elfType].useUart);
//        XRCCTRL(*this, "VTBaudTText" + elfTypeStr, wxStaticText)->Enable(elfConfiguration[elfType].vtType != VTNONE);
        XRCCTRL(*this,"AddressText1"+elfTypeStr, wxStaticText)->Enable(elfConfiguration[elfType].useElfControlWindows);
        XRCCTRL(*this,"AddressText2"+elfTypeStr, wxStaticText)->Enable(elfConfiguration[elfType].useElfControlWindows);
//        XRCCTRL(*this, "VTBaudTChoice" + elfTypeStr, wxChoice)->Enable(elfConfiguration[elfType].vtType != VTNONE);

		XRCCTRL(*this, "VideoType"+elfTypeStr, wxChoice)->SetSelection(conf[elfType].videoMode_);
		XRCCTRL(*this, "AutoBoot"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].autoBoot);
		XRCCTRL(*this, "BootAddress"+elfTypeStr, wxTextCtrl)->SetValue(bootAddress);
		XRCCTRL(*this, "StartRam"+elfTypeStr, wxTextCtrl)->SetValue(startRam);
		XRCCTRL(*this, "EndRam"+elfTypeStr, wxTextCtrl)->SetValue(endRam);
        XRCCTRL(*this, "UpperCase"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].forceUpperCase);
		XRCCTRL(*this, "DiskType"+elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].diskType);
		XRCCTRL(*this, "Memory"+elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].memoryType);
		XRCCTRL(*this, "Keyboard"+elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].keyboardType);
        XRCCTRL(*this, "HexEf"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].useHexKeyboardEf3);

        correctZoomAndValue(elfType, elfTypeStr, SET_SPIN);
        correctZoomVtAndValue(elfType, elfTypeStr, SET_SPIN);

		XRCCTRL(*this, "PortExt"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].usePortExtender);
		XRCCTRL(*this, "ControlWindows"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].useElfControlWindows);
		XRCCTRL(*this, "Interlace"+elfTypeStr, wxCheckBox)->SetValue(conf[elfType].interlace_);
        XRCCTRL(*this, "StretchDot"+elfTypeStr, wxCheckBox)->SetValue(conf[elfType].stretchDot_);

        XRCCTRL(*this, "Giant"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].giantBoardMapping);
        XRCCTRL(*this, "EFButtons"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].efButtons);

        XRCCTRL(*this, "TilType"+elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].tilType);

        XRCCTRL(*this, "BootStrap"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].bootStrap);

		setPrinterState(elfType);
		XRCCTRL(*this, "PrintMode"+elfTypeStr, wxChoice)->SetSelection(conf[elfType].printMode_);
		setPrintMode();

        if (clockTextCtrl[elfType] != NULL)
            clockTextCtrl[elfType]->ChangeValue(conf[elfType].clock_);
		wxString beepFrequency;
		beepFrequency.Printf("%d", conf[elfType].beepFrequency_);
		XRCCTRL(*this, "BeepFrequency"+elfTypeStr, wxTextCtrl)->ChangeValue(beepFrequency);

		XRCCTRL(*this, "Turbo"+elfTypeStr, wxCheckBox)->SetValue(conf[elfType].turbo_);
		turboGui(elfTypeStr);

		XRCCTRL(*this, "TurboClock"+elfTypeStr, wxTextCtrl)->SetValue(conf[elfType].turboClock_);
		XRCCTRL(*this, "AutoCasLoad"+elfTypeStr, wxCheckBox)->SetValue(conf[elfType].autoCassetteLoad_);
		setTapeType(elfTypeStr, elfType);

		XRCCTRL(*this, "Volume"+elfTypeStr, wxSlider)->SetValue(conf[elfType].volume_);

		XRCCTRL(*this, "ShowAddress"+elfTypeStr, wxTextCtrl)->ChangeValue(conf[elfType].ledTime_);
		XRCCTRL(*this,"ShowAddress"+elfTypeStr, wxTextCtrl)->Enable(elfConfiguration[elfType].useElfControlWindows);

        XRCCTRL(*this, "UseLocation"+elfTypeStr, wxCheckBox)->SetValue(conf[elfType].useLoadLocation_);
        if (conf[elfType].saveStart_ != 0)
            XRCCTRL(*this, "SaveStart"+elfTypeStr, wxTextCtrl)->SetValue(conf[elfType].saveStartString_);

/*#ifdef __WXMAC__
        XRCCTRL(*this,"Eject_IDE"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
        XRCCTRL(*this,"EjectKeyFile"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
        XRCCTRL(*this,"Eject_IDE"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
        XRCCTRL(*this,"PrintButton"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
        XRCCTRL(*this,"ScreenDumpF5"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
        XRCCTRL(*this,"Tape"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
        XRCCTRL(*this,"EjectCas"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
#endif*/
    }

	setDiskType(elfTypeStr, elfType, elfConfiguration[elfType].diskType);
	setElfKeyboard(elfTypeStr, elfType, elfConfiguration[elfType].keyboardType, elfConfiguration[elfType].useHexKeyboardEf3);
	setRealCas(elfType);

	configPointer->Read(elfTypeStr+"/Load_Mode_Rom_1", &romMode, true);
	if (romMode)
	{
		loadromMode_[0] = ROM;
		if (mode_.gui)
		{
			XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetLabel("ROM 1");
			XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf ROM file");
		}
	}
	else
	{
		loadromMode_[0] = RAM;
		if (mode_.gui)
		{
			XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetLabel("RAM 1");
			XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf RAM file");
		}
	}

	configPointer->Read(elfTypeStr+"/Load_Mode_Rom_2", &romMode, false);
	if (romMode)
	{
		loadromMode_[1] = ROM;
		if (mode_.gui)
		{
			XRCCTRL(*this, "RomButton2"+elfTypeStr, wxButton)->SetLabel("ROM 2");
			XRCCTRL(*this, "RomButton2"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf ROM file");
		}
	}
	else
	{
		loadromMode_[1] = RAM;
		if (mode_.gui)
		{
			XRCCTRL(*this, "RomButton2"+elfTypeStr, wxButton)->SetLabel("RAM 2");
			XRCCTRL(*this, "RomButton2"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf RAM file");
		}
	}

    setMemoryNetronics(elfTypeStr, elfType, elfConfiguration[elfType].memoryType);

	if (elfType == ELF)
	{
		configPointer->Read("/Elf/Enable_Led_Module", &elfConfiguration[ELF].useLedModule, false);
		if (mode_.gui)
		{
			XRCCTRL(*this, "UseLedModule", wxCheckBox)->SetValue(elfConfiguration[ELF].useLedModule);
			XRCCTRL(*this,"UseLedModule",wxCheckBox)->Enable(elfConfiguration[ELF].useElfControlWindows);
			if (!elfConfiguration[ELF].useElfControlWindows)
				XRCCTRL(*this,"UseLedModule",wxCheckBox)->SetValue(false);
		}
	}
	else
	{
		elfConfiguration[elfType].useLedModule = false;
	}

	conf[elfType].loadFileNameFull_ = "";
	conf[elfType].loadFileName_ = "";

	conf[elfType].pLoadSaveName_[0] = 'S';
	conf[elfType].pLoadSaveName_[1] = 'U';
	conf[elfType].pLoadSaveName_[2] = 'P';
	conf[elfType].pLoadSaveName_[3] = 'E';

	conf[elfType].defus_ = 0x2a81;
	conf[elfType].eop_ = 0x2a83;
	conf[elfType].string_ = 0x2a92;
	conf[elfType].arrayValue_ = 0x2a94;
	conf[elfType].eod_ = 0x2a99;
	conf[elfType].basicRamAddress_ = 0x2c00;
}

void GuiNetronics::writeNetronicsDirConfig()
{
    return;  //*** to be removed
    int elfType = NETRONICS;
    wxString elfTypeStr = "Netronics";
    
	writeConfigDir("/Dir/" + elfTypeStr + "/Main", conf[elfType].mainDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Main_Rom_1_File", conf[elfType].romDir_[MAINROM1]);
	writeConfigDir("/Dir/" + elfTypeStr + "/Main_Rom_2_File", conf[elfType].romDir_[MAINROM2]);
	writeConfigDir("/Dir/" + elfTypeStr + "/Font_Rom_File", conf[elfType].charRomDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Vt_Font_Rom_File", elfConfiguration[elfType].vtCharRomDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Software_File", conf[elfType].ramDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Ide_File", conf[elfType].ideDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Key_File", conf[elfType].keyFileDir_);
    writeConfigDir("/Dir/" + elfTypeStr + "/Print_File", conf[elfType].printFileDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Video_Dump_File", conf[elfType].screenDumpFileDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Wav_File", conf[elfType].wavFileDir_[0]);
	writeConfigDir("/Dir/" + elfTypeStr + "/Vt_Wav_File", elfConfiguration[elfType].vtWavFileDir_);
}
	
void GuiNetronics::writeNetronicsConfig()
{
    return;  //*** to be removed
	wxString buffer;
    
    int elfType = NETRONICS;
    wxString elfTypeStr = "Netronics";
    
	writeElfPortConfig(elfType, elfTypeStr);

	configPointer->Write(elfTypeStr + "/Main_Rom_1_File", conf[elfType].rom_[MAINROM1]);
	configPointer->Write(elfTypeStr+"/Main_Rom_2_File", conf[elfType].rom_[MAINROM2]);
	configPointer->Write(elfTypeStr+"/Font_Rom_File", conf[elfType].charRom_);
	configPointer->Write(elfTypeStr+"/Vt_Font_Rom_File", elfConfiguration[elfType].vtCharRom_);
	configPointer->Write(elfTypeStr+"/Ide_File", conf[elfType].ide_);
	configPointer->Write(elfTypeStr+"/Key_File", conf[elfType].keyFile_);
	configPointer->Write(elfTypeStr+"/Print_File", conf[elfType].printFile_);
	configPointer->Write(elfTypeStr+"/Video_Dump_File", conf[elfType].screenDumpFile_);
	configPointer->Write(elfTypeStr+"/Wav_File", conf[elfType].wavFile_[0]);
	configPointer->Write(elfTypeStr+"/Vt_Wav_File", elfConfiguration[elfType].vtWavFile_);
    configPointer->Write(elfTypeStr+"/VtSerialPortChoice", elfConfiguration[elfType].serialPort_);

	configPointer->Write(elfTypeStr+"/Load_Mode_Rom_1", (loadromMode_[0] == ROM));
	configPointer->Write(elfTypeStr+"/Load_Mode_Rom_2", (loadromMode_[1] == ROM));
	configPointer->Write(elfTypeStr+"/Disk_Type", elfConfiguration[elfType].diskType);
	configPointer->Write(elfTypeStr+"/Enable_Q_Sound", elfConfiguration[elfType].qSound_);
	configPointer->Write(elfTypeStr+"/Memory_Type", elfConfiguration[elfType].memoryType);
	buffer.Printf("%04X", (unsigned int)startRam_[elfType]);
	configPointer->Write(elfTypeStr+"/Ram_Start_Address", buffer);
	buffer.Printf("%04X", (unsigned int)endRam_[elfType]);
	configPointer->Write(elfTypeStr+"/Ram_End_Address", buffer);
	configPointer->Write(elfTypeStr+"/VT_Type", elfConfiguration[elfType].vtType);

    long value = elfConfiguration[elfType].vt52SetUpFeature_.to_ulong();
    configPointer->Write(elfTypeStr+"/VT52Setup", value);
    value = elfConfiguration[elfType].vt100SetUpFeature_.to_ulong();
    configPointer->Write(elfTypeStr+"/VT100Setup", value);
    value = elfConfiguration[elfType].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write(elfTypeStr+"/VTExternalSetup", value);
    configPointer->Write(elfTypeStr+"/VT100CharPerRow", elfConfiguration[elfType].vtCharactersPerRow);
    configPointer->Write(elfTypeStr+"/VT100CharWidth", elfConfiguration[elfType].vt100CharWidth);
    configPointer->Write(elfTypeStr+"/VT52CharWidth", elfConfiguration[elfType].vt52CharWidth);

	configPointer->Write(elfTypeStr+"/Vt_Baud_Receive", elfConfiguration[elfType].baudR);
	configPointer->Write(elfTypeStr+"/Vt_Baud_Transmit", elfConfiguration[elfType].baudT);
	configPointer->Write(elfTypeStr + "/Bell_Frequency", elfConfiguration[elfType].bellFrequency_);
    configPointer->Write(elfTypeStr+"/SerialLog", elfConfiguration[elfType].serialLog);
    configPointer->Write(elfTypeStr+"/ESCError", elfConfiguration[elfType].escError);
    configPointer->Write(elfTypeStr+"/Uart", elfConfiguration[elfType].useUart);
    configPointer->Write(elfTypeStr+"/Uart16450", elfConfiguration[elfType].useUart16450);
    configPointer->Write(elfTypeStr+"/ClearRtc", elfConfiguration[elfType].clearRtc);
    configPointer->Write(elfTypeStr+"/Enable_Auto_Boot", elfConfiguration[elfType].autoBoot);
	buffer.Printf("%04X", (unsigned int)conf[elfType].bootAddress_);
	configPointer->Write(elfTypeStr+"/Boot_Address", buffer);
	configPointer->Write(elfTypeStr+"/Video_Type", conf[elfType].videoMode_);
    configPointer->Write(elfTypeStr+"/Keyboard_Type", elfConfiguration[elfType].keyboardType);
    configPointer->Write(elfTypeStr+"/UseHexEf", elfConfiguration[elfType].useHexKeyboardEf3);
	configPointer->Write(elfTypeStr+"/Zoom", conf[elfType].zoom_);
	configPointer->Write(elfTypeStr+"/Vt_Zoom", conf[elfType].zoomVt_);
    configPointer->Write(elfTypeStr+"/Force_Uppercase", elfConfiguration[elfType].forceUpperCase);
    configPointer->Write(elfTypeStr+"/UseLoadLocation", conf[elfType].useLoadLocation_);
    configPointer->Write(elfTypeStr+"/SaveStart", conf[elfType].saveStartString_);

    configPointer->Write(elfTypeStr+"/GiantBoardMapping", elfConfiguration[elfType].giantBoardMapping);
    configPointer->Write(elfTypeStr+"/EfButtons", elfConfiguration[elfType].efButtons);

	configPointer->Write(elfTypeStr+"/Enable_Printer", conf[elfType].printerOn_);
	configPointer->Write(elfTypeStr + "/Print_Mode", conf[elfType].printMode_);

    configPointer->Write(elfTypeStr+"/Enable_Extended_Ports", elfConfiguration[elfType].usePortExtender);
	configPointer->Write(elfTypeStr+"/Open_Control_Windows", elfConfiguration[elfType].useElfControlWindows);
	configPointer->Write(elfTypeStr+"/Enable_Interlace", conf[elfType].interlace_);
	configPointer->Write(elfTypeStr+"/Enable_Vt_Stretch_Dot", conf[elfType].stretchDot_);
    configPointer->Write(elfTypeStr+"/Enable_Vt_External", elfConfiguration[elfType].vtExternal);

	configPointer->Write(elfTypeStr+"/Clock_Speed", conf[elfType].clock_);
	configPointer->Write(elfTypeStr+"/Beep_Frequency", conf[elfType].beepFrequency_);

    configPointer->Write(elfTypeStr+"/TilType", elfConfiguration[elfType].tilType);

    configPointer->Write(elfTypeStr+"/BootStrap", elfConfiguration[elfType].bootStrap);

    configPointer->Write(elfTypeStr+"/Led_Update_Frequency", conf[elfType].ledTime_);
	configPointer->Write(elfTypeStr+"/Enable_Turbo_Cassette", conf[elfType].turbo_);
	configPointer->Write(elfTypeStr+"/Turbo_Clock_Speed", conf[elfType].turboClock_);
	configPointer->Write(elfTypeStr+"/Enable_Auto_Cassette", conf[elfType].autoCassetteLoad_);
	configPointer->Write(elfTypeStr+"/Enable_Real_Cassette", conf[elfType].realCassetteLoad_);
    configPointer->Write(elfTypeStr+"/Enable_Cassette", elfConfiguration[elfType].useTape);
    configPointer->Write(elfTypeStr+"/Enable_Xmodem", elfConfiguration[elfType].useXmodem);
    configPointer->Write(elfTypeStr+"/Ymodem_PacketSize", elfConfiguration[elfType].packetSize);
	configPointer->Write(elfTypeStr+"/Volume", conf[elfType].volume_);
}

void GuiNetronics::readNetronicsWindowConfig()
{
    return;  //*** to be removed
    int elfType = NETRONICS;
    wxString elfTypeStr = "Netronics";
    
    conf[elfType].pixieX_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Pixie_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[elfType].pixieY_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Pixie_Y", mainWindowY_);
    conf[elfType].tmsX_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Tms_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[elfType].tmsY_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Tms_Y", mainWindowY_);
    conf[elfType].mc6845X_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_MC6845_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[elfType].mc6845Y_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_MC6845_Y", mainWindowY_);
    conf[elfType].mc6847X_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_MC6847_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[elfType].mc6847Y_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_MC6847_Y", mainWindowY_);
    conf[elfType].i8275X_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_I8275_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[elfType].i8275Y_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_I8275_Y", mainWindowY_);
    conf[elfType].vtX_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[elfType].vtY_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Vt_Y", mainWindowY_);
    conf[elfType].mainX_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_X", mainWindowX_);
    conf[elfType].mainY_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    
    if (elfType == ELF)
    {
        ledModuleX_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Led_Module_X", mainWindowX_+346+windowInfo.xBorder2);
        ledModuleY_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Led_Module_Y", mainWindowY_+windowInfo.mainwY+229+windowInfo.yBorder2);
        conf[elfType].keypadX_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Keypad_X", mainWindowX_+346+windowInfo.xBorder2);
        conf[elfType].keypadY_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Keypad_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    }
}

void GuiNetronics::writeNetronicsWindowConfig()
{
    return;  //*** to be removed
    int elfType = NETRONICS;
    wxString elfTypeStr = "Netronics";
    
	if (conf[elfType].mainX_ > 0)
		configPointer->Write(elfTypeStr + "/MainX", conf[elfType].mainX_);
	if (conf[elfType].mainY_ > 0)
		configPointer->Write(elfTypeStr + "/MainY", conf[elfType].mainY_);
	if (conf[elfType].pixieX_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_Pixie_X", conf[elfType].pixieX_);
	if (conf[elfType].pixieY_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_Pixie_Y", conf[elfType].pixieY_);
	if (conf[elfType].tmsX_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_Tms_X", conf[elfType].tmsX_);
	if (conf[elfType].tmsY_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_Tms_Y", conf[elfType].tmsY_);
	if (conf[elfType].mc6845X_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_MC6845_X", conf[elfType].mc6845X_);
	if (conf[elfType].mc6845Y_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_MC6845_Y", conf[elfType].mc6845Y_);
	if (conf[elfType].mc6847X_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_MC6847_X", conf[elfType].mc6847X_);
	if (conf[elfType].mc6847Y_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_MC6847_Y", conf[elfType].mc6847Y_);
	if (conf[elfType].i8275X_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_I8275_X", conf[elfType].i8275X_);
	if (conf[elfType].i8275Y_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_I8275_Y", conf[elfType].i8275Y_);
	if (conf[elfType].vtX_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_Vt_X", conf[elfType].vtX_);
	if (conf[elfType].vtY_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_Vt_Y", conf[elfType].vtY_);

	if (elfType == ELF)
	{
		if (conf[elfType].keypadX_ > 0)
			configPointer->Write(elfTypeStr + "/Window_Position_Keypad_X", conf[elfType].keypadX_);
		if (conf[elfType].keypadY_ > 0)
			configPointer->Write(elfTypeStr + "/Window_Position_Keypad_Y", conf[elfType].keypadY_);
		if (ledModuleX_ > 0)
			configPointer->Write(elfTypeStr + "/Window_Position_Led_Module_X", ledModuleX_);
		if (ledModuleY_ > 0)
			configPointer->Write(elfTypeStr + "/Window_Position_Led_Module_Y", ledModuleY_);
	}
}

void GuiNetronics::onMemoryNetronics(wxCommandEvent&event)
{
	setMemoryNetronics(computerInfo[selectedComputer_].gui, selectedComputer_, event.GetSelection());
}

void GuiNetronics::setMemoryNetronics(wxString elfTypeStr, int elfType, int Selection)
{
    if (elfConfiguration[selectedComputer_].memoryType == 3)
    {
        if (mode_.gui)
        {
            if (loadromMode_[0] == ROM)
            {
                XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetLabel("ROM 1");
                XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf ROM file");
            }
            else
            {
                XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetLabel("RAM 1");
                XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf RAM file");
            }
            XRCCTRL(*this, "Rom1"+elfTypeStr, wxBitmapButton)->Enable(true);
        }
    }
    
    elfConfiguration[selectedComputer_].memoryType = Selection;
	switch(Selection)
	{
		case MEM_64K:
			elfConfiguration[elfType].useEms = false;
            elfConfiguration[elfType].usePager = false;
            elfConfiguration[elfType].useRomMapper = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "StartRamText"+elfTypeStr, wxStaticText)->Enable(true);
				XRCCTRL(*this, "StartRam"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "EndRamText"+elfTypeStr, wxStaticText)->Enable(true);
				XRCCTRL(*this, "EndRam"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "PortExt"+elfTypeStr, wxCheckBox)->Enable(true);
			}
		break;

		case MEM_EMS:
			elfConfiguration[elfType].useEms = true;
			elfConfiguration[elfType].usePager = false;
            elfConfiguration[elfType].useRomMapper = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "StartRamText"+elfTypeStr, wxStaticText)->Enable(false);
				XRCCTRL(*this, "StartRam"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this, "EndRamText"+elfTypeStr, wxStaticText)->Enable(false);
				XRCCTRL(*this, "EndRam"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this, "PortExt"+elfTypeStr, wxCheckBox)->Enable(true);
			}
		break;

		case MEM_MAP:
			elfConfiguration[elfType].useEms = false;
			elfConfiguration[elfType].usePager = true;
			elfConfiguration[elfType].usePortExtender = true;
            elfConfiguration[elfType].useRomMapper = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "PortExt"+elfTypeStr, wxCheckBox)->Enable(false);
				XRCCTRL(*this, "PortExt"+elfTypeStr, wxCheckBox)->SetValue(true);
				XRCCTRL(*this, "StartRamText"+elfTypeStr, wxStaticText)->Enable(false);
				XRCCTRL(*this, "StartRam"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this, "EndRamText"+elfTypeStr, wxStaticText)->Enable(false);
				XRCCTRL(*this, "EndRam"+elfTypeStr, wxTextCtrl)->Enable(false);
			}
		break;

        case MEM_ROM_MAP:
            elfConfiguration[elfType].useEms = false;
            elfConfiguration[elfType].usePager = false;
            elfConfiguration[elfType].useRomMapper = true;
            if (mode_.gui)
            {
                XRCCTRL(*this, "StartRamText"+elfTypeStr, wxStaticText)->Enable(false);
                XRCCTRL(*this, "StartRam"+elfTypeStr, wxTextCtrl)->Enable(false);
                XRCCTRL(*this, "EndRamText"+elfTypeStr, wxStaticText)->Enable(false);
                XRCCTRL(*this, "EndRam"+elfTypeStr, wxTextCtrl)->Enable(false);
                XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetLabel("Mapper");
                XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf ROM Mapper file");
                XRCCTRL(*this, "Rom1"+elfTypeStr, wxBitmapButton)->Enable(false);
                XRCCTRL(*this, "EndRam"+elfTypeStr, wxTextCtrl)->SetValue("7FFF");
            }
        break;
    }
}

void GuiNetronics::onNetronicsControlWindows(wxCommandEvent&event)
{
	elfConfiguration[selectedComputer_].useElfControlWindows = event.IsChecked();

	if (mode_.gui)
	{
		XRCCTRL(*this,"ShowAddress"+computerInfo[selectedComputer_].gui,wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText1"+computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
		XRCCTRL(*this,"AddressText2"+computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
	}

    p_Netronics->Show(elfConfiguration[runningComputer_].useElfControlWindows);
}

void GuiNetronics::onRom1(wxCommandEvent&WXUNUSED(event))
{
	if (loadromMode_[0] == ROM)
	{
		loadromMode_[0] = RAM;
		XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 1");
		XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetToolTip("Browse for Elf RAM file");
	}
	else
	{
		loadromMode_[0] = ROM;
		XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("ROM 1");
		XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetToolTip("Browse for Elf ROM file");
	}
}

void GuiNetronics::onRom2(wxCommandEvent&WXUNUSED(event))
{
	if (loadromMode_[1] == ROM)
	{
		loadromMode_[1] = RAM;
		XRCCTRL(*this, "RomButton2"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 2");
		XRCCTRL(*this, "RomButton2"+computerInfo[selectedComputer_].gui, wxButton)->SetToolTip("Browse for Elf RAM file");
	}
	else
	{
		loadromMode_[1] = ROM;
		XRCCTRL(*this, "RomButton2"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("ROM 2");
		XRCCTRL(*this, "RomButton2"+computerInfo[selectedComputer_].gui, wxButton)->SetToolTip("Browse for Elf ROM file");
	}
}

int GuiNetronics::getLoadromModeNetronics(int num)
{
    return loadromMode_[num];
}

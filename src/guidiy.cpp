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
#include "guidiy.h"

BEGIN_EVENT_TABLE(GuiDiy, GuiPico)

    EVT_TEXT(XRCID("MainRamDiy"), GuiDiy::onMainRamTextDiy)
    EVT_COMBOBOX(XRCID("MainRamDiy"), GuiDiy::onMainRamTextDiy)
    EVT_BUTTON(XRCID("RamButtonDiy"), GuiDiy::onMainRamDiy)

    EVT_TEXT(XRCID("MainXmlDiy"), GuiDiy::onMainXmlTextDiy)
    EVT_COMBOBOX(XRCID("MainXmlDiy"), GuiDiy::onMainXmlTextDiy)
    EVT_BUTTON(XRCID("XmlButtonDiy"), GuiDiy::onMainXmlDiy)

    EVT_TEXT(XRCID("IdeFileDiy"), GuiMain::onIdeText)
	EVT_BUTTON(XRCID("IDE_ButtonDiy"), GuiMain::onIde)
	EVT_BUTTON(XRCID("Eject_IDEDiy"), GuiMain::onIdeEject)

	EVT_TEXT(XRCID("KeyFileDiy"), GuiMain::onKeyFileText)
	EVT_BUTTON(XRCID("KeyFileButtonDiy"), GuiMain::onKeyFile)
	EVT_BUTTON(XRCID("EjectKeyFileDiy"), GuiMain::onKeyFileEject)

	EVT_CHOICE(XRCID("VTTypeDiy"), GuiMain::onVT100)
	EVT_CHOICE(XRCID("QsoundDiy"), GuiElf::onQsound)
	EVT_SPIN_UP(XRCID("ZoomSpinDiy"), GuiMain::onZoom)
	EVT_SPIN_DOWN(XRCID("ZoomSpinDiy"), GuiMain::onZoom)
	EVT_TEXT(XRCID("ZoomValueDiy"), GuiMain::onZoomValue)
	EVT_SPIN_UP(XRCID("ZoomSpinVtDiy"), GuiMain::onZoomVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtDiy"), GuiMain::onZoomVt)
	EVT_TEXT(XRCID("ZoomValueVtDiy"), GuiMain::onZoomValueVt)
	EVT_CHECKBOX(XRCID("InterlaceDiy"), GuiMain::onInterlace)
	EVT_CHECKBOX(XRCID("StretchDotDiy"), GuiMain::onStretchDot)
	EVT_BUTTON(XRCID("ScreenDumpFileButtonDiy"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileDiy"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileDiy"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5Diy"), GuiMain::onScreenDump)
	EVT_BUTTON(XRCID("SaveButtonDiy"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("DP_ButtonDiy"), GuiMain::onDp)
	EVT_BUTTON(XRCID("VtSetupDiy"), GuiMain::onVtSetup)
	EVT_BUTTON(XRCID("LoadButtonDiy"), GuiMain::onLoadButton)
	EVT_BUTTON(XRCID("RunButtonDiy"), GuiMain::onLoadRunButton)
	EVT_BUTTON(XRCID("DsaveButtonDiy"), GuiMain::onDataSaveButton)
	EVT_CHECKBOX(XRCID("UseLocationDiy"), GuiMain::onUseLocation)
	EVT_TEXT(XRCID("BeepFrequencyDiy"), GuiMain::onBeepFrequency)
	EVT_TEXT(XRCID("CharRomDiy"), GuiMain::onCharRomText)
	EVT_COMBOBOX(XRCID("CharRomDiy"), GuiMain::onCharRomText)
	EVT_BUTTON(XRCID("CharRomButtonDiy"), GuiMain::onCharRom)

	EVT_CHOICE(XRCID("VideoTypeDiy"), GuiElf::onVideoType)
	EVT_CHOICE(XRCID("KeyboardDiy"), GuiElf::onElfKeyboard)
	EVT_CHECKBOX(XRCID("UpperCaseDiy"), GuiElf::onForceUpperCase)
    EVT_CHECKBOX(XRCID("EFButtonsDiy"), GuiElf::onEfButtons)

	EVT_CHOICE(XRCID("DiskTypeDiy"), GuiElf::onDiskType)
	EVT_BUTTON(XRCID("FullScreenF3Diy"), GuiMain::onFullScreen)

	EVT_TEXT(XRCID("PrintFileDiy"), GuiMain::onPrintFileText)
	EVT_BUTTON(XRCID("PrintButtonDiy"), GuiMain::onPrintButton)
	EVT_CHOICE(XRCID("PrintModeDiy"), GuiMain::onPrintMode)
	EVT_BUTTON(XRCID("PrintFileButtonDiy"), GuiMain::onPrintFile)
		
	EVT_CHOICE(XRCID("VTBaudTChoiceDiy"), GuiMain::onBaudT)
	EVT_CHOICE(XRCID("VTBaudRChoiceDiy"), GuiMain::onBaudR)

	EVT_COMMAND(wxID_ANY, OPEN_PRINTER_WINDOW, GuiMain::openPrinterFrame) 

	EVT_CHECKBOX(XRCID("PortExtDiy"), GuiElf::onUsePortExtender)
    EVT_CHECKBOX(XRCID("ControlWindowsDiy"), GuiDiy::onNetronicsControlWindows)
	EVT_CHECKBOX(XRCID("AutoBootDiy"), GuiElf::onAutoBoot)
	EVT_TEXT(XRCID("BootAddressDiy"), GuiDiy::onBootAddress)
	EVT_BUTTON(XRCID("KeyMapDiy"), Main::onHexKeyDef)

	EVT_BUTTON(XRCID("CasButtonDiy"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasDiy"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileDiy"), GuiMain::onCassetteText)
	EVT_BUTTON(XRCID("RealCasLoadDiy"), GuiMain::onRealCas)
	EVT_BUTTON(XRCID("CasLoadDiy"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSaveDiy"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopDiy"), GuiMain::onCassetteStop)
	EVT_CHECKBOX(XRCID("TurboDiy"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockDiy"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadDiy"), GuiMain::onAutoLoad)
	EVT_BUTTON(XRCID("TapeDiy"), GuiElf::onTape)

	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeDiy"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeDiy"), GuiMain::onVolume)

    EVT_BUTTON(XRCID("ColoursDiy"), Main::onColoursDef)
	EVT_TEXT(XRCID("ShowAddressDiy"), GuiMain::onLedTimer)
	EVT_TEXT(XRCID("SaveStartDiy"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndDiy"), GuiMain::onSaveEnd)
	EVT_TEXT(XRCID("SaveExecDiy"), GuiMain::onSaveExec)
    EVT_CHECKBOX(XRCID("BootStrapDiy"), GuiElf::onBootStrap)
    EVT_CHECKBOX(XRCID("HexEfDiy"), GuiElf::onHexEf)
    EVT_CHOICE(XRCID("TilTypeDiy"), GuiElf::onTilType)

	END_EVENT_TABLE()

GuiDiy::GuiDiy(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiPico(title, pos, size, mode, dataDir, iniDir)
{
	tapeOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/tick.png", wxBITMAP_TYPE_PNG);
	tapeOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/minus.png", wxBITMAP_TYPE_PNG);
}

void GuiDiy::readNetronicsConfig()
{
    selectedComputer_ = DIY; // *** to be removed
    readElfPortConfig(DIY, "Diy"); // *** to be removed

    conf[DIY].xmlDir_ = readConfigDir("Dir/Diy/Xml_File", dataDir_);;
    conf[DIY].xmlFile_ = configPointer->Read("Diy/Xml_File", "example.xml");

    parseXmlFile(DIY,conf[DIY].xmlDir_, conf[DIY].xmlFile_);
    
    if (conf[DIY].memConfig_[0].filename == "")
    {
        conf[DIY].memConfig_[0].dirname = readConfigDir("/Dir/Diy/Main_Ram_File", dataDir_);
        conf[DIY].memConfig_[0].filename = configPointer->Read("Diy/Main_Ram_File", "");
        ramFileFromGui_ = true;
    }
    else
        ramFileFromGui_ = false;

    int elfType = DIY;
    wxString elfTypeStr = "Diy";
    
	selectedComputer_ = elfType;

	conf[elfType].volume_ = 0;

    conf[elfType].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + computerInfo[DIY].gui + pathSeparator_;
    conf[elfType].mainDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Main", dataDir_ + computerInfo[DIY].gui + pathSeparator_);
	conf[elfType].charRomDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Font_Rom_File", dataDir_ + computerInfo[DIY].gui + pathSeparator_);
	elfConfiguration[elfType].vtCharRomDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Vt_Font_Rom_File", dataDir_ + computerInfo[DIY].gui + pathSeparator_);
	conf[elfType].ramDir_ = readConfigDir("/Dir/"+elfTypeStr+"/SWD", dataDir_ + computerInfo[DIY].gui + pathSeparator_);
	conf[elfType].ideDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Ide_File", dataDir_ + computerInfo[DIY].gui + pathSeparator_);
	conf[elfType].keyFileDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Key_File", dataDir_ + computerInfo[DIY].gui + pathSeparator_);
	conf[elfType].printFileDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Print_File", dataDir_ + computerInfo[DIY].gui + pathSeparator_);
	conf[elfType].screenDumpFileDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Video_Dump_File", dataDir_ + computerInfo[DIY].gui + pathSeparator_);
	conf[elfType].wavFileDir_[0] = readConfigDir("/Dir/"+elfTypeStr+"/Wav_File", dataDir_ + computerInfo[DIY].gui + pathSeparator_);
	elfConfiguration[elfType].vtWavFileDir_ = readConfigDir("/Dir/" + elfTypeStr + "/Vt_Wav_File", dataDir_ + computerInfo[DIY].gui + pathSeparator_);

	conf[elfType].videoMode_ = (int)configPointer->Read(elfTypeStr+"/Video_Type", 0l);

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

        XRCCTRL(*this, "MainXmlDiy", wxComboBox)->SetValue(conf[DIY].xmlFile_);
        XRCCTRL(*this, "MainRamDiy", wxComboBox)->SetValue(conf[elfType].memConfig_[0].filename);
        if (!ramFileFromGui_)
        {
            XRCCTRL(*this,"MainRamDiy", wxComboBox)->Enable(false);
            XRCCTRL(*this,"RamButtonDiy", wxButton)->Enable(false);
        }

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
        XRCCTRL(*this, "UpperCase"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].forceUpperCase);
		XRCCTRL(*this, "DiskType"+elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].diskType);
		XRCCTRL(*this, "Keyboard"+elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].keyboardType);
        XRCCTRL(*this, "HexEf"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].useHexKeyboardEf3);

        correctZoomAndValue(elfType, elfTypeStr, SET_SPIN);
        correctZoomVtAndValue(elfType, elfTypeStr, SET_SPIN);

		XRCCTRL(*this, "PortExt"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].usePortExtender);
		XRCCTRL(*this, "ControlWindows"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].useElfControlWindows);
		XRCCTRL(*this, "Interlace"+elfTypeStr, wxCheckBox)->SetValue(conf[elfType].interlace_);
        XRCCTRL(*this, "StretchDot"+elfTypeStr, wxCheckBox)->SetValue(conf[elfType].stretchDot_);

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

void GuiDiy::writeNetronicsDirConfig()
{
    int elfType = DIY;
    wxString elfTypeStr = "Diy";
    
	writeConfigDir("/Dir/Diy/Main", conf[DIY].mainDir_);
    writeConfigDir("/Dir/Diy/Xml_File", conf[DIY].xmlDir_);
    if (ramFileFromGui_)
        writeConfigDir("/Dir/Diy/Main_Ram_File", conf[DIY].memConfig_[0].dirname);

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
	
void GuiDiy::writeNetronicsConfig()
{
	wxString buffer;
    
    int elfType = DIY;
    wxString elfTypeStr = "Diy";
    
	writeElfPortConfig(elfType, elfTypeStr);

    configPointer->Write("Diy/Xml_File", conf[DIY].xmlFile_);
    if (ramFileFromGui_)
        configPointer->Write("Diy/Main_Ram_File", conf[DIY].memConfig_[0].filename);

	configPointer->Write(elfTypeStr+"/Font_Rom_File", conf[elfType].charRom_);
	configPointer->Write(elfTypeStr+"/Vt_Font_Rom_File", elfConfiguration[elfType].vtCharRom_);
	configPointer->Write(elfTypeStr+"/Ide_File", conf[elfType].ide_);
	configPointer->Write(elfTypeStr+"/Key_File", conf[elfType].keyFile_);
	configPointer->Write(elfTypeStr+"/Print_File", conf[elfType].printFile_);
	configPointer->Write(elfTypeStr+"/Video_Dump_File", conf[elfType].screenDumpFile_);
	configPointer->Write(elfTypeStr+"/Wav_File", conf[elfType].wavFile_[0]);
	configPointer->Write(elfTypeStr+"/Vt_Wav_File", elfConfiguration[elfType].vtWavFile_);
    configPointer->Write(elfTypeStr+"/VtSerialPortChoice", elfConfiguration[elfType].serialPort_);

	configPointer->Write(elfTypeStr+"/Disk_Type", elfConfiguration[elfType].diskType);
	configPointer->Write(elfTypeStr+"/Enable_Q_Sound", elfConfiguration[elfType].qSound_);
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

void GuiDiy::readNetronicsWindowConfig()
{
    int elfType = DIY;
    wxString elfTypeStr = "Diy";
    
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

void GuiDiy::writeNetronicsWindowConfig()
{
    int elfType = DIY;
    wxString elfTypeStr = "Diy";
    
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

void GuiDiy::onNetronicsControlWindows(wxCommandEvent&event)
{
	elfConfiguration[selectedComputer_].useElfControlWindows = event.IsChecked();

	if (mode_.gui)
	{
		XRCCTRL(*this,"ShowAddress"+computerInfo[selectedComputer_].gui,wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText1"+computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
		XRCCTRL(*this,"AddressText2"+computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
	}

    p_Diy->Show(elfConfiguration[runningComputer_].useElfControlWindows);
}

void GuiDiy::onMainRamDiy(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;

    fileName = wxFileSelector( "Select the RAM file to load",
                              conf[selectedComputer_].memConfig_[0].dirname, XRCCTRL(*this, "MainRamDiy", wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                   "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
    if (!fileName)
        return;

    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    conf[selectedComputer_].memConfig_[0].dirname = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].memConfig_[0].filename = FullPath.GetFullName();

    XRCCTRL(*this, "MainRamDiy", wxComboBox)->SetValue(conf[selectedComputer_].memConfig_[0].filename);
}

void GuiDiy::onMainRamTextDiy(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].memConfig_[0].filename = XRCCTRL(*this, "MainRamDiy", wxComboBox)->GetValue();
}

void GuiDiy::onMainXmlDiy(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;

    fileName = wxFileSelector( "Select the XML file to load",
                              conf[selectedComputer_].xmlDir_, XRCCTRL(*this, "MainXmlDiy", wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                   "XML|*.xml|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
    if (!fileName)
        return;

    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    conf[selectedComputer_].xmlDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].xmlFile_ = FullPath.GetFullName();

    XRCCTRL(*this, "MainXmlDiy", wxComboBox)->SetValue(conf[selectedComputer_].xmlFile_);
}

void GuiDiy::onMainXmlTextDiy(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].xmlFile_ = XRCCTRL(*this, "MainXmlDiy", wxComboBox)->GetValue();
    
    parseXmlFile(selectedComputer_, conf[selectedComputer_].xmlDir_, conf[selectedComputer_].xmlFile_);
}

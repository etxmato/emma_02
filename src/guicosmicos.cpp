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
#include "guielf2k.h"

BEGIN_EVENT_TABLE(GuiCosmicos, GuiMicrotutor2)

	EVT_TEXT(XRCID("MainRomCosmicos"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomCosmicos"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonCosmicos"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("VtCharRomCosmicos"), GuiMain::onVtCharRomText)
	EVT_COMBOBOX(XRCID("VtCharRomCosmicos"), GuiMain::onVtCharRomText)
	EVT_BUTTON(XRCID("VtCharRomButtonCosmicos"), GuiMain::onVtCharRom)

	EVT_CHOICE(XRCID("VTTypeCosmicos"), GuiMain::onVT100)
	EVT_SPIN_UP(XRCID("ZoomSpinCosmicos"), GuiMain::onZoomUp)
	EVT_SPIN_DOWN(XRCID("ZoomSpinCosmicos"), GuiMain::onZoomDown)
	EVT_TEXT(XRCID("ZoomValueCosmicos"), GuiMain::onZoomValue)
	EVT_SPIN_UP(XRCID("ZoomSpinVtCosmicos"), GuiMain::onZoomUpVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtCosmicos"), GuiMain::onZoomDownVt)
	EVT_TEXT(XRCID("ZoomValueVtCosmicos"), GuiMain::onZoomValueVt)
	EVT_BUTTON(XRCID("FullScreenF3Cosmicos"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("ColoursCosmicos"), Main::onColoursDef)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeCosmicos"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeCosmicos"), GuiMain::onVolume)

	EVT_BUTTON(XRCID("ScreenDumpFileButtonCosmicos"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileCosmicos"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileCosmicos"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5Cosmicos"), GuiMain::onScreenDump)

	EVT_BUTTON(XRCID("DP_ButtonCosmicos"), GuiMain::onDp)
	EVT_BUTTON(XRCID("SaveButtonCosmicos"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonCosmicos"), GuiMain::onLoadButton)
	EVT_CHECKBOX(XRCID("StretchDotCosmicos"), GuiMain::onStretchDot)
	EVT_BUTTON(XRCID("KeyMapCosmicos"), Main::onHexKeyDef)
	EVT_CHECKBOX(XRCID("AutoBootCosmicos"), GuiElf::onAutoBoot)
	EVT_BUTTON(XRCID("VtSetupCosmicos"), GuiMain::onVtSetup)

	EVT_CHOICE(XRCID("VideoTypeCosmicos"), GuiCosmicos::onCosmicosVideoType)
	EVT_CHOICE(XRCID("KeyboardCosmicos"), GuiCosmicos::onCosmicosKeyboard)
	EVT_CHECKBOX(XRCID("ForceUCCosmicos"), GuiCosmicos::onCosmicosForceUpperCase)
	EVT_CHECKBOX(XRCID("ControlWindowsCosmicos"), GuiCosmicos::onCosmicosControlWindows)
	EVT_CHECKBOX(XRCID("HexCosmicos"), GuiCosmicos::onCosmicosHex)
	EVT_SPINCTRL(XRCID("RamCosmicos"), GuiCosmicos::onRam)

	EVT_BUTTON(XRCID("CasButtonCosmicos"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasCosmicos"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileCosmicos"), GuiMain::onCassetteText)
	EVT_BUTTON(XRCID("RealCasLoadCosmicos"), GuiMain::onRealCas)
	EVT_BUTTON(XRCID("CasLoadCosmicos"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSaveCosmicos"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopCosmicos"), GuiMain::onCassetteStop)
	EVT_CHECKBOX(XRCID("TurboCosmicos"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockCosmicos"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadCosmicos"), GuiMain::onAutoLoad)

	EVT_TEXT(XRCID("ShowAddressCosmicos"), GuiMain::onLedTimer)

	EVT_TEXT(XRCID("SaveStartCosmicos"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndCosmicos"), GuiMain::onSaveEnd)

	EVT_CHOICE(XRCID("VTBaudTChoiceCosmicos"), GuiCosmicos::onCosmicosBaudT)

END_EVENT_TABLE()

GuiCosmicos::GuiCosmicos(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiMicrotutor2(title, pos, size, mode, dataDir, iniDir)
{
	conf[COSMICOS].saveStartString_ = "";
	conf[COSMICOS].saveEndString_ = "";
    
    position_.x = 0;
}

void GuiCosmicos::readCosmicosConfig()
{
	selectedComputer_ = COSMICOS;

    conf[COSMICOS].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Cosmicos" + pathSeparator_;

    conf[COSMICOS].mainDir_ = readConfigDir("/Dir/Cosmicos/Main", dataDir_ + "Cosmicos" + pathSeparator_);
	conf[COSMICOS].romDir_[MAINROM1] = readConfigDir("/Dir/Cosmicos/Main_Rom_File", dataDir_ + "Cosmicos" + pathSeparator_);
	conf[COSMICOS].vtCharRomDir_ = readConfigDir("/Dir/Cosmicos/Vt_Font_Rom_File", dataDir_ + "Cosmicos" + pathSeparator_);
	conf[COSMICOS].ramDir_ = readConfigDir("/Dir/Cosmicos/Software_File", dataDir_ + "Cosmicos" + pathSeparator_);
	conf[COSMICOS].screenDumpFileDir_ = readConfigDir("/Dir/Cosmicos/Video_Dump_File", dataDir_ + "Cosmicos" + pathSeparator_);
	conf[COSMICOS].wavFileDir_[0] = readConfigDir("/Dir/Cosmicos/Wav_File", dataDir_ + "Cosmicos" + pathSeparator_);
	elfConfiguration[COSMICOS].vtWavFileDir_ = readConfigDir("/Dir/Cosmicos/Vt_Wav_File", dataDir_ + "Cosmicos" + pathSeparator_);

	conf[COSMICOS].rom_[MAINROM1] = configPointer->Read("/Cosmicos/Main_Rom_File", "hex.and.UT4.monitor.bin");
	conf[COSMICOS].screenDumpFile_ = configPointer->Read("/Cosmicos/Video_Dump_File", "screendump.png");
	conf[COSMICOS].wavFile_[0] = configPointer->Read("/Cosmicos/Wav_File", "");
	elfConfiguration[COSMICOS].vtWavFile_ = configPointer->Read("/Cosmicos/Vt_Wav_File", "");
    elfConfiguration[COSMICOS].serialPort_ = configPointer->Read("/Cosmicos/VtSerialPortChoice", "");

	conf[COSMICOS].turboClock_ = configPointer->Read("/Cosmicos/Turbo_Clock_Speed", "15");
	conf[COSMICOS].volume_ = (int)configPointer->Read("/Cosmicos/Volume", 25l);
	conf[COSMICOS].videoMode_ = (int)configPointer->Read("/Cosmicos/Video_Type", 0l);
	conf[COSMICOS].ramType_ = (int)configPointer->Read("/Cosmicos/Ram_Type", 48l);

	configPointer->Read("/Cosmicos/Enable_Auto_Cassette", &conf[COSMICOS].autoCassetteLoad_, true);
	configPointer->Read("/Cosmicos/Enable_Vt_Stretch_Dot", &conf[COSMICOS].stretchDot_, false);
    configPointer->Read("/Cosmicos/Enable_Vt_External", &elfConfiguration[COSMICOS].vtExternal, false);
	configPointer->Read("/Cosmicos/Enable_Turbo_Cassette", &conf[COSMICOS].turbo_, true);
	configPointer->Read("/Cosmicos/Enable_Real_Cassette", &conf[COSMICOS].realCassetteLoad_, false);

	getConfigBool("/Cosmicos/SerialLog", false);
    configPointer->Read("/Cosmicos/VtEf", &elfConfiguration[COSMICOS].vtEf, false);
	configPointer->Read("/Cosmicos/VtQ", &elfConfiguration[COSMICOS].vtQ, true);
	elfConfiguration[COSMICOS].bellFrequency_ = (int)configPointer->Read("/Cosmicos/Bell_Frequency", 800);

    elfConfiguration[COSMICOS].useUart = false;
	elfConfiguration[COSMICOS].vtType = (int)configPointer->Read("/Cosmicos/VT_Type", 0l);
    elfConfiguration[COSMICOS].vt52SetUpFeature_ = configPointer->Read("/Cosmicos/VT52Setup", 0x4092l);
    elfConfiguration[COSMICOS].vt100SetUpFeature_ = configPointer->Read("/Cosmicos/VT100Setup", 0xcad2l);
    elfConfiguration[COSMICOS].vtExternalSetUpFeature_ = configPointer->Read("/Cosmicos/VTExternalSetup", 0xcad2l);

    elfConfiguration[COSMICOS].keyboardType = (int)configPointer->Read("/Cosmicos/Keyboard_Type", 0l);
	elfConfiguration[COSMICOS].baudT = (int)configPointer->Read("/Cosmicos/Vt_Baud", 10l);
	elfConfiguration[COSMICOS].baudR = elfConfiguration[COSMICOS].baudT;

	configPointer->Read("/Cosmicos/Force_Uppercase", &elfConfiguration[COSMICOS].forceUpperCase, true);
	configPointer->Read("/Cosmicos/Enable_Auto_Boot", &elfConfiguration[COSMICOS].autoBoot, true);
	configPointer->Read("/Cosmicos/Open_Control_Windows", &elfConfiguration[COSMICOS].useElfControlWindows, true);
	configPointer->Read("/Cosmicos/Use_Hex_Panel", &elfConfiguration[COSMICOS].useHex, true);

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[COSMICOS].zoom_ = configPointer->Read("/Cosmicos/Zoom", defaultZoom);
	defaultZoom.Printf("%2.2f", 1.0);
	conf[COSMICOS].zoomVt_ = configPointer->Read("/Cosmicos/Vt_Zoom", defaultZoom);
	wxString defaultScale;
	defaultScale.Printf("%i", 4);
	conf[COSMICOS].xScale_ = configPointer->Read("/Cosmicos/Window_Scale_Factor_X", defaultScale);

	wxString defaultClock;
	defaultClock.Printf("%1.2f", 1.75);
	conf[COSMICOS].clock_ = configPointer->Read("/Cosmicos/Clock_Speed", defaultClock);

	wxString defaultTimer;
	defaultTimer.Printf("%d", 100);
	conf[COSMICOS].ledTime_ = configPointer->Read("/Cosmicos/Led_Update_Frequency", defaultTimer);

	setVtType("Cosmicos", COSMICOS, elfConfiguration[COSMICOS].vtType, false);
	setCosmicosVideoType(conf[COSMICOS].videoMode_);
	setRealCas(COSMICOS);

	conf[COSMICOS].vtCharRom_ = configPointer->Read("/Cosmicos/Vt_Font_Rom_File", "vt52.a.bin");

	if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomCosmicos", wxComboBox)->SetValue(conf[COSMICOS].rom_[MAINROM1]);
		XRCCTRL(*this, "VtCharRomCosmicos", wxComboBox)->SetValue(conf[COSMICOS].vtCharRom_);
		XRCCTRL(*this, "ScreenDumpFileCosmicos", wxComboBox)->SetValue(conf[COSMICOS].screenDumpFile_);
		XRCCTRL(*this, "WavFileCosmicos", wxTextCtrl)->SetValue(conf[COSMICOS].wavFile_[0]);

		XRCCTRL(*this, "VTTypeCosmicos", wxChoice)->SetSelection(elfConfiguration[COSMICOS].vtType);

		XRCCTRL(*this, "VTBaudTChoiceCosmicos", wxChoice)->SetSelection(elfConfiguration[COSMICOS].baudT);
		XRCCTRL(*this, "VTBaudRChoiceCosmicos", wxChoice)->SetSelection(elfConfiguration[COSMICOS].baudR);
//		XRCCTRL(*this, "VTBaudTTextCosmicos", wxStaticText)->Enable(elfConfiguration[COSMICOS].vtType != VTNONE);
        XRCCTRL(*this, "AddressText1Cosmicos",wxStaticText)->Enable(elfConfiguration[COSMICOS].useElfControlWindows);
        XRCCTRL(*this, "AddressText2Cosmicos",wxStaticText)->Enable(elfConfiguration[COSMICOS].useElfControlWindows);
//        XRCCTRL(*this, "VTBaudTChoiceCosmicos", wxChoice)->Enable(elfConfiguration[COSMICOS].vtType != VTNONE);

		XRCCTRL(*this, "ForceUCCosmicos", wxCheckBox)->SetValue(elfConfiguration[COSMICOS].forceUpperCase);
//		XRCCTRL(*this, "VtCharRomButtonCosmicos", wxButton)->Enable(elfConfiguration[COSMICOS].vtType != VTNONE);
//		XRCCTRL(*this, "VtCharRomCosmicos", wxComboBox)->Enable(elfConfiguration[COSMICOS].vtType != VTNONE);
//		XRCCTRL(*this, "VtSetupCosmicos", wxButton)->Enable(elfConfiguration[COSMICOS].vtType != VTNONE);
		XRCCTRL(*this, "VideoTypeCosmicos", wxChoice)->SetSelection(conf[COSMICOS].videoMode_);
		XRCCTRL(*this, "AutoBootCosmicos", wxCheckBox)->SetValue(elfConfiguration[COSMICOS].autoBoot);
		XRCCTRL(*this, "KeyboardCosmicos", wxChoice)->SetSelection(elfConfiguration[COSMICOS].keyboardType);
		XRCCTRL(*this, "ZoomValueCosmicos", wxTextCtrl)->ChangeValue(conf[COSMICOS].zoom_);
		XRCCTRL(*this, "ZoomValueVtCosmicos", wxTextCtrl)->ChangeValue(conf[COSMICOS].zoomVt_);
		XRCCTRL(*this, "ControlWindowsCosmicos", wxCheckBox)->SetValue(elfConfiguration[COSMICOS].useElfControlWindows);
		XRCCTRL(*this, "ShowAddressCosmicos",wxTextCtrl)->Enable(elfConfiguration[COSMICOS].useElfControlWindows);
		XRCCTRL(*this, "StretchDotCosmicos", wxCheckBox)->SetValue(conf[COSMICOS].stretchDot_);
		XRCCTRL(*this, "RamCosmicos", wxSpinCtrl)->SetValue(conf[COSMICOS].ramType_);
		XRCCTRL(*this, "HexCosmicos", wxCheckBox)->SetValue(elfConfiguration[COSMICOS].useHex);
		XRCCTRL(*this, "TurboCosmicos", wxCheckBox)->SetValue(conf[COSMICOS].turbo_);
		turboGui("Cosmicos");
		XRCCTRL(*this, "TurboClockCosmicos", wxTextCtrl)->SetValue(conf[COSMICOS].turboClock_);
		XRCCTRL(*this, "AutoCasLoadCosmicos", wxCheckBox)->SetValue(conf[COSMICOS].autoCassetteLoad_);
		XRCCTRL(*this, "VolumeCosmicos", wxSlider)->SetValue(conf[COSMICOS].volume_);
		clockTextCtrl[COSMICOS]->ChangeValue(conf[COSMICOS].clock_);
		XRCCTRL(*this, "ShowAddressCosmicos", wxTextCtrl)->ChangeValue(conf[COSMICOS].ledTime_);
	}

	setCosmicosKeyboard(elfConfiguration[COSMICOS].keyboardType);

	elfConfiguration[COSMICOS].usePortExtender = false;
	elfConfiguration[COSMICOS].ideEnabled = false;
    elfConfiguration[COSMICOS].fdcEnabled = false;
    elfConfiguration[COSMICOS].useLedModule = false;
	elfConfiguration[COSMICOS].useTape = true;
}

void GuiCosmicos::writeCosmicosDirConfig()
{
    writeConfigDir("/Dir/Cosmicos/Main", conf[COSMICOS].mainDir_);
	writeConfigDir("/Dir/Cosmicos/Main_Rom_File", conf[COSMICOS].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Cosmicos/Vt_Font_Rom_File", conf[COSMICOS].vtCharRomDir_);
	writeConfigDir("/Dir/Cosmicos/Software_File", conf[COSMICOS].ramDir_);
	writeConfigDir("/Dir/Cosmicos/Video_Dump_File", conf[COSMICOS].screenDumpFileDir_);
	writeConfigDir("/Dir/Cosmicos/Wav_File", conf[COSMICOS].wavFileDir_[0]);
	writeConfigDir("/Dir/Cosmicos/Vt_Wav_File", elfConfiguration[COSMICOS].vtWavFileDir_);
}

void GuiCosmicos::writeCosmicosConfig()
{
    configPointer->Write("/Cosmicos/Main_Rom_File", conf[COSMICOS].rom_[MAINROM1]);
	configPointer->Write("/Cosmicos/Vt_Font_Rom_File", conf[COSMICOS].vtCharRom_);
	configPointer->Write("/Cosmicos/Video_Dump_File", conf[COSMICOS].screenDumpFile_);
	configPointer->Write("/Cosmicos/Wav_File", conf[COSMICOS].wavFile_[0]);
	configPointer->Write("/Cosmicos/Vt_Wav_File", elfConfiguration[COSMICOS].vtWavFile_);
    configPointer->Write("/Cosmicos/VtSerialPortChoice", elfConfiguration[COSMICOS].serialPort_);

    configPointer->Write("/Cosmicos/VtEf", elfConfiguration[COSMICOS].vtEf);
	configPointer->Write("/Cosmicos/VtQ", elfConfiguration[COSMICOS].vtQ);
	configPointer->Write("/Cosmicos/VT_Type", elfConfiguration[COSMICOS].vtType);
	configPointer->Write("/Cosmicos/Bell_Frequency", elfConfiguration[COSMICOS].bellFrequency_);

    long value = elfConfiguration[COSMICOS].vt52SetUpFeature_.to_ulong();
    configPointer->Write("/Cosmicos/VT52Setup", value);
    value = elfConfiguration[COSMICOS].vt100SetUpFeature_.to_ulong();
    configPointer->Write("/Cosmicos/VT100Setup", value);
    value = elfConfiguration[COSMICOS].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("/Cosmicos/VTExternalSetup", value);
    
	configPointer->Write("/Cosmicos/Vt_Baud", elfConfiguration[COSMICOS].baudR);
	configPointer->Write("/Cosmicos/Enable_Auto_Boot", elfConfiguration[COSMICOS].autoBoot);
	configPointer->Write("/Cosmicos/Video_Type", conf[COSMICOS].videoMode_);
	configPointer->Write("/Cosmicos/Keyboard_Type", elfConfiguration[COSMICOS].keyboardType);
	configPointer->Write("/Cosmicos/Zoom", conf[COSMICOS].zoom_);
	configPointer->Write("/Cosmicos/Vt_Zoom", conf[COSMICOS].zoomVt_);
	configPointer->Write("/Cosmicos/Force_Uppercase", elfConfiguration[COSMICOS].forceUpperCase);
	configPointer->Write("/Cosmicos/Use_Hex_Panel", elfConfiguration[COSMICOS].useHex);
	configPointer->Write("/Cosmicos/Open_Control_Windows", elfConfiguration[COSMICOS].useElfControlWindows);
	configPointer->Write("/Cosmicos/Enable_Vt_Stretch_Dot", conf[COSMICOS].stretchDot_);
    configPointer->Write("/Cosmicos/Enable_Vt_External", elfConfiguration[COSMICOS].vtExternal);
	configPointer->Write("/Cosmicos/Ram_Type", conf[COSMICOS].ramType_);
	configPointer->Write("/Cosmicos/Enable_Turbo_Cassette", conf[COSMICOS].turbo_);
	configPointer->Write("/Cosmicos/Turbo_Clock_Speed", conf[COSMICOS].turboClock_);
	configPointer->Write("/Cosmicos/Enable_Auto_Cassette", conf[COSMICOS].autoCassetteLoad_);
	configPointer->Write("/Cosmicos/Enable_Real_Cassette", conf[COSMICOS].realCassetteLoad_);
	configPointer->Write("/Cosmicos/Volume", conf[COSMICOS].volume_);
	configPointer->Write("/Cosmicos/Led_Update_Frequency", conf[COSMICOS].ledTime_);

    configPointer->Write("/Cosmicos/Clock_Speed", conf[COSMICOS].clock_);
}

void GuiCosmicos::readCosmicosWindowConfig()
{
    conf[COSMICOS].pixieX_ = (int)configPointer->Read("/Cosmicos/Window_Position_Pixie_X", mainWindowX_+windowInfo.mainwX);
    conf[COSMICOS].pixieY_ = (int)configPointer->Read("/Cosmicos/Window_Position_Pixie_Y", mainWindowY_);
    conf[COSMICOS].vtX_ = (int)configPointer->Read("/Cosmicos/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX);
    conf[COSMICOS].vtY_ = (int)configPointer->Read("/Cosmicos/Window_Position_Vt_Y", mainWindowY_);
    conf[COSMICOS].mainX_ = (int)configPointer->Read("/Cosmicos/Window_Position_X", mainWindowX_);
    conf[COSMICOS].mainY_ = (int)configPointer->Read("/Cosmicos/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    conf[COSMICOS].keypadX_ = (int)configPointer->Read("/Cosmicos/Window_Position_Hex_Panel_X", mainWindowX_+333+windowInfo.xBorder2);
    conf[COSMICOS].keypadY_ = (int)configPointer->Read("/Cosmicos/Window_Position_Hex_Panel_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiCosmicos::writeCosmicosWindowConfig()
{
    if (conf[COSMICOS].pixieX_ > 0)
        configPointer->Write("/Cosmicos/Window_Position_Pixie_X", conf[COSMICOS].pixieX_);
    if (conf[COSMICOS].pixieY_ > 0)
        configPointer->Write("/Cosmicos/Window_Position_Pixie_Y", conf[COSMICOS].pixieY_);
    if (conf[COSMICOS].vtX_ > 0)
        configPointer->Write("/Cosmicos/Window_Position_Vt_X", conf[COSMICOS].vtX_);
    if (conf[COSMICOS].vtY_ > 0)
        configPointer->Write("/Cosmicos/Window_Position_Vt_Y", conf[COSMICOS].vtY_);
    if (conf[COSMICOS].keypadX_ > 0)
        configPointer->Write("/Cosmicos/Window_Position_Hex_Panel_X", conf[COSMICOS].keypadX_);
    if (conf[COSMICOS].keypadY_ > 0)
        configPointer->Write("/Cosmicos/Window_Position_Hex_Panel_Y", conf[COSMICOS].keypadY_);
    if (conf[COSMICOS].mainX_ > 0)
        configPointer->Write("/Cosmicos/Window_Position_X", conf[COSMICOS].mainX_);
    if (conf[COSMICOS].mainY_ > 0)
        configPointer->Write("/Cosmicos/Window_Position_Y", conf[COSMICOS].mainY_);
}

void GuiCosmicos::onCosmicosVideoType(wxCommandEvent&event)
{
	setCosmicosVideoType(event.GetSelection());
}

void GuiCosmicos::onCosmicosKeyboard(wxCommandEvent&event)
{
	setCosmicosKeyboard(event.GetSelection());
}

void GuiCosmicos::onCosmicosBaudT(wxCommandEvent&event)
{
	elfConfiguration[COSMICOS].baudT = event.GetSelection();
	elfConfiguration[COSMICOS].baudR = event.GetSelection();
	XRCCTRL(*this, "VTBaudRChoiceCosmicos", wxChoice)->SetSelection(elfConfiguration[COSMICOS].baudR);
}

void GuiCosmicos::onCosmicosForceUpperCase(wxCommandEvent&event)
{
	elfConfiguration[COSMICOS].forceUpperCase = event.IsChecked();
	if (runningComputer_ == COSMICOS)
	{
		p_Cosmicos->setForceUpperCase(event.IsChecked());
	}
}

void GuiCosmicos::onCosmicosControlWindows(wxCommandEvent&event)
{
	elfConfiguration[COSMICOS].useElfControlWindows = event.IsChecked();
	if (mode_.gui)
	{
		XRCCTRL(*this,"ShowAddressCosmicos",wxTextCtrl)->Enable(elfConfiguration[COSMICOS].useElfControlWindows);
        XRCCTRL(*this,"AddressText1Cosmicos",wxStaticText)->Enable(elfConfiguration[COSMICOS].useElfControlWindows);
		XRCCTRL(*this,"AddressText2Cosmicos",wxStaticText)->Enable(elfConfiguration[COSMICOS].useElfControlWindows);
    }
	if (runningComputer_ == COSMICOS)
		p_Cosmicos->Show(elfConfiguration[COSMICOS].useElfControlWindows);
}

void GuiCosmicos::setCosmicosKeyboard(int Selection)
{
	elfConfiguration[COSMICOS].keyboardType = Selection;

	switch(Selection)
	{
		case KEYBOARDNONE:
			elfConfiguration[COSMICOS].useHexKeyboardEf3 = false;
			elfConfiguration[COSMICOS].useKeyboard = false;
			elfConfiguration[COSMICOS].UsePS2 = false;
			elfConfiguration[COSMICOS].usePs2gpio = false;
		break;
		case KEYBOARD_HEXCOSMICOS:
			elfConfiguration[COSMICOS].useHexKeyboardEf3 = true;
			elfConfiguration[COSMICOS].useKeyboard = false;
			elfConfiguration[COSMICOS].UsePS2 = false;
			elfConfiguration[COSMICOS].usePs2gpio = false;
		break;
	}
}

void GuiCosmicos::setCosmicosVideoType(int Selection)
{
	conf[COSMICOS].videoMode_ = Selection;

	switch(Selection)
	{
		case VIDEONONE:
			elfConfiguration[COSMICOS].usePixie = false;
			elfConfiguration[COSMICOS].use6845 = false;
			elfConfiguration[COSMICOS].useS100 = false;
			elfConfiguration[COSMICOS].use6847 = false;
			elfConfiguration[COSMICOS].useTMS9918 = false;
			elfConfiguration[COSMICOS].use8275 = false;
			if (mode_.gui)
			{
//				XRCCTRL(*this, "VTTypeCosmicos", wxChoice)->Enable(true);
//                XRCCTRL(*this, "VTBaudTTextCosmicos", wxStaticText)->Enable(elfConfiguration[COSMICOS].vtType != VTNONE);
                XRCCTRL(*this, "ZoomTextCosmicos", wxStaticText)->Enable(false);
//				XRCCTRL(*this, "VTBaudRTextCosmicos", wxStaticText)->Enable((elfConfiguration[COSMICOS].vtType != VTNONE) && elfConfiguration[COSMICOS].useUart);
//				XRCCTRL(*this, "VTBaudRChoiceCosmicos", wxChoice)->Enable((elfConfiguration[COSMICOS].vtType != VTNONE) && elfConfiguration[COSMICOS].useUart);
//				XRCCTRL(*this, "VTBaudTChoiceCosmicos", wxChoice)->Enable(elfConfiguration[COSMICOS].vtType != VTNONE);
//				XRCCTRL(*this, "VtSetupCosmicos", wxButton)->Enable(elfConfiguration[COSMICOS].vtType != VTNONE);
				XRCCTRL(*this, "ZoomValueCosmicos", wxTextCtrl)->Enable(false);
				XRCCTRL(*this, "ZoomSpinCosmicos", wxSpinButton)->Enable(false);
			}
		break;

		case VIDEOPIXIE:
			elfConfiguration[COSMICOS].usePixie = true;
			elfConfiguration[COSMICOS].use6845 = false;
			elfConfiguration[COSMICOS].useS100 = false;
			elfConfiguration[COSMICOS].use6847 = false;
			elfConfiguration[COSMICOS].useTMS9918 = false;
			elfConfiguration[COSMICOS].use8275 = false;
			if (mode_.gui)
			{
//				XRCCTRL(*this, "VTTypeCosmicos", wxChoice)->Enable(true);
                XRCCTRL(*this, "ZoomTextCosmicos", wxStaticText)->Enable(true);
//                XRCCTRL(*this, "VTBaudTTextCosmicos", wxStaticText)->Enable(elfConfiguration[COSMICOS].vtType != VTNONE);
//                XRCCTRL(*this, "VTBaudRTextCosmicos", wxStaticText)->Enable((elfConfiguration[COSMICOS].vtType != VTNONE) && elfConfiguration[COSMICOS].useUart);
//				XRCCTRL(*this, "VTBaudRChoiceCosmicos", wxChoice)->Enable((elfConfiguration[COSMICOS].vtType != VTNONE) && elfConfiguration[COSMICOS].useUart);
//				XRCCTRL(*this, "VTBaudTChoiceCosmicos", wxChoice)->Enable(elfConfiguration[COSMICOS].vtType != VTNONE);
//				XRCCTRL(*this, "VtSetupCosmicos", wxButton)->Enable(elfConfiguration[COSMICOS].vtType != VTNONE);
				XRCCTRL(*this, "ZoomValueCosmicos", wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "ZoomSpinCosmicos", wxSpinButton)->Enable(true);
			}
		break;
	}
}

void GuiCosmicos::onCosmicosHex(wxCommandEvent&event)
{
	elfConfiguration[COSMICOS].useHex = event.IsChecked();
	if (runningComputer_ == COSMICOS)
		p_Cosmicos->showModules(elfConfiguration[COSMICOS].useHex);
}

void GuiCosmicos::onRam(wxSpinEvent&event)
{
	conf[COSMICOS].ramType_ = event.GetPosition();

	switch (conf[COSMICOS].ramType_&0x3)
	{
		case 0:
			return;
		break;

		case 1:
			conf[COSMICOS].ramType_ += 3;
		break;

		case 2:
			conf[COSMICOS].ramType_ += 2;
		break;

		case 3:
			conf[COSMICOS].ramType_ -= 3;
		break;
	}

	if (conf[COSMICOS].ramType_ < 0)
		conf[COSMICOS].ramType_ = 0;
	if (conf[COSMICOS].ramType_ > 48)
		conf[COSMICOS].ramType_ = 48;

	XRCCTRL(*this, "RamCosmicos", wxSpinCtrl)->SetValue(conf[COSMICOS].ramType_);
}

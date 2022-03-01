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
#include "guitmc2000.h"
#include "pixie.h"

#include "psave.h"

BEGIN_EVENT_TABLE(GuiTMC2000, GuiEti)
	EVT_TEXT(XRCID("MainRomTMC2000"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomTMC2000"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonTMC2000"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("RamSWTMC2000"), GuiMain::onRamSWText)
	EVT_COMBOBOX(XRCID("RamSWTMC2000"), GuiMain::onRamSWText)
	EVT_BUTTON(XRCID("RamSWButtonTMC2000"), GuiMain::onRamSW)

	EVT_TEXT(XRCID("Chip8SWTMC2000"), GuiMain::onChip8SWText)
	EVT_BUTTON(XRCID("Chip8SWButtonTMC2000"), GuiMain::onChip8SW)
	EVT_BUTTON(XRCID("EjectChip8SWTMC2000"), GuiMain::onEjectChip8SW)

	EVT_SPIN_UP(XRCID("ZoomSpinTMC2000"), GuiMain::onZoom)
	EVT_SPIN_DOWN(XRCID("ZoomSpinTMC2000"), GuiMain::onZoom)
	EVT_TEXT(XRCID("ZoomValueTMC2000"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3TMC2000"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("CasButtonTMC2000"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasTMC2000"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileTMC2000"), GuiMain::onCassetteText)

	EVT_BUTTON(XRCID("ScreenDumpFileButtonTMC2000"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileTMC2000"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileTMC2000"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5TMC2000"), GuiMain::onScreenDump)

	EVT_BUTTON(XRCID("RealCasLoadTMC2000"), GuiMain::onRealCas)
	EVT_BUTTON(XRCID("CasLoadTMC2000"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSaveTMC2000"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopTMC2000"), GuiMain::onCassetteStop)
	EVT_CHECKBOX(XRCID("TurboTMC2000"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockTMC2000"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadTMC2000"), GuiMain::onAutoLoad)
	EVT_BUTTON(XRCID("SaveButtonTMC2000"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonTMC2000"), GuiMain::onLoadButton)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeTMC2000"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeTMC2000"), GuiMain::onVolume)
	EVT_BUTTON(XRCID("KeyMapTMC2000"), Main::onHexKeyDef)
	EVT_CHECKBOX(XRCID("KeyboardTMC2000"), GuiMain::onKeyboard)
	EVT_BUTTON(XRCID("ColoursTMC2000"), Main::onColoursDef)
	EVT_CHOICE(XRCID("RamTMC2000"), GuiMain::onChoiceRam)

	EVT_TEXT(XRCID("SaveStartTMC2000"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndTMC2000"), GuiMain::onSaveEnd)

	EVT_TEXT(XRCID("MainRomTMC1800"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomTMC1800"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonTMC1800"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("RamSWTMC1800"), GuiMain::onRamSWText)
	EVT_COMBOBOX(XRCID("RamSWTMC1800"), GuiMain::onRamSWText)
	EVT_BUTTON(XRCID("RamSWButtonTMC1800"), GuiMain::onRamSW)

	EVT_TEXT(XRCID("Chip8SWTMC1800"), GuiMain::onChip8SWText)
	EVT_BUTTON(XRCID("Chip8SWButtonTMC1800"), GuiMain::onChip8SW)
	EVT_BUTTON(XRCID("EjectChip8SWTMC1800"), GuiMain::onEjectChip8SW)

	EVT_SPIN_UP(XRCID("ZoomSpinTMC1800"), GuiMain::onZoom)
	EVT_SPIN_DOWN(XRCID("ZoomSpinTMC1800"), GuiMain::onZoom)
	EVT_TEXT(XRCID("ZoomValueTMC1800"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3TMC1800"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("CasButtonTMC1800"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasTMC1800"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileTMC1800"), GuiMain::onCassetteText)

	EVT_BUTTON(XRCID("ScreenDumpFileButtonTMC1800"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileTMC1800"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileTMC1800"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5TMC1800"), GuiMain::onScreenDump)

	EVT_BUTTON(XRCID("RealCasLoadTMC1800"), GuiMain::onRealCas)
	EVT_BUTTON(XRCID("CasLoadTMC1800"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSaveTMC1800"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopTMC1800"), GuiMain::onCassetteStop)
	EVT_CHECKBOX(XRCID("TurboTMC1800"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockTMC1800"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadTMC1800"), GuiMain::onAutoLoad)
	EVT_BUTTON(XRCID("SaveButtonTMC1800"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonTMC1800"), GuiMain::onLoadButton)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeTMC1800"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeTMC1800"), GuiMain::onVolume)
	EVT_BUTTON(XRCID("KeyMapTMC1800"), Main::onHexKeyDef)
	EVT_CHECKBOX(XRCID("KeyboardTMC1800"), GuiMain::onKeyboard)
	EVT_BUTTON(XRCID("ColoursTMC1800"), Main::onColoursDef)
	EVT_TEXT(XRCID("BeepFrequencyTMC1800"), GuiMain::onBeepFrequency)
	EVT_CHOICE(XRCID("RamTMC1800"), GuiMain::onChoiceRam)

	EVT_TEXT(XRCID("SaveStartTMC1800"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndTMC1800"), GuiMain::onSaveEnd)

END_EVENT_TABLE()

GuiTMC2000::GuiTMC2000(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiEti(title, pos, size, mode, dataDir, iniDir)
{
	conf[TMC2000].saveStartString_ = "";
	conf[TMC2000].saveEndString_ = "";
	conf[TMC1800].saveStartString_ = "";
	conf[TMC1800].saveEndString_ = "";
}

void GuiTMC2000::readTMC2000Config()
{
	selectedComputer_ = TMC2000;

    conf[TMC2000].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "TMC2000" + pathSeparator_;
    conf[TMC2000].mainDir_ = readConfigDir("/Dir/TMC2000/Main", dataDir_ + "TMC2000" + pathSeparator_);
    
    conf[TMC2000].romDir_[MAINROM1] = readConfigDir("/Dir/TMC2000/Main_Rom_File", dataDir_ + "TMC2000"  + pathSeparator_);
	conf[TMC2000].ramDir_ = readConfigDir("/Dir/TMC2000/Ram_Software", dataDir_ + "TMC2000"  + pathSeparator_);
	conf[TMC2000].chip8SWDir_ = readConfigDir("/Dir/TMC2000/Chip_8_Software", dataDir_ + "Chip-8"  + pathSeparator_ + "Chip-8 Games"  + pathSeparator_);
	conf[TMC2000].screenDumpFileDir_ = readConfigDir("/Dir/TMC2000/Video_Dump_File", dataDir_ + "TMC2000" + pathSeparator_);
	conf[TMC2000].wavFileDir_[0] = readConfigDir("/Dir/TMC2000/Wav_File", dataDir_ + "TMC2000" + pathSeparator_);

	conf[TMC2000].rom_[MAINROM1] = configPointer->Read("/TMC2000/Main_Rom_File", "telmac2000.rom");
	conf[TMC2000].ram_ = configPointer->Read("/TMC2000/Ram_Software", "chip8.ram");
	conf[TMC2000].chip8SW_ = configPointer->Read("/TMC2000/Chip_8_Software", "Pong (1 player).ch8");
	conf[TMC2000].screenDumpFile_ = configPointer->Read("/TMC2000/Video_Dump_File", "screendump.png");
	conf[TMC2000].wavFile_[0] = configPointer->Read("/TMC2000/Wav_File", "");

	conf[TMC2000].turboClock_ = configPointer->Read("/TMC2000/Turbo_Clock_Speed", "15");
	conf[TMC2000].volume_ = (int)configPointer->Read("/TMC2000/Volume", 25l);
	conf[TMC2000].ramType_ = (int)configPointer->Read("/TMC2000/Ram_Type", 02);

	configPointer->Read("/TMC2000/Use_Keyboard", &conf[TMC2000].useKeyboard_, false);
	configPointer->Read("/TMC2000/Enable_Turbo_Cassette", &conf[TMC2000].turbo_, true);
	configPointer->Read("/TMC2000/Enable_Auto_Cassette", &conf[TMC2000].autoCassetteLoad_, true);
	configPointer->Read("/TMC2000/Enable_Real_Cassette", &conf[TMC2000].realCassetteLoad_, false);

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[TMC2000].zoom_ = convertLocale(configPointer->Read("/TMC2000/Zoom", defaultZoom));
	wxString defaultScale;
	defaultScale.Printf("%i", 4);
	conf[TMC2000].xScale_ = convertLocale(configPointer->Read("/TMC2000/Window_Scale_Factor_X", defaultScale));
	wxString defaultClock;
	defaultClock.Printf("%1.2f", 1.75);
	conf[TMC2000].clock_ = convertLocale(configPointer->Read("/TMC2000/Clock_Speed", defaultClock));

	setRealCas(TMC2000);

    long value;
    conf[TMC2000].saveStartString_ = configPointer->Read("/TMC2000/SaveStart", "0");
    if (!conf[TMC2000].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[TMC2000].saveStart_ = value;

	if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomTMC2000", wxComboBox)->SetValue(conf[TMC2000].rom_[MAINROM1]);
		XRCCTRL(*this, "RamSWTMC2000", wxComboBox)->SetValue(conf[TMC2000].ram_);
		XRCCTRL(*this, "Chip8SWTMC2000", wxTextCtrl)->SetValue(conf[TMC2000].chip8SW_);
		XRCCTRL(*this, "ScreenDumpFileTMC2000", wxComboBox)->SetValue(conf[TMC2000].screenDumpFile_);
		XRCCTRL(*this, "WavFileTMC2000", wxTextCtrl)->SetValue(conf[TMC2000].wavFile_[0]);

        correctZoomAndValue(TMC2000, "TMC2000", SET_SPIN);

		XRCCTRL(*this, "TurboTMC2000", wxCheckBox)->SetValue(conf[TMC2000].turbo_);
		turboGui("TMC2000");
		XRCCTRL(*this, "TurboClockTMC2000", wxTextCtrl)->SetValue(conf[TMC2000].turboClock_);
		XRCCTRL(*this, "AutoCasLoadTMC2000", wxCheckBox)->SetValue(conf[TMC2000].autoCassetteLoad_);

		XRCCTRL(*this, "VolumeTMC2000", wxSlider)->SetValue(conf[TMC2000].volume_);
		XRCCTRL(*this, "RamTMC2000", wxChoice)->SetSelection(conf[TMC2000].ramType_);
        if (clockTextCtrl[TMC2000] != NULL)
            clockTextCtrl[TMC2000]->ChangeValue(conf[TMC2000].clock_);
		XRCCTRL(*this, "KeyboardTMC2000", wxCheckBox)->SetValue(conf[TMC2000].useKeyboard_);
		XRCCTRL(*this, "KeyMapTMC2000", wxButton)->Enable(!conf[TMC2000].useKeyboard_);
        if (conf[TMC2000].saveStart_ != 0)
            XRCCTRL(*this, "SaveStartTMC2000", wxTextCtrl)->SetValue(conf[TMC2000].saveStartString_);
	}
}

void GuiTMC2000::writeTMC2000DirConfig()
{
    writeConfigDir("/Dir/TMC2000/Main", conf[TMC2000].mainDir_);
    writeConfigDir("/Dir/TMC2000/Main_Rom_File", conf[TMC2000].romDir_[MAINROM1]);
	writeConfigDir("/Dir/TMC2000/Ram_Software", conf[TMC2000].ramDir_);
	writeConfigDir("/Dir/TMC2000/Chip_8_Software", conf[TMC2000].chip8SWDir_);
	writeConfigDir("/Dir/TMC2000/Video_Dump_File", conf[TMC2000].screenDumpFileDir_);
	writeConfigDir("/Dir/TMC2000/Wav_File", conf[TMC2000].wavFileDir_[0]);
}

void GuiTMC2000::writeTMC2000Config()
{
    configPointer->Write("/TMC2000/Main_Rom_File", conf[TMC2000].rom_[MAINROM1]);
	configPointer->Write("/TMC2000/Ram_Software", conf[TMC2000].ram_);
	configPointer->Write("/TMC2000/Chip_8_Software", conf[TMC2000].chip8SW_);
	configPointer->Write("/TMC2000/Video_Dump_File", conf[TMC2000].screenDumpFile_);
	configPointer->Write("/TMC2000/Wav_File", conf[TMC2000].wavFile_[0]);

	configPointer->Write("/TMC2000/Zoom", conf[TMC2000].zoom_);

	configPointer->Write("/TMC2000/Enable_Turbo_Cassette", conf[TMC2000].turbo_);
	configPointer->Write("/TMC2000/Turbo_Clock_Speed", conf[TMC2000].turboClock_);
	configPointer->Write("/TMC2000/Enable_Auto_Cassette", conf[TMC2000].autoCassetteLoad_);
	configPointer->Write("/TMC2000/Enable_Real_Cassette", conf[TMC2000].realCassetteLoad_);
	configPointer->Write("/TMC2000/Volume", conf[TMC2000].volume_);
	configPointer->Write("/TMC2000/Ram_Type", conf[TMC2000].ramType_);
    configPointer->Write("/TMC2000/SaveStart", conf[TMC2000].saveStartString_);

	configPointer->Write("/TMC2000/Clock_Speed", conf[TMC2000].clock_);

	configPointer->Write("/TMC2000/Use_Keyboard", conf[TMC2000].useKeyboard_);
}

void GuiTMC2000::readTMC2000WindowConfig()
{
    conf[TMC2000].mainX_ = (int)configPointer->Read("/TMC2000/Window_Position_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[TMC2000].mainY_ = (int)configPointer->Read("/TMC2000/Window_Position_Y", mainWindowY_);
}

void GuiTMC2000::writeTMC2000WindowConfig()
{
    if (conf[TMC2000].mainX_ > 0)
        configPointer->Write("/TMC2000/Window_Position_X", conf[TMC2000].mainX_);
    if (conf[TMC2000].mainY_ > 0)
        configPointer->Write("/TMC2000/Window_Position_Y", conf[TMC2000].mainY_);
}

void GuiTMC2000::readTMC1800Config()
{
	selectedComputer_ = TMC1800;

    conf[TMC1800].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "TMC1800" + pathSeparator_;
    conf[TMC1800].mainDir_ = readConfigDir("/Dir/TMC1800/Main", dataDir_ + "TMC1800" + pathSeparator_);
    
    conf[TMC1800].romDir_[MAINROM1] = readConfigDir("/Dir/TMC1800/Main_Rom_File", dataDir_ + "TMC1800"  + pathSeparator_);
	conf[TMC1800].ramDir_ = readConfigDir("/Dir/TMC1800/Ram_Software", dataDir_ + "TMC1800"  + pathSeparator_);
	conf[TMC1800].chip8SWDir_ = readConfigDir("/Dir/TMC1800/Chip_8_Software", dataDir_ + "Chip-8"  + pathSeparator_ + "Chip-8 Games"  + pathSeparator_);
	conf[TMC1800].screenDumpFileDir_ = configPointer->Read("/TMC1800/ScreenDumpFileDir", dataDir_ + "TMC1800" + pathSeparator_);
	conf[TMC1800].wavFileDir_[0] = readConfigDir("/Dir/TMC1800/Wav_File", dataDir_ + "TMC1800" + pathSeparator_);

	conf[TMC1800].rom_[MAINROM1] = configPointer->Read("/TMC1800/Main_Rom_File", "telmac1800.rom");
	conf[TMC1800].ram_ = configPointer->Read("/TMC1800/Ram_Software", "chip8.ram");
	conf[TMC1800].chip8SW_ = configPointer->Read("/TMC1800/Chip_8_Software", "Pong (1 player).ch8");
	conf[TMC1800].screenDumpFile_ = configPointer->Read("/TMC1800/Video_Dump_File", "screendump.png");
	conf[TMC1800].wavFile_[0] = configPointer->Read("/TMC1800/Wav_File", "");

	conf[TMC1800].turboClock_ = configPointer->Read("/TMC1800/Turbo_Clock_Speed", "15");
	conf[TMC1800].volume_ = (int)configPointer->Read("/TMC1800/Volume", 25l);
	conf[TMC1800].ramType_ = (int)configPointer->Read("/TMC1800/Ram_Type", 02);
	conf[TMC1800].beepFrequency_ = (int)configPointer->Read("/TMC1800/Beep_Frequency", 250);

	configPointer->Read("/TMC1800/Use_Keyboard", &conf[TMC1800].useKeyboard_, false);
	configPointer->Read("/TMC1800/Enable_Turbo_Cassette", &conf[TMC1800].turbo_, true);
	configPointer->Read("/TMC1800/Enable_Auto_Cassette", &conf[TMC1800].autoCassetteLoad_, true);
	configPointer->Read("/TMC1800/Enable_Real_Cassette", &conf[TMC1800].realCassetteLoad_, false);

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[TMC1800].zoom_ = convertLocale(configPointer->Read("/TMC1800/Zoom", defaultZoom));
	wxString defaultScale;
	defaultScale.Printf("%i", 3);
	conf[TMC1800].xScale_ = convertLocale(configPointer->Read("/TMC1800/Window_Scale_Factor_X", defaultScale));
	wxString defaultClock;
	defaultClock.Printf("%1.2f", 1.75);
	conf[TMC1800].clock_ = convertLocale(configPointer->Read("/TMC1800/Clock_Speed", defaultClock));

	setRealCas(TMC1800);

    long value;
    conf[TMC1800].saveStartString_ = configPointer->Read("/TMC1800/SaveStart", "0");
    if (!conf[TMC1800].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[TMC1800].saveStart_ = value;

	if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomTMC1800", wxComboBox)->SetValue(conf[TMC1800].rom_[MAINROM1]);
		XRCCTRL(*this, "RamSWTMC1800", wxComboBox)->SetValue(conf[TMC1800].ram_);
		XRCCTRL(*this, "Chip8SWTMC1800", wxTextCtrl)->SetValue(conf[TMC1800].chip8SW_);
		XRCCTRL(*this, "ScreenDumpFileTMC1800", wxComboBox)->SetValue(conf[TMC1800].screenDumpFile_);
		XRCCTRL(*this, "WavFileTMC1800", wxTextCtrl)->SetValue(conf[TMC1800].wavFile_[0]);

        correctZoomAndValue(TMC1800, "TMC1800", SET_SPIN);

		XRCCTRL(*this, "TurboTMC1800", wxCheckBox)->SetValue(conf[TMC1800].turbo_);
		turboGui("TMC1800");
		XRCCTRL(*this, "TurboClockTMC1800", wxTextCtrl)->SetValue(conf[TMC1800].turboClock_);
		XRCCTRL(*this, "AutoCasLoadTMC1800", wxCheckBox)->SetValue(conf[TMC1800].autoCassetteLoad_);

		XRCCTRL(*this, "VolumeTMC1800", wxSlider)->SetValue(conf[TMC1800].volume_);
		XRCCTRL(*this, "RamTMC1800", wxChoice)->SetSelection(conf[TMC1800].ramType_);
        if (clockTextCtrl[TMC1800] != NULL)
            clockTextCtrl[TMC1800]->ChangeValue(conf[TMC1800].clock_);

		XRCCTRL(*this, "KeyboardTMC1800", wxCheckBox)->SetValue(conf[TMC1800].useKeyboard_);
		XRCCTRL(*this, "KeyMapTMC1800", wxButton)->Enable(!conf[TMC1800].useKeyboard_);

		wxString beepFrequency;
		beepFrequency.Printf("%d", conf[TMC1800].beepFrequency_);
		XRCCTRL(*this, "BeepFrequencyTMC1800", wxTextCtrl)->ChangeValue(beepFrequency);
        if (conf[TMC1800].saveStart_ != 0)
            XRCCTRL(*this, "SaveStartTMC1800", wxTextCtrl)->SetValue(conf[TMC1800].saveStartString_);
	}
}

void GuiTMC2000::writeTMC1800DirConfig()
{
    writeConfigDir("/Dir/TMC1800/Main", conf[TMC1800].mainDir_);
    writeConfigDir("/Dir/TMC1800/Main_Rom_File", conf[TMC1800].romDir_[MAINROM1]);
	writeConfigDir("/Dir/TMC1800/Ram_Software", conf[TMC1800].ramDir_);
	writeConfigDir("/Dir/TMC1800/Chip_8_Software", conf[TMC1800].chip8SWDir_);
	writeConfigDir("/Dir/TMC1800/Video_Dump_File", conf[TMC1800].screenDumpFileDir_);
	writeConfigDir("/Dir/TMC1800/Wav_File", conf[TMC1800].wavFileDir_[0]);
}

void GuiTMC2000::writeTMC1800Config()
{
	configPointer->Write("/TMC1800/Main_Rom_File", conf[TMC1800].rom_[MAINROM1]);
	configPointer->Write("/TMC1800/Ram_Software", conf[TMC1800].ram_);
	configPointer->Write("/TMC1800/Chip_8_Software", conf[TMC1800].chip8SW_);
	configPointer->Write("/TMC1800/Video_Dump_File", conf[TMC1800].screenDumpFile_);
	configPointer->Write("/TMC1800/Wav_File", conf[TMC1800].wavFile_[0]);

	configPointer->Write("/TMC1800/Zoom", conf[TMC1800].zoom_);

	configPointer->Write("/TMC1800/Enable_Turbo_Cassette", conf[TMC1800].turbo_);
	configPointer->Write("/TMC1800/Turbo_Clock_Speed", conf[TMC1800].turboClock_);
	configPointer->Write("/TMC1800/Enable_Auto_Cassette", conf[TMC1800].autoCassetteLoad_);
	configPointer->Write("/TMC1800/Enable_Real_Cassette", conf[TMC1800].realCassetteLoad_);
	configPointer->Write("/TMC1800/Volume", conf[TMC1800].volume_);
	configPointer->Write("/TMC1800/Ram_Type", conf[TMC1800].ramType_);
    configPointer->Write("/TMC1800/SaveStart", conf[TMC1800].saveStartString_);

	configPointer->Write("/TMC1800/Clock_Speed", conf[TMC1800].clock_);
	configPointer->Write("/TMC1800/Beep_Frequency", conf[TMC1800].beepFrequency_);
	configPointer->Write("/TMC1800/Use_Keyboard", conf[TMC1800].useKeyboard_);
}

void GuiTMC2000::readTMC1800WindowConfig()
{
    conf[TMC1800].mainX_ = (int)configPointer->Read("/TMC1800/Window_Position_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[TMC1800].mainY_ = (int)configPointer->Read("/TMC1800/Window_Position_Y", mainWindowY_);
}

void GuiTMC2000::writeTMC1800WindowConfig()
{
    if (conf[TMC1800].mainX_ > 0)
        configPointer->Write("/TMC1800/Window_Position_X", conf[TMC1800].mainX_);
    if (conf[TMC1800].mainY_ > 0)
        configPointer->Write("/TMC1800/Window_Position_Y", conf[TMC1800].mainY_);
}


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
#include "guieti660.h"
#include "pixie.h"

#include "psave.h"

BEGIN_EVENT_TABLE(GuiEti, GuiNano)
	EVT_TEXT(XRCID("MainRomEti"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomEti"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonEti"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("Chip8SWEti"), GuiMain::onChip8SWText)
	EVT_BUTTON(XRCID("Chip8SWButtonEti"), GuiMain::onChip8SW)
	EVT_BUTTON(XRCID("EjectChip8SWEti"), GuiMain::onEjectChip8SW)

	EVT_SPIN_UP(XRCID("ZoomSpinEti"), GuiMain::onZoomUp)
	EVT_SPIN_DOWN(XRCID("ZoomSpinEti"), GuiMain::onZoomDown)
	EVT_TEXT(XRCID("ZoomValueEti"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3Eti"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("CasButtonEti"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasEti"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileEti"), GuiMain::onCassetteText)

	EVT_BUTTON(XRCID("ScreenDumpFileButtonEti"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileEti"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileEti"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5Eti"), GuiMain::onScreenDump)

	EVT_BUTTON(XRCID("RealCasLoadEti"), GuiMain::onRealCas)
	EVT_BUTTON(XRCID("CasLoadEti"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSaveEti"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopEti"), GuiMain::onCassetteStop)
	EVT_CHECKBOX(XRCID("TurboEti"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockEti"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadEti"), GuiMain::onAutoLoad)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeEti"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeEti"), GuiMain::onVolume)
	EVT_BUTTON(XRCID("SaveButtonEti"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonEti"), GuiMain::onLoadButton)
	EVT_BUTTON(XRCID("KeyMapEti"), Main::onHexKeyDef)
	EVT_BUTTON(XRCID("ColoursEti"), Main::onColoursDef)
	EVT_CHOICE(XRCID("RamEti"), GuiMain::onChoiceRam)

	EVT_TEXT(XRCID("SaveStartEti"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndEti"), GuiMain::onSaveEnd)

END_EVENT_TABLE()

GuiEti::GuiEti(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir)
: GuiNano(title, pos, size, mode, dataDir)
{
	conf[ETI].saveStartString_ = "";
	conf[ETI].saveEndString_ = "";
}

void GuiEti::readEtiConfig()
{
	selectedComputer_ = ETI;

    conf[ETI].configurationDir_ = dataDir_ + "Configurations" + pathSeparator_ + "Eti" + pathSeparator_;
    conf[ETI].mainDir_ = readConfigDir("/Dir/Eti/Main", dataDir_ + "Eti" + pathSeparator_);
    
    conf[ETI].romDir_[MAINROM1] = readConfigDir("/Dir/Eti/Main_Rom_File", dataDir_ + "Eti"  + pathSeparator_);
	conf[ETI].ramDir_ = readConfigDir("/Dir/Eti/Software_File", dataDir_ + "Eti"  + pathSeparator_);
	conf[ETI].chip8SWDir_ = readConfigDir("/Dir/Eti/Chip_8_Software", dataDir_ + "Chip-8"  + pathSeparator_ + "Chip-8 ETI660 Hybrids"  + pathSeparator_);
	conf[ETI].screenDumpFileDir_ = readConfigDir("/Dir/Eti/Video_Dump_File", dataDir_ + "Eti" + pathSeparator_);
	conf[ETI].wavFileDir_ = readConfigDir("/Dir/Eti/Wav_File", dataDir_ + "Eti" + pathSeparator_);

	conf[ETI].rom_[MAINROM1] = configPointer->Read("/Eti/Main_Rom_File", "eti-660.bin");
	conf[ETI].ram_ = configPointer->Read("/TMC2000/Ram_Software", "chip8.ram");
	conf[ETI].chip8SW_ = configPointer->Read("/Eti/Chip_8_Software", "Wipeout (ETI660 hybrid) [W.F. Kreykes, 1982].ch8");
	conf[ETI].wavFile_ = configPointer->Read("/Eti/Wav_File", "");
	conf[ETI].screenDumpFile_ = configPointer->Read("/Eti/Video_Dump_File", "screendump.png");

	conf[ETI].turboClock_ = configPointer->Read("/Eti/Turbo_Clock_Speed", "15");
	conf[ETI].volume_ = (int)configPointer->Read("/Eti/Volume", 25l);
	conf[ETI].ramType_ = (int)configPointer->Read("/Eti/Memory_Type", 0l);

	configPointer->Read("/Eti/Enable_Turbo_Cassette", &conf[ETI].turbo_, true);
	configPointer->Read("/Eti/Enable_Auto_Cassette", &conf[ETI].autoCassetteLoad_, true);
	configPointer->Read("/Eti/Enable_Real_Cassette", &conf[ETI].realCassetteLoad_, false);

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[ETI].zoom_ = configPointer->Read("/Eti/Zoom", defaultZoom);
	wxString defaultScale;
	defaultScale.Printf("%i", 4);
	conf[ETI].xScale_ = configPointer->Read("/Eti/Window_Scale_Factor_X", defaultScale);
	wxString defaultClock;
	defaultClock.Printf("%1.3f", 1.773);
	conf[ETI].clock_ = configPointer->Read("/Eti/Clock_Speed", defaultClock);

	setRealCas(ETI);

	if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomEti", wxComboBox)->SetValue(conf[ETI].rom_[MAINROM1]);
		XRCCTRL(*this, "Chip8SWEti", wxTextCtrl)->SetValue(conf[ETI].chip8SW_);
		XRCCTRL(*this, "ScreenDumpFileEti", wxComboBox)->SetValue(conf[ETI].screenDumpFile_);
		XRCCTRL(*this, "WavFileEti", wxTextCtrl)->SetValue(conf[ETI].wavFile_);

		XRCCTRL(*this, "ZoomValueEti", wxTextCtrl)->ChangeValue(conf[ETI].zoom_);

		XRCCTRL(*this, "TurboEti", wxCheckBox)->SetValue(conf[ETI].turbo_);
		turboGui("Eti");
		XRCCTRL(*this, "TurboClockEti", wxTextCtrl)->SetValue(conf[ETI].turboClock_);
		XRCCTRL(*this, "AutoCasLoadEti", wxCheckBox)->SetValue(conf[ETI].autoCassetteLoad_);

		XRCCTRL(*this, "VolumeEti", wxSlider)->SetValue(conf[ETI].volume_);
		XRCCTRL(*this, "RamEti", wxChoice)->SetSelection(conf[ETI].ramType_);

		clockTextCtrl[ETI]->ChangeValue(conf[ETI].clock_);
	}
}

void GuiEti::writeEtiDirConfig()
{
    writeConfigDir("/Dir/Eti/Main", conf[ETI].mainDir_);
    writeConfigDir("/Dir/Eti/Main_Rom_File", conf[ETI].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Eti/Software_File", conf[ETI].ramDir_);
	writeConfigDir("/Dir/Eti/Chip_8_Software", conf[ETI].chip8SWDir_);
	writeConfigDir("/Dir/Eti/Video_Dump_File", conf[ETI].screenDumpFileDir_);
	writeConfigDir("/Dir/Eti/Wav_File", conf[ETI].wavFileDir_);
}

void GuiEti::writeEtiConfig()
{
	configPointer->Write("/Eti/Main_Rom_File", conf[ETI].rom_[MAINROM1]);
	configPointer->Write("/Eti/Chip_8_Software", conf[ETI].chip8SW_);
	configPointer->Write("/Eti/Video_Dump_File", conf[ETI].screenDumpFile_);
	configPointer->Write("/Eti/Wav_File", conf[ETI].wavFile_);

	configPointer->Write("/Eti/Zoom", conf[ETI].zoom_);

    configPointer->Write("/Eti/Enable_Turbo_Cassette", conf[ETI].turbo_);
	configPointer->Write("/Eti/Turbo_Clock_Speed", conf[ETI].turboClock_);
	configPointer->Write("/Eti/Enable_Auto_Cassette", conf[ETI].autoCassetteLoad_);
	configPointer->Write("/Eti/Enable_Real_Cassette", conf[ETI].realCassetteLoad_);
	configPointer->Write("/Eti/Volume", conf[ETI].volume_);
	configPointer->Write("/Eti/Memory_Type", conf[ETI].ramType_);

	configPointer->Write("/Eti/Clock_Speed", conf[ETI].clock_);
}

void GuiEti::readEtiWindowConfig()
{
    conf[ETI].mainX_ = (int)configPointer->Read("/Eti/Window_Position_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[ETI].mainY_ = (int)configPointer->Read("/Eti/Window_Position_Y", mainWindowY_);
}

void GuiEti::writeEtiWindowConfig()
{
    if (conf[ETI].mainX_ > 0)
        configPointer->Write("/Eti/Window_Position_X", conf[ETI].mainX_);
    if (conf[ETI].mainY_ > 0)
        configPointer->Write("/Eti/Window_Position_Y", conf[ETI].mainY_);
}
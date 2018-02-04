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
#include "guinano.h"
#include "pixie.h"

#include "psave.h"

BEGIN_EVENT_TABLE(GuiNano, GuiCidelsa)

	EVT_TEXT(XRCID("MainRomNano"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomNano"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonNano"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("RamSWNano"), GuiMain::onRamSWText)
	EVT_COMBOBOX(XRCID("RamSWNano"), GuiMain::onRamSWText)
	EVT_BUTTON(XRCID("RamSWButtonNano"), GuiMain::onRamSW)

	EVT_TEXT(XRCID("Chip8SWNano"), GuiMain::onChip8SWText)
	EVT_BUTTON(XRCID("Chip8SWButtonNano"), GuiMain::onChip8SW)
	EVT_BUTTON(XRCID("EjectChip8SWNano"), GuiMain::onEjectChip8SW)

	EVT_SPIN_UP(XRCID("ZoomSpinNano"), GuiMain::onZoomUp)
	EVT_SPIN_DOWN(XRCID("ZoomSpinNano"), GuiMain::onZoomDown)
	EVT_TEXT(XRCID("ZoomValueNano"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3Nano"), GuiMain::onFullScreen)

	EVT_BUTTON(XRCID("ScreenDumpFileButtonNano"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileNano"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileNano"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5Nano"), GuiMain::onScreenDump)

	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeNano"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeNano"), GuiMain::onVolume)

	EVT_BUTTON(XRCID("CasButtonNano"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasNano"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileNano"), GuiMain::onCassetteText)

	EVT_BUTTON(XRCID("RealCasLoadNano"), GuiMain::onRealCas)
	EVT_CHECKBOX(XRCID("TurboNano"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockNano"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadNano"), GuiMain::onAutoLoad)
	EVT_BUTTON(XRCID("CasLoadNano"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSaveNano"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopNano"), GuiMain::onCassetteStop)
	EVT_BUTTON(XRCID("SaveButtonNano"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonNano"), GuiMain::onLoadButton)
	EVT_BUTTON(XRCID("KeyMapNano"), Main::onHexKeyDef)
	EVT_BUTTON(XRCID("ColoursNano"), Main::onColoursDef)

	EVT_CHOICE(XRCID("SoundNano"), GuiNano::onNanoSound)
	
	EVT_TEXT(XRCID("SaveStartNano"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndNano"), GuiMain::onSaveEnd)

END_EVENT_TABLE()

GuiNano::GuiNano(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiCidelsa(title, pos, size, mode, dataDir, iniDir)
{
	conf[NANO].saveStartString_ = "";
	conf[NANO].saveEndString_ = "";
}

void GuiNano::readNanoConfig()
{
	selectedComputer_ = NANO;

    conf[NANO].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Nano" + pathSeparator_;
    conf[NANO].mainDir_ = readConfigDir("/Dir/Nano/Main", dataDir_ + "Nano" + pathSeparator_);
    
    conf[NANO].romDir_[MAINROM1] = readConfigDir("/Dir/Nano/Main_Rom_File", dataDir_ + "Nano"  + pathSeparator_);
	conf[NANO].ramDir_ = readConfigDir("/Dir/Nano/Ram_Software", dataDir_ + "Nano"  + pathSeparator_);
	conf[NANO].chip8SWDir_ = readConfigDir("/Dir/Nano/Chip_8_Software", dataDir_ + "Chip-8"  + pathSeparator_ + "Chip-8 Games"  + pathSeparator_);
	conf[NANO].screenDumpFileDir_ = readConfigDir("/Dir/Nano/Video_Dump_File", dataDir_ + "Nano" + pathSeparator_);
	conf[NANO].wavFileDir_ = readConfigDir("/Dir/Nano/Wav_File", dataDir_ + "Nano" + pathSeparator_);

	conf[NANO].rom_[MAINROM1] = configPointer->Read("/Nano/Main_Rom_File", "nano.rom");
	conf[NANO].ram_ = configPointer->Read("/Nano/Ram_Software", "chip8.ram");
	conf[NANO].chip8SW_ = configPointer->Read("/Nano/Chip_8_Software", "Breakout (Brix hack) [David Winter, 1997].ch8");
	conf[NANO].screenDumpFile_ = configPointer->Read("/Nano/Video_Dump_File", "screendump.png");
	conf[NANO].wavFile_ = configPointer->Read("/Nano/Wav_File", "");

	conf[NANO].turboClock_ = configPointer->Read("/Nano/Turbo_Clock_Speed", "15");
	conf[NANO].volume_ = (int)configPointer->Read("/Nano/Volume", 25l);
	conf[NANO].soundType_ = (int)configPointer->Read("/Nano/Sound", 0l);

	configPointer->Read("/Nano/Enable_Turbo_Cassette", &conf[NANO].turbo_, true);
	configPointer->Read("/Nano/Enable_Auto_Cassette", &conf[NANO].autoCassetteLoad_, true);
	configPointer->Read("/Nano/Enable_Real_Cassette", &conf[NANO].realCassetteLoad_, false);

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[NANO].zoom_ = configPointer->Read("/Nano/Zoom", defaultZoom);
	wxString defaultScale;
	defaultScale.Printf("%i", 4);
	conf[NANO].xScale_ = configPointer->Read("/Nano/Window_Scale_Factor_X", defaultScale);
	wxString defaultClock;
	defaultClock.Printf("%1.2f", 1.75);
	conf[NANO].clock_ = configPointer->Read("/Nano/Clock_Speed", defaultClock);

	setRealCas(NANO);

	if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomNano", wxComboBox)->SetValue(conf[NANO].rom_[MAINROM1]);
		XRCCTRL(*this, "RamSWNano", wxComboBox)->SetValue(conf[NANO].ram_);
		XRCCTRL(*this, "Chip8SWNano", wxTextCtrl)->SetValue(conf[NANO].chip8SW_);
		XRCCTRL(*this, "ScreenDumpFileNano", wxComboBox)->SetValue(conf[NANO].screenDumpFile_);
		XRCCTRL(*this, "WavFileNano", wxTextCtrl)->SetValue(conf[NANO].wavFile_);

		XRCCTRL(*this, "ZoomValueNano", wxTextCtrl)->ChangeValue(conf[NANO].zoom_);


		XRCCTRL(*this, "TurboNano", wxCheckBox)->SetValue(conf[NANO].turbo_);
		turboGui("Nano");
		XRCCTRL(*this, "TurboClockNano", wxTextCtrl)->SetValue(conf[NANO].turboClock_);
		XRCCTRL(*this, "AutoCasLoadNano", wxCheckBox)->SetValue(conf[NANO].autoCassetteLoad_);

		XRCCTRL(*this, "VolumeNano", wxSlider)->SetValue(conf[NANO].volume_);
		clockTextCtrl[NANO]->ChangeValue(conf[NANO].clock_);

		XRCCTRL(*this, "SoundNano", wxChoice)->SetSelection(conf[NANO].soundType_);
	}
}

void GuiNano::writeNanoDirConfig()
{
    writeConfigDir("/Dir/Nano/Main", conf[NANO].mainDir_);
    writeConfigDir("/Dir/Nano/Main_Rom_File", conf[NANO].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Nano/Ram_Software", conf[NANO].ramDir_);
	writeConfigDir("/Dir/Nano/Chip_8_Software", conf[NANO].chip8SWDir_);
	writeConfigDir("/Dir/Nano/Video_Dump_File", conf[NANO].screenDumpFileDir_);
	writeConfigDir("/Dir/Nano/Wav_File", conf[NANO].wavFileDir_);
}

void GuiNano::writeNanoConfig()
{
	configPointer->Write("/Nano/Main_Rom_File", conf[NANO].rom_[MAINROM1]);
	configPointer->Write("/Nano/Ram_Software", conf[NANO].ram_);
	configPointer->Write("/Nano/Chip_8_Software", conf[NANO].chip8SW_);
	configPointer->Write("/Nano/Video_Dump_File", conf[NANO].screenDumpFile_);
	configPointer->Write("/Nano/Wav_File", conf[NANO].wavFile_);

	configPointer->Write("/Nano/Zoom", conf[NANO].zoom_);

	configPointer->Write("/Nano/Enable_Turbo_Cassette", conf[NANO].turbo_);
	configPointer->Write("/Nano/Turbo_Clock_Speed", conf[NANO].turboClock_);
	configPointer->Write("/Nano/Enable_Auto_Cassette", conf[NANO].autoCassetteLoad_);
	configPointer->Write("/Nano/Enable_Real_Cassette", conf[NANO].realCassetteLoad_);
	configPointer->Write("/Nano/Volume", conf[NANO].volume_);

	configPointer->Write("/Nano/Clock_Speed", conf[NANO].clock_);
	configPointer->Write("/Nano/Sound", conf[NANO].soundType_);
}

void GuiNano::readNanoWindowConfig()
{
    conf[NANO].mainX_ = (int)configPointer->Read("/Nano/Window_Position_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[NANO].mainY_ = (int)configPointer->Read("/Nano/Window_Position_Y", mainWindowY_);
}

void GuiNano::writeNanoWindowConfig()
{
    if (conf[NANO].mainX_ > 0)
        configPointer->Write("/Nano/Window_Position_X", conf[NANO].mainX_);
    if (conf[NANO].mainY_ > 0)
        configPointer->Write("/Nano/Window_Position_Y", conf[NANO].mainY_);
}

void GuiNano::onNanoSound(wxCommandEvent&event)
{
	conf[NANO].soundType_ = event.GetSelection();
}


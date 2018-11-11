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
#include "guivip2k.h"
#include "pixie.h"

#include "psave.h"

DEFINE_EVENT_TYPE(ON_UART_VIP2K)

BEGIN_EVENT_TABLE(GuiVip2K, GuiVelf)

	EVT_TEXT(XRCID("MainRomVip2K"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomVip2K"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonVip2K"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("MainRom2Vip2K"), GuiMain::onMainRom2Text)
	EVT_COMBOBOX(XRCID("MainRom2Vip2K"), GuiMain::onMainRom2Text)
	EVT_BUTTON(XRCID("RomButton2Vip2K"), GuiMain::onMainRom2)

	EVT_TEXT(XRCID("RamSWVip2K"), GuiVip2K::onRamSWText)
	EVT_COMBOBOX(XRCID("RamSWVip2K"), GuiVip2K::onRamSWText)
	EVT_BUTTON(XRCID("RamSWButtonVip2K"), GuiVip2K::onRamSW)

	EVT_TEXT(XRCID("Chip8SWVip2K"), GuiVip2K::onChip8SWText)
	EVT_BUTTON(XRCID("Chip8SWButtonVip2K"), GuiMain::onChip8SW)
	EVT_BUTTON(XRCID("EjectChip8SWVip2K"), GuiMain::onEjectChip8SW)

	EVT_SPIN_UP(XRCID("ZoomSpinVip2K"), GuiMain::onZoomUp)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVip2K"), GuiMain::onZoomDown)
	EVT_TEXT(XRCID("ZoomValueVip2K"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3Vip2K"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("ScreenDumpFileButtonVip2K"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileVip2K"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileVip2K"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5Vip2K"), GuiMain::onScreenDump)

	EVT_BUTTON(XRCID("CasLoadVip2K"), GuiMain::onTerminalLoad)
	EVT_BUTTON(XRCID("CasSaveVip2K"), GuiMain::onTerminalSave)
	EVT_BUTTON(XRCID("CasStopVip2K"), GuiMain::onTerminalStop)
    EVT_BUTTON(XRCID("CasButtonVip2K"), GuiMain::onTerminalFile)
    EVT_BUTTON(XRCID("EjectCasVip2K"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileVip2K"), GuiMain::onCassetteText)
	EVT_CHECKBOX(XRCID("AutoCasLoadVip2K"), GuiMain::onAutoLoad)

	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeVip2K"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeVip2K"), GuiMain::onVolume)
	EVT_BUTTON(XRCID("ColoursVip2K"), Main::onColoursDef)

	EVT_TEXT(XRCID("SaveEndVip2K"), GuiMain::onSaveEnd)
	EVT_TEXT(XRCID("SaveExecVip2K"), GuiMain::onSaveExec)

	EVT_BUTTON(XRCID("SaveButtonVip2K"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonVip2K"), GuiMain::onLoadButton)

	EVT_TEXT(XRCID("VtCharRomVip2K"), GuiMain::onVtCharRomText)
	EVT_COMBOBOX(XRCID("VtCharRomVip2K"), GuiMain::onVtCharRomText)
	EVT_BUTTON(XRCID("VtCharRomButtonVip2K"), GuiMain::onVtCharRom)

	EVT_CHOICE(XRCID("VTTypeVip2K"), GuiMain::onVT100)
	EVT_SPIN_UP(XRCID("ZoomSpinVtVip2K"), GuiMain::onZoomUpVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtVip2K"), GuiMain::onZoomDownVt)
	EVT_TEXT(XRCID("ZoomValueVtVip2K"), GuiMain::onZoomValueVt)
	EVT_BUTTON(XRCID("VtSetupVip2K"), GuiMain::onVtSetup)
	EVT_CHECKBOX(XRCID("StretchDotVip2K"), GuiMain::onStretchDot)

    EVT_CHECKBOX(XRCID("VtShowVip2K"), GuiVip2K::onVtShow)
	EVT_CHOICE(XRCID("VTBaudTChoiceVip2K"), GuiVip2K::onVipBaudT)
	EVT_CHOICE(XRCID("VTBaudRChoiceVip2K"), GuiVip2K::onVipBaudR)

    EVT_COMMAND(wxID_ANY, ON_UART_VIP2K, GuiVip2K::onVipUart)

END_EVENT_TABLE()

GuiVip2K::GuiVip2K(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiVelf(title, pos, size, mode, dataDir, iniDir)
{
	conf[VIP2K].loadFileNameFull_ = "";
	conf[VIP2K].loadFileName_ = "";

	conf[VIP2K].pLoadSaveName_[0] = 'F';
	conf[VIP2K].pLoadSaveName_[1] = 'P';
	conf[VIP2K].pLoadSaveName_[2] = 'B';
	conf[VIP2K].pLoadSaveName_[3] = ' ';

	conf[VIP2K].saveStartString_ = "";
	conf[VIP2K].saveEndString_ = "";
	conf[VIP2K].saveExecString_ = "";
    
    position_.x = 0;
}

void GuiVip2K::readVip2KConfig()
{
	selectedComputer_ = VIP2K;

    conf[VIP2K].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Vip2K" + pathSeparator_;
    conf[VIP2K].mainDir_ = readConfigDir("/Dir/Vip2K/Main", dataDir_ + "Vip2K" + pathSeparator_);
    
    conf[VIP2K].romDir_[MAINROM1] = readConfigDir("/Dir/Vip2K/Main_Rom_File", dataDir_ + "Vip2K"  + pathSeparator_);
    conf[VIP2K].romDir_[MAINROM2] = readConfigDir("/Dir/Vip2K/Sequencer_Rom_File", dataDir_ + "Vip2K"  + pathSeparator_);
	conf[VIP2K].ramDir_ = readConfigDir("/Dir/Vip2K/Software_File", dataDir_ + "Vip2K"  + pathSeparator_);
	conf[VIP2K].vtCharRomDir_ = readConfigDir("/Dir/Vip2K/Vt_Font_Rom_File", dataDir_ + "Vip2K" + pathSeparator_);
	conf[VIP2K].chip8SWDir_ = readConfigDir("/Dir/Vip2K/Chip_8_Software", dataDir_ + "Chip-8"  + pathSeparator_ + "Chip-8 Games"  + pathSeparator_);
	conf[VIP2K].printFileDir_ = readConfigDir("/Dir/Vip2K/Print_File", dataDir_ + "Vip2K" + pathSeparator_);
	conf[VIP2K].screenDumpFileDir_ = readConfigDir("/Dir/Vip2K/Video_Dump_File", dataDir_ + "Vip2K" + pathSeparator_);
	conf[VIP2K].wavFileDir_ = readConfigDir("/Dir/Vip2K/Wav_File", dataDir_ + "Vip2K" + pathSeparator_);
	elfConfiguration[VIP2K].vtWavFileDir_ = readConfigDir("/Dir/Vip2K/Vt_Wav_File", dataDir_ + "Vip2K" + pathSeparator_);

	conf[VIP2K].rom_[MAINROM1] = configPointer->Read("/Vip2K/Main_Rom_File", "vip2k13.hex");
	conf[VIP2K].rom_[MAINROM2] = configPointer->Read("/Vip2K/Sequencer_Rom_File", "vip-2716.hex");
	conf[VIP2K].ram_ = configPointer->Read("/Vip2K/Ram_Software", "");
	conf[VIP2K].chip8SW_ = configPointer->Read("/Vip2K/Chip_8_Software", "");
	elfConfiguration[VIP2K].vtWavFile_ = configPointer->Read("/Vip2K/Vt_Wav_File", "file.hex");
    elfConfiguration[VIP2K].serialPort_ = configPointer->Read("/Vip2K/VtSerialPortChoice", "");

	conf[VIP2K].printFile_ = configPointer->Read("/Vip2K/Print_File", "printerout.txt");
	conf[VIP2K].screenDumpFile_ = configPointer->Read("/Vip2K/Video_Dump_File", "screendump.png");
	conf[VIP2K].turboClock_ = configPointer->Read("/Vip2K/Turbo_Clock_Speed", "15");

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[VIP2K].zoom_ = configPointer->Read("/Vip2K/Zoom", defaultZoom);
	defaultZoom.Printf("%2.2f", 1.0);
	conf[VIP2K].zoomVt_ = configPointer->Read("/Vip2K/Vt_Zoom", defaultZoom);
	wxString defaultScale;
	defaultScale.Printf("%1.1f", 1.5);
	conf[VIP2K].xScale_ = configPointer->Read("/Vip2K/Window_Scale_Factor_X", defaultScale);

	configPointer->Read("/Vip2K/Enable_Vt_Stretch_Dot", &conf[VIP2K].stretchDot_, false);
    configPointer->Read("/Vip2K/Enable_Vt_External", &elfConfiguration[VIP2K].vtExternal, false);
	configPointer->Read("/Vip2K/Enable_Turbo_Cassette", &conf[VIP2K].turbo_, true);
	configPointer->Read("/Vip2K/Enable_Auto_Cassette", &conf[VIP2K].autoCassetteLoad_, true);
	configPointer->Read("/Vip2K/Enable_Real_Cassette", &conf[VIP2K].realCassetteLoad_, false);

	conf[VIP2K].volume_ = (int)configPointer->Read("/Vip2K/Volume", 25l);

	setRealCas(VIP2K);

	wxString defaultClock;
	defaultClock.Printf("%1.1f", 4.0);
	conf[VIP2K].clock_ = configPointer->Read("/Vip2K/Clock_Speed", defaultClock);

	configPointer->Read("/Vip2K/VtEf", &elfConfiguration[VIP2K].vtEf, true);
	configPointer->Read("/Vip2K/VtQ", &elfConfiguration[VIP2K].vtQ, false);
	elfConfiguration[VIP2K].bellFrequency_ = (int)configPointer->Read("/Vip2K/Bell_Frequency", 800);

    configPointer->Read("/Vip2K/Uart", &elfConfiguration[VIP2K].useUart, false);
	elfConfiguration[VIP2K].vtType = (int)configPointer->Read("/Vip2K/VT_Type", 2l);
    elfConfiguration[VIP2K].vt52SetUpFeature_ = configPointer->Read("/Vip2K/VT52Setup", 0x00004092l);
    elfConfiguration[VIP2K].vt100SetUpFeature_ = configPointer->Read("/Vip2K/VT100Setup", 0x0000ca52l);
    elfConfiguration[VIP2K].vtExternalSetUpFeature_ = configPointer->Read("/Vip2K/VTExternalSetup", 0x0000ca52l);
	elfConfiguration[VIP2K].baudT = (int)configPointer->Read("/Vip2K/Vt_Baud", 5l);
    elfConfiguration[VIP2K].baudR = (int)configPointer->Read("/Membership/Vt_Baud_Receive", 1l);
    elfConfiguration[VIP2K].baudT = (int)configPointer->Read("/Membership/Vt_Baud_Transmit", 1l);
    configPointer->Read("/Vip2K/ShowVtWindow", &elfConfiguration[VIP2K].vtShow, false);

	setVtType("Vip2K", VIP2K, elfConfiguration[VIP2K].vtType, false);

	conf[VIP2K].vtCharRom_ = configPointer->Read("/Vip2K/Vt_Font_Rom_File", "vt100.bin");

	if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomVip2K", wxComboBox)->SetValue(conf[VIP2K].rom_[MAINROM1]);
		XRCCTRL(*this, "MainRom2Vip2K", wxComboBox)->SetValue(conf[VIP2K].rom_[MAINROM2]);
		XRCCTRL(*this, "RamSWVip2K", wxComboBox)->SetValue(conf[VIP2K].ram_);
		XRCCTRL(*this, "Chip8SWVip2K", wxTextCtrl)->SetValue(conf[VIP2K].chip8SW_);
		XRCCTRL(*this, "VtCharRomVip2K", wxComboBox)->SetValue(conf[VIP2K].vtCharRom_);
		XRCCTRL(*this, "ScreenDumpFileVip2K", wxComboBox)->SetValue(conf[VIP2K].screenDumpFile_);
		XRCCTRL(*this, "WavFileVip2K", wxTextCtrl)->SetValue(elfConfiguration[VIP2K].vtWavFile_);

		XRCCTRL(*this, "VTTypeVip2K", wxChoice)->SetSelection(elfConfiguration[VIP2K].vtType);

		XRCCTRL(*this, "VTBaudTChoiceVip2K", wxChoice)->SetSelection(elfConfiguration[VIP2K].baudT);
		XRCCTRL(*this, "VTBaudRChoiceVip2K", wxChoice)->SetSelection(elfConfiguration[VIP2K].baudT);
		XRCCTRL(*this, "ZoomValueVtVip2K", wxTextCtrl)->ChangeValue(conf[VIP2K].zoomVt_);
        XRCCTRL(*this, "VtShowVip2K", wxCheckBox)->SetValue(elfConfiguration[VIP2K].vtShow);

		XRCCTRL(*this, "ZoomValueVip2K", wxTextCtrl)->ChangeValue(conf[VIP2K].zoom_);
        XRCCTRL(*this, "AutoCasLoadVip2K", wxCheckBox)->SetValue(conf[VIP2K].autoCassetteLoad_);
		XRCCTRL(*this, "VolumeVip2K", wxSlider)->SetValue(conf[VIP2K].volume_);
		clockTextCtrl[VIP2K]->ChangeValue(conf[VIP2K].clock_);
	}
}

void GuiVip2K::writeVip2KDirConfig()
{
    writeConfigDir("/Dir/Vip2K/Main", conf[VIP2K].mainDir_);
    writeConfigDir("/Dir/Vip2K/Main_Rom_File", conf[VIP2K].romDir_[MAINROM1]);
    writeConfigDir("/Dir/Vip2K/Sequencer_Rom_File", conf[VIP2K].romDir_[MAINROM2]);
	writeConfigDir("/Dir/Vip2K/Software_File", conf[VIP2K].ramDir_);
	writeConfigDir("/Dir/Vip2K/Chip_8_Software", conf[VIP2K].chip8SWDir_);
	writeConfigDir("/Dir/Vip2K/Print_File", conf[VIP2K].printFileDir_);
	writeConfigDir("/Dir/Vip2K/Vt_Font_Rom_File", conf[VIP2K].vtCharRomDir_);
	writeConfigDir("/Dir/Vip2K/Video_Dump_File", conf[VIP2K].screenDumpFileDir_);
	writeConfigDir("/Dir/Vip2K/Wav_File", conf[VIP2K].wavFileDir_);
	writeConfigDir("/Dir/Vip2K/Vt_Wav_File", elfConfiguration[VIP2K].vtWavFileDir_);
}

void GuiVip2K::writeVip2KConfig()
{
    configPointer->Write("/Vip2K/Main_Rom_File", conf[VIP2K].rom_[MAINROM1]);
    configPointer->Write("/Vip2K/Sequencer_Rom_File", conf[VIP2K].rom_[MAINROM2]);
	configPointer->Write("/Vip2K/Vt_Font_Rom_File", conf[VIP2K].vtCharRom_);
	configPointer->Write("/Vip2K/Ram_Software", conf[VIP2K].ram_);
	configPointer->Write("/Vip2K/Chip_8_Software", conf[VIP2K].chip8SW_);
	configPointer->Write("/Vip2K/Print_File", conf[VIP2K].printFile_);
	configPointer->Write("/Vip2K/Video_Dump_File", conf[VIP2K].screenDumpFile_);
	configPointer->Write("/Vip2K/Vt_Wav_File", elfConfiguration[VIP2K].vtWavFile_);
    configPointer->Write("/Vip2K/VtSerialPortChoice", elfConfiguration[VIP2K].serialPort_);

	configPointer->Write("/Vip2K/VtEf", elfConfiguration[VIP2K].vtEf);
	configPointer->Write("/Vip2K/VtQ", elfConfiguration[VIP2K].vtQ);
	configPointer->Write("/Vip2K/Bell_Frequency", elfConfiguration[VIP2K].bellFrequency_);
    configPointer->Write("/Vip2K/Uart", elfConfiguration[VIP2K].useUart);
	configPointer->Write("/Vip2K/VT_Type", elfConfiguration[VIP2K].vtType);
    configPointer->Write("/Vip2K/ShowVtWindow", elfConfiguration[VIP2K].vtShow);

    long value = elfConfiguration[VIP2K].vt52SetUpFeature_.to_ulong();
    configPointer->Write("/Vip2K/VT52Setup", value);
    value = elfConfiguration[VIP2K].vt100SetUpFeature_.to_ulong();
    configPointer->Write("/Vip2K/VT100Setup", value);
    value = elfConfiguration[VIP2K].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("/Vip2K/VTExternalSetup", value);

	configPointer->Write("/Vip2K/Vt_Baud", elfConfiguration[VIP2K].baudT);

	configPointer->Write("/Vip2K/Zoom", conf[VIP2K].zoom_);
	configPointer->Write("/Vip2K/Vt_Zoom", conf[VIP2K].zoomVt_);
	configPointer->Write("/Vip2K/Enable_Vt_Stretch_Dot", conf[VIP2K].stretchDot_);
    configPointer->Write("/Vip2K/Enable_Vt_External", elfConfiguration[VIP2K].vtExternal);

	configPointer->Write("/Vip2K/Enable_Turbo_Cassette", conf[VIP2K].turbo_);
	configPointer->Write("/Vip2K/Turbo_Clock_Speed", conf[VIP2K].turboClock_);
	configPointer->Write("/Vip2K/Enable_Auto_Cassette", conf[VIP2K].autoCassetteLoad_);
	configPointer->Write("/Vip2K/Enable_Real_Cassette", conf[VIP2K].realCassetteLoad_);
	configPointer->Write("/Vip2K/Volume", conf[VIP2K].volume_);

	configPointer->Write("/Vip2K/Clock_Speed", conf[VIP2K].clock_);
}

void GuiVip2K::readVip2KWindowConfig()
{
    conf[VIP2K].mainX_ = (int)configPointer->Read("/Vip2K/VipX", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[VIP2K].mainY_ = (int)configPointer->Read("/Vip2K/VipY", mainWindowY_);
    conf[VIP2K].vtX_ = (int)configPointer->Read("/Vip2K/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[VIP2K].vtY_ = (int)configPointer->Read("/Vip2K/Window_Position_Vt_Y", mainWindowY_);
}

void GuiVip2K::writeVip2KWindowConfig()
{
    if (conf[VIP2K].mainX_ > 0)
        configPointer->Write("/Vip2K/VipX", conf[VIP2K].mainX_);
    if (conf[VIP2K].mainY_ > 0)
        configPointer->Write("/Vip2K/VipY", conf[VIP2K].mainY_);
    if (conf[VIP2K].vtX_ > 0)
        configPointer->Write("/Vip2K/Window_Position_Vt_X", conf[VIP2K].vtX_);
    if (conf[VIP2K].vtY_ > 0)
        configPointer->Write("/Vip2K/Window_Position_Vt_Y", conf[VIP2K].vtY_);
}

void GuiVip2K::onVipBaudR(wxCommandEvent&event)
{
	elfConfiguration[VIP2K].baudR = event.GetSelection();
}

void GuiVip2K::onVipBaudT(wxCommandEvent&event)
{
	elfConfiguration[VIP2K].baudT = event.GetSelection();
	if (!elfConfiguration[VIP2K].useUart)
	{
		elfConfiguration[VIP2K].baudR = event.GetSelection();
		XRCCTRL(*this, "VTBaudRChoiceVip2K", wxChoice)->SetSelection(elfConfiguration[VIP2K].baudR);
	}
}

void GuiVip2K::onVipUart(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this, "VTBaudRChoiceVip2K", wxChoice)->SetSelection(elfConfiguration[VIP2K].baudR);
    XRCCTRL(*this, "VTBaudTChoiceVip2K", wxChoice)->SetSelection(elfConfiguration[VIP2K].baudT);
}

void GuiVip2K::onVtShow(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].vtShow = event.IsChecked();
}



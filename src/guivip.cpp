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
#include "guivip.h"
#include "pixie.h"

#include "psave.h"

BEGIN_EVENT_TABLE(GuiVip, GuiVipII)

	EVT_TEXT(XRCID("MainRomVip"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomVip"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonVip"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("RamSWVip"), GuiVip::onRamSWText)
	EVT_COMBOBOX(XRCID("RamSWVip"), GuiVip::onRamSWText)
	EVT_BUTTON(XRCID("RamSWButtonVip"), GuiVip::onRamSWVip)

	EVT_TEXT(XRCID("Chip8SWVip"), GuiVip::onChip8SWText)
	EVT_BUTTON(XRCID("Chip8SWButtonVip"), GuiMain::onChip8SW)
	EVT_BUTTON(XRCID("EjectChip8SWVip"), GuiMain::onEjectChip8SW)

	EVT_SPIN_UP(XRCID("ZoomSpinVip"), GuiMain::onZoomUp)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVip"), GuiMain::onZoomDown)
	EVT_TEXT(XRCID("ZoomValueVip"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3Vip"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("CasButtonVip"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasVip"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileVip"), GuiMain::onCassetteText)
	EVT_BUTTON(XRCID("ScreenDumpFileButtonVip"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileVip"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileVip"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5Vip"), GuiMain::onScreenDump)
	EVT_BUTTON(XRCID("RealCasLoadVip"), GuiMain::onRealCas)
	EVT_BUTTON(XRCID("CasLoadVip"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSaveVip"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopVip"), GuiMain::onCassetteStop)
	EVT_CHECKBOX(XRCID("TurboVip"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockVip"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadVip"), GuiMain::onAutoLoad)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeVip"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeVip"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("TempoVip"), GuiVip::onTempo)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("TempoVip"), GuiVip::onTempo)
	EVT_CHOICE(XRCID("SoundVip"), GuiVip::onSound)
	EVT_BUTTON(XRCID("KeyMapVip"), Main::onHexKeyDef)
	EVT_CHECKBOX(XRCID("KeyboardVip"), GuiMain::onKeyboard)
	EVT_BUTTON(XRCID("ColoursVip"), Main::onColoursDef)
	EVT_TEXT(XRCID("PrintFileVip"), GuiMain::onPrintFileText)
	EVT_BUTTON(XRCID("PrintButtonVip"), GuiMain::onPrintButton)
	EVT_CHOICE(XRCID("PrintModeVip"), GuiMain::onPrintMode)
	EVT_BUTTON(XRCID("PrintFileButtonVip"), GuiMain::onPrintFile)
	EVT_COMMAND(wxID_ANY, OPEN_PRINTER_WINDOW, GuiMain::openPrinterFrame) 
	EVT_TEXT(XRCID("BeepFrequencyVip"), GuiMain::onBeepFrequency)
	EVT_SPINCTRL(XRCID("RamVip"), GuiVip::onVipRam)
	EVT_SPINCTRL(XRCID("VP570"), GuiVip::onVipVp570)

	EVT_CHECKBOX(XRCID("UseLocationVip"), GuiMain::onUseLocation)
	EVT_TEXT(XRCID("SaveStartVip"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndVip"), GuiMain::onSaveEnd)
	EVT_TEXT(XRCID("SaveExecVip"), GuiMain::onSaveExec)

	EVT_BUTTON(XRCID("SaveButtonVip"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonVip"), GuiMain::onLoadButton)
	EVT_BUTTON(XRCID("RunButtonVip"), GuiMain::onLoadRunButton)
	EVT_BUTTON(XRCID("DsaveButtonVip"), GuiMain::onDataSaveButton)

	EVT_CHECKBOX(XRCID("HighResVip"), GuiVip::onVipHighRes)
	EVT_CHECKBOX(XRCID("VP580"), GuiVip::onVipVp580)
	EVT_CHECKBOX(XRCID("VP590"), GuiVip::onVipVp590)
	EVT_CHECKBOX(XRCID("StereoVip"), GuiVip::onVipStereo)

	EVT_CHECKBOX(XRCID("LatchVip"), GuiVip::onLatch)

	EVT_TEXT(XRCID("VtCharRomVip"), GuiMain::onVtCharRomText)
	EVT_COMBOBOX(XRCID("VtCharRomVip"), GuiMain::onVtCharRomText)
	EVT_BUTTON(XRCID("VtCharRomButtonVip"), GuiMain::onVtCharRom)

	EVT_CHOICE(XRCID("VTTypeVip"), GuiMain::onVT100)
	EVT_SPIN_UP(XRCID("ZoomSpinVtVip"), GuiMain::onZoomUpVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtVip"), GuiMain::onZoomDownVt)
	EVT_TEXT(XRCID("ZoomValueVtVip"), GuiMain::onZoomValueVt)
	EVT_BUTTON(XRCID("VtSetupVip"), GuiMain::onVtSetup)
	EVT_CHECKBOX(XRCID("StretchDotVip"), GuiMain::onStretchDot)

	EVT_CHOICE(XRCID("VTBaudTChoiceVip"), GuiVip::onVipBaudT)
	EVT_CHOICE(XRCID("VTBaudRChoiceVip"), GuiVip::onVipBaudR)

END_EVENT_TABLE()

GuiVip::GuiVip(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiVipII(title, pos, size, mode, dataDir, iniDir)
{
	conf[VIP].loadFileNameFull_ = "";
	conf[VIP].loadFileName_ = "";

	conf[VIP].pLoadSaveName_[0] = 'F';
	conf[VIP].pLoadSaveName_[1] = 'P';
	conf[VIP].pLoadSaveName_[2] = 'B';
	conf[VIP].pLoadSaveName_[3] = ' ';

	conf[VIP].saveStartString_ = "";
	conf[VIP].saveEndString_ = "";
	conf[VIP].saveExecString_ = "";
    
    position_.x = 0;
}

void GuiVip::readVipConfig()
{
	selectedComputer_ = VIP;

    conf[VIP].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Vip" + pathSeparator_;
    conf[VIP].mainDir_ = readConfigDir("/Dir/Vip/Main", dataDir_ + "Vip" + pathSeparator_);
    
    conf[VIP].romDir_[MAINROM1] = readConfigDir("/Dir/Vip/Main_Rom_File", dataDir_ + "Vip"  + pathSeparator_);
	conf[VIP].ramDir_ = readConfigDir("/Dir/Vip/Software_File", dataDir_ + "Vip"  + pathSeparator_);
	conf[VIP].vtCharRomDir_ = readConfigDir("/Dir/Vip/Vt_Font_Rom_File", dataDir_ + "Vip" + pathSeparator_);
	conf[VIP].chip8SWDir_ = readConfigDir("/Dir/Vip/Chip_8_Software", dataDir_ + "Chip-8"  + pathSeparator_ + "Chip-8 Games"  + pathSeparator_);
	conf[VIP].printFileDir_ = readConfigDir("/Dir/Vip/Print_File", dataDir_ + "Vip" + pathSeparator_);
	conf[VIP].screenDumpFileDir_ = readConfigDir("/Dir/Vip/Video_Dump_File", dataDir_ + "Vip" + pathSeparator_);
	conf[VIP].wavFileDir_ = readConfigDir("/Dir/Vip/Wav_File", dataDir_ + "Vip" + pathSeparator_);
	elfConfiguration[VIP].vtWavFileDir_ = readConfigDir("/Dir/Vip/Vt_Wav_File", dataDir_ + "Vip" + pathSeparator_);

	conf[VIP].rom_[MAINROM1] = configPointer->Read("/Vip/Main_Rom_File", "vip.rom");
	conf[VIP].ram_ = configPointer->Read("/Vip/Ram_Software", "fpb_color.bin");
	conf[VIP].chip8SW_ = configPointer->Read("/Vip/Chip_8_Software", "");
	conf[VIP].wavFile_ = configPointer->Read("/Vip/Wav_File", "");
	elfConfiguration[VIP].vtWavFile_ = configPointer->Read("/Vip/Vt_Wav_File", "");
    elfConfiguration[VIP].serialPort_ = configPointer->Read("/Vip/VtSerialPortChoice", "");

	conf[VIP].printFile_ = configPointer->Read("/Vip/Print_File", "printerout.txt");
	conf[VIP].screenDumpFile_ = configPointer->Read("/Vip/Video_Dump_File", "screendump.png");
	conf[VIP].turboClock_ = configPointer->Read("/Vip/Turbo_Clock_Speed", "15");
	conf[VIP].useLoadLocation_ = false;

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[VIP].zoom_ = configPointer->Read("/Vip/Zoom", defaultZoom);
	defaultZoom.Printf("%2.2f", 1.0);
	conf[VIP].zoomVt_ = configPointer->Read("/Vip/Vt_Zoom", defaultZoom);
	wxString defaultScale;
	defaultScale.Printf("%i", 3);
	conf[VIP].xScale_ = configPointer->Read("/Vip/Window_Scale_Factor_X", defaultScale);

	configPointer->Read("/Vip/Enable_Vt_Stretch_Dot", &conf[VIP].stretchDot_, false);
    configPointer->Read("/Vip/Enable_Vt_External", &elfConfiguration[VIP].vtExternal, false);
	configPointer->Read("/Vip/Enable_Turbo_Cassette", &conf[VIP].turbo_, true);
	configPointer->Read("/Vip/Enable_Auto_Cassette", &conf[VIP].autoCassetteLoad_, true);
	configPointer->Read("/Vip/Enable_Real_Cassette", &conf[VIP].realCassetteLoad_, false);
	configPointer->Read("/Vip/Printer", &conf[VIP].printerOn_, false);
	configPointer->Read("/Vip/Use_Keyboard", &conf[VIP].useKeyboard_, true);

	configPointer->Read("/Vip/Chip_10_High_Resolution", &highRes_, false);
	configPointer->Read("/Vip/Enable_VP590", &vipVp590_, true);
	configPointer->Read("/Vip/Enable_VP580", &vipVp580_, false);
	configPointer->Read("/Vip/Stereo", &stereo_, true);
	configPointer->Read("/Vip/Latch", &latch_, false);

	vipVp570_ = (int)configPointer->Read("/Vip/Enable_VP570", 7l);

	conf[VIP].ramType_ = (int)configPointer->Read("/Vip/Ram_Type", 4l);
	conf[VIP].soundType_ = (int)configPointer->Read("/Vip/Sound_Type", 0l);
	conf[VIP].volume_ = (int)configPointer->Read("/Vip/Volume", 25l);
	conf[VIP].tempo_ = (int)configPointer->Read("/Vip/Tempo", 100l);
	conf[VIP].printMode_ = (int)configPointer->Read("/Vip/Print_Mode", 1l);
	conf[VIP].beepFrequency_ = (int)configPointer->Read("/Vip/Beep_Frequency", 650);

	setRealCas(VIP);

	wxString defaultClock;
	defaultClock.Printf("%1.4f", 1.7609);
	conf[VIP].clock_ = configPointer->Read("/Vip/Clock_Speed", defaultClock);

	configPointer->Read("/Vip/VtEf", &elfConfiguration[VIP].vtEf, false);
	configPointer->Read("/Vip/VtQ", &elfConfiguration[VIP].vtQ, true);
	elfConfiguration[VIP].bellFrequency_ = (int)configPointer->Read("/Vip/Bell_Frequency", 800);

    elfConfiguration[VIP].useUart = false;
	elfConfiguration[VIP].vtType = (int)configPointer->Read("/Vip/VT_Type", 0l);
    elfConfiguration[VIP].vt52SetUpFeature_ = configPointer->Read("/Vip/VT52Setup", 0x00004092l);
    elfConfiguration[VIP].vt100SetUpFeature_ = configPointer->Read("/Vip/VT100Setup", 0x0000ca52l);
    elfConfiguration[VIP].vtExternalSetUpFeature_ = configPointer->Read("/Vip/VTExternalSetup", 0x0000ca52l);
	elfConfiguration[VIP].baudT = (int)configPointer->Read("/Vip/Vt_Baud", 5l);
	elfConfiguration[VIP].baudR = elfConfiguration[VIP].baudT;

	if (mode_.gui)
		setBaudChoiceVip();

	setVtType("Vip", VIP, elfConfiguration[VIP].vtType, false);

	conf[VIP].vtCharRom_ = configPointer->Read("/Vip/Vt_Font_Rom_File", "vt52.a.bin");

	if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomVip", wxComboBox)->SetValue(conf[VIP].rom_[MAINROM1]);
		XRCCTRL(*this, "RamSWVip", wxComboBox)->SetValue(conf[VIP].ram_);
		XRCCTRL(*this, "Chip8SWVip", wxTextCtrl)->SetValue(conf[VIP].chip8SW_);
		XRCCTRL(*this, "PrintFileVip", wxTextCtrl)->SetValue(conf[VIP].printFile_);
		XRCCTRL(*this, "VtCharRomVip", wxComboBox)->SetValue(conf[VIP].vtCharRom_);
		XRCCTRL(*this, "ScreenDumpFileVip", wxComboBox)->SetValue(conf[VIP].screenDumpFile_);
		XRCCTRL(*this, "WavFileVip", wxTextCtrl)->SetValue(conf[VIP].wavFile_);

		XRCCTRL(*this, "VTTypeVip", wxChoice)->SetSelection(elfConfiguration[VIP].vtType);

		XRCCTRL(*this, "VTBaudTChoiceVip", wxChoice)->SetSelection(elfConfiguration[VIP].baudT);
		XRCCTRL(*this, "VTBaudRChoiceVip", wxChoice)->SetSelection(elfConfiguration[VIP].baudT);
//		XRCCTRL(*this, "VTBaudTTextVip", wxStaticText)->Enable(elfConfiguration[VIP].vtType != VTNONE);
//		XRCCTRL(*this, "VTBaudTChoiceVip", wxChoice)->Enable(elfConfiguration[VIP].vtType != VTNONE);

//		XRCCTRL(*this, "VtCharRomButtonVip", wxButton)->Enable(elfConfiguration[VIP].vtType != VTNONE);
//		XRCCTRL(*this, "VtCharRomVip", wxComboBox)->Enable(elfConfiguration[VIP].vtType != VTNONE);
//		XRCCTRL(*this, "VtSetupVip", wxButton)->Enable(elfConfiguration[VIP].vtType != VTNONE);
		XRCCTRL(*this, "ZoomValueVtVip", wxTextCtrl)->ChangeValue(conf[VIP].zoomVt_);

		XRCCTRL(*this, "ZoomValueVip", wxTextCtrl)->ChangeValue(conf[VIP].zoom_);
		XRCCTRL(*this, "HighResVip", wxCheckBox)->SetValue(highRes_);
		XRCCTRL(*this, "VP590", wxCheckBox)->SetValue(vipVp590_);
		XRCCTRL(*this, "VP580", wxCheckBox)->SetValue(vipVp580_);
		XRCCTRL(*this, "SoundVip", wxChoice)->SetSelection(conf[VIP].soundType_);
		setSoundGui(conf[VIP].soundType_);
		XRCCTRL(*this, "StereoVip", wxCheckBox)->SetValue(stereo_);
		XRCCTRL(*this, "LatchVip", wxCheckBox)->SetValue(latch_);
		XRCCTRL(*this, "KeyboardVip", wxCheckBox)->SetValue(conf[VIP].useKeyboard_);
		XRCCTRL(*this, "TurboVip", wxCheckBox)->SetValue(conf[VIP].turbo_);
		turboGui("Vip");
		XRCCTRL(*this, "TurboClockVip", wxTextCtrl)->SetValue(conf[VIP].turboClock_);
		XRCCTRL(*this, "AutoCasLoadVip", wxCheckBox)->SetValue(conf[VIP].autoCassetteLoad_);
		setPrinterState(VIP);
		XRCCTRL(*this, "VolumeVip", wxSlider)->SetValue(conf[VIP].volume_);
		XRCCTRL(*this, "TempoVip", wxSlider)->SetValue(conf[VIP].tempo_);
		XRCCTRL(*this, "RamVip", wxSpinCtrl)->SetValue(conf[VIP].ramType_);
		XRCCTRL(*this, "VP570", wxSpinCtrl)->SetValue(vipVp570_);
		XRCCTRL(*this, "PrintModeVip", wxChoice)->SetSelection((int)configPointer->Read("/Vip/Print_Mode", 1l));
		setPrintMode();
		clockTextCtrl[VIP]->ChangeValue(conf[VIP].clock_);
		wxString beepFrequency;
		beepFrequency.Printf("%d", conf[VIP].beepFrequency_);
		XRCCTRL(*this, "BeepFrequencyVip", wxTextCtrl)->ChangeValue(beepFrequency);
	}
}

void GuiVip::writeVipDirConfig()
{
    writeConfigDir("/Dir/Vip/Main", conf[VIP].mainDir_);
    writeConfigDir("/Dir/Vip/Main_Rom_File", conf[VIP].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Vip/Software_File", conf[VIP].ramDir_);
	writeConfigDir("/Dir/Vip/Chip_8_Software", conf[VIP].chip8SWDir_);
	writeConfigDir("/Dir/Vip/Print_File", conf[VIP].printFileDir_);
	writeConfigDir("/Dir/Vip/Vt_Font_Rom_File", conf[VIP].vtCharRomDir_);
	writeConfigDir("/Dir/Vip/Video_Dump_File", conf[VIP].screenDumpFileDir_);
	writeConfigDir("/Dir/Vip/Wav_File", conf[VIP].wavFileDir_);
	writeConfigDir("/Dir/Vip/Vt_Wav_File", elfConfiguration[VIP].vtWavFileDir_);
}

void GuiVip::writeVipConfig()
{
    configPointer->Write("/Vip/Main_Rom_File", conf[VIP].rom_[MAINROM1]);
	configPointer->Write("/Vip/Vt_Font_Rom_File", conf[VIP].vtCharRom_);
	configPointer->Write("/Vip/Ram_Software", conf[VIP].ram_);
	configPointer->Write("/Vip/Chip_8_Software", conf[VIP].chip8SW_);
	configPointer->Write("/Vip/Print_File", conf[VIP].printFile_);
	configPointer->Write("/Vip/Video_Dump_File", conf[VIP].screenDumpFile_);
	configPointer->Write("/Vip/Wav_File", conf[VIP].wavFile_);
	configPointer->Write("/Vip/Vt_Wav_File", elfConfiguration[VIP].vtWavFile_);
    configPointer->Write("/Vip/VtSerialPortChoice", elfConfiguration[VIP].serialPort_);

	configPointer->Write("/Vip/VtEf", elfConfiguration[VIP].vtEf);
	configPointer->Write("/Vip/VtQ", elfConfiguration[VIP].vtQ);
	configPointer->Write("/Vip/Bell_Frequency", elfConfiguration[VIP].bellFrequency_);
	configPointer->Write("/Vip/VT_Type", elfConfiguration[VIP].vtType);

    long value = elfConfiguration[VIP].vt52SetUpFeature_.to_ulong();
    configPointer->Write("/Vip/VT52Setup", value);
    value = elfConfiguration[VIP].vt100SetUpFeature_.to_ulong();
    configPointer->Write("/Vip/VT100Setup", value);
    value = elfConfiguration[VIP].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("/Vip/VTExternalSetup", value);

	configPointer->Write("/Vip/Vt_Baud", elfConfiguration[VIP].baudT);

	configPointer->Write("/Vip/Zoom", conf[VIP].zoom_);
	configPointer->Write("/Vip/Vt_Zoom", conf[VIP].zoomVt_);
	configPointer->Write("/Vip/Enable_Vt_Stretch_Dot", conf[VIP].stretchDot_);
    configPointer->Write("/Vip/Enable_Vt_External", elfConfiguration[VIP].vtExternal);

	configPointer->Write("/Vip/Chip_10_High_Resolution", highRes_);
	configPointer->Write("/Vip/Enable_VP570", vipVp570_);
	configPointer->Write("/Vip/Enable_VP580", vipVp580_);
	configPointer->Write("/Vip/Enable_VP590", vipVp590_);
	configPointer->Write("/Vip/Sound_Type", conf[VIP].soundType_);
	configPointer->Write("/Vip/Stereo", stereo_);
	configPointer->Write("/Vip/Latch", latch_);
	configPointer->Write("/Vip/Use_Keyboard", conf[VIP].useKeyboard_);

	configPointer->Write("/Vip/Enable_Turbo_Cassette", conf[VIP].turbo_);
	configPointer->Write("/Vip/Turbo_Clock_Speed", conf[VIP].turboClock_);
	configPointer->Write("/Vip/Enable_Auto_Cassette", conf[VIP].autoCassetteLoad_);
	configPointer->Write("/Vip/Enable_Real_Cassette", conf[VIP].realCassetteLoad_);
	configPointer->Write("/Vip/Printer", conf[VIP].printerOn_);
	configPointer->Write("/Vip/Volume", conf[VIP].volume_);
	configPointer->Write("/Vip/Tempo", conf[VIP].tempo_);
	configPointer->Write("/Vip/Ram_Type", conf[VIP].ramType_);

	configPointer->Write("/Vip/Print_Mode", conf[VIP].printMode_);
	configPointer->Write("/Vip/Clock_Speed", conf[VIP].clock_);
	configPointer->Write("/Vip/Beep_Frequency", conf[VIP].beepFrequency_);
}

void GuiVip::readVipWindowConfig()
{
    conf[VIP].mainX_ = (int)configPointer->Read("/Vip/VipX", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[VIP].mainY_ = (int)configPointer->Read("/Vip/VipY", mainWindowY_);
    conf[VIP].vtX_ = (int)configPointer->Read("/Vip/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[VIP].vtY_ = (int)configPointer->Read("/Vip/Window_Position_Vt_Y", mainWindowY_);
}

void GuiVip::writeVipWindowConfig()
{
    if (conf[VIP].mainX_ > 0)
        configPointer->Write("/Vip/VipX", conf[VIP].mainX_);
    if (conf[VIP].mainY_ > 0)
        configPointer->Write("/Vip/VipY", conf[VIP].mainY_);
    if (conf[VIP].vtX_ > 0)
        configPointer->Write("/Vip/Window_Position_Vt_X", conf[VIP].vtX_);
    if (conf[VIP].vtY_ > 0)
        configPointer->Write("/Vip/Window_Position_Vt_Y", conf[VIP].vtY_);
}

void GuiVip::onTempo(wxScrollEvent&event)
{
	conf[selectedComputer_].tempo_ = event.GetPosition();
	if (computerRunning_)
		p_Computer->setTempo(conf[selectedComputer_].tempo_);
}

void GuiVip::onSound(wxCommandEvent&event)
{
	setSoundGui(event.GetSelection());
}

void GuiVip::onRamSWVip(wxCommandEvent&event)
{
	onRamSW(event);
	XRCCTRL(*this,"HighResVip", wxCheckBox)->SetValue(XRCCTRL(*this,"RamSWVip", wxComboBox)->GetValue() == "chip10.hex");
}

void GuiVip::onRamSWText(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].ram_ = XRCCTRL(*this, "RamSWVip", wxComboBox)->GetValue();
	XRCCTRL(*this,"HighResVip", wxCheckBox)->SetValue(XRCCTRL(*this,"RamSWVip", wxComboBox)->GetValue() == "chip10.hex");
}

void GuiVip::setSoundGui(int sound)
{
	conf[VIP].soundType_ = sound;
    XRCCTRL(*this, "BeepFrequencyTextVip", wxStaticText)->Enable(conf[VIP].soundType_ == VIP_BEEP);
    XRCCTRL(*this, "BeepFrequencyTextHzVip", wxStaticText)->Enable(conf[VIP].soundType_ == VIP_BEEP);
    XRCCTRL(*this, "BeepFrequencyVip", wxTextCtrl)->Enable(conf[VIP].soundType_ == VIP_BEEP);

	if (conf[VIP].soundType_ == VIP_SUPER2 || conf[VIP].soundType_ == VIP_SUPER4)
	{
		XRCCTRL(*this,"TempoTextVip", wxStaticText)->Enable(true);
        XRCCTRL(*this,"TempoVip", wxSlider)->Enable(true);
		XRCCTRL(*this,"StereoVip", wxCheckBox)->Enable(true);
	}
	else
	{
		XRCCTRL(*this,"TempoTextVip", wxStaticText)->Enable(false);
        XRCCTRL(*this,"TempoVip", wxSlider)->Enable(false);
		XRCCTRL(*this,"StereoVip", wxCheckBox)->Enable(false);
	}
}

void GuiVip::onVipRam(wxSpinEvent&event)
{
	conf[VIP].ramType_ = event.GetPosition();
}

void GuiVip::onVipVp570(wxSpinEvent&event)
{
	vipVp570_ = event.GetPosition();
}

void GuiVip::onVipVp580(wxCommandEvent&event)
{
	vipVp580_ = event.IsChecked();
}

void GuiVip::onVipVp590(wxCommandEvent&event)
{
	vipVp590_ = event.IsChecked();
}

void GuiVip::onVipHighRes(wxCommandEvent&event)
{
	highRes_ = event.IsChecked();
}

void GuiVip::onVipStereo(wxCommandEvent&event)
{
	stereo_ = event.IsChecked();
}

void GuiVip::onLatch(wxCommandEvent&event)
{
	latch_ = event.IsChecked();
}

void GuiVip::setBaudChoiceVip()
{
	wxArrayString choices;
	choices.Add("2400");
	choices.Add("2000");
	choices.Add("1800");
	choices.Add("1200");
	choices.Add("600");
	choices.Add("300");

	XRCCTRL(*this, "VTBaudRTextVip", wxStaticText)->Enable(false);
	XRCCTRL(*this, "VTBaudRChoiceVip", wxChoice)->Enable(false);
	XRCCTRL(*this, "VTBaudTChoiceVip", wxChoice)->Set(choices);
	XRCCTRL(*this, "VTBaudRChoiceVip", wxChoice)->Set(choices);
}

void GuiVip::onVipBaudR(wxCommandEvent&event)
{
	elfConfiguration[VIP].baudR = event.GetSelection();
}

void GuiVip::onVipBaudT(wxCommandEvent&event)
{
	elfConfiguration[VIP].baudT = event.GetSelection();
	if (!elfConfiguration[VIP].useUart)
	{
		elfConfiguration[VIP].baudR = event.GetSelection();
		XRCCTRL(*this, "VTBaudRChoiceVip", wxChoice)->SetSelection(elfConfiguration[VIP].baudR);
	}
}


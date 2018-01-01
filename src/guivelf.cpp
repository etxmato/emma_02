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
#include "guivelf.h"
#include "pixie.h"

#include "psave.h"

BEGIN_EVENT_TABLE(GuiVelf, GuiTMC2000)

	EVT_TEXT(XRCID("MainRomVelf"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomVelf"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonVelf"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("RamSWVelf"), GuiVelf::onRamSWText)
	EVT_COMBOBOX(XRCID("RamSWVelf"), GuiVelf::onRamSWText)
	EVT_BUTTON(XRCID("RamSWButtonVelf"), GuiVelf::onRamSWVelf)

	EVT_TEXT(XRCID("Chip8SWVelf"), GuiVelf::onChip8SWText)
	EVT_BUTTON(XRCID("Chip8SWButtonVelf"), GuiMain::onChip8SW)
	EVT_BUTTON(XRCID("EjectChip8SWVelf"), GuiMain::onEjectChip8SW)

	EVT_SPIN_UP(XRCID("ZoomSpinVelf"), GuiMain::onZoomUp)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVelf"), GuiMain::onZoomDown)
	EVT_TEXT(XRCID("ZoomValueVelf"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3Velf"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("CasButtonVelf"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasVelf"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileVelf"), GuiMain::onCassetteText)
	EVT_BUTTON(XRCID("ScreenDumpFileButtonVelf"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileVelf"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileVelf"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5Velf"), GuiMain::onScreenDump)
	EVT_BUTTON(XRCID("RealCasLoadVelf"), GuiMain::onRealCas)
	EVT_BUTTON(XRCID("CasLoadVelf"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSaveVelf"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopVelf"), GuiMain::onCassetteStop)
	EVT_CHECKBOX(XRCID("TurboVelf"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockVelf"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadVelf"), GuiMain::onAutoLoad)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeVelf"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeVelf"), GuiMain::onVolume)
	EVT_BUTTON(XRCID("KeyMapVelf"), Main::onHexKeyDef)
	EVT_BUTTON(XRCID("ColoursVelf"), Main::onColoursDef)
	EVT_TEXT(XRCID("PrintFileVelf"), GuiMain::onPrintFileText)
	EVT_BUTTON(XRCID("PrintButtonVelf"), GuiMain::onPrintButton)
	EVT_CHOICE(XRCID("PrintModeVelf"), GuiMain::onPrintMode)
	EVT_BUTTON(XRCID("PrintFileButtonVelf"), GuiMain::onPrintFile)
	EVT_COMMAND(wxID_ANY, OPEN_PRINTER_WINDOW, GuiMain::openPrinterFrame) 
	EVT_TEXT(XRCID("BeepFrequencyVelf"), GuiMain::onBeepFrequency)

	EVT_TEXT(XRCID("SaveStartVelf"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndVelf"), GuiMain::onSaveEnd)

	EVT_BUTTON(XRCID("SaveButtonVelf"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonVelf"), GuiMain::onLoadButton)

	EVT_CHECKBOX(XRCID("LatchVelf"), GuiVelf::onLatch)

	EVT_TEXT(XRCID("VtCharRomVelf"), GuiMain::onVtCharRomText)
	EVT_COMBOBOX(XRCID("VtCharRomVelf"), GuiMain::onVtCharRomText)
	EVT_BUTTON(XRCID("VtCharRomButtonVelf"), GuiMain::onVtCharRom)

    EVT_CHECKBOX(XRCID("VtExternalVelf"), GuiMain::onVtExternal)
	EVT_CHOICE(XRCID("VTTypeVelf"), GuiMain::onVT100)
	EVT_SPIN_UP(XRCID("ZoomSpinVtVelf"), GuiMain::onZoomUpVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtVelf"), GuiMain::onZoomDownVt)
	EVT_TEXT(XRCID("ZoomValueVtVelf"), GuiMain::onZoomValueVt)
	EVT_BUTTON(XRCID("VtSetupVelf"), GuiMain::onVtSetup)
	EVT_CHECKBOX(XRCID("StretchDotVelf"), GuiMain::onStretchDot)

    EVT_TEXT(XRCID("ShowAddressVelf"), GuiMain::onLedTimer)
    EVT_CHECKBOX(XRCID("ControlWindowsVelf"), GuiVelf::onVelfControlWindows)
    EVT_CHECKBOX(XRCID("AutoBootVelf"), GuiVelf::onAutoBoot)

	EVT_CHOICE(XRCID("VTBaudTChoiceVelf"), GuiVelf::onVelfBaudT)
	EVT_CHOICE(XRCID("VTBaudRChoiceVelf"), GuiVelf::onVelfBaudR)

END_EVENT_TABLE()

GuiVelf::GuiVelf(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiTMC2000(title, pos, size, mode, dataDir, iniDir)
{
	conf[VELF].loadFileNameFull_ = "";
	conf[VELF].loadFileName_ = "";

	conf[VELF].pLoadSaveName_[0] = 'F';
	conf[VELF].pLoadSaveName_[1] = 'P';
	conf[VELF].pLoadSaveName_[2] = 'B';
	conf[VELF].pLoadSaveName_[3] = ' ';

	conf[VELF].saveStartString_ = "";
	conf[VELF].saveEndString_ = "";
	conf[VELF].saveExecString_ = "";
    
    position_.x = 0;
}

void GuiVelf::readVelfConfig()
{
	selectedComputer_ = VELF;

    conf[VELF].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Velf" + pathSeparator_;
    conf[VELF].mainDir_ = readConfigDir("/Dir/Velf/Main", dataDir_ + "Velf" + pathSeparator_);
    
    conf[VELF].romDir_[MAINROM1] = readConfigDir("/Dir/Velf/Main_Rom_File", dataDir_ + "Velf"  + pathSeparator_);
	conf[VELF].ramDir_ = readConfigDir("/Dir/Velf/Software_File", dataDir_ + "Velf"  + pathSeparator_);
	conf[VELF].vtCharRomDir_ = readConfigDir("/Dir/Velf/Vt_Font_Rom_File", dataDir_ + "Velf" + pathSeparator_);
	conf[VELF].chip8SWDir_ = readConfigDir("/Dir/Velf/Chip_8_Software", dataDir_ + "Chip-8"  + pathSeparator_ + "Chip-8 Games"  + pathSeparator_);
	conf[VELF].printFileDir_ = readConfigDir("/Dir/Velf/Print_File", dataDir_ + "Velf" + pathSeparator_);
	conf[VELF].screenDumpFileDir_ = readConfigDir("/Dir/Velf/Video_Dump_File", dataDir_ + "Velf" + pathSeparator_);
	conf[VELF].wavFileDir_ = readConfigDir("/Dir/Velf/Wav_File", dataDir_ + "Velf" + pathSeparator_);
	elfConfiguration[VELF].vtWavFileDir_ = readConfigDir("/Dir/Velf/Vt_Wav_File", dataDir_ + "Velf" + pathSeparator_);

	conf[VELF].rom_[MAINROM1] = configPointer->Read("/Velf/Main_Rom_File", "VELFbios-v3.1.bin");
	conf[VELF].ram_ = configPointer->Read("/Velf/Ram_Software", "");
	conf[VELF].chip8SW_ = configPointer->Read("/Velf/Chip_8_Software", "");
	conf[VELF].wavFile_ = configPointer->Read("/Velf/Wav_File", "");
	elfConfiguration[VELF].vtWavFile_ = configPointer->Read("/Velf/Vt_Wav_File", "");

	conf[VELF].printFile_ = configPointer->Read("/Velf/Print_File", "printerout.txt");
	conf[VELF].screenDumpFile_ = configPointer->Read("/Velf/Video_Dump_File", "screendump.png");
	conf[VELF].turboClock_ = configPointer->Read("/Velf/Turbo_Clock_Speed", "15");
	conf[VELF].useLoadLocation_ = false;

    configPointer->Read("/Velf/Open_Control_Windows", &elfConfiguration[VELF].useElfControlWindows, true);
    configPointer->Read("/Velf/Enable_Vt_External", &elfConfiguration[VELF].vtExternal, false);

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[VELF].zoom_ = configPointer->Read("/Velf/Zoom", defaultZoom);
	defaultZoom.Printf("%2.2f", 1.0);
	conf[VELF].zoomVt_ = configPointer->Read("/Velf/Vt_Zoom", defaultZoom);
	wxString defaultScale;
	defaultScale.Printf("%i", 3);
	conf[VELF].xScale_ = configPointer->Read("/Velf/Window_Scale_Factor_X", defaultScale);

	configPointer->Read("/Velf/Enable_Vt_Stretch_Dot", &conf[VELF].stretchDot_, false);
	configPointer->Read("/Velf/Enable_Turbo_Cassette", &conf[VELF].turbo_, true);
	configPointer->Read("/Velf/Enable_Auto_Cassette", &conf[VELF].autoCassetteLoad_, true);
	configPointer->Read("/Velf/Enable_Real_Cassette", &conf[VELF].realCassetteLoad_, false);
	configPointer->Read("/Velf/Printer", &conf[VELF].printerOn_, false);

	configPointer->Read("/Velf/Latch", &latch_, false);

	conf[VELF].volume_ = (int)configPointer->Read("/Velf/Volume", 25l);
	conf[VELF].printMode_ = (int)configPointer->Read("/Velf/Print_Mode", 1l);
	conf[VELF].beepFrequency_ = (int)configPointer->Read("/Velf/Beep_Frequency", 650);

	setRealCas(VELF);

	wxString defaultClock;
	defaultClock.Printf("%1.4f", 3.579545);
	conf[VELF].clock_ = configPointer->Read("/Velf/Clock_Speed", defaultClock);

    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[VELF].ledTime_ = configPointer->Read("/Velf/Led_Update_Frequency", defaultTimer);
    
	configPointer->Read("/Velf/VtEf", &elfConfiguration[VELF].vtEf, true);
	configPointer->Read("/Velf/VtQ", &elfConfiguration[VELF].vtQ, false);
	elfConfiguration[VELF].bellFrequency_ = (int)configPointer->Read("/Velf/Bell_Frequency", 800);

    elfConfiguration[VELF].useUart = false;
	elfConfiguration[VELF].vtType = (int)configPointer->Read("/Velf/VT_Type", 2l);
    elfConfiguration[VELF].vt52SetUpFeature_ = configPointer->Read("/Velf/VT52Setup", 0x00004092l);
    elfConfiguration[VELF].vt100SetUpFeature_ = configPointer->Read("/Velf/VT100Setup", 0x0000ca52l);
	elfConfiguration[VELF].baudT = (int)configPointer->Read("/Velf/Vt_Baud", 0l);
	elfConfiguration[VELF].baudR = elfConfiguration[VELF].baudT;

	if (mode_.gui)
		setBaudChoiceVelf();

	setVtType("Velf", VELF, elfConfiguration[VELF].vtType);

	conf[VELF].vtCharRom_ = configPointer->Read("/Velf/Vt_Font_Rom_File", "vt100.bin");

    conf[VELF].velfMode_ = (int)configPointer->Read("/Velf/ModeVelf", 0l);
    configPointer->Read("/Velf/Enable_Auto_Boot", &elfConfiguration[VELF].autoBoot, true);

    if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomVelf", wxComboBox)->SetValue(conf[VELF].rom_[MAINROM1]);
		XRCCTRL(*this, "RamSWVelf", wxComboBox)->SetValue(conf[VELF].ram_);
		XRCCTRL(*this, "Chip8SWVelf", wxTextCtrl)->SetValue(conf[VELF].chip8SW_);
		XRCCTRL(*this, "PrintFileVelf", wxTextCtrl)->SetValue(conf[VELF].printFile_);
		XRCCTRL(*this, "VtCharRomVelf", wxComboBox)->SetValue(conf[VELF].vtCharRom_);
		XRCCTRL(*this, "ScreenDumpFileVelf", wxComboBox)->SetValue(conf[VELF].screenDumpFile_);
		XRCCTRL(*this, "WavFileVelf", wxTextCtrl)->SetValue(conf[VELF].wavFile_);

		XRCCTRL(*this, "VTTypeVelf", wxChoice)->SetSelection(elfConfiguration[VELF].vtType);

		XRCCTRL(*this, "VTBaudTChoiceVelf", wxChoice)->SetSelection(elfConfiguration[VELF].baudT);
		XRCCTRL(*this, "VTBaudRChoiceVelf", wxChoice)->SetSelection(elfConfiguration[VELF].baudT);
		XRCCTRL(*this, "VTBaudTTextVelf", wxStaticText)->Enable(elfConfiguration[VELF].vtType != VTNONE);
		XRCCTRL(*this, "VTBaudTChoiceVelf", wxChoice)->Enable(elfConfiguration[VELF].vtType != VTNONE);

        XRCCTRL(*this,"AddressText1Velf", wxStaticText)->Enable(elfConfiguration[VELF].useElfControlWindows);
        XRCCTRL(*this,"AddressText2Velf", wxStaticText)->Enable(elfConfiguration[VELF].useElfControlWindows);
        XRCCTRL(*this, "ControlWindowsVelf", wxCheckBox)->SetValue(elfConfiguration[VELF].useElfControlWindows);

		XRCCTRL(*this, "VtCharRomButtonVelf", wxButton)->Enable(elfConfiguration[VELF].vtType != VTNONE);
		XRCCTRL(*this, "VtCharRomVelf", wxComboBox)->Enable(elfConfiguration[VELF].vtType != VTNONE);
		XRCCTRL(*this, "VtSetupVelf", wxButton)->Enable(elfConfiguration[VELF].vtType != VTNONE);
        XRCCTRL(*this, "VtExternalVelf", wxCheckBox)->SetValue(elfConfiguration[VELF].vtExternal);
		XRCCTRL(*this, "ZoomValueVtVelf", wxTextCtrl)->ChangeValue(conf[VELF].zoomVt_);

		XRCCTRL(*this, "ZoomValueVelf", wxTextCtrl)->ChangeValue(conf[VELF].zoom_);
		XRCCTRL(*this, "LatchVelf", wxCheckBox)->SetValue(latch_);
		XRCCTRL(*this, "TurboVelf", wxCheckBox)->SetValue(conf[VELF].turbo_);
		turboGui("Velf");
		XRCCTRL(*this, "TurboClockVelf", wxTextCtrl)->SetValue(conf[VELF].turboClock_);
		XRCCTRL(*this, "AutoCasLoadVelf", wxCheckBox)->SetValue(conf[VELF].autoCassetteLoad_);
		setPrinterState(VELF);
		XRCCTRL(*this, "VolumeVelf", wxSlider)->SetValue(conf[VELF].volume_);
		XRCCTRL(*this, "PrintModeVelf", wxChoice)->SetSelection((int)configPointer->Read("/Velf/Print_Mode", 1l));
		setPrintMode();
		clockTextCtrl[VELF]->ChangeValue(conf[VELF].clock_);
		wxString beepFrequency;
		beepFrequency.Printf("%d", conf[VELF].beepFrequency_);
		XRCCTRL(*this, "BeepFrequencyVelf", wxTextCtrl)->ChangeValue(beepFrequency);
        XRCCTRL(*this, "ShowAddressVelf", wxTextCtrl)->ChangeValue(conf[VELF].ledTime_);
        XRCCTRL(*this,"ShowAddressVelf", wxTextCtrl)->Enable(elfConfiguration[VELF].useElfControlWindows);
        XRCCTRL(*this, "AutoBootVelf", wxCheckBox)->SetValue(elfConfiguration[VELF].autoBoot);
    }
}

void GuiVelf::writeVelfDirConfig()
{
    writeConfigDir("/Dir/Velf/Main", conf[VELF].mainDir_);
    writeConfigDir("/Dir/Velf/Main_Rom_File", conf[VELF].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Velf/Software_File", conf[VELF].ramDir_);
	writeConfigDir("/Dir/Velf/Chip_8_Software", conf[VELF].chip8SWDir_);
	writeConfigDir("/Dir/Velf/Print_File", conf[VELF].printFileDir_);
	writeConfigDir("/Dir/Velf/Vt_Font_Rom_File", conf[VELF].vtCharRomDir_);
	writeConfigDir("/Dir/Velf/Video_Dump_File", conf[VELF].screenDumpFileDir_);
	writeConfigDir("/Dir/Velf/Wav_File", conf[VELF].wavFileDir_);
	writeConfigDir("/Dir/Velf/Vt_Wav_File", elfConfiguration[VELF].vtWavFileDir_);
}

void GuiVelf::writeVelfConfig()
{
    configPointer->Write("/Velf/Main_Rom_File", conf[VELF].rom_[MAINROM1]);
	configPointer->Write("/Velf/Vt_Font_Rom_File", conf[VELF].vtCharRom_);
	configPointer->Write("/Velf/Ram_Software", conf[VELF].ram_);
	configPointer->Write("/Velf/Chip_8_Software", conf[VELF].chip8SW_);
	configPointer->Write("/Velf/Print_File", conf[VELF].printFile_);
	configPointer->Write("/Velf/Video_Dump_File", conf[VELF].screenDumpFile_);
	configPointer->Write("/Velf/Wav_File", conf[VELF].wavFile_);
	configPointer->Write("/Velf/Vt_Wav_File", elfConfiguration[VELF].vtWavFile_);

	configPointer->Write("/Velf/VtEf", elfConfiguration[VELF].vtEf);
	configPointer->Write("/Velf/VtQ", elfConfiguration[VELF].vtQ);
	configPointer->Write("/Velf/Bell_Frequency", elfConfiguration[VELF].bellFrequency_);
	configPointer->Write("/Velf/VT_Type", elfConfiguration[VELF].vtType);

    long value = elfConfiguration[VELF].vt52SetUpFeature_.to_ulong();
    configPointer->Write("/Velf/VT52Setup", value);
    value = elfConfiguration[VELF].vt100SetUpFeature_.to_ulong();
    configPointer->Write("/Velf/VT100Setup", value);

	configPointer->Write("/Velf/Vt_Baud", elfConfiguration[VELF].baudT);
    configPointer->Write("/Velf/Enable_Auto_Boot", elfConfiguration[VELF].autoBoot);

	configPointer->Write("/Velf/Zoom", conf[VELF].zoom_);
	configPointer->Write("/Velf/Vt_Zoom", conf[VELF].zoomVt_);
	configPointer->Write("/Velf/Enable_Vt_Stretch_Dot", conf[VELF].stretchDot_);
    configPointer->Write("/Velf/Enable_Vt_External", elfConfiguration[VELF].vtExternal);
    configPointer->Write("/Velf/Open_Control_Windows", elfConfiguration[VELF].useElfControlWindows);

	configPointer->Write("/Velf/Latch", latch_);

	configPointer->Write("/Velf/Enable_Turbo_Cassette", conf[VELF].turbo_);
	configPointer->Write("/Velf/Turbo_Clock_Speed", conf[VELF].turboClock_);
	configPointer->Write("/Velf/Enable_Auto_Cassette", conf[VELF].autoCassetteLoad_);
	configPointer->Write("/Velf/Enable_Real_Cassette", conf[VELF].realCassetteLoad_);
	configPointer->Write("/Velf/Printer", conf[VELF].printerOn_);
	configPointer->Write("/Velf/Volume", conf[VELF].volume_);
    configPointer->Write("/Velf/Led_Update_Frequency", conf[VELF].ledTime_);

	configPointer->Write("/Velf/Print_Mode", conf[VELF].printMode_);
	configPointer->Write("/Velf/Clock_Speed", conf[VELF].clock_);
	configPointer->Write("/Velf/Beep_Frequency", conf[VELF].beepFrequency_);
    configPointer->Write("/Velf/ModeVelf", conf[VELF].velfMode_);
}

void GuiVelf::readVelfWindowConfig()
{    
    conf[VELF].mainX_ = (int)configPointer->Read("/Velf/VelfX", mainWindowX_);
    conf[VELF].mainY_ = (int)configPointer->Read("/Velf/VelfY", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    conf[VELF].pixieX_ = (int)configPointer->Read("/Velf/VelfPixieX", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[VELF].pixieY_ = (int)configPointer->Read("/Velf/VelfPixieY", mainWindowY_);
    conf[VELF].vtX_ = (int)configPointer->Read("/Velf/Window_Position_Vt_X", mainWindowX_ + windowInfo.mainwX + windowInfo.xBorder);
    conf[VELF].vtY_ = (int)configPointer->Read("/Velf/Window_Position_Vt_Y", mainWindowY_ + 426 + windowInfo.yBorder);
}

void GuiVelf::writeVelfWindowConfig()
{
    if (conf[VELF].mainX_ > 0)
        configPointer->Write("/Velf/VelfX", conf[VELF].mainX_);
    if (conf[VELF].mainY_ > 0)
        configPointer->Write("/Velf/VelfY", conf[VELF].mainY_);
    if (conf[VELF].pixieX_ > 0)
        configPointer->Write("/Velf/VelfPixieX", conf[VELF].pixieX_);
    if (conf[VELF].pixieY_ > 0)
        configPointer->Write("/Velf/VelfPixieY", conf[VELF].pixieY_);
    if (conf[VELF].vtX_ > 0)
        configPointer->Write("/Velf/Window_Position_Vt_X", conf[VELF].vtX_);
    if (conf[VELF].vtY_ > 0)
        configPointer->Write("/Velf/Window_Position_Vt_Y", conf[VELF].vtY_);
}

void GuiVelf::onRamSWVelf(wxCommandEvent&event)
{
	onRamSW(event);
}

void GuiVelf::onRamSWText(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].ram_ = XRCCTRL(*this, "RamSWVelf", wxComboBox)->GetValue();
}

void GuiVelf::onLatch(wxCommandEvent&event)
{
	latch_ = event.IsChecked();
}

void GuiVelf::setBaudChoiceVelf()
{
	wxArrayString choices;
	choices.Add("9600");
	choices.Add("4800");
	choices.Add("3600");
	choices.Add("2400");
	choices.Add("2000");
	choices.Add("1800");
	choices.Add("1200");
	choices.Add("600");
	choices.Add("300");

	XRCCTRL(*this, "VTBaudRTextVelf", wxStaticText)->Enable(false);
	XRCCTRL(*this, "VTBaudRChoiceVelf", wxChoice)->Enable(false);
	XRCCTRL(*this, "VTBaudTChoiceVelf", wxChoice)->Set(choices);
	XRCCTRL(*this, "VTBaudRChoiceVelf", wxChoice)->Set(choices);
}

void GuiVelf::onVelfBaudR(wxCommandEvent&event)
{
	elfConfiguration[VELF].baudR = event.GetSelection();
}

void GuiVelf::onVelfBaudT(wxCommandEvent&event)
{
	elfConfiguration[VELF].baudT = event.GetSelection();
	if (!elfConfiguration[VELF].useUart)
	{
		elfConfiguration[VELF].baudR = event.GetSelection();
		XRCCTRL(*this, "VTBaudRChoiceVelf", wxChoice)->SetSelection(elfConfiguration[VELF].baudR);
	}
}

void GuiVelf::onVelfControlWindows(wxCommandEvent&event)
{
    elfConfiguration[VELF].useElfControlWindows = event.IsChecked();
    
    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddressVelf",wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText1Velf",wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText2Velf",wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
    }
    
    if (runningComputer_ == VELF)
        p_Velf->Show(elfConfiguration[VELF].useElfControlWindows);
}

bool GuiVelf::getUseVelfControlWindows()
{
    return elfConfiguration[VELF].useElfControlWindows;
}

int GuiVelf::getVelfMode()
{
    return conf[VELF].velfMode_;
}

void GuiVelf::switchVelfMode()
{
    if (conf[VELF].velfMode_ == 0)
        conf[VELF].velfMode_ = 1;
    else
        conf[VELF].velfMode_ = 0;
}

void GuiVelf::onAutoBoot(wxCommandEvent&event)
{
    elfConfiguration[VELF].autoBoot = event.IsChecked();
}



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
#include "guicdp18s020.h"
#include "pixie.h"

#include "psave.h"

BEGIN_EVENT_TABLE(GuiCdp18s020, GuiCdp18s600)

	EVT_TEXT(XRCID("MainRomCDP18S020"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomCDP18S020"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonCDP18S020"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("RamSWCDP18S020"), GuiCdp18s020::onRamSWText)
	EVT_COMBOBOX(XRCID("RamSWCDP18S020"), GuiCdp18s020::onRamSWText)
	EVT_BUTTON(XRCID("RamSWButtonCDP18S020"), GuiCdp18s020::onRamSWCdp18s020)

	EVT_SPIN_UP(XRCID("ZoomSpinCDP18S020"), GuiMain::onZoomUp)
	EVT_SPIN_DOWN(XRCID("ZoomSpinCDP18S020"), GuiMain::onZoomDown)
	EVT_TEXT(XRCID("ZoomValueCDP18S020"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3CDP18S020"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("ScreenDumpFileButtonCDP18S020"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileCDP18S020"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileCDP18S020"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5CDP18S020"), GuiMain::onScreenDump)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeCDP18S020"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeCDP18S020"), GuiMain::onVolume)
	EVT_BUTTON(XRCID("ColoursCDP18S020"), Main::onColoursDef)

	EVT_TEXT(XRCID("SaveStartCDP18S020"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndCDP18S020"), GuiMain::onSaveEnd)

	EVT_BUTTON(XRCID("SaveButtonCDP18S020"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonCDP18S020"), GuiMain::onLoadButton)

	EVT_CHECKBOX(XRCID("LatchCDP18S020"), GuiCdp18s020::onLatch)

	EVT_TEXT(XRCID("VtCharRomCDP18S020"), GuiMain::onVtCharRomText)
	EVT_COMBOBOX(XRCID("VtCharRomCDP18S020"), GuiMain::onVtCharRomText)
	EVT_BUTTON(XRCID("VtCharRomButtonCDP18S020"), GuiMain::onVtCharRom)

	EVT_CHOICE(XRCID("VTTypeCDP18S020"), GuiMain::onVT100)
	EVT_SPIN_UP(XRCID("ZoomSpinVtCDP18S020"), GuiMain::onZoomUpVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtCDP18S020"), GuiMain::onZoomDownVt)
	EVT_TEXT(XRCID("ZoomValueVtCDP18S020"), GuiMain::onZoomValueVt)
	EVT_BUTTON(XRCID("VtSetupCDP18S020"), GuiMain::onVtSetup)
	EVT_CHECKBOX(XRCID("StretchDotCDP18S020"), GuiMain::onStretchDot)

    EVT_TEXT(XRCID("ShowAddressCDP18S020"), GuiMain::onLedTimer)
    EVT_CHECKBOX(XRCID("ControlWindowsCDP18S020"), GuiCdp18s020::onCdp18s020ControlWindows)
    EVT_CHECKBOX(XRCID("AutoBootCDP18S020"), GuiCdp18s020::onAutoBoot)

	EVT_CHOICE(XRCID("VTBaudTChoiceCDP18S020"), GuiCdp18s020::onCdp18s020BaudT)
	EVT_CHOICE(XRCID("VTBaudRChoiceCDP18S020"), GuiCdp18s020::onCdp18s020BaudR)

    EVT_CHOICE(XRCID("RamCDP18S020"), GuiMain::onChoiceRam)

    EVT_BUTTON(XRCID("CasLoadCDP18S020"), GuiMain::onTerminalLoad)
    EVT_BUTTON(XRCID("CasSaveCDP18S020"), GuiMain::onTerminalSave)
    EVT_BUTTON(XRCID("CasStopCDP18S020"), GuiMain::onTerminalStop)
    EVT_BUTTON(XRCID("CasButtonCDP18S020"), GuiMain::onTerminalFile)
    EVT_BUTTON(XRCID("EjectCasCDP18S020"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileCDP18S020"), GuiMain::onCassetteText)

    EVT_CHOICE(XRCID("AutoBootTypeCDP18S020"), GuiCdp18s020::onAutoBootType)
    EVT_CHECKBOX(XRCID("ForceUCCDP18S020"), GuiCdp18s020::onCdp18s020ForceUpperCase)

END_EVENT_TABLE()

GuiCdp18s020::GuiCdp18s020(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiCdp18s600(title, pos, size, mode, dataDir, iniDir)
{
	conf[CDP18S020].loadFileNameFull_ = "";
	conf[CDP18S020].loadFileName_ = "";

	conf[CDP18S020].pLoadSaveName_[0] = 'F';
	conf[CDP18S020].pLoadSaveName_[1] = 'P';
	conf[CDP18S020].pLoadSaveName_[2] = 'B';
	conf[CDP18S020].pLoadSaveName_[3] = ' ';

	conf[CDP18S020].saveStartString_ = "";
	conf[CDP18S020].saveEndString_ = "";
	conf[CDP18S020].saveExecString_ = "";
    
    position_.x = 0;
}

void GuiCdp18s020::readCdp18s020Config()
{
	selectedComputer_ = CDP18S020;

    conf[CDP18S020].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "CDP18S020" + pathSeparator_;
    conf[CDP18S020].mainDir_ = readConfigDir("/Dir/CDP18S020/Main", dataDir_ + "CDP18S020" + pathSeparator_);
    
    conf[CDP18S020].romDir_[MAINROM1] = readConfigDir("/Dir/CDP18S020/Main_Rom_File", dataDir_ + "CDP18S020"  + pathSeparator_);
	conf[CDP18S020].ramDir_ = readConfigDir("/Dir/CDP18S020/Software_File", dataDir_ + "CDP18S020"  + pathSeparator_);
	conf[CDP18S020].vtCharRomDir_ = readConfigDir("/Dir/CDP18S020/Vt_Font_Rom_File", dataDir_ + "CDP18S020" + pathSeparator_);
	conf[CDP18S020].chip8SWDir_ = readConfigDir("/Dir/CDP18S020/Chip_8_Software", dataDir_ + "Chip-8"  + pathSeparator_ + "Chip-8 Games"  + pathSeparator_);
	conf[CDP18S020].printFileDir_ = readConfigDir("/Dir/CDP18S020/Print_File", dataDir_ + "CDP18S020" + pathSeparator_);
	conf[CDP18S020].screenDumpFileDir_ = readConfigDir("/Dir/CDP18S020/Video_Dump_File", dataDir_ + "CDP18S020" + pathSeparator_);
	conf[CDP18S020].wavFileDir_[0] = readConfigDir("/Dir/CDP18S020/Wav_File", dataDir_ + "CDP18S020" + pathSeparator_);
	elfConfiguration[CDP18S020].vtWavFileDir_ = readConfigDir("/Dir/CDP18S020/Vt_Wav_File", dataDir_ + "CDP18S020" + pathSeparator_);

	conf[CDP18S020].rom_[MAINROM1] = configPointer->Read("/CDP18S020/Main_Rom_File", "ut4.bin");
	conf[CDP18S020].ram_ = configPointer->Read("/CDP18S020/Ram_Software", "Cosmac Tiny Basic.bin");
    conf[CDP18S020].wavFile_[0] = configPointer->Read("/CDP18S020/Terminal_File", "Cosmac Tiny Basic.txt");
	elfConfiguration[CDP18S020].vtWavFile_ = configPointer->Read("/CDP18S020/Vt_Wav_File", "");
    elfConfiguration[CDP18S020].serialPort_ = configPointer->Read("/CDP18S020/VtSerialPortChoice", "");

	conf[CDP18S020].screenDumpFile_ = configPointer->Read("/CDP18S020/Video_Dump_File", "screendump.png");
	conf[CDP18S020].useLoadLocation_ = false;

    configPointer->Read("/CDP18S020/Open_Control_Windows", &elfConfiguration[CDP18S020].useElfControlWindows, true);
    configPointer->Read("/CDP18S020/Enable_Vt_External", &elfConfiguration[CDP18S020].vtExternal, false);

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[CDP18S020].zoom_ = configPointer->Read("/CDP18S020/Zoom", defaultZoom);
	defaultZoom.Printf("%2.2f", 1.0);
	conf[CDP18S020].zoomVt_ = configPointer->Read("/CDP18S020/Vt_Zoom", defaultZoom);
	wxString defaultScale;
	defaultScale.Printf("%i", 3);
	conf[CDP18S020].xScale_ = configPointer->Read("/CDP18S020/Window_Scale_Factor_X", defaultScale);

	configPointer->Read("/CDP18S020/Enable_Vt_Stretch_Dot", &conf[CDP18S020].stretchDot_, false);

	configPointer->Read("/CDP18S020/Latch", &latch_, false);

    configPointer->Read("/CDP18S020/Enable_Turbo_Cassette", &conf[CDP18S020].turbo_, true);
    configPointer->Read("/CDP18S020/Enable_Real_Cassette", &conf[CDP18S020].realCassetteLoad_, false);
    conf[CDP18S020].turboClock_ = configPointer->Read("/CDP18S020/Turbo_Clock_Speed", "15");

    conf[CDP18S020].autoCassetteLoad_ = false;
    
    setRealCas(CDP18S020);

    conf[CDP18S020].volume_ = (int)configPointer->Read("/CDP18S020/Volume", 25l);

	wxString defaultClock;
	defaultClock.Printf("%1.1f", 2.0);
	conf[CDP18S020].clock_ = configPointer->Read("/CDP18S020/Clock_Speed", defaultClock);

    wxString defaultTimer;
    defaultTimer.Printf("%d", 500);
    conf[CDP18S020].ledTime_ = configPointer->Read("/CDP18S020/Led_Update_Frequency", defaultTimer);
    
	elfConfiguration[CDP18S020].bellFrequency_ = (int)configPointer->Read("/CDP18S020/Bell_Frequency", 800);
    elfConfiguration[CDP18S020].useUart = false;
	elfConfiguration[CDP18S020].vtType = (int)configPointer->Read("/CDP18S020/VT_Type", 2l);
    elfConfiguration[CDP18S020].vt52SetUpFeature_ = configPointer->Read("/CDP18S020/VT52Setup", 0x00004092l);
    elfConfiguration[CDP18S020].vt100SetUpFeature_ = configPointer->Read("/CDP18S020/VT100Setup", 0x0000cad2l);
    elfConfiguration[CDP18S020].vtExternalSetUpFeature_ = configPointer->Read("/CDP18S020/VTExternalSetup", 0x0000cad2l);
	elfConfiguration[CDP18S020].baudT = (int)configPointer->Read("/CDP18S020/Vt_Baud", 10l);
	elfConfiguration[CDP18S020].baudR = elfConfiguration[CDP18S020].baudT;
    elfConfiguration[CDP18S020].vtEf = false;
    elfConfiguration[CDP18S020].vtQ = true;

	setVtType("CDP18S020", CDP18S020, elfConfiguration[CDP18S020].vtType, false);

	conf[CDP18S020].vtCharRom_ = configPointer->Read("/CDP18S020/Vt_Font_Rom_File", "vt100.bin");

    configPointer->Read("/CDP18S020/Enable_Auto_Boot", &elfConfiguration[CDP18S020].autoBoot, true);
    conf[CDP18S020].ramType_ = (int)configPointer->Read("/CDP18S020/Ram_Type", 4l);
    elfConfiguration[CDP18S020].autoBootType = (int)configPointer->Read("/CDP18S020/AutoBootType", 0l);
    if (elfConfiguration[CDP18S020].autoBootType == 0)
        conf[CDP18S020].bootAddress_ = 0x8000;
    else
        conf[CDP18S020].bootAddress_ = 0;

    configPointer->Read("/CDP18S020/Force_Uppercase", &elfConfiguration[CDP18S020].forceUpperCase, true);

    if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomCDP18S020", wxComboBox)->SetValue(conf[CDP18S020].rom_[MAINROM1]);
		XRCCTRL(*this, "RamSWCDP18S020", wxComboBox)->SetValue(conf[CDP18S020].ram_);
		XRCCTRL(*this, "VtCharRomCDP18S020", wxComboBox)->SetValue(conf[CDP18S020].vtCharRom_);
		XRCCTRL(*this, "ScreenDumpFileCDP18S020", wxComboBox)->SetValue(conf[CDP18S020].screenDumpFile_);
        XRCCTRL(*this, "WavFileCDP18S020", wxTextCtrl)->SetValue(conf[CDP18S020].wavFile_[0]);

		XRCCTRL(*this, "VTTypeCDP18S020", wxChoice)->SetSelection(elfConfiguration[CDP18S020].vtType);

		XRCCTRL(*this, "VTBaudTChoiceCDP18S020", wxChoice)->SetSelection(elfConfiguration[CDP18S020].baudT);
		XRCCTRL(*this, "VTBaudRChoiceCDP18S020", wxChoice)->SetSelection(elfConfiguration[CDP18S020].baudT);

        XRCCTRL(*this,"AddressText1CDP18S020", wxStaticText)->Enable(elfConfiguration[CDP18S020].useElfControlWindows);
        XRCCTRL(*this,"AddressText2CDP18S020", wxStaticText)->Enable(elfConfiguration[CDP18S020].useElfControlWindows);
        XRCCTRL(*this, "ControlWindowsCDP18S020", wxCheckBox)->SetValue(elfConfiguration[CDP18S020].useElfControlWindows);
        XRCCTRL(*this, "ZoomValueVtCDP18S020", wxTextCtrl)->ChangeValue(conf[CDP18S020].zoomVt_);

		XRCCTRL(*this, "ZoomValueCDP18S020", wxTextCtrl)->ChangeValue(conf[CDP18S020].zoom_);
		XRCCTRL(*this, "LatchCDP18S020", wxCheckBox)->SetValue(latch_);
		XRCCTRL(*this, "VolumeCDP18S020", wxSlider)->SetValue(conf[CDP18S020].volume_);
		clockTextCtrl[CDP18S020]->ChangeValue(conf[CDP18S020].clock_);
        XRCCTRL(*this, "ShowAddressCDP18S020", wxTextCtrl)->ChangeValue(conf[CDP18S020].ledTime_);
        XRCCTRL(*this,"ShowAddressCDP18S020", wxTextCtrl)->Enable(elfConfiguration[CDP18S020].useElfControlWindows);
        XRCCTRL(*this, "AutoBootCDP18S020", wxCheckBox)->SetValue(elfConfiguration[CDP18S020].autoBoot);

        XRCCTRL(*this, "RamCDP18S020", wxChoice)->SetSelection(conf[CDP18S020].ramType_);
        XRCCTRL(*this, "AutoBootTypeCDP18S020", wxChoice)->SetSelection(elfConfiguration[CDP18S020].autoBootType);
        XRCCTRL(*this, "ForceUCCDP18S020", wxCheckBox)->SetValue(elfConfiguration[CDP18S020].forceUpperCase);
    }
}

void GuiCdp18s020::writeCdp18s020DirConfig()
{
    writeConfigDir("/Dir/CDP18S020/Main", conf[CDP18S020].mainDir_);
    writeConfigDir("/Dir/CDP18S020/Main_Rom_File", conf[CDP18S020].romDir_[MAINROM1]);
	writeConfigDir("/Dir/CDP18S020/Software_File", conf[CDP18S020].ramDir_);
	writeConfigDir("/Dir/CDP18S020/Chip_8_Software", conf[CDP18S020].chip8SWDir_);
	writeConfigDir("/Dir/CDP18S020/Print_File", conf[CDP18S020].printFileDir_);
	writeConfigDir("/Dir/CDP18S020/Vt_Font_Rom_File", conf[CDP18S020].vtCharRomDir_);
	writeConfigDir("/Dir/CDP18S020/Video_Dump_File", conf[CDP18S020].screenDumpFileDir_);
	writeConfigDir("/Dir/CDP18S020/Wav_File", conf[CDP18S020].wavFileDir_[0]);
	writeConfigDir("/Dir/CDP18S020/Vt_Wav_File", elfConfiguration[CDP18S020].vtWavFileDir_);
}

void GuiCdp18s020::writeCdp18s020Config()
{
    configPointer->Write("/CDP18S020/Main_Rom_File", conf[CDP18S020].rom_[MAINROM1]);
	configPointer->Write("/CDP18S020/Vt_Font_Rom_File", conf[CDP18S020].vtCharRom_);
	configPointer->Write("/CDP18S020/Ram_Software", conf[CDP18S020].ram_);
	configPointer->Write("/CDP18S020/Video_Dump_File", conf[CDP18S020].screenDumpFile_);
    configPointer->Write("/CDP18S020/Terminal_File", conf[CDP18S020].wavFile_[0]);
	configPointer->Write("/CDP18S020/Vt_Wav_File", elfConfiguration[CDP18S020].vtWavFile_);
    configPointer->Write("/CDP18S020/VtSerialPortChoice", elfConfiguration[CDP18S020].serialPort_);

	configPointer->Write("/CDP18S020/Bell_Frequency", elfConfiguration[CDP18S020].bellFrequency_);
	configPointer->Write("/CDP18S020/VT_Type", elfConfiguration[CDP18S020].vtType);

    long value = elfConfiguration[CDP18S020].vt52SetUpFeature_.to_ulong();
    configPointer->Write("/CDP18S020/VT52Setup", value);
    value = elfConfiguration[CDP18S020].vt100SetUpFeature_.to_ulong();
    configPointer->Write("/CDP18S020/VT100Setup", value);
    value = elfConfiguration[CDP18S020].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("/CDP18S020/VTExternalSetup", value);

	configPointer->Write("/CDP18S020/Vt_Baud", elfConfiguration[CDP18S020].baudT);
    configPointer->Write("/CDP18S020/Enable_Auto_Boot", elfConfiguration[CDP18S020].autoBoot);

	configPointer->Write("/CDP18S020/Zoom", conf[CDP18S020].zoom_);
	configPointer->Write("/CDP18S020/Vt_Zoom", conf[CDP18S020].zoomVt_);
	configPointer->Write("/CDP18S020/Enable_Vt_Stretch_Dot", conf[CDP18S020].stretchDot_);
    configPointer->Write("/CDP18S020/Enable_Vt_External", elfConfiguration[CDP18S020].vtExternal);
    configPointer->Write("/CDP18S020/Open_Control_Windows", elfConfiguration[CDP18S020].useElfControlWindows);

	configPointer->Write("/CDP18S020/Latch", latch_);

    configPointer->Write("/CDP18S020/Enable_Turbo_Cassette", conf[CDP18S020].turbo_);
    configPointer->Write("/CDP18S020/Turbo_Clock_Speed", conf[CDP18S020].turboClock_);
    configPointer->Write("/CDP18S020/Enable_Real_Cassette", conf[CDP18S020].realCassetteLoad_);
	configPointer->Write("/CDP18S020/Volume", conf[CDP18S020].volume_);
    configPointer->Write("/CDP18S020/Led_Update_Frequency", conf[CDP18S020].ledTime_);

	configPointer->Write("/CDP18S020/Clock_Speed", conf[CDP18S020].clock_);
    configPointer->Write("/CDP18S020/Ram_Type", conf[CDP18S020].ramType_);
    configPointer->Write("/CDP18S020/AutoBootType", elfConfiguration[CDP18S020].autoBootType);
    configPointer->Write("/CDP18S020/Force_Uppercase", elfConfiguration[CDP18S020].forceUpperCase);
}

void GuiCdp18s020::readCdp18s020WindowConfig()
{    
    conf[CDP18S020].mainX_ = (int)configPointer->Read("/CDP18S020/Cdp18s020X", mainWindowX_);
    conf[CDP18S020].mainY_ = (int)configPointer->Read("/CDP18S020/Cdp18s020Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    conf[CDP18S020].vtX_ = (int)configPointer->Read("/CDP18S020/Cdp18s020PixieX", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[CDP18S020].vtY_ = (int)configPointer->Read("/CDP18S020/Cdp18s020PixieY", mainWindowY_);
}

void GuiCdp18s020::writeCdp18s020WindowConfig()
{
    if (conf[CDP18S020].mainX_ > 0)
        configPointer->Write("/CDP18S020/Cdp18s020X", conf[CDP18S020].mainX_);
    if (conf[CDP18S020].mainY_ > 0)
        configPointer->Write("/CDP18S020/Cdp18s020Y", conf[CDP18S020].mainY_);
    if (conf[CDP18S020].vtX_ > 0)
        configPointer->Write("/CDP18S020/Window_Position_Vt_X", conf[CDP18S020].vtX_);
    if (conf[CDP18S020].vtY_ > 0)
        configPointer->Write("/CDP18S020/Window_Position_Vt_Y", conf[CDP18S020].vtY_);
}

void GuiCdp18s020::onRamSWCdp18s020(wxCommandEvent&event)
{
	onRamSW(event);
}

void GuiCdp18s020::onLatch(wxCommandEvent&event)
{
	latch_ = event.IsChecked();
}

void GuiCdp18s020::onCdp18s020BaudR(wxCommandEvent&event)
{
	elfConfiguration[CDP18S020].baudR = event.GetSelection();
}

void GuiCdp18s020::onCdp18s020BaudT(wxCommandEvent&event)
{
	elfConfiguration[CDP18S020].baudT = event.GetSelection();
	if (!elfConfiguration[CDP18S020].useUart)
	{
		elfConfiguration[CDP18S020].baudR = event.GetSelection();
		XRCCTRL(*this, "VTBaudRChoiceCDP18S020", wxChoice)->SetSelection(elfConfiguration[CDP18S020].baudR);
	}
}

void GuiCdp18s020::onCdp18s020ControlWindows(wxCommandEvent&event)
{
    elfConfiguration[CDP18S020].useElfControlWindows = event.IsChecked();
    
    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddressCDP18S020",wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText1CDP18S020",wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText2CDP18S020",wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
    }
    
    if (runningComputer_ == CDP18S020)
        p_Cdp18s020->Show(elfConfiguration[CDP18S020].useElfControlWindows);
}

bool GuiCdp18s020::getUseCdp18s020ControlWindows()
{
    return elfConfiguration[CDP18S020].useElfControlWindows;
}

void GuiCdp18s020::onAutoBoot(wxCommandEvent&event)
{
    elfConfiguration[CDP18S020].autoBoot = event.IsChecked();
}

void GuiCdp18s020::onAutoBootType(wxCommandEvent&event)
{
    elfConfiguration[CDP18S020].autoBootType = event.GetSelection();
    if (elfConfiguration[CDP18S020].autoBootType == 0)
        conf[CDP18S020].bootAddress_ = 0x8000;
    else
        conf[CDP18S020].bootAddress_ = 0;
    if (runningComputer_ == CDP18S020)
        p_Cdp18s020->setAddressLatch(conf[CDP18S020].bootAddress_);
}

void GuiCdp18s020::onCdp18s020ForceUpperCase(wxCommandEvent&event)
{
    elfConfiguration[CDP18S020].forceUpperCase = event.IsChecked();
    if (runningComputer_ == CDP18S020)
    {
        p_Computer->setForceUpperCase(event.IsChecked());
    }
}




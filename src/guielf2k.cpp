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

DEFINE_EVENT_TYPE(ON_UART_ELF2K)

BEGIN_EVENT_TABLE(GuiElf2K, GuiMS2000)

	EVT_TEXT(XRCID("MainRomElf2K"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomElf2K"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonElf2K"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("IdeFileElf2K"), GuiMain::onIdeText)
	EVT_BUTTON(XRCID("IDE_ButtonElf2K"), GuiMain::onIde)
	EVT_BUTTON(XRCID("Eject_IDEElf2K"), GuiMain::onIdeEject)

	EVT_TEXT(XRCID("CharRomElf2K"), GuiMain::onCharRomText)
	EVT_COMBOBOX(XRCID("CharRomElf2K"), GuiMain::onCharRomText)
	EVT_BUTTON(XRCID("CharRomButtonElf2K"), GuiMain::onCharRom)

	EVT_TEXT(XRCID("KeyFileElf2K"), GuiMain::onKeyFileText)
	EVT_BUTTON(XRCID("KeyFileButtonElf2K"), GuiMain::onKeyFile)
	EVT_BUTTON(XRCID("EjectKeyFileElf2K"), GuiMain::onKeyFileEject)

	EVT_CHOICE(XRCID("VTTypeElf2K"), GuiMain::onVT100)
	EVT_SPIN_UP(XRCID("ZoomSpinElf2K"), GuiMain::onZoom)
	EVT_SPIN_DOWN(XRCID("ZoomSpinElf2K"), GuiMain::onZoom)
	EVT_TEXT(XRCID("ZoomValueElf2K"), GuiMain::onZoomValue)
	EVT_SPIN_UP(XRCID("ZoomSpinVtElf2K"), GuiMain::onZoomVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtElf2K"), GuiMain::onZoomVt)
	EVT_TEXT(XRCID("ZoomValueVtElf2K"), GuiMain::onZoomValueVt)
	EVT_BUTTON(XRCID("FullScreenF3Elf2K"), GuiMain::onFullScreen)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeElf2K"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeElf2K"), GuiMain::onVolume)
	EVT_BUTTON(XRCID("ScreenDumpFileButtonElf2K"), GuiMain::onScreenDumpFile)

	EVT_TEXT(XRCID("ScreenDumpFileElf2K"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileElf2K"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5Elf2K"), GuiMain::onScreenDump)

	EVT_BUTTON(XRCID("DP_ButtonElf2K"), GuiMain::onDp)
	EVT_BUTTON(XRCID("VtSetupElf2K"), GuiMain::onVtSetup)
	EVT_BUTTON(XRCID("SaveButtonElf2K"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonElf2K"), GuiMain::onLoadButton)
	EVT_CHECKBOX(XRCID("InterlaceElf2K"), GuiMain::onInterlace)
	EVT_CHECKBOX(XRCID("StretchDotElf2K"), GuiMain::onStretchDot)
	EVT_TEXT(XRCID("ShowAddressElf2K"), GuiMain::onLedTimer)
	EVT_BUTTON(XRCID("KeyMapElf2K"), Main::onHexKeyDef)
	EVT_BUTTON(XRCID("ColoursElf2K"), Main::onColoursDef)
	EVT_CHECKBOX(XRCID("Elf2KClearRam"), GuiMain::onClearRam)

	EVT_TEXT(XRCID("SaveStartElf2K"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndElf2K"), GuiMain::onSaveEnd)

	EVT_CHOICE(XRCID("Elf2KVideoType"), GuiElf2K::onElf2KVideoType)
	EVT_CHOICE(XRCID("Elf2KKeyboard"), GuiElf2K::onElf2KKeyboard)
	EVT_CHECKBOX(XRCID("Elf2KForceUC"), GuiElf2K::onElf2KForceUpperCase)
	EVT_CHECKBOX(XRCID("Elf2KControlWindows"), GuiElf2K::onElf2KControlWindows)
	EVT_CHECKBOX(XRCID("Elf2KNvr"), GuiElf2K::onElf2KNvr)
	EVT_CHECKBOX(XRCID("Elf2KRtc"), GuiElf2K::onElf2KRtc)
	EVT_CHECKBOX(XRCID("Elf2KSwitch"), GuiElf2K::onElf2KSwitch)
	EVT_CHECKBOX(XRCID("Elf2KHex"), GuiElf2K::onElf2KHex)
	EVT_CHECKBOX(XRCID("Elf2KBootRam"), GuiElf2K::onBootRam)
	EVT_CHECKBOX(XRCID("Elf2KClearRtc"), GuiElf2K::onClearRtc)

    EVT_CHOICE(XRCID("VTBaudTChoiceElf2K"), GuiElf2K::onElf2KBaudT)
    EVT_CHOICE(XRCID("VTBaudRChoiceElf2K"), GuiElf2K::onElf2KBaudR)

    EVT_BUTTON(XRCID("CasButtonElf2K"), GuiMain::onCassette)
    EVT_BUTTON(XRCID("EjectCasElf2K"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileElf2K"), GuiMain::onCassetteText)
    EVT_BUTTON(XRCID("CasLoadElf2K"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasSaveElf2K"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasStopElf2K"), GuiMain::onCassetteStop)
    EVT_CHECKBOX(XRCID("TurboElf2K"), GuiMain::onTurbo)
    EVT_TEXT(XRCID("TurboClockElf2K"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadElf2K"), GuiMain::onAutoLoad)
    EVT_BUTTON(XRCID("TapeElf2K"), GuiElf2K::onTape)

    EVT_COMMAND(wxID_ANY, ON_UART_ELF2K, GuiElf::onElf2KUart)

END_EVENT_TABLE()

GuiElf2K::GuiElf2K(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode_, wxString dataDir, wxString iniDir)
: GuiMS2000(title, pos, size, mode_, dataDir, iniDir)
{
	conf[ELF2K].saveStartString_ = "";
	conf[ELF2K].saveEndString_ = "";
    position_.x = 0;
}

void GuiElf2K::readElf2KConfig()
{
	selectedComputer_ = ELF2K;

	readElfPortConfig(ELF2K, "Elf2K");
	
	conf[ELF2K].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Elf2K" + pathSeparator_;

    conf[ELF2K].mainDir_ = readConfigDir("/Dir/Elf2K/Main", dataDir_ + "Elf2K" + pathSeparator_);
	conf[ELF2K].romDir_[MAINROM1] = readConfigDir("/Dir/Elf2K/Main_Rom_File", dataDir_ + "Elf2K" + pathSeparator_);
	conf[ELF2K].charRomDir_ = readConfigDir("/Dir/Elf2K/Font_Rom_File", dataDir_ + "Elf2K" + pathSeparator_);
	elfConfiguration[ELF2K].vtCharRomDir_ = readConfigDir("/Dir/Elf2K/Vt_Font_Rom_File", dataDir_ + "Elf2K" + pathSeparator_);
	conf[ELF2K].ramDir_ = readConfigDir("/Dir/Elf2K/Software_File", dataDir_ + "Elf2K" + pathSeparator_);
	conf[ELF2K].ideDir_ = readConfigDir("/Dir/Elf2K/Ide_File", dataDir_ + "Elf2K" + pathSeparator_);
	conf[ELF2K].keyFileDir_ = readConfigDir("/Dir/Elf2K/Key_File", dataDir_ + "Elf2K" + pathSeparator_);
	conf[ELF2K].screenDumpFileDir_ = readConfigDir("/Dir/Elf2K/Video_Dump_File", dataDir_ + "Elf2K" + pathSeparator_);
    conf[ELF2K].wavFileDir_[0] = readConfigDir("/Dir/Elf2K/Wav_File", dataDir_ + "Elf2K" + pathSeparator_);
	elfConfiguration[ELF2K].vtWavFileDir_ = readConfigDir("/Dir/Elf2K/Vt_Wav_File", dataDir_ + "Elf2K" + pathSeparator_);
 
	conf[ELF2K].rom_[MAINROM1] = configPointer->Read("/Elf2K/Main_Rom_File", "Elf2Kv112.bin");
	conf[ELF2K].charRom_ = configPointer->Read("/Elf2K/I8275_Font_Rom_File", "intel8275.bin");
	conf[ELF2K].ide_ = configPointer->Read("/Elf2K/Ide_File", "elf2k.ide");
	conf[ELF2K].keyFile_ = configPointer->Read("/Elf2K/Key_File", "");
	conf[ELF2K].screenDumpFile_ = configPointer->Read("/Elf2K/Video_Dump_File", "screendump.png");
    conf[ELF2K].wavFile_[0] = configPointer->Read("/Elf2K/Wav_File", "");
    conf[ELF2K].terminalFiles_.Add(conf[ELF2K].wavFile_[0]);
    conf[ELF2K].terminalPaths_.Add(conf[ELF2K].wavFileDir_[0]+conf[ELF2K].wavFile_[0]);
	elfConfiguration[ELF2K].vtWavFile_ = configPointer->Read("/Elf2K/Vt_Wav_File", "");
    elfConfiguration[ELF2K].serialPort_ = configPointer->Read("/Elf2K/VtSerialPortChoice", "");
	conf[ELF2K].volume_ = (int)configPointer->Read("/Elf2K/Volume", 25l);
	conf[ELF2K].videoMode_ = (int)configPointer->Read("/Elf2K/Video_Type", 2l);

    configPointer->Read("/Elf2K/SerialLog", &elfConfiguration[ELF2K].serialLog, false);
	configPointer->Read("/Elf2K/Enable_Interlace", &conf[ELF2K].interlace_, true);
	configPointer->Read("/Elf2K/Enable_Vt_Stretch_Dot", &conf[ELF2K].stretchDot_, false);
    configPointer->Read("/Elf2K/Enable_Vt_External", &elfConfiguration[ELF2K].vtExternal, false);
    conf[ELF2K].beepFrequency_ = (int)configPointer->Read("/Elf2K/Beep_Frequency", 250);
    
	elfConfiguration[ELF2K].bellFrequency_ = (int)configPointer->Read("/Elf2K/Bell_Frequency", 800);
	elfConfiguration[ELF2K].baudR = (int)configPointer->Read("/Elf2K/Vt_Baud_Receive", 3l);
	elfConfiguration[ELF2K].baudT = (int)configPointer->Read("/Elf2K/Vt_Baud_Transmit", 3l);
	elfConfiguration[ELF2K].vtType = (int)configPointer->Read("/Elf2K/VT_Type", 0l);
    elfConfiguration[ELF2K].vt52SetUpFeature_ = configPointer->Read("/Elf2K/VT52Setup", 0x00004092l);
    elfConfiguration[ELF2K].vt100SetUpFeature_ = configPointer->Read("/Elf2K/VT100Setup", 0x0000ca52l);
    elfConfiguration[ELF2K].vtExternalSetUpFeature_ = configPointer->Read("/Elf2K/VTExternalSetup", 0x0000ca52l);
	elfConfiguration[ELF2K].keyboardType = (int)configPointer->Read("/Elf2K/Keyboard_Type", 2l);
	elfConfiguration[ELF2K].clearRam = false;
	elfConfiguration[ELF2K].clearRtc = false;

    elfConfiguration[ELF2K].vtCharactersPerRow = (int)configPointer->Read("/Elf2K/VT100CharPerRow", 80);
    elfConfiguration[ELF2K].vt100CharWidth = (int)configPointer->Read("/Elf2K/VT100CharWidth", 10);
    elfConfiguration[ELF2K].vt52CharWidth = (int)configPointer->Read("/Elf2K/VT52CharWidth", 9);

	configPointer->Read("/Elf2K/Uart", &elfConfiguration[ELF2K].useUart, false);
	configPointer->Read("/Elf2K/Force_Uppercase", &elfConfiguration[ELF2K].forceUpperCase, false);
	configPointer->Read("/Elf2K/Open_Control_Windows", &elfConfiguration[ELF2K].useElfControlWindows, true);
	configPointer->Read("/Elf2K/Use_Non_Volatile_Ram", &elfConfiguration[ELF2K].nvr, true);
	configPointer->Read("/Elf2K/Use_Real_Time_Clock", &elfConfiguration[ELF2K].rtc, true);
	configPointer->Read("/Elf2K/Use_Switch_Panel", &elfConfiguration[ELF2K].useSwitch, false);
	configPointer->Read("/Elf2K/Use_Hex_Panel", &elfConfiguration[ELF2K].useHex, false);
	configPointer->Read("/Elf2K/Boot_From_Ram", &elfConfiguration[ELF2K].bootRam, false);

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 1.0);
	conf[ELF2K].zoom_ = configPointer->Read("/Elf2K/Zoom", defaultZoom);
	conf[ELF2K].zoomVt_ = configPointer->Read("/Elf2K/Vt_Zoom", defaultZoom);
	wxString defaultScale;
	defaultScale.Printf("%i", 3);
	conf[ELF2K].xScale_ = configPointer->Read("/Elf2K/Window_Scale_Factor_X", defaultScale);
	wxString defaultTimer;
	defaultTimer.Printf("%d", 100);
	conf[ELF2K].ledTime_ = configPointer->Read("/Elf2K/Led_Update_Frequency", defaultTimer);

	wxString defaultClock;
	defaultClock.Printf("%1.4f", 3.5795);
	elf2K8275Clock_ = configPointer->Read("/Elf2K/Clock_Speed_When_Using_I8275", defaultClock);
	defaultClock.Printf("%1.4f", 1.78975);
	elf2KPixieClock_ = configPointer->Read("/Elf2K/Clock_Speed_When_Using_Pixie", defaultClock);
	if (conf[ELF2K].videoMode_ == VIDEOPIXIE)
		conf[ELF2K].clock_ = elf2KPixieClock_;
	else
		conf[ELF2K].clock_ = elf2K8275Clock_;

    configPointer->Read("/Elf2K/Enable_Turbo_Cassette", &conf[ELF2K].turbo_, true);
    conf[ELF2K].turboClock_ = configPointer->Read("/Elf2K/Turbo_Clock_Speed", "15");
    configPointer->Read("/Elf2K/Enable_Auto_Cassette", &conf[ELF2K].autoCassetteLoad_, true);
    configPointer->Read("/Elf2K/Enable_Xmodem", &elfConfiguration[ELF2K].useXmodem, true);
    configPointer->Read("/Elf2K/Enable_HexModem", &elfConfiguration[ELF2K].useHexModem, false);
    elfConfiguration[ELF2K].packetSize = (int)configPointer->Read("/Elf2K/Ymodem_PacketSize", 0l);
    conf[ELF2K].realCassetteLoad_ =  false;
    elfConfiguration[ELF2K].useTape = true;

	if (mode_.gui)
		setBaudChoiceElf2K();

	setVtType("Elf2K", ELF2K, elfConfiguration[ELF2K].vtType, false);
	setElf2KVideoType(conf[ELF2K].videoMode_);

	elfConfiguration[ELF2K].vtCharRom_ = configPointer->Read("/Elf2K/Vt_Font_Rom_File", "vt52.a.bin");
  
    long value;
    conf[ELF2K].saveStartString_ = configPointer->Read("/Elf2K/SaveStart", "0");
    if (!conf[ELF2K].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[ELF2K].saveStart_ = value;

	if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomElf2K", wxComboBox)->SetValue(conf[ELF2K].rom_[MAINROM1]);
		XRCCTRL(*this, "CharRomElf2K", wxComboBox)->SetValue(conf[ELF2K].charRom_);
		XRCCTRL(*this, "IdeFileElf2K", wxTextCtrl)->SetValue(conf[ELF2K].ide_);
		XRCCTRL(*this, "KeyFileElf2K", wxTextCtrl)->SetValue(conf[ELF2K].keyFile_);
		XRCCTRL(*this, "ScreenDumpFileElf2K", wxComboBox)->SetValue(conf[ELF2K].screenDumpFile_);
        XRCCTRL(*this, "WavFileElf2K", wxTextCtrl)->SetValue(conf[ELF2K].wavFile_[0]);

        if (elfConfiguration[ELF2K].vtExternal)
            XRCCTRL(*this, "VTTypeElf2K", wxChoice)->SetSelection(EXTERNAL_TERMINAL);
        else
            XRCCTRL(*this, "VTTypeElf2K", wxChoice)->SetSelection(elfConfiguration[ELF2K].vtType);
		XRCCTRL(*this, "Elf2KVideoType", wxChoice)->SetSelection(conf[ELF2K].videoMode_);
		XRCCTRL(*this, "Elf2KForceUC", wxCheckBox)->SetValue(elfConfiguration[ELF2K].forceUpperCase);

		XRCCTRL(*this, "VTBaudRChoiceElf2K", wxChoice)->SetSelection(elfConfiguration[ELF2K].baudR);
		XRCCTRL(*this, "VTBaudTChoiceElf2K", wxChoice)->SetSelection(elfConfiguration[ELF2K].baudT);
        XRCCTRL(*this, "Elf2KAddressText1",wxStaticText)->Enable(elfConfiguration[ELF2K].useElfControlWindows);
        XRCCTRL(*this, "Elf2KAddressText2",wxStaticText)->Enable(elfConfiguration[ELF2K].useElfControlWindows);

        XRCCTRL(*this, "Elf2KKeyboard", wxChoice)->SetSelection(elfConfiguration[ELF2K].keyboardType);
        
        correctZoomAndValue(ELF2K, "Elf2K", SET_SPIN);
        correctZoomVtAndValue(ELF2K, "Elf2K", SET_SPIN);

		XRCCTRL(*this, "ShowAddressElf2K", wxTextCtrl)->ChangeValue(conf[ELF2K].ledTime_);
		XRCCTRL(*this, "Elf2KControlWindows", wxCheckBox)->SetValue(elfConfiguration[ELF2K].useElfControlWindows);
		XRCCTRL(*this, "ShowAddressElf2K",wxTextCtrl)->Enable(elfConfiguration[ELF2K].useElfControlWindows);
		XRCCTRL(*this, "InterlaceElf2K", wxCheckBox)->SetValue(conf[ELF2K].interlace_);
		XRCCTRL(*this, "StretchDotElf2K", wxCheckBox)->SetValue(conf[ELF2K].stretchDot_);
		XRCCTRL(*this, "Elf2KNvr", wxCheckBox)->SetValue(elfConfiguration[ELF2K].nvr);
		XRCCTRL(*this, "Elf2KRtc", wxCheckBox)->SetValue(elfConfiguration[ELF2K].rtc);
		XRCCTRL(*this, "Elf2KSwitch", wxCheckBox)->SetValue(elfConfiguration[ELF2K].useSwitch);
		XRCCTRL(*this, "Elf2KHex", wxCheckBox)->SetValue(elfConfiguration[ELF2K].useHex);
		XRCCTRL(*this, "Elf2KClearRam", wxCheckBox)->Enable(elfConfiguration[ELF2K].nvr);
		XRCCTRL(*this, "Elf2KClearRam", wxCheckBox)->SetValue(elfConfiguration[ELF2K].clearRam);
		XRCCTRL(*this, "Elf2KClearRtc", wxCheckBox)->Enable(elfConfiguration[ELF2K].rtc);
		XRCCTRL(*this, "Elf2KClearRtc", wxCheckBox)->SetValue(elfConfiguration[ELF2K].clearRtc);
		XRCCTRL(*this, "Elf2KBootRam", wxCheckBox)->SetValue(elfConfiguration[ELF2K].bootRam);
		XRCCTRL(*this, "VolumeElf2K", wxSlider)->SetValue(conf[ELF2K].volume_);

        XRCCTRL(*this, "TurboElf2K", wxCheckBox)->SetValue(conf[ELF2K].turbo_);

        XRCCTRL(*this, "TurboClockElf2K", wxTextCtrl)->SetValue(conf[ELF2K].turboClock_);
        XRCCTRL(*this, "AutoCasLoadElf2K", wxCheckBox)->SetValue(conf[ELF2K].autoCassetteLoad_);
        if (elfConfiguration[runningComputer_].useXmodem || elfConfiguration[runningComputer_].useHexModem)
        {
            turboGui("Elf2K");
            enableTapeGui(elfConfiguration[ELF2K].useTape, ELF2K);
        }
        
		if (conf[ELF2K].videoMode_ == VIDEOPIXIE)
        {
            if (clockTextCtrl[ELF2K] != NULL)
                clockTextCtrl[ELF2K]->ChangeValue(elf2KPixieClock_);
        }
		else
        {
            if (clockTextCtrl[ELF2K] != NULL)
                clockTextCtrl[ELF2K]->ChangeValue(elf2K8275Clock_);
        }
        setTape();
        if (conf[ELF2K].saveStart_ != 0)
            XRCCTRL(*this, "SaveStartElf2K", wxTextCtrl)->SetValue(conf[ELF2K].saveStartString_);
	}
	setElf2KKeyboard(elfConfiguration[ELF2K].keyboardType);

	elfConfiguration[ELF2K].usePortExtender = false;
	elfConfiguration[ELF2K].ideEnabled = true;
    elfConfiguration[ELF2K].fdcEnabled = false;
    elfConfiguration[ELF2K].useLedModule = false;
	elfConfiguration[ELF2K].useTape = false;
	conf[ELF2K].realCassetteLoad_ = false;

}

void GuiElf2K::writeElf2KDirConfig()
{
    writeConfigDir("/Dir/Elf2K/Main", conf[ELF2K].mainDir_);
	writeConfigDir("/Dir/Elf2K/Main_Rom_File", conf[ELF2K].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Elf2K/I8275_Font_Rom_File", conf[ELF2K].charRomDir_);
	writeConfigDir("/Dir/Elf2K/Vt_Font_Rom_File", elfConfiguration[ELF2K].vtCharRomDir_);
	writeConfigDir("/Dir/Elf2K/Software_File", conf[ELF2K].ramDir_);
	writeConfigDir("/Dir/Elf2K/Ide_File", conf[ELF2K].ideDir_);
	writeConfigDir("/Dir/Elf2K/Key_File", conf[ELF2K].keyFileDir_);
	writeConfigDir("/Dir/Elf2K/Video_Dump_File", conf[ELF2K].screenDumpFileDir_);
    writeConfigDir("/Dir/Elf2K/Wav_File", conf[ELF2K].wavFileDir_[0]);
	writeConfigDir("/Dir/Elf2K/Vt_Wav_File", elfConfiguration[ELF2K].vtWavFileDir_);
}

void GuiElf2K::writeElf2KConfig()
{
    writeElfPortConfig(ELF2K, "Elf2K");

    configPointer->Write("/Elf2K/Main_Rom_File", conf[ELF2K].rom_[MAINROM1]);
	configPointer->Write("/Elf2K/I8275_Font_Rom_File", conf[ELF2K].charRom_);
	configPointer->Write("/Elf2K/Vt_Font_Rom_File", elfConfiguration[ELF2K].vtCharRom_);
	configPointer->Write("/Elf2K/Ide_File", conf[ELF2K].ide_);
	configPointer->Write("/Elf2K/Key_File", conf[ELF2K].keyFile_);
	configPointer->Write("/Elf2K/Video_Dump_File", conf[ELF2K].screenDumpFile_);
    configPointer->Write("/Elf2K/Wav_File", conf[ELF2K].wavFile_[0]);
	configPointer->Write("/Elf2K/Vt_Wav_File", elfConfiguration[ELF2K].vtWavFile_);
    configPointer->Write("/Elf2K/VtSerialPortChoice", elfConfiguration[ELF2K].serialPort_);

	configPointer->Write("/Elf2K/Bell_Frequency", elfConfiguration[ELF2K].bellFrequency_);
	configPointer->Write("/Elf2K/VT_Type", elfConfiguration[ELF2K].vtType);
    
    long value = elfConfiguration[ELF2K].vt52SetUpFeature_.to_ulong();
    configPointer->Write("/Elf2K/VT52Setup", value);
    value = elfConfiguration[ELF2K].vt100SetUpFeature_.to_ulong();
    configPointer->Write("/Elf2K/VT100Setup", value);
    value = elfConfiguration[ELF2K].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("/Elf2K/VTExternalSetup", value);
    configPointer->Write("/Elf2K/VT100CharPerRow", elfConfiguration[ELF2K].vtCharactersPerRow);
    configPointer->Write("/Elf2K/VT100CharWidth", elfConfiguration[ELF2K].vt100CharWidth);
    configPointer->Write("/Elf2K/VT52CharWidth", elfConfiguration[ELF2K].vt52CharWidth);

	configPointer->Write("/Elf2K/Vt_Baud_Receive", elfConfiguration[ELF2K].baudR);
	configPointer->Write("/Elf2K/Vt_Baud_Transmit", elfConfiguration[ELF2K].baudT);
	configPointer->Write("/Elf2K/Video_Type", conf[ELF2K].videoMode_);
	configPointer->Write("/Elf2K/Keyboard_Type", elfConfiguration[ELF2K].keyboardType);
	configPointer->Write("/Elf2K/Zoom", conf[ELF2K].zoom_);
	configPointer->Write("/Elf2K/Vt_Zoom", conf[ELF2K].zoomVt_);
    configPointer->Write("/Elf2K/Force_Uppercase", elfConfiguration[ELF2K].forceUpperCase);
    configPointer->Write("/Elf2K/SerialLog", elfConfiguration[ELF2K].serialLog);
    configPointer->Write("/Elf2K/Uart", elfConfiguration[ELF2K].useUart);
    configPointer->Write("/Elf2K/Led_Update_Frequency", conf[ELF2K].ledTime_);
	configPointer->Write("/Elf2K/Use_Real_Time_Clock", elfConfiguration[ELF2K].rtc);
	configPointer->Write("/Elf2K/Use_Non_Volatile_Ram", elfConfiguration[ELF2K].nvr);
	configPointer->Write("/Elf2K/Boot_From_Ram", elfConfiguration[ELF2K].bootRam);
	configPointer->Write("/Elf2K/Use_Switch_Panel", elfConfiguration[ELF2K].useSwitch);
	configPointer->Write("/Elf2K/Use_Hex_Panel", elfConfiguration[ELF2K].useHex);
	configPointer->Write("/Elf2K/Open_Control_Windows", elfConfiguration[ELF2K].useElfControlWindows);
	configPointer->Write("/Elf2K/Enable_Interlace", conf[ELF2K].interlace_);
	configPointer->Write("/Elf2K/Enable_Vt_Stretch_Dot", conf[ELF2K].stretchDot_);
    configPointer->Write("/Elf2K/Enable_Vt_External", elfConfiguration[ELF2K].vtExternal);
    configPointer->Write("/Elf2K/Enable_Turbo_Cassette", conf[ELF2K].turbo_);
    configPointer->Write("/Elf2K/Turbo_Clock_Speed", conf[ELF2K].turboClock_);
    configPointer->Write("/Elf2K/Enable_Auto_Cassette", conf[ELF2K].autoCassetteLoad_);
    configPointer->Write("/Elf2K/Enable_Xmodem", elfConfiguration[ELF2K].useXmodem);
    configPointer->Write("/Elf2K/Enable_HexModem", elfConfiguration[ELF2K].useHexModem);
    configPointer->Write("/Elf2K/Ymodem_PacketSize", elfConfiguration[ELF2K].packetSize);
	configPointer->Write("/Elf2K/Volume", conf[ELF2K].volume_);
    configPointer->Write("/Elf2K/SaveStart", conf[ELF2K].saveStartString_);

	configPointer->Write("/Elf2K/Clock_Speed_When_Using_Pixie", elf2KPixieClock_);
	configPointer->Write("/Elf2K/Clock_Speed_When_Using_I8275", elf2K8275Clock_);
}

void GuiElf2K::readElf2KWindowConfig()
{
    conf[ELF2K].i8275X_ = (int)configPointer->Read("/Elf2K/Window_Position_I8275_X", mainWindowX_+windowInfo.mainwX);
    conf[ELF2K].i8275Y_ = (int)configPointer->Read("/Elf2K/Window_Position_I8275_Y", mainWindowY_);
    conf[ELF2K].pixieX_ = (int)configPointer->Read("/Elf2K/Window_Position_Pixie_X", mainWindowX_+windowInfo.mainwX);
    conf[ELF2K].pixieY_ = (int)configPointer->Read("/Elf2K/Window_Position_Pixie_Y", mainWindowY_);
    conf[ELF2K].vtX_ = (int)configPointer->Read("/Elf2K/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX);
    conf[ELF2K].vtY_ = (int)configPointer->Read("/Elf2K/Window_Position_Vt_Y", mainWindowY_);
    conf[ELF2K].mainX_ = (int)configPointer->Read("/Elf2K/Window_Position_X", mainWindowX_);
    conf[ELF2K].mainY_ = (int)configPointer->Read("/Elf2K/Window_Position_Y", mainWindowY_+windowInfo.mainwY);
    switchX_ = (int)configPointer->Read("/Elf2K/Window_Position_Switch_Panel_X", mainWindowX_+507+windowInfo.xBorder2);
    switchY_ = (int)configPointer->Read("/Elf2K/Window_Position_Switch_Panel_Y", mainWindowY_+478+windowInfo.yBorder2);
    conf[ELF2K].keypadX_ = (int)configPointer->Read("/Elf2K/Window_Position_Hex_Panel_X", mainWindowX_+507+windowInfo.xBorder2);
    conf[ELF2K].keypadY_ = (int)configPointer->Read("/Elf2K/Window_Position_Hex_Panel_Y", mainWindowY_+478+windowInfo.yBorder2);
}

void GuiElf2K::writeElf2KWindowConfig()
{
    if (conf[ELF2K].i8275X_ > 0)
        configPointer->Write("/Elf2K/Window_Position_I8275_X", conf[ELF2K].i8275X_);
    if (conf[ELF2K].i8275Y_ > 0)
        configPointer->Write("/Elf2K/Window_Position_I8275_Y", conf[ELF2K].i8275Y_);
    if (conf[ELF2K].pixieX_ > 0)
        configPointer->Write("/Elf2K/Window_Position_Pixie_X", conf[ELF2K].pixieX_);
    if (conf[ELF2K].pixieY_ > 0)
        configPointer->Write("/Elf2K/Window_Position_Pixie_Y", conf[ELF2K].pixieY_);
    if (conf[ELF2K].vtX_ > 0)
        configPointer->Write("/Elf2K/Window_Position_Vt_X", conf[ELF2K].vtX_);
    if (conf[ELF2K].vtY_ > 0)
        configPointer->Write("/Elf2K/Window_Position_Vt_Y", conf[ELF2K].vtY_);
    if (switchX_ > 0)
        configPointer->Write("/Elf2K/Window_Position_Switch_Panel_X", switchX_);
    if (switchY_ > 0)
        configPointer->Write("/Elf2K/Window_Position_Switch_Panel_Y", switchY_);
    if (conf[ELF2K].keypadX_ > 0)
        configPointer->Write("/Elf2K/Window_Position_Hex_Panel_X", conf[ELF2K].keypadX_);
    if (conf[ELF2K].keypadY_ > 0)
        configPointer->Write("/Elf2K/Window_Position_Hex_Panel_Y", conf[ELF2K].keypadY_);
    if (conf[ELF2K].mainX_ > 0)
        configPointer->Write("/Elf2K/Window_Position_X", conf[ELF2K].mainX_);
    if (conf[ELF2K].mainY_ > 0)
        configPointer->Write("/Elf2K/Window_Position_Y", conf[ELF2K].mainY_);
    
}

void GuiElf2K::onElf2KVideoType(wxCommandEvent&event)
{
	setElf2KVideoType(event.GetSelection());
}

void GuiElf2K::onElf2KKeyboard(wxCommandEvent&event)
{
	setElf2KKeyboard(event.GetSelection());
}

void GuiElf2K::onElf2KBaudR(wxCommandEvent&event)
{
	if (elfConfiguration[ELF2K].baudR != event.GetSelection())
	{
		XRCCTRL(*this, "Elf2KClearRam", wxCheckBox)->SetValue(true);
		elfConfiguration[ELF2K].clearRam = true;
		elfConfiguration[ELF2K].baudR = event.GetSelection();
	}
}

void GuiElf2K::onElf2KBaudT(wxCommandEvent&event)
{
	if (elfConfiguration[ELF2K].baudT != event.GetSelection())
	{
		XRCCTRL(*this, "Elf2KClearRam", wxCheckBox)->SetValue(true);
		elfConfiguration[ELF2K].clearRam = true;
		elfConfiguration[ELF2K].baudT = event.GetSelection();
		if (!elfConfiguration[ELF2K].useUart)
		{
			elfConfiguration[ELF2K].baudR = event.GetSelection();
			XRCCTRL(*this, "VTBaudRChoiceElf2K", wxChoice)->SetSelection(elfConfiguration[ELF2K].baudR);
		}
	}
}

void GuiElf2K::onElf2KUart(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "Elf2KClearRam", wxCheckBox)->SetValue(true);
	elfConfiguration[ELF2K].clearRam = true;

	setBaudChoiceElf2K();

	if (elfConfiguration[ELF2K].useUart)
	{
		elfConfiguration[ELF2K].baudR += 3;
		elfConfiguration[ELF2K].baudT += 3;
		XRCCTRL(*this, "VTBaudRChoiceElf2K", wxChoice)->SetSelection(elfConfiguration[ELF2K].baudR);
		XRCCTRL(*this, "VTBaudTChoiceElf2K", wxChoice)->SetSelection(elfConfiguration[ELF2K].baudT);
	}
	else
	{
		if (elfConfiguration[ELF2K].baudT < 3)  elfConfiguration[ELF2K].baudT = 3;
		if (elfConfiguration[ELF2K].baudT > 9)  elfConfiguration[ELF2K].baudT = 9;
		elfConfiguration[ELF2K].baudT -= 3;
		elfConfiguration[ELF2K].baudR = elfConfiguration[ELF2K].baudT;
		XRCCTRL(*this, "VTBaudRChoiceElf2K", wxChoice)->SetSelection(elfConfiguration[ELF2K].baudR);
		XRCCTRL(*this, "VTBaudTChoiceElf2K", wxChoice)->SetSelection(elfConfiguration[ELF2K].baudT);
		XRCCTRL(*this, "VTBaudTTextElf2K", wxStaticText)->Enable(false);
	}
}

void GuiElf2K::onElf2KForceUpperCase(wxCommandEvent&event)
{
	elfConfiguration[ELF2K].forceUpperCase = event.IsChecked();
	if (runningComputer_ == ELF2K)
	{
		p_Computer->setForceUpperCase(event.IsChecked());
	}
}

void GuiElf2K::onBootRam(wxCommandEvent&event)
{
	elfConfiguration[ELF2K].bootRam = event.IsChecked();
}

void GuiElf2K::onClearRtc(wxCommandEvent&event)
{
	elfConfiguration[ELF2K].clearRtc = event.IsChecked();
}

void GuiElf2K::onTape(wxCommandEvent&WXUNUSED(event))
{
    if (!elfConfiguration[selectedComputer_].useXmodem)
    {
        elfConfiguration[selectedComputer_].useXmodem = true;
        elfConfiguration[selectedComputer_].useHexModem = false;
    }
    else
    {
        elfConfiguration[selectedComputer_].useXmodem = false;
        elfConfiguration[selectedComputer_].useHexModem = true;
    }
    setTape();
}

void GuiElf2K::setTape()
{
    if (elfConfiguration[selectedComputer_].useXmodem)
        XRCCTRL(*this, "CasButtonElf2K", wxButton)->SetLabel("XMODEM");
    else
        XRCCTRL(*this, "CasButtonElf2K", wxButton)->SetLabel("HEX");
}

void GuiElf2K::onElf2KControlWindows(wxCommandEvent&event)
{
	elfConfiguration[ELF2K].useElfControlWindows = event.IsChecked();
	if (mode_.gui)
	{
		XRCCTRL(*this,"ShowAddressElf2K",wxTextCtrl)->Enable(elfConfiguration[ELF2K].useElfControlWindows);
        XRCCTRL(*this,"Elf2KAddressText1",wxStaticText)->Enable(elfConfiguration[ELF2K].useElfControlWindows);
		XRCCTRL(*this,"Elf2KAddressText2",wxStaticText)->Enable(elfConfiguration[ELF2K].useElfControlWindows);
    }
	if (runningComputer_ == ELF2K)
		p_Elf2K->Show(elfConfiguration[ELF2K].useElfControlWindows);
}

void GuiElf2K::setElf2KKeyboard(int Selection)
{
	elfConfiguration[ELF2K].keyboardType = Selection;

	switch(Selection)
	{
		case KEYBOARDNONE:
			elfConfiguration[ELF2K].useHexKeyboardEf3 = false;
			elfConfiguration[ELF2K].useKeyboard = false;
			elfConfiguration[ELF2K].UsePS2 = false;
			elfConfiguration[ELF2K].usePs2gpio = false;
			if (mode_.gui)
			{
				XRCCTRL(*this,"KeyFileButtonElf2K", wxButton)->Enable(false);
				XRCCTRL(*this,"KeyFileElf2K", wxTextCtrl)->Enable(false);
				XRCCTRL(*this,"EjectKeyFileElf2K", wxBitmapButton)->Enable(false);
			}
		break;
		case KEYBOARDELF2KHEX:
			elfConfiguration[ELF2K].useHexKeyboardEf3 = true;
			elfConfiguration[ELF2K].useKeyboard = false;
			elfConfiguration[ELF2K].UsePS2 = false;
			elfConfiguration[ELF2K].usePs2gpio = false;
			if (mode_.gui)
			{
				XRCCTRL(*this,"KeyFileButtonElf2K", wxButton)->Enable(false);
				XRCCTRL(*this,"KeyFileElf2K", wxTextCtrl)->Enable(false);
				XRCCTRL(*this,"EjectKeyFileElf2K", wxBitmapButton)->Enable(false);
			}
		break;
		case KEYBOARD_PS2:
			elfConfiguration[ELF2K].useHexKeyboardEf3 = false;
			elfConfiguration[ELF2K].useKeyboard = false;
			elfConfiguration[ELF2K].UsePS2 = false;
			elfConfiguration[ELF2K].usePs2gpio = true;
			if (mode_.gui)
			{
				XRCCTRL(*this,"KeyFileButtonElf2K", wxButton)->Enable(true);
				XRCCTRL(*this,"KeyFileElf2K", wxTextCtrl)->Enable(true);
				XRCCTRL(*this,"EjectKeyFileElf2K", wxBitmapButton)->Enable(true);
			}
		break;
		case KEYBOARD2KPS2GPIOJP4:
			elfConfiguration[ELF2K].useHexKeyboardEf3 = false;
			elfConfiguration[ELF2K].useKeyboard = false;
			elfConfiguration[ELF2K].UsePS2 = true;
			elfConfiguration[ELF2K].usePs2gpio = true;
			if (mode_.gui)
			{
				XRCCTRL(*this,"KeyFileButtonElf2K", wxButton)->Enable(true);
				XRCCTRL(*this,"KeyFileElf2K", wxTextCtrl)->Enable(true);
				XRCCTRL(*this,"EjectKeyFileElf2K", wxBitmapButton)->Enable(true);
			}
		break;
	}
}

void GuiElf2K::setElf2KVideoType(int Selection)
{
	conf[ELF2K].videoMode_ = Selection;
	switch(Selection)
	{
		case VIDEONONE:
			elfConfiguration[ELF2K].usePixie = false;
			elfConfiguration[ELF2K].use6845 = false;
			elfConfiguration[ELF2K].useS100 = false;
			elfConfiguration[ELF2K].use6847 = false;
			elfConfiguration[ELF2K].useTMS9918 = false;
			elfConfiguration[ELF2K].use8275 = false;
			if (mode_.gui)
			{
                XRCCTRL(*this, "ZoomTextElf2K", wxStaticText)->Enable(false);
				XRCCTRL(*this, "ZoomSpinElf2K", wxSpinButton)->Enable(false);
				XRCCTRL(*this, "ZoomValueElf2K", wxTextCtrl)->Enable(false);
				XRCCTRL(*this, "InterlaceElf2K", wxCheckBox)->Enable(false);
				XRCCTRL(*this,"CharRomButtonElf2K", wxButton)->Enable(false);
				XRCCTRL(*this,"CharRomElf2K", wxComboBox)->Enable(false);
                if (clockTextCtrl[ELF2K] != NULL)
                    clockTextCtrl[ELF2K]->ChangeValue(elf2K8275Clock_);
            }
            if (!elfConfiguration[ELF2K].useHexModem && !elfConfiguration[ELF2K].useXmodem)
                elfConfiguration[ELF2K].useXmodem = true;
		break;

		case VIDEOPIXIE:
			elfConfiguration[ELF2K].usePixie = true;
			elfConfiguration[ELF2K].use6845 = false;
			elfConfiguration[ELF2K].useS100 = false;
			elfConfiguration[ELF2K].use6847 = false;
			elfConfiguration[ELF2K].useTMS9918 = false;
			elfConfiguration[ELF2K].use8275 = false;
			if (mode_.gui)
			{
                XRCCTRL(*this, "ZoomTextElf2K", wxStaticText)->Enable(true);
				XRCCTRL(*this, "ZoomSpinElf2K", wxSpinButton)->Enable(true);
				XRCCTRL(*this, "ZoomValueElf2K", wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "InterlaceElf2K", wxCheckBox)->Enable(false);
				XRCCTRL(*this,"CharRomButtonElf2K", wxButton)->Enable(false);
				XRCCTRL(*this,"CharRomElf2K", wxComboBox)->Enable(false);
                if (clockTextCtrl[ELF2K] != NULL)
                    clockTextCtrl[ELF2K]->ChangeValue(elf2KPixieClock_);
            }
            if (!elfConfiguration[ELF2K].useHexModem && !elfConfiguration[ELF2K].useXmodem)
                elfConfiguration[ELF2K].useXmodem = true;
		break;

		case VIDEO2KI8275:
			elfConfiguration[ELF2K].usePixie = false;
			elfConfiguration[ELF2K].use6845 = false;
			elfConfiguration[ELF2K].useS100 = false;
			elfConfiguration[ELF2K].use6847 = false;
			elfConfiguration[ELF2K].useTMS9918 = false;
			elfConfiguration[ELF2K].use8275 = true;
			elfConfiguration[ELF2K].vtType = VTNONE;
			if (mode_.gui)
			{
                XRCCTRL(*this, "ZoomTextElf2K", wxStaticText)->Enable(true);
				XRCCTRL(*this, "VTTypeElf2K", wxChoice)->SetSelection(0);
				elfConfiguration[ELF2K].vtExternal = false;
				XRCCTRL(*this, "ZoomSpinElf2K", wxSpinButton)->Enable(true);
				XRCCTRL(*this, "ZoomValueElf2K", wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "InterlaceElf2K", wxCheckBox)->Enable(true);
				XRCCTRL(*this,"CharRomButtonElf2K", wxButton)->Enable(true);
				XRCCTRL(*this,"CharRomElf2K", wxComboBox)->Enable(true);
                if (clockTextCtrl[ELF2K] != NULL)
                    clockTextCtrl[ELF2K]->ChangeValue(elf2K8275Clock_);
            }
            elfConfiguration[ELF2K].useXmodem = false;
            elfConfiguration[ELF2K].useHexModem = false;
		break;
	}

    if (mode_.gui)
    {
        XRCCTRL(*this,"TapeElf2K", wxButton)->Enable(elfConfiguration[ELF2K].useXmodem | elfConfiguration[ELF2K].useHexModem);
        XRCCTRL(*this,"CasButtonElf2K", wxButton)->Enable(elfConfiguration[ELF2K].useXmodem | elfConfiguration[ELF2K].useHexModem);
        XRCCTRL(*this,"WavFileElf2K", wxTextCtrl)->Enable(elfConfiguration[ELF2K].useXmodem | elfConfiguration[ELF2K].useHexModem);
        XRCCTRL(*this,"EjectCasElf2K", wxBitmapButton)->Enable(elfConfiguration[ELF2K].useXmodem | elfConfiguration[ELF2K].useHexModem);
        XRCCTRL(*this,"AutoCasLoadElf2K", wxCheckBox)->Enable(elfConfiguration[ELF2K].useXmodem | elfConfiguration[ELF2K].useHexModem);
        XRCCTRL(*this,"TurboElf2K", wxCheckBox)->Enable(elfConfiguration[ELF2K].useXmodem | elfConfiguration[ELF2K].useHexModem);
        XRCCTRL(*this,"TurboClockElf2K", wxTextCtrl)->Enable(elfConfiguration[ELF2K].useXmodem | elfConfiguration[ELF2K].useHexModem);
        XRCCTRL(*this,"TurboMhzTextElf2K", wxStaticText)->Enable(elfConfiguration[ELF2K].useXmodem | elfConfiguration[ELF2K].useHexModem);
    }
    setVtType("Elf2K", ELF2K, elfConfiguration[ELF2K].vtType, false);
}

wxPoint GuiElf2K::getElf2KswitchPos()
{
	return wxPoint(switchX_, switchY_);
}

void GuiElf2K::setElf2KswitchPos(wxPoint position)
{
	if (!mode_.window_position_fixed)
	{
		switchX_ = -1;
		switchY_ = -1;
	}
	else
	{
		if (position.x > 0)
			switchX_ = position.x;
		if (position.y > 0)
			switchY_ = position.y;
	}
}

void GuiElf2K::onElf2KNvr(wxCommandEvent&event)
{
	elfConfiguration[ELF2K].nvr = event.IsChecked();
	XRCCTRL(*this, "Elf2KClearRam", wxCheckBox)->Enable(event.IsChecked());
}

void GuiElf2K::onElf2KRtc(wxCommandEvent&event)
{
	elfConfiguration[ELF2K].rtc = event.IsChecked();
	XRCCTRL(*this, "Elf2KClearRtc", wxCheckBox)->Enable(event.IsChecked());
}

void GuiElf2K::onElf2KSwitch(wxCommandEvent&event)
{
	elfConfiguration[ELF2K].useSwitch = event.IsChecked();
	if (elfConfiguration[ELF2K].useSwitch)
	{
		elfConfiguration[ELF2K].useHex = false;
		if (mode_.gui)
			XRCCTRL(*this, "Elf2KHex", wxCheckBox)->SetValue(false);
	}
	if (runningComputer_ == ELF2K)
		p_Elf2K->showModules(elfConfiguration[ELF2K].useSwitch, elfConfiguration[ELF2K].useHex);
}

void GuiElf2K::onElf2KHex(wxCommandEvent&event)
{
	elfConfiguration[ELF2K].useHex = event.IsChecked();
	if (elfConfiguration[ELF2K].useHex)
	{
		elfConfiguration[ELF2K].useSwitch = false;
		if (mode_.gui)
			XRCCTRL(*this, "Elf2KSwitch", wxCheckBox)->SetValue(false);
	}
	if (runningComputer_ == ELF2K)
		p_Elf2K->showModules(elfConfiguration[ELF2K].useSwitch, elfConfiguration[ELF2K].useHex);
}

void GuiElf2K::setBaudChoiceElf2K()
{
    XRCCTRL(*this, "VTBaudRTextElf2K", wxStaticText)->Enable(elfConfiguration[ELF2K].useUart);
    XRCCTRL(*this, "VTBaudRChoiceElf2K", wxChoice)->Enable(elfConfiguration[ELF2K].useUart);
}

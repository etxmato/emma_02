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

BEGIN_EVENT_TABLE(GuiMembership, GuiStudio2)

	EVT_TEXT(XRCID("MainRomMembership"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomMembership"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonMembership"), GuiMain::onMainRom1)

	EVT_BUTTON(XRCID("RomMembership"), GuiMembership::onRomEvent)

	EVT_CHOICE(XRCID("VTTypeMembership"), GuiMain::onVT100)
	EVT_SPIN_UP(XRCID("ZoomSpinVtMembership"), GuiMain::onZoomVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtMembership"), GuiMain::onZoomVt)
	EVT_TEXT(XRCID("ZoomValueVtMembership"), GuiMain::onZoomValueVt)
	EVT_BUTTON(XRCID("FullScreenF3Membership"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("ColoursMembership"), Main::onColoursDef)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeMembership"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeMembership"), GuiMain::onVolume)
	EVT_BUTTON(XRCID("ScreenDumpFileButtonMembership"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileMembership"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileMembership"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5Membership"), GuiMain::onScreenDump)
	EVT_BUTTON(XRCID("DP_ButtonMembership"), GuiMain::onDp)
	EVT_BUTTON(XRCID("SaveButtonMembership"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonMembership"), GuiMain::onLoadButton)
	EVT_CHECKBOX(XRCID("StretchDotMembership"), GuiMain::onStretchDot)
	EVT_BUTTON(XRCID("KeyMapMembership"), Main::onHexKeyDef)
	EVT_CHECKBOX(XRCID("AutoBootMembership"), GuiElf::onAutoBoot)
	EVT_BUTTON(XRCID("VtSetupMembership"), GuiMain::onVtSetup)
	EVT_TEXT(XRCID("ShowAddressMembership"), GuiMain::onLedTimer)
	EVT_CHECKBOX(XRCID("ClearRamMembership"), GuiMain::onClearRam)
	EVT_TEXT(XRCID("BootAddressMembership"), GuiMain::onBootAddress)

	EVT_TEXT(XRCID("SaveStartMembership"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndMembership"), GuiMain::onSaveEnd)

	EVT_CHECKBOX(XRCID("ForceUCMembership"), GuiMembership::onMembershipForceUpperCase)
	EVT_CHECKBOX(XRCID("ControlWindowsMembership"), GuiMembership::onMembershipControlWindows)
	EVT_CHOICE(XRCID("RamMembership"), GuiMembership::onRam)
	EVT_CHECKBOX(XRCID("NvrMembership"), GuiMembership::onNvrMembership)

    EVT_CHOICE(XRCID("IoMembership"), GuiMembership::onIo)
    EVT_CHOICE(XRCID("FrontMembership"), GuiMembership::onFront)

    EVT_BUTTON(XRCID("CasLoadMembership"), GuiMain::onTerminalLoad)
    EVT_BUTTON(XRCID("CasSaveMembership"), GuiMain::onTerminalSave)
    EVT_BUTTON(XRCID("CasStopMembership"), GuiMain::onTerminalStop)
	EVT_BUTTON(XRCID("CasButtonMembership"), GuiMain::onTerminalFile)
	EVT_BUTTON(XRCID("EjectCasMembership"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileMembership"), GuiMain::onCassetteText)
	EVT_CHECKBOX(XRCID("AutoCasLoadMembership"), GuiMain::onAutoLoad)

    EVT_CHOICE(XRCID("VTBaudTChoiceMembership"), GuiMembership::onMembershipBaudT)
    EVT_CHOICE(XRCID("VTBaudRChoiceMembership"), GuiMembership::onMembershipBaudR)

END_EVENT_TABLE()

GuiMembership::GuiMembership(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiStudio2(title, pos, size, mode, dataDir, iniDir)
{
	conf[MEMBER].saveStartString_ = "";
	conf[MEMBER].saveEndString_ = "";
}

void GuiMembership::readMembershipConfig()
{
	bool romMode;
	selectedComputer_ = MEMBER;

    configPointer->Read("/Membership/SerialLog", &elfConfiguration[MEMBER].serialLog, false);
	configPointer->Read("/Membership/VtEf", &elfConfiguration[MEMBER].vtEf, true);
	configPointer->Read("/Membership/VtQ", &elfConfiguration[MEMBER].vtQ, true);
	elfConfiguration[MEMBER].bellFrequency_ = (int)configPointer->Read("/Membership/Bell_Frequency", 800);
	elfConfiguration[MEMBER].useUart = false;

    conf[MEMBER].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Membership" + pathSeparator_;

    conf[MEMBER].mainDir_ = readConfigDir("/Dir/Membership/Main", dataDir_ + "Membership" + pathSeparator_);
	conf[MEMBER].romDir_[MAINROM1] = readConfigDir("/Dir/Membership/Main_Rom_File", dataDir_ + "Membership" + pathSeparator_);
	elfConfiguration[MEMBER].vtCharRomDir_ = readConfigDir("/Dir/Membership/Vt_Font_Rom_File", dataDir_ + "Membership" + pathSeparator_);
	conf[MEMBER].ramDir_ = readConfigDir("/Dir/Membership/Software_File", dataDir_ + "Membership" + pathSeparator_);
	conf[MEMBER].screenDumpFileDir_ = readConfigDir("/Dir/Membership/Video_Dump_File", dataDir_ + "Membership" + pathSeparator_);
    conf[MEMBER].wavFileDir_[0] = readConfigDir("/Dir/Membership/Terminal_File", dataDir_ + "Membership" + pathSeparator_);
	elfConfiguration[MEMBER].vtWavFileDir_ = readConfigDir("/Dir/Membership/Vt_Wav_File", dataDir_ + "Membership" + pathSeparator_);

	elfConfiguration[MEMBER].vtType = (int)configPointer->Read("/Membership/VT_Type", 2l);
    elfConfiguration[MEMBER].vt52SetUpFeature_ = configPointer->Read("/Membership/VT52Setup", 0x00004092l);
    elfConfiguration[MEMBER].vt100SetUpFeature_ = configPointer->Read("/Membership/VT100Setup", 0x0000ca52l);
    elfConfiguration[MEMBER].vtExternalSetUpFeature_ = configPointer->Read("/Membership/VTExternalSetup", 0x0000ca52l);

    elfConfiguration[MEMBER].vtCharactersPerRow = (int)configPointer->Read("/Membership/VT100CharPerRow", 80);
    elfConfiguration[MEMBER].vt100CharWidth = (int)configPointer->Read("/Membership/VT100CharWidth", 10);
    elfConfiguration[MEMBER].vt52CharWidth = (int)configPointer->Read("/Membership/VT52CharWidth", 9);

	configPointer->Read("/Membership/Load_Mode_Rom", &romMode, true);
	if (romMode)
		loadromMode_ = RAM;
	else
		loadromMode_ = ROM;
	onRom();

	long value; 
	wxString bootAddress = configPointer->Read("/Membership/Boot_Address", "0000");
	if (!bootAddress.ToLong(&value, 16))
		value = 0;
	conf[MEMBER].bootAddress_ = value;
	conf[MEMBER].ramType_ = (int)configPointer->Read("/Membership/Ram_Type", 5l);

	conf[MEMBER].rom_[MAINROM1] = configPointer->Read("/Membership/Main_Rom_File", "MCSMP20B.bin");
	conf[MEMBER].screenDumpFile_ = configPointer->Read("/Membership/Video_Dump_File", "screendump.png");
    conf[MEMBER].wavFile_[0] = configPointer->Read("/Membership/Terminal_File", "");
	elfConfiguration[MEMBER].vtWavFile_ = configPointer->Read("/Membership/Vt_Wav_File", "");
    elfConfiguration[MEMBER].serialPort_ = configPointer->Read("/Membership/VtSerialPortChoice", "");

	conf[MEMBER].volume_ = (int)configPointer->Read("/Membership/Volume", 25l);

	elfConfiguration[MEMBER].clearRam = false;
	elfConfiguration[MEMBER].baudR = (int)configPointer->Read("/Membership/Vt_Baud_Receive", 2l);
	elfConfiguration[MEMBER].baudT = (int)configPointer->Read("/Membership/Vt_Baud_Transmit", 2l);

	configPointer->Read("/Membership/Open_Control_Windows", &elfConfiguration[MEMBER].useElfControlWindows, true);
	configPointer->Read("/Membership/Force_Uppercase", &elfConfiguration[MEMBER].forceUpperCase, true);
	configPointer->Read("/Membership/Enable_Auto_Boot", &elfConfiguration[MEMBER].autoBoot, true);
    configPointer->Read("/Membership/Enable_Vt_Stretch_Dot", &conf[MEMBER].stretchDot_, false);
    configPointer->Read("/Membership/Enable_Vt_External", &elfConfiguration[MEMBER].vtExternal, false);
	configPointer->Read("/Membership/Use_Non_Volatile_Ram", &elfConfiguration[MEMBER].nvr, true);
    elfConfiguration[MEMBER].ioType = (int)configPointer->Read("/Membership/IO_Type", IO_TYPE_N2);
    elfConfiguration[MEMBER].frontType = (int)configPointer->Read("/Membership/Front_Type", FRONT_TYPE_J);

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 1.0);
	conf[MEMBER].zoomVt_ = convertLocale(configPointer->Read("/Membership/Vt_Zoom", defaultZoom));
	wxString defaultClock;
	defaultClock.Printf("%1.2f", 1.75);
	wxString defaultTimer;
	defaultTimer.Printf("%d", 100);
	conf[MEMBER].ledTime_ = configPointer->Read("/Membership/Led_Update_Frequency", defaultTimer);
    conf[MEMBER].beepFrequency_ = (int)configPointer->Read("/Membership/Beep_Frequency", 250);

	conf[MEMBER].clock_ = convertLocale(configPointer->Read("/Membership/Clock_Speed", defaultClock));

	configPointer->Read("/Membership/Enable_Auto_Cassette", &conf[MEMBER].autoCassetteLoad_, true);
    conf[MEMBER].realCassetteLoad_ = false;

	setVtType("Membership", MEMBER, elfConfiguration[MEMBER].vtType, false);
	elfConfiguration[MEMBER].vtCharRom_ = configPointer->Read("/Membership/Vt_Font_Rom_File", "vt100.bin");

    conf[MEMBER].saveStartString_ = configPointer->Read("/Membership/SaveStart", "0");
    if (!conf[MEMBER].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[MEMBER].saveStart_ = value;

	if (mode_.gui)
	{
		if (conf[MEMBER].ramType_ == 5 || conf[MEMBER].ramType_ == 6)
			XRCCTRL(*this, "RomMembership", wxButton)->Enable(false);
		else
			XRCCTRL(*this, "RomMembership", wxButton)->Enable(true);

		XRCCTRL(*this, "MainRomMembership", wxComboBox)->SetValue(conf[MEMBER].rom_[MAINROM1]);
		XRCCTRL(*this, "ScreenDumpFileMembership", wxComboBox)->SetValue(conf[MEMBER].screenDumpFile_);
        XRCCTRL(*this, "WavFileMembership", wxTextCtrl)->SetValue(conf[MEMBER].wavFile_[0]);
		XRCCTRL(*this, "AutoCasLoadMembership", wxCheckBox)->SetValue(conf[MEMBER].autoCassetteLoad_);

        if (elfConfiguration[MEMBER].vtExternal)
            XRCCTRL(*this, "VTTypeMembership", wxChoice)->SetSelection(EXTERNAL_TERMINAL);
        else
            XRCCTRL(*this, "VTTypeMembership", wxChoice)->SetSelection(elfConfiguration[MEMBER].vtType);

		XRCCTRL(*this, "VTBaudRChoiceMembership", wxChoice)->SetSelection(elfConfiguration[MEMBER].baudR);
		XRCCTRL(*this, "VTBaudTChoiceMembership", wxChoice)->SetSelection(elfConfiguration[MEMBER].baudT);
        XRCCTRL(*this,"AddressText1Membership",wxStaticText)->Enable(elfConfiguration[MEMBER].useElfControlWindows);
        XRCCTRL(*this,"AddressText2Membership",wxStaticText)->Enable(elfConfiguration[MEMBER].useElfControlWindows);

		XRCCTRL(*this, "ForceUCMembership", wxCheckBox)->SetValue(elfConfiguration[MEMBER].forceUpperCase);
		XRCCTRL(*this, "AutoBootMembership", wxCheckBox)->SetValue(elfConfiguration[MEMBER].autoBoot);
		XRCCTRL(*this, "BootAddressMembership", wxTextCtrl)->SetValue(bootAddress);
        
        correctZoomVtAndValue(MEMBER, "Membership", SET_SPIN);

		XRCCTRL(*this, "ControlWindowsMembership", wxCheckBox)->SetValue(elfConfiguration[MEMBER].useElfControlWindows);
        XRCCTRL(*this, "StretchDotMembership", wxCheckBox)->SetValue(conf[MEMBER].stretchDot_);
		XRCCTRL(*this, "RamMembership", wxChoice)->SetSelection(conf[MEMBER].ramType_);
        XRCCTRL(*this, "IoMembership", wxChoice)->SetSelection(elfConfiguration[MEMBER].ioType);
        XRCCTRL(*this, "FrontMembership", wxChoice)->SetSelection(elfConfiguration[MEMBER].frontType);
		XRCCTRL(*this, "VolumeMembership", wxSlider)->SetValue(conf[MEMBER].volume_);
        if (clockTextCtrl[MEMBER] != NULL)
            clockTextCtrl[MEMBER]->ChangeValue(conf[MEMBER].clock_);
		XRCCTRL(*this, "NvrMembership", wxCheckBox)->SetValue(elfConfiguration[MEMBER].nvr);
		XRCCTRL(*this, "ClearRamMembership", wxCheckBox)->Enable(elfConfiguration[MEMBER].nvr);
		XRCCTRL(*this, "ClearRamMembership", wxCheckBox)->SetValue(elfConfiguration[MEMBER].clearRam);
		XRCCTRL(*this, "ShowAddressMembership", wxTextCtrl)->ChangeValue(conf[MEMBER].ledTime_);
		XRCCTRL(*this,"ShowAddressMembership",wxTextCtrl)->Enable(elfConfiguration[MEMBER].useElfControlWindows);
        if (conf[MEMBER].saveStart_ != 0)
            XRCCTRL(*this, "SaveStartMembership", wxTextCtrl)->SetValue(conf[MEMBER].saveStartString_);
	}

	elfConfiguration[MEMBER].usePortExtender = false;
	elfConfiguration[MEMBER].ideEnabled = false;
	elfConfiguration[MEMBER].fdcEnabled = false;
	elfConfiguration[MEMBER].useLedModule = false;
	elfConfiguration[MEMBER].useTape = false;
}

void GuiMembership::writeMembershipDirConfig()
{
    writeConfigDir("/Dir/Membership/Main", conf[MEMBER].mainDir_);
    writeConfigDir("/Dir/Membership/Main_Rom_File", conf[MEMBER].romDir_[MAINROM1]);
    writeConfigDir("/Dir/Membership/Software_File", conf[MEMBER].ramDir_);
    writeConfigDir("/Dir/Membership/Vt_Font_Rom_File", elfConfiguration[MEMBER].vtCharRomDir_);
    writeConfigDir("/Dir/Membership/Video_Dump_File", conf[MEMBER].screenDumpFileDir_);
    writeConfigDir("/Dir/Membership/Terminal_File", conf[MEMBER].wavFileDir_[0]);
	writeConfigDir("/Dir/Membership/Vt_Wav_File", elfConfiguration[MEMBER].vtWavFileDir_);
}

void GuiMembership::writeMembershipConfig()
{
	wxString buffer;

	configPointer->Write("/Membership/Main_Rom_File", conf[MEMBER].rom_[MAINROM1]);
	configPointer->Write("/Membership/Vt_Font_Rom_File", elfConfiguration[MEMBER].vtCharRom_);
	configPointer->Write("/Membership/Video_Dump_File", conf[MEMBER].screenDumpFile_);
    configPointer->Write("/Membership/Terminal_File", conf[MEMBER].wavFile_[0]);
    configPointer->Write("/Membership/Vt_Wav_File", elfConfiguration[MEMBER].vtWavFile_);
    configPointer->Write("/Membership/VtSerialPortChoice", elfConfiguration[MEMBER].serialPort_);

    configPointer->Write("/Membership/SerialLog", elfConfiguration[MEMBER].serialLog);
	configPointer->Write("/Membership/Load_Mode_Rom", (loadromMode_ == ROM));
	configPointer->Write("/Membership/VtEf", elfConfiguration[MEMBER].vtEf);
	configPointer->Write("/Membership/VtQ", elfConfiguration[MEMBER].vtQ);
	configPointer->Write("/Membership/Bell_Frequency", elfConfiguration[MEMBER].bellFrequency_);
    if (elfConfiguration[MEMBER].vtExternal)
        configPointer->Write("/Membership/VT_Type", VTNONE);
    else
        configPointer->Write("/Membership/VT_Type", elfConfiguration[MEMBER].vtType);
    
    long value = elfConfiguration[MEMBER].vt52SetUpFeature_.to_ulong();
    configPointer->Write("/Membership/VT52Setup", value);
    value = elfConfiguration[MEMBER].vt100SetUpFeature_.to_ulong();
    configPointer->Write("/Membership/VT100Setup", value);
    value = elfConfiguration[MEMBER].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("/Membership/VTExternalSetup", value);
    configPointer->Write("/Membership/VT100CharPerRow", elfConfiguration[MEMBER].vtCharactersPerRow);
    configPointer->Write("/Membership/VT100CharWidth", elfConfiguration[MEMBER].vt100CharWidth);
    configPointer->Write("/Membership/VT52CharWidth", elfConfiguration[MEMBER].vt52CharWidth);

	configPointer->Write("/Membership/Vt_Baud_Receive", elfConfiguration[MEMBER].baudR);
	configPointer->Write("/Membership/Vt_Baud_Transmit", elfConfiguration[MEMBER].baudT);
	configPointer->Write("/Membership/Enable_Auto_Boot", elfConfiguration[MEMBER].autoBoot);
	buffer.Printf("%04X", (unsigned int)conf[MEMBER].bootAddress_);
	configPointer->Write("/Membership/Boot_Address", buffer);
	configPointer->Write("/Membership/Zoom", conf[MEMBER].zoom_);
	configPointer->Write("/Membership/Vt_Zoom", conf[MEMBER].zoomVt_);
	configPointer->Write("/Membership/Force_Uppercase", elfConfiguration[MEMBER].forceUpperCase);
	configPointer->Write("/Membership/Open_Control_Windows", elfConfiguration[MEMBER].useElfControlWindows);
    configPointer->Write("/Membership/Enable_Vt_Stretch_Dot", conf[MEMBER].stretchDot_);
    configPointer->Write("/Membership/Enable_Vt_External", elfConfiguration[MEMBER].vtExternal);
	configPointer->Write("/Membership/Ram_Type", conf[MEMBER].ramType_);
	configPointer->Write("/Membership/Volume", conf[MEMBER].volume_);
	configPointer->Write("/Membership/Use_Non_Volatile_Ram", elfConfiguration[MEMBER].nvr);
	configPointer->Write("/Membership/Led_Update_Frequency", conf[MEMBER].ledTime_);
	configPointer->Write("/Membership/Enable_Auto_Cassette", conf[MEMBER].autoCassetteLoad_);
    configPointer->Write("/Membership/IO_Type", elfConfiguration[MEMBER].ioType);
    configPointer->Write("/Membership/Front_Type", elfConfiguration[MEMBER].frontType);
    configPointer->Write("/Membership/SaveStart", conf[MEMBER].saveStartString_);

    configPointer->Write("/Membership/Clock_Speed", conf[MEMBER].clock_);
    configPointer->Write("/Membership/Beep_Frequency", conf[MEMBER].beepFrequency_);
}

void GuiMembership::readMembershipWindowConfig()
{
    conf[MEMBER].vtX_ = (int)configPointer->Read("/Membership/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[MEMBER].vtY_ = (int)configPointer->Read("/Membership/Window_Position_Vt_Y", mainWindowY_);
    conf[MEMBER].mainX_ = (int)configPointer->Read("/Membership/Window_Position_X", mainWindowX_);
    conf[MEMBER].mainY_ = (int)configPointer->Read("/Membership/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiMembership::writeMembershipWindowConfig()
{
    if (conf[MEMBER].vtX_ > 0)
        configPointer->Write("/Membership/Window_Position_Vt_X", conf[MEMBER].vtX_);
    if (conf[MEMBER].vtY_ > 0)
        configPointer->Write("/Membership/Window_Position_Vt_Y", conf[MEMBER].vtY_);
    if (conf[MEMBER].mainX_ > 0)
        configPointer->Write("/Membership/Window_Position_X", conf[MEMBER].mainX_);
    if (conf[MEMBER].mainY_ > 0)
        configPointer->Write("/Membership/Window_Position_Y", conf[MEMBER].mainY_);
}

void GuiMembership::onMembershipBaudR(wxCommandEvent&event)
{
	elfConfiguration[MEMBER].baudR = event.GetSelection();
}

void GuiMembership::onMembershipBaudT(wxCommandEvent&event)
{
	elfConfiguration[MEMBER].baudT = event.GetSelection();
	if (!elfConfiguration[MEMBER].useUart)
	{
		elfConfiguration[MEMBER].baudR = event.GetSelection();
		XRCCTRL(*this, "VTBaudRChoiceMembership", wxChoice)->SetSelection(elfConfiguration[MEMBER].baudR);
	}
}

void GuiMembership::onMembershipForceUpperCase(wxCommandEvent&event)
{
	elfConfiguration[MEMBER].forceUpperCase = event.IsChecked();
	if (runningComputer_ == MEMBER)
	{
		p_Computer->setForceUpperCase(event.IsChecked());
	}
}

void GuiMembership::onMembershipControlWindows(wxCommandEvent&event)
{
	elfConfiguration[MEMBER].useElfControlWindows = event.IsChecked();
	XRCCTRL(*this,"ShowAddressMembership",wxTextCtrl)->Enable(elfConfiguration[MEMBER].useElfControlWindows);
    XRCCTRL(*this,"AddressText1Membership",wxStaticText)->Enable(elfConfiguration[MEMBER].useElfControlWindows);
	XRCCTRL(*this,"AddressText2Membership",wxStaticText)->Enable(elfConfiguration[MEMBER].useElfControlWindows);
    if (runningComputer_ == MEMBER)
		p_Membership->Show(elfConfiguration[MEMBER].useElfControlWindows);
}

void GuiMembership::onRam(wxCommandEvent&event)
{
	conf[MEMBER].ramType_ = event.GetSelection();

	XRCCTRL(*this, "ClearRamMembership", wxCheckBox)->SetValue(true);
	elfConfiguration[MEMBER].clearRam = true;
    
    switch (conf[MEMBER].ramType_)
    {
        case 5:
		case 6:
			loadromMode_ = ROM;
            if (mode_.gui)
            {
                XRCCTRL(*this, "RomButtonMembership", wxButton)->SetLabel("ROM");
                XRCCTRL(*this, "RomButtonMembership", wxButton)->SetToolTip("Browse for Membership Card ROM file");
				XRCCTRL(*this, "RomMembership", wxButton)->Enable(false);
			}
        break;
            
        default:
            loadromMode_ = RAM;
            if (mode_.gui)
            {
                XRCCTRL(*this, "RomButtonMembership", wxButton)->SetLabel("RAM");
                XRCCTRL(*this, "RomButtonMembership", wxButton)->SetToolTip("Browse for Membership Card RAM file");
				XRCCTRL(*this, "RomMembership", wxButton)->Enable(true);
			}
            
        break;
    }
}

void GuiMembership::onIo(wxCommandEvent&event)
{
    elfConfiguration[MEMBER].ioType = event.GetSelection();
}

void GuiMembership::onFront(wxCommandEvent&event)
{
    elfConfiguration[MEMBER].frontType = event.GetSelection();
    elfConfiguration[MEMBER].vtQ = (elfConfiguration[MEMBER].frontType == FRONT_TYPE_J);
}

void GuiMembership::onRomEvent(wxCommandEvent&WXUNUSED(event))
{
	onRom();
}

void GuiMembership::onRom()
{
	if (loadromMode_ == ROM)
	{
		loadromMode_ = RAM;
		if (mode_.gui)
		{
			XRCCTRL(*this, "RomButtonMembership", wxButton)->SetLabel("RAM");
			XRCCTRL(*this, "RomButtonMembership", wxButton)->SetToolTip("Browse for Membership Card RAM file");
		}
	}
	else
	{
		loadromMode_ = ROM;
		if (mode_.gui)
		{
			XRCCTRL(*this, "RomButtonMembership", wxButton)->SetLabel("ROM");
			XRCCTRL(*this, "RomButtonMembership", wxButton)->SetToolTip("Browse for Membership Card ROM file");
		}
	}
}

int GuiMembership::getLoadromModeMembership()
{
	return loadromMode_;
}

void GuiMembership::onNvrMembership(wxCommandEvent&event)
{
	elfConfiguration[MEMBER].nvr = event.IsChecked();
	XRCCTRL(*this, "ClearRamMembership", wxCheckBox)->Enable(event.IsChecked());
}

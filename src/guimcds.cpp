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
#include "guimcds.h"

BEGIN_EVENT_TABLE(GuiMcds, GuiCosmicos)

	EVT_TEXT(XRCID("MainRomMcds"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomMcds"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonMcds"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("MainRom2Mcds"), GuiMain::onMainRom2Text)
	EVT_COMBOBOX(XRCID("MainRom2Mcds"), GuiMain::onMainRom2Text)
	EVT_BUTTON(XRCID("RomButton2Mcds"), GuiMain::onMainRom2)

	EVT_TEXT(XRCID("MainRom3Mcds"), GuiMain::onMainRom3Text)
	EVT_COMBOBOX(XRCID("MainRom3Mcds"), GuiMain::onMainRom3Text)
	EVT_BUTTON(XRCID("RomButton3Mcds"), GuiMain::onMainRom3)

	EVT_BUTTON(XRCID("CasButtonMcds"), GuiMain::onCassette)
    EVT_BUTTON(XRCID("EjectCasMcds"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileMcds"), GuiMain::onCassetteText)

    EVT_TEXT(XRCID("VtCharRomMcds"), GuiMain::onVtCharRomText)
	EVT_COMBOBOX(XRCID("VtCharRomMcds"), GuiMain::onVtCharRomText)
	EVT_BUTTON(XRCID("VtCharRomButtonMcds"), GuiMain::onVtCharRom)

	EVT_TEXT(XRCID("KeyFileMcds"), GuiMain::onKeyFileText)
	EVT_BUTTON(XRCID("KeyFileButtonMcds"), GuiMain::onKeyFile)
	EVT_BUTTON(XRCID("EjectKeyFileMcds"), GuiMain::onKeyFileEject)

    EVT_TEXT(XRCID("PrintFileMcds"), GuiMain::onPrintFileText)
    EVT_BUTTON(XRCID("PrintButtonMcds"), GuiMain::onPrintButton)
    EVT_CHOICE(XRCID("PrintModeMcds"), GuiMain::onPrintMode)
    EVT_BUTTON(XRCID("PrintFileButtonMcds"), GuiMain::onPrintFile)

    EVT_COMMAND(wxID_ANY, OPEN_PRINTER_WINDOW, GuiMain::openPrinterFrame)

//    EVT_CHECKBOX(XRCID("VtExternalMcds"), GuiMain::onVtExternal)
    EVT_CHOICE(XRCID("VTTypeMcds"), GuiMain::onVT100)
	EVT_SPIN_UP(XRCID("ZoomSpinVtMcds"), GuiMain::onZoomUpVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtMcds"), GuiMain::onZoomDownVt)
	EVT_TEXT(XRCID("ZoomValueVtMcds"), GuiMain::onZoomValueVt)
	EVT_BUTTON(XRCID("FullScreenF3Mcds"), GuiMain::onFullScreen)
    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeMcds"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeMcds"), GuiMain::onVolume)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonMcds"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileMcds"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileMcds"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5Mcds"), GuiMain::onScreenDump)

    EVT_CHECKBOX(XRCID("McdsForceUC"), GuiMcds::onMcdsForceUpperCase)
    EVT_BUTTON(XRCID("VtSetupMcds"), GuiMain::onVtSetup)
	EVT_CHECKBOX(XRCID("StretchDotMcds"), GuiMain::onStretchDot)
	EVT_BUTTON(XRCID("ColoursMcds"), Main::onColoursDef)
    EVT_CHECKBOX(XRCID("McdsBootRam"), GuiMcds::onBootRam)

    EVT_BUTTON(XRCID("SaveButtonMcds"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonMcds"), GuiMain::onLoadButton)
    EVT_BUTTON(XRCID("RunButtonMcds"), GuiMain::onLoadRunButton)
    EVT_BUTTON(XRCID("DsaveButtonMcds"), GuiMain::onDataSaveButton)

    EVT_CHECKBOX(XRCID("UseLocationMcds"), GuiMain::onUseLocation)
    EVT_TEXT(XRCID("SaveStartMcds"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndMcds"), GuiMain::onSaveEnd)

    EVT_CHECKBOX(XRCID("TurboMcds"), GuiMain::onTurbo)
    EVT_TEXT(XRCID("TurboClockMcds"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadMcds"), GuiMain::onAutoLoad)
    EVT_BUTTON(XRCID("CasLoadMcds"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasSaveMcds"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasStopMcds"), GuiMain::onCassetteStop)
    EVT_BUTTON(XRCID("RealCasLoadMcds"), GuiMain::onRealCas)

	EVT_CHOICE(XRCID("VTBaudTChoiceMcds"), GuiMcds::onMcdsBaudT)

END_EVENT_TABLE()

GuiMcds::GuiMcds(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode_, wxString dataDir, wxString iniDir)
: GuiCosmicos(title, pos, size, mode_, dataDir, iniDir)
{
    conf[MCDS].loadFileNameFull_ = "";
    conf[MCDS].loadFileName_ = "";
    
    conf[MCDS].pLoadSaveName_[0] = 'S';
    conf[MCDS].pLoadSaveName_[1] = 'U';
    conf[MCDS].pLoadSaveName_[2] = 'P';
    conf[MCDS].pLoadSaveName_[3] = 'E';
    
    conf[MCDS].defus_ = 0x81;
    conf[MCDS].eop_ = 0x83;
    conf[MCDS].string_ = 0x92;
    conf[MCDS].arrayValue_ = 0x94;
    conf[MCDS].eod_ = 0x99;
    conf[MCDS].basicRamAddress_ = 0x200;
    
    conf[MCDS].saveStartString_ = "";
    conf[MCDS].saveEndString_ = "";
    conf[MCDS].saveExecString_ = "";

    position_.x = 0;
}

void GuiMcds::readMcdsConfig()
{
	selectedComputer_ = MCDS;

	conf[MCDS].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "MCDS" + pathSeparator_;

    conf[MCDS].mainDir_ = readConfigDir("/Dir/Mcds/Main", dataDir_ + "MCDS" + pathSeparator_);
	conf[MCDS].romDir_[MAINROM1] = readConfigDir("/Dir/Mcds/Main_Rom_File1", dataDir_ + "MCDS" + pathSeparator_);
	conf[MCDS].romDir_[MAINROM2] = readConfigDir("/Dir/Mcds/Main_Rom_File2", dataDir_ + "MCDS" + pathSeparator_);
	conf[MCDS].romDir_[MAINROM3] = readConfigDir("/Dir/Mcds/Main_Rom_File3", dataDir_ + "MCDS" + pathSeparator_);
	conf[MCDS].vtCharRomDir_ = readConfigDir("/Dir/Mcds/Vt_Font_Rom_File", dataDir_ + "MCDS" + pathSeparator_);
    floppyDirMcds_[0] = readConfigDir("/Dir/Mcds/FDC0_File", dataDir_ + "MCDS" + pathSeparator_);
    floppyDirMcds_[1] = readConfigDir("/Dir/Mcds/FDC1_File", dataDir_ + "MCDS" + pathSeparator_);
    floppyDirMcds_[2] = readConfigDir("/Dir/Mcds/FDC2_File", dataDir_ + "MCDS" + pathSeparator_);
    floppyDirMcds_[3] = readConfigDir("/Dir/Mcds/FDC3_File", dataDir_ + "MCDS" + pathSeparator_);
    floppyDirSwitchedMcds_[0] = readConfigDir("/Dir/Mcds/FDC0_File_Switched", dataDir_ + "MCDS" + pathSeparator_ + "Software" + pathSeparator_);
    floppyDirSwitchedMcds_[1] = readConfigDir("/Dir/Mcds/FDC1_File_Switched", dataDir_ + "MCDS" + pathSeparator_ + "Software" + pathSeparator_);
    floppyDirSwitchedMcds_[2] = readConfigDir("/Dir/Mcds/FDC2_File_Switched", dataDir_ + "MCDS" + pathSeparator_ + "PLM" + pathSeparator_);
    floppyDirSwitchedMcds_[3] = readConfigDir("/Dir/Mcds/FDC3_File_Switched", dataDir_ + "MCDS" + pathSeparator_ + "BASIC" + pathSeparator_);
    conf[MCDS].keyFileDir_ = readConfigDir("/Dir/Mcds/Key_File", dataDir_ + "MCDS" + pathSeparator_);
    conf[MCDS].printFileDir_ = readConfigDir("Dir/Mcds/Print_File", dataDir_ + "MCDS" + pathSeparator_);
	conf[MCDS].screenDumpFileDir_ = readConfigDir("/Dir/Mcds/Video_Dump_File", dataDir_ + "MCDS" + pathSeparator_);
    conf[MCDS].wavFileDir_ = readConfigDir("/Dir/Mcds/Wav_File", dataDir_ + "MCDS" + pathSeparator_);
    
	conf[MCDS].rom_[MAINROM1] = configPointer->Read("/Mcds/Main_Rom_File1", "ut62_rev2.bin");
	conf[MCDS].rom_[MAINROM2] = configPointer->Read("/Mcds/Main_Rom_File2", "edit_asm.bin");
	conf[MCDS].rom_[MAINROM3] = configPointer->Read("/Mcds/Main_Rom_File3", "basic_3.bin");
	floppyMcds_[0] = configPointer->Read("/Mcds/FDC0_File", "microdos.img");
    floppyMcds_[1] = configPointer->Read("/Mcds/FDC1_File", "PLM 1800.img");
    floppyMcds_[2] = configPointer->Read("/Mcds/FDC2_File", "basic1.img");
    floppyMcds_[3] = configPointer->Read("/Mcds/FDC3_File", "");
    conf[MCDS].keyFile_ = configPointer->Read("/Mcds/Key_File", "");
    conf[MCDS].printFile_ = configPointer->Read("/Mcds/Print_File", "printerout.txt");
    conf[MCDS].screenDumpFile_ = configPointer->Read("/Mcds/Video_Dump_File", "screendump.png");
    conf[MCDS].wavFile_ = configPointer->Read("/Mcds/Wav_File", "");
    conf[MCDS].volume_ = (int)configPointer->Read("/Mcds/Volume", 25l);
    
	getConfigBool("/Mcds/SerialLog", false);

	configPointer->Read("/Mcds/Enable_Vt_Stretch_Dot", &conf[MCDS].stretchDot_, false);
    configPointer->Read("/Mcds/Enable_Vt_External", &elfConfiguration[MCDS].vtExternal, false);

    elfConfiguration[MCDS].useUart = false; 
    elfConfiguration[MCDS].bellFrequency_ = (int)configPointer->Read("/Mcds/Bell_Frequency", 800);
	elfConfiguration[MCDS].baudR = (int)configPointer->Read("/Mcds/Vt_Baud_Receive", 0l);
	elfConfiguration[MCDS].baudT = (int)configPointer->Read("/Mcds/Vt_Baud_Transmit", 0l);
	elfConfiguration[MCDS].vtType = (int)configPointer->Read("/Mcds/VT_Type", 2l);
    elfConfiguration[MCDS].vt52SetUpFeature_ = configPointer->Read("/Mcds/VT52Setup", 0x00004092l);
    elfConfiguration[MCDS].vt100SetUpFeature_ = configPointer->Read("/Mcds/VT100Setup", 0x0000cad2l);
    elfConfiguration[MCDS].vtExternalSetUpFeature_ = configPointer->Read("/Mcds/VTExternalSetup", 0x0000cad2l);
    elfConfiguration[MCDS].serialPort_ = configPointer->Read("/Mcds/VtSerialPortChoice", "");

    configPointer->Read("/Mcds/Force_Uppercase", &elfConfiguration[MCDS].forceUpperCase, true);
    configPointer->Read("/Mcds/Boot_From_Ram", &elfConfiguration[MCDS].bootRam, false);
    
	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 1.0);
	conf[MCDS].zoomVt_ = configPointer->Read("/Mcds/Vt_Zoom", defaultZoom);
	wxString defaultScale;
	defaultScale.Printf("%i", 3);
	conf[MCDS].xScale_ = configPointer->Read("/Mcds/Window_Scale_Factor_X", defaultScale);

	wxString defaultClock;
	defaultClock.Printf("%1.1f", 2.0);
	conf[MCDS].clock_ = configPointer->Read("/Mcds/Clock_Speed", defaultClock);

    conf[MCDS].turboClock_ = configPointer->Read("/Mcds/Turbo_Clock_Speed", "15");
    conf[MCDS].printMode_ = (int)configPointer->Read("/Mcds/Print_Mode", 1l);
    configPointer->Read("/Mcds/Enable_Turbo_Cassette", &conf[MCDS].turbo_, true);
    configPointer->Read("/Mcds/Enable_Auto_Cassette", &conf[MCDS].autoCassetteLoad_, true);
    configPointer->Read("/Mcds/Enable_Real_Cassette", &conf[MCDS].realCassetteLoad_, false);
	conf[MCDS].useLoadLocation_ = false;

    if (mode_.gui)
		setBaudChoiceMcds();

	setVtType("Mcds", MCDS, elfConfiguration[MCDS].vtType, false);

	conf[MCDS].vtCharRom_ = configPointer->Read("/Mcds/Vt_Font_Rom_File", "vt100.bin");

    configPointer->Read("/Mcds/DirectoryMode_0", &directoryMode_[0], false);
    configPointer->Read("/Mcds/DirectoryMode_1", &directoryMode_[1], false);
    configPointer->Read("/Mcds/DirectoryMode_2", &directoryMode_[2], false);
    configPointer->Read("/Mcds/DirectoryMode_3", &directoryMode_[3], false);

    if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomMcds", wxComboBox)->SetValue(conf[MCDS].rom_[MAINROM1]);
		XRCCTRL(*this, "MainRom2Mcds", wxComboBox)->SetValue(conf[MCDS].rom_[MAINROM2]);
		XRCCTRL(*this, "MainRom3Mcds", wxComboBox)->SetValue(conf[MCDS].rom_[MAINROM3]);
		XRCCTRL(*this, "VtCharRomMcds", wxComboBox)->SetValue(conf[MCDS].vtCharRom_);
        XRCCTRL(*this, "KeyFileMcds", wxTextCtrl)->SetValue(conf[MCDS].keyFile_);
        XRCCTRL(*this, "PrintFileMcds", wxTextCtrl)->SetValue(conf[MCDS].printFile_);
        XRCCTRL(*this, "ScreenDumpFileMcds", wxComboBox)->SetValue(conf[MCDS].screenDumpFile_);
        
        XRCCTRL(*this, "TurboClockMcds", wxTextCtrl)->SetValue(conf[MCDS].turboClock_);
        XRCCTRL(*this, "TurboMcds", wxCheckBox)->SetValue(conf[MCDS].turbo_);
        turboGui("Mcds");
        XRCCTRL(*this, "AutoCasLoadMcds", wxCheckBox)->SetValue(conf[MCDS].autoCassetteLoad_);
        
        XRCCTRL(*this, "PrintModeMcds", wxChoice)->SetSelection((int)configPointer->Read("/Mcds/Print_Mode", 1l));
        setPrintMode();
  
        XRCCTRL(*this, "VTTypeMcds", wxChoice)->SetSelection(elfConfiguration[MCDS].vtType);
        XRCCTRL(*this, "McdsForceUC", wxCheckBox)->SetValue(elfConfiguration[MCDS].forceUpperCase);
        
		XRCCTRL(*this, "VTBaudRChoiceMcds", wxChoice)->SetSelection(elfConfiguration[MCDS].baudR);
		XRCCTRL(*this, "VTBaudTChoiceMcds", wxChoice)->SetSelection(elfConfiguration[MCDS].baudT);
//		XRCCTRL(*this, "VTBaudTTextMcds", wxStaticText)->Enable(elfConfiguration[MCDS].vtType != VTNONE);
//		XRCCTRL(*this, "VTBaudTChoiceMcds", wxChoice)->Enable(elfConfiguration[MCDS].vtType != VTNONE);

//		XRCCTRL(*this, "VtCharRomButtonMcds", wxButton)->Enable(elfConfiguration[MCDS].vtType != VTNONE);
//		XRCCTRL(*this, "VtCharRomMcds", wxComboBox)->Enable(elfConfiguration[MCDS].vtType != VTNONE);
//		XRCCTRL(*this, "VtSetupMcds", wxButton)->Enable(elfConfiguration[MCDS].vtType != VTNONE);
		XRCCTRL(*this, "ZoomValueVtMcds", wxTextCtrl)->ChangeValue(conf[MCDS].zoomVt_);
        XRCCTRL(*this, "McdsBootRam", wxCheckBox)->SetValue(elfConfiguration[MCDS].bootRam);
        
 //       XRCCTRL(*this, "VtExternalMcds", wxCheckBox)->SetValue(elfConfiguration[MCDS].vtExternal);
		XRCCTRL(*this, "StretchDotMcds", wxCheckBox)->SetValue(conf[MCDS].stretchDot_);
        XRCCTRL(*this, "WavFileMcds", wxTextCtrl)->SetValue(conf[MCDS].wavFile_);
        XRCCTRL(*this, "VolumeMcds", wxSlider)->SetValue(conf[MCDS].volume_);
        
        clockTextCtrl[MCDS]->ChangeValue(conf[MCDS].clock_);

        XRCCTRL(*this, "UseLocationMcds", wxCheckBox)->SetValue(conf[MCDS].useLoadLocation_);
    }
}

void GuiMcds::writeMcdsDirConfig()
{
    writeConfigDir("/Dir/Mcds/Main", conf[MCDS].mainDir_);
	writeConfigDir("/Dir/Mcds/Main_Rom_File1", conf[MCDS].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Mcds/Main_Rom_File2", conf[MCDS].romDir_[MAINROM2]);
	writeConfigDir("/Dir/Mcds/Main_Rom_File3", conf[MCDS].romDir_[MAINROM3]);
	writeConfigDir("/Dir/Mcds/Vt_Font_Rom_File", conf[MCDS].vtCharRomDir_);
    writeConfigDir("/Dir/Mcds/FDC0_File", floppyDirMcds_[0]);
    writeConfigDir("/Dir/Mcds/FDC1_File", floppyDirMcds_[1]);
    writeConfigDir("/Dir/Mcds/FDC2_File", floppyDirMcds_[2]);
    writeConfigDir("/Dir/Mcds/FDC3_File", floppyDirMcds_[3]);
    writeConfigDir("/Dir/Mcds/FDC0_File_Switched", floppyDirSwitchedMcds_[0]);
    writeConfigDir("/Dir/Mcds/FDC1_File_Switched", floppyDirSwitchedMcds_[1]);
    writeConfigDir("/Dir/Mcds/FDC2_File_Switched", floppyDirSwitchedMcds_[2]);
    writeConfigDir("/Dir/Mcds/FDC3_File_Switched", floppyDirSwitchedMcds_[3]);
    writeConfigDir("/Dir/Mcds/Key_File", conf[MCDS].keyFileDir_);
    writeConfigDir("/Dir/Mcds/Print_File", conf[MCDS].printFileDir_);
    writeConfigDir("/Dir/Mcds/Video_Dump_File", conf[MCDS].screenDumpFileDir_);
    writeConfigDir("/Dir/Mcds/Wav_File", conf[MCDS].wavFileDir_);
}

void GuiMcds::writeMcdsConfig()
{
    writeElfPortConfig(MCDS, "Mcds");

	configPointer->Write("/Mcds/Main_Rom_File1", conf[MCDS].rom_[MAINROM1]);
	configPointer->Write("/Mcds/Main_Rom_File2", conf[MCDS].rom_[MAINROM2]);
	configPointer->Write("/Mcds/Main_Rom_File3", conf[MCDS].rom_[MAINROM3]);
	configPointer->Write("/Mcds/Vt_Font_Rom_File", conf[MCDS].vtCharRom_);
    configPointer->Write("/Mcds/FDC0_File", floppyMcds_[0]);
    configPointer->Write("/Mcds/FDC1_File", floppyMcds_[1]);
    configPointer->Write("/Mcds/FDC2_File", floppyMcds_[2]);
    configPointer->Write("/Mcds/FDC3_File", floppyMcds_[3]);
    configPointer->Write("/Mcds/Key_File", conf[MCDS].keyFile_);
    configPointer->Write("/Mcds/Print_File", conf[MCDS].printFile_);
    configPointer->Write("/Mcds/Video_Dump_File", conf[MCDS].screenDumpFile_);
    configPointer->Write("/Mcds/Wav_File", conf[MCDS].wavFile_);
    configPointer->Write("/Mcds/VtSerialPortChoice", elfConfiguration[MCDS].serialPort_);

	configPointer->Write("/Mcds/Bell_Frequency", elfConfiguration[MCDS].bellFrequency_);
	configPointer->Write("/Mcds/VT_Type", elfConfiguration[MCDS].vtType);
    
    long value = elfConfiguration[MCDS].vt52SetUpFeature_.to_ulong();
    configPointer->Write("/Mcds/VT52Setup", value);
    value = elfConfiguration[MCDS].vt100SetUpFeature_.to_ulong();
    configPointer->Write("/Mcds/VT100Setup", value);
    value = elfConfiguration[MCDS].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("/Mcds/VTExternalSetup", value);
    
	configPointer->Write("/Mcds/Vt_Baud_Receive", elfConfiguration[MCDS].baudR);
	configPointer->Write("/Mcds/Vt_Baud_Transmit", elfConfiguration[MCDS].baudT);
	configPointer->Write("/Mcds/Vt_Zoom", conf[MCDS].zoomVt_);
    configPointer->Write("/Mcds/Force_Uppercase", elfConfiguration[MCDS].forceUpperCase);
    configPointer->Write("/Mcds/Enable_Vt_Stretch_Dot", conf[MCDS].stretchDot_);
    configPointer->Write("/Mcds/Enable_Vt_External", elfConfiguration[MCDS].vtExternal);
    configPointer->Write("/Mcds/Volume", conf[MCDS].volume_);
    configPointer->Write("/Mcds/Boot_From_Ram", elfConfiguration[MCDS].bootRam);
    
    configPointer->Write("/Mcds/Clock_Speed", conf[MCDS].clock_);
    configPointer->Write("/Mcds/Turbo_Clock_Speed", conf[MCDS].turboClock_);
    configPointer->Write("/Mcds/Enable_Turbo_Cassette", conf[MCDS].turbo_);
    configPointer->Write("/Mcds/Enable_Auto_Cassette", conf[MCDS].autoCassetteLoad_);
    configPointer->Write("/Mcds/Enable_Real_Cassette", conf[MCDS].realCassetteLoad_);
    configPointer->Write("/Mcds/Print_Mode", conf[MCDS].printMode_);
    
    configPointer->Write("/Mcds/DirectoryMode_0", directoryMode_[0]);
    configPointer->Write("/Mcds/DirectoryMode_1", directoryMode_[1]);
    configPointer->Write("/Mcds/DirectoryMode_2", directoryMode_[2]);
    configPointer->Write("/Mcds/DirectoryMode_3", directoryMode_[3]);
}

void GuiMcds::readMcdsWindowConfig()
{
    conf[MCDS].vtX_ = (int)configPointer->Read("/Mcds/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX);
    conf[MCDS].vtY_ = (int)configPointer->Read("/Mcds/Window_Position_Vt_Y", mainWindowY_);
    conf[MCDS].mainX_ = (int)configPointer->Read("/Mcds/Window_Position_X", mainWindowX_);
    conf[MCDS].mainY_ = (int)configPointer->Read("/Mcds/Window_Position_Y", mainWindowY_+windowInfo.mainwY);
}

void GuiMcds::writeMcdsWindowConfig()
{
    if (conf[MCDS].vtX_ > 0)
        configPointer->Write("/Mcds/Window_Position_Vt_X", conf[MCDS].vtX_);
    if (conf[MCDS].vtY_ > 0)
        configPointer->Write("/Mcds/Window_Position_Vt_Y", conf[MCDS].vtY_);
    if (conf[MCDS].mainX_ > 0)
        configPointer->Write("/Mcds/Window_Position_X", conf[MCDS].mainX_);
    if (conf[MCDS].mainY_ > 0)
        configPointer->Write("/Mcds/Window_Position_Y", conf[MCDS].mainY_);
    
}

void GuiMcds::onMcdsBaudT(wxCommandEvent&event)
{
	elfConfiguration[MCDS].baudT = event.GetSelection();
    elfConfiguration[MCDS].baudR = event.GetSelection();
    XRCCTRL(*this, "VTBaudRChoiceMcds", wxChoice)->SetSelection(elfConfiguration[MCDS].baudR);
}

void GuiMcds::onMcdsForceUpperCase(wxCommandEvent&event)
{
    elfConfiguration[MCDS].forceUpperCase = event.IsChecked();
    if (runningComputer_ == MCDS)
    {
        p_Mcds->setForceUpperCase(event.IsChecked());
    }
}

void GuiMcds::onBootRam(wxCommandEvent&event)
{
    elfConfiguration[MCDS].bootRam = event.IsChecked();
	if (runningComputer_ == MCDS)
	{
		p_Mcds->setBootRam(event.IsChecked());
	}
}

void GuiMcds::setBaudChoiceMcds()
{
	wxArrayString choices;
	choices.Add("1200");
	choices.Add("600");
	choices.Add("300");
	choices.Add("200");
	choices.Add("150");
	choices.Add("134");
	choices.Add("110");
	choices.Add("75");
	choices.Add("50");

	XRCCTRL(*this, "VTBaudRTextMcds", wxStaticText)->Enable(false);
	XRCCTRL(*this, "VTBaudRChoiceMcds", wxChoice)->Enable(false);
	XRCCTRL(*this, "VTBaudTChoiceMcds", wxChoice)->Set(choices);
	XRCCTRL(*this, "VTBaudRChoiceMcds", wxChoice)->Set(choices);
}

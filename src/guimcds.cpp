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

	EVT_TEXT(XRCID("MainRomMCDS"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomMCDS"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonMCDS"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("MainRom2MCDS"), GuiMain::onMainRom2Text)
	EVT_COMBOBOX(XRCID("MainRom2MCDS"), GuiMain::onMainRom2Text)
	EVT_BUTTON(XRCID("RomButton2MCDS"), GuiMain::onMainRom2)

	EVT_TEXT(XRCID("MainRom3MCDS"), GuiMain::onMainRom3Text)
	EVT_COMBOBOX(XRCID("MainRom3MCDS"), GuiMain::onMainRom3Text)
	EVT_BUTTON(XRCID("RomButton3MCDS"), GuiMain::onMainRom3)

	EVT_BUTTON(XRCID("CasButtonMCDS"), GuiMain::onCassette)
    EVT_BUTTON(XRCID("EjectCasMCDS"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileMCDS"), GuiMain::onCassetteText)

    EVT_BUTTON(XRCID("CasButton1MCDS"), GuiMain::onCassette1)
    EVT_BUTTON(XRCID("EjectCas1MCDS"), GuiMain::onCassette1Eject)
    EVT_TEXT(XRCID("WavFile1MCDS"), GuiMain::onCassette1Text)

	EVT_TEXT(XRCID("KeyFileMCDS"), GuiMain::onKeyFileText)
	EVT_BUTTON(XRCID("KeyFileButtonMCDS"), GuiMain::onKeyFile)
	EVT_BUTTON(XRCID("EjectKeyFileMCDS"), GuiMain::onKeyFileEject)

    EVT_TEXT(XRCID("PrintFileMCDS"), GuiMain::onPrintFileText)
    EVT_BUTTON(XRCID("PrintButtonMCDS"), GuiMain::onPrintButton)
    EVT_CHOICE(XRCID("PrintModeMCDS"), GuiMain::onPrintMode)
    EVT_BUTTON(XRCID("PrintFileButtonMCDS"), GuiMain::onPrintFile)

    EVT_COMMAND(wxID_ANY, OPEN_PRINTER_WINDOW, GuiMain::openPrinterFrame)

    EVT_CHOICE(XRCID("VTTypeMCDS"), GuiMain::onVT100)
	EVT_SPIN_UP(XRCID("ZoomSpinVtMCDS"), GuiMain::onZoomVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtMCDS"), GuiMain::onZoomVt)
	EVT_TEXT(XRCID("ZoomValueVtMCDS"), GuiMain::onZoomValueVt)
	EVT_BUTTON(XRCID("FullScreenF3MCDS"), GuiMain::onFullScreen)
    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeMCDS"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeMCDS"), GuiMain::onVolume)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonMCDS"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileMCDS"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileMCDS"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5MCDS"), GuiMain::onScreenDump)

    EVT_CHECKBOX(XRCID("McdsForceUC"), GuiMcds::onMcdsForceUpperCase)
    EVT_BUTTON(XRCID("VtSetupMCDS"), GuiMain::onVtSetup)
	EVT_CHECKBOX(XRCID("StretchDotMCDS"), GuiMain::onStretchDot)
	EVT_BUTTON(XRCID("ColoursMCDS"), Main::onColoursDef)
    EVT_CHECKBOX(XRCID("McdsBootRam"), GuiMcds::onBootRam)

    EVT_BUTTON(XRCID("SaveButtonMCDS"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonMCDS"), GuiMain::onLoadButton)
    EVT_BUTTON(XRCID("RunButtonMCDS"), GuiMain::onLoadRunButton)
    EVT_BUTTON(XRCID("DsaveButtonMCDS"), GuiMain::onDataSaveButton)

    EVT_CHECKBOX(XRCID("UseLocationMCDS"), GuiMain::onUseLocation)
    EVT_TEXT(XRCID("SaveStartMCDS"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndMCDS"), GuiMain::onSaveEnd)

    EVT_CHECKBOX(XRCID("TurboMCDS"), GuiMain::onTurbo)
    EVT_TEXT(XRCID("TurboClockMCDS"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadMCDS"), GuiMain::onAutoLoad)
    EVT_BUTTON(XRCID("CasLoadMCDS"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasSaveMCDS"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasStopMCDS"), GuiMain::onCassetteStop)
    EVT_BUTTON(XRCID("RealCasLoadMCDS"), GuiMain::onRealCas)

    EVT_BUTTON(XRCID("CasLoad1MCDS"), GuiMain::onCassetteLoad1)
    EVT_BUTTON(XRCID("CasSave1MCDS"), GuiMain::onCassetteSave1)
    EVT_BUTTON(XRCID("CasStop1MCDS"), GuiMain::onCassetteStop)

	EVT_CHOICE(XRCID("VTBaudTChoiceMCDS"), GuiMcds::onMcdsBaudT)

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
	elfConfiguration[MCDS].vtCharRomDir_ = readConfigDir("/Dir/Mcds/Vt_Font_Rom_File", dataDir_ + "MCDS" + pathSeparator_);
    conf[MCDS].keyFileDir_ = readConfigDir("/Dir/Mcds/Key_File", dataDir_ + "MCDS" + pathSeparator_);
    conf[MCDS].printFileDir_ = readConfigDir("Dir/Mcds/Print_File", dataDir_ + "MCDS" + pathSeparator_);
	conf[MCDS].screenDumpFileDir_ = readConfigDir("/Dir/Mcds/Video_Dump_File", dataDir_ + "MCDS" + pathSeparator_);
    conf[MCDS].wavFileDir_[0] = readConfigDir("/Dir/Mcds/Wav_File", dataDir_ + "MCDS" + pathSeparator_);
    conf[MCDS].wavFileDir_[1] = readConfigDir("/Dir/Mcds/Wav_File1", dataDir_ + "MCDS" + pathSeparator_);

	conf[MCDS].rom_[MAINROM1] = configPointer->Read("/Mcds/Main_Rom_File1", "ut62_rev2.bin");
	conf[MCDS].rom_[MAINROM2] = configPointer->Read("/Mcds/Main_Rom_File2", "edit_asm.bin");
	conf[MCDS].rom_[MAINROM3] = configPointer->Read("/Mcds/Main_Rom_File3", "basic_3.bin");
    conf[MCDS].keyFile_ = configPointer->Read("/Mcds/Key_File", "");
    conf[MCDS].printFile_ = configPointer->Read("/Mcds/Print_File", "printerout.txt");
    conf[MCDS].screenDumpFile_ = configPointer->Read("/Mcds/Video_Dump_File", "screendump.png");
    conf[MCDS].wavFile_[0] = configPointer->Read("/Mcds/Wav_File", "");
    conf[MCDS].wavFile_[1] = configPointer->Read("/Mcds/Wav_File1", "");
    conf[MCDS].volume_ = (int)configPointer->Read("/Mcds/Volume", 25l);
    
	getConfigBool("/Mcds/SerialLog", false);

	configPointer->Read("/Mcds/Enable_Vt_Stretch_Dot", &conf[MCDS].stretchDot_, false);
    configPointer->Read("/Mcds/Enable_Vt_External", &elfConfiguration[MCDS].vtExternal, false);

    elfConfiguration[MCDS].useUart = false; 
    elfConfiguration[MCDS].bellFrequency_ = (int)configPointer->Read("/Mcds/Bell_Frequency", 800);
	elfConfiguration[MCDS].baudR = (int)configPointer->Read("/Mcds/Vt_Baud_Receive", 8l);
	elfConfiguration[MCDS].baudT = (int)configPointer->Read("/Mcds/Vt_Baud_Transmit", 8l);
	elfConfiguration[MCDS].vtType = (int)configPointer->Read("/Mcds/VT_Type", 2l);
    elfConfiguration[MCDS].vt52SetUpFeature_ = configPointer->Read("/Mcds/VT52Setup", 0x00004092l);
    elfConfiguration[MCDS].vt100SetUpFeature_ = configPointer->Read("/Mcds/VT100Setup", 0x0000cad2l);
    elfConfiguration[MCDS].vtExternalSetUpFeature_ = configPointer->Read("/Mcds/VTExternalSetup", 0x0000cad2l);
    elfConfiguration[MCDS].serialPort_ = configPointer->Read("/Mcds/VtSerialPortChoice", "");
    elfConfiguration[MCDS].vtEf = false;
    elfConfiguration[MCDS].vtQ = true;

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

 	setVtType("MCDS", MCDS, elfConfiguration[MCDS].vtType, false);

	elfConfiguration[MCDS].vtCharRom_ = configPointer->Read("/Mcds/Vt_Font_Rom_File", "vt100.bin");

    if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomMCDS", wxComboBox)->SetValue(conf[MCDS].rom_[MAINROM1]);
		XRCCTRL(*this, "MainRom2MCDS", wxComboBox)->SetValue(conf[MCDS].rom_[MAINROM2]);
		XRCCTRL(*this, "MainRom3MCDS", wxComboBox)->SetValue(conf[MCDS].rom_[MAINROM3]);
        XRCCTRL(*this, "KeyFileMCDS", wxTextCtrl)->SetValue(conf[MCDS].keyFile_);
        XRCCTRL(*this, "PrintFileMCDS", wxTextCtrl)->SetValue(conf[MCDS].printFile_);
        XRCCTRL(*this, "ScreenDumpFileMCDS", wxComboBox)->SetValue(conf[MCDS].screenDumpFile_);
        
        XRCCTRL(*this, "TurboClockMCDS", wxTextCtrl)->SetValue(conf[MCDS].turboClock_);
        XRCCTRL(*this, "TurboMCDS", wxCheckBox)->SetValue(conf[MCDS].turbo_);
        turboGui("MCDS");
        XRCCTRL(*this, "AutoCasLoadMCDS", wxCheckBox)->SetValue(conf[MCDS].autoCassetteLoad_);
        
        XRCCTRL(*this, "PrintModeMCDS", wxChoice)->SetSelection((int)configPointer->Read("/Mcds/Print_Mode", 1l));
        setPrintMode();
  
        XRCCTRL(*this, "VTTypeMCDS", wxChoice)->SetSelection(elfConfiguration[MCDS].vtType);
        XRCCTRL(*this, "McdsForceUC", wxCheckBox)->SetValue(elfConfiguration[MCDS].forceUpperCase);
        
		XRCCTRL(*this, "VTBaudRChoiceMCDS", wxChoice)->SetSelection(elfConfiguration[MCDS].baudR);
		XRCCTRL(*this, "VTBaudTChoiceMCDS", wxChoice)->SetSelection(elfConfiguration[MCDS].baudT);

        correctZoomVtAndValue(MCDS, "MCDS", SET_SPIN);

        XRCCTRL(*this, "McdsBootRam", wxCheckBox)->SetValue(elfConfiguration[MCDS].bootRam);
        
		XRCCTRL(*this, "StretchDotMCDS", wxCheckBox)->SetValue(conf[MCDS].stretchDot_);
        XRCCTRL(*this, "WavFileMCDS", wxTextCtrl)->SetValue(conf[MCDS].wavFile_[0]);
        XRCCTRL(*this, "WavFile1MCDS", wxTextCtrl)->SetValue(conf[MCDS].wavFile_[1]);
        XRCCTRL(*this, "VolumeMCDS", wxSlider)->SetValue(conf[MCDS].volume_);
        
        if (clockTextCtrl[MCDS] != NULL)
            clockTextCtrl[MCDS]->ChangeValue(conf[MCDS].clock_);

        XRCCTRL(*this, "UseLocationMCDS", wxCheckBox)->SetValue(conf[MCDS].useLoadLocation_);
    }
}

void GuiMcds::writeMcdsDirConfig()
{
    writeConfigDir("/Dir/Mcds/Main", conf[MCDS].mainDir_);
	writeConfigDir("/Dir/Mcds/Main_Rom_File1", conf[MCDS].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Mcds/Main_Rom_File2", conf[MCDS].romDir_[MAINROM2]);
	writeConfigDir("/Dir/Mcds/Main_Rom_File3", conf[MCDS].romDir_[MAINROM3]);
	writeConfigDir("/Dir/Mcds/Vt_Font_Rom_File", elfConfiguration[MCDS].vtCharRomDir_);
    writeConfigDir("/Dir/Mcds/Key_File", conf[MCDS].keyFileDir_);
    writeConfigDir("/Dir/Mcds/Print_File", conf[MCDS].printFileDir_);
    writeConfigDir("/Dir/Mcds/Video_Dump_File", conf[MCDS].screenDumpFileDir_);
    writeConfigDir("/Dir/Mcds/Wav_File", conf[MCDS].wavFileDir_[0]);
    writeConfigDir("/Dir/Mcds/Wav_File1", conf[MCDS].wavFileDir_[1]);
}

void GuiMcds::writeMcdsConfig()
{
    writeElfPortConfig(MCDS, "MCDS");

	configPointer->Write("/Mcds/Main_Rom_File1", conf[MCDS].rom_[MAINROM1]);
	configPointer->Write("/Mcds/Main_Rom_File2", conf[MCDS].rom_[MAINROM2]);
	configPointer->Write("/Mcds/Main_Rom_File3", conf[MCDS].rom_[MAINROM3]);
	configPointer->Write("/Mcds/Vt_Font_Rom_File", elfConfiguration[MCDS].vtCharRom_);
    configPointer->Write("/Mcds/Key_File", conf[MCDS].keyFile_);
    configPointer->Write("/Mcds/Print_File", conf[MCDS].printFile_);
    configPointer->Write("/Mcds/Video_Dump_File", conf[MCDS].screenDumpFile_);
    configPointer->Write("/Mcds/Wav_File", conf[MCDS].wavFile_[0]);
    configPointer->Write("/Mcds/Wav_File1", conf[MCDS].wavFile_[1]);
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
    XRCCTRL(*this, "VTBaudRChoiceMCDS", wxChoice)->SetSelection(elfConfiguration[MCDS].baudR);
}

void GuiMcds::onMcdsForceUpperCase(wxCommandEvent&event)
{
    elfConfiguration[MCDS].forceUpperCase = event.IsChecked();
    if (runningComputer_ == MCDS)
    {
        p_Computer->setForceUpperCase(event.IsChecked());
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

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
#include "guipecom.h"
#include "psave.h"

BEGIN_EVENT_TABLE(GuiPecom, GuiMain)

	EVT_TEXT(XRCID("MainRomPecom"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomPecom"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonPecom"), GuiMain::onMainRom1)

	EVT_BUTTON(XRCID("ScreenDumpFileButtonPecom"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFilePecom"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFilePecom"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5Pecom"), GuiMain::onScreenDump)

	EVT_SPIN_UP(XRCID("ZoomSpinPecom"), GuiMain::onZoom)
	EVT_SPIN_DOWN(XRCID("ZoomSpinPecom"), GuiMain::onZoom)
	EVT_TEXT(XRCID("ZoomValuePecom"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3Pecom"), GuiMain::onFullScreen)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumePecom"), GuiMain::onVolume) 
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumePecom"), GuiMain::onVolume) 
	EVT_BUTTON(XRCID("CasButtonPecom"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasPecom"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFilePecom"), GuiMain::onCassetteText)
	EVT_BUTTON(XRCID("RealCasLoadPecom"), GuiMain::onRealCas)
	EVT_BUTTON(XRCID("CasLoadPecom"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSavePecom"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopPecom"), GuiMain::onCassetteStop)
	EVT_CHECKBOX(XRCID("TurboPecom"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockPecom"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadPecom"), GuiMain::onAutoLoad)
	EVT_BUTTON(XRCID("PrintFileButtonPecom"), GuiMain::onPrintFile)
	EVT_TEXT(XRCID("PrintFilePecom"), GuiMain::onPrintFileText)
	EVT_BUTTON(XRCID("PrintButtonPecom"), GuiMain::onPrintButton)
	EVT_CHOICE(XRCID("PrintModePecom"), GuiMain::onPrintMode)
	EVT_BUTTON(XRCID("KeyFileButtonPecom"), GuiMain::onKeyFile)
	EVT_TEXT(XRCID("KeyFilePecom"), GuiMain::onKeyFileText)
	EVT_BUTTON(XRCID("EjectKeyFilePecom"), GuiMain::onKeyFileEject)
	EVT_CHECKBOX(XRCID("UseLocationPecom"), GuiMain::onUseLocation)

	EVT_TEXT(XRCID("SaveStartPecom"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndPecom"), GuiMain::onSaveEnd)
	EVT_TEXT(XRCID("SaveExecPecom"), GuiMain::onSaveExec)

	EVT_BUTTON(XRCID("SaveButtonPecom"), GuiMain::onSaveButton) 
	EVT_BUTTON(XRCID("LoadButtonPecom"), GuiMain::onLoadButton)
	EVT_BUTTON(XRCID("RunButtonPecom"), GuiMain::onLoadRunButton)
	EVT_BUTTON(XRCID("DsaveButtonPecom"), GuiMain::onDataSaveButton)
	EVT_COMMAND(wxID_ANY, OPEN_PRINTER_WINDOW, GuiMain::openPrinterFrame) 
	EVT_BUTTON(XRCID("ColoursPecom"), Main::onColoursDef)

    EVT_CHECKBOX(XRCID("DramPecom"), GuiPecom::onPecomDram)

END_EVENT_TABLE()

GuiPecom::GuiPecom(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiMain(title, pos, size, mode, dataDir, iniDir)
{
	conf[PECOM].loadFileNameFull_ = "";
	conf[PECOM].loadFileName_ = "";

	conf[PECOM].pLoadSaveName_[0] = 'P';
	conf[PECOM].pLoadSaveName_[1] = '-';
	conf[PECOM].pLoadSaveName_[2] = '6';
	conf[PECOM].pLoadSaveName_[3] = '4';

	conf[PECOM].defus_ = 0x81;
	conf[PECOM].eop_ = 0x83;
	conf[PECOM].string_ = 0x92;
	conf[PECOM].arrayValue_ = 0x94;
	conf[PECOM].eod_ = 0x99;
	conf[PECOM].basicRamAddress_ = 0x200;

	conf[PECOM].saveStartString_ = "";
	conf[PECOM].saveEndString_ = "";
	conf[PECOM].saveExecString_ = "";
}

void GuiPecom::readPecomConfig()
{
	selectedComputer_ = PECOM;

    conf[PECOM].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Pecom" + pathSeparator_;
    conf[PECOM].mainDir_ = readConfigDir("/Dir/Pecom/Main", dataDir_ + "Pecom" + pathSeparator_);
    
    conf[PECOM].romDir_[MAINROM1] = readConfigDir("/Dir/Pecom/Main_Rom_File", dataDir_ + "Pecom" + pathSeparator_);
	conf[PECOM].ramDir_ = readConfigDir("/Dir/Pecom/Software_File", dataDir_ + "Pecom" + pathSeparator_); 
	conf[PECOM].keyFileDir_ = readConfigDir("/Dir/Pecom/Key_File", dataDir_ + "Pecom" + pathSeparator_);
	conf[PECOM].printFileDir_ = readConfigDir("/Dir/Pecom/Print_File", dataDir_ + "Pecom" + pathSeparator_); 
	conf[PECOM].screenDumpFileDir_ = readConfigDir("/Dir/Pecom/Video_Dump_File", dataDir_ + "Pecom" + pathSeparator_); 
	conf[PECOM].wavFileDir_[0] = readConfigDir("/Dir/Pecom/Wav_File", dataDir_ + "Pecom" + pathSeparator_); 

	conf[PECOM].rom_[MAINROM1] = configPointer->Read("/Pecom/Main_Rom_File", "pecom64.v4.bin");
	conf[PECOM].keyFile_ = configPointer->Read("/Pecom/Key_File", "");
	conf[PECOM].printFile_ = configPointer->Read("/Pecom/Print_File", "printerout.txt");
	conf[PECOM].screenDumpFile_ = configPointer->Read("/Pecom/Video_Dump_File", "screendump.png");
	conf[PECOM].wavFile_[0] = configPointer->Read("/Pecom/Wav_File", "");

	conf[PECOM].volume_ = (int)configPointer->Read("/Pecom/Volume", 25l);
	conf[PECOM].turboClock_ = configPointer->Read("/Pecom/Turbo_Clock_Speed", "15"); 
	conf[PECOM].printMode_ = (int)configPointer->Read("/Pecom/Print_Mode", 1l);

    configPointer->Read("/Pecom/UseLoadLocation", &conf[PECOM].useLoadLocation_, false);
	configPointer->Read("/Pecom/Enable_Turbo_Cassette", &conf[PECOM].turbo_, true);
	configPointer->Read("/Pecom/Enable_Auto_Cassette", &conf[PECOM].autoCassetteLoad_, true);
	configPointer->Read("/Pecom/Enable_Real_Cassette", &conf[PECOM].realCassetteLoad_, false);

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[PECOM].zoom_ = convertLocale(configPointer->Read("/Pecom/Zoom", defaultZoom));
	wxString defaultClock;
	defaultClock.Printf("%1.3f", 2.813);
	conf[PECOM].clock_ = convertLocale(configPointer->Read("/Pecom/Clock_Speed", defaultClock));

    configPointer->Read("/Pecom/Dram", &conf[PECOM].dram_, true);

	setRealCas(PECOM);
    
    long value;
    conf[PECOM].saveStartString_ = configPointer->Read("/Pecom/SaveStart", "0");
    if (!conf[PECOM].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[PECOM].saveStart_ = value;

	if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomPecom", wxComboBox)->SetValue(conf[PECOM].rom_[MAINROM1]);
		XRCCTRL(*this, "KeyFilePecom", wxTextCtrl)->SetValue(conf[PECOM].keyFile_);
		XRCCTRL(*this, "PrintFilePecom", wxTextCtrl)->SetValue(conf[PECOM].printFile_);
		XRCCTRL(*this, "ScreenDumpFilePecom", wxComboBox)->SetValue(conf[PECOM].screenDumpFile_);
		XRCCTRL(*this, "WavFilePecom", wxTextCtrl)->SetValue(conf[PECOM].wavFile_[0]);

        correctZoomAndValue(PECOM, "Pecom", SET_SPIN);

		XRCCTRL(*this, "VolumePecom", wxSlider)->SetValue(conf[PECOM].volume_);

        if (clockTextCtrl[PECOM] != NULL)
            clockTextCtrl[PECOM]->ChangeValue(conf[PECOM].clock_);
		XRCCTRL(*this, "TurboPecom", wxCheckBox)->SetValue(conf[PECOM].turbo_);
		turboGui("Pecom");
		XRCCTRL(*this, "TurboClockPecom", wxTextCtrl)->SetValue(conf[PECOM].turboClock_);
		XRCCTRL(*this, "AutoCasLoadPecom", wxCheckBox)->SetValue(conf[PECOM].autoCassetteLoad_);

		XRCCTRL(*this, "PrintModePecom", wxChoice)->SetSelection((int)configPointer->Read("/Pecom/Print_Mode", 1l));
		setPrintMode();
		XRCCTRL(*this, "UseLocationPecom", wxCheckBox)->SetValue(false);
        XRCCTRL(*this, "DramPecom", wxCheckBox)->SetValue(conf[PECOM].dram_);
        XRCCTRL(*this, "UseLocationPecom", wxCheckBox)->SetValue(conf[PECOM].useLoadLocation_);
        if (conf[PECOM].saveStart_ != 0)
            XRCCTRL(*this, "SaveStartPecom", wxTextCtrl)->SetValue(conf[PECOM].saveStartString_); 
	}
}

void GuiPecom::writePecomDirConfig()
{
    writeConfigDir("/Dir/Pecom/Main", conf[PECOM].mainDir_);
    writeConfigDir("/Dir/Pecom/Main_Rom_File", conf[PECOM].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Pecom/Software_File", conf[PECOM].ramDir_);
	writeConfigDir("/Dir/Pecom/Key_File", conf[PECOM].keyFileDir_);
	writeConfigDir("/Dir/Pecom/Print_File", conf[PECOM].printFileDir_);
	writeConfigDir("/Dir/Pecom/Video_Dump_File", conf[PECOM].screenDumpFileDir_);
	writeConfigDir("/Dir/Pecom/Wav_File", conf[PECOM].wavFileDir_[0]);
}

void GuiPecom::writePecomConfig()
{
	configPointer->Write("/Pecom/Main_Rom_File", conf[PECOM].rom_[MAINROM1]);
	configPointer->Write("/Pecom/Key_File", conf[PECOM].keyFile_);
	configPointer->Write("/Pecom/Print_File", conf[PECOM].printFile_);
	configPointer->Write("/Pecom/Video_Dump_File", conf[PECOM].screenDumpFile_);
	configPointer->Write("/Pecom/Wav_File", conf[PECOM].wavFile_[0]);

	configPointer->Write("/Pecom/Zoom", conf[PECOM].zoom_);
	configPointer->Write("/Pecom/Volume", conf[PECOM].volume_);

	configPointer->Write("/Pecom/Clock_Speed", conf[PECOM].clock_);
	configPointer->Write("/Pecom/Enable_Turbo_Cassette", conf[PECOM].turbo_);
	configPointer->Write("/Pecom/Turbo_Clock_Speed", conf[PECOM].turboClock_); 
	configPointer->Write("/Pecom/Enable_Auto_Cassette", conf[PECOM].autoCassetteLoad_);
	configPointer->Write("/Pecom/Enable_Real_Cassette", conf[PECOM].realCassetteLoad_);
	configPointer->Write("/Pecom/Print_Mode", conf[PECOM].printMode_);
    configPointer->Write("/Pecom/Dram", conf[PECOM].dram_);
    configPointer->Write("/Pecom/UseLoadLocation", conf[PECOM].useLoadLocation_);
    configPointer->Write("/Pecom/SaveStart", conf[PECOM].saveStartString_);
}

void GuiPecom::readPecomWindowConfig()
{
    conf[PECOM].mainX_ = (int)configPointer->Read("/Pecom/Window_Position_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[PECOM].mainY_ = (int)configPointer->Read("/Pecom/Window_Position_Y", mainWindowY_);
}

void GuiPecom::writePecomWindowConfig()
{
    if (conf[PECOM].mainX_ > 0)
        configPointer->Write("/Pecom/Window_Position_X", conf[PECOM].mainX_);
    if (conf[PECOM].mainY_ > 0)
        configPointer->Write("/Pecom/Window_Position_Y", conf[PECOM].mainY_);
}

void GuiPecom::onPecomDram(wxCommandEvent&event)
{
    conf[PECOM].dram_ = event.IsChecked();
}

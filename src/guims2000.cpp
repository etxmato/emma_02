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
#include "guims2000.h"

DEFINE_EVENT_TYPE(ON_UART_MS2000)

BEGIN_EVENT_TABLE(GuiMS2000, GuiMcds)

	EVT_TEXT(XRCID("MainRomMS2000"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomMS2000"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonMS2000"), GuiMain::onMainRom1)

    EVT_BUTTON(XRCID("FDC0_ButtonMS2000"), GuiMS2000::onMs2000Disk0)
    EVT_TEXT(XRCID("FDC0_FileMS2000"), GuiMS2000::onMs2000DiskText0)
    EVT_BUTTON(XRCID("Eject_FDC0MS2000"), GuiMS2000::onMs2000DiskEject0)
    EVT_BUTTON(XRCID("FDC0_SwitchMS2000"), GuiMS2000::onMS2000DiskDirSwitch0)
    EVT_BUTTON(XRCID("FDC1_ButtonMS2000"), GuiMS2000::onMs2000Disk1)
    EVT_TEXT(XRCID("FDC1_FileMS2000"), GuiMS2000::onMs2000DiskText1)
    EVT_BUTTON(XRCID("Eject_FDC1MS2000"), GuiMS2000::onMs2000DiskEject1)
    EVT_BUTTON(XRCID("FDC1_SwitchMS2000"), GuiMS2000::onMS2000DiskDirSwitch1)
    EVT_BUTTON(XRCID("FDC2_ButtonMS2000"), GuiMS2000::onMs2000Disk2)
    EVT_TEXT(XRCID("FDC2_FileMS2000"), GuiMS2000::onMs2000DiskText2)
    EVT_BUTTON(XRCID("Eject_FDC2MS2000"), GuiMS2000::onMs2000DiskEject2)
    EVT_BUTTON(XRCID("FDC2_SwitchMS2000"), GuiMS2000::onMS2000DiskDirSwitch2)
    EVT_BUTTON(XRCID("FDC3_ButtonMS2000"), GuiMS2000::onMs2000Disk3)
    EVT_TEXT(XRCID("FDC3_FileMS2000"), GuiMS2000::onMs2000DiskText3)
    EVT_BUTTON(XRCID("Eject_FDC3MS2000"), GuiMS2000::onMs2000DiskEject3)
    EVT_BUTTON(XRCID("FDC3_SwitchMS2000"), GuiMS2000::onMS2000DiskDirSwitch3)

    EVT_BUTTON(XRCID("CasButtonMS2000"), GuiMain::onCassette)
    EVT_BUTTON(XRCID("EjectCasMS2000"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileMS2000"), GuiMain::onCassetteText)

    EVT_TEXT(XRCID("VtCharRomMS2000"), GuiMain::onVtCharRomText)
	EVT_COMBOBOX(XRCID("VtCharRomMS2000"), GuiMain::onVtCharRomText)
	EVT_BUTTON(XRCID("VtCharRomButtonMS2000"), GuiMain::onVtCharRom)

	EVT_TEXT(XRCID("KeyFileMS2000"), GuiMain::onKeyFileText)
	EVT_BUTTON(XRCID("KeyFileButtonMS2000"), GuiMain::onKeyFile)
	EVT_BUTTON(XRCID("EjectKeyFileMS2000"), GuiMain::onKeyFileEject)

    EVT_TEXT(XRCID("PrintFileMS2000"), GuiMain::onPrintFileText)
    EVT_BUTTON(XRCID("PrintButtonMS2000"), GuiMain::onPrintButton)
    EVT_CHOICE(XRCID("PrintModeMS2000"), GuiMain::onPrintMode)
    EVT_BUTTON(XRCID("PrintFileButtonMS2000"), GuiMain::onPrintFile)

    EVT_COMMAND(wxID_ANY, OPEN_PRINTER_WINDOW, GuiMain::openPrinterFrame)

    EVT_CHOICE(XRCID("VTTypeMS2000"), GuiMain::onVT100)
	EVT_SPIN_UP(XRCID("ZoomSpinVtMS2000"), GuiMain::onZoomUpVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtMS2000"), GuiMain::onZoomDownVt)
	EVT_TEXT(XRCID("ZoomValueVtMS2000"), GuiMain::onZoomValueVt)
	EVT_BUTTON(XRCID("FullScreenF3MS2000"), GuiMain::onFullScreen)
    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeMS2000"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeMS2000"), GuiMain::onVolume)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonMS2000"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileMS2000"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileMS2000"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5MS2000"), GuiMain::onScreenDump)

    EVT_CHECKBOX(XRCID("MS2000ForceUC"), GuiMS2000::onMS2000ForceUpperCase)
    EVT_BUTTON(XRCID("VtSetupMS2000"), GuiMain::onVtSetup)
	EVT_BUTTON(XRCID("SaveButtonMS2000"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonMS2000"), GuiMain::onLoadButton)
	EVT_CHECKBOX(XRCID("StretchDotMS2000"), GuiMain::onStretchDot)
	EVT_BUTTON(XRCID("ColoursMS2000"), Main::onColoursDef)
    EVT_CHECKBOX(XRCID("MS2000BootRam"), GuiMS2000::onBootRam)

	EVT_TEXT(XRCID("SaveStartMS2000"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndMS2000"), GuiMain::onSaveEnd)

    EVT_CHECKBOX(XRCID("TurboMS2000"), GuiMain::onTurbo)
    EVT_TEXT(XRCID("TurboClockMS2000"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadMS2000"), GuiMain::onAutoLoad)
    EVT_BUTTON(XRCID("CasLoadMS2000"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasSaveMS2000"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasStopMS2000"), GuiMain::onCassetteStop)
    EVT_BUTTON(XRCID("RealCasLoadMS2000"), GuiMain::onRealCas)

    EVT_COMMAND(wxID_ANY, ON_UART_MS2000, GuiMS2000::onMS2000Uart)

	EVT_CHOICE(XRCID("VTBaudTChoiceMS2000"), GuiMS2000::onMS2000BaudT)
	EVT_CHOICE(XRCID("VTBaudRChoiceMS2000"), GuiMS2000::onMS2000BaudR)

END_EVENT_TABLE()

GuiMS2000::GuiMS2000(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode_, wxString dataDir, wxString iniDir)
: GuiMcds(title, pos, size, mode_, dataDir, iniDir)
{
	conf[MS2000].saveStartString_ = "";
	conf[MS2000].saveEndString_ = "";
    position_.x = 0;
}

void GuiMS2000::readMS2000Config()
{
	selectedComputer_ = MS2000;

	conf[MS2000].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "MS2000" + pathSeparator_;

    conf[MS2000].mainDir_ = readConfigDir("/Dir/MS2000/Main", dataDir_ + "MS2000" + pathSeparator_);
	conf[MS2000].romDir_[MAINROM1] = readConfigDir("/Dir/MS2000/Main_Rom_File", dataDir_ + "MS2000" + pathSeparator_);
	conf[MS2000].vtCharRomDir_ = readConfigDir("/Dir/MS2000/Vt_Font_Rom_File", dataDir_ + "MS2000" + pathSeparator_);
    floppyDirMs2000_[0] = readConfigDir("/Dir/MS2000/FDC0_File", dataDir_ + "MS2000" + pathSeparator_);
    floppyDirMs2000_[1] = readConfigDir("/Dir/MS2000/FDC1_File", dataDir_ + "MS2000" + pathSeparator_);
    floppyDirMs2000_[2] = readConfigDir("/Dir/MS2000/FDC2_File", dataDir_ + "MS2000" + pathSeparator_);
    floppyDirMs2000_[3] = readConfigDir("/Dir/MS2000/FDC3_File", dataDir_ + "MS2000" + pathSeparator_);
    floppyDirSwitchedMs2000_[0] = readConfigDir("/Dir/MS2000/FDC0_File_Switched", dataDir_ + "MS2000" + pathSeparator_ + "Software" + pathSeparator_);
    floppyDirSwitchedMs2000_[1] = readConfigDir("/Dir/MS2000/FDC1_File_Switched", dataDir_ + "MS2000" + pathSeparator_ + "Software" + pathSeparator_);
    floppyDirSwitchedMs2000_[2] = readConfigDir("/Dir/MS2000/FDC2_File_Switched", dataDir_ + "MS2000" + pathSeparator_ + "PLM" + pathSeparator_);
    floppyDirSwitchedMs2000_[3] = readConfigDir("/Dir/MS2000/FDC3_File_Switched", dataDir_ + "MS2000" + pathSeparator_ + "BASIC" + pathSeparator_);
    conf[MS2000].keyFileDir_ = readConfigDir("/Dir/MS2000/Key_File", dataDir_ + "MS2000" + pathSeparator_);
    conf[MS2000].printFileDir_ = readConfigDir("Dir/MS2000/Print_File", dataDir_ + "MS2000" + pathSeparator_);
	conf[MS2000].screenDumpFileDir_ = readConfigDir("/Dir/MS2000/Video_Dump_File", dataDir_ + "MS2000" + pathSeparator_);
    conf[MS2000].wavFileDir_[0] = readConfigDir("/Dir/MS2000/Wav_File", dataDir_ + "MS2000" + pathSeparator_);
    
	conf[MS2000].rom_[MAINROM1] = configPointer->Read("/MS2000/Main_Rom_File", "ut71.bin");
    floppyMs2000_[0] = configPointer->Read("/MS2000/FDC0_File", "microdos.img");
    floppyMs2000_[1] = configPointer->Read("/MS2000/FDC1_File", "PLM 1800.img");
    floppyMs2000_[2] = configPointer->Read("/MS2000/FDC2_File", "basic1.img");
    floppyMs2000_[3] = configPointer->Read("/MS2000/FDC3_File", "");
    conf[MS2000].keyFile_ = configPointer->Read("/MS2000/Key_File", "");
    conf[MS2000].printFile_ = configPointer->Read("/MS2000/Print_File", "printerout.txt");
    conf[MS2000].screenDumpFile_ = configPointer->Read("/MS2000/Video_Dump_File", "screendump.png");
    conf[MS2000].wavFile_[0] = configPointer->Read("/MS2000/Wav_File", "");
    conf[MS2000].volume_ = (int)configPointer->Read("/MS2000/Volume", 25l);
    
	getConfigBool("/MS2000/SerialLog", false);

	configPointer->Read("/MS2000/Enable_Vt_Stretch_Dot", &conf[MS2000].stretchDot_, false);
    configPointer->Read("/MS2000/Enable_Vt_External", &elfConfiguration[MS2000].vtExternal, false);

    elfConfiguration[MS2000].uartGroup = 0;
    elfConfiguration[MS2000].useUart = true;
    elfConfiguration[MS2000].bellFrequency_ = (int)configPointer->Read("/MS2000/Bell_Frequency", 800);
	elfConfiguration[MS2000].baudR = (int)configPointer->Read("/MS2000/Vt_Baud_Receive", 1l);
	elfConfiguration[MS2000].baudT = (int)configPointer->Read("/MS2000/Vt_Baud_Transmit", 1l);
	elfConfiguration[MS2000].vtType = (int)configPointer->Read("/MS2000/VT_Type", 2l);
    elfConfiguration[MS2000].vt52SetUpFeature_ = configPointer->Read("/MS2000/VT52Setup", 0x00004092l);
    elfConfiguration[MS2000].vt100SetUpFeature_ = configPointer->Read("/MS2000/VT100Setup", 0x0000cad2l);
    elfConfiguration[MS2000].vtExternalSetUpFeature_ = configPointer->Read("/MS2000/VTExternalSetup", 0x0000cad2l);
    elfConfiguration[MS2000].serialPort_ = configPointer->Read("/MS2000/VtSerialPortChoice", "");

    configPointer->Read("/MS2000/Force_Uppercase", &elfConfiguration[MS2000].forceUpperCase, true);
    configPointer->Read("/MS2000/Boot_From_Ram", &elfConfiguration[MS2000].bootRam, false);
    
	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 1.0);
	conf[MS2000].zoomVt_ = configPointer->Read("/MS2000/Vt_Zoom", defaultZoom);
	wxString defaultScale;
	defaultScale.Printf("%i", 3);
	conf[MS2000].xScale_ = configPointer->Read("/MS2000/Window_Scale_Factor_X", defaultScale);

	wxString defaultClock;
	defaultClock.Printf("%1.1f", 2.0);
	conf[MS2000].clock_ = configPointer->Read("/MS2000/Clock_Speed", defaultClock);

    conf[MS2000].turboClock_ = configPointer->Read("/MS2000/Turbo_Clock_Speed", "15");
    conf[MS2000].printMode_ = (int)configPointer->Read("/MS2000/Print_Mode", 1l);
    configPointer->Read("/MS2000/Enable_Turbo_Cassette", &conf[MS2000].turbo_, true);
    configPointer->Read("/MS2000/Enable_Auto_Cassette", &conf[MS2000].autoCassetteLoad_, true);
    configPointer->Read("/MS2000/Enable_Real_Cassette", &conf[MS2000].realCassetteLoad_, false);
 
	setVtType("MS2000", MS2000, elfConfiguration[MS2000].vtType, false);

	conf[MS2000].vtCharRom_ = configPointer->Read("/MS2000/Vt_Font_Rom_File", "vt100.bin");

    configPointer->Read("/MS2000/DirectoryMode_0", &directoryMode_[0], false);
    configPointer->Read("/MS2000/DirectoryMode_1", &directoryMode_[1], false);
    configPointer->Read("/MS2000/DirectoryMode_2", &directoryMode_[2], false);
    configPointer->Read("/MS2000/DirectoryMode_3", &directoryMode_[3], false);

    if (mode_.gui)
	{
        if (directoryMode_[0])
        {
            XRCCTRL(*this, "FDC0_ButtonMS2000", wxButton)->SetLabel("HD 0");
            XRCCTRL(*this, "FDC0_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 HD Directory 0");
            XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->Enable(false);
            XRCCTRL(*this, "Eject_FDC0MS2000", wxBitmapButton)->Enable(false);
            wxFileName setectedDirFile = wxFileName(floppyDirSwitchedMs2000_[0]);
            wxArrayString dirArray = setectedDirFile.GetDirs();
            wxString dirName = dirArray.Last();
            XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->SetValue(dirName);
        }
        else
        {
            XRCCTRL(*this, "FDC0_ButtonMS2000", wxButton)->SetLabel("FDC 0");
            XRCCTRL(*this, "FDC0_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 FDC 0 image file");
            XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->Enable(true);
            XRCCTRL(*this, "Eject_FDC0MS2000", wxBitmapButton)->Enable(true);
            XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[0]);
        }
        
        if (directoryMode_[1])
        {
            XRCCTRL(*this, "FDC1_ButtonMS2000", wxButton)->SetLabel("HD 1");
            XRCCTRL(*this, "FDC1_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 HD Directory 1");
            XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->Enable(false);
            XRCCTRL(*this, "Eject_FDC1MS2000", wxBitmapButton)->Enable(false);
            wxFileName setectedDirFile = wxFileName(floppyDirSwitchedMs2000_[1]);
            wxArrayString dirArray = setectedDirFile.GetDirs();
            wxString dirName = dirArray.Last();
            XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->SetValue(dirName);
        }
        else
        {
            XRCCTRL(*this, "FDC1_ButtonMS2000", wxButton)->SetLabel("FDC 1");
            XRCCTRL(*this, "FDC1_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 FDC 1 image file");
            XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->Enable(true);
            XRCCTRL(*this, "Eject_FDC1MS2000", wxBitmapButton)->Enable(true);
            XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[1]);
        }
        
        if (directoryMode_[2])
        {
            XRCCTRL(*this, "FDC2_ButtonMS2000", wxButton)->SetLabel("HD 2");
            XRCCTRL(*this, "FDC2_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 HD Directory 2");
            XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->Enable(false);
            XRCCTRL(*this, "Eject_FDC2MS2000", wxBitmapButton)->Enable(false);
            wxFileName setectedDirFile = wxFileName(floppyDirSwitchedMs2000_[2]);
            wxArrayString dirArray = setectedDirFile.GetDirs();
            wxString dirName = dirArray.Last();
            XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->SetValue(dirName);
        }
        else
        {
            XRCCTRL(*this, "FDC2_ButtonMS2000", wxButton)->SetLabel("FDC 2");
            XRCCTRL(*this, "FDC2_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 FDC 2 image file");
            XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->Enable(true);
            XRCCTRL(*this, "Eject_FDC2MS2000", wxBitmapButton)->Enable(true);
            XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[2]);
        }
        
        if (directoryMode_[3])
        {
            XRCCTRL(*this, "FDC3_ButtonMS2000", wxButton)->SetLabel("HD 3");
            XRCCTRL(*this, "FDC3_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 HD Directory 3");
            XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->Enable(false);
            XRCCTRL(*this, "Eject_FDC3MS2000", wxBitmapButton)->Enable(false);
            wxFileName setectedDirFile = wxFileName(floppyDirSwitchedMs2000_[3]);
            wxArrayString dirArray = setectedDirFile.GetDirs();
            wxString dirName = dirArray.Last();
            XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->SetValue(dirName);
        }
        else
        {
            XRCCTRL(*this, "FDC3_ButtonMS2000", wxButton)->SetLabel("FDC 3");
            XRCCTRL(*this, "FDC3_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 FDC 3 image file");
            XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->Enable(true);
            XRCCTRL(*this, "Eject_FDC3MS2000", wxBitmapButton)->Enable(true);
            XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[3]);
        }

        XRCCTRL(*this, "MainRomMS2000", wxComboBox)->SetValue(conf[MS2000].rom_[MAINROM1]);
		XRCCTRL(*this, "VtCharRomMS2000", wxComboBox)->SetValue(conf[MS2000].vtCharRom_);
        XRCCTRL(*this, "KeyFileMS2000", wxTextCtrl)->SetValue(conf[MS2000].keyFile_);
        XRCCTRL(*this, "PrintFileMS2000", wxTextCtrl)->SetValue(conf[MS2000].printFile_);
        XRCCTRL(*this, "ScreenDumpFileMS2000", wxComboBox)->SetValue(conf[MS2000].screenDumpFile_);
        
        XRCCTRL(*this, "TurboClockMS2000", wxTextCtrl)->SetValue(conf[MS2000].turboClock_);
        XRCCTRL(*this, "TurboMS2000", wxCheckBox)->SetValue(conf[MS2000].turbo_);
        turboGui("MS2000");
        XRCCTRL(*this, "AutoCasLoadMS2000", wxCheckBox)->SetValue(conf[MS2000].autoCassetteLoad_);
        
        XRCCTRL(*this, "PrintModeMS2000", wxChoice)->SetSelection((int)configPointer->Read("/MS2000/Print_Mode", 1l));
        setPrintMode();
  
        XRCCTRL(*this, "VTTypeMS2000", wxChoice)->SetSelection(elfConfiguration[MS2000].vtType);
        XRCCTRL(*this, "MS2000ForceUC", wxCheckBox)->SetValue(elfConfiguration[MS2000].forceUpperCase);
        
		XRCCTRL(*this, "VTBaudRChoiceMS2000", wxChoice)->SetSelection(elfConfiguration[MS2000].baudR);
		XRCCTRL(*this, "VTBaudTChoiceMS2000", wxChoice)->SetSelection(elfConfiguration[MS2000].baudT);

        XRCCTRL(*this, "ZoomValueVtMS2000", wxTextCtrl)->ChangeValue(conf[MS2000].zoomVt_);
        XRCCTRL(*this, "MS2000BootRam", wxCheckBox)->SetValue(elfConfiguration[MS2000].bootRam);
        
		XRCCTRL(*this, "StretchDotMS2000", wxCheckBox)->SetValue(conf[MS2000].stretchDot_);
        XRCCTRL(*this, "WavFileMS2000", wxTextCtrl)->SetValue(conf[MS2000].wavFile_[0]);
        XRCCTRL(*this, "VolumeMS2000", wxSlider)->SetValue(conf[MS2000].volume_);
        
        clockTextCtrl[MS2000]->ChangeValue(conf[MS2000].clock_);
	}
}

void GuiMS2000::writeMS2000DirConfig()
{
    writeConfigDir("/Dir/MS2000/Main", conf[MS2000].mainDir_);
	writeConfigDir("/Dir/MS2000/Main_Rom_File", conf[MS2000].romDir_[MAINROM1]);
	writeConfigDir("/Dir/MS2000/Vt_Font_Rom_File", conf[MS2000].vtCharRomDir_);
    writeConfigDir("/Dir/MS2000/FDC0_File", floppyDirMs2000_[0]);
    writeConfigDir("/Dir/MS2000/FDC1_File", floppyDirMs2000_[1]);
    writeConfigDir("/Dir/MS2000/FDC2_File", floppyDirMs2000_[2]);
    writeConfigDir("/Dir/MS2000/FDC3_File", floppyDirMs2000_[3]);
    writeConfigDir("/Dir/MS2000/FDC0_File_Switched", floppyDirSwitchedMs2000_[0]);
    writeConfigDir("/Dir/MS2000/FDC1_File_Switched", floppyDirSwitchedMs2000_[1]);
    writeConfigDir("/Dir/MS2000/FDC2_File_Switched", floppyDirSwitchedMs2000_[2]);
    writeConfigDir("/Dir/MS2000/FDC3_File_Switched", floppyDirSwitchedMs2000_[3]);
    writeConfigDir("/Dir/MS2000/Key_File", conf[MS2000].keyFileDir_);
    writeConfigDir("/Dir/MS2000/Print_File", conf[MS2000].printFileDir_);
    writeConfigDir("/Dir/MS2000/Video_Dump_File", conf[MS2000].screenDumpFileDir_);
    writeConfigDir("/Dir/MS2000/Wav_File", conf[MS2000].wavFileDir_[0]);
}

void GuiMS2000::writeMS2000Config()
{
    writeElfPortConfig(MS2000, "MS2000");

    configPointer->Write("/MS2000/Main_Rom_File", conf[MS2000].rom_[MAINROM1]);
	configPointer->Write("/MS2000/Vt_Font_Rom_File", conf[MS2000].vtCharRom_);
    configPointer->Write("/MS2000/FDC0_File", floppyMs2000_[0]);
    configPointer->Write("/MS2000/FDC1_File", floppyMs2000_[1]);
    configPointer->Write("/MS2000/FDC2_File", floppyMs2000_[2]);
    configPointer->Write("/MS2000/FDC3_File", floppyMs2000_[3]);
    configPointer->Write("/MS2000/Key_File", conf[MS2000].keyFile_);
    configPointer->Write("/MS2000/Print_File", conf[MS2000].printFile_);
    configPointer->Write("/MS2000/Video_Dump_File", conf[MS2000].screenDumpFile_);
    configPointer->Write("/MS2000/Wav_File", conf[MS2000].wavFile_[0]);
    configPointer->Write("/MS2000/VtSerialPortChoice", elfConfiguration[MS2000].serialPort_);
  
	configPointer->Write("/MS2000/Bell_Frequency", elfConfiguration[MS2000].bellFrequency_);
	configPointer->Write("/MS2000/VT_Type", elfConfiguration[MS2000].vtType);
    
    long value = elfConfiguration[MS2000].vt52SetUpFeature_.to_ulong();
    configPointer->Write("/MS2000/VT52Setup", value);
    value = elfConfiguration[MS2000].vt100SetUpFeature_.to_ulong();
    configPointer->Write("/MS2000/VT100Setup", value);
    value = elfConfiguration[MS2000].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("/MS2000/VTExternalSetup", value);
    
	configPointer->Write("/MS2000/Vt_Baud_Receive", elfConfiguration[MS2000].baudR);
	configPointer->Write("/MS2000/Vt_Baud_Transmit", elfConfiguration[MS2000].baudT);
	configPointer->Write("/MS2000/Vt_Zoom", conf[MS2000].zoomVt_);
    configPointer->Write("/MS2000/Force_Uppercase", elfConfiguration[MS2000].forceUpperCase);
    configPointer->Write("/MS2000/Enable_Vt_Stretch_Dot", conf[MS2000].stretchDot_);
    configPointer->Write("/MS2000/Enable_Vt_External", elfConfiguration[MS2000].vtExternal);
    configPointer->Write("/MS2000/Volume", conf[MS2000].volume_);
    configPointer->Write("/MS2000/Boot_From_Ram", elfConfiguration[MS2000].bootRam);
    
    configPointer->Write("/MS2000/Clock_Speed", conf[MS2000].clock_);
    configPointer->Write("/MS2000/Turbo_Clock_Speed", conf[MS2000].turboClock_);
    configPointer->Write("/MS2000/Enable_Turbo_Cassette", conf[MS2000].turbo_);
    configPointer->Write("/MS2000/Enable_Auto_Cassette", conf[MS2000].autoCassetteLoad_);
    configPointer->Write("/MS2000/Enable_Real_Cassette", conf[MS2000].realCassetteLoad_);
    configPointer->Write("/MS2000/Print_Mode", conf[MS2000].printMode_);
    
    configPointer->Write("/MS2000/DirectoryMode_0", directoryMode_[0]);
    configPointer->Write("/MS2000/DirectoryMode_1", directoryMode_[1]);
    configPointer->Write("/MS2000/DirectoryMode_2", directoryMode_[2]);
    configPointer->Write("/MS2000/DirectoryMode_3", directoryMode_[3]);
}

void GuiMS2000::readMS2000WindowConfig()
{
    conf[MS2000].vtX_ = (int)configPointer->Read("/MS2000/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX);
    conf[MS2000].vtY_ = (int)configPointer->Read("/MS2000/Window_Position_Vt_Y", mainWindowY_);
    conf[MS2000].mainX_ = (int)configPointer->Read("/MS2000/Window_Position_X", mainWindowX_);
    conf[MS2000].mainY_ = (int)configPointer->Read("/MS2000/Window_Position_Y", mainWindowY_+windowInfo.mainwY);
}

void GuiMS2000::writeMS2000WindowConfig()
{
    if (conf[MS2000].vtX_ > 0)
        configPointer->Write("/MS2000/Window_Position_Vt_X", conf[MS2000].vtX_);
    if (conf[MS2000].vtY_ > 0)
        configPointer->Write("/MS2000/Window_Position_Vt_Y", conf[MS2000].vtY_);
    if (conf[MS2000].mainX_ > 0)
        configPointer->Write("/MS2000/Window_Position_X", conf[MS2000].mainX_);
    if (conf[MS2000].mainY_ > 0)
        configPointer->Write("/MS2000/Window_Position_Y", conf[MS2000].mainY_);
    
}

void GuiMS2000::onMS2000BaudR(wxCommandEvent&event)
{
	elfConfiguration[MS2000].baudR = event.GetSelection();
}

void GuiMS2000::onMS2000BaudT(wxCommandEvent&event)
{
	elfConfiguration[MS2000].baudT = event.GetSelection();
//    elfConfiguration[MS2000].baudR = event.GetSelection();
//    XRCCTRL(*this, "VTBaudRChoiceMS2000", wxChoice)->SetSelection(elfConfiguration[MS2000].baudR);
}

void GuiMS2000::onMS2000Uart(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this, "VTBaudRChoiceMS2000", wxChoice)->SetSelection(elfConfiguration[MS2000].baudR);
    XRCCTRL(*this, "VTBaudTChoiceMS2000", wxChoice)->SetSelection(elfConfiguration[MS2000].baudT);
}

void GuiMS2000::onMS2000ForceUpperCase(wxCommandEvent&event)
{
    elfConfiguration[MS2000].forceUpperCase = event.IsChecked();
    if (runningComputer_ == MS2000)
    {
        p_Computer->setForceUpperCase(event.IsChecked());
    }
}

void GuiMS2000::onBootRam(wxCommandEvent&event)
{
	elfConfiguration[MS2000].bootRam = event.IsChecked();
	if (runningComputer_ == MS2000)
	{
		p_Mcds->setBootRam(event.IsChecked());
	}
}

void GuiMS2000::onMs2000Disk0(wxCommandEvent& WXUNUSED(event) )
{
    if (directoryMode_[0])
    {
        wxString dirName = wxDirSelector( "Select the FDC 0 Directory", floppyDirSwitchedMs2000_[0]);
        if (!dirName)
            return;
        
        floppyDirSwitchedMs2000_[0] = dirName + pathSeparator_;
 
        if (mode_.gui)
        {
            wxFileName setectedDirFile = wxFileName(floppyDirSwitchedMs2000_[0]);
            wxArrayString dirArray = setectedDirFile.GetDirs();
            dirName = dirArray.Last();
            XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->SetValue(dirName);
        }
    }
    else
    {
        wxString fileName;
        
        fileName = wxFileSelector( "Select the FDC 0 file to load",
                                  floppyDirMs2000_[0], floppyMs2000_[0],
                                  "img",
                                  wxString::Format
                                  (
                                   "FDC Image (*.img)|*.img|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                                   ),
                                  wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                                  this
                                  );
        if (!fileName)
            return;
        
        wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
        floppyDirMs2000_[0] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
        floppyMs2000_[0] = FullPath.GetFullName();
        
        if (mode_.gui)
            XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[0]);
    }
}

void GuiMS2000::onMs2000DiskText0(wxCommandEvent&event)
{
    if (directoryMode_[0])
    {
        if (runningComputer_ == MS2000)
            p_Ms2000->setDiskName(1, floppyDirSwitchedMs2000_[0], "");
        return;
    }
    
    floppyMs2000_[0] = event.GetString();
    if (runningComputer_ == MS2000)
    {
        if (floppyMs2000_[0].Len() == 0)
            p_Ms2000->setDiskName(1, floppyDirMs2000_[0], "");
        else
            p_Ms2000->setDiskName(1, floppyDirMs2000_[0], floppyMs2000_[0]);
    }
}

void GuiMS2000::onMs2000DiskEject0(wxCommandEvent& WXUNUSED(event) )
{
    floppyMs2000_[0] = "";
    if (mode_.gui)
        XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[0]);
}

void GuiMS2000::onMS2000DiskDirSwitch0(wxCommandEvent&WXUNUSED(event))
{
    if (directoryMode_[0])
    {
        directoryMode_[0] = false;
        XRCCTRL(*this, "FDC0_ButtonMS2000", wxButton)->SetLabel("FDC 0");
        XRCCTRL(*this, "FDC0_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 FDC 0 image file");
        XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->Enable(true);
        XRCCTRL(*this, "Eject_FDC0MS2000", wxBitmapButton)->Enable(true);

        XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[0]);
    }
    else
    {
        directoryMode_[0] = true;
        XRCCTRL(*this, "FDC0_ButtonMS2000", wxButton)->SetLabel("HD 0");
        XRCCTRL(*this, "FDC0_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 HD Directory 0");
        XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "Eject_FDC0MS2000", wxBitmapButton)->Enable(false);

        wxFileName setectedDirFile = wxFileName(floppyDirSwitchedMs2000_[0]);
        wxArrayString dirArray = setectedDirFile.GetDirs();
        wxString dirName = dirArray.Last();
        XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->SetValue(dirName);
    }
}

void GuiMS2000::onMs2000Disk1(wxCommandEvent& WXUNUSED(event) )
{
    if (directoryMode_[1])
    {
        wxString dirName = wxDirSelector( "Select the FDC 1 Directory", floppyDirSwitchedMs2000_[1]);
        if (!dirName)
            return;
        
        floppyDirSwitchedMs2000_[1] = dirName + pathSeparator_;
        
        if (mode_.gui)
        {
            wxFileName setectedDirFile = wxFileName(floppyDirSwitchedMs2000_[1]);
            wxArrayString dirArray = setectedDirFile.GetDirs();
            dirName = dirArray.Last();
            XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->SetValue(dirName);
        }
    }
    else
    {
        wxString fileName;
    
        fileName = wxFileSelector( "Select the FDC 1 file to load",
                                  floppyDirMs2000_[1], floppyMs2000_[1],
                                  "img",
                                  wxString::Format
                                  (
                                   "FDC Image (*.img)|*.img|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                                   ),
                                  wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                                  this
                                  );
        if (!fileName)
            return;
        
        wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
        floppyDirMs2000_[1] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
        floppyMs2000_[1] = FullPath.GetFullName();
        
        if (mode_.gui)
            XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[1]);
    }
}

void GuiMS2000::onMs2000DiskText1(wxCommandEvent&event)
{
    if (directoryMode_[1])
    {
        if (runningComputer_ == MS2000)
            p_Ms2000->setDiskName(2, floppyDirSwitchedMs2000_[1], "");
        return;
    }
    
    floppyMs2000_[1] = event.GetString();
    if (runningComputer_ == MS2000)
    {
        if (floppyMs2000_[1].Len() == 0)
            p_Ms2000->setDiskName(2, floppyDirMs2000_[1], "");
        else
            p_Ms2000->setDiskName(2, floppyDirMs2000_[1], floppyMs2000_[1]);
    }
}

void GuiMS2000::onMs2000DiskEject1(wxCommandEvent& WXUNUSED(event) )
{
    floppyMs2000_[1] = "";
    if (mode_.gui)
        XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[1]);
}

void GuiMS2000::onMS2000DiskDirSwitch1(wxCommandEvent&WXUNUSED(event))
{
    if (directoryMode_[1])
    {
        directoryMode_[1] = false;
        XRCCTRL(*this, "FDC1_ButtonMS2000", wxButton)->SetLabel("FDC 1");
        XRCCTRL(*this, "FDC1_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 FDC 1 image file");
        XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->Enable(true);
        XRCCTRL(*this, "Eject_FDC1MS2000", wxBitmapButton)->Enable(true);

        XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[1]);
    }
    else
    {
        directoryMode_[1] = true;
        XRCCTRL(*this, "FDC1_ButtonMS2000", wxButton)->SetLabel("HD 1");
        XRCCTRL(*this, "FDC1_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 HD Directory 1");
        XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "Eject_FDC1MS2000", wxBitmapButton)->Enable(false);

        wxFileName setectedDirFile = wxFileName(floppyDirSwitchedMs2000_[1]);
        wxArrayString dirArray = setectedDirFile.GetDirs();
        wxString dirName = dirArray.Last();
        XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->SetValue(dirName);
    }
}

void GuiMS2000::onMs2000Disk2(wxCommandEvent& WXUNUSED(event) )
{
    if (directoryMode_[2])
    {
        wxString dirName = wxDirSelector( "Select the FDC 2 Directory", floppyDirSwitchedMs2000_[2]);
        if (!dirName)
            return;
        
        floppyDirSwitchedMs2000_[2] = dirName + pathSeparator_;
        
        if (mode_.gui)
        {
            wxFileName setectedDirFile = wxFileName(floppyDirSwitchedMs2000_[2]);
            wxArrayString dirArray = setectedDirFile.GetDirs();
            dirName = dirArray.Last();
            XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->SetValue(dirName);
        }
    }
    else
    {
        wxString fileName;
    
        fileName = wxFileSelector( "Select the FDC 2 file to load",
                                  floppyDirMs2000_[2], floppyMs2000_[2],
                                  "img",
                                  wxString::Format
                                  (
                                   "FDC Image (*.img)|*.img|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                                   ),
                                  wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                                  this
                                  );
        if (!fileName)
            return;
        
        wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
        floppyDirMs2000_[2] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
        floppyMs2000_[2] = FullPath.GetFullName();
        
        if (mode_.gui)
            XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[2]);
    }
}

void GuiMS2000::onMs2000DiskText2(wxCommandEvent&event)
{
    if (directoryMode_[2])
    {
        if (runningComputer_ == MS2000)
            p_Ms2000->setDiskName(3, floppyDirSwitchedMs2000_[2], "");
        return;
    }
    
    floppyMs2000_[2] = event.GetString();
    if (runningComputer_ == MS2000)
    {
        if (floppyMs2000_[2].Len() == 0)
            p_Ms2000->setDiskName(3, floppyDirMs2000_[2], "");
        else
            p_Ms2000->setDiskName(3, floppyDirMs2000_[2], floppyMs2000_[2]);
    }
}

void GuiMS2000::onMs2000DiskEject2(wxCommandEvent& WXUNUSED(event) )
{
    floppyMs2000_[2] = "";
    if (mode_.gui)
        XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[2]);
}

void GuiMS2000::onMS2000DiskDirSwitch2(wxCommandEvent&WXUNUSED(event))
{
    if (directoryMode_[2])
    {
        directoryMode_[2] = false;
        XRCCTRL(*this, "FDC2_ButtonMS2000", wxButton)->SetLabel("FDC 2");
        XRCCTRL(*this, "FDC2_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 FDC 2 image file");
        XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->Enable(true);
        XRCCTRL(*this, "Eject_FDC2MS2000", wxBitmapButton)->Enable(true);
        
        XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[2]);
    }
    else
    {
        directoryMode_[2] = true;
        XRCCTRL(*this, "FDC2_ButtonMS2000", wxButton)->SetLabel("HD 2");
        XRCCTRL(*this, "FDC2_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 HD Directory 2");
        XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "Eject_FDC2MS2000", wxBitmapButton)->Enable(false);

        wxFileName setectedDirFile = wxFileName(floppyDirSwitchedMs2000_[2]);
        wxArrayString dirArray = setectedDirFile.GetDirs();
        wxString dirName = dirArray.Last();
        XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->SetValue(dirName);
    }
}

void GuiMS2000::onMs2000Disk3(wxCommandEvent& WXUNUSED(event) )
{
    if (directoryMode_[3])
    {
        wxString dirName = wxDirSelector( "Select the FDC 3 Directory", floppyDirSwitchedMs2000_[3]);
        if (!dirName)
            return;
        
        floppyDirSwitchedMs2000_[3] = dirName + pathSeparator_;
        
        if (mode_.gui)
        {
            wxFileName setectedDirFile = wxFileName(floppyDirSwitchedMs2000_[3]);
            wxArrayString dirArray = setectedDirFile.GetDirs();
            dirName = dirArray.Last();
            XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->SetValue(dirName);
        }
    }
    else
    {
        wxString fileName;
    
        fileName = wxFileSelector( "Select the FDC 3 file to load",
                                  floppyDirMs2000_[3], floppyMs2000_[3],
                                  "img",
                                  wxString::Format
                                  (
                                   "FDC Image (*.img)|*.img|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                                   ),
                                  wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                                  this
                                  );
        if (!fileName)
            return;
        
        wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
        floppyDirMs2000_[3] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
        floppyMs2000_[3] = FullPath.GetFullName();
        
        if (mode_.gui)
            XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[3]);
    }
}

void GuiMS2000::onMs2000DiskText3(wxCommandEvent&event)
{
    if (directoryMode_[3])
    {
        if (runningComputer_ == MS2000)
            p_Ms2000->setDiskName(4, floppyDirSwitchedMs2000_[3], "");
        return;
    }
    
    floppyMs2000_[3] = event.GetString();
    if (runningComputer_ == MS2000)
    {
        if (floppyMs2000_[3].Len() == 0)
            p_Ms2000->setDiskName(4, floppyDirMs2000_[3], "");
        else
            p_Ms2000->setDiskName(4, floppyDirMs2000_[3], floppyMs2000_[3]);
    }
}

void GuiMS2000::onMs2000DiskEject3(wxCommandEvent& WXUNUSED(event) )
{
    floppyMs2000_[3] = "";
    if (mode_.gui)
        XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[3]);
}

void GuiMS2000::onMS2000DiskDirSwitch3(wxCommandEvent&WXUNUSED(event))
{
    if (directoryMode_[3])
    {
        directoryMode_[3] = false;
        XRCCTRL(*this, "FDC3_ButtonMS2000", wxButton)->SetLabel("FDC 3");
        XRCCTRL(*this, "FDC3_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 FDC 3 image file");
        XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->Enable(true);
        XRCCTRL(*this, "Eject_FDC3MS2000", wxBitmapButton)->Enable(true);
 
        XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->SetValue(floppyMs2000_[3]);
    }
    else
    {
        directoryMode_[3] = true;
        XRCCTRL(*this, "FDC3_ButtonMS2000", wxButton)->SetLabel("HD 3");
        XRCCTRL(*this, "FDC3_ButtonMS2000", wxButton)->SetToolTip("Browse for MS2000 HD Directory 3");
        XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "Eject_FDC3MS2000", wxBitmapButton)->Enable(false);

        wxFileName setectedDirFile = wxFileName(floppyDirSwitchedMs2000_[3]);
        wxArrayString dirArray = setectedDirFile.GetDirs();
        wxString dirName = dirArray.Last();
        XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->SetValue(dirName);
    }
}

bool GuiMS2000::getDirectoryMode(int drive)
{
    return directoryMode_[drive];
}

void GuiMS2000::setDirectoryMode(int drive, bool state)
{
    directoryMode_[drive] = state;
}

wxString GuiMS2000::getMs2000FloppyDirSwitched(int drive)
{
    return floppyDirSwitchedMs2000_[drive];
}

wxString GuiMS2000::getMs2000FloppyDir(int drive)
{
    return floppyDirMs2000_[drive];
}

wxString GuiMS2000::getMs2000FloppyFile(int drive)
{
    return floppyMs2000_[drive];
}

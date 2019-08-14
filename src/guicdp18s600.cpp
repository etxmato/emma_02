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
#include "guicdp18s600.h"
#include "microsetup.h"

BEGIN_EVENT_TABLE(GuiCdp18s600, GuiTMC2000)

    EVT_TEXT(XRCID("MainRomU21CDP18S600"), GuiCdp18s600::onRomU21Text)
    EVT_COMBOBOX(XRCID("MainRomU21CDP18S600"), GuiCdp18s600::onRomU21Text)
    EVT_BUTTON(XRCID("RomButtonU21CDP18S600"), GuiCdp18s600::onRomU21)

    EVT_TEXT(XRCID("MainRomU20CDP18S600"), GuiCdp18s600::onRomU20Text)
    EVT_COMBOBOX(XRCID("MainRomU20CDP18S600"), GuiCdp18s600::onRomU20Text)
    EVT_BUTTON(XRCID("RomButtonU20CDP18S600"), GuiCdp18s600::onRomU20)

    EVT_TEXT(XRCID("MainRomU19CDP18S600"), GuiCdp18s600::onRomU19Text)
    EVT_COMBOBOX(XRCID("MainRomU19CDP18S600"), GuiCdp18s600::onRomU19Text)
    EVT_BUTTON(XRCID("RomButtonU19CDP18S600"), GuiCdp18s600::onRomU19)

    EVT_TEXT(XRCID("MainRomU18CDP18S600"), GuiCdp18s600::onRomU18Text)
    EVT_COMBOBOX(XRCID("MainRomU18CDP18S600"), GuiCdp18s600::onRomU18Text)
    EVT_BUTTON(XRCID("RomButtonU18CDP18S600"), GuiCdp18s600::onRomU18)

    EVT_TEXT(XRCID("MainRomU17CDP18S600"), GuiCdp18s600::onRomU17Text)
    EVT_COMBOBOX(XRCID("MainRomU17CDP18S600"), GuiCdp18s600::onRomU17Text)
    EVT_BUTTON(XRCID("RomButtonU17CDP18S600"), GuiCdp18s600::onRomU17)

    EVT_CHOICE(XRCID("OneSocketBank"), GuiCdp18s600::onOneSocketBank)
    EVT_CHOICE(XRCID("FourSocketBank"), GuiCdp18s600::onFourSocketBank)

    EVT_TEXT(XRCID("VtCharRomCDP18S600"), GuiMain::onVtCharRomText)
    EVT_COMBOBOX(XRCID("VtCharRomCDP18S600"), GuiMain::onVtCharRomText)
    EVT_BUTTON(XRCID("VtCharRomButtonCDP18S600"), GuiMain::onVtCharRom)

    EVT_TEXT(XRCID("KeyFileCDP18S600"), GuiMain::onKeyFileText)
    EVT_BUTTON(XRCID("KeyFileButtonCDP18S600"), GuiMain::onKeyFile)
    EVT_BUTTON(XRCID("EjectKeyFileCDP18S600"), GuiMain::onKeyFileEject)

    EVT_CHOICE(XRCID("VTTypeCDP18S600"), GuiMain::onVT100)
    EVT_SPIN_UP(XRCID("ZoomSpinVtCDP18S600"), GuiMain::onZoomUpVt)
    EVT_SPIN_DOWN(XRCID("ZoomSpinVtCDP18S600"), GuiMain::onZoomDownVt)
    EVT_TEXT(XRCID("ZoomValueVtCDP18S600"), GuiMain::onZoomValueVt)
    EVT_BUTTON(XRCID("FullScreenF3CDP18S600"), GuiMain::onFullScreen)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonCDP18S600"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileCDP18S600"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileCDP18S600"), GuiMain::onScreenDumpFileText)
    EVT_BUTTON(XRCID("ScreenDumpF5CDP18S600"), GuiMain::onScreenDump)

    EVT_CHECKBOX(XRCID("CDP18S600ForceUC"), GuiCdp18s600::onCdp18s600ForceUpperCase)
    EVT_BUTTON(XRCID("VtSetupCDP18S600"), GuiMain::onVtSetup)
    EVT_CHECKBOX(XRCID("StretchDotCDP18S600"), GuiMain::onStretchDot)
    EVT_BUTTON(XRCID("ColoursCDP18S600"), Main::onColoursDef)

    EVT_BUTTON(XRCID("SaveButtonCDP18S600"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonCDP18S600"), GuiMain::onLoadButton)
    EVT_BUTTON(XRCID("RunButtonCDP18S600"), GuiMain::onLoadRunButton)
    EVT_BUTTON(XRCID("DsaveButtonCDP18S600"), GuiMain::onDataSaveButton)

    EVT_CHECKBOX(XRCID("UseLocationCDP18S600"), GuiMain::onUseLocation)
    EVT_TEXT(XRCID("SaveStartCDP18S600"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndCDP18S600"), GuiMain::onSaveEnd)

    EVT_CHOICE(XRCID("VTBaudTChoiceCDP18S600"), GuiCdp18s600::onCdp18s600BaudT)

    EVT_BUTTON(XRCID("OneSocketSetup"), GuiCdp18s600::onOneSocketSetup)
    EVT_BUTTON(XRCID("FourSocketSetup"), GuiCdp18s600::onFourSocketSetup)

    EVT_CHECKBOX(XRCID("AutoBootCDP18S600"), GuiCdp18s600::onAutoBoot)
    EVT_CHOICE(XRCID("AutoBootTypeCDP18S600"), GuiCdp18s600::onAutoBootType)

END_EVENT_TABLE()

GuiCdp18s600::GuiCdp18s600(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode_, wxString dataDir, wxString iniDir)
: GuiTMC2000(title, pos, size, mode_, dataDir, iniDir)
{
    conf[CDP18S600].loadFileNameFull_ = "";
    conf[CDP18S600].loadFileName_ = "";
    
    conf[CDP18S600].pLoadSaveName_[0] = 'S';
    conf[CDP18S600].pLoadSaveName_[1] = 'U';
    conf[CDP18S600].pLoadSaveName_[2] = 'P';
    conf[CDP18S600].pLoadSaveName_[3] = 'E';
    
    conf[CDP18S600].defus_ = 0x81;
    conf[CDP18S600].eop_ = 0x83;
    conf[CDP18S600].string_ = 0x92;
    conf[CDP18S600].arrayValue_ = 0x94;
    conf[CDP18S600].eod_ = 0x99;
    conf[CDP18S600].basicRamAddress_ = 0x200;
    
    conf[CDP18S600].saveStartString_ = "";
    conf[CDP18S600].saveEndString_ = "";
    conf[CDP18S600].saveExecString_ = "";
    
    position_.x = 0;
}

void GuiCdp18s600::readCdp18s600Config()
{
    selectedComputer_ = CDP18S600;
    
    conf[CDP18S600].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "CDP18S600" + pathSeparator_;
    
    conf[CDP18S600].mainDir_ = readConfigDir("/Dir/CDP18S600/Main", dataDir_ + "CDP18S600" + pathSeparator_);
    conf[CDP18S600].romDir_[U21ROM] = readConfigDir("/Dir/CDP18S600/Main_Rom_File21", dataDir_ + "CDP18S600" + pathSeparator_);
    conf[CDP18S600].romDir_[U20ROM] = readConfigDir("/Dir/CDP18S600/Main_Rom_File20", dataDir_ + "CDP18S600" + pathSeparator_);
    conf[CDP18S600].romDir_[U19ROM] = readConfigDir("/Dir/CDP18S600/Main_Rom_File19", dataDir_ + "CDP18S600" + pathSeparator_);
    conf[CDP18S600].romDir_[U18ROM] = readConfigDir("/Dir/CDP18S600/Main_Rom_File18", dataDir_ + "CDP18S600" + pathSeparator_);
    conf[CDP18S600].romDir_[U17ROM] = readConfigDir("/Dir/CDP18S600/Main_Rom_File17", dataDir_ + "CDP18S600" + pathSeparator_);
    conf[CDP18S600].vtCharRomDir_ = readConfigDir("/Dir/CDP18S600/Vt_Font_Rom_File", dataDir_ + "CDP18S600" + pathSeparator_);
    conf[CDP18S600].keyFileDir_ = readConfigDir("/Dir/CDP18S600/Key_File", dataDir_ + "CDP18S600" + pathSeparator_);
    conf[CDP18S600].screenDumpFileDir_ = readConfigDir("/Dir/CDP18S600/Video_Dump_File", dataDir_ + "CDP18S600" + pathSeparator_);
    
    conf[CDP18S600].rom_[U21ROM] = configPointer->Read("/CDP18S600/Main_Rom_File21", "");
    conf[CDP18S600].rom_[U20ROM] = configPointer->Read("/CDP18S600/Main_Rom_File20", "ut71.bin");
    conf[CDP18S600].rom_[U19ROM] = configPointer->Read("/CDP18S600/Main_Rom_File19", "");
    conf[CDP18S600].rom_[U18ROM] = configPointer->Read("/CDP18S600/Main_Rom_File18", "");
    conf[CDP18S600].rom_[U17ROM] = configPointer->Read("/CDP18S600/Main_Rom_File17", "");
    conf[CDP18S600].keyFile_ = configPointer->Read("/CDP18S600/Key_File", "");
    conf[CDP18S600].screenDumpFile_ = configPointer->Read("/CDP18S600/Video_Dump_File", "screendump.png");
    
    getConfigBool("/CDP18S600/SerialLog", false);
    
    configPointer->Read("/CDP18S600/Enable_Vt_Stretch_Dot", &conf[CDP18S600].stretchDot_, false);
    configPointer->Read("/CDP18S600/Enable_Vt_External", &elfConfiguration[CDP18S600].vtExternal, false);
    
    configPointer->Read("/CDP18S600/Uart", &elfConfiguration[CDP18S600].useUart, true);
    elfConfiguration[CDP18S600].uartGroup = (int)configPointer->Read("/CDP18S600/UartGroup", 0l);
    elfConfiguration[CDP18S600].bellFrequency_ = (int)configPointer->Read("/CDP18S600/Bell_Frequency", 800);
    elfConfiguration[CDP18S600].baudR = (int)configPointer->Read("/CDP18S600/Vt_Baud_Receive", 0l);
    elfConfiguration[CDP18S600].baudT = (int)configPointer->Read("/CDP18S600/Vt_Baud_Transmit", 0l);
    elfConfiguration[CDP18S600].vtType = (int)configPointer->Read("/CDP18S600/VT_Type", 2l);
    elfConfiguration[CDP18S600].vt52SetUpFeature_ = configPointer->Read("/CDP18S600/VT52Setup", 0x00004092l);
    elfConfiguration[CDP18S600].vt100SetUpFeature_ = configPointer->Read("/CDP18S600/VT100Setup", 0x0000cad2l);
    elfConfiguration[CDP18S600].vtExternalSetUpFeature_ = configPointer->Read("/CDP18S600/VTExternalSetup", 0x0000cad2l);
    elfConfiguration[CDP18S600].serialPort_ = configPointer->Read("/CDP18S600/VtSerialPortChoice", "");
    elfConfiguration[CDP18S600].vtEf = false;
    elfConfiguration[CDP18S600].vtQ = true;

    configPointer->Read("/CDP18S600/Force_Uppercase", &elfConfiguration[CDP18S600].forceUpperCase, true);
    
    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 1.0);
    conf[CDP18S600].zoomVt_ = configPointer->Read("/CDP18S600/Vt_Zoom", defaultZoom);
    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[CDP18S600].xScale_ = configPointer->Read("/CDP18S600/Window_Scale_Factor_X", defaultScale);
    
    wxString defaultClock;
    defaultClock.Printf("%1.4f", 4.9152);
    conf[CDP18S600].clock_ = configPointer->Read("/CDP18S600/Clock_Speed", defaultClock);
    
    conf[CDP18S600].useLoadLocation_ = false;
    
    setVtType("CDP18S600", CDP18S600, elfConfiguration[CDP18S600].vtType, false);
    
    conf[CDP18S600].vtCharRom_ = configPointer->Read("/CDP18S600/Vt_Font_Rom_File", "vt100.bin");
    
    conf[CDP18S600].microChipType_[ONE_SOCKET] = (int)configPointer->Read("/CDP18S600/MicroChipTypeOneSocket", 2l);
    conf[CDP18S600].microChipType_[FOUR_SOCKET] = (int)configPointer->Read("/CDP18S600/MicroChipTypeFourSocket", 0l);

    conf[CDP18S600].microChipLocation_[ONE_SOCKET] = (int)configPointer->Read("/CDP18S600/MicroChipLocationOneSocket", 0l);
    conf[CDP18S600].microChipLocation_[FOUR_SOCKET] = (int)configPointer->Read("/CDP18S600/MicroChipLocationFourSocket", 4l);

    conf[CDP18S600].microChipMemory_[U21ROM] = (int)configPointer->Read("/CDP18S600/MicroChipMemoryU21", 1l);
    configPointer->Read("/CDP18S600/MicroChiDisableU21", &conf[CDP18S600].microChipDisable_[U21ROM], false);

    conf[CDP18S600].microChipMemory_[U20ROM] = (int)configPointer->Read("/CDP18S600/MicroChipMemoryU20", 0l);
    configPointer->Read("/CDP18S600/MicroChiDisableU20", &conf[CDP18S600].microChipDisable_[U20ROM], false);

    conf[CDP18S600].microChipMemory_[U19ROM] = (int)configPointer->Read("/CDP18S600/MicroChipMemoryU19", 1l);
    configPointer->Read("/CDP18S600/MicroChiDisableU19", &conf[CDP18S600].microChipDisable_[U19ROM], false);

    conf[CDP18S600].microChipMemory_[U18ROM] = (int)configPointer->Read("/CDP18S600/MicroChipMemoryU18", 0l);
    configPointer->Read("/CDP18S600/MicroChiDisableU18", &conf[CDP18S600].microChipDisable_[U18ROM], true);

    conf[CDP18S600].microChipMemory_[U17ROM] = (int)configPointer->Read("/CDP18S600/MicroChipMemoryU17", 0l);
    configPointer->Read("/CDP18S600/MicroChiDisableU17", &conf[CDP18S600].microChipDisable_[U17ROM], true);

    configPointer->Read("/CDP18S600/Enable_Auto_Boot", &elfConfiguration[CDP18S600].autoBoot, true);
    elfConfiguration[CDP18S600].autoBootType = (int)configPointer->Read("/CDP18S600/AutoBootType", 0l);
    if (elfConfiguration[CDP18S600].autoBootType == 0)
        conf[CDP18S600].bootAddress_ = 0x8000;
    else
        conf[CDP18S600].bootAddress_ = 0;

    if (mode_.gui)
    {
        XRCCTRL(*this, "MainRomU21CDP18S600", wxComboBox)->SetValue(conf[CDP18S600].rom_[U21ROM]);
        XRCCTRL(*this, "MainRomU20CDP18S600", wxComboBox)->SetValue(conf[CDP18S600].rom_[U20ROM]);
        XRCCTRL(*this, "MainRomU19CDP18S600", wxComboBox)->SetValue(conf[CDP18S600].rom_[U19ROM]);
        XRCCTRL(*this, "MainRomU18CDP18S600", wxComboBox)->SetValue(conf[CDP18S600].rom_[U18ROM]);
        XRCCTRL(*this, "MainRomU17CDP18S600", wxComboBox)->SetValue(conf[CDP18S600].rom_[U17ROM]);
        XRCCTRL(*this, "VtCharRomCDP18S600", wxComboBox)->SetValue(conf[CDP18S600].vtCharRom_);
        XRCCTRL(*this, "KeyFileCDP18S600", wxTextCtrl)->SetValue(conf[CDP18S600].keyFile_);
        XRCCTRL(*this, "ScreenDumpFileCDP18S600", wxComboBox)->SetValue(conf[CDP18S600].screenDumpFile_);
        
        XRCCTRL(*this, "VTTypeCDP18S600", wxChoice)->SetSelection(elfConfiguration[CDP18S600].vtType);
        XRCCTRL(*this, "CDP18S600ForceUC", wxCheckBox)->SetValue(elfConfiguration[CDP18S600].forceUpperCase);
        
        XRCCTRL(*this, "VTBaudRChoiceCDP18S600", wxChoice)->SetSelection(elfConfiguration[CDP18S600].baudR);
        XRCCTRL(*this, "VTBaudTChoiceCDP18S600", wxChoice)->SetSelection(elfConfiguration[CDP18S600].baudT);
        
        XRCCTRL(*this, "ZoomValueVtCDP18S600", wxTextCtrl)->ChangeValue(conf[CDP18S600].zoomVt_);
        XRCCTRL(*this, "StretchDotCDP18S600", wxCheckBox)->SetValue(conf[CDP18S600].stretchDot_);
        
        clockTextCtrl[CDP18S600]->ChangeValue(conf[CDP18S600].clock_);
        
        XRCCTRL(*this, "UseLocationCDP18S600", wxCheckBox)->SetValue(conf[CDP18S600].useLoadLocation_);
        
        XRCCTRL(*this, "OneSocketBank", wxChoice)->SetSelection(conf[CDP18S600].microChipType_[ONE_SOCKET]);
        XRCCTRL(*this, "FourSocketBank", wxChoice)->SetSelection(conf[CDP18S600].microChipType_[FOUR_SOCKET]);

        XRCCTRL(*this, "AutoBootCDP18S600", wxCheckBox)->SetValue(elfConfiguration[CDP18S600].autoBoot);
        XRCCTRL(*this, "AutoBootTypeCDP18S600", wxChoice)->SetSelection(elfConfiguration[CDP18S600].autoBootType);

        setOneSocketState();
        setFourSocketState();
    }
}

void GuiCdp18s600::writeCdp18s600DirConfig()
{
    writeConfigDir("/Dir/CDP18S600/Main", conf[CDP18S600].mainDir_);
    writeConfigDir("/Dir/CDP18S600/Main_Rom_File21", conf[CDP18S600].romDir_[U21ROM]);
    writeConfigDir("/Dir/CDP18S600/Main_Rom_File20", conf[CDP18S600].romDir_[U20ROM]);
    writeConfigDir("/Dir/CDP18S600/Main_Rom_File19", conf[CDP18S600].romDir_[U19ROM]);
    writeConfigDir("/Dir/CDP18S600/Main_Rom_File18", conf[CDP18S600].romDir_[U18ROM]);
    writeConfigDir("/Dir/CDP18S600/Main_Rom_File18", conf[CDP18S600].romDir_[U17ROM]);
    writeConfigDir("/Dir/CDP18S600/Vt_Font_Rom_File", conf[CDP18S600].vtCharRomDir_);
    writeConfigDir("/Dir/CDP18S600/Key_File", conf[CDP18S600].keyFileDir_);
    writeConfigDir("/Dir/CDP18S600/Video_Dump_File", conf[CDP18S600].screenDumpFileDir_);
}

void GuiCdp18s600::writeCdp18s600Config()
{
    writeElfPortConfig(CDP18S600, "CDP18S600");
    
    configPointer->Write("/CDP18S600/Main_Rom_File21", conf[CDP18S600].rom_[U21ROM]);
    configPointer->Write("/CDP18S600/Main_Rom_File20", conf[CDP18S600].rom_[U20ROM]);
    configPointer->Write("/CDP18S600/Main_Rom_File19", conf[CDP18S600].rom_[U19ROM]);
    configPointer->Write("/CDP18S600/Main_Rom_File18", conf[CDP18S600].rom_[U18ROM]);
    configPointer->Write("/CDP18S600/Main_Rom_File17", conf[CDP18S600].rom_[U17ROM]);
    configPointer->Write("/CDP18S600/Vt_Font_Rom_File", conf[CDP18S600].vtCharRom_);
    configPointer->Write("/CDP18S600/Key_File", conf[CDP18S600].keyFile_);
    configPointer->Write("/CDP18S600/Video_Dump_File", conf[CDP18S600].screenDumpFile_);
    configPointer->Write("/CDP18S600/VtSerialPortChoice", elfConfiguration[CDP18S600].serialPort_);
    
    configPointer->Write("/CDP18S600/Bell_Frequency", elfConfiguration[CDP18S600].bellFrequency_);
    configPointer->Write("/CDP18S600/VT_Type", elfConfiguration[CDP18S600].vtType);
    
    long value = elfConfiguration[CDP18S600].vt52SetUpFeature_.to_ulong();
    configPointer->Write("/CDP18S600/VT52Setup", value);
    value = elfConfiguration[CDP18S600].vt100SetUpFeature_.to_ulong();
    configPointer->Write("/CDP18S600/VT100Setup", value);
    value = elfConfiguration[CDP18S600].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("/CDP18S600/VTExternalSetup", value);
    configPointer->Write("/CDP18S600/Uart", elfConfiguration[CDP18S600].useUart);
    configPointer->Write("/CDP18S600/UartGroup", elfConfiguration[CDP18S600].uartGroup);

    configPointer->Write("/CDP18S600/Vt_Baud_Receive", elfConfiguration[CDP18S600].baudR);
    configPointer->Write("/CDP18S600/Vt_Baud_Transmit", elfConfiguration[CDP18S600].baudT);
    configPointer->Write("/CDP18S600/Vt_Zoom", conf[CDP18S600].zoomVt_);
    configPointer->Write("/CDP18S600/Force_Uppercase", elfConfiguration[CDP18S600].forceUpperCase);
    configPointer->Write("/CDP18S600/Enable_Vt_Stretch_Dot", conf[CDP18S600].stretchDot_);
    configPointer->Write("/CDP18S600/Enable_Vt_External", elfConfiguration[CDP18S600].vtExternal);
    
    configPointer->Write("/CDP18S600/MicroChipTypeOneScoket", conf[CDP18S600].microChipType_[ONE_SOCKET]);
    configPointer->Write("/CDP18S600/MicroChipTypeFourScoket", conf[CDP18S600].microChipType_[FOUR_SOCKET]);

    configPointer->Write("/CDP18S600/MicroChipLocationOneScoket", conf[CDP18S600].microChipLocation_[ONE_SOCKET]);
    configPointer->Write("/CDP18S600/MicroChipLocationFourScoket", conf[CDP18S600].microChipLocation_[FOUR_SOCKET]);

    configPointer->Write("/CDP18S600/MicroChipMemoryU21", conf[CDP18S600].microChipMemory_[U21ROM]);
    configPointer->Write("/CDP18S600/MicroChiDisableU21", conf[CDP18S600].microChipDisable_[U21ROM]);

    configPointer->Write("/CDP18S600/MicroChipMemoryU20", conf[CDP18S600].microChipMemory_[U20ROM]);
    configPointer->Write("/CDP18S600/MicroChiDisableU20", conf[CDP18S600].microChipDisable_[U20ROM]);

    configPointer->Write("/CDP18S600/MicroChipMemoryU19", conf[CDP18S600].microChipMemory_[U19ROM]);
    configPointer->Write("/CDP18S600/MicroChiDisableU19", conf[CDP18S600].microChipDisable_[U19ROM]);

    configPointer->Write("/CDP18S600/MicroChipMemoryU18", conf[CDP18S600].microChipMemory_[U18ROM]);
    configPointer->Write("/CDP18S600/MicroChiDisableU18", conf[CDP18S600].microChipDisable_[U18ROM]);

    configPointer->Write("/CDP18S600/MicroChipMemoryU17", conf[CDP18S600].microChipMemory_[U17ROM]);
    configPointer->Write("/CDP18S600/MicroChiDisableU17", conf[CDP18S600].microChipDisable_[U17ROM]);

    configPointer->Write("/CDP18S600/Enable_Auto_Boot", elfConfiguration[CDP18S600].autoBoot);
    configPointer->Write("/CDP18S600/AutoBootType", elfConfiguration[CDP18S600].autoBootType);
}

void GuiCdp18s600::readCdp18s600WindowConfig()
{
    conf[CDP18S600].vtX_ = (int)configPointer->Read("/CDP18S600/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX);
    conf[CDP18S600].vtY_ = (int)configPointer->Read("/CDP18S600/Window_Position_Vt_Y", mainWindowY_);
    conf[CDP18S600].mainX_ = (int)configPointer->Read("/CDP18S600/Window_Position_X", mainWindowX_);
    conf[CDP18S600].mainY_ = (int)configPointer->Read("/CDP18S600/Window_Position_Y", mainWindowY_+windowInfo.mainwY);
}

void GuiCdp18s600::writeCdp18s600WindowConfig()
{
    if (conf[CDP18S600].vtX_ > 0)
        configPointer->Write("/CDP18S600/Window_Position_Vt_X", conf[CDP18S600].vtX_);
    if (conf[CDP18S600].vtY_ > 0)
        configPointer->Write("/CDP18S600/Window_Position_Vt_Y", conf[CDP18S600].vtY_);
    if (conf[CDP18S600].mainX_ > 0)
        configPointer->Write("/CDP18S600/Window_Position_X", conf[CDP18S600].mainX_);
    if (conf[CDP18S600].mainY_ > 0)
        configPointer->Write("/CDP18S600/Window_Position_Y", conf[CDP18S600].mainY_);
    
}

void GuiCdp18s600::onCdp18s600BaudT(wxCommandEvent&event)
{
    elfConfiguration[CDP18S600].baudT = event.GetSelection();
    elfConfiguration[CDP18S600].baudR = event.GetSelection();
    XRCCTRL(*this, "VTBaudRChoiceCDP18S600", wxChoice)->SetSelection(elfConfiguration[CDP18S600].baudR);
}

void GuiCdp18s600::onCdp18s600ForceUpperCase(wxCommandEvent&event)
{
    elfConfiguration[CDP18S600].forceUpperCase = event.IsChecked();
    if (runningComputer_ == CDP18S600)
    {
        p_Cdp18s600->setForceUpperCase(event.IsChecked());
    }
}

void GuiCdp18s600::onOneSocketBank(wxCommandEvent&event)
{
    conf[CDP18S600].microChipType_[ONE_SOCKET] = event.GetSelection();
}

void GuiCdp18s600::onFourSocketBank(wxCommandEvent&event)
{
    conf[CDP18S600].microChipType_[FOUR_SOCKET] = event.GetSelection();
}

void GuiCdp18s600::onRomU21(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" U21 ROM file to load",
                              conf[selectedComputer_].romDir_[U21ROM], XRCCTRL(*this, "MainRomU21"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    conf[selectedComputer_].romDir_[U21ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].rom_[U21ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU21"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[U21ROM]);
}

void GuiCdp18s600::onRomU21Text(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].rom_[U21ROM] = XRCCTRL(*this, "MainRomU21"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiCdp18s600::onRomU20(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" U210 ROM file to load",
                              conf[selectedComputer_].romDir_[U20ROM], XRCCTRL(*this, "MainRomU20"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    conf[selectedComputer_].romDir_[U20ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].rom_[U20ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU20"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[U20ROM]);
}

void GuiCdp18s600::onRomU20Text(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].rom_[U20ROM] = XRCCTRL(*this, "MainRomU20"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiCdp18s600::onRomU19(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" U19 ROM file to load",
                              conf[selectedComputer_].romDir_[U19ROM], XRCCTRL(*this, "MainRomU19"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    conf[selectedComputer_].romDir_[U19ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].rom_[U19ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU19"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[U19ROM]);
}

void GuiCdp18s600::onRomU19Text(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].rom_[U19ROM] = XRCCTRL(*this, "MainRomU19"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiCdp18s600::onRomU18(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" U18 ROM file to load",
                              conf[selectedComputer_].romDir_[U18ROM], XRCCTRL(*this, "MainRomU18"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    conf[selectedComputer_].romDir_[U18ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].rom_[U18ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU81"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[U18ROM]);
}

void GuiCdp18s600::onRomU18Text(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].rom_[U18ROM] = XRCCTRL(*this, "MainRomU18"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiCdp18s600::onRomU17(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" U21 ROM file to load",
                              conf[selectedComputer_].romDir_[U17ROM], XRCCTRL(*this, "MainRomU17"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    conf[selectedComputer_].romDir_[U17ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].rom_[U17ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU17"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[U17ROM]);
}

void GuiCdp18s600::onRomU17Text(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].rom_[U17ROM] = XRCCTRL(*this, "MainRomU17"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiCdp18s600::onOneSocketSetup(wxCommandEvent&WXUNUSED(event))
{
    MicroOneSocketSetupDialog MicroOneSocketSetupDialog(this);
    MicroOneSocketSetupDialog.ShowModal();
}

void GuiCdp18s600::setOneSocketState()
{
    setRamlabel(U21ROM, "U21");
}

void GuiCdp18s600::setRamlabel(int romNumber, wxString romString)
{
    wxString buttonString = "RomButton" + romString + "CDP18S600";
    wxString comboString = "MainRom" + romString + "CDP18S600";
    wxString typeString = "ROM ";
    
    switch (conf[selectedComputer_].microChipMemory_[romNumber])
    {
        case MICRO_RAM:
            typeString = "RAM ";
        break;
            
        case MICRO_CPURAM:
            typeString = "CRAM ";
        break;
    }
    XRCCTRL(*this, buttonString, wxButton)->SetLabel(typeString + romString);

    if (conf[selectedComputer_].microChipDisable_[romNumber])
    {
        XRCCTRL(*this, buttonString, wxButton)->Disable();
        XRCCTRL(*this, comboString, wxComboBox)->Disable();
    }
    else
    {
        XRCCTRL(*this, buttonString, wxButton)->Enable();
        XRCCTRL(*this, comboString, wxComboBox)->Enable();
    }
}

void GuiCdp18s600::onFourSocketSetup(wxCommandEvent&WXUNUSED(event))
{
    MicroFourSocketSetupDialog MicroFourSocketSetupDialog(this);
    MicroFourSocketSetupDialog.ShowModal();
}

void GuiCdp18s600::setFourSocketState()
{
    setRamlabel(U20ROM, "U20");
    setRamlabel(U19ROM, "U19");
    setRamlabel(U18ROM, "U18");
    setRamlabel(U17ROM, "U17");
}

void GuiCdp18s600::onAutoBoot(wxCommandEvent&event)
{
    elfConfiguration[CDP18S600].autoBoot = event.IsChecked();
}

void GuiCdp18s600::onAutoBootType(wxCommandEvent&event)
{
    elfConfiguration[CDP18S600].autoBootType = event.GetSelection();
    if (elfConfiguration[CDP18S600].autoBootType == 0)
        conf[CDP18S600].bootAddress_ = 0x8000;
    else
        conf[CDP18S600].bootAddress_ = 0;
    if (runningComputer_ == CDP18S600)
        p_Cdp18s600->setAddressLatch(conf[CDP18S600].bootAddress_);
}



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
#include "guivip2.h"
#include "pixie.h"

#include "psave.h"

DEFINE_EVENT_TYPE(STATUS_BAR_PIXIE)

BEGIN_EVENT_TABLE(GuiVipII, GuiVip2K)

    EVT_TEXT(XRCID("MainRomVipII"), GuiMain::onMainRom1Text)
    EVT_COMBOBOX(XRCID("MainRomVipII"), GuiMain::onMainRom1Text)
    EVT_BUTTON(XRCID("RomButtonVipII"), GuiMain::onMainRom1)

    EVT_TEXT(XRCID("MainRom2VipII"), GuiMain::onMainRom2Text)
    EVT_COMBOBOX(XRCID("MainRom2VipII"), GuiMain::onMainRom2Text)
    EVT_BUTTON(XRCID("RomButton2VipII"), GuiMain::onMainRom2)

    EVT_TEXT(XRCID("RamSWVipII"), GuiVipII::onRamSWTextII)
    EVT_COMBOBOX(XRCID("RamSWVipII"), GuiVipII::onRamSWTextII)
    EVT_BUTTON(XRCID("RamSWButtonVipII"), GuiVipII::onRamSWVipII)

    EVT_TEXT(XRCID("Chip8SWVipII"), GuiMain::onChip8SWText)
    EVT_BUTTON(XRCID("Chip8SWButtonVipII"), GuiMain::onChip8SW)
    EVT_BUTTON(XRCID("EjectChip8SWVipII"), GuiMain::onEjectChip8SW)

    EVT_SPIN_UP(XRCID("ZoomSpinVipII"), GuiMain::onZoom)
    EVT_SPIN_DOWN(XRCID("ZoomSpinVipII"), GuiMain::onZoom)
    EVT_TEXT(XRCID("ZoomValueVipII"), GuiMain::onZoomValue)
    EVT_BUTTON(XRCID("FullScreenF3VipII"), GuiMain::onFullScreen)
    EVT_BUTTON(XRCID("CasButtonVipII"), GuiMain::onCassette)
    EVT_BUTTON(XRCID("EjectCasVipII"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileVipII"), GuiMain::onCassetteText)
    EVT_BUTTON(XRCID("ScreenDumpFileButtonVipII"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileVipII"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileVipII"), GuiMain::onScreenDumpFileText)
    EVT_BUTTON(XRCID("ScreenDumpF5VipII"), GuiMain::onScreenDump)
    EVT_BUTTON(XRCID("RealCasLoadVipII"), GuiMain::onRealCas)
    EVT_BUTTON(XRCID("CasLoadVipII"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasSaveVipII"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasStopVipII"), GuiMain::onCassetteStop)
    EVT_CHECKBOX(XRCID("TurboVipII"), GuiMain::onTurbo)
    EVT_TEXT(XRCID("TurboClockVipII"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadVipII"), GuiMain::onAutoLoad)

    EVT_CHECKBOX(XRCID("UseLocationVipII"), GuiMain::onUseLocation)
    EVT_TEXT(XRCID("SaveStartVipII"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndVipII"), GuiMain::onSaveEnd)
    EVT_TEXT(XRCID("SaveExecVipII"), GuiMain::onSaveExec)

    EVT_BUTTON(XRCID("SaveButtonVipII"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonVipII"), GuiMain::onLoadButton)
    EVT_BUTTON(XRCID("RunButtonVipII"), GuiMain::onLoadRunButton)
    EVT_BUTTON(XRCID("DsaveButtonVipII"), GuiMain::onDataSaveButton)

    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeVipII"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeVipII"), GuiMain::onVolume)
    EVT_BUTTON(XRCID("KeyMapVipII"), Main::onHexKeyDef)
    EVT_BUTTON(XRCID("ColoursVipII"), Main::onColoursDef)

    EVT_COMMAND(wxID_ANY, STATUS_BAR_PIXIE, GuiVipII::pixieBarSize)

    EVT_TEXT(XRCID("ShowAddressVipII"), GuiMain::onLedTimer)

    EVT_CHOICE(XRCID("ComputerVersionVipII"), GuiVipII::onComputerVersion)

    EVT_CHOICE(XRCID("RamVipII"), GuiMain::onChoiceRam)
    EVT_CHECKBOX(XRCID("AutoBootVipII"), GuiVipII::onAutoBootVipII)
    EVT_CHOICE(XRCID("AutoBootTypeVipII"), GuiVipII::onAutoBootTypeVipII)


END_EVENT_TABLE()

GuiVipII::GuiVipII(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiVip2K(title, pos, size, mode, dataDir, iniDir)
{
    conf[VIPII].loadFileNameFull_ = "";
    conf[VIPII].loadFileName_ = "";

    conf[VIPII].pLoadSaveName_[0] = 'F';
    conf[VIPII].pLoadSaveName_[1] = 'P';
    conf[VIPII].pLoadSaveName_[2] = 'B';
    conf[VIPII].pLoadSaveName_[3] = ' ';

    conf[VIPII].saveStartString_ = "";
    conf[VIPII].saveEndString_ = "";
    conf[VIPII].saveExecString_ = "";
}

void GuiVipII::readVipIIConfig()
{
    selectedComputer_ = VIPII;

    conf[VIPII].emsConfigNumber_ = 0;
    conf[VIPII].videoNumber_ = 0;

    conf[VIPII].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "VipII" + pathSeparator_;
    conf[VIPII].mainDir_ = readConfigDir("/Dir/VipII/Main", dataDir_ + "VipII" + pathSeparator_);
    
    conf[VIPII].romDir_[MAINROM1] = readConfigDir("/Dir/VipII/Main_Rom_1_File", dataDir_ + "VipII"  + pathSeparator_);
    conf[VIPII].romDir_[MAINROM2] = readConfigDir("/Dir/VipII/Main_Rom_2_File", dataDir_ + "VipII"  + pathSeparator_);
    conf[VIPII].ramDir_ = readConfigDir("/Dir/VipII/Software_File", dataDir_ + "VipII"  + pathSeparator_);
    conf[VIPII].chip8SWDir_ = readConfigDir("/Dir/VipII/Chip_8_Software", dataDir_ + "Chip-8"  + pathSeparator_ + "Chip-8 Games"  + pathSeparator_);
    conf[VIPII].screenDumpFileDir_ = readConfigDir("/Dir/VipII/Video_Dump_File", dataDir_ + "VipII" + pathSeparator_);
    conf[VIPII].wavFileDir_[0] = readConfigDir("/Dir/VipII/Wav_File", dataDir_ + "VipII" + pathSeparator_);

    conf[VIPII].rom_[MAINROM1] = configPointer->Read("/VipII/Main_Rom_1_File", "fpb.rom");
    conf[VIPII].rom_[MAINROM2] = configPointer->Read("/VipII/Main_Rom_2_File", "vip.rom");
    conf[VIPII].ram_ = configPointer->Read("/VipII/Ram_Software", "");
    conf[VIPII].chip8SW_ = configPointer->Read("/VipII/Chip_8_Software", "");
    conf[VIPII].wavFile_[0] = configPointer->Read("/VipII/Wav_File", "");
    conf[VIPII].screenDumpFile_ = configPointer->Read("/VipII/Video_Dump_File", "screendump.png");
    conf[VIPII].turboClock_ = configPointer->Read("/VipII/Turbo_Clock_Speed", "15");
    configPointer->Read("/VipII/UseLoadLocation", &conf[VIPII].useLoadLocation_, false);

    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 2.0);
    conf[VIPII].zoom_[VIDEOMAIN] = convertLocale(configPointer->Read("/VipII/Zoom", defaultZoom));
    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[VIPII].xScale_ = convertLocale(configPointer->Read("/VipII/Window_Scale_Factor_X", defaultScale));

    configPointer->Read("/VipII/Enable_Turbo_Cassette", &conf[VIPII].turbo_, true);
    configPointer->Read("/VipII/Enable_Auto_Cassette", &conf[VIPII].autoCassetteLoad_, true);
    configPointer->Read("/VipII/Enable_Real_Cassette", &conf[VIPII].realCassetteLoad_, false);
    configPointer->Read("/VipII/Printer", &conf[VIPII].printerOn_, false);

    conf[VIPII].volume_ = (int)configPointer->Read("/VipII/Volume", 25l);
    conf[VIPII].printMode_ = (int)configPointer->Read("/VipII/Print_Mode", 1l);
    conf[VIPII].computerVersion_ = (int)configPointer->Read("/VipII/ComputerVersion", 0l);
    setComputerVersion();

    setRealCas(VIPII);

    wxString defaultClock;
    defaultClock.Printf("%1.4f", 1.789773);
    conf[VIPII].clock_ = convertLocale(configPointer->Read("/VipII/Clock_Speed", defaultClock));
    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[VIPII].ledTime_ = configPointer->Read("/VipII/Led_Update_Frequency", defaultTimer);

    conf[VIPII].ramType_ = (int)configPointer->Read("/VipII/Ram_Type", 2l);

    configPointer->Read("/VipII/Enable_Auto_Boot", &conf[VIPII].autoBoot, true);
    conf[VIPII].autoBootType = (int)configPointer->Read("/VipII/AutoBootType", 0l);
    if (conf[VIPII].autoBoot)
        setAutoBootTypeVipII();
    else
        conf[VIPII].bootAddress_ = 3;

    long value;
    conf[VIPII].saveStartString_ = configPointer->Read("/VipII/SaveStart", "0");
    if (!conf[VIPII].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[VIPII].saveStart_ = value;

    if (mode_.gui)
    {
        XRCCTRL(*this, "MainRomVipII", wxComboBox)->SetValue(conf[VIPII].rom_[MAINROM1]);
        XRCCTRL(*this, "MainRom2VipII", wxComboBox)->SetValue(conf[VIPII].rom_[MAINROM2]);
        XRCCTRL(*this, "RamSWVipII", wxComboBox)->SetValue(conf[VIPII].ram_);
        XRCCTRL(*this, "Chip8SWVipII", wxTextCtrl)->SetValue(conf[VIPII].chip8SW_);
        XRCCTRL(*this, "ScreenDumpFileVipII", wxComboBox)->SetValue(conf[VIPII].screenDumpFile_);
        XRCCTRL(*this, "WavFileVipII", wxTextCtrl)->SetValue(conf[VIPII].wavFile_[0]);

        correctZoomAndValue(VIPII, "VipII", SET_SPIN, VIDEOMAIN);

        XRCCTRL(*this, "TurboVipII", wxCheckBox)->SetValue(conf[VIPII].turbo_);
        turboGui("VipII");
        XRCCTRL(*this, "TurboClockVipII", wxTextCtrl)->SetValue(conf[VIPII].turboClock_);
        XRCCTRL(*this, "AutoCasLoadVipII", wxCheckBox)->SetValue(conf[VIPII].autoCassetteLoad_);
        XRCCTRL(*this, "VolumeVipII", wxSlider)->SetValue(conf[VIPII].volume_);
        if (clockTextCtrl[VIPII] != NULL)
            clockTextCtrl[VIPII]->ChangeValue(conf[VIPII].clock_);
        XRCCTRL(*this, "ShowAddressVipII", wxTextCtrl)->ChangeValue(conf[VIPII].ledTime_);
        XRCCTRL(*this, "ComputerVersionVipII", wxChoice)->SetSelection(conf[VIPII].computerVersion_);

        XRCCTRL(*this, "RamVipII", wxChoice)->SetSelection(conf[VIPII].ramType_);
        XRCCTRL(*this, "AutoBootVipII", wxCheckBox)->SetValue(conf[VIPII].autoBoot);
        XRCCTRL(*this, "AutoBootTypeVipII", wxChoice)->SetSelection(conf[VIPII].autoBootType);
        XRCCTRL(*this, "UseLocationVipII", wxCheckBox)->SetValue(conf[VIPII].useLoadLocation_);
        if (conf[VIPII].saveStart_ != 0)
            XRCCTRL(*this, "SaveStartVipII", wxTextCtrl)->SetValue(conf[VIPII].saveStartString_);
    }
}

void GuiVipII::writeVipIIDirConfig()
{
    writeConfigDir("/Dir/VipII/Main", conf[VIPII].mainDir_);
    writeConfigDir("/Dir/VipII/Main_Rom_1_File", conf[VIPII].romDir_[MAINROM1]);
    writeConfigDir("/Dir/VipII/Main_Rom_2_File", conf[VIPII].romDir_[MAINROM2]);
    writeConfigDir("/Dir/VipII/Software_File", conf[VIPII].ramDir_);
    writeConfigDir("/Dir/VipII/Chip_8_Software", conf[VIPII].chip8SWDir_);
    writeConfigDir("/Dir/VipII/Video_Dump_File", conf[VIPII].screenDumpFileDir_);
    writeConfigDir("/Dir/VipII/Wav_File", conf[VIPII].wavFileDir_[0]);
}

void GuiVipII::writeVipIIConfig()
{
    configPointer->Write("/VipII/Main_Rom_1_File", conf[VIPII].rom_[MAINROM1]);
    configPointer->Write("/VipII/Main_Rom_2_File", conf[VIPII].rom_[MAINROM2]);
    configPointer->Write("/VipII/Ram_Software", conf[VIPII].ram_);
    configPointer->Write("/VipII/Chip_8_Software", conf[VIPII].chip8SW_);
    configPointer->Write("/VipII/Video_Dump_File", conf[VIPII].screenDumpFile_);
    configPointer->Write("/VipII/Wav_File", conf[VIPII].wavFile_[0]);

    configPointer->Write("/VipII/Zoom", conf[VIPII].zoom_[VIDEOMAIN]);

    configPointer->Write("/VipII/Enable_Turbo_Cassette", conf[VIPII].turbo_);
    configPointer->Write("/VipII/Turbo_Clock_Speed", conf[VIPII].turboClock_);
    configPointer->Write("/VipII/Enable_Auto_Cassette", conf[VIPII].autoCassetteLoad_);
    configPointer->Write("/VipII/Enable_Real_Cassette", conf[VIPII].realCassetteLoad_);
    configPointer->Write("/VipII/Printer", conf[VIPII].printerOn_);
    configPointer->Write("/VipII/Volume", conf[VIPII].volume_);

    configPointer->Write("/VipII/Print_Mode", conf[VIPII].printMode_);
    configPointer->Write("/VipII/ComputerVersion", conf[VIPII].computerVersion_);
    configPointer->Write("/VipII/Clock_Speed", conf[VIPII].clock_);
    configPointer->Write("/VipII/Led_Update_Frequency", conf[VIPII].ledTime_);
    configPointer->Write("/VipII/Ram_Type", conf[VIPII].ramType_);
    configPointer->Write("/VipII/Enable_Auto_Boot", conf[VIPII].autoBoot);
    configPointer->Write("/VipII/AutoBootType", conf[VIPII].autoBootType);
    configPointer->Write("/VipII/UseLoadLocation", conf[VIPII].useLoadLocation_);
    configPointer->Write("/VipII/SaveStart", conf[VIPII].saveStartString_);
}

void GuiVipII::readVipIIWindowConfig()
{
    conf[VIPII].mainX_ = (int)configPointer->Read("/VipII/VipX", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[VIPII].mainY_ = (int)configPointer->Read("/VipII/VipY", mainWindowY_);
}

void GuiVipII::writeVipIIWindowConfig()
{
    if (conf[VIPII].mainX_ > 0)
        configPointer->Write("/VipII/VipX", conf[VIPII].mainX_);
    if (conf[VIPII].mainY_ > 0)
        configPointer->Write("/VipII/VipY", conf[VIPII].mainY_);
}

void GuiVipII::onRamSWVipII(wxCommandEvent&event)
{
    onRamSW(event);
}

void GuiVipII::onRamSWTextII(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].ram_ = XRCCTRL(*this, "RamSWVipII", wxComboBox)->GetValue();
}

void GuiVipII::pixieBarSize(wxCommandEvent&WXUNUSED(event))
{
    if (runningComputer_ == VIPII)
        p_Vip2->pixieBarSize();
}

void GuiVipII::pixieBarSizeEvent()
{
    if (runningComputer_ != VIPII)
        return;

    wxCommandEvent event(STATUS_BAR_PIXIE, 810);
    event.SetEventObject(this);
    wxPostEvent(this, event);
}

void GuiVipII::onComputerVersion(wxCommandEvent&event)
{
    conf[VIPII].computerVersion_ =  event.GetSelection();
    setComputerVersion();
}

void GuiVipII::setComputerVersion()
{
    if (conf[VIPII].computerVersion_ == VIPII_RCA)
    {
        conf[VIPII].defus_ = 0x7a81;
        conf[VIPII].eop_ = 0x7a83;
        conf[VIPII].string_ = 0x7a92;
        conf[VIPII].arrayValue_ = 0x7a94;
        conf[VIPII].eod_ = 0x7a99;
        conf[VIPII].basicRamAddress_ = 0;
    }
    else
    {
        conf[VIPII].defus_ = 0x4081;
        conf[VIPII].eop_ = 0x4083;
        conf[VIPII].string_ = 0x4092;
        conf[VIPII].arrayValue_ = 0x4094;
        conf[VIPII].eod_ = 0x4099;
        conf[VIPII].basicRamAddress_ = 0x4200;
    }

    if (mode_.gui)
    {
        XRCCTRL(*this,"MainRom2VipII", wxComboBox)->Enable(conf[VIPII].computerVersion_ == VIPII_ED);
        XRCCTRL(*this,"RomButton2VipII", wxButton)->Enable(conf[VIPII].computerVersion_ == VIPII_ED);
        XRCCTRL(*this,"RamTextVipII", wxStaticText)->Enable(conf[VIPII].computerVersion_ == VIPII_RCA);
        XRCCTRL(*this,"RamVipII", wxChoice)->Enable(conf[VIPII].computerVersion_ == VIPII_RCA);
        XRCCTRL(*this,"AutoBootTypeVipII", wxChoice)->Enable(conf[VIPII].computerVersion_ == VIPII_RCA);
        XRCCTRL(*this,"AutoBootVipII", wxCheckBox)->Enable(conf[VIPII].computerVersion_ == VIPII_RCA);
    }
}

void GuiVipII::onAutoBootVipII(wxCommandEvent&event)
{
    conf[VIPII].autoBoot = event.IsChecked();
    if (conf[VIPII].autoBoot)
        setAutoBootTypeVipII();
    else
        conf[VIPII].bootAddress_ = 3;
}

void GuiVipII::onAutoBootTypeVipII(wxCommandEvent&event)
{
    conf[VIPII].autoBootType = event.GetSelection();
    setAutoBootTypeVipII();
}

void GuiVipII::setAutoBootTypeVipII()
{
    switch (conf[VIPII].autoBootType)
    {
        case 0:
            conf[VIPII].bootAddress_ = 1;
        break;
        case 1:
            conf[VIPII].bootAddress_ = 2;
        break;
        case 2:
            conf[VIPII].bootAddress_ = 0xc;
        break;
    }
}


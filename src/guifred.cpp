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
#include "guifred.h"
#include "pixie.h"

BEGIN_EVENT_TABLE(GuiFred, GuiVip)

    EVT_TEXT(XRCID("RamSWFRED"), GuiMain::onRamSWText)
    EVT_COMBOBOX(XRCID("RamSWFRED"), GuiMain::onRamSWText)
    EVT_BUTTON(XRCID("RamSWButtonFRED"), GuiMain::onRamSW)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonFRED"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileFRED"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileFRED"), GuiMain::onScreenDumpFileText)

    EVT_BUTTON(XRCID("CasButtonFRED"), GuiMain::onCassette)
    EVT_BUTTON(XRCID("EjectCasFRED"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileFRED"), GuiMain::onCassetteText)

    EVT_SPIN_UP(XRCID("ZoomSpinFRED"), GuiMain::onZoomUp)
    EVT_SPIN_DOWN(XRCID("ZoomSpinFRED"), GuiMain::onZoomDown)
    EVT_TEXT(XRCID("ZoomValueFRED"), GuiMain::onZoomValue)
    EVT_BUTTON(XRCID("FullScreenF3FRED"), GuiMain::onFullScreen)
    EVT_BUTTON(XRCID("ScreenDumpF5FRED"), GuiMain::onScreenDump)
    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeFRED"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeFRED"), GuiMain::onVolume)
    EVT_BUTTON(XRCID("KeyMapFRED"), Main::onHexKeyDef)
    EVT_BUTTON(XRCID("ColoursFRED"), Main::onColoursDef)
    EVT_TEXT(XRCID("BeepFrequencyFRED"), GuiMain::onBeepFrequency)

    EVT_BUTTON(XRCID("SaveButtonFRED"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonFRED"), GuiMain::onLoadButton)
    EVT_TEXT(XRCID("SaveStartFRED"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndFRED"), GuiMain::onSaveEnd)

    EVT_CHECKBOX(XRCID("TurboFRED"), GuiMain::onTurbo)
    EVT_TEXT(XRCID("TurboClockFRED"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadFRED"), GuiMain::onAutoLoad)
    EVT_BUTTON(XRCID("CasLoadFRED"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasSaveFRED"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasStopFRED"), GuiMain::onCassetteStop)
    EVT_BUTTON(XRCID("RealCasLoadFRED"), GuiMain::onRealCas)
END_EVENT_TABLE()

GuiFred::GuiFred(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiVip(title, pos, size, mode, dataDir, iniDir)
{
}

void GuiFred::readFredConfig()
{
    selectedComputer_ = FRED;
    
    conf[FRED].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "FRED" + pathSeparator_;
    conf[FRED].mainDir_ = readConfigDir("/Dir/FRED/Main", dataDir_ + "FRED" + pathSeparator_);
    
    conf[FRED].ramDir_ = readConfigDir("/Dir/FRED/Main_Ram_File", dataDir_ + "FRED"  + pathSeparator_);
    conf[FRED].screenDumpFileDir_ = readConfigDir("/Dir/FRED/Video_Dump_File", dataDir_ + "FRED" + pathSeparator_);
    conf[FRED].wavFileDir_ = readConfigDir("/Dir/FRED/Wav_File", dataDir_ + "FRED" + pathSeparator_);

    conf[FRED].ram_ = configPointer->Read("/FRED/Main_Ram_File", "fel.bin");
    conf[FRED].screenDumpFile_ = configPointer->Read("/FRED/Video_Dump_File", "screendump.png");
    conf[FRED].wavFile_ = configPointer->Read("/FRED/Wav_File", "");

    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 2.0);
    conf[FRED].zoom_ = configPointer->Read("/FRED/Zoom", defaultZoom);
    wxString defaultClock;
    defaultClock.Printf("%1.2f", 1.0);
    conf[FRED].clock_ = configPointer->Read("/FRED/Clock_Speed", defaultClock);
    conf[FRED].beepFrequency_ = 640;
    conf[FRED].turboClock_ = configPointer->Read("/FRED/Turbo_Clock_Speed", "15");
    configPointer->Read("/FRED/Enable_Turbo_Cassette", &conf[FRED].turbo_, true);
    configPointer->Read("/FRED/Enable_Auto_Cassette", &conf[FRED].autoCassetteLoad_, true);
    configPointer->Read("/FRED/Enable_Real_Cassette", &conf[FRED].realCassetteLoad_, false);
    conf[FRED].volume_ = (int)configPointer->Read("/FRED/Volume", 25l);
    
    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[FRED].xScale_ = configPointer->Read("/FRED/Window_Scale_Factor_X", defaultScale);
    conf[FRED].realCassetteLoad_ = false;
    
    if (mode_.gui)
    {
        XRCCTRL(*this, "RamSWFRED", wxComboBox)->SetValue(conf[FRED].ram_);
        XRCCTRL(*this, "ScreenDumpFileFRED", wxComboBox)->SetValue(conf[FRED].screenDumpFile_);
        XRCCTRL(*this, "ZoomValueFRED", wxTextCtrl)->ChangeValue(conf[FRED].zoom_);
        clockTextCtrl[FRED]->ChangeValue(conf[FRED].clock_);
        XRCCTRL(*this, "TurboClockFRED", wxTextCtrl)->SetValue(conf[FRED].turboClock_);
        XRCCTRL(*this, "TurboFRED", wxCheckBox)->SetValue(conf[FRED].turbo_);
        turboGui("FRED");
        XRCCTRL(*this, "AutoCasLoadFRED", wxCheckBox)->SetValue(conf[FRED].autoCassetteLoad_);
        
        XRCCTRL(*this, "VolumeFRED", wxSlider)->SetValue(conf[FRED].volume_);
        XRCCTRL(*this, "WavFileFRED", wxTextCtrl)->SetValue(conf[FRED].wavFile_);
    }
}

void GuiFred::writeFredDirConfig()
{
    writeConfigDir("/Dir/FRED/Main", conf[FRED].mainDir_);
    writeConfigDir("/Dir/FRED/Main_Ram_File", conf[FRED].ramDir_);
    writeConfigDir("/Dir/FRED/Video_Dump_File", conf[FRED].screenDumpFileDir_);
    writeConfigDir("/Dir/FRED/Wav_File", conf[FRED].wavFileDir_);
}

void GuiFred::writeFredConfig()
{
    configPointer->Write("/FRED/Main_Ram_File", conf[FRED].ram_);
    configPointer->Write("/FRED/Video_Dump_File", conf[FRED].screenDumpFile_);
    configPointer->Write("/FRED/Wav_File", conf[FRED].wavFile_);

    configPointer->Write("/FRED/Zoom", conf[FRED].zoom_);
    configPointer->Write("/FRED/Clock_Speed", conf[FRED].clock_);
    configPointer->Write("/FRED/Turbo_Clock_Speed", conf[FRED].turboClock_);
    configPointer->Write("/FRED/Enable_Turbo_Cassette", conf[FRED].turbo_);
    configPointer->Write("/FRED/Enable_Auto_Cassette", conf[FRED].autoCassetteLoad_);
    configPointer->Write("/FRED/Enable_Real_Cassette", conf[FRED].realCassetteLoad_);
    configPointer->Write("/FRED/Volume", conf[FRED].volume_);
}

void GuiFred::readFredWindowConfig()
{
    conf[FRED].mainX_ = (int)configPointer->Read("/FRED/Window_Position_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[FRED].mainY_ = (int)configPointer->Read("/FRED/Window_Position_Y", mainWindowY_);
}

void GuiFred::writeFredWindowConfig()
{
    if (conf[FRED].mainX_ > 0)
        configPointer->Write("/FRED/Window_Position_X", conf[FRED].mainX_);
    if (conf[FRED].mainY_ > 0)
        configPointer->Write("/FRED/Window_Position_Y", conf[FRED].mainY_);
}

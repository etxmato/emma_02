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

    EVT_TEXT(XRCID("RamSWFRED2"), GuiMain::onRamSWText)
    EVT_COMBOBOX(XRCID("RamSWFRED2"), GuiMain::onRamSWText)
    EVT_BUTTON(XRCID("RamSWButtonFRED2"), GuiMain::onRamSW)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonFRED2"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileFRED2"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileFRED2"), GuiMain::onScreenDumpFileText)

    EVT_BUTTON(XRCID("CasButtonFRED2"), GuiMain::onCassette)
    EVT_BUTTON(XRCID("EjectCasFRED2"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileFRED2"), GuiMain::onCassetteText)

    EVT_SPIN_UP(XRCID("ZoomSpinFRED2"), GuiMain::onZoomUp)
    EVT_SPIN_DOWN(XRCID("ZoomSpinFRED2"), GuiMain::onZoomDown)
    EVT_TEXT(XRCID("ZoomValueFRED2"), GuiMain::onZoomValue)
    EVT_BUTTON(XRCID("FullScreenF3FRED2"), GuiMain::onFullScreen)
    EVT_BUTTON(XRCID("ScreenDumpF5FRED2"), GuiMain::onScreenDump)
    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeFRED2"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeFRED2"), GuiMain::onVolume)
    EVT_BUTTON(XRCID("KeyMapFRED2"), Main::onHexKeyDef)
    EVT_BUTTON(XRCID("ColoursFRED2"), Main::onColoursDef)
    EVT_TEXT(XRCID("BeepFrequencyFRED2"), GuiMain::onBeepFrequency)
    EVT_CHOICE(XRCID("RamFRED2"), GuiMain::onChoiceRam)
    EVT_CHOICE(XRCID("CpuFRED2"), GuiMain::onChoiceCpu)

    EVT_BUTTON(XRCID("SaveButtonFRED2"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonFRED2"), GuiMain::onLoadButton)
    EVT_TEXT(XRCID("SaveStartFRED2"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndFRED2"), GuiMain::onSaveEnd)
    EVT_CHECKBOX(XRCID("ControlWindowsFRED2"), GuiFred::onFred2ControlWindows)
    EVT_CHECKBOX(XRCID("AutoBootFRED2"), GuiFred::onAutoBoot)
    EVT_CHECKBOX(XRCID("stopToneFRED2"), GuiFred::onStopTone)
    EVT_CHECKBOX(XRCID("TapeStartFRED2"), GuiFred::onTapeStart)
    EVT_CHECKBOX(XRCID("InterlaceFRED2"), GuiMain::onInterlace)

    EVT_CHECKBOX(XRCID("TurboFRED2"), GuiMain::onTurbo)
    EVT_TEXT(XRCID("TurboClockFRED2"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadFRED2"), GuiMain::onAutoLoad)
    EVT_BUTTON(XRCID("CasLoadFRED2"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasSaveFRED2"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasStopFRED2"), GuiMain::onCassetteStop)
    EVT_BUTTON(XRCID("CasPauseFRED2"), GuiMain::onCassettePause)
    EVT_BUTTON(XRCID("RealCasLoadFRED2"), GuiMain::onRealCas)

    EVT_TEXT(XRCID("ShowAddressFRED2"), GuiMain::onLedTimer)
    EVT_TEXT(XRCID("BootAddressFRED2"), GuiElf::onBootAddress)

END_EVENT_TABLE()

GuiFred::GuiFred(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiVip(title, pos, size, mode, dataDir, iniDir)
{
}

void GuiFred::readFred1Config()
{
    selectedComputer_ = FRED1;
    
    conf[FRED1].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "FRED1" + pathSeparator_;
    conf[FRED1].mainDir_ = readConfigDir("/Dir/FRED1/Main", dataDir_ + "FRED1" + pathSeparator_);
    
    conf[FRED1].ramDir_ = readConfigDir("/Dir/FRED1/Main_Ram_File", dataDir_ + "FRED1"  + pathSeparator_);
    conf[FRED1].screenDumpFileDir_ = readConfigDir("/Dir/FRED1/Video_Dump_File", dataDir_ + "FRED1" + pathSeparator_);
    conf[FRED1].wavFileDir_ = readConfigDir("/Dir/FRED1/Wav_File", dataDir_ + "FRED1" + pathSeparator_);

    conf[FRED1].ram_ = configPointer->Read("/FRED1/Main_Ram_File", "fel.bin");
    conf[FRED1].screenDumpFile_ = configPointer->Read("/FRED1/Video_Dump_File", "screendump.png");
    conf[FRED1].wavFile_ = configPointer->Read("/FRED1/Wav_File", "");

    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 2.0);
    conf[FRED1].zoom_ = configPointer->Read("/FRED1/Zoom", defaultZoom);
    wxString defaultClock;
    defaultClock.Printf("%1.2f", 1.0);
    conf[FRED1].clock_ = configPointer->Read("/FRED1/Clock_Speed", defaultClock);
    
    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[FRED1].ledTime_ = configPointer->Read("/FRED1/Led_Update_Frequency", defaultTimer);

    conf[FRED1].beepFrequency_ = 640;
    conf[FRED1].turboClock_ = configPointer->Read("/FRED1/Turbo_Clock_Speed", "15");
    configPointer->Read("/FRED1/Enable_Turbo_Cassette", &conf[FRED1].turbo_, true);
    configPointer->Read("/FRED1/Enable_Auto_Cassette", &conf[FRED1].autoCassetteLoad_, true);
    configPointer->Read("/FRED1/Enable_Real_Cassette", &conf[FRED1].realCassetteLoad_, false);
    conf[FRED1].volume_ = (int)configPointer->Read("/FRED1/Volume", 25l);
    conf[FRED1].ramType_ = (int)configPointer->Read("/FRED1/Ram_Type", 0l);
    conf[FRED1].overrideCpuType_ = (int)configPointer->Read("/FRED1/Override_Cpu_Type", 1l);
    configPointer->Read("/FRED1/Open_Control_Windows", &elfConfiguration[FRED1].useElfControlWindows, true);
    configPointer->Read("/FRED1/Enable_Interlace", &conf[FRED1].interlace_, true);

    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[FRED1].xScale_ = configPointer->Read("/FRED1/Window_Scale_Factor_X", defaultScale);
//    conf[FRED1].realCassetteLoad_ = false;
    
    configPointer->Read("/FRED1/Enable_Auto_Boot", &elfConfiguration[FRED1].autoBoot, true);
    configPointer->Read("/FRED1/Enable_Stop_Tone", &elfConfiguration[FRED1].stopTone, true);
    configPointer->Read("/FRED1/Enable_Tape_Start", &elfConfiguration[FRED1].tapeStart, true);

    long value;
    wxString bootAddress = configPointer->Read("/FRED1/Boot_Address", "0001");
    if (!bootAddress.ToLong(&value, 16))
        value = 0x1;
    conf[FRED1].bootAddress_ = value;

    if (mode_.gui)
    {
        XRCCTRL(*this, "RamSWFRED1", wxComboBox)->SetValue(conf[FRED1].ram_);
        XRCCTRL(*this, "ScreenDumpFileFRED1", wxComboBox)->SetValue(conf[FRED1].screenDumpFile_);
        XRCCTRL(*this, "ZoomValueFRED1", wxTextCtrl)->ChangeValue(conf[FRED1].zoom_);
        clockTextCtrl[FRED1]->ChangeValue(conf[FRED1].clock_);
        XRCCTRL(*this, "TurboClockFRED1", wxTextCtrl)->SetValue(conf[FRED1].turboClock_);
        XRCCTRL(*this, "TurboFRED1", wxCheckBox)->SetValue(conf[FRED1].turbo_);
        turboGui("FRED1");
        XRCCTRL(*this, "AutoCasLoadFRED1", wxCheckBox)->SetValue(conf[FRED1].autoCassetteLoad_);
        
        XRCCTRL(*this, "VolumeFRED1", wxSlider)->SetValue(conf[FRED1].volume_);
        XRCCTRL(*this, "WavFileFRED1", wxTextCtrl)->SetValue(conf[FRED1].wavFile_);
        XRCCTRL(*this, "RamFRED1", wxChoice)->SetSelection(conf[FRED1].ramType_);
        XRCCTRL(*this, "CpuFRED1", wxChoice)->SetSelection(conf[FRED1].overrideCpuType_);
        XRCCTRL(*this,"AddressText1FRED1", wxStaticText)->Enable(elfConfiguration[FRED1].useElfControlWindows);
        XRCCTRL(*this,"AddressText2FRED1", wxStaticText)->Enable(elfConfiguration[FRED1].useElfControlWindows);
        XRCCTRL(*this, "ControlWindowsFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].useElfControlWindows);
        XRCCTRL(*this, "ControlWindowsFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].useElfControlWindows);
        XRCCTRL(*this, "ShowAddressFRED1", wxTextCtrl)->ChangeValue(conf[FRED1].ledTime_);
        XRCCTRL(*this,"ShowAddressFRED1", wxTextCtrl)->Enable(elfConfiguration[FRED1].useElfControlWindows);
        XRCCTRL(*this, "AutoBootFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].autoBoot);
        XRCCTRL(*this, "stopToneFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].stopTone);
        XRCCTRL(*this, "TapeStartFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].tapeStart);
        XRCCTRL(*this, "InterlaceFRED1", wxCheckBox)->SetValue(conf[FRED1].interlace_);
        XRCCTRL(*this, "BootAddressFRED1", wxTextCtrl)->SetValue(bootAddress);
    }
}

void GuiFred::writeFred1DirConfig()
{
    writeConfigDir("/Dir/FRED1/Main", conf[FRED1].mainDir_);
    writeConfigDir("/Dir/FRED1/Main_Ram_File", conf[FRED1].ramDir_);
    writeConfigDir("/Dir/FRED1/Video_Dump_File", conf[FRED1].screenDumpFileDir_);
    writeConfigDir("/Dir/FRED1/Wav_File", conf[FRED1].wavFileDir_);
}

void GuiFred::writeFred1Config()
{
    wxString buffer;

    configPointer->Write("/FRED1/Main_Ram_File", conf[FRED1].ram_);
    configPointer->Write("/FRED1/Video_Dump_File", conf[FRED1].screenDumpFile_);
    configPointer->Write("/FRED1/Wav_File", conf[FRED1].wavFile_);

    configPointer->Write("/FRED1/Zoom", conf[FRED1].zoom_);
    configPointer->Write("/FRED1/Clock_Speed", conf[FRED1].clock_);
    configPointer->Write("/FRED1/Turbo_Clock_Speed", conf[FRED1].turboClock_);
    configPointer->Write("/FRED1/Enable_Turbo_Cassette", conf[FRED1].turbo_);
    configPointer->Write("/FRED1/Enable_Auto_Cassette", conf[FRED1].autoCassetteLoad_);
    configPointer->Write("/FRED1/Enable_Real_Cassette", conf[FRED1].realCassetteLoad_);
    configPointer->Write("/FRED1/Volume", conf[FRED1].volume_);
    configPointer->Write("/FRED1/Ram_Type", conf[FRED1].ramType_);
    configPointer->Write("/FRED1/Override_Cpu_Type", conf[FRED1].overrideCpuType_);
    configPointer->Write("/FRED1/Open_Control_Windows", elfConfiguration[FRED1].useElfControlWindows);
    configPointer->Write("/FRED1/Led_Update_Frequency", conf[FRED1].ledTime_);
    configPointer->Write("/FRED1/Enable_Auto_Boot", elfConfiguration[FRED1].autoBoot);
    configPointer->Write("/FRED1/Enable_Stop_Tone", elfConfiguration[FRED1].stopTone);
    configPointer->Write("/FRED1/Enable_Tape_Start", elfConfiguration[FRED1].tapeStart);
    configPointer->Write("/FRED1/Enable_Interlace", conf[FRED1].interlace_);
    buffer.Printf("%04X", (unsigned int)conf[FRED1].bootAddress_);
    configPointer->Write("/FRED1/Boot_Address", buffer);
}

void GuiFred::readFred1WindowConfig()
{
    conf[FRED1].pixieX_ = (int)configPointer->Read("/FRED1/Window_Position_Pixie_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[FRED1].pixieY_ = (int)configPointer->Read("/FRED1/Window_Position_Pixie_Y", mainWindowY_);
    conf[FRED1].mainX_ = (int)configPointer->Read("/FRED1/Window_Position_X",  mainWindowX_);
    conf[FRED1].mainY_ = (int)configPointer->Read("/FRED1/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiFred::writeFred1WindowConfig()
{
    if (conf[FRED1].pixieX_ > 0)
        configPointer->Write("/FRED1/Window_Position_Pixie_X", conf[FRED1].pixieX_);
    if (conf[FRED1].pixieY_ > 0)
        configPointer->Write("/FRED1/Window_Position_Pixie_Y", conf[FRED1].pixieY_);
    if (conf[FRED1].mainX_ > 0)
        configPointer->Write("/FRED1/Window_Position_X", conf[FRED1].mainX_);
    if (conf[FRED1].mainY_ > 0)
        configPointer->Write("/FRED1/Window_Position_Y", conf[FRED1].mainY_);
}

void GuiFred::onFred1ControlWindows(wxCommandEvent&event)
{
    elfConfiguration[FRED1].useElfControlWindows = event.IsChecked();
    
    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddressFRED1",wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText1FRED1",wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText2FRED1",wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
    }
    
    if (runningComputer_ == FRED1)
        p_Fred->Show(elfConfiguration[FRED1].useElfControlWindows);
}

void GuiFred::readFred2Config()
{
    selectedComputer_ = FRED2;
    
    conf[FRED2].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "FRED2" + pathSeparator_;
    conf[FRED2].mainDir_ = readConfigDir("/Dir/FRED2/Main", dataDir_ + "FRED2" + pathSeparator_);
    
    conf[FRED2].ramDir_ = readConfigDir("/Dir/FRED2/Main_Ram_File", dataDir_ + "FRED2"  + pathSeparator_);
    conf[FRED2].screenDumpFileDir_ = readConfigDir("/Dir/FRED2/Video_Dump_File", dataDir_ + "FRED2" + pathSeparator_);
    conf[FRED2].wavFileDir_ = readConfigDir("/Dir/FRED2/Wav_File", dataDir_ + "FRED2" + pathSeparator_);
    
    conf[FRED2].ram_ = configPointer->Read("/FRED2/Main_Ram_File", "fel.bin");
    conf[FRED2].screenDumpFile_ = configPointer->Read("/FRED2/Video_Dump_File", "screendump.png");
    conf[FRED2].wavFile_ = configPointer->Read("/FRED2/Wav_File", "");
    
    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 2.0);
    conf[FRED2].zoom_ = configPointer->Read("/FRED2/Zoom", defaultZoom);
    wxString defaultClock;
    defaultClock.Printf("%1.2f", 1.0);
    conf[FRED2].clock_ = configPointer->Read("/FRED2/Clock_Speed", defaultClock);
    
    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[FRED2].ledTime_ = configPointer->Read("/FRED2/Led_Update_Frequency", defaultTimer);
    
    conf[FRED2].beepFrequency_ = 640;
    conf[FRED2].turboClock_ = configPointer->Read("/FRED2/Turbo_Clock_Speed", "15");
    configPointer->Read("/FRED2/Enable_Turbo_Cassette", &conf[FRED2].turbo_, true);
    configPointer->Read("/FRED2/Enable_Auto_Cassette", &conf[FRED2].autoCassetteLoad_, true);
    configPointer->Read("/FRED2/Enable_Real_Cassette", &conf[FRED2].realCassetteLoad_, false);
    conf[FRED2].volume_ = (int)configPointer->Read("/FRED2/Volume", 25l);
    conf[FRED2].ramType_ = (int)configPointer->Read("/FRED2/Ram_Type", 0l);
    conf[FRED2].overrideCpuType_ = (int)configPointer->Read("/FRED2/Override_Cpu_Type", 1l);
    configPointer->Read("/FRED2/Open_Control_Windows", &elfConfiguration[FRED2].useElfControlWindows, true);
    configPointer->Read("/FRED2/Enable_Interlace", &conf[FRED2].interlace_, true);
    
    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[FRED2].xScale_ = configPointer->Read("/FRED2/Window_Scale_Factor_X", defaultScale);
    //    conf[FRED2].realCassetteLoad_ = false;
    
    configPointer->Read("/FRED2/Enable_Auto_Boot", &elfConfiguration[FRED2].autoBoot, true);
    configPointer->Read("/FRED2/Enable_Stop_Tone", &elfConfiguration[FRED2].stopTone, true);
    configPointer->Read("/FRED2/Enable_Tape_Start", &elfConfiguration[FRED2].tapeStart, true);
    
    long value;
    wxString bootAddress = configPointer->Read("/FRED2/Boot_Address", "0001");
    if (!bootAddress.ToLong(&value, 16))
        value = 0x1;
    conf[FRED2].bootAddress_ = value;
    
    if (mode_.gui)
    {
        XRCCTRL(*this, "RamSWFRED2", wxComboBox)->SetValue(conf[FRED2].ram_);
        XRCCTRL(*this, "ScreenDumpFileFRED2", wxComboBox)->SetValue(conf[FRED2].screenDumpFile_);
        XRCCTRL(*this, "ZoomValueFRED2", wxTextCtrl)->ChangeValue(conf[FRED2].zoom_);
        clockTextCtrl[FRED2]->ChangeValue(conf[FRED2].clock_);
        XRCCTRL(*this, "TurboClockFRED2", wxTextCtrl)->SetValue(conf[FRED2].turboClock_);
        XRCCTRL(*this, "TurboFRED2", wxCheckBox)->SetValue(conf[FRED2].turbo_);
        turboGui("FRED2");
        XRCCTRL(*this, "AutoCasLoadFRED2", wxCheckBox)->SetValue(conf[FRED2].autoCassetteLoad_);
        
        XRCCTRL(*this, "VolumeFRED2", wxSlider)->SetValue(conf[FRED2].volume_);
        XRCCTRL(*this, "WavFileFRED2", wxTextCtrl)->SetValue(conf[FRED2].wavFile_);
        XRCCTRL(*this, "RamFRED2", wxChoice)->SetSelection(conf[FRED2].ramType_);
        XRCCTRL(*this, "CpuFRED2", wxChoice)->SetSelection(conf[FRED2].overrideCpuType_);
        XRCCTRL(*this,"AddressText1FRED2", wxStaticText)->Enable(elfConfiguration[FRED2].useElfControlWindows);
        XRCCTRL(*this,"AddressText2FRED2", wxStaticText)->Enable(elfConfiguration[FRED2].useElfControlWindows);
        XRCCTRL(*this, "ControlWindowsFRED2", wxCheckBox)->SetValue(elfConfiguration[FRED2].useElfControlWindows);
        XRCCTRL(*this, "ControlWindowsFRED2", wxCheckBox)->SetValue(elfConfiguration[FRED2].useElfControlWindows);
        XRCCTRL(*this, "ShowAddressFRED2", wxTextCtrl)->ChangeValue(conf[FRED2].ledTime_);
        XRCCTRL(*this,"ShowAddressFRED2", wxTextCtrl)->Enable(elfConfiguration[FRED2].useElfControlWindows);
        XRCCTRL(*this, "AutoBootFRED2", wxCheckBox)->SetValue(elfConfiguration[FRED2].autoBoot);
        XRCCTRL(*this, "stopToneFRED2", wxCheckBox)->SetValue(elfConfiguration[FRED2].stopTone);
        XRCCTRL(*this, "TapeStartFRED2", wxCheckBox)->SetValue(elfConfiguration[FRED2].tapeStart);
        XRCCTRL(*this, "InterlaceFRED2", wxCheckBox)->SetValue(conf[FRED2].interlace_);
        XRCCTRL(*this, "BootAddressFRED2", wxTextCtrl)->SetValue(bootAddress);
    }
}

void GuiFred::writeFred2DirConfig()
{
    writeConfigDir("/Dir/FRED2/Main", conf[FRED2].mainDir_);
    writeConfigDir("/Dir/FRED2/Main_Ram_File", conf[FRED2].ramDir_);
    writeConfigDir("/Dir/FRED2/Video_Dump_File", conf[FRED2].screenDumpFileDir_);
    writeConfigDir("/Dir/FRED2/Wav_File", conf[FRED2].wavFileDir_);
}

void GuiFred::writeFred2Config()
{
    wxString buffer;
    
    configPointer->Write("/FRED2/Main_Ram_File", conf[FRED2].ram_);
    configPointer->Write("/FRED2/Video_Dump_File", conf[FRED2].screenDumpFile_);
    configPointer->Write("/FRED2/Wav_File", conf[FRED2].wavFile_);
    
    configPointer->Write("/FRED2/Zoom", conf[FRED2].zoom_);
    configPointer->Write("/FRED2/Clock_Speed", conf[FRED2].clock_);
    configPointer->Write("/FRED2/Turbo_Clock_Speed", conf[FRED2].turboClock_);
    configPointer->Write("/FRED2/Enable_Turbo_Cassette", conf[FRED2].turbo_);
    configPointer->Write("/FRED2/Enable_Auto_Cassette", conf[FRED2].autoCassetteLoad_);
    configPointer->Write("/FRED2/Enable_Real_Cassette", conf[FRED2].realCassetteLoad_);
    configPointer->Write("/FRED2/Volume", conf[FRED2].volume_);
    configPointer->Write("/FRED2/Ram_Type", conf[FRED2].ramType_);
    configPointer->Write("/FRED2/Override_Cpu_Type", conf[FRED2].overrideCpuType_);
    configPointer->Write("/FRED2/Open_Control_Windows", elfConfiguration[FRED2].useElfControlWindows);
    configPointer->Write("/FRED2/Led_Update_Frequency", conf[FRED2].ledTime_);
    configPointer->Write("/FRED2/Enable_Auto_Boot", elfConfiguration[FRED2].autoBoot);
    configPointer->Write("/FRED2/Enable_Stop_Tone", elfConfiguration[FRED2].stopTone);
    configPointer->Write("/FRED2/Enable_Tape_Start", elfConfiguration[FRED2].tapeStart);
    configPointer->Write("/FRED2/Enable_Interlace", conf[FRED2].interlace_);
    buffer.Printf("%04X", (unsigned int)conf[FRED2].bootAddress_);
    configPointer->Write("/FRED2/Boot_Address", buffer);
}

void GuiFred::readFred2WindowConfig()
{
    conf[FRED2].pixieX_ = (int)configPointer->Read("/FRED2/Window_Position_Pixie_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[FRED2].pixieY_ = (int)configPointer->Read("/FRED2/Window_Position_Pixie_Y", mainWindowY_);
    conf[FRED2].mainX_ = (int)configPointer->Read("/FRED2/Window_Position_X",  mainWindowX_);
    conf[FRED2].mainY_ = (int)configPointer->Read("/FRED2/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiFred::writeFred2WindowConfig()
{
    if (conf[FRED2].pixieX_ > 0)
        configPointer->Write("/FRED2/Window_Position_Pixie_X", conf[FRED2].pixieX_);
    if (conf[FRED2].pixieY_ > 0)
        configPointer->Write("/FRED2/Window_Position_Pixie_Y", conf[FRED2].pixieY_);
    if (conf[FRED2].mainX_ > 0)
        configPointer->Write("/FRED2/Window_Position_X", conf[FRED2].mainX_);
    if (conf[FRED2].mainY_ > 0)
        configPointer->Write("/FRED2/Window_Position_Y", conf[FRED2].mainY_);
}

void GuiFred::onFred2ControlWindows(wxCommandEvent&event)
{
    elfConfiguration[FRED2].useElfControlWindows = event.IsChecked();
    
    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddressFRED2",wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText1FRED2",wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText2FRED2",wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
    }
    
    if (runningComputer_ == FRED2)
        p_Fred->Show(elfConfiguration[FRED2].useElfControlWindows);
}

bool GuiFred::getUseControlWindows()
{
    return elfConfiguration[FRED2].useElfControlWindows;
}

void GuiFred::onAutoBoot(wxCommandEvent&event)
{
    elfConfiguration[FRED2].autoBoot = event.IsChecked();
}

void GuiFred::onStopTone(wxCommandEvent&event)
{
    elfConfiguration[FRED2].stopTone = event.IsChecked();
}

void GuiFred::onTapeStart(wxCommandEvent&event)
{
    elfConfiguration[FRED2].tapeStart = event.IsChecked();
}




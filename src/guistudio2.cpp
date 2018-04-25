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
#include "guistudio2.h"
#include "pixie.h"

BEGIN_EVENT_TABLE(GuiStudio2, GuiVip)

	EVT_TEXT(XRCID("MainRomStudio2"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomStudio2"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonStudio2"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("CartRomStudio2"), GuiMain::onCartRomText)
	EVT_COMBOBOX(XRCID("CartRomStudio2"), GuiMain::onCartRomText)
	EVT_BUTTON(XRCID("CartRomButtonStudio2"), GuiMain::onCartRom)

	EVT_BUTTON(XRCID("ScreenDumpFileButtonStudio2"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileStudio2"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileStudio2"), GuiMain::onScreenDumpFileText)

	EVT_SPIN_UP(XRCID("ZoomSpinStudio2"), GuiMain::onZoomUp)
	EVT_SPIN_DOWN(XRCID("ZoomSpinStudio2"), GuiMain::onZoomDown)
	EVT_TEXT(XRCID("ZoomValueStudio2"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3Studio2"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("ScreenDumpF5Studio2"), GuiMain::onScreenDump)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeStudio2"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeStudio2"), GuiMain::onVolume)
	EVT_BUTTON(XRCID("KeyMapStudio2"), Main::onHexKeyDef)
	EVT_BUTTON(XRCID("ColoursStudio2"), Main::onColoursDef)
	EVT_TEXT(XRCID("BeepFrequencyStudio2"), GuiMain::onBeepFrequency)

    EVT_CHECKBOX(XRCID("MultiCartStudio2"), GuiStudio2::onMultiCartStudio)
	EVT_CHECKBOX(XRCID("DisableSystemRomStudio2"), GuiStudio2::onDisableSystemRomStudio)
	EVT_SPINCTRL(XRCID("MsbStudio2"), GuiStudio2::onMsbStudio)
    EVT_SPINCTRL(XRCID("LsbStudio2"), GuiStudio2::onLsbStudio)


    EVT_TEXT(XRCID("MainRomCoinArcade"), GuiMain::onMainRom1Text)
    EVT_COMBOBOX(XRCID("MainRomCoinArcade"), GuiMain::onMainRom1Text)
    EVT_BUTTON(XRCID("RomButtonCoinArcade"), GuiMain::onMainRom1)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonCoinArcade"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileCoinArcade"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileCoinArcade"), GuiMain::onScreenDumpFileText)

    EVT_SPIN_UP(XRCID("ZoomSpinCoinArcade"), GuiMain::onZoomUp)
    EVT_SPIN_DOWN(XRCID("ZoomSpinCoinArcade"), GuiMain::onZoomDown)
    EVT_TEXT(XRCID("ZoomValueCoinArcade"), GuiMain::onZoomValue)
    EVT_BUTTON(XRCID("FullScreenF3CoinArcade"), GuiMain::onFullScreen)
    EVT_BUTTON(XRCID("ScreenDumpF5CoinArcade"), GuiMain::onScreenDump)
    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeCoinArcade"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeCoinArcade"), GuiMain::onVolume)
    EVT_BUTTON(XRCID("KeyMapCoinArcade"), Main::onHexKeyDef)
    EVT_BUTTON(XRCID("ColoursCoinArcade"), Main::onColoursDef)
    EVT_TEXT(XRCID("BeepFrequencyCoinArcade"), GuiMain::onBeepFrequency)


	EVT_TEXT(XRCID("MainRomVisicom"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomVisicom"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonVisicom"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("CartRomVisicom"), GuiMain::onCartRomText)
	EVT_COMBOBOX(XRCID("CartRomVisicom"), GuiMain::onCartRomText)
	EVT_BUTTON(XRCID("CartRomButtonVisicom"), GuiMain::onCartRom)

	EVT_BUTTON(XRCID("ScreenDumpFileButtonVisicom"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileVisicom"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileVisicom"), GuiMain::onScreenDumpFileText)

	EVT_SPIN_UP(XRCID("ZoomSpinVisicom"), GuiMain::onZoomUp)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVisicom"), GuiMain::onZoomDown)
	EVT_TEXT(XRCID("ZoomValueVisicom"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3Visicom"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("ScreenDumpF5Visicom"), GuiMain::onScreenDump)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeVisicom"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeVisicom"), GuiMain::onVolume)
	EVT_BUTTON(XRCID("KeyMapVisicom"), Main::onHexKeyDef)
	EVT_BUTTON(XRCID("ColoursVisicom"), Main::onColoursDef)
	EVT_TEXT(XRCID("BeepFrequencyVisicom"), GuiMain::onBeepFrequency)

    EVT_CHECKBOX(XRCID("MultiCartVictory"), GuiStudio2::onMultiCartVictory)
    EVT_CHECKBOX(XRCID("DisableSystemRomVictory"), GuiStudio2::onDisableSystemRomVictory)
    EVT_SPINCTRL(XRCID("MsbVictory"), GuiStudio2::onMsbVictory)
    EVT_SPINCTRL(XRCID("LsbVictory"), GuiStudio2::onLsbVictory)
    EVT_CHOICE(XRCID("VidModeVictory"), GuiStudio2::onVictoryVideoMode)

	
	EVT_TEXT(XRCID("MainRomVictory"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomVictory"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonVictory"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("CartRomVictory"), GuiMain::onCartRomText)
	EVT_COMBOBOX(XRCID("CartRomVictory"), GuiMain::onCartRomText)
	EVT_BUTTON(XRCID("CartRomButtonVictory"), GuiMain::onCartRom)

	EVT_BUTTON(XRCID("ScreenDumpFileButtonVictory"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileVictory"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileVictory"), GuiMain::onScreenDumpFileText)

	EVT_SPIN_UP(XRCID("ZoomSpinVictory"), GuiMain::onZoomUp)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVictory"), GuiMain::onZoomDown)
	EVT_TEXT(XRCID("ZoomValueVictory"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3Victory"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("ScreenDumpF5Victory"), GuiMain::onScreenDump)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeVictory"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeVictory"), GuiMain::onVolume)
	EVT_BUTTON(XRCID("KeyMapVictory"), Main::onHexKeyDef)
	EVT_BUTTON(XRCID("ColoursVictory"), Main::onColoursDef)

	
    EVT_TEXT(XRCID("MainRomStudioIV"), GuiMain::onMainRom1Text)
    EVT_COMBOBOX(XRCID("MainRomStudioIV"), GuiMain::onMainRom1Text)
    EVT_BUTTON(XRCID("RomButtonStudioIV"), GuiMain::onMainRom1)

    EVT_TEXT(XRCID("CartRomStudioIV"), GuiMain::onCartRomText)
    EVT_COMBOBOX(XRCID("CartRomStudioIV"), GuiMain::onCartRomText)
    EVT_BUTTON(XRCID("CartRomButtonStudioIV"), GuiMain::onCartRom)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonStudioIV"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileStudioIV"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileStudioIV"), GuiMain::onScreenDumpFileText)

    EVT_SPIN_UP(XRCID("ZoomSpinStudioIV"), GuiMain::onZoomUp)
    EVT_SPIN_DOWN(XRCID("ZoomSpinStudioIV"), GuiMain::onZoomDown)
    EVT_TEXT(XRCID("ZoomValueStudioIV"), GuiMain::onZoomValue)
    EVT_BUTTON(XRCID("FullScreenF3StudioIV"), GuiMain::onFullScreen)
    EVT_BUTTON(XRCID("ScreenDumpF5StudioIV"), GuiMain::onScreenDump)
    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeStudioIV"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeStudioIV"), GuiMain::onVolume)
    EVT_BUTTON(XRCID("KeyMapStudioIV"), Main::onHexKeyDef)
    EVT_BUTTON(XRCID("ColoursStudioIV"), Main::onColoursDef)
    EVT_CHOICE(XRCID("VidModeStudioIV"), GuiStudio2::onStudioIVVideoMode)

END_EVENT_TABLE()

GuiStudio2::GuiStudio2(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiVip(title, pos, size, mode, dataDir, iniDir)
{
}

void GuiStudio2::readStudioConfig()
{
	selectedComputer_ = STUDIO;

    conf[STUDIO].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Studio2" + pathSeparator_;
    conf[STUDIO].mainDir_ = readConfigDir("/Dir/Studio2/Main", dataDir_ + "Studio2" + pathSeparator_);
    
    conf[STUDIO].romDir_[MAINROM1] = readConfigDir("/Dir/Studio2/Main_Rom_File", dataDir_ + "Studio2"  + pathSeparator_);
	conf[STUDIO].romDir_[CARTROM] = readConfigDir("/Dir/Studio2/St2_File", dataDir_ + "Studio2" + pathSeparator_);
	conf[STUDIO].screenDumpFileDir_ = readConfigDir("/Dir/Studio2/Video_Dump_File", dataDir_ + "Studio2" + pathSeparator_);

	conf[STUDIO].rom_[MAINROM1] = configPointer->Read("/Studio2/Main_Rom_File", "studio2.rom");
	conf[STUDIO].rom_[CARTROM] = configPointer->Read("/Studio2/St2_File", "");
	conf[STUDIO].screenDumpFile_ = configPointer->Read("/Studio2/Video_Dump_File", "screendump.png");

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[STUDIO].zoom_ = configPointer->Read("/Studio2/Zoom", defaultZoom);
	wxString defaultClock;
	defaultClock.Printf("%1.2f", 1.76);
	conf[STUDIO].clock_ = configPointer->Read("/Studio2/Clock_Speed", defaultClock);
 //   conf[STUDIO].beepFrequency_ = (int)configPointer->Read("/Studio2/Beep_Frequency", 480);
    conf[STUDIO].beepFrequency_ = 640;
	conf[STUDIO].volume_ = (int)configPointer->Read("/Studio2/Volume", 25l);

	wxString defaultScale;
	defaultScale.Printf("%i", 3);
	conf[STUDIO].xScale_ = configPointer->Read("/Studio2/Window_Scale_Factor_X", defaultScale);
	conf[STUDIO].realCassetteLoad_ = false;

    configPointer->Read("/Studio2/MultiCart", &conf[STUDIO].multiCart_, false);
    configPointer->Read("/Studio2/DisableSystemRom", &conf[STUDIO].disableSystemRom_, true);
    conf[STUDIO].lsb_ = (Byte)configPointer->Read("/Studio2/Lsb", 0l);
    conf[STUDIO].msb_ = (Byte)configPointer->Read("/Studio2/Msb", 0l);
    
    if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomStudio2", wxComboBox)->SetValue(conf[STUDIO].rom_[MAINROM1]);
		XRCCTRL(*this, "CartRomStudio2", wxComboBox)->SetValue(conf[STUDIO].rom_[CARTROM]);
		XRCCTRL(*this, "ScreenDumpFileStudio2", wxComboBox)->SetValue(conf[STUDIO].screenDumpFile_);
		XRCCTRL(*this, "ZoomValueStudio2", wxTextCtrl)->ChangeValue(conf[STUDIO].zoom_);
		clockTextCtrl[STUDIO]->ChangeValue(conf[STUDIO].clock_);
//		wxString beepFrequency;
//		beepFrequency.Printf("%d", conf[STUDIO].beepFrequency_);
//		XRCCTRL(*this, "BeepFrequencyStudio2", wxTextCtrl)->ChangeValue(beepFrequency);
		XRCCTRL(*this, "VolumeStudio2", wxSlider)->SetValue(conf[STUDIO].volume_);

        XRCCTRL(*this, "MultiCartStudio2", wxCheckBox)->SetValue(conf[STUDIO].multiCart_);
        XRCCTRL(*this, "DisableSystemRomStudio2", wxCheckBox)->SetValue(conf[STUDIO].disableSystemRom_);
        XRCCTRL(*this, "LsbStudio2", wxSpinCtrl)->SetValue(conf[STUDIO].lsb_);
        XRCCTRL(*this, "MsbStudio2", wxSpinCtrl)->SetValue(conf[STUDIO].msb_);
		XRCCTRL(*this, "DisableSystemRomStudio2", wxCheckBox)->Enable(conf[STUDIO].multiCart_);
		XRCCTRL(*this, "MainRomStudio2", wxComboBox)->Enable(!conf[STUDIO].disableSystemRom_ | !conf[STUDIO].multiCart_);
		XRCCTRL(*this, "RomButtonStudio2", wxButton)->Enable(!conf[STUDIO].disableSystemRom_ | !conf[STUDIO].multiCart_);
	}
}

void GuiStudio2::writeStudioDirConfig()
{
    writeConfigDir("/Dir/Studio2/Main", conf[STUDIO].mainDir_);
    writeConfigDir("/Dir/Studio2/Main_Rom_File", conf[STUDIO].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Studio2/St2_File", conf[STUDIO].romDir_[CARTROM]);
	writeConfigDir("/Dir/Studio2/Video_Dump_File", conf[STUDIO].screenDumpFileDir_);
}

void GuiStudio2::writeStudioConfig()
{
	configPointer->Write("/Studio2/Main_Rom_File", conf[STUDIO].rom_[MAINROM1]);
	configPointer->Write("/Studio2/St2_File", conf[STUDIO].rom_[CARTROM]);
	configPointer->Write("/Studio2/Video_Dump_File", conf[STUDIO].screenDumpFile_);

	configPointer->Write("/Studio2/Zoom", conf[STUDIO].zoom_);
	configPointer->Write("/Studio2/Clock_Speed", conf[STUDIO].clock_);
//	configPointer->Write("/Studio2/Beep_Frequency", conf[STUDIO].beepFrequency_);
    configPointer->Write("/Studio2/Volume", conf[STUDIO].volume_);
    configPointer->Write("/Studio2/MultiCart", conf[STUDIO].multiCart_);
    configPointer->Write("/Studio2/DisableSystemRom", conf[STUDIO].disableSystemRom_);
    configPointer->Write("/Studio2/Lsb", conf[STUDIO].lsb_);
    configPointer->Write("/Studio2/Msb", conf[STUDIO].msb_);
}

void GuiStudio2::readStudioWindowConfig()
{
    conf[STUDIO].mainX_ = (int)configPointer->Read("/Studio2/Window_Position_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[STUDIO].mainY_ = (int)configPointer->Read("/Studio2/Window_Position_Y", mainWindowY_);
}

void GuiStudio2::writeStudioWindowConfig()
{
    if (conf[STUDIO].mainX_ > 0)
        configPointer->Write("/Studio2/Window_Position_X", conf[STUDIO].mainX_);
    if (conf[STUDIO].mainY_ > 0)
        configPointer->Write("/Studio2/Window_Position_Y", conf[STUDIO].mainY_);
}

void GuiStudio2::onMultiCartStudio(wxCommandEvent&event)
{
    conf[STUDIO].multiCart_ = event.IsChecked();
    XRCCTRL(*this,"DisableSystemRomStudio2", wxCheckBox)->Enable(conf[STUDIO].multiCart_);
    XRCCTRL(*this,"MainRomStudio2", wxComboBox)->Enable(!conf[STUDIO].disableSystemRom_ | !conf[STUDIO].multiCart_);
    XRCCTRL(*this,"RomButtonStudio2", wxButton)->Enable(!conf[STUDIO].disableSystemRom_ | !conf[STUDIO].multiCart_);
}

void GuiStudio2::onDisableSystemRomStudio(wxCommandEvent&event)
{
	conf[STUDIO].disableSystemRom_ = event.IsChecked();
	if (runningComputer_ == STUDIO)
		p_Computer->setDisableSystemRom(conf[STUDIO].disableSystemRom_);
    else
    {
        XRCCTRL(*this,"MainRomStudio2", wxComboBox)->Enable(!conf[STUDIO].disableSystemRom_ | !conf[STUDIO].multiCart_);
        XRCCTRL(*this,"RomButtonStudio2", wxButton)->Enable(!conf[STUDIO].disableSystemRom_ | !conf[STUDIO].multiCart_);
    }
}

void GuiStudio2::onMsbStudio(wxSpinEvent&event)
{
    conf[STUDIO].msb_ = event.GetPosition();
    if (runningComputer_ == STUDIO)
        p_Computer->setMultiCartMsb(conf[STUDIO].msb_);
}

void GuiStudio2::onLsbStudio(wxSpinEvent&event)
{
    conf[STUDIO].lsb_ = event.GetPosition();
    if (runningComputer_ == STUDIO)
        p_Studio2->setMultiCartLsb(conf[STUDIO].lsb_);
}

void GuiStudio2::readCoinArcadeConfig()
{
    selectedComputer_ = COINARCADE;
    
    conf[COINARCADE].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "CoinArcade" + pathSeparator_;
    conf[COINARCADE].mainDir_ = readConfigDir("/Dir/CoinArcade/Main", dataDir_ + "CoinArcade" + pathSeparator_);
    
    conf[COINARCADE].romDir_[MAINROM1] = readConfigDir("/Dir/CoinArcade/Main_Rom_File", dataDir_ + "CoinArcade"  + pathSeparator_);
    conf[COINARCADE].romDir_[CARTROM] = readConfigDir("/Dir/CoinArcade/St2_File", dataDir_ + "CoinArcade" + pathSeparator_);
    conf[COINARCADE].screenDumpFileDir_ = readConfigDir("/Dir/CoinArcade/Video_Dump_File", dataDir_ + "CoinArcade" + pathSeparator_);
    
    conf[COINARCADE].rom_[MAINROM1] = configPointer->Read("/CoinArcade/Main_Rom_File", "Swords.arc");
    conf[COINARCADE].rom_[CARTROM] = configPointer->Read("/CoinArcade/St2_File", "");
    conf[COINARCADE].screenDumpFile_ = configPointer->Read("/CoinArcade/Video_Dump_File", "screendump.png");
    
    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 2.0);
    conf[COINARCADE].zoom_ = configPointer->Read("/CoinArcade/Zoom", defaultZoom);
    wxString defaultClock;
    defaultClock.Printf("%1.2f", 1.6);
    conf[COINARCADE].clock_ = configPointer->Read("/CoinArcade/Clock_Speed", defaultClock);
    conf[COINARCADE].beepFrequency_ = 640;
    conf[COINARCADE].volume_ = (int)configPointer->Read("/CoinArcade/Volume", 25l);
    
    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[COINARCADE].xScale_ = configPointer->Read("/CoinArcade/Window_Scale_Factor_X", defaultScale);
    conf[COINARCADE].realCassetteLoad_ = false;
    
    if (mode_.gui)
    {
        XRCCTRL(*this, "MainRomCoinArcade", wxComboBox)->SetValue(conf[COINARCADE].rom_[MAINROM1]);
        XRCCTRL(*this, "ScreenDumpFileCoinArcade", wxComboBox)->SetValue(conf[COINARCADE].screenDumpFile_);
        XRCCTRL(*this, "ZoomValueCoinArcade", wxTextCtrl)->ChangeValue(conf[COINARCADE].zoom_);
        clockTextCtrl[COINARCADE]->ChangeValue(conf[COINARCADE].clock_);
        XRCCTRL(*this, "VolumeCoinArcade", wxSlider)->SetValue(conf[COINARCADE].volume_);
        
        XRCCTRL(*this, "MainRomCoinArcade", wxComboBox)->Enable(!conf[COINARCADE].disableSystemRom_ | !conf[COINARCADE].multiCart_);
        XRCCTRL(*this, "RomButtonCoinArcade", wxButton)->Enable(!conf[COINARCADE].disableSystemRom_ | !conf[COINARCADE].multiCart_);
    }
}

void GuiStudio2::writeCoinArcadeDirConfig()
{
    writeConfigDir("/Dir/CoinArcade/Main", conf[COINARCADE].mainDir_);
    writeConfigDir("/Dir/CoinArcade/Main_Rom_File", conf[COINARCADE].romDir_[MAINROM1]);
    writeConfigDir("/Dir/CoinArcade/St2_File", conf[COINARCADE].romDir_[CARTROM]);
    writeConfigDir("/Dir/CoinArcade/Video_Dump_File", conf[COINARCADE].screenDumpFileDir_);
}

void GuiStudio2::writeCoinArcadeConfig()
{
    configPointer->Write("/CoinArcade/Main_Rom_File", conf[COINARCADE].rom_[MAINROM1]);
    configPointer->Write("/CoinArcade/St2_File", conf[COINARCADE].rom_[CARTROM]);
    configPointer->Write("/CoinArcade/Video_Dump_File", conf[COINARCADE].screenDumpFile_);
    
    configPointer->Write("/CoinArcade/Zoom", conf[COINARCADE].zoom_);
    configPointer->Write("/CoinArcade/Clock_Speed", conf[COINARCADE].clock_);
    configPointer->Write("/CoinArcade/Volume", conf[COINARCADE].volume_);
}

void GuiStudio2::readCoinArcadeWindowConfig()
{
    conf[COINARCADE].mainX_ = (int)configPointer->Read("/CoinArcade/Window_Position_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[COINARCADE].mainY_ = (int)configPointer->Read("/CoinArcade/Window_Position_Y", mainWindowY_);
}

void GuiStudio2::writeCoinArcadeWindowConfig()
{
    if (conf[COINARCADE].mainX_ > 0)
        configPointer->Write("/CoinArcade/Window_Position_X", conf[COINARCADE].mainX_);
    if (conf[COINARCADE].mainY_ > 0)
        configPointer->Write("/CoinArcade/Window_Position_Y", conf[COINARCADE].mainY_);
}

void GuiStudio2::readVisicomConfig()
{
	selectedComputer_ = VISICOM;

    conf[VISICOM].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Visicom" + pathSeparator_;
    conf[VISICOM].mainDir_ = readConfigDir("/Dir/Visicom/Main", dataDir_ + "Visicom" + pathSeparator_);
    
    conf[VISICOM].romDir_[MAINROM1] = readConfigDir("/Dir/Visicom/Main_Rom_File", dataDir_ + "Visicom"  + pathSeparator_);
	conf[VISICOM].romDir_[CARTROM] = readConfigDir("/Dir/Visicom/St2_File", dataDir_ + "Visicom" + pathSeparator_);
	conf[VISICOM].screenDumpFileDir_ = readConfigDir("/Dir/Visicom/Video_Dump_File", dataDir_ + "Visicom" + pathSeparator_);

	conf[VISICOM].rom_[MAINROM1] = configPointer->Read("/Visicom/Main_Rom_File", "visicom.rom");
	conf[VISICOM].rom_[CARTROM] = configPointer->Read("/Visicom/St2_File", "");
	conf[VISICOM].screenDumpFile_ = configPointer->Read("/Visicom/Video_Dump_File", "screendump.png");

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[VISICOM].zoom_ = configPointer->Read("/Visicom/Zoom", defaultZoom);
	wxString defaultClock;
	defaultClock.Printf("%1.2f", 1.76);
	conf[VISICOM].clock_ = configPointer->Read("/Visicom/Clock_Speed", defaultClock);
//	conf[VISICOM].beepFrequency_ = (int)configPointer->Read("/Visicom/Beep_Frequency", 480);
    conf[VISICOM].beepFrequency_ = 640;
	conf[VISICOM].volume_ = (int)configPointer->Read("/Visicom/Volume", 25l);

	wxString defaultScale;
	defaultScale.Printf("%i", 3);
	conf[VISICOM].xScale_ = configPointer->Read("/Visicom/Window_Scale_Factor_X", defaultScale);
	conf[VISICOM].realCassetteLoad_ = false;

	if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomVisicom", wxComboBox)->SetValue(conf[VISICOM].rom_[MAINROM1]);
		XRCCTRL(*this, "CartRomVisicom", wxComboBox)->SetValue(conf[VISICOM].rom_[CARTROM]);
		XRCCTRL(*this, "ScreenDumpFileVisicom", wxComboBox)->SetValue(conf[VISICOM].screenDumpFile_);
		XRCCTRL(*this, "ZoomValueVisicom", wxTextCtrl)->ChangeValue(conf[VISICOM].zoom_);
		clockTextCtrl[VISICOM]->ChangeValue(conf[VISICOM].clock_);
//		wxString beepFrequency;
//		beepFrequency.Printf("%d", conf[VISICOM].beepFrequency_);
//		XRCCTRL(*this, "BeepFrequencyVisicom", wxTextCtrl)->ChangeValue(beepFrequency);
		XRCCTRL(*this, "VolumeVisicom", wxSlider)->SetValue(conf[VISICOM].volume_);
	}
}

void GuiStudio2::writeVisicomDirConfig()
{
    writeConfigDir("/Dir/Visicom/Main", conf[VISICOM].mainDir_);
    writeConfigDir("/Dir/Visicom/Main_Rom_File", conf[VISICOM].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Visicom/St2_File", conf[VISICOM].romDir_[CARTROM]);
	writeConfigDir("/Dir/Visicom/Video_Dump_File", conf[VISICOM].screenDumpFileDir_);
}

void GuiStudio2::writeVisicomConfig()
{
	configPointer->Write("/Visicom/Main_Rom_File", conf[VISICOM].rom_[MAINROM1]);
	configPointer->Write("/Visicom/St2_File", conf[VISICOM].rom_[CARTROM]);
	configPointer->Write("/Visicom/Video_Dump_File", conf[VISICOM].screenDumpFile_);

	configPointer->Write("/Visicom/Zoom", conf[VISICOM].zoom_);
	configPointer->Write("/Visicom/Clock_Speed", conf[VISICOM].clock_);
//	configPointer->Write("/Visicom/Beep_Frequency", conf[VISICOM].beepFrequency_);
	configPointer->Write("/Visicom/Volume",conf[VISICOM].volume_);
}

void GuiStudio2::readVisicomWindowConfig()
{
    conf[VISICOM].mainX_ = (int)configPointer->Read("/Visicom/Window_Position_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[VISICOM].mainY_ = (int)configPointer->Read("/Visicom/Window_Position_Y", mainWindowY_);
}

void GuiStudio2::writeVisicomWindowConfig()
{
    if (conf[VISICOM].mainX_ > 0)
        configPointer->Write("/Visicom/Window_Position_X", conf[VISICOM].mainX_);
    if (conf[VISICOM].mainY_ > 0)
        configPointer->Write("/Visicom/Window_Position_Y", conf[VISICOM].mainY_);
}

void GuiStudio2::readVictoryConfig()
{
	selectedComputer_ = VICTORY;

    conf[VICTORY].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Victory" + pathSeparator_;
    conf[VICTORY].mainDir_ = readConfigDir("/Dir/Victory/Main", dataDir_ + "Victory" + pathSeparator_);
    
    conf[VICTORY].romDir_[MAINROM1] = readConfigDir("/Dir/Victory/Main_Rom_File", dataDir_ + "Victory"  + pathSeparator_);
	conf[VICTORY].romDir_[CARTROM] = readConfigDir("/Dir/Victory/St2_File", dataDir_ + "Victory" + pathSeparator_);
	conf[VICTORY].screenDumpFileDir_ = readConfigDir("/Dir/Victory/Video_Dump_File", dataDir_ + "Victory" + pathSeparator_);

	conf[VICTORY].rom_[MAINROM1] = configPointer->Read("/Victory/Main_Rom_File", "victory.rom");
	conf[VICTORY].rom_[CARTROM] = configPointer->Read("/Victory/St2_File", "");
	conf[VICTORY].screenDumpFile_ = configPointer->Read("/Victory/Video_Dump_File", "screendump.png");

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[VICTORY].zoom_ = configPointer->Read("/Victory/Zoom", defaultZoom);
	wxString defaultClock;
	defaultClock.Printf("%1.2f", 1.76);
	conf[VICTORY].clock_ = configPointer->Read("/Victory/Clock_Speed", defaultClock);
	conf[VICTORY].volume_ = (int)configPointer->Read("/Victory/Volume", 25l);

	wxString defaultScale;
	defaultScale.Printf("%i", 4);
	conf[VICTORY].xScale_ = configPointer->Read("/Victory/Window_Scale_Factor_X", defaultScale);
	conf[VICTORY].realCassetteLoad_ = false;

    configPointer->Read("/Victory/MultiCart", &conf[VICTORY].multiCart_, false);
    configPointer->Read("/Victory/DisableSystemRom", &conf[VICTORY].disableSystemRom_, true);
    conf[VICTORY].lsb_ = (Byte)configPointer->Read("/Victory/Lsb", 0l);
    conf[VICTORY].msb_ = (Byte)configPointer->Read("/Victory/Msb", 0l);
    conf[VICTORY].videoMode_ = (int)configPointer->Read("/Victory/Video_Mode", 0l);
    
    if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomVictory", wxComboBox)->SetValue(conf[VICTORY].rom_[MAINROM1]);
		XRCCTRL(*this, "CartRomVictory", wxComboBox)->SetValue(conf[VICTORY].rom_[CARTROM]);
		XRCCTRL(*this, "ScreenDumpFileVictory", wxComboBox)->SetValue(conf[VICTORY].screenDumpFile_);
		XRCCTRL(*this, "ZoomValueVictory", wxTextCtrl)->ChangeValue(conf[VICTORY].zoom_);
		clockTextCtrl[VICTORY]->ChangeValue(conf[VICTORY].clock_);
		XRCCTRL(*this, "VolumeVictory", wxSlider)->SetValue(conf[VICTORY].volume_);

        XRCCTRL(*this, "MultiCartVictory", wxCheckBox)->SetValue(conf[VICTORY].multiCart_);
        XRCCTRL(*this, "DisableSystemRomVictory", wxCheckBox)->SetValue(conf[VICTORY].disableSystemRom_);
        XRCCTRL(*this, "LsbVictory", wxSpinCtrl)->SetValue(conf[VICTORY].lsb_);
        XRCCTRL(*this, "MsbVictory", wxSpinCtrl)->SetValue(conf[VICTORY].msb_);
		XRCCTRL(*this, "DisableSystemRomVictory", wxCheckBox)->Enable(conf[VICTORY].multiCart_);
		XRCCTRL(*this, "MainRomVictory", wxComboBox)->Enable(!conf[VICTORY].disableSystemRom_ | !conf[VICTORY].multiCart_);
		XRCCTRL(*this, "RomButtonVictory", wxButton)->Enable(!conf[VICTORY].disableSystemRom_ | !conf[VICTORY].multiCart_);
        XRCCTRL(*this, "VidModeVictory", wxChoice)->SetSelection(conf[VICTORY].videoMode_);
	}
}

void GuiStudio2::writeVictoryDirConfig()
{
    writeConfigDir("/Dir/Victory/Main", conf[VICTORY].mainDir_);
    writeConfigDir("/Dir/Victory/Main_Rom_File", conf[VICTORY].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Victory/St2_File", conf[VICTORY].romDir_[CARTROM]);
	writeConfigDir("/Dir/Victory/Video_Dump_File", conf[VICTORY].screenDumpFileDir_);
}

void GuiStudio2::writeVictoryConfig()
{
	configPointer->Write("/Victory/Main_Rom_File",conf[VICTORY].rom_[MAINROM1]);
	configPointer->Write("/Victory/St2_File", conf[VICTORY].rom_[CARTROM]);
	configPointer->Write("/Victory/Video_Dump_File", conf[VICTORY].screenDumpFile_);

	configPointer->Write("/Victory/Zoom", conf[VICTORY].zoom_);
	configPointer->Write("/Victory/Clock_Speed", conf[VICTORY].clock_);
	configPointer->Write("/Victory/Volume", conf[VICTORY].volume_);

    configPointer->Write("/Victory/MultiCart", conf[VICTORY].multiCart_);
    configPointer->Write("/Victory/DisableSystemRom", conf[VICTORY].disableSystemRom_);
    configPointer->Write("/Victory/Lsb", conf[VICTORY].lsb_);
    configPointer->Write("/Victory/Msb", conf[VICTORY].msb_);
    configPointer->Write("/Victory/Video_Mode", conf[VICTORY].videoMode_);
}

void GuiStudio2::readVictoryWindowConfig()
{
    conf[VICTORY].mainX_ = (int)configPointer->Read("/Victory/Window_Position_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[VICTORY].mainY_ = (int)configPointer->Read("/Victory/Window_Position_Y", mainWindowY_);
}

void GuiStudio2::writeVictoryWindowConfig()
{
	if (conf[VICTORY].mainX_ > 0)
		configPointer->Write("/Victory/Window_Position_X", conf[VICTORY].mainX_);
	if (conf[VICTORY].mainY_ > 0)
		configPointer->Write("/Victory/Window_Position_Y", conf[VICTORY].mainY_);
}

void GuiStudio2::onMultiCartVictory(wxCommandEvent&event)
{
	conf[VICTORY].multiCart_ = event.IsChecked();
	XRCCTRL(*this, "DisableSystemRomVictory", wxCheckBox)->Enable(conf[VICTORY].multiCart_);
    XRCCTRL(*this, "MainRomVictory", wxComboBox)->Enable(!conf[VICTORY].disableSystemRom_ | !conf[VICTORY].multiCart_);
    XRCCTRL(*this, "RomButtonVictory", wxButton)->Enable(!conf[VICTORY].disableSystemRom_ | !conf[VICTORY].multiCart_);
}

void GuiStudio2::onDisableSystemRomVictory(wxCommandEvent&event)
{
	conf[VICTORY].disableSystemRom_ = event.IsChecked();
	if (runningComputer_ == VICTORY)
		p_Computer->setDisableSystemRom(conf[VICTORY].disableSystemRom_);
    else
    {
        XRCCTRL(*this, "MainRomVictory", wxComboBox)->Enable(!conf[VICTORY].disableSystemRom_ | !conf[VICTORY].multiCart_);
        XRCCTRL(*this, "RomButtonVictory", wxButton)->Enable(!conf[VICTORY].disableSystemRom_ | !conf[VICTORY].multiCart_);
    }
}

void GuiStudio2::onMsbVictory(wxSpinEvent&event)
{
	conf[VICTORY].msb_ = event.GetPosition();
	if (runningComputer_ == VICTORY)
		p_Computer->setMultiCartMsb(conf[VICTORY].msb_);
}

void GuiStudio2::onLsbVictory(wxSpinEvent&event)
{
	conf[VICTORY].lsb_ = event.GetPosition();
	if (runningComputer_ == VICTORY)
		p_Computer->setMultiCartLsb(conf[VICTORY].lsb_);
}

void GuiStudio2::onVictoryVideoMode(wxCommandEvent&event)
{
    conf[VICTORY].videoMode_ = event.GetSelection();
    if (mode_.gui)
    {
        if (conf[VICTORY].videoMode_ == PAL)
            conf[VICTORY].rom_[MAINROM1] = "victory.rom";
        else
            conf[VICTORY].rom_[MAINROM1] = "studio3.rom";
        XRCCTRL(*this, "MainRomVictory", wxComboBox)->SetValue(conf[VICTORY].rom_[MAINROM1]);
    }
}

void GuiStudio2::readStudioIVConfig()
{
    selectedComputer_ = STUDIOIV;
    
    conf[STUDIOIV].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "StudioIV" + pathSeparator_;
    conf[STUDIOIV].mainDir_ = readConfigDir("/Dir/StudioIV/Main", dataDir_ + "StudioIV" + pathSeparator_);
    
    conf[STUDIOIV].romDir_[MAINROM1] = readConfigDir("/Dir/StudioIV/Main_Rom_File", dataDir_ + "StudioIV"  + pathSeparator_);
    conf[STUDIOIV].romDir_[CARTROM] = readConfigDir("/Dir/StudioIV/St2_File", dataDir_ + "StudioIV" + pathSeparator_);
    conf[STUDIOIV].screenDumpFileDir_ = readConfigDir("/Dir/StudioIV/Video_Dump_File", dataDir_ + "StudioIV" + pathSeparator_);
    
    conf[STUDIOIV].rom_[MAINROM1] = configPointer->Read("/StudioIV/Main_Rom_File", "Studio IV V3 PAL.bin");
    conf[STUDIOIV].rom_[CARTROM] = configPointer->Read("/StudioIV/St2_File", "");
    conf[STUDIOIV].screenDumpFile_ = configPointer->Read("/StudioIV/Video_Dump_File", "screendump.png");
    
    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 2.0);
    conf[STUDIOIV].zoom_ = configPointer->Read("/StudioIV/Zoom", defaultZoom);
    wxString defaultClock;
    defaultClock.Printf("%1.2f", 3.58);
    conf[STUDIOIV].clock_ = configPointer->Read("/StudioIV/Clock_Speed", defaultClock);
    conf[STUDIOIV].volume_ = (int)configPointer->Read("/StudioIV/Volume", 25l);
    
    wxString defaultScale;
    defaultScale.Printf("%i", 4);
    conf[STUDIOIV].xScale_ = configPointer->Read("/StudioIV/Window_Scale_Factor_X", defaultScale);
    conf[STUDIOIV].realCassetteLoad_ = false;
    
    conf[STUDIOIV].videoMode_ = (int)configPointer->Read("/StudioIV/Video_Mode", 0l);

    if (mode_.gui)
    {
        XRCCTRL(*this, "MainRomStudioIV", wxComboBox)->SetValue(conf[STUDIOIV].rom_[MAINROM1]);
        XRCCTRL(*this, "CartRomStudioIV", wxComboBox)->SetValue(conf[STUDIOIV].rom_[CARTROM]);
        XRCCTRL(*this, "ScreenDumpFileStudioIV", wxComboBox)->SetValue(conf[STUDIOIV].screenDumpFile_);
        XRCCTRL(*this, "ZoomValueStudioIV", wxTextCtrl)->ChangeValue(conf[STUDIOIV].zoom_);
        clockTextCtrl[STUDIOIV]->ChangeValue(conf[STUDIOIV].clock_);
        XRCCTRL(*this, "VolumeStudioIV", wxSlider)->SetValue(conf[STUDIOIV].volume_);
        
        XRCCTRL(*this, "MainRomStudioIV", wxComboBox)->Enable(!conf[STUDIOIV].disableSystemRom_ | !conf[STUDIOIV].multiCart_);
        XRCCTRL(*this, "RomButtonStudioIV", wxButton)->Enable(!conf[STUDIOIV].disableSystemRom_ | !conf[STUDIOIV].multiCart_);
        XRCCTRL(*this, "VidModeStudioIV", wxChoice)->SetSelection(conf[STUDIOIV].videoMode_);
    }
}

void GuiStudio2::writeStudioIVDirConfig()
{
    writeConfigDir("/Dir/StudioIV/Main", conf[STUDIOIV].mainDir_);
    writeConfigDir("/Dir/StudioIV/Main_Rom_File", conf[STUDIOIV].romDir_[MAINROM1]);
    writeConfigDir("/Dir/StudioIV/St2_File", conf[STUDIOIV].romDir_[CARTROM]);
    writeConfigDir("/Dir/StudioIV/Video_Dump_File", conf[STUDIOIV].screenDumpFileDir_);
}

void GuiStudio2::writeStudioIVConfig()
{
    configPointer->Write("/StudioIV/Main_Rom_File",conf[STUDIOIV].rom_[MAINROM1]);
    configPointer->Write("/StudioIV/St2_File", conf[STUDIOIV].rom_[CARTROM]);
    configPointer->Write("/StudioIV/Video_Dump_File", conf[STUDIOIV].screenDumpFile_);
    
    configPointer->Write("/StudioIV/Zoom", conf[STUDIOIV].zoom_);
    configPointer->Write("/StudioIV/Clock_Speed", conf[STUDIOIV].clock_);
    configPointer->Write("/StudioIV/Volume", conf[STUDIOIV].volume_);
    
    configPointer->Write("/StudioIV/Video_Mode", conf[STUDIOIV].videoMode_);
}

void GuiStudio2::readStudioIVWindowConfig()
{
    conf[STUDIOIV].mainX_ = (int)configPointer->Read("/StudioIV/Window_Position_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[STUDIOIV].mainY_ = (int)configPointer->Read("/StudioIV/Window_Position_Y", mainWindowY_);
}

void GuiStudio2::writeStudioIVWindowConfig()
{
    if (conf[STUDIOIV].mainX_ > 0)
        configPointer->Write("/StudioIV/Window_Position_X", conf[STUDIOIV].mainX_);
    if (conf[STUDIOIV].mainY_ > 0)
        configPointer->Write("/StudioIV/Window_Position_Y", conf[STUDIOIV].mainY_);
}

void GuiStudio2::onStudioIVVideoMode(wxCommandEvent&event)
{
    conf[STUDIOIV].videoMode_ = event.GetSelection();
    if (mode_.gui)
    {
        if (conf[STUDIOIV].videoMode_ == PAL)
            conf[STUDIOIV].rom_[MAINROM1] = "Studio IV V3 PAL.bin";
        else
            conf[STUDIOIV].rom_[MAINROM1] = "Studio IV V3 NTSC.bin";
        XRCCTRL(*this, "MainRomStudioIV", wxComboBox)->SetValue(conf[STUDIOIV].rom_[MAINROM1]);
    }
}

int GuiStudio2::getStudioVideoMode(int computer)
{
    return conf[computer].videoMode_;
}

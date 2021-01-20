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
#include "guicidelsa.h"

int dracoCoin[] = 
{ 
	0x40, 0xe0, 0xc0, 0xa0, 0x80, 0x60, 0x20, 0
};

BEGIN_EVENT_TABLE(GuiCidelsa, GuiTelmac)

	EVT_TEXT(XRCID("MainRomCidelsa"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomCidelsa"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonCidelsa"), GuiMain::onMainRom1)
	EVT_BUTTON(XRCID("ScreenDumpFileButtonCidelsa"), GuiMain::onScreenDumpFile)
	EVT_TEXT(XRCID("ScreenDumpFileCidelsa"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileCidelsa"), GuiMain::onScreenDumpFileText)
	EVT_BUTTON(XRCID("ScreenDumpF5Cidelsa"), GuiMain::onScreenDump)
	EVT_SPIN_UP(XRCID("ZoomSpinCidelsa"), GuiMain::onZoom)
	EVT_SPIN_DOWN(XRCID("ZoomSpinCidelsa"), GuiMain::onZoom)
	EVT_TEXT(XRCID("ZoomValueCidelsa"), GuiMain::onZoomValue)
	EVT_BUTTON(XRCID("FullScreenF3Cidelsa"), GuiMain::onFullScreen)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeCidelsa"), GuiMain::onVolume) 
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeCidelsa"), GuiMain::onVolume) 
	EVT_BUTTON(XRCID("ColoursCidelsa"), Main::onColoursDef)

	EVT_CHOICE(XRCID("CidelsaDifficulty"), GuiCidelsa::onCidelsaDifficulty)
	EVT_CHOICE(XRCID("CidelsaBonus"), GuiCidelsa::onCidelsaBonus)
	EVT_CHOICE(XRCID("CidelsaLives"), GuiCidelsa::onCidelsaLives)
	EVT_CHOICE(XRCID("CidelsaCoin"), GuiCidelsa::onCidelsaCoin)
	EVT_CHOICE(XRCID("CidelsaDifficultyDraco"), GuiCidelsa::onCidelsaDifficultyDraco)
	EVT_CHOICE(XRCID("CidelsaBonusDraco"), GuiCidelsa::onCidelsaBonusDraco)
	EVT_CHOICE(XRCID("CidelsaLivesDraco"), GuiCidelsa::onCidelsaLivesDraco)
	EVT_CHOICE(XRCID("CidelsaCoinDraco"), GuiCidelsa::onCidelsaCoinDraco)
    EVT_CHOICE(XRCID("CidelsaHw"), GuiCidelsa::onCidelsaHw)

END_EVENT_TABLE()

GuiCidelsa::GuiCidelsa(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiTelmac(title, pos, size, mode, dataDir, iniDir)
{
}

void GuiCidelsa::readCidelsaConfig()
{
	selectedComputer_ = CIDELSA;

    conf[CIDELSA].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Cidelsa" + pathSeparator_;
    
	conf[CIDELSA].romDir_[MAINROM1] = readConfigDir("/Dir/Cidelsa/Main_Rom_File", dataDir_ + "Cidelsa" + pathSeparator_);
	conf[CIDELSA].screenDumpFileDir_ = readConfigDir("/Dir/Cidelsa/Video_Dump_File", dataDir_ + "Cidelsa" + pathSeparator_);

	conf[CIDELSA].rom_[MAINROM1] = configPointer->Read("/Cidelsa/Main_Rom_File", "");
	conf[CIDELSA].screenDumpFile_ = configPointer->Read("/Cidelsa/Video_Dump_File", "screendump.png");

	wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[CIDELSA].zoom_ = configPointer->Read("/Cidelsa/Zoom", defaultZoom);
	conf[CIDELSA].volume_ = (int)configPointer->Read("/Cidelsa/Volume", 25l);

	in2Value_ = configPointer->Read("/Cidelsa/Difficulty", 0l) ^ 0x3;
	in2Value_ = (in2Value_ & 0xf3) | ((configPointer->Read("/Cidelsa/Bonus_Lives", 0l) << 2) ^ 0xc);
	in2Value_ = (in2Value_ & 0xcf) | ((configPointer->Read("/Cidelsa/Number_Of_Lives", 3l) << 4) ^ 0x30);
	in2Value_ = (in2Value_ & 0x3f) | ((configPointer->Read("/Cidelsa/Coins", 3l) << 6) ^ 0xc0);

	in2ValueDraco_ = (configPointer->Read("/Cidelsa/Difficulty_Draco", 0l) + 1) ^ 0x3;
	in2ValueDraco_ = (in2ValueDraco_ & 0xf3) | ((configPointer->Read("/Cidelsa/Bonus_Lives_Draco", 0l) << 2) ^ 0xc);
	in2ValueDraco_ = (in2ValueDraco_ & 0x0f) | ((configPointer->Read("/Cidelsa/Number_Of_Lives_Draco", 1l) << 4) ^ 0x10);
	in2ValueDraco_ = (in2ValueDraco_ & 0x1f) | dracoCoin[configPointer->Read("/Cidelsa/Coins_Draco", 7l)];

    hwConfiguration_ = configPointer->Read("/Cidelsa/HwConfiguration", 0l);

	wxString defaultClock;
	defaultClock.Printf("%1.3f", 3.579);
	conf[CIDELSA].clock_ = configPointer->Read("/Cidelsa/Clock_Speed", defaultClock);
	conf[CIDELSA].realCassetteLoad_ = false;

	if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomCidelsa", wxComboBox)->SetValue(conf[CIDELSA].rom_[MAINROM1]);
        
        correctZoomAndValue(CIDELSA, "Cidelsa", SET_SPIN);

		XRCCTRL(*this, "VolumeCidelsa", wxSlider)->SetValue(conf[CIDELSA].volume_);
		XRCCTRL(*this, "ScreenDumpFileCidelsa", wxComboBox)->SetValue(conf[CIDELSA].screenDumpFile_);
		
        XRCCTRL(*this, "CidelsaHw", wxChoice)->SetSelection(hwConfiguration_);
		XRCCTRL(*this, "CidelsaDifficulty", wxChoice)->SetSelection((in2Value_ & 0x03) ^ 0x3);
		XRCCTRL(*this, "CidelsaBonus", wxChoice)->SetSelection(((in2Value_ & 0xc) >> 2) ^ 0x3);
		XRCCTRL(*this, "CidelsaLives", wxChoice)->SetSelection(((in2Value_ & 0x30) >> 4) ^ 0x3);
		XRCCTRL(*this, "CidelsaCoin", wxChoice)->SetSelection(((in2Value_ & 0xc0) >> 6) ^ 0x3);

		XRCCTRL(*this, "CidelsaDifficultyDraco", wxChoice)->SetSelection(((in2ValueDraco_ & 0x3) ^ 0x3) - 1);
		XRCCTRL(*this, "CidelsaBonusDraco", wxChoice)->SetSelection(((in2ValueDraco_ & 0xc) >> 2) ^ 0x3);
		XRCCTRL(*this, "CidelsaLivesDraco", wxChoice)->SetSelection(((in2ValueDraco_ & 0x10) >> 4) ^ 0x1);
		int i = 0;
		while (dracoCoin[i] != (in2ValueDraco_ & 0xe0) && i <= 7)
			i++;
		XRCCTRL(*this, "CidelsaCoinDraco", wxChoice)->SetSelection(i);
        if (clockTextCtrl[CIDELSA] != NULL)
            clockTextCtrl[CIDELSA]->ChangeValue(conf[CIDELSA].clock_);
	}
}

void GuiCidelsa::writeCidelsaDirConfig()
{
    writeConfigDir("/Dir/Cidelsa/Main_Rom_File", conf[CIDELSA].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Cidelsa/Video_Dump_File", conf[CIDELSA].screenDumpFileDir_);
}

void GuiCidelsa::writeCidelsaConfig()
{
	configPointer->Write("/Cidelsa/Main_Rom_File", conf[CIDELSA].rom_[MAINROM1]);
	configPointer->Write("/Cidelsa/Video_Dump_File", conf[CIDELSA].screenDumpFile_);

	configPointer->Write("/Cidelsa/Zoom", conf[CIDELSA].zoom_);
	configPointer->Write("/Cidelsa/Volume", conf[CIDELSA].volume_);
	configPointer->Write("/Cidelsa/Difficulty", (in2Value_ & 0x03) ^ 0x3);
	configPointer->Write("/Cidelsa/Bonus_Lives", ((in2Value_ & 0xc) >> 2) ^ 0x3);
	configPointer->Write("/Cidelsa/Number_Of_Lives", ((in2Value_ & 0x30) >> 4) ^ 0x3);
	configPointer->Write("/Cidelsa/Coins", ((in2Value_ & 0xc0) >> 6) ^ 0x3);
	configPointer->Write("/Cidelsa/Difficulty_Draco", ((in2ValueDraco_ & 0x3) ^ 0x3) - 1);
	configPointer->Write("/Cidelsa/Bonus_Lives_Draco", ((in2ValueDraco_ & 0xc) >> 2) ^ 0x3);
    configPointer->Write("/Cidelsa/Number_Of_Lives_Draco", ((in2ValueDraco_ & 0x10) >> 4) ^ 0x1);
    configPointer->Write("/Cidelsa/HwConfiguration", hwConfiguration_);
	int i = 0;
	while (dracoCoin[i] != (in2ValueDraco_ & 0xe0) && i <= 7)
		i++;
	configPointer->Write("/Cidelsa/Coins_Draco", i);

    configPointer->Write("/Cidelsa/Clock_Speed", conf[CIDELSA].clock_);
}

void GuiCidelsa::readCidelsaWindowConfig()
{
    conf[CIDELSA].mainX_ = (int)configPointer->Read("/Cidelsa/Window_Position_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[CIDELSA].mainY_ = (int)configPointer->Read("/Cidelsa/Window_Position_Y", mainWindowY_);
}

void GuiCidelsa::writeCidelsaWindowConfig()
{
    if (conf[CIDELSA].mainX_ > 0)
        configPointer->Write("/Cidelsa/Window_Position_X", conf[CIDELSA].mainX_);
    if (conf[CIDELSA].mainY_ > 0)
        configPointer->Write("/Cidelsa/Window_Position_Y", conf[CIDELSA].mainY_);
}

void GuiCidelsa::onCidelsaDifficulty(wxCommandEvent&event)
{
	in2Value_ = (in2Value_ & 0xfc) | (event.GetSelection() ^ 0x3); 
	if (runningComputer_ == CIDELSA)
		p_Cidelsa->setIn2Value(in2Value_);
}

void GuiCidelsa::onCidelsaBonus(wxCommandEvent&event)
{
	in2Value_ = (in2Value_ & 0xf3) | ((event.GetSelection() << 2) ^ 0xc); 
	if (runningComputer_ == CIDELSA)
		p_Cidelsa->setIn2Value(in2Value_);
}

void GuiCidelsa::onCidelsaLives(wxCommandEvent&event)
{
	in2Value_ = (in2Value_ & 0xcf) | ((event.GetSelection() << 4) ^ 0x30); 
	if (runningComputer_ == CIDELSA)
		p_Cidelsa->setIn2Value(in2Value_);
}

void GuiCidelsa::onCidelsaCoin(wxCommandEvent&event)
{
	in2Value_ = (in2Value_ & 0x3f) | ((event.GetSelection() << 6) ^ 0xc0); 
	if (runningComputer_ == CIDELSA)
		p_Cidelsa->setIn2Value(in2Value_);
}

void GuiCidelsa::onCidelsaDifficultyDraco(wxCommandEvent&event)
{
	in2ValueDraco_ = (in2ValueDraco_ & 0xfc) | ((event.GetSelection() + 1) ^ 0x3); 
	if (runningComputer_ == CIDELSA)
		p_Cidelsa->setIn2ValueDraco(in2ValueDraco_);
}

void GuiCidelsa::onCidelsaBonusDraco(wxCommandEvent&event)
{
	in2ValueDraco_ = (in2ValueDraco_ & 0xf3) | ((event.GetSelection() << 2) ^ 0xc); 
	if (runningComputer_ == CIDELSA)
		p_Cidelsa->setIn2ValueDraco(in2ValueDraco_);
}

void GuiCidelsa::onCidelsaLivesDraco(wxCommandEvent&event)
{
	in2ValueDraco_ = (in2ValueDraco_ & 0xef) | ((event.GetSelection() << 4) ^ 0x10); 
	if (runningComputer_ == CIDELSA)
		p_Cidelsa->setIn2ValueDraco(in2ValueDraco_);
}

void GuiCidelsa::onCidelsaCoinDraco(wxCommandEvent&event)
{
	in2ValueDraco_ = (in2ValueDraco_ & 0x1f) | dracoCoin[event.GetSelection()]; 
	if (runningComputer_ == CIDELSA)
		p_Cidelsa->setIn2ValueDraco(in2ValueDraco_);
}

void GuiCidelsa::onCidelsaHw(wxCommandEvent&event)
{
    hwConfiguration_ = event.GetSelection();
}



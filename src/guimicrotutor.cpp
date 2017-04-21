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

BEGIN_EVENT_TABLE(GuiMicrotutor, GuiMembership)

	EVT_TEXT(XRCID("MainRomMicrotutor"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomMicrotutor"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonMicrotutor"), GuiMain::onMainRom1)

	EVT_BUTTON(XRCID("SaveButtonMicrotutor"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("LoadButtonMicrotutor"), GuiMain::onLoadButton)
	EVT_CHECKBOX(XRCID("AutoBootMicrotutor"), GuiElf::onAutoBoot)
	EVT_TEXT(XRCID("BootAddressMicrotutor"), GuiMain::onBootAddress)

	EVT_TEXT(XRCID("ShowAddressMicrotutor"), GuiMain::onLedTimer)

	EVT_TEXT(XRCID("SaveStartMicrotutor"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveEndMicrotutor"), GuiMain::onSaveEnd)

END_EVENT_TABLE()

GuiMicrotutor::GuiMicrotutor(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir)
: GuiMembership(title, pos, size, mode, dataDir)
{
	conf[MICROTUTOR].saveStartString_ = "";
	conf[MICROTUTOR].saveEndString_ = "";
    
    position_.x = 0;
}

void GuiMicrotutor::readMicrotutorConfig()
{
	selectedComputer_ = MICROTUTOR;

    conf[MICROTUTOR].configurationDir_ = dataDir_ + "Configurations" + pathSeparator_ + "Microtutor" + pathSeparator_;

    conf[MICROTUTOR].mainDir_ = readConfigDir("/Dir/Microtutor/Main", dataDir_ + "Microtutor" + pathSeparator_);
	conf[MICROTUTOR].romDir_[MAINROM1] = readConfigDir("/Dir/Microtutor/Main_Rom_File", dataDir_ + "Microtutor" + pathSeparator_);
	conf[MICROTUTOR].ramDir_ = readConfigDir("/Dir/Microtutor/Software_File", dataDir_ + "Microtutor" + pathSeparator_);

	conf[MICROTUTOR].rom_[MAINROM1] = configPointer->Read("/Microtutor/Main_Rom_File", "");

	long value;
	wxString bootAddress = configPointer->Read("/Microtutor/Boot_Address", "0001");
	if (!bootAddress.ToLong(&value, 16))
		value = 0;
	conf[MICROTUTOR].bootAddress_ = value;
	configPointer->Read("/Microtutor/Enable_Auto_Boot", &elfConfiguration[MICROTUTOR].autoBoot, false);

	wxString defaultClock;
	defaultClock.Printf("%1.2f", 1.75);
	conf[MICROTUTOR].clock_ = configPointer->Read("/Microtutor/Clock_Speed", defaultClock);

	wxString defaultTimer;
	defaultTimer.Printf("%d", 100);
	conf[MICROTUTOR].ledTime_ = configPointer->Read("/Microtutor/Led_Update_Frequency", defaultTimer);
	conf[MICROTUTOR].realCassetteLoad_ = false;

	if (mode_.gui)
	{
		XRCCTRL(*this, "MainRomMicrotutor", wxComboBox)->SetValue(conf[MICROTUTOR].rom_[MAINROM1]);
		XRCCTRL(*this, "BootAddressMicrotutor", wxTextCtrl)->SetValue(bootAddress);
		XRCCTRL(*this, "AutoBootMicrotutor", wxCheckBox)->SetValue(elfConfiguration[MICROTUTOR].autoBoot);
		clockTextCtrl[MICROTUTOR]->ChangeValue(conf[MICROTUTOR].clock_);
	}

	elfConfiguration[MICROTUTOR].usePortExtender = false;
	elfConfiguration[MICROTUTOR].ideEnabled = false;
    elfConfiguration[MICROTUTOR].fdcEnabled = false;
    elfConfiguration[MICROTUTOR].useLedModule = false;
	elfConfiguration[MICROTUTOR].useTape = true;
}

void GuiMicrotutor::writeMicrotutorDirConfig()
{
    writeConfigDir("/Dir/Microtutor/Main", conf[MICROTUTOR].mainDir_);
	writeConfigDir("/Dir/Microtutor/Main_Rom_File", conf[MICROTUTOR].romDir_[MAINROM1]);
	writeConfigDir("/Dir/Microtutor/Software_File", conf[MICROTUTOR].ramDir_);
}

void GuiMicrotutor::writeMicrotutorConfig()
{
	wxString buffer;

    configPointer->Write("/Microtutor/Main_Rom_File", conf[MICROTUTOR].rom_[MAINROM1]);
	buffer.Printf("%04X", (unsigned int)conf[MICROTUTOR].bootAddress_);
	configPointer->Write("/Microtutor/Boot_Address", buffer);
	configPointer->Write("/Microtutor/Enable_Auto_Boot", elfConfiguration[MICROTUTOR].autoBoot);
	configPointer->Write("/Microtutor/Led_Update_Frequency", conf[MICROTUTOR].ledTime_);

    configPointer->Write("/Microtutor/Clock_Speed", conf[MICROTUTOR].clock_);
}

void GuiMicrotutor::readMicrotutorWindowConfig()
{
    conf[MICROTUTOR].mainX_ = (int)configPointer->Read("/Microtutor/Window_Position_X", mainWindowX_);
    conf[MICROTUTOR].mainY_ = (int)configPointer->Read("/Microtutor/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiMicrotutor::writeMicrotutorWindowConfig()
{
	if (conf[MICROTUTOR].mainX_ > 0)
        configPointer->Write("/Microtutor/Window_Position_X", conf[MICROTUTOR].mainX_);
    if (conf[MICROTUTOR].mainY_ > 0)
        configPointer->Write("/Microtutor/Window_Position_Y", conf[MICROTUTOR].mainY_);
}
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

BEGIN_EVENT_TABLE(GuiUc1800, GuiMembership)

    EVT_TEXT(XRCID("RamSWUC1800"), GuiMain::onRamSWText)
    EVT_COMBOBOX(XRCID("RamSWUC1800"), GuiMain::onRamSWText)
    EVT_BUTTON(XRCID("RamSWButtonUC1800"), GuiMain::onRamSW)

    EVT_BUTTON(XRCID("SaveButtonUC1800"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonUC1800"), GuiMain::onLoadButton)
    EVT_CHECKBOX(XRCID("AutoBootUC1800"), GuiElf::onAutoBoot)
    EVT_TEXT(XRCID("BootAddressUC1800"), GuiMain::onBootAddress)

    EVT_TEXT(XRCID("ShowAddressUC1800"), GuiMain::onLedTimer)

    EVT_TEXT(XRCID("SaveStartUC1800"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndUC1800"), GuiMain::onSaveEnd)

 //   EVT_SPINCTRL(XRCID("HexOutputUC1800"), GuiUc1800::onUC1800HexOutput)
 //   EVT_SPINCTRL(XRCID("HexInputUC1800"), GuiUc1800::onUC1800HexInput)

    EVT_BUTTON(XRCID("KeyMapUC1800"), Main::onHexKeyDef)

END_EVENT_TABLE()

GuiUc1800::GuiUc1800(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiMembership(title, pos, size, mode, dataDir, iniDir)
{
    conf[UC1800].saveStartString_ = "";
    conf[UC1800].saveEndString_ = "";
}

void GuiUc1800::readUc1800Config()
{
    selectedComputer_ = UC1800;

    conf[UC1800].emsConfigNumber_ = 0;
    conf[UC1800].videoNumber_ = 0;

    conf[UC1800].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "UC1800" + pathSeparator_;

    conf[UC1800].mainDir_ = readConfigDir("/Dir/UC1800/Main", dataDir_ + "UC1800" + pathSeparator_);
    conf[UC1800].ramDir_ = readConfigDir("/Dir/UC1800/Software_File", dataDir_ + "UC1800" + pathSeparator_);

    conf[UC1800].ram_ = configPointer->Read("/UC1800/Main_Ram_File", "keybug_2020.bin");

    long value;
    wxString bootAddress = configPointer->Read("/UC1800/Boot_Address", "0000");
    if (!bootAddress.ToLong(&value, 16))
        value = 0;
    conf[UC1800].bootAddress_ = value;
    configPointer->Read("/UC1800/Enable_Auto_Boot", &elfConfiguration[UC1800].autoBoot, false);

    wxString defaultClock;
    defaultClock.Printf("%1.2f", 1.75);
    conf[UC1800].clock_ = convertLocale(configPointer->Read("/UC1800/Clock_Speed", defaultClock));

    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[UC1800].ledTime_ = configPointer->Read("/UC1800/Led_Update_Frequency", defaultTimer);
    conf[UC1800].realCassetteLoad_ = false;

    elfConfiguration[UC1800].ioConfiguration.hexOutput.portNumber = (int)configPointer->Read("UC1800/HexOutput", 4l);
    elfConfiguration[UC1800].ioConfiguration.hexInput.portNumber = (int)configPointer->Read("UC1800/HexInput", 4l);

    conf[UC1800].saveStartString_ = configPointer->Read("/UC1800/SaveStart", "0");
    if (!conf[UC1800].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[UC1800].saveStart_ = value;

    if (mode_.gui)
    {
        XRCCTRL(*this, "RamSWUC1800", wxComboBox)->SetValue(conf[UC1800].ram_);
        XRCCTRL(*this, "BootAddressUC1800", wxTextCtrl)->SetValue(bootAddress);
        XRCCTRL(*this, "AutoBootUC1800", wxCheckBox)->SetValue(elfConfiguration[UC1800].autoBoot);
        if (clockTextCtrl[UC1800] != NULL)
            clockTextCtrl[UC1800]->ChangeValue(conf[UC1800].clock_);
 //       XRCCTRL(*this, "HexOutputUC1800", wxSpinCtrl)->SetValue(elfConfiguration[UC1800].ioConfiguration.hexOutput.portNumber);
 //       XRCCTRL(*this, "HexInputUC1800", wxSpinCtrl)->SetValue(elfConfiguration[UC1800].ioConfiguration.hexInput.portNumber);
        if (conf[UC1800].saveStart_ != 0)
            XRCCTRL(*this, "SaveStartUC1800", wxTextCtrl)->SetValue(conf[UC1800].saveStartString_);    
    }
    
    elfConfiguration[UC1800].usePortExtender = false;
    elfConfiguration[UC1800].ideEnabled = false;
    elfConfiguration[UC1800].fdc1793Enabled = false;
    elfConfiguration[UC1800].useLedModule = false;
    elfConfiguration[UC1800].useTape = true;
}

void GuiUc1800::writeUc1800DirConfig()
{
    writeConfigDir("/Dir/UC1800/Main", conf[UC1800].mainDir_);
    writeConfigDir("/Dir/UC1800/Software_File", conf[UC1800].ramDir_);
}

void GuiUc1800::writeUc1800Config()
{
    wxString buffer;

    configPointer->Write("/UC1800/Main_Ram_File", conf[UC1800].ram_);
    buffer.Printf("%04X", (unsigned int)conf[UC1800].bootAddress_);
    configPointer->Write("/UC1800/Boot_Address", buffer);
    configPointer->Write("/UC1800/Enable_Auto_Boot", elfConfiguration[UC1800].autoBoot);
    configPointer->Write("/UC1800/Led_Update_Frequency", conf[UC1800].ledTime_);
    configPointer->Write("/UC1800/SaveStart", conf[UC1800].saveStartString_);

    configPointer->Write("/UC1800/Clock_Speed", conf[UC1800].clock_);
    configPointer->Write("UC1800/HexOutput", elfConfiguration[UC1800].ioConfiguration.hexOutput.portNumber);
    configPointer->Write("UC1800/HexInput", elfConfiguration[UC1800].ioConfiguration.hexInput.portNumber);
}

void GuiUc1800::readUc1800WindowConfig()
{
    conf[UC1800].mainX_ = (int)configPointer->Read("/UC1800/Window_Position_X", mainWindowX_);
    conf[UC1800].mainY_ = (int)configPointer->Read("/UC1800/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiUc1800::writeUc1800WindowConfig()
{
    if (conf[UC1800].mainX_ > 0)
        configPointer->Write("/UC1800/Window_Position_X", conf[UC1800].mainX_);
    if (conf[UC1800].mainY_ > 0)
        configPointer->Write("/UC1800/Window_Position_Y", conf[UC1800].mainY_);
}

void GuiUc1800::onUC1800HexOutput(wxSpinEvent&event)
{
    elfConfiguration[UC1800].ioConfiguration.hexOutput.portNumber = event.GetPosition();
}

void GuiUc1800::onUC1800HexInput(wxSpinEvent&event)
{
    elfConfiguration[UC1800].ioConfiguration.hexInput.portNumber = event.GetPosition();
}


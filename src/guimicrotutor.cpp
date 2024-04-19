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

BEGIN_EVENT_TABLE(GuiMicrotutor, GuiUc1800)

    EVT_TEXT(XRCID("RamSWMicrotutor"), GuiMain::onRamSWText)
    EVT_COMBOBOX(XRCID("RamSWMicrotutor"), GuiMain::onRamSWText)
    EVT_BUTTON(XRCID("RamSWButtonMicrotutor"), GuiMain::onRamSW)

    EVT_BUTTON(XRCID("SaveButtonMicrotutor"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonMicrotutor"), GuiMain::onLoadButton)
    EVT_CHECKBOX(XRCID("AutoBootMicrotutor"), GuiElf::onAutoBoot)
    EVT_TEXT(XRCID("BootAddressMicrotutor"), GuiMain::onBootAddress)

    EVT_TEXT(XRCID("ShowAddressMicrotutor"), GuiMain::onLedTimer)

    EVT_TEXT(XRCID("SaveStartMicrotutor"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndMicrotutor"), GuiMain::onSaveEnd)

END_EVENT_TABLE()

GuiMicrotutor::GuiMicrotutor(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiUc1800(title, pos, size, mode, dataDir, iniDir)
{
    conf[MICROTUTOR].saveStartString_ = "";
    conf[MICROTUTOR].saveEndString_ = "";
}

void GuiMicrotutor::readMicrotutorConfig()
{
    selectedComputer_ = MICROTUTOR;

    elfConfiguration[MICROTUTOR].useTapeHw = false;
    conf[MICROTUTOR].emsConfigNumber_ = 0;
    conf[MICROTUTOR].videoNumber_ = 0;

    conf[MICROTUTOR].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Microtutor" + pathSeparator_;

    conf[MICROTUTOR].mainDir_ = readConfigDir("/Dir/Microtutor/Main", dataDir_ + "Microtutor" + pathSeparator_);
    conf[MICROTUTOR].ramDir_ = readConfigDir("/Dir/Microtutor/Software_File", dataDir_ + "Microtutor" + pathSeparator_);

    conf[MICROTUTOR].ram_ = configPointer->Read("/Microtutor/Main_Ram_File", "");

    long value;
    wxString bootAddress = configPointer->Read("/Microtutor/Boot_Address", "0001");
    if (!bootAddress.ToLong(&value, 16))
        value = 0;
    conf[MICROTUTOR].bootAddress_ = value;
    configPointer->Read("/Microtutor/Enable_Auto_Boot", &elfConfiguration[MICROTUTOR].autoBoot, false);

    wxString defaultClock;
    defaultClock.Printf("%1.2f", 1.75);
    conf[MICROTUTOR].clock_ = convertLocale(configPointer->Read("/Microtutor/Clock_Speed", defaultClock));

    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[MICROTUTOR].ledTime_ = configPointer->Read("/Microtutor/Led_Update_Frequency", defaultTimer);
    conf[MICROTUTOR].realCassetteLoad_ = false;

    conf[MICROTUTOR].saveStartString_ = configPointer->Read("/Microtutor/SaveStart", "0");
    if (!conf[MICROTUTOR].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[MICROTUTOR].saveStart_ = value;

    if (mode_.gui)
    {
        XRCCTRL(*this, "RamSWMicrotutor", wxComboBox)->SetValue(conf[MICROTUTOR].ram_);
        XRCCTRL(*this, "BootAddressMicrotutor", wxTextCtrl)->SetValue(bootAddress);
        XRCCTRL(*this, "AutoBootMicrotutor", wxCheckBox)->SetValue(elfConfiguration[MICROTUTOR].autoBoot);
        if (clockTextCtrl[MICROTUTOR] != NULL)
            clockTextCtrl[MICROTUTOR]->ChangeValue(conf[MICROTUTOR].clock_);
        if (conf[MICROTUTOR].saveStart_ != 0)
            XRCCTRL(*this, "SaveStartMicrotutor", wxTextCtrl)->SetValue(conf[MEMBER].saveStartString_);
    }

    elfConfiguration[MICROTUTOR].usePortExtender = false;
    elfConfiguration[MICROTUTOR].ideEnabled = false;
    elfConfiguration[MICROTUTOR].fdc1793Enabled = false;
    elfConfiguration[MICROTUTOR].useLedModule = false;
    elfConfiguration[MICROTUTOR].useTape = true;
}

void GuiMicrotutor::writeMicrotutorDirConfig()
{
    writeConfigDir("/Dir/Microtutor/Main", conf[MICROTUTOR].mainDir_);
    writeConfigDir("/Dir/Microtutor/Software_File", conf[MICROTUTOR].ramDir_);
}

void GuiMicrotutor::writeMicrotutorConfig()
{
    wxString buffer;

    configPointer->Write("/Microtutor/Main_Ram_File", conf[MICROTUTOR].ram_);
    buffer.Printf("%04X", (unsigned int)conf[MICROTUTOR].bootAddress_);
    configPointer->Write("/Microtutor/Boot_Address", buffer);
    configPointer->Write("/Microtutor/Enable_Auto_Boot", elfConfiguration[MICROTUTOR].autoBoot);
    configPointer->Write("/Microtutor/Led_Update_Frequency", conf[MICROTUTOR].ledTime_);
    configPointer->Write("/Microtutor/SaveStart", conf[MICROTUTOR].saveStartString_);

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

BEGIN_EVENT_TABLE(GuiMicrotutor2, GuiMicrotutor)

    EVT_TEXT(XRCID("MainRamMMicrotutorII"), GuiMicrotutor2::onMainRamMText)
    EVT_COMBOBOX(XRCID("MainRamMMicrotutorII"), GuiMicrotutor2::onMainRamMText)
    EVT_BUTTON(XRCID("RamButtonMMicrotutorII"), GuiMicrotutor2::onMainRamM)

    EVT_TEXT(XRCID("MainRamEMicrotutorII"), GuiMicrotutor2::onMainRamEText)
    EVT_COMBOBOX(XRCID("MainRamEMicrotutorII"), GuiMicrotutor2::onMainRamEText)
    EVT_BUTTON(XRCID("RamButtonEMicrotutorII"), GuiMicrotutor2::onMainRamE)

    EVT_BUTTON(XRCID("SaveButtonMicrotutorII"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonMicrotutorII"), GuiMain::onLoadButton)
    EVT_CHECKBOX(XRCID("AutoBootMicrotutorII"), GuiElf::onAutoBoot)
    EVT_TEXT(XRCID("BootAddressMicrotutorII"), GuiMain::onBootAddress)

    EVT_TEXT(XRCID("ShowAddressMicrotutorII"), GuiMain::onLedTimer)

    EVT_TEXT(XRCID("SaveStartMicrotutorII"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndMicrotutorII"), GuiMain::onSaveEnd)

    EVT_CHOICE(XRCID("RamMicrotutorII"), GuiMain::onChoiceRam)
    EVT_CHECKBOX(XRCID("UtilityMemoryMcrotutor2"), GuiMicrotutor2::onUtilityMemory)

END_EVENT_TABLE()

GuiMicrotutor2::GuiMicrotutor2(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiMicrotutor(title, pos, size, mode, dataDir, iniDir)
{
    conf[MICROTUTOR2].saveStartString_ = "";
    conf[MICROTUTOR2].saveEndString_ = "";
}

void GuiMicrotutor2::readMicrotutor2Config()
{
    selectedComputer_ = MICROTUTOR2;
    
    elfConfiguration[MICROTUTOR2].useTapeHw = false;
    conf[MICROTUTOR2].emsConfigNumber_ = 0;
    conf[MICROTUTOR2].videoNumber_ = 0;

    conf[MICROTUTOR2].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "MicrotutorII" + pathSeparator_;
    
    conf[MICROTUTOR2].mainDir_ = readConfigDir("/Dir/MicrotutorII/Main", dataDir_ + "MicrotutorII" + pathSeparator_);
    conf[MICROTUTOR2].romDir_[MAINROM1] = readConfigDir("/Dir/MicrotutorII/Main_Ram_1_File", dataDir_ + "MicrotutorII" + pathSeparator_);
    conf[MICROTUTOR2].romDir_[MAINROM2] = readConfigDir("/Dir/MicrotutorII/Main_Ram_2_File", dataDir_ + "MicrotutorII" + pathSeparator_);
    conf[MICROTUTOR2].ramDir_ = readConfigDir("/Dir/MicrotutorII/Software_File", dataDir_ + "MicrotutorII" + pathSeparator_);
    
    conf[MICROTUTOR2].rom_[MAINROM1] = configPointer->Read("/MicrotutorII/Main_Ram_1_File", "TOPS.bin");
    conf[MICROTUTOR2].rom_[MAINROM2] = configPointer->Read("/MicrotutorII/Main_Ram_2_File", "Blink.bin");

    long value;
    wxString bootAddress = configPointer->Read("/MicrotutorII/Boot_Address", "0000");
    if (!bootAddress.ToLong(&value, 16))
        value = 0;
    conf[MICROTUTOR2].bootAddress_ = value;
    configPointer->Read("/MicrotutorII/Enable_Auto_Boot", &elfConfiguration[MICROTUTOR2].autoBoot, false);
    
    wxString defaultClock;
    defaultClock.Printf("%1.2f", 1.75);
    conf[MICROTUTOR2].clock_ = convertLocale(configPointer->Read("/MicrotutorII/Clock_Speed", defaultClock));
    
    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[MICROTUTOR2].ledTime_ = configPointer->Read("/MicrotutorII/Led_Update_Frequency", defaultTimer);
    conf[MICROTUTOR2].realCassetteLoad_ = false;
    
    conf[MICROTUTOR2].ramType_ = (int)configPointer->Read("/MicrotutorII/Ram_Type", 0l);
    configPointer->Read("/MicrotutorII/UtilityMemory", &elfConfiguration[MICROTUTOR2].utilityMemory, true);

    conf[MICROTUTOR2].saveStartString_ = configPointer->Read("/MicrotutorII/SaveStart", "0");
    if (!conf[MICROTUTOR2].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[MICROTUTOR2].saveStart_ = value;
    
    if (mode_.gui)
    {
        XRCCTRL(*this, "MainRamMMicrotutorII", wxComboBox)->SetValue(conf[MICROTUTOR2].rom_[MAINROM1]);
        XRCCTRL(*this, "MainRamEMicrotutorII", wxComboBox)->SetValue(conf[MICROTUTOR2].rom_[MAINROM2]);
        XRCCTRL(*this, "BootAddressMicrotutorII", wxTextCtrl)->SetValue(bootAddress);
        XRCCTRL(*this, "AutoBootMicrotutorII", wxCheckBox)->SetValue(elfConfiguration[MICROTUTOR2].autoBoot);
        if (clockTextCtrl[MICROTUTOR2] != NULL)
            clockTextCtrl[MICROTUTOR2]->ChangeValue(conf[MICROTUTOR2].clock_);
        XRCCTRL(*this, "RamMicrotutorII", wxChoice)->SetSelection(conf[MICROTUTOR2].ramType_);
        XRCCTRL(*this, "UtilityMemoryMcrotutor2", wxCheckBox)->SetValue(elfConfiguration[MICROTUTOR2].utilityMemory);
        
        XRCCTRL(*this, "RamButtonEMicrotutorII", wxButton)->Enable(elfConfiguration[MICROTUTOR2].utilityMemory);
        XRCCTRL(*this, "MainRamEMicrotutorII", wxComboBox)->Enable(elfConfiguration[MICROTUTOR2].utilityMemory);
        
        if (elfConfiguration[MICROTUTOR2].utilityMemory)
            XRCCTRL(*this, "RamText2MicrotutorII", wxStaticText)->SetLabel("per Slot");
        else
            XRCCTRL(*this, "RamText2MicrotutorII", wxStaticText)->SetLabel("");
        if (conf[MICROTUTOR2].saveStart_ != 0)
            XRCCTRL(*this, "SaveStartMicrotutorII", wxTextCtrl)->SetValue(conf[MICROTUTOR2].saveStartString_);
    }
    
    elfConfiguration[MICROTUTOR2].usePortExtender = false;
    elfConfiguration[MICROTUTOR2].ideEnabled = false;
    elfConfiguration[MICROTUTOR2].fdc1793Enabled = false;
    elfConfiguration[MICROTUTOR2].useLedModule = false;
    elfConfiguration[MICROTUTOR2].useTape = true;
}

void GuiMicrotutor2::writeMicrotutor2DirConfig()
{
    writeConfigDir("/Dir/MicrotutorII/Main", conf[MICROTUTOR2].mainDir_);
    writeConfigDir("/Dir/MicrotutorII/Main_Ram_1_File", conf[MICROTUTOR2].romDir_[MAINROM1]);
    writeConfigDir("/Dir/MicrotutorII/Main_Ram_2_File", conf[MICROTUTOR2].romDir_[MAINROM2]);
    writeConfigDir("/Dir/MicrotutorII/Software_File", conf[MICROTUTOR2].ramDir_);
}

void GuiMicrotutor2::writeMicrotutor2Config()
{
    wxString buffer;
    
    configPointer->Write("/MicrotutorII/Main_Ram_1_File", conf[MICROTUTOR2].rom_[MAINROM1]);
    configPointer->Write("/MicrotutorII/Main_Ram_2_File", conf[MICROTUTOR2].rom_[MAINROM2]);
    buffer.Printf("%04X", (unsigned int)conf[MICROTUTOR2].bootAddress_);
    configPointer->Write("/MicrotutorII/Boot_Address", buffer);
    configPointer->Write("/MicrotutorII/Enable_Auto_Boot", elfConfiguration[MICROTUTOR2].autoBoot);
    configPointer->Write("/MicrotutorII/Led_Update_Frequency", conf[MICROTUTOR2].ledTime_);
    configPointer->Write("/MicrotutorII/SaveStart", conf[MICROTUTOR2].saveStartString_);

    configPointer->Write("/MicrotutorII/Clock_Speed", conf[MICROTUTOR2].clock_);
    configPointer->Write("/MicrotutorII/Ram_Type", conf[MICROTUTOR2].ramType_);
    configPointer->Write("/MicrotutorII/UtilityMemory", elfConfiguration[MICROTUTOR2].utilityMemory);
}

void GuiMicrotutor2::readMicrotutor2WindowConfig()
{
    conf[MICROTUTOR2].mainX_ = (int)configPointer->Read("/MicrotutorII/Window_Position_X", mainWindowX_);
    conf[MICROTUTOR2].mainY_ = (int)configPointer->Read("/MicrotutorII/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiMicrotutor2::writeMicrotutor2WindowConfig()
{
    if (conf[MICROTUTOR2].mainX_ > 0)
        configPointer->Write("/MicrotutorII/Window_Position_X", conf[MICROTUTOR2].mainX_);
    if (conf[MICROTUTOR2].mainY_ > 0)
        configPointer->Write("/MicrotutorII/Window_Position_Y", conf[MICROTUTOR2].mainY_);
}

void GuiMicrotutor2::onUtilityMemory(wxCommandEvent&event)
{
    elfConfiguration[MICROTUTOR2].utilityMemory = event.IsChecked();
    
    XRCCTRL(*this, "RamButtonEMicrotutorII", wxButton)->Enable(elfConfiguration[MICROTUTOR2].utilityMemory);
    XRCCTRL(*this, "MainRamEMicrotutorII", wxComboBox)->Enable(elfConfiguration[MICROTUTOR2].utilityMemory);

    if (elfConfiguration[MICROTUTOR2].utilityMemory)
        XRCCTRL(*this, "RamText2MicrotutorII", wxStaticText)->SetLabel("per Slot");
    else
        XRCCTRL(*this, "RamText2MicrotutorII", wxStaticText)->SetLabel("");
}

void GuiMicrotutor2::onMainRamM(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" RAM - M file to load",
                              conf[selectedComputer_].romDir_[MAINROM1], XRCCTRL(*this, "MainRamMMicrotutorII", wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.arc;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|Intel Hex File|*.hex|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    conf[selectedComputer_].romDir_[MAINROM1] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].rom_[MAINROM1] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRamMMicrotutorII", wxComboBox)->SetValue(conf[selectedComputer_].rom_[MAINROM1]);
}

void GuiMicrotutor2::onMainRamMText(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].rom_[MAINROM1] = XRCCTRL(*this, "MainRamMMicrotutorII", wxComboBox)->GetValue();
}

void GuiMicrotutor2::onMainRamE(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" RAM - E file to load",
                              conf[selectedComputer_].romDir_[MAINROM2], XRCCTRL(*this, "MainRamEMicrotutorII", wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    conf[selectedComputer_].romDir_[MAINROM2] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].rom_[MAINROM2] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRamEMicrotutorII", wxComboBox)->SetValue(conf[selectedComputer_].rom_[MAINROM2]);
}

void GuiMicrotutor2::onMainRamEText(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].rom_[MAINROM2] = XRCCTRL(*this, "MainRamEMicrotutorII", wxComboBox)->GetValue();
}



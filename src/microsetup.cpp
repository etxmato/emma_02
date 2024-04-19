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

#include "main.h"
#include "microsetup.h"

#include "wx/xrc/xmlres.h"             
#include "wx/spinctrl.h"
#include "wx/statline.h"

wxString romLocation[2][24]=
{
    {"0800 - 0FFF", "2800 - 2FFF", "4800 - 4FFF", "6800 - 6FFF", "8800 - 8FFF", "A800 - AFFF", "C800 - CFFF", "E800 - EFFF" },
};

BEGIN_EVENT_TABLE(One600SocketSetupDialog, wxDialog)
    EVT_BUTTON(XRCID("MicroSetupSave"), One600SocketSetupDialog::onSaveButton)
END_EVENT_TABLE()

One600SocketSetupDialog::One600SocketSetupDialog(MicroboardSetupDialog* parent, Conf configuration)
{
    parent_ = parent;
    configuration_ = configuration;

    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup_" + p_Main->getFontSize() + ".xrc");
    
    ramSize_ = 2 << (configuration_.microChipType_[ONE_SOCKET]);
    wxString xmlString;
    xmlString.Printf("MicroSetup_1_%01dK_Dialog", ramSize_);
    
    wxXmlResource::Get()->LoadDialog(this, parent, xmlString);
    
    int location = configuration_.microChipLocation_[ONE_SOCKET] / (ramSize_ / 2);
    
    XRCCTRL(*this, "MicroSetupMemory", wxChoice)->SetSelection(configuration_.microChipMemory_[U21ROM] );
    XRCCTRL(*this, "MicroSetupDisable", wxCheckBox)->SetValue(configuration_.microChipDisable_[U21ROM]);
    XRCCTRL(*this, "MicroSetupLocation", wxChoice)->SetSelection(location);
    
    if (p_Main->isComputerRunning())
        XRCCTRL(*this, "MicroSetupSave", wxButton)->Disable();
}

void One600SocketSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    configuration_.microChipMemory_[U21ROM] = XRCCTRL(*this, "MicroSetupMemory", wxChoice)->GetSelection();
    configuration_.microChipDisable_[U21ROM] = XRCCTRL(*this, "MicroSetupDisable", wxCheckBox)->GetValue();
    configuration_.microChipLocation_[ONE_SOCKET] = XRCCTRL(*this, "MicroSetupLocation", wxChoice)->GetSelection() * (ramSize_ / 2);
    
    parent_->setConfiguration(configuration_);

    EndModal(wxID_OK);
}

BEGIN_EVENT_TABLE(Four600SocketSetupDialog, wxDialog)
    EVT_BUTTON(XRCID("MicroSetupSave"), Four600SocketSetupDialog::onSaveButton)
    EVT_CHOICE(XRCID("MicroSetupLocationU20"), Four600SocketSetupDialog::onLocation)
    EVT_CHOICE(XRCID("MicroSetupLocationU19"), Four600SocketSetupDialog::onLocation)
    EVT_CHOICE(XRCID("MicroSetupLocationU18"), Four600SocketSetupDialog::onLocation)
    EVT_CHOICE(XRCID("MicroSetupLocationU17"), Four600SocketSetupDialog::onLocation)
END_EVENT_TABLE()

Four600SocketSetupDialog::Four600SocketSetupDialog(MicroboardSetupDialog* parent, Conf configuration)
{
    parent_ = parent;
    configuration_ = configuration;

    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup_" + p_Main->getFontSize() + ".xrc");
    
    ramSize_ = 2 << (configuration_.microChipType_[FOUR_SOCKET]);
    wxString xmlString;
    xmlString.Printf("MicroSetup_4_%01dK_Dialog", ramSize_);
    
    wxXmlResource::Get()->LoadDialog(this, parent, xmlString);
    
    int location = configuration_.microChipLocation_[FOUR_SOCKET] / (ramSize_ / 2);
    
    XRCCTRL(*this, "MicroSetupMemoryU20", wxChoice)->SetSelection(configuration_.microChipMemory_[U20ROM]);
    XRCCTRL(*this, "MicroSetupMemoryU19", wxChoice)->SetSelection(configuration_.microChipMemory_[U19ROM]);
    XRCCTRL(*this, "MicroSetupMemoryU18", wxChoice)->SetSelection(configuration_.microChipMemory_[U18ROM]);
    XRCCTRL(*this, "MicroSetupMemoryU17", wxChoice)->SetSelection(configuration_.microChipMemory_[U17ROM]);
    
    XRCCTRL(*this, "MicroSetupDisableU20", wxCheckBox)->SetValue(configuration_.microChipDisable_[U20ROM]);
    XRCCTRL(*this, "MicroSetupDisableU19", wxCheckBox)->SetValue(configuration_.microChipDisable_[U19ROM]);
    XRCCTRL(*this, "MicroSetupDisableU18", wxCheckBox)->SetValue(configuration_.microChipDisable_[U18ROM]);
    XRCCTRL(*this, "MicroSetupDisableU17", wxCheckBox)->SetValue(configuration_.microChipDisable_[U17ROM]);
    
    XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU19", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU18", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU17", wxChoice)->SetSelection(location);
    
    if (p_Main->isComputerRunning())
        XRCCTRL(*this, "MicroSetupSave", wxButton)->Disable();
}

void Four600SocketSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    configuration_.microChipMemory_[U20ROM] = XRCCTRL(*this, "MicroSetupMemoryU20", wxChoice)->GetSelection();
    configuration_.microChipMemory_[U19ROM] = XRCCTRL(*this, "MicroSetupMemoryU19", wxChoice)->GetSelection();
    configuration_.microChipMemory_[U18ROM] = XRCCTRL(*this, "MicroSetupMemoryU18", wxChoice)->GetSelection();
    configuration_.microChipMemory_[U17ROM] = XRCCTRL(*this, "MicroSetupMemoryU17", wxChoice)->GetSelection();

    configuration_.microChipDisable_[U20ROM] = XRCCTRL(*this, "MicroSetupDisableU20", wxCheckBox)->GetValue();
    configuration_.microChipDisable_[U19ROM] = XRCCTRL(*this, "MicroSetupDisableU19", wxCheckBox)->GetValue();
    configuration_.microChipDisable_[U18ROM] = XRCCTRL(*this, "MicroSetupDisableU18", wxCheckBox)->GetValue();
    configuration_.microChipDisable_[U17ROM] = XRCCTRL(*this, "MicroSetupDisableU17", wxCheckBox)->GetValue();

    configuration_.microChipLocation_[FOUR_SOCKET] = XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->GetSelection() * (ramSize_ / 2);
    
    parent_->setConfiguration(configuration_);

    EndModal(wxID_OK);
}

void Four600SocketSetupDialog::onLocation( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU19", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU18", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU17", wxChoice)->SetSelection(location);
}

BEGIN_EVENT_TABLE(Four601SocketSetupDialog, wxDialog)
    EVT_BUTTON(XRCID("MicroSetupSave"), Four601SocketSetupDialog::onSaveButton)
    EVT_CHOICE(XRCID("MicroSetupLocationU20"), Four601SocketSetupDialog::onLocationXu25)
    EVT_CHOICE(XRCID("MicroSetupLocationU19"), Four601SocketSetupDialog::onLocationXu25)
    EVT_CHOICE(XRCID("MicroSetupLocationU18"), Four601SocketSetupDialog::onLocationXu27)
    EVT_CHOICE(XRCID("MicroSetupLocationU17"), Four601SocketSetupDialog::onLocationXu27)
    EVT_CHOICE(XRCID("MicroSetupLocationU20_1"), Four601SocketSetupDialog::onLocationXu)
    EVT_CHOICE(XRCID("MicroSetupLocationU19_1"), Four601SocketSetupDialog::onLocationXu)
    EVT_CHOICE(XRCID("MicroSetupLocationU18_1"), Four601SocketSetupDialog::onLocationXu)
    EVT_CHOICE(XRCID("MicroSetupLocationU17_1"), Four601SocketSetupDialog::onLocationXu)
END_EVENT_TABLE()

Four601SocketSetupDialog::Four601SocketSetupDialog(MicroboardSetupDialog* parent, Conf configuration)
{
    parent_ = parent;
    configuration_ = configuration;

    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup_" + p_Main->getFontSize() + ".xrc");
    
    int locationXu25 = configuration_.microChipLocation_[FOUR_SOCKET_ROM1];
    int locationXu27 = configuration_.microChipLocation_[FOUR_SOCKET_ROM2];
    
    if (configuration_.microChipType_[FOUR_SOCKET] == 1)
    {
        wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S601_4_2K_Dialog");
        XRCCTRL(*this, "MicroSetupMemoryU20", wxChoice)->SetSelection(configuration_.microChipMemory_[XU27ROM]);
        XRCCTRL(*this, "MicroSetupMemoryU19", wxChoice)->SetSelection(configuration_.microChipMemory_[XU26ROM]);
        XRCCTRL(*this, "MicroSetupMemoryU18", wxChoice)->SetSelection(configuration_.microChipMemory_[XU25ROM]);
        XRCCTRL(*this, "MicroSetupMemoryU17", wxChoice)->SetSelection(configuration_.microChipMemory_[XU24ROM]);
        
        XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->SetSelection(locationXu25);
        XRCCTRL(*this, "MicroSetupLocationU19", wxChoice)->SetSelection(locationXu25);
        XRCCTRL(*this, "MicroSetupLocationU18", wxChoice)->SetSelection(locationXu27);
        XRCCTRL(*this, "MicroSetupLocationU17", wxChoice)->SetSelection(locationXu27);
    }
    else
    {
        wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S601_4_1K_Dialog");
        
        XRCCTRL(*this, "MicroSetupLocationU20_1", wxChoice)->SetSelection(locationXu27);
        XRCCTRL(*this, "MicroSetupLocationU19_1", wxChoice)->SetSelection(locationXu27);
        XRCCTRL(*this, "MicroSetupLocationU18_1", wxChoice)->SetSelection(locationXu27);
        XRCCTRL(*this, "MicroSetupLocationU17_1", wxChoice)->SetSelection(locationXu27);
    }
    
    if (p_Main->isComputerRunning())
        XRCCTRL(*this, "MicroSetupSave", wxButton)->Disable();
}

void Four601SocketSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    if (configuration_.microChipType_[FOUR_SOCKET] == 1)
    {
        configuration_.microChipMemory_[XU27ROM] = XRCCTRL(*this, "MicroSetupMemoryU20", wxChoice)->GetSelection();
        configuration_.microChipMemory_[XU26ROM] = XRCCTRL(*this, "MicroSetupMemoryU19", wxChoice)->GetSelection();
        configuration_.microChipMemory_[XU25ROM] = XRCCTRL(*this, "MicroSetupMemoryU18", wxChoice)->GetSelection();
        configuration_.microChipMemory_[XU24ROM] = XRCCTRL(*this, "MicroSetupMemoryU17", wxChoice)->GetSelection();
        
        int locationXu25 = XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->GetSelection();
        configuration_.microChipLocation_[FOUR_SOCKET_ROM1] = locationXu25;
        int locationXu27 = XRCCTRL(*this, "MicroSetupLocationU18", wxChoice)->GetSelection();
        configuration_.microChipLocation_[FOUR_SOCKET_ROM2] = locationXu27;
    }
    else
    {
        int locationXu = XRCCTRL(*this, "MicroSetupLocationU20_1", wxChoice)->GetSelection();
        configuration_.microChipLocation_[FOUR_SOCKET_ROM2] = locationXu;
    }
    
    parent_->setConfiguration(configuration_);

    EndModal(wxID_OK);
}

void Four601SocketSetupDialog::onLocationXu25( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU19", wxChoice)->SetSelection(location);
}

void Four601SocketSetupDialog::onLocationXu27( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU18", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU17", wxChoice)->SetSelection(location);
}

void Four601SocketSetupDialog::onLocationXu( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU20_1", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU19_1", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU18_1", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU17_1", wxChoice)->SetSelection(location);
}

BEGIN_EVENT_TABLE(MicroboardSetupDialog, wxDialog)
    EVT_BUTTON(XRCID("MicroSetupSave"), MicroboardSetupDialog::onSaveButton)

    EVT_TEXT(XRCID("MainRomU21Microboard"), MicroboardSetupDialog::onRomU21Text)
    EVT_COMBOBOX(XRCID("MainRomU21Microboard"), MicroboardSetupDialog::onRomU21Text)
    EVT_BUTTON(XRCID("RomButtonU21Microboard"), MicroboardSetupDialog::onRomU21)

    EVT_TEXT(XRCID("MainRomU20Microboard"), MicroboardSetupDialog::onRomU20Text)
    EVT_COMBOBOX(XRCID("MainRomU20Microboard"), MicroboardSetupDialog::onRomU20Text)
    EVT_BUTTON(XRCID("RomButtonU20Microboard"), MicroboardSetupDialog::onRomU20)

    EVT_TEXT(XRCID("MainRomU19Microboard"), MicroboardSetupDialog::onRomU19Text)
    EVT_COMBOBOX(XRCID("MainRomU19Microboard"), MicroboardSetupDialog::onRomU19Text)
    EVT_BUTTON(XRCID("RomButtonU19Microboard"), MicroboardSetupDialog::onRomU19)

    EVT_TEXT(XRCID("MainRomU18Microboard"), MicroboardSetupDialog::onRomU18Text)
    EVT_COMBOBOX(XRCID("MainRomU18Microboard"), MicroboardSetupDialog::onRomU18Text)
    EVT_BUTTON(XRCID("RomButtonU18Microboard"), MicroboardSetupDialog::onRomU18)

    EVT_TEXT(XRCID("MainRomU17Microboard"), MicroboardSetupDialog::onRomU17Text)
    EVT_COMBOBOX(XRCID("MainRomU17Microboard"), MicroboardSetupDialog::onRomU17Text)
    EVT_BUTTON(XRCID("RomButtonU17Microboard"), MicroboardSetupDialog::onRomU17)

    EVT_BUTTON(XRCID("One600SocketSetupMicroboard"), MicroboardSetupDialog::onOne600SocketSetup)
    EVT_BUTTON(XRCID("Four600SocketSetupMicroboard"), MicroboardSetupDialog::onFour600SocketSetup)
    EVT_BUTTON(XRCID("Four601SocketSetupMicroboard"), MicroboardSetupDialog::onFour601SocketSetup)
    EVT_BUTTON(XRCID("Four603ASocketSetupMicroboard"), MicroboardSetupDialog::onFour601SocketSetup)

    EVT_CHOICE(XRCID("OneSocketBankMicroboard"), MicroboardSetupDialog::onOneSocketBank)
    EVT_CHOICE(XRCID("FourSocketBankMicroboard"), MicroboardSetupDialog::onFourSocketBank)
    EVT_CHOICE(XRCID("VtUartGroup"), MicroboardSetupDialog::onUartGroup)

    EVT_TEXT(XRCID("RamBlockMicroboard"), MicroboardSetupDialog::on604BRamBlock)
    EVT_TEXT(XRCID("RomBlockMicroboard"), MicroboardSetupDialog::on604BRomBlock)

    EVT_TEXT(XRCID("RamBlock602Microboard"), MicroboardSetupDialog::on602RamBlock)
    EVT_TEXT(XRCID("RomBlock602Microboard"), MicroboardSetupDialog::on602RomBlock)

    EVT_CHECKBOX(XRCID("FourSocketBankDIsableMicroboard"), MicroboardSetupDialog::onFourSocketBankDisable)

END_EVENT_TABLE()

MicroboardSetupDialog::MicroboardSetupDialog(wxWindow* parent, Conf configuration, ElfConfiguration elfConfiguration)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup_" + p_Main->getFontSize() + ".xrc");
    configuration_ = configuration;
    elfConfiguration_ = elfConfiguration;
    selectedComputer_ = p_Main->getSelectedComputerId();
    selectedComputerName_ = p_Main->getSelectedComputerText();
    selectedComputerStr_ = p_Main->getSelectedComputerStr();
    memoryError_ = false;
    boardError_ = false;
    guiLoaded_ = false;

    switch (configuration_.microboardType_[0])
    {
        case MICROBOARD_CDP18S600:
            wxXmlResource::Get()->LoadDialog(this, parent, p_Main->getMicroboardTypeStr(configuration_.microboardType_[0])+ "_Setup_Dialog");
        break;
            
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S602:
        case MICROBOARD_CDP18S603:
        case MICROBOARD_CDP18S604B:
            wxXmlResource::Get()->LoadDialog(this, parent, p_Main->getMicroboardTypeStr(configuration_.microboardType_[0])+ "_Setup_Dialog");
        break;
            
        case MICROBOARD_CDP18S606:
        case MICROBOARD_CDP18S607:
        case MICROBOARD_CDP18S608:
            wxXmlResource::Get()->LoadDialog(this, parent, p_Main->getMicroboardTypeStr(configuration_.microboardType_[0]-6)+ "_Setup_Dialog");
            XRCCTRL(*this, "MicroSetupCpuType", wxStaticText)->SetLabel("CPU Type: CDP1805");
        break;

        case MICROBOARD_CDP18S609:
            wxXmlResource::Get()->LoadDialog(this, parent, p_Main->getMicroboardTypeStr(configuration_.microboardType_[0]-5)+ "_Setup_Dialog");
            XRCCTRL(*this, "MicroSetupCpuType", wxStaticText)->SetLabel("CPU Type: CDP1805");
        break;

        case MICROBOARD_CDP18S605:
        case MICROBOARD_CDP18S610:
            wxXmlResource::Get()->LoadDialog(this, parent, p_Main->getMicroboardTypeStr(MICROBOARD_CDP18S602)+ "_Setup_Dialog");
        break;

        case MICROBOARD_CDP18S603A:
            wxXmlResource::Get()->LoadDialog(this, parent, p_Main->getMicroboardTypeStr(MICROBOARD_CDP18S603)+ "_Setup_Dialog");
        break;

        case RCASBC:
            wxXmlResource::Get()->LoadDialog(this, parent, "RCASBC_Setup_Dialog");
        break;
    }
    guiLoaded_ = true;

    this->SetTitle(p_Main->getMicroboardTypeStr(configuration_.microboardType_[0])+ " Setup");
  
    XRCCTRL(*this, "MainRomU21" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U21ROM]);
    XRCCTRL(*this, "MainRomU20" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U20ROM]);

    switch (configuration_.microboardType_[0])
    {
        case MICROBOARD_CDP18S600:
            XRCCTRL(*this, "MainRomU19" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U19ROM]);
            XRCCTRL(*this, "MainRomU18" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U18ROM]);
            XRCCTRL(*this, "MainRomU17" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U17ROM]);
            XRCCTRL(*this, "OneSocketBank" + selectedComputerStr_, wxChoice)->SetSelection(configuration_.microChipType_[ONE_SOCKET]);
            XRCCTRL(*this, "VtUartGroup", wxChoice)->SetSelection(elfConfiguration.uartGroup - 1);
            setOneSocketState();
            setFourSocketState();
            XRCCTRL(*this, "FourSocketBank" + selectedComputerStr_, wxChoice)->SetSelection(configuration_.microChipType_[FOUR_SOCKET]);
        break;
 
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S603:
        case MICROBOARD_CDP18S603A:
        case MICROBOARD_CDP18S606:
        case MICROBOARD_CDP18S608:
            XRCCTRL(*this, "MainRomU19" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U19ROM]);
            XRCCTRL(*this, "MainRomU18" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U18ROM]);
            XRCCTRL(*this, "MainRomU17" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U17ROM]);
            XRCCTRL(*this, "OneSocketBank" + selectedComputerStr_, wxChoice)->SetSelection(configuration_.microChipType_[ONE_SOCKET]);
            XRCCTRL(*this, "FourSocketBank" + selectedComputerStr_, wxChoice)->SetSelection(configuration_.microChipType_[FOUR_SOCKET]);
        break;

        case MICROBOARD_CDP18S602:
        case MICROBOARD_CDP18S605:
        case MICROBOARD_CDP18S607:
        case MICROBOARD_CDP18S610:
            XRCCTRL(*this, "MainRomU19" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U19ROM]);
            XRCCTRL(*this, "RamBlock602" + selectedComputerStr_, wxTextCtrl)->ChangeValue(configuration_.microChipBlock_[ONE_SOCKET]);
            XRCCTRL(*this, "RomBlock602" + selectedComputerStr_, wxTextCtrl)->ChangeValue(configuration_.microChipBlock_[FOUR_SOCKET]);
            XRCCTRL(*this, "FourSocketBankDIsable" + selectedComputerStr_, wxCheckBox)->SetValue(configuration_.microChipDisable_[FOUR_SOCKET]);
            XRCCTRL(*this, "VtUartGroup", wxChoice)->SetSelection(elfConfiguration.uartGroup - 1);
            one602SocketBankGui();
            four602SocketBankGui();
            XRCCTRL(*this, "FourSocketBank" + selectedComputerStr_, wxChoice)->SetSelection(configuration_.microChipType_[FOUR_SOCKET]);
        break;

        case MICROBOARD_CDP18S604B:
        case MICROBOARD_CDP18S609:
            XRCCTRL(*this, "RamBlock" + selectedComputerStr_, wxTextCtrl)->ChangeValue(configuration_.microChipBlock_[ONE_SOCKET]);
            XRCCTRL(*this, "RomBlock" + selectedComputerStr_, wxTextCtrl)->ChangeValue(configuration_.microChipBlock_[FOUR_SOCKET]);
            one604BSocketBankGui();
            four604BSocketBankGui();
            XRCCTRL(*this, "FourSocketBank" + selectedComputerStr_, wxChoice)->SetSelection(configuration_.microChipType_[FOUR_SOCKET]);
        break;

        case RCASBC:
        break;
    }

    updateErrorStatus();

    if (p_Main->isComputerRunning())
        XRCCTRL(*this, "MicroSetupSave", wxButton)->Disable();
}

void MicroboardSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    p_Main->setConfiguration(selectedComputer_, configuration_);
    p_Main->setElfConfiguration(selectedComputer_, elfConfiguration_);

    EndModal(wxID_OK);
}

void MicroboardSetupDialog::checkErrorStatus()
{
    p_Main->checkAllBoardTypes(&configuration_, &elfConfiguration_);
    updateErrorStatus();
}

void MicroboardSetupDialog::updateErrorStatus()
{
    bool refresh = false;
    wxColour colour = XRCCTRL(*this, "MicroSetupSave", wxButton)->GetForegroundColour();

    if (configuration_.errorMemoryOverlapp_[0] != "")
    {
        XRCCTRL(*this, "MemoryErrorMicroboard", wxStaticText)->SetLabel(configuration_.errorMemoryOverlapp_[0]);
        XRCCTRL(*this, "MemoryErrorMicroboard", wxStaticText)->Show();
        if (configuration_.errorMemoryOverlapp_[0].Right(4) == "card")
            XRCCTRL(*this, "MemoryErrorMicroboard", wxStaticText)->SetForegroundColour(colour);
        else
            XRCCTRL(*this, "MemoryErrorMicroboard", wxStaticText)->SetForegroundColour(p_Main->getGuiTextColour(GUI_COL_RED));
        
        if (!memoryError_)
            refresh = true;
        memoryError_ = true;
    }
    else
    {
        XRCCTRL(*this, "MemoryErrorMicroboard", wxStaticText)->Hide();
        if (memoryError_)
            refresh = true;
        memoryError_ = false;
    }
    
    if (configuration_.errorDoubleBoard_[0] != "")
    {
        XRCCTRL(*this, "DoubleBoardErrorMicroboard", wxStaticText)->SetLabel(configuration_.errorDoubleBoard_[0]);
        XRCCTRL(*this, "DoubleBoardErrorMicroboard", wxStaticText)->Show();
        if (!boardError_)
            refresh = true;
        boardError_ = true;
    }
    else
    {
        XRCCTRL(*this, "DoubleBoardErrorMicroboard", wxStaticText)->Hide();
        if (boardError_)
            refresh = true;
        boardError_ = false;
    }

    if (refresh)
        this->DoLayoutAdaptation();
}

void MicroboardSetupDialog::onFourSocketBankDisable(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    configuration_.microChipDisable_[FOUR_SOCKET] = event.IsChecked();
    
    four602SocketBankGui();
    checkErrorStatus();
}

void MicroboardSetupDialog::onRomU21(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    wxString fileName, romName = "";
    switch (configuration_.microboardType_[0])
    {
        case MICROBOARD_CDP18S600:
            romName = " U21";
        break;
            
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S602:
        case MICROBOARD_CDP18S607:
        case MICROBOARD_CDP18S603:
        case MICROBOARD_CDP18S603A:
        case MICROBOARD_CDP18S604B:
        case MICROBOARD_CDP18S605:
        case MICROBOARD_CDP18S606:
        case MICROBOARD_CDP18S608:
        case MICROBOARD_CDP18S609:
        case MICROBOARD_CDP18S610:
        case RCASBC:
            romName = " RAM";
        break;
    }
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+romName+" file to load",
                              configuration_.romDir_[U21ROM], XRCCTRL(*this, "MainRomU21"+selectedComputerStr_, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.arc;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    configuration_.romDir_[U21ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    configuration_.rom_[U21ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU21"+selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U21ROM]);
}

void MicroboardSetupDialog::onRomU21Text(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    configuration_.rom_[U21ROM] = XRCCTRL(*this, "MainRomU21"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardSetupDialog::onRomU20(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    wxString fileName, romName = "";
    switch (configuration_.microboardType_[0])
    {
        case MICROBOARD_CDP18S600:
            romName = " U20";
        break;
     
        case MICROBOARD_CDP18S602:
        case MICROBOARD_CDP18S605:
        case MICROBOARD_CDP18S607:
        case MICROBOARD_CDP18S610:
            romName = " U9";
        break;

        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S603:
        case MICROBOARD_CDP18S603A:
        case MICROBOARD_CDP18S606:
        case MICROBOARD_CDP18S608:
            romName = " XU27 ROM";
        break;

        case MICROBOARD_CDP18S604B:
        case MICROBOARD_CDP18S609:
        case RCASBC:
            romName = " ROM";
        break;
    }

    fileName = wxFileSelector( "Select the "+selectedComputerName_+romName+" file to load",
                              configuration_.romDir_[U20ROM], XRCCTRL(*this, "MainRomU20"+selectedComputerStr_, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.arc;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    configuration_.romDir_[U20ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    configuration_.rom_[U20ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU20"+selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U20ROM]);
}

void MicroboardSetupDialog::onRomU20Text(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    configuration_.rom_[U20ROM] = XRCCTRL(*this, "MainRomU20"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardSetupDialog::onRomU19(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    wxString fileName, romName = "";
    switch (configuration_.microboardType_[0])
    {
        case MICROBOARD_CDP18S600:
            romName = " U19";
        break;
            
        case MICROBOARD_CDP18S602:
        case MICROBOARD_CDP18S605:
        case MICROBOARD_CDP18S607:
        case MICROBOARD_CDP18S610:
            romName = " U10";
        break;
            
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S603:
        case MICROBOARD_CDP18S603A:
        case MICROBOARD_CDP18S606:
        case MICROBOARD_CDP18S608:
            romName = " XU26 ROM";
        break;
    }
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+romName+" ROM file to load",
                              configuration_.romDir_[U19ROM], XRCCTRL(*this, "MainRomU19"+selectedComputerStr_, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.arc;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    configuration_.romDir_[U19ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    configuration_.rom_[U19ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU19"+selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U19ROM]);
}

void MicroboardSetupDialog::onRomU19Text(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    configuration_.rom_[U19ROM] = XRCCTRL(*this, "MainRomU19"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardSetupDialog::onRomU18(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    wxString fileName, romName = "";
    switch (configuration_.microboardType_[0])
    {
        case MICROBOARD_CDP18S600:
            romName = " U18";
        break;
            
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S603:
        case MICROBOARD_CDP18S603A:
        case MICROBOARD_CDP18S606:
        case MICROBOARD_CDP18S608:
            romName = " XU25 ROM";
        break;
    }
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+romName+" ROM file to load",
                              configuration_.romDir_[U18ROM], XRCCTRL(*this, "MainRomU18"+selectedComputerStr_, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.arc;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    configuration_.romDir_[U18ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    configuration_.rom_[U18ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU18"+selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U18ROM]);
}

void MicroboardSetupDialog::onRomU18Text(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    configuration_.rom_[U18ROM] = XRCCTRL(*this, "MainRomU18"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardSetupDialog::onRomU17(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    wxString fileName, romName = "";
    switch (configuration_.microboardType_[0])
    {
        case MICROBOARD_CDP18S600:
            romName = " U17";
        break;
            
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S603:
        case MICROBOARD_CDP18S603A:
        case MICROBOARD_CDP18S606:
        case MICROBOARD_CDP18S608:
            romName = " XU24 ROM";
        break;
    }
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+romName+" ROM file to load",
                              configuration_.romDir_[U17ROM], XRCCTRL(*this, "MainRomU17"+selectedComputerStr_, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.arc;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    configuration_.romDir_[U17ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    configuration_.rom_[U17ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU17"+selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U17ROM]);
}

void MicroboardSetupDialog::onRomU17Text(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    configuration_.rom_[U17ROM] = XRCCTRL(*this, "MainRomU17"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardSetupDialog::setOneSocketState()
{
    setRamlabel(U21ROM, "U21");
}

void MicroboardSetupDialog::setFourSocketState()
{
    if (configuration_.microboardType_[0] == MICROBOARD_CDP18S600)
    {
        setRamlabel(U20ROM, "U20");
        setRamlabel(U19ROM, "U19");
        setRamlabel(U18ROM, "U18");
        setRamlabel(U17ROM, "U17");
    }
}
void MicroboardSetupDialog::setRamlabel(int romNumber, wxString romString)
{
    wxString buttonString = "RomButton" + romString + "Microboard";
    wxString comboString = "MainRom" + romString + "Microboard";
    wxString typeString = "ROM ";
    
    switch (configuration_.microChipMemory_[romNumber])
    {
        case MICRO_RAM:
            typeString = "RAM ";
        break;
            
        case MICRO_CPURAM:
            typeString = "CRAM ";
        break;
    }
    XRCCTRL(*this, buttonString, wxButton)->SetLabel(typeString + romString);
    
    if (configuration_.microChipDisable_[romNumber])
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

void MicroboardSetupDialog::onFour604BSocketBank(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;

    wxString blockStr = XRCCTRL(*this, "RomBlock"+selectedComputerStr_, wxTextCtrl)->GetValue();
    
    long block;
    blockStr.ToLong(&block);
    
    int oldType = convert604BChipType(configuration_.microChipType_[FOUR_SOCKET]);
    
    configuration_.microChipType_[FOUR_SOCKET] = event.GetSelection();
    int newType = convert604BChipType(configuration_.microChipType_[FOUR_SOCKET]);
    
    double multiplier = (double) (oldType) / (double) (newType);
    block = block * multiplier;
    
    blockStr.Printf("%d", (int)block);
    XRCCTRL(*this, "RomBlock"+selectedComputerStr_, wxTextCtrl)->SetValue(blockStr);
    
    four604BSocketBankGui();
    checkErrorStatus();
}

int MicroboardSetupDialog::convert604BChipType(int type)
{
    if (type == 0)
        type = 1;
    
    type = 1 << (type-1);
    return type;
}

void MicroboardSetupDialog::on604BRamBlock(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;

    wxString stringBlock = event.GetString();
    if (stringBlock == "")  stringBlock = "0";
    long block;
    if (!stringBlock.ToLong(&block, 10))
        return;
    
    if (block > 63)
    {
        (void)wxMessageBox( "Invalid block number\n",
                           "Emma 02", wxICON_ERROR | wxOK );
        return;
    }
    
    configuration_.microChipBlock_[ONE_SOCKET] = stringBlock;
    
    one604BSocketBankGui();
    checkErrorStatus();
}

void MicroboardSetupDialog::on604BRomBlock(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;

    int chipType = convert604BChipType(configuration_.microChipType_[FOUR_SOCKET]);
    int maxBlock = (64/chipType)-1;
    
    wxString stringBlock = event.GetString();
    if (stringBlock == "")  stringBlock = "0";
    long block;
    if (!stringBlock.ToLong(&block, 10))
        return;
    
    if (block > maxBlock)
    {
        (void)wxMessageBox( "Invalid block number\n",
                           "Emma 02", wxICON_ERROR | wxOK );
        return;
    }
    
    configuration_.microChipBlock_[FOUR_SOCKET] = stringBlock;
    
    four604BSocketBankGui();
    checkErrorStatus();
}

void MicroboardSetupDialog::one604BSocketBankGui()
{
    long block;
    configuration_.microChipBlock_[ONE_SOCKET].ToLong(&block);

    wxString label;
    label.Printf("address: %04X-%04X", (int)(block*0x400), (int)(block*0x400+0x3ff));
    XRCCTRL(*this, "RamAddress"+selectedComputerStr_, wxStaticText)->SetLabel(label);
}

void MicroboardSetupDialog::four604BSocketBankGui()
{
    long block;

    int chipType = convert604BChipType(configuration_.microChipType_[FOUR_SOCKET]);
    
    wxString label;
    label.Printf("%dK Block (0-%d)", (int)chipType, (int)((64/chipType)-1));
    XRCCTRL(*this, "RomBlockText"+selectedComputerStr_, wxStaticText)->SetLabel(label);
    
    configuration_.microChipBlock_[FOUR_SOCKET].ToLong(&block);
    
    if (configuration_.microChipType_[FOUR_SOCKET] == 0)
        label.Printf("address: %04X-%04X", (int)(block*chipType*0x400), (int)(block*chipType*0x400+0x1ff));
    else
        label.Printf("address: %04X-%04X", (int)(block*chipType*0x400), (int)(block*chipType*0x400+chipType*0x400-1));
    XRCCTRL(*this, "RomAddress"+selectedComputerStr_, wxStaticText)->SetLabel(label);
}

void MicroboardSetupDialog::onFour602SocketBank(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;

    wxString blockStr = XRCCTRL(*this, "RomBlock602"+selectedComputerStr_, wxTextCtrl)->GetValue();
    
    long block;
    blockStr.ToLong(&block);
    
    int oldType = (configuration_.microChipType_[FOUR_SOCKET]+1)*2;
    
    configuration_.microChipType_[FOUR_SOCKET] = event.GetSelection();
    int newType = (configuration_.microChipType_[FOUR_SOCKET]+1)*2;
    
    double multiplier = (double) (oldType) / (double) (newType);
    block = block * multiplier;
    
    blockStr.Printf("%d", (int)block);
    XRCCTRL(*this, "RomBlock602"+selectedComputerStr_, wxTextCtrl)->SetValue(blockStr);
    
    four602SocketBankGui();
    checkErrorStatus();
}

void MicroboardSetupDialog::on602RamBlock(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;

    wxString stringBlock = event.GetString();
    if (stringBlock == "")  stringBlock = "0";
    long block;
    if (!stringBlock.ToLong(&block, 10))
        return;
    
    if (block > 31)
    {
        (void)wxMessageBox( "Invalid block number\n",
                           "Emma 02", wxICON_ERROR | wxOK );
        return;
    }
    
    configuration_.microChipBlock_[ONE_SOCKET] = stringBlock;
    
    one602SocketBankGui();
    checkErrorStatus();
}

void MicroboardSetupDialog::on602RomBlock(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;

    int maxBlock = (32/(configuration_.microChipType_[FOUR_SOCKET]+1))-1;
    
    wxString stringBlock = event.GetString();
    if (stringBlock == "")  stringBlock = "0";
    long block;
    if (!stringBlock.ToLong(&block, 10))
        return;
    
    if (block > maxBlock)
    {
        (void)wxMessageBox( "Invalid block number\n",
                           "Emma 02", wxICON_ERROR | wxOK );
        return;
    }
    
    configuration_.microChipBlock_[FOUR_SOCKET] = stringBlock;
    
    four602SocketBankGui();
    checkErrorStatus();
}

void MicroboardSetupDialog::one602SocketBankGui()
{
    long block;
    configuration_.microChipBlock_[ONE_SOCKET].ToLong(&block);

    wxString label;
    label.Printf("address: %04X-%04X", (int)(block*0x800), (int)(block*0x800+0x7ff));
    XRCCTRL(*this, "RamAddressMicroboard", wxStaticText)->SetLabel(label);
}

void MicroboardSetupDialog::four602SocketBankGui()
{
    long block;

    int chipType = (configuration_.microChipType_[FOUR_SOCKET]+1)*2;

    wxString label;
    label.Printf("%dK Block (0-%d)", (int)chipType, (int)((32/(configuration_.microChipType_[FOUR_SOCKET]+1))-1));
    XRCCTRL(*this, "RomBlockText"+selectedComputerStr_, wxStaticText)->SetLabel(label);
    
    configuration_.microChipBlock_[FOUR_SOCKET].ToLong(&block);
    
    label.Printf("address U9-U10: %04X-%04X", (int)(block*chipType*0x400), (int)(block*chipType*0x400+chipType*0x400-1));
    XRCCTRL(*this, "RomAddress"+selectedComputerStr_, wxStaticText)->SetLabel(label);
    
    if (configuration_.microChipDisable_[FOUR_SOCKET])
    {
        XRCCTRL(*this, "RomSocketText"+selectedComputerStr_, wxStaticText)->Disable();
        XRCCTRL(*this, "FourSocketBank"+selectedComputerStr_, wxChoice)->Disable();
        XRCCTRL(*this, "RomBlockText"+selectedComputerStr_, wxStaticText)->Disable();
        XRCCTRL(*this, "RomBlock602"+selectedComputerStr_, wxTextCtrl)->Disable();
        XRCCTRL(*this, "RomAddress"+selectedComputerStr_, wxStaticText)->Disable();
        XRCCTRL(*this, "RomButtonU20"+selectedComputerStr_, wxButton)->Disable();
        XRCCTRL(*this, "MainRomU20"+selectedComputerStr_, wxComboBox)->Disable();
        XRCCTRL(*this, "RomButtonU19"+selectedComputerStr_, wxButton)->Disable();
        XRCCTRL(*this, "MainRomU19"+selectedComputerStr_, wxComboBox)->Disable();
    }
    else
    {
        XRCCTRL(*this, "RomSocketText"+selectedComputerStr_, wxStaticText)->Enable();
        XRCCTRL(*this, "FourSocketBank"+selectedComputerStr_, wxChoice)->Enable();
        XRCCTRL(*this, "RomBlockText"+selectedComputerStr_, wxStaticText)->Enable();
        XRCCTRL(*this, "RomBlock602"+selectedComputerStr_, wxTextCtrl)->Enable();
        XRCCTRL(*this, "RomAddress"+selectedComputerStr_, wxStaticText)->Enable();
        XRCCTRL(*this, "RomButtonU20"+selectedComputerStr_, wxButton)->Enable();
        XRCCTRL(*this, "MainRomU20"+selectedComputerStr_, wxComboBox)->Enable();
        XRCCTRL(*this, "RomButtonU19"+selectedComputerStr_, wxButton)->Enable();
        XRCCTRL(*this, "MainRomU19"+selectedComputerStr_, wxComboBox)->Enable();
    }
}

void MicroboardSetupDialog::onOneSocketBank(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    configuration_.microChipType_[ONE_SOCKET] = event.GetSelection();

    checkErrorStatus();
}

void MicroboardSetupDialog::onFourSocketBank(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    switch (configuration_.microboardType_[0])
    {
        case MICROBOARD_CDP18S602:
        case MICROBOARD_CDP18S605:
        case MICROBOARD_CDP18S607:
        case MICROBOARD_CDP18S610:
            onFour602SocketBank(event);
        break;
            
        case MICROBOARD_CDP18S604B:
        case MICROBOARD_CDP18S609:
            onFour604BSocketBank(event);
        break;

        default:
            configuration_.microChipType_[FOUR_SOCKET] = event.GetSelection();
    }
    checkErrorStatus();
}

void MicroboardSetupDialog::onUartGroup(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    elfConfiguration_.uartGroup = event.GetSelection() + 1;
}

void MicroboardSetupDialog::onOne600SocketSetup(wxCommandEvent&WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    One600SocketSetupDialog One600SocketSetupDialog(this, configuration_);
    One600SocketSetupDialog.ShowModal();
    
    setOneSocketState();
    checkErrorStatus();
}

void MicroboardSetupDialog::onFour600SocketSetup(wxCommandEvent&WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    Four600SocketSetupDialog Four600SocketSetupDialog(this, configuration_);
    Four600SocketSetupDialog.ShowModal();
    
    setFourSocketState();
    checkErrorStatus();
}

void MicroboardSetupDialog::onFour601SocketSetup(wxCommandEvent&WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    Four601SocketSetupDialog Four601SocketSetupDialog(this, configuration_);
    Four601SocketSetupDialog.ShowModal();

    checkErrorStatus();
}

BEGIN_EVENT_TABLE(MicroboardCardSetupDialog, wxDialog)
    EVT_BUTTON(XRCID("MicroSetupSave"), MicroboardCardSetupDialog::onSaveButton)
    EVT_CHOICE(XRCID("FdcIoGroupMicroboard"), MicroboardCardSetupDialog::onFdcIoGroupMicroboard)
    EVT_CHOICE(XRCID("UartIoGroupMicroboard"), MicroboardCardSetupDialog::onUartIoGroupMicroboard)
    EVT_CHOICE(XRCID("PrinterIoGroupMicroboard"), MicroboardCardSetupDialog::onPrinterIoGroupMicroboard)
    EVT_CHOICE(XRCID("V1870IoGroupMicroboard"), MicroboardCardSetupDialog::onV1870IoGroupMicroboard)
    EVT_CHOICE(XRCID("Pio1IoGroupMicroboard"), MicroboardCardSetupDialog::onPio1IoGroupMicroboard)
    EVT_CHOICE(XRCID("Pio2IoGroupMicroboard"), MicroboardCardSetupDialog::onPio2IoGroupMicroboard)
    EVT_CHOICE(XRCID("Socket1BankMicroboard"), MicroboardCardSetupDialog::onSocket1BankMicroboard)
    EVT_CHOICE(XRCID("Socket2BankMicroboard"), MicroboardCardSetupDialog::onSocket2BankMicroboard)
    EVT_CHOICE(XRCID("LowHighMicroboard"), MicroboardCardSetupDialog::onLowHighMicroboard)
    EVT_CHOICE(XRCID("MicroSetupVideoMode"), MicroboardCardSetupDialog::onVideoMode)

    EVT_TEXT(XRCID("RamSection1Microboard"), MicroboardCardSetupDialog::onRamSec1Text)
    EVT_COMBOBOX(XRCID("RamSection1Microboard"), MicroboardCardSetupDialog::onRamSec1Text)
    EVT_BUTTON(XRCID("RamButtonSection1Microboard"), MicroboardCardSetupDialog::onRamSec1)

    EVT_TEXT(XRCID("RomSection1Microboard"), MicroboardCardSetupDialog::onRomSec1Text)
    EVT_COMBOBOX(XRCID("RomSection1Microboard"), MicroboardCardSetupDialog::onRomSec1Text)
    EVT_BUTTON(XRCID("RomButtonSection1Microboard"), MicroboardCardSetupDialog::onRomSec1)

    EVT_TEXT(XRCID("RomSection2Microboard"), MicroboardCardSetupDialog::onRomSec2Text)
    EVT_COMBOBOX(XRCID("RomSection2Microboard"), MicroboardCardSetupDialog::onRomSec2Text)
    EVT_BUTTON(XRCID("RomButtonSection2Microboard"), MicroboardCardSetupDialog::onRomSec2)

    EVT_TEXT(XRCID("RomSection3Microboard"), MicroboardCardSetupDialog::onRomSec3Text)
    EVT_COMBOBOX(XRCID("RomSection3Microboard"), MicroboardCardSetupDialog::onRomSec3Text)
    EVT_BUTTON(XRCID("RomButtonSection3Microboard"), MicroboardCardSetupDialog::onRomSec3)

    EVT_TEXT(XRCID("RomSection4Microboard"), MicroboardCardSetupDialog::onRomSec4Text)
    EVT_COMBOBOX(XRCID("RomSection4Microboard"), MicroboardCardSetupDialog::onRomSec4Text)
    EVT_BUTTON(XRCID("RomButtonSection4Microboard"), MicroboardCardSetupDialog::onRomSec4)

    EVT_CHOICE(XRCID("RamSizeMicroboard"), MicroboardCardSetupDialog::onRamSize)
    EVT_TEXT(XRCID("RamBlockMicroboard"), MicroboardCardSetupDialog::onRamBlock)

    EVT_TEXT(XRCID("Rom1Block625Microboard"), MicroboardCardSetupDialog::onRom1Block625)
    EVT_TEXT(XRCID("Rom2Block625Microboard"), MicroboardCardSetupDialog::onRom2Block625)

    EVT_CHOICE(XRCID("MicroSetup621Location0"), MicroboardCardSetupDialog::on621Location0)
    EVT_CHOICE(XRCID("MicroSetup621Location1"), MicroboardCardSetupDialog::on621Location1)

    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeMicroboard"), MicroboardCardSetupDialog::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeMicroboard"), MicroboardCardSetupDialog::onVolume)

    EVT_TEXT(XRCID("MicroSetupCharacterRom"), MicroboardCardSetupDialog::onCharacterRomText)
    EVT_COMBOBOX(XRCID("MicroSetupCharacterRom"), MicroboardCardSetupDialog::onCharacterRomText)
    EVT_BUTTON(XRCID("MicroSetupCharacterRomButton"), MicroboardCardSetupDialog::onCharacterRom)

    EVT_TEXT(XRCID("PrintFileMicroboard"), MicroboardCardSetupDialog::onPrintFileText)
    EVT_CHOICE(XRCID("PrintModeMicroboard"), MicroboardCardSetupDialog::onPrintMode)
    EVT_BUTTON(XRCID("PrintFileButtonMicroboard"), MicroboardCardSetupDialog::onPrintFile)

    EVT_BUTTON(XRCID("Four660SocketSetupMicroboard"), MicroboardCardSetupDialog::onMicroRom660SocketSetup)

    EVT_CHECKBOX(XRCID("Pio1IoWindow"), MicroboardCardSetupDialog::onPio1IoWindow)
    EVT_CHECKBOX(XRCID("Pio2IoWindow"), MicroboardCardSetupDialog::onPio2IoWindow)

    EVT_CHOICE(XRCID("MicroSetupLocation"), MicroboardCardSetupDialog::onMicroSetupLocation0)
    EVT_CHOICE(XRCID("MicroSetup621Location0"), MicroboardCardSetupDialog::onMicroSetupLocation0)
    EVT_CHOICE(XRCID("OneSocketBankMicroboard"), MicroboardCardSetupDialog::onMicroSetupLocation0)
    EVT_CHECKBOX(XRCID("MicroSetupDisableBlock00"), MicroboardCardSetupDialog::onMicroSetupInhibit)
    EVT_CHECKBOX(XRCID("MicroSetupDisableBlock01"), MicroboardCardSetupDialog::onMicroSetupInhibit)
    EVT_CHECKBOX(XRCID("MicroSetupDisableBlock02"), MicroboardCardSetupDialog::onMicroSetupInhibit)
    EVT_CHECKBOX(XRCID("MicroSetupDisableBlock03"), MicroboardCardSetupDialog::onMicroSetupInhibit)
    EVT_CHECKBOX(XRCID("MicroSetupDisableBlock10"), MicroboardCardSetupDialog::onMicroSetupInhibit)
    EVT_CHECKBOX(XRCID("MicroSetupDisableBlock11"), MicroboardCardSetupDialog::onMicroSetupInhibit)
    EVT_CHECKBOX(XRCID("MicroSetupDisableBlock12"), MicroboardCardSetupDialog::onMicroSetupInhibit)
    EVT_CHECKBOX(XRCID("MicroSetupDisableBlock13"), MicroboardCardSetupDialog::onMicroSetupInhibit)
    EVT_CHOICE(XRCID("MemoryInhibit64Microboard"), MicroboardCardSetupDialog::onMicroSetupInhibit64)
    EVT_CHOICE(XRCID("MemoryInhibit32LowMicroboard"), MicroboardCardSetupDialog::onMicroSetupInhibit32Low)
    EVT_CHOICE(XRCID("MemoryInhibit32HighMicroboard"), MicroboardCardSetupDialog::onMicroSetupInhibit32High)
    EVT_CHECKBOX(XRCID("MemoryDisableMicroboard"), MicroboardCardSetupDialog::onMicroSetupDisableMemory)
    EVT_CHOICE(XRCID("Socket626BankMicroboard"), MicroboardCardSetupDialog::onMicroSetupSocketSize0)
    EVT_CHOICE(XRCID("FourSocketBankMicroboard"), MicroboardCardSetupDialog::onMicroSetupSocketSize1)

END_EVENT_TABLE()

MicroboardCardSetupDialog::MicroboardCardSetupDialog(wxWindow* parent, Conf configuration, ElfConfiguration elfConfiguration, MicroMemoryConf microMemConfigutation, int cardNumber)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup_" + p_Main->getFontSize() + ".xrc");
    configuration_ = configuration;
    elfConfiguration_ = elfConfiguration;
    microMemConfigutation_ = microMemConfigutation;
    cardNumber_ = cardNumber;
    selectedComputer_ = p_Main->getSelectedComputerId();
    runningComputer_ = p_Main->getRunningComputerId();
    selectedComputerName_ = p_Main->getSelectedComputerText();
    selectedComputerStr_ = p_Main->getSelectedComputerStr();
    memoryError_ = false;
    boardError_ = false;
    guiLoaded_ = false;

    romText_[0] = " block 1";
    romText_[1] = " block 2";
    romText_[2] = " block 3";
    romText_[3] = " block 4";

    switch (configuration_.microboardType_[cardNumber])
    {
        case CARD_CDP18S620:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S620_Setup_Dialog");
            XRCCTRL(*this, "RomSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC1]);
            XRCCTRL(*this, "MicroSetupLocation", wxChoice)->SetSelection(microMemConfigutation.memLocation_[0]);
        break;

        case CARD_CDP18S621:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S621_Setup_Dialog");
            XRCCTRL(*this, "RomSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC1]);
            XRCCTRL(*this, "MicroSetup621Location0", wxChoice)->SetSelection(microMemConfigutation.memLocation_[0]);
            XRCCTRL(*this, "MicroSetupDisableBlock00", wxCheckBox)->SetValue(microMemConfigutation.inhibitBlock_[0][0]);
            XRCCTRL(*this, "MicroSetupDisableBlock01", wxCheckBox)->SetValue(microMemConfigutation.inhibitBlock_[0][1]);
            XRCCTRL(*this, "MicroSetupDisableBlock02", wxCheckBox)->SetValue(microMemConfigutation.inhibitBlock_[0][2]);
            XRCCTRL(*this, "MicroSetupDisableBlock03", wxCheckBox)->SetValue(microMemConfigutation.inhibitBlock_[0][3]);
            setMemoryInhibit621(0);
        break;

        case CARD_CDP18S623A:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S623A_Setup_Dialog");
            XRCCTRL(*this, "RomSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC1]);
            XRCCTRL(*this, "MicroSetupLocation", wxChoice)->SetSelection(microMemConfigutation.memLocation_[0]);
        break;
            
        case CARD_CDP18S625:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S625_Setup_Dialog");
            XRCCTRL(*this, "RomSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC1]);
            XRCCTRL(*this, "RomSection2Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC2]);
            XRCCTRL(*this, "Rom1Block625Microboard", wxTextCtrl)->ChangeValue(microMemConfigutation.chipBlockRom_[0]);
            XRCCTRL(*this, "Rom2Block625Microboard", wxTextCtrl)->ChangeValue(microMemConfigutation.chipBlockRom_[1]);
            XRCCTRL(*this, "MicroSetupDisableBlock00", wxCheckBox)->SetValue(microMemConfigutation.inhibitBlock_[0][0]);
            XRCCTRL(*this, "MicroSetupDisableBlock01", wxCheckBox)->SetValue(microMemConfigutation.inhibitBlock_[0][1]);
            XRCCTRL(*this, "MicroSetupDisableBlock02", wxCheckBox)->SetValue(microMemConfigutation.inhibitBlock_[0][2]);
            XRCCTRL(*this, "MicroSetupDisableBlock03", wxCheckBox)->SetValue(microMemConfigutation.inhibitBlock_[0][3]);
            XRCCTRL(*this, "MicroSetupDisableBlock10", wxCheckBox)->SetValue(microMemConfigutation.inhibitBlock_[1][0]);
            XRCCTRL(*this, "MicroSetupDisableBlock11", wxCheckBox)->SetValue(microMemConfigutation.inhibitBlock_[1][1]);
            XRCCTRL(*this, "MicroSetupDisableBlock12", wxCheckBox)->SetValue(microMemConfigutation.inhibitBlock_[1][2]);
            XRCCTRL(*this, "MicroSetupDisableBlock13", wxCheckBox)->SetValue(microMemConfigutation.inhibitBlock_[1][3]);
            XRCCTRL(*this, "Socket1BankMicroboard", wxChoice)->SetSelection(microMemConfigutation.socketSize_[0]);
            XRCCTRL(*this, "Socket2BankMicroboard", wxChoice)->SetSelection(microMemConfigutation.socketSize_[1]);
            rom1SocketBankGui625();
            setMemoryInhibit625(0);
            rom2SocketBankGui625();
            setMemoryInhibit625(1);
        break;

        case CARD_CDP18S626:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S626_Setup_Dialog");
            XRCCTRL(*this, "RomSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC1]);
            XRCCTRL(*this, "RomSection2Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC2]);
            XRCCTRL(*this, "RomSection3Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC3]);
            XRCCTRL(*this, "RomSection4Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC4]);
            XRCCTRL(*this, "MemorySection1Microboard", wxChoice)->SetSelection(microMemConfigutation.memType[CDP626_SEC1]);
            XRCCTRL(*this, "MemorySection2Microboard", wxChoice)->SetSelection(microMemConfigutation.memType[CDP626_SEC2]);
            XRCCTRL(*this, "MemorySection3Microboard", wxChoice)->SetSelection(microMemConfigutation.memType[CDP626_SEC3]);
            XRCCTRL(*this, "MemorySection4Microboard", wxChoice)->SetSelection(microMemConfigutation.memType[CDP626_SEC4]);
            XRCCTRL(*this, "LowHighMicroboard", wxChoice)->SetSelection(microMemConfigutation.memLocation_[0]);
            XRCCTRL(*this, "Socket626BankMicroboard", wxChoice)->SetSelection(microMemConfigutation.socketSize_[0]);
            XRCCTRL(*this, "MemoryInhibit64Microboard", wxChoice)->SetSelection(microMemConfigutation.inhibit64_);
            XRCCTRL(*this, "MemoryInhibit32LowMicroboard", wxChoice)->SetSelection(microMemConfigutation.inhibit32Low_);
            XRCCTRL(*this, "MemoryInhibit32HighMicroboard", wxChoice)->SetSelection(microMemConfigutation.inhibit32High_);
            setInhibit();
        break;

        case CARD_CDP18S627:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S627_Setup_Dialog");
            XRCCTRL(*this, "RomSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC1]);
            XRCCTRL(*this, "MicroSetupLocation", wxChoice)->SetSelection(microMemConfigutation.memLocation_[0]);
        break;

        case CARD_CDP18S628:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S628_Setup_Dialog");
            XRCCTRL(*this, "RomSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC1]);
            XRCCTRL(*this, "RomSection2Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC2]);
            XRCCTRL(*this, "RomSection3Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC3]);
            XRCCTRL(*this, "RomSection4Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC4]);
            XRCCTRL(*this, "MemorySection1Microboard", wxChoice)->SetSelection(microMemConfigutation.memType[CDP626_SEC1]);
            XRCCTRL(*this, "MemorySection2Microboard", wxChoice)->SetSelection(microMemConfigutation.memType[CDP626_SEC2]);
            XRCCTRL(*this, "MemorySection3Microboard", wxChoice)->SetSelection(microMemConfigutation.memType[CDP626_SEC3]);
            XRCCTRL(*this, "MemorySection4Microboard", wxChoice)->SetSelection(microMemConfigutation.memType[CDP626_SEC4]);
            XRCCTRL(*this, "LowHighMicroboard", wxChoice)->SetSelection(microMemConfigutation.memLocation_[0]);
            XRCCTRL(*this, "MemoryInhibit32LowMicroboard", wxChoice)->SetSelection(microMemConfigutation.inhibit32Low_);
            XRCCTRL(*this, "MemoryInhibit32HighMicroboard", wxChoice)->SetSelection(microMemConfigutation.inhibit32High_);
            setInhibit();
        break;

        case CARD_CDP18S629:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S629_Setup_Dialog");
            XRCCTRL(*this, "RomSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC1]);
            XRCCTRL(*this, "RomSection2Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC2]);
            XRCCTRL(*this, "RomSection3Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC3]);
            XRCCTRL(*this, "RomSection4Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC4]);
            XRCCTRL(*this, "MemorySection1Microboard", wxChoice)->SetSelection(microMemConfigutation.memType[CDP626_SEC1]);
            XRCCTRL(*this, "MemorySection2Microboard", wxChoice)->SetSelection(microMemConfigutation.memType[CDP626_SEC2]);
            XRCCTRL(*this, "MemorySection3Microboard", wxChoice)->SetSelection(microMemConfigutation.memType[CDP626_SEC3]);
            XRCCTRL(*this, "MemorySection4Microboard", wxChoice)->SetSelection(microMemConfigutation.memType[CDP626_SEC4]);
            XRCCTRL(*this, "LowHighMicroboard", wxChoice)->SetSelection(microMemConfigutation.memLocation_[0]);
            XRCCTRL(*this, "MemoryInhibit32LowMicroboard", wxChoice)->SetSelection(microMemConfigutation.inhibit32Low_);
            XRCCTRL(*this, "MemoryInhibit32HighMicroboard", wxChoice)->SetSelection(microMemConfigutation.inhibit32High_);
            setInhibit();
        break;

        case CARD_CDP18S640:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S640A_Setup_Dialog");
            XRCCTRL(*this, "RomSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC1]);
            XRCCTRL(*this, "RomSection2Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC2]);
        break;
            
        case CARD_CDP18S641:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S641_Setup_Dialog");
            if (elfConfiguration_.uartGroup >= 1 &&  elfConfiguration_.uartGroup <= 8)
            {
                switch (elfConfiguration_.uartGroup)
                {
                    case 1:
                        XRCCTRL(*this, "UartIoGroup" + selectedComputerStr_, wxChoice)->SetSelection(0);
                    break;
                    case 2:
                        XRCCTRL(*this, "UartIoGroup" + selectedComputerStr_, wxChoice)->SetSelection(1);
                    break;
                    case 4:
                        XRCCTRL(*this, "UartIoGroup" + selectedComputerStr_, wxChoice)->SetSelection(2);
                    break;
                    case 8:
                        XRCCTRL(*this, "UartIoGroup" + selectedComputerStr_, wxChoice)->SetSelection(3);
                    break;
                }
            }
            else
            {
                XRCCTRL(*this, "UartIoGroup" + selectedComputerStr_, wxChoice)->SetSelection((elfConfiguration_.uartGroup >> 4) + 4);
            }
            if (elfConfiguration_.ioConfiguration.uartOut < 2)
                elfConfiguration_.ioConfiguration.uartOut = 2;
            if (elfConfiguration_.ioConfiguration.uartControl < 2)
                elfConfiguration_.ioConfiguration.uartControl = 3;

            XRCCTRL(*this, "UartDataIo" + selectedComputerStr_, wxChoice)->SetSelection(elfConfiguration_.ioConfiguration.uartOut - 2);
            XRCCTRL(*this, "UartStatusControl" + selectedComputerStr_, wxChoice)->SetSelection(elfConfiguration_.ioConfiguration.uartControl - 2);
        break;
            
        case CARD_CDP18S646:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S646_Setup_Dialog");
            if (elfConfiguration_.printerGroup >= 1 && elfConfiguration_.printerGroup <= 8)
            {
                switch (elfConfiguration_.printerGroup)
                {
                    case 1:
                        XRCCTRL(*this, "PrinterIoGroup" + selectedComputerStr_, wxChoice)->SetSelection(0);
                        break;
                    case 2:
                        XRCCTRL(*this, "PrinterIoGroup" + selectedComputerStr_, wxChoice)->SetSelection(1);
                        break;
                    case 4:
                        XRCCTRL(*this, "PrinterIoGroup" + selectedComputerStr_, wxChoice)->SetSelection(2);
                        break;
                    case 8:
                        XRCCTRL(*this, "PrinterIoGroup" + selectedComputerStr_, wxChoice)->SetSelection(3);
                        break;
                }
            }
            else
            {
                XRCCTRL(*this, "PrinterIoGroup" + selectedComputerStr_, wxChoice)->SetSelection((elfConfiguration_.printerGroup >> 4) + 4);
            }
            XRCCTRL(*this, "PrintFile"+selectedComputerStr_, wxTextCtrl)->SetValue(configuration_.printFile_);
            XRCCTRL(*this, "PrintMode"+selectedComputerStr_, wxChoice)->SetSelection(configuration_.printMode_);
            setPrintMode();
        break;

        case CARD_CDP18S651:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S651_Setup_Dialog");
            if (elfConfiguration_.upd765Group >= 1 && elfConfiguration_.upd765Group <= 8)
            {
                switch (elfConfiguration_.upd765Group)
                {
                    case 1:
                        XRCCTRL(*this, "FdcIoGroup" + selectedComputerStr_, wxChoice)->SetSelection(0);
                    break;
                    case 2:
                        XRCCTRL(*this, "FdcIoGroup" + selectedComputerStr_, wxChoice)->SetSelection(1);
                    break;
                    case 4:
                        XRCCTRL(*this, "FdcIoGroup" + selectedComputerStr_, wxChoice)->SetSelection(2);
                    break;
                    case 8:
                        XRCCTRL(*this, "FdcIoGroup" + selectedComputerStr_, wxChoice)->SetSelection(3);
                    break;
                }
            }
            else
            {
                XRCCTRL(*this, "FdcIoGroup" + selectedComputerStr_, wxChoice)->SetSelection((elfConfiguration_.upd765Group >> 4) + 4);
            }
        break;

        case CARD_CDP18S652:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S652_Setup_Dialog");
            XRCCTRL(*this, "RamSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.ram_);
            XRCCTRL(*this, "RamSizeMicroboard", wxChoice)->SetSelection(microMemConfigutation.socketSize_[0]);

            if (microMemConfigutation_.socketSize_[0] == 0)
                XRCCTRL(*this, "RamAddress"+selectedComputerStr_, wxStaticText)->SetLabel("address: 8C00-8FFF");
            else
                XRCCTRL(*this, "RamAddress"+selectedComputerStr_, wxStaticText)->SetLabel("address: 8800-8FFF");

            XRCCTRL(*this, "RomSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC1]);
            XRCCTRL(*this, "RomSection2Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC2]);
            XRCCTRL(*this, "RomSection3Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC3]);
            XRCCTRL(*this, "RomSection4Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC4]);

            XRCCTRL(*this, "MemoryDisableMicroboard", wxCheckBox)->SetValue(microMemConfigutation.disableCardMemory_);
        break;

        case CARD_CDP18S660:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S660_Setup_Dialog");
            XRCCTRL(*this, "Pio1IoGroupMicroboard", wxChoice)->SetSelection((elfConfiguration_.cdp18s660Group1 >> 4) - 1);
            XRCCTRL(*this, "Pio2IoGroupMicroboard", wxChoice)->SetSelection((elfConfiguration_.cdp18s660Group2 >> 4) - 1);

            XRCCTRL(*this, "RamSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.ram_);
            XRCCTRL(*this, "RomSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC1]);
            XRCCTRL(*this, "RomSection2Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC2]);
            XRCCTRL(*this, "RomSection3Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC3]);
            XRCCTRL(*this, "RomSection4Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[CDP626_SEC4]);

            XRCCTRL(*this, "OneSocketBankMicroboard", wxChoice)->SetSelection(microMemConfigutation.memLocation_[ONE_SOCKET]);
            XRCCTRL(*this, "FourSocketBankMicroboard", wxChoice)->SetSelection(microMemConfigutation.socketSize_[FOUR_SOCKET]);

            XRCCTRL(*this, "Pio1IoWindow", wxCheckBox)->SetValue(elfConfiguration_.usePioWindow1Cdp18s660);
            XRCCTRL(*this, "Pio2IoWindow", wxCheckBox)->SetValue(elfConfiguration_.usePioWindow2Cdp18s660);

            romText_[0] = " ROM XU23";
            romText_[1] = " ROM XU22";
            romText_[2] = " ROM XU21";
            romText_[3] = " ROM XU20";
        break;

        case CARD_CDP18S661B:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S661B_Setup_Dialog");
            XRCCTRL(*this, "V1870IoGroupMicroboard", wxChoice)->SetSelection((elfConfiguration_.ioConfiguration.v1870ioGroup >> 4) - 8);
            XRCCTRL(*this, "VolumeMicroboard", wxSlider)->SetValue(configuration_.volume_);

            XRCCTRL(*this, "MicroSetupPageMemory", wxChoice)->SetSelection(elfConfiguration_.ioConfiguration.v1870pageMemSize);
            XRCCTRL(*this, "MicroSetupVideoMode", wxChoice)->SetSelection(elfConfiguration_.ioConfiguration.v1870videoMode-1);
            XRCCTRL(*this, "MicroSetupCharacterRom", wxComboBox)->SetValue(configuration_.charRom_);
            setVideoMode();
            XRCCTRL(*this, "MicroSetupInterruptMode", wxChoice)->SetSelection(elfConfiguration_.ioConfiguration.v1870interruptMode);
            XRCCTRL(*this, "MicroSetupKeyEf", wxChoice)->SetSelection(elfConfiguration_.ioConfiguration.keyboardEf-2);
            XRCCTRL(*this, "MicroSetupKeyType", wxChoice)->SetSelection(elfConfiguration_.keyboardType);
        break;

        case CARD_CDP18S661V1:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S661V1_Setup_Dialog");
            XRCCTRL(*this, "V1870IoGroupMicroboard", wxChoice)->SetSelection((elfConfiguration_.ioConfiguration.v1870ioGroup >> 4) - 8);
            XRCCTRL(*this, "VolumeMicroboard", wxSlider)->SetValue(configuration_.volume_);
            XRCCTRL(*this, "MicroSetupVideoPalMode", wxChoice)->SetSelection(elfConfiguration_.ioConfiguration.v1870videoMode-1);
            XRCCTRL(*this, "MicroSetupInterruptMode", wxChoice)->SetSelection(elfConfiguration_.ioConfiguration.v1870interruptMode);
            XRCCTRL(*this, "MicroSetupKeyEf", wxChoice)->SetSelection(elfConfiguration_.ioConfiguration.keyboardEf-2);
            XRCCTRL(*this, "MicroSetupKeyType", wxChoice)->SetSelection(elfConfiguration_.keyboardType);
        break;
            
        case CARD_CDP18S661V3:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S661V3_Setup_Dialog");
            XRCCTRL(*this, "V1870IoGroupMicroboard", wxChoice)->SetSelection((elfConfiguration_.ioConfiguration.v1870ioGroup >> 4) - 8);
            XRCCTRL(*this, "VolumeMicroboard", wxSlider)->SetValue(configuration_.volume_);
            XRCCTRL(*this, "MicroSetupVideoPalMode", wxChoice)->SetSelection(elfConfiguration_.ioConfiguration.v1870videoMode-1);
            XRCCTRL(*this, "MicroSetupInterruptMode", wxChoice)->SetSelection(elfConfiguration_.ioConfiguration.v1870interruptMode);
            XRCCTRL(*this, "MicroSetupKeyEf", wxChoice)->SetSelection(elfConfiguration_.ioConfiguration.keyboardEf-2);
            XRCCTRL(*this, "MicroSetupKeyType", wxChoice)->SetSelection(elfConfiguration_.keyboardType);
        break;
    }
    guiLoaded_ = true;

    updateErrorStatus();

    if (p_Main->isComputerRunning())
        XRCCTRL(*this, "MicroSetupSave", wxButton)->Disable();
}

void MicroboardCardSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    switch (configuration_.microboardType_[cardNumber_])
    {
        case CARD_CDP18S626:
        case CARD_CDP18S628:
        case CARD_CDP18S629:
            microMemConfigutation_.memType[CDP626_SEC1] = XRCCTRL(*this, "MemorySection1Microboard", wxChoice)->GetSelection();
            microMemConfigutation_.memType[CDP626_SEC2] = XRCCTRL(*this, "MemorySection2Microboard", wxChoice)->GetSelection();
            microMemConfigutation_.memType[CDP626_SEC3] = XRCCTRL(*this, "MemorySection3Microboard", wxChoice)->GetSelection();
            microMemConfigutation_.memType[CDP626_SEC4] = XRCCTRL(*this, "MemorySection4Microboard", wxChoice)->GetSelection();
        break;

        case CARD_CDP18S641:
            elfConfiguration_.ioConfiguration.uartOut = XRCCTRL(*this, "UartDataIoMicroboard", wxChoice)->GetSelection() + 2;
            elfConfiguration_.ioConfiguration.uartControl = XRCCTRL(*this, "UartStatusControlMicroboard", wxChoice)->GetSelection() + 2;
        break;

        case CARD_CDP18S661B:
            elfConfiguration_.ioConfiguration.v1870pageMemSize = XRCCTRL(*this, "MicroSetupPageMemory", wxChoice)->GetSelection();
            elfConfiguration_.ioConfiguration.v1870interruptMode = XRCCTRL(*this, "MicroSetupInterruptMode", wxChoice)->GetSelection();
            elfConfiguration_.ioConfiguration.keyboardEf = XRCCTRL(*this, "MicroSetupKeyEf", wxChoice)->GetSelection() + 2;
            elfConfiguration_.keyboardType = XRCCTRL(*this, "MicroSetupKeyType", wxChoice)->GetSelection();
        break;

        case CARD_CDP18S661V1:
        case CARD_CDP18S661V3:
            elfConfiguration_.ioConfiguration.v1870interruptMode = XRCCTRL(*this, "MicroSetupInterruptMode", wxChoice)->GetSelection();
            elfConfiguration_.ioConfiguration.v1870videoMode = XRCCTRL(*this, "MicroSetupVideoPalMode", wxChoice)->GetSelection()+1;
            elfConfiguration_.ioConfiguration.keyboardEf = XRCCTRL(*this, "MicroSetupKeyEf", wxChoice)->GetSelection() + 2;
            elfConfiguration_.keyboardType = XRCCTRL(*this, "MicroSetupKeyType", wxChoice)->GetSelection();
        break;
    }

    p_Main->setConfiguration(selectedComputer_, configuration_);
    p_Main->setElfConfiguration(selectedComputer_, elfConfiguration_);
    p_Main->setMicroMemConfiguration(cardNumber_, microMemConfigutation_);

    EndModal(wxID_OK);
}

void MicroboardCardSetupDialog::checkErrorStatus()
{
    p_Main->checkAllBoardTypes(&configuration_, &elfConfiguration_, microMemConfigutation_, cardNumber_);
    updateErrorStatus();
}
    
void MicroboardCardSetupDialog::updateErrorStatus()
{
    bool refresh = false;
    wxColour colour = XRCCTRL(*this, "MicroSetupSave", wxButton)->GetForegroundColour();

    if (configuration_.errorMemoryOverlapp_[cardNumber_] != "")
    {
        XRCCTRL(*this, "MemoryErrorMicroboard", wxStaticText)->SetLabel(configuration_.errorMemoryOverlapp_[cardNumber_]);
        XRCCTRL(*this, "MemoryErrorMicroboard", wxStaticText)->Show();
         if (configuration_.errorMemoryOverlapp_[cardNumber_].Right(4) == "card")
            XRCCTRL(*this, "MemoryErrorMicroboard", wxStaticText)->SetForegroundColour(colour);
        else
            XRCCTRL(*this, "MemoryErrorMicroboard", wxStaticText)->SetForegroundColour(p_Main->getGuiTextColour(GUI_COL_RED));

        if (!memoryError_)
            refresh = true;
        memoryError_ = true;
    }
    else
    {
        XRCCTRL(*this, "MemoryErrorMicroboard", wxStaticText)->Hide();
        if (memoryError_)
            refresh = true;
        memoryError_ = false;
    }
    
    if (configuration_.errorDoubleBoard_[cardNumber_] != "")
    {
        XRCCTRL(*this, "DoubleBoardErrorMicroboard", wxStaticText)->SetLabel(configuration_.errorDoubleBoard_[cardNumber_]);
        XRCCTRL(*this, "DoubleBoardErrorMicroboard", wxStaticText)->Show();
        if (!boardError_)
            refresh = true;
        boardError_ = true;
    }
    else
    {
        XRCCTRL(*this, "DoubleBoardErrorMicroboard", wxStaticText)->Hide();
        if (boardError_)
            refresh = true;
        boardError_ = false;
    }
    
    if (refresh)
        this->DoLayoutAdaptation();
}

void MicroboardCardSetupDialog::onMicroSetupLocation0(wxCommandEvent& event)
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.memLocation_[0] = event.GetSelection();
    checkErrorStatus();
}

void MicroboardCardSetupDialog::onMicroSetupInhibit(wxCommandEvent& event)
{
    if (!guiLoaded_)
        return;
    
    long id;
    wxString idReference, buttonNumber;

    idReference = wxWindow::FindWindowById(event.GetId())->GetName();
    buttonNumber = idReference.Right(2);
    if (!buttonNumber.ToLong(&id, 16))
        return;

    microMemConfigutation_.inhibitBlock_[(id&0xf0)>>4][id&0xf] = event.IsChecked();
    checkErrorStatus();
}

void MicroboardCardSetupDialog::onMicroSetupInhibit64(wxCommandEvent& event)
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.inhibit64_ = event.GetSelection();
    checkErrorStatus();
}

void MicroboardCardSetupDialog::onMicroSetupInhibit32Low(wxCommandEvent& event)
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.inhibit32Low_ = event.GetSelection();
    checkErrorStatus();
}

void MicroboardCardSetupDialog::onMicroSetupInhibit32High(wxCommandEvent& event)
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.inhibit32High_ = event.GetSelection();
    checkErrorStatus();
}

void MicroboardCardSetupDialog::onMicroSetupDisableMemory(wxCommandEvent& event)
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.disableCardMemory_ = event.IsChecked();
    checkErrorStatus();
}

void MicroboardCardSetupDialog::onMicroSetupSocketSize0(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.socketSize_[0] = event.GetSelection();
    setInhibit();

    checkErrorStatus();
}

void MicroboardCardSetupDialog::onMicroSetupSocketSize1(wxCommandEvent& event)
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.socketSize_[1] = event.GetSelection();
    checkErrorStatus();
}

void MicroboardCardSetupDialog::onFdcIoGroupMicroboard(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    int selection = event.GetSelection();
    
    if (selection < 4)
        elfConfiguration_.upd765Group = 1 << selection;
    else
        elfConfiguration_.upd765Group = (selection - 4) << 4;
}

void MicroboardCardSetupDialog::onUartIoGroupMicroboard(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    int selection = event.GetSelection();
    
    if (selection < 4)
        elfConfiguration_.uartGroup = 1 << selection;
    else
        elfConfiguration_.uartGroup = (selection - 4) << 4;
}

void MicroboardCardSetupDialog::onPrinterIoGroupMicroboard(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    int selection = event.GetSelection();
    
    if (selection < 4)
        elfConfiguration_.printerGroup = 1 << selection;
    else
        elfConfiguration_.printerGroup = (selection - 4) << 4;
}

void MicroboardCardSetupDialog::onV1870IoGroupMicroboard(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    int selection = event.GetSelection();
    
    elfConfiguration_.ioConfiguration.v1870ioGroup = (selection + 8) << 4;
}

void MicroboardCardSetupDialog::onPio1IoGroupMicroboard(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    int selection = event.GetSelection();
    
    elfConfiguration_.cdp18s660Group1 = (selection + 1) << 4;
}

void MicroboardCardSetupDialog::onPio2IoGroupMicroboard(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    int selection = event.GetSelection();
    
    elfConfiguration_.cdp18s660Group2 = (selection + 1) << 4;
}

void MicroboardCardSetupDialog::setInhibit()
{
    if (microMemConfigutation_.socketSize_[0] == 1)
    {
        microMemConfigutation_.memLocation_[0] = 0;
        XRCCTRL(*this, "LowHighMicroboard", wxChoice)->SetSelection(microMemConfigutation_.memLocation_[0]);
        XRCCTRL(*this, "LowHighMicroboard", wxChoice)->Disable();
        XRCCTRL(*this, "MemoryInhibit64Microboard", wxChoice)->Enable();
        XRCCTRL(*this, "MemoryInhibit32LowMicroboard", wxChoice)->Disable();
        XRCCTRL(*this, "MemoryInhibit32HighMicroboard", wxChoice)->Disable();
        XRCCTRL(*this, "MemorySection1Microboard", wxChoice)->Disable();
        XRCCTRL(*this, "MemorySection1Microboard", wxChoice)->SetSelection(1);
        XRCCTRL(*this, "MemorySection2Microboard", wxChoice)->Disable();
        XRCCTRL(*this, "MemorySection2Microboard", wxChoice)->SetSelection(1);
        XRCCTRL(*this, "MemorySection3Microboard", wxChoice)->Disable();
        XRCCTRL(*this, "MemorySection3Microboard", wxChoice)->SetSelection(1);
        XRCCTRL(*this, "MemorySection4Microboard", wxChoice)->Disable();
        XRCCTRL(*this, "MemorySection4Microboard", wxChoice)->SetSelection(1);
    }
    else
    {
        XRCCTRL(*this, "MemorySection1Microboard", wxChoice)->Enable();
        XRCCTRL(*this, "MemorySection2Microboard", wxChoice)->Enable();
        XRCCTRL(*this, "MemorySection3Microboard", wxChoice)->Enable();
        XRCCTRL(*this, "MemorySection4Microboard", wxChoice)->Enable();
        XRCCTRL(*this, "LowHighMicroboard", wxChoice)->Enable();
        XRCCTRL(*this, "MemoryInhibit64Microboard", wxChoice)->Disable();
        if (microMemConfigutation_.memLocation_[0] == 0)
        {
            XRCCTRL(*this, "MemoryInhibit32LowMicroboard", wxChoice)->Enable();
            XRCCTRL(*this, "MemoryInhibit32HighMicroboard", wxChoice)->Disable();
        }
        else
        {
            XRCCTRL(*this, "MemoryInhibit32LowMicroboard", wxChoice)->Disable();
            XRCCTRL(*this, "MemoryInhibit32HighMicroboard", wxChoice)->Enable();
        }
    }
}

void MicroboardCardSetupDialog::onLowHighMicroboard(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.memLocation_[0] = event.GetSelection();
    
    if (microMemConfigutation_.memLocation_[0] == 0)
    {
        XRCCTRL(*this, "MemoryInhibit32LowMicroboard", wxChoice)->Enable();
        XRCCTRL(*this, "MemoryInhibit32HighMicroboard", wxChoice)->Disable();
    }
    else
    {
        XRCCTRL(*this, "MemoryInhibit32LowMicroboard", wxChoice)->Disable();
        XRCCTRL(*this, "MemoryInhibit32HighMicroboard", wxChoice)->Enable();
    }
    checkErrorStatus();
}

void MicroboardCardSetupDialog::onRamSec1(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+"RAM file to load",
                              microMemConfigutation_.romDir_[CDP626_SEC1], XRCCTRL(*this, "RamSection1"+selectedComputerStr_, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.arc;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    microMemConfigutation_.ramDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    microMemConfigutation_.ram_ = FullPath.GetFullName();
    
    XRCCTRL(*this, "RamSection1"+selectedComputerStr_, wxComboBox)->SetValue(microMemConfigutation_.ram_);
}

void MicroboardCardSetupDialog::onRamSec1Text(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.ram_ = XRCCTRL(*this, "RamSection1"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardCardSetupDialog::onRomSec1(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+romText_[0]+" file to load",
                              microMemConfigutation_.romDir_[CDP626_SEC1], XRCCTRL(*this, "RomSection1"+selectedComputerStr_, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.arc;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    microMemConfigutation_.romDir_[CDP626_SEC1] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    microMemConfigutation_.rom_[CDP626_SEC1] = FullPath.GetFullName();
    
    XRCCTRL(*this, "RomSection1"+selectedComputerStr_, wxComboBox)->SetValue(microMemConfigutation_.rom_[CDP626_SEC1]);
}

void MicroboardCardSetupDialog::onCharacterRomText(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    configuration_.charRom_ = XRCCTRL(*this, "MicroSetupCharacterRom", wxComboBox)->GetValue();
}

void MicroboardCardSetupDialog::onCharacterRom(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    wxString fileName;
    
    fileName = wxFileSelector( "Select the CDP18S661 video character ROM file to load",
                              configuration_.charRomDir_, XRCCTRL(*this, "MicroSetupCharacterRom", wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.arc;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    configuration_.charRomDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    configuration_.charRom_ = FullPath.GetFullName();
    
    XRCCTRL(*this, "MicroSetupCharacterRom", wxComboBox)->SetValue(configuration_.charRom_);
}

void MicroboardCardSetupDialog::onVideoMode(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    elfConfiguration_.ioConfiguration.v1870videoMode = event.GetSelection() + 1;
    setVideoMode();
}

void MicroboardCardSetupDialog::setVideoMode()
{
    switch (elfConfiguration_.ioConfiguration.v1870videoMode)
    {
        case 1:
        case 2:
        case 3:
            XRCCTRL(*this, "MicroSetupCharacterRomButton", wxButton)->Disable();
            XRCCTRL(*this, "MicroSetupCharacterRom", wxComboBox)->Disable();
        break;

        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            XRCCTRL(*this, "MicroSetupCharacterRomButton", wxButton)->Enable();
            XRCCTRL(*this, "MicroSetupCharacterRom", wxComboBox)->Enable();
        break;
    }
}


void MicroboardCardSetupDialog::onRomSec1Text(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.rom_[CDP626_SEC1] = XRCCTRL(*this, "RomSection1"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardCardSetupDialog::onRomSec2(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+romText_[1]+" file to load",
                              microMemConfigutation_.romDir_[CDP626_SEC2], XRCCTRL(*this, "RomSection2"+selectedComputerStr_, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.arc;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    microMemConfigutation_.romDir_[CDP626_SEC2] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    microMemConfigutation_.rom_[CDP626_SEC2] = FullPath.GetFullName();
    
    XRCCTRL(*this, "RomSection2"+selectedComputerStr_, wxComboBox)->SetValue(microMemConfigutation_.rom_[CDP626_SEC2]);
}

void MicroboardCardSetupDialog::onRomSec2Text(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.rom_[CDP626_SEC2] = XRCCTRL(*this, "RomSection2"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardCardSetupDialog::onRomSec3(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+romText_[2]+" file to load",
                              microMemConfigutation_.romDir_[CDP626_SEC3], XRCCTRL(*this, "RomSection3"+selectedComputerStr_, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.arc;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    microMemConfigutation_.romDir_[CDP626_SEC3] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    microMemConfigutation_.rom_[CDP626_SEC3] = FullPath.GetFullName();
    
    XRCCTRL(*this, "RomSection3"+selectedComputerStr_, wxComboBox)->SetValue(microMemConfigutation_.rom_[CDP626_SEC3]);
}

void MicroboardCardSetupDialog::onRomSec3Text(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.rom_[CDP626_SEC3] = XRCCTRL(*this, "RomSection3"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardCardSetupDialog::onRomSec4(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    wxString fileName;

    fileName = wxFileSelector( "Select the "+selectedComputerName_+romText_[3]+" file to load",
                              microMemConfigutation_.romDir_[CDP626_SEC4], XRCCTRL(*this, "RomSection4"+selectedComputerStr_, wxComboBox)->GetValue(),
                              "",
                              wxString::Format
                              (
                               "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.arc;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    microMemConfigutation_.romDir_[CDP626_SEC4] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    microMemConfigutation_.rom_[CDP626_SEC4] = FullPath.GetFullName();
    
    XRCCTRL(*this, "RomSection4"+selectedComputerStr_, wxComboBox)->SetValue(microMemConfigutation_.rom_[CDP626_SEC4]);
}

void MicroboardCardSetupDialog::onRomSec4Text(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.rom_[CDP626_SEC4] = XRCCTRL(*this, "RomSection4"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardCardSetupDialog::onRamSize(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.socketSize_[0] = event.GetSelection();
    
    if (microMemConfigutation_.socketSize_[0] == 0)
        XRCCTRL(*this, "RamAddress"+selectedComputerStr_, wxStaticText)->SetLabel("address: 8C00-8FFF");
    else
        XRCCTRL(*this, "RamAddress"+selectedComputerStr_, wxStaticText)->SetLabel("address: 8800-8FFF");

    checkErrorStatus();
}

void MicroboardCardSetupDialog::onRamBlock(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    wxString stringBlock = event.GetString();
    if (stringBlock == "")  stringBlock = "0";
    long block;
    if (!stringBlock.ToLong(&block, 10))
        return;
    
    if (block > 63)
    {
        (void)wxMessageBox( "Invalid block number\n",
                           "Emma 02", wxICON_ERROR | wxOK );
        return;
    }
    
    microMemConfigutation_.chipBlockRam_ = stringBlock;
    
    ramSocketBankGui();
    
    checkErrorStatus();
}

void MicroboardCardSetupDialog::ramSocketBankGui()
{
    long block;
    microMemConfigutation_.chipBlockRam_.ToLong(&block);

    wxString label;
    label.Printf("address: %04X-%04X", (int)(block*0x400), (int)(block*0x400+0x3ff));
    XRCCTRL(*this, "RamAddress"+selectedComputerStr_, wxStaticText)->SetLabel(label);
}

void MicroboardCardSetupDialog::on621Location0(wxCommandEvent& event)
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.memLocation_[0] = event.GetSelection();
    setMemoryInhibit621(0);

    checkErrorStatus();
}

void MicroboardCardSetupDialog::on621Location1(wxCommandEvent& event)
{
    if (!guiLoaded_)
        return;
    
    microMemConfigutation_.memLocation_[1] = event.GetSelection();
    setMemoryInhibit621(1);

    checkErrorStatus();
}

void MicroboardCardSetupDialog::setMemoryInhibit621(int bank)
{
    Word socketSize = (1 << (microMemConfigutation_.socketSize_[bank]))*0x400;
    Word startAddress = microMemConfigutation_.memLocation_[bank] * socketSize * 4;
    
    wxString label, checkBox;
    
    for (int i=0; i<4; i++)
    {
        label.Printf("%04X-%04X", startAddress&0xffff, (startAddress+socketSize-1)&0xffff);
        checkBox.Printf("MicroSetupDisableBlock%d%d", bank, i);
        XRCCTRL(*this, checkBox, wxCheckBox)->SetLabel(label);
        startAddress += socketSize;
    }
}

void MicroboardCardSetupDialog::onSocket1BankMicroboard(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    wxString blockStr = XRCCTRL(*this, "Rom1Block625"+selectedComputerStr_, wxTextCtrl)->GetValue();
    
    long block;
    blockStr.ToLong(&block);
    
    int oldType = 1 << (microMemConfigutation_.socketSize_[0]);
    microMemConfigutation_.socketSize_[0] = event.GetSelection();
    int newType = 1 << (microMemConfigutation_.socketSize_[0]);

    double multiplier = (double) (oldType) / (double) (newType);
    block = block * multiplier;

    blockStr.Printf("%d", (int)block);
    XRCCTRL(*this, "Rom1Block625"+selectedComputerStr_, wxTextCtrl)->SetValue(blockStr);

    rom1SocketBankGui625();
    setMemoryInhibit625(0);

    checkErrorStatus();
}

void MicroboardCardSetupDialog::onSocket2BankMicroboard(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    wxString blockStr = XRCCTRL(*this, "Rom2Block625"+selectedComputerStr_, wxTextCtrl)->GetValue();
    
    long block;
    blockStr.ToLong(&block);
    
    int oldType = 1 << (microMemConfigutation_.socketSize_[1]);
    microMemConfigutation_.socketSize_[1] = event.GetSelection();
    int newType = 1 << (microMemConfigutation_.socketSize_[1]);
    
    double multiplier = (double) (oldType) / (double) (newType);
    block = block * multiplier;
    
    blockStr.Printf("%d", (int)block);
    XRCCTRL(*this, "Rom2Block625"+selectedComputerStr_, wxTextCtrl)->SetValue(blockStr);

    rom2SocketBankGui625();
    setMemoryInhibit625(1);

    checkErrorStatus();
}

void MicroboardCardSetupDialog::onRom1Block625(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    wxString stringBlock = event.GetString();
    if (stringBlock == "")  stringBlock = "0";
    long block;
    if (!stringBlock.ToLong(&block, 10))
        return;
    
    int maxBlock = (16/(1 << microMemConfigutation_.socketSize_[0]))-1;
    if (block > maxBlock)
    {
        (void)wxMessageBox( "Invalid block number\n",
                           "Emma 02", wxICON_ERROR | wxOK );
        return;
    }
    
    microMemConfigutation_.chipBlockRom_[0] = stringBlock;
    
    rom1SocketBankGui625();
    setMemoryInhibit625(0);

    checkErrorStatus();
}

void MicroboardCardSetupDialog::rom1SocketBankGui625()
{
    long block;
    microMemConfigutation_.chipBlockRom_[0].ToLong(&block);

    Word socketSize = (1 << (microMemConfigutation_.socketSize_[0]))*0x400;

    wxString label;
    label.Printf("address: %04X-%04X", (int)(block*socketSize*4), (int)(block*socketSize*4+(socketSize*4)-1));
    XRCCTRL(*this, "Rom1Address"+selectedComputerStr_, wxStaticText)->SetLabel(label);
}

void MicroboardCardSetupDialog::onRom2Block625(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    wxString stringBlock = event.GetString();
    if (stringBlock == "")  stringBlock = "0";
    long block;
    if (!stringBlock.ToLong(&block, 10))
        return;
    
    int maxBlock = (16/(1 << microMemConfigutation_.socketSize_[1]))-1;
    if (block > maxBlock)
    {
        (void)wxMessageBox( "Invalid block number\n",
                           "Emma 02", wxICON_ERROR | wxOK );
        return;
    }
    
    microMemConfigutation_.chipBlockRom_[1] = stringBlock;
    
    rom2SocketBankGui625();
    setMemoryInhibit625(1);

    checkErrorStatus();
}

void MicroboardCardSetupDialog::rom2SocketBankGui625()
{
    long block;
    microMemConfigutation_.chipBlockRom_[1].ToLong(&block);

    Word socketSize = (1 << (microMemConfigutation_.socketSize_[1]))*0x400;

    wxString label;
    label.Printf("address: %04X-%04X", (int)(block*socketSize*4), (int)(block*socketSize*4+(socketSize*4)-1));
    XRCCTRL(*this, "Rom2Address"+selectedComputerStr_, wxStaticText)->SetLabel(label);
}

void MicroboardCardSetupDialog::setMemoryInhibit625(int bank)
{
    wxString label, checkBox, staticText;

    int maxBlock = (16/(1 << microMemConfigutation_.socketSize_[bank]))-1;
    label.Printf("Block (0-%d)", (int)maxBlock);
    
    staticText.Printf("Rom%dBlockTextMicroboard", bank+1);
    XRCCTRL(*this, staticText, wxStaticText)->SetLabel(label);

    long block;
    microMemConfigutation_.chipBlockRom_[bank].ToLong(&block);

    Word socketSize = (1 << (microMemConfigutation_.socketSize_[bank]))*0x400;
    Word startAddress = block * socketSize * 4;
    
    for (int i=0; i<4; i++)
    {
        label.Printf("%04X-%04X", startAddress&0xffff, (startAddress+socketSize-1)&0xffff);
        checkBox.Printf("MicroSetupDisableBlock%d%d", bank, i);
        XRCCTRL(*this, checkBox, wxCheckBox)->SetLabel(label);
        startAddress += socketSize;
    }
}

void MicroboardCardSetupDialog::onVolume(wxScrollEvent&event)
{
    configuration_.volume_ = event.GetPosition();
    if (p_Main->isComputerRunning())
        p_Computer->setVolume(configuration_.volume_);
}

void MicroboardCardSetupDialog::onPrintMode(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    configuration_.printMode_ = event.GetSelection();
    setPrintMode();
    
    if (runningComputer_ == selectedComputer_)
        p_Printer->setPrintMode(configuration_.printMode_);
}

void MicroboardCardSetupDialog::setPrintMode()
{
    if (configuration_.printMode_ == PRINTFILE)
    {
        XRCCTRL(*this, "PrintFileButton"+selectedComputerStr_, wxButton)->Enable(true);
        XRCCTRL(*this, "PrintFile"+selectedComputerStr_, wxTextCtrl)->Enable(true);
    }
    else
    {
        XRCCTRL(*this, "PrintFileButton"+selectedComputerStr_, wxButton)->Enable(false);
        XRCCTRL(*this, "PrintFile"+selectedComputerStr_, wxTextCtrl)->Enable(false);
    }
}

void MicroboardCardSetupDialog::onPrintFile(wxCommandEvent& WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    wxString fileName;
    
    fileName = wxFileSelector( "Select the Printer Output File",
                              configuration_.printFileDir_, XRCCTRL(*this, "PrintFile"+selectedComputerStr_, wxTextCtrl)->GetValue(),
                              "txt",
                              wxString::Format
                              (
                               "Text File (*.txt)|*.txt|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    
    
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    configuration_.printFileDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    configuration_.printFile_ = FullPath.GetFullName();
    
    XRCCTRL(*this, "PrintFile"+selectedComputerStr_, wxTextCtrl)->SetValue(configuration_.printFile_);
}

void MicroboardCardSetupDialog::onPrintFileText(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    wxString printFile;
    
    configuration_.printFile_ = event.GetString();
    
    if (runningComputer_ == selectedComputer_)
    {
        if (configuration_.printFile_.Len() == 0)
            p_Printer->setPrintfileName("");
        else
            p_Printer->setPrintfileName(configuration_.printFileDir_ + configuration_.printFile_);
    }
}

void MicroboardCardSetupDialog::onMicroRom660SocketSetup(wxCommandEvent&WXUNUSED(event))
{
    if (!guiLoaded_)
        return;
    
    MicroRom660SocketSetupDialog MicroRom660SocketSetupDialog(this, microMemConfigutation_);
    MicroRom660SocketSetupDialog.ShowModal();

    checkErrorStatus();
}

void MicroboardCardSetupDialog::onPio1IoWindow(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    elfConfiguration_.usePioWindow1Cdp18s660 = event.IsChecked();

//    XRCCTRL(*this,"ShowAddress" + selectedComputerStr_,wxTextCtrl)->Enable(elfConfiguration_.usePioWindow1Cdp18s660 | elfConfiguration_.usePioWindow2Cdp18s660 | elfConfiguration_.useElfControlWindows | elfConfiguration_.usePio);
//    XRCCTRL(*this,"AddressText1" + selectedComputerStr_,wxStaticText)->Enable(elfConfiguration_.usePioWindow1Cdp18s660 | elfConfiguration_.usePioWindow2Cdp18s660 | elfConfiguration_.useElfControlWindows | elfConfiguration_.usePio);
//    XRCCTRL(*this,"AddressText2" + selectedComputerStr_,wxStaticText)->Enable(elfConfiguration_.usePioWindow1Cdp18s660 | elfConfiguration_.usePioWindow2Cdp18s660 | elfConfiguration_.useElfControlWindows | elfConfiguration_.usePio);
    
    if (runningComputer_ == selectedComputer_)
        p_Computer->showCdp18s660Pio1(elfConfiguration_.usePioWindow1Cdp18s660);
}

void MicroboardCardSetupDialog::onPio2IoWindow(wxCommandEvent&event)
{
    if (!guiLoaded_)
        return;
    
    elfConfiguration_.usePioWindow2Cdp18s660 = event.IsChecked();

//    XRCCTRL(*this,"ShowAddress" + selectedComputerStr_,wxTextCtrl)->Enable(elfConfiguration_.usePioWindow1Cdp18s660 | elfConfiguration_.usePioWindow2Cdp18s660 | elfConfiguration_.useElfControlWindows | elfConfiguration_.usePio);
//    XRCCTRL(*this,"AddressText1" + selectedComputerStr_,wxStaticText)->Enable(elfConfiguration_.usePioWindow1Cdp18s660 | elfConfiguration_.usePioWindow2Cdp18s660 | elfConfiguration_.useElfControlWindows | elfConfiguration_.usePio);
//    XRCCTRL(*this,"AddressText2" + selectedComputerStr_,wxStaticText)->Enable(elfConfiguration_.usePioWindow1Cdp18s660 | elfConfiguration_.usePioWindow2Cdp18s660 | elfConfiguration_.useElfControlWindows | elfConfiguration_.usePio);

    if (runningComputer_ == selectedComputer_)
        p_Computer->showCdp18s660Pio2(elfConfiguration_.usePioWindow2Cdp18s660);
}

BEGIN_EVENT_TABLE(MicroboardAdditionalCardSetupDialog, wxDialog)
    EVT_BUTTON(XRCID("MicroSetupSave"), MicroboardAdditionalCardSetupDialog::onSaveButton)

    EVT_BUTTON(XRCID("CardMicroboard05"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard06"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard07"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard08"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard09"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard10"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard11"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard12"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard13"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard14"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard15"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard16"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard17"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard18"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard19"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard20"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard21"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard22"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard23"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_BUTTON(XRCID("CardMicroboard24"), MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup)
    EVT_CHOICE(XRCID("CardChoiceMicroboard05"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard06"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard07"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard08"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard09"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard10"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard11"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard12"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard13"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard14"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard15"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard16"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard17"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard18"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard19"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard20"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard21"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard22"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard23"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
    EVT_CHOICE(XRCID("CardChoiceMicroboard24"), MicroboardAdditionalCardSetupDialog::onMicroboardType)
END_EVENT_TABLE()

MicroboardAdditionalCardSetupDialog::MicroboardAdditionalCardSetupDialog(wxWindow* parent, Conf configuration, ElfConfiguration elfConfiguration)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup_" + p_Main->getFontSize() + ".xrc");
    wxXmlResource::Get()->LoadDialog(this, parent, "CARD_Setup_Dialog");

    configuration_ = configuration;
    elfConfiguration_ = elfConfiguration;
    selectedComputer_ = p_Main->getSelectedComputerId();
    runningComputer_ = p_Main->getRunningComputerId();
    selectedComputerName_ = p_Main->getSelectedComputerText();
    selectedComputerStr_ = p_Main->getSelectedComputerStr();
    
    setCardType();

    setMaxCards();
    if (p_Main->isComputerRunning())
        XRCCTRL(*this, "MicroSetupSave", wxButton)->Disable();

    this->DoLayoutAdaptation();
}

void MicroboardAdditionalCardSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    p_Main->setConfiguration(selectedComputer_, configuration_);
    p_Main->setElfConfiguration(selectedComputer_, elfConfiguration_);
    
    EndModal(wxID_OK);
}

void MicroboardAdditionalCardSetupDialog::onMicroboardCardSetup(wxCommandEvent&event)
{
    long card;
    wxString idReference, buttonNumber;

    idReference = wxWindow::FindWindowById(event.GetId())->GetName();
    buttonNumber = idReference.Right(2);
    if (!buttonNumber.ToLong(&card, 10))
        return;

    card -= 1;
    microboardCardSetup((int)card);
}

void MicroboardAdditionalCardSetupDialog::onMicroboardType(wxCommandEvent&event)
{
    long card;
    wxString idReference, buttonNumber;
    
    idReference = wxWindow::FindWindowById(event.GetId())->GetName();
    buttonNumber = idReference.Right(2);
    if (!buttonNumber.ToLong(&card, 10))
        return;

    card -= 1;
    int selection = event.GetSelection();
    
    if (configuration_.microboardType_[card] != selection)
    {
        configuration_.microboardType_[card] = selection;
     
        p_Main->setCardMax(&configuration_);
        p_Main->clearConfigName();
        setMaxCards();
        setCardType();
    }
}

void MicroboardAdditionalCardSetupDialog::setMaxCards()
{
    wxString cardNumberStr;
    
    int maxCards = configuration_.microboardMaxCards_+1;
    if (maxCards == 24)
        maxCards = 23;
    for (int card=4; card<=maxCards; card++)
    {
        cardNumberStr.Printf("%02d",card+1);
        XRCCTRL(*this, "Card" + selectedComputerStr_+ cardNumberStr, wxButton)->Enable();
        XRCCTRL(*this, "CardChoice" + selectedComputerStr_+ cardNumberStr, wxChoice)->Enable();
        XRCCTRL(*this, "CardChoice" + selectedComputerStr_ + cardNumberStr, wxChoice)->SetSelection(configuration_.microboardType_[card]);
        
        if (configuration_.microboardType_[card] == 0)
        {
            XRCCTRL(*this, "Card" + selectedComputerStr_+ cardNumberStr, wxButton)->Disable();
            setButtonColor(cardNumberStr, false);
        }
    }
    for (int card=maxCards+1; card< 24; card++)
    {
        cardNumberStr.Printf("%02d",card+1);
        XRCCTRL(*this, "Card" + selectedComputerStr_+ cardNumberStr, wxButton)->Disable();
        XRCCTRL(*this, "CardChoice" + selectedComputerStr_+ cardNumberStr, wxChoice)->Disable();
        setButtonColor(cardNumberStr, false);
    }
}

void MicroboardAdditionalCardSetupDialog::setCardType()
{
    wxString cardStr;

    p_Main->checkAllBoardTypes(&configuration_, &elfConfiguration_);

    for (int card=4; card<=configuration_.microboardMaxCards_; card++)
    {
        cardStr.Printf("%02d", card+1);
        
        setButtonColor(cardStr, configuration_.errorMemoryOverlapp_[card] != "" || configuration_.errorDoubleBoard_[card] != "");
    }
}

void MicroboardAdditionalCardSetupDialog::setButtonColor(wxString cardstring, bool error)
{
    wxColour colour;
#ifdef __WXMAC__
    if (!error)
    {
        colour = XRCCTRL(*this, "MicroSetupSave", wxButton)->GetBackgroundColour();
        XRCCTRL(*this, "Card" + selectedComputerStr_ + cardstring, wxButton)->SetBackgroundColour(colour);
    }
    else
        XRCCTRL(*this, "Card" + selectedComputerStr_ + cardstring, wxButton)->SetBackgroundColour(p_Main->getGuiTextColour(GUI_COL_RED));
#else
    if (!error)
    {
        colour = XRCCTRL(*this, "MicroSetupSave", wxButton)->GetForegroundColour();
        XRCCTRL(*this, "Card" + selectedComputerStr_ + cardstring, wxButton)->SetForegroundColour(colour);
    }
    else
        XRCCTRL(*this, "Card" + selectedComputerStr_ + cardstring, wxButton)->SetForegroundColour(p_Main->getGuiTextColour(GUI_COL_RED));
#endif
    XRCCTRL(*this, "Card" + selectedComputerStr_ + cardstring, wxButton)->Refresh();
}

void MicroboardAdditionalCardSetupDialog::microboardCardSetup(int card)
{
    int boardType[24], maxCards;
    
    MicroMemoryConf microMemConf = p_Main->getMicroMemConf(card);
    
    MicroboardCardSetupDialog MicroboardCardSetupDialog(this, configuration_, elfConfiguration_, microMemConf, card);
    MicroboardCardSetupDialog.ShowModal();

    for (int cardNumber=4; cardNumber <24; cardNumber++)
        boardType[cardNumber] = configuration_.microboardType_[cardNumber];
    maxCards = configuration_.microboardMaxCards_;
    configuration_ = p_Main->getConfiguration(MICROBOARD);
    for (int cardNumber =4; cardNumber <24; cardNumber++)
        configuration_.microboardType_[cardNumber] = boardType[cardNumber];
    configuration_.microboardMaxCards_ = maxCards;
    elfConfiguration_ = p_Main->getElfConfiguration(MICROBOARD);
    
    setCardType();
}

BEGIN_EVENT_TABLE(MicroRom660SocketSetupDialog, wxDialog)
    EVT_BUTTON(XRCID("MicroSetupSave"), MicroRom660SocketSetupDialog::onSaveButton)
    EVT_CHOICE(XRCID("MicroSetupLocationU23"), MicroRom660SocketSetupDialog::onLocationXu23)
    EVT_CHOICE(XRCID("MicroSetupLocationU22"), MicroRom660SocketSetupDialog::onLocationXu23)
    EVT_CHOICE(XRCID("MicroSetupLocationU21"), MicroRom660SocketSetupDialog::onLocationXu21)
    EVT_CHOICE(XRCID("MicroSetupLocationU20"), MicroRom660SocketSetupDialog::onLocationXu21)
    EVT_CHOICE(XRCID("MicroSetupLocationU23_1"), MicroRom660SocketSetupDialog::onLocationXu)
    EVT_CHOICE(XRCID("MicroSetupLocationU22_1"), MicroRom660SocketSetupDialog::onLocationXu)
    EVT_CHOICE(XRCID("MicroSetupLocationU21_1"), MicroRom660SocketSetupDialog::onLocationXu)
    EVT_CHOICE(XRCID("MicroSetupLocationU20_1"), MicroRom660SocketSetupDialog::onLocationXu)
END_EVENT_TABLE()

MicroRom660SocketSetupDialog::MicroRom660SocketSetupDialog(MicroboardCardSetupDialog* parent, MicroMemoryConf microMemConfigutation)
{
    parent_ = parent;
    microMemConfigutation_ = microMemConfigutation;
    
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup_" + p_Main->getFontSize() + ".xrc");
    
    int locationXu23 = microMemConfigutation_.memLocation_[FOUR_SOCKET_ROM1];
    int locationXu21 = microMemConfigutation_.memLocation_[FOUR_SOCKET_ROM2];
    
    if (microMemConfigutation_.socketSize_[FOUR_SOCKET] == 1)
    {
        wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S660_4_2K_Dialog");
        XRCCTRL(*this, "MicroSetupMemoryU23", wxChoice)->SetSelection(microMemConfigutation_.memType[XU23ROM]);
        XRCCTRL(*this, "MicroSetupMemoryU22", wxChoice)->SetSelection(microMemConfigutation_.memType[XU22ROM]);
        XRCCTRL(*this, "MicroSetupMemoryU21", wxChoice)->SetSelection(microMemConfigutation_.memType[XU21ROM]);
        XRCCTRL(*this, "MicroSetupMemoryU20", wxChoice)->SetSelection(microMemConfigutation_.memType[XU20ROM]);
        
        XRCCTRL(*this, "MicroSetupLocationU23", wxChoice)->SetSelection(locationXu23);
        XRCCTRL(*this, "MicroSetupLocationU22", wxChoice)->SetSelection(locationXu23);
        XRCCTRL(*this, "MicroSetupLocationU21", wxChoice)->SetSelection(locationXu21);
        XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->SetSelection(locationXu21);
    }
    else
    {
        wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S660_4_1K_Dialog");
        
        XRCCTRL(*this, "MicroSetupLocationU23_1", wxChoice)->SetSelection(locationXu23);
        XRCCTRL(*this, "MicroSetupLocationU22_1", wxChoice)->SetSelection(locationXu23);
        XRCCTRL(*this, "MicroSetupLocationU21_1", wxChoice)->SetSelection(locationXu23);
        XRCCTRL(*this, "MicroSetupLocationU20_1", wxChoice)->SetSelection(locationXu23);
    }
    
    if (p_Main->isComputerRunning())
        XRCCTRL(*this, "MicroSetupSave", wxButton)->Disable();
}

void MicroRom660SocketSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    if (microMemConfigutation_.socketSize_[FOUR_SOCKET] == 1)
    {
        microMemConfigutation_.memType[XU23ROM] = XRCCTRL(*this, "MicroSetupMemoryU23", wxChoice)->GetSelection();
        microMemConfigutation_.memType[XU22ROM] = XRCCTRL(*this, "MicroSetupMemoryU22", wxChoice)->GetSelection();
        microMemConfigutation_.memType[XU21ROM] = XRCCTRL(*this, "MicroSetupMemoryU21", wxChoice)->GetSelection();
        microMemConfigutation_.memType[XU20ROM] = XRCCTRL(*this, "MicroSetupMemoryU20", wxChoice)->GetSelection();
        
        int locationXu23 = XRCCTRL(*this, "MicroSetupLocationU23", wxChoice)->GetSelection();
        microMemConfigutation_.memLocation_[FOUR_SOCKET_ROM1] = locationXu23;
        int locationXu21 = XRCCTRL(*this, "MicroSetupLocationU21", wxChoice)->GetSelection();
        microMemConfigutation_.memLocation_[FOUR_SOCKET_ROM2] = locationXu21;
    }
    else
    {
        int locationXu = XRCCTRL(*this, "MicroSetupLocationU23_1", wxChoice)->GetSelection();
        microMemConfigutation_.memLocation_[FOUR_SOCKET_ROM1] = locationXu;
    }
    
    parent_->setConfiguration(microMemConfigutation_);
    
    EndModal(wxID_OK);
}

void MicroRom660SocketSetupDialog::onLocationXu23( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU23", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU22", wxChoice)->SetSelection(location);
}

void MicroRom660SocketSetupDialog::onLocationXu21( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU21", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->SetSelection(location);
}

void MicroRom660SocketSetupDialog::onLocationXu( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU23_1", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU22_1", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU21_1", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU20_1", wxChoice)->SetSelection(location);
}

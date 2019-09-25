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


BEGIN_EVENT_TABLE(MicroOneSocketSetupDialog, wxDialog)
	EVT_BUTTON(XRCID("MicroSetupSave"), MicroOneSocketSetupDialog::onSaveButton)
END_EVENT_TABLE()

MicroOneSocketSetupDialog::MicroOneSocketSetupDialog(wxWindow* parent)
{
	wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup.xrc");
    
    ramSize_ = 2 << (p_Main->getMicroChipType(CDP18S600, ONE_SOCKET));
    wxString xmlString;
    xmlString.Printf("MicroSetup_1_%01DK_Dialog", ramSize_);
    
	wxXmlResource::Get()->LoadDialog(this, parent, xmlString);
    
    int location = p_Main->getMicroChipLocation(CDP18S600, ONE_SOCKET) / (ramSize_ / 2);
    
    XRCCTRL(*this, "MicroSetupMemory", wxChoice)->SetSelection(p_Main->getMicroChipMemory(CDP18S600, U21ROM));
    XRCCTRL(*this, "MicroSetupDisable", wxCheckBox)->SetValue(p_Main->getMicroChipDisable(CDP18S600, U21ROM));
    XRCCTRL(*this, "MicroSetupLocation", wxChoice)->SetSelection(location);
    
    if (p_Main->isComputerRunning())
        XRCCTRL(*this, "MicroSetupSave", wxButton)->Disable();
}

void MicroOneSocketSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    p_Main->setMicroChipMemory(CDP18S600, U21ROM,  XRCCTRL(*this, "MicroSetupMemory", wxChoice)->GetSelection());
    p_Main->setMicroChipDisable(CDP18S600, U21ROM,  XRCCTRL(*this, "MicroSetupDisable", wxCheckBox)->GetValue());

    int location = XRCCTRL(*this, "MicroSetupLocation", wxChoice)->GetSelection() * (ramSize_ / 2);
    p_Main->setMicroChipLocation(CDP18S600, ONE_SOCKET, location);

    p_Main->setOneSocketState("CDP18S600");

    EndModal(wxID_OK);
}

BEGIN_EVENT_TABLE(MicroFourSocketSetupDialog, wxDialog)
    EVT_BUTTON(XRCID("MicroSetupSave"), MicroFourSocketSetupDialog::onSaveButton)
    EVT_CHOICE(XRCID("MicroSetupLocationU20"), MicroFourSocketSetupDialog::onLocation)
    EVT_CHOICE(XRCID("MicroSetupLocationU19"), MicroFourSocketSetupDialog::onLocation)
    EVT_CHOICE(XRCID("MicroSetupLocationU18"), MicroFourSocketSetupDialog::onLocation)
    EVT_CHOICE(XRCID("MicroSetupLocationU17"), MicroFourSocketSetupDialog::onLocation)
END_EVENT_TABLE()

MicroFourSocketSetupDialog::MicroFourSocketSetupDialog(wxWindow* parent)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup.xrc");
    
    ramSize_ = 2 << (p_Main->getMicroChipType(CDP18S600, FOUR_SOCKET));
    wxString xmlString;
    xmlString.Printf("MicroSetup_4_%01DK_Dialog", ramSize_);
    
    wxXmlResource::Get()->LoadDialog(this, parent, xmlString);
    
    int location = p_Main->getMicroChipLocation(CDP18S600, FOUR_SOCKET) / (ramSize_ / 2);
    
    XRCCTRL(*this, "MicroSetupMemoryU20", wxChoice)->SetSelection(p_Main->getMicroChipMemory(CDP18S600, U20ROM));
    XRCCTRL(*this, "MicroSetupMemoryU19", wxChoice)->SetSelection(p_Main->getMicroChipMemory(CDP18S600, U19ROM));
    XRCCTRL(*this, "MicroSetupMemoryU18", wxChoice)->SetSelection(p_Main->getMicroChipMemory(CDP18S600, U18ROM));
    XRCCTRL(*this, "MicroSetupMemoryU17", wxChoice)->SetSelection(p_Main->getMicroChipMemory(CDP18S600, U17ROM));

    XRCCTRL(*this, "MicroSetupDisableU20", wxCheckBox)->SetValue(p_Main->getMicroChipDisable(CDP18S600, U20ROM));
    XRCCTRL(*this, "MicroSetupDisableU19", wxCheckBox)->SetValue(p_Main->getMicroChipDisable(CDP18S600, U19ROM));
    XRCCTRL(*this, "MicroSetupDisableU18", wxCheckBox)->SetValue(p_Main->getMicroChipDisable(CDP18S600, U18ROM));
    XRCCTRL(*this, "MicroSetupDisableU17", wxCheckBox)->SetValue(p_Main->getMicroChipDisable(CDP18S600, U17ROM));

    XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU19", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU18", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU17", wxChoice)->SetSelection(location);

    if (p_Main->isComputerRunning())
        XRCCTRL(*this, "MicroSetupSave", wxButton)->Disable();
}

void MicroFourSocketSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    p_Main->setMicroChipMemory(CDP18S600, U20ROM,  XRCCTRL(*this, "MicroSetupMemoryU20", wxChoice)->GetSelection());
    p_Main->setMicroChipMemory(CDP18S600, U19ROM,  XRCCTRL(*this, "MicroSetupMemoryU19", wxChoice)->GetSelection());
    p_Main->setMicroChipMemory(CDP18S600, U18ROM,  XRCCTRL(*this, "MicroSetupMemoryU18", wxChoice)->GetSelection());
    p_Main->setMicroChipMemory(CDP18S600, U17ROM,  XRCCTRL(*this, "MicroSetupMemoryU17", wxChoice)->GetSelection());

    p_Main->setMicroChipDisable(CDP18S600, U20ROM,  XRCCTRL(*this, "MicroSetupDisableU20", wxCheckBox)->GetValue());
    p_Main->setMicroChipDisable(CDP18S600, U19ROM,  XRCCTRL(*this, "MicroSetupDisableU19", wxCheckBox)->GetValue());
    p_Main->setMicroChipDisable(CDP18S600, U18ROM,  XRCCTRL(*this, "MicroSetupDisableU18", wxCheckBox)->GetValue());
    p_Main->setMicroChipDisable(CDP18S600, U17ROM,  XRCCTRL(*this, "MicroSetupDisableU17", wxCheckBox)->GetValue());

    int location = XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->GetSelection() * (ramSize_ / 2);
    p_Main->setMicroChipLocation(CDP18S600, FOUR_SOCKET, location);
    
    p_Main->setFourSocketState(CDP18S600, "CDP18S600");

    EndModal(wxID_OK);
}

void MicroFourSocketSetupDialog::onLocation( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU19", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU18", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU17", wxChoice)->SetSelection(location);

}

BEGIN_EVENT_TABLE(MicroRomSocketSetupDialog, wxDialog)
    EVT_BUTTON(XRCID("MicroSetupSave"), MicroRomSocketSetupDialog::onSaveButton)
    EVT_CHOICE(XRCID("MicroSetupLocationU20"), MicroRomSocketSetupDialog::onLocationXu25)
    EVT_CHOICE(XRCID("MicroSetupLocationU19"), MicroRomSocketSetupDialog::onLocationXu25)
    EVT_CHOICE(XRCID("MicroSetupLocationU18"), MicroRomSocketSetupDialog::onLocationXu27)
    EVT_CHOICE(XRCID("MicroSetupLocationU17"), MicroRomSocketSetupDialog::onLocationXu27)
    EVT_CHOICE(XRCID("MicroSetupLocationU20_1"), MicroRomSocketSetupDialog::onLocationXu)
    EVT_CHOICE(XRCID("MicroSetupLocationU19_1"), MicroRomSocketSetupDialog::onLocationXu)
    EVT_CHOICE(XRCID("MicroSetupLocationU18_1"), MicroRomSocketSetupDialog::onLocationXu)
    EVT_CHOICE(XRCID("MicroSetupLocationU17_1"), MicroRomSocketSetupDialog::onLocationXu)
END_EVENT_TABLE()

MicroRomSocketSetupDialog::MicroRomSocketSetupDialog(wxWindow* parent)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup.xrc");

    int locationXu25 = p_Main->getMicroChipLocation(CDP18S601, FOUR_SOCKET_ROM1);
    int locationXu27 = p_Main->getMicroChipLocation(CDP18S601, FOUR_SOCKET_ROM2);

    if (p_Main->getMicroChipType(CDP18S601, FOUR_SOCKET) == 1)
    {
        wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S601_4_2K_Dialog");
        XRCCTRL(*this, "MicroSetupMemoryU20", wxChoice)->SetSelection(p_Main->getMicroChipMemory(CDP18S601, XU27ROM));
        XRCCTRL(*this, "MicroSetupMemoryU19", wxChoice)->SetSelection(p_Main->getMicroChipMemory(CDP18S601, XU26ROM));
        XRCCTRL(*this, "MicroSetupMemoryU18", wxChoice)->SetSelection(p_Main->getMicroChipMemory(CDP18S601, XU25ROM));
        XRCCTRL(*this, "MicroSetupMemoryU17", wxChoice)->SetSelection(p_Main->getMicroChipMemory(CDP18S601, XU24ROM));
        
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

void MicroRomSocketSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    if (p_Main->getMicroChipType(CDP18S601, FOUR_SOCKET) == 1)
    {
        p_Main->setMicroChipMemory(CDP18S601, XU27ROM,  XRCCTRL(*this, "MicroSetupMemoryU20", wxChoice)->GetSelection());
        p_Main->setMicroChipMemory(CDP18S601, XU26ROM,  XRCCTRL(*this, "MicroSetupMemoryU19", wxChoice)->GetSelection());
        p_Main->setMicroChipMemory(CDP18S601, XU25ROM,  XRCCTRL(*this, "MicroSetupMemoryU18", wxChoice)->GetSelection());
        p_Main->setMicroChipMemory(CDP18S601, XU24ROM,  XRCCTRL(*this, "MicroSetupMemoryU17", wxChoice)->GetSelection());

        int locationXu25 = XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->GetSelection();
        p_Main->setMicroChipLocation(CDP18S601, FOUR_SOCKET_ROM1, locationXu25);
        int locationXu27 = XRCCTRL(*this, "MicroSetupLocationU18", wxChoice)->GetSelection();
        p_Main->setMicroChipLocation(CDP18S601, FOUR_SOCKET_ROM2, locationXu27);
    }
    else
    {
        int locationXu = XRCCTRL(*this, "MicroSetupLocationU20_1", wxChoice)->GetSelection();
        p_Main->setMicroChipLocation(CDP18S601, FOUR_SOCKET_ROM2, locationXu);
    }
    
    EndModal(wxID_OK);
}

void MicroRomSocketSetupDialog::onLocationXu25( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU19", wxChoice)->SetSelection(location);
}

void MicroRomSocketSetupDialog::onLocationXu27( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU18", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU17", wxChoice)->SetSelection(location);
}

void MicroRomSocketSetupDialog::onLocationXu( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU20_1", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU19_1", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU18_1", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU17_1", wxChoice)->SetSelection(location);
}

BEGIN_EVENT_TABLE(MicroRom603ASocketSetupDialog, wxDialog)
    EVT_BUTTON(XRCID("MicroSetupSave"), MicroRom603ASocketSetupDialog::onSaveButton)
    EVT_CHOICE(XRCID("MicroSetupLocationU20"), MicroRom603ASocketSetupDialog::onLocationXu25)
    EVT_CHOICE(XRCID("MicroSetupLocationU19"), MicroRom603ASocketSetupDialog::onLocationXu25)
    EVT_CHOICE(XRCID("MicroSetupLocationU18"), MicroRom603ASocketSetupDialog::onLocationXu27)
    EVT_CHOICE(XRCID("MicroSetupLocationU17"), MicroRom603ASocketSetupDialog::onLocationXu27)
    EVT_CHOICE(XRCID("MicroSetupLocationU20_1"), MicroRom603ASocketSetupDialog::onLocationXu)
    EVT_CHOICE(XRCID("MicroSetupLocationU19_1"), MicroRom603ASocketSetupDialog::onLocationXu)
    EVT_CHOICE(XRCID("MicroSetupLocationU18_1"), MicroRom603ASocketSetupDialog::onLocationXu)
    EVT_CHOICE(XRCID("MicroSetupLocationU17_1"), MicroRom603ASocketSetupDialog::onLocationXu)
END_EVENT_TABLE()

MicroRom603ASocketSetupDialog::MicroRom603ASocketSetupDialog(wxWindow* parent)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup.xrc");
    
    int locationXu25 = p_Main->getMicroChipLocation(CDP18S603A, FOUR_SOCKET_ROM1);
    int locationXu27 = p_Main->getMicroChipLocation(CDP18S603A, FOUR_SOCKET_ROM2);
    
    if (p_Main->getMicroChipType(CDP18S603A, FOUR_SOCKET) == 1)
    {
        wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S601_4_2K_Dialog");
        XRCCTRL(*this, "MicroSetupMemoryU20", wxChoice)->SetSelection(p_Main->getMicroChipMemory(CDP18S603A, XU27ROM));
        XRCCTRL(*this, "MicroSetupMemoryU19", wxChoice)->SetSelection(p_Main->getMicroChipMemory(CDP18S603A, XU26ROM));
        XRCCTRL(*this, "MicroSetupMemoryU18", wxChoice)->SetSelection(p_Main->getMicroChipMemory(CDP18S603A, XU25ROM));
        XRCCTRL(*this, "MicroSetupMemoryU17", wxChoice)->SetSelection(p_Main->getMicroChipMemory(CDP18S603A, XU24ROM));
        
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

void MicroRom603ASocketSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    if (p_Main->getMicroChipType(CDP18S603A, FOUR_SOCKET) == 1)
    {
        p_Main->setMicroChipMemory(CDP18S603A, XU27ROM,  XRCCTRL(*this, "MicroSetupMemoryU20", wxChoice)->GetSelection());
        p_Main->setMicroChipMemory(CDP18S603A, XU26ROM,  XRCCTRL(*this, "MicroSetupMemoryU19", wxChoice)->GetSelection());
        p_Main->setMicroChipMemory(CDP18S603A, XU25ROM,  XRCCTRL(*this, "MicroSetupMemoryU18", wxChoice)->GetSelection());
        p_Main->setMicroChipMemory(CDP18S603A, XU24ROM,  XRCCTRL(*this, "MicroSetupMemoryU17", wxChoice)->GetSelection());
        
        int locationXu25 = XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->GetSelection();
        p_Main->setMicroChipLocation(CDP18S603A, FOUR_SOCKET_ROM1, locationXu25);
        int locationXu27 = XRCCTRL(*this, "MicroSetupLocationU18", wxChoice)->GetSelection();
        p_Main->setMicroChipLocation(CDP18S603A, FOUR_SOCKET_ROM2, locationXu27);
    }
    else
    {
        int locationXu = XRCCTRL(*this, "MicroSetupLocationU20_1", wxChoice)->GetSelection();
        p_Main->setMicroChipLocation(CDP18S603A, FOUR_SOCKET_ROM2, locationXu);
    }
    
    EndModal(wxID_OK);
}

void MicroRom603ASocketSetupDialog::onLocationXu25( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU20", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU19", wxChoice)->SetSelection(location);
}

void MicroRom603ASocketSetupDialog::onLocationXu27( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU18", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU17", wxChoice)->SetSelection(location);
}

void MicroRom603ASocketSetupDialog::onLocationXu( wxCommandEvent& event)
{
    int location = event.GetSelection();
    
    XRCCTRL(*this, "MicroSetupLocationU20_1", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU19_1", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU18_1", wxChoice)->SetSelection(location);
    XRCCTRL(*this, "MicroSetupLocationU17_1", wxChoice)->SetSelection(location);
}

BEGIN_EVENT_TABLE(One600SocketSetupDialog, wxDialog)
    EVT_BUTTON(XRCID("MicroSetupSave"), One600SocketSetupDialog::onSaveButton)
END_EVENT_TABLE()

One600SocketSetupDialog::One600SocketSetupDialog(MicroboardSetupDialog* parent, Conf configuration)
{
    parent_ = parent;
    configuration_ = configuration;

    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup.xrc");
    
    ramSize_ = 2 << (configuration_.microChipType_[ONE_SOCKET]);
    wxString xmlString;
    xmlString.Printf("MicroSetup_1_%01DK_Dialog", ramSize_);
    
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

    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup.xrc");
    
    ramSize_ = 2 << (configuration_.microChipType_[FOUR_SOCKET]);
    wxString xmlString;
    xmlString.Printf("MicroSetup_4_%01DK_Dialog", ramSize_);
    
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

    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup.xrc");
    
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

    EVT_TEXT(XRCID("RamBlockMicroboard"), MicroboardSetupDialog::on604BRamBlock)
    EVT_TEXT(XRCID("RomBlockMicroboard"), MicroboardSetupDialog::on604BRomBlock)

END_EVENT_TABLE()

MicroboardSetupDialog::MicroboardSetupDialog(wxWindow* parent, Conf configuration)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup.xrc");
    configuration_ = configuration;
    selectedComputer_ = p_Main->getSelectedComputerId();
    selectedComputerName_ = p_Main->getSelectedComputerText();
    selectedComputerStr_ = p_Main->getSelectedComputerStr();
    
    switch (configuration_.microboardType_[1])
    {
        case MICROBOARD_CDP18S600:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S600_Setup_Dialog");
        break;

        case MICROBOARD_CDP18S601:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S601_Setup_Dialog");
        break;
            
        case MICROBOARD_CDP18S603A:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S603A_Setup_Dialog");
        break;
            
        case MICROBOARD_CDP18S604B:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S604B_Setup_Dialog");
        break;
    }

    XRCCTRL(*this, "MainRomU21" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U21ROM]);
    XRCCTRL(*this, "MainRomU20" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U20ROM]);

    switch (configuration_.microboardType_[1])
    {
        case MICROBOARD_CDP18S600:
            if (configuration_.microChipType_[FOUR_SOCKET] > 2)
                configuration_.microChipType_[FOUR_SOCKET] = 2;
            XRCCTRL(*this, "MainRomU19" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U19ROM]);
            XRCCTRL(*this, "MainRomU18" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U18ROM]);
            XRCCTRL(*this, "MainRomU17" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U17ROM]);
            XRCCTRL(*this, "OneSocketBank" + selectedComputerStr_, wxChoice)->SetSelection(configuration_.microChipType_[ONE_SOCKET]);
            setOneSocketState();
            setFourSocketState();
        break;
 
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S603A:
            if (configuration_.microChipType_[FOUR_SOCKET] > 1)
                configuration_.microChipType_[FOUR_SOCKET] = 1;
            XRCCTRL(*this, "MainRomU19" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U19ROM]);
            XRCCTRL(*this, "MainRomU18" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U18ROM]);
            XRCCTRL(*this, "MainRomU17" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U17ROM]);
            XRCCTRL(*this, "OneSocketBank" + selectedComputerStr_, wxChoice)->SetSelection(configuration_.microChipType_[ONE_SOCKET]);
        break;

        case MICROBOARD_CDP18S604B:
            XRCCTRL(*this, "MainRomU21" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U21ROM]);
            XRCCTRL(*this, "MainRomU20" + selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U20ROM]);
            XRCCTRL(*this, "RamBlock" + selectedComputerStr_, wxTextCtrl)->ChangeValue(configuration_.microChipBlock_[ONE_SOCKET]);
            XRCCTRL(*this, "RomBlock" + selectedComputerStr_, wxTextCtrl)->ChangeValue(configuration_.microChipBlock_[FOUR_SOCKET]);
            one604BSocketBankGui();
            four604BSocketBankGui();
        break;
    }

    XRCCTRL(*this, "FourSocketBank" + selectedComputerStr_, wxChoice)->SetSelection(configuration_.microChipType_[FOUR_SOCKET]);

    if (p_Main->isComputerRunning())
        XRCCTRL(*this, "MicroSetupSave", wxButton)->Disable();
}

void MicroboardSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    p_Main->setConfiguration(selectedComputer_, configuration_);
    
    EndModal(wxID_OK);
}

void MicroboardSetupDialog::onRomU21(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName, romName = "";
    switch (configuration_.microboardType_[1])
    {
        case MICROBOARD_CDP18S600:
            romName = " U21";
        break;
            
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S603A:
        case MICROBOARD_CDP18S604B:
            romName = " RAM";
        break;
    }
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+romName+" file to load",
                              configuration_.romDir_[U21ROM], XRCCTRL(*this, "MainRomU21"+selectedComputerStr_, wxComboBox)->GetValue(),
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
    configuration_.romDir_[U21ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    configuration_.rom_[U21ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU21"+selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U21ROM]);
}

void MicroboardSetupDialog::onRomU21Text(wxCommandEvent& WXUNUSED(event))
{
    configuration_.rom_[U21ROM] = XRCCTRL(*this, "MainRomU21"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardSetupDialog::onRomU20(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName, romName = "";
    switch (configuration_.microboardType_[1])
    {
        case MICROBOARD_CDP18S600:
            romName = " U20";
        break;
            
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S603A:
            romName = " XU27 ROM";
        break;

        case MICROBOARD_CDP18S604B:
            romName = " ROM";
        break;
    }

    fileName = wxFileSelector( "Select the "+selectedComputerName_+romName+" file to load",
                              configuration_.romDir_[U20ROM], XRCCTRL(*this, "MainRomU20"+selectedComputerStr_, wxComboBox)->GetValue(),
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
    configuration_.romDir_[U20ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    configuration_.rom_[U20ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU20"+selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U20ROM]);
}

void MicroboardSetupDialog::onRomU20Text(wxCommandEvent& WXUNUSED(event))
{
    configuration_.rom_[U20ROM] = XRCCTRL(*this, "MainRomU20"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardSetupDialog::onRomU19(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName, romName = "";
    switch (configuration_.microboardType_[1])
    {
        case MICROBOARD_CDP18S600:
            romName = " U19";
        break;
            
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S603A:
            romName = " XU26 ROM";
        break;
    }
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+romName+" ROM file to load",
                              configuration_.romDir_[U19ROM], XRCCTRL(*this, "MainRomU19"+selectedComputerStr_, wxComboBox)->GetValue(),
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
    configuration_.romDir_[U19ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    configuration_.rom_[U19ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU19"+selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U19ROM]);
}

void MicroboardSetupDialog::onRomU19Text(wxCommandEvent& WXUNUSED(event))
{
    configuration_.rom_[U19ROM] = XRCCTRL(*this, "MainRomU19"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardSetupDialog::onRomU18(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName, romName = "";
    switch (configuration_.microboardType_[1])
    {
        case MICROBOARD_CDP18S600:
            romName = " U18";
        break;
            
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S603A:
            romName = " XU25 ROM";
        break;
    }
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+romName+" ROM file to load",
                              configuration_.romDir_[U18ROM], XRCCTRL(*this, "MainRomU18"+selectedComputerStr_, wxComboBox)->GetValue(),
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
    configuration_.romDir_[U18ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    configuration_.rom_[U18ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU81"+selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U18ROM]);
}

void MicroboardSetupDialog::onRomU18Text(wxCommandEvent& WXUNUSED(event))
{
    configuration_.rom_[U18ROM] = XRCCTRL(*this, "MainRomU18"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardSetupDialog::onRomU17(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName, romName = "";
    switch (configuration_.microboardType_[1])
    {
        case MICROBOARD_CDP18S600:
            romName = " U17";
            break;
            
        case MICROBOARD_CDP18S601:
        case MICROBOARD_CDP18S603A:
            romName = " XU24 ROM";
            break;
    }
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+romName+" ROM file to load",
                              configuration_.romDir_[U17ROM], XRCCTRL(*this, "MainRomU17"+selectedComputerStr_, wxComboBox)->GetValue(),
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
    configuration_.romDir_[U17ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    configuration_.rom_[U17ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU17"+selectedComputerStr_, wxComboBox)->SetValue(configuration_.rom_[U17ROM]);
}

void MicroboardSetupDialog::onRomU17Text(wxCommandEvent& WXUNUSED(event))
{
    configuration_.rom_[U17ROM] = XRCCTRL(*this, "MainRomU17"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardSetupDialog::setOneSocketState()
{
    setRamlabel(U21ROM, "U21");
}

void MicroboardSetupDialog::setFourSocketState()
{
    if (configuration_.microboardType_[1] == MICROBOARD_CDP18S600)
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
}

void MicroboardSetupDialog::on604BRomBlock(wxCommandEvent&event)
{
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
    label.Printf("%dK Block (0-%d)", chipType, (64/chipType)-1);
    XRCCTRL(*this, "RomBlockText"+selectedComputerStr_, wxStaticText)->SetLabel(label);
    
    configuration_.microChipBlock_[FOUR_SOCKET].ToLong(&block);
    
    if (configuration_.microChipType_[FOUR_SOCKET] == 0)
        label.Printf("address: %04X-%04X", (int)(block*chipType*0x400), (int)(block*chipType*0x400+0x1ff));
    else
        label.Printf("address: %04X-%04X", (int)(block*chipType*0x400), (int)(block*chipType*0x400+chipType*0x400-1));
    XRCCTRL(*this, "RomAddress"+selectedComputerStr_, wxStaticText)->SetLabel(label);
}

void MicroboardSetupDialog::onOneSocketBank(wxCommandEvent&event)
{
    configuration_.microChipType_[ONE_SOCKET] = event.GetSelection();
}

void MicroboardSetupDialog::onFourSocketBank(wxCommandEvent&event)
{
    if (configuration_.microboardType_[1] != MICROBOARD_CDP18S604B)
        configuration_.microChipType_[FOUR_SOCKET] = event.GetSelection();
    else
        onFour604BSocketBank(event);
}

void MicroboardSetupDialog::onOne600SocketSetup(wxCommandEvent&WXUNUSED(event))
{
    One600SocketSetupDialog One600SocketSetupDialog(this, configuration_);
    One600SocketSetupDialog.ShowModal();
    
    setOneSocketState();
}

void MicroboardSetupDialog::onFour600SocketSetup(wxCommandEvent&WXUNUSED(event))
{
    Four600SocketSetupDialog Four600SocketSetupDialog(this, configuration_);
    Four600SocketSetupDialog.ShowModal();
    
    setFourSocketState();
}

void MicroboardSetupDialog::onFour601SocketSetup(wxCommandEvent&WXUNUSED(event))
{
    Four601SocketSetupDialog Four601SocketSetupDialog(this, configuration_);
    Four601SocketSetupDialog.ShowModal();
}

BEGIN_EVENT_TABLE(MicroboardCardSetupDialog, wxDialog)
    EVT_BUTTON(XRCID("MicroSetupSave"), MicroboardCardSetupDialog::onSaveButton)
    EVT_CHOICE(XRCID("FdcIoGroupMicroboard"), MicroboardCardSetupDialog::onFdcIoGroupMicroboard)
    EVT_CHOICE(XRCID("SocketBankMicroboard"), MicroboardCardSetupDialog::onSocketBankMicroboard)
    EVT_CHOICE(XRCID("LowHighMicroboard"), MicroboardCardSetupDialog::onLowHighMicroboard)

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

    EVT_TEXT(XRCID("Rom1BlockMicroboard"), MicroboardCardSetupDialog::onRom1Block)
    EVT_TEXT(XRCID("Rom2BlockMicroboard"), MicroboardCardSetupDialog::onRom2Block)
    EVT_TEXT(XRCID("RamBlockMicroboard"), MicroboardCardSetupDialog::onRamBlock)

END_EVENT_TABLE()

MicroboardCardSetupDialog::MicroboardCardSetupDialog(wxWindow* parent, Conf configuration, ElfConfiguration elfConfiguration, MicroMemoryConf microMemConfigutation, int cardNumber)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"microsetup.xrc");
    configuration_ = configuration;
    elfConfiguration_ = elfConfiguration;
    microMemConfigutation_ = microMemConfigutation;
    cardNumber_ = cardNumber;
    selectedComputer_ = p_Main->getSelectedComputerId();
    selectedComputerName_ = p_Main->getSelectedComputerText();
    selectedComputerStr_ = p_Main->getSelectedComputerStr();
 
    switch (configuration_.microboardType_[cardNumber])
    {
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
            XRCCTRL(*this, "SocketBankMicroboard", wxChoice)->SetSelection(microMemConfigutation.socketSize_);
            XRCCTRL(*this, "MemoryInhibit64Microboard", wxChoice)->SetSelection(microMemConfigutation.inhibit64_);
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

        case CARD_CDP18S651:
            wxXmlResource::Get()->LoadDialog(this, parent, "CDP18S651_Setup_Dialog");
            if (elfConfiguration_.upd765GroupMask == 0xf)
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
            XRCCTRL(*this, "RomSection1Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[ROM_SOCKET1]);
            XRCCTRL(*this, "RomSection2Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[ROM_SOCKET2]);
            XRCCTRL(*this, "RomSection3Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[ROM_SOCKET3]);
            XRCCTRL(*this, "RomSection4Microboard", wxComboBox)->SetValue(microMemConfigutation.rom_[ROM_SOCKET4]);
            XRCCTRL(*this, "Rom3SocketBankMicroboard", wxChoice)->SetSelection(microMemConfigutation.memLocation_[0]);
            XRCCTRL(*this, "Rom4SocketBankMicroboard", wxChoice)->SetSelection(microMemConfigutation.memLocation_[1]);
            XRCCTRL(*this, "RamBlockMicroboard", wxTextCtrl)->ChangeValue(microMemConfigutation.chipBlockRam_);
            XRCCTRL(*this, "Rom1BlockMicroboard", wxTextCtrl)->ChangeValue(microMemConfigutation.chipBlockRom_[0]);
            XRCCTRL(*this, "Rom2BlockMicroboard", wxTextCtrl)->ChangeValue(microMemConfigutation.chipBlockRom_[1]);

            ramSocketBankGui();
            rom1SocketBankGui();
            rom2SocketBankGui();
        break;
    }
    
    if (p_Main->isComputerRunning())
        XRCCTRL(*this, "MicroSetupSave", wxButton)->Disable();
}

void MicroboardCardSetupDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    switch (configuration_.microboardType_[cardNumber_])
    {
        case CARD_CDP18S626:
        case CARD_CDP18S629:
            microMemConfigutation_.memType[CDP626_SEC1] = XRCCTRL(*this, "MemorySection1Microboard", wxChoice)->GetSelection();
            microMemConfigutation_.memType[CDP626_SEC2] = XRCCTRL(*this, "MemorySection2Microboard", wxChoice)->GetSelection();
            microMemConfigutation_.memType[CDP626_SEC3] = XRCCTRL(*this, "MemorySection3Microboard", wxChoice)->GetSelection();
            microMemConfigutation_.memType[CDP626_SEC4] = XRCCTRL(*this, "MemorySection4Microboard", wxChoice)->GetSelection();
            microMemConfigutation_.inhibit64_ = XRCCTRL(*this, "MemoryInhibit64Microboard", wxChoice)->GetSelection();
            microMemConfigutation_.inhibit32Low_ = XRCCTRL(*this, "MemoryInhibit32LowMicroboard", wxChoice)->GetSelection();
            microMemConfigutation_.inhibit32High_ = XRCCTRL(*this, "MemoryInhibit32HighMicroboard", wxChoice)->GetSelection();
        break;

        case CARD_CDP18S652:
            microMemConfigutation_.memLocation_[0] = XRCCTRL(*this, "Rom3SocketBankMicroboard", wxChoice)->GetSelection();
            microMemConfigutation_.memLocation_[1] = XRCCTRL(*this, "Rom4SocketBankMicroboard", wxChoice)->GetSelection();
        break;
    }

    p_Main->setConfiguration(selectedComputer_, configuration_);
    p_Main->setElfConfiguration(selectedComputer_, elfConfiguration_);
    p_Main->setMicroMemConfiguration(cardNumber_, microMemConfigutation_);

    EndModal(wxID_OK);
}

void MicroboardCardSetupDialog::onFdcIoGroupMicroboard(wxCommandEvent&event)
{
    int selection = event.GetSelection();
    
    if (selection < 4)
    {
        elfConfiguration_.upd765Group = 1 << selection;
        elfConfiguration_.upd765GroupMask = 0xf;
    }
    else
    {
        elfConfiguration_.upd765Group = (selection - 4) << 4;
        elfConfiguration_.upd765GroupMask = 0xf0;
    }
}

void MicroboardCardSetupDialog::onSocketBankMicroboard(wxCommandEvent&event)
{
    microMemConfigutation_.socketSize_ = event.GetSelection();
    setInhibit();
}

void MicroboardCardSetupDialog::setInhibit()
{
    if (microMemConfigutation_.socketSize_ == 1)
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
}

void MicroboardCardSetupDialog::onRamSec1(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+"RAM file to load",
                              microMemConfigutation_.romDir_[CDP626_SEC1], XRCCTRL(*this, "RamSection1"+selectedComputerStr_, wxComboBox)->GetValue(),
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
    microMemConfigutation_.ramDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    microMemConfigutation_.ram_ = FullPath.GetFullName();
    
    XRCCTRL(*this, "RamSection1"+selectedComputerStr_, wxComboBox)->SetValue(microMemConfigutation_.ram_);
}

void MicroboardCardSetupDialog::onRamSec1Text(wxCommandEvent& WXUNUSED(event))
{
    microMemConfigutation_.ram_ = XRCCTRL(*this, "RamSection1"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardCardSetupDialog::onRomSec1(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+"block 1 file to load",
                              microMemConfigutation_.romDir_[CDP626_SEC1], XRCCTRL(*this, "RomSection1"+selectedComputerStr_, wxComboBox)->GetValue(),
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
    microMemConfigutation_.romDir_[CDP626_SEC1] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    microMemConfigutation_.rom_[CDP626_SEC1] = FullPath.GetFullName();
    
    XRCCTRL(*this, "RomSection1"+selectedComputerStr_, wxComboBox)->SetValue(microMemConfigutation_.rom_[CDP626_SEC1]);
}

void MicroboardCardSetupDialog::onRomSec1Text(wxCommandEvent& WXUNUSED(event))
{
    microMemConfigutation_.rom_[CDP626_SEC1] = XRCCTRL(*this, "RomSection1"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardCardSetupDialog::onRomSec2(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+"block 2 file to load",
                              microMemConfigutation_.romDir_[CDP626_SEC2], XRCCTRL(*this, "RomSection2"+selectedComputerStr_, wxComboBox)->GetValue(),
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
    microMemConfigutation_.romDir_[CDP626_SEC2] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    microMemConfigutation_.rom_[CDP626_SEC2] = FullPath.GetFullName();
    
    XRCCTRL(*this, "RomSection2"+selectedComputerStr_, wxComboBox)->SetValue(microMemConfigutation_.rom_[CDP626_SEC2]);
}

void MicroboardCardSetupDialog::onRomSec2Text(wxCommandEvent& WXUNUSED(event))
{
    microMemConfigutation_.rom_[CDP626_SEC2] = XRCCTRL(*this, "RomSection2"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardCardSetupDialog::onRomSec3(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+selectedComputerName_+"block 3 file to load",
                              microMemConfigutation_.romDir_[CDP626_SEC3], XRCCTRL(*this, "RomSection3"+selectedComputerStr_, wxComboBox)->GetValue(),
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
    microMemConfigutation_.romDir_[CDP626_SEC3] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    microMemConfigutation_.rom_[CDP626_SEC3] = FullPath.GetFullName();
    
    XRCCTRL(*this, "RomSection3"+selectedComputerStr_, wxComboBox)->SetValue(microMemConfigutation_.rom_[CDP626_SEC3]);
}

void MicroboardCardSetupDialog::onRomSec3Text(wxCommandEvent& WXUNUSED(event))
{
    microMemConfigutation_.rom_[CDP626_SEC3] = XRCCTRL(*this, "RomSection3"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardCardSetupDialog::onRomSec4(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;

    fileName = wxFileSelector( "Select the "+selectedComputerName_+"block 4 file to load",
                              microMemConfigutation_.romDir_[CDP626_SEC4], XRCCTRL(*this, "RomSection4"+selectedComputerStr_, wxComboBox)->GetValue(),
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
    microMemConfigutation_.romDir_[CDP626_SEC4] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    microMemConfigutation_.rom_[CDP626_SEC4] = FullPath.GetFullName();
    
    XRCCTRL(*this, "RomSection4"+selectedComputerStr_, wxComboBox)->SetValue(microMemConfigutation_.rom_[CDP626_SEC4]);
}

void MicroboardCardSetupDialog::onRomSec4Text(wxCommandEvent& WXUNUSED(event))
{
    microMemConfigutation_.rom_[CDP626_SEC4] = XRCCTRL(*this, "RomSection4"+selectedComputerStr_, wxComboBox)->GetValue();
}

void MicroboardCardSetupDialog::onRamBlock(wxCommandEvent&event)
{
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
}

void MicroboardCardSetupDialog::ramSocketBankGui()
{
    long block;
    microMemConfigutation_.chipBlockRam_.ToLong(&block);
    
    wxString label;
    label.Printf("address: %04X-%04X", (int)(block*0x400), (int)(block*0x400+0x3ff));
    XRCCTRL(*this, "RamAddress"+selectedComputerStr_, wxStaticText)->SetLabel(label);
}

void MicroboardCardSetupDialog::onRom1Block(wxCommandEvent&event)
{
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
    
    microMemConfigutation_.chipBlockRom_[0] = stringBlock;
    
    rom1SocketBankGui();
}

void MicroboardCardSetupDialog::rom1SocketBankGui()
{
    long block;
    microMemConfigutation_.chipBlockRom_[0].ToLong(&block);
    
    wxString label;
    label.Printf("address: %04X-%04X", (int)(block*0x400), (int)(block*0x400+0x3ff));
    XRCCTRL(*this, "Rom1Address"+selectedComputerStr_, wxStaticText)->SetLabel(label);
}

void MicroboardCardSetupDialog::onRom2Block(wxCommandEvent&event)
{
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
    
    microMemConfigutation_.chipBlockRom_[1] = stringBlock;
    
    rom2SocketBankGui();
}

void MicroboardCardSetupDialog::rom2SocketBankGui()
{
    long block;
    microMemConfigutation_.chipBlockRom_[1].ToLong(&block);
    
    wxString label;
    label.Printf("address: %04X-%04X", (int)(block*0x800), (int)(block*0x800+0x7ff));
    XRCCTRL(*this, "Rom2Address"+selectedComputerStr_, wxStaticText)->SetLabel(label);
}


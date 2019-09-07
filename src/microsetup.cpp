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


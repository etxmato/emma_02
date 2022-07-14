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
#include "guieprom.h"

#include "wx/xrc/xmlres.h"             

BEGIN_EVENT_TABLE(EpromDialog, wxDialog)
    EVT_BUTTON(XRCID("EpromSave"), EpromDialog::onSaveButton)
    EVT_BUTTON(XRCID("EpromButton0"), EpromDialog::onEprom0Button)
    EVT_BUTTON(XRCID("EpromButton1"), EpromDialog::onEprom1Button)
    EVT_BUTTON(XRCID("EpromButton2"), EpromDialog::onEprom2Button)
    EVT_BUTTON(XRCID("EpromButton3"), EpromDialog::onEprom3Button)
    EVT_BUTTON(XRCID("EpromButton4"), EpromDialog::onEprom4Button)
END_EVENT_TABLE()

EpromDialog::EpromDialog(wxWindow* parent)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"eprom_" + p_Main->getFontSize() + ".xrc");
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("EpromDialog"));

    wxString number;

    for (int i=0; i<5; i++)
    {
        number.Printf("%1d",i);
        epromRomDir_[i] = p_Main->getEpromRomDirectory(i);
    }

    XRCCTRL(*this, "Eprom0", wxComboBox)->SetValue(p_Main->getEpromRom(0));
    XRCCTRL(*this, "Eprom1", wxComboBox)->SetValue(p_Main->getEpromRom(1));
    XRCCTRL(*this, "Eprom2", wxComboBox)->SetValue(p_Main->getEpromRom(2));
    XRCCTRL(*this, "Eprom3", wxComboBox)->SetValue(p_Main->getEpromRom(3));
    XRCCTRL(*this, "Eprom4", wxComboBox)->SetValue(p_Main->getEpromRom(4));

}

void EpromDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    wxString number;

    for (int i=0; i<5; i++)
    {
        number.Printf("%1d",i);
        p_Main->setEpromRomDirectory(i, epromRomDir_[i]);
        p_Main->setEpromRom(i, XRCCTRL(*this, "Eprom"+number, wxComboBox)->GetValue());
    }
    EndModal( wxID_OK );
}

void EpromDialog::onEprom0Button(wxCommandEvent& WXUNUSED(event) )
{
    epromButton(0);
}

void EpromDialog::onEprom1Button(wxCommandEvent& WXUNUSED(event) )
{
    epromButton(1);
}

void EpromDialog::onEprom2Button(wxCommandEvent& WXUNUSED(event) )
{
    epromButton(2);
}

void EpromDialog::onEprom3Button(wxCommandEvent& WXUNUSED(event) )
{
    epromButton(3);
}

void EpromDialog::onEprom4Button(wxCommandEvent& WXUNUSED(event) )
{
    epromButton(4);
}

void EpromDialog::epromButton(int number)
{
    wxString stringNumber;
    wxString fileName;

    stringNumber.Printf("%1d", number);
    fileName = wxFileSelector( "Select the Eprom" + stringNumber +" file to load",
                               epromRomDir_[number], XRCCTRL(*this, "Eprom" + stringNumber, wxComboBox)->GetValue(),
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
    epromRomDir_[number] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

    XRCCTRL(*this, "Eprom" + stringNumber, wxComboBox)->SetValue(FullPath.GetFullName());
}

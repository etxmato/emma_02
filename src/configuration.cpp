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
#include "configuration.h"

#include "wx/xrc/xmlres.h"
#include "wx/combo.h"

BEGIN_EVENT_TABLE(ConfigurationDialog, wxDialog)
    EVT_BUTTON(XRCID("ConfigurationSave"), ConfigurationDialog::onSaveButton)
    EVT_TEXT(XRCID("configurationMenuName"), ConfigurationDialog::onMenuName)
    EVT_TEXT(XRCID("configurationSubMenuName"), ConfigurationDialog::onSubMenuName)
    EVT_COMBOBOX(XRCID("configurationSubMenuName"), ConfigurationDialog::onSubMenuName)
END_EVENT_TABLE()

ConfigurationDialog::ConfigurationDialog(wxWindow* parent, ConfigurationInfo configurationInfo, vector<ConfigurationSubMenuInfo> configurationSubMenuInfo, size_t configurationSubMenuInfoNumber)
{
	oldConfigurationInfo_ = configurationInfo;
    fileNameMenuPart_ = "";
    fileNameSubMenuPart_ = "";
    fileName_ = "";
	wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"configuration.xrc");
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("ConfigurationDialog"));
    
    XRCCTRL(*this, "configurationMenuName", wxTextCtrl)->SetValue(configurationInfo.menuName);
    XRCCTRL(*this, "configurationSubMenuName", wxComboBox)->SetValue(configurationInfo.subMenuName);
    
    for (size_t i=0; i<configurationSubMenuInfoNumber; i++)
        XRCCTRL(*this, "configurationSubMenuName", wxComboBox)->Append(configurationSubMenuInfo[i].subMenuName);
}

void ConfigurationDialog::onMenuName( wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName = XRCCTRL(*this, "configurationMenuName", wxTextCtrl)->GetValue();
    fileName.Replace(" - ", "_");
    fileName.Replace(" ", "_");
    fileName.Replace(".", "_");
    fileName.Replace("\\", "");
    fileName.Replace("/", "");
    fileName.Replace(":", "");
    fileName.Replace("*", "");
    fileName.Replace("?", "");
    fileName.Replace("\"", "");
    fileName.Replace("<", "");
    fileName.Replace(">", "");
    fileName.Replace("|", "");
    fileName.Replace("&", "and");
    fileName.LowerCase();
    
    fileNameMenuPart_ = fileName;
    if (fileNameSubMenuPart_ != "")
        fileName = fileNameSubMenuPart_ + "_" + fileNameMenuPart_;
    
    if (fileName != "")
        fileName_ = fileName+".ini";

//    XRCCTRL(*this, "configurationFilename", wxStaticText)->SetLabel(fileName_);
    
    if (XRCCTRL(*this, "configurationMenuName", wxTextCtrl)->GetValue() != "" && fileName != "_" && fileName != "")
        XRCCTRL(*this, "ConfigurationSave", wxButton)->Enable(true);
    else
        XRCCTRL(*this, "ConfigurationSave", wxButton)->Enable(false);
}

void ConfigurationDialog::onSubMenuName( wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName = XRCCTRL(*this, "configurationSubMenuName", wxComboBox)->GetValue();
    fileName.Replace(" - ", "_");
    fileName.Replace(" ", "_");
    fileName.Replace(".", "_");
    fileName.Replace("\\", "");
    fileName.Replace("/", "");
    fileName.Replace(":", "");
    fileName.Replace("*", "");
    fileName.Replace("?", "");
    fileName.Replace("\"", "");
    fileName.Replace("<", "");
    fileName.Replace(">", "");
    fileName.Replace("|", "");
    fileName.Replace("&", "and");
    fileName.LowerCase();
    
    fileNameSubMenuPart_ = fileName;
    if (fileNameSubMenuPart_ != "")
        fileName = fileNameSubMenuPart_ + "_" + fileNameMenuPart_;
    else
        fileName = fileNameMenuPart_;
    
    if (fileName != "")
        fileName_ = fileName+".ini";

//    XRCCTRL(*this, "configurationFilename", wxStaticText)->SetLabel(fileName_);

    if (XRCCTRL(*this, "configurationMenuName", wxTextCtrl)->GetValue() != "" && fileName != "_" && fileName != "")
        XRCCTRL(*this, "ConfigurationSave", wxButton)->Enable(true);
    else
        XRCCTRL(*this, "ConfigurationSave", wxButton)->Enable(false);
}

void ConfigurationDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    ConfigurationInfo configurationInfo;
    
    configurationInfo.fileName = fileName_;
    configurationInfo.menuName = XRCCTRL(*this, "configurationMenuName", wxTextCtrl)->GetValue();
    configurationInfo.subMenuName = XRCCTRL(*this, "configurationSubMenuName", wxComboBox)->GetValue();

    configurationInfo.menuName.Replace("&", "&&");
    
    if (configurationInfo.fileName != "")
    {
        int answer = p_Main->saveComputerConfig(configurationInfo, oldConfigurationInfo_);
        
        if (answer == wxYES)
            EndModal( wxID_OK );
    }    
}
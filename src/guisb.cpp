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
#include "guisb.h"

#include "wx/xrc/xmlres.h"             

BEGIN_EVENT_TABLE(SBDialog, wxDialog)
    EVT_BUTTON(XRCID("SBSave"), SBDialog::onSaveButton)
    EVT_CHOICE(XRCID("OnlineBMChoice"), SBDialog::onBMChoice)
    EVT_BUTTON(XRCID("OnlineBMStore"), SBDialog::onBMStore)
    EVT_CHOICE(XRCID("OnlineAliasChoice"), SBDialog::onAliasChoice)
    EVT_BUTTON(XRCID("OnlineAliasStore"), SBDialog::onAliasStore)
    EVT_BUTTON(XRCID("OnlineAliasDel"), SBDialog::onAliasDel)
    EVT_BUTTON(XRCID("SettingsRoot"), SBDialog::onRoot)
    EVT_TEXT(XRCID("OnlineBM"), SBDialog::onOnlineBMChange)
    EVT_TEXT(XRCID("OnlineAlias"), SBDialog::onOnlineAliasChange)
    EVT_TEXT(XRCID("OnlineAliasEmail"), SBDialog::onOnlineAliasChange)

    EVT_BUTTON(XRCID("BackupYes"), SBDialog::onYesButton)
    EVT_CHECKBOX(XRCID("BackupSub"), SBDialog::onSub)
    EVT_BUTTON(XRCID("BackupFolder"), SBDialog::onFolder)
END_EVENT_TABLE()

SBDialog::SBDialog(wxWindow* parent)
{
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"sb_" + p_Main->getFontSize() + ".xrc");
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("SBDialog"));

    rootDir_ = p_Main->getSbRootDirectory();

    wxString number;
    
    for (int i=0; i<11; i++)
    {
        number.Printf("%1d",i);
        SBRomDir_[i] = p_Main->getSbRomDirectory(i);
    }

    XRCCTRL(*this, "OnlinePlayer", wxTextCtrl)->SetValue(p_Main->getSbPlayer());
    XRCCTRL(*this, "OnlineLocation", wxTextCtrl)->SetValue(p_Main->getSbLocation());
    XRCCTRL(*this, "OnlineEmailAddress", wxTextCtrl)->SetValue(p_Main->getSbEmail());
    XRCCTRL(*this, "OnlineHome", wxTextCtrl)->SetValue(p_Main->getSbUrlHome());

    for (int i=0; i<10; i++)
        urlBookMark_[i] = p_Main->getUrlBookMark(i);

    XRCCTRL(*this, "OnlineBM", wxTextCtrl)->SetValue(urlBookMark_[0]);
#ifdef __WXMAC__
    XRCCTRL(*this, "OnlineBMStore", wxButton)->SetBackgroundColour(wxNullColour);
#else
    XRCCTRL(*this, "OnlineBMStore", wxButton)->SetForegroundColour(p_Main->getGuiTextColour(GUI_COL_BLACK));
#endif
    XRCCTRL(*this,"OnlineBMStore",wxButton)->Enable(false);
    bmNumber_ = 0;

    aliasNumber_ = 0;
    while (p_Main->getAlias(aliasNumber_) != "")
    {
        XRCCTRL(*this, "OnlineAliasChoice", wxChoice)->Append(p_Main->getAlias(aliasNumber_));
        aliasNumber_++;
    }
#ifdef __WXMAC__
    XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetBackgroundColour(wxNullColour);
#else
    XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetForegroundColour(p_Main->getGuiTextColour(GUI_COL_BLACK));
#endif
    XRCCTRL(*this,"OnlineAliasStore",wxButton)->Enable(false);

    aliasNumber_ = -1;

    XRCCTRL(*this, "SettingsRootTxt", wxTextCtrl)->SetValue(rootDir_);

    XRCCTRL(*this, "SettingsCdRoot", wxChoice)->SetSelection(p_Main->getSbCdRoot());
    XRCCTRL(*this, "SettingsBackup", wxChoice)->SetSelection(p_Main->getSbBackup());
    XRCCTRL(*this, "SettingsBackupSys", wxChoice)->SetSelection(p_Main->getSbBackupSys());
    XRCCTRL(*this, "SettingsCaseFile", wxChoice)->SetSelection(p_Main->getSbCaseFile());
    XRCCTRL(*this, "SettingsCaseDir", wxChoice)->SetSelection(p_Main->getSbCaseDir());

    if (p_Main->isComputerRunning())
    {
        XRCCTRL(*this, "SettingsRoot", wxButton)->Enable(false);
        XRCCTRL(*this, "SettingsRootTxt", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "BackupActivate", wxStaticText)->Hide();
    }
    else
    {
        XRCCTRL(*this, "BackupActivate", wxStaticText)->Enable(true);
        XRCCTRL(*this, "BackupFolder", wxButton)->Enable(false);
        XRCCTRL(*this, "BackupRootTxt", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "BackupList", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "BackupWarning", wxStaticText)->Enable(false);
        XRCCTRL(*this, "BackupSub", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "BackupYes", wxButton)->Enable(false);
    }

    XRCCTRL(*this, "BackupRootTxt", wxTextCtrl)->SetValue(rootDir_);
    sub_ = false;

    onListBackup();
}

void SBDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
    wxString number;

    p_Main->setSbPlayer(XRCCTRL(*this, "OnlinePlayer", wxTextCtrl)->GetValue());
    p_Main->setSbLocation(XRCCTRL(*this, "OnlineLocation", wxTextCtrl)->GetValue());
    p_Main->setSbEmail(XRCCTRL(*this, "OnlineEmailAddress", wxTextCtrl)->GetValue());
    p_Main->setSbUrlHome(XRCCTRL(*this, "OnlineHome", wxTextCtrl)->GetValue());

    p_Main->setConfigItem("/Comx/PlayerName", XRCCTRL(*this, "OnlinePlayer", wxTextCtrl)->GetValue());
    p_Main->setConfigItem("/Comx/PlayerLocation", XRCCTRL(*this, "OnlineLocation", wxTextCtrl)->GetValue());
    p_Main->setConfigItem("/Comx/Email", XRCCTRL(*this, "OnlineEmailAddress", wxTextCtrl)->GetValue());
    p_Main->setConfigItem("/Comx/HomePage", XRCCTRL(*this, "OnlineHome", wxTextCtrl)->GetValue());

    urlBookMark_[bmNumber_] = XRCCTRL(*this, "OnlineBM", wxTextCtrl)->GetValue();
    p_Main->setUrlBookMark(bmNumber_, urlBookMark_[bmNumber_]); 

    for (int i=0; i<10; i++)
    {
        wxString numberStr;
        numberStr.Printf("/Comx/BookMark%d",i);
        p_Main->setConfigItem(numberStr, urlBookMark_[i]);
    }

    p_Main->setSbRootDirectory(XRCCTRL(*this, "SettingsRootTxt", wxTextCtrl)->GetValue());
    p_Main->setSbCdRoot(XRCCTRL(*this, "SettingsCdRoot", wxChoice)->GetSelection());
    p_Main->getSbBackup(XRCCTRL(*this, "SettingsBackup", wxChoice)->GetSelection());
    p_Main->getSbBackupSys(XRCCTRL(*this, "SettingsBackupSys", wxChoice)->GetSelection());
    p_Main->getSbCaseFile(XRCCTRL(*this, "SettingsCaseFile", wxChoice)->GetSelection());
    p_Main->setSbCaseDir(XRCCTRL(*this, "SettingsCaseDir", wxChoice)->GetSelection());

    wxString alias, aliasEmail;

    alias = XRCCTRL(*this, "OnlineAlias", wxTextCtrl)->GetValue();
    aliasEmail = XRCCTRL(*this, "OnlineAliasEmail", wxTextCtrl)->GetValue();
    if (alias != "alias" && aliasEmail != "email address")
        p_Main->setAlias(aliasNumber_, alias, aliasEmail); 

    wxString aliasStr, aliasEmailStr;
    for (size_t i=0; i<p_Main->getNumberOfAlias(); i++)
    {
        aliasStr.Printf("/Comx/Alias%d", (int)i);
        aliasEmailStr.Printf("/Comx/AliasEmail%d", (int)i);

        p_Main->setConfigItem(aliasStr, p_Main->getAlias(i));
        p_Main->setConfigItem(aliasEmailStr, p_Main->getAliasEmail(i));
    }
    p_Main->setConfigItem("/Comx/AliasNumberOf", (int)p_Main->getNumberOfAlias());

    EndModal( wxID_OK );
}


void SBDialog::onBMChoice(wxCommandEvent&event)
{
    bmNumber_ = event.GetSelection();
    XRCCTRL(*this, "OnlineBM", wxTextCtrl)->SetValue(urlBookMark_[bmNumber_]);
#ifdef __WXMAC__
    XRCCTRL(*this, "OnlineBMStore", wxButton)->SetBackgroundColour(wxNullColour);
#else
    XRCCTRL(*this, "OnlineBMStore", wxButton)->SetForegroundColour(p_Main->getGuiTextColour(GUI_COL_BLACK));
#endif
    XRCCTRL(*this,"OnlineBMStore",wxButton)->Enable(false);
}

void SBDialog::onBMStore(wxCommandEvent& WXUNUSED(event) )
{
    XRCCTRL(*this,"OnlineBMStore",wxButton)->Enable(false);
#ifdef __WXMAC__
    XRCCTRL(*this, "OnlineBMStore", wxButton)->SetBackgroundColour(wxNullColour);
#else
    XRCCTRL(*this, "OnlineBMStore", wxButton)->SetForegroundColour(p_Main->getGuiTextColour(GUI_COL_BLACK));
#endif
    urlBookMark_[bmNumber_] = XRCCTRL(*this, "OnlineBM", wxTextCtrl)->GetValue();
    p_Main->setUrlBookMark(bmNumber_, urlBookMark_[bmNumber_]); 
}

void SBDialog::onAliasChoice(wxCommandEvent&event)
{
    aliasNumber_ = event.GetSelection()-1;

    if (aliasNumber_ == -1)
    {
        XRCCTRL(*this, "OnlineAlias", wxTextCtrl)->SetValue("alias");
        XRCCTRL(*this, "OnlineAliasEmail", wxTextCtrl)->SetValue("email address");
    }
    else
    {
        XRCCTRL(*this, "OnlineAlias", wxTextCtrl)->SetValue(p_Main->getAlias(aliasNumber_));
        XRCCTRL(*this, "OnlineAliasEmail", wxTextCtrl)->SetValue(p_Main->getAliasEmail(aliasNumber_));
    }
#ifdef __WXMAC__
    XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetBackgroundColour(wxNullColour);
#else
    XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetForegroundColour(p_Main->getGuiTextColour(GUI_COL_BLACK));
#endif
    XRCCTRL(*this,"OnlineAliasStore",wxButton)->Enable(false);
}

void SBDialog::onAliasStore(wxCommandEvent& WXUNUSED(event) )
{
    wxString alias, aliasEmail;

    alias = XRCCTRL(*this, "OnlineAlias", wxTextCtrl)->GetValue();
    aliasEmail = XRCCTRL(*this, "OnlineAliasEmail", wxTextCtrl)->GetValue();
    p_Main->setAlias(aliasNumber_, alias, aliasEmail); 
    if (aliasNumber_ == -1)
        XRCCTRL(*this, "OnlineAliasChoice", wxChoice)->Append(alias);

    XRCCTRL(*this, "OnlineAlias", wxTextCtrl)->SetValue("alias");
    XRCCTRL(*this, "OnlineAliasEmail", wxTextCtrl)->SetValue("email address");

#ifdef __WXMAC__
    XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetBackgroundColour(wxNullColour);
#else
    XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetForegroundColour(p_Main->getGuiTextColour(GUI_COL_BLACK));
#endif
    XRCCTRL(*this,"OnlineAliasStore",wxButton)->Enable(false);
}

void SBDialog::onAliasDel(wxCommandEvent& WXUNUSED(event) )
{
    if (aliasNumber_ != -1)
    {
        p_Main->deleteAlias(aliasNumber_);
        XRCCTRL(*this, "OnlineAliasChoice", wxChoice)->Delete(aliasNumber_+1);

        XRCCTRL(*this, "OnlineAlias", wxTextCtrl)->SetValue("alias");
        XRCCTRL(*this, "OnlineAliasEmail", wxTextCtrl)->SetValue("email address");

#ifdef __WXMAC__
        XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetBackgroundColour(wxNullColour);
#else
        XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetForegroundColour(p_Main->getGuiTextColour(GUI_COL_BLACK));
#endif
        XRCCTRL(*this,"OnlineAliasStore",wxButton)->Enable(false);

        aliasNumber_ = -1;
    }
}

void SBDialog::onOnlineBMChange(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this,"OnlineBMStore",wxButton)->Enable(true);
#ifdef __WXMAC__
    XRCCTRL(*this, "OnlineBMStore", wxButton)->SetBackgroundColour(p_Main->getGuiTextColour(GUI_COL_RED));
#else
    XRCCTRL(*this, "OnlineBMStore", wxButton)->SetForegroundColour(p_Main->getGuiTextColour(GUI_COL_RED));
#endif
}

void SBDialog::onOnlineAliasChange(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this,"OnlineAliasStore",wxButton)->Enable(true);
#ifdef __WXMAC__
    XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetBackgroundColour(p_Main->getGuiTextColour(GUI_COL_RED));
#else
    XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetForegroundColour(p_Main->getGuiTextColour(GUI_COL_RED));
#endif
}

void SBDialog::onRoot(wxCommandEvent& WXUNUSED(event))
{
    wxString dirName = wxDirSelector( "Select the COMX SB root folder", XRCCTRL(*this, "SettingsRootTxt", wxTextCtrl)->GetValue());
    if (!dirName)
        return;

    rootDir_ = dirName + wxFileName::GetPathSeparator(wxPATH_NATIVE);
    XRCCTRL(*this, "SettingsRootTxt", wxTextCtrl)->SetValue(rootDir_);
}

void SBDialog::onYesButton( wxCommandEvent& WXUNUSED(event) )
{
    p_Computer->onBackupYes(rootDir_, sub_);
    EndModal( wxID_OK );
}

void SBDialog::onNoButton(wxCommandEvent& WXUNUSED(event))
{

    EndModal( wxID_OK );
}

void SBDialog::onSub(wxCommandEvent&event)
{
    sub_ = event.IsChecked();
    XRCCTRL(*this, "BackupList", wxTextCtrl)->Clear();

    onListBackup();
}

void SBDialog::onFolder(wxCommandEvent& WXUNUSED(event))
{
    wxString dirName = wxDirSelector( "Select the desired folder", rootDir_);
    if (dirName.empty() )
        return;

    XRCCTRL(*this, "BackupRootTxt", wxTextCtrl)->SetValue(dirName + p_Main->getPathSep());
    XRCCTRL(*this, "BackupSub", wxCheckBox)->SetValue(false);
    XRCCTRL(*this, "BackupList", wxTextCtrl)->Clear();
    rootDir_ = dirName+p_Main->getPathSep();
    sub_ = false;

    onListBackup();
}

void SBDialog::listBackup(wxFileName directory, wxString name, wxString ext, int flags)
{
    wxString fileName, fullName;
    bool fileFound;
    int number;

    wxDir *dir;

    dir = new wxDir (directory.GetLongPath());
    fileFound = dir->GetFirst(&fileName, name+"*" + ext + ".bak", flags);

    while (fileFound)
    {
        fullName = directory.GetLongPath() + fileName;
        number = 1;
        while (fullName.Left(number) == rootDir_.Left(number))
            number++;
        if (!first_)
            XRCCTRL(*this, "BackupList", wxTextCtrl)->AppendText("\n");
        else
            first_ = false;
        XRCCTRL(*this, "BackupList", wxTextCtrl)->AppendText(fullName.Right(fullName.Len()-number+1));

        fileFound = dir->GetNext(&fileName);
    }

    delete dir;
}

void SBDialog::listAllBackup(wxString directory)
{
    wxString dirName;
    bool dirFound;
    wxFileName directoryName;

    wxDir *dir;

    directoryName = wxFileName(directory);
    dir = new wxDir (directoryName.GetLongPath());

    listBackup(directoryName, "", ".*", wxDIR_FILES);

    if (sub_)
    {
    
        dirFound = dir->GetFirst(&dirName, "", wxDIR_DIRS);

        while (dirFound)
        {

            listAllBackup(directory+dirName+p_Main->getPathSep());
            dirFound = dir->GetNext(&dirName);
        }
    }
    delete dir;
}

void SBDialog::onListBackup()
{
    first_ = true;
    listAllBackup(rootDir_);
}

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
    EVT_BUTTON(XRCID("SB_ROM_00_Button"), SBDialog::onSB0Button)
    EVT_BUTTON(XRCID("SB_ROM_01_Button"), SBDialog::onSB1Button)
    EVT_BUTTON(XRCID("SB_ROM_02_Button"), SBDialog::onSB2Button)
    EVT_BUTTON(XRCID("SB_ROM_03_Button"), SBDialog::onSB3Button)
    EVT_BUTTON(XRCID("SB_ROM_04_Button"), SBDialog::onSB4Button)
    EVT_BUTTON(XRCID("SB_ROM_05_Button"), SBDialog::onSB5Button)
    EVT_BUTTON(XRCID("SB_ROM_06_Button"), SBDialog::onSB6Button)
    EVT_BUTTON(XRCID("SB_ROM_07_Button"), SBDialog::onSB7Button)
    EVT_BUTTON(XRCID("SB_ROM_08_Button"), SBDialog::onSB8Button)
    EVT_BUTTON(XRCID("SB_ROM_09_Button"), SBDialog::onSB9Button)
    EVT_BUTTON(XRCID("SB_ROM_10_Button"), SBDialog::onSB10Button)
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

	XRCCTRL(*this, "SB_ROM_00", wxComboBox)->SetValue(p_Main->getSbRom(0));
	XRCCTRL(*this, "SB_ROM_01", wxComboBox)->SetValue(p_Main->getSbRom(1));
	XRCCTRL(*this, "SB_ROM_02", wxComboBox)->SetValue(p_Main->getSbRom(2));
	XRCCTRL(*this, "SB_ROM_03", wxComboBox)->SetValue(p_Main->getSbRom(3));
	XRCCTRL(*this, "SB_ROM_04", wxComboBox)->SetValue(p_Main->getSbRom(4));
	XRCCTRL(*this, "SB_ROM_05", wxComboBox)->SetValue(p_Main->getSbRom(5));
	XRCCTRL(*this, "SB_ROM_06", wxComboBox)->SetValue(p_Main->getSbRom(6));
	XRCCTRL(*this, "SB_ROM_07", wxComboBox)->SetValue(p_Main->getSbRom(7));
	XRCCTRL(*this, "SB_ROM_08", wxComboBox)->SetValue(p_Main->getSbRom(8));
	XRCCTRL(*this, "SB_ROM_09", wxComboBox)->SetValue(p_Main->getSbRom(9));
	XRCCTRL(*this, "SB_ROM_10", wxComboBox)->SetValue(p_Main->getSbRom(10));

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
	XRCCTRL(*this, "OnlineBMStore", wxButton)->SetForegroundColour(*wxBLACK);
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
	XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetForegroundColour(*wxBLACK);
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
		XRCCTRL(*this, "SB_ROMS", wxPanel)->Enable(false);
		XRCCTRL(*this, "SettingsRoot", wxButton)->Enable(false);
		XRCCTRL(*this, "SettingsRootTxt", wxTextCtrl)->Enable(false);
	}
		XRCCTRL(*this, "SB_Backup", wxPanel)->Enable(false);

	XRCCTRL(*this, "BackupRootTxt", wxTextCtrl)->SetValue(rootDir_);
	sub_ = false;

	onListBackup();
}

void SBDialog::onSaveButton( wxCommandEvent& WXUNUSED(event) )
{
	wxString number;

	for (int i=0; i<11; i++)
	{
		number.Printf("%02d",i);
        p_Main->setSbRomDirectory(i, SBRomDir_[i]);
        p_Main->setSbRom(i, XRCCTRL(*this, "SB_ROM_"+number, wxComboBox)->GetValue());
	}

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

	p_Main->diagSbChange();

	EndModal( wxID_OK );
}


void SBDialog::onBMChoice(wxCommandEvent&event)
{
	bmNumber_ = event.GetSelection();
	XRCCTRL(*this, "OnlineBM", wxTextCtrl)->SetValue(urlBookMark_[bmNumber_]);
#ifdef __WXMAC__
    XRCCTRL(*this, "OnlineBMStore", wxButton)->SetBackgroundColour(wxNullColour);
#else
	XRCCTRL(*this, "OnlineBMStore", wxButton)->SetForegroundColour(*wxBLACK);
#endif
    XRCCTRL(*this,"OnlineBMStore",wxButton)->Enable(false);
}

void SBDialog::onBMStore(wxCommandEvent& WXUNUSED(event) )
{
	XRCCTRL(*this,"OnlineBMStore",wxButton)->Enable(false);
#ifdef __WXMAC__
    XRCCTRL(*this, "OnlineBMStore", wxButton)->SetBackgroundColour(wxNullColour);
#else
	XRCCTRL(*this, "OnlineBMStore", wxButton)->SetForegroundColour(*wxBLACK);
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
	XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetForegroundColour(*wxBLACK);
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
	XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetForegroundColour(*wxBLACK);
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
		XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetForegroundColour(*wxBLACK);
#endif
        XRCCTRL(*this,"OnlineAliasStore",wxButton)->Enable(false);

		aliasNumber_ = -1;
	}
}

void SBDialog::onSB0Button(wxCommandEvent& WXUNUSED(event) )
{
	SBButton(0, "Main ROM replacement");
}

void SBDialog::onSB1Button(wxCommandEvent& WXUNUSED(event) )
{
	SBButton(1, "Expansion ROM at E000-EFFF");
}

void SBDialog::onSB2Button(wxCommandEvent& WXUNUSED(event) )
{
	SBButton(2, "Expansion ROM at F800-FFFF");
}

void SBDialog::onSB3Button(wxCommandEvent& WXUNUSED(event) )
{
	SBButton(3, "BANK 0");
}

void SBDialog::onSB4Button(wxCommandEvent& WXUNUSED(event) )
{
	SBButton(4, "BANK 1");
}

void SBDialog::onSB5Button(wxCommandEvent& WXUNUSED(event) )
{
	SBButton(5, "BANK 2");
}

void SBDialog::onSB6Button(wxCommandEvent& WXUNUSED(event) )
{
	SBButton(6, "BANK 3");
}

void SBDialog::onSB7Button(wxCommandEvent& WXUNUSED(event) )
{
	SBButton(7, "BANK 4");
}

void SBDialog::onSB8Button(wxCommandEvent& WXUNUSED(event) )
{
	SBButton(8, "BANK 5");
}

void SBDialog::onSB9Button(wxCommandEvent& WXUNUSED(event) )
{
	SBButton(9, "BANK 6");
}

void SBDialog::onSB10Button(wxCommandEvent& WXUNUSED(event) )
{
	SBButton(10, "BANK 7");
}

void SBDialog::SBButton(int number, wxString textMessage)
{
	wxString stringNumber;
	wxString fileName;

	stringNumber.Printf("%02d", number);
	fileName = wxFileSelector( "Select the SB " + textMessage +" file to load",
                               SBRomDir_[number], XRCCTRL(*this, "SB_ROM_" + stringNumber, wxComboBox)->GetValue(),
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
	SBRomDir_[number] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	XRCCTRL(*this, "SB_ROM_" + stringNumber, wxComboBox)->SetValue(FullPath.GetFullName());
}

void SBDialog::onOnlineBMChange(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this,"OnlineBMStore",wxButton)->Enable(true);
#ifdef __WXMAC__
    XRCCTRL(*this, "OnlineBMStore", wxButton)->SetBackgroundColour(*wxRED);
#else
	XRCCTRL(*this, "OnlineBMStore", wxButton)->SetForegroundColour(*wxRED);
#endif
}

void SBDialog::onOnlineAliasChange(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this,"OnlineAliasStore",wxButton)->Enable(true);
#ifdef __WXMAC__
    XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetBackgroundColour(*wxRED);
#else
	XRCCTRL(*this, "OnlineAliasStore", wxButton)->SetForegroundColour(*wxRED);
#endif
}

void SBDialog::onRoot(wxCommandEvent& WXUNUSED(event))
{
	wxString dirName = wxDirSelector( "Select the COMX root folder", XRCCTRL(*this, "SettingsRootTxt", wxTextCtrl)->GetValue());
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

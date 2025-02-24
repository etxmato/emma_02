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
#include "wx/tglbtn.h"
#include "wx/utils.h"

#include "main.h"
#include "guicomx.h"
#include "printer.h"
#include "guisb.h"

DEFINE_EVENT_TYPE(STATUS_LED_ON)
DEFINE_EVENT_TYPE(STATUS_LED_OFF)
DEFINE_EVENT_TYPE(EXP_LED_ON)
DEFINE_EVENT_TYPE(EXP_LED_OFF)
DEFINE_EVENT_TYPE(STATUS_BAR_1870)

BEGIN_EVENT_TABLE(GuiComx, GuiXml)

    EVT_COMMAND(wxID_ANY, STATUS_LED_ON, GuiComx::statusLedOn)
    EVT_COMMAND(wxID_ANY, STATUS_LED_OFF, GuiComx::statusLedOff)
    EVT_COMMAND(wxID_ANY, STATUS_BAR_1870, GuiComx::v1870BarSize)
    EVT_COMMAND(wxID_ANY, EXP_LED_ON, GuiComx::expLedOn)
    EVT_COMMAND(wxID_ANY, EXP_LED_OFF, GuiComx::expLedOff)

END_EVENT_TABLE()

GuiComx::GuiComx(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiXml(title, pos, size, mode, dataDir, iniDir)
{
}

GuiComx::~GuiComx()
{
}

void GuiComx::statusLedOnEvent()
{
    if (computerConfiguration.vis1870Configuration.statusBarType != STATUSBAR_COMX)
        return;
    
    if (wxIsMainThread())
    {
        statusLedOnDirect();
        return;
    }
    wxCommandEvent event(STATUS_LED_ON, 801);
    
    event.SetEventObject(this);
    wxPostEvent(this, event);
    while(!isStatusLedOn_)
        p_Computer->sleepComputer(1);
}

void GuiComx::statusLedOffEvent()
{
    if (computerConfiguration.vis1870Configuration.statusBarType != STATUSBAR_COMX)
        return;
    
    if (wxIsMainThread())
    {
        statusLedOffDirect();
        return;
    }
    wxCommandEvent event(STATUS_LED_OFF, 802);

    event.SetEventObject(this);
    wxPostEvent(this, event);
    while(isStatusLedOn_)
        p_Computer->sleepComputer(1);
}

void GuiComx::expLedOnEvent()
{
    if (computerConfiguration.vis1870Configuration.statusBarType != STATUSBAR_COMX)
        return;

    if (wxIsMainThread() || !slotLedUpdate_)
    {
        expLedOnDirect();
        return;
    }
    if (isExpLedOn_)  return;
    wxCommandEvent event(EXP_LED_ON, 805);
    event.SetEventObject(this);
    wxPostEvent(this, event);
    while(!isExpLedOn_)
        p_Computer->sleepComputer(1);
}

void GuiComx::expLedOffEvent()
{
    if (computerConfiguration.vis1870Configuration.statusBarType != STATUSBAR_COMX)
        return;

    if (wxIsMainThread() || !slotLedUpdate_)
    {
        expLedOffDirect();
        return;
    }
    if (!isExpLedOn_)  return;
    wxCommandEvent event(EXP_LED_OFF, 806);
    event.SetEventObject(this);
    wxPostEvent(this, event);
    while(isExpLedOn_)
        p_Computer->sleepComputer(1);
}

void GuiComx::v1870BarSizeEvent()
{
    wxCommandEvent event(STATUS_BAR_1870, 803);
    event.SetEventObject(this);
    wxPostEvent(this, event);
}

wxString GuiComx::getPL80Data(int item)
{
    return Pl80Data_[item];
}

void GuiComx::setPL80Data(int item, wxString data)
{
    Pl80Data_[item] = data;
}

void GuiComx::statusLedOn(wxCommandEvent&WXUNUSED(event))
{
    statusLedOnDirect();
}

void GuiComx::statusLedOnDirect()
{
    p_Video[computerConfiguration.vis1870Configuration.videoNumber]->updateComxStatusLed(true);
    isStatusLedOn_ = true;
}

void GuiComx::statusLedOff(wxCommandEvent&WXUNUSED(event))
{
    statusLedOffDirect();
}


void GuiComx::statusLedOffDirect()
{
    p_Video[computerConfiguration.vis1870Configuration.videoNumber]->updateComxStatusLed(false);
    isStatusLedOn_ = false;
}

void GuiComx::expLedOn(wxCommandEvent&WXUNUSED(event))
{
    expLedOnDirect();
}

void GuiComx::expLedOnDirect()
{
    p_Video[computerConfiguration.vis1870Configuration.videoNumber]->updateComxExpansionLed(true);
    isExpLedOn_ = true;
}

void GuiComx::expLedOff(wxCommandEvent&WXUNUSED(event))
{
    expLedOffDirect();
}

void GuiComx::expLedOffDirect()
{
    p_Video[computerConfiguration.vis1870Configuration.videoNumber]->updateComxExpansionLed(false);
    isExpLedOn_ = false;
}

void GuiComx::v1870BarSize(wxCommandEvent&WXUNUSED(event))
{
    p_Video[computerConfiguration.vis1870Configuration.videoNumber]->reDrawBar();
}

void GuiComx::setFandMBasicGui()
{
    if (!mode_.gui)  return;
    if (p_Computer->isFAndMBasicRunning())
    {
        XRCCTRL(*this, "RunButtonXml", wxButton)->SetLabel("F&&M RUN");
        XRCCTRL(*this, "SaveButtonXml", wxButton)->SetLabel("F&&M SAVE");
        XRCCTRL(*this, "LoadButtonXml", wxButton)->SetLabel("F&&M LOAD");
    }
    else
    {
        XRCCTRL(*this, "RunButtonXml", wxButton)->SetLabel("RUN");
        XRCCTRL(*this, "SaveButtonXml", wxButton)->SetLabel("SAVE");
        XRCCTRL(*this, "LoadButtonXml", wxButton)->SetLabel("LOAD");
    }
}

void GuiComx::onSBDialog(wxCommandEvent&WXUNUSED(event))
{
    SBDialog SBDialog(this);
    SBDialog.ShowModal();
}

void GuiComx::readSbConfig()
{
    wxString name=wxGetUserName();
    sbPlayer_ = configPointer->Read("/Comx/PlayerName", name);
    sbLocation_ = configPointer->Read("/Comx/PlayerLocation", "");
    sbEmail_ = configPointer->Read("/Comx/Email", "");
    sbUrlHome_ = configPointer->Read("/Comx/HomePage", "http://www.emma02.hobby-site.com");
    
    sbUrlBookMark_[0] = configPointer->Read("/Comx/BookMark0", "http://www.comx35.com/comx35sbhs/hs2.php");
    sbUrlBookMark_[1] = configPointer->Read("/Comx/BookMark1", "http://www.comx35.com/comx35.html");
    sbUrlBookMark_[2] = configPointer->Read("/Comx/BookMark2", "http://www.comx35.com/superboard.html");
    sbUrlBookMark_[3] = configPointer->Read("/Comx/BookMark3", "http://www.emma02.hobby-site.com/comx.htm");
    sbUrlBookMark_[4] = configPointer->Read("/Comx/BookMark4", "http://www.emma02.hobby-site.com/basic_comx.htm");
    sbUrlBookMark_[5] = configPointer->Read("/Comx/BookMark5", "http://www.elf-emulation.com/");
    sbUrlBookMark_[6] = configPointer->Read("/Comx/BookMark6", "http://www.cosmacelf.com/");
    sbUrlBookMark_[7] = configPointer->Read("/Comx/BookMark7", "http://www.sparetimegizmos.com/");
    sbUrlBookMark_[8] = configPointer->Read("/Comx/BookMark8", "http://www.cosmacvip.com/");
    sbUrlBookMark_[9] = configPointer->Read("/Comx/BookMark9", "http://www.comxclub.hobby-site.com/");
    
    numberOfAlias_ = 0;
    wxString aliasStr, aliasEmailStr;
    
    numberOfAlias_ = configPointer->Read("/Comx/AliasNumberOf", 0l);
    alias.resize(numberOfAlias_);
    aliasEmail.resize(numberOfAlias_);
    
    for (size_t number=0; number < numberOfAlias_; number++)
    {
        aliasStr.Printf("/Comx/Alias%d", (int)number);
        aliasEmailStr.Printf("/Comx/AliasEmail%d", (int)number);
        
        alias[number] = configPointer->Read(aliasStr);
        aliasEmail[number] = configPointer->Read(aliasEmailStr);
    }

    sbFwVersion_ = (int)configPointer->Read("/Comx/FwVersion", 0l);

    sbRootDirectory_ = readConfigDir("/Dir/Comx/RootDirectory", dataDir_ + "Comx" + pathSeparator_);
    sbCdRoot_ = (int)configPointer->Read("/Comx/CdRoot", 0l);
    sbBackup_ = (int)configPointer->Read("/Comx/Backup", 0l);
    sbBackupSys_ = (int)configPointer->Read("/Comx/BackupSys", 1l);
    sbCaseFile_ = (int)configPointer->Read("/Comx/CaseFile", 0l);
    sbCaseDir_ = (int)configPointer->Read("/Comx/CaseDir", 2l);
    
}

void GuiComx::writeSbConfig()
{
    writeConfigDir("/Dir/Comx/RootDirectory", sbRootDirectory_);
    configPointer->Write("/Comx/CdRoot", sbCdRoot_);
    configPointer->Write("/Comx/Backup", sbBackup_);
    configPointer->Write("/Comx/BackupSys", sbBackupSys_);
    configPointer->Write("/Comx/CaseFile", sbCaseFile_);
    configPointer->Write("/Comx/CaseDir", sbCaseDir_);
}

void GuiComx::setSbUrlHome(wxString urlHome)
{
    sbUrlHome_ = urlHome;
    p_Main->setConfigItem("/Comx/HomePage", sbUrlHome_);
} 

void GuiComx::setSbFwVersion(int version)
{
    sbFwVersion_ = version;
    p_Main->setConfigItem("/Comx/FwVersion", sbFwVersion_);
} 

void GuiComx::setSbUrlBookMark(int number, wxString urlBookMark)
{
    sbUrlBookMark_[number] = urlBookMark;
    
    wxString numberStr;
    numberStr.Printf("/Comx/BookMark%d",number);
    p_Main->setConfigItem(numberStr, sbUrlBookMark_[number]);
} 

void GuiComx::setUrlBookMark(int number, wxString urlBookMark)
{
    sbUrlBookMark_[number] = urlBookMark;
}

void GuiComx::setAlias(long number, wxString aliasNew, wxString aliasEmailNew)
{
    if (number == -1)
    {
        number = numberOfAlias_;
        numberOfAlias_++;
        alias.resize(numberOfAlias_);
        aliasEmail.resize(numberOfAlias_);
    }

    alias[number] = aliasNew;
    aliasEmail[number] = aliasEmailNew;
}

void GuiComx::deleteAlias(int aliasDel)
{
    for (int number = aliasDel+1; number<(int)numberOfAlias_; number++)
    {
        alias[aliasDel] = alias[number];
        aliasEmail[aliasDel] = aliasEmail[number];
        aliasDel++;
    }
    numberOfAlias_--;
    alias.resize(numberOfAlias_);
    aliasEmail.resize(numberOfAlias_);
}

wxString GuiComx::getAlias(size_t number) 
{
    if (number >= numberOfAlias_)
        return "";
    else
        return alias[number];
}

wxString GuiComx::getAliasEmail(size_t number) 
{
    if (number >= numberOfAlias_)
        return "";
    else
        return aliasEmail[number];
}

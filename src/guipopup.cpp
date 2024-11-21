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
#include "guipopup.h"

#include "wx/xrc/xmlres.h"             

BEGIN_EVENT_TABLE(PopupDialog, wxDialog)
    EVT_BUTTON(XRCID("CasButton"), PopupDialog::onCassette)
    EVT_TEXT(XRCID("WavFile"), PopupDialog::onCassetteText)
    EVT_BUTTON(XRCID("EjectCas"), PopupDialog::onCassetteEject)
    EVT_BUTTON(XRCID("CasButton1"), PopupDialog::onCassette1)
    EVT_TEXT(XRCID("WavFile1"), PopupDialog::onCassetteText1)
    EVT_BUTTON(XRCID("EjectCas1"), PopupDialog::onCassetteEject1)
    EVT_BUTTON(XRCID("RunButton"), PopupDialog::onLoadRunButton)
    EVT_BUTTON(XRCID("LoadButton"), PopupDialog::onLoadButton)
    EVT_BUTTON(XRCID("SaveButton"), PopupDialog::onSaveButton)
    EVT_BUTTON(XRCID("DsaveButton"), PopupDialog::onDataSaveButton)
    EVT_CHECKBOX(XRCID("UseLocation"), PopupDialog::onUseLocation)
    EVT_TEXT(XRCID("SaveStart"), PopupDialog::onSaveStart)
    EVT_TEXT(XRCID("SaveEnd"), PopupDialog::onSaveEnd)
    EVT_TEXT(XRCID("SaveExec"), PopupDialog::onSaveExec)

    EVT_BUTTON(XRCID("FDC0_ButtonXml"), PopupDialog::onDisk0)
    EVT_TEXT(XRCID("FDC0_FileXml"), PopupDialog::onDiskText0)
    EVT_BUTTON(XRCID("Eject_FDC0Xml"), PopupDialog::onDiskEject0)
    EVT_BUTTON(XRCID("FDC0_SwitchXml"), PopupDialog::onDiskDirSwitch0)
    EVT_BUTTON(XRCID("FDC1_ButtonXml"), PopupDialog::onDisk1)
    EVT_TEXT(XRCID("FDC1_FileXml"), PopupDialog::onDiskText1)
    EVT_BUTTON(XRCID("Eject_FDC1Xml"), PopupDialog::onDiskEject1)
    EVT_BUTTON(XRCID("FDC1_SwitchXml"), PopupDialog::onDiskDirSwitch1)
    EVT_BUTTON(XRCID("FDC2_ButtonXml"), PopupDialog::onDisk2)
    EVT_TEXT(XRCID("FDC2_FileXml"), PopupDialog::onDiskText2)
    EVT_BUTTON(XRCID("Eject_FDC2Xml"), PopupDialog::onDiskEject2)
    EVT_BUTTON(XRCID("FDC2_SwitchXml"), PopupDialog::onDiskDirSwitch2)
    EVT_BUTTON(XRCID("FDC3_ButtonXml"), PopupDialog::onDisk3)
    EVT_TEXT(XRCID("FDC3_FileXml"), PopupDialog::onDiskText3)
    EVT_BUTTON(XRCID("Eject_FDC3Xml"), PopupDialog::onDiskEject3)
    EVT_BUTTON(XRCID("FDC3_SwitchXml"), PopupDialog::onDiskDirSwitch3)

    EVT_CHECKBOX(XRCID("ControlWindowsPopupXml"), PopupDialog::onXmlControlWindows)

    EVT_BUTTON(XRCID("XmodemButton"), PopupDialog::onXmodem)
    EVT_BUTTON(XRCID("EjectXmodem"), PopupDialog::onXmodemEject)
    EVT_TEXT(XRCID("XmodemFile"), PopupDialog::onXmodemText)

    EVT_BUTTON(XRCID("OK"), PopupDialog::onOk)
END_EVENT_TABLE()

PopupDialog::PopupDialog(wxWindow* parent)
{
    loadingGui_ = true;
    
    currentComputerConfiguration = p_Main->getConfiguration();
    wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"menuXml_" + p_Main->getFontSize() + ".xrc");
    wxXmlResource::Get()->LoadDialog(this, parent, "PopupXml");
    XRCCTRL(*this, "PopupXml", wxDialog)->SetLabel(p_Main->getSelectedComputerText()+" Menu");

    loadingGui_ = false;
}

PopupDialog::~PopupDialog()
{
}

void PopupDialog::init()
{
    setTape1Type(currentComputerConfiguration.swTapeConfiguration.twoDecks);
    XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(0));

    XRCCTRL(*this, "XmodemButton", wxButton)->Enable(currentComputerConfiguration.videoTerminalConfiguration.xModem_defined || currentComputerConfiguration.videoTerminalConfiguration.hexModem_defined);
    XRCCTRL(*this, "XmodemFile", wxTextCtrl)->Enable(currentComputerConfiguration.videoTerminalConfiguration.xModem_defined || currentComputerConfiguration.videoTerminalConfiguration.hexModem_defined);
    XRCCTRL(*this, "EjectXmodem", wxBitmapButton)->Enable(currentComputerConfiguration.videoTerminalConfiguration.xModem_defined || currentComputerConfiguration.videoTerminalConfiguration.hexModem_defined);
    if (currentComputerConfiguration.videoTerminalConfiguration.hexModem_defined)
        XRCCTRL(*this, "XmodemButton", wxButton)->SetLabel("HEX");
    else
        XRCCTRL(*this, "XmodemButton", wxButton)->SetLabel("XMODEM");
    
    XRCCTRL(*this, "ControlWindowsPopupXml", wxCheckBox)->Enable(currentComputerConfiguration.frontPanelConfiguration[PANEL_MAIN].defined);
    XRCCTRL(*this, "ControlWindowsPopupXml", wxCheckBox)->SetValue(currentComputerConfiguration.frontPanelConfiguration[PANEL_MAIN].show);
    setLocation(p_Main->getUseLoadLocation(), p_Main->getSaveStartString(), p_Main->getSaveEndString(), p_Main->getSaveExecString());

    for (int drive=0; drive <4; drive++)
        setUpdFloppyGui(drive);
}

void PopupDialog::setLocation(bool state, wxString saveStart, wxString saveEnd, wxString saveExec)
{
    wxString printBuffer;
    if (state)
    {
        XRCCTRL(*this,"SaveStart", wxTextCtrl)->SetValue(saveStart);
        XRCCTRL(*this,"SaveEnd", wxTextCtrl)->SetValue(saveEnd);
        XRCCTRL(*this,"SaveExec", wxTextCtrl)->SetValue(saveExec);
    }
    else
    {
        XRCCTRL(*this,"SaveStart", wxTextCtrl)->SetValue("");
        XRCCTRL(*this,"SaveEnd", wxTextCtrl)->SetValue("");
        XRCCTRL(*this,"SaveExec", wxTextCtrl)->SetValue("");
    }
    XRCCTRL(*this, "UseLocation", wxCheckBox)->SetValue(state);
    XRCCTRL(*this, "TextStart", wxStaticText)->Enable(state);
    XRCCTRL(*this, "TextEnd", wxStaticText)->Enable(state);
    XRCCTRL(*this, "TextExec", wxStaticText)->Enable(state);
    XRCCTRL(*this, "SaveStart", wxTextCtrl)->Enable(state);
    XRCCTRL(*this, "SaveEnd", wxTextCtrl)->Enable(state);
    XRCCTRL(*this, "SaveExec", wxTextCtrl)->Enable(state);
}

void PopupDialog::setLocation(bool state)
{
    XRCCTRL(*this, "UseLocation", wxCheckBox)->SetValue(state);
    XRCCTRL(*this, "TextStart", wxStaticText)->Enable(state);
    XRCCTRL(*this, "TextEnd", wxStaticText)->Enable(state);
    XRCCTRL(*this, "TextExec", wxStaticText)->Enable(state);
    XRCCTRL(*this, "SaveStart", wxTextCtrl)->Enable(state);
    XRCCTRL(*this, "SaveEnd", wxTextCtrl)->Enable(state);
    XRCCTRL(*this, "SaveExec", wxTextCtrl)->Enable(state);
}

void PopupDialog::setStartLocation(wxString saveStart)
{
    XRCCTRL(*this,"SaveStart", wxTextCtrl)->SetValue(saveStart);
}

void PopupDialog::setEndLocation(wxString saveEnd)
{
    XRCCTRL(*this,"SaveEnd", wxTextCtrl)->SetValue(saveEnd);
}

void PopupDialog::setTape1Type(bool useTape)
{
    XRCCTRL(*this, "CasButton1", wxButton)->Enable(useTape);
    XRCCTRL(*this, "WavFile1", wxTextCtrl)->Enable(useTape);
    XRCCTRL(*this, "EjectCas1", wxButton)->Enable(useTape);
}

void PopupDialog::onOk(wxCommandEvent&WXUNUSED(event))
{
    Show(false);
}

void PopupDialog::onXmlControlWindows(wxCommandEvent& event)
{
    p_Main->onXmlControlWindows(event);
   if (p_Main->getGuiMode())
       p_Main->setCheckBox("ControlWindowsXml", event.IsChecked());
}

void PopupDialog::onCassette(wxCommandEvent&event)
{
    p_Main->onCassette(event);
    XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(0));
}

void PopupDialog::onCassetteText(wxCommandEvent&event)
{
    p_Main->onCassetteText(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("WavFileXml", p_Main->getWaveFile(0));
}

void PopupDialog::onCassetteEject(wxCommandEvent&event)
{
    p_Main->onCassetteEject(event);
    XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(0));
}

void PopupDialog::onCassette1(wxCommandEvent&event)
{
    p_Main->onCassette1(event);
    XRCCTRL(*this, "WavFile1", wxTextCtrl)->SetValue(p_Main->getWaveFile(1));
}

void PopupDialog::onCassetteText1(wxCommandEvent&event)
{
    p_Main->onCassette1Text(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("WavFile1Xml", p_Main->getWaveFile(1));
}

void PopupDialog::onCassetteEject1(wxCommandEvent&event)
{
    p_Main->onCassette1Eject(event);
    XRCCTRL(*this, "WavFile1", wxTextCtrl)->SetValue(p_Main->getWaveFile(1));
}

void PopupDialog::onXmodem(wxCommandEvent& event)
{
    p_Main->onXmodem(event);
    XRCCTRL(*this, "XmodemFile", wxTextCtrl)->SetValue(p_Main->getXmodemFileFullStr());
}

void PopupDialog::onXmodemEject(wxCommandEvent& event)
{
    p_Main->onXmodemEject(event);
    XRCCTRL(*this, "XmodemFile", wxTextCtrl)->SetValue(p_Main->getXmodemFile());
}

void PopupDialog::onXmodemText(wxCommandEvent& event)
{
    p_Main->onXmodemText(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("XmodemFileXml", p_Main->getXmodemFile());
}

void PopupDialog::onLoadRunButton(wxCommandEvent&event)
{
    p_Main->onLoadRunButton(event);
}

void PopupDialog::onLoadButton(wxCommandEvent&event)
{
    p_Main->onLoadButton(event);
}

void PopupDialog::onSaveButton(wxCommandEvent&event)
{
    p_Main->onSaveButton(event);
}

void PopupDialog::onDataSaveButton(wxCommandEvent&event)
{
    p_Main->onDataSaveButton(event);
}

void PopupDialog::onSaveStart(wxCommandEvent& event)
{
    p_Main->onSaveStart(event);
    p_Main->setTextCtrl("SaveStartXml", event.GetString());
}

void PopupDialog::onSaveEnd(wxCommandEvent& event)
{
    p_Main->onSaveEnd(event);
    p_Main->setTextCtrl("SaveEndXml", event.GetString());
}

void PopupDialog::onSaveExec(wxCommandEvent& event)
{
    p_Main->onSaveExec(event);
    p_Main->setTextCtrl("SaveExecXml", event.GetString());
}

void PopupDialog::onUseLocation(wxCommandEvent&event)
{
    p_Main->onUseLocation(event);
    XRCCTRL(*this, "TextStart", wxStaticText)->Enable(event.IsChecked());
    XRCCTRL(*this, "TextEnd", wxStaticText)->Enable(event.IsChecked());
    XRCCTRL(*this, "TextExec", wxStaticText)->Enable(event.IsChecked());
    XRCCTRL(*this, "SaveStart", wxTextCtrl)->Enable(event.IsChecked());
    XRCCTRL(*this, "SaveEnd", wxTextCtrl)->Enable(event.IsChecked());
    XRCCTRL(*this, "SaveExec", wxTextCtrl)->Enable(event.IsChecked());
    if (p_Main->getGuiMode())
        p_Main->setCheckBox("UseLocationXml", event.IsChecked());
}

void PopupDialog::onDisk0(wxCommandEvent&event)
{
    p_Main->onDisk0(event);
    XRCCTRL(*this, "FDC0_FileXml", wxTextCtrl)->SetValue(getFdcName(0));
}

void PopupDialog::onDiskText0(wxCommandEvent&event)
{
    p_Main->onDiskText0(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("FDC0_FileXml", getFdcName(0));
}

void PopupDialog::onDiskEject0(wxCommandEvent&event)
{
    p_Main->onDiskEject0(event);
    XRCCTRL(*this, "FDC0_FileXml", wxTextCtrl)->SetValue(getFdcName(0));
}

void PopupDialog::onDiskDirSwitch0(wxCommandEvent&event)
{
    p_Main->onDiskDirSwitch0(event);
    setDirSwitch(0);
}

void PopupDialog::onDisk1(wxCommandEvent&event)
{
    p_Main->onDisk1(event);
    XRCCTRL(*this, "FDC1_FileXml", wxTextCtrl)->SetValue(getFdcName(1));
}

void PopupDialog::onDiskText1(wxCommandEvent&event)
{
    p_Main->onDiskText1(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("FDC1_FileXml", getFdcName(1));
}

void PopupDialog::onDiskEject1(wxCommandEvent&event)
{
    p_Main->onDiskEject1(event);
    XRCCTRL(*this, "FDC1_FileXml", wxTextCtrl)->SetValue(getFdcName(1));
}

void PopupDialog::onDiskDirSwitch1(wxCommandEvent&event)
{
    p_Main->onDiskDirSwitch1(event);
    setDirSwitch(1);
}

void PopupDialog::onDisk2(wxCommandEvent&event)
{
    p_Main->onDisk2(event);
    XRCCTRL(*this, "FDC2_FileXml", wxTextCtrl)->SetValue(getFdcName(2));
}

void PopupDialog::onDiskText2(wxCommandEvent&event)
{
    p_Main->onDiskText2(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("FDC2_FileXml", getFdcName(2));
}

void PopupDialog::onDiskEject2(wxCommandEvent&event)
{
    p_Main->onDiskEject2(event);
    XRCCTRL(*this, "FDC2_FileXml", wxTextCtrl)->SetValue(getFdcName(2));
}

void PopupDialog::onDiskDirSwitch2(wxCommandEvent&event)
{
    p_Main->onDiskDirSwitch2(event);
    setDirSwitch(2);
}

void PopupDialog::onDisk3(wxCommandEvent&event)
{
    p_Main->onDisk3(event);
    XRCCTRL(*this, "FDC3_FileXml", wxTextCtrl)->SetValue(getFdcName(3));
}

void PopupDialog::onDiskText3(wxCommandEvent&event)
{
    p_Main->onDiskText3(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("FDC3_FileXml", getFdcName(3));
}

void PopupDialog::onDiskEject3(wxCommandEvent&event)
{
    p_Main->onDiskEject3(event);
    XRCCTRL(*this, "FDC3_FileXml", wxTextCtrl)->SetValue(getFdcName(3));
}

void PopupDialog::onDiskDirSwitch3(wxCommandEvent&event)
{
    p_Main->onDiskDirSwitch3(event);
    setDirSwitch(3);
}

wxString PopupDialog::getFdcName(int drive)
{
    if (p_Main->getDirectoryMode(currentComputerConfiguration.fdcType_, drive))
    {
        wxFileName setectedDirFile = wxFileName(p_Main->getUpdFloppyDirSwitched(currentComputerConfiguration.fdcType_, drive));
        wxArrayString dirArray = setectedDirFile.GetDirs();
        return dirArray.Last();
    }
    else
        return p_Main->getUpdFloppyFile(currentComputerConfiguration.fdcType_, drive);
}

void PopupDialog::setDirSwitch(int drive)
{
    wxString textCtrl, ejectButton;
    textCtrl.Printf("FDC%01d_FileXml", drive);
    ejectButton.Printf("Eject_FDC%01dXml", drive);
    if (p_Main->getDirectoryMode(currentComputerConfiguration.fdcType_, drive))
    {
        XRCCTRL(*this, textCtrl, wxTextCtrl)->Enable(false);
        XRCCTRL(*this, ejectButton, wxBitmapButton)->Enable(false);
    }
    else
    {
        XRCCTRL(*this, textCtrl, wxTextCtrl)->Enable(true);
        XRCCTRL(*this, ejectButton, wxBitmapButton)->Enable(true);
    }
    XRCCTRL(*this, textCtrl, wxTextCtrl)->SetValue(getFdcName(drive));
}

void PopupDialog::setUpdFloppyGui(int drive)
{
    wxString driveStr;
    driveStr.Printf("%d", drive);
    bool deActivateFdc;
    
    deActivateFdc = true;
    
    XRCCTRL(*this, "FDC"+driveStr+"_SwitchXml", wxBitmapButton)->Enable(false);
    if (currentComputerConfiguration.fdcConfiguration.wd1770_defined)
    {
        if (drive < 2 && drive < currentComputerConfiguration.fdcConfiguration.drives)
            deActivateFdc = false;
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->Enable( currentComputerConfiguration.fdcConfiguration.wd1770_defined && drive < 2 && drive < currentComputerConfiguration.fdcConfiguration.drives);
    }
    if (currentComputerConfiguration.fdcConfiguration.wd1793_defined)
    {
        if (drive < currentComputerConfiguration.fdcConfiguration.drives)
            deActivateFdc = false;
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->Enable(currentComputerConfiguration.fdcConfiguration.wd1793_defined && drive < currentComputerConfiguration.fdcConfiguration.drives);
    }
    if (currentComputerConfiguration.upd765Configuration.defined)
    {
        if (drive < currentComputerConfiguration.fdcConfiguration.drives)
            deActivateFdc = false;
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->Enable(currentComputerConfiguration.upd765Configuration.defined && drive < currentComputerConfiguration.fdcConfiguration.drives);
        XRCCTRL(*this, "FDC"+driveStr+"_SwitchXml", wxBitmapButton)->Enable(true);
    }
        
    if (p_Main->getDirectoryMode(currentComputerConfiguration.fdcType_, drive))
    {
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->SetLabel("HD "+driveStr);
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->SetToolTip("Browse for HD Directory "+driveStr);
        XRCCTRL(*this, "FDC"+driveStr+"_FileXml", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "Eject_FDC"+driveStr+"Xml", wxBitmapButton)->Enable(false);
        wxFileName selectedDirFile = wxFileName(p_Main->getUpdFloppyDirSwitched(currentComputerConfiguration.fdcType_, drive));
        wxArrayString dirArray = selectedDirFile.GetDirs();
        wxString dirName = dirArray.Last();
        XRCCTRL(*this, "FDC"+driveStr+"_FileXml", wxTextCtrl)->SetValue(dirName);
    }
    else
    {
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->SetLabel("FDC "+driveStr);
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->SetToolTip("Browse for FDC "+driveStr+" image file");
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->Enable(!deActivateFdc);
        XRCCTRL(*this, "FDC"+driveStr+"_FileXml", wxTextCtrl)->Enable(!deActivateFdc);
        XRCCTRL(*this, "Eject_FDC"+driveStr+"Xml", wxBitmapButton)->Enable(!deActivateFdc);
        XRCCTRL(*this, "FDC"+driveStr+"_FileXml", wxTextCtrl)->SetValue(p_Main->getUpdFloppyFile(currentComputerConfiguration.fdcType_, drive));
    }
}

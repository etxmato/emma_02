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
    EVT_BUTTON(XRCID("Disk1Button"), PopupDialog::onComxDisk1)
    EVT_TEXT(XRCID("Disk1File"), PopupDialog::onComxDiskText1)
    EVT_BUTTON(XRCID("EjectDisk1"), PopupDialog::onComxDiskEject1)
    EVT_BUTTON(XRCID("Disk2Button"), PopupDialog::onComxDisk2)
    EVT_TEXT(XRCID("Disk2File"), PopupDialog::onComxDiskText2)
    EVT_BUTTON(XRCID("EjectDisk2"), PopupDialog::onComxDiskEject2)
    EVT_BUTTON(XRCID("TerminalCasButton"), PopupDialog::onTerminalFile)
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

    EVT_BUTTON(XRCID("FDC0_ButtonMS2000"), PopupDialog::onUpdDisk0)
    EVT_TEXT(XRCID("FDC0_FileMS2000"), PopupDialog::onUpdDiskText0)
    EVT_BUTTON(XRCID("Eject_FDC0MS2000"), PopupDialog::onUpdDiskEject0)
    EVT_BUTTON(XRCID("FDC0_SwitchMS2000"), PopupDialog::onUpdDiskDirSwitch0)
    EVT_BUTTON(XRCID("FDC1_ButtonMS2000"), PopupDialog::onUpdDisk1)
    EVT_TEXT(XRCID("FDC1_FileMS2000"), PopupDialog::onUpdDiskText1)
    EVT_BUTTON(XRCID("Eject_FDC1MS2000"), PopupDialog::onUpdDiskEject1)
    EVT_BUTTON(XRCID("FDC1_SwitchMS2000"), PopupDialog::onUpdDiskDirSwitch1)
    EVT_BUTTON(XRCID("FDC2_ButtonMS2000"), PopupDialog::onUpdDisk2)
    EVT_TEXT(XRCID("FDC2_FileMS2000"), PopupDialog::onUpdDiskText2)
    EVT_BUTTON(XRCID("Eject_FDC2MS2000"), PopupDialog::onUpdDiskEject2)
    EVT_BUTTON(XRCID("FDC2_SwitchMS2000"), PopupDialog::onUpdDiskDirSwitch2)
    EVT_BUTTON(XRCID("FDC3_ButtonMS2000"), PopupDialog::onUpdDisk3)
    EVT_TEXT(XRCID("FDC3_FileMS2000"), PopupDialog::onUpdDiskText3)
    EVT_BUTTON(XRCID("Eject_FDC3MS2000"), PopupDialog::onUpdDiskEject3)
    EVT_BUTTON(XRCID("FDC3_SwitchMS2000"), PopupDialog::onUpdDiskDirSwitch3)

    EVT_CHECKBOX(XRCID("Elf2KControlWindowsPopup"), PopupDialog::onElf2KControlWindows)
    EVT_CHECKBOX(XRCID("Elf2KSwitchPopup"), PopupDialog::onElf2KSwitch)
    EVT_CHECKBOX(XRCID("Elf2KHexPopup"), PopupDialog::onElf2KHex)

    EVT_CHECKBOX(XRCID("ControlWindowsPopupCosmicos"), PopupDialog::onCosmicosControlWindows)
    EVT_CHECKBOX(XRCID("HexPopupCosmicos"), PopupDialog::onCosmicosHex)

    EVT_CHECKBOX(XRCID("ControlWindowsPopupElf"), PopupDialog::onElfControlWindows)

    EVT_CHECKBOX(XRCID("ControlWindowsPopupVelf"), PopupDialog::onVelfControlWindows)
    EVT_CHECKBOX(XRCID("ControlWindowsPopupCDP18S020"), PopupDialog::onCdp18s020ControlWindows)
    EVT_CHECKBOX(XRCID("ControlWindowsPopupFRED"), PopupDialog::onFredControlWindows)

    EVT_CHECKBOX(XRCID("ControlWindowsPopupMembership"), PopupDialog::onMembershipControlWindows)

    EVT_SPINCTRL(XRCID("AdsChannelPopup"), PopupDialog::onTelmacAdsChannel)
    EVT_TEXT(XRCID("AdsChannelPopup"), PopupDialog::onTelmacAdsChannelText)
    EVT_SPINCTRL(XRCID("AdsVoltPopup"), PopupDialog::onTelmacAdsVolt)
    EVT_TEXT(XRCID("AdsVoltPopup"), PopupDialog::onTelmacAdsVoltText)
    EVT_SPINCTRL(XRCID("AdiChannelPopup"), PopupDialog::onTelmacAdiChannel)
    EVT_TEXT(XRCID("AdiChannelPopup"), PopupDialog::onTelmacAdiChannelText)
    EVT_SPINCTRL(XRCID("AdiVoltPopup"), PopupDialog::onTelmacAdiVolt)
    EVT_TEXT(XRCID("AdiVoltPopup"), PopupDialog::onTelmacAdiVoltText)

    EVT_BUTTON(XRCID("OK"), PopupDialog::onOk)
    EVT_ACTIVATE(PopupDialog::onFocus)
END_EVENT_TABLE()

PopupDialog::PopupDialog(wxWindow* parent)
{
    computerStr_ = p_Main->getRunningComputerStr();
    computer_ = p_Main->getRunningComputerId();
    fdcType_ = p_Main->getFdcType(computer_);

    tapeOnBitmap = wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/tick.png", wxBITMAP_TYPE_PNG);
    tapeOffBitmap = wxBitmap(p_Main->getApplicationDir() + IMAGES_FOLDER + "/minus.png", wxBITMAP_TYPE_PNG);

    loadingGui_ = true;
    switch (computer_)
    {
        case ELFII:
        case SUPERELF:
        case XML:
        case PICO:
            wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"menuElf_" + p_Main->getFontSize() + ".xrc");
            wxXmlResource::Get()->LoadDialog(this, parent, wxT("PopupElf"));
            XRCCTRL(*this, "PopupElf", wxDialog)->SetLabel(p_Main->getSelectedComputerText()+" Menu");
        break;

        case TMC1800:
        case TMC2000:
        case NANO:
        case ETI:
            wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"menu_Cas_Mem_" + p_Main->getFontSize() + ".xrc");
            wxXmlResource::Get()->LoadDialog(this, parent, wxT("Popup_Cas_Mem"));
            XRCCTRL(*this, "Popup_Cas_Mem", wxDialog)->SetLabel(p_Main->getSelectedComputerText()+" Menu");
        break;
            
        case UC1800:
        case MICROTUTOR:
        case MICROTUTOR2:
            wxXmlResource::Get()->Load(p_Main->getApplicationDir() + p_Main->getPathSep() + "menu_Cas_Mem_" + p_Main->getFontSize() + ".xrc");
            wxXmlResource::Get()->LoadDialog(this, parent, wxT("Popup_Cas_Mem"));
            XRCCTRL(*this, "Popup_Cas_Mem", wxDialog)->SetLabel(p_Main->getSelectedComputerText() + " Menu");
            XRCCTRL(*this, "CasButton", wxButton)->Hide();
            XRCCTRL(*this, "WavFile", wxTextCtrl)->Hide();
            XRCCTRL(*this, "EjectCas", wxBitmapButton)->Hide();
        break;

        case VIP:
        case VIPII:
            wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"menuVip_" + p_Main->getFontSize() + ".xrc");
            wxXmlResource::Get()->LoadDialog(this, parent, wxT("PopupVip"));
            XRCCTRL(*this, "PopupVip", wxDialog)->SetLabel(p_Main->getSelectedComputerText()+" Menu");
        break;
      
        case FRED1:
        case FRED1_5:
            wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"menuFred_" + p_Main->getFontSize() + ".xrc");
            wxXmlResource::Get()->LoadDialog(this, parent, wxT("PopupFred"));
            XRCCTRL(*this, "PopupFred", wxDialog)->SetLabel(p_Main->getSelectedComputerText()+" Menu");
        break;

        case VIP2K:
        case MEMBER:
        case CDP18S020:
            wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"menuMembership_" + p_Main->getFontSize() + ".xrc");
            wxXmlResource::Get()->LoadDialog(this, parent, "PopupMembership");
            XRCCTRL(*this, "PopupMembership", wxDialog)->SetLabel(p_Main->getSelectedComputerText()+" Menu");
            if (computer_ == VIP2K)
                XRCCTRL(*this, "ControlWindowsPopupMembership", wxCheckBox)->Hide();
        break;

        case MICROBOARD:
            wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"menu"+computerStr_+"_" + p_Main->getFontSize() + ".xrc");
            wxXmlResource::Get()->LoadDialog(this, parent, "Popup"+computerStr_);
            XRCCTRL(*this, "Popup"+computerStr_, wxDialog)->SetLabel(p_Main->getSelectedComputerText()+" Menu");
        break;

        default:
            wxXmlResource::Get()->Load(p_Main->getApplicationDir()+p_Main->getPathSep()+"menu"+computerStr_+"_" + p_Main->getFontSize() + ".xrc");
            wxXmlResource::Get()->LoadDialog(this, parent, "Popup"+computerStr_);
            XRCCTRL(*this, "Popup"+computerStr_, wxDialog)->SetLabel(p_Main->getSelectedComputerText()+" Menu");
        break;
    }
    loadingGui_ = false;
}

PopupDialog::~PopupDialog()
{
}

void PopupDialog::init()
{
    p_Main->setNoteBook();
    ElfConfiguration currentElfConfig;

    switch (computer_)
    {
        case COMX:
            XRCCTRL(*this, "Disk1File", wxTextCtrl)->SetValue(p_Main->getFloppyFile(0));
            XRCCTRL(*this, "Disk2File", wxTextCtrl)->SetValue(p_Main->getFloppyFile(1));
            XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
            setLocation(p_Main->getUseLoadLocation(computer_), p_Main->getSaveStartString(computer_), p_Main->getSaveEndString(computer_), p_Main->getSaveExecString(computer_));
        break;

        case ELF2K:
            XRCCTRL(*this, "Elf2KControlWindowsPopup", wxCheckBox)->SetValue(p_Main->getUseElfControlWindows(computer_));
            XRCCTRL(*this, "Elf2KSwitchPopup", wxCheckBox)->SetValue(p_Main->getUseSwitch(computer_));
            XRCCTRL(*this, "Elf2KHexPopup", wxCheckBox)->SetValue(p_Main->getUseHex(computer_));
            setStartLocation(p_Main->getSaveStartString(computer_));
            setEndLocation(p_Main->getSaveEndString(computer_));
        break;

        case FRED1:
        case FRED1_5:
            XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
            XRCCTRL(*this, "ControlWindowsPopupFRED", wxCheckBox)->SetValue(p_Main->getUseFredControlWindows());
            setStartLocation(p_Main->getSaveStartString(computer_));
            setEndLocation(p_Main->getSaveEndString(computer_));
        break;
            
        case TMC1800:
        case TMC2000:
        case NANO:
        case ETI:
        case UC1800:
        case MICROTUTOR:
        case MICROTUTOR2:
            XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
            setStartLocation(p_Main->getSaveStartString(computer_));
            setEndLocation(p_Main->getSaveEndString(computer_));
        break;
            
        case VIP:
        case VIPII:
            XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
            setStartLocation(p_Main->getSaveStartString(computer_));
            setEndLocation(p_Main->getSaveEndString(computer_));
            setLocation(p_Main->getUseLoadLocation(computer_), p_Main->getSaveStartString(computer_), p_Main->getSaveEndString(computer_), p_Main->getSaveExecString(computer_));
        break;
            
        case VELF:
            XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
            XRCCTRL(*this, "ControlWindowsPopupVelf", wxCheckBox)->SetValue(p_Main->getUseVelfControlWindows());
            setStartLocation(p_Main->getSaveStartString(computer_));
            setEndLocation(p_Main->getSaveEndString(computer_));
        break;
            
        case COSMICOS:
            XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
            XRCCTRL(*this, "ControlWindowsPopupCosmicos", wxCheckBox)->SetValue(p_Main->getUseElfControlWindows(computer_));
            XRCCTRL(*this, "HexPopupCosmicos", wxCheckBox)->SetValue(p_Main->getUseHex(computer_));
            setStartLocation(p_Main->getSaveStartString(computer_));
            setEndLocation(p_Main->getSaveEndString(computer_));
        break;

        case ELF:
        case ELFII:
        case SUPERELF:
        case XML:
        case PICO:
            setTapeType(p_Main->getUseTape(computer_));
            enableMemAccessGui(true);
            if (p_Main->getUseXmodem(computer_))
            {
                XRCCTRL(*this, "CasButton", wxButton)->SetLabel("XMODEM");
            }
            XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
            XRCCTRL(*this, "ControlWindowsPopupElf", wxCheckBox)->SetValue(p_Main->getUseElfControlWindows(computer_));
            setLocation(p_Main->getUseLoadLocation(computer_), p_Main->getSaveStartString(computer_), p_Main->getSaveEndString(computer_), p_Main->getSaveExecString(computer_));
        break;

        case MEMBER:
        case VIP2K:
        case CDP18S020:
            XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
            XRCCTRL(*this, "ControlWindowsPopupMembership", wxCheckBox)->SetValue(p_Main->getUseElfControlWindows(computer_));
            setStartLocation(p_Main->getSaveStartString(computer_));
            setEndLocation(p_Main->getSaveEndString(computer_));
        break;

        case MS2000:
            XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->SetValue(getFdcName(0));
            XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->SetValue(getFdcName(1));
            XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->SetValue(getFdcName(2));
            XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->SetValue(getFdcName(3));
            setDirSwitch(0);
            setDirSwitch(1);
            setDirSwitch(2);
            setDirSwitch(3);
            XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
        break;
  
        case MCDS:
            setTapeType(true);
            setTape1Type(true);
            enableMemAccessGui(true);
            XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
        break;
      
        case MICROBOARD:
            currentElfConfig = p_Main->getElfConfiguration(computer_);
            setTapeType(p_Main->getUseTape(computer_));
            setTape1Type(p_Main->getUseTape(computer_));
            enableMemAccessGui(true);
            XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));

            XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->SetValue(getFdcName(0));
            XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->SetValue(getFdcName(1));
            XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->SetValue(getFdcName(2));
            XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->SetValue(getFdcName(3));
            setDirSwitch(0);
            setDirSwitch(1);
            setDirSwitch(2);
            setDirSwitch(3);

            if (!currentElfConfig.useUpd765)
            {
                XRCCTRL(*this, "FDC0_ButtonMS2000", wxButton)->Enable(false);
                XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->Enable(false);
                XRCCTRL(*this, "FDC0_SwitchMS2000", wxBitmapButton)->Enable(false);
                XRCCTRL(*this, "Eject_FDC0MS2000", wxBitmapButton)->Enable(false);
                XRCCTRL(*this, "FDC1_ButtonMS2000", wxButton)->Enable(false);
                XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->Enable(false);
                XRCCTRL(*this, "FDC1_SwitchMS2000", wxBitmapButton)->Enable(false);
                XRCCTRL(*this, "Eject_FDC1MS2000", wxBitmapButton)->Enable(false);
                XRCCTRL(*this, "FDC2_ButtonMS2000", wxButton)->Enable(false);
                XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->Enable(false);
                XRCCTRL(*this, "FDC2_SwitchMS2000", wxBitmapButton)->Enable(false);
                XRCCTRL(*this, "Eject_FDC2MS2000", wxBitmapButton)->Enable(false);
                XRCCTRL(*this, "FDC3_ButtonMS2000", wxButton)->Enable(false);
                XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->Enable(false);
                XRCCTRL(*this, "FDC3_SwitchMS2000", wxBitmapButton)->Enable(false);
                XRCCTRL(*this, "Eject_FDC3MS2000", wxBitmapButton)->Enable(false);
            }
        break;

        case TMC600:
            XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
            setLocation(p_Main->getUseLoadLocation(computer_), p_Main->getSaveStartString(computer_), p_Main->getSaveEndString(computer_), p_Main->getSaveExecString(computer_));
            if (p_Computer->getInType(4) == TELMACIN)
                enableIoGui();
        break;

        case PECOM:
            XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
            setLocation(p_Main->getUseLoadLocation(computer_), p_Main->getSaveStartString(computer_), p_Main->getSaveEndString(computer_), p_Main->getSaveExecString(computer_));
        break;

    }
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

void PopupDialog::setStartLocation(wxString saveStart)
{
    XRCCTRL(*this,"SaveStart", wxTextCtrl)->SetValue(saveStart);
}

void PopupDialog::setEndLocation(wxString saveEnd)
{
    XRCCTRL(*this,"SaveEnd", wxTextCtrl)->SetValue(saveEnd);
}

void PopupDialog::setTapeType(bool useTape)
{
    XRCCTRL(*this, "CasButton", wxButton)->Enable(useTape);
    XRCCTRL(*this, "WavFile", wxTextCtrl)->Enable(useTape);
    XRCCTRL(*this, "EjectCas", wxButton)->Enable(useTape);
}

void PopupDialog::setTape1Type(bool useTape)
{
    XRCCTRL(*this, "CasButton1", wxButton)->Enable(useTape);
    XRCCTRL(*this, "WavFile1", wxTextCtrl)->Enable(useTape);
    XRCCTRL(*this, "EjectCas1", wxButton)->Enable(useTape);
}

void PopupDialog::enableMemAccessGui(bool status)
{
    if ((p_Computer->getLoadedProgram()&0x1) == 0x1)
    {
        XRCCTRL(*this, "RunButton", wxButton)->Enable(status);
        XRCCTRL(*this, "UseLocation", wxCheckBox)->Enable(status);
        XRCCTRL(*this, "UseLocationText", wxStaticText)->Enable(status);
        XRCCTRL(*this, "DsaveButton", wxButton)->Enable(status);
        XRCCTRL(*this, "TextStart", wxStaticText)->Enable(p_Main->getUseLoadLocation(computer_));
        XRCCTRL(*this, "TextEnd", wxStaticText)->Enable(p_Main->getUseLoadLocation(computer_));
        XRCCTRL(*this, "TextExec", wxStaticText)->Enable(p_Main->getUseLoadLocation(computer_));
        XRCCTRL(*this, "SaveStart", wxTextCtrl)->Enable(p_Main->getUseLoadLocation(computer_));
        XRCCTRL(*this, "SaveEnd", wxTextCtrl)->Enable(p_Main->getUseLoadLocation(computer_));
        XRCCTRL(*this, "SaveExec", wxTextCtrl)->Enable(p_Main->getUseLoadLocation(computer_));
    }
}

void PopupDialog::enableIoGui()
{
    XRCCTRL(*this,"AdiInputTextPopup", wxStaticText)->Enable(true);
    XRCCTRL(*this,"AdiChannelPopup", wxSpinCtrl)->Enable(true);
    XRCCTRL(*this,"AdiVoltPopup", wxSpinCtrl)->Enable(true);
    XRCCTRL(*this,"AdiVoltTextPopup", wxStaticText)->Enable(true);
    XRCCTRL(*this,"AdsInputTextPopup", wxStaticText)->Enable(true);
    XRCCTRL(*this,"AdsChannelPopup", wxSpinCtrl)->Enable(true);
    XRCCTRL(*this,"AdsVoltPopup", wxSpinCtrl)->Enable(true);
    XRCCTRL(*this,"AdsVoltTextPopup", wxStaticText)->Enable(true);
}

void PopupDialog::onOk(wxCommandEvent&WXUNUSED(event))
{
    Show(false);
}

void PopupDialog::onFocus(wxActivateEvent&WXUNUSED(event))
{
    p_Main->setNoteBook();
}

void PopupDialog::onComxDisk1(wxCommandEvent&event)
{
    p_Main->onComxDisk1(event);
    XRCCTRL(*this, "Disk1File", wxTextCtrl)->SetValue(p_Main->getFloppyFile(0));
}

void PopupDialog::onComxDiskText1(wxCommandEvent&event)
{
    p_Main->onComxDiskText1(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("Disk1FileComx", p_Main->getFloppyFile(0));
}

void PopupDialog::onComxDiskEject1(wxCommandEvent&event)
{
    p_Main->onComxDiskEject1(event);
    XRCCTRL(*this, "Disk1File", wxTextCtrl)->SetValue(p_Main->getFloppyFile(0));
}

void PopupDialog::onComxDisk2(wxCommandEvent&event)
{
    p_Main->onComxDisk2(event);
    XRCCTRL(*this, "Disk2File", wxTextCtrl)->SetValue(p_Main->getFloppyFile(1));
}

void PopupDialog::onComxDiskText2(wxCommandEvent&event)
{
    p_Main->onComxDiskText2(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("Disk2FileComx", p_Main->getFloppyFile(1));
}

void PopupDialog::onComxDiskEject2(wxCommandEvent&event)
{
    p_Main->onComxDiskEject2(event);
    XRCCTRL(*this, "Disk2File", wxTextCtrl)->SetValue(p_Main->getFloppyFile(1));
}

void PopupDialog::onElf2KControlWindows(wxCommandEvent&event)
{
     p_Main->onElf2KControlWindows(event);
    if (p_Main->getGuiMode())
        p_Main->setCheckBox("Elf2KControlWindows", event.IsChecked());
}

void PopupDialog::onElf2KSwitch(wxCommandEvent&event)
{
     p_Main->onElf2KSwitch(event);
    if (p_Main->getGuiMode())
    {
        if (event.IsChecked())
            XRCCTRL(*this, "Elf2KHexPopup", wxCheckBox)->SetValue(false);
        p_Main->setCheckBox("Elf2KSwitch", event.IsChecked());
    }
}

void PopupDialog::onElf2KHex(wxCommandEvent&event)
{
     p_Main->onElf2KHex(event);
    if (p_Main->getGuiMode())
    {
        if (event.IsChecked())
            XRCCTRL(*this, "Elf2KSwitchPopup", wxCheckBox)->SetValue(false);
        p_Main->setCheckBox("Elf2KHex", event.IsChecked());
    }
}

void PopupDialog::onCosmicosControlWindows(wxCommandEvent&event)
{
     p_Main->onCosmicosControlWindows(event);
    if (p_Main->getGuiMode())
        p_Main->setCheckBox("ControlWindowsCosmicos", event.IsChecked());
}

void PopupDialog::onCosmicosHex(wxCommandEvent&event)
{
     p_Main->onCosmicosHex(event);
    if (p_Main->getGuiMode())
        p_Main->setCheckBox("HexCosmicos", event.IsChecked());
}

void PopupDialog::onElfControlWindows(wxCommandEvent&event)
{
    p_Main->onElfControlWindows(event);
    if (p_Main->getGuiMode())
        p_Main->setCheckBox("ControlWindows"+computerStr_, event.IsChecked());
}

void PopupDialog::onVelfControlWindows(wxCommandEvent&event)
{
    p_Main->onVelfControlWindows(event);
    if (p_Main->getGuiMode())
        p_Main->setCheckBox("ControlWindowsVelf", event.IsChecked());
}

void PopupDialog::onCdp18s020ControlWindows(wxCommandEvent&event)
{
    p_Main->onCdp18s020ControlWindows(event);
    if (p_Main->getGuiMode())
        p_Main->setCheckBox("ControlWindowsCDP18S020", event.IsChecked());
}

void PopupDialog::onFredControlWindows(wxCommandEvent&event)
{
    if (computer_ == FRED1)
    {
    p_Main->onFred1ControlWindows(event);
    if (p_Main->getGuiMode())
        p_Main->setCheckBox("ControlWindowsFRED1", event.IsChecked());
    }
    else
    {
        p_Main->onFred2ControlWindows(event);
        if (p_Main->getGuiMode())
            p_Main->setCheckBox("ControlWindowsFRED1_5", event.IsChecked());
    }
}

void PopupDialog::onMembershipControlWindows(wxCommandEvent&event)
{
    switch (computer_)
    {
        case MEMBER:
            p_Main->onMembershipControlWindows(event);
        break;
        case CDP18S020:
            p_Main->onCdp18s020ControlWindows(event);
        break;
    }
    if (p_Main->getGuiMode())
        p_Main->setCheckBox("ControlWindowsMembership", event.IsChecked());
}

void PopupDialog::onTelmacAdsChannel(wxSpinEvent&event)
{
    if (loadingGui_)  return;
    p_Main->onTelmacAdsChannel(event);
    XRCCTRL(*this, "AdsVoltPopup", wxSpinCtrl)->SetValue(p_Computer->getAds(event.GetPosition()));

    if (p_Main->getGuiMode())
        p_Main->setSpinCtrl("AdsChannel", event.GetPosition());
}

void PopupDialog::onTelmacAdsVolt(wxSpinEvent&event)
{
    if (loadingGui_)  return;
    p_Main->onTelmacAdsVolt(event);
    if (p_Main->getGuiMode())
        p_Main->setSpinCtrl("AdsVolt", event.GetPosition()); 
}

void PopupDialog::onTelmacAdsChannelText(wxCommandEvent&event)
{
    if (loadingGui_)  return;
    p_Main->onTelmacAdsChannelText(event);
    int adsChannel = XRCCTRL(*this, "AdsChannelPopup", wxSpinCtrl)->GetValue();
    int adsValue = p_Computer->getAds(adsChannel);
    XRCCTRL(*this, "AdsVoltPopup", wxSpinCtrl)->SetValue(adsValue);

    if (p_Main->getGuiMode())
    {
        p_Main->setSpinCtrl("AdsChannel", XRCCTRL(*this, "AdsChannelPopup", wxSpinCtrl)->GetValue());
        p_Main->setSpinCtrl("AdsVolt", adsValue);
    }
}

void PopupDialog::onTelmacAdsVoltText(wxCommandEvent&event)
{
    if (loadingGui_)  return;
    p_Main->onTelmacAdsVoltText(event);
    if (p_Main->getGuiMode())
        p_Main->setSpinCtrl("AdsVolt", XRCCTRL(*this, "AdsVoltPopup", wxSpinCtrl)->GetValue()); 
}

void PopupDialog::onTelmacAdiChannel(wxSpinEvent&event)
{
    if (loadingGui_)  return;
    p_Main->onTelmacAdiChannel(event);
    XRCCTRL(*this, "AdiVoltPopup", wxSpinCtrl)->SetValue(p_Computer->getAdi(event.GetPosition()));

    if (p_Main->getGuiMode())
        p_Main->setSpinCtrl("AdiChannel", event.GetPosition());
}

void PopupDialog::onTelmacAdiVolt(wxSpinEvent&event)
{
    if (loadingGui_)  return;
    p_Main->onTelmacAdiVolt(event);
    if (p_Main->getGuiMode())
        p_Main->setSpinCtrl("AdiVolt", event.GetPosition()); 
}

void PopupDialog::onTelmacAdiChannelText(wxCommandEvent&event)
{
    if (loadingGui_)  return;
    p_Main->onTelmacAdiChannelText(event);
    int adiChannel = XRCCTRL(*this, "AdiChannelPopup", wxSpinCtrl)->GetValue();
    int adiValue = p_Computer->getAdi(adiChannel);
    XRCCTRL(*this, "AdiVoltPopup", wxSpinCtrl)->SetValue(adiValue);

    if (p_Main->getGuiMode())
    {
        p_Main->setSpinCtrl("AdiChannel", XRCCTRL(*this, "AdiChannelPopup", wxSpinCtrl)->GetValue());
        p_Main->setSpinCtrl("AdiVolt", adiValue);
    }
}

void PopupDialog::onTelmacAdiVoltText(wxCommandEvent&event)
{
    if (loadingGui_)  return;
    p_Main->onTelmacAdiVoltText(event);
    if (p_Main->getGuiMode())
        p_Main->setSpinCtrl("AdiVolt", XRCCTRL(*this, "AdiVoltPopup", wxSpinCtrl)->GetValue()); 
}

void PopupDialog::onTerminalFile(wxCommandEvent&event)
{
     p_Main->onTerminalFile(event);
    XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
}

void PopupDialog::onCassette(wxCommandEvent&event)
{
     p_Main->onCassette(event);
    XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
}

void PopupDialog::onCassetteText(wxCommandEvent&event)
{
    if (computer_ == MICROTUTOR || computer_ == MICROTUTOR2 || computer_ == UC1800)
        return;

     p_Main->onCassetteText(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("WavFile"+computerStr_, p_Main->getWaveFile(computer_));
}

void PopupDialog::onCassetteEject(wxCommandEvent&event)
{
     p_Main->onCassetteEject(event);
    XRCCTRL(*this, "WavFile", wxTextCtrl)->SetValue(p_Main->getWaveFile(computer_));
}

void PopupDialog::onCassette1(wxCommandEvent&event)
{
    p_Main->onCassette1(event);
    XRCCTRL(*this, "WavFile1", wxTextCtrl)->SetValue(p_Main->getWaveFile1(computer_));
}

void PopupDialog::onCassetteText1(wxCommandEvent&event)
{
    p_Main->onCassette1Text(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("WavFile1"+computerStr_, p_Main->getWaveFile1(computer_));
}

void PopupDialog::onCassetteEject1(wxCommandEvent&event)
{
    p_Main->onCassette1Eject(event);
    XRCCTRL(*this, "WavFile1", wxTextCtrl)->SetValue(p_Main->getWaveFile1(computer_));
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
    p_Main->setTextCtrl("SaveStart"+computerStr_, event.GetString());
}

void PopupDialog::onSaveEnd(wxCommandEvent& event)
{
     p_Main->onSaveEnd(event);
    p_Main->setTextCtrl("SaveEnd"+computerStr_, event.GetString());
}

void PopupDialog::onSaveExec(wxCommandEvent& event)
{
     p_Main->onSaveExec(event);
    p_Main->setTextCtrl("SaveExec"+computerStr_, event.GetString());
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
        p_Main->setCheckBox("UseLocation"+computerStr_, event.IsChecked());
}

void PopupDialog::onUpdDisk0(wxCommandEvent&event)
{
    p_Main->onUpdDisk0(event);
    XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->SetValue(getFdcName(0));
}

void PopupDialog::onUpdDiskText0(wxCommandEvent&event)
{
    p_Main->onUpdDiskText0(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("FDC0_FileMS2000", getFdcName(0));
}

void PopupDialog::onUpdDiskEject0(wxCommandEvent&event)
{
    p_Main->onUpdDiskEject0(event);
    XRCCTRL(*this, "FDC0_FileMS2000", wxTextCtrl)->SetValue(getFdcName(0));
}

void PopupDialog::onUpdDiskDirSwitch0(wxCommandEvent&event)
{
    p_Main->onUpdDiskDirSwitch0(event);
    setDirSwitch(0);
}

void PopupDialog::onUpdDisk1(wxCommandEvent&event)
{
    p_Main->onUpdDisk1(event);
    XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->SetValue(getFdcName(1));
}

void PopupDialog::onUpdDiskText1(wxCommandEvent&event)
{
    p_Main->onUpdDiskText1(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("FDC1_FileMS2000", getFdcName(1));
}

void PopupDialog::onUpdDiskEject1(wxCommandEvent&event)
{
    p_Main->onUpdDiskEject1(event);
    XRCCTRL(*this, "FDC1_FileMS2000", wxTextCtrl)->SetValue(getFdcName(1));
}

void PopupDialog::onUpdDiskDirSwitch1(wxCommandEvent&event)
{
    p_Main->onUpdDiskDirSwitch1(event);
    setDirSwitch(1);
}

void PopupDialog::onUpdDisk2(wxCommandEvent&event)
{
    p_Main->onUpdDisk1(event);
    XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->SetValue(getFdcName(2));
}

void PopupDialog::onUpdDiskText2(wxCommandEvent&event)
{
    p_Main->onUpdDiskText2(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("FDC2_FileMS2000", getFdcName(2));
}

void PopupDialog::onUpdDiskEject2(wxCommandEvent&event)
{
    p_Main->onUpdDiskEject2(event);
    XRCCTRL(*this, "FDC2_FileMS2000", wxTextCtrl)->SetValue(getFdcName(2));
}

void PopupDialog::onUpdDiskDirSwitch2(wxCommandEvent&event)
{
    p_Main->onUpdDiskDirSwitch2(event);
    setDirSwitch(2);
}

void PopupDialog::onUpdDisk3(wxCommandEvent&event)
{
    p_Main->onUpdDisk3(event);
    XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->SetValue(getFdcName(3));
}

void PopupDialog::onUpdDiskText3(wxCommandEvent&event)
{
    p_Main->onUpdDiskText3(event);
    if (p_Main->getGuiMode())
        p_Main->setTextCtrl("FDC3_FileMS2000", getFdcName(3));
}

void PopupDialog::onUpdDiskEject3(wxCommandEvent&event)
{
    p_Main->onUpdDiskEject3(event);
    XRCCTRL(*this, "FDC3_FileMS2000", wxTextCtrl)->SetValue(getFdcName(3));
}

void PopupDialog::onUpdDiskDirSwitch3(wxCommandEvent&event)
{
    p_Main->onUpdDiskDirSwitch3(event);
    setDirSwitch(3);
}

wxString PopupDialog::getFdcName(int drive)
{
    if (p_Main->getDirectoryMode(fdcType_, drive))
    {
        wxFileName setectedDirFile = wxFileName(p_Main->getUpdFloppyDirSwitched(fdcType_, drive));
        wxArrayString dirArray = setectedDirFile.GetDirs();
        return dirArray.Last();
    }
    else
        return p_Main->getUpdFloppyFile(fdcType_, drive);
}

void PopupDialog::setDirSwitch(int drive)
{
    wxString textCtrl, ejectButton;
    textCtrl.Printf("FDC%01d_FileMS2000", drive);
    ejectButton.Printf("Eject_FDC%01dMS2000", drive);
    if (p_Main->getDirectoryMode(fdcType_, drive))
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

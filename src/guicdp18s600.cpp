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

#include "main.h"
#include "guicdp18s600.h"
#include "microsetup.h"

BEGIN_EVENT_TABLE(GuiCdp18s600, GuiTMC2000)

    EVT_TEXT(XRCID("MainRomU21CDP18S600"), GuiCdp18s600::onRomU21Text)
    EVT_COMBOBOX(XRCID("MainRomU21CDP18S600"), GuiCdp18s600::onRomU21Text)
    EVT_BUTTON(XRCID("RomButtonU21CDP18S600"), GuiCdp18s600::onRomU21)

    EVT_TEXT(XRCID("MainRomU20CDP18S600"), GuiCdp18s600::onRomU20Text)
    EVT_COMBOBOX(XRCID("MainRomU20CDP18S600"), GuiCdp18s600::onRomU20Text)
    EVT_BUTTON(XRCID("RomButtonU20CDP18S600"), GuiCdp18s600::onRomU20)

    EVT_TEXT(XRCID("MainRomU19CDP18S600"), GuiCdp18s600::onRomU19Text)
    EVT_COMBOBOX(XRCID("MainRomU19CDP18S600"), GuiCdp18s600::onRomU19Text)
    EVT_BUTTON(XRCID("RomButtonU19CDP18S600"), GuiCdp18s600::onRomU19)

    EVT_TEXT(XRCID("MainRomU18CDP18S600"), GuiCdp18s600::onRomU18Text)
    EVT_COMBOBOX(XRCID("MainRomU18CDP18S600"), GuiCdp18s600::onRomU18Text)
    EVT_BUTTON(XRCID("RomButtonU18CDP18S600"), GuiCdp18s600::onRomU18)

    EVT_TEXT(XRCID("MainRomU17CDP18S600"), GuiCdp18s600::onRomU17Text)
    EVT_COMBOBOX(XRCID("MainRomU17CDP18S600"), GuiCdp18s600::onRomU17Text)
    EVT_BUTTON(XRCID("RomButtonU17CDP18S600"), GuiCdp18s600::onRomU17)

    EVT_CHOICE(XRCID("OneSocketBankCDP18S600"), GuiCdp18s600::onOneSocketBank)
    EVT_CHOICE(XRCID("FourSocketBankCDP18S600"), GuiCdp18s600::onFourSocketBank)

    EVT_TEXT(XRCID("VtCharRomCDP18S600"), GuiMain::onVtCharRomText)
    EVT_COMBOBOX(XRCID("VtCharRomCDP18S600"), GuiMain::onVtCharRomText)
    EVT_BUTTON(XRCID("VtCharRomButtonCDP18S600"), GuiMain::onVtCharRom)

    EVT_TEXT(XRCID("KeyFileCDP18S600"), GuiMain::onKeyFileText)
    EVT_BUTTON(XRCID("KeyFileButtonCDP18S600"), GuiMain::onKeyFile)
    EVT_BUTTON(XRCID("EjectKeyFileCDP18S600"), GuiMain::onKeyFileEject)

    EVT_CHOICE(XRCID("VTTypeCDP18S600"), GuiMain::onVT100)
    EVT_SPIN_UP(XRCID("ZoomSpinVtCDP18S600"), GuiMain::onZoomUpVt)
    EVT_SPIN_DOWN(XRCID("ZoomSpinVtCDP18S600"), GuiMain::onZoomDownVt)
    EVT_TEXT(XRCID("ZoomValueVtCDP18S600"), GuiMain::onZoomValueVt)
    EVT_BUTTON(XRCID("FullScreenF3CDP18S600"), GuiMain::onFullScreen)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonCDP18S600"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileCDP18S600"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileCDP18S600"), GuiMain::onScreenDumpFileText)
    EVT_BUTTON(XRCID("ScreenDumpF5CDP18S600"), GuiMain::onScreenDump)

    EVT_CHECKBOX(XRCID("CDP18S600ForceUC"), GuiCdp18s600::onCdp18s600ForceUpperCase)
    EVT_BUTTON(XRCID("VtSetupCDP18S600"), GuiMain::onVtSetup)
    EVT_CHECKBOX(XRCID("StretchDotCDP18S600"), GuiMain::onStretchDot)
    EVT_BUTTON(XRCID("ColoursCDP18S600"), Main::onColoursDef)

    EVT_BUTTON(XRCID("SaveButtonCDP18S600"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonCDP18S600"), GuiMain::onLoadButton)
    EVT_BUTTON(XRCID("RunButtonCDP18S600"), GuiMain::onLoadRunButton)
    EVT_BUTTON(XRCID("DsaveButtonCDP18S600"), GuiMain::onDataSaveButton)

    EVT_CHECKBOX(XRCID("UseLocationCDP18S600"), GuiMain::onUseLocation)
    EVT_TEXT(XRCID("SaveStartCDP18S600"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndCDP18S600"), GuiMain::onSaveEnd)

    EVT_CHOICE(XRCID("VTBaudTChoiceCDP18S600"), GuiCdp18s600::onCdp18s600BaudT)

    EVT_BUTTON(XRCID("OneSocketSetupCDP18S600"), GuiCdp18s600::onOneSocketSetup)
    EVT_BUTTON(XRCID("FourSocketSetupCDP18S600"), GuiCdp18s600::onFourSocketSetup)

    EVT_CHECKBOX(XRCID("AutoBootCDP18S600"), GuiCdp18s600::onAutoBoot)
    EVT_CHOICE(XRCID("AutoBootTypeCDP18S600"), GuiCdp18s600::onAutoBootType)

    EVT_TEXT(XRCID("ShowAddressCDP18S600"), GuiMain::onLedTimer)
    EVT_CHECKBOX(XRCID("ControlWindowsCDP18S600"), GuiCdp18s600::onCdp18s600ControlWindows)
    EVT_CHECKBOX(XRCID("PioCDP18S600"), GuiCdp18s600::onPioWindows)


    EVT_TEXT(XRCID("MainRomU21CDP18S601"), GuiCdp18s600::onRomU21Text)
    EVT_COMBOBOX(XRCID("MainRomU21CDP18S601"), GuiCdp18s600::onRomU21Text)
    EVT_BUTTON(XRCID("RomButtonU21CDP18S601"), GuiCdp18s600::onRomU21)

    EVT_TEXT(XRCID("MainRomU20CDP18S601"), GuiCdp18s600::onRomU20Text)
    EVT_COMBOBOX(XRCID("MainRomU20CDP18S601"), GuiCdp18s600::onRomU20Text)
    EVT_BUTTON(XRCID("RomButtonU20CDP18S601"), GuiCdp18s600::onRomU20)

    EVT_TEXT(XRCID("MainRomU19CDP18S601"), GuiCdp18s600::onRomU19Text)
    EVT_COMBOBOX(XRCID("MainRomU19CDP18S601"), GuiCdp18s600::onRomU19Text)
    EVT_BUTTON(XRCID("RomButtonU19CDP18S601"), GuiCdp18s600::onRomU19)

    EVT_TEXT(XRCID("MainRomU18CDP18S601"), GuiCdp18s600::onRomU18Text)
    EVT_COMBOBOX(XRCID("MainRomU18CDP18S601"), GuiCdp18s600::onRomU18Text)
    EVT_BUTTON(XRCID("RomButtonU18CDP18S601"), GuiCdp18s600::onRomU18)

    EVT_TEXT(XRCID("MainRomU17CDP18S601"), GuiCdp18s600::onRomU17Text)
    EVT_COMBOBOX(XRCID("MainRomU17CDP18S601"), GuiCdp18s600::onRomU17Text)
    EVT_BUTTON(XRCID("RomButtonU17CDP18S601"), GuiCdp18s600::onRomU17)

    EVT_CHOICE(XRCID("OneSocketBankCDP18S601"), GuiCdp18s600::onOneSocketBank)
    EVT_CHOICE(XRCID("FourSocketBankCDP18S601"), GuiCdp18s600::onFourSocketBank)

    EVT_TEXT(XRCID("VtCharRomCDP18S601"), GuiMain::onVtCharRomText)
    EVT_COMBOBOX(XRCID("VtCharRomCDP18S601"), GuiMain::onVtCharRomText)
    EVT_BUTTON(XRCID("VtCharRomButtonCDP18S601"), GuiMain::onVtCharRom)

    EVT_TEXT(XRCID("KeyFileCDP18S601"), GuiMain::onKeyFileText)
    EVT_BUTTON(XRCID("KeyFileButtonCDP18S601"), GuiMain::onKeyFile)
    EVT_BUTTON(XRCID("EjectKeyFileCDP18S601"), GuiMain::onKeyFileEject)

    EVT_CHOICE(XRCID("VTTypeCDP18S601"), GuiMain::onVT100)
    EVT_SPIN_UP(XRCID("ZoomSpinVtCDP18S601"), GuiMain::onZoomUpVt)
    EVT_SPIN_DOWN(XRCID("ZoomSpinVtCDP18S601"), GuiMain::onZoomDownVt)
    EVT_TEXT(XRCID("ZoomValueVtCDP18S601"), GuiMain::onZoomValueVt)
    EVT_BUTTON(XRCID("FullScreenF3CDP18S601"), GuiMain::onFullScreen)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonCDP18S601"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileCDP18S601"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileCDP18S601"), GuiMain::onScreenDumpFileText)
    EVT_BUTTON(XRCID("ScreenDumpF5CDP18S601"), GuiMain::onScreenDump)

    EVT_CHECKBOX(XRCID("CDP18S601ForceUC"), GuiCdp18s600::onCdp18s600ForceUpperCase)
    EVT_BUTTON(XRCID("VtSetupCDP18S601"), GuiMain::onVtSetup)
    EVT_CHECKBOX(XRCID("StretchDotCDP18S601"), GuiMain::onStretchDot)
    EVT_BUTTON(XRCID("ColoursCDP18S601"), Main::onColoursDef)

    EVT_BUTTON(XRCID("SaveButtonCDP18S601"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonCDP18S601"), GuiMain::onLoadButton)
    EVT_BUTTON(XRCID("RunButtonCDP18S601"), GuiMain::onLoadRunButton)
    EVT_BUTTON(XRCID("DsaveButtonCDP18S601"), GuiMain::onDataSaveButton)

    EVT_CHECKBOX(XRCID("UseLocationCDP18S601"), GuiMain::onUseLocation)
    EVT_TEXT(XRCID("SaveStartCDP18S601"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndCDP18S601"), GuiMain::onSaveEnd)

    EVT_CHOICE(XRCID("VTBaudTChoiceCDP18S601"), GuiCdp18s600::onCdp18s600BaudT)

    EVT_BUTTON(XRCID("FourSocketSetupCDP18S601"), GuiCdp18s600::onRomSocketSetup)

    EVT_CHECKBOX(XRCID("AutoBootCDP18S601"), GuiCdp18s600::onAutoBoot)
    EVT_CHOICE(XRCID("AutoBootTypeCDP18S601"), GuiCdp18s600::onAutoBootType)

    EVT_TEXT(XRCID("ShowAddressCDP18S601"), GuiMain::onLedTimer)
    EVT_CHECKBOX(XRCID("ControlWindowsCDP18S601"), GuiCdp18s600::onCdp18s600ControlWindows)
    EVT_CHECKBOX(XRCID("PioCDP18S601"), GuiCdp18s600::onPioWindows)


    EVT_TEXT(XRCID("MainRomU21CDP18S603A"), GuiCdp18s600::onRomU21Text)
    EVT_COMBOBOX(XRCID("MainRomU21CDP18S603A"), GuiCdp18s600::onRomU21Text)
    EVT_BUTTON(XRCID("RomButtonU21CDP18S603A"), GuiCdp18s600::onRomU21)

    EVT_TEXT(XRCID("MainRomU20CDP18S603A"), GuiCdp18s600::onRomU20Text)
    EVT_COMBOBOX(XRCID("MainRomU20CDP18S603A"), GuiCdp18s600::onRomU20Text)
    EVT_BUTTON(XRCID("RomButtonU20CDP18S603A"), GuiCdp18s600::onRomU20)

    EVT_CHOICE(XRCID("OneSocketBankCDP18S603A"), GuiCdp18s600::onOneSocketBank)
    EVT_CHOICE(XRCID("FourSocketBankCDP18S603A"), GuiCdp18s600::onFourSocketBank)

    EVT_TEXT(XRCID("VtCharRomCDP18S603A"), GuiMain::onVtCharRomText)
    EVT_COMBOBOX(XRCID("VtCharRomCDP18S603A"), GuiMain::onVtCharRomText)
    EVT_BUTTON(XRCID("VtCharRomButtonCDP18S603A"), GuiMain::onVtCharRom)

    EVT_TEXT(XRCID("KeyFileCDP18S603A"), GuiMain::onKeyFileText)
    EVT_BUTTON(XRCID("KeyFileButtonCDP18S603A"), GuiMain::onKeyFile)
    EVT_BUTTON(XRCID("EjectKeyFileCDP18S603A"), GuiMain::onKeyFileEject)

    EVT_CHOICE(XRCID("VTTypeCDP18S603A"), GuiMain::onVT100)
    EVT_SPIN_UP(XRCID("ZoomSpinVtCDP18S603A"), GuiMain::onZoomUpVt)
    EVT_SPIN_DOWN(XRCID("ZoomSpinVtCDP18S603A"), GuiMain::onZoomDownVt)
    EVT_TEXT(XRCID("ZoomValueVtCDP18S603A"), GuiMain::onZoomValueVt)
    EVT_BUTTON(XRCID("FullScreenF3CDP18S603A"), GuiMain::onFullScreen)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonCDP18S603A"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileCDP18S603A"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileCDP18S603A"), GuiMain::onScreenDumpFileText)
    EVT_BUTTON(XRCID("ScreenDumpF5CDP18S603A"), GuiMain::onScreenDump)

    EVT_CHECKBOX(XRCID("CDP18S603AForceUC"), GuiCdp18s600::onCdp18s600ForceUpperCase)
    EVT_BUTTON(XRCID("VtSetupCDP18S603A"), GuiMain::onVtSetup)
    EVT_CHECKBOX(XRCID("StretchDotCDP18S603A"), GuiMain::onStretchDot)
    EVT_BUTTON(XRCID("ColoursCDP18S603A"), Main::onColoursDef)

    EVT_BUTTON(XRCID("SaveButtonCDP18S603A"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonCDP18S603A"), GuiMain::onLoadButton)
    EVT_BUTTON(XRCID("RunButtonCDP18S603A"), GuiMain::onLoadRunButton)
    EVT_BUTTON(XRCID("DsaveButtonCDP18S603A"), GuiMain::onDataSaveButton)

    EVT_CHECKBOX(XRCID("UseLocationCDP18S603A"), GuiMain::onUseLocation)
    EVT_TEXT(XRCID("SaveStartCDP18S603A"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndCDP18S603A"), GuiMain::onSaveEnd)

    EVT_CHOICE(XRCID("VTBaudTChoiceCDP18S603A"), GuiCdp18s600::onCdp18s600BaudT)

    EVT_BUTTON(XRCID("FourSocketSetupCDP18S603A"), GuiCdp18s600::onRom603ASocketSetup)

    EVT_CHECKBOX(XRCID("AutoBootCDP18S603A"), GuiCdp18s600::onAutoBoot)
    EVT_CHOICE(XRCID("AutoBootTypeCDP18S603A"), GuiCdp18s600::onAutoBootType)

    EVT_TEXT(XRCID("ShowAddressCDP18S603A"), GuiMain::onLedTimer)
    EVT_CHECKBOX(XRCID("ControlWindowsCDP18S603A"), GuiCdp18s600::onCdp18s600ControlWindows)
    EVT_CHECKBOX(XRCID("PioCDP18S603A"), GuiCdp18s600::onPioWindows)


    EVT_TEXT(XRCID("MainRomU21CDP18S604B"), GuiCdp18s600::onRomU21Text)
    EVT_COMBOBOX(XRCID("MainRomU21CDP18S604B"), GuiCdp18s600::onRomU21Text)
    EVT_BUTTON(XRCID("RomButtonU21CDP18S604B"), GuiCdp18s600::onRomU21)

    EVT_TEXT(XRCID("MainRomU20CDP18S604B"), GuiCdp18s600::onRomU20Text)
    EVT_COMBOBOX(XRCID("MainRomU20CDP18S604B"), GuiCdp18s600::onRomU20Text)
    EVT_BUTTON(XRCID("RomButtonU20CDP18S604B"), GuiCdp18s600::onRomU20)

    EVT_TEXT(XRCID("MainRomU19CDP18S604B"), GuiCdp18s600::onRomU19Text)
    EVT_COMBOBOX(XRCID("MainRomU19CDP18S604B"), GuiCdp18s600::onRomU19Text)
    EVT_BUTTON(XRCID("RomButtonU19CDP18S604B"), GuiCdp18s600::onRomU19)

    EVT_TEXT(XRCID("MainRomU18CDP18S604B"), GuiCdp18s600::onRomU18Text)
    EVT_COMBOBOX(XRCID("MainRomU18CDP18S604B"), GuiCdp18s600::onRomU18Text)
    EVT_BUTTON(XRCID("RomButtonU18CDP18S604B"), GuiCdp18s600::onRomU18)

    EVT_TEXT(XRCID("MainRomU17CDP18S604B"), GuiCdp18s600::onRomU17Text)
    EVT_COMBOBOX(XRCID("MainRomU17CDP18S604B"), GuiCdp18s600::onRomU17Text)
    EVT_BUTTON(XRCID("RomButtonU17CDP18S604B"), GuiCdp18s600::onRomU17)

    EVT_CHOICE(XRCID("FourSocketBankCDP18S604B"), GuiCdp18s600::onCdp18s604bFourSocketBank)
    EVT_TEXT(XRCID("RamBlockCDP18S604B"), GuiCdp18s600::onCdp18s604bRamBlock)
    EVT_TEXT(XRCID("RomBlockCDP18S604B"), GuiCdp18s600::onCdp18s604bRomBlock)

    EVT_TEXT(XRCID("VtCharRomCDP18S604B"), GuiMain::onVtCharRomText)
    EVT_COMBOBOX(XRCID("VtCharRomCDP18S604B"), GuiMain::onVtCharRomText)
    EVT_BUTTON(XRCID("VtCharRomButtonCDP18S604B"), GuiMain::onVtCharRom)

    EVT_TEXT(XRCID("KeyFileCDP18S604B"), GuiMain::onKeyFileText)
    EVT_BUTTON(XRCID("KeyFileButtonCDP18S604B"), GuiMain::onKeyFile)
    EVT_BUTTON(XRCID("EjectKeyFileCDP18S604B"), GuiMain::onKeyFileEject)

    EVT_CHOICE(XRCID("VTTypeCDP18S604B"), GuiMain::onVT100)
    EVT_SPIN_UP(XRCID("ZoomSpinVtCDP18S604B"), GuiMain::onZoomUpVt)
    EVT_SPIN_DOWN(XRCID("ZoomSpinVtCDP18S604B"), GuiMain::onZoomDownVt)
    EVT_TEXT(XRCID("ZoomValueVtCDP18S604B"), GuiMain::onZoomValueVt)
    EVT_BUTTON(XRCID("FullScreenF3CDP18S604B"), GuiMain::onFullScreen)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonCDP18S604B"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileCDP18S604B"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileCDP18S604B"), GuiMain::onScreenDumpFileText)
    EVT_BUTTON(XRCID("ScreenDumpF5CDP18S604B"), GuiMain::onScreenDump)

    EVT_CHECKBOX(XRCID("CDP18S604BForceUC"), GuiCdp18s600::onCdp18s600ForceUpperCase)
    EVT_BUTTON(XRCID("VtSetupCDP18S604B"), GuiMain::onVtSetup)
    EVT_CHECKBOX(XRCID("StretchDotCDP18S604B"), GuiMain::onStretchDot)
    EVT_BUTTON(XRCID("ColoursCDP18S604B"), Main::onColoursDef)

    EVT_BUTTON(XRCID("SaveButtonCDP18S604B"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonCDP18S604B"), GuiMain::onLoadButton)
    EVT_BUTTON(XRCID("RunButtonCDP18S604B"), GuiMain::onLoadRunButton)
    EVT_BUTTON(XRCID("DsaveButtonCDP18S604B"), GuiMain::onDataSaveButton)

    EVT_CHECKBOX(XRCID("UseLocationCDP18S604B"), GuiMain::onUseLocation)
    EVT_TEXT(XRCID("SaveStartCDP18S604B"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndCDP18S604B"), GuiMain::onSaveEnd)

    EVT_CHOICE(XRCID("VTBaudTChoiceCDP18S604B"), GuiCdp18s600::onCdp18s600BaudT)

    EVT_CHECKBOX(XRCID("AutoBootCDP18S604B"), GuiCdp18s600::onAutoBoot)
    EVT_CHOICE(XRCID("AutoBootTypeCDP18S604B"), GuiCdp18s600::onAutoBootType)

    EVT_TEXT(XRCID("ShowAddressCDP18S604B"), GuiMain::onLedTimer)
    EVT_CHECKBOX(XRCID("ControlWindowsCDP18S604B"), GuiCdp18s600::onCdp18s600ControlWindows)
    EVT_CHECKBOX(XRCID("CDP1852CDP18S604B"), GuiCdp18s600::onCdp1852Windows)

END_EVENT_TABLE()

GuiCdp18s600::GuiCdp18s600(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode_, wxString dataDir, wxString iniDir)
: GuiTMC2000(title, pos, size, mode_, dataDir, iniDir)
{
    position_.x = 0;
}

void GuiCdp18s600::readCdp18s600Config(int cdpType, wxString cdpTypeStr)
{
    selectedComputer_ = cdpType;
    
    conf[cdpType].loadFileNameFull_ = "";
    conf[cdpType].loadFileName_ = "";
    
    conf[cdpType].pLoadSaveName_[0] = 'S';
    conf[cdpType].pLoadSaveName_[1] = 'U';
    conf[cdpType].pLoadSaveName_[2] = 'P';
    conf[cdpType].pLoadSaveName_[3] = 'E';
    
    conf[cdpType].defus_ = 0x81;
    conf[cdpType].eop_ = 0x83;
    conf[cdpType].string_ = 0x92;
    conf[cdpType].arrayValue_ = 0x94;
    conf[cdpType].eod_ = 0x99;
    conf[cdpType].basicRamAddress_ = 0x200;
    
    conf[cdpType].saveStartString_ = "";
    conf[cdpType].saveEndString_ = "";
    conf[cdpType].saveExecString_ = "";
    
    conf[cdpType].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + cdpTypeStr + pathSeparator_;
    
    conf[cdpType].mainDir_ = readConfigDir("/Dir/" + cdpTypeStr + "/Main", dataDir_ + cdpTypeStr + pathSeparator_);
    conf[cdpType].romDir_[U21ROM] = readConfigDir("/Dir/" + cdpTypeStr + "/Main_Rom_File21", dataDir_ + cdpTypeStr + pathSeparator_);
    conf[cdpType].romDir_[U20ROM] = readConfigDir("/Dir/" + cdpTypeStr + "/Main_Rom_File20", dataDir_ + cdpTypeStr + pathSeparator_);
    conf[cdpType].romDir_[U19ROM] = readConfigDir("/Dir/" + cdpTypeStr + "/Main_Rom_File19", dataDir_ + cdpTypeStr + pathSeparator_);
    conf[cdpType].romDir_[U18ROM] = readConfigDir("/Dir/" + cdpTypeStr + "/Main_Rom_File18", dataDir_ + cdpTypeStr + pathSeparator_);
    conf[cdpType].romDir_[U17ROM] = readConfigDir("/Dir/" + cdpTypeStr + "/Main_Rom_File17", dataDir_ + cdpTypeStr + pathSeparator_);
    conf[cdpType].vtCharRomDir_ = readConfigDir("/Dir/" + cdpTypeStr + "/Vt_Font_Rom_File", dataDir_ + cdpTypeStr + pathSeparator_);
    conf[cdpType].keyFileDir_ = readConfigDir("/Dir/" + cdpTypeStr + "/Key_File", dataDir_ + cdpTypeStr + pathSeparator_);
    conf[cdpType].screenDumpFileDir_ = readConfigDir("/Dir/" + cdpTypeStr + "/Video_Dump_File", dataDir_ + cdpTypeStr + pathSeparator_);
    
    conf[cdpType].keyFile_ = configPointer->Read("/" + cdpTypeStr + "/Key_File", "");
    conf[cdpType].screenDumpFile_ = configPointer->Read("/" + cdpTypeStr + "/Video_Dump_File", "screendump.png");
    
    getConfigBool("/" + cdpTypeStr + "/SerialLog", false);
    
    configPointer->Read("/" + cdpTypeStr + "/Enable_Vt_Stretch_Dot", &conf[cdpType].stretchDot_, false);
    configPointer->Read("/" + cdpTypeStr + "/Enable_Vt_External", &elfConfiguration[cdpType].vtExternal, false);
    
    wxString defaultClock;

    switch (cdpType)
    {
        case CDP18S600:
            configPointer->Read("/" + cdpTypeStr + "/Uart", &elfConfiguration[cdpType].useUart, true);
            elfConfiguration[cdpType].baudR = (int)configPointer->Read("/" + cdpTypeStr + "/Vt_Baud_Receive", 0l);
            elfConfiguration[cdpType].baudT = (int)configPointer->Read("/" + cdpTypeStr + "/Vt_Baud_Transmit", 0l);
            defaultClock.Printf("%1.4f", 4.9152);

            conf[cdpType].rom_[U21ROM] = configPointer->Read("/" + cdpTypeStr + "/Main_Rom_File21", "");
            conf[cdpType].rom_[U20ROM] = configPointer->Read("/" + cdpTypeStr + "/Main_Rom_File20", "ut71.bin");
            conf[cdpType].rom_[U19ROM] = configPointer->Read("/" + cdpTypeStr + "/Main_Rom_File19", "");
            conf[cdpType].rom_[U18ROM] = configPointer->Read("/" + cdpTypeStr + "/Main_Rom_File18", "");
            conf[cdpType].rom_[U17ROM] = configPointer->Read("/" + cdpTypeStr + "/Main_Rom_File17", "");

            conf[cdpType].microChipType_[ONE_SOCKET] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipTypeOneSocket", 2l);
            conf[cdpType].microChipType_[FOUR_SOCKET] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipTypeFourSocket", 0l);

            conf[cdpType].microChipMemory_[U21ROM] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipMemoryU21", 1l);
            configPointer->Read("/" + cdpTypeStr + "/MicroChiDisableU21", &conf[cdpType].microChipDisable_[U21ROM], false);
            
            conf[cdpType].microChipMemory_[U20ROM] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipMemoryU20", 0l);
            configPointer->Read("/" + cdpTypeStr + "/MicroChiDisableU20", &conf[cdpType].microChipDisable_[U20ROM], false);
            
            conf[cdpType].microChipMemory_[U19ROM] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipMemoryU19", 1l);
            configPointer->Read("/" + cdpTypeStr + "/MicroChiDisableU19", &conf[cdpType].microChipDisable_[U19ROM], false);
            
            conf[cdpType].microChipMemory_[U18ROM] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipMemoryU18", 0l);
            configPointer->Read("/" + cdpTypeStr + "/MicroChiDisableU18", &conf[cdpType].microChipDisable_[U18ROM], true);
            
            conf[cdpType].microChipMemory_[U17ROM] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipMemoryU17", 0l);
            configPointer->Read("/" + cdpTypeStr + "/MicroChiDisableU17", &conf[cdpType].microChipDisable_[U17ROM], true);

            conf[cdpType].microChipLocation_[ONE_SOCKET] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipLocationOneSocket", 0l);
            conf[cdpType].microChipLocation_[FOUR_SOCKET] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipLocationFourSocket", 4l);

            elfConfiguration[cdpType].useCdp1852 = false;
            configPointer->Read("/" + cdpTypeStr + "/Pio_Windows", &elfConfiguration[cdpType].usePio, true);
        break;

        case CDP18S601:
        case CDP18S603A:
            configPointer->Read("/" + cdpTypeStr + "/Uart", &elfConfiguration[cdpType].useUart, false);
            elfConfiguration[cdpType].baudR = (int)configPointer->Read("/" + cdpTypeStr + "/Vt_Baud_Receive", 10l);
            elfConfiguration[cdpType].baudT = (int)configPointer->Read("/" + cdpTypeStr + "/Vt_Baud_Transmit", 10l);
            defaultClock.Printf("%1.1f", 2.0);
            
            conf[cdpType].rom_[U21ROM] = configPointer->Read("/" + cdpTypeStr + "/Main_Rom_File21", "");
            conf[cdpType].rom_[U20ROM] = configPointer->Read("/" + cdpTypeStr + "/Main_Rom_File20", "");
            conf[cdpType].rom_[U19ROM] = configPointer->Read("/" + cdpTypeStr + "/Main_Rom_File19", "");
            conf[cdpType].rom_[U18ROM] = configPointer->Read("/" + cdpTypeStr + "/Main_Rom_File18", "ut4.bin");
            conf[cdpType].rom_[U17ROM] = configPointer->Read("/" + cdpTypeStr + "/Main_Rom_File17", "");

            conf[cdpType].microChipType_[ONE_SOCKET] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipTypeOneSocket", 0l);
            conf[cdpType].microChipType_[FOUR_SOCKET] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipTypeFourSocket", 1l);

            conf[cdpType].microChipMemory_[U21ROM] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipMemoryU21", 1l);
            conf[cdpType].microChipDisable_[U21ROM] = false;
            
            conf[cdpType].microChipMemory_[XU27ROM] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipMemoryU20", 1l);
            conf[cdpType].microChipDisable_[XU27ROM] = false;
            
            conf[cdpType].microChipMemory_[XU26ROM] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipMemoryU19", 1l);
            conf[cdpType].microChipDisable_[XU26ROM] = false;
            
            conf[cdpType].microChipMemory_[XU25ROM] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipMemoryU18", 1l);
            conf[cdpType].microChipDisable_[XU25ROM] = false;
            
            conf[cdpType].microChipMemory_[XU24ROM] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipMemoryU17", 1l);
            conf[cdpType].microChipDisable_[XU24ROM] = false;
            
            conf[cdpType].microChipLocation_[FOUR_SOCKET_ROM1] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipLocationRom1Socket", 9l);
            conf[cdpType].microChipLocation_[FOUR_SOCKET_ROM2] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipLocationRom2Socket", 8l);

            elfConfiguration[cdpType].useCdp1852 = false;
            configPointer->Read("/" + cdpTypeStr + "/Pio_Windows", &elfConfiguration[cdpType].usePio, true);
        break;
            
        case CDP18S604B:
            configPointer->Read("/" + cdpTypeStr + "/Uart", &elfConfiguration[cdpType].useUart, false);
            elfConfiguration[cdpType].baudR = (int)configPointer->Read("/" + cdpTypeStr + "/Vt_Baud_Receive", 10l);
            elfConfiguration[cdpType].baudT = (int)configPointer->Read("/" + cdpTypeStr + "/Vt_Baud_Transmit", 10l);
            defaultClock.Printf("%1.6f", 2.097152);
            
            conf[cdpType].rom_[U21ROM] = configPointer->Read("/" + cdpTypeStr + "/Main_Rom_File21", "");
            conf[cdpType].rom_[U20ROM] = configPointer->Read("/" + cdpTypeStr + "/Main_Rom_File20", "");
            
            conf[cdpType].microChipType_[FOUR_SOCKET] = (int)configPointer->Read("/" + cdpTypeStr + "/MicroChipTypeFourSocket", 3l);
            
            wxString defaultBlock;
            defaultBlock.Printf("%d", 16);
            conf[cdpType].microChipBlock_[ONE_SOCKET] = configPointer->Read("/" + cdpTypeStr + "/MicroBlockOneSocket", defaultBlock);

            defaultBlock.Printf("%d", 0);
            conf[cdpType].microChipBlock_[FOUR_SOCKET] = configPointer->Read("/" + cdpTypeStr + "/MicroBlockFourSocket", defaultBlock);

            elfConfiguration[cdpType].usePio = false;
            configPointer->Read("/" + cdpTypeStr + "/Cdp1852_Windows", &elfConfiguration[cdpType].useCdp1852, true);
        break;
    }
    
    elfConfiguration[cdpType].uartGroup = (int)configPointer->Read("/" + cdpTypeStr + "/UartGroup", 0l);
    elfConfiguration[cdpType].bellFrequency_ = (int)configPointer->Read("/" + cdpTypeStr + "/Bell_Frequency", 800);
    elfConfiguration[cdpType].vtType = (int)configPointer->Read("/" + cdpTypeStr + "/VT_Type", 2l);
    elfConfiguration[cdpType].vt52SetUpFeature_ = configPointer->Read("/" + cdpTypeStr + "/VT52Setup", 0x00004092l);
    elfConfiguration[cdpType].vt100SetUpFeature_ = configPointer->Read("/" + cdpTypeStr + "/VT100Setup", 0x0000cad2l);
    elfConfiguration[cdpType].vtExternalSetUpFeature_ = configPointer->Read("/" + cdpTypeStr + "/VTExternalSetup", 0x0000cad2l);
    elfConfiguration[cdpType].serialPort_ = configPointer->Read("/" + cdpTypeStr + "/VtSerialPortChoice", "");
    elfConfiguration[cdpType].vtEf = false;
    elfConfiguration[cdpType].vtQ = true;

    configPointer->Read("/" + cdpTypeStr + "/Force_Uppercase", &elfConfiguration[cdpType].forceUpperCase, true);
    configPointer->Read("/" + cdpTypeStr + "/Open_Control_Windows", &elfConfiguration[cdpType].useElfControlWindows, true);

    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 1.0);
    conf[cdpType].zoomVt_ = configPointer->Read("/" + cdpTypeStr + "/Vt_Zoom", defaultZoom);
    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[cdpType].xScale_ = configPointer->Read("/" + cdpTypeStr + "/Window_Scale_Factor_X", defaultScale);
    
    conf[cdpType].clock_ = configPointer->Read("/" + cdpTypeStr + "/Clock_Speed", defaultClock);
    
    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[cdpType].ledTime_ = configPointer->Read("/" + cdpTypeStr + "/Led_Update_Frequency", defaultTimer);
    
    conf[cdpType].useLoadLocation_ = false;
    
    setVtType(cdpTypeStr, cdpType, elfConfiguration[cdpType].vtType, false);
    
    conf[cdpType].vtCharRom_ = configPointer->Read("/" + cdpTypeStr + "/Vt_Font_Rom_File", "vt100.bin");
    
    configPointer->Read("/" + cdpTypeStr + "/Enable_Auto_Boot", &elfConfiguration[cdpType].autoBoot, true);
    elfConfiguration[cdpType].autoBootType = (int)configPointer->Read("/" + cdpTypeStr + "/AutoBootType", 0l);
    if (elfConfiguration[cdpType].autoBootType == 0)
        conf[cdpType].bootAddress_ = 0x8000;
    else
        conf[cdpType].bootAddress_ = 0;

    if (mode_.gui)
    {
        XRCCTRL(*this, "MainRomU21" + cdpTypeStr, wxComboBox)->SetValue(conf[cdpType].rom_[U21ROM]);
        XRCCTRL(*this, "MainRomU20" + cdpTypeStr, wxComboBox)->SetValue(conf[cdpType].rom_[U20ROM]);
        XRCCTRL(*this, "VtCharRom" + cdpTypeStr, wxComboBox)->SetValue(conf[cdpType].vtCharRom_);
        XRCCTRL(*this, "KeyFile" + cdpTypeStr, wxTextCtrl)->SetValue(conf[cdpType].keyFile_);
        XRCCTRL(*this, "ScreenDumpFile" + cdpTypeStr, wxComboBox)->SetValue(conf[cdpType].screenDumpFile_);
        
        XRCCTRL(*this, "VTType" + cdpTypeStr, wxChoice)->SetSelection(elfConfiguration[cdpType].vtType);
        XRCCTRL(*this, cdpTypeStr + "ForceUC", wxCheckBox)->SetValue(elfConfiguration[cdpType].forceUpperCase);
        
        XRCCTRL(*this, "VTBaudRChoice" + cdpTypeStr, wxChoice)->SetSelection(elfConfiguration[cdpType].baudR);
        XRCCTRL(*this, "VTBaudTChoice" + cdpTypeStr, wxChoice)->SetSelection(elfConfiguration[cdpType].baudT);
        
        XRCCTRL(*this, "ZoomValueVt" + cdpTypeStr, wxTextCtrl)->ChangeValue(conf[cdpType].zoomVt_);
        XRCCTRL(*this, "StretchDot" + cdpTypeStr, wxCheckBox)->SetValue(conf[cdpType].stretchDot_);
        
        clockTextCtrl[cdpType]->ChangeValue(conf[cdpType].clock_);
        
        XRCCTRL(*this, "UseLocation" + cdpTypeStr, wxCheckBox)->SetValue(conf[cdpType].useLoadLocation_);
        
        XRCCTRL(*this, "FourSocketBank" + cdpTypeStr, wxChoice)->SetSelection(conf[cdpType].microChipType_[FOUR_SOCKET]);

        switch (cdpType)
        {
            case CDP18S600:
            case CDP18S601:
            case CDP18S603A:
                XRCCTRL(*this, "MainRomU19" + cdpTypeStr, wxComboBox)->SetValue(conf[cdpType].rom_[U19ROM]);
                XRCCTRL(*this, "MainRomU18" + cdpTypeStr, wxComboBox)->SetValue(conf[cdpType].rom_[U18ROM]);
                XRCCTRL(*this, "MainRomU17" + cdpTypeStr, wxComboBox)->SetValue(conf[cdpType].rom_[U17ROM]);
                XRCCTRL(*this, "OneSocketBank" + cdpTypeStr, wxChoice)->SetSelection(conf[cdpType].microChipType_[ONE_SOCKET]);
                setOneSocketState(cdpTypeStr);
                setFourSocketState(cdpType, cdpTypeStr);
                XRCCTRL(*this, "Pio" + cdpTypeStr, wxCheckBox)->SetValue(elfConfiguration[cdpType].usePio);
            break;
                
            case CDP18S604B:
                XRCCTRL(*this, "RamBlock" + cdpTypeStr, wxTextCtrl)->ChangeValue(conf[cdpType].microChipBlock_[ONE_SOCKET]);
                XRCCTRL(*this, "RomBlock" + cdpTypeStr, wxTextCtrl)->ChangeValue(conf[cdpType].microChipBlock_[FOUR_SOCKET]);
                cdp18s604bOneSocketBankGui();
                cdp18s604bFourSocketBankGui();
                XRCCTRL(*this, "CDP1852" + cdpTypeStr, wxCheckBox)->SetValue(elfConfiguration[cdpType].useCdp1852);
            break;
        }

        
        XRCCTRL(*this, "ShowAddress" + cdpTypeStr, wxTextCtrl)->ChangeValue(conf[cdpType].ledTime_);
        XRCCTRL(*this,"ShowAddress" + cdpTypeStr, wxTextCtrl)->Enable(elfConfiguration[cdpType].useElfControlWindows);
        XRCCTRL(*this, "AutoBoot" + cdpTypeStr, wxCheckBox)->SetValue(elfConfiguration[cdpType].autoBoot);
        XRCCTRL(*this, "AutoBootType" + cdpTypeStr, wxChoice)->SetSelection(elfConfiguration[cdpType].autoBootType);

        XRCCTRL(*this,"AddressText1" + cdpTypeStr, wxStaticText)->Enable(elfConfiguration[cdpType].useElfControlWindows);
        XRCCTRL(*this,"AddressText2" + cdpTypeStr, wxStaticText)->Enable(elfConfiguration[cdpType].useElfControlWindows);
        XRCCTRL(*this, "ControlWindows" + cdpTypeStr, wxCheckBox)->SetValue(elfConfiguration[cdpType].useElfControlWindows);
    }
}

void GuiCdp18s600::writeCdp18s600DirConfig(int cdpType, wxString cdpTypeStr)
{
    writeConfigDir("/Dir/" + cdpTypeStr + "/Main", conf[cdpType].mainDir_);
    writeConfigDir("/Dir/" + cdpTypeStr + "/Main_Rom_File21", conf[cdpType].romDir_[U21ROM]);
    writeConfigDir("/Dir/" + cdpTypeStr + "/Main_Rom_File20", conf[cdpType].romDir_[U20ROM]);
    writeConfigDir("/Dir/" + cdpTypeStr + "/Main_Rom_File19", conf[cdpType].romDir_[U19ROM]);
    writeConfigDir("/Dir/" + cdpTypeStr + "/Main_Rom_File18", conf[cdpType].romDir_[U18ROM]);
    writeConfigDir("/Dir/" + cdpTypeStr + "/Main_Rom_File18", conf[cdpType].romDir_[U17ROM]);
    writeConfigDir("/Dir/" + cdpTypeStr + "/Vt_Font_Rom_File", conf[cdpType].vtCharRomDir_);
    writeConfigDir("/Dir/" + cdpTypeStr + "/Key_File", conf[cdpType].keyFileDir_);
    writeConfigDir("/Dir/" + cdpTypeStr + "/Video_Dump_File", conf[cdpType].screenDumpFileDir_);
}

void GuiCdp18s600::writeCdp18s600Config(int cdpType, wxString cdpTypeStr)
{
    writeElfPortConfig(cdpType, cdpTypeStr);
    
    configPointer->Write("/" + cdpTypeStr + "/Main_Rom_File21", conf[cdpType].rom_[U21ROM]);
    configPointer->Write("/" + cdpTypeStr + "/Main_Rom_File20", conf[cdpType].rom_[U20ROM]);
    configPointer->Write("/" + cdpTypeStr + "/Main_Rom_File19", conf[cdpType].rom_[U19ROM]);
    configPointer->Write("/" + cdpTypeStr + "/Main_Rom_File18", conf[cdpType].rom_[U18ROM]);
    configPointer->Write("/" + cdpTypeStr + "/Main_Rom_File17", conf[cdpType].rom_[U17ROM]);
    configPointer->Write("/" + cdpTypeStr + "/Vt_Font_Rom_File", conf[cdpType].vtCharRom_);
    configPointer->Write("/" + cdpTypeStr + "/Key_File", conf[cdpType].keyFile_);
    configPointer->Write("/" + cdpTypeStr + "/Video_Dump_File", conf[cdpType].screenDumpFile_);
    configPointer->Write("/" + cdpTypeStr + "/VtSerialPortChoice", elfConfiguration[cdpType].serialPort_);
    
    configPointer->Write("/" + cdpTypeStr + "/Bell_Frequency", elfConfiguration[cdpType].bellFrequency_);
    configPointer->Write("/" + cdpTypeStr + "/VT_Type", elfConfiguration[cdpType].vtType);
    
    long value = elfConfiguration[cdpType].vt52SetUpFeature_.to_ulong();
    configPointer->Write("/" + cdpTypeStr + "/VT52Setup", value);
    value = elfConfiguration[cdpType].vt100SetUpFeature_.to_ulong();
    configPointer->Write("/" + cdpTypeStr + "/VT100Setup", value);
    value = elfConfiguration[cdpType].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("/" + cdpTypeStr + "/VTExternalSetup", value);
    configPointer->Write("/" + cdpTypeStr + "/Uart", elfConfiguration[cdpType].useUart);
    configPointer->Write("/" + cdpTypeStr + "/UartGroup", elfConfiguration[cdpType].uartGroup);

    configPointer->Write("/" + cdpTypeStr + "/Vt_Baud_Receive", elfConfiguration[cdpType].baudR);
    configPointer->Write("/" + cdpTypeStr + "/Vt_Baud_Transmit", elfConfiguration[cdpType].baudT);
    configPointer->Write("/" + cdpTypeStr + "/Vt_Zoom", conf[cdpType].zoomVt_);
    configPointer->Write("/" + cdpTypeStr + "/Force_Uppercase", elfConfiguration[cdpType].forceUpperCase);
    configPointer->Write("/" + cdpTypeStr + "/Enable_Vt_Stretch_Dot", conf[cdpType].stretchDot_);
    configPointer->Write("/" + cdpTypeStr + "/Enable_Vt_External", elfConfiguration[cdpType].vtExternal);
    configPointer->Write("/" + cdpTypeStr + "/Open_Control_Windows", elfConfiguration[cdpType].useElfControlWindows);
    configPointer->Write("/" + cdpTypeStr + "/Led_Update_Frequency", conf[cdpType].ledTime_);


    configPointer->Write("/" + cdpTypeStr + "/MicroChipLocationOneSocket", conf[cdpType].microChipLocation_[ONE_SOCKET]);
    configPointer->Write("/" + cdpTypeStr + "/MicroChipTypeOneSocket", conf[cdpType].microChipType_[ONE_SOCKET]);
    configPointer->Write("/" + cdpTypeStr + "/MicroChipTypeFourSocket", conf[cdpType].microChipType_[FOUR_SOCKET]);

    switch (cdpType)
    {
        case CDP18S600:
            configPointer->Write("/" + cdpTypeStr + "/MicroChipLocationFourSocket", conf[cdpType].microChipLocation_[FOUR_SOCKET]);
            configPointer->Write("/" + cdpTypeStr + "/Pio_Windows", elfConfiguration[cdpType].usePio);
        break;

        case CDP18S601:
        case CDP18S603A:
            configPointer->Write("/" + cdpTypeStr + "/MicroChipLocationRom1Socket", conf[cdpType].microChipLocation_[FOUR_SOCKET_ROM1]);
            configPointer->Write("/" + cdpTypeStr + "/MicroChipLocationRom2Socket", conf[cdpType].microChipLocation_[FOUR_SOCKET_ROM2]);
            configPointer->Write("/" + cdpTypeStr + "/Pio_Windows", elfConfiguration[cdpType].usePio);
        break;

        case CDP18S604B:
            configPointer->Write("/" + cdpTypeStr + "/MicroBlockOneSocket", conf[cdpType].microChipBlock_[ONE_SOCKET]);
            configPointer->Write("/" + cdpTypeStr + "/MicroBlockFourSocket", conf[cdpType].microChipBlock_[FOUR_SOCKET]);
            configPointer->Write("/" + cdpTypeStr + "/Cdp1852_Windows", elfConfiguration[cdpType].useCdp1852);
        break;
    }

    configPointer->Write("/" + cdpTypeStr + "/MicroChipMemoryU21", conf[cdpType].microChipMemory_[U21ROM]);
    configPointer->Write("/" + cdpTypeStr + "/MicroChiDisableU21", conf[cdpType].microChipDisable_[U21ROM]);

    configPointer->Write("/" + cdpTypeStr + "/MicroChipMemoryU20", conf[cdpType].microChipMemory_[U20ROM]);
    configPointer->Write("/" + cdpTypeStr + "/MicroChiDisableU20", conf[cdpType].microChipDisable_[U20ROM]);

    configPointer->Write("/" + cdpTypeStr + "/MicroChipMemoryU19", conf[cdpType].microChipMemory_[U19ROM]);
    configPointer->Write("/" + cdpTypeStr + "/MicroChiDisableU19", conf[cdpType].microChipDisable_[U19ROM]);

    configPointer->Write("/" + cdpTypeStr + "/MicroChipMemoryU18", conf[cdpType].microChipMemory_[U18ROM]);
    configPointer->Write("/" + cdpTypeStr + "/MicroChiDisableU18", conf[cdpType].microChipDisable_[U18ROM]);

    configPointer->Write("/" + cdpTypeStr + "/MicroChipMemoryU17", conf[cdpType].microChipMemory_[U17ROM]);
    configPointer->Write("/" + cdpTypeStr + "/MicroChiDisableU17", conf[cdpType].microChipDisable_[U17ROM]);

    configPointer->Write("/" + cdpTypeStr + "/Enable_Auto_Boot", elfConfiguration[cdpType].autoBoot);
    configPointer->Write("/" + cdpTypeStr + "/AutoBootType", elfConfiguration[cdpType].autoBootType);

    configPointer->Write("/" + cdpTypeStr + "/Clock_Speed", conf[cdpType].clock_);
}

void GuiCdp18s600::readCdp18s600WindowConfig(int cdpType, wxString cdpTypeStr)
{
    conf[cdpType].vtX_ = (int)configPointer->Read("/" + cdpTypeStr + "/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX);
    conf[cdpType].vtY_ = (int)configPointer->Read("/" + cdpTypeStr + "/Window_Position_Vt_Y", mainWindowY_);
    conf[cdpType].mainX_ = (int)configPointer->Read("/" + cdpTypeStr + "/Window_Position_X", mainWindowX_);
    conf[cdpType].mainY_ = (int)configPointer->Read("/" + cdpTypeStr + "/Window_Position_Y", mainWindowY_+windowInfo.mainwY);
    conf[cdpType].secondFrameX_ = (int)configPointer->Read("/" + cdpTypeStr + "/Window_Position_SecondFrame_X", mainWindowX_ + 310);
    conf[cdpType].secondFrameY_ = (int)configPointer->Read("/" + cdpTypeStr + "/Window_Position_SecondFrame_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiCdp18s600::writeCdp18s600WindowConfig(int cdpType, wxString cdpTypeStr)
{
    if (conf[cdpType].vtX_ > 0)
        configPointer->Write("/" + cdpTypeStr + "/Window_Position_Vt_X", conf[cdpType].vtX_);
    if (conf[cdpType].vtY_ > 0)
        configPointer->Write("/" + cdpTypeStr + "/Window_Position_Vt_Y", conf[cdpType].vtY_);
    if (conf[cdpType].mainX_ > 0)
        configPointer->Write("/" + cdpTypeStr + "/Window_Position_X", conf[cdpType].mainX_);
    if (conf[cdpType].mainY_ > 0)
        configPointer->Write("/" + cdpTypeStr + "/Window_Position_Y", conf[cdpType].mainY_);
    if (conf[cdpType].secondFrameX_ > 0)
        configPointer->Write("/" + cdpTypeStr + "/Window_Position_SecondFrame_X", conf[cdpType].secondFrameX_);
    if (conf[cdpType].secondFrameY_ > 0)
        configPointer->Write("/" + cdpTypeStr + "/Window_Position_SecondFrame_Y", conf[cdpType].secondFrameY_);
}

void GuiCdp18s600::onCdp18s600BaudT(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].baudT = event.GetSelection();
    elfConfiguration[selectedComputer_].baudR = event.GetSelection();
    XRCCTRL(*this, "VTBaudRChoice"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(elfConfiguration[selectedComputer_].baudR);
}

void GuiCdp18s600::onCdp18s600ForceUpperCase(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].forceUpperCase = event.IsChecked();
    if (runningComputer_ == selectedComputer_)
    {
        p_Computer->setForceUpperCase(event.IsChecked());
    }
}

void GuiCdp18s600::onOneSocketBank(wxCommandEvent&event)
{
    conf[selectedComputer_].microChipType_[ONE_SOCKET] = event.GetSelection();
}

void GuiCdp18s600::onFourSocketBank(wxCommandEvent&event)
{
    conf[selectedComputer_].microChipType_[FOUR_SOCKET] = event.GetSelection();
}

void GuiCdp18s600::onCdp18s604bFourSocketBank(wxCommandEvent&event)
{
    wxString blockStr = XRCCTRL(*this, "RomBlock"+computerInfo[selectedComputer_].gui, wxTextCtrl)->GetValue();
    
    long block;
    blockStr.ToLong(&block);

    int oldType = convertCdp18s604ChipType(conf[selectedComputer_].microChipType_[FOUR_SOCKET]);
    
    conf[selectedComputer_].microChipType_[FOUR_SOCKET] = event.GetSelection();
    int newType = convertCdp18s604ChipType(conf[selectedComputer_].microChipType_[FOUR_SOCKET]);
    
    double multiplier = (double) (oldType) / (double) (newType);
    block = block * multiplier;
    
    blockStr.Printf("%d", (int)block);
    XRCCTRL(*this, "RomBlock"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue(blockStr);
    
    cdp18s604bFourSocketBankGui();
}

int GuiCdp18s600::convertCdp18s604ChipType(int type)
{
    if (type == 0)
        type = 1;
    
    type = 1 << (type-1);
    return type;
}

void GuiCdp18s600::onCdp18s604bRamBlock(wxCommandEvent&event)
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

    conf[selectedComputer_].microChipBlock_[ONE_SOCKET] = stringBlock;

    cdp18s604bOneSocketBankGui();
}

void GuiCdp18s600::onCdp18s604bRomBlock(wxCommandEvent&event)
{
    int chipType = convertCdp18s604ChipType(conf[selectedComputer_].microChipType_[FOUR_SOCKET]);
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

    conf[selectedComputer_].microChipBlock_[FOUR_SOCKET] = stringBlock;

    cdp18s604bFourSocketBankGui();
}

void GuiCdp18s600::cdp18s604bOneSocketBankGui()
{
    long block;
    conf[selectedComputer_].microChipBlock_[ONE_SOCKET].ToLong(&block);
    
    wxString label;
    label.Printf("address: %04X-%04X", (int)(block*0x400), (int)(block*0x400+0x3ff));
    XRCCTRL(*this, "RamAddress"+computerInfo[selectedComputer_].gui, wxStaticText)->SetLabel(label);
}

void GuiCdp18s600::cdp18s604bFourSocketBankGui()
{
    long block;

    int chipType = convertCdp18s604ChipType(conf[selectedComputer_].microChipType_[FOUR_SOCKET]);

    wxString label;
    label.Printf("%dK Block (0-%d)", chipType, (64/chipType)-1);
    XRCCTRL(*this, "RomBlockText"+computerInfo[selectedComputer_].gui, wxStaticText)->SetLabel(label);

    conf[selectedComputer_].microChipBlock_[FOUR_SOCKET].ToLong(&block);

    if (conf[selectedComputer_].microChipType_[FOUR_SOCKET] == 0)
        label.Printf("address: %04X-%04X", (int)(block*chipType*0x400), (int)(block*chipType*0x400+0x1ff));
    else
        label.Printf("address: %04X-%04X", (int)(block*chipType*0x400), (int)(block*chipType*0x400+chipType*0x400-1));
    XRCCTRL(*this, "RomAddress"+computerInfo[selectedComputer_].gui, wxStaticText)->SetLabel(label);
}

void GuiCdp18s600::onRomU21(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" U21 ROM file to load",
                              conf[selectedComputer_].romDir_[U21ROM], XRCCTRL(*this, "MainRomU21"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
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
    conf[selectedComputer_].romDir_[U21ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].rom_[U21ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU21"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[U21ROM]);
}

void GuiCdp18s600::onRomU21Text(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].rom_[U21ROM] = XRCCTRL(*this, "MainRomU21"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiCdp18s600::onRomU20(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" U210 ROM file to load",
                              conf[selectedComputer_].romDir_[U20ROM], XRCCTRL(*this, "MainRomU20"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
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
    conf[selectedComputer_].romDir_[U20ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].rom_[U20ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU20"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[U20ROM]);
}

void GuiCdp18s600::onRomU20Text(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].rom_[U20ROM] = XRCCTRL(*this, "MainRomU20"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiCdp18s600::onRomU19(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" U19 ROM file to load",
                              conf[selectedComputer_].romDir_[U19ROM], XRCCTRL(*this, "MainRomU19"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
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
    conf[selectedComputer_].romDir_[U19ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].rom_[U19ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU19"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[U19ROM]);
}

void GuiCdp18s600::onRomU19Text(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].rom_[U19ROM] = XRCCTRL(*this, "MainRomU19"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiCdp18s600::onRomU18(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" U18 ROM file to load",
                              conf[selectedComputer_].romDir_[U18ROM], XRCCTRL(*this, "MainRomU18"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
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
    conf[selectedComputer_].romDir_[U18ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].rom_[U18ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU81"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[U18ROM]);
}

void GuiCdp18s600::onRomU18Text(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].rom_[U18ROM] = XRCCTRL(*this, "MainRomU18"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiCdp18s600::onRomU17(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" U21 ROM file to load",
                              conf[selectedComputer_].romDir_[U17ROM], XRCCTRL(*this, "MainRomU17"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
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
    conf[selectedComputer_].romDir_[U17ROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].rom_[U17ROM] = FullPath.GetFullName();
    
    XRCCTRL(*this, "MainRomU17"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[U17ROM]);
}

void GuiCdp18s600::onRomU17Text(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].rom_[U17ROM] = XRCCTRL(*this, "MainRomU17"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiCdp18s600::onOneSocketSetup(wxCommandEvent&WXUNUSED(event))
{
    MicroOneSocketSetupDialog MicroOneSocketSetupDialog(this);
    MicroOneSocketSetupDialog.ShowModal();
}

void GuiCdp18s600::setOneSocketState(wxString cdpTypeStr)
{
    setRamlabel(U21ROM, "U21", cdpTypeStr, "U21");
}

void GuiCdp18s600::setRamlabel(int romNumber, wxString romString, wxString cdpTypeStr, wxString label)
{
    wxString buttonString = "RomButton" + romString + cdpTypeStr;
    wxString comboString = "MainRom" + romString + cdpTypeStr;
    wxString typeString = "ROM ";
    
    switch (conf[selectedComputer_].microChipMemory_[romNumber])
    {
        case MICRO_RAM:
            typeString = "RAM ";
        break;
            
        case MICRO_CPURAM:
            typeString = "CRAM ";
        break;
    }
    XRCCTRL(*this, buttonString, wxButton)->SetLabel(typeString + label);

    if (conf[selectedComputer_].microChipDisable_[romNumber])
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

void GuiCdp18s600::onFourSocketSetup(wxCommandEvent&WXUNUSED(event))
{
    MicroFourSocketSetupDialog MicroFourSocketSetupDialog(this);
    MicroFourSocketSetupDialog.ShowModal();
}

void GuiCdp18s600::onRomSocketSetup(wxCommandEvent&WXUNUSED(event))
{
    MicroRomSocketSetupDialog MicroRomSocketSetupDialog(this);
    MicroRomSocketSetupDialog.ShowModal();
}

void GuiCdp18s600::onRom603ASocketSetup(wxCommandEvent&WXUNUSED(event))
{
    MicroRom603ASocketSetupDialog MicroRom603ASocketSetupDialog(this);
    MicroRom603ASocketSetupDialog.ShowModal();
}

void GuiCdp18s600::setFourSocketState(int cdpType, wxString cdpTypeStr)
{
    if (cdpType == CDP18S600)
    {
        setRamlabel(U20ROM, "U20", cdpTypeStr, "U20");
        setRamlabel(U19ROM, "U19", cdpTypeStr, "U19");
        setRamlabel(U18ROM, "U18", cdpTypeStr, "U18");
        setRamlabel(U17ROM, "U17", cdpTypeStr, "U17");
    }
}

void GuiCdp18s600::onCdp18s600ControlWindows(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].useElfControlWindows = event.IsChecked();
    
    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddress" + computerInfo[selectedComputer_].gui,wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].usePio | elfConfiguration[selectedComputer_].useCdp1852);
        XRCCTRL(*this,"AddressText1" + computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].usePio | elfConfiguration[selectedComputer_].useCdp1852);
        XRCCTRL(*this,"AddressText2" + computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].usePio | elfConfiguration[selectedComputer_].useCdp1852);
    }
    
    if (runningComputer_ == selectedComputer_)
    {
        switch (runningComputer_)
        {
            case CDP18S600:
                p_Cdp18s600->Show(elfConfiguration[selectedComputer_].useElfControlWindows);
            break;

            case CDP18S601:
                p_Cdp18s601->Show(elfConfiguration[selectedComputer_].useElfControlWindows);
            break;

            case CDP18S603A:
                p_Cdp18s603a->Show(elfConfiguration[selectedComputer_].useElfControlWindows);
            break;

            case CDP18S604B:
                p_Cdp18s604b->Show(elfConfiguration[selectedComputer_].useElfControlWindows);
            break;
        }
    }
}

void GuiCdp18s600::onPioWindows(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].usePio = event.IsChecked();
    
    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddress" + computerInfo[selectedComputer_].gui,wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].usePio);
        XRCCTRL(*this,"AddressText1" + computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].usePio);
        XRCCTRL(*this,"AddressText2" + computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].usePio);
    }
    
    if (runningComputer_ == selectedComputer_)
    {
        switch (runningComputer_)
        {
            case CDP18S600:
                p_Cdp18s600->showPio(elfConfiguration[selectedComputer_].usePio);
            break;
                
            case CDP18S601:
                p_Cdp18s601->showPio(elfConfiguration[selectedComputer_].usePio);
            break;

            case CDP18S603A:
                p_Cdp18s603a->showPio(elfConfiguration[selectedComputer_].usePio);
            break;
        }
    }
}

void GuiCdp18s600::onCdp1852Windows(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].useCdp1852 = event.IsChecked();
    
    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddress" + computerInfo[selectedComputer_].gui,wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].useCdp1852);
        XRCCTRL(*this,"AddressText1" + computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].useCdp1852);
        XRCCTRL(*this,"AddressText2" + computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows | elfConfiguration[selectedComputer_].useCdp1852);
    }
    
    if (runningComputer_ == selectedComputer_)
    {
        switch (runningComputer_)
        {
            case CDP18S604B:
                p_Cdp18s604b->showCdp1852(elfConfiguration[selectedComputer_].useCdp1852);
            break;
        }
    }
}

void GuiCdp18s600::pioWindows(int computerType, bool state)
{
    elfConfiguration[computerType].usePio = state;
    XRCCTRL(*this, "Pio" + computerInfo[computerType].gui, wxCheckBox)->SetValue(elfConfiguration[computerType].usePio);
}

void GuiCdp18s600::cdp1852Windows(int computerType, bool state)
{
    elfConfiguration[computerType].useCdp1852 = state;
    XRCCTRL(*this, "CDP1852" + computerInfo[computerType].gui, wxCheckBox)->SetValue(elfConfiguration[computerType].useCdp1852);
}

bool GuiCdp18s600::getUseCdp18s600ControlWindows()
{
    return elfConfiguration[selectedComputer_].useElfControlWindows;
}

bool GuiCdp18s600::getUsePioWindows()
{
    return elfConfiguration[selectedComputer_].usePio;
}

void GuiCdp18s600::onAutoBoot(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].autoBoot = event.IsChecked();
}

void GuiCdp18s600::onAutoBootType(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].autoBootType = event.GetSelection();
    if (elfConfiguration[selectedComputer_].autoBootType == 0)
        conf[selectedComputer_].bootAddress_ = 0x8000;
    else
        conf[selectedComputer_].bootAddress_ = 0;
    
    if (runningComputer_ == selectedComputer_)
    {
        if (runningComputer_ == selectedComputer_)
        {
            switch (runningComputer_)
            {
                case CDP18S600:
                    p_Cdp18s600->setAddressLatch(conf[selectedComputer_].bootAddress_);
                break;
                    
                case CDP18S601:
                    p_Cdp18s601->setAddressLatch(conf[selectedComputer_].bootAddress_);
                break;

                case CDP18S603A:
                    p_Cdp18s603a->setAddressLatch(conf[selectedComputer_].bootAddress_);
                break;

                case CDP18S604B:
                    p_Cdp18s604b->setAddressLatch(conf[selectedComputer_].bootAddress_);
                break;
            }
        }
    }
}



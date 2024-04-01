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
#include "guixml.h"

BEGIN_EVENT_TABLE(GuiXml, GuiPico)

    EVT_TEXT(XRCID("RomRam1Xml"), GuiXml::onRomRom1TextXml)
    EVT_COMBOBOX(XRCID("RomRam1Xml"), GuiXml::onRomRom1ComboXml)
    EVT_BUTTON(XRCID("RomRamButton1Xml"), GuiXml::onRomRam1Xml)

    EVT_TEXT(XRCID("RomRam0Xml"), GuiXml::onRomRom0TextXml)
    EVT_COMBOBOX(XRCID("RomRam0Xml"), GuiXml::onRomRom0ComboXml)
    EVT_BUTTON(XRCID("RomRamButton0Xml"), GuiXml::onRomRam0Xml)

    EVT_TEXT(XRCID("MainXmlXml"), GuiXml::onMainXmlTextXml)
    EVT_COMBOBOX(XRCID("MainXmlXml"), GuiXml::onMainXmlComboXml)
    EVT_BUTTON(XRCID("XmlButtonXml"), GuiXml::onMainXmlXml)

    EVT_CHOICE(XRCID("MainDirXml"), GuiXml::onMainDirComboXml)

    EVT_TEXT(XRCID("KeyFileXml"), GuiMain::onKeyFileText)
    EVT_BUTTON(XRCID("KeyFileButtonXml"), GuiMain::onKeyFile)
    EVT_BUTTON(XRCID("EjectKeyFileXml"), GuiMain::onKeyFileEject)

    EVT_CHOICE(XRCID("VTTypeXml"), GuiXml::onVT100)

    EVT_SPIN_UP(XRCID("ZoomSpinXml"), GuiMain::onZoom)
    EVT_SPIN_DOWN(XRCID("ZoomSpinXml"), GuiMain::onZoom)
    EVT_TEXT(XRCID("ZoomValueXml"), GuiMain::onZoomValue)
    EVT_SPIN_UP(XRCID("ZoomSpinVtXml"), GuiMain::onZoomVt)
    EVT_SPIN_DOWN(XRCID("ZoomSpinVtXml"), GuiMain::onZoomVt)
    EVT_TEXT(XRCID("ZoomValueVtXml"), GuiMain::onZoomValueVt)
    EVT_CHECKBOX(XRCID("InterlaceXml"), GuiMain::onInterlace)
    EVT_CHECKBOX(XRCID("StretchDotXml"), GuiMain::onStretchDot)
    EVT_BUTTON(XRCID("ScreenDumpFileButtonXml"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileXml"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileXml"), GuiMain::onScreenDumpFileText)
    EVT_BUTTON(XRCID("ScreenDumpF5Xml"), GuiMain::onScreenDump)
    EVT_BUTTON(XRCID("SaveButtonXml"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("VtSetupXml"), GuiMain::onVtSetup)
    EVT_BUTTON(XRCID("LoadButtonXml"), GuiMain::onLoadButton)
    EVT_BUTTON(XRCID("RunButtonXml"), GuiMain::onLoadRunButton)
    EVT_BUTTON(XRCID("DsaveButtonXml"), GuiMain::onDataSaveButton)
    EVT_CHECKBOX(XRCID("UseLocationXml"), GuiMain::onUseLocation)

    EVT_BUTTON(XRCID("FullScreenF3Xml"), GuiMain::onFullScreen)

    EVT_TEXT(XRCID("PrintFileXml"), GuiXml::onXmlPrintFileText)
    EVT_BUTTON(XRCID("PrintButtonXml"), GuiXml::onXmlPrintButton)
    EVT_CHOICE(XRCID("PrintModeXml"), GuiXml::onXmlPrintMode)
    EVT_BUTTON(XRCID("PrintFileButtonXml"), GuiMain::onPrintFile)
        
    EVT_CHOICE(XRCID("VTBaudTChoiceXml"), GuiXml::onXmlBaudT)
    EVT_CHOICE(XRCID("VTBaudRChoiceXml"), GuiXml::onXmlBaudR)

    EVT_CHECKBOX(XRCID("ControlWindowsXml"), GuiXml::onXmlControlWindows)
    EVT_BUTTON(XRCID("KeyMapXml"), Main::onHexKeyDef)

    EVT_BUTTON(XRCID("FDC0_ButtonXml"), GuiMain::onUpdDisk0)
    EVT_TEXT(XRCID("FDC0_FileXml"), GuiMain::onUpdDiskText0)
    EVT_BUTTON(XRCID("Eject_FDC0Xml"), GuiMain::onUpdDiskEject0)
    EVT_BUTTON(XRCID("FDC0_SwitchXml"), GuiMain::onUpdDiskDirSwitch0)
    EVT_BUTTON(XRCID("FDC1_ButtonXml"), GuiMain::onUpdDisk1)
    EVT_TEXT(XRCID("FDC1_FileXml"), GuiMain::onUpdDiskText1)
    EVT_BUTTON(XRCID("Eject_FDC1Xml"), GuiMain::onUpdDiskEject1)
    EVT_BUTTON(XRCID("FDC1_SwitchXml"), GuiMain::onUpdDiskDirSwitch1)
    EVT_BUTTON(XRCID("FDC2_ButtonXml"), GuiMain::onUpdDisk2)
    EVT_TEXT(XRCID("FDC2_FileXml"), GuiMain::onUpdDiskText2)
    EVT_BUTTON(XRCID("Eject_FDC2Xml"), GuiMain::onUpdDiskEject2)
    EVT_BUTTON(XRCID("FDC2_SwitchXml"), GuiMain::onUpdDiskDirSwitch2)
    EVT_BUTTON(XRCID("FDC3_ButtonXml"), GuiMain::onUpdDisk3)
    EVT_TEXT(XRCID("FDC3_FileXml"), GuiMain::onUpdDiskText3)
    EVT_BUTTON(XRCID("Eject_FDC3Xml"), GuiMain::onUpdDiskEject3)
    EVT_BUTTON(XRCID("FDC3_SwitchXml"), GuiMain::onUpdDiskDirSwitch3)

    EVT_BUTTON(XRCID("CasButtonXml"), GuiMain::onCassette)
    EVT_BUTTON(XRCID("EjectCasXml"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileXml"), GuiMain::onCassetteText)

    EVT_BUTTON(XRCID("CasButton1Xml"), GuiMain::onCassette1)
    EVT_BUTTON(XRCID("EjectCas1Xml"), GuiMain::onCassette1Eject)
    EVT_TEXT(XRCID("WavFile1Xml"), GuiMain::onCassette1Text)

    EVT_BUTTON(XRCID("XmodemButtonXml"), GuiMain::onXmodem)
    EVT_BUTTON(XRCID("EjectXmodemXml"), GuiMain::onXmodemEject)
    EVT_TEXT(XRCID("XmodemFileXml"), GuiMain::onXmodemText)

    EVT_BUTTON(XRCID("RealCasLoadXml"), GuiMain::onRealCas)
    EVT_BUTTON(XRCID("CasStopXml"), GuiMain::onCassetteStop)
    EVT_BUTTON(XRCID("CasSaveXml"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasRewindXml"), GuiMain::onCassetteRewind)
    EVT_BUTTON(XRCID("CasLoadXml"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasForwardXml"), GuiMain::onCassetteForward)
    EVT_BUTTON(XRCID("CasPauseXml"), GuiMain::onCassettePause)

    EVT_CHECKBOX(XRCID("TurboXml"), GuiMain::onTurbo)
    EVT_TEXT(XRCID("TurboClockXml"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadXml"), GuiMain::onAutoLoad)

    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeXml"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeXml"), GuiMain::onVolume)

    EVT_TEXT(XRCID("ShowAddressXml"), GuiMain::onLedTimer)
    EVT_TEXT(XRCID("SaveStartXml"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndXml"), GuiMain::onSaveEnd)
    EVT_TEXT(XRCID("SaveExecXml"), GuiMain::onSaveExec)

    EVT_BUTTON(XRCID("VideoNumberXml"), GuiXml::onVideoNumber)

    EVT_BUTTON(XRCID("BatchButtonXml"), GuiMain::onBatchFileDialog)
    EVT_BUTTON(XRCID("BatchConvertButtonXml"), GuiMain::onBatchConvertStart)

    EVT_CHECKBOX(XRCID("XmlClearRtc"), GuiMain::onClearRtc)
    EVT_CHECKBOX(XRCID("XmlClearRam"), GuiMain::onClearRam)

    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("TempoXml"), GuiXml::onTempo)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("TempoXml"), GuiXml::onTempo)

    END_EVENT_TABLE()

GuiXml::GuiXml(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiPico(title, pos, size, mode, dataDir, iniDir)
{
    tapeOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/tick.png", wxBITMAP_TYPE_PNG);
    tapeOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/minus.png", wxBITMAP_TYPE_PNG);
    
    dropdownUpdateOngoing_ = false;
    xmlFileComboSelection = 0;
    xmlDirComboSelection = 0;
    romRamFileComboSelection[0] = 0;
    romRamFileComboSelection[1] = 0;
}

void GuiXml::readXmlConfig()
{
    selectedComputer_ = XML; // *** to be removed
    elfConfiguration[XML].ioConfiguration.emsOutput.resize(1);

    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[XML].ledTime_ = configPointer->Read("Xmlemu/Led_Update_Frequency", defaultTimer);

    xmlDirComboSelection = (int)configPointer->Read("Xmlemu/XmlDirComboSelection", 2);
    xmlDirComboString = configPointer->Read("Xmlemu/XmlDirComboString", "Comx");
    conf[XML].xmlMainDir_ = readConfigDir("Dir/Xmlemu/XmlFile", dataDir_ + "Xml" + pathSeparator_);
    
    setXmlDirDropDown();
    conf[XML].xmlDir_ = conf[XML].xmlMainDir_ + conf[XML].xmlSubDir_ + pathSeparator_;
    setXmlDropDown();

    dropdownUpdateOngoing_ = false;

    clearXmlData(XML);
    parseXmlFile(XML,conf[XML].xmlDir_, conf[XML].xmlFile_);
    
    setRomRamButtonOrder();
    
    dropdownUpdateOngoing_ = true;
    if (mode_.gui)
    {
        XRCCTRL(*this, "RomRam1Xml", wxComboBox)->Clear();
        XRCCTRL(*this, "RomRam0Xml", wxComboBox)->Clear();
    }
    dropdownUpdateOngoing_ = false;
    setRomRamDropDown(romRamButton0_, "0");
    setRomRamDropDown(romRamButton1_, "1");

    selectedComputer_ = XML;

    conf[XML].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + computerInfo[XML].gui + pathSeparator_;

    conf[XML].terminalFiles_.Add(conf[XML].xmodemFile_);
    conf[XML].terminalPaths_.Add(conf[XML].xmodemFileDir_+conf[XML].xmodemFile_);
    conf[XML].numberOfTerminalFiles_ = 1;

    conf[XML].saveEndString_ = "";
    conf[XML].saveExecString_ = "";

    elfConfiguration[XML].vt52SetUpFeature_ = configPointer->Read("Xmlemu/VT52Setup", 0x4092l);
    elfConfiguration[XML].vt100SetUpFeature_ = configPointer->Read("Xmlemu/VT100Setup", 0xcad2l);
    elfConfiguration[XML].vtExternalSetUpFeature_ = configPointer->Read("Xmlemu/VTExternalSetup", 0xcad2l);
    
    elfConfiguration[XML].vtCharactersPerRow = (int)configPointer->Read("Xmlemu/VT100CharPerRow", 80);
    elfConfiguration[XML].vt100CharWidth = (int)configPointer->Read("Xmlemu/VT100CharWidth", 10);
    elfConfiguration[XML].vt52CharWidth = (int)configPointer->Read("Xmlemu/VT52CharWidth", 9);

    elfConfiguration[XML].bellFrequency_ = (int)configPointer->Read("Xmlemu/Bell_Frequency", 800);
    configPointer->Read("Xmlemu/SerialLog", &elfConfiguration[XML].serialLog, false);
    configPointer->Read("Xmlemu/ESCError", &elfConfiguration[XML].escError, false);
    elfConfiguration[XML].clearRam = false;
    elfConfiguration[XML].clearRtc = false;

    configPointer->Read("Xmlemu/Enable_Vt_Stretch_Dot", &conf[XML].stretchDot_, false);
    configPointer->Read("Xmlemu/Enable_Vt_External", &elfConfiguration[XML].vtExternal, false);
    conf[XML].printMode_ = (int)configPointer->Read("Xmlemu/Print_Mode", 1l);

    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 1.0);
    conf[XML].zoomVt_ = convertLocale(configPointer->Read("Xmlemu/Vt_Zoom", defaultZoom));
//    wxString defaultScale;
//    defaultScale.Printf("%i", 3);
//    conf[XML].xScale_ = convertLocale(configPointer->Read("Xmlemu/Window_Scale_Factor_X", defaultScale));

    configPointer->Read("Xmlemu/Enable_Turbo_Cassette", &conf[XML].turbo_, true);
    conf[XML].turboClock_ = configPointer->Read("Xmlemu/Turbo_Clock_Speed", "15");
    configPointer->Read("Xmlemu/Enable_Auto_Cassette", &conf[XML].autoCassetteLoad_, true);
    elfConfiguration[XML].packetSize = (int)configPointer->Read("Xmlemu/Ymodem_PacketSize", 0l);
    configPointer->Read("Xmlemu/Enable_Real_Cassette", &conf[XML].realCassetteLoad_, false);
    conf[XML].volume_ = (int)configPointer->Read("Xmlemu/Volume", 25l);

    long value; 

    conf[XML].saveStartString_ = configPointer->Read("Xmlemu/SaveStart", "0");
    if (!conf[XML].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[XML].saveStart_ = value;

    configPointer->Read("Xmlemu/UseLoadLocation", &conf[XML].useLoadLocation_, false);
    hwTapeState_ = HW_TAPE_STATE_PLAY;
    
    if (elfConfiguration[XML].useTapeHw)
        conf[XML].autoCassetteLoad_ = true;
    
//    configPointer->Read("Xmlemu/DisableNvRam", &elfConfiguration[XML].nvRamDisable, elfConfiguration[XML].nvRamDisableDefault);

    setRealCas(XML);
    setXmlGui();
    
    conf[XML].tempo_ = (int)configPointer->Read("/Xmlemu/Tempo", 100l);

    if (mode_.gui)
    {
        correctZoomVtAndValue(XML, "Xml", SET_SPIN);
            
        XRCCTRL(*this, "StretchDotXml", wxCheckBox)->SetValue(conf[XML].stretchDot_);

        XRCCTRL(*this, "PrintModeXml", wxChoice)->SetSelection(conf[XML].printMode_);

        XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->SetValue(conf[XML].turboClock_);

        XRCCTRL(*this, "VolumeXml", wxSlider)->SetValue(conf[XML].volume_);
    }

    conf[XML].loadFileNameFull_ = "";
    conf[XML].loadFileName_ = "";
}

void GuiXml::writeXmlDirConfig()
{
    wxString number, type;
    
    writeConfigDir("/Dir/Xmlemu/XmlFile", conf[XML].xmlMainDir_);
    writeConfigDir("/Dir/Xmlemu/Key_File", conf[XML].keyFileDir_);
    writeConfigDir("/Dir/Xmlemu/Video_Dump_File", conf[XML].screenDumpFileDir_);
    writeConfigDir("/Dir/Xmlemu/Print_File", conf[XML].printFileDir_);
    writeConfigDir("/Dir/Xmlemu/Xmodem_File", conf[XML].xmodemFileDir_);
    for (int fdcType = 0; fdcType<FDCTYPE_MAX; fdcType++)
    {
        type.Printf("%d", fdcType);
        for (int disk=0; disk<4; disk++)
        {
            number.Printf("%d", disk);
            writeConfigDir("/Dir/Xmlemu/FDC" + number + "_File_" + type, floppyDir_[fdcType][disk]);
        }
    }
    for (int tape=0; tape<2; tape++)
    {
        number.Printf("%d", tape);
        writeConfigDir("/Dir/Xmlemu/Wav_File" + number, conf[XML].wavFileDir_[tape]);
    }
}

void GuiXml::writeXmlConfig()
{
    wxString buffer, number, type;

    configPointer->Write("Xmlemu/XmlDirComboSelection", xmlDirComboSelection);
    for (size_t number=0; number < dirNameList_.GetCount(); number++)
    {
        if (dirNameListDefaultFile_.GetCount() > 0)
            configPointer->Write("/Xmlemu/XmlFile/"+dirNameList_[number], dirNameListDefaultFile_[number]);
        if (dirNameListGui_.GetCount() > 0)
        {
            configPointer->Write("/Xmlemu/XmlGui/"+dirNameList_[number], dirNameListGui_[number]);
            configPointer->Write("/Xmlemu/XmlDir/"+dirNameListGui_[number], dirNameList_[number]);
        }
    }

    configPointer->Write("/Xmlemu/GuiRomRam0/"+dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection], conf[XML].memConfig_[romRamButton0_].filename);
    configPointer->Write("/Xmlemu/GuiRomRam1/"+dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection], conf[XML].memConfig_[romRamButton1_].filename);

    configPointer->Write("/Xmlemu/Key_File", conf[XML].keyFile_);
    configPointer->Write("/Xmlemu/Video_Dump_File", conf[XML].screenDumpFile_);
    configPointer->Write("/Xmlemu/Print_File", conf[XML].printFile_);
    configPointer->Write("/Xmlemu/Xmodem_File", conf[XML].xmodemFile_);
    for (int tape=0; tape<2; tape++)
    {
        number.Printf("%d", tape);
        configPointer->Write("/Xmlemu/Wav_File" + number, conf[XML].wavFile_[tape]);
    }
    for (int fdcType = 0; fdcType<FDCTYPE_MAX; fdcType++)
    {
        type.Printf("%d", fdcType);
        for (int disk=0; disk<4; disk++)
        {
            number.Printf("%d", disk);
            configPointer->Write("/Xmlemu/FDC" + number + "_File_" + type, floppy_[fdcType][disk]);
        }
    }

    buffer.Printf("%04X", (unsigned int)startRam_[XML]);
    configPointer->Write("Xmlemu/Ram_Start_Address", buffer);
    buffer.Printf("%04X", (unsigned int)endRam_[XML]);
    configPointer->Write("Xmlemu/Ram_End_Address", buffer);

    long value = elfConfiguration[XML].vt52SetUpFeature_.to_ulong();
    configPointer->Write("Xmlemu/VT52Setup", value);
    value = elfConfiguration[XML].vt100SetUpFeature_.to_ulong();
    configPointer->Write("Xmlemu/VT100Setup", value);
    value = elfConfiguration[XML].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("Xmlemu/VTExternalSetup", value);
    configPointer->Write("Xmlemu/VT100CharPerRow", elfConfiguration[XML].vtCharactersPerRow);
    configPointer->Write("Xmlemu/VT100CharWidth", elfConfiguration[XML].vt100CharWidth);
    configPointer->Write("Xmlemu/VT52CharWidth", elfConfiguration[XML].vt52CharWidth);

    configPointer->Write("Xmlemu/Bell_Frequency", elfConfiguration[XML].bellFrequency_);
    configPointer->Write("Xmlemu/SerialLog", elfConfiguration[XML].serialLog);
    configPointer->Write("Xmlemu/ESCError", elfConfiguration[XML].escError);
    configPointer->Write("Xmlemu/Vt_Zoom", conf[XML].zoomVt_);
    configPointer->Write("Xmlemu/UseLoadLocation", conf[XML].useLoadLocation_);
    configPointer->Write("Xmlemu/SaveStart", conf[XML].saveStartString_);

    configPointer->Write("Xmlemu/Enable_Printer", conf[XML].printerOn_);
    configPointer->Write("Xmlemu/Print_Mode", conf[XML].printMode_);

    configPointer->Write("Xmlemu/Enable_Vt_Stretch_Dot", conf[XML].stretchDot_);
    configPointer->Write("Xmlemu/Enable_Vt_External", elfConfiguration[XML].vtExternal);

    configPointer->Write("Xmlemu/Led_Update_Frequency", conf[XML].ledTime_);
    configPointer->Write("Xmlemu/Enable_Turbo_Cassette", conf[XML].turbo_);
    configPointer->Write("Xmlemu/Turbo_Clock_Speed", conf[XML].turboClock_);
    configPointer->Write("Xmlemu/Enable_Auto_Cassette", conf[XML].autoCassetteLoad_);
    configPointer->Write("Xmlemu/Enable_Real_Cassette", conf[XML].realCassetteLoad_);
    configPointer->Write("Xmlemu/DisableNvRam", elfConfiguration[XML].nvRamDisable);
    configPointer->Write("Xmlemu/Ymodem_PacketSize", elfConfiguration[XML].packetSize);
    configPointer->Write("Xmlemu/Volume", conf[XML].volume_);
    configPointer->Write("/Xmlemu/Tempo", conf[XML].tempo_);
}

void GuiXml::readXmlWindowConfig()
{
    conf[XML].coinX_ = (int)configPointer->Read("Xmlemu/Window_Position_CoinVideo_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defCoinX_);
    conf[XML].coinY_ = (int)configPointer->Read("Xmlemu/Window_Position_CoinVideo_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defCoinY_);
    conf[XML].pixieX_ = (int)configPointer->Read("Xmlemu/Window_Position_Pixie_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defPixieX_);
    conf[XML].pixieY_ = (int)configPointer->Read("Xmlemu/Window_Position_Pixie_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defPixieY_);
    conf[XML].cdp1864X_ = (int)configPointer->Read("Xmlemu/Window_Position_CDP1864_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defCdp1864X_);
    conf[XML].cdp1864Y_ = (int)configPointer->Read("Xmlemu/Window_Position_CDP1864_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defCdp1864Y_);
    conf[XML].st4X_ = (int)configPointer->Read("Xmlemu/Window_Position_ST4_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defSt4X_);
    conf[XML].st4Y_ = (int)configPointer->Read("Xmlemu/Window_Position_ST4_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defSt4Y_);
    conf[XML].vip2KX_ = (int)configPointer->Read("Xmlemu/Window_Position_VIP2K_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defVip2KX_);
    conf[XML].vip2KY_ = (int)configPointer->Read("Xmlemu/Window_Position_VIP2K_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defVip2KY_);
    conf[XML].fredX_ = (int)configPointer->Read("Xmlemu/Window_Position_FRED_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defFredX_);
    conf[XML].fredY_ = (int)configPointer->Read("Xmlemu/Window_Position_FRED_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defFredY_);
    conf[XML].tmsX_ = (int)configPointer->Read("Xmlemu/Window_Position_Tms_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defTmsX_);
    conf[XML].tmsY_ = (int)configPointer->Read("Xmlemu/Window_Position_Tms_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defTmsY_);
    conf[XML].mc6845X_ = (int)configPointer->Read("Xmlemu/Window_Position_MC6845_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defMc6845X_);
    conf[XML].mc6845Y_ = (int)configPointer->Read("Xmlemu/Window_Position_MC6845_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defMc6845Y_);
    conf[XML].mc6847X_ = (int)configPointer->Read("Xmlemu/Window_Position_MC6847_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defMc6847X_);
    conf[XML].mc6847Y_ = (int)configPointer->Read("Xmlemu/Window_Position_MC6847_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defMc6847Y_);
    conf[XML].i8275X_ = (int)configPointer->Read("Xmlemu/Window_Position_I8275_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defi8275X_);
    conf[XML].i8275Y_ = (int)configPointer->Read("Xmlemu/Window_Position_I8275_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defi8275Y_);
    conf[XML].v1870X_ = (int)configPointer->Read("Xmlemu/Window_Position_v1870_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defv1870X_);
    conf[XML].v1870Y_ = (int)configPointer->Read("Xmlemu/Window_Position_v1870_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defv1870Y_);
    conf[XML].SN76430NX_ = (int)configPointer->Read("Xmlemu/Window_Position_SN76430N_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defSN76430NX_);
    conf[XML].SN76430NY_ = (int)configPointer->Read("Xmlemu/Window_Position_SN76430N_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defSN76430NY_);
    int num=0;
    wxString numStr;
    for (std::vector<Cdp1851>::iterator cdp1851 = elfConfiguration[XML].ioConfiguration.cdp1851.begin (); cdp1851 != elfConfiguration[XML].ioConfiguration.cdp1851.end (); ++cdp1851)
    {
        numStr.Printf("%d",num);
        cdp1851->pos.x = (int)configPointer->Read("Xmlemu/Window_Position_Cdp1851_"+numStr+"_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, cdp1851->defaultPos.x);
        cdp1851->pos.y = (int)configPointer->Read("Xmlemu/Window_Position_Cdp1851_"+numStr+"_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, cdp1851->defaultPos.y);
    }
    for (std::vector<Cdp1852>::iterator cdp1852 = elfConfiguration[XML].ioConfiguration.cdp1852.begin (); cdp1852 != elfConfiguration[XML].ioConfiguration.cdp1852.end (); ++cdp1852)
    {
        numStr.Printf("%d",num);
        cdp1852->pos.x = (int)configPointer->Read("Xmlemu/Window_Position_Cdp1852_"+numStr+"_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, cdp1852->defaultPos.x);
        cdp1852->pos.y = (int)configPointer->Read("Xmlemu/Window_Position_Cdp1852_"+numStr+"_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, cdp1852->defaultPos.y);
    }
    conf[XML].vtX_ = (int)configPointer->Read("Xmlemu/Window_Position_Vt_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defVtX_);
    conf[XML].vtY_ = (int)configPointer->Read("Xmlemu/Window_Position_Vt_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].defVtY_);
    conf[XML].mainX_ = (int)configPointer->Read("Xmlemu/Window_Position_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, mainWindowX_);
    conf[XML].mainY_ = (int)configPointer->Read("Xmlemu/Window_Position_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    ledModuleX_ = (int)configPointer->Read("Xmlemu/Window_Position_Led_Module_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, mainWindowX_+346+windowInfo.xBorder2);
    ledModuleY_ = (int)configPointer->Read("Xmlemu/Window_Position_Led_Module_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, mainWindowY_+windowInfo.mainwY+229+windowInfo.yBorder2);
    conf[XML].keypadX_ = (int)configPointer->Read("Xmlemu/Window_Position_Keypad_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, mainWindowX_+346+windowInfo.xBorder2);
    conf[XML].keypadY_ = (int)configPointer->Read("Xmlemu/Window_Position_Keypad_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiXml::writeXmlWindowConfig()
{
    if (conf[XML].mainX_ > 0)
        configPointer->Write("Xmlemu/MainX/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].mainX_);
    if (conf[XML].mainY_ > 0)
        configPointer->Write("Xmlemu/MainY/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].mainY_);
    if (elfConfiguration[XML].useCoinVideo)
    {
        if (conf[XML].coinX_ > 0)
            configPointer->Write("Xmlemu/Window_Position_Pixie_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].coinX_);
        if (conf[XML].coinY_ > 0)
            configPointer->Write("Xmlemu/Window_Position_Pixie_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].coinY_);
    }
    if (elfConfiguration[XML].usePixie)
    {
        if (conf[XML].pixieX_ > 0)
            configPointer->Write("Xmlemu/Window_Position_Pixie_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].pixieX_);
        if (conf[XML].pixieY_ > 0)
            configPointer->Write("Xmlemu/Window_Position_Pixie_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].pixieY_);
    }
    if (elfConfiguration[XML].use1864)
    {
        if (conf[XML].cdp1864X_ > 0)
            configPointer->Write("Xmlemu/Window_Position_CDP1864_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].cdp1864X_);
        if (conf[XML].cdp1864Y_ > 0)
            configPointer->Write("Xmlemu/Window_Position_CDP1864_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].cdp1864Y_);
    }
    if (elfConfiguration[XML].useSt4Video)
    {
        if (conf[XML].st4X_ > 0)
            configPointer->Write("Xmlemu/Window_Position_ST4_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].st4X_);
        if (conf[XML].st4Y_ > 0)
            configPointer->Write("Xmlemu/Window_Position_ST4_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].st4Y_);
    }
    if (elfConfiguration[XML].useVip2KVideo)
    {
        if (conf[XML].vip2KX_ > 0)
            configPointer->Write("Xmlemu/Window_Position_VIP2K_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].vip2KX_);
        if (conf[XML].vip2KY_ > 0)
            configPointer->Write("Xmlemu/Window_Position_VIP2K_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].vip2KY_);
    }
    if (elfConfiguration[XML].useFredVideo)
    {
        if (conf[XML].fredX_ > 0)
            configPointer->Write("Xmlemu/Window_Position_FRED_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].fredX_);
        if (conf[XML].fredY_ > 0)
            configPointer->Write("Xmlemu/Window_Position_FRED_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].fredY_);
    }
    if (elfConfiguration[XML].useTMS9918)
    {
        if (conf[XML].tmsX_ > 0)
            configPointer->Write("Xmlemu/Window_Position_Tms_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].tmsX_);
        if (conf[XML].tmsY_ > 0)
            configPointer->Write("Xmlemu/Window_Position_Tms_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].tmsY_);
    }
    if (elfConfiguration[XML].use6845)
    {
        if (conf[XML].mc6845X_ > 0)
            configPointer->Write("Xmlemu/Window_Position_MC6845_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].mc6845X_);
        if (conf[XML].mc6845Y_ > 0)
            configPointer->Write("Xmlemu/Window_Position_MC6845_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].mc6845Y_);
    }
    if (elfConfiguration[XML].use6847)
    {
        if (conf[XML].mc6847X_ > 0)
            configPointer->Write("Xmlemu/Window_Position_MC6847_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].mc6847X_);
        if (conf[XML].mc6847Y_ > 0)
            configPointer->Write("Xmlemu/Window_Position_MC6847_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].mc6847Y_);
    }
    if (elfConfiguration[XML].use8275)
    {
        if (conf[XML].i8275X_ > 0)
            configPointer->Write("Xmlemu/Window_Position_I8275_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].i8275X_);
        if (conf[XML].i8275Y_ > 0)
            configPointer->Write("Xmlemu/Window_Position_I8275_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].i8275Y_);
    }
    if (elfConfiguration[XML].usev1870)
    {
        if (conf[XML].v1870X_ > 0)
            configPointer->Write("Xmlemu/Window_Position_v1870_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].v1870X_);
        if (conf[XML].v1870Y_ > 0)
            configPointer->Write("Xmlemu/Window_Position_v1870_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].v1870Y_);
    }
    if (elfConfiguration[XML].useSN76430N)
    {
        if (conf[XML].SN76430NX_ > 0)
            configPointer->Write("Xmlemu/Window_Position_SN76430N_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].SN76430NX_);
        if (conf[XML].SN76430NY_ > 0)
            configPointer->Write("Xmlemu/Window_Position_SN76430N_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].SN76430NY_);
    }
    int num=0;
    wxString numStr;
    for (std::vector<Cdp1851>::iterator cdp1851 = elfConfiguration[XML].ioConfiguration.cdp1851.begin (); cdp1851 != elfConfiguration[XML].ioConfiguration.cdp1851.end (); ++cdp1851)
    {
        numStr.Printf("%d",num);
        if (cdp1851->pos.x > 0)
            configPointer->Write("Xmlemu/Window_Position_Cdp1851_"+numStr+"_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, cdp1851->pos.x);
        if (cdp1851->pos.y > 0)
            configPointer->Write("Xmlemu/Window_Position_Cdp1851_"+numStr+"_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, cdp1851->pos.y);
    }
    for (std::vector<Cdp1852>::iterator cdp1852 = elfConfiguration[XML].ioConfiguration.cdp1852.begin (); cdp1852 != elfConfiguration[XML].ioConfiguration.cdp1852.end (); ++cdp1852)
    {
        numStr.Printf("%d",num);
        if (cdp1852->pos.x > 0)
            configPointer->Write("Xmlemu/Window_Position_Cdp1852_"+numStr+"_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, cdp1852->pos.x);
        if (cdp1852->pos.y > 0)
            configPointer->Write("Xmlemu/Window_Position_Cdp1852_"+numStr+"_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, cdp1852->pos.y);
    }
    if (elfConfiguration[XML].vtType == VT52 || elfConfiguration[XML].vtType == VT100)
    {
        if (conf[XML].vtX_ > 0)
            configPointer->Write("Xmlemu/Window_Position_Vt_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].vtX_);
        if (conf[XML].vtY_ > 0)
            configPointer->Write("Xmlemu/Window_Position_Vt_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].vtY_);
    }
    if (((elfConfiguration[XML].panelType_ == PANEL_COSMAC || elfConfiguration[XML].panelType_ == PANEL_COSMICOS) && elfConfiguration[XML].useHexKeyboard) || (elfConfiguration[XML].panelType_ == PANEL_ELF2K &&  elfConfiguration[XML].useHex))
    {
        if (conf[XML].keypadX_ > 0)
            configPointer->Write("Xmlemu//Window_Position_Keypad_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].keypadX_);
        if (conf[XML].keypadY_ > 0)
            configPointer->Write("Xmlemu//Window_Position_Keypad_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, conf[XML].keypadY_);
    }
    if (elfConfiguration[XML].useLedModule)
    {
        if (ledModuleX_ > 0)
            configPointer->Write("Xmlemu//Window_Position_Led_Module_X/"+conf[XML].xmlDir_+conf[XML].xmlFile_, ledModuleX_);
        if (ledModuleY_ > 0)
            configPointer->Write("Xmlemu//Window_Position_Led_Module_Y/"+conf[XML].xmlDir_+conf[XML].xmlFile_, ledModuleY_);
    }
}

void GuiXml::onXmlControlWindows(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].useElfControlWindows = event.IsChecked();

    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddress"+computerInfo[selectedComputer_].gui,wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText1"+computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText2"+computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
    }
    
    if (computerRunning_)
    {
        p_Xmlemu->Show(elfConfiguration[runningComputer_].useElfControlWindows);
        if (elfConfiguration[runningComputer_].panelType_ == PANEL_COSMAC)
            p_Xmlemu->showModules(elfConfiguration[runningComputer_].useElfControlWindows);
        if (elfConfiguration[runningComputer_].panelType_ == PANEL_ELF2K)
            p_Xmlemu->showModules(elfConfiguration[runningComputer_].useElfControlWindows, elfConfiguration[runningComputer_].useSwitch, elfConfiguration[runningComputer_].useHex);
        if (elfConfiguration[runningComputer_].panelType_ == PANEL_COSMICOS)
            p_Xmlemu->showModules(elfConfiguration[runningComputer_].useElfControlWindows);
    }
}

void GuiXml::onRomRam0Xml(wxCommandEvent& WXUNUSED(event) )
{
    romRamXml(romRamButton0_, "0");
}

void GuiXml::onRomRam1Xml(wxCommandEvent& WXUNUSED(event) )
{
    romRamXml(romRamButton1_, "1");
}

void GuiXml::romRamXml(int romRamButton, wxString romRamButtonString)
{
    wxString fileName, romRam = "ROM";

    if (conf[selectedComputer_].memConfig_[romRamButton].type == MAINRAM)
        romRam = "RAM";
    
    if (conf[selectedComputer_].memConfig_[romRamButton].type == NVRAM)
        romRam = "NVR";

    fileName = wxFileSelector( "Select the " + romRam + " file to load",
                              conf[selectedComputer_].memConfig_[romRamButton].dirname, XRCCTRL(*this, "RomRam"+romRamButtonString+"Xml", wxComboBox)->GetValue(),
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
    conf[selectedComputer_].memConfig_[romRamButton].dirname = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].memConfig_[romRamButton].filename = FullPath.GetFullName();

    XRCCTRL(*this, "RomRam"+romRamButtonString+"Xml", wxComboBox)->SetValue(conf[selectedComputer_].memConfig_[romRamButton].filename);

    configPointer->Write("/Xmlemu/GuiRomRam"+romRamButtonString+"/"+dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection], conf[XML].memConfig_[romRamButton].filename);
    configPointer->Flush();
}

void GuiXml::onRomRom0TextXml(wxCommandEvent& WXUNUSED(event))
{
    romRomTextXml(romRamButton0_, "0");
}

void GuiXml::onRomRom1TextXml(wxCommandEvent& WXUNUSED(event))
{
    romRomTextXml(romRamButton1_, "1");
}

void GuiXml::romRomTextXml(int romRamButton, wxString romRamButtonString)
{
    if (dropdownUpdateOngoing_)
        return;

    conf[selectedComputer_].memConfig_[romRamButton].filename = XRCCTRL(*this, "RomRam"+romRamButtonString+"Xml", wxComboBox)->GetValue();

    configPointer->Write("/Xmlemu/GuiRomRam"+romRamButtonString+"/"+dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection], conf[XML].memConfig_[romRamButton].filename);
    configPointer->Flush();
}

void GuiXml::onRomRom0ComboXml(wxCommandEvent& event)
{
    if (dropdownUpdateOngoing_)
        return;

    romRamFileComboSelection[romRamButton0_] = event.GetSelection();

    romRomComboXml(romRamButton0_, "0");
}

void GuiXml::onRomRom1ComboXml(wxCommandEvent& event)
{
    if (dropdownUpdateOngoing_)
        return;

    romRamFileComboSelection[romRamButton1_] = event.GetSelection();

    romRomComboXml(romRamButton1_, "1");
}

void GuiXml::romRomComboXml(int romRamButton, wxString romRamButtonString)
{
    conf[selectedComputer_].memConfig_[romRamButton].dirname = romRamDirNameListGui_[romRamButton][romRamFileComboSelection[romRamButton]];
    conf[selectedComputer_].memConfig_[romRamButton].filename = XRCCTRL(*this, "RomRam"+romRamButtonString+"Xml", wxComboBox)->GetValue();

    configPointer->Write("/Xmlemu/GuiRomRam"+romRamButtonString+"/"+dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection], conf[XML].memConfig_[romRamButton].filename);
    configPointer->Flush();
}

void GuiXml::onMainXmlXml(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;

    wxString dirName = wxDirSelector( "Select main XML folder", conf[selectedComputer_].xmlMainDir_);
    if (!dirName)
        return;
    
    conf[selectedComputer_].xmlMainDir_ = dirName + pathSeparator_;
    
    setXmlDirDropDown();
    conf[XML].xmlDir_ = conf[XML].xmlMainDir_ + conf[XML].xmlSubDir_ + pathSeparator_;
    setXmlDropDown();

    dropdownUpdateOngoing_ = false;
}

void GuiXml::onMainXmlTextXml(wxCommandEvent& WXUNUSED(event))
{
    if (dropdownUpdateOngoing_)
        return;

    conf[selectedComputer_].xmlFile_ = XRCCTRL(*this, "MainXmlXml", wxComboBox)->GetValue();
    
    setXmlDirFileGui();
    setRomRamDropDown(romRamButton0_, "0");
    setRomRamDropDown(romRamButton1_, "1");
}

void GuiXml::onMainXmlComboXml(wxCommandEvent& event)
{
    xmlFileComboSelection = event.GetSelection();

    conf[selectedComputer_].xmlFile_ = event.GetString();
    if (dirNameListDefaultFile_.GetCount() > 0)
        dirNameListDefaultFile_[xmlDirComboSelection] = conf[selectedComputer_].xmlFile_;

    setXmlDirFileGui();
    setRomRamDropDown(romRamButton0_, "0");
    setRomRamDropDown(romRamButton1_, "1");
}

void GuiXml::onMainDirComboXml(wxCommandEvent& event)
{
    xmlDirComboSelection = event.GetSelection();
    
    if (dirNameList_.GetCount() > 0)
        conf[selectedComputer_].xmlSubDir_ = dirNameList_[xmlDirComboSelection];
    conf[selectedComputer_].xmlDir_ = conf[selectedComputer_].xmlMainDir_ + conf[selectedComputer_].xmlSubDir_ + pathSeparator_;
 
    setXmlDropDown();
    setXmlDirFileGui();
    setRomRamDropDown(romRamButton0_, "0");
    setRomRamDropDown(romRamButton1_, "1");
}

void GuiXml::setXmlDirFileGui()
{
    dropdownUpdateOngoing_ = true;
    if (mode_.gui)
    {
        XRCCTRL(*this, "RomRam1Xml", wxComboBox)->Clear();
        XRCCTRL(*this, "RomRam0Xml", wxComboBox)->Clear();
    }
    dropdownUpdateOngoing_ = false;
    
    parseXmlFile(selectedComputer_, conf[selectedComputer_].xmlDir_, conf[selectedComputer_].xmlFile_);
    if (conf[selectedComputer_].xmlFile_ != "")
        if (dirNameListGui_.GetCount() > 0)
            dirNameListGui_[xmlDirComboSelection] = computerInfo[selectedComputer_].name;
    setXmlGui();

    checkWavFileDownload(selectedComputer_, true);
}

void GuiXml::setXmlDropDown()
{
    dropdownUpdateOngoing_ = true;

    if (mode_.gui)
        XRCCTRL(*this, "MainXmlXml", wxComboBox)->Clear();
    conf[selectedComputer_].xmlFile_ = "";
    xmlFileComboSelection = 0;
    
    wxArrayString fileNameList;
    wxString fileName;
    fileNameList.Clear();
    
    wxDir *dir;
    dir = new wxDir (conf[selectedComputer_].xmlDir_);
    
    bool dirFound = dir->GetFirst(&fileName,  wxEmptyString, wxDIR_FILES);
    while (dirFound)
    {
        fileNameList.Add(fileName);
        dirFound = dir->GetNext(&fileName);
    }
    fileNameList.Sort();
    for (size_t number=0; number < fileNameList.GetCount(); number++)
    {
        if (mode_.gui)
            XRCCTRL(*this, "MainXmlXml", wxComboBox)->Append(fileNameList[number]);
        
        if (dirNameListDefaultFile_.GetCount() > 0)
        {
            if (dirNameListDefaultFile_[xmlDirComboSelection] == fileNameList[number])
            {
                conf[selectedComputer_].xmlFile_ = dirNameListDefaultFile_[xmlDirComboSelection];
                xmlFileComboSelection =  number;
            }
        }
    }
    
    if (mode_.gui)
        XRCCTRL(*this, "MainXmlXml", wxComboBox)->SetValue(conf[selectedComputer_].xmlFile_);

    dropdownUpdateOngoing_ = false;
    delete dir;
}

void GuiXml::setRomRamDropDown(int romRamButton, wxString romRamButtonString)
{
    if (conf[XML].memConfig_[romRamButton].pulldownDir.Count() == 0)
        return;
    
    dropdownUpdateOngoing_ = true;

    wxArrayString fileNameList;
    wxArrayString lowerCaseFileNameList;
    wxArrayString dirNameList;
    wxString fileName;
    romRamFileNameListGui_[romRamButton].Clear();
    romRamDirNameListGui_[romRamButton].Clear();

    wxDir *dir;
    dir = new wxDir;
    bool dirFound;
    
    fileNameList.Add("");
    romRamFileNameListGui_[romRamButton].Add("");
    lowerCaseFileNameList.Add("");
    dirNameList.Add(conf[XML].memConfig_[romRamButton].pulldownDir[0]);
    romRamDirNameListGui_[romRamButton].Add(conf[XML].memConfig_[romRamButton].pulldownMask[0]);

    for (size_t number=0; number < conf[XML].memConfig_[romRamButton].pulldownDir.GetCount(); number++)
    {
        if (!dir->Open(conf[XML].memConfig_[romRamButton].pulldownDir[number]))
        {
            dropdownUpdateOngoing_ = false;
            delete dir;
            return;
        }
        dirFound = dir->GetFirst(&fileName, conf[XML].memConfig_[romRamButton].pulldownMask[number], wxDIR_FILES);
        while (dirFound)
        {
            if ((conf[XML].memConfig_[romRamButton].pulldownExclude[number] == "" || fileName.Find(conf[XML].memConfig_[romRamButton].pulldownExclude[number]) == wxNOT_FOUND) && (conf[XML].memConfig_[romRamButton].pulldownExclude2[number] == "" || fileName.Find(conf[XML].memConfig_[romRamButton].pulldownExclude2[number]) == wxNOT_FOUND))
            {
                fileNameList.Add(fileName);
                romRamFileNameListGui_[romRamButton].Add(fileName);
                fileName.LowerCase();
                lowerCaseFileNameList.Add(fileName);
                dirNameList.Add(conf[XML].memConfig_[romRamButton].pulldownDir[number]);
                romRamDirNameListGui_[romRamButton].Add(conf[XML].memConfig_[romRamButton].pulldownMask[number]);
            }
            dirFound = dir->GetNext(&fileName);
        }
    }
    lowerCaseFileNameList.Sort();
    for (size_t number=0; number < romRamFileNameListGui_[romRamButton].GetCount(); number++)
    {
        for (size_t gameNumber=0; gameNumber < fileNameList.GetCount(); gameNumber++)
        {
            fileName = fileNameList[gameNumber];
            fileName.LowerCase();

            if (lowerCaseFileNameList[number] == fileName)
            {
                romRamDirNameListGui_[romRamButton][number] = dirNameList[gameNumber];
                romRamFileNameListGui_[romRamButton][number] = fileNameList[gameNumber];
            }
        }

        if (mode_.gui)
            XRCCTRL(*this, "RomRam"+romRamButtonString+"Xml", wxComboBox)->Append(romRamFileNameListGui_[romRamButton][number]);
    }

    dropdownUpdateOngoing_ = false;
    delete dir;
}

void GuiXml::setXmlDirDropDown()
{
    wxString defaultList[]=
    {
        "CDP18S020",
        "CDP18S020 Evaluation Kit",
        "tiny basic ram.xml",
        "Cidelsa",
        "Cidelsa Arcade Game Console",
        "altair.xml",
        "CoinArcade",
        "RCA Video Coin Arcade Game Console",
        "bare.xml",
        "Comix",
        "COMIX-35",
        "pal_high_speed_direct_connect.xml",
        "Comx",
        "COMX-35",
        "superboard.xml",
        "Conic",
        "Conic (Apollo, Mustang, MPT-02 and M1200)",
        "soundic_victory_mpt-02.xml",
        "CosmacElf",
        "COSMAC Elf",
        "bare.xml",
        "CosmacGameSystem",
        "RCA COSMAC Computer Game System",
        "bare.xml",
        "Cosmicos",
        "Cosmicos - COSmac MIcro COmputer System",
        "ut4-monitor.xml",
        "Cybervision",
        "Cybervision 2001",
        "escape.xml",
        "Elf",
        "Elf",
        "elfos-serial.xml",
        "Elf2K",
        "COSMAC Elf 2000",
        "i8275.xml",
        "Eti",
        "ETI-660",
        "wipeout.xml",
        "FRED1",
        "FRED 1",
        "animate demo.xml",
        "FRED1_5",
        "FRED 1.5",
        "bare.xml",
        "HUG1802",
        "HUG1802",
        "basic.xml",
        "JVIP",
        "JVIP",
        "bare.xml",
        "Macbug",
        "Macbug",
        "bare.xml",
        "MCDS",
        "RCA Microboard Computer Development System",
        "cdp18s695.xml",
        "Membership",
        "Membership Card",
        "monitor-j-basic3-rom-8000.xml",
        "Microboard",
        "RCA COSMAC Microboard Computer",
        "cdp18s600.xml",
        "Microtutor",
        "COSMAC Microtutor I",
        "bare.xml",
        "MicrotutorII",
        "COSMAC Microtutor II",
        "tops.xml",
        "MS2000",
        "RCA MicroDisk Development System MS2000",
        "hd.xml",
        "Nano",
        "Oscom Nano",
        "chip8.xml",
        "NetronicsElfII",
        "Netronics Elf II",
        "tinybasic-serial.xml",
        "Pecom32",
        "PECOM 32",
        "bare.xml",
        "Pecom64",
        "PECOM 64",
        "rom_v4.xml",
        "PicoElfV2",
        "Pico/Elf V2",
        "elfos-uart.xml",
        "QuestSuperElf",
        "Quest Super Elf",
        "bare.xml",
        "Studio2020",
        "Studio 2020",
        "pal.xml",
        "StudioII",
        "RCA Studio II",
        "multicart.xml",
        "StudioIII",
        "RCA Studio III",
        "chip8-ntsc.xml",
        "StudioIV",
        "RCA Studio IV",
        "ntsc-basic-32k-2020.xml",
        "TMC600",
        "Telmac TMC-600",
        "exp-151182.xml",
        "TMC1800",
        "Telmac 1800",
        "chip8.xml",
        "TMC2000",
        "Telmac 2000",
        "chip8.xml",
        "UC1800",
        "Infinite UC1800",
        "keybug2020.xml",
        "Velf",
        "VELF",
        "bare.xml",
        "Vip",
        "COSMAC VIP",
        "fpb-vp590.xml",
        "Vip2K",
        "VIP2K Membership Card",
        "monitor-1.5,ntsc.xml",
        "VipII",
        "COSMAC VIP II",
        "ed.xml",
        "Visicom",
        "Visicom COM-100",
        "standard.xml",
        "",
        "",
        "",
    };
    
    dropdownUpdateOngoing_ = true;
    if (mode_.gui)
        XRCCTRL(*this, "MainDirXml", wxChoice)->Clear();

    wxString dirName;
    dirNameList_.Clear();
    dirNameListDefaultFile_.Clear();
    dirNameListGui_.Clear();

    wxDir *dir;
    dir = new wxDir (conf[XML].xmlMainDir_);
    
    bool dirFound = dir->GetFirst(&dirName,  wxEmptyString, wxDIR_DIRS);
    while (dirFound)
    {
        size_t number=0;
        while (defaultList[number] != "" && defaultList[number] != dirName)
            number += 3;

        if (defaultList[number] != "")
            dirNameListGui_.Add(configPointer->Read("Xmlemu/XmlGui/"+dirName, defaultList[number+1]));
        else
            dirNameListGui_.Add(configPointer->Read("Xmlemu/XmlGui/"+dirName, dirName));

        dirNameList_.Add("");
        dirNameListDefaultFile_.Add("");
        dirFound = dir->GetNext(&dirName);
    }
                                
    dirNameListGui_.Sort();

    for (size_t number=0; number < dirNameListGui_.GetCount(); number++)
    {
        int defaultNumber=1;
        while (defaultList[defaultNumber] != "" && defaultList[defaultNumber] != dirNameListGui_[number])
            defaultNumber += 3;
        
        if (defaultList[defaultNumber] != "")
            dirNameList_[number] = configPointer->Read("/Xmlemu/XmlDir/"+dirNameListGui_[number], defaultList[defaultNumber-1]);
        else
            dirNameList_[number] = configPointer->Read("/Xmlemu/XmlDir/"+dirNameListGui_[number], dirNameListGui_[number]);

        dirNameListDefaultFile_[number] = configPointer->Read("/Xmlemu/XmlFile/"+dirNameList_[number], defaultList[defaultNumber+1]);

        if (mode_.gui)
            XRCCTRL(*this, "MainDirXml", wxChoice)->Append(dirNameListGui_[number]);
    }
    
    if (dirNameList_.GetCount() > 0)
        conf[XML].xmlSubDir_ = dirNameList_[xmlDirComboSelection];
    else
        conf[XML].xmlSubDir_ = "";
    
    if (mode_.gui)
    {
        if (dirNameListGui_.GetCount() > 0)
            XRCCTRL(*this, "MainDirXml", wxChoice)->SetSelection((int)xmlDirComboSelection);
   //     else
   //        XRCCTRL(*this, "MainDirXml", wxComboBox)->SetString("");
    }
    
    dropdownUpdateOngoing_ = false;
    delete dir;
}

void GuiXml::setXmlSubDir(wxString directory)
{
    if (dirNameListGui_.GetCount() <= 0)
    {
        conf[XML].xmlSubDir_ = "";
        return;
    }

    xmlDirComboSelection = 0;
    while (dirNameList[xmlDirComboSelection] != directory || xmlDirComboSelection == (dirNameList_.GetCount() - 1))     
        xmlDirComboSelection++;

    conf[XML].xmlSubDir_ = dirNameList_[xmlDirComboSelection];

    if (mode_.gui)
        XRCCTRL(*this, "MainDirXml", wxChoice)->SetSelection((int)xmlDirComboSelection);
}

void GuiXml::setPrintModeXml()
{
    if (conf[selectedComputer_].printerOn_)
    {
        XRCCTRL(*this, "PrintModeXml", wxChoice)->Enable(true);
        if (computerRunning_)
            XRCCTRL(*this, "PrintButtonXml", wxBitmapButton)->Enable(true);
        else
            XRCCTRL(*this, "PrintButtonXml", wxBitmapButton)->Enable(false);
    }
    else
    {
        XRCCTRL(*this, "PrintModeXml", wxChoice)->Enable(false);
        XRCCTRL(*this, "PrintButtonXml", wxBitmapButton)->Enable(false);
    }
    if (conf[selectedComputer_].printMode_ != PRINTFILE || !conf[selectedComputer_].printerOn_)
    {
        XRCCTRL(*this, "PrintFileButton"+computerInfo[selectedComputer_].gui, wxButton)->Enable(false);
        XRCCTRL(*this, "PrintFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->Enable(false);
    }
    else
    {
        XRCCTRL(*this, "PrintFileButton"+computerInfo[selectedComputer_].gui, wxButton)->Enable(true);
        XRCCTRL(*this, "PrintFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->Enable(true);
    }
}

void GuiXml::setRomRamButtonOrder()
{
    romRamButton0_ = 0;
    romRamButton1_ = 1;

    bool romRamButtonEnable0 = (conf[XML].memConfig_[romRamButton0_].type & 0xff) == MAINRAM || (conf[XML].memConfig_[romRamButton0_].type & 0xff) == MAINROM || (conf[XML].memConfig_[romRamButton0_].type & 0xff) == NVRAM;
    bool romRamButtonEnable1 = (conf[XML].memConfig_[romRamButton1_].type & 0xff) == MAINRAM || (conf[XML].memConfig_[romRamButton1_].type & 0xff) == MAINROM || (conf[XML].memConfig_[romRamButton1_].type & 0xff) == NVRAM;

    if (romRamButtonEnable0 && romRamButtonEnable1)
    {
        if (conf[XML].memConfig_[romRamButton0_].start > conf[XML].memConfig_[romRamButton1_].start)
        {
            romRamButton0_ = 1;
            romRamButton1_ = 0;
        }
    }
    else
    {
        if (romRamButtonEnable1)
        {
            romRamButton0_ = 1;
            romRamButton1_ = 0;
        }
    }
}

void GuiXml::setXmlGui()
{
    readXmlWindowConfig();
    
    setVtType("Xml", XML, elfConfiguration[XML].vtType, false);
    
    if (!mode_.gui)
        return;
  
    XRCCTRL(*this, "ShowAddressXml", wxTextCtrl)->ChangeValue(conf[XML].ledTime_);

    setRomRamButtonOrder();
    bool ramButtonEnable = (conf[XML].memConfig_[romRamButton1_].type & 0xff) == MAINRAM || (conf[XML].memConfig_[romRamButton1_].type & 0xff) == MAINROM || (conf[XML].memConfig_[romRamButton1_].type & 0xff) == NVRAM;
    wxString buttonText = "....";
    if (ramButtonEnable)
    {
        if ((conf[XML].memConfig_[romRamButton1_].type & 0xff) == MAINRAM)
            buttonText.Printf("RAM @%04X", conf[XML].memConfig_[romRamButton1_].start);
        if ((conf[XML].memConfig_[romRamButton1_].type & 0xff) == MAINROM)
            buttonText.Printf("ROM @%04X", conf[XML].memConfig_[romRamButton1_].start);
        if ((conf[XML].memConfig_[romRamButton1_].type & 0xff) == NVRAM)
            buttonText.Printf("NVR @%04X", conf[XML].memConfig_[romRamButton1_].start);
    }
    XRCCTRL(*this,"RomRamButton1Xml", wxButton)->SetLabel(buttonText);
    XRCCTRL(*this,"RomRam1Xml", wxComboBox)->Enable(ramButtonEnable);
    XRCCTRL(*this,"RomRamButton1Xml", wxButton)->Enable(ramButtonEnable);
        
    bool romButtonEnable = (conf[XML].memConfig_[romRamButton0_].type & 0xff) == MAINRAM || (conf[XML].memConfig_[romRamButton0_].type & 0xff) == MAINROM || (conf[XML].memConfig_[romRamButton0_].type & 0xff) == NVRAM;
    buttonText = "....";
    if (romButtonEnable)
    {
        if ((conf[XML].memConfig_[romRamButton0_].type & 0xff) == MAINRAM)
            buttonText.Printf("RAM @%04X", conf[XML].memConfig_[romRamButton0_].start);
        if ((conf[XML].memConfig_[romRamButton0_].type & 0xff) == MAINROM)
            buttonText.Printf("ROM @%04X", conf[XML].memConfig_[romRamButton0_].start);
        if ((conf[XML].memConfig_[romRamButton0_].type & 0xff) == NVRAM)
            buttonText.Printf("NVR @%04X", conf[XML].memConfig_[romRamButton0_].start);
    }
    XRCCTRL(*this,"RomRamButton0Xml", wxButton)->SetLabel(buttonText);
    XRCCTRL(*this,"RomRam0Xml", wxComboBox)->Enable(romButtonEnable);
    XRCCTRL(*this,"RomRamButton0Xml", wxButton)->Enable(romButtonEnable);

    if (isXmlRomRamOptionGui())
    {
        conf[XML].memConfig_[romRamButton1_].filename = configPointer->Read("/Xmlemu/GuiRomRam0/"+dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection], conf[XML].memConfig_[romRamButton1_].filename);
        conf[XML].memConfig_[romRamButton0_].filename = configPointer->Read("/Xmlemu/GuiRomRam1/"+dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection], conf[XML].memConfig_[romRamButton0_].filename);
    }
    XRCCTRL(*this, "RomRam1Xml", wxComboBox)->SetValue(conf[XML].memConfig_[romRamButton1_].filename);
    XRCCTRL(*this, "RomRam0Xml", wxComboBox)->SetValue(conf[XML].memConfig_[romRamButton0_].filename);
    
    XRCCTRL(*this, "WavFileXml", wxTextCtrl)->SetValue(conf[XML].wavFile_[0]);
    XRCCTRL(*this, "WavFile1Xml", wxTextCtrl)->SetValue(conf[XML].wavFile_[1]);
    XRCCTRL(*this, "XmodemFileXml", wxTextCtrl)->SetValue(conf[XML].xmodemFile_);

    XRCCTRL(*this, "CasButtonXml", wxButton)->Enable(elfConfiguration[XML].useTape || elfConfiguration[XML].useTapeHw);
    XRCCTRL(*this, "WavFileXml", wxTextCtrl)->Enable(elfConfiguration[XML].useTape || elfConfiguration[XML].useTapeHw);
    XRCCTRL(*this, "EjectCasXml", wxBitmapButton)->Enable(elfConfiguration[XML].useTape || elfConfiguration[XML].useTapeHw);

    XRCCTRL(*this, "CasButton1Xml", wxButton)->Enable(elfConfiguration[XML].useTape1);
    XRCCTRL(*this, "WavFile1Xml", wxTextCtrl)->Enable(elfConfiguration[XML].useTape1);
    XRCCTRL(*this, "EjectCas1Xml", wxBitmapButton)->Enable(elfConfiguration[XML].useTape1);

    XRCCTRL(*this, "XmodemButtonXml", wxButton)->Enable(elfConfiguration[XML].useXmodem || elfConfiguration[XML].useHexModem);
    XRCCTRL(*this, "XmodemFileXml", wxTextCtrl)->Enable(elfConfiguration[XML].useXmodem || elfConfiguration[XML].useHexModem);
    XRCCTRL(*this, "EjectXmodemXml", wxBitmapButton)->Enable(elfConfiguration[XML].useXmodem || elfConfiguration[XML].useHexModem);

    XRCCTRL(*this, "PrintFileXml", wxTextCtrl)->ChangeValue(conf[XML].printFile_);
    XRCCTRL(*this, "KeyFileXml", wxTextCtrl)->ChangeValue(conf[XML].keyFile_);
    XRCCTRL(*this, "ScreenDumpFileXml", wxComboBox)->ChangeValue(conf[XML].screenDumpFile_);

    if (elfConfiguration[XML].useHexModem)
        XRCCTRL(*this, "XmodemButtonXml", wxButton)->SetLabel("HEX");
    else
        XRCCTRL(*this, "XmodemButtonXml", wxButton)->SetLabel("XMODEM");
    
    for (int drive=0; drive <4; drive++)
        setUpdFloppyGui(drive, XML);

    setBaudChoice(XML);
    setPrintModeXml();
    
    XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->Enable((elfConfiguration[XML].useTape || elfConfiguration[XML].useXmodem || elfConfiguration[XML].useHexModem || isTapeHwFred(XML)) && !conf[XML].realCassetteLoad_ && !isTapeHwCybervision(XML));
    XRCCTRL(*this, "TurboXml", wxCheckBox)->Enable((elfConfiguration[XML].useTape || elfConfiguration[XML].useXmodem || elfConfiguration[XML].useHexModem || elfConfiguration[XML].useTapeHw) && !conf[XML].realCassetteLoad_);
    XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->Enable((elfConfiguration[XML].useTape || elfConfiguration[XML].useXmodem || elfConfiguration[XML].useHexModem || elfConfiguration[XML].useTapeHw) && conf[XML].turbo_);
    XRCCTRL(*this, "TurboMhzTextXml", wxStaticText)->Enable((elfConfiguration[XML].useTape || elfConfiguration[XML].useXmodem || elfConfiguration[XML].useHexModem || elfConfiguration[XML].useTapeHw) && conf[XML].turbo_);
#if defined(__WXMSW__)
    XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->Enable(elfConfiguration[XML].useTape && !elfConfiguration[XML].useXmodem || elfConfiguration[XML].useHexModem || elfConfiguration[XML].useTapeHw);
#endif

    XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->Enable(elfConfiguration[XML].useTapeHw);
    if (hwTapeState_ == HW_TAPE_STATE_FF)
        XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardDarkGreenBitmap);
    else
        XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
    if (hwTapeState_ == HW_TAPE_STATE_RW)
        XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindDarkGreenBitmap);
    else
        XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
    if (elfConfiguration[XML].useTapeHw)
    {
        XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->Enable(true);
        if (hwTapeState_ == HW_TAPE_STATE_PLAY)
            XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playDarkGreenBitmap);
        else
            XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
    }
    if (elfConfiguration[XML].useTapeHw)
    {
        XRCCTRL(*this, "CasLoadXml", wxButton)->Enable(false);
        XRCCTRL(*this, "CasForwardXml", wxButton)->Enable(false);
        XRCCTRL(*this, "CasRewindXml", wxButton)->Enable(false);
    }

    XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->SetSelection(elfConfiguration[XML].baudR);
    XRCCTRL(*this, "VTBaudTChoiceXml", wxChoice)->SetSelection(elfConfiguration[XML].baudT);
    if (elfConfiguration[XML].vtExternal)
        XRCCTRL(*this, "VTTypeXml", wxChoice)->SetSelection(EXTERNAL_TERMINAL);
    else
        XRCCTRL(*this, "VTTypeXml", wxChoice)->SetSelection(elfConfiguration[XML].vtType);
  
    XRCCTRL(*this, "ControlWindowsXml", wxCheckBox)->SetValue(elfConfiguration[XML].useElfControlWindows);
    XRCCTRL(*this,"AddressText1Xml", wxStaticText)->Enable((elfConfiguration[XML].useElfControlWindows && elfConfiguration[XML].panelType_ != PANEL_NONE) || elfConfiguration[XML].ioConfiguration.statusBarType == STATUSBAR_VIP2);
    XRCCTRL(*this,"AddressText2Xml", wxStaticText)->Enable((elfConfiguration[XML].useElfControlWindows && elfConfiguration[XML].panelType_ != PANEL_NONE) || elfConfiguration[XML].ioConfiguration.statusBarType == STATUSBAR_VIP2);
    XRCCTRL(*this,"ShowAddressXml", wxTextCtrl)->Enable((elfConfiguration[XML].useElfControlWindows && elfConfiguration[XML].panelType_ != PANEL_NONE) || elfConfiguration[XML].ioConfiguration.statusBarType == STATUSBAR_VIP2);

    XRCCTRL(*this,"ControlWindowsXml", wxCheckBox)->Enable(elfConfiguration[XML].panelType_ != PANEL_NONE);

    XRCCTRL(*this, "InterlaceXml", wxCheckBox)->SetValue(conf[XML].interlace_);
    XRCCTRL(*this, "InterlaceXml", wxCheckBox)->Enable(elfConfiguration[XML].use6845 || elfConfiguration[XML].use8275);

    if (clockTextCtrl[XML] != NULL)
        clockTextCtrl[XML]->ChangeValue(conf[XML].clock_);

    conf[XML].videoNumber_ = VIDEOMAIN;
    
    if (conf[XML].numberOfVideoTypes_ > 0)
    {
        XRCCTRL(*this, "ZoomTextXml", wxStaticText)->SetLabel(conf[XML].videoName_[conf[XML].videoNumber_] + ", zoom:");
        XRCCTRL(*this, "ZoomTextXml", wxStaticText)->Enable(true);
        XRCCTRL(*this, "ZoomValueXml", wxTextCtrl)->Enable(true);
        XRCCTRL(*this, "ZoomSpinXml", wxSpinButton)->Enable(true);
        XRCCTRL(*this, "FullScreenF3Xml", wxBitmapButton)->Enable(true);
    }
    else
    {
        XRCCTRL(*this, "ZoomTextXml", wxStaticText)->SetLabel(".................. Zoom:");
        XRCCTRL(*this, "ZoomTextXml", wxStaticText)->Enable(false);
        XRCCTRL(*this, "ZoomValueXml", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "ZoomSpinXml", wxSpinButton)->Enable(false);
        XRCCTRL(*this, "FullScreenF3Xml", wxBitmapButton)->Enable(false);
    }
    
    if (conf[XML].numberOfVideoTypes_ > 1)
        XRCCTRL(*this, "VideoNumberXml", wxBitmapButton)->Enable(true);
    else
        XRCCTRL(*this, "VideoNumberXml", wxBitmapButton)->Enable(false);
    
    if (conf[XML].numberOfVideoTypes_ > 0)
        correctZoomAndValue(XML, "Xml", SET_SPIN, conf[XML].videoNumber_);
    
    XRCCTRL(*this, "BatchButtonXml", wxButton)->Enable(conf[XML].useBatchWav_);
    XRCCTRL(*this, "BatchFileXml", wxStaticText)->Enable(conf[XML].useBatchWav_);

    XRCCTRL(*this, "TextClearXml", wxStaticText)->Enable(elfConfiguration[XML].useRtcDS12887 || elfConfiguration[XML].useNvRam);

    XRCCTRL(*this, "XmlClearRtc", wxCheckBox)->Enable(elfConfiguration[XML].useRtcDS12887);
    XRCCTRL(*this, "XmlClearRtc", wxCheckBox)->SetValue(elfConfiguration[XML].clearRtc);

    XRCCTRL(*this, "XmlClearRam", wxCheckBox)->Enable(elfConfiguration[XML].useNvRam);
    XRCCTRL(*this, "XmlClearRam", wxCheckBox)->SetValue(elfConfiguration[XML].clearRam);

    XRCCTRL(*this, "UseLocationXml", wxCheckBox)->SetValue(conf[XML].useLoadLocation_);
    
    if (conf[XML].saveStart_ != 0)
        XRCCTRL(*this, "SaveStartXml", wxTextCtrl)->SetValue(conf[XML].saveStartString_);
    else
    {
        if (conf[XML].useLoadLocation_)
            XRCCTRL(*this, "SaveStartXml", wxTextCtrl)->SetValue("0000");
        else
            XRCCTRL(*this, "SaveStartXml", wxTextCtrl)->SetValue("");
    }

    XRCCTRL(*this,"DebugSCRT", wxCheckBox)->SetValue(conf[XML].scrtMode_);

    XRCCTRL(*this,"DebugSCRT",wxCheckBox)->Enable(true);
    if (conf[XML].scrtMode_)
    {
        XRCCTRL(*this,"DebugCallText",wxStaticText)->Enable(true);
        XRCCTRL(*this,"DebugCallReg",wxTextCtrl)->Enable(true);
        XRCCTRL(*this,"DebugCallAddress",wxTextCtrl)->Enable(true);
        XRCCTRL(*this,"DebugRetText",wxStaticText)->Enable(true);
        XRCCTRL(*this,"DebugRetReg",wxTextCtrl)->Enable(true);
        XRCCTRL(*this,"DebugRetAddress",wxTextCtrl)->Enable(true);
    }
    
    wxString valueString;
    
    if (conf[XML].debugCallReg_ == -1)
        valueString = "";
    else
        valueString.Printf("%01X", (int)conf[XML].debugCallReg_);
    XRCCTRL(*this,"DebugCallReg",wxTextCtrl)->ChangeValue(valueString);
    
    if (conf[XML].debugCallAddress_ == -1)
        valueString = "";
    else
        valueString.Printf("%04X", (int)conf[XML].debugCallAddress_);
    XRCCTRL(*this,"DebugCallAddress",wxTextCtrl)->ChangeValue(valueString);
    
    if (conf[XML].debugRetReg_ == -1)
        valueString = "";
    else
        valueString.Printf("%01X", (int)conf[XML].debugRetReg_);
    
    XRCCTRL(*this,"DebugRetReg",wxTextCtrl)->ChangeValue(valueString);
    if (conf[XML].debugRetAddress_ == -1)
        valueString = "";
    else
        valueString.Printf("%04X", (int)conf[XML].debugRetAddress_);
    XRCCTRL(*this,"DebugRetAddress",wxTextCtrl)->ChangeValue(valueString);

    if (conf[XML].soundType_ == SOUND_SUPER_VP550 || conf[XML].soundType_ == SOUND_SUPER_VP551)
    {
        XRCCTRL(*this,"TempoTextXml", wxStaticText)->Enable(true);
        XRCCTRL(*this,"TempoXml", wxSlider)->Enable(true);
    }
    else
    {
        XRCCTRL(*this,"TempoTextXml", wxStaticText)->Enable(false);
        XRCCTRL(*this,"TempoXml", wxSlider)->Enable(false);
    }
    XRCCTRL(*this, "TempoXml", wxSlider)->SetValue(conf[XML].tempo_);
}

void GuiXml::onVideoNumber(wxCommandEvent&WXUNUSED(event))
{
    conf[XML].videoNumber_++;
    if (conf[XML].videoNumber_ == conf[XML].numberOfVideoTypes_)
        conf[XML].videoNumber_ = 0;
    
    XRCCTRL(*this, "ZoomTextXml", wxStaticText)->SetLabel(conf[XML].videoName_[conf[XML].videoNumber_] + ", zoom:");
    correctZoomAndValue(XML, "Xml", SET_SPIN, conf[XML].videoNumber_);
}


void GuiXml::onXmlPrintFileText(wxCommandEvent&WXUNUSED(event))
{
    conf[XML].printFile_ = XRCCTRL(*this, "PrintFileXml", wxTextCtrl)->GetValue();

    if (runningComputer_ == XML)
    {
        if (conf[XML].useBasicPrinter_)
        {
            if (conf[XML].printFile_.Len() == 0)
                p_PrinterParallel->setPrintfileName("");
            else
                p_PrinterParallel->setPrintfileName(conf[XML].printFileDir_ + conf[XML].printFile_);
        }
        if (conf[XML].useParallelPrinter_)
        {
            if (conf[XML].printFile_.Len() == 0)
                p_PrinterParallel->setPrintfileName("");
            else
                p_PrinterParallel->setPrintfileName(conf[XML].printFileDir_ + conf[XML].printFile_);
        }
        if (conf[XML].useSerialPrinter_)
        {
            if (conf[XML].printFile_.Len() == 0)
                p_PrinterSerial->setPrintfileName("");
            else
                p_PrinterSerial->setPrintfileName(conf[XML].printFileDir_ + conf[XML].printFile_);
        }
        if (conf[XML].useThermalPrinter_)
        {
            if (conf[XML].printFile_.Len() == 0)
                p_PrinterThermal->setPrintfileName("");
            else
                p_PrinterThermal->setPrintfileName(conf[XML].printFileDir_ + conf[XML].printFile_);
        }
    }
}

void GuiXml::onXmlPrintMode(wxCommandEvent&event)
{
    conf[XML].printMode_ = event.GetSelection();
    setPrintMode();

    if (runningComputer_ == XML)
    {
        if (conf[XML].useParallelPrinter_)
        {
            p_PrinterParallel->setPrintMode(conf[XML].printMode_);
        }
        if (conf[XML].useSerialPrinter_)
        {
            p_PrinterSerial->setPrintMode(conf[XML].printMode_);
        }
    }
}

void GuiXml::onXmlPrintButton(wxCommandEvent&WXUNUSED(event))
{
#define FORECE_START true
    onXmlF4(FORECE_START);
}

void GuiXml::onXmlF4(bool forceStart)
{
    if (runningComputer_ == XML)
    {
        p_Xmlemu->onXmlF4(forceStart);
        if (conf[runningComputer_].printMode_ == PRINTFILE)
        {
            conf[runningComputer_].printMode_ = PRINTPLOTTER;
            if (mode_.gui)
                XRCCTRL(*this, "PrintModeXml", wxChoice)->SetSelection(conf[runningComputer_].printMode_);
            setPrintMode();
        }
    }
}

void GuiXml::onXmlBaudR(wxCommandEvent&event)
{
    if (elfConfiguration[XML].baudR != event.GetSelection())
    {
        XRCCTRL(*this, "XmlClearRam", wxCheckBox)->SetValue(true);
        elfConfiguration[XML].clearRam = true;
        elfConfiguration[XML].baudR = event.GetSelection();
    }
}

void GuiXml::onXmlBaudT(wxCommandEvent&event)
{
    if (elfConfiguration[XML].baudT != event.GetSelection())
    {
        XRCCTRL(*this, "XmlClearRam", wxCheckBox)->SetValue(true);
        elfConfiguration[XML].clearRam = true;
        elfConfiguration[XML].baudT = event.GetSelection();
        if (!elfConfiguration[XML].useUart && !elfConfiguration[selectedComputer_].useUart16450)
        {
            elfConfiguration[XML].baudR = event.GetSelection();
            XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->SetSelection(elfConfiguration[XML].baudR);
        }
    }
}

void GuiXml::onTempo(wxScrollEvent&event)
{
    conf[selectedComputer_].tempo_ = event.GetPosition();
    if (computerRunning_)
        p_Computer->setTempo(conf[selectedComputer_].tempo_);
}


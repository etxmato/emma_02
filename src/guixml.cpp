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

    EVT_TEXT(XRCID("MainRamXml"), GuiXml::onMainRamTextXml)
    EVT_COMBOBOX(XRCID("MainRamXml"), GuiXml::onMainRamTextXml)
    EVT_BUTTON(XRCID("RamButtonXml"), GuiXml::onMainRamXml)

    EVT_TEXT(XRCID("MainXmlXml"), GuiXml::onMainXmlTextXml)
    EVT_COMBOBOX(XRCID("MainXmlXml"), GuiXml::onMainXmlComboXml)
    EVT_BUTTON(XRCID("XmlButtonXml"), GuiXml::onMainXmlXml)

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
    EVT_BUTTON(XRCID("CasLoadXml"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasForwardXml"), GuiMain::onCassetteForward)
    EVT_BUTTON(XRCID("CasRewindXml"), GuiMain::onCassetteRewind)
    EVT_BUTTON(XRCID("CasSaveXml"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasStopXml"), GuiMain::onCassetteStop)
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

    END_EVENT_TABLE()

GuiXml::GuiXml(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiPico(title, pos, size, mode, dataDir, iniDir)
{
    tapeOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/tick.png", wxBITMAP_TYPE_PNG);
    tapeOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/minus.png", wxBITMAP_TYPE_PNG);
    
    dropdownUpdateOngoing_ = false;
}

void GuiXml::readXmlConig()
{
//    return; // *** to be removed
    
    selectedComputer_ = XML; // *** to be removed
    elfConfiguration[XML].ioConfiguration.emsOutput.resize(1);

    fileNameXml[0] = "";
    fileDirXml[0] = dataDir_ + "Xml" + pathSeparator_;
    fileNameXml[1] = configPointer->Read("Xmlemu/XmlFile1", "comx,superboard.xml");
    fileDirXml[1] = readConfigDir("Dir/Xmlemu/XmlFile1", dataDir_ + "Xml" + pathSeparator_ + "Comx" + pathSeparator_);
    fileNameXml[2] = configPointer->Read("Xmlemu/XmlFile2", "comx,fdc-print-32k-eprom.xml");
    fileDirXml[2] = readConfigDir("Dir/Xmlemu/XmlFile2", dataDir_ + "Xml" + pathSeparator_ + "Comx" + pathSeparator_);
    fileNameXml[3] = configPointer->Read("Xmlemu/XmlFile3", "cidelsa,altair.xml");
    fileDirXml[3] = readConfigDir("Dir/Xmlemu/XmlFile3", dataDir_ + "Xml" + pathSeparator_ + "Cidelsa" + pathSeparator_);
    fileNameXml[4] = configPointer->Read("Xmlemu/XmlFile4", "comix,bare-pal.xml");
    fileDirXml[4] = readConfigDir("Dir/Xmlemu/XmlFile4", dataDir_ + "Xml" + pathSeparator_ + "Comix" + pathSeparator_);
    fileNameXml[5] = configPointer->Read("Xmlemu/XmlFile5", "cosmac-elf,bare.xml");
    fileDirXml[5] = readConfigDir("Dir/Xmlemu/XmlFile5", dataDir_ + "Xml" + pathSeparator_ + "Cosmac Elf" + pathSeparator_);
    fileNameXml[6] = configPointer->Read("Xmlemu/XmlFile6", "elf,superbasic6.0-supervideo.xml");
    fileDirXml[6] = readConfigDir("Dir/Xmlemu/XmlFile6", dataDir_ + "Xml" + pathSeparator_ + "Elf" + pathSeparator_);
    fileNameXml[7] = configPointer->Read("Xmlemu/XmlFile7", "microtutor2-tops.xml");
    fileDirXml[7] = readConfigDir("Dir/Xmlemu/XmlFile7", dataDir_ + "Xml" + pathSeparator_ + "Microtutor II" + pathSeparator_);
    fileNameXml[8] = configPointer->Read("Xmlemu/XmlFile8", "pecom,64.v4.xml");
    fileDirXml[8] = readConfigDir("Dir/Xmlemu/XmlFile8", dataDir_ + "Xml" + pathSeparator_ + "Pecom" + pathSeparator_);
    fileNameXml[9] = configPointer->Read("Xmlemu/XmlFile9", "pico,elfos-uart.xml");
    fileDirXml[9] = readConfigDir("Dir/Xmlemu/XmlFile9", dataDir_ + "Xml" + pathSeparator_ + "Pico Elf V2" + pathSeparator_);
    fileNameXml[10] = configPointer->Read("Xmlemu/XmlFile10", "tmc-600,exp-151182.xml");
    fileDirXml[10] = readConfigDir("Dir/Xmlemu/XmlFile10", dataDir_ + "Xml" + pathSeparator_ + "TMC-600" + pathSeparator_);
    fileNameXml[11] = configPointer->Read("Xmlemu/XmlFile11", "cybervision2001.xml");
    fileDirXml[11] = readConfigDir("Dir/Xmlemu/XmlFile11", dataDir_ + "Xml" + pathSeparator_ + "Cybervision" + pathSeparator_);

    if (mode_.gui)
        setXmlDropDown();
    dropdownUpdateOngoing_ = false;

    conf[XML].xmlDir_ = fileDirXml[1];
    conf[XML].xmlFile_ = fileNameXml[1];

    clearXmlData(XML);
    parseXmlFile(XML,conf[XML].xmlDir_, conf[XML].xmlFile_);
 
    selectedComputer_ = XML;

    conf[XML].volume_ = 0;

    conf[XML].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + computerInfo[XML].gui + pathSeparator_;

    conf[XML].terminalFiles_.Add(conf[XML].xmodemFile_);
    conf[XML].terminalPaths_.Add(conf[XML].xmodemFileDir_+conf[XML].xmodemFile_);
    conf[XML].numberOfTerminalFiles_ = 1;

    conf[XML].saveEndString_ = "";
    conf[XML].saveExecString_ = "";

    elfConfiguration[XML].vt52SetUpFeature_ = configPointer->Read("Xmlemu/VT52Setup", 0x00004092l);
    elfConfiguration[XML].vt100SetUpFeature_ = configPointer->Read("Xmlemu/VT100Setup", 0x0000ca52l);
    elfConfiguration[XML].vtExternalSetUpFeature_ = configPointer->Read("Xmlemu/VTExternalSetup", 0x0000ca52l);
    
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
    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[XML].xScale_ = convertLocale(configPointer->Read("Xmlemu/Window_Scale_Factor_X", defaultScale));

    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[XML].ledTime_ = configPointer->Read("Xmlemu/Led_Update_Frequency", defaultTimer);

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
    
    setRealCas(XML);
    setXmlGui();
    
    if (mode_.gui)
    {
        XRCCTRL(*this, "MainXmlXml", wxComboBox)->SetValue(conf[XML].xmlFile_);
    
        correctZoomVtAndValue(XML, "Xml", SET_SPIN);
            
        XRCCTRL(*this, "StretchDotXml", wxCheckBox)->SetValue(conf[XML].stretchDot_);

        XRCCTRL(*this, "PrintModeXml", wxChoice)->SetSelection(conf[XML].printMode_);

        XRCCTRL(*this, "TurboXml", wxCheckBox)->SetValue(conf[XML].turbo_);

        XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->SetValue(conf[XML].turboClock_);
        XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->SetValue(conf[XML].autoCassetteLoad_);

        XRCCTRL(*this, "VolumeXml", wxSlider)->SetValue(conf[XML].volume_);

        XRCCTRL(*this, "ShowAddressXml", wxTextCtrl)->ChangeValue(conf[XML].ledTime_);

        XRCCTRL(*this, "UseLocationXml", wxCheckBox)->SetValue(conf[XML].useLoadLocation_);
        if (conf[XML].saveStart_ != 0)
            XRCCTRL(*this, "SaveStartXml", wxTextCtrl)->SetValue(conf[XML].saveStartString_);
    }

    conf[XML].loadFileNameFull_ = "";
    conf[XML].loadFileName_ = "";
}

void GuiXml::writeXmlDirConfig()
{
    wxString number, type;
    
    for (int i=1; i<MAX_XML_DROPDOWN_FILES; i++)
    {
        number.Printf("%d",i);
        writeConfigDir("/Dir/Xmlemu/XmlFile" + number, fileDirXml[i]);
    }
    
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
    for (int i=1; i<MAX_XML_DROPDOWN_FILES; i++)
    {
        number.Printf("%d",i);
        configPointer->Write("Xmlemu/XmlFile" + number, fileNameXml[i]);
    }
    
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
    configPointer->Write("Xmlemu/Ymodem_PacketSize", elfConfiguration[XML].packetSize);
    configPointer->Write("Xmlemu/Volume", conf[XML].volume_);
}

void GuiXml::readXmlWindowConfig()
{
//    return; // *** to be removed
    
    conf[XML].pixieX_ = (int)configPointer->Read("Xmlemu/Window_Position_Pixie_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[XML].pixieY_ = (int)configPointer->Read("Xmlemu/Window_Position_Pixie_Y", mainWindowY_);
    conf[XML].cdp1864X_ = (int)configPointer->Read("Xmlemu/Window_Position_CDP1864_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[XML].cdp1864Y_ = (int)configPointer->Read("Xmlemu/Window_Position_CDP1864_Y", mainWindowY_);
    conf[XML].tmsX_ = (int)configPointer->Read("Xmlemu/Window_Position_Tms_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[XML].tmsY_ = (int)configPointer->Read("Xmlemu/Window_Position_Tms_Y", mainWindowY_);
    conf[XML].mc6845X_ = (int)configPointer->Read("Xmlemu/Window_Position_MC6845_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[XML].mc6845Y_ = (int)configPointer->Read("Xmlemu/Window_Position_MC6845_Y", mainWindowY_);
    conf[XML].mc6847X_ = (int)configPointer->Read("Xmlemu/Window_Position_MC6847_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[XML].mc6847Y_ = (int)configPointer->Read("Xmlemu/Window_Position_MC6847_Y", mainWindowY_);
    conf[XML].i8275X_ = (int)configPointer->Read("Xmlemu/Window_Position_I8275_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[XML].i8275Y_ = (int)configPointer->Read("Xmlemu/Window_Position_I8275_Y", mainWindowY_);
    conf[XML].v1870X_ = (int)configPointer->Read("Xmlemu/Window_Position_v1870_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[XML].v1870Y_ = (int)configPointer->Read("Xmlemu/Window_Position_v1870_Y", mainWindowY_);
    conf[XML].SN76430NX_ = (int)configPointer->Read("Xmlemu/Window_Position_SN76430N_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[XML].SN76430NY_ = (int)configPointer->Read("Xmlemu/Window_Position_SN76430N_Y", mainWindowY_);
    conf[XML].vtX_ = (int)configPointer->Read("Xmlemu/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[XML].vtY_ = (int)configPointer->Read("Xmlemu/Window_Position_Vt_Y", mainWindowY_);
    conf[XML].mainX_ = (int)configPointer->Read("Xmlemu/Window_Position_X", mainWindowX_);
    conf[XML].mainY_ = (int)configPointer->Read("Xmlemu/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    ledModuleX_ = (int)configPointer->Read("Xmlemu/Window_Position_Led_Module_X", mainWindowX_+346+windowInfo.xBorder2);
    ledModuleY_ = (int)configPointer->Read("Xmlemu/Window_Position_Led_Module_Y", mainWindowY_+windowInfo.mainwY+229+windowInfo.yBorder2);
    conf[XML].keypadX_ = (int)configPointer->Read("Xmlemu/Window_Position_Keypad_X", mainWindowX_+346+windowInfo.xBorder2);
    conf[XML].keypadY_ = (int)configPointer->Read("Xmlemu/Window_Position_Keypad_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiXml::writeXmlWindowConfig()
{
//    return; // *** to be removed xx
    
    if (conf[XML].mainX_ > 0)
        configPointer->Write("Xmlemu/MainX", conf[XML].mainX_);
    if (conf[XML].mainY_ > 0)
        configPointer->Write("Xmlemu/MainY", conf[XML].mainY_);
    if (conf[XML].pixieX_ > 0)
        configPointer->Write("Xmlemu/Window_Position_Pixie_X", conf[XML].pixieX_);
    if (conf[XML].pixieY_ > 0)
        configPointer->Write("Xmlemu/Window_Position_Pixie_Y", conf[XML].pixieY_);
    if (conf[XML].cdp1864X_ > 0)
        configPointer->Write("Xmlemu/Window_Position_CDP1864_X", conf[XML].cdp1864X_);
    if (conf[XML].cdp1864Y_ > 0)
        configPointer->Write("Xmlemu/Window_Position_CDP1864_Y", conf[XML].cdp1864Y_);
    if (conf[XML].tmsX_ > 0)
        configPointer->Write("Xmlemu/Window_Position_Tms_X", conf[XML].tmsX_);
    if (conf[XML].tmsY_ > 0)
        configPointer->Write("Xmlemu/Window_Position_Tms_Y", conf[XML].tmsY_);
    if (conf[XML].mc6845X_ > 0)
        configPointer->Write("Xmlemu/Window_Position_MC6845_X", conf[XML].mc6845X_);
    if (conf[XML].mc6845Y_ > 0)
        configPointer->Write("Xmlemu/Window_Position_MC6845_Y", conf[XML].mc6845Y_);
    if (conf[XML].mc6847X_ > 0)
        configPointer->Write("Xmlemu/Window_Position_MC6847_X", conf[XML].mc6847X_);
    if (conf[XML].mc6847Y_ > 0)
        configPointer->Write("Xmlemu/Window_Position_MC6847_Y", conf[XML].mc6847Y_);
    if (conf[XML].i8275X_ > 0)
        configPointer->Write("Xmlemu/Window_Position_I8275_X", conf[XML].i8275X_);
    if (conf[XML].i8275Y_ > 0)
        configPointer->Write("Xmlemu/Window_Position_I8275_Y", conf[XML].i8275Y_);
    if (conf[XML].v1870X_ > 0)
        configPointer->Write("Xmlemu/Window_Position_v1870_X", conf[XML].v1870X_);
    if (conf[XML].v1870Y_ > 0)
        configPointer->Write("Xmlemu/Window_Position_v1870_Y", conf[XML].v1870Y_);
    if (conf[XML].SN76430NX_ > 0)
        configPointer->Write("Xmlemu/Window_Position_SN76430N_X", conf[XML].SN76430NX_);
    if (conf[XML].SN76430NY_ > 0)
        configPointer->Write("Xmlemu/Window_Position_SN76430N_Y", conf[XML].SN76430NY_);
    if (conf[XML].vtX_ > 0)
        configPointer->Write("Xmlemu/Window_Position_Vt_X", conf[XML].vtX_);
    if (conf[XML].vtY_ > 0)
        configPointer->Write("Xmlemu/Window_Position_Vt_Y", conf[XML].vtY_);
    if (conf[XML].keypadX_ > 0)
        configPointer->Write("Xmlemu//Window_Position_Keypad_X", conf[XML].keypadX_);
    if (conf[XML].keypadY_ > 0)
        configPointer->Write("Xmlemu//Window_Position_Keypad_Y", conf[XML].keypadY_);
    if (ledModuleX_ > 0)
        configPointer->Write("Xmlemu//Window_Position_Led_Module_X", ledModuleX_);
    if (ledModuleY_ > 0)
        configPointer->Write("Xmlemu//Window_Position_Led_Module_Y", ledModuleY_);
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

void GuiXml::onMainRamXml(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;

    fileName = wxFileSelector( "Select the RAM file to load",
                              conf[selectedComputer_].memConfig_[0].dirname, XRCCTRL(*this, "MainRamXml", wxComboBox)->GetValue(),
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
    conf[selectedComputer_].memConfig_[0].dirname = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].memConfig_[0].filename = FullPath.GetFullName();

    XRCCTRL(*this, "MainRamXml", wxComboBox)->SetValue(conf[selectedComputer_].memConfig_[0].filename);
}

void GuiXml::onMainRamTextXml(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].memConfig_[0].filename = XRCCTRL(*this, "MainRamXml", wxComboBox)->GetValue();
}

void GuiXml::onMainXmlXml(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;

    fileName = wxFileSelector( "Select the XML file to load",
                              conf[selectedComputer_].xmlDir_, XRCCTRL(*this, "MainXmlXml", wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                   "XML|*.xml|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
    if (!fileName)
        return;

    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    conf[selectedComputer_].xmlDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    conf[selectedComputer_].xmlFile_ = FullPath.GetFullName();

    bool found = false;
    
    for (int i=1; i<MAX_XML_DROPDOWN_FILES; i++)
    {
        if (fileNameXml[i] == conf[selectedComputer_].xmlFile_ && fileDirXml[i] == conf[selectedComputer_].xmlDir_)
            found = true;
    }
    
    if (found)
        XRCCTRL(*this, "MainXmlXml", wxComboBox)->SetValue(conf[selectedComputer_].xmlFile_);
    else
    {
        for (int i=MAX_XML_DROPDOWN_FILES-1; i>1; i--)
        {
            fileNameXml[i] = fileNameXml[i-1];
            fileDirXml[i] = fileDirXml[i-1];
        }
        fileNameXml[1] = conf[selectedComputer_].xmlFile_;
        fileDirXml[1] = conf[selectedComputer_].xmlDir_;
        
        setXmlDropDown();
        dropdownUpdateOngoing_ = false;

        XRCCTRL(*this, "MainXmlXml", wxComboBox)->SetValue(fileNameXml[1]);
    }
}

void GuiXml::onMainXmlTextXml(wxCommandEvent& WXUNUSED(event))
{
    if (dropdownUpdateOngoing_)
        return;

    conf[selectedComputer_].xmlFile_ = XRCCTRL(*this, "MainXmlXml", wxComboBox)->GetValue();
    
    parseXmlFile(selectedComputer_, conf[selectedComputer_].xmlDir_, conf[selectedComputer_].xmlFile_);
    setXmlGui();
}

void GuiXml::onMainXmlComboXml(wxCommandEvent& event)
{
    int selection = event.GetSelection();
    if (selection >= MAX_XML_DROPDOWN_FILES)
        selection = MAX_XML_DROPDOWN_FILES-1;
    
    conf[selectedComputer_].xmlFile_ = fileNameXml[selection];
    conf[selectedComputer_].xmlDir_ = fileDirXml[selection];

    parseXmlFile(selectedComputer_, conf[selectedComputer_].xmlDir_, conf[selectedComputer_].xmlFile_);
    setXmlGui();
}

void GuiXml::setXmlDropDown()
{
    dropdownUpdateOngoing_ = true;
    for (int i=0; i<MAX_XML_DROPDOWN_FILES; i++)
        XRCCTRL(*this, "MainXmlXml", wxComboBox)->SetString(i, fileNameXml[i]);
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

void GuiXml::setXmlGui()
{
    setVtType("Xml", XML, elfConfiguration[XML].vtType, false);
    
    if (!mode_.gui)
        return;
  
    XRCCTRL(*this,"MainRamXml", wxComboBox)->Enable(conf[XML].ramFileFromGui_);
    XRCCTRL(*this,"RamButtonXml", wxButton)->Enable(conf[XML].ramFileFromGui_);
    XRCCTRL(*this, "MainRamXml", wxComboBox)->SetValue(conf[XML].memConfig_[0].filename);
    
    XRCCTRL(*this, "WavFileXml", wxTextCtrl)->SetValue(conf[XML].wavFile_[0]);
    XRCCTRL(*this, "WavFile1Xml", wxTextCtrl)->SetValue(conf[XML].wavFile_[1]);

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
    
    XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->Enable((elfConfiguration[XML].useTape || elfConfiguration[XML].useXmodem || elfConfiguration[XML].useHexModem) && !conf[XML].realCassetteLoad_ && !elfConfiguration[XML].useTapeHw);
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
    XRCCTRL(*this,"AddressText1Xml", wxStaticText)->Enable(elfConfiguration[XML].useElfControlWindows && elfConfiguration[XML].panelType_ != PANEL_NONE);
    XRCCTRL(*this,"AddressText2Xml", wxStaticText)->Enable(elfConfiguration[XML].useElfControlWindows && elfConfiguration[XML].panelType_ != PANEL_NONE);
    XRCCTRL(*this,"ShowAddressXml", wxTextCtrl)->Enable(elfConfiguration[XML].useElfControlWindows && elfConfiguration[XML].panelType_ != PANEL_NONE);

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



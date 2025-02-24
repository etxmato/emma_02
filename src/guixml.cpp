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

BEGIN_EVENT_TABLE(GuiXml, GuiVipII)

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

    EVT_TEXT(XRCID("KeyFileXml"), GuiXml::onKeyFileText)
    EVT_BUTTON(XRCID("KeyFileButtonXml"), GuiXml::onKeyFile)
    EVT_BUTTON(XRCID("EjectKeyFileXml"), GuiXml::onKeyFileEject)

    EVT_CHOICE(XRCID("VTTypeXml"), GuiXml::onVT100)

    EVT_SPIN_UP(XRCID("ZoomSpinXml"), GuiMain::onZoom)
    EVT_SPIN_DOWN(XRCID("ZoomSpinXml"), GuiMain::onZoom)
    EVT_TEXT(XRCID("ZoomValueXml"), GuiMain::onZoomValue)
    EVT_SPIN_UP(XRCID("ZoomSpinVtXml"), GuiMain::onZoomVt)
    EVT_SPIN_DOWN(XRCID("ZoomSpinVtXml"), GuiMain::onZoomVt)
    EVT_TEXT(XRCID("ZoomValueVtXml"), GuiMain::onZoomValueVt)
    EVT_CHECKBOX(XRCID("InterlaceXml"), GuiMain::onInterlace)
    EVT_CHECKBOX(XRCID("StretchDotXml"), GuiMain::onStretchDot)
    EVT_BUTTON(XRCID("ScreenDumpFileButtonXml"), GuiXml::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileXml"), GuiXml::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileXml"), GuiXml::onScreenDumpFileText)
    EVT_BUTTON(XRCID("ScreenDumpF5Xml"), GuiMain::onScreenDump)
    EVT_BUTTON(XRCID("SaveButtonXml"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("VtSetupXml"), GuiMain::onVtSetup)
    EVT_BUTTON(XRCID("LoadButtonXml"), GuiMain::onLoadButton)
    EVT_BUTTON(XRCID("RunButtonXml"), GuiMain::onLoadRunButton)
    EVT_BUTTON(XRCID("DsaveButtonXml"), GuiMain::onDataSaveButton)
    EVT_CHECKBOX(XRCID("UseLocationXml"), GuiXml::onUseLocationXml)

    EVT_BUTTON(XRCID("FullScreenF3Xml"), GuiMain::onFullScreen)

    EVT_TEXT(XRCID("PrintFileXml"), GuiXml::onXmlPrintFileText)
    EVT_BUTTON(XRCID("PrintButtonXml"), GuiXml::onXmlPrintButton)
    EVT_CHOICE(XRCID("PrintModeXml"), GuiXml::onXmlPrintMode)

    EVT_BUTTON(XRCID("PrintFileButtonXml"), GuiXml::onPrintFile)
        
    EVT_CHOICE(XRCID("VTBaudTChoiceXml"), GuiXml::onXmlBaudT)
    EVT_CHOICE(XRCID("VTBaudRChoiceXml"), GuiXml::onXmlBaudR)

    EVT_CHECKBOX(XRCID("ControlWindowsXml"), GuiXml::onXmlControlWindows)
    EVT_BUTTON(XRCID("KeyMapXml"), Main::onHexKeyDef)

    EVT_BUTTON(XRCID("FDC0_ButtonXml"), GuiXml::onDisk0)
    EVT_TEXT(XRCID("FDC0_FileXml"), GuiXml::onDiskText0)
    EVT_BUTTON(XRCID("Eject_FDC0Xml"), GuiXml::onDiskEject0)
    EVT_BUTTON(XRCID("FDC0_SwitchXml"), GuiXml::onDiskDirSwitch0)
    EVT_BUTTON(XRCID("FDC1_ButtonXml"), GuiXml::onDisk1)
    EVT_TEXT(XRCID("FDC1_FileXml"), GuiXml::onDiskText1)
    EVT_BUTTON(XRCID("Eject_FDC1Xml"), GuiXml::onDiskEject1)
    EVT_BUTTON(XRCID("FDC1_SwitchXml"), GuiXml::onDiskDirSwitch1)
    EVT_BUTTON(XRCID("FDC2_ButtonXml"), GuiXml::onDisk2)
    EVT_TEXT(XRCID("FDC2_FileXml"), GuiXml::onDiskText2)
    EVT_BUTTON(XRCID("Eject_FDC2Xml"), GuiXml::onDiskEject2)
    EVT_BUTTON(XRCID("FDC2_SwitchXml"), GuiXml::onDiskDirSwitch2)
    EVT_BUTTON(XRCID("FDC3_ButtonXml"), GuiXml::onDisk3)
    EVT_TEXT(XRCID("FDC3_FileXml"), GuiXml::onDiskText3)
    EVT_BUTTON(XRCID("Eject_FDC3Xml"), GuiXml::onDiskEject3)
    EVT_BUTTON(XRCID("FDC3_SwitchXml"), GuiXml::onDiskDirSwitch3)

    EVT_BUTTON(XRCID("CasButtonXml"), GuiXml::onCassette)
    EVT_BUTTON(XRCID("EjectCasXml"), GuiXml::onCassetteEject)
    EVT_TEXT(XRCID("WavFileXml"), GuiXml::onCassetteText)

    EVT_BUTTON(XRCID("CasButton1Xml"), GuiXml::onCassette1)
    EVT_BUTTON(XRCID("EjectCas1Xml"), GuiXml::onCassette1Eject)
    EVT_TEXT(XRCID("WavFile1Xml"), GuiXml::onCassette1Text)

    EVT_BUTTON(XRCID("XmodemButtonXml"), GuiXml::onXmodem)
    EVT_BUTTON(XRCID("EjectXmodemXml"), GuiXml::onXmodemEject)
    EVT_TEXT(XRCID("XmodemFileXml"), GuiXml::onXmodemText)

    EVT_BUTTON(XRCID("RealCasLoadXml"), GuiMain::onRealCas)
    EVT_BUTTON(XRCID("CasStopXml"), GuiMain::onCassetteStop)
    EVT_BUTTON(XRCID("CasSaveXml"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasRewindXml"), GuiMain::onCassetteRewind)
    EVT_BUTTON(XRCID("CasLoadXml"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasForwardXml"), GuiMain::onCassetteForward)
    EVT_BUTTON(XRCID("CasPauseXml"), GuiMain::onCassettePause)

    EVT_CHECKBOX(XRCID("TurboXml"), GuiXml::onTurboXml)
    EVT_TEXT(XRCID("TurboClockXml"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadXml"), GuiXml::onAutoLoadXml)

    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeXml"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeXml"), GuiMain::onVolume)

    EVT_TEXT(XRCID("ShowAddressXml"), GuiXml::onLedTimerXml)
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
: GuiVipII(title, pos, size, mode, dataDir, iniDir)
{
    dropdownUpdateOngoing_ = false;
    xmlFileComboSelection = 0;
    xmlDirComboSelection = 0;
    romRamFileComboSelection[0] = 0;
    romRamFileComboSelection[1] = 0;
}

void GuiXml::readXmlConfig()
{
    selectedTab_ = XML;

    xmlDirComboSelection = (int)configPointer->Read("Computer/XmlDirComboSelection", 2);
    xmlDirComboString = configPointer->Read("Computer/XmlDirComboString", "Comx");
    computerConfiguration.xmlFileConfiguration.mainDirectory = readConfigDir("Dir/Computer/XmlFile", dataDir_ + "Xml" + pathSeparator_);
    
    setXmlDirDropDown();
    setXmlSubDir(xmlDirComboString);
    computerConfiguration.xmlFileConfiguration.directory = computerConfiguration.xmlFileConfiguration.mainDirectory + computerConfiguration.xmlFileConfiguration.subDirectory + pathSeparator_;
    setXmlDropDown();

    dropdownUpdateOngoing_ = false;

    parseXmlFile(computerConfiguration.xmlFileConfiguration.directory, computerConfiguration.xmlFileConfiguration.fileName);
    
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

    computerConfiguration.videoTerminalConfiguration.terminalFiles.Add(computerConfiguration.videoTerminalConfiguration.xmodemFileName);
    computerConfiguration.videoTerminalConfiguration.terminalPaths.Add(computerConfiguration.videoTerminalConfiguration.xmodemDirectory+computerConfiguration.videoTerminalConfiguration.xmodemFileName);
    computerConfiguration.videoTerminalConfiguration.numberOfTerminalFiles = 1;

    computerConfiguration.memAccessConfiguration.saveEndString = "";
    computerConfiguration.memAccessConfiguration.saveExecString = "";

    configPointer->Read("Computer/SerialLog", &computerConfiguration.videoTerminalConfiguration.serialLog, false);
    configPointer->Read("Computer/ESCError", &computerConfiguration.videoTerminalConfiguration.escError, false);
    computerConfiguration.clearRam = false;
    computerConfiguration.clearRtc = false;

    configPointer->Read("Computer/Enable_Vt_Stretch_Dot", &computerConfiguration.videoTerminalConfiguration.stretchDot, false);
    configPointer->Read("Computer/Enable_Vt_External", &computerConfiguration.videoTerminalConfiguration.external, false);
    configPointer->Read("Computer/Enable_Vt_LoopBack", &computerConfiguration.videoTerminalConfiguration.loop_back, false);
    computerConfiguration.printMode_ = (int)configPointer->Read("Computer/Print_Mode", 1l);

    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 1.0);
    computerConfiguration.videoTerminalConfiguration.zoom = convertLocale(configPointer->Read("Computer/Vt_Zoom", defaultZoom));

    computerConfiguration.turboClock_ = configPointer->Read("Computer/Turbo_Clock_Speed", "15");
    computerConfiguration.videoTerminalConfiguration.packetSize = (int)configPointer->Read("Computer/Ymodem_PacketSize", 0l);
    configPointer->Read("Computer/Enable_Real_Cassette", &computerConfiguration.realCassetteLoad_, false);
    computerConfiguration.soundConfiguration.volume = (int)configPointer->Read("Computer/Volume", 25l);

    hwTapeState_ = HW_TAPE_STATE_PLAY;
        
    setRealCas();
    setXmlGui();
    
    computerConfiguration.soundConfiguration.tempo = (int)configPointer->Read("/Computer/Tempo", 100l);

    if (mode_.gui)
    {
        correctZoomVtAndValue(SET_SPIN);
            
        XRCCTRL(*this, "StretchDotXml", wxCheckBox)->SetValue(computerConfiguration.videoTerminalConfiguration.stretchDot);

        XRCCTRL(*this, "PrintModeXml", wxChoice)->SetSelection(computerConfiguration.printMode_);

        XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->SetValue(computerConfiguration.turboClock_);

        XRCCTRL(*this, "VolumeXml", wxSlider)->SetValue(computerConfiguration.soundConfiguration.volume);
    }

    computerConfiguration.memAccessConfiguration.fullFileName = "";
    computerConfiguration.memAccessConfiguration.fileName = "";
}

void GuiXml::writeXmlDirConfig()
{
    wxString number, type;
    
    writeConfigDir("/Dir/Computer/XmlFile", computerConfiguration.xmlFileConfiguration.mainDirectory);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/"+computerConfiguration.memoryConfiguration[romRamButton0_].filename+"/Dir/GuiRomRam0", computerConfiguration.memoryConfiguration[romRamButton0_].dirname);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/"+computerConfiguration.memoryConfiguration[romRamButton1_].filename+"/Dir/GuiRomRam1", computerConfiguration.memoryConfiguration[romRamButton1_].dirname);
    writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Key_File", computerConfiguration.keyFileConfiguration.directory);
    writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Video_Dump_File", computerConfiguration.screenDumpFileConfiguration.directory);
    writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Print_File", computerConfiguration.printerFileConfiguration.directory);
    writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Xmodem_File", computerConfiguration.videoTerminalConfiguration.xmodemDirectory);
    for (int tape=0; tape<2; tape++)
    {
        number.Printf("%d", tape);
        writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Wav_File" + number, computerConfiguration.wavConfiguration[tape].directory);
    }
    for (int fdcType = 0; fdcType<FDCTYPE_MAX; fdcType++)
    {
        type.Printf("%d", fdcType);
        for (int disk=0; disk<4; disk++)
        {
            number.Printf("%d", disk);
            writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/FDC" + number + "_File_" + type, floppyDir_[fdcType][disk]);
        }
    }
}

void GuiXml::writeXmlConfig()
{
    wxString buffer, numberStr, type;

    configPointer->Write("Computer/XmlDirComboSelection", xmlDirComboSelection);
    configPointer->Write("Computer/XmlDirComboString", xmlDirComboString);

    for (size_t number=0; number < dirNameList_.GetCount(); number++)
    {
        if (dirNameListDefaultFile_.GetCount() > 0)
            configPointer->Write("/Computer/XmlFile/"+dirNameList_[number], dirNameListDefaultFile_[number]);
        if (dirNameListGui_.GetCount() > 0)
        {
            configPointer->Write("/Computer/XmlGui/"+dirNameList_[number], dirNameListGui_[number]);
            configPointer->Write("/Computer/XmlDir/"+dirNameListGui_[number], dirNameList_[number]);
        }
    }

    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/GuiRomRam0", computerConfiguration.memoryConfiguration[romRamButton0_].filename);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/GuiRomRam1", computerConfiguration.memoryConfiguration[romRamButton1_].filename);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Led_Update_Frequency", computerConfiguration.ledTime_);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Enable_Turbo_Cassette", computerConfiguration.turbo_);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Enable_Auto_Cassette", computerConfiguration.autoCassetteLoad_);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/UseLoadLocation", computerConfiguration.memAccessConfiguration.useLocation);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/SaveStart", computerConfiguration.memAccessConfiguration.saveStartString);

    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Key_File", computerConfiguration.keyFileConfiguration.fileName);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Video_Dump_File", computerConfiguration.screenDumpFileConfiguration.fileName);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Print_File", computerConfiguration.printerFileConfiguration.fileName);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Xmodem_File", computerConfiguration.videoTerminalConfiguration.xmodemFileName);
    for (int tape=0; tape<2; tape++)
    {
        numberStr.Printf("%d", tape);
        configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Wav_File" + numberStr, computerConfiguration.wavConfiguration[tape].fileName);
    }
    for (int fdcType = 0; fdcType<FDCTYPE_MAX; fdcType++)
    {
        type.Printf("%d", fdcType);
        for (int disk=0; disk<4; disk++)
        {
            numberStr.Printf("%d", disk);
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/FDC" + numberStr + "_File_" + type, floppy_[fdcType][disk]);
        }
    }

    configPointer->Write("Computer/SerialLog", computerConfiguration.videoTerminalConfiguration.serialLog);
    configPointer->Write("Computer/ESCError", computerConfiguration.videoTerminalConfiguration.escError);
    configPointer->Write("Computer/Vt_Zoom", computerConfiguration.videoTerminalConfiguration.zoom);

    configPointer->Write("Computer/Enable_Printer", computerConfiguration.printerOn_);
    configPointer->Write("Computer/Print_Mode", computerConfiguration.printMode_);

    configPointer->Write("Computer/Enable_Vt_Stretch_Dot", computerConfiguration.videoTerminalConfiguration.stretchDot);
    configPointer->Write("Computer/Enable_Vt_External", computerConfiguration.videoTerminalConfiguration.external);
    configPointer->Write("Computer/Enable_Vt_LoopBack", computerConfiguration.videoTerminalConfiguration.loop_back);
    
    configPointer->Write("Computer/Turbo_Clock_Speed", computerConfiguration.turboClock_);
    configPointer->Write("Computer/Enable_Real_Cassette", computerConfiguration.realCassetteLoad_);
    configPointer->Write("Computer/DisableNvRam", computerConfiguration.nvRamConfiguration.disable);
    configPointer->Write("Computer/Ymodem_PacketSize", computerConfiguration.videoTerminalConfiguration.packetSize);
    configPointer->Write("Computer/Volume", computerConfiguration.soundConfiguration.volume);
    configPointer->Write("/Computer/Tempo", computerConfiguration.soundConfiguration.tempo);
}

void GuiXml::readXmlWindowConfig()
{
    computerConfiguration.coinConfiguration.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_CoinVideo_X", computerConfiguration.coinConfiguration.defaultX);
    computerConfiguration.coinConfiguration.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_CoinVideo_Y", computerConfiguration.coinConfiguration.defaultY);
    computerConfiguration.cdp1861Configuration.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Pixie_X", computerConfiguration.cdp1861Configuration.defaultX);
    computerConfiguration.cdp1861Configuration.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Pixie_Y", computerConfiguration.cdp1861Configuration.defaultY);
    computerConfiguration.cdp1864Configuration.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_CDP1864_X", computerConfiguration.cdp1864Configuration.defaultX);
    computerConfiguration.cdp1864Configuration.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_CDP1864_Y", computerConfiguration.cdp1864Configuration.defaultY);
    computerConfiguration.studio4VideoConfiguration.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_ST4_X", computerConfiguration.studio4VideoConfiguration.defaultX);
    computerConfiguration.studio4VideoConfiguration.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_ST4_Y", computerConfiguration.studio4VideoConfiguration.defaultY);
    computerConfiguration.vip2KVideoConfiguration.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_VIP2K_X", computerConfiguration.vip2KVideoConfiguration.defaultX);
    computerConfiguration.vip2KVideoConfiguration.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_VIP2K_Y", computerConfiguration.vip2KVideoConfiguration.defaultY);
    computerConfiguration.fredVideoConfiguration.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_FRED_X", computerConfiguration.fredVideoConfiguration.defaultX);
    computerConfiguration.fredVideoConfiguration.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_FRED_Y", computerConfiguration.fredVideoConfiguration.defaultY);
    computerConfiguration.tmsConfiguration.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Tms_X", computerConfiguration.tmsConfiguration.defaultX);
    computerConfiguration.tmsConfiguration.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Tms_Y", computerConfiguration.tmsConfiguration.defaultY);
    computerConfiguration.mc6845Configuration.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_MC6845_X", computerConfiguration.mc6845Configuration.defaultX);
    computerConfiguration.mc6845Configuration.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_MC6845_Y", computerConfiguration.mc6845Configuration.defaultY);
    computerConfiguration.mc6847Configuration.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_MC6847_X", computerConfiguration.mc6847Configuration.defaultX);
    computerConfiguration.mc6847Configuration.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_MC6847_Y", computerConfiguration.mc6847Configuration.defaultY);
    computerConfiguration.i8275Configuration.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_I8275_X", computerConfiguration.i8275Configuration.defaultX);
    computerConfiguration.i8275Configuration.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_I8275_Y", computerConfiguration.i8275Configuration.defaultY);
    computerConfiguration.vis1870Configuration.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_v1870_X", computerConfiguration.vis1870Configuration.defaultX);
    computerConfiguration.vis1870Configuration.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_v1870_Y", computerConfiguration.vis1870Configuration.defaultY);
    computerConfiguration.sn76430NConfiguration.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_SN76430N_X", computerConfiguration.sn76430NConfiguration.defaultX);
    computerConfiguration.sn76430NConfiguration.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_SN76430N_Y", computerConfiguration.sn76430NConfiguration.defaultY);
    int num=0;
    wxString numStr;
    for (std::vector<Cdp1851Configuration>::iterator cdp1851 = computerConfiguration.cdp1851Configuration.begin (); cdp1851 != computerConfiguration.cdp1851Configuration.end (); ++cdp1851)
    {
        numStr.Printf("%d",num++);
        cdp1851->pos.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Cdp1851_"+numStr+"_X", cdp1851->defaultPos.x);
        cdp1851->pos.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Cdp1851_"+numStr+"_Y", cdp1851->defaultPos.y);
    }
    num=0;
    for (std::vector<Cdp1852Configuration>::iterator cdp1852 = computerConfiguration.cdp1852Configuration.begin (); cdp1852 != computerConfiguration.cdp1852Configuration.end (); ++cdp1852)
    {
        numStr.Printf("%d",num++);
        cdp1852->pos.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Cdp1852_"+numStr+"_X", cdp1852->defaultPos.x);
        cdp1852->pos.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Cdp1852_"+numStr+"_Y", cdp1852->defaultPos.y);
    }
    num=0;
    for (std::vector<FrontPanelConfiguration>::iterator frontpanel = computerConfiguration.frontPanelConfiguration.begin (); frontpanel != computerConfiguration.frontPanelConfiguration.end (); ++frontpanel)
    {
        numStr.Printf("%d",num++);
        frontpanel->pos.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_FrontPanel_"+numStr+"_X", frontpanel->defaultPos.x);
        frontpanel->pos.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_FrontPanel_"+numStr+"_Y", frontpanel->defaultPos.y);
    }
    computerConfiguration.videoTerminalConfiguration.x = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Vt_X", computerConfiguration.videoTerminalConfiguration.defaultX);
    computerConfiguration.videoTerminalConfiguration.y = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Vt_Y", computerConfiguration.videoTerminalConfiguration.defaultY);
}

void GuiXml::writeXmlWindowConfig()
{
    writeCoinWindowConfig();

    if (computerConfiguration.cdp1861Configuration.defined)
    {
        if (computerConfiguration.cdp1861Configuration.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Pixie_X", computerConfiguration.cdp1861Configuration.x);
        if (computerConfiguration.cdp1861Configuration.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Pixie_Y", computerConfiguration.cdp1861Configuration.y);
    }
    if (computerConfiguration.cdp1864Configuration.defined)
    {
        if (computerConfiguration.cdp1864Configuration.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_CDP1864_X", computerConfiguration.cdp1864Configuration.x);
        if (computerConfiguration.cdp1864Configuration.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_CDP1864_Y", computerConfiguration.cdp1864Configuration.y);
    }
    if (computerConfiguration.studio4VideoConfiguration.defined)
    {
        if (computerConfiguration.studio4VideoConfiguration.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_ST4_X", computerConfiguration.studio4VideoConfiguration.x);
        if (computerConfiguration.studio4VideoConfiguration.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_ST4_Y", computerConfiguration.studio4VideoConfiguration.y);
    }
    if (computerConfiguration.vip2KVideoConfiguration.defined)
    {
        if (computerConfiguration.vip2KVideoConfiguration.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_VIP2K_X", computerConfiguration.vip2KVideoConfiguration.x);
        if (computerConfiguration.vip2KVideoConfiguration.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_VIP2K_Y", computerConfiguration.vip2KVideoConfiguration.y);
    }
    if (computerConfiguration.fredVideoConfiguration.defined)
    {
        if (computerConfiguration.fredVideoConfiguration.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_FRED_X", computerConfiguration.fredVideoConfiguration.x);
        if (computerConfiguration.fredVideoConfiguration.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_FRED_Y", computerConfiguration.fredVideoConfiguration.y);
    }
    if (computerConfiguration.tmsConfiguration.defined)
    {
        if (computerConfiguration.tmsConfiguration.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Tms_X", computerConfiguration.tmsConfiguration.x);
        if (computerConfiguration.tmsConfiguration.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Tms_Y", computerConfiguration.tmsConfiguration.y);
    }
    if (computerConfiguration.mc6845Configuration.defined)
    {
        if (computerConfiguration.mc6845Configuration.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_MC6845_X", computerConfiguration.mc6845Configuration.x);
        if (computerConfiguration.mc6845Configuration.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_MC6845_Y", computerConfiguration.mc6845Configuration.y);
    }
    if (computerConfiguration.mc6847Configuration.defined)
    {
        if (computerConfiguration.mc6847Configuration.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_MC6847_X", computerConfiguration.mc6847Configuration.x);
        if (computerConfiguration.mc6847Configuration.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_MC6847_Y", computerConfiguration.mc6847Configuration.y);
    }
    if (computerConfiguration.i8275Configuration.defined)
    {
        if (computerConfiguration.i8275Configuration.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_I8275_X", computerConfiguration.i8275Configuration.x);
        if (computerConfiguration.i8275Configuration.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_I8275_Y", computerConfiguration.i8275Configuration.y);
    }
    if (computerConfiguration.vis1870Configuration.defined)
    {
        if (computerConfiguration.vis1870Configuration.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_v1870_X", computerConfiguration.vis1870Configuration.x);
        if (computerConfiguration.vis1870Configuration.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_v1870_Y", computerConfiguration.vis1870Configuration.y);
    }
    if (computerConfiguration.sn76430NConfiguration.defined)
    {
        if (computerConfiguration.sn76430NConfiguration.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_SN76430N_X", computerConfiguration.sn76430NConfiguration.x);
        if (computerConfiguration.sn76430NConfiguration.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_SN76430N_Y", computerConfiguration.sn76430NConfiguration.y);
    }
    int num=0;
    wxString numStr;
    for (std::vector<Cdp1851Configuration>::iterator cdp1851 = computerConfiguration.cdp1851Configuration.begin (); cdp1851 != computerConfiguration.cdp1851Configuration.end (); ++cdp1851)
    {
        numStr.Printf("%d",num++);
        if (cdp1851->pos.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Cdp1851_"+numStr+"_X", cdp1851->pos.x);
        if (cdp1851->pos.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Cdp1851_"+numStr+"_Y", cdp1851->pos.y);
    }
    num=0;
    for (std::vector<Cdp1852Configuration>::iterator cdp1852 = computerConfiguration.cdp1852Configuration.begin (); cdp1852 != computerConfiguration.cdp1852Configuration.end (); ++cdp1852)
    {
        numStr.Printf("%d",num++);
        if (cdp1852->pos.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Cdp1852_"+numStr+"_X", cdp1852->pos.x);
        if (cdp1852->pos.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Cdp1852_"+numStr+"_Y", cdp1852->pos.y);
    }
    num=0;
    for (std::vector<FrontPanelConfiguration>::iterator frontpanel = computerConfiguration.frontPanelConfiguration.begin (); frontpanel != computerConfiguration.frontPanelConfiguration.end (); ++frontpanel)
    {
        numStr.Printf("%d",num++);
        if (frontpanel->pos.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_FrontPanel_"+numStr+"_X", frontpanel->pos.x);
        if (frontpanel->pos.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_FrontPanel_"+numStr+"_Y", frontpanel->pos.y);
    }
    if (computerConfiguration.videoTerminalConfiguration.type == VT52 || computerConfiguration.videoTerminalConfiguration.type == VT100)
    {
        if (computerConfiguration.videoTerminalConfiguration.x > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Vt_X", computerConfiguration.videoTerminalConfiguration.x);
        if (computerConfiguration.videoTerminalConfiguration.y > 0)
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_Vt_Y", computerConfiguration.videoTerminalConfiguration.y);
    }
}

void GuiXml::writeCoinWindowConfig()
{
    if (!computerConfiguration.coinConfiguration.defined)
        return;

    if (computerConfiguration.coinConfiguration.x > 0)
        configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_CoinVideo_X", computerConfiguration.coinConfiguration.x);
    if (computerConfiguration.coinConfiguration.y > 0)
        configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Window_Position_CoinVideo_Y", computerConfiguration.coinConfiguration.y);
}

void GuiXml::readDefaultVtConfig()
{
    long value = computerConfiguration.videoTerminalConfiguration.vt52DefaultSetUpFeature.to_ulong();
    computerConfiguration.videoTerminalConfiguration.vt52SetUpFeature = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/VT52Setup", value);
    value = computerConfiguration.videoTerminalConfiguration.vt100DefaultSetUpFeature.to_ulong();
    computerConfiguration.videoTerminalConfiguration.vt100SetUpFeature = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/VT100Setup", value);
    value = computerConfiguration.videoTerminalConfiguration.vtExternalDefaultSetUpFeature.to_ulong();
    computerConfiguration.videoTerminalConfiguration.vtExternalSetUpFeature = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/VTExternalSetup", value);
    computerConfiguration.videoTerminalConfiguration.vtLoopBackSetUpFeature = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/VTLoopBackSetup", value);
    computerConfiguration.videoTerminalConfiguration.bellFrequency = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Bell_Frequency", computerConfiguration.videoTerminalConfiguration.defaultBellFrequency);
    computerConfiguration.videoTerminalConfiguration.charactersPerRow = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/VtCharactersPerRow", computerConfiguration.videoTerminalConfiguration.defaultCharactersPerRow);
    computerConfiguration.videoTerminalConfiguration.characterWidth = (int)configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/VtCharacterWidth", computerConfiguration.videoTerminalConfiguration.defaultCharacterWidth);
}

void GuiXml::writeDefaultVtConfig()
{
    long value = computerConfiguration.videoTerminalConfiguration.vt52SetUpFeature.to_ulong();
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/VT52Setup", value);
    value = computerConfiguration.videoTerminalConfiguration.vt100SetUpFeature.to_ulong();
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/VT100Setup", value);
    value = computerConfiguration.videoTerminalConfiguration.vtExternalSetUpFeature.to_ulong();
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/VTExternalSetup", value);
    value = computerConfiguration.videoTerminalConfiguration.vtLoopBackSetUpFeature.to_ulong();
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/VTLoopBackSetup", value);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Bell_Frequency", computerConfiguration.videoTerminalConfiguration.bellFrequency);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/VtCharactersPerRow", computerConfiguration.videoTerminalConfiguration.charactersPerRow);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/VtCharacterWidth", computerConfiguration.videoTerminalConfiguration.characterWidth);
}

bool GuiXml::showSplashScreen()
{
    bool showSplashScreen = false;
    
    configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/ShowSplashScreen", &showSplashScreen, true);
    return showSplashScreen;
}

void GuiXml::hideSplashScreen()
{
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/ShowSplashScreen", false);
}

void GuiXml::onKeyFile(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;

    fileName = wxFileSelector( "Select the Key file to load",
                               computerConfiguration.keyFileConfiguration.directory, XRCCTRL(*this, "KeyFileXml", wxTextCtrl)->GetValue(),
                               "txt",
                               wxString::Format
                              (
                                    "Text File (*.txt)|*.txt|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    computerConfiguration.keyFileConfiguration.directory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    computerConfiguration.keyFileConfiguration.fileName = FullPath.GetFullName();

    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Key_File", computerConfiguration.keyFileConfiguration.fileName);
    writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Key_File", computerConfiguration.keyFileConfiguration.directory);
    XRCCTRL(*this, "KeyFileXml", wxTextCtrl)->SetValue(computerConfiguration.keyFileConfiguration.fileName);
}

void GuiXml::onKeyFileText(wxCommandEvent&WXUNUSED(event))
{
    computerConfiguration.keyFileConfiguration.fileName = XRCCTRL(*this, "KeyFileXml", wxTextCtrl)->GetValue();
    
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Key_File", computerConfiguration.keyFileConfiguration.fileName);
}

void GuiXml::onKeyFileEject(wxCommandEvent& WXUNUSED(event) )
{
    XRCCTRL(*this, "KeyFileXml", wxTextCtrl)->SetValue("");
    computerConfiguration.keyFileConfiguration.fileName = "";

    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Key_File", computerConfiguration.keyFileConfiguration.fileName);

    if (!computerRunning_)
        return;

    p_Computer->closeKeyFile();
}

void GuiXml::onScreenDumpFile(wxCommandEvent& WXUNUSED(event))
{
     wxString fileName;

    fileName = wxFileSelector( "Select the Video Dump Output File",
                               computerConfiguration.screenDumpFileConfiguration.directory, XRCCTRL(*this, "ScreenDumpFileXml", wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                   "Portable Network Graphics Image (*.png)|*.png|JPEG Image (*.jpeg)|*.jpeg|Bitmap Image (*.bmp)|*.bmp|Paintbrush Image (*.pcx)|*.pcx|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );


    if (!fileName)
        return;

    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    computerConfiguration.screenDumpFileConfiguration.directory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    computerConfiguration.screenDumpFileConfiguration.fileName = FullPath.GetFullName();

    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Video_Dump_File", computerConfiguration.screenDumpFileConfiguration.fileName);
    writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Video_Dump_File", computerConfiguration.screenDumpFileConfiguration.directory);
    XRCCTRL(*this, "ScreenDumpFileXml", wxComboBox)->SetValue(computerConfiguration.screenDumpFileConfiguration.fileName);
}

void GuiXml::onScreenDumpFileText(wxCommandEvent& WXUNUSED(event))
{
    computerConfiguration.screenDumpFileConfiguration.fileName = XRCCTRL(*this, "ScreenDumpFileXml", wxComboBox)->GetValue();
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Video_Dump_File", computerConfiguration.screenDumpFileConfiguration.fileName);
}

void GuiXml::onXmodem(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog openFileDialog(this,
                                "Select the XMODEM file(s) to save/load",
                                computerConfiguration.videoTerminalConfiguration.xmodemDirectory,
                                computerConfiguration.videoTerminalConfiguration.xmodemFileName,
                                "All files (*.*)|*.*",
                                wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_MULTIPLE
                               );

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...

    computerConfiguration.videoTerminalConfiguration.terminalPaths.Clear();
    computerConfiguration.videoTerminalConfiguration.terminalFiles.Clear();
    openFileDialog.GetPaths(computerConfiguration.videoTerminalConfiguration.terminalPaths);
    openFileDialog.GetFilenames(computerConfiguration.videoTerminalConfiguration.terminalFiles);

    computerConfiguration.videoTerminalConfiguration.numberOfTerminalFiles = computerConfiguration.videoTerminalConfiguration.terminalPaths.GetCount();
    
    wxFileName FullPath;
    computerConfiguration.videoTerminalConfiguration.xmodemFullFileName = "";
    for (int i=0; i<(int)computerConfiguration.videoTerminalConfiguration.numberOfTerminalFiles; i++)
    {
        FullPath = wxFileName(computerConfiguration.videoTerminalConfiguration.terminalPaths[i], wxPATH_NATIVE);
        if (i == 0)
        {
            computerConfiguration.videoTerminalConfiguration.xmodemDirectory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
            computerConfiguration.videoTerminalConfiguration.xmodemFileName = FullPath.GetFullName();
        }
        computerConfiguration.videoTerminalConfiguration.xmodemFullFileName += computerConfiguration.videoTerminalConfiguration.terminalFiles[i];
        computerConfiguration.videoTerminalConfiguration.xmodemFullFileName += " ";
    }

    writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Xmodem_File", computerConfiguration.videoTerminalConfiguration.xmodemDirectory);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Xmodem_File", computerConfiguration.videoTerminalConfiguration.xmodemFileName);
    if (mode_.gui)
        XRCCTRL(*this, "XmodemFileXml", wxTextCtrl)->ChangeValue(computerConfiguration.videoTerminalConfiguration.xmodemFullFileName);
}

void GuiXml::onXmodemText(wxCommandEvent&event)
{
    computerConfiguration.videoTerminalConfiguration.xmodemFileName = event.GetString();
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Xmodem_File", computerConfiguration.videoTerminalConfiguration.xmodemFileName);
}

void GuiXml::onXmodemEject(wxCommandEvent& WXUNUSED(event) )
{
    computerConfiguration.videoTerminalConfiguration.xmodemFileName = "";
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Xmodem_File", computerConfiguration.videoTerminalConfiguration.xmodemFileName);

    if (mode_.gui)
        XRCCTRL(*this, "XmodemFileXml", wxTextCtrl)->SetValue(computerConfiguration.videoTerminalConfiguration.xmodemFileName);
}

void GuiXml::startAutoTerminalSave(int protocol)
{
    if (!computerRunning_)
        return;

    if (computerConfiguration.autoCassetteLoad_)
        startTerminalSave(protocol);
}

void GuiXml::startTerminalSave(int protocol)
{
    if (terminalSave_ || terminalLoad_)
    {
        stopTerminal();
        p_Computer->terminalStop();
    }

    wxString filePath, fileName;
    
    filePath = computerConfiguration.videoTerminalConfiguration.xmodemDirectory;
    fileName = computerConfiguration.videoTerminalConfiguration.xmodemFileName;
    filePath.operator += (fileName);
    
    if (fileName.Len() == 0)
    {
        fileName = p_Main->eventShowFileSelector( "Select the terminal file to save",
                                                 computerConfiguration.wavConfiguration[0].directory, fileName,
                                                 "",
                                                 "All files (*.*)|*.*",
                                                 wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW
                                                 );
        if (!fileName)
            return;
        
        wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
        computerConfiguration.videoTerminalConfiguration.xmodemFileName = FullPath.GetFullName();
        computerConfiguration.videoTerminalConfiguration.xmodemDirectory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
        
        writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Xmodem_File", computerConfiguration.videoTerminalConfiguration.xmodemDirectory);
        configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Xmodem_File", computerConfiguration.videoTerminalConfiguration.xmodemFileName);
        p_Main->eventSetTextValue("XmodemFileXml", computerConfiguration.videoTerminalConfiguration.xmodemFileName);
        
        filePath = computerConfiguration.videoTerminalConfiguration.xmodemDirectory;
        filePath.operator += (computerConfiguration.videoTerminalConfiguration.xmodemFileName);
    }
    if (wxFile::Exists(filePath))
    {
        p_Main->eventShowMessageBox( fileName+" already exists.\n"+"Do you want to replace it?",
                                    "Confirm Save As", wxICON_EXCLAMATION | wxYES_NO);
        
        if (messageBoxAnswer_ == wxNO)
        {
            fileName = p_Main->eventShowFileSelector( "Select the terminal file to save",
                                                     computerConfiguration.wavConfiguration[0].directory, fileName,
                                                     "",
                                                     "All files (*.*)|*.*",
                                                     wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_OVERWRITE_PROMPT
                                                     );
            if (!fileName)
                return;
            
            wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
            computerConfiguration.videoTerminalConfiguration.xmodemFileName = FullPath.GetFullName();
            computerConfiguration.videoTerminalConfiguration.xmodemDirectory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
            
            writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Xmodem_File", computerConfiguration.videoTerminalConfiguration.xmodemDirectory);
            configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Xmodem_File", computerConfiguration.videoTerminalConfiguration.xmodemFileName);
            p_Main->eventSetTextValue("XmodemFileXml", computerConfiguration.videoTerminalConfiguration.xmodemFileName);
            
            filePath = computerConfiguration.videoTerminalConfiguration.xmodemDirectory;
            filePath.operator += (computerConfiguration.videoTerminalConfiguration.xmodemFileName);
        }
//        if (messageBoxAnswer_ == wxYES)
  //          wxRemoveFile(filePath);
    }
    terminalSave_ = true;

    p_Main->eventSetTapeState(TAPE_RECORD, "");
    p_Computer->terminalSave(filePath, protocol);
}

void GuiXml::onCassette(wxCommandEvent& WXUNUSED(event))
{
    onCassetteFileSelector();
}

void GuiXml::onCassetteFileSelector()
{
    wxString fileName;
    wxString typeStr = "WAV";
    wxString formatStr = "WAV File (*.wav)|*.wav|All files (%s)|%s";
        
    wxString oldFile = computerConfiguration.wavConfiguration[0].fileName;
    wxString oldFileDir = computerConfiguration.wavConfiguration[0].directory;

    fileName = wxFileSelector( "Select the "+typeStr+" file to save/load",
                               computerConfiguration.wavConfiguration[0].directory, computerConfiguration.wavConfiguration[0].fileName,
                               "wav",
                               wxString::Format
                              (
                                   formatStr,
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );


    if (!fileName)
        return;

    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    computerConfiguration.wavConfiguration[0].directory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    computerConfiguration.wavConfiguration[0].fileName = FullPath.GetFullName();

    if (oldFileDir != computerConfiguration.wavConfiguration[0].directory || oldFile != computerConfiguration.wavConfiguration[0].fileName)
    {
        if (computerRunning_)
            p_Computer->resetTape();
    }
    
    writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Wav_File0", computerConfiguration.wavConfiguration[0].directory);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Wav_File0", computerConfiguration.wavConfiguration[0].fileName);
    if (mode_.gui)
        XRCCTRL(*this, "WavFileXml", wxTextCtrl)->SetValue(computerConfiguration.wavConfiguration[0].fileName);
    
    checkWavFileDownload();
}

void GuiXml::onCassette1(wxCommandEvent& WXUNUSED(event))
{
    wxString fileName;
    
    fileName = wxFileSelector( "Select the WAV file to save/load",
                              computerConfiguration.wavConfiguration[1].directory, computerConfiguration.wavConfiguration[1].fileName,
                              "wav",
                              wxString::Format
                              (
                               "WAV File (*.wav)|*.wav|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                               ),
                              wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                              this
                              );
    
    
    if (!fileName)
        return;
    
    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    computerConfiguration.wavConfiguration[1].directory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    computerConfiguration.wavConfiguration[1].fileName = FullPath.GetFullName();
    
    writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Wav_File1", computerConfiguration.wavConfiguration[1].directory);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Wav_File1", computerConfiguration.wavConfiguration[1].fileName);
    if (mode_.gui)
        XRCCTRL(*this, "WavFile1Xml", wxTextCtrl)->SetValue(computerConfiguration.wavConfiguration[1].fileName);
}

void GuiXml::onCassetteEject(wxCommandEvent& WXUNUSED(event) )
{
    if (computerRunning_)
        p_Computer->resetTape();
    
    computerConfiguration.wavConfiguration[0].fileName = "";
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Wav_File0", computerConfiguration.wavConfiguration[0].fileName);
    if (mode_.gui)
        XRCCTRL(*this, "WavFileXml", wxTextCtrl)->SetValue(computerConfiguration.wavConfiguration[0].fileName);
}

void GuiXml::onCassette1Eject(wxCommandEvent& WXUNUSED(event) )
{
    computerConfiguration.wavConfiguration[1].fileName = "";
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Wav_File1", computerConfiguration.wavConfiguration[1].fileName);
    if (mode_.gui)
        XRCCTRL(*this, "WavFile1Xml", wxTextCtrl)->SetValue(computerConfiguration.wavConfiguration[1].fileName);
}

void GuiXml::onCassetteText(wxCommandEvent&event)
{
    wxString newFile = event.GetString();
    
    if (newFile != computerConfiguration.wavConfiguration[0].fileName)
    {
        if (computerRunning_)
            p_Computer->resetTape();

        computerConfiguration.wavConfiguration[0].fileName = newFile;
        configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Wav_File0", computerConfiguration.wavConfiguration[0].fileName);

        checkWavFileDownload();
    }
}

void GuiXml::onCassette1Text(wxCommandEvent&event)
{
    computerConfiguration.wavConfiguration[1].fileName = event.GetString();
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Wav_File1", computerConfiguration.wavConfiguration[1].fileName);
}

void GuiXml::onDisk0(wxCommandEvent& WXUNUSED(event) )
{
    onDisk(0);
}

void GuiXml::onDisk1(wxCommandEvent& WXUNUSED(event) )
{
    onDisk(1);
}

void GuiXml::onDisk2(wxCommandEvent& WXUNUSED(event) )
{
    onDisk(2);
}

void GuiXml::onDisk3(wxCommandEvent& WXUNUSED(event) )
{
    onDisk(3);
}

void GuiXml::onDisk(int diskNumber)
{
    wxString diskNumberString;
    diskNumberString.Printf("%d", diskNumber);

    if (directoryMode_[computerConfiguration.fdcType_][diskNumber])
    {
        wxString dirName = wxDirSelector( "Select the disk "+diskNumberString+" Directory", floppyDirSwitched_[computerConfiguration.fdcType_][diskNumber]);
        if (!dirName)
            return;
        
        floppyDirSwitched_[computerConfiguration.fdcType_][diskNumber] = dirName + pathSeparator_;
        
        if (mode_.gui)
        {
            wxFileName setectedDirFile = wxFileName(floppyDirSwitched_[computerConfiguration.fdcType_][diskNumber]);
            wxArrayString dirArray = setectedDirFile.GetDirs();
            dirName = dirArray.Last();
            XRCCTRL(*this, "FDC"+diskNumberString+"_FileXml", wxTextCtrl)->SetValue(dirName);
        }
    }
    else
    {
        wxString fileName;
        
        fileName = wxFileSelector( "Select the disk "+diskNumberString+" file to load",
                                  floppyDir_[computerConfiguration.fdcType_][diskNumber], floppy_[computerConfiguration.fdcType_][diskNumber],
                                  "img",
                                  wxString::Format
                                  (
                                   "FDC Image (*.img)|*.img|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                                   ),
                                  wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                                  this
                                  );
        if (!fileName)
            return;
        
        wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
        floppyDir_[computerConfiguration.fdcType_][diskNumber] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
        floppy_[computerConfiguration.fdcType_][diskNumber] = FullPath.GetFullName();
        
        wxString type;
        type.Printf("%d", computerConfiguration.fdcType_);
        writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/FDC"+diskNumberString+"_File_" + type, floppyDir_[computerConfiguration.fdcType_][diskNumber]);
        configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/FDC"+diskNumberString+"_File_" + type, floppy_[computerConfiguration.fdcType_][diskNumber]);
        if (mode_.gui)
            XRCCTRL(*this, "FDC"+diskNumberString+"_FileXml", wxTextCtrl)->SetValue(floppy_[computerConfiguration.fdcType_][diskNumber]);
    }
}

void GuiXml::onDiskText0(wxCommandEvent&event)
{
    onDiskText(0, event.GetString());
}

void GuiXml::onDiskText1(wxCommandEvent&event)
{
    onDiskText(1, event.GetString());
}

void GuiXml::onDiskText2(wxCommandEvent&event)
{
    onDiskText(2, event.GetString());
}

void GuiXml::onDiskText3(wxCommandEvent&event)
{
    onDiskText(3, event.GetString());
}

void GuiXml::onDiskText(int diskNumber, wxString eventString)
{
    wxString diskNumberString;
    diskNumberString.Printf("%d", diskNumber);

    if (directoryMode_[computerConfiguration.fdcType_][diskNumber])
    {
        if (p_Computer == NULL)
            return;

        p_Computer->changeDiskName(diskNumber, floppyDirSwitched_[computerConfiguration.fdcType_][diskNumber], "");
        return;
    }
    
    floppy_[computerConfiguration.fdcType_][diskNumber] = eventString;
    wxString type;
    type.Printf("%d", computerConfiguration.fdcType_);
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/FDC"+diskNumberString+"_File_" + type, floppy_[computerConfiguration.fdcType_][diskNumber]);

    if (p_Computer == NULL)
        return;

    if (floppy_[computerConfiguration.fdcType_][diskNumber].Len() == 0)
        p_Computer->changeDiskName(diskNumber, floppyDir_[computerConfiguration.fdcType_][diskNumber], "");
    else
        p_Computer->changeDiskName(diskNumber, floppyDir_[computerConfiguration.fdcType_][diskNumber], floppy_[computerConfiguration.fdcType_][diskNumber]);
}

void GuiXml::onDiskEject0(wxCommandEvent& WXUNUSED(event))
{
    onDiskEject(0);
}

void GuiXml::onDiskEject1(wxCommandEvent& WXUNUSED(event))
{
    onDiskEject(1);
}

void GuiXml::onDiskEject2(wxCommandEvent& WXUNUSED(event))
{
    onDiskEject(2);
}

void GuiXml::onDiskEject3(wxCommandEvent& WXUNUSED(event))
{
    onDiskEject(3);
}

void GuiXml::onDiskEject(int diskNumber)
{
    wxString diskNumberString;
    diskNumberString.Printf("%d", diskNumber);
    wxString type;
    type.Printf("%d", computerConfiguration.fdcType_);

    floppy_[computerConfiguration.fdcType_][diskNumber] = "";
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/FDC"+diskNumberString+"_File_" + type, floppy_[computerConfiguration.fdcType_][diskNumber]);

    if (mode_.gui)
        XRCCTRL(*this, "FDC"+diskNumberString+"_FileXml", wxTextCtrl)->SetValue(floppy_[computerConfiguration.fdcType_][diskNumber]);
}

void GuiXml::onDiskDirSwitch0(wxCommandEvent&WXUNUSED(event))
{
    onDiskDirSwitch(0);
}

void GuiXml::onDiskDirSwitch1(wxCommandEvent&WXUNUSED(event))
{
    onDiskDirSwitch(1);
}

void GuiXml::onDiskDirSwitch2(wxCommandEvent&WXUNUSED(event))
{
    onDiskDirSwitch(2);
}

void GuiXml::onDiskDirSwitch3(wxCommandEvent&WXUNUSED(event))
{
    onDiskDirSwitch(3);
}

void GuiXml::onDiskDirSwitch(int diskNumber)
{
    directoryMode_[computerConfiguration.fdcType_][diskNumber] = !directoryMode_[computerConfiguration.fdcType_][diskNumber];
    setUpdFloppyGui(diskNumber);
}

void GuiXml::onXmlControlWindows(wxCommandEvent&event)
{
    computerConfiguration.frontPanelConfiguration[PANEL_MAIN].show = event.IsChecked();

    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddressXml",wxTextCtrl)->Enable(computerConfiguration.frontPanelConfiguration[PANEL_MAIN].show);
        XRCCTRL(*this,"AddressText1Xml",wxStaticText)->Enable(computerConfiguration.frontPanelConfiguration[PANEL_MAIN].show);
        XRCCTRL(*this,"AddressText2Xml",wxStaticText)->Enable(computerConfiguration.frontPanelConfiguration[PANEL_MAIN].show);
    }
    
    if (computerRunning_)
        p_Computer->showPanel();
}

void GuiXml::onLedTimerXml(wxCommandEvent&event)
{
    wxString stringMs = event.GetString();
    if (stringMs == "")  stringMs = "0";
    long ms;
    if (!stringMs.ToLong(&ms, 10))
        return;
    
    computerConfiguration.ledTime_ = stringMs;
    computerConfiguration.ledTimeMs_ = ms;
    
    if (computerRunning_ && (selectedTab_ == XMLTAB))
        p_Computer->setLedMs(ms);

    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Led_Update_Frequency", computerConfiguration.ledTime_);
    configPointer->Flush();
}

void GuiXml::onAutoLoadXml(wxCommandEvent&event)
{
    computerConfiguration.autoCassetteLoad_ = event.IsChecked();
    if (computerRunning_ && (selectedTab_ == XMLTAB))
    {
        XRCCTRL(*this, "CasLoadXml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
        XRCCTRL(*this, "CasSaveXml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_ && !computerConfiguration.videoTerminalConfiguration.hexModem_defined);
        if (computerConfiguration.swTapeConfiguration.twoDecks || computerConfiguration.hwTapeConfiguration.twoDecks)
        {
            XRCCTRL(*this, "CasLoad1Xml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
            XRCCTRL(*this, "CasSave1Xml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
        }
    }

    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Enable_Auto_Cassette", computerConfiguration.autoCassetteLoad_);
    configPointer->Flush();
}

void GuiXml::onTurboXml(wxCommandEvent&event)
{
    if (computerRunning_ && turboOn_)
        turboOff();
    
    computerConfiguration.turbo_ = event.IsChecked();
    turboGui();

    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Enable_Turbo_Cassette", computerConfiguration.turbo_);
    configPointer->Flush();
}

void GuiXml::onUseLocationXml(wxCommandEvent&event)
{
    computerConfiguration.memAccessConfiguration.useLocation = event.IsChecked();
    enableLocationGui();

    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/UseLoadLocation", computerConfiguration.memAccessConfiguration.useLocation);
    configPointer->Flush();
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

    if (computerConfiguration.memoryConfiguration[romRamButton].type == RAM)
        romRam = "RAM";
    
    if (computerConfiguration.memoryConfiguration[romRamButton].type == NVRAM)
        romRam = "NVR";

    fileName = wxFileSelector( "Select the " + romRam + " file to load",
                              computerConfiguration.memoryConfiguration[romRamButton].dirname, XRCCTRL(*this, "RomRam"+romRamButtonString+"Xml", wxComboBox)->GetValue(),
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
    computerConfiguration.memoryConfiguration[romRamButton].dirname = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    computerConfiguration.memoryConfiguration[romRamButton].filename = FullPath.GetFullName();

    XRCCTRL(*this, "RomRam"+romRamButtonString+"Xml", wxComboBox)->SetValue(computerConfiguration.memoryConfiguration[romRamButton].filename);

    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/GuiRomRam"+romRamButtonString, computerConfiguration.memoryConfiguration[romRamButton].filename);
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

    computerConfiguration.memoryConfiguration[romRamButton].filename = XRCCTRL(*this, "RomRam"+romRamButtonString+"Xml", wxComboBox)->GetValue();

    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/GuiRomRam"+romRamButtonString, computerConfiguration.memoryConfiguration[romRamButton].filename);
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
    computerConfiguration.memoryConfiguration[romRamButton].dirname = romRamDirNameListGui_[romRamButton][romRamFileComboSelection[romRamButton]];
    computerConfiguration.memoryConfiguration[romRamButton].filename = XRCCTRL(*this, "RomRam"+romRamButtonString+"Xml", wxComboBox)->GetValue();

    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/GuiRomRam"+romRamButtonString, computerConfiguration.memoryConfiguration[romRamButton].filename);
    configPointer->Flush();
}

void GuiXml::onMainXmlXml(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;

    wxString dirName = wxDirSelector( "Select main XML folder", computerConfiguration.xmlFileConfiguration.mainDirectory);
    if (!dirName)
        return;
    
    computerConfiguration.xmlFileConfiguration.mainDirectory = dirName + pathSeparator_;
    
    setXmlDirDropDown();
    computerConfiguration.xmlFileConfiguration.directory = computerConfiguration.xmlFileConfiguration.mainDirectory + computerConfiguration.xmlFileConfiguration.subDirectory + pathSeparator_;
    setXmlDropDown();

    dropdownUpdateOngoing_ = false;
}

void GuiXml::onMainXmlTextXml(wxCommandEvent& WXUNUSED(event))
{
    if (dropdownUpdateOngoing_)
        return;

    computerConfiguration.xmlFileConfiguration.fileName = XRCCTRL(*this, "MainXmlXml", wxComboBox)->GetValue();
    
    setXmlDirFileGui();
    setRomRamDropDown(romRamButton0_, "0");
    setRomRamDropDown(romRamButton1_, "1");
}

void GuiXml::onMainXmlComboXml(wxCommandEvent& event)
{
    xmlFileComboSelection = event.GetSelection();

    computerConfiguration.xmlFileConfiguration.fileName = event.GetString();
    if (dirNameListDefaultFile_.GetCount() > 0)
        dirNameListDefaultFile_[xmlDirComboSelection] = computerConfiguration.xmlFileConfiguration.fileName;

    setXmlDirFileGui();
    setRomRamDropDown(romRamButton0_, "0");
    setRomRamDropDown(romRamButton1_, "1");
}

void GuiXml::onMainDirComboXml(wxCommandEvent& event)
{
    xmlDirComboSelection = event.GetSelection();
    xmlDirComboString = dirNameList_[xmlDirComboSelection];
    
    if (dirNameList_.GetCount() > 0)
        computerConfiguration.xmlFileConfiguration.subDirectory = dirNameList_[xmlDirComboSelection];
    computerConfiguration.xmlFileConfiguration.directory = computerConfiguration.xmlFileConfiguration.mainDirectory + computerConfiguration.xmlFileConfiguration.subDirectory + pathSeparator_;
 
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
    
    parseXmlFile(computerConfiguration.xmlFileConfiguration.directory, computerConfiguration.xmlFileConfiguration.fileName);
    if (computerConfiguration.xmlFileConfiguration.fileName != "")
        if (dirNameListGui_.GetCount() > 0)
            dirNameListGui_[xmlDirComboSelection] = computerInfo.name;
    setXmlGui();

    checkWavFileDownload(true);
}

void GuiXml::setXmlDropDown()
{
    dropdownUpdateOngoing_ = true;

    if (mode_.gui)
        XRCCTRL(*this, "MainXmlXml", wxComboBox)->Clear();
    computerConfiguration.xmlFileConfiguration.fileName = "";
    xmlFileComboSelection = 0;
    
    wxArrayString fileNameList;
    wxString fileName;
    fileNameList.Clear();
    
    wxDir *dir;
    dir = new wxDir (computerConfiguration.xmlFileConfiguration.directory);
    
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
                computerConfiguration.xmlFileConfiguration.fileName = dirNameListDefaultFile_[xmlDirComboSelection];
                xmlFileComboSelection =  number;
            }
        }
    }
    
    if (mode_.gui)
        XRCCTRL(*this, "MainXmlXml", wxComboBox)->SetValue(computerConfiguration.xmlFileConfiguration.fileName);

    dropdownUpdateOngoing_ = false;
    delete dir;
}

void GuiXml::setRomRamDropDown(int romRamButton, wxString romRamButtonString)
{
    if (computerConfiguration.memoryConfiguration[romRamButton].pulldownDir.Count() == 0)
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
    dirNameList.Add(computerConfiguration.memoryConfiguration[romRamButton].pulldownDir[0]);
    romRamDirNameListGui_[romRamButton].Add(computerConfiguration.memoryConfiguration[romRamButton].pulldownMask[0]);

    for (size_t number=0; number < computerConfiguration.memoryConfiguration[romRamButton].pulldownDir.GetCount(); number++)
    {
        if (!dir->Open(computerConfiguration.memoryConfiguration[romRamButton].pulldownDir[number]))
        {
            dropdownUpdateOngoing_ = false;
            delete dir;
            return;
        }
        dirFound = dir->GetFirst(&fileName, computerConfiguration.memoryConfiguration[romRamButton].pulldownMask[number], wxDIR_FILES);
        while (dirFound)
        {
            if ((computerConfiguration.memoryConfiguration[romRamButton].pulldownExclude[number] == "" || fileName.Find(computerConfiguration.memoryConfiguration[romRamButton].pulldownExclude[number]) == wxNOT_FOUND) && (computerConfiguration.memoryConfiguration[romRamButton].pulldownExclude2[number] == "" || fileName.Find(computerConfiguration.memoryConfiguration[romRamButton].pulldownExclude2[number]) == wxNOT_FOUND))
            {
                fileNameList.Add(fileName);
                romRamFileNameListGui_[romRamButton].Add(fileName);
                fileName.LowerCase();
                lowerCaseFileNameList.Add(fileName);
                dirNameList.Add(computerConfiguration.memoryConfiguration[romRamButton].pulldownDir[number]);
                romRamDirNameListGui_[romRamButton].Add(computerConfiguration.memoryConfiguration[romRamButton].pulldownMask[number]);
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
    dropdownUpdateOngoing_ = true;
    if (mode_.gui)
        XRCCTRL(*this, "MainDirXml", wxChoice)->Clear();

    wxString dirName, dummyFile;
    dirNameList_.Clear();
    dirNameListDefaultFile_.Clear();
    dirNameListGui_.Clear();

    wxDir *dir;
    dir = new wxDir (computerConfiguration.xmlFileConfiguration.mainDirectory);
    
    bool dirFound = dir->GetFirst(&dirName, wxEmptyString, wxDIR_DIRS);
    while (dirFound)
    {
        size_t number=0;
        while (defaultComputerList_[number] != "" && defaultComputerList_[number] != dirName)
            number += 3;
        
        if (defaultComputerList_[number] != "")
            dirNameListGui_.Add(configPointer->Read("Computer/XmlGui/"+dirName, defaultComputerList_[number+1]));
        else
            dirNameListGui_.Add(configPointer->Read("Computer/XmlGui/"+dirName, dirName));

        dirNameList_.Add("");
        dirNameListDefaultFile_.Add("");
        dirFound = dir->GetNext(&dirName);
    }
                                
    dirNameListGui_.Sort();

    for (size_t number=0; number < dirNameListGui_.GetCount(); number++)
    {
        int defaultNumber=1;
        while (defaultComputerList_[defaultNumber] != "" && defaultComputerList_[defaultNumber] != dirNameListGui_[number])
            defaultNumber += 3;
        
        if (defaultComputerList_[defaultNumber] != "")
            dirNameList_[number] = configPointer->Read("/Computer/XmlDir/"+dirNameListGui_[number], defaultComputerList_[defaultNumber-1]);
        else
            dirNameList_[number] = configPointer->Read("/Computer/XmlDir/"+dirNameListGui_[number], dirNameListGui_[number]);

        dirNameListDefaultFile_[number] = configPointer->Read("/Computer/XmlFile/"+dirNameList_[number], defaultComputerList_[defaultNumber+1]);

        if (mode_.gui)
            XRCCTRL(*this, "MainDirXml", wxChoice)->Append(dirNameListGui_[number]);
    }
    
    if (dirNameList_.GetCount() > 0)
        computerConfiguration.xmlFileConfiguration.subDirectory = dirNameList_[xmlDirComboSelection];
    else
        computerConfiguration.xmlFileConfiguration.subDirectory = "";
    
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
        computerConfiguration.xmlFileConfiguration.subDirectory = "";
        return;
    }

    xmlDirComboSelection = 0;
    while (dirNameList_[xmlDirComboSelection] != directory && xmlDirComboSelection < (dirNameList_.GetCount()-1))
        xmlDirComboSelection++;

    if (dirNameList_[xmlDirComboSelection] != directory)
        xmlDirComboSelection = 0;
    
    computerConfiguration.xmlFileConfiguration.subDirectory = dirNameList_[xmlDirComboSelection];

    if (mode_.gui)
        XRCCTRL(*this, "MainDirXml", wxChoice)->SetSelection((int)xmlDirComboSelection);
}

void GuiXml::setPrintModeXml()
{
    if (computerConfiguration.printerOn_)
    {
        XRCCTRL(*this, "PrintModeXml", wxChoice)->Enable(true);
        if (computerRunning_ && computerConfiguration.printMode_ != PRINTFILE )
            XRCCTRL(*this, "PrintButtonXml", wxBitmapButton)->Enable(true);
        else
            XRCCTRL(*this, "PrintButtonXml", wxBitmapButton)->Enable(false);
    }
    else
    {
        XRCCTRL(*this, "PrintModeXml", wxChoice)->Enable(false);
        XRCCTRL(*this, "PrintButtonXml", wxBitmapButton)->Enable(false);
    }
    if (computerConfiguration.printMode_ != PRINTFILE || !computerConfiguration.printerOn_)
    {
        XRCCTRL(*this, "PrintFileButtonXml", wxButton)->Enable(false);
        XRCCTRL(*this, "PrintFileXml", wxTextCtrl)->Enable(false);
    }
    else
    {
        XRCCTRL(*this, "PrintFileButtonXml", wxButton)->Enable(true);
        XRCCTRL(*this, "PrintFileXml", wxTextCtrl)->Enable(true);
    }
}

void GuiXml::setRomRamButtonOrder()
{
    romRamButton0_ = 0;
    romRamButton1_ = 1;

    bool romRamButtonEnable0 = (computerConfiguration.memoryConfiguration[romRamButton0_].type & 0xff) != UNDEFINED;
    bool romRamButtonEnable1 = (computerConfiguration.memoryConfiguration[romRamButton1_].type & 0xff) != UNDEFINED;

    if (romRamButtonEnable0 && romRamButtonEnable1)
    {
        if (computerConfiguration.memoryConfiguration[romRamButton0_].start > computerConfiguration.memoryConfiguration[romRamButton1_].start)
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
    readDefaultVtConfig();

    setVtType(computerConfiguration.videoTerminalConfiguration.type, false);
    
    setRomRamButtonOrder();
    if (isXmlRomRamOptionGui())
    {
        computerConfiguration.memoryConfiguration[romRamButton0_].dirname = readConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/"+computerConfiguration.memoryConfiguration[romRamButton0_].filename+"/Dir/GuiRomRam0", computerConfiguration.memoryConfiguration[romRamButton0_].dirname);
        computerConfiguration.memoryConfiguration[romRamButton1_].dirname = readConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/"+computerConfiguration.memoryConfiguration[romRamButton1_].filename+"/Dir/GuiRomRam1", computerConfiguration.memoryConfiguration[romRamButton1_].dirname);
        computerConfiguration.memoryConfiguration[romRamButton0_].filename = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/GuiRomRam0", computerConfiguration.memoryConfiguration[romRamButton0_].filename);
        computerConfiguration.memoryConfiguration[romRamButton1_].filename = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/GuiRomRam1", computerConfiguration.memoryConfiguration[romRamButton1_].filename);
    }
    computerConfiguration.ledTime_ = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Led_Update_Frequency", computerConfiguration.ledTime_);
    configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Enable_Turbo_Cassette", &computerConfiguration.turbo_, computerConfiguration.turbo_);
    configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Enable_Auto_Cassette", &computerConfiguration.autoCassetteLoad_, computerConfiguration.autoCassetteLoad_);
    configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/UseLoadLocation", &computerConfiguration.memAccessConfiguration.useLocation, computerConfiguration.memAccessConfiguration.useLocation);
    computerConfiguration.memAccessConfiguration.saveStartString = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/SaveStart", computerConfiguration.memAccessConfiguration.saveStartString);
    
    computerConfiguration.keyFileConfiguration.fileName = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Key_File", computerConfiguration.keyFileConfiguration.fileName);
    computerConfiguration.keyFileConfiguration.directory = readConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Key_File", computerConfiguration.keyFileConfiguration.directory);
    computerConfiguration.printerFileConfiguration.fileName = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Print_File", computerConfiguration.printerFileConfiguration.fileName);
    computerConfiguration.printerFileConfiguration.directory = readConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Print_File", computerConfiguration.printerFileConfiguration.directory);
    computerConfiguration.screenDumpFileConfiguration.fileName = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Video_Dump_File", computerConfiguration.screenDumpFileConfiguration.fileName);
    computerConfiguration.screenDumpFileConfiguration.directory = readConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Video_Dump_File", computerConfiguration.screenDumpFileConfiguration.directory);
    computerConfiguration.videoTerminalConfiguration.xmodemFileName = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Xmodem_File", computerConfiguration.videoTerminalConfiguration.xmodemFileName);
    computerConfiguration.videoTerminalConfiguration.xmodemDirectory = readConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Xmodem_File", computerConfiguration.videoTerminalConfiguration.xmodemDirectory);

    wxString number, type;
    for (int tape=0; tape<2; tape++)
    {
        number.Printf("%d", tape);
        
        computerConfiguration.wavConfiguration[tape].fileName = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Wav_File" + number, computerConfiguration.wavConfiguration[tape].fileName);
        computerConfiguration.wavConfiguration[tape].directory = readConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Wav_File" + number, computerConfiguration.wavConfiguration[tape].directory);
    }
    for (int fdcType = 0; fdcType<FDCTYPE_MAX; fdcType++)
    {
        type.Printf("%d", fdcType);
        
        for (int disk=0; disk<4; disk++)
        {
            number.Printf("%d", disk);

            floppy_[fdcType][disk] = configPointer->Read(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/FDC" + number + "_File_" + type, floppy_[fdcType][disk]);
            floppyDir_[fdcType][disk] = readConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/FDC" + number + "_File_" + type, floppyDir_[fdcType][disk]);
        }
    }

    long value;
    if (!computerConfiguration.memAccessConfiguration.saveStartString.ToLong(&value, 16))
        value = 0;
    computerConfiguration.memAccessConfiguration.saveStart = value;

    if (!mode_.gui)
        return;
  
    XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->SetValue(computerConfiguration.autoCassetteLoad_);
    XRCCTRL(*this, "TurboXml", wxCheckBox)->SetValue(computerConfiguration.turbo_);

    XRCCTRL(*this, "ShowAddressXml", wxTextCtrl)->ChangeValue(computerConfiguration.ledTime_);

    bool ramButtonEnable = (computerConfiguration.memoryConfiguration[romRamButton1_].type & 0xff) != UNDEFINED;
    wxString buttonText = "....";
    if (ramButtonEnable)
    {
        if ((computerConfiguration.memoryConfiguration[romRamButton1_].type & 0xff) == RAM)
            buttonText.Printf("RAM @%04X", computerConfiguration.memoryConfiguration[romRamButton1_].start);
        if ((computerConfiguration.memoryConfiguration[romRamButton1_].type & 0xff) == ROM)
            buttonText.Printf("ROM @%04X", computerConfiguration.memoryConfiguration[romRamButton1_].start);
        if ((computerConfiguration.memoryConfiguration[romRamButton1_].type & 0xff) == NVRAM)
            buttonText.Printf("NVR @%04X", computerConfiguration.memoryConfiguration[romRamButton1_].start);
    }
    XRCCTRL(*this,"RomRamButton1Xml", wxButton)->SetLabel(buttonText);
    XRCCTRL(*this,"RomRam1Xml", wxComboBox)->Enable(ramButtonEnable);
    XRCCTRL(*this,"RomRamButton1Xml", wxButton)->Enable(ramButtonEnable);
        
    bool romButtonEnable = (computerConfiguration.memoryConfiguration[romRamButton0_].type & 0xff) != UNDEFINED;
    buttonText = "....";
    if (romButtonEnable)
    {
        if ((computerConfiguration.memoryConfiguration[romRamButton0_].type & 0xff) == RAM)
            buttonText.Printf("RAM @%04X", computerConfiguration.memoryConfiguration[romRamButton0_].start);
        if ((computerConfiguration.memoryConfiguration[romRamButton0_].type & 0xff) == ROM)
            buttonText.Printf("ROM @%04X", computerConfiguration.memoryConfiguration[romRamButton0_].start);
        if ((computerConfiguration.memoryConfiguration[romRamButton0_].type & 0xff) == NVRAM)
            buttonText.Printf("NVR @%04X", computerConfiguration.memoryConfiguration[romRamButton0_].start);
    }
    XRCCTRL(*this,"RomRamButton0Xml", wxButton)->SetLabel(buttonText);
    XRCCTRL(*this,"RomRam0Xml", wxComboBox)->Enable(romButtonEnable);
    XRCCTRL(*this,"RomRamButton0Xml", wxButton)->Enable(romButtonEnable);

    XRCCTRL(*this, "RomRam1Xml", wxComboBox)->SetValue(computerConfiguration.memoryConfiguration[romRamButton1_].filename);
    XRCCTRL(*this, "RomRam0Xml", wxComboBox)->SetValue(computerConfiguration.memoryConfiguration[romRamButton0_].filename);
    
    XRCCTRL(*this, "WavFileXml", wxTextCtrl)->SetValue(computerConfiguration.wavConfiguration[0].fileName);
    XRCCTRL(*this, "WavFile1Xml", wxTextCtrl)->SetValue(computerConfiguration.wavConfiguration[1].fileName);
    XRCCTRL(*this, "XmodemFileXml", wxTextCtrl)->SetValue(computerConfiguration.videoTerminalConfiguration.xmodemFileName);

    XRCCTRL(*this, "CasButtonXml", wxButton)->Enable(computerConfiguration.swTapeConfiguration.defined || computerConfiguration.hwTapeConfiguration.defined);
    XRCCTRL(*this, "WavFileXml", wxTextCtrl)->Enable(computerConfiguration.swTapeConfiguration.defined || computerConfiguration.hwTapeConfiguration.defined);
    XRCCTRL(*this, "EjectCasXml", wxBitmapButton)->Enable(computerConfiguration.swTapeConfiguration.defined || computerConfiguration.hwTapeConfiguration.defined);

    XRCCTRL(*this, "CasButton1Xml", wxButton)->Enable(computerConfiguration.swTapeConfiguration.twoDecks || computerConfiguration.hwTapeConfiguration.twoDecks);
    XRCCTRL(*this, "WavFile1Xml", wxTextCtrl)->Enable(computerConfiguration.swTapeConfiguration.twoDecks || computerConfiguration.hwTapeConfiguration.twoDecks);
    XRCCTRL(*this, "EjectCas1Xml", wxBitmapButton)->Enable(computerConfiguration.swTapeConfiguration.twoDecks || computerConfiguration.hwTapeConfiguration.twoDecks);

    XRCCTRL(*this, "XmodemButtonXml", wxButton)->Enable(computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined);
    XRCCTRL(*this, "XmodemFileXml", wxTextCtrl)->Enable(computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined);
    XRCCTRL(*this, "EjectXmodemXml", wxBitmapButton)->Enable(computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined);

    XRCCTRL(*this, "PrintFileXml", wxTextCtrl)->ChangeValue(computerConfiguration.printerFileConfiguration.fileName);
    XRCCTRL(*this, "KeyFileXml", wxTextCtrl)->ChangeValue(computerConfiguration.keyFileConfiguration.fileName);
    XRCCTRL(*this, "ScreenDumpFileXml", wxComboBox)->ChangeValue(computerConfiguration.screenDumpFileConfiguration.fileName);

    if (computerConfiguration.videoTerminalConfiguration.hexModem_defined)
        XRCCTRL(*this, "XmodemButtonXml", wxButton)->SetLabel("HEX");
    else
        XRCCTRL(*this, "XmodemButtonXml", wxButton)->SetLabel("XMODEM");
    
    for (int drive=0; drive <4; drive++)
        setUpdFloppyGui(drive);

    setBaudChoice();
    setPrintModeXml();
    
    XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->Enable((computerConfiguration.swTapeConfiguration.defined || computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined || isTapeHwFred()) && !computerConfiguration.realCassetteLoad_ && !isTapeHwCybervision());
    XRCCTRL(*this, "TurboXml", wxCheckBox)->Enable((computerConfiguration.swTapeConfiguration.defined || computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined || computerConfiguration.hwTapeConfiguration.defined) && !computerConfiguration.realCassetteLoad_);
    XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->Enable((computerConfiguration.swTapeConfiguration.defined || computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined || computerConfiguration.hwTapeConfiguration.defined) && computerConfiguration.turbo_);
    XRCCTRL(*this, "TurboMhzTextXml", wxStaticText)->Enable((computerConfiguration.swTapeConfiguration.defined || computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined || computerConfiguration.hwTapeConfiguration.defined) && computerConfiguration.turbo_);
#if defined(__WXMSW__)
    XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->Enable(computerConfiguration.swTapeConfiguration.defined && !computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined || computerConfiguration.hwTapeConfiguration.defined);
#endif

    XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->Enable(computerConfiguration.hwTapeConfiguration.defined);
    if (hwTapeState_ == HW_TAPE_STATE_FF)
        XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardDarkGreenBitmap);
    else
        XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
    if (hwTapeState_ == HW_TAPE_STATE_RW)
        XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindDarkGreenBitmap);
    else
        XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
    if (computerConfiguration.hwTapeConfiguration.defined)
    {
        XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->Enable(true);
        if (hwTapeState_ == HW_TAPE_STATE_PLAY)
            XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playDarkGreenBitmap);
        else
            XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
    }
    if (computerConfiguration.hwTapeConfiguration.defined)
    {
        XRCCTRL(*this, "CasLoadXml", wxButton)->Enable(false);
        XRCCTRL(*this, "CasForwardXml", wxButton)->Enable(false);
        XRCCTRL(*this, "CasRewindXml", wxButton)->Enable(false);
    }

    XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->SetSelection(computerConfiguration.videoTerminalConfiguration.baudR);
    XRCCTRL(*this, "VTBaudTChoiceXml", wxChoice)->SetSelection(computerConfiguration.videoTerminalConfiguration.baudT);
    if (computerConfiguration.videoTerminalConfiguration.external)
        XRCCTRL(*this, "VTTypeXml", wxChoice)->SetSelection(EXTERNAL_TERMINAL);
    else
    {
        if (computerConfiguration.videoTerminalConfiguration.loop_back)
            XRCCTRL(*this, "VTTypeXml", wxChoice)->SetSelection(LOOP_BACK);
        else
            XRCCTRL(*this, "VTTypeXml", wxChoice)->SetSelection(computerConfiguration.videoTerminalConfiguration.type);
    }

    XRCCTRL(*this, "ControlWindowsXml", wxCheckBox)->SetValue(computerConfiguration.frontPanelConfiguration[PANEL_MAIN].show);
    XRCCTRL(*this,"AddressText1Xml", wxStaticText)->Enable((computerConfiguration.frontPanelConfiguration[PANEL_MAIN].show && computerConfiguration.frontPanelConfiguration[PANEL_MAIN].defined) || computerConfiguration.vis1870Configuration.statusBarType == STATUSBAR_VIP2);
    XRCCTRL(*this,"AddressText2Xml", wxStaticText)->Enable((computerConfiguration.frontPanelConfiguration[PANEL_MAIN].show && computerConfiguration.frontPanelConfiguration[PANEL_MAIN].defined) || computerConfiguration.vis1870Configuration.statusBarType == STATUSBAR_VIP2);
    XRCCTRL(*this,"ShowAddressXml", wxTextCtrl)->Enable((computerConfiguration.frontPanelConfiguration[PANEL_MAIN].show && computerConfiguration.frontPanelConfiguration[PANEL_MAIN].defined) || computerConfiguration.vis1870Configuration.statusBarType == STATUSBAR_VIP2);

    XRCCTRL(*this,"ControlWindowsXml", wxCheckBox)->Enable(computerConfiguration.frontPanelConfiguration[PANEL_MAIN].defined);

    XRCCTRL(*this, "InterlaceXml", wxCheckBox)->SetValue(computerConfiguration.interlace_);
    XRCCTRL(*this, "InterlaceXml", wxCheckBox)->Enable(computerConfiguration.videoTerminalConfiguration.defined || computerConfiguration.fredVideoConfiguration.defined || computerConfiguration.mc6845Configuration.defined || computerConfiguration.i8275Configuration.defined);

    if (clockTextCtrl != NULL)
        clockTextCtrl->ChangeValue(computerConfiguration.clock_);

    computerConfiguration.videoNumber_ = VIDEOMAIN;
    
    if (computerConfiguration.numberOfVideoTypes_ > 0)
    {
        XRCCTRL(*this, "ZoomTextXml", wxStaticText)->SetLabel(computerConfiguration.videoName_[computerConfiguration.videoNumber_] + ", zoom:");
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
    
    if (computerConfiguration.numberOfVideoTypes_ > 1)
        XRCCTRL(*this, "VideoNumberXml", wxBitmapButton)->Enable(true);
    else
        XRCCTRL(*this, "VideoNumberXml", wxBitmapButton)->Enable(false);
    
    if (computerConfiguration.numberOfVideoTypes_ > 0)
        correctZoomAndValue(SET_SPIN, computerConfiguration.videoNumber_);
    
    XRCCTRL(*this, "BatchButtonXml", wxButton)->Enable(computerConfiguration.useBatchWav_);
    XRCCTRL(*this, "BatchFileXml", wxStaticText)->Enable(computerConfiguration.useBatchWav_);

    XRCCTRL(*this, "TextClearXml", wxStaticText)->Enable(computerConfiguration.rtcDs12887Configuration.defined || computerConfiguration.nvRamConfiguration.defined);

    XRCCTRL(*this, "XmlClearRtc", wxCheckBox)->Enable(computerConfiguration.rtcDs12887Configuration.defined);
    XRCCTRL(*this, "XmlClearRtc", wxCheckBox)->SetValue(computerConfiguration.clearRtc);

    XRCCTRL(*this, "XmlClearRam", wxCheckBox)->Enable(computerConfiguration.nvRamConfiguration.defined);
    XRCCTRL(*this, "XmlClearRam", wxCheckBox)->SetValue(computerConfiguration.clearRam);

    XRCCTRL(*this, "UseLocationXml", wxCheckBox)->SetValue(computerConfiguration.memAccessConfiguration.useLocation);
    
    if (computerConfiguration.memAccessConfiguration.saveStart != 0)
        XRCCTRL(*this, "SaveStartXml", wxTextCtrl)->SetValue(computerConfiguration.memAccessConfiguration.saveStartString);
    else
    {
        if (computerConfiguration.memAccessConfiguration.useLocation)
            XRCCTRL(*this, "SaveStartXml", wxTextCtrl)->SetValue("0000");
        else
            XRCCTRL(*this, "SaveStartXml", wxTextCtrl)->SetValue("");
    }

    XRCCTRL(*this,"DebugSCRT", wxCheckBox)->SetValue(computerConfiguration.debuggerConfiguration.mode);

    XRCCTRL(*this,"DebugSCRT",wxCheckBox)->Enable(true);
    if (computerConfiguration.debuggerConfiguration.mode)
    {
        XRCCTRL(*this,"DebugCallText",wxStaticText)->Enable(true);
        XRCCTRL(*this,"DebugCallReg",wxTextCtrl)->Enable(true);
        XRCCTRL(*this,"DebugCallAddress",wxTextCtrl)->Enable(true);
        XRCCTRL(*this,"DebugRetText",wxStaticText)->Enable(true);
        XRCCTRL(*this,"DebugRetReg",wxTextCtrl)->Enable(true);
        XRCCTRL(*this,"DebugRetAddress",wxTextCtrl)->Enable(true);
    }
    
    wxString valueString;
    
    if (computerConfiguration.debuggerConfiguration.callRegister == -1)
        valueString = "";
    else
        valueString.Printf("%01X", (int)computerConfiguration.debuggerConfiguration.callRegister);
    XRCCTRL(*this,"DebugCallReg",wxTextCtrl)->ChangeValue(valueString);
    
    if (computerConfiguration.debuggerConfiguration.callAddress == -1)
        valueString = "";
    else
        valueString.Printf("%04X", (int)computerConfiguration.debuggerConfiguration.callAddress);
    XRCCTRL(*this,"DebugCallAddress",wxTextCtrl)->ChangeValue(valueString);
    
    if (computerConfiguration.debuggerConfiguration.returnRegister == -1)
        valueString = "";
    else
        valueString.Printf("%01X", (int)computerConfiguration.debuggerConfiguration.returnRegister);
    
    XRCCTRL(*this,"DebugRetReg",wxTextCtrl)->ChangeValue(valueString);
    if (computerConfiguration.debuggerConfiguration.returnAddress == -1)
        valueString = "";
    else
        valueString.Printf("%04X", (int)computerConfiguration.debuggerConfiguration.returnAddress);
    XRCCTRL(*this,"DebugRetAddress",wxTextCtrl)->ChangeValue(valueString);

    if (computerConfiguration.soundConfiguration.type == SOUND_SUPER_VP550 || computerConfiguration.soundConfiguration.type == SOUND_SUPER_VP551)
    {
        XRCCTRL(*this,"TempoTextXml", wxStaticText)->Enable(true);
        XRCCTRL(*this,"TempoXml", wxSlider)->Enable(true);
    }
    else
    {
        XRCCTRL(*this,"TempoTextXml", wxStaticText)->Enable(false);
        XRCCTRL(*this,"TempoXml", wxSlider)->Enable(false);
    }
    XRCCTRL(*this, "TempoXml", wxSlider)->SetValue(computerConfiguration.soundConfiguration.tempo);
    
    XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->Enable(computerConfiguration.slotConfiguration.maxSlotNumber_ > 0);
    XRCCTRL(*this, "DebugExpansionBank", SlotEdit)->Enable(computerConfiguration.slotConfiguration.banksInUse_);

    if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
        XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->setRange(0, computerConfiguration.slotConfiguration.maxSlotNumber_-1);
}

void GuiXml::onVideoNumber(wxCommandEvent&WXUNUSED(event))
{
    computerConfiguration.videoNumber_++;
    if (computerConfiguration.videoNumber_ == computerConfiguration.numberOfVideoTypes_)
        computerConfiguration.videoNumber_ = 0;
    
    XRCCTRL(*this, "ZoomTextXml", wxStaticText)->SetLabel(computerConfiguration.videoName_[computerConfiguration.videoNumber_] + ", zoom:");
    correctZoomAndValue(SET_SPIN, computerConfiguration.videoNumber_);
}

void GuiXml::onPrintFile(wxCommandEvent& WXUNUSED(event))
{
     wxString fileName;

    fileName = wxFileSelector( "Select the Printer Output File",
                               computerConfiguration.printerFileConfiguration.directory, XRCCTRL(*this, "PrintFileXml", wxTextCtrl)->GetValue(),
                               "txt",
                               wxString::Format
                              (
                                   "Text File (*.txt)|*.txt|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );


    if (!fileName)
        return;

    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    computerConfiguration.printerFileConfiguration.directory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    computerConfiguration.printerFileConfiguration.fileName = FullPath.GetFullName();

    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Print_File", computerConfiguration.printerFileConfiguration.fileName);
    writeConfigDir(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Dir/Print_File", computerConfiguration.printerFileConfiguration.directory);
    XRCCTRL(*this, "PrintFileXml", wxTextCtrl)->SetValue(computerConfiguration.printerFileConfiguration.fileName);
}

void GuiXml::onXmlPrintFileText(wxCommandEvent&WXUNUSED(event))
{
    computerConfiguration.printerFileConfiguration.fileName = XRCCTRL(*this, "PrintFileXml", wxTextCtrl)->GetValue();
    configPointer->Write(dirNameList_[xmlDirComboSelection]+"/"+dirNameListDefaultFile_[xmlDirComboSelection]+"/Print_File", computerConfiguration.printerFileConfiguration.fileName);

    if (computerRunning_)
    {
        if (computerConfiguration.basicPrinterConfiguration.defined)
        {
            if (computerConfiguration.printerFileConfiguration.fileName.Len() == 0)
                p_PrinterParallel->setPrintfileName("");
            else
                p_PrinterParallel->setPrintfileName(computerConfiguration.printerFileConfiguration.directory + computerConfiguration.printerFileConfiguration.fileName);
        }
        if (computerConfiguration.parallelPrinterConfiguration.defined)
        {
            if (computerConfiguration.printerFileConfiguration.fileName.Len() == 0)
                p_PrinterParallel->setPrintfileName("");
            else
                p_PrinterParallel->setPrintfileName(computerConfiguration.printerFileConfiguration.directory + computerConfiguration.printerFileConfiguration.fileName);
        }
        if (computerConfiguration.serialPrinterConfiguration.defined)
        {
            if (computerConfiguration.printerFileConfiguration.fileName.Len() == 0)
                p_PrinterSerial->setPrintfileName("");
            else
                p_PrinterSerial->setPrintfileName(computerConfiguration.printerFileConfiguration.directory + computerConfiguration.printerFileConfiguration.fileName);
        }
        if (computerConfiguration.thermalPrinterConfiguration.defined)
        {
            if (computerConfiguration.printerFileConfiguration.fileName.Len() == 0)
                p_PrinterThermal->setPrintfileName("");
            else
                p_PrinterThermal->setPrintfileName(computerConfiguration.printerFileConfiguration.directory + computerConfiguration.printerFileConfiguration.fileName);
        }
    }
}

void GuiXml::onXmlPrintMode(wxCommandEvent&event)
{
    computerConfiguration.printMode_ = event.GetSelection();
    setPrintMode();

    if (computerRunning_)
    {
        if (computerConfiguration.parallelPrinterConfiguration.defined)
        {
            p_PrinterParallel->setPrintMode(computerConfiguration.printMode_);
        }
        if (computerConfiguration.serialPrinterConfiguration.defined)
        {
            p_PrinterSerial->setPrintMode(computerConfiguration.printMode_);
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
    if (computerRunning_)
    {
        p_Computer->onXmlF4(forceStart);
        if (computerConfiguration.printMode_ == PRINTFILE)
        {
            computerConfiguration.printMode_ = PRINTWINDOW;
            if (mode_.gui)
                XRCCTRL(*this, "PrintModeXml", wxChoice)->SetSelection(computerConfiguration.printMode_);
            setPrintMode();
        }
    }
}

void GuiXml::onXmlBaudR(wxCommandEvent&event)
{
    if (computerConfiguration.videoTerminalConfiguration.baudR != event.GetSelection())
    {
        XRCCTRL(*this, "XmlClearRam", wxCheckBox)->SetValue(true);
        computerConfiguration.clearRam = true;
        computerConfiguration.videoTerminalConfiguration.baudR = event.GetSelection();
    }
}

void GuiXml::onXmlBaudT(wxCommandEvent&event)
{
    if (computerConfiguration.videoTerminalConfiguration.baudT != event.GetSelection())
    {
        XRCCTRL(*this, "XmlClearRam", wxCheckBox)->SetValue(true);
        computerConfiguration.clearRam = true;
        computerConfiguration.videoTerminalConfiguration.baudT = event.GetSelection();
        if (!computerConfiguration.videoTerminalConfiguration.uart1854_defined && !computerConfiguration.videoTerminalConfiguration.uart16450_defined)
        {
            computerConfiguration.videoTerminalConfiguration.baudR = event.GetSelection();
            XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->SetSelection(computerConfiguration.videoTerminalConfiguration.baudR);
        }
    }
}

void GuiXml::onTempo(wxScrollEvent&event)
{
    computerConfiguration.soundConfiguration.tempo = event.GetPosition();
    if (computerRunning_)
        p_Computer->setTempo(computerConfiguration.soundConfiguration.tempo);
}


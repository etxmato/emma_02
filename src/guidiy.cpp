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
#include "guidiy.h"

BEGIN_EVENT_TABLE(GuiDiy, GuiPico)

    EVT_TEXT(XRCID("MainRamDiy"), GuiDiy::onMainRamTextDiy)
    EVT_COMBOBOX(XRCID("MainRamDiy"), GuiDiy::onMainRamTextDiy)
    EVT_BUTTON(XRCID("RamButtonDiy"), GuiDiy::onMainRamDiy)

    EVT_TEXT(XRCID("MainXmlDiy"), GuiDiy::onMainXmlTextDiy)
    EVT_COMBOBOX(XRCID("MainXmlDiy"), GuiDiy::onMainXmlTextDiy)
    EVT_BUTTON(XRCID("XmlButtonDiy"), GuiDiy::onMainXmlDiy)

    EVT_TEXT(XRCID("KeyFileDiy"), GuiMain::onKeyFileText)
    EVT_BUTTON(XRCID("KeyFileButtonDiy"), GuiMain::onKeyFile)
    EVT_BUTTON(XRCID("EjectKeyFileDiy"), GuiMain::onKeyFileEject)

    EVT_CHOICE(XRCID("VTTypeDiy"), GuiDiy::onVT100)

    EVT_SPIN_UP(XRCID("ZoomSpinDiy"), GuiMain::onZoom)
    EVT_SPIN_DOWN(XRCID("ZoomSpinDiy"), GuiMain::onZoom)
    EVT_TEXT(XRCID("ZoomValueDiy"), GuiMain::onZoomValue)
    EVT_SPIN_UP(XRCID("ZoomSpinVtDiy"), GuiMain::onZoomVt)
    EVT_SPIN_DOWN(XRCID("ZoomSpinVtDiy"), GuiMain::onZoomVt)
    EVT_TEXT(XRCID("ZoomValueVtDiy"), GuiMain::onZoomValueVt)
    EVT_CHECKBOX(XRCID("InterlaceDiy"), GuiMain::onInterlace)
    EVT_CHECKBOX(XRCID("StretchDotDiy"), GuiMain::onStretchDot)
    EVT_BUTTON(XRCID("ScreenDumpFileButtonDiy"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileDiy"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileDiy"), GuiMain::onScreenDumpFileText)
    EVT_BUTTON(XRCID("ScreenDumpF5Diy"), GuiMain::onScreenDump)
    EVT_BUTTON(XRCID("SaveButtonDiy"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("VtSetupDiy"), GuiMain::onVtSetup)
    EVT_BUTTON(XRCID("LoadButtonDiy"), GuiMain::onLoadButton)
    EVT_BUTTON(XRCID("RunButtonDiy"), GuiMain::onLoadRunButton)
    EVT_BUTTON(XRCID("DsaveButtonDiy"), GuiMain::onDataSaveButton)
    EVT_CHECKBOX(XRCID("UseLocationDiy"), GuiMain::onUseLocation)

    EVT_BUTTON(XRCID("FullScreenF3Diy"), GuiMain::onFullScreen)

    EVT_TEXT(XRCID("PrintFileDiy"), GuiMain::onPrintFileText)
    EVT_BUTTON(XRCID("PrintButtonDiy"), GuiMain::onPrintButton)
    EVT_CHOICE(XRCID("PrintModeDiy"), GuiMain::onPrintMode)
    EVT_BUTTON(XRCID("PrintFileButtonDiy"), GuiMain::onPrintFile)
        
    EVT_CHOICE(XRCID("VTBaudTChoiceDiy"), GuiMain::onBaudT)
    EVT_CHOICE(XRCID("VTBaudRChoiceDiy"), GuiMain::onBaudR)

    EVT_COMMAND(wxID_ANY, OPEN_PRINTER_WINDOW, GuiMain::openPrinterFrame) 

    EVT_CHECKBOX(XRCID("ControlWindowsDiy"), GuiDiy::onDiyControlWindows)
    EVT_BUTTON(XRCID("KeyMapDiy"), Main::onHexKeyDef)

    EVT_BUTTON(XRCID("CasButtonDiy"), GuiMain::onCassette)
    EVT_BUTTON(XRCID("EjectCasDiy"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileDiy"), GuiMain::onCassetteText)
    EVT_BUTTON(XRCID("RealCasLoadDiy"), GuiMain::onRealCas)
    EVT_BUTTON(XRCID("CasLoadDiy"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasSaveDiy"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasStopDiy"), GuiMain::onCassetteStop)
    EVT_CHECKBOX(XRCID("TurboDiy"), GuiMain::onTurbo)
    EVT_TEXT(XRCID("TurboClockDiy"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadDiy"), GuiMain::onAutoLoad)
    EVT_BUTTON(XRCID("TapeDiy"), GuiElf::onTape)

    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeDiy"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeDiy"), GuiMain::onVolume)

    EVT_BUTTON(XRCID("ColoursDiy"), Main::onColoursDef)
    EVT_TEXT(XRCID("ShowAddressDiy"), GuiMain::onLedTimer)
    EVT_TEXT(XRCID("SaveStartDiy"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndDiy"), GuiMain::onSaveEnd)
    EVT_TEXT(XRCID("SaveExecDiy"), GuiMain::onSaveExec)

    END_EVENT_TABLE()

GuiDiy::GuiDiy(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiPico(title, pos, size, mode, dataDir, iniDir)
{
    tapeOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/tick.png", wxBITMAP_TYPE_PNG);
    tapeOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/minus.png", wxBITMAP_TYPE_PNG);
}

void GuiDiy::readDiyConig()
{
//    return; // *** to be removed
    
    selectedComputer_ = DIY; // *** to be removed
    elfConfiguration[DIY].elfPortConf.emsOutput.resize(1);

    conf[DIY].xmlDir_ = readConfigDir("Dir/Diy/Xml_File", dataDir_);;
    conf[DIY].xmlFile_ = configPointer->Read("Diy/Xml_File", "example.xml");

    parseXmlFile(DIY,conf[DIY].xmlDir_, conf[DIY].xmlFile_);
    
    if (conf[DIY].memConfig_[0].filename == "")
    {
        conf[DIY].memConfig_[0].dirname = readConfigDir("/Dir/Diy/Main_Ram_File", dataDir_);
        conf[DIY].memConfig_[0].filename = configPointer->Read("Diy/Main_Ram_File", "");
        ramFileFromGui_ = true;
    }
    else
        ramFileFromGui_ = false;
    
    selectedComputer_ = DIY;

    conf[DIY].volume_ = 0;

    conf[DIY].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + computerInfo[DIY].gui + pathSeparator_;
    conf[DIY].ramDir_ = readConfigDir("/Dir/Diy/SWD", dataDir_ + computerInfo[DIY].gui + pathSeparator_);
    conf[DIY].keyFileDir_ = readConfigDir("/Dir/Diy/Key_File", dataDir_ + computerInfo[DIY].gui + pathSeparator_);
    conf[DIY].printFileDir_ = readConfigDir("/Dir/Diy/Print_File", dataDir_ + computerInfo[DIY].gui + pathSeparator_);
    conf[DIY].screenDumpFileDir_ = readConfigDir("/Dir/Diy/Video_Dump_File", dataDir_ + computerInfo[DIY].gui + pathSeparator_);
    conf[DIY].wavFileDir_[0] = readConfigDir("/Dir/Diy/Wav_File", dataDir_ + computerInfo[DIY].gui + pathSeparator_);

    conf[DIY].keyFile_ = configPointer->Read("Diy/Key_File", "");
    conf[DIY].printFile_ = configPointer->Read("Diy/Print_File", "printerout.txt");
    conf[DIY].screenDumpFile_ = configPointer->Read("Diy/Video_Dump_File", "screendump.png");
    conf[DIY].wavFile_[0] = configPointer->Read("Diy/Wav_File", "");
    conf[DIY].terminalFiles_.Add(conf[DIY].wavFile_[0]);
    conf[DIY].terminalPaths_.Add(conf[DIY].wavFileDir_[0]+conf[DIY].wavFile_[0]);
    conf[DIY].numberOfTerminalFiles_ = 1;

    conf[DIY].saveEndString_ = "";
    conf[DIY].saveExecString_ = "";

    elfConfiguration[DIY].vt52SetUpFeature_ = configPointer->Read("Diy/VT52Setup", 0x00004092l);
    elfConfiguration[DIY].vt100SetUpFeature_ = configPointer->Read("Diy/VT100Setup", 0x0000ca52l);
    elfConfiguration[DIY].vtExternalSetUpFeature_ = configPointer->Read("Diy/VTExternalSetup", 0x0000ca52l);
    
    elfConfiguration[DIY].vtCharactersPerRow = (int)configPointer->Read("Diy/VT100CharPerRow", 80);
    elfConfiguration[DIY].vt100CharWidth = (int)configPointer->Read("Diy/VT100CharWidth", 10);
    elfConfiguration[DIY].vt52CharWidth = (int)configPointer->Read("Diy/VT52CharWidth", 9);

    elfConfiguration[DIY].bellFrequency_ = (int)configPointer->Read("Diy/Bell_Frequency", 800);
    configPointer->Read("Diy/SerialLog", &elfConfiguration[DIY].serialLog, false);
    configPointer->Read("Diy/ESCError", &elfConfiguration[DIY].escError, false);
    configPointer->Read("Diy/ClearRtc", &elfConfiguration[DIY].clearRtc, false);
    configPointer->Read("Diy/Open_Control_Windows", &elfConfiguration[DIY].useElfControlWindows, false);

    configPointer->Read("Diy/Enable_Interlace", &conf[DIY].interlace_, true);
    configPointer->Read("Diy/Enable_Vt_Stretch_Dot", &conf[DIY].stretchDot_, false);
    configPointer->Read("Diy/Enable_Vt_External", &elfConfiguration[DIY].vtExternal, false);
    conf[DIY].printMode_ = (int)configPointer->Read("Diy/Print_Mode", 1l);

    configPointer->Read("Diy/GiantBoardMapping", &elfConfiguration[DIY].giantBoardMapping, false);

    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 2.0);
    conf[DIY].zoom_ = convertLocale(configPointer->Read("Diy/Zoom", defaultZoom));
    defaultZoom.Printf("%2.2f", 1.0);
    conf[DIY].zoomVt_ = convertLocale(configPointer->Read("Diy/Vt_Zoom", defaultZoom));
    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[DIY].xScale_ = convertLocale(configPointer->Read("Diy/Window_Scale_Factor_X", defaultScale));

    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[DIY].ledTime_ = configPointer->Read("Diy/Led_Update_Frequency", defaultTimer);

    configPointer->Read("Diy/Enable_Turbo_Cassette", &conf[DIY].turbo_, true);
    conf[DIY].turboClock_ = configPointer->Read("Diy/Turbo_Clock_Speed", "15");
    configPointer->Read("Diy/Enable_Auto_Cassette", &conf[DIY].autoCassetteLoad_, true);
    elfConfiguration[DIY].packetSize = (int)configPointer->Read("Diy/Ymodem_PacketSize", 0l);
    configPointer->Read("Diy/Enable_Real_Cassette", &conf[DIY].realCassetteLoad_, false);
    conf[DIY].volume_ = (int)configPointer->Read("Diy/Volume", 25l);

    long value; 

    conf[DIY].saveStartString_ = configPointer->Read("Diy/SaveStart", "0");
    if (!conf[DIY].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[DIY].saveStart_ = value;

    configPointer->Read("Diy/UseLoadLocation", &conf[DIY].useLoadLocation_, false);

    setXmlGui();
    
    if (mode_.gui)
    {
        XRCCTRL(*this, "MainXmlDiy", wxComboBox)->SetValue(conf[DIY].xmlFile_);
        XRCCTRL(*this, "MainRamDiy", wxComboBox)->SetValue(conf[DIY].memConfig_[0].filename);
        if (!ramFileFromGui_)
        {
            XRCCTRL(*this,"MainRamDiy", wxComboBox)->Enable(false);
            XRCCTRL(*this,"RamButtonDiy", wxButton)->Enable(false);
        }

        XRCCTRL(*this, "KeyFileDiy", wxTextCtrl)->SetValue(conf[DIY].keyFile_);
        XRCCTRL(*this, "PrintFileDiy", wxTextCtrl)->SetValue(conf[DIY].printFile_);
        XRCCTRL(*this, "ScreenDumpFileDiy", wxComboBox)->SetValue(conf[DIY].screenDumpFile_);
        XRCCTRL(*this, "WavFileDiy", wxTextCtrl)->SetValue(conf[DIY].wavFile_[0]);
    
        correctZoomAndValue(DIY, "Diy", SET_SPIN);
        correctZoomVtAndValue(DIY, "Diy", SET_SPIN);

        XRCCTRL(*this, "ControlWindowsDiy", wxCheckBox)->SetValue(elfConfiguration[DIY].useElfControlWindows);
            
        XRCCTRL(*this, "InterlaceDiy", wxCheckBox)->SetValue(conf[DIY].interlace_);
        XRCCTRL(*this, "StretchDotDiy", wxCheckBox)->SetValue(conf[DIY].stretchDot_);

        XRCCTRL(*this, "PrintModeDiy", wxChoice)->SetSelection(conf[DIY].printMode_);

        XRCCTRL(*this, "TurboDiy", wxCheckBox)->SetValue(conf[DIY].turbo_);
        turboGui("Diy");

        XRCCTRL(*this, "TurboClockDiy", wxTextCtrl)->SetValue(conf[DIY].turboClock_);
        XRCCTRL(*this, "AutoCasLoadDiy", wxCheckBox)->SetValue(conf[DIY].autoCassetteLoad_);

        XRCCTRL(*this, "VolumeDiy", wxSlider)->SetValue(conf[DIY].volume_);

        XRCCTRL(*this,"AddressText1Diy", wxStaticText)->Enable(elfConfiguration[DIY].useElfControlWindows && elfConfiguration[DIY].panelType_ != PANEL_NONE);
        XRCCTRL(*this,"AddressText2Diy", wxStaticText)->Enable(elfConfiguration[DIY].useElfControlWindows && elfConfiguration[DIY].panelType_ != PANEL_NONE);
        XRCCTRL(*this,"ShowAddressDiy", wxTextCtrl)->Enable(elfConfiguration[DIY].useElfControlWindows && elfConfiguration[DIY].panelType_ != PANEL_NONE);
        XRCCTRL(*this, "ShowAddressDiy", wxTextCtrl)->ChangeValue(conf[DIY].ledTime_);

        XRCCTRL(*this, "UseLocationDiy", wxCheckBox)->SetValue(conf[DIY].useLoadLocation_);
        if (conf[DIY].saveStart_ != 0)
            XRCCTRL(*this, "SaveStartDiy", wxTextCtrl)->SetValue(conf[DIY].saveStartString_);

    }

    setRealCas(DIY);

    elfConfiguration[DIY].useLedModule = false;

    conf[DIY].loadFileNameFull_ = "";
    conf[DIY].loadFileName_ = "";

    conf[DIY].pLoadSaveName_[0] = 'S';
    conf[DIY].pLoadSaveName_[1] = 'U';
    conf[DIY].pLoadSaveName_[2] = 'P';
    conf[DIY].pLoadSaveName_[3] = 'E';

    conf[DIY].defus_ = 0x2a81;
    conf[DIY].eop_ = 0x2a83;
    conf[DIY].string_ = 0x2a92;
    conf[DIY].arrayValue_ = 0x2a94;
    conf[DIY].eod_ = 0x2a99;
    conf[DIY].basicRamAddress_ = 0x2c00;
}

void GuiDiy::writeDiyDirConfig()
{
//    return; // *** to be removed
    
    writeConfigDir("/Dir/Diy/Xml_File", conf[DIY].xmlDir_);
    if (ramFileFromGui_)
        writeConfigDir("/Dir/Diy/Main_Ram_File", conf[DIY].memConfig_[0].dirname);

    writeConfigDir("/Dir/Diy/Software_File", conf[DIY].ramDir_);
    writeConfigDir("/Dir/Diy/Key_File", conf[DIY].keyFileDir_);
    writeConfigDir("/Dir/Diy/Print_File", conf[DIY].printFileDir_);
    writeConfigDir("/Dir/Diy/Video_Dump_File", conf[DIY].screenDumpFileDir_);
    writeConfigDir("/Dir/Diy/Wav_File", conf[DIY].wavFileDir_[0]);
}
    
void GuiDiy::writeDiyConfig()
{
//    return; // *** to be removed
    wxString buffer;
    
    configPointer->Write("Diy/Xml_File", conf[DIY].xmlFile_);
    if (ramFileFromGui_)
        configPointer->Write("Diy/Main_Ram_File", conf[DIY].memConfig_[0].filename);

    configPointer->Write("Diy/Key_File", conf[DIY].keyFile_);
    configPointer->Write("Diy/Print_File", conf[DIY].printFile_);
    configPointer->Write("Diy/Video_Dump_File", conf[DIY].screenDumpFile_);
    configPointer->Write("Diy/Wav_File", conf[DIY].wavFile_[0]);

    buffer.Printf("%04X", (unsigned int)startRam_[DIY]);
    configPointer->Write("Diy/Ram_Start_Address", buffer);
    buffer.Printf("%04X", (unsigned int)endRam_[DIY]);
    configPointer->Write("Diy/Ram_End_Address", buffer);

    long value = elfConfiguration[DIY].vt52SetUpFeature_.to_ulong();
    configPointer->Write("Diy/VT52Setup", value);
    value = elfConfiguration[DIY].vt100SetUpFeature_.to_ulong();
    configPointer->Write("Diy/VT100Setup", value);
    value = elfConfiguration[DIY].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("Diy/VTExternalSetup", value);
    configPointer->Write("Diy/VT100CharPerRow", elfConfiguration[DIY].vtCharactersPerRow);
    configPointer->Write("Diy/VT100CharWidth", elfConfiguration[DIY].vt100CharWidth);
    configPointer->Write("Diy/VT52CharWidth", elfConfiguration[DIY].vt52CharWidth);

    configPointer->Write("Diy/Bell_Frequency", elfConfiguration[DIY].bellFrequency_);
    configPointer->Write("Diy/SerialLog", elfConfiguration[DIY].serialLog);
    configPointer->Write("Diy/ESCError", elfConfiguration[DIY].escError);
    configPointer->Write("Diy/ClearRtc", elfConfiguration[DIY].clearRtc);
    configPointer->Write("Diy/Zoom", conf[DIY].zoom_);
    configPointer->Write("Diy/Vt_Zoom", conf[DIY].zoomVt_);
    configPointer->Write("Diy/UseLoadLocation", conf[DIY].useLoadLocation_);
    configPointer->Write("Diy/SaveStart", conf[DIY].saveStartString_);

    configPointer->Write("Diy/GiantBoardMapping", elfConfiguration[DIY].giantBoardMapping);

    configPointer->Write("Diy/Enable_Printer", conf[DIY].printerOn_);
    configPointer->Write("Diy/Print_Mode", conf[DIY].printMode_);

    configPointer->Write("Diy/Open_Control_Windows", elfConfiguration[DIY].useElfControlWindows);
    configPointer->Write("Diy/Enable_Interlace", conf[DIY].interlace_);
    configPointer->Write("Diy/Enable_Vt_Stretch_Dot", conf[DIY].stretchDot_);
    configPointer->Write("Diy/Enable_Vt_External", elfConfiguration[DIY].vtExternal);

    configPointer->Write("Diy/Led_Update_Frequency", conf[DIY].ledTime_);
    configPointer->Write("Diy/Enable_Turbo_Cassette", conf[DIY].turbo_);
    configPointer->Write("Diy/Turbo_Clock_Speed", conf[DIY].turboClock_);
    configPointer->Write("Diy/Enable_Auto_Cassette", conf[DIY].autoCassetteLoad_);
    configPointer->Write("Diy/Enable_Real_Cassette", conf[DIY].realCassetteLoad_);
    configPointer->Write("Diy/Ymodem_PacketSize", elfConfiguration[DIY].packetSize);
    configPointer->Write("Diy/Volume", conf[DIY].volume_);
}

void GuiDiy::readDiyWindowConfig()
{
//    return; // *** to be removed
    
    conf[DIY].pixieX_ = (int)configPointer->Read("Diy/Window_Position_Pixie_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[DIY].pixieY_ = (int)configPointer->Read("Diy/Window_Position_Pixie_Y", mainWindowY_);
    conf[DIY].tmsX_ = (int)configPointer->Read("Diy/Window_Position_Tms_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[DIY].tmsY_ = (int)configPointer->Read("Diy/Window_Position_Tms_Y", mainWindowY_);
    conf[DIY].mc6845X_ = (int)configPointer->Read("Diy/Window_Position_MC6845_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[DIY].mc6845Y_ = (int)configPointer->Read("Diy/Window_Position_MC6845_Y", mainWindowY_);
    conf[DIY].mc6847X_ = (int)configPointer->Read("Diy/Window_Position_MC6847_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[DIY].mc6847Y_ = (int)configPointer->Read("Diy/Window_Position_MC6847_Y", mainWindowY_);
    conf[DIY].i8275X_ = (int)configPointer->Read("Diy/Window_Position_I8275_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[DIY].i8275Y_ = (int)configPointer->Read("Diy/Window_Position_I8275_Y", mainWindowY_);
    conf[DIY].vtX_ = (int)configPointer->Read("Diy/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[DIY].vtY_ = (int)configPointer->Read("Diy/Window_Position_Vt_Y", mainWindowY_);
    conf[DIY].mainX_ = (int)configPointer->Read("Diy/Window_Position_X", mainWindowX_);
    conf[DIY].mainY_ = (int)configPointer->Read("Diy/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiDiy::writeDiyWindowConfig()
{
//    return; // *** to be removed xx
    
    if (conf[DIY].mainX_ > 0)
        configPointer->Write("Diy/MainX", conf[DIY].mainX_);
    if (conf[DIY].mainY_ > 0)
        configPointer->Write("Diy/MainY", conf[DIY].mainY_);
    if (conf[DIY].pixieX_ > 0)
        configPointer->Write("Diy/Window_Position_Pixie_X", conf[DIY].pixieX_);
    if (conf[DIY].pixieY_ > 0)
        configPointer->Write("Diy/Window_Position_Pixie_Y", conf[DIY].pixieY_);
    if (conf[DIY].tmsX_ > 0)
        configPointer->Write("Diy/Window_Position_Tms_X", conf[DIY].tmsX_);
    if (conf[DIY].tmsY_ > 0)
        configPointer->Write("Diy/Window_Position_Tms_Y", conf[DIY].tmsY_);
    if (conf[DIY].mc6845X_ > 0)
        configPointer->Write("Diy/Window_Position_MC6845_X", conf[DIY].mc6845X_);
    if (conf[DIY].mc6845Y_ > 0)
        configPointer->Write("Diy/Window_Position_MC6845_Y", conf[DIY].mc6845Y_);
    if (conf[DIY].mc6847X_ > 0)
        configPointer->Write("Diy/Window_Position_MC6847_X", conf[DIY].mc6847X_);
    if (conf[DIY].mc6847Y_ > 0)
        configPointer->Write("Diy/Window_Position_MC6847_Y", conf[DIY].mc6847Y_);
    if (conf[DIY].i8275X_ > 0)
        configPointer->Write("Diy/Window_Position_I8275_X", conf[DIY].i8275X_);
    if (conf[DIY].i8275Y_ > 0)
        configPointer->Write("Diy/Window_Position_I8275_Y", conf[DIY].i8275Y_);
    if (conf[DIY].vtX_ > 0)
        configPointer->Write("Diy/Window_Position_Vt_X", conf[DIY].vtX_);
    if (conf[DIY].vtY_ > 0)
        configPointer->Write("Diy/Window_Position_Vt_Y", conf[DIY].vtY_);
}

void GuiDiy::onDiyControlWindows(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].useElfControlWindows = event.IsChecked();

    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddress"+computerInfo[selectedComputer_].gui,wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText1"+computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText2"+computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
    }
    
    if (computerRunning_)
        p_Diy->Show(elfConfiguration[runningComputer_].useElfControlWindows);
}

void GuiDiy::onMainRamDiy(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;

    fileName = wxFileSelector( "Select the RAM file to load",
                              conf[selectedComputer_].memConfig_[0].dirname, XRCCTRL(*this, "MainRamDiy", wxComboBox)->GetValue(),
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

    XRCCTRL(*this, "MainRamDiy", wxComboBox)->SetValue(conf[selectedComputer_].memConfig_[0].filename);
}

void GuiDiy::onMainRamTextDiy(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].memConfig_[0].filename = XRCCTRL(*this, "MainRamDiy", wxComboBox)->GetValue();
}

void GuiDiy::onMainXmlDiy(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;

    fileName = wxFileSelector( "Select the XML file to load",
                              conf[selectedComputer_].xmlDir_, XRCCTRL(*this, "MainXmlDiy", wxComboBox)->GetValue(),
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

    XRCCTRL(*this, "MainXmlDiy", wxComboBox)->SetValue(conf[selectedComputer_].xmlFile_);
}

void GuiDiy::onMainXmlTextDiy(wxCommandEvent& WXUNUSED(event))
{
    conf[selectedComputer_].xmlFile_ = XRCCTRL(*this, "MainXmlDiy", wxComboBox)->GetValue();
    
    parseXmlFile(selectedComputer_, conf[selectedComputer_].xmlDir_, conf[selectedComputer_].xmlFile_);
}

void GuiDiy::setPrintModeDiy()
{
    if (conf[selectedComputer_].printerOn_)
    {
        XRCCTRL(*this, "PrintModeDiy", wxChoice)->Enable(true);
        if (computerRunning_)
            XRCCTRL(*this, "PrintButtonDiy", wxBitmapButton)->Enable(true);
        else
            XRCCTRL(*this, "PrintButtonDiy", wxBitmapButton)->Enable(false);
    }
    else
    {
        XRCCTRL(*this, "PrintModeDiy", wxChoice)->Enable(false);
        XRCCTRL(*this, "PrintButtonDiy", wxBitmapButton)->Enable(false);
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

void GuiDiy::setXmlGui()
{
    setVtType("Diy", DIY, elfConfiguration[DIY].vtType, false);

    if (!mode_.gui)
        return;
    
    setBaudChoice(DIY);
    setPrintModeDiy();
    setTapeType("Diy", DIY);

    XRCCTRL(*this, "VTBaudRChoiceDiy", wxChoice)->SetSelection(elfConfiguration[DIY].baudR);
    XRCCTRL(*this, "VTBaudTChoiceDiy", wxChoice)->SetSelection(elfConfiguration[DIY].baudT);
    if (elfConfiguration[DIY].vtExternal)
        XRCCTRL(*this, "VTTypeDiy", wxChoice)->SetSelection(EXTERNAL_TERMINAL);
    else
        XRCCTRL(*this, "VTTypeDiy", wxChoice)->SetSelection(elfConfiguration[DIY].vtType);
  
    if (elfConfiguration[DIY].panelType_ == PANEL_NONE)
        elfConfiguration[DIY].useElfControlWindows = false;

    XRCCTRL(*this,"ControlWindowsDiy", wxCheckBox)->Enable(elfConfiguration[DIY].panelType_ != PANEL_NONE);

    if (clockTextCtrl[DIY] != NULL)
        clockTextCtrl[DIY]->ChangeValue(conf[DIY].clock_);
}


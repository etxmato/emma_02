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
#include "guipico.h"

BEGIN_EVENT_TABLE(GuiPico, GuiElf)

    EVT_TEXT(XRCID("MainRomPico"), GuiMain::onMainRom1Text)
    EVT_COMBOBOX(XRCID("MainRomPico"), GuiMain::onMainRom1Text)
    EVT_BUTTON(XRCID("RomButtonPico"), GuiMain::onMainRom1)

    EVT_TEXT(XRCID("IdeFilePico"), GuiMain::onIdeText)
    EVT_BUTTON(XRCID("IDE_ButtonPico"), GuiMain::onIde)
    EVT_BUTTON(XRCID("Eject_IDEPico"), GuiMain::onIdeEject)

    EVT_CHOICE(XRCID("VTTypePico"), GuiMain::onVT100)
    EVT_CHOICE(XRCID("QsoundPico"), GuiElf::onQsound)
    EVT_SPIN_UP(XRCID("ZoomSpinPico"), GuiMain::onZoom)
    EVT_SPIN_DOWN(XRCID("ZoomSpinPico"), GuiMain::onZoom)
    EVT_TEXT(XRCID("ZoomValuePico"), GuiMain::onZoomValue)
    EVT_SPIN_UP(XRCID("ZoomSpinVtPico"), GuiMain::onZoomVt)
    EVT_SPIN_DOWN(XRCID("ZoomSpinVtPico"), GuiMain::onZoomVt)
    EVT_TEXT(XRCID("ZoomValueVtPico"), GuiMain::onZoomValueVt)
    EVT_CHECKBOX(XRCID("StretchDotPico"), GuiMain::onStretchDot)
    EVT_BUTTON(XRCID("ScreenDumpFileButtonPico"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFilePico"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFilePico"), GuiMain::onScreenDumpFileText)
    EVT_BUTTON(XRCID("ScreenDumpF5Pico"), GuiMain::onScreenDump)
    EVT_BUTTON(XRCID("SaveButtonPico"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("DP_ButtonPico"), GuiMain::onDp)
    EVT_BUTTON(XRCID("VtSetupPico"), GuiMain::onVtSetup)
    EVT_BUTTON(XRCID("LoadButtonPico"), GuiMain::onLoadButton)
    EVT_BUTTON(XRCID("RunButtonPico"), GuiMain::onLoadRunButton)
    EVT_BUTTON(XRCID("DsaveButtonPico"), GuiMain::onDataSaveButton)
    EVT_CHECKBOX(XRCID("UseLocationPico"), GuiMain::onUseLocation)
    EVT_TEXT(XRCID("BeepFrequencyPico"), GuiMain::onBeepFrequency)

    EVT_CHOICE(XRCID("VideoTypePico"), GuiPico::onVideoTypePico)

    EVT_CHOICE(XRCID("DiskTypePico"), GuiPico::onDiskTypePico)
    EVT_BUTTON(XRCID("FullScreenF3Pico"), GuiMain::onFullScreen)

    EVT_TEXT(XRCID("PrintFilePico"), GuiMain::onPrintFileText)
    EVT_BUTTON(XRCID("PrintButtonPico"), GuiMain::onPrintButton)
    EVT_CHOICE(XRCID("PrintModePico"), GuiMain::onPrintMode)
    EVT_BUTTON(XRCID("PrintFileButtonPico"), GuiMain::onPrintFile)
        
    EVT_CHOICE(XRCID("VTBaudTChoicePico"), GuiMain::onBaudT)
    EVT_CHOICE(XRCID("VTBaudRChoicePico"), GuiMain::onBaudR)

    EVT_COMMAND(wxID_ANY, OPEN_PRINTER_WINDOW, GuiMain::openPrinterFrame) 

    EVT_BUTTON(XRCID("CasButtonPico"), GuiMain::onCassette)
    EVT_BUTTON(XRCID("EjectCasPico"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFilePico"), GuiMain::onCassetteText)
    EVT_BUTTON(XRCID("CasLoadPico"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasSavePico"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasStopPico"), GuiMain::onCassetteStop)
    EVT_CHECKBOX(XRCID("TurboPico"), GuiMain::onTurbo)
    EVT_TEXT(XRCID("TurboClockPico"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadPico"), GuiMain::onAutoLoad)

    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumePico"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumePico"), GuiMain::onVolume)

    EVT_BUTTON(XRCID("ColoursPico"), Main::onColoursDef)
    EVT_TEXT(XRCID("SaveStartPico"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndPico"), GuiMain::onSaveEnd)
    EVT_TEXT(XRCID("SaveExecPico"), GuiMain::onSaveExec)

    EVT_CHECKBOX(XRCID("PicoRtc"), GuiPico::onPicoRtc)
    EVT_CHECKBOX(XRCID("PicoClearRtc"), GuiPico::onPicoClearRtc)

    END_EVENT_TABLE()

GuiPico::GuiPico(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiElf(title, pos, size, mode, dataDir, iniDir)
{
    tapeOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/tick.png", wxBITMAP_TYPE_PNG);
    tapeOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/minus.png", wxBITMAP_TYPE_PNG);
}

void GuiPico::readPicoConfig()
{
    selectedComputer_ = PICO;

    elfConfiguration[PICO].elfPortConf.emsOutput.resize(1);
    readElfPortConfig(PICO, "Pico");
    
    conf[PICO].volume_ = 0;

    conf[PICO].emsConfigNumber_ = 0;

    conf[PICO].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Pico" + pathSeparator_;
    conf[PICO].mainDir_ = readConfigDir("/Dir/Pico/Main", dataDir_ + "Pico" + pathSeparator_);
    conf[PICO].romDir_[MAINROM1] = readConfigDir("/Dir/Pico/Main_Rom_1_File", dataDir_ + "Pico" + pathSeparator_);
    conf[PICO].charRomDir_ = readConfigDir("/Dir/Pico/Font_Rom_File", dataDir_ + "Pico" + pathSeparator_);
    elfConfiguration[PICO].vt100CharRomDir_ = readConfigDir("/Dir/Pico/Vt100_Font_Rom_File", dataDir_ + "Pico" + pathSeparator_);
    elfConfiguration[PICO].vt52CharRomDir_ = readConfigDir("/Dir/Pico/Vt52_Font_Rom_File", dataDir_ + "Pico" + pathSeparator_);
    conf[PICO].ramDir_ = readConfigDir("/Dir/Pico/SWD", dataDir_ + "Pico" + pathSeparator_);
    conf[PICO].ideDir_ = readConfigDir("/Dir/Pico/Ide_File", dataDir_ + "Pico" + pathSeparator_);
    conf[PICO].keyFileDir_ = readConfigDir("/Dir/Pico/Key_File", dataDir_ + "Pico" + pathSeparator_);
    conf[PICO].printFileDir_ = readConfigDir("/Dir/Pico/Print_File", dataDir_ + "Pico" + pathSeparator_);
    conf[PICO].screenDumpFileDir_ = readConfigDir("/Dir/Pico/Video_Dump_File", dataDir_ + "Pico" + pathSeparator_);
    conf[PICO].wavFileDir_[0] = readConfigDir("/Dir/Pico/Wav_File", dataDir_ + "Pico" + pathSeparator_);
    elfConfiguration[PICO].vtWavFileDir_ = readConfigDir("/Dir/Pico/Vt_Wav_File", dataDir_ + "Pico" + pathSeparator_);

    conf[PICO].videoMode_ = (int)configPointer->Read("Pico/Video_Type", 2l);

    conf[PICO].rom_[MAINROM1] = configPointer->Read("Pico/Main_Rom_1_File", "PicoElfv112.bin");
    conf[PICO].ide_ = configPointer->Read("Pico/Ide_File", "pixie.ide");
    conf[PICO].printFile_ = configPointer->Read("Pico/Print_File", "printerout.txt");
    conf[PICO].screenDumpFile_ = configPointer->Read("Pico/Video_Dump_File", "screendump.png");
    conf[PICO].wavFile_[0] = configPointer->Read("Pico/Wav_File", "");
    conf[PICO].terminalFiles_.Add(conf[PICO].wavFile_[0]);
    conf[PICO].terminalPaths_.Add(conf[PICO].wavFileDir_[0]+conf[PICO].wavFile_[0]);
    conf[PICO].numberOfTerminalFiles_ = 1;
    elfConfiguration[PICO].vtWavFile_ = configPointer->Read("Pico/Vt_Wav_File", "");
    elfConfiguration[PICO].serialPort_ = configPointer->Read("Pico/VtSerialPortChoice", "");

    conf[PICO].saveEndString_ = "";
    conf[PICO].saveExecString_ = "";

    elfConfiguration[PICO].qSound_ = (int)configPointer->Read("Pico/Enable_Q_Sound", 0l);
    elfConfiguration[PICO].vtType = (int)configPointer->Read("Pico/VT_Type", 2l);
    elfConfiguration[PICO].vt52SetUpFeature_ = configPointer->Read("Pico/VT52Setup", 0x00004092l);
    elfConfiguration[PICO].vt100SetUpFeature_ = configPointer->Read("Pico/VT100Setup", 0x0000ca52l);
    elfConfiguration[PICO].vtExternalSetUpFeature_ = configPointer->Read("Pico/VTExternalSetup", 0x0000ca52l);
    elfConfiguration[PICO].baudR = (int)configPointer->Read("Pico/Vt_Baud_Receive", 1l);
    elfConfiguration[PICO].baudT = (int)configPointer->Read("Pico/Vt_Baud_Transmit", 1l);
    
    elfConfiguration[PICO].vtCharactersPerRow = (int)configPointer->Read("Pico/VT100CharPerRow", 80);
    elfConfiguration[PICO].vt100CharWidth = (int)configPointer->Read("Pico/VT100CharWidth", 10);
    elfConfiguration[PICO].vt52CharWidth = (int)configPointer->Read("Pico/VT52CharWidth", 9);

    elfConfiguration[PICO].diskType = (int)configPointer->Read("Pico/Disk_Type", 1l);
    elfConfiguration[PICO].keyboardType = 0;

    elfConfiguration[PICO].bellFrequency_ = (int)configPointer->Read("Pico/Bell_Frequency", 800);
    configPointer->Read("Pico/SerialLog", &elfConfiguration[PICO].serialLog, false);
    configPointer->Read("Pico/ESCError", &elfConfiguration[PICO].escError, false);
    configPointer->Read("Pico/Uart", &elfConfiguration[PICO].useUart, false);
    configPointer->Read("Pico/Uart16450", &elfConfiguration[PICO].useUart16450, true);
    configPointer->Read("Pico/ClearRtc", &elfConfiguration[PICO].clearRtc, true);
    elfConfiguration[PICO].forceUpperCase = false;
    configPointer->Read("Pico/Use_Real_Time_Clock", &elfConfiguration[PICO].rtc, true);
    configPointer->Read("Pico/Enable_Printer", &conf[PICO].printerOn_, false);
    configPointer->Read("Pico/Enable_Vt_Stretch_Dot", &conf[PICO].stretchDot_, false);
    configPointer->Read("Pico/Enable_Vt_External", &elfConfiguration[PICO].vtExternal, false);
    conf[PICO].printMode_ = (int)configPointer->Read("Pico/Print_Mode", 1l);

    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 2.0);
    conf[PICO].zoom_ = convertLocale(configPointer->Read("Pico/Zoom", defaultZoom));
    defaultZoom.Printf("%2.2f", 1.0);
    conf[PICO].zoomVt_ = convertLocale(configPointer->Read("Pico/Vt_Zoom", defaultZoom));
    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[PICO].xScale_ = convertLocale(configPointer->Read("Pico/Window_Scale_Factor_X", defaultScale));

    wxString defaultClock;
    defaultClock.Printf("%1.2f", 4.0);
    conf[PICO].clock_ = convertLocale(configPointer->Read("Pico/Clock_Speed", defaultClock));

    conf[PICO].beepFrequency_ = (int)configPointer->Read("Pico/Beep_Frequency", 250);

    configPointer->Read("Pico/Enable_Turbo_Cassette", &conf[PICO].turbo_, true);
    conf[PICO].turboClock_ = configPointer->Read("Pico/Turbo_Clock_Speed", "15");
    configPointer->Read("Pico/Enable_Auto_Cassette", &conf[PICO].autoCassetteLoad_, true);
    elfConfiguration[PICO].useTape = false;
    elfConfiguration[PICO].useXmodem = true;
    elfConfiguration[PICO].packetSize = (int)configPointer->Read("Pico/Ymodem_PacketSize", 0l);
    conf[PICO].realCassetteLoad_ = false;
    conf[PICO].volume_ = (int)configPointer->Read("Pico/Volume", 25l);

    long value;
    conf[PICO].saveStartString_ = configPointer->Read("Pico/SaveStart", "0");
    if (!conf[PICO].saveStartString_.ToLong(&value, 16))
        value = 0;
    conf[PICO].saveStart_ = value;

    if (mode_.gui)
        setBaudChoice(PICO);

    setVtType("Pico", PICO, elfConfiguration[PICO].vtType, false);
    setVideoTypePico(conf[PICO].videoMode_);

    conf[PICO].charRom_ = configPointer->Read("Pico/Font_Rom_File", "super.video.bin");
    elfConfiguration[PICO].vt100CharRom_ = configPointer->Read("/Pico/Vt100_Font_Rom_File", "vt100.bin");
    elfConfiguration[PICO].vt52CharRom_ = configPointer->Read("/Pico/Vt52_Font_Rom_File", "vt52.a.bin");

    configPointer->Read("Pico/UseLoadLocation", &conf[PICO].useLoadLocation_, true);

    if (mode_.gui)
    {
        setPrinterState(PICO);
        XRCCTRL(*this, "MainRomPico", wxComboBox)->SetValue(conf[PICO].rom_[MAINROM1]);
        XRCCTRL(*this, "IdeFilePico", wxTextCtrl)->SetValue(conf[PICO].ide_);
        XRCCTRL(*this, "PrintFilePico", wxTextCtrl)->SetValue(conf[PICO].printFile_);
        XRCCTRL(*this, "ScreenDumpFilePico", wxComboBox)->SetValue(conf[PICO].screenDumpFile_);
        XRCCTRL(*this, "WavFilePico", wxTextCtrl)->SetValue(conf[PICO].wavFile_[0]);
    
        XRCCTRL(*this, "QsoundPico", wxChoice)->SetSelection(elfConfiguration[PICO].qSound_);
        if (elfConfiguration[PICO].vtExternal)
            XRCCTRL(*this, "VTTypePico", wxChoice)->SetSelection(EXTERNAL_TERMINAL);
        else
            XRCCTRL(*this, "VTTypePico", wxChoice)->SetSelection(elfConfiguration[PICO].vtType);
        XRCCTRL(*this, "PicoRtc", wxCheckBox)->SetValue(elfConfiguration[PICO].rtc);
        XRCCTRL(*this, "PicoClearRtc", wxCheckBox)->Enable(elfConfiguration[ELF2K].rtc);
        XRCCTRL(*this, "PicoClearRtc", wxCheckBox)->SetValue(elfConfiguration[PICO].clearRtc);
        XRCCTRL(*this, "BeepFrequencyPico", wxTextCtrl)->Enable(elfConfiguration[PICO].qSound_ == QSOUNDEXT);

        XRCCTRL(*this, "VTBaudRChoicePico", wxChoice)->SetSelection(elfConfiguration[PICO].baudR);
        XRCCTRL(*this, "VTBaudTChoicePico", wxChoice)->SetSelection(elfConfiguration[PICO].baudT);
        XRCCTRL(*this, "BeepFrequencyTextPico", wxStaticText)->Enable(elfConfiguration[PICO].qSound_ == QSOUNDEXT);
        XRCCTRL(*this, "BeepFrequencyTextHzPico", wxStaticText)->Enable(elfConfiguration[PICO].qSound_ == QSOUNDEXT);

        XRCCTRL(*this, "VideoTypePico", wxChoice)->SetSelection(conf[PICO].videoMode_);
        XRCCTRL(*this, "DiskTypePico", wxChoice)->SetSelection(elfConfiguration[PICO].diskType);

        correctZoomAndValue(PICO, "Pico", SET_SPIN);
        correctZoomVtAndValue(PICO, "Pico", SET_SPIN);

        XRCCTRL(*this, "StretchDotPico", wxCheckBox)->SetValue(conf[PICO].stretchDot_);

        setPrinterState(PICO);
        XRCCTRL(*this, "PrintModePico", wxChoice)->SetSelection(conf[PICO].printMode_);
        setPrintMode();

        if (clockTextCtrl[PICO] != NULL)
            clockTextCtrl[PICO]->ChangeValue(conf[PICO].clock_);
        wxString beepFrequency;
        beepFrequency.Printf("%d", conf[PICO].beepFrequency_);
        XRCCTRL(*this, "BeepFrequencyPico", wxTextCtrl)->ChangeValue(beepFrequency);

        XRCCTRL(*this, "TurboPico", wxCheckBox)->SetValue(conf[PICO].turbo_);
        turboGui("Pico");

        XRCCTRL(*this, "TurboClockPico", wxTextCtrl)->SetValue(conf[PICO].turboClock_);
        XRCCTRL(*this, "AutoCasLoadPico", wxCheckBox)->SetValue(conf[PICO].autoCassetteLoad_);
        setTapeType("Pico", PICO);

        XRCCTRL(*this, "VolumePico", wxSlider)->SetValue(conf[PICO].volume_);

        XRCCTRL(*this, "UseLocationPico", wxCheckBox)->SetValue(conf[PICO].useLoadLocation_);
        if (conf[PICO].saveStart_ != 0)
            XRCCTRL(*this, "SaveStartPico", wxTextCtrl)->SetValue(conf[PICO].saveStartString_);
    }

    setDiskTypePico(elfConfiguration[PICO].diskType);
    elfConfiguration[PICO].useHexKeyboard = false;
    elfConfiguration[PICO].useHexKeyboardEf3 = false;
    elfConfiguration[PICO].useKeyboard = false;
    elfConfiguration[PICO].UsePS2 = false;
    elfConfiguration[PICO].usePs2gpio = false;
    setRealCas(PICO);

    elfConfiguration[PICO].useLedModule = false;

    conf[PICO].loadFileNameFull_ = "";
    conf[PICO].loadFileName_ = "";

    conf[PICO].pLoadSaveName_[0] = 'S';
    conf[PICO].pLoadSaveName_[1] = 'U';
    conf[PICO].pLoadSaveName_[2] = 'P';
    conf[PICO].pLoadSaveName_[3] = 'E';

    conf[PICO].defus_ = 0x2a81;
    conf[PICO].eop_ = 0x2a83;
    conf[PICO].string_ = 0x2a92;
    conf[PICO].arrayValue_ = 0x2a94;
    conf[PICO].eod_ = 0x2a99;
    conf[PICO].basicRamAddress_ = 0x2c00;
}

void GuiPico::writePicoDirConfig()
{
    writeConfigDir("/Dir/Pico/Main", conf[PICO].mainDir_);
    writeConfigDir("/Dir/Pico/Main_Rom_1_File", conf[PICO].romDir_[MAINROM1]);
    writeConfigDir("/Dir/Pico/Font_Rom_File", conf[PICO].charRomDir_);
    writeConfigDir("/Dir/Pico/Vt100_Font_Rom_File", elfConfiguration[PICO].vt100CharRomDir_);
    writeConfigDir("/Dir/Pico/Vt52_Font_Rom_File", elfConfiguration[PICO].vt52CharRomDir_);
    writeConfigDir("/Dir/Pico/Software_File", conf[PICO].ramDir_);
    writeConfigDir("/Dir/Pico/Ide_File", conf[PICO].ideDir_);
    writeConfigDir("/Dir/Pico/Key_File", conf[PICO].keyFileDir_);
    writeConfigDir("/Dir/Pico/Print_File", conf[PICO].printFileDir_);
    writeConfigDir("/Dir/Pico/Video_Dump_File", conf[PICO].screenDumpFileDir_);
    writeConfigDir("/Dir/Pico/Wav_File", conf[PICO].wavFileDir_[0]);
    writeConfigDir("/Dir/Pico/Vt_Wav_File", elfConfiguration[PICO].vtWavFileDir_);
}
    
void GuiPico::writePicoConfig()
{
    wxString buffer;
        
    writeElfPortConfig(PICO, "Pico");

    configPointer->Write("Pico/Main_Rom_1_File", conf[PICO].rom_[MAINROM1]);
    configPointer->Write("Pico/Font_Rom_File", conf[PICO].charRom_);
    configPointer->Write("/Pico/Vt100_Font_Rom_File", elfConfiguration[PICO].vt100CharRom_);
    configPointer->Write("/Pico/Vt52_Font_Rom_File", elfConfiguration[PICO].vt52CharRom_);
    configPointer->Write("Pico/Ide_File", conf[PICO].ide_);
    configPointer->Write("Pico/Print_File", conf[PICO].printFile_);
    configPointer->Write("Pico/Video_Dump_File", conf[PICO].screenDumpFile_);
    configPointer->Write("Pico/Wav_File", conf[PICO].wavFile_[0]);
    configPointer->Write("Pico/Vt_Wav_File", elfConfiguration[PICO].vtWavFile_);
    configPointer->Write("Pico/VtSerialPortChoice", elfConfiguration[PICO].serialPort_);

    configPointer->Write("Pico/Disk_Type", elfConfiguration[PICO].diskType);
    configPointer->Write("Pico/Enable_Q_Sound", elfConfiguration[PICO].qSound_);
    configPointer->Write("Pico/VT_Type", elfConfiguration[PICO].vtType);

    long value = elfConfiguration[PICO].vt52SetUpFeature_.to_ulong();
    configPointer->Write("Pico/VT52Setup", value);
    value = elfConfiguration[PICO].vt100SetUpFeature_.to_ulong();
    configPointer->Write("Pico/VT100Setup", value);
    value = elfConfiguration[PICO].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write("Pico/VTExternalSetup", value);
    configPointer->Write("Pico/VT100CharPerRow", elfConfiguration[PICO].vtCharactersPerRow);
    configPointer->Write("Pico/VT100CharWidth", elfConfiguration[PICO].vt100CharWidth);
    configPointer->Write("Pico/VT52CharWidth", elfConfiguration[PICO].vt52CharWidth);

    configPointer->Write("Pico/Vt_Baud_Receive", elfConfiguration[PICO].baudR);
    configPointer->Write("Pico/Vt_Baud_Transmit", elfConfiguration[PICO].baudT);
    configPointer->Write("Pico/Bell_Frequency", elfConfiguration[PICO].bellFrequency_);
    configPointer->Write("Pico/SerialLog", elfConfiguration[PICO].serialLog);
    configPointer->Write("Pico/ESCError", elfConfiguration[PICO].escError);
    configPointer->Write("Pico/Uart", elfConfiguration[PICO].useUart);
    configPointer->Write("Pico/Uart16450", elfConfiguration[PICO].useUart16450);
    configPointer->Write("Pico/ClearRtc", elfConfiguration[PICO].clearRtc);
    configPointer->Write("Pico/Use_Real_Time_Clock", elfConfiguration[PICO].rtc);
    configPointer->Write("Pico/Video_Type", conf[PICO].videoMode_);
    configPointer->Write("Pico/Zoom", conf[PICO].zoom_);
    configPointer->Write("Pico/Vt_Zoom", conf[PICO].zoomVt_);
    configPointer->Write("Pico/UseLoadLocation", conf[PICO].useLoadLocation_);
    configPointer->Write("Pico/SaveStart", conf[PICO].saveStartString_);

    configPointer->Write("Pico/Enable_Printer", conf[PICO].printerOn_);
    configPointer->Write("Pico/Print_Mode", conf[PICO].printMode_);

    configPointer->Write("Pico/Enable_Vt_Stretch_Dot", conf[PICO].stretchDot_);
    configPointer->Write("Pico/Enable_Vt_External", elfConfiguration[PICO].vtExternal);

    configPointer->Write("Pico/Clock_Speed", conf[PICO].clock_);
    configPointer->Write("Pico/Beep_Frequency", conf[PICO].beepFrequency_);

    configPointer->Write("Pico/Enable_Turbo_Cassette", conf[PICO].turbo_);
    configPointer->Write("Pico/Turbo_Clock_Speed", conf[PICO].turboClock_);
    configPointer->Write("Pico/Enable_Auto_Cassette", conf[PICO].autoCassetteLoad_);
    configPointer->Write("Pico/Ymodem_PacketSize", elfConfiguration[PICO].packetSize);
    configPointer->Write("Pico/Volume", conf[PICO].volume_);
}

void GuiPico::readPicoWindowConfig()
{
    conf[PICO].pixieX_ = (int)configPointer->Read("Pico/Window_Position_Pixie_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[PICO].pixieY_ = (int)configPointer->Read("Pico/Window_Position_Pixie_Y", mainWindowY_);
    conf[PICO].tmsX_ = (int)configPointer->Read("Pico/Window_Position_Tms_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[PICO].tmsY_ = (int)configPointer->Read("Pico/Window_Position_Tms_Y", mainWindowY_);
    conf[PICO].mc6845X_ = (int)configPointer->Read("Pico/Window_Position_MC6845_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[PICO].mc6845Y_ = (int)configPointer->Read("Pico/Window_Position_MC6845_Y", mainWindowY_);
    conf[PICO].mc6847X_ = (int)configPointer->Read("Pico/Window_Position_MC6847_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[PICO].mc6847Y_ = (int)configPointer->Read("Pico/Window_Position_MC6847_Y", mainWindowY_);
    conf[PICO].i8275X_ = (int)configPointer->Read("Pico/Window_Position_I8275_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[PICO].i8275Y_ = (int)configPointer->Read("Pico/Window_Position_I8275_Y", mainWindowY_);
    conf[PICO].vtX_ = (int)configPointer->Read("Pico/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[PICO].vtY_ = (int)configPointer->Read("Pico/Window_Position_Vt_Y", mainWindowY_);
    conf[PICO].mainX_ = (int)configPointer->Read("Pico/Window_Position_X", mainWindowX_);
    conf[PICO].mainY_ = (int)configPointer->Read("Pico/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiPico::writePicoWindowConfig()
{
    if (conf[PICO].mainX_ > 0)
        configPointer->Write("Pico/MainX", conf[PICO].mainX_);
    if (conf[PICO].mainY_ > 0)
        configPointer->Write("Pico/MainY", conf[PICO].mainY_);
    if (conf[PICO].pixieX_ > 0)
        configPointer->Write("Pico/Window_Position_Pixie_X", conf[PICO].pixieX_);
    if (conf[PICO].pixieY_ > 0)
        configPointer->Write("Pico/Window_Position_Pixie_Y", conf[PICO].pixieY_);
    if (conf[PICO].tmsX_ > 0)
        configPointer->Write("Pico/Window_Position_Tms_X", conf[PICO].tmsX_);
    if (conf[PICO].tmsY_ > 0)
        configPointer->Write("Pico/Window_Position_Tms_Y", conf[PICO].tmsY_);
    if (conf[PICO].mc6845X_ > 0)
        configPointer->Write("Pico/Window_Position_MC6845_X", conf[PICO].mc6845X_);
    if (conf[PICO].mc6845Y_ > 0)
        configPointer->Write("Pico/Window_Position_MC6845_Y", conf[PICO].mc6845Y_);
    if (conf[PICO].mc6847X_ > 0)
        configPointer->Write("Pico/Window_Position_MC6847_X", conf[PICO].mc6847X_);
    if (conf[PICO].mc6847Y_ > 0)
        configPointer->Write("Pico/Window_Position_MC6847_Y", conf[PICO].mc6847Y_);
    if (conf[PICO].i8275X_ > 0)
        configPointer->Write("Pico/Window_Position_I8275_X", conf[PICO].i8275X_);
    if (conf[PICO].i8275Y_ > 0)
        configPointer->Write("Pico/Window_Position_I8275_Y", conf[PICO].i8275Y_);
    if (conf[PICO].vtX_ > 0)
        configPointer->Write("Pico/Window_Position_Vt_X", conf[PICO].vtX_);
    if (conf[PICO].vtY_ > 0)
        configPointer->Write("Pico/Window_Position_Vt_Y", conf[PICO].vtY_);
}

void GuiPico::onVideoTypePico(wxCommandEvent&event)
{
    setVideoTypePico(event.GetSelection());
}

void GuiPico::setVideoTypePico(int Selection)
{
    conf[PICO].videoMode_ = Selection;

    switch(Selection)
    {
        case VIDEONONE:
            elfConfiguration[PICO].usePixie = false;
            elfConfiguration[PICO].use6845 = false;
            elfConfiguration[PICO].useS100 = false;
            elfConfiguration[PICO].use6847 = false;
            elfConfiguration[PICO].useTMS9918 = false;
            elfConfiguration[PICO].use8275 = false;
            if (mode_.gui)
            {
                XRCCTRL(*this, "ZoomSpinPico", wxSpinButton)->Enable(false);
                XRCCTRL(*this, "ZoomValuePico", wxTextCtrl)->Enable(false);
                XRCCTRL(*this, "ZoomTextPico", wxStaticText)->Enable(false);
            }
        break;

        case VIDEOPIXIE:
            elfConfiguration[PICO].usePixie = true;
            elfConfiguration[PICO].use6845 = false;
            elfConfiguration[PICO].useS100 = false;
            elfConfiguration[PICO].use6847 = false;
            elfConfiguration[PICO].useTMS9918 = false;
            elfConfiguration[PICO].use8275 = false;
            if (mode_.gui)
            {
                XRCCTRL(*this, "ZoomSpinPico", wxSpinButton)->Enable(true);
                XRCCTRL(*this, "ZoomValuePico", wxTextCtrl)->Enable(true);
                XRCCTRL(*this, "ZoomTextPico", wxStaticText)->Enable(true);
            }
        break;

        case VIDEOTMSPICO:
            elfConfiguration[PICO].usePixie = false;
            elfConfiguration[PICO].use6845 = false;
            elfConfiguration[PICO].useS100 = false;
            elfConfiguration[PICO].use6847 = false;
            elfConfiguration[PICO].useTMS9918 = true;
            elfConfiguration[PICO].use8275 = false;
            if (mode_.gui)
            {
                XRCCTRL(*this, "ZoomSpinPico", wxSpinButton)->Enable(true);
                XRCCTRL(*this, "ZoomValuePico", wxTextCtrl)->Enable(true);
                XRCCTRL(*this, "ZoomTextPico", wxStaticText)->Enable(true);
            }
        break;
   }
}

void GuiPico::onDiskTypePico(wxCommandEvent&event)
{
    setDiskTypePico(event.GetSelection());
}


void GuiPico::setDiskTypePico(int Selection)
{
    elfConfiguration[PICO].diskType = Selection;

    switch(Selection)
    {
        case DISKNONE:
            elfConfiguration[PICO].ideEnabled = false;
            elfConfiguration[PICO].fdcEnabled = false;
            if (mode_.gui)
            {
                XRCCTRL(*this, "IDE_ButtonPico", wxButton)->Enable(false);
                XRCCTRL(*this, "IdeFilePico", wxTextCtrl)->Enable(false);
                XRCCTRL(*this, "Eject_IDEPico", wxButton)->Enable(false);
            }
        break;

        case DISKIDEPICO:
            elfConfiguration[PICO].ideEnabled = true;
            elfConfiguration[PICO].fdcEnabled = false;
            if (mode_.gui)
            {
                XRCCTRL(*this, "IDE_ButtonPico", wxButton)->Enable(true);
                XRCCTRL(*this, "IdeFilePico", wxTextCtrl)->Enable(true);
                XRCCTRL(*this, "Eject_IDEPico", wxButton)->Enable(true);
            }
        break;
    }
}

void GuiPico::onPicoRtc(wxCommandEvent&event)
{
    elfConfiguration[PICO].rtc = event.IsChecked();
    XRCCTRL(*this, "PicoClearRtc", wxCheckBox)->Enable(event.IsChecked());
}

void GuiPico::onPicoClearRtc(wxCommandEvent&event)
{
    elfConfiguration[PICO].clearRtc = event.IsChecked();
}


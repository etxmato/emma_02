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
#include "guimain.h"
#include "keymap.h"
#include "colour.h"
#include "ports.h"
#include "vtsetup.h"
#include "psave.h"
#include "wx/stdpaths.h"

BEGIN_EVENT_TABLE(GuiMain, wxFrame)

	EVT_MENU(XRCID("MI_Psave"), GuiMain::onPsaveMenu)
	EVT_MENU(XRCID("MI_FullScreenFloat"), GuiMain::onFullScreenFloat)

END_EVENT_TABLE()

GuiMain::GuiMain(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: wxFrame((wxFrame *)NULL, -1, title, pos, size) 
{
	configPointer = wxConfigBase::Get();
	dataDir_ = dataDir;
	iniDir_ = iniDir;
	mode_ = mode;

#if defined(__WXMAC__)
	if (mode_.portable)
		workingDir_ = wxGetCwd();
	else
		workingDir_ = wxStandardPaths::Get().GetDataDir();
	wxFileName applicationFile = wxFileName(workingDir_, "", wxPATH_NATIVE);
	pathSeparator_ = applicationFile.GetPathSeparator(wxPATH_NATIVE);
	if (mode_.portable)
		applicationDirectory_ = applicationFile.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	else
        applicationDirectory_ = wxStandardPaths::Get().GetResourcesDir() + pathSeparator_;
#else
	workingDir_ = wxGetCwd();
	wxFileName applicationFile = wxFileName(workingDir_, "", wxPATH_NATIVE);
	pathSeparator_ = applicationFile.GetPathSeparator(wxPATH_NATIVE);
	applicationDirectory_ = applicationFile.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
#endif
#if defined(__linux__)
    if (!wxFile::Exists(applicationDirectory_ + "main.xrc"))
    {
        applicationDirectory_ = wxStandardPaths::Get().GetExecutablePath();
        applicationDirectory_ = applicationDirectory_.Left(applicationDirectory_.Len()-11);
        applicationDirectory_ = applicationDirectory_ + "share" + pathSeparator_ + "emma_02" + pathSeparator_;
    }
#endif

    wxDir checkDirForFiles;
    bool dataDirEmpty = true;
    if (wxDir::Exists(dataDir_))
    {
        checkDirForFiles.Open(dataDir_);
        if (checkDirForFiles.HasFiles() || checkDirForFiles.HasSubDirs())
            dataDirEmpty = false;
    }
    
    if (dataDirEmpty)
    {
        if (!wxDir::Exists(dataDir_))
            wxDir::Make(dataDir_);
        
        int answer = wxMessageBox("1802 Software directory is empty, install default files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
        if (answer == wxYES)
            p_Main->reInstall(applicationDirectory_ + "data" + pathSeparator_, dataDir_, pathSeparator_);
    }
    
	playBlackBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/play_black.png", wxBITMAP_TYPE_PNG);
	playGreenBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/play_green.png", wxBITMAP_TYPE_PNG);

    recOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/rec_off.png", wxBITMAP_TYPE_PNG);
    recOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/rec_on.png", wxBITMAP_TYPE_PNG);
    
    pauseOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/pause_off.png", wxBITMAP_TYPE_PNG);
    pauseOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/pause_on.png", wxBITMAP_TYPE_PNG);
    
	realCasOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/real_cas_on.png", wxBITMAP_TYPE_PNG);
	realCasOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/real_cas_off.png", wxBITMAP_TYPE_PNG);

	printerOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/print_on.png", wxBITMAP_TYPE_PNG);
	printerOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/print_off.png", wxBITMAP_TYPE_PNG);

    ejectBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/eject.png", wxBITMAP_TYPE_PNG);

    tapeState_ = TAPE_STOP;
	turboOn_ = false;
	zoomTextValueChanged_ = false;
    
    position_[ELF].x = 0;
    position_[ELFII].x = 0;
    position_[SUPERELF].x = 0;
}

void GuiMain::readElfPortConfig(int elfType, wxString elfTypeStr)
{
    elfConfiguration[elfType].elfPortConf.ef1default = (int)configPointer->Read("/" + elfTypeStr + "/UnusedEf1", 1l);
    elfConfiguration[elfType].elfPortConf.ef2default = (int)configPointer->Read(elfTypeStr + "/UnusedEf2", 1l);
    if (elfType == ELF2K)
        elfConfiguration[elfType].elfPortConf.ef3default = (int)configPointer->Read(elfTypeStr + "/UnusedEf3", 0l);
    else
        elfConfiguration[elfType].elfPortConf.ef3default = (int)configPointer->Read(elfTypeStr + "/UnusedEf3", 1l);
    
    elfConfiguration[elfType].elfPortConf.pixieInput = (int)configPointer->Read(elfTypeStr + "/PixieInput", 1l);
    elfConfiguration[elfType].elfPortConf.pixieOutput = (int)configPointer->Read(elfTypeStr + "/PixieOutput", 1l);
    elfConfiguration[elfType].elfPortConf.pixieEf = (int)configPointer->Read(elfTypeStr + "/PixieEF", 1l);
    
    elfConfiguration[elfType].elfPortConf.portExtenderSelectOutput = (int)configPointer->Read(elfTypeStr + "/PortExtenderSelectOutput", 1l);
    elfConfiguration[elfType].elfPortConf.portExtenderWriteOutput = (int)configPointer->Read(elfTypeStr + "/PortExtenderWriteOutput", 1l);
    elfConfiguration[elfType].elfPortConf.portExtenderInput = (int)configPointer->Read(elfTypeStr + "/PortExtenderInput", 1l);
    
    elfConfiguration[elfType].elfPortConf.ideSelectOutput = (int)configPointer->Read(elfTypeStr + "/IdeSelectOutput", 2l);
    elfConfiguration[elfType].elfPortConf.ideWriteOutput = (int)configPointer->Read(elfTypeStr + "/IdeWriteOutput", 3l);
    elfConfiguration[elfType].elfPortConf.ideStatus = (int)configPointer->Read(elfTypeStr + "/IdeStatus", 2l);
    elfConfiguration[elfType].elfPortConf.ideInput = (int)configPointer->Read(elfTypeStr + "/IdeInput", 3l);
    elfConfiguration[elfType].elfPortConf.ideTracks = (int)configPointer->Read(elfTypeStr + "/IdeTracks", 512l);
    elfConfiguration[elfType].elfPortConf.ideHeads = (int)configPointer->Read(elfTypeStr + "/IdeHeads", 4l);
    elfConfiguration[elfType].elfPortConf.ideSectors = (int)configPointer->Read(elfTypeStr + "/IdeSectors", 26l);
  
    elfConfiguration[elfType].elfPortConf.fdcSelectOutput = (int)configPointer->Read(elfTypeStr + "/FdcSelectOutput", 2l);
    elfConfiguration[elfType].elfPortConf.fdcWriteOutput = (int)configPointer->Read(elfTypeStr + "/FdcWriteOutput", 3l);
    elfConfiguration[elfType].elfPortConf.fdcInput = (int)configPointer->Read(elfTypeStr + "/FdcInput", 3l);
    elfConfiguration[elfType].elfPortConf.fdcEf = (int)configPointer->Read(elfTypeStr + "/FdcEf", 2l);

    elfConfiguration[elfType].elfPortConf.keyboardInput = (int)configPointer->Read(elfTypeStr+"/KeyboardInput", 7l);
    elfConfiguration[elfType].elfPortConf.keyboardEf = (int)configPointer->Read(elfTypeStr+"/KeyboardEfi", 3l);

    elfConfiguration[elfType].elfPortConf.ps2KeyboardInput = (int)configPointer->Read(elfTypeStr+"/Ps2KeyboardInput", 7l);
    elfConfiguration[elfType].elfPortConf.ps2KeyboardOutput = (int)configPointer->Read(elfTypeStr+"/Ps2KeyboardOutput", 7l);
    elfConfiguration[elfType].elfPortConf.ps2KeyboardEf = (int)configPointer->Read(elfTypeStr+"/Ps2KeyboardEf", 3l);

	elfConfiguration[elfType].elfPortConf.printerEf = (int)configPointer->Read(elfTypeStr + "/PrinterEf", 0l);
	elfConfiguration[elfType].elfPortConf.printerOutput = (int)configPointer->Read(elfTypeStr + "/PrinterOutput", 7l);

    elfConfiguration[elfType].elfPortConf.emsOutput = (int)configPointer->Read(elfTypeStr+"/EmsOutput", 7l);

    elfConfiguration[elfType].elfPortConf.vt100Output = (int)configPointer->Read(elfTypeStr+"/Vt100Output", 7l);
    if (elfTypeStr != "Elf2K")
    	elfConfiguration[elfType].elfPortConf.vt100Ef = (int)configPointer->Read(elfTypeStr+"/Vt100Ef", 2l);
    else
    	elfConfiguration[elfType].elfPortConf.vt100Ef = (int)configPointer->Read(elfTypeStr+"/Vt100Ef", 3l);
    elfConfiguration[elfType].elfPortConf.vt100ReverseEf = (int)configPointer->Read(elfTypeStr+"/Vt100ReverseEf", 1l);
    elfConfiguration[elfType].elfPortConf.vt100ReverseQ = (int)configPointer->Read(elfTypeStr+"/Vt100ReverseQ", 0l);

    elfConfiguration[elfType].elfPortConf.uartOut = (int)configPointer->Read(elfTypeStr+"/UartOut", 2l);
    elfConfiguration[elfType].elfPortConf.uartIn = (int)configPointer->Read(elfTypeStr+"/UartIn", 2l);
    elfConfiguration[elfType].elfPortConf.uartControl = (int)configPointer->Read(elfTypeStr+"/UartControl", 3l);
    elfConfiguration[elfType].elfPortConf.uartStatus = (int)configPointer->Read(elfTypeStr+"/UartStatus", 3l);

    elfConfiguration[elfType].elfPortConf.tmsModeHighOutput = (int)configPointer->Read(elfTypeStr+"/TmsModeHighOutput", 5l);
    elfConfiguration[elfType].elfPortConf.tmsModeLowOutput = (int)configPointer->Read(elfTypeStr+"/TmsModeLowOutput", 6l);

    elfConfiguration[elfType].elfPortConf.i8275WriteCommand = (int)configPointer->Read(elfTypeStr+"/I8275WriteCommand", 5l);
    elfConfiguration[elfType].elfPortConf.i8275ReadStatus = (int)configPointer->Read(elfTypeStr+"/I8275ReadStatus", 5l);
    elfConfiguration[elfType].elfPortConf.i8275WriteParameter = (int)configPointer->Read(elfTypeStr+"/I8275WriteParameter", 1l);
    elfConfiguration[elfType].elfPortConf.i8275ReadParameter = (int)configPointer->Read(elfTypeStr+"/I8275ReadParameter", 1l);
    elfConfiguration[elfType].elfPortConf.i8275VerticalRetrace = (int)configPointer->Read(elfTypeStr+"/I8275VerticalRetrace", 1l);

    elfConfiguration[elfType].elfPortConf.led_Module_Output = (int)configPointer->Read(elfTypeStr+"/Led_Module_Output", 4l);

    elfConfiguration[elfType].elfPortConf.mc6847Output = (int)configPointer->Read(elfTypeStr+"/MC6847Output", 5l);
    elfConfiguration[elfType].elfPortConf.mc6847b7 = (int)configPointer->Read(elfTypeStr+"/MC6847-B7", 0l);
    elfConfiguration[elfType].elfPortConf.mc6847b6 = (int)configPointer->Read(elfTypeStr+"/MC6847-B6", 0l);
    elfConfiguration[elfType].elfPortConf.mc6847b5 = (int)configPointer->Read(elfTypeStr+"/MC6847-B5", 0l);
    elfConfiguration[elfType].elfPortConf.mc6847b4 = (int)configPointer->Read(elfTypeStr+"/MC6847-B4", 0l);
    elfConfiguration[elfType].elfPortConf.mc6847b3 = (int)configPointer->Read(elfTypeStr+"/MC6847-B3", 3l);
    elfConfiguration[elfType].elfPortConf.mc6847b2 = (int)configPointer->Read(elfTypeStr+"/MC6847-B2", 4l);
    elfConfiguration[elfType].elfPortConf.mc6847b1 = (int)configPointer->Read(elfTypeStr+"/MC6847-B1", 6l);
    elfConfiguration[elfType].elfPortConf.mc6847b0 = (int)configPointer->Read(elfTypeStr+"/MC6847-B0", 5l);
    elfConfiguration[elfType].elfPortConf.mc6847dd7 = (int)configPointer->Read(elfTypeStr+"/MC6847-DD7", 1l);
    elfConfiguration[elfType].elfPortConf.mc6847dd6 = (int)configPointer->Read(elfTypeStr+"/MC6847-DD6", 0l);

    elfConfiguration[elfType].elfPortConf.mc6847StartRam = (int)configPointer->Read(elfTypeStr+"/mc6847StartRam", 0xe000l);
    elfConfiguration[elfType].elfPortConf.mc6847EndRam = (int)configPointer->Read(elfTypeStr+"/mc6847EndRam", 0xe3ffl);

    elfConfiguration[elfType].elfPortConf.mc6845StartRam = (int)configPointer->Read(elfTypeStr+"/mc6845StartRam", 0xe000l);
    elfConfiguration[elfType].elfPortConf.mc6845EndRam = (int)configPointer->Read(elfTypeStr+"/mc6845EndRam", 0xe7ffl);
    elfConfiguration[elfType].elfPortConf.mc6845Address = (int)configPointer->Read(elfTypeStr+"/mc6845Address", 0xe800l);
    elfConfiguration[elfType].elfPortConf.mc6845Data = (int)configPointer->Read(elfTypeStr+"/mc6845Data", 0xe801l);
    elfConfiguration[elfType].elfPortConf.mc6845Ef = (int)configPointer->Read(elfTypeStr+"/mc6845Ef", 2l);

    elfConfiguration[elfType].elfPortConf.hexOutput = (int)configPointer->Read(elfTypeStr+"/HexOutput", 4l);
    elfConfiguration[elfType].elfPortConf.hexInput = (int)configPointer->Read(elfTypeStr+"/HexInput", 4l);
    elfConfiguration[elfType].elfPortConf.hexEf = (int)configPointer->Read(elfTypeStr+"/HexEf", 3l);
    
    elfConfiguration[elfType].elfPortConf.tapeEf = (int)configPointer->Read(elfTypeStr+"/TapeEf", 2l);
}

void GuiMain::writeElfPortConfig(int elfType, wxString elfTypeStr)
{
    configPointer->Write(elfTypeStr + "/UnusedEf1", elfConfiguration[elfType].elfPortConf.ef1default);
    configPointer->Write(elfTypeStr + "/UnusedEf2", elfConfiguration[elfType].elfPortConf.ef2default);
    configPointer->Write(elfTypeStr + "/UnusedEf3", elfConfiguration[elfType].elfPortConf.ef3default);
    
    configPointer->Write(elfTypeStr + "/PixieInput", elfConfiguration[elfType].elfPortConf.pixieInput);
    configPointer->Write(elfTypeStr + "/PixieOutput", elfConfiguration[elfType].elfPortConf.pixieOutput);
    configPointer->Write(elfTypeStr + "/PixieEF", elfConfiguration[elfType].elfPortConf.pixieEf);
    
    configPointer->Write(elfTypeStr + "/PortExtenderSelectOutput", elfConfiguration[elfType].elfPortConf.portExtenderSelectOutput);
    configPointer->Write(elfTypeStr + "/PortExtenderWriteOutput", elfConfiguration[elfType].elfPortConf.portExtenderWriteOutput);
    configPointer->Write(elfTypeStr + "/PortExtenderInput", elfConfiguration[elfType].elfPortConf.portExtenderInput);
    
    configPointer->Write(elfTypeStr + "/IdeSelectOutput", elfConfiguration[elfType].elfPortConf.ideSelectOutput);
    configPointer->Write(elfTypeStr + "/IdeWriteOutput", elfConfiguration[elfType].elfPortConf.ideWriteOutput);
    configPointer->Write(elfTypeStr + "/IdeStatus", elfConfiguration[elfType].elfPortConf.ideStatus);
    configPointer->Write(elfTypeStr + "/IdeInput", elfConfiguration[elfType].elfPortConf.ideInput);
    configPointer->Write(elfTypeStr + "/IdeTracks", elfConfiguration[elfType].elfPortConf.ideTracks);
    configPointer->Write(elfTypeStr + "/IdeHeads", elfConfiguration[elfType].elfPortConf.ideHeads);
    configPointer->Write(elfTypeStr + "/IdeSectors", elfConfiguration[elfType].elfPortConf.ideSectors);
   
    configPointer->Write(elfTypeStr + "/FdcSelectOutput", elfConfiguration[elfType].elfPortConf.fdcSelectOutput);
    configPointer->Write(elfTypeStr + "/FdcWriteOutput", elfConfiguration[elfType].elfPortConf.fdcWriteOutput);
    configPointer->Write(elfTypeStr + "/FdcInput", elfConfiguration[elfType].elfPortConf.fdcInput);
    configPointer->Write(elfTypeStr + "/FdcEf", elfConfiguration[elfType].elfPortConf.fdcEf);
    
    configPointer->Write(elfTypeStr+"/KeyboardInput", elfConfiguration[elfType].elfPortConf.keyboardInput);
    configPointer->Write(elfTypeStr+"/KeyboardEfi", elfConfiguration[elfType].elfPortConf.keyboardEf);

    configPointer->Write(elfTypeStr+"/Ps2KeyboardInput", elfConfiguration[elfType].elfPortConf.ps2KeyboardInput);
    configPointer->Write(elfTypeStr+"/Ps2KeyboardOutput", elfConfiguration[elfType].elfPortConf.ps2KeyboardOutput);
    configPointer->Write(elfTypeStr+"/Ps2KeyboardEf", elfConfiguration[elfType].elfPortConf.ps2KeyboardEf);

	configPointer->Write(elfTypeStr + "/PrinterEf", elfConfiguration[elfType].elfPortConf.printerEf);
	configPointer->Write(elfTypeStr + "/PrinterOutput", elfConfiguration[elfType].elfPortConf.printerOutput);

    configPointer->Write(elfTypeStr+"/EmsOutput", elfConfiguration[elfType].elfPortConf.emsOutput);

    configPointer->Write(elfTypeStr+"/Vt100Output", elfConfiguration[elfType].elfPortConf.vt100Output);
    configPointer->Write(elfTypeStr+"/Vt100Ef", elfConfiguration[elfType].elfPortConf.vt100Ef);
    configPointer->Write(elfTypeStr+"/Vt100ReverseEf", elfConfiguration[elfType].elfPortConf.vt100ReverseEf);
    configPointer->Write(elfTypeStr+"/Vt100ReverseQ", elfConfiguration[elfType].elfPortConf.vt100ReverseQ);
    configPointer->Write(elfTypeStr+"/UartOut", elfConfiguration[elfType].elfPortConf.uartOut);
    configPointer->Write(elfTypeStr+"/UartIn", elfConfiguration[elfType].elfPortConf.uartIn);
    configPointer->Write(elfTypeStr+"/UartControl", elfConfiguration[elfType].elfPortConf.uartControl);
    configPointer->Write(elfTypeStr+"/UartStatus", elfConfiguration[elfType].elfPortConf.uartStatus);
    configPointer->Write(elfTypeStr+"/TmsModeHighOutput", elfConfiguration[elfType].elfPortConf.tmsModeHighOutput);
    configPointer->Write(elfTypeStr+"/TmsModeLowOutput", elfConfiguration[elfType].elfPortConf.tmsModeLowOutput);
    configPointer->Write(elfTypeStr+"/I8275WriteCommand", elfConfiguration[elfType].elfPortConf.i8275WriteCommand);
    configPointer->Write(elfTypeStr+"/I8275ReadStatus", elfConfiguration[elfType].elfPortConf.i8275ReadStatus);
    configPointer->Write(elfTypeStr+"/I8275WriteParameter", elfConfiguration[elfType].elfPortConf.i8275WriteParameter);
    configPointer->Write(elfTypeStr+"/I8275ReadParameter", elfConfiguration[elfType].elfPortConf.i8275ReadParameter);
    configPointer->Write(elfTypeStr+"/I8275VerticalRetrace", elfConfiguration[elfType].elfPortConf.i8275VerticalRetrace);
    configPointer->Write(elfTypeStr+"/Led_Module_Output", elfConfiguration[elfType].elfPortConf.led_Module_Output);

    configPointer->Write(elfTypeStr+"/MC6847Output", elfConfiguration[elfType].elfPortConf.mc6847Output);
    configPointer->Write(elfTypeStr+"/MC6847-B7", elfConfiguration[elfType].elfPortConf.mc6847b7);
    configPointer->Write(elfTypeStr+"/MC6847-B6", elfConfiguration[elfType].elfPortConf.mc6847b6);
    configPointer->Write(elfTypeStr+"/MC6847-B5", elfConfiguration[elfType].elfPortConf.mc6847b5);
    configPointer->Write(elfTypeStr+"/MC6847-B4", elfConfiguration[elfType].elfPortConf.mc6847b4);
    configPointer->Write(elfTypeStr+"/MC6847-B3", elfConfiguration[elfType].elfPortConf.mc6847b3);
    configPointer->Write(elfTypeStr+"/MC6847-B2", elfConfiguration[elfType].elfPortConf.mc6847b2);
    configPointer->Write(elfTypeStr+"/MC6847-B1", elfConfiguration[elfType].elfPortConf.mc6847b1);
    configPointer->Write(elfTypeStr+"/MC6847-B0", elfConfiguration[elfType].elfPortConf.mc6847b0);
    configPointer->Write(elfTypeStr+"/MC6847-DD7", elfConfiguration[elfType].elfPortConf.mc6847dd7);
    configPointer->Write(elfTypeStr+"/MC6847-DD6", elfConfiguration[elfType].elfPortConf.mc6847dd6);

    configPointer->Write(elfTypeStr+"/mc6847StartRam", elfConfiguration[elfType].elfPortConf.mc6847StartRam);
    configPointer->Write(elfTypeStr+"/mc6847EndRam", elfConfiguration[elfType].elfPortConf.mc6847EndRam);

    configPointer->Write(elfTypeStr+"/mc6845StartRam", elfConfiguration[elfType].elfPortConf.mc6845StartRam);
    configPointer->Write(elfTypeStr+"/mc6845EndRam", elfConfiguration[elfType].elfPortConf.mc6845EndRam);
    configPointer->Write(elfTypeStr+"/mc6845Address", elfConfiguration[elfType].elfPortConf.mc6845Address);
    configPointer->Write(elfTypeStr+"/mc6845Data", elfConfiguration[elfType].elfPortConf.mc6845Data);
    configPointer->Write(elfTypeStr+"/mc6845Ef", elfConfiguration[elfType].elfPortConf.mc6845Ef);

    configPointer->Write(elfTypeStr+"/HexOutput", elfConfiguration[elfType].elfPortConf.hexOutput);
    configPointer->Write(elfTypeStr+"/HexInput", elfConfiguration[elfType].elfPortConf.hexInput);
    configPointer->Write(elfTypeStr+"/HexEf", elfConfiguration[elfType].elfPortConf.hexEf);
    
    configPointer->Write(elfTypeStr+"/TapeEf", elfConfiguration[elfType].elfPortConf.tapeEf);
}

wxString GuiMain::readConfigDir(const wxString& key, const wxString& defVal)
{
    wxString returnDir = configPointer->Read(key, defVal);
    
    if (returnDir == "")
        returnDir = dataDir_ + returnDir;

    wxDir directory;
    if (!directory.Open(returnDir))
        returnDir = dataDir_ + returnDir;
    
//	if (returnDir.Left(1) != "/" && returnDir.Left(2) != "C:")
//        returnDir = dataDir_ + returnDir;
   
    return returnDir;
}

void GuiMain::writeConfigDir(const wxString& key, const wxString& value)
{
    wxString tempStr = value;
    tempStr.Replace(dataDir_, "");
    configPointer->Write(key, tempStr);
}

void GuiMain::onMainRom1(wxCommandEvent& WXUNUSED(event))
{
	wxString fileName;

	fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" ROM file to load",
                               conf[selectedComputer_].romDir_[MAINROM1], XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                   "Binary File|*.bin;*.rom;*.ram;*.cos;*.arc|Intel Hex File|*.hex|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[selectedComputer_].romDir_[MAINROM1] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].rom_[MAINROM1] = FullPath.GetFullName();

	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[MAINROM1]);
}

void GuiMain::onMainRom1Text(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].rom_[MAINROM1] = XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiMain::onMainRom2(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" ROM or RAM file to load",
                               conf[selectedComputer_].romDir_[MAINROM2], XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                   "Binary File|*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[selectedComputer_].romDir_[MAINROM2] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].rom_[MAINROM2] = FullPath.GetFullName();

	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[MAINROM2]);
}

void GuiMain::onMainRom2Text(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].rom_[MAINROM2] = XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiMain::onMainRom3(wxCommandEvent& WXUNUSED(event))
{
	wxString fileName;

	fileName = wxFileSelector("Select the " + computerInfo[selectedComputer_].name + " ROM or RAM file to load",
		conf[selectedComputer_].romDir_[MAINROM2], XRCCTRL(*this, "MainRom3" + computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
		"",
		wxString::Format
		(
			"Binary File|*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s",
			wxFileSelectorDefaultWildcardStr,
			wxFileSelectorDefaultWildcardStr
			),
		wxFD_OPEN | wxFD_CHANGE_DIR | wxFD_PREVIEW,
		this
		);
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[selectedComputer_].romDir_[MAINROM3] = FullPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].rom_[MAINROM3] = FullPath.GetFullName();

	XRCCTRL(*this, "MainRom3" + computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[MAINROM3]);
}

void GuiMain::onMainRom3Text(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].rom_[MAINROM3] = XRCCTRL(*this, "MainRom3" + computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiMain::onCartRom(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" Cartridge ROM file to load",
                               conf[selectedComputer_].romDir_[CARTROM], XRCCTRL(*this, "CartRom"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
                               "st2",
                               wxString::Format
                              (
                                   "ST2 File (*.st2)|*.st2|Chip 8 File|*.c8;*.ch8;|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[selectedComputer_].romDir_[CARTROM] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].rom_[CARTROM] = FullPath.GetFullName();

	XRCCTRL(*this, "CartRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].rom_[CARTROM]);
}

void GuiMain::onCartRomText(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].rom_[CARTROM] = XRCCTRL(*this, "CartRom"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiMain::onRamSW(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the RAM software file to load",
                               conf[selectedComputer_].ramDir_, XRCCTRL(*this, "RamSW"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
                               "",
                               wxString::Format
                              (
                                   "Binary File|*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[selectedComputer_].ramDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].ram_ = FullPath.GetFullName();

	XRCCTRL(*this, "RamSW"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].ram_);
	
	if (selectedComputer_ == FRED1 || selectedComputer_ == FRED2)
        p_Main->setGameId(conf[selectedComputer_].ram_);

	if (selectedComputer_ == FRED2)
        p_Main->checkGameFred2(conf[selectedComputer_].ram_);
}

void GuiMain::onRamSWText(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].ram_ = XRCCTRL(*this, "RamSW"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();

	if (selectedComputer_ == FRED1 && conf[FRED1].gameId_ != -1)
        p_Main->setGameId(conf[selectedComputer_].ram_);

	if (selectedComputer_ == FRED2 && conf[FRED2].gameId_ != -1)
        p_Main->checkGameFred2(conf[selectedComputer_].ram_);
}

void GuiMain::onChip8SW(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	wxString mainText = "Select the Chip 8 software file to load";
	if (selectedComputer_ == ETI)
		mainText = "Select the RAM or Chip 8 software file to load";

	fileName = wxFileSelector( mainText,
                               conf[selectedComputer_].chip8SWDir_, XRCCTRL(*this, "Chip8SW"+computerInfo[selectedComputer_].gui, wxTextCtrl)->GetValue(),
                               "",
                               wxString::Format
                              (
                                   "Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10|Intel Hex File|*.hex|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[selectedComputer_].chip8SWDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].chip8SW_ = FullPath.GetFullName();

	XRCCTRL(*this, "Chip8SW"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue(conf[selectedComputer_].chip8SW_);
}

void GuiMain::onChip8SWText(wxCommandEvent& event)
{
	conf[selectedComputer_].chip8SW_ = event.GetString();
}

void GuiMain::onEjectChip8SW(wxCommandEvent& WXUNUSED(event) )
{
	conf[selectedComputer_].chip8SW_ = "";
	XRCCTRL(*this, "Chip8SW"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue(conf[selectedComputer_].chip8SW_);
}

void GuiMain::onPrintFile(wxCommandEvent& WXUNUSED(event))
{
 	wxString fileName;

	fileName = wxFileSelector( "Select the Printer Output File",
                               conf[selectedComputer_].printFileDir_, XRCCTRL(*this, "PrintFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->GetValue(),
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
	conf[selectedComputer_].printFileDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].printFile_ = FullPath.GetFullName();

	XRCCTRL(*this, "PrintFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue(conf[selectedComputer_].printFile_);
}

void GuiMain::onPrintFileText(wxCommandEvent&event)
{
	wxString printFile;

	conf[selectedComputer_].printFile_ = event.GetString();

	if (runningComputer_ == selectedComputer_)
	{
		if (conf[selectedComputer_].printFile_.Len() == 0)
			p_Printer->setPrintfileName("");
		else
			p_Printer->setPrintfileName(conf[selectedComputer_].printFileDir_ + conf[selectedComputer_].printFile_);
	}
}

void GuiMain::onPrintButton(wxCommandEvent&WXUNUSED(event))
{
	if (selectedComputer_ == VIP || selectedComputer_ == VIPII || selectedComputer_ == VELF ||selectedComputer_ == ELF || selectedComputer_ == ELFII || selectedComputer_ == SUPERELF)
	{
		if (!computerRunning_ || (selectedComputer_ != runningComputer_))
		{
			conf[selectedComputer_].printerOn_ = !conf[selectedComputer_].printerOn_;
			setPrinterState(selectedComputer_);
			return;
		}
	}
	onF4();
}

void GuiMain::onF4()
{
	if (computerRunning_)
	{
		p_Printer->onF4();
		if (conf[runningComputer_].printMode_ == PRINTFILE)
		{
			conf[runningComputer_].printMode_ = PRINTWINDOW;
			if (mode_.gui)
			{
				XRCCTRL(*this, "PrintMode"+computerInfo[runningComputer_].gui, wxChoice)->SetSelection(conf[runningComputer_].printMode_);
				XRCCTRL(*this, "PrintFileButton"+computerInfo[runningComputer_].gui, wxButton)->Enable(false);
				XRCCTRL(*this, "PrintFile"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(false);
			}
			p_Printer->setPrintMode(conf[selectedComputer_].printMode_);
		}
	}
}

void GuiMain::onPrintMode(wxCommandEvent&event)
{
	conf[selectedComputer_].printMode_ = event.GetSelection();
	setPrintMode();
	
	if (runningComputer_ == selectedComputer_)
		p_Printer->setPrintMode(conf[selectedComputer_].printMode_);
}

void GuiMain::setPrintMode()
{
	if (!mode_.gui)  return;
	if (conf[selectedComputer_].printMode_ == PRINTFILE)
	{
		XRCCTRL(*this, "PrintFileButton"+computerInfo[selectedComputer_].gui, wxButton)->Enable(true);
		XRCCTRL(*this, "PrintFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->Enable(true);
	}
	else
	{
		XRCCTRL(*this, "PrintFileButton"+computerInfo[selectedComputer_].gui, wxButton)->Enable(false);
		XRCCTRL(*this, "PrintFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->Enable(false);
	}
}

int GuiMain::getPrintMode()
{
	return conf[runningComputer_].printMode_;
}

void GuiMain::openPrinterFrame(wxCommandEvent&WXUNUSED(event))
{
	onF4();
}

void GuiMain::onIde(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the IDE file to load",
                               conf[selectedComputer_].ideDir_, XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->GetValue(),
                               "ide",
                               wxString::Format
                              (
                                    "IDE Image (*.ide)|*.ide|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[selectedComputer_].ideDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].ide_ = FullPath.GetFullName();

	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue(conf[selectedComputer_].ide_);
}

void GuiMain::onIdeEject(wxCommandEvent& WXUNUSED(event) )
{
	conf[selectedComputer_].ide_ = "";
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue(conf[selectedComputer_].ide_);
}

void GuiMain::onIdeText(wxCommandEvent& event)
{
	conf[selectedComputer_].ide_ = event.GetString();
}

void GuiMain::onCharRom(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the Character Font file to load",
                               conf[selectedComputer_].charRomDir_, XRCCTRL(*this, "CharRom"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
                               "bin",
                               wxString::Format
                              (
                                    "Binary Font File|*.bin;*.rom|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[selectedComputer_].charRomDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].charRom_ = FullPath.GetFullName();

	XRCCTRL(*this, "CharRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].charRom_);
}

void GuiMain::onCharRomText(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].charRom_ = XRCCTRL(*this, "CharRom"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiMain::onVtCharRom(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the VT Character Font file to load",
                               conf[selectedComputer_].vtCharRomDir_, XRCCTRL(*this, "VtCharRom"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
                               "bin",
                               wxString::Format
                              (
                                    "Binary Font File|*.bin;*.rom|All files (%s)|%s",
                                    wxFileSelectorDefaultWildcardStr,
                                    wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[selectedComputer_].vtCharRomDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].vtCharRom_ = FullPath.GetFullName();

	XRCCTRL(*this, "VtCharRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].vtCharRom_);
}

void GuiMain::onVtCharRomText(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].vtCharRom_ = XRCCTRL(*this, "VtCharRom"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiMain::onKeyFile(wxCommandEvent& WXUNUSED(event) )
{
	wxString fileName;

	fileName = wxFileSelector( "Select the Key file to load",
                               conf[selectedComputer_].keyFileDir_, XRCCTRL(*this, "KeyFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->GetValue(),
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
	conf[selectedComputer_].keyFileDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].keyFile_ = FullPath.GetFullName();

	XRCCTRL(*this, "KeyFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue(conf[selectedComputer_].keyFile_);
}

void GuiMain::onKeyFileText(wxCommandEvent&WXUNUSED(event))
{
	conf[selectedComputer_].keyFile_ = XRCCTRL(*this, "KeyFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->GetValue();
}

void GuiMain::onKeyFileEject(wxCommandEvent& WXUNUSED(event) )
{
	XRCCTRL(*this, "KeyFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	conf[selectedComputer_].keyFile_ = "";
	if (selectedComputer_ != runningComputer_)
		return;
	if (computerRunning_)
	{
		switch(runningComputer_)
		{
			case ELF:
				p_Elf->closeElfKeyFile();
			break;

			case ELFII:
				p_Elf2->closeElfKeyFile();
			break;

			case SUPERELF:
				p_Super->closeElfKeyFile();
			break;

			case ELF2K:
				p_Elf2K->closePs2gpioKeyFile();
			break;
                
            case PECOM:
				p_Pecom->closePecomKeyFile();
			break;

			case COMX:
				p_Comx->closeComxKeyFile();
			break;

			case TMC600:
				p_Tmc600->closeTelmacKeyFile();
			break;
		}
	}
}

void GuiMain::onVT100(wxCommandEvent&event)
{
	int Selection = event.GetSelection();
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, Selection, true);

	if (Selection != VTNONE)
	{
		if (selectedComputer_ == ELF2K)
		{
			XRCCTRL(*this, "Elf2KVideoType", wxChoice)->SetSelection(VIDEONONE);
			p_Main->setElf2KVideoType(VIDEONONE);
		}
	}
}

void GuiMain::setVtType(wxString elfTypeStr, int elfType, int Selection, bool GuiChange)
{
	elfConfiguration[elfType].vtType = Selection;

    if (elfConfiguration[selectedComputer_].vtExternal & !GuiChange)
        Selection = EXTERNAL_TERMINAL;
    
	switch(Selection)
	{
		case VTNONE:
			if (mode_.gui)
			{
                XRCCTRL(*this, "VTBaudRChoice"+elfTypeStr, wxChoice)->Enable(false);
                XRCCTRL(*this, "VTBaudTChoice"+elfTypeStr, wxChoice)->Enable(false);
                XRCCTRL(*this, "VTBaudRText"+elfTypeStr, wxStaticText)->Enable(false);
                XRCCTRL(*this, "VTBaudTText"+elfTypeStr, wxStaticText)->Enable(false);
                XRCCTRL(*this, "VtSetup"+elfTypeStr, wxButton)->Enable(false);
                if (elfType == ELF || elfType == ELFII || elfType == SUPERELF)
                {
                    XRCCTRL(*this, "Qsound"+elfTypeStr, wxChoice)->Enable(true);
                    XRCCTRL(*this, "QsoundText"+elfTypeStr, wxStaticText)->Enable(true);
                }

                XRCCTRL(*this, "ZoomTextVt"+elfTypeStr, wxStaticText)->Enable(false);
				XRCCTRL(*this, "VtCharRomButton"+elfTypeStr, wxButton)->Enable(false);
				XRCCTRL(*this, "VtCharRom"+elfTypeStr, wxComboBox)->Enable(false);
				XRCCTRL(*this, "ZoomSpinVt"+elfTypeStr, wxSpinButton)->Enable(false);
				XRCCTRL(*this, "ZoomValueVt"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this, "StretchDot"+elfTypeStr, wxCheckBox)->Enable(false);
			}
            elfConfiguration[selectedComputer_].vtExternal = false;
		break;

		case VT52:
			if (elfType == COSMICOS || elfType == ELF2K || elfType == MS2000 || elfType == MEMBER || elfType == VIP || elfType == VELF)
				conf[elfType].vtCharRomDir_ = dataDir_ + elfTypeStr + pathSeparator_;
			else
                if (elfType == MCDS)
                    conf[elfType].vtCharRomDir_ = dataDir_ + "MCDS" + pathSeparator_;
                else
                    conf[elfType].vtCharRomDir_ = dataDir_ + "Elf" + pathSeparator_;
			conf[elfType].vtCharRom_ = "vt52.a.bin";
			if (mode_.gui)
			{
				XRCCTRL(*this, "VTBaudRChoice"+elfTypeStr, wxChoice)->Enable(elfConfiguration[elfType].useUart);
				XRCCTRL(*this, "VTBaudTChoice"+elfTypeStr, wxChoice)->Enable(true);
                XRCCTRL(*this, "VTBaudRText"+elfTypeStr, wxStaticText)->Enable(elfConfiguration[elfType].useUart);
                XRCCTRL(*this, "VTBaudTText"+elfTypeStr, wxStaticText)->Enable(true);
                XRCCTRL(*this, "ZoomTextVt"+elfTypeStr, wxStaticText)->Enable(true);
                XRCCTRL(*this, "VtSetup"+elfTypeStr, wxButton)->Enable(true);
				XRCCTRL(*this, "VtCharRomButton"+elfTypeStr, wxButton)->Enable(true);
				XRCCTRL(*this, "VtCharRom"+elfTypeStr, wxComboBox)->Enable(true);
				XRCCTRL(*this, "VtCharRom"+elfTypeStr, wxComboBox)->SetValue(conf[elfType].vtCharRom_);
				XRCCTRL(*this, "ZoomSpinVt"+elfTypeStr, wxSpinButton)->Enable(true);
				XRCCTRL(*this, "ZoomValueVt"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "StretchDot"+elfTypeStr, wxCheckBox)->Enable(true);
			}
			if (elfType == ELF || elfType == ELFII || elfType == SUPERELF)
			{
				elfConfiguration[elfType].qSound_ = QSOUNDOFF;
				if (mode_.gui)
				{
					XRCCTRL(*this, "Qsound"+elfTypeStr, wxChoice)->SetSelection(QSOUNDOFF);
					XRCCTRL(*this, "BeepFrequency"+elfTypeStr, wxTextCtrl)->Enable(false);
                    XRCCTRL(*this, "Qsound"+elfTypeStr, wxChoice)->Enable(false);
                    XRCCTRL(*this, "BeepFrequencyText"+elfTypeStr, wxStaticText)->Enable(false);
					XRCCTRL(*this, "BeepFrequencyTextHz"+elfTypeStr, wxStaticText)->Enable(false);
					XRCCTRL(*this, "QsoundText"+elfTypeStr, wxStaticText)->Enable(false);
				}
			}
            elfConfiguration[selectedComputer_].vtExternal = false;
		break;

		case VT100:
			if (elfType == COSMICOS || elfType == ELF2K || elfType == MS2000 || elfType == MEMBER || elfType == VIP || elfType == VELF)
				conf[elfType].vtCharRomDir_ = dataDir_ + elfTypeStr + pathSeparator_;
			else
                if (elfType == MCDS)
                    conf[elfType].vtCharRomDir_ = dataDir_ + "MCDS" + pathSeparator_;
                else
                    conf[elfType].vtCharRomDir_ = dataDir_ + "Elf" + pathSeparator_;
			conf[elfType].vtCharRom_ = "vt100.bin";
			if (mode_.gui)
			{
				XRCCTRL(*this, "VTBaudRChoice"+elfTypeStr, wxChoice)->Enable(elfConfiguration[elfType].useUart);
				XRCCTRL(*this, "VTBaudTChoice"+elfTypeStr, wxChoice)->Enable(true);
                XRCCTRL(*this, "VTBaudRText"+elfTypeStr, wxStaticText)->Enable(elfConfiguration[elfType].useUart);
                XRCCTRL(*this, "VTBaudTText"+elfTypeStr, wxStaticText)->Enable(true);
                XRCCTRL(*this, "ZoomTextVt"+elfTypeStr, wxStaticText)->Enable(true);
                XRCCTRL(*this, "VtSetup"+elfTypeStr, wxButton)->Enable(true);
				XRCCTRL(*this, "VtCharRomButton"+elfTypeStr, wxButton)->Enable(true);
				XRCCTRL(*this, "VtCharRom"+elfTypeStr, wxComboBox)->Enable(true);
				XRCCTRL(*this, "VtCharRom"+elfTypeStr, wxComboBox)->SetValue(conf[elfType].vtCharRom_);
				XRCCTRL(*this, "ZoomSpinVt"+elfTypeStr, wxSpinButton)->Enable(true);
				XRCCTRL(*this, "ZoomValueVt"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "StretchDot"+elfTypeStr, wxCheckBox)->Enable(true);
			}
			if (elfType == ELF || elfType == ELFII || elfType == SUPERELF)
			{
				elfConfiguration[elfType].qSound_ = QSOUNDOFF;
				if (mode_.gui)
				{
					XRCCTRL(*this, "Qsound"+elfTypeStr, wxChoice)->SetSelection(QSOUNDOFF);
					XRCCTRL(*this, "BeepFrequency"+elfTypeStr, wxTextCtrl)->Enable(false);
                    XRCCTRL(*this, "Qsound"+elfTypeStr, wxChoice)->Enable(false);
					XRCCTRL(*this, "BeepFrequencyTextHz"+elfTypeStr, wxStaticText)->Enable(false);
                    XRCCTRL(*this, "BeepFrequencyText"+elfTypeStr, wxStaticText)->Enable(false);
					XRCCTRL(*this, "QsoundText"+elfTypeStr, wxStaticText)->Enable(false);
				}
			}
            elfConfiguration[selectedComputer_].vtExternal = false;
		break;
    
        case EXTERNAL_TERMINAL:
            XRCCTRL(*this, "VTBaudRChoice"+elfTypeStr, wxChoice)->Enable(elfConfiguration[elfType].useUart);
            XRCCTRL(*this, "VTBaudTChoice"+elfTypeStr, wxChoice)->Enable(true);
            XRCCTRL(*this, "VTBaudRText"+elfTypeStr, wxStaticText)->Enable(elfConfiguration[elfType].useUart);
            XRCCTRL(*this, "VTBaudTText"+elfTypeStr, wxStaticText)->Enable(true);
            XRCCTRL(*this, "VtSetup"+elfTypeStr, wxButton)->Enable(true);
            if (elfType == ELF || elfType == ELFII || elfType == SUPERELF)
            {
                elfConfiguration[elfType].qSound_ = QSOUNDOFF;
                if (mode_.gui)
                {
                    XRCCTRL(*this, "Qsound"+elfTypeStr, wxChoice)->SetSelection(QSOUNDOFF);
                    XRCCTRL(*this, "BeepFrequency"+elfTypeStr, wxTextCtrl)->Enable(false);
                    XRCCTRL(*this, "Qsound"+elfTypeStr, wxChoice)->Enable(false);
                    XRCCTRL(*this, "BeepFrequencyText"+elfTypeStr, wxStaticText)->Enable(false);
                    XRCCTRL(*this, "BeepFrequencyTextHz"+elfTypeStr, wxStaticText)->Enable(false);
                    XRCCTRL(*this, "QsoundText"+elfTypeStr, wxStaticText)->Enable(false);
                }
            }
            elfConfiguration[selectedComputer_].vtExternal = true;
            elfConfiguration[elfType].vtType = VTNONE;
        break;
	}
}

void GuiMain::changeZoom(double zoom)
{
	if (selectedComputer_ == runningComputer_)
	{
        p_Video->setZoom(zoom);
		switch(runningComputer_)
		{
			case CIDELSA:
			case VIPII:
			case COMX:
				p_Video->reDrawBar();
			break;
		}
	}
	else
		XRCCTRL(*this, "ZoomValue"+computerInfo[selectedComputer_].gui, wxTextCtrl)->ChangeValue(conf[selectedComputer_].zoom_);
}

void GuiMain::onZoomUp(wxSpinEvent&WXUNUSED(event))
{
	double zoom;
	if ((selectedComputer_ == runningComputer_) && p_Video != NULL)
		zoom = p_Video->getZoom();
	else
		conf[selectedComputer_].zoom_.ToDouble(&zoom);

	if (!fullScreenFloat_)
		zoom = (int) (zoom + 1);
	else
	{
		zoom = (int)(zoom*10+0.5);
		zoom += 1;
		zoom /= 10;
	}
	conf[selectedComputer_].zoom_.Printf("%2.2f", zoom);
	changeZoom(zoom);
}

void GuiMain::onZoomDown(wxSpinEvent&WXUNUSED(event))
{
	double zoom;
	if ((selectedComputer_ == runningComputer_) && p_Video != NULL)
		zoom = p_Video->getZoom();
	else
		conf[selectedComputer_].zoom_.ToDouble(&zoom);

	if (!fullScreenFloat_)
	{
		zoom = (int) (zoom - 1);
		if (zoom < 1)  zoom = 1;
	}
	else
	{
		zoom = (int)(zoom*10+0.5);
		zoom -= 1;
		zoom /= 10;
		if (zoom < 0.5)  zoom = 0.5;
	}
	conf[selectedComputer_].zoom_.Printf("%2.2f", zoom);
	changeZoom(zoom);
}

void GuiMain::onZoomValue(wxCommandEvent&event)
{
	zoomTextValueChanged_ = true;
	wxString zoomString = event.GetString();
	double zoom;
	if (zoomString.ToDouble(&zoom))
	{
		if (!fullScreenFloat_)
			zoom = (int) (zoom);
		conf[selectedComputer_].zoom_ = zoomString;
		if (selectedComputer_ == runningComputer_)
		{
			p_Video->setZoom(zoom);
			switch(runningComputer_)
			{
				case CIDELSA:
				case COMX:
				case VIPII:
					p_Video->reDrawBar();
				break;
			}
		}
#if defined(__WXMSW__) || defined(__WXMAC__)
		else
			XRCCTRL(*this, "ZoomValue"+computerInfo[selectedComputer_].gui, wxTextCtrl)->ChangeValue(conf[selectedComputer_].zoom_);
#endif
	}
	else
	{
		(void)wxMessageBox( "Please specify a number\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
}

void GuiMain::changeZoomVt(double zoom)
{
	if ((selectedComputer_ == runningComputer_) && p_Vt100 != NULL)
	{
		p_Vt100->setZoom(zoom);
		if (runningComputer_ != ELF2K)
			p_Vt100->copyScreen();
	}
	else
		XRCCTRL(*this, "ZoomValueVt"+computerInfo[selectedComputer_].gui, wxTextCtrl)->ChangeValue(conf[selectedComputer_].zoomVt_);
}

void GuiMain::onZoomUpVt(wxSpinEvent&WXUNUSED(event))
{
	double zoomVt;
	if ((selectedComputer_ == runningComputer_) && p_Vt100 != NULL)
		zoomVt = p_Vt100->getZoom();
	else
		conf[selectedComputer_].zoomVt_.ToDouble(&zoomVt);

	if (!fullScreenFloat_)
		zoomVt = (int) (zoomVt + 1);
	else
	{
		zoomVt = (int)(zoomVt*10+0.5);
		zoomVt += 1;
		zoomVt /= 10;
	}
	conf[selectedComputer_].zoomVt_.Printf("%2.2f", zoomVt);
	changeZoomVt(zoomVt);
}

void GuiMain::onZoomDownVt(wxSpinEvent&WXUNUSED(event))
{
	double zoomVt;
	if ((selectedComputer_ == runningComputer_) && p_Vt100 != NULL)
		zoomVt = p_Vt100->getZoom();
	else
		conf[selectedComputer_].zoomVt_.ToDouble(&zoomVt);

	if (!fullScreenFloat_)
	{
		zoomVt = (int) (zoomVt - 1);
		if (zoomVt < 1)  zoomVt = 1;
	}
	else
	{
		zoomVt = (int)(zoomVt*10+0.5);
		zoomVt -= 1;
		zoomVt /= 10;
		if (zoomVt < 0.5)  zoomVt = 0.5;
	}
	conf[selectedComputer_].zoomVt_.Printf("%2.2f", zoomVt);
	changeZoomVt(zoomVt);
}

void GuiMain::onZoomValueVt(wxCommandEvent&event)
{
	wxString zoomString = event.GetString();
	double zoomVt;
	if (zoomString.ToDouble(&zoomVt))
	{
		if (!fullScreenFloat_)
		{
			zoomVt = (int) (zoomVt);
		}
		conf[selectedComputer_].zoomVt_ = zoomString;
		changeZoomVt(zoomVt);
	}
	else
	{
		(void)wxMessageBox( "Please specify a number\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
}

void GuiMain::onFullScreen(wxCommandEvent&WXUNUSED(event))
{
	if (computerRunning_ && p_Vt100 != NULL)
		p_Vt100->onF3();
	else if (computerRunning_ && p_Video != NULL)
		p_Video->onF3();
}

void GuiMain::onInterlace(wxCommandEvent&event)
{
	conf[selectedComputer_].interlace_ = event.IsChecked();
	if (computerRunning_ && p_Video != NULL)
		p_Video->setInterlace(event.IsChecked());
}

void GuiMain::onStretchDot(wxCommandEvent&event)
{
	conf[selectedComputer_].stretchDot_ = event.IsChecked();
	if (computerRunning_ && p_Vt100 != NULL)
		p_Vt100->setStretchDot(event.IsChecked());
}

void GuiMain::onScreenDumpFile(wxCommandEvent& WXUNUSED(event))
{
 	wxString fileName;

	fileName = wxFileSelector( "Select the Video Dump Output File",
                               conf[selectedComputer_].screenDumpFileDir_, XRCCTRL(*this, "ScreenDumpFile"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue(),
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
	conf[selectedComputer_].screenDumpFileDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].screenDumpFile_ = FullPath.GetFullName();

	XRCCTRL(*this, "ScreenDumpFile"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue(conf[selectedComputer_].screenDumpFile_);
}

void GuiMain::onScreenDumpFileText(wxCommandEvent& WXUNUSED(event))
{
	conf[selectedComputer_].screenDumpFile_ = XRCCTRL(*this, "ScreenDumpFile"+computerInfo[selectedComputer_].gui, wxComboBox)->GetValue();
}

void GuiMain::onScreenDump(wxCommandEvent&WXUNUSED(event))
{
	if (computerRunning_ && p_Video != NULL)
		p_Video->onF5();
	if (computerRunning_ && p_Vt100 != NULL)
		p_Vt100->onF5();
}

void GuiMain::onDp(wxCommandEvent&WXUNUSED(event))
{
	DevicePortsDialog DevicePortsDialog(this);
 	DevicePortsDialog.ShowModal();
}

void GuiMain::onVolume(wxScrollEvent&event)
{
	conf[selectedComputer_].volume_ = event.GetPosition();
	if (computerRunning_)
		p_Computer->setVolume(conf[selectedComputer_].volume_);
}

void GuiMain::onCassette(wxCommandEvent& WXUNUSED(event))
{
 	wxString fileName;

	fileName = wxFileSelector( "Select the WAV file to save/load",
                               conf[selectedComputer_].wavFileDir_, conf[selectedComputer_].wavFile_,
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
	conf[selectedComputer_].wavFileDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].wavFile_ = FullPath.GetFullName();

	if (mode_.gui)
		XRCCTRL(*this, "WavFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue(conf[selectedComputer_].wavFile_);
}

void GuiMain::onTerminalFile(wxCommandEvent& WXUNUSED(event))
{
	wxString fileName;

	fileName = wxFileSelector("Select the terminal file to save/load",
		conf[selectedComputer_].wavFileDir_, conf[selectedComputer_].wavFile_,
		"hex",
		wxString::Format
		(
			"Terminal File (*.hex)|*.hex|Tiny Basic File (*.txt)|*.txt|All files (%s)|%s",
			wxFileSelectorDefaultWildcardStr,
			wxFileSelectorDefaultWildcardStr
			),
		wxFD_OPEN | wxFD_CHANGE_DIR | wxFD_PREVIEW,
		this
		);


	if (!fileName)
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[selectedComputer_].wavFileDir_ = FullPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	conf[selectedComputer_].wavFile_ = FullPath.GetFullName();

	if (mode_.gui)
		XRCCTRL(*this, "WavFile" + computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue(conf[selectedComputer_].wavFile_);
}

void GuiMain::onCassetteEject(wxCommandEvent& WXUNUSED(event) )
{
	conf[selectedComputer_].wavFile_ = "";
	if (mode_.gui)
		XRCCTRL(*this, "WavFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue(conf[selectedComputer_].wavFile_);
}

void GuiMain::onCassetteText(wxCommandEvent&event)
{
	conf[selectedComputer_].wavFile_ = event.GetString();
    
    if (conf[selectedComputer_].wavFile_ != "")
    {
        if (wxFile::Exists(conf[selectedComputer_].wavFileDir_ + conf[selectedComputer_].wavFile_))
            p_Main->checkWavFile(conf[selectedComputer_].wavFileDir_ + conf[selectedComputer_].wavFile_);
    }
}

void GuiMain::onAutoLoad(wxCommandEvent&event)
{
	conf[selectedComputer_].autoCassetteLoad_ = event.IsChecked();
	if (computerRunning_ && (selectedComputer_ == runningComputer_))
	{
		XRCCTRL(*this, "CasLoad"+computerInfo[selectedComputer_].gui, wxButton)->Enable(!conf[runningComputer_].autoCassetteLoad_);
		XRCCTRL(*this, "CasSave"+computerInfo[selectedComputer_].gui, wxButton)->Enable(!conf[runningComputer_].autoCassetteLoad_);
	}
}

void GuiMain::onTurbo(wxCommandEvent&event)
{
	conf[selectedComputer_].turbo_ = event.IsChecked();
	turboGui(computerInfo[selectedComputer_].gui);
}

void GuiMain::onTurboClock(wxCommandEvent&WXUNUSED(event))
{
	double clk;

	conf[selectedComputer_].turboClock_ =  XRCCTRL(*this, "TurboClock"+computerInfo[selectedComputer_].gui, wxTextCtrl)->GetValue();
	if (conf[selectedComputer_].turboClock_ == "")
	{
		conf[selectedComputer_].turboClock_ = "15";
		return;
	}
	if (!conf[selectedComputer_].turboClock_.ToDouble((double*)&clk))
	{
		(void)wxMessageBox( "Please specify frequency in MHz\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
	if (clk == 0)
	{
		(void)wxMessageBox( "Please specify frequency value > 0\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
}

void GuiMain::onUseLocation(wxCommandEvent&event)
{
	conf[runningComputer_].useLoadLocation_ = event.IsChecked();
	enableLocationGui();
}

void GuiMain::onCassetteLoad(wxCommandEvent& WXUNUSED(event))
{
    if (runningComputer_ == FRED1 ||runningComputer_ == FRED2)
        p_Fred->startLoad(true);
    else
        startLoad();
}

void GuiMain::onCassetteSave(wxCommandEvent& WXUNUSED(event))
{
	startSave();
}

void GuiMain::onCassetteStop(wxCommandEvent& WXUNUSED(event))
{
	p_Computer->stopSaveLoad();
}

void GuiMain::onCassettePause(wxCommandEvent& WXUNUSED(event))
{
	p_Computer->pauseTape();
}

void GuiMain::onKeyboard(wxCommandEvent&event)
{
	conf[selectedComputer_].useKeyboard_ = event.IsChecked();
	XRCCTRL(*this, "KeyMap"+computerInfo[selectedComputer_].gui, wxButton)->Enable(!event.IsChecked());
}

void GuiMain::onPsave(wxString fileName)
{
	wxFile outputFile;
	size_t length;
    int address, start; // startAddress;
	char buffer[65536];
	wxString stringAdress;
	Byte highRamAddress;

	if (!fileName.empty())
	{
		if (outputFile.Create(fileName, true))
		{
			if (conf[runningComputer_].useLoadLocation_)
			{
/*				stringAdress = XRCCTRL(*this,"SaveStart"+computerInfo[runningComputer_].gui,wxTextCtrl)->GetValue();
				if (!stringAdress.ToLong(&saveStart, 16))
				{
					(void)wxMessageBox( "Please specify start address in hexadecimal\n",
										"Emma 02", wxICON_ERROR | wxOK );
					return;
				}
				stringAdress = XRCCTRL(*this,"SaveEnd"+computerInfo[runningComputer_].gui,wxTextCtrl)->GetValue();
				if (!stringAdress.ToLong(&saveEnd, 16))
				{
					(void)wxMessageBox( "Please specify end address in hexadecimal\n",
										"Emma 02", wxICON_ERROR | wxOK );
					return;
				}
				stringAdress = XRCCTRL(*this,"SaveExec"+computerInfo[runningComputer_].gui,wxTextCtrl)->GetValue();
				if (!stringAdress.ToLong(&saveExec, 16))
				{
					(void)wxMessageBox( "Please specify exec address in hexadecimal\n",
										"Emma 02", wxICON_ERROR | wxOK );
					return;
				}*/
				buffer [0] = 1;
				buffer [1] = conf[runningComputer_].pLoadSaveName_[0];
				buffer [2] = conf[runningComputer_].pLoadSaveName_[1];
				buffer [3] = conf[runningComputer_].pLoadSaveName_[2];
				buffer [4] = conf[runningComputer_].pLoadSaveName_[3];
				buffer [5] = (conf[selectedComputer_].saveStart_ >> 8) &0xff;
				buffer [6] = conf[selectedComputer_].saveStart_&0xff;
				buffer [7] = (conf[selectedComputer_].saveEnd_ >> 8) &0xff;
				buffer [8] = conf[selectedComputer_].saveEnd_&0xff;
				buffer [9] = (conf[selectedComputer_].saveExec_ >> 8) &0xff;
				buffer [10] = conf[selectedComputer_].saveExec_&0xff;
				address = conf[selectedComputer_].saveStart_;
				length = conf[selectedComputer_].saveEnd_ - conf[selectedComputer_].saveStart_ + 12;
				start = 11;
			}
			else
			{
				XRCCTRL(*this,"SaveStart"+computerInfo[runningComputer_].gui,wxTextCtrl)->SetValue("");
				XRCCTRL(*this,"SaveEnd"+computerInfo[runningComputer_].gui,wxTextCtrl)->SetValue("");
				XRCCTRL(*this,"SaveExec"+computerInfo[runningComputer_].gui,wxTextCtrl)->SetValue("");

				address = conf[runningComputer_].basicRamAddress_;
				highRamAddress = (conf[runningComputer_].basicRamAddress_ & 0xff00) >> 8;
				buffer [0] = 6;
				length = (p_Computer->getRam(conf[runningComputer_].eop_) - highRamAddress) << 8;
				if (runningComputer_ == COMX)
				{
					if (p_Comx->isFAndMBasicRunning())
					{
						address = 0x6700;
						buffer [0] = 3;
						highRamAddress = 0;
						length = (p_Computer->getRam(conf[runningComputer_].eop_) - 0x67) << 8;
					}
				}
				length += p_Computer->getRam(conf[runningComputer_].eop_+1);
				buffer [1] = conf[runningComputer_].pLoadSaveName_[0];
				buffer [2] = conf[runningComputer_].pLoadSaveName_[1];
				buffer [3] = conf[runningComputer_].pLoadSaveName_[2];
				buffer [4] = conf[runningComputer_].pLoadSaveName_[3];
				buffer [5] = p_Computer->getRam(conf[runningComputer_].defus_)-highRamAddress;
				buffer [6] = p_Computer->getRam(conf[runningComputer_].defus_+1);
				buffer [7] = p_Computer->getRam(conf[runningComputer_].eop_)-highRamAddress;
				buffer [8] = p_Computer->getRam(conf[runningComputer_].eop_+1);
				buffer [9] = p_Computer->getRam(conf[runningComputer_].string_)-highRamAddress;
				buffer [10] = p_Computer->getRam(conf[runningComputer_].string_+1);
				buffer [11] = p_Computer->getRam(conf[runningComputer_].arrayValue_)-highRamAddress;
				buffer [12] = p_Computer->getRam(conf[runningComputer_].arrayValue_+1);
				buffer [13] = p_Computer->getRam(conf[runningComputer_].eod_)-highRamAddress;
				buffer [14] = p_Computer->getRam(conf[runningComputer_].eod_+1);
				start = 15;
				length += start;
			}
//			startAddress = address;
			for (size_t i=start; i<(length); i++)
			{
				buffer[i] = p_Computer->readMem(address);
				address++;
			}
			outputFile.Write(buffer, length);
			outputFile.Close();
		}
		else
		{
			(void)wxMessageBox( "Error writing " + fileName,
							    "Emma 02", wxICON_ERROR | wxOK );
			return;
		}
	}
}

void GuiMain::onLoadRunButton(wxCommandEvent& WXUNUSED(event) )
{
	onLoad(false);
}

void GuiMain::onDataSaveButton(wxCommandEvent& WXUNUSED(event) )
{
	wxFile outputFile;
	size_t dataLength, arrayLength;
	int address, start, arrayStart, stringStart, dataEnd;
	char buffer[65536];
	wxString fileName, stringAdress;

	fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" Data file to save",
                               conf[runningComputer_].ramDir_, conf[runningComputer_].loadFileName_,
                               "",
                               wxString::Format
                              (
                                   computerInfo[selectedComputer_].name+" Data File (*."+computerInfo[selectedComputer_].ploadExtension+")|*."+computerInfo[selectedComputer_].ploadExtension+"|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_OVERWRITE_PROMPT,
                               this
                              );
	if (!fileName)
		return;

	wxFileName swFullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[runningComputer_].ramDir_ = swFullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	if (!fileName.empty())
	{
		if (outputFile.Create(fileName, true))
		{
			arrayStart = p_Computer->getRam(conf[runningComputer_].arrayValue_) << 8;
			arrayStart += p_Computer->getRam(conf[runningComputer_].arrayValue_+1);
			stringStart = p_Computer->getRam(conf[runningComputer_].string_) << 8;
			stringStart += p_Computer->getRam(conf[runningComputer_].string_+1);
			dataEnd = p_Computer->getRam(conf[runningComputer_].eod_) << 8;
			dataEnd += p_Computer->getRam(conf[runningComputer_].eod_+1);
			dataLength = dataEnd-arrayStart;
			arrayLength = stringStart-arrayStart;
			buffer [0] = 5;
			buffer [1] = conf[runningComputer_].pLoadSaveName_[0];
			buffer [2] = conf[runningComputer_].pLoadSaveName_[1];
			buffer [3] = conf[runningComputer_].pLoadSaveName_[2];
			buffer [4] = conf[runningComputer_].pLoadSaveName_[3];
			buffer [5] = (arrayLength >> 8) &0xff;
			buffer [6] = arrayLength &0xff;
			address = arrayStart;
			start = 7;
			dataLength += start;

			for (size_t i=start; i<(dataLength); i++)
			{
				buffer[i] = p_Computer->getRam(address);
				address++;
			}
			outputFile.Write(buffer, dataLength);
			outputFile.Close();
		}
		else
		{
			(void)wxMessageBox( "Error writing " + fileName,
							    "Emma 02", wxICON_ERROR | wxOK );
			return;
		}
	}
}

void GuiMain::onLoadButton(wxCommandEvent& WXUNUSED(event))
{
	onLoad(true);
}

void GuiMain::onSaveStart(wxCommandEvent& event)
{
    if (selectedComputer_ >= NO_COMPUTER)
        return;
    
	conf[selectedComputer_].saveStartString_ = event.GetString();

	long value;

	if (conf[selectedComputer_].saveStartString_.ToLong(&value, 16))
		conf[selectedComputer_].saveStart_ = value;
	else
		conf[selectedComputer_].saveStart_ = 0;
}

void GuiMain::onSaveEnd(wxCommandEvent& event)
{
    if (selectedComputer_ >= NO_COMPUTER)
        return;
    
    conf[selectedComputer_].saveEndString_ = event.GetString();

	long value;

	if (conf[selectedComputer_].saveEndString_.ToLong(&value, 16))
		conf[selectedComputer_].saveEnd_ = value;
	else
		conf[selectedComputer_].saveEnd_ = 0;
}

void GuiMain::onSaveExec(wxCommandEvent& event)
{
    if (selectedComputer_ >= NO_COMPUTER)
        return;
    
	conf[selectedComputer_].saveExecString_ = event.GetString();

	long value;

	if (conf[selectedComputer_].saveExecString_.ToLong(&value, 16))
		conf[selectedComputer_].saveExec_ = value;
	else
		conf[selectedComputer_].saveExec_ = 0;
}

void GuiMain::runSoftware(bool load)
{
	conf[runningComputer_].loadFileName_ = configPointer->Read("/"+computerInfo[runningComputer_].gui+"/Software_File", "");
	conf[runningComputer_].loadFileNameFull_ = conf[runningComputer_].ramDir_ + conf[runningComputer_].loadFileName_;
	p_Main->setSwName (conf[runningComputer_].loadFileName_);
    p_Main->updateTitle();

	switch(runningComputer_)
	{
		case COMX:
		case PECOM:
		case TMC600:
            p_Computer->startComputerRun(load);
		break;

		case SUPERELF:
		case ELFII:
		case ELF:
			if (p_Computer->getLoadedProgram()&0x1)
				p_Computer->startComputerRun(load);
			else
				p_Computer->readFile(conf[runningComputer_].loadFileNameFull_, NOCHANGE, 0, 0x10000, SHOWNAME);
		break;

		case ELF2K:
			if (elfConfiguration[ELF2K].use8275)
				p_Elf2K->readFile(conf[runningComputer_].loadFileNameFull_, NOCHANGE, 0, 0x7700, SHOWNAME);
			else
				p_Elf2K->readFile(conf[runningComputer_].loadFileNameFull_, NOCHANGE, 0, 0x7f00, SHOWNAME);
		break;

		case ETI:
			p_Eti->readFile(conf[runningComputer_].loadFileNameFull_, NOCHANGE, 0x600, 0xfff, SHOWNAME);
		break;

		default:
			p_Computer->readFile(conf[runningComputer_].loadFileNameFull_, NOCHANGE, 0, 0x10000, SHOWNAME);
		break;
	}
}

void GuiMain::onLoad(bool load)
{
	wxFFile inputFile;
	wxString fileName;
	wxString extension;

	if (p_Computer->isComputerRunning())
	{
		int answer = wxMessageBox(	computerInfo[selectedComputer_].name+" Emulator is running software\n"
									"Reset before loading/running NEW software?",
									"Emma 02", wxYES_NO);

		if (answer == wxYES)
		{
			if (p_Computer == NULL)
				return;
			p_Computer->onReset();
		}
	}
	if (p_Computer == NULL)
		return;

	switch (selectedComputer_)
	{
		case ETI:
		case TMC1800:
		case TMC2000:
		case NANO:
			extension = "Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10|Intel Hex File|*.hex|All files (%s)|%s";
		break;

        case VIP:
			if (p_Computer->getLoadedProgram()==FPBBASIC)
				extension = computerInfo[selectedComputer_].name+" Program File|*."+computerInfo[selectedComputer_].ploadExtension+"|Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10|Intel Hex File|*.hex|All files (%s)|%s";
			else
				extension = "Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10|Intel Hex File|*.hex|All files (%s)|%s";
		break;

		case COMX:
		case PECOM:
		case TMC600:
        case VIPII:
        case MCDS:
            extension = computerInfo[selectedComputer_].name+" Program File|*."+computerInfo[selectedComputer_].ploadExtension+"|Binary File|*.bin;*.rom;*.ram;|Intel Hex File|*.hex|All files (%s)|%s";
		break;

		case SUPERELF:
		case ELFII:
		case ELF:
			if (p_Computer->getLoadedProgram()&0x1)
				extension = computerInfo[selectedComputer_].name+" Program File|*."+computerInfo[selectedComputer_].ploadExtension+"|Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10|Intel Hex File|*.hex|All files (%s)|%s";
			else
				extension = "Intel Hex File|*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10|All files (%s)|%s";
		break;

		default:
			extension = "Intel Hex File|*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10|All files (%s)|%s";
		break;
	}

	fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" Program file to load/run",
                               conf[selectedComputer_].ramDir_, conf[selectedComputer_].loadFileName_,
                               "",
                               wxString::Format
                              (
                                   extension,
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_FILE_MUST_EXIST,
                               this
                              );
	if (!fileName)
		return;

	wxFileName swFullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[selectedComputer_].ramDir_ = swFullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	conf[runningComputer_].loadFileNameFull_ = fileName;
	conf[selectedComputer_].loadFileName_ = swFullPath.GetFullName();
	p_Main->setSwName(swFullPath.GetName());
    p_Main->updateTitle();

#define SHOW_ADDRESS_POPUP_WINDOW true
    
	switch(selectedComputer_)
	{
		case COMX:
		case PECOM:
		case TMC600:
		case VIPII:
            p_Computer->startComputerRun(load);
		break;

		case MCDS:
			if (swFullPath.GetExt() == computerInfo[selectedComputer_].ploadExtension)
				p_Computer->startComputerRun(load);
            else
                p_Computer->readFile(fileName, NOCHANGE, 0, 0x10000, SHOWNAME, SHOW_ADDRESS_POPUP_WINDOW, conf[selectedComputer_].saveStart_);
        break;

		case SUPERELF:
		case ELFII:
		case ELF:
        case VIP:
			if (p_Computer->getLoadedProgram()&0x1)
				p_Computer->startComputerRun(load);
			else
				p_Computer->readFile(fileName, NOCHANGE, 0, 0x10000, SHOWNAME, SHOW_ADDRESS_POPUP_WINDOW, conf[selectedComputer_].saveStart_);
		break;

		case ELF2K:
			if (elfConfiguration[ELF2K].use8275)
				p_Elf2K->readFile(fileName, NOCHANGE, 0, 0x7700, SHOWNAME, SHOW_ADDRESS_POPUP_WINDOW, conf[selectedComputer_].saveStart_);
			else
				p_Elf2K->readFile(fileName, NOCHANGE, 0, 0x7f00, SHOWNAME, SHOW_ADDRESS_POPUP_WINDOW, conf[selectedComputer_].saveStart_);
		break;

		case ETI:
			p_Eti->readFile(fileName, NOCHANGE, 0x600, 0xfff, SHOWNAME, SHOW_ADDRESS_POPUP_WINDOW, conf[selectedComputer_].saveStart_);
		break;

		default:
			p_Computer->readFile(fileName, NOCHANGE, 0, 0x10000, SHOWNAME, SHOW_ADDRESS_POPUP_WINDOW, conf[selectedComputer_].saveStart_);
		break;
	}
}

void GuiMain::onSaveButton(wxCommandEvent& WXUNUSED(event))
{
	wxFile outputFile;
	wxString fileName, stringAdress;
	wxString extension;

	switch (selectedComputer_)
	{
		case ETI:
		case TMC1800:
		case TMC2000:
		case NANO:
			extension = "Binary File|*.c8;*.ch8;*.c8x;*.ch10;*.bin;*.rom;*.ram;*.cos|Intel Hex File|*.hex|All files (%s)|%s";
		break;

        case VIP:
			if (p_Computer->getLoadedProgram()==FPBBASIC)
				extension = computerInfo[selectedComputer_].name+" Program File|*."+computerInfo[selectedComputer_].ploadExtension+"|Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10|Intel Hex File|*.hex|All files (%s)|%s";
			else
				extension = "Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10|Intel Hex File|*.hex|All files (%s)|%s";
		break;

		case COMX:
		case PECOM:
		case TMC600:
		case VIPII:
        case MCDS:
            extension = computerInfo[selectedComputer_].name+" Program File (*."+computerInfo[selectedComputer_].ploadExtension+")|*."+computerInfo[selectedComputer_].ploadExtension+"|Binary File|*.bin;*.rom;*.ram;|Intel Hex File|*.hex|All files (%s)|%s";
		break;

		case SUPERELF:
		case ELFII:
		case ELF:
			if (p_Computer->getLoadedProgram()&0x1)
			{
				extension = computerInfo[selectedComputer_].name+" Program File (*."+computerInfo[selectedComputer_].ploadExtension+")|*."+computerInfo[selectedComputer_].ploadExtension+"|Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10|Intel Hex File|*.hex|All files (%s)|%s";
			}
			else
			{
				extension = "Intel Hex File|*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10|All files (%s)|%s";
			}
		break;

		default:
			extension = "Intel Hex File|*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10|All files (%s)|%s";
		break;
	}

	fileName = wxFileSelector( "Select the "+computerInfo[selectedComputer_].name+" Program file to save",
                               conf[selectedComputer_].ramDir_, conf[selectedComputer_].loadFileName_,
                               "",
                               wxString::Format
                              (
                                   extension,
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_OVERWRITE_PROMPT,
                               this
                              );
	if (!fileName)
		return;

	wxFileName swFullPath = wxFileName(fileName, wxPATH_NATIVE);
	conf[selectedComputer_].ramDir_ = swFullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	conf[selectedComputer_].loadFileNameFull_ = fileName;
	conf[selectedComputer_].loadFileName_ = swFullPath.GetFullName();
	p_Main->setSwName(swFullPath.GetName());
    p_Main->updateTitle();

	extension = swFullPath.GetExt();
	if (extension == computerInfo[selectedComputer_].ploadExtension && computerInfo[selectedComputer_].ploadExtension != "")
	{ 
		onPsave(fileName);
		return;
	}

/*	stringAdress = XRCCTRL(*this, "SaveStart"+computerInfo[selectedComputer_].gui,wxTextCtrl)->GetValue();
	if (!stringAdress.ToLong(&saveStart, 16))
	{
		(void)wxMessageBox( "Please specify start address in hexadecimal\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
	stringAdress = XRCCTRL(*this,"SaveEnd"+computerInfo[selectedComputer_].gui,wxTextCtrl)->GetValue();
	if (!stringAdress.ToLong(&saveEnd, 16))
	{
		(void)wxMessageBox( "Please specify end address in hexadecimal\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}*/
	if (conf[selectedComputer_].saveStart_ > conf[selectedComputer_].saveEnd_)
	{
		(void)wxMessageBox( "Please specify start < end\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
	if ((extension == "bin") || (extension == "rom") || (extension == "ram") || (extension == "cos") || (extension == "c8") || (extension == "ch8") || (extension == "c8x") || (extension == "ch10"))
		p_Computer->saveBinFile(fileName, conf[selectedComputer_].saveStart_, conf[selectedComputer_].saveEnd_);
	else
		p_Computer->saveIntelFile(fileName, conf[selectedComputer_].saveStart_, conf[selectedComputer_].saveEnd_);
}

void GuiMain::onClock(wxCommandEvent&WXUNUSED(event))
{
	double clk;

	wxString clock =  clockTextCtrl[selectedComputer_]->GetValue();
	if (!clock.ToDouble((double*)&clk))
	{
		(void)wxMessageBox( "Please specify frequency in MHz\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
	if (clk == 0)
	{
		(void)wxMessageBox( "Please specify frequency value > 0\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
	if (clk >= 500)
	{
		(void)wxMessageBox( "Please specify frequency value < 500\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
	conf[selectedComputer_].clock_ = clock;
	switch (selectedComputer_)
	{
		case COMX:
			if (conf[selectedComputer_].videoMode_ == PAL)
				comxPalClock_ = clock;
			else
				comxNtscClock_ = clock;
		break;

		case ELF2K:
			if (conf[selectedComputer_].videoMode_ == VIDEOPIXIE)
				elf2KPixieClock_ = clock;
			else
				elf2K8275Clock_ = clock;
		break;
	}

	setClock(selectedComputer_);
}

void GuiMain::onBaudR(wxCommandEvent&event)
{
	elfConfiguration[selectedComputer_].baudR = event.GetSelection();
}

void GuiMain::onBaudT(wxCommandEvent&event)
{
	elfConfiguration[selectedComputer_].baudT = event.GetSelection();
	if (!elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].baudR = event.GetSelection();
		XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(elfConfiguration[selectedComputer_].baudR);
	}
}

void GuiMain::onPsaveMenu(wxCommandEvent&WXUNUSED(event))
{
	PsaveDialog PsaveDialog(this);
 	PsaveDialog.ShowModal();
}

void GuiMain::onVtSetup(wxCommandEvent&WXUNUSED(event))
{
	VtSetupDialog VtSetupDialog(this);
 	VtSetupDialog.ShowModal();
}

void GuiMain::onBeepFrequency(wxCommandEvent&WXUNUSED(event))
{
	setBeepFrequency(selectedComputer_);
}

wxString GuiMain::getKeyFile()
{
	return conf[runningComputer_].keyFile_;
}

wxString GuiMain::getKeyFileDir()
{
	return conf[runningComputer_].keyFileDir_;
}

wxString GuiMain::getScreenDumpFile()
{
	wxString screenDumpFile = conf[runningComputer_].screenDumpFile_;
	if (screenDumpFile.Len() != 0)
		screenDumpFile = conf[runningComputer_].screenDumpFileDir_ + screenDumpFile;

	return screenDumpFile;
}

wxString GuiMain::getPrintFile()
{
	wxString fileName = conf[runningComputer_].printFile_;

	if (fileName.Len() != 0)
		fileName = conf[runningComputer_].printFileDir_ + fileName;

	return fileName;
}

ElfConfiguration GuiMain::getElfConfiguration()
{
	return elfConfiguration[selectedComputer_]; 
}

ElfConfiguration GuiMain::getElfConfiguration(int computer)
{
	return elfConfiguration[computer];
}

void GuiMain::setElfConfiguration(ElfConfiguration elfConf)
{
    elfConfiguration[selectedComputer_] = elfConf;
}

void GuiMain::setSerialPorts(wxString port)
{
	for (int computer = 0; computer <= LAST_ELF_TYPE; computer++)
	{
		if (elfConfiguration[computer].serialPort_ == "")
			elfConfiguration[computer].serialPort_ = port;
	}
}

long GuiMain::getBitValue(wxString reference)
{
	wxString strValue = XRCCTRL(*this, reference, wxTextCtrl)->GetValue();
	if (strValue == "")
		return -1;

	long value;

	if (!strValue.ToLong(&value))
	{
		(void)wxMessageBox( "Please specify value of 0 or 1\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return -1;
	}

	if (value > 1)
	{
		(void)wxMessageBox( "Please specify value of 0 or 1\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return -1;
	}

	return value;
}

long GuiMain::get8BitValue(wxString reference)
{
	wxString strValue = XRCCTRL(*this, reference, wxTextCtrl)->GetValue();
	if (strValue == "")
		return -1;

	long value;

	if (!strValue.ToLong(&value, 16))
	{
		(void)wxMessageBox( "Please specify value in hexadecimal\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return -1;
	}

	if (value > 0xff)
	{
		(void)wxMessageBox( "Please specify value of 8 bit max\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return -1;
	}

	return value;
}

long GuiMain::get12BitValue(wxString reference)
{
	wxString strValue = XRCCTRL(*this, reference, wxTextCtrl)->GetValue();
	if (strValue == "")
		return -1;

	long value;

	if (!strValue.ToLong(&value, 16))
	{
		(void)wxMessageBox( "Please specify value in hexadecimal\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return -1;
	}

	if (value > 0xfff)
	{
		(void)wxMessageBox( "Please specify value of 12 bit max\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return -1;
	}

	return value;
}

long GuiMain::get16BitValue(wxString reference)
{
	wxString strValue = XRCCTRL(*this, reference, wxTextCtrl)->GetValue();
	if (strValue == "")
		return -1;

	long value;

	if (!strValue.ToLong(&value, 16))
	{
		(void)wxMessageBox( "Please specify value in hexadecimal\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return -1;
	}

	if (value > 0xffff)
	{
		(void)wxMessageBox( "Please specify value of 16 bit max\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return -1;
	}

	return value;
}

void GuiMain::setClock(int computerType)
{
	double clk;

	wxString clock =  conf[computerType].clock_;
	if (!clock.ToDouble((double*)&clk))
	{
		(void)wxMessageBox( "Please specify frequency in MHz\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
	if (clk == 0)
	{
		(void)wxMessageBox( "Please specify frequency value > 0\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
	if (clk >= 500)
	{
		(void)wxMessageBox( "Please specify frequency value < 500\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
	conf[computerType].clockSpeed_ = clk;
	conf[computerType].fdcCpms_ = clk * 100;

	if ((computerType == runningComputer_) && computerRunning_)
	{
		setClockRate();
		if (p_Video != NULL)
			p_Video->setClock(conf[computerType].clockSpeed_);
		if (p_Vt100 != NULL)
			p_Vt100->setClock(conf[computerType].clockSpeed_);
	}
}

void GuiMain::setBeepFrequency(int computerType)
{
	long frequency;

	wxString frequencyString =  XRCCTRL(*this, "BeepFrequency"+computerInfo[computerType].gui, wxTextCtrl)->GetValue();
	if (!frequencyString.ToLong((long*)&frequency))
	{
		(void)wxMessageBox( "Please specify frequency in Hz\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
	if (frequency == 0)
	{
		(void)wxMessageBox( "Please specify frequency value > 0\n",
							"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
	conf[computerType].beepFrequency_ = (int)frequency;
	if ((computerType == runningComputer_) && computerRunning_)
	{
		p_Computer->changeBeepFrequency(conf[computerType].beepFrequency_);
	}
}

void GuiMain::setClockRate()
{
	p_Computer->setClockRate(conf[runningComputer_].clockSpeed_);
	if (runningComputer_ == ELF2K)
			p_Elf2K->setElf2KClockSpeed(conf[runningComputer_].clockSpeed_);
}

int GuiMain::getConfigItem(wxString Item, long DefaultValue)
{
	return (int)configPointer->Read(Item, DefaultValue);
}

void GuiMain::setConfigItem(wxString Item, int Value)
{
    configPointer->Write(Item, Value);
}

void GuiMain::setConfigItem(wxString Item, long Value)
{
    configPointer->Write(Item, Value);
}

wxString GuiMain::getConfigItem(wxString Item, wxString DefaultValue)
{
	return configPointer->Read(Item, DefaultValue);
}

void GuiMain::setConfigItem(wxString Item, wxString Value)
{
	configPointer->Write(Item, Value);
}

bool GuiMain::getConfigBool(wxString Item, bool DefaultValue)
{
	bool ret;
	configPointer->Read(Item, &ret, DefaultValue);
	return ret;
}

void GuiMain::setConfigBool(wxString Item, bool Value)
{
	configPointer->Write(Item, Value);
}

int GuiMain::getChoiceSelection(wxString info)
{
	return XRCCTRL(*this, info, wxChoice)->GetCurrentSelection();
}

void GuiMain::setCheckBox(wxString info, bool status)
{
	if (mode_.gui)
		XRCCTRL(*this, info, wxCheckBox)->SetValue(status);
	if (info == "Elf2KClearRam")
		elfConfiguration[ELF2K].clearRam = status;
	if (info == "Elf2KClearRtc")
		elfConfiguration[ELF2K].clearRtc = status;
	if (info == "ClearRamMembership")
		elfConfiguration[MEMBER].clearRam = status;
}

void GuiMain::setSpinCtrl(wxString info, int value)
{
	if (mode_.gui)
		XRCCTRL(*this, info, wxSpinCtrl)->SetValue(value);
}

void GuiMain::setTextCtrl(wxString info, wxString value)
{
	if (mode_.gui)
		XRCCTRL(*this, info, wxTextCtrl)->SetValue(value);
}

void GuiMain::setComboBox(wxString info, wxString value)
{
	if (mode_.gui)
		XRCCTRL(*this, info, wxComboBox)->SetValue(value);
}

void GuiMain::onHexKeyDef(wxCommandEvent&WXUNUSED(event))
{
	KeyMapDialog KeyMapDialog(this);
 	KeyMapDialog.ShowModal();

	if (computerRunning_)
		p_Computer->reDefineKeys();
}

void GuiMain::onColoursDef(wxCommandEvent&WXUNUSED(event))
{
	ColourDialog ColourDialog(this);
 	ColourDialog.ShowModal();
}

void GuiMain::setScale(wxString scale)
{
	conf[selectedComputer_].xScale_ = scale;
}

double GuiMain::getScale()
{
	double scale;
	if (!conf[runningComputer_].xScale_.ToDouble(&scale))
		scale = 3;

	if (!fullScreenFloat_)
		scale = (int) scale;
	return scale;
}

double GuiMain::getZoom()
{
	double zoom;
	conf[runningComputer_].zoom_.ToDouble(&zoom);

	if (!fullScreenFloat_)
		zoom = (int) zoom;
	return zoom;
}

double GuiMain::getZoomVt()
{
	double zoomVt;
	conf[runningComputer_].zoomVt_.ToDouble(&zoomVt);

	if (!fullScreenFloat_)
		zoomVt = (int) zoomVt;
	return zoomVt;
}

wxSize GuiMain::getMainSize(int computerType)
{
	return wxSize(conf[computerType].sizeX_, conf[computerType].sizeY_);
}

void GuiMain::setMainSize(int computerType, wxSize size)
{
	conf[computerType].sizeX_ = size.x;
	conf[computerType].sizeY_ = size.y;
}

wxSize GuiMain::get6845Size(int computerType)
{
	return wxSize(conf[computerType].sizeX6845_, conf[computerType].sizeY6845_);
}

void GuiMain::set6845Size(int computerType, wxSize size)
{
	conf[computerType].sizeX6845_ = size.x;
	conf[computerType].sizeY6845_ = size.y;
}

wxPoint GuiMain::getMainPos(int computerType)
{
	return wxPoint(conf[computerType].mainX_, conf[computerType].mainY_);
}

void GuiMain::setMainPos(int computerType, wxPoint position)
{
	if (!mode_.window_position_fixed)
	{
		conf[computerType].mainX_ = -1;
		conf[computerType].mainY_ = -1;
	}
	else
	{
		if (position.x > 0)
			conf[computerType].mainX_ = position.x;
		if (position.y > 0)
			conf[computerType].mainY_ = position.y;
	}
}

wxPoint GuiMain::getPixiePos(int computerType)
{
	return wxPoint(conf[computerType].pixieX_, conf[computerType].pixieY_);
}

void GuiMain::setPixiePos(int computerType, wxPoint position)
{
	if (!mode_.window_position_fixed)
	{
		conf[computerType].pixieX_ = -1;
		conf[computerType].pixieY_ = -1;
	}
	else
	{
		if (position.x > 0)
			conf[computerType].pixieX_ = position.x;
		if (position.y > 0)
			conf[computerType].pixieY_ = position.y;
	}
}

wxPoint GuiMain::getTmsPos(int computerType)
{
	return wxPoint(conf[computerType].tmsX_, conf[computerType].tmsY_);
}

void GuiMain::setTmsPos(int computerType, wxPoint position)
{
	if (!mode_.window_position_fixed)
	{
		conf[computerType].tmsX_ = -1;
		conf[computerType].tmsY_ = -1;
	}
	else
	{
		if (position.x > 0)
			conf[computerType].tmsX_ = position.x;
		if (position.y > 0)
			conf[computerType].tmsY_ = position.y;
	}
}

wxPoint GuiMain::getVtPos(int computerType)
{
	return wxPoint(conf[computerType].vtX_, conf[computerType].vtY_);
}

void GuiMain::setVtPos(int computerType, wxPoint position)
{
	if (!mode_.window_position_fixed)
	{
		conf[computerType].vtX_ = -1;
		conf[computerType].vtY_ = -1;
	}
	else
	{
		if (position.x > 0)
			conf[computerType].vtX_ = position.x;
		if (position.y > 0)
			conf[computerType].vtY_ = position.y;
	}
}

wxPoint GuiMain::get6845Pos(int computerType)
{
	return wxPoint(conf[computerType].mc6845X_, conf[computerType].mc6845Y_);
}

void GuiMain::set6845Pos(int computerType, wxPoint position)
{
	if (!mode_.window_position_fixed)
	{
		conf[computerType].mc6845X_ = -1;
		conf[computerType].mc6845Y_ = -1;
	}
	else
	{
		if (position.x > 0)
			conf[computerType].mc6845X_ = position.x;
		if (position.y > 0)
			conf[computerType].mc6845Y_ = position.y;
	}
}

wxPoint GuiMain::get6847Pos(int computerType)
{
	return wxPoint(conf[computerType].mc6847X_, conf[computerType].mc6847Y_);
}

void GuiMain::set6847Pos(int computerType, wxPoint position)
{
	if (!mode_.window_position_fixed)
	{
		conf[computerType].mc6847X_ = -1;
		conf[computerType].mc6847Y_ = -1;
	}
	else
	{
		if (position.x > 0)
			conf[computerType].mc6847X_ = position.x;
		if (position.y > 0)
			conf[computerType].mc6847Y_ = position.y;
	}
}


wxPoint GuiMain::get8275Pos(int computerType)
{
	return wxPoint(conf[computerType].i8275X_, conf[computerType].i8275Y_);
}

void GuiMain::set8275Pos(int computerType, wxPoint position)
{
	if (!mode_.window_position_fixed)
	{
		conf[computerType].i8275X_ = -1;
		conf[computerType].i8275Y_ = -1;
	}
	else
	{
		if (position.x > 0)
			conf[computerType].i8275X_ = position.x;
		if (position.y > 0)
			conf[computerType].i8275Y_ = position.y;
	}
}

wxPoint GuiMain::getKeypadPos(int computerType)
{
	return wxPoint(conf[computerType].keypadX_, conf[computerType].keypadY_);
}

void GuiMain::setKeypadPos(int computerType, wxPoint position)
{
	if (!mode_.window_position_fixed)
	{
		conf[computerType].keypadX_ = -1;
		conf[computerType].keypadY_ = -1;
	}
	else
	{
		if (position.y > 0)
			conf[computerType].keypadX_ = position.x;
		if (position.x > 0)
			conf[computerType].keypadY_ = position.y;
	}
}

int GuiMain::pload()
{
	wxFFile inputFile;
	size_t length, arrayLength;
	int address, start, startAddress, dataEnd, stringStart, fAndMBasicOffset;
//	Byte slot;
	char buffer[65536];
	wxString printBuffer;
	Byte highRamAddress;

	conf[selectedComputer_].saveExec_ = 0;

	if (inputFile.Open(conf[runningComputer_].loadFileNameFull_, _("rb")))
	{
		inputFile.Read(buffer, 4);
		inputFile.Close();

		if (buffer[0] == ':')
			return p_Computer->readIntelFile(conf[runningComputer_].loadFileNameFull_, RAM, 0x10000, true);
		else if (buffer[0] == '0' && buffer[1] == '0' && buffer[2] == '0' && buffer[3] == '0')
			return p_Computer->readLstFile(conf[runningComputer_].loadFileNameFull_, RAM, 0x10000, true);
	}

	if (buffer [0] != 1)
	{
		p_Main->eventSetTextValue("SaveStart"+computerInfo[runningComputer_].gui, "");
		p_Main->eventSetTextValue("SaveEnd"+computerInfo[runningComputer_].gui, "");
		p_Main->eventSetTextValue("SaveExec"+computerInfo[runningComputer_].gui, "");
	}

	if (inputFile.Open(conf[runningComputer_].loadFileNameFull_, _("rb")))
	{
		length = inputFile.Read(buffer, 65536);
		address = conf[runningComputer_].basicRamAddress_;
		start = conf[runningComputer_].basicRamAddress_ & 0xf00;
		if ((buffer [0] == 1 || buffer [0] == 2 || buffer [0] == 3 || buffer [0] == 4 || buffer [0] == 5 || buffer [0] == 6) && (buffer [1] == conf[runningComputer_].pLoadSaveName_[0]) && (buffer [2] == conf[runningComputer_].pLoadSaveName_[1]) && (buffer [3] == conf[runningComputer_].pLoadSaveName_[2]) && (buffer [4] == conf[runningComputer_].pLoadSaveName_[3]))
		{
			switch(buffer [0])
			{
				case 1: /* Machine code LOAD */
					address = (Word)(buffer [5] << 8) + buffer [6];
					p_Main->eventSetLocation(true, (buffer [5] << 8) + buffer [6], (buffer [7] << 8) + buffer [8], (buffer [9] << 8) + buffer [10]);
					start = 11;
				break;

				case 2: /* Regular LOAD */
					fAndMBasicOffset = 0;
					address = conf[runningComputer_].basicRamAddress_;
					if (runningComputer_ == COMX)
					{
						if (p_Comx->isFAndMBasicRunning())
						{
							fAndMBasicOffset = 0x23;
							if (buffer[5] != 0x44)
							{
								p_Main->errorMessage(	"File " + conf[runningComputer_].loadFileNameFull_ + " can only be loaded in COMX Basic");
								return 1;
							}
							address = 0x6700;
						}
					}
					p_Computer->setRam(conf[runningComputer_].defus_, (Byte)buffer[5]+fAndMBasicOffset);
					p_Computer->setRam(conf[runningComputer_].defus_+1, (Byte)buffer[6]);
					p_Computer->setRam(conf[runningComputer_].eop_, (Byte)buffer[7]+fAndMBasicOffset);
					p_Computer->setRam(conf[runningComputer_].eop_+1, (Byte)buffer[8]);
					p_Computer->setRam(conf[runningComputer_].string_, (Byte)buffer[9]+fAndMBasicOffset);
					p_Computer->setRam(conf[runningComputer_].string_+1, (Byte)buffer[10]);
					p_Computer->setRam(conf[runningComputer_].arrayValue_, (Byte)buffer[11]+fAndMBasicOffset);
					p_Computer->setRam(conf[runningComputer_].arrayValue_+1, (Byte)buffer[12]);
					p_Computer->setRam(conf[runningComputer_].eod_, (Byte)buffer[9]+fAndMBasicOffset);
					p_Computer->setRam(conf[runningComputer_].eod_+1, (Byte)buffer[10]);
					p_Main->eventSetLocation(false);
					start = 15;
				break;

				case 3: /* Regular F&M BASIC LOAD */
					if (runningComputer_ == COMX)
					{
						if (p_Comx->isFAndMBasicRunning())
							address = 0x6700;
						else
						{
							p_Main->errorMessage( "File " + conf[runningComputer_].loadFileNameFull_ + " can only be loaded in F&M Basic V2.00");
							return 1;
						}
					}
					else
					{
						p_Main->errorMessage( "File " + conf[runningComputer_].loadFileNameFull_ + " can only be loaded in COMX F&M Basic V2.00");
						return 1;
					}
					p_Computer->setRam(conf[runningComputer_].defus_, (Byte)buffer[5]);
					p_Computer->setRam(conf[runningComputer_].defus_+1, (Byte)buffer[6]);
					p_Computer->setRam(conf[runningComputer_].eop_, (Byte)buffer[7]);
					p_Computer->setRam(conf[runningComputer_].eop_+1, (Byte)buffer[8]);
					p_Computer->setRam(conf[runningComputer_].string_, (Byte)buffer[9]);
					p_Computer->setRam(conf[runningComputer_].string_+1, (Byte)buffer[10]);
					p_Computer->setRam(conf[runningComputer_].arrayValue_, (Byte)buffer[11]);
					p_Computer->setRam(conf[runningComputer_].arrayValue_+1, (Byte)buffer[12]);
					p_Computer->setRam(conf[runningComputer_].eod_, (Byte)buffer[9]);
					p_Computer->setRam(conf[runningComputer_].eod_+1, (Byte)buffer[10]);
					p_Main->eventSetLocation(false);
					start = 15;
				break;

				case 4: /* Old, incorrect Data only LOAD */
					address = p_Computer->getRam(conf[runningComputer_].string_) << 8;
					address += p_Computer->getRam(conf[runningComputer_].string_+1);
					start = 5;
					dataEnd = address + (int)length - start;
					p_Computer->setRam(conf[runningComputer_].eod_, (dataEnd >> 8) & 0xff);
					p_Computer->setRam(conf[runningComputer_].eod_+1, dataEnd & 0xff);
				break;

				case 5: /* Data only LOAD */
					address = p_Computer->getRam(conf[runningComputer_].arrayValue_) << 8;
					address += p_Computer->getRam(conf[runningComputer_].arrayValue_+1);
					start = 7;
					arrayLength = (Byte)buffer[5] << 8;
					arrayLength += (Byte)buffer[6];
					dataEnd = address + (int)length - start;
					p_Computer->setRam(conf[runningComputer_].eod_, (dataEnd >> 8) & 0xff);
					p_Computer->setRam(conf[runningComputer_].eod_+1, dataEnd & 0xff);
					stringStart = address + (int)arrayLength;
					p_Computer->setRam(conf[runningComputer_].string_, (stringStart >> 8) & 0xff);
					p_Computer->setRam(conf[runningComputer_].string_+1, stringStart & 0xff);
				break;

				case 6: /* New Regular LOAD */
					fAndMBasicOffset = 0;
					address = conf[runningComputer_].basicRamAddress_;
					highRamAddress = (conf[runningComputer_].basicRamAddress_ & 0xff00) >> 8;
					if (runningComputer_ == COMX)
					{
						if (p_Comx->isFAndMBasicRunning())
						{
							fAndMBasicOffset = 0x23;
							if (buffer[5] != 0)
							{
								p_Main->errorMessage("File " + conf[runningComputer_].loadFileNameFull_ + " can only be loaded in COMX Basic");
								return 1;
							}
							address = 0x6700;
						}
					}
					p_Computer->setRam(conf[runningComputer_].defus_, (Byte)buffer[5]+fAndMBasicOffset+highRamAddress);
					p_Computer->setRam(conf[runningComputer_].defus_+1, (Byte)buffer[6]);
					p_Computer->setRam(conf[runningComputer_].eop_, (Byte)buffer[7]+fAndMBasicOffset+highRamAddress);
					p_Computer->setRam(conf[runningComputer_].eop_+1, (Byte)buffer[8]);
					p_Computer->setRam(conf[runningComputer_].string_, (Byte)buffer[9]+fAndMBasicOffset+highRamAddress);
					p_Computer->setRam(conf[runningComputer_].string_+1, (Byte)buffer[10]);
					p_Computer->setRam(conf[runningComputer_].arrayValue_, (Byte)buffer[11]+fAndMBasicOffset+highRamAddress);
					p_Computer->setRam(conf[runningComputer_].arrayValue_+1, (Byte)buffer[12]);
					p_Computer->setRam(conf[runningComputer_].eod_, (Byte)buffer[9]+fAndMBasicOffset+highRamAddress);
					p_Computer->setRam(conf[runningComputer_].eod_+1, (Byte)buffer[10]);
					p_Main->eventSetLocation(false);
					start = 15;
				break;
			}

			startAddress = address;
			for (size_t i = start; i<length; i++)
			{
				p_Computer->writeMem(address, (Byte)buffer[i], false);
				address++;
			}
			inputFile.Close();
			//		slot = 0;
			//		if (runningComputer_ == COMX)
			//			slot = p_Computer->getOutValue(1);
			//		p_Main->AssAddConfig(conf[runningComputer_].ramDir_, conf[runningComputer_].loadFileName_, startAddress, address, address, slot, port);

			return conf[selectedComputer_].saveExec_;
		}
		else
		{
/*			if (runningComputer_ == COMX)
			{
				if (p_Comx->isFAndMBasicRunning())
				{
					p_Main->errorMessage("File " + conf[runningComputer_].loadFileNameFull_ + " can only be loaded in COMX Basic");
					return 1;
				}
			}
			p_Computer->setRam(conf[runningComputer_].defus_, (Byte)buffer[0x281]);
			p_Computer->setRam(conf[runningComputer_].defus_+1, (Byte)buffer[0x282]);
			p_Computer->setRam(conf[runningComputer_].eop_, (Byte)buffer[0x283]);
			p_Computer->setRam(conf[runningComputer_].eop_+1, (Byte)buffer[0x284]);
			p_Computer->setRam(conf[runningComputer_].string_, (Byte)buffer[0x292]);
			p_Computer->setRam(conf[runningComputer_].string_+1, (Byte)buffer[0x293]);
			p_Computer->setRam(conf[runningComputer_].arrayValue_, (Byte)buffer[0x294]);
			p_Computer->setRam(conf[runningComputer_].arrayValue_+1, (Byte)buffer[0x295]);
			p_Computer->setRam(conf[runningComputer_].eod_, (Byte)buffer[0x299]);
			p_Computer->setRam(conf[runningComputer_].eod_+1, (Byte)buffer[0x29A]);
			p_Main->eventSetLocation(false);*/
			address = conf[selectedComputer_].saveStart_;
			if (address != 0)
			{
				p_Main->eventShowAddressPopup(address);

				int answer = p_Main->getAddressPopupAnswer();
#if defined (__WXMAC__)
				if (answer != wxID_YES)
					address = 0;
#else
				if (answer != wxID_NO)
					address = 0;
#endif
				if (answer == wxID_CANCEL)
					return false;
			}
			startAddress = address;
			for (size_t i = 0; i<length; i++)
			{
				p_Computer->writeMem(address, (Byte)buffer[i], false);
				address++;
			}
			inputFile.Close();
			p_Main->eventSetLocation(true, startAddress, address - 1, startAddress);
			return startAddress;
		}
	}
	else
	{
		p_Main->errorMessage("Error reading " + conf[runningComputer_].loadFileNameFull_);
		return 1;
	}
}

bool GuiMain::startCassetteLoad()
{
	if (conf[runningComputer_].autoCassetteLoad_)
		return startLoad();
    else
        return false;
}

void GuiMain::onRealCas(wxCommandEvent&WXUNUSED(event))
{
	if (computerRunning_ && !conf[selectedComputer_].realCassetteLoad_ && !p_Computer->getAudioInStatus())
		(void)wxMessageBox( "SDL audio input initialization or start\n"
							"failed, real cassette loading not possible" ,
						    "Emma 02", wxICON_ERROR | wxOK );
	else
	{
		conf[selectedComputer_].realCassetteLoad_ = !conf[selectedComputer_].realCassetteLoad_;
		setRealCas(selectedComputer_);
	}
}

void GuiMain::onWavFile(wxCommandEvent&WXUNUSED(event))
{
	if (computerRunning_ && (selectedComputer_ == runningComputer_))
		p_Computer->wavFile();
}

void GuiMain::setRealCas(int computerType)
{
	bool useTape = true;
	if (((computerType == ELFII) || (computerType == SUPERELF) || (computerType == ELF)) && (!elfConfiguration[computerType].useTape))
		useTape = false;

	if (!mode_.gui)
		return;

	if (conf[computerType].realCassetteLoad_)
	{
		conf[computerType].autoCassetteLoad_ = false;
		XRCCTRL(*this, "RealCasLoad"+computerInfo[computerType].gui, wxBitmapButton)->SetBitmapLabel(realCasOnBitmap);
		XRCCTRL(*this, "RealCasLoad"+computerInfo[computerType].gui, wxBitmapButton)->SetBitmapCurrent(realCasOnBitmap);
		XRCCTRL(*this, "RealCasLoad"+computerInfo[computerType].gui, wxBitmapButton)->SetBitmapFocus(realCasOnBitmap);
		XRCCTRL(*this, "RealCasLoad"+computerInfo[computerType].gui, wxBitmapButton)->SetToolTip("Disable real cassette loading");
		XRCCTRL(*this, "Turbo"+computerInfo[computerType].gui, wxCheckBox)->Enable(false);
		XRCCTRL(*this, "Turbo"+computerInfo[computerType].gui, wxCheckBox)->SetValue(false);
		turboGui(computerInfo[computerType].gui);
		XRCCTRL(*this, "AutoCasLoad"+computerInfo[computerType].gui, wxCheckBox)->Enable(false);
		XRCCTRL(*this, "AutoCasLoad"+computerInfo[computerType].gui, wxCheckBox)->SetValue(false);
		if (computerRunning_ && (computerType == runningComputer_))
		{
			XRCCTRL(*this, "CasLoad"+computerInfo[computerType].gui, wxButton)->Enable(false);
			XRCCTRL(*this, "CasSave"+computerInfo[computerType].gui, wxButton)->Enable(!conf[computerType].autoCassetteLoad_);
		}
	}
	else
	{
		XRCCTRL(*this, "RealCasLoad"+computerInfo[computerType].gui, wxBitmapButton)->SetBitmapLabel(realCasOffBitmap);
		XRCCTRL(*this, "RealCasLoad"+computerInfo[computerType].gui, wxBitmapButton)->SetBitmapCurrent(realCasOffBitmap);
		XRCCTRL(*this, "RealCasLoad"+computerInfo[computerType].gui, wxBitmapButton)->SetBitmapFocus(realCasOffBitmap);
		XRCCTRL(*this, "RealCasLoad"+computerInfo[computerType].gui, wxBitmapButton)->SetToolTip("Enable real cassette loading");
		XRCCTRL(*this, "Turbo"+computerInfo[computerType].gui, wxCheckBox)->Enable(useTape);
		XRCCTRL(*this, "AutoCasLoad"+computerInfo[computerType].gui, wxCheckBox)->Enable(useTape);
		if (computerRunning_ && (computerType == runningComputer_))
		{
			XRCCTRL(*this, "CasLoad"+computerInfo[computerType].gui, wxButton)->Enable(!conf[computerType].autoCassetteLoad_);
			XRCCTRL(*this, "CasSave"+computerInfo[computerType].gui, wxButton)->Enable(!conf[computerType].autoCassetteLoad_);
		}
	}
	if (computerRunning_ && (computerType == runningComputer_))
		p_Computer->record_pause(conf[computerType].realCassetteLoad_);
}

void GuiMain::setRealCas2(int computerType)
{
	bool useTape = true;
	if (((computerType == ELFII) || (computerType == SUPERELF) || (computerType == ELF)) && (!elfConfiguration[computerType].useTape))
		useTape = false;

	if (!mode_.gui)
		return;

	if (conf[computerType].realCassetteLoad_)
	{
		XRCCTRL(*this, "Turbo"+computerInfo[computerType].gui, wxCheckBox)->Enable(false);
		XRCCTRL(*this, "Turbo"+computerInfo[computerType].gui, wxCheckBox)->SetValue(false);
		turboGui(computerInfo[computerType].gui);
		XRCCTRL(*this, "AutoCasLoad"+computerInfo[computerType].gui, wxCheckBox)->Enable(false);
		XRCCTRL(*this, "AutoCasLoad"+computerInfo[computerType].gui, wxCheckBox)->SetValue(false);
		conf[computerType].autoCassetteLoad_ = false;
		if (computerRunning_ && (computerType == runningComputer_))
		{
			XRCCTRL(*this, "CasLoad"+computerInfo[computerType].gui, wxButton)->Enable(false);
			XRCCTRL(*this, "CasSave"+computerInfo[computerType].gui, wxButton)->Enable(!conf[computerType].autoCassetteLoad_);
		}
	}
	else
	{
		XRCCTRL(*this, "Turbo"+computerInfo[computerType].gui, wxCheckBox)->Enable(useTape);
		XRCCTRL(*this, "AutoCasLoad"+computerInfo[computerType].gui, wxCheckBox)->Enable(useTape);
		if (computerRunning_ && (computerType == runningComputer_))
		{
			XRCCTRL(*this, "CasLoad"+computerInfo[computerType].gui, wxButton)->Enable(!conf[computerType].autoCassetteLoad_);
			XRCCTRL(*this, "CasSave"+computerInfo[computerType].gui, wxButton)->Enable(!conf[computerType].autoCassetteLoad_);
		}
	}
}

void GuiMain::setRealCasOff(int computerType)
{
	bool useTape = true;
	if (((computerType == ELFII) || (computerType == SUPERELF) || (computerType == ELF)) && (!elfConfiguration[computerType].useTape))
		useTape = false;

	conf[computerType].realCassetteLoad_ = false;

	if (!mode_.gui)
		return;

	XRCCTRL(*this, "RealCasLoad"+computerInfo[computerType].gui, wxBitmapButton)->SetBitmapLabel(realCasOffBitmap);
	XRCCTRL(*this, "RealCasLoad"+computerInfo[computerType].gui, wxBitmapButton)->SetBitmapCurrent(realCasOffBitmap);
	XRCCTRL(*this, "RealCasLoad"+computerInfo[computerType].gui, wxBitmapButton)->SetBitmapFocus(realCasOffBitmap);
	XRCCTRL(*this, "RealCasLoad"+computerInfo[computerType].gui, wxBitmapButton)->SetToolTip("Enable real cassette loading");
	XRCCTRL(*this, "Turbo"+computerInfo[computerType].gui, wxCheckBox)->Enable(useTape);
	XRCCTRL(*this, "AutoCasLoad"+computerInfo[computerType].gui, wxCheckBox)->Enable(useTape);
	if (computerRunning_ && (computerType == runningComputer_))
	{
		XRCCTRL(*this, "CasLoad"+computerInfo[computerType].gui, wxButton)->Enable(!conf[computerType].autoCassetteLoad_);
		XRCCTRL(*this, "CasSave"+computerInfo[computerType].gui, wxButton)->Enable(!conf[computerType].autoCassetteLoad_);
	}
}

void GuiMain::setPrinterState(int computerType)
{
	if (conf[computerType].printerOn_)
	{
		XRCCTRL(*this, "PrintButton"+computerInfo[computerType].gui, wxBitmapButton)->SetBitmapLabel(printerOnBitmap);
		XRCCTRL(*this,"PrintButton"+computerInfo[computerType].gui, wxBitmapButton)->SetToolTip("Disable printer support");
	}
	else
	{
		XRCCTRL(*this, "PrintButton"+computerInfo[computerType].gui, wxBitmapButton)->SetBitmapLabel(printerOffBitmap);
		XRCCTRL(*this,"PrintButton"+computerInfo[computerType].gui, wxBitmapButton)->SetToolTip("Enable printer support");
	}
}

bool GuiMain::startLoad()
{
	wxString filePath, fileName;

	filePath = conf[runningComputer_].wavFileDir_;
	fileName = conf[runningComputer_].wavFile_;
	filePath.operator += (fileName);

	if (fileName.Len() != 0)
	{
		if (wxFile::Exists(filePath))
		{
			p_Main->eventSetTapeState(TAPE_PLAY);
			return p_Computer->ploadStartTape(filePath);
		}
	}
    return false;
}

void GuiMain::stopCassette()
{
	if (conf[runningComputer_].autoCassetteLoad_)
		p_Computer->stopTape();
}

void GuiMain::startCassetteSave()
{
	if (conf[runningComputer_].autoCassetteLoad_)
		startSave();
}

void GuiMain::startSave()
{
	wxString filePath, fileName;

	filePath = conf[runningComputer_].wavFileDir_;
	fileName = conf[runningComputer_].wavFile_;
	filePath.operator += (fileName);

	if (fileName.Len() == 0)
	{
		fileName = p_Main->eventShowFileSelector( "Select the WAV file to save/load",
                       conf[runningComputer_].wavFileDir_, fileName,
                       "wav",
                       wxString::Format
                      (
                           "WAV File (*.wav)|*.wav|All files (%s)|%s",
                           wxFileSelectorDefaultWildcardStr,
                           wxFileSelectorDefaultWildcardStr
                       ),
                       wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_OVERWRITE_PROMPT
                      );
		if (!fileName)
		{
			if ((runningComputer_ == COMX) || (runningComputer_ == ETI))
				p_Computer->keyClear();
			return;
		}

		wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
		conf[runningComputer_].wavFile_ = FullPath.GetFullName();
		conf[runningComputer_].wavFileDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

		p_Main->eventSetTextValue("WavFile"+computerInfo[runningComputer_].gui, conf[runningComputer_].wavFile_);

		filePath = conf[runningComputer_].wavFileDir_;
		filePath.operator += (conf[runningComputer_].wavFile_);
	}
	if (wxFile::Exists(filePath))
	{
        p_Main->eventShowMessageBox( fileName+" already exists.\n"+"Do you want to replace it?",
                                    "Confirm Save As", wxICON_EXCLAMATION | wxYES_NO);

		if (messageBoxAnswer_ == wxNO)
		{
			fileName = p_Main->eventShowFileSelector( "Select the WAV file to save/load",
                           conf[runningComputer_].wavFileDir_, fileName,
                           "wav",
                           wxString::Format
                          (
                               "WAV File (*.wav)|*.wav|All files (%s)|%s",
                               wxFileSelectorDefaultWildcardStr,
                               wxFileSelectorDefaultWildcardStr
                           ),
                           wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_OVERWRITE_PROMPT
                          );
			if (!fileName)
			{
				if ((runningComputer_ == COMX) || (runningComputer_ == ETI))
					p_Computer->keyClear();
				return;
			}

			wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
			conf[runningComputer_].wavFile_ = FullPath.GetFullName();
			conf[runningComputer_].wavFileDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

			p_Main->eventSetTextValue("WavFile"+computerInfo[runningComputer_].gui, conf[runningComputer_].wavFile_);

			filePath = conf[runningComputer_].wavFileDir_;
			filePath.operator += (conf[runningComputer_].wavFile_);
		}
		if ((runningComputer_ == COMX) || (runningComputer_ == ETI))
			p_Computer->keyClear();
	}
	p_Main->eventSetTapeState(TAPE_RECORD);
	p_Computer->psaveStartTape(filePath);
}

void GuiMain::onTerminalSave(wxCommandEvent&WXUNUSED(event))
{
    startTerminalSave();
}

void GuiMain::onTerminalLoad(wxCommandEvent&WXUNUSED(event))
{
    startTerminalLoad();
}

void GuiMain::startAutoTerminalLoad()
{
	if (runningComputer_ != MEMBER)
		return;

	if (conf[runningComputer_].autoCassetteLoad_)
		startTerminalLoad();
}

void GuiMain::startTerminalLoad()
{
    if (terminalSave_ || terminalLoad_)
        return;

    terminalLoad_ = true;
    
    wxString filePath, fileName;
    
    filePath = conf[runningComputer_].wavFileDir_;
    fileName = conf[runningComputer_].wavFile_;
    filePath.operator += (fileName);
    
    if (fileName.Len() != 0)
    {
        if (wxFile::Exists(filePath))
        {
            p_Main->eventSetTapeState(TAPE_PLAY);
            p_Computer->terminalLoad(filePath);
        }
    }
}

void GuiMain::onTerminalStop(wxCommandEvent&WXUNUSED(event))
{
    if (!terminalSave_ && !terminalLoad_)
        return;
    
    stopTerminal();
	p_Computer->terminalStop();
}

void GuiMain::stopAutoTerminal()
{
    if (!terminalSave_ && !terminalLoad_)
        return;
    
    if (conf[runningComputer_].autoCassetteLoad_)
    {
        stopTerminal();
        p_Computer->terminalStop();
    }
}

void GuiMain::stopTerminal()
{
    terminalSave_ = false;
    terminalLoad_ = false;
    
    p_Main->eventSetTapeState(TAPE_STOP);
}

void GuiMain::startAutoTerminalSave()
{
	if (runningComputer_ != MEMBER)
		return;

	if (conf[runningComputer_].autoCassetteLoad_)
        startTerminalSave();
}

void GuiMain::startTerminalSave()
{
    if (terminalSave_ || terminalLoad_)
        return;

    terminalSave_ = true;

    wxString filePath, fileName;
    
    filePath = conf[runningComputer_].wavFileDir_;
    fileName = conf[runningComputer_].wavFile_;
    filePath.operator += (fileName);
    
    if (fileName.Len() == 0)
    {
        fileName = p_Main->eventShowFileSelector( "Select the terminal file to save",
                                                 conf[runningComputer_].wavFileDir_, fileName,
                                                 "hex|txt",
                                                 wxString::Format
                                                 (
													"Terminal File (*.hex)|*.hex|Tiny Basic File (*.txt)|*.txt|All files (%s)|%s",
													wxFileSelectorDefaultWildcardStr,
													wxFileSelectorDefaultWildcardStr
                                                  ),
                                                 wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW
                                                 );
        if (!fileName)
            return;
        
        wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
        conf[runningComputer_].wavFile_ = FullPath.GetFullName();
        conf[runningComputer_].wavFileDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
        
        p_Main->eventSetTextValue("WavFile"+computerInfo[runningComputer_].gui, conf[runningComputer_].wavFile_);
        
        filePath = conf[runningComputer_].wavFileDir_;
        filePath.operator += (conf[runningComputer_].wavFile_);
    }
    if (wxFile::Exists(filePath))
    {
        p_Main->eventShowMessageBox( fileName+" already exists.\n"+"Do you want to replace it?",
                                    "Confirm Save As", wxICON_EXCLAMATION | wxYES_NO);
        
        if (messageBoxAnswer_ == wxNO)
        {
            fileName = p_Main->eventShowFileSelector( "Select the terminal file to save",
                                                     conf[runningComputer_].wavFileDir_, fileName,
                                                     "hex|txt",
                                                     wxString::Format
                                                     (
														"Terminal File (*.hex)|*.hex|Tiny Basic File (*.txt)|*.txt|All files (%s)|%s",
														wxFileSelectorDefaultWildcardStr,
														wxFileSelectorDefaultWildcardStr
                                                      ),
                                                     wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_OVERWRITE_PROMPT
                                                     );
            if (!fileName)
                return;
            
            wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
            conf[runningComputer_].wavFile_ = FullPath.GetFullName();
            conf[runningComputer_].wavFileDir_ = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
            
            p_Main->eventSetTextValue("WavFile"+computerInfo[runningComputer_].gui, conf[runningComputer_].wavFile_);
            
            filePath = conf[runningComputer_].wavFileDir_;
            filePath.operator += (conf[runningComputer_].wavFile_);
        }
 //       if (messageBoxAnswer_ == wxYES)
 //           wxRemoveFile(filePath);
    }
    p_Main->eventSetTapeState(TAPE_RECORD);
    p_Computer->terminalSave(filePath);
}

void GuiMain::turboOn()
{
	if (turboOn_)  return;

	if (conf[runningComputer_].turbo_)
	{
		if (mode_.gui)
			clockTextCtrl[runningComputer_]->Enable(false);
		savedSpeed_ = conf[runningComputer_].clockSpeed_;

		wxString clock =  conf[runningComputer_].turboClock_;
		clock.ToDouble((double*)&conf[runningComputer_].clockSpeed_);

		setClockRate();
		turboOn_ = true;
	}
}

void GuiMain::turboOff()
{
	if (!turboOn_)  return;
	if (conf[runningComputer_].turbo_)
	{
		conf[runningComputer_].clockSpeed_ = savedSpeed_;
		setClockRate();

		if (mode_.gui)
			clockTextCtrl[runningComputer_]->Enable(true);
		turboOn_ = false;
	}
}

void GuiMain::enableStartButtonGui(bool status)
{
	for (int i=0; i<NO_COMPUTER; i++)
	{
        startButton[i]->Enable(status);
        stopButton[i]->Enable(false);
	}

	if (status)
	{
		startButton[runningComputer_]->SetLabel("Start");
		startButton[runningComputer_]->SetToolTip("Start " + computerInfo[runningComputer_].name + " emulator (F12)");
	}
	else
	{
		startButton[runningComputer_]->SetLabel("Reset");
		startButton[runningComputer_]->SetToolTip("Reset " + computerInfo[runningComputer_].name + " emulator (F12)");
        startButton[runningComputer_]->Enable(true);
        stopButton[runningComputer_]->Enable(true);
	}
}

void GuiMain::enableLocationGui()
{
	if (!mode_.gui)
		return;

	XRCCTRL(*this, "TextStart"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(conf[runningComputer_].useLoadLocation_);
	XRCCTRL(*this, "TextEnd"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(conf[runningComputer_].useLoadLocation_);
	XRCCTRL(*this, "TextExec"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(conf[runningComputer_].useLoadLocation_);
	XRCCTRL(*this, "SaveStart"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(conf[runningComputer_].useLoadLocation_);
	XRCCTRL(*this, "SaveEnd"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(conf[runningComputer_].useLoadLocation_);
	XRCCTRL(*this, "SaveExec"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(conf[runningComputer_].useLoadLocation_);
}

void GuiMain::enableMemAccessGui(bool status)
{
	if (mode_.gui)
	{
		XRCCTRL(*this, "SaveButton"+computerInfo[runningComputer_].gui, wxButton)->Enable(status);
		XRCCTRL(*this, "LoadButton"+computerInfo[runningComputer_].gui, wxButton)->Enable(status);
	}

	bool superBasic = false;
	bool disableAll = false;
	if (computerRunning_)
		superBasic = ((p_Computer->getLoadedProgram()&0x1) == 0x1);
	else
	{
		if ((runningComputer_ == ELF) || (runningComputer_ == ELFII) || (runningComputer_ == SUPERELF) || (runningComputer_ == VIP))
			disableAll = true;
	}
	if (superBasic)
	{
		switch (p_Computer->getLoadedProgram())
		{
			case SUPERBASICV1:
				conf[runningComputer_].defus_ = 0x2a81;
				conf[runningComputer_].eop_ = 0x2a83;
				conf[runningComputer_].string_ = 0x2a92;
				conf[runningComputer_].arrayValue_ = 0x2a94;
				conf[runningComputer_].eod_ = 0x2a99;
				conf[runningComputer_].basicRamAddress_ = 0x2c00;
				computerInfo[runningComputer_].ploadExtension = "super";
			break;
			case SUPERBASICV3:
				conf[runningComputer_].defus_ = 0x2f81;
				conf[runningComputer_].eop_ = 0x2f83;
				conf[runningComputer_].string_ = 0x2f92;
				conf[runningComputer_].arrayValue_ = 0x2f94;
				conf[runningComputer_].eod_ = 0x2f99;
				conf[runningComputer_].basicRamAddress_ = 0x3100;
				computerInfo[runningComputer_].ploadExtension = "super";
			break;
			case SUPERBASICV5:
				conf[runningComputer_].defus_ = 0x3581;
				conf[runningComputer_].eop_ = 0x3583;
				conf[runningComputer_].string_ = 0x3592;
				conf[runningComputer_].arrayValue_ = 0x3594;
				conf[runningComputer_].eod_ = 0x3599;
				conf[runningComputer_].basicRamAddress_ = 0x3700;
				computerInfo[runningComputer_].ploadExtension = "super";
			break;
			case SUPERBASICV6:
				conf[runningComputer_].defus_ = 0x3581;
				conf[runningComputer_].eop_ = 0x3583;
				conf[runningComputer_].string_ = 0x3592;
				conf[runningComputer_].arrayValue_ = 0x3594;
				conf[runningComputer_].eod_ = 0x3599;
				conf[runningComputer_].basicRamAddress_ = 0x3700;
				computerInfo[runningComputer_].ploadExtension = "super";
			break;
			case RCABASIC3:
				conf[runningComputer_].defus_ = 0x5281;
				conf[runningComputer_].eop_ = 0x5283;
				conf[runningComputer_].string_ = 0x5292;
				conf[runningComputer_].arrayValue_ = 0x5294;
				conf[runningComputer_].eod_ = 0x5299;
				conf[runningComputer_].basicRamAddress_ = 0x5400;
				computerInfo[runningComputer_].ploadExtension = "rca";
			break;
			case RCABASIC4:
				conf[runningComputer_].defus_ = 0x5581;
				conf[runningComputer_].eop_ = 0x5583;
				conf[runningComputer_].string_ = 0x5592;
				conf[runningComputer_].arrayValue_ = 0x5594;
				conf[runningComputer_].eod_ = 0x5599;
				conf[runningComputer_].basicRamAddress_ = 0x5700;
				computerInfo[runningComputer_].ploadExtension = "rca";
			break;
			case FPBBASIC:
				conf[runningComputer_].defus_ = 0x4081;
				conf[runningComputer_].eop_ = 0x4083;
				conf[runningComputer_].string_ = 0x4092;
				conf[runningComputer_].arrayValue_ = 0x4094;
				conf[runningComputer_].eod_ = 0x4099;
				conf[runningComputer_].basicRamAddress_ = 0x4200;
				computerInfo[runningComputer_].ploadExtension = "fpb";
			break;
		}
	}
	if (!mode_.gui)
		return;
	if ((runningComputer_ == MCDS) || (runningComputer_ == COMX) || (runningComputer_ == PECOM) || (runningComputer_ == TMC600) || (runningComputer_ == VIPII) || (runningComputer_ == VIP) || superBasic || disableAll)
	{
		XRCCTRL(*this, "RunButton"+computerInfo[runningComputer_].gui, wxButton)->Enable(status);
		XRCCTRL(*this, "UseLocation"+computerInfo[runningComputer_].gui, wxCheckBox)->Enable(status);
		XRCCTRL(*this, "DsaveButton"+computerInfo[runningComputer_].gui, wxButton)->Enable(status);
		enableLocationGui();
	}
	else
	{
//		XRCCTRL(*this, "RunButton"+computerInfo[runningComputer_].gui, wxButton)->Enable(false);
//		XRCCTRL(*this, "UseLocation"+computerInfo[runningComputer_].gui, wxCheckBox)->Enable(false);
//		XRCCTRL(*this, "DsaveButton"+computerInfo[runningComputer_].gui, wxButton)->Enable(false);
		XRCCTRL(*this, "TextStart"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(status);
		XRCCTRL(*this, "TextEnd"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(status);
		XRCCTRL(*this, "SaveStart"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(status);
		XRCCTRL(*this, "SaveEnd"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(status);
	}
}

void GuiMain::enableTapeGui(bool status, int computerType)
{
	XRCCTRL(*this, "CasButton"+computerInfo[computerType].gui, wxButton)->Enable(status);
	XRCCTRL(*this, "WavFile"+computerInfo[computerType].gui, wxTextCtrl)->Enable(status);
	XRCCTRL(*this, "EjectCas"+computerInfo[computerType].gui, wxButton)->Enable(status);
	XRCCTRL(*this, "AutoCasLoad"+computerInfo[computerType].gui, wxCheckBox)->Enable(status&!conf[computerType].realCassetteLoad_);
	XRCCTRL(*this, "Turbo"+computerInfo[computerType].gui, wxCheckBox)->Enable(status&!conf[computerType].realCassetteLoad_);
	XRCCTRL(*this, "TurboClock"+computerInfo[computerType].gui, wxTextCtrl)->Enable(status);
	XRCCTRL(*this, "TurboMhzText"+computerInfo[computerType].gui, wxStaticText)->Enable(status);
    XRCCTRL(*this, "RealCasLoad"+computerInfo[computerType].gui, wxBitmapButton)->Enable(status);
//	XRCCTRL(*this, "Volume"+computerInfo[computerType].gui, wxSlider)->Enable(status);
//	XRCCTRL(*this, "VolumeText"+computerInfo[computerType].gui, wxStaticText)->Enable(status);
}

void GuiMain::enableLoadGui(bool status)
{
	enableMemAccessGui(status);
	if (((runningComputer_ == ELFII) || (runningComputer_ == SUPERELF) || (runningComputer_ == ELF)) && (!elfConfiguration[runningComputer_].useTape))
	{
		enableTapeGui(false, runningComputer_);
		return;
	}
	if (computerRunning_)
	{
		XRCCTRL(*this, "CasButton"+computerInfo[runningComputer_].gui, wxButton)->Enable(status);
		XRCCTRL(*this, "WavFile"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(status);
		XRCCTRL(*this, "EjectCas"+computerInfo[runningComputer_].gui, wxButton)->Enable(status);
		XRCCTRL(*this, "AutoCasLoad"+computerInfo[runningComputer_].gui, wxCheckBox)->Enable(status&!conf[runningComputer_].realCassetteLoad_);
		XRCCTRL(*this, "Turbo"+computerInfo[runningComputer_].gui, wxCheckBox)->Enable(status&!conf[runningComputer_].realCassetteLoad_);
		if (!status)
		{
			XRCCTRL(*this, "TurboClock"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(false);
			XRCCTRL(*this, "TurboMhzText"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(false);
        }
		else
		{
			XRCCTRL(*this, "TurboClock"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(XRCCTRL(*this, "Turbo"+computerInfo[runningComputer_].gui, wxCheckBox)->IsChecked());
			XRCCTRL(*this, "TurboMhzText"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(XRCCTRL(*this, "Turbo"+computerInfo[runningComputer_].gui, wxCheckBox)->IsChecked());
        }
	}
	else
	{
		XRCCTRL(*this, "AutoCasLoad"+computerInfo[runningComputer_].gui, wxCheckBox)->Enable(true);
		XRCCTRL(*this, "Turbo"+computerInfo[runningComputer_].gui, wxCheckBox)->Enable(true);
		XRCCTRL(*this, "CasButton"+computerInfo[runningComputer_].gui, wxButton)->Enable(true);
		XRCCTRL(*this, "WavFile"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(true);
		XRCCTRL(*this, "EjectCas"+computerInfo[runningComputer_].gui, wxButton)->Enable(true);
		XRCCTRL(*this, "TurboClock"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(true);
		XRCCTRL(*this, "TurboMhzText"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(true);
    }
	if (!conf[runningComputer_].autoCassetteLoad_)
	{
		if (runningComputer_ == FRED1 || runningComputer_ == FRED2)
			XRCCTRL(*this, "CasPause"+computerInfo[runningComputer_].gui, wxButton)->Enable(false);
		XRCCTRL(*this, "CasStop"+computerInfo[runningComputer_].gui, wxButton)->Enable(false);
		XRCCTRL(*this, "CasLoad"+computerInfo[runningComputer_].gui, wxButton)->Enable(status&!conf[runningComputer_].realCassetteLoad_);
		XRCCTRL(*this, "CasSave"+computerInfo[runningComputer_].gui, wxButton)->Enable(status);
	}
	if (tapeState_ == TAPE_RECORD)
		XRCCTRL(*this, "CasSave"+computerInfo[runningComputer_].gui, wxBitmapButton)->SetBitmapLabel(recOffBitmap);
	if (tapeState_ == TAPE_PLAY)
		XRCCTRL(*this, "CasLoad"+computerInfo[runningComputer_].gui, wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
	tapeState_ = TAPE_STOP;
}

void GuiMain::setTapeState(int tapeState)
{
	tapeState_ = tapeState;
	if (!mode_.gui)
		return;

    if (runningComputer_ == FRED1 || runningComputer_ == FRED2)
    {
        if (tapeState == TAPE_PAUSE)
        {
            XRCCTRL(*this, "CasPause"+computerInfo[runningComputer_].gui, wxBitmapButton)->SetBitmapLabel(pauseOnBitmap);
            tapeState = TAPE_STOP;
        }
        else
            XRCCTRL(*this, "CasPause"+computerInfo[runningComputer_].gui, wxBitmapButton)->SetBitmapLabel(pauseOffBitmap);
    }
    
	XRCCTRL(*this, "CasButton"+computerInfo[runningComputer_].gui, wxButton)->Enable(tapeState == TAPE_STOP);
	XRCCTRL(*this, "WavFile"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(tapeState == TAPE_STOP);
	XRCCTRL(*this, "EjectCas"+computerInfo[runningComputer_].gui, wxButton)->Enable(tapeState == TAPE_STOP);
	XRCCTRL(*this, "AutoCasLoad"+computerInfo[runningComputer_].gui, wxCheckBox)->Enable((tapeState == TAPE_STOP)&!conf[runningComputer_].realCassetteLoad_);
	XRCCTRL(*this, "Turbo"+computerInfo[runningComputer_].gui, wxCheckBox)->Enable((tapeState == TAPE_STOP)&!conf[runningComputer_].realCassetteLoad_);
    if (tapeState != TAPE_STOP)
	{
		XRCCTRL(*this, "TurboClock"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(false);
		XRCCTRL(*this, "TurboMhzText"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(false);
    }
	else
	{
		XRCCTRL(*this, "TurboClock"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(XRCCTRL(*this, "Turbo"+computerInfo[runningComputer_].gui, wxCheckBox)->IsChecked());
		XRCCTRL(*this, "TurboMhzText"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(XRCCTRL(*this, "Turbo"+computerInfo[runningComputer_].gui, wxCheckBox)->IsChecked());
    }
	if (conf[runningComputer_].autoCassetteLoad_)
	{
		if (tapeState == TAPE_RECORD)
			XRCCTRL(*this, "CasSave"+computerInfo[runningComputer_].gui, wxBitmapButton)->SetBitmapLabel(recOnBitmap);
		else
			XRCCTRL(*this, "CasSave"+computerInfo[runningComputer_].gui, wxBitmapButton)->SetBitmapLabel(recOffBitmap);
		if (tapeState == TAPE_PLAY)
			XRCCTRL(*this, "CasLoad"+computerInfo[runningComputer_].gui, wxBitmapButton)->SetBitmapLabel(playGreenBitmap);
		else
			XRCCTRL(*this, "CasLoad"+computerInfo[runningComputer_].gui, wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
	}
	else
	{
		if (runningComputer_ == FRED1 || runningComputer_ == FRED2)
			XRCCTRL(*this, "CasPause"+computerInfo[runningComputer_].gui, wxButton)->Enable(tapeState != TAPE_STOP);
		XRCCTRL(*this, "CasStop"+computerInfo[runningComputer_].gui, wxButton)->Enable(tapeState != TAPE_STOP);
		XRCCTRL(*this, "CasLoad"+computerInfo[runningComputer_].gui, wxButton)->Enable((tapeState == TAPE_STOP)&!conf[runningComputer_].realCassetteLoad_);
		XRCCTRL(*this, "CasSave"+computerInfo[runningComputer_].gui, wxButton)->Enable(tapeState == TAPE_STOP);
	}
}

void GuiMain::turboGui(wxString computerString)
{
	if (!mode_.gui)
		return;
	XRCCTRL(*this, "TurboClock"+computerString, wxTextCtrl)->Enable(conf[selectedComputer_].turbo_);
	XRCCTRL(*this, "TurboMhzText"+computerString, wxStaticText)->Enable(conf[selectedComputer_].turbo_);
}

void GuiMain::setComputerInfo(int id, wxString gui, wxString name, wxString ploadExtension)
{
	computerInfo[id].gui = gui;
	computerInfo[id].name = name;
	computerInfo[id].ploadExtension = ploadExtension;
}

void GuiMain::setScreenInfo(int id, int start, int end, wxString colour[])
{
	screenInfo[id].start = start;
	screenInfo[id].number = end;
	screenInfo[id].numberVideo = 0;
	for (int i=start; i<end; i++)
		screenInfo[id].defaultColour[i] = colour[i];
}

void GuiMain::setScreenInfo(int id, int start, int end, wxString colour[], int numberVideo, int borderX[], int borderY[])
{
	screenInfo[id].start = start;
	screenInfo[id].number = end;
	screenInfo[id].numberVideo = numberVideo;
	for (int i=start; i<end; i++)
		screenInfo[id].defaultColour[i] = colour[i];
	for (int i=0; i<screenInfo[id].numberVideo; i++)
	{
		screenInfo[id].borderX[i] = borderX[i];
		screenInfo[id].borderY[i] = borderY[i];
	}
}

ScreenInfo GuiMain::getScreenInfo(int id)
{
	return screenInfo[id];
}

void GuiMain::setBaudChoice(int computerType)
{
    XRCCTRL(*this, "VTBaudRText" + computerInfo[computerType].gui, wxStaticText)->Enable(elfConfiguration[computerType].useUart);
    XRCCTRL(*this, "VTBaudRChoice" + computerInfo[computerType].gui, wxChoice)->Enable(elfConfiguration[computerType].useUart);
}

void GuiMain::setBaud(int baudR, int baudT)
{
    XRCCTRL(*this, "VTBaudRChoice" + computerInfo[runningComputer_].gui, wxChoice)->SetSelection(baudR);
    XRCCTRL(*this, "VTBaudTChoice" + computerInfo[runningComputer_].gui, wxChoice)->SetSelection(baudT);
    elfConfiguration[runningComputer_].baudR = baudR;
    elfConfiguration[runningComputer_].baudT = baudT;
}

void GuiMain::onFullScreenFloat(wxCommandEvent&WXUNUSED(event))
{
	fullScreenFloat_ = !fullScreenFloat_;
}

void GuiMain::onLedTimer(wxCommandEvent&event)
{
    wxString stringMs = event.GetString();
    if (stringMs == "")  stringMs = "0";
    long ms;
    if (!stringMs.ToLong(&ms, 10))
        return;
    
    conf[selectedComputer_].ledTime_ = stringMs;
    conf[selectedComputer_].ledTimeMs_ = ms;
    
    if (computerRunning_ && (selectedComputer_ == runningComputer_))
        p_Computer->setLedMs(ms);
}

int GuiMain::getCpuType()
{
    cpuType_ = defaultCpuType_;
    if (runningComputer_ == FRED1)
    	cpuType_ = SYSTEM00;
	if (runningComputer_ == MICROTUTOR || runningComputer_ == COINARCADE || runningComputer_ == FRED2)
        cpuType_ = CPU1801;
    
    return cpuType_;
};

void GuiMain::onClearRam(wxCommandEvent&event)
{
	elfConfiguration[selectedComputer_].clearRam = event.IsChecked();
}

void GuiMain::onBootAddress(wxCommandEvent&WXUNUSED(event))
{
	conf[selectedComputer_].bootAddress_ = get16BitValue("BootAddress"+computerInfo[selectedComputer_].gui);
	if (conf[selectedComputer_].bootAddress_ == -1)  conf[selectedComputer_].bootAddress_ = 0;
}

long GuiMain::getBootAddress(wxString computerTypeStr, int computerType)
{
	if (mode_.gui)
	{
		wxString address;
		address.Printf("%04X", (unsigned int)conf[computerType].bootAddress_);
		XRCCTRL(*this,"BootAddress"+computerTypeStr, wxTextCtrl)->ChangeValue(address);
	}

	return conf[computerType].bootAddress_;
}

void GuiMain::onChoiceRam(wxCommandEvent&event)
{
	conf[selectedComputer_].ramType_ = event.GetSelection();
}
                              
bool GuiMain::checkWavFile(wxString fileName)
{
    wxFFile inFile;
    
    if (inFile.Open(fileName, "rb"))
    {
        unsigned char Riff [12];
        inFile.Read(Riff, 12);
        long fiePosition = 12;
        
        long fileSize = Riff [4] +(Riff [5]<<8) +(Riff [6]<<16) +(Riff [7]<<24);
        
        while(fiePosition < fileSize)
        {
            unsigned char chunkId [4];
            inFile.Read(chunkId, 4);
            
            unsigned char chunkSize [4];
            inFile.Read(chunkSize, 4);
            fiePosition += 8;
            
            long longChunkSize = chunkSize [0] +(chunkSize [1]<<8) +(chunkSize [2]<<16) +(chunkSize [3]<<24);
            
            if ((chunkId[0] == 'd') &&(chunkId[1] == 'a') &&(chunkId[2] == 't') &&(chunkId[3] == 'a'))
            {
                return true;
            }
            else
            {
                unsigned char* chunkData;
                chunkData = (unsigned char*) malloc( longChunkSize * sizeof *chunkData );
                
                fiePosition += longChunkSize;
                inFile.Read(chunkData, longChunkSize);
                
                if ((chunkId[0] == 'f') &&(chunkId[1] == 'm') &&(chunkId[2] == 't') &&(chunkId[3] == ' '))
                {
                    int compressionCode = chunkData [0] +(chunkData [1]<<8);
                    if (compressionCode != 1)
                    {
                        (void)wxMessageBox("Only PCM wav files are supported", "Emma 02", wxICON_ERROR | wxOK );
                        free (chunkData);
                        return false;
                    }
                    
                    int channelCount = chunkData [2] +(chunkData [3]<<8);
                    if (channelCount > 2)
                    {
                        (void)wxMessageBox("Only mono and stereo wav files are supported", "Emma 02", wxICON_ERROR | wxOK );
                        free (chunkData);
                        return false;
                    }
                }
                free (chunkData);
            }
        }
        (void)wxMessageBox("Data part of wav file not found, file might be corrupt", "Emma 02", wxICON_ERROR | wxOK );
        return false;
    }
    (void)wxMessageBox("Wav file could not be opened", "Emma 02", wxICON_ERROR | wxOK );
    return false;
}

bool GuiMain::showSoundError()
{
	bool showSoundError;

	configPointer->Read("/Main/ShowSoundError", &showSoundError, true);
	return showSoundError;
}

void GuiMain::hideSoundError()
{
	configPointer->Write("/Main/ShowSoundError", false);
}

bool GuiMain::showSplashScreen()
{
    wxString computerStr = getRunningComputerStr();
    int computer = getRunningComputerId();

    bool showSplashScreen = false;
    
    switch (computer)
    {
        case ELF:
        case ELFII:
        case SUPERELF:
            switch (p_Computer->getLoadedProgram())
            {
                case SUPERBASICV1:
                    if (elfConfiguration[computer].vtType != VTNONE)
                        configPointer->Read("/"+computerStr+"/ShowSplashScreenSBV1_VT", &showSplashScreen, true);
                    else
                        configPointer->Read("/"+computerStr+"/ShowSplashScreenSBV1_SUPER", &showSplashScreen, true);
                break;

                case SUPERBASICV3:
                    configPointer->Read("/"+computerStr+"/ShowSplashScreenSBV3_SUPER", &showSplashScreen, true);
                break;

                case RCABASIC3:
                    configPointer->Read("/"+computerStr+"/ShowSplashScreenRCA_BASIC", &showSplashScreen, true);
                break;
 
                case MINIMON:
                    configPointer->Read("/"+computerStr+"/ShowSplashScreenMinimon", &showSplashScreen, true);
                break;
                    
                case GOLDMON:
                    configPointer->Read("/"+computerStr+"/ShowSplashScreenGoldmon", &showSplashScreen, true);
                break;
                    
                case TINYBASIC:
                    if (elfConfiguration[computer].vtType != VTNONE)
                        configPointer->Read("/"+computerStr+"/ShowSplashScreenTINY_VT", &showSplashScreen, true);
                    else
                        configPointer->Read("/"+computerStr+"/ShowSplashScreenTINY", &showSplashScreen, true);
                break;
                    
                default:
                    if (p_Computer->getLoadedOs() == ELFOS && elfConfiguration[computer].vtType != VTNONE)
                        configPointer->Read("/"+computerStr+"/ShowSplashScreenELFOS", &showSplashScreen, true);
                break;
            }
        break;
            
        case ELF2K:
            if (elfConfiguration[computer].vtType != VTNONE)
                configPointer->Read("/"+computerStr+"/ShowSplashScreenElf2K", &showSplashScreen, true);
        break;
                
        default:
            configPointer->Read("/"+computerStr+"/ShowSplashScreen", &showSplashScreen, true);
        break;
    }
    return showSplashScreen;
}

void GuiMain::hideSplashScreen()
{
    wxString computerStr = getRunningComputerStr();
    int computer = getRunningComputerId();
    
    switch (computer)
    {
        case ELF:
        case ELFII:
        case SUPERELF:
            switch (p_Computer->getLoadedProgram())
            {
                case SUPERBASICV1:
                    if (elfConfiguration[computer].vtType != VTNONE)
                        configPointer->Write("/"+computerStr+"/ShowSplashScreenSBV1_VT", false);
                    else
                        configPointer->Write("/"+computerStr+"/ShowSplashScreenSBV1_SUPER", false);
                break;

                case SUPERBASICV3:
                    configPointer->Write("/"+computerStr+"/ShowSplashScreenSBV3_SUPER", false);
                break;

                case RCABASIC3:
                    configPointer->Write("/"+computerStr+"/ShowSplashScreenRCA_BASIC", false);
                break;
                    
                case MINIMON:
                    configPointer->Write("/"+computerStr+"/ShowSplashScreenMinimon", false);
                break;
                    
                case GOLDMON:
                    configPointer->Write("/"+computerStr+"/ShowSplashScreenGoldmon", false);
                break;
                    
                case TINYBASIC:
                    if (elfConfiguration[computer].vtType != VTNONE)
                        configPointer->Write("/"+computerStr+"/ShowSplashScreenTINY_VT", false);
                    else
                        configPointer->Write("/"+computerStr+"/ShowSplashScreenTINY", false);
                break;
                    
                default:
                    if (p_Computer->getLoadedOs() == ELFOS && elfConfiguration[computer].vtType != VTNONE)
                        configPointer->Write("/"+computerStr+"/ShowSplashScreenELFOS", false);
                break;
            }
        break;
            
        case ELF2K:
            if (elfConfiguration[computer].vtType != VTNONE)
                configPointer->Write("/"+computerStr+"/ShowSplashScreenElf2K", false);
        break;
            
        default:
            configPointer->Write("/"+computerStr+"/ShowSplashScreen", false);
        break;
    }
}


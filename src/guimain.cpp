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
#include "vtsetup.h"
#include "psave.h"
#include "wx/stdpaths.h"
#include "wx/fileconf.h"
#include "wx/wfstream.h"
#include "wxcurl_http.h"
#if !defined (_DEBUG)
#include "wxcurl_dialog.h"
#endif

#if defined (__WXMSW__)
// RTL_OSVERSIONINFOEXW is defined in winnt.h
BOOL GetOsVersion(RTL_OSVERSIONINFOEXW* pk_OsVer)
{
    typedef LONG(WINAPI* tRtlGetVersion)(RTL_OSVERSIONINFOEXW*);

    memset(pk_OsVer, 0, sizeof(RTL_OSVERSIONINFOEXW));
    pk_OsVer->dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);

    HMODULE h_NtDll = GetModuleHandleW(L"ntdll.dll");
    tRtlGetVersion f_RtlGetVersion = (tRtlGetVersion)GetProcAddress(h_NtDll, "RtlGetVersion");

    if (!f_RtlGetVersion)
        return FALSE; // This will never happen (all processes load ntdll.dll)

    LONG Status = f_RtlGetVersion(pk_OsVer);
    return Status == 0; // STATUS_SUCCESS;
}
#endif

BEGIN_EVENT_TABLE(GuiMain, wxFrame)

    EVT_MENU(XRCID("MI_Psave"), GuiMain::onPsaveMenu)
    EVT_MENU(XRCID("MI_Sb"), GuiComx::onSBDialog)
    EVT_MENU(XRCID("MI_FullScreenFloat"), GuiMain::onFullScreenFloat)
    EVT_COMMAND(wxID_ANY, OPEN_PRINTER_WINDOW, GuiMain::openPrinterFrame)

END_EVENT_TABLE()

GuiMain::GuiMain(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: wxFrame((wxFrame *)NULL, -1, title, pos, size) 
{
    terminalSave_ = false;
    terminalLoad_ = false;
    zoomPosition_ = 0;

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
    if (!wxFile::Exists(applicationDirectory_ + "main_11.xrc"))
    {
        applicationDirectory_ = wxStandardPaths::Get().GetExecutablePath();
        applicationDirectory_ = applicationDirectory_.Left(applicationDirectory_.Len()-11);
        applicationDirectory_ = applicationDirectory_ + "share" + pathSeparator_ + "emma_02" + pathSeparator_;
    }
#endif

    fontSizeString_ = configPointer->Read("/Main/FontSizeString", "11");
    windowInfo = getWinSizeInfo(applicationDirectory_, fontSizeString_);

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
    playDarkGreenBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/play_darkgreen.png", wxBITMAP_TYPE_PNG);

    rewindBlackBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/rewind_black.png", wxBITMAP_TYPE_PNG);
    rewindGreenBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/rewind_green.png", wxBITMAP_TYPE_PNG);
    rewindDarkGreenBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/rewind_darkgreen.png", wxBITMAP_TYPE_PNG);

    forwardBlackBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/forward_black.png", wxBITMAP_TYPE_PNG);
    forwardGreenBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/forward_green.png", wxBITMAP_TYPE_PNG);
    forwardDarkGreenBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/forward_darkgreen.png", wxBITMAP_TYPE_PNG);

    recOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/rec_off.png", wxBITMAP_TYPE_PNG);
    recOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/rec_on.png", wxBITMAP_TYPE_PNG);
    recPressedBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/rec_pressed.png", wxBITMAP_TYPE_PNG);

    pauseOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/pause_off.png", wxBITMAP_TYPE_PNG);
    pauseOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/pause_on.png", wxBITMAP_TYPE_PNG);
    
    realCasOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/real_cas_on.png", wxBITMAP_TYPE_PNG);
    realCasOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/real_cas_off.png", wxBITMAP_TYPE_PNG);

    printerOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/print_on.png", wxBITMAP_TYPE_PNG);
    printerOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/print_off.png", wxBITMAP_TYPE_PNG);

    ejectBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/eject.png", wxBITMAP_TYPE_PNG);

    tapeState_ = TAPE_STOP;
    turboOn_ = false;
    oldXmlFileName_ = "";
    batchConvertActive_ = false;
}

WindowInfo GuiMain::getWinSizeInfo(wxString appDir, wxString fontSizeString)
{
    WindowInfo returnValue;
    wxString windowInfoFile;
    int major, minor;
    
    returnValue.errorMessage = "";
    wxGetOsVersion(&major, &minor);
    
    wxConfigBase *windowConfigPointer;
    
    returnValue.bitness = wxPlatformInfo::Get().GetBitness();
    wxString cpuArchitecture = wxPlatformInfo::Get().GetCpuArchitectureName();

    returnValue.arm = (cpuArchitecture == "arm64" || cpuArchitecture == "aarch64");

#if defined (__WXMAC__)
    windowInfoFile = "osx_" + fontSizeString + ".ini";
    wxString appName = "emma_02";
    
    returnValue.operatingSystem = OS_MAC;
    if ((major == 10 && minor <= 9) || returnValue.bitness == wxBITNESS_32)
        returnValue.operatingSystem = OS_MAC_PRE_10_10;
#endif
   
#if defined (__linux__)
    wxLinuxDistributionInfo distInfo;

    distInfo = wxPlatformInfo::Get().GetLinuxDistributionInfo();
    returnValue.packageDeb = true;

    if (distInfo.Id == "Ubuntu")
    {
        switch (major)
        {
            case 2:
                distInfo.Id += ".2";
            break;
                
            case 3:
                distInfo.Id += ".3";
            break;

            default:
                distInfo.Id += ".4";
            break;
        }
    }

    windowInfoFile = distInfo.Id + "_" + fontSizeString + ".ini";

    returnValue.suse = false;
    if (distInfo.Id == "")
    {
        distInfo.Id = wxPlatformInfo::Get().GetOperatingSystemDescription();
        
        windowInfoFile = "suse_" + fontSizeString + ".ini";
        returnValue.suse = true;
        returnValue.packageDeb = false;

        if (distInfo.Id.Find("fc") != wxNOT_FOUND) // Fedor is something like: Linux 4.11.11-300.fc26.x86_64 x86_64
        {
            windowInfoFile = "fedora_" + fontSizeString + ".ini";
            returnValue.suse = false;
        }
    }
    
    wxString appName = "emma_02";
    
#endif
    
#if defined (__WXMSW__)
    wxString appName = "Emma 02";
    returnValue.operatingSystem = OS_WINDOWS;
    
    RTL_OSVERSIONINFOEXW osVersion;
    GetOsVersion(&osVersion);
    
    switch (osVersion.dwMajorVersion)
    {
        case OS_MAJOR_XP_2000:
            if (osVersion.dwMinorVersion == OS_MINOR_2000)
            {
                windowInfoFile = "win2000_" + fontSizeString + ".ini";
                returnValue.operatingSystem = OS_WINDOWS_2000;
            }
            else
                windowInfoFile = "winxp_" + fontSizeString + ".ini";
        break;
            
        case OS_MAJOR_VISTA_8_1:
            windowInfoFile = "win8_" + fontSizeString + ".ini";
        break;
            
        default:
            windowInfoFile = "win10_" + fontSizeString + ".ini";
        break;
    }
#endif
    
    windowInfoFile = windowInfoFile.MakeLower();
    
    bool fileExists = wxFile::Exists(appDir + windowInfoFile);
    if (!fileExists)
    {
        returnValue.errorMessage = "Configuration file '" + windowInfoFile + "' not found, loading default configuration\n";

        windowInfoFile = "linuxdefault_" + fontSizeString + ".ini";
    }

    wxFileConfig *pConfig = new wxFileConfig(appName, "Marcel van Tongeren", appDir + windowInfoFile);
    
    wxConfigBase *currentConfigPointer = wxConfigBase::Set(pConfig);
    windowConfigPointer = wxConfigBase::Get();
    
    returnValue.xBorder = (int)windowConfigPointer->Read("/Border/x", 1);
    returnValue.yBorder = (int)windowConfigPointer->Read("/Border/y", 1);
    returnValue.xBorder2 = (int)windowConfigPointer->Read("/Border/x2", 1);
    returnValue.yBorder2 = (int)windowConfigPointer->Read("/Border/y2", 24);
    returnValue.xPrint = (int)windowConfigPointer->Read("/Print/x", 19);
    
    returnValue.clockTextCorrectionX = (int)windowConfigPointer->Read("/Correction/clockTextX", 315);
    returnValue.clockTextCorrectionY = (int)windowConfigPointer->Read("/Correction/clockTextY", 121);
    returnValue.clockTextCorrectionSingleTabX = (int)windowConfigPointer->Read("/Correction/clockTextSingleTabX", 316);
    returnValue.clockTextCorrectionSingleTabY = (int)windowConfigPointer->Read("/Correction/clockTextSingleTabY", 97);
    
    returnValue.clockCorrectionX = (int)windowConfigPointer->Read("/Correction/clockX", 279);
    returnValue.clockCorrectionY = (int)windowConfigPointer->Read("/Correction/clockY", 124);
    returnValue.clockCorrectionSingleTabX = (int)windowConfigPointer->Read("/Correction/clockSingleTabX", 280);
    returnValue.clockCorrectionSingleTabY = (int)windowConfigPointer->Read("/Correction/clockSingleTabY", 100);
    
    returnValue.mhzTextCorrectionX = (int)windowConfigPointer->Read("/Correction/mhzTextX", 230);
    returnValue.mhzTextCorrectionY = (int)windowConfigPointer->Read("/Correction/mhzTextY", 121);
    returnValue.mhzTextCorrectionSingleTabX = (int)windowConfigPointer->Read("/Correction/mhzTextSingleTabX", 231);
    returnValue.mhzTextCorrectionSingleTabY = (int)windowConfigPointer->Read("/Correction/mhzTextSingleTabY", 97);
    
    returnValue.stopCorrectionX = (int)windowConfigPointer->Read("/Correction/stopX", 202);
    returnValue.stopCorrectionY = (int)windowConfigPointer->Read("/Correction/stopY", 124);
    returnValue.stopCorrectionSingleTabX = (int)windowConfigPointer->Read("/Correction/stopSingleTabX", 203);
    returnValue.stopCorrectionSingleTabY = (int)windowConfigPointer->Read("/Correction/stopSingleTabY", 100);
    
    returnValue.startCorrectionX = (int)windowConfigPointer->Read("/Correction/startX", 119);
    returnValue.startCorrectionY = (int)windowConfigPointer->Read("/Correction/startY", 124);
    returnValue.startCorrectionSingleTabX = (int)windowConfigPointer->Read("/Correction/startSingleTabX", 120);
    returnValue.startCorrectionSingleTabY = (int)windowConfigPointer->Read("/Correction/startSingleTabY", 100);
    
    returnValue.ledPosY = (int)windowConfigPointer->Read("/Bar/ledPosY", 2);
    returnValue.ledPosX1 = (int)windowConfigPointer->Read("/Bar/ledPosX1", 0l);
    returnValue.ledPosX2 = (int)windowConfigPointer->Read("/Bar/ledPosX2", 19);
    returnValue.ledSpacing = (int)windowConfigPointer->Read("/Bar/ledSpacing", 1);
    returnValue.ledPosDiagY = (int)windowConfigPointer->Read("/Bar/ledPosDiagY", -2);
    returnValue.ledPosVip2Y = (int)windowConfigPointer->Read("/Bar/ledPosVip2Y", -1);
    
    returnValue.statusBarLeader = windowConfigPointer->Read("/Bar/leader", "%d:           X");
    returnValue.statusBarLeader = returnValue.statusBarLeader.Left (returnValue.statusBarLeader.Len()-1);

    returnValue.statusBarLeaderCidelsa = windowConfigPointer->Read("/Bar/leaderCidelsa", "      X");
    returnValue.statusBarLeaderCidelsa = returnValue.statusBarLeaderCidelsa.Mid (1, returnValue.statusBarLeaderCidelsa.Len()-2);

    returnValue.statusBarElementMeasure[0] = (int)windowConfigPointer->Read("/Bar/ElementMeasure0", 40);
    returnValue.statusBarElementMeasure[1] = (int)windowConfigPointer->Read("/Bar/ElementMeasure1", 70);
    returnValue.statusBarElementMeasure[2] = (int)windowConfigPointer->Read("/Bar/ElementMeasure2", 80);
    returnValue.statusBarElementMeasure[3] = (int)windowConfigPointer->Read("/Bar/ElementMeasure3", 100);
    returnValue.statusBarElementMeasure[4] = (int)windowConfigPointer->Read("/Bar/ElementMeasure4", 150);

    returnValue.floatHeight = (int)windowConfigPointer->Read("/Correction/floatHeight", 21);
    returnValue.startHeight = (int)windowConfigPointer->Read("/Correction/startHeight", -1);
    returnValue.clockSize = (int)windowConfigPointer->Read("/Correction/clockSize", 47);
    
    returnValue.red = (int)windowConfigPointer->Read("/Colour/red", 219);
    returnValue.green = (int)windowConfigPointer->Read("/Colour/green", 219);
    returnValue.blue = (int)windowConfigPointer->Read("/Colour/blue", 219);
        
    delete pConfig;
    wxConfigBase::Set(currentConfigPointer);

    return returnValue;
}

wxString GuiMain::readConfigDir(const wxString& key, const wxString& defVal)
{
    wxString returnDir = configPointer->Read(key, defVal);
    
    if (returnDir == "")
        returnDir = dataDir_ + returnDir;
    
    wxDir directory;
    if (!directory.Open(returnDir))
        returnDir = dataDir_ + returnDir;
    
//    if (returnDir.Left(1) != "/" && returnDir.Left(2) != "C:")
//        returnDir = dataDir_ + returnDir;
   
    return returnDir;
}

void GuiMain::writeConfigDir(const wxString& key, const wxString& value)
{
    wxString tempStr = value;
    tempStr.Replace(dataDir_, "");
    configPointer->Write(key, tempStr);
}

void GuiMain::onF4()
{
    if (computerRunning_)
    {
        p_Printer->onF4();
        if (computerConfiguration.printMode_ == PRINTFILE)
        {
            computerConfiguration.printMode_ = PRINTWINDOW;
            if (mode_.gui)
            {
                XRCCTRL(*this, "PrintModeXml", wxChoice)->SetSelection(computerConfiguration.printMode_);
                XRCCTRL(*this, "PrintFileButtonXml", wxButton)->Enable(false);
                XRCCTRL(*this, "PrintFileXml", wxTextCtrl)->Enable(false);
            }
            p_Printer->setPrintMode(computerConfiguration.printMode_);
        }
    }
}

void GuiMain::onPrintMode(wxCommandEvent&event)
{
    computerConfiguration.printMode_ = event.GetSelection();
    setPrintMode();
    
    if (computerRunning_)
        p_Printer->setPrintMode(computerConfiguration.printMode_);
}

void GuiMain::setPrintMode()
{
    if (!mode_.gui)  return;
    if (computerConfiguration.printMode_ == PRINTFILE)
    {
        XRCCTRL(*this, "PrintFileButtonXml", wxButton)->Enable(true);
        XRCCTRL(*this, "PrintFileXml", wxTextCtrl)->Enable(true);
        XRCCTRL(*this, "PrintButtonXml", wxBitmapButton)->Enable(false);
    }
    else
    {
        XRCCTRL(*this, "PrintFileButtonXml", wxButton)->Enable(false);
        XRCCTRL(*this, "PrintFileXml", wxTextCtrl)->Enable(false);
        if (computerRunning_)
            XRCCTRL(*this, "PrintButtonXml", wxBitmapButton)->Enable(true);
        else
            XRCCTRL(*this, "PrintButtonXml", wxBitmapButton)->Enable(false);
    }
}

int GuiMain::getPrintMode()
{
    return computerConfiguration.printMode_;
}

void GuiMain::openPrinterFrame(wxCommandEvent&WXUNUSED(event))
{
#define NO_FORCE_START false
    p_Main->onXmlF4(NO_FORCE_START);
}

void GuiMain::onCharRom(wxCommandEvent& WXUNUSED(event) )
{
    wxString fileName;

    fileName = wxFileSelector( "Select the Character Font file to load",
                               computerConfiguration.characterRomConfiguration.directory, XRCCTRL(*this, "CharRomXml", wxComboBox)->GetValue(),
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
    computerConfiguration.characterRomConfiguration.directory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    computerConfiguration.characterRomConfiguration.fileName = FullPath.GetFullName();

    XRCCTRL(*this, "CharRomXml", wxComboBox)->SetValue(computerConfiguration.characterRomConfiguration.fileName);
}

void GuiMain::onCharRomText(wxCommandEvent& WXUNUSED(event))
{
    computerConfiguration.characterRomConfiguration.fileName = XRCCTRL(*this, "CharRomXml", wxComboBox)->GetValue();
}

void GuiMain::onVT100(wxCommandEvent&event)
{
    int Selection = event.GetSelection();
    setVtType(Selection, true);
}

void GuiMain::setVtType(int Selection, bool GuiChange)
{
    computerConfiguration.videoTerminalConfiguration.type = Selection;

    if (computerConfiguration.videoTerminalConfiguration.external & !GuiChange)
        Selection = EXTERNAL_TERMINAL;
    
    if (computerConfiguration.videoTerminalConfiguration.loop_back & !GuiChange)
        Selection = LOOP_BACK;

    switch(Selection)
    {
        case VTNONE:
            if (mode_.gui)
            {
                XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->Enable(false);
                XRCCTRL(*this, "VTBaudTChoiceXml", wxChoice)->Enable(false);
                XRCCTRL(*this, "VTBaudRTextXml", wxStaticText)->Enable(false);
                XRCCTRL(*this, "VTBaudTTextXml", wxStaticText)->Enable(false);
                XRCCTRL(*this, "VtSetupXml", wxButton)->Enable(false);

                XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->SetValue(computerConfiguration.autoCassetteLoad_);
                XRCCTRL(*this, "TurboXml", wxCheckBox)->SetValue(computerConfiguration.turbo_);

                XRCCTRL(*this, "ZoomTextVtXml", wxStaticText)->Enable(false);
                XRCCTRL(*this, "ZoomSpinVtXml", wxSpinButton)->Enable(false);
                XRCCTRL(*this, "ZoomValueVtXml", wxTextCtrl)->Enable(false);
                XRCCTRL(*this, "StretchDotXml", wxCheckBox)->Enable(false);
            }
            computerConfiguration.videoTerminalConfiguration.external = false;
            computerConfiguration.videoTerminalConfiguration.loop_back = false;
        break;

        case VT52:
            if (mode_.gui)
            {
                XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->Enable(computerConfiguration.videoTerminalConfiguration.uart1854_defined || computerConfiguration.videoTerminalConfiguration.uart16450_defined);
                XRCCTRL(*this, "VTBaudTChoiceXml", wxChoice)->Enable(true);
                XRCCTRL(*this, "VTBaudRTextXml", wxStaticText)->Enable(computerConfiguration.videoTerminalConfiguration.uart1854_defined || computerConfiguration.videoTerminalConfiguration.uart16450_defined);
                XRCCTRL(*this, "VTBaudTTextXml", wxStaticText)->Enable(true);
                XRCCTRL(*this, "ZoomTextVtXml", wxStaticText)->Enable(true);
                XRCCTRL(*this, "VtSetupXml", wxButton)->Enable(true);

                XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->SetValue(computerConfiguration.autoCassetteLoad_);
                XRCCTRL(*this, "TurboXml", wxCheckBox)->SetValue(computerConfiguration.turbo_);
            
                XRCCTRL(*this, "ZoomSpinVtXml", wxSpinButton)->Enable(true);
                XRCCTRL(*this, "ZoomValueVtXml", wxTextCtrl)->Enable(true);
                XRCCTRL(*this, "StretchDotXml", wxCheckBox)->Enable(true);
            }
            computerConfiguration.videoTerminalConfiguration.external = false;
            computerConfiguration.videoTerminalConfiguration.loop_back = false;
        break;

        case VT100:
            if (mode_.gui)
            {
                XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->Enable(computerConfiguration.videoTerminalConfiguration.uart1854_defined || computerConfiguration.videoTerminalConfiguration.uart16450_defined);
                XRCCTRL(*this, "VTBaudTChoiceXml", wxChoice)->Enable(true);
                XRCCTRL(*this, "VTBaudRTextXml", wxStaticText)->Enable(computerConfiguration.videoTerminalConfiguration.uart1854_defined || computerConfiguration.videoTerminalConfiguration.uart16450_defined);
                XRCCTRL(*this, "VTBaudTTextXml", wxStaticText)->Enable(true);
                XRCCTRL(*this, "ZoomTextVtXml", wxStaticText)->Enable(true);
                XRCCTRL(*this, "VtSetupXml", wxButton)->Enable(true);

                XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->SetValue(computerConfiguration.autoCassetteLoad_);
                XRCCTRL(*this, "TurboXml", wxCheckBox)->SetValue(computerConfiguration.turbo_);

                XRCCTRL(*this, "ZoomSpinVtXml", wxSpinButton)->Enable(true);
                XRCCTRL(*this, "ZoomValueVtXml", wxTextCtrl)->Enable(true);
                XRCCTRL(*this, "StretchDotXml", wxCheckBox)->Enable(true);
            }
            computerConfiguration.videoTerminalConfiguration.external = false;
            computerConfiguration.videoTerminalConfiguration.loop_back = false;
        break;
    
        case EXTERNAL_TERMINAL:
            if (mode_.gui)
            {
                XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->Enable(computerConfiguration.videoTerminalConfiguration.uart1854_defined || computerConfiguration.videoTerminalConfiguration.uart16450_defined);
                XRCCTRL(*this, "VTBaudTChoiceXml", wxChoice)->Enable(true);
                XRCCTRL(*this, "VTBaudRTextXml", wxStaticText)->Enable(computerConfiguration.videoTerminalConfiguration.uart1854_defined || computerConfiguration.videoTerminalConfiguration.uart16450_defined);
                XRCCTRL(*this, "VTBaudTTextXml", wxStaticText)->Enable(true);
                XRCCTRL(*this, "VtSetupXml", wxButton)->Enable(true);

                XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->SetValue(false);
                XRCCTRL(*this, "TurboXml", wxCheckBox)->SetValue(false);
            }
            computerConfiguration.videoTerminalConfiguration.external = true;
            computerConfiguration.videoTerminalConfiguration.loop_back = false;
            computerConfiguration.videoTerminalConfiguration.type = VTNONE;
        break;

        case LOOP_BACK:
            if (mode_.gui)
            {
                XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->Enable(computerConfiguration.videoTerminalConfiguration.uart1854_defined || computerConfiguration.videoTerminalConfiguration.uart16450_defined);
                XRCCTRL(*this, "VTBaudTChoiceXml", wxChoice)->Enable(true);
                XRCCTRL(*this, "VTBaudRTextXml", wxStaticText)->Enable(computerConfiguration.videoTerminalConfiguration.uart1854_defined || computerConfiguration.videoTerminalConfiguration.uart16450_defined);
                XRCCTRL(*this, "VTBaudTTextXml", wxStaticText)->Enable(true);
                XRCCTRL(*this, "VtSetupXml", wxButton)->Enable(true);

                XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->SetValue(false);
                XRCCTRL(*this, "TurboXml", wxCheckBox)->SetValue(false);
            }
            computerConfiguration.videoTerminalConfiguration.external = false;
            computerConfiguration.videoTerminalConfiguration.loop_back = true;
            computerConfiguration.videoTerminalConfiguration.type = VTNONE;
        break;
    }
}

void GuiMain::onFullScreenFloat(wxCommandEvent&WXUNUSED(event))
{
    fullScreenFloat_ = !fullScreenFloat_;
    correctZoomAndValue(SET_SPIN, VIDEOMAIN);
    correctZoomVtAndValue(SET_SPIN);
}

void GuiMain::correctZoomAndValue(bool setSpin, int videoNumber)
{
    correctZoom(setSpin, videoNumber);
    if (videoNumber == computerConfiguration.videoNumber_)
        XRCCTRL(*this, "ZoomValueXml", wxTextCtrl)->ChangeValue(computerConfiguration.zoom_[videoNumber]);
}

void GuiMain::correctZoom(bool setSpin, int videoNumber)
{
    double zoom;
    int zoomInt;

    p_Main->toDouble(computerConfiguration.zoom_[videoNumber], &zoom);
    if (!fullScreenFloat_)
    {
        zoomInt = (int) (zoom + 0.5);
        if (zoomInt == 0)
            zoomInt++;
        if (setSpin)
            XRCCTRL(*this, "ZoomSpinXml", wxSpinButton)->SetValue(zoomInt);
#if defined(__WXMSW__)
        XRCCTRL(*this, "ZoomSpinXml", wxSpinButton)->SetRange(2,9);
#else
        XRCCTRL(*this, "ZoomSpinXml", wxSpinButton)->SetRange(1,10);
#endif
        computerConfiguration.zoom_[videoNumber].Printf("%2.2f", (double)zoomInt);
    }
    else
    {
        zoomInt = (int)(zoom*10+0.4);
        if (setSpin)
            XRCCTRL(*this, "ZoomSpinXml", wxSpinButton)->SetValue(zoomInt);
#if defined(__WXMSW__)
        XRCCTRL(*this, "ZoomSpinXml", wxSpinButton)->SetRange(6,99);
#else
        XRCCTRL(*this, "ZoomSpinXml", wxSpinButton)->SetRange(5,100);
#endif
        computerConfiguration.zoom_[videoNumber].Printf("%2.2f", zoom);
    }
    if (computerRunning_)
        p_Main->eventZoomChange(zoom, videoNumber);
    else
    {
        zoomEventOngoing_ = false;
        fullScreenEventOngoing_ = false;
    }
}

void GuiMain::onZoom(wxSpinEvent&event)
{
    if (zoomEventOngoing_ && computerRunning_)
        return;

    if (computerRunning_)
        zoomEventOngoing_ = true;

    int position = event.GetPosition();
    double zoom;
    
    if (!fullScreenFloat_)
        zoom = position;
    else
        zoom = (double)position/10;
    
    computerConfiguration.zoom_[computerConfiguration.videoNumber_].Printf("%2.2f", zoom);
    correctZoomAndValue(DO_NOT_SET_SPIN, computerConfiguration.videoNumber_);
}

void GuiMain::onZoomValue(wxCommandEvent&event)
{
    if (zoomEventOngoing_ && computerRunning_)
        return;

    if (computerRunning_)
        zoomEventOngoing_ = true;

    wxString zoomString = event.GetString();
    double zoom;
    if (toDouble(zoomString, &zoom))
    {
        if (!fullScreenFloat_)
            zoom = (int) (zoom);
        computerConfiguration.zoom_[computerConfiguration.videoNumber_] = zoomString;
        correctZoom(SET_SPIN, computerConfiguration.videoNumber_);
    }
    else
    {
        zoomEventOngoing_ = false;
        fullScreenEventOngoing_ = false;
        if (zoomString != "")
        {
            (void)wxMessageBox( "Please specify a number\n",
                                "Emma 02", wxICON_ERROR | wxOK );
        }
    }
}

void GuiMain::correctZoomVtAndValue(bool setSpin)
{
    correctZoomVt(setSpin);
    XRCCTRL(*this, "ZoomValueVtXml", wxTextCtrl)->ChangeValue(computerConfiguration.videoTerminalConfiguration.zoom);
}

void GuiMain::correctZoomVt(bool setSpin)
{
    double zoom;
    int zoomInt;

    toDouble(computerConfiguration.videoTerminalConfiguration.zoom, &zoom);
    if (!fullScreenFloat_)
    {
        zoomInt = (int) (zoom + 0.5);
        if (zoomInt == 0)
            zoomInt++;
        if (setSpin)
            XRCCTRL(*this, "ZoomSpinVtXml", wxSpinButton)->SetValue(zoomInt);
#if defined(__WXMSW__)
        XRCCTRL(*this, "ZoomSpinVtXml", wxSpinButton)->SetRange(2,9);
#else
        XRCCTRL(*this, "ZoomSpinVtXml", wxSpinButton)->SetRange(1,10);
#endif
        computerConfiguration.videoTerminalConfiguration.zoom.Printf("%2.2f", (double)zoomInt);
    }
    else
    {
        zoomInt = (int)(zoom*10+0.4);
        if (setSpin)
            XRCCTRL(*this, "ZoomSpinVtXml", wxSpinButton)->SetValue(zoomInt);
#if defined(__WXMSW__)
        XRCCTRL(*this, "ZoomSpinVtXml", wxSpinButton)->SetRange(6,99);
#else
        XRCCTRL(*this, "ZoomSpinVtXml", wxSpinButton)->SetRange(5,100);
#endif
        computerConfiguration.videoTerminalConfiguration.zoom.Printf("%2.2f", zoom);
    }
    if (computerRunning_ && p_Vt100[UART1] != NULL)
        p_Main->eventZoomVtChange(zoom, UART1);
    else
    {
        if (computerRunning_ && p_Vt100[UART2] != NULL)
            p_Main->eventZoomVtChange(zoom, UART2);
        else
        {
            zoomEventOngoing_ = false;
            fullScreenEventOngoing_ = false;
        }
    }
}

void GuiMain::onZoomVt(wxSpinEvent&event)
{
    if (zoomEventOngoing_ && computerRunning_)
        return;

    if (computerRunning_)
        zoomEventOngoing_ = true;

    int position = event.GetPosition();
    double zoomVt;

    if (!fullScreenFloat_)
        zoomVt = position;
    else
        zoomVt = (double)position/10;
    
    computerConfiguration.videoTerminalConfiguration.zoom.Printf("%2.2f", zoomVt);
    correctZoomVtAndValue(DO_NOT_SET_SPIN);
}

void GuiMain::onZoomValueVt(wxCommandEvent&event)
{
    if (zoomEventOngoing_ && computerRunning_)
        return;

    if (computerRunning_)
        zoomEventOngoing_ = true;

    wxString zoomString = event.GetString();
    double zoomVt;
    if (toDouble(zoomString, &zoomVt))
    {
        if (!fullScreenFloat_)
            zoomVt = (int) (zoomVt);

        computerConfiguration.videoTerminalConfiguration.zoom = zoomString;
        correctZoomVt(SET_SPIN);
    }
    else
    {
        zoomEventOngoing_ = false;
        fullScreenEventOngoing_ = false;
        if (zoomString != "")
        {
            (void)wxMessageBox( "Please specify a number\n",
                                "Emma 02", wxICON_ERROR | wxOK );
        }
    }
}

void GuiMain::onFullScreen(wxCommandEvent&WXUNUSED(event))
{
    if (computerRunning_ && p_Vt100[UART1] != NULL)
        p_Vt100[UART1]->onF3();
    if (computerRunning_ && p_Vt100[UART2] != NULL)
        p_Vt100[UART2]->onF3();
    else if (computerRunning_ && p_Video[computerConfiguration.videoNumber_] != NULL)
        p_Video[computerConfiguration.videoNumber_]->onF3();
}

void GuiMain::onInterlace(wxCommandEvent&event)
{
    computerConfiguration.interlace_ = event.IsChecked();
    if (computerRunning_)
    {
        for (int video=0; video<computerConfiguration.numberOfVideoTypes_; video++)
            p_Video[video]->setInterlace(event.IsChecked());
    }
}

void GuiMain::onStretchDot(wxCommandEvent&event)
{
    computerConfiguration.videoTerminalConfiguration.stretchDot = event.IsChecked();
    if (computerRunning_ && p_Vt100[UART1] != NULL)
        p_Vt100[UART1]->setStretchDot(event.IsChecked());
    if (computerRunning_ && p_Vt100[UART2] != NULL)
        p_Vt100[UART2]->setStretchDot(event.IsChecked());
}

void GuiMain::onScreenDump(wxCommandEvent&WXUNUSED(event))
{
    if (computerRunning_)
    {
        for (int video=0; video<computerConfiguration.numberOfVideoTypes_; video++)
            p_Video[video]->onF5();
        if (p_Vt100[UART1] != NULL)
        {
            if (computerConfiguration.videoTerminalConfiguration.show)
                p_Vt100[UART1]->onF5();
        }
        if (p_Vt100[UART2] != NULL)
            p_Vt100[UART2]->onF5();
    }
}

void GuiMain::onVolume(wxScrollEvent&event)
{
    computerConfiguration.soundConfiguration.volume = event.GetPosition();
    if (computerRunning_)
        p_Computer->setVolume(computerConfiguration.soundConfiguration.volume);
}

void GuiMain::onAutoLoad(wxCommandEvent&event)
{
    computerConfiguration.autoCassetteLoad_ = event.IsChecked();
    if (computerRunning_)
    {
        XRCCTRL(*this, "CasLoadXml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
        XRCCTRL(*this, "CasSaveXml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_ && !computerConfiguration.videoTerminalConfiguration.hexModem_defined);
        if (computerConfiguration.swTapeConfiguration.twoDecks || computerConfiguration.hwTapeConfiguration.twoDecks)
        {
            XRCCTRL(*this, "CasLoad1Xml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
            XRCCTRL(*this, "CasSave1Xml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
        }
    }
}

void GuiMain::onTurbo(wxCommandEvent&event)
{
    if (computerRunning_ && turboOn_)
        turboOff();
    
    computerConfiguration.turbo_ = event.IsChecked();
    turboGui();
}

void GuiMain::onTurboClock(wxCommandEvent&WXUNUSED(event))
{
    double clk;

    computerConfiguration.turboClock_ =  XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->GetValue();
    if (computerConfiguration.turboClock_ == "")
    {
        computerConfiguration.turboClock_ = "15";
        return;
    }
    if (!toDouble(computerConfiguration.turboClock_, (double*)&clk))
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
    computerConfiguration.memAccessConfiguration.useLocation = event.IsChecked();
    enableLocationGui();
}

void GuiMain::onCassetteLoad(wxCommandEvent& WXUNUSED(event))
{
    if (computerConfiguration.videoTerminalConfiguration.xModem_defined)
    {
        startTerminalLoad(TERM_XMODEM_LOAD);
        return;
    }
    if (computerConfiguration.videoTerminalConfiguration.hexModem_defined)
    {
        startTerminalLoad(TERM_HEX);
        return;
    }
    
    if (isTapeHwCybervision())
    {
        if (hwTapeState_ == HW_TAPE_STATE_REC)
            p_Computer->pauseTape();

        if (hwTapeState_ == HW_TAPE_STATE_PLAY) // && p_Computer->getFlipFlopQ() != 1)
            hwTapeState_ = HW_TAPE_STATE_OFF;
        else
            hwTapeState_ = HW_TAPE_STATE_PLAY;

        if (hwTapeState_ == HW_TAPE_STATE_PLAY)
        {
            XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playDarkGreenBitmap);
            XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
            XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
            XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
            if (p_Computer->getFlipFlopQ() == 1)
                p_Computer->startLoad(0, true);
        }
        else
        {
            XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
            if (p_Computer->getFlipFlopQ() == 1)
                p_Computer->pauseTape();
        }
        
    }
    else
        p_Computer->startLoad(0, true);
}

void GuiMain::onCassetteForward(wxCommandEvent& WXUNUSED(event))
{
    if (hwTapeState_ == HW_TAPE_STATE_REC)
        p_Computer->pauseTape();

    if (hwTapeState_ == HW_TAPE_STATE_FF)
        hwTapeState_ = HW_TAPE_STATE_OFF;
    else
        hwTapeState_ = HW_TAPE_STATE_FF;

    if (hwTapeState_ == HW_TAPE_STATE_FF)
    {
        XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardDarkGreenBitmap);
        XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
        XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
        XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
        if (p_Computer->getFlipFlopQ() == 1)
        {
            p_Computer->startLoad(0, true);
            p_Computer->forwardTape(TAPE_FF);
        }
    }
    else
    {
        XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
        if (p_Computer->getFlipFlopQ() == 1)
            p_Computer->forwardTape(TAPE_STOP);
    }
}

void GuiMain::onCassetteRewind(wxCommandEvent& WXUNUSED(event))
{
    if (hwTapeState_ == HW_TAPE_STATE_REC)
        p_Computer->pauseTape();

    if (hwTapeState_ == HW_TAPE_STATE_RW)
        hwTapeState_ = HW_TAPE_STATE_OFF;
    else
        hwTapeState_ = HW_TAPE_STATE_RW;

    if (hwTapeState_ == HW_TAPE_STATE_RW)
    {
        XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
        XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindDarkGreenBitmap);
        XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
        XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
        if (p_Computer->getFlipFlopQ() == 1)
        {
            p_Computer->startLoad(0, true);
            p_Computer->rewindTape(TAPE_RW);
        }
    }
    else
    {
        XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
        if (p_Computer->getFlipFlopQ() == 1)
            p_Computer->rewindTape(TAPE_STOP);
    }
}

void GuiMain::onCassetteLoad1(wxCommandEvent& WXUNUSED(event))
{
    p_Computer->startLoad(1, true);
}

void GuiMain::onCassetteSave(wxCommandEvent& WXUNUSED(event))
{
    if (computerConfiguration.videoTerminalConfiguration.xModem_defined)
    {
        p_Main->startTerminalSave(TERM_XMODEM_SAVE);
        return;
    }
    
    if (isTapeHwCybervision())
    {
        if (hwTapeState_ == HW_TAPE_STATE_REC)
        {
            hwTapeState_ = HW_TAPE_STATE_OFF;
            p_Computer->pauseTape();
        }
        else
            hwTapeState_ = HW_TAPE_STATE_REC;

        if (hwTapeState_ == HW_TAPE_STATE_REC)
        {
            XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
            XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
            XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
            XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recPressedBitmap);
            if (p_Computer->getFlipFlopQ() == 1)
                p_Computer->startRecording(0);
        }
        else
        {
            XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
            if (p_Computer->getFlipFlopQ() == 1)
                p_Computer->pauseTape();
        }
    }
    else
        startSaveNew(0);
}

void GuiMain::onCassetteSave1(wxCommandEvent& WXUNUSED(event))
{
    startSaveNew(1);
}

void GuiMain::onCassetteStop(wxCommandEvent& WXUNUSED(event))
{
    if (computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined)
    {
        stopTerminal();
        p_Computer->terminalStop();
        return;
    }
    p_Computer->stopSaveLoad();
}

void GuiMain::onCassettePause(wxCommandEvent& WXUNUSED(event))
{
    p_Computer->pauseTape();
}

void GuiMain::startHwSave()
{
    if (hwTapeState_ == HW_TAPE_STATE_REC)
        return;
    
    hwTapeState_ = HW_TAPE_STATE_REC;
    startSaveNew(0);
    p_Main->eventHwTapeStateChange(HW_TAPE_STATE_REC);
}

void GuiMain::startHwLoad()
{
    if (hwTapeState_ == HW_TAPE_STATE_PLAY)
        return;
    
    if (p_Computer->getFlipFlopQ() == 1)
        p_Computer->startLoad(0, true);

    p_Main->eventHwTapeStateChange(HW_TAPE_STATE_PLAY);
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
            if (computerConfiguration.memAccessConfiguration.useLocation)
            {
                buffer [0] = 1;
                buffer [1] = computerConfiguration.basicConfiguration.pLoadSaveName[0];
                buffer [2] = computerConfiguration.basicConfiguration.pLoadSaveName[1];
                buffer [3] = computerConfiguration.basicConfiguration.pLoadSaveName[2];
                buffer [4] = computerConfiguration.basicConfiguration.pLoadSaveName[3];
                buffer [5] = (computerConfiguration.memAccessConfiguration.saveStart >> 8) &0xff;
                buffer [6] = computerConfiguration.memAccessConfiguration.saveStart&0xff;
                buffer [7] = (computerConfiguration.memAccessConfiguration.saveEnd >> 8) &0xff;
                buffer [8] = computerConfiguration.memAccessConfiguration.saveEnd&0xff;
                buffer [9] = (computerConfiguration.memAccessConfiguration.saveExec >> 8) &0xff;
                buffer [10] = computerConfiguration.memAccessConfiguration.saveExec&0xff;
                address = computerConfiguration.memAccessConfiguration.saveStart;
                length = computerConfiguration.memAccessConfiguration.saveEnd - computerConfiguration.memAccessConfiguration.saveStart + 12;
                start = 11;
            }
            else
            {
                XRCCTRL(*this,"SaveStartXml",wxTextCtrl)->SetValue("");
                XRCCTRL(*this,"SaveEndXml",wxTextCtrl)->SetValue("");
                XRCCTRL(*this,"SaveExecXml",wxTextCtrl)->SetValue("");

                address = computerConfiguration.basicConfiguration.ramAddress.value;
                highRamAddress = (computerConfiguration.basicConfiguration.ramAddress.value & 0xff00) >> 8;
                buffer [0] = 6;
                length = (p_Computer->getMainMemory(computerConfiguration.basicConfiguration.eop.value) - highRamAddress) << 8;

                if (p_Computer->isFAndMBasicRunning())
                {
                    address = 0x6700;
                    buffer [0] = 3;
                    highRamAddress = 0;
                    length = (p_Computer->getMainMemory(computerConfiguration.basicConfiguration.eop.value) - 0x67) << 8;
                }
                length += p_Computer->getMainMemory(computerConfiguration.basicConfiguration.eop.value+1);
                buffer [1] = computerConfiguration.basicConfiguration.pLoadSaveName[0];
                buffer [2] = computerConfiguration.basicConfiguration.pLoadSaveName[1];
                buffer [3] = computerConfiguration.basicConfiguration.pLoadSaveName[2];
                buffer [4] = computerConfiguration.basicConfiguration.pLoadSaveName[3];
                buffer [5] = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.defus.value)-highRamAddress;
                buffer [6] = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.defus.value+1);
                buffer [7] = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.eop.value)-highRamAddress;
                buffer [8] = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.eop.value+1);
                buffer [9] = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.string.value)-highRamAddress;
                buffer [10] = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.string.value+1);
                buffer [11] = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.array.value)-highRamAddress;
                buffer [12] = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.array.value+1);
                buffer [13] = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.eod.value)-highRamAddress;
                buffer [14] = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.eod.value+1);
                start = 15;
                length += start;
            }
//            startAddress = address;
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
    int address, start, arrayStart, stringStart, dataEnd, eop;
    char buffer[65536];
    wxString fileName, stringAdress;

    fileName = wxFileSelector( "Select the "+computerInfo.name+" Data file to save",
                               computerConfiguration.memAccessConfiguration.directory, computerConfiguration.memAccessConfiguration.fileName,
                               "",
                               wxString::Format
                              (
                                   computerInfo.name+" Data File (*."+computerInfo.ploadExtension+")|*."+computerInfo.ploadExtension+"|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_OVERWRITE_PROMPT,
                               this
                              );
    if (!fileName)
        return;

    wxFileName swFullPath = wxFileName(fileName, wxPATH_NATIVE);
    computerConfiguration.memAccessConfiguration.directory = swFullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

    if (!fileName.empty())
    {
        if (outputFile.Create(fileName, true))
        {
            eop = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.eop.value) << 8;
            eop += p_Computer->getMainMemory(computerConfiguration.basicConfiguration.eop.value+1);
            arrayStart = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.array.value) << 8;
            arrayStart += p_Computer->getMainMemory(computerConfiguration.basicConfiguration.array.value+1);
            stringStart = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.string.value) << 8;
            stringStart += p_Computer->getMainMemory(computerConfiguration.basicConfiguration.string.value+1);
            dataEnd = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.eod.value) << 8;
            dataEnd += p_Computer->getMainMemory(computerConfiguration.basicConfiguration.eod.value+1);
            dataLength = dataEnd-arrayStart;
            arrayLength = stringStart-arrayStart;
            buffer [0] = 5;
            buffer [1] = computerConfiguration.basicConfiguration.pLoadSaveName[0];
            buffer [2] = computerConfiguration.basicConfiguration.pLoadSaveName[1];
            buffer [3] = (dataLength >> 8) &0xff;
            buffer [4] = dataLength &0xff;
            buffer [5] = (arrayLength >> 8) &0xff;
            buffer [6] = arrayLength &0xff;
            address = arrayStart;
            start = 7;
            dataLength += start;
            dataLength &= 0xffff;
            address &= 0xffff;

            for (size_t i=start; i<dataLength; i++)
            {
                buffer[i] = p_Computer->getMainMemory(address);
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
    if (selectedTab_ > XMLTAB)
        return;
    
    computerConfiguration.memAccessConfiguration.saveStartString = event.GetString();

    long value;

    if (computerConfiguration.memAccessConfiguration.saveStartString.ToLong(&value, 16))
        computerConfiguration.memAccessConfiguration.saveStart = value;
    else
        computerConfiguration.memAccessConfiguration.saveStart = 0;
}

void GuiMain::onSaveEnd(wxCommandEvent& event)
{
    if (selectedTab_ > XMLTAB)
        return;
    
    computerConfiguration.memAccessConfiguration.saveEndString = event.GetString();

    long value;

    if (computerConfiguration.memAccessConfiguration.saveEndString.ToLong(&value, 16))
        computerConfiguration.memAccessConfiguration.saveEnd = value;
    else
        computerConfiguration.memAccessConfiguration.saveEnd = 0;
}

void GuiMain::onSaveExec(wxCommandEvent& event)
{
    if (selectedTab_ > XMLTAB)
        return;
    
    computerConfiguration.memAccessConfiguration.saveExecString = event.GetString();

    long value;

    if (computerConfiguration.memAccessConfiguration.saveExecString.ToLong(&value, 16))
        computerConfiguration.memAccessConfiguration.saveExec = value;
    else
        computerConfiguration.memAccessConfiguration.saveExec = 0;
}

void GuiMain::getSoftware(wxString computer, wxString type, wxString software)
{
    wxFileName FullPath = wxFileName(software, wxPATH_NATIVE);
    if (FullPath.IsRelative())
    {
        configPointer->Write(computer + "/"+type, software);
    }
    else
    {
        configPointer->Write("/Dir/"+computer+"/"+type, FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE));
        configPointer->Write(computer + "/"+type, FullPath.GetFullName());
    }
}

void GuiMain::runSoftware(bool load)
{
    wxString fileName, fileNameFull, extension;
    
    fileName = configPointer->Read("/Xml/Software_File0", "");
    extension = fileName.Right(fileName.Len()-fileName.Find('.', false)-1);
    
    if (extension == computerInfo.ploadExtension && fileName != "")
    {
        if (wxFile::Exists(fileName))
            computerConfiguration.memAccessConfiguration.fullFileName = fileName;
        else
            computerConfiguration.memAccessConfiguration.fullFileName = computerConfiguration.memAccessConfiguration.directory + fileName;
        
        if (!wxFile::Exists(computerConfiguration.memAccessConfiguration.fullFileName))
            return;
        
        computerConfiguration.memAccessConfiguration.fileName = fileName;

        p_Main->setSwName(computerConfiguration.memAccessConfiguration.fileName);
        p_Main->updateTitle();
        p_Computer->startComputerRun(load);
    }
    else
    {
        for (size_t config=0; config<2; config++)
        {
            if (computerConfiguration.memoryConfiguration[config].cartType == CART_CH8)
            {
                fileName = configPointer->Read("/Xml/Chip_8_Software", "");
                if (wxFile::Exists(fileName))
                    fileNameFull = fileName;
                else
                    fileNameFull = computerConfiguration.memoryConfiguration[config].dirname + fileName;
                
                if (wxFile::Exists(fileNameFull))
                    p_Computer->readFile(fileNameFull, NOCHANGE, computerConfiguration.memoryConfiguration[config].start, 0x10000,  computerConfiguration.memoryConfiguration[config].loadOffSet, NONAME);
            }

            if (computerConfiguration.memoryConfiguration[config].cartType == CART_ST2)
            {
                fileName = configPointer->Read("/Xml/St2_File", "");
                if (wxFile::Exists(fileName))
                    fileNameFull = fileName;
                else
                    fileNameFull = computerConfiguration.memoryConfiguration[config].dirname + fileName;
                
                if (wxFile::Exists(fileNameFull))
                    p_Computer->readSt2Program(fileNameFull, NOCHANGE);
            }
            if (computerConfiguration.memoryConfiguration[config].cartType == CART_BIN)
            {
                if (config == 1)
                    fileName = configPointer->Read("/Xml/Software_File1", "");

                if (wxFile::Exists(fileName))
                    fileNameFull = fileName;
                else
                    fileNameFull = computerConfiguration.memoryConfiguration[config].dirname + fileName;
                
                if (wxFile::Exists(fileNameFull))
                    p_Computer->readFile(fileNameFull, NOCHANGE, computerConfiguration.memoryConfiguration[config].start, 0x10000, SHOWNAME);
            }
        }
    }
}

void GuiMain::onLoad(bool load)
{
    wxFFile inputFile;
    wxString fileName;
    wxString extension;

    if (p_Computer->isComputerRunning())
    {
        int answer = wxMessageBox(    computerInfo.name+" Emulator is running software\n"
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

    if (computerInfo.ploadExtension != "")
        extension = computerInfo.name+" Program File|*."+computerInfo.ploadExtension+"|Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10;*.sc8;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10;*.sc8|Intel Hex File|*.hex|All files (%s)|%s";
    else
        extension = "Binary & Hex|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10;*.sc8;*.hex|Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10;*.sc8|Intel Hex File|*.hex|All files (%s)|%s";

    fileName = wxFileSelector( "Select the "+computerInfo.name+" Program file to load/run",
                               computerConfiguration.memAccessConfiguration.directory, computerConfiguration.memAccessConfiguration.fileName,
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
    computerConfiguration.memAccessConfiguration.directory = swFullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

    computerConfiguration.memAccessConfiguration.fullFileName = fileName;
    computerConfiguration.memAccessConfiguration.fileName = swFullPath.GetFullName();
    p_Main->setSwName(swFullPath.GetName());
    p_Main->updateTitle();

#define SHOW_ADDRESS_POPUP_WINDOW true
    extension = swFullPath.GetExt();
    extension.Trim();
    if (extension == "tmc600" || extension == "comx" || extension == "pecom" || extension == "rca" || extension == "tiny" || extension == "super" || extension == "fpb")
        p_Computer->startComputerRun(load);
    else
        p_Computer->readFile(fileName, NOCHANGE, 0, 0x10000, SHOWNAME, SHOW_ADDRESS_POPUP_WINDOW, computerConfiguration.memAccessConfiguration.saveStart);
}

void GuiMain::onSaveButton(wxCommandEvent& WXUNUSED(event))
{
    wxFile outputFile;
    wxString fileName, stringAdress;
    wxString extension;

    if (computerInfo.ploadExtension != "")
    {
        extension = computerInfo.name+" Program File (*."+computerInfo.ploadExtension+")|*."+computerInfo.ploadExtension+"|Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10;*.sc8|Intel Hex File|*.hex|All files (%s)|%s";
    }
    else
    {
        extension = "Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10;*.sc8|Intel Hex File|*.hex|All files (%s)|%s";
    }

    fileName = wxFileSelector( "Select the "+computerInfo.name+" Program file to save",
                               computerConfiguration.memAccessConfiguration.directory, computerConfiguration.memAccessConfiguration.fileName,
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
    computerConfiguration.memAccessConfiguration.directory = swFullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

    computerConfiguration.memAccessConfiguration.fullFileName = fileName;
    computerConfiguration.memAccessConfiguration.fileName = swFullPath.GetFullName();
    p_Main->setSwName(swFullPath.GetName());
    p_Main->updateTitle();

    extension = swFullPath.GetExt();
    if (extension == computerInfo.ploadExtension && computerInfo.ploadExtension != "")
    { 
        onPsave(fileName);
        return;
    }

/*    stringAdress = XRCCTRL(*this, "SaveStartXml",wxTextCtrl)->GetValue();
    if (!stringAdress.ToLong(&saveStart, 16))
    {
        (void)wxMessageBox( "Please specify start address in hexadecimal\n",
                            "Emma 02", wxICON_ERROR | wxOK );
        return;
    }
    stringAdress = XRCCTRL(*this,"SaveEndXml",wxTextCtrl)->GetValue();
    if (!stringAdress.ToLong(&saveEnd, 16))
    {
        (void)wxMessageBox( "Please specify end address in hexadecimal\n",
                            "Emma 02", wxICON_ERROR | wxOK );
        return;
    }*/
    if (computerConfiguration.memAccessConfiguration.saveStart > computerConfiguration.memAccessConfiguration.saveEnd)
    {
        (void)wxMessageBox( "Please specify start < end\n",
                            "Emma 02", wxICON_ERROR | wxOK );
        return;
    }
    if ((extension == "bin") || (extension == "rom") || (extension == "ram") || (extension == "cos") || (extension == "c8") || (extension == "ch8") || (extension == "c8x") || (extension == "ch10"))
        p_Computer->saveBinFile(fileName, computerConfiguration.memAccessConfiguration.saveStart, computerConfiguration.memAccessConfiguration.saveEnd);
    else
        p_Computer->saveIntelFile(fileName, computerConfiguration.memAccessConfiguration.saveStart, computerConfiguration.memAccessConfiguration.saveEnd);
}

void GuiMain::onClock(wxCommandEvent&WXUNUSED(event))
{
    double clk;

    wxString clock =  clockTextCtrl->GetValue();
    if (!toDouble(clock, (double*)&clk))
    {
        if (clock != "")
        {
            (void)wxMessageBox( "Please specify frequency in MHz\n",
                            "Emma 02", wxICON_ERROR | wxOK );
        }
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
    computerConfiguration.clock_ = clock;
    setClock();
}

void GuiMain::onBaudR(wxCommandEvent&event)
{
    computerConfiguration.videoTerminalConfiguration.baudR = event.GetSelection();
}

void GuiMain::onBaudT(wxCommandEvent&event)
{
    computerConfiguration.videoTerminalConfiguration.baudT = event.GetSelection();
    if (!computerConfiguration.videoTerminalConfiguration.uart1854_defined && !computerConfiguration.videoTerminalConfiguration.uart16450_defined)
    {
        computerConfiguration.videoTerminalConfiguration.baudR = event.GetSelection();
        XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->SetSelection(computerConfiguration.videoTerminalConfiguration.baudR);
    }
}

void GuiMain::resetClearRamState()
{
    p_Main->eventSetCheckBox("XmlClearRam", false);
    computerConfiguration.clearRam = false;
}

void GuiMain::resetClearRtcState()
{
    p_Main->eventSetCheckBox("XmlClearRtc", false);
    computerConfiguration.clearRtc = false;
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
    
    setVtType(computerConfiguration.videoTerminalConfiguration.type, false);
}

wxString GuiMain::convertLocale(wxString returnString)
{
    double value;
    
    int dotPosition = returnString.Find(".");
    int precision;
    
    if (dotPosition == wxNOT_FOUND)
        dotPosition = returnString.Find(",");
    if (dotPosition == wxNOT_FOUND)
        dotPosition = returnString.Find("'");
    
    if (dotPosition == wxNOT_FOUND)
        precision = 0;
    else
        precision = (int)(returnString.Len() - dotPosition);
    
    if (toDouble(returnString, &value))
        returnString.FromDouble(value, precision);

    return returnString;
}

bool GuiMain::toDouble(wxString stringName, double* result)
{
    stringName.Replace(",",".");
    stringName.Replace("'",".");

    bool returnValue = stringName.ToCDouble(result);

    return returnValue;
}

wxString GuiMain::getKeyFile()
{
    return computerConfiguration.keyFileConfiguration.fileName;
}

wxString GuiMain::getKeyFileDir()
{
    return computerConfiguration.keyFileConfiguration.directory;
}

wxString GuiMain::getScreenDumpFile()
{
    wxString screenDumpFile = computerConfiguration.screenDumpFileConfiguration.fileName;
    if (screenDumpFile.Len() != 0)
        screenDumpFile = computerConfiguration.screenDumpFileConfiguration.directory + screenDumpFile;

    return screenDumpFile;
}

wxString GuiMain::getPrintFile()
{
    wxString fileName = computerConfiguration.printerFileConfiguration.fileName;

    if (fileName.Len() != 0)
        fileName = computerConfiguration.printerFileConfiguration.directory + fileName;

    return fileName;
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

void GuiMain::setClock()
{
    double clk;

    wxString clock =  computerConfiguration.clock_;
    if (!toDouble(clock, (double*)&clk))
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
    computerConfiguration.clockSpeed_ = clk;
    computerConfiguration.fdcConfiguration.cyclesPerMilliSecond = clk * 100;
    computerConfiguration.upd765Configuration.cyclesPerMilliSecond = clk * 100;

    if (computerRunning_)
    {
        setClockRate();
        for (int video=0; video<computerConfiguration.numberOfVideoTypes_; video++)
            p_Video[video]->setClock(computerConfiguration.clockSpeed_);
        if (p_Vt100[UART1] != NULL)
            p_Vt100[UART1]->setClock(computerConfiguration.clockSpeed_);
        if (p_Vt100[UART2] != NULL)
            p_Vt100[UART2]->setClock(computerConfiguration.clockSpeed_);
    }
}

void GuiMain::setClockRate()
{
    p_Computer->setClockRate(computerConfiguration.clockSpeed_);
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
}

void GuiMain::setTextCtrl(wxString info, wxString value)
{
    if (mode_.gui)
        XRCCTRL(*this, info, wxTextCtrl)->SetValue(value);
}

void GuiMain::onHexKeyDef(wxCommandEvent&WXUNUSED(event))
{
    KeyMapDialog KeyMapDialog(this);
    KeyMapDialog.ShowModal();

// TO BE CHECKED - when/if re-introducing keymap dialog
//    if (computerRunning_)
//        p_Computer->reDefineKeys();
}

double GuiMain::getZoom(int videoNumber)
{
    double zoom;
    toDouble(computerConfiguration.zoom_[videoNumber], &zoom);

    if (!fullScreenFloat_)
        zoom = (int) zoom;
    return zoom;
}

double GuiMain::getZoomVt()
{
    double zoomVt;
    toDouble(computerConfiguration.videoTerminalConfiguration.zoom, &zoomVt);

    if (!fullScreenFloat_)
        zoomVt = (int) zoomVt;
    return zoomVt;
}

wxPoint GuiMain::getCoinPos()
{
    return wxPoint(computerConfiguration.coinConfiguration.x, computerConfiguration.coinConfiguration.y);
}

void GuiMain::setCoinPos(wxPoint position)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.coinConfiguration.x = -1;
        computerConfiguration.coinConfiguration.y = -1;
    }
    else
    {
        if (position.x > 0)
            computerConfiguration.coinConfiguration.x = position.x;
        if (position.y > 0)
            computerConfiguration.coinConfiguration.y = position.y;
    }
}

wxPoint GuiMain::getPixiePos()
{
    return wxPoint(computerConfiguration.cdp1861Configuration.x, computerConfiguration.cdp1861Configuration.y);
}

void GuiMain::setPixiePos(wxPoint position)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.cdp1861Configuration.x = -1;
        computerConfiguration.cdp1861Configuration.y = -1;
    }
    else
    {
        if (position.x > 0)
            computerConfiguration.cdp1861Configuration.x = position.x;
        if (position.y > 0)
            computerConfiguration.cdp1861Configuration.y = position.y;
    }
}

wxPoint GuiMain::getCdp1864Pos()
{
    return wxPoint(computerConfiguration.cdp1864Configuration.x, computerConfiguration.cdp1864Configuration.y);
}

void GuiMain::setCdp1864Pos(wxPoint position)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.cdp1864Configuration.x = -1;
        computerConfiguration.cdp1864Configuration.y = -1;
    }
    else
    {
        if (position.x > 0)
            computerConfiguration.cdp1864Configuration.x = position.x;
        if (position.y > 0)
            computerConfiguration.cdp1864Configuration.y = position.y;
    }
}

wxPoint GuiMain::getSt4Pos()
{
    return wxPoint(computerConfiguration.studio4VideoConfiguration.x, computerConfiguration.studio4VideoConfiguration.y);
}

void GuiMain::setSt4Pos(wxPoint position)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.studio4VideoConfiguration.x = -1;
        computerConfiguration.studio4VideoConfiguration.y = -1;
    }
    else
    {
        if (position.x > 0)
            computerConfiguration.studio4VideoConfiguration.x = position.x;
        if (position.y > 0)
            computerConfiguration.studio4VideoConfiguration.y = position.y;
    }
}

wxPoint GuiMain::getVip2KPos()
{
    return wxPoint(computerConfiguration.vip2KVideoConfiguration.x, computerConfiguration.vip2KVideoConfiguration.y);
}

void GuiMain::setVip2KPos(wxPoint position)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.vip2KVideoConfiguration.x = -1;
        computerConfiguration.vip2KVideoConfiguration.y = -1;
    }
    else
    {
        if (position.x > 0)
            computerConfiguration.vip2KVideoConfiguration.x = position.x;
        if (position.y > 0)
            computerConfiguration.vip2KVideoConfiguration.y = position.y;
    }
}

wxPoint GuiMain::getFredPos()
{
    return wxPoint(computerConfiguration.fredVideoConfiguration.x, computerConfiguration.fredVideoConfiguration.y);
}

void GuiMain::setFredPos(wxPoint position)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.fredVideoConfiguration.x = -1;
        computerConfiguration.fredVideoConfiguration.y = -1;
    }
    else
    {
        if (position.x > 0)
            computerConfiguration.fredVideoConfiguration.x = position.x;
        if (position.y > 0)
            computerConfiguration.fredVideoConfiguration.y = position.y;
    }
}

wxPoint GuiMain::getTmsPos()
{
    return wxPoint(computerConfiguration.tmsConfiguration.x, computerConfiguration.tmsConfiguration.y);
}

void GuiMain::setTmsPos(wxPoint position)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.tmsConfiguration.x = -1;
        computerConfiguration.tmsConfiguration.y = -1;
    }
    else
    {
        if (position.x > 0)
            computerConfiguration.tmsConfiguration.x = position.x;
        if (position.y > 0)
            computerConfiguration.tmsConfiguration.y = position.y;
    }
}

wxPoint GuiMain::getVtPos()
{
    return wxPoint(computerConfiguration.videoTerminalConfiguration.x, computerConfiguration.videoTerminalConfiguration.y);
}

void GuiMain::setVtPos(wxPoint position)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.videoTerminalConfiguration.x = -1;
        computerConfiguration.videoTerminalConfiguration.y = -1;
    }
    else
    {
        if (position.x > 0)
            computerConfiguration.videoTerminalConfiguration.x = position.x;
        if (position.y > 0)
            computerConfiguration.videoTerminalConfiguration.y = position.y;
    }
}

wxPoint GuiMain::get6845Pos()
{
    return wxPoint(computerConfiguration.mc6845Configuration.x, computerConfiguration.mc6845Configuration.y);
}

void GuiMain::set6845Pos(wxPoint position)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.mc6845Configuration.x = -1;
        computerConfiguration.mc6845Configuration.y = -1;
    }
    else
    {
        if (position.x > 0)
            computerConfiguration.mc6845Configuration.x = position.x;
        if (position.y > 0)
            computerConfiguration.mc6845Configuration.y = position.y;
    }
}

wxPoint GuiMain::get6847Pos()
{
    return wxPoint(computerConfiguration.mc6847Configuration.x, computerConfiguration.mc6847Configuration.y);
}

void GuiMain::set6847Pos(wxPoint position)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.mc6847Configuration.x = -1;
        computerConfiguration.mc6847Configuration.y = -1;
    }
    else
    {
        if (position.x > 0)
            computerConfiguration.mc6847Configuration.x = position.x;
        if (position.y > 0)
            computerConfiguration.mc6847Configuration.y = position.y;
    }
}


wxPoint GuiMain::get8275Pos()
{
    return wxPoint(computerConfiguration.i8275Configuration.x, computerConfiguration.i8275Configuration.y);
}

void GuiMain::set8275Pos(wxPoint position)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.i8275Configuration.x = -1;
        computerConfiguration.i8275Configuration.y = -1;
    }
    else
    {
        if (position.x > 0)
            computerConfiguration.i8275Configuration.x = position.x;
        if (position.y > 0)
            computerConfiguration.i8275Configuration.y = position.y;
    }
}

wxPoint GuiMain::getV1870Pos()
{
    return wxPoint(computerConfiguration.vis1870Configuration.x, computerConfiguration.vis1870Configuration.y);
}

void GuiMain::setV1870Pos(wxPoint position)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.vis1870Configuration.x = -1;
        computerConfiguration.vis1870Configuration.y = -1;
    }
    else
    {
        if (position.y > 0)
            computerConfiguration.vis1870Configuration.x = position.x;
        if (position.x > 0)
            computerConfiguration.vis1870Configuration.y = position.y;
    }
}

wxPoint GuiMain::getSN76430NPos()
{
    return wxPoint(computerConfiguration.sn76430NConfiguration.x, computerConfiguration.sn76430NConfiguration.y);
}

void GuiMain::setSN76430NPos(wxPoint position)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.sn76430NConfiguration.x = -1;
        computerConfiguration.sn76430NConfiguration.y = -1;
    }
    else
    {
        if (position.y > 0)
            computerConfiguration.sn76430NConfiguration.x = position.x;
        if (position.x > 0)
            computerConfiguration.sn76430NConfiguration.y = position.y;
    }
}

wxPoint GuiMain::getCdp1851Pos(int number)
{
    return wxPoint(computerConfiguration.cdp1851Configuration[number].pos.x, computerConfiguration.cdp1851Configuration[number].pos.y);
}

void GuiMain::setCdp1851Pos(wxPoint position, int number)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.cdp1851Configuration[number].pos.x = -1;
        computerConfiguration.cdp1851Configuration[number].pos.y = -1;
    }
    else
    {
        if (position.y > 0)
            computerConfiguration.cdp1851Configuration[number].pos.x = position.x;
        if (position.x > 0)
            computerConfiguration.cdp1851Configuration[number].pos.y = position.y;
    }
}

wxPoint GuiMain::getCdp1852Pos(int number)
{
    return wxPoint(computerConfiguration.cdp1852Configuration[number].pos.x, computerConfiguration.cdp1852Configuration[number].pos.y);
}

void GuiMain::setCdp1852Pos(wxPoint position, int number)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.cdp1852Configuration[number].pos.x = -1;
        computerConfiguration.cdp1852Configuration[number].pos.y = -1;
    }
    else
    {
        if (position.y > 0)
            computerConfiguration.cdp1852Configuration[number].pos.x = position.x;
        if (position.x > 0)
            computerConfiguration.cdp1852Configuration[number].pos.y = position.y;
    }
}

wxPoint GuiMain::getFrontPanelPos(int number)
{
    return wxPoint(computerConfiguration.frontPanelConfiguration[number].pos.x, computerConfiguration.frontPanelConfiguration[number].pos.y);
}

void GuiMain::setFrontPanelPos(wxPoint position, int number)
{
    if (!mode_.window_position_fixed)
    {
        computerConfiguration.frontPanelConfiguration[number].pos.x = -1;
        computerConfiguration.frontPanelConfiguration[number].pos.y = -1;
    }
    else
    {
        if (position.y > 0)
            computerConfiguration.frontPanelConfiguration[number].pos.x = position.x;
        if (position.x > 0)
            computerConfiguration.frontPanelConfiguration[number].pos.y = position.y;
    }
}

int GuiMain::pload()
{
    wxFFile inputFile;
    size_t length, arrayLength;
    int address, start, startAddress, dataEnd, stringStart, fAndMBasicOffset;
//    Byte slot;
    char buffer[65536];
    wxString printBuffer;
    Byte highRamAddress;

    computerConfiguration.memAccessConfiguration.saveExec = 0;

    if (inputFile.Open(computerConfiguration.memAccessConfiguration.fullFileName, _("rb")))
    {
        inputFile.Read(buffer, 4);
        inputFile.Close();

        if (buffer[0] == ':')
        {
            if (p_Computer->readIntelFile(computerConfiguration.memAccessConfiguration.fullFileName, RAM, 0x10000, true))
                return 0;
            else
                return wxNOT_FOUND;
        }
        else if (buffer[0] == '0' && buffer[1] == '0' && buffer[2] == '0' && buffer[3] == '0')
        {
            if (p_Computer->readLstFile(computerConfiguration.memAccessConfiguration.fullFileName, RAM, 0x10000, true))
                return 0;
            else
                return wxNOT_FOUND;
        }
    }

    if (buffer [0] != 1)
    {
        p_Main->eventSetTextValue("SaveStartXml", "");
        p_Main->eventSetTextValue("SaveEndXml", "");
        p_Main->eventSetTextValue("SaveExecXml", "");
    }

    if (inputFile.Open(computerConfiguration.memAccessConfiguration.fullFileName, _("rb")))
    {
        length = inputFile.Read(buffer, 65536);
        address = computerConfiguration.basicConfiguration.ramAddress.value;
        start = computerConfiguration.basicConfiguration.ramAddress.value & 0xf00;
        
        if (!(buffer [1] == computerConfiguration.basicConfiguration.pLoadSaveName[0] && buffer [2] == computerConfiguration.basicConfiguration.pLoadSaveName[1]))
        {
            p_Main->eventShowMessageBox( computerConfiguration.memAccessConfiguration.fileName+" might not be compatible.",
                                        "Load anyway?", wxICON_EXCLAMATION | wxYES_NO);

            if (messageBoxAnswer_ == wxNO)
                return wxNOT_FOUND;
        }
        
        if (buffer [0] == 1 || buffer [0] == 2 || buffer [0] == 3 || buffer [0] == 4 || buffer [0] == 5 || buffer [0] == 6)
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
                    address = computerConfiguration.basicConfiguration.ramAddress.value;
                    if (p_Computer->isFAndMBasicRunning())
                    {
                        fAndMBasicOffset = 0x23;
                        if (buffer[5] != 0x44)
                        {
                            p_Main->errorMessage(    "File " + computerConfiguration.memAccessConfiguration.fullFileName + " can only be loaded in COMX Basic");
                            return wxNOT_FOUND;
                        }
                        address = 0x6700;
                    }
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.defus.value, (Byte)buffer[5]+fAndMBasicOffset);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.defus.value+1, (Byte)buffer[6]);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eop.value, (Byte)buffer[7]+fAndMBasicOffset);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eop.value+1, (Byte)buffer[8]);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.string.value, (Byte)buffer[9]+fAndMBasicOffset);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.string.value+1, (Byte)buffer[10]);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.array.value, (Byte)buffer[11]+fAndMBasicOffset);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.array.value+1, (Byte)buffer[12]);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eod.value, (Byte)buffer[9]+fAndMBasicOffset);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eod.value+1, (Byte)buffer[10]);
                    p_Main->eventSetLocation(false);
                    start = 15;
                break;

                case 3: /* Regular F&M BASIC LOAD */
                    if (p_Computer->isFAndMBasicRunning())
                        address = 0x6700;
                    else
                    {
                        p_Main->errorMessage( "File " + computerConfiguration.memAccessConfiguration.fullFileName + " can only be loaded in F&M Basic V2.00");
                        return wxNOT_FOUND;
                    }
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.defus.value, (Byte)buffer[5]);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.defus.value+1, (Byte)buffer[6]);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eop.value, (Byte)buffer[7]);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eop.value+1, (Byte)buffer[8]);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.string.value, (Byte)buffer[9]);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.string.value+1, (Byte)buffer[10]);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.array.value, (Byte)buffer[11]);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.array.value+1, (Byte)buffer[12]);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eod.value, (Byte)buffer[9]);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eod.value+1, (Byte)buffer[10]);
                    p_Main->eventSetLocation(false);
                    start = 15;
                break;

                case 4: /* Old, incorrect Data only LOAD */
                    address = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.string.value) << 8;
                    address += p_Computer->getMainMemory(computerConfiguration.basicConfiguration.string.value+1);
                    start = 5;
                    dataEnd = address + (int)length - start;
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eod.value, (dataEnd >> 8) & 0xff);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eod.value+1, dataEnd & 0xff);
                break;

                case 5: /* Data only LOAD */
                    address = p_Computer->getMainMemory(computerConfiguration.basicConfiguration.array.value) << 8;
                    address += p_Computer->getMainMemory(computerConfiguration.basicConfiguration.array.value+1);
                    start = 7;
                    arrayLength = (Byte)buffer[5] << 8;
                    arrayLength += (Byte)buffer[6];
                    dataEnd = address + (int)length - start;
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eod.value, (dataEnd >> 8) & 0xff);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eod.value+1, dataEnd & 0xff);
                    stringStart = address + (int)arrayLength;
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.string.value, (stringStart >> 8) & 0xff);
                    p_Computer->setMainMemory(computerConfiguration.basicConfiguration.string.value+1, stringStart & 0xff);
                break;

                case 6: /* New Regular LOAD */
                    fAndMBasicOffset = 0;
                    address = computerConfiguration.basicConfiguration.ramAddress.value;
                    highRamAddress = (computerConfiguration.basicConfiguration.ramAddress.value & 0xff00) >> 8;
                    if (p_Computer->isFAndMBasicRunning())
                    {
                        fAndMBasicOffset = 0x23;
                        if (buffer[5] != 0)
                        {
                            p_Main->errorMessage("File " + computerConfiguration.memAccessConfiguration.fullFileName + " can only be loaded in COMX Basic");
                            return wxNOT_FOUND;
                        }
                        address = 0x6700;
                    }
                    
                    if (computerConfiguration.basicConfiguration.defus.defined)
                    {
                        p_Computer->setMainMemory(computerConfiguration.basicConfiguration.defus.value, (Byte)buffer[5]+fAndMBasicOffset+highRamAddress);
                        p_Computer->setMainMemory(computerConfiguration.basicConfiguration.defus.value+1, (Byte)buffer[6]);
                    }
                    if (computerConfiguration.basicConfiguration.string.defined)
                    {
                        p_Computer->setMainMemory(computerConfiguration.basicConfiguration.string.value, (Byte)buffer[9]+fAndMBasicOffset+highRamAddress);
                        p_Computer->setMainMemory(computerConfiguration.basicConfiguration.string.value+1, (Byte)buffer[10]);
                    }
                    if (computerConfiguration.basicConfiguration.array.defined)
                    {
                        p_Computer->setMainMemory(computerConfiguration.basicConfiguration.array.value, (Byte)buffer[11]+fAndMBasicOffset+highRamAddress);
                        p_Computer->setMainMemory(computerConfiguration.basicConfiguration.array.value+1, (Byte)buffer[12]);
                    }
                    if (computerConfiguration.basicConfiguration.eod.defined)
                    {
                        p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eod.value, (Byte)buffer[9]+fAndMBasicOffset+highRamAddress);
                        p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eod.value+1, (Byte)buffer[10]);
                    }
                    if (computerConfiguration.basicConfiguration.eop.defined)
                    {
                        p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eop.value, (Byte)buffer[7]+fAndMBasicOffset+highRamAddress);
                        p_Computer->setMainMemory(computerConfiguration.basicConfiguration.eop.value+1, (Byte)buffer[8]);
                    }

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
            //        slot = 0;
            //        if (runningComputer_ == COMX)
            //            slot = p_Computer->getOutValue(1);
            //        p_Main->AssAddConfig(computerConfiguration.memAccessConfiguration.directory, computerConfiguration.memAccessConfiguration.fileName, startAddress, address, address, slot, port);

            return computerConfiguration.memAccessConfiguration.saveExec;
        }
        else
        {
            p_Main->eventSetLocation(false);
            address = computerConfiguration.memAccessConfiguration.saveStart;
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
                    return wxNOT_FOUND;
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
        p_Main->errorMessage("Error reading " + computerConfiguration.memAccessConfiguration.fullFileName);
        return wxNOT_FOUND;
    }
}

bool GuiMain::startCassetteLoad(int tapeNumber)
{
    if (computerConfiguration.autoCassetteLoad_)
        return startLoad(tapeNumber);
    else
        return false;
}

void GuiMain::onRealCas(wxCommandEvent&WXUNUSED(event))
{
    if (computerRunning_)
        p_Computer->pauseTape();

    if (computerRunning_ && !computerConfiguration.realCassetteLoad_ && !p_Computer->getAudioInStatus())
        (void)wxMessageBox( "SDL audio input initialization or start\n"
                            "failed, real cassette loading not possible" ,
                            "Emma 02", wxICON_ERROR | wxOK );
    else
    {
        computerConfiguration.realCassetteLoad_ = !computerConfiguration.realCassetteLoad_;
        setRealCas();
    }
}

void GuiMain::setRealCas()
{
    if (!mode_.gui)
        return;

    bool useTape = computerConfiguration.swTapeConfiguration.defined || computerConfiguration.hwTapeConfiguration.defined;

    if (computerConfiguration.realCassetteLoad_)
    {
        computerConfiguration.autoCassetteLoad_ = false;
#if defined(__WXMSW__)
        XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->SetBitmapLabel(realCasOnBitmap);
        XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->SetBitmapCurrent(realCasOnBitmap);
        XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->SetBitmapFocus(realCasOnBitmap);
        XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->SetToolTip("Disable real cassette loading");
#endif
        XRCCTRL(*this, "TurboXml", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "TurboXml", wxCheckBox)->SetValue(false);
        turboGui();
        XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->SetValue(false);
        if (computerRunning_)
        {
            XRCCTRL(*this, "CasLoadXml", wxButton)->Enable(false);
            XRCCTRL(*this, "CasSaveXml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
            XRCCTRL(*this, "CasForwardXml", wxButton)->Enable(false);
            XRCCTRL(*this, "CasRewindXml", wxButton)->Enable(false);
            if (computerConfiguration.swTapeConfiguration.twoDecks || computerConfiguration.hwTapeConfiguration.twoDecks)
            {
                XRCCTRL(*this, "CasLoad1Xml", wxButton)->Enable(false);
                XRCCTRL(*this, "CasSave1Xml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
            }
        }
    }
    else
    {
#if defined(__WXMSW__)
        XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->SetBitmapLabel(realCasOffBitmap);
        XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->SetBitmapCurrent(realCasOffBitmap);
        XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->SetBitmapFocus(realCasOffBitmap);
        XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->SetToolTip("Enable real cassette loading");
#endif
        XRCCTRL(*this, "TurboXml", wxCheckBox)->Enable(useTape);
        XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->Enable((useTape || computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined || isTapeHwFred()) & !isTapeHwCybervision());
        if (computerRunning_)
        {
            XRCCTRL(*this, "CasLoadXml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_|computerConfiguration.hwTapeConfiguration.defined);
            XRCCTRL(*this, "CasSaveXml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_|computerConfiguration.hwTapeConfiguration.defined);
            XRCCTRL(*this, "CasForwardXml", wxButton)->Enable(computerConfiguration.hwTapeConfiguration.defined);
            XRCCTRL(*this, "CasRewindXml", wxButton)->Enable(computerConfiguration.hwTapeConfiguration.defined);
            if (computerConfiguration.hwTapeConfiguration.defined)
                XRCCTRL(*this, "CasStopXml", wxButton)->Enable(true);
            if (computerConfiguration.swTapeConfiguration.twoDecks || computerConfiguration.hwTapeConfiguration.twoDecks)
            {
                XRCCTRL(*this, "CasLoad1Xml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
                XRCCTRL(*this, "CasSave1Xml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
            }
        }
    }
    if (computerRunning_)
        p_Computer->record_pause(computerConfiguration.realCassetteLoad_);
}

void GuiMain::setRealCas2()
{
    if (!mode_.gui)
        return;

    bool useTape = computerConfiguration.swTapeConfiguration.defined || computerConfiguration.hwTapeConfiguration.defined;

    if (computerConfiguration.realCassetteLoad_)
    {
        XRCCTRL(*this, "TurboXml", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "TurboXml", wxCheckBox)->SetValue(false);
        turboGui();
        XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->SetValue(false);
        computerConfiguration.autoCassetteLoad_ = false;
        if (computerRunning_)
        {
            XRCCTRL(*this, "CasLoadXml", wxButton)->Enable(false);
            XRCCTRL(*this, "CasSaveXml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
            XRCCTRL(*this, "CasForwardXml", wxButton)->Enable(false);
            XRCCTRL(*this, "CasRewindXml", wxButton)->Enable(false);
            if (computerConfiguration.swTapeConfiguration.twoDecks || computerConfiguration.hwTapeConfiguration.twoDecks)
            {
                XRCCTRL(*this, "CasLoad1Xml", wxButton)->Enable(false);
                XRCCTRL(*this, "CasSave1Xml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
            }
        }
    }
    else
    {
        XRCCTRL(*this, "TurboXml", wxCheckBox)->Enable(useTape || computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined);
        XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->Enable((useTape || computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined || isTapeHwFred()) & !isTapeHwCybervision());
        if (computerRunning_)
        {
            XRCCTRL(*this, "CasLoadXml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_|computerConfiguration.hwTapeConfiguration.defined);
            XRCCTRL(*this, "CasSaveXml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_|computerConfiguration.hwTapeConfiguration.defined);
            XRCCTRL(*this, "CasForwardXml", wxButton)->Enable(computerConfiguration.hwTapeConfiguration.defined);
            XRCCTRL(*this, "CasRewindXml", wxButton)->Enable(computerConfiguration.hwTapeConfiguration.defined);
            if (computerConfiguration.hwTapeConfiguration.defined)
                XRCCTRL(*this, "CasStopXml", wxButton)->Enable(true);
            if (computerConfiguration.swTapeConfiguration.twoDecks || computerConfiguration.hwTapeConfiguration.twoDecks)
            {
                XRCCTRL(*this, "CasLoad1Xml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
                XRCCTRL(*this, "CasSave1Xml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
            }
        }
    }
}

void GuiMain::setRealCasOff()
{
    computerConfiguration.realCassetteLoad_ = false;

    if (!mode_.gui)
        return;

    bool useTape = computerConfiguration.swTapeConfiguration.defined || computerConfiguration.hwTapeConfiguration.defined;

#if defined(__WXMSW__)
    XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->SetBitmapLabel(realCasOffBitmap);
    XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->SetBitmapCurrent(realCasOffBitmap);
    XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->SetBitmapFocus(realCasOffBitmap);
    XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->SetToolTip("Enable real cassette loading");
#endif
    XRCCTRL(*this, "TurboXml", wxCheckBox)->Enable(useTape);
    XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->Enable((useTape || computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined || isTapeHwFred()) & !isTapeHwCybervision());
    
    if (computerRunning_)
    {
        XRCCTRL(*this, "CasLoadXml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_|computerConfiguration.hwTapeConfiguration.defined);
        XRCCTRL(*this, "CasSaveXml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_|computerConfiguration.hwTapeConfiguration.defined);
        XRCCTRL(*this, "CasForwardXml", wxButton)->Enable(computerConfiguration.hwTapeConfiguration.defined);
        XRCCTRL(*this, "CasRewindXml", wxButton)->Enable(computerConfiguration.hwTapeConfiguration.defined);
        if (computerConfiguration.hwTapeConfiguration.defined)
            XRCCTRL(*this, "CasStopXml", wxButton)->Enable(true);
        if (computerConfiguration.swTapeConfiguration.twoDecks || computerConfiguration.hwTapeConfiguration.twoDecks)
        {
            XRCCTRL(*this, "CasLoad1Xml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
            XRCCTRL(*this, "CasSave1Xml", wxButton)->Enable(!computerConfiguration.autoCassetteLoad_);
        }
    }
}

bool GuiMain::startLoad(int tapeNumber)
{
    wxString filePath, fileName, tapeString;
    tapeString.Printf("%d", tapeNumber);

    filePath = computerConfiguration.wavConfiguration[tapeNumber].directory;
    fileName = computerConfiguration.wavConfiguration[tapeNumber].fileName;
    filePath.operator += (fileName);

    if (fileName.Len() != 0)
    {
        if (wxFile::Exists(filePath))
        {
            if (tapeNumber == 0)
                p_Main->eventSetTapeState(TAPE_PLAY, tapeString);
            else
                p_Main->eventSetTapeState(TAPE_PLAY1, tapeString);
            return p_Computer->ploadStartTape(filePath, tapeString);
        }
    }
    return false;
}

void GuiMain::stopCassette()
{
    if (computerConfiguration.autoCassetteLoad_)
        p_Computer->stopTape();
}

void GuiMain::startCassetteSave(int tapeNumber)
{
    if (computerConfiguration.autoCassetteLoad_)
        startSaveNew(tapeNumber);
}

void GuiMain::startSaveNew(int tapeNumber)
{
    startSave(tapeNumber, "Do you want to replace it?", false);
}

bool GuiMain::startSaveCont(int tapeNumber, wxString tapeCounterStr)
{
    wxString messageStr;
    
    bool cont;
    if (tapeCounterStr == "00:00:000")
    {
        cont = false;
        messageStr = "Do you want to replace it?";
    }
    else
    {
        cont = true;
        messageStr = "Continue save at: " + tapeCounterStr;
    }
    
    return startSave(tapeNumber, messageStr, cont);
}

bool GuiMain::startSave(int tapeNumber, wxString messageStr, bool cont)
{
    wxString filePath, fileName, tapeString;
    tapeString.Printf("%d", tapeNumber);
    if (tapeNumber == 0)
        tapeString = "";

    filePath = computerConfiguration.wavConfiguration[tapeNumber].directory;
    fileName = computerConfiguration.wavConfiguration[tapeNumber].fileName;
    filePath.operator += (fileName);

    if (fileName.Len() == 0)
    {
        fileName = p_Main->eventShowFileSelector( "Select the WAV file to save",
                       computerConfiguration.wavConfiguration[tapeNumber].directory, fileName,
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
            if (computerConfiguration.swTapeConfiguration.keyClear)
                p_Computer->keyClear();
            return false;
        }

        wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
        computerConfiguration.wavConfiguration[tapeNumber].fileName = FullPath.GetFullName();
        computerConfiguration.wavConfiguration[tapeNumber].directory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

        p_Main->eventSetTextValue("WavFile"+tapeString+"Xml", computerConfiguration.wavConfiguration[tapeNumber].fileName);

        filePath = computerConfiguration.wavConfiguration[tapeNumber].directory;
        filePath.operator += (computerConfiguration.wavConfiguration[tapeNumber].fileName);
    }
    if (wxFile::Exists(filePath))
    {
        p_Main->eventShowMessageBox( fileName+" already exists.\n"+messageStr,
                                    "Confirm Save As", wxICON_EXCLAMATION | wxYES_NO);

        if (messageBoxAnswer_ == wxNO && cont)
        {
            p_Main->eventShowMessageBox( fileName+" already exists.\n"+"Do you want to overwrite?",
                                        "Confirm Save As", wxICON_EXCLAMATION | wxYES_NO);
            
            if (messageBoxAnswer_ == wxYES)
                p_Computer->stopTape();
        }

        if (messageBoxAnswer_ == wxNO)
        {
            fileName = p_Main->eventShowFileSelector( "Select the WAV file to save",
                           computerConfiguration.wavConfiguration[tapeNumber].directory, fileName,
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
                if (computerConfiguration.swTapeConfiguration.keyClear)
                    p_Computer->keyClear();
                return false;
            }

            wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
            computerConfiguration.wavConfiguration[tapeNumber].fileName = FullPath.GetFullName();
            computerConfiguration.wavConfiguration[tapeNumber].directory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

            p_Main->eventSetTextValue("WavFile"+tapeString+"Xml", computerConfiguration.wavConfiguration[tapeNumber].fileName);

            filePath = computerConfiguration.wavConfiguration[tapeNumber].directory;
            filePath.operator += (computerConfiguration.wavConfiguration[tapeNumber].fileName);
        }
        if (computerConfiguration.swTapeConfiguration.keyClear)
            p_Computer->keyClear();
    }
    
    tapeString.Printf("%d", tapeNumber);
    if (tapeNumber == 0)
        p_Main->eventSetTapeState(TAPE_RECORD, tapeString);
    else
        p_Main->eventSetTapeState(TAPE_RECORD1, tapeString);
    
    if (isTapeHwCybervision())
        p_Computer->startSaveTapeHw(filePath, tapeString);
    else
        p_Computer->startSaveTape(filePath, tapeString);
    return true;
}

void GuiMain::onTerminalSave(wxCommandEvent&WXUNUSED(event))
{
    p_Main->startTerminalSave(TERM_HEX);
}

void GuiMain::onTerminalLoad(wxCommandEvent&WXUNUSED(event))
{
    startTerminalLoad(TERM_HEX);
}

void GuiMain::startAutoTerminalLoad(int protocol)
{
    if (!computerRunning_)
        return;

    if (computerConfiguration.autoCassetteLoad_)
        startTerminalLoad(protocol);
}

void GuiMain::startTerminalLoad(int protocol)
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
    
    if (fileName.Len() != 0)
    {
        if (wxFile::Exists(filePath))
        {
            terminalLoad_ = true;
            
            p_Main->eventSetTapeState(TAPE_PLAY, "");
            p_Computer->terminalLoad(filePath, fileName, protocol);
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
    
    if (computerConfiguration.autoCassetteLoad_)
    {
        stopTerminal();
        p_Computer->terminalStop();
    }
}

void GuiMain::stopTerminal()
{
    terminalSave_ = false;
    terminalLoad_ = false;
    
    p_Main->eventSetTapeState(TAPE_STOP, "");
}

void GuiMain::startYsTerminalSave(int protocol)
{
    if (terminalSave_ || terminalLoad_)
    {
        stopTerminal();
        p_Computer->terminalStop();
    }
    
    wxString filePath;
    filePath = computerConfiguration.videoTerminalConfiguration.xmodemDirectory;

    terminalSave_ = true;

    p_Main->eventSetTapeState(TAPE_RECORD, "");
    p_Computer->terminalYsSave(filePath, protocol);
}

void GuiMain::turboOn()
{
    if (turboOn_)
    {
        wxString clock =  computerConfiguration.turboClock_;
        toDouble(clock, (double*)&computerConfiguration.clockSpeed_);

        setClockRate();
        return;
    }
    
    if (computerConfiguration.turbo_)
    {
        if (mode_.gui)
            p_Main->eventEnableClock(false);
        savedSpeed_ = computerConfiguration.clockSpeed_;

        wxString clock =  computerConfiguration.turboClock_;
        toDouble(clock, (double*)&computerConfiguration.clockSpeed_);

        setClockRate();
        turboOn_ = true;
    }
}

void GuiMain::turboOff()
{
    if (!turboOn_)  return;
    if (computerConfiguration.turbo_)
    {
        computerConfiguration.clockSpeed_ = savedSpeed_;
        setClockRate();

        if (mode_.gui)
            p_Main->eventEnableClock(true);
        turboOn_ = false;
    }
}

void GuiMain::enableStartButtonGui(bool status)
{
    startButton->Enable(status);
    stopButton->Enable(false);

    if (status)
    {
        startButton->SetLabel("Start");
        startButton->SetToolTip("Start " + computerInfo.name + " emulator (F12)");
    }
    else
    {
        startButton->SetLabel("Reset");
        startButton->SetToolTip("Reset " + computerInfo.name + " emulator (F12)");
        startButton->Enable(true);
        stopButton->Enable(true);
    }
}

void GuiMain::enableLocationGui()
{
    if (!mode_.gui)
        return;

    XRCCTRL(*this, "TextStartXml", wxStaticText)->Enable(computerConfiguration.memAccessConfiguration.useLocation);
    XRCCTRL(*this, "TextEndXml", wxStaticText)->Enable(computerConfiguration.memAccessConfiguration.useLocation);
    XRCCTRL(*this, "TextExecXml", wxStaticText)->Enable(computerConfiguration.memAccessConfiguration.useLocation);
    XRCCTRL(*this, "SaveStartXml", wxTextCtrl)->Enable(computerConfiguration.memAccessConfiguration.useLocation);
    XRCCTRL(*this, "SaveEndXml", wxTextCtrl)->Enable(computerConfiguration.memAccessConfiguration.useLocation);
    XRCCTRL(*this, "SaveExecXml", wxTextCtrl)->Enable(computerConfiguration.memAccessConfiguration.useLocation);
}

void GuiMain::enableMemAccessGui(bool status)
{
    if (!mode_.gui)
        return;

    XRCCTRL(*this, "SaveButtonXml", wxButton)->Enable(status);
    XRCCTRL(*this, "LoadButtonXml", wxButton)->Enable(status);
    XRCCTRL(*this, "RunButtonXml", wxButton)->Enable(status);
    XRCCTRL(*this, "UseLocationXml", wxCheckBox)->Enable(status);
    XRCCTRL(*this, "UseLocationTextXml", wxStaticText)->Enable(status);
    XRCCTRL(*this, "DsaveButtonXml", wxButton)->Enable(status);
    enableLocationGui();
}

void GuiMain::enableTapeGui(bool status)
{
    XRCCTRL(*this, "CasButtonXml", wxButton)->Enable(status);
    XRCCTRL(*this, "WavFileXml", wxTextCtrl)->Enable(status);
    XRCCTRL(*this, "EjectCasXml", wxButton)->Enable(status);
    XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->Enable((status&!computerConfiguration.realCassetteLoad_) & !isTapeHwCybervision());
    XRCCTRL(*this, "TurboXml", wxCheckBox)->Enable(status&!computerConfiguration.realCassetteLoad_);
    XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->Enable(status);
    XRCCTRL(*this, "TurboMhzTextXml", wxStaticText)->Enable(status);
#if defined(__WXMSW__)
    XRCCTRL(*this, "RealCasLoadXml", wxBitmapButton)->Enable(status&(!(computerConfiguration.videoTerminalConfiguration.xModem_defined && !computerConfiguration.videoTerminalConfiguration.hexModem_defined)));
#endif
}

void GuiMain::enableLoadGui(bool status)
{
    enableMemAccessGui(status);

    //if (!(computerConfiguration.videoTerminalConfiguration.xModem_defined || computerConfiguration.videoTerminalConfiguration.hexModem_defined))
    //{
    //    enableTapeGui(false);
    //    return;
    //}
    if (computerRunning_)
    {
        if (computerConfiguration.swTapeConfiguration.defined || computerConfiguration.hwTapeConfiguration.defined)
        {
            XRCCTRL(*this, "CasButtonXml", wxButton)->Enable(status);
            XRCCTRL(*this, "WavFileXml", wxTextCtrl)->Enable(status);
            XRCCTRL(*this, "EjectCasXml", wxButton)->Enable(status);
        }
        XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->Enable((status&!computerConfiguration.realCassetteLoad_) & !isTapeHwCybervision());
        XRCCTRL(*this, "TurboXml", wxCheckBox)->Enable(status&!computerConfiguration.realCassetteLoad_);
        if (!status)
        {
            XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->Enable(false);
            XRCCTRL(*this, "TurboMhzTextXml", wxStaticText)->Enable(false);
        }
        else
        {
            XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->Enable(XRCCTRL(*this, "TurboXml", wxCheckBox)->IsChecked());
            XRCCTRL(*this, "TurboMhzTextXml", wxStaticText)->Enable(XRCCTRL(*this, "TurboXml", wxCheckBox)->IsChecked());
        }
    }
    else
    {
        XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->Enable(true  & !isTapeHwCybervision());
        XRCCTRL(*this, "TurboXml", wxCheckBox)->Enable(true);
        if (computerConfiguration.swTapeConfiguration.defined || computerConfiguration.hwTapeConfiguration.defined)
        {
            XRCCTRL(*this, "CasButtonXml", wxButton)->Enable(true);
            XRCCTRL(*this, "WavFileXml", wxTextCtrl)->Enable(true);
            XRCCTRL(*this, "EjectCasXml", wxButton)->Enable(true);
        }
        XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->Enable(true);
        XRCCTRL(*this, "TurboMhzTextXml", wxStaticText)->Enable(true);
    }
    if (!computerConfiguration.autoCassetteLoad_)
    {
        if (isTapeHwFred())
            XRCCTRL(*this, "CasPauseXml", wxButton)->Enable(false);
        XRCCTRL(*this, "CasStopXml", wxButton)->Enable(status&!computerConfiguration.realCassetteLoad_);
        XRCCTRL(*this, "CasLoadXml", wxButton)->Enable(status&!computerConfiguration.realCassetteLoad_);
        XRCCTRL(*this, "CasSaveXml", wxButton)->Enable(status&!computerConfiguration.videoTerminalConfiguration.hexModem_defined);
        if (computerRunning_ && (computerConfiguration.swTapeConfiguration.twoDecks || computerConfiguration.hwTapeConfiguration.twoDecks))
        {
            XRCCTRL(*this, "CasStop1Xml", wxButton)->Enable(false);
            XRCCTRL(*this, "CasLoad1Xml", wxButton)->Enable(status&!computerConfiguration.realCassetteLoad_);
            XRCCTRL(*this, "CasSave1Xml", wxButton)->Enable(status);
        }
    }
    if (computerRunning_ && isTapeHwCybervision())
    {
        if (computerConfiguration.hwTapeConfiguration.defined)
        {
            XRCCTRL(*this, "CasLoadXml", wxButton)->Enable(status);
            XRCCTRL(*this, "CasForwardXml", wxButton)->Enable(status);
            XRCCTRL(*this, "CasRewindXml", wxButton)->Enable(status);
            XRCCTRL(*this, "CasSaveXml", wxButton)->Enable(status);
            XRCCTRL(*this, "CasStopXml", wxButton)->Enable(status);
        }
        if (tapeState_ == TAPE_PLAY)
        {
            if (hwTapeState_ == HW_TAPE_STATE_PLAY)
                XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playDarkGreenBitmap);
            else
                XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
        }
        if (tapeState_ == TAPE_RECORD)
        {
            if (hwTapeState_ == HW_TAPE_STATE_REC)
                XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recPressedBitmap);
            else
                XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
        }
    }
    else
    {
        if (tapeState_ == TAPE_PLAY)
            XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
        if (tapeState_ == TAPE_RECORD)
            XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
    }
    if (tapeState_ == TAPE_FF)
        XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardDarkGreenBitmap);
    if (tapeState_ == TAPE_RW)
        XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindDarkGreenBitmap);
    if (tapeState_ == TAPE_RECORD1)
        XRCCTRL(*this, "CasSave1Xml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
    if (tapeState_ == TAPE_PLAY1)
        XRCCTRL(*this, "CasLoad1Xml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
    tapeState_ = TAPE_STOP;
}

void GuiMain::setTapeState(int tapeState, wxString tapeNumber)
{
    if (tapeNumber == "0")
        tapeNumber = "";
    
    tapeState_ = tapeState;
    if (!mode_.gui)
        return;

    if (isTapeHwFred())
    {
        if (tapeState == TAPE_PAUSE)
        {
            XRCCTRL(*this, "CasPauseXml", wxBitmapButton)->SetBitmapLabel(pauseOnBitmap);
            tapeState = TAPE_STOP;
        }
        else
            XRCCTRL(*this, "CasPauseXml", wxBitmapButton)->SetBitmapLabel(pauseOffBitmap);
    }
    
    if  (computerConfiguration.videoTerminalConfiguration.hexModem_defined || computerConfiguration.videoTerminalConfiguration.xModem_defined)
    {
        XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->Enable(true);
        XRCCTRL(*this, "TurboXml", wxCheckBox)->Enable(true);
        XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->Enable(XRCCTRL(*this, "TurboXml", wxCheckBox)->IsChecked());
        XRCCTRL(*this, "TurboMhzTextXml", wxStaticText)->Enable(XRCCTRL(*this, "TurboXml", wxCheckBox)->IsChecked());
        if (computerConfiguration.autoCassetteLoad_)
        {
            XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
            XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
        }
        else
        {
            XRCCTRL(*this, "CasStopXml", wxButton)->Enable(true);
            XRCCTRL(*this, "CasLoadXml", wxButton)->Enable(true);
            XRCCTRL(*this, "CasSaveXml", wxButton)->Enable(true);
        }
    }
    
    if (!(computerConfiguration.swTapeConfiguration.defined || computerConfiguration.hwTapeConfiguration.defined))
        return;
    
    XRCCTRL(*this, "CasButton"+tapeNumber+"Xml", wxButton)->Enable(tapeState == TAPE_STOP);
    XRCCTRL(*this, "WavFile"+tapeNumber+"Xml", wxTextCtrl)->Enable(tapeState == TAPE_STOP);
    XRCCTRL(*this, "EjectCas"+tapeNumber+"Xml", wxButton)->Enable(tapeState == TAPE_STOP);
    XRCCTRL(*this, "AutoCasLoadXml", wxCheckBox)->Enable(((tapeState == TAPE_STOP)&!computerConfiguration.realCassetteLoad_) & !isTapeHwCybervision());
    XRCCTRL(*this, "TurboXml", wxCheckBox)->Enable((tapeState == TAPE_STOP)&!computerConfiguration.realCassetteLoad_);
    if (tapeState != TAPE_STOP)
    {
        XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "TurboMhzTextXml", wxStaticText)->Enable(false);
    }
    else
    {
        XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->Enable(XRCCTRL(*this, "TurboXml", wxCheckBox)->IsChecked());
        XRCCTRL(*this, "TurboMhzTextXml", wxStaticText)->Enable(XRCCTRL(*this, "TurboXml", wxCheckBox)->IsChecked());
    }
    if (computerRunning_ && isTapeHwCybervision())
    {
        if (tapeState == TAPE_FF)
            XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardGreenBitmap);
        else
        {
            if (hwTapeState_ == HW_TAPE_STATE_FF)
                XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardDarkGreenBitmap);
            else
                XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
        }
        if (tapeState == TAPE_RW)
            XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindGreenBitmap);
        else
        {
            if (hwTapeState_ == HW_TAPE_STATE_RW)
                XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindDarkGreenBitmap);
            else
                XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
        }
    }
    if (computerConfiguration.autoCassetteLoad_)
    {
        if (computerRunning_ && isTapeHwCybervision())
        {
            if (tapeState == TAPE_PLAY)
                XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playGreenBitmap);
            else
            {
                if (hwTapeState_ == HW_TAPE_STATE_PLAY)
                    XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playDarkGreenBitmap);
                else
                    XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
            }
            if (tapeState == TAPE_RECORD)
                XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOnBitmap);
            else
            {
                if (hwTapeState_ == HW_TAPE_STATE_REC)
                    XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recPressedBitmap);
                else
                    XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
            }
        }
        else
        {
            if (tapeState == TAPE_PLAY)
                XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playGreenBitmap);
            else
                XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
            if (tapeState == TAPE_RECORD)
                XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOnBitmap);
            else
                XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
        }
        if (computerRunning_ && (computerConfiguration.swTapeConfiguration.twoDecks || computerConfiguration.hwTapeConfiguration.twoDecks))
        {
            if (tapeState == TAPE_RECORD1)
                XRCCTRL(*this, "CasSave1Xml", wxBitmapButton)->SetBitmapLabel(recOnBitmap);
            else
                XRCCTRL(*this, "CasSave1Xml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
            if (tapeState == TAPE_PLAY1)
                XRCCTRL(*this, "CasLoad1Xml", wxBitmapButton)->SetBitmapLabel(playGreenBitmap);
            else
                XRCCTRL(*this, "CasLoad1Xml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
        }
    }
    else
    {
        if (isTapeHwFred())
            XRCCTRL(*this, "CasPauseXml", wxButton)->Enable(tapeState != TAPE_STOP);
        XRCCTRL(*this, "CasStopXml", wxButton)->Enable(tapeState != TAPE_STOP);
        XRCCTRL(*this, "CasLoadXml", wxButton)->Enable((tapeState == TAPE_STOP)&!computerConfiguration.realCassetteLoad_);
        if (!computerConfiguration.videoTerminalConfiguration.hexModem_defined)
            XRCCTRL(*this, "CasSaveXml", wxButton)->Enable(tapeState == TAPE_STOP);
        if (computerConfiguration.swTapeConfiguration.twoDecks || computerConfiguration.hwTapeConfiguration.twoDecks)
        {
            XRCCTRL(*this, "CasStop1Xml", wxButton)->Enable(tapeState != TAPE_STOP);
            XRCCTRL(*this, "CasLoad1Xml", wxButton)->Enable((tapeState == TAPE_STOP)&!computerConfiguration.realCassetteLoad_);
            XRCCTRL(*this, "CasSave1Xml", wxButton)->Enable(tapeState == TAPE_STOP);
        }
    }
}

void GuiMain::turboGui()
{
    if (!mode_.gui)
        return;
    XRCCTRL(*this, "TurboClockXml", wxTextCtrl)->Enable(computerConfiguration.turbo_);
    XRCCTRL(*this, "TurboMhzTextXml", wxStaticText)->Enable(computerConfiguration.turbo_);
}

void GuiMain::setComputerInfo(wxString name, wxString ploadExtension)
{
    computerInfo.name = name;
    computerInfo.ploadExtension = ploadExtension;
}

void GuiMain::setScreenInfo(int start, int end, wxString colour[])
{
    screenInfo.start = start;
    screenInfo.number = end;
    screenInfo.numberVideo = 0;
    for (int i=start; i<end; i++)
        screenInfo.defaultColour[i] = colour[i];
}

void GuiMain::setScreenInfo(int start, int end, wxString colour[], int numberVideo, int borderX[], int borderY[])
{
    screenInfo.start = start;
    screenInfo.number = end;
    screenInfo.numberVideo = numberVideo;
    for (int i=start; i<end; i++)
        screenInfo.defaultColour[i] = colour[i];
    for (int i=0; i<screenInfo.numberVideo; i++)
    {
        screenInfo.borderX[i] = borderX[i];
        screenInfo.borderY[i] = borderY[i];
    }
}

ScreenInfo GuiMain::getScreenInfo()
{
    return screenInfo;
}

void GuiMain::setBaudChoice()
{
    XRCCTRL(*this, "VTBaudRTextXml", wxStaticText)->Enable(computerConfiguration.videoTerminalConfiguration.uart1854_defined || computerConfiguration.videoTerminalConfiguration.uart16450_defined);
    XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->Enable(computerConfiguration.videoTerminalConfiguration.uart1854_defined|| computerConfiguration.videoTerminalConfiguration.uart16450_defined);
}

void GuiMain::setBaud(int baudR, int baudT)
{
    if (mode_.gui)
    {
        XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->SetSelection(baudR);
        XRCCTRL(*this, "VTBaudTChoiceXml", wxChoice)->SetSelection(baudT);
    }
    computerConfiguration.videoTerminalConfiguration.baudR = baudR;
    computerConfiguration.videoTerminalConfiguration.baudT = baudT;
}

void GuiMain::saveSetup(int baudR, int baudT, bitset<32> setupFeature, int charactersPerRow, int charWidth)
{
    computerConfiguration.videoTerminalConfiguration.baudR = baudR;
    computerConfiguration.videoTerminalConfiguration.baudT = baudT;
    computerConfiguration.videoTerminalConfiguration.vt100SetUpFeature = setupFeature;
    computerConfiguration.videoTerminalConfiguration.charactersPerRow = charactersPerRow;
    computerConfiguration.videoTerminalConfiguration.characterWidth = charWidth;
    
}

int GuiMain::getCpuType()
{
    cpuType_ = defaultCpuType_;
    if (cpuTypeXml_ != -1)
        cpuType_ = cpuTypeXml_;

    return cpuType_;
};

void GuiMain::setCpuType(wxString type)
{
    cpuTypeXml_ = -1;
    
    if (type.Len()<4)
        return;
    
    if (type == "system" || type == "system00")
        cpuTypeXml_ = SYSTEM00;
    else
        type = type.Right(4);
    
    if (type == "1801")
        cpuTypeXml_ = CPU1801;
    if (type == "1802")
        cpuTypeXml_ = CPU1802;
    if (type == "1804")
        cpuTypeXml_ = CPU1804;
    if (type == "1805")
        cpuTypeXml_ = CPU1805;
}

void GuiMain::onClearRam(wxCommandEvent&event)
{
    computerConfiguration.clearRam = event.IsChecked();
}

void GuiMain::onClearRtc(wxCommandEvent&event)
{
    computerConfiguration.clearRtc = event.IsChecked();
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

bool GuiMain::getDirectoryMode(int fdcType, int drive)
{
    return directoryMode_[fdcType][drive];
}

void GuiMain::setDirectoryMode(int fdcType, int drive, bool state)
{
    directoryMode_[fdcType][drive] = state;
}

wxString GuiMain::getFloppyDirSwitched(int fdcType, int drive)
{
    return floppyDirSwitched_[fdcType][drive];
}

wxString GuiMain::getFloppyDir(int fdcType, int drive)
{
    return floppyDir_[fdcType][drive];
}

wxString GuiMain::getFloppyFile(int fdcType, int drive)
{
    return floppy_[fdcType][drive];
}

void GuiMain::setUpdFloppyGui(int drive)
{
    wxString driveStr, driveStrMinus2;
    driveStr.Printf("%d", drive);
    driveStrMinus2.Printf("%d", drive-2);
    bool deActivateFdc = true;
        
    XRCCTRL(*this, "FDC"+driveStr+"_SwitchXml", wxBitmapButton)->Enable(false);
    if (computerConfiguration.fdcConfiguration.wd1770_defined)
    {
        if (drive < 2 && drive < computerConfiguration.fdcConfiguration.drives)
            deActivateFdc = false;
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->Enable( computerConfiguration.fdcConfiguration.wd1770_defined && drive < 2 && drive < computerConfiguration.fdcConfiguration.drives);
    }
    if (computerConfiguration.fdcConfiguration.wd1793_defined)
    {
        if (drive < computerConfiguration.fdcConfiguration.drives)
            deActivateFdc = false;
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->Enable(computerConfiguration.fdcConfiguration.wd1793_defined && drive < computerConfiguration.fdcConfiguration.drives);
    }
    if (computerConfiguration.upd765Configuration.defined)
    {
        if (drive < 4)
            deActivateFdc = false;
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->Enable(drive < 4);
        XRCCTRL(*this, "FDC"+driveStr+"_SwitchXml", wxBitmapButton)->Enable(true);
    }
    if (computerConfiguration.ideConfiguration.defined && drive < 2)
    {
        deActivateFdc = false;
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->Enable(true);
    }
    if (computerConfiguration.tu58Configuration.defined && drive > 1 && drive < 4)
    {
        deActivateFdc = false;
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->Enable(true);
    }

    if (directoryMode_[computerConfiguration.fdcType_][drive])
    {
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->SetLabel("HD "+driveStr);
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->SetToolTip("Browse for HD Directory "+driveStr);
        XRCCTRL(*this, "FDC"+driveStr+"_FileXml", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "Eject_FDC"+driveStr + "Xml", wxBitmapButton)->Enable(false);
        wxFileName selectedDirFile = wxFileName(floppyDirSwitched_[computerConfiguration.fdcType_][drive]);
        wxArrayString dirArray = selectedDirFile.GetDirs();
        wxString dirName = dirArray.Last();
        XRCCTRL(*this, "FDC"+driveStr+"_FileXml", wxTextCtrl)->SetValue(dirName);
    }
    else
    {
        if (computerConfiguration.fdcType_ == FDCTYPE_TU58_IDE)
        {
            if (drive < 2)
            {
                XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->SetLabel("IDE "+driveStr);
                XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->SetToolTip("Browse for IDE "+driveStr+" image file");
            }
            else
            {
                XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->SetLabel("TU58 "+driveStrMinus2);
                XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->SetToolTip("Browse for TU58 "+driveStrMinus2+" image file");
            }
        }
        else
        {
            XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->SetLabel("FDC "+driveStr);
            XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->SetToolTip("Browse for FDC "+driveStr+" image file");
        }
        XRCCTRL(*this, "FDC"+driveStr+"_ButtonXml", wxButton)->Enable(!deActivateFdc);
        XRCCTRL(*this, "FDC"+driveStr+"_FileXml", wxTextCtrl)->Enable(!deActivateFdc);
        XRCCTRL(*this, "Eject_FDC"+driveStr + "Xml", wxBitmapButton)->Enable(!deActivateFdc);
        XRCCTRL(*this, "FDC"+driveStr+"_FileXml", wxTextCtrl)->SetValue(floppy_[computerConfiguration.fdcType_][drive]);
    }
}

bool GuiMain::isDiagOn()
{
    if (computerConfiguration.diagnosticBoardConfiguration.defined)
        return computerConfiguration.diagnosticBoardConfiguration.active;
    else
        return false;
}

void GuiMain::onBatchConvertStart(wxCommandEvent&WXUNUSED(event))
{
    if (numberOfBatchFiles_ > 0)
    {
        if (p_Computer->isComputerRunning())
        {
            int answer = wxMessageBox("Emulator is running software\n"
                                        "Reset before loading NEW software?",
                                        "Emma 02", wxYES_NO);

            if (answer == wxYES)
                p_Computer->onReset();
            else
                return;
        }

        batchSaveWavFileDir_ = computerConfiguration.wavConfiguration[0].directory;
        batchSaveWavFile_ = computerConfiguration.wavConfiguration[0].fileName;
        p_Computer->setBatchFileNumber((int)numberOfBatchFiles_ - 1);
        batchConvertActive_ = true;
    }
}

void GuiMain::batchConvertStop()
{
    if (batchConvertActive_)
    {
        computerConfiguration.wavConfiguration[0].directory = batchSaveWavFileDir_;
        computerConfiguration.wavConfiguration[0].fileName = batchSaveWavFile_;
        batchConvertActive_ = false;
    }
}

void GuiMain::onBatchFileDialog(wxCommandEvent&WXUNUSED(event))
{
    wxString fileName, extension, errorList = "";
    int numberOfFiles = 0, numberOfErrorFiles = 0;
    wxFFile inputFile;
    char buffer[1];

    
    wxFileDialog openFileDialog(this,
                                "Select the ." + computerInfo.ploadExtension  + " files to convert",
                                computerConfiguration.batchConfiguration.directory,
                                computerConfiguration.batchConfiguration.fileName,
                                computerInfo.name + " files (*." + computerInfo.ploadExtension  + ")|*." + computerInfo.ploadExtension,
                                wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_MULTIPLE
                               );

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...

    batchPaths_.Clear();
    batchFiles_.Clear();
    openFileDialog.GetPaths(batchPaths_);
    openFileDialog.GetFilenames(batchFiles_);

    numberOfBatchFiles_ = batchPaths_.GetCount();
    
    wxFileName FullPath;

    for (int i=0; i<(int)numberOfBatchFiles_; i++)
    {
        FullPath = wxFileName(batchPaths_[i], wxPATH_NATIVE);

        batchPaths_[i] = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
        extension = FullPath.GetExt();

        if (i == 0)
        {
            computerConfiguration.batchConfiguration.directory = FullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
            computerConfiguration.batchConfiguration.fileName = FullPath.GetFullName();
        }
        
        if (extension != computerInfo.ploadExtension)
        {
            errorList += (batchFiles_[i] + "\n");
            numberOfErrorFiles++;
        }
        else
        {
            if (inputFile.Open(batchPaths_[i] + batchFiles_[i], _("rb")))
            {
                inputFile.Read(buffer, 1);
                inputFile.Close();
                
                if (buffer[0] == 1)
                {
                    errorList += (batchFiles_[i] + "\n");
                    batchFiles_[i] += "x";
                    numberOfErrorFiles++;
                }
                else
                    numberOfFiles++;
            }
            else
            {
                errorList += (batchFiles_[i] + "\n");
                batchFiles_[i] += "x";
                numberOfErrorFiles++;
            }
        }
    }

    if (errorList != "")
    {
        if (numberOfErrorFiles == 1)
            (void)wxMessageBox( "Following file will not be converted\n\n"+errorList,
                                    "Emma 02", wxICON_ERROR | wxOK );
        else
            (void)wxMessageBox( "Following files will not be converted\n\n"+errorList,
                                    "Emma 02", wxICON_ERROR | wxOK );
    }
    
    wxString numberStr;
    if (numberOfFiles == 1)
        numberStr = "1 File selected";
    else
        numberStr.Printf("%d Files selected", numberOfFiles);
    
    if (mode_.gui)
        XRCCTRL(*this, "BatchFileXml", wxStaticText)->SetLabel(numberStr);
}

void GuiMain::downloadWavFiles()
{
    int answer = wxMessageBox("Additional wav file required: " + computerConfiguration.wavConfiguration[0].fileName, "Download file?", wxICON_EXCLAMATION | wxYES_NO);
    if (answer == wxYES)
    {
        wxString fileName = computerConfiguration.wavConfiguration[0].directory + computerConfiguration.wavConfiguration[0].fileName;
        wxFileOutputStream html_stream(fileName);

        wxString url = "https://www.emma02.hobby-site.com/wave/" + computerConfiguration.wavConfiguration[0].fileName;
        url.Replace(" ", "%20");
        wxCurlHTTP http(url);
    
        if (!http.Get(html_stream))
            wxMessageBox( "Download failed", "Emma 02", wxICON_ERROR | wxOK );
    }
}
/*
void GuiMain::checkWavFileDownload()
{
    wxFFile inFile;
    size_t length = 8;

    char* buffer = new char[length];

    if (wxFile::Exists(computerConfiguration.wavConfiguration[0].directory + computerConfiguration.wavConfiguration[0].fileName))
    {
        if (inFile.Open(computerConfiguration.wavConfiguration[0].directory + computerConfiguration.wavConfiguration[0].fileName, _("rb")))
        {
            length = inFile.Read(buffer, length);
            if (length == 8)
            {
                if (buffer[0] == 'd' && buffer[1] == 'o' && buffer[2] == 'w' && buffer[3] == 'n' && buffer[4] == 'l' && buffer[5] == 'o' && buffer[6] == 'a' && buffer[7] == 'd')
                    downloadWavFiles();
            }
            inFile.Close();
        }
    }

}*/

void GuiMain::checkWavFileDownload(bool downloadIfMissing)
{
    if (computerConfiguration.wavConfiguration[0].fileName == "")
        return;
    
    wxFFile inFile;
    size_t length = 8;

    char* waveHeader = new char[length];
    wxString fileName = computerConfiguration.wavConfiguration[0].directory + computerConfiguration.wavConfiguration[0].fileName;
    
    bool fileExists = wxFile::Exists(fileName);
    
    if (fileExists)
    {
        downloadIfMissing = false;
        if (inFile.Open(fileName, _("rb")))
        {
            length = inFile.Read(waveHeader, length);
            inFile.Close();
        }
    }
    
    if (fileExists || downloadIfMissing)
    {
        if (length == 8 || length == 0 || downloadIfMissing)
        {
            if ((waveHeader[0] == 'd' && waveHeader[1] == 'o' && waveHeader[2] == 'w' && waveHeader[3] == 'n' && waveHeader[4] == 'l' && waveHeader[5] == 'o' && waveHeader[6] == 'a' && waveHeader[7] == 'd') || length == 0 || downloadIfMissing)
            {
                wxFileOutputStream html_stream(fileName);
                
                wxString url = "https://www.emma02.hobby-site.com/wave/" + computerConfiguration.wavConfiguration[0].fileName;
                url.Replace(" ", "%20");
#if !defined (_DEBUG)
                wxCurlDownloadDialog downloadDialog;
                downloadDialog.Create(url, &html_stream, "Download File?", computerConfiguration.wavConfiguration[0].fileName, wxNullBitmap, this, wxCTDS_CAN_START|wxCTDS_CAN_PAUSE|wxCTDS_CAN_ABORT|wxCTDS_REMAINING_TIME|wxCTDS_SIZE|wxCTDS_AUTO_CLOSE, 120);
                
                downloadDialog.RunModal();
#endif
            }
        }
    }
    delete[] waveHeader;
}

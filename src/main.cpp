
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

#define MAIN

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#if !wxUSE_COMBOCTRL
    #error "Please set wxUSE_COMBOCTRL to 1 and rebuild the library."
#endif

#include "wx/xrc/xmlres.h"
#include "wx/filesys.h"
//#include "wx/url.h"
#include "wxcurl_http.h"
#include "wx/dir.h"
#include "wx/fs_zip.h"
#include "wx/sysopt.h" 
#include "wx/stdpaths.h"
#include "wx/platinfo.h"
#include "wx/tglbtn.h"
#include "wx/fileconf.h"
#include "wx/cmdline.h"
#include "wx/sstream.h"
#include "wx/numformatter.h"

#if defined(__linux__)
#include <X11/Xlib.h>
#include "app_icon.xpm"
#endif

#include "main.h"
#include "ports.h"
#include "psave.h"
#include "functionkey.h"
#include "datadir.h"
#include "configuration.h"
#include "printer.h"
#include "about.h"
#include "guipopup.h"
#include "splash.h"
#include "definition.h"

#if defined(__WXMSW__) && (_DEBUG) //** only run vld in debug version
//#include <vld.h>
#endif

#define VU_RED 60
#define VU_MAX 100
#define VU_HI 10

PopupDialog *popupDialog_;

IMPLEMENT_DYNAMIC_CLASS(FloatEdit, wxTextCtrl) 
IMPLEMENT_DYNAMIC_CLASS(IntEdit, wxTextCtrl) 
IMPLEMENT_DYNAMIC_CLASS(CheckBoxListCtrl, wxListCtrl) 
IMPLEMENT_DYNAMIC_CLASS(SlotEdit, wxTextCtrl) 
IMPLEMENT_DYNAMIC_CLASS(HexEdit, wxTextCtrl) 
IMPLEMENT_DYNAMIC_CLASS(HexEditX, wxTextCtrl) 
IMPLEMENT_DYNAMIC_CLASS(MemEdit, wxTextCtrl) 

wxString cpuName[] =
{
    "",
    "SYSTEM00",
    "RCA CDP1801",
    "RCA CDP1802",
    "RCA CDP1804",
    "RCA CDP1805"
};

wxString guiSizers[] =
{
    "ComxBottomRight",
    "ComxBottomLeft",
    "ElfBottomRight",
    "ElfBottomLeft",
    "AssBottomRight",
    "DebugRight",
    "MemoryDumpBottomRight",
    "MemoryDumpBottomLeft",
    "MemoryDumpBottom",
    "End"
};

wxString objectClass[]=
{
    "wxStaticText",
    "sizeritem",
    "wxButton",
    "wxStaticBoxSizer",
    "undefined"
};

enum
{
    WXSTATICTEXT,
    SIZERITEM,
    WXBUTTON,
    WXSTATICBOXSIZER,
    UNDEFINED_OBJECT_CLASS
};

wxString markupList[]=
{
    "object",
    "size",
    "label",
    "font",
    "sysfont",
    "family",
    "undefined"
};

enum
{
    MU_OBJECT,
    MU_SIZE,
    MU_LABEL,
    MU_FONT,
    MU_SYSFONT,
    MU_FAMILY,
    UNDEFINED_MARKUP
};

FloatEdit::FloatEdit()
: wxTextCtrl() 
{ 
    Connect(wxEVT_CHAR, wxKeyEventHandler( FloatEdit::OnChar ), NULL, this ); 
    wxString tempText;
    tempText.Printf("%1.1f", 0.0);
    comma_ = tempText.Mid(1,1);
} 

FloatEdit::FloatEdit(wxWindow *parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size) 
: wxTextCtrl(parent, id, value, pos, size) 
{ 
    Connect(wxEVT_CHAR, wxKeyEventHandler( FloatEdit::OnChar ), NULL, this ); 
    wxString tempText;
    tempText.Printf("%1.1f", 0.0);
    comma_ = tempText.Mid(1,1);
} 

void FloatEdit::OnChar(wxKeyEvent& event) 
{ 
    const int keyCode = event.GetKeyCode(); 

    // Accept a number 
    if (keyCode >= '0' && keyCode <= '9') 
        event.Skip(); 

    if (keyCode == comma_) 
        event.Skip(); 

    // Accept some other keys 
    switch (keyCode) 
    { 
        case WXK_BACK: 
        case WXK_LEFT: 
        case WXK_RIGHT: 
        case WXK_TAB: 
        case WXK_DELETE: 
        case WXK_END: 
        case WXK_HOME: 
        case WXK_RETURN:
            event.Skip(); 
        break; 
    } 
}

IntEdit::IntEdit() 
: wxTextCtrl() 
{ 
    Connect(wxEVT_CHAR, wxKeyEventHandler( IntEdit::OnChar ), NULL, this ); 
} 

void IntEdit::OnChar(wxKeyEvent& event) 
{ 
    const int keyCode = event.GetKeyCode(); 

    // Accept a number 
    if (keyCode >= '0' && keyCode <= '9')
    {
        event.Skip(); 
//        return;
    }

    // Accept some other keys 
    switch (keyCode) 
    { 
        case WXK_BACK: 
        case WXK_LEFT: 
        case WXK_RIGHT: 
        case WXK_TAB: 
        case WXK_DELETE: 
        case WXK_END: 
        case WXK_HOME: 
        case WXK_RETURN:
            event.Skip(); 
        break; 

        default:
//            wxNotificationMessage notify("test", "error", this); 
//            notify.SetFlags( wxICON_ERROR ); 
//            notify.Show(); 
        break; 
    } 
}

CheckBoxListCtrl::CheckBoxListCtrl() 
: wxListCtrl() 
{ 
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler( CheckBoxListCtrl::onMouseRelease ), NULL, this ); 
} 

void CheckBoxListCtrl::onMouseRelease(wxMouseEvent& event) 
{ 
    wxString idReference = wxWindow::FindWindowById(event.GetId())->GetName();

    int flags;

    int item = (int)HitTest(event.GetPosition(), flags);
    if (flags == wxLIST_HITTEST_ONITEMICON)
    {
        if (idReference == "BreakPointWindow")
            p_Main->switchBreakPoint(item);
        if (idReference == "Chip8BreakPointWindow")
            p_Main->switchChip8BreakPoint(item);
        if (idReference == "TrapWindow")
            p_Main->switchTrap(item);
        if (idReference == "TregWindow")
            p_Main->switchTreg(item);

    }
    else
        event.Skip();
}

SlotEdit::SlotEdit() 
: wxTextCtrl() 
{ 
    Connect(wxEVT_CHAR, wxKeyEventHandler( SlotEdit::OnChar ), NULL, this ); 
    min_ = 0;
    max_ = 9;
    number_ = -1;
} 

void SlotEdit::OnChar(wxKeyEvent& event) 
{ 
    const int keyCode = event.GetKeyCode(); 

    // Accept a number 
    if (keyCode >= ('0' + min_) && keyCode <= ('0' + max_)) 
        event.Skip(); 

    // Accept some other keys 
    switch (keyCode) 
    { 
        case WXK_BACK: 
        case WXK_LEFT: 
        case WXK_RIGHT: 
        case WXK_TAB: 
        case WXK_DELETE: 
        case WXK_END: 
        case WXK_HOME: 
        case WXK_RETURN:
            event.Skip(); 
        break; 
    } 
}

void SlotEdit::setRange(int min, int max)
{
    min_ = min;
    max_ = max;
}

void SlotEdit::changeNumber(int number)
{
    wxString numberString;

    if (number == number_) return;
    number_ = number;

    numberString.Printf("%X", number);
    ChangeValue(numberString);
}

void SlotEdit::saveNumber(int number)
{
    number_ = number;
}

HexEdit::HexEdit() 
: wxTextCtrl() 
{ 
    Connect(wxEVT_CHAR, wxKeyEventHandler( HexEdit::OnChar ), NULL, this ); 
    number_ = -1;
    min_ = 0;
} 

void HexEdit::OnChar(wxKeyEvent& event) 
{ 
    const int keyCode = event.GetKeyCode(); 

    // Accept a number 
    if (keyCode >= ('0'+min_) && keyCode <= '9') 
        event.Skip(); 

    if (keyCode >= 'a' && keyCode <= 'f')
        event.Skip(); 

    if (keyCode >= 'A' && keyCode <= 'F') 
        event.Skip(); 

    // Accept some other keys 
    switch (keyCode) 
    { 
        case WXK_BACK: 
        case WXK_LEFT: 
        case WXK_RIGHT: 
        case WXK_TAB: 
        case WXK_DELETE: 
        case WXK_END: 
        case WXK_HOME: 
        case WXK_RETURN:
            event.Skip(); 
        break; 
    } 
}

void HexEdit::setStart(int min)
{
    min_ = min;
}

void HexEdit::changeNumber(int number)
{
    wxString numberString;

    if (number == number_) return;
    number_ = number;

    numberString.Printf("%X", number);
    ChangeValue(numberString);
}

void HexEdit::saveNumber(int number)
{
    number_ = number;
}

HexEditX::HexEditX() 
: wxTextCtrl() 
{ 
    Connect(wxEVT_CHAR, wxKeyEventHandler( HexEditX::OnChar ), NULL, this ); 
} 

void HexEditX::OnChar(wxKeyEvent& event) 
{ 
    const int keyCode = event.GetKeyCode(); 

    // Accept a number 
    if (keyCode >= '0' && keyCode <= '9') 
        event.Skip(); 

    if (keyCode >= 'a' && keyCode <= 'f')
        event.Skip(); 

    if (keyCode >= 'A' && keyCode <= 'F') 
        event.Skip(); 

    if (keyCode == 'X' || keyCode == 'x') 
        event.Skip(); 

    // Accept some other keys 
    switch (keyCode) 
    { 
        case WXK_BACK: 
        case WXK_LEFT: 
        case WXK_RIGHT: 
        case WXK_TAB: 
        case WXK_DELETE: 
        case WXK_END: 
        case WXK_HOME: 
        case WXK_RETURN:
            event.Skip(); 
        break; 
    } 
}

MemEdit::MemEdit() 
: wxTextCtrl() 
{ 
    Connect(wxEVT_CHAR, wxKeyEventHandler( MemEdit::OnChar ), NULL, this ); 
    number_ = -1;
} 

void MemEdit::OnChar(wxKeyEvent& event) 
{ 
    if (p_Main->getChoiceSelection("DebugMemType") == 1)
        event.Skip(); 

    const int keyCode = event.GetKeyCode(); 

    // Accept a number 
    if (keyCode >= '0' && keyCode <= '9') 
        event.Skip(); 

    if (keyCode >= 'a' && keyCode <= 'f')
        event.Skip(); 

    if (keyCode >= 'A' && keyCode <= 'F') 
        event.Skip(); 

    // Accept some other keys 
    switch (keyCode) 
    { 
        case WXK_BACK: 
        case WXK_LEFT: 
        case WXK_RIGHT: 
        case WXK_TAB: 
        case WXK_DELETE: 
        case WXK_END: 
        case WXK_HOME: 
        case WXK_RETURN:
            event.Skip(); 
        break; 
    } 
}

void MemEdit::changeNumber1X(int number)
{
    wxString numberString;

    if (number == number_) return;
    number_ = number;

    numberString.Printf("%01X", number);
    ChangeValue(numberString);
}

void MemEdit::changeNumber2X(int number)
{
    wxString numberString;

    if (number == number_) return;
    number_ = number;

    numberString.Printf("%02X", number);
    ChangeValue(numberString);
}

void MemEdit::saveNumber(int number)
{
    number_ = number;
}

DEFINE_EVENT_TYPE(OPEN_PRINTER_WINDOW)
DEFINE_EVENT_TYPE(wxEVT_ERROR_MSG)
DEFINE_EVENT_TYPE(KILL_COMPUTER)
//DEFINE_EVENT_TYPE(GUI_MSG) // This is for wx 2.8.12 but doesn't actually work

BEGIN_EVENT_TABLE(Main, DebugWindow)

    EVT_CLOSE(Main::onClose)

//#if defined(__linux__) || defined(__WXMAC__)
    EVT_SIZE(Main::windowSizeChanged)
//#endif
    EVT_MENU(wxID_EXIT, Main::onQuit)
    EVT_MENU(wxID_ABOUT, Main::onAbout)
    EVT_MENU(XRCID("MI_DataDir"), Main::onDataDir)
    EVT_MENU(XRCID("MI_Home"), Main::onHome)
    EVT_MENU(XRCID("MI_Home_SB"), Main::onHomeSb)
    EVT_MENU(XRCID("MI_Home_SBHS"), Main::onHomeSbHs)
    EVT_MENU(XRCID("MI_UpdateCheck"), Main::onUpdateCheck)
    EVT_MENU(XRCID("MI_UpdateEmma"), Main::onUpdateEmma)
    EVT_MENU(wxID_HELP, Main::onHelp)
    EVT_MENU(XRCID(GUISAVECONFIG), Main::onSaveConfig)
    EVT_MENU(XRCID(GUISAVECOMPUTERCONFIG), Main::onConfiguration)
    EVT_MENU(XRCID(GUISAVEONEXIT), Main::onSaveOnExit)
    EVT_MENU(XRCID(GUIDEFAULTWINDOWPOS), Main::onDefaultWindowPosition)
    EVT_MENU(XRCID(GUIDEFAULTGUISIZE), Main::onDefaultGuiSize)
    EVT_MENU(XRCID("MI_ReInstallConfig"), Main::onReInstallConfig)
    EVT_MENU(XRCID("MI_ReInstallData"), Main::onReInstallData)
    EVT_MENU(XRCID("MI_FixedWindowPosition"), Main::onFixedWindowPosition)
    EVT_MENU(XRCID("MI_NumPad"), Main::onUseNumPad)
    EVT_MENU(XRCID("MI_FunctionKeys"), Main::onFunctionKeys)
    EVT_MENU(XRCID("MI_ActivateMain"), Main::onActivateMainWindow)
    EVT_MENU(XRCID("MI_FullScreen"), Main::onFullScreenMenu)
    EVT_MENU(XRCID(GUIDEFAULT), Main::onDefaultSettings)
    EVT_MENU(XRCID("SYSTEM00"), Main::onSystem00)
    EVT_MENU(XRCID("CDP1801"), Main::on1801)
    EVT_MENU(XRCID("CDP1802"), Main::on1802)
    EVT_MENU(XRCID("CDP1804"), Main::on1804)
    EVT_MENU(XRCID("CDP1805"), Main::on1805)
    EVT_MENU(XRCID("StartupRegistersZeroed"), Main::onCpuStartupRegistersZeroed)
    EVT_MENU(XRCID("StartupRegistersRandom"), Main::onCpuStartupRegistersRandom)
    EVT_MENU(XRCID("StartupRamZeroed"), Main::onCpuStartupRamZeroed)
    EVT_MENU(XRCID("StartupRamRandom"), Main::onCpuStartupRamRandom)
    EVT_MENU(XRCID("StartupRamDynamic"), Main::onCpuStartupRamDynamic)
    EVT_MENU(XRCID("StartupVideoRamZeroed"), Main::onCpuStartupVideoRamZeroed)
    EVT_MENU(XRCID("StartupVideoRamRandom"), Main::onCpuStartupVideoRamRandom)
    EVT_MENU(XRCID("StartupVideoRamDynamic"), Main::onCpuStartupVideoRamDynamic)
    EVT_MENU(XRCID("KeyboardDutch"), Main::onKeyboardDutch)
    EVT_MENU(XRCID("KeyboardGerman"), Main::onKeyboardGerman)
    EVT_MENU(XRCID("KeyboardFrench"), Main::onKeyboardFrench)
    EVT_MENU(XRCID("KeyboardSwedish"), Main::onKeyboardSwedish)
    EVT_MENU(XRCID("KeyboardUs"), Main::onKeyboardUs)
    EVT_MENU(XRCID("KeyboardUserDefined"), Main::onKeyboardUserDefined)
    EVT_MENU(XRCID("Flat"), Main::onFlat)
    EVT_MENU(XRCID("Crisp"), Main::onCrisp)
    EVT_MENU(XRCID("Default"), Main::onDefault)
    EVT_MENU(XRCID("TV Speaker"), Main::onTvSpeaker)
    EVT_MENU(XRCID("Handheld"), Main::onHandheld)
    EVT_MENU(XRCID("MI_1801"), Main::on1801)
    EVT_MENU(XRCID("MI_1802"), Main::on1802)
    EVT_MENU(XRCID("MI_1804"), Main::on1804)
    EVT_MENU(XRCID("MI_1805"), Main::on1805)
    EVT_MENU(XRCID("MI_FontSize11"), Main::onFontSize11)
    EVT_MENU(XRCID("MI_FontSize14"), Main::onFontSize14)

    EVT_CHOICEBOOK_PAGE_CHANGED(XRCID("StudioChoiceBook"), Main::onStudioChoiceBook)
    EVT_CHOICEBOOK_PAGE_CHANGED(XRCID("TelmacChoiceBook"), Main::onTelmacChoiceBook)
    EVT_CHOICEBOOK_PAGE_CHANGED(XRCID("ElfChoiceBook"), Main::onElfChoiceBook)
    EVT_CHOICEBOOK_PAGE_CHANGED(XRCID("RcaChoiceBook"), Main::onRcaChoiceBook)
    EVT_NOTEBOOK_PAGE_CHANGED(XRCID("DebuggerChoiceBook"), Main::onDebuggerChoiceBook)
    EVT_TIMER(902, Main::vuTimeout)
//    EVT_TIMER(903, Main::cpuTimeout)
    EVT_TIMER(905, Main::updateCheckTimeout)
    EVT_TIMER(906, Main::traceTimeout)
    EVT_TIMER(907, Main::debounceTimeout)
    EVT_TIMER(908, Main::guiSizeTimeout)
    EVT_TIMER(909, Main::guiRedrawBarTimeOut)
    EVT_TIMER(910, Main::directAssTimeout)

    EVT_KEY_DOWN(Main::onKeyDown)
    EVT_KEY_UP(Main::onKeyUp)
    EVT_MOUSEWHEEL(Main::onWheel)

    EVT_ERROR_MSG(wxID_ANY, Main::errorMessageEvent)

    EVT_GUI_MSG(SET_LOCATION, Main::setLocationEvent)
    EVT_GUI_MSG(SET_SW_NAME, Main::setSwNameEvent)
    EVT_GUI_MSG(SET_TAPE_STATE, Main::setTapeStateEvent)
    EVT_GUI_MSG(SET_TEXT_VALUE, Main::setTextValueEvent)
    EVT_GUI_MSG(SET_STATIC_TEXT_VALUE, Main::setStaticTextValueEvent)
    EVT_GUI_MSG(SET_BUTTON_LABEL, Main::setButtonLabelEvent)
    EVT_GUI_MSG(SET_CHECK_BOX, Main::setCheckBoxEvent)
    EVT_GUI_MSG(PRINT_DEFAULT, Main::printDefaultEvent)
    EVT_GUI_MSG(PRINT_PARALLEL, Main::printParallelEvent)
    EVT_GUI_MSG(PRINT_PARALLEL_FINISHED, Main::printParallelFinishedEvent)
    EVT_GUI_MSG(PRINT_THERMAL, Main::printThermalEvent)
    EVT_GUI_MSG(PRINT_THERMAL_FINISHED, Main::printThermalFinishedEvent)
    EVT_GUI_MSG(PRINT_SERIAL, Main::printSerialEvent)
    EVT_GUI_MSG(PRINT_SERIAL_FINISHED, Main::printSerialFinishedEvent)
    EVT_GUI_MSG(PRINT_PECOM, Main::printPecomEvent)
    EVT_GUI_MSG(SHOW_MESSAGE_BOX, Main::ShowMessageBoxEvent)
    EVT_GUI_MSG(SHOW_ADDRESS_POPUP, Main::ShowAddressPopupEvent)
    EVT_GUI_MSG(SHOW_FILE_SELECTOR, Main::showFileSelectorEvent)
    EVT_GUI_MSG(SET_FM_GUI, Main::setFandMBasicGuiEvent)
    EVT_GUI_MSG(SET_SAVE_START, Main::setSaveStartEvent)
    EVT_GUI_MSG(SET_SAVE_END, Main::setSaveEndEvent) 
    EVT_GUI_MSG(ENABLE_MEM_ACCESS, Main::enableMemAccesEvent) 
    EVT_GUI_MSG(SET_VIDEO_FULLSCREEN, Main::setVideoFullScreenEvent)
    EVT_GUI_MSG(SET_VT_FULLSCREEN, Main::setVtFullScreenEvent)
    EVT_GUI_MSG(CHANGE_NOTEBOOK, Main::setChangeNoteBookEvent)
    EVT_GUI_MSG(DISABLE_CONTROLS, Main::setDisableControlsEvent)
    EVT_GUI_MSG(UPDATE_TITLE, Main::setUpdateTitle)
    EVT_GUI_MSG(SHOW_MESSAGE, Main::showMessageEvent)
    EVT_GUI_MSG(SHOW_TEXT_MESSAGE, Main::showTextMessageEvent)
    EVT_GUI_MSG(DEBOUNCE_TIMER, Main::setDebounceTimer)
    EVT_GUI_MSG(ZOOM_CHANGE, Main::setZoomChange)
    EVT_GUI_MSG(ZOOMVT_CHANGE, Main::setZoomVtChange)
    EVT_GUI_MSG(GET_CLIENT_SIZE, Main::GetClientSizeEvent)
    EVT_GUI_MSG(SET_CLIENT_SIZE, Main::SetClientSizeEvent)
    EVT_GUI_MSG(REFRESH_VIDEO, Main::refreshVideoEvent)
    EVT_GUI_MSG(REFRESH_PANEL, Main::refreshPanelEvent)
    EVT_GUI_MSG(EVENT_ZOOM, Main::SetZoomEvent)
    EVT_GUI_MSG(SET_CONVERT_STATE, Main::setConvertStateEvent)
    EVT_GUI_MSG(SET_COMXLED, Main::setUpdateComxLedStatus)
    EVT_GUI_MSG(SET_DIAGLED, Main::setUpdateDiagLedStatus)
    EVT_GUI_MSG(ENABLE_CLOCK, Main::setEnableClockEvent)
    EVT_GUI_MSG(PAUSE_STATE, Main::setPauseStateEvent)
    EVT_GUI_MSG(CHANGE_HW_TAPE_STATE, Main::setHwTapeStateEvent)

    EVT_SYS_COLOUR_CHANGED(Main::sysColourChangeEvent)

    EVT_COMMAND(wxID_ANY, KILL_COMPUTER, Main::killComputer)

END_EVENT_TABLE()

#if defined (__linux__)
IMPLEMENT_APP_NO_MAIN(Emu1802)
int main(int argc, char *argv[])
{
    if (!XInitThreads())
        return false;

    wxDISABLE_DEBUG_SUPPORT();
//    return wxEntry(argc, argv);

//  code as suggested initially by Mark but replaced by two lines above as on Elf2K exit with assert
    wxEntryStart( argc, argv );
    wxTheApp->OnInit();
    wxTheApp->OnRun();
    wxTheApp->OnExit();
    wxEntryCleanup();
    return 0;
}
#else
IMPLEMENT_APP(Emu1802)
#endif

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MyFrame();

    void onClose(wxCloseEvent&event );
private:
};

bool Emu1802::OnInit()
{
    if (!wxApp::OnInit())
        return false;
    
    wxSystemOptions::SetOption("msw.window.no-clip-children", 1);
    
    int offsetX = 0, offsetY = 0;

    wxInitAllImageHandlers();
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();

    wxString fontSizeString = configPointer->Read("/Main/FontSizeString", "11");

    wxString xrcFile;
#if defined(__linux__)
    offsetX = 36;
    xrcFile = applicationDirectory_ + "main_" + fontSizeString + ".xrc";
#elif (__WXMAC__)
#ifdef DEBUG
    createXml();
#endif
    offsetX = 30;
    offsetY = 30;
    xrcFile = applicationDirectory_ + "main_mac_" + fontSizeString + ".xrc";
#else
    offsetX = 0;
    xrcFile = applicationDirectory_ + "main_" + fontSizeString + ".xrc";
#endif

#if wxCHECK_VERSION(2, 9, 0)
    if (!wxXmlResource::Get()->LoadFile(xrcFile))
#else
    if (!wxXmlResource::Get()->Load(xrcFile))
#endif
    {
        (void)wxMessageBox( "Error loading " + xrcFile + "\n",
                            "Emma 02", wxICON_ERROR | wxOK );
        return false;
    }

    PrintDataPointer = new wxPrintData;
    PrintDataPointer->SetPaperId(wxPAPER_A4);

    p_PageSetupData = new wxPageSetupDialogData;
    (*p_PageSetupData) = *PrintDataPointer;
    p_PageSetupData->SetMarginTopLeft(wxPoint(9, 9));
    p_PageSetupData->SetMarginBottomRight(wxPoint(9, 9));

    int mainWindowX = (int)configPointer->Read("/Main/Window_Position_X", 30 + offsetX);
    int mainWindowY = (int)configPointer->Read("/Main/Window_Position_Y", 30 + offsetY);
    
    p_Main = new Main("Emma 02", wxPoint(mainWindowX, mainWindowY), wxSize(-1, -1), mode_, dataDir_, iniDirectory_);

    p_Main->Show(mode_.gui);

    if (startComputer_ != -1)
        p_Main->onStart(startComputer_);
    if (mode_.run || mode_.load)
        p_Main->runSoftware(!mode_.run);
    return true;
}

int Emu1802::OnExit()
{
    delete wxConfigBase::Set((wxConfigBase *) NULL);
    delete PrintDataPointer;
    delete p_PageSetupData;
    return 0;
}

int Emu1802::OnRun()
{
    int exitcode = wxApp::OnRun();
    //wxTheClipboard->Flush();
    //if (exitcode!=0)
    return exitcode;
}

static const wxCmdLineEntryDesc cmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, "h", "help", "show this help message", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_SWITCH, "p", "portable", "run in portable mode" },                                    
    { wxCMD_LINE_SWITCH, "v", "verbose", "verbose output"},                                            // only valid in combination with -c
    { wxCMD_LINE_SWITCH, "f", "fullscreen", "full screen mode"},                                    // only valid in combination with -c
    { wxCMD_LINE_SWITCH, "u", "skipupdate", "skip update check"},                                    
    { wxCMD_LINE_SWITCH, "w", "window", "non fixed window positions"},                                    
    { wxCMD_LINE_OPTION, "c", "computer", "start emulator without gui and for specified computer" },// Switch off GUI
    { wxCMD_LINE_OPTION, "s", "software", "load specified software on start" },                        // only valid in combination with -c
    { wxCMD_LINE_OPTION, "r", "run", "run specified software on start" },                            // only valid in combination with -c
    { wxCMD_LINE_OPTION, "ch", "chip8", "load specified chip8 software on start" },                    // only valid in combination with -c

    { wxCMD_LINE_NONE }
};

void Emu1802::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetDesc (cmdLineDesc);
    // must refuse '/' as parameter starter or cannot use "/path" style paths
    parser.SetSwitchChars (wxT("-"));
}
 
bool Emu1802::OnCmdLineParsed(wxCmdLineParser& parser)
{
    mode_.window_position_fixed = !parser.Found("w");
     mode_.portable = parser.Found("p");
    mode_.verbose = parser.Found("v");
    mode_.full_screen = parser.Found("f");
    mode_.update_check = !parser.Found("u");

    SetVendorName("Marcel van Tongeren");
#if defined(__linux__)
    wxString appName = "emma_02";
#else
    wxString appName = "Emma 02";
#endif
    SetAppName(appName);

    wxString configDirectory = wxStandardPaths::Get().GetUserConfigDir();

#if defined(__linux__) || defined (__WXMSW__)
    workingDir_ = wxGetCwd();
#endif
#if defined (__WXMAC__)
    if (mode_.portable)
        workingDir_ = wxGetCwd();
    else
        workingDir_ = wxStandardPaths::Get().GetDataDir();
#endif

    wxFileName applicationFile = wxFileName(workingDir_, "", wxPATH_NATIVE);
    pathSeparator_ = applicationFile.GetPathSeparator(wxPATH_NATIVE);
    
#if defined(__linux__)
    iniDirectory_ = configDirectory + pathSeparator_ + ".emma_02";
    if (wxFile::Exists(iniDirectory_))
        wxRemoveFile(iniDirectory_);
    iniDirectory_ = iniDirectory_ + pathSeparator_;
#endif
#if defined (__WXMAC__)
    iniDirectory_ = configDirectory + pathSeparator_ + "Emma 02" + pathSeparator_;
#endif
#if defined (__WXMSW__)
    iniDirectory_ = configDirectory + pathSeparator_ + "Emma 02 Config" + pathSeparator_;
#endif

    if (!mode_.portable)
    {
        if (!wxDir::Exists(iniDirectory_))
            wxDir::Make(iniDirectory_);
    }
    else
        iniDirectory_ = applicationFile.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

    if (!wxFile::Exists(iniDirectory_ + "emma_02.ini"))
    {
        wxConfigBase *regPointer;
        regPointer = wxConfigBase::Get();
        regPointer->SetRecordDefaults();
        dataDir_ = regPointer->Read("/DataDir", "");
        if (wxFile::Exists(dataDir_ + "emma_02.ini"))
        {
            if (wxCopyFile(dataDir_ + "emma_02.ini", iniDirectory_ + "emma_02.ini"))
                wxRemoveFile (dataDir_ + "emma_02.ini");
        }
    }
            
    wxFileConfig *pConfig = new wxFileConfig(appName, "Marcel van Tongeren", iniDirectory_ + "emma_02.ini");
    wxConfigBase::Set(pConfig);
    configPointer = wxConfigBase::Get();
    
#if defined(__linux__)
    dataDir_ = configPointer->Read("/DataDir", configDirectory + pathSeparator_ + "emma_02_data" + pathSeparator_);
    applicationDirectory_ = applicationFile.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    if (!wxFile::Exists(applicationDirectory_ + "main_11.xrc"))
    {
        applicationDirectory_ = wxStandardPaths::Get().GetExecutablePath();
        applicationDirectory_ = applicationDirectory_.Left(applicationDirectory_.Len()-11);
        applicationDirectory_ = applicationDirectory_ + "share" + pathSeparator_ + "emma_02" + pathSeparator_;
    }
#endif
#if defined (__WXMAC__)
    dataDir_ = wxStandardPaths::Get().GetDocumentsDir();
    if (dataDir_.Right(9) == "Documents")
        dataDir_ = dataDir_.Left(dataDir_.Len()-9) + "Emma 02" + pathSeparator_;
    else
        dataDir_ = dataDir_ + pathSeparator_;
    dataDir_ = configPointer->Read("/DataDir", dataDir_);
 
    if (mode_.portable)
        applicationDirectory_ = applicationFile.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    else
        applicationDirectory_ = wxStandardPaths::Get().GetResourcesDir() + pathSeparator_;
#endif
#if defined (__WXMSW__)
    dataDir_ = configPointer->Read("/DataDir", wxStandardPaths::Get().GetUserDataDir() + pathSeparator_);
    applicationDirectory_ = applicationFile.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
#endif

    configPointer->Read("/DataDirRelative", &dataDirRelative_, false);

    if (dataDirRelative_)
    {
        if (dataDir_.Right(1) == pathSeparator_)
            dataDir_ = applicationDirectory_ + dataDir_;
        else
            dataDir_ = applicationDirectory_ + dataDir_ + pathSeparator_;
    }

    if (dataDir_.Find(applicationDirectory_) != wxNOT_FOUND)
    {
        configPointer->Write("/DataDirRelative", true);

        wxString tempDir = dataDir_;
        tempDir.Replace(applicationDirectory_, "");
        configPointer->Write("/DataDir", tempDir);
    }
    else
    {
        configPointer->Write("/DataDirRelative", false);
        configPointer->Write("/DataDir", dataDir_);
    }

    if (mode_.portable)
    {
        dataDir_ = applicationDirectory_ + "data" + pathSeparator_;

        if (configPointer->Read("/Dir/Main/Debug", dataDir_) != dataDir_)
            configPointer->DeleteGroup("Dir");
    }

    startComputer_ = -1;
    mode_.gui = true;
    mode_.run = false;
    mode_.load = false;
    wxString software = "";
    wxString computer;
 
    if (parser.Found("c", &computer))
    {
#if wxCHECK_VERSION(2, 9, 0)
        computer = computer.Capitalize();
        switch(computer[0].GetValue())
#else
        computer = computer.MakeUpper();
        switch(computer[0])
#endif
        {
            case 'C':
                if (computer == "Comx")
                {
                    startComputer_ = COMX;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        mode_.run = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on COMX-35 emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Cosmicos")
                {
                    startComputer_ = COSMICOS;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on Cosmicos emulator");
                        return false;
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Cosmicosemulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Cosmac")
                {
                    startComputer_ = ELF;
                    computer = "Elf";
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        mode_.run = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Cosmac Elf emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Cidelsa")
                {
                    startComputer_ = CIDELSA;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "Main_Rom_File", software);
                    if (parser.Found("r", &software))
                        getSoftware(computer, "Main_Rom_File", software);
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Cidelsa emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "CoinArcade")
                {
                    startComputer_ = COINARCADE;
                    computer = "CoinArcade";
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "St2_File", software);
                    if (parser.Found("r", &software))
                        getSoftware(computer, "St2_File", software);
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on RCA Video Coin Arcade emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Cdp18s020" || computer == "CDP18S020")
                {
                    startComputer_ = CDP18S020;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "Ram_Software", software);
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on CDP18S020 emulator");
                        return false;
                    }
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on CDP18S020 emulator");
                        return false;
                    }
                    return true;
                }
                wxMessageOutput::Get()->Printf("Incorrect computer name specified");
                return false;
            break;

            case 'E':
                if (computer == "Elf2k")
                {
                    startComputer_ = ELF2K;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on Elf 2000 emulator");
                        return false;
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Elf 2000 emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Eti" || computer == "Eti660")
                {
                    startComputer_ = ETI;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("ch", &software))
                        getSoftware(computer, "Chip_8_Software", software);
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on HUG1802/ETI-660 emulator");
                        return false;
                    }
                    return true;
                }
                wxMessageOutput::Get()->Printf("Incorrect computer name specified");
                return false;
            break;

            case 'F':
                if (computer == "Fred1")
                {
                    startComputer_ = FRED1;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on FRED 1 emulator");
                        return false;
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on FRED 1 emulator");
                        return false;
                    }
                    return true;
                }
            
                if (computer == "Fred15" || computer == "Fred1.5")
                {
                    startComputer_ = FRED1_5;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on FRED 1.5 emulator");
                        return false;
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on FRED 1.5 emulator");
                        return false;
                    }
                    return true;
                }
                wxMessageOutput::Get()->Printf("Incorrect computer name specified");
                return false;
            break;
                
            case 'I':
                if (computer == "Infinite")
                {
                    startComputer_ = UC1800;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on Infinite UC1800 emulator");
                        return false;
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Infinite UC1800 emulator");
                        return false;
                    }
                    return true;
                }
            break;

            case 'M':
                if (computer == "Membership")
                {
                    startComputer_ = MEMBER;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on Membership Card emulator");
                        return false;
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Membership Card emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Microboard" || computer == "Micro")
                {
                    startComputer_ = MICROBOARD;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on Microboard emulator");
                        return false;
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Microboard emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Microtutor")
                {
                    startComputer_ = MICROTUTOR;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on Microtutor emulator");
                        return false;
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Microtutor emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Microtutor2" || computer == "MicrotutorII")
                {
                    startComputer_ = MICROTUTOR2;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on Microtutor II emulator");
                        return false;
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Microtutor II emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Ms2000" || computer == "MS2000")
                {
                    startComputer_ = MS2000;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on MS2000 emulator");
                        return false;
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on MS2000 emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Mcds" || computer == "MCDS")
                {
                    startComputer_ = MCDS;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on MCDS emulator");
                        return false;
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on MCDS emulator");
                        return false;
                    }
                    return true;
                }
                wxMessageOutput::Get()->Printf("Incorrect computer name specified");
                return false;
            break;

            case 'N':
                if (computer == "Netronics")
                {
                    startComputer_ = ELFII;
                    computer = "ElfII";
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        mode_.run = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Netronics Elf II emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Nano")
                {
                    startComputer_ = NANO;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "Ram_Software", software);
                    if (parser.Found("ch", &software))
                        getSoftware(computer, "Chip_8_Software", software);
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on Oscom Nano emulator");
                        return false;
                    }
                    return true;
                }
                wxMessageOutput::Get()->Printf("Incorrect computer name specified");
                return false;
            break;

            case 'P':
                if (computer == "Pecom")
                {
                    startComputer_ = PECOM;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        mode_.run = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Pecom 64 emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Pico")
                {
                    startComputer_ = PICO;
                    computer = "Pico";
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        mode_.run = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Pico/Elf V2 emulator");
                        return false;
                    }
                    return true;
                }
            break;

            case 'Q':
                if (computer == "Quest")
                {
                    startComputer_ = SUPERELF;
                    computer = "SuperElf";
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        mode_.run = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Quest Super Elf emulator");
                        return false;
                    }
                    return true;
                }
                wxMessageOutput::Get()->Printf("Incorrect computer name specified");
                return false;
            break;

            case 'S':
                if (computer == "Studio" || computer == "Studio2" || computer == "Studioii")
                {
                    startComputer_ = STUDIO;
                    computer = "Studio2";
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "St2_File", software);
                    if (parser.Found("r", &software))
                        getSoftware(computer, "St2_File", software);
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on RCA Studio II emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Studioiii" || computer == "Studio3")
                {
                    startComputer_ = VICTORY;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "St2_File", software);
                    if (parser.Found("r", &software))
                        getSoftware(computer, "St2_File", software);
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on RCA Studio III emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Studioiv" || computer == "Studio4")
                {
                    startComputer_ = STUDIOIV;
                    computer = "StudioIV";
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "St2_File", software);
                    if (parser.Found("r", &software))
                        getSoftware(computer, "St2_File", software);
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on RCA Studio IV emulator");
                        return false;
                    }
                    return true;
                }
                wxMessageOutput::Get()->Printf("Incorrect computer name specified");
                return false;
            break;

            case 'T':
                if (computer == "Tmc600" || computer == "Telmac600")
                {
                    startComputer_ = TMC600;
                    computer = "TMC600";
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        mode_.run = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on TMC-600 emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Tmc1800" || computer == "Telmac1800")
                {
                    startComputer_ = TMC1800;
                    computer = "TMC1800";
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "Ram_Software", software);
                    if (parser.Found("ch", &software))
                        getSoftware(computer, "Chip_8_Software", software);
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on Telmac 1800 emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Tmc2000" || computer == "Telmac2000")
                {
                    startComputer_ = TMC2000;
                    computer = "TMC2000";
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "Ram_Software", software);
                    if (parser.Found("ch", &software))
                        getSoftware(computer, "Chip_8_Software", software);
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on Telmac 2000 emulator");
                        return false;
                    }
                    return true;
                }
                wxMessageOutput::Get()->Printf("Incorrect computer name specified");
                return false;
            break;

            case 'U':
                if (computer == "Uc1800")
                {
                    startComputer_ = UC1800;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                    {
                        mode_.load = true;
                        getSoftware(computer, "Software_File", software);
                    }
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on Infinite UC1800 emulator");
                        return false;
                    }
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Infinite UC1800 emulator");
                        return false;
                    }
                    return true;
                }
            break;

            case 'V':
                if (computer == "Visicom")
                {
                    startComputer_ = VISICOM;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "St2_File", software);
                    if (parser.Found("r", &software))
                        getSoftware(computer, "St2_File", software);
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Visicom COM-100 emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Victory")
                {
                    startComputer_ = VICTORY;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "St2_File", software);
                    if (parser.Found("r", &software))
                        getSoftware(computer, "St2_File", software);
                    if (parser.Found("ch", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -ch is not supported on Victory MPT-02 emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Vip")
                {
                    startComputer_ = VIP;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "Ram_Software", software);
                    if (parser.Found("ch", &software))
                        getSoftware(computer, "Chip_8_Software", software);
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on Cosmac VIP emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Vipii")
                {
                    startComputer_ = VIPII;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "Ram_Software", software);
                    if (parser.Found("ch", &software))
                        getSoftware(computer, "Chip_8_Software", software);
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on Cosmac VIP II emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Vip2k")
                {
                    startComputer_ = VIP2K;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "Ram_Software", software);
                    if (parser.Found("ch", &software))
                        getSoftware(computer, "Chip_8_Software", software);
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on VIP2K Membership Card emulator");
                        return false;
                    }
                    return true;
                }
                if (computer == "Velf")
                {
                    startComputer_ = VELF;
                    mode_.gui = false;
                    if (parser.Found("s", &software))
                        getSoftware(computer, "Ram_Software", software);
                    if (parser.Found("ch", &software))
                        getSoftware(computer, "Chip_8_Software", software);
                    if (parser.Found("r", &software))
                    {
                        wxMessageOutput::Get()->Printf("Option -r is not supported on VELF emulator");
                        return false;
                    }
                    return true;
                }
                wxMessageOutput::Get()->Printf("Incorrect computer name specified");
                return false;
            break;

            default:
                wxMessageOutput::Get()->Printf("Incorrect computer name specified");
                return false;
            break;
        }
    } 
    return true;
}

void Emu1802::getSoftware(wxString computer, wxString type, wxString software)
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

void Emu1802::createXml()
{
    wxString dirname = "/Users/etxmato/workspace/emma_02/";
    wxDir dir(dirname);
    if ( !dir.IsOpened() )
        return;

    wxString filename, filePart;
    bool cont = dir.GetFirst( &filename );
    while ( cont )
    {
        filePart = filename.Left(filename.Len()-4);
        if (filename.Right(3) == "xrc" && filePart.Right(2) != "11" && filePart.Right(2) != "12" &&  filePart.Right(2) != "14")
            createXmlFile(dirname, filePart);
        cont = dir.GetNext( &filename );
    }
}

void Emu1802::createXmlFile(wxString xrcDir, wxString xrcFile)
{
    wxTextFile inputFile, outputFile11, outputApplicationFile11, outputFile12, outputFile14, outputApplicationFile14;

    wxString input = xrcDir + xrcFile + ".xrc";
    wxString output11 = xrcDir + xrcFile + "_11.xrc";
    wxString output12 = xrcDir + xrcFile + "_12.xrc";
    wxString output14 = xrcDir + xrcFile + "_14.xrc";
    wxString outputApplication11 = applicationDirectory_ + xrcFile + "_11.xrc";
    wxString outputApplication14 = applicationDirectory_ + xrcFile + "_14.xrc";
    wxString line, line2, line3, line4, line5, line6, flagLine;
    int classType = UNDEFINED_OBJECT_CLASS, markup = UNDEFINED_MARKUP, markup2 = UNDEFINED_MARKUP;
    
    if (wxFile::Exists(input))
    {
        wxFileName inputFileName = wxFileName(input);
        wxFileName outputFileName = wxFileName(output11);
        wxDateTime inputDate = inputFileName.GetModificationTime();
        wxDateTime outputDate = outputFileName.GetModificationTime();

        if (outputDate.IsEarlierThan(inputDate))
        {
            inputFile.Open(input);
            
            createFile(&outputFile11, output11);
            createFile(&outputApplicationFile11, outputApplication11);
            createFile(&outputFile12, output12);
            createFile(&outputFile14, output14);
            createFile(&outputApplicationFile14, outputApplication14);

            for (line=inputFile.GetFirstLine(); !inputFile.Eof(); line=inputFile.GetNextLine())
            {
                line.Trim(false);
                
                markup = getMarkup(line);
                
                if (markup == MU_OBJECT)
                    classType = getObjectClass(line);

                if (markup == MU_FAMILY)
                {
                    line=inputFile.GetNextLine();
                    line.Trim(false);
                    markup = getMarkup(line);
                }
              
                if (markup == MU_LABEL && classType == WXSTATICTEXT)
                {
                    if (line.Right(8) == "</label>")
                    {
                        line = line.Left(line.Length()-8);
                        line.Trim(true);
                        line = line + " </label>";
                    }
                }

                outputFile11.AddLine(line);
                outputApplicationFile11.AddLine(line);

                if (xrcFile != "main" && markup == MU_SIZE)
                {
                    if (line == "<size>34,14</size>")   // HEX 4 (Memory)
                        line = "<size>46,18</size>";
                    
                    if (line == "<size>22,13</size>")   // Rx (1802 Debug)
                        line = "<size>28,18</size>";

                    if (line == "<size>125,13</size>")  // MEM Message (Memory)
                        line = "<size>154,18</size>";

                    if (line == "<size>128,16</size>")  // CHAR x (Memory)
                        line = "<size>165,16</size>";

                    if (line == "<size>20,14</size>"    // HEX 2 (Memory)
                        || line == "<size>26,13</size>")// Flags text 1802 Debug
                        line = "<size>26,18</size>";

                    if (line == "<size>13,14</size>")   // HEX 1 (1802 Debug)
                        line = "<size>16,18</size>";
                    
                    if (line == "<size>20,13</size>")   // Rx and Ox text 1802 Debug
                        line = "<size>24,18</size>";

                    if (line == "<size>17,13</size>")   // I text 1802 Debug
                        line = "<size>19,18</size>";

                    if (line == "<size>14,19</size>")   // Spacers O1, O2, O3
                        line = "<size>16,19</size>";
                    
                    if (line == "<size>-1,13</size>")    // Main registers text - 1802 Debug
                        line = "<size>-1,18</size>";

                    if (line == "<size>42,23</size>")    // buttons 1802 and Pdeudo debug
                        line = "<size>52,23</size>";

                    if (line == "<size>46,23</size>")    // buttons 1802 and Pdeudo debug
                        line = "<size>56,23</size>";

                    if (line == "<size>85,23</size>")    // breakpoint buttons
                        line = "<size>105,23</size>";

                    if (line == "<size>14,23</size>")    // SCRT HEX 1 (Debug 1802)
                        line = "<size>16,23</size>";
                    
                    if (line == "<size>40,23</size>")    // HEX 4 (Direct Assembler)
                        line = "<size>52,23</size>";

                    if (line == "<size>40,21</size>")    // HEX 4 (Direct Assembler)
                        line = "<size>52,21</size>";

                    if (line == "<size>65,-1</size>")    // Branch (Direct Assembler)
                        line = "<size>75,-1</size>";

                    if (line == "<size>75,-1</size>")    // Printer / IDE choice
                        line = "<size>95,-1</size>";

                    if (line == "<size>91,-1</size>")    // IDE field
                        line = "<size>131,-1</size>";

                    if (line == "<size>90,-1</size>")    // Printer field
                        line = "<size>131,-1</size>";

                    if (line == "<size>60,-1</size>")    // One/Range (Direct Assembler)
                        line = "<size>70,-1</size>";

                    if (line == "<size>42,25</size>")    // Mark (Direct Assembler)
                        line = "<size>52,25</size>";

                    if (line == "<size>54,23</size>")    // Search (Direct Assembler)
                        line = "<size>64,23</size>";

                    if (line == "<size>44,25</size>")    // Configuration (Direct Assembler)
                        line = "<size>54,25</size>";

                    if (line == "<size>29,5</size>")     // Configuration (Direct Assembler)
                        line = "<size>39,5</size>";

                    if (line == "<size>149,23</size>")   // Configuration (Direct Assembler)
                        line = "<size>185,23</size>";

                    if (line == "<size>272,0</size>")   // Profiler spacer
                        line = "<size>332,0</size>";

                    if (line == "<size>192,-1</size>")   // drop down selectors
                        line = "<size>252,-1</size>";

                    if (line == "<size>120,-1</size>")   // fdc fields
                        line = "<size>204,-1</size>";
                    
                    if (line == "<size>166,-1</size>")   // drop down selectors Membership/videodump
                        line = "<size>226,-1</size>";
                }

                outputFile14.AddLine(line);
                outputApplicationFile14.AddLine(line);

                if (xrcFile != "main_mac" && markup == MU_SIZE)
                {
                    if (line == "<size>22,-1d</size>")   // HEX 4 (Memory)
                        line = "<size>30,-1d</size>";
                    
                    if (line == "<size>71,8d</size>")    // MEM Message (Memory)
                        line = "<size>87,10d</size>";

                    if (line == "<size>73,9d</size>")    // CHAR x (Memory)
                        line = "<size>90,10d</size>";

                    if (line == "<size>10,-1d</size>")   // HEX 2 (Memory)
                        line = "<size>14,-1d</size>";

                   if (line == "<size>29,-1d</size>")    // HEX 4 (Direct Assembler)
                        line = "<size>36,-1d</size>";

                    if (line == "<size>152,0d</size>")   // Profiler spacer
                        line = "<size>192,0d</size>";

                    if (line == "<size>153,0d</size>")   // Profiler spacer
                        line = "<size>218,0d</size>";

                    if (line == "<size>115,-1d</size>")   // drop down selectors
                        line = "<size>150,-1d</size>";
                   
                    if (line == "<size>83,-1d</size>")     // Disk, printer fields
                        line = "<size>137,-1d</size>";

                   if (line == "<size>61,-1d</size>")     // Disk fields
                       line = "<size>100,-1d</size>";

                   if (line == "<size>60,-1d</size>")     // Printer field ELF
                       line = "<size>89,-1d</size>";

                   if (line == "<size>63,-1d</size>")     // Printer field COMX
                       line = "<size>103,-1d</size>";

                   if (line == "<size>69,-1d</size>")     // HEX / XMODEM
                       line = "<size>123,-1d</size>";

                    if (line == "<size>101,-1d</size>")   // drop down selectors Membership
                        line = "<size>131,-1d</size>";
                }
                
                outputFile12.AddLine(line);

                if (markup == MU_OBJECT && classType == SIZERITEM)
                {
                    line2=inputFile.GetNextLine(); // flag line
                    line2.Trim(false);

                    line3=inputFile.GetNextLine(); // border line
                    line3.Trim(false);

                    line4=inputFile.GetNextLine(); // object line
                    line4.Trim(false);

                    markup2 = getMarkup(line4);

                    if (markup2 == MU_OBJECT)
                    {
                        classType = getObjectClass(line4);

                        switch (classType)
                        {
                            case WXBUTTON:
                                flagLine = line2.Right(line2.Len()-6);
                                if (flagLine.Left(6) == "wxGROW")
                                    flagLine = flagLine.Right(flagLine.Len()-6);
                                if (flagLine.Left(1) == "|")
                                    flagLine = flagLine.Right(flagLine.Len()-1);

                                outputFile11.AddLine("<flag>wxGROW</flag>");
                                outputApplicationFile11.AddLine("<flag>wxGROW</flag>");
                                outputFile12.AddLine("<flag>wxGROW</flag>");
                        //        outputFile12.AddLine("<flag>wxGROW|"+flagLine);
                                outputFile14.AddLine("<flag>wxGROW</flag>");
                        //        outputFile14.AddLine("<flag>wxGROW|"+flagLine);
                                outputApplicationFile14.AddLine("<flag>wxGROW</flag>");
                        //        outputApplicationFile14.AddLine("<flag>wxGROW|"+flagLine);
                                outputFile11.AddLine(line3);
                                outputApplicationFile11.AddLine(line3);
                                outputFile11.AddLine(line4);
                                outputApplicationFile11.AddLine(line4);

                                outputFile12.AddLine(line3);
                                outputFile14.AddLine(line3);
                                outputApplicationFile14.AddLine(line3);

                                outputFile12.AddLine(line4);
                                outputFile14.AddLine(line4);
                                outputApplicationFile14.AddLine(line4);
                            break;

                            case WXSTATICBOXSIZER:
                                outputFile11.AddLine(line2);
                                outputApplicationFile11.AddLine(line2);
                                outputFile11.AddLine(line3);
                                outputApplicationFile11.AddLine(line3);
                                outputFile11.AddLine(line4);
                                outputApplicationFile11.AddLine(line4);
                              
                                outputFile12.AddLine(line2);
                                outputFile12.AddLine(line3);
                                outputFile12.AddLine(line4);
                                
                                line5=inputFile.GetNextLine(); // orient line
                                line5.Trim(false);
                                outputFile11.AddLine(line5);
                                outputApplicationFile11.AddLine(line5);
                                outputFile12.AddLine(line5);

                                line6=inputFile.GetNextLine(); // label line
                                line6.Trim(false);
                                outputFile11.AddLine(line6);
                                outputApplicationFile11.AddLine(line6);
                                outputFile12.AddLine(line6);

 //                               outputFile14.AddLine("<flag>wxALIGN_LEFT|wxLEFT</flag>");
 //                               outputApplicationFile14.AddLine("<flag>wxALIGN_LEFT|wxLEFT</flag>");
                                outputFile14.AddLine("<border>12</border>");
                                outputApplicationFile14.AddLine("<border>12</border>");
                                outputFile14.AddLine("<object class=\"wxStaticText\" name=\"wxID_STATIC\">");
                                outputApplicationFile14.AddLine("<object class=\"wxStaticText\" name=\"wxID_STATIC\">");
                                outputFile14.AddLine("<font>");
                                outputApplicationFile14.AddLine("<font>");
                                outputFile14.AddLine("<size>14</size>");
                                outputApplicationFile14.AddLine("<size>14</size>");
                                outputFile14.AddLine("</font>");
                                outputApplicationFile14.AddLine("</font>");
                                outputFile14.AddLine(line6);
                                outputApplicationFile14.AddLine(line6);
                                outputFile14.AddLine("</object>");
                                outputApplicationFile14.AddLine("</object>");
                                outputFile14.AddLine("</object>");
                                outputApplicationFile14.AddLine("</object>");
                                outputFile14.AddLine(line);
                                outputApplicationFile14.AddLine(line);
                                outputFile14.AddLine(line2);
                                outputApplicationFile14.AddLine(line2);
                                outputFile14.AddLine(line3);
                                outputApplicationFile14.AddLine(line3);
                                outputFile14.AddLine(line4);
                                outputApplicationFile14.AddLine(line4);
                                outputFile14.AddLine(line5);
                                outputApplicationFile14.AddLine(line5);
                                outputFile14.AddLine("<label></label>");
                                outputApplicationFile14.AddLine("<label></label>");
                            break;

                            default:
                                outputFile11.AddLine(line2);
                                outputApplicationFile11.AddLine(line2);
                                outputFile11.AddLine(line3);
                                outputApplicationFile11.AddLine(line3);
                                outputFile11.AddLine(line4);
                                outputApplicationFile11.AddLine(line4);

                                outputFile12.AddLine(line2);
                                outputFile14.AddLine(line2);
                                outputApplicationFile14.AddLine(line2);

                                outputFile12.AddLine(line3);
                                outputFile14.AddLine(line3);
                                outputApplicationFile14.AddLine(line3);

                                outputFile12.AddLine(line4);
                                outputFile14.AddLine(line4);
                                outputApplicationFile14.AddLine(line4);
                            break;
                        }
                    }
                    else
                    {
                       outputFile11.AddLine(line2);
                       outputApplicationFile11.AddLine(line2);
                       outputFile11.AddLine(line3);
                       outputApplicationFile11.AddLine(line3);
                       outputFile11.AddLine(line4);
                       outputApplicationFile11.AddLine(line4);
                    }
                }
                
                if (markup == MU_FONT)
                {
                    line=inputFile.GetNextLine();
                    line.Trim(false);
                    markup = getMarkup(line);

                    if (markup == MU_SYSFONT)
                    {
                        outputFile11.AddLine(line);
                        outputApplicationFile11.AddLine(line);
          
                        line=inputFile.GetNextLine();
                        line.Trim(false);
                        markup = getMarkup(line);
                    }

                    if (markup == MU_SIZE)
                    {
                        line="<size>-1</size>"; // OSX: 11, Windows: 9
                        outputFile11.AddLine(line);
                        outputApplicationFile11.AddLine(line);
                        
                        line="<size>12</size>";
                        outputFile12.AddLine(line);

                        line="<size>14</size>";
                        outputFile14.AddLine(line);
                        outputApplicationFile14.AddLine(line);
                    }
                    else
                    {
                        outputFile11.AddLine(line);
                        outputApplicationFile11.AddLine(line);
                        outputFile12.AddLine(line);
                        outputFile14.AddLine(line);
                        outputApplicationFile14.AddLine(line);
                   }
                }
            }
            
            outputFile11.Write();
            outputApplicationFile11.Write();
            outputFile12.Write();
            outputFile14.Write();
            outputApplicationFile14.Write();

            outputFile11.Close();
            outputApplicationFile11.Close();
            outputFile12.Close();
            outputFile14.Close();
            outputApplicationFile14.Close();

            inputFile.Close();
        }
    }
}

void Emu1802::createFile(wxTextFile* filename, wxString name)
{
    if (wxFile::Exists(name))
    {
        filename->Open(name);
        filename->Clear();
    }
    else
        filename->Create(name);
}

int Emu1802::getObjectClass(wxString line)
{
    wxString classType;
    
    classType = line.Right(line.Length()-(line.Find("\"")+1));
    classType = classType.Left(classType.Find("\""));

    int classTypeInt = 0;
    
    while (classTypeInt != UNDEFINED_OBJECT_CLASS && objectClass[classTypeInt] != classType)
        classTypeInt++;
    
    return classTypeInt;
}

int Emu1802::getMarkup(wxString line)
{
    wxString markupType;
    
    markupType = line.Right(line.Length()-(line.Find("<")+1));
    int end = markupType.Find(">");
    int end2 = markupType.Find(" ");
    if (end2 < end && end2 != wxNOT_FOUND)
        end = end2;
    
    markupType = markupType.Left(end);

    int markupTypeInt = 0;
    
    while (markupTypeInt != UNDEFINED_MARKUP && markupList[markupTypeInt] != markupType)
        markupTypeInt++;
    
    return markupTypeInt;
}

Main::Main(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: DebugWindow(title, pos, size, mode, dataDir, iniDir)
{
    zoomEventOngoing_ = false;
    fullScreenEventOngoing_ = false;
    selectedComputer_ = COMX;
    computerRunning_ = false;
    runningComputer_ = NO_COMPUTER;

    popupDialog_ = NULL;
    emmaClosing_ = false;
    
    xmlLoaded_ = false;
    configurationMenuOn_ = false;
    guiInitialized_ = false;
    
#ifndef __WXMAC__
    SetIcon(wxICON(app_icon));
#endif

    bool forceGuiSizeReset;
    configPointer->Read("/Main/ForceGuiSizeReset", &forceGuiSizeReset, false);

    if (mode_.gui)
    {
        SetMenuBar(wxXmlResource::Get()->LoadMenuBar("Main_Menu"));
        wxXmlResource::Get()->LoadPanel(this, "Main_GUI");
        
        defaultGuiSize_ = getDefaultGuiSize();
        
        if (forceGuiSizeReset)
        {
            windowInfo.mainwX = defaultGuiSize_.x;
            windowInfo.mainwY = defaultGuiSize_.y;
            configPointer->Write("/Main/ForceGuiSizeReset", false);
        }
        else
        {
            windowInfo.mainwX = (int)configPointer->Read("/Main/Window_Size_X_133", defaultGuiSize_.x);
            windowInfo.mainwY = (int)configPointer->Read("/Main/Window_Size_Y_133", defaultGuiSize_.y);
        }
        
        this->SetSize(wxSize(windowInfo.mainwX, windowInfo.mainwY));
    }

    updateMemory_ = false;
    updateSlotinfo_ = false;
    for (int x=0; x<16; x++)
        for (int y=0; y<16; y++)
            memoryChanged_[x][y] = false;
    for (int y=0; y<16; y++)
        rowChanged_[y] = false;

    xmlLoaded_ = true;

    eventNumber_ = 0;

    wxString helpFile = applicationDirectory_ + "emma_02.htb";

    help_ = new MyHtmlHelpController(wxHF_TOOLBAR | wxHF_CONTENTS | wxHF_INDEX | wxHF_SEARCH | wxHF_BOOKMARKS | wxHF_PRINT | wxHF_BOOKMARKS);
        
    if (!help_->AddBook(helpFile))
    {
        (void)wxMessageBox( "Failed adding book " + helpFile + "\n", "Emma 02", wxICON_ERROR | wxOK );
    }
    
    if (mode_.gui)
    {
        this->Connect(XRCID(GUICOMPUTERNOTEBOOK), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler(Main::onComputer) );
        wxFont smallFont(6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetFont(smallFont);
        
        wxFont* defaultFont;

        if (fontSize_ == 11)
        {
            defaultFont = new wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
            defaultFont->SetPointSize(fontSize_);
        }
        else
            defaultFont = new wxFont(fontSize_, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetFont(*defaultFont);
        delete defaultFont;

        wxString text;
        chip8TraceWindowPointer = XRCCTRL(*this,"Chip8TraceWindow", wxTextCtrl);
        for (int i=0; i<16; i++)
        {
            text.Printf("R%d",i);
            registerTextPointer[i] = XRCCTRL(*this,text, wxTextCtrl);
            text.Printf("V%01X",i);
            chip8varTextPointer[i] = XRCCTRL(*this,text, wxTextCtrl);
        }
        for (int i=1; i<8; i++)
        {
            text.Printf("I%d",i);
            inTextPointer[i] = XRCCTRL(*this,text, wxTextCtrl);
            text.Printf("O%d",i);
            outTextPointer[i] = XRCCTRL(*this,text, wxTextCtrl);
        }
        for (int i=1; i<5; i++)
        {
            text.Printf("EF%d",i);
            efTextPointer[i] = XRCCTRL(*this,text, wxTextCtrl);
        }
        dfTextPointer = XRCCTRL(*this,"DF", wxTextCtrl);
        qTextPointer = XRCCTRL(*this,"Q", wxTextCtrl);
        ieTextPointer = XRCCTRL(*this,"IE", wxTextCtrl);
        cieTextPointer = XRCCTRL(*this,"CIE", wxTextCtrl);
        chTextPointer = XRCCTRL(*this,"CH", wxTextCtrl);
        counterTextPointer = XRCCTRL(*this,"CNTR", wxTextCtrl);
        dTextPointer = XRCCTRL(*this,"D", wxTextCtrl);
        pTextPointer = XRCCTRL(*this,"P", wxTextCtrl);
        xTextPointer = XRCCTRL(*this,"X", wxTextCtrl);
        tTextPointer = XRCCTRL(*this,"T", wxTextCtrl);
        bTextPointer = XRCCTRL(*this,"B", wxTextCtrl);

        traceWindowPointer = XRCCTRL(*this,"TraceWindow", wxTextCtrl);
        assInputWindowPointer = XRCCTRL(*this,"AssInputWindow", wxTextCtrl);
        assErrorWindowPointer = XRCCTRL(*this,"AssErrorMultiLine", wxTextCtrl);
        breakPointWindowPointer = XRCCTRL(*this,"BreakPointWindow", wxListCtrl);
        chip8BreakPointWindowPointer = XRCCTRL(*this,"Chip8BreakPointWindow", wxListCtrl);
        tregWindowPointer = XRCCTRL(*this,"TregWindow", wxListCtrl);
        trapWindowPointer = XRCCTRL(*this,"TrapWindow", wxListCtrl);
    }

    initConfig();
    if (windowInfo.errorMessage != "")
        message(windowInfo.errorMessage);

    wxString oldVersionString = configPointer->Read("/Main/OldVersion", "13600");
    double oldVersion;
    oldVersionString.ToDouble(&oldVersion);

    if ((int)(EMMA_VERSION*10000 + EMMA_SUB_VERSION) > (int)oldVersion)
    {
        oldVersionString.Printf("%d", (int)(EMMA_VERSION*10000 + EMMA_SUB_VERSION));
        configPointer->Write("/Main/OldVersion", oldVersionString);

        int answer = wxMessageBox("New release detected: \n\nRe-install of configuration files recommended\n\nThis will overwrite files in the configuration directory:\n"+iniDir_ + "Configurations" + pathSeparator_+"\n\nContinue to install default configuration files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
        if (answer == wxYES)
        {
            reInstall(applicationDirectory_ + "Configurations" + pathSeparator_, iniDir_ + "Configurations" + pathSeparator_, pathSeparator_);
        }

        answer = wxMessageBox("New release detected: \n\nRe-install of 1802 software files recommended\n\nThis will overwrite files in the 1802 software directory:\n"+dataDir_+"\n\nContinue to install default 1802 software files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
        if (answer == wxYES)
        {
            reInstall(applicationDirectory_ + "data" + pathSeparator_, dataDir_, pathSeparator_);
        }
    }

    bool softwareDirInstalled;
    for (int computer=2; computer<NO_COMPUTER; computer++)
    {
        int confComputer = computer;
        if (confComputer == 2)
            confComputer = 0;
        if (confComputer == XML) // *** to be removed ??
        {
           confComputer++;
           computer++;
        }
        if (confComputer == FRED1_5)
        {
            if (wxDir::Exists(dataDir + "FRED2") && !wxDir::Exists(dataDir + computerInfo[confComputer].gui))
                wxRenameFile(dataDir + "FRED2", dataDir + computerInfo[confComputer].gui);
        }
        
        configPointer->Read(computerInfo[confComputer].gui + "/SoftwareDirInstalled", &softwareDirInstalled, false);
        if (!softwareDirInstalled)
        {
            if (!wxDir::Exists(dataDir + computerInfo[confComputer].gui))
            {
                int answer = wxMessageBox("1802 Software directory " + computerInfo[confComputer].gui + " does not exist, install default files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
                if (answer == wxYES)
                {
                    wxDir::Make(dataDir + computerInfo[confComputer].gui);
                    p_Main->reInstall(applicationDirectory_ + "data" + pathSeparator_ + computerInfo[confComputer].gui + pathSeparator_,  dataDir + computerInfo[confComputer].gui + pathSeparator_, pathSeparator_);
                }
                configPointer->Write(computerInfo[confComputer].gui + "/SoftwareDirInstalled", true);
            }
            wxDir compDir(dataDir + computerInfo[confComputer].gui);
            if (!compDir.HasFiles())
            {
                int answer = wxMessageBox("1802 Software directory " + computerInfo[confComputer].gui + " is empty, install default files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
                if (answer == wxYES)
                {
                    p_Main->reInstall(applicationDirectory_ + "data" + pathSeparator_ + computerInfo[confComputer].gui + pathSeparator_,  dataDir + computerInfo[confComputer].gui + pathSeparator_, pathSeparator_);
                }
                configPointer->Write(computerInfo[confComputer].gui + "/SoftwareDirInstalled", true);
            }
        }
    }
    bool redundantFilesRemoveCheck;
    configPointer->Read("/Main/RedundantFilesRemoveCheck", &redundantFilesRemoveCheck, false);
    if (!redundantFilesRemoveCheck)
    {
        removeRedundantFiles();
        configPointer->Write("/Main/RedundantFilesRemoveCheck", true);
    }
 
    readConfig();
    
    if (forceGuiSizeReset && mode_.window_position_fixed)
        fixedWindowPosition();

    oldGauge_ = 1;
    vuPointer = new wxTimer(this, 902);
 //   cpuPointer = new wxTimer(this, 903);
    updateCheckPointer = new wxTimer(this, 905);
    traceTimeoutPointer = new wxTimer(this, 906);
    keyDebounceTimeoutPointer = new wxTimer(this, 907);
    guiSizeTimeoutPointer = new wxTimer(this, 908);
    guiRedrawBarTimeOutPointer = new wxTimer(this, 909);
    directAssPointer = new wxTimer(this, 910);
    tapePauseTimerPointer = new wxTimer(this, 911);
    guiSizeTimerStarted_ = false;
    
    if (mode_.gui)
        buildConfigMenu();

    this->connectKeyEvent(this);

    wxSystemOptions::SetOption("msw.window.no-clip-children", 0);
    
    updateCheckStarted_ = false;
    if (mode_.update_check) 
        updateCheckPointer->Start(10000, wxTIMER_ONE_SHOT);

    guiInitialized_ = true;
    panelRefreshOngoing_ = false;
    videoRefreshOngoing_ = false;
    emuClosing_ = false;
}

Main::~Main()
{
    if (mode_.gui)
    {
        if (selectedComputer_ < NO_COMPUTER && configurationMenuOn_ == true)
        {
            configurationMenu->Destroy(GUI_CONFIG_MENU);
            configurationDeleteMenu->Destroy(GUI_CONFIG_DELETE_MENU);
        }

        if (selectedComputer_ == 0 || selectedComputer_ == 1)
            selectedComputer_ = 2;
        
        for (int computer = 2; computer<NO_COMPUTER; computer++)
        {
     //      if (computer == XML) // *** to be removed
     //        computer++;

            if (selectedComputer_ != computer)
            {
                delete conf[computer].configurationMenu;
                delete conf[computer].configurationDeleteMenu;
            }
        }

        for (int computer=0; computer<NO_COMPUTER; computer++)
        {
     //       if (computer == XML) // *** to be removed
     //         computer++;

            delete clockText[computer];
            delete clockTextCtrl[computer];
            delete mhzText[computer];
            delete startButton[computer];
        }
    }

    delete vuPointer;
    delete directAssPointer;
//    delete cpuPointer;
    delete updateCheckPointer;
    delete traceTimeoutPointer;
    delete keyDebounceTimeoutPointer;
    delete tapePauseTimerPointer;
    delete guiSizeTimeoutPointer;
    delete guiRedrawBarTimeOutPointer;
    delete help_;
    if (configPointer == NULL || !saveOnExit_)
        return;

    this->GetPosition(&mainWindowX_, &mainWindowY_);
    writeConfig();
    
    if (updateCheckStarted_)
    {
        {
            wxCriticalSectionLocker enter(m_pUpdateCheckThreadCS);
            if (m_pUpdateCheckThread)         // does the thread still exist?
            {
                m_pUpdateCheckThread->Delete();
            }
        }
        // exit from the critical section to give the thread
        // the possibility to enter its destructor
        // (which is guarded with m_pUpdateCheckThreadCS critical section!)
        while (1)
        {
            { // was the ~UpdateCheckThread() function executed?
                wxCriticalSectionLocker enter(m_pUpdateCheckThreadCS);
                if (!m_pUpdateCheckThread) break;
            }
            // wait for thread completion
            wxThread::This()->Sleep(1);
        }
    }
}

wxSize Main::getPosition(wxString control, wxSize size)
{
    int x, y;
    
    XRCCTRL(*this, control, wxStaticText)->GetPosition(&x, &y);
    if (x > size.x) size.x = x;
    if (y > size.y) size.y = y;
    
    return size;
}

wxSize Main::getDefaultGuiSize()
{
    wxSize size;
    size.x = 0; size.y = 0;
    
    int number = 0;
    while (guiSizers[number] != "End")
    {
        size = getPosition(guiSizers[number], size);
        number++;
    }
    
#if defined (__WXMAC__)
    size.x += 40;
    size.y += 114;
#endif
#if defined (__linux__)
    size.x += 30;
    size.y += 140;
#endif
#if defined (__WXMSW__)
    size.x += 28;
    size.y += 114;
#endif

    return size;
}

void Main::pageSetup()
{
    (*p_PageSetupData) = *PrintDataPointer;

     wxPageSetupDialog pageSetupDialog(this, p_PageSetupData);
     pageSetupDialog.ShowModal();

    (*PrintDataPointer) = pageSetupDialog.GetPageSetupDialogData().GetPrintData();
    (*p_PageSetupData) = pageSetupDialog.GetPageSetupDialogData();
}

void Main::writeConfig()
{
    wxMenuBar *menubarPointer = GetMenuBar();

    if (mainWindowX_ > 0)
        configPointer->Write("/Main/Window_Position_X", mainWindowX_);
    if (mainWindowY_ > 0)
        configPointer->Write("/Main/Window_Position_Y", mainWindowY_);
    configPointer->Write("/Main/FontSize", fontSize_);
    configPointer->Write("/Main/FontSizeString", fontSizeString_);

    this->GetSize(&windowInfo.mainwX, &windowInfo.mainwY);
//#if defined (__linux__)
//    windowInfo.mainwY -= 28;
//#endif
    configPointer->Write("/Main/Window_Size_X_133", windowInfo.mainwX);
    configPointer->Write("/Main/Window_Size_Y_133", windowInfo.mainwY);
    
    if (mode_.gui)
    {
        configPointer->Write("/Main/Selected_Tab", XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->GetSelection());
        configPointer->Write("/Main/Selected_Cosmac_Tab", XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->GetSelection());
        configPointer->Write("/Main/Selected_Rca_Tab", XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->GetSelection());
        configPointer->Write("/Main/Selected_Studio_Tab", XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->GetSelection());
        configPointer->Write("/Main/Selected_Telmac_Tab", XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->GetSelection());
        configPointer->Write("/Main/Selected_Debugger_Tab", XRCCTRL(*this, "DebuggerChoiceBook", wxNotebook)->GetSelection());

        if (menubarPointer->IsChecked(XRCID("SYSTEM00")))
            configPointer->Write("/Main/Cpu_Type", "SYSTEM00");
        if (menubarPointer->IsChecked(XRCID("CDP1801")))
            configPointer->Write("/Main/Cpu_Type", "CDP1801");
        if (menubarPointer->IsChecked(XRCID("CDP1802")))
            configPointer->Write("/Main/Cpu_Type", "CDP1802");
        if (menubarPointer->IsChecked(XRCID("CDP1804")))
            configPointer->Write("/Main/Cpu_Type", "CDP1804");
        if (menubarPointer->IsChecked(XRCID("CDP1805")))
            configPointer->Write("/Main/Cpu_Type", "CDP1805");

        if (menubarPointer->IsChecked(XRCID("StartupRegistersZeroed")))
            configPointer->Write("/Main/Cpu_StartupRegisters", "StartupRegistersZeroed");
        if (menubarPointer->IsChecked(XRCID("StartupRegistersRandom")))
            configPointer->Write("/Main/Cpu_StartupRegisters", "StartupRegistersRandom");

        if (menubarPointer->IsChecked(XRCID("StartupRamZeroed")))
            configPointer->Write("/Main/Cpu_StartupRam", "StartupRamZeroed");
        if (menubarPointer->IsChecked(XRCID("StartupRamRandom")))
            configPointer->Write("/Main/Cpu_StartupRam", "StartupRamRandom");
        if (menubarPointer->IsChecked(XRCID("StartupRamDynamic")))
            configPointer->Write("/Main/Cpu_StartupRam", "StartupRamDynamic");
        
        if (menubarPointer->IsChecked(XRCID("StartupVideoRamZeroed")))
            configPointer->Write("/Main/Cpu_StartupVideoRam", "StartupVideoRamZeroed");
        if (menubarPointer->IsChecked(XRCID("StartupVideoRamRandom")))
            configPointer->Write("/Main/Cpu_StartupVideoRam", "StartupVideoRamRandom");
        if (menubarPointer->IsChecked(XRCID("StartupVideoRamDynamic")))
            configPointer->Write("/Main/Cpu_StartupVideoRam", "StartupVideoRamDynamic");
        
        if (menubarPointer->IsChecked(XRCID("KeyboardDutch")))
            configPointer->Write("/Main/Keyboard_Type", "KeyboardDutch");
        if (menubarPointer->IsChecked(XRCID("KeyboardGerman")))
            configPointer->Write("/Main/Keyboard_Type", "KeyboardGerman");
        if (menubarPointer->IsChecked(XRCID("KeyboardFrench")))
            configPointer->Write("/Main/Keyboard_Type", "KeyboardFrench");
        if (menubarPointer->IsChecked(XRCID("KeyboardSwedish")))
            configPointer->Write("/Main/Keyboard_Type", "KeyboardSwedish");
        if (menubarPointer->IsChecked(XRCID("KeyboardUs")))
            configPointer->Write("/Main/Keyboard_Type", "KeyboardUs");
        if (menubarPointer->IsChecked(XRCID("KeyboardUserDefined")))
            configPointer->Write("/Main/Keyboard_Type", "KeyboardUserDefined");

        if (menubarPointer->IsChecked(XRCID("Flat")))
            configPointer->Write("/Main/Equalization", "Flat");
        if (menubarPointer->IsChecked(XRCID("Crisp")))
            configPointer->Write("/Main/Equalization", "Crisp");
        if (menubarPointer->IsChecked(XRCID("Default")))
            configPointer->Write("/Main/Equalization", "Default");
        if (menubarPointer->IsChecked(XRCID("TV Speaker")))
            configPointer->Write("/Main/Equalization", "TV Speaker");
        if (menubarPointer->IsChecked(XRCID("Handheld")))
            configPointer->Write("/Main/Equalization", "Handheld");
    }
    if (!mode_.portable)
    {
        if (dataDir_.Find(applicationDirectory_) != wxNOT_FOUND)
        {
            configPointer->Write("/DataDirRelative", true);

            wxString tempDir = dataDir_;
            tempDir.Replace(applicationDirectory_, "");
            configPointer->Write("/DataDir", tempDir);
        }
        else
        {
            configPointer->Write("/DataDirRelative", false);
            configPointer->Write("/DataDir", dataDir_);
        }
    }
    configPointer->Write("/Main/LapTimeTrigger", lapTimeTrigger_);
    configPointer->Write("/Main/Save_Debug_File", saveDebugFile_);
    configPointer->Write("/Main/Save_On_Exit", saveOnExit_);
    configPointer->Write("/Main/Check_For_Update", checkForUpdate_);
    configPointer->Write("/Main/Floating_Point_Zoom", fullScreenFloat_);
    configPointer->Write("/Main/Use_Num_Pad", useNumPad_);
    configPointer->Write("/Main/Use_Exit_Key", useExitKey_);
    configPointer->Write("/Main/Use_Ctrlv_Key", useCtrlvKey_);
    configPointer->Write("/Main/Exit_Key", functionKey_[0]);
    configPointer->Write("/Main/Help_Key", functionKey_[1]);
    configPointer->Write("/Main/Activate_Main_Key", functionKey_[2]);
    configPointer->Write("/Main/Full_Screen_Key", functionKey_[3]);
    configPointer->Write("/Main/Printer_Window_Key", functionKey_[4]);
    configPointer->Write("/Main/Video_Dump_Key", functionKey_[5]);
    configPointer->Write("/Main/Debug_Mode_Key", functionKey_[6]);
    configPointer->Write("/Main/Menu_Key", functionKey_[7]);
    configPointer->Write("/Main/VT_Setup", functionKey_[8]);
    configPointer->Write("/Main/Start_Reset_Key", functionKey_[12]);
    configPointer->Write("/Main/Ctrlv_Key", functionKey_[13]);
    configPointer->Write("/Main/Psave_Volume", psaveData_[0]);
    configPointer->Write("/Main/Psave_Bit_Rate", psaveData_[1]);
    configPointer->Write("/Main/Psave_Bits_Per_Sample", psaveData_[2]);
    configPointer->Write("/Main/Wav_Conversion_Type", psaveData_[3]);
    configPointer->Write("/Main/Cassette_Input_Channel", psaveData_[4]);
    configPointer->Write("/Main/Cassette_Playback_Input", psaveData_[5]);
    configPointer->Write("/Main/Cassette_Reversed_Polarity", psaveData_[6]);
    configPointer->Write("/Main/Cassette_Conversion_Type", psaveData_[7]);
    configPointer->Write("/Main/Cassette_Fred_Threshold_8", psaveData_[8]);
    configPointer->Write("/Main/Cassette_Fred_Threshold_16", psaveData_[9]);
    configPointer->Write("/Main/Cassette_Fred_Freq", psaveData_[10]);
    configPointer->Write("/Main/Cassette_CV_A", psaveData_[11]);
    configPointer->Write("/Main/Cassette_CV_B", psaveData_[12]);
    configPointer->Write("/Main/UseXmlThreshold", psaveData_[13]);
    configPointer->Write("/Main/Cassette_Fred_Threshold_24", psaveData_[14]);
    configPointer->Write("/Main/Window_Positions_Fixed", mode_.window_position_fixed);

    writeDebugConfig();

    writeComxDirConfig();
    writeElf2KDirConfig();
    writeMS2000DirConfig();
    writeMcdsDirConfig();
    writeCosmicosDirConfig();
    writeVipDirConfig();
    writeVipIIDirConfig();
    writeVip2KDirConfig();
    writeVelfDirConfig();
    writeCdp18s020DirConfig();
    writeCdp18s600DirConfig();
    writeElfDirConfig(ELF, "Elf");
    writeElfDirConfig(ELFII, "ElfII");
    writeElfDirConfig(SUPERELF, "SuperElf");
    writeXmlDirConfig();
    writePicoDirConfig();
    writeMembershipDirConfig();
    writeUc1800DirConfig();
    writeMicrotutorDirConfig();
    writeMicrotutor2DirConfig();
    writeStudioDirConfig();
    writeCoinArcadeDirConfig();
    writeFred1DirConfig();
    writeFred2DirConfig();
    writeVisicomDirConfig();
    writeVictoryDirConfig();
    writeStudioIVDirConfig();
    writeCidelsaDirConfig();
    writeTelmacDirConfig();
    writeTMC2000DirConfig();
    writeTMC1800DirConfig();
    writeNanoDirConfig();
    writePecomDirConfig();
    writeEtiDirConfig();

    writeComxConfig();
    writeElf2KConfig();
    writeMS2000Config();
    writeMcdsConfig();
    writeCosmicosConfig();
    writeVipConfig();
    writeVipIIConfig();
    writeVip2KConfig();
    writeVelfConfig();
    writeCdp18s020Config();
    writeCdp18s600Config();
    writeElfConfig(ELF, "Elf");
    writeElfConfig(ELFII, "ElfII");
    writeElfConfig(SUPERELF, "SuperElf");
    writeXmlConfig();
    writePicoConfig();
    writeMembershipConfig();
    writeUc1800Config();
    writeMicrotutorConfig();
    writeMicrotutor2Config();
    writeStudioConfig();
    writeCoinArcadeConfig();
    writeFred1Config();
    writeFred2Config();
    writeVisicomConfig();
    writeVictoryConfig();
    writeStudioIVConfig();
    writeCidelsaConfig();
    writeTelmacConfig();
    writeTMC2000Config();
    writeTMC1800Config();
    writeNanoConfig();
    writePecomConfig();
    writeEtiConfig();

    writeComxWindowConfig();
    writeElf2KWindowConfig();
    writeMS2000WindowConfig();
    writeMcdsWindowConfig();
    writeCosmicosWindowConfig();
    writeVipWindowConfig();
       writeVipIIWindowConfig();
       writeVip2KWindowConfig();
    writeVelfWindowConfig();
    writeCdp18s020WindowConfig();
    writeCdp18s600WindowConfig();
    writeElfWindowConfig(ELF, "Elf");
    writeElfWindowConfig(ELFII, "ElfII");
    writeElfWindowConfig(SUPERELF, "SuperElf");
    writeXmlWindowConfig();
    writePicoWindowConfig();
    writeMembershipWindowConfig();
    writeUc1800WindowConfig();
    writeMicrotutorWindowConfig();
    writeMicrotutor2WindowConfig();
    writeStudioWindowConfig();
    writeCoinArcadeWindowConfig();
    writeFred1WindowConfig();
    writeFred2WindowConfig();
    writeVisicomWindowConfig();
    writeVictoryWindowConfig();
    writeStudioIVWindowConfig();
    writeCidelsaWindowConfig();
    writeTelmacWindowConfig();
    writeTMC2000WindowConfig();
    writeTMC1800WindowConfig();
    writeNanoWindowConfig();
    writePecomWindowConfig();
    writeEtiWindowConfig();
}

void Main::initConfig()
{
    setSysColours();
    
    Byte brightness[8] = { 0, 28, 77, 105, 150, 194, 227, 0xff };

#if defined(__linux__)
    offsetX_ = 36;
    offsetY_ = 0;
#elif (__WXMAC__)
    offsetX_ = 30;
    offsetY_ = 30;
#else
    offsetX_ = 0;
    offsetY_ = 0;
#endif

    mainWindowX_ = (int)configPointer->Read("/Main/Window_Position_X", 30 + offsetX_);
    mainWindowY_ = (int)configPointer->Read("/Main/Window_Position_Y", 30 + offsetY_);

    percentageClock_ = 1;

    wxString colour[COL_MAX];
    int borderX[10];
    int borderY[10];

    for (int i = 0; i < 8; i++)
    {
        colour[i * 8].Printf("#%02X%02X%02X", (brightness[i] ^ 0xff) & 0xff, (brightness[i] ^ 0xff) & 0xff, (brightness[i] ^ 0xff) & 0xff);
        colour[i * 8 + 1].Printf("#%02X%02X%02X", 0, brightness[i], 0);
        colour[i * 8 + 2].Printf("#%02X%02X%02X", 0, 0, brightness[i]);
        colour[i * 8 + 3].Printf("#%02X%02X%02X", 0, brightness[i], brightness[i]);
        colour[i * 8 + 4].Printf("#%02X%02X%02X", brightness[i], 0, 0);
        colour[i * 8 + 5].Printf("#%02X%02X%02X", brightness[i], brightness[i], 0);
        colour[i * 8 + 6].Printf("#%02X%02X%02X", brightness[i], 0, brightness[i]);
        colour[i * 8 + 7].Printf("#%02X%02X%02X", brightness[i], brightness[i], brightness[i]);
    }
    colour[COL_MC6845_FORE] = "#00ff00";    // foreground mc6845
    colour[COL_MC6845_BACK] = "#004000";    // background 80 Columnmc6845

    borderX[VIDEO1870] = 0;
    borderY[VIDEO1870] = 0; // CDP 1870
    borderX[VIDEO80COL] = 0;
    borderY[VIDEO80COL] = 0;  // 80 Column

    setScreenInfo(COMX, 0, COL_MC6845_BACK+1, colour, 3, borderX, borderY);
    setComputerInfo(COMX, "Comx", "COMX-35", "comx");

    setScreenInfo(CIDELSA, COL_V1870_MAIN, COL_V1870_MAIN+8, colour, 1, borderX, borderY);
    setComputerInfo(CIDELSA, "Cidelsa", "Cidelsa", "");

    setScreenInfo(TMC600, COL_V1870_MAIN, COL_V1870_MAIN+8, colour, 1, borderX, borderY);
    setComputerInfo(TMC600, "TMC600", "Telmac 600", "tmc600");

    setScreenInfo(PECOM, COL_V1870_MAIN, COL_V1870_MAIN+8, colour, 1, borderX, borderY);
    setComputerInfo(PECOM, "Pecom", "Pecom 64", "pecom");

    borderX[VIDEOMICROVT] = 0;
    borderY[VIDEOMICROVT] = 0;  //Video Terminal
    borderX[VIDEO1870] = 0;
    borderY[VIDEO1870] = 0; // CDP 1870

    colour[COL_VT_FORE] = "#00ff00";    // foreground vt
    colour[COL_VT_BACK] = "#004000";    // background vt
    colour[COL_VT_HIGH] = "#00ff00";    // highlight vt

    setScreenInfo(MICROBOARD, 0, COL_VT_HIGH+1, colour, 2, borderX, borderY);
    setComputerInfo(MICROBOARD, "Microboard", "Microboard System", "rca");

    borderX[VIDEOVT] = 0;
    borderY[VIDEOVT] = 0;  //Video Terminal
    borderX[VIDEOXML1870] = 0;
    borderY[VIDEOXML1870] = 0; // CDP 1870
    borderX[VIDEOXMLPIXIE] = 11;
    borderY[VIDEOXMLPIXIE] = 33;  //Pixie
    borderX[VIDEOXML6845] = 0;
    borderY[VIDEOXML6845] = 0;  //6845
    borderX[VIDEOXML6847] = 25;
    borderY[VIDEOXML6847] = 25;  //6847
    borderX[VIDEOXMLTMS] = 32;
    borderY[VIDEOXMLTMS] = 24;  //TMS
    borderX[VIDEOXMLI8275] = 0;
    borderY[VIDEOXMLI8275] = 0;  //i8275
    borderX[VIDEOXML1864] = 0;
    borderY[VIDEOXML1864] = 0;  //1864
    borderX[VIDEOXMLSN76430N] = 0;
    borderY[VIDEOXMLSN76430N] = 0;  //1864
    borderX[VIDEOVIP2K] = 0;
    borderY[VIDEOVIP2K] = 0;  //VIP2K

    colour[COL_PIXIE_FORE] = "#ffffff";    // foreground pixie
    colour[COL_PIXIE_BACK] = "#000000";    // background pixie
    colour[COL_I8275_FORE] = "#00ff00";    // foreground i8275
    colour[COL_I8275_BACK] = "#004000";    // background i8275
    colour[COL_I8275_HIGH] = "#00ff00";    // highlight i8275
    colour[COL_MC6847_TEXT_BLACK] = "#000000";     // background mc6847
    colour[COL_MC6847_TEXT_GREEN] = "#00ff00";     // text green
    colour[COL_MC6847_TEXT_ORANGE] = "#ffc418";    // text orange
    colour[COL_MC6847_GRAPH_GREEN] = "#00ff00";    // graphic Green
    colour[COL_MC6847_GRAPH_YELLOW] = "#ffff00";   // graphic Yellow
    colour[COL_MC6847_GRAPH_BLUE] = "#0000ff";     // graphic Blue
    colour[COL_MC6847_GRAPH_RED] = "#ff0000";      // graphic Red
    colour[COL_MC6847_GRAPH_BUFF] = "#ffffff";     // graphic Buff
    colour[COL_MC6847_GRAPH_CYAN] = "#00ffff";     // graphic Cyan
    colour[COL_MC6847_GRAPH_MAGENTA] = "#ff00ff";  // graphic Magenta
    colour[COL_MC6847_GRAPH_ORANGE] = "#ffc418";   // graphic Orange
    colour[COL_TMS_TRANSPARANT] = "#000000";
    colour[COL_TMS_BLACK] = "#000000";
    colour[COL_TMS_MEDIUM_GREEN] = "#21C842";
    colour[COL_TMS_LIGHT_GREEN] = "#5EDC78";
    colour[COL_TMS_DARK_BLUE] = "#5455ED";
    colour[COL_TMS_LIGHT_BLUE] = "#7D76FC";
    colour[COL_TMS_DARK_RED] = "#D4524D";
    colour[COL_TMS_AQUA_BLUE] = "#42EBF5";
    colour[COL_TMS_MEDIUM_RED] = "#FC5554";
    colour[COL_TMS_LIGHT_RED] = "#FF7978";
    colour[COL_TMS_DARK_YELLOW] = "#D4C154";
    colour[COL_TMS_LIGHT_YELLOW] = "#E6CE80";
    colour[COL_TMS_DARK_GREEN] = "#21B03B";
    colour[COL_TMS_PURPLE] = "#C95BBA";
    colour[COL_TMS_GRAY] = "#CCCCCC";
    colour[COL_TMS_WHITE] = "#ffffff";
    colour[COL_SN76430N_WHITE] = "#ffffff";
    colour[COL_SN76430N_YELLOW] = "#FFFF00";
    colour[COL_SN76430N_GREEN] = "#00FF00";
    colour[COL_SN76430N_RED] = "#FFC0CB";
    colour[COL_SN76430N_BLACK] = "#000000";

    setScreenInfo(XML, 0, COL_MAX, colour, 10, borderX, borderY);
    setComputerInfo(XML, "Xml", "Xml", "");

    borderX[VIDEOVT] = 0;
    borderY[VIDEOVT] = 0;  //Video Terminal
    borderX[VIDEOPIXIE] = 11;
    borderY[VIDEOPIXIE] = 33;  //Pixie
    borderX[VIDEO2KI8275] = 0;
    borderY[VIDEO2KI8275] = 0;  //i8275

    colour[0] = "#ffffff";    // foreground pixie
    colour[1] = "#000000";    // background pixie
    colour[2] = "#00ff00";    // foreground i8275
    colour[3] = "#004000";    // background i8275
    colour[4] = "#00ff00";    // highlight i8275
    setScreenInfo(ELF2K, 0, 5, colour, 3, borderX, borderY);
    setComputerInfo(ELF2K, "Elf2K", "Elf 2000", "");
    setScreenInfo(MS2000, 0, 5, colour, 3, borderX, borderY);
    setComputerInfo(MS2000, "MS2000", "MS2000", "");
    setScreenInfo(MCDS, 0, 5, colour, 3, borderX, borderY);
    setComputerInfo(MCDS, "MCDS", "MCDS", "rca");
    setScreenInfo(MEMBER, 0, 5, colour, 1, borderX, borderY);
    setComputerInfo(MEMBER, "Membership", "Membership Card", "");
    setScreenInfo(CDP18S020, 0, 5, colour, 1, borderX, borderY);
    setComputerInfo(CDP18S020, "CDP18S020", "CDP18S020 Evaluation Kit", "");

    borderX[VIDEOPIXIE] = 8;
    borderY[VIDEOPIXIE] = 32;  //CDP1864

    setScreenInfo(COSMICOS, 0, 5, colour, 2, borderX, borderY);
    setComputerInfo(COSMICOS, "Cosmicos", "Cosmicos", "");

    setScreenInfo(UC1800, 0, 5, colour, 2, borderX, borderY);
    setComputerInfo(UC1800, "UC1800", "Infinite UC1800", "");
    
    setScreenInfo(MICROTUTOR, 0, 5, colour, 2, borderX, borderY);
    setComputerInfo(MICROTUTOR, "Microtutor", "RCA Microtutor", "");
    
    setScreenInfo(MICROTUTOR2, 0, 5, colour, 2, borderX, borderY);
    setComputerInfo(MICROTUTOR2, "Microtutor2", "RCA Microtutor II", "");
    
    colour[5] = "#000000";    // background mc6847
    colour[6] = "#00ff00";    // text green
    colour[7] = "#ffc418";    // text orange
    colour[8] = "#00ff00";    // graphic Green
    colour[9] = "#ffff00";    // graphic Yellow 
    colour[10] = "#0000ff";    // graphic Blue
    colour[11] = "#ff0000";    // graphic Red
    colour[12] = "#ffffff";    // graphic Buff
    colour[13] = "#00ffff";    // graphic Cyan
    colour[14] = "#ff00ff";    // graphic Magenta
    colour[15] = "#ffc418";    // graphic Orange
    colour[16] = "#000000";
    colour[17] = "#000000";
    colour[18] = "#21C842";
    colour[19] = "#5EDC78";
    colour[20] = "#5455ED";
    colour[21] = "#7D76FC";
    colour[22] = "#D4524D";
    colour[23] = "#42EBF5";
    colour[24] = "#FC5554";
    colour[25] = "#FF7978";
    colour[26] = "#D4C154";
    colour[27] = "#E6CE80";
    colour[28] = "#21B03B";
    colour[29] = "#C95BBA";
    colour[30] = "#CCCCCC";
    colour[31] = "#ffffff";

    borderX[VIDEO6845] = 0;
    borderY[VIDEO6845] = 0;  //6845
    borderX[VIDEO6847] = 25;
    borderY[VIDEO6847] = 25;  //6847
    borderX[VIDEOTMS] = 32;
    borderY[VIDEOTMS] = 24;  //TMS
    borderX[VIDEOI8275] = 0;
    borderY[VIDEOI8275] = 0;  //i8275
    borderX[VIDEOPIXIE] = 11;
    borderY[VIDEOPIXIE] = 33;  //Pixie

    setScreenInfo(ELF, 0, 32, colour, 6, borderX, borderY);
    setComputerInfo(ELF, "Elf", "Cosmac Elf", "");

    setScreenInfo(ELFII, 0, 32, colour, 6, borderX, borderY);
    setComputerInfo(ELFII, "ElfII", "Netronics Elf II", "super");

    setScreenInfo(SUPERELF, 0, 32, colour, 6, borderX, borderY);
    setComputerInfo(SUPERELF, "SuperElf", "Quest Super Elf", "super");

    setScreenInfo(PICO, 0, 32, colour, 6, borderX, borderY);
    setComputerInfo(PICO, "Pico", "Pico/Elf V2", "super");

    setScreenInfo(STUDIO, 0, 2, colour, 2, borderX, borderY);
    setComputerInfo(STUDIO, "Studio2", "Studio II", "");
    
    setScreenInfo(COINARCADE, 0, 2, colour, 2, borderX, borderY);
    setComputerInfo(COINARCADE, "CoinArcade", "RCA Video Coin Arcade", "");
    
    setScreenInfo(TMC1800, 0, 2, colour, 2, borderX, borderY);
    setComputerInfo(TMC1800, "TMC1800", "Telmac 1800", "");

    setScreenInfo(VELF, 0, 5, colour, 2, borderX, borderY);
    setComputerInfo(VELF, "Velf", "VELF", "");
    
    borderX[VIDEOPIXIE] = 0;
    borderY[VIDEOPIXIE] = 0;

    setScreenInfo(VIP2K, 0, 5, colour, 2, borderX, borderY);
    setComputerInfo(VIP2K, "Vip2K", "VIP2K Membership Card", "");
    
    borderX[VIDEOPIXIE] = 33;
    borderY[VIDEOPIXIE] = 33;  //Pixie

    setScreenInfo(FRED1, 0, 2, colour, 2, borderX, borderY);
    setComputerInfo(FRED1, "FRED1", "FRED 1", "");
    
    setScreenInfo(FRED1_5, 0, 2, colour, 2, borderX, borderY);
    setComputerInfo(FRED1_5, "FRED1_5", "FRED 1.5", "");
    
    borderX[VIDEOPIXIE] = 8;
    borderY[VIDEOPIXIE] = 32;  //CDP1864

    setScreenInfo(NANO, 0, 2, colour, 2, borderX, borderY);
    setComputerInfo(NANO, "Nano", "Telmac Nano", "");

    borderX[VIDEOPIXIE] = 11;
    borderY[VIDEOPIXIE] = 33;  //Pixie

    colour[0] = "#004000";
    colour[1] = "#70d0ff";
    colour[2] = "#d0ff70";
    colour[3] = "#ff7070";
    setScreenInfo(VISICOM, 0, 4, colour, 2, borderX, borderY);
    setComputerInfo(VISICOM, "Visicom", "Visicom COM-100", "");

    colour[0] = "#141414";
    colour[1] = "#ff0000";
    colour[2] = "#0000ff";
    colour[3] = "#ff00ff";
    colour[4] = "#00ff00";
    colour[5] = "#ffff00";
    colour[6] = "#00ffff";
    colour[7] = "#ffffff";
    colour[8] = "#000080";
    colour[9] = "#000000";
    colour[10] = "#008000";
    colour[11] = "#800000";
    colour[12] = "#00ff00";    // foreground vt
    colour[13] = "#004000";    // background vt
    colour[14] = "#00ff00";    // highlight vt
    setScreenInfo(VIP, 0, 15, colour, 2, borderX, borderY);
    setComputerInfo(VIP, "Vip", "Cosmac VIP", "");

    setScreenInfo(VIPII, 0, 12, colour, 2, borderX, borderY);
    setComputerInfo(VIPII, "VipII", "Cosmac VIP II", "fpb");

    borderX[VIDEOPIXIE] = 8;
    borderY[VIDEOPIXIE] = 32;  //CDP1864

    setScreenInfo(ETI, 0, 12, colour, 2, borderX, borderY);
    setComputerInfo(ETI, "Eti", "HUG1802/ETI-660", "");

    colour[0] = "#ffffff";
    colour[1] = "#ff00ff";
    colour[2] = "#00ffff";
    colour[3] = "#0000ff";
    colour[4] = "#ffff00";
    colour[5] = "#ff0000";
    colour[6] = "#00ff00";
    colour[7] = "#141414";
    setScreenInfo(TMC2000, 0, 12, colour, 2, borderX, borderY);
    setComputerInfo(TMC2000, "TMC2000", "Telmac 2000", "");

    colour[0] = "#141414"; // Black
    colour[1] = "#ff4040"; // Red
    colour[2] = "#4040ff"; // Blue
    colour[3] = "#ff40ff"; // Pink
    colour[4] = "#40ff40"; // Green
    colour[5] = "#ffff40"; // Yellow
    colour[6] = "#40ffff"; // Cyan
    colour[7] = "#ffffff"; // white
    colour[8] = "#000080";
    colour[9] = "#000000";
    colour[10] = "#008000";
    colour[11] = "#800000";
    colour[12] = "#141414"; // Black
    colour[13] = "#AC0324"; // Red
    colour[14] = "#221354"; // Blue
    colour[15] = "#A16976"; // Pink
    colour[16] = "#375E01"; // Green
    colour[17] = "#99B348"; // Yellow
    colour[18] = "#8580C0"; // Cyan
    colour[19] = "#ffffff"; // white
    colour[20] = "#110247";
    colour[21] = "#000000";
    colour[22] = "#284C02";
    colour[23] = "#800000";

    setScreenInfo(VICTORY, 0, 24, colour, 2, borderX, borderY);
    setComputerInfo(VICTORY, "Victory", "Studio III / Victory MPT-02", "");

    colour[0] = "#141414"; // Black
    colour[1] = "#ff4040"; // Red
    colour[2] = "#4040ff"; // Blue
    colour[3] = "#ff40ff"; // Pink
    colour[4] = "#40ff40"; // Green
    colour[5] = "#ffff40"; // Yellow
    colour[6] = "#40ffff"; // Cyan
    colour[7] = "#ffffff"; // white
    colour[8] = "#000000";
    colour[9] = "#a00000";
    colour[10] = "#0000a0";
    colour[11] = "#a000a0";
    colour[12] = "#00a000";
    colour[13] = "#d0d000";
    colour[14] = "#00d0d0";
    colour[15] = "#fbfbfb";

    borderY[VIDEOPIXIE] = 20; 
    setScreenInfo(STUDIOIV, 0, 16, colour, 2, borderX, borderY);
    setComputerInfo(STUDIOIV, "StudioIV", "Studio IV", "tiny");
    
#if defined(__WXMAC__)
    wxFont* defaultFont;
    if (fontSize_ == 11)
    {
        defaultFont = new wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        defaultFont->SetPointSize(fontSize_);
    }
    else
        defaultFont = new wxFont(fontSize_, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#endif

    if (mode_.gui)
    {
        for (int computer = 0; computer < NO_COMPUTER; computer++)
        {
  //          if (computer == XML) // *** to be removed
  //              computer++;
            switch (computer)
            {
                case COMX:
                case PECOM:
                case CIDELSA:
                case ETI:
                case XML:
                    clockText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "Clock:", wxPoint(defaultGuiSize_.x - windowInfo.clockTextCorrectionSingleTabX, defaultGuiSize_.y - windowInfo.clockTextCorrectionSingleTabY));
                    clockTextCtrl[computer] = new FloatEdit(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_CLOCK_TEXTCTRL + computer, "", wxPoint(defaultGuiSize_.x - windowInfo.clockCorrectionSingleTabX, defaultGuiSize_.y - windowInfo.clockCorrectionSingleTabY), wxSize(windowInfo.clockSize, windowInfo.floatHeight));
                    mhzText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "MHz", wxPoint(defaultGuiSize_.x - windowInfo.mhzTextCorrectionSingleTabX, defaultGuiSize_.y - windowInfo.mhzTextCorrectionSingleTabY));
                    stopButton[computer] = new wxButton(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_STOP_BUTTON + computer, "Stop", wxPoint(defaultGuiSize_.x - windowInfo.stopCorrectionSingleTabX, defaultGuiSize_.y - windowInfo.stopCorrectionSingleTabY), wxSize(80, windowInfo.startHeight));
                    startButton[computer] = new wxButton(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_START_BUTTON + computer, "Start", wxPoint(defaultGuiSize_.x - windowInfo.startCorrectionSingleTabX, defaultGuiSize_.y - windowInfo.startCorrectionSingleTabY), wxSize(80, windowInfo.startHeight));
                break;
                    
                default:
                    clockText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "Clock:", wxPoint(defaultGuiSize_.x - windowInfo.clockTextCorrectionX, defaultGuiSize_.y - windowInfo.clockTextCorrectionY));
                    clockTextCtrl[computer] = new FloatEdit(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_CLOCK_TEXTCTRL + computer, "", wxPoint(defaultGuiSize_.x - windowInfo.clockCorrectionX, defaultGuiSize_.y - windowInfo.clockCorrectionY), wxSize(windowInfo.clockSize, windowInfo.floatHeight));
                    mhzText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "MHz", wxPoint(defaultGuiSize_.x - windowInfo.mhzTextCorrectionX, defaultGuiSize_.y - windowInfo.mhzTextCorrectionY));
                    stopButton[computer] = new wxButton(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_STOP_BUTTON + computer, "Stop", wxPoint(defaultGuiSize_.x - windowInfo.stopCorrectionX, defaultGuiSize_.y - windowInfo.stopCorrectionY), wxSize(80, windowInfo.startHeight));
                    startButton[computer] = new wxButton(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_START_BUTTON + computer, "Start", wxPoint(defaultGuiSize_.x - windowInfo.startCorrectionX, defaultGuiSize_.y - windowInfo.startCorrectionY), wxSize(80, windowInfo.startHeight));
                break;
            }
            
#if defined(__WXMAC__)
            clockText[computer]->SetFont(*defaultFont);
            clockTextCtrl[computer]->SetFont(*defaultFont);
            mhzText[computer]->SetFont(*defaultFont);
            startButton[computer]->SetFont(*defaultFont);
            stopButton[computer]->SetFont(*defaultFont);
#endif
            this->Connect(GUI_CLOCK_TEXTCTRL + computer, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(GuiMain::onClock));
            this->Connect(GUI_START_BUTTON + computer, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::onStart));
            this->Connect(GUI_STOP_BUTTON + computer, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::onStop));
            stopButton[computer]->Enable(false);
        }
    }

    for (int computer = 0; computer < NO_COMPUTER; computer++)
    {
  //     if (computer == XML) // *** to be removed
  //         computer++;
       
        conf[computer].configurationInfo_.menuName = "";
        conf[computer].configurationInfo_.subMenuName = "";
        conf[computer].configurationInfo_.fileName = "";
        conf[computer].saveStart_ = 0;
    }
#if defined(__WXMAC__)
    delete defaultFont;
#endif
}

void Main::readConfig()
{
    wxMenuBar *menubarPointer = GetMenuBar();

    int saveSelectedComputer = selectedComputer_;
    configPointer->Read("/Main/Floating_Point_Zoom", &fullScreenFloat_, true);

    readDebugConfig();
    readComxConfig();
    readInitialComxConfig();
    readElf2KConfig();
    readMS2000Config();
    readMcdsConfig();
    readCosmicosConfig();
    readVipConfig(); //**** need to be fixed for windows XP
    readVipIIConfig();
    readVip2KConfig();
    readVelfConfig();
    readCdp18s020Config();
    readCdp18s600Config();
    readElfConfig(ELF, "Elf");
    readElfConfig(ELFII, "ElfII");
    readElfConfig(SUPERELF, "SuperElf");
    readXmlConfig();
    readPicoConfig();
    readMembershipConfig();
    readUc1800Config();
    readMicrotutorConfig();
    readMicrotutor2Config();
    readStudioConfig();
    readCoinArcadeConfig();
    readFred1Config();
    readFred2Config();
    readVisicomConfig();
    readVictoryConfig();
    readStudioIVConfig();
    readCidelsaConfig();
    readTelmacConfig();
    readTMC1800Config();
    readTMC2000Config();
    readNanoConfig();
    readPecomConfig();
    readEtiConfig();

    readComxWindowConfig();
    readElf2KWindowConfig();
    readMS2000WindowConfig();
    readMcdsWindowConfig();
    readCosmicosWindowConfig();
    readVipWindowConfig();
    readVipIIWindowConfig();
    readVip2KWindowConfig();
    readVelfWindowConfig();
    readCdp18s020WindowConfig();
    readCdp18s600WindowConfig();
    readElfWindowConfig(ELF, "Elf");
    readElfWindowConfig(ELFII, "ElfII");
    readElfWindowConfig(SUPERELF, "SuperElf");
    readXmlWindowConfig();
    readPicoWindowConfig();
    readMembershipWindowConfig();
    readUc1800WindowConfig();
    readMicrotutorWindowConfig();
    readMicrotutor2WindowConfig();
    readStudioWindowConfig();
    readCoinArcadeWindowConfig();
    readFred1WindowConfig();
    readFred2WindowConfig();
    readVisicomWindowConfig();
    readVictoryWindowConfig();
    readStudioIVWindowConfig();
    readCidelsaWindowConfig();
    readTelmacWindowConfig();
    readTMC1800WindowConfig();
    readTMC2000WindowConfig();
    readNanoWindowConfig();
    readPecomWindowConfig();
    readEtiWindowConfig();

    selectedComputer_ = saveSelectedComputer;

    if (mode_.window_position_fixed)
        configPointer->Read("/Main/Window_Positions_Fixed", &mode_.window_position_fixed, true);
    else
        nonFixedWindowPosition();

    lapTimeTrigger_ = (int)configPointer->Read("/Main/LapTimeTrigger", (long)LAPTIME_OFF);
    configPointer->Read("/Main/Save_Debug_File", &saveDebugFile_, false);
    configPointer->Read("/Main/Save_On_Exit", &saveOnExit_, true);
    configPointer->Read("/Main/Check_For_Update", &checkForUpdate_, true);
    configPointer->Read("/Main/Use_Num_Pad", &useNumPad_, true);
    configPointer->Read("/Main/Use_Exit_Key", &useExitKey_, false);
    configPointer->Read("/Main/Use_Ctrlv_Key", &useCtrlvKey_, true);
    runPressed_ = false;
    functionKey_[0] = (int)configPointer->Read("/Main/Exit_Key", (long)WXK_ESCAPE);
    functionKey_[1] = (int)configPointer->Read("/Main/Help_Key", (long)WXK_F1);
    functionKey_[2] = (int)configPointer->Read("/Main/Activate_Main_Key", (long)WXK_F2);
    functionKey_[3] = (int)configPointer->Read("/Main/Full_Screen_Key", (long)WXK_F3);
    functionKey_[4] = (int)configPointer->Read("/Main/Printer_Window_Key", (long)WXK_F4);
    functionKey_[5] = (int)configPointer->Read("/Main/Video_Dump_Key", (long)WXK_F5);
    functionKey_[6] = (int)configPointer->Read("/Main/Debug_Mode_Key", (long)WXK_F6);
    functionKey_[7] = (int)configPointer->Read("/Main/Menu_Key", (long)WXK_F7);
    functionKey_[8] = (int)configPointer->Read("/Main/VT_Setup", (long)WXK_F8);
    functionKey_[12] = (int)configPointer->Read("/Main/Start_Reset_Key", (long)WXK_F12);
#ifdef __WXMAC__
    functionKey_[13] = (int)configPointer->Read("/Main/Ctrlv_Key", (long)86);
#else
    functionKey_[13] = (int)configPointer->Read("/Main/Ctrlv_Key", (long)WXK_CONTROL_V);
#endif
    
    wxString cpuTypeString = configPointer->Read("/Main/Cpu_Type", "CDP1805");
    wxString cpuStartupRegistersString = configPointer->Read("/Main/Cpu_StartupRegisters", "StartupRegistersRandom");
    wxString cpuStartupRamString = configPointer->Read("/Main/Cpu_StartupRam", "StartupRamZeroed");
    wxString cpuStartupVideoRamString = configPointer->Read("/Main/Cpu_StartupVideoRam", "StartupVideoRamZeroed");
    keyboardTypeMenuItem_ = configPointer->Read("/Main/Keyboard_Type", "KeyboardUs");
    wxString equalizationString = configPointer->Read("/Main/Equalization", "TV Speaker");

    wxFont* defaultFont;
    if (fontSize_ == 11)
    {
        defaultFont = new wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        defaultFont->SetPointSize(fontSize_);
    }
    else
        defaultFont = new wxFont(fontSize_, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    if (mode_.gui)
    {
        XRCCTRL(*this, "LapTimeTrigger", wxChoice)->SetSelection(lapTimeTrigger_);
        XRCCTRL(*this, "AssSaveDebugFile", wxCheckBox)->SetValue(saveDebugFile_);
        menubarPointer->Check(XRCID(GUISAVEONEXIT), saveOnExit_);
        menubarPointer->Check(XRCID("MI_UpdateCheck"), checkForUpdate_);
        menubarPointer->Check(XRCID("MI_FullScreenFloat"), fullScreenFloat_);
        menubarPointer->Check(XRCID("MI_FixedWindowPosition"), mode_.window_position_fixed);
        menubarPointer->Check(XRCID("MI_NumPad"), useNumPad_);
#if defined(__WXMAC__)
        if (fontSize_ == 11)
#else
        if (fontSize_ == 9)
#endif
            menubarPointer->Check(XRCID("MI_FontSize11"), true);
        else
            menubarPointer->Check(XRCID("MI_FontSize14"), true);

        menubarPointer->Check(XRCID(equalizationString), true);
        menubarPointer->Check(XRCID(cpuTypeString), true);
        menubarPointer->Check(XRCID(cpuStartupRegistersString), true);
        menubarPointer->Check(XRCID(cpuStartupRamString), true);
        menubarPointer->Check(XRCID(cpuStartupVideoRamString), true);
        menubarPointer->Check(XRCID(keyboardTypeMenuItem_), true);
        menubarPointer->SetFont(*defaultFont);
        delete defaultFont;
    }
    
    if (cpuTypeString == "SYSTEM00")
        cpuType_ = SYSTEM00;
    if (cpuTypeString == "CDP1801")
        cpuType_ = CPU1801;
    if (cpuTypeString == "CDP1802")
        cpuType_ = CPU1802;
    if (cpuTypeString == "CDP1804")
        cpuType_ = CPU1804;
    if (cpuTypeString == "CDP1805")
        cpuType_ = CPU1805;

    defaultCpuType_ = cpuType_;

    if (cpuStartupRegistersString == "StartupRegistersZeroed")
        cpuStartupRegisters_ = STARTUP_ZEROED;
    if (cpuStartupRegistersString == "StartupRegistersRandom")
        cpuStartupRegisters_ = STARTUP_RANDOM;
    
    if (cpuStartupRamString == "StartupRamZeroed")
        cpuStartupRam_ = STARTUP_ZEROED;
    if (cpuStartupRamString == "StartupRamRandom")
        cpuStartupRam_ = STARTUP_RANDOM;
    if (cpuStartupRamString == "StartupRamDynamic")
        cpuStartupRam_ = STARTUP_DYNAMIC;
    
    if (cpuStartupVideoRamString == "StartupVideoRamZeroed")
        cpuStartupVideoRam_ = STARTUP_ZEROED;
    if (cpuStartupVideoRamString == "StartupVideoRamRandom")
        cpuStartupVideoRam_ = STARTUP_RANDOM;
    if (cpuStartupVideoRamString == "StartupVideoRamDynamic")
        cpuStartupVideoRam_ = STARTUP_DYNAMIC;
    
    if (keyboardTypeMenuItem_ == "KeyboardDutch")
        keyboardType_ = "dutch";
    if (keyboardTypeMenuItem_ == "KeyboardGerman")
        keyboardType_ = "german";
    if (keyboardTypeMenuItem_ == "KeyboardFrench")
        keyboardType_ = "french";
    if (keyboardTypeMenuItem_ == "KeyboardSwedish")
        keyboardType_ = "swedish";
    if (keyboardTypeMenuItem_ == "KeyboardUs")
        keyboardType_ = "us";
    if (keyboardTypeMenuItem_ == "KeyboardUserDefined")
        keyboardType_ = "user_defined";

    if (equalizationString == "Flat")
    {
        bass_ = 1;
        treble_ = 0;
    }

    if (equalizationString == "Crisp")
    {
        bass_ = 1;
        treble_ = 5;
    }

    if (equalizationString == "Default")
    {
        bass_ = 16;
        treble_ = -8;
    }

    if (equalizationString == "TV Speaker")
    {
        bass_ = 180;
        treble_ = -8;
    }

    if (equalizationString == "Handheld")
    {
        bass_ = 2000;
        treble_ = -47;
    }

    if (mode_.gui)
    {
        long selected_tab = configPointer->Read("/Main/Selected_Cosmac_Tab", 0l);
        if (selected_tab > LASTELFTAB)
            selected_tab = 0;
        XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(selected_tab);
        selected_tab = configPointer->Read("/Main/Selected_Rca_Tab", 0l);
        if (selected_tab > LASTRCATAB)
            selected_tab = 0;
        XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(selected_tab);
        selected_tab = configPointer->Read("/Main/Selected_Studio_Tab", 0l);
        if (selected_tab > LASTSTUDIOTAB)
            selected_tab = 0;
        XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetSelection(selected_tab);
        selected_tab = configPointer->Read("/Main/Selected_Telmac_Tab", 0l);
        if (selected_tab > LASTTELMACTAB)
            selected_tab = 0;
        XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->SetSelection(selected_tab);
        selected_tab = configPointer->Read("/Main/Selected_Debugger_Tab", 0l);
        if (selected_tab > LASTDEBUGGERTAB)
            selected_tab = 0;
        XRCCTRL(*this, "DebuggerChoiceBook", wxNotebook)->SetSelection(selected_tab);

        selected_tab = configPointer->Read("/Main/Selected_Tab", 0l);
        if (selected_tab > DEBUGGERTAB)
            selected_tab = 0;
        XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(selected_tab);
        eventChangeNoteBook();

        int offset = 24;
        wxSize mainWindowSize = this->GetClientSize();
        XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetClientSize(mainWindowSize.x - offset, mainWindowSize.y - offset);
        XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetClientSize(mainWindowSize.x - offset, mainWindowSize.y - offset);
        XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetClientSize(mainWindowSize.x - offset, mainWindowSize.y - offset);
        XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->SetClientSize(mainWindowSize.x - offset, mainWindowSize.y - offset);
        XRCCTRL(*this, "DebuggerChoiceBook", wxNotebook)->SetClientSize(mainWindowSize.x - offset, mainWindowSize.y - offset);

#if defined (__WXMSW__)
        if (windowInfo.operatingSystem != OS_WINDOWS_2000 )
        {
            XRCCTRL(*this, "PanelElf2K", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelCosmicos", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelElf", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelElfII", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelSuperElf", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelMembership", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelVelf", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelUC1800", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelXml", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelPico", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelFRED1", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelFRED1_5", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelMicrotutor", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelMicrotutor2", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelCDP18S020", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelVip", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelVipII", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelMicroboard", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelMCDS", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelMS2000", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelCoinArcade", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelStudio2", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelVictory", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelStudioIV", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelVisicom", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelTMC600", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelTMC1800", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelTMC2000", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelNano", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "Message", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelDirectAssembler", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelProfiler", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelTrace", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelChip8", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelMemory", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
        }
#endif
    }
//    XRCCTRL(*this, "Computer", wxNotebook)->DeletePage(8); // *** to be removed
   
    psaveData_[0] = (int)configPointer->Read("/Main/Psave_Volume", 15l);
    psaveData_[1] = (int)configPointer->Read("/Main/Psave_Bit_Rate", 1l);
    psaveData_[2] = (int)configPointer->Read("/Main/Psave_Bits_Per_Sample", 1l);
    psaveData_[3] = (int)configPointer->Read("/Main/Wav_Conversion_Type", 0l);
    psaveData_[4] = (int)configPointer->Read("/Main/Cassette_Input_Channel", 0l);
    psaveData_[5] = (int)configPointer->Read("/Main/Cassette_Playback_Input", 0l);
    psaveData_[6] = (int)configPointer->Read("/Main/Cassette_Reversed_Polarity", 0l);
    psaveData_[7] = (int)configPointer->Read("/Main/Cassette_Conversion_Type", 1l);
    psaveData_[8] = (int)configPointer->Read("/Main/Cassette_Fred_Threshold_8", 10l);
    psaveData_[9] = (int)configPointer->Read("/Main/Cassette_Fred_Threshold_16", 500l);
    psaveData_[10] = (int)configPointer->Read("/Main/Cassette_Fred_Freq", 58l);
    psaveData_[11] = (int)configPointer->Read("/Main/Cassette_CV_A", 17l);
    psaveData_[12] = (int)configPointer->Read("/Main/Cassette_CV_B", 18l);
    psaveData_[13] = (int)configPointer->Read("/Main/UseXmlThreshold", 1l);
    psaveData_[14] = (int)configPointer->Read("/Main/Cassette_Fred_Threshold_24", 1000l);
}

#if defined(__WXMSW__)
void Main::windowSizeChanged(wxSizeEvent& WXUNUSED(event))
{
    if (xmlLoaded_ && !guiSizeTimerStarted_)
    {
        guiSizeTimeoutPointer->Start(20, wxTIMER_ONE_SHOT);
        guiSizeTimerStarted_ = true;
    }

}
#else
void Main::windowSizeChanged(wxSizeEvent& event)
{
    adjustGuiSize();
    event.Skip();
}
#endif


void Main::adjustGuiSize()
{
    wxSize mainWindowSize = this->GetClientSize();
    int borderSizeX, borderSizeY, borderSizeY2;
    
    if (mainWindowSize.x > 2000)
        mainWindowSize.x = 2000;
    if (mainWindowSize.y > 2000)
        mainWindowSize.y = 2000;
#if defined(__linux__)
    borderSizeX = 8;
    borderSizeY = 8;
    borderSizeY2 = 70;
#endif
#if defined (__WXMSW__)
    borderSizeX = 10;
    borderSizeY = 10;
    borderSizeY2 = 56;
#endif
#if defined(__WXMAC__)
    borderSizeX = 21;
    borderSizeY = 8;
    borderSizeY2 = 72;
#endif
    
    if (xmlLoaded_)
    {
        int fontFactorX = 0, fontFactorY = 0, debugTraceWindowX = 10;
        
#if defined(__WXMAC__)
        if (fontSize_ != 11)
#else
        if (fontSize_ != 9)
#endif
        {
            fontFactorX = 8;
            fontFactorY = 6;
            debugTraceWindowX = 0;
        }
        XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetClientSize(mainWindowSize.x-borderSizeX, mainWindowSize.y-borderSizeY);
        XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetClientSize(mainWindowSize.x-borderSizeX, mainWindowSize.y-borderSizeY);
        XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetClientSize(mainWindowSize.x-borderSizeX, mainWindowSize.y-borderSizeY);
        XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->SetClientSize(mainWindowSize.x-borderSizeX, mainWindowSize.y-borderSizeY);
        XRCCTRL(*this, "DebuggerChoiceBook", wxNotebook)->SetClientSize(mainWindowSize.x-borderSizeX, mainWindowSize.y-borderSizeY);
        
        wxPoint position, positionBreakPointWindow, positionBreakPointWindowText;
        
        position = XRCCTRL(*this, "Message_Window", wxTextCtrl)->GetPosition();
        XRCCTRL(*this, "Message_Window", wxTextCtrl)->SetSize(mainWindowSize.x-position.x-borderSizeX-fontFactorX, mainWindowSize.y-position.y-borderSizeY2-fontFactorY);
        
        position = XRCCTRL(*this, "TraceWindow", wxTextCtrl)->GetPosition();
        XRCCTRL(*this, "TraceWindow", wxTextCtrl)->SetSize(mainWindowSize.x-position.x-borderSizeX-fontFactorX-debugTraceWindowX, mainWindowSize.y-position.y-borderSizeY2-fontFactorY);
        positionBreakPointWindow = XRCCTRL(*this, "BreakPointWindow", wxListCtrl)->GetPosition();
        positionBreakPointWindowText = XRCCTRL(*this, "BreakPointWindowText", wxStaticText)->GetPosition();
        XRCCTRL(*this, "BreakPointWindow", wxListCtrl)->SetSize((position.x-6)/3, mainWindowSize.y-positionBreakPointWindow.y-borderSizeY2-fontFactorY);
        positionBreakPointWindow.x += (position.x/3);
        positionBreakPointWindowText.x = positionBreakPointWindow.x;
        XRCCTRL(*this, "TregWindowText", wxStaticText)->SetPosition(positionBreakPointWindowText);
        XRCCTRL(*this, "TregWindow", wxListCtrl)->SetPosition(positionBreakPointWindow);
        XRCCTRL(*this, "TregWindow", wxListCtrl)->SetSize((position.x-6)/3, mainWindowSize.y-positionBreakPointWindow.y-borderSizeY2-fontFactorY);
        positionBreakPointWindow.x += (position.x/3);
        positionBreakPointWindowText.x = positionBreakPointWindow.x;
        XRCCTRL(*this, "TrapWindowText", wxStaticText)->SetPosition(positionBreakPointWindowText);
        XRCCTRL(*this, "TrapWindow", wxListCtrl)->SetPosition(positionBreakPointWindow);
        XRCCTRL(*this, "TrapWindow", wxListCtrl)->SetSize((position.x-6)/3, mainWindowSize.y-positionBreakPointWindow.y-borderSizeY2-fontFactorY);
        
        position = XRCCTRL(*this, "Chip8TraceWindow", wxTextCtrl)->GetPosition();
        positionBreakPointWindow = XRCCTRL(*this, "Chip8BreakPointWindow", wxListCtrl)->GetPosition();
            XRCCTRL(*this, "Chip8TraceWindow", wxTextCtrl)->SetSize(mainWindowSize.x-position.x-borderSizeX-fontFactorX, mainWindowSize.y-position.y-borderSizeY2-fontFactorY);
            XRCCTRL(*this, "Chip8BreakPointWindow", wxListCtrl)->SetSize(position.x-6, mainWindowSize.y-positionBreakPointWindow.y-borderSizeY2-fontFactorY);
        
        changeNumberOfDebugLines(mainWindowSize.y - borderSizeY2);
    }
}

void Main::onHelp(wxCommandEvent& WXUNUSED(event))
{
    wxSetWorkingDirectory(p_Main->getApplicationDir());
    help_->DisplayContents();
}
/*
wxString Main::downloadString(wxString urlString)
{
    wxURL url(urlString);
    wxString returnString = "";
 
    if(url.GetError()==wxURL_NOERR)
    {
        wxInputStream *in = url.GetInputStream();

        if (in && in->IsOk())
        {
            wxStringOutputStream html_stream(&returnString);
            in->Read(html_stream);
        }
        if (in)
            delete in;
    }
    return returnString;
}*/

wxString Main::downloadString(wxString urlString)
{
    wxString returnString = "";
    wxStringOutputStream html_stream(&returnString);

    wxCurlHTTP http(urlString);

    http.Get(html_stream);
    return returnString;
}

bool Main::checkUpdateEmma()
{
//    wxString version = configPointer->Read("/Main/Version", EMMA_VERSION);
    wxString version;
    version.Printf("%1.2f", EMMA_VERSION);

    latestVersion_ = downloadString("https://www.emma02.hobby-site.com/Emma_02_version.txt");
    
    if (latestVersion_ == "")
        return false;

    int urlStart=latestVersion_.Find("HREF=");
    if (urlStart != wxNOT_FOUND)
    {    
        urlStart+=6;
        latestVersion_ = latestVersion_.Mid(urlStart);
        int urlEnd=latestVersion_.Find((char)34);
        latestVersion_ = latestVersion_.Mid(0, urlEnd);
        latestVersion_ = downloadString(latestVersion_);
    }

    double latestVersionDouble;
    double versionDouble;
    
    if (version.ToDouble(&versionDouble))
    {
        latestVersion_.ToDouble(&latestVersionDouble);
    }
    else
    {
        latestVersion_.Replace(".",",");
        version.Replace(".",",");
        latestVersion_.ToDouble(&latestVersionDouble);
        version.ToDouble(&versionDouble);
    }

    latestVersionDouble = (int)(ceil(latestVersionDouble*100));
    versionDouble = (int)(ceil(versionDouble*100));


    if (versionDouble >= latestVersionDouble || latestVersion_ == "")
        return false;
    else
        return true;
}

bool Main::updateEmma()
{
    p_Main->eventShowMessageBox( "Emma 02 V" + latestVersion_ + " is available, download now?\n",
                               "Emma 02", wxICON_QUESTION  | wxYES_DEFAULT | wxYES_NO);

    if (messageBoxAnswer_ == wxYES)
    {
#if defined(__linux__)
        if (windowInfo.packageDeb)
        {
           if (windowInfo.arm)
              ::wxLaunchDefaultBrowser("https://www.emma02.hobby-site.com/ccount/click.php?id=20"); // arm deb
           else
              ::wxLaunchDefaultBrowser("https://www.emma02.hobby-site.com/ccount/click.php?id=11"); // x86 deb
        }
        else
        {
           if (windowInfo.suse)
           {
              if (windowInfo.arm)
                 ::wxLaunchDefaultBrowser("https://www.emma02.hobby-site.com/ccount/click.php?id=23"); // arm rpm
              else
                 ::wxLaunchDefaultBrowser("https://www.emma02.hobby-site.com/ccount/click.php?id=22"); // x86 rpm
           }
           else
           {
              if (windowInfo.arm)
                 ::wxLaunchDefaultBrowser("https://www.emma02.hobby-site.com/ccount/click.php?id=21"); // arm rpm
              else
                 ::wxLaunchDefaultBrowser("https://www.emma02.hobby-site.com/ccount/click.php?id=13"); // x86 rpm
           }
        }
        return true;
#elif defined(__WXMAC__)
        if (windowInfo.operatingSystem == OS_MAC_PRE_10_10)
            ::wxLaunchDefaultBrowser("https://www.emma02.hobby-site.com/ccount/click.php?id=15"); // 32 bit & pre 10_10
        else
           if (windowInfo.arm)
            ::wxLaunchDefaultBrowser("https://www.emma02.hobby-site.com/ccount/click.php?id=19"); // arm
           else
            ::wxLaunchDefaultBrowser("https://www.emma02.hobby-site.com/ccount/click.php?id=18"); // x86
        return true;
#else
        if (wxIsPlatform64Bit())
            ::wxLaunchDefaultBrowser("https://www.emma02.hobby-site.com/ccount/click.php?id=17"); // windows 64
        else
            ::wxLaunchDefaultBrowser("https://www.emma02.hobby-site.com/ccount/click.php?id=16"); // windows 32
        return true;
#endif
    }
    return false;
}

void Main::onQuit(wxCommandEvent&WXUNUSED(event))
{
    stopComputer();
    if (!computerRunning_)
        Destroy();
    else
        emmaClosing_ = true;
    //    Destroy();
}

void Main::onSaveOnExit(wxCommandEvent&event)
{
    saveOnExit_ = event.IsChecked();
}

void Main::onSaveConfig(wxCommandEvent&WXUNUSED(event))
{
    writeConfig();
}

void Main::buildConfigMenu()
{
    wxMenuBar *menubarPointer = GetMenuBar();
    wxMenu *fileMenu = menubarPointer->GetMenu(0);
    int menuId = fileMenu->FindItem("Save");
    fileMenu->FindItem(menuId, &configurationMenu);
    fileMenu->FindItem(menuId, &configurationDeleteMenu);
    
    int menuItem = GUI_CONFIG_MENU+1;
    int menuItemDelete = GUI_CONFIG_DELETE_MENU+1;
    
    ConfigurationInfo configurationInfo;
    configurationMenuInfoNumber_ = 0;

    if (!wxDir::Exists(iniDir_ + "Configurations"))
    {
        wxDir::Make(iniDir_ + "Configurations");
        reInstall(applicationDirectory_ + "Configurations" + pathSeparator_, iniDir_ + "Configurations" + pathSeparator_, pathSeparator_);
    }
    
    for (int computer=2; computer<NO_COMPUTER; computer++)
    {
        int confComputer = computer;
        if (confComputer == 2)
            confComputer = 0;
  //      if (confComputer == XML) // *** to be removed
  //      {
  //         confComputer++;
  //         computer++;
  //      }
       
        if (confComputer == FRED1_5)
        {
            if (wxDir::Exists(iniDir_ + "Configurations" + pathSeparator_ + "FRED2") && !wxDir::Exists(iniDir_ + "Configurations" + pathSeparator_ + computerInfo[confComputer].gui))
            {
                wxRenameFile(iniDir_ + "Configurations" + pathSeparator_ + "FRED2", iniDir_ + "Configurations" + pathSeparator_ + computerInfo[confComputer].gui);
                reInstall(applicationDirectory_ + "Configurations" + pathSeparator_ + "FRED1_5" + pathSeparator_, iniDir_ + "Configurations" + pathSeparator_ + "FRED1_5" + pathSeparator_, pathSeparator_);
                
                int selectedComputer = selectedComputer_;
                selectedComputer_ = FRED1_5;
                loadComputerConfig("default.ini");
                selectedComputer_ = selectedComputer;
            }
        }
        if (!wxDir::Exists(iniDir_ + "Configurations" + pathSeparator_ + computerInfo[confComputer].gui))
        {
            wxDir::Make(iniDir_ + "Configurations" + pathSeparator_ + computerInfo[confComputer].gui);
            if (wxDir::Exists(applicationDirectory_ + "Configurations" + pathSeparator_ + computerInfo[confComputer].gui))
                reInstall(applicationDirectory_ + "Configurations" + pathSeparator_ + computerInfo[confComputer].gui + pathSeparator_, iniDir_ + "Configurations" + pathSeparator_+ computerInfo[confComputer].gui + pathSeparator_, pathSeparator_);
        }
        
        wxString filename;

        if (!wxDir::Exists(conf[computer].configurationDir_))
            wxDir::Make(conf[computer].configurationDir_);
        
        wxString dirName = conf[computer].configurationDir_;
        wxDir dir (dirName);
        
        conf[computer].configurationMenu = new wxMenu;
        conf[computer].configurationDeleteMenu = new wxMenu;
        conf[computer].configurationSubMenuInfoNumber_ = 0;
        
        wxArrayString filenames;
        size_t numberOfFiles = dir.GetAllFiles(dirName, &filenames, "*.ini", wxDIR_DEFAULT);
        filenames.Sort();

        for (size_t i=0; i<numberOfFiles; i++)
        {
            filename = filenames.Item(i);
        
            wxFileName configFile(filename);
            configurationInfo = getMenuInfo(filename);
            
            if (configurationInfo.subMenuName != "")
            {
                int subMenuFound = -1;
                for (size_t j=0; j<conf[computer].configurationSubMenuInfoNumber_; j++)
                {
                    if (conf[computer].configurationSubMenuInfo_[j].subMenuName == configurationInfo.subMenuName)
                        subMenuFound = (int)j;
                }
                
                if (subMenuFound == -1)
                {
                    conf[computer].configurationSubMenuInfoNumber_++;
                    conf[computer].configurationSubMenuInfo_.resize(conf[computer].configurationSubMenuInfoNumber_);
                    conf[computer].configurationSubMenuInfo_[conf[computer].configurationSubMenuInfoNumber_-1].subMenuName = configurationInfo.subMenuName;
                    conf[computer].configurationSubMenuInfo_[conf[computer].configurationSubMenuInfoNumber_-1].subMenu =  new wxMenu;
                    conf[computer].configurationSubMenuInfo_[conf[computer].configurationSubMenuInfoNumber_-1].deleteSubMenu =  new wxMenu;
                    
                    conf[computer].configurationMenu->AppendSubMenu(conf[computer].configurationSubMenuInfo_[conf[computer].configurationSubMenuInfoNumber_-1].subMenu, configurationInfo.subMenuName);
                    conf[computer].configurationDeleteMenu->AppendSubMenu(conf[computer].configurationSubMenuInfo_[conf[computer].configurationSubMenuInfoNumber_-1].deleteSubMenu, configurationInfo.subMenuName);

                    subMenuFound = (int)conf[computer].configurationSubMenuInfoNumber_-1;
                }
                conf[computer].configurationSubMenuInfo_[subMenuFound].subMenu->Append(menuItem, configurationInfo.menuName);
                conf[computer].configurationSubMenuInfo_[subMenuFound].deleteSubMenu->Append(menuItemDelete, configurationInfo.menuName);
            }
            else
            {
                if (configurationInfo.menuName != "")
                {
                    conf[computer].configurationMenu->Append(menuItem, configurationInfo.menuName);
                    conf[computer].configurationDeleteMenu->Append(menuItemDelete, configurationInfo.menuName);
                }
            }
            
            this->Connect(menuItem++, wxEVT_COMMAND_MENU_SELECTED , wxCommandEventHandler(Main::onConfigMenu) );
            this->Connect(menuItemDelete++, wxEVT_COMMAND_MENU_SELECTED , wxCommandEventHandler(Main::onDeleteConfiguration) );
            configurationMenuInfoNumber_++;
            configurationMenuInfo_.resize(configurationMenuInfoNumber_);
            configurationMenuInfo_[configurationMenuInfoNumber_-1].fileName = configFile.GetFullName();
            configurationMenuInfo_[configurationMenuInfoNumber_-1].menuName = configurationInfo.menuName;
            configurationMenuInfo_[configurationMenuInfoNumber_-1].subMenuName = configurationInfo.subMenuName;
       }
    }
    
    configurationMenuOn_ = true;
    if (selectedComputer_ <= 2)
    {
        configurationMenu->Insert(0, GUI_CONFIG_MENU, "Load", conf[2].configurationMenu);
        configurationDeleteMenu->Insert(2, GUI_CONFIG_DELETE_MENU, "Delete", conf[2].configurationDeleteMenu);
    }
    else
    {
        if (selectedComputer_ < NO_COMPUTER)
        {
            configurationMenu->Insert(0, GUI_CONFIG_MENU, "Load", conf[selectedComputer_].configurationMenu);
            configurationDeleteMenu->Insert(2, GUI_CONFIG_DELETE_MENU, "Delete", conf[selectedComputer_].configurationDeleteMenu);
        }
    }
}

void Main::onConfigMenu(wxCommandEvent& event)
{
    size_t id = event.GetId() - GUI_CONFIG_MENU - 1;
    
    if (id < configurationMenuInfoNumber_)
        loadComputerConfig(configurationMenuInfo_[id].fileName);
}

int Main::saveComputerConfig(ConfigurationInfo configurationInfo, ConfigurationInfo oldConfigurationInfo)
{
    int computer = selectedComputer_;
    if (computer < 2)
        computer = 2;
    
    wxString newMenuNameString, menuTextString = "Item Name: '";
    if (configurationInfo.subMenuName == "")
        newMenuNameString = configurationInfo.menuName;
    else
    {
        menuTextString = "Sub Menu/" +  menuTextString;
        newMenuNameString = configurationInfo.subMenuName + "/" + configurationInfo.menuName;
    }
    
    if (wxFile::Exists(conf[selectedComputer_].configurationDir_ + configurationInfo.fileName))
    {
        int answer = wxMessageBox("Overwrite " + menuTextString + newMenuNameString + "'?", "Confirm Overwrite", wxICON_EXCLAMATION | wxYES_NO);
        if (answer != wxYES)
            return wxNO;
    }
    else
    {
        if (oldConfigurationInfo.fileName != "")
        {
            wxString menuNameString;
            menuTextString = "Item Name: '";
            if (oldConfigurationInfo.subMenuName == "")
                menuNameString = oldConfigurationInfo.menuName;
            else
            {
                menuTextString = "Sub Menu/" +  menuTextString;
                menuNameString = oldConfigurationInfo.subMenuName + "/" + oldConfigurationInfo.menuName;
            }
            
            wxMessageDialog replaceDialog(this, "Replace " + menuTextString + menuNameString + "' with '" + newMenuNameString + "' or create a new menu item?", "Replace or Create New", wxICON_EXCLAMATION | wxYES_NO);
            
            replaceDialog.SetYesNoLabels("Replace", "Create New");
            
            int answer = replaceDialog.ShowModal();
            if (answer == wxID_YES)
                wxRemoveFile(conf[selectedComputer_].configurationDir_ + oldConfigurationInfo.fileName);
    //        if (answer == wxCANCEL)
    //            return wxNO;
        }
    }

    conf[computer].configurationInfo_.fileName = configurationInfo.fileName;
    conf[computer].configurationInfo_.menuName = configurationInfo.menuName;
    conf[computer].configurationInfo_.subMenuName = configurationInfo.subMenuName;
    
#if defined(__linux__) || defined(__WXMAC__)
    wxString appName = "emma_02";
#else
    wxString appName = "Emma 02";
#endif

    wxConfigBase *tempConfigPointer;
    
    tempConfigPointer = configPointer;
    
    wxFileConfig *pConfig = new wxFileConfig(appName, "Marcel van Tongeren", conf[selectedComputer_].configurationDir_ + configurationInfo.fileName);
    wxConfigBase *currentConfigPointer = wxConfigBase::Set(pConfig);
    configPointer = wxConfigBase::Get();

    configPointer->Write("/MenuName", configurationInfo.menuName);
    configPointer->Write("/SubMenuName", configurationInfo.subMenuName);

    switch (selectedComputer_)
    {
        case COMX:
            writeComxDirConfig();
            writeComxConfig();
        break;

        case CIDELSA:
            writeCidelsaDirConfig();
            writeCidelsaConfig();
        break;
            
        case TMC600:
            writeTelmacDirConfig();
            writeTelmacConfig();
        break;
            
        case TMC2000:
            writeTMC2000DirConfig();
            writeTMC2000Config();
        break;
            
        case TMC1800:
            writeTMC1800DirConfig();
            writeTMC1800Config();
        break;
            
        case ETI:
            writeEtiDirConfig();
            writeEtiConfig();
        break;
            
        case NANO:
            writeNanoDirConfig();
            writeNanoConfig();
        break;
            
        case PECOM:
            writePecomDirConfig();
            writePecomConfig();
        break;
            
        case STUDIO:
            writeStudioDirConfig();
            writeStudioConfig();
        break;
            
        case COINARCADE:
            writeCoinArcadeDirConfig();
            writeCoinArcadeConfig();
        break;
            
        case FRED1:
            writeFred1DirConfig();
            writeFred1Config();
        break;
            
        case FRED1_5:
            writeFred2DirConfig();
            writeFred2Config();
        break;
            
        case VISICOM:
            writeVisicomDirConfig();
            writeVisicomConfig();
        break;
            
        case VICTORY:
            writeVictoryDirConfig();
            writeVictoryConfig();
        break;
            
        case STUDIOIV:
            writeStudioIVDirConfig();
            writeStudioIVConfig();
        break;
            
        case VIP:
            writeVipDirConfig();
            writeVipConfig();
        break;
            
        case VIPII:
            writeVipIIDirConfig();
            writeVipIIConfig();
        break;
            
        case VIP2K:
            writeVip2KDirConfig();
            writeVip2KConfig();
        break;
            
        case VELF:
            writeVelfDirConfig();
            writeVelfConfig();
        break;
            
        case CDP18S020:
            writeCdp18s020DirConfig();
            writeCdp18s020Config();
        break;
            
        case MICROBOARD:
            writeCdp18s600DirConfig();
            writeCdp18s600Config();
        break;
            
        case MEMBER:
            writeMembershipDirConfig();
            writeMembershipConfig();
        break;
            
        case UC1800:
            writeUc1800DirConfig();
            writeUc1800Config();
        break;
 
        case MICROTUTOR:
            writeMicrotutorDirConfig();
            writeMicrotutorConfig();
        break;
            
        case MICROTUTOR2:
            writeMicrotutor2DirConfig();
            writeMicrotutor2Config();
        break;
            
        case ELF2K:
            writeElf2KDirConfig();
            writeElf2KConfig();
        break;
            
        case MS2000:
            writeMS2000DirConfig();
            writeMS2000Config();
        break;
            
        case MCDS:
            writeMcdsDirConfig();
            writeMcdsConfig();
        break;

        case COSMICOS:
            writeCosmicosDirConfig();
            writeCosmicosConfig();
        break;
            
        case ELF:
            writeElfDirConfig(ELF, "Elf");
            writeElfDirConfig(ELF, "ElfII");
            writeElfDirConfig(ELF, "SuperElf");
            writeElfConfig(ELF, "Elf");
            writeElfConfig(ELF, "ElfII");
            writeElfConfig(ELF, "SuperElf");
        break;

        case ELFII:
            writeElfDirConfig(ELFII, "Elf");
            writeElfDirConfig(ELFII, "ElfII");
            writeElfDirConfig(ELFII, "SuperElf");
            writeElfConfig(ELFII, "Elf");
            writeElfConfig(ELFII, "ElfII");
            writeElfConfig(ELFII, "SuperElf");
        break;

        case SUPERELF:
            writeElfDirConfig(SUPERELF, "Elf");
            writeElfDirConfig(SUPERELF, "ElfII");
            writeElfDirConfig(SUPERELF, "SuperElf");
            writeElfConfig(SUPERELF, "Elf");
            writeElfConfig(SUPERELF, "ElfII");
            writeElfConfig(SUPERELF, "SuperElf");
        break;

        case XML:
            writeXmlDirConfig();
            writeXmlConfig();
        break;

        case PICO:
            writePicoDirConfig();
            writePicoConfig();
        break;
    }
    configPointer = tempConfigPointer;
    delete pConfig;
    wxConfigBase::Set(currentConfigPointer);
       
    if (selectedComputer_ < NO_COMPUTER && configurationMenuOn_ == true)
    {
        configurationMenu->Remove(GUI_CONFIG_MENU);
        configurationDeleteMenu->Remove(GUI_CONFIG_DELETE_MENU);
    }

    for (int comp=2; comp<NO_COMPUTER; comp++)
    {
 //       if (comp == XML) // *** to be removed
 //          comp++;
        delete conf[comp].configurationMenu;
        delete conf[comp].configurationDeleteMenu;
    }
    
    buildConfigMenu();
    
    return wxYES;
}

ConfigurationInfo Main::getMenuInfo(wxString fileName)
{
    ConfigurationInfo configurationInfo;
#if defined(__linux__) || defined(__WXMAC__)
    wxString appName = "emma_02";
#else
    wxString appName = "Emma 02";
#endif
    
    wxConfigBase *tempConfigPointer;
    
    tempConfigPointer = configPointer;
    
    wxFileConfig *pConfig = new wxFileConfig(appName, "Marcel van Tongeren", fileName);
    wxConfigBase *currentConfigPointer = wxConfigBase::Set(pConfig);
    configPointer = wxConfigBase::Get();
    
    configurationInfo.menuName = configPointer->Read("/MenuName", "");
    configurationInfo.subMenuName = configPointer->Read("/SubMenuName", "");
    
    configPointer = tempConfigPointer;
    delete pConfig;
    wxConfigBase::Set(currentConfigPointer);
    
    return configurationInfo;
}

void Main::loadComputerConfig(wxString fileName)
{
    wxString menuNameFull;
#if defined(__linux__) || defined(__WXMAC__)
    wxString appName = "emma_02";
#else
    wxString appName = "Emma 02";
#endif
    
    wxConfigBase *tempConfigPointer;
    
    tempConfigPointer = configPointer;
    
    wxFileConfig *pConfig = new wxFileConfig(appName, "Marcel van Tongeren", conf[selectedComputer_].configurationDir_ + fileName);
    wxConfigBase *currentConfigPointer = wxConfigBase::Set(pConfig);
    configPointer = wxConfigBase::Get();
    
    if (selectedComputer_ < 2)
    {
        conf[2].configurationInfo_.fileName = fileName;
        conf[2].configurationInfo_.menuName = configPointer->Read("/MenuName", "");
        conf[2].configurationInfo_.subMenuName = configPointer->Read("/SubMenuName", "");
    }
    else
    {
        conf[selectedComputer_].configurationInfo_.fileName = fileName;
        conf[selectedComputer_].configurationInfo_.menuName = configPointer->Read("/MenuName", "");
        conf[selectedComputer_].configurationInfo_.subMenuName = configPointer->Read("/SubMenuName", "");
    }
  
    switch (selectedComputer_)
    {
        case COMX:
            readComxConfig();
        break;
            
        case CIDELSA:
            readCidelsaConfig();
        break;
            
        case TMC600:
            readTelmacConfig();
        break;
            
        case TMC2000:
            readTMC2000Config();
        break;
            
        case TMC1800:
            readTMC1800Config();
        break;
            
        case ETI:
            readEtiConfig();
        break;
            
        case NANO:
            readNanoConfig();
        break;
            
        case PECOM:
            readPecomConfig();
        break;
            
        case STUDIO:
            readStudioConfig();
        break;
            
        case COINARCADE:
            readCoinArcadeConfig();
        break;
            
        case FRED1:
            readFred1Config();
        break;
            
        case FRED1_5:
            readFred2Config();
        break;
            
        case VISICOM:
            readVisicomConfig();
        break;
            
        case VICTORY:
            readVictoryConfig();
        break;
            
        case STUDIOIV:
            readStudioIVConfig();
        break;
            
        case VIP:
            readVipConfig();
        break;
            
        case VIPII:
            readVipIIConfig();
        break;
            
        case VIP2K:
            readVip2KConfig();
        break;
            
        case VELF:
            readVelfConfig();
        break;
            
        case CDP18S020:
            readCdp18s020Config();
        break;
            
        case MICROBOARD:
            readCdp18s600Config();
            
            menuNameFull = conf[selectedComputer_].configurationInfo_.menuName;
            if (conf[selectedComputer_].configurationInfo_.subMenuName != "")
                menuNameFull = conf[selectedComputer_].configurationInfo_.subMenuName + " - " + menuNameFull;

            XRCCTRL(*this, "ConfigTextMicroboard", wxStaticText)->SetLabel(menuNameFull.Left(34));
        break;
            
        case MEMBER:
            readMembershipConfig();
            XRCCTRL(*this, "ClearRamMembership", wxCheckBox)->SetValue(true);
            elfConfiguration[MEMBER].clearRam = true;
        break;
            
        case UC1800:
            readUc1800Config();
        break;
            
        case MICROTUTOR:
            readMicrotutorConfig();
        break;
            
        case MICROTUTOR2:
            readMicrotutor2Config();
        break;
            
        case ELF2K:
            readElf2KConfig();
        break;
            
        case MS2000:
            readMS2000Config();
        break;
            
        case MCDS:
            readMcdsConfig();
        break;

        case COSMICOS:
            readCosmicosConfig();
        break;
            
        case ELF:
            readElfConfig(ELF, "Elf");
        break;
            
        case ELFII:
            readElfConfig(ELFII, "ElfII");
        break;
            
        case SUPERELF:
            readElfConfig(SUPERELF, "SuperElf");
        break;
            
        case XML:
            readXmlConfig();
        break;

        case PICO:
            readPicoConfig();
        break;
    }
    configPointer = tempConfigPointer;
    delete pConfig;
    wxConfigBase::Set(currentConfigPointer);
}

void Main::onClose(wxCloseEvent&event)
{
    stopComputer();
    if (!computerRunning_)
        Destroy();
    else
    {
        event.Veto();
        emmaClosing_ = true;
    }
//    Destroy();
}

void Main::onAbout(wxCommandEvent&WXUNUSED(event))
{
    MyAboutDialog myAboutDialog(this);
     myAboutDialog.ShowModal();
}

void Main::onDataDir(wxCommandEvent&WXUNUSED(event))
{
    DatadirDialog dataDialog(this);
    dataDialog.ShowModal();
}

void Main::onReInstallConfig(wxCommandEvent&WXUNUSED(event))
{
    int answer = wxMessageBox("This will overwrite files in the configuration directory:\n"+iniDir_ + "Configurations" + pathSeparator_+"\n\nContinue to install default configuration files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
    if (answer == wxYES)
    {
        reInstall(applicationDirectory_ + "Configurations" + pathSeparator_, iniDir_ + "Configurations" + pathSeparator_, pathSeparator_);
    }
}

void Main::onReInstallData(wxCommandEvent&WXUNUSED(event))
{
    int answer = wxMessageBox("This will overwrite files in the 1802 software directory:\n"+dataDir_+"\n\nContinue to install default 1802 software files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
    if (answer == wxYES)
    {
        reInstall(applicationDirectory_ + "data" + pathSeparator_, dataDir_, pathSeparator_);
        completedSplashScreen_ = new CompletedSplashScreen(this);
    }
}

void Main::removeRedundantFiles()
{
    bool cpd18s600 = wxDir::Exists(dataDir_ + "CDP18S600");
    bool cpd18s601 = wxDir::Exists(dataDir_ + "CDP18S601");
    bool cpd18s603 = wxDir::Exists(dataDir_ + "CDP18S603A");
    bool cpd18s600conf = wxDir::Exists(iniDir_ + "Configurations" + pathSeparator_ + "CDP18S600");
    bool cpd18s601conf = wxDir::Exists(iniDir_ + "Configurations" + pathSeparator_ + "CDP18S601");
    bool cpd18s603conf = wxDir::Exists(iniDir_ + "Configurations" + pathSeparator_ + "CDP18S603A");

    if (cpd18s600 || cpd18s601 || cpd18s603 || cpd18s600conf || cpd18s601conf || cpd18s603conf)
    {
        wxString message = "Redundant files and directories found:\n";
        if (cpd18s600)
            message = message + dataDir_ + "CDP18S600\n";
        if (cpd18s601)
            message = message + dataDir_ + "CDP18S601\n";
        if (cpd18s603)
            message = message + dataDir_ + "CDP18S603A\n";
        if (cpd18s600conf)
            message = message + iniDir_ + "Configurations" + pathSeparator_ + "CDP18S600\n";
        if (cpd18s601conf)
            message = message + iniDir_ + "Configurations" + pathSeparator_ + "CDP18S601\n";
        if (cpd18s603conf)
            message = message + iniDir_ + "Configurations" + pathSeparator_ + "CDP18S603A\n";

        int answer = wxMessageBox(message+"\nDelete files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
        
        if (answer == wxYES)
        {
            if (cpd18s600)
                deleteDir (dataDir_ + "CDP18S600");
            if (cpd18s601)
                deleteDir (dataDir_ + "CDP18S601");
            if (cpd18s603)
                deleteDir (dataDir_ + "CDP18S603A");
            if (cpd18s600conf)
                deleteDir (iniDir_ + "Configurations" + pathSeparator_ + "CDP18S600");
            if (cpd18s601conf)
                deleteDir (iniDir_ + "Configurations" + pathSeparator_ + "CDP18S601");
            if (cpd18s603conf)
                deleteDir (iniDir_ + "Configurations" + pathSeparator_ + "CDP18S603A");
        }
    }
}

void Main::deleteDir(wxString directory)
{
    wxString fileName;
    bool fileFound;
    
    wxDir *dir;
    
    dir = new wxDir (directory);
    fileFound = dir->GetFirst(&fileName);
    
    while (fileFound)
    {
        wxRemoveFile(directory+pathSeparator_+fileName);
        fileFound = dir->GetNext(&fileName);
    }
    
    wxRmdir (directory);
    delete dir;
}

void Main::reInstall(wxString sourceDir, wxString destinationDir, wxString pathSep)
{
    wxString filename;

    wxFileName destination(destinationDir);
    wxDir dir (sourceDir);
    bool cont = dir.GetFirst(&filename);
    
    while ( cont )
    {
        if (wxDir::Exists(sourceDir + filename))
            filename += pathSep;
        
        wxFileName source(sourceDir + filename);
        
        copyTree(&source, &destination, pathSep);
        cont = dir.GetNext(&filename);
    }
}

void Main::reInstallOnNotFound(int computerType, wxString fileTypeString)
{
    int answer = wxMessageBox(fileTypeString + " file for the " + computerInfo[computerType].name + " is missing,\nre-install all " + computerInfo[computerType].name + " default files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
    if (answer == wxYES)
    {
        p_Main->reInstall(applicationDirectory_ + "data" + pathSeparator_ + computerInfo[computerType].gui + pathSeparator_,  dataDir_ + computerInfo[computerType].gui + pathSeparator_, pathSeparator_);
    }
}

void Main::checkAndReInstallMainRom(int computerType)
{
    if (p_Main->getRomFile(computerType, MAINROM1) == "")
        return;
    
    if (!wxFile::Exists(p_Main->getRomDir(computerType, MAINROM1) + p_Main->getRomFile(computerType, MAINROM1)))
        p_Main->reInstallOnNotFound(computerType, "ROM");
}

void Main::checkAndReInstallFile(int computerType, wxString fileTypeString, int fileType)
{
    if (p_Main->getRomFile(computerType, fileType) == "")
        return;
    
    if (!wxFile::Exists(p_Main->getRomDir(computerType, fileType) + p_Main->getRomFile(computerType, fileType)))
        p_Main->reInstallOnNotFound(computerType, fileTypeString);
}

void Main::checkAndReInstallFile(wxString fileAndPath, int computerType, wxString fileTypeString)
{
    if (fileAndPath == "")
        return;
    
    if (!wxFile::Exists(fileAndPath))
        p_Main->reInstallOnNotFound(computerType, fileTypeString);
}

void Main::checkAndReInstallCharFile(int computerType, wxString fileTypeString, int fileType)
{
    if (p_Main->getRomFile(computerType, fileType) == "")
        return;
    
    if (!wxFile::Exists(p_Main->getCharRomDir(computerType) + p_Main->getCharRomFile(computerType)))
        p_Main->reInstallOnNotFound(computerType, fileTypeString);
}

bool Main::copyTree( wxFileName* source, wxFileName* destination, wxString pathSep)
{
    // Copy file if it isn't a directory.
    if ( ! wxDir::Exists(source->GetFullPath()) )
    {
        if ( ! wxCopyFile(source->GetFullPath(), destination->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + source->GetFullName() ))
            return false;
        
        wxYield ();
        return true;
    }
    else
    {
#if defined (__WXMAC__)
        if( ! wxFileName::Mkdir(destination->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + source->GetDirs()[source->GetDirCount() - 1], 0777, wxPATH_MKDIR_FULL) )
            return false;
#else
        if( ! wxFileName::Mkdir(destination->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + source->GetDirs()[source->GetDirCount() - 1] + pathSep, 0777, wxPATH_MKDIR_FULL) )
            return false;
#endif
    }
    
    // Deal sequentially with each child file & subdir.
    wxDir dir( source->GetPath() );
    if ( !dir.IsOpened() )        return false;
    
    // Go thru the dir, cloning 1 child @ a time.
    wxString filename;
    bool cont = dir.GetFirst( &filename );
    while ( cont )
    {
        wxString childPath = source->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + filename;
        wxString newDestinationPath = destination->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + source->GetDirs()[source->GetDirCount() - 1] + pathSep;

        wxFileName child;
        
        if ( wxDir::Exists(childPath) )
        {
            child.Assign( childPath + pathSep);
        }
        else
        {
            child.Assign( source->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR), filename );
        }
        
        // Make a new wxFileName for child
        wxFileName newDestination( newDestinationPath );
        
        // Clone it by recursion, whether it's a dir or file.
        if ( ! copyTree( &child, &newDestination, pathSep ) )
            // If this fails, bug out.
            return false;
        
        cont = dir.GetNext(&filename);
    } 
    return true;
} 

void Main::onConfiguration(wxCommandEvent&WXUNUSED(event))
{
    if (selectedComputer_ < 2)
    {
        ConfigurationDialog configurationDialog(this, conf[2].configurationInfo_, conf[2].configurationSubMenuInfo_, conf[2].configurationSubMenuInfoNumber_);
        configurationDialog.ShowModal();
    }
    else
    {
        ConfigurationDialog configurationDialog(this, conf[selectedComputer_].configurationInfo_, conf[selectedComputer_].configurationSubMenuInfo_, conf[selectedComputer_].configurationSubMenuInfoNumber_);
        configurationDialog.ShowModal();
    }
}

void Main::onDeleteConfiguration(wxCommandEvent& event)
{
    size_t id = event.GetId() - GUI_CONFIG_DELETE_MENU - 1;
    
    if (id < configurationMenuInfoNumber_)
    {
        int computer = selectedComputer_;
        if (computer < 2)
            computer = 2;
        if (wxFile::Exists(conf[computer].configurationDir_ + configurationMenuInfo_[id].fileName ))
        {
            wxString menuNameString, menuTextString = "Item Name: '";
            if (configurationMenuInfo_[id].subMenuName == "")
                menuNameString = configurationMenuInfo_[id].menuName;
            else
            {
                menuTextString = "Sub Menu/" +  menuTextString;
                menuNameString = configurationMenuInfo_[id].subMenuName + "/" + configurationMenuInfo_[id].menuName;
            }

            int answer = wxMessageBox("Delete " + menuTextString + menuNameString + "'?", "Confirm Delete", wxICON_EXCLAMATION | wxYES_NO);
            if (answer == wxYES)
            {
                wxRemoveFile(conf[computer].configurationDir_ + configurationMenuInfo_[id].fileName);

                if (selectedComputer_ < NO_COMPUTER && configurationMenuOn_ == true)
                {
                    configurationMenu->Remove(GUI_CONFIG_MENU);
                    configurationDeleteMenu->Remove(GUI_CONFIG_DELETE_MENU);
                }
            
                for (int comp=2; comp<NO_COMPUTER; comp++)
                {
      //              if (comp == XML) // *** to be removed
      //                 comp++;
                    delete conf[comp].configurationMenu;
                    delete conf[comp].configurationDeleteMenu;
                }

                buildConfigMenu();
            }
        }
    }
}

void Main::onFunctionKeys(wxCommandEvent&WXUNUSED(event))
{
    FunctionKeyMapDialog functionKeyMapDialog(this);
     functionKeyMapDialog.ShowModal();
    
//    FunctionKeyMapDialog *p_frame;
    
//    p_frame = new FunctionKeyMapDialog("Function and Hot Key Mapping", wxPoint(-1, -1), wxSize(420, 220));

}

void Main::onHome(wxCommandEvent&WXUNUSED(event))
{
    ::wxLaunchDefaultBrowser("http://www.emma02.hobby-site.com/");
}

void Main::onHomeSb(wxCommandEvent&WXUNUSED(event))
{
    ::wxLaunchDefaultBrowser("http://www.comx35.com/superboard.html");
}

void Main::onHomeSbHs(wxCommandEvent&WXUNUSED(event))
{
    ::wxLaunchDefaultBrowser("http://www.comx35.com/highscores.php");
}

void Main::onUpdateCheck(wxCommandEvent&event)
{
    checkForUpdate_ = event.IsChecked();
}

void Main::onUpdateEmma(wxCommandEvent&WXUNUSED(event))
{
    if (!checkUpdateEmma())
    {
        (void)wxMessageBox( "No Emma 02 update found\n",
                            "Emma 02", wxOK );
    }
    else
    {
        if (updateEmma())
            Destroy();
    }
}

void Main::onDefaultSettings(wxCommandEvent&WXUNUSED(event))
{
    setDefaultSettings();
}

void Main::setDefaultSettings()
{
    wxMenuBar *menubarPointer = GetMenuBar();

 /*   for (int i=0; i<5; i++)
    {
        delete baudTextT[i];
        delete baudTextR[i];
        delete baudChoiceT[i];
        delete baudChoiceR[i];
    }*/
    
    configPointer->DeleteAll();
    
    configPointer->Write("/Comx/PlayerName", sbPlayer_);
    configPointer->Write("/Comx/PlayerLocation", sbLocation_);
    configPointer->Write("/Comx/Email", sbEmail_);

    configPointer->Write("/Main/Save_On_Exit", saveOnExit_);
    
    wxString oldVersionString;
    oldVersionString.Printf("%d", (int)(EMMA_VERSION*10000 + EMMA_SUB_VERSION));
    configPointer->Write("/Main/OldVersion", oldVersionString);

    
    if (mode_.gui)
    {
        configPointer->Write("/Main/Selected_Tab", XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->GetSelection());
        configPointer->Write("/Main/Selected_Cosmac_Tab", XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->GetSelection());
        configPointer->Write("/Main/Selected_Rca_Tab", XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->GetSelection());
        configPointer->Write("/Main/Selected_Studio_Tab", XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->GetSelection());
        configPointer->Write("/Main/Selected_Telmac_Tab", XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->GetSelection());
        configPointer->Write("/Main/Selected_Debugger_Tab", XRCCTRL(*this, "DebuggerChoiceBook", wxNotebook)->GetSelection());
        
        if (menubarPointer->IsChecked(XRCID("Flat")))
            configPointer->Write("/Main/Equalization", "Flat");
        if (menubarPointer->IsChecked(XRCID("Crisp")))
            configPointer->Write("/Main/Equalization", "Crisp");
        if (menubarPointer->IsChecked(XRCID("Default")))
            configPointer->Write("/Main/Equalization", "Default");
        if (menubarPointer->IsChecked(XRCID("TV Speaker")))
            configPointer->Write("/Main/Equalization", "TV Speaker");
        if (menubarPointer->IsChecked(XRCID("Handheld")))
            configPointer->Write("/Main/Equalization", "Handheld");
 
        if (menubarPointer->IsChecked(XRCID("CDP1802")))
            configPointer->Write("/Main/Cpu_Type", "CDP1802");
        if (menubarPointer->IsChecked(XRCID("CDP1804")))
            configPointer->Write("/Main/Cpu_Type", "CDP1804");
        if (menubarPointer->IsChecked(XRCID("CDP1805")))
            configPointer->Write("/Main/Cpu_Type", "CDP1805");

        if (menubarPointer->IsChecked(XRCID("StartupRegistersZeroed")))
            configPointer->Write("/Main/Cpu_StartupRegisters", "StartupRegistersZeroed");
        if (menubarPointer->IsChecked(XRCID("StartupRegistersRandom")))
            configPointer->Write("/Main/Cpu_StartupRegisters", "StartupRegistersRandom");
        
        if (menubarPointer->IsChecked(XRCID("StartupRamZeroed")))
            configPointer->Write("/Main/Cpu_StartupRam", "StartupRamZeroed");
        if (menubarPointer->IsChecked(XRCID("StartupRamRandom")))
            configPointer->Write("/Main/Cpu_StartupRam", "StartupRamRandom");
        if (menubarPointer->IsChecked(XRCID("StartupRamDynamic")))
            configPointer->Write("/Main/Cpu_StartupRam", "StartupRamDynamic");
        
        if (menubarPointer->IsChecked(XRCID("StartupVideoRamZeroed")))
            configPointer->Write("/Main/Cpu_StartupVideoRam", "StartupVideoRamZeroed");
        if (menubarPointer->IsChecked(XRCID("StartupVideoRamRandom")))
            configPointer->Write("/Main/Cpu_StartupVideoRam", "StartupVideoRamRandom");
        if (menubarPointer->IsChecked(XRCID("StartupVideoRamDynamic")))
            configPointer->Write("/Main/Cpu_StartupVideoRam", "StartupVideoRamDynamic");
        
        if (menubarPointer->IsChecked(XRCID("KeyboardDutch")))
            configPointer->Write("/Main/Keyboard_Type", "KeyboardDutch");
        if (menubarPointer->IsChecked(XRCID("KeyboardGerman")))
            configPointer->Write("/Main/Keyboard_Type", "KeyboardGerman");
        if (menubarPointer->IsChecked(XRCID("KeyboardFrench")))
            configPointer->Write("/Main/Keyboard_Type", "KeyboardFrench");
        if (menubarPointer->IsChecked(XRCID("KeyboardSwedish")))
            configPointer->Write("/Main/Keyboard_Type", "KeyboardSwedish");
        if (menubarPointer->IsChecked(XRCID("KeyboardUs")))
            configPointer->Write("/Main/Keyboard_Type", "KeyboardUs");
        if (menubarPointer->IsChecked(XRCID("KeyboardUserDefined")))
            configPointer->Write("/Main/Keyboard_Type", "KeyboardUserDefined");
    }
    
    if (!mode_.portable)
    {
        if (dataDir_.Find(applicationDirectory_) != wxNOT_FOUND)
        {
            configPointer->Write("/DataDirRelative", true);

            wxString tempDir = dataDir_;
            tempDir.Replace(applicationDirectory_, "");
            configPointer->Write("/DataDir", tempDir);
        }
        else
        {
            configPointer->Write("/DataDirRelative", false);
            configPointer->Write("/DataDir", dataDir_);
        }
    }

    configPointer->Write("/Main/Window_Positions_Fixed", mode_.window_position_fixed);
    configPointer->Write("/Main/Floating_Point_Zoom", fullScreenFloat_);

    configPointer->Write("/Main/Use_Num_Pad", useNumPad_);
    configPointer->Write("/Main/Check_For_Update", checkForUpdate_);
    
    wxString aliasStr, aliasEmailStr;
    for (size_t i=0; i<numberOfAlias_; i++)
    {
        aliasStr.Printf("/Comx/Alias%d", (int)i);
        aliasEmailStr.Printf("/Comx/AliasEmail%d", (int)i);

        configPointer->Write(aliasStr, alias[i]);
        configPointer->Write(aliasEmailStr, aliasEmail[i]);
    }
    configPointer->Write("/Comx/AliasNumberOf", (unsigned int)numberOfAlias_); 

    readConfig();
}

void Main::onSystem00(wxCommandEvent&WXUNUSED(event))
{
    cpuType_ = SYSTEM00;
    defaultCpuType_ = SYSTEM00;
}

void Main::on1801(wxCommandEvent&WXUNUSED(event))
{
    cpuType_ = CPU1801;
    defaultCpuType_ = CPU1801;
}

void Main::on1802(wxCommandEvent&WXUNUSED(event))
{
    cpuType_ = CPU1802;
    defaultCpuType_ = CPU1802;
}

void Main::on1804(wxCommandEvent&WXUNUSED(event))
{
    cpuType_ = CPU1804;
    defaultCpuType_ = CPU1804;
}

void Main::on1805(wxCommandEvent&WXUNUSED(event))
{
    cpuType_ = CPU1805;
    defaultCpuType_ = CPU1805;
}

void Main::onCpuStartupRegistersZeroed(wxCommandEvent&WXUNUSED(event))
{
    cpuStartupRegisters_ = STARTUP_ZEROED;
}

void Main::onCpuStartupRegistersRandom(wxCommandEvent&WXUNUSED(event))
{
    cpuStartupRegisters_ = STARTUP_RANDOM;
}

void Main::onCpuStartupRamZeroed(wxCommandEvent&WXUNUSED(event))
{
    cpuStartupRam_ = STARTUP_ZEROED;
}

void Main::onCpuStartupRamRandom(wxCommandEvent&WXUNUSED(event))
{
    cpuStartupRam_ = STARTUP_RANDOM;
}

void Main::onCpuStartupRamDynamic(wxCommandEvent&WXUNUSED(event))
{
    cpuStartupRam_ = STARTUP_DYNAMIC;
}

void Main::onCpuStartupVideoRamZeroed(wxCommandEvent&WXUNUSED(event))
{
    cpuStartupVideoRam_ = STARTUP_ZEROED;
}

void Main::onCpuStartupVideoRamRandom(wxCommandEvent&WXUNUSED(event))
{
    cpuStartupVideoRam_ = STARTUP_RANDOM;
}

void Main::onCpuStartupVideoRamDynamic(wxCommandEvent&WXUNUSED(event))
{
    cpuStartupVideoRam_ = STARTUP_DYNAMIC;
}

void Main::onKeyboardDutch(wxCommandEvent&WXUNUSED(event))
{
    if (wxFile::Exists(dataDir_ + "dutch.ini"))
    {
        keyboardType_ = "dutch";
        keyboardTypeMenuItem_ = "KeyboardDutch";
    }
    else
    {
        wxMenuBar *menubarPointer = GetMenuBar();
        menubarPointer->Check(XRCID(keyboardTypeMenuItem_), true);
        wxMessageBox( "Key definition file not found", "Emma 02", wxICON_ERROR | wxOK );
    }
}

void Main::onKeyboardGerman(wxCommandEvent&WXUNUSED(event))
{
    if (wxFile::Exists(dataDir_ + "german.ini"))
    {
        keyboardType_ = "german";
        keyboardTypeMenuItem_ = "KeyboardGerman";
    }
    else
    {
        wxMenuBar *menubarPointer = GetMenuBar();
        menubarPointer->Check(XRCID(keyboardTypeMenuItem_), true);
        wxMessageBox( "Key definition file not found", "Emma 02", wxICON_ERROR | wxOK );
    }
}

void Main::onKeyboardFrench(wxCommandEvent&WXUNUSED(event))
{
    if (wxFile::Exists(dataDir_ + "french.ini"))
    {
        keyboardType_ = "french";
        keyboardTypeMenuItem_ = "KeyboardFrench";
    }
    else
    {
        wxMenuBar *menubarPointer = GetMenuBar();
        menubarPointer->Check(XRCID(keyboardTypeMenuItem_), true);
        wxMessageBox( "Key definition file not found", "Emma 02", wxICON_ERROR | wxOK );
    }
}

void Main::onKeyboardSwedish(wxCommandEvent&WXUNUSED(event))
{
    if (wxFile::Exists(dataDir_ + "swedish.ini"))
    {
        keyboardType_ = "swedish";
        keyboardTypeMenuItem_ = "KeyboardSwedish";
    }
    else
    {
        wxMenuBar *menubarPointer = GetMenuBar();
        menubarPointer->Check(XRCID(keyboardTypeMenuItem_), true);
        wxMessageBox( "Key definition file not found", "Emma 02", wxICON_ERROR | wxOK );
    }
}

void Main::onKeyboardUs(wxCommandEvent&WXUNUSED(event))
{
    if (wxFile::Exists(dataDir_ + "us.ini"))
    {
        keyboardType_ = "us";
        keyboardTypeMenuItem_ = "KeyboardUs";
    }
    else
    {
        wxMenuBar *menubarPointer = GetMenuBar();
        menubarPointer->Check(XRCID(keyboardTypeMenuItem_), true);
        wxMessageBox( "Key definition file not found", "Emma 02", wxICON_ERROR | wxOK );
    }
}

void Main::onKeyboardUserDefined(wxCommandEvent&WXUNUSED(event))
{
    if (wxFile::Exists(dataDir_ + "user_defined.ini"))
    {
        keyboardType_ = "user_defined";
        keyboardTypeMenuItem_ = "KeyboardUserDefined";
    }
    else
    {
        wxMenuBar *menubarPointer = GetMenuBar();
        menubarPointer->Check(XRCID(keyboardTypeMenuItem_), true);
        wxMessageBox( "Key definition file not found", "Emma 02", wxICON_ERROR | wxOK );
    }
}

void Main::onFontSize11(wxCommandEvent&WXUNUSED(event))
{
#if (__WXMAC__)
    fontSize_ = 11;
#else
    fontSize_ = 9;
#endif
    fontSizeString_ = "11";
    configPointer->Write("/Main/ForceGuiSizeReset", true);
}

void Main::onFontSize14(wxCommandEvent&WXUNUSED(event))
{
#if (__WXMAC__)
    fontSize_ = 14;
    fontSizeString_ = "14";
#else
    fontSize_ = 12;
    fontSizeString_ = "12";
#endif
    configPointer->Write("/Main/ForceGuiSizeReset", true);
}

void Main::onFlat(wxCommandEvent&WXUNUSED(event))
{
    bass_ = 1;
    treble_ = 0;
    if (computerRunning_)
        p_Computer->setEqualization(bass_, treble_);
}

void Main::onCrisp(wxCommandEvent&WXUNUSED(event))
{
    bass_ = 1;
    treble_ = 5;
    if (computerRunning_)
        p_Computer->setEqualization(bass_, treble_);
}

void Main::onDefault(wxCommandEvent&WXUNUSED(event))
{
    bass_ = 16;
    treble_ = -8;
    if (computerRunning_)
        p_Computer->setEqualization(bass_, treble_);
}

void Main::onTvSpeaker(wxCommandEvent&WXUNUSED(event))
{
    bass_ = 180;
    treble_ = -8;
    if (computerRunning_)
        p_Computer->setEqualization(bass_, treble_);
}

void Main::onHandheld(wxCommandEvent&WXUNUSED(event))
{
    bass_ = 2000;
    treble_ = -47;
    if (computerRunning_)
        p_Computer->setEqualization(bass_, treble_);
}

void Main::onChar(wxKeyEvent& event)
{
    int keyCode = event.GetKeyCode();
    if (computerRunning_)
        p_Computer->charEvent(keyCode);
}

void Main::onKeyDown(wxKeyEvent& event)
{
    int key = event.GetKeyCode();
    if (key == WXK_UP)
    {
        if (computerRunning_ && selectedComputer_ == DEBUGGER && (debuggerChoice_ == DIRECTASSTAB || debuggerChoice_ == PROFILERTAB))
        {
            if (event.GetModifiers() == wxMOD_SHIFT)
                onAssSpinPageUp();
            else
            {
                assSpinUpScroll();
                directAss();
            }
            return;
        }
    }

    if (key == WXK_DOWN)
    {
        if (computerRunning_ && selectedComputer_ == DEBUGGER && (debuggerChoice_ == DIRECTASSTAB || debuggerChoice_ == PROFILERTAB))
        {
            if (event.GetModifiers() == wxMOD_SHIFT)
                onAssSpinPageDown();
            else
            {
                assSpinDownScroll();
                directAss();
            }
            return;
        }
    }

    if (!checkFunctionKey(event))
        event.Skip();
}

void Main::onWheel(wxMouseEvent& event)
{
    int rot = event.GetWheelRotation ();
    if (rot > 0)
    {
        if (computerRunning_ && selectedComputer_ == DEBUGGER && (debuggerChoice_ == DIRECTASSTAB || debuggerChoice_ == PROFILERTAB))
        {
            if (event.GetModifiers() == wxMOD_SHIFT)
                onAssSpinPageUp();
            else
            {
                assSpinUpScroll();
                directAss();
            }
            return;
        }
    }

    if (rot < 0)
    {
        if (computerRunning_ && selectedComputer_ == DEBUGGER && (debuggerChoice_ == DIRECTASSTAB || debuggerChoice_ == PROFILERTAB))
        {
            if (event.GetModifiers() == wxMOD_SHIFT)
                onAssSpinPageDown();
            else
            {
                assSpinDownScroll();
                directAss();
            }
            return;
        }
    }

    event.Skip();
}

bool Main::checkFunctionKey(wxKeyEvent& event)
{
    if (event.GetModifiers() == wxMOD_SHIFT)
        return false;

    int key = event.GetKeyCode();

    if (key == functionKey_[0] && computerRunning_ && useExitKey_)
    {
        p_Main->stopComputer();
        return true;
    }

    if (key == functionKey_[1])
    {
        wxSetWorkingDirectory(p_Main->getApplicationDir());
        help_->DisplayContents();
        return true;
    }

    if (key == functionKey_[2])
    {
        activateMainWindow();
        return true;
    }

    if (key == functionKey_[3])
    {
        fullScreenMenu();
        return true;
    }

    if (key == functionKey_[4])
    {
        if (computerRunning_)
        {
            switch (runningComputer_)
            {
                case COMX:
                    p_Main->onComxF4();
                break;
                case TMC600:
                case PECOM:
                case MS2000:
                case MCDS:
                    p_Main->onF4();
                break;
                case ELF:
                case ELFII:
                case SUPERELF:
                case VIP:
                case VELF:
                case XML:
                case PICO:
                    if (conf[runningComputer_].printerOn_)
                        p_Main->onF4();
                break;
            }
        }
        return true;
    }

    if (key == functionKey_[5])
    {
        if (computerRunning_)
        {
            if (p_Video[VIDEOMAIN] != NULL)
                p_Video[VIDEOMAIN]->onF5();
            if (p_Vt100[UART1] != NULL)
                p_Vt100[UART1]->onF5();
            if (p_Vt100[UART2] != NULL)
                p_Vt100[UART2]->onF5();
        }
        return true;
    }

    if (key == functionKey_[6])
    {
        p_Main->onFxDebugMode();
        return true;
    }

    if (key == functionKey_[7])
    {
        if (computerRunning_)
            p_Main->popUp();
        return true;
    }

    if (key == functionKey_[12])
    {
        runPressed_ = true;
        if (runningComputer_ == VIPII)
            p_Vip2->runPressed();
        return true;
    }

    if (computerRunning_)
    {
        switch (runningComputer_)
        {
            case UC1800:
                if (key == p_Computer->getInKey1())
                {
                    p_Computer->onInButtonPress();
                    return true;
                }
            break;

            default:
                if (key == p_Computer->getInKey1() || key == p_Computer->getInKey2())
                {
                    p_Computer->onInButtonPress();
                    return true;
                }
            break;
        }
    }

    return false;
}    

void Main::onActivateMainWindow(wxCommandEvent& WXUNUSED(event))
{
    activateMainWindow();
}

void Main::activateMainWindow()
{
    if (computerRunning_)
    {
        if (p_Vt100[UART1] != NULL)
            p_Vt100[UART1]->activateMainWindow();
        if (p_Vt100[UART2] != NULL)
            p_Vt100[UART2]->activateMainWindow();
        else if (p_Video[VIDEOMAIN] != NULL)
            p_Video[VIDEOMAIN]->activateMainWindow();
        else p_Computer->activateMainWindow();
    }
}

void Main::onFullScreenMenu(wxCommandEvent& WXUNUSED(event))
{
    fullScreenMenu();
}

void Main::onUseNumPad(wxCommandEvent& event)
{
    useNumPad_ = event.IsChecked();
}

void Main::fullScreenMenu()
{
    if (computerRunning_)
    {
        if ((p_Video[conf[runningComputer_].videoNumber_] != NULL) && (p_Vt100[UART1] != NULL))
        {
            if (!p_Video[conf[runningComputer_].videoNumber_]->isFullScreenSet() && !p_Vt100[UART1]->isFullScreenSet())
                p_Vt100[UART1]->onF3();
            else if (p_Vt100[UART1]->isFullScreenSet())
            {
                p_Vt100[UART1]->onF3();
                while (p_Vt100[UART1]->isFullScreenSet()) 
                {
                    wxSleep(1);
                }
                p_Video[conf[runningComputer_].videoNumber_]->onF3();
            }
            else
            {
                p_Video[conf[runningComputer_].videoNumber_]->onF3();
            }
        }
        else
        {
            if ((p_Vt100[UART1] != NULL) && (p_Vt100[UART2] != NULL))
            {
                if (!p_Vt100[UART1]->isFullScreenSet() && !p_Vt100[UART2]->isFullScreenSet())
                    p_Vt100[UART1]->onF3();
                else if (p_Vt100[UART1]->isFullScreenSet())
                {
                    p_Vt100[UART1]->onF3();
                    while (p_Vt100[UART1]->isFullScreenSet())
                    {
                        wxSleep(1);
                    }
                    p_Vt100[UART2]->onF3();
                }
                else
                {
                    p_Vt100[UART2]->onF3();
                }
            }
            else
            {
                if (p_Video[conf[runningComputer_].videoNumber_] != NULL)
                    p_Video[conf[runningComputer_].videoNumber_]->onF3();
                if (p_Vt100[UART1] != NULL)
                    p_Vt100[UART1]->onF3();
            }
        }
    }
}

void Main::popUp()
{
    if (runningComputer_ == STUDIO || runningComputer_ == COINARCADE || runningComputer_ == VICTORY || runningComputer_ == STUDIOIV || runningComputer_ == VISICOM || runningComputer_ == CIDELSA)
        return;

    if (popupDialog_ == NULL)
        popupDialog_ = new PopupDialog(this);
    popupDialog_->init();
    popupDialog_->Show(true);
}

void Main::onKeyUp(wxKeyEvent& event)
{
    int key = event.GetKeyCode();
    if (computerRunning_)
    {
        if (key == p_Computer->getInKey1() || key == p_Computer->getInKey2())
        {
            p_Computer->onInButtonRelease();
            return;
        }
    }
    if (key == functionKey_[12])
    {
        runPressed_ = false;
        if (computerRunning_)
            p_Computer->onRun();
        else
            onStart(selectedComputer_);
        return;
    }
    event.Skip();
}

void Main::connectKeyEvent(wxWindow* pclComponent)
{
  if(pclComponent)
  {
    pclComponent->Connect(wxID_ANY,
                          wxEVT_KEY_DOWN,
                          wxKeyEventHandler(Main::onKeyDown),
                          (wxObject*) NULL,
                          this);
    pclComponent->Connect(wxID_ANY,
                          wxEVT_KEY_UP,
                          wxKeyEventHandler(Main::onKeyUp),
                          (wxObject*) NULL,
                          this);

    wxWindowListNode* pclNode = pclComponent->GetChildren().GetFirst();
    while(pclNode)
    {
      wxWindow* pclChild = pclNode->GetData();
      this->connectKeyEvent(pclChild);
     
      pclNode = pclNode->GetNext();
    }
  }
}

void Main::onDefaultWindowPosition(wxCommandEvent&WXUNUSED(event))
{
    fixedWindowPosition();

    this->Move(mainWindowX_, mainWindowY_);
    switch (runningComputer_)
    {
        case COMX:
            p_Comx->Move(conf[COMX].mainX_, conf[COMX].mainY_);
        break;

        case CIDELSA:
            p_Cidelsa->Move(conf[CIDELSA].mainX_, conf[CIDELSA].mainY_);
        break;

        case TMC600:
            p_Tmc600->Move(conf[TMC600].mainX_, conf[TMC600].mainY_);
        break;

        case TMC2000:
            p_Tmc2000->Move(conf[TMC2000].mainX_, conf[TMC2000].mainY_);
        break;

        case TMC1800:
            p_Tmc1800->Move(conf[TMC1800].mainX_, conf[TMC1800].mainY_);
        break;

        case ETI:
            p_Eti->Move(conf[ETI].mainX_, conf[ETI].mainY_);
        break;

        case NANO:
            p_Nano->Move(conf[NANO].mainX_, conf[NANO].mainY_);
        break;

        case PECOM:
            p_Pecom->Move(conf[PECOM].mainX_, conf[PECOM].mainY_);
        break;

        case COINARCADE:
            p_CoinArcade->Move(conf[COINARCADE].mainX_, conf[COINARCADE].mainY_);
        break;
            
        case FRED1:
            p_Fred->moveWindows();
            p_Fred->Move(conf[FRED1].mainX_, conf[FRED1].mainY_);
        break;

        case FRED1_5:
            p_Fred->moveWindows();
            p_Fred->Move(conf[FRED1_5].mainX_, conf[FRED1_5].mainY_);
        break;

        case STUDIO:
            p_Studio2->Move(conf[STUDIO].mainX_, conf[STUDIO].mainY_);
        break;
            
        case VISICOM:
            p_Visicom->Move(conf[VISICOM].mainX_, conf[VISICOM].mainY_);
        break;

        case VICTORY:
            p_Victory->Move(conf[VICTORY].mainX_, conf[VICTORY].mainY_);
        break;

        case STUDIOIV:
            p_StudioIV->Move(conf[STUDIOIV].mainX_, conf[STUDIOIV].mainY_);
        break;
            
        case VIP:
            p_Vip->moveWindows();
            p_Vip->Move(conf[VIP].mainX_, conf[VIP].mainY_);
        break;

        case VIPII:
            p_Vip2->Move(conf[VIPII].mainX_, conf[VIPII].mainY_);
        break;
            
        case VIP2K:
            p_Vip2K->moveWindows();
            p_Vip2K->Move(conf[VIP2K].mainX_, conf[VIP2K].mainY_);
        break;
            
        case VELF:
            p_Velf->moveWindows();
            p_Velf->Move(conf[VELF].mainX_, conf[VELF].mainY_);
        break;
            
        case CDP18S020:
            p_Cdp18s020->moveWindows();
            p_Cdp18s020->Move(conf[CDP18S020].mainX_, conf[CDP18S020].mainY_);
        break;
            
        case MICROBOARD:
            p_Computer->moveWindows();
        break;
            
        case MEMBER:
            p_Membership->moveWindows();
            p_Membership->Move(conf[MEMBER].mainX_, conf[MEMBER].mainY_);
        break;

        case UC1800:
            p_Uc1800->Move(conf[UC1800].mainX_, conf[UC1800].mainY_);
        break;

        case MICROTUTOR:
            p_Microtutor->Move(conf[MICROTUTOR].mainX_, conf[MICROTUTOR].mainY_);
        break;

        case MICROTUTOR2:
            p_Microtutor2->Move(conf[MICROTUTOR2].mainX_, conf[MICROTUTOR2].mainY_);
        break;
            
        case ELF2K:
            p_Elf2K->moveWindows();
            p_Elf2K->Move(conf[ELF2K].mainX_, conf[ELF2K].mainY_);
        break;

        case MS2000:
            p_Ms2000->moveWindows();
            p_Ms2000->Move(conf[MS2000].mainX_, conf[MS2000].mainY_);
        break;
            
        case MCDS:
            p_Mcds->moveWindows();
            p_Mcds->Move(conf[MCDS].mainX_, conf[MCDS].mainY_);
        break;

        case COSMICOS:
            p_Cosmicos->moveWindows();
            p_Cosmicos->Move(conf[COSMICOS].mainX_, conf[COSMICOS].mainY_);
        break;

        case ELF:
            p_Elf->moveWindows();
            p_Elf->Move(conf[ELF].mainX_, conf[ELF].mainY_);
        break;

        case ELFII:
            p_Elf2->moveWindows();
            p_Elf2->Move(conf[ELFII].mainX_, conf[ELFII].mainY_);
        break;

        case SUPERELF:
            p_Super->moveWindows();
            p_Super->Move(conf[SUPERELF].mainX_, conf[SUPERELF].mainY_);
        break;

        case XML:
            p_Xmlemu->moveWindows();
            p_Xmlemu->Move(conf[XML].mainX_, conf[XML].mainY_);
        break;

        case PICO:
            p_Pico->moveWindows();
            p_Pico->Move(conf[PICO].mainX_, conf[PICO].mainY_);
        break;

    }
}

void Main::onDefaultGuiSize(wxCommandEvent& WXUNUSED(event))
{
    this->SetSize(defaultGuiSize_.x, defaultGuiSize_.y);
}

void Main::onFixedWindowPosition(wxCommandEvent&event)
{
    mode_.window_position_fixed = event.IsChecked();
    if (!mode_.window_position_fixed)
        nonFixedWindowPosition();
}

void Main::nonFixedWindowPosition()
{
    mainWindowX_ = -1;
    mainWindowY_ = -1;
    conf[COMX].mainX_ = -1;
    conf[COMX].mainY_ = -1;
    conf[CIDELSA].mainX_ = -1;
    conf[CIDELSA].mainY_ = -1;
    conf[TMC600].mainX_ = -1;
    conf[TMC600].mainY_ = -1;
    conf[VIP].mainX_ = -1;
    conf[VIP].mainY_ = -1;
    conf[VIPII].mainX_ = -1;
    conf[VIPII].mainY_ = -1;
    conf[VIP2K].mainX_ = -1;
    conf[VIP2K].mainY_ = -1;
    conf[VELF].mainX_ = -1;
    conf[VELF].mainY_ = -1;
    conf[CDP18S020].mainX_ = -1;
    conf[CDP18S020].mainY_ = -1;
    conf[MICROBOARD].mainX_ = -1;
    conf[MICROBOARD].mainY_ = -1;
    conf[TMC1800].mainX_ = -1;
    conf[TMC1800].mainY_ = -1;
    conf[TMC2000].mainX_ = -1;
    conf[TMC2000].mainY_ = -1;
    conf[ETI].mainX_ = -1;
    conf[ETI].mainY_ = -1;
    conf[NANO].mainX_ = -1;
    conf[NANO].mainY_ = -1;
    conf[PECOM].mainX_ = -1;
    conf[PECOM].mainY_ = -1;
    conf[STUDIO].mainX_ = -1;
    conf[STUDIO].mainY_ = -1;
    conf[COINARCADE].mainX_ = -1;
    conf[COINARCADE].mainY_ = -1;
    conf[FRED1].mainX_ = -1;
    conf[FRED1].mainY_ = -1;
    conf[FRED1_5].mainX_ = -1;
    conf[FRED1_5].mainY_ = -1;
    conf[VISICOM].mainX_ = -1;
    conf[VISICOM].mainY_ = -1;
    conf[VICTORY].mainX_ = -1;
    conf[VICTORY].mainY_ = -1;
    conf[STUDIOIV].mainX_ = -1;
    conf[STUDIOIV].mainY_ = -1;
    conf[MEMBER].mainX_ = -1;
    conf[MEMBER].mainY_ = -1;
    conf[UC1800].mainX_ = -1;
    conf[UC1800].mainY_ = -1;
    conf[MICROTUTOR].mainX_ = -1;
    conf[MICROTUTOR].mainY_ = -1;
    conf[MICROTUTOR2].mainX_ = -1;
    conf[MICROTUTOR2].mainY_ = -1;

    for (int i=0; i<COSMICOS; i++)
    {
        conf[i].pixieX_ = -1;
        conf[i].pixieY_ = -1;
        conf[i].tmsX_ = -1;
        conf[i].tmsY_ = -1;
        conf[i].mc6845X_ = -1;
        conf[i].mc6845Y_ = -1;
        conf[i].mc6847X_ = -1;
        conf[i].mc6847Y_ = -1;
        conf[i].i8275X_ = -1;
        conf[i].i8275Y_ = -1;
        conf[i].vtX_ = -1;
        conf[i].vtY_ = -1;

        conf[i].mainX_ = -1;
        conf[i].mainY_ = -1;
    }

    conf[XML].keypadX_ = -1;
    conf[XML].keypadY_ = -1;
    conf[XML].v1870X_ = -1;
    conf[XML].v1870Y_ = -1;
    conf[XML].SN76430NX_ = -1;
    conf[XML].SN76430NY_ = -1;
    conf[XML].cdp1864X_ = -1;
    conf[XML].cdp1864Y_ = -1;
    conf[XML].vip2KX_ = -1;
    conf[XML].vip2KY_ = -1;
    conf[ELF2K].keypadX_ = -1;
    conf[ELF2K].keypadY_ = -1;
    conf[COSMICOS].keypadX_ = -1;
    conf[COSMICOS].keypadY_ = -1;
    conf[ELF].keypadX_ = -1;
    conf[ELF].keypadY_ = -1;
    conf[VELF].pixieX_ = -1;
    conf[VELF].pixieY_ = -1;
    conf[VELF].vtX_ = -1;
    conf[VELF].vtY_ = -1;
    conf[CDP18S020].vtX_ = -1;
    conf[CDP18S020].vtY_ = -1;
    conf[MICROBOARD].vtX_ = -1;
    conf[MICROBOARD].vtY_ = -1;
    conf[MICROBOARD].vtUart2X_ = -1;
    conf[MICROBOARD].vtUart2Y_ = -1;
    conf[MICROBOARD].secondFrameX_ = -1;
    conf[MICROBOARD].secondFrameY_ = -1;
    conf[MICROBOARD].v1870X_ = -1;
    conf[MICROBOARD].v1870Y_ = -1;
    conf[MICROBOARD].thirdFrameX_ = -1;
    conf[MICROBOARD].thirdFrameY_ = -1;
    conf[MICROBOARD].fourthFrameX_ = -1;
    conf[MICROBOARD].fourthFrameY_ = -1;
    conf[FRED1].pixieX_ = -1;
    conf[FRED1].pixieY_ = -1;
    conf[FRED1_5].pixieX_ = -1;
    conf[FRED1_5].pixieY_ = -1;
    conf[MS2000].vtX_ = -1;
    conf[MS2000].vtY_ = -1;
    conf[MCDS].vtX_ = -1;
    conf[MCDS].vtY_ = -1;
    conf[VIP].vtX_ = -1;
    conf[VIP].vtY_ = -1;
    conf[VIP2K].vtX_ = -1;
    conf[VIP2K].vtY_ = -1;
    ledModuleX_ = -1;
    ledModuleY_ = -1;
    switchX_ = -1;
    switchY_ = -1;
}

void Main::fixedWindowPosition()
{
    mainWindowX_ = 30 + offsetX_;
    mainWindowY_ = 30 + offsetY_;
    conf[COMX].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[COMX].mainY_ = mainWindowY_;
    conf[COMX].mc6845X_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[COMX].mc6845Y_ = mainWindowY_;
    conf[CIDELSA].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[CIDELSA].mainY_ = mainWindowY_;
    conf[TMC600].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[TMC600].mainY_ = mainWindowY_;
    conf[VIP].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[VIP].mainY_ = mainWindowY_;
    conf[VIPII].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[VIPII].mainY_ = mainWindowY_;
    conf[VIP2K].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[VIP2K].mainY_ = mainWindowY_;
    conf[VELF].mainX_ = mainWindowX_;
    conf[VELF].mainY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
    conf[CDP18S020].mainX_ = mainWindowX_;
    conf[CDP18S020].mainY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
    conf[MICROBOARD].mainX_ = mainWindowX_;
    conf[MICROBOARD].mainY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
    conf[TMC1800].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[TMC1800].mainY_ = mainWindowY_;
    conf[TMC2000].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[TMC2000].mainY_ = mainWindowY_;
    conf[ETI].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[ETI].mainY_ = mainWindowY_;
    conf[NANO].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[NANO].mainY_ = mainWindowY_;
    conf[PECOM].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[PECOM].mainY_ = mainWindowY_;
    conf[STUDIO].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[STUDIO].mainY_ = mainWindowY_;
    conf[COINARCADE].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[COINARCADE].mainY_ = mainWindowY_;
    conf[FRED1].mainX_ = mainWindowX_;
    conf[FRED1].mainY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
    conf[FRED1_5].mainX_ = mainWindowX_;
    conf[FRED1_5].mainY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
    conf[VISICOM].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[VISICOM].mainY_ = mainWindowY_;
    conf[VICTORY].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[VICTORY].mainY_ = mainWindowY_;
    conf[STUDIOIV].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[STUDIOIV].mainY_ = mainWindowY_;
    conf[MEMBER].mainX_ = mainWindowX_;
    conf[MEMBER].mainY_ = mainWindowY_ + windowInfo.mainwY + windowInfo.yBorder;
    conf[UC1800].mainX_ = mainWindowX_;
    conf[UC1800].mainY_ = mainWindowY_ + windowInfo.mainwY + windowInfo.yBorder;
    conf[MICROTUTOR].mainX_ = mainWindowX_;
    conf[MICROTUTOR].mainY_ = mainWindowY_ + windowInfo.mainwY + windowInfo.yBorder;
    conf[MICROTUTOR2].mainX_ = mainWindowX_;
    conf[MICROTUTOR2].mainY_ = mainWindowY_ + windowInfo.mainwY + windowInfo.yBorder;

    for (int i=0; i<COSMICOS; i++)
    {
       if (i == XML)
       {
          conf[i].pixieX_ = conf[i].defPixieX_;
          conf[i].pixieY_ = conf[i].defPixieY_;
          conf[i].tmsX_ = conf[XML].defTmsX_;
          conf[i].tmsY_ = conf[XML].defTmsY_;
          conf[i].mc6845X_ = conf[XML].defMc6845X_;
          conf[i].mc6845Y_ = conf[XML].defMc6845Y_;
          conf[i].mc6847X_ = conf[XML].defMc6847X_;
          conf[i].mc6847Y_ = conf[XML].defMc6847Y_;
          conf[i].i8275X_ = conf[XML].defi8275X_;
          conf[i].i8275Y_ = conf[XML].defi8275Y_;
          conf[i].vtX_ = conf[XML].defVtX_;
          conf[i].vtY_ = conf[XML].defVtY_;

          conf[i].mainX_ = mainWindowX_;
          conf[i].mainY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
       }
       else
       {
           conf[i].pixieX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
           conf[i].pixieY_ = mainWindowY_;
           conf[i].tmsX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
           conf[i].tmsY_ = mainWindowY_;
           conf[i].mc6845X_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
           conf[i].mc6845Y_ = mainWindowY_;
           conf[i].mc6847X_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
           conf[i].mc6847Y_ = mainWindowY_;
           conf[i].i8275X_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
           conf[i].i8275Y_ = mainWindowY_;
           conf[i].vtX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
           conf[i].vtY_ = mainWindowY_;

           conf[i].mainX_ = mainWindowX_;
           conf[i].mainY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
       }
    }

    conf[XML].keypadX_ = mainWindowX_+346+windowInfo.xBorder2;
    conf[XML].keypadY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
    conf[XML].v1870X_ = conf[XML].defv1870X_;
    conf[XML].v1870Y_ = conf[XML].defv1870Y_;
    conf[XML].SN76430NX_ = conf[XML].defSN76430NX_;
    conf[XML].SN76430NY_ = conf[XML].defSN76430NY_;
    conf[XML].cdp1864X_ = conf[XML].defCdp1864X_;
    conf[XML].cdp1864Y_ = conf[XML].defCdp1864Y_;
    conf[XML].vip2KX_ = conf[XML].defVip2KX_;
    conf[XML].vip2KY_ = conf[XML].defVip2KY_;
    conf[ELF2K].keypadX_ = mainWindowX_+507+windowInfo.xBorder2;
    conf[ELF2K].keypadY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
    conf[COSMICOS].keypadX_ = mainWindowX_+333+windowInfo.xBorder2;
    conf[COSMICOS].keypadY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
    conf[ELF].keypadX_ = mainWindowX_+346+windowInfo.xBorder2;
    conf[ELF].keypadY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
    conf[VELF].pixieX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[VELF].pixieY_ = mainWindowY_;
    conf[VELF].vtX_ = mainWindowX_ + windowInfo.mainwX + windowInfo.xBorder;
    conf[VELF].vtY_ = mainWindowY_ + 426 + windowInfo.yBorder;
    conf[CDP18S020].vtX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[CDP18S020].vtY_ = mainWindowY_;
    conf[MEMBER].vtX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[MEMBER].vtY_ = mainWindowY_;
    conf[MICROBOARD].vtX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[MICROBOARD].vtY_ = mainWindowY_;
    conf[MICROBOARD].vtUart2X_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[MICROBOARD].vtUart2Y_ = mainWindowY_+ 530;
    conf[MICROBOARD].secondFrameX_ = mainWindowX_ + 310 + windowInfo.xBorder2;
    conf[MICROBOARD].secondFrameY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
#if defined (__WXMAC__) || (__linux__)
    conf[MICROBOARD].thirdFrameX_ = mainWindowX_ + 620 + windowInfo.xBorder2;
    conf[MICROBOARD].fourthFrameX_ = mainWindowX_ + 930 + windowInfo.xBorder2;
#else
    conf[MICROBOARD].thirdFrameX_ = mainWindowX_ + 639 + windowInfo.xBorder2;
    conf[MICROBOARD].fourthFrameX_ = mainWindowX_ + 968 + windowInfo.xBorder2;
#endif
    conf[MICROBOARD].thirdFrameY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
    conf[MICROBOARD].fourthFrameY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
    conf[MICROBOARD].v1870X_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[MICROBOARD].v1870Y_ = mainWindowY_;
    conf[FRED1].pixieX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[FRED1].pixieY_ = mainWindowY_;
    conf[FRED1_5].pixieX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    conf[FRED1_5].pixieY_ = mainWindowY_;
    conf[MS2000].vtX_ = mainWindowX_ + windowInfo.mainwX + windowInfo.xBorder;
    conf[MS2000].vtY_ = mainWindowY_;
    conf[MCDS].vtX_ = mainWindowX_ + windowInfo.mainwX + windowInfo.xBorder;
    conf[MCDS].vtY_ = mainWindowY_;
    conf[VIP].vtX_ = mainWindowX_ + windowInfo.mainwX + windowInfo.xBorder;
    conf[VIP].vtY_ = mainWindowY_ + 426 + windowInfo.yBorder;
    conf[VIP2K].vtX_ = mainWindowX_ + windowInfo.mainwX + windowInfo.xBorder;
    conf[VIP2K].vtY_ = mainWindowY_ + 426 + windowInfo.yBorder;
    ledModuleX_ = mainWindowX_+346+windowInfo.xBorder2;
    ledModuleY_ = mainWindowY_+windowInfo.mainwY+229+windowInfo.yBorder2;
    switchX_ = mainWindowX_+507+windowInfo.xBorder2;
    switchY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
}

void Main::onStart(wxCommandEvent&WXUNUSED(event))
{
    if (computerRunning_)
    {
        p_Computer->onReset();
        return;
    }
    onStart(selectedComputer_);
}

void Main::onStart(int computer)
{
    double zoom, xScale;
    long ms;
    int comxy;
    int stereo = 1;
    int toneChannels = 1;
    int x, y;

    updateAssPage_ = true;
    updateMemoryPage_ = true;
    emuClosing_ = false;
    thermalEf_ = false;
    statusLedUpdate_ = false;
    slotLedUpdate_ = false;
    chip8Steps_ = -1;
    performChip8Step_ = false;
    additionalChip8Details_ = false;

    for (int video=0; video<VIDEOXMLMAX; video++)
      p_Video[video] = NULL;
    p_Vt100[UART1] = NULL;
    p_Vt100[UART2] = NULL;
    p_Serial = NULL;

    runningComputer_ = computer;
    selectedComputer_ = computer;
#ifndef __WXMAC__
    wxSetWorkingDirectory(workingDir_);
#endif
    if (runningComputer_ != XML)
       conf[runningComputer_].numberOfVideoTypes_ = 0;
    setClock(runningComputer_);
    toDouble(conf[runningComputer_].zoom_[conf[runningComputer_].videoNumber_], &zoom);
    toDouble(conf[runningComputer_].xScale_, &xScale);

    if (!fullScreenFloat_)
        zoom = (int) zoom;

    wxDisplaySize(&x, &y);
    if (conf[runningComputer_].mainX_ >= x)
        conf[runningComputer_].mainX_ = -1;
    if (conf[runningComputer_].mainY_ >= y)
        conf[runningComputer_].mainY_ = -1;

    if (mode_.gui)
       XRCCTRL(*this, "Chip8Type", wxStaticText)->SetLabel("");
   
    wxSize mainWindowSize;
    switch (runningComputer_)
    {
        case COMX:
            p_Main->setComxExpLedOn (false);
            if (conf[COMX].videoMode_ == PAL)
                comxy = 216;
            else
                comxy = 192;
            p_Comx = new Comx(computerInfo[COMX].name, wxPoint(conf[COMX].mainX_, conf[COMX].mainY_), wxSize(240 * zoom, comxy * zoom), zoom, COMX, conf[COMX].clockSpeed_, conf[COMX]);
            p_Video[VIDEOMAIN] = p_Comx;
            conf[COMX].numberOfVideoTypes_ = 1;
            p_Computer = p_Comx;
        break;

        case MEMBER:
            switch (elfConfiguration[MEMBER].frontType)
            {
                case FRONT_TYPE_C:
                case FRONT_TYPE_I:
                case FRONT_TYPE_J:
                    x = 480; y = 299;
                break;
                default:
                    x = 483; y = 297;
                break;
            }
            p_Membership = new Membership(computerInfo[MEMBER].name, wxPoint(conf[MEMBER].mainX_, conf[MEMBER].mainY_), wxSize(x, y), conf[MEMBER].clockSpeed_, elfConfiguration[MEMBER], conf[MEMBER]);
            p_Computer = p_Membership;
        break;

        case UC1800:
            p_Uc1800 = new Uc1800(computerInfo[UC1800].name, wxPoint(conf[UC1800].mainX_, conf[UC1800].mainY_), wxSize(464, 264), conf[UC1800].clockSpeed_, elfConfiguration[UC1800], conf[UC1800]);
            p_Computer = p_Uc1800;
        break;

        case MICROTUTOR:
            p_Microtutor = new Microtutor(computerInfo[MICROTUTOR].name, wxPoint(conf[MICROTUTOR].mainX_, conf[MICROTUTOR].mainY_), wxSize(333, 160), conf[MICROTUTOR].clockSpeed_, elfConfiguration[MICROTUTOR], conf[MICROTUTOR]);
            p_Computer = p_Microtutor;
        break;

        case MICROTUTOR2:
            p_Microtutor2 = new Microtutor2(computerInfo[MICROTUTOR2].name, wxPoint(conf[MICROTUTOR2].mainX_, conf[MICROTUTOR2].mainY_), wxSize(333, 160), conf[MICROTUTOR2].clockSpeed_, elfConfiguration[MICROTUTOR2], conf[MICROTUTOR2]);
            p_Computer = p_Microtutor2;
        break;
            
        case ELF:
            p_Elf = new Elf(computerInfo[ELF].name, wxPoint(conf[ELF].mainX_, conf[ELF].mainY_), wxSize(346, 464), conf[ELF].clockSpeed_, elfConfiguration[ELF], conf[ELF]);
            p_Computer = p_Elf;
        break;

        case ELFII:
            p_Elf2 = new Elf2(computerInfo[ELFII].name, wxPoint(conf[ELFII].mainX_, conf[ELFII].mainY_), wxSize(534, 386), conf[ELFII].clockSpeed_, elfConfiguration[ELFII], conf[ELFII]);
            p_Computer = p_Elf2;
        break;

        case SUPERELF:
            p_Super = new Super(computerInfo[SUPERELF].name, wxPoint(conf[SUPERELF].mainX_, conf[SUPERELF].mainY_), wxSize(534, 386), conf[SUPERELF].clockSpeed_, elfConfiguration[SUPERELF], conf[SUPERELF]);
            p_Computer = p_Super;
        break;

        case XML:
            parseXmlFile(XML,conf[XML].xmlDir_, conf[XML].xmlFile_);
            setXmlGui();

            switch (elfConfiguration[XML].panelType_)
            {
               case PANEL_COSMAC:
                  mainWindowSize = wxSize(346, 464);
               break;
                  
               case PANEL_ELF2K:
                  mainWindowSize = wxSize(507, 459);
               break;
                  
               case PANEL_COSMICOS:
                  mainWindowSize = wxSize(333, 160);
               break;
                  
               case PANEL_MEMBER:
                  switch (elfConfiguration[XML].frontType)
                  {
                      case FRONT_TYPE_C:
                      case FRONT_TYPE_I:
                      case FRONT_TYPE_J:
                          x = 480; y = 299;
                      break;
                      default:
                          x = 483; y = 297;
                      break;
                  }
                  mainWindowSize = wxSize(x, y);
               break;
                  
               case PANEL_MICROTUTOR:
               case PANEL_MICROTUTOR2:
                  mainWindowSize = wxSize(333, 160);
                  p_Xmlemu = new Xmlemu(computerInfo[XML].name, wxPoint(conf[XML].mainX_, conf[XML].mainY_), wxSize(333, 160), conf[XML].clockSpeed_, elfConfiguration[XML], conf[XML]);
               break;
                  
               default:
                  mainWindowSize = wxSize(534, 386);
               break;
            }

           p_Xmlemu = new Xmlemu(computerInfo[XML].name, wxPoint(conf[XML].mainX_, conf[XML].mainY_), mainWindowSize, conf[XML].clockSpeed_, elfConfiguration[XML], conf[XML]);
           p_Computer = p_Xmlemu;
        break;

        case PICO:
            p_Pico = new Pico(computerInfo[PICO].name, wxPoint(conf[PICO].mainX_, conf[PICO].mainY_), wxSize(534, 386), conf[PICO].clockSpeed_, elfConfiguration[PICO], conf[PICO]);
            p_Computer = p_Pico;
        break;

        case ELF2K:
            p_Elf2K = new Elf2K(computerInfo[ELF2K].name, wxPoint(conf[ELF2K].mainX_, conf[ELF2K].mainY_), wxSize(507, 459), conf[ELF2K].clockSpeed_, elfConfiguration[ELF2K], conf[ELF2K]);
            p_Computer = p_Elf2K;
        break;

        case MS2000:
            p_Ms2000 = new Ms2000(computerInfo[MS2000].name, wxPoint(conf[MS2000].mainX_, conf[MS2000].mainY_), wxSize(507, 459), conf[MS2000].clockSpeed_, elfConfiguration[MS2000], conf[MS2000]);
            p_Computer = p_Ms2000;
        break;
            
        case MCDS:
            p_Mcds = new Mcds(computerInfo[MCDS].name, wxPoint(conf[MCDS].mainX_, conf[MCDS].mainY_), wxSize(507, 459), conf[MCDS].clockSpeed_, elfConfiguration[MCDS], conf[MCDS]);
            p_Computer = p_Mcds;
        break;

        case COSMICOS:
            p_Cosmicos = new Cosmicos(computerInfo[COSMICOS].name, wxPoint(conf[COSMICOS].mainX_, conf[COSMICOS].mainY_), wxSize(333, 160), conf[COSMICOS].clockSpeed_, elfConfiguration[COSMICOS], conf[COSMICOS]);
            p_Computer = p_Cosmicos;
        break;

        case STUDIO:
            p_Studio2 = new Studio2(computerInfo[STUDIO].name, wxPoint(conf[STUDIO].mainX_, conf[STUDIO].mainY_), wxSize(64*zoom*xScale, 128*zoom), zoom, xScale, STUDIO, conf[STUDIO]);
            p_Video[VIDEOMAIN] = p_Studio2;
            conf[STUDIO].numberOfVideoTypes_ = 1;
            p_Computer = p_Studio2;
        break;

        case COINARCADE:
            p_CoinArcade = new CoinArcade(computerInfo[COINARCADE].name, wxPoint(conf[COINARCADE].mainX_, conf[COINARCADE].mainY_), wxSize(64*zoom*xScale, 128*zoom), zoom, xScale, COINARCADE, conf[COINARCADE]);
            p_Video[VIDEOMAIN] = p_CoinArcade;
             conf[COINARCADE].numberOfVideoTypes_ = 1;
            p_Computer = p_CoinArcade;
        break;
            
        case FRED1:
            p_Fred = new Fred(computerInfo[FRED1].name, wxPoint(conf[FRED1].mainX_, conf[FRED1].mainY_), wxSize(310,180), conf[FRED1].clockSpeed_, elfConfiguration[FRED1], FRED1, conf[FRED1]);
            p_Computer = p_Fred;
        break;
            
        case FRED1_5:
            p_Fred = new Fred(computerInfo[FRED1_5].name, wxPoint(conf[FRED1_5].mainX_, conf[FRED1_5].mainY_), wxSize(310,180), conf[FRED1_5].clockSpeed_, elfConfiguration[FRED1_5], FRED1_5, conf[FRED1_5]);
            p_Computer = p_Fred;
        break;
            
        case VISICOM:
            p_Visicom = new Visicom(computerInfo[VISICOM].name, wxPoint(conf[VISICOM].mainX_, conf[VISICOM].mainY_), wxSize(64*zoom*xScale, 128*zoom), zoom, xScale, VISICOM, conf[VISICOM]);
            p_Video[VIDEOMAIN] = p_Visicom;
            conf[VISICOM].numberOfVideoTypes_ = 1;
            p_Computer = p_Visicom;
        break;

        case VICTORY:
            p_Victory = new Victory(computerInfo[VICTORY].name, wxPoint(conf[VICTORY].mainX_, conf[VICTORY].mainY_), wxSize(64*zoom*xScale, 192*zoom), zoom, xScale, VICTORY, conf[VICTORY]);
            p_Video[VIDEOMAIN] = p_Victory;
            conf[VICTORY].numberOfVideoTypes_ = 1;
            p_Computer = p_Victory;
        break;

        case STUDIOIV:
            p_StudioIV = new StudioIV(computerInfo[STUDIOIV].name, wxPoint(conf[STUDIOIV].mainX_, conf[STUDIOIV].mainY_), wxSize(64*zoom*xScale, 192*zoom), zoom, xScale, STUDIOIV, conf[STUDIOIV]);
            p_Video[VIDEOMAIN] = p_StudioIV;
            conf[STUDIOIV].numberOfVideoTypes_ = 1;
            p_Computer = p_StudioIV;
        break;
            
        case VIP:
            p_Vip = new Vip(computerInfo[VIP].name, wxPoint(conf[VIP].mainX_, conf[VIP].mainY_), wxSize(64*zoom*xScale, 128*zoom), zoom, xScale, VIP, conf[VIP].clockSpeed_, conf[VIP].tempo_, elfConfiguration[VIP], conf[VIP]);
            p_Video[VIDEOMAIN] = p_Vip;
            conf[VIP].numberOfVideoTypes_ = 1;
            p_Computer = p_Vip;
            if (getVipStereo())
                stereo = 2;
            if (conf[VIP].soundType_ == VIP_SUPER2)
                toneChannels = 2;
            if (conf[VIP].soundType_ == VIP_SUPER4)
                toneChannels = 4;
        break;

        case VIPII:
            p_Vip2 = new VipII(computerInfo[VIPII].name, wxPoint(conf[VIPII].mainX_, conf[VIPII].mainY_), wxSize(64*zoom*xScale, 128*zoom), zoom, xScale, VIPII, conf[VIPII].clockSpeed_, conf[VIPII].tempo_, conf[VIPII]);
            p_Video[VIDEOMAIN] = p_Vip2;
            conf[VIPII].numberOfVideoTypes_ = 1;
            p_Computer = p_Vip2;
        break;

        case VIP2K:
            p_Vip2K = new Vip2K(computerInfo[VIP2K].name, wxPoint(conf[VIP2K].mainX_, conf[VIP2K].mainY_), wxSize(198*xScale, 200), zoom, xScale, VIP2K, conf[VIP2K].clockSpeed_, elfConfiguration[VIP2K], conf[VIP2K]);
            p_Video[VIDEOMAIN] = p_Vip2K;
            conf[VIP2K].numberOfVideoTypes_ = 1;
            p_Computer = p_Vip2K;
        break;
            
        case VELF:
            p_Velf = new Velf(computerInfo[VELF].name, wxPoint(conf[VELF].mainX_, conf[VELF].mainY_), wxSize(310, 180), conf[VELF].clockSpeed_, elfConfiguration[VELF], conf[VELF]);
            p_Computer = p_Velf;
        break;
            
        case CDP18S020:
            p_Cdp18s020 = new Cdp18s020(computerInfo[CDP18S020].name, wxPoint(conf[CDP18S020].mainX_, conf[CDP18S020].mainY_), wxSize(310, 180), conf[CDP18S020].clockSpeed_, elfConfiguration[CDP18S020], conf[CDP18S020]);
            p_Computer = p_Cdp18s020;
        break;
                        
        case MICROBOARD:
            switch (XRCCTRL(*this, "Card1ChoiceMicroboard", wxChoice)->GetSelection())
            {
                case MICROBOARD_CDP18S600:
                    p_Cdp18s600 = new Cdp18s600(computerInfo[MICROBOARD].name, wxPoint(conf[MICROBOARD].v1870X_, conf[MICROBOARD].v1870Y_), wxSize(240 * zoom, 216 * zoom), zoom, MICROBOARD,conf[MICROBOARD].clockSpeed_, elfConfiguration[MICROBOARD], conf[MICROBOARD]);
                    p_Computer = p_Cdp18s600;
                    p_Video[VIDEOMAIN] = p_Cdp18s600;
                break;
                
                case MICROBOARD_CDP18S601:
                case MICROBOARD_CDP18S606:
                    p_Cdp18s601 = new Cdp18s601(computerInfo[MICROBOARD].name, wxPoint(conf[MICROBOARD].v1870X_, conf[MICROBOARD].v1870Y_), wxSize(240 * zoom, 216 * zoom), zoom, MICROBOARD,conf[MICROBOARD].clockSpeed_, elfConfiguration[MICROBOARD], conf[MICROBOARD]);
                    p_Computer = p_Cdp18s601;
                    p_Video[VIDEOMAIN] = p_Cdp18s601;
                break;
                
                case MICROBOARD_CDP18S602:
                case MICROBOARD_CDP18S605:
                case MICROBOARD_CDP18S607:
                case MICROBOARD_CDP18S610:
                    p_Cdp18s602 = new Cdp18s602(computerInfo[MICROBOARD].name, wxPoint(conf[MICROBOARD].v1870X_, conf[MICROBOARD].v1870Y_), wxSize(240 * zoom, 216 * zoom), zoom, MICROBOARD,conf[MICROBOARD].clockSpeed_, elfConfiguration[MICROBOARD], conf[MICROBOARD]);
                    p_Computer = p_Cdp18s602;
                    p_Video[VIDEOMAIN] = p_Cdp18s602;
               break;
                    
                case MICROBOARD_CDP18S603:
                case MICROBOARD_CDP18S603A:
                case MICROBOARD_CDP18S608:
                    p_Cdp18s603a = new Cdp18s603a(computerInfo[MICROBOARD].name, wxPoint(conf[MICROBOARD].v1870X_, conf[MICROBOARD].v1870Y_), wxSize(240 * zoom, 216 * zoom), zoom, MICROBOARD,conf[MICROBOARD].clockSpeed_, elfConfiguration[MICROBOARD], conf[MICROBOARD]);
                    p_Computer = p_Cdp18s603a;
                    p_Video[VIDEOMAIN] = p_Cdp18s603a;
               break;

                case MICROBOARD_CDP18S604B:
                case MICROBOARD_CDP18S609:
                    p_Cdp18s604b = new Cdp18s604b(computerInfo[MICROBOARD].name, wxPoint(conf[MICROBOARD].v1870X_, conf[MICROBOARD].v1870Y_), wxSize(240 * zoom, 216 * zoom), zoom, MICROBOARD,conf[MICROBOARD].clockSpeed_, elfConfiguration[MICROBOARD], conf[MICROBOARD]);
                    p_Computer = p_Cdp18s604b;
                    p_Video[VIDEOMAIN] = p_Cdp18s604b;
               break;

                case RCASBC:
                    p_Rcasbc = new Rcasbc(computerInfo[MICROBOARD].name, wxPoint(conf[MICROBOARD].v1870X_, conf[MICROBOARD].v1870Y_), wxSize(240 * zoom, 216 * zoom), zoom, MICROBOARD,conf[MICROBOARD].clockSpeed_, elfConfiguration[MICROBOARD], conf[MICROBOARD]);
                    p_Computer = p_Rcasbc;
                    p_Video[VIDEOMAIN] = p_Rcasbc;
                break;
            }
            conf[MICROBOARD].numberOfVideoTypes_ = 1;
        break;
            
        case TMC1800:
            p_Tmc1800 = new Tmc1800(computerInfo[TMC1800].name, wxPoint(conf[TMC1800].mainX_, conf[TMC1800].mainY_), wxSize(64*zoom*xScale, 128*zoom), zoom, xScale, TMC1800, conf[TMC1800]);
            p_Video[VIDEOMAIN] = p_Tmc1800;
            conf[TMC1800].numberOfVideoTypes_ = 1;
            p_Computer = p_Tmc1800;
        break;

        case TMC2000:
            p_Tmc2000 = new Tmc2000(computerInfo[TMC2000].name, wxPoint(conf[TMC2000].mainX_, conf[TMC2000].mainY_), wxSize(64*zoom*xScale, 192*zoom), zoom, xScale, TMC2000, conf[TMC2000]);
            p_Video[VIDEOMAIN] = p_Tmc2000;
            conf[TMC2000].numberOfVideoTypes_ = 1;
            p_Computer = p_Tmc2000;
        break;

        case ETI:
            p_Eti = new Eti(computerInfo[ETI].name, wxPoint(conf[ETI].mainX_, conf[ETI].mainY_), wxSize(64*zoom*xScale, 192*zoom), zoom, xScale, ETI, conf[ETI]);
            p_Video[VIDEOMAIN] = p_Eti;
            conf[ETI].numberOfVideoTypes_ = 1;
            p_Computer = p_Eti;
        break;

        case NANO:
            p_Nano = new Nano(computerInfo[NANO].name, wxPoint(conf[NANO].mainX_, conf[NANO].mainY_), wxSize(64*zoom*xScale, 192*zoom), zoom, xScale, NANO, conf[NANO]);
            p_Video[VIDEOMAIN] = p_Nano;
            conf[NANO].numberOfVideoTypes_ = 1;
            p_Computer = p_Nano;
        break;

        case CIDELSA:
            p_Cidelsa = new Cidelsa(computerInfo[CIDELSA].name, wxPoint(conf[CIDELSA].mainX_, conf[CIDELSA].mainY_), wxSize(200*zoom, 240*zoom), zoom, CIDELSA, conf[CIDELSA].clockSpeed_, conf[CIDELSA]);
            p_Video[VIDEOMAIN] = p_Cidelsa;
            conf[CIDELSA].numberOfVideoTypes_ = 1;
            p_Computer = p_Cidelsa;
        break;

        case TMC600:
            p_Tmc600 = new Tmc600(computerInfo[TMC600].name, wxPoint(conf[TMC600].mainX_, conf[TMC600].mainY_), wxSize(240*zoom, 216*zoom), zoom, TMC600, conf[TMC600].clockSpeed_, conf[XML]);
            p_Video[VIDEOMAIN] = p_Tmc600;
            conf[TMC600].numberOfVideoTypes_ = 1;
            p_Computer = p_Tmc600;
        break;

        case PECOM:
            p_Pecom = new Pecom(computerInfo[PECOM].name, wxPoint(conf[PECOM].mainX_, conf[PECOM].mainY_), wxSize(240*zoom, 216*zoom), zoom, PECOM, conf[PECOM].clockSpeed_, conf[PECOM]);
            p_Video[VIDEOMAIN] = p_Pecom;
            conf[PECOM].numberOfVideoTypes_ = 1;
           p_Computer = p_Pecom;
        break;
    }
    
    if (runningComputer_ <= LAST_LED_COMPUTER || runningComputer_ == VIPII || runningComputer_ == FRED1 || runningComputer_ == FRED1_5 )
    {
        conf[runningComputer_].ledTime_.ToLong(&ms);
        conf[runningComputer_].ledTimeMs_ = ms;
    }
    if (mode_.gui)
    {
        XRCCTRL(*this, "Message_Window", wxTextCtrl)->Clear();
        vuPointer->Start(100, wxTIMER_CONTINUOUS);
        directAssPointer->Start(1000, wxTIMER_CONTINUOUS);
#if defined(__WXMAC__) || defined(__linux__)
        traceTimeoutPointer->Start(100, wxTIMER_CONTINUOUS);
#endif
 //       cpuPointer->Start(1000, wxTIMER_CONTINUOUS);
    }

    enableGui(false);
    computerRunning_ = true;
    p_Computer->initCpu(selectedComputer_);
    p_Computer->configureComputer();
    if (!debugMode_)  percentageClock_ = 1;
    if (runningComputer_ <= LAST_ELF_TYPE)
        p_Computer->initSound(conf[runningComputer_].clockSpeed_, percentageClock_, runningComputer_, conf[runningComputer_].volume_, bass_, treble_, toneChannels, stereo, conf[runningComputer_].realCassetteLoad_, conf[runningComputer_].beepFrequency_, elfConfiguration[runningComputer_].bellFrequency_);
    else
        p_Computer->initSound(conf[runningComputer_].clockSpeed_, percentageClock_, runningComputer_, conf[runningComputer_].volume_, bass_, treble_, toneChannels, stereo, conf[runningComputer_].realCassetteLoad_, conf[runningComputer_].beepFrequency_, 250);
    p_Computer->initComputer();
    AssInitConfig();

    if (mode_.gui)
    {
        p_Main->resetDisplay();
#if wxCHECK_VERSION(2, 9, 0)
        XRCCTRL(*this, "RunningComputer", wxStaticText)->SetLabelText("Running computer:  "+computerInfo[runningComputer_].name);
        XRCCTRL(*this, "RunningCpu", wxStaticText)->SetLabelText("Running CPU:  "+cpuName[cpuType_]);
#else
        XRCCTRL(*this, "RunningComputer", wxStaticText)->SetLabel("Running computer:  "+computerInfo[runningComputer_].name);
        XRCCTRL(*this, "RunningCpu", wxStaticText)->SetLabel("Running CPU:  "+cpuName[cpuType_]);
#endif
        assNew(0);
    }
    p_Computer->startComputer();
    if (!mode_.gui)
    {
        if (runningComputer_ < 6)
            if (elfConfiguration[runningComputer_].vtType == VTNONE)
                p_Main->eventVideoSetFullScreen(mode_.full_screen, 0);
            else
                p_Main->eventVtSetFullScreen(mode_.full_screen, UART1);
        else    
            p_Main->eventVideoSetFullScreen(mode_.full_screen, 0);
    }
}

void Main::onStop(wxCommandEvent&WXUNUSED(event))
{
    stopComputer();
}

void Main::stopComputer()
{ 
    stopAssLog();

    if (emuClosing_)  return;
    emuClosing_ = true;
    if (popupDialog_ != NULL)
        delete popupDialog_;
    popupDialog_ = NULL;
    if (mode_.gui)
    {
        directAssPointer->Stop();
        vuPointer->Stop();
//        cpuPointer->Stop();
        switch (runningComputer_)
        {
            case COMX:
                setComxStatusLedOn (false);
                vuSet("Vu"+computerInfo[runningComputer_].gui, 0);
            break;
            case ELF:
            case ELFII:
            case SUPERELF:
            case XML:
            case PICO:
                enableMemAccessGui(false);
                vuSet("Vu"+computerInfo[runningComputer_].gui, 0);
            break;
            case FRED1:
            case FRED1_5:
            case VIP:
            case VIP2K:
            case VIPII:
            case VELF:
            case COSMICOS:
            case TMC600:
            case TMC1800:
            case TMC2000:
            case NANO:
            case PECOM:
            case ETI:
            case STUDIOIV:
                vuSet("Vu"+computerInfo[runningComputer_].gui, 0);
            break;
            case UC1800:
                p_Computer->resumeComputer();
            break;
            case NO_COMPUTER:
                return;
            break;
        }
#if wxCHECK_VERSION(2, 9, 0)
        XRCCTRL(*this, "RunningComputer", wxStaticText)->SetLabelText("Last computer: "+computerInfo[runningComputer_].name);
        XRCCTRL(*this, "RunningCpu", wxStaticText)->SetLabelText("Last CPU:  "+cpuName[cpuType_]);
#else
        XRCCTRL(*this, "RunningComputer", wxStaticText)->SetLabel("Last computer: "+computerInfo[runningComputer_].name);
        XRCCTRL(*this, "RunningCpu", wxStaticText)->SetLabel("Last CPU:  "+cpuName[cpuType_]);
#endif
        showTime();
    }
}

void Main::killComputer(wxCommandEvent&WXUNUSED(event))
{
    if (saveDebugFile_)
        saveAll();

    delete p_Computer;
    p_Computer = NULL;
    p_Video[VIDEOMAIN] = NULL;
    p_Vt100[UART1] = NULL;
    p_Vt100[UART2] = NULL;
    p_Serial = NULL;
    computerRunning_ = false;
    enableGui(true);
    runningComputer_ = NO_COMPUTER;
    if (emmaClosing_ || !mode_.gui)
        Destroy();
}

void Main::onComputer(wxNotebookEvent&event)
{
    wxMenuBar *menubarPointer = GetMenuBar();
    menubarPointer->Enable(XRCID(GUISAVECOMPUTERCONFIG), true);

    if (selectedComputer_ < NO_COMPUTER && configurationMenuOn_ == true)
    {
        configurationMenu->Delete(GUI_CONFIG_MENU);
        configurationDeleteMenu->Delete(GUI_CONFIG_DELETE_MENU);
    }
    
    switch(event.GetSelection())
    {
        case COMXTAB:
            selectedComputer_ = COMX;
        break;

        case COSMACELFTAB:
            switch(XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->GetSelection())
            {
                case ELF2KTAB:
                    elfChoice_ = ELF2K;
                break;
                    
                case COSMICOSTAB:
                    elfChoice_ = COSMICOS; 
                break;

                case ELFTAB:
                    elfChoice_ = ELF; 
                break;

                case ELFIITAB:
                    elfChoice_ = ELFII;
                break;

                case SUPERELFTAB:
                    elfChoice_ = SUPERELF;
                break;

                case MEMBERTAB:
                    elfChoice_ = MEMBER;
                break;
                    
                case VIP2KTAB:
                    elfChoice_ = VIP2K;
                break;
                    
                case VELFTAB:
                    elfChoice_ = VELF;
                break;

                case UC1800TAB:
                    elfChoice_ = UC1800;
                break;

                case PICOTAB:
                    elfChoice_ = PICO;
                break;

            }
            selectedComputer_ = elfChoice_;
        break;

        case RCATAB:
            switch (XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->GetSelection())
            {
                case MICROTUTORTAB:
                    rcaChoice_ = MICROTUTOR;
                break;

                case MICROTUTOR2TAB:
                    rcaChoice_ = MICROTUTOR2;
                break;
                    
                case FRED1TAB:
                    rcaChoice_ = FRED1;
                break;
                    
                case FRED2TAB:
                    rcaChoice_ = FRED1_5;
                break;
                    
                case VIPTAB:
                    rcaChoice_ = VIP;
                break;

                case VIPIITAB:
                    rcaChoice_ = VIPII;
                break;

                case CDP18S020TAB:
                    rcaChoice_ = CDP18S020;
                break;
                    
                case MICROBOARDTAB:
                    rcaChoice_ = MICROBOARD;
                break;
                    
                case MCDSTAB:
                    rcaChoice_ = MCDS;
                break;
                    
                case MS2000TAB:
                    rcaChoice_ = MS2000;
                break;
            }
            selectedComputer_ = rcaChoice_;
        break;

        case STUDIOTAB:
            switch(XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->GetSelection())
            {
                case COINARCADETAB:
                    studioChoice_ = COINARCADE;
                break;
                    
                case STUDIOIITAB:
                    studioChoice_ = STUDIO;
                break;
                    
                case VICTORYTAB:
                    studioChoice_ = VICTORY;
                break;

                case STUDIOIVTAB:
                    studioChoice_ = STUDIOIV;
                break;
                    
                case VISICOMTAB:
                    studioChoice_ = VISICOM;
                break;
                    
                default:
                    studioChoice_ = STUDIO;
                break;
            }
            selectedComputer_ = studioChoice_;
        break;

        case CIDELSATAB:
            selectedComputer_ = CIDELSA;
        break;

        case TELMACTAB:
            switch(XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->GetSelection())
            {
                case TMC600TAB:
                    telmacChoice_ = TMC600;
                break;

                case TMC1800TAB:
                    telmacChoice_ = TMC1800;
                break;

                case TMC2000TAB:
                    telmacChoice_ = TMC2000;
                break;

                case NANOTAB:
                    telmacChoice_ = NANO;
                break;
            }
            selectedComputer_ = telmacChoice_;
        break;

        case PECOMTAB:
            selectedComputer_ = PECOM;
        break;

        case ETITAB:
            selectedComputer_ = ETI;
        break;

        case XMLTAB:
            selectedComputer_ = XML;
        break;

        case DEBUGGERTAB:
            menubarPointer->Enable(XRCID(GUISAVECOMPUTERCONFIG), false);
            switch(XRCCTRL(*this, "DebuggerChoiceBook", wxNotebook)->GetSelection())
            {
                case MESSAGETAB:
                    debuggerChoice_ = MESSAGETAB;
                break;

                case TRACETAB:
                    debuggerChoice_ = TRACETAB;
                break;

                case CHIP8TAB:
                    debuggerChoice_ = CHIP8TAB;
                break;

                case MEMORYTAB:
                    debuggerChoice_ = MEMORYTAB;
                    memoryDisplay();
                break;

                case PROFILERTAB:
                    debuggerChoice_ = PROFILERTAB;
                    directAss();
                break;

                case DIRECTASSTAB:
                    debuggerChoice_ = DIRECTASSTAB;
                    directAss();
                break;

            }
            selectedComputer_ = DEBUGGER;
        break;
    }
    
    setConfigurationMenu();
}

void Main::onStudioChoiceBook(wxChoicebookEvent&event)
{
    if (selectedComputer_ < NO_COMPUTER && configurationMenuOn_ == true)
    {
        configurationMenu->Delete(GUI_CONFIG_MENU);
        configurationDeleteMenu->Delete(GUI_CONFIG_DELETE_MENU);
    }
    
    switch(event.GetSelection())
    {
        case COINARCADETAB:
            studioChoice_ = COINARCADE;
        break;

        case STUDIOIITAB:
            studioChoice_ = STUDIO;
        break;
            
        case VICTORYTAB:
            studioChoice_ = VICTORY;
        break;

        case STUDIOIVTAB:
            studioChoice_ = STUDIOIV;
            selectedComputer_ = studioChoice_;
            if (guiInitialized_)
            {
                vuSet("Vu"+computerInfo[selectedComputer_].gui, 1);
                vuSet("Vu"+computerInfo[selectedComputer_].gui, 0);
            }
        break;

        case VISICOMTAB:
            studioChoice_ = VISICOM;
        break;
    }
    selectedComputer_ = studioChoice_;
    setConfigurationMenu();
}

void Main::onTelmacChoiceBook(wxChoicebookEvent&event)
{
    if (selectedComputer_ < NO_COMPUTER && configurationMenuOn_ == true)
    {
        configurationMenu->Delete(GUI_CONFIG_MENU);
        configurationDeleteMenu->Delete(GUI_CONFIG_DELETE_MENU);
    }
    
    switch(event.GetSelection())
    {
        case TMC600TAB:
            telmacChoice_ = TMC600;
        break;

        case TMC1800TAB:
            telmacChoice_ = TMC1800;
        break;

        case TMC2000TAB:
            telmacChoice_ = TMC2000;
        break;

        case NANOTAB:
            telmacChoice_ = NANO;
        break;
    }
    selectedComputer_ = telmacChoice_;
    if (guiInitialized_)
    {
        vuSet("Vu"+computerInfo[selectedComputer_].gui, 1);
        vuSet("Vu"+computerInfo[selectedComputer_].gui, 0);
    }
    setConfigurationMenu();
}

void Main::onElfChoiceBook(wxChoicebookEvent&event)
{
    if (selectedComputer_ < NO_COMPUTER && configurationMenuOn_ == true)
    {
        configurationMenu->Delete(GUI_CONFIG_MENU);
        configurationDeleteMenu->Delete(GUI_CONFIG_DELETE_MENU);
    }

    switch (event.GetSelection())
    {
        case ELF2KTAB:
            elfChoice_ = ELF2K;
        break;

        case COSMICOSTAB:
            elfChoice_ = COSMICOS;
        break;

        case ELFTAB:
            elfChoice_ = ELF;
        break;

        case ELFIITAB:
            elfChoice_ = ELFII;
        break;

        case SUPERELFTAB:
            elfChoice_ = SUPERELF;
        break;

        case MEMBERTAB:
            elfChoice_ = MEMBER;
        break;

        case VIP2KTAB:
            elfChoice_ = VIP2K;
        break;
            
        case VELFTAB:
            elfChoice_ = VELF;
        break;

        case UC1800TAB:
            elfChoice_ = UC1800;
        break;

        case PICOTAB:
            elfChoice_ = PICO;
        break;
    }
    selectedComputer_ = elfChoice_;
    if (guiInitialized_)
    {
        switch (selectedComputer_)
        {
            case COSMICOS:
            case ELF:
            case ELFII:
            case SUPERELF:
            case VIP2K:
            case VELF:
            case XML:
            case PICO:
                vuSet("Vu"+computerInfo[selectedComputer_].gui, 1);
                vuSet("Vu"+computerInfo[selectedComputer_].gui, 0);
            break;
        }
    }
    setConfigurationMenu();
}

void Main::onRcaChoiceBook(wxChoicebookEvent&event)
{
    if (selectedComputer_ < NO_COMPUTER && configurationMenuOn_ == true)
    {
        configurationMenu->Delete(GUI_CONFIG_MENU);
        configurationDeleteMenu->Delete(GUI_CONFIG_DELETE_MENU);
    }

    switch (event.GetSelection())
    {
        case MICROTUTORTAB:
            rcaChoice_ = MICROTUTOR;
        break;

        case MICROTUTOR2TAB:
            rcaChoice_ = MICROTUTOR2;
        break;
            
        case FRED1TAB:
            rcaChoice_ = FRED1;
        break;
            
        case FRED2TAB:
            rcaChoice_ = FRED1_5;
        break;
            
        case VIPTAB:
            rcaChoice_ = VIP;
        break;

        case VIPIITAB:
            rcaChoice_ = VIPII;
        break;

        case CDP18S020TAB:
            rcaChoice_ = CDP18S020;
        break;
            
        case MICROBOARDTAB:
            rcaChoice_ = MICROBOARD;
        break;
            
        case MCDSTAB:
            rcaChoice_ = MCDS;
        break;
            
        case MS2000TAB:
            rcaChoice_ = MS2000;
        break;
    }
    selectedComputer_ = rcaChoice_;
    if (guiInitialized_)
    {
        switch (selectedComputer_)
        {
            case FRED1:
            case FRED1_5:
            case VIP:
            case VIPII:
                vuSet("Vu"+computerInfo[selectedComputer_].gui, 1);
                vuSet("Vu"+computerInfo[selectedComputer_].gui, 0);
            break;
        }
    }
    setConfigurationMenu();
}

void Main::onDebuggerChoiceBook(wxNotebookEvent&event)
{
    switch(event.GetSelection())
    {
        case MESSAGETAB:
            debuggerChoice_ = MESSAGETAB;
        break;

        case TRACETAB:
            debuggerChoice_ = TRACETAB;
        break;

        case CHIP8TAB:
            debuggerChoice_ = CHIP8TAB;
            if (computerRunning_ && pseudoLoaded_)
                updateChip8Window();
        break;

        case MEMORYTAB:
            debuggerChoice_ = MEMORYTAB;
            memoryDisplay();
        break;

        case PROFILERTAB:
            debuggerChoice_ = PROFILERTAB;
            directAss();
        break;

        case DIRECTASSTAB:
            debuggerChoice_ = DIRECTASSTAB;
            directAss();
        break;
    }
}

void Main::setConfigurationMenu()
{
    if (configurationMenuOn_ == true)
    {
        if (selectedComputer_ <= 2)
        {
            configurationMenu->Insert(0, GUI_CONFIG_MENU, "Load", conf[2].configurationMenu);
            configurationDeleteMenu->Insert(2, GUI_CONFIG_DELETE_MENU, "Delete", conf[2].configurationDeleteMenu);
        }
        else
        {
            if (selectedComputer_ < NO_COMPUTER)
            {
                configurationMenu->Insert(0, GUI_CONFIG_MENU, "Load", conf[selectedComputer_].configurationMenu);
                configurationDeleteMenu->Insert(2, GUI_CONFIG_DELETE_MENU, "Delete", conf[selectedComputer_].configurationDeleteMenu);
            }
        }
    }
}

void Main::setNoteBook()
{
    if (!mode_.gui || runningComputer_ == selectedComputer_)
        return;

    switch (runningComputer_)
    {
        case COMX:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COMXTAB);
        break;

        case ELF2K:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COSMACELFTAB);
            XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(ELF2KTAB);
        break;

        case VELF:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COSMACELFTAB);
            XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(VELFTAB);
        break;
            
        case COSMICOS:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COSMACELFTAB);
            XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(COSMICOSTAB);
        break;

        case ELF:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COSMACELFTAB);
            XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(ELFTAB);
        break;

        case ELFII:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COSMACELFTAB);
            XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(ELFIITAB);
        break;

        case SUPERELF:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COSMACELFTAB);
            XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(SUPERELFTAB);
        break;

        case XML:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COSMACELFTAB);
            XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(XMLTAB);
        break;

        case PICO:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COSMACELFTAB);
            XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(PICOTAB);
        break;

        case MEMBER:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COSMACELFTAB);
            XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(MEMBERTAB);
        break;

        case VIP2K:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COSMACELFTAB);
            XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(VIP2KTAB);
        break;
            
        case UC1800:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COSMACELFTAB);
            XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(UC1800TAB);
        break;

        case MICROTUTOR:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
            XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(MICROTUTORTAB);
        break;

        case MICROTUTOR2:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
            XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(MICROTUTOR2TAB);
        break;
            
        case FRED1:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
            XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(FRED1TAB);
        break;
            
        case FRED1_5:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
            XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(FRED2TAB);
        break;
            
        case VIP:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
            XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(VIPTAB);
        break;

        case VIPII:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
            XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(VIPIITAB);
        break;

        case CDP18S020:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
            XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(CDP18S020TAB);
        break;
            
        case MICROBOARD:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
            XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(MICROBOARDTAB);
        break;
            
        case MCDS:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
            XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(MCDSTAB);
        break;
            
        case MS2000:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
            XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(MS2000TAB);
        break;

        case STUDIO:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(STUDIOTAB);
            XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetSelection(STUDIOIITAB);
        break;

        case COINARCADE:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(STUDIOTAB);
            XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetSelection(COINARCADETAB);
        break;
            
        case VISICOM:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(STUDIOTAB);
            XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetSelection(VISICOMTAB);
        break;

        case VICTORY:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(STUDIOTAB);
            XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetSelection(VICTORYTAB);
        break;

        case STUDIOIV:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(STUDIOTAB);
            XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetSelection(STUDIOIVTAB);
        break;
            
        case CIDELSA:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(CIDELSATAB);
        break;

        case TMC600:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(TELMACTAB);
            XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->SetSelection(TMC600TAB);
        break;

        case TMC1800:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(TELMACTAB);
            XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->SetSelection(TMC1800TAB);
        break;

        case TMC2000:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(TELMACTAB);
            XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->SetSelection(TMC2000TAB);
        break;

        case NANO:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(TELMACTAB);
            XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->SetSelection(NANOTAB);
        break;

        case PECOM:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(PECOMTAB);
        break;

        case ETI:
            XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(ETITAB);
        break;
    }
}

void Main::enableColorbutton(bool status)
{
    XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status | (runningComputer_ == COMX));
    XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status | (runningComputer_ == ELF2K));
    XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status | (runningComputer_ == COSMICOS));
    XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status | (runningComputer_ == ELF));
    XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status | (runningComputer_ == ELFII));
    XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status | (runningComputer_ == SUPERELF));
    XRCCTRL(*this,"ColoursPico", wxButton)->Enable(status | (runningComputer_ == PICO));
    XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status | (runningComputer_ == MEMBER));
    XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status | (runningComputer_ == VELF));
    XRCCTRL(*this,"ColoursCDP18S020", wxButton)->Enable(status | (runningComputer_ == CDP18S020));
    XRCCTRL(*this,"ColoursMicroboard", wxButton)->Enable(status | (runningComputer_ == MICROBOARD));
    XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status | (runningComputer_ == VIP));
    XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status | (runningComputer_ == VIPII));
    XRCCTRL(*this,"ColoursVip2K", wxButton)->Enable(status | (runningComputer_ == VIP2K));
    XRCCTRL(*this,"ColoursMCDS", wxButton)->Enable(status | (runningComputer_ == MCDS));
    XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status | (runningComputer_ == MS2000));
    XRCCTRL(*this,"ColoursCoinArcade", wxButton)->Enable(status | (runningComputer_ == COINARCADE));
    XRCCTRL(*this,"ColoursFRED1", wxButton)->Enable(status | (runningComputer_ == FRED1));
    XRCCTRL(*this,"ColoursFRED1_5", wxButton)->Enable(status | (runningComputer_ == FRED1_5));
    XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status | (runningComputer_ == STUDIO));
    XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status | (runningComputer_ == VICTORY));
    XRCCTRL(*this,"ColoursStudioIV", wxButton)->Enable(status | (runningComputer_ == STUDIOIV));
    XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status | (runningComputer_ == VISICOM));
    XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status | (runningComputer_ == CIDELSA));
    XRCCTRL(*this,"ColoursTMC600", wxButton)->Enable(status | (runningComputer_ == TMC600));
    XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status | (runningComputer_ == TMC1800));
    XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status | (runningComputer_ == TMC2000));
    XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status | (runningComputer_ == NANO));
    XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status | (runningComputer_ == PECOM));
    XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status | (runningComputer_ == ETI));
}

void Main::enableGui(bool status)
{
    if (!mode_.gui)
        return;
    
    wxMenuBar *menubarPointer = GetMenuBar();

    menubarPointer->Enable(XRCID(GUIDEFAULT), status);
    menubarPointer->Enable(XRCID("MI_ActivateMain"), !status);
    menubarPointer->Enable(XRCID("MI_FullScreen"), !status);
    menubarPointer->Enable(XRCID("SYSTEM00"), status);
    menubarPointer->Enable(XRCID("CDP1801"), status);
    menubarPointer->Enable(XRCID("CDP1802"), status);
    menubarPointer->Enable(XRCID("CDP1804"), status);
    menubarPointer->Enable(XRCID("CDP1805"), status);
    menubarPointer->Enable(XRCID("StartupRegistersZeroed"), status);
    menubarPointer->Enable(XRCID("StartupRegistersRandom"), status);
    menubarPointer->Enable(XRCID("StartupRamZeroed"), status);
    menubarPointer->Enable(XRCID("StartupRamRandom"), status);
    menubarPointer->Enable(XRCID("StartupRamDynamic"), status);
    menubarPointer->Enable(XRCID("StartupVideoRamZeroed"), status);
    menubarPointer->Enable(XRCID("StartupVideoRamRandom"), status);
    menubarPointer->Enable(XRCID("StartupVideoRamDynamic"), status);
    menubarPointer->Enable(XRCID("KeyboardDutch"), status);
    menubarPointer->Enable(XRCID("KeyboardGerman"), status);
    menubarPointer->Enable(XRCID("KeyboardFrench"), status);
    menubarPointer->Enable(XRCID("KeyboardSwedish"), status);
    menubarPointer->Enable(XRCID("KeyboardUs"), status);
    menubarPointer->Enable(XRCID("KeyboardUserDefined"), status);

    XRCCTRL(*this,"ProfilerCounter", wxChoice)->Enable(status);

    enableColorbutton(status);
    if (runningComputer_ == COMX)
    {
        p_Main->scrtValues(status, true, 4, 0x2e14, 5, 0x31EB);

        chip8ProtectedMode_= false;
        XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false); 
        XRCCTRL(*this,"EpromComx", wxButton)->Enable(status);
        XRCCTRL(*this,"MainRomComx", wxComboBox)->Enable(status&!conf[COMX].sbActive_);
        XRCCTRL(*this,"RomButtonComx", wxButton)->Enable(status&!conf[COMX].sbActive_);
        XRCCTRL(*this,"ExpRomComx", wxComboBox)->Enable(status&!(conf[COMX].sbActive_ || conf[COMX].useDiagnosticBoard_));
        XRCCTRL(*this,"ExpRomButtonComx", wxButton)->Enable(status&!(conf[COMX].sbActive_ || conf[COMX].useDiagnosticBoard_));
        XRCCTRL(*this, "Cart1RomComx", wxComboBox)->Enable(status&!conf[COMX].useDiagnosticBoard_);
        XRCCTRL(*this, "Cart1RomButtonComx", wxButton)->Enable(status&!conf[COMX].useDiagnosticBoard_);
        if (expansionRomLoaded_ || conf[COMX].sbActive_)
        {
            XRCCTRL(*this,"Cart2RomComx", wxComboBox)->Enable(status);
            XRCCTRL(*this,"Cart2RomButtonComx", wxButton)->Enable(status);
            XRCCTRL(*this,"Cart3RomComx", wxComboBox)->Enable(status);
            XRCCTRL(*this,"Cart3RomButtonComx", wxButton)->Enable(status);
            XRCCTRL(*this,"Cart4RomComx", wxComboBox)->Enable(status&!conf[COMX].sbActive_);
            XRCCTRL(*this,"Cart4RomButtonComx", wxButton)->Enable(status&!conf[COMX].sbActive_);
        }
        XRCCTRL(*this,"BatchConvertButtonComx", wxButton)->Enable(!status);
        XRCCTRL(*this,"VidModeComx", wxChoice)->Enable(status);
        XRCCTRL(*this,"VidModeTextComx", wxStaticText)->Enable(status);
        XRCCTRL(*this,"PrintButtonComx", wxButton)->Enable(!status);
        XRCCTRL(*this,"ScreenDumpF5Comx", wxButton)->Enable(!status);
        XRCCTRL(*this,"FullScreenF3Comx", wxButton)->Enable(!status);
        XRCCTRL(*this,"ExpRamComx", wxCheckBox)->Enable(status&!conf[COMX].useDiagnosticBoard_);
        XRCCTRL(*this, "SbActiveComx", wxCheckBox)->Enable(status);
        XRCCTRL(*this, "DramComx", wxCheckBox)->Enable(status);
        XRCCTRL(*this, "DiagActiveComx", wxCheckBox)->Enable(status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
        enableDiskRomGui(diskRomLoaded_);
        enableComxGui(status);
    }
    if (runningComputer_ == CIDELSA)
    {
        chip8ProtectedMode_= false;
        XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
        XRCCTRL(*this,"MainRomCidelsa", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonCidelsa", wxButton)->Enable(status);
        XRCCTRL(*this,"ScreenDumpF5Cidelsa", wxButton)->Enable(!status);
        XRCCTRL(*this,"FullScreenF3Cidelsa", wxButton)->Enable(!status);
    }
    if (runningComputer_ == TMC600)
    {
        p_Main->scrtValues(status, true, 4, 0x16BC, 5, 0x16DD);

        chip8ProtectedMode_= false;
        XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
        XRCCTRL(*this,"PrintButtonTMC600", wxButton)->Enable(!status);
        XRCCTRL(*this,"ScreenDumpF5TMC600", wxButton)->Enable(!status);
        XRCCTRL(*this,"FullScreenF3TMC600", wxButton)->Enable(!status);
        XRCCTRL(*this,"MainRomTMC600", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonTMC600", wxButton)->Enable(status);
        XRCCTRL(*this,"ExpRomTMC600", wxComboBox)->Enable(status);
        XRCCTRL(*this,"ExpRomButtonTMC600", wxButton)->Enable(status);
        XRCCTRL(*this,"CharRomTMC600", wxComboBox)->Enable(status);
        XRCCTRL(*this,"CharRomButtonTMC600", wxButton)->Enable(status);
        XRCCTRL(*this,"RamTMC600", wxChoice)->Enable(status);
        XRCCTRL(*this,"RamTextTMC600", wxStaticText)->Enable(status);
        if (status == true)
        {
            XRCCTRL(*this,"AdiInputText", wxStaticText)->Enable(!status);
            XRCCTRL(*this,"AdiChannel", wxSpinCtrl)->Enable(!status);
            XRCCTRL(*this,"AdiVolt", wxSpinCtrl)->Enable(!status);
            XRCCTRL(*this,"AdiVoltText", wxStaticText)->Enable(!status);
            XRCCTRL(*this,"AdsInputText", wxStaticText)->Enable(!status);
            XRCCTRL(*this,"AdsChannel", wxSpinCtrl)->Enable(!status);
            XRCCTRL(*this,"AdsVolt", wxSpinCtrl)->Enable(!status);
            XRCCTRL(*this,"AdsVoltText", wxStaticText)->Enable(!status);
        }
        XRCCTRL(*this,"RealTimeClockTMC600", wxCheckBox)->Enable(status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == PECOM)
    {
        p_Main->scrtValues(status, true, 4, 0xAFE8, 5, 0xA5F2);

        chip8ProtectedMode_= false;
        XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
        XRCCTRL(*this,"ColoursCoinArcade", wxButton)->Enable(status);
        XRCCTRL(*this,"PrintButtonPecom", wxButton)->Enable(!status);
        XRCCTRL(*this,"ScreenDumpF5Pecom", wxButton)->Enable(!status);
        XRCCTRL(*this,"FullScreenF3Pecom", wxButton)->Enable(!status);
        XRCCTRL(*this,"MainRomPecom", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonPecom", wxButton)->Enable(status);
        XRCCTRL(*this, "DramPecom", wxCheckBox)->Enable(status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == FRED1)
    {
        p_Main->scrtValues(status, false, -1, -1, -1, -1);

        enableChip8DebugGui(!status);
        XRCCTRL(*this,"RamSWFRED1", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RamSWButtonFRED1", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3FRED1", wxButton)->Enable(!status);
        XRCCTRL(*this,"ScreenDumpF5FRED1", wxButton)->Enable(!status);
        XRCCTRL(*this,"RamFRED1", wxChoice)->Enable(status);
        XRCCTRL(*this,"RamTextFRED1", wxStaticText)->Enable(status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == FRED1_5)
    {
        p_Main->scrtValues(status, false, -1, -1, -1, -1);

        enableChip8DebugGui(!status);
        XRCCTRL(*this,"RamSWFRED1_5", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RamSWButtonFRED1_5", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3FRED1_5", wxButton)->Enable(!status);
        XRCCTRL(*this,"ScreenDumpF5FRED1_5", wxButton)->Enable(!status);
        XRCCTRL(*this,"RamFRED1_5", wxChoice)->Enable(status);
        XRCCTRL(*this,"RamTextFRED1_5", wxStaticText)->Enable(status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == VIP)
    {
        enableChip8DebugGui(!status);
        XRCCTRL(*this,"HighResVip", wxCheckBox)->Enable(status);
        XRCCTRL(*this,"MainRomVip", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonVip", wxButton)->Enable(status);
        XRCCTRL(*this,"RamSWVip", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RamSWButtonVip", wxButton)->Enable(status);
        XRCCTRL(*this,"Chip8SWVip", wxTextCtrl)->Enable(status);
        XRCCTRL(*this,"Chip8SWButtonVip", wxButton)->Enable(status);
        XRCCTRL(*this,"EjectChip8SWVip", wxButton)->Enable(status);
        XRCCTRL(*this,"VP580",wxCheckBox)->Enable(status);
        XRCCTRL(*this,"VP590",wxCheckBox)->Enable(status);
        XRCCTRL(*this,"KeyboardVip",wxCheckBox)->Enable(status);
        XRCCTRL(*this,"SoundVip",wxChoice)->Enable(status);
        XRCCTRL(*this,"StereoVip",wxCheckBox)->Enable(status);
        XRCCTRL(*this,"FullScreenF3Vip", wxButton)->Enable(!status);
        XRCCTRL(*this,"RamVip", wxSpinCtrl)->Enable(status);
        XRCCTRL(*this,"RamTextVip", wxStaticText)->Enable(status);
        XRCCTRL(*this,"RamKbVip", wxStaticText)->Enable(status);
        XRCCTRL(*this,"VP570", wxSpinCtrl)->Enable(status);
        XRCCTRL(*this,"VP570Text", wxStaticText)->Enable(status);
        XRCCTRL(*this,"RamKbVip", wxStaticText)->Enable(status);
        if (!status)
        {
            XRCCTRL(*this,"PrintButtonVip", wxBitmapButton)->Enable(conf[VIP].printerOn_);
            XRCCTRL(*this,"PrintButtonVip", wxBitmapButton)->SetToolTip("Open printer window (F4)");
        }
        else
        {
            XRCCTRL(*this,"PrintButtonVip", wxBitmapButton)->Enable(true);
            if (conf[VIP].printerOn_)
                XRCCTRL(*this,"PrintButtonVip", wxBitmapButton)->SetToolTip("Disable printer support");
            else
                XRCCTRL(*this,"PrintButtonVip", wxBitmapButton)->SetToolTip("Enable printer support");
        }
        XRCCTRL(*this, "VTTypeVip", wxChoice)->Enable(status);
        if (XRCCTRL(*this,"VTTypeVip",wxChoice)->GetSelection() != VTNONE)
        {
            if (elfConfiguration[VIP].useUart)
            {
                XRCCTRL(*this, "VTBaudRTextVip", wxStaticText)->Enable(status);
                XRCCTRL(*this, "VTBaudRChoiceVip", wxChoice)->Enable(status);
            }
            XRCCTRL(*this, "VTBaudTChoiceVip", wxChoice)->Enable(status);
            XRCCTRL(*this, "VTBaudTTextVip", wxStaticText)->Enable(status);
            XRCCTRL(*this,"VtSetupVip", wxButton)->Enable(status);
        }

        XRCCTRL(*this,"ScreenDumpF5Vip", wxButton)->Enable(!status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == VIPII)
    {
//        p_Main->scrtValues(status, true, 4, 0x28EF, 5, 0x23E7);

        enableChip8DebugGui(!status);
        XRCCTRL(*this,"MainRomVipII", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"RamSWVipII", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RamSWButtonVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"Chip8SWVipII", wxTextCtrl)->Enable(status);
        XRCCTRL(*this,"Chip8SWButtonVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"EjectChip8SWVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3VipII", wxButton)->Enable(!status);
        XRCCTRL(*this,"ScreenDumpF5VipII", wxButton)->Enable(!status);
        XRCCTRL(*this,"ComputerVersionVipII", wxChoice)->Enable(status);
        if (conf[VIPII].computerVersion_ == VIPII_ED)
        {
            XRCCTRL(*this,"MainRom2VipII", wxComboBox)->Enable(status);
            XRCCTRL(*this,"RomButton2VipII", wxButton)->Enable(status);
        }
        else
        {
            XRCCTRL(*this,"AutoBootTypeVipII", wxChoice)->Enable(status);
            XRCCTRL(*this,"AutoBootVipII", wxCheckBox)->Enable(status);
            XRCCTRL(*this,"RamTextVipII", wxStaticText)->Enable(status);
            XRCCTRL(*this,"RamVipII", wxChoice)->Enable(status);
        }
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == VIP2K)
    {
        XRCCTRL(*this,"VtShowVip2K", wxCheckBox)->Enable(status);
        XRCCTRL(*this,"MainRomVip2K", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonVip2K", wxButton)->Enable(status);
        XRCCTRL(*this,"RamSWVip2K", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RamSWButtonVip2K", wxButton)->Enable(status);
//        XRCCTRL(*this,"Chip8SWVip2K", wxTextCtrl)->Enable(status);
//        XRCCTRL(*this,"Chip8SWButtonVip2K", wxButton)->Enable(status);
//        XRCCTRL(*this,"EjectChip8SWVip2K", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3Vip2K", wxButton)->Enable(!status);
        XRCCTRL(*this, "VTTypeVip2K", wxChoice)->Enable(status);
        if (XRCCTRL(*this,"VTTypeVip2K",wxChoice)->GetSelection() != VTNONE)
        {
            if (elfConfiguration[VIP2K].useUart)
            {
                XRCCTRL(*this, "VTBaudRTextVip2K", wxStaticText)->Enable(status);
                XRCCTRL(*this, "VTBaudRChoiceVip2K", wxChoice)->Enable(status);
            }
            XRCCTRL(*this, "VTBaudTChoiceVip2K", wxChoice)->Enable(status);
            XRCCTRL(*this, "VTBaudTTextVip2K", wxStaticText)->Enable(status);
            XRCCTRL(*this,"VtSetupVip2K", wxButton)->Enable(status);
        }
        
        XRCCTRL(*this,"ScreenDumpF5Vip2K", wxButton)->Enable(!status);
        enableLoadGui(!status);
    }
    if (runningComputer_ == VELF)
    {
        enableChip8DebugGui(!status);
        XRCCTRL(*this,"MainRomVelf", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"RamSWVelf", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RamSWButtonVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"Chip8SWVelf", wxTextCtrl)->Enable(status);
        XRCCTRL(*this,"Chip8SWButtonVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"EjectChip8SWVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3Velf", wxButton)->Enable(!status);
        if (!status)
        {
            XRCCTRL(*this,"PrintButtonVelf", wxBitmapButton)->Enable(conf[VELF].printerOn_);
            XRCCTRL(*this,"PrintButtonVelf", wxBitmapButton)->SetToolTip("Open printer window (F4)");
        }
        else
        {
            XRCCTRL(*this,"PrintButtonVelf", wxBitmapButton)->Enable(true);
            if (conf[VELF].printerOn_)
                XRCCTRL(*this,"PrintButtonVelf", wxBitmapButton)->SetToolTip("Disable printer support");
            else
                XRCCTRL(*this,"PrintButtonVelf", wxBitmapButton)->SetToolTip("Enable printer support");
        }
        XRCCTRL(*this, "VTTypeVelf", wxChoice)->Enable(status);
        if (XRCCTRL(*this,"VTTypeVelf",wxChoice)->GetSelection() != VTNONE)
        {
            if (elfConfiguration[VELF].useUart)
            {
                XRCCTRL(*this, "VTBaudRTextVelf", wxStaticText)->Enable(status);
                XRCCTRL(*this, "VTBaudRChoiceVelf", wxChoice)->Enable(status);
            }
            XRCCTRL(*this, "VTBaudTChoiceVelf", wxChoice)->Enable(status);
            XRCCTRL(*this, "VTBaudTTextVelf", wxStaticText)->Enable(status);
            XRCCTRL(*this,"VtSetupVelf", wxButton)->Enable(status);
        }
        
        XRCCTRL(*this,"ScreenDumpF5Velf", wxButton)->Enable(!status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == CDP18S020)
    {
        enableChip8DebugGui(!status);
        XRCCTRL(*this,"MainRomCDP18S020", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonCDP18S020", wxButton)->Enable(status);
        XRCCTRL(*this,"RamSWCDP18S020", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RamSWButtonCDP18S020", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3CDP18S020", wxButton)->Enable(!status);
        XRCCTRL(*this, "VTTypeCDP18S020", wxChoice)->Enable(status);
        if (XRCCTRL(*this,"VTTypeCDP18S020",wxChoice)->GetSelection() != VTNONE)
        {
            if (elfConfiguration[CDP18S020].useUart)
            {
                XRCCTRL(*this, "VTBaudRTextCDP18S020", wxStaticText)->Enable(status);
                XRCCTRL(*this, "VTBaudRChoiceCDP18S020", wxChoice)->Enable(status);
            }
            XRCCTRL(*this, "VTBaudTChoiceCDP18S020", wxChoice)->Enable(status);
            XRCCTRL(*this, "VTBaudTTextCDP18S020", wxStaticText)->Enable(status);
            XRCCTRL(*this,"VtSetupCDP18S020", wxButton)->Enable(status);
        }
        
        XRCCTRL(*this,"ScreenDumpF5CDP18S020", wxButton)->Enable(!status);
        XRCCTRL(*this,"RamCDP18S020", wxChoice)->Enable(status);
        XRCCTRL(*this,"AutoBootTypeCDP18S020", wxChoice)->Enable(status);
//        XRCCTRL(*this, "AutoBootCDP18S020", wxCheckBox)->SetValue(status);
        enableLoadGui(!status);
    }
    if (runningComputer_ == MICROBOARD)
    {
        p_Main->scrtValues(status, true, 4, 0x8364, 5, 0x8374);
        
        XRCCTRL(*this, "Card1ChoiceMicroboard", wxChoice)->Enable(status);
        XRCCTRL(*this, "Card2ChoiceMicroboard", wxChoice)->Enable(status);
        XRCCTRL(*this, "Card3ChoiceMicroboard", wxChoice)->Enable(status);
        XRCCTRL(*this, "Card4ChoiceMicroboard", wxChoice)->Enable(status);
        XRCCTRL(*this, "Card5ChoiceMicroboard", wxChoice)->Enable(status);

        XRCCTRL(*this, "Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this, "Chip8DebugMode", wxCheckBox)->SetValue(false);
        XRCCTRL(*this,"AutoBootTypeMicroboard", wxChoice)->Enable(status);
        XRCCTRL(*this, "AutoBootMicroboard", wxCheckBox)->Enable(status);
        enableMemAccessGui(!status);
        if (!elfConfiguration[runningComputer_].vtExternal)
        {
            XRCCTRL(*this, "FullScreenF3Microboard", wxButton)->Enable(!status&(elfConfiguration[MICROBOARD].vtType != VTNONE));
            XRCCTRL(*this, "ScreenDumpF5Microboard", wxButton)->Enable(!status&(elfConfiguration[MICROBOARD].vtType != VTNONE));
        }
        XRCCTRL(*this, "VTTypeMicroboard", wxChoice)->Enable(status);
        if (elfConfiguration[MICROBOARD].useUart)
        {
            XRCCTRL(*this, "VTBaudRTextMicroboard", wxStaticText)->Enable(status);
            XRCCTRL(*this, "VTBaudRChoiceMicroboard", wxChoice)->Enable(status);
        }
        XRCCTRL(*this, "VTBaudTChoiceMicroboard", wxChoice)->Enable(status);
        XRCCTRL(*this, "VTBaudTTextMicroboard", wxStaticText)->Enable(status);
        XRCCTRL(*this, "VtSetupMicroboard", wxButton)->Enable(status);
        
//        setCardType();
    }
    if (runningComputer_ == STUDIO)
    {
        p_Main->scrtValues(status, false, -1, -1, -1, -1);

        enableChip8DebugGui(!status);
        XRCCTRL(*this,"MainRomStudio2", wxComboBox)->Enable(status&(!conf[STUDIO].disableSystemRom_ | !conf[STUDIO].multiCart_));
        XRCCTRL(*this,"RomButtonStudio2", wxButton)->Enable(status&(!conf[STUDIO].disableSystemRom_ | !conf[STUDIO].multiCart_));
        XRCCTRL(*this,"CartRomStudio2", wxComboBox)->Enable(status);
        XRCCTRL(*this,"CartRomButtonStudio2", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3Studio2", wxButton)->Enable(!status);
        XRCCTRL(*this,"ScreenDumpF5Studio2", wxButton)->Enable(!status);
        XRCCTRL(*this,"MultiCartStudio2", wxCheckBox)->Enable(status);
    }
    if (runningComputer_ == COINARCADE)
    {
        p_Main->scrtValues(status, false, -1, -1, -1, -1);
 
        enableChip8DebugGui(!status);
        XRCCTRL(*this,"MainRomCoinArcade", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonCoinArcade", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3CoinArcade", wxButton)->Enable(!status);
        XRCCTRL(*this,"ScreenDumpF5CoinArcade", wxButton)->Enable(!status);
    }
    if (runningComputer_ == VISICOM)
    {
        p_Main->scrtValues(status, false, -1, -1, -1, -1);

        enableChip8DebugGui(!status);
        XRCCTRL(*this,"MainRomVisicom", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonVisicom", wxButton)->Enable(status);
        XRCCTRL(*this,"CartRomVisicom", wxComboBox)->Enable(status);
        XRCCTRL(*this,"CartRomButtonVisicom", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3Visicom", wxButton)->Enable(!status);
        XRCCTRL(*this,"ScreenDumpF5Visicom", wxButton)->Enable(!status);
    }
    if (runningComputer_ == VICTORY)
    {
        p_Main->scrtValues(status, false, -1, -1, -1, -1);

        enableChip8DebugGui(!status);
        XRCCTRL(*this,"MainRomVictory", wxComboBox)->Enable(status&(!conf[VICTORY].disableSystemRom_ | !conf[VICTORY].multiCart_));
        XRCCTRL(*this,"RomButtonVictory", wxButton)->Enable(status&(!conf[VICTORY].disableSystemRom_ | !conf[VICTORY].multiCart_));
        XRCCTRL(*this,"CartRomVictory", wxComboBox)->Enable(status);
        XRCCTRL(*this,"CartRomButtonVictory", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3Victory", wxButton)->Enable(!status);
        XRCCTRL(*this,"ScreenDumpF5Victory", wxButton)->Enable(!status);
        XRCCTRL(*this,"MultiCartVictory", wxCheckBox)->Enable(status);
        XRCCTRL(*this,"VidModeStudioIV", wxChoice)->Enable(status);
    }
    if (runningComputer_ == STUDIOIV)
    {
        p_Main->scrtValues(status, false, -1, -1, -1, -1);

        enableChip8DebugGui(!status);
        XRCCTRL(*this,"MainRomStudioIV", wxComboBox)->Enable(status&(!conf[STUDIOIV].disableSystemRom_ | !conf[STUDIOIV].multiCart_));
        XRCCTRL(*this,"RomButtonStudioIV", wxButton)->Enable(status&(!conf[STUDIOIV].disableSystemRom_ | !conf[STUDIOIV].multiCart_));
        XRCCTRL(*this,"CartRomStudioIV", wxComboBox)->Enable(status);
        XRCCTRL(*this,"CartRomButtonStudioIV", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3StudioIV", wxButton)->Enable(!status);
        XRCCTRL(*this,"ScreenDumpF5StudioIV", wxButton)->Enable(!status);
        XRCCTRL(*this,"VidModeStudioIV", wxChoice)->Enable(status);
        XRCCTRL(*this, "2020StudioIV", wxCheckBox)->Enable(status);
        XRCCTRL(*this,"CartSwitchStudioIV", wxButton)->Enable(status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == TMC2000)
    {
        p_Main->scrtValues(status, false, -1, -1, -1, -1);

        enableChip8DebugGui(!status);
        XRCCTRL(*this,"MainRomTMC2000", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonTMC2000", wxButton)->Enable(status);
        XRCCTRL(*this,"RamSWTMC2000", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RamSWButtonTMC2000", wxButton)->Enable(status);
        XRCCTRL(*this,"Chip8SWTMC2000", wxTextCtrl)->Enable(status);
        XRCCTRL(*this,"EjectChip8SWTMC2000", wxButton)->Enable(status);
        XRCCTRL(*this,"Chip8SWButtonTMC2000", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3TMC2000", wxButton)->Enable(!status);
        XRCCTRL(*this,"RamTMC2000", wxChoice)->Enable(status);
        XRCCTRL(*this,"RamTextTMC2000", wxStaticText)->Enable(status);
        XRCCTRL(*this,"ScreenDumpF5TMC2000", wxButton)->Enable(!status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == TMC1800)
    {
        p_Main->scrtValues(status, false, -1, -1, -1, -1);

        enableChip8DebugGui(!status);
        XRCCTRL(*this,"MainRomTMC1800", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonTMC1800", wxButton)->Enable(status);
        XRCCTRL(*this,"RamSWTMC1800", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RamSWButtonTMC1800", wxButton)->Enable(status);
        XRCCTRL(*this,"Chip8SWTMC1800", wxTextCtrl)->Enable(status);
        XRCCTRL(*this,"EjectChip8SWTMC1800", wxButton)->Enable(status);
        XRCCTRL(*this,"Chip8SWButtonTMC1800", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3TMC1800", wxButton)->Enable(!status);
        XRCCTRL(*this,"RamTMC1800", wxChoice)->Enable(status);
        XRCCTRL(*this,"RamTextTMC1800", wxStaticText)->Enable(status);
        XRCCTRL(*this,"ScreenDumpF5TMC1800", wxButton)->Enable(!status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == ETI)
    {
        p_Main->scrtValues(status, false, -1, -1, -1, -1);

        enableChip8DebugGui(!status);
        XRCCTRL(*this,"MainRomEti", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonEti", wxButton)->Enable(status);
        XRCCTRL(*this,"Chip8SWEti", wxTextCtrl)->Enable(status);
        XRCCTRL(*this,"EjectChip8SWEti", wxButton)->Enable(status);
        XRCCTRL(*this,"Chip8SWButtonEti", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3Eti", wxButton)->Enable(!status);
        XRCCTRL(*this,"ScreenDumpF5Eti", wxButton)->Enable(!status);
        XRCCTRL(*this,"RamEti", wxChoice)->Enable(status);
        XRCCTRL(*this,"RamTextEti", wxStaticText)->Enable(status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == NANO)
    {
        p_Main->scrtValues(status, false, -1, -1, -1, -1);

        enableChip8DebugGui(!status);
        XRCCTRL(*this,"MainRomNano", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonNano", wxButton)->Enable(status);
        XRCCTRL(*this,"RamSWNano", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RamSWButtonNano", wxButton)->Enable(status);
        XRCCTRL(*this,"Chip8SWNano", wxTextCtrl)->Enable(status);
        XRCCTRL(*this,"EjectChip8SWNano", wxButton)->Enable(status);
        XRCCTRL(*this,"Chip8SWButtonNano", wxButton)->Enable(status);
        XRCCTRL(*this,"FullScreenF3Nano", wxButton)->Enable(!status);
        XRCCTRL(*this,"ScreenDumpF5Nano", wxButton)->Enable(!status);
        XRCCTRL(*this,"SoundNano", wxChoice)->Enable(status);
        XRCCTRL(*this,"SoundTextNano", wxStaticText)->Enable(status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }

    if (runningComputer_ == ELF || runningComputer_ == ELFII || runningComputer_ == SUPERELF)
    {
        chip8ProtectedMode_= false;
        XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
        wxString elfTypeStr;
        switch (runningComputer_)
        {
            case ELF:
                XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
                XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
                XRCCTRL(*this,"UseLedModule", wxCheckBox)->Enable(status);
                elfTypeStr = "Elf";
            break;
            case ELFII:
                XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
                XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
                elfTypeStr = "ElfII";
                XRCCTRL(*this,"Giant"+elfTypeStr, wxCheckBox)->Enable(status);
                XRCCTRL(*this,"EFButtons"+elfTypeStr, wxCheckBox)->Enable(status);
            break;
            case SUPERELF:
                XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
                XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
                elfTypeStr = "SuperElf";
            break;
        }
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
        XRCCTRL(*this,"MainRom"+elfTypeStr, wxComboBox)->Enable(status);
        XRCCTRL(*this,"MainRom2"+elfTypeStr, wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButton"+elfTypeStr, wxButton)->Enable(status);
        if (elfConfiguration[selectedComputer_].memoryType != 3)
            XRCCTRL(*this,"Rom1"+elfTypeStr, wxButton)->Enable(status);
        XRCCTRL(*this,"RomButton2"+elfTypeStr, wxButton)->Enable(status);
        XRCCTRL(*this,"Rom2"+elfTypeStr, wxButton)->Enable(status);
        XRCCTRL(*this,"DP_Button"+elfTypeStr, wxButton)->Enable(status);
        if (elfConfiguration[runningComputer_].ideEnabled)
        {
            XRCCTRL(*this,"IDE_Button"+elfTypeStr, wxButton)->Enable(status);
            XRCCTRL(*this,"IdeFile"+elfTypeStr, wxTextCtrl)->Enable(status);
            XRCCTRL(*this,"Eject_IDE"+elfTypeStr, wxButton)->Enable(status);
        }
        if (!status)
        {
            XRCCTRL(*this,"PrintButton"+elfTypeStr, wxBitmapButton)->Enable(conf[runningComputer_].printerOn_);
            XRCCTRL(*this,"PrintButton"+elfTypeStr, wxBitmapButton)->SetToolTip("Open printer window (F4)");
        }
        else
        {
            XRCCTRL(*this,"PrintButton"+elfTypeStr, wxBitmapButton)->Enable(true);
            if (conf[runningComputer_].printerOn_)
                XRCCTRL(*this,"PrintButton"+elfTypeStr, wxBitmapButton)->SetToolTip("Disable printer support");
            else
                XRCCTRL(*this,"PrintButton"+elfTypeStr, wxBitmapButton)->SetToolTip("Enable printer support");
        }
        XRCCTRL(*this,"Memory"+elfTypeStr, wxChoice)->Enable(status);
        XRCCTRL(*this,"MemoryText"+elfTypeStr,wxStaticText)->Enable(status);
        if (elfConfiguration[runningComputer_].usePager)
            XRCCTRL(*this,"PortExt"+elfTypeStr, wxCheckBox)->Enable(false);
        else
            XRCCTRL(*this,"PortExt"+elfTypeStr, wxCheckBox)->Enable(status);
        XRCCTRL(*this,"BootStrap"+elfTypeStr, wxCheckBox)->Enable(status);

        if (elfConfiguration[runningComputer_].usePager || elfConfiguration[runningComputer_].useEms)
        {
            XRCCTRL(*this, "StartRamText"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(false);
            XRCCTRL(*this, "StartRam"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(false);
            XRCCTRL(*this, "EndRamText"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(false);
            XRCCTRL(*this, "EndRam"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(false);
        }
        else
        {
            XRCCTRL(*this, "StartRamText"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(status);
            XRCCTRL(*this, "StartRam"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(status);
            XRCCTRL(*this, "EndRamText"+computerInfo[runningComputer_].gui, wxStaticText)->Enable(status);
            XRCCTRL(*this, "EndRam"+computerInfo[runningComputer_].gui, wxTextCtrl)->Enable(status);
        }
        XRCCTRL(*this,"VTType"+elfTypeStr,wxChoice)->Enable(status);
        if (XRCCTRL(*this,"VTType"+elfTypeStr,wxChoice)->GetSelection() != VTNONE)
        {
            if (elfConfiguration[runningComputer_].useUart || elfConfiguration[runningComputer_].useUart16450)
            {
                XRCCTRL(*this, "VTBaudRText" + elfTypeStr, wxStaticText)->Enable(status);
                XRCCTRL(*this, "VTBaudRChoice" + elfTypeStr, wxChoice)->Enable(status);
            }
            XRCCTRL(*this, "VTBaudTChoice" + elfTypeStr, wxChoice)->Enable(status);
            XRCCTRL(*this, "VTBaudTText" + elfTypeStr, wxStaticText)->Enable(status);
            XRCCTRL(*this, "VtSetup"+elfTypeStr, wxButton)->Enable(status);
        }
        XRCCTRL(*this,"VideoType"+elfTypeStr,wxChoice)->Enable(status);
        XRCCTRL(*this,"VideoTypeText"+elfTypeStr,wxStaticText)->Enable(status);
        XRCCTRL(*this,"DiskType"+elfTypeStr,wxChoice)->Enable(status);
        XRCCTRL(*this,"Keyboard"+elfTypeStr,wxChoice)->Enable(status);
        XRCCTRL(*this,"KeyboardText"+elfTypeStr,wxStaticText)->Enable(status);
        XRCCTRL(*this,"CharRomButton"+elfTypeStr, wxButton)->Enable(status&(elfConfiguration[runningComputer_].use6847||elfConfiguration[runningComputer_].use8275||elfConfiguration[runningComputer_].use6845||elfConfiguration[runningComputer_].useS100));
        if (!elfConfiguration[runningComputer_].vtExternal)
        {
            XRCCTRL(*this,"FullScreenF3"+elfTypeStr, wxButton)->Enable(!status&(elfConfiguration[runningComputer_].usePixie||elfConfiguration[runningComputer_].useTMS9918||elfConfiguration[runningComputer_].use6847||elfConfiguration[runningComputer_].use6845||elfConfiguration[runningComputer_].use8275||elfConfiguration[runningComputer_].useS100||(elfConfiguration[runningComputer_].vtType != VTNONE)));
            XRCCTRL(*this,"ScreenDumpF5"+elfTypeStr, wxButton)->Enable(!status&(elfConfiguration[runningComputer_].usePixie||elfConfiguration[runningComputer_].useTMS9918||elfConfiguration[runningComputer_].use6847||elfConfiguration[runningComputer_].use6845||elfConfiguration[runningComputer_].use8275||elfConfiguration[runningComputer_].useS100||(elfConfiguration[runningComputer_].vtType != VTNONE)));
       }
        XRCCTRL(*this,"CharRom"+elfTypeStr, wxComboBox)->Enable(status&(elfConfiguration[runningComputer_].use6847||elfConfiguration[runningComputer_].use8275||elfConfiguration[runningComputer_].use6845||elfConfiguration[runningComputer_].useS100));
        
        XRCCTRL(*this,"TilType"+elfTypeStr,wxChoice)->Enable(status);
        XRCCTRL(*this,"TilText"+elfTypeStr,wxStaticText)->Enable(status);
        enableMemAccessGui(!status);
    }
    if (runningComputer_ == XML)
    {
       if (status)
           saveScrtValues("");

        chip8ProtectedMode_= false;
        XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
        
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
        
        XRCCTRL(*this, "BatchConvertButtonXml", wxButton)->Enable(conf[XML].useBatchWav_ && !status);
        XRCCTRL(*this,"MainXmlXml", wxComboBox)->Enable(status);
        XRCCTRL(*this,"XmlButtonXml", wxButton)->Enable(status);
       if (conf[runningComputer_].ramFileFromGui_)
        {
            XRCCTRL(*this,"MainRamXml", wxComboBox)->Enable(status);
            XRCCTRL(*this,"RamButtonXml", wxButton)->Enable(status);
        }
        XRCCTRL(*this,"PrintButtonXml", wxButton)->Enable(!status && conf[runningComputer_].printerOn_);

        XRCCTRL(*this,"VTTypeXml",wxChoice)->Enable(status);
        if (XRCCTRL(*this,"VTTypeXml",wxChoice)->GetSelection() != VTNONE)
        {
            if (elfConfiguration[runningComputer_].useUart || elfConfiguration[runningComputer_].useUart16450)
            {
                XRCCTRL(*this, "VTBaudRTextXml", wxStaticText)->Enable(status);
                XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->Enable(status);
            }
            XRCCTRL(*this, "VTBaudTChoiceXml", wxChoice)->Enable(status);
            XRCCTRL(*this, "VTBaudTTextXml", wxStaticText)->Enable(status);
            XRCCTRL(*this, "VtSetupXml", wxButton)->Enable(status);
        }
        if (!elfConfiguration[runningComputer_].vtExternal)
        {
            XRCCTRL(*this,"FullScreenF3Xml", wxButton)->Enable(!status&(elfConfiguration[runningComputer_].usePixie||elfConfiguration[runningComputer_].useTMS9918||elfConfiguration[runningComputer_].use6847||elfConfiguration[runningComputer_].use6845||elfConfiguration[runningComputer_].use8275||elfConfiguration[runningComputer_].useS100||elfConfiguration[runningComputer_].usev1870||elfConfiguration[runningComputer_].useSN76430N||elfConfiguration[runningComputer_].use1864||(elfConfiguration[runningComputer_].vtType != VTNONE)));
            XRCCTRL(*this,"ScreenDumpF5Xml", wxButton)->Enable(!status&(elfConfiguration[runningComputer_].usePixie||elfConfiguration[runningComputer_].useTMS9918||elfConfiguration[runningComputer_].use6847||elfConfiguration[runningComputer_].use6845||elfConfiguration[runningComputer_].use8275||elfConfiguration[runningComputer_].useS100||elfConfiguration[runningComputer_].usev1870||elfConfiguration[runningComputer_].useSN76430N||elfConfiguration[runningComputer_].use1864||(elfConfiguration[runningComputer_].vtType != VTNONE)));
       }
        
        enableMemAccessGui(!status);
    }
    if (runningComputer_ == PICO)
    {
        chip8ProtectedMode_= false;
        XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
        
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
        XRCCTRL(*this,"MainRomPico", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonPico", wxButton)->Enable(status);
        XRCCTRL(*this,"DP_ButtonPico", wxButton)->Enable(status);
        if (elfConfiguration[runningComputer_].ideEnabled)
        {
            XRCCTRL(*this,"IDE_ButtonPico", wxButton)->Enable(status);
            XRCCTRL(*this,"IdeFilePico", wxTextCtrl)->Enable(status);
            XRCCTRL(*this,"Eject_IDEPico", wxButton)->Enable(status);
        }
        if (!status)
        {
            XRCCTRL(*this,"PrintButtonPico", wxBitmapButton)->Enable(conf[runningComputer_].printerOn_);
            XRCCTRL(*this,"PrintButtonPico", wxBitmapButton)->SetToolTip("Open printer window (F4)");
        }
        else
        {
            XRCCTRL(*this,"PrintButtonPico", wxBitmapButton)->Enable(true);
            if (conf[runningComputer_].printerOn_)
                XRCCTRL(*this,"PrintButtonPico", wxBitmapButton)->SetToolTip("Disable printer support");
            else
                XRCCTRL(*this,"PrintButtonPico", wxBitmapButton)->SetToolTip("Enable printer support");
        }

        XRCCTRL(*this,"VTTypePico",wxChoice)->Enable(status);
        if (XRCCTRL(*this,"VTTypePico",wxChoice)->GetSelection() != VTNONE)
        {
            if (elfConfiguration[runningComputer_].useUart || elfConfiguration[runningComputer_].useUart16450)
            {
                XRCCTRL(*this, "VTBaudRTextPico", wxStaticText)->Enable(status);
                XRCCTRL(*this, "VTBaudRChoicePico", wxChoice)->Enable(status);
            }
            XRCCTRL(*this, "VTBaudTChoicePico", wxChoice)->Enable(status);
            XRCCTRL(*this, "VTBaudTTextPico", wxStaticText)->Enable(status);
            XRCCTRL(*this, "VtSetupPico", wxButton)->Enable(status);
        }
        XRCCTRL(*this,"VideoTypePico",wxChoice)->Enable(status);
        XRCCTRL(*this,"VideoTypeTextPico",wxStaticText)->Enable(status);
        XRCCTRL(*this,"DiskTypePico",wxChoice)->Enable(status);
        if (!elfConfiguration[runningComputer_].vtExternal)
        {
            XRCCTRL(*this,"FullScreenF3Pico", wxButton)->Enable(!status&(elfConfiguration[runningComputer_].usePixie||elfConfiguration[runningComputer_].useTMS9918||elfConfiguration[runningComputer_].use6847||elfConfiguration[runningComputer_].use6845||elfConfiguration[runningComputer_].use8275||elfConfiguration[runningComputer_].useS100||(elfConfiguration[runningComputer_].vtType != VTNONE)));
            XRCCTRL(*this,"ScreenDumpF5Pico", wxButton)->Enable(!status&(elfConfiguration[runningComputer_].usePixie||elfConfiguration[runningComputer_].useTMS9918||elfConfiguration[runningComputer_].use6847||elfConfiguration[runningComputer_].use6845||elfConfiguration[runningComputer_].use8275||elfConfiguration[runningComputer_].useS100||(elfConfiguration[runningComputer_].vtType != VTNONE)));
       }
        
        enableMemAccessGui(!status);
    }
    if (runningComputer_ == ELF2K)
    {
        p_Main->scrtValues(status, true, 4, 0xFA7B, 5, 0xFA8D);

        chip8ProtectedMode_= false;
        XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
        XRCCTRL(*this,"MainRomElf2K", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonElf2K", wxButton)->Enable(status);
        XRCCTRL(*this,"IDE_ButtonElf2K", wxButton)->Enable(status);
        XRCCTRL(*this,"IdeFileElf2K", wxTextCtrl)->Enable(status);
        XRCCTRL(*this,"Eject_IDEElf2K", wxButton)->Enable(status);
        XRCCTRL(*this,"DP_ButtonElf2K", wxButton)->Enable(status);
        enableMemAccessGui(!status);
        if (!elfConfiguration[ELF2K].use8275)
        {
            XRCCTRL(*this, "VTTypeElf2K", wxChoice)->Enable(status);
            if (XRCCTRL(*this,"VTTypeElf2K",wxChoice)->GetSelection() != VTNONE)
            {
                if (elfConfiguration[ELF2K].useUart)
                {
                    XRCCTRL(*this, "VTBaudRTextElf2K", wxStaticText)->Enable(status);
                    XRCCTRL(*this, "VTBaudRChoiceElf2K", wxChoice)->Enable(status);
                }
                XRCCTRL(*this, "VTBaudTChoiceElf2K", wxChoice)->Enable(status);
                XRCCTRL(*this, "VTBaudTTextElf2K", wxStaticText)->Enable(status);
                XRCCTRL(*this,"VtSetupElf2K", wxButton)->Enable(status);
            }
        }
        XRCCTRL(*this,"Elf2KVideoType",wxChoice)->Enable(status);
        XRCCTRL(*this,"Elf2KVideoType_Text",wxStaticText)->Enable(status);
        XRCCTRL(*this,"Elf2KKeyboard",wxChoice)->Enable(status);
        XRCCTRL(*this,"Elf2KKeyboard_Text",wxStaticText)->Enable(status);
        XRCCTRL(*this,"CharRomButtonElf2K", wxButton)->Enable(status&elfConfiguration[ELF2K].use8275);
        if (!elfConfiguration[runningComputer_].vtExternal)
        {
            XRCCTRL(*this,"FullScreenF3Elf2K", wxButton)->Enable(!status&(elfConfiguration[ELF2K].usePixie||elfConfiguration[ELF2K].use8275||(elfConfiguration[ELF2K].vtType != VTNONE)));
            XRCCTRL(*this,"ScreenDumpF5Elf2K", wxButton)->Enable(!status&(elfConfiguration[ELF2K].usePixie||elfConfiguration[ELF2K].use8275||(elfConfiguration[ELF2K].vtType != VTNONE)));
        }
        XRCCTRL(*this,"CharRomElf2K", wxComboBox)->Enable(status&elfConfiguration[ELF2K].use8275);
        XRCCTRL(*this,"Elf2KRtc", wxCheckBox)->Enable(status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == MS2000)
    {
        p_Main->scrtValues(status, true, 4, 0x8364, 5, 0x8374);

        XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
        XRCCTRL(*this,"MainRomMS2000", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonMS2000", wxButton)->Enable(status);
        enableMemAccessGui(!status);
        if (!elfConfiguration[runningComputer_].vtExternal)
        {
            XRCCTRL(*this,"FullScreenF3MS2000", wxButton)->Enable(!status&(elfConfiguration[MS2000].vtType != VTNONE));
            XRCCTRL(*this,"ScreenDumpF5MS2000", wxButton)->Enable(!status&(elfConfiguration[MS2000].vtType != VTNONE));
        }
        XRCCTRL(*this, "VTTypeMS2000", wxChoice)->Enable(status);
        if (elfConfiguration[MS2000].useUart)
        {
            XRCCTRL(*this, "VTBaudRTextMS2000", wxStaticText)->Enable(status);
            XRCCTRL(*this, "VTBaudRChoiceMS2000", wxChoice)->Enable(status);
        }
        XRCCTRL(*this, "VTBaudTChoiceMS2000", wxChoice)->Enable(status);
        XRCCTRL(*this, "VTBaudRTextMS2000", wxStaticText)->Enable(status);
        XRCCTRL(*this, "VtSetupMS2000", wxButton)->Enable(status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == MCDS)
    {
        p_Main->scrtValues(status, true, 4, 0x8364, 5, 0x8374);

        XRCCTRL(*this, "Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this, "Chip8DebugMode", wxCheckBox)->SetValue(false);
        XRCCTRL(*this, "MainRomMCDS", wxComboBox)->Enable(status);
        XRCCTRL(*this, "MainRom2MCDS", wxComboBox)->Enable(status);
        XRCCTRL(*this, "MainRom3MCDS", wxComboBox)->Enable(status);
        XRCCTRL(*this, "RomButtonMCDS", wxButton)->Enable(status);
        XRCCTRL(*this, "RomButton2MCDS", wxButton)->Enable(status);
        XRCCTRL(*this, "RomButton3MCDS", wxButton)->Enable(status);
        enableMemAccessGui(!status);
        if (!elfConfiguration[runningComputer_].vtExternal)
        {
            XRCCTRL(*this, "FullScreenF3MCDS", wxButton)->Enable(!status&(elfConfiguration[MCDS].vtType != VTNONE));
            XRCCTRL(*this, "ScreenDumpF5MCDS", wxButton)->Enable(!status&(elfConfiguration[MCDS].vtType != VTNONE));
        }
        XRCCTRL(*this, "VTTypeMCDS", wxChoice)->Enable(status);
        if (elfConfiguration[MCDS].useUart)
        {
            XRCCTRL(*this, "VTBaudRTextMCDS", wxStaticText)->Enable(status);
            XRCCTRL(*this, "VTBaudRChoiceMCDS", wxChoice)->Enable(status);
        }
        XRCCTRL(*this, "VTBaudTChoiceMCDS", wxChoice)->Enable(status);
        XRCCTRL(*this, "VTBaudTTextMCDS", wxStaticText)->Enable(status);
        XRCCTRL(*this, "VtSetupMCDS", wxButton)->Enable(status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
    if (runningComputer_ == COSMICOS)
    {
        chip8ProtectedMode_= false;
        XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
        XRCCTRL(*this,"MainRomCosmicos", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonCosmicos", wxButton)->Enable(status);
        XRCCTRL(*this,"RamCosmicos", wxSpinCtrl)->Enable(status);
        XRCCTRL(*this,"RamTextCosmicos", wxStaticText)->Enable(status);
        XRCCTRL(*this,"RamKbCosmicos", wxStaticText)->Enable(status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
        enableMemAccessGui(!status);

        XRCCTRL(*this, "VTTypeCosmicos", wxChoice)->Enable(status);
        if (XRCCTRL(*this,"VTTypeCosmicos",wxChoice)->GetSelection() != VTNONE)
        {
            XRCCTRL(*this, "VTBaudTChoiceCosmicos", wxChoice)->Enable(status);
            XRCCTRL(*this, "VTBaudTTextCosmicos", wxStaticText)->Enable(status);
            XRCCTRL(*this,"VtSetupCosmicos", wxButton)->Enable(status);
        }

        XRCCTRL(*this,"VideoTypeCosmicos",wxChoice)->Enable(status);
        XRCCTRL(*this,"VideoType_TextCosmicos",wxStaticText)->Enable(status);
        XRCCTRL(*this,"KeyboardCosmicos",wxChoice)->Enable(status);
        XRCCTRL(*this,"Keyboard_TextCosmicos",wxStaticText)->Enable(status);
        if (!elfConfiguration[runningComputer_].vtExternal)
        {
            XRCCTRL(*this,"FullScreenF3Cosmicos", wxButton)->Enable(!status&(elfConfiguration[COSMICOS].usePixie||(elfConfiguration[COSMICOS].vtType != VTNONE)));
            XRCCTRL(*this,"ScreenDumpF5Cosmicos", wxButton)->Enable(!status&(elfConfiguration[COSMICOS].usePixie||(elfConfiguration[COSMICOS].vtType != VTNONE)));
        }
    }
    if (runningComputer_ == MEMBER)
    {
        chip8ProtectedMode_= false;
        XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
        XRCCTRL(*this,"MainRomMembership", wxComboBox)->Enable(status);
        if (conf[MEMBER].ramType_ == 5 || conf[MEMBER].ramType_ == 6)
            XRCCTRL(*this, "RomMembership", wxButton)->Enable(false);
        else
            XRCCTRL(*this,"RomMembership", wxButton)->Enable(status);
        XRCCTRL(*this,"RomButtonMembership", wxButton)->Enable(status);
        XRCCTRL(*this,"RamMembership", wxChoice)->Enable(status);
        XRCCTRL(*this, "RamTextMembership", wxStaticText)->Enable(status);
        XRCCTRL(*this, "IoMembership", wxChoice)->Enable(status);
        XRCCTRL(*this, "FrontMembership", wxChoice)->Enable(status);
        XRCCTRL(*this, "IoTextMembership", wxStaticText)->Enable(status);
        enableLoadGui(!status);
        enableMemAccessGui(!status);

        XRCCTRL(*this, "VTTypeMembership", wxChoice)->Enable(status);
        if (XRCCTRL(*this,"VTTypeMembership",wxChoice)->GetSelection() != VTNONE)
        {
            XRCCTRL(*this, "VTBaudTChoiceMembership", wxChoice)->Enable(status);
            XRCCTRL(*this, "VTBaudTTextMembership", wxStaticText)->Enable(status);
            XRCCTRL(*this,"VtSetupMembership", wxButton)->Enable(status);
        }

        if (!elfConfiguration[runningComputer_].vtExternal)
        {
            XRCCTRL(*this,"FullScreenF3Membership", wxButton)->Enable(elfConfiguration[MEMBER].vtType != VTNONE);
            XRCCTRL(*this,"ScreenDumpF5Membership", wxButton)->Enable(elfConfiguration[MEMBER].vtType != VTNONE);
        }
    }
    if (runningComputer_ == UC1800)
    {
        XRCCTRL(*this, "Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this, "Chip8DebugMode", wxCheckBox)->SetValue(false);
        XRCCTRL(*this, "RamSWUC1800", wxComboBox)->Enable(status);
        XRCCTRL(*this, "RamSWButtonUC1800", wxButton)->Enable(status);
 //       XRCCTRL(*this, "HexOutputUC1800", wxSpinCtrl)->Enable(status);
 //       XRCCTRL(*this, "HexInputUC1800", wxSpinCtrl)->Enable(status);
        enableMemAccessGui(!status);
    }
    if (runningComputer_ == MICROTUTOR)
    {
        XRCCTRL(*this, "Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this, "Chip8DebugMode", wxCheckBox)->SetValue(false);
        XRCCTRL(*this, "RamSWMicrotutor", wxComboBox)->Enable(status);
        XRCCTRL(*this, "RamSWButtonMicrotutor", wxButton)->Enable(status);
        enableMemAccessGui(!status);
    }
    if (runningComputer_ == MICROTUTOR2)
    {
        XRCCTRL(*this, "Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this, "Chip8DebugMode", wxCheckBox)->SetValue(false);
        XRCCTRL(*this, "RamButtonMMicrotutor2", wxButton)->Enable(status);
        XRCCTRL(*this, "MainRamMMicrotutor2", wxComboBox)->Enable(status);
        if (elfConfiguration[runningComputer_].utilityMemory)
        {
            XRCCTRL(*this, "RamButtonEMicrotutor2", wxButton)->Enable(status);
            XRCCTRL(*this, "MainRamEMicrotutor2", wxComboBox)->Enable(status);
        }
        else
        {
            XRCCTRL(*this, "RamButtonEMicrotutor2", wxButton)->Enable(false);
            XRCCTRL(*this, "MainRamEMicrotutor2", wxComboBox)->Enable(false);
        }
        XRCCTRL(*this,"RamMicrotutor2", wxChoice)->Enable(status);
        enableMemAccessGui(!status);
    }
    enableDebugGui(!status);

    if (status)
    {
        XRCCTRL(*this, "AssStore", wxButton)->Enable(false);
#ifdef __WXMAC__
        XRCCTRL(*this, "AssStore", wxButton)->SetBackgroundColour(wxNullColour);
#else
        XRCCTRL(*this, "AssStore", wxButton)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);
#endif
    }

    XRCCTRL(*this, "AssInsert", wxButton)->Enable(!status);
    XRCCTRL(*this, "AssDelete", wxButton)->Enable(!status);
    XRCCTRL(*this, "AssSaveAll", wxButton)->Enable(!status);
    XRCCTRL(*this, "AssFrom", wxButton)->Enable(!status);
    XRCCTRL(*this, "AssFromL", wxButton)->Enable(!status);
    XRCCTRL(*this, "AssFromV", wxButton)->Enable(!status);
    XRCCTRL(*this, "AssFromA", wxButton)->Enable(!status);
    XRCCTRL(*this, "AssRangeSpin", wxSpinButton)->Enable(!status);
    enableStartButtonGui(status);
}

void Main::message(wxString buffer)
{
    if (!mode_.gui)
    {
        if (mode_.verbose) 
        {
            buffer = buffer + "\n"; 
            wxMessageOutput::Get()->Printf("%s", buffer.mb_str());
        }
    } 
    else
    {
        XRCCTRL(*this,"Message_Window",wxTextCtrl)->AppendText(buffer);
        XRCCTRL(*this,"Message_Window",wxTextCtrl)->AppendText("\n");
    }
}

void Main::messageNoReturn(wxString buffer)
{
    if (!mode_.gui)
    {
        if (mode_.verbose)
            wxMessageOutput::Get()->Printf("%s", buffer.mb_str());
    }
    else
        XRCCTRL(*this, "Message_Window", wxTextCtrl)->AppendText(buffer);
}

void Main::messageInt(int value)
{
    wxString buffer;

    buffer.Printf("%d", value);
    message(buffer);
}

void Main::messageHex(int value)
{
    wxString buffer;

    buffer.Printf("%04X", value);
    message(buffer);
}

void Main::eventMessageHex(int value)
{
    wxString buffer;
    
    buffer.Printf("%04X ", value);
    eventShowTextMessage(buffer);
}

wxString Main::getApplicationDir()
{
    return applicationDirectory_;
}

wxString Main::getPathSep()
{
    return pathSeparator_;
}

int Main::setFdcStepRate(int rate)
{
    switch(rate)
    {
        case 0: return conf[runningComputer_].fdcCpms_ * 6;
        case 1: return conf[runningComputer_].fdcCpms_ * 12;
        case 2: return conf[runningComputer_].fdcCpms_ * 20;
        case 3: return conf[runningComputer_].fdcCpms_ * 30;
    }
    return conf[runningComputer_].fdcCpms_ * 12;
}

int Main::getFdcCpms()
{
    return conf[runningComputer_].fdcCpms_;
}

int Main::getPsaveData(int item)
{
    return psaveData_[item];
}

void Main::setPsaveData(int item, int data)
{
    psaveData_[item] = data;
    if (computerRunning_)
        p_Computer->setPsaveSettings();
}

int Main::getFunctionKey(int item)
{
    return functionKey_[item];
}

void Main::setFunctionKey(int item, int value)
{
    functionKey_[item] = value;
}

void Main::zoomEvent(double zoom, int videoNumber)
{
    if (!mode_.gui)
        return;

    if (zoomEventOngoing_)
        return;

    zoomEventOngoing_ = true;
//    wxString zoomStr;
//    zoomStr.Printf("%2.2f", zoom);
//    XRCCTRL(*this, "ZoomValue"+computerInfo[runningComputer_].gui, wxTextCtrl)->ChangeValue(zoomStr);

    conf[runningComputer_].zoom_[videoNumber].Printf("%2.2f", zoom);
    correctZoomAndValue(runningComputer_, computerInfo[runningComputer_].gui, SET_SPIN, videoNumber);
}

void Main::zoomEventVt(double zoom)
{
    if (!mode_.gui)
        return;

    if (zoomEventOngoing_)
        return;

//    wxString zoomStr;
//    zoomStr.Printf("%2.2f", zoom);
//    XRCCTRL(*this, "ZoomValueVt"+computerInfo[runningComputer_].gui, wxTextCtrl)->ChangeValue(zoomStr);

    conf[runningComputer_].zoomVt_.Printf("%2.2f", zoom);
    correctZoomVtAndValue(runningComputer_, computerInfo[runningComputer_].gui, SET_SPIN);
}


void Main::traceTimeout(wxTimerEvent&WXUNUSED(event))
{
    if (selectedComputer_ == DEBUGGER && debuggerChoice_ == TRACETAB)
    {
        wxString buffer;
        if (traceString_ != "")
        {
            buffer = traceString_;
            traceString_ = "";
            traceWindowPointer->SetInsertionPointEnd();
            traceWindowPointer->WriteText(buffer);
        }
    }
    if (selectedComputer_ == DEBUGGER && debuggerChoice_ == CHIP8TAB)
    {
        wxString buffer;
        if (chipTraceString_ != "")
        {
            buffer = chipTraceString_;
            chipTraceString_ = "";
            chip8TraceWindowPointer->SetInsertionPointEnd();
            chip8TraceWindowPointer->WriteText(buffer);
        }
    }
}

void Main::directAssTimeout(wxTimerEvent&WXUNUSED(event))
{
    if (selectedComputer_ == DEBUGGER)
    {
        switch (debuggerChoice_)
        {
            case MESSAGETAB:
            break;

            case TRACETAB:
                if (percentageClock_ == 1)
                    p_Main->updateWindow();
            break;

            case DIRECTASSTAB:
                if (updateAssPage_)
                {
                    directAss();
                    updateAssPage_ = false;
                }
            break;

            case PROFILERTAB:
                showTime();
                if (updateAssPage_)
                {
                    directAss();
                    updateAssPage_ = false;
                }
            break;

            case MEMORYTAB:
                if (updateMemoryPage_)
                {
                    memoryDisplay();
                    updateMemoryPage_ = false;
                }

                if (updateSlotinfo_)
                {
                    wxString slotValue;
                    switch (runningComputer_)
                    {
                        case COMX:
                            XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->changeNumber(p_Comx->getComxExpansionSlot()+1);
                            if (p_Comx->isRamCardActive())
                                XRCCTRL(*this, "DebugExpansionRam", SlotEdit)->changeNumber(p_Comx->getComxExpansionRamBank());
                            if (p_Comx->isEpromBoardLoaded() || p_Comx->isSuperBoardLoaded())
                                XRCCTRL(*this, "DebugExpansionEprom", HexEdit)->changeNumber(p_Comx->getComxExpansionEpromBank());
                        break;
                        case ELF:
                        case ELFII:
                        case SUPERELF:
                        case PICO:
                            if (elfConfiguration[runningComputer_].usePager)
                            {
                                XRCCTRL(*this, "DebugPager", HexEdit)->changeNumber(p_Computer->getPager(portExtender_));
                                XRCCTRL(*this, "DebugPortExtender", HexEdit)->changeNumber(portExtender_);
                            }
                            if (elfConfiguration[runningComputer_].useEms)
                                XRCCTRL(*this, "DebugEmsPage", HexEdit)->changeNumber(p_Computer->getEmsPage(emsNumber_));
                        break;

                        case XML:
                            if (elfConfiguration[runningComputer_].usePager)
                            {
                                XRCCTRL(*this, "DebugPager", HexEdit)->changeNumber(p_Computer->getPager(portExtender_));
                                XRCCTRL(*this, "DebugPortExtender", HexEdit)->changeNumber(portExtender_);
                            }
                            if (elfConfiguration[runningComputer_].useEms)
                            {
                                XRCCTRL(*this, "DebugEmsNumber", HexEdit)->changeNumber((int)emsNumber_);
                                XRCCTRL(*this, "DebugEmsPage", HexEdit)->changeNumber(p_Computer->getEmsPage(emsNumber_));
                            }
                        break;
                    }
                    updateSlotinfo_ = false;
                }

                if (updateMemory_ && memoryDisplay_ != CPU_TYPE && memoryDisplay_ != CPU_PROFILER)
                {
                    updateMemory_ = false;
                    wxString idReference, valueStr;
                    for (int y=0; y<16; y++)
                    {
                        if (rowChanged_[y])
                        {
                            rowChanged_[y] = false;
                            ShowCharacters(memoryStart_+(y*16), y);
                            for (int x=0; x<16; x++)
                            {
                                if (memoryChanged_[x][y])
                                {
                                    memoryChanged_[x][y] = false;
                                    idReference.Printf("MEM%01X%01X", y, x);

                                    switch (memoryDisplay_)
                                    {
                                        case V_6847_RAM:
                                            if (elfConfiguration[runningComputer_].use6847)
                                            {
                                                if (memoryStart_ <= getAddressMask())
                                                    XRCCTRL(*this, idReference, MemEdit)->changeNumber2X(debugReadMem(memoryStart_+y*16+x));
                                                else
                                                    XRCCTRL(*this, idReference, MemEdit)->changeNumber1X(debugReadMem(memoryStart_+y*16+x));
                                            }
                                            else
                                                XRCCTRL(*this, idReference, MemEdit)->changeNumber2X(0);
                                        break;
                                            
                                        case CDP_1870_COLOUR:
                                            if (runningComputer_ == TMC600)
                                                XRCCTRL(*this, idReference, MemEdit)->changeNumber1X(debugReadMem(memoryStart_+y*16+x));
                                        break;
                                               
                                        case RTCRAM:
                                            if ((memoryStart_+y*16+x) < 0x7f)
                                                XRCCTRL(*this, idReference, MemEdit)->changeNumber2X(debugReadMem(memoryStart_+y*16+x));
                                        break;

                                        default:
                                            XRCCTRL(*this, idReference, MemEdit)->changeNumber2X(debugReadMem(memoryStart_+y*16+x));
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            break;
        }
    }
}

void Main::vuTimeout(wxTimerEvent&WXUNUSED(event))
{
    switch (runningComputer_)
    {
        case FRED1:
        case FRED1_5:
        case COSMICOS:
        case ELF: 
        case ELFII:
        case SUPERELF:
        case COMX:
        case VIP: 
        case VIPII:
        case VIP2K:
        case VELF:
        case TMC600:
        case TMC1800:
        case TMC2000:
        case PECOM:
        case ETI:
        case NANO:
        case STUDIOIV:
        case XML:
        case PICO:
            vuSet("Vu"+computerInfo[runningComputer_].gui, p_Computer->getGaugeValue());
        break;
    }
}

void Main::updateMemoryTab()
{
    updateMemoryPage_ = true;
}

void Main::updateAssTab()
{
    updateAssPage_ = true;
}

void Main::updateSlotInfo()
{
    updateSlotinfo_ = true;
    updateMemoryTab();
    updateAssTab();
}

void Main::updateCheckTimeout(wxTimerEvent&WXUNUSED(event))
{
    bool checkForUpdate;
    configPointer->Read("/Main/Check_For_Update", &checkForUpdate, true);

    if (checkForUpdate)
    {
        updateCheckStarted_ = true;

        m_pUpdateCheckThread = new UpdateCheckThread(this);
        m_pUpdateCheckThread->Run();
        
/*        if (p_Main->checkUpdateEmma())
        {
            if (p_Main->updateEmma())
                Destroy();
        }*/
    }
}

/*
void Main::cpuTimeout(wxTimerEvent&WXUNUSED(event))
{
    if (selectedComputer_ == DEBUGGER && debuggerChoice_ == MESSAGETAB)
        showTime();
}*/

void Main::startTime()
{
    startTime_ = wxGetLocalTime();
    lapTime_ = 0;
    lapTimeStart_ = 0;
    lastNumberOfCpuCycles_ = 0;
    lastInstructionCounter_= 0;
    cpuCyclesOverflow_ = false;
}

void Main::showTime()
{
    wxString print_buffer;
    int h,m,s;
    double f1,f2;
    float videoFreq;
    time_t endTime;

    uint64_t cpuCycles = p_Computer->getCpuCycles();
    uint64_t instructionCounter = p_Computer->getInstructionCounter();

    endTime = wxGetLocalTime();
    s = (int)(endTime - startTime_);
    
    if (instructionCounter != 0 && lastInstructionCounter_ != instructionCounter)
    {
        if (instructionCounter < lastInstructionCounter_)
            instructionCounterOverflow_ = true;

        if (instructionCounterOverflow_)
            print_buffer = "---";
        else
            print_buffer.Printf(wxNumberFormatter::ToString((double)instructionCounter, 0));

#if wxCHECK_VERSION(2, 9, 0)
        XRCCTRL(*this, "InstructionCounter", wxStaticText)->SetLabelText(print_buffer);
#else
        XRCCTRL(*this, "InstructionCounter", wxStaticText)->SetLabel(print_buffer);
#endif

        if (instructionCounterOverflow_)
            print_buffer = "---";
        else
            print_buffer.Printf(wxNumberFormatter::ToString((double)instructionCounter/s, 0));

#if wxCHECK_VERSION(2, 9, 0)
        XRCCTRL(*this, "InstructionsPerSecond", wxStaticText)->SetLabelText(print_buffer);
#else
        XRCCTRL(*this, "InstructionsPerSecond", wxStaticText)->SetLabel(print_buffer);
#endif
    }
    lastInstructionCounter_ = instructionCounter;

    if (cpuCycles != 0 && lastNumberOfCpuCycles_ != cpuCycles)
    {
        if (cpuCycles < lastNumberOfCpuCycles_)
            cpuCyclesOverflow_ = true;

        h = s / 3600;
        s -= (h * 3600);
        m = s / 60;
        s -= (m * 60);
        f2 = endTime - startTime_;
        f1 = cpuCycles;
        f1 /= f2;
        f1 = f1 / 1000000 * 8;

        if (cpuCyclesOverflow_)
            print_buffer = "---";
        else
            print_buffer.Printf(wxNumberFormatter::ToString((double)cpuCycles, 0));

#if wxCHECK_VERSION(2, 9, 0)
        XRCCTRL(*this, "CpuCycles", wxStaticText)->SetLabelText(print_buffer);
#else
        XRCCTRL(*this, "CpuCycles", wxStaticText)->SetLabel(print_buffer);
#endif

        print_buffer.Printf("%02d:%02d:%02d",h,m,s);
#if wxCHECK_VERSION(2, 9, 0)
        XRCCTRL(*this, "RunTime", wxStaticText)->SetLabelText(print_buffer);
#else
        XRCCTRL(*this, "RunTime", wxStaticText)->SetLabel(print_buffer);
#endif

        if (lapTimeStart_ != 0)
        {
            endTime = wxGetLocalTime();
            lapTime_ = (int)(endTime - lapTimeStart_);
        }
        s = (int)(lapTime_);
        h = s / 3600;
        s -= (h * 3600);
        m = s / 60;
        s -= (m * 60);

        print_buffer.Printf("%02d:%02d:%02d",h,m,s);
#if wxCHECK_VERSION(2, 9, 0)
        XRCCTRL(*this, "LapTime", wxStaticText)->SetLabelText(print_buffer);
#else
        XRCCTRL(*this, "LapTime", wxStaticText)->SetLabel(print_buffer);
#endif
        
        if (cpuCyclesOverflow_)
            print_buffer = "---";
        else
            print_buffer.Printf("%6.3f MHz",f1);
#if wxCHECK_VERSION(2, 9, 0)
        XRCCTRL(*this, "EffectiveClock", wxStaticText)->SetLabelText(print_buffer);
#else
        XRCCTRL(*this, "EffectiveClock", wxStaticText)->SetLabel(print_buffer);
#endif

        if (p_Video[VIDEOMAIN] != NULL)
        {
            long videoSyncCount = p_Video[VIDEOMAIN]->getVideoSyncCount();
            if (videoSyncCount != 0)
            {
                videoFreq = (float) (videoSyncCount / f2);
                print_buffer.Printf("%2.1f Hz",videoFreq);
            }
        }
        else
            print_buffer = "---";
#if wxCHECK_VERSION(2, 9, 0)
        XRCCTRL(*this, "VideoFrequency", wxStaticText)->SetLabelText(print_buffer);
#else
        XRCCTRL(*this, "VideoFrequency", wxStaticText)->SetLabel(print_buffer);
#endif
    }
    lastNumberOfCpuCycles_ = cpuCycles;
}

void Main::lapTime()
{
    if (lapTimeStart_ == 0)
        lapTimeStart_ = wxGetLocalTime();
    else
    {
        time_t endTime;

        endTime = wxGetLocalTime();
        lapTime_ = (int)(endTime - lapTimeStart_);

        lapTimeStart_ = 0;
    }
}

void Main::vuSet(wxString item, int gaugeValue)
{
#ifndef __linux__
    if (gaugeValue == oldGauge_)
        return;

    oldGauge_ = gaugeValue;

    wxBitmap vu(100, VU_HI, 24);
    wxMemoryDC dcVu;

    dcVu.SelectObject(vu);
    dcVu.SetPen(wxPen(wxColour(0, 0xc0, 0)));
    dcVu.SetBrush(wxBrush(wxColour(0, 0xc0, 0)));

    int gaugeGreen = gaugeValue / 60;
    if (gaugeGreen > VU_RED)  gaugeGreen = VU_RED;
    dcVu.DrawRectangle(0, 0, gaugeGreen, VU_HI);

    if (gaugeGreen < VU_RED)
    {
        dcVu.SetPen(wxPen(wxColour(0, 0x22, 0)));
        dcVu.SetBrush(wxBrush(wxColour(0, 0x22, 0)));
        dcVu.DrawRectangle(gaugeGreen, 0, VU_RED-gaugeGreen, VU_HI);
    }

    dcVu.SetPen(wxPen(wxColour(0xc0, 0, 0)));
    dcVu.SetBrush(wxBrush(wxColour(0xc0, 0, 0)));

    int gaugeRed = gaugeValue / 60;
    if (gaugeRed > VU_MAX)  gaugeRed = VU_MAX;
    if (gaugeRed < VU_RED)  gaugeRed = VU_RED;
    dcVu.DrawRectangle(VU_RED, 0, gaugeRed-VU_RED, VU_HI);

    if (gaugeRed < VU_MAX)
    {
        dcVu.SetPen(wxPen(wxColour(0x22, 0, 0)));
        dcVu.SetBrush(wxBrush(wxColour(0x22, 0, 0)));
        dcVu.DrawRectangle(gaugeRed, 0, VU_MAX-gaugeRed, VU_HI);
    }
    dcVu.SelectObject(wxNullBitmap);
    XRCCTRL(*this, item, wxStaticBitmap)->SetBitmap(vu);
#endif
}

void Main::sysColourChangeEvent(wxSysColourChangedEvent& event)
{
    setSysColours();
    
    if (mode_.gui)
        paintDebugBackground();

    if (computerRunning_ && selectedComputer_ == DEBUGGER)
    {
        if (debuggerChoice_ == DIRECTASSTAB || debuggerChoice_ == PROFILERTAB)
            directAss();

        if (debuggerChoice_ == MEMORYTAB)
            memoryDisplay();
    }

    event.Skip();
}

void Main::setSysColours()
{
#if defined (__WXMAC__) || (__linux__)
    wxSystemAppearance system = wxSystemSettings::GetAppearance();
    
    darkMode_ = system.IsDark();
    guiBackGround_ = wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK);
    guiTextColour[GUI_COL_BLACK] = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
#else
    darkMode_ = false;
    guiBackGround_ = wxColour(windowInfo.red, windowInfo.green, windowInfo.blue);
    wxColourDatabase colour;
    guiTextColour[GUI_COL_BLACK] = wxColour(colour.Find("BLACK"));
#endif
    
    if (darkMode_)
    {
        guiTextColour[GUI_COL_BLUE] = wxColour(0xdc, 0xcc, 0x8b);
        guiTextColour[GUI_COL_PINK] = wxColour(0xfe, 0x8a, 0xb6);
        guiTextColour[GUI_COL_RED] = wxColour(0xc6, 0x28, 0x28);
        guiTextColour[GUI_COL_STEEL] = wxColour(0x43, 0x95, 0x96);
        guiTextColour[GUI_COL_ORANGE] = wxColour(0xfc, 0x59, 0x00);
        guiTextColour[GUI_COL_PURPLE] = wxColour(0x67, 0x3a, 0xb7);
        guiTextColour[GUI_COL_GREY] = wxColour(0x9e, 0xa5, 0xad);
        guiTextColour[GUI_COL_GREEN] = wxColour(0, 0x89, 0x7B);
    }
    else
    {
        guiTextColour[GUI_COL_BLUE] = wxColour(0x2e, 0x41, 0xd5);
        guiTextColour[GUI_COL_PINK] = wxColour(0xb2, 0x51, 0xA7);
        guiTextColour[GUI_COL_RED] = wxColour(0xc6, 0x28, 0x28);
        guiTextColour[GUI_COL_STEEL] = wxColour(0x43, 0x95, 0x96);
        guiTextColour[GUI_COL_ORANGE] = wxColour(0xfc, 0x59, 0x00);
        guiTextColour[GUI_COL_PURPLE] = wxColour(0x67, 0x3a, 0xb7);
        guiTextColour[GUI_COL_GREY] = wxColour(0x9e, 0xa5, 0xad);
        guiTextColour[GUI_COL_GREEN] = wxColour(0, 0x89, 0x7B);
    }
    setMemDumpColours();
}

void Main::setMemDumpColours()
{
   if (!mode_.gui)
      return;
   XRCCTRL(*this, "DebugExpansionSlotText", wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_ORANGE]);
   XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->SetForegroundColour(guiTextColour[GUI_COL_ORANGE]);

   XRCCTRL(*this, "DebugExpansionRamText", wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_GREEN]);
   XRCCTRL(*this, "DebugExpansionRam", SlotEdit)->SetForegroundColour(guiTextColour[GUI_COL_GREEN]);

   XRCCTRL(*this, "DebugExpansionEpromText", wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_PINK]);
   XRCCTRL(*this, "DebugExpansionEprom", HexEdit)->SetForegroundColour(guiTextColour[GUI_COL_PINK]);

   XRCCTRL(*this, "DebugEmsPageText", wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLUE]);
   XRCCTRL(*this, "DebugEmsNumber", HexEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLUE]);
   XRCCTRL(*this, "DebugEmsPage", HexEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLUE]);

   XRCCTRL(*this, "DebugPagerText", wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_PURPLE]);
   XRCCTRL(*this, "DebugPortExtender", HexEdit)->SetForegroundColour(guiTextColour[GUI_COL_PURPLE]);
   XRCCTRL(*this, "DebugPager", HexEdit)->SetForegroundColour(guiTextColour[GUI_COL_PURPLE]);
}

void Main::errorMessageEvent(wxErrorMsgEvent&event)
{
    wxString message = event.GetMsg();
    (void)wxMessageBox( message,
                        "Emma 02", wxICON_ERROR | wxOK );
}

void Main::errorMessage(wxString msg)
{
    wxErrorMsgEvent event( wxEVT_ERROR_MSG, GetId() );
    event.SetEventObject( p_Main );
    event.SetMsg( msg );
    GetEventHandler()->AddPendingEvent( event );
}

wxErrorMsgEvent::wxErrorMsgEvent( wxEventType commandType, int id )
: wxNotifyEvent( commandType, id )
{
    message = "";
}

void Main::setLocationEvent(guiEvent&WXUNUSED(event))
{
    wxString printBuffer;

    if (popupDialog_ != NULL)
        popupDialog_->setLocation(conf[runningComputer_].useLoadLocation_, conf[runningComputer_].saveStartString_, conf[runningComputer_].saveEndString_, conf[runningComputer_].saveExecString_);
    if (!mode_.gui)
        return;

    if (conf[runningComputer_].useLoadLocation_)
    {
        XRCCTRL(*this,"SaveStart"+computerInfo[runningComputer_].gui, wxTextCtrl)->ChangeValue(conf[runningComputer_].saveStartString_);
        XRCCTRL(*this,"SaveEnd"+computerInfo[runningComputer_].gui, wxTextCtrl)->ChangeValue(conf[runningComputer_].saveEndString_);
        XRCCTRL(*this,"SaveExec"+computerInfo[runningComputer_].gui, wxTextCtrl)->ChangeValue(conf[runningComputer_].saveExecString_);
    }
    else
    {
        XRCCTRL(*this,"SaveStart"+computerInfo[runningComputer_].gui, wxTextCtrl)->ChangeValue("");
        XRCCTRL(*this,"SaveEnd"+computerInfo[runningComputer_].gui, wxTextCtrl)->ChangeValue("");
        XRCCTRL(*this,"SaveExec"+computerInfo[runningComputer_].gui, wxTextCtrl)->ChangeValue("");
    }
    XRCCTRL(*this, "UseLocation"+computerInfo[runningComputer_].gui, wxCheckBox)->SetValue(conf[runningComputer_].useLoadLocation_);
    enableLocationGui();
}

void Main::eventSetLocation(bool state, Word saveStart, Word saveEnd, Word saveExec)
{
     wxString printBuffer;
    guiEvent event(GUI_MSG, SET_LOCATION);
    event.SetEventObject( p_Main );

     conf[runningComputer_].useLoadLocation_ = state;
    conf[runningComputer_].saveStart_ = saveStart;
    conf[runningComputer_].saveEnd_ = saveEnd;
    conf[runningComputer_].saveExec_ = saveExec;

    printBuffer.Printf("%04X",saveStart);
     conf[runningComputer_].saveStartString_ = printBuffer;
    printBuffer.Printf("%04X",saveEnd);
     conf[runningComputer_].saveEndString_ = printBuffer;
    printBuffer.Printf("%04X",saveExec);
     conf[runningComputer_].saveExecString_ = printBuffer;

    GetEventHandler()->AddPendingEvent(event);
}

void Main::eventSetLocation(bool state)
{
    guiEvent event(GUI_MSG, SET_LOCATION);
    event.SetEventObject( p_Main );

    conf[runningComputer_].useLoadLocation_ = state;
    conf[runningComputer_].saveExec_ = 0;

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setEnableClockEvent(guiEvent& event)
{
   clockTextCtrl[event.GetInt()]->Enable(event.GetBoolValue());
}

void Main::eventEnableClock(bool state)
{
   guiEvent event(GUI_MSG, ENABLE_CLOCK);
   event.SetEventObject( p_Main );

   event.SetInt(runningComputer_);
   event.SetBoolValue(state);

   GetEventHandler()->AddPendingEvent(event);
}

void Main::setHwTapeStateEvent(guiEvent&WXUNUSED(event))
{
   if (!mode_.gui)
      return;
   
   switch (hwTapeState_)
   {
      case HW_TAPE_STATE_PLAY:
         XRCCTRL(*this, "CasLoad"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(playDarkGreenBitmap);
         XRCCTRL(*this, "CasForward"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
         XRCCTRL(*this, "CasRewind"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
         XRCCTRL(*this, "CasSave"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(recOffBitmap);
      break;
        
      case HW_TAPE_STATE_FF:
         XRCCTRL(*this, "CasLoad"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
         XRCCTRL(*this, "CasForward"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(forwardDarkGreenBitmap);
         XRCCTRL(*this, "CasRewind"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
         XRCCTRL(*this, "CasSave"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(recOffBitmap);
      break;
        
      case HW_TAPE_STATE_RW:
         XRCCTRL(*this, "CasLoad"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
         XRCCTRL(*this, "CasForward"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
         XRCCTRL(*this, "CasRewind"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(rewindDarkGreenBitmap);
         XRCCTRL(*this, "CasSave"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(recOffBitmap);
      break;

      case HW_TAPE_STATE_REC:
         XRCCTRL(*this, "CasLoad"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
         XRCCTRL(*this, "CasForward"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
         XRCCTRL(*this, "CasRewind"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
         XRCCTRL(*this, "CasSave"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(recOnBitmap);
      break;
        
      default:
         hwTapeState_ = HW_TAPE_STATE_OFF;
         XRCCTRL(*this, "CasLoad"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
         XRCCTRL(*this, "CasForward"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
         XRCCTRL(*this, "CasRewind"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
         XRCCTRL(*this, "CasSave"+computerInfo[XML].gui, wxBitmapButton)->SetBitmapLabel(recOffBitmap);
      break;
   }
}

void Main::eventHwTapeStateChange(int status)
{
   hwTapeState_ = status;
   
   guiEvent event(GUI_MSG, CHANGE_HW_TAPE_STATE);
   event.SetEventObject( p_Main );
   
   event.SetInt(status);

   GetEventHandler()->AddPendingEvent(event);
}

void Main::setSaveStartEvent(guiEvent&WXUNUSED(event))
{
    if (popupDialog_ != NULL)
        popupDialog_->setStartLocation(conf[runningComputer_].saveStartString_);
    if (!mode_.gui)
        return;

    XRCCTRL(*this,"SaveStart"+computerInfo[runningComputer_].gui, wxTextCtrl)->SetValue(conf[runningComputer_].saveStartString_);
}

void Main::eventSaveStart(Word saveStart)
{
    guiEvent event(GUI_MSG, SET_SAVE_START);
    event.SetEventObject( p_Main );

    conf[runningComputer_].saveStart_ = saveStart;
    conf[runningComputer_].saveStartString_.Printf("%04X",saveStart);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setSaveEndEvent(guiEvent&WXUNUSED(event))
{
    if (popupDialog_ != NULL)
        popupDialog_->setEndLocation(conf[runningComputer_].saveEndString_);
    if (!mode_.gui)
        return;

    XRCCTRL(*this,"SaveEnd"+computerInfo[runningComputer_].gui, wxTextCtrl)->SetValue(conf[runningComputer_].saveEndString_);
}

void Main::eventSaveEnd(Word saveEnd)
{
    guiEvent event(GUI_MSG, SET_SAVE_END);
    event.SetEventObject( p_Main );

    conf[runningComputer_].saveEnd_ = saveEnd;
    conf[runningComputer_].saveEndString_.Printf("%04X",saveEnd);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setSwNameEvent(guiEvent&event)
{
    wxString name = event.GetString();

    setSwName(name);
    updateTitle();
}

void Main::eventSetSwName(wxString swName)
{
    guiEvent event(GUI_MSG, SET_SW_NAME);
    event.SetEventObject( p_Main );

    event.SetString(swName);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setTapeStateEvent(guiEvent&event)
{
    int tapeState = event.GetInt();
    wxString tapeNumber = event.GetString();

    setTapeState(tapeState, tapeNumber);
}

void Main::eventSetTapeState(int tapeState, wxString tapeNumber)
{
    guiEvent event(GUI_MSG, SET_TAPE_STATE);
    event.SetEventObject( p_Main );
    
    event.SetInt(tapeState);
    event.SetString(tapeNumber);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setConvertStateEvent(guiEvent&event)
{
    bool convertState = event.GetBoolValue();

    XRCCTRL(*this,"BatchConvertButtonComx", wxButton)->Enable(convertState);
    XRCCTRL(*this,"BatchButtonComx", wxButton)->Enable(convertState);
}

void Main::eventSetConvertState(bool convertState)
{
    guiEvent event(GUI_MSG, SET_CONVERT_STATE);
    event.SetEventObject( p_Main );
    
    event.SetBoolValue(convertState);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setTextValueEvent(guiEvent&event)
{
    wxString info = event.GetString();
    wxString value = event.GetStringValue2();

    XRCCTRL(*this, info, wxTextCtrl)->SetValue(value); 
}

void Main::eventSetTextValue(wxString info, wxString value)
{
    if (!mode_.gui)
        return;

    guiEvent event(GUI_MSG, SET_TEXT_VALUE);
    event.SetEventObject( p_Main );

    event.SetString(info);
    event.SetStringValue2(value);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setStaticTextValueEvent(guiEvent&event)
{
    wxString info = event.GetString();
    wxString value = event.GetStringValue2();
    
    XRCCTRL(*this, info, wxStaticText)->SetLabel(value);
}

void Main::eventSetStaticTextValue(wxString info, wxString value)
{
    if (!mode_.gui)
        return;
    
    guiEvent event(GUI_MSG, SET_STATIC_TEXT_VALUE);
    event.SetEventObject( p_Main );
    
    event.SetString(info);
    event.SetStringValue2(value);
    
    GetEventHandler()->AddPendingEvent(event);
}

void Main::setButtonLabelEvent(guiEvent&event)
{
    wxString info = event.GetString();
    wxString value = event.GetStringValue2();
    
    XRCCTRL(*this, info, wxButton)->SetLabel(value);
}

void Main::eventSetButtonLabel(wxString info, wxString value)
{
    if (!mode_.gui)
        return;
    
    guiEvent event(GUI_MSG, SET_BUTTON_LABEL);
    event.SetEventObject( p_Main );
    
    event.SetString(info);
    event.SetStringValue2(value);
    
    GetEventHandler()->AddPendingEvent(event);
}

void Main::setCheckBoxEvent(guiEvent&event)
{
    wxString info = event.GetString();
    bool status = event.GetBoolValue();

    setCheckBox(info, status);
}
 
void Main::eventSetCheckBox(wxString info, bool state)
{
    if (!mode_.gui)
        return;
    guiEvent event(GUI_MSG, SET_CHECK_BOX);
    event.SetEventObject( p_Main );

    event.SetString(info);
    event.SetBoolValue(state);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::showMessageEvent(guiEvent&event)
{
    Word value = event.GetInt();
    
    wxString buffer;
    
    buffer.Printf("%04X", value);
    message(buffer);
}

void Main::eventShowMessage(Word message)
{
    guiEvent event(GUI_MSG, SHOW_MESSAGE);
    event.SetEventObject( p_Main );
    
    event.SetInt(message);
    
    GetEventHandler()->AddPendingEvent(event);
}

void Main::showTextMessageEvent(guiEvent&event)
{
    wxString messageText = event.GetString();
    message(messageText);
}

void Main::eventShowTextMessage(wxString messageText)
{
    guiEvent event(GUI_MSG, SHOW_TEXT_MESSAGE);
    event.SetEventObject(p_Main);

    event.SetString(messageText);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setZoomChange(guiEvent&event)
{
    double zoom = event.GetDoubleValue();
    int videoNumber  = event.GetInt();

    if (p_Video[videoNumber] == NULL)
       return;
   
    p_Video[videoNumber]->setZoom(zoom);
    switch(runningComputer_)
    {
        case CIDELSA:
        case COMX:
        case VIPII:
#if defined(__linux__)
            guiRedrawBarTimeOutPointer->Start(200, wxTIMER_ONE_SHOT);
#else
            p_Video[videoNumber]->reDrawBar();
#endif
        break;
    }
    zoomEventFinished();
}

void Main::eventZoomChange(double zoom, int videoNumber)
{
    guiEvent event(GUI_MSG, ZOOM_CHANGE);
    event.SetEventObject(p_Main);

    event.SetDoubleValue(zoom);
    event.SetInt(videoNumber);

    GetEventHandler()->AddPendingEvent(event);
}

bool Main::isZoomEventOngoing()
{
    return zoomEventOngoing_;
}

bool Main::isZoomEventOngoingButNotFullScreen()
{
   if (fullScreenEventOngoing_)
      return false;
   
   return zoomEventOngoing_;
}

void Main::zoomEventFinished()
{
    zoomEventOngoing_ = false;
    fullScreenEventOngoing_ = false;
}

void Main::setZoomVtChange(guiEvent&event)
{
    double zoom = event.GetDoubleValue();
    int uartNumber  = event.GetInt();

    p_Vt100[uartNumber]->setZoom(zoom);
//    if (runningComputer_ != ELF2K && runningComputer_ != MEMBER)
//        p_Vt100[UART1]->copyScreen();
    zoomVtEventFinished();
}

void Main::eventZoomVtChange(double zoom, int uartNumber)
{
    guiEvent event(GUI_MSG, ZOOMVT_CHANGE);
    event.SetEventObject(p_Main);

    event.SetDoubleValue(zoom);
    event.SetInt(uartNumber);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::SetZoomEvent(guiEvent& event)
{
    bool isVt = event.GetBoolValue1();
    double zoom = event.GetDoubleValue();
    int videoNumber  = event.GetInt();

    if (isVt)
        p_Main->zoomEventVt(zoom);
    else
        p_Main->zoomEvent(zoom, videoNumber);

}

void Main::eventZoom(double zoom, int videoNumber, bool isVt)
{
    guiEvent event(GUI_MSG, EVENT_ZOOM);
    event.SetEventObject(p_Main);

    event.SetBoolValue1(isVt);
    event.SetDoubleValue(zoom);
    event.SetInt(videoNumber);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::zoomVtEventFinished()
{
    zoomEventOngoing_ = false;
    fullScreenEventOngoing_ = false;
}

void Main::printDefaultEvent(guiEvent&event)
{
    Byte value = event.GetByteValue1();

    p_Printer->printerOut(value);
}

void Main::eventPrintDefault(Byte value)
{
    guiEvent event(GUI_MSG, PRINT_DEFAULT);
    event.SetEventObject( p_Main );

    event.SetByteValue1(value);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::printParallelEvent(guiEvent&event)
{
    Byte value = event.GetByteValue1();

    p_PrinterParallel->outParallel(value);
    if (statusLedUpdate_)
        p_Main->statusLedOffDirect();
}

void Main::eventPrintParallel(Byte value)
{
    if (statusLedUpdate_)
        p_Main->statusLedOnEvent();
    guiEvent event(GUI_MSG, PRINT_PARALLEL);
    event.SetEventObject( p_Main );

    event.SetByteValue1(value);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::printParallelFinishedEvent(guiEvent&WXUNUSED(event))
{
    p_PrinterParallel->printFinished();
}

void Main::eventPrintParallelFinished()
{
    guiEvent event(GUI_MSG, PRINT_PARALLEL_FINISHED);
    event.SetEventObject( p_Main );

    GetEventHandler()->AddPendingEvent(event);
}

void Main::printThermalEvent(guiEvent&event)
{
    Byte value = event.GetByteValue1();
    Byte Qflag = event.GetByteValue2();

    p_PrinterThermal->outThermal(value, Qflag);
    thermalEf_ = false;
    if (statusLedUpdate_)
        p_Main->statusLedOffDirect();
}

void Main::eventPrintThermal(Byte value, Byte Qflag)
{
    if (statusLedUpdate_)
        p_Main->statusLedOnEvent();
    guiEvent event(GUI_MSG, PRINT_THERMAL);
    event.SetEventObject( p_Main );

    event.SetByteValue1(value);
    event.SetByteValue2(Qflag);

    thermalEf_ = true;
    GetEventHandler()->AddPendingEvent(event);
}

void Main::printThermalFinishedEvent(guiEvent&WXUNUSED(event))
{
    p_PrinterThermal->printFinished();
    thermalEf_ = false;
}

void Main::eventPrintThermalFinished()
{
    guiEvent event(GUI_MSG, PRINT_THERMAL_FINISHED);
    event.SetEventObject( p_Main );

    GetEventHandler()->AddPendingEvent(event);
}

void Main::printSerialEvent(guiEvent&event)
{
    Byte value = event.GetByteValue1();
    
    p_PrinterSerial->outSerial(value);
    if (statusLedUpdate_)
        p_Main->statusLedOffDirect();
}

void Main::eventPrintSerial(Byte value)
{
    if (statusLedUpdate_)
        p_Main->statusLedOnEvent();
    guiEvent event(GUI_MSG, PRINT_SERIAL);
    event.SetEventObject( p_Main );
    
    event.SetByteValue1(value);
    
    GetEventHandler()->AddPendingEvent(event);
}

void Main::printSerialFinishedEvent(guiEvent&WXUNUSED(event))
{
    p_PrinterSerial->printFinished();
}

void Main::eventPrintSerialFinished()
{
    guiEvent event(GUI_MSG, PRINT_SERIAL_FINISHED);
    event.SetEventObject( p_Main );

    GetEventHandler()->AddPendingEvent(event);
}

void Main::printPecomEvent(guiEvent&event)
{
    Byte value = event.GetByteValue1();

    p_Printer->printerOut(value);
}

void Main::eventPrintPecom(Byte value)
{
    guiEvent event(GUI_MSG, PRINT_PECOM);
    event.SetEventObject( p_Main );

    event.SetByteValue1(value);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::refreshVideoEvent(guiEvent&event)
{
    bool isVt = event.GetBoolValue1();
    int uart_video_Number = event.GetInt();

    if (isVt)
        p_Vt100[uart_video_Number]->refreshVideo();
    else
        p_Video[uart_video_Number]->refreshVideo();
    videoRefreshOngoing_ = false;
}

void Main::eventRefreshVideo(bool isVt, int uart_video_Number)
{
    if (videoRefreshOngoing_)
        return;
    
    videoRefreshOngoing_ = true;
    guiEvent event(GUI_MSG, REFRESH_VIDEO);
    event.SetEventObject( p_Main );

    event.SetBoolValue1(isVt);
    event.SetInt(uart_video_Number);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::refreshPanelEvent(guiEvent&WXUNUSED(event))
{
    p_Computer->refreshPanel();
    panelRefreshOngoing_ = false;
}

void Main::eventRefreshPanel()
{
    if (panelRefreshOngoing_)
        return;
    
    panelRefreshOngoing_ = true;
    guiEvent event(GUI_MSG, REFRESH_PANEL);
    event.SetEventObject( p_Main );

    GetEventHandler()->AddPendingEvent(event);
}

void Main::ShowMessageBoxEvent(guiEvent&event)
{
    wxString message = event.GetString();
    wxString caption = event.GetStringValue2();
    int style = event.GetInt();
    
    p_Main->setMessageBoxAnswer(wxMessageBox(message, caption, style));
}

int Main::eventShowMessageBox(wxString message, wxString caption, int style)
{
//#if defined (__WXMAC__)
//    messageBoxAnswer_ = wxMessageBox(message, caption, style);
//#else
    if (wxIsMainThread())
    {
        messageBoxAnswer_ = wxMessageBox(message, caption, style);
    }
    else
    {
        guiEvent event(GUI_MSG, SHOW_MESSAGE_BOX);
        event.SetEventObject( p_Main );
        
        event.SetString(message);
        event.SetStringValue2(caption);
        event.SetInt(style);
        
        messageBoxAnswer_ = -1;
        
        GetEventHandler()->AddPendingEvent(event);
        
        while (messageBoxAnswer_ == -1)
        {
            if (isComputerRunning())
                p_Computer->sleepComputer(1);
            else
                wxYield();
        }
    }
//#endif
    return messageBoxAnswer_;
}

void Main::setMessageBoxAnswer(int answer)
{
    messageBoxAnswer_ = answer;
}

void Main::GetClientSizeEvent(guiEvent&event)
{
    bool isVt = event.GetBoolValue1();
    int uart_video_Number = event.GetInt();

    if (isVt)
        clientSize_ = p_Vt100[uart_video_Number]->GetClientSize();
    else
        clientSize_ = p_Video[uart_video_Number]->GetClientSize();
}

wxSize Main::eventGetClientSize(bool isVt, int uart_video_Number)
{
    guiEvent event(GUI_MSG, GET_CLIENT_SIZE);
    event.SetEventObject( p_Main );
    
    event.SetBoolValue1(isVt);
    event.SetInt(uart_video_Number);

    clientSize_.x = -1;
    
    GetEventHandler()->AddPendingEvent(event);
    
    while (clientSize_.x == -1)
    {
        if (isComputerRunning())
            p_Computer->sleepComputer(1);
        else
            wxYield();
    }
    return clientSize_;
}

void Main::SetClientSizeEvent(guiEvent&event)
{
    wxSize size = event.GetSizeValue();
    bool changeScreenSize = event.GetBoolValue();
    bool isVt = event.GetBoolValue1();
    int uart_video_Number = event.GetInt();

    if (isVt)
    {
        p_Vt100[uart_video_Number]->setClientSize(size);
        if (changeScreenSize)
            p_Vt100[uart_video_Number]->changeScreenSize();
    }
    else
    {
        p_Video[uart_video_Number]->setClientSize(size);
        if (changeScreenSize)
            p_Video[uart_video_Number]->changeScreenSize();
    }
    sizeChanged_ = true;
}

void Main::eventSetClientSize(wxSize size, bool changeScreenSize, bool isVt, int uart_video_Number)
{
    sizeChanged_ = false;
    guiEvent event(GUI_MSG, SET_CLIENT_SIZE);
    event.SetEventObject( p_Main );
   
    event.SetBoolValue(changeScreenSize);
    event.SetBoolValue1(isVt);
    event.SetSizeValue(size);
    event.SetInt(uart_video_Number);

    GetEventHandler()->AddPendingEvent(event);

    while (!sizeChanged_)
    {
        if (isComputerRunning())
            p_Computer->sleepComputer(1);
        else
            wxYield();
    }
}

void Main::eventSetClientSize(int sizex, int sizey, bool changeScreenSize, bool isVt, int uartNumber)
{
    sizeChanged_ = false;
    guiEvent event(GUI_MSG, SET_CLIENT_SIZE);
    event.SetEventObject( p_Main );
   
    event.SetBoolValue(changeScreenSize);
    event.SetBoolValue1(isVt);
    event.SetInt(uartNumber);

    wxSize size;
    size.x = sizex;
    size.y = sizey;

    event.SetSizeValue(size);

    GetEventHandler()->AddPendingEvent(event);
    
    while (!sizeChanged_)
    {
        if (isComputerRunning())
            p_Computer->sleepComputer(1);
        else
            wxYield();
    }
}


void Main::ShowAddressPopupEvent(guiEvent&event)
{
    wxMessageDialog showAddressPopupDialog(this, "Select start address", "Binary load", wxCANCEL | wxYES_NO);

    wxString address;
    address.Printf("%04X", event.GetInt());
#if defined (__WXMAC__)
    showAddressPopupDialog.SetYesNoLabels(address, "0000");
#else
    showAddressPopupDialog.SetYesNoLabels("0000", address);
#endif

    p_Main->setAddressPopupAnswer(showAddressPopupDialog.ShowModal());
}

int Main::eventShowAddressPopup(Word specifiedStartAddress)
{
    if (wxIsMainThread())
    {
        wxMessageDialog showAddressPopupDialog(this, "Select start address", "Binary load", wxCANCEL | wxYES_NO);

        wxString address;
        address.Printf("%04X", specifiedStartAddress);
#if defined (__WXMAC__)
        showAddressPopupDialog.SetYesNoLabels(address, "0000");
#else
        showAddressPopupDialog.SetYesNoLabels("0000", address);
#endif
        messageAddressPopupAnswer_ = showAddressPopupDialog.ShowModal();
    }
    else
    {
        guiEvent event(GUI_MSG, SHOW_ADDRESS_POPUP);
        event.SetEventObject(p_Main);

        event.SetInt(specifiedStartAddress);

        messageAddressPopupAnswer_ = -1;

        GetEventHandler()->AddPendingEvent(event);

        while (messageAddressPopupAnswer_ == -1)
            p_Computer->sleepComputer(1);
    }
    return messageAddressPopupAnswer_;
}

void Main::setAddressPopupAnswer(int answer)
{
    messageAddressPopupAnswer_ = answer;
}

int Main::getAddressPopupAnswer()
{
    return messageAddressPopupAnswer_;
}

void Main::showFileSelectorEvent(guiEvent&event)
{
    wxString message = event.GetString();
    wxString default_path = event.GetStringValue2();
    wxString default_filename = event.GetStringValue3();
    wxString default_extension = event.GetStringValue4();
    wxString wildcard = event.GetStringValue5();
    int flags = event.GetInt();
   
    p_Main->setFileSelectorAnswer(wxFileSelector(message, default_path, default_filename, default_extension, wildcard, flags));
}

wxString Main::eventShowFileSelector(wxString message, wxString default_path, wxString default_filename, wxString default_extension, wxString wildcard, int flags)
{
    if (wxIsMainThread())
    {
        fileSelectorAnswer_ = wxFileSelector(message, default_path, default_filename, default_extension, wildcard, flags);
    }
    else
    {
        guiEvent event(GUI_MSG, SHOW_FILE_SELECTOR);
        event.SetEventObject( p_Main );
        
        event.SetString(message);
        event.SetStringValue2(default_path);
        event.SetStringValue3(default_filename);
        event.SetStringValue4(default_extension);
        event.SetStringValue5(wildcard);
        event.SetInt(flags);
        
        fileSelectorAnswer_ = "";
        
        GetEventHandler()->AddPendingEvent(event);
        
        while (fileSelectorAnswer_ == "")
            p_Computer->sleepComputer(1);

        if (fileSelectorCancel_)
            fileSelectorAnswer_ = "";
    }

    return fileSelectorAnswer_;
}

void Main::setFileSelectorAnswer(wxString answer)
{
    fileSelectorCancel_ = (answer == "");
    if (fileSelectorCancel_)
        answer = "CANCEL";
    
    fileSelectorAnswer_ = answer;
}

void Main::updateDebugMemory(Word address)
{
    if (selectedComputer_ == DEBUGGER)
    {
        if (debuggerChoice_ == MEMORYTAB)
        {
            updateMemory_ = true;

            int x, y;

            x = (address-memoryStart_) & 0xf;
            y = ((address-memoryStart_)  >> 4) & 0xf;

            rowChanged_[y] = true;
            memoryChanged_[x][y] = true;
        }
    }
}

void Main::showChip8Register(int variable, int value, bool chip8register12bit)
{
    wxString idReference;
    idReference.Printf("V%01X", variable);

    wxString valueStr;
    if (chip8register12bit)
        valueStr.Printf("%03X", value&0xfff);
    else
        valueStr.Printf("%02X", value);

    eventSetTextValue(idReference, valueStr);
//    XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue(valueStr);
}

void Main::setFandMBasicGuiEvent(guiEvent&WXUNUSED(event))
{
    setFandMBasicGui();
}

void Main::eventSetFandMBasicGui()
{
     if (!mode_.gui)
        return;
    guiEvent event(GUI_MSG, SET_FM_GUI);
    event.SetEventObject( p_Main );

    GetEventHandler()->AddPendingEvent(event);
}

void Main::enableMemAccesEvent(guiEvent&event)
{
    bool status = event.GetBoolValue();

    enableMemAccessGui(status);
    if (popupDialog_ != NULL)
        popupDialog_->enableMemAccessGui(status);
}

void Main::eventEnableMemAccess(bool state)
{
    if (!mode_.gui)
        return;
    guiEvent event(GUI_MSG, ENABLE_MEM_ACCESS);
    event.SetEventObject( p_Main );

    event.SetBoolValue(state);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setVideoFullScreenEvent(guiEvent&event)
{
    fullScreenEventOngoing_ = true;
    bool status = event.GetBoolValue();
    int videoNumber  = event.GetInt();
    if (p_Video[videoNumber] != NULL)
        p_Video[videoNumber]->setFullScreen(status);
}

void Main::eventVideoSetFullScreen(bool state, int videoNumber)
{
    guiEvent event(GUI_MSG, SET_VIDEO_FULLSCREEN);
    event.SetEventObject( p_Main );

    event.SetBoolValue(state);
    event.SetInt(videoNumber);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setVtFullScreenEvent(guiEvent&event)
{
    bool status = event.GetBoolValue();
    int uartNumber  = event.GetInt();
    if (p_Vt100[uartNumber] != NULL)
        p_Vt100[uartNumber]->setFullScreen(status);
}

void Main::eventVtSetFullScreen(bool state, int uartNumber)
{
    guiEvent event(GUI_MSG, SET_VT_FULLSCREEN);
    event.SetEventObject( p_Main );

    event.SetBoolValue(state);
    event.SetInt(uartNumber);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setChangeNoteBookEvent(guiEvent& WXUNUSED(event))
{
    XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COMXTAB);
    XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(DEBUGGERTAB);
    long selected_tab = configPointer->Read("/Main/Selected_Tab", 0l);
    if (selected_tab > DEBUGGERTAB)
        selected_tab = 0;
    XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(selected_tab);
}

void Main::eventChangeNoteBook()
{
    guiEvent event(GUI_MSG, CHANGE_NOTEBOOK);
    event.SetEventObject( p_Main );

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setDisableControlsEvent(guiEvent& WXUNUSED(event))
{
    XRCCTRL(*this, "TextStartComx", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartComx", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndComx", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndComx", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextExecComx", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextExecComx", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartElf2K", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartElf2K", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndElf2K", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndElf2K", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartVip", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartVip", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndVip", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndVip", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartVipII", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartVipII", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndVipII", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndVipII", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextExecVipII", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextExecVipII", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartCosmicos", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartCosmicos", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndCosmicos", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndCosmicos", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartElf", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartElf", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndElf", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndElf", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextExecElf", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextExecElf", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartElfII", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartElfII", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndElfII", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndElfII", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextExecElfII", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextExecElfII", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartSuperElf", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartSuperElf", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndSuperElf", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndSuperElf", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextExecSuperElf", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextExecSuperElf", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartMembership", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartMembership", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndMembership", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndMembership", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartTMC600", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartTMC600", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndTMC600", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndTMC600", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextExecTMC600", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextExecTMC600", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartTMC1800", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartTMC1800", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndTMC1800", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndTMC1800", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartTMC2000", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartTMC2000", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndTMC2000", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndTMC2000", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartNano", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartNano", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndNano", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndNano", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartPecom", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartPecom", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndPecom", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndPecom", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextExecPecom", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextExecPecom", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartEti", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartEti", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndEti", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndEti", wxStaticText)->Enable(false);
}

void Main::eventDisableControls()
{
    guiEvent event(GUI_MSG, DISABLE_CONTROLS);
    event.SetEventObject( p_Main );

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setUpdateTitle(guiEvent& WXUNUSED(event))
{
    updateTitle();
}

void Main::eventUpdateTitle()
{
    guiEvent event(GUI_MSG, UPDATE_TITLE);
    event.SetEventObject( p_Main );

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setPauseStateEvent(guiEvent& WXUNUSED(event))
{
   setPauseState();
}

void Main::eventPauseState()
{
    guiEvent event(GUI_MSG, PAUSE_STATE);
    event.SetEventObject( p_Main );

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setUpdateComxLedStatus(guiEvent& event)
{
   int card = event.GetByteValue1();
   int i = event.GetByteValue2();
   bool status = event.GetBoolValue();
   
   switch (runningComputer_)
   {
      case COMX:
         p_Comx->updateLedStatus(card, i, status);
      break;

      case XML:
         p_Video[elfConfiguration[XML].ioConfiguration.v1870VideoNumber]->updateLedStatus(card, i, status);
      break;
   }
}

void Main::eventUpdateComxLedStatus(int card, int i, bool status)
{
   guiEvent event(GUI_MSG, SET_COMXLED);
   event.SetEventObject( p_Main );

   event.SetBoolValue(status);
   event.SetByteValue1(card);
   event.SetByteValue2(i);

   GetEventHandler()->AddPendingEvent(event);
}

void Main::setUpdateDiagLedStatus(guiEvent& event)
{
   int i = event.GetByteValue2();
   bool status = event.GetBoolValue();
   
   switch (runningComputer_)
   {
      case COMX:
         p_Comx->updateDiagLedStatus(i, status);
      break;

      case XML:
         p_Video[elfConfiguration[XML].ioConfiguration.v1870VideoNumber]->updateDiagLedStatus(i, status);
      break;
   }
}

void Main::eventUpdateDiagLedStatus(int i, bool status)
{
   guiEvent event(GUI_MSG, SET_DIAGLED);
   event.SetEventObject( p_Main );

   event.SetBoolValue(status);
   event.SetByteValue2(i);

   GetEventHandler()->AddPendingEvent(event);
}

void Main::debounceTimeout(wxTimerEvent&WXUNUSED(event))
{
    p_Computer->keyClear();
}

void Main::setDebounceTimer(guiEvent&WXUNUSED(event))
{
    keyDebounceTimeoutPointer->Start(80, wxTIMER_ONE_SHOT);
}

void Main::eventDebounceTimer()
{
    guiEvent event(GUI_MSG, DEBOUNCE_TIMER);
    event.SetEventObject(p_Main);
    
    GetEventHandler()->AddPendingEvent(event);
}

void Main::guiSizeTimeout(wxTimerEvent&WXUNUSED(event))
{
    adjustGuiSize();
    guiSizeTimerStarted_ = false;
}

void Main::guiRedrawBarTimeOut(wxTimerEvent&WXUNUSED(event))
{
    p_Video[VIDEOMAIN]->reDrawBar();
}

wxString Main::getMultiCartGame(Byte findMsb, Byte findLsb)
{
    wxTextFile multiCartDefinitionFile;
    wxString returnName = "";
    
    if (multiCartDefinitionFile.Open(dataDir_ + "multi-cart.txt"))
    {
        wxString line, game;
        wxString msbStr, lsbStr;
        long msb, lsb;

        for (line = multiCartDefinitionFile.GetFirstLine(); !multiCartDefinitionFile.Eof(); line = multiCartDefinitionFile.GetNextLine())
        {
            game = line.AfterLast(',');
            line = line.MakeLower();
            line.Replace(" ", "");
            msbStr = line.BeforeFirst(',');
            line = line.AfterFirst(',');
            lsbStr = line.BeforeFirst(',');
            line = line.AfterFirst(',');
            msbStr.ToLong(&msb);
            lsbStr.ToLong(&lsb);
            if (findMsb == (Byte)msb && findLsb == (Byte)lsb)
            {
                returnName = game;
                break;
            }
        }
    }
    return returnName;
}

bool Main::loadKeyDefinition(wxString findFile1, wxString findFile2, int keyDefA1[], int keyDefB1[], int keyDefA2[], bool * simDefA2, int keyDefB2[], bool * simDefB2, int * inButton1, int * inButton2, int gameKeysA[], int gameKeysB[], wxString keyFileName)
{
    wxTextFile keyDefinitionFile;
    wxString gameName, nextValue;
    
#if defined(__linux__) || defined(__WXMAC__)
    wxString appName = "emma_02";
#else
    wxString appName = "Emma 02";
#endif

    wxConfigBase *keyConfigPointer;

    wxFileConfig *pConfig = new wxFileConfig(appName, "Marcel van Tongeren", dataDir_ + keyboardType_ + ".ini");
    wxConfigBase *currentConfigPointer = wxConfigBase::Set(pConfig);
    keyConfigPointer = wxConfigBase::Get();

    findFile1 = findFile1.MakeLower();
    findFile1.Replace(" ","");
    findFile2 = findFile2.MakeLower();
    findFile2.Replace(" ","");
 
    if (keyDefinitionFile.Open(dataDir_ + keyFileName))
    {
        for (gameName=keyDefinitionFile.GetFirstLine(); !keyDefinitionFile.Eof(); gameName=keyDefinitionFile.GetNextLine())
        {
            gameName = gameName.MakeLower();
            gameName.Replace(" ","");
            if ( ((gameName == "name:"+findFile1) && ( findFile1 != "")) || ((gameName == "name:"+findFile2) && (findFile2 != "")) || ((gameName == "elfosname:"+findFile1) && ( findFile1 != "")) || ((gameName == "elfosname:"+findFile2) && (findFile2 != "")))
            {
                if (keyDefinitionFile.Eof())
                {
                    delete pConfig;
                    wxConfigBase::Set(currentConfigPointer);
                    return false;
                }
                 wxString keyValues = keyDefinitionFile.GetNextLine();

                keyValues = keyValues.MakeLower();
                keyValues.Replace(" ","");

                bool padA1nnp = false;
                bool padB1nnp = false;
                bool padA2nnp = false;
                bool padB2nnp = false;

                while (keyValues.Left(5) != "name:" && !keyDefinitionFile.Eof())
                {
                    if (keyValues.BeforeFirst(':') == "load")
                    {
                        p_Main->loadKeyDefinition("", keyValues.AfterFirst(':'), keyDefA1, keyDefB1, keyDefA2, simDefA2, keyDefB2, simDefB2, inButton1, inButton2, gameKeysA, gameKeysB, keyFileName);
                    }
                
                    if (keyValues == "sima2")
                        *simDefA2 = true;
                    
                    if (keyValues == "simb2")
                        *simDefB2 = true;
                    
                    if (keyValues == "!sima2")
                        *simDefA2 = false;
                    
                    if (keyValues == "!simb2")
                        *simDefB2 = false;
                    
                    if ((keyValues.BeforeFirst(':') == "pada1" && !padA1nnp) || (keyValues.BeforeFirst(':') == "pada1nnp" && !useNumPad()))
                    {
                        if (keyValues.BeforeFirst(':') == "pada1nnp" && !useNumPad())
                            padA1nnp = true;
                        
                        keyValues = keyValues.AfterFirst(':');
                        
                        nextValue = keyValues.BeforeFirst(',');
                        long value;
                        int i=0;
                        while (nextValue.Len()>0 && i < 16)
                        {
                            if (nextValue.ToLong(&value, 10))
                                keyDefA1[i] = (int)value;
                            else
                                keyDefA1[i] = (int)keyConfigPointer->Read(nextValue, 0l);
                            i++;
                            keyValues = keyValues.AfterFirst(',');
                            nextValue = keyValues.BeforeFirst(',');
                        }
                    }
                    
                    if ((keyValues.BeforeFirst(':') == "padb1" && !padB1nnp) || (keyValues.BeforeFirst(':') == "padb1nnp" && !useNumPad()))
                    {
                        if (keyValues.BeforeFirst(':') == "padb1nnp" && !useNumPad())
                            padB1nnp = true;

                        keyValues = keyValues.AfterFirst(':');
                        
                        nextValue = keyValues.BeforeFirst(',');
                        long value;
                        int i=0;
                        while (nextValue.Len()>0 && i < 16)
                        {
                            if (nextValue.ToLong(&value, 10))
                                keyDefB1[i] = (int)value;
                            else
                                keyDefB1[i] = (int)keyConfigPointer->Read(nextValue, 0l);
                            i++;
                            keyValues = keyValues.AfterFirst(',');
                            nextValue = keyValues.BeforeFirst(',');
                        }
                    }

                    if ((keyValues.BeforeFirst(':') == "pada2" && !padA2nnp) || (keyValues.BeforeFirst(':') == "pada2nnp" && !useNumPad()))
                    {
                        if (keyValues.BeforeFirst(':') == "pada2nnp" && !useNumPad())
                            padA2nnp = true;

                        keyValues = keyValues.AfterFirst(':');
                        
                        nextValue = keyValues.BeforeFirst(',');
                        long value;
                        int i=0;
                        while (nextValue.Len()>0 && i < 16)
                        {
                            if (nextValue.ToLong(&value, 10))
                                keyDefA2[i] = (int)value;
                            else
                                keyDefA2[i] = (int)keyConfigPointer->Read(nextValue, 0l);
                            i++;
                            keyValues = keyValues.AfterFirst(',');
                            nextValue = keyValues.BeforeFirst(',');
                        }
                    }
                    
                    if ((keyValues.BeforeFirst(':') == "padb2" && !padB2nnp) || (keyValues.BeforeFirst(':') == "padb2nnp" && !useNumPad()))
                    {
                        if (keyValues.BeforeFirst(':') == "padb2nnp" && !useNumPad())
                            padB2nnp = true;

                        keyValues = keyValues.AfterFirst(':');
                        
                        nextValue = keyValues.BeforeFirst(',');
                        long value;
                        int i=0;
                        while (nextValue.Len()>0 && i < 16)
                        {
                            if (nextValue.ToLong(&value, 10))
                                keyDefB2[i] = (int)value;
                            else
                                keyDefB2[i] = (int)keyConfigPointer->Read(nextValue, 0l);
                            i++;
                            keyValues = keyValues.AfterFirst(',');
                            nextValue = keyValues.BeforeFirst(',');
                        }
                    }
                    
                    if (keyValues.BeforeFirst(':') == "in1")
                    {
                        keyValues = keyValues.AfterFirst(':');
                        
                        long value;
                        if (keyValues.ToLong(&value, 10))
                            *inButton1 = (int) value;
                        else
                            *inButton1 = (int)keyConfigPointer->Read(nextValue, 0l);
                    }
                    
                    if (keyValues.BeforeFirst(':') == "in2")
                    {
                        keyValues = keyValues.AfterFirst(':');
                        
                        long value;
                        if (keyValues.ToLong(&value, 10))
                            *inButton2 = (int) value;
                        else
                            *inButton2 = (int)keyConfigPointer->Read(nextValue, 0l);
                    }

                    if (keyValues.BeforeFirst(':') == "gamekeys")
                    {
                        keyValues = keyValues.AfterFirst(':');
                        
                        nextValue = keyValues.BeforeFirst(',');
                        long value;
                        int i=0;
                        while (nextValue.ToLong(&value, 16) && i<10)
                        {
                            if (i < 5)
                                gameKeysA[i] = (int)value;
                            else
                                gameKeysB[i-5] = (int)value;
                            
                            i++;
                            keyValues = keyValues.AfterFirst(',');
                            nextValue = keyValues.BeforeFirst(',');
                        }
                    }
                    
                    keyValues=keyDefinitionFile.GetNextLine();
                    keyValues = keyValues.MakeLower();
                    keyValues.Replace(" ","");
                }
                delete pConfig;
                wxConfigBase::Set(currentConfigPointer);
                return true;
            }
        }
    }
    delete pConfig;
    wxConfigBase::Set(currentConfigPointer);
    return false;
}

int Main::getDefaultInKey1(wxString computerStr)
{
#ifdef __WXMAC__
    return (int)configPointer->Read(computerStr+"/InButton1", WXK_RAW_CONTROL);
#else
    return (int)configPointer->Read(computerStr+"/InButton1", WXK_INSERT);
#endif
}

int Main::getDefaultInKey2(wxString computerStr)
{
    return (int)configPointer->Read(computerStr+"/InButton2", 0l);
}

int Main::getDefaultInKey2(wxString computerStr, int defaultKey)
{
    return (int)configPointer->Read(computerStr+"/InButton2", (long)defaultKey);
}

void Main::getDefaultHexKeys(int computerType, wxString computerStr, wxString player, int keysHex1[], int keysHex2[], int keyDefGameHexA_[])
{
    int keyDefA1_[16];
    int keyDefB1_[16];
    int keyDefA2_[16];
    int keyDefB2_[16];
    int inKey1_, inKey2_;
    
    bool simDefA2_;
    bool simDefB2_;
    
//    int keyDefGameHexA_[5];
    int keyDefGameHexB_[5];
    
    wxString keyStr;
    bool keysFound;

    switch (computerType)
    {
        case STUDIO:
        case COINARCADE:
        case VICTORY:
            keysFound = loadKeyDefinition("", "studiodefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
        break;
            
        case VISICOM:
            keysFound = loadKeyDefinition("", "studiodefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
        break;
            
        case VIP:
        case VIP2K:
            if (p_Main->getVipVp590() || p_Main->getVipVp580())
                keysFound = loadKeyDefinition("", "vipdefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
            else
                keysFound = loadKeyDefinition("", "vipiidefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
        break;

        case STUDIOIV:
            keysFound = loadKeyDefinition("", "studioivdefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
        break;

        case ELF:
        case ELFII:
        case SUPERELF:
        case ELF2K:
        case COSMICOS:
        case XML:
        case PICO:
            keysFound = loadKeyDefinition("", "elfdefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
        break;

        case UC1800:
            keysFound = loadKeyDefinition("", "uc1800default", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
        break;

        case FRED1:
        case FRED1_5:
            keysFound = loadKeyDefinition("", "freddefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
        break;

        case VIPII:
        case VELF:
            keysFound = loadKeyDefinition("", "vipiidefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
        break;

        default:
            keysFound = loadKeyDefinition("", "vipiidefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
        break;
    }
    
    if (keysFound)
    {
        switch (computerType)
        {
            case STUDIO:
            case COINARCADE:
            case VICTORY:
            case VISICOM:
                if (player == "A")
                {
                    for (int i=0; i<10; i++)
                    {
                        keyStr.Printf("/HexKeySet1"+player+"%01X",i);
                        keysHex1[i] = getConfigItem(computerStr+keyStr, keyDefA1_[i]);
                        keyStr.Printf("/HexKeySet2"+player+"%01X",i);
                        keysHex2[i] = getConfigItem(computerStr+keyStr, keyDefA2_[i]);
                    }
                }
                else
                {
                    for (int i=0; i<10; i++)
                    {
                        keyStr.Printf("/HexKeySet1"+player+"%01X",i);
                        keysHex1[i] = getConfigItem(computerStr+keyStr, keyDefB1_[i]);
                        keyStr.Printf("/HexKeySet2"+player+"%01X",i);
                        keysHex2[i] = getConfigItem(computerStr+keyStr, keyDefB2_[i]);
                    }
                }
            break;
            
            case FRED1:
            case FRED1_5:
            case VIP:
            case VIP2K:
            case ELF:
            case ELFII:
            case SUPERELF:
            case UC1800:
            case ELF2K:
            case ETI:
            case TMC1800:
            case TMC2000:
            case NANO:
            case COSMICOS:
            case VIPII:
            case VELF:
            case STUDIOIV:
            case XML:
            case PICO:
                if (player == "A")
                {
                    for (int i=0; i<16; i++)
                    {
                        keyStr.Printf("/HexKeySet1"+player+"%01X",i);
                        keysHex1[i] = getConfigItem(computerStr+keyStr, keyDefA1_[i]);
                        keyStr.Printf("/HexKeySet2"+player+"%01X",i);
                        keysHex2[i] = getConfigItem(computerStr+keyStr, keyDefA2_[i]);
                        
                    }
                }
                else
                {
                    for (int i=0; i<16; i++)
                    {
                        keyStr.Printf("/HexKeySet1"+player+"%01X",i);
                        keysHex1[i] = getConfigItem(computerStr+keyStr, keyDefB1_[i]);
                        keyStr.Printf("/HexKeySet2"+player+"%01X",i);
                        keysHex2[i] = getConfigItem(computerStr+keyStr, keyDefB2_[i]);
                    }
                }
            break;
        }
    }
    else
    {
        if (player == "A")
        {
            keysHex1[0] = getConfigItem(computerStr+"/HexKeySet1A0", 48);
            keysHex1[1] = getConfigItem(computerStr+"/HexKeySet1A1", 49);
            keysHex1[2] = getConfigItem(computerStr+"/HexKeySet1A2", 50);
            keysHex1[3] = getConfigItem(computerStr+"/HexKeySet1A3", 51);
            keysHex1[4] = getConfigItem(computerStr+"/HexKeySet1A4", 52);
            keysHex1[5] = getConfigItem(computerStr+"/HexKeySet1A5", 53);
            keysHex1[6] = getConfigItem(computerStr+"/HexKeySet1A6", 54);
            keysHex1[7] = getConfigItem(computerStr+"/HexKeySet1A7", 55);
            keysHex1[8] = getConfigItem(computerStr+"/HexKeySet1A8", 56);
            keysHex1[9] = getConfigItem(computerStr+"/HexKeySet1A9", 57);
            keysHex1[10] = getConfigItem(computerStr+"/HexKeySet1AA", 65);
            keysHex1[11] = getConfigItem(computerStr+"/HexKeySet1AB", 66);
            keysHex1[12] = getConfigItem(computerStr+"/HexKeySet1AC", 67);
            keysHex1[13] = getConfigItem(computerStr+"/HexKeySet1AD", 68);
            keysHex1[14] = getConfigItem(computerStr+"/HexKeySet1AE", 69);
            keysHex1[15] = getConfigItem(computerStr+"/HexKeySet1AF", 70);
            
            if (computerStr == "Vip" && (p_Main->getVipVp590() || p_Main->getVipVp580()))
            {
                keysHex2[0] = getConfigItem(computerStr+"/HexKeySet2A0", 32);
                keysHex2[1] = getConfigItem(computerStr+"/HexKeySet2A1", 0);
                keysHex2[2] = getConfigItem(computerStr+"/HexKeySet2A2", 315);
                keysHex2[3] = getConfigItem(computerStr+"/HexKeySet2A3", 0);
                keysHex2[4] = getConfigItem(computerStr+"/HexKeySet2A4", 314);
                keysHex2[5] = getConfigItem(computerStr+"/HexKeySet2A5", 0);
                keysHex2[6] = getConfigItem(computerStr+"/HexKeySet2A6", 316);
                keysHex2[7] = getConfigItem(computerStr+"/HexKeySet2A7", 0);
                keysHex2[8] = getConfigItem(computerStr+"/HexKeySet2A8", 317);
                keysHex2[9] = getConfigItem(computerStr+"/HexKeySet2A9", 0);
                keysHex2[10] = getConfigItem(computerStr+"/HexKeySet2AA", 0);
                keysHex2[11] = getConfigItem(computerStr+"/HexKeySet2AB", 0);
                keysHex2[12] = getConfigItem(computerStr+"/HexKeySet2AC", 0);
                keysHex2[13] = getConfigItem(computerStr+"/HexKeySet2AD", 0);
                keysHex2[14] = getConfigItem(computerStr+"/HexKeySet2AE", 0);
                keysHex2[15] = getConfigItem(computerStr+"/HexKeySet2AF", 0);
            }
            else
            {
                if (computerStr == "Elf" || computerStr == "ElfII" || computerStr == "SuperElf" || computerStr == "Elf2K"|| computerStr == "Cosmicos")
                {
                    if (useNumPad())
                    {
                        keysHex2[0] = getConfigItem(computerStr+"/HexKeySet2A0", 0);
                        keysHex2[1] = getConfigItem(computerStr+"/HexKeySet2A1", WXK_NUMPAD_ENTER);
                        keysHex2[2] = getConfigItem(computerStr+"/HexKeySet2A2", 0);
                        keysHex2[3] = getConfigItem(computerStr+"/HexKeySet2A3", 0);
                        keysHex2[4] = getConfigItem(computerStr+"/HexKeySet2A4", WXK_NUMPAD4);
                        keysHex2[5] = getConfigItem(computerStr+"/HexKeySet2A5", 317);
                        keysHex2[6] = getConfigItem(computerStr+"/HexKeySet2A6", WXK_NUMPAD6);
                        keysHex2[7] = getConfigItem(computerStr+"/HexKeySet2A7", 0);
                        keysHex2[8] = getConfigItem(computerStr+"/HexKeySet2A8", 314);
                        keysHex2[9] = getConfigItem(computerStr+"/HexKeySet2A9", 32);
                        keysHex2[10] = getConfigItem(computerStr+"/HexKeySet2AA", 316);
                        keysHex2[11] = getConfigItem(computerStr+"/HexKeySet2AB", WXK_NUMPAD2);
                        keysHex2[12] = getConfigItem(computerStr+"/HexKeySet2AC", 0);
                        keysHex2[13] = getConfigItem(computerStr+"/HexKeySet2AD", 315);
                        keysHex2[14] = getConfigItem(computerStr+"/HexKeySet2AE", 0);
                        keysHex2[15] = getConfigItem(computerStr+"/HexKeySet2AF", WXK_NUMPAD8);
                    }
                    else
                    {
                        keysHex2[0] = getConfigItem(computerStr+"/HexKeySet2A0", 0);
                        keysHex2[1] = getConfigItem(computerStr+"/HexKeySet2A1", WXK_TAB);
                        keysHex2[2] = getConfigItem(computerStr+"/HexKeySet2A2", 0);
                        keysHex2[3] = getConfigItem(computerStr+"/HexKeySet2A3", 0);
                        keysHex2[4] = getConfigItem(computerStr+"/HexKeySet2A4", 71);
                        keysHex2[5] = getConfigItem(computerStr+"/HexKeySet2A5", 317);
                        keysHex2[6] = getConfigItem(computerStr+"/HexKeySet2A6", 72);
                        keysHex2[7] = getConfigItem(computerStr+"/HexKeySet2A7", 0);
                        keysHex2[8] = getConfigItem(computerStr+"/HexKeySet2A8", 314);
                        keysHex2[9] = getConfigItem(computerStr+"/HexKeySet2A9", 32);
                        keysHex2[10] = getConfigItem(computerStr+"/HexKeySet2AA", 316);
                        keysHex2[11] = getConfigItem(computerStr+"/HexKeySet2AB", 78);
                        keysHex2[12] = getConfigItem(computerStr+"/HexKeySet2AC", 0);
                        keysHex2[13] = getConfigItem(computerStr+"/HexKeySet2AD", 315);
                        keysHex2[14] = getConfigItem(computerStr+"/HexKeySet2AE", 0);
                        keysHex2[15] = getConfigItem(computerStr+"/HexKeySet2AF", 89);
                    }
                    
                }
                else
                {
                    if (useNumPad())
                    {
                        keysHex2[0] = getConfigItem(computerStr+"/HexKeySet2A0", WXK_NUMPAD_ENTER);
                        keysHex2[1] = getConfigItem(computerStr+"/HexKeySet2A1", 0);
                        keysHex2[2] = getConfigItem(computerStr+"/HexKeySet2A2", 315);
                        keysHex2[3] = getConfigItem(computerStr+"/HexKeySet2A3", 0);
                        keysHex2[4] = getConfigItem(computerStr+"/HexKeySet2A4", 314);
                        keysHex2[5] = getConfigItem(computerStr+"/HexKeySet2A5", 32);
                        keysHex2[6] = getConfigItem(computerStr+"/HexKeySet2A6", 316);
                        keysHex2[7] = getConfigItem(computerStr+"/HexKeySet2A7", WXK_NUMPAD4);
                        keysHex2[8] = getConfigItem(computerStr+"/HexKeySet2A8", 317);
                        keysHex2[9] = getConfigItem(computerStr+"/HexKeySet2A9", WXK_NUMPAD6);
                        keysHex2[10] = getConfigItem(computerStr+"/HexKeySet2AA", 0);
                        keysHex2[11] = getConfigItem(computerStr+"/HexKeySet2AB", 0);
                        keysHex2[12] = getConfigItem(computerStr+"/HexKeySet2AC", WXK_NUMPAD8);
                        keysHex2[13] = getConfigItem(computerStr+"/HexKeySet2AD", WXK_NUMPAD2);
                        keysHex2[14] = getConfigItem(computerStr+"/HexKeySet2AE", 0);
                        keysHex2[15] = getConfigItem(computerStr+"/HexKeySet2AF", 0);
                    }
                    else
                    {
                        keysHex2[0] = getConfigItem(computerStr+"/HexKeySet2A0", WXK_TAB);
                        keysHex2[1] = getConfigItem(computerStr+"/HexKeySet2A1", 0);
                        keysHex2[2] = getConfigItem(computerStr+"/HexKeySet2A2", 315);
                        keysHex2[3] = getConfigItem(computerStr+"/HexKeySet2A3", 0);
                        keysHex2[4] = getConfigItem(computerStr+"/HexKeySet2A4", 314);
                        keysHex2[5] = getConfigItem(computerStr+"/HexKeySet2A5", 32);
                        keysHex2[6] = getConfigItem(computerStr+"/HexKeySet2A6", 316);
                        keysHex2[7] = getConfigItem(computerStr+"/HexKeySet2A7", 71);
                        keysHex2[8] = getConfigItem(computerStr+"/HexKeySet2A8", 317);
                        keysHex2[9] = getConfigItem(computerStr+"/HexKeySet2A9", 72);
                        keysHex2[10] = getConfigItem(computerStr+"/HexKeySet2AA", 0);
                        keysHex2[11] = getConfigItem(computerStr+"/HexKeySet2AB", 0);
                        keysHex2[12] = getConfigItem(computerStr+"/HexKeySet2AC", 89);
                        keysHex2[13] = getConfigItem(computerStr+"/HexKeySet2AD", 78);
                        keysHex2[14] = getConfigItem(computerStr+"/HexKeySet2AE", 0);
                        keysHex2[15] = getConfigItem(computerStr+"/HexKeySet2AF", 0);
                    }
                    
                }
                
            }
        }
        else
        {
            keysHex1[0] = getConfigItem(computerStr+"/HexKeySet1B0", WXK_NUMPAD0);
            keysHex1[2] = getConfigItem(computerStr+"/HexKeySet1B2", WXK_NUMPAD2);
            keysHex1[1] = getConfigItem(computerStr+"/HexKeySet1B1", WXK_NUMPAD1);
            keysHex1[3] = getConfigItem(computerStr+"/HexKeySet1B3", WXK_NUMPAD3);
            keysHex1[4] = getConfigItem(computerStr+"/HexKeySet1B4", WXK_NUMPAD4);
            keysHex1[5] = getConfigItem(computerStr+"/HexKeySet1B5", WXK_NUMPAD5);
            keysHex1[6] = getConfigItem(computerStr+"/HexKeySet1B6", WXK_NUMPAD6);
            keysHex1[7] = getConfigItem(computerStr+"/HexKeySet1B7", WXK_NUMPAD7);
            keysHex1[8] = getConfigItem(computerStr+"/HexKeySet1B8", WXK_NUMPAD8);
            keysHex1[9] = getConfigItem(computerStr+"/HexKeySet1B9", WXK_NUMPAD9);
            keysHex1[10] = getConfigItem(computerStr+"/HexKeySet1BA", WXK_NUMPAD_DIVIDE);
            keysHex1[11] = getConfigItem(computerStr+"/HexKeySet1BB", WXK_NUMPAD_MULTIPLY);
            keysHex1[12] = getConfigItem(computerStr+"/HexKeySet1BC", WXK_NUMPAD_SUBTRACT);
            keysHex1[13] = getConfigItem(computerStr+"/HexKeySet1BD", WXK_NUMPAD_ADD);
            keysHex1[14] = getConfigItem(computerStr+"/HexKeySet1BE", WXK_NUMPAD_ENTER);
            keysHex1[15] = getConfigItem(computerStr+"/HexKeySet1BF", WXK_NUMPAD_DECIMAL);
            
            keysHex2[0] = getConfigItem(computerStr+"/HexKeySet2B0", WXK_TAB);
            keysHex2[1] = getConfigItem(computerStr+"/HexKeySet2B1", 0);
            keysHex2[2] = getConfigItem(computerStr+"/HexKeySet2B2", 89);
            keysHex2[3] = getConfigItem(computerStr+"/HexKeySet2B3", 0);
            keysHex2[4] = getConfigItem(computerStr+"/HexKeySet2B4", 71);
            keysHex2[5] = getConfigItem(computerStr+"/HexKeySet2B5", 0);
            keysHex2[6] = getConfigItem(computerStr+"/HexKeySet2B6", 72);
            keysHex2[7] = getConfigItem(computerStr+"/HexKeySet2B7", 0);
            keysHex2[8] = getConfigItem(computerStr+"/HexKeySet2B8", 78);
            keysHex2[9] = getConfigItem(computerStr+"/HexKeySet2B9", 0);
            keysHex2[10] = getConfigItem(computerStr+"/HexKeySet2BA", 0);
            keysHex2[11] = getConfigItem(computerStr+"/HexKeySet2BB", 0);
            keysHex2[12] = getConfigItem(computerStr+"/HexKeySet2BC", 0);
            keysHex2[13] = getConfigItem(computerStr+"/HexKeySet2BD", 0);
            keysHex2[14] = getConfigItem(computerStr+"/HexKeySet2BE", 0);
            keysHex2[15] = getConfigItem(computerStr+"/HexKeySet2BF", 0);
        }
    }
}

void Main::getTmc600Keys(int keysNormal[], int keysShift[])
{
    getDefaultTmc600Keys(keysNormal, keysShift);

    keysNormal[0] = p_Main->getConfigItem("/TMC600/Key00", keysNormal[0]);
    keysNormal[1] = p_Main->getConfigItem("/TMC600/Key01", keysNormal[1]);
    keysNormal[2] = p_Main->getConfigItem("/TMC600/Key02", keysNormal[2]);
    keysNormal[3] = p_Main->getConfigItem("/TMC600/Key03", keysNormal[3]);
    keysNormal[4] = p_Main->getConfigItem("/TMC600/Key04", keysNormal[4]);
    keysNormal[5] = p_Main->getConfigItem("/TMC600/Key05", keysNormal[5]);
    keysNormal[6] = p_Main->getConfigItem("/TMC600/Key06", keysNormal[6]);
    keysNormal[7] = p_Main->getConfigItem("/TMC600/Key07", keysNormal[7]);
    keysNormal[8] = p_Main->getConfigItem("/TMC600/Key08", keysNormal[8]);
    keysNormal[9] = p_Main->getConfigItem("/TMC600/Key09", keysNormal[9]);
    keysNormal[10] = p_Main->getConfigItem("/TMC600/Key0A", keysNormal[10]);
    keysNormal[11] = p_Main->getConfigItem("/TMC600/Key0B", keysNormal[11]);
    keysNormal[12] = p_Main->getConfigItem("/TMC600/Key0C", keysNormal[12]);
    keysNormal[13] = p_Main->getConfigItem("/TMC600/Key0D", keysNormal[13]);
    keysNormal[14] = p_Main->getConfigItem("/TMC600/Key0E", keysNormal[14]);
    keysNormal[15] = p_Main->getConfigItem("/TMC600/Key0F", keysNormal[15]);
    keysNormal[16] = p_Main->getConfigItem("/TMC600/Key10", keysNormal[16]);
    keysNormal[20] = p_Main->getConfigItem("/TMC600/Key2E", keysNormal[20]);

    keysShift[0] = p_Main->getConfigItem("/TMC600/KeyShift00", keysShift[0]);
    keysShift[1] = p_Main->getConfigItem("/TMC600/KeyShift01", keysShift[1]);
    keysShift[2] = p_Main->getConfigItem("/TMC600/KeyShift02", keysShift[2]);
    keysShift[3] = p_Main->getConfigItem("/TMC600/KeyShift03", keysShift[3]);
    keysShift[4] = p_Main->getConfigItem("/TMC600/KeyShift04", keysShift[4]);
    keysShift[5] = p_Main->getConfigItem("/TMC600/KeyShift05", keysShift[5]);
    keysShift[6] = p_Main->getConfigItem("/TMC600/KeyShift06", keysShift[6]);
    keysShift[7] = p_Main->getConfigItem("/TMC600/KeyShift07", keysShift[7]);
    keysShift[8] = p_Main->getConfigItem("/TMC600/KeyShift08", keysShift[8]);
    keysShift[9] = p_Main->getConfigItem("/TMC600/KeyShift09", keysShift[9]);
    keysShift[10] = p_Main->getConfigItem("/TMC600/KeyShift0A", keysShift[10]);
    keysShift[11] = p_Main->getConfigItem("/TMC600/KeyShift0B", keysShift[11]);
    keysShift[12] = p_Main->getConfigItem("/TMC600/KeyShift0C", keysShift[12]);
    keysShift[13] = p_Main->getConfigItem("/TMC600/KeyShift0D", keysShift[13]);
    keysShift[14] = p_Main->getConfigItem("/TMC600/KeyShift0E", keysShift[14]);
    keysShift[15] = p_Main->getConfigItem("/TMC600/KeyShift0F", keysShift[15]);
    keysShift[16] = p_Main->getConfigItem("/TMC600/KeyShift10", keysShift[16]);
    keysShift[17] = p_Main->getConfigItem("/TMC600/KeyShift2B", keysShift[17]);
    keysShift[18] = p_Main->getConfigItem("/TMC600/KeyShift2C", keysShift[18]);
    keysShift[19] = p_Main->getConfigItem("/TMC600/KeyShift2D", keysShift[19]);
    keysShift[20] = p_Main->getConfigItem("/TMC600/KeyShift2E", keysShift[20]);
}

void Main::getDefaultTmc600Keys(int keysNormal[], int keysShift[])
{
    for (int i = 0; i<21; i++)
    {
        keysNormal[i] = 0;
        keysShift[i] = 0;
    }

#if defined(__linux__) || defined(__WXMAC__)
    wxString appName = "emma_02";
#else
    wxString appName = "Emma 02";
#endif

    wxConfigBase *keyConfigPointer;

    wxFileConfig *pConfig = new wxFileConfig(appName, "Marcel van Tongeren", dataDir_ + keyboardType_ + ".ini");
    wxConfigBase *currentConfigPointer = wxConfigBase::Set(pConfig);
    keyConfigPointer = wxConfigBase::Get();

    keysNormal[0] = (int)keyConfigPointer->Read("/TMC600/Key00", 48);
    keysNormal[1] = (int)keyConfigPointer->Read("/TMC600/Key01", 49);
    keysNormal[2] = (int)keyConfigPointer->Read("/TMC600/Key02", 50);
    keysNormal[3] = (int)keyConfigPointer->Read("/TMC600/Key03", 51);
    keysNormal[4] = (int)keyConfigPointer->Read("/TMC600/Key04", 52);
    keysNormal[5] = (int)keyConfigPointer->Read("/TMC600/Key05", 53);
    keysNormal[6] = (int)keyConfigPointer->Read("/TMC600/Key06", 54);
    keysNormal[7] = (int)keyConfigPointer->Read("/TMC600/Key07", 55);
    keysNormal[8] = (int)keyConfigPointer->Read("/TMC600/Key08", 56);
    keysNormal[9] = (int)keyConfigPointer->Read("/TMC600/Key09", 57);
    keysNormal[10] = (int)keyConfigPointer->Read("/TMC600/Key0A", 39);
    keysNormal[11] = (int)keyConfigPointer->Read("/TMC600/Key0B", 59);
    keysNormal[12] = (int)keyConfigPointer->Read("/TMC600/Key0C", 44);
    keysNormal[13] = (int)keyConfigPointer->Read("/TMC600/Key0D", 45);
    keysNormal[14] = (int)keyConfigPointer->Read("/TMC600/Key0E", 46);
    keysNormal[15] = (int)keyConfigPointer->Read("/TMC600/Key0F", 47);
    keysNormal[16] = (int)keyConfigPointer->Read("/TMC600/Key10", 0l);
    keysNormal[20] = (int)keyConfigPointer->Read("/TMC600/Key2E", 0l);

    keysShift[0] = (int)keyConfigPointer->Read("/TMC600/KeyShift00", 95);
    keysShift[1] = (int)keyConfigPointer->Read("/TMC600/KeyShift01", 33);
    keysShift[2] = (int)keyConfigPointer->Read("/TMC600/KeyShift02", 34);
    keysShift[3] = (int)keyConfigPointer->Read("/TMC600/KeyShift03", 35);
    keysShift[4] = (int)keyConfigPointer->Read("/TMC600/KeyShift04", 36);
    keysShift[5] = (int)keyConfigPointer->Read("/TMC600/KeyShift05", 37);
    keysShift[6] = (int)keyConfigPointer->Read("/TMC600/KeyShift06", 38);
    keysShift[7] = (int)keyConfigPointer->Read("/TMC600/KeyShift07", 126);
    keysShift[8] = (int)keyConfigPointer->Read("/TMC600/KeyShift08", 40);
    keysShift[9] = (int)keyConfigPointer->Read("/TMC600/KeyShift09", 41);
    keysShift[10] = (int)keyConfigPointer->Read("/TMC600/KeyShift0A", 42);
    keysShift[11] = (int)keyConfigPointer->Read("/TMC600/KeyShift0B", 43);
    keysShift[12] = (int)keyConfigPointer->Read("/TMC600/KeyShift0C", 60);
    keysShift[13] = (int)keyConfigPointer->Read("/TMC600/KeyShift0D", 58);
    keysShift[14] = (int)keyConfigPointer->Read("/TMC600/KeyShift0E", 62);
    keysShift[15] = (int)keyConfigPointer->Read("/TMC600/KeyShift0F", 63);
    keysShift[16] = (int)keyConfigPointer->Read("/TMC600/KeyShift10", 64);
    keysShift[17] = (int)keyConfigPointer->Read("/TMC600/KeyShift2B", 123);
    keysShift[18] = (int)keyConfigPointer->Read("/TMC600/KeyShift2C", 125);
    keysShift[19] = (int)keyConfigPointer->Read("/TMC600/KeyShift2D", 124);
    keysShift[20] = (int)keyConfigPointer->Read("/TMC600/KeyShift2E", 94);

    delete pConfig;
    wxConfigBase::Set(currentConfigPointer);
}

void Main::storeDefaultTmc600Keys(int keysNormal[], int keysShift[])
{
    configPointer->Write("/TMC600/Key00", keysNormal[0]);
    configPointer->Write("/TMC600/Key01", keysNormal[1]);
    configPointer->Write("/TMC600/Key02", keysNormal[2]);
    configPointer->Write("/TMC600/Key03", keysNormal[3]);
    configPointer->Write("/TMC600/Key04", keysNormal[4]);
    configPointer->Write("/TMC600/Key05", keysNormal[5]);
    configPointer->Write("/TMC600/Key06", keysNormal[6]);
    configPointer->Write("/TMC600/Key07", keysNormal[7]);
    configPointer->Write("/TMC600/Key08", keysNormal[8]);
    configPointer->Write("/TMC600/Key09", keysNormal[9]);
    configPointer->Write("/TMC600/Key0A", keysNormal[10]);
    configPointer->Write("/TMC600/Key0B", keysNormal[11]);
    configPointer->Write("/TMC600/Key0C", keysNormal[12]);
    configPointer->Write("/TMC600/Key0D", keysNormal[13]);
    configPointer->Write("/TMC600/Key0E", keysNormal[14]);
    configPointer->Write("/TMC600/Key0F", keysNormal[15]);
    configPointer->Write("/TMC600/Key10", keysNormal[16]);
    configPointer->Write("/TMC600/Key2E", keysNormal[20]);
    
    configPointer->Write("/TMC600/KeyShift00", keysShift[0]);
    configPointer->Write("/TMC600/KeyShift01", keysShift[1]);
    configPointer->Write("/TMC600/KeyShift02", keysShift[2]);
    configPointer->Write("/TMC600/KeyShift03", keysShift[3]);
    configPointer->Write("/TMC600/KeyShift04", keysShift[4]);
    configPointer->Write("/TMC600/KeyShift05", keysShift[5]);
    configPointer->Write("/TMC600/KeyShift06", keysShift[6]);
    configPointer->Write("/TMC600/KeyShift07", keysShift[7]);
    configPointer->Write("/TMC600/KeyShift08", keysShift[8]);
    configPointer->Write("/TMC600/KeyShift09", keysShift[9]);
    configPointer->Write("/TMC600/KeyShift0A", keysShift[10]);
    configPointer->Write("/TMC600/KeyShift0B", keysShift[11]);
    configPointer->Write("/TMC600/KeyShift0C", keysShift[12]);
    configPointer->Write("/TMC600/KeyShift0D", keysShift[13]);
    configPointer->Write("/TMC600/KeyShift0E", keysShift[14]);
    configPointer->Write("/TMC600/KeyShift0F", keysShift[15]);
    configPointer->Write("/TMC600/KeyShift10", keysShift[16]);
    configPointer->Write("/TMC600/KeyShift2B", keysShift[17]);
    configPointer->Write("/TMC600/KeyShift2C", keysShift[18]);
    configPointer->Write("/TMC600/KeyShift2D", keysShift[19]);
    configPointer->Write("/TMC600/KeyShift2E", keysShift[20]);
}

int Main::getDefaultCoinArcadeKeys(int keysA[], int keysB[])
{
    keysA[0] = (int)configPointer->Read("/CoinArcade/A-Up", 315);
    keysA[1] = (int)configPointer->Read("/CoinArcade/A-Left", 314);
    keysA[2] = (int)configPointer->Read("/CoinArcade/A-Right", 316);
    keysA[3] = (int)configPointer->Read("/CoinArcade/A-Down", 317);
    keysA[4] = (int)configPointer->Read("/CoinArcade/A-Fire", 32);
    
    keysB[0] = (int)configPointer->Read("/CoinArcade/B-Up", 87);
    keysB[1] = (int)configPointer->Read("/CoinArcade/B-Left", 65);
    keysB[2] = (int)configPointer->Read("/CoinArcade/B-Right", 83);
    keysB[3] = (int)configPointer->Read("/CoinArcade/B-Down", 90);
    keysB[4] = (int)configPointer->Read("/CoinArcade/B-Fire", 9);
    
    int coin = (int)configPointer->Read("/CoinArcade/Coin", 67);
    
    return coin;
}

void Main::storeDefaultCoinArcadeKeys(int keysA[], int keysB[], int coin)
{
    configPointer->Write("/CoinArcade/A-Up", keysA[0]);
    configPointer->Write("/CoinArcade/A-Left", keysA[1]);
    configPointer->Write("/CoinArcade/A-Right", keysA[2]);
    configPointer->Write("/CoinArcade/A-Down", keysA[3]);
    configPointer->Write("/CoinArcade/A-Fire", keysA[4]);
    
    configPointer->Write("/CoinArcade/B-Up", keysB[0]);
    configPointer->Write("/CoinArcade/B-Left", keysB[1]);
    configPointer->Write("/CoinArcade/B-Right", keysB[2]);
    configPointer->Write("/CoinArcade/B-Down", keysB[3]);
    configPointer->Write("/CoinArcade/B-Fire", keysB[4]);

    configPointer->Write("/CoinArcade/Coin", coin);
}

wxThread::ExitCode UpdateCheckThread::Entry()
{
    if (p_Main->checkUpdateEmma())
    {
        if (p_Main->updateEmma())
            p_Main->Destroy();
    }

    return (wxThread::ExitCode)0;     // success
}

UpdateCheckThread::~UpdateCheckThread()
{
    wxCriticalSectionLocker enter(m_pHandler->m_pUpdateCheckThreadCS);
    // the thread is being destroyed; make sure not to leave dangling pointers around
    m_pHandler->m_pUpdateCheckThread = NULL;
}


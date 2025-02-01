
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
#include "psave.h"
#include "functionkey.h"
#include "datadir.h"
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
    if (max < 9)
       max_ = max;
    else
       max_ = 9;
}

void SlotEdit::changeNumber(int number)
{
    wxString numberString;

    if (number == number_) return;

    numberString.Printf("%d", number);
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
    EVT_MENU(XRCID(GUISAVEONEXIT), Main::onSaveOnExit)
    EVT_MENU(XRCID(GUIDEFAULTWINDOWPOS), Main::onDefaultWindowPosition)
    EVT_MENU(XRCID(GUIDEFAULTGUISIZE), Main::onDefaultGuiSize)
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

    EVT_MENU(XRCID("GUI"), Main::onXmlRomRamOptionGui)
    EVT_MENU(XRCID("XML"), Main::onXmlRomRamOptionXml)

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
    EVT_GUI_MSG(SET_STATUS_BAR_LED, Main::setUpdateLedStatus)
    EVT_GUI_MSG(ENABLE_CLOCK, Main::setEnableClockEvent)
    EVT_GUI_MSG(PAUSE_STATE, Main::setPauseStateEvent)
    EVT_GUI_MSG(CHANGE_HW_TAPE_STATE, Main::setHwTapeStateEvent)
    EVT_GUI_MSG(SET_LOCATION_STATE, Main::setLocationStateEvent)
    EVT_GUI_MSG(SET_VIPIILED, Main::setUpdateVipIILedStatus)

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
        p_Main->onStart();
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
    { wxCMD_LINE_SWITCH, "v", "verbose", "verbose output"},                              // only valid in combination with -c
    { wxCMD_LINE_SWITCH, "f", "fullscreen", "full screen mode"},                         // only valid in combination with -c
    { wxCMD_LINE_SWITCH, "u", "skipupdate", "skip update check"},                                    
    { wxCMD_LINE_SWITCH, "w", "window", "non fixed window positions"},                                    
    { wxCMD_LINE_OPTION, "c", "computer", "start emulator without gui and for specified computer" }, // Switch off GUI
    { wxCMD_LINE_OPTION, "s", "software", "load specified software on start" },          // only valid in combination with -c
    { wxCMD_LINE_OPTION, "s0", "software0", "load binary software in gui slot 0 on start" },  // only valid in combination with -c
    { wxCMD_LINE_OPTION, "s1", "software1", "load binary software in gui slot 1 on start" },  // only valid in combination with -c
    { wxCMD_LINE_OPTION, "r", "run", "run specified software on start" },                // only valid in combination with -c
    { wxCMD_LINE_OPTION, "x", "xml", "load specified xml file on start" },               // only valid in combination with -c
    { wxCMD_LINE_OPTION, "ch", "chip8", "load specified chip8 software on start" },      // only valid in combination with -c
    { wxCMD_LINE_OPTION, "st", "st2", "load specified st2 software on start" },          // only valid in combination with -c

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
    wxString software = "", xmlFile = "";
    wxString computer, computerLower, dirName, dirNameLower, computerFound = "";
 
    if (parser.Found("c", &computer))
    {
        wxDir *dir;
        dir = new wxDir (dataDir_ + "Xml");

        computerLower = computer;
        computerLower.MakeLower();
        bool dirFound = dir->GetFirst(&dirName,  wxEmptyString, wxDIR_DIRS);
        while (dirFound)
        {
            dirNameLower = dirName;
            dirNameLower.MakeLower();
            if (dirNameLower == computerLower)
                computerFound = dirName;

            dirFound = dir->GetNext(&dirName);
        }

        if (computerFound == "")
        {
            size_t number = 0;
            while (commandComputerList_[number] != computerLower && commandComputerList_[number] != "")
                number+=2;

            if (commandComputerList_[number] == computerLower)
                computerFound = commandComputerList_[number+1];
        }
       
        if (computerFound != "")
        {
            configPointer->Write("Computer/XmlDirComboString", computerFound);
            startComputer_ = XML;
            mode_.gui = false;

            if (parser.Found("x", &xmlFile))
            {
                if (xmlFile.Right(4) != ".xml")
                    xmlFile += ".xml";
               
                if (wxFile::Exists(dataDir_ + "Xml" + pathSeparator_ + computerFound + pathSeparator_ + xmlFile))
                {
                   configPointer->Write("/Computer/XmlFile/" + computerFound, xmlFile);
                }
                else
                {
                   wxMessageOutput::Get()->Printf("Xml file not found");
                   return false;
                }
            }
            else
            {
                xmlFile = configPointer->Read("/Computer/XmlFile/"+ computerFound, "");
                if (xmlFile == "")
                {
                   int number = 0;
                   while (defaultComputerList_[number] != "" && defaultComputerList_[number] != computerFound)
                       number += 3;

                   if (defaultComputerList_[number] != "")
                       xmlFile = defaultComputerList_[number+2];
                }
               
                if (xmlFile == "")
                {
                   wxMessageOutput::Get()->Printf("Xml file not found");
                   return false;
                }
            }
            
            getSoftware("Xml", "Software_File0", "");
            getSoftware("Xml", "Software_File1", "");
            if (parser.Found("s", &software))
            {
                mode_.load = true;
                if (software.Right(4) == ".st2")
                   getSoftware("Xml", "St2_File", software);
                else
                {
                   if (software.Right(4) == ".ch8" || software.Right(4) == ".c8x" || software.Right(5) == ".ch10" || software.Right(4) == ".sc8")
                      getSoftware("Xml", "Chip_8_Software", software);
                   else
                      getSoftware("Xml", "Software_File0", software);
                }
            }
            if (parser.Found("s0", &software))
            {
                mode_.load = true;
                if (software.Right(4) == ".st2")
                   getSoftware("Xml", "St2_File", software);
                else
                {
                   if (software.Right(4) == ".ch8" || software.Right(4) == ".c8x" || software.Right(5) == ".ch10" || software.Right(4) == ".sc8")
                      getSoftware("Xml", "Chip_8_Software", software);
                   else
                      getSoftware("Xml", "Software_File0", software);
                }
            }
            if (parser.Found("s1", &software))
            {
                mode_.load = true;
                if (software.Right(4) == ".st2")
                   getSoftware("Xml", "St2_File", software);
                else
                {
                   if (software.Right(4) == ".ch8" || software.Right(4) == ".c8x" || software.Right(5) == ".ch10" || software.Right(4) == ".sc8")
                      getSoftware("Xml", "Chip_8_Software", software);
                   else
                      getSoftware("Xml", "Software_File1", software);
                }
            }
            if (parser.Found("r", &software))
            {
               mode_.run = true;
               if (software.Right(4) == ".st2")
                  getSoftware("Xml", "St2_File", software);
               else
               {
                  if (software.Right(4) == ".ch8" || software.Right(4) == ".c8x" || software.Right(5) == ".ch10" || software.Right(4) == ".sc8")
                     getSoftware("Xml", "Chip_8_Software", software);
                  else
                     getSoftware("Xml", "Software_File0", software);
               }
            }
            if (parser.Found("ch", &software))
            {
               mode_.load = true;
               getSoftware("Xml", "Chip_8_Software", software);
            }
            if (parser.Found("st", &software))
            {
               mode_.load = true;
               getSoftware("Xml", "St2_File", software);
            }

            return true;
        }
        wxMessageOutput::Get()->Printf("Incorrect computer name specified");
        delete dir;
        return false;
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
    selectedTab_ = XML;
    computerRunning_ = false;

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

        int answer = wxMessageBox("New release detected: \n\nRe-install of 1802 software files recommended\n\nThis will overwrite files in the 1802 software directory:\n"+dataDir_+"\n\nContinue to install default 1802 software files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
        if (answer == wxYES)
        {
            reInstall(applicationDirectory_ + "data" + pathSeparator_, dataDir_, pathSeparator_);
           
            if (wxFile::Exists(dataDir_ + "CosmacElf"  + pathSeparator_+ "cosmac-elf,bare.xml"))
            {
               answer = wxMessageBox("Old XML files detected: \n\nCleanup of Xml folder recommended\n\nThis will remove older .xml files:\n"+dataDir_+"\n\nContinue cleanup of Xml folder?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
               if (answer == wxYES)
               {
                  removeOldXml(dataDir_ + "Xml" + pathSeparator_, pathSeparator_);
               }
            }
        }
    }

    bool softwareDirInstalled;
   
    wxDir *dir;
    wxString dirName;
    dir = new wxDir (applicationDirectory_ + "data" + pathSeparator_);
   
    bool dirFound = dir->GetFirst(&dirName, wxEmptyString, wxDIR_DIRS);
    while (dirFound)
    {
        configPointer->Read(dirName + "/SoftwareDirInstalled", &softwareDirInstalled, false);
        if (!softwareDirInstalled)
        {
            if (!wxDir::Exists(dataDir + dirName))
            {
                int answer = wxMessageBox("1802 Software directory " + dirName + " does not exist, install default files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
                if (answer == wxYES)
                {
                    wxDir::Make(dataDir + dirName);
                    p_Main->reInstall(applicationDirectory_ + "data" + pathSeparator_ + dirName + pathSeparator_,  dataDir + dirName + pathSeparator_, pathSeparator_);
                }
                configPointer->Write(dirName + "/SoftwareDirInstalled", true);
            }
            wxDir compDir(dataDir + dirName);
            if (!compDir.HasFiles())
            {
                int answer = wxMessageBox("1802 Software directory " + dirName + " is empty, install default files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
                if (answer == wxYES)
                {
                    p_Main->reInstall(applicationDirectory_ + "data" + pathSeparator_ + dirName + pathSeparator_,  dataDir + dirName + pathSeparator_, pathSeparator_);
                }
                configPointer->Write(dirName + "/SoftwareDirInstalled", true);
            }
        }
       dirFound = dir->GetNext(&dirName);
    }
    delete dir;
    bool redundantFilesRemoveCheck;
    configPointer->Read("/Main/RedundantFilesRemoveCheck", &redundantFilesRemoveCheck, false);
    if (!redundantFilesRemoveCheck)
    {
        removeRedundantFiles();
        configPointer->Write("/Main/RedundantFilesRemoveCheck", true);
    }
 
   if (forceGuiSizeReset && mode_.window_position_fixed)
       fixedWindowPosition();

    readConfig();
    
    oldGauge_ = 1;
    vuPointer = new wxTimer(this, 902);
 //   cpuPointer = new wxTimer(this, 903);
    updateCheckPointer = new wxTimer(this, 905);
    traceTimeoutPointer = new wxTimer(this, 906);
    keyDebounceTimeoutPointer = new wxTimer(this, 907);
    guiSizeTimeoutPointer = new wxTimer(this, 908);
    guiRedrawBarTimeOutPointer = new wxTimer(this, 909);
    directAssPointer = new wxTimer(this, 910);
    guiSizeTimerStarted_ = false;
    
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
       delete clockText;
       delete clockTextCtrl;
       delete mhzText;
       delete startButton;
    }

    delete vuPointer;
    delete directAssPointer;
//    delete cpuPointer;
    delete updateCheckPointer;
    delete traceTimeoutPointer;
    delete keyDebounceTimeoutPointer;
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
    size.y += 86;
#endif
#if defined (__linux__)
    size.x += 30;
    size.y += 110;
#endif
#if defined (__WXMSW__)
    size.x += 44;
    size.y += 110;
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
    
    if (mode_.gui)
    {
        configPointer->Write("/Main/Window_Size_X_133", windowInfo.mainwX);
        configPointer->Write("/Main/Window_Size_Y_133", windowInfo.mainwY);
       
        configPointer->Write("/Main/Selected_Tab", XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->GetSelection());
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

       if (menubarPointer->IsChecked(XRCID("GUI")))
           configPointer->Write("/Main/XmlRomRamOption", "GUI");
       if (menubarPointer->IsChecked(XRCID("XML")))
           configPointer->Write("/Main/XmlRomRamOption", "XML");
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
    writeXmlDirConfig();
    writeSbConfig();
    writeXmlConfig();
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
    int borderX[VIDEOXMLMAX];
    int borderY[VIDEOXMLMAX];

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

    borderX[VIDEOMICROVT] = 0;
    borderY[VIDEOMICROVT] = 0;  //Video Terminal
    borderX[VIDEO1870] = 0;
    borderY[VIDEO1870] = 0; // CDP 1870

    colour[COL_VT_FORE] = "#00ff00";    // foreground vt
    colour[COL_VT_BACK] = "#004000";    // background vt
    colour[COL_VT_HIGH] = "#00ff00";    // highlight vt

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
    borderX[VIDEOXML1864] = 8;
    borderY[VIDEOXML1864] = 32;  //1864
    borderX[VIDEOXMLSN76430N] = 0;
    borderY[VIDEOXMLSN76430N] = 0;  //1864
    borderX[VIDEOVIP2K] = 0;
    borderY[VIDEOVIP2K] = 0;  //VIP2K
    borderX[VIDEOFRED] = 0;
    borderY[VIDEOFRED] = 0;  //FRED
    borderX[VIDEOCOIN] = 11;
    borderY[VIDEOCOIN] = 33;  //Coin

    colour[COL_PIXIE_FORE] = "#ffffff";    // foreground pixie
    colour[COL_PIXIE_BACK] = "#000000";    // background pixie
    colour[COL_CDP1862_BLACK] = "#141414";
    colour[COL_CDP1862_RED] = "#ff0000";
    colour[COL_CDP1862_BLUE] = "#0000ff";
    colour[COL_CDP1862_MAGENTA] = "#ff00ff";
    colour[COL_CDP1862_GREEN] = "#00ff00";
    colour[COL_CDP1862_YELLOW] = "#ffff00";
    colour[COL_CDP1862_CYAN] = "#00ffff";
    colour[COL_CDP1862_WHITE] = "#ffffff";
    colour[COL_CDP1862_BACK_BLUE] = "#000080";
    colour[COL_CDP1862_BACK_BLACK] = "#000000";
    colour[COL_CDP1862_BACK_GREEN] = "#008000";
    colour[COL_CDP1862_BACK_RED] = "#800000";
    colour[COL_CDP1864_WHITE] = "#ffffff";
    colour[COL_CDP1864_RED] = "#ff0000";
    colour[COL_CDP1864_BLUE] = "#0000ff";
    colour[COL_CDP1864_MAGENTA] = "#ff00ff";
    colour[COL_CDP1864_GREEN] = "#00ff00";
    colour[COL_CDP1864_YELLOW] = "#ffff00";
    colour[COL_CDP1864_CYAN] = "#00ffff";
    colour[COL_CDP1864_BLACK] = "#141414";
    colour[COL_CDP1864_BACK_BLUE] = "#000080";
    colour[COL_CDP1864_BACK_BLACK] = "#000000";
    colour[COL_CDP1864_BACK_GREEN] = "#008000";
    colour[COL_CDP1864_BACK_RED] = "#800000";
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
    colour[COL_ST4_BLACK] = "#141414"; // Black
    colour[COL_ST4_RED] = "#ff4040"; // Red
    colour[COL_ST4_BLUE] = "#4040ff"; // Blue
    colour[COL_ST4_MAGENTA] = "#ff40ff"; // Pink
    colour[COL_ST4_GREEN] = "#40ff40"; // Green
    colour[COL_ST4_YELLOW] = "#ffff40"; // Yellow
    colour[COL_ST4_CYAN] = "#40ffff"; // Cyan
    colour[COL_ST4_WHITE] = "#ffffff"; // white
    colour[COL_ST4_BACK_BLACK] = "#000000";
    colour[COL_ST4_BACK_RED] = "#a00000";
    colour[COL_ST4_BACK_BLUE] = "#0000a0";
    colour[COL_ST4_BACK_MAGENTA] = "#a000a0";
    colour[COL_ST4_BACK_GREEN] = "#00a000";
    colour[COL_ST4_BACK_YELLOW] = "#d0d000";
    colour[COL_ST4_BACK_CYAN] = "#00d0d0";
    colour[COL_ST4_BACK_WHITE] = "#fbfbfb";

    setScreenInfo(0, COL_MAX, colour, VIDEOXMLMAX, borderX, borderY);
    setComputerInfo("Xml", "");
        
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
         clockText = new wxStaticText(XRCCTRL(*this, "Computer", wxPanel), wxID_ANY, "Clock:", wxPoint(defaultGuiSize_.x - windowInfo.clockTextCorrectionSingleTabX, defaultGuiSize_.y - windowInfo.clockTextCorrectionSingleTabY+13));
         clockTextCtrl = new FloatEdit(XRCCTRL(*this, "Computer", wxPanel), GUI_CLOCK_TEXTCTRL + 0, "", wxPoint(defaultGuiSize_.x - windowInfo.clockCorrectionSingleTabX, defaultGuiSize_.y - windowInfo.clockCorrectionSingleTabY+13), wxSize(windowInfo.clockSize, windowInfo.floatHeight));
         mhzText = new wxStaticText(XRCCTRL(*this, "Computer", wxPanel), wxID_ANY, "MHz", wxPoint(defaultGuiSize_.x - windowInfo.mhzTextCorrectionSingleTabX, defaultGuiSize_.y - windowInfo.mhzTextCorrectionSingleTabY+13));
         stopButton = new wxButton(XRCCTRL(*this, "Computer", wxPanel), GUI_STOP_BUTTON + 0, "Stop", wxPoint(defaultGuiSize_.x - windowInfo.stopCorrectionSingleTabX, defaultGuiSize_.y - windowInfo.stopCorrectionSingleTabY+13), wxSize(80, windowInfo.startHeight));
         startButton = new wxButton(XRCCTRL(*this, "Computer", wxPanel), GUI_START_BUTTON + 0, "Start", wxPoint(defaultGuiSize_.x - windowInfo.startCorrectionSingleTabX, defaultGuiSize_.y - windowInfo.startCorrectionSingleTabY+13), wxSize(80, windowInfo.startHeight));
         
#if defined(__WXMAC__)
          clockText->SetFont(*defaultFont);
          clockTextCtrl->SetFont(*defaultFont);
          mhzText->SetFont(*defaultFont);
          startButton->SetFont(*defaultFont);
          stopButton->SetFont(*defaultFont);
#endif
          this->Connect(GUI_CLOCK_TEXTCTRL + 0, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(GuiMain::onClock));
          this->Connect(GUI_START_BUTTON + 0, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::onStart));
          this->Connect(GUI_STOP_BUTTON + 0, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::onStop));
          stopButton->Enable(false);
    }

    computerConfiguration.memAccessConfiguration.saveStart = 0;
#if defined(__WXMAC__)
    delete defaultFont;
#endif
}

void Main::readConfig()
{
    wxMenuBar *menubarPointer = GetMenuBar();

    int saveSelectedTab = selectedTab_;
    configPointer->Read("/Main/Floating_Point_Zoom", &fullScreenFloat_, true);

    wxString RomRamOptionString = configPointer->Read("/Main/XmlRomRamOption", "GUI");
    if (mode_.gui)
       menubarPointer->Check(XRCID(RomRamOptionString), true);
    XmlRomRamOptionGui_ = (RomRamOptionString == "GUI");

    readDebugConfig();
    readSbConfig();
    readXmlConfig();

    selectedTab_ = saveSelectedTab;

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
        long selected_tab = configPointer->Read("/Main/Selected_Debugger_Tab", 0l);
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
        XRCCTRL(*this, "DebuggerChoiceBook", wxNotebook)->SetClientSize(mainWindowSize.x - offset, mainWindowSize.y - offset);

#if defined (__WXMSW__)
        if (windowInfo.operatingSystem != OS_WINDOWS_2000 )
        {
            XRCCTRL(*this, "Computer", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "Configuration", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "DirectAssembler", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "Profiler", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelTrace", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelChip8", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "MemoryDump", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
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
    int borderSizeX, borderSizeX2, borderSizeY, borderSizeY2, borderSizeY3;
    
    if (mainWindowSize.x > 2000)
        mainWindowSize.x = 2000;
    if (mainWindowSize.y > 2000)
        mainWindowSize.y = 2000;
#if defined(__linux__)
    borderSizeX = 8;
    borderSizeX2 = 6;
    borderSizeY = 8;
    borderSizeY2 = 70;  // -33 due to move to regular tab
    borderSizeY3 = 10;
#endif
#if defined (__WXMSW__)
    borderSizeX = 10;
    borderSizeX2 = 6;
    borderSizeY = 10;
    borderSizeY2 = 19;  // -37 due to move to regular tab
    borderSizeY3 = 10;
#endif
#if defined(__WXMAC__)
    borderSizeX = 21;
    borderSizeX2 = 6;
    borderSizeY = 8;
    borderSizeY2 = 35;  // -37 due to move to regular tab
    borderSizeY3 = 10;
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
        XRCCTRL(*this, "DebuggerChoiceBook", wxNotebook)->SetClientSize(mainWindowSize.x-borderSizeX, mainWindowSize.y-borderSizeY);
        
        wxPoint position, positionBreakPointWindow, positionBreakPointWindowText;
        
        position = XRCCTRL(*this, "Message_Window", wxTextCtrl)->GetPosition();
        XRCCTRL(*this, "Message_Window", wxTextCtrl)->SetSize(mainWindowSize.x-position.x-borderSizeX2-fontFactorX, mainWindowSize.y-position.y-borderSizeY2-fontFactorY);
        
        position = XRCCTRL(*this, "TraceWindow", wxTextCtrl)->GetPosition();
        XRCCTRL(*this, "TraceWindow", wxTextCtrl)->SetSize(mainWindowSize.x-position.x-borderSizeX-fontFactorX-debugTraceWindowX, mainWindowSize.y-position.y-borderSizeY2-borderSizeY3-fontFactorY);
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
            XRCCTRL(*this, "Chip8TraceWindow", wxTextCtrl)->SetSize(mainWindowSize.x-position.x-borderSizeX-fontFactorX, mainWindowSize.y-position.y-borderSizeY2-borderSizeY3-fontFactorY);
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

void Main::onReInstallData(wxCommandEvent&WXUNUSED(event))
{
   int answer = wxMessageBox("This will overwrite files in the 1802 software directory:\n"+dataDir_+"\n\nContinue to install default 1802 software files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
   if (answer == wxYES)
   {
      reInstall(applicationDirectory_ + "data" + pathSeparator_, dataDir_, pathSeparator_);

      if (wxFile::Exists(dataDir_ + "Xml" + pathSeparator_+ "CosmacElf" + pathSeparator_+ "cosmac-elf,bare.xml") || wxFile::Exists(dataDir_ + "Xml" + pathSeparator_+ "Cosmac Elf" + pathSeparator_+ "cosmac-elf,bare.xml") )
      {
         answer = wxMessageBox("Old XML files detected: \n\nCleanup of Xml folder recommended\n\nThis will remove older .xml files in:\n"+dataDir_+"Xml\n\nContinue cleanup of Xml folder?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
         if (answer == wxYES)
         {
            removeOldXml(dataDir_ + "Xml" + pathSeparator_, pathSeparator_);
         }
      }
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

void Main::removeOldXml(wxString dirName, wxString pathSep)
{
   wxString dirList[]=
   {
      "CDP18S020",
      "cdp18s020",
      "Cidelsa",
      "cidelsa",
      "CoinArcade",
      "coinarcade",
      "Comix",
      "comix",
      "Comx",
      "comx",
      "CosmacElf",
      "cosmac-elf",
      "Cosmac Elf",
      "cosmac-elf",
      "CosmacGameSystem",
      "cosmacgame",
      "Cosmicos",
      "cosmicos",
      "Cybervision",
      "cybervision2001",
      "Elf",
      "elf",
      "Elf2K",
      "elf2k",
      "Eti",
      "eti-660",
      "FRED1",
      "fred1",
      "FRED1_5",
      "fred1.5",
      "HUG1802",
      "hug1802",
      "JVIP",
      "jvip",
      "Velf",
      "jvip",
      "Macbug",
      "macbug",
      "MCDS",
      "mcds",
      "Membership",
      "membership",
      "Microboard",
      "microboard",
      "Microtutor",
      "microtutor",
      "MicrotutorII",
      "microtutor2",
      "MS2000",
      "ms2000",
      "Nano",
      "nano",
      "NetronicsElfII",
      "netronics-elf",
      "PicoElfV2",
      "pico",
      "QuestSuperElf",
      "quest-super-elf",
      "Studio2020",
      "studioiv",
      "StudioII",
      "studioii",
      "StudioIII",
      "studioiii",
      "StudioIV",
      "studioiv",
      "TMC600",
      "tmc-600",
      "TMC1800",
      "tmc1800",
      "TMC2000",
      "tmc2000",
      "UC1800",
      "uc1800",
      "Velf",
      "velf",
      "Vip",
      "vip",
      "Vip2K",
      "vip2k",
      "VipII",
      "vipii",
      "Visicom",
      "visicom",
      "",
      "",
   };
   
   wxString fileDeleteList[]=
   {
      "Pecom",
      "pecom,32.xml",
      "Pecom",
      "pecom,64.v1.xml",
      "Pecom",
      "pecom,64.v4.xml",
      "Pecom",
      ".DS_Store",
      "Cosmac Elf",
      "cosmac-elf,bare.xml",
      "Cosmac Elf",
      "cosmac-elf,led,keypad.xml",
      "Cosmac Elf",
      ".DS_Store",
      "Pico Elf V2",
      "pico,elfos0.3.2-serial.xml",
      "Pico Elf V2",
      "pico,elfos0.3.2-uart.xml",
      "Pico Elf V2",
      "pico,elfos-diskless.xml",
      "Pico Elf V2",
      "pico,elfos-serial.xml",
      "Pico Elf V2",
      "pico,elfos-uart.xml",
      "Pico Elf V2",
      ".DS_Store",
      "Quest Super Elf",
      "quest-super-elf,bare.xml",
      "Quest Super Elf",
      ".DS_Store",
      "Microtutor II",
      "microtutor2-bare.xml",
      "Microtutor II",
      "microtutor2-tops.xml",
      "Microtutor II",
      ".DS_Store",
      "Netronics Elf II",
      "netronics-elf,bare.xml",
      "Netronics Elf II",
      ".DS_Store",
      "TMC-600",
      "tmc-600,bare.xml",
      "TMC-600",
      "tmc-600,exp-151182.xml",
      "TMC-600",
      ".DS_Store",
      "",
      "",
   };

   wxString dirDeleteList[]=
   {
      "Pecom",
      "Cosmac Elf",
      "Pico Elf V2",
      "Quest Super Elf",
      "Microtutor II",
      "Netronics Elf II",
      "TMC-600",
      "",
      "",
   };

   wxDir dir (dirName);
   bool cont;
   size_t number=0;
   wxString filename;

   while (dirList[number] != "")
   {
      dir.Open(dirName + dirList[number]);
      cont = dir.GetFirst(&filename);
      
      while ( cont )
      {
         if (filename.Left(dirList[number+1].Len()) == dirList[number+1])
         {
            wxString folder = dirName + dirList[number] + pathSeparator_;
            wxString newFileName = filename.Right(filename.Len() - dirList[number+1].Len() - 1);
            wxString newFileName2 = filename.Right(filename.Len() - dirList[number+1].Len() - 3);
            newFileName2.Replace(" + ", "+");

            if (newFileName == "xml")
               newFileName = dirList[number+1] + ".xml";

            if (wxFile::Exists(folder + newFileName))
               wxRemoveFile(folder + filename);
            if (wxFile::Exists(folder + newFileName2))
               wxRemoveFile(folder + filename);
         }
         cont = dir.GetNext(&filename);
      }
      number += 2;
   }
  
   number=0;
   while (fileDeleteList[number] != "")
   {
      if (wxFile::Exists(dirName + fileDeleteList[number] + pathSeparator_ + fileDeleteList[number+1]))
         wxRemoveFile(dirName + fileDeleteList[number] + pathSeparator_ + fileDeleteList[number+1]);
      number += 2;
   }

   number=0;
   wxString dummyFile;
   while (dirDeleteList[number] != "")
   {
      dir.Open(dirName + dirDeleteList[number]);

      if (wxDir::Exists(dirName + dirDeleteList[number]) && !dir.HasFiles() && !dir.HasSubDirs())
          wxDir::Remove(dirName + dirDeleteList[number]);
      number ++;
   }
}

void Main::reInstallOnNotFound(wxString fileTypeString)
{
    int answer = wxMessageBox(fileTypeString + " file for the " + computerInfo.name + " is missing,\nre-install all " + computerInfo.name + " default files?", "Emma 02",  wxICON_EXCLAMATION | wxYES_NO);
    if (answer == wxYES)
    {
        
        p_Main->reInstall(applicationDirectory_ + "data" + pathSeparator_ + xmlDirComboString + pathSeparator_,  dataDir_ + xmlDirComboString + pathSeparator_, pathSeparator_);
    }
}

void Main::checkAndReInstallFile(wxString fileAndPath, wxString fileTypeString)
{
    if (fileAndPath == "")
        return;
    
    if (!wxFile::Exists(fileAndPath))
        p_Main->reInstallOnNotFound(fileTypeString);
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
 
       if (menubarPointer->IsChecked(XRCID("GUI")))
           configPointer->Write("/Main/XmlRomRamOption", "GUI");
       if (menubarPointer->IsChecked(XRCID("XML")))
           configPointer->Write("/Main/XmlRomRamOption", "XML");

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
    oldXmlFileName_ = "";

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

void Main::onXmlRomRamOptionGui(wxCommandEvent&WXUNUSED(event))
{
    XmlRomRamOptionGui_ = true;
}

void Main::onXmlRomRamOptionXml(wxCommandEvent&WXUNUSED(event))
{
    XmlRomRamOptionGui_ = false;
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
        if (computerRunning_ && (selectedTab_ == PROFILERTAB || selectedTab_ == DIRECTASSTAB))
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
        if (computerRunning_ && (selectedTab_ == PROFILERTAB || selectedTab_ == DIRECTASSTAB))
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
        if (computerRunning_ && (selectedTab_ == PROFILERTAB || selectedTab_ == DIRECTASSTAB))
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
        if (computerRunning_ && (selectedTab_ == PROFILERTAB || selectedTab_ == DIRECTASSTAB))
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
           if (computerConfiguration.printerOn_)
               p_Main->onF4();
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
        if (computerRunning_)
           p_Computer->runPressed();
        return true;
    }

    if (computerRunning_)
    {
        if (key == p_Computer->getInKey1() || key == p_Computer->getInKey2())
        {
            p_Computer->onInButtonPress(computerConfiguration.mainFrontPanelConfiguration.dataSwitchButtons);
            return true;
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
        if ((p_Video[computerConfiguration.videoNumber_] != NULL) && (p_Vt100[UART1] != NULL))
        {
            if (!p_Video[computerConfiguration.videoNumber_]->isFullScreenSet() && !p_Vt100[UART1]->isFullScreenSet())
                p_Vt100[UART1]->onF3();
            else if (p_Vt100[UART1]->isFullScreenSet())
            {
                p_Vt100[UART1]->onF3();
                while (p_Vt100[UART1]->isFullScreenSet()) 
                {
                    wxSleep(1);
                }
                p_Video[computerConfiguration.videoNumber_]->onF3();
            }
            else
            {
                p_Video[computerConfiguration.videoNumber_]->onF3();
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
                if (p_Video[computerConfiguration.videoNumber_] != NULL)
                    p_Video[computerConfiguration.videoNumber_]->onF3();
                if (p_Vt100[UART1] != NULL)
                    p_Vt100[UART1]->onF3();
            }
        }
    }
}

void Main::popUp()
{
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
        {
            if (selectedTab_ == XMLTAB)
               onStart();
        }
        return;
    }
    event.Skip();
}

bool Main::runPressed()
{
   bool returnValue = runPressed_;
   runPressed_ = false;
   
   return returnValue;
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
    p_Main->writeXmlWindowConfig();

    this->Move(mainWindowX_, mainWindowY_);
   
    if (computerRunning_)
       p_Computer->moveWindows();
   
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

    computerConfiguration.cdp1861Configuration.x = -1;
    computerConfiguration.cdp1861Configuration.y = -1;
    computerConfiguration.tmsConfiguration.x = -1;
    computerConfiguration.tmsConfiguration.y = -1;
    computerConfiguration.mc6845Configuration.x = -1;
    computerConfiguration.mc6845Configuration.y = -1;
    computerConfiguration.mc6847Configuration.x = -1;
    computerConfiguration.mc6847Configuration.y = -1;
    computerConfiguration.i8275Configuration.x = -1;
    computerConfiguration.i8275Configuration.y = -1;
    computerConfiguration.videoTerminalConfiguration.x = -1;
    computerConfiguration.videoTerminalConfiguration.y = -1;

    computerConfiguration.vis1870Configuration.x = -1;
    computerConfiguration.vis1870Configuration.y = -1;
    computerConfiguration.sn76430NConfiguration.x = -1;
    computerConfiguration.sn76430NConfiguration.y = -1;
    computerConfiguration.coinConfiguration.x = -1;
    computerConfiguration.coinConfiguration.y = -1;
    computerConfiguration.cdp1864Configuration.x = -1;
    computerConfiguration.cdp1864Configuration.y = -1;
    computerConfiguration.studio4VideoConfiguration.x = -1;
    computerConfiguration.studio4VideoConfiguration.y = -1;
    computerConfiguration.vip2KVideoConfiguration.x = -1;
    computerConfiguration.vip2KVideoConfiguration.y = -1;
    computerConfiguration.fredVideoConfiguration.x = -1;
    computerConfiguration.fredVideoConfiguration.y = -1;
    for (std::vector<Cdp1851Configuration>::iterator cdp1851 = computerConfiguration.cdp1851Configuration.begin (); cdp1851 != computerConfiguration.cdp1851Configuration.end (); ++cdp1851)
    {
       cdp1851->pos.x = -1;
       cdp1851->pos.y = -1;
    }
    for (std::vector<Cdp1852Configuration>::iterator cdp1852 = computerConfiguration.cdp1852Configuration.begin (); cdp1852 != computerConfiguration.cdp1852Configuration.end (); ++cdp1852)
    {
       cdp1852->pos.x = -1;
       cdp1852->pos.y = -1;
    }
   for (std::vector<FrontPanelConfiguration>::iterator frontpanel = computerConfiguration.frontPanelConfiguration.begin (); frontpanel != computerConfiguration.frontPanelConfiguration.end (); ++frontpanel)
   {
      frontpanel->pos.x = -1;
      frontpanel->pos.y = -1;
   }
}

void Main::fixedWindowPosition()
{
    mainWindowX_ = 30 + offsetX_;
    mainWindowY_ = 30 + offsetY_;

    computerConfiguration.cdp1861Configuration.x = computerConfiguration.cdp1861Configuration.defaultX;
    computerConfiguration.cdp1861Configuration.y = computerConfiguration.cdp1861Configuration.defaultY;
    computerConfiguration.tmsConfiguration.x = computerConfiguration.tmsConfiguration.defaultX;
    computerConfiguration.tmsConfiguration.y = computerConfiguration.tmsConfiguration.defaultY;
    computerConfiguration.mc6845Configuration.x = computerConfiguration.mc6845Configuration.defaultX;
    computerConfiguration.mc6845Configuration.y = computerConfiguration.mc6845Configuration.defaultY;
    computerConfiguration.mc6847Configuration.x = computerConfiguration.mc6847Configuration.defaultX;
    computerConfiguration.mc6847Configuration.y = computerConfiguration.mc6847Configuration.defaultY;
    computerConfiguration.i8275Configuration.x = computerConfiguration.i8275Configuration.defaultX;
    computerConfiguration.i8275Configuration.y = computerConfiguration.i8275Configuration.defaultY;
    computerConfiguration.videoTerminalConfiguration.x = computerConfiguration.videoTerminalConfiguration.defaultX;
    computerConfiguration.videoTerminalConfiguration.y = computerConfiguration.videoTerminalConfiguration.defaultY;

    computerConfiguration.vis1870Configuration.x = computerConfiguration.vis1870Configuration.defaultX;
    computerConfiguration.vis1870Configuration.y = computerConfiguration.vis1870Configuration.defaultY;
    computerConfiguration.sn76430NConfiguration.x = computerConfiguration.sn76430NConfiguration.defaultX;
    computerConfiguration.sn76430NConfiguration.y = computerConfiguration.sn76430NConfiguration.defaultY;
    computerConfiguration.coinConfiguration.x = computerConfiguration.coinConfiguration.defaultX;
    computerConfiguration.coinConfiguration.y = computerConfiguration.coinConfiguration.defaultY;
    computerConfiguration.cdp1864Configuration.x = computerConfiguration.cdp1864Configuration.defaultX;
    computerConfiguration.cdp1864Configuration.y = computerConfiguration.cdp1864Configuration.defaultY;
    computerConfiguration.studio4VideoConfiguration.x = computerConfiguration.studio4VideoConfiguration.defaultX;
    computerConfiguration.studio4VideoConfiguration.y = computerConfiguration.studio4VideoConfiguration.defaultY;
    computerConfiguration.vip2KVideoConfiguration.x = computerConfiguration.vip2KVideoConfiguration.defaultX;
    computerConfiguration.vip2KVideoConfiguration.y = computerConfiguration.vip2KVideoConfiguration.defaultY;
    computerConfiguration.fredVideoConfiguration.x = computerConfiguration.fredVideoConfiguration.defaultX;
    computerConfiguration.fredVideoConfiguration.y = computerConfiguration.fredVideoConfiguration.defaultY;
   
    for (std::vector<Cdp1851Configuration>::iterator cdp1851 = computerConfiguration.cdp1851Configuration.begin (); cdp1851 != computerConfiguration.cdp1851Configuration.end (); ++cdp1851)
    {
       cdp1851->pos.x = cdp1851->defaultPos.x;
       cdp1851->pos.y = cdp1851->defaultPos.y;
    }
    for (std::vector<Cdp1852Configuration>::iterator cdp1852 = computerConfiguration.cdp1852Configuration.begin (); cdp1852 != computerConfiguration.cdp1852Configuration.end (); ++cdp1852)
    {
       cdp1852->pos.x = cdp1852->defaultPos.x;
       cdp1852->pos.y = cdp1852->defaultPos.y;
    }
   for (std::vector<FrontPanelConfiguration>::iterator frontpanel = computerConfiguration.frontPanelConfiguration.begin (); frontpanel != computerConfiguration.frontPanelConfiguration.end (); ++frontpanel)
   {
      frontpanel->pos.x = frontpanel->defaultPos.x;
      frontpanel->pos.y = frontpanel->defaultPos.y;
   }
}

void Main::onStart(wxCommandEvent&WXUNUSED(event))
{
    if (computerRunning_)
    {
        p_Computer->onReset();
        return;
    }
    onStart();
}

void Main::onStart()
{
    double zoom;
    long ms;
    int stereo = 1;
    int toneChannels = 1;
    int noiseChannels = 1;
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

    selectedTab_ = XML;
#ifndef __WXMAC__
    wxSetWorkingDirectory(workingDir_);
#endif
    setClock();
    toDouble(computerConfiguration.zoom_[computerConfiguration.videoNumber_], &zoom);

    if (!fullScreenFloat_)
        zoom = (int) zoom;

    wxDisplaySize(&x, &y);
   
    if (mode_.gui)
       XRCCTRL(*this, "Chip8Type", wxStaticText)->SetLabel("");
   
    parseXmlFile(computerConfiguration.xmlFileConfiguration.directory, computerConfiguration.xmlFileConfiguration.fileName);
    setXmlGui();
    
     p_Computer = new Computer (computerInfo.name, computerConfiguration.clockSpeed_, computerConfiguration.soundConfiguration.tempo, computerConfiguration);
     stereo = computerConfiguration.soundConfiguration.stereo;
     if (computerConfiguration.soundConfiguration.type == SOUND_SUPER_VP550)
         toneChannels = 2;
     if (computerConfiguration.soundConfiguration.type == SOUND_SUPER_VP551)
         toneChannels = 4;
     if (computerConfiguration.soundConfiguration.type == SOUND_AY_3_8912)
     {
         toneChannels = 6;
         noiseChannels = 6;
      }

    computerConfiguration.ledTime_.ToLong(&ms);
    computerConfiguration.ledTimeMs_ = ms;

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
    p_Computer->initCpu();
    p_Computer->configureComputer();
    if (!debugMode_)  percentageClock_ = 1;

   p_Computer->initSound(percentageClock_, bass_, treble_, toneChannels, noiseChannels, stereo, computerConfiguration);
   
    p_Computer->initComputer();
    AssInitConfig();

    if (mode_.gui)
    {
        p_Main->resetDisplay();
#if wxCHECK_VERSION(2, 9, 0)
        XRCCTRL(*this, "RunningComputer", wxStaticText)->SetLabelText("Running computer:  "+computerInfo.name);
        XRCCTRL(*this, "RunningCpu", wxStaticText)->SetLabelText("Running CPU:  "+cpuName[cpuType_]);
#else
        XRCCTRL(*this, "RunningComputer", wxStaticText)->SetLabel("Running computer:  "+computerInfo.name);
        XRCCTRL(*this, "RunningCpu", wxStaticText)->SetLabel("Running CPU:  "+cpuName[cpuType_]);
#endif
        assNew(0);
    }
    p_Computer->startComputer();
    if (!mode_.gui)
    {
      if (computerConfiguration.videoTerminalConfiguration.type == VTNONE)
          p_Main->eventVideoSetFullScreen(mode_.full_screen, 0);
      else
          p_Main->eventVtSetFullScreen(mode_.full_screen, UART1);
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
       
        if (!computerRunning_)
           return;

        setComxStatusLedOn (false);
        enableMemAccessGui(false);
        vuSet("VuXml", 0);

#if wxCHECK_VERSION(2, 9, 0)
        XRCCTRL(*this, "RunningComputer", wxStaticText)->SetLabelText("Last computer: "+computerInfo.name);
        XRCCTRL(*this, "RunningCpu", wxStaticText)->SetLabelText("Last CPU:  "+cpuName[cpuType_]);
#else
        XRCCTRL(*this, "RunningComputer", wxStaticText)->SetLabel("Last computer: "+computerInfo.name);
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
    if (emmaClosing_ || !mode_.gui)
        Destroy();
}

void Main::onComputer(wxNotebookEvent&event)
{
    switch(event.GetSelection())
    {
        case XMLTAB:
            selectedTab_ = XML;
            if (guiInitialized_)
            {
               vuSet("VuXml", 1);
               vuSet("VuXml", 0);
            }
        break;

        case DIRECTASSTAB:
            selectedTab_ = DIRECTASSTAB;
            directAss();
        break;

        case PROFILERTAB:
            selectedTab_ = PROFILERTAB;
            directAss();
        break;

        case MEMORYTAB:
            selectedTab_ = MEMORYTAB;
            memoryDisplay();
        break;

       case MESSAGETAB:
            selectedTab_ = MESSAGETAB;
       break;

        case DEBUGGERTAB:
            switch(XRCCTRL(*this, "DebuggerChoiceBook", wxNotebook)->GetSelection())
            {
                case TRACETAB:
                    debuggerChoice_ = TRACETAB;
                break;

                case CHIP8TAB:
                    debuggerChoice_ = CHIP8TAB;
                break;
            }
            selectedTab_ = DEBUGGERTAB;
        break;
    }
}

void Main::onDebuggerChoiceBook(wxNotebookEvent&event)
{
    switch(event.GetSelection())
    {
        case TRACETAB:
            debuggerChoice_ = TRACETAB;
        break;

        case CHIP8TAB:
            debuggerChoice_ = CHIP8TAB;
            if (computerRunning_ && pseudoLoaded_)
                updateChip8Window();
        break;
    }
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

    if (status)
        saveScrtValues("");

//     chip8ProtectedMode_= false;
//     XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
//     XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
     
     enableLoadGui(!status);
     setRealCas2();
     
     XRCCTRL(*this, "BatchConvertButtonXml", wxButton)->Enable(computerConfiguration.useBatchWav_ && !status);
     XRCCTRL(*this,"MainDirXml", wxChoice)->Enable(status);
     XRCCTRL(*this,"MainXmlXml", wxComboBox)->Enable(status);
     XRCCTRL(*this,"XmlButtonXml", wxButton)->Enable(status);
    
     if ((computerConfiguration.memoryConfiguration[romRamButton0_].type & 0xff) != UNDEFINED)
     {
         XRCCTRL(*this,"RomRam0Xml", wxComboBox)->Enable(status);
         XRCCTRL(*this,"RomRamButton0Xml", wxButton)->Enable(status);
     }
     if ((computerConfiguration.memoryConfiguration[romRamButton1_].type & 0xff) != UNDEFINED)
     {
         XRCCTRL(*this,"RomRam1Xml", wxComboBox)->Enable(status);
         XRCCTRL(*this,"RomRamButton1Xml", wxButton)->Enable(status);
     }
     XRCCTRL(*this,"PrintButtonXml", wxButton)->Enable(!status && computerConfiguration.printerOn_ && computerConfiguration.printMode_ != PRINTFILE);

     XRCCTRL(*this,"VTTypeXml",wxChoice)->Enable(status);
     if (XRCCTRL(*this,"VTTypeXml",wxChoice)->GetSelection() != VTNONE)
     {
         if (computerConfiguration.videoTerminalConfiguration.uart1854_defined || computerConfiguration.videoTerminalConfiguration.uart16450_defined)
         {
             XRCCTRL(*this, "VTBaudRTextXml", wxStaticText)->Enable(status);
             XRCCTRL(*this, "VTBaudRChoiceXml", wxChoice)->Enable(status);
         }
         XRCCTRL(*this, "VTBaudTChoiceXml", wxChoice)->Enable(status);
         XRCCTRL(*this, "VTBaudTTextXml", wxStaticText)->Enable(status);
         XRCCTRL(*this, "VtSetupXml", wxButton)->Enable(status);
     }
     if (!(computerConfiguration.videoTerminalConfiguration.external || computerConfiguration.videoTerminalConfiguration.loop_back))
     {
         XRCCTRL(*this,"FullScreenF3Xml", wxButton)->Enable(!status&(computerConfiguration.cdp1861Configuration.defined||computerConfiguration.tmsConfiguration.defined||computerConfiguration.mc6847Configuration.defined||computerConfiguration.mc6845Configuration.defined||computerConfiguration.i8275Configuration.defined||computerConfiguration.vis1870Configuration.defined||computerConfiguration.sn76430NConfiguration.defined||computerConfiguration.cdp1864Configuration.defined||(computerConfiguration.videoTerminalConfiguration.type != VTNONE)));
         XRCCTRL(*this,"ScreenDumpF5Xml", wxButton)->Enable(!status&(computerConfiguration.cdp1861Configuration.defined||computerConfiguration.tmsConfiguration.defined||computerConfiguration.mc6847Configuration.defined||computerConfiguration.mc6845Configuration.defined||computerConfiguration.i8275Configuration.defined||computerConfiguration.vis1870Configuration.defined||computerConfiguration.sn76430NConfiguration.defined||computerConfiguration.cdp1864Configuration.defined||(computerConfiguration.videoTerminalConfiguration.type != VTNONE)));
    }
     
    enableMemAccessGui(!status);
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

void Main::configureMessage(int ioGroup, wxString text)
{
    p_Main->message("Configuring " + text + getGroupMessage(ioGroup));
}

wxString Main::getGroupMessage(int ioGroup)
{
    wxString ioGroupStr = "";
    if (ioGroup != 0)
        ioGroupStr.Printf(" on group %d", ioGroup-1);

    return ioGroupStr;
}

void Main::configureMessage(vector<int>* ioGroup, wxString text)
{
    p_Main->message("Configuring " + text + getGroupMessage(ioGroup));
}

wxString Main::getGroupMessage(vector<int>* ioGroup)
{
    if (ioGroup->size() == 0)
       return "";
   
    wxString ioGroupString = " on group ", tempIoGroup = "%d";
    for (std::vector<int>::iterator ioGroupIterator = ioGroup->begin (); ioGroupIterator != ioGroup->end (); ++ioGroupIterator)
    {
        tempIoGroup.Printf(tempIoGroup, *ioGroupIterator);
        ioGroupString += tempIoGroup;
        tempIoGroup = ", %d";
    }

    return ioGroupString;
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
/*
wxString Main::getApplicationDir()
{
    return applicationDirectory_;
}*/

wxString Main::getPathSep()
{
    return pathSeparator_;
}

int Main::setFdcStepRate(int rate)
{
    switch(rate)
    {
        case 0: return computerConfiguration.fdcConfiguration.cyclesPerMilliSecond * 6;
        case 1: return computerConfiguration.fdcConfiguration.cyclesPerMilliSecond * 12;
        case 2: return computerConfiguration.fdcConfiguration.cyclesPerMilliSecond * 20;
        case 3: return computerConfiguration.fdcConfiguration.cyclesPerMilliSecond * 30;
    }
    return computerConfiguration.fdcConfiguration.cyclesPerMilliSecond * 12;
}

int Main::getUpdFdcCpms()
{
    return computerConfiguration.upd765Configuration.cyclesPerMilliSecond;
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
//    XRCCTRL(*this, "ZoomValueXml", wxTextCtrl)->ChangeValue(zoomStr);

    computerConfiguration.zoom_[videoNumber].Printf("%2.2f", zoom);
    correctZoomAndValue(SET_SPIN, videoNumber);
}

void Main::zoomEventVt(double zoom)
{
    if (!mode_.gui)
        return;

    if (zoomEventOngoing_)
        return;

//    wxString zoomStr;
//    zoomStr.Printf("%2.2f", zoom);
//    XRCCTRL(*this, "ZoomValueVtXml", wxTextCtrl)->ChangeValue(zoomStr);

    computerConfiguration.videoTerminalConfiguration.zoom.Printf("%2.2f", zoom);
    correctZoomVtAndValue(SET_SPIN);
}

void Main::traceTimeout(wxTimerEvent&WXUNUSED(event))
{
    if (selectedTab_ == DEBUGGERTAB && debuggerChoice_ == TRACETAB)
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
    if (selectedTab_ == DEBUGGERTAB && debuggerChoice_ == CHIP8TAB)
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
    switch (selectedTab_)
    {
       case DIRECTASSTAB:
           directAss();
           updateAssPage_ = false;
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
              if (computerConfiguration.memoryMapperConfiguration.defined)
              {
                  XRCCTRL(*this, "DebugPager", HexEdit)->changeNumber(p_Computer->getPager(portExtender_));
                  XRCCTRL(*this, "DebugPortExtender", HexEdit)->changeNumber(portExtender_);
              }
              if (computerConfiguration.emsMemoryConfiguration.size() != 0)
              {
                  XRCCTRL(*this, "DebugEmsNumber", HexEdit)->changeNumber((int)emsNumber_);
                  XRCCTRL(*this, "DebugEmsPage", HexEdit)->changeNumber(p_Computer->getEmsPage(emsNumber_));
              }
              if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
                  XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->changeNumber(p_Computer->getSelectedSlot());
              if (computerConfiguration.slotConfiguration.banksInUse_)
                  XRCCTRL(*this, "DebugExpansionBank", SlotEdit)->changeNumber(p_Computer->getSelectedBank());

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
                                       if (computerConfiguration.mc6847Configuration.defined)
                                       {
                                           if (memoryStart_ <= getAddressMask())
                                               XRCCTRL(*this, idReference, MemEdit)->changeNumber2X(debugReadMem(memoryStart_+y*16+x));
                                           else
                                               XRCCTRL(*this, idReference, MemEdit)->changeNumber1X(debugReadMem(memoryStart_+y*16+x));
                                       }
                                       else
                                           XRCCTRL(*this, idReference, MemEdit)->changeNumber2X(0);
                                   break;
                                       
                 /*                  case CDP_1870_COLOUR:
                                       if (runningComputer_ == TMC600)
                                           XRCCTRL(*this, idReference, MemEdit)->changeNumber1X(debugReadMem(memoryStart_+y*16+x));
                                   break; TO BE FIXED, how does 1870 colour memory work in XML - change to nibles? */
                                          
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
    if (selectedTab_ == DEBUGGERTAB)
    {
        switch (debuggerChoice_)
        {
            case TRACETAB:
                if (percentageClock_ == 1)
                    p_Main->updateWindow();
            break;
        }
    }
}

void Main::vuTimeout(wxTimerEvent&WXUNUSED(event))
{
    if (computerRunning_)
       vuSet("VuXml", p_Computer->getGaugeValue());
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
    
   if (lastInstructionCounter_ != instructionCounter || instructionCounter == 0)
   //if (instructionCounter != 0 && lastInstructionCounter_ != instructionCounter)
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

    if (computerRunning_)
    {
       switch (selectedTab_)
       {
          case DIRECTASSTAB:
          case PROFILERTAB:
             directAss();
          break;

          case MEMORYTAB:
            memoryDisplay();
          break;
       }
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
    guiTextColour[GUI_COL_WHITE] = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
   
#else
    darkMode_ = false;
    guiBackGround_ = wxColour(windowInfo.red, windowInfo.green, windowInfo.blue);
    wxColourDatabase colour;
    guiTextColour[GUI_COL_BLACK] = wxColour(colour.Find("BLACK"));
    guiTextColour[GUI_COL_WHITE] = wxColour(colour.Find("WHITE"));
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

   XRCCTRL(*this, "DebugExpansionBankText", wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_GREEN]);
   XRCCTRL(*this, "DebugExpansionBank", SlotEdit)->SetForegroundColour(guiTextColour[GUI_COL_GREEN]);

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
        popupDialog_->setLocation(computerConfiguration.memAccessConfiguration.useLocation, computerConfiguration.memAccessConfiguration.saveStartString, computerConfiguration.memAccessConfiguration.saveEndString, computerConfiguration.memAccessConfiguration.saveExecString);
   
    if (!mode_.gui)
        return;

    if (computerConfiguration.memAccessConfiguration.useLocation)
    {
        XRCCTRL(*this,"SaveStartXml", wxTextCtrl)->ChangeValue(computerConfiguration.memAccessConfiguration.saveStartString);
        XRCCTRL(*this,"SaveEndXml", wxTextCtrl)->ChangeValue(computerConfiguration.memAccessConfiguration.saveEndString);
        XRCCTRL(*this,"SaveExecXml", wxTextCtrl)->ChangeValue(computerConfiguration.memAccessConfiguration.saveExecString);
    }
    else
    {
        XRCCTRL(*this,"SaveStartXml", wxTextCtrl)->ChangeValue("");
        XRCCTRL(*this,"SaveEndXml", wxTextCtrl)->ChangeValue("");
        XRCCTRL(*this,"SaveExecXml", wxTextCtrl)->ChangeValue("");
    }
    XRCCTRL(*this, "UseLocationXml", wxCheckBox)->SetValue(computerConfiguration.memAccessConfiguration.useLocation);
    enableLocationGui();
}

void Main::eventSetLocation(bool state, Word saveStart, Word saveEnd, Word saveExec)
{
     wxString printBuffer;
    guiEvent event(GUI_MSG, SET_LOCATION);
    event.SetEventObject( p_Main );

     computerConfiguration.memAccessConfiguration.useLocation = state;
    computerConfiguration.memAccessConfiguration.saveStart = saveStart;
    computerConfiguration.memAccessConfiguration.saveEnd = saveEnd;
    computerConfiguration.memAccessConfiguration.saveExec = saveExec;

    printBuffer.Printf("%04X",saveStart);
     computerConfiguration.memAccessConfiguration.saveStartString = printBuffer;
    printBuffer.Printf("%04X",saveEnd);
     computerConfiguration.memAccessConfiguration.saveEndString = printBuffer;
    printBuffer.Printf("%04X",saveExec);
     computerConfiguration.memAccessConfiguration.saveExecString = printBuffer;

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setLocationStateEvent(guiEvent&WXUNUSED(event))
{
    wxString printBuffer;

    if (popupDialog_ != NULL)
        popupDialog_->setLocation(computerConfiguration.memAccessConfiguration.useLocation);
   
    if (!mode_.gui)
        return;

    XRCCTRL(*this, "UseLocationXml", wxCheckBox)->SetValue(computerConfiguration.memAccessConfiguration.useLocation);
   
    enableLocationGui();
}

void Main::eventSetLocation(bool state)
{
    guiEvent event(GUI_MSG, SET_LOCATION_STATE);
    event.SetEventObject( p_Main );

    computerConfiguration.memAccessConfiguration.useLocation = state;
//    computerConfiguration.memAccessConfiguration.saveExec = 0;

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setEnableClockEvent(guiEvent& event)
{
   clockTextCtrl->Enable(event.GetBoolValue());
}

void Main::eventEnableClock(bool state)
{
   guiEvent event(GUI_MSG, ENABLE_CLOCK);
   event.SetEventObject( p_Main );

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
         XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playDarkGreenBitmap);
         XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
         XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
         XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
      break;
        
      case HW_TAPE_STATE_FF:
         XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
         XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardDarkGreenBitmap);
         XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
         XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
      break;
        
      case HW_TAPE_STATE_RW:
         XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
         XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
         XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindDarkGreenBitmap);
         XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
      break;

      case HW_TAPE_STATE_REC:
         XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
         XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
         XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
         XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOnBitmap);
      break;
        
      default:
         hwTapeState_ = HW_TAPE_STATE_OFF;
         XRCCTRL(*this, "CasLoadXml", wxBitmapButton)->SetBitmapLabel(playBlackBitmap);
         XRCCTRL(*this, "CasForwardXml", wxBitmapButton)->SetBitmapLabel(forwardBlackBitmap);
         XRCCTRL(*this, "CasRewindXml", wxBitmapButton)->SetBitmapLabel(rewindBlackBitmap);
         XRCCTRL(*this, "CasSaveXml", wxBitmapButton)->SetBitmapLabel(recOffBitmap);
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
        popupDialog_->setStartLocation(computerConfiguration.memAccessConfiguration.saveStartString);
    if (!mode_.gui)
        return;

    XRCCTRL(*this,"SaveStartXml", wxTextCtrl)->SetValue(computerConfiguration.memAccessConfiguration.saveStartString);
}

void Main::eventSaveStart(Word saveStart)
{
    guiEvent event(GUI_MSG, SET_SAVE_START);
    event.SetEventObject( p_Main );

    computerConfiguration.memAccessConfiguration.saveStart = saveStart;
    computerConfiguration.memAccessConfiguration.saveStartString.Printf("%04X",saveStart);

    GetEventHandler()->AddPendingEvent(event);
}

void Main::setSaveEndEvent(guiEvent&WXUNUSED(event))
{
    if (popupDialog_ != NULL)
        popupDialog_->setEndLocation(computerConfiguration.memAccessConfiguration.saveEndString);
    if (!mode_.gui)
        return;

    XRCCTRL(*this,"SaveEndXml", wxTextCtrl)->SetValue(computerConfiguration.memAccessConfiguration.saveEndString);
}

void Main::eventSaveEnd(Word saveEnd)
{
    guiEvent event(GUI_MSG, SET_SAVE_END);
    event.SetEventObject( p_Main );

    computerConfiguration.memAccessConfiguration.saveEnd = saveEnd;
    computerConfiguration.memAccessConfiguration.saveEndString.Printf("%04X",saveEnd);

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

    XRCCTRL(*this,"BatchConvertButtonXml", wxButton)->Enable(convertState);
    XRCCTRL(*this,"BatchButtonXml", wxButton)->Enable(convertState);
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
    if (computerRunning_)
    {
#if defined(__linux__)
       guiRedrawBarTimeOutPointer->Start(200, wxTIMER_ONE_SHOT);
#else
       p_Video[videoNumber]->reDrawBar();
#endif
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
    if (selectedTab_ == MEMORYTAB)
    {
         updateMemory_ = true;

         int x, y;

         x = (address-memoryStart_) & 0xf;
         y = ((address-memoryStart_)  >> 4) & 0xf;

         rowChanged_[y] = true;
         memoryChanged_[x][y] = true;
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
    XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(XMLTAB);
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
    XRCCTRL(*this, "TextStartElf2K", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartElf2K", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndElf2K", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndElf2K", wxStaticText)->Enable(false);

    XRCCTRL(*this, "TextStartVip", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextStartVip", wxStaticText)->Enable(false);
    XRCCTRL(*this, "TextEndVip", wxStaticText)->Enable(true);
    XRCCTRL(*this, "TextEndVip", wxStaticText)->Enable(false);

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

void Main::setUpdateLedStatus(guiEvent& event)
{
   bool status = event.GetBoolValue();
   int card = event.GetByteValue1();
   int i = event.GetByteValue2();
   
   if (computerRunning_)
      p_Video[computerConfiguration.vis1870Configuration.videoNumber]->updateStatusLed(status, card, i);
}

void Main::eventUpdateLedStatus(bool status, int card, int i)
{
   guiEvent event(GUI_MSG, SET_STATUS_BAR_LED);
   event.SetEventObject( p_Main );

   event.SetBoolValue(status);
   event.SetByteValue1(card);
   event.SetByteValue2(i);

   GetEventHandler()->AddPendingEvent(event);
}

void Main::setUpdateVipIILedStatus(guiEvent& event)
{
   int led = event.GetByteValue1();
   bool status = event.GetBoolValue();
   
   p_Computer->updateStatusBarLedStatus(status, led);
}

void Main::eventUpdateVipIILedStatus(int led, bool status)
{
   guiEvent event(GUI_MSG, SET_VIPIILED);
   event.SetEventObject( p_Main );

   event.SetBoolValue(status);
   event.SetByteValue1(led);

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

bool Main::loadKeyDefinition(wxString findFile1, wxString findFile2, int keyDefA1[], int keyDefB1[], int keyDefA2[], bool * simDefA2, int keyDefB2[], bool * simDefB2, int * inButton1, int * inButton2, int gameKeysA[], int gameKeysB[])
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
 
    if (keyDefinitionFile.Open(dataDir_ + computerConfiguration.keyPadDefinitionFile))
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
                        p_Main->loadKeyDefinition("", keyValues.AfterFirst(':'), keyDefA1, keyDefB1, keyDefA2, simDefA2, keyDefB2, simDefB2, inButton1, inButton2, gameKeysA, gameKeysB);
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
    return (int)configPointer->Read(computerStr+"/InButton2", -1l);
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

   if (computerStr == "StudioII" || computerStr == "CoinArcade" || computerStr == "Conic")
   {
      keysFound = loadKeyDefinition("", "studiodefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
   }
   else if (computerStr == "Visicom")
   {
      keysFound = loadKeyDefinition("", "studiodefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
   }
   else if (computerStr == "Vip" || computerStr == "Vip2K")
   {
      if (computerConfiguration.keyLatchConfiguration[2].defined)
         keysFound = loadKeyDefinition("", "vipdefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
      else
         keysFound = loadKeyDefinition("", "vipiidefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
   }
   else if (computerStr == "Visicom")
   {
      keysFound = loadKeyDefinition("", "studioivdefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
   }
   else if (computerStr == "Elf" || computerStr == "CosmacElf" || computerStr == "NetronicsElfII" || computerStr == "QuestSuperElf" || computerStr == "Elf2K" || computerStr == "Cosmicos" || computerStr == "PicoElfV2")
   {
      keysFound = loadKeyDefinition("", "elfdefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
   }
   else if (computerStr == "UC1800")
   {
      keysFound = loadKeyDefinition("", "uc1800default", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
   }
   else if (computerStr == "FRED1" || computerStr == "FRED1_5")
   {
      keysFound = loadKeyDefinition("", "freddefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
   }
   else if (computerStr == "VipII" || computerStr == "Velf" || computerStr == "JVIP")
   {
      keysFound = loadKeyDefinition("", "vipiidefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
   }
   else
   {
      keysFound = loadKeyDefinition("", "vipiidefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_);
   }

   if (keysFound)
   {
      if (computerStr == "StudioII" || computerStr == "CoinArcade" || computerStr == "Conic" || computerStr == "Visicom")
      {
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
      }
      else if (computerStr == "FRED1" || computerStr == "FRED1_5" || computerStr == "Vip" || computerStr == "Vip2K" || computerStr == "Elf" || computerStr == "CosmacElf" || computerStr == "NetronicsElfII" || computerStr == "QuestSuperElf" || computerStr == "Elf2K" || computerStr == "Cosmicos" || computerStr == "PicoElfV2" || computerStr == "Eti" || computerStr == "HEC1802" || computerStr == "HUG1802" || computerStr == "UC1800" || computerStr == "TMC1800" || computerStr == "TMC2000" || computerStr == "Nano" || computerStr == "VipII" || computerStr == "Velf" || computerStr == "JVIP")
      {
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
            
            if (computerConfiguration.keyLatchConfiguration[2].defined)
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

// TO BE FIXED - remove TMC600 keymap?
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


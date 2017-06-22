
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
#include "wx/fs_inet.h"
#include "wx/filesys.h"
#include "wx/url.h"
#include "wx/dir.h"
#include "wx/fs_zip.h"
#include "wx/sysopt.h" 
#include "wx/stdpaths.h"
#include "wx/platinfo.h"
#include "wx/tglbtn.h"
#include "wx/fileconf.h"
#include "wx/cmdline.h"
#include "wx/sstream.h"

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
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

//For memory leak debugging, uncomment next line
//#include <vld.h>

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
//		return;
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
//			wxNotificationMessage notify("test", "error", this); 
//			notify.SetFlags( wxICON_ERROR ); 
//			notify.Show(); 
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

DEFINE_EVENT_TYPE(OPEN_COMX_PRINTER_WINDOW)
DEFINE_EVENT_TYPE(OPEN_PRINTER_WINDOW)
DEFINE_EVENT_TYPE(wxEVT_ERROR_MSG)
DEFINE_EVENT_TYPE(KILL_COMPUTER)
//DEFINE_EVENT_TYPE(GUI_MSG) // This is for wx 2.8.12 but doesn't actually work

BEGIN_EVENT_TABLE(Main, DebugWindow)

	EVT_CLOSE(Main::onClose)

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
    EVT_MENU(XRCID("MI_FixedWindowPosition"), Main::onFixedWindowPosition)
    EVT_MENU(XRCID("MI_NumPad"), Main::onUseNumPad)
	EVT_MENU(XRCID("MI_FunctionKeys"), Main::onFunctionKeys)
	EVT_MENU(XRCID("MI_ActivateMain"), Main::onActivateMainWindow)
	EVT_MENU(XRCID("MI_FullScreen"), Main::onFullScreenMenu)
	EVT_MENU(XRCID(GUIDEFAULT), Main::onDefaultSettings)
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

	EVT_CHOICEBOOK_PAGE_CHANGED(XRCID("StudioChoiceBook"), Main::onStudioChoiceBook)
	EVT_CHOICEBOOK_PAGE_CHANGED(XRCID("TelmacChoiceBook"), Main::onTelmacChoiceBook)
	EVT_CHOICEBOOK_PAGE_CHANGED(XRCID("ElfChoiceBook"), Main::onElfChoiceBook)
	EVT_CHOICEBOOK_PAGE_CHANGED(XRCID("RcaChoiceBook"), Main::onRcaChoiceBook)
	EVT_CHOICEBOOK_PAGE_CHANGED(XRCID("DebuggerChoiceBook"), Main::onDebuggerChoiceBook)
	EVT_TIMER(902, Main::vuTimeout)
	EVT_TIMER(903, Main::cpuTimeout)
	EVT_TIMER(904, Main::ledTimeout)
	EVT_TIMER(905, Main::updateCheckTimeout)
    EVT_TIMER(906, Main::traceTimeout)

	EVT_KEY_DOWN(Main::onKeyDown)
	EVT_KEY_UP(Main::onKeyUp)
	EVT_MOUSEWHEEL(Main::onWheel)

	EVT_ERROR_MSG(wxID_ANY, Main::errorMessageEvent)

	EVT_GUI_MSG(SET_LOCATION, Main::setLocationEvent)
	EVT_GUI_MSG(SET_SW_NAME, Main::setSwNameEvent)
	EVT_GUI_MSG(SET_TAPE_STATE, Main::setTapeStateEvent)
	EVT_GUI_MSG(SET_TEXT_VALUE, Main::setTextValueEvent)
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

	EVT_COMMAND(wxID_ANY, KILL_COMPUTER, Main::killComputer)

END_EVENT_TABLE()

#if defined (__linux__)
IMPLEMENT_APP_NO_MAIN(Emu1802)
int main(int argc, char *argv[])
{
    if (!XInitThreads())
        return false;

    wxDISABLE_DEBUG_SUPPORT();
    return wxEntry(argc, argv);

//  code as suggested initially by Mark but replaced by two lines above as on Elf2K exit with assert
//  wxEntryStart( argc, argv );
//  wxTheApp->OnInit();
//  wxTheApp->OnRun();
//  wxTheApp->OnExit();
//  wxEntryCleanup();
//  return 0;
}
#else
IMPLEMENT_APP(Emu1802)
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

WindowInfo getWinSizeInfo()
{
	WindowInfo returnValue;

#if defined (__linux__)
	int major, minor;
	wxString var, value;

	wxGetOsVersion(&major, &minor);

	wxLinuxDistributionInfo distInfo;

	distInfo = wxPlatformInfo::Get().GetLinuxDistributionInfo();

	if (distInfo.Id == "Ubuntu")
	{
		if (major > 2)
		{	// Ubuntu 11.10
			returnValue.mainwY = 460;
			var = "UBUNTU_MENUPROXY";
			if (wxGetEnv(var , &value))
			{
				if (value == "libappmenu.so")
					returnValue.mainwY = 454;
			}
			returnValue.xBorder = 2;
			returnValue.yBorder = 30;
			returnValue.xBorder2 = 1;
			returnValue.yBorder2 = 60;
			returnValue.mainwX = 541;
			returnValue.xPrint = 2;
			returnValue.RegularClockY = 375;
			returnValue.RegularClockX = 333;
			returnValue.ChoiceClockY = 344;
			returnValue.ChoiceClockX = 334;
			returnValue.operatingSystem = OS_LINUX_UBUNTU_11_10;
		}
		else
		{	// Ubuntu 11.04
			returnValue.xBorder = 0;	
			returnValue.yBorder = 0;	
			returnValue.xBorder2 = 0;	
			returnValue.yBorder2 = 30;
			returnValue.mainwY = 500;
			returnValue.mainwX = 545;
			returnValue.xPrint = 2;
			returnValue.RegularClockY = 375;
			returnValue.RegularClockX = 333;
			returnValue.ChoiceClockY = 344;
			returnValue.ChoiceClockX = 334;
			returnValue.operatingSystem = OS_LINUX_UBUNTU_11_04;
		}
	}
	else
	{
		returnValue.xBorder = 2;
		returnValue.yBorder = 30;
		returnValue.xBorder2 = 1;
		returnValue.yBorder2 = 60;
		returnValue.mainwX = 590;
		returnValue.mainwY = 640;
		returnValue.xPrint = 2;
		returnValue.RegularClockY = 455;
		returnValue.RegularClockX = 333;
		returnValue.ChoiceClockY = 414;
		returnValue.ChoiceClockX = 334;
		returnValue.operatingSystem = OS_LINUX_FEDORA;

		// GUI changes:
		// <size>172,-1</size> to <size>172,25</size>
		// <size>146,-1</size> to <size>146,25</size>
		// <size>73,-1</size> to <size>73,25</size>
		wxString desktop = wxPlatformInfo::Get().GetDesktopEnvironment();
		if (desktop == "KDE")
		{ // openSUSE KDE
			returnValue.xBorder = 6;
			returnValue.yBorder = 27;
			returnValue.xBorder2 = 6;
			returnValue.yBorder2 = 54;
			returnValue.mainwY = 470;
			returnValue.mainwX = 551;
			returnValue.xPrint = 21;
			returnValue.RegularClockY = 383;
			returnValue.RegularClockX = 333;
			returnValue.ChoiceClockY = 354;
			returnValue.ChoiceClockX = 334;
			returnValue.operatingSystem = OS_LINUX_OPENSUSE_KDE;
		}
		if (desktop == "GNOME")
		{ // openSUSE GNOME
			returnValue.xBorder = 0;
			returnValue.yBorder = 0;
			returnValue.xBorder2 = 2;
			returnValue.yBorder2 = 36;
			returnValue.mainwY = 510;
			returnValue.mainwX = 545;
			returnValue.xPrint = 20;
			returnValue.RegularClockY = 365;
			returnValue.RegularClockX = 333;
			returnValue.ChoiceClockY = 334;
			returnValue.ChoiceClockX = 334;
			returnValue.operatingSystem = OS_LINUX_OPENSUSE_GNOME;
		}
	}
#endif

#if defined (__WXMAC__)
	int major, minor;
	wxOperatingSystemId operatingSystemId;
	operatingSystemId = wxGetOsVersion(&major, &minor);

	returnValue.xBorder = 1;
	returnValue.yBorder = 1;
	returnValue.xBorder2 = 1;
	returnValue.yBorder2 = 24;
	returnValue.mainwX = 572;
    returnValue.mainwY = 480;
	returnValue.xPrint = 19;
	returnValue.RegularClockY = 375;
	returnValue.RegularClockX = 333;
	returnValue.ChoiceClockY = 351;
	returnValue.ChoiceClockX = 334;
	returnValue.operatingSystem = OS_MAC;
#endif

#if defined (__WXMSW__)
	RTL_OSVERSIONINFOEXW osVersion;
	GetOsVersion(&osVersion);

	switch (osVersion.dwMajorVersion)
	{
		case OS_MAJOR_XP_2000:
			switch (osVersion.dwMinorVersion)
			{
				case OS_MINOR_2000:
					returnValue.xBorder2 = 8;
					returnValue.yBorder2 = 36;
					returnValue.mainwY = 489;
					returnValue.operatingSystem = OS_WINDOWS_2000;
				break;

				case OS_MINOR_XP:
					returnValue.xBorder2 = 8;
					returnValue.yBorder2 = 36;
					returnValue.mainwY = 489;
					returnValue.operatingSystem = OS_WINDOWS_XP;
				break;
                    
                default:
                    returnValue.xBorder2 = 8;
                    returnValue.yBorder2 = 36;
                    returnValue.mainwY = 489;
                    returnValue.operatingSystem = OS_WINDOWS_2000;
                break;
			}
			returnValue.xBorder = 0;
			returnValue.yBorder = 0;
		break;

		case OS_MAJOR_VISTA_8_1:
			switch (osVersion.dwMinorVersion)
			{
				case OS_MINOR_VISTA:
					returnValue.xBorder2 = 16;
					returnValue.yBorder2 = 36;
					returnValue.mainwY = 491;
					returnValue.operatingSystem = OS_WINDOWS_VISTA;
				break;

				case OS_MINOR_7:
					returnValue.xBorder2 = 16;
					returnValue.yBorder2 = 36;
					returnValue.mainwY = 495;
					returnValue.operatingSystem = OS_WINDOWS_7;
				break;

				case OS_MINOR_8:
				case OS_MINOR_8_1:
					returnValue.xBorder2 = 16;
					returnValue.yBorder2 = 36;
					returnValue.mainwY = 495;
					returnValue.operatingSystem = OS_WINDOWS_8;
				break;

   				default:
                    returnValue.xBorder2 = 16;
                    returnValue.yBorder2 = 36;
                    returnValue.mainwY = 491;
                    returnValue.operatingSystem = OS_WINDOWS_VISTA;
                break;
            }
			returnValue.xBorder = 0;
			returnValue.yBorder = 0;
		break;

		case OS_MAJOR_10:
			returnValue.xBorder2 = 2;
			returnValue.yBorder2 = 9;
			returnValue.mainwY = 494;
			returnValue.operatingSystem = OS_WINDOWS_10;
			returnValue.xBorder = -14;
			returnValue.yBorder = -7;
        break;

		default:
            returnValue.xBorder2 = 2;
            returnValue.yBorder2 = 9;
            returnValue.mainwY = 494;
            returnValue.operatingSystem = OS_WINDOWS_10;
            returnValue.xBorder = -14;
            returnValue.yBorder = -7;
		break;
	}
	returnValue.RegularClockY = 370;
	returnValue.RegularClockX = 333;
	returnValue.ChoiceClockY = 343;
	returnValue.ChoiceClockX = 333;
	returnValue.xPrint = 21;
	returnValue.mainwX = 560;
#endif

	return returnValue;
}

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
    
    int ubuntuOffsetX;

	wxInitAllImageHandlers();
    wxFileSystem::AddHandler(new wxZipFSHandler);
	wxXmlResource::Get()->InitAllHandlers();

 	configPointer = wxConfigBase::Get();

	wxString xrcFile;
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	wxString workingDir_ = wxGetCwd();
	wxFileName applicationFile = wxFileName(workingDir_, "", wxPATH_NATIVE);
	wxString pathSeparator_ = applicationFile.GetPathSeparator(wxPATH_NATIVE);
    wxString applicationDirectory_;
    applicationDirectory_ = applicationFile.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    ubuntuOffsetX = 36;
#elif (__WXMAC__)
	wxString workingDir_;
	if (mode_.portable)
		workingDir_ = wxGetCwd();
	else
		workingDir_ = wxStandardPaths::Get().GetDataDir();
	wxFileName applicationFile = wxFileName(workingDir_, "", wxPATH_NATIVE);
	wxString pathSeparator_ = applicationFile.GetPathSeparator(wxPATH_NATIVE);
	wxString applicationDirectory_;
	if (mode_.portable)
		applicationDirectory_ = applicationFile.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	else
        applicationDirectory_ = wxStandardPaths::Get().GetResourcesDir() + pathSeparator_;
    ubuntuOffsetX = 30;
#else
	wxString workingDir_ = wxGetCwd();
	wxFileName applicationFile = wxFileName(workingDir_, "", wxPATH_NATIVE);
	wxString pathSeparator_ = applicationFile.GetPathSeparator(wxPATH_NATIVE);
	wxString applicationDirectory_ = applicationFile.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
	ubuntuOffsetX = 0;
#endif

	xrcFile = applicationDirectory_ + "main.xrc";
//	checkXrc(xrcFile);
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

	int mainWindowX = (int)configPointer->Read("/Main/Window_Position_X", 30 + ubuntuOffsetX);
	int mainWindowY = (int)configPointer->Read("/Main/Window_Position_Y", 30);

	WindowInfo windowInfo = getWinSizeInfo();

	p_Main = new Main("Emma 02", wxPoint(mainWindowX, mainWindowY), wxSize(windowInfo.mainwX, windowInfo.mainwY), mode_, dataDir_, regPointer);
//	p_Main = new Main("Emma 02", wxPoint(mainWindowX, mainWindowY), wxSize(-1, -1), mode_, dataDir_, regPointer);

	configPointer->Write("/Main/Version", EMMA_VERSION);

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
	delete regPointer;
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
	{ wxCMD_LINE_SWITCH, "v", "verbose", "verbose output"},											// only valid in combination with -c
	{ wxCMD_LINE_SWITCH, "f", "fullscreen", "full screen mode"},									// only valid in combination with -c
	{ wxCMD_LINE_SWITCH, "u", "skipupdate", "skip update check"},									
	{ wxCMD_LINE_SWITCH, "w", "window", "non fixed window positions"},									
    { wxCMD_LINE_OPTION, "c", "computer", "start emulator without gui and for specified computer" },// Switch off GUI
	{ wxCMD_LINE_OPTION, "s", "software", "load specified software on start" },						// only valid in combination with -c
	{ wxCMD_LINE_OPTION, "r", "run", "run specified software on start" },							// only valid in combination with -c
	{ wxCMD_LINE_OPTION, "ch", "chip8", "load specified chip8 software on start" },					// only valid in combination with -c

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
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
	wxString appName = "emma_02";
#else
	wxString appName = "Emma 02";
#endif
	SetAppName(appName);

	regPointer = wxConfigBase::Get();
	regPointer->SetRecordDefaults();

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	wxString workingDir_ = wxGetCwd();
	wxFileName applicationFile = wxFileName(workingDir_, "", wxPATH_NATIVE);
	wxString pathSeparator_ = applicationFile.GetPathSeparator(wxPATH_NATIVE);
	dataDir_ = regPointer->Read("/DataDir", "data" + pathSeparator_);
	if (!wxFileName::IsDirWritable(workingDir_ + pathSeparator_ + dataDir_))
		dataDir_ = regPointer->Read("/DataDir", wxStandardPaths::Get().GetDocumentsDir() + pathSeparator_ + "Documents" + pathSeparator_ + "emma_02" + pathSeparator_);
    if (!wxDir::Exists(dataDir_))
        wxDir::Make(dataDir_);
    wxString applicationDirectory_;
    applicationDirectory_ = applicationFile.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
#elif (__WXMAC__)
    wxString workingDir_;
    if (mode_.portable)
        workingDir_ = wxGetCwd();
    else
        workingDir_ = wxStandardPaths::Get().GetDataDir();
    wxFileName applicationFile = wxFileName(workingDir_, "", wxPATH_NATIVE);
    wxString pathSeparator_ = applicationFile.GetPathSeparator(wxPATH_NATIVE);
    dataDir_ = regPointer->Read("/DataDir", wxStandardPaths::Get().GetDocumentsDir() + pathSeparator_ + "emma_02" + pathSeparator_);
    if (!wxDir::Exists(dataDir_))
        wxDir::Make(dataDir_);
    wxString applicationDirectory_;
    if (mode_.portable)
        applicationDirectory_ = applicationFile.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
    else
        applicationDirectory_ = wxStandardPaths::Get().GetResourcesDir() + pathSeparator_;
#else
	wxString workingDir_ = wxGetCwd();
	wxFileName applicationFile = wxFileName(workingDir_, "", wxPATH_NATIVE);
	wxString pathSeparator_ = applicationFile.GetPathSeparator(wxPATH_NATIVE);
	dataDir_ = regPointer->Read("/DataDir", wxStandardPaths::Get().GetUserDataDir() + pathSeparator_);
	wxString applicationDirectory_ = applicationFile.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);
#endif

	regPointer->Read("/DataDirRelative", &dataDirRelative_, false);

	if (dataDirRelative_)
	{
		if (dataDir_.Right(1) == pathSeparator_)
			dataDir_ = applicationDirectory_ + dataDir_;
		else
			dataDir_ = applicationDirectory_ + dataDir_ + pathSeparator_;
	}

	if (dataDir_.Find(applicationDirectory_) != wxNOT_FOUND)
	{
		regPointer->Write("/DataDirRelative", true);

		wxString tempDir = dataDir_;
		tempDir.Replace(applicationDirectory_, "");
		regPointer->Write("/DataDir", tempDir);
	}
	else
	{
		regPointer->Write("/DataDirRelative", false);
		regPointer->Write("/DataDir", dataDir_);
	}

	if (mode_.portable)
	{
		dataDir_ = applicationDirectory_ + "data" + pathSeparator_;
#if wxCHECK_VERSION(2, 9, 0)
		if (!wxDir::Exists(dataDir_))
			wxDir::Make(dataDir_);
#endif
	}

	wxFileConfig *pConfig = new wxFileConfig(appName, "Marcel van Tongeren", dataDir_ + "emma_02.ini");
    wxConfigBase::Set(pConfig);
	configPointer= wxConfigBase::Get();

	if (mode_.portable)
	{
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
						wxMessageOutput::Get()->Printf("Option -r is not supported on ETI 660 emulator");
						return false;
					}
					return true;
				}
				wxMessageOutput::Get()->Printf("Incorrect computer name specified");
				return false;
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
				if (computer == "Studio")
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

/* started some conversion routine but first want to check GUI layout when using GTK3
void checkXrc(wxString xrcFile)
{
	wxTextFile xrc;

#if defined(__linux__)
	wxString os = "<!-- Linux -->";
	wxString replaceString = "8";
#else
	wxString os = "<!-- Windows -->";
	wxString replaceString = "-1";
#endif

	wxString line;

	if (xrc.Open(xrcFile))
	{
		line = xrc.GetFirstLine();

		if (line != os)
		{
			xrc.InsertLine(os, 0);

			for (line = xrc.GetFirstLine(); !xrc.Eof(); line = xrc.GetNextLine())
			{
				if (line.Find("<sysfont>wxSYS_DEFAULT_GUI_FONT</sysfont>"))
				{

				}
			}
			xrc.Write();
		}
		xrc.Close();
	}
}*/

Main::Main(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxConfigBase *regP)
: DebugWindow(title, pos, size, mode, dataDir)
{
	selectedComputer_ = COMX;
	computerRunning_ = false;
	runningComputer_ = NO_COMPUTER;

	wxFileSystem::AddHandler(new wxInternetFSHandler);
	regPointer = regP;

	popupDialog_ = NULL;
	emmaClosing_ = false;
	windowInfo = getWinSizeInfo();
	xmlLoaded_ = false;
    configurationMenuOn_ = false;
    
#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	if (mode_.gui)
	{
        SetMenuBar(wxXmlResource::Get()->LoadMenuBar("Main_Menu"));
		wxXmlResource::Get()->LoadPanel(this, "Main_GUI");
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

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__) || defined(__WXMAC__)
	wxString helpFile = applicationDirectory_ + "emma_02.htb";
#else
	wxString helpFile = applicationDirectory_ + "Emma 02.htb";
#endif

    help_ = new wxHtmlHelpController(wxHF_TOOLBAR | wxHF_CONTENTS | wxHF_INDEX | wxHF_SEARCH | wxHF_BOOKMARKS | wxHF_PRINT | wxHF_BOOKMARKS);
	
 	if (!help_->AddBook(helpFile))
	{
		(void)wxMessageBox( "Failed adding book " + helpFile + "\n",
							"Emma 02", wxICON_ERROR | wxOK );
	}

	if (mode_.gui)
	{
		this->Connect(XRCID(GUICOMPUTERNOTEBOOK), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler(Main::onComputer) );
		wxFont smallFont(6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetFont(smallFont);
#if defined(__WXMAC__)
		wxFont defaultFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
/*		XRCCTRL(*this, "VuComx", wxStaticBitmap)->Hide();
        XRCCTRL(*this, "VuVip", wxStaticBitmap)->Hide();
        XRCCTRL(*this, "VuVipII", wxStaticBitmap)->Hide();
		XRCCTRL(*this, "VuCosmicos", wxStaticBitmap)->Hide();
		XRCCTRL(*this, "VuElf", wxStaticBitmap)->Hide();
		XRCCTRL(*this, "VuElfII", wxStaticBitmap)->Hide();
		XRCCTRL(*this, "VuSuperElf", wxStaticBitmap)->Hide();
		XRCCTRL(*this, "VuTmc600", wxStaticBitmap)->Hide();
		XRCCTRL(*this, "VuTMC2000", wxStaticBitmap)->Hide();
		XRCCTRL(*this, "VuTMC1800", wxStaticBitmap)->Hide();
		XRCCTRL(*this, "VuNano", wxStaticBitmap)->Hide();
		XRCCTRL(*this, "VuPecom", wxStaticBitmap)->Hide();
		XRCCTRL(*this, "VuEti", wxStaticBitmap)->Hide();*/
#else
        wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#endif
		XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetFont(defaultFont);

		wxString text;
		pcTextPointer = XRCCTRL(*this,"Chip8PC", wxTextCtrl);
		iTextPointer = XRCCTRL(*this,"Chip8I", wxTextCtrl);
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
		dTextPointer = XRCCTRL(*this,"D", wxTextCtrl);
		pTextPointer = XRCCTRL(*this,"P", wxTextCtrl);
		xTextPointer = XRCCTRL(*this,"X", wxTextCtrl);
		tTextPointer = XRCCTRL(*this,"T", wxTextCtrl);

//		assemblerWindowPointer = XRCCTRL(*this,"AssemblerWindow", wxTextCtrl);
//		disassemblerWindowPointer = XRCCTRL(*this,"DisassemblerWindow", wxTextCtrl);
        traceWindowPointer = XRCCTRL(*this,"TraceWindow", wxTextCtrl);
//		inputWindowPointer = XRCCTRL(*this,"InputWindow", wxTextCtrl);
		assInputWindowPointer = XRCCTRL(*this,"AssInputWindow", wxTextCtrl);
		assErrorWindowPointer = XRCCTRL(*this,"AssErrorMultiLine", wxTextCtrl);
		breakPointWindowPointer = XRCCTRL(*this,"BreakPointWindow", wxListCtrl);
		chip8BreakPointWindowPointer = XRCCTRL(*this,"Chip8BreakPointWindow", wxListCtrl);
		tregWindowPointer = XRCCTRL(*this,"TregWindow", wxListCtrl);
		trapWindowPointer = XRCCTRL(*this,"TrapWindow", wxListCtrl);
	}

	initConfig();
	readConfig();

	oldGauge_ = 1;
	vuPointer = new wxTimer(this, 902);
	cpuPointer = new wxTimer(this, 903);
	ledTimePointer = new wxTimer(this, 904);
    updateCheckPointer = new wxTimer(this, 905);
    traceTimeoutPointer = new wxTimer(this, 906);
    
    if (mode_.gui)
        buildConfigMenu();

	this->connectKeyEvent(this);

	wxSystemOptions::SetOption("msw.window.no-clip-children", 0);
    
	int lastYearWeek;
	configPointer->Read("/Main/YearWeek", &lastYearWeek ,0);

	wxDateTime now = wxDateTime::Now();
	int yearWeek = now.GetYear()*100 + now.GetWeekOfYear();

	if (mode_.update_check) 
	{
		if (lastYearWeek < yearWeek)
		{
			updateCheckPointer->Start(10000, wxTIMER_ONE_SHOT);
			configPointer->Write("/Main/YearWeek", yearWeek);
		}
	}
}

Main::~Main()
{
    if (mode_.gui)
	{
		if (selectedComputer_ < NO_COMPUTER && configurationMenuOn_ == true)
		{
			configurationMenu->Remove(GUI_CONFIG_MENU);
			configurationDeleteMenu->Remove(GUI_CONFIG_DELETE_MENU);
		}

		for (int computer = 2; computer<NO_COMPUTER; computer++)
		{
			delete conf[computer].configurationMenu;
			delete conf[computer].configurationDeleteMenu;
		}

		for (int computer=0; computer<NO_COMPUTER; computer++)
		{
			delete clockText[computer];
			delete clockTextCtrl[computer];
			delete mhzText[computer];
			delete startButton[computer];
		}
	}

	delete vuPointer;
	delete cpuPointer;
	delete updateCheckPointer;
    delete ledTimePointer;
    delete traceTimeoutPointer;
	delete help_;
	if (configPointer == NULL || !saveOnExit_)
		return;

	this->GetPosition(&mainWindowX_, &mainWindowY_);
	writeConfig();
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

	if (mode_.gui)
	{
		configPointer->Write("/Main/Selected_Tab", XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->GetSelection());
		configPointer->Write("/Main/Selected_Cosmac_Tab", XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->GetSelection());
		configPointer->Write("/Main/Selected_Rca_Tab", XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->GetSelection());
		configPointer->Write("/Main/Selected_Studio_Tab", XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->GetSelection());
		configPointer->Write("/Main/Selected_Telmac_Tab", XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->GetSelection());
		configPointer->Write("/Main/Selected_Debugger_Tab", XRCCTRL(*this, "DebuggerChoiceBook", wxChoicebook)->GetSelection());

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
			regPointer->Write("/DataDirRelative", true);

			wxString tempDir = dataDir_;
			tempDir.Replace(applicationDirectory_, "");
			regPointer->Write("/DataDir", tempDir);
		}
		else
		{
			regPointer->Write("/DataDirRelative", false);
			regPointer->Write("/DataDir", dataDir_);
		}
	}
	configPointer->Write("/Main/Save_Debug_File", saveDebugFile_);
    configPointer->Write("/Main/Save_On_Exit", saveOnExit_);
	configPointer->Write("/Main/Check_For_Update", checkForUpdate_);
    configPointer->Write("/Main/Floating_Point_Zoom", fullScreenFloat_);
    configPointer->Write("/Main/Use_Num_Pad", useNumPad_);
	configPointer->Write("/Main/Use_Exit_Key", useExitKey_);
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
	configPointer->Write("/Main/Psave_Volume", psaveData_[0]);
	configPointer->Write("/Main/Psave_Bit_Rate", psaveData_[1]);
	configPointer->Write("/Main/Psave_Bits_Per_Sample", psaveData_[2]);
	configPointer->Write("/Main/Wav_Conversion_Type", psaveData_[3]);
	configPointer->Write("/Main/Cassette_Input_Channel", psaveData_[4]);
	configPointer->Write("/Main/Cassette_Playback_Input", psaveData_[5]);
	configPointer->Write("/Main/Cassette_Reversed_Polarity", psaveData_[6]);
	configPointer->Write("/Main/Cassette_Conversion_Type", psaveData_[7]);
	configPointer->Write("/Main/Window_Positions_Fixed", mode_.window_position_fixed);

    writeDebugConfig();

    writeComxDirConfig();
    writeElf2KDirConfig();
	writeMS2000DirConfig();
	writeMcdsDirConfig();
	writeCosmicosDirConfig();
    writeVipDirConfig();
    writeVipIIDirConfig();
    writeVelfDirConfig();
	writeElfDirConfig(ELF, "Elf");
	writeElfDirConfig(ELFII, "ElfII");
	writeElfDirConfig(SUPERELF, "SuperElf");
    writeMembershipDirConfig();
    writeMicrotutorDirConfig();
    writeStudioDirConfig();
    writeVisicomDirConfig();
    writeVictoryDirConfig();
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
    writeVelfConfig();
	writeElfConfig(ELF, "Elf");
	writeElfConfig(ELFII, "ElfII");
	writeElfConfig(SUPERELF, "SuperElf");
    writeMembershipConfig();
    writeMicrotutorConfig();
    writeStudioConfig();
	writeVisicomConfig();
	writeVictoryConfig();
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
   	writeVelfWindowConfig();
	writeElfWindowConfig(ELF, "Elf");
	writeElfWindowConfig(ELFII, "ElfII");
	writeElfWindowConfig(SUPERELF, "SuperElf");
    writeMembershipWindowConfig();
    writeMicrotutorWindowConfig();
    writeStudioWindowConfig();
    writeVisicomWindowConfig();
    writeVictoryWindowConfig();
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
	Byte brightness[8] = { 0, 28, 77, 105, 150, 194, 227, 0xff };

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
	ubuntuOffsetX_ = 36;
#elif (__WXMAC__)
	ubuntuOffsetX_ = 30;
#else
	ubuntuOffsetX_ = 0;
#endif

	mainWindowX_ = (int)configPointer->Read("/Main/Window_Position_X", 30 + ubuntuOffsetX_);
	mainWindowY_ = (int)configPointer->Read("/Main/Window_Position_Y", 30);

	percentageClock_ = 1;

	wxString colour[66];
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
	colour[64] = "#00ff00";	// foreground 80 Column
	colour[65] = "#004000";	// background 80 Column

	borderX[VIDEO1870] = 0;
	borderY[VIDEO1870] = 0; // CDP 1870
	borderX[VIDEO80COL] = 0;
	borderY[VIDEO80COL] = 0;  // 80 Column

	setScreenInfo(COMX, 0, 66, colour, 3, borderX, borderY);
	setComputerInfo(COMX, "Comx", "COMX-35", "comx");

	setScreenInfo(CIDELSA, 56, 64, colour, 1, borderX, borderY);
	setComputerInfo(CIDELSA, "Cidelsa", "Cidelsa", "");

	setScreenInfo(TMC600, 56, 64, colour, 1, borderX, borderY);
	setComputerInfo(TMC600, "Tmc600", "Telmac 600", "tmc600");

	setScreenInfo(PECOM, 56, 64, colour, 1, borderX, borderY);
	setComputerInfo(PECOM, "Pecom", "Pecom 64", "pecom");

	borderX[VIDEOVT] = 0;
	borderY[VIDEOVT] = 0;  //Video Terminal
	borderX[VIDEOPIXIE] = 11;
	borderY[VIDEOPIXIE] = 33;  //Pixie
	borderX[VIDEO2KI8275] = 0;
	borderY[VIDEO2KI8275] = 0;  //i8275

	colour[0] = "#ffffff";	// foreground pixie
	colour[1] = "#000000";	// background pixie
	colour[2] = "#00ff00";	// foreground i8275
	colour[3] = "#004000";	// background i8275
	colour[4] = "#00ff00";	// highlight i8275
    setScreenInfo(ELF2K, 0, 5, colour, 3, borderX, borderY);
    setComputerInfo(ELF2K, "Elf2K", "Elf 2000", "");
    setScreenInfo(MS2000, 0, 5, colour, 3, borderX, borderY);
    setComputerInfo(MS2000, "MS2000", "MS2000", "");
	setScreenInfo(MCDS, 0, 5, colour, 3, borderX, borderY);
	setComputerInfo(MCDS, "Mcds", "MCDS", "rca");
	setScreenInfo(MEMBER, 0, 5, colour, 1, borderX, borderY);
	setComputerInfo(MEMBER, "Membership", "Membership Card", "");

	borderX[VIDEOPIXIE] = 8;
	borderY[VIDEOPIXIE] = 32;  //CDP1864

	setScreenInfo(COSMICOS, 0, 5, colour, 2, borderX, borderY);
	setComputerInfo(COSMICOS, "Cosmicos", "Cosmicos", "");

    setScreenInfo(MICROTUTOR, 0, 5, colour, 2, borderX, borderY);
    setComputerInfo(MICROTUTOR, "Microtutor", "RCA Microtutor", "");
    
    colour[5] = "#000000";	// background mc6847
	colour[6] = "#00ff00";	// text green
	colour[7] = "#ffc418";	// text orange
	colour[8] = "#00ff00";	// graphic Green
	colour[9] = "#ffff00";	// graphic Yellow 
	colour[10] = "#0000ff";	// graphic Blue
	colour[11] = "#ff0000";	// graphic Red
	colour[12] = "#ffffff";	// graphic Buff
	colour[13] = "#00ffff";	// graphic Cyan
	colour[14] = "#ff00ff";	// graphic Magenta
	colour[15] = "#ffc418";	// graphic Orange
	colour[16] = "#000000";
	colour[17] = "#000000";
	colour[18] = "#007f00";
	colour[19] = "#00ff00";
	colour[20] = "#00003f";
	colour[21] = "#0000ff";
	colour[22] = "#3f0000";
	colour[23] = "#007f7f";
	colour[24] = "#7f0000";
	colour[25] = "#ff0000";
	colour[26] = "#7f7f00";
	colour[27] = "#ffff00";
	colour[28] = "#003f00";
	colour[29] = "#7f007f";
	colour[30] = "#7f7f7f";
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

	setScreenInfo(STUDIO, 0, 2, colour, 2, borderX, borderY);
	setComputerInfo(STUDIO, "Studio2", "Studio II", "");

	setScreenInfo(TMC1800, 0, 2, colour, 2, borderX, borderY);
	setComputerInfo(TMC1800, "TMC1800", "Telmac 1800", "");

	setScreenInfo(VELF, 0, 5, colour, 2, borderX, borderY);
	setComputerInfo(VELF, "Velf", "VELF", "");

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
	colour[12] = "#00ff00";	// foreground vt
	colour[13] = "#004000";	// background vt
	colour[14] = "#00ff00";	// highlight vt
	setScreenInfo(VIP, 0, 15, colour, 2, borderX, borderY);
	setComputerInfo(VIP, "Vip", "Cosmac VIP", "");

	setScreenInfo(VIPII, 0, 12, colour, 2, borderX, borderY);
	setComputerInfo(VIPII, "VipII", "Cosmac VIP II", "fpb");

	borderX[VIDEOPIXIE] = 8;
	borderY[VIDEOPIXIE] = 32;  //CDP1864

	setScreenInfo(ETI, 0, 12, colour, 2, borderX, borderY);
	setComputerInfo(ETI, "Eti", "ETI 660", "");

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
	setComputerInfo(VICTORY, "Victory", "Victory MPT-02", "");

	wxFont smallFont(6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#if defined(__WXMAC__)
	wxFont defaultFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#else
	wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#endif
	if (mode_.gui)
	{
		for (int computer = 0; computer < NO_COMPUTER; computer++)
		{
			switch (computer)
			{
			case COMX:
			case PECOM:
			case CIDELSA:
			case ETI:
#if defined(__WXMAC__)
				clockText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "Clock:", wxPoint(windowInfo.RegularClockX, windowInfo.RegularClockY + 3));
				clockTextCtrl[computer] = new FloatEdit(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_CLOCK_TEXTCTRL + computer, "", wxPoint(windowInfo.RegularClockX + 35, windowInfo.RegularClockY), wxSize(45, 21));
				mhzText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "MHz", wxPoint(windowInfo.RegularClockX + 81, windowInfo.RegularClockY + 3));
				startButton[computer] = new wxButton(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_START_BUTTON + computer, "Start", wxPoint(windowInfo.RegularClockX + 109, windowInfo.RegularClockY - 6), wxSize(80, 25));
#endif
#if defined(__WXMSW__)
				clockText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "Clock:", wxPoint(windowInfo.RegularClockX, windowInfo.RegularClockY + 3));
				clockTextCtrl[computer] = new FloatEdit(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_CLOCK_TEXTCTRL + computer, "", wxPoint(windowInfo.RegularClockX + 32, windowInfo.RegularClockY), wxSize(45, 23));
				mhzText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "MHz", wxPoint(windowInfo.RegularClockX + 81, windowInfo.RegularClockY + 3));
				startButton[computer] = new wxButton(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_START_BUTTON + computer, "Start", wxPoint(windowInfo.RegularClockX + 109, windowInfo.RegularClockY - 1), wxSize(80, 25));
#endif
#if defined(__linux__)
				clockText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "Clock:", wxPoint(windowInfo.RegularClockX - 20, windowInfo.RegularClockY + 8));
				clockTextCtrl[computer] = new FloatEdit(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_CLOCK_TEXTCTRL + computer, "", wxPoint(windowInfo.RegularClockX + 22, windowInfo.RegularClockY), wxSize(55, -1));
				mhzText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "MHz", wxPoint(windowInfo.RegularClockX + 81, windowInfo.RegularClockY + 8));
				startButton[computer] = new wxButton(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_START_BUTTON + computer, "Start", wxPoint(windowInfo.RegularClockX + 114, windowInfo.RegularClockY - 1), wxSize(80, -1));
#endif
				break;

			default:
#if defined(__WXMAC__)
				clockText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "Clock:", wxPoint(windowInfo.ChoiceClockX, windowInfo.ChoiceClockY + 3));
				clockTextCtrl[computer] = new FloatEdit(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_CLOCK_TEXTCTRL + computer, "", wxPoint(windowInfo.ChoiceClockX + 35, windowInfo.ChoiceClockY), wxSize(45, 21));
				mhzText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "MHz", wxPoint(windowInfo.ChoiceClockX + 81, windowInfo.ChoiceClockY + 3));
				startButton[computer] = new wxButton(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_START_BUTTON + computer, "Start", wxPoint(windowInfo.ChoiceClockX + 109, windowInfo.ChoiceClockY - 6), wxSize(80, 25));
#endif
#if defined(__WXMSW__)
				clockText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "Clock:", wxPoint(windowInfo.ChoiceClockX, windowInfo.ChoiceClockY + 3));
				clockTextCtrl[computer] = new FloatEdit(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_CLOCK_TEXTCTRL + computer, "", wxPoint(windowInfo.ChoiceClockX + 32, windowInfo.ChoiceClockY), wxSize(45, 23));
				mhzText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "MHz", wxPoint(windowInfo.ChoiceClockX + 81, windowInfo.ChoiceClockY + 3));
				startButton[computer] = new wxButton(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_START_BUTTON + computer, "Start", wxPoint(windowInfo.ChoiceClockX + 109, windowInfo.ChoiceClockY - 1), wxSize(80, 25));
#endif
#if defined(__linux__)
				clockText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "Clock:", wxPoint(windowInfo.ChoiceClockX - 21, windowInfo.ChoiceClockY + 8));
				clockTextCtrl[computer] = new FloatEdit(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_CLOCK_TEXTCTRL + computer, "", wxPoint(windowInfo.ChoiceClockX + 21, windowInfo.ChoiceClockY), wxSize(55, -1));
				mhzText[computer] = new wxStaticText(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), wxID_ANY, "MHz", wxPoint(windowInfo.ChoiceClockX + 80, windowInfo.ChoiceClockY + 8));
				startButton[computer] = new wxButton(XRCCTRL(*this, "Panel" + computerInfo[computer].gui, wxPanel), GUI_START_BUTTON + computer, "Start", wxPoint(windowInfo.ChoiceClockX + 113, windowInfo.ChoiceClockY - 1), wxSize(80, -1));
#endif
				break;
			}
#if defined(__WXMAC__)
			clockText[computer]->SetFont(defaultFont);
			clockTextCtrl[computer]->SetFont(defaultFont);
			mhzText[computer]->SetFont(defaultFont);
			startButton[computer]->SetFont(defaultFont);
#endif
			this->Connect(GUI_CLOCK_TEXTCTRL + computer, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(GuiMain::onClock));
			this->Connect(GUI_START_BUTTON + computer, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::onStart));
		}
	}

	for (int computer = 0; computer < NO_COMPUTER; computer++)
	{
		conf[computer].configurationInfo_.menuName = "";
		conf[computer].configurationInfo_.subMenuName = "";
		conf[computer].configurationInfo_.fileName = "";
		conf[computer].saveStart_ = 0;
	}
}

void Main::readConfig()
{
	wxMenuBar *menubarPointer = GetMenuBar();

    int saveSelectedComputer = selectedComputer_;
    
    readDebugConfig();
	readComxConfig();
    readInitialComxConfig();
    readElf2KConfig();
	readMS2000Config();
	readMcdsConfig();
	readCosmicosConfig();
	readVipConfig();		
	readVipIIConfig();		
    readVelfConfig();
	readElfConfig(ELF, "Elf");
	readElfConfig(ELFII, "ElfII");
	readElfConfig(SUPERELF, "SuperElf");
	readMembershipConfig();
    readMicrotutorConfig();
    readStudioConfig();
	readVisicomConfig();	
	readVictoryConfig();	
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
    readVelfWindowConfig();
    readElfWindowConfig(ELF, "Elf");
    readElfWindowConfig(ELFII, "ElfII");
    readElfWindowConfig(SUPERELF, "SuperElf");
    readMembershipWindowConfig();
    readMicrotutorWindowConfig();
    readStudioWindowConfig();
    readVisicomWindowConfig();
    readVictoryWindowConfig();
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

	configPointer->Read("/Main/Save_Debug_File", &saveDebugFile_, false);
    configPointer->Read("/Main/Save_On_Exit", &saveOnExit_, true);
	configPointer->Read("/Main/Check_For_Update", &checkForUpdate_, true);
    configPointer->Read("/Main/Floating_Point_Zoom", &fullScreenFloat_, true);
    configPointer->Read("/Main/Use_Num_Pad", &useNumPad_, true);
	configPointer->Read("/Main/Use_Exit_Key", &useExitKey_, false);
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

    wxString cpuTypeString = configPointer->Read("/Main/Cpu_Type", "CDP1805");
    wxString cpuStartupRegistersString = configPointer->Read("/Main/Cpu_StartupRegisters", "StartupRegistersRandom");
    wxString cpuStartupRamString = configPointer->Read("/Main/Cpu_StartupRam", "StartupRamZeroed");
    wxString cpuStartupVideoRamString = configPointer->Read("/Main/Cpu_StartupVideoRam", "StartupVideoRamZeroed");
    keyboardTypeMenuItem_ = configPointer->Read("/Main/Keyboard_Type", "KeyboardUs");
    wxString equalizationString = configPointer->Read("/Main/Equalization", "TV Speaker");

	wxFont smallFont(6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#if defined(__WXMAC__)
	wxFont defaultFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#else
	wxFont defaultFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#endif

	if (mode_.gui)
	{
		XRCCTRL(*this, "AssSaveDebugFile", wxCheckBox)->SetValue(saveDebugFile_);
        menubarPointer->Check(XRCID(GUISAVEONEXIT), saveOnExit_);
		menubarPointer->Check(XRCID("MI_UpdateCheck"), checkForUpdate_);
		menubarPointer->Check(XRCID("MI_FullScreenFloat"), fullScreenFloat_);
        menubarPointer->Check(XRCID("MI_FixedWindowPosition"), mode_.window_position_fixed);
        menubarPointer->Check(XRCID("MI_NumPad"), useNumPad_);
		menubarPointer->Check(XRCID(equalizationString), true);
        menubarPointer->Check(XRCID(cpuTypeString), true);
        menubarPointer->Check(XRCID(cpuStartupRegistersString), true);
        menubarPointer->Check(XRCID(cpuStartupRamString), true);
        menubarPointer->Check(XRCID(cpuStartupVideoRamString), true);
        menubarPointer->Check(XRCID(keyboardTypeMenuItem_), true);
		menubarPointer->SetFont(defaultFont);
	}
    
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
//		XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COMXTAB);
//		XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(DEBUGGERTAB);

		XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetSelection(configPointer->Read("/Main/Selected_Studio_Tab", 0l));
        
        long elfChoiceBookTab = configPointer->Read("/Main/Selected_Cosmac_Tab", 0l);
        if (elfChoiceBookTab > 6)
            elfChoiceBookTab = 0;
		XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(elfChoiceBookTab);
		XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(configPointer->Read("/Main/Selected_Rca_Tab", 0l));
		XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->SetSelection(configPointer->Read("/Main/Selected_Telmac_Tab", 0l));
		XRCCTRL(*this, "DebuggerChoiceBook", wxChoicebook)->SetSelection(configPointer->Read("/Main/Selected_Debugger_Tab", 0l));
        /*
#if defined(__WXMAC__)
		XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetFont(defaultFont);
		XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetFont(defaultFont);
		XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetFont(defaultFont);
		XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->SetFont(defaultFont);
		XRCCTRL(*this, "DebuggerChoiceBook", wxChoicebook)->SetFont(defaultFont);
		XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetFont(defaultFont);
		XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetFont(defaultFont);
		XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->SetFont(defaultFont);
		XRCCTRL(*this, "DebuggerChoiceBook", wxChoicebook)->SetFont(defaultFont);
//		eventDisableControls();
#endif*/

		XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(configPointer->Read("/Main/Selected_Tab", 0l));
		eventChangeNoteBook();
/*
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
		if (windowInfo.operatingSystem == OS_LINUX_OPENSUSE_GNOME || windowInfo.operatingSystem == OS_LINUX_UBUNTU_11_04 || windowInfo.operatingSystem == OS_LINUX_FEDORA)
		{
			XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX - windowInfo.xBorder - 6, windowInfo.mainwY - windowInfo.yBorder);
			XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX - windowInfo.xBorder - 6, windowInfo.mainwY - windowInfo.yBorder);
			XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX-windowInfo.xBorder -6, windowInfo.mainwY-windowInfo.yBorder);
			XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX-windowInfo.xBorder -6, windowInfo.mainwY-windowInfo.yBorder);
			XRCCTRL(*this, "DebuggerChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX-windowInfo.xBorder -6, windowInfo.mainwY-windowInfo.yBorder);
		}
		else
		{
			XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX - windowInfo.xBorder, windowInfo.mainwY - windowInfo.yBorder); // 537 406
			XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX - windowInfo.xBorder, windowInfo.mainwY - windowInfo.yBorder); // 537 406
			XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX-windowInfo.xBorder, windowInfo.mainwY-windowInfo.yBorder);
			XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX-windowInfo.xBorder, windowInfo.mainwY-windowInfo.yBorder);
			XRCCTRL(*this, "DebuggerChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX-windowInfo.xBorder, windowInfo.mainwY-windowInfo.yBorder);
		}
#endif*/
//#if defined (__WXMSW__) || (__WXMAC__)
#if defined (__WXMAC__)
		XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX - windowInfo.xBorder - 24, windowInfo.mainwY - windowInfo.yBorder);
		XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX - windowInfo.xBorder - 24, windowInfo.mainwY - windowInfo.yBorder);
		XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX-windowInfo.xBorder-24, windowInfo.mainwY-windowInfo.yBorder);
		XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX-windowInfo.xBorder-24, windowInfo.mainwY-windowInfo.yBorder);
		XRCCTRL(*this, "DebuggerChoiceBook", wxChoicebook)->SetClientSize(windowInfo.mainwX-windowInfo.xBorder-24, windowInfo.mainwY-windowInfo.yBorder);
#endif
#if defined (__WXMSW__)
		if (windowInfo.operatingSystem != OS_WINDOWS_2000 )
		{
			XRCCTRL(*this, "PanelElf2K", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelVip", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelVipII", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
			XRCCTRL(*this, "PanelCosmicos", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
			XRCCTRL(*this, "PanelElf", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
			XRCCTRL(*this, "PanelElfII", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
			XRCCTRL(*this, "PanelSuperElf", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelMembership", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelVelf", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelMS2000", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelMicrotutor", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelStudio2", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
			XRCCTRL(*this, "PanelVisicom", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
			XRCCTRL(*this, "PanelVictory", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
			XRCCTRL(*this, "PanelTmc600", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
			XRCCTRL(*this, "PanelTMC1800", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
			XRCCTRL(*this, "PanelTMC2000", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
			XRCCTRL(*this, "PanelNano", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "Message", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelDirectAssembler", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelTrace", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
            XRCCTRL(*this, "PanelChip8", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
			XRCCTRL(*this, "PanelMemory", wxPanel)->SetBackgroundColour(wxColour(255,255,255));
		}
#endif
	}

	psaveData_[0] = (int)configPointer->Read("/Main/Psave_Volume", 15l);
	psaveData_[1] = (int)configPointer->Read("/Main/Psave_Bit_Rate", 1l);
	psaveData_[2] = (int)configPointer->Read("/Main/Psave_Bits_Per_Sample", 1l);
	psaveData_[3] = (int)configPointer->Read("/Main/Wav_Conversion_Type", 0l);
	psaveData_[4] = (int)configPointer->Read("/Main/Cassette_Input_Channel", 0l);
	psaveData_[5] = (int)configPointer->Read("/Main/Cassette_Playback_Input", 0l);
	psaveData_[6] = (int)configPointer->Read("/Main/Cassette_Reversed_Polarity", 0l);
	psaveData_[7] = (int)configPointer->Read("/Main/Cassette_Conversion_Type", 1l);
}

void Main::onHelp(wxCommandEvent& WXUNUSED(event))
{
	wxSetWorkingDirectory(p_Main->getApplicationDir());
	help_->DisplayContents();
}

wxString Main::downloadString(wxString url)
{
	wxFileSystem fs; 
	wxFSFile *file= fs.OpenFile(url); 
	wxString returnString = "";
 
	if (file)   
	{ 
		wxInputStream *in_stream = file->GetStream(); 

		if (in_stream)
		{
			wxStringOutputStream html_stream(&returnString);
			in_stream->Read(html_stream);
		}
		delete file;
	}
	return returnString;
}

bool Main::checkUpdateEmma()
{
	wxString version = configPointer->Read("/Main/Version", EMMA_VERSION);

	latestVersion_ = downloadString("http://www.emma02.hobby-site.com/Emma_02_version.txt");
	
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

	if (latestVersion_.ToDouble(&latestVersionDouble))
	{
		version.ToDouble(&versionDouble);
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
	int answer = wxMessageBox( "Emma 02 V" + latestVersion_ + " is available, download now?\n",
							   "Emma 02", wxICON_QUESTION  | wxYES_DEFAULT | wxYES_NO);

	if (answer == wxYES)
	{
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
		wxLinuxDistributionInfo linuxDistro;
		linuxDistro = ::wxGetLinuxDistributionInfo();
		if (linuxDistro.Id == "Ubuntu")
		{
			if (wxIsPlatform64Bit())
				::wxLaunchDefaultBrowser("http://www.emma02.hobby-site.com/ccount/click.php?id=11"); // 64 Bit
			else
				::wxLaunchDefaultBrowser("http://www.emma02.hobby-site.com/ccount/click.php?id=12"); // 32 Bit
		}
		else
		{
			if (wxIsPlatform64Bit())
				::wxLaunchDefaultBrowser("http://www.emma02.hobby-site.com/ccount/click.php?id=13"); // 64 Bit
			else
				::wxLaunchDefaultBrowser("http://www.emma02.hobby-site.com/ccount/click.php?id=14"); // 32 Bit
		}
		return true;
#elif defined(__WXMAC__)
		::wxLaunchDefaultBrowser("http://www.emma02.hobby-site.com/ccount/click.php?id=15"); 
		return true;
#else
		if (windowInfo.operatingSystem == OS_WINDOWS_XP || windowInfo.operatingSystem == OS_WINDOWS_2000)
		{
			::wxLaunchDefaultBrowser("http://www.emma02.hobby-site.com/ccount/click.php?id=1");
			return true;
		}
		if (wxIsPlatform64Bit())
			::wxLaunchDefaultBrowser("http://www.emma02.hobby-site.com/ccount/click.php?id=17");
		else
			::wxLaunchDefaultBrowser("http://www.emma02.hobby-site.com/ccount/click.php?id=16");
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
	//	Destroy();
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

    if (!wxDir::Exists(dataDir_ + "Configurations"))
        wxDir::Make(dataDir_ + "Configurations");

    for (int computer=2; computer<NO_COMPUTER; computer++)
    {
        wxString filename;

        if (!wxDir::Exists(conf[computer].configurationDir_))
            wxDir::Make(conf[computer].configurationDir_);
        
        wxString dirName = conf[computer].configurationDir_;
        wxDir dir (dirName);
        
        conf[computer].configurationMenu = new wxMenu;
        conf[computer].configurationDeleteMenu = new wxMenu;
        conf[computer].configurationSubMenuInfoNumber_ = 0;
        
        bool cont = dir.GetFirst(&filename);
        
        while ( cont )
        {
            wxFileName configFile(dirName + filename);
            if (configFile.GetExt() == "ini")
            {
                configurationInfo = getMenuInfo(dirName + filename);
                
                if (configurationInfo.subMenuName != "")
                {
                    int subMenuFound = -1;
                    for (size_t i=0; i<conf[computer].configurationSubMenuInfoNumber_; i++)
                    {
                        if (conf[computer].configurationSubMenuInfo_[i].subMenuName == configurationInfo.subMenuName)
                            subMenuFound = (int)i;
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
                configurationMenuInfo_[configurationMenuInfoNumber_-1].fileName = filename;
                configurationMenuInfo_[configurationMenuInfoNumber_-1].menuName = configurationInfo.menuName;
                configurationMenuInfo_[configurationMenuInfoNumber_-1].subMenuName = configurationInfo.subMenuName;
           }
            cont = dir.GetNext(&filename);
        }
    }
    
    configurationMenuOn_ = true;
    if (selectedComputer_ <= 2)
    {
        configurationMenu->Insert(0, GUI_CONFIG_MENU, "Load", conf[2].configurationMenu);
        configurationMenu->Insert(2, GUI_CONFIG_DELETE_MENU, "Delete", conf[2].configurationDeleteMenu);
    }
    else
    {
        if (selectedComputer_ < NO_COMPUTER)
        {
            configurationMenu->Insert(0, GUI_CONFIG_MENU, "Load", conf[selectedComputer_].configurationMenu);
            configurationMenu->Insert(2, GUI_CONFIG_DELETE_MENU, "Delete", conf[selectedComputer_].configurationDeleteMenu);
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
    
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
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
            
        case VISICOM:
            writeVisicomDirConfig();
            writeVisicomConfig();
        break;
            
        case VICTORY:
            writeVictoryDirConfig();
            writeVictoryConfig();
        break;
            
        case VIP:
            writeVipDirConfig();
            writeVipConfig();
        break;
            
        case VIPII:
            writeVipIIDirConfig();
            writeVipIIConfig();
        break;
            
        case VELF:
            writeVelfDirConfig();
            writeVelfConfig();
        break;
            
        case MEMBER:
            writeMembershipDirConfig();
            writeMembershipConfig();
        break;
            
        case MICROTUTOR:
            writeMicrotutorDirConfig();
            writeMicrotutorConfig();
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
    }
    configPointer = tempConfigPointer;
    delete pConfig;
    wxConfigBase::Set(currentConfigPointer);
       
    if (selectedComputer_ < NO_COMPUTER && configurationMenuOn_ == true)
    {
        configurationMenu->Remove(GUI_CONFIG_MENU);
        configurationMenu->Remove(GUI_CONFIG_DELETE_MENU);
    }

    for (int comp=2; comp<NO_COMPUTER; comp++)
    {
        delete conf[comp].configurationMenu;
        delete conf[comp].configurationDeleteMenu;
    }
    
    buildConfigMenu();
    
    return wxYES;
}

ConfigurationInfo Main::getMenuInfo(wxString fileName)
{
    ConfigurationInfo configurationInfo;
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
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
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
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
            
        case VISICOM:
            readVisicomConfig();
        break;
            
        case VICTORY:
            readVictoryConfig();
        break;
            
        case VIP:
            readVipConfig();
        break;
            
        case VIPII:
            readVipIIConfig();
        break;
            
        case VELF:
            readVelfConfig();
        break;
            
        case MEMBER:
            readMembershipConfig();
            XRCCTRL(*this, "ClearRamMembership", wxCheckBox)->SetValue(true);
            elfConfiguration[MEMBER].clearRam = true;
        break;
            
        case MICROTUTOR:
            readMicrotutorConfig();
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
//	Destroy();
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
                configurationMenu->Remove(GUI_CONFIG_DELETE_MENU);
            }
            
            for (int comp=2; comp<NO_COMPUTER; comp++)
            {
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
    
    
    if (mode_.gui)
    {
        configPointer->Write("/Main/Selected_Tab", XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->GetSelection());
		configPointer->Write("/Main/Selected_Cosmac_Tab", XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->GetSelection());
		configPointer->Write("/Main/Selected_Rca_Tab", XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->GetSelection());
		configPointer->Write("/Main/Selected_Studio_Tab", XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->GetSelection());
        configPointer->Write("/Main/Selected_Telmac_Tab", XRCCTRL(*this, "TelmacChoiceBook", wxChoicebook)->GetSelection());
        configPointer->Write("/Main/Selected_Debugger_Tab", XRCCTRL(*this, "DebuggerChoiceBook", wxChoicebook)->GetSelection());
        
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
			regPointer->Write("/DataDirRelative", true);

			wxString tempDir = dataDir_;
			tempDir.Replace(applicationDirectory_, "");
			regPointer->Write("/DataDir", tempDir);
		}
		else
		{
			regPointer->Write("/DataDirRelative", false);
			regPointer->Write("/DataDir", dataDir_);
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
	if (computerRunning_)
		p_Computer->charEvent(event.GetKeyCode());
}

void Main::onKeyDown(wxKeyEvent& event)
{
	int key = event.GetKeyCode();
	if (key == WXK_UP)
	{
		if (computerRunning_ && selectedComputer_ == DEBUGGER && debuggerChoice_ == DIRECTASSTAB)
		{
			if (event.GetModifiers() == wxMOD_SHIFT)
				onAssSpinPageUp();
			else
			{
				assSpinUp();
				directAss();
			}
			return;
		}
	}

	if (key == WXK_DOWN)
	{
		if (computerRunning_ && selectedComputer_ == DEBUGGER && debuggerChoice_ == DIRECTASSTAB)
		{
			if (event.GetModifiers() == wxMOD_SHIFT)
				onAssSpinPageDown();
			else
			{
				assSpinDown();
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
		if (computerRunning_ && selectedComputer_ == DEBUGGER && debuggerChoice_ == DIRECTASSTAB)
		{
			if (event.GetModifiers() == wxMOD_SHIFT)
				onAssSpinPageUp();
			else
			{
				assSpinUp();
				directAss();
			}
			return;
		}
	}

	if (rot < 0)
	{
		if (computerRunning_ && selectedComputer_ == DEBUGGER && debuggerChoice_ == DIRECTASSTAB)
		{
			if (event.GetModifiers() == wxMOD_SHIFT)
				onAssSpinPageDown();
			else
			{
				assSpinDown();
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
			if (p_Video != NULL)
				p_Video->onF5();
			if (p_Vt100 != NULL)
				p_Vt100->onF5();
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
		if (key == p_Computer->getInKey1() || key == p_Computer->getInKey2())
		{
			p_Computer->onInButtonPress();
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
		if (p_Vt100 != NULL)
			p_Vt100->activateMainWindow();
		else if (p_Video != NULL)
			p_Video->activateMainWindow();
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
		if ((p_Video != NULL) && (p_Vt100 != NULL))
		{
			if (!p_Video->isFullScreenSet() && !p_Vt100->isFullScreenSet())
				p_Vt100->onF3();
			else if (p_Vt100->isFullScreenSet())
			{
				p_Vt100->onF3();
				while (p_Vt100->isFullScreenSet()) 
				{
					wxSleep(1);
				}
				p_Video->onF3();
			}
			else
			{
				p_Video->onF3();
			}
		}
		else
		{
			if (p_Video != NULL)
				p_Video->onF3();
			if (p_Vt100 != NULL)
				p_Vt100->onF3();
		}
	}
}

void Main::popUp()
{
	if (runningComputer_ == STUDIO || runningComputer_ == VICTORY || runningComputer_ == VISICOM || runningComputer_ == CIDELSA)
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

		case STUDIO:
			p_Studio2->Move(conf[STUDIO].mainX_, conf[STUDIO].mainY_);
		break;

		case VISICOM:
			p_Visicom->Move(conf[VISICOM].mainX_, conf[VISICOM].mainY_);
		break;

		case VICTORY:
			p_Victory->Move(conf[VICTORY].mainX_, conf[VICTORY].mainY_);
		break;

		case VIP:
			p_Vip->moveWindows();
			p_Vip->Move(conf[VIP].mainX_, conf[VIP].mainY_);
		break;

        case VIPII:
            p_Vip2->Move(conf[VIPII].mainX_, conf[VIPII].mainY_);
        break;
            
        case VELF:
			p_Velf->moveWindows();
			p_Velf->Move(conf[VELF].mainX_, conf[VELF].mainY_);
		break;
            
		case MEMBER:
			p_Membership->moveWindows();
			p_Membership->Move(conf[MEMBER].mainX_, conf[MEMBER].mainY_);
		break;

		case MICROTUTOR:
			p_Microtutor->Move(conf[MICROTUTOR].mainX_, conf[MICROTUTOR].mainY_);
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
	}
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
    conf[VELF].mainX_ = -1;
    conf[VELF].mainY_ = -1;
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
	conf[VISICOM].mainX_ = -1;
	conf[VISICOM].mainY_ = -1;
	conf[VICTORY].mainX_ = -1;
	conf[VICTORY].mainY_ = -1;
	conf[MEMBER].mainX_ = -1;
	conf[MEMBER].mainY_ = -1;
	conf[MICROTUTOR].mainX_ = -1;
	conf[MICROTUTOR].mainY_ = -1;

	for (int i=0; i<5; i++)
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
	conf[MS2000].vtX_ = -1;
	conf[MS2000].vtY_ = -1;
	conf[MCDS].vtX_ = -1;
	conf[MCDS].vtY_ = -1;
	conf[VIP].vtX_ = -1;
    conf[VIP].vtY_ = -1;
	ledModuleX_ = -1;
	ledModuleY_ = -1;
	switchX_ = -1;
	switchY_ = -1;
}

void Main::fixedWindowPosition()
{
	mainWindowX_ = 30 + ubuntuOffsetX_;
	mainWindowY_ = 30;
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
    conf[VELF].mainX_ = mainWindowX_;
    conf[VELF].mainY_ = mainWindowY_+windowInfo.mainwY+windowInfo.yBorder;
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
	conf[VISICOM].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
	conf[VISICOM].mainY_ = mainWindowY_;
	conf[VICTORY].mainX_ = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
	conf[VICTORY].mainY_ = mainWindowY_;
	conf[MEMBER].mainX_ = mainWindowX_;
	conf[MEMBER].mainY_ = mainWindowY_ + windowInfo.mainwY + windowInfo.yBorder;
	conf[MICROTUTOR].mainX_ = mainWindowX_;
	conf[MICROTUTOR].mainY_ = mainWindowY_ + windowInfo.mainwY + windowInfo.yBorder;

	for (int i=0; i<5; i++)
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
	conf[MS2000].vtX_ = mainWindowX_ + windowInfo.mainwX + windowInfo.xBorder;
	conf[MS2000].vtY_ = mainWindowY_;
	conf[MCDS].vtX_ = mainWindowX_ + windowInfo.mainwX + windowInfo.xBorder;
	conf[MCDS].vtY_ = mainWindowY_;
	conf[VIP].vtX_ = mainWindowX_ + windowInfo.mainwX + windowInfo.xBorder;
	conf[VIP].vtY_ = mainWindowY_ + 426 + windowInfo.yBorder;
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

	updateAssPage_ = true;
	updateMemoryPage_ = true;
	emuClosing_ = false;
	thermalEf_ = false;
	statusLedUpdate_ = true;
	slotLedUpdate_ = true;
	chip8Steps_ = -1;
	performChip8Step_ = false;
	additionalChip8Details_ = false;

	p_Video = NULL;
	p_Vt100 = NULL;

	runningComputer_ = computer;
	selectedComputer_ = computer;
#ifndef __WXMAC__
	wxSetWorkingDirectory(workingDir_);
#endif
	setClock(runningComputer_);
	conf[runningComputer_].zoom_.ToDouble(&zoom);
	conf[runningComputer_].xScale_.ToDouble(&xScale);

	if (!fullScreenFloat_)
		zoom = (int) zoom;

	switch (runningComputer_)
	{
		case COMX:
			p_Main->setComxExpLedOn (false);
			if (conf[COMX].videoMode_ == PAL)
				comxy = 216;
			else
				comxy = 192;
			p_Comx = new Comx(computerInfo[COMX].name, wxPoint(conf[COMX].mainX_, conf[COMX].mainY_), wxSize(240 * zoom, comxy * zoom), zoom, COMX, conf[COMX].clockSpeed_);
			p_Video = p_Comx;
			p_Computer = p_Comx;
		break;

		case MEMBER:
			p_Membership = new Membership(computerInfo[MEMBER].name, wxPoint(conf[MEMBER].mainX_, conf[MEMBER].mainY_), wxSize(483, 297), conf[MEMBER].clockSpeed_, elfConfiguration[MEMBER]);
			p_Computer = p_Membership;
		break;

		case MICROTUTOR:
			p_Microtutor = new Microtutor(computerInfo[MICROTUTOR].name, wxPoint(conf[MICROTUTOR].mainX_, conf[MICROTUTOR].mainY_), wxSize(333, 160), conf[MICROTUTOR].clockSpeed_, elfConfiguration[MICROTUTOR]);
			p_Computer = p_Microtutor;
		break;

		case ELF:
			p_Elf = new Elf(computerInfo[ELF].name, wxPoint(conf[ELF].mainX_, conf[ELF].mainY_), wxSize(346, 464), conf[ELF].clockSpeed_, elfConfiguration[ELF]);
			p_Computer = p_Elf;
        break;

		case ELFII:
			p_Elf2 = new Elf2(computerInfo[ELFII].name, wxPoint(conf[ELFII].mainX_, conf[ELFII].mainY_), wxSize(534, 386), conf[ELFII].clockSpeed_, elfConfiguration[ELFII]);
			p_Computer = p_Elf2;
		break;

		case SUPERELF:
			p_Super = new Super(computerInfo[SUPERELF].name, wxPoint(conf[SUPERELF].mainX_, conf[SUPERELF].mainY_), wxSize(534, 386), conf[SUPERELF].clockSpeed_, elfConfiguration[SUPERELF]);
			p_Computer = p_Super;
		break;

		case ELF2K:
			p_Elf2K = new Elf2K(computerInfo[ELF2K].name, wxPoint(conf[ELF2K].mainX_, conf[ELF2K].mainY_), wxSize(507, 459), conf[ELF2K].clockSpeed_, elfConfiguration[ELF2K]);
			p_Computer = p_Elf2K;
        break;

        case MS2000:
            p_Ms2000 = new Ms2000(computerInfo[MS2000].name, wxPoint(conf[MS2000].mainX_, conf[MS2000].mainY_), wxSize(507, 459), conf[MS2000].clockSpeed_, elfConfiguration[MS2000]);
            p_Computer = p_Ms2000;
        break;
            
		case MCDS:
			p_Mcds = new Mcds(computerInfo[MCDS].name, wxPoint(conf[MCDS].mainX_, conf[MCDS].mainY_), wxSize(507, 459), conf[MCDS].clockSpeed_, elfConfiguration[MCDS]);
			p_Computer = p_Mcds;
		break;

		case COSMICOS:
			p_Cosmicos = new Cosmicos(computerInfo[COSMICOS].name, wxPoint(conf[COSMICOS].mainX_, conf[COSMICOS].mainY_), wxSize(333, 160), conf[COSMICOS].clockSpeed_, elfConfiguration[COSMICOS]);
			p_Computer = p_Cosmicos;
		break;

		case STUDIO:
			p_Studio2 = new Studio2(computerInfo[STUDIO].name, wxPoint(conf[STUDIO].mainX_, conf[STUDIO].mainY_), wxSize(64*zoom*xScale, 128*zoom), zoom, xScale, STUDIO);
			p_Video = p_Studio2;
			p_Computer = p_Studio2;
		break;

		case VISICOM:
			p_Visicom = new Visicom(computerInfo[VISICOM].name, wxPoint(conf[VISICOM].mainX_, conf[VISICOM].mainY_), wxSize(64*zoom*xScale, 128*zoom), zoom, xScale, VISICOM);
			p_Video = p_Visicom;
			p_Computer = p_Visicom;
		break;

		case VICTORY:
			p_Victory = new Victory(computerInfo[VICTORY].name, wxPoint(conf[VICTORY].mainX_, conf[VICTORY].mainY_), wxSize(64*zoom*xScale, 192*zoom), zoom, xScale, VICTORY);
			p_Video = p_Victory;
			p_Computer = p_Victory;
		break;

		case VIP:
			p_Vip = new Vip(computerInfo[VIP].name, wxPoint(conf[VIP].mainX_, conf[VIP].mainY_), wxSize(64*zoom*xScale, 128*zoom), zoom, xScale, VIP, conf[VIP].clockSpeed_, conf[VIP].tempo_, elfConfiguration[VIP]);
			p_Video = p_Vip;
			p_Computer = p_Vip;
			if (getVipStereo())
				stereo = 2;
			if (conf[VIP].soundType_ == VIP_SUPER2)
				toneChannels = 2;
			if (conf[VIP].soundType_ == VIP_SUPER4)
				toneChannels = 4;
		break;

		case VIPII:
			p_Vip2 = new VipII(computerInfo[VIPII].name, wxPoint(conf[VIPII].mainX_, conf[VIPII].mainY_), wxSize(64*zoom*xScale, 128*zoom), zoom, xScale, VIPII, conf[VIPII].clockSpeed_, conf[VIPII].tempo_);
			p_Video = p_Vip2;
			p_Computer = p_Vip2;
		break;

        case VELF:
            p_Velf = new Velf(computerInfo[VELF].name, wxPoint(conf[VELF].mainX_, conf[VELF].mainY_), wxSize(310, 180), conf[VELF].clockSpeed_, elfConfiguration[VELF]);
            p_Computer = p_Velf;
        break;
            
		case TMC1800:
			p_Tmc1800 = new Tmc1800(computerInfo[TMC1800].name, wxPoint(conf[TMC1800].mainX_, conf[TMC1800].mainY_), wxSize(64*zoom*xScale, 128*zoom), zoom, xScale, TMC1800);
			p_Video = p_Tmc1800;
			p_Computer = p_Tmc1800;
		break;

		case TMC2000:
			p_Tmc2000 = new Tmc2000(computerInfo[TMC2000].name, wxPoint(conf[TMC2000].mainX_, conf[TMC2000].mainY_), wxSize(64*zoom*xScale, 192*zoom), zoom, xScale, TMC2000);
			p_Video = p_Tmc2000;
			p_Computer = p_Tmc2000;
		break;

		case ETI:
			p_Eti = new Eti(computerInfo[ETI].name, wxPoint(conf[ETI].mainX_, conf[ETI].mainY_), wxSize(64*zoom*xScale, 192*zoom), zoom, xScale, ETI);
			p_Video = p_Eti;
			p_Computer = p_Eti;
		break;

		case NANO:
			p_Nano = new Nano(computerInfo[NANO].name, wxPoint(conf[NANO].mainX_, conf[NANO].mainY_), wxSize(64*zoom*xScale, 192*zoom), zoom, xScale, NANO);
			p_Video = p_Nano;
			p_Computer = p_Nano;
		break;

		case CIDELSA:
			p_Cidelsa = new Cidelsa(computerInfo[CIDELSA].name, wxPoint(conf[CIDELSA].mainX_, conf[CIDELSA].mainY_), wxSize(200*zoom, 240*zoom), zoom, CIDELSA, conf[CIDELSA].clockSpeed_);
			p_Video = p_Cidelsa;
			p_Computer = p_Cidelsa;
		break;

		case TMC600:
			p_Tmc600 = new Tmc600(computerInfo[TMC600].name, wxPoint(conf[TMC600].mainX_, conf[TMC600].mainY_), wxSize(240*zoom, 216*zoom), zoom, TMC600, conf[TMC600].clockSpeed_);
			p_Video = p_Tmc600;
			p_Computer = p_Tmc600;
		break;

		case PECOM:
			p_Pecom = new Pecom(computerInfo[PECOM].name, wxPoint(conf[PECOM].mainX_, conf[PECOM].mainY_), wxSize(240*zoom, 216*zoom), zoom, PECOM, conf[PECOM].clockSpeed_);
			p_Video = p_Pecom;
			p_Computer = p_Pecom;
		break;
	}
    
    if (runningComputer_ < 9 || runningComputer_ == VIPII)
    {
        conf[runningComputer_].ledTime_.ToLong(&ms);
        conf[runningComputer_].ledTimeMs_ = ms;
        if (ms == 0)
            ledTimePointer->Stop();
        else
            ledTimePointer->Start((int)ms, wxTIMER_CONTINUOUS);
    }
    if (mode_.gui)
    {
        XRCCTRL(*this, "Message_Window", wxTextCtrl)->Clear();
        vuPointer->Start(100, wxTIMER_CONTINUOUS);
#ifdef __WXMAC__
        traceTimeoutPointer->Start(100, wxTIMER_CONTINUOUS);
#endif
        cpuPointer->Start(1000, wxTIMER_CONTINUOUS);
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
		XRCCTRL(*this, "RunningComputer", wxStaticText)->SetLabelText("Running computer: "+computerInfo[runningComputer_].name);
#else
		XRCCTRL(*this, "RunningComputer", wxStaticText)->SetLabel("Running computer: "+computerInfo[runningComputer_].name);
#endif
	    assNew(0);
	}
    p_Computer->startComputer();
	if (!mode_.gui)
	{
		if (runningComputer_ < 6)
			if (elfConfiguration[runningComputer_].vtType == VTNONE)
				p_Main->eventVideoSetFullScreen(mode_.full_screen);
			else
				p_Main->eventVtSetFullScreen(mode_.full_screen);
		else	
			p_Main->eventVideoSetFullScreen(mode_.full_screen);
	}
}

void Main::stopComputer()
{ 
	stopAssLog();

	if (emuClosing_)  return;
	emuClosing_ = true;
	if (popupDialog_ != NULL)
		delete popupDialog_;
	popupDialog_ = NULL;
	ledTimePointer->Stop();
	if (mode_.gui)
	{
		vuPointer->Stop();
		cpuPointer->Stop();
		switch (runningComputer_)
		{
			case COMX:
				setComxStatusLedOn (false);
				vuSet("Vu"+computerInfo[runningComputer_].gui, 0);
			break;
			case ELF:
			case ELFII:
			case SUPERELF:
				enableMemAccessGui(false);
				vuSet("Vu"+computerInfo[runningComputer_].gui, 0);
			break;
			case VIP:
			case VIPII:
            case VELF:
			case COSMICOS:
			case TMC600:
			case TMC1800:
			case TMC2000:
			case NANO:
			case PECOM:
			case ETI:
				vuSet("Vu"+computerInfo[runningComputer_].gui, 0);
			break;
			case NO_COMPUTER:
				return;
			break;
		}
#if wxCHECK_VERSION(2, 9, 0)
		XRCCTRL(*this, "RunningComputer", wxStaticText)->SetLabelText("Last executed computer: "+computerInfo[runningComputer_].name);
#else
		XRCCTRL(*this, "RunningComputer", wxStaticText)->SetLabel("Last executed computer: "+computerInfo[runningComputer_].name);
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
	p_Video = NULL;
	p_Vt100 = NULL;
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
        configurationMenu->Remove(GUI_CONFIG_MENU);
        configurationMenu->Remove(GUI_CONFIG_DELETE_MENU);
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
                    
				case VELFTAB:
                    elfChoice_ = VELF;
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

				case VIPTAB:
					rcaChoice_ = VIP;
				break;

				case VIPIITAB:
					rcaChoice_ = VIPII;
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
				case STUDIOIITAB:
					studioChoice_ = STUDIO;
				break;

				case VISICOMTAB:
					studioChoice_ = VISICOM;
				break;

				case VICTORYTAB:
					studioChoice_ = VICTORY;
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

		case DEBUGGERTAB:
            menubarPointer->Enable(XRCID(GUISAVECOMPUTERCONFIG), false);
			switch(XRCCTRL(*this, "DebuggerChoiceBook", wxChoicebook)->GetSelection())
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

				case ASSTAB:
					debuggerChoice_ = ASSTAB;
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
        configurationMenu->Remove(GUI_CONFIG_MENU);
        configurationMenu->Remove(GUI_CONFIG_DELETE_MENU);
    }
    
	switch(event.GetSelection())
	{
		case STUDIOIITAB:
			studioChoice_ = STUDIO;
		break;

		case VISICOMTAB:
			studioChoice_ = VISICOM;
		break;

		case VICTORYTAB:
			studioChoice_ = VICTORY;
		break;
	}
	selectedComputer_ = studioChoice_;
    setConfigurationMenu();
}

void Main::onTelmacChoiceBook(wxChoicebookEvent&event)
{
    if (selectedComputer_ < NO_COMPUTER && configurationMenuOn_ == true)
    {
        configurationMenu->Remove(GUI_CONFIG_MENU);
        configurationMenu->Remove(GUI_CONFIG_DELETE_MENU);
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
    setConfigurationMenu();
}

void Main::onElfChoiceBook(wxChoicebookEvent&event)
{
	if (selectedComputer_ < NO_COMPUTER && configurationMenuOn_ == true)
	{
		configurationMenu->Remove(GUI_CONFIG_MENU);
		configurationMenu->Remove(GUI_CONFIG_DELETE_MENU);
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

	case VELFTAB:
		elfChoice_ = VELF;
		break;
	}
	selectedComputer_ = elfChoice_;
	setConfigurationMenu();
}

void Main::onRcaChoiceBook(wxChoicebookEvent&event)
{
	if (selectedComputer_ < NO_COMPUTER && configurationMenuOn_ == true)
	{
		configurationMenu->Remove(GUI_CONFIG_MENU);
		configurationMenu->Remove(GUI_CONFIG_DELETE_MENU);
	}

	switch (event.GetSelection())
	{
	case MICROTUTORTAB:
		rcaChoice_ = MICROTUTOR;
		break;

	case VIPTAB:
		rcaChoice_ = VIP;
		break;

	case VIPIITAB:
		rcaChoice_ = VIPII;
		break;

	case MCDSTAB:
		rcaChoice_ = MCDS;
		break;

	case MS2000TAB:
		rcaChoice_ = MS2000;
		break;
	}
	selectedComputer_ = rcaChoice_;
	setConfigurationMenu();
}

void Main::onDebuggerChoiceBook(wxChoicebookEvent&event)
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
		break;

		case MEMORYTAB:
			debuggerChoice_ = MEMORYTAB;
			memoryDisplay();
		break;

		case ASSTAB:
			debuggerChoice_ = ASSTAB;
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
            configurationMenu->Insert(2, GUI_CONFIG_DELETE_MENU, "Delete", conf[2].configurationDeleteMenu);
        }
        else
        {
            if (selectedComputer_ < NO_COMPUTER)
            {
                configurationMenu->Insert(0, GUI_CONFIG_MENU, "Load", conf[selectedComputer_].configurationMenu);
                configurationMenu->Insert(2, GUI_CONFIG_DELETE_MENU, "Delete", conf[selectedComputer_].configurationDeleteMenu);
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

		case MEMBER:
			XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COSMACELFTAB);
			XRCCTRL(*this, "ElfChoiceBook", wxChoicebook)->SetSelection(MEMBERTAB);
		break;

		case MICROTUTOR:
			XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
			XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(MICROTUTORTAB);
		break;

		case VIP:
			XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
			XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(VIPTAB);
		break;

		case VIPII:
			XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
			XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(VIPIITAB);
		break;

		case MS2000:
			XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
			XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(MS2000TAB);
		break;

		case MCDS:
			XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(RCATAB);
			XRCCTRL(*this, "RcaChoiceBook", wxChoicebook)->SetSelection(MCDSTAB);
		break;

		case STUDIO:
			XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(STUDIOTAB);
			XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetSelection(STUDIOIITAB);
		break;

		case VISICOM:
			XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(STUDIOTAB);
			XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetSelection(VISICOMTAB);
		break;

		case VICTORY:
			XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(STUDIOTAB);
			XRCCTRL(*this, "StudioChoiceBook", wxChoicebook)->SetSelection(VICTORYTAB);
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

void Main::enableGui(bool status)
{
	if (!mode_.gui)
		return;
	wxMenuBar *menubarPointer = GetMenuBar();

	menubarPointer->Enable(XRCID(GUIDEFAULT), status);
	menubarPointer->Enable(XRCID("MI_ActivateMain"), !status);
	menubarPointer->Enable(XRCID("MI_FullScreen"), !status);
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

	if (runningComputer_ == COMX)
	{
		chip8ProtectedMode_= false;
		XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
		XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false); 
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
		XRCCTRL(*this,"EpromComx", wxButton)->Enable(status);
		XRCCTRL(*this,"MainRomComx", wxComboBox)->Enable(status&!conf[COMX].sbActive_);
		XRCCTRL(*this,"RomButtonComx", wxButton)->Enable(status&!conf[COMX].sbActive_);
		XRCCTRL(*this,"ExpRomComx", wxComboBox)->Enable(status&!conf[COMX].sbActive_);
		XRCCTRL(*this,"ExpRomButtonComx", wxButton)->Enable(status&!conf[COMX].sbActive_);
		XRCCTRL(*this, "Cart1RomComx", wxComboBox)->Enable(status);
		XRCCTRL(*this, "Cart1RomButtonComx", wxButton)->Enable(status);
		if (expansionRomLoaded_ || conf[COMX].sbActive_)
		{
			XRCCTRL(*this,"Cart2RomComx", wxComboBox)->Enable(status);
			XRCCTRL(*this,"Cart2RomButtonComx", wxButton)->Enable(status);
			XRCCTRL(*this,"Cart3RomComx", wxComboBox)->Enable(status);
			XRCCTRL(*this,"Cart3RomButtonComx", wxButton)->Enable(status);
			XRCCTRL(*this,"Cart4RomComx", wxComboBox)->Enable(status&!conf[COMX].sbActive_);
			XRCCTRL(*this,"Cart4RomButtonComx", wxButton)->Enable(status&!conf[COMX].sbActive_);
		}
		XRCCTRL(*this,"VidModeComx", wxChoice)->Enable(status);
		XRCCTRL(*this,"VidModeTextComx", wxStaticText)->Enable(status);
		XRCCTRL(*this,"PrintButtonComx", wxButton)->Enable(!status);
		XRCCTRL(*this,"ScreenDumpF5Comx", wxButton)->Enable(!status);
		XRCCTRL(*this,"FullScreenF3Comx", wxButton)->Enable(!status);
		XRCCTRL(*this,"ExpRamComx", wxCheckBox)->Enable(status);
		XRCCTRL(*this,"SbActiveComx", wxCheckBox)->Enable(status);
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
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
		XRCCTRL(*this,"MainRomCidelsa", wxComboBox)->Enable(status);
		XRCCTRL(*this,"RomButtonCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ScreenDumpF5Cidelsa", wxButton)->Enable(!status);
		XRCCTRL(*this,"FullScreenF3Cidelsa", wxButton)->Enable(!status);
	}
	if (runningComputer_ == TMC600)
	{
		chip8ProtectedMode_= false;
		XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
		XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
		XRCCTRL(*this,"PrintButtonTmc600", wxButton)->Enable(!status);
		XRCCTRL(*this,"ScreenDumpF5Tmc600", wxButton)->Enable(!status);
		XRCCTRL(*this,"FullScreenF3Tmc600", wxButton)->Enable(!status);
		XRCCTRL(*this,"MainRomTmc600", wxComboBox)->Enable(status);
		XRCCTRL(*this,"RomButtonTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ExpRomTmc600", wxComboBox)->Enable(status);
		XRCCTRL(*this,"ExpRomButtonTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"CharRomTmc600", wxComboBox)->Enable(status);
		XRCCTRL(*this,"CharRomButtonTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"RamTmc600", wxChoice)->Enable(status);
		XRCCTRL(*this,"RamTextTmc600", wxStaticText)->Enable(status);
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
		XRCCTRL(*this,"RealTimeClockTmc600", wxCheckBox)->Enable(status);
		enableLoadGui(!status);
		setRealCas2(runningComputer_);
	}
	if (runningComputer_ == PECOM)
	{
		chip8ProtectedMode_= false;
		XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
		XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
		XRCCTRL(*this,"PrintButtonPecom", wxButton)->Enable(!status);
		XRCCTRL(*this,"ScreenDumpF5Pecom", wxButton)->Enable(!status);
		XRCCTRL(*this,"FullScreenF3Pecom", wxButton)->Enable(!status);
		XRCCTRL(*this,"MainRomPecom", wxComboBox)->Enable(status);
		XRCCTRL(*this,"RomButtonPecom", wxButton)->Enable(status);
		enableLoadGui(!status);
		setRealCas2(runningComputer_);
	}
	if (runningComputer_ == VIP)
	{
		enableChip8DebugGui(!status);
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
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
		enableChip8DebugGui(!status);
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
		XRCCTRL(*this,"MainRomVipII", wxComboBox)->Enable(status);
		XRCCTRL(*this,"MainRom2VipII", wxComboBox)->Enable(status);
		XRCCTRL(*this,"RomButtonVipII", wxButton)->Enable(status);
		XRCCTRL(*this,"RomButton2VipII", wxButton)->Enable(status);
		XRCCTRL(*this,"RamSWVipII", wxComboBox)->Enable(status);
		XRCCTRL(*this,"RamSWButtonVipII", wxButton)->Enable(status);
		XRCCTRL(*this,"Chip8SWVipII", wxTextCtrl)->Enable(status);
		XRCCTRL(*this,"Chip8SWButtonVipII", wxButton)->Enable(status);
		XRCCTRL(*this,"EjectChip8SWVipII", wxButton)->Enable(status);
		XRCCTRL(*this,"FullScreenF3VipII", wxButton)->Enable(!status);
		XRCCTRL(*this,"ScreenDumpF5VipII", wxButton)->Enable(!status);
		enableLoadGui(!status);
		setRealCas2(runningComputer_);
	}
    if (runningComputer_ == VELF)
    {
        enableChip8DebugGui(!status);
        XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
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
	if (runningComputer_ == STUDIO)
	{
		enableChip8DebugGui(!status);
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
		XRCCTRL(*this,"MainRomStudio2", wxComboBox)->Enable(status&(!conf[STUDIO].disableSystemRom_ | !conf[STUDIO].multiCart_));
		XRCCTRL(*this,"RomButtonStudio2", wxButton)->Enable(status&(!conf[STUDIO].disableSystemRom_ | !conf[STUDIO].multiCart_));
		XRCCTRL(*this,"CartRomStudio2", wxComboBox)->Enable(status);
		XRCCTRL(*this,"CartRomButtonStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"FullScreenF3Studio2", wxButton)->Enable(!status);
		XRCCTRL(*this,"ScreenDumpF5Studio2", wxButton)->Enable(!status);
        XRCCTRL(*this,"MultiCartStudio2", wxCheckBox)->Enable(status);
	}
	if (runningComputer_ == VISICOM)
	{
		enableChip8DebugGui(!status);
		XRCCTRL(*this,"MainRomVisicom", wxComboBox)->Enable(status);
		XRCCTRL(*this,"RomButtonVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"CartRomVisicom", wxComboBox)->Enable(status);
		XRCCTRL(*this,"CartRomButtonVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"FullScreenF3Visicom", wxButton)->Enable(!status);
		XRCCTRL(*this,"ScreenDumpF5Visicom", wxButton)->Enable(!status);
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
	}
	if (runningComputer_ == VICTORY)
	{
		enableChip8DebugGui(!status);
		XRCCTRL(*this,"MainRomVictory", wxComboBox)->Enable(status&(!conf[VICTORY].disableSystemRom_ | !conf[VICTORY].multiCart_));
		XRCCTRL(*this,"RomButtonVictory", wxButton)->Enable(status&(!conf[VICTORY].disableSystemRom_ | !conf[VICTORY].multiCart_));
		XRCCTRL(*this,"CartRomVictory", wxComboBox)->Enable(status);
		XRCCTRL(*this,"CartRomButtonVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"FullScreenF3Victory", wxButton)->Enable(!status);
		XRCCTRL(*this,"ScreenDumpF5Victory", wxButton)->Enable(!status);
        XRCCTRL(*this,"MultiCartVictory", wxCheckBox)->Enable(status);
        XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
	}
	if (runningComputer_ == TMC2000)
	{
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
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
		enableLoadGui(!status);
		setRealCas2(runningComputer_);
	}
	if (runningComputer_ == TMC1800)
	{
		enableChip8DebugGui(!status);
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
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
		enableChip8DebugGui(!status);
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
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
		enableChip8DebugGui(!status);
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
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
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
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
			break;
			case SUPERELF:
				XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
				XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
				elfTypeStr = "SuperElf";
			break;
		}
		enableLoadGui(!status);
		setRealCas2(runningComputer_);
		XRCCTRL(*this,"Tape"+elfTypeStr, wxBitmapButton)->Enable(status);
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

		if (elfConfiguration[runningComputer_].usePager || elfConfiguration[runningComputer_].useEms || elfConfiguration[runningComputer_].useRomMapper)
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
			if (elfConfiguration[runningComputer_].useUart)
			{
				XRCCTRL(*this, "VTBaudRText" + elfTypeStr, wxStaticText)->Enable(status);
				XRCCTRL(*this, "VTBaudRChoice" + elfTypeStr, wxChoice)->Enable(status);
			}
			XRCCTRL(*this, "VTBaudTChoice" + elfTypeStr, wxChoice)->Enable(status);
			XRCCTRL(*this, "VTBaudTText" + elfTypeStr, wxStaticText)->Enable(status);
			XRCCTRL(*this,"VtSetup"+elfTypeStr, wxButton)->Enable(status);
		}
		XRCCTRL(*this,"VideoType"+elfTypeStr,wxChoice)->Enable(status);
		XRCCTRL(*this,"VideoTypeText"+elfTypeStr,wxStaticText)->Enable(status);
		XRCCTRL(*this,"DiskType"+elfTypeStr,wxChoice)->Enable(status);
		XRCCTRL(*this,"Keyboard"+elfTypeStr,wxChoice)->Enable(status);
		XRCCTRL(*this,"KeyboardText"+elfTypeStr,wxStaticText)->Enable(status);
		XRCCTRL(*this,"CharRomButton"+elfTypeStr, wxButton)->Enable(status&(elfConfiguration[runningComputer_].use6847||elfConfiguration[runningComputer_].use8275||elfConfiguration[runningComputer_].use6845||elfConfiguration[runningComputer_].useS100));
		XRCCTRL(*this,"VtCharRomButton"+elfTypeStr, wxButton)->Enable(status&(elfConfiguration[runningComputer_].vtType != VTNONE));
		XRCCTRL(*this,"VtCharRom"+elfTypeStr, wxComboBox)->Enable(elfConfiguration[runningComputer_].vtType != VTNONE);
		XRCCTRL(*this,"CharRom"+elfTypeStr, wxComboBox)->Enable(status&(elfConfiguration[runningComputer_].use6847||elfConfiguration[runningComputer_].use8275||elfConfiguration[runningComputer_].use6845||elfConfiguration[runningComputer_].useS100));
		XRCCTRL(*this,"FullScreenF3"+elfTypeStr, wxButton)->Enable(!status&(elfConfiguration[runningComputer_].usePixie||elfConfiguration[runningComputer_].useTMS9918||elfConfiguration[runningComputer_].use6847||elfConfiguration[runningComputer_].use6845||elfConfiguration[runningComputer_].use8275||elfConfiguration[runningComputer_].useS100||(elfConfiguration[runningComputer_].vtType != VTNONE)));
		XRCCTRL(*this,"ScreenDumpF5"+elfTypeStr, wxButton)->Enable(!status&(elfConfiguration[runningComputer_].usePixie||elfConfiguration[runningComputer_].useTMS9918||elfConfiguration[runningComputer_].use6847||elfConfiguration[runningComputer_].use6845||elfConfiguration[runningComputer_].use8275||elfConfiguration[runningComputer_].useS100||(elfConfiguration[runningComputer_].vtType != VTNONE)));
		enableMemAccessGui(!status);
	}
	if (runningComputer_ == ELF2K)
	{
		chip8ProtectedMode_= false;
		XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
		XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
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
		XRCCTRL(*this,"VtCharRomButtonElf2K", wxButton)->Enable(status&(elfConfiguration[ELF2K].vtType != VTNONE));
		XRCCTRL(*this,"VtCharRomElf2K", wxComboBox)->Enable(status&(elfConfiguration[ELF2K].vtType != VTNONE));
		XRCCTRL(*this,"CharRomElf2K", wxComboBox)->Enable(status&elfConfiguration[ELF2K].use8275);
		XRCCTRL(*this,"FullScreenF3Elf2K", wxButton)->Enable(!status&(elfConfiguration[ELF2K].usePixie||elfConfiguration[ELF2K].use8275||(elfConfiguration[ELF2K].vtType != VTNONE)));
		XRCCTRL(*this,"ScreenDumpF5Elf2K", wxButton)->Enable(!status&(elfConfiguration[ELF2K].usePixie||elfConfiguration[ELF2K].use8275||(elfConfiguration[ELF2K].vtType != VTNONE)));
		XRCCTRL(*this,"Elf2KRtc", wxCheckBox)->Enable(status);
	}
    if (runningComputer_ == MS2000)
    {
        XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
        XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
        XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
        XRCCTRL(*this,"MainRomMS2000", wxComboBox)->Enable(status);
        XRCCTRL(*this,"RomButtonMS2000", wxButton)->Enable(status);
        enableMemAccessGui(!status);
        XRCCTRL(*this,"VtCharRomButtonMS2000", wxButton)->Enable(status&(elfConfiguration[MS2000].vtType != VTNONE));
        XRCCTRL(*this,"VtCharRomMS2000", wxComboBox)->Enable(status&(elfConfiguration[MS2000].vtType != VTNONE));
        XRCCTRL(*this,"FullScreenF3MS2000", wxButton)->Enable(!status&(elfConfiguration[MS2000].vtType != VTNONE));
        XRCCTRL(*this,"ScreenDumpF5MS2000", wxButton)->Enable(!status&(elfConfiguration[MS2000].vtType != VTNONE));
        XRCCTRL(*this, "VTTypeMS2000", wxChoice)->Enable(status);
		if (elfConfiguration[MS2000].useUart)
		{
			XRCCTRL(*this, "VTBaudRTextMS2000", wxStaticText)->Enable(status);
			XRCCTRL(*this, "VTBaudRChoiceMS2000", wxChoice)->Enable(status);
		}
		XRCCTRL(*this, "VTBaudTChoiceMS2000", wxChoice)->Enable(status);
		XRCCTRL(*this, "VTBaudRTextMS2000", wxStaticText)->Enable(status);
        XRCCTRL(*this,"VtSetupMS2000", wxButton)->Enable(status);
        enableLoadGui(!status);
        setRealCas2(runningComputer_);
    }
	if (runningComputer_ == MCDS)
	{
		XRCCTRL(*this, "Chip8TraceButton", wxToggleButton)->SetValue(false);
		XRCCTRL(*this, "Chip8DebugMode", wxCheckBox)->SetValue(false);
		XRCCTRL(*this, "ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursVipII", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursVelf", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursMembership", wxButton)->Enable(status);
		XRCCTRL(*this, "MainRomMcds", wxComboBox)->Enable(status);
        XRCCTRL(*this, "MainRom2Mcds", wxComboBox)->Enable(status);
        XRCCTRL(*this, "MainRom3Mcds", wxComboBox)->Enable(status);
        XRCCTRL(*this, "RomButtonMcds", wxButton)->Enable(status);
        XRCCTRL(*this, "RomButton2Mcds", wxButton)->Enable(status);
        XRCCTRL(*this, "RomButton3Mcds", wxButton)->Enable(status);
        enableMemAccessGui(!status);
		XRCCTRL(*this, "VtCharRomButtonMcds", wxButton)->Enable(status&(elfConfiguration[MCDS].vtType != VTNONE));
		XRCCTRL(*this, "VtCharRomMcds", wxComboBox)->Enable(status&(elfConfiguration[MCDS].vtType != VTNONE));
		XRCCTRL(*this, "FullScreenF3Mcds", wxButton)->Enable(!status&(elfConfiguration[MCDS].vtType != VTNONE));
		XRCCTRL(*this, "ScreenDumpF5Mcds", wxButton)->Enable(!status&(elfConfiguration[MCDS].vtType != VTNONE));
		XRCCTRL(*this, "VTTypeMcds", wxChoice)->Enable(status);
		if (elfConfiguration[MCDS].useUart)
		{
			XRCCTRL(*this, "VTBaudRTextMcds", wxStaticText)->Enable(status);
			XRCCTRL(*this, "VTBaudRChoiceMcds", wxChoice)->Enable(status);
		}
		XRCCTRL(*this, "VTBaudTChoiceMcds", wxChoice)->Enable(status);
		XRCCTRL(*this, "VTBaudTTextMcds", wxStaticText)->Enable(status);
		XRCCTRL(*this, "VtSetupMcds", wxButton)->Enable(status);
		enableLoadGui(!status);
		setRealCas2(runningComputer_);
	}
	if (runningComputer_ == COSMICOS)
    {
        chip8ProtectedMode_= false;
		XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
		XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMembership", wxButton)->Enable(status);
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
			if (elfConfiguration[COSMICOS].useUart)
			{
				XRCCTRL(*this, "VTBaudRTextCosmicos", wxStaticText)->Enable(status);
                XRCCTRL(*this, "VTBaudRChoiceCosmicos", wxChoice)->Enable(status);
			}
			XRCCTRL(*this, "VTBaudTChoiceCosmicos", wxChoice)->Enable(status);
			XRCCTRL(*this, "VTBaudTTextCosmicos", wxStaticText)->Enable(status);
            XRCCTRL(*this,"VtSetupCosmicos", wxButton)->Enable(status);
		}

		XRCCTRL(*this,"VideoTypeCosmicos",wxChoice)->Enable(status);
		XRCCTRL(*this,"VideoType_TextCosmicos",wxStaticText)->Enable(status);
		XRCCTRL(*this,"KeyboardCosmicos",wxChoice)->Enable(status);
		XRCCTRL(*this,"Keyboard_TextCosmicos",wxStaticText)->Enable(status);
		XRCCTRL(*this,"VtCharRomButtonCosmicos", wxButton)->Enable(status&(elfConfiguration[COSMICOS].vtType != VTNONE));
		XRCCTRL(*this,"VtCharRomCosmicos", wxComboBox)->Enable(status&(elfConfiguration[COSMICOS].vtType != VTNONE));
		XRCCTRL(*this,"FullScreenF3Cosmicos", wxButton)->Enable(!status&(elfConfiguration[COSMICOS].usePixie||(elfConfiguration[COSMICOS].vtType != VTNONE)));
		XRCCTRL(*this,"ScreenDumpF5Cosmicos", wxButton)->Enable(!status&(elfConfiguration[COSMICOS].usePixie||(elfConfiguration[COSMICOS].vtType != VTNONE)));
	}
	if (runningComputer_ == MEMBER)
	{
		chip8ProtectedMode_= false;
		XRCCTRL(*this,"Chip8TraceButton", wxToggleButton)->SetValue(false);
		XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(false);
		XRCCTRL(*this,"ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVipII", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursVelf", wxButton)->Enable(status);
        XRCCTRL(*this,"ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursCosmicos", wxButton)->Enable(status);
		XRCCTRL(*this,"MainRomMembership", wxComboBox)->Enable(status);
		if (conf[MEMBER].ramType_ == 5 || conf[MEMBER].ramType_ == 6)
			XRCCTRL(*this, "RomMembership", wxButton)->Enable(false);
		else
			XRCCTRL(*this,"RomMembership", wxButton)->Enable(status);
		XRCCTRL(*this,"RomButtonMembership", wxButton)->Enable(status);
		XRCCTRL(*this,"RamMembership", wxChoice)->Enable(status);
		XRCCTRL(*this, "RamTextMembership", wxStaticText)->Enable(status);
		XRCCTRL(*this, "IoMembership", wxChoice)->Enable(status);
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

		XRCCTRL(*this,"VtCharRomButtonMembership", wxButton)->Enable(status&(elfConfiguration[MEMBER].vtType != VTNONE));
		XRCCTRL(*this,"VtCharRomMembership", wxComboBox)->Enable(status&(elfConfiguration[MEMBER].vtType != VTNONE));
		XRCCTRL(*this,"FullScreenF3Membership", wxButton)->Enable(elfConfiguration[MEMBER].vtType != VTNONE);
		XRCCTRL(*this,"ScreenDumpF5Membership", wxButton)->Enable(elfConfiguration[MEMBER].vtType != VTNONE);
	}
	if (runningComputer_ == MICROTUTOR)
	{
		XRCCTRL(*this, "Chip8TraceButton", wxToggleButton)->SetValue(false);
		XRCCTRL(*this, "Chip8DebugMode", wxCheckBox)->SetValue(false);
		XRCCTRL(*this, "ColoursComx", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursVip", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursVipII", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursVelf", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursMS2000", wxButton)->Enable(status);
		XRCCTRL(*this,"ColoursMcds", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursElf", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursElfII", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursElf2K", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursSuperElf", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursStudio2", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursVisicom", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursVictory", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursCidelsa", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursTmc600", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursTMC1800", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursTMC2000", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursNano", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursPecom", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursEti", wxButton)->Enable(status);
		XRCCTRL(*this, "ColoursMembership", wxButton)->Enable(status);
		XRCCTRL(*this, "MainRomMicrotutor", wxComboBox)->Enable(status);
		XRCCTRL(*this, "RomButtonMicrotutor", wxButton)->Enable(status);
		enableMemAccessGui(!status);
	}
	enableDebugGui(!status);

/*	if (status == true)
	{
		XRCCTRL(*this,"AssProgramSlot",wxTextCtrl)->ChangeValue("");
		XRCCTRL(*this,"AssProgramStart",wxTextCtrl)->ChangeValue("");
		XRCCTRL(*this,"AssCodeEnd",wxTextCtrl)->ChangeValue("");
		XRCCTRL(*this,"AssProgramEnd",wxTextCtrl)->ChangeValue("");
		XRCCTRL(*this,"AssRangeNumber",wxStaticText)->SetLabel("0:");
		XRCCTRL(*this,"AssFileName",wxTextCtrl)->ChangeValue("");
		XRCCTRL(*this,"AssStore",wxButton)->Enable(false);
		XRCCTRL(*this, "AssStore", wxButton)->SetForegroundColour(*wxBLACK);
	}*/

//	assNew(0);

	if (status)
	{
		XRCCTRL(*this, "AssStore", wxButton)->Enable(false);
#ifdef __WXMAC__
        XRCCTRL(*this, "AssStore", wxButton)->SetBackgroundColour(wxNullColour);
#else
		XRCCTRL(*this, "AssStore", wxButton)->SetForegroundColour(*wxBLACK);
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

wxString Main::getApplicationDir()
{
	return applicationDirectory_;
}

wxChar Main::getPathSep()
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

void Main::zoomEvent(double zoom)
{
	if (!mode_.gui)
		return;
	if (zoomTextValueChanged_)
		zoomTextValueChanged_ = false;
	else
	{
		wxString zoomStr;
		zoomStr.Printf("%2.2f", zoom);
		XRCCTRL(*this, "ZoomValue"+computerInfo[runningComputer_].gui, wxTextCtrl)->ChangeValue(zoomStr);
	}
}

void Main::zoomEventVt(double zoom)
{
	if (!mode_.gui)
		return;
	if (zoomTextValueChanged_)
		zoomTextValueChanged_ = false;
	else
	{
		wxString zoomStr;
		zoomStr.Printf("%2.2f", zoom);
		XRCCTRL(*this, "ZoomValueVt"+computerInfo[runningComputer_].gui, wxTextCtrl)->ChangeValue(zoomStr);
	}
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

void Main::vuTimeout(wxTimerEvent&WXUNUSED(event))
{
	switch (runningComputer_)
	{
		case COSMICOS: 
		case ELF: 
		case ELFII:
		case SUPERELF:
		case COMX:
		case VIP: 
        case VIPII:
        case VELF:
		case TMC600:
		case TMC1800:
		case TMC2000:
		case PECOM:
		case ETI:
		case NANO:
			vuSet("Vu"+computerInfo[runningComputer_].gui, p_Computer->getGaugeValue());
		break;
	}

	if (selectedComputer_ == DEBUGGER)
	{
		switch (debuggerChoice_)
		{
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
							if (elfConfiguration[runningComputer_].usePager)
							{
								XRCCTRL(*this, "DebugPager", HexEdit)->changeNumber(p_Computer->getPager(portExtender_));
								XRCCTRL(*this, "DebugPortExtender", HexEdit)->changeNumber(portExtender_);
							}
							if (elfConfiguration[runningComputer_].useEms)
								XRCCTRL(*this, "DebugEmsPage", HexEdit)->changeNumber(p_Computer->getEmsPage());
						break;
					}
					updateSlotinfo_ = false;
				}

				if (updateMemory_ && memoryDisplay_ != CPU_TYPE)
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

void Main::ledTimeout(wxTimerEvent&WXUNUSED(event))
{
	p_Computer->ledTimeout();
}

void Main::updateCheckTimeout(wxTimerEvent&WXUNUSED(event))
{
	bool checkForUpdate;
	configPointer->Read("/Main/Check_For_Update", &checkForUpdate, true);

	if (checkForUpdate)
	{
		if (p_Main->checkUpdateEmma())
		{
			if (p_Main->updateEmma())
				Destroy();
		}
	}
}

void Main::cpuTimeout(wxTimerEvent&WXUNUSED(event))
{
	if (selectedComputer_ == DEBUGGER && debuggerChoice_ == MESSAGETAB)
		showTime();

}

void Main::startTime()
{
	startTime_ = wxGetLocalTime();
}

void Main::showTime()
{
	wxString print_buffer;
	int h,m,s;
	double f1,f2;
	float videoFreq;
	time_t endTime;

	long cpuCycles = p_Computer->getCpuCycles();
	if (cpuCycles != 0)
	{
		endTime = wxGetLocalTime();
		s = (int)(endTime - startTime_);
		h = s / 3600;
		s -= (h * 3600);
		m = s / 60;
		s -= (m * 60);
		f2 = endTime - startTime_;
		f1 = cpuCycles;
		f1 /= f2;
		f1 = f1 / 1000000 * 8;

		print_buffer.Printf("%ld", cpuCycles);
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

		print_buffer.Printf("%6.3f MHz",f1);
#if wxCHECK_VERSION(2, 9, 0)
		XRCCTRL(*this, "EffectiveClock", wxStaticText)->SetLabelText(print_buffer);
#else
		XRCCTRL(*this, "EffectiveClock", wxStaticText)->SetLabel(print_buffer);
#endif

		if (p_Video != NULL)
		{
			long videoSyncCount = p_Video->getVideoSyncCount();
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
}

void Main::vuSet(wxString item, int gaugeValue)
{
	if (gaugeValue == oldGauge_)
		return;

	oldGauge_ = gaugeValue;

	wxBitmap vu(100, VU_HI, 24);
	wxMemoryDC dcVu;

	dcVu.SelectObject(vu);
	dcVu.SetPen(wxPen(wxColour(0, 0xc0, 0)));
	dcVu.SetBrush(wxBrush(wxColour(0, 0xc0, 0)));

	int gaugeGreen = gaugeValue / 100;
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

	int gaugeRed = gaugeValue / 100;
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

	setTapeState(tapeState);
}

void Main::eventSetTapeState(int tapeState)
{
    guiEvent event(GUI_MSG, SET_TAPE_STATE);
    event.SetEventObject( p_Main );

    event.SetInt(tapeState);

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

void Main::ShowMessageBoxEvent(guiEvent&event)
{
    wxString message = event.GetString();
    wxString caption = event.GetStringValue2();
    int style = event.GetInt();
    
    p_Main->setMessageBoxAnswer(wxMessageBox(message, caption, style));
}

int Main::eventShowMessageBox(wxString message, wxString caption, int style)
{
#if defined (__WXMAC__)
    messageBoxAnswer_ = wxMessageBox(message, caption, style);
#else
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
            p_Computer->sleepComputer(1);
    }
#endif
    return messageBoxAnswer_;
}

void Main::setMessageBoxAnswer(int answer)
{
    messageBoxAnswer_ = answer;
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
    }
    return fileSelectorAnswer_;
}

void Main::setFileSelectorAnswer(wxString answer)
{
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

void Main::showChip8Register(int variable, int value)
{
	wxString idReference;
	idReference.Printf("V%01X", variable);

	wxString valueStr;
	valueStr.Printf("%02X", value);

	XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue(valueStr);
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
    bool status = event.GetBoolValue();
	if (p_Video != NULL)
		p_Video->setFullScreen(status);
}

void Main::eventVideoSetFullScreen(bool state)
{
    guiEvent event(GUI_MSG, SET_VIDEO_FULLSCREEN);
    event.SetEventObject( p_Main );

    event.SetBoolValue(state);

	GetEventHandler()->AddPendingEvent(event);
}

void Main::setVtFullScreenEvent(guiEvent&event)
{
    bool status = event.GetBoolValue();
	if (p_Vt100 != NULL)
		p_Vt100->setFullScreen(status);
}

void Main::eventVtSetFullScreen(bool state)
{
    guiEvent event(GUI_MSG, SET_VT_FULLSCREEN);
    event.SetEventObject( p_Main );

    event.SetBoolValue(state);

	GetEventHandler()->AddPendingEvent(event);
}

void Main::setChangeNoteBookEvent(guiEvent& WXUNUSED(event))
{
	XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(COMXTAB);
	XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(DEBUGGERTAB);
	XRCCTRL(*this, GUICOMPUTERNOTEBOOK, wxNotebook)->SetSelection(configPointer->Read("/Main/Selected_Tab", 0l));
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

	XRCCTRL(*this, "TextStartTmc600", wxStaticText)->Enable(true);
	XRCCTRL(*this, "TextStartTmc600", wxStaticText)->Enable(false);
	XRCCTRL(*this, "TextEndTmc600", wxStaticText)->Enable(true);
	XRCCTRL(*this, "TextEndTmc600", wxStaticText)->Enable(false);
	XRCCTRL(*this, "TextExecTmc600", wxStaticText)->Enable(true);
	XRCCTRL(*this, "TextExecTmc600", wxStaticText)->Enable(false);

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
    
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
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
		case VICTORY:
			keysFound = loadKeyDefinition("", "studiodefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
        break;
            
        case VISICOM:
            keysFound = loadKeyDefinition("", "studiodefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
        break;
            
        case VIP:
            if (p_Main->getVipVp590() || p_Main->getVipVp580())
                keysFound = loadKeyDefinition("", "vipdefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
            else
                keysFound = loadKeyDefinition("", "vipiidefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
        break;

        case ELF:
        case ELFII:
        case SUPERELF:
        case ELF2K:
        case COSMICOS:
            keysFound = loadKeyDefinition("", "elfdefault", keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");
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
            
            case VIP:
            case ELF:
            case ELFII:
            case SUPERELF:
            case ELF2K:
            case ETI:
            case TMC1800:
            case TMC2000:
            case NANO:
            case COSMICOS:
            case VIPII:
            case VELF:
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

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
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

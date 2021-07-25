#ifndef MAIN_H
#define MAIN_H

//#ifdef  WIN32
typedef unsigned char Byte;
typedef unsigned short Word;
//#endif

#include "wx/file.h"
#include "wx/spinctrl.h"
#include "wx/choicebk.h"
#include "vector"
#include "wx/listctrl.h"
#include "wx/html/helpctrl.h"

class MyHtmlHelpController : public wxHtmlHelpController
{
public:
    MyHtmlHelpController(int style=wxHF_DEFAULT_STYLE, wxWindow *parentWindow=NULL)
    : wxHtmlHelpController(style, parentWindow) {}
    MyHtmlHelpController(wxWindow *parentWindow, int style=wxHF_DEFAULT_STYLE)
    : wxHtmlHelpController(parentWindow, style) {}
protected:
    virtual wxWindow* CreateHelpWindow()
    {
        wxHtmlHelpController::CreateHelpWindow();
        
        m_helpWindow->Bind(wxEVT_HTML_LINK_CLICKED, &MyHtmlHelpController::OnHtmlLinkClicked);
        
        return m_helpWindow;
    }
private:
    static void OnHtmlLinkClicked(wxHtmlLinkEvent& event)
    {
        const wxString href = event.GetLinkInfo().GetHref();
        
        if ( href.StartsWith("http://") || href.StartsWith("https://"))
            wxLaunchDefaultBrowser(href);
        else
            event.Skip(true);
    }
};

// code defining event

class wxErrorMsgEvent: public wxNotifyEvent
{
public:
    wxErrorMsgEvent( wxEventType commandType = wxEVT_NULL, int id = 0 );

    // accessors
    wxString GetMsg()
        { return message; }
	void SetMsg(wxString msg)
		{ message = msg;}
    // required for sending with wxPostEvent()
	wxEvent *Clone(void) const { return new wxErrorMsgEvent(*this); }

private:
    wxString   message;
};

DECLARE_EVENT_TYPE( wxEVT_ERROR_MSG, -1 )

typedef void (wxEvtHandler::*wxErrorMsgEventFunction)(wxErrorMsgEvent&);

#define EVT_ERROR_MSG(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_ERROR_MSG, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( wxErrorMsgEventFunction, & fn ), (wxObject *) NULL ),

class guiEvent: public wxThreadEvent
{
public:
    guiEvent( wxEventType commandType = wxEVT_NULL, int id = 0 )
	: wxThreadEvent( commandType, id )
	{
	}

    // accessors
    bool GetBoolValue()            { return boolValue_;}
    bool GetBoolValue1()            { return boolValue1_;}
    double GetDoubleValue()        { return doubleValue_;}
    wxCoord GetCoord1()        { return wxCoord1_;}
    wxCoord GetCoord2()        { return wxCoord2_;}
    wxCoord GetCoord3()        { return wxCoord3_;}
    wxCoord GetCoord4()        { return wxCoord4_;}
    wxCoord GetCoord5()        { return wxCoord5_;}
    wxCoord GetCoord6()        { return wxCoord6_;}
    Word GetWordValue1()        { return wordValue1_;}
    Word GetWordValue2()		{ return wordValue2_;}
    Word GetWordValue3()		{ return wordValue3_;}
    Byte GetByteValue1()		{ return byteValue1_;}
    Byte GetByteValue2()		{ return byteValue2_;}
    wxString GetStringValue2()	{ return stringValue2_;}
    wxString GetStringValue3()	{ return stringValue3_;}
    wxString GetStringValue4()	{ return stringValue4_;}
    wxString GetStringValue5()    { return stringValue5_;}
    wxSize GetSizeValue()    { return sizeValue_;}
    void SetBoolValue(bool boolValue)    { boolValue_ = boolValue;}
    void SetBoolValue1(bool boolValue)    { boolValue1_ = boolValue;}
    void SetDoubleValue(double doubleValue)    { doubleValue_ = doubleValue;}
    void SetCoord1(wxCoord coordValue)    { wxCoord1_ = coordValue;}
    void SetCoord2(wxCoord coordValue)    { wxCoord2_ = coordValue;}
    void SetCoord3(wxCoord coordValue)    { wxCoord3_ = coordValue;}
    void SetCoord4(wxCoord coordValue)    { wxCoord4_ = coordValue;}
    void SetCoord5(wxCoord coordValue)    { wxCoord5_ = coordValue;}
    void SetCoord6(wxCoord coordValue)    { wxCoord6_ = coordValue;}
    void SetWordValue1(Word wordValue)    { wordValue1_ = wordValue;}
    void SetWordValue2(Word wordValue)	{ wordValue2_ = wordValue;}
    void SetWordValue3(Word wordValue)	{ wordValue3_ = wordValue;}
    void SetByteValue1(Byte byteValue)	{ byteValue1_ = byteValue;}
    void SetByteValue2(Byte byteValue)	{ byteValue2_ = byteValue;}
    void SetStringValue2(wxString stringValue2)	{ stringValue2_ = stringValue2;}
    void SetStringValue3(wxString stringValue3)	{ stringValue3_ = stringValue3;}
    void SetStringValue4(wxString stringValue4)	{ stringValue4_ = stringValue4;}
    void SetStringValue5(wxString stringValue5)    { stringValue5_ = stringValue5;}
    void SetSizeValue(wxSize sizeValue)    { sizeValue_ = sizeValue;}

    // required for sending with wxPostEvent()
	wxEvent *Clone(void) const { return new guiEvent(*this); }

private:
    bool boolValue_;
    bool boolValue1_;
    double doubleValue_;
    wxCoord wxCoord1_;
    wxCoord wxCoord2_;
    wxCoord wxCoord3_;
    wxCoord wxCoord4_;
    wxCoord wxCoord5_;
    wxCoord wxCoord6_;
    Word wordValue1_;
	Word wordValue2_;
	Word wordValue3_;
	Byte byteValue1_;
	Byte byteValue2_;
    wxString stringValue2_;
    wxString stringValue3_;
    wxString stringValue4_;
    wxString stringValue5_;
    wxSize sizeValue_;
};

wxDEFINE_EVENT(GUI_MSG, guiEvent);

#define guiEventHandler(func) (&func)

#define EVT_GUI_MSG(id, func) \
    wx__DECLARE_EVT1(GUI_MSG, id, (&func))

/*class guiEvent: public wxEvent
{
public:
    guiEvent( wxEventType commandType = wxEVT_NULL, int id = 0 )
	: wxEvent( commandType, id )
	{
	}

    // accessors
    int GetInt()			{ return intValue_;}
    bool GetBoolValue()			{ return boolValue_;}
    Word GetWordValue1()		{ return wordValue1_;}
    Word GetWordValue2()		{ return wordValue2_;}
    Word GetWordValue3()		{ return wordValue3_;}
    Byte GetByteValue1()		{ return byteValue1_;}
    Byte GetByteValue2()		{ return byteValue2_;}
    wxString GetString()	{ return stringValue1_;}
    wxString GetStringValue2()	{ return stringValue2_;}
    void SetInt(int intValue)	{ intValue_ = intValue;}
    void SetBoolValue(bool boolValue)	{ boolValue_ = boolValue;}
    void SetWordValue1(Word wordValue)	{ wordValue1_ = wordValue;}
    void SetWordValue2(Word wordValue)	{ wordValue2_ = wordValue;}
    void SetWordValue3(Word wordValue)	{ wordValue3_ = wordValue;}
    void SetByteValue1(Byte byteValue)	{ byteValue1_ = byteValue;}
    void SetByteValue2(Byte byteValue)	{ byteValue2_ = byteValue;}
    void SetString(wxString stringValue1)	{ stringValue1_ = stringValue1;}
    void SetStringValue2(wxString stringValue2)	{ stringValue2_ = stringValue2;}

    // required for sending with wxPostEvent()
	wxEvent *Clone(void) const { return new guiEvent(*this); }

private:
	int intValue_;
	bool boolValue_;
	Word wordValue1_;
	Word wordValue2_;
	Word wordValue3_;
	Byte byteValue1_;
	Byte byteValue2_;
	wxString stringValue1_;
	wxString stringValue2_;
};

DECLARE_EVENT_TYPE( wxEVT_GUI_MSG, -1 )

typedef void (wxEvtHandler::*guiEventFunction)(guiEvent&);

#define EVT_GUI_MSG(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_GUI_MSG, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxEventFunction) \
    wxStaticCastEvent( guiEventFunction, & fn ), (wxObject *) NULL ),*/

class Main;

class UpdateCheckThread : public wxThread
{
public:
    UpdateCheckThread(Main *handler)
    : wxThread(wxTHREAD_DETACHED)
    { m_pHandler = handler; }
    ~UpdateCheckThread();
protected:
    virtual ExitCode Entry();
    Main *m_pHandler;
};

#define SET_LOCATION 1
#define SET_SW_NAME 2
#define SET_TAPE_STATE 3
#define SET_TEXT_VALUE 4
#define SET_CHECK_BOX 5
#define PRINT_DEFAULT 6
#define PRINT_PARALLEL 7
#define PRINT_PARALLEL_FINISHED 8
#define PRINT_THERMAL 9
#define PRINT_THERMAL_FINISHED 10
#define PRINT_THERMAL_CYCLE 11
#define PRINT_SERIAL 12
#define PRINT_SERIAL_FINISHED 13
#define PRINT_PECOM 14
#define SET_FM_GUI 16
#define SET_SAVE_START 17
#define SET_SAVE_END 18
#define ENABLE_MEM_ACCESS 19
#define SET_VIDEO_FULLSCREEN 20
#define SET_VT_FULLSCREEN 21
#define CHANGE_NOTEBOOK 22
#define DISABLE_CONTROLS 23
#define DEBUG_TRACE 24
#define UPDATE_TITLE 25
#define SHOW_MESSAGE_BOX 26
#define SHOW_FILE_SELECTOR 27
#define SHOW_MESSAGE 28
#define SHOW_ADDRESS_POPUP 29
#define SHOW_TEXT_MESSAGE 30
#define DEBOUNCE_TIMER 31
#define SET_STATIC_TEXT_VALUE 32
#define ZOOM_CHANGE 33
#define ZOOMVT_CHANGE 34
#define GET_CLIENT_SIZE 35
#define SET_CLIENT_SIZE 36
#define REFRESH_VIDEO 37
#define REFRESH_PANEL 38
#define EVENT_ZOOM 39

#define OS_WINDOWS_2000 0
#define OS_WINDOWS_XP 1
#define OS_WINDOWS 2
#define OS_LINUX 10
#define OS_MAC 20
#define OS_MAC_PRE_10_9 21

#define OS_MAJOR_XP_2000 5
#define OS_MAJOR_VISTA_8_1 6
#define OS_MAJOR_10 10

#define OS_MINOR_2000 0
#define OS_MINOR_XP 1
#define OS_MINOR_VISTA 0
#define OS_MINOR_7 1
#define OS_MINOR_8 2
#define OS_MINOR_8_1 3
#define OS_MINOR_10 0

#define FRED_HEX_MODE 0
#define FRED_BYTE_MODE 1
#define FRED_HEX_PULSE_MODE 2

#define MICROKEY_VIP601 0
#define MICROKEY_COMX 1

class WindowInfo
{
public:
    int mainwX, mainwY;
	int xBorder, yBorder;
	int xBorder2, yBorder2;
	int xPrint;
	int operatingSystem;
    
    int clockTextCorrectionSingleTabX;
    int clockCorrectionSingleTabX;
    int mhzTextCorrectionSingleTabX;
    int stopCorrectionSingleTabX;
    int startCorrectionSingleTabX;
    int clockTextCorrectionX;
    int clockCorrectionX;
    int mhzTextCorrectionX;
    int stopCorrectionX;
    int startCorrectionX;
    
    int clockTextCorrectionSingleTabY;
    int clockCorrectionSingleTabY;
    int mhzTextCorrectionSingleTabY;
    int stopCorrectionSingleTabY;
    int startCorrectionSingleTabY;
    int clockTextCorrectionY;
    int clockCorrectionY;
    int mhzTextCorrectionY;
    int stopCorrectionY;
    int startCorrectionY;
    
    int floatHeight;
    int startHeight;

    int clockSize;

    int red;
    int green;
    int blue;
    
    int ledPosX1;
    int ledPosX2;
    int ledSpacing;
    int ledPosY;
    int ledPosDiagY;
    int ledPosVip2Y;
    
    int statusBarElementMeasure[5];

    wxString statusBarLeader;
    wxString statusBarLeaderCidelsa;

    bool packageDeb;
    
    wxString errorMessage;
};

class Mode
{
	public: 
	bool window_position_fixed;
	bool gui;
	bool verbose;
	bool portable;
	bool full_screen;
	bool run;
	bool load;
	bool update_check;
};

class FloatEdit : public wxTextCtrl 
{ 
public: 
	FloatEdit();
	FloatEdit(wxWindow *parent, wxWindowID id, const wxString& value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual void OnChar(wxKeyEvent& event); 

private:
	wxString comma_;
	DECLARE_DYNAMIC_CLASS(FloatEdit) 
}; 

class IntEdit : public wxTextCtrl 
{ 
public: 
	IntEdit();
	virtual void OnChar(wxKeyEvent& event); 

private:
	DECLARE_DYNAMIC_CLASS(IntEdit) 
}; 

class CheckBoxListCtrl : public wxListCtrl 
{ 
public: 
	CheckBoxListCtrl();
	void onMouseRelease(wxMouseEvent& event);

private:
	DECLARE_DYNAMIC_CLASS(CheckBoxListCtrl) 
}; 

class SlotEdit : public wxTextCtrl 
{ 
public: 
	SlotEdit();
	virtual void OnChar(wxKeyEvent& event); 
	void setRange(int min, int max); 
	void changeNumber(int number);
	void saveNumber(int number);

private:
	int min_;
	int max_;
	int number_;

	DECLARE_DYNAMIC_CLASS(SlotEdit) 
}; 

class HexEdit : public wxTextCtrl 
{ 
public: 
	HexEdit();
	virtual void OnChar(wxKeyEvent& event); 
	void setStart(int min); 
	void changeNumber(int number);
	void saveNumber(int number);

private:
	int min_;
	int number_;

	DECLARE_DYNAMIC_CLASS(HexEdit) 
}; 

class HexEditX : public wxTextCtrl 
{ 
public: 
	HexEditX();
	virtual void OnChar(wxKeyEvent& event); 

private:
	DECLARE_DYNAMIC_CLASS(HexEditX) 
}; 

class MemEdit : public wxTextCtrl 
{ 
public: 
	MemEdit();
	virtual void OnChar(wxKeyEvent& event); 
	void changeNumber1X(int number);
	void changeNumber2X(int number);
	void saveNumber(int number);

private:
	int number_;

	DECLARE_DYNAMIC_CLASS(MemEdit) 
}; 
class ScreenInfo
{
public:
	int start;
	int number;
	wxString defaultColour[67];
	int numberVideo;
	int borderX[10];
	int borderY[10];
};

class ConfigurationInfo
{
public:
    wxString menuName;
    wxString subMenuName;
    wxString fileName;
};

#include "cidelsa.h"
#include "comx35.h"
#include "cdp18s020.h"
#include "cdp18s600.h"
#include "hbelf.h"
#include "member.h"
#include "uc1800.h"
#include "microtutor.h"
#include "microtutor2.h"
#include "elf2.h"
#include "elf2k.h"
#include "ms2000.h"
#include "mcds.h"
#include "cosmicos.h"
#include "super.h"
#include "studio2.h"
#include "studioiv.h"
#include "coinarcade.h"
#include "fred.h"
#include "visicom.h"
#include "victory.h"
#include "vip.h"
#include "vip2.h"
#include "vip2k.h"
#include "velf.h"
#include "pecom.h"
#include "tmc600.h"
#include "tmc1800.h"
#include "tmc2000.h"
#include "eti660.h"
#include "nano.h"
#include "guicomx.h"
#include "debug.h"
#include "video.h"
#include "serial.h"

#define EMMA_VERSION 1.40
#define EMMA_SUB_VERSION 6
#define ELF 0
#define ELFII 1
#define SUPERELF 2
#define ELF2K 3
#define COSMICOS 4
#define MEMBER 5
#define VIP 6
#define VIP2K 7
#define VELF 8
#define MICROTUTOR 9
#define MICROTUTOR2 10
#define UC1800 11
#define CDP18S020 12
#define MICROBOARD 13
#define LAST_LED_COMPUTER 13
#define MS2000 14
#define MCDS 15
#define FRED1 16
#define FRED1_5 17
#define COMX 18
#define STUDIO 19
#define ETI 20
#define CIDELSA 21
#define TMC600 22
#define TMC1800 23
#define TMC2000 24
#define NANO 25
#define PECOM 26
#define VISICOM 27
#define VICTORY 28
#define VIPII 29
#define COINARCADE 30
#define STUDIOIV 31
#define DEBUGGER 32

#define TELMACPRINTER 0
#define PECOMPRINTER 3
#define VIPPRINTER 4
#define ELFPRINTER 5
#define MS2000PRINTER 6
#define COMXPRINTER 1
#define COMXTHPRINTER 2
#define COMXFLOP 3
#define COMX80COLUMN 4
#define NETWORK 6
#define COMXRAM 7
#define COMXJOY 16
#define COMXRS232 17
#define COMXSUPERBOARD 0x21
#define COMXEPROMBOARD 0x73
#define COMXDIAG 0xC2
#define COMXEMPTY 255
#define PRINTFILE 0
#define PRINTWINDOW 1
#define NOPRINTER 2
#define COMXPRINTPLOTTER 1
#define COMXPRINTPRINTER 2
#define TIMERINTERVAL 50
#define PRINTER_PLOTTER 6000
#define PLOTTEREXTTEXT 6001
#define PLOTTEREXT 6002
#define PLOTTERROMTEXT 6003
#define PLOTTERROM 6004
#define PRINTERROWS 6005
#define PRINTERFONT 6006
#define PAL 0
#define NTSC 1
#define COMXTAB 0
#define COSMACELFTAB 1
#define RCATAB 2
#define STUDIOTAB 3
#define CIDELSATAB 4
#define TELMACTAB 5
#define PECOMTAB 6
#define ETITAB 7
#define DEBUGGERTAB 8
#define DISKNONE 0
#define DISKFDC 1
#define DISKIDE 2

#define ELF2KTAB 0
#define COSMICOSTAB 1
#define ELFTAB 2
#define ELFIITAB 3
#define SUPERELFTAB 4
#define MEMBERTAB 5
#define VIP2KTAB 6
#define VELFTAB 7
#define UC1800TAB 8
#define LASTELFTAB 8

#define FRED1TAB 0
#define FRED2TAB 1
#define MICROTUTORTAB 2
#define MICROTUTOR2TAB 3
#define CDP18S020TAB 4
#define VIPTAB 5
#define VIPIITAB 6
#define MICROBOARDTAB 7
#define MCDSTAB 8
#define MS2000TAB 9
#define LASTRCATAB 9

#define COINARCADETAB 0
#define STUDIOIITAB 1
#define VICTORYTAB 2
#define STUDIOIVTAB 3
#define VISICOMTAB 4
#define LASTSTUDIOTAB 4

#define TMC600TAB 0
#define TMC1800TAB 1
#define TMC2000TAB 2
#define NANOTAB 3
#define LASTTELMACTAB 3

#define MESSAGETAB 0
#define DIRECTASSTAB 1
#define PROFILERTAB 2
#define TRACETAB 3
#define CHIP8TAB 4
#define MEMORYTAB 5
#define LASTDEBUGGERTAB 5

#define MICROBOARD_CDP18S600 0
#define MICROBOARD_CDP18S601 1
#define MICROBOARD_CDP18S602 2
#define MICROBOARD_CDP18S603 3
#define MICROBOARD_CDP18S603A 4
#define MICROBOARD_CDP18S604B 5
#define MICROBOARD_CDP18S605 6
#define MICROBOARD_CDP18S606 7
#define MICROBOARD_CDP18S607 8
#define MICROBOARD_CDP18S608 9
#define MICROBOARD_CDP18S609 10
#define MICROBOARD_CDP18S610 11
#define RCASBC 12
#define MICROBOARD_LAST 12

#define VIDEONONE 0
#define VIDEO1870 0
#define VIDEO80COL 2
#define VIDEOMICROVT 1

#define VIDEOVT 0
#define VIDEOPIXIE 1
#define VIDEO2KI8275 2

//#define VIDEOVT 0
//#define VIDEOPIXIE 1
#define VIDEO6845 2
#define VIDEO6847 3
#define VIDEOTMS 4
#define VIDEOI8275 5
#define VIDEOS100 6

#define ELFLED 0
#define ELFIILED 1
#define SUPERELFLED 2
#define ELF2KLED1 3
#define ELF2KLED2 4
#define ELF2KLED3 5
#define COSMICOSLED 6
#define MEMBERLED 7
#define MEMBERLEDGREEN 8
#define PIOLED 9

#define KEYBOARDNONE 0
#define KEYBOARD_PS2 1
#define KEYBOARD_PS2_INT 2
#define KEYBOARD_ASCII 3
#define KEYBOARD_ASCII_HEX_EF 4
#define KEYBOARD_HEX 5

#define KEYBOARD2KPS2GPIOJP4 2
#define KEYBOARDELF2KHEX 3

#define KEYBOARD_HEXCOSMICOS 1

#define MEM_64K 0
#define MEM_EMS 1
#define MEM_MAP 2
#define MEM_ROM_MAP 3

#define COMXPARALLEL 0
#define COMXSERIAL 1
#define COMXTHERMAL 2

#define UNDEFINED 0
#define RAM 1
#define ROM 2
#define RAMROM 3
#define PAGER 3
#define CRAM1870 5
#define PRAM1870 6
#define COMXEXPROM 7
#define COPYCOMXEXPROM 8
#define RAMBANK 9
#define MC6845RAM 11
#define MC6845REGISTERS 12
#define COPYFLOPROM 13
#define EMSMEMORY 15
#define COMXEXPBOX 16
#define MC6847RAM 18
#define CARTRIDGEROM 19
#define MAPPEDRAM 20
#define COLOURRAM 21
#define VP570RAM 22 
#define EPROMBANK 23
#define SUPERBANK 24
#define NVRAM 24
#define ROMMAPPER 25
#define MULTICART 26
#define DIAGROM 27
#define MAPPEDROM 28
#define MAPPEDMULTICART 29
#define TESTCARTRIDGEROM 30
#define REGSTORAGE 31
#define CPURAM 32
#define UART1_82C51 33
#define UART2_82C51 34
#define NOCHANGE 35

#define MICRO_ROM 0
#define MICRO_RAM 1
#define MICRO_CPURAM 2

#define SHOWNAME true
#define NONAME false
#define SET_SPIN true
#define DO_NOT_SET_SPIN false

#define HEX_UP 0
#define HEX_LEFT 1
#define HEX_RIGHT 2
#define HEX_DOWN 3
#define HEX_FIRE 4
#define HEX_UP_LEFT 5
#define HEX_UP_RIGHT 6
#define HEX_DOWN_LEFT 7
#define HEX_DOWN_RIGHT 8

#define NOPROGRAM 0
#define SUPERBASICV1 1 // bit 0=1: .super/.rca load
#define SUPERBASICV3 3 
#define SUPERBASICV5 5 
#define SUPERBASICV6 7 
#define RCABASIC3 9
#define RCABASIC4 11
#define FPBBASIC 13
#define FPBBOOT 14
#define FPBBASIC_AT_8000 15
#define VIPTINY 17

#define TINYBASIC 2
#define MINIMON 4
#define GOLDMON 6
#define MONITOR_CHUCK_LOW 8
#define MONITOR_CHUCK_HIGH 10
#define UT4 12
#define UT62 14
#define UT63 16
#define UT71 18

#define BASICADDR_KEY 0
#define BASICADDR_READY 1
#define BASICADDR_KEY_VT_RESTART 2
#define BASICADDR_KEY_VT_INPUT 3

#define BASICADDR_KEY_SB1 0x2625
#define BASICADDR_READY_SB1 0x3f
#define BASICADDR_RUN_SB1 0x16a3
#define BASICADDR_CALL_SB1 0x1082
#define BASICADDR_PSAVE_SB1 0x0107
#define BASICADDR_PLOAD_SB1 0x0101
#define BASICADDR_DSAVE_SB1 0x010a
#define BASICADDR_DLOAD_SB1 0x0104
#define BASICADDR_ROUTINE1_SB1 0x2a9f
#define BASICADDR_ROUTINE2_SB1 0x35a3
#define BASICADDR_VT_RESTART_SB1 0x2880
#define BASICADDR_VT_INPUT_SB1 0x71

#define BASICADDR_KEY_SB3 0x2e09
#define BASICADDR_READY_SB3 0x37
#define BASICADDR_RUN_SB3 0x0cdc
#define BASICADDR_CALL_SB3 0x03d8
#define BASICADDR_PSAVE_SB3 0x2b06
#define BASICADDR_PLOAD_SB3 0x2b00
#define BASICADDR_DSAVE_SB3 0x2b09
#define BASICADDR_DLOAD_SB3 0x2b03
#define BASICADDR_ROUTINE1_SB3 0x2f9f
#define BASICADDR_ROUTINE2_SB3 0x2fa3
#define BASICADDR_VT_RESTART_SB3 -1
#define BASICADDR_VT_INPUT_SB3 -1

#define BASICADDR_KEY_SB5 0x3309
#define BASICADDR_READY_SB5 0x143
#define BASICADDR_RUN_SB5 0x1691
#define BASICADDR_CALL_SB5 0x0900
#define BASICADDR_PSAVE_SB5 0x0E06
#define BASICADDR_PLOAD_SB5 0x0E00
#define BASICADDR_DSAVE_SB5 0x0E09
#define BASICADDR_DLOAD_SB5 0x0E03
#define BASICADDR_ROUTINE1_SB5 0x359f
#define BASICADDR_ROUTINE2_SB5 0x35a3
#define BASICADDR_VT_RESTART_SB5 0x34a3
#define BASICADDR_VT_INPUT_SB5 0x3428

#define BASICADDR_KEY_SB6 0x3109
#define BASICADDR_READY_SB6 0x143
#define BASICADDR_RUN_SB6 0x1d00
#define BASICADDR_CALL_SB6 0x1b00
#define BASICADDR_PSAVE_SB6 0x03f1
#define BASICADDR_PLOAD_SB6 0x03eb
#define BASICADDR_DSAVE_SB6 0x03f4
#define BASICADDR_DLOAD_SB6 0x03ee
#define BASICADDR_ROUTINE1_SB6 0x359f
#define BASICADDR_ROUTINE2_SB6 0x35a3
#define BASICADDR_VT_RESTART_SB6 -1
#define BASICADDR_VT_INPUT_SB6 -1

#define BASICADDR_READY_RCA3 0x2257
#define BASICADDR_RUN_RCA3 0x3276
#define BASICADDR_CALL_RCA3 -1
#define BASICADDR_ROUTINE1_RCA3 0x529f
#define BASICADDR_ROUTINE2_RCA3 0x52a3

#define BASICADDR_READY_RCA4 0x2257
#define BASICADDR_RUN_RCA4 0x3276
#define BASICADDR_CALL_RCA4 -1
#define BASICADDR_ROUTINE1_RCA4 0x529f
#define BASICADDR_ROUTINE2_RCA4 0x52a3

#define BASICADDR_READY_MCDS 0xb053
#define BASICADDR_RUN_MCDS 0xc076
#define BASICADDR_CALL_MCDS 0xc79f
#define BASICADDR_ROUTINE1_MCDS 0x9f
#define BASICADDR_ROUTINE2_MCDS 0xa3

#define BASICADDR_VT_RESTART_RCA 0xfc19
#define BASICADDR_VT_INPUT_RCA 0xfc98

#define NOOS 0
#define ELFOS 1

#define HEXMON 2
#define ASCIIMON 4

#define QSOUNDOFF 0
#define QSOUNDSW 1
#define QSOUNDEXT 2

#define VTNONE 0
#define VT52 1
#define VT100 2
#define EXTERNAL_TERMINAL 3

#define UART1 0
#define UART2 1

	// 1 Scroll - repeat - screen reverse - cursor block line
	// 2 bell - keyklick - ansi/vt52 - xon/xoff
	// 3 US - wrap - newline - interlace
	// 4 parity sense - parity - bits per char - 60/50Hz

#define VTLOCALECHO 16
#define VTSMOOTHSCROLL 15
#define VTREPEAT 14
#define VTREVERSESCREEN 13
#define VTCURSORBLOCK 12
#define VTBELL 11
#define VTKEYCLICK 10
#define VTANSI 9
#define VTAUTOXON 8
#define VTUSASCII 7
#define VTWRAPAROUND 6
#define VTNEWLINE 5
#define VTINTERLACE 4
#define VTPARITYSENSE 3
#define VTPARITY 2
#define VTBITS 1
#define VTPOWER 0

#define VIP_BEEP 0
#define VIP_1864 1
#define VIP_SUPER2 2
#define VIP_SUPER4 3

#define GUISAVEONEXIT "MI_SaveOnExit"
#define GUISAVECONFIG "MI_SaveConfig"
#define GUISAVECOMPUTERCONFIG "MI_SaveComputerConfig"
#define GUIDEFAULTWINDOWPOS "MI_DefaultWindowPosition"
#define GUIDEFAULTGUISIZE "MI_DefaultGuiSize"
#define GUIDEFAULT "MI_DefaultSettings"
#define GUIPROTECTEDMODE "ProtectedMode"

#define GUICOMPUTERNOTEBOOK "Computer"

#define RESETSTATE 0
#define RESETSTATECW 3
#define BASICSTATE 1
#define RUNSTATE 2
#define COMMAND_C 3

#define LEFTCHANNEL false
#define RIGHTCHANNEL true

#define GUI_ELF2K_BAUDR 30000
#define GUI_ELF2K_BAUDT 30001
#define GUI_ELF_BAUDR 30002
#define GUI_ELF_BAUDT 30003
//#define GUI_ELFII_BAUDR 30004
//#define GUI_ELFII_BAUDT 30005
//#define GUI_SUPERELF_BAUDR 30006
//#define GUI_SUPERELF_BAUDT 30007
#define GUI_COSMICOS_BAUDR 30008
#define GUI_COSMICOS_BAUDT 30009
#define GUI_MEMBER_BAUDR 30010
#define GUI_MEMBER_BAUDT 30011
#define GUI_VIP_BAUDR 30012
#define GUI_VIP_BAUDT 30013
#define GUI_VELF_BAUDR 30014
#define GUI_VELF_BAUDT 30015
#define GUI_MS2000_BAUDR 30016
#define GUI_MS2000_BAUDT 30017
#define GUI_MCDS_BAUDR 30018
#define GUI_MCDS_BAUDT 30019
#define GUI_CLOCK_TEXTCTRL 30020
#define GUI_START_BUTTON 30060
#define GUI_STOP_BUTTON 30100
#define GUI_CONFIG_MENU 10000
#define GUI_CONFIG_DELETE_MENU 20000

#define TAPE_STOP 0
#define TAPE_PLAY 1
#define TAPE_RECORD 2
#define TAPE_PAUSE 3
#define TAPE_PLAY1 4
#define TAPE_RECORD1 5

#define PRINT_BUFFER_SIZE 1000

#define CHIP_NONE 0
#define CHIP8 1
#define CHIP8X 2
#define CHIPETI 3
#define CHIPST2 4
#define CHIPFEL1 5
#define CHIPGPL1 6
#define CHIPGPL2 7
#define CHIPFEL3 8
#define CHIPFEL4 9
#define CHIPFEL5 10
#define CHIPFPL 11
#define CHIPSTIV 12
#define CARDTRAN 13

#define CPU_OVERRIDE_DEFAULT 0
#define CPU_OVERRIDE_CPU1801 1
#define CPU_OVERRIDE_SYSTEM00 2
#define SYSTEM00 1
#define CPU1801 2
#define CPU1802 3
#define CPU1804 4
#define CPU1805 5
#define PROFILERTYPELIN 0
#define PROFILERTYPELOG 1
#define PROFILER_OFF 0
#define PROFILER_ON 1

#define STARTUP_ZEROED 0
#define STARTUP_RANDOM 1
#define STARTUP_DYNAMIC 2

#define MEM_TYPE_DATA 0
#define MEM_TYPE_JUMP 1
#define MEM_TYPE_OPCODE 2
#define MEM_TYPE_OPERAND 3
#define MEM_TYPE_PSEUDO_1 6
#define MEM_TYPE_PSEUDO_2 7
#define MEM_TYPE_UNDEFINED 8
#define MEM_TYPE_JUMP_REV 9
#define MEM_TYPE_TEXT 10
#define MEM_TYPE_OPCODE_RSHR 170
#define MEM_TYPE_OPCODE_RSHL 171
#define MEM_TYPE_OPCODE_BPZ 172
#define MEM_TYPE_OPCODE_BGE 173
#define MEM_TYPE_OPCODE_BM 174
#define MEM_TYPE_OPCODE_BL 175
#define MEM_TYPE_OPCODE_LSKP 176
#define MEM_TYPE_OPCODE_RLDL 177
#define MEM_TYPE_OPCODE_SKP 178
#define MEM_TYPE_OPCODE_LBR_SLOT 179
#define MEM_TYPE_OPCODE_JUMP_SLOT 180
#define MEM_TYPE_OPCODE_LDV 181
#define MEM_TYPE_OPCODE_LDL 182
#define MEM_TYPE_OPERAND_LD_2 183
#define MEM_TYPE_OPERAND_LD_3 184
#define MEM_TYPE_OPERAND_LD_5 185
#define MEM_TYPE_OPCODE_LDL_SLOT 186

#define LABEL_TYPE_NONE 0
#define LABEL_TYPE_BRANCH 1
#define LABEL_TYPE_JUMP 2
#define LABEL_TYPE_SUB 3

#define IO_TYPE_N0 0
#define IO_TYPE_N1 1
#define IO_TYPE_N2 2

#define FRONT_TYPE_B 0
#define FRONT_TYPE_C 1
#define FRONT_TYPE_I 2
#define FRONT_TYPE_J 3

#define VIDEO 0
#define PIXIE 1

#define TIL311 0
#define TIL313 1

#define LAPTIME_OFF 0
#define LAPTIME_Q 1
#define LAPTIME_OUT 2

#define CALL_CHANGE_SCREEN_SIZE true
#define DON_T_CALL_CHANGE_SCREEN_SIZE false

#if defined(__WXMAC__)
#define IMAGES_FOLDER "images_osx"
#define CTRL_V wxMOD_CMD
#else
#define IMAGES_FOLDER "images"
#define CTRL_V wxMOD_CONTROL
#endif

#define TERM_HEX 1
#define TERM_BIN 2
#define TERM_XMODEM_LOAD 3
//#define TERM_XMODEM_LOAD_128 4
#define TERM_XMODEM_SAVE 5
//#define TERM_XMODEM_SAVE_128 6

class Emu1802: public wxApp
{
	virtual bool OnInit();
	virtual int OnExit();
    virtual int OnRun();
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
	void getSoftware(wxString computer, wxString type, wxString software);
	void checkXrc(wxString xrcFile);

private:
    wxLocale locale;
	wxConfigBase *configPointer;

	Mode mode_;
    int startComputer_;
	bool dataDirRelative_;
    
    wxString dataDir_;
    wxString workingDir_;
    wxString iniDirectory_;
    wxString pathSeparator_;
    wxString applicationDirectory_;
};

class Main: public DebugWindow
{
public:

	Main(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~Main();
    
    wxSize getPosition(wxString control, wxSize size);
    wxSize getDefaultGuiSize();
    void windowSizeChanged(wxSizeEvent& event);
    void adjustGuiSize();
	void pageSetup();
	void onClose(wxCloseEvent&event );

	bool checkUpdateEmma();
	bool updateEmma();
	wxString downloadString(wxString url);

	void onQuit(wxCommandEvent& event);
	void onAbout(wxCommandEvent& event);
    void onDataDir(wxCommandEvent& event);
    void onReInstallConfig(wxCommandEvent& event);
    void onReInstallData(wxCommandEvent& event);
    void removeRedundantFiles();
    void deleteDir(wxString directory);
    void reInstall(wxString source, wxString destination, wxString pathSep);
    void reInstallOnNotFound(int computerType, wxString fileTypeString);
    void checkAndReInstallMainRom(int computerType);
    void checkAndReInstallFile(int computerType, wxString fileTypeString, int fileType);
    void checkAndReInstallFile(wxString fileAndPath, int computerType, wxString fileTypeString);
    void checkAndReInstallCharFile(int computerType, wxString fileTypeString, int fileType);
    bool copyTree( wxFileName* source, wxFileName* destination, wxString pathSep);
    void onConfiguration(wxCommandEvent& event);
    void onDeleteConfiguration(wxCommandEvent& event);
	void onHome(wxCommandEvent& event);
	void onHomeSb(wxCommandEvent& event);
	void onHomeSbHs(wxCommandEvent& event);
	void onUpdateCheck(wxCommandEvent& event);
	void onUpdateEmma(wxCommandEvent& event);
	void onHelp(wxCommandEvent& event);
    void onSaveConfig(wxCommandEvent& event);
    void buildConfigMenu();
    void onConfigMenu(wxCommandEvent& event);
    int saveComputerConfig(ConfigurationInfo configurationInfo, ConfigurationInfo oldConfigurationInfo);
    ConfigurationInfo getMenuInfo(wxString fileName);
    void loadComputerConfig(wxString fileName);
	void onSaveOnExit(wxCommandEvent& event);
    void onDefaultWindowPosition(wxCommandEvent& event);
    void onDefaultGuiSize(wxCommandEvent& event);
	void onFixedWindowPosition(wxCommandEvent& event);
	void nonFixedWindowPosition();
	void fixedWindowPosition();
	void onFunctionKeys(wxCommandEvent& event);
	void onDefaultSettings(wxCommandEvent& event);
	void setDefaultSettings();
    void onSystem00(wxCommandEvent& event);
    void on1801(wxCommandEvent& event);
	void on1802(wxCommandEvent& event);
	void on1804(wxCommandEvent& event);
    void on1805(wxCommandEvent& event);
    void onCpuStartupRegistersZeroed(wxCommandEvent& event);
    void onCpuStartupRegistersRandom(wxCommandEvent& event);
    void onCpuStartupRamZeroed(wxCommandEvent& event);
    void onCpuStartupRamRandom(wxCommandEvent& event);
    void onCpuStartupRamDynamic(wxCommandEvent& event);
    void onCpuStartupVideoRamZeroed(wxCommandEvent& event);
    void onCpuStartupVideoRamRandom(wxCommandEvent& event);
    void onCpuStartupVideoRamDynamic(wxCommandEvent& event);
    void onKeyboardDutch(wxCommandEvent& event);
    void onKeyboardGerman(wxCommandEvent& event);
    void onKeyboardFrench(wxCommandEvent& event);
    void onKeyboardSwedish(wxCommandEvent& event);
    void onKeyboardUs(wxCommandEvent& event);
    void onKeyboardUserDefined(wxCommandEvent& event);
	void onFlat(wxCommandEvent& event);
	void onCrisp(wxCommandEvent& event);
	void onDefault(wxCommandEvent& event);
	void onTvSpeaker(wxCommandEvent& event);
	void onHandheld(wxCommandEvent& event);
	void onChar(wxKeyEvent&event);
	void onKeyDown(wxKeyEvent&event);
	void onWheel(wxMouseEvent&event);
	bool checkFunctionKey(wxKeyEvent& event);
	void onActivateMainWindow(wxCommandEvent& event);
	void activateMainWindow();
    void onFullScreenMenu(wxCommandEvent& event);
    void onUseNumPad(wxCommandEvent& event);
	void fullScreenMenu();
	void popUp();
	void onKeyUp(wxKeyEvent&event);
	void connectKeyEvent(wxWindow* pclComponent);
	bool runPressed() {return runPressed_;};

	void onComputer(wxNotebookEvent& event);
	void onStudioChoiceBook(wxChoicebookEvent& event);
	void onTelmacChoiceBook(wxChoicebookEvent& event);
	void onElfChoiceBook(wxChoicebookEvent& event);
    void onRcaChoiceBook(wxChoicebookEvent& event);
	void onDebuggerChoiceBook(wxNotebookEvent& event);
    void setConfigurationMenu();
	void setNoteBook();
	void onStart(wxCommandEvent& event);
	void onStart(int computer);
    void onStop(wxCommandEvent& event);

	void stopComputer();
    void killComputer(wxCommandEvent&WXUNUSED(event));
    void enableColorbutton(bool status);
    void enableGui(bool status);
	void message(wxString buffer);
	void messageNoReturn(wxString buffer);
	void messageInt(int value);
    void messageHex(int value);
    void eventMessageHex(int value);
	void initConfig();
	void readConfig();
	void writeConfig();

	wxString getApplicationDir();

	wxString getPathSep();
	int setFdcStepRate(int rate);
    int getFdcCpms();
	int getPsaveData(int item);
	void setPsaveData(int item, int data);
	int getFunctionKey(int item);
	void setFunctionKey(int item, int value);
    bool getUseExitKey() {return useExitKey_;};
    bool getUseCtrlvKey() {return useCtrlvKey_;};
    int getCtrlvKey() {return functionKey_[13];};
    void setUseExitKey(bool status) {useExitKey_ = status;};
    void setUseCtrlvKey(bool status) {useCtrlvKey_ = status;};
    void traceTimeout(wxTimerEvent& event);
    void directAssTimeout(wxTimerEvent& event);
    void vuTimeout(wxTimerEvent& event);
	void updateMemoryTab();
	void updateAssTab();
	void updateSlotInfo();
	void ledTimeout(wxTimerEvent& event);
//	void cpuTimeout(wxTimerEvent& event);
	void updateCheckTimeout(wxTimerEvent& event);
	void startTime();
	void showTime();
    void lapTime();
	void zoomEvent(double zoom);
	void zoomEventVt(double zoom);
    void vuSet(wxString Item, int value);
	void errorMessageEvent(wxErrorMsgEvent& event);
	void errorMessage(wxString msg);

	void setLocationEvent(guiEvent& event);
	void eventSetLocation(bool state, Word saveStart, Word saveEnd, Word saveExec);
	void eventSetLocation(bool state);

	void setSaveStartEvent(guiEvent& event);
	void eventSaveStart(Word saveStart);

	void setSaveEndEvent(guiEvent& event);
	void eventSaveEnd(Word saveEnd);

    void setSwNameEvent(guiEvent& event);
    void eventSetSwName(wxString swName);
    
	void setTapeStateEvent(guiEvent& event);
    void eventSetTapeState(int status, wxString tapeNumber);
    
    void setTextValueEvent(guiEvent& event);
    void eventSetTextValue(wxString info, wxString value);
    
    void setStaticTextValueEvent(guiEvent& event);
    void eventSetStaticTextValue(wxString info, wxString value);
    
	void setCheckBoxEvent(guiEvent& event);
	void eventSetCheckBox(wxString info, bool state);

    void showMessageEvent(guiEvent& event);
    void eventShowMessage(Word value);
    
    void showTextMessageEvent(guiEvent& event);
    void eventShowTextMessage(wxString messageText);
    
    void setZoomChange(guiEvent& event);
    void eventZoomChange(double zoom);
    void zoomEventFinished();
    bool isZoomEventOngoing();
    
    void setZoomVtChange(guiEvent& event);
    void eventZoomVtChange(double zoom, int uartNumber);
    void zoomVtEventFinished();

    void SetZoomEvent(guiEvent& event);
    void eventZoom(double zoom, bool isVt);

    void printDefaultEvent(guiEvent& event);
	void eventPrintDefault(Byte value);

	void printParallelEvent(guiEvent& event);
	void eventPrintParallel(Byte value);
	void printParallelFinishedEvent(guiEvent& event);
	void eventPrintParallelFinished();

	void printThermalEvent(guiEvent& event);
	void eventPrintThermal(Byte value, Byte Qflag);
	void printThermalFinishedEvent(guiEvent& event);
	void eventPrintThermalFinished();

    void printSerialEvent(guiEvent& event);
    void eventPrintSerial(Byte value);

	void printSerialFinishedEvent(guiEvent& event);
	void eventPrintSerialFinished();

    void printPecomEvent(guiEvent& event);
    void eventPrintPecom(Byte value);

    void refreshVideoEvent(guiEvent& event);
    void eventRefreshVideo(bool isVt, int uartNumber);

    void refreshPanelEvent(guiEvent& event);
    void eventRefreshPanel();

	void ShowMessageBoxEvent(guiEvent& event);
	int eventShowMessageBox(wxString message, wxString caption, int style);
	void setMessageBoxAnswer(int answer);

    void GetClientSizeEvent(guiEvent& event);
    wxSize eventGetClientSize(bool isVt, int uartNumber);

    void SetClientSizeEvent(guiEvent& event);
    void eventSetClientSize(wxSize size, bool changeScreenSize, bool isVt, int uartNumber);
    void eventSetClientSize(int sizex, int sizey, bool changeScreenSize, bool isVt, int uartNumber);

	void ShowAddressPopupEvent(guiEvent& event);
	int eventShowAddressPopup(Word specifiedStartAddress);
	void setAddressPopupAnswer(int answer);
	int getAddressPopupAnswer();

	void showFileSelectorEvent(guiEvent& event);
    wxString eventShowFileSelector(wxString message, wxString default_path, wxString default_filename, wxString default_extension, wxString wildcard, int flags);
    void setFileSelectorAnswer(wxString answer);
    
	void updateDebugMemory(Word address);

	void showChip8Register(int variable, int value, bool chip8register12bit);

	void setFandMBasicGuiEvent(guiEvent& event);
	void eventSetFandMBasicGui();

	void enableMemAccesEvent(guiEvent& event);
	void eventEnableMemAccess(bool state);

	void setVideoFullScreenEvent(guiEvent& event);
	void eventVideoSetFullScreen(bool state);

	void setVtFullScreenEvent(guiEvent& event);
	void eventVtSetFullScreen(bool state, int uartNumber);
 
	void setChangeNoteBookEvent(guiEvent& event);
	void eventChangeNoteBook();

	void setDisableControlsEvent(guiEvent& event);
	void eventDisableControls();

	void setUpdateTitle(guiEvent& event);
	void eventUpdateTitle();

	void debounceTimeout(wxTimerEvent& event);
	void setDebounceTimer(guiEvent& event);
	void eventDebounceTimer();

    void guiSizeTimeout(wxTimerEvent& event);
    
    void guiRedrawBarTimeOut(wxTimerEvent& event);

    wxString getMultiCartGame(Byte msb, Byte lsb);
    bool loadKeyDefinition(wxString gameName1, wxString gameName2, int *, int *, int *, bool *, int *, bool *, int *, int *, int*, int*, wxString keyFileName);
    int getDefaultInKey1(wxString computerStr);
    int getDefaultInKey2(wxString computerStr);
    int getDefaultInKey2(wxString computerStr, int defaultKey);
    void getDefaultHexKeys(int computerType, wxString computerStr, wxString player, int *, int *, int*);
	void getTmc600Keys(int *, int *);
	void getDefaultTmc600Keys(int *, int *);
    void storeDefaultTmc600Keys(int *, int *);
    int getDefaultCoinArcadeKeys(int *, int *);
    void storeDefaultCoinArcadeKeys(int *, int *, int coin);
	bool emuClosing() { return emuClosing_; };
	bool getThermalEf() {return thermalEf_;};
	void setStatusLedUpdate(bool status) {statusLedUpdate_ =  status;};
	void setSlotLedUpdate(bool status) {slotLedUpdate_ =  status;};
   
    UpdateCheckThread *m_pUpdateCheckThread;
    wxCriticalSection m_pUpdateCheckThreadCS;    // protects the m_pUpdateCheckThread pointer

private:
	MyHtmlHelpController *help_;
	wxString latestVersion_;

    bool saveOnExit_;
	bool checkForUpdate_;
	bool runPressed_;

	bool thermalEf_;
	bool statusLedUpdate_;

    bool panelRefreshOngoing_;
    bool videoRefreshOngoing_;

	bool emuClosing_;
	bool emmaClosing_;
	int bass_;
	int treble_;
//	wxTimer *cpuPointer;
	wxTimer *updateCheckPointer;
	bool updateCheckStarted_;
	int oldGauge_;

	wxString message_;
    time_t startTime_;
    time_t lapTime_;
    time_t lapTimeStart_;
    uint64_t lastNumberOfCpuCycles_;
    uint64_t lastInstructionCounter_;
	bool cpuCyclesOverflow_;
	bool instructionCounterOverflow_;

	int eventNumber_;

	bool updateMemory_;
	bool updateMemoryPage_;
	bool updateAssPage_;
	bool updateSlotinfo_;
	bool rowChanged_[16];
	bool memoryChanged_[16][16];

	int functionKey_[14];
    CompletedSplashScreen *completedSplashScreen_;
    
	DECLARE_EVENT_TABLE()
};

#endif // MAIN_H

#undef EXT
#define EXT extern

#ifdef MAIN
#undef EXT
#define EXT
#endif

EXT Main *p_Main;
EXT Video *p_Video;
EXT Video *p_Vt100[2];
EXT Serial *p_Serial;
EXT Cdp1802 *p_Computer;

EXT	Printer *p_PrinterParallel;
EXT	Printer *p_PrinterSerial;
EXT	Printer *p_PrinterThermal;
EXT	Printer *p_Printer;
EXT wxPrintData *PrintDataPointer;
EXT wxPageSetupDialogData *p_PageSetupData;

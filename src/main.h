#ifndef MAIN_H
#define MAIN_H

//#ifdef  WIN32
typedef unsigned char Byte;
typedef unsigned short Word;
//#endif

#include "wx/file.h"
#include "wx/html/helpctrl.h"
#include "wx/spinctrl.h"
#include "wx/choicebk.h"
#include "vector"
#include "wx/listctrl.h"

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
    bool GetBoolValue()			{ return boolValue_;}
    Word GetWordValue1()		{ return wordValue1_;}
    Word GetWordValue2()		{ return wordValue2_;}
    Word GetWordValue3()		{ return wordValue3_;}
    Byte GetByteValue1()		{ return byteValue1_;}
    Byte GetByteValue2()		{ return byteValue2_;}
    wxString GetStringValue2()	{ return stringValue2_;}
    wxString GetStringValue3()	{ return stringValue3_;}
    wxString GetStringValue4()	{ return stringValue4_;}
    wxString GetStringValue5()	{ return stringValue5_;}
    void SetBoolValue(bool boolValue)	{ boolValue_ = boolValue;}
    void SetWordValue1(Word wordValue)	{ wordValue1_ = wordValue;}
    void SetWordValue2(Word wordValue)	{ wordValue2_ = wordValue;}
    void SetWordValue3(Word wordValue)	{ wordValue3_ = wordValue;}
    void SetByteValue1(Byte byteValue)	{ byteValue1_ = byteValue;}
    void SetByteValue2(Byte byteValue)	{ byteValue2_ = byteValue;}
    void SetStringValue2(wxString stringValue2)	{ stringValue2_ = stringValue2;}
    void SetStringValue3(wxString stringValue3)	{ stringValue3_ = stringValue3;}
    void SetStringValue4(wxString stringValue4)	{ stringValue4_ = stringValue4;}
    void SetStringValue5(wxString stringValue5)	{ stringValue5_ = stringValue5;}

    // required for sending with wxPostEvent()
	wxEvent *Clone(void) const { return new guiEvent(*this); }

private:
	bool boolValue_;
	Word wordValue1_;
	Word wordValue2_;
	Word wordValue3_;
	Byte byteValue1_;
	Byte byteValue2_;
    wxString stringValue2_;
    wxString stringValue3_;
    wxString stringValue4_;
    wxString stringValue5_;
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

#define OS_WINDOWS_2000 0
#define OS_WINDOWS_XP 1
#define OS_WINDOWS_VISTA 2
#define OS_WINDOWS_7 3
#define OS_WINDOWS_8 4
#define OS_WINDOWS_10 5
#define OS_LINUX_UBUNTU_11_04 10
#define OS_LINUX_UBUNTU_11_10 11
#define OS_LINUX_OPENSUSE_KDE 20
#define OS_LINUX_OPENSUSE_GNOME 21
#define OS_MAC 30
#define OS_LINUX_FEDORA 40

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

class WindowInfo
{
public:
	int xBorder, yBorder;
	int xBorder2, yBorder2;
	int xPrint;
	int mainwX, mainwY;
	int RegularClockY, RegularClockX;
	int ChoiceClockY, ChoiceClockX;
	int operatingSystem;
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
	wxString defaultColour[66];
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
#include "hbelf.h"
#include "member.h"
#include "microtutor.h"
#include "elf2.h"
#include "elf2k.h"
#include "ms2000.h"
#include "mcds.h"
#include "cosmicos.h"
#include "super.h"
#include "studio2.h"
#include "visicom.h"
#include "victory.h"
#include "vip.h"
#include "vip2.h"
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

#define EMMA_VERSION 1.24
#define EMMA_SUB_VERSION 33
#define ELF 0
#define ELFII 1
#define SUPERELF 2
#define ELF2K 3
#define COSMICOS 4
#define MEMBER 5
#define VIP 6
#define VELF 7
#define MICROTUTOR 8
#define MS2000 9
#define MCDS 10
#define COMX 11
#define STUDIO 12
#define ETI 13
#define CIDELSA 14
#define TMC600 15
#define TMC1800 16
#define TMC2000 17
#define NANO 18
#define PECOM 19
#define VISICOM 20
#define VICTORY 21
#define VIPII 22
#define DEBUGGER 23
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
#define VELFTAB 6

#define MICROTUTORTAB 0
#define VIPTAB 1
#define VIPIITAB 2
#define MCDSTAB 3
#define MS2000TAB 4

#define STUDIOIITAB 0
#define VISICOMTAB 1
#define VICTORYTAB 2

#define TMC600TAB 0
#define TMC1800TAB 1
#define TMC2000TAB 2
#define NANOTAB 3

#define MESSAGETAB 0
#define DIRECTASSTAB 1
#define TRACETAB 2
#define CHIP8TAB 3
#define MEMORYTAB 4
#define ASSTAB 5

#define VIDEONONE 0
#define VIDEO1870 0
#define VIDEO80COL 2

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

#define KEYBOARDNONE 0
#define KEYBOARD_PS2 1
#define KEYBOARD_PS2_INT 2
#define KEYBOARD_ASCII 3
#define KEYBOARD_ASCII_HEX_EF 4
#define KEYBOARD_HEX_EF 5
#define KEYBOARD_HEX 6

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
#define NOCHANGE 30

#define SHOWNAME true
#define NONAME false

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

#define TINYBASIC 2
#define MINIMON 4
#define GOLDMON 6
#define MONITOR_CHUCK_LOW 8
#define MONITOR_CHUCK_HIGH 10

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
#define GUI_START_BUTTON 30040
#define GUI_CONFIG_MENU 10000
#define GUI_CONFIG_DELETE_MENU 20000

#define TAPE_STOP 0
#define TAPE_PLAY 1
#define TAPE_RECORD 2

#define PRINT_BUFFER_SIZE 1000

#define CHIP_NONE 0
#define CHIP8 1
#define CHIP8X 2
#define CHIPETI 3
#define CHIPST2 4

#define CPU1801 1
#define CPU1802 2
#define CPU1804 4
#define CPU1805 5

#define STARTUP_ZEROED 0
#define STARTUP_RANDOM 1
#define STARTUP_DYNAMIC 2

#define MEM_TYPE_DATA 0
#define MEM_TYPE_JUMP 1
#define MEM_TYPE_OPCODE 2
#define MEM_TYPE_OPERAND 3
#define MEM_TYPE_ST2_1 4
#define MEM_TYPE_ST2_2 5
#define MEM_TYPE_CHIP_8_1 6
#define MEM_TYPE_CHIP_8_2 7
#define MEM_TYPE_UNDEFINED 8
#define MEM_TYPE_JUMP_REV 9
#define MEM_TYPE_TEXT 10
#define MEM_TYPE_OPCODE_RSHR 70
#define MEM_TYPE_OPCODE_RSHL 71
#define MEM_TYPE_OPCODE_BPZ 72
#define MEM_TYPE_OPCODE_BGE 73
#define MEM_TYPE_OPCODE_BM 74
#define MEM_TYPE_OPCODE_BL 75
#define MEM_TYPE_OPCODE_LSKP 76
#define MEM_TYPE_OPCODE_RLDL 77
#define MEM_TYPE_OPCODE_SKP 78
#define MEM_TYPE_OPCODE_LBR_SLOT 79
#define MEM_TYPE_OPCODE_JUMP_SLOT 80
#define MEM_TYPE_OPCODE_LDV 81
#define MEM_TYPE_OPCODE_LDL 82
#define MEM_TYPE_OPERAND_LD_2 83
#define MEM_TYPE_OPERAND_LD_3 84
#define MEM_TYPE_OPERAND_LD_5 85
#define MEM_TYPE_OPCODE_LDL_SLOT 86

#define LABEL_TYPE_NONE 0
#define LABEL_TYPE_BRANCH 1
#define LABEL_TYPE_JUMP 2
#define LABEL_TYPE_SUB 3

#define IO_TYPE_N0 0
#define IO_TYPE_N1 1
#define IO_TYPE_N2 2

#define VIDEO 0
#define PIXIE 1

#if defined(__WXMAC__)
#define IMAGES_FOLDER "images_osx"
#else
#define IMAGES_FOLDER "images"
#endif

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

WindowInfo getWinSizeInfo();

class Main: public DebugWindow
{
public:

	Main(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~Main();
    
    wxSize getPosition(wxString control, wxSize size);
    wxSize getDefaultGuiSize();
    void windowSizeChanged(wxSizeEvent& event);
	void pageSetup();
	void onClose(wxCloseEvent&event );

	bool checkUpdateEmma();
	bool updateEmma();
	wxString downloadString(wxString url);

	void onQuit(wxCommandEvent& event);
	void onAbout(wxCommandEvent& event);
    void onDataDir(wxCommandEvent& event);
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
	void onDebuggerChoiceBook(wxChoicebookEvent& event);
    void setConfigurationMenu();
	void setNoteBook();
	void onStart(wxCommandEvent& event);
	void onStart(int computer);

	void stopComputer();
    void killComputer(wxCommandEvent&WXUNUSED(event));
	void enableGui(bool status);
	void message(wxString buffer);
	void messageNoReturn(wxString buffer);
	void messageInt(int value);
	void messageHex(int value);
	void initConfig();
	void readConfig();
	void writeConfig();

	wxString getApplicationDir();

	wxChar getPathSep();
	int setFdcStepRate(int rate);
    int getFdcCpms();
	int getPsaveData(int item);
	void setPsaveData(int item, int data);
	int getFunctionKey(int item);
	void setFunctionKey(int item, int value);
	bool getUseExitKey() {return useExitKey_;};
	void setUseExitKey(bool status) {useExitKey_ = status;};
    void traceTimeout(wxTimerEvent& event);
    void vuTimeout(wxTimerEvent& event);
	void updateMemoryTab();
	void updateAssTab();
	void updateSlotInfo();
	void ledTimeout(wxTimerEvent& event);
	void cpuTimeout(wxTimerEvent& event);
	void updateCheckTimeout(wxTimerEvent& event);
	void startTime();
	void showTime();
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
	void eventSetTapeState(int status);

	void setTextValueEvent(guiEvent& event);
	void eventSetTextValue(wxString info, wxString value);

	void setCheckBoxEvent(guiEvent& event);
	void eventSetCheckBox(wxString info, bool state);

    void showMessageEvent(guiEvent& event);
    void eventShowMessage(Word value);
    
    void showTextMessageEvent(guiEvent& event);
    void eventShowTextMessage(wxString messageText);
    
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

	void ShowMessageBoxEvent(guiEvent& event);
	int eventShowMessageBox(wxString message, wxString caption, int style);
	void setMessageBoxAnswer(int answer);

	void ShowAddressPopupEvent(guiEvent& event);
	int eventShowAddressPopup(Word specifiedStartAddress);
	void setAddressPopupAnswer(int answer);
	int getAddressPopupAnswer();

	void showFileSelectorEvent(guiEvent& event);
    wxString eventShowFileSelector(wxString message, wxString default_path, wxString default_filename, wxString default_extension, wxString wildcard, int flags);
    void setFileSelectorAnswer(wxString answer);
    
	void updateDebugMemory(Word address);

	void showChip8Register(int variable, int value);

	void setFandMBasicGuiEvent(guiEvent& event);
	void eventSetFandMBasicGui();

	void enableMemAccesEvent(guiEvent& event);
	void eventEnableMemAccess(bool state);

	void setVideoFullScreenEvent(guiEvent& event);
	void eventVideoSetFullScreen(bool state);

	void setVtFullScreenEvent(guiEvent& event);
	void eventVtSetFullScreen(bool state);

	void setChangeNoteBookEvent(guiEvent& event);
	void eventChangeNoteBook();

	void setDisableControlsEvent(guiEvent& event);
	void eventDisableControls();

	void setUpdateTitle(guiEvent& event);
	void eventUpdateTitle();

	void debounceTimeout(wxTimerEvent& event);
	void setDebounceTimer(guiEvent& event);
	void eventDebounceTimer();

	wxString getMultiCartGame(Byte msb, Byte lsb);
    bool loadKeyDefinition(wxString gameName1, wxString gameName2, int *, int *, int *, bool *, int *, bool *, int *, int *, int*, int*, wxString keyFileName);
    int getDefaultInKey1(wxString computerStr);
    int getDefaultInKey2(wxString computerStr);
    void getDefaultHexKeys(int computerType, wxString computerStr, wxString player, int *, int *, int*);
    void storeDefaultTmc600Keys(int *, int *);
	void getTmc600Keys(int *, int *);
	void getDefaultTmc600Keys(int *, int *);
	bool emuClosing() { return emuClosing_; };
	bool getThermalEf() {return thermalEf_;};
	void setStatusLedUpdate(bool status) {statusLedUpdate_ =  status;};
	void setSlotLedUpdate(bool status) {slotLedUpdate_ =  status;};

    UpdateCheckThread *m_pUpdateCheckThread;
    wxCriticalSection m_pUpdateCheckThreadCS;    // protects the m_pUpdateCheckThread pointer

private:
	wxHtmlHelpController *help_;
	wxString latestVersion_;

    bool saveOnExit_;
	bool checkForUpdate_;
	bool runPressed_;

	bool thermalEf_;
	bool statusLedUpdate_;

	bool emuClosing_;
	bool emmaClosing_;
	int bass_;
	int treble_;
	wxTimer *cpuPointer;
	wxTimer *updateCheckPointer;
	bool updateCheckStarted_;
	int oldGauge_;

	wxString message_;
	time_t startTime_;

	int eventNumber_;

	bool updateMemory_;
	bool updateMemoryPage_;
	bool updateAssPage_;
	bool updateSlotinfo_;
	bool rowChanged_[16];
	bool memoryChanged_[16][16];

	int functionKey_[13];
    
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
EXT Video *p_Vt100;
EXT Cdp1802 *p_Computer;

EXT	Printer *p_PrinterParallel;
EXT	Printer *p_PrinterSerial;
EXT	Printer *p_PrinterThermal;
EXT	Printer *p_Printer;
EXT wxPrintData *PrintDataPointer;
EXT wxPageSetupDialogData *p_PageSetupData;

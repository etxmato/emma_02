/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 ***                                                             ***
 *** 1802 Code based on elf emulator by Michael H Riley with     ***
 *** copyright as below                                          ***
 *******************************************************************
*/

/*
 *******************************************************************
 *** This software is copyright 2006 by Michael H Riley          ***
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
#include "wx/textfile.h"
#include "wx/tglbtn.h"
#include "wx/listbox.h"
#include "wx/fileconf.h"

#include "main.h"
#include "debug.h"
#include "elfconfiguration.h"
#include "breakpoints.h"

#define CHIP8_I 10
#define CHIP8_PC 5
#if defined (__linux__)
#define EDIT_ROW 18
#define NUMBER_OF_DEBUG_LINES 40
#define LINE_SPACE 13
#define ASS_WIDTH 268
#define CHAR_WIDTH 8
#endif
#if defined (__WXMSW__)
#define EDIT_ROW 16
#define NUMBER_OF_DEBUG_LINES 33
#define LINE_SPACE 11
#define ASS_WIDTH 268
#define CHAR_WIDTH 8
#endif
#if defined (__WXMAC__)
#define EDIT_ROW 16
#define NUMBER_OF_DEBUG_LINES 35
#define LINE_SPACE 11
#define ASS_WIDTH 268
#define CHAR_WIDTH 8
#endif

enum
{
	CHIP8_VX,           //0
	CHIP8_VX_MEM,       //1
    ASS_HEX_VALUE,      //2
    ASS_HEX_VALUE_MEM,  //3
	ASS_STRING,         //4
	ASS_REG,            //5
	ASS_SLOT,           //6
    FEL_C,              //7
    DUMMY8,
    DUMMY9,
    DUMMY10,
    DUMMY11,
    DUMMY12,
	DUMMY13,
	DUMMY14,
	DUMMY15,
    DUMMY16,
    DUMMY17,
    DUMMY18,
    DUMMY19,
    DUMMY20,
    DUMMY21,
    DUMMY22,
    ERROR_START,
    ERROR_4BIT,
    ERROR_8BIT,
    ERROR_16BIT,
    ERROR_12BIT,
	ERROR_NS_8,
	ERROR_NS_8X,
	ERROR_NS_ETI,
	ERROR_NO_SLASH,
	ERROR_2ND_REG,
	ERROR_NO_REG0,
	DUMMY_JUMP,
	ERROR_ONLY_REG0,
	ERROR_ONLY_REGB,
	ERROR_ONLY_REGC,
	ERROR_NO_VALUE_0,
	ERROR_NO_VALUE_F,
	ERROR_ONLY_VALUE_0,
	ERROR_4REG,
	ERROR_8REG,
	ERROR_SYNTAX,
	ERROR_REG,
	ERROR_DEC,
	ERROR_REG_EXP,
	ERROR_PAR,
	ERROR_IO,
	ERROR_COMMA,
    ERROR_SLASH,
    ERROR_DASH,
    ERROR_CPU_1802,
	ERROR_CPU_1804,
	ERROR_INST,
	ERROR_COMMAND_SEP,
	ERROR_SPRITE,
	ERROR_VIDEO,
	ERROR_LD,
	ERROR_CLR,
	ERROR_NO_ADDRESS,
	ERROR_END_START,
	ERROR_DEBUG_CLEAR,
	ERROR_DEBUG_ADDRESS,
	ERROR_COMPUTER_NOT_RUNNING,
	ERROR_NO_RANGE,
	ERROR_INVALID_START,
	ERROR_INVALID_FILE_NAME,
	ERROR_FILE_NOTFOUND,
	ERROR_REGAND16,
	ERROR_MACRO_NOT_FOUND,
	ERROR_MEMORY_WARNING,
	ERROR_SLOT,
	ERROR_SLOT_RANGE,
	ERROR_PAGE_RANGE,
	ERROR_CONF_SAVED,
	ERROR_CONF_LOADED,
	ERROR_COPIED_NO_JUMPS,
	ERROR_COPIED_JUMPS,
    ERROR_CPU_1801,
    ERROR_STUDIO_CHIP_ADDRESS,
    ERROR_FEL2_CHIP_ADDRESS,
    ERROR_RAM_CHIP_ADDRESS,
    ERROR_STUDIO_CHIP_ADDRESS_I,
    ERROR_STUDIOIV_ADDRESS_I,
    ERROR_COMX_NOT_RUNNING,
    WARNING_MISSING_SLOT_ADDRESS,
    ERROR_HEX,
    ERROR_HEX_TO_HIGH,
    ERROR_NO_COMPUTER_RUNNING,
    ERROR_INCORR_ADDRESS,
    ERROR_INCORRECT_PAR,
    ERROR_INCORRECT_REG,
    ERROR_TEMP_PAR,
    ERROR_TEMP_CPU_1801,
    ERROR_LAST,
};

wxString DirAssErrorCodes[] =
{
	"4 bit value expected",
	"8 bit value expected",
	"16 bit value expected",
	"12 bit value expected",
	"Not supported on Chip-8",
	"Not supported on Chip-8X",
	"Not supported on ETI Chip-8",
	"Seperator not allowed",
	"Second register value incorrect",
	"Register 0 not allowed",
	"Only register 0 allowed",
	"Dummy for JUMP",
	"Only register B allowed",
	"Only register C allowed",
	"Value 0 not allowed",
	"Value F not allowed",
	"Only fill with 0 allowed",
	"4 bit value or register expected",
	"8 bit value or register expected",
	"Syntax error",
	"Register value not recognized",
	"Decimal value not recognized",
	"Register value expected",
	"Too many parameters",
	"I/O value not recognized",
	"Comma expected after parameter",
    "Slash expected after parameter",
    "Dash expected after parameter",
	"Not supported on CDP1802",
	"Not supported on CDP1804,",
    "Instruction not recognized",
	"Space expected after command",
	"Incorrect sprite (SP) command",
	"Incorrect video command",
	"Incorrect load (LD) command",
	"Incorrect colour (CLR) command",
	"No address specified",
	"Start > end specified",
	"Debug memory cleared",
	"Not within any address range",
	"Computer not running",
	"No range defined",
	"Specify: end > code end >= start",
	"No file name specified",
	"File not found",
	"Register and 16 bit value expected",
	"Macro not found in memory",
	"Memory warning!",
	"Slot or page value not recognized",
	"Slot conig: > C000 and < DFFF",
	"Page conig: > 8000 and < BFFF",
	"Configuartion saved",
	"Configuration loaded",
	"Copied, NO branches corrected",
	"Copied, long branches corrected",
    "Not supported on CDP1801",
    "Specify address > 2FC and <= FFF",
    "Specify address > 1FF and <= FFF",
    "Specify address >= 800 and <= 8FF",
    "Specify address >= 100 and <= FFF",
    "Specify address >= 2700 and <= 27FF",
    "COMX-35 not running",
    "Warning: no slot specified",
    "Hexadecimal value expected",
    "Hexadecimal value to high",
    "No computer running",
    "Incorrect address value",
    "Incorrect parameter",
    "Incorrect register value",
    "Too many parameters",
    "Not supported on CDP1801",
};

int opCode[] =
{
	0x74, 0x7c, 0xf4, 0xfc, 0xf2, 0xfa, // A
	0x34, 0x35, 0x36, 0x37, 0x68, 0x33, 0x33, 0x3b, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x3b, 0x39, 0x3a, 0x33, 0x31, 0x30, 0x68, 0x32, // B
	0x68, 0x68, // C
	0x68, 0x68, 0x68, 0x68, 0x68, 0x20, 0x71, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, // D
	0x68, // E
	0x68, 0x90, 0x80, // G
	0x00, 0x10, 0x69, 0x60, // I
	0xc3, 0xcb, 0xc9, 0xca, 0xc1, 0xc0, 0xc2, 0x40, 0x68, 0xf8, 0x00, 0xf0, 0x72, 0xcf, 0xcc, 0xc8, 0xc7, 0xc5, 0xc6, 0xcd, 0xce, // L
	0x79, // M
	0x38, 0xc8, 0xc4, // N
	0xf1, 0xf9, 0x61, // O
	0xb0, 0xa0, // P
	0x7a, 0x70, 0x68, 0x68, 0x68, 0x7e, 0x76, 0x68, // R
	0x78, 0x68, 0x68, 0x68, 0xf5, 0x75, 0x7d, 0xfd, 0xd0, 0x7b, 0xe0, 0xfe, 0x7e, 0xf6, 0x76, 0x38, 0xf7, 0x77, 0x7f, 0xff, 0x68, 0x68, 0x68, 0x68, 0x68, 0x50, 0x73, // S
	0x68, 0x68, 0xf3, 0xfb // X
};

int opCode2[] =
{
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, // A
	0x30, 0x30, 0x30, 0x30, 0x3e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3f, 0x30, // B
	0x0d, 0x0c, // C
	0x7c, 0x74, 0xf4, 0xfc, 0x20, 0x30, 0x30, 0x76, 0x7f, 0xf7, 0x77, 0xff, 0x01, // D
	0x09,
	0x08, 0x30, 0x30, // G
	0x30, 0x30, 0x30, 0x30, // I
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x06, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, // L
	0x30, // M
	0x30, 0x30, 0x30, // N
	0x30, 0x30, 0x30, // O
	0x30, 0x30, // P
	0x30, 0x30, 0xc0, 0x60, 0xb0, 0x30, 0x30, 0xa0, // R
	0x30, 0x80, 0x05, 0x03, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x04, 0x02, 0x90, 0x07, 0x00, 0x30, 0x30, // S
	0x0b, 0x0a, 0x30, 0x30 // X
};

int macro[] =
{
	0, 0, 0, 0, 0, 0, // A
	0, 0, 0, 0, 0, 0, MEM_TYPE_OPCODE_BGE, MEM_TYPE_OPCODE_BL, MEM_TYPE_OPCODE_BM, 0, 0, 0, 0, 0, 0, 0, MEM_TYPE_OPCODE_BPZ, 0, 0, 0, 0, // B
	0, 0, // C
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // D
	0,
	0, 0, 0, // G
	0, 0, 0, 0, // I
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MEM_TYPE_OPCODE_LSKP, 0, 0, 0, 0, 0, // L
	0, // M
	0, 0, 0, // N
	0, 0, 0, // O
	0, 0, // P
	0, 0, 0, 0, 0, MEM_TYPE_OPCODE_RSHL, MEM_TYPE_OPCODE_RSHR, 0, // R
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MEM_TYPE_OPCODE_SKP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // S
	0, 0, 0, 0 // X
};

int minCpuType[] =
{
	2, 2, 1, 1, 1, 1, // A
	1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 4, 1, // B
	4, 4, // C
	5, 5, 5, 5, 5, 1, 1, 5, 5, 5, 5, 5, 4, // D
	4,
	4, 1, 1, // G
	1, 1, 1, 1, // I
	2, 2, 2, 2, 2, 2, 2, 1, 4, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, // L
	2, // M
	1, 2, 2, // N
	1, 1, 1, // O
	1, 1, // P
	2, 1, 4, 4, 4, 2, 2, 4, // R
	1, 4, 4, 4, 1, 2, 2, 1, 1, 2, 1, 2, 2, 1, 2, 1, 1, 2, 2, 1, 4, 4, 4, 4, 4, 1, 2, // S
	4, 4, 1, 1 // X
};

int numberOfBytes[] =
{
	1, 2, 1, 2, 1, 2,
	2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2,
	2, 2, // C
	3, 2, 2, 3, 4, 1, 1, 2, 3, 2, 2, 3, 2, // D
	2,
	2, 1, 1,
	1, 1, 2, 1, // I
	3, 3, 3, 3, 3, 3, 3 , 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 , 1,
	1,
	1, 1, 1,
	1, 2, 2, // O
	1, 1,
	1, 1, 4, 2, 2, 1, 1, 2,// R
	1, 4, 2, 2, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, // S
	2, 2, 1, 2
};

int numberOfBytes1802[] =
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 1x
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 2x
	2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2,  // 3x
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 4x
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 5x
	1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,  // 6x
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 2,  // 7x
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 8x
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 9x
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Ax
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Bx
	3, 3, 3, 3, 1, 1, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1,  // Cx
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Dx
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Ex
	1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // Fx
};

int numberOfBytes1806[] =
{
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0,  // 0x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 1x
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  // 2x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3,  // 3x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 4x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 5x
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 6x
	0, 0, 0, 0, 2, 0, 2, 2, 0, 0, 0, 0, 3, 0, 0, 3,  // 7x
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  // 8x
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 9x
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // Ax
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // Bx
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  // Cx
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Dx
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Ex
	0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 3,  // Fx
};

bool useRegister[] =
{
	false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, // C
	false, false, false, false, true, true, false, false, false, false, false, false, false, // D
	false,
	false, true, true,
	false, true, false, false,
	false, false, false, false, false, false, false, true, false, false, true, false, false, false, false, false, false, false, false, false, false,
	false,
	false, false, false,
	false, false, false,
	true, true,
	false, false, true, true, false, false, true, // R
	false, true, true, false, false, false, false, false, false, true, false, true, false, false, false, false, false, false, false, false, false, false, false, true, false, false, true, false,
	false, false, false, false,
};

int jumpCorrection[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 1
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 2
 	1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,  // 3
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 4
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 5
	0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,  // 6
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 7
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 8
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 9
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // a
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // b
	2, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0,  // c
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // d
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // e
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // f
};

Word chip8Sprite[] =
{
	0x130,
	0x139,
	0x122,
	0x12a,
	0x13e,
	0x120,
	0x124,
	0x134,
	0x126,
	0x128,
	0x12e,
	0x118,
	0x114,
	0x11c,
	0x110,
	0x112
};

BEGIN_EVENT_TABLE(DebugWindow, GuiComx)

	EVT_CHECKBOX(XRCID("DebugMode"), Main::onDebugMode)
//	EVT_TEXT_ENTER(XRCID("InputWindow"), DebugWindow::onEnter)

	EVT_BUTTON(XRCID("TraceLog"), DebugWindow::onLog)
	EVT_TOGGLEBUTTON(XRCID("TraceButton"), DebugWindow::onTrace)
	EVT_TOGGLEBUTTON(XRCID("DmaButton"), DebugWindow::onTraceDma)
	EVT_TOGGLEBUTTON(XRCID("IntButton"), DebugWindow::onTraceInt)
	EVT_TOGGLEBUTTON(XRCID("Chip8IntButton"), DebugWindow::onChip8TraceInt)
	EVT_TOGGLEBUTTON(XRCID("TraceTrapButton"), DebugWindow::onTraceTrap)

	EVT_BUTTON(XRCID("DebugInterrupt"), DebugWindow::onInt)
	EVT_BUTTON(XRCID("ClearButton"), DebugWindow::onClear)
    EVT_BUTTON(XRCID("DebugReset"), DebugWindow::onReset)

	EVT_BUTTON(XRCID("DebugPauseButton"), DebugWindow::onPauseButton)
	EVT_BUTTON(XRCID("DebugStepButton"), DebugWindow::onStepButton)
	EVT_BUTTON(XRCID("DebugRunButton"), DebugWindow::onRunButton)
	EVT_TEXT(XRCID("DebugRunAddress"), DebugWindow::onRunAddress)
	EVT_TEXT_ENTER(XRCID("BreakPointAddress"), DebugWindow::onBreakPointSet)
	EVT_TEXT(XRCID("TregValue"), DebugWindow::onTregValue)
	EVT_TEXT(XRCID("NumberOfSteps"), DebugWindow::onNumberOfSteps)

	EVT_BUTTON(XRCID("BreakPointSet"), DebugWindow::onBreakPointSet)
	EVT_BUTTON(XRCID("TregSet"), DebugWindow::onTregSet)
	EVT_TEXT_ENTER(XRCID("TregValue"), DebugWindow::onTregSet)
	EVT_CHOICE(XRCID("TrapCommand"), DebugWindow::onTrapCommand)
	EVT_BUTTON(XRCID("TrapSet"), DebugWindow::onTrapSet)
	EVT_TEXT_ENTER(XRCID("TrapValue"), DebugWindow::onTrapSet)

	EVT_LIST_DELETE_ITEM(XRCID("BreakPointWindow"), DebugWindow::deleteBreakPoint)
	EVT_LIST_DELETE_ITEM(XRCID("TregWindow"), DebugWindow::deleteTreg)
	EVT_LIST_DELETE_ITEM(XRCID("TrapWindow"), DebugWindow::deleteTrap)
	EVT_LIST_END_LABEL_EDIT(XRCID("BreakPointWindow"), DebugWindow::editBreakPoint)
	EVT_LIST_END_LABEL_EDIT(XRCID("TregWindow"), DebugWindow::editTreg)
	EVT_LIST_END_LABEL_EDIT(XRCID("TrapWindow"), DebugWindow::editTrap)

	EVT_LIST_ITEM_SELECTED(XRCID("BreakPointWindow"), DebugWindow::selectBreakPoint)
	EVT_LIST_ITEM_DESELECTED(XRCID("BreakPointWindow"), DebugWindow::deselectBreakPoint)
	EVT_LIST_KEY_DOWN(XRCID("BreakPointWindow"), DebugWindow::keyBreakPoint)

	EVT_LIST_ITEM_SELECTED(XRCID("Chip8BreakPointWindow"), DebugWindow::selectChip8BreakPoint)
	EVT_LIST_ITEM_DESELECTED(XRCID("Chip8BreakPointWindow"), DebugWindow::deselectChip8BreakPoint)
	EVT_LIST_KEY_DOWN(XRCID("Chip8BreakPointWindow"), DebugWindow::keyChip8BreakPoint)

	EVT_LIST_ITEM_SELECTED(XRCID("TregWindow"), DebugWindow::selectTreg)
	EVT_LIST_ITEM_DESELECTED(XRCID("TregWindow"), DebugWindow::deselectTreg)
	EVT_LIST_KEY_DOWN(XRCID("TregWindow"), DebugWindow::keyTreg)

	EVT_LIST_ITEM_SELECTED(XRCID("TrapWindow"), DebugWindow::selectTrap)
	EVT_LIST_ITEM_DESELECTED(XRCID("TrapWindow"), DebugWindow::deselectTrap)
	EVT_LIST_KEY_DOWN(XRCID("TrapWindow"), DebugWindow::keyTrap)

	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("PercentageClock"), DebugWindow::onPercentageClock)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("PercentageClock"), DebugWindow::onPercentageClock)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("Chip8PercentageClock"), DebugWindow::onChip8PercentageClock)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("Chip8PercentageClock"), DebugWindow::onChip8PercentageClock)

	EVT_TEXT_ENTER(XRCID("O1"), DebugWindow::O1)
	EVT_TEXT_ENTER(XRCID("O2"), DebugWindow::O2)
	EVT_TEXT_ENTER(XRCID("O3"), DebugWindow::O3)
	EVT_TEXT_ENTER(XRCID("O4"), DebugWindow::O4)
	EVT_TEXT_ENTER(XRCID("O5"), DebugWindow::O5)
	EVT_TEXT_ENTER(XRCID("O6"), DebugWindow::O6)
	EVT_TEXT_ENTER(XRCID("O7"), DebugWindow::O7)

	EVT_TEXT_ENTER(XRCID("D"), DebugWindow::D)
	EVT_TEXT_ENTER(XRCID("P"), DebugWindow::P)
	EVT_TEXT_ENTER(XRCID("X"), DebugWindow::X)
	EVT_TEXT_ENTER(XRCID("T"), DebugWindow::T)
	EVT_TEXT_ENTER(XRCID("DF"), DebugWindow::DF)
	EVT_TEXT_ENTER(XRCID("Q"), DebugWindow::Q)
	EVT_TEXT_ENTER(XRCID("IE"), DebugWindow::IE)
	EVT_TEXT_ENTER(XRCID("EF1"), DebugWindow::EF1)
	EVT_TEXT_ENTER(XRCID("EF2"), DebugWindow::EF2)
	EVT_TEXT_ENTER(XRCID("EF3"), DebugWindow::EF3)
	EVT_TEXT_ENTER(XRCID("EF4"), DebugWindow::EF4)

	EVT_TEXT_ENTER(XRCID("R0"), DebugWindow::R0)
	EVT_TEXT_ENTER(XRCID("R1"), DebugWindow::R1)
	EVT_TEXT_ENTER(XRCID("R2"), DebugWindow::R2)
	EVT_TEXT_ENTER(XRCID("R3"), DebugWindow::R3)
	EVT_TEXT_ENTER(XRCID("R4"), DebugWindow::R4)
	EVT_TEXT_ENTER(XRCID("R5"), DebugWindow::R5)
	EVT_TEXT_ENTER(XRCID("R6"), DebugWindow::R6)
	EVT_TEXT_ENTER(XRCID("R7"), DebugWindow::R7)
	EVT_TEXT_ENTER(XRCID("R8"), DebugWindow::R8)
	EVT_TEXT_ENTER(XRCID("R9"), DebugWindow::R9)
	EVT_TEXT_ENTER(XRCID("R10"), DebugWindow::RA)
	EVT_TEXT_ENTER(XRCID("R11"), DebugWindow::RB)
	EVT_TEXT_ENTER(XRCID("R12"), DebugWindow::RC)
	EVT_TEXT_ENTER(XRCID("R13"), DebugWindow::RD)
	EVT_TEXT_ENTER(XRCID("R14"), DebugWindow::RE)
	EVT_TEXT_ENTER(XRCID("R15"), DebugWindow::RF)

	EVT_TEXT_ENTER(XRCID("V0"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("V1"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("V2"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("V3"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("V4"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("V5"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("V6"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("V7"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("V8"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("V9"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("VA"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("VB"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("VC"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("VD"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("VE"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("VF"), DebugWindow::Vx)
	EVT_TEXT_ENTER(XRCID("Chip8I"), DebugWindow::chip8I)

	EVT_TEXT(XRCID("AssAddress"), DebugWindow::onAssAddress)
	EVT_SPIN_UP(XRCID("AssSpin"), DebugWindow::onAssSpinUp)
	EVT_SPIN_DOWN(XRCID("AssSpin"), DebugWindow::onAssSpinDown)
	EVT_SPIN_UP(XRCID("AssSpinPage"), DebugWindow::onAssSpinPageUp)
	EVT_SPIN_DOWN(XRCID("AssSpinPage"), DebugWindow::onAssSpinPageDown)
	EVT_TEXT_ENTER(XRCID("AssInputWindow"), DebugWindow::onAssEnter)
	EVT_BUTTON(XRCID("AssMarkType"), DebugWindow::onAssMark)
	EVT_CHECKBOX(XRCID("AssSaveDebugFile"), DebugWindow::onSaveDebugFile)
	EVT_BUTTON(XRCID("AssClr"), DebugWindow::onClearErrorLog)
	EVT_BUTTON(XRCID("AssFrom"), DebugWindow::onAssFrom)
	EVT_BUTTON(XRCID("AssFromL"), DebugWindow::onAssFromL)
    EVT_BUTTON(XRCID("AssFromV"), DebugWindow::onAssFromV)
    EVT_BUTTON(XRCID("AssFromA"), DebugWindow::onAssFromAll)
    EVT_BUTTON(XRCID("AssInsert"), DebugWindow::onInsert)
	EVT_BUTTON(XRCID("AssDelete"), DebugWindow::onDelete)
//	EVT_BUTTON(XRCID("AssSave"), DebugWindow::onAssSave)
	EVT_BUTTON(XRCID("AssLoadAll"), DebugWindow::onAssLoadAll)
	EVT_BUTTON(XRCID("AssSaveAll"), DebugWindow::onAssSaveAll)
	EVT_BUTTON(XRCID("AssNew"), DebugWindow::onAssNew)
	EVT_BUTTON(XRCID("AssStore"), DebugWindow::onAssStore)
	EVT_BUTTON(XRCID("AssDir"), DebugWindow::onAssDir)
	EVT_BUTTON(XRCID("AssSaveSb"), DebugWindow::onAssSaveSb)
	EVT_SPIN_UP(XRCID("AssRangeSpin"), DebugWindow::onAssRangeSpinUp)
	EVT_SPIN_DOWN(XRCID("AssRangeSpin"), DebugWindow::onAssRangeSpinDown)
	EVT_CHOICE(XRCID("AssDataView"), DebugWindow::onAssDataView)
	EVT_BUTTON(XRCID("AssCopy"), DebugWindow::onAssCopy)
	EVT_BUTTON(XRCID("AssDis"), DebugWindow::onAssDis)
	EVT_TEXT(XRCID("AssFileName"), DebugWindow::onAssTextChange)
	EVT_TEXT(XRCID("AssProgramStart"), DebugWindow::onAssTextChange)
	EVT_TEXT(XRCID("AssCodeEnd"), DebugWindow::onAssTextChange)
	EVT_TEXT(XRCID("AssProgramEnd"), DebugWindow::onAssTextChange)
	EVT_TEXT(XRCID("AssProgramSlot"), DebugWindow::onAssTextChange)

	EVT_TEXT(XRCID("DebugDisplayPage"), DebugWindow::onDebugDisplayPage)
#ifdef __WXMAC__
    EVT_SPIN_UP(XRCID("DebugDisplayPageSpinButton"), DebugWindow::onDebugDisplayPageSpinDown)
    EVT_SPIN_DOWN(XRCID("DebugDisplayPageSpinButton"), DebugWindow::onDebugDisplayPageSpinUp)
#else
	EVT_SPIN_UP(XRCID("DebugDisplayPageSpinButton"), DebugWindow::onDebugDisplayPageSpinUp)
	EVT_SPIN_DOWN(XRCID("DebugDisplayPageSpinButton"), DebugWindow::onDebugDisplayPageSpinDown)
#endif
	EVT_CHOICE(XRCID("DebugMemType"), DebugWindow::onDebugMemType)
	EVT_TEXT(XRCID("DebugExpansionSlot"), DebugWindow::onDebugExpansionSlot)
	EVT_TEXT(XRCID("DebugExpansionRam"), DebugWindow::onDebugExpansionRam)
	EVT_TEXT(XRCID("DebugExpansionEprom"), DebugWindow::onDebugExpansionEprom)
	EVT_TEXT(XRCID("DebugEmsPage"), DebugWindow::onDebugEmsPage)
	EVT_TEXT(XRCID("DebugPager"), DebugWindow::onDebugPager)
	EVT_TEXT(XRCID("DebugPortExtender"), DebugWindow::onDebugPortExtender)
	EVT_BUTTON(XRCID("DebugSave"), DebugWindow::onDebugSaveDump)
	EVT_BUTTON(XRCID("DebugCopy"), DebugWindow::onDebugCopy)
	EVT_TEXT(XRCID("DebugCopyStart"), DebugWindow::onDebugCopyStart)
	EVT_TEXT(XRCID("DebugCopyEnd"), DebugWindow::onDebugCopyEnd)
	EVT_TEXT(XRCID("DebugCopyTo"), DebugWindow::onDebugCopyTo)

	EVT_TEXT(XRCID("MEM00"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM01"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM02"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM03"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM04"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM05"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM06"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM07"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM08"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM09"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM0A"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM0B"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM0C"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM0D"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM0E"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM0F"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEM10"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM11"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM12"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM13"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM14"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM15"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM16"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM17"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM18"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM19"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM1A"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM1B"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM1C"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM1D"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM1E"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM1F"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEM20"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM21"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM22"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM23"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM24"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM25"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM26"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM27"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM28"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM29"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM2A"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM2B"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM2C"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM2D"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM2E"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM2F"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEM30"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM31"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM32"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM33"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM34"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM35"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM36"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM37"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM38"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM39"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM3A"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM3B"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM3C"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM3D"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM3E"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM3F"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEM40"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM41"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM42"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM43"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM44"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM45"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM46"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM47"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM48"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM49"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM4A"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM4B"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM4C"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM4D"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM4E"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM4F"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEM50"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM51"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM52"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM53"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM54"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM55"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM56"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM57"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM58"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM59"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM5A"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM5B"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM5C"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM5D"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM5E"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM5F"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEM60"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM61"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM62"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM63"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM64"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM65"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM66"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM67"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM68"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM69"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM6A"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM6B"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM6C"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM6D"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM6E"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM6F"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEM70"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM71"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM72"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM73"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM74"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM75"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM76"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM77"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM78"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM79"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM7A"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM7B"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM7C"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM7D"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM7E"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM7F"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEM80"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM81"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM82"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM83"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM84"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM85"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM86"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM87"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM88"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM89"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM8A"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM8B"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM8C"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM8D"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM8E"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM8F"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEM90"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM91"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM92"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM93"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM94"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM95"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM96"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM97"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM98"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM99"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM9A"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM9B"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM9C"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM9D"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM9E"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEM9F"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEMA0"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMA1"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMA2"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMA3"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMA4"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMA5"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMA6"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMA7"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMA8"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMA9"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMAA"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMAB"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMAC"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMAD"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMAE"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMAF"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEMB0"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMB1"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMB2"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMB3"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMB4"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMB5"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMB6"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMB7"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMB8"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMB9"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMBA"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMBB"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMBC"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMBD"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMBE"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMBF"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEMC0"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMC1"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMC2"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMC3"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMC4"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMC5"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMC6"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMC7"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMC8"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMC9"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMCA"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMCB"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMCC"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMCD"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMCE"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMCF"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEMD0"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMD1"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMD2"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMD3"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMD4"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMD5"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMD6"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMD7"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMD8"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMD9"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMDA"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMDB"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMDC"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMDD"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMDE"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMDF"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEME0"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEME1"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEME2"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEME3"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEME4"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEME5"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEME6"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEME7"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEME8"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEME9"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMEA"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMEB"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMEC"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMED"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMEE"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMEF"), DebugWindow::onEditMemory)

	EVT_TEXT(XRCID("MEMF0"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMF1"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMF2"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMF3"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMF4"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMF5"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMF6"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMF7"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMF8"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMF9"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMFA"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMFB"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMFC"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMFD"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMFE"), DebugWindow::onEditMemory)
	EVT_TEXT(XRCID("MEMFF"), DebugWindow::onEditMemory)

	EVT_CHECKBOX(XRCID("Chip8DebugMode"), Main::onChip8DebugMode)
	EVT_BUTTON(XRCID("Chip8PauseButton"), DebugWindow::onChip8PauseButton)
	EVT_BUTTON(XRCID("Chip8ClearButton"), DebugWindow::onChip8Clear)
	EVT_BUTTON(XRCID("Chip8StepButton"), DebugWindow::onChip8StepButton)
	EVT_TOGGLEBUTTON(XRCID("Chip8TraceButton"), DebugWindow::onChip8Trace)
	EVT_CHECKBOX(XRCID("Chip8ProtectedMode"), DebugWindow::onChip8ProtectedMode)
	EVT_BUTTON(XRCID("Chip8TraceLog"), DebugWindow::onChip8Log)

	EVT_TEXT_ENTER(XRCID("Chip8BreakPointAddress"), DebugWindow::onChip8BreakPointSet) 
	EVT_BUTTON(XRCID("Chip8BreakPointSet"), DebugWindow::onChip8BreakPointSet)
	EVT_LIST_DELETE_ITEM(XRCID("Chip8BreakPointWindow"), DebugWindow::deleteChip8BreakPoint)
	EVT_LIST_END_LABEL_EDIT(XRCID("Chip8BreakPointWindow"), DebugWindow::editChip8BreakPoint)

	EVT_CHECKBOX(XRCID(GUIPROTECTEDMODE), DebugWindow::onProtectedMode)
END_EVENT_TABLE()

vector<wxString> dirAssFileNameVector(0);
vector<wxString> dirAssDirNameVector(0);
vector<Word> dirAssProgramStartVector(0);
vector<Word> dirAssProgramEndVector(0);
vector<Word> dirAssDataEndVector(0);
vector<Byte> dirAssSlotVector(0);

DebugWindow::DebugWindow(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiComx(title, pos, size, mode, dataDir, iniDir)
{
	traceString_ = "";
	updatingTraceString_ = false;
	numberOfBreakPoints_ = 0;
	numberOfChip8BreakPoints_ = 0;
	numberOfTraps_ = 0;
	numberOfTregs_ = 0;
	debugMode_ = false;
	chip8DebugMode_ = false;
	protectedMode_ = true;
	chip8ProtectedMode_ = true;
	traceDma_ = false;
	traceChip8Int_ = false; 
	traceInt_ = false;
	traceTrap_ = false;
	trace_ = false;
	chip8Trace_ = false;
	additionalChip8Details_ = false;
	performStep_ = false;
	steps_ = 1;
	memoryDisplay_ = CPU_MEMORY;
	debugAddress_ = 0;
	portExtender_ = 1;
	swName_ = "";
	spinning_ = false;
	showInstructionTrap_ = false;
	dataViewDump = true;
	selectedBreakPoint_ = -1;
	selectedChip8BreakPoint_ = -1;
	selectedTreg_ = -1;
	selectedTrap_ = -1;

    pauseOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/pause_on.png", wxBITMAP_TYPE_PNG);
    pauseOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/pause_off.png", wxBITMAP_TYPE_PNG);
    
#if defined(__WXMSW__)
	uncheckBitmap_ = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/unchecked.png", wxBITMAP_TYPE_PNG);
	checkedBitmap_ = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/checked.png", wxBITMAP_TYPE_PNG);
	imageList_ = new wxImageList(13, 13, true, 2);
	checkedButton_ = imageList_->Add(checkedBitmap_, wxColour (255, 255, 255));
	uncheckButton_ = imageList_->Add(uncheckBitmap_, wxColour (255, 255, 255));
#endif

	debugDir_ = readConfigDir("/Dir/Main/Debug", dataDir_);

	dirAssStart_ = 0;
	dirAssEnd_ = 0;
	lastRange_ = 0;
	workingRange_ = -1;
	shownRange_ = -1;
	lastAssError_ = "";

	numberOfDebugLines_ = (int)configPointer->Read("/Main/NumberOfDebugLines", NUMBER_OF_DEBUG_LINES);

    assBmp = new wxBitmap(ASS_WIDTH, numberOfDebugLines_*LINE_SPACE+4, 24);
}

DebugWindow::~DebugWindow()
{
    if (!mode_.gui)
    	return;

	for (int i=0; i<16; i++)
		delete lineBmp[i];
#if defined(__WXMSW__) 
	delete imageList_;
#endif
	delete assBmp;
}

void DebugWindow::readDebugConfig()
{
	dirAssConfigFile_ = configPointer->Read("/DebugConfigFile", "debug.config");

	dirAssConfigFileDir_ = readConfigDir("/Dir/Main/DebugConfig", dataDir_);
	debugDir_ = readConfigDir("/Dir/Main/Debug", dataDir_);

	numberOfDebugLines_ = (int)configPointer->Read("/Main/NumberOfDebugLines", NUMBER_OF_DEBUG_LINES);

	if (!mode_.gui)
		return;
#if defined (__WXMSW__) || (__WXMAC__)
	breakPointWindowPointer->SetImageList(imageList_, wxIMAGE_LIST_SMALL);
	chip8BreakPointWindowPointer->SetImageList(imageList_, wxIMAGE_LIST_SMALL);
	tregWindowPointer->SetImageList(imageList_, wxIMAGE_LIST_SMALL);
	trapWindowPointer->SetImageList(imageList_, wxIMAGE_LIST_SMALL);
#endif
	XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->setRange(1, 4);
	XRCCTRL(*this, "DebugExpansionRam", SlotEdit)->setRange(0, 3);
//	XRCCTRL(*this, "DebugExpansionEprom", SlotEdit)->setRange(0, 4);
	XRCCTRL(*this, "DebugPortExtender", HexEdit)->setStart(1);
    paintDebugBackground();
}

void DebugWindow::writeDebugConfig()
{
	configPointer->Write("/DebugConfigFile", dirAssConfigFile_);

	writeConfigDir("/Dir/Main/DebugConfig", dirAssConfigFileDir_);
	writeConfigDir("/Dir/Main/Debug", debugDir_);

	configPointer->Write("/Main/NumberOfDebugLines", numberOfDebugLines_);
}

void DebugWindow::enableDebugGuiMemory ()
{
	if (!mode_.gui)
		return;
	wxString value;
	switch (runningComputer_)
	{
		case COMX:
#ifndef __WXMAC__
            XRCCTRL(*this, "DebugExpansionSlotText", wxStaticText)->Enable(p_Comx->checkExpansionRomLoaded());
            XRCCTRL(*this, "DebugExpansionRamText", wxStaticText)->Enable(p_Comx->isRamCardActive());
            XRCCTRL(*this, "DebugExpansionEpromText", wxStaticText)->Enable(p_Comx->isEpromBoardLoaded() || p_Comx->isSuperBoardLoaded());
            XRCCTRL(*this, "DebugEmsPageText", wxStaticText)->Enable(false);
            XRCCTRL(*this, "DebugPagerText", wxStaticText)->Enable(false);
#endif
            XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->Enable(p_Comx->checkExpansionRomLoaded());
			XRCCTRL(*this, "DebugExpansionRam", SlotEdit)->Enable(p_Comx->isRamCardActive());
			XRCCTRL(*this, "DebugExpansionEprom", HexEdit)->Enable(p_Comx->isEpromBoardLoaded() || p_Comx->isSuperBoardLoaded());
			XRCCTRL(*this, "DebugEmsPage", HexEdit)->Enable(false);
			XRCCTRL(*this, "DebugPager", HexEdit)->Enable(false);
			XRCCTRL(*this, "DebugPortExtender", HexEdit)->Enable(false);
		break;

		case ELF:
		case ELFII:
		case SUPERELF:
#ifndef __WXMAC__
			XRCCTRL(*this, "DebugExpansionSlotText", wxStaticText)->Enable(false);
            XRCCTRL(*this, "DebugExpansionRamText", wxStaticText)->Enable(false);
            XRCCTRL(*this, "DebugExpansionEpromText", wxStaticText)->Enable(false);
            XRCCTRL(*this, "DebugEmsPageText", wxStaticText)->Enable(elfConfiguration[runningComputer_].useEms || elfConfiguration[runningComputer_].useRomMapper);
            XRCCTRL(*this, "DebugPagerText", wxStaticText)->Enable(elfConfiguration[runningComputer_].usePager);
#endif
			XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->Enable(false);
			XRCCTRL(*this, "DebugExpansionRam", SlotEdit)->Enable(false);
			XRCCTRL(*this, "DebugExpansionEprom", HexEdit)->Enable(false);
			XRCCTRL(*this, "DebugEmsPage", HexEdit)->Enable(elfConfiguration[runningComputer_].useEms || elfConfiguration[runningComputer_].useRomMapper);
			XRCCTRL(*this, "DebugPager", HexEdit)->Enable(elfConfiguration[runningComputer_].usePager);
			XRCCTRL(*this, "DebugPortExtender", HexEdit)->Enable(elfConfiguration[runningComputer_].usePager);
			p_Main->updateSlotInfo();
		break;

		default:
#ifndef __WXMAC__
			XRCCTRL(*this, "DebugExpansionSlotText", wxStaticText)->Enable(false);
            XRCCTRL(*this, "DebugExpansionRamText", wxStaticText)->Enable(false);
            XRCCTRL(*this, "DebugExpansionEpromText", wxStaticText)->Enable(false);
            XRCCTRL(*this, "DebugEmsPageText", wxStaticText)->Enable(false);
            XRCCTRL(*this, "DebugPagerText", wxStaticText)->Enable(false);
#endif
			XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->Enable(false);
			XRCCTRL(*this, "DebugExpansionRam", SlotEdit)->Enable(false);
			XRCCTRL(*this, "DebugExpansionEprom", HexEdit)->Enable(false);
			XRCCTRL(*this, "DebugEmsPage", HexEdit)->Enable(false);
			XRCCTRL(*this, "DebugPager", HexEdit)->Enable(false);
			XRCCTRL(*this, "DebugPortExtender", HexEdit)->Enable(false);
		break;
	}
}

void DebugWindow::enableChip8DebugGui(bool status)
{
	wxString text;

	XRCCTRL(*this,"Chip8PercentageClock", wxSlider)->Enable(status);
	XRCCTRL(*this,"Chip8PercentageClockText", wxStaticText)->Enable(status);
    XRCCTRL(*this,"Chip8TraceLog", wxButton)->Enable(status);
	XRCCTRL(*this,"Chip8PauseButton", wxBitmapButton)->Enable(status);
	XRCCTRL(*this,"Chip8StepButton", wxBitmapButton)->Enable(false);
	for (int i=0; i<16; i++)
	{
		chip8varTextPointer[i]->Enable(status&&!chip8ProtectedMode_);
		text.Printf("V%01XText",i);
		XRCCTRL(*this,text, wxStaticText)->Enable(status);
	}
	XRCCTRL(*this,"Chip8PCText", wxStaticText)->Enable(status);
	iTextPointer->Enable(status&&!chip8ProtectedMode_);
	XRCCTRL(*this,"Chip8IText", wxStaticText)->Enable(status);
	XRCCTRL(*this,"Chip8ProtectedMode", wxCheckBox)->Enable(status);

	if (computerRunning_)
		setChip8PauseState();
	else
		XRCCTRL(*this, "Chip8PauseButton", wxBitmapButton)->SetBitmapLabel(pauseOffBitmap);
}

void DebugWindow::enableDebugGui(bool status)
{
	wxString text;

	for (int i=0; i<16; i++)
	{
		registerTextPointer[i]->Enable(status&&!protectedMode_);
        text.Printf("R%dText",i);
		XRCCTRL(*this,text, wxStaticText)->Enable(status);
	}
	for (int i=1; i<8; i++)
	{
		text.Printf("I%dText",i);
		XRCCTRL(*this,text, wxStaticText)->Enable(status);
        text.Printf("O%dText",i);
        XRCCTRL(*this,text, wxStaticText)->Enable(status);
//        text.Printf("O%d",i);
		outTextPointer[i]->Enable(status&&!protectedMode_);
	}
	for (int i=1; i<5; i++)
	{
		text.Printf("EF%dText",i);
		XRCCTRL(*this,text, wxStaticText)->Enable(status);
	}
    XRCCTRL(*this,"DFText", wxStaticText)->Enable(status);
    XRCCTRL(*this,"QText", wxStaticText)->Enable(status);
    XRCCTRL(*this,"IEText", wxStaticText)->Enable(status);
    dfTextPointer->Enable(status&&!protectedMode_);
	qTextPointer->Enable(status&&!protectedMode_);
	ieTextPointer->Enable(status&&!protectedMode_);
	efTextPointer[1]->Enable(status&&!protectedMode_);
	efTextPointer[2]->Enable(status&&!protectedMode_);
	efTextPointer[3]->Enable(status&&!protectedMode_);
	efTextPointer[4]->Enable(status&&!protectedMode_);
	dTextPointer->Enable(status&&!protectedMode_);
	pTextPointer->Enable(status&&!protectedMode_);
	xTextPointer->Enable(status&&!protectedMode_);
	tTextPointer->Enable(status&&!protectedMode_);
//	XRCCTRL(*this,"InputWindow", wxTextCtrl)->Enable(status);
//	XRCCTRL(*this,"DebugAssemblerAddress", wxTextCtrl)->Enable(status);
	XRCCTRL(*this,"ProtectedMode", wxCheckBox)->Enable(status);
	XRCCTRL(*this,"DebugRunButton", wxButton)->Enable(status);
    XRCCTRL(*this,"DebugInterrupt", wxButton)->Enable(status);
    XRCCTRL(*this,"DebugReset", wxButton)->Enable(status);
//	XRCCTRL(*this,"DebugDis", wxButton)->Enable(status);
//	XRCCTRL(*this,"DebugDisLog", wxButton)->Enable(status);
	XRCCTRL(*this,"DebugCopy", wxButton)->Enable(status);
	XRCCTRL(*this, "DebugSave", wxButton)->Enable(status);
	XRCCTRL(*this,"DebugPauseButton", wxBitmapButton)->Enable(status);
	XRCCTRL(*this,"DebugStepButton", wxBitmapButton)->Enable(false);

	if (computerRunning_)
		setPauseState();
	else
		XRCCTRL(*this, "DebugPauseButton", wxBitmapButton)->SetBitmapLabel(pauseOffBitmap);

	XRCCTRL(*this, "DebugDisplayPage", wxTextCtrl)->Enable(memoryDisplay_ != CPU_TYPE);

	if (!status)
	{
		XRCCTRL(*this, "DebugPortExtender", HexEdit)->Enable(false);
		XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->Enable(false);
		XRCCTRL(*this, "DebugExpansionRam", SlotEdit)->Enable(false);
		XRCCTRL(*this, "DebugExpansionEprom", HexEdit)->Enable(false);
		XRCCTRL(*this, "DebugEmsPage", HexEdit)->Enable(false);
		XRCCTRL(*this, "DebugPager", HexEdit)->Enable(false);
		XRCCTRL(*this, "DebugSave", wxButton)->Enable(false);
#ifndef __WXMAC__
        XRCCTRL(*this, "DebugExpansionSlotText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "DebugPagerText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "DebugEmsPageText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "DebugExpansionEpromText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "DebugExpansionRamText", wxStaticText)->Enable(false);
#endif
    }
}

void DebugWindow::updateAssTabCheck(Word address)
{
	if (address>= dirAssStart_ && address<dirAssEnd_)
		p_Main->updateAssTab();
}


void DebugWindow::cycleChip8Debug()
{
	if (p_Computer->getChip8Type() == CHIP_NONE)
		return;

	Byte programCounter = p_Computer->getProgramCounter();
	Word programCounterAddress = p_Computer->getScratchpadRegister(programCounter);

	if (selectedComputer_ == DEBUGGER && debuggerChoice_ == CHIP8TAB)
	{
		if (programCounterAddress == p_Computer->getChip8MainLoop())
			updateChip8Window();
	}

	Word chip8PC = p_Computer->getScratchpadRegister(CHIP8_PC);

	if (programCounterAddress == p_Computer->getChip8MainLoop())
	{
		p_Computer->writeMemDataType(chip8PC, MEM_TYPE_CHIP_8_1);
		p_Computer->writeMemDataType((chip8PC+1)&0xffff, MEM_TYPE_CHIP_8_2);
		p_Main->updateAssTabCheck(chip8PC);
	}

	if (chip8DebugMode_)
	{
		if (chip8Steps_ >= 0)
		{
			if (programCounterAddress == p_Computer->getChip8MainLoop())
			{
				if (additionalChip8Details_)
					chip8DebugTrace(chip8Disassemble(chip8PC-2, true, false));
				if (chip8Steps_ == 1)
				{
					p_Computer->setSteps(0);
					chip8Steps_--;
					setChip8PauseState();
				}
				if (chip8Steps_ != 0)
				{
					if (chip8BreakPointCheck())  return;
					if (chip8Trace_)
						chip8Trace(chip8PC);
					chip8Steps_--;
				}
			}
		}
		else
		{
			if (programCounterAddress == p_Computer->getChip8MainLoop())
			{
				if (additionalChip8Details_)
					chip8DebugTrace(chip8Disassemble(chip8PC-2, true, false));
				if (chip8BreakPointCheck())  return;
				if (chip8Trace_)
					chip8Trace(chip8PC);
			}
		}
	}
}

void DebugWindow::cycleFredDebug()
{
    if (p_Computer->getChip8Type() == CHIP_NONE)
        return;
    
    Byte programCounter = p_Computer->getProgramCounter();
    Word programCounterAddress = p_Computer->getScratchpadRegister(programCounter);
    
    if (selectedComputer_ == DEBUGGER && debuggerChoice_ == CHIP8TAB)
    {
        if (programCounterAddress == p_Computer->getChip8MainLoop())
            updateChip8Window();
    }
    
    Word chip8PC = p_Computer->getScratchpadRegister(CHIP8_PC);
    
    if (programCounterAddress == p_Computer->getChip8MainLoop())
    {
        p_Computer->writeMemDataType(chip8PC, MEM_TYPE_FEL2_1);
        switch (p_Computer->readMem(chip8PC))
        {
            case 0xC0:
            case 0xC4:
            case 0xCC:
                if (p_Computer->getChip8Type() != CHIPFEL2)
                    p_Computer->writeMemDataType((chip8PC+1)&0xffff, MEM_TYPE_FEL2_2);
            break;
                
            default:
                p_Computer->writeMemDataType((chip8PC+1)&0xffff, MEM_TYPE_FEL2_2);
            break;
        }
        p_Main->updateAssTabCheck(chip8PC);
    }
    
    if (chip8DebugMode_)
    {
        if (chip8Steps_ >= 0)
        {
            if (programCounterAddress == p_Computer->getChip8MainLoop())
            {
                if (additionalChip8Details_)
                    chip8DebugTrace(fel2Disassemble(chip8PC-2, true, false));
                if (chip8Steps_ == 1)
                {
                    p_Computer->setSteps(0);
                    chip8Steps_--;
                    setChip8PauseState();
                }
                if (chip8Steps_ != 0)
                {
                    if (chip8BreakPointCheck())  return;
                    if (chip8Trace_)
                        fredTrace(chip8PC);
                    chip8Steps_--;
                }
            }
        }
        else
        {
            if (programCounterAddress == p_Computer->getChip8MainLoop())
            {
                if (additionalChip8Details_)
                    chip8DebugTrace(fel2Disassemble(chip8PC-2, true, false));
                if (chip8BreakPointCheck())  return;
                if (chip8Trace_)
                    fredTrace(chip8PC);
            }
        }
    }
}

void DebugWindow::cycleSt2Debug()
{
	Byte programCounter = p_Computer->getProgramCounter();
	Word programCounterAddress = p_Computer->getScratchpadRegister(programCounter);

	if (selectedComputer_ == DEBUGGER && debuggerChoice_ == CHIP8TAB)
	{
		if (programCounterAddress == p_Computer->getChip8MainLoop())
			updateChip8Window();
	}

	Word chip8PC = p_Computer->getScratchpadRegister(CHIP8_PC);

	if (programCounterAddress == p_Computer->getChip8MainLoop())
	{
		p_Computer->writeMemDataType(chip8PC, MEM_TYPE_ST2_1);
		switch (p_Computer->readMem(chip8PC))
		{
			case 0xC0:
			case 0xE0:
			case 0xE1:
			case 0xE2:
			case 0xE4:
			break;

			default:		
				p_Computer->writeMemDataType((chip8PC+1)&0xffff, MEM_TYPE_ST2_2);
			break;
		}
		p_Main->updateAssTabCheck(chip8PC);
	}

	if (chip8DebugMode_)
	{
		if (chip8Steps_ >= 0)
		{
			if (programCounterAddress == p_Computer->getChip8MainLoop())
			{
				if (additionalChip8Details_)
					chip8DebugTrace(chip8Disassemble(chip8PC-2, true, false));
				if (chip8Steps_ == 1)
				{
					p_Computer->setSteps(0);
					chip8Steps_--;
					setChip8PauseState();
				}
				if (chip8Steps_ != 0)
				{
					if (chip8BreakPointCheck())  return;
					if (chip8Trace_)
						st2Trace(chip8PC);
					chip8Steps_--;
				}
			}
		}
		else
		{
			if (programCounterAddress == p_Computer->getChip8MainLoop())
			{
				if (additionalChip8Details_)
					chip8DebugTrace(chip8Disassemble(chip8PC-2, true, false));
				if (chip8BreakPointCheck())  return;
				if (chip8Trace_)
					st2Trace(chip8PC);
			}
		}
	}
}

bool DebugWindow::chip8BreakPointCheck()
{
	Word chip8PC = p_Computer->getScratchpadRegister(CHIP8_PC);
	if (chip8Steps_ != 0 && numberOfChip8BreakPoints_ > 0 && !performChip8Step_)
	{
		for (int i=0; i<numberOfChip8BreakPoints_; i++)
		{
			if (chip8BreakPoints_[i] == chip8PC && chip8BreakPointsSelected_[i])
			{
				chip8DebugTrace("Hit Breakpoint");
				p_Computer->setSteps(0);
				chip8Steps_ = 0;
				setChip8PauseState();
				return true;
			}
		}
	}
	performChip8Step_ = false;
	return false;
}

void DebugWindow::showInstructionTrace()
{
	if (showInstructionTrap_)
	{
		if (traceTrap_ && !trace_ && steps_ != 0)
		{
			showInstructionTrap_ = false;
			debugTrace(cdp1802disassemble(&showInstructionTrapAddress_, true, false, DIRECT_ASSEMBLER, 0, 0xFFFF));
		}
	}
}

void DebugWindow::cycleDebug()
{
	wxString printBuffer, printBuffer2;
	bool trapFound = false;

	Byte programCounter = p_Computer->getProgramCounter();
	Word programCounterAddress = p_Computer->getScratchpadRegister(programCounter);
	Byte mask;
	Byte instruction;

	if (percentageClock_ != 1)
		if (selectedComputer_ == DEBUGGER && debuggerChoice_ == TRACETAB)
			p_Main->updateWindow();

	if (p_Computer->getSteps() != 0 && numberOfBreakPoints_ > 0)
	{
		for (int i=0; i<numberOfBreakPoints_; i++)
		{
			if (breakPoints_[i] == programCounterAddress && breakPointsSelected_[i])
			{
				p_Computer->setSteps(0);
				debugTrace("Hit Breakpoint");
				setPauseState();
			}
		}
	}
	if (p_Computer->getSteps() != 0 && numberOfTraps_ > 0)
	{
		for (int i=0; i<numberOfTraps_; i++)
		{
			mask = traps_[i][7];
			instruction = p_Computer->readMem(programCounterAddress);
			if (instruction == 0x60)
				mask = 0xff;
			if (instruction == 0x68)
			{
				if ((traps_[i][0] == 1 && traps_[i][1] == instruction && trapsSelected_[i]) ||
					(traps_[i][0] == 2 && traps_[i][1] == instruction &&
										 (traps_[i][2]&mask) == (p_Computer->readMem(programCounterAddress+1)&mask) && trapsSelected_[i]) ||
					(traps_[i][0] == 3 && traps_[i][1] == instruction &&
										 (traps_[i][2]&mask) == (p_Computer->readMem(programCounterAddress+1)&mask) &&
										 traps_[i][3] == p_Computer->readMem(programCounterAddress+2) && trapsSelected_[i]) ||
					(traps_[i][0] == 4 && traps_[i][1] == instruction &&
										 (traps_[i][2]&mask) == (p_Computer->readMem(programCounterAddress+1)&mask) &&
										 traps_[i][3] == p_Computer->readMem(programCounterAddress+2) &&
										 traps_[i][4] == p_Computer->readMem(programCounterAddress+3) && trapsSelected_[i]))
				{
					trapFound = true;
				}
			}
			else
			{
				if ((traps_[i][0] == 1 && (traps_[i][1]&mask) == (instruction&mask) && trapsSelected_[i]) ||
					(traps_[i][0] == 2 && traps_[i][1] == instruction &&
										 traps_[i][2] == p_Computer->readMem(programCounterAddress+1) && trapsSelected_[i]) ||
					(traps_[i][0] == 3 && traps_[i][1] == instruction &&
										 traps_[i][2] == p_Computer->readMem(programCounterAddress+1) &&
										 traps_[i][3] == p_Computer->readMem(programCounterAddress+2) && trapsSelected_[i]))
				{
					trapFound = true;
				}
			}
			if (trapFound)
			{
				if (traceTrap_)
				{
					if (!trace_)
					{
						showInstructionTrap_ = true;
						showInstructionTrapAddress_ = programCounterAddress;
					}
				}
				else
				{
					p_Computer->setSteps(0);
					debugTrace("Instruction Trap");
					setPauseState();
				}
			}
		}
	}
	if (p_Computer->getSteps() != 0 && numberOfTregs_ > 0)
	{
		int j = -1;
		printBuffer2 = "Register Trap: ";
		for (int i=0; i<numberOfTregs_; i++)
		{
			if (tregsSelected_[i])
			{
				if (tregs_[i][0] == TREG_D && tregs_[i][1] == p_Computer->getAccumulator())
				{
					printBuffer.Printf("D=%02X", p_Computer->getAccumulator());
					j = i;
				}
				if (tregs_[i][0] == TREG_DF && tregs_[i][1] == p_Computer->getDataFlag())
				{
					printBuffer.Printf("DF=%X", p_Computer->getDataFlag());
					j = i;
				}
				if (tregs_[i][0] == TREG_P && tregs_[i][1] == programCounter)
				{
					printBuffer.Printf("P=%X", programCounter);
					j = i;
				}
				if (tregs_[i][0] == TREG_X && tregs_[i][1] == p_Computer->getDataPointer())
				{
					printBuffer.Printf("X=%X", p_Computer->getDataPointer());
					j = i;
				}
				if (tregs_[i][0] == TREG_T && tregs_[i][1] == p_Computer->getRegisterT())
				{
					printBuffer.Printf("X=%02X",p_Computer->getRegisterT());
					j = i;
				}
				if (tregs_[i][0] == TREG_Q && tregs_[i][1] == p_Computer->getFlipFlopQ())
				{
					printBuffer.Printf("Q=%X", p_Computer->getFlipFlopQ());
					j = i;
				}
				if (tregs_[i][0] >= TREG_R0 && tregs_[i][0] <= TREG_RF &&
					tregs_[i][1] == p_Computer->getScratchpadRegister(tregs_[i][0]-TREG_R0))
				{
					printBuffer.Printf("R%X=%04X",tregs_[i][0]-TREG_R0,tregs_[i][1]);
					j = i;
				}
			}
		}
		if (j >= 0)
		{
			p_Computer->setSteps(0);
			printBuffer2.operator += (printBuffer);
			debugTrace(printBuffer2);
			setPauseState();
		}
	}

	if (p_Computer->getSteps() > 0)
	{
		p_Computer->setSteps(p_Computer->getSteps()-1);

		if (p_Computer->getSteps() == 0)
			setPauseState();
	}

	if (performStep_)
	{
		if (traceTrap_)
		{
			if (!trace_)
			{
				for (int i=0; i<numberOfTraps_; i++)
				{
					mask = traps_[i][7];
					instruction = p_Computer->readMem(programCounterAddress);
					if (instruction == 0x60)
						mask = 0xff;
					if (instruction == 0x68)
					{
						if ((traps_[i][0] == 1 && traps_[i][1] == instruction && trapsSelected_[i]) ||
							(traps_[i][0] == 2 && traps_[i][1] == instruction &&
												 (traps_[i][2]&mask) == (p_Computer->readMem(programCounterAddress+1)&mask) && trapsSelected_[i]) ||
							(traps_[i][0] == 3 && traps_[i][1] == instruction &&
												 (traps_[i][2]&mask) == (p_Computer->readMem(programCounterAddress+1)&mask) &&
												 traps_[i][3] == p_Computer->readMem(programCounterAddress+2) && trapsSelected_[i]) ||
							(traps_[i][0] == 4 && traps_[i][1] == instruction &&
												 (traps_[i][2]&mask) == (p_Computer->readMem(programCounterAddress+1)&mask) &&
												 traps_[i][3] == p_Computer->readMem(programCounterAddress+2) &&
												 traps_[i][4] == p_Computer->readMem(programCounterAddress+3) && trapsSelected_[i]))
						{
							trapFound = true;
						}
					}
					else
					{
						if ((traps_[i][0] == 1 && (traps_[i][1]&mask) == (instruction&mask) && trapsSelected_[i]) ||
							(traps_[i][0] == 2 && traps_[i][1] == instruction &&
												 traps_[i][2] == p_Computer->readMem(programCounterAddress+1) && trapsSelected_[i]) ||
							(traps_[i][0] == 3 && traps_[i][1] == instruction &&
												 traps_[i][2] == p_Computer->readMem(programCounterAddress+1) &&
												 traps_[i][3] == p_Computer->readMem(programCounterAddress+2) && trapsSelected_[i]))
						{
							trapFound = true;
						}
					}
					if (trapFound)
					{
						showInstructionTrap_ = true;
						showInstructionTrapAddress_ = programCounterAddress;
					}
				}
			}
		}
		p_Computer->setSteps(steps_);
		performStep_ = false;
		setPauseState();
	}
}

void DebugWindow::resetDisplay()
{
	for (int i=0; i<16; i++) lastR_[i] = p_Computer->getScratchpadRegister(i)+1;
	for (int i=1; i<8; i++) lastOut_[i] = p_Computer->getOutValue(i) + 1;
	for (int i=1; i<8; i++) lastIn_[i] = p_Computer->getInValue(i) + 1;
	lastD_ = p_Computer->getAccumulator() + 1;
	lastP_ = p_Computer->getProgramCounter() + 1;
	lastX = p_Computer->getDataPointer() + 1;
	lastT_ = p_Computer->getRegisterT() + 1;
	lastDf_ = p_Computer->getDataFlag() ^ 1;
	lastQ_ = p_Computer->getFlipFlopQ() ^ 1;
	lastIe_ = p_Computer->getInterruptEnable() ^ 1;
	Byte cpuFlag = p_Computer->getEfFlags();
	lastEf1_ = (cpuFlag & 1) ^ 0x1;
	lastEf2_ = (cpuFlag & 2) ^ 0x2;
	lastEf3_ = (cpuFlag & 4) ^ 0x4;
	lastEf4_ = (cpuFlag & 8) ^ 0x8;
	updateWindow();
}

void DebugWindow::updateChip8Window()
{
	wxString buffer;
	Word scratchpadRegister;
#if defined(__linux__)
	if (!wxIsMainThread())
		wxMutexGuiEnter();
#endif
	scratchpadRegister = p_Computer->getScratchpadRegister(CHIP8_PC);
	if (scratchpadRegister != lastPC_)
	{
        if (chip8Type_ == CHIPSTIV)
            buffer.Printf("%04X", scratchpadRegister);
        else
            buffer.Printf("%03X", scratchpadRegister&0xfff);
		pcTextPointer->ChangeValue(buffer);
		lastPC_ = scratchpadRegister;
	}
    if (chip8Type_ == CHIPSTIV)
        scratchpadRegister = (p_Computer->readMem(0x27f6)<<8)+p_Computer->readMem(0x27f7);
    else
        scratchpadRegister = p_Computer->getScratchpadRegister(CHIP8_I);
	if (scratchpadRegister != lastI_)
	{
        if (chip8Type_ == CHIPSTIV)
            buffer.Printf("%04X", scratchpadRegister);
        else
            buffer.Printf("%03X", scratchpadRegister&0xfff);
		iTextPointer->ChangeValue(buffer);
		lastI_ = scratchpadRegister;
	}
	p_Computer->showChip8Registers();
#if defined(__linux__)
	if (!wxIsMainThread())
		wxMutexGuiLeave();
#endif
}

void DebugWindow::updateWindow()
{
	wxString buffer;
	Word scratchpadRegister;
	Byte cpucpuRegister;
	Byte cpuFlag;

#if defined(__linux__)
	if (!wxIsMainThread())
		wxMutexGuiEnter();
#endif
	for (int i=0; i<16; i++)
	{
		scratchpadRegister = p_Computer->getScratchpadRegister(i);
		if (scratchpadRegister != lastR_[i])
		{
			buffer.Printf("%04X", scratchpadRegister);
			registerTextPointer[i]->ChangeValue(buffer);
			lastR_[i] = scratchpadRegister;
		}
	}

	for (int i=1; i<8; i++)
	{
		if (p_Computer->getOutValue(i) != lastOut_[i])
		{
			switch (runningComputer_)
			{
				case COMX:
				case CIDELSA:
				case PECOM:
					if (i>3)
						buffer.Printf("%04X",p_Computer->getOutValue(i));
					else
						buffer.Printf("%02X",p_Computer->getOutValue(i));
				break;

				case TMC600:
					if (i==5 && (p_Computer->getOutValue(7) != 0x20) && (p_Computer->getOutValue(7) != 0x30))
						buffer.Printf("%04X",p_Computer->getOutValue(i));
					else
						buffer.Printf("%02X",p_Computer->getOutValue(i));
				break;

				default:
					buffer.Printf("%02X",p_Computer->getOutValue(i));
				break;
			}
			outTextPointer[i]->ChangeValue(buffer);
			lastOut_[i] = p_Computer->getOutValue(i);
		}
		if (p_Computer->getInValue(i) != lastIn_[i])
		{
			buffer.Printf("%02X",p_Computer->getInValue(i));
			inTextPointer[i]->ChangeValue(buffer);
			lastIn_[i] = p_Computer->getInValue(i);
		}
	}

	cpucpuRegister = p_Computer->getAccumulator();
 	if (cpucpuRegister != lastD_)
	{
		buffer.Printf("%02X", cpucpuRegister);
		dTextPointer->ChangeValue(buffer);
		lastD_ = cpucpuRegister;
	}
	cpucpuRegister = p_Computer->getProgramCounter();
	if (cpucpuRegister != lastP_)
	{
		buffer.Printf("%01X", cpucpuRegister);
		pTextPointer->ChangeValue(buffer);
		lastP_ = cpucpuRegister;
	}
	cpucpuRegister = p_Computer->getDataPointer();
	if (cpucpuRegister != lastX)
	{
		buffer.Printf("%01X", cpucpuRegister);
		xTextPointer->ChangeValue(buffer);
		lastX = cpucpuRegister;
	}
	cpucpuRegister = p_Computer->getRegisterT();
	if (cpucpuRegister != lastT_)
	{
		buffer.Printf("%02X", cpucpuRegister);
		tTextPointer->ChangeValue(buffer);
		lastT_ = cpucpuRegister;
	}

	cpuFlag = p_Computer->getDataFlag();
	if (cpuFlag != lastDf_)
	{
		buffer.Printf("%01X", cpuFlag);
		dfTextPointer->ChangeValue(buffer);
		lastDf_ = cpuFlag;
	}
	cpuFlag = p_Computer->getFlipFlopQ();
	if (cpuFlag != lastQ_)
	{
		buffer.Printf("%01X", cpuFlag);
		qTextPointer->ChangeValue(buffer);
		lastQ_ = cpuFlag;
	}
	cpuFlag = p_Computer->getInterruptEnable();
	if (cpuFlag != lastIe_)
	{
		buffer.Printf("%01X", cpuFlag);
		ieTextPointer->ChangeValue(buffer);
		lastIe_ = cpuFlag;
	}
	cpuFlag = p_Computer->getEfFlags();
	if ((cpuFlag & 1) != lastEf1_)
	{
		buffer.Printf("%01X", cpuFlag & 1);
		efTextPointer[1]->ChangeValue(buffer);
		lastEf1_ = cpuFlag & 1;
	}
	if ((cpuFlag & 2) != lastEf2_)
	{
		buffer.Printf("%01X", (cpuFlag >> 1) & 1);
		efTextPointer[2]->ChangeValue(buffer);
		lastEf2_ = cpuFlag & 2;
	}
	if ((cpuFlag & 4) != lastEf3_)
	{
		buffer.Printf("%01X", (cpuFlag >> 2) & 1);
		efTextPointer[3]->ChangeValue(buffer);
		lastEf3_ = cpuFlag & 4;
	}
	if ((cpuFlag & 8) != lastEf4_)
	{
		buffer.Printf("%01X", (cpuFlag >> 3) & 1);
		efTextPointer[4]->ChangeValue(buffer);
		lastEf4_ = cpuFlag & 8;
	}

#if defined(__linux__)
	if (!wxIsMainThread())
		wxMutexGuiLeave();
#endif
}

void DebugWindow::debugTrace(wxString buffer)
{
	if (!debugMode_)  return;
#if defined(__WXMAC__) || defined(__linux__)
	traceString_ = traceString_ + buffer + "\n";
#else
	traceWindowPointer->AppendText(buffer+"\n");
#endif
}

void DebugWindow::chip8DebugTrace(wxString buffer)
{
	if (!chip8DebugMode_)  return;
#if defined(__WXMAC__) || defined(__linux__)
    chipTraceString_ = chipTraceString_ + buffer;
    if (!additionalChip8Details_)
        chipTraceString_ = chipTraceString_ + "\n";
#else
    chip8TraceWindowPointer->AppendText(buffer);
    if (!additionalChip8Details_)
        chip8TraceWindowPointer->AppendText("\n");
#endif
}

void DebugWindow::deleteBreakPoint(wxListEvent&event)
{
	long selectedItem = event.GetIndex();

	if (selectedItem > -1)
	{
		for (long i=selectedItem; i<numberOfBreakPoints_-1; i++)
			breakPoints_[i] = breakPoints_[i+1];
		numberOfBreakPoints_--;
		SetDebugMode();
	}
}

void DebugWindow::deleteTrap(wxListEvent&event)
{
	long selectedItem = event.GetIndex();

	if (selectedItem > -1)
	{
		for (long j=selectedItem; j<numberOfTraps_-1; j++)
		{
			traps_[j][0] = traps_[j+1][0];
			traps_[j][1] = traps_[j+1][1];
			traps_[j][2] = traps_[j+1][2];
			traps_[j][3] = traps_[j+1][3];
			traps_[j][4] = traps_[j+1][4];
			traps_[j][5] = traps_[j+1][5];
			traps_[j][6] = traps_[j+1][6];
			traps_[j][7] = traps_[j+1][7];
		}
		numberOfTraps_--;
		SetDebugMode();
	}
}

void DebugWindow::deleteTreg(wxListEvent&event)
{
	long selectedItem = event.GetIndex();

	if (selectedItem > -1)
	{
		for (long j=selectedItem; j<numberOfTregs_-1; j++)
		{
			tregs_[j][0] = tregs_[j+1][0];
			tregs_[j][1] = tregs_[j+1][1];
		}
		numberOfTregs_--;
		SetDebugMode();
	}
}

void DebugWindow::editBreakPoint(wxListEvent&event)
{
	wxString bpStr = event.GetText();
	long selectedItem = event.GetIndex();

	if (bpStr == "")
	{
		breakPointWindowPointer->DeleteItem(selectedItem);
		event.Veto();
		return;
	}

	wxString strValue = extractWord (&bpStr);
	long value;

	if (!strValue.ToLong(&value, 16))
	{
		(void)wxMessageBox( "Please specify value in hexadecimal\n",
									"Emma 02", wxICON_ERROR | wxOK );
		event.Veto();
		return;
	}

	if (value > 0xffff)
	{
		(void)wxMessageBox( "Please specify value of 16 bit max\n",
									"Emma 02", wxICON_ERROR | wxOK );
		event.Veto();
		return;
	}

	breakPoints_[selectedItem] = value;
}

void DebugWindow::editTreg(wxListEvent&event)
{
	wxString tregStr = event.GetText();
	long selectedItem = event.GetIndex();

	if (tregStr == "")
	{
		tregWindowPointer->DeleteItem(selectedItem);
		event.Veto();
		return;
	}

	wxString registerStr = extractWord (&tregStr);
	int registerValue = getRegister (registerStr);

	if (registerValue != TREG_FAULT)
	{
		wxString strValue = extractWord (&tregStr);
		long value;

		if (!strValue.ToLong(&value, 16))
		{
			(void)wxMessageBox( "Please specify value in hexadecimal\n",
										"Emma 02", wxICON_ERROR | wxOK );
			event.Veto();
			return;
		}

		if ((registerValue >= TREG_R0) && (registerValue <= TREG_RF))
		{
			if (value > 0xffff)
			{
				(void)wxMessageBox( "Please specify value of 16 bit max\n",
											"Emma 02", wxICON_ERROR | wxOK );
				event.Veto();
				return;
			}
		}
		else
		{
			if (registerValue <= TREG_T)
			{
				if (value > 0xff)
				{
					(void)wxMessageBox( "Please specify value of 8 bit max\n",
												"Emma 02", wxICON_ERROR | wxOK );
					event.Veto();
					return;
				}
			}
			else
			{
				if (value > 1)
				{
					(void)wxMessageBox( "Please specify value of 0 or 1\n",
												"Emma 02", wxICON_ERROR | wxOK );
					event.Veto();
					return;
				}
			}
		}

		tregs_[selectedItem][0] = registerValue;
		tregs_[selectedItem][1] = value;
	}
	else
	{
		(void)wxMessageBox( "Please specify R0 to RF, D, P, X, T, DF or Q\n",
									"Emma 02", wxICON_ERROR | wxOK );
		event.Veto();
		return;
	}
}

void DebugWindow::editTrap(wxListEvent&event)
{
	wxString trapStr = event.GetText();
	trapStr = trapStr.MakeUpper();
	wxString error;
	long selectedItem = event.GetIndex();

	if (trapStr == "")
	{
		trapWindowPointer->DeleteItem(selectedItem);
		event.Veto();
		return;
	}

	Word count;
	Byte b1, b2, b3, b4, b5, b6, b7;
	int typeOpcode=0;

	count = assemble(&trapStr, &b1, &b2, &b3, &b4, &b5, &b6, &b7, true);

	if (count >10 && count <15)
		count = ERROR_INST;

	if (count == 21)
		count = ERROR_INST;

	if (count >= MEM_TYPE_OPCODE_RSHR)
	{
		switch (count)
		{
			case MEM_TYPE_OPCODE_RSHR:
				typeOpcode = MEM_TYPE_OPCODE_RSHR;
				count = 1;
			break;
			case MEM_TYPE_OPCODE_RSHL:
				typeOpcode = MEM_TYPE_OPCODE_RSHL;
				count = 1;
			break;
			case MEM_TYPE_OPCODE_BPZ:
				typeOpcode = MEM_TYPE_OPCODE_BPZ;
				count = 2;
			break;
			case MEM_TYPE_OPCODE_BGE:
				typeOpcode = MEM_TYPE_OPCODE_BGE;
				count = 2;
			break;
			case MEM_TYPE_OPCODE_BM:
				typeOpcode = MEM_TYPE_OPCODE_BM;
				count = 2;
			break;
			case MEM_TYPE_OPCODE_BL:
				typeOpcode = MEM_TYPE_OPCODE_BL;
				count = 2;
			break;
			case MEM_TYPE_OPCODE_LSKP:
				typeOpcode = MEM_TYPE_OPCODE_LSKP;
				count = 1;
			break;
			case MEM_TYPE_OPCODE_SKP:
				typeOpcode = MEM_TYPE_OPCODE_SKP;
				count = 1;
			break;
			case MEM_TYPE_OPCODE_RLDL:
			case MEM_TYPE_OPCODE_LDV:
			case MEM_TYPE_OPCODE_LDL:
			case MEM_TYPE_OPCODE_LDL_SLOT:
			case MEM_TYPE_OPCODE_LBR_SLOT:
			case MEM_TYPE_OPCODE_JUMP_SLOT:
				count = ERROR_INST;
			break;
		}
	}
	if (count > 0 && count < 7)
	{
		traps_[selectedItem][0] = count;
		traps_[selectedItem][1] = b1;
		traps_[selectedItem][2] = b2;
		traps_[selectedItem][3] = b3;
		traps_[selectedItem][4] = b4;
		traps_[selectedItem][5] = b5; 
		traps_[selectedItem][6] = b6; 
		traps_[selectedItem][7] = b7; 
		traps_[selectedItem][8] = typeOpcode; 
	}
	else
	{
		(void)wxMessageBox( DirAssErrorCodes[count-ERROR_START-1]+"\n",
									"Emma 02", wxICON_ERROR | wxOK );
		event.Veto();
		return;
	}
}

void DebugWindow::switchBreakPoint(int item)
{
	breakPointsSelected_[item] = !breakPointsSelected_[item] ;
	if (breakPointsSelected_[item])
		breakPointWindowPointer->SetItemImage(item, checkedButton_, -1);
	else
		breakPointWindowPointer->SetItemImage(item, uncheckButton_, -1);
}

void DebugWindow::selectBreakPoint(wxListEvent&event)
{
	selectedBreakPoint_ = (int)event.GetIndex();
}

void DebugWindow::deselectBreakPoint(wxListEvent&WXUNUSED(event))
{
	selectedBreakPoint_ = -1;
}

void DebugWindow::keyBreakPoint(wxListEvent&event)
{
	const int keyCode = event.GetKeyCode();
	if (keyCode == WXK_DELETE && selectedBreakPoint_ != -1)
  		XRCCTRL(*this,"BreakPointWindow", wxListCtrl)->DeleteItem(selectedBreakPoint_);
}

void DebugWindow::switchChip8BreakPoint(int item)
{
	chip8BreakPointsSelected_[item] = !chip8BreakPointsSelected_[item] ;
	if (chip8BreakPointsSelected_[item])
		chip8BreakPointWindowPointer->SetItemImage(item, checkedButton_, -1);
	else
		chip8BreakPointWindowPointer->SetItemImage(item, uncheckButton_, -1);
}

void DebugWindow::selectChip8BreakPoint(wxListEvent&event)
{
	selectedChip8BreakPoint_ = (int)event.GetIndex();
}

void DebugWindow::deselectChip8BreakPoint(wxListEvent&WXUNUSED(event))
{
	selectedChip8BreakPoint_ = -1;
}

void DebugWindow::keyChip8BreakPoint(wxListEvent&event)
{
	const int keyCode = event.GetKeyCode();
	if (keyCode == WXK_DELETE && selectedChip8BreakPoint_ != -1)
  		XRCCTRL(*this,"Chip8BreakPointWindow", wxListCtrl)->DeleteItem(selectedChip8BreakPoint_);
}

void DebugWindow::switchTreg(int item)
{
	tregsSelected_[item] = !tregsSelected_[item] ;
	if (tregsSelected_[item])
		tregWindowPointer->SetItemImage(item, checkedButton_, -1);
	else
		tregWindowPointer->SetItemImage(item, uncheckButton_, -1);
}

void DebugWindow::selectTreg(wxListEvent&event)
{
	selectedTreg_ = (int)event.GetIndex();
}

void DebugWindow::deselectTreg(wxListEvent&WXUNUSED(event))
{
	selectedTreg_ = -1;
}

void DebugWindow::keyTreg(wxListEvent&event)
{
	const int keyCode = event.GetKeyCode();
	if (keyCode == WXK_DELETE && selectedTreg_ != -1)
  		XRCCTRL(*this,"TregWindow", wxListCtrl)->DeleteItem(selectedTreg_);
}

void DebugWindow::switchTrap(int item)
{
	trapsSelected_[item] = !trapsSelected_[item] ;
	if (trapsSelected_[item])
		trapWindowPointer->SetItemImage(item, checkedButton_, -1);
	else
		trapWindowPointer->SetItemImage(item, uncheckButton_, -1);
}

void DebugWindow::selectTrap(wxListEvent&event)
{
	selectedTrap_ = (int)event.GetIndex();
}

void DebugWindow::deselectTrap(wxListEvent&WXUNUSED(event))
{
	selectedTrap_ = -1;
}

void DebugWindow::keyTrap(wxListEvent&event)
{
	const int keyCode = event.GetKeyCode();
	if (keyCode == WXK_DELETE && selectedTrap_ != -1)
  		XRCCTRL(*this,"TrapWindow", wxListCtrl)->DeleteItem(selectedTrap_);
}

void DebugWindow::addBreakPoint()
{
	wxString printBuffer;

	printBuffer.Printf("%04X", breakPoints_[numberOfBreakPoints_]);
	breakPointWindowPointer->InsertItem(numberOfBreakPoints_, printBuffer);
	numberOfBreakPoints_++;
}

void DebugWindow::addTrap()
{
	wxString printBuffer, printBuffer2;

	int i = numberOfTraps_;
	printBuffer = "";
	printBuffer2 = "";

	int n = traps_[i][1] & 0xf, n1805;
	int inst = traps_[i][1] >> 4, i1805;
	switch(inst)
	{
		case 0x0:
			switch(n)
			{
				case 0x0:
					printBuffer.operator += ("IDL");
				break;

				default:
					if (traps_[i][7] == 0xf0)
						printBuffer.Printf("LDN  Rx");
					else
						printBuffer.Printf("LDN  R%X",n);
				break;
			}
		break;

		case 0x1:
			if (traps_[i][7] == 0xf0)
				printBuffer.Printf("INC  Rx");
			else
				printBuffer.Printf("INC  R%X",n);
		break;

		case 0x2:
			if (traps_[i][7] == 0xf0)
				printBuffer.Printf("DEC  Rx");
			else
				printBuffer.Printf("DEC  R%X",n);
		break;

		case 0x3:
			printBuffer2.Printf("%02X", traps_[i][2]);
			switch(n)
			{
				case 0x0:
					printBuffer.Printf("BR   ");
				break;
				case 0x1:
					printBuffer.Printf("BQ   ");
				break;
				case 0x2:
					printBuffer.Printf("BZ   ");
				break;
				case 0x3:
					if (traps_[i][8] == MEM_TYPE_OPCODE_BGE)
						printBuffer.Printf("BGE  ");
					else
					{
						if (traps_[i][8] == MEM_TYPE_OPCODE_BPZ)
							printBuffer.Printf("BPZ  ");
						else
							printBuffer.Printf("BDF  ");
					}
				break;
				case 0x4:
					printBuffer.Printf("B1   ");
				break;
				case 0x5:
					printBuffer.Printf("B2   ");
				break;
				case 0x6:
					printBuffer.Printf("B3   ");
				break;
				case 0x7:
					printBuffer.Printf("B4   ");
				break;
				case 0x8:
					if (traps_[i][8] == MEM_TYPE_OPCODE_SKP)
						printBuffer.operator += ("SKP");
					else
						printBuffer.operator += ("NBR"); 
				break;
				case 0x9:
					printBuffer.Printf("BNQ  ");
				break;
				case 0xa:
					printBuffer.Printf("BNZ  ");
				break;
				case 0xb:
					if (traps_[i][8] == MEM_TYPE_OPCODE_BL)
						printBuffer.Printf("BL   ");
					else
					{
						if (traps_[i][8] == MEM_TYPE_OPCODE_BM)
							printBuffer.Printf("BM   ");
						else
							printBuffer.Printf("BNF  ");
					}
				break;
				case 0xc:
					printBuffer.Printf("BN1  ");
				break;
				case 0xd:
					printBuffer.Printf("BN2  ");
				break;
				case 0xe:
					printBuffer.Printf("BN3  ");
				break;
				case 0xf:
					printBuffer.Printf("BN4  ");
				break;
			}
			if (traps_[i][0] == 2)
				printBuffer.operator += (printBuffer2);
			else
			{
				if (printBuffer.Len() == 5)
					printBuffer.operator += ("x");
			}
		break;

		case 0x4:
			if (traps_[i][7] == 0xf0)
				printBuffer.Printf("LDA  Rx");
			else
				printBuffer.Printf("LDA  R%X",n);
		break;

		case 0x5:
			if (traps_[i][7] == 0xf0)
				printBuffer.Printf("STR  Rx");
			else
				printBuffer.Printf("STR  R%X",n);
		break;

		case 0x6:
			switch(n)
			{
				case 0x0:
					printBuffer.operator += ("IRX");
				break;
				case 0x1:
				case 0x2:
				case 0x3:
				case 0x4:
				case 0x5:
				case 0x6:
				case 0x7:
					if (traps_[i][7] == 0xf8)
						printBuffer.Printf("OUT  x");
					else
						printBuffer.Printf("OUT  %X",n);
				break;
				case 0x8:
                    if (cpuType_ == CPU1801)
                        printBuffer.Printf("INP  %X",n-8);
                    else
                    {
					n1805 = traps_[i][2] & 0xf;
					i1805 = traps_[i][2] >> 4;
					switch(i1805)
					{
						case 0:
							switch(n1805)
							{
								case 0:
									printBuffer.Printf("STPC");
								break;
								case 1: 
									printBuffer.Printf("DTC");
								break;
								case 2: 
									printBuffer.Printf("SPM2");
								break;
								case 3: 
									printBuffer.Printf("SCM2");
								break;
								case 4: 
									printBuffer.Printf("SPM1");
								break;
								case 5: 
									printBuffer.Printf("SCM1");
								break;
								case 6: 
									printBuffer.Printf("LDC");
								break;
								case 7:
									printBuffer.Printf("STM");
								break;
								case 8: 
									printBuffer.Printf("GEC");
								break;
								case 9: 
									printBuffer.Printf("ETQ");
								break;
								case 0xa: 
									printBuffer.Printf("XIE");
								break;
								case 0xb: 
									printBuffer.Printf("XID");
								break;
								case 0xc: 
									printBuffer.Printf("CIE");
								break;
								case 0xd: 
									printBuffer.Printf("CID");
								break;
							}
						break;
						case 2:
							if (traps_[i][0] == 2)
							{
								if (traps_[i][7] == 0xf0)
									printBuffer.Printf("DBNZ Rx,x");
								else
									printBuffer.Printf("DBNZ R%X,x", n1805);
							}
							else
							{
								if (traps_[i][7] == 0xf0)
									printBuffer.Printf("DBNZ Rx,%04X", traps_[i][3]<<8|traps_[i][4]);
								else
									printBuffer.Printf("DBNZ R%X,%04X", n1805, traps_[i][3]<<8|traps_[i][4]);
							}
						break;
						case 3:
							printBuffer2.Printf("%02X", traps_[i][3]);
							switch(n1805)
							{
								case 0xe:
									printBuffer.Printf("BCI  ");
								break;
								case 0xf:
									printBuffer.Printf("BXI  ");
								break;
							}
							if (traps_[i][0] == 3)
								printBuffer.operator += (printBuffer2);
							else
							{
								printBuffer.operator += ("x");
							}
						break;
						case 6:
							if (traps_[i][7] == 0xf0)
								printBuffer.Printf("RLXA Rx");
							else
								printBuffer.Printf("RLXA R%X", n1805);
						break;
						case 7:
							printBuffer2.Printf("%02X", traps_[i][3]);
							switch(n1805)
							{
								case 0x4:
									printBuffer.Printf("DADC");
								break;
								case 0x6:
									printBuffer.Printf("DSAV");
								break;
								case 0x7:
									printBuffer.Printf("DSMB");
								break;
								case 0xc:
									printBuffer.Printf("DACI ");
								break;
								case 0xf:
									printBuffer.Printf("DSBI ");
								break;
							}
							if (traps_[i][0] == 3)
								printBuffer.operator += (printBuffer2);
							else
							{
								if (printBuffer.Len() == 5)
									printBuffer.operator += ("x");
							}
						break;
						case 8:
							if (traps_[i][0] == 2)
							{
								if (traps_[i][7] == 0xf0)
									printBuffer.Printf("SCAL Rx,x");
								else
									printBuffer.Printf("SCAL R%X,x", n1805);
							}
							else
							{
								if (traps_[i][7] == 0xf0)
									printBuffer.Printf("SCAL Rx,%04X", traps_[i][3]<<8|traps_[i][4]);
								else
									printBuffer.Printf("SCAL R%X,%04X", n1805, traps_[i][3]<<8|traps_[i][4]);
							}
						break;
						case 9:
							if (traps_[i][7] == 0xf0)
								printBuffer.Printf("SRET Rx");
							else
								printBuffer.Printf("SRET R%X", n1805);
						break;
						case 0xa: 
							if (traps_[i][7] == 0xf0)
								printBuffer.Printf("RSXD Rx");
							else
								printBuffer.Printf("RSXD R%X", n1805);
						break;
						case 0xb:
							if (traps_[i][7] == 0xf0)
								printBuffer.Printf("RNX  Rx");
							else
								printBuffer.Printf("RNX  R%X", n1805);
						break;
						case 0xc:
							if (traps_[i][0] == 2)
							{
								if (traps_[i][7] == 0xf0)
									printBuffer.Printf("RLDI Rx,x");
								else
									printBuffer.Printf("RLDI R%X,x", n1805);
							}
							else
							{
								if (traps_[i][7] == 0xf0)
									printBuffer.Printf("RLDI Rx,%04X", traps_[i][3]<<8|traps_[i][4]);
								else
									printBuffer.Printf("RLDI R%X,%04X", n1805, traps_[i][3]<<8|traps_[i][4]);
							}
						break;
						case 0xf:
							printBuffer2.Printf("%02X", traps_[i][3]);
							switch(n1805)
							{
								case 0x4:
									printBuffer.Printf("DADD");
								break;
								case 0x7:
									printBuffer.Printf("DSM");
								break;
								case 0xc:
									printBuffer.Printf("DADI ");
								break;
								case 0xf:
									printBuffer.Printf("DSMI ");
								break;
							}
							if (traps_[i][0] == 3)
								printBuffer.operator += (printBuffer2);
							else
							{
								if (printBuffer.Len() == 5)
									printBuffer.operator += ("x");
							}
						break;

					}
                    }
				break;
				case 0x9:
				case 0xa:
				case 0xb:
				case 0xc:
				case 0xd:
				case 0xe:
				case 0xf:
					if (traps_[i][7] == 0xf8)
						printBuffer.Printf("INP  x");
					else
						printBuffer.Printf("INP  %X",n-8);
				break;
			}
		break;
		case 0x7:
			printBuffer2.Printf("%02X", traps_[i][2]);
			switch(n)
			{
				case 0x0:
					printBuffer.operator += ("RET");
				break;
				case 0x1:
					printBuffer.operator += ("DIS");
				break;
				case 0x2:
					printBuffer.operator += ("LDXA");
				break;
				case 0x3:
					printBuffer.operator += ("STXD");
				break;
				case 0x4:
					printBuffer.operator += ("ADC");
				break;
				case 0x5:
					printBuffer.operator += ("SDB");
				break;
				case 0x6:
					if (traps_[i][8] == MEM_TYPE_OPCODE_RSHR)
						printBuffer.operator += ("RSHR");
					else
						printBuffer.operator += ("SHRC");
				break;
				case 0x7:
					printBuffer.operator += ("SMB");
				break;
				case 0x8:
					printBuffer.operator += ("SAV");
				break;
				case 0x9:
					printBuffer.operator += ("MARK");
				break;
				case 0xa:
					printBuffer.operator += ("REQ");
				break;
				case 0xb:
					printBuffer.operator += ("SEQ");
				break;
				case 0xe:
					if (traps_[i][8] == MEM_TYPE_OPCODE_RSHL)
						printBuffer.operator += ("RSHL");
					else
						printBuffer.operator += ("SHLC");
				break;
				case 0xc:
					printBuffer.Printf("ADCI ");
				break;
				case 0xd:
					printBuffer.Printf("SDBI ");
				break;
				case 0xf:
					printBuffer.Printf("SMBI ");
				break;
			}
			if (traps_[i][0] == 2)
				printBuffer.operator += (printBuffer2);
			else
			{
				if (printBuffer.Len() == 5)
					printBuffer.operator += ("x");
			}
		break;
		case 0x8:
			if (traps_[i][7] == 0xf0)
				printBuffer.Printf("GLO  Rx");
			else
				printBuffer.Printf("GLO  R%X",n);
		break;
		case 0x9:
			if (traps_[i][7] == 0xf0)
				printBuffer.Printf("GHI  Rx");
			else
				printBuffer.Printf("GHI  R%X",n);
		break;
		case 0xa:
			if (traps_[i][7] == 0xf0)
				printBuffer.Printf("PLO  Rx");
			else
				printBuffer.Printf("PLO  R%X",n);
		break;
		case 0xb:
			if (traps_[i][7] == 0xf0)
				printBuffer.Printf("PHI  Rx");
			else
				printBuffer.Printf("PHI  R%X",n);
		break;
		case 0xc:
			printBuffer2.Printf("%04X", traps_[i][2]<<8|traps_[i][3]);
			switch(n)
			{
				case 0x0:
					printBuffer.Printf("LBR  "); 
				break;
				case 0x1:
					printBuffer.Printf("LBQ  "); 
				break;
				case 0x2:
					printBuffer.Printf("LBZ  "); 
				break;
				case 0x3:
					printBuffer.Printf("LBDF ");
				break;
				case 0x4:
					printBuffer.operator += ("NOP");
				break;
				case 0x5:
					printBuffer.operator += ("LSNQ");
				break;
				case 0x6:
					printBuffer.operator += ("LSNZ");
				break;
				case 0x7:
					printBuffer.operator += ("LSNF");
				break;
				case 0x8:
					if (traps_[i][8] == MEM_TYPE_OPCODE_LSKP)
						printBuffer.operator += ("LSKP");
					else
						printBuffer.operator += ("NLBR");
				break;
				case 0x9:
					printBuffer.Printf("LBNQ ");
				break;
				case 0xa:
					printBuffer.Printf("LBNZ "); 
				break;
				case 0xb:
					printBuffer.Printf("LBNF "); 
				break;
				case 0xc:
					printBuffer.operator += ("LSIE");
				break;
				case 0xd:
					printBuffer.operator += ("LSQ");
				break;
				case 0xe:
					printBuffer.operator += ("LSZ");
				break;
				case 0xf:
					printBuffer.operator += ("LSDF");
				break;
			}
			if (traps_[i][0] == 3)
				printBuffer.operator += (printBuffer2);
			else
			{
				if (printBuffer.Len() == 5)
					printBuffer.operator += ("x");
			}
		break;
		case 0xd:
			if (traps_[i][7] == 0xf0)
				printBuffer.Printf("SEP  Rx");
			else
				printBuffer.Printf("SEP  R%X",n);
		break;
		case 0xe:
			if (traps_[i][7] == 0xf0)
				printBuffer.Printf("SEX  Rx");
			else
				printBuffer.Printf("SEX  R%X",n);
		break;
		case 0xf:
			printBuffer2.Printf("%02X", traps_[i][2]);
			switch(n)
			{
				case 0x0:
					printBuffer.operator += ("LDX");
				break;
				case 0x1:
					printBuffer.operator += ("OR");
				break;
				case 0x2:
					printBuffer.operator += ("AND");
				break;
				case 0x3:
					printBuffer.operator += ("XOR");
				break;
				case 0x4:
					printBuffer.operator += ("ADD");
				break;
				case 0x5:
					printBuffer.operator += ("SD");
				break;
				case 0x6:
					printBuffer.operator += ("SHR");
				break;
				case 0x7:
					printBuffer.operator += ("SM");
				break;
				case 0x8:
					printBuffer.Printf("LDI  ");
				break;
				case 0x9:
					printBuffer.Printf("ORI  ");
				break;
				case 0xa:
					printBuffer.Printf("ANI  ");
				break;
				case 0xb:
					printBuffer.Printf("XRI  ");
				break;
				case 0xc:
					printBuffer.Printf("ADI  ");
				break;
				case 0xd:
					printBuffer.Printf("SDI  ");
				break;
				case 0xe:
					printBuffer.operator += ("SHL");
				break;
				case 0xf:
					printBuffer.Printf("SMI  ");
				break;
			}
			if (traps_[i][0] == 2)
				printBuffer.operator += (printBuffer2);
			else
			{
				if (printBuffer.Len() == 5)
					printBuffer.operator += ("x");
			}
		break;
	}
	trapWindowPointer->InsertItem(i, printBuffer);
	numberOfTraps_++;
}

void DebugWindow::addTreg()
{
	wxString printBuffer;

	switch(tregs_[numberOfTregs_][0])
	{
		case TREG_R0: case TREG_R1: case TREG_R2: case TREG_R3: case TREG_R4: case TREG_R5: case TREG_R6: case TREG_R7: case TREG_R8: case TREG_R9:
		case TREG_RA: case TREG_RB: case TREG_RC: case TREG_RD: case TREG_RE: case TREG_RF:
			printBuffer.Printf("R%X %04X", tregs_[numberOfTregs_][0] - TREG_R0, tregs_[numberOfTregs_][1]);
		break;

		case TREG_D: printBuffer.Printf("D  %02X", tregs_[numberOfTregs_][1]);
		break;

		case TREG_DF: printBuffer.Printf("DF %X", (tregs_[numberOfTregs_][1])?1:0);
		break;

		case TREG_Q: printBuffer.Printf("Q  %X", (tregs_[numberOfTregs_][1])?1:0);
		break;

		case TREG_P: printBuffer.Printf("P  %X", tregs_[numberOfTregs_][1]);
		break;

		case TREG_X: printBuffer.Printf("X  %X", tregs_[numberOfTregs_][1]);
		break;

		case TREG_T: printBuffer.Printf("T  %02X", tregs_[numberOfTregs_][1]);
		break;
	}
	tregWindowPointer->InsertItem(numberOfTregs_, printBuffer);
	numberOfTregs_++;
}

wxString DebugWindow::extractWord(wxString *buffer)
{
	int end;
	wxString ret;

	buffer->Trim(false);

	end = buffer->Find(' ');
	if (end == -1)
	{
		ret = *buffer;
		*buffer = "";
		return ret;
	}

	ret = buffer->Mid(0, end);
	*buffer = buffer->Mid(end, buffer->Len()-end);
	return ret;
}

wxString DebugWindow::extractNextWord(wxString *buffer, wxString *seperator)
{
	size_t end;
	wxString ret;

	buffer->Trim(false);
	buffer->Trim(true);

	end = 0;
	while (buffer->Mid(end, 1) != " " && buffer->Mid(end, 1) != "/" && buffer->Mid(end, 1) != "," && buffer->Mid(end, 1) != "+" && buffer->Mid(end, 1) != "-" && buffer->Mid(end, 1) != "=" && end != buffer->Len())
		end++;

	if (end == buffer->Len())
	{
		ret = *buffer;
		*buffer = "";
		*seperator = " ";
		return ret;
	}

	ret = buffer->Mid(0, end);
	*seperator = buffer->Mid(end, 1);
	*buffer = buffer->Mid(end + 1, buffer->Len()- end);
	return ret;
}

wxString DebugWindow::cdp1802disassemble(Word* address, bool showDetails, bool showOpcode, bool textAssembler, Word start, Word end)
{
	wxString printBufferOpcode, printBufferAssembler, printBufferTemp, printBufferAddress, printBufferDetails;
	int i, n, i1805, n1805;
	Word instructionAddress = *address;
	Byte memType;

	i = p_Computer->readMem(*address);

	printBufferAddress.Printf("%04X: ", *address);
	printBufferOpcode.Printf("%02X ", i);
	printBufferAssembler = "";
	printBufferDetails = "";
	n = i & 0xf;
	i >>= 4;
	*address = *address + 1;

	Byte accumulator = p_Computer->getAccumulator();

	switch(i)
	{
		case 0x0:
			switch(n)
			{
				case 0x0:
					printBufferAssembler.operator += ("IDL");
				break;

				default:
					printBufferAssembler.Printf("LDN  R%X",n);
					printBufferDetails.Printf("D=%02X", accumulator);
				break;
			}
		break;

		case 0x1:
			printBufferAssembler.Printf("INC  R%X",n);
			printBufferDetails.Printf("R%X=%04X", n, p_Computer->getScratchpadRegister(n));
		break;

		case 0x2:
			printBufferAssembler.Printf("DEC  R%X",n);
			printBufferDetails.Printf("R%X=%04X", n, p_Computer->getScratchpadRegister(n));
		break;

		case 0x3:
            switch(n)
			{
				case 0x0:
                    printBufferAssembler = "BR   " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X ", p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 1;
				break;
				case 0x1:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler = "BQ   " + getShortAddressOrLabel(*address, textAssembler, start, end);
                        printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
                        printBufferOpcode.operator += (printBufferTemp);
                    }
					*address = *address + 1;
				break;
				case 0x2:
                    printBufferAssembler = "BZ   " + getShortAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 1;
				break;
				case 0x3:
					if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_BPZ)
                        printBufferAssembler = "BPZ  " + getShortAddressOrLabel(*address, textAssembler, start, end);
					else
					{
						if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_BGE)
                            printBufferAssembler = "BGE  " + getShortAddressOrLabel(*address, textAssembler, start, end);
						else
                            printBufferAssembler = "BDF  " + getShortAddressOrLabel(*address, textAssembler, start, end);
					}
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 1;
				break;
				case 0x4:
                    printBufferAssembler = "B1   " + getShortAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 1;
				break;
				case 0x5:
                    printBufferAssembler = "B2   " + getShortAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 1;
				break;
				case 0x6:
                    printBufferAssembler = "B3   " + getShortAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 1;
				break;
				case 0x7:
                    printBufferAssembler = "B4   " + getShortAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 1;
				break;
				case 0x8:
					if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_SKP || textAssembler)
						printBufferAssembler.operator += ("SKP");
					else
						printBufferAssembler.operator += ("NBR"); 
				break;
				case 0x9:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler = "BNQ  " + getShortAddressOrLabel(*address, textAssembler, start, end);
                        printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
                        printBufferOpcode.operator += (printBufferTemp);
                    }
					*address = *address + 1;
				break;
				case 0xa:
                    printBufferAssembler = "BNZ  " + getShortAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 1;
				break;
				case 0xb:
					if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_BM)
                        printBufferAssembler = "BM   " + getShortAddressOrLabel(*address, textAssembler, start, end);
					else
					{
						if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_BL)
                            printBufferAssembler = "BL   " + getShortAddressOrLabel(*address, textAssembler, start, end);
						else
                            printBufferAssembler = "BNF  " + getShortAddressOrLabel(*address, textAssembler, start, end);
					}
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 1;
				break;
				case 0xc:
                    printBufferAssembler = "BN1  " + getShortAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 1;
				break;
				case 0xd:
                    printBufferAssembler = "BN2  " + getShortAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 1;
				break;
				case 0xe:
                    printBufferAssembler = "BN3  " + getShortAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 1;
				break;
				case 0xf:
                    printBufferAssembler = "BN4  " + getShortAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 1;
				break;
			}
		break;

		case 0x4:
			printBufferAssembler.Printf("LDA  R%X",n);
			printBufferDetails.Printf("D=M(%04X)=%02X", p_Computer->getScratchpadRegister(n)-1, accumulator);
		break;

		case 0x5:
			printBufferAssembler.Printf("STR  R%X",n);
			printBufferDetails.Printf("M(%04X)=%02X", p_Computer->getScratchpadRegister(n), accumulator);
		break;

		case 0x6:
			switch(n)
			{
				case 0x0:
                    if (cpuType_ != CPU1801)
                    {
                        printBufferAssembler.operator += ("IRX");
                        printBufferDetails.Printf("R%X=%04X", p_Computer->getDataPointer(), p_Computer->getScratchpadRegister(p_Computer->getDataPointer()));
                    }
                    else
                    {
                        printBufferAssembler.Printf("OUT  %X",n);
                        printBufferDetails.Printf("[%02X]", p_Computer->readMem(p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1));
                    }
				break;
				case 0x1:
				case 0x2:
				case 0x3:
				case 0x4:
				case 0x5:
				case 0x6:
				case 0x7:
					printBufferAssembler.Printf("OUT  %X",n);
					switch (runningComputer_)
					{
						case COMX:
						case CIDELSA:
						case PECOM:
							if (n>3)
								printBufferDetails.Printf("[%04X]", p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1);
							else
								printBufferDetails.Printf("[%02X]", p_Computer->readMem(p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1));
						break;

						case TMC600:
							if (n==5 && (p_Computer->getOutValue(7) != 0x20) && (p_Computer->getOutValue(7) != 0x30))
								printBufferDetails.Printf("[%04X]", p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1);
							else
								printBufferDetails.Printf("[%02X]", p_Computer->readMem(p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1));
						break;

						default:
							printBufferDetails.Printf("[%02X]", p_Computer->readMem(p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1));
						break;
					}
				break;
				case 0x8:
					if (cpuType_ == CPU1804 || cpuType_ == CPU1805)
					{
						i1805 = p_Computer->readMem(*address);

						printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
						printBufferOpcode.operator += (printBufferTemp);
						*address = *address + 1;

						n1805 = i1805 & 15;
						i1805 = i1805>>4;
						switch(i1805)
						{
							case 0:
								switch(n1805)
								{
									case 0: // 1804
										printBufferAssembler.Printf("STPC");
									break;
									case 1: // 1804
										printBufferAssembler.Printf("DTC");
										printBufferDetails.Printf("CNTR=%02X", p_Computer->get1805Counter());
									break;
									case 2: // 1804
										printBufferAssembler.Printf("SPM2");
									break;
									case 3: // 1804
										printBufferAssembler.Printf("SCM2");
									break;
									case 4: // 1804
										printBufferAssembler.Printf("SPM1");
									break;
									case 5: // 1804
										printBufferAssembler.Printf("SCM1");
									break;
									case 6: // 1804
										printBufferAssembler.Printf("LDC");
										if (!p_Computer->is1805CtrRunning())
											printBufferDetails.Printf("CH/CNTR=%02X", p_Computer->get1805Ch());
										else
											printBufferDetails.Printf("CNTR=%02X", p_Computer->get1805Ch());
									break;
									case 7: // 1804
										printBufferAssembler.Printf("STM");
									break;
									case 8: // 1804
										printBufferAssembler.Printf("GEC");
										printBufferDetails.Printf("D=%02X", accumulator);
									break;
									case 9: // 1804
										printBufferAssembler.Printf("ETQ");
									break;
									case 0xa: // 1804
										printBufferAssembler.Printf("XIE");
									break;
									case 0xb: // 1804
										printBufferAssembler.Printf("XID");
									break;
									case 0xc: // 1804
										printBufferAssembler.Printf("CIE");
									break;
									case 0xd: // 1804
										printBufferAssembler.Printf("CID");
									break;
									default:
										printBufferAssembler.Printf("Illegal code");
									break;
								}
							break;

							case 2:
								if (cpuType_ == CPU1805)
								{
                                    printBufferAssembler.Printf("DBNZ R%X,",n1805);
                                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
									printBufferTemp.Printf("%02X %02X ", p_Computer->readMem(*address),p_Computer->readMem((*address)+1));
									printBufferOpcode.operator += (printBufferTemp);
									printBufferDetails.Printf(" R%X=%04X", n1805, p_Computer->getScratchpadRegister(n1805));
									*address = *address + 2;
								}
							break;

							case 3:
								switch(n1805)
								{
									case 0xe: // 1804
                                        printBufferAssembler = "BCI  " + getShortAddressOrLabel(*address, textAssembler, start, end);
										printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
										printBufferOpcode.operator += (printBufferTemp);
										*address = *address + 1;
									break;
									case 0xf: // 1804
                                        printBufferAssembler = "BXI  " + getShortAddressOrLabel(*address, textAssembler, start, end);
										printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
										printBufferOpcode.operator += (printBufferTemp);
										*address = *address + 1;
									break;
									default:
										printBufferAssembler.Printf("Illegal code");
									break;
								}
 							break;

							case 6: // 1804
								printBufferAssembler.Printf("RLXA R%X", n1805);
 								printBufferDetails.Printf("R%X=%04X", n1805, p_Computer->getScratchpadRegister(n1805));
 							break;

							case 7:
								if (cpuType_ == CPU1805)
								{
									switch(n1805)
									{
										case 4:
											printBufferAssembler.Printf("DADC");
											printBufferDetails.Printf("D=%02X", accumulator);
										break;
										case 6:
											printBufferAssembler.Printf("DSAV");
											printBufferDetails.Printf("D=%02X", accumulator);
										break;
										case 7:
											printBufferAssembler.Printf("DSMB");
											printBufferDetails.Printf("D=%02X", accumulator);
										break;
										case 0xc:
                                            printBufferAssembler = "DACI " + getHexByte(*address, textAssembler);
                                            printBufferDetails.Printf("D=%02X", accumulator);
											printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
											printBufferOpcode.operator += (printBufferTemp);
											*address = *address + 1;
										break;
										case 0xf:
                                            printBufferAssembler = "DSBI " + getHexByte(*address, textAssembler);
											printBufferDetails.Printf("D=%02X", accumulator);
											printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
											printBufferOpcode.operator += (printBufferTemp);
											*address = *address + 1;
										break;
										default:
											printBufferAssembler.Printf("Illegal code");
										break;
									}
								}
 							break;

							case 8: // 1804
                                printBufferAssembler.Printf("SCAL R%X,",n1805);
                                printBufferAssembler = printBufferAssembler + getSubAddressOrLabel(*address, textAssembler, start, end);
                                printBufferTemp.Printf("%02X %02X ", p_Computer->readMem(*address),p_Computer->readMem((*address)+1));
								printBufferOpcode.operator += (printBufferTemp);
								*address = *address + 2;
  							break;

							case 9: // 1804
								printBufferAssembler.Printf("SRET R%X", n1805);
								printBufferDetails.Printf("R%X(P)=%04X", p_Computer->getProgramCounter(), p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()));
  							break;

							case 0xa: // 1804
								printBufferAssembler.Printf("RSXD R%X", n1805);
  							break;

							case 0xb: // 1804
								printBufferAssembler.Printf("RNX  R%X", n1805);
 								printBufferDetails.Printf("R%X=%04X", n1805, p_Computer->getScratchpadRegister(n1805));
 							break;

							case 0xc:  // 1804
                                if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_RLDL)
                                {
                                    printBufferAssembler.Printf("RLDL R%X,",n1805);
                                    printBufferAssembler = printBufferAssembler + getSubAddressOrLabel(*address, textAssembler, start, end);
                                }
                                else
                                    printBufferAssembler.Printf("RLDI R%X,%04X",n1805, p_Computer->readMem(*address)<<8|p_Computer->readMem((*address)+1));
								printBufferTemp.Printf("%02X %02X ", p_Computer->readMem(*address),p_Computer->readMem((*address)+1));
								printBufferOpcode.operator += (printBufferTemp);
								*address = *address + 2;
  							break;

							case 0xF:
								if (cpuType_ == CPU1805)
								{
									switch(n1805)
									{
										case 4:
											printBufferAssembler.Printf("DADD");
											printBufferDetails.Printf("D=%02X", accumulator);
										break;
										case 7:
											printBufferAssembler.Printf("DSM");
											printBufferDetails.Printf("D=%02X", accumulator);
										break;
										case 0xc:
                                            printBufferAssembler = "DADI " + getHexByte(*address, textAssembler);
											printBufferDetails.Printf("D=%02X", accumulator);
											printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
											printBufferOpcode.operator += (printBufferTemp);
											*address = *address + 1;
										break;
										case 0xf:
                                            printBufferAssembler = "DSMI " + getHexByte(*address, textAssembler);
											printBufferDetails.Printf("D=%02X", accumulator);
											printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
											printBufferOpcode.operator += (printBufferTemp);
											*address = *address + 1;
										break;
										default:
											printBufferAssembler.Printf("Illegal code");
										break;
									}
								}
							break;
							default:
								printBufferAssembler.Printf("Illegal code");
							break;
						}
					}
                    else
                    {
                        printBufferAssembler.Printf("INP  %X",n-8);
                        printBufferDetails.Printf("D=M(%04X)=%02X", p_Computer->getScratchpadRegister(p_Computer->getDataPointer()), accumulator);
                    }
				break;
				case 0x9:
				case 0xa:
				case 0xb:
				case 0xc:
				case 0xd:
				case 0xe:
				case 0xf:
					printBufferAssembler.Printf("INP  %X",n-8);
					printBufferDetails.Printf("D=M(%04X)=%02X", p_Computer->getScratchpadRegister(p_Computer->getDataPointer()), accumulator);
				break;
			}
		break;
		case 0x7:
            switch(n)
			{
				case 0x0:
					printBufferAssembler.operator += ("RET");
					printBufferDetails.Printf("P=R%X, X=R%X", p_Computer->getProgramCounter(), p_Computer->getDataPointer());
				break;
				case 0x1:
					printBufferAssembler.operator += ("DIS");
					printBufferDetails.Printf("P=R%X, X=R%X", p_Computer->getProgramCounter(), p_Computer->getDataPointer());
				break;
				case 0x2:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler.operator += ("LDXA");
                        printBufferDetails.Printf("D=M(%04X)=%02X", p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1, accumulator);
                    }
				break;
				case 0x3:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler.operator += ("STXD");
                        printBufferDetails.Printf("M(%04X)=%02X", p_Computer->getScratchpadRegister(p_Computer->getDataPointer())+1, accumulator);
                    }
				break;
				case 0x4:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler.operator += ("ADC");
                        printBufferDetails.Printf("D=%02X", accumulator);
                    }
				break;
				case 0x5:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler.operator += ("SDB");
                        printBufferDetails.Printf("D=%02X", accumulator);
                    }
				break;
				case 0x6:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_RSHR)
                            printBufferAssembler.operator += ("RSHR");
                        else
                            printBufferAssembler.operator += ("SHRC");
                        printBufferDetails.Printf("D=%02X", accumulator);
                    }
				break;
				case 0x7:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler.operator += ("SMB");
                        printBufferDetails.Printf("D=%02X", accumulator);
                    }
				break;
				case 0x8:
					printBufferAssembler.operator += ("SAV");
					printBufferDetails.Printf("M(%04X)=%02X", p_Computer->getScratchpadRegister(p_Computer->getDataPointer()), p_Computer->getRegisterT());
				break;
				case 0x9:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                        printBufferAssembler.operator += ("MARK");
				break;
				case 0xa:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                        printBufferAssembler.operator += ("REQ");
				break;
				case 0xb:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                        printBufferAssembler.operator += ("SEQ");
				break;
				case 0xc:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler = "ADCI " + getHexByte(*address, textAssembler);
                        printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
                        printBufferOpcode.operator += (printBufferTemp);
                        printBufferDetails.Printf("D=%02X", accumulator);
                        *address = *address + 1;
                    }
				break;
				case 0xd:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler = "SDBI " + getHexByte(*address, textAssembler);
                        printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
                        printBufferOpcode.operator += (printBufferTemp);
                        printBufferDetails.Printf("D=%02X", accumulator);
                        *address = *address + 1;
                    }
				break;
				case 0xe:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_RSHL)
                            printBufferAssembler.operator += ("RSHL");
                        else
                            printBufferAssembler.operator += ("SHLC");
                        printBufferDetails.Printf("D=%02X", accumulator);
                    }
				break;
				case 0xf:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler = "SMBI " + getHexByte(*address, textAssembler);
                        printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
                        printBufferOpcode.operator += (printBufferTemp);
                        printBufferDetails.Printf("D=%02X", accumulator);
                        *address = *address + 1;
                    }
				break;
			}
		break;
		case 0x8:
			printBufferAssembler.Printf("GLO  R%X",n);
			printBufferDetails.Printf("D=%02X", accumulator);
		break;
		case 0x9:
			printBufferAssembler.Printf("GHI  R%X",n);
			printBufferDetails.Printf("D=%02X", accumulator);
		break;
		case 0xa:
			printBufferAssembler.Printf("PLO  R%X",n);
			printBufferDetails.Printf("R%X=%04X", n, p_Computer->getScratchpadRegister(n));
		break;
		case 0xb:
			printBufferAssembler.Printf("PHI  R%X",n);
			printBufferDetails.Printf("R%X=%04X", n, p_Computer->getScratchpadRegister(n));
		break;
		case 0xc:
            if (cpuType_ == CPU1801)
                printBufferAssembler.Printf("Illegal code");
            else
            {
			switch(n)
			{
				case 0x0:
					if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_LBR_SLOT)
						printBufferAssembler.Printf("LBR  S%02X,",p_Computer->readMemDataType(*address));
					else
                            printBufferAssembler = "LBR  ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X %02X ",p_Computer->readMem(*address),p_Computer->readMem((*address)+1));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 2;
				break;
				case 0x1:
					if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_LBR_SLOT)
						printBufferAssembler.Printf("LBQ  S%02X",p_Computer->readMemDataType(*address));
					else
						printBufferAssembler = "LBQ  ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X %02X ",p_Computer->readMem(*address),p_Computer->readMem((*address)+1));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 2;
				break;
				case 0x2:
					if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_LBR_SLOT)
						printBufferAssembler.Printf("LBZ  S%02X,",p_Computer->readMemDataType(*address));
					else
						printBufferAssembler = "LBZ  ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X %02X ",p_Computer->readMem(*address),p_Computer->readMem((*address)+1));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 2;
				break;
				case 0x3:
					if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_LBR_SLOT)
						printBufferAssembler.Printf("LBDF S%02X,",p_Computer->readMemDataType(*address));
					else
						printBufferAssembler = "LBDF ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X %02X ",p_Computer->readMem(*address),p_Computer->readMem((*address)+1));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 2;
				break;
				case 0x4:
					printBufferAssembler.operator += ("NOP");
				break;
				case 0x5:
					printBufferAssembler.operator += ("LSNQ");
				break;
				case 0x6:
					printBufferAssembler.operator += ("LSNZ");
				break;
				case 0x7:
					printBufferAssembler.operator += ("LSNF");
				break;
				case 0x8:
					if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_LSKP || textAssembler)
						printBufferAssembler.operator += ("LSKP");
					else
						printBufferAssembler.operator += ("NLBR");
				break;
				case 0x9:
					if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_LBR_SLOT)
						printBufferAssembler.Printf("LBNQ S%02X,",p_Computer->readMemDataType(*address));
					else
						printBufferAssembler = "LBNQ ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X %02X ",p_Computer->readMem(*address),p_Computer->readMem((*address)+1));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 2;
				break;
				case 0xa:
					if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_LBR_SLOT)
						printBufferAssembler.Printf("LBNZ S%02X,",p_Computer->readMemDataType(*address));
					else
						printBufferAssembler = "LBNZ ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X %02X ",p_Computer->readMem(*address),p_Computer->readMem((*address)+1));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 2;
				break;
				case 0xb:
					if (p_Computer->readMemDataType(instructionAddress) == MEM_TYPE_OPCODE_LBR_SLOT)
						printBufferAssembler.Printf("LBNF S%02X,",p_Computer->readMemDataType(*address));
					else
						printBufferAssembler = "LBNF ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
					printBufferTemp.Printf("%02X %02X ",p_Computer->readMem(*address),p_Computer->readMem((*address)+1));
					printBufferOpcode.operator += (printBufferTemp);
					*address = *address + 2;
				break;
				case 0xc:
					printBufferAssembler.operator += ("LSIE");
				break;
				case 0xd:
					printBufferAssembler.operator += ("LSQ");
				break;
				case 0xe:
					printBufferAssembler.operator += ("LSZ");
				break;
				case 0xf:
					printBufferAssembler.operator += ("LSDF");
				break;
			}
            }
		break;
		case 0xd:
			printBufferAssembler.Printf("SEP  R%X",n);
		break;
		case 0xe:
			printBufferAssembler.Printf("SEX  R%X",n);
		break;
		case 0xf:
			switch(n)
			{
				case 0x0:
					printBufferAssembler.operator += ("LDX");
					printBufferDetails.Printf("D=M(%04X)=%02X", p_Computer->getScratchpadRegister(p_Computer->getDataPointer()), accumulator);
				break;
				case 0x1:
					printBufferAssembler.operator += ("OR");
					printBufferDetails.Printf("D=%02X", accumulator);
				break;
				case 0x2:
					printBufferAssembler.operator += ("AND");
					printBufferDetails.Printf("D=%02X", accumulator);
				break;
				case 0x3:
					printBufferAssembler.operator += ("XOR");
					printBufferDetails.Printf("D=%02X", accumulator);
				break;
				case 0x4:
					printBufferAssembler.operator += ("ADD");
					printBufferDetails.Printf("D=%02X", accumulator);
				break;
				case 0x5:
					printBufferAssembler.operator += ("SD");
					printBufferDetails.Printf("D=%02X", accumulator);
				break;
				case 0x6:
					printBufferAssembler.operator += ("SHR");
					printBufferDetails.Printf("D=%02X", accumulator);
				break;
				case 0x7:
					printBufferAssembler.operator += ("SM");
					printBufferDetails.Printf("D=%02X", accumulator);
				break;
				case 0x8:
					memType = p_Computer->readMemDataType(instructionAddress);
					if (memType == MEM_TYPE_OPCODE_LDV || memType == MEM_TYPE_OPCODE_LDL || memType == MEM_TYPE_OPCODE_LDL_SLOT)
					{
						if (memType == MEM_TYPE_OPCODE_LDV)
                        {
                            if (textAssembler)
                            {
                                if (p_Computer->readMem(*address) >= 0xA0)
                                    printBufferAssembler.Printf("LDV  R%X,%03X%02XH",p_Computer->readMem(*address+1)&0xf, p_Computer->readMem(*address), p_Computer->readMem(*address+3));
                                else
                                    printBufferAssembler.Printf("LDV  R%X,%02X%02XH",p_Computer->readMem(*address+1)&0xf, p_Computer->readMem(*address), p_Computer->readMem(*address+3));
                            }
                            else
                                printBufferAssembler.Printf("LDV  R%X,%02X%02X",p_Computer->readMem(*address+1)&0xf, p_Computer->readMem(*address), p_Computer->readMem(*address+3));
                        }
						else
						{
							if (memType == MEM_TYPE_OPCODE_LDL_SLOT)
								printBufferAssembler.Printf("LDL  S%02X,R%X,", p_Computer->readMemDataType(*address),p_Computer->readMem(*address+1)&0xf);
                        	else
                            {
                                if (textAssembler)
                                {
                                    if (p_Computer->readMem(*address) >= 0xA0)
                                        printBufferAssembler.Printf("LDL  R%X,%03X%02XH",p_Computer->readMem(*address+1)&0xf, p_Computer->readMem(*address), p_Computer->readMem(*address+3));
                                    else
                                        printBufferAssembler.Printf("LDL  R%X,%02X%02XH",p_Computer->readMem(*address+1)&0xf, p_Computer->readMem(*address), p_Computer->readMem(*address+3));
                                }
                                else
                                    printBufferAssembler.Printf("LDL  R%X,%02X%02X",p_Computer->readMem(*address+1)&0xf, p_Computer->readMem(*address), p_Computer->readMem(*address+3));
                            }
						}
						printBufferTemp.Printf("%02X %02X",p_Computer->readMem(*address), p_Computer->readMem(*address+1));
						printBufferOpcode.operator += (printBufferTemp);
						*address = *address + 5;
					}
					else
					{
                        printBufferAssembler = "LDI  " + getHexByte(*address, textAssembler);
						printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
						printBufferOpcode.operator += (printBufferTemp);
						*address = *address + 1;
					}
				break;
				case 0x9:
                    printBufferAssembler = "ORI  " + getHexByte(*address, textAssembler);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					printBufferDetails.Printf("D=%02X", accumulator);
					*address = *address + 1;
				break;
				case 0xa:
                    printBufferAssembler = "ANI  " + getHexByte(*address, textAssembler);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					printBufferDetails.Printf("D=%02X", accumulator);
					*address = *address + 1;
				break;
				case 0xb:
                    printBufferAssembler = "XRI  " + getHexByte(*address, textAssembler);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					printBufferDetails.Printf("D=%02X", accumulator);
					*address = *address + 1;
				break;
				case 0xc:
                    printBufferAssembler = "ADI  " + getHexByte(*address, textAssembler);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					printBufferDetails.Printf("D=%02X", accumulator);
					*address = *address + 1;
				break;
				case 0xd:
                    printBufferAssembler = "SDI  " + getHexByte(*address, textAssembler);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					printBufferDetails.Printf("D=%02X", accumulator);
					*address = *address + 1;
				break;
				case 0xe:
                    if (cpuType_ == CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler.operator += ("SHL");
                        printBufferDetails.Printf("D=%02X", accumulator);
                    }
				break;
				case 0xf:
                    printBufferAssembler = "SMI  " + getHexByte(*address, textAssembler);
					printBufferTemp.Printf("%02X ",p_Computer->readMem(*address));
					printBufferOpcode.operator += (printBufferTemp);
					printBufferDetails.Printf("D=%02X", accumulator);
					*address = *address + 1;
				break;
			}
		break;
	}
	while(printBufferOpcode.Len() < 12) printBufferOpcode.operator += (" ");
	while(printBufferAssembler.Len() < 10) printBufferAssembler.operator += (" ");

    if (textAssembler)
    {
        printBufferTemp = printBufferAssembler;
        while (printBufferTemp.Len()<= 20)
            printBufferTemp += " ";
        printBufferTemp += ";" + printBufferAddress + printBufferOpcode;
        return printBufferTemp;
    }
    
	if (showOpcode)
		printBufferTemp = printBufferAddress + printBufferOpcode + printBufferAssembler;
	else
		printBufferTemp =  printBufferAddress + printBufferAssembler;

	if (showDetails)
		return printBufferTemp + printBufferDetails;
	else
		return printBufferTemp;
}

wxString DebugWindow::getShortAddressOrLabel(Word address, bool textAssembler, Word start, Word end)
{
    wxString branchAddressString;
    
    Word branchAddress = (address&0xff00) + p_Computer->readMem(address);
    
    if (branchAddress < start || branchAddress > end)
    {
        if (branchAddress >= 0xA000)
            branchAddressString.Printf("%05XH", branchAddress);
        else
            branchAddressString.Printf("%04XH", branchAddress);
        labelInfo_[address-1].outOfRange = true;
        labelInfo_[address-1].outOfRangeAddress = branchAddress;
        return branchAddressString;
    }
    
    if (textAssembler)
    {
        labelInfo_[branchAddress].branchToFound = true;
        if (p_Computer->readMemLabelType(branchAddress) >= LABEL_TYPE_BRANCH)
        {
            if (p_Computer->readMemLabelType(branchAddress) == LABEL_TYPE_BRANCH)
                branchAddressString.Printf("R%04X", branchAddress);
            if (p_Computer->readMemLabelType(branchAddress) == LABEL_TYPE_JUMP)
                branchAddressString.Printf("J%04X", branchAddress);
            if (p_Computer->readMemLabelType(branchAddress) == LABEL_TYPE_SUB)
                branchAddressString.Printf("S%04X", branchAddress);
        }
        else
        {
            if (disassemblePass_ == 1)
            {
                p_Computer->writeMemLabelType(branchAddress, LABEL_TYPE_BRANCH);
                branchAddressString.Printf("R%04X", branchAddress);
                disassembleAgain_ = true;
            }
            else
            {
                if (branchAddress >= 0xA000)
                    branchAddressString.Printf("%05XH", branchAddress);
                else
                    branchAddressString.Printf("%04XH", branchAddress);
            }
        }
    }
    else
        branchAddressString.Printf("%02X", p_Computer->readMem(address));
    
   return branchAddressString;
}

wxString DebugWindow::getLongAddressOrLabel(Word address, bool textAssembler, Word start, Word end)
{
    wxString branchAddressString;
    
    Word branchAddress = p_Computer->readMem(address)<<8|p_Computer->readMem(address+1);

    if (branchAddress < start || branchAddress > end)
    {
        if (branchAddress >= 0xA000)
            branchAddressString.Printf("%05XH", branchAddress);
        else
            branchAddressString.Printf("%04XH", branchAddress);
        labelInfo_[address-1].outOfRange = true;
        labelInfo_[address-1].outOfRangeAddress = branchAddress;
        return branchAddressString;
    }
    
    if (textAssembler)
    {
        labelInfo_[branchAddress].branchToFound = true;
        if (p_Computer->readMemLabelType(branchAddress) >= LABEL_TYPE_JUMP)
        {
            if (p_Computer->readMemLabelType(branchAddress) == LABEL_TYPE_JUMP)
                branchAddressString.Printf("J%04X", branchAddress);
            if (p_Computer->readMemLabelType(branchAddress) == LABEL_TYPE_SUB)
                branchAddressString.Printf("S%04X", branchAddress);
        }
        else
        {
            if (disassemblePass_ == 1)
            {
                p_Computer->writeMemLabelType(branchAddress, LABEL_TYPE_JUMP);
                branchAddressString.Printf("J%04X", branchAddress);
                disassembleAgain_ = true;
            }
            else
            {
                if (branchAddress >= 0xA000)
                    branchAddressString.Printf("%05XH", branchAddress);
                else
                    branchAddressString.Printf("%04XH", branchAddress);
            }
        }
    }
    else
        branchAddressString.Printf("%04X", branchAddress);
 
    return branchAddressString;
}

wxString DebugWindow::getSubAddressOrLabel(Word address, bool textAssembler, Word start, Word end)
{
    wxString branchAddressString;
    
    Word branchAddress = p_Computer->readMem(address)<<8|p_Computer->readMem(address+1);

    if (branchAddress < start || branchAddress > end)
    {
        if (branchAddress >= 0xA000)
            branchAddressString.Printf("%05XH", branchAddress);
        else
            branchAddressString.Printf("%04XH", branchAddress);
        labelInfo_[address-1].outOfRange = true;
        labelInfo_[address-1].outOfRangeAddress = branchAddress;
        return branchAddressString;
    }
    
    if (textAssembler)
    {
        labelInfo_[branchAddress].branchToFound = true;
        if (p_Computer->readMemLabelType(branchAddress) >= LABEL_TYPE_SUB)
            branchAddressString.Printf("S%04X", branchAddress);
        else
        {
            if (disassemblePass_ == 1)
            {
                p_Computer->writeMemLabelType(branchAddress, LABEL_TYPE_SUB);
                branchAddressString.Printf("S%04X", branchAddress);
                disassembleAgain_ = true;
            }
            else
            {
                if (branchAddress >= 0xA000)
                    branchAddressString.Printf("%05XH", branchAddress);
                else
                    branchAddressString.Printf("%04XH", branchAddress);
            }
        }
    }
    else
        branchAddressString.Printf("%04X", branchAddress);
    
    return branchAddressString;
}

wxString DebugWindow::getLoadAddressOrLabel(Word address, Word start, Word end)
{
    wxString branchAddressString;
    
    Word branchAddress = p_Computer->readMem(address)<<8|p_Computer->readMem(address+3);
    
    if (branchAddress < start || branchAddress > end)
    {
        if (branchAddress >= 0xA000)
            branchAddressString.Printf("%05XH", branchAddress);
        else
            branchAddressString.Printf("%04XH", branchAddress);
        labelInfo_[address-1].outOfRange = true;
        labelInfo_[address-1].outOfRangeAddress = branchAddress;
        return branchAddressString;
    }
    
    labelInfo_[branchAddress].branchToFound = true;
    if (p_Computer->readMemLabelType(branchAddress) >= LABEL_TYPE_SUB)
    {
        branchAddressString.Printf("S%04X", branchAddress);
    }
    else
    {
        if (disassemblePass_ == 1)
        {
            p_Computer->writeMemLabelType(branchAddress, LABEL_TYPE_SUB);
            branchAddressString.Printf("S%04X", branchAddress);
            disassembleAgain_ = true;
        }
        else
        {
            if (branchAddress >= 0xA000)
                branchAddressString.Printf("%05XH", branchAddress);
            else
                branchAddressString.Printf("%04XH", branchAddress);
        }
    }
    
    return branchAddressString;
}

wxString DebugWindow::getLoadAddress(Word address)
{
    wxString branchAddressString;
    
    Word branchAddress = p_Computer->readMem(address)<<8|p_Computer->readMem(address+3);
    
    if (branchAddress >= 0xA000)
        branchAddressString.Printf("%05XH", branchAddress);
    else
        branchAddressString.Printf("%04XH", branchAddress);

    return branchAddressString;
}

wxString DebugWindow::getCurrentAddresssLabel(Word address)
{
    wxString addressString = "";
    
    if (p_Computer->readMemLabelType(address) == LABEL_TYPE_BRANCH)
        addressString.Printf("R%04X", address);
    if (p_Computer->readMemLabelType(address) == LABEL_TYPE_JUMP)
        addressString.Printf("J%04X", address);
    if (p_Computer->readMemLabelType(address) == LABEL_TYPE_SUB)
        addressString.Printf("S%04X", address);
    
    return addressString;
}

wxString DebugWindow::getHexByte(Word address, bool textAssembler)
{
    wxString branchAddressString;

    if (textAssembler)
    {
        if (p_Computer->readMem(address) >= 0xA0)
            branchAddressString.Printf("%03XH", p_Computer->readMem(address));
        else
            branchAddressString.Printf("%02XH", p_Computer->readMem(address));
    }
    else
        branchAddressString.Printf("%02X",p_Computer->readMem(address));
    
    return branchAddressString;
}

AssInput DebugWindow::getAssInput(wxString buffer)
{
	AssInput result;
	result.numberOfParameters = 0;
	result.errorCode = 0;

	if (buffer.Left(1) == ':')
		buffer.Replace(":",": ", false);

	result.command = extractNextWord(&buffer, &result.commandSeperator);

	while (buffer != "" && result.numberOfParameters <= 8)
	{
		result.parameterString[result.numberOfParameters] = extractNextWord(&buffer, &result.seperator[result.numberOfParameters]);

		result.errorCode = translateChipParameter(result.parameterString[result.numberOfParameters], &result.parameterValue[result.numberOfParameters], &result.parameterType[result.numberOfParameters]);
		if (result.errorCode != 0)  return result;

		result.numberOfParameters++;
	}
	
	return result;
}

int DebugWindow::assembleSt2(wxString *buffer, Byte* b1, Byte* b2)
{
	AssInput assInput = getAssInput(*buffer);
	if (assInput.errorCode != 0)  return assInput.errorCode;
	if (assInput.commandSeperator != " ") return ERROR_COMMAND_SEP;

	if (assInput.command == "ADD") 
	{
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != ASS_HEX_VALUE && assInput.parameterType[1] != CHIP8_VX)
			return ERROR_8REG;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
		{ // ADD vx, kk
			if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
				return ERROR_8BIT;
			if (assInput.parameterValue[0] == 0)
				return ERROR_NO_REG0;
			*b1 = 0x70 | assInput.parameterValue[0];
			*b2 = assInput.parameterValue[1];
			return 2;
		}
		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{ // ADD Vx, Vy
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x04;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "AFOR") 
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX)
		{
			if (assInput.parameterValue[0] == 0) return ERROR_NO_REG0;
			*b1 = 0xF0 | assInput.parameterValue[0];
			*b2 = 0xb6;
			return 2;
		}
		return ERROR_REG_EXP;
	}
	if (assInput.command == "AND") 
	{ // AND Vx, Vy
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x02;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "CALL") 
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		if (assInput.parameterType[0] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xfff)
				return ERROR_12BIT;
			*b1 = (assInput.parameterValue[0] >> 8) | 0x20;
			*b2 = assInput.parameterValue[0] & 0xff;
			return 2;
		}
		return ERROR_12BIT;
	}
	if (assInput.command == "DJNZ") 
	{
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != ASS_HEX_VALUE)
			return ERROR_8BIT;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[0] != 0)
				return ERROR_ONLY_REG0;
			if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
				return ERROR_8BIT;
			*b1 = 0x70;
			*b2 = assInput.parameterValue[1];
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "JKP") 
	{
		if (assInput.parameterType[0] != CHIP8_VX && assInput.parameterType[0] != ASS_HEX_VALUE)
			return ERROR_4REG;
		if (assInput.parameterType[1] != ASS_HEX_VALUE)
			return ERROR_8BIT;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.parameterType[1] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[0] == 0xf) 
				return ERROR_NO_VALUE_F;
			if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xf)
				return ERROR_4BIT;
			if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
				return ERROR_8BIT;
			*b1 = 0xd0 | assInput.parameterValue[0];
			*b2 = assInput.parameterValue[1];
			return 2;
		}
		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[0] != 0xb) 
				return ERROR_ONLY_REGB;
			if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
				return ERROR_8BIT;
			*b1 = 0xdf;
			*b2 = assInput.parameterValue[1];
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "JP") 
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		if (assInput.parameterType[0] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xfff)
				return ERROR_12BIT;
			*b1 = (assInput.parameterValue[0] >> 8) | 0x10;
			*b2 = assInput.parameterValue[0] & 0xff;
			return 2;
		}
		return ERROR_12BIT;
	}
	if (assInput.command == "JNZ") 
	{
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != ASS_HEX_VALUE)
			return ERROR_8BIT;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
				return ERROR_8BIT;
			*b1 = 0x30 | assInput.parameterValue[0];
			*b2 = assInput.parameterValue[1];
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "JZ") 
	{
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != ASS_HEX_VALUE)
			return ERROR_8BIT;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
				return ERROR_8BIT;
			*b1 = 0x40 | assInput.parameterValue[0];
			*b2 = assInput.parameterValue[1];
			return 2;
		}
		return ERROR_8BIT;
	}
	if (assInput.command == "LD") 
	{
		if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.parameterValue[0] == 0xb)
		{ // LD B, [Vy], Vx or LD B, Vy, Vx
			if (assInput.parameterType[1] != CHIP8_VX && assInput.parameterType[1] != CHIP8_VX_MEM)
				return ERROR_REG_EXP;
			if (assInput.parameterType[2] != CHIP8_VX)
				return ERROR_REG_EXP;
			if (assInput.seperator[0] != "," || assInput.seperator[1] != "," )
				return ERROR_COMMA;
			if (assInput.seperator[2] != " " || assInput.numberOfParameters > 3)
				return ERROR_PAR;

			if ((assInput.parameterType[1] == CHIP8_VX_MEM || assInput.parameterType[1] == CHIP8_VX)  && assInput.parameterType[2] == CHIP8_VX)
			{  
				*b1 = 0x90 | assInput.parameterValue[2];
				*b2 = assInput.parameterValue[1] << 4 | 0x8;
				return 2;
			}
		}
		if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "[I]" && assInput.numberOfParameters > 2)
		{ // LD [I], kk, n
			if (assInput.parameterType[1] != ASS_HEX_VALUE)
				return ERROR_8BIT;
			if (assInput.seperator[0] != "," || assInput.seperator[1] != "," )
				return ERROR_COMMA;

			if (assInput.parameterType[1] == ASS_HEX_VALUE && assInput.parameterType[2] == ASS_HEX_VALUE && assInput.numberOfParameters == 3)
			{ 
                if (assInput.parameterType[2] != ASS_HEX_VALUE)
                    return ERROR_4BIT;
                if (assInput.seperator[2] != " ")
                    return ERROR_PAR;
				if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
					return ERROR_8BIT;
				if (assInput.parameterValue[2] < 0 || assInput.parameterValue[2] > 0xf)
					return ERROR_4BIT;
				*b1 = 0xb0 | assInput.parameterValue[2];
				*b2 = assInput.parameterValue[1];
				return 2;
			}
            if (assInput.parameterType[1] == ASS_HEX_VALUE && assInput.parameterType[3] == ASS_HEX_VALUE && assInput.numberOfParameters == 4 && assInput.seperator[2] == "+")
            {//LD [I], kk, +n
                if (assInput.parameterType[3] != ASS_HEX_VALUE)
                    return ERROR_4BIT;
                if (assInput.seperator[3] != " ")
                    return ERROR_PAR;
                if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
                    return ERROR_8BIT;
                if (assInput.parameterValue[3] < 0 || assInput.parameterValue[3] > 0xf)
                    return ERROR_4BIT;
                *b1 = 0xb0 | assInput.parameterValue[3];
                *b2 = assInput.parameterValue[1];
                return 2;
            }
		}
		if (assInput.parameterType[0] != CHIP8_VX && assInput.parameterType[0] != ASS_STRING && assInput.parameterType[0] != CHIP8_VX_MEM)
			return ERROR_SYNTAX;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I")
		{ // LD I, aaa
			if (assInput.parameterType[1] != ASS_HEX_VALUE)
				return ERROR_12BIT;
			if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xfff)
				return ERROR_12BIT;
			*b1 = 0xa0 | (assInput.parameterValue[1]>>8);
			*b2 = assInput.parameterValue[1];
			return 2;
		}
		if (assInput.parameterType[0] == CHIP8_VX)
		{ 
			if (assInput.parameterType[1] == ASS_HEX_VALUE)
			{ // LD Vx, kk
				if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
					return ERROR_8BIT;
				*b1 = 0x60 | assInput.parameterValue[0];
				*b2 = assInput.parameterValue[1];
				return 2;
			}
			if (assInput.parameterType[1] == CHIP8_VX)
			{ // LD Vx, Vy
				*b1 = 0x80 | assInput.parameterValue[0];
				*b2 = (assInput.parameterValue[1] << 4) &0xf0;
				return 2;
			}
			if (assInput.parameterType[1] == CHIP8_VX_MEM)
			{ // LD [Vx], Vy
				*b1 = 0x90 | assInput.parameterValue[0];
				*b2 = (assInput.parameterValue[1] << 4) | 0x2;
				return 2;
			}
			return ERROR_8REG;
		}
		if (assInput.parameterType[0] == CHIP8_VX_MEM && assInput.parameterType[1] == CHIP8_VX)
		{ // LD [Vy], Vx
			*b1 = 0x90 | assInput.parameterValue[1];
			*b2 = (assInput.parameterValue[0] << 4) | 0x4;
			return 2;
		}
		if (assInput.parameterType[0] == ASS_STRING && (assInput.parameterString[0] == "[>I]" || assInput.parameterString[0] == "[I]") && assInput.parameterType[1] == ASS_HEX_VALUE)
		{ // LD [>I], 0 or LD [I], 0
			if (assInput.parameterValue[1] != 0)
				return ERROR_ONLY_VALUE_0;
			*b1 = 0x02;
			*b2 = 0xf2;
			return 2;
		}
		return ERROR_8REG;
	} 
	if (assInput.command == "LDM") 
	{ // LDM Vx, [I] or LDM [I], Vx
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "[I]")
		{
			if (assInput.parameterValue[0] == 0) 
				return ERROR_NO_REG0;
			*b1 = 0xf0 | assInput.parameterValue[0];
			*b2 = 0xa6;
			return 2;
		}
		if (assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "[I]")
		{
			if (assInput.parameterValue[1] == 0) 
				return ERROR_NO_REG0;
			*b1 = 0xf0 | assInput.parameterValue[1];
			*b2 = 0xa9;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "LDMA") 
	{ // LDMA Vx, [I] or LDMA [I], Vx
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "[I]")
		{
			if (assInput.parameterValue[0] == 0) 
				return ERROR_NO_REG0;
			*b1 = 0xf0 | assInput.parameterValue[0];
			*b2 = 0xac;
			return 2;
		}
		if (assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "[I]")
		{
			if (assInput.parameterValue[1] == 0) 
				return ERROR_NO_REG0;
			*b1 = 0xf0 | assInput.parameterValue[1];
			*b2 = 0xaf;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "OR") 
	{ // OR Vx, Vy
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x01;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "RETURN")
	{ // RET
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;

		*b1 = 0xc0;
		return 1;
	} 
	if (assInput.command == "RND") 
	{ // RND Vx, kk
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != ASS_HEX_VALUE)
			return ERROR_8BIT;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
				return ERROR_8BIT;
			*b1 = 0xc0 | assInput.parameterValue[0];
			*b2 = assInput.parameterValue[1];
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "SLSB") 
	{ // SLSB Vx
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX)
		{
			if (assInput.parameterValue[0] == 0) return ERROR_NO_REG0;
			*b1 = 0xF0 | assInput.parameterValue[0];
			*b2 = 0xb3;
			return 2;
		}
		return ERROR_REG_EXP;
	}
	if (assInput.command == "SHL") 
	{ // SHL Vx or SHL Vx, Vy
		if (assInput.parameterType[0] == CHIP8_VX && assInput.numberOfParameters == 1)
		{
			if (assInput.seperator[0] != " ")
				return ERROR_REG_EXP;
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[0] << 4) | 0xe;
			return 2;
		}
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0xe;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "SHR") 
	{ // SHR Vx or SHR Vx, Vy
		if (assInput.parameterType[0] == CHIP8_VX && assInput.numberOfParameters == 1)
		{
			if (assInput.seperator[0] != " ")
				return ERROR_REG_EXP;
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[0] << 4) | 0x6;
			return 2;
		}
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x6;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "SNE") 
	{ // SNE Vx, kk or SNE Vx, Vy
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != ASS_HEX_VALUE && assInput.parameterType[1] != CHIP8_VX)
			return ERROR_8REG;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
				return ERROR_8BIT;
			*b1 = 0x50 | assInput.parameterValue[0];
			*b2 = assInput.parameterValue[1];
			return 2;
		}
		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x90 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) & 0xf0;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "SP") 
	{ 
		if (assInput.parameterType[0] != ASS_STRING && assInput.parameterType[0] != CHIP8_VX)
			return ERROR_SPRITE;
		if (assInput.numberOfParameters == 1)
		{ 
			if (assInput.seperator[0] != " ")
				return ERROR_SYNTAX;
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "CLEAR")
			{ // SP CLEAR
				*b1 = 0xe0;
				return 1;
			}
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "MOVE")
			{ // SP MOVE
				*b1 = 0xe1;
				return 1;
			}
			return ERROR_SPRITE;
		}
		if (assInput.parameterType[1] != ASS_STRING && assInput.parameterType[1] != ASS_HEX_VALUE)
			return ERROR_SPRITE;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.numberOfParameters == 2)
		{ 
			if (assInput.seperator[1] != " ")
				return ERROR_SYNTAX;
			if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "MOVE")
			{ // SP VC, Move
				if (assInput.parameterValue[0] != 0xc) 
					return ERROR_ONLY_REGC;
				*b1 = 0xe2;
				return 1;
			}
			if (assInput.parameterType[0] == ASS_STRING &&  assInput.parameterString[0] == "XOR" && assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "[I]")
			{ // SP XOR, [I]
				*b1 = 0xe4;
				return 1;
			}
			if (assInput.parameterType[0] == ASS_STRING &&  assInput.parameterString[0] == "DRW" && assInput.parameterType[1] == ASS_HEX_VALUE)
			{ // SP DRW, kk
				if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
					return ERROR_8BIT;
				*b1 = 0xe8;
				*b2 = assInput.parameterValue[1];
				return 2;
			}
		}
		if (assInput.parameterType[2] != ASS_HEX_VALUE || assInput.parameterValue[2] < 0 || assInput.parameterValue[2] > 0xff)
			return ERROR_8BIT;
		if (assInput.seperator[1] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[2] != " " || assInput.numberOfParameters > 3)
			return ERROR_PAR;

		if (assInput.parameterType[0] == ASS_STRING &&  assInput.parameterString[0] == "DRW" && assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "JC")
		{ // SP DRW, JC, kk
			*b1 = 0xe8;
			*b2 = assInput.parameterValue[2];
			return 2;
		}
		return ERROR_VIDEO;
	} 
	if (assInput.command == "SUB") 
	{ // SUB Vx, Vy
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x05;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "SUBN") 
	{ // SUBN Vx, Vy
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x07;
			return 2;
		}
		return ERROR_8REG;
	}
	if (assInput.command == "SYS") 
	{ // SYS aaa
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		if (assInput.parameterType[0] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xfff)
				return ERROR_12BIT;
			*b1 = (assInput.parameterValue[0] >> 8) & 0xf;
			if (*b1 == 2)
				*b1 = 0xf0;
			*b2 = assInput.parameterValue[0] & 0xff;
			return 2;
		}
		return ERROR_12BIT;
	}
	if (assInput.command == "VIDEO")
	{ 
		if (assInput.parameterType[0] != ASS_STRING)
			return ERROR_VIDEO;
		if (assInput.numberOfParameters == 1)
		{ 
			if (assInput.seperator[0] != " ")
				return ERROR_SYNTAX;
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "ON")
			{ // VIDEO ON
				*b1 = 0;
				*b2 = 0x66;
				return 2;
			}
		}
		return ERROR_VIDEO;
	}
	if (assInput.command == "XOR") 
	{ // XOR Vx, Vy
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x03;
			return 2;
		}
		return ERROR_8REG;
	}
	return ERROR_INST;
}

int DebugWindow::assembleChip(wxString *buffer, Byte* b1, Byte* b2)
{
	AssInput assInput = getAssInput(*buffer);
	if (assInput.errorCode != 0)  return assInput.errorCode;
	if (assInput.commandSeperator != " ") return ERROR_COMMAND_SEP;

	if (assInput.command == "ADD") 
	{
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I" && assInput.parameterType[1] == CHIP8_VX)
		{ // ADD I, Vx
			*b1 = 0xf0 | assInput.parameterValue[1];
			*b2 = 0x1e;
			return 2;
		}

		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != ASS_HEX_VALUE && assInput.parameterType[1] != CHIP8_VX)
			return ERROR_8REG;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
		{ // ADD Vx, kk
			if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
				return ERROR_8BIT;
			*b1 = 0x70 | assInput.parameterValue[0];
			*b2 = assInput.parameterValue[1];
			return 2;
		}
		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{ // ADD Vx, Vy
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x04;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "AND") 
	{ // ADD Vx, Vy
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x02;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "CALL") 
	{ // CALL aaa
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		if (assInput.parameterType[0] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xfff)
				return ERROR_12BIT;
			*b1 = (assInput.parameterValue[0] >> 8) | 0x20;
            if (runningComputer_ == STUDIO || runningComputer_ == VICTORY)
            {
                if (assInput.parameterValue[0] < 0x2fc)
                    return ERROR_STUDIO_CHIP_ADDRESS;
                if ((*b1 & 0xf) >= 0xc)
                    *b1 = *b1 & 0xfb;
                *b1 -= 1;
            }
            *b2 = assInput.parameterValue[0] & 0xff;
			return 2;
		}
		return ERROR_12BIT;
	}
	if (assInput.command == "CLS") 
	{ // CLS
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;

		*b1 = 0x00;
		*b2 = 0xe0;
		return 2;
	}
	if (assInput.command == "DRW") 
	{ // DRW Vx, Vy, n
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[2] != ASS_HEX_VALUE)
			return ERROR_4BIT;
		if (assInput.seperator[0] != "," || assInput.seperator[1] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[2] != " " || assInput.numberOfParameters > 3)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xf)
				return ERROR_4BIT;
			*b1 = 0xd0 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | assInput.parameterValue[2];
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "JP") 
	{
		if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.numberOfParameters == 1 && assInput.seperator[0] == " ")
		{ // JP aaa
			if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xfff)
				return ERROR_12BIT;
			*b1 = (assInput.parameterValue[0] >> 8) | 0x10;
            if (runningComputer_ == STUDIO || runningComputer_ == VICTORY)
            {
                if (assInput.parameterValue[0] < 0x2fc)
                    return ERROR_STUDIO_CHIP_ADDRESS;
                if ((*b1 & 0xf) >= 0xc)
                    *b1 = *b1 & 0xfb;
                *b1 -= 1;
            }
			*b2 = assInput.parameterValue[0] & 0xff;
			return 2;
		}

		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != ASS_HEX_VALUE)
			return ERROR_12BIT;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
		{ // JP V0, aaa
			if (p_Computer->getChip8Type() == CHIP8X)
				return ERROR_NS_8X;
			if (assInput.parameterValue[0] != 0)
				return ERROR_ONLY_REG0;
			*b1 = 0xb0 | (assInput.parameterValue[1]>>8);
            if (runningComputer_ == STUDIO || runningComputer_ == VICTORY)
            {
                if (assInput.parameterValue[0] < 0x2fc)
                    return ERROR_STUDIO_CHIP_ADDRESS;
                if ((*b1 & 0xf) >= 0xc)
                    *b1 = *b1 & 0xfb;
                *b1 -= 1;
            }
            *b2 = assInput.parameterValue[1] & 0xff;
			return 2;
		}
		return ERROR_12BIT;
	}
	if (assInput.command == "LD") 
	{
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[0] == 0xb)
			{ // LD B, Vx
				if (assInput.parameterType[1] != CHIP8_VX)
					return ERROR_REG_EXP;
				*b1 = 0xf0 | assInput.parameterValue[1];
				*b2 = 0x33;
				return 2;
			}
			if (assInput.parameterValue[0] == 0xf)
			{ // LD F, Vx
				if (assInput.parameterType[1] != CHIP8_VX)
					return ERROR_REG_EXP;
				*b1 = 0xf0 | assInput.parameterValue[1];
				*b2 = 0x29;
				return 2;
			}
			return ERROR_LD;
		}
		if (assInput.parameterType[0] == ASS_STRING)
		{
			if (assInput.parameterString[0] == "DT")
			{ // LD DT, Vx
				if (assInput.parameterType[1] != CHIP8_VX)
					return ERROR_REG_EXP;
				*b1 = 0xf0 | assInput.parameterValue[1];
				*b2 = 0x15;
				return 2;
			}
			if (assInput.parameterString[0] == "I")
			{ // LD I, aaa
				if (assInput.parameterType[1] != ASS_HEX_VALUE || assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xfff)
					return ERROR_12BIT;

				*b1 = 0xa0 | (assInput.parameterValue[1]>>8);
                if (runningComputer_ == STUDIO || runningComputer_ == VICTORY)
                {
                    if (assInput.parameterValue[0] < 0x100)
                        return ERROR_STUDIO_CHIP_ADDRESS_I;
                    if ((*b1 & 0xf) >= 0xc)
                        *b1 = *b1 & 0xfb;
                    else if ((*b1 & 0xf) >= 0x8)
                        *b1 = *b1 | 0x4;
                    *b1 -= 1;
                }
				*b2 = assInput.parameterValue[1];
				return 2;
			}
			if (assInput.parameterString[0] == "ST")
			{ // LD ST, Vx
				if (assInput.parameterType[1] != CHIP8_VX)
					return ERROR_REG_EXP;
				*b1 = 0xf0 | assInput.parameterValue[1];
				*b2 = 0x18;
				return 2;
			}
			if (assInput.parameterString[0] == "[I]")
			{ // LD [I], Vx
				if (assInput.parameterType[1] != CHIP8_VX)
					return ERROR_REG_EXP;
				*b1 = 0xf0 | assInput.parameterValue[1];
				*b2 = 0x55;
				return 2;
			}
			if (assInput.parameterString[0] == "P")
			{ // LD P, Vx
				if (p_Computer->getChip8Type() != CHIPETI)
					return ERROR_NS_8;
				*b1 = 0xf0 | assInput.parameterValue[1];
				*b2 = 0x00;
				return 2;
			}
		}
		if (assInput.parameterType[0] == CHIP8_VX)
		{
			if (assInput.parameterType[1] == ASS_HEX_VALUE)
			{ // LD Vx, kk
				if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xff)
					return ERROR_8BIT;
				*b1 = 0x60 | assInput.parameterValue[0];
				*b2 = assInput.parameterValue[1];
				return 2;
			}
			if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "DT")
			{ // LD Vx, DT
				*b1 = 0xf0 | assInput.parameterValue[0];
				*b2 = 0x07;
				return 2;
			}
			if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "K")
			{ // LD Vx, K
				*b1 = 0xf0 | assInput.parameterValue[0];
				*b2 = 0x0a;
				return 2;
			}
			if (assInput.parameterType[1] == CHIP8_VX)
			{ // LD Vx Vy
				*b1 = 0x80 | assInput.parameterValue[0];
				*b2 = (assInput.parameterValue[0] << 4) &0xf0;
				return 2;
			}
			if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "[I]")
			{ // LD Vx, [I]
				*b1 = 0xf0 | assInput.parameterValue[0];
				*b2 = 0x65;
				return 2;
			}
		}
		return ERROR_LD;
	}
	if (assInput.command == "OR") 
	{ // OR Vx, Vy
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x01;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "RETURN")
	{ // RET
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;

		*b1 = 0x00;
		*b2 = 0xee;
		return 2;
	}
	if (assInput.command == "RND") 
	{ // RND Vx,, kk
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != ASS_HEX_VALUE)
			return ERROR_8BIT;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
				return ERROR_8BIT;

			*b1 = 0xc0 | assInput.parameterValue[0];
			*b2 = assInput.parameterValue[1];
			return 2;
		}
		return ERROR_8REG;
	}
	if (assInput.command == "SE") 
	{ 
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != ASS_HEX_VALUE && assInput.parameterType[1] != CHIP8_VX)
			return ERROR_8REG;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
		{ // SE Vx, kk
			if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
				return ERROR_8BIT;

			*b1 = 0x30 | assInput.parameterValue[0];
			*b2 = assInput.parameterValue[1];
			return 2;
		}
		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{ // SE vx, Vy
			*b1 = 0x50 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) & 0xf0;
			return 2;
		}
		return ERROR_8REG;
	}
	if (assInput.command == "SHL") 
	{ // SHL Vx or SHL Vx, Vy
		if (assInput.parameterType[0] == CHIP8_VX && assInput.numberOfParameters == 1)
		{
			if (assInput.seperator[0] != " ")
				return ERROR_REG_EXP;
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[0] << 4) | 0xe;
			return 2;
		}
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0xe;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "SHR") 
	{ // SHR Vx or SHR Vx, Vy
		if (assInput.parameterType[0] == CHIP8_VX && assInput.numberOfParameters == 1)
		{
			if (assInput.seperator[0] != " ")
				return ERROR_REG_EXP;
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[0] << 4) | 0x6;
			return 2;
		}
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x6;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "SKP") 
	{ // SKP Vx
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX)
		{
			*b1 = 0xe0 | assInput.parameterValue[0];
			*b2 = 0x9e;
			return 2;
		}
		return ERROR_REG_EXP;
	}
	if (assInput.command == "SKNP") 
	{ // SKNP
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX)
		{
			*b1 = 0xe0 | assInput.parameterValue[0];
			*b2 = 0xa1;
			return 2;
		}
		return ERROR_REG_EXP;
	}
	if (assInput.command == "SNE") 
	{
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != ASS_HEX_VALUE && assInput.parameterType[1] != CHIP8_VX)
			return ERROR_8REG;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
		{ // SNE Vx, kk
			if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
				return ERROR_8BIT;
			*b1 = 0x40 | assInput.parameterValue[0];
			*b2 = assInput.parameterValue[1];
			return 2;
		}
		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{ // SNE Vx, Vy
			*b1 = 0x90 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) & 0xf0;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "SUB") 
	{ // SUB Vx, Vy
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x05;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "SUBN") 
	{ // SUBN Vx, Vy
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x07;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "SYS") 
	{ // SYS aaa
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		if (assInput.parameterType[0] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xfff)
				return ERROR_12BIT;
			*b1 = (assInput.parameterValue[0] >> 8) & 0xf;
			*b2 = assInput.parameterValue[0] & 0xff;
			return 2;
		}
		return ERROR_12BIT;
	}
	if (assInput.command == "XOR") 
	{ // XOR Vx, Vy
		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x80 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x03;
			return 2;
		}
		return ERROR_SYNTAX;
	}
	if (assInput.command == "CLR" || assInput.command == "CLRL" || assInput.command == "CLRH")
	{
		if (p_Computer->getChip8Type() != CHIP8X)
		{
			if (p_Computer->getChip8Type() == CHIPETI)
				return ERROR_NS_ETI;
			else
				return ERROR_NS_8;
		}

		if (assInput.numberOfParameters > 4)
			return ERROR_PAR;

		if (assInput.numberOfParameters == 1)
		{
			if (assInput.command == "CLRH" || assInput.command == "CLRL")
				return ERROR_CLR;
			if (assInput.seperator[0] != " ")
				return ERROR_PAR;
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "BACK") 
			{ // CLR BACK or CLR B
				*b1 = 0x02;
				*b2 = 0xa0;
				return 2;
			}
			if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.parameterValue[0] == 0xb) 
			{ // CLR BACK or CLR B
				*b1 = 0x02;
				*b2 = 0xa0;
				return 2;
			}
			return ERROR_CLR;
		}

		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		
		if (assInput.numberOfParameters == 2)
		{ // CLR Vx, Vy or CLRH Vx, Vy
			if (assInput.command == "CLRH")
				return ERROR_CLR;
			if (assInput.seperator[0] != ",")
				return ERROR_COMMA;
			if (assInput.seperator[1] != " ")
				return ERROR_PAR;
			if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
			{
				*b1 = 0xB0 | assInput.parameterValue[0];
				*b2 = (assInput.parameterValue[1] << 4) & 0xf0;
				return 2;
			}
			return ERROR_REG_EXP;
		}

		if (assInput.numberOfParameters == 3)
		{
			if (assInput.seperator[1] != ",")
				return ERROR_COMMA;
			if (assInput.seperator[2] != " ")
				return ERROR_PAR;
			
			if (assInput.seperator[0] == ",")
			{ // CLR Vx, Vy, n or CLRL Vx, Vy, n
				if (assInput.command == "CLRL")
					return ERROR_CLR;
				if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == ASS_HEX_VALUE)
				{
					if (assInput.parameterValue[2] < 0 || assInput.parameterValue[2] > 0xf)
						return ERROR_4BIT;
					if (assInput.parameterValue[2] == 0)
						return ERROR_NO_VALUE_0;
					*b1 = 0xB0 | assInput.parameterValue[0];
					*b2 = (assInput.parameterValue[1] << 4) | assInput.parameterValue[2];
					return 2;
				}
				return ERROR_CLR;
			}

			if (assInput.seperator[0] == "/")
			{ // CLR Vx/V(x+1), Vy or CLRH Vx/V(x+1), Vy
				if (assInput.command == "CLRH")
					return ERROR_CLR;
				if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == CHIP8_VX)
				{
					if (assInput.parameterValue[0] != assInput.parameterValue[1] - 1)
						return ERROR_2ND_REG;
					*b1 = 0xB0 | assInput.parameterValue[0];
					*b2 = (assInput.parameterValue[2] << 4) & 0xf0;
					return 2;
				}
				return ERROR_REG_EXP;
			}
		}

		if (assInput.numberOfParameters == 4)
		{ // CLR Vx/V(x+1), Vy, n or CLRL Vx/V(x+1), Vy, n
			if (assInput.command == "CLRL")
				return ERROR_CLR;
			if (assInput.seperator[0] != "/")
				return ERROR_SLASH;
			if (assInput.seperator[1] != "," || assInput.seperator[2] != ",")
				return ERROR_COMMA;
			
			if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == CHIP8_VX && assInput.parameterType[3] == ASS_HEX_VALUE)
			{
				if (assInput.parameterValue[3] < 0 || assInput.parameterValue[3] > 0xf)
					return ERROR_4BIT;
				if (assInput.parameterValue[3] == 0)
					return ERROR_NO_VALUE_0;
				if (assInput.parameterValue[0] != assInput.parameterValue[1] - 1)
					return ERROR_2ND_REG;
				*b1 = 0xB0 | assInput.parameterValue[0];
				*b2 = (assInput.parameterValue[2] << 4) | assInput.parameterValue[3];
				return 2;
			}
			return ERROR_CLR;
		}
		return ERROR_CLR;
	}
	if (assInput.command == "ADD8")
	{ // ADD8 Vx, Vy
		if (p_Computer->getChip8Type() != CHIP8X)
		{
			if (p_Computer->getChip8Type() == CHIPETI)
				return ERROR_NS_ETI;
			else
				return ERROR_NS_8;
		}

		if (assInput.parameterType[0] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
		if (assInput.seperator[0] != ",")
			return ERROR_COMMA;
		if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
		{
			*b1 = 0x50 | assInput.parameterValue[0];
			*b2 = (assInput.parameterValue[1] << 4) | 0x01;
			return 2;
		}
		return ERROR_REG_EXP;
	}
	if (assInput.command == "SKP2") 
	{ // SKP2 Vx
		if (p_Computer->getChip8Type() != CHIP8X)
		{
			if (p_Computer->getChip8Type() == CHIPETI)
				return ERROR_NS_ETI;
			else
				return ERROR_NS_8;
		}

		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX)
		{
			*b1 = 0xe0 | assInput.parameterValue[0];
			*b2 = 0xf2;
			return 2;
		}
		return ERROR_REG_EXP;
	}
	if (assInput.command == "SKNP2") 
	{ // SKNP2 Vx
		if (p_Computer->getChip8Type() != CHIP8X)
		{
			if (p_Computer->getChip8Type() == CHIPETI)
				return ERROR_NS_ETI;
			else
				return ERROR_NS_8;
		}

		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX)
		{
			*b1 = 0xe0 | assInput.parameterValue[0];
			*b2 = 0xf5;
			return 2;
		}
		return ERROR_REG_EXP;
	}
	if (assInput.command == "OUT3")
	{ // OUT3 Vx
		if (p_Computer->getChip8Type() != CHIP8X)
		{
			if (p_Computer->getChip8Type() == CHIPETI)
				return ERROR_NS_ETI;
			else
				return ERROR_NS_8;
		}

		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX)
		{
			*b1 = 0xf0 | assInput.parameterValue[0];
			*b2 = 0xf8;
			return 2;
		}
		return ERROR_REG_EXP;
	}
	if (assInput.command == "INP1")
	{ // INP1 Vx
		if (p_Computer->getChip8Type() != CHIP8X)
		{
			if (p_Computer->getChip8Type() == CHIPETI)
				return ERROR_NS_ETI;
			else
				return ERROR_NS_8;
		}

		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		if (assInput.parameterType[0] == CHIP8_VX)
		{
			*b1 = 0xf0 | assInput.parameterValue[0];
			*b2 = 0xfb;
			return 2;
		}
		return ERROR_REG_EXP;
	}
	return ERROR_INST;
}

int DebugWindow::assembleFel2(wxString *buffer, Byte* b1, Byte* b2, Byte* b3, Byte* b4)
{
    AssInput assInput = getAssInput(*buffer);
    if (assInput.errorCode != 0)  return assInput.errorCode;
    if (assInput.commandSeperator != " ") return ERROR_COMMAND_SEP;
    
    if (assInput.command == "ADD")
    {
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
		if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 
			if (assInput.numberOfParameters > 2)
				return ERROR_PAR;
			
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "[V0V1]" && 
			    assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "[V2V3]")
			{ // ADD [V0V1], [V2V3]
				*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
				*b2 = 0x54;
				return 2;
			}
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "V0V1" && 
			    assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "V2V3")
			{ // ADD V0V1, V2V3
				*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
				*b2 = 0x54;
				return 2;
			}
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I" && 
			    assInput.parameterType[1] == CHIP8_VX && assInput.parameterValue[1] == 9)
			{ // ADD I, V9
				*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
				*b2 = 0x8e;
				return 2;
			}
		}
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.numberOfParameters == 2)
        { // ADD Vx, Vy
            if (assembleCommand_[ADD_VX_VY_VZ] != -1)
            {
                if (assInput.seperator[1] != " ")
                    return ERROR_PAR;
                *b1 = assembleCommand_[ADD_VX_VY_VZ]  | assInput.parameterValue[0];
                *b2 = (assInput.parameterValue[1] << 4) | assInput.parameterValue[0];
                return 2;
            }
            if (assembleCommand_[FEL3_COMMAND_8] != -1)
            {
                if (assInput.seperator[1] != " ")
                    return ERROR_PAR;
                *b1 = assembleCommand_[FEL3_COMMAND_8]  | assInput.parameterValue[0];
                *b2 = (assInput.parameterValue[1] << 4) | 4;
                return 2;
            }
            if (assembleCommand_[STIV_COMMAND_4] != -1)
            {
                if (assInput.seperator[1] != " ")
                    return ERROR_PAR;
                *b1 = assembleCommand_[STIV_COMMAND_4]  | assInput.parameterValue[0];
                *b2 = assInput.parameterValue[1] | 0x40;
                return 2;
            }
            return ERROR_INST;
        }
        if (assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == CHIP8_VX && assInput.seperator[1] == "+")
        { // ADD Vz, Vx+Vy
            if (assembleCommand_[ADD_VX_VY_VZ] == -1)
                return ERROR_INST;
            if (assInput.seperator[2] != " " || assInput.numberOfParameters > 3)
                return ERROR_PAR;
            *b1 = assembleCommand_[ADD_VX_VY_VZ]  | assInput.parameterValue[1];
            *b2 = ((assInput.parameterValue[2]<<4)&0xf0)|assInput.parameterValue[0];
            return 2;
        }
        if (assInput.parameterType[0] == CHIP8_VX)
        {
            if (assInput.parameterType[1] == ASS_HEX_VALUE)
            { // ADD Vx, kk
                if (assembleCommand_[ADD_VX_KK] == -1)
                    return ERROR_INST;
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xff)
                    return ERROR_8BIT;
                *b1 = assembleCommand_[ADD_VX_KK] | assInput.parameterValue[0];
                *b2 = assInput.parameterValue[1];
                return 2;
            }
        }
        if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I")
        {
            if (assInput.parameterType[1] == ASS_HEX_VALUE)
            { // ADD I, kk
                if (assembleCommand_[STIV_COMMAND_6] == -1)
                    return ERROR_INST;
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
                    return ERROR_8BIT;
                *b1 = assembleCommand_[STIV_COMMAND_6] | 2;
                *b2 = assInput.parameterValue[1];
                return 2;
            }
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "ADD8")
    { // ADD8 Vx, Vy, Vz
        if (assembleCommand_[ADD8_VX_VY_N] == -1)
            return ERROR_INST;
        if (assInput.parameterType[0] != CHIP8_VX)
            return ERROR_REG_EXP;
        if (assInput.parameterType[1] != CHIP8_VX)
            return ERROR_REG_EXP;
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.seperator[2] != " " || assInput.numberOfParameters > 3)
            return ERROR_PAR;
        
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == CHIP8_VX)
        {
            *b1 = assembleCommand_[ADD8_VX_VY_N] | assInput.parameterValue[0];
            *b2 = (assInput.parameterValue[1] << 4) | assInput.parameterValue[2];
            return 2;
        }
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == FEL_C)
        {
            *b1 = assembleCommand_[ADD8_VX_VY_N] | assInput.parameterValue[0];
            *b2 = (assInput.parameterValue[1] << 4) | assInput.parameterValue[2];
            return 2;
        }
        return ERROR_REG_EXP;
    }
    if (assInput.command == "ADDN")
    {
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.numberOfParameters == 2)
        { // ADDN Vx, Vy
            if (assembleCommand_[STIV_COMMAND_4] != -1)
			{
				if (assInput.seperator[1] != " ")
					return ERROR_PAR;
				*b1 = assembleCommand_[STIV_COMMAND_4]  | assInput.parameterValue[0];
				*b2 = assInput.parameterValue[1] | 0xa0;
				return 2;
			}
            return ERROR_INST;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "AND")
    {
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.numberOfParameters == 2)
        { // AND Vx, Vy
            if (assembleCommand_[FEL3_COMMAND_8] != -1)
			{
				if (assInput.seperator[1] != " ")
					return ERROR_PAR;
				*b1 = assembleCommand_[FEL3_COMMAND_8]  | assInput.parameterValue[0];
				*b2 = (assInput.parameterValue[1] << 4) | 2;
				return 2;
			}
            if (assembleCommand_[STIV_COMMAND_4] != -1)
			{
				if (assInput.seperator[1] != " ")
					return ERROR_PAR;
				*b1 = assembleCommand_[STIV_COMMAND_4]  | assInput.parameterValue[0];
				*b2 = assInput.parameterValue[1] | 0x20;
				return 2;
			}
            return ERROR_INST;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "BEEP")
    {
        if (assembleCommand_[BEEP_F_KK_N] == -1)
            return ERROR_INST;
        if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.parameterType[1] == ASS_HEX_VALUE && assInput.numberOfParameters == 2)
        { // BEEP kk, d
            if (assInput.seperator[1] != " ")
                return ERROR_PAR;
            if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xff)
                return ERROR_8BIT;
            if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xf)
                return ERROR_4BIT;
            *b1 = assembleCommand_[BEEP_F_KK_N] | assInput.parameterValue[1];
            *b2 = assInput.parameterValue[0];
            return 2;
        }
        if (assInput.seperator[2] != " " || assInput.numberOfParameters > 3)
            return ERROR_PAR;
        if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.parameterValue[0]== 0xf && assInput.parameterType[1] == ASS_HEX_VALUE && assInput.parameterType[2] == ASS_HEX_VALUE)
        { // BEEP F=kk, d
            if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
                return ERROR_8BIT;
            if (assInput.parameterValue[2] < 0 || assInput.parameterValue[2] > 0xf)
                return ERROR_4BIT;
            *b1 = assembleCommand_[BEEP_F_KK_N] | assInput.parameterValue[2];
            *b2 = assInput.parameterValue[1];
            return 2;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "CALL")
    { 
        if (assembleCommand_[CALL_MMM] != -1)
		{ // CALL aaa
		   if (assInput.numberOfParameters > 1)
				return ERROR_PAR;
			
			if (assInput.parameterType[0] == ASS_HEX_VALUE)
			{
				if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xfff)
					return ERROR_12BIT;
				if (assInput.parameterValue[0] <= 0x1ff)
					return ERROR_FEL2_CHIP_ADDRESS;
				*b1 = assembleCommand_[CALL_MMM] | (assInput.parameterValue[0] >> 8);
				*b2 = assInput.parameterValue[0] & 0xff;
				return 2;
			}
			return ERROR_12BIT;
		}
        if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 
			if (assInput.parameterType[0] == ASS_HEX_VALUE)
			{ // CALL aaaa
				if (assInput.numberOfParameters > 1)
					return ERROR_PAR;
				if ((assInput.parameterValue[0] >= 0x600 && assInput.parameterValue[0] <= 0x8ff) ||
					(assInput.parameterValue[0] >= 0x1000 && assInput.parameterValue[0] <= 0x11ff))
				{
					*b1 = assembleCommand_[STIV_COMMAND_6] | ((assInput.parameterValue[0] >> 8) & 0xf);
					*b2 = assInput.parameterValue[0] & 0xff;
					return 2;
				}
				return ERROR_INCORR_ADDRESS;
			}
			if (assInput.parameterType[0] == ASS_STRING)
			{ // CALL I, KK
				if (assInput.numberOfParameters > 2)
					return ERROR_PAR;
				if (assInput.parameterString[0] == "I" && assInput.parameterType[1] == ASS_HEX_VALUE)
				{
					*b1 = assembleCommand_[STIV_COMMAND_6] | 9;
					*b2 = assInput.parameterValue[1] & 0xff;
					return 2;
				}
				return ERROR_REG;
			}
			return ERROR_INST;
		}
        return ERROR_INST;
    }
    if (assInput.command == "CHAR")
    { 
        if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 
 		    if (assInput.numberOfParameters > 3)
				return ERROR_PAR;
			
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "[I]")
			{ // CHAR [I], V0, V1 or CHAR [I]
				*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
				*b2 = 0x2c;
				return 2;
			}
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "[V2V3]")
			{ // CHAR [V2V3], V0, V1 or CHAR [V2V3]
				*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
				*b2 = 0x2e;
				return 2;
			}
			return ERROR_REG_EXP;
		}
        return ERROR_INST;
    }
    if (assInput.command == "CLR")
    {
		if (assembleCommand_[STIV_COMMAND_6] != -1 && assInput.numberOfParameters == 0)
		{ 
			*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
			*b2 = 0x88;
			return 2;
		}
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.numberOfParameters == 2)
        { // CLR Vx, Vy
            if (assembleCommand_[STIV_COMMAND_5] != -1)
			{
				if (assInput.seperator[1] != " ")
					return ERROR_PAR;
				*b1 = assembleCommand_[STIV_COMMAND_5]  | assInput.parameterValue[0];
				*b2 = assInput.parameterValue[1] | 0x50;
				return 2;
			}
            return ERROR_INST;
        }
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE && assInput.numberOfParameters == 2)
        { // CLR Vx, c
            if (assembleCommand_[STIV_COMMAND_5] != -1)
			{
                if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xf)
                    return ERROR_4BIT;
				if (assInput.seperator[1] != " ")
					return ERROR_PAR;
				*b1 = assembleCommand_[STIV_COMMAND_5]  | assInput.parameterValue[0];
				*b2 = assInput.parameterValue[1] | 0x60;
				return 2;
			}
            return ERROR_INST;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "CLS")
    { // CLS
		if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 
			if (assInput.numberOfParameters > 0)
				return ERROR_PAR;
			
			*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
			*b2 = 0x12;
			return 2;
		}
	}
	if (assInput.command == "CP")
    { 
        if (assembleCommand_[STIV_COMMAND_6] != -1 && assInput.numberOfParameters == 1)
		{
            if (assInput.parameterType[0] == ASS_STRING && (assInput.parameterString[0] == "[I]" || assInput.parameterString[0] == "I"))
			{ // CP [I]
				*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
				*b2 = 0x90;
				return 2;
			}
		}
		if (assembleCommand_[STIV_COMMAND_5] != -1)
		{
			if (assInput.seperator[0] != ",")
				return ERROR_COMMA;
			if (assInput.seperator[2] != " " || assInput.numberOfParameters > 3)
				return ERROR_PAR;

			if (assInput.parameterType[0] == CHIP8_VX)
			{ // CP Vx, Vy, [I]
				if (assInput.parameterType[1] != CHIP8_VX)
					return ERROR_REG_EXP;
				if (assInput.parameterType[2] != ASS_STRING && (assInput.parameterString[2] == "I" || assInput.parameterString[2] == "[I]"))
					return ERROR_REG_EXP;
				
				*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[0];
                *b2 = assInput.parameterValue[1] | 0xC0;
				return 2;
			}
			if (assInput.parameterType[1] == CHIP8_VX)
			{ // CP [I], Vx, Vy
				if (assInput.parameterType[2] != CHIP8_VX)
					return ERROR_REG_EXP;
				if (assInput.parameterType[0] != ASS_STRING && (assInput.parameterString[0] == "I" || assInput.parameterString[0] == "[I]"))
					return ERROR_REG_EXP;
				
				*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[1];
                *b2 = assInput.parameterValue[2] | 0xD0;
				return 2;
			}
			return ERROR_REG_EXP;
		}
		return ERROR_INST;
    }
    if (assInput.command == "DECB")
    { // DECB
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;

        if (assembleCommand_[FEL3_COMMAND_C] == -1)
            return ERROR_INST;
        *b1 = assembleCommand_[FEL3_COMMAND_C];
        *b2 = 0x54;
        return 2;
    }
    if (assInput.command == "DELAY")
    {
        if (assInput.parameterType[0] == CHIP8_VX && assInput.numberOfParameters == 1)
        { // DELAY Vx
            if (assembleCommand_[FEL1_COMMAND_7] == -1)
                return ERROR_INST;
            if (assInput.seperator[0] != " ")
                return ERROR_PAR;
            *b1 = assembleCommand_[FEL1_COMMAND_7]  | assInput.parameterValue[0];
            *b2 = 0x4B;
            return 2;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "DRW")
    { 
        if (assembleCommand_[DRW_VX_VY_N] != -1)
        { // DRW Vx, Vy, n
            if (assInput.parameterType[0] != CHIP8_VX)
                return ERROR_REG_EXP;
            if (assInput.parameterType[1] != CHIP8_VX)
                return ERROR_REG_EXP;
            if (assInput.parameterType[2] != ASS_HEX_VALUE)
                return ERROR_4BIT;
            if (assInput.seperator[0] != "," || assInput.seperator[1] != ",")
                return ERROR_COMMA;
            if (assInput.seperator[2] != " " || assInput.numberOfParameters > 3)
                return ERROR_PAR;
            
            if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == ASS_HEX_VALUE)
            {
                if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xf)
                    return ERROR_4BIT;
                *b1 = assembleCommand_[DRW_VX_VY_N] | assInput.parameterValue[0];
                *b2 = (assInput.parameterValue[1] << 4) | assInput.parameterValue[2];
                return 2;
            }
            return ERROR_SYNTAX;
        }
        if (assembleCommand_[DRW_VX_L_N] != -1)
        { // DRW Vx, l , n
            if (assInput.parameterType[0] != CHIP8_VX)
                return ERROR_REG_EXP;
            if (assInput.parameterType[1] != ASS_HEX_VALUE)
                return ERROR_4BIT;
            if (assInput.parameterType[2] != ASS_HEX_VALUE)
                return ERROR_4BIT;
            if (assInput.seperator[0] != "," || assInput.seperator[1] != ",")
                return ERROR_COMMA;
            if (assInput.seperator[2] != " " || assInput.numberOfParameters > 3)
                return ERROR_PAR;
            
            if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE && assInput.parameterType[2] == ASS_HEX_VALUE)
            {
                if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xf)
                    return ERROR_4BIT;
                *b1 = assembleCommand_[DRW_VX_L_N] | assInput.parameterValue[0];
                *b2 = (assInput.parameterValue[1] << 4) | assInput.parameterValue[2];
                return 2;
            }
            return ERROR_SYNTAX;
        }
        if (assembleCommand_[STIV_COMMAND_4] != -1 && assInput.numberOfParameters == 3)
        { // DRW I, Vx, n
            if (assInput.parameterType[0] != ASS_STRING || assInput.parameterString[0] != "I")
                return ERROR_REG;
            if (assInput.parameterType[1] != CHIP8_VX)
                return ERROR_REG_EXP;
            if (assInput.parameterType[2] != ASS_HEX_VALUE)
                return ERROR_4BIT;
            if (assInput.seperator[0] != "," || assInput.seperator[1] != ",")
                return ERROR_COMMA;
            if (assInput.seperator[2] != " ")
                return ERROR_PAR;
            
			if (assInput.parameterValue[2] < 0 || assInput.parameterValue[2] > 0xf)
				return ERROR_4BIT;
			*b1 = assembleCommand_[STIV_COMMAND_4] | assInput.parameterValue[1];
			*b2 = assInput.parameterValue[2] | 0xe0;
			return 2;
        }
        if (assembleCommand_[STIV_COMMAND_5] != -1)
        { // DRW I, Vx
            if (assInput.parameterType[0] != ASS_STRING || assInput.parameterString[0] != "I")
                return ERROR_REG;
            if (assInput.parameterType[1] != CHIP8_VX)
                return ERROR_REG_EXP;
            if (assInput.seperator[0] != ",")
                return ERROR_COMMA;
            if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                return ERROR_PAR;
            
			*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[1];
			*b2 = 0x20;
			return 2;
        }
        return ERROR_INST;
    }
    if (assInput.command == "DRWR")
    { 
        if (assembleCommand_[STIV_COMMAND_5] != -1)
        { // DRWR I, Vx
            if (assInput.parameterType[0] != ASS_STRING || assInput.parameterString[0] != "I")
                return ERROR_REG;
            if (assInput.parameterType[1] != CHIP8_VX)
                return ERROR_REG_EXP;
            if (assInput.seperator[0] != ",")
                return ERROR_COMMA;
            if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                return ERROR_PAR;
            
			*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[1];
			*b2 = 0x70;
			return 2;
        }
        return ERROR_INST;
    }
    if (assInput.command == "INCA")
    { // INCA
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        if (assembleCommand_[FEL3_COMMAND_C] != -1)
        {
            *b1 = assembleCommand_[FEL3_COMMAND_C];
            *b2 = 0x50;
            return 2;
        }
        if (assembleCommand_[FEL1_COMMAND_7] != -1)
        {
            *b1 = 0x7F;
            *b2 = 0x6F;
            return 2;
        }
        return ERROR_INST;
    }
    if (assInput.command == "INCB")
    { // INCB
        if (assembleCommand_[FEL3_COMMAND_C] == -1)
            return ERROR_INST;
        *b1 = assembleCommand_[FEL3_COMMAND_C];
        *b2 = 0x51;
        return 2;
    }
    if (assInput.command == "JE")
    { 
        if (assembleCommand_[JE_I_VX_KK] != -1 && (assInput.parameterType[1] == ASS_HEX_VALUE || assInput.parameterType[2] == ASS_HEX_VALUE ))
		{ // JE I, Vx, kk
			if (assInput.seperator[0] != ",")
				return ERROR_COMMA;
			if (assInput.numberOfParameters > 3)
				return ERROR_PAR;
			
            if (assInput.numberOfParameters == 2 && assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
            { // JE Vx, kk
                if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
                    return ERROR_8BIT;
                
                *b1 = assembleCommand_[JE_I_VX_KK] | assInput.parameterValue[0];
                *b2 = assInput.parameterValue[1];
                return 2;
            }
            if (assInput.parameterType[1] != CHIP8_VX)
                return ERROR_REG_EXP;
			if (assInput.numberOfParameters == 3 && assInput.seperator[2] == " " && assInput.seperator[1] == "," && assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I" && assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == ASS_HEX_VALUE)
			{ // JE I, Vx, kk
				if (assInput.parameterValue[2] < 0 || assInput.parameterValue[2] > 0xff)
					return ERROR_8BIT;
				
				*b1 = assembleCommand_[JE_I_VX_KK] | assInput.parameterValue[1];
				*b2 = assInput.parameterValue[2];
				return 2;
			}
			return ERROR_8REG;
		}
        if (assembleCommand_[STIV_COMMAND_5] != -1)
		{ // JE I, Vx, Vy
			if (assInput.parameterType[1] != CHIP8_VX)
				return ERROR_REG;
			if (assInput.seperator[0] != ",")
				return ERROR_COMMA;
			if (assInput.numberOfParameters > 3)
				return ERROR_PAR;
			
			if (assInput.numberOfParameters == 3 && assInput.seperator[2] == " " && assInput.seperator[1] == "," &&
                assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I" &&
                assInput.parameterType[2] == CHIP8_VX)
			{ // JE I, Vx, Vy
				*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[1];
				*b2 = assInput.parameterValue[2] | 0x30;
				return 2;
			}
			if (assInput.numberOfParameters == 2 && assInput.parameterType[0] == CHIP8_VX)
			{ // JE Vx, Vy
				*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[0];
				*b2 = assInput.parameterValue[1] | 0x30;
				return 2;
			}
			return ERROR_8REG;
		}
        return ERROR_INST;
    }
    if (assInput.command == "JG")
    { 
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
        if (assembleCommand_[STIV_COMMAND_5] != -1)
		{
			if (assInput.parameterType[1] != CHIP8_VX)
				return ERROR_REG;
			if (assInput.seperator[0] != ",")
				return ERROR_COMMA;
			if (assInput.numberOfParameters > 3)
				return ERROR_PAR;
			
			if (assInput.numberOfParameters == 3 && assInput.seperator[2] == " " && assInput.seperator[1] == "," &&
                assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I" &&
                assInput.parameterType[2] == CHIP8_VX)
			{ // JG I, Vx, Vy
				*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[1];
				*b2 = assInput.parameterValue[2] | 0xA0;
				return 2;
			}
			if (assInput.numberOfParameters == 2 && assInput.parameterType[0] == CHIP8_VX)
			{ // JG Vx, Vy
				*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[0];
				*b2 = assInput.parameterValue[1] | 0xA0;
				return 2;
			}
			return ERROR_INST;
		}
        return ERROR_INST;
    }
    if (assInput.command == "JK")
    { 
        if (assembleCommand_[STIV_COMMAND_5] != -1)
        { 
			if (assInput.numberOfParameters == 2)
			{ // JK I, Vy
				if (assInput.parameterType[0] != ASS_STRING || assInput.parameterString[0] != "I")
					return ERROR_REG;
				if (assInput.parameterType[1] != CHIP8_VX)
					return ERROR_REG_EXP;
				if (assInput.seperator[0] != ",")
					return ERROR_COMMA;
				if (assInput.seperator[1] != " ")
					return ERROR_PAR;
				
				*b1 = assembleCommand_[STIV_COMMAND_5];
				*b2 =  assInput.parameterValue[1] | 0x80;
				return 2;
			}
			if (assInput.numberOfParameters == 1)
			{ // JK Vy
				if (assInput.parameterType[0] != CHIP8_VX)
					return ERROR_REG_EXP;
				if (assInput.seperator[0] != " ")
					return ERROR_PAR;
				
				*b1 = assembleCommand_[STIV_COMMAND_5];
				*b2 =  assInput.parameterValue[0] | 0x80;
				return 2;
			}
			return ERROR_PAR;
        }
        return ERROR_INST;
    }
    if (assInput.command == "JNE")
    { // JNE I, Vx, kk
        if (assembleCommand_[JNE_I_VX_KK] == -1)
            return ERROR_INST;
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.numberOfParameters > 3)
            return ERROR_PAR;
        
        if (assInput.numberOfParameters == 2 && assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
        {
            if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
                return ERROR_8BIT;
            
            *b1 = assembleCommand_[JNE_I_VX_KK] | assInput.parameterValue[0];
            *b2 = assInput.parameterValue[1];
            return 2;
        }
        if (assInput.numberOfParameters == 3 && assInput.seperator[2] == " " && assInput.seperator[1] == "," && assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I" && assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == ASS_HEX_VALUE)
        {
            if (assInput.parameterValue[2] < 0 || assInput.parameterValue[2] > 0xff)
                return ERROR_8BIT;
            
            *b1 = assembleCommand_[JNE_I_VX_KK] | assInput.parameterValue[1];
            *b2 = assInput.parameterValue[2];
            return 2;
        }
        return ERROR_8REG;
    }
    if (assInput.command == "JNK")
    { 
        if (assembleCommand_[STIV_COMMAND_5] != -1)
        { 
			if (assInput.numberOfParameters == 2)
			{ // JNK I, Vy
				if (assInput.parameterType[0] != ASS_STRING || assInput.parameterString[0] != "I")
					return ERROR_REG;
				if (assInput.parameterType[1] != CHIP8_VX)
					return ERROR_REG_EXP;
				if (assInput.seperator[0] != ",")
					return ERROR_COMMA;
				if (assInput.seperator[1] != " ")
					return ERROR_PAR;
				
				*b1 = assembleCommand_[STIV_COMMAND_5];
				*b2 =  assInput.parameterValue[1] | 0x90;
				return 2;
			}
			if (assInput.numberOfParameters == 1)
			{ // JNK Vy
				if (assInput.parameterType[0] != CHIP8_VX)
					return ERROR_REG_EXP;
				if (assInput.seperator[0] != " ")
					return ERROR_PAR;
				
				*b1 = assembleCommand_[STIV_COMMAND_5];
				*b2 =  assInput.parameterValue[0] | 0x90;
				return 2;
			}
            return ERROR_PAR;
        }
        return ERROR_INST;
    }
    if (assInput.command == "JNZ")
    { // JNZ Vx, kk
        if (assembleCommand_[JNZ_VX_KK] == -1)
            return ERROR_INST;
        if (assInput.parameterType[0] != CHIP8_VX)
            return ERROR_REG_EXP;
        if (assInput.parameterType[1] != ASS_HEX_VALUE)
            return ERROR_8BIT;
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
            return ERROR_PAR;
        
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
        {
            if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
                return ERROR_8BIT;
            
            *b1 = assembleCommand_[JNZ_VX_KK] | assInput.parameterValue[0];
            *b2 = assInput.parameterValue[1];
            return 2;
        }
        return ERROR_8REG;
    }
    if (assInput.command == "JP")
    {
        if (assembleCommand_[JP_MMM] != -1)
		{
			if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.numberOfParameters == 1 && assInput.seperator[0] == " ")
			{ // JP aaa
				if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xfff)
					return ERROR_12BIT;
				if (assInput.parameterValue[0] < 0x1ff)
					return ERROR_FEL2_CHIP_ADDRESS;
				*b1 = assembleCommand_[JP_MMM] | (assInput.parameterValue[0] >> 8);
				*b2 = assInput.parameterValue[0] & 0xff;
				return 2;
			}
			return ERROR_12BIT;
		}
        if (assembleCommand_[STIV_COMMAND_6] != -1 && assInput.parameterType[0] == ASS_HEX_VALUE)
		{
			if (assInput.numberOfParameters == 1 && assInput.seperator[0] == " ")
			{ // JP KK
				if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xff)
					return ERROR_8BIT;
				*b1 = assembleCommand_[STIV_COMMAND_6] | 5;
				*b2 = assInput.parameterValue[0] & 0xff;
				return 2;
			}
			return ERROR_12BIT;
		}
        if (assembleCommand_[STIV_COMMAND_4] != -1)
        {// JP I or JP
            if (assInput.numberOfParameters > 1)
                return ERROR_PAR;
			
			if (assInput.numberOfParameters == 0 || (assInput.numberOfParameters == 1 && assInput.parameterString[0] == "I" && assInput.parameterType[0] == ASS_STRING))
			{
				*b1 = assembleCommand_[STIV_COMMAND_4];
				*b2 = 0xb0;
				return 2;
			}
        }
        return ERROR_INST;
    }
    if (assInput.command == "JS")
    { 
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
        if (assembleCommand_[STIV_COMMAND_5] != -1)
		{ // JS I, Vx, Vy
			if (assInput.parameterType[1] != CHIP8_VX)
				return ERROR_REG;
			if (assInput.seperator[0] != ",")
				return ERROR_COMMA;
			if (assInput.numberOfParameters > 3)
				return ERROR_PAR;
			
			if (assInput.numberOfParameters == 3 && assInput.seperator[2] == " " && assInput.seperator[1] == "," &&
                assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I" &&
                assInput.parameterType[2] == CHIP8_VX)
			{ // JS I, Vx, Vy
				*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[1];
				*b2 = assInput.parameterValue[2] | 0xB0;
				return 2;
			}
			if (assInput.numberOfParameters == 2 && assInput.parameterType[0] == CHIP8_VX)
			{ // JS Vx, Vy
				*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[0];
				*b2 = assInput.parameterValue[1] | 0xB0;
				return 2;
			}
			return ERROR_INST;
		}
        return ERROR_INST;
    }
    if (assInput.command == "JU")
    { 
		if (assInput.parameterType[1] != CHIP8_VX)
			return ERROR_REG_EXP;
        if (assembleCommand_[STIV_COMMAND_5] != -1)
		{
			if (assInput.parameterType[1] != CHIP8_VX)
				return ERROR_REG;
			if (assInput.seperator[0] != ",")
				return ERROR_COMMA;
			if (assInput.numberOfParameters > 3)
				return ERROR_PAR;
			
			if (assInput.numberOfParameters == 3 && assInput.seperator[2] == " " && assInput.seperator[1] == "," &&
                assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I" &&
                assInput.parameterType[2] == CHIP8_VX)
			{ // JU I, Vx, Vy
				*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[1];
				*b2 = assInput.parameterValue[2] | 0x40;
				return 2;
			}
			if (assInput.numberOfParameters == 2 && assInput.parameterType[0] == CHIP8_VX)
			{ // JU Vx, Vy
				*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[0];
				*b2 = assInput.parameterValue[1] | 0x40;
				return 2;
			}
			return ERROR_PAR;
		}
        return ERROR_INST;
    }
    if (assInput.command == "JZ")
    { // JZ Vx, kk
        if (assembleCommand_[JZ_VX_KK] == -1)
            return ERROR_INST;
        if (assInput.parameterType[0] != CHIP8_VX)
            return ERROR_REG_EXP;
        if (assInput.parameterType[1] != ASS_HEX_VALUE)
            return ERROR_8BIT;
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
            return ERROR_PAR;
        
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
        {
            if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
                return ERROR_8BIT;
            
            *b1 = assembleCommand_[JZ_VX_KK] | assInput.parameterValue[0];
            *b2 = assInput.parameterValue[1];
            return 2;
        }
        return ERROR_8REG;
    }
    if (assInput.command == "KEY")
    {
        if (assInput.parameterType[0] == CHIP8_VX && assInput.numberOfParameters == 1)
        { // KEY Vy
            if (assembleCommand_[STIV_COMMAND_4] == -1)
                return ERROR_INST;
            if (assInput.seperator[0] != " ")
                return ERROR_PAR;
            *b1 = assembleCommand_[STIV_COMMAND_4]  ;
            *b2 = assInput.parameterValue[0] | 0xf0;
            return 2;
        }
		if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 
			if (assInput.numberOfParameters > 1)
				return ERROR_PAR;
			
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "SWITCH")
			{ // KEY SWITCH
				*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
				*b2 = 0x1c;
				return 2;
			}
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "WAIT")
			{ // KEY WAIT
				*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
				*b2 = 0xaa;
				return 2;
			}
            return ERROR_INCORRECT_PAR;
		}
        return ERROR_INST;
    }
    if (assInput.command == "KEYP")
    {
        if (assInput.parameterType[0] == CHIP8_VX && assInput.numberOfParameters == 1)
        { // KEYP Vy
            if (assembleCommand_[STIV_COMMAND_4] == -1)
                return ERROR_INST;
            if (assInput.seperator[0] != " ")
                return ERROR_PAR;
            *b1 = assembleCommand_[STIV_COMMAND_4]  ;
            *b2 = assInput.parameterValue[0] | 0x70;
            return 2;
        }
        return ERROR_INST;
    }
    if (assInput.command == "KEYR")
    {
        if (assInput.parameterType[0] == CHIP8_VX && assInput.numberOfParameters == 1)
        { // KEYR Vy
            if (assembleCommand_[STIV_COMMAND_4] == -1)
                return ERROR_INST;
            if (assInput.seperator[0] != " ")
                return ERROR_PAR;
            *b1 = assembleCommand_[STIV_COMMAND_4]  ;
            *b2 = assInput.parameterValue[0] | 0x80;
            return 2;
        }
        return ERROR_INST;
    }
    if (assInput.command == "LD")
    {
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.numberOfParameters > 3)
            return ERROR_PAR;
        
        if (assInput.parameterType[0] == ASS_HEX_VALUE || assInput.parameterType[0] == ASS_REG)
        {
            if (assInput.parameterValue[0] == 0xa)
            {
                if (assembleCommand_[LD_RA_MMM] != -1)
                { // LD A, MMM
                    if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                        return ERROR_PAR;
                    if (assInput.parameterType[1] != ASS_HEX_VALUE)
                        return ERROR_12BIT;
                    if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xfff)
                        return ERROR_12BIT;
                    *b1 = assembleCommand_[LD_RA_MMM] | ((assInput.parameterValue[1] >> 8) & 0xf);
                    *b2 = assInput.parameterValue[1] & 0xff;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterValue[0] == 0xb && assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "[RA]" && assInput.parameterType[2] == CHIP8_VX && assInput.numberOfParameters == 3)
            { // LD B, [RA], Vx
                if (assembleCommand_[FEL1_COMMAND_7] == -1)
                    return ERROR_INST;
                if (assInput.seperator[2] != " ")
                    return ERROR_PAR;
                *b1 = assembleCommand_[FEL1_COMMAND_7] | assInput.parameterValue[2];
                *b2 = 0x52;
                return 2;
            }
            if (assInput.parameterValue[0] == 0xb)
            {
                if (assembleCommand_[LD_B_VX_VY] != -1 && assInput.parameterType[0] == ASS_HEX_VALUE)
                { // LD B, Vy, Vx
                    if (assInput.seperator[2] != " ")
                        return ERROR_PAR;
                    if (assInput.parameterType[1] != CHIP8_VX)
                        return ERROR_REG_EXP;
                    if (assInput.parameterType[2] != CHIP8_VX)
                        return ERROR_REG_EXP;
                    *b1 = assembleCommand_[LD_B_VX_VY] | assInput.parameterValue[2];
                    *b2 = ((assInput.parameterValue[1]<<4)&0xf0);
                    return 2;
                }
                if (assembleCommand_[STIV_COMMAND_4] != -1 && assInput.parameterType[0] == ASS_HEX_VALUE)
                { // LD B, Vy, Vx or LD B, [Vy], Vx
                    if (assInput.seperator[2] != " ")
                        return ERROR_PAR;
                    if (assInput.parameterType[1] != CHIP8_VX && assInput.parameterType[1] != CHIP8_VX_MEM)
                        return ERROR_REG_EXP;
                    if (assInput.parameterType[2] != CHIP8_VX)
                        return ERROR_REG_EXP;
                    *b1 = assembleCommand_[STIV_COMMAND_4] | assInput.parameterValue[2];
                    *b2 = assInput.parameterValue[1];
                    return 2;
                }
                if (assembleCommand_[LD_RB_MMM] != -1)
                { // LD B, MMM
                    if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                        return ERROR_PAR;
                    if (assInput.parameterType[1] != ASS_HEX_VALUE)
                        return ERROR_12BIT;
                    if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xfff)
                        return ERROR_12BIT;
                    *b1 = assembleCommand_[LD_RB_MMM] | ((assInput.parameterValue[1] >> 8) & 0xf);
                    *b2 = assInput.parameterValue[1] & 0xff;
                    return 2;
                }
                return ERROR_INST;
            }
        }
        if (assInput.parameterType[0] == ASS_STRING)
        {
            if (assInput.parameterType[1] == ASS_HEX_VALUE && assInput.parameterString[0] == "I")
            {
                if (assembleCommand_[LD_I_MMMM] != -1)
                { // LD I, MMMM
                    if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                        return ERROR_PAR;
                    if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0x3fff)
                        return ERROR_INCORR_ADDRESS;
                    *b1 = (assInput.parameterValue[1] >> 8) & 0xff;
                    *b2 = assInput.parameterValue[1] & 0xff;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[0] == "I")
            {
				if (assInput.parameterString[1] == "[I]" && assInput.numberOfParameters == 2)
				{ // LD I, [I]
					*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
					*b2 = 0x9e;
					return 2;
				}
				if (assInput.parameterString[1] == "[I" && assInput.parameterType[2] == ASS_STRING && assInput.parameterString[2] == "V9]" &&  assInput.numberOfParameters == 3 && assInput.seperator[1] == "+")
				{ // LD I, [I+V9]
					*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
					*b2 = 0x9c;
					return 2;
				}
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_HEX_VALUE_MEM && assInput.parameterString[0] == "I")
            {
                if (assembleCommand_[STIV_COMMAND_6] != -1)
				{ // LD I, [27kk]
					if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
						return ERROR_PAR;
					if (assInput.parameterValue[1] < 0x2700 || assInput.parameterValue[1] > 0x27ff)
						return ERROR_STUDIOIV_ADDRESS_I;
					*b1 = assembleCommand_[STIV_COMMAND_6] | 3;
					*b2 = assInput.parameterValue[1];
					return 2;
				}
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == CHIP8_VX && assInput.parameterString[0] == "I")
            { 
                if (assInput.numberOfParameters > 3)
                    return ERROR_PAR;
                if (assembleCommand_[STIV_COMMAND_6] != -1)
				{ // LD I, Vy, Vy+1
					*b1 = assembleCommand_[STIV_COMMAND_6] | 3;
					*b2 = assInput.parameterValue[1] | 0xe0;
					return 2;
				}
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_HEX_VALUE && assInput.parameterString[0] == "RA")
            { 
                if (assembleCommand_[LD_RA_MMM] != -1)
                { // LD RA, MMM
                    if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                        return ERROR_PAR;
                    if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xfff)
                        return ERROR_12BIT;
                    *b1 = assembleCommand_[LD_RA_MMM] | ((assInput.parameterValue[1] >> 8) & 0xf);
                    *b2 = assInput.parameterValue[1] & 0xff;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_HEX_VALUE && assInput.parameterString[0] == "RB")
            {
                if (assembleCommand_[LD_RB_MMM] != -1)
                { // LD RB, MMM
                    if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                        return ERROR_PAR;
                    if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xfff)
                        return ERROR_12BIT;
                    *b1 = assembleCommand_[LD_RB_MMM] | ((assInput.parameterValue[1] >> 8) & 0xf);
                    *b2 = assInput.parameterValue[1] & 0xff;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == CHIP8_VX && assInput.parameterString[0] == "[RA]")
            { // LD [RA], Vx
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL3_COMMAND_C] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_C] | assInput.parameterValue[1];
                    *b2 = 0x3B;
                    return 2;
                }
                if (assembleCommand_[FEL1_COMMAND_7] != -1)
                {
                    *b1 = assembleCommand_[FEL1_COMMAND_7] | assInput.parameterValue[1];
                    *b2 = 0x24;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == CHIP8_VX && assInput.parameterString[0] == "[RB]")
            { // LD [RB], Vx
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL3_COMMAND_C] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_C] | assInput.parameterValue[1];
                    *b2 = 0x3E;
                    return 2;
                }
                if (assembleCommand_[FEL1_COMMAND_7] != -1)
                {
                    *b1 = assembleCommand_[FEL1_COMMAND_7] | assInput.parameterValue[1];
                    *b2 = 0x27;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == CHIP8_VX && assInput.parameterString[0] == "RB.0")
            { // LD RB.0, Vx
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL3_COMMAND_C] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_C] | assInput.parameterValue[1];
                    *b2 = 0x41;
                    return 2;
                }
                if (assembleCommand_[FEL1_COMMAND_7] != -1)
                {
                    *b1 = assembleCommand_[FEL1_COMMAND_7] | assInput.parameterValue[1];
                    *b2 = 0x34;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == CHIP8_VX && assInput.parameterString[0] == "RB.1")
            { // LD RB.1, Vx
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL3_COMMAND_C] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_C] | assInput.parameterValue[1];
                    *b2 = 0x44;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == CHIP8_VX && assInput.parameterString[0] == "RA.0")
            { // LD RA.0, Vx
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL3_COMMAND_C] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_C] | assInput.parameterValue[1];
                    *b2 = 0x4d;
                    return 2;
                }
                if (assembleCommand_[FEL1_COMMAND_7] != -1)
                {
                    *b1 = assembleCommand_[FEL1_COMMAND_7] | assInput.parameterValue[1];
                    *b2 = 0x2A;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == CHIP8_VX && assInput.parameterString[0] == "RA.1")
            { // LD RA.0, Vx
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL1_COMMAND_7] != -1)
                {
                    *b1 = assembleCommand_[FEL1_COMMAND_7] | assInput.parameterValue[1];
                    *b2 = 0x30;
                    return 2;
                }
                return ERROR_INST;
            }
        }
        if (assInput.parameterType[0] == CHIP8_VX_MEM)
		{
            if (assInput.parameterType[0] == CHIP8_VX_MEM && assInput.parameterType[1] == CHIP8_VX && assInput.seperator[1] == " ")
            { 
                if (assembleCommand_[STIV_COMMAND_5] != -1)
				{ // LD [Vx], Vy
					if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
						return ERROR_PAR;
					*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[1];
					*b2 = 0xE0 | assInput.parameterValue[0];
					return 2;
				}
				return ERROR_INST;
            }
		}
        if (assInput.parameterType[0] == CHIP8_VX)
        {
            if (assInput.parameterType[1] == ASS_HEX_VALUE)
            { 
                if (assembleCommand_[LD_VX_KK] != -1)
				{ // LD Vx, kk
					if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
						return ERROR_PAR;
					if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
						return ERROR_8BIT;
					*b1 = assembleCommand_[LD_VX_KK] | assInput.parameterValue[0];
					*b2 = assInput.parameterValue[1];
					return 2;
				}
                if (assembleCommand_[LD_VX_27KK] != -1)
				{ // LD Vx, 27kk
					if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
						return ERROR_PAR;
					if (assInput.parameterValue[1] < 0x2700 || assInput.parameterValue[1] > 0x27ff)
						return ERROR_STUDIOIV_ADDRESS_I;
					*b1 = assembleCommand_[LD_VX_27KK] | assInput.parameterValue[0];
					*b2 = assInput.parameterValue[1];
					return 2;
				}
				return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_HEX_VALUE_MEM)
            { 
                if (assembleCommand_[LD_VX_27KK] != -1)
				{ // LD Vx, [27kk]
					if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
						return ERROR_PAR;
					if (assInput.parameterValue[1] < 0x2700 || assInput.parameterValue[1] > 0x27ff)
						return ERROR_STUDIOIV_ADDRESS_I;
					*b1 = assembleCommand_[LD_VX_27KK] | assInput.parameterValue[0];
					*b2 = assInput.parameterValue[1];
					return 2;
				}
				return ERROR_INST;
            }
            if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.seperator[1] == " ")
            { 
                if (assembleCommand_[FEL3_COMMAND_4] != -1)
                { // LD Vx, Vy
                    *b1 = assembleCommand_[FEL3_COMMAND_4] | assInput.parameterValue[0];
                    *b2 = (assInput.parameterValue[1] << 4) + 1;
                    return 2;
                }
                if (assembleCommand_[LD_VX_27KK] != -1)
                { // LD Vx, Vy
                    *b1 = assembleCommand_[LD_VX_27KK] | assInput.parameterValue[0];
                    *b2 = assembleCommand_[LD_VX_27KK] | assInput.parameterValue[1];
                    return 2;
                }
			}
            if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE_MEM && assInput.seperator[1] == " ")
            { 
                if (assembleCommand_[LD_VX_27KK] != -1)
				{ // LD Vx, [27kk] (LD Vx, 27Ey)
					if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
						return ERROR_PAR;
					*b1 = assembleCommand_[LD_VX_27KK] | assInput.parameterValue[0];
					*b2 = 0xE0 | assInput.parameterValue[1];
					return 2;
				}
			}
            if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == ASS_STRING)
            { 
                if (assembleCommand_[STIV_COMMAND_6] != -1)
				{ // LD Vy, Vy+1, I
					if (assInput.numberOfParameters > 3)
						return ERROR_PAR;
					if (assInput.parameterString[2] != "I" && assInput.parameterString[1] != "I")
						return ERROR_REG;            
					*b1 = assembleCommand_[STIV_COMMAND_6] | 4;
					*b2 = assInput.parameterValue[0] | 0xe0;
					return 2;
				}
				return ERROR_INST;
            }
            if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX_MEM && assInput.seperator[1] == " ")
            { 
                if (assembleCommand_[STIV_COMMAND_5] != -1)
				{ // LD Vx, [Vy]
					if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
						return ERROR_PAR;
					*b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[0];
					*b2 = 0xF0 | assInput.parameterValue[1];
					return 2;
				}
				return ERROR_INST;
            }
            if (assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == CHIP8_VX && assInput.seperator[1] == "+")
            { // LD Vz, Vx+Vy
                if (assembleCommand_[ADD_VX_VY_VZ] == -1)
                    return ERROR_INST;
                if (assInput.seperator[2] != " ")
                    return ERROR_PAR;
                *b1 = assembleCommand_[ADD_VX_VY_VZ] | assInput.parameterValue[1];
                *b2 = ((assInput.parameterValue[2]<<4)&0xf0)|assInput.parameterValue[0];
                return 2;
            }
            if (assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == CHIP8_VX)
            { // LD Vz, Vx-Vy
                if (assembleCommand_[SUB_VX_VY_VZ] == -1)
                    return ERROR_INST;
                if (assInput.seperator[1] != "-")
                    return ERROR_DASH;
                if (assInput.seperator[2] != " ")
                    return ERROR_PAR;
                *b1 = assembleCommand_[SUB_VX_VY_VZ] | assInput.parameterValue[1];
                *b2 = ((assInput.parameterValue[2]<<4)&0xf0)|assInput.parameterValue[0];
                return 2;
            }
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "I")
            { 
                if (assembleCommand_[STIV_COMMAND_6] != -1)
				{ // LD Vy, I
					if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
						return ERROR_PAR;
					*b1 = assembleCommand_[STIV_COMMAND_6] | 4;
                    *b2 = assInput.parameterValue[0] | 0xe0;
					return 2;
				}
			}
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "[RA]")
            { // LD Vx, [RA]
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL3_COMMAND_C] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_C] | assInput.parameterValue[0];
                    *b2 = 0x35;
                    return 2;
                }
                if (assembleCommand_[FEL1_COMMAND_7] != -1)
                {
                    *b1 = assembleCommand_[FEL1_COMMAND_7] | assInput.parameterValue[0];
                    *b2 = 0x1E;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "[RB]")
            { // LD Vx, [RB]
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL3_COMMAND_C] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_C] | assInput.parameterValue[0];
                    *b2 = 0x38;
                    return 2;
                }
                if (assembleCommand_[FEL1_COMMAND_7] != -1)
                {
                    *b1 = assembleCommand_[FEL1_COMMAND_7] | assInput.parameterValue[0];
                    *b2 = 0x21;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "RB.0")
            { // LD Vx, RB.0
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL3_COMMAND_C] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_C] | assInput.parameterValue[0];
                    *b2 = 0x47;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "RB.1")
            { // LD Vx, RB.1
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL3_COMMAND_C] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_C] | assInput.parameterValue[0];
                    *b2 = 0x4A;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "RA.0")
            { // LD Vx, RA.0
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL3_COMMAND_C] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_C] | assInput.parameterValue[0];
                    *b2 = 0x56;
                    return 2;
                }
                if (assembleCommand_[FEL1_COMMAND_7] != -1)
                {
                    *b1 = assembleCommand_[FEL1_COMMAND_7] | assInput.parameterValue[0];
                    *b2 = 0x38;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "RA.1")
            { // LD Vx, RA.0
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL1_COMMAND_7] != -1)
                {
                    *b1 = assembleCommand_[FEL1_COMMAND_7] | assInput.parameterValue[0];
                    *b2 = 0x3B;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "PAR")
            { // LD Vx, PAR
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL2_COMMAND_C] != -1)
                {
                    *b1 = assembleCommand_[FEL2_COMMAND_C] | 0x8;
                    *b2 = (assInput.parameterValue[0] << 4) &0xf0;
                    return 2;
                }
                if (assembleCommand_[FEL3_COMMAND_6] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_6] | assInput.parameterValue[0];
                    *b2 = 0x7b;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "COIN")
            { // LD Vx, COIN
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL2_COMMAND_E] != -1)
                {
                    *b1 = assembleCommand_[FEL2_COMMAND_E] | assInput.parameterValue[0];
                    *b2 = 0;
                    return 2;
                }
                if (assembleCommand_[FEL3_COMMAND_6] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_6] | assInput.parameterValue[0];
                    *b2 = 0x5d;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "FIREA")
            { // LD Vx, COIN
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL2_COMMAND_E] != -1)
                {
                    *b1 = assembleCommand_[FEL2_COMMAND_E] | assInput.parameterValue[0];
                    *b2 = 0x1;
                    return 2;
                }
                if (assembleCommand_[FEL3_COMMAND_6] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_6] | assInput.parameterValue[0];
                    *b2 = 0x63;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "FIREB")
            { // LD Vx, COIN
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL2_COMMAND_E] != -1)
                {
                    *b1 = assembleCommand_[FEL2_COMMAND_E] | assInput.parameterValue[0];
                    *b2 = 0x2;
                    return 2;
                }
                if (assembleCommand_[FEL3_COMMAND_6] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_6] | assInput.parameterValue[0];
                    *b2 = 0x67;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "JOYA")
            { // LD Vx, COIN
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL2_COMMAND_E] != -1)
                {
                    *b1 = assembleCommand_[FEL2_COMMAND_E] | assInput.parameterValue[0];
                    *b2 = 0x4;
                    return 2;
                }
                if (assembleCommand_[FEL3_COMMAND_6] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_6] | assInput.parameterValue[0];
                    *b2 = 0x6b;
                    return 2;
                }
                return ERROR_INST;
            }
            if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "JOYB")
            { // LD Vx, COIN
                if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                    return ERROR_PAR;
                if (assembleCommand_[FEL2_COMMAND_E] != -1)
                {
                    *b1 = assembleCommand_[FEL2_COMMAND_E] | assInput.parameterValue[0];
                    *b2 = 0x8;
                    return 2;
                }
                if (assembleCommand_[FEL3_COMMAND_6] != -1)
                {
                    *b1 = assembleCommand_[FEL3_COMMAND_6] | assInput.parameterValue[0];
                    *b2 = 0x74;
                    return 2;
                }
                return ERROR_INST;
            }
        }
        if (assInput.parameterType[0] == ASS_HEX_VALUE_MEM && assInput.parameterType[1] == CHIP8_VX)
        { 
            if (assembleCommand_[LD_M8AA_VX] != -1)
			{ // LD [8aa], Vx
				if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
					return ERROR_PAR;
				if (assInput.parameterValue[0] < 0x800 || assInput.parameterValue[0] > 0x8ff)
					return ERROR_RAM_CHIP_ADDRESS;
				*b1 = assembleCommand_[LD_M8AA_VX] | assInput.parameterValue[1];
				*b2 = assInput.parameterValue[0] & 0xff;
				return 2;
			}
            if (assembleCommand_[LD_27KK_VX] != -1)
			{ // LD [27kk], Vx
				if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
					return ERROR_PAR;
				if (assInput.parameterValue[0] < 0x2700 || assInput.parameterValue[0] > 0x27ff)
					return ERROR_STUDIOIV_ADDRESS_I;
				*b1 = assembleCommand_[LD_27KK_VX] | assInput.parameterValue[1];
				*b2 = assInput.parameterValue[0] & 0xff;
				return 2;
			}
        }
        if (assInput.parameterType[0] == ASS_HEX_VALUE_MEM && assInput.parameterType[1] == ASS_STRING)
        {
			if (assembleCommand_[STIV_COMMAND_6] != -1)
			{ // LD [27kk], I
				if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
					return ERROR_PAR;
				if (assInput.parameterValue[0] < 0x2700 || assInput.parameterValue[0] > 0x27ff)
					return ERROR_STUDIOIV_ADDRESS_I;
				if (assInput.parameterString[1] != "I")
					return ERROR_REG;
				
				*b1 = assembleCommand_[STIV_COMMAND_6] | 4;
				*b2 = assInput.parameterValue[0];
				return 2;
			}
            return ERROR_INST;
        }
        if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.parameterType[1] == CHIP8_VX)
        { 
            if (assembleCommand_[LD_M8AA_VX] != -1)
			{ // LD 8aa, Vx
				if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
					return ERROR_PAR;
				if (assInput.parameterValue[0] < 0x800 || assInput.parameterValue[0] > 0x8ff)
					return ERROR_RAM_CHIP_ADDRESS;
				*b1 = assembleCommand_[LD_M8AA_VX] | assInput.parameterValue[1];
				*b2 = assInput.parameterValue[0] & 0xff;
				return 2;
			}
            if (assembleCommand_[LD_27KK_VX] != -1)
			{ // LD 27kk, Vx
				if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
					return ERROR_PAR;
				if (assInput.parameterValue[0] < 0x2700 || assInput.parameterValue[0] > 0x27ff)
					return ERROR_STUDIOIV_ADDRESS_I;
				*b1 = assembleCommand_[LD_27KK_VX] | assInput.parameterValue[1];
				*b2 = assInput.parameterValue[0] & 0xff;
				return 2;
			}
			if (assembleCommand_[STIV_COMMAND_6] != -1)
			{ // 27kk, I
				if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
					return ERROR_PAR;
				if (assInput.parameterValue[0] < 0x2700 || assInput.parameterValue[0] > 0x27ff)
					return ERROR_STUDIOIV_ADDRESS_I;
				if (assInput.parameterString[1] != "I")
					return ERROR_REG;
				
				*b1 = assembleCommand_[STIV_COMMAND_6] | 4;
				*b2 = assInput.parameterValue[1];
				return 2;
			}
			return ERROR_INST;
        }
        return ERROR_LD;
    }
    if (assInput.command == "NO")
    { // NO OP
        if (assembleCommand_[FEL2_COMMAND_C] != -1)
        {
            if (assInput.numberOfParameters > 1)
                return ERROR_PAR;
            
            if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "OP")
            {
                *b1 = assembleCommand_[FEL2_COMMAND_C] | 4;
                return 1;
            }
        }
        return ERROR_INST;

    }
    if (assInput.command == "OR")
    {
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.numberOfParameters == 2)
        { // OR Vx, Vy
            if (assembleCommand_[FEL3_COMMAND_8] != -1)
			{
				if (assInput.seperator[1] != " ")
					return ERROR_PAR;
				*b1 = assembleCommand_[FEL3_COMMAND_8]  | assInput.parameterValue[0];
				*b2 = (assInput.parameterValue[1] << 4) | 1;
				return 2;
			}
            if (assembleCommand_[STIV_COMMAND_4] != -1)
			{
				if (assInput.seperator[1] != " ")
					return ERROR_PAR;
				*b1 = assembleCommand_[STIV_COMMAND_4]  | assInput.parameterValue[0];
				*b2 = assInput.parameterValue[1] | 0x10;
				return 2;
			}
            return ERROR_INST;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "OUT4")
    { 
        if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 
			if (assInput.numberOfParameters > 1)
				return ERROR_PAR;
			if (assInput.parameterType[0] == CHIP8_VX)
			{ // OUT4 Vy
				*b1 = assembleCommand_[STIV_COMMAND_6] | 0xE;
				*b2 = assInput.parameterValue[0];
				return 2;
			}
			if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.parameterValue[0] <= 0xff)
			{ // OUT4 KK
				*b1 = assembleCommand_[STIV_COMMAND_6] | 0xF;
				*b2 = assInput.parameterValue[0] & 0xff;
				return 2;
			}
			return ERROR_8BIT;
		}
        return ERROR_INST;
    }
    if (assInput.command == "POP")
    { 
		if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I")
			{ // POP I
				if (assInput.numberOfParameters > 1)
					return ERROR_PAR;
				*b1 = assembleCommand_[STIV_COMMAND_6] | 0xC;
				*b2 = 0;
				return 2;
			}
			if (assInput.numberOfParameters == 0)
			{ // POP
				*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
				*b2 = 0xa;
				return 2;
			}
			if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.parameterValue[0] == 0 && assInput.parameterValue[1] == 9)
			{ // POP V0-V9
                if (assInput.seperator[0] != "-")
                    return ERROR_DASH;
				if (assInput.numberOfParameters > 2)
					return ERROR_PAR;
				*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
				*b2 = 0xa;
				return 2;
			}
			return ERROR_REG;
		}
		return ERROR_INST;
    }
    if (assInput.command == "PRINT")
    { 
        if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 
 		    if (assInput.numberOfParameters > 2)
				return ERROR_PAR;
			
			if (assInput.numberOfParameters == 0)
			{ // PRINT
				*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
				*b2 = 0x48;
				return 2;
			}
            if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "[I]" && assInput.numberOfParameters == 1)
            { // PRINT [I]
                *b1 = assembleCommand_[STIV_COMMAND_6] | 6;
                *b2 = 0x56;
                return 2;
            }
            if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I" && assInput.numberOfParameters == 1)
            { // PRINT I
                *b1 = assembleCommand_[STIV_COMMAND_6] | 6;
                *b2 = 0x56;
                return 2;
            }
			if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.parameterValue[0] == 0xd &&
			    assInput.parameterType[1] == ASS_HEX_VALUE && assInput.parameterValue[1] == 3 && assInput.numberOfParameters == 2) 
			{ // PRINT D, 3
				*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
				*b2 = 0x60;
				return 2;
			}
			if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.parameterValue[0] == 0xd &&
			    assInput.parameterType[1] == ASS_HEX_VALUE && assInput.parameterValue[1] == 2 && assInput.numberOfParameters == 2) 
			{ // PRINT D, 2
				*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
				*b2 = 0x7c;
				return 2;
			}
			if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.parameterValue[0] == 0xd &&
			    assInput.parameterType[1] == ASS_HEX_VALUE && assInput.parameterValue[1] == 1 && assInput.numberOfParameters == 2) 
			{ // PRINT D, 1
				*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
				*b2 = 0x82;
				return 2;
			}
			return ERROR_INCORRECT_PAR;
		}
        return ERROR_INST;
    }
    if (assInput.command == "PUSH")
    { 
		if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 			
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I")
			{ // PUSH I
				if (assInput.numberOfParameters > 1)
					return ERROR_PAR;

				*b1 = assembleCommand_[STIV_COMMAND_6] | 0xA;
				*b2 = 0;
				return 2;
			}
			if (assInput.numberOfParameters == 0)
			{ // PUSH
				*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
				*b2 = 0;
				return 2;
			}
			if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.parameterValue[0] == 0 && assInput.parameterValue[1] == 9)
			{ // PUSH V0-V9
                if (assInput.seperator[0] != "-")
                    return ERROR_DASH;
				if (assInput.numberOfParameters > 2)
					return ERROR_PAR;
				*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
				*b2 = 0;
				return 2;
			}
			return ERROR_REG;
		}
		return ERROR_INST;
    }
    if (assInput.command == "RESET")
    { 
		if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 
			if (assInput.numberOfParameters > 1)
				return ERROR_PAR;
			
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "RAM")
			{ // RESET RAM
				*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
				*b2 = 0;
				return 2;
			}
            return ERROR_INCORRECT_PAR;
		}
        return ERROR_INST;
    }
    if (assInput.command == "RETURN")
    { // RET
        if (assembleCommand_[FEL2_COMMAND_C] != -1)
        {
            if (assInput.numberOfParameters > 0)
                return ERROR_PAR;
            
            *b1 = assembleCommand_[FEL2_COMMAND_C];
            return 1;
        }
		if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ // RET
			if (assInput.numberOfParameters > 0)
				return ERROR_PAR;
			
			*b1 = assembleCommand_[STIV_COMMAND_6] | 0xB;
			*b2 = 0;
			return 2;
		}
        return ERROR_INST;
    }
    if (assInput.command == "RND")
    { 
		if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 
			if (assInput.numberOfParameters > 3)
				return ERROR_PAR;
			
			if (assInput.parameterType[0] == ASS_HEX_VALUE_MEM &&
			    assInput.parameterType[1] == CHIP8_VX &&
				assInput.parameterType[2] == CHIP8_VX)
			{ // RND [270B], V8, V9
                if (assInput.parameterValue[0] != 0x270B)
                    return ERROR_INCORR_ADDRESS;
                if (assInput.parameterValue[1] != 8 || assInput.parameterValue[2] != 9)
                    return ERROR_INCORRECT_REG;
                if (assInput.seperator[0] != ",")
                    return ERROR_COMMA;
 				*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
				*b2 = 0xbc;
				return 2;
			}
			if (assInput.numberOfParameters > 2)
				return ERROR_PAR;
			if (assInput.parameterType[0] == CHIP8_VX &&
				assInput.parameterType[1] == CHIP8_VX)
			{ // RND V8, V9
                if (assInput.parameterValue[0] != 8 || assInput.parameterValue[1] != 9)
                    return ERROR_INCORRECT_REG;
                if (assInput.seperator[0] != ",")
                    return ERROR_COMMA;
				*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
				*b2 = 0xbc;
				return 2;
			}
            if (assInput.parameterType[0] == ASS_HEX_VALUE_MEM &&
			    assInput.parameterType[1] == CHIP8_VX)
			{ // RND [270B], V9
                if (assInput.parameterValue[1] != 9)
                    return ERROR_INCORRECT_REG;
                if (assInput.parameterValue[0] != 0x270B)
                    return ERROR_INCORR_ADDRESS;
                if (assInput.seperator[0] != ",")
                    return ERROR_COMMA;
				*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
                *b2 = 0xb6;
				return 2;
			}
			if (assInput.parameterType[0] == CHIP8_VX && assInput.numberOfParameters == 1)
			{ // RND V9
                if (assInput.parameterValue[0] != 9)
                    return ERROR_INCORRECT_REG;
				*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
				*b2 = 0xb6;
				return 2;
			}
		}
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assembleCommand_[RND_VX_KK] == -1)
            return ERROR_INST;
        if (assInput.parameterType[0] != CHIP8_VX)
            return ERROR_REG_EXP;
        if (assInput.parameterType[1] != ASS_HEX_VALUE)
            return ERROR_8BIT;
        if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
            return ERROR_PAR;
        
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
        { // RND Vx, kk
            if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
                return ERROR_8BIT;
            
            *b1 = assembleCommand_[RND_VX_KK] | assInput.parameterValue[0];
            *b2 = assInput.parameterValue[1];
            return 2;
        }
        return ERROR_8REG;
    }
    if (assInput.command == "SCR")
    { 
		if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 
			if (assInput.numberOfParameters > 1)
				return ERROR_PAR;
			
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "CLS")
			{ // SCR CLS
				*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
				*b2 = 0x12;
				return 2;
			}
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "FILL")
			{ // SCR FILL
				*b1 = assembleCommand_[STIV_COMMAND_6] | 6;
				*b2 = 0x26;
				return 2;
			}
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "XOR")
			{ // SCR XOR
				*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
				*b2 = 0xb;
				return 2;
			}
		}
        return ERROR_INCORRECT_PAR;
    }
    if (assInput.command == "SE")
    {
        if (assInput.parameterType[0] != CHIP8_VX)
            return ERROR_REG_EXP;
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
            return ERROR_PAR;
        
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
        { // SE Vx, kk
            if (assInput.parameterType[1] != ASS_HEX_VALUE_MEM && assInput.parameterType[1] != ASS_HEX_VALUE)
                return ERROR_8REG;
            if (assInput.parameterValue[1] > 0 && assInput.parameterValue[1] <= 0xff)
            {
                if (assembleCommand_[SE_VX_KK] == -1)
                    return ERROR_INST;
                *b1 = assembleCommand_[SE_VX_KK] | assInput.parameterValue[0];
                *b2 = assInput.parameterValue[1];
            }
            return 2;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "SHL")
    {
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE && assInput.numberOfParameters == 2)
        { // SHL Vx, n
            if (assembleCommand_[STIV_COMMAND_4] == -1)
                return ERROR_INST;
            if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xf)
                return ERROR_4BIT;
            if (assInput.seperator[1] != " ")
                return ERROR_PAR;
            *b1 = assembleCommand_[STIV_COMMAND_4]  | assInput.parameterValue[0];
            *b2 = assInput.parameterValue[1] | 0x60;
            return 2;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "SHL4")
    {
        if (assInput.parameterType[0] == CHIP8_VX && assInput.numberOfParameters == 1)
        { // SHL4 Vx
            if (assembleCommand_[FEL1_COMMAND_7] == -1)
                return ERROR_INST;
            if (assInput.seperator[0] != " ")
                return ERROR_PAR;
            *b1 = assembleCommand_[FEL1_COMMAND_7]  | assInput.parameterValue[0];
            *b2 = 0x3E;
            return 2;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "SHR")
    {
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.numberOfParameters == 2)
        { // SHR Vx, Vy
            if (assembleCommand_[FEL3_COMMAND_8] != -1)
			{
				if (assInput.seperator[1] != " ")
					return ERROR_PAR;
				*b1 = assembleCommand_[FEL3_COMMAND_8]  | assInput.parameterValue[0];
				*b2 = (assInput.parameterValue[1] << 4) | 6;
				return 2;
			}
            if (assembleCommand_[STIV_COMMAND_4] != -1)
			{
				if (assInput.seperator[1] != " ")
					return ERROR_PAR;
				*b1 = assembleCommand_[STIV_COMMAND_4]  | assInput.parameterValue[0];
				*b2 = assInput.parameterValue[1] | 0xc0;
				return 2;
			}
            return ERROR_INST;
        }
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE && assInput.numberOfParameters == 2)
        { // SHR Vx, n
            if (assembleCommand_[STIV_COMMAND_4] == -1)
                return ERROR_INST;
            if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xf)
                return ERROR_4BIT;
            if (assInput.seperator[1] != " ")
                return ERROR_PAR;
            *b1 = assembleCommand_[STIV_COMMAND_4]  | assInput.parameterValue[0];
            *b2 = assInput.parameterValue[1] | 0x90;
            return 2;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "SHR4")
    {
        if (assInput.parameterType[0] == CHIP8_VX && assInput.numberOfParameters == 1)
        { // SHR4 Vx
            if (assembleCommand_[FEL1_COMMAND_7] == -1)
                return ERROR_INST;
            if (assInput.seperator[0] != " ")
                return ERROR_PAR;
            *b1 = assembleCommand_[FEL1_COMMAND_7]  | assInput.parameterValue[0];
            *b2 = 0x41;
            return 2;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "SNE")
    {
        if (assInput.parameterType[0] != CHIP8_VX)
            return ERROR_REG_EXP;
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
            return ERROR_PAR;
        
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE)
        { // SNE Vx, kk
            if (assInput.parameterValue[1] > 0 && assInput.parameterValue[1] <= 0xff)
            {
                if (assembleCommand_[SNE_VX_KK] == -1)
                    return ERROR_INST;
                *b1 = assembleCommand_[SNE_VX_KK] | assInput.parameterValue[0];
                *b2 = assInput.parameterValue[1];
            }
            else
            {// SNE Vx, 8aa
                if (assembleCommand_[SNE_VX_M8AA] == -1)
                    return ERROR_INST;
                if (assInput.parameterValue[1] < 0x800 || assInput.parameterValue[1] > 0x8ff)
                    return ERROR_RAM_CHIP_ADDRESS;
                *b1 = assembleCommand_[SNE_VX_M8AA] | assInput.parameterValue[0];
                *b2 = assInput.parameterValue[1] & 0xf0;
            }
            return 2;
        }
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == ASS_HEX_VALUE_MEM)
        { // SNE Vx, [8aa]
            if (assembleCommand_[SNE_VX_M8AA] == -1)
                return ERROR_INST;
            if (assInput.parameterValue[1] < 0x800 || assInput.parameterValue[1] > 0x8ff)
                return ERROR_RAM_CHIP_ADDRESS;
            *b1 = assembleCommand_[SNE_VX_M8AA] | assInput.parameterValue[0];
            *b2 = assInput.parameterValue[1] & 0xf0;
            return 2;
        }
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX)
        { // SNE Vx, Vy
            if (assembleCommand_[FEL3_COMMAND_4] != -1)
            {
                *b1 = assembleCommand_[FEL3_COMMAND_4] | assInput.parameterValue[0];
                *b2 = assInput.parameterValue[1] << 4;
                return 2;
            }
            if (assembleCommand_[SNE_VX_VY] != -1)
            {
                *b1 = assembleCommand_[SNE_VX_VY] | assInput.parameterValue[0];
                *b2 = assInput.parameterValue[1] << 4;
                return 2;
            }
            return ERROR_INST;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "STOP")
    { // STOP
        if (assembleCommand_[STIV_COMMAND_4] != -1)
        {
            if (assInput.numberOfParameters > 0)
                return ERROR_PAR;
            
            *b1 = assembleCommand_[STIV_COMMAND_4];
            *b2 = 0xd0;
            return 2;
        }
        return ERROR_INST;
    }
    if (assInput.command == "SUB")
    {
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
		if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 
			if (assInput.numberOfParameters > 2)
				return ERROR_PAR;
			
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "[V0V1]" && 
			    assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "[V2V3]")
			{ // SUB [V0V1], [V2V3]
				*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
				*b2 = 0x5a;
				return 2;
			}
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "V0V1" && 
			    assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "V2V3")
			{ // SUB V0V1, V2V3
				*b1 = assembleCommand_[STIV_COMMAND_6] | 7;
				*b2 = 0x5a;
				return 2;
			}
		}
        if (assInput.parameterType[0] != CHIP8_VX)
            return ERROR_REG_EXP;
        if (assInput.parameterType[1] != CHIP8_VX)
            return ERROR_REG_EXP;
        
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.numberOfParameters == 2)
        {
            if (assembleCommand_[SUB_VX_VY_VZ] != -1)
            {// SUB Vx, Vy
                if (assInput.seperator[1] != " ")
                    return ERROR_PAR;
                *b1 = assembleCommand_[SUB_VX_VY_VZ] | assInput.parameterValue[0];
                *b2 = (assInput.parameterValue[1] << 4) | assInput.parameterValue[0];
                return 2;
            }
            if (assembleCommand_[FEL3_COMMAND_8] != -1)
            {// SUB Vy, Vx
                if (assInput.seperator[1] != " ")
                    return ERROR_PAR;
                *b1 = assembleCommand_[FEL3_COMMAND_8]  | assInput.parameterValue[0];
                *b2 = (assInput.parameterValue[1] << 4) | 5;
                return 2;
            }
            if (assembleCommand_[STIV_COMMAND_4] != -1)
            {// SUB Vx, Vy
                if (assInput.seperator[1] != " ")
                    return ERROR_PAR;
                *b1 = assembleCommand_[STIV_COMMAND_4]  | assInput.parameterValue[0];
                *b2 = assInput.parameterValue[1] | 0x50;
                return 2;
            }
            return ERROR_INST;
        }
        if (assInput.parameterType[1] == CHIP8_VX && assInput.parameterType[2] == CHIP8_VX && assInput.seperator[1] == "-")
        { // SUB Vz, Vx-Vy
            if (assembleCommand_[SUB_VX_VY_VZ] == -1)
                return ERROR_INST;
            if (assInput.seperator[2] != " " || assInput.numberOfParameters > 3)
                return ERROR_PAR;
            *b1 = assembleCommand_[SUB_VX_VY_VZ] | assInput.parameterValue[1];
            *b2 = ((assInput.parameterValue[2]<<4)&0xf0)|assInput.parameterValue[0];
            return 2;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "SUBN")
    {
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.parameterType[0] != CHIP8_VX)
            return ERROR_REG_EXP;
        if (assInput.parameterType[1] != CHIP8_VX)
            return ERROR_REG_EXP;
        
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.numberOfParameters == 2)
        {
            if (assembleCommand_[FEL3_COMMAND_8] != -1)
            {// SUBN Vy, Vx
                if (assInput.seperator[1] != " ")
                    return ERROR_PAR;
                *b1 = assembleCommand_[FEL3_COMMAND_8]  | assInput.parameterValue[0];
                *b2 = (assInput.parameterValue[1] << 4) | 7;
                return 2;
            }
            return ERROR_INST;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "SWITCH")
    { // SWITCH Vx, Vy, [I]
        if (assembleCommand_[STIV_COMMAND_5] == -1)
            return ERROR_INST;
        if (assInput.parameterType[0] != CHIP8_VX)
            return ERROR_REG_EXP;
        if (assInput.parameterType[1] != CHIP8_VX)
            return ERROR_REG_EXP;
        if (assInput.parameterType[2] != ASS_STRING && (assInput.parameterString[2] == "I" || assInput.parameterString[2] == "[I]"))
            return ERROR_REG_EXP;
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.seperator[2] != " " || assInput.numberOfParameters > 3)
            return ERROR_PAR;
        
        *b1 = assembleCommand_[STIV_COMMAND_5] | assInput.parameterValue[0];
        *b2 = assInput.parameterValue[1] | 0x10;
        return 2;
    }
    if (assInput.command == "SYS")
    { 
		if (assembleCommand_[SYS_MMM] != -1)
		{ // SYS aaa
			if (assInput.numberOfParameters > 1)
				return ERROR_PAR;
			
			if (assInput.parameterType[0] == ASS_HEX_VALUE)
			{
				if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xfff)
					return ERROR_12BIT;
				*b1 = assembleCommand_[SYS_MMM] | ((assInput.parameterValue[0] >> 8) & 0xf);
				*b2 = assInput.parameterValue[0] & 0xff;
				return 2;
			}
			return ERROR_12BIT;
		}
		if (assembleCommand_[STIV_COMMAND_5] != -1)
		{ // SYS I
			if (assInput.numberOfParameters > 1)
				return ERROR_PAR;
			
			if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "I")
			{
				*b1 = assembleCommand_[STIV_COMMAND_5];
				*b2 = 0;
				return 2;
			}
			return ERROR_REG;
		}
		return ERROR_INST;
    }
    if (assInput.command == "SYS1" && assembleCommand_[SYS1_AA] != -1)
    { // SYS1 aa
        if (assInput.numberOfParameters > 1)
            return ERROR_PAR;
        
        if (assInput.parameterType[0] == ASS_HEX_VALUE)
        {
            if (assembleCommand_[SYS1_AA] == -1)
                return ERROR_INST;
            if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xff)
                return ERROR_12BIT;
            *b1 = assembleCommand_[SYS1_AA] | 1;
            *b2 = assInput.parameterValue[0];
            return 2;
        }
        return ERROR_12BIT;
    }
    if (assInput.command == "TAPE")
    {
        if (assembleCommand_[TAPE_KK] == -1)
            return ERROR_INST;
        if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.numberOfParameters == 1)
        { // TAPE kk
            if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xff)
                return ERROR_8BIT;
            *b1 = assembleCommand_[TAPE_KK];
            *b2 = assInput.parameterValue[0];
            return 2;
        }
        return ERROR_SYNTAX;
    }
    if (assInput.command == "TONE")
    {
        if (assembleCommand_[FEL2_COMMAND_C] != -1)
        {
            if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.numberOfParameters == 1)
            { // TONE kk
                if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xff)
                    return ERROR_8BIT;
                *b1 = assembleCommand_[FEL2_COMMAND_C] | 1;
                *b2 = assInput.parameterValue[0];
                return 2;
            }
            if (assInput.parameterType[0] == ASS_STRING && assInput.numberOfParameters == 1 && assInput.parameterString[0] == "OFF")
            { // TONE OFF
                *b1 = assembleCommand_[FEL2_COMMAND_C] | 2;
                *b2 = 0;
                return 2;
            }
            if (assInput.seperator[1] != " " || assInput.numberOfParameters > 2)
                return ERROR_PAR;
            if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.parameterValue[0]== 0xf && assInput.parameterType[1] == ASS_HEX_VALUE)
            { // TONE F=kk
                if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xff)
                    return ERROR_8BIT;
                *b1 = assembleCommand_[FEL2_COMMAND_C] | 1;
                *b2 = assInput.parameterValue[1];
                return 2;
            }
        }
        if (assembleCommand_[TONE_VX_VY] != -1)
        {
            if (assInput.seperator[0] != ",")
                return ERROR_COMMA;
            if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.numberOfParameters == 2)
            { // TONE Vx, Vy
                if (assInput.seperator[1] != " ")
                    return ERROR_PAR;
                *b1 = assembleCommand_[TONE_VX_VY]  | assInput.parameterValue[0];
                *b2 = (assInput.parameterValue[1] << 4);
                return 2;
            }
        }
        return ERROR_INST;
    }
    if (assInput.command == "WAIT")
    { 
        if (assembleCommand_[STIV_COMMAND_6] != -1)
		{ 
			if (assInput.parameterType[0] == ASS_STRING)
			{ // WAIT I, KK
				if (assInput.numberOfParameters > 2)
					return ERROR_PAR;
				if ((assInput.parameterString[0] == "I" || assInput.parameterString[0] == "[I]") && assInput.parameterType[1] == ASS_HEX_VALUE)
				{
					*b1 = assembleCommand_[STIV_COMMAND_6] | 0xD;
					*b2 = assInput.parameterValue[0] & 0xff;
					return 2;
				}
				return ERROR_REG;
			}
			return ERROR_8BIT;
		}
        return ERROR_INST;
    }
    if (assInput.command == "XOR")
    {
        if (assInput.seperator[0] != ",")
            return ERROR_COMMA;
        if (assInput.parameterType[0] == CHIP8_VX && assInput.parameterType[1] == CHIP8_VX && assInput.numberOfParameters == 2)
        { // XOR Vx, Vy
            if (assembleCommand_[FEL3_COMMAND_8] != -1)
			{
				if (assInput.seperator[1] != " ")
					return ERROR_PAR;
				*b1 = assembleCommand_[FEL3_COMMAND_8]  | assInput.parameterValue[0];
				*b2 = (assInput.parameterValue[1] << 4) | 3;
				return 2;
			}
            if (assembleCommand_[STIV_COMMAND_4] != -1)
			{
				if (assInput.seperator[1] != " ")
					return ERROR_PAR;
				*b1 = assembleCommand_[STIV_COMMAND_4]  | assInput.parameterValue[0];
				*b2 = assInput.parameterValue[1] | 0x30;
				return 2;
			}
            return ERROR_INST;
        }
        return ERROR_SYNTAX;
    }
    return ERROR_INST;
}

int DebugWindow::assemble(wxString *buffer, Byte* b1, Byte* b2, Byte* b3, Byte* b4, Byte* b5, Byte* b6, Byte* b7, bool allowX)
{
	wxString command;
	long registerNumber;
	int ret;

	AssInput assInput = getAssInput(*buffer);
	if (assInput.errorCode != 0)  return assInput.errorCode;
	if (assInput.commandSeperator != " ") return ERROR_COMMAND_SEP;

	*b7 = 0xff;

	if (assInput.command == ":")
	{ 
		if (assInput.parameterType[0] == ASS_SLOT)
		{
			ret = getSlot(assInput, b3);
			if (ret > ERROR_START)  return ret;

			*b1 = (assInput.parameterValue[1]&0xff00)>>8; 
			*b2 = assInput.parameterValue[1]&0xff; 
			if (assInput.parameterType[1] == ASS_HEX_VALUE && assInput.numberOfParameters <= 2)
				return MEM_TYPE_OPCODE_JUMP_SLOT;

			return ERROR_SYNTAX; 
		}
		else
		{
			*b1 = (assInput.parameterValue[0]&0xff00)>>8; 
			*b2 = assInput.parameterValue[0]&0xff; 
			if (assInput.parameterType[0] == ASS_HEX_VALUE && assInput.numberOfParameters <= 1)
				return 21;

			return ERROR_SYNTAX; 
		}
	}
	if (assInput.command == "ADC") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x74; 
		return 1; 
	}
	if (assInput.command == "ADCI")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        *b1 = 0x7c;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "ADD") 
	{
		if (assInput.numberOfParameters > 0)
			return ERROR_TEMP_PAR;
		*b1 = 0xf4; 
		return 1;
	}
	if (assInput.command == "ADI")
	{
		*b1 = 0xfc;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "AND") 
	{ 
		if (assInput.numberOfParameters > 0)
			return ERROR_TEMP_PAR;
		*b1 = 0xf2; 
		return 1; 
	}
	if (assInput.command == "ANI")
	{
		*b1 = 0xfa;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "B1")
	{
		*b1 = 0x34;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "B2")
	{
		*b1 = 0x35;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "B3")
	{
		*b1 = 0x36;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "B4")
	{
		*b1 = 0x37;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "BDF")
	{
		*b1 = 0x33;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "BPZ")
	{
		*b1 = 0x33;
		getByte(assInput, b2, allowX);
		return MEM_TYPE_OPCODE_BPZ;
	}
	if (assInput.command == "BGE")
	{
		*b1 = 0x33;
		getByte(assInput, b2, allowX);
		return MEM_TYPE_OPCODE_BGE;
	}
	if (assInput.command == "BN1")
	{
		*b1 = 0x3c;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "BN2")
	{
		*b1 = 0x3d;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "BN3")
	{
		*b1 = 0x3e;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "BN4")
	{
		*b1 = 0x3f;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "BNF")
	{
		*b1 = 0x3b;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "BM")
	{
		*b1 = 0x3b;
		getByte(assInput, b2, allowX);
		return MEM_TYPE_OPCODE_BM;
	}
	if (assInput.command == "BL")
	{
		*b1 = 0x3b;
		getByte(assInput, b2, allowX);
		return MEM_TYPE_OPCODE_BL;
	}
	if (assInput.command == "BNQ")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        *b1 = 0x39;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "BNZ")
	{
		*b1 = 0x3a;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "BQ")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        *b1 = 0x31;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "BR")
	{
		*b1 = 0x30;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "BZ")
	{
		*b1 = 0x32;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "DEC")
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		*b1 = 0x20 | registerNumber;
		return ret;
	}
	if (assInput.command == "DIS") 
	{ 
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x71; 
		return 1; 
	}
	if (assInput.command == "GHI")
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		*b1 = 0x90 | registerNumber;
		return ret;
	}
	if (assInput.command == "GLO")
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		*b1 = 0x80 | registerNumber;
		return ret;
	}
	if (assInput.command == "IDL") 
	{ 
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x00; 
		return 1; 
	}
	if (assInput.command == "INC")
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		*b1 = 0x10 | registerNumber;
		return ret;
	}
	if (assInput.command == "INP")
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

        int lowestInpValue = 1;
        if (cpuType_ == CPU1801)
            lowestInpValue = 0;

        if (assInput.parameterType[0] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[0] < lowestInpValue || assInput.parameterValue[0] > 7)
				return ERROR_IO;

			*b1 = 0x60 | ((assInput.parameterValue[0] & 0x7)+8); 
			return 1;
		}
		if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "X" && allowX)
		{
			*b1 = 0x68 | lowestInpValue;
			*b7 = 0xf0;
			return 1;
		}
		return ERROR_IO;
	}
	if (assInput.command == "IRX") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x60; 
		return 1; 
	}
	if (assInput.command == "LBDF")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.parameterType[0] == ASS_SLOT)
		{
			*b1 = 0xc3;
			ret = getSlot(assInput, b2);
			if (ret > ERROR_START)  return ret;

			ret = getWordPar2(assInput, b3, b4, allowX);
			if (ret > ERROR_START)  return ret;

			return MEM_TYPE_OPCODE_LBR_SLOT;
		}
		else
		{
			*b1 = 0xc3;
			return getWord(assInput, b2, b3, allowX);
		}
	}
	if (assInput.command == "LBNF")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.parameterType[0] == ASS_SLOT)
		{
			*b1 = 0xcb;
			ret = getSlot(assInput, b2);
			if (ret > ERROR_START)  return ret;

			ret = getWordPar2(assInput, b3, b4, allowX);
			if (ret > ERROR_START)  return ret;

			return MEM_TYPE_OPCODE_LBR_SLOT;
		}
		else
		{
			*b1 = 0xcb;
			return getWord(assInput, b2, b3, allowX);
		}
	}
	if (assInput.command == "LBNQ")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.parameterType[0] == ASS_SLOT)
		{
			*b1 = 0xc9;
			ret = getSlot(assInput, b2);
			if (ret > ERROR_START)  return ret;

			ret = getWordPar2(assInput, b3, b4, allowX);
			if (ret > ERROR_START)  return ret;

			return MEM_TYPE_OPCODE_LBR_SLOT;
		}
		else
		{
			*b1 = 0xc9;
			return getWord(assInput, b2, b3, allowX);
		}
	}
	if (assInput.command == "LBNZ")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.parameterType[0] == ASS_SLOT)
		{
			*b1 = 0xca;
			ret = getSlot(assInput, b2);
			if (ret > ERROR_START)  return ret;

			ret = getWordPar2(assInput, b3, b4, allowX);
			if (ret > ERROR_START)  return ret;

			return MEM_TYPE_OPCODE_LBR_SLOT;
		}
		else
		{
			*b1 = 0xca;
			return getWord(assInput, b2, b3, allowX);
		}
	}
	if (assInput.command == "LBQ")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.parameterType[0] == ASS_SLOT)
		{
			*b1 = 0xc1;
			ret = getSlot(assInput, b2);
			if (ret > ERROR_START)  return ret;

			ret = getWordPar2(assInput, b3, b4, allowX);
			if (ret > ERROR_START)  return ret;

			return MEM_TYPE_OPCODE_LBR_SLOT;
		}
		else
		{
			*b1 = 0xc1;
			return getWord(assInput, b2, b3, allowX);
		}
	}
	if (assInput.command == "LBR")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.parameterType[0] == ASS_SLOT)
		{
			*b1 = 0xc0;
			ret = getSlot(assInput, b2);
			if (ret > ERROR_START)  return ret;

			ret = getWordPar2(assInput, b3, b4, allowX);
			if (ret > ERROR_START)  return ret;

			return MEM_TYPE_OPCODE_LBR_SLOT;
		}
		else
		{
			*b1 = 0xc0;
			return getWord(assInput, b2, b3, allowX);
		}
	}
	if (assInput.command == "LBZ")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.parameterType[0] == ASS_SLOT)
		{
			*b1 = 0xc2;
			ret = getSlot(assInput, b2);
			if (ret > ERROR_START)  return ret;

			ret = getWordPar2(assInput, b3, b4, allowX);
			if (ret > ERROR_START)  return ret;

			return MEM_TYPE_OPCODE_LBR_SLOT;
		}
		else
		{
			*b1 = 0xc2;
			return getWord(assInput, b2, b3, allowX);
		}
	}
	if (assInput.command == "LDV")
	{
		if (assInput.numberOfParameters != 2)
			return ERROR_REGAND16;

		ret = getRegisterNumber(assInput, &registerNumber, b7, allowX);
		if (ret != 1)
			return ret;

		ret = getWordPar2(assInput, b2, b5, allowX);

		*b1 = 0xF8;
		*b3 = 0xB0 | registerNumber;
		*b4 = 0xF8;
		*b6 = 0xA0 | registerNumber;
		return MEM_TYPE_OPCODE_LDV;
	}
	if (assInput.command == "LDL")
	{
		if (assInput.parameterType[0] == ASS_SLOT)
		{
			if (assInput.numberOfParameters != 3)
				return ERROR_REGAND16;

			ret = getSlot(assInput, b4);
			if (ret > ERROR_START)  return ret;

			assInput.parameterValue[0] = assInput.parameterValue[1];
			assInput.parameterType[0] = assInput.parameterType[1];

			assInput.parameterValue[1] = assInput.parameterValue[2];
			assInput.parameterType[1] = assInput.parameterType[2];

			assInput.numberOfParameters--;

			ret = getRegisterNumber(assInput, &registerNumber, b7, allowX);
			if (ret != 1)
				return ret;

			ret = getWordPar2(assInput, b2, b5, allowX);
			if (ret != 4)
				return ret;

			*b1 = 0xF8;
			*b3 = 0xB0 | registerNumber;
			*b6 = 0xA0 | registerNumber;
			return MEM_TYPE_OPCODE_LDL_SLOT;
		}
		else
		{
			if (assInput.numberOfParameters != 2)
				return ERROR_REGAND16;

			ret = getRegisterNumber(assInput, &registerNumber, b7, allowX);
			if (ret != 1)
				return ret;

			ret = getWordPar2(assInput, b2, b5, allowX);
			if (ret != 4)
				return ret;

			*b1 = 0xF8;
			*b3 = 0xB0 | registerNumber;
			*b4 = 0xF8;
			*b6 = 0xA0 | registerNumber;
			return MEM_TYPE_OPCODE_LDL;
		}
	}
	if (assInput.command == "LDA")
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		*b1 = 0x40 | registerNumber;
		return ret;
	}
	if (assInput.command == "LDI")
	{
		*b1 = 0xf8;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "LDN")
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		*b1 = 0x00 | registerNumber;
		return ret;
	}
	if (assInput.command == "LDX") 
	{ 
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0xf0; 
		return 1; 
	}
	if (assInput.command == "LDXA") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x72; 
		return 1; 
	}
	if (assInput.command == "LSDF") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0xcf; 
		return 1; 
	}
	if (assInput.command == "LSIE") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0xcc; 
		return 1; 
	}
	if (assInput.command == "LSKP") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0xc8; 
		return MEM_TYPE_OPCODE_LSKP; 
	}
	if (assInput.command == "NLBR") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0xc8; 
		return 1; 
	}
	if (assInput.command == "LSNF") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0xc7; 
		return 1; 
	}
	if (assInput.command == "LSNQ") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0xc5; 
		return 1; 
	}
	if (assInput.command == "LSNZ") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0xc6; 
		return 1; 
	}
	if (assInput.command == "LSQ") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0xcd; 
		return 1; 
	}
	if (assInput.command == "LSZ") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0xce; 
		return 1; 
	}
	if (assInput.command == "MARK") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x79; 
		return 1; 
	}
	if (assInput.command == "NOP") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0xc4; 
		return 1; 
	}
	if (assInput.command == "OR") 
	{ 
		if (assInput.numberOfParameters > 0)
			return ERROR_TEMP_PAR;
		*b1 = 0xf1; 
		return 1; 
	}
	if (assInput.command == "ORI")
	{
		*b1 = 0xf9;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "OUT")
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

        int lowestOutValue = 1;
        if (cpuType_ == CPU1801)
            lowestOutValue = 0;
        
		if (assInput.parameterType[0] == ASS_HEX_VALUE)
		{
			if (assInput.parameterValue[0] < lowestOutValue || assInput.parameterValue[0] > 7)
				return ERROR_IO;

			*b1 = 0x60 | (assInput.parameterValue[0] & 0x7); 
			return 1;
		}
		if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "X" && allowX)
		{
			*b1 = 0x60 | lowestOutValue;
			*b7 = 0xf0;
			return 1;
		}
		return ERROR_IO;
	}
	if (assInput.command == "PHI")
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		*b1 = 0xb0 | registerNumber;
		return ret;
	}
	if (assInput.command == "PLO")
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		*b1 = 0xa0 | registerNumber;
		return ret;
	}
	if (assInput.command == "REQ") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x7a; 
		return 1; 
	}
	if (assInput.command == "RET") 
	{ 
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x70; 
		return 1; 
	}
	if (assInput.command == "SAV") 
	{ 
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x78; 
		return 1; 
	}
	if (assInput.command == "SD") 
	{ 
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0xf5; 
		return 1; 
	}
	if (assInput.command == "SDB") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x75; 
		return 1; 
	}
	if (assInput.command == "SDBI")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		*b1 = 0x7d;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "SDI")
	{
		*b1 = 0xfd;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "SEP")
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		*b1 = 0xd0 | registerNumber;
		return ret;
	}
	if (assInput.command == "SEQ") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x7b; 
		return 1; 
	}
	if (assInput.command == "SEX")
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		*b1 = 0xe0 | registerNumber;
		return ret;
	}
	if (assInput.command == "SHL") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_TEMP_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_TEMP_PAR;
		*b1 = 0xfe; 
		return 1; 
	}
	if (assInput.command == "SHLC") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x7e; 
		return 1; 
	}
	if (assInput.command == "RSHL") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x7e; 
		return MEM_TYPE_OPCODE_RSHL; 
	}
	if (assInput.command == "SHR") 
	{ 
		if (assInput.numberOfParameters > 0)
			return ERROR_TEMP_PAR;
		*b1 = 0xf6; 
		return 1; 
	}
	if (assInput.command == "SHRC") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x76; 
		return 1; 
	}
	if (assInput.command == "RSHR") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x76; 
		return MEM_TYPE_OPCODE_RSHR; 
	}
	if (assInput.command == "NBR" || assInput.command == "NB") 
	{ 
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x38; 
		return 1; 
	}
	if (assInput.command == "SKP") 
	{ 
		if (assInput.numberOfParameters > 0)
			return ERROR_TEMP_PAR;
		*b1 = 0x38; 
		return MEM_TYPE_OPCODE_SKP; 
	}
	if (assInput.command == "SM") 
	{ 
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0xf7; 
		return 1; 
	}
	if (assInput.command == "SMB") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x77; 
		return 1; 
	}
	if (assInput.command == "SMBI")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		*b1 = 0x7f;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "SMI")
	{
		*b1 = 0xff;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "STR")
	{
		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		*b1 = 0x50 | registerNumber;
		return ret;
	}
	if (assInput.command == "STXD") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;
		*b1 = 0x73; 
		return 1; 
	}
	if (assInput.command == "XOR") 
	{ 
		if (assInput.numberOfParameters > 0)
			return ERROR_TEMP_PAR;
		*b1 = 0xf3; 
		return 1; 
	}
	if (assInput.command == "XRI")
	{
		*b1 = 0xfb;
		return getByte(assInput, b2, allowX);
	}
	if (assInput.command == "STPC")
	{
		*b1 = 0x68;
		*b2 = 0;
		if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
	}
	if (assInput.command == "DTC")
	{
		*b1 = 0x68;
		*b2 = 0x1;
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		return 2;
	}
	if (assInput.command == "SPM2")
	{
		*b1 = 0x68;
		*b2 = 0x2;
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		return 2;
	}
	if (assInput.command == "SCM2")
	{
		*b1 = 0x68;
		*b2 = 0x3;
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		return 2;
	}
	if (assInput.command == "SPM1")
	{
		*b1 = 0x68;
		*b2 = 0x4;
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		return 2;
	}
	if (assInput.command == "SCM1")
	{
		*b1 = 0x68;
		*b2 = 0x5;
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		return 2;
	}
	if (assInput.command == "LDC")
	{
		*b1 = 0x68;
		*b2 = 0x6;
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		return 2;
	}
	if (assInput.command == "STM")
	{
		*b1 = 0x68;
		*b2 = 0x7;
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		return 2;
	}
	if (assInput.command == "GEC")
	{
		*b1 = 0x68;
		*b2 = 0x8;
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		return 2;
	}
	if (assInput.command == "ETQ")
	{
		*b1 = 0x68;
		*b2 = 0x9;
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		return 2;
	}
	if (assInput.command == "XIE")
	{
		*b1 = 0x68;
		*b2 = 0xa;
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		return 2;
	}
	if (assInput.command == "XID")
	{
		*b1 = 0x68;
		*b2 = 0xb;
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		return 2;
	}
	if (assInput.command == "CIE")
	{
		*b1 = 0x68;
		*b2 = 0xc;
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		return 2;
	}
	if (assInput.command == "CID")
	{
		*b1 = 0x68;
		*b2 = 0xd;
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		return 2;
	}
	if (assInput.command == "DBNZ")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		if (cpuType_ == CPU1804)  return ERROR_CPU_1804;

		if (assInput.numberOfParameters > 2)
			return ERROR_PAR;

		ret = getRegisterNumber(assInput, &registerNumber, b7, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0x20 | registerNumber;
		return getWordPar2(assInput, b3, b4, allowX);
	}
	if (assInput.command == "BCI")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;

		ret = getByte(assInput, b3, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0x3e;
		return 3;
	}
	if (assInput.command == "BXI")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;

		ret = getByte(assInput, b3, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0x3f;
		return 3;
	}
	if (assInput.command == "RLXA")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;

		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0x60 | registerNumber;
		return 2;
	}
	if (assInput.command == "DADC")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		if (cpuType_ == CPU1804)  return ERROR_CPU_1804;
		*b1 = 0x68;
		*b2 = 0x74;
		return 2;
	}
	if (assInput.command == "DSAV")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		if (cpuType_ == CPU1804)  return ERROR_CPU_1804;
		*b1 = 0x68;
		*b2 = 0x76;
		return 2;
	}
	if (assInput.command == "DSMB")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		if (cpuType_ == CPU1804)  return ERROR_CPU_1804;
		*b1 = 0x68;
		*b2 = 0x77;
		return 2;
	}
	if (assInput.command == "DACI")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		if (cpuType_ == CPU1804)  return ERROR_CPU_1804;

		ret = getByte(assInput, b3, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0x7c;
		return 3;
	}
	if (assInput.command == "DSBI")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		if (cpuType_ == CPU1804)  return ERROR_CPU_1804;

		ret = getByte(assInput, b3, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0x7f;
		return 3;
	}
	if (assInput.command == "SCAL")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;

		if (assInput.numberOfParameters > 2)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0x80 | registerNumber;
		return getWordPar2(assInput, b3, b4, allowX);
	}
	if (assInput.command == "SRET")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;

		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0x90 | registerNumber;
		return 2;
	}
	if (assInput.command == "RSXD")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;

		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0xa0 | registerNumber;
		return 2;
	}
	if (assInput.command == "RNX")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;

		if (assInput.numberOfParameters > 1)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0xb0 | registerNumber;
		return 2;
	}
	if (assInput.command == "RLDI")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;

		if (assInput.numberOfParameters > 2)
			return ERROR_PAR;

		ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0xc0 | registerNumber;
		return getWordPar2(assInput, b3, b4, allowX);
	}
	if (assInput.command == "RLDL") 
	{ 
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;

		if (assInput.numberOfParameters > 2)
			return ERROR_PAR;

		ret = getRegisterNumber(assInput, &registerNumber, b7, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0xc0 | registerNumber; 

		ret =  getWordPar2(assInput, b3, b4, allowX);
		if (ret > ERROR_START)  return ret;

		return MEM_TYPE_OPCODE_RLDL; 
	}
	if (assInput.command == "DADD")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		if (cpuType_ == CPU1804)  return ERROR_CPU_1804;

		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;

		*b1 = 0x68;
		*b2 = 0xf4;
		return 2;
	}
	if (assInput.command == "DSM")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		if (cpuType_ == CPU1804)  return ERROR_CPU_1804;

		if (assInput.numberOfParameters > 0)
			return ERROR_PAR;

		*b1 = 0x68;
		*b2 = 0xf7;
		return 2;
	}
	if (assInput.command == "DADI")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		if (cpuType_ == CPU1804)  return ERROR_CPU_1804;

		ret = getByte(assInput, b3, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0xfc;
		return 3;
	}
	if (assInput.command == "DSMI")
	{
        if (cpuType_ == CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
		if (cpuType_ == CPU1804)  return ERROR_CPU_1804;

		ret = getByte(assInput, b3, allowX);
		if (ret > ERROR_START)  return ret;

		*b1 = 0x68;
		*b2 = 0xff;
		return 3;
	}
	return ERROR_INST;
}

int DebugWindow::getByte(AssInput assInput, Byte* b2, bool allowX)
{
	if (assInput.numberOfParameters > 1)
		return ERROR_PAR;

	if (assInput.parameterType[0] == ASS_HEX_VALUE)
	{
		if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xff)
			return ERROR_8BIT;

		*b2 = assInput.parameterValue[0];
		return 2;
	}
	if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "X" && allowX)
	{
		return 1;
	}
	return ERROR_8BIT;
}

int DebugWindow::getSlot(AssInput assInput, Byte* b2)
{
	if (assInput.numberOfParameters > 3)
		return ERROR_PAR;

	if (assInput.parameterType[0] == ASS_SLOT)
	{
		if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xff)
			return ERROR_8BIT;

		*b2 = assInput.parameterValue[0];
		return 2;
	}
	return ERROR_8BIT;
}

int DebugWindow::getWord(AssInput assInput, Byte* b2, Byte* b3, bool allowX)
{
	if (assInput.numberOfParameters > 1)
		return ERROR_PAR;

	if (assInput.parameterType[0] == ASS_HEX_VALUE)
	{
		if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xffff)
			return ERROR_16BIT;

		*b2 = assInput.parameterValue[0] >> 8;
		*b3 = assInput.parameterValue[0] & 0xff;
		return 3;
	}
	if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "X" && allowX)
	{
		return 1;
	}
	return ERROR_8BIT;
}

int DebugWindow::getWordPar2(AssInput assInput, Byte* b3, Byte* b4, bool allowX)
{
	if (assInput.numberOfParameters > 2)
		return ERROR_PAR;

	if (assInput.parameterType[1] == ASS_HEX_VALUE)
	{
		if (assInput.parameterValue[1] < 0 || assInput.parameterValue[1] > 0xffff)
			return ERROR_16BIT;

		*b3 = assInput.parameterValue[1] >> 8;
		*b4 = assInput.parameterValue[1] & 0xff;
		return 4;
	}
	if (assInput.parameterType[1] == ASS_STRING && assInput.parameterString[1] == "X" && allowX)
	{
		return 2;
	}
	return ERROR_8BIT;
}

int DebugWindow::getRegisterNumber(AssInput assInput, long* registerNumber, Byte* b7, bool allowX)
{
	if (assInput.parameterType[0] == ASS_HEX_VALUE)
	{
		if (assInput.parameterValue[0] < 0 || assInput.parameterValue[0] > 0xf)
			return ERROR_REG;
		*registerNumber = assInput.parameterValue[0];
		return 1;
	}
	if (assInput.parameterType[0] == ASS_REG)
	{
		*registerNumber = assInput.parameterValue[0];
		return 1;
	}
	if (assInput.parameterType[0] == ASS_STRING && assInput.parameterString[0] == "X" && allowX) 
	{
		*registerNumber = 0;
		*b7 = 0xf0;
		return 1;
	}
	return ERROR_REG;
}

int DebugWindow::translateChipParameter(wxString buffer, long* value, int* type)
{
    if (buffer.Left(4)== "[RA]" || buffer.Left(4)== "[RB]" || buffer.Left(2)== "RA" || buffer.Left(2)== "RB" || 
		buffer.Left(4)== "RB.0" || buffer.Left(4)== "RB.1" || buffer.Left(4)== "RA.0" ||
		buffer.Left(6)== "[V0V1]" || buffer.Left(4)== "V0V1" || buffer.Left(6)== "[V2V3]" || buffer.Left(4)== "V2V3" ||
		buffer.Left(2)== "[I" || buffer.Left(3)== "V9]"|| buffer.Left(3)== "[I]" || buffer.Left(4)== "[>I]" ||
        buffer.Left(6)== "SWITCH" || buffer.Left(2)== "ST")
    {
        *type = ASS_STRING;
        return 0;
    }
	if (buffer.Left(1) == "V")
	{
		buffer = buffer.Right(buffer.Len()-1);
		if (!buffer.ToLong(value, 16))
			return ERROR_REG;
		if (*value < 0 || *value > 15)
			return ERROR_REG;
		*type = CHIP8_VX;
		return 0;
	}
	if (buffer.Left(1) == "R")
	{
		buffer = buffer.Right(buffer.Len()-1);
		if (!buffer.ToLong(value, 16))
			return ERROR_REG;
		if (*value < 0 || *value > 15)
			return ERROR_REG;
		*type = ASS_REG;
		return 0;
	}
	if (buffer.Left(1) == "S")
	{
		buffer = buffer.Right(buffer.Len()-1);
		if (!buffer.ToLong(value, 16))
			return ERROR_SLOT;
		if (*value < 0 || *value > 255)
			return ERROR_SLOT;
		*type = ASS_SLOT;
		return 0;
	}
	if (buffer.Left(2) == "[V" && buffer.Right(1)== "]")
	{
		buffer = buffer.Mid(2, 1);
		if (!buffer.ToLong(value, 16))
			return ERROR_REG;
		if (*value < 0 || *value > 15)
			return ERROR_REG;
		*type = CHIP8_VX_MEM;
		return 0;
	}
    if (buffer.Left(1) == "[" && buffer.Right(1)== "]")
    {
        buffer = buffer.Right(buffer.Len()-1);
        buffer = buffer.Left(buffer.Len()-1);
        if (buffer.ToLong(value, 16))
            *type = ASS_HEX_VALUE_MEM;
        return 0;
    }
    if (buffer.Left(1) == "&")
    {
        buffer = buffer.Right(buffer.Len()-1);
        if (!buffer.ToLong(value, 10))
        {
            buffer = "&" + buffer;
            *type = ASS_STRING;
        }
        else
            *type = ASS_HEX_VALUE;
        return 0;
    }
	if (!buffer.ToLong(value, 16))
		*type = ASS_STRING;
	else
		*type = ASS_HEX_VALUE;
	return 0;
}

int DebugWindow::getRegister(wxString buffer)
{
	if (buffer == "D")  return TREG_D;
	if (buffer == "DF")  return TREG_DF;
	if (buffer == "P")  return TREG_P;
	if (buffer == "X")  return TREG_X;
	if (buffer == "T")  return TREG_T;
	if (buffer == "Q")  return TREG_Q;
	if (buffer == "R0")  return TREG_R0;
	if (buffer == "R1")  return TREG_R1;
	if (buffer == "R2")  return TREG_R2;
	if (buffer == "R3")  return TREG_R3;
	if (buffer == "R4")  return TREG_R4;
	if (buffer == "R5")  return TREG_R5;
	if (buffer == "R6")  return TREG_R6;
	if (buffer == "R7")  return TREG_R7;
	if (buffer == "R8")  return TREG_R8;
	if (buffer == "R9")  return TREG_R9;
	if (buffer == "RA")  return TREG_RA;
	if (buffer == "RB")  return TREG_RB;
	if (buffer == "RC")  return TREG_RC;
	if (buffer == "RD")  return TREG_RD;
	if (buffer == "RE")  return TREG_RE;
	if (buffer == "RF")  return TREG_RF;
	return TREG_FAULT;
}

void DebugWindow::onDebugSaveDump(wxCommandEvent&WXUNUSED(event))
{
	int num = 0;
	long addr;
	Byte value;
	wxFile outputFile;
	wxTextFile outputTextFile;
	wxString fileName, memoryStr, number, strValue, line;

	switch (memoryDisplay_)
	{
		case CPU_MEMORY:
			memoryStr = "CPU Memory";
			fileName = "memorydump";
		break;

        case CDP_1870_C:
            memoryStr = "CDP 1870 Character Ram";
            fileName = "cdp1870charramdump";
            break;
            
        case CDP_1870_COLOUR:
            memoryStr = "CDP 1870 Colour Ram";
            fileName = "cdp1870colourramdump";
            break;
            
		case CDP_1870_P:
			memoryStr = "CDP 1870 Page Ram";
			fileName = "cdp1870pageramdump";
		break;

		case TMS_MEMORY:
			memoryStr = "TMS video Ram";
			fileName = "tmsramdump";
		break;

		case VT_RAM:
			memoryStr = "VT video Ram";
			fileName = "vtramdump";
		break;

		case CDP_1864:
			memoryStr = "CDP 1864 Color Ram";
			fileName = "cdp1864colorramdump";
		break;

		case V_6845:
			memoryStr = "MC6845 Char Rom";
			fileName = "mc6845charromdump";
		break;

		case V_6847:
			memoryStr = "MC6847 Char Rom";
			fileName = "mc6847charromdump";
		break;

		case V_6847_RAM:
			memoryStr = "MC6847 video Ram";
			fileName = "mc6847ramdump";
		break;

        case I_8275:
            memoryStr = "Intel 8275 Char Rom";
            fileName = "i8275charromdump";
        break;
            
        case I_8275_RAM:
            memoryStr = "Intel 8275 video Ram";
            fileName = "i8275ramdump";
        break;
	}

	fileName = wxFileSelector( "Select the " + memoryStr + " dump file to save",
                               debugDir_, fileName,
                               "bin|txt",
                               wxString::Format
                              (
                                   "Binary File (*.bin)|*.bin|Text File (*.txt)|*.txt"
                               ),
                               wxFD_SAVE|wxFD_CHANGE_DIR,
                               this
                              );

	if (!fileName || fileName.empty())
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	wxString name = FullPath.GetName();
	wxString path = FullPath.GetPath();
	wxString ext = FullPath.GetExt();

	while(wxFile::Exists(fileName))
	{
		num++;
		number.Printf("%d", num);
		fileName = path + pathSeparator_ + name + "." + number + "." + ext;
	}

	if (ext == "bin")
	{
		outputFile.Create(fileName);
		for (long address = 0; address <= getAddressMask(); address++)
		{
			value = debugReadMem(address);
			outputFile.Write(&value, 1);
		}
		outputFile.Close();
	}
	else
	{
		outputTextFile.Create(fileName);
		addr = 0;
		while(addr <= getAddressMask())
		{
			line.Printf("%04X:", (unsigned int)addr);
			for (int i=0; i<16; i++)
			{
				strValue.Printf(" %02X", debugReadMem(addr+i));
				line = line + strValue;
			}

			outputTextFile.AddLine(line);
			addr += 16;
		}
		outputTextFile.Write();
		outputTextFile.Close();
	}
}

void DebugWindow::onPauseButton(wxCommandEvent&WXUNUSED(event))
{
	if (p_Computer->getSteps() < 0)
		p_Computer->setSteps(0);
	else
		p_Computer->setSteps(-1);
	setPauseState();
}

void DebugWindow::setPauseState()
{
	if (p_Computer->getSteps() == 0)
	{
		XRCCTRL(*this, "DebugPauseButton", wxBitmapButton)->SetBitmapLabel(pauseOnBitmap);
		XRCCTRL(*this,"DebugStepButton", wxBitmapButton)->Enable(true);
	}
	else
	{
		if (p_Computer->getSteps() > 0)
		{
			XRCCTRL(*this, "DebugPauseButton", wxBitmapButton)->SetBitmapLabel(pauseOffBitmap);
			XRCCTRL(*this,"DebugStepButton", wxBitmapButton)->Enable(true);
		}
		else
		{
			XRCCTRL(*this, "DebugPauseButton", wxBitmapButton)->SetBitmapLabel(pauseOffBitmap);
			XRCCTRL(*this,"DebugStepButton", wxBitmapButton)->Enable(false);
		}
	}
    p_Main->eventUpdateTitle();
}

void DebugWindow::onStepButton(wxCommandEvent&WXUNUSED(event))
{
	performStep_ = true;
}

void DebugWindow::onRunButton(wxCommandEvent&WXUNUSED(event))
{
	p_Computer->setSteps(-1);
	setPauseState();

	long runAddress = get16BitValue("DebugRunAddress");
	if (runAddress == -1)  return;

	p_Computer->setScratchpadRegister(p_Computer->getProgramCounter(), runAddress);
}

void DebugWindow::onRunAddress(wxCommandEvent&WXUNUSED(event))
{
	get16BitValue("DebugRunAddress");
}

void DebugWindow::onBreakPointAddress(wxCommandEvent&WXUNUSED(event))
{
	get16BitValue("BreakPointAddress");
}

void DebugWindow::onTregValue(wxCommandEvent&WXUNUSED(event))
{
	get16BitValue("TregValue");
}

void DebugWindow::onNumberOfSteps(wxCommandEvent&WXUNUSED(event))
{
	wxString stepsStr = XRCCTRL(*this,"NumberOfSteps",wxTextCtrl)->GetValue();
	if (!stepsStr.ToLong(&steps_))
		steps_ = 1;
}

void DebugWindow::SetDebugMode()
{
	if (trace_ || traceTrap_ || traceDma_ || traceInt_ || (numberOfBreakPoints_ > 0) || (numberOfTraps_ > 0) || (numberOfTregs_ > 0))
		updateDebugMenu(true);
	else
		updateDebugMenu(false);
}

void DebugWindow::SetChip8DebugMode()
{
	if (chip8Trace_|| traceChip8Int_ || (numberOfChip8BreakPoints_ > 0))
		updateChip8DebugMenu(true);
	else
		updateChip8DebugMenu(false);
}

void DebugWindow::onLog(wxCommandEvent& WXUNUSED(event))
{
//	wxSetWorkingDirectory (workingDir_);
	int num = 0;
	wxString fileName, number;

	fileName = wxFileSelector( "Select the log file to save",
                               debugDir_, "trace.log",
                               "log",
                               wxString::Format
                              (
                                   "Dump File (*.log)|*.log|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );

	if (!fileName || fileName.empty())
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	wxString name = FullPath.GetName();
	wxString path = FullPath.GetPath();
	wxString ext = FullPath.GetExt();

	while(wxFile::Exists(fileName))
	{
		num++;
		number.Printf("%d", num);
		fileName = path + pathSeparator_ + name + "." + number + "." + ext;
	}
	traceWindowPointer->SaveFile(fileName);
}

void DebugWindow::onDebugDisChip8(wxCommandEvent& WXUNUSED(event))
{
	enableDebugGui(true);
}

void DebugWindow::onClear(wxCommandEvent& WXUNUSED(event))
{
	traceWindowPointer->Clear();
}

void DebugWindow::onTrace(wxCommandEvent& WXUNUSED(event))
{
	trace_ = !trace_;
	if (computerRunning_)
	{
		p_Computer->setTraceStatus(trace_);
		enableDebugGui(true);
	}
	SetDebugMode();
}

void DebugWindow::onTraceDma(wxCommandEvent& WXUNUSED(event))
{
	traceDma_ = !traceDma_;
	if (computerRunning_)
	{
		p_Computer->setDmaTraceStatus(traceDma_);
		enableDebugGui(true);
	}
	SetDebugMode();
}

void DebugWindow::onTraceInt(wxCommandEvent& WXUNUSED(event))
{
	traceInt_ = !traceInt_;
	if (computerRunning_)
	{
		p_Computer->setIntTraceStatus(traceInt_);
		enableDebugGui(true);
	}
	SetDebugMode();
}

void DebugWindow::onChip8TraceInt(wxCommandEvent& WXUNUSED(event))
{
	traceChip8Int_ = !traceChip8Int_;
	if (computerRunning_)
	{
		p_Computer->setChip8IntTraceStatus(traceChip8Int_);
		enableChip8DebugGui(true);
	}
	SetChip8DebugMode();
}

void DebugWindow::onTraceTrap(wxCommandEvent& WXUNUSED(event))
{
	traceTrap_ = !traceTrap_;
	if (traceTrap_)
		XRCCTRL(*this, "TrapWindowText", wxStaticText)->SetLabel("Instr. Trace:");
	else
		XRCCTRL(*this, "TrapWindowText", wxStaticText)->SetLabel("Instr. Traps:");
	enableDebugGui(true);
	SetDebugMode();
}

void DebugWindow::onInt(wxCommandEvent& WXUNUSED(event))
{
	p_Computer->interrupt();
}

void DebugWindow::onReset(wxCommandEvent& WXUNUSED(event))
{
    p_Computer->onReset();
}

void DebugWindow::onBreakPointSet(wxCommandEvent&WXUNUSED(event))
{
	if (numberOfBreakPoints_ == 64)
	{
		(void)wxMessageBox( "Maximum number of Break Points set\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return;
	}

	long breakPointAddress = get16BitValue("BreakPointAddress");
	if (breakPointAddress == -1)
	{
		(void)wxMessageBox( "No Break Point value specified\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return;
	}

	breakPoints_[numberOfBreakPoints_] = breakPointAddress;
	breakPointsSelected_[numberOfBreakPoints_] = true;
	addBreakPoint();
	SetDebugMode();
}

void DebugWindow::onTregSet(wxCommandEvent&WXUNUSED(event))
{
	wxString printBuffer, strValue;

	char reg = XRCCTRL(*this,"TregRegister",wxChoice)->GetCurrentSelection();

	if ((reg >= TREG_D) && (reg <= TREG_RF))
	{
		long tregValue = get16BitValue("TregValue");
		if (tregValue == -1)
		{
			(void)wxMessageBox( "No Register Value value specified\n",
										"Emma 02", wxICON_ERROR | wxOK );
			return;
		}

		if (numberOfTregs_ == 64)
		{
			(void)wxMessageBox( "Maximum number of Register Traps set\n",
										"Emma 02", wxICON_ERROR | wxOK );
			return;
		}

		if ((reg >= TREG_R0) && (reg <= TREG_RF))
		{
			if (tregValue > 0xffff)
			{
				(void)wxMessageBox( "Please specify value of 16 bit max\n",
											"Emma 02", wxICON_ERROR | wxOK );
				return;
			}
		}
		else
		{
			if (reg <= TREG_T)
			{
				if (tregValue > 0xff)
				{
					(void)wxMessageBox( "Please specify value of 8 bit max\n",
												"Emma 02", wxICON_ERROR | wxOK );
					return;
				}
			}
			else
			{
				if (tregValue > 1)
				{
					(void)wxMessageBox( "Please specify value of 0 or 1\n",
												"Emma 02", wxICON_ERROR | wxOK );
					return;
				}
			}
		}

		tregs_[numberOfTregs_][0] = reg;
		tregs_[numberOfTregs_][1] = tregValue;
		tregsSelected_[numberOfTregs_] = true;
		addTreg();
		SetDebugMode();
	}
	else
	{
		(void)wxMessageBox( "No register specified\n",
									"Emma 02", wxICON_ERROR | wxOK );
	}

}

void DebugWindow::onTrapCommand(wxCommandEvent&event)
{
	int command = event.GetSelection();
	wxString value = XRCCTRL(*this, "TrapValue", wxTextCtrl)->GetValue();

	if (opCode2[command] == 0x30)
	{
		XRCCTRL(*this, "TrapValue", wxTextCtrl)->Enable(numberOfBytes[command] > 1);
		if (numberOfBytes[command] == 2)
		{
			XRCCTRL(*this, "TrapValue", wxTextCtrl)->SetMaxLength(2);
			if (value != "")
				XRCCTRL(*this, "TrapValue", wxTextCtrl)->SetValue(value.Right(2));
		}
		if (numberOfBytes[command] == 3)
			XRCCTRL(*this, "TrapValue", wxTextCtrl)->SetMaxLength(4);
	}
	else
	{
		XRCCTRL(*this, "TrapValue", wxTextCtrl)->Enable(numberOfBytes[command] > 2);
		if (numberOfBytes[command] == 3)
		{
			XRCCTRL(*this, "TrapValue", wxTextCtrl)->SetMaxLength(2);
			if (value != "")
				XRCCTRL(*this, "TrapValue", wxTextCtrl)->SetValue(value.Right(2));
		}
		if (numberOfBytes[command] == 4)
			XRCCTRL(*this, "TrapValue", wxTextCtrl)->SetMaxLength(4);
	}

	XRCCTRL(*this, "TrapRegister", wxChoice)->Enable(useRegister[command]);
}

void DebugWindow::onTrapSet(wxCommandEvent&WXUNUSED(event))
{
	if (numberOfTraps_ == 64)
	{
		(void)wxMessageBox( "Maximum number of Command Traps set\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return;
	}

	int command = XRCCTRL(*this,"TrapCommand",wxChoice)->GetCurrentSelection();
	wxString strValue = XRCCTRL(*this,"TrapValue",wxTextCtrl)->GetValue(), errorCode;
	long trapValue;

	if ((command >= 0) && (command <= 118))
	{
		traps_[numberOfTraps_][7] = 0xff;
		traps_[numberOfTraps_][0] = numberOfBytes[command];
		traps_[numberOfTraps_][1] = opCode[command];
		traps_[numberOfTraps_][8] = macro[command];

		if (opCode[command] == 0x68)
		{
			if (cpuType_ >= minCpuType[command])
			{
				traps_[numberOfTraps_][2] = opCode2[command];
				if (useRegister[command])
				{
					int registerValue = XRCCTRL(*this,"TrapRegister",wxChoice)->GetCurrentSelection()-1;
					if ((registerValue >= 0) && (registerValue <= 0xf))
					{
						traps_[numberOfTraps_][2] |= registerValue;
					}
					else
					{
						traps_[numberOfTraps_][7] = 0xf0;
					}
				}
				if (traps_[numberOfTraps_][0] == 3)
				{
					strValue = strValue.MakeUpper();
					if (strValue == "X")
					{
						traps_[numberOfTraps_][0] = 2;
					}
					else
					{
						if (!strValue.ToLong(&trapValue, 16))
						{
							(void)wxMessageBox( "Please specify value in hexadecimal\n",
														"Emma 02", wxICON_ERROR | wxOK );
							return;
						}
						if (trapValue > 0xff)
						{
							(void)wxMessageBox( "Please specify value of 8 bit max\n",
														"Emma 02", wxICON_ERROR | wxOK );
							return;
						}
						else
						{
							traps_[numberOfTraps_][3] = trapValue;
						}
					}
				}
				if (traps_[numberOfTraps_][0] == 4)
				{
					strValue = strValue.MakeUpper();
					if (strValue == "X")
					{
						traps_[numberOfTraps_][0] = 2;
					}
					else
					{
						if (!strValue.ToLong(&trapValue, 16))
						{
							(void)wxMessageBox( "Please specify value in hexadecimal\n",
														"Emma 02", wxICON_ERROR | wxOK );
							return;
						}
						if (trapValue > 0xffff)
						{
							(void)wxMessageBox( "Please specify value of 16 bit max\n",
														"Emma 02", wxICON_ERROR | wxOK );
							return;
						}
						else
						{
							traps_[numberOfTraps_][3] = (trapValue >> 8) & 0xff;
							traps_[numberOfTraps_][4] = trapValue & 0xff;
						}
					}
				}
			}
			else
			{
                if (cpuType_ == CPU1801)
                    errorCode = "Not supported on CDP1801\n";
                if (cpuType_ == CPU1802)
					errorCode = "Not supported on CDP1802\n";
				if (cpuType_ == CPU1804)
					errorCode = "Not supported on CDP1804\n";
				(void)wxMessageBox( errorCode,
											"Emma 02", wxICON_ERROR | wxOK );
				return;
			}
		}
		else
		{
            if (cpuType_ < minCpuType[command])
            {
                errorCode = "Not supported on CDP1801\n";
                (void)wxMessageBox( errorCode,
                                   "Emma 02", wxICON_ERROR | wxOK );
                return;
            }
            
            if (useRegister[command])
			{
				int registerValue = XRCCTRL(*this,"TrapRegister",wxChoice)->GetCurrentSelection()-1;
				if ((registerValue >= 0) && (registerValue <= 0xf))
				{
					traps_[numberOfTraps_][1] |= registerValue;
				}
				else
				{
					traps_[numberOfTraps_][7] = 0xf0;
				}
			}
			if ((opCode[command] == 0x61) || (opCode[command] == 0x69))
			{
				strValue = strValue.MakeUpper();
				traps_[numberOfTraps_][0] = 1;
				if (strValue == "X")
				{
					traps_[numberOfTraps_][7] = 0xf8;
				}
				else
				{
					if (!strValue.ToLong(&trapValue))
					{
						(void)wxMessageBox( "Please specify value 0 to 7\n",
													"Emma 02", wxICON_ERROR | wxOK );
						return;
					}
					if ((trapValue >= 0) && (trapValue <= 7))
					{
						traps_[numberOfTraps_][1] = traps_[numberOfTraps_][1] + trapValue - 1;
					}
					else
					{
						(void)wxMessageBox( "Please specify value 0 to 7\n",
													"Emma 02", wxICON_ERROR | wxOK );
						return;
					}
				}
			}
			if (traps_[numberOfTraps_][0] == 2)
			{
				strValue = strValue.MakeUpper();
				if (strValue == "X")
				{
					traps_[numberOfTraps_][0] = 1;
				}
				else
				{
					if (!strValue.ToLong(&trapValue, 16))
					{
						(void)wxMessageBox( "Please specify value in hexadecimal\n",
													"Emma 02", wxICON_ERROR | wxOK );
						return;
					}
					if (trapValue > 0xff)
					{
						(void)wxMessageBox( "Please specify value of 8 bit max\n",
													"Emma 02", wxICON_ERROR | wxOK );
						return;
					}
					else
					{
						traps_[numberOfTraps_][2] = trapValue;
					}
				}
			}
			if (traps_[numberOfTraps_][0] == 3)
			{
				strValue = strValue.MakeUpper();
				if (strValue == "X")
				{
					traps_[numberOfTraps_][0] = 1;
				}
				else
				{
					if (!strValue.ToLong(&trapValue, 16))
					{
						(void)wxMessageBox( "Please specify value in hexadecimal\n",
													"Emma 02", wxICON_ERROR | wxOK );
						return;
					}
					if (trapValue > 0xffff)
					{
						(void)wxMessageBox( "Please specify value of 16 bit max\n",
													"Emma 02", wxICON_ERROR | wxOK );
						return;
					}
					else
					{
						traps_[numberOfTraps_][2] = (trapValue >> 8) & 0xff;
						traps_[numberOfTraps_][3] = trapValue & 0xff;
					}
				}
			}
		}
	}
	else
	{
		(void)wxMessageBox( "No command specified\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return;
	}
	trapsSelected_[numberOfTraps_] = true;
	addTrap();
	SetDebugMode();
}

void DebugWindow::D(wxCommandEvent&WXUNUSED(event))
{
	long value = get8BitValue("D");
	if (value == -1)  return;

	p_Computer->setAccumulator(value);
}

void DebugWindow::P(wxCommandEvent&WXUNUSED(event))
{
	long value = get8BitValue("P");
	if (value == -1)  return;


	p_Computer->setProgramCounter(value);
}

void DebugWindow::X(wxCommandEvent&WXUNUSED(event))
{
	long value = get8BitValue("X");
	if (value == -1)  return;

	p_Computer->setDataPointer(value);
}

void DebugWindow::T(wxCommandEvent&WXUNUSED(event))
{
	long value = get8BitValue("T");
	if (value == -1)  return;

	p_Computer->setRegisterT(value);
}

void DebugWindow::DF(wxCommandEvent&WXUNUSED(event))
{
	long value = getBitValue("DF");
	if (value == -1)  return;

	p_Computer->setDataFlag(value);
}

void DebugWindow::Q(wxCommandEvent&WXUNUSED(event))
{
	long value = getBitValue("Q");
	if (value == -1)  return;

	p_Computer->setFlipFlopQ(value);
}

void DebugWindow::IE(wxCommandEvent&WXUNUSED(event))
{
	long value = getBitValue("IE");
	if (value == -1)  return;

	p_Computer->setInterruptEnable(value);
}

void DebugWindow::EF1(wxCommandEvent&WXUNUSED(event))
{
	long value = getBitValue("EF1");
	if (value == -1)  return;

	p_Computer->setEf(1, (int)value);
    lastEf1_ = value;
}

void DebugWindow::EF2(wxCommandEvent&WXUNUSED(event))
{
	long value = getBitValue("EF2");
	if (value == -1)  return;

	p_Computer->setEf(2, (int)value);
    lastEf2_ = value;
}

void DebugWindow::EF3(wxCommandEvent&WXUNUSED(event))
{
	long value = getBitValue("EF3");
	if (value == -1)  return;

	p_Computer->setEf(3, (int)value);
    lastEf3_ = value;
}

void DebugWindow::EF4(wxCommandEvent&WXUNUSED(event))
{
	long value = getBitValue("EF4");
	if (value == -1)  return;

	p_Computer->setEf(4, (int)value);
    lastEf4_ = value;
}

void DebugWindow::chip8I(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("Chip8I");
	if (value == -1)  return;

    if (chip8Type_ == CHIPSTIV)
    {
        p_Computer->writeMem(0x27f6, (value&0xff00)>>8, false);
        p_Computer->writeMem(0x27f7, value&0xff, false);
    }
    else
        p_Computer->setScratchpadRegister(CHIP8_I, value);
}

void DebugWindow::Vx(wxCommandEvent&event)
{
	wxString buttonName = wxWindow::FindWindowById(event.GetId())->GetName();
	long value = get16BitValue(buttonName);
	if (value == -1)  return;

	wxString buttonNumber = buttonName.Last();
	long number;
	if (buttonNumber.ToLong(&number, 16))
		p_Computer->writeMem(p_Computer->getChip8baseVar() + number, value, false);
}

void DebugWindow::R0(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R0");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(0, value);
}

void DebugWindow::R1(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R1");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(1, value);
}

void DebugWindow::R2(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R2");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(2, value);
}

void DebugWindow::R3(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R3");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(3, value);
}

void DebugWindow::R4(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R4");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(4, value);
}

void DebugWindow::R5(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R5");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(5, value);
}

void DebugWindow::R6(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R6");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(6, value);
}

void DebugWindow::R7(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R7");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(7, value);
}

void DebugWindow::R8(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R8");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(8, value);
}

void DebugWindow::R9(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R9");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(9, value);
}

void DebugWindow::RA(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R10");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(10, value);
}

void DebugWindow::RB(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R11");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(11, value);
}

void DebugWindow::RC(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R12");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(12, value);
}

void DebugWindow::RD(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R13");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(13, value);
}

void DebugWindow::RE(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R14");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(14, value);
}

void DebugWindow::RF(wxCommandEvent&WXUNUSED(event))
{
	long value = get16BitValue("R15");
	if (value == -1)  return;

	p_Computer->setScratchpadRegister(15, value);
}

void DebugWindow::O1(wxCommandEvent&WXUNUSED(event))
{
	long value = get8BitValue("O1");
	if (value == -1)  return;

	p_Computer->out(1, 0, value);
}

void DebugWindow::O2(wxCommandEvent&WXUNUSED(event))
{
	long value = get8BitValue("O2");
	if (value == -1)  return;

	p_Computer->out(2, 0, value);
}

void DebugWindow::O3(wxCommandEvent&WXUNUSED(event))
{
	long value = get8BitValue("O3");
	if (value == -1)  return;

	p_Computer->out(3, 0, value);
}

void DebugWindow::O4(wxCommandEvent&WXUNUSED(event))
{
	long value;

	if (runningComputer_ == COMX || runningComputer_ == CIDELSA || runningComputer_ ==  TMC600 || runningComputer_ == PECOM)
	{
		value = get16BitValue("O4");
		if (value == -1)  return;
		p_Computer->out(4, value, 0);
	}
	else
	{
		value = get8BitValue("O4");
		if (value == -1)  return;
		p_Computer->out(4, 0, value);
	}
}

void DebugWindow::O5(wxCommandEvent&WXUNUSED(event))
{
	long value;

	if (runningComputer_ == COMX || runningComputer_ == CIDELSA || runningComputer_ ==  TMC600 || runningComputer_ == PECOM)
	{
		value = get16BitValue("O5");
		if (value == -1)  return;
		p_Computer->out(5, value, 0);
	}
	else
	{
		value = get8BitValue("O5");
		if (value == -1)  return;
		p_Computer->out(5, 0, value);
	}
}

void DebugWindow::O6(wxCommandEvent&WXUNUSED(event))
{
	long value;

	if (runningComputer_ == COMX || runningComputer_ == CIDELSA || runningComputer_ ==  TMC600 || runningComputer_ == PECOM)
	{
		value = get16BitValue("O6");
		if (value == -1)  return;
		p_Computer->out(6, value, 0);
	}
	else
	{
		value = get8BitValue("O6");
		if (value == -1)  return;
		p_Computer->out(6, 0, value);
	}
}

void DebugWindow::O7(wxCommandEvent&WXUNUSED(event))
{
	long value;

	if (runningComputer_ == COMX || runningComputer_ == CIDELSA || runningComputer_ ==  TMC600 || runningComputer_ == PECOM)
	{
		value = get16BitValue("O7");
		if (value == -1)  return;
		p_Computer->out(7, value, 0);
	}
	else
	{
		value = get8BitValue("O7");
		if (value == -1)  return;
		p_Computer->out(7, 0, value);
	}
}

void DebugWindow::onProtectedMode(wxCommandEvent&event)
{
	protectedMode_ = event.IsChecked();
	enableDebugGui(true);
}

void DebugWindow::onDebugDisplayPageSpinUp(wxSpinEvent&WXUNUSED(event))
{
	debugDisplayPageSpinUp();
}

void DebugWindow::debugDisplayPageSpinUp()
{
	long address = get16BitValue("DebugDisplayPage");
	if (address == -1)  return;

	address += 0x100;

	Word ramMask = getAddressMask();

	if (memoryDisplay_ == V_6847_RAM)
	{
		while (address >= ((ramMask+1)*2)) 
			address -=  ((ramMask + 1)*2);
	}
	else
	{
		while (address > ramMask)  
			address -=  (ramMask + 1);
	}

	XRCCTRL(*this, "DebugDisplayPage", HexEdit)->changeNumber((unsigned int)address);

	memoryDisplay();
}

void DebugWindow::onDebugDisplayPageSpinDown(wxSpinEvent&WXUNUSED(event))
{
	long address = get16BitValue("DebugDisplayPage");
	if (address == -1)  return;

	address -= 0x100;

	Word ramMask = getAddressMask();

	if (memoryDisplay_ == V_6847_RAM)
	{
		while (address < 0)  
			address +=  ((ramMask + 1)*2);
	}
	else
	{
		while (address < 0)  
			address +=  (ramMask + 1);
	}

	XRCCTRL(*this, "DebugDisplayPage", HexEdit)->changeNumber((unsigned int)address);

	memoryDisplay();
}

void DebugWindow::directAss()
{
	if (!computerRunning_)
		return;

#if defined(__WXMAC__)
	wxFont exactFont(13, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxFont exactFontBold(13, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
#else
	wxFont exactFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxFont exactFontBold(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
#endif
	dcAss.SelectObject(*assBmp);

//	dcAss.SetFont(exactFont);
    switch (windowInfo.operatingSystem)
    {
        case OS_MAC:
            dcAss.SetPen(wxPen(wxColour(219, 219, 219)));
            dcAss.SetBrush(wxBrush(wxColour(219, 219, 219)));
            dcAss.SetTextBackground(wxColour(219, 219, 219));
        break;
            
        case OS_WINDOWS_2000:
            dcAss.SetPen(wxPen(wxColour(0xd4, 0xd0, 0xc8)));
            dcAss.SetBrush(wxBrush(wxColour(0xd4, 0xd0, 0xc8)));
            dcAss.SetTextBackground(wxColour(0xd4, 0xd0, 0xc8));
        break;
            
        case OS_LINUX_OPENSUSE_GNOME:
        case OS_LINUX_OPENSUSE_KDE:
            dcAss.SetPen(wxPen(wxColour(0xfb, 0xf8, 0xf1)));
            dcAss.SetBrush(wxBrush(wxColour(0xfb, 0xf8, 0xf1)));
            dcAss.SetTextBackground(wxColour(0xfb, 0xf8, 0xf1));
        break;
            
        case OS_LINUX_UBUNTU_11_04:
        case OS_LINUX_UBUNTU_11_10:
            dcAss.SetPen(wxPen(wxColour(242, 241, 240)));
            dcAss.SetBrush(wxBrush(wxColour(242, 241, 240)));
            dcAss.SetTextBackground(wxColour(242, 241, 240));
        break;

        case OS_LINUX_FEDORA:
            dcAss.SetPen(wxPen(wxColour(232, 232, 231)));
            dcAss.SetBrush(wxBrush(wxColour(232, 232, 231)));
            dcAss.SetTextBackground(wxColour(0xfb, 0xf8, 0xf1));
        break;

        default:
            dcAss.SetPen(wxPen(wxColour(255,255,255)));
            dcAss.SetBrush(wxBrush(wxColour(255,255,255)));
            dcAss.SetTextBackground(wxColour(255,255,255));
        break;
    }
	dcAss.DrawRectangle(0, 0, ASS_WIDTH, numberOfDebugLines_*LINE_SPACE+4);

	if (dirAssStart_ == dirAssEnd_)
	{
		for (int i=0; i<EDIT_ROW; i++)
			assSpinUp();
	}

	Word address = dirAssStart_;

	wxString code, text;
	int count;
	wxString line2;
	wxString printBufferAddress, printBufferOpcode;

	for (int line=0; line <numberOfDebugLines_; line ++)
	{
		wxColourDatabase colour;
		if (line == EDIT_ROW)
		{
			dcAss.SetFont(exactFontBold);
			dirAssAddress_ = address;
			dcAss.SetTextForeground(colour.Find("BLACK"));
			dcAss.DrawText(">", 1, 1+line*LINE_SPACE);
			dcAss.DrawText("<", ASS_WIDTH-9, 1+EDIT_ROW*LINE_SPACE);
		}
		else
			dcAss.SetFont(exactFont);
			
		dcAss.SetTextForeground(colour.Find("GREY"));
		for (int i=0; i<lastRange_; i++)
		{
			if (address >= dirAssProgramStartVector[i] && address <= dirAssProgramEndVector[i])
			{
				switch (runningComputer_)
				{
					case COMX:
						if (address >= 0xC000 && address <= 0xDFFF)
						{
							if (p_Computer->getOutValue(1) ==  dirAssSlotVector[i])
								dcAss.SetTextForeground(colour.Find("BLACK"));
						}
						else
							dcAss.SetTextForeground(colour.Find("BLACK"));
					break;

					case ELF:
					case ELFII:
					case SUPERELF:
						if (elfConfiguration[runningComputer_].useEms)
						{
							if (address >= 0x8000 && address <= 0xBFFF)
							{
								if (p_Computer->getEmsPage() == dirAssSlotVector[i])
									dcAss.SetTextForeground(colour.Find("BLACK"));
							}
							else
								dcAss.SetTextForeground(colour.Find("BLACK"));
						}
						else
							dcAss.SetTextForeground(colour.Find("BLACK"));
                        if (elfConfiguration[runningComputer_].useRomMapper)
                        {
                            if (address >= 0x8000 && address <= 0xFFFF)
                            {
                                if (p_Computer->getEmsPage() == dirAssSlotVector[i])
                                    dcAss.SetTextForeground(colour.Find("BLACK"));
                            }
                            else
                                dcAss.SetTextForeground(colour.Find("BLACK"));
                        }
                        else
                            dcAss.SetTextForeground(colour.Find("BLACK"));
					break;

					default:
						dcAss.SetTextForeground(colour.Find("BLACK"));
					break;
				}
			}
		}

        switch(p_Computer->readMemLabelType(address))
        {
            case LABEL_TYPE_BRANCH:
                dcAss.SetTextForeground(colour.Find("FOREST GREEN"));
            break;
            case LABEL_TYPE_JUMP:
                dcAss.SetTextForeground(colour.Find("FOREST GREEN"));
            break;
            case LABEL_TYPE_SUB:
                dcAss.SetTextForeground(wxColour(200,51,161));
            break;
        }
        
		Byte memType = p_Computer->readMemDataType(address);
        Byte tempByte;
        
		switch (memType)
		{
			case MEM_TYPE_OPCODE:
			case MEM_TYPE_OPCODE_RSHR:
			case MEM_TYPE_OPCODE_RSHL:
			case MEM_TYPE_OPCODE_BPZ:
			case MEM_TYPE_OPCODE_BGE:
			case MEM_TYPE_OPCODE_BM:
			case MEM_TYPE_OPCODE_BL:
			case MEM_TYPE_OPCODE_LSKP:
			case MEM_TYPE_OPCODE_SKP:
				switch (jumpCorrection[p_Computer->readMem(address)])
				{
					case 1:
						text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
						dcAss.DrawText(text.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
						dcAss.SetTextForeground(colour.Find("BLACK"));
						dcAss.DrawText(text.Mid(6,2), 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
						dcAss.DrawText(text.Mid(18,5), 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
						dcAss.SetTextForeground(colour.Find("FOREST GREEN"));
						dcAss.DrawText(text.Mid(9,2), 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
                        dcAss.DrawText(text.Right(text.Len()-23), 1+CHAR_WIDTH*24, 1+line*LINE_SPACE);
					break;
										
					case 2:
						text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
						dcAss.DrawText(text.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
						dcAss.SetTextForeground(colour.Find("BLACK"));
                        dcAss.DrawText(text.Mid(6,2), 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                        dcAss.DrawText(text.Mid(18,5), 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
						dcAss.SetTextForeground(colour.Find("FOREST GREEN"));
                        dcAss.DrawText(text.Mid(9,2), 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
                        dcAss.DrawText(text.Mid(12,2), 1+CHAR_WIDTH*13, 1+line*LINE_SPACE);
                        dcAss.DrawText(text.Right(text.Len()-23), 1+CHAR_WIDTH*24, 1+line*LINE_SPACE);
					break;
				
					case 3:
                        tempByte = p_Computer->readMem(address+1);
						if ((tempByte&0xf0) == 0x80 || (tempByte&0xf0) == 0x20)
						{
							text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
							dcAss.DrawText(text.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
							dcAss.SetTextForeground(colour.Find("BLACK"));
                            dcAss.DrawText(text.Mid(6,2), 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                            dcAss.DrawText(text.Mid(9,2), 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
                            dcAss.DrawText(text.Mid(18,5), 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
                            dcAss.DrawText(text.Mid(23,3), 1+CHAR_WIDTH*24, 1+line*LINE_SPACE);
                            if ((tempByte&0xf0) == 0x80)
                                dcAss.SetTextForeground(wxColour(200,51,161));
                            else
                                dcAss.SetTextForeground(colour.Find("FOREST GREEN"));
                            dcAss.DrawText(text.Mid(12,2), 1+CHAR_WIDTH*13, 1+line*LINE_SPACE);
                            dcAss.DrawText(text.Mid(15,2), 1+CHAR_WIDTH*16, 1+line*LINE_SPACE);
                            dcAss.DrawText(text.Right(4), 1+CHAR_WIDTH*27, 1+line*LINE_SPACE);
						}
						else
						{
							if (tempByte == 0x3e || tempByte == 0x3f)
							{
								text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
								dcAss.DrawText(text.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
								dcAss.SetTextForeground(colour.Find("BLACK"));
                                dcAss.DrawText(text.Mid(6,2), 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                                dcAss.DrawText(text.Mid(9,2), 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
                                dcAss.DrawText(text.Mid(15,2), 1+CHAR_WIDTH*16, 1+line*LINE_SPACE);
                                dcAss.DrawText(text.Mid(18,5), 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
								dcAss.SetTextForeground(colour.Find("FOREST GREEN"));
								dcAss.DrawText(text.Mid(12,2), 1+CHAR_WIDTH*13, 1+line*LINE_SPACE);
                                dcAss.DrawText(text.Right(5), 1+CHAR_WIDTH*24, 1+line*LINE_SPACE);
							}
							else
							{
								text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
								dcAss.DrawText(text.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
								dcAss.SetTextForeground(colour.Find("BLACK"));
                                dcAss.DrawText(text.Mid(6,2), 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                                dcAss.DrawText(text.Mid(9,2), 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
                                dcAss.DrawText(text.Mid(12,2), 1+CHAR_WIDTH*13, 1+line*LINE_SPACE);
                                dcAss.DrawText(text.Mid(15,2), 1+CHAR_WIDTH*16, 1+line*LINE_SPACE);
                                if (text.Len() >= 23)
                                {
                                    dcAss.DrawText(text.Mid(18,5), 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
                                    dcAss.DrawText(text.Right(text.Len()-23), 1+CHAR_WIDTH*24, 1+line*LINE_SPACE);
                                    
                                }
                                else
                                    dcAss.DrawText(text.Right(text.Len()-18), 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
							}
						}
					break;

					default:
						text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
						dcAss.DrawText(text.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
						dcAss.SetTextForeground(colour.Find("BLACK"));
                        dcAss.DrawText(text.Mid(6,2), 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                        dcAss.DrawText(text.Mid(9,2), 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
                        if (text.Len() >= 23)
                        {
                            dcAss.DrawText(text.Mid(18,5), 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
                            dcAss.DrawText(text.Right(text.Len()-23), 1+CHAR_WIDTH*24, 1+line*LINE_SPACE);
                            
                        }
                        else
                            dcAss.DrawText(text.Right(text.Len()-18), 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
					break;
				}
			break;

			case MEM_TYPE_OPCODE_LBR_SLOT:
				text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
				dcAss.DrawText(text.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
				dcAss.SetTextForeground(colour.Find("BLACK"));
                dcAss.DrawText(text.Mid(6,2), 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(18,5), 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
				dcAss.DrawText(text.Mid(23,4), 1+CHAR_WIDTH*24, 1+line*LINE_SPACE);
				dcAss.SetTextForeground(colour.Find("FOREST GREEN"));
                dcAss.DrawText(text.Mid(9,2), 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(12,2), 1+CHAR_WIDTH*13, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Right(4), 1+CHAR_WIDTH*28, 1+line*LINE_SPACE);
			break;

			case MEM_TYPE_OPCODE_RLDL:
				text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
				dcAss.DrawText(text.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
				dcAss.SetTextForeground(colour.Find("BLACK"));
                dcAss.DrawText(text.Mid(6,2), 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(9,2), 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
				dcAss.DrawText(text.Mid(18,4), 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(23,3), 1+CHAR_WIDTH*24, 1+line*LINE_SPACE);
				dcAss.SetTextForeground(wxColour(200,51,161));
                dcAss.DrawText(text.Mid(12,2), 1+CHAR_WIDTH*13, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(15,2), 1+CHAR_WIDTH*16, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Right(4), 1+CHAR_WIDTH*27, 1+line*LINE_SPACE);
			break;

			case MEM_TYPE_OPCODE_LDV:
                text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
                dcAss.DrawText(text.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
                dcAss.SetTextForeground(colour.Find("BLACK"));
                dcAss.DrawText(text.Mid(6,2), 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(9,2), 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(12,2), 1+CHAR_WIDTH*13, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(18,4), 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(23,7), 1+CHAR_WIDTH*24, 1+line*LINE_SPACE);
				line += 1;
				if (line == EDIT_ROW)
				{
					dcAss.SetFont(exactFontBold);
					dirAssAddress_ = address - 3;
					dcAss.SetTextForeground(colour.Find("BLACK"));
					dcAss.DrawText(">", 1, 1+line*LINE_SPACE);
					dcAss.DrawText("<", ASS_WIDTH-9, 1+EDIT_ROW*LINE_SPACE);
				}
				else
					dcAss.SetFont(exactFont);
				if (line < numberOfDebugLines_)
				{
                    line2.Printf("%02X", p_Computer->readMem(address-3));
                    dcAss.DrawText(line2, 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                    line2.Printf("%02X", p_Computer->readMem(address-2));
                    dcAss.DrawText(line2, 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
                    line2.Printf("%02X", p_Computer->readMem(address-1));
                    dcAss.DrawText(line2, 1+CHAR_WIDTH*13, 1+line*LINE_SPACE);
				}
			break;

			case MEM_TYPE_OPCODE_LDL:
				text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
				dcAss.DrawText(text.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
				dcAss.SetTextForeground(colour.Find("BLACK"));
                dcAss.DrawText(text.Mid(6,2), 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(12,2), 1+CHAR_WIDTH*13, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(18,4), 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(23,3), 1+CHAR_WIDTH*24, 1+line*LINE_SPACE);
				dcAss.SetTextForeground(wxColour(200,51,161));
                dcAss.DrawText(text.Mid(9,2), 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Right(4), 1+CHAR_WIDTH*27, 1+line*LINE_SPACE);
				line += 1;
				if (line == EDIT_ROW)
				{
					dcAss.SetFont(exactFontBold);
					dirAssAddress_ = address - 3;
					dcAss.SetTextForeground(colour.Find("BLACK"));
					dcAss.DrawText(">", 1, 1+line*LINE_SPACE);
					dcAss.DrawText("<", ASS_WIDTH-9, 1+EDIT_ROW*LINE_SPACE);
				}
				else
					dcAss.SetFont(exactFont);
				if (line < numberOfDebugLines_)
				{
					dcAss.SetTextForeground(colour.Find("BLACK"));
                    line2.Printf("%02X", p_Computer->readMem(address-3));
                    dcAss.DrawText(line2, 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                    line2.Printf("%02X", p_Computer->readMem(address-1));
                    dcAss.DrawText(line2, 1+CHAR_WIDTH*13, 1+line*LINE_SPACE);
					dcAss.SetTextForeground(wxColour(200,51,161));
                    line2.Printf("%02X", p_Computer->readMem(address-2));
                    dcAss.DrawText(line2, 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
				}
			break;

			case MEM_TYPE_OPCODE_LDL_SLOT:
				text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
				dcAss.DrawText(text.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
				dcAss.SetTextForeground(colour.Find("BLACK"));
                dcAss.DrawText(text.Mid(6,2), 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(12,2), 1+CHAR_WIDTH*13, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(18,4), 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
                dcAss.DrawText(text.Mid(23,7), 1+CHAR_WIDTH*24, 1+line*LINE_SPACE);
				dcAss.SetTextForeground(wxColour(200,51,161));
                dcAss.DrawText(text.Mid(9,2), 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
				line += 1;
				if (line == EDIT_ROW)
				{
					dcAss.SetFont(exactFontBold);
					dirAssAddress_ = address - 3;
					dcAss.SetTextForeground(colour.Find("BLACK"));
					dcAss.DrawText(">", 1, 1+line*LINE_SPACE);
					dcAss.DrawText("<", ASS_WIDTH-9, 1+EDIT_ROW*LINE_SPACE);
				}
				else
					dcAss.SetFont(exactFont);
				if (line < numberOfDebugLines_)
				{
					dcAss.SetTextForeground(colour.Find("BLACK"));
                    line2.Printf("%02X", p_Computer->readMem(address-3));
                    dcAss.DrawText(line2, 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                    line2.Printf("%02X", p_Computer->readMem(address-1));
                    dcAss.DrawText(line2, 1+CHAR_WIDTH*13, 1+line*LINE_SPACE);
                    dcAss.SetTextForeground(wxColour(200,51,161));
                    line2.Printf("%02X", p_Computer->readMem(address-2));
                    dcAss.DrawText(line2, 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
                    line2.Printf("%02X%02X", p_Computer->readMem(address-5),p_Computer->readMem(address-2));
                    dcAss.DrawText(line2, 1+CHAR_WIDTH*24, 1+line*LINE_SPACE);
				}
			break;

			case MEM_TYPE_OPERAND_LD_3:
                dcAss.SetTextForeground(colour.Find("BLACK"));
				if (p_Computer->readMemDataType((address-3)&0xffff) == MEM_TYPE_OPCODE_LDL_SLOT)
				{
					line2.Printf("%02X%02X", p_Computer->readMem((address-2)&0xffff),p_Computer->readMem(address+1));
                    dcAss.DrawText(line2, 1+CHAR_WIDTH*24, 1+line*LINE_SPACE);
				}
                
                line2.Printf("%02X", p_Computer->readMem(address));
                dcAss.DrawText(line2, 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                line2.Printf("%02X", p_Computer->readMem(address+2));
                dcAss.DrawText(line2, 1+CHAR_WIDTH*13, 1+line*LINE_SPACE);
                line2.Printf("%02X", p_Computer->readMem(address+1));

                if (p_Computer->readMemDataType((address-3)&0xffff) != MEM_TYPE_OPCODE_LDV)
					dcAss.SetTextForeground(wxColour(200,51,161));
                dcAss.DrawText(line2, 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
				address+=3;
				address&=0xffff;
			break;

            case MEM_TYPE_FEL2_1:
                code = fel2Disassemble(address, false, true);
                dcAss.DrawText(code.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
                dcAss.SetTextForeground(colour.Find("STEEL BLUE"));
                dcAss.DrawText(code.Right(code.Len()-6), 57, 1+line*LINE_SPACE);
                switch (p_Computer->readMem(address))
                {
                    case 0xC0:
                    case 0xC4:
                    case 0xCC:
                        if (p_Computer->getChip8Type() != CHIPFEL2)
                            address+=2;
                        else
                            address+=1;
                    break;
                    
                    default:
                        address+=2;
                    break;
                }
                address&=0xffff;
            break;
                
            case MEM_TYPE_CHIP_8_1:
				code = chip8Disassemble(address, false, true);
				dcAss.DrawText(code.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
				dcAss.SetTextForeground(colour.Find("STEEL BLUE"));
				dcAss.DrawText(code.Right(code.Len()-6), 57, 1+line*LINE_SPACE);
				address+=2;
				address&=0xffff;
			break;

			case MEM_TYPE_ST2_1:
				code = st2Disassemble(address, false, true);
				dcAss.DrawText(code.Left(5), 1+CHAR_WIDTH, 1+line*LINE_SPACE);
				dcAss.SetTextForeground(colour.Find("STEEL BLUE"));
				dcAss.DrawText(code.Right(code.Len()-6), 57, 1+line*LINE_SPACE);
				switch (p_Computer->readMem(address))
				{
					case 0xC0:
					case 0xE0:
					case 0xE1:
					case 0xE2:
					case 0xE4:
						address+=1;
					break;

					default:		
						address+=2;
					break;
				}
			break;

			case MEM_TYPE_OPCODE_JUMP_SLOT:
				printBufferAddress.Printf("%04X: ", address);
				dcAss.DrawText(printBufferAddress, 1+CHAR_WIDTH, 1+line*LINE_SPACE);

				dcAss.SetTextForeground(colour.Find("BLACK"));
				printBufferOpcode.Printf("S%02X,", p_Computer->readMemDataType(address+1));
				dcAss.DrawText(printBufferOpcode, 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);

				dcAss.SetTextForeground(wxColour(200,51,161));
				printBufferOpcode.Printf("%02X", p_Computer->readMem(address));
                dcAss.DrawText(printBufferOpcode, 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                printBufferOpcode.Printf("%02X", p_Computer->readMem(address+1));
                dcAss.DrawText(printBufferOpcode, 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
				printBufferOpcode.Printf("%04X", (p_Computer->readMem(address)<<8) + p_Computer->readMem(address+1));
				dcAss.DrawText(printBufferOpcode, 1+CHAR_WIDTH*23, 1+line*LINE_SPACE);
				address+=2;
				address&=0xffff;
			break;

			case MEM_TYPE_JUMP:
				printBufferAddress.Printf("%04X: ", address);
				dcAss.DrawText(printBufferAddress, 1+CHAR_WIDTH, 1+line*LINE_SPACE);

				dcAss.SetTextForeground(wxColour(200,51,161));
                printBufferOpcode.Printf("%02X", p_Computer->readMem(address));
                dcAss.DrawText(printBufferOpcode, 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                printBufferOpcode.Printf("%02X", p_Computer->readMem(address+1));
                dcAss.DrawText(printBufferOpcode, 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
				printBufferOpcode.Printf("%04X", (p_Computer->readMem(address)<<8) + p_Computer->readMem(address+1));
				dcAss.DrawText(printBufferOpcode, 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
				address+=2;
				address&=0xffff;
			break;

			case MEM_TYPE_JUMP_REV:
				printBufferAddress.Printf("%04X: ", address);
				dcAss.DrawText(printBufferAddress, 1+CHAR_WIDTH, 1+line*LINE_SPACE);

				dcAss.SetTextForeground(colour.Find("RED"));
                printBufferOpcode.Printf("%02X", p_Computer->readMem(address));
                dcAss.DrawText(printBufferOpcode, 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                printBufferOpcode.Printf("%02X", p_Computer->readMem(address+1));
                dcAss.DrawText(printBufferOpcode, 1+CHAR_WIDTH*10, 1+line*LINE_SPACE);
				printBufferOpcode.Printf("%04X", (p_Computer->readMem(address+1)<<8) + p_Computer->readMem(address));
				dcAss.DrawText(printBufferOpcode, 1+CHAR_WIDTH*19, 1+line*LINE_SPACE);
				address+=2;
				address&=0xffff;
			break;

/*            case MEM_TYPE_TEXT:
                printBufferAddress.Printf("%04X: ", address);
                dcAss.DrawText(printBufferAddress, 1+CHAR_WIDTH, 1+line*LINE_SPACE);
                
                printBufferOpcode.Printf("%02X", p_Computer->readMem(address));
                dcAss.SetTextForeground(colour.Find("SALMON"));
                dcAss.DrawText(printBufferOpcode, 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
                drawAssCharacter(address, line, 0);
                address++;
                address&=0xffff;
            break;*/
                
            default:
				if (dataViewDump)
				{
					printBufferAddress.Printf("%04X: ", address);
					dcAss.DrawText(printBufferAddress, 1+CHAR_WIDTH, 1+line*LINE_SPACE);
					count = 0;
					memType = p_Computer->readMemDataType(address);
					while (count < 4 && (memType == MEM_TYPE_UNDEFINED || memType == MEM_TYPE_DATA ||  memType == MEM_TYPE_TEXT || memType == MEM_TYPE_ST2_2 || memType == MEM_TYPE_FEL2_2 ||memType == MEM_TYPE_CHIP_8_2 || memType == MEM_TYPE_OPERAND))
					{
						printBufferOpcode.Printf("%02X", p_Computer->readMem(address));
                        switch (memType)
                        {
                            case MEM_TYPE_UNDEFINED:
                                dcAss.SetTextForeground(colour.Find("RED"));
                            break;
                            case MEM_TYPE_TEXT:
                                dcAss.SetTextForeground(colour.Find("SALMON"));
                            break;
                            default:
                                dcAss.SetTextForeground(colour.Find("BLUE"));
                            break;
                        }
                        dcAss.DrawText(printBufferOpcode, 1+CHAR_WIDTH*(7+count*3), 1+line*LINE_SPACE);
						drawAssCharacter(address, line, count);
						address++;
						address&=0xffff;
						count++;
						memType = p_Computer->readMemDataType(address);
					}
				}
				else
				{
					printBufferAddress.Printf("%04X: ", address);
					dcAss.DrawText(printBufferAddress, 1+CHAR_WIDTH, 1+line*LINE_SPACE);

					printBufferOpcode.Printf("%02X", p_Computer->readMem(address));
                    switch (memType)
                    {
                        case MEM_TYPE_UNDEFINED:
                            dcAss.SetTextForeground(colour.Find("RED"));
                            break;
                        case MEM_TYPE_TEXT:
                            dcAss.SetTextForeground(colour.Find("SALMON"));
                            break;
                        default:
                            dcAss.SetTextForeground(colour.Find("BLUE"));
                            break;
                    }
					dcAss.DrawText(printBufferOpcode, 1+CHAR_WIDTH*7, 1+line*LINE_SPACE);
					drawAssCharacter(address, line, 0);
					address++;
					address&=0xffff;
				}
			break;
		}
//		if (line == EDIT_ROW)
//			dcAss.SetFont(exactFont);
	}
	dirAssEnd_ = address;
	dcAss.SelectObject(wxNullBitmap);
	XRCCTRL(*this, "AssBitmap", wxStaticBitmap)->SetBitmap(*assBmp);
	int range = XRCCTRL(*this,"AssRangeType",wxChoice)->GetCurrentSelection();
	if (range == 0)
	{
		printBufferAddress.Printf("%04X", dirAssAddress_);
		XRCCTRL(*this, "AssStartType", wxTextCtrl)->SetValue(printBufferAddress);
	}
}

void DebugWindow::drawAssCharacter(Word address, int line, int count)
{
	int t;
	char bits [9];

	if ((runningComputer_ == COMX) || (runningComputer_ == TMC600) || (runningComputer_ == PECOM))
	{
		for (int i=0; i<9; i++)
		{
			if (runningComputer_ == COMX)
				t = p_Comx->readCramDirect((p_Comx->readMem(address)&0x7f)*16+i);
			else if (runningComputer_ == TMC600)
				t = p_Tmc600->readCramDirect((p_Tmc600->readMem(address)&0xff)*16+i);
			else
				t = p_Pecom->readCramDirect((p_Pecom->readMem(address)&0x7f)*16+i);
			bits[i] = (t & 0x1) << 5;
			bits[i] |= (t & 0x2) << 3;
			bits[i] |= (t & 0x4) << 1;
			bits[i] |= (t & 0x8) >> 1;
			bits[i] |= (t & 0x10) >> 3;
			bits[i] |= (t & 0x20) >> 5;
		}
		wxBitmap character(bits, 6, 9, 1);
		dcAss.DrawBitmap(character, 1+CHAR_WIDTH*(19+count), 5+line*LINE_SPACE, false);
	}
	else
	{
		wxString character;
		Byte byteValue = p_Computer->readMem(address)&0x7f;
        
        if (runningComputer_ ==  STUDIOIV)
        {
            if (byteValue>=0 && byteValue <=9)
                byteValue += 0x30;
            if (byteValue>=0xa && byteValue <=0x23)
                byteValue = byteValue + 0x41 - 0xa;
            switch(byteValue)
            {
                case 0x24: byteValue = '?'; break;
                case 0x25: byteValue = '!'; break;
                case 0x26: byteValue = '@'; break;
                case 0x27: byteValue = '-'; break;
                case 0x28: byteValue = '+'; break;
                case 0x29: byteValue = '%'; break;
                case 0x2a: byteValue = ':'; break;
                case 0x2b: byteValue = '/'; break;
                case 0x2c: byteValue = '$'; break;
                case 0x2d: byteValue = '='; break;
                case 0x2e: byteValue = '#'; break;
                case 0x2f: byteValue = ' '; break;
            }
        }

		character.Printf("%c", byteValue);
		if (byteValue > 32)
			dcAss.DrawText(character, 1+CHAR_WIDTH*(19+count), 1+line*LINE_SPACE);
	}

}

void DebugWindow::onAssEnter(wxCommandEvent&WXUNUSED(event))
{
    if (!computerRunning_)
    {
        assErrorDisplay(DirAssErrorCodes[ERROR_NO_COMPUTER_RUNNING-ERROR_START-1]);
        return;
    }
    
	wxString debugIn, address, error;
	Byte b1, b2, b3=0, b4=0, b5=0, b6=0, b7;
	int count;
	Word addressValue = dirAssAddress_;
	Byte typeOpcode, typeOperand1=MEM_TYPE_OPERAND, typeOperand2=MEM_TYPE_OPERAND, typeOperand3=MEM_TYPE_OPERAND, typeOperand4=MEM_TYPE_OPERAND, typeOperand5=MEM_TYPE_OPERAND;
	int	dataViewCount = 4;

	debugIn = assInputWindowPointer->GetValue();
    debugIn = debugIn.Trim(false);

	if (debugIn.Left(1) == "\"" || debugIn.Left(1) == "'" )
	{
		Byte highLight = 0;
		Byte character;
		if (debugIn.Left(1) == "\"")
			highLight = 0x80;

		for (size_t i=1; i<debugIn.Len(); i++)
		{
			character = debugIn.GetChar(i);
			if (runningComputer_ ==  STUDIOIV)
			{
                switch(character)
                {
                    case '?':character = 0x24; break;
                    case '!':character = 0x25; break;
                    case '@':character = 0x26; break;
                    case '-':character = 0x27; break;
                    case '+':character = 0x28; break;
                    case '%': character = 0x29; break;
                    case ':':character = 0x2a; break;
                    case '/': character = 0x2b; break;
                    case '$': character = 0x2c; break;
                    case '=': character = 0x2d; break;
                    case '#': character = 0x2e; break;
                    case ' ': character = 0x2f; break;
                }
				if (character>=0x30 && character<=0x39)
					character -= 0x30;
				if (character>=0x41 && character<=0x5a)
					character = character-0x41+0xa;
				if (character>=0x61 && character<=0x7a)
					character = character-0x61+0xa;
			}
			character |= highLight;
			p_Computer->writeMem(addressValue, character, true);
			p_Computer->writeMemDataType(addressValue++, MEM_TYPE_TEXT);
			assInputWindowPointer->Clear();
			if (dataViewDump)
			{
				dataViewCount--;
				if (dataViewCount <= 0)
				{
					assSpinDown();
					dataViewCount = 4;
				}
			}
			else
				assSpinDown();
			directAss();
			assErrorDisplay("");
		}
		return;
	}

    if (debugIn.Left(1) == "#")
    {
        long nextByte, maskByte, byteValue;
        int shift;
        wxString seperator, nextByteString;
        debugIn = debugIn.Right(debugIn.Len()-1);
        
        while (debugIn.Len() > 0)
        {
            nextByteString = extractNextWord(&debugIn, &seperator);
            
            if (nextByteString.ToLong(&nextByte, 16))
            {
                maskByte = 0xFF;
                shift = 0;
                while (nextByte > maskByte)
                {
                    maskByte = maskByte << 8;
                    shift += 8;
                    
                    if (maskByte == 0)
                    {
                        assErrorDisplay(DirAssErrorCodes[ERROR_HEX_TO_HIGH-ERROR_START-1]);
                        return;
                    }
                }
                while (shift >= 0)
                {
                    byteValue = nextByte & maskByte;
                    p_Computer->writeMem(addressValue, byteValue >> shift, true);
                    p_Computer->writeMemDataType(addressValue++, MEM_TYPE_DATA);
                    nextByte = nextByte - byteValue;
                    shift -= 8;
                    maskByte = maskByte >> 8;

                    assInputWindowPointer->Clear();
                    if (dataViewDump)
                    {
                        dataViewCount--;
                        if (dataViewCount <= 0)
                        {
                            assSpinDown();
                            dataViewCount = 4;
                        }
                    }
                    else
                        assSpinDown();
                    directAss();
                    assErrorDisplay("");
                }
            }
            else
            {
                assErrorDisplay(DirAssErrorCodes[ERROR_HEX-ERROR_START-1]);
                return;
            }
        }
        return;
    }

	debugIn = debugIn.MakeUpper();

	if (debugIn == "")  return;

    typeOpcode = MEM_TYPE_OPCODE;
    count = assemble(&debugIn, &b1, &b2, &b3, &b4, &b5, &b6, &b7, false);
    if (count == 21)
    {
        count -= 19;
        typeOpcode = MEM_TYPE_JUMP;
        typeOperand1 = MEM_TYPE_DATA;
    }
    if (count == 22)
    {
        count -= 20;
        typeOpcode = MEM_TYPE_JUMP_REV;
        typeOperand1 = MEM_TYPE_DATA;
    }
    if (count >= MEM_TYPE_OPCODE_RSHR)
    {
        switch (count)
        {
            case MEM_TYPE_OPCODE_RSHR:
                typeOpcode = MEM_TYPE_OPCODE_RSHR;
                count = 1;
            break;
            case MEM_TYPE_OPCODE_RSHL:
                typeOpcode = MEM_TYPE_OPCODE_RSHL;
                count = 1;
            break;
            case MEM_TYPE_OPCODE_BPZ:
                typeOpcode = MEM_TYPE_OPCODE_BPZ;
                count = 2;
            break;
            case MEM_TYPE_OPCODE_BGE:
                typeOpcode = MEM_TYPE_OPCODE_BGE;
                count = 2;
            break;
            case MEM_TYPE_OPCODE_BM:
                typeOpcode = MEM_TYPE_OPCODE_BM;
                count = 2;
            break;
            case MEM_TYPE_OPCODE_BL:
                typeOpcode = MEM_TYPE_OPCODE_BL;
                count = 2;
            break;
            case MEM_TYPE_OPCODE_LSKP:
                typeOpcode = MEM_TYPE_OPCODE_LSKP;
                count = 1;
            break;
            case MEM_TYPE_OPCODE_SKP:
                typeOpcode = MEM_TYPE_OPCODE_SKP;
                count = 1;
            break;
            case MEM_TYPE_OPCODE_RLDL:
                typeOpcode = MEM_TYPE_OPCODE_RLDL;
                count = 4;
            break;
            case MEM_TYPE_OPCODE_JUMP_SLOT:
                typeOpcode = MEM_TYPE_OPCODE_JUMP_SLOT;
                typeOperand1 = b3;
                count = 2;
            break;
            case MEM_TYPE_OPCODE_LBR_SLOT:
                typeOpcode = MEM_TYPE_OPCODE_LBR_SLOT;
                typeOperand1 = b2;
                typeOperand2 = MEM_TYPE_OPCODE_LBR_SLOT;
                b2 = b3;
                b3 = b4;
                count = 3;
            break;
            case MEM_TYPE_OPCODE_LDV:
                typeOpcode = MEM_TYPE_OPCODE_LDV;
                typeOperand2 = MEM_TYPE_OPERAND_LD_2;
                typeOperand3 = MEM_TYPE_OPERAND_LD_3;
                typeOperand5 = MEM_TYPE_OPERAND_LD_5;
                count = 6;
            break;
            case MEM_TYPE_OPCODE_LDL:
                typeOpcode = MEM_TYPE_OPCODE_LDL;
                typeOperand2 = MEM_TYPE_OPERAND_LD_2;
                typeOperand3 = MEM_TYPE_OPERAND_LD_3;
                typeOperand5 = MEM_TYPE_OPERAND_LD_5;
                count = 6;
            break;
            case MEM_TYPE_OPCODE_LDL_SLOT:
                typeOpcode = MEM_TYPE_OPCODE_LDL_SLOT;
                typeOperand1 = b4;
                b4 = 0xf8;
                typeOperand2 = MEM_TYPE_OPERAND_LD_2;
                typeOperand3 = MEM_TYPE_OPERAND_LD_3;
                typeOperand5 = MEM_TYPE_OPERAND_LD_5;
                count = 6;
            break;
        }
    }

    if (p_Computer->getChip8Type() != CHIP_NONE && (count == ERROR_INST || count == ERROR_TEMP_PAR || count == ERROR_TEMP_CPU_1801))
    {
        switch (p_Computer->getChip8Type())
        {
            case CHIPST2:
                typeOpcode = MEM_TYPE_ST2_1;
                typeOperand1 = MEM_TYPE_ST2_2;
                count = assembleSt2(&debugIn, &b1, &b2);
            break;
                
            case CHIPFEL1:
            case CHIPFEL2:
            case CHIPFEL3:
            case CHIPSTIV:
                typeOpcode = MEM_TYPE_FEL2_1;
                typeOperand1 = MEM_TYPE_FEL2_2;
                count = assembleFel2(&debugIn, &b1, &b2, &b3, &b4);
            break;
                
            default:
                typeOpcode = MEM_TYPE_CHIP_8_1;
                typeOperand1 = MEM_TYPE_CHIP_8_2;
                count = assembleChip(&debugIn, &b1, &b2);
            break;
        }
    }

	if (count > 0 && count < 7)
	{
		if (p_Computer->readMemDataType(addressValue) == MEM_TYPE_OPERAND_LD_3)
			addressValue -= 3;

		for (int i=addressValue; i<addressValue+count; i++)
		{
			if (p_Computer->readMemDataType(i) == MEM_TYPE_OPCODE_LBR_SLOT)
			{
				for (int j=0; j<3; j++)
				{
					p_Computer->writeMem(i+j, 0xc4, true);
					p_Computer->writeMemDataType(i+j, MEM_TYPE_OPCODE);
				}
			}
			if (p_Computer->readMemDataType(i) == MEM_TYPE_JUMP_REV || p_Computer->readMemDataType(i) == MEM_TYPE_JUMP || p_Computer->readMemDataType(i) == MEM_TYPE_OPCODE_JUMP_SLOT)
			{
				for (int j=0; j<2; j++)
				{
					p_Computer->writeMem(i+j, 0xc4, true);
					p_Computer->writeMemDataType(i+j, MEM_TYPE_OPCODE);
				}
			}
		}

		int checkAddres = setMemLabel(dirAssAddress_, true);
		
		p_Computer->writeMem(addressValue, b1, true);
		p_Computer->writeMemDataType(addressValue++, typeOpcode);

		if (count > 1)
		{
			p_Computer->writeMem(addressValue, b2, true);
			p_Computer->writeMemDataType(addressValue++, typeOperand1);
		}
		if (count > 2) 
		{
			p_Computer->writeMem(addressValue, b3, true);
			p_Computer->writeMemDataType(addressValue++, typeOperand2);
		}
		if (count > 3) 
		{
			p_Computer->writeMem(addressValue, b4, true);
			p_Computer->writeMemDataType(addressValue++, typeOperand3);
		}
		if (count > 4) 
		{
			p_Computer->writeMem(addressValue, b5, true);
			p_Computer->writeMemDataType(addressValue++, typeOperand4);
		}
		if (count > 5) 
		{
			p_Computer->writeMem(addressValue, b6, true);
			p_Computer->writeMemDataType(addressValue++, typeOperand5);
		}
		setMemLabel(dirAssAddress_, false);
        if (checkAddres != -1)
        {
            Byte out1 = getOut1();
            checkBranch(MEM_LABEL_TYPE, checkAddres);
            checkLoadL(MEM_LABEL_TYPE, checkAddres);
            setOut1(out1);
        }
        
/*		while (p_Computer->readMemDataType(addressValue) == MEM_TYPE_OPERAND || p_Computer->readMemDataType(addressValue) == MEM_TYPE_OPERAND_LD_2  || p_Computer->readMemDataType(addressValue) == MEM_TYPE_OPERAND_LD_3  || p_Computer->readMemDataType(addressValue) == MEM_TYPE_OPERAND_LD_5 || p_Computer->readMemDataType(addressValue) == MEM_TYPE_ST2_2 || p_Computer->readMemDataType(addressValue) == MEM_TYPE_FEL2_2 || p_Computer->readMemDataType(addressValue) == MEM_TYPE_CHIP_8_2)
		{
            p_Computer->writeMem(addressValue, 0xc4, true);
            p_Computer->writeMemDataType(addressValue++, MEM_TYPE_OPCODE);
		}*/

        while (p_Computer->readMemDataType(addressValue) == MEM_TYPE_OPERAND || p_Computer->readMemDataType(addressValue) == MEM_TYPE_OPERAND_LD_2  || p_Computer->readMemDataType(addressValue) == MEM_TYPE_OPERAND_LD_3  || p_Computer->readMemDataType(addressValue) == MEM_TYPE_OPERAND_LD_5 || p_Computer->readMemDataType(addressValue) == MEM_TYPE_ST2_2 || p_Computer->readMemDataType(addressValue) == MEM_TYPE_FEL2_2 || p_Computer->readMemDataType(addressValue) == MEM_TYPE_CHIP_8_2)
        {
            p_Computer->writeMem(addressValue, 0, true);
            p_Computer->writeMemDataType(addressValue++, MEM_TYPE_DATA);
        }

        assInputWindowPointer->Clear();
		if (dataViewDump || typeOpcode == MEM_TYPE_OPCODE || typeOpcode == MEM_TYPE_JUMP   || typeOpcode == MEM_TYPE_JUMP_REV || typeOpcode == MEM_TYPE_OPCODE_LDL_SLOT || (typeOpcode >= MEM_TYPE_OPCODE_RSHR && typeOpcode <= MEM_TYPE_OPCODE_LDL))
		{
			assSpinDown();
			if (typeOpcode == MEM_TYPE_OPCODE_LDV || typeOpcode == MEM_TYPE_OPCODE_LDL || typeOpcode == MEM_TYPE_OPCODE_LDL_SLOT)
				assSpinDown();
		}
		else
			for (int i=0; i<count; i++)
				assSpinDown();
		directAss();
//		assErrorDisplay("");
	}
	else
	{
		assErrorDisplay(DirAssErrorCodes[count-ERROR_START-1]);
	}
}

int DebugWindow::setMemLabel(Word labelAddress, bool removeMemLabel)
{
    int branchAddress = -1;
    Byte out1, newBranchMemLabel, newJumpMemLabel, newSubMemLabel;
     
	if (removeMemLabel)
	{
		newBranchMemLabel = LABEL_TYPE_NONE;
		newJumpMemLabel = LABEL_TYPE_NONE;
		newSubMemLabel = LABEL_TYPE_NONE;
	}
	else 
	{
		newBranchMemLabel = LABEL_TYPE_BRANCH;
		newJumpMemLabel = LABEL_TYPE_JUMP;
		newSubMemLabel = LABEL_TYPE_SUB;
	}
	switch(p_Computer->readMemDataType(labelAddress))
	{
		case MEM_TYPE_OPCODE:
		case MEM_TYPE_OPCODE_BPZ:
		case MEM_TYPE_OPCODE_BGE:
		case MEM_TYPE_OPCODE_BM:
		case MEM_TYPE_OPCODE_BL:
			switch(jumpCorrection[p_Computer->readMem(labelAddress)])
            {
                case 1:
                    branchAddress = ((labelAddress+1)&0xff00) + p_Computer->readMem(labelAddress+1);
                    p_Computer->writeMemLabelType(branchAddress, newBranchMemLabel);
                break;
                    
                case 2:
                    branchAddress = (p_Computer->readMem(labelAddress+1)<<8)+p_Computer->readMem(labelAddress+2);
                    p_Computer->writeMemLabelType(branchAddress, newJumpMemLabel);
                    checkSlotAddressWarning(branchAddress);
                break;

                case 3:
                    if (p_Computer->readMem(labelAddress+1) == 0x3E || p_Computer->readMem(labelAddress+1) == 0x3F)
                    {
                        branchAddress = ((labelAddress+2)&0xff00) + p_Computer->readMem(labelAddress+2);
                        p_Computer->writeMemLabelType(branchAddress, newBranchMemLabel);
                    }
                    if ((p_Computer->readMem(labelAddress+1)&0xf0) == 0x20)
                    {
                        branchAddress = (p_Computer->readMem(labelAddress+2)<<8)+p_Computer->readMem(labelAddress+3);
                        p_Computer->writeMemLabelType(branchAddress, newJumpMemLabel);
                        checkSlotAddressWarning(branchAddress);
                    }
                    if ((p_Computer->readMem(labelAddress+1)&0xf0) == 0x80)
                    {
                        branchAddress = (p_Computer->readMem(labelAddress+2)<<8)+p_Computer->readMem(labelAddress+3);
                        p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
                        checkSlotAddressWarning(branchAddress);
                    }
                break;
            }
        break;
	
		case MEM_TYPE_OPCODE_RLDL:
			branchAddress = (p_Computer->readMem(labelAddress+2)<<8)+p_Computer->readMem(labelAddress+3);
			p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
            checkSlotAddressWarning(branchAddress);
		break;
	
		case MEM_TYPE_OPCODE_LDL:
			branchAddress = (p_Computer->readMem(labelAddress+1)<<8)+p_Computer->readMem(labelAddress+4);
			p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
            checkSlotAddressWarning(branchAddress);
		break;
    
		case MEM_TYPE_JUMP:
			branchAddress = (p_Computer->readMem(labelAddress)<<8)+p_Computer->readMem(labelAddress+1);
			p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
            checkSlotAddressWarning(branchAddress);
		break;
    
		case MEM_TYPE_JUMP_REV:
			branchAddress = (p_Computer->readMem(labelAddress+1)<<8)+p_Computer->readMem(labelAddress);
			p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
            checkSlotAddressWarning(branchAddress);
		break;
    
		case MEM_TYPE_OPCODE_JUMP_SLOT:
			out1 = getOut1();
			setOut1(p_Computer->readMemDataType(labelAddress+1));
			branchAddress = (p_Computer->readMem(labelAddress)<<8)+p_Computer->readMem(labelAddress+1);
			p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
			setOut1(out1);
		break;
	
		case MEM_TYPE_OPCODE_LBR_SLOT:
			out1 = getOut1();
			setOut1(p_Computer->readMemDataType(labelAddress+1));
			branchAddress = (p_Computer->readMem(labelAddress+1)<<8)+p_Computer->readMem(labelAddress+2);
			p_Computer->writeMemLabelType(branchAddress, newJumpMemLabel);
			setOut1(out1);
		break;
	
		case MEM_TYPE_OPCODE_LDL_SLOT:
			out1 = getOut1();
			setOut1(p_Computer->readMemDataType(labelAddress+1));
			branchAddress = (p_Computer->readMem(labelAddress+1)<<8)+p_Computer->readMem(labelAddress+4);
			p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
			setOut1(out1);
		break;
	}
	return branchAddress;
}

void DebugWindow::checkSlotAddressWarning(Word branchAddress)
{
	if (slotAddress(branchAddress))
		assErrorDisplay(DirAssErrorCodes[WARNING_MISSING_SLOT_ADDRESS-ERROR_START-1]);
}

bool DebugWindow::slotAddress(Word branchAddress)
{
	switch (runningComputer_)
	{
		case COMX:
			if (branchAddress >= 0xC000 && branchAddress < 0xE000)
				return true;
		break;

		case ELF:
		case ELFII:
		case SUPERELF:
    /*        if (elfConfiguration[runningComputer_].useEms)
                p_Computer->setEmsPage(out1);
            if (elfConfiguration[runningComputer_].useRomMapper)
                p_Computer->setRomMapper(out1);*/
		break;
	}
	return false;
}

void DebugWindow::assErrorDisplay(wxString buffer)
{
	if (lastAssError_ != buffer || assErrorWindowPointer->IsEmpty())
	{
        if (!assErrorWindowPointer->IsEmpty())
            assErrorWindowPointer->AppendText("\n");
        
		lastAssError_ =  buffer;
		assErrorWindowPointer->AppendText(buffer);
	}
}

void DebugWindow::onAssAddress(wxCommandEvent&WXUNUSED(event))
{
	long address = get16BitValue("AssAddress");
	if (address == -1)  return;

	dirAssStart_ = address;

	for (int i=0; i<EDIT_ROW; i++)
		assSpinUp();

	if (xmlLoaded_)
		p_Main->updateAssTab();
}

void DebugWindow::onAssSpinDown(wxSpinEvent&WXUNUSED(event))
{
	if (!computerRunning_)
		return;

	assSpinDown();

	directAss();
	assErrorDisplay("");
}

void DebugWindow::assSpinDown()
{
	int count;
	switch(p_Computer->readMemDataType(dirAssStart_))
	{
		case MEM_TYPE_OPCODE:
		case MEM_TYPE_OPCODE_RSHR:
		case MEM_TYPE_OPCODE_RSHL:
		case MEM_TYPE_OPCODE_BPZ:
		case MEM_TYPE_OPCODE_BGE:
		case MEM_TYPE_OPCODE_BM:
		case MEM_TYPE_OPCODE_BL:
		case MEM_TYPE_OPCODE_LSKP:
		case MEM_TYPE_OPCODE_SKP:
		case MEM_TYPE_OPCODE_RLDL:
			dirAssStart_++;
			dirAssStart_&=0xffff;
			while (p_Computer->readMemDataType(dirAssStart_) == MEM_TYPE_OPERAND)
			{
				dirAssStart_++;
				dirAssStart_&=0xffff;
			}
		break;

		case MEM_TYPE_OPCODE_LDV:
		case MEM_TYPE_OPCODE_LDL:
		case MEM_TYPE_OPCODE_LDL_SLOT:
		case MEM_TYPE_OPERAND_LD_3:
			dirAssStart_ += 3;
			dirAssStart_&=0xffff;
		break;

		case MEM_TYPE_OPCODE_LBR_SLOT:
			dirAssStart_ += 3;
			dirAssStart_&=0xffff;
		break;

		case MEM_TYPE_JUMP:
		case MEM_TYPE_JUMP_REV:
		case MEM_TYPE_OPCODE_JUMP_SLOT:
        case MEM_TYPE_CHIP_8_1:
			dirAssStart_+=2;
			dirAssStart_&=0xffff;
		break;

        case MEM_TYPE_FEL2_1:
            dirAssStart_++;
            dirAssStart_&=0xffff;
            if (p_Computer->readMemDataType(dirAssStart_) == MEM_TYPE_FEL2_2)
            {
                dirAssStart_++;
                dirAssStart_&=0xffff;
            }
        break;

        case MEM_TYPE_ST2_1:
			dirAssStart_++;
			dirAssStart_&=0xffff;
			if (p_Computer->readMemDataType(dirAssStart_) == MEM_TYPE_ST2_2)
			{
				dirAssStart_++;
				dirAssStart_&=0xffff;
			}
		break;

        case MEM_TYPE_DATA:
        case MEM_TYPE_TEXT:
        case MEM_TYPE_UNDEFINED:
			if (dataViewDump)
			{
				count = 0;
				while (count < 4 && (p_Computer->readMemDataType(dirAssStart_) == MEM_TYPE_DATA || p_Computer->readMemDataType(dirAssStart_) == MEM_TYPE_TEXT || p_Computer->readMemDataType(dirAssStart_) == MEM_TYPE_UNDEFINED))
				{
					dirAssStart_++;
					dirAssStart_&=0xffff;
					count++;
				}
			}
			else
			{
				dirAssStart_++;
				dirAssStart_&=0xffff;
			}
		break;

		default:
			dirAssStart_++;
			dirAssStart_&=0xffff;
		break;
	}
}

void DebugWindow::onAssSpinUp(wxSpinEvent&WXUNUSED(event))
{
	if (!computerRunning_)
		return;

	assSpinUp();

	directAss();
	assErrorDisplay("");
}

void DebugWindow::assSpinUp()
{
	if (!computerRunning_)
		return;

	int count;
	dirAssStart_--;
	dirAssStart_&=0xffff;

	if (p_Computer->readMemDataType(dirAssStart_-1) == MEM_TYPE_JUMP_REV || p_Computer->readMemDataType(dirAssStart_-1) == MEM_TYPE_JUMP || p_Computer->readMemDataType(dirAssStart_-1) == MEM_TYPE_OPCODE_JUMP_SLOT)
	{
		dirAssStart_--;
		dirAssStart_&=0xffff;
		return;
	}
	switch(p_Computer->readMemDataType(dirAssStart_))
	{
		case MEM_TYPE_OPERAND:
			while (p_Computer->readMemDataType(dirAssStart_) == MEM_TYPE_OPERAND)
			{
				dirAssStart_--;
				dirAssStart_&=0xffff;
			}
		break;

		case MEM_TYPE_OPERAND_LD_5:
		case MEM_TYPE_OPERAND_LD_2:
		case MEM_TYPE_OPCODE_LBR_SLOT:
			dirAssStart_ -= 2;
			dirAssStart_&=0xffff;
		break;

		case MEM_TYPE_CHIP_8_2:
        case MEM_TYPE_ST2_2:
        case MEM_TYPE_FEL2_2:
			dirAssStart_--;
			dirAssStart_&=0xffff;
		break;

        case MEM_TYPE_DATA:
        case MEM_TYPE_TEXT:
        case MEM_TYPE_UNDEFINED:
			if (dataViewDump)
			{
				count = 0;
				while (count < 3 && (p_Computer->readMemDataType((dirAssStart_-1)&0xFFFF) == MEM_TYPE_DATA || p_Computer->readMemDataType((dirAssStart_-1)&0xFFFF) == MEM_TYPE_TEXT || p_Computer->readMemDataType((dirAssStart_-1)&0xFFFF) == MEM_TYPE_UNDEFINED))
				{
					dirAssStart_--;
					dirAssStart_&=0xffff;
					count++;
				}
			}
		break;
	}
}

void DebugWindow::onAssSpinPageDown(wxSpinEvent&WXUNUSED(event))
{
	if (!computerRunning_)
		return;

	dirAssStart_ = dirAssEnd_;
	dirAssEnd_++;

	directAss();
	assErrorDisplay("");
}

void DebugWindow::onAssSpinPageDown()
{
	dirAssStart_ = dirAssEnd_;
	dirAssEnd_++;

	directAss();
	assErrorDisplay("");
}

void DebugWindow::onAssSpinPageUp(wxSpinEvent&WXUNUSED(event))
{
	if (!computerRunning_)
		return;

	for (int i=0; i<numberOfDebugLines_; i++)
		assSpinUp();

	directAss();
	assErrorDisplay("");
}

void DebugWindow::onAssSpinPageUp()
{
	for (int i=0; i<numberOfDebugLines_; i++)
		assSpinUp();

	directAss();
	assErrorDisplay("");
}

void DebugWindow::onAssMark(wxCommandEvent&WXUNUSED(event))
{
	if (!computerRunning_)
		return;

	long start = get16BitValue("AssStartType");
	if (start == -1)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_NO_ADDRESS-ERROR_START-1]);
		return;
	}

	assErrorDisplay("");
	int range = XRCCTRL(*this,"AssRangeType",wxChoice)->GetCurrentSelection();
	int type = XRCCTRL(*this,"AssType",wxChoice)->GetCurrentSelection();

	if (range == 0)
	{
		if (markType(&start, type) != 0)
            return;
        assSpinDown();
	}
	else
	{
		long end = get16BitValue("AssEndType");
		if (end == -1)
		{
			assErrorDisplay(DirAssErrorCodes[ERROR_NO_ADDRESS-ERROR_START-1]);
			return;
		}
		if (end < start)
		{
			assErrorDisplay(DirAssErrorCodes[ERROR_END_START-ERROR_START-1]);
			return;
		}
		for (long address=start; address<=end; address++)
		{
			if (markType(&address, type) != 0)
			{
				directAss();
				return;
			}
		}
	}
	directAss();
	assErrorDisplay("");
}


void DebugWindow::onAssRangeSpinUp(wxSpinEvent&WXUNUSED(event))
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}
	if (lastRange_ <= 0)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_NO_RANGE-ERROR_START-1]);
	}
	else
	{
		shownRange_++;
		if (shownRange_ >= lastRange_)
			shownRange_ = 0;
		showConfiguration();
	}
}

void DebugWindow::onAssRangeSpinDown(wxSpinEvent&WXUNUSED(event))
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}
	if (lastRange_ <= 0)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_NO_RANGE-ERROR_START-1]);
	}
	else
	{
		shownRange_--;
		if (shownRange_ < 0)
			shownRange_ = lastRange_ - 1;
		showConfiguration();
	}
}

void DebugWindow::showConfiguration()
{
	wxString value;
	value.Printf("%02X",dirAssSlotVector[shownRange_]);
	XRCCTRL(*this,"AssProgramSlot",wxTextCtrl)->ChangeValue(value);
	value.Printf("%04X",dirAssProgramStartVector[shownRange_]);
	XRCCTRL(*this,"AssProgramStart",wxTextCtrl)->ChangeValue(value);
	value.Printf("%04X",dirAssProgramEndVector[shownRange_]);
	XRCCTRL(*this,"AssCodeEnd",wxTextCtrl)->ChangeValue(value);
	value.Printf("%04X",dirAssDataEndVector[shownRange_]);
	XRCCTRL(*this,"AssProgramEnd",wxTextCtrl)->ChangeValue(value);
	value.Printf("%0d:", shownRange_+1);
	XRCCTRL(*this,"AssRangeNumber",wxStaticText)->SetLabel(value);
	XRCCTRL(*this,"AssFileName",wxTextCtrl)->ChangeValue(dirAssFileNameVector[shownRange_]);

	XRCCTRL(*this,"AssStore",wxButton)->Enable(false);
#ifdef __WXMAC__
    XRCCTRL(*this, "AssStore", wxButton)->SetBackgroundColour(wxNullColour);
#else
	XRCCTRL(*this, "AssStore", wxButton)->SetForegroundColour(*wxBLACK);
#endif
    assErrorDisplay("");
}

void DebugWindow::onAssDataView(wxCommandEvent&event)
{
	if (event.GetSelection() == 0)
		dataViewDump = true;
	else
		dataViewDump = false;

	if (!computerRunning_)
		return;

	dirAssStart_ = dirAssAddress_;

	for (int i=0; i<EDIT_ROW; i++)
		assSpinUp();

	directAss();
}

int DebugWindow::markType(long *addrLong, int type)
{
	Word address = (Word)*addrLong;
	int bytes;
	switch (type)
	{
		case 0:
			p_Computer->writeMemDataType(address, MEM_TYPE_OPCODE);
			bytes =	numberOfBytes1802[p_Computer->readMem(address)];
			if (bytes == 0)
				bytes =	numberOfBytes1806[p_Computer->readMem(address+1)];
			address++;
			for (int i=1; i<bytes; i++)
			{
				p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND);
			}
		break;
		case 1:
			if (p_Computer->readMemDataType(address) == MEM_TYPE_JUMP_REV || p_Computer->readMemDataType(address) == MEM_TYPE_JUMP || p_Computer->readMemDataType(address) == MEM_TYPE_OPCODE_JUMP_SLOT)
				p_Computer->writeMemDataType(address++, MEM_TYPE_DATA);
			p_Computer->writeMemDataType(address++, MEM_TYPE_DATA);
		break;
        case 2:
            if (p_Computer->readMem(address) < 0x20)
                p_Computer->writeMemDataType(address, MEM_TYPE_DATA);
            else
                p_Computer->writeMemDataType(address, MEM_TYPE_TEXT);

            if (p_Computer->readMemDataType(address) == MEM_TYPE_JUMP_REV || p_Computer->readMemDataType(address) == MEM_TYPE_JUMP || p_Computer->readMemDataType(address) == MEM_TYPE_OPCODE_JUMP_SLOT)
            {
                address++;
                p_Computer->writeMemDataType(address++, MEM_TYPE_DATA);
            }
        break;
        case 3:
			p_Computer->writeMemDataType(address++, MEM_TYPE_JUMP);
			p_Computer->writeMemDataType(address++, MEM_TYPE_DATA);
		break;
		case 4:
			p_Computer->writeMemDataType(address++, MEM_TYPE_JUMP_REV);
			p_Computer->writeMemDataType(address++, MEM_TYPE_DATA);
		break;
		case 5:
			if (p_Computer->readMem(address) != 0xF8 || (p_Computer->readMem(address+2)&0xF0) != 0xB0 || p_Computer->readMem(address+3) != 0xF8 || (p_Computer->readMem(address+5)&0xF0) != 0xA0)
			{
				assErrorDisplay(DirAssErrorCodes[ERROR_MACRO_NOT_FOUND-ERROR_START-1]);
				return 1;
			}
			p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_LDL);
			p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND);
			p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND_LD_2);
			p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND_LD_3);
			p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND);
			p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND_LD_5);
		break;
		case 6:
			if (p_Computer->readMem(address) != 0xF8 || (p_Computer->readMem(address+2)&0xF0) != 0xB0 || p_Computer->readMem(address+3) != 0xF8 || (p_Computer->readMem(address+5)&0xF0) != 0xA0)
			{
				assErrorDisplay(DirAssErrorCodes[ERROR_MACRO_NOT_FOUND-ERROR_START-1]);
				return 1;
			}
			p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_LDV);
			p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND);
			p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND_LD_2);
			p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND_LD_3);
			p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND);
			p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND_LD_5);
		break;
		case 7:
			switch (p_Computer->readMem(address))
			{
				case 0x76:
					if (p_Computer->readMemDataType(address)== MEM_TYPE_OPCODE_RSHR)
						p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE);
					else
						p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_RSHR);
				break;

				case 0x7e:
					if (p_Computer->readMemDataType(address)== MEM_TYPE_OPCODE_RSHL)
						p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE);
					else
						p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_RSHL);
				break;

				case 0x33:
					if (p_Computer->readMemDataType(address)== MEM_TYPE_OPCODE_BPZ)
						p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_BGE);
					else
					{
						if (p_Computer->readMemDataType(address)== MEM_TYPE_OPCODE_BGE)
							p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE);
						else
							p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_BPZ);
					}
				break;

				case 0x3b:
					if (p_Computer->readMemDataType(address)== MEM_TYPE_OPCODE_BM)
						p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_BL);
					else
					{
						if (p_Computer->readMemDataType(address)== MEM_TYPE_OPCODE_BL)
							p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE);
						else
							p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_BM);
					}
				break;

				case 0xc8:
					if (p_Computer->readMemDataType(address)== MEM_TYPE_OPCODE_LSKP)
						p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE);
					else
						p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_LSKP);
				break;

				case 0x38:
					if (p_Computer->readMemDataType(address)== MEM_TYPE_OPCODE_SKP)
						p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE);
					else
						p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_SKP);
				break;

				case 0x68:
					if ((p_Computer->readMem(address+1)&0xf0) == 0xc0)
					{
						if (p_Computer->readMemDataType(address)== MEM_TYPE_OPCODE_RLDL)
							p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE);
						else
							p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_RLDL);
					}
					else
					{
						assErrorDisplay(DirAssErrorCodes[ERROR_MACRO_NOT_FOUND-ERROR_START-1]);
						return 1;
					}
				break;

				default:
					assErrorDisplay(DirAssErrorCodes[ERROR_MACRO_NOT_FOUND-ERROR_START-1]);
					return 1;
				break;
			}
		break;
		case 8:
			switch (p_Computer->getChip8Type())
			{
				case CHIP8:
				case CHIP8X:
				case CHIPETI:
					p_Computer->writeMemDataType(address++, MEM_TYPE_CHIP_8_1);
					p_Computer->writeMemDataType(address++, MEM_TYPE_CHIP_8_2);
				break;
				
                case CHIPST2:
					p_Computer->writeMemDataType(address, MEM_TYPE_ST2_1);
					switch (p_Computer->readMem(address))
					{
						case 0xC0:
						case 0xE0:
						case 0xE1:
						case 0xE2:
						case 0xE4:
							address++;
						break;

						default:	
							address++;
							p_Computer->writeMemDataType(address++, MEM_TYPE_ST2_2);
						break;
					}
				break;

                case CHIPFEL1:
                case CHIPFEL2:
                case CHIPFEL3:
                case CHIPSTIV:
					p_Computer->writeMemDataType(address, MEM_TYPE_FEL2_1);
					switch (p_Computer->readMem(address))
					{
						case 0xC0:
						case 0xC4:
						case 0xCC:
							address++;
							if (p_Computer->getChip8Type() != CHIPFEL2)
								p_Computer->writeMemDataType(address++, MEM_TYPE_FEL2_2);
						break;
						
						default:
							address++;
							p_Computer->writeMemDataType(address++, MEM_TYPE_FEL2_2);
						break;
					}
				break;
			}
		break;
	}
	Word clearAddress = address;
	while (p_Computer->readMemDataType(clearAddress) == MEM_TYPE_OPERAND || p_Computer->readMemDataType(clearAddress) == MEM_TYPE_ST2_2 ||  p_Computer->readMemDataType(clearAddress) == MEM_TYPE_FEL2_2 || p_Computer->readMemDataType(clearAddress) == MEM_TYPE_CHIP_8_2 || p_Computer->readMemDataType(clearAddress) == MEM_TYPE_OPERAND_LD_2 || p_Computer->readMemDataType(clearAddress) == MEM_TYPE_OPERAND_LD_3 || p_Computer->readMemDataType(clearAddress) == MEM_TYPE_OPERAND_LD_5)
		p_Computer->writeMemDataType(clearAddress++, MEM_TYPE_DATA);
    setMemLabel((Word)*addrLong, false);
    return 0;
}

void DebugWindow::onSaveDebugFile(wxCommandEvent&event)
{
	saveDebugFile_ = event.IsChecked();
}

void DebugWindow::onClearErrorLog(wxCommandEvent&WXUNUSED(event))
{
	assErrorWindowPointer->Clear();
}

Byte DebugWindow::getOut1()
{
	Byte out1 = 0x10;
	switch (runningComputer_)
	{
		case COMX:
			out1 = p_Computer->getOutValue(1);
		break;

		case ELF:
		case ELFII:
		case SUPERELF:
			if (elfConfiguration[runningComputer_].useEms || elfConfiguration[runningComputer_].useRomMapper)
				out1 = p_Computer->getEmsPage();
		break;
	}
	return out1;
}

void DebugWindow::setOut1(Byte out1)
{
	switch (runningComputer_)
	{
		case COMX:
			p_Comx->bankOut(out1);
		break;

		case ELF:
		case ELFII:
		case SUPERELF:
            if (elfConfiguration[runningComputer_].useEms)
                p_Computer->setEmsPage(out1);
            if (elfConfiguration[runningComputer_].useRomMapper)
                p_Computer->setRomMapper(out1);
		break;
	}
}

void DebugWindow::onAssFrom(wxCommandEvent&WXUNUSED(event))
{
    onAssFrom();
    assSpinDown();
    directAss();
}

void DebugWindow::onAssFrom()
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}
	if (!findWorkingRang())
		return;
		
	Byte out1 = getOut1();
    p_Computer->writeMemLabelType(dirAssAddress_, LABEL_TYPE_NONE);
    checkBranch(FIND_BRANCH, 0);
	setOut1(out1);
}

void DebugWindow::onAssFromL(wxCommandEvent&WXUNUSED(event))
{
    onAssFromL();
    assSpinDown();
    directAss();
}

void DebugWindow::onAssFromL()
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}
	if (!findWorkingRang())
		return;

	Byte out1 = getOut1();
    p_Computer->writeMemLabelType(dirAssAddress_, LABEL_TYPE_NONE);
	checkLoadL(FIND_BRANCH, 0);
	setOut1(out1);
}

void DebugWindow::onAssFromV(wxCommandEvent&WXUNUSED(event))
{
    onAssFromV();
    assSpinDown();
    directAss();
}

void DebugWindow::onAssFromV()
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}
	if (!findWorkingRang())
		return;

	Byte out1 = getOut1();
	checkLoadV();
	setOut1(out1);
}

void DebugWindow::onAssFromAll(wxCommandEvent&WXUNUSED(event))
{
    onAssFrom();
    onAssFromL();
    assSpinDown();
    directAss();
}

void DebugWindow::checkBranch(bool function, Word checkAddress)
{
	bool hit = false;
	wxString text;
	int branchAddr;
	Word  foundAddr=0;
	Byte branchType=LABEL_TYPE_NONE;
		
	for (int i=0; i<lastRange_; i++)
	{
		setOut1(dirAssSlotVector[i]);
		for (int addr=dirAssProgramStartVector[i]; addr<=dirAssDataEndVector[i]; addr++)
		{
			branchAddr = -1;
			switch (p_Computer->readMemDataType(addr))
			{
				case MEM_TYPE_OPCODE:
				case MEM_TYPE_OPCODE_BPZ:
				case MEM_TYPE_OPCODE_BGE:
				case MEM_TYPE_OPCODE_BM:
				case MEM_TYPE_OPCODE_BL:
					switch(jumpCorrection[p_Computer->readMem(addr)])
					{
						case 1: // short branch
							branchAddr = ((addr+1)&0xff00)+p_Computer->readMem(addr+1);
							foundAddr = addr;
							addr++;
							branchType = LABEL_TYPE_BRANCH;
						break;
						
						case 2: // long branch
							branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+2);
							foundAddr = addr;
							addr+=2;
							branchType = LABEL_TYPE_JUMP;
						break;

						case 3:
							if ((p_Computer->readMem(addr+1)&0xf0) == 0x20)
							{   // SCAL RN,xxxx + DBNZ
								branchAddr = (p_Computer->readMem(addr+2) << 8) +  p_Computer->readMem(addr+3);
								foundAddr = addr;
								addr+=3;
								branchType = LABEL_TYPE_JUMP;
							}
							if ((p_Computer->readMem(addr+1)&0xf0) == 0x80)
							{   // SCAL RN,xxxx + DBNZ
								branchAddr = (p_Computer->readMem(addr+2) << 8) +  p_Computer->readMem(addr+3);
								foundAddr = addr;
								addr+=3;
								branchType = LABEL_TYPE_SUB;
							}
							if (p_Computer->readMem(addr+1) == 0x3e || p_Computer->readMem(addr+1) == 0x3f)
							{
								branchAddr = ((addr+2)&0xff00)+p_Computer->readMem(addr+2);
								foundAddr = addr;
								addr+=2;
								branchType = LABEL_TYPE_BRANCH;
							}
						break;
					}
					if (!branchChangeNeeded(i, addr, branchAddr))
						branchAddr = -1;
				break;

				case MEM_TYPE_JUMP:
					branchAddr = (p_Computer->readMem(addr) << 8) +  p_Computer->readMem(addr+1);
					if (!branchChangeNeeded(i, addr, branchAddr))
						branchAddr = -1;
					foundAddr = addr;
					addr++;
					branchType = LABEL_TYPE_SUB;
				break;

				case MEM_TYPE_JUMP_REV:
					branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr);
					if (!branchChangeNeeded(i, addr, branchAddr))
						branchAddr = -1;
					foundAddr = addr;
					addr++;
					branchType = LABEL_TYPE_SUB;
				break;

				case MEM_TYPE_OPCODE_JUMP_SLOT:
					branchAddr = (p_Computer->readMem(addr) << 8) +  p_Computer->readMem(addr+1);
					if (!branchChangeNeeded(i, addr, branchAddr))
						branchAddr = -1;
					foundAddr = addr;
					addr++;
					branchType = LABEL_TYPE_SUB;
				break;

				case MEM_TYPE_OPCODE_LBR_SLOT:
					branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+2);
					if (!branchChangeNeeded(i, addr, branchAddr))
						branchAddr = -1;
					foundAddr = addr;
					addr+=2;
					branchType = LABEL_TYPE_JUMP;
				break;
			}

			if (function == MEM_LABEL_TYPE)
			{
				if (branchAddr == checkAddress)
				{
					if (branchType > p_Computer->readMemLabelType(branchAddr))
						p_Computer->writeMemLabelType(branchAddr, branchType);
				}
			}
			else 
			{
				if (branchAddr == dirAssAddress_)
				{
                    if (branchType > p_Computer->readMemLabelType(branchAddr))
                        p_Computer->writeMemLabelType(branchAddr, branchType);
                    if (!hit)
					{
						text.Printf("Locations with branches to %04X:", dirAssAddress_);
						assErrorDisplay(text);
					}
					hit = true;

					switch (runningComputer_)
					{
						case COMX:
							if (addr >= 0xC000 && addr <= 0xDFFF)
								text.Printf("%04X (Slot: %02X)", foundAddr, dirAssSlotVector[i]);
							else
								text.Printf("%04X", foundAddr);
						break;

						case ELF:
						case ELFII:
						case SUPERELF:
							if (elfConfiguration[runningComputer_].useEms)
							{
								if (addr >= 0x8000 && addr <= 0xBFFF)
									text.Printf("%04X (Page: %02X)", foundAddr, dirAssSlotVector[i]);
								else
									text.Printf("%04X", foundAddr);
							}
							else
								text.Printf("%04X", foundAddr);
							if (elfConfiguration[runningComputer_].useRomMapper)
							{
								if (addr >= 0x8000 && addr <= 0xFFFF)
									text.Printf("%04X (Map: %02X)", foundAddr, dirAssSlotVector[i]);
								else 
									text.Printf("%04X", foundAddr);
							}
							else
								text.Printf("%04X", foundAddr);
						break;

						default:
							text.Printf("%04X", foundAddr);
						break;
					}
					assErrorDisplay(text);
				}
			}
		}
	}
	if (!hit && function != MEM_LABEL_TYPE)
		assErrorDisplay("No branches found");
}

void DebugWindow::checkLoadL(bool function, Word checkAddress)
{
	bool hit = false;
	wxString text, leader;
	int loadAddr, foundAddr=-1;
	Byte branchType=LABEL_TYPE_NONE;
	
	for (int i=0; i<lastRange_; i++)
	{
		setOut1(dirAssSlotVector[i]);
		for (int addr=dirAssProgramStartVector[i]; addr<=dirAssDataEndVector[i]; addr++)
		{
			loadAddr = -1;
			foundAddr = -1;
			leader = "";

			switch (p_Computer->readMemDataType(addr))
			{
				case MEM_TYPE_OPCODE_LDL:
					loadAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+4);
					if (!branchChangeNeeded(i, addr, loadAddr))
						loadAddr = -1;
					leader ="LDL: ";
					if (loadAddr == dirAssAddress_)
						foundAddr = addr;
					addr+=5;
					branchType = LABEL_TYPE_SUB;
				break;

				case MEM_TYPE_OPCODE_LDL_SLOT:
					loadAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+4);
					if (!branchChangeNeeded(i, addr, loadAddr))
						loadAddr = -1;
					leader ="LDL: ";
					if (loadAddr == dirAssAddress_)
						foundAddr = addr;
					addr+=5;
					branchType = LABEL_TYPE_SUB;
				break;

				case MEM_TYPE_OPCODE_RLDL:
					loadAddr = (p_Computer->readMem(addr+2) << 8) +  p_Computer->readMem(addr+3);
					if (!branchChangeNeeded(i, addr, loadAddr))
						loadAddr = -1;
					leader ="RLDL: ";
					if (loadAddr == dirAssAddress_)
						foundAddr = addr;
					addr+=3;
					branchType = LABEL_TYPE_SUB;
				break;
			}

			if (function == MEM_LABEL_TYPE)
			{
				if (loadAddr == checkAddress)
				{
					if (branchType > p_Computer->readMemLabelType(loadAddr))
						p_Computer->writeMemLabelType(loadAddr, branchType);
				}
			}
			else 
			{
				if (foundAddr != -1)
				{
                    if (branchType > p_Computer->readMemLabelType(loadAddr))
                        p_Computer->writeMemLabelType(loadAddr, branchType);
                    if (!hit)
					{
						text.Printf("Locations loading 'location' %04X:", dirAssAddress_);
						assErrorDisplay(text);
					}
					hit = true;

					switch (runningComputer_)
					{
						case COMX:
							if (addr >= 0xC000 && addr <= 0xDFFF)
								text.Printf(leader+"%04X (Slot: %02X)", foundAddr, dirAssSlotVector[i]);
							else
								text.Printf(leader+"%04X", foundAddr);
						break;

						case ELF:
						case ELFII:
						case SUPERELF:
							if (elfConfiguration[runningComputer_].useEms)
							{
								if (addr >= 0x8000 && addr <= 0xBFFF)
									text.Printf(leader+"%04X (Page: %02X)", foundAddr, dirAssSlotVector[i]);
								else
									text.Printf(leader+"%04X", foundAddr);
							}
							else
								text.Printf(leader+"%04X", foundAddr);
							if (elfConfiguration[runningComputer_].useRomMapper)
							{
								if (addr >= 0x8000 && addr <= 0xFFFF)
									text.Printf(leader+"%04X (Page: %02X)", foundAddr, dirAssSlotVector[i]);
								else
									text.Printf(leader+"%04X", foundAddr);
							}
							else
								text.Printf(leader+"%04X", foundAddr);
						break;

						default:
							text.Printf(leader+"%04X", foundAddr);
						break;
					}

					assErrorDisplay(text);
				}
			}
		}
	}
	if (!hit && function != MEM_LABEL_TYPE)
		assErrorDisplay("No loads found");
}


void DebugWindow::checkLoadV()
{
	bool hit = false;
	wxString text, leader;
	int loadAddr, foundAddr=-1;

	for (int i=0; i<lastRange_; i++)
	{
		setOut1(dirAssSlotVector[i]);
		for (int addr=dirAssProgramStartVector[i]; addr<=dirAssDataEndVector[i]; addr++)
		{
			loadAddr = -1;
			foundAddr = -1;
			leader = "";

			switch (p_Computer->readMemDataType(addr))
			{
				case MEM_TYPE_OPCODE:
					if (p_Computer->readMem(addr) == 0xf8 && (p_Computer->readMem(addr+2)&0xf0) == 0xb0 && p_Computer->readMem(addr+3) == 0xf8 && (p_Computer->readMem(addr+5)&0xf0) == 0xa0 && (p_Computer->readMem(addr+5)&0xf) == (p_Computer->readMem(addr+2)&0xf))
					{
						loadAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+4);
						if (!branchChangeNeeded(i, addr, loadAddr))
							loadAddr = -1;
						leader ="LD H-L: ";
						if (loadAddr == dirAssAddress_)
						{
							foundAddr = addr;
							addr+=5;
						}
					}

					if (p_Computer->readMem(addr) == 0xf8 && (p_Computer->readMem(addr+2)&0xf0) == 0xa0 && p_Computer->readMem(addr+3) == 0xf8 && (p_Computer->readMem(addr+5)&0xf0) == 0xb0 && (p_Computer->readMem(addr+5)&0xf) == (p_Computer->readMem(addr+2)&0xf) && foundAddr == -1)
					{
						loadAddr = (p_Computer->readMem(addr+4) << 8) +  p_Computer->readMem(addr+1);
						if (!branchChangeNeeded(i, addr, loadAddr))
							loadAddr = -1;
						leader ="LD L-H: ";
						if (loadAddr == dirAssAddress_)
						{
							foundAddr = addr;
							addr+=5;
						}
					}

					if (p_Computer->readMem(addr) == 0x68 && (p_Computer->readMem(addr+1)&0xf0) == 0xc0 && foundAddr == -1)
					{
						loadAddr = (p_Computer->readMem(addr+2) << 8) +  p_Computer->readMem(addr+3);
						if (!branchChangeNeeded(i, addr, loadAddr))
							loadAddr = -1;
						leader ="RLDI: ";
						if (loadAddr == dirAssAddress_)
						{
							foundAddr = addr;
							addr+=3;
						}
					}

					if (p_Computer->readMem(addr) == 0xf8 && (p_Computer->readMem(addr+2)&0xf0) == 0xa0 && foundAddr == -1)
					{
						loadAddr = p_Computer->readMem(addr+1);
						if (!branchChangeNeeded(i, addr, loadAddr))
							loadAddr = -1;
						leader ="LD L: ";
						if (loadAddr == (dirAssAddress_ & 0xff))
						{
							foundAddr = addr;
							addr+=1;
						}
					}

					if (p_Computer->readMem(addr) == 0xf8 && (p_Computer->readMem(addr+2)&0xf0) == 0xb0 && foundAddr == -1)
					{
						loadAddr = p_Computer->readMem(addr+1);
						if (!branchChangeNeeded(i, addr, loadAddr))
							loadAddr = -1;
						leader ="LD H: ";
						if (loadAddr == ((dirAssAddress_ >> 8) & 0xff))
						{
							foundAddr = addr;
							addr+=1;
						}
					}
				break;

				case MEM_TYPE_OPCODE_LDV:
					loadAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+4);
					if (!branchChangeNeeded(i, addr, loadAddr))
						loadAddr = -1;
					leader ="LDV: ";
					if (loadAddr == dirAssAddress_)
						foundAddr = addr;
					addr+=5;
				break;

				case MEM_TYPE_OPCODE_LDL_SLOT:
					loadAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+4);
					if (!branchChangeNeeded(i, addr, loadAddr))
						loadAddr = -1;
					leader ="LDL: ";
					if (loadAddr == dirAssAddress_)
						foundAddr = addr;
					addr+=5;
				break;

				case MEM_TYPE_OPCODE_LDL:
					loadAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+4);
					if (!branchChangeNeeded(i, addr, loadAddr))
						loadAddr = -1;
					leader ="LDL: ";
					if (loadAddr == dirAssAddress_)
						foundAddr = addr;
					addr+=5;
				break;

				case MEM_TYPE_OPCODE_RLDL:
					loadAddr = (p_Computer->readMem(addr+2) << 8) +  p_Computer->readMem(addr+3);
					if (!branchChangeNeeded(i, addr, loadAddr))
						loadAddr = -1;
					leader ="RLDL: ";
					if (loadAddr == dirAssAddress_)
						foundAddr = addr;
					addr+=3;
				break;
			}

			if (foundAddr != -1)
			{
				if (!hit)
				{
					text.Printf("Locations loading 'value' or 'location' %04X:", dirAssAddress_);
					assErrorDisplay(text);
				}
				hit = true;

				switch (runningComputer_)
				{
					case COMX:
						if (addr >= 0xC000 && addr <= 0xDFFF)
							text.Printf(leader+"%04X (Slot: %02X)", foundAddr, dirAssSlotVector[i]);
						else
							text.Printf(leader+"%04X", foundAddr);
					break;

					case ELF:
					case ELFII:
					case SUPERELF:
						if (elfConfiguration[runningComputer_].useEms)
						{
							if (addr >= 0x8000 && addr <= 0xBFFF)
								text.Printf(leader+"%04X (Page: %02X)", foundAddr, dirAssSlotVector[i]);
							else
								text.Printf(leader+"%04X", foundAddr);
						}
						else
							text.Printf(leader+"%04X", foundAddr);
                        if (elfConfiguration[runningComputer_].useRomMapper)
                        {
                            if (addr >= 0x8000 && addr <= 0xFFFF)
                                text.Printf(leader+"%04X (Page: %02X)", foundAddr, dirAssSlotVector[i]);
                            else
                                text.Printf(leader+"%04X", foundAddr);
                        }
                        else
                            text.Printf(leader+"%04X", foundAddr);
					break;

					default:
						text.Printf(leader+"%04X", foundAddr);
					break;
				}

				assErrorDisplay(text);
			}
		}
	}
	if (!hit)
		assErrorDisplay("No loads found");
}

bool DebugWindow::findWorkingRang()
{
	workingRange_ = -1;
	for (int i=0; i<lastRange_; i++)
	{
		if (dirAssAddress_ >= dirAssProgramStartVector[i] && dirAssAddress_ < dirAssDataEndVector[i])
		{
			switch (runningComputer_)
			{
				case COMX:
					if (dirAssAddress_ >= 0xC000 && dirAssAddress_ <= 0xDFFF)
					{
						if (p_Computer->getOutValue(1) ==  dirAssSlotVector[i])
							workingRange_ = i;
					}
					else
						workingRange_ = i;
				break;

				case ELF:
				case ELFII:
				case SUPERELF:
                    if (elfConfiguration[runningComputer_].useEms)
                    {
                        if (dirAssAddress_ >= 0x8000 && dirAssAddress_ <= 0xBFFF)
                        {
                            if (p_Computer->getEmsPage() ==  dirAssSlotVector[i])
                                workingRange_ = i;
                        }
                        else
                            workingRange_ = i;
                    }
                    else
                        workingRange_ = i;
                    if (elfConfiguration[runningComputer_].useRomMapper)
                    {
                        if (dirAssAddress_ >= 0x8000 && dirAssAddress_ <= 0xFFFF)
                        {
                            if (p_Computer->getEmsPage() ==  dirAssSlotVector[i])
                                workingRange_ = i;
                        }
                        else
                            workingRange_ = i;
                    }
                    else
                        workingRange_ = i;
				break;

				default:
					workingRange_ = i;
				break;
			}
		}
	}
	if (workingRange_ == -1)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_DEBUG_ADDRESS-ERROR_START-1]);
		return false;
	}
	return true;
}

void DebugWindow::onInsert(wxCommandEvent&WXUNUSED(event))
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}
	if (lastRange_ <= 0)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_NO_RANGE-ERROR_START-1]);
		return;
	}

	if (!findWorkingRang())
		return;

	for (int i=dirAssProgramStartVector[workingRange_]; i<=dirAssDataEndVector[workingRange_]; i++)
	{
		branchAddressTable[i] = 0;
		branchAddressTableCorrection[i] = false;
	}

	if (p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_DATA || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_TEXT || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_ST2_1 || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_CHIP_8_1 || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_FEL2_1)
		insertByte(dirAssAddress_, 0, -1);
	else
		insertByte(dirAssAddress_, 0xc4, -1);
	shortLongBranch();
	correctionList();
	directAss();
	XRCCTRL(*this, "AssInputWindow", wxTextCtrl)->SetFocus();
}

void DebugWindow::insertByte(Word insertAddress, Byte instruction, int branchAddress)
{
//	if (insertAddress > dirAssDataEndVector[workingRange_] || insertAddress < dirAssProgramStartVector[workingRange_])
//		assErrorDisplay(DirAssErrorCodes[ERROR_DEBUG_ADDRESS-ERROR_START-1]);
	Byte chip8_instruction;
	Word endAddr;

	if (insertAddress > dirAssProgramEndVector[workingRange_])
		endAddr = dirAssDataEndVector[workingRange_];
	else
		endAddr = dirAssProgramEndVector[workingRange_];

	Word lastUsedAddr = endAddr;
	for (int addr=endAddr; addr>insertAddress;addr--)
	{
		if (p_Computer->readMem(addr) == 0 && addr == lastUsedAddr)
			lastUsedAddr = addr-1;

		p_Computer->writeMem(addr, p_Computer->readMem(addr-1), true);
		p_Computer->writeMemDataType(addr, p_Computer->readMemDataType(addr - 1));
		if (addr == insertAddress + 1)
			p_Computer->writeMemLabelType(addr, LABEL_TYPE_NONE);
		else
			p_Computer->writeMemLabelType(addr, p_Computer->readMemLabelType(addr - 1));
		branchAddressTable[addr] = branchAddressTable[addr-1];
		branchAddressTableCorrection[addr] = branchAddressTableCorrection[addr-1];
	}

	if (lastUsedAddr >= endAddr - 17)
		assErrorDisplay(DirAssErrorCodes[ERROR_MEMORY_WARNING-ERROR_START-1]);

	p_Computer->writeMem(insertAddress, instruction, true);
	if (instruction != 0)
		p_Computer->writeMemDataType(insertAddress, MEM_TYPE_OPCODE);
	else
		p_Computer->writeMemDataType(insertAddress, MEM_TYPE_DATA);

	if (branchAddress >= 0)
	{
		p_Computer->writeMem(insertAddress+1, (branchAddress&0xff00)>>8, true);
		p_Computer->writeMemDataType(insertAddress+1, MEM_TYPE_OPERAND);
		p_Computer->writeMem(insertAddress+2, branchAddress&0xff, true);
		p_Computer->writeMemDataType(insertAddress+2, MEM_TYPE_OPERAND);
	}

	Word branchAddr;

	for (int addr=dirAssProgramStartVector[workingRange_]; addr<=dirAssProgramEndVector[workingRange_]; addr++)
	{
		if (branchAddressTable[addr]!=0)
		{
			if (branchAddressTable[addr]>insertAddress)
				branchAddressTable[addr]++;
		}
		switch (p_Computer->readMemDataType(addr))
		{
			case MEM_TYPE_OPCODE:
			case MEM_TYPE_OPCODE_BPZ:
			case MEM_TYPE_OPCODE_BGE:
			case MEM_TYPE_OPCODE_BM:
			case MEM_TYPE_OPCODE_BL:
				switch(jumpCorrection[p_Computer->readMem(addr)])
				{
					case 1: // short branch
						if (insertAddress>addr)
							branchAddr = ((addr+1)&0xff00)+p_Computer->readMem(addr+1);
						else
							branchAddr = (addr&0xff00)+p_Computer->readMem(addr+1);
						if (branchAddr<=insertAddress)
						{
							if ((addr&0xff)==0xff && addr >= insertAddress)
							{
								changeBranch(addr, branchAddr);
							}
						}
						else
						{
							p_Computer->writeMem(addr+1, p_Computer->readMem(addr+1)+1, true);
							if ((addr&0xff)==0xff)
							{
								changeBranch(addr, branchAddr+1);
							}
							else
							{
								if ((branchAddr&0xff)==0xff)
								{
									changeBranch(addr, branchAddr+1);
								}
							}
						}
					break;

					case 2: // long branch
						branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+2);
						if (branchAddr>insertAddress && branchAddr < endAddr)
						{
							branchAddr++;
							p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
							p_Computer->writeMem(addr+2, branchAddr&0xff, true);
						}
					break;

					case 3: 
						if ((p_Computer->readMem(addr+1)&0xf0) == 0x80 || (p_Computer->readMem(addr+1)&0xf0) == 0x20)
						{   // SCAL RN,xxxx + DBNZ
							branchAddr = (p_Computer->readMem(addr+2) << 8) +  p_Computer->readMem(addr+3);
							if (branchAddr>insertAddress && branchAddr < endAddr)
							{
								branchAddr++;
								p_Computer->writeMem(addr+2, (branchAddr&0xff00)>>8, true);
								p_Computer->writeMem(addr+3, branchAddr&0xff, true);
							}
						}
						if (p_Computer->readMem(addr+1) == 0x3e || p_Computer->readMem(addr+1) == 0x3f)
						{
							if (insertAddress>addr)
								branchAddr = ((addr+2)&0xff00)+p_Computer->readMem(addr+2);
							else
								branchAddr = ((addr+1)&0xff00)+p_Computer->readMem(addr+2);
							if (branchAddr<=insertAddress)
							{
								if (((addr+1)&0xff)==0xff && addr >= insertAddress)
								{
									changeBranch(addr, branchAddr);
								}
							}
							else
							{
								p_Computer->writeMem(addr+2, p_Computer->readMem(addr+2)+1, true);
								if (((addr+1)&0xff)==0xff)
								{
									changeBranch(addr, branchAddr+1);
								}
								else
								{
									if ((branchAddr&0xff)==0xff)
									{
										changeBranch(addr, branchAddr+1);
									}
								}
							}
						}
					break;
				}
			break;
		
			case MEM_TYPE_JUMP:
				branchAddr = (p_Computer->readMem(addr) << 8) +  p_Computer->readMem(addr+1);
				if (branchAddr>insertAddress && branchAddr < endAddr)
				{
					branchAddr++;
					p_Computer->writeMem(addr, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr+1, branchAddr&0xff, true);
				}
				addr++;
			break;
		
			case MEM_TYPE_JUMP_REV:
				branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr);
				if (branchAddr>insertAddress && branchAddr < endAddr)
				{
					branchAddr++;
					p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr, branchAddr&0xff, true);
				}
				addr++;
			break;
		
			case MEM_TYPE_OPCODE_JUMP_SLOT:
				branchAddr = (p_Computer->readMem(addr) << 8) +  p_Computer->readMem(addr+1);
				if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
				{
					branchAddr++;
					p_Computer->writeMem(addr, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr+1, branchAddr&0xff, true);
				}
				addr++;
			break;

			case MEM_TYPE_OPCODE_LBR_SLOT:
				branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+2);
				if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
				{
					branchAddr++;
					p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr+2, branchAddr&0xff, true);
				}
				addr+=2;
			break;
		
			case MEM_TYPE_OPCODE_LDL:
				branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+4);
				if (branchAddr>insertAddress && branchAddr < endAddr)
				{
					branchAddr++;
					p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr+4, branchAddr&0xff, true);
				}
				addr+=5;
			break;

			case MEM_TYPE_OPCODE_LDL_SLOT:
				branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+4);
				if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
				{
					branchAddr++;
					p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr+4, branchAddr&0xff, true);
				}
				addr+=5;
			break;

			case  MEM_TYPE_OPCODE_RLDL:
				branchAddr = (p_Computer->readMem(addr+2) << 8) +  p_Computer->readMem(addr+3);
				if (branchAddr>insertAddress && branchAddr < endAddr)
				{
					branchAddr++;
					p_Computer->writeMem(addr+2, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr+3, branchAddr&0xff, true);
				}
				addr+=3;
			break;

            case MEM_TYPE_FEL2_1:
				switch (p_Computer->getChip8Type())
				{
					case CHIPFEL1:
					case CHIPFEL2:
					case CHIPFEL3:
						chip8_instruction = p_Computer->readMem(addr)&0xf0;
						switch (chip8_instruction)
						{
							case 0:
							case 0x10:
							case 0xF0:
								branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
								if (branchAddr>insertAddress && branchAddr < endAddr)
								{
									branchAddr++;
									p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
									p_Computer->writeMem(addr+1, branchAddr&0xff, true);
								}
								addr++;
							break;
						}
					break;
					
					case CHIPSTIV:
						chip8_instruction = p_Computer->readMem(addr);
                        if ((chip8_instruction & 0xf0) == 0x0 || (chip8_instruction & 0xf0) == 0x10 || (chip8_instruction & 0xf0) == 0x20 || (chip8_instruction & 0xf0) == 0x30 || (chip8_instruction & 0xf0) == 0x80 || (chip8_instruction & 0xf0) == 0x90)
							chip8_instruction &= 0xf0;
						switch (chip8_instruction)
						{
                            case 0x0:
                            case 0x10:
                            case 0x20:
                            case 0x30:
							case 0x61:
							case 0x66:
							case 0x67:
							case 0x68:
								branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
								if (branchAddr>insertAddress && branchAddr < endAddr)
								{
									branchAddr++;
									p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
									p_Computer->writeMem(addr+1, branchAddr&0xff, true);
								}
								addr++;
							break;

							case 0x65:
							case 0x80:
							case 0x90:
                                if (insertAddress>addr)
                                    branchAddr = ((addr+1)&0xff00)+p_Computer->readMem(addr+1);
                                else
                                    branchAddr = (addr&0xff00)+p_Computer->readMem(addr+1);
                                if (branchAddr<=insertAddress)
                                {
                                    if ((addr&0xff)==0xff && addr >= insertAddress)
                                    {
                                        branchAddressTableCorrection[addr] = true;
                                    }
                                }
                                else
                                {
                                    p_Computer->writeMem(addr+1, p_Computer->readMem(addr+1)+1, true);
                                    if ((addr&0xff)==0xff)
                                    {
                                        branchAddressTableCorrection[addr] = true;
                                    }
                                    else
                                    {
                                        if ((branchAddr&0xff)==0xff)
                                        {
                                            branchAddressTableCorrection[addr] = true;
                                        }
                                    }
                                }
							break;
						}
					break;
				}
            break;
                
            case MEM_TYPE_CHIP_8_1:
				chip8_instruction = p_Computer->readMem(addr)&0xf0;
				switch (chip8_instruction)
				{
					case 0:
					case 0x10:
					case 0x20:
					case 0xA0:
					case 0xB0:
						branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
						if (branchAddr>insertAddress && branchAddr < endAddr)
						{
							branchAddr++;
							p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
							p_Computer->writeMem(addr+1, branchAddr&0xff, true);
						}
						addr++;
					break;
				}
			break;

			case MEM_TYPE_ST2_1:
				chip8_instruction = p_Computer->readMem(addr)&0xf0;
				switch (chip8_instruction)
				{
					case 0:    // SYS aaa
					case 0x10: // JP aaa
					case 0x20: // CALL aaa
					case 0xA0: // LD I,aaa
						branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
						if (branchAddr>insertAddress && branchAddr < endAddr)
						{
							branchAddr++;
							p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
							p_Computer->writeMem(addr+1, branchAddr&0xff, true);
						}
						addr++;
					break;

					case 0x30: // JNZ Vx, kk
					case 0x40: // JZ Vx, kk
					case 0xD0: // JKP n, kk  and  JKP VB, kk
						if (insertAddress>addr)
							branchAddr = ((addr+1)&0xff00)+p_Computer->readMem(addr+1);
						else
							branchAddr = (addr&0xff00)+p_Computer->readMem(addr+1);
						if (branchAddr<=insertAddress)
						{
							if ((addr&0xff)==0xff && addr >= insertAddress)
							{
								branchAddressTableCorrection[addr] = true;
							}
						}
						else
						{
							p_Computer->writeMem(addr+1, p_Computer->readMem(addr+1)+1, true);
							if ((addr&0xff)==0xff)
							{
								branchAddressTableCorrection[addr] = true;
							}
							else
							{
								if ((branchAddr&0xff)==0xff)
								{
									branchAddressTableCorrection[addr] = true;
								}
							}
						}
					break;
				}
				switch (p_Computer->readMem(addr))
				{
					case 0x70: // DJNZ V0, kk
					case 0xE8: // SP DRW, JC, kk  and  SP DRW, kk
						if (insertAddress>addr)
							branchAddr = ((addr+1)&0xff00)+p_Computer->readMem(addr+1);
						else
							branchAddr = (addr&0xff00)+p_Computer->readMem(addr+1);
						if (branchAddr<=insertAddress)
						{
							if ((addr&0xff)==0xff && addr >= insertAddress)
							{
								branchAddressTableCorrection[addr] = true;
							}
						}
						else
						{
							p_Computer->writeMem(addr+1, p_Computer->readMem(addr+1)+1, true);
							if ((addr&0xff)==0xff)
							{
								branchAddressTableCorrection[addr] = true;
							}
							else
							{
								if ((branchAddr&0xff)==0xff)
								{
									branchAddressTableCorrection[addr] = true;
								}
							}
						}
					break;
				}
			break;
		}
	}

	Byte out1=getOut1();

	int loopStart, loopEnd;

	for (int i=0; i<lastRange_; i++)
	{
		if (i != workingRange_) 
		{
			loopStart = dirAssProgramStartVector[i];
			loopEnd = dirAssDataEndVector[i];
		}
		else
		{
			loopStart = dirAssProgramEndVector[i]+1;
			loopEnd = dirAssDataEndVector[i];
		}

		setOut1(dirAssSlotVector[i]);
		for (int addr=loopStart; addr<=loopEnd; addr++)
		{
			switch (p_Computer->readMemDataType(addr))
			{
				case MEM_TYPE_OPCODE:
				case MEM_TYPE_OPCODE_LBR_SLOT:
					switch(jumpCorrection[p_Computer->readMem(addr)])
					{
						case 2: // long branch
							branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+2);
							if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
							{
								branchAddr++;
								p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
								p_Computer->writeMem(addr+2, branchAddr&0xff, true);
							}
							addr +=2;
						break;

						case 3: // SCAL RN,xxxx + DBNZ
							if ((p_Computer->readMem(addr+1)&0xf0) == 0x80 || (p_Computer->readMem(addr+1)&0xf0) == 0x20)
							{
								branchAddr = (p_Computer->readMem(addr+2) << 8) +  p_Computer->readMem(addr+3);
								if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
								{
									branchAddr++;
									p_Computer->writeMem(addr+2, (branchAddr&0xff00)>>8, true);
									p_Computer->writeMem(addr+3, branchAddr&0xff, true);
								}
							}
							addr +=3;
						break;
					}
				break;
			
				case MEM_TYPE_JUMP:
				case MEM_TYPE_OPCODE_JUMP_SLOT:
					branchAddr = (p_Computer->readMem(addr) << 8) +  p_Computer->readMem(addr+1);
					if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
					{
						branchAddr++;
						p_Computer->writeMem(addr, (branchAddr&0xff00)>>8, true);
						p_Computer->writeMem(addr+1, branchAddr&0xff, true);
					}
					addr++;
				break;
			
				case MEM_TYPE_JUMP_REV:
					branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr);
					if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
					{
						branchAddr++;
						p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
						p_Computer->writeMem(addr, branchAddr&0xff, true);
					}
					addr++;
				break;
			
				case MEM_TYPE_OPCODE_LDL:
				case MEM_TYPE_OPCODE_LDL_SLOT:
					branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+4);
					if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
					{
						branchAddr++;
						p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
						p_Computer->writeMem(addr+4, branchAddr&0xff, true);
					}
					addr+=5;
				break;
			
				case MEM_TYPE_OPCODE_RLDL:
					branchAddr = (p_Computer->readMem(addr+2) << 8) +  p_Computer->readMem(addr+3);
					if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
					{
						branchAddr++;
						p_Computer->writeMem(addr+2, (branchAddr&0xff00)>>8, true);
						p_Computer->writeMem(addr+3, branchAddr&0xff, true);
					}
					addr+=3;
				break;

				case MEM_TYPE_FEL2_1:
					switch (p_Computer->getChip8Type())
					{
						case CHIPFEL1:
						case CHIPFEL2:
						case CHIPFEL3:
							chip8_instruction = p_Computer->readMem(addr)&0xf0;
							switch (chip8_instruction)
							{
								case 0:
								case 0x10:
								case 0xF0:
									branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
									if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
									{
										branchAddr++;
										p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
										p_Computer->writeMem(addr+1, branchAddr&0xff, true);
									}
									addr++;
								break;
							}
						break;
						
						case CHIPSTIV:
							chip8_instruction = p_Computer->readMem(addr);
                            if ((chip8_instruction & 0xf0) == 0x0 || (chip8_instruction & 0xf0) == 0x10 || (chip8_instruction & 0xf0) == 0x20 || (chip8_instruction & 0xf0) == 0x30 || (chip8_instruction & 0xf0) == 0x80 || (chip8_instruction & 0xf0) == 0x90)
                                chip8_instruction &= 0xf0;
							switch (chip8_instruction)
							{
                                case 0x0:
                                case 0x10:
                                case 0x20:
                                case 0x30:
								case 0x60:
								case 0x61:
								case 0x66:
								case 0x67:
								case 0x68:
									branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
									if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
									{
										branchAddr++;
										p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
										p_Computer->writeMem(addr+1, branchAddr&0xff, true);
									}
									addr++;
								break;
							}
						break;
					}
				break;

				case MEM_TYPE_ST2_1:
					chip8_instruction = p_Computer->readMem(addr)&0xf0;
					switch (chip8_instruction)
					{
						case 0:    // SYS aaa
						case 0x10: // JP aaa
						case 0x20: // CALL aaa
						case 0xA0: // LD I,aaa
							branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
							if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
							{
								branchAddr++;
								p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
								p_Computer->writeMem(addr+1, branchAddr&0xff, true);
							}
							addr++;
						break;
					}
				break;
			}
		}
		setOut1(out1);
	}
	for (int i=dirAssProgramEndVector[workingRange_]; i>=dirAssProgramStartVector[workingRange_]; i--)
	{
		if (branchAddressTable[i]!=0)
		{
			branchAddress = branchAddressTable[i];
			branchAddressTable[i] = 0;
			insertByte(i, (p_Computer->readMem(i) & 0x0f) | 0xc0, branchAddress);
		}
	}
}

bool DebugWindow::branchChangeNeeded(int range, Word address, Word branchAddr)
{
	switch (runningComputer_)
	{
		case COMX:
			if (p_Computer->readMemDataType(address) == MEM_TYPE_OPCODE_LBR_SLOT || p_Computer->readMemDataType(address) == MEM_TYPE_OPCODE_JUMP_SLOT || p_Computer->readMemDataType(address) == MEM_TYPE_OPCODE_LDL_SLOT)
			{
				if (p_Computer->readMemDataType(address+1) == dirAssSlotVector[workingRange_] || branchAddr < 0xC000 || branchAddr >= 0xE000)
					return true;
			}
			else
			{
				if (dirAssSlotVector[range] == dirAssSlotVector[workingRange_] || branchAddr < 0xC000 || branchAddr >= 0xE000)
					return true;
			}
		break;

		case ELF:
		case ELFII:
		case SUPERELF:
            if (elfConfiguration[runningComputer_].useEms)
            {
                if (p_Computer->readMemDataType(address) == MEM_TYPE_OPCODE_LBR_SLOT || p_Computer->readMemDataType(address) == MEM_TYPE_OPCODE_JUMP_SLOT || p_Computer->readMemDataType(address) == MEM_TYPE_OPCODE_LDL_SLOT)
                {
                    if (p_Computer->readMemDataType(address+1) == dirAssSlotVector[workingRange_] || branchAddr < 0x8000 || branchAddr >= 0xC000)
                        return true;
                }
                else
                {
                    if (dirAssSlotVector[range] == dirAssSlotVector[workingRange_] || branchAddr < 0x8000 || branchAddr >= 0xC000)
                        return true;
                }	
            }
            else
                return true;
            if (elfConfiguration[runningComputer_].useRomMapper)
            {
                if (p_Computer->readMemDataType(address) == MEM_TYPE_OPCODE_LBR_SLOT || p_Computer->readMemDataType(address) == MEM_TYPE_OPCODE_JUMP_SLOT || p_Computer->readMemDataType(address) == MEM_TYPE_OPCODE_LDL_SLOT)
                {
                    if (p_Computer->readMemDataType(address+1) == dirAssSlotVector[workingRange_] || branchAddr < 0x8000 || branchAddr >= 0xFFFF)
                        return true;
                }
                else
                {
                    if (dirAssSlotVector[range] == dirAssSlotVector[workingRange_] || branchAddr < 0x8000 || branchAddr >= 0xFFFF)
                        return true;
                }	
            }
            else
                return true;
		break;

		default:
			return true;
		break;
	}
	return false;
}

void DebugWindow::onDelete(wxCommandEvent&WXUNUSED(event))
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}
	if (lastRange_ <= 0)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_NO_RANGE-ERROR_START-1]);
		return;
	}

	if (!findWorkingRang())
		return;

	for (int i=dirAssProgramStartVector[workingRange_]; i<=dirAssDataEndVector[workingRange_]; i++)
	{
		branchAddressTable[i] = 0;
		branchAddressTableCorrection[i] = false;
	}

	if (p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_OPERAND_LD_3)
		dirAssAddress_ -= 3;

    int checkAddres = setMemLabel(dirAssAddress_, true);
	if (p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_OPCODE_LBR_SLOT || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_OPCODE_JUMP_SLOT || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_JUMP_REV || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_JUMP)
	{
		deleteByte(dirAssAddress_);
		p_Computer->writeMemDataType(dirAssAddress_, MEM_TYPE_DATA);
		deleteByte(dirAssAddress_);
        if (checkAddres != -1)
            checkAddres-=2;
        if (p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_OPCODE_LBR_SLOT)
        {
			deleteByte(dirAssAddress_);
            if (checkAddres != -1)
                checkAddres--;
        }
	}
	else
	{
		if (p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_OPCODE || p_Computer->readMemDataType(dirAssAddress_) >= MEM_TYPE_OPCODE_RSHR)
		{
			deleteByte(dirAssAddress_);
			while (p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_OPERAND || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_OPERAND_LD_2 || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_OPERAND_LD_3 || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_OPERAND_LD_5)
            {
				deleteByte(dirAssAddress_);
                if (checkAddres != -1)
                    checkAddres--;
            }
		}
		else
		{
			if (p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_ST2_1 || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_CHIP_8_1 || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_FEL2_1)
			{
				deleteByte(dirAssAddress_);
				if (p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_ST2_2 || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_FEL2_2 || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_CHIP_8_2 || p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_FEL2_2)
                {
					deleteByte(dirAssAddress_);
                    if (checkAddres != -1)
                        checkAddres--;
                }
			}
			else
            {
				deleteByte(dirAssAddress_);
                if (checkAddres != -1)
                    checkAddres--;
            }
        }
	}

    if (checkAddres != -1)
    {
        Byte out1 = getOut1();
        checkBranch(MEM_LABEL_TYPE, checkAddres-1);
        checkLoadL(MEM_LABEL_TYPE, checkAddres-1);
        setOut1(out1);
    }
//	if (p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_JUMP)
//		deleteByte(dirAssAddress_);

	shortLongBranch();
	correctionList();
	directAss();
	XRCCTRL(*this, "AssInputWindow", wxTextCtrl)->SetFocus();
}

void DebugWindow::deleteByte(Word insertAddress)
{
//	if (insertAddress > dirAssDataEndVector[workingRange_] || insertAddress < dirAssProgramStartVector[workingRange_])
//		assErrorDisplay(DirAssErrorCodes[ERROR_DEBUG_ADDRESS-ERROR_START-1]);
	Byte chip8_instruction;
	Word endAddr;

	if (insertAddress > dirAssProgramEndVector[workingRange_])
		endAddr = dirAssDataEndVector[workingRange_];
	else
		endAddr = dirAssProgramEndVector[workingRange_];

    for (int addr=insertAddress; addr<=endAddr;addr++)
	{
		p_Computer->writeMem(addr, p_Computer->readMem(addr+1), true);
		p_Computer->writeMemDataType(addr, p_Computer->readMemDataType(addr+1));
        if (addr == insertAddress)
            p_Computer->writeMemLabelType(addr, p_Computer->readMemLabelType(addr) | p_Computer->readMemLabelType(addr+1));
        else
            p_Computer->writeMemLabelType(addr, p_Computer->readMemLabelType(addr+1));
        branchAddressTable[addr] = branchAddressTable[addr+1];
		branchAddressTableCorrection[addr] = branchAddressTableCorrection[addr+1];
	}
	p_Computer->writeMem(endAddr, 0, true);
	p_Computer->writeMemDataType(endAddr, MEM_TYPE_DATA);
	branchAddressTable[endAddr] = 0;
	branchAddressTableCorrection[endAddr] = false;

	Word branchAddr;

	for (int addr=dirAssProgramStartVector[workingRange_]; addr<=dirAssProgramEndVector[workingRange_]; addr++)
	{
		if (branchAddressTable[addr]!=0)
		{
			if (branchAddressTable[addr]>insertAddress)
				branchAddressTable[addr]--;
		}
		switch (p_Computer->readMemDataType(addr))
		{
			case MEM_TYPE_OPCODE:
			case MEM_TYPE_OPCODE_BPZ:
			case MEM_TYPE_OPCODE_BGE:
			case MEM_TYPE_OPCODE_BM:
			case MEM_TYPE_OPCODE_BL:
				switch(jumpCorrection[p_Computer->readMem(addr)])
				{
					case 1: // short branch
						if (insertAddress>addr)
							branchAddr = ((addr+1)&0xff00)+p_Computer->readMem(addr+1);
						else
							branchAddr = ((addr+2)&0xff00)+p_Computer->readMem(addr+1);
						if (branchAddr>insertAddress)
						{
							p_Computer->writeMem(addr+1, p_Computer->readMem(addr+1)-1, true);
							if ((addr&0xff)==0xfe)
							{
								changeBranch(addr, branchAddr-1);
							}
							else
							{
								if ((branchAddr&0xff)==0)
								{
									changeBranch(addr, branchAddr-1);
								}
							}
						}
					break;

					case 2: // long branch
						branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+2);
						if (branchAddr>insertAddress && branchAddr < endAddr)
						{
							branchAddr--;
							p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
							p_Computer->writeMem(addr+2, branchAddr&0xff, true);
						}
					break;

					case 3: // SCAL RN,xxxx + DBNZ
						if ((p_Computer->readMem(addr+1)&0xf0) == 0x80 || (p_Computer->readMem(addr+1)&0xf0) == 0x20)
						{
							branchAddr = (p_Computer->readMem(addr+2) << 8) +  p_Computer->readMem(addr+3);
							if (branchAddr>insertAddress && branchAddr < endAddr)
							{
								branchAddr--;
								p_Computer->writeMem(addr+2, (branchAddr&0xff00)>>8, true);
								p_Computer->writeMem(addr+3, branchAddr&0xff, true);
							}
						}
						if (p_Computer->readMem(addr+1) == 0x3e || p_Computer->readMem(addr+1) == 0x3f)
						{
							if (insertAddress>addr)
								branchAddr = ((addr+2)&0xff00)+p_Computer->readMem(addr+2);
							else
								branchAddr = ((addr+3)&0xff00)+p_Computer->readMem(addr+2);
							if (branchAddr>insertAddress)
							{
								p_Computer->writeMem(addr+2, p_Computer->readMem(addr+2)-1, true);
								if (((addr+1)&0xff)==0xfe)
								{
									changeBranch(addr, branchAddr-1);
								}
								else
								{
									if ((branchAddr&0xff)==0)
									{
										changeBranch(addr, branchAddr-1);
									}
								}
							}
						}
					break;
				}
			break;

			case MEM_TYPE_JUMP:
				branchAddr = (p_Computer->readMem(addr) << 8) +  p_Computer->readMem(addr+1);
				if (branchAddr>insertAddress && branchAddr < endAddr)
				{
					branchAddr--;
					p_Computer->writeMem(addr, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr+1, branchAddr&0xff, true);
				}
				addr++;
			break;

			case MEM_TYPE_JUMP_REV:
				branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr);
				if (branchAddr>insertAddress && branchAddr < endAddr)
				{
					branchAddr--;
					p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr, branchAddr&0xff, true);
				}
				addr++;
			break;

			case MEM_TYPE_OPCODE_JUMP_SLOT:
				branchAddr = (p_Computer->readMem(addr) << 8) +  p_Computer->readMem(addr+1);
				if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
				{
					branchAddr--;
					p_Computer->writeMem(addr, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr+1, branchAddr&0xff, true);
				}
				addr++;
			break;
		
			case MEM_TYPE_OPCODE_LBR_SLOT:
				branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+2);
				if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
				{
					branchAddr--;
					p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr+2, branchAddr&0xff, true);
				}
				addr+=2;
			break;

			case MEM_TYPE_OPCODE_LDL:
				branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+4);
				if (branchAddr>insertAddress && branchAddr < endAddr)
				{
					branchAddr--;
					p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr+4, branchAddr&0xff, true);
				}
				addr+=5;
			break;

			case MEM_TYPE_OPCODE_LDL_SLOT:
				branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+4);
				if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
				{
					branchAddr--;
					p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr+4, branchAddr&0xff, true);
				}
				addr+=5;
			break;

			case MEM_TYPE_OPCODE_RLDL:
				branchAddr = (p_Computer->readMem(addr+2) << 8) +  p_Computer->readMem(addr+3);
				if (branchAddr>insertAddress && branchAddr < endAddr)
				{
					branchAddr--;
					p_Computer->writeMem(addr+2, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(addr+3, branchAddr&0xff, true);
				}
				addr+=3;
			break;

			case MEM_TYPE_FEL2_1:
				switch (p_Computer->getChip8Type())
				{
					case CHIPFEL1:
					case CHIPFEL2:
					case CHIPFEL3:
						chip8_instruction = p_Computer->readMem(addr)&0xf0;
						switch (chip8_instruction)
						{
							case 0:
							case 0x10:
							case 0xF0:
								branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
								if (branchAddr>insertAddress && branchAddr < endAddr)
								{
									branchAddr--;
									p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
									p_Computer->writeMem(addr+1, branchAddr&0xff, true);
								}
								addr++;
							break;
						}
					break;
					
					case CHIPSTIV:
						chip8_instruction = p_Computer->readMem(addr);
                        if ((chip8_instruction & 0xf0) == 0x0 || (chip8_instruction & 0xf0) == 0x10 || (chip8_instruction & 0xf0) == 0x20 || (chip8_instruction & 0xf0) == 0x30 || (chip8_instruction & 0xf0) == 0x80 || (chip8_instruction & 0xf0) == 0x90)
							chip8_instruction &= 0xf0;
						switch (chip8_instruction)
						{
                            case 0x0:
                            case 0x10:
                            case 0x20:
                            case 0x30:
							case 0x60:
							case 0x61:
							case 0x66:
							case 0x67:
							case 0x68:
								branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
								if (branchAddr>insertAddress && branchAddr < endAddr)
								{
									branchAddr--;
									p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
									p_Computer->writeMem(addr+1, branchAddr&0xff, true);
								}
								addr++;
							break;

							case 0x65:
							case 0x80:
							case 0x90:
                                if (insertAddress>addr)
                                    branchAddr = ((addr+1)&0xff00)+p_Computer->readMem(addr+1);
                                else
                                    branchAddr = ((addr+2)&0xff00)+p_Computer->readMem(addr+1);
                                if (branchAddr>insertAddress)
                                {
                                    p_Computer->writeMem(addr+1, p_Computer->readMem(addr+1)-1, true);
                                    if ((addr&0xff)==0xfe)
                                    {
                                        branchAddressTableCorrection[addr] = true;
                                    }
                                    else
                                    {
                                        if ((branchAddr&0xff)==0)
                                        {
                                            branchAddressTableCorrection[addr] = true;
                                        }
                                    }
                                }
							break;
						}
					break;
				}
			break;
                
            case MEM_TYPE_CHIP_8_1:
				chip8_instruction = p_Computer->readMem(addr)&0xf0;
				switch (chip8_instruction)
				{
					case 0:
					case 0x10:
					case 0x20:
					case 0xA0:
					case 0xB0:
						branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
						if (branchAddr>insertAddress && branchAddr < endAddr)
						{
							branchAddr--;
							p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
							p_Computer->writeMem(addr+1, branchAddr&0xff, true);
						}
						addr++;
					break;
				}
			break;

			case MEM_TYPE_ST2_1:
				chip8_instruction = p_Computer->readMem(addr)&0xf0;
				switch (chip8_instruction)
				{
					case 0:
					case 0x10:
					case 0x20:
					case 0xA0:
						branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
						if (branchAddr>insertAddress && branchAddr < endAddr)
						{
							branchAddr--;
							p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
							p_Computer->writeMem(addr+1, branchAddr&0xff, true);
						}
						addr++;
					break;

					case 0x30:
					case 0x40:
					case 0xD0:
						if (insertAddress>addr)
							branchAddr = ((addr+1)&0xff00)+p_Computer->readMem(addr+1);
						else
							branchAddr = ((addr+2)&0xff00)+p_Computer->readMem(addr+1);
						if (branchAddr>insertAddress)
						{
							p_Computer->writeMem(addr+1, p_Computer->readMem(addr+1)-1, true);
							if ((addr&0xff)==0xfe)
							{
								branchAddressTableCorrection[addr] = true;
							}
							else
							{
								if ((branchAddr&0xff)==0)
								{
									branchAddressTableCorrection[addr] = true;
								}
							}
						}
					break;
				}
				switch (p_Computer->readMem(addr))
				{
					case 0x70:
					case 0xE8:
						if (insertAddress>addr)
							branchAddr = ((addr+1)&0xff00)+p_Computer->readMem(addr+1);
						else
							branchAddr = ((addr+2)&0xff00)+p_Computer->readMem(addr+1);
						if (branchAddr>insertAddress)
						{
							p_Computer->writeMem(addr+1, p_Computer->readMem(addr+1)-1, true);
							if ((addr&0xff)==0xfe)
							{
								branchAddressTableCorrection[addr] = true;
							}
							else
							{
								if ((branchAddr&0xff)==0)
								{
									branchAddressTableCorrection[addr] = true;
								}
							}
						}
					break;
				}
			break;
		}
	}

	Byte out1=getOut1();
	int loopStart, loopEnd;

	for (int i=0; i<lastRange_; i++)
	{
		if (i != workingRange_) 
		{
			loopStart = dirAssProgramStartVector[i];
			loopEnd = dirAssDataEndVector[i];
		}
		else
		{
			loopStart = dirAssProgramEndVector[i]+1;
			loopEnd = dirAssDataEndVector[i];
		}

		setOut1(dirAssSlotVector[i]);

		for (int addr=loopStart; addr<=loopEnd; addr++)
		{
			switch (p_Computer->readMemDataType(addr))
			{
				case MEM_TYPE_OPCODE:
				case MEM_TYPE_OPCODE_LBR_SLOT:
					switch(jumpCorrection[p_Computer->readMem(addr)])
					{
						case 2: // long branch
							branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+2);
							if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
							{
								branchAddr--;
								p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
								p_Computer->writeMem(addr+2, branchAddr&0xff, true);
							}
							addr+=2;
						break;

						case 3: // SCAL RN,xxxx + DBNZ
							if ((p_Computer->readMem(addr+1)&0xf0) == 0x80 || (p_Computer->readMem(addr+1)&0xf0) == 0x20)
							{
								branchAddr = (p_Computer->readMem(addr+2) << 8) +  p_Computer->readMem(addr+3);
								if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
								{
									branchAddr--;
									p_Computer->writeMem(addr+2, (branchAddr&0xff00)>>8, true);
									p_Computer->writeMem(addr+3, branchAddr&0xff, true);
								}
							}
							addr+=3;
						break;
					}
				break;

				case MEM_TYPE_JUMP:
				case MEM_TYPE_OPCODE_JUMP_SLOT:
					branchAddr = (p_Computer->readMem(addr) << 8) +  p_Computer->readMem(addr+1);
					if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
					{
						branchAddr--;
						p_Computer->writeMem(addr, (branchAddr&0xff00)>>8, true);
						p_Computer->writeMem(addr+1, branchAddr&0xff, true);
					}
					addr++;
				break;

				case MEM_TYPE_JUMP_REV:
					branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr);
					if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
					{
						branchAddr--;
						p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
						p_Computer->writeMem(addr, branchAddr&0xff, true);
					}
					addr++;
				break;

				case MEM_TYPE_OPCODE_LDL:
				case MEM_TYPE_OPCODE_LDL_SLOT:
					branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+4);
					if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
					{
						branchAddr--;
						p_Computer->writeMem(addr+1, (branchAddr&0xff00)>>8, true);
						p_Computer->writeMem(addr+4, branchAddr&0xff, true);
					}
					addr+=5;
				break;

				case MEM_TYPE_OPCODE_RLDL:
					branchAddr = (p_Computer->readMem(addr+2) << 8) +  p_Computer->readMem(addr+3);
					if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
					{
						branchAddr--;
						p_Computer->writeMem(addr+2, (branchAddr&0xff00)>>8, true);
						p_Computer->writeMem(addr+3, branchAddr&0xff, true);
					}
					addr+=3;
				break;

				case MEM_TYPE_FEL2_1:
					switch (p_Computer->getChip8Type())
					{
						case CHIPFEL1:
						case CHIPFEL2:
						case CHIPFEL3:
							chip8_instruction = p_Computer->readMem(addr)&0xf0;
							switch (chip8_instruction)
							{
								case 0:
								case 0x10:
								case 0xF0:
									branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
									if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
									{
										branchAddr--;
										p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
										p_Computer->writeMem(addr+1, branchAddr&0xff, true);
									}
									addr++;
								break;
							}
						break;
						
						case CHIPSTIV:
							chip8_instruction = p_Computer->readMem(addr);
                            if ((chip8_instruction & 0xf0) == 0x0 || (chip8_instruction & 0xf0) == 0x10 || (chip8_instruction & 0xf0) == 0x20 || (chip8_instruction & 0xf0) == 0x30 || (chip8_instruction & 0xf0) == 0x80 || (chip8_instruction & 0xf0) == 0x90)
								chip8_instruction &= 0xf0;
							switch (chip8_instruction)
							{
                                case 0x0:
                                case 0x10:
                                case 0x20:
                                case 0x30:
								case 0x60:
								case 0x61:
								case 0x66:
								case 0x67:
								case 0x68:
									branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
									if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
									{
										branchAddr--;
										p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
										p_Computer->writeMem(addr+1, branchAddr&0xff, true);
									}
									addr++;
								break;
							}
						break;
					}
				break;
                    
                case MEM_TYPE_CHIP_8_1:
					chip8_instruction = p_Computer->readMem(addr)&0xf0;
					switch (chip8_instruction)
					{
						case 0:
						case 0x10:
						case 0x20:
						case 0xA0:
						case 0xB0:
							branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
							if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
							{
								branchAddr--;
								p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
								p_Computer->writeMem(addr+1, branchAddr&0xff, true);
							}
							addr++;
						break;
					}
				break;

				case MEM_TYPE_ST2_1:
					chip8_instruction = p_Computer->readMem(addr)&0xf0;
					switch (chip8_instruction)
					{
						case 0:
						case 0x10:
						case 0x20:
						case 0xA0:
							branchAddr = ((p_Computer->readMem(addr)&0xf) << 8) +  p_Computer->readMem(addr+1);
							if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
							{
								branchAddr--;
								p_Computer->writeMem(addr, chip8_instruction | ((branchAddr&0xf00)>>8), true);
								p_Computer->writeMem(addr+1, branchAddr&0xff, true);
							}
							addr++;
						break;
					}
				break;
			}
		}
		setOut1(out1);
	}
	Word branchAddress;
	for (int i=dirAssProgramEndVector[workingRange_]; i>=dirAssProgramStartVector[workingRange_]; i--)
	{
		if (branchAddressTable[i]!=0)
		{
			branchAddress = branchAddressTable[i];
			branchAddressTable[i] = 0;
			insertByte(i, (p_Computer->readMem(i) & 0x0f) | 0xc0, branchAddress);
		}
	}
}

void DebugWindow::shortLongBranch()
{
	Word branchAddr;

	for (int addr=dirAssProgramStartVector[workingRange_]; addr<=dirAssProgramEndVector[workingRange_]; addr++)
	{
		if (p_Computer->readMemDataType(addr) == MEM_TYPE_OPCODE)
		{
			switch(jumpCorrection[p_Computer->readMem(addr)])
			{
				case 2: // long branch
					branchAddr = (p_Computer->readMem(addr+1) << 8) +  p_Computer->readMem(addr+2);
					if ((branchAddr&0xff00) == (addr&0xff00) && ((addr&0xff) != 0xff))
					{
						p_Computer->writeMem(addr, (p_Computer->readMem(addr) & 0x0f) | 0x30, true);
						p_Computer->writeMem(addr+1, p_Computer->readMem(addr+2), true);
						p_Computer->writeMem(addr+2, 0xc4, true);
						p_Computer->writeMemDataType(addr+2, MEM_TYPE_OPCODE);
						deleteByte(addr+2);
					}
				break;
			}
		}
	}
}

void DebugWindow::correctionList()
{
	wxString buffer;
	for (int addr=dirAssProgramStartVector[workingRange_]; addr<=dirAssProgramEndVector[workingRange_]; addr++)
	{
		if (branchAddressTableCorrection[addr])
		{
			buffer.Printf("Check: %04X", addr);
			assErrorDisplay(buffer);
		}
	}
}

void DebugWindow::changeBranch(Word addr, Word branchAddr)
{
	Byte instruction=p_Computer->readMem(addr);

	switch(instruction)
	{
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x39:
		case 0x3a:
		case 0x3b:
			if (branchAddressTable[addr]==0)
				branchAddressTable[addr] = branchAddr;
		break;

		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x3c:
		case 0x3d:
		case 0x3e:
		case 0x3f:
		case 0x68:
			branchAddressTableCorrection[addr] = true;
		break;
	}
}

void DebugWindow::onAssSave(wxCommandEvent&WXUNUSED(event))
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}
	if (lastRange_ <= 0 || workingRange_ == -1)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_NO_RANGE-ERROR_START-1]);
		return;
	}

	onAssSave(workingRange_);
}

void DebugWindow::onAssLoadAll(wxCommandEvent&WXUNUSED(event))
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}

	wxString fileName = wxFileSelector( "Select the configuration file to load",
                               dirAssConfigFileDir_, dirAssConfigFile_,
                               "",
                               wxString::Format
                              (
								   "Congiguration File (*.config)|*.config|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );
	if (!fileName)
		return;

	wxFileName configFullPath = wxFileName(fileName, wxPATH_NATIVE);
	dirAssConfigFile_ = configFullPath.GetFullName();
	dirAssConfigFileDir_ = configFullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	loadAll(dirAssConfigFileDir_ + dirAssConfigFile_);
}

void DebugWindow::loadAll(wxString configFileName)
{
	wxTextFile configFileIn;

	if (wxFile::Exists(configFileName))
		configFileIn.Open(configFileName);
	else
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_FILE_NOTFOUND-ERROR_START-1]);
		return;
	}

	wxString numberStr, str, addressStr;
	long addressLong;
	int number = 0;
	
	lastRange_ = 0;
	workingRange_ = -1;

	numberStr.Printf("%d",number);
	wxString identifier;

	for (identifier = configFileIn.GetFirstLine(); !configFileIn.Eof(); identifier = configFileIn.GetNextLine()) 
	{
		if (identifier.Left(5) == "Name=")
		{
			lastRange_++;
			dirAssProgramStartVector.resize(lastRange_);
			dirAssProgramEndVector.resize(lastRange_);
			dirAssDataEndVector.resize(lastRange_);
			dirAssDirNameVector.resize(lastRange_);
			dirAssFileNameVector.resize(lastRange_);
			dirAssSlotVector.resize(lastRange_);

			identifier = identifier.Right(identifier.Len()-5);

			dirAssFileNameVector[number] = identifier;

			wxString tempStr = configFileIn.GetNextLine();
            tempStr.Replace("\\", wxFileName::GetPathSeparator(wxPATH_NATIVE));
            tempStr.Replace("/", wxFileName::GetPathSeparator(wxPATH_NATIVE));
			if (tempStr.Left(2) == "c:" || tempStr.Left(2) == "C:")
				dirAssDirNameVector[number] = tempStr;
			else
				dirAssDirNameVector[number] = dataDir_ + tempStr;

			addressStr = configFileIn.GetNextLine();
			addressStr.ToLong(&addressLong, 16);
			dirAssProgramStartVector[number] = addressLong;

			addressStr = configFileIn.GetNextLine();
			addressStr.ToLong(&addressLong, 16);
			dirAssProgramEndVector[number] = addressLong;

			addressStr = configFileIn.GetNextLine();
			addressStr.ToLong(&addressLong, 16);
			dirAssDataEndVector[number] = addressLong;

			addressStr = configFileIn.GetNextLine();
			addressStr.ToLong(&addressLong, 16);
			dirAssSlotVector[number] = addressLong;

			number++;
			numberStr.Printf("%d",number);
		}
	}
	configFileIn.Close();

	Byte out1 = getOut1();
	for (int i=0; i<lastRange_; i++)
	{
		if (!onAssLoad(i))
		{
			assErrorDisplay(DirAssErrorCodes[ERROR_FILE_NOTFOUND-ERROR_START-1]);
			return;
		}
	}
	setOut1(out1);

	if (lastRange_ != 0)
	{
		XRCCTRL(*this, "AssInsert", wxButton)->Enable(true);
		XRCCTRL(*this, "AssDelete", wxButton)->Enable(true);
		XRCCTRL(*this, "AssSaveAll", wxButton)->Enable(true);
		XRCCTRL(*this, "AssFrom", wxButton)->Enable(true);
		XRCCTRL(*this, "AssFromL", wxButton)->Enable(true);
		XRCCTRL(*this, "AssFromV", wxButton)->Enable(true);
        XRCCTRL(*this, "AssFromA", wxButton)->Enable(true);
        XRCCTRL(*this, "AssRangeSpin", wxSpinButton)->Enable(true);

		shownRange_ = 0;
		showConfiguration();
	}
	assErrorDisplay(DirAssErrorCodes[ERROR_CONF_LOADED-ERROR_START-1]);
}

void DebugWindow::onAssSaveSb(wxCommandEvent&WXUNUSED(event))
{
	if (runningComputer_ != COMX)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}

    if (dirAssConfigFile_ != "sb.config")
        lastRange_ = 0;
        
	dirAssConfigFileDir_ = debugDir_ + "Comx" + pathSeparator_ + "Usb FW Development" + pathSeparator_;
	dirAssConfigFile_ = "sb.config";

	if (!wxFile::Exists(dirAssConfigFileDir_ + dirAssConfigFile_))
		dirAssConfigFileDir_ = debugDir_ + "Comx" + pathSeparator_ + "Usb FW" + pathSeparator_;

	if (lastRange_ == 0)
	{
		loadAll(dirAssConfigFileDir_ + dirAssConfigFile_);
		return;
	}

	if (!saveAll(dirAssConfigFileDir_ + dirAssConfigFile_))
		return;

	if (runningComputer_ == COMX)
	{
		Byte out1 = p_Computer->getOutValue(1);

		Byte value;
		wxFile outputFile, outputFileBank;
		wxString fileName =  dirAssDirNameVector[0] + pathSeparator_ + "system_rom.bin";
		outputFile.Create(fileName, true);
		for (long address = 0; address <= 0x3fff; address++)
		{
			value = p_Computer->readMem(address);
			outputFile.Write(&value, 1);
		}
		for (long address = 0xe000; address <= 0xe7ff; address++)
		{
			value = p_Computer->readMem(address);
			outputFile.Write(&value, 1);
		}
		for (long address = 0x4800; address <= 0x67ff; address++)
		{
			value = 0xff;
			outputFile.Write(&value, 1);
		}
		for (long address = 0xe800; address <= 0xefff; address++)
		{
			value = p_Computer->readMem(address);
			outputFile.Write(&value, 1);
		}
		for (long address = 0x7000; address <= 0x77ff; address++)
		{
			value = 0xff;
			outputFile.Write(&value, 1);
		}
		for (long address = 0xf800; address <= 0xffff; address++)
		{
			value = p_Computer->readMem(address);
			outputFile.Write(&value, 1);
		}
		outputFile.Close();
		fileName =  dirAssDirNameVector[0] + pathSeparator_ + "usb_rom.bin";
		outputFile.Create(fileName, true);
		p_Comx->bankOut(0x10);
		for (long address = 0xc000; address <= 0xdfff; address++)
		{
			value = p_Computer->readMem(address);
			outputFile.Write(&value, 1);
		}
		p_Comx->bankOut(0x30);
		for (long address = 0xc000; address <= 0xdfff; address++)
		{
			value = p_Computer->readMem(address);
			outputFile.Write(&value, 1);
		}
		p_Comx->bankOut(0x50);
		for (long address = 0xc000; address <= 0xdfff; address++)
		{
			value = p_Computer->readMem(address);
			outputFile.Write(&value, 1);
		}
		p_Comx->bankOut(0x70);
		for (long address = 0xc000; address <= 0xdfff; address++)
		{
			value = p_Computer->readMem(address);
			outputFile.Write(&value, 1);
		}
		outputFile.Close();
		fileName =  dirAssDirNameVector[0] + pathSeparator_ + "f&m_rom_2.bin";
		outputFile.Create(fileName, true);
		p_Comx->bankOut(0x90); // 1001
		for (long address = 0xc000; address <= 0xdfff; address++)
		{
			value = p_Computer->readMem(address);
			outputFile.Write(&value, 1);
		}

		p_Comx->bankOut(0xb0); // 1011
		wxString bankFileName =  dirAssDirNameVector[0] + pathSeparator_ + "sb.c000-5.bin";
		outputFileBank.Create(bankFileName, true);
		for (long address = 0xc000; address <= 0xdfff; address++)
		{
			value = p_Computer->readMem(address);
			outputFile.Write(&value, 1);
			outputFileBank.Write(&value, 1);
		}
		outputFileBank.Close();

		p_Comx->bankOut(0xd0); // 1101
		for (long address = 0xc000; address <= 0xdfff; address++)
		{
			value = p_Computer->readMem(address);
			outputFile.Write(&value, 1);
		}
		p_Comx->bankOut(0xf0); // 1111
		for (long address = 0xc000; address <= 0xdfff; address++)
		{
			value = p_Computer->readMem(address);
			outputFile.Write(&value, 1);
		}
		outputFile.Close();

		wxString systemRomName = dirAssDirNameVector[0] + pathSeparator_ + "sb.0000.bin";
		outputFile.Create(systemRomName, true);
		for (long address = 0x0000; address <= 0x3fff; address++)
		{
			value = p_Computer->readMem(address);
			outputFile.Write(&value, 1);
		}
		outputFile.Close();

		p_Comx->bankOut(out1);
	}
}

void DebugWindow::onAssSaveAll(wxCommandEvent&WXUNUSED(event))
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}
	if (lastRange_ <= 0)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_NO_RANGE-ERROR_START-1]);
		return;
	}

	wxString fileName = wxFileSelector( "Select the configuration file to save",
                               dirAssConfigFileDir_, dirAssConfigFile_,
                               "",
                               wxString::Format
                              (
								   "Congiguration File (*.config)|*.config|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_OVERWRITE_PROMPT,
                               this
                              );
	if (!fileName)
		return;

	wxFileName configFullPath = wxFileName(fileName, wxPATH_NATIVE);
	dirAssConfigFile_ = configFullPath.GetFullName();
	dirAssConfigFileDir_ = configFullPath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR, wxPATH_NATIVE);

	saveAll(dirAssConfigFileDir_ + dirAssConfigFile_);
}

void DebugWindow::onAssTextChange(wxCommandEvent&WXUNUSED(event))
{
	if (lastRange_ > 0)
	{
		XRCCTRL(*this,"AssStore",wxButton)->Enable(true);
#ifdef __WXMAC__
        XRCCTRL(*this, "AssStore", wxButton)->SetBackgroundColour(*wxRED);
#else
		XRCCTRL(*this, "AssStore", wxButton)->SetForegroundColour(*wxRED);
#endif
    }
}

void DebugWindow::onAssNew(wxCommandEvent&WXUNUSED(event))
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}

	assNew(lastRange_);
	assErrorDisplay("");
}

void DebugWindow::assNew(int range)
{
	lastRange_ = range;
	XRCCTRL(*this,"AssProgramSlot",wxTextCtrl)->ChangeValue("");
	XRCCTRL(*this,"AssProgramStart",wxTextCtrl)->ChangeValue("");
	XRCCTRL(*this,"AssCodeEnd",wxTextCtrl)->ChangeValue("");
	XRCCTRL(*this,"AssProgramEnd",wxTextCtrl)->ChangeValue("");
	wxString value;
	value.Printf("%0d:",lastRange_+1);
	XRCCTRL(*this,"AssRangeNumber",wxStaticText)->SetLabel(value);
	XRCCTRL(*this,"AssFileName",wxTextCtrl)->ChangeValue("");
	shownRange_ = lastRange_;
    dirAssNewDir_ = dirAssConfigFileDir_;

	XRCCTRL(*this,"AssStore",wxButton)->Enable(true);
#ifdef __WXMAC__
    XRCCTRL(*this, "AssStore", wxButton)->SetBackgroundColour(*wxRED);
#else
	XRCCTRL(*this, "AssStore", wxButton)->SetForegroundColour(*wxRED);
#endif
/*		XRCCTRL(*this,"AssProgramSlot",wxTextCtrl)->ChangeValue("");
		XRCCTRL(*this,"AssProgramStart",wxTextCtrl)->ChangeValue("");
		XRCCTRL(*this,"AssCodeEnd",wxTextCtrl)->ChangeValue("");
		XRCCTRL(*this,"AssProgramEnd",wxTextCtrl)->ChangeValue("");
		XRCCTRL(*this,"AssRangeNumber",wxStaticText)->SetLabel("0:");
		XRCCTRL(*this,"AssFileName",wxTextCtrl)->ChangeValue("");
		XRCCTRL(*this,"AssStore",wxButton)->Enable(false);
		XRCCTRL(*this, "AssStore", wxButton)->SetForegroundColour(*wxBLACK);*/
}

void DebugWindow::assDefault(wxString fileName, Word start, Word end)
{
	if (!mode_.gui)
		return;

    wxString valueString;
    
    XRCCTRL(*this,"AssProgramSlot",wxTextCtrl)->ChangeValue("");
    valueString.Printf("%04X", start);
    XRCCTRL(*this,"AssProgramStart",wxTextCtrl)->ChangeValue(valueString);
    valueString.Printf("%04X", end);
    XRCCTRL(*this,"AssCodeEnd",wxTextCtrl)->ChangeValue(valueString);
    XRCCTRL(*this,"AssProgramEnd",wxTextCtrl)->ChangeValue(valueString);
    wxString value;
    value.Printf("%0d:",lastRange_+1);
    XRCCTRL(*this,"AssRangeNumber",wxStaticText)->SetLabel(value);
    XRCCTRL(*this,"AssFileName",wxTextCtrl)->ChangeValue(fileName + ".bin");
    shownRange_ = lastRange_;
    dirAssNewDir_ = conf[runningComputer_].mainDir_;
    dirAssConfigFileDir_ = conf[runningComputer_].mainDir_;
    dirAssConfigFile_ = fileName + ".config";
    onAssStore();
}

void DebugWindow::onAssStore(wxCommandEvent&WXUNUSED(event))
{
    if (!computerRunning_)
    {
        assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
        return;
    }
    onAssStore();
}

void DebugWindow::onAssStore()
{
	if (shownRange_ == -1)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_NO_RANGE-ERROR_START-1]);
		return;
	}

	long start = get16BitValue("AssProgramStart");
	long codeEnd = get16BitValue("AssCodeEnd");
	long end = get16BitValue("AssProgramEnd");
	long slot = get8BitValue("AssProgramSlot");
	wxString name = XRCCTRL(*this,"AssFileName",wxTextCtrl)->GetValue();

	if (slot == -1)
	{
		if (runningComputer_ == COMX)
			slot = 0x10;
		else
			slot = 0;
	}

	if (name == "")
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_INVALID_FILE_NAME-ERROR_START-1]);
		return;
	}

	if (end == -1)
		end = codeEnd;

	if (start >= codeEnd || codeEnd > end || start == -1 || codeEnd == -1)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_INVALID_START-ERROR_START-1]);
		return;
	}

	switch (runningComputer_)
	{
		case COMX:
			if (start >= 0xc000 && start < 0xe000)
			{
				if (codeEnd >= 0xe000 || end >= 0xe000)
				{
					assErrorDisplay(DirAssErrorCodes[ERROR_SLOT_RANGE-ERROR_START-1]);
					return;
				}
			}
			if (codeEnd >= 0xc000 && codeEnd < 0xe000)
			{
				if (start < 0xc000 || end >= 0xe000)
				{
					assErrorDisplay(DirAssErrorCodes[ERROR_SLOT_RANGE-ERROR_START-1]);
					return;
				}
			}
			if (end >= 0xc000 && end < 0xe000)
			{
				if (start < 0xc000 || codeEnd < 0xc000)
				{
					assErrorDisplay(DirAssErrorCodes[ERROR_SLOT_RANGE-ERROR_START-1]);
					return;
				}
			}
		break;

		case ELF:
		case ELFII:
		case SUPERELF:
			if (start >= 0x8000 && start < 0xc000)
			{
				if (codeEnd >= 0xc000 || end >= 0xc000)
				{
					assErrorDisplay(DirAssErrorCodes[ERROR_SLOT_RANGE-ERROR_START-1]);
					return;
				}
			}
			if (codeEnd >= 0x8000 && codeEnd < 0xc000)
			{
				if (start < 0x8000 || end >= 0xc000)
				{
					assErrorDisplay(DirAssErrorCodes[ERROR_SLOT_RANGE-ERROR_START-1]);
					return;
				}
			}
			if (end >= 0x8000 && end < 0xc000)
			{
				if (start < 0x8000 || codeEnd < 0x8000)
				{
					assErrorDisplay(DirAssErrorCodes[ERROR_SLOT_RANGE-ERROR_START-1]);
					return;
				}
			}
		break;
	}

	if (dirAssNewDir_ == "")
		dirAssNewDir_ = dirAssConfigFileDir_;

	if (shownRange_ >= lastRange_)
		AssAddConfig(dirAssNewDir_, name, start, codeEnd, end, slot); 
	else
		AssStoreConfig(shownRange_, dirAssDirNameVector[shownRange_], name, start, codeEnd, end, slot); 

	XRCCTRL(*this,"AssStore",wxButton)->Enable(false);
#ifdef __WXMAC__
    XRCCTRL(*this, "AssStore", wxButton)->SetBackgroundColour(wxNullColour);
#else
	XRCCTRL(*this, "AssStore", wxButton)->SetForegroundColour(*wxBLACK);
#endif
    if (lastRange_ != 0)
	{
		XRCCTRL(*this, "AssInsert", wxButton)->Enable(true);
		XRCCTRL(*this, "AssDelete", wxButton)->Enable(true);
		XRCCTRL(*this, "AssSaveAll", wxButton)->Enable(true);
		XRCCTRL(*this, "AssFrom", wxButton)->Enable(true);
		XRCCTRL(*this, "AssFromL", wxButton)->Enable(true);
		XRCCTRL(*this, "AssFromV", wxButton)->Enable(true);
        XRCCTRL(*this, "AssFromA", wxButton)->Enable(true);
		XRCCTRL(*this, "AssRangeSpin", wxSpinButton)->Enable(true);
	}
	assErrorDisplay("");
}

void DebugWindow::onAssDir(wxCommandEvent& WXUNUSED(event))
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}

	if (shownRange_ == -1)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_NO_RANGE-ERROR_START-1]);
		return;
	}

	wxString folder;

	if (shownRange_ >= lastRange_)
		folder = debugDir_;
	else	
		folder = dirAssDirNameVector[shownRange_];

	wxString dirName = wxDirSelector( "Select configuration folder", folder);
	if (!dirName)
		return;

	if (shownRange_ >= lastRange_)
		dirAssNewDir_ = dirName;
	else	
		dirAssDirNameVector[shownRange_] = dirName;
}

void DebugWindow::saveAll()
{
	if (lastRange_ != 0)
		saveAll(dirAssConfigFileDir_ + dirAssConfigFile_);
}

bool DebugWindow::saveAll(wxString configFileName)
{
	wxTextFile configFileOut;

	if (wxFile::Exists(configFileName))
	{
		if (!wxFileName::IsFileWritable(configFileName))
		{
			(void)wxMessageBox( "Configuration not saved!\nFile is open in another application \n",
								"Emma 02", wxICON_ERROR | wxOK );
			return false;
		}
		else
			configFileOut.Open(configFileName);
	}
	else
		configFileOut.Create(configFileName);

	configFileOut.Clear();

	wxString hexAddress;

	Byte out1 = getOut1();

	for (int i=0; i<lastRange_; i++)
	{
		wxString fileName = dirAssDirNameVector[i] + pathSeparator_ + dirAssFileNameVector[i];
		if (wxFile::Exists(fileName) && !wxFileName::IsFileWritable(fileName))
		{
			(void)wxMessageBox( "Configuration not saved!\nOne or more code files are open in another application\n",
								"Emma 02", wxICON_ERROR | wxOK );
			return false;
		}
	}

	for (int i=0; i<lastRange_; i++)
	{
		onAssSave(i);
		configFileOut.AddLine("Name="+dirAssFileNameVector[i]);

		size_t dataDirLen = dataDir_.Len();
		wxString tempStr;

		if (dirAssDirNameVector[i].Left(dataDirLen) == dataDir_)
			tempStr = dirAssDirNameVector[i].Right(dirAssDirNameVector[i].Len()-dataDirLen);
		else
			tempStr = dataDir_;

		configFileOut.AddLine(tempStr);

		hexAddress.Printf("%04X",dirAssProgramStartVector[i]);
		configFileOut.AddLine(hexAddress);

		hexAddress.Printf("%04X",dirAssProgramEndVector[i]);
		configFileOut.AddLine(hexAddress);

		hexAddress.Printf("%04X",dirAssDataEndVector[i]);
		configFileOut.AddLine(hexAddress);

		hexAddress.Printf("%02X",dirAssSlotVector[i]);
		configFileOut.AddLine(hexAddress);
	}

	configFileOut.Write();
	configFileOut.Close();

	setOut1(out1);

	assErrorDisplay(DirAssErrorCodes[ERROR_CONF_SAVED-ERROR_START-1]);
	return true;
}

void DebugWindow::onAssSave(int range)
{
	Byte value;
	wxFile outputFile;
	wxTextFile outputTextFile;
	wxString fileName, memoryStr, strValue, line;

	fileName = dirAssDirNameVector[range] + pathSeparator_ + dirAssFileNameVector[range];

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	wxString name = FullPath.GetName();
	wxString path = FullPath.GetPath();
	wxString ext = FullPath.GetExt();

	setOut1(dirAssSlotVector[range]);

	if (wxFile::Exists(fileName))
	{
		int number=1;
		wxString numberString;

		numberString.Printf(".%d",number);
		while (wxFile::Exists(fileName+numberString))
		{
			number++;
			numberString.Printf(".%d",number);
		}
		wxRenameFile(fileName,fileName+numberString,true);
		if (wxFile::Exists(path+pathSeparator_+name+".debug"))
			wxRenameFile(path+pathSeparator_+name+".debug",path+pathSeparator_+name+".debug"+numberString,true);
	}

    outputFile.Create(fileName, true);
    for (long address = dirAssProgramStartVector[range]; address <= dirAssDataEndVector[range]; address++)
    {
        value = p_Computer->readMem(address);
        outputFile.Write(&value, 1);
    }
    outputFile.Close();

    p_Computer->writeDebugFile(dirAssDirNameVector[range]+pathSeparator_, dirAssFileNameVector[range], dirAssProgramStartVector[range], dirAssDataEndVector[range]);
}

bool DebugWindow::onAssLoad(int range)
{
	setOut1(dirAssSlotVector[range]);

	wxString numberStr = XRCCTRL(*this, "AssFileNumber", wxTextCtrl)->GetValue();
    wxString assDir;
    if (dirAssDirNameVector[range].Right(1) != pathSeparator_)
        assDir = dirAssDirNameVector[range]+pathSeparator_;
    else
        assDir = dirAssDirNameVector[range];

	long number;

	if (numberStr.ToLong(&number))
	{
//		if (dirAssFileNameVector[range] == "sb.0000.bin")
//			number+=2;
//		if (dirAssFileNameVector[range] == "sb.e000.bin")
//			number-=3;
		numberStr = "." + numberStr;

		if (!wxFile::Exists(assDir+dirAssFileNameVector[range]+numberStr))
			return false;

		if (!wxFile::Exists(assDir+dirAssFileNameVector[range].Left(dirAssFileNameVector[range].Len()-4)+".debug"+numberStr))
			return false;

		p_Computer->readProgram(assDir, dirAssFileNameVector[range]+numberStr, NOCHANGE, dirAssProgramStartVector[range], NONAME);
		p_Computer->readDebugFile(assDir, dirAssFileNameVector[range], numberStr, dirAssProgramStartVector[range]);
	}
	else
	{
		p_Computer->readProgram(assDir, dirAssFileNameVector[range], NOCHANGE, dirAssProgramStartVector[range], NONAME);
		p_Computer->readDebugFile(assDir, dirAssFileNameVector[range], "", dirAssProgramStartVector[range]);
	}
	return true;
}


void DebugWindow::AssAddConfig(wxString dir, wxString name, Word programStart, Word programEnd, Word dataEnd, Byte slot)
{
	lastRange_++;
	dirAssProgramStartVector.resize(lastRange_);
	dirAssProgramEndVector.resize(lastRange_);
	dirAssDataEndVector.resize(lastRange_);
	dirAssDirNameVector.resize(lastRange_);
	dirAssFileNameVector.resize(lastRange_);
	dirAssSlotVector.resize(lastRange_);

	AssStoreConfig(lastRange_-1, dir, name, programStart, programEnd, dataEnd, slot);
}

void DebugWindow::AssStoreConfig(int range, wxString dir, wxString name, Word programStart, Word programEnd, Word dataEnd, Byte slot)
{
	dirAssProgramStartVector[range] = programStart;
	dirAssProgramEndVector[range] = programEnd;
	dirAssDataEndVector[range]  = dataEnd;
	dirAssDirNameVector[range] = dir;
	dirAssFileNameVector[range] = name;
	dirAssSlotVector[range] = slot;
}

void DebugWindow::AssInitConfig()
{
	dirAssStart_ = 0;
	dirAssEnd_ = 0;
	workingRange_ = -1;
	lastAssError_ = "";

    AssInitLog();
	lastLogValue_ = -1;
}

void DebugWindow::AssInitLog()
{
    if (runningComputer_ == COMX && conf[COMX].videoLog_)
    {
        wxString fileName = debugDir_ + "debug.log";
        if (wxFile::Exists(fileName))
            dirAssLogFile_.Open(fileName, wxFile::write);
        else
            dirAssLogFile_.Create(fileName);
    }
}

void DebugWindow::onAssCopy(wxCommandEvent&WXUNUSED(event))
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}

	long start = get16BitValue("AssCopyStart");
	long end = get16BitValue("AssCopyEnd");
	long destination = get16BitValue("AssCopyTo");
	if (start == -1 || end == -1 || destination == -1)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_NO_RANGE-ERROR_START-1]);
		return;
	}

	long address = start;
	long correctAddress = destination;
	long moveCorrection = destination - start;
	long length = end - start;
	Byte chip8_instruction;

	if ((moveCorrection &0xff) == 0)
	{
		if (destination <= start)
		{
			while(address <= end)
			{
				p_Computer->writeMem(destination, p_Computer->readMem(address), true);
				p_Computer->writeMemDataType(destination++, p_Computer->readMemDataType(address++));
			}
		}
		else
		{
			address = end;
			destination += length;
			while(address >= start)
			{
				p_Computer->writeMem(destination, p_Computer->readMem(address), true);
				p_Computer->writeMemDataType(destination--, p_Computer->readMemDataType(address--));
			}
			destination += length;
		}

		Word branchAddr;
		while (correctAddress <= destination)
		{
			if (p_Computer->readMemDataType(correctAddress) == MEM_TYPE_OPCODE || p_Computer->readMemDataType(correctAddress) == MEM_TYPE_OPCODE_LBR_SLOT)
			{
				if (jumpCorrection[p_Computer->readMem(correctAddress)] == 2) // long branch
				{
					branchAddr = (p_Computer->readMem(correctAddress+1) << 8) +  p_Computer->readMem(correctAddress+2);
					if (branchAddr >= start && branchAddr <= end)
					{
						branchAddr += moveCorrection;
						p_Computer->writeMem(correctAddress+1, (branchAddr&0xff00)>>8, true);
						p_Computer->writeMem(correctAddress+2, branchAddr&0xff, true);
					}
				}
				if (jumpCorrection[p_Computer->readMem(correctAddress)] == 3) // SCAL RN,xxxx + DBNZ
				{
					if ((p_Computer->readMem(correctAddress+1)&0xf0) == 0x80 || (p_Computer->readMem(correctAddress+1)&0xf0) == 0x20)
					{
						branchAddr = (p_Computer->readMem(correctAddress+2) << 8) +  p_Computer->readMem(correctAddress+3);
						if (branchAddr >= start && branchAddr <= end)
						{
							branchAddr += moveCorrection;
							p_Computer->writeMem(correctAddress+2, (branchAddr&0xff00)>>8, true);
							p_Computer->writeMem(correctAddress+3, branchAddr&0xff, true);
						}
					}
				}
			}	
			if (p_Computer->readMemDataType(correctAddress) == MEM_TYPE_JUMP || p_Computer->readMemDataType(correctAddress) == MEM_TYPE_OPCODE_JUMP_SLOT)
			{
				branchAddr = (p_Computer->readMem(correctAddress) << 8) +  p_Computer->readMem(correctAddress+1);
				if (branchAddr >= start && branchAddr <= end)
				{
					branchAddr += moveCorrection;
					p_Computer->writeMem(correctAddress, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(correctAddress+1, branchAddr&0xff, true);
				}
			}
			if (p_Computer->readMemDataType(correctAddress) == MEM_TYPE_JUMP_REV)
			{
				branchAddr = (p_Computer->readMem(correctAddress+1) << 8) +  p_Computer->readMem(correctAddress);
				if (branchAddr >= start && branchAddr <= end)
				{
					branchAddr += moveCorrection;
					p_Computer->writeMem(correctAddress+1, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(correctAddress, branchAddr&0xff, true);
				}
			}
			if (p_Computer->readMemDataType(correctAddress) == MEM_TYPE_OPCODE_LDL || p_Computer->readMemDataType(correctAddress) == MEM_TYPE_OPCODE_LDL_SLOT)
			{
				branchAddr = (p_Computer->readMem(correctAddress+1) << 8) +  p_Computer->readMem(correctAddress+4);
				if (branchAddr >= start && branchAddr <= end)
				{
					branchAddr += moveCorrection;
					p_Computer->writeMem(correctAddress+1, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(correctAddress+4, branchAddr&0xff, true);
				}
			}
			if (p_Computer->readMemDataType(correctAddress) == MEM_TYPE_OPCODE_RLDL)
			{
				branchAddr = (p_Computer->readMem(correctAddress+2) << 8) +  p_Computer->readMem(correctAddress+3);
				if (branchAddr >= start && branchAddr <= end)
				{
					branchAddr += moveCorrection;
					p_Computer->writeMem(correctAddress+2, (branchAddr&0xff00)>>8, true);
					p_Computer->writeMem(correctAddress+3, branchAddr&0xff, true);
				}
			}
			if (p_Computer->readMemDataType(correctAddress) == MEM_TYPE_CHIP_8_1)
			{
				chip8_instruction = p_Computer->readMem(correctAddress)&0xf0;
				switch (chip8_instruction)
				{
					case 0:
					case 0x10:
					case 0x20:
					case 0xA0:
					case 0xB0:
						branchAddr = ((p_Computer->readMem(correctAddress)&0xf) << 8) +  p_Computer->readMem(correctAddress+1);
						if (branchAddr>start && branchAddr < end)
						{
							branchAddr += moveCorrection;
							p_Computer->writeMem(correctAddress, chip8_instruction | ((branchAddr&0xf00)>>8), true);
							p_Computer->writeMem(correctAddress+1, branchAddr&0xff, true);
						}
					break;
				}
			}
            if (p_Computer->readMemDataType(correctAddress) == MEM_TYPE_FEL2_1)
            {
				switch (p_Computer->getChip8Type())
				{
					case CHIPFEL1:
					case CHIPFEL2:
					case CHIPFEL3:
						chip8_instruction = p_Computer->readMem(correctAddress)&0xf0;
						switch (chip8_instruction)
						{
							case 0:
							case 0x10:
							case 0xF0:
								branchAddr = ((p_Computer->readMem(correctAddress)&0xf) << 8) +  p_Computer->readMem(correctAddress+1);
								if (branchAddr>start && branchAddr < end)
								{
									branchAddr += moveCorrection;
									p_Computer->writeMem(correctAddress, chip8_instruction | ((branchAddr&0xf00)>>8), true);
									p_Computer->writeMem(correctAddress+1, branchAddr&0xff, true);
								}
							break;
						}
					break;
					
					case CHIPSTIV:
						chip8_instruction = p_Computer->readMem(correctAddress);
                        if ((chip8_instruction & 0xf0) == 0x0 || (chip8_instruction & 0xf0) == 0x10 || (chip8_instruction & 0xf0) == 0x20 || (chip8_instruction & 0xf0) == 0x30 || (chip8_instruction & 0xf0) == 0x80 || (chip8_instruction & 0xf0) == 0x90)
							chip8_instruction &= 0xf0;
						switch (chip8_instruction)
						{
                            case 0x0:
                            case 0x10:
                            case 0x20:
                            case 0x30:
							case 0x60:
							case 0x61:
							case 0x66:
							case 0x67:
							case 0x68:
								branchAddr = ((p_Computer->readMem(correctAddress)&0xf) << 8) +  p_Computer->readMem(correctAddress+1);
								if (branchAddr>start && branchAddr < end)
								{
									branchAddr += moveCorrection;
									p_Computer->writeMem(correctAddress, chip8_instruction | ((branchAddr&0xf00)>>8), true);
									p_Computer->writeMem(correctAddress+1, branchAddr&0xff, true);
								}
							break;
                        }
					break;
				}
            }
			if (p_Computer->readMemDataType(correctAddress) == MEM_TYPE_ST2_1)
			{
				chip8_instruction = p_Computer->readMem(correctAddress)&0xf0;
				switch (chip8_instruction)
				{
					case 0:
					case 0x10:
					case 0x20:
					case 0xA0:
						branchAddr = ((p_Computer->readMem(correctAddress)&0xf) << 8) +  p_Computer->readMem(correctAddress+1);
						if (branchAddr>start && branchAddr < end)
						{
							branchAddr += moveCorrection;
							p_Computer->writeMem(correctAddress, chip8_instruction | ((branchAddr&0xf00)>>8), true);
							p_Computer->writeMem(correctAddress+1, branchAddr&0xff, true);
						}
					break;
				}
			}

			correctAddress++;
		}
		assErrorDisplay(DirAssErrorCodes[ERROR_COPIED_JUMPS-ERROR_START-1]);
	}
	else
	{
		if (destination <= start)
		{
			while(address <= end)
			{
				p_Computer->writeMem(destination++, p_Computer->readMem(address++), true);
			}
		}
		else
		{
			address = end;
			destination += length;
			while(address >= start)
			{
				p_Computer->writeMem(destination--, p_Computer->readMem(address--), true);
			}
		}
		assErrorDisplay(DirAssErrorCodes[ERROR_COPIED_NO_JUMPS-ERROR_START-1]);
	}
}

void DebugWindow::onAssDis(wxCommandEvent&WXUNUSED(event))
{
	if (!computerRunning_)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_COMPUTER_NOT_RUNNING-ERROR_START-1]);
		return;
	}

	wxTextFile outputTextFile;
	wxString fileName, number, printBufferOpcode;

	long start = get16BitValue("AssCopyStart");
	long end = get16BitValue("AssCopyEnd");
	if (start == -1 || end == -1)
	{
		assErrorDisplay(DirAssErrorCodes[ERROR_NO_RANGE-ERROR_START-1]);
		return;
	}
    
	fileName = wxFileSelector( "Select the assembler file to save",
                               debugDir_, fileName,
                               "",
                               wxString::Format
                              (
                                   "Assembler File (*.asm)|*.asm|Text File (*.txt)|*.txt"
                               ),
                               wxFD_SAVE|wxFD_CHANGE_DIR,
                               this
                              );

	if (!fileName || fileName.empty())
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	wxString name = FullPath.GetName();
	wxString path = FullPath.GetPath();
	wxString ext = FullPath.GetExt();

    disassembleAgain_ = true;
    disassemblePass_ = 1;
    
    outputTextFile.Create(fileName);
 
    for (int i=0; i<=0xffff;i++)
    {
        labelInfo_[i].labelDefined = false;
        labelInfo_[i].branchToFound = false;
        labelInfo_[i].outOfRange = false;
    }
    
    while (disassembleAgain_ && disassemblePass_ <= 2)
    {
        disassembleAgain_ = false;
        outputTextFile.Clear();
    
        Word address = (Word)start;
    
        wxString line, tempLine, characters, newChar, text, label, addressAndOpcode;
        Byte value;
        Word textStart, branchAddress;
        
        line.Printf("; Origin set to %05XH, EOF = %05XH", (Word)start, (Word)end);
        outputTextFile.AddLine(line);
        line.Printf("		ORG  %05XH", (Word)start);
        outputTextFile.AddLine(line);

        outputTextFile.AddLine("");
        outputTextFile.AddLine("; CPU Type:");
        if (getCpuType() <= CPU1802)
            line.Printf("		CPU 1802");
        else
            line.Printf("		CPU 1805");
        outputTextFile.AddLine(line);
        
        outputTextFile.AddLine("");
        outputTextFile.AddLine("; Register Definitions:");
        
        for (int i=0; i<16; i++)
        {
            line.Printf("R%01X		EQU %d", i, i);
            outputTextFile.AddLine(line);
        }
        
        outputTextFile.AddLine("");
        outputTextFile.AddLine("; Start code segment");
        
        while(address <= end && address >= start)
        {
            if (start == 0 && address >= 0xfff8)
                start += 8;

            Byte memType = p_Computer->readMemDataType(address);

            label = getCurrentAddresssLabel(address);
            if (label != "")
            {
                outputTextFile.AddLine(label);
                labelInfo_[address].labelDefined = true;
            }
        
            switch (memType)
            {
                case MEM_TYPE_OPCODE:
                case MEM_TYPE_OPCODE_RSHR:
                case MEM_TYPE_OPCODE_RSHL:
                case MEM_TYPE_OPCODE_BPZ:
                case MEM_TYPE_OPCODE_BGE:
                case MEM_TYPE_OPCODE_BM:
                case MEM_TYPE_OPCODE_BL:
                case MEM_TYPE_OPCODE_LSKP:
                case MEM_TYPE_OPCODE_SKP:
                case MEM_TYPE_OPCODE_RLDL:
                    line = cdp1802disassemble(&address, false, true, TEXT_ASSEMBLER, start, end);
                break;
                    
                case MEM_TYPE_OPCODE_LBR_SLOT:
                    text.Printf(" - Slot %02X", p_Computer->readMemDataType(address+1));
                    line = cdp1802disassemble(&address, false, true, TEXT_ASSEMBLER, start, end);
                    line = line.Left(5)+line.Mid(9, 12) + "    "+line.Mid(21);
                    line.Trim();
                    line += text;
                break;
                    
                case MEM_TYPE_OPCODE_LDV:
                    addressAndOpcode.Printf(";%04X: %02X %02X %02X", address, p_Computer->readMem(address), p_Computer->readMem(address+1), p_Computer->readMem(address+2));
                    line.Printf("LOAD R%01X,", p_Computer->readMem(address+2)&0xf);
                    line += getLoadAddress(address+1);
                    while (line.Len()<= 20)
                        line += " ";
                    line += addressAndOpcode;
                    address+=3;
                    address&=0xffff;
                break;
                    
                case MEM_TYPE_OPCODE_LDL:
                    addressAndOpcode.Printf(";%04X: %02X %02X %02X", address, p_Computer->readMem(address), p_Computer->readMem(address+1), p_Computer->readMem(address+2));
                    line.Printf("LOAD R%01X,", p_Computer->readMem(address+2)&0xf);
                    line += getLoadAddressOrLabel(address+1, start, end);
                    while (line.Len()<= 20)
                        line += " ";
                    line += addressAndOpcode;
                    address+=3;
                    address&=0xffff;
                break;
                    
                case MEM_TYPE_OPCODE_LDL_SLOT:
                    addressAndOpcode.Printf(";%04X: %02X %02X %02X - Slot %02X", address, p_Computer->readMem(address), p_Computer->readMem(address+1), p_Computer->readMem(address+2), p_Computer->readMemDataType(address+1));
                    line.Printf("LOAD R%01X,", p_Computer->readMem(address+2)&0xf);
                    line = line + getLoadAddressOrLabel(address+1, start, end);
                    while (line.Len()<= 20)
                        line += " ";
                    line += addressAndOpcode;
                    address+=3;
                    address&=0xffff;
                break;
                    
                case MEM_TYPE_OPERAND_LD_3:
                    addressAndOpcode.Printf(";      %02X %02X %02X", p_Computer->readMem(address), p_Computer->readMem(address+1), p_Computer->readMem(address+2));
                    line = "";
                    while (line.Len()<= 20)
                        line += " ";
                    line += addressAndOpcode;
                    address+=3;
                    address&=0xffff;
                break;
                    
                case MEM_TYPE_CHIP_8_1:
                    tempLine = chip8Disassemble(address, false, true);

                    value = p_Computer->readMem(address++);
                    if (value >= 0xa0)
                        line.Printf("DB   %03XH", value);
                    else
                        line.Printf("DB   %02XH", value);
                    value = p_Computer->readMem(address++);
                    if (value >= 0xa0)
                        text.Printf(", %03XH", value);
                    else
                        text.Printf(", %02XH", value);
                    line = line + text;
                    while (line.Len() <= 20)
                        line += " ";
                    line = line + ";"+ tempLine.Left(6)+tempLine.Right(tempLine.Len()-13);
                    
                    address&=0xffff;
                break;
                    
                case MEM_TYPE_FEL2_1:
                    tempLine = fel2Disassemble(address, false, true);
                    
                    value = p_Computer->readMem(address++);
                    if (value >= 0xa0)
                        line.Printf("DB   %03XH", value);
                    else
                        line.Printf("DB   %02XH", value);

                    switch (value)
                    {
                        case 0xC0:
                        case 0xC4:
                        case 0xCC:
                            if (p_Computer->getChip8Type() != CHIPFEL2)
                            {
                                value = p_Computer->readMem(address++);
                                if (value >= 0xa0)
                                    text.Printf(", %03XH", value);
                                else
                                    text.Printf(", %02XH", value);
                                line = line + text;
                            }
                        break;
                        
                        default:
                            value = p_Computer->readMem(address++);
                            if (value >= 0xa0)
                                text.Printf(", %03XH", value);
                            else
                                text.Printf(", %02XH", value);
                            line = line + text;
                        break;
                    }

                    while (line.Len() <= 20)
                        line += " ";
                    line = line + ";"+ tempLine.Left(6)+tempLine.Right(tempLine.Len()-13);

                    address&=0xffff;
                break;
                    
                case MEM_TYPE_ST2_1:
                    tempLine = st2Disassemble(address, false, true);

                    value = p_Computer->readMem(address++);
                    if (value >= 0xa0)
                        line.Printf("DB   %03XH", value);
                    else
                        line.Printf("DB   %02XH", value);
                        
                    switch (value)
                    {
                        case 0xC0:
                        case 0xE0:
                        case 0xE1:
                        case 0xE2:
                        case 0xE4:
                        break;
                        
                        default:
                            value = p_Computer->readMem(address++);
                            if (value >= 0xa0)
                                text.Printf(", %03XH", value);
                            else
                                text.Printf(", %02XH", value);
                            line = line + text;
                        break;
                    }
                    
                    while (line.Len() <= 20)
                            line += " ";
                    line = line + ";"+ tempLine.Left(6)+tempLine.Right(tempLine.Len()-13);

                    address&=0xffff;
                break;
                    
                case MEM_TYPE_OPCODE_JUMP_SLOT:
                    addressAndOpcode.Printf(";%04X: %02X %02X - Slot %02X", address, p_Computer->readMem(address), p_Computer->readMem(address + 1), p_Computer->readMemDataType(address+1));
                    line = "DW   " + getSubAddressOrLabel(address, TEXT_ASSEMBLER, start, end);
                    while (line.Len()<= 20)
                        line += " ";
                    line += addressAndOpcode;
                    address+=2;
                    address&=0xffff;
                break;
                    
                case MEM_TYPE_JUMP:
                    addressAndOpcode.Printf(";%04X: %02X %02X", address, p_Computer->readMem(address), p_Computer->readMem(address + 1));
                    line = "DW   " + getSubAddressOrLabel(address, TEXT_ASSEMBLER, start, end);
                    while (line.Len()<= 20)
                        line += " ";
                    line += addressAndOpcode;
                    address+=2;
                    address&=0xffff;
                break;
                    
                case MEM_TYPE_JUMP_REV:
                    branchAddress = p_Computer->readMem(address+1)<<8|p_Computer->readMem(address);
                    if (p_Computer->readMemLabelType(branchAddress) >= LABEL_TYPE_SUB)
                    {
                        addressAndOpcode.Printf(";%04X: %02X", address, p_Computer->readMem(address));
                        line.Printf("DW   LOW S%02X", branchAddress);
                        while (line.Len()<= 20)
                            line += " ";
                        line += addressAndOpcode;
                        outputTextFile.AddLine("		" + line);
                        addressAndOpcode.Printf(";%04X: %02X", address, p_Computer->readMem(address+1));
                        line.Printf("DW   HIGH S%02X", branchAddress);
                    }
                    else
                    {
                        if (disassemblePass_ == 1)
                        {
                            p_Computer->writeMemLabelType(branchAddress, LABEL_TYPE_SUB);
                            disassembleAgain_ = true;
                            addressAndOpcode.Printf(";%04X: %02X", address, p_Computer->readMem(address));
                            line.Printf("DW   LOW S%02X", branchAddress);
                            while (line.Len()<= 20)
                                line += " ";
                            line += addressAndOpcode;
                            outputTextFile.AddLine("		" + line);
                            addressAndOpcode.Printf(";%04X: %02X", address, p_Computer->readMem(address+1));
                            line.Printf("DW   HIGH S%02X", branchAddress);
                        }
                        else
                        {
                            addressAndOpcode.Printf(";%04X: %02X %02X", address, p_Computer->readMem(address), p_Computer->readMem(address + 1));
                            if (branchAddress >= 0xA000)
                                line.Printf("DW   %05XH", branchAddress);
                            else
                                line.Printf("DW   %04XH", branchAddress);
                        }
                    }
                    while (line.Len()<= 20)
                        line += " ";
                    line += addressAndOpcode;
                    address+=2;
                    address&=0xffff;
                break;
                    
                case MEM_TYPE_TEXT:
                    text = "";
                    textStart = address;
                    while (memType == MEM_TYPE_TEXT)
                    {
                        value = p_Computer->readMem(address);
                        characters = " ";
                        if (value > 0x20)
                            characters.SetChar(0, value&0x7f);
                        text = text + characters;
                        address++;
                        address&=0xffff;
                        memType = p_Computer->readMemDataType(address);
                    }
                    addressAndOpcode.Printf(";%04X: %02X", textStart, p_Computer->readMem(textStart));
                    line = "DB   '"+text+"'";
                    while (line.Len()<= 20)
                        line += " ";
                    line += addressAndOpcode;
                    
                    textStart++;
                    memType = p_Computer->readMemDataType(textStart);
                    
                    if (memType == MEM_TYPE_TEXT)
                        outputTextFile.AddLine("		" + line);
                    
                    while (memType == MEM_TYPE_TEXT)
                    {
                        value = p_Computer->readMem(textStart);
                        addressAndOpcode.Printf(";%04X: %02X", textStart, value);
                        line = "";
                        while (line.Len()<= 20)
                            line += " ";
                        line += addressAndOpcode;
                        textStart++;
                        textStart&=0xffff;
                        memType = p_Computer->readMemDataType(textStart);
                        if (memType == MEM_TYPE_TEXT)
                            outputTextFile.AddLine("		" + line);
                    }
                break;
                    
                default:
                    value = p_Computer->readMem(address);
                    addressAndOpcode.Printf(";%04X: %02X", address, value);
                    if (value >= 0xa0)
                        line.Printf("DB   %03XH", value);
                    else
                        line.Printf("DB   %02XH", value);
                    while (line.Len()<= 20)
                        line += " ";
                    line += addressAndOpcode;
                    address++;
                    address&=0xffff;
                break;
            }
            outputTextFile.AddLine("		" + line);
        }
        outputTextFile.AddLine("		END");
        disassemblePass_++;
    }
    
    size_t lineNumber = 5;
    bool firstLabelFound = true;
    wxString AddressString, label, outOfRangeAddress;
    for (int i=0; i<=0xffff;i++)
    {
        if (labelInfo_[i].labelDefined && labelInfo_[i].branchToFound)
        {
            if (firstLabelFound)
            {
                outputTextFile.InsertLine("", lineNumber++);
                outputTextFile.InsertLine("; Labels:", lineNumber++);
                firstLabelFound = false;
            }
            label = getCurrentAddresssLabel(i);
            if (label != "")
            {
                if (i >= 0xA000)
                    AddressString.Printf("%05XH", i);
                else
                    AddressString.Printf("%04XH", i);
                
                outputTextFile.InsertLine(label + "	EQU " + AddressString, lineNumber++);
            }
        }
    }

    firstLabelFound = true;
    for (int i=0; i<=0xffff;i++)
    {
        if (labelInfo_[i].labelDefined && !labelInfo_[i].branchToFound)
        {
            if (firstLabelFound)
            {
                outputTextFile.InsertLine("", lineNumber++);
                outputTextFile.InsertLine("; Unused or indirect labels:", lineNumber++);
                firstLabelFound = false;
            }
            label = getCurrentAddresssLabel(i);
            if (label != "")
                outputTextFile.InsertLine("; " + label, lineNumber++);
        }
    }

    firstLabelFound = true;
    for (int i=0; i<=0xffff;i++)
    {
        if (labelInfo_[i].outOfRange)
        {
            if (firstLabelFound)
            {
                outputTextFile.InsertLine("", lineNumber++);
                outputTextFile.InsertLine("; Out of range branches or addresses:", lineNumber++);
                firstLabelFound = false;
            }
            if (i >= 0xA000)
                AddressString.Printf("%05XH", i);
            else
                AddressString.Printf("%04XH", i);
            if (labelInfo_[i].outOfRangeAddress >= 0xA000)
                outOfRangeAddress.Printf("%05XH", labelInfo_[i].outOfRangeAddress);
            else
                outOfRangeAddress.Printf("%04XH", labelInfo_[i].outOfRangeAddress);
            outputTextFile.InsertLine("; " + outOfRangeAddress + " referenced from " + AddressString, lineNumber++);
        }
    }
    outputTextFile.Write();
    outputTextFile.Close();
}

void DebugWindow::stopAssLog()
{
	if (runningComputer_ == COMX && conf[COMX].videoLog_)
	{
		dirAssLogFile_.Close();
	}
}

void DebugWindow::assLog(Byte value)
{
	if (!(runningComputer_ == COMX && conf[COMX].videoLog_))
		return;

	wxString character;
	Byte converted = value&0x7f;

	if (value == 0 && lastLogValue_ != -1)
	{
		converted = 0x0d;
		dirAssLogFile_.Write(&converted, 1);
		converted = 0x0a;
		dirAssLogFile_.Write(&converted, 1);
		lastLogValue_ = -1;
	}
	else
	{
		if (value == 0)
		{
			if (lastLogValue_ != -1)
				lastLogValue_ = value;
		}
		else
			lastLogValue_ = value;
	}

	if (runningComputer_ == COMX && conf[COMX].videoLog_)
	{
		if (value != 0xff && value != 0 && lastLogValue_!= -1)
			dirAssLogFile_.Write(&converted, 1);
	}
}

void DebugWindow::paintDebugBackground()
{
    for (int i=0; i<16; i++)
        lineBmp[i] = new wxBitmap(128, 16, 24);
    dcLine.SelectObject(*lineBmp[0]);
    switch (windowInfo.operatingSystem)
    {
        case OS_MAC:
            dcLine.SetPen(wxPen(wxColour(219, 219, 219)));
            dcLine.SetBrush(wxBrush(wxColour(219, 219, 219)));
            dcLine.SetTextBackground(wxColour(219, 219, 219));
        break;
            
        case OS_WINDOWS_2000:
            dcLine.SetPen(wxPen(wxColour(0xd4, 0xd0, 0xc8)));
            dcLine.SetBrush(wxBrush(wxColour(0xd4, 0xd0, 0xc8)));
            dcLine.SetTextBackground(wxColour(0xd4, 0xd0, 0xc8));
        break;
            
        case OS_LINUX_OPENSUSE_GNOME:
        case OS_LINUX_OPENSUSE_KDE:
            dcLine.SetPen(wxPen(wxColour(242, 241, 240)));
            dcLine.SetBrush(wxBrush(wxColour(242, 241, 240)));
            dcLine.SetTextBackground(wxColour(242, 241, 240));
        break;

        case OS_LINUX_UBUNTU_11_04:
        case OS_LINUX_UBUNTU_11_10:
            dcLine.SetPen(wxPen(wxColour(0xfb, 0xf8, 0xf1)));
            dcLine.SetBrush(wxBrush(wxColour(0xfb, 0xf8, 0xf1)));
            dcLine.SetTextBackground(wxColour(0xfb, 0xf8, 0xf1));
        break;

        case OS_LINUX_FEDORA:
        	dcLine.SetPen(wxPen(wxColour(232, 232, 231)));
        	dcLine.SetBrush(wxBrush(wxColour(232, 232, 231)));
        break;

        default:
            dcLine.SetPen(wxPen(wxColour(255,255,255)));
            dcLine.SetBrush(wxBrush(wxColour(255,255,255)));
            dcLine.SetTextBackground(wxColour(255,255,255));
        break;
    }
    dcLine.DrawRectangle(0, 0, 128, 16);

    wxMemoryDC dcDebugBackground;
    
    dcDebugBackground.SelectObject(*assBmp);
    switch (windowInfo.operatingSystem)
    {
        case OS_MAC:
            dcDebugBackground.SetPen(wxPen(wxColour(219, 219, 219)));
            dcDebugBackground.SetBrush(wxBrush(wxColour(219, 219, 219)));
        break;
            
        case OS_WINDOWS_2000:
			dcDebugBackground.SetPen(wxPen(wxColour(0xd4, 0xd0, 0xc8)));
            dcDebugBackground.SetBrush(wxBrush(wxColour(0xd4, 0xd0, 0xc8)));
        break;
            
        case OS_LINUX_OPENSUSE_GNOME:
        case OS_LINUX_OPENSUSE_KDE:
            dcDebugBackground.SetPen(wxPen(wxColour(0xfb, 0xf8, 0xf1)));
            dcDebugBackground.SetBrush(wxBrush(wxColour(0xfb, 0xf8, 0xf1)));
        break;

        case OS_LINUX_UBUNTU_11_04:
        case OS_LINUX_UBUNTU_11_10:
            dcDebugBackground.SetPen(wxPen(wxColour(242, 241, 240)));
            dcDebugBackground.SetBrush(wxBrush(wxColour(242, 241, 240)));
        break;

        case OS_LINUX_FEDORA:
        	dcDebugBackground.SetPen(wxPen(wxColour(232, 232, 231)));
        	dcDebugBackground.SetBrush(wxBrush(wxColour(232, 232, 231)));
        break;

        default:
            dcDebugBackground.SetPen(wxPen(wxColour(255,255,255)));
            dcDebugBackground.SetBrush(wxBrush(wxColour(255,255,255)));
        break;
    }
    
    dcDebugBackground.DrawRectangle(0, 0, ASS_WIDTH, numberOfDebugLines_*LINE_SPACE+4);
    
    dcDebugBackground.SelectObject(wxNullBitmap);
    XRCCTRL(*this, "AssBitmap", wxStaticBitmap)->SetBitmap(*assBmp);
}

void DebugWindow::onDebugDisplayPage(wxCommandEvent&WXUNUSED(event))
{
	if (xmlLoaded_)
		p_Main->updateMemoryTab();
}

void DebugWindow::DebugDisplayPage()
{
	if (!computerRunning_)
	{
		if (xmlLoaded_)
			XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("No emulation running");
		return;
	}

	long start = get16BitValue("DebugDisplayPage");
	if (start == -1)  return;
	XRCCTRL(*this, "DebugDisplayPage", HexEdit)->saveNumber((int)start);

	Word ramMask = getAddressMask();
	while (start > ramMask)
		start -=  (ramMask + 1);

	XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("");

	memoryStart_ = (unsigned int)start;
	p_Computer->setDebugMemoryStart(start);

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
			if (elfConfiguration[runningComputer_].useEms || elfConfiguration[runningComputer_].useRomMapper)
				XRCCTRL(*this, "DebugEmsPage", HexEdit)->changeNumber(p_Computer->getEmsPage());
		break;
	}

	wxString idReference, value;

	for (int x=0; x<16; x++)
	{
		idReference.Printf("TOP_HEADER%01X", x);
		value.Printf("  %01X", (unsigned int)(start+x)&0xf);
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
	}
	for (int y=0; y<16; y++)
	{
		idReference.Printf("MEM_HEADER%01X", y);
		XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(*wxBLACK);
        switch (p_Computer->getMemoryType((int)start/256))
		{
			case COMXEXPBOX:
				XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(wxColour(0x80, 0x80, 0xff));
			break;

            case EMSMEMORY:
            case ROMMAPPER:
                XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(wxColour(0xc8, 0xb4, 0x3e));
            break;
                
			case PAGER:
//				if (((start>>12)&0xf) == portExtender_)
				if (start >= 0x1000)
					XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(wxColour(0xa9, 0x3e, 0xac));
			break;
		}

		value.Printf("%04X", (unsigned int)start);
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

		ShowCharacters(start, y);
		for (int x=0; x<16; x++)
		{
			idReference.Printf("MEM%01X%01X", y, x);
			value.Printf("%02X", p_Computer->readMem(start));

			XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(*wxBLACK);
			if (p_Computer->getMemoryType((int)start/256) == COMXEXPBOX)
			{
                if (p_Computer->getExpansionMemoryType(p_Comx->getComxExpansionSlot(), (start&0x1fff)/256) == RAMBANK)
                    XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(wxColour(0x45, 0xac, 0x22));
                if (p_Computer->getExpansionMemoryType(p_Comx->getComxExpansionSlot(), (start&0x1fff)/256) == EPROMBANK)
                    XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(wxColour(0x80, 0x00, 0x40));
                if (p_Computer->getExpansionMemoryType(p_Comx->getComxExpansionSlot(), (start&0x1fff)/256) == SUPERBANK)
                    XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(wxColour(0x80, 0x00, 0x40));
			}

			XRCCTRL(*this, idReference, MemEdit)->ChangeValue("");
			XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);

			start++;
			while (start > ramMask)
				start -=  (ramMask + 1);
		}
	}
}

void DebugWindow::ShowCharacters(Word address, int y)
{
	wxString idReference;
	idReference.Printf("CHAR%01X", y);

	int t;
	char bits [9];

	dcLine.SelectObject(*lineBmp[y]);
	dcLine.DrawRectangle(0, 0, 128, 16);
#if defined(__WXMAC__)
	wxFont exactFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#else
	wxFont exactFont(7, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#endif

	for (int j=0; j<16; j++)
	{
		if ((runningComputer_ == COMX) || (runningComputer_ == TMC600) || (runningComputer_ == PECOM))
		{
			for (int i=0; i<9; i++)
			{
				if (runningComputer_ == COMX)
					t = p_Comx->readCramDirect((debugReadMem(address+j)&0x7f)*16+i);
				else if (runningComputer_ == TMC600)
					t = p_Tmc600->readCramDirect((debugReadMem(address+j)&0xff)*16+i);
				else
					t = p_Pecom->readCramDirect((debugReadMem(address+j)&0x7f)*16+i);
				bits[i] = (t & 0x1) << 5;
				bits[i] |= (t & 0x2) << 3;
				bits[i] |= (t & 0x4) << 1;
				bits[i] |= (t & 0x8) >> 1;
				bits[i] |= (t & 0x10) >> 3;
				bits[i] |= (t & 0x20) >> 5;
			}
			wxBitmap character(bits, 6, 9, 1);
			dcLine.DrawBitmap(character, j*8+1, 5, false);
		}
		else
		{
			wxString character;
			Byte byteValue = debugReadMem(address+j)&0x7f;
			character.Printf("%c", byteValue);
			dcLine.SetFont(exactFont);
/*			int offset = 1;
			if (byteValue == 0x4d || byteValue == 0x57)
				offset = 0;
			if (byteValue == 0x49)
				offset = 3;
			if (byteValue == 0x54)
				offset = 2;*/
			dcLine.DrawText(character, (j*8), 1);
		}
	}

	dcLine.SelectObject(wxNullBitmap);
	XRCCTRL(*this, idReference, wxStaticBitmap)->SetForegroundColour(*wxBLACK);
	XRCCTRL(*this, idReference, wxStaticBitmap)->SetBitmap(*lineBmp[y]);
}

void DebugWindow::DebugDisplayMap()
{
	if (!computerRunning_)
	{
		if (xmlLoaded_)
			XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("No emulation running");
		return;
	}

	wxString idReference, idReference2, value;
	bool textGreen, textOrange;

	XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("");

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
            if (elfConfiguration[runningComputer_].useEms || elfConfiguration[runningComputer_].useRomMapper)
				XRCCTRL(*this, "DebugEmsPage", HexEdit)->changeNumber(p_Computer->getEmsPage());
		break;
	}

	for (int x=0; x<16; x++)
	{
		idReference.Printf("TOP_HEADER%01X", x);
		value.Printf("  %01X", x);
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
	}
	for (int y=0; y<16; y++)
	{
		idReference.Printf("MEM_HEADER%01X", y);
		value.Printf("%04X", y*4096);
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
		XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(*wxBLACK);

		for (int x=0; x<16; x++)
		{
			textGreen = false;
			textOrange = false;
			switch (p_Computer->getMemoryType((y<<4)+x))
			{
				case UNDEFINED:
					value.Printf (" ");
				break;

				case RAM:
					value.Printf (".");
				break;

                case MAPPEDRAM:
                    value.Printf ("M.");
                break;
                    
                case MAPPEDROM:
                    value.Printf ("MR");
                break;
                    
				case VP570RAM:
					value.Printf ("E.");
				break;

				case COLOURRAM:
					value.Printf ("C.");
				break;

				case COMXEXPROM:
				case ROM:
					value.Printf ("R");
				break;

				case CARTRIDGEROM:
                case CRAM1870:
                    value.Printf ("CR");
                break;
                    
                case TESTCARTRIDGEROM:
                    value.Printf ("TC");
                break;
                
                case MULTICART:
                    value.Printf ("MC");
                break;
                    
                case MAPPEDMULTICART:
                    value.Printf ("MM");
                break;
                    
				case PRAM1870:
					value.Printf ("PR");
				break;

				case COMXEXPBOX:
					XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(wxColour(0x80, 0x80, 0xff));
					switch (p_Computer->getExpansionMemoryType(p_Comx->getComxExpansionSlot(), (y&1)*16+x))
					{
						case MC6845RAM:
							value.Printf ("M5");
						break;

						case MC6845REGISTERS:
							value.Printf ("MR");
						break;

						case RAMBANK:
							textGreen = true;
							switch (p_Computer->getBankMemoryType(p_Comx->getComxExpansionRamBank(), (y&1)*16+x))
							{
								case RAM:
									value.Printf (".");
								break;

								case ROM:
									value.Printf ("R");
								break;

								case UNDEFINED:
									value.Printf (" ");
								break;

								default:
									value.Printf ("xx");
								break;
							}
						break;

                        case EPROMBANK:
                            textOrange = true;
                            switch (p_Computer->getEpromBankMemoryType(p_Comx->getComxExpansionEpromBank(), (y&1)*16+x))
                            {
                                case RAM:
                                    value.Printf (".");
                                break;
                                
                                case ROM:
                                    value.Printf ("R");
                                break;
                                
                                case UNDEFINED:
                                    value.Printf (" ");
                                break;
                                
                                default:
                                    value.Printf ("xx");
                                break;
                            }
                            break;
                            
                        case SUPERBANK:
                            textOrange = true;
                            switch (p_Computer->getEpromBankMemoryType(p_Comx->getComxExpansionEpromBank(), (y&1)*16+x))
                            {
                                case RAM:
                                    value.Printf (".");
                                break;
                                
                                case ROM:
                                    value.Printf ("R");
                                break;
                                
                                case UNDEFINED:
                                    value.Printf (" ");
                                break;
                                
                                default:
                                    value.Printf ("xx");
                                break;
                            }
                            break;
                            
						case RAM:
							value.Printf (".");
						break;

						case ROM:
							value.Printf ("R");
						break;

						case UNDEFINED:
							value.Printf (" ");
						break;

						default:
							value.Printf ("xx");
						break;
					}
				break;

				case COPYFLOPROM:
					value.Printf ("CF");
				break;

				case COPYCOMXEXPROM:
					value.Printf ("CE");
				break;

                case ROMMAPPER:
                    XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(wxColour(0xc8, 0xb4, 0x3e));
                    
                    switch (p_Computer->getRomMapperMemoryType(y*16+x))
                    {
                        case RAM:
                            value.Printf (".");
                        break;
                        
                        case ROM:
                            value.Printf ("R");
                        break;
                        
                        case UNDEFINED:
                            value.Printf (" ");
                        break;
                        
                        default:
                            value.Printf ("xx");
                        break;
                    }
                break;

                case EMSMEMORY:
					XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(wxColour(0xc8, 0xb4, 0x3e));

					switch (p_Computer->getEmsMemoryType((y&3)*16+x))
					{
						case RAM:
							value.Printf (".");
						break;

						case ROM:
							value.Printf ("R");
						break;

						case UNDEFINED:
							value.Printf (" ");
						break;

						default:
							value.Printf ("xx");
						break;
					}
				break;

				case PAGER:
					if (y == portExtender_)
						XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(wxColour(0xa9, 0x3e, 0xac));
					switch (p_Computer->getPagerMemoryType(y*16+x))
					{
						case RAM:
							value.Printf (".");
						break;

						case ROM:
							value.Printf ("R");
						break;

						case UNDEFINED:
							value.Printf (" ");
						break;

						default:
							value.Printf ("xx");
						break;
					}
				break;

				case MC6847RAM:
					value.Printf ("M7");
				break;

				case MC6845RAM:
					value.Printf ("M5");
				break;

				case MC6845REGISTERS:
					value.Printf ("MR");
				break;

				default:
					value.Printf ("xx");
				break;
			}
			idReference2.Printf("MEM%01X%01X", y, x);
			XRCCTRL(*this, idReference2, wxTextCtrl)->ChangeValue("");
			XRCCTRL(*this, idReference2, MemEdit)->ChangeValue(value);
			XRCCTRL(*this, idReference2, MemEdit)->saveNumber(-1);
			if (textGreen)
				XRCCTRL(*this, idReference2, MemEdit)->SetForegroundColour(wxColour(0x45, 0xac, 0x22));
			else
			{
				if (textOrange)
					XRCCTRL(*this, idReference2, MemEdit)->SetForegroundColour(wxColour(0x80, 0x00, 0x40));
				else
					XRCCTRL(*this, idReference2, MemEdit)->SetForegroundColour(*wxBLACK);
			}
		}

		idReference.Printf("CHAR%01X", y);
		wxBitmap line(128, 16, 24);
		wxMemoryDC dcMapLine;

		dcMapLine.SelectObject(line);
#if defined(__linux__)
		dcMapLine.SetPen(wxPen(wxColour(0xfb, 0xf8, 0xf1)));
		dcMapLine.SetBrush(wxBrush(wxColour(0xfb, 0xf8, 0xf1)));
		dcMapLine.SetTextBackground(wxColour(0xfb, 0xf8, 0xf1));
#else
#if defined(__WXMAC__)
		dcMapLine.SetPen(wxPen(wxColour(223, 223, 223)));
		dcMapLine.SetBrush(wxBrush(wxColour(223, 223, 223)));
		dcMapLine.SetTextBackground(wxColour(223, 223, 223));
#else
		dcMapLine.SetPen(*wxWHITE_PEN);
		dcMapLine.SetBrush(*wxWHITE_BRUSH);
		dcMapLine.SetTextBackground(wxColour(255,255,255));
#endif
#endif
		dcMapLine.DrawRectangle(0, 0, 128, 16);

		dcMapLine.SelectObject(wxNullBitmap);
		XRCCTRL(*this, idReference, wxStaticBitmap)->SetBitmap(line);
	}
}

void DebugWindow::DebugDisplay1870VideoRam()
{
    if (!(runningComputer_ == COMX || runningComputer_ == CIDELSA || runningComputer_ ==  TMC600 || runningComputer_ == PECOM))
    {
        if (xmlLoaded_)
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("CDP 1870 not running");
        return;
    }

	long start = get16BitValue("DebugDisplayPage");
	if (start == -1)  return;
	XRCCTRL(*this, "DebugDisplayPage", HexEdit)->saveNumber((int)start);

	wxString idReference, value;

	Word ramMask = getAddressMask();
	while (start > ramMask)  
		start -=  (ramMask + 1);

	memoryStart_ = (unsigned int)start;
	p_Computer->setDebugMemoryStart(start);

	XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("");
	//XRCCTRL(*this, "DebugDisplayPage", HexEdit)->changeNumber(memoryStart_);

	for (int x=0; x<16; x++)
	{
		idReference.Printf("TOP_HEADER%01X", x);
		value.Printf("  %01X", (unsigned int)(start+x)&0xf);
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
	}
	for (int y=0; y<16; y++)
	{
		idReference.Printf("MEM_HEADER%01X", y);
		XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(*wxBLACK);

		value.Printf("%04X", (unsigned int)start);
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

		ShowCharacters(start, y);
		for (int x=0; x<16; x++)
		{
			idReference.Printf("MEM%01X%01X", y, x);
			value.Printf("%02X", debugReadMem(start));

			XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(*wxBLACK);

			XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue("");
			XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);

			start++;
			while (start > ramMask)  
			start -=  (ramMask + 1);
		}
	}
}

void DebugWindow::DebugDisplay1870ColourRam()
{
    if (!(runningComputer_ ==  TMC600 || runningComputer_ == CIDELSA ))
    {
        if (xmlLoaded_)
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Colour RAM not used");
        return;
    }
    
    long start = get16BitValue("DebugDisplayPage");
    if (start == -1)  return;
    XRCCTRL(*this, "DebugDisplayPage", HexEdit)->saveNumber((int)start);
    
    wxString idReference, value;
    
    Word ramMask = getAddressMask();
    while (start > ramMask)
        start -=  (ramMask + 1);
    
    memoryStart_ = (unsigned int)start;
    p_Computer->setDebugMemoryStart(start);
    
    XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("");
    //XRCCTRL(*this, "DebugDisplayPage", HexEdit)->changeNumber(memoryStart_);
    
    for (int x=0; x<16; x++)
    {
        idReference.Printf("TOP_HEADER%01X", x);
        value.Printf("  %01X", (unsigned int)(start+x)&0xf);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
    }
    for (int y=0; y<16; y++)
    {
        idReference.Printf("MEM_HEADER%01X", y);
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(*wxBLACK);
        
        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
        
        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            value.Printf("%01X", debugReadMem(start));
            
            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(*wxBLACK);
            
            XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue("");
            XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);
            
            start++;
            while (start > ramMask)  
                start -=  (ramMask + 1);
        }
    }
}

void DebugWindow::DebugDisplay1864ColorRam()
{
	if (!(runningComputer_ == TMC2000 || runningComputer_ == VIP || runningComputer_ == VIPII || runningComputer_ ==  ETI ))
	{
		if (xmlLoaded_)
			XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("1864 Color RAM not used");
		return;
	}

	long start = get16BitValue("DebugDisplayPage");
	if (start == -1)  return;
	XRCCTRL(*this, "DebugDisplayPage", HexEdit)->saveNumber((int)start);

	wxString idReference, value;

	Word ramMask = getAddressMask();
	while (start > ramMask)  
		start -=  (ramMask + 1);

	memoryStart_ = (unsigned int)start;
	p_Computer->setDebugMemoryStart(start);

	XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("");
	//XRCCTRL(*this, "DebugDisplayPage", HexEdit)->changeNumber(memoryStart_);

	for (int x=0; x<16; x++)
	{
		idReference.Printf("TOP_HEADER%01X", x);
		value.Printf("  %01X", (unsigned int)((start+x)&0xf));
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
	}
	for (int y=0; y<16; y++)
	{
		idReference.Printf("MEM_HEADER%01X", y);
		XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(*wxBLACK);

		value.Printf("%04X", (unsigned int)start);
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

		ShowCharacters(start, y);
		for (int x=0; x<16; x++)
		{
			idReference.Printf("MEM%01X%01X", y, x);
			value.Printf("%02X", debugReadMem(start));

			XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(*wxBLACK);

			XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue("");
			XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);

			start++;
			while (start > ramMask)  
			start -=  (ramMask + 1);
		}
	}
}

void DebugWindow::DebugDisplay6845CharRom()
{
	switch (runningComputer_)
	{
		case COMX:
		break;

		case ELF:
		case ELFII:
		case SUPERELF:
			if (!(elfConfiguration[runningComputer_].use6845 || elfConfiguration[runningComputer_].useS100))  
			{
				XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("MC6845 not running");
				return;
			}
		break;

		default:
			if (xmlLoaded_)
				XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("MC6845 not running");
			return;
		break;
	}

	long start = get16BitValue("DebugDisplayPage");
	if (start == -1)  return;
	XRCCTRL(*this, "DebugDisplayPage", HexEdit)->saveNumber((int)start);

	wxString idReference, value;

	Word ramMask = getAddressMask();
	while (start > ramMask)  
		start -=  (ramMask + 1);

	memoryStart_ = (unsigned int)start;
	p_Computer->setDebugMemoryStart(start);

	XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("");
	//XRCCTRL(*this, "DebugDisplayPage", HexEdit)->changeNumber(memoryStart_);

	for (int x=0; x<16; x++)
	{
		idReference.Printf("TOP_HEADER%01X", x);
		value.Printf("  %01X", (unsigned int)((start+x)&0xf));
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
	}
	for (int y=0; y<16; y++)
	{
		idReference.Printf("MEM_HEADER%01X", y);
		XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(*wxBLACK);

		value.Printf("%04X", (unsigned int)start);
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

		ShowCharacters(start, y);
		for (int x=0; x<16; x++)
		{
			idReference.Printf("MEM%01X%01X", y, x);
			value.Printf("%02X", debugReadMem(start));

			XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(*wxBLACK);

			XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue("");
			XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);

			start++;
			while (start > ramMask)  
			start -=  (ramMask + 1);
		}
	}
}

void DebugWindow::DebugDisplay8275CharRom()
{
	switch (runningComputer_)
	{
		case ELF:
		case ELFII:
		case SUPERELF:
		case ELF2K:
			if (!elfConfiguration[runningComputer_].use8275)  
			{
				XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Intel 8275 not running");
				return;
			}
		break;

		default:
			XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Intel 8275 not running");
			return;
		break;
	}

	long start = get16BitValue("DebugDisplayPage");
	if (start == -1)  return;
	XRCCTRL(*this, "DebugDisplayPage", HexEdit)->saveNumber((int)start);

	wxString idReference, value;

	Word ramMask = getAddressMask();
	while (start > ramMask)  
		start -=  (ramMask + 1);

	memoryStart_ = (unsigned int)start;
	p_Computer->setDebugMemoryStart(start);

	XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("");
	//XRCCTRL(*this, "DebugDisplayPage", HexEdit)->changeNumber(memoryStart_);

	for (int x=0; x<16; x++)
	{
		idReference.Printf("TOP_HEADER%01X", x);
		value.Printf("  %01X", (unsigned int)((start+x)&0xf));
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
	}
	for (int y=0; y<16; y++)
	{
		idReference.Printf("MEM_HEADER%01X", y);
		XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(*wxBLACK);

		value.Printf("%04X", (unsigned int)start);
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

		ShowCharacters(start, y);
		for (int x=0; x<16; x++)
		{
			idReference.Printf("MEM%01X%01X", y, x);
			value.Printf("%02X", debugReadMem(start));

			XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(*wxBLACK);

			XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue("");
			XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);

			start++;
			while (start > ramMask)  
			start -=  (ramMask + 1);
		}
	}
}

void DebugWindow::DebugDisplay8275VideoRam()
{
    switch (runningComputer_)
    {
        case ELF:
        case ELFII:
        case SUPERELF:
        case ELF2K:
            if (!elfConfiguration[runningComputer_].use8275)
            {
                XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Intel 8275 not running");
                return;
            }
            break;
            
        default:
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Intel 8275 not running");
            return;
            break;
    }
    
    long start = get16BitValue("DebugDisplayPage");
    if (start == -1)  return;
    XRCCTRL(*this, "DebugDisplayPage", HexEdit)->saveNumber((int)start);
    
    wxString idReference, value;
    
    Word ramMask = getAddressMask();
    while (start > ramMask)
        start -=  (ramMask + 1);
    
    memoryStart_ = (unsigned int)start;
    p_Computer->setDebugMemoryStart(start);
    
    XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("");
    //XRCCTRL(*this, "DebugDisplayPage", HexEdit)->changeNumber(memoryStart_);
    
    for (int x=0; x<16; x++)
    {
        idReference.Printf("TOP_HEADER%01X", x);
        value.Printf("  %01X", (unsigned int)((start+x)&0xf));
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
    }
    for (int y=0; y<16; y++)
    {
        idReference.Printf("MEM_HEADER%01X", y);
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(*wxBLACK);
        
        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
        
        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            value.Printf("%02X", debugReadMem(start));
            
            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(*wxBLACK);
            
            XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue("");
            XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);
            
            start++;
            while (start > ramMask)  
                start -=  (ramMask + 1);
        }
    }
}

void DebugWindow::DebugDisplay6847CharRom()
{
	switch (runningComputer_)
	{
		case ELF:
		case ELFII:
		case SUPERELF:
			if (!elfConfiguration[runningComputer_].use6847)
			{
				XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("MC6847 not running");
				return;
			}
		break;

		default:
			if (xmlLoaded_)
				XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("MC6847 not running");
			return;
		break;
	}

	long start = get16BitValue("DebugDisplayPage");
	if (start == -1)  return;
	XRCCTRL(*this, "DebugDisplayPage", HexEdit)->saveNumber((int)start);

	wxString idReference, value;

	Word ramMask = getAddressMask();
	while (start > ramMask)  
		start -=  (ramMask + 1);

	memoryStart_ = (unsigned int)start;
	p_Computer->setDebugMemoryStart(start);

	if (start < 0x200)
		XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Showing: Internal Rom");
	else
		XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Showing: External Rom");

	//XRCCTRL(*this, "DebugDisplayPage", HexEdit)->changeNumber(memoryStart_);

	for (int x=0; x<16; x++)
	{
		idReference.Printf("TOP_HEADER%01X", x);
		value.Printf("  %01X", (unsigned int)((start+x)&0xf));
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
	}
	for (int y=0; y<16; y++)
	{
		idReference.Printf("MEM_HEADER%01X", y);
		XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(*wxBLACK);

		value.Printf("%04X", (unsigned int)start);
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

		ShowCharacters(start, y);
		for (int x=0; x<16; x++)
		{
			idReference.Printf("MEM%01X%01X", y, x);
			value.Printf("%02X", debugReadMem(start));

			XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(*wxBLACK);

			XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue("");
			XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);

			start++;
			while (start > ramMask)  
			start -=  (ramMask + 1);
		}
	}
}

void DebugWindow::DebugDisplay6847VideoRam()
{
	switch (runningComputer_)
	{
		case ELF:
		case ELFII:
		case SUPERELF:
			if (!elfConfiguration[runningComputer_].use6847)
			{
				XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("MC6847 not running");
				return;
			}
		break;

		default:
			if (xmlLoaded_)
				XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("MC6847 not running");
			return;
		break;
	}

	long start = get16BitValue("DebugDisplayPage");
	if (start == -1)  return;
	XRCCTRL(*this, "DebugDisplayPage", HexEdit)->saveNumber((int)start);

	wxString idReference, value;

	Word ramMask = getAddressMask();
	while (start >= ((ramMask+1)*2)) 
		start -=  ((ramMask+1)*2);

	memoryStart_ = (unsigned int)start;
	p_Computer->setDebugMemoryStart(start);

	if (start <= ramMask)
		XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Showing: Bit 0 to 7");
	else
		XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Showing: Bit 8 to 11");

	//XRCCTRL(*this, "DebugDisplayPage", HexEdit)->changeNumber(memoryStart_);

	for (int x=0; x<16; x++)
	{
		idReference.Printf("TOP_HEADER%01X", x);
		value.Printf("  %01X", (unsigned int)((start+x)&0xf));
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
	}
	for (int y=0; y<16; y++)
	{
		idReference.Printf("MEM_HEADER%01X", y);
		XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(*wxBLACK);

		value.Printf("%04X", (unsigned int)start);
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

		ShowCharacters(start, y);
		for (int x=0; x<16; x++)
		{
			idReference.Printf("MEM%01X%01X", y, x);

			XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(*wxBLACK);

			if (start <= ramMask)
			{
				XRCCTRL(*this, idReference, MemEdit)->saveNumber(-1);
				XRCCTRL(*this, idReference, MemEdit)->changeNumber2X(debugReadMem(start));
			}
			else
			{
				XRCCTRL(*this, idReference, MemEdit)->saveNumber(-1);
				XRCCTRL(*this, idReference, MemEdit)->changeNumber1X(debugReadMem(start));
			}

			start++;
			while (start >= ((ramMask+1)*2))  
				start -=  ((ramMask+1)*2);
		}
	}
}

void DebugWindow::DebugDisplayTmsRam()
{
	switch (runningComputer_)
	{
		case ELF:
		case ELFII:
		case SUPERELF:
			if (!elfConfiguration[runningComputer_].useTMS9918)
			{
				XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("TMS 9918 not running");
				return;
			}
		break;

		default:
			if (xmlLoaded_)
				XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("TMS 9918 not running");
			return;
		break;
	}

	long start = get16BitValue("DebugDisplayPage");
	if (start == -1)  return;
	XRCCTRL(*this, "DebugDisplayPage", HexEdit)->saveNumber((int)start);

	wxString idReference, value;

	Word ramMask = getAddressMask();
	while (start > ramMask)  
		start -=  (ramMask + 1);

	memoryStart_ = (unsigned int)start;
	p_Computer->setDebugMemoryStart(start);

	XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("");
	//XRCCTRL(*this, "DebugDisplayPage", HexEdit)->changeNumber(memoryStart_);

	for (int x=0; x<16; x++)
	{
		idReference.Printf("TOP_HEADER%01X", x);
		value.Printf("  %01X", (unsigned int)((start+x)&0xf));
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
	}
	for (int y=0; y<16; y++)
	{
		idReference.Printf("MEM_HEADER%01X", y);
		XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(*wxBLACK);

		value.Printf("%04X", (unsigned int)start);
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

		ShowCharacters(start, y);
		for (int x=0; x<16; x++)
		{
			idReference.Printf("MEM%01X%01X", y, x);

			XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(*wxBLACK);
			XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue("");

			switch(runningComputer_)
			{
				case ELF:
					value.Printf("%02X", p_Elf->getTmsMemory((int)start));
				break;

				case ELFII:
					value.Printf("%02X", p_Elf2->getTmsMemory((int)start));
				break;

				case SUPERELF:
					value.Printf("%02X", p_Super->getTmsMemory((int)start));
				break;
			}
			XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);

			start++;
			while (start > ramMask)  
			start -=  (ramMask + 1);
		}
	}
}

void DebugWindow::DebugDisplayVtRam()
{
	switch (runningComputer_)
	{
		case ELF:
		case ELFII:
		case ELF2K:
		case COSMICOS:
        case VIP:
        case VELF:
		case MEMBER:
		case SUPERELF:
			if (elfConfiguration[runningComputer_].vtType == VTNONE)
			{
				XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("VT not running");
				return;
			}
		break;

		default:
			if (xmlLoaded_)
				XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("VT not running");
			return;
		break;
	}

	long start = get16BitValue("DebugDisplayPage");
	if (start == -1)  return;
	XRCCTRL(*this, "DebugDisplayPage", HexEdit)->saveNumber((int)start);

	wxString idReference, value;

	Word ramMask = getAddressMask();
	while (start > ramMask)  
		start -=  (ramMask + 1);

	memoryStart_ = (unsigned int)start;
	p_Computer->setDebugMemoryStart(start);

	XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("");
	//XRCCTRL(*this, "DebugDisplayPage", HexEdit)->changeNumber(memoryStart_);

	for (int x=0; x<16; x++)
	{
		idReference.Printf("TOP_HEADER%01X", x);
		value.Printf("  %01X", (unsigned int)((start+x)&0xf));
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
	}
	for (int y=0; y<16; y++)
	{
		idReference.Printf("MEM_HEADER%01X", y);
		XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(*wxBLACK);

		value.Printf("%04X", (unsigned int)start);
		XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

		ShowCharacters(start, y);
		for (int x=0; x<16; x++)
		{
			idReference.Printf("MEM%01X%01X", y, x);

			value.Printf("%02X", p_Vt100->getVtMemory((int)start));

			XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(*wxBLACK);

			XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue("");
			XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);

			start++;
			while (start > ramMask)  
			start -=  (ramMask + 1);
		}
	}
}

void DebugWindow::onEditMemory(wxCommandEvent&event)
{
	long address, value, id;
	wxString strValue, idReference, buttonNumber;

	if (!computerRunning_)
	{
		(void)wxMessageBox( "Memory edit not possible as no machine is running\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return;
	}

	idReference = wxWindow::FindWindowById(event.GetId())->GetName();
	buttonNumber = idReference.Right(2);
	if (!buttonNumber.ToLong(&id, 16))
		return;

	switch (memoryDisplay_)
	{
		case CPU_TYPE:
			strValue = XRCCTRL(*this, idReference, wxTextCtrl)->GetValue();
			strValue = strValue.MakeUpper();
			if (strValue != " ")
			{
				strValue.Trim(false);
				strValue.Trim(true);
			}

			if (strValue == "R")
				setMemoryType((int)id, ROM);
			else if (strValue == ".")
				setMemoryType((int)id, RAM);
			else if (strValue == "M.")
				setMemoryType((int)id, MAPPEDRAM);
			else if (strValue == "E.")
				setMemoryType((int)id, VP570RAM);
			else if (strValue == "C.")
				setMemoryType((int)id, COLOURRAM);
			else if (strValue == " ")
				setMemoryType((int)id, UNDEFINED);
			else if (strValue == "")
				setMemoryType((int)id, UNDEFINED);
			else if (strValue == "CR")
				setMemoryType((int)id, CRAM1870);
			else if (strValue == "PR")
				setMemoryType((int)id, PRAM1870);
			else if (strValue == "M7")
				setMemoryType((int)id, MC6847RAM);
			else if (strValue == "M5")
				setMemoryType((int)id, MC6845RAM);
			else if (strValue == "MR")
				setMemoryType((int)id, MC6845REGISTERS);
			else if (strValue == "CF")
				setMemoryType((int)id, COPYFLOPROM);
			else if (strValue == "CE")
				setMemoryType((int)id, COPYCOMXEXPROM);
			else if (strValue == "P")
				setMemoryType((int)id, UNDEFINED);
			else if (strValue == "C")
				setMemoryType((int)id, UNDEFINED);
			else if (strValue == "E")
				setMemoryType((int)id, UNDEFINED);
			else if (strValue == "M")
				setMemoryType((int)id, UNDEFINED);
            else if (strValue == "TC")
                setMemoryType((int)id, TESTCARTRIDGEROM);
			else
			{
				(void)wxMessageBox( 	"Please use one of the following codes:\n"
										"space = UNDEFINED\n"
										". = RAM\n"
										"R = ROM\n"
										"M. = Mapped RAM\n"
										"E. = VP570 Expansion RAM\n"
										"PR = 1870 Page RAM\n"
                                        "CR = 1870 Character RAM or\n"
                                        "     Cartridge ROM\n"
										"M7 = MC6847 Video RAM\n"
										"M5 = MC6845 Video RAM\n"
										"MR = MC6845 Register\n"
										"CE = COMX Expansion ROM copy\n"
										"CF = COMX Floppy disk ROM copy\n"
                                        "TC = Test Cartridge ROM\n"
										"C. = Victory or Vip Colour RAM access\n"
										"\nNote: some options are only allowed\n"
										"in specific cases.\n",
											"Emma 02", wxICON_ERROR | wxOK );
				return;
			}

			XRCCTRL(*this, "DebugDisplayPage", HexEdit)->changeNumber((int)id*256);
		break;

		case CPU_MEMORY:
        case CDP_1870_C:
        case CDP_1870_COLOUR:
		case CDP_1870_P:
		case TMS_MEMORY:
		case VT_RAM:
		case CDP_1864:
		case V_6845:
        case I_8275:
        case I_8275_RAM:
		case V_6847:
		case V_6847_RAM:
			address = get16BitValue("DebugDisplayPage");
			if (address == -1)  return;

			value = get8BitValue(idReference);
			if (value == -1)  return;

			address += id;
			debugWriteMem(address, value);
			XRCCTRL(*this, idReference, MemEdit)->saveNumber((int)value);

			ShowCharacters(address-(id&0xf), (int)(id/16));
			
#if defined(__WXMSW__) || defined(__WXMAC__)
			strValue = XRCCTRL(*this, idReference, MemEdit)->GetValue();
			if (strValue.Len() == 2)
			{
				if (id == 255)
				{
					debugDisplayPageSpinUp();
					XRCCTRL(*this, "MEM00", MemEdit)->SetFocus();
					XRCCTRL(*this, "MEM00", MemEdit)->SelectAll();
				}
				else	
				{
					wxKeyEvent keyEvent(wxEVT_KEY_DOWN); 
					keyEvent.m_rawCode = WXK_TAB; 
					XRCCTRL(*this, idReference, MemEdit)->EmulateKeyPress(keyEvent);
				}
			}
#endif
		break;
	}
}

void DebugWindow::setMemoryType(int id, int setType)
{
	switch (runningComputer_)
	{
		case COMX:
			if ((setType == RAM) || (setType == ROM) || (setType == UNDEFINED) || (setType == CRAM1870) || (setType == PRAM1870)|| (setType == COMXEXPBOX) || (setType == MC6845RAM) || (setType == MC6845REGISTERS) || (setType == COPYFLOPROM) || (setType == COPYCOMXEXPROM))
			{
				if (((setType == MC6845RAM) || (setType == MC6845REGISTERS)) && !p_Comx->isColumnRomLoaded())
				{
					(void)wxMessageBox( "No 80 column card configured\n",
												"Emma 02", wxICON_ERROR | wxOK );
					return;
				}
				if (p_Computer->getMemoryType(id) == COMXEXPBOX)
				{
					if (p_Computer->getExpansionMemoryType(p_Comx->getComxExpansionSlot(), id&0x1f) == RAMBANK)
					{
						if ((setType == RAM) || (setType == ROM) || (setType == UNDEFINED))
							p_Computer->defineBankMemoryType(p_Comx->getComxExpansionRamBank(), (id&0x1f)*256, setType);
						else
						{
							(void)wxMessageBox( "Only RAM (.), ROM (R) or UNDEFINED (space) allowed in 32K RAM Card\n",
														"Emma 02", wxICON_ERROR | wxOK );
						}
					}
					else
					{
						if ((p_Computer->getExpansionMemoryType(p_Comx->getComxExpansionSlot(), id&0x1f) == EPROMBANK) || (p_Computer->getExpansionMemoryType(p_Comx->getComxExpansionSlot(), id&0x1f) == SUPERBANK))
						{
							if ((setType == RAM) || (setType == ROM) || (setType == UNDEFINED))
								p_Computer->defineEpromBankMemoryType(p_Comx->getComxExpansionEpromBank(), (id&0x1f)*256, setType);
							else
							{
								(void)wxMessageBox( "Only RAM (.), ROM (R) or UNDEFINED (space) allowed in F&M EPROM or SB Card\n",
															"Emma 02", wxICON_ERROR | wxOK );
							}
						}
						else
						{
							if ((setType == RAM) || (setType == ROM) || (setType == UNDEFINED) || (setType == MC6845RAM) || (setType == MC6845REGISTERS))
								p_Computer->defineExpansionMemoryType(p_Comx->getComxExpansionSlot(), (id&0x1f)*256, setType);
							else
							{
								(void)wxMessageBox( "Only RAM (.), ROM (R), MC6845 (MR/M5) or UNDEFINED (space) allowed in COMX Expansion Slot\n",
															"Emma 02", wxICON_ERROR | wxOK );
							}
						}
					}
				}
				else
					p_Computer->defineMemoryType(id*256, setType);
			}
			else
			{
				(void)wxMessageBox( "Only RAM (.), ROM (R), CDP1870 (PR/CR), MC6845 (MR/M5), ROM Copy (CF/CE) or UNDEFINED (space) allowed in COMX emulation\n",
											"Emma 02", wxICON_ERROR | wxOK );
			}
		break;

		case ELF:
		case ELFII:
		case SUPERELF:
			if ((setType == RAM) || (setType == ROM) || (setType == UNDEFINED) || (setType == MAPPEDRAM) || (setType == MC6847RAM) || (setType == MC6845RAM) || (setType == MC6845REGISTERS) )
			{
				if (!(elfConfiguration[runningComputer_].use6845 || elfConfiguration[runningComputer_].useS100) && ((setType == MC6845RAM) || (setType == MC6845REGISTERS)))
				{
					(void)wxMessageBox( "No MC6845 configured\n",
												"Emma 02", wxICON_ERROR | wxOK );
					return;
				}
				if (!elfConfiguration[runningComputer_].use6847 && (setType == MC6847RAM))
				{
					(void)wxMessageBox( "No MC6847 configured\n",
												"Emma 02", wxICON_ERROR | wxOK );
					return;
				}
				if (p_Computer->getMemoryType(id) == EMSMEMORY)
				{
					if ((setType == RAM) || (setType == ROM) || (setType == UNDEFINED))
						p_Computer->defineEmsMemoryType((id&0x3f)*256, setType);
					else
					{
						(void)wxMessageBox( "Only RAM (.), ROM (R), or UNDEFINED (space) allowed in EMS Memory bank\n",
													"Emma 02", wxICON_ERROR | wxOK );
					}
				}
                if (p_Computer->getMemoryType(id) == ROMMAPPER)
                {
                    if ((setType == RAM) || (setType == ROM) || (setType == UNDEFINED))
                        p_Computer->defineRomMapperMemoryType(id*256, setType);
                    else
                    {
                        (void)wxMessageBox( "Only RAM (.), ROM (R), or UNDEFINED (space) allowed in EMS Memory bank\n",
                                           "Emma 02", wxICON_ERROR | wxOK );
                    }
                }
				else if (p_Computer->getMemoryType(id) == PAGER)
				{
					if ((setType == RAM) || (setType == ROM) || (setType == UNDEFINED))
						p_Computer->definePagerMemoryType(id*256, setType);
					else
					{
						(void)wxMessageBox( "Only RAM (.), ROM (R), or UNDEFINED (space) allowed in Pager Memory\n",
													"Emma 02", wxICON_ERROR | wxOK );
					}
				}
				else
					p_Computer->defineMemoryType(id*256, setType);
			}
			else
			{
				(void)wxMessageBox( "Only RAM (.), Mapped RAM (M.), ROM (R), MC6845 (M5/MR), MC6847 (M7) or UNDEFINED (space) allowed in Elf emulation\n",
											"Emma 02", wxICON_ERROR | wxOK );
			}
		break;

		case CIDELSA:
		case TMC600:
		case PECOM:
			if ((setType == RAM) || (setType == ROM) || (setType == UNDEFINED) || (setType == CRAM1870)|| (setType == PRAM1870))
				p_Computer->defineMemoryType(id*256, setType);
			else
			{
				(void)wxMessageBox( "Only RAM (.), ROM (R), CDP1870 (PR/CR) or UNDEFINED (space) allowed in "+computerInfo[runningComputer_].name+" emulation\n",
											"Emma 02", wxICON_ERROR | wxOK );
			}
		break;

		case VIP:
			if ((setType == RAM) || (setType == ROM) || (setType == COLOURRAM) || (setType == UNDEFINED) || (setType == MAPPEDRAM) || (setType == VP570RAM))
				p_Computer->defineMemoryType(id*256, setType);
			else
			{
				(void)wxMessageBox( "Only RAM (.), ROM (R), Mapped RAM (M.), Colour RAM (C.), VP570 Expansion RAM (VP) or UNDEFINED (space) allowed in "+computerInfo[runningComputer_].name+" emulation\n",
											"Emma 02", wxICON_ERROR | wxOK );
			}
		break;

		case VIPII:
			if ((setType == RAM) || (setType == ROM) || (setType == COLOURRAM) || (setType == UNDEFINED))
				p_Computer->defineMemoryType(id*256, setType);
			else
			{
				(void)wxMessageBox( "Only RAM (.), ROM (R), Mapped RAM (M.), Colour RAM (C.), VP570 Expansion RAM (VP) or UNDEFINED (space) allowed in "+computerInfo[runningComputer_].name+" emulation\n",
											"Emma 02", wxICON_ERROR | wxOK );
			}
		break;

        case VELF:
            if ((setType == RAM) || (setType == ROM) || (setType == UNDEFINED))
                p_Computer->defineMemoryType(id*256, setType);
            else
            {
                (void)wxMessageBox( "Only RAM (.), ROM (R) or UNDEFINED (space) allowed in "+computerInfo[runningComputer_].name+" emulation\n",
                                   "Emma 02", wxICON_ERROR | wxOK );
            }
        break;
            
		case ELF2K:
		case TMC2000:
		case TMC1800:
		case NANO:
		case MEMBER:
		case MICROTUTOR:
		case ETI:
			if ((setType == RAM) || (setType == ROM) || (setType == UNDEFINED))
				p_Computer->defineMemoryType(id*256, setType);
			else
			{
				(void)wxMessageBox( "Only RAM (.), ROM (R), or UNDEFINED (space) allowed in "+computerInfo[runningComputer_].name+" emulation\n",
											"Emma 02", wxICON_ERROR | wxOK );
			}
		break;

		case COSMICOS:
        case FRED:
			if ((setType == MAPPEDRAM) || (setType == RAM) || (setType == ROM) || (setType == UNDEFINED))
				p_Computer->defineMemoryType(id*256, setType);
			else
			{
				(void)wxMessageBox( "Only RAM (.), ROM (R), Main RAM (M.) or UNDEFINED (space) allowed in "+computerInfo[runningComputer_].name+" emulation\n",
											"Emma 02", wxICON_ERROR | wxOK );
			}
		break;

        case VICTORY:
			if (setType == CRAM1870)
				setType = CARTRIDGEROM;
			if ((setType == RAM) || (setType == MAPPEDRAM) || (setType == COLOURRAM) || (setType == ROM) || (setType == CARTRIDGEROM) || (setType == UNDEFINED) || (setType == TESTCARTRIDGEROM))
				p_Computer->defineMemoryType(id*256, setType);
			else
			{
				(void)wxMessageBox( "Only RAM (.), Mapped RAM (M.), Colour RAM (C.), ROM (R), Cartridge ROM (CR), Test Cartridge ROM (TC) or UNDEFINED (space) allowed in "+computerInfo[runningComputer_].name+" emulation\n",
											"Emma 02", wxICON_ERROR | wxOK );
			}
		break;

        case STUDIOIV:
            if ((setType == RAM) || (setType == COLOURRAM) || (setType == ROM) || (setType == UNDEFINED))
                p_Computer->defineMemoryType(id*256, setType);
            else
            {
                (void)wxMessageBox( "Only RAM (.), Colour RAM (C.), ROM (R) or UNDEFINED (space) allowed in "+computerInfo[runningComputer_].name+" emulation\n",
                                   "Emma 02", wxICON_ERROR | wxOK );
            }
            break;
            
		case VISICOM:
        case STUDIO:
        case COINARCADE:
			if (setType == CRAM1870)
				setType = CARTRIDGEROM;
			if ((setType == RAM) || (setType == MAPPEDRAM) || (setType == MAPPEDROM) || (setType == ROM) || (setType == CARTRIDGEROM) || (setType == UNDEFINED))
				p_Computer->defineMemoryType(id*256, setType);
			else
			{
				(void)wxMessageBox( "Only RAM (.), Mapped RAM (M.), ROM (R), Cartridge ROM (CR) or UNDEFINED (space) allowed in "+computerInfo[runningComputer_].name+" emulation\n",
											"Emma 02", wxICON_ERROR | wxOK );
			}
		break;
	}
}


void DebugWindow::memoryDisplay()
{
//	XRCCTRL(*this, "DebugDisplayPage", wxTextCtrl)->SetForegroundColour(*wxBLACK);
	switch (memoryDisplay_)
	{
		case CPU_MEMORY:
			DebugDisplayPage();
		break;

		case CPU_TYPE:
			DebugDisplayMap();
		break;

        case CDP_1870_C:
            DebugDisplay1870VideoRam();
        break;
            
        case CDP_1870_COLOUR:
            DebugDisplay1870ColourRam();
        break;
            
		case CDP_1870_P:
			DebugDisplay1870VideoRam();
		break;

		case TMS_MEMORY:
			DebugDisplayTmsRam();
		break;

		case VT_RAM:
			DebugDisplayVtRam();
		break;

		case CDP_1864:
			DebugDisplay1864ColorRam();
		break;

        case I_8275:
            DebugDisplay8275CharRom();
            break;
            
        case I_8275_RAM:
            DebugDisplay8275VideoRam();
        break;
            
		case V_6845:
			DebugDisplay6845CharRom();
		break;

		case V_6847:
			DebugDisplay6847CharRom();
		break;

		case V_6847_RAM:
			DebugDisplay6847VideoRam();
		break;
	}
}

Word DebugWindow::getAddressMask()
{
	switch (memoryDisplay_)
	{
		case CPU_MEMORY:
			return 0xffff;
		break;

		case CPU_TYPE:
			return 0xffff;
		break;

		case CDP_1870_C:
			switch (runningComputer_)
			{
				case COMX:
					return p_Comx->getCharacterMemoryMask();
				break;
				case CIDELSA:
					return p_Cidelsa->getCharacterMemoryMask();
				break;
				case TMC600:
					return p_Tmc600->getCharacterMemoryMask();
				break;
				case PECOM:
					return p_Pecom->getCharacterMemoryMask();
				break;
				default:
					return 0x7ff;
				break;
			}
		break;

            
        case CDP_1870_COLOUR:
            switch (runningComputer_)
            {
                case CIDELSA:
                    return p_Cidelsa->getCharacterMemoryMask();
                break;
                case TMC600:
                    return 0x3ff;
                break;
                default:
                    return 0;
                break;
            }
        break;
            
		case CDP_1870_P:
			switch (runningComputer_)
			{
				case COMX:
					return p_Comx->getPageMemoryMask();
				break;
				case CIDELSA:
					return p_Cidelsa->getPageMemoryMask();
				break;
				case TMC600:
					return p_Tmc600->getPageMemoryMask();
				break;
				case PECOM:
					return p_Pecom->getPageMemoryMask();
				break;
				default:
					return 0x3ff;
				break;
			}
		break;

		case TMS_MEMORY:
			return 0x3fff;
		break;

		case CDP_1864:
			switch (runningComputer_)
			{
				case ETI:
                case VIP:
                    return 0xff;
				break;
				case VIPII:
					return 0x3ff; 
				break;
				default:
					return 0x3ff;
				break;
			}
		break;

        case I_8275:
            return 0x1fff;
        break;
            
        case I_8275_RAM:
            return 0x13ff;
        break;
            
		case VT_RAM:
		case V_6845:
		case V_6847:
			return 0x7ff;
		break;

		case V_6847_RAM:
			switch(runningComputer_)
			{
				case ELF:
					return p_Elf->get6847RamMask();
				break;

				case ELFII:
					return p_Elf2->get6847RamMask();
				break;

				case SUPERELF:
					return p_Super->get6847RamMask();
				break;

				default:
					return 0;
				break;
			}
		break;

		default:
			return 0xffff;
		break;
	}
}

void DebugWindow::onDebugMemType(wxCommandEvent&event)
{
	memoryDisplay_ = event.GetSelection();

	memoryDisplay();

	XRCCTRL(*this, "DebugDisplayPage", wxTextCtrl)->Enable(memoryDisplay_ != CPU_TYPE);
#if defined(__WXMSW__) || defined(__WXMAC__)
	XRCCTRL(*this, "DebugDisplayPageSpinButton", wxSpinButton)->Enable(memoryDisplay_ != CPU_TYPE);
#endif
	XRCCTRL(*this, "DebugSave", wxButton)->Enable(memoryDisplay_ != CPU_TYPE);
	XRCCTRL(*this, "DebugCopy", wxButton)->Enable(memoryDisplay_ != CPU_TYPE);
}

void DebugWindow::onDebugExpansionSlot(wxCommandEvent&WXUNUSED(event))
{
	wxString value = XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->GetValue();

	if (!computerRunning_) return;

	long selection;
	if (!value.ToLong(&selection))
		return;

	if (p_Comx->getComxExpansionSlot() == selection-1)  return;
	XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->saveNumber((int)selection);

	int slot = 1 << selection;
	int bank = p_Computer->getOutValue(1) & 0xe0;
	slot = bank | slot;

	p_Comx->out(1, 0, slot);
}

void DebugWindow::onDebugExpansionRam(wxCommandEvent&WXUNUSED(event))
{
	wxString value = XRCCTRL(*this, "DebugExpansionRam", SlotEdit)->GetValue();

	if (!computerRunning_) return;

	long selection;
	if (!value.ToLong(&selection))
		return;

	if (p_Comx->getComxExpansionRamBank() == selection)  return;
	XRCCTRL(*this, "DebugExpansionRam", SlotEdit)->saveNumber((int)selection);

	int bank = (int)selection << 5;
	int slot = p_Computer->getOutValue(1) & 0x1f;
	slot = bank | slot;

	p_Comx->out(1, 0, slot);
}

void DebugWindow::onDebugExpansionEprom(wxCommandEvent&WXUNUSED(event))
{
	wxString value = XRCCTRL(*this, "DebugExpansionEprom", HexEdit)->GetValue();

	if (!computerRunning_) return;

	long selection;
	if (!value.ToLong(&selection, 16))
		return;

	if (p_Comx->getComxExpansionEpromBank() == selection)  return;
	XRCCTRL(*this, "DebugExpansionEprom", HexEdit)->saveNumber((int)selection);

    int bank = (int)(((selection & 7) << 5) | ((selection & 8) >> 3));
	int slot = p_Computer->getOutValue(1) & 0x1e;
	slot = bank | slot;

	p_Comx->out(1, 0, slot);
}

void DebugWindow::onDebugEmsPage(wxCommandEvent&WXUNUSED(event))
{
	if (!elfConfiguration[runningComputer_].useEms && !elfConfiguration[runningComputer_].useRomMapper)
		return;

	wxString value = XRCCTRL(*this, "DebugEmsPage", HexEdit)->GetValue();

	if (!computerRunning_) return;

	long page;
	if (!value.ToLong(&page, 16))
		return;

	XRCCTRL(*this, "DebugEmsPage", HexEdit)->saveNumber((int)page);

    if (!p_Computer->isRomMapperDefined())
    {
        if (page > 0x1f)
            page = 0x1f;
        p_Computer->setEmsPage(page);
    }
    else
        p_Computer->setRomMapper(page);
}

void DebugWindow::onDebugPager(wxCommandEvent&WXUNUSED(event))
{
	if (!elfConfiguration[runningComputer_].usePager)
		return;

	wxString value = XRCCTRL(*this, "DebugPager", HexEdit)->GetValue();

	if (!computerRunning_) return;

	long page;
	if (!value.ToLong(&page, 16))
		return;

	XRCCTRL(*this, "DebugPager", HexEdit)->saveNumber((int)page);

//	int selectOutput = getConfigItem(computerInfo[runningComputer_].gui+"/PortExtenderSelectOutput", 5l);
	p_Computer->out(elfConfiguration[runningComputer_].elfPortConf.portExtenderSelectOutput, 0, portExtender_);

//	int writeOutput = getConfigItem(computerInfo[runningComputer_].gui+"PortExtenderWriteOutput", 6l);
	p_Computer->out(elfConfiguration[runningComputer_].elfPortConf.portExtenderWriteOutput, 0, page);
}

void DebugWindow::onDebugPortExtender(wxCommandEvent&WXUNUSED(event))
{
	if (!elfConfiguration[runningComputer_].usePager)
		return;

	wxString value = XRCCTRL(*this, "DebugPortExtender", HexEdit)->GetValue();

	if (!computerRunning_) return;

	long portExt;
	if (!value.ToLong(&portExt, 16))
		return;

	portExtender_ = portExt;

	XRCCTRL(*this, "DebugPager", HexEdit)->changeNumber(p_Computer->getPager(portExtender_));

	memoryDisplay();
}

void DebugWindow::onDebugCopy(wxCommandEvent&WXUNUSED(event))
{
	long start = get16BitValue("DebugCopyStart");
	if (start == -1)  return;

	if (start > getAddressMask())
	{
		(void)wxMessageBox( "Please specify a start value within specified memory\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return;
	}

	long end = get16BitValue("DebugCopyEnd");
	if (end == -1)  return;

	if (end > getAddressMask())
	{
		(void)wxMessageBox( "Please specify a end value within specified memory\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return;
	}

	long destination = get16BitValue("DebugCopyTo");
	if (destination == -1)  return;

	if (destination > getAddressMask())
	{
		(void)wxMessageBox( "Please specify a destination value within specified memory\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return;
	}

	while(start <= end)
	{
		debugWriteMem(destination++, debugReadMem(start++));
	}
}

void DebugWindow::onDebugCopyStart(wxCommandEvent&WXUNUSED(event))
{
	get16BitValue("DebugCopyStart");
}

void DebugWindow::onDebugCopyEnd(wxCommandEvent&WXUNUSED(event))
{
	get16BitValue("DebugCopyEnd");
}

void DebugWindow::onDebugCopyTo(wxCommandEvent&WXUNUSED(event))
{
	get16BitValue("DebugCopyTo");
}
/*
void DebugWindow::onDebugAssemblerAddress(wxCommandEvent&WXUNUSED(event))
{
	long address = get16BitValue("DebugAssemblerAddress");
	if (address == -1)  return;

	debugAddress_ =	address;
}*/
/*
void DebugWindow::onDebugDisStart(wxCommandEvent&WXUNUSED(event))
{
	get16BitValue("DebugDisStart");
}*/
/*
void DebugWindow::onDebugDisEnd(wxCommandEvent&WXUNUSED(event))
{
	get16BitValue("DebugDisEnd");
}*/


Byte DebugWindow::debugReadMem(Word address)
{
	switch (memoryDisplay_)
	{
		case CPU_MEMORY:
			return p_Computer->readMem(address);
		break;

		case CDP_1870_C:
			switch (runningComputer_)
			{
				case COMX:
					return p_Comx->readCramDirect(address);
				break;
				case CIDELSA:
					return p_Cidelsa->readCramDirect(address);
				break;
				case TMC600:
					return p_Tmc600->readCramDirect(address);
				break;
				case PECOM:
					return p_Pecom->readCramDirect(address);
				break;
				default:
					return 0;
				break;
			}
		break;

        case CDP_1870_COLOUR:
            switch (runningComputer_)
            {
                case TMC600:
                    return p_Tmc600->readColourRamDirect(address);
                break;
                case CIDELSA:
                    return p_Cidelsa->readColourRamDirect(address);
                break;
                default:
                    return 0;
                break;
            }
        break;

        case CDP_1870_P:
			switch (runningComputer_)
			{
				case COMX:
					return p_Comx->readPramDirect(address);
				break;
				case CIDELSA:
					return p_Cidelsa->readPramDirect(address);
				break;
				case TMC600:
					return p_Tmc600->readPramDirect(address);
				break;
				case PECOM:
					return p_Pecom->readPramDirect(address);
				break;
				default:
					return 0;
				break;
			}
		break;

		case TMS_MEMORY:
			switch(runningComputer_)
			{
				case ELF:
					return p_Elf->getTmsMemory(address);
				break;

				case ELFII:
					return p_Elf2->getTmsMemory(address);
				break;

				case SUPERELF:
					return p_Super->getTmsMemory(address);
				break;

				default:
					return 0;
				break;
			}
		break;

		case VT_RAM:
			if ((computerRunning_ && (p_Vt100 != NULL)))
				return p_Vt100->getVtMemory(address);
			else
				return 0;
		break;

		case CDP_1864:
			switch(runningComputer_)
			{
				case TMC600:
					return p_Tmc2000->read1864ColorDirect(address);
				break;
				case ETI:
					return p_Eti->read1864ColorDirect(address);
				break;
				case VIP:
					return p_Vip->read1864ColorDirect(address);
				break;
				case VIPII:
					return p_Vip2->read1864ColorDirect(address);
				break;
				default:
					return 0;
				break;
			}
		break;

		case I_8275:
			switch(runningComputer_)
			{
				case ELF:
					return p_Elf->read8275CharRom(address);
				break;

				case ELFII:
					return p_Elf2->read8275CharRom(address);
				break;

				case SUPERELF:
					return p_Super->read8275CharRom(address);
				break;

				case ELF2K:
					return p_Elf2K->read8275CharRom(address);
				break;

				default:
					return 0;
				break;
			}
		break;

        case I_8275_RAM:
            switch(runningComputer_)
            {
                case ELF:
                    return p_Elf->read8275VideoRam(address);
                break;
                
                case ELFII:
                    return p_Elf2->read8275VideoRam(address);
                break;
                
                case SUPERELF:
                    return p_Super->read8275VideoRam(address);
                break;
                
                case ELF2K:
                    return p_Elf2K->read8275VideoRam(address);
                break;
                
                default:
                    return 0;
                break;
        }
            break;
            
		case V_6845:
			switch(runningComputer_)
			{
				case COMX:
					return p_Comx->read6845CharRom(address);
				break;

				case ELF:
					return p_Elf->read6845CharRom(address);
				break;

				case ELFII:
					return p_Elf2->read6845CharRom(address);
				break;

				case SUPERELF:
					return p_Super->read6845CharRom(address);
				break;

				default:
					return 0;
				break;
			}
		break;

		case V_6847:
			switch(runningComputer_)
			{
				case ELF:
					return p_Elf->read6847CharRom(address);
				break;

				case ELFII:
					return p_Elf2->read6847CharRom(address);
				break;

				case SUPERELF:
					return p_Super->read6847CharRom(address);
				break;

				default:
					return 0;
				break;
			}
		break;

		case V_6847_RAM:
			switch(runningComputer_)
			{
				case ELF:
					return p_Elf->readDirect6847(address);
				break;

				case ELFII:
					return p_Elf2->readDirect6847(address);
				break;

				case SUPERELF:
					return p_Super->readDirect6847(address);
				break;

				default:
					return 0;
				break;
			}
		break;

		default:
			return 0;
		break;
	}
}

void DebugWindow::debugWriteMem(Word address, Byte value)
{
	switch (memoryDisplay_)
	{
		case CPU_MEMORY:
			p_Computer->writeMem(address, value, true);
		break;

		case CDP_1870_C:
			switch (runningComputer_)
			{
				case COMX:
					p_Comx->writeCramDirect(address, value);
				break;
				case CIDELSA:
					p_Cidelsa->writeCramDirect(address, value);
				break;
				case TMC600:
					p_Tmc600->writeCramDirect(address, value);
				break;
				case PECOM:
					p_Pecom->writeCramDirect(address, value);
				break;
			}
		break;

        case CDP_1870_COLOUR:
            switch (runningComputer_)
            {
                case TMC600:
                    p_Tmc600->writeColourRamDirect(address, value);
                break;
                case CIDELSA:
                    p_Cidelsa->writeColourRamDirect(address, value);
                break;
           }
        break;
            
		case CDP_1870_P:
			switch (runningComputer_)
			{
				case COMX:
					p_Comx->writePramDirect(address, value);
				break;
				case CIDELSA:
					p_Cidelsa->writePramDirect(address, value);
				break;
				case TMC600:
					p_Tmc600->writePramDirect(address, value);
				break;
				case PECOM:
					p_Pecom->writePramDirect(address, value);
				break;
			}
		break;

		case TMS_MEMORY:
			switch(runningComputer_)
			{
				case ELF:
					p_Elf->setTmsMemory(address, value);
				break;

				case ELFII:
					p_Elf2->setTmsMemory(address, value);
				break;

				case SUPERELF:
					p_Super->setTmsMemory(address, value);
				break;
			}
		break;

		case VT_RAM:
			if (computerRunning_ && (p_Vt100 != NULL))
				p_Vt100->setVtMemory(address, value);
		break;

		case CDP_1864:
			switch (runningComputer_)
			{
				case ETI:
					p_Eti->write1864ColorDirect(address, value);
				break;
				case TMC2000:
					p_Tmc2000->write1864ColorDirect(address, value);
				break;
				case VIP:
					p_Vip->write1864ColorDirect(address, value);
				break;
				case VIPII:
					p_Vip2->write1864ColorDirect(address, value);
				break;
			}
		break;

		case I_8275:
			switch(runningComputer_)
			{
				case ELF:
					p_Elf->write8275CharRom(address, value);
				break;

				case ELFII:
					p_Elf2->write8275CharRom(address, value);
				break;

				case SUPERELF:
					p_Super->write8275CharRom(address, value);
				break;

				case ELF2K:
					p_Elf2K->write8275CharRom(address, value);
				break;
			}
		break;

        case I_8275_RAM:
            switch(runningComputer_)
            {
                case ELF:
                    p_Elf->write8275VideoRam(address, value);
                break;
                
                case ELFII:
                    p_Elf2->write8275VideoRam(address, value);
                break;
                
                case SUPERELF:
                    p_Super->write8275VideoRam(address, value);
                break;
                
                case ELF2K:
                    p_Elf2K->write8275VideoRam(address, value);
                break;
            }
            break;
            
		case V_6845:
			switch(runningComputer_)
			{
				case COMX:
					p_Comx->write6845CharRom(address, value);
				break;

				case ELF:
					p_Elf->write6845CharRom(address, value);
				break;

				case ELFII:
					p_Elf2->write6845CharRom(address, value);
				break;

				case SUPERELF:
					p_Super->write6845CharRom(address, value);
				break;
			}
		break;

		case V_6847:
			switch(runningComputer_)
			{
				case ELF:
					p_Elf->write6847CharRom(address, value);
				break;

				case ELFII:
					p_Elf2->write6847CharRom(address, value);
				break;

				case SUPERELF:
					p_Super->write6847CharRom(address, value);
				break;
			}
		break;

		case V_6847_RAM:
			switch(runningComputer_)
			{
				case ELF:
					p_Elf->writeDirect6847(address, value);
				break;

				case ELFII:
					p_Elf2->writeDirect6847(address, value);
				break;

				case SUPERELF:
					p_Super->writeDirect6847(address, value);
				break;
			}
		break;
	}
}

void DebugWindow::setSwName(wxString swName)
{
    swName_ = swName;
    if (swName != "")
        swName_ = ", Software: " + swName_;
}

void DebugWindow::updateTitle()
{
	wxString title;

	title = swName_;
	if (debugMode_)
	{
		title = swName_ + ", Debug Mode";
		if (chip8DebugMode_)
			title = swName_ + ", Chip-8 and 1802 Debug Mode";
	}
	else
	{
		if (chip8DebugMode_)
			title = swName_ + ", Chip-8 Debug Mode";
	}

	switch (runningComputer_)
	{
		case COMX:
			if (p_Comx->getSteps()==0)
				title = title + " ** PAUSED **";
			p_Comx->SetTitle("COMX-35" + title);
			p_Comx->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case CIDELSA:
			if (p_Cidelsa->getSteps()==0)
				title = title + " ** PAUSED **";
			p_Cidelsa->SetTitle("Cidelsa" + title);
			p_Cidelsa->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case TMC600:
			if (p_Tmc600->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Tmc600->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Tmc600->SetTitle("Telmac" + title);
			p_Tmc600->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case PECOM:
			if (p_Pecom->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Pecom->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Pecom->SetTitle("Pecom" + title);
			p_Pecom->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

        case COINARCADE:
            if (p_CoinArcade->getSteps()==0)
                title = title + " ** PAUSED **";
            p_CoinArcade->SetTitle("RCA Video Coin Arcade" + title);
            p_CoinArcade->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
        break;
            
        case FRED:
            if (p_Fred->getSteps()==0)
                title = title + " ** PAUSED **";
            p_Fred->SetTitle("FRED" + title);
            p_Fred->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
        break;
            
        case STUDIO:
            if (p_Studio2->getSteps()==0)
                title = title + " ** PAUSED **";
            p_Studio2->SetTitle("Studio II" + title);
            p_Studio2->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
            break;
            
		case VISICOM:
			if (p_Visicom->getSteps()==0)
				title = title + " ** PAUSED **";
			p_Visicom->SetTitle("Visicom COM-100" + title);
			p_Visicom->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

        case VICTORY:
            if (p_Victory->getSteps()==0)
                title = title + " ** PAUSED **";
            p_Victory->SetTitle("Studio III / Victory MPT-02" + title);
            p_Victory->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
        break;
            
        case STUDIOIV:
            if (p_StudioIV->getSteps()==0)
                title = title + " ** PAUSED **";
            p_StudioIV->SetTitle("Studio IV" + title);
            p_StudioIV->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
            break;
            
		case VIP:
			if (p_Vip->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Vip->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Vip->SetTitle("Cosmac VIP" + title);
			p_Vip->updateTitle(title);
			p_Vip->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case VIPII:
			if (p_Vip2->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Vip2->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Vip2->SetTitle("Cosmac VIP II" + title);
			p_Vip2->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

        case VELF:
            if (p_Velf->getSteps()==0)
                title = title + " ** PAUSED **";
            if (p_Velf->getClear()==0)
                title = title + " ** CPU STOPPED **";
            p_Velf->SetTitle("VELF" + title);
			p_Velf->updateTitle(title);
			p_Velf->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
        break;
            
		case TMC2000:
			if (p_Tmc2000->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Tmc2000->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Tmc2000->SetTitle("Telmac 2000" + title);
			p_Tmc2000->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case TMC1800:
			if (p_Tmc1800->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Tmc1800->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Tmc1800->SetTitle("Telmac 1800" + title);
			p_Tmc1800->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case ETI:
			if (p_Eti->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Eti->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Eti->SetTitle("ETI 660" + title);
			p_Eti->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case NANO:
			if (p_Nano->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Nano->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Nano->SetTitle("Telmac Nano" + title);
			p_Nano->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case ELF2K:
			if (p_Elf2K->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Elf2K->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Elf2K->SetTitle("Elf 2000" + title);
			p_Elf2K->updateTitle(title);
			p_Elf2K->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

        case MS2000:
            if (p_Ms2000->getSteps()==0)
                title = title + " ** PAUSED **";
            if (p_Ms2000->getClear()==0)
                title = title + " ** CPU STOPPED **";
            p_Ms2000->SetTitle("MS2000" + title);
            p_Ms2000->updateTitle(title);
            p_Ms2000->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
        break;
            
		case MCDS:
			if (p_Mcds->getSteps() == 0)
				title = title + " ** PAUSED **";
			if (p_Mcds->getClear() == 0)
				title = title + " ** CPU STOPPED **";
			p_Mcds->SetTitle("MCDS" + title);
			p_Mcds->updateTitle(title);
			p_Mcds->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case COSMICOS:
			if (p_Cosmicos->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Cosmicos->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Cosmicos->SetTitle("Cosmicos" + title);
			p_Cosmicos->updateTitle(title);
			p_Cosmicos->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case MEMBER:
			if (p_Membership->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Membership->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Membership->SetTitle("Membership Card" + title);
			p_Membership->updateTitle(title);
			p_Membership->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case MICROTUTOR:
			if (p_Microtutor->getSteps() == 0)
				title = title + " ** PAUSED **";
			if (p_Microtutor->getClear() == 0)
				title = title + " ** CPU STOPPED **";
			p_Microtutor->SetTitle("Microtutor" + title);
			p_Microtutor->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case ELF:
			if (p_Elf->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Elf->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Elf->SetTitle("Elf" + title);
			p_Elf->updateTitle(title);
			p_Elf->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case ELFII:
			if (p_Elf2->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Elf2->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Elf2->SetTitle("Elf II" + title);
			p_Elf2->updateTitle(title);
			p_Elf2->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;

		case SUPERELF:
			if (p_Super->getSteps()==0)
				title = title + " ** PAUSED **";
			if (p_Super->getClear()==0)
				title = title + " ** CPU STOPPED **";
			p_Super->SetTitle("Super Elf" + title);
			p_Super->updateTitle(title);
			p_Super->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
		break;
	}
}

void DebugWindow::updateDebugMenu(bool debugMode)
{
	debugMode_ = debugMode;
	XRCCTRL(*this,"DebugMode", wxCheckBox)->SetValue(debugMode_);
	if (computerRunning_)
	{
		if (!debugMode_ && !chip8DebugMode_)
			p_Computer->setPercentageClock(1);
		else
			p_Computer->setPercentageClock(percentageClock_);
	}
	XRCCTRL(*this,"PercentageClock", wxSlider)->Enable(debugMode_);
	XRCCTRL(*this,"PercentageClockText", wxStaticText)->Enable(debugMode_);
    p_Main->eventUpdateTitle();
}

void DebugWindow::onDebugMode(wxCommandEvent&event)
{
	debugMode_ = event.IsChecked();
	updateDebugMenu(debugMode_);
}

void DebugWindow::onFxDebugMode()
{
	if (!mode_.gui)
		return;
	debugMode_ = !debugMode_;
 	updateDebugMenu(debugMode_);
}

void DebugWindow::updateChip8DebugMenu(bool debugMode)
{
	chip8DebugMode_ = debugMode;
	XRCCTRL(*this,"Chip8DebugMode", wxCheckBox)->SetValue(chip8DebugMode_);
    p_Main->eventUpdateTitle();
}

void DebugWindow::onChip8DebugMode(wxCommandEvent&event)
{
	chip8DebugMode_ = event.IsChecked();
	if (computerRunning_)
	{
		if (!debugMode_ && !chip8DebugMode_)
			p_Computer->setPercentageClock(1);
		else
			p_Computer->setPercentageClock(percentageClock_);
	}
    p_Main->eventUpdateTitle();
	enableChip8DebugGui(true);
}

void DebugWindow::onChip8PauseButton(wxCommandEvent&WXUNUSED(event))
{
	if (chip8Steps_ < 0)
	{
		chip8Steps_ = 1;
		XRCCTRL(*this, "Chip8PauseButton", wxBitmapButton)->SetBitmapLabel(pauseOnBitmap);
		XRCCTRL(*this,"Chip8PauseButton", wxBitmapButton)->Enable(false);
		updateChip8DebugMenu(true);
	}
	else
	{
		chip8Steps_ = -1;
		p_Computer->setSteps(-1);
		setChip8PauseState();
	}
}

void DebugWindow::setChip8PauseState()
{
	if (chip8Steps_ == 0)
	{
		XRCCTRL(*this, "Chip8PauseButton", wxBitmapButton)->SetBitmapLabel(pauseOnBitmap);
		XRCCTRL(*this,"Chip8PauseButton", wxBitmapButton)->Enable(true);
		XRCCTRL(*this,"Chip8StepButton", wxBitmapButton)->Enable(true);
	}
	else
	{
		if (chip8Steps_ > 0)
		{
			XRCCTRL(*this, "Chip8PauseButton", wxBitmapButton)->SetBitmapLabel(pauseOffBitmap);
			XRCCTRL(*this,"Chip8StepButton", wxBitmapButton)->Enable(true);
		}
		else
		{
			XRCCTRL(*this, "Chip8PauseButton", wxBitmapButton)->SetBitmapLabel(pauseOffBitmap);
			XRCCTRL(*this,"Chip8StepButton", wxBitmapButton)->Enable(false);
		}
	}
    p_Main->eventUpdateTitle();
}

void DebugWindow::onChip8StepButton(wxCommandEvent&WXUNUSED(event))
{
	wxString stepsStr = XRCCTRL(*this,"Chip8NumberOfSteps",wxTextCtrl)->GetValue();
	if (!stepsStr.ToLong(&chip8Steps_))
		chip8Steps_ = 1;

	chip8Steps_++;
	setChip8PauseState();
	p_Computer->setSteps(-1);
	performChip8Step_ = true;
}

void DebugWindow::onChip8Clear(wxCommandEvent& WXUNUSED(event))
{
	chip8TraceWindowPointer->Clear();
}

void DebugWindow::chip8Trace(Word address)
{	
	chip8DebugTrace(chip8Disassemble(address, true, false));
}

void DebugWindow::fredTrace(Word address)
{
    chip8DebugTrace(fel2Disassemble(address, true, false));
}

wxString DebugWindow::chip8Disassemble(Word dis_address, bool includeDetails, bool showOpcode)
{
	Byte chip8_opcode1 = p_Computer->readMem(dis_address);
	Byte chip8_opcode2 = p_Computer->readMem(dis_address + 1);
	wxString buffer, detailsBuffer, addressStr;

	buffer = "";
	detailsBuffer = "";

	//Instruction value
	int vX = chip8_opcode1&0xf;
	int vY = (chip8_opcode2>>4)&0xf;
	Word address = ((chip8_opcode1 & 0xf) << 8) + chip8_opcode2;
    if ((runningComputer_ == STUDIO || runningComputer_ == VICTORY)  && (chip8_opcode1 & 0xf0) != 0)
        address += 0x100;

    Byte nibble = chip8_opcode2&0xf;
	Byte value = chip8_opcode2;

	//Real time value
	Word addressX = p_Computer->getChip8baseVar() + vX;
	Word addressY = p_Computer->getChip8baseVar() + vY;
	Byte valueX = p_Computer->readMem(addressX);
	Byte valueY = p_Computer->readMem(addressY);
    Word valueI = p_Computer->getScratchpadRegister(CHIP8_I);
    
	//Calculation variables
	int resultValue;
	wxString tempStr1;
	long tempLong1, tempLong2;

	if (showOpcode)
		addressStr.Printf("%04X: %02X%02X  ", dis_address, chip8_opcode1, chip8_opcode2);
	else
		addressStr.Printf("%03X", dis_address&0xfff);

	switch (chip8_opcode1 & 0xf0)
	{
		case 0:
			if (p_Computer->getChip8Type() == CHIP8X && chip8_opcode1 == 0x02 && chip8_opcode2 == 0xA0)
				buffer.Printf(" CLR   BACK");
			else
				switch (chip8_opcode2)
				{
					case 0xe0:
						buffer.Printf(" CLS");
					break;

					case 0xee:
						buffer.Printf(" RETURN");
					break;

					default:
						buffer.Printf(" SYS   %03X", address);
					break;
				}
		break;

		case 0x10:
            address = address | ((address & 0x800) >> 1);
            buffer.Printf(" JP    %03X", address );
		break;

		case 0x20:
            address = address | ((address & 0x800) >> 1);
			buffer.Printf(" CALL  %03X", address );
		break;

		case 0x30:
			buffer.Printf(" SE    V%01X, %02X", vX, value);
		break;

		case 0x40:
			buffer.Printf(" SNE   V%01X, %02X", vX, value);
		break;

		case 0x50:
			switch (chip8_opcode2 & 0xf)
			{
				case 0x0:
					buffer.Printf(" SE    V%01X, V%01X", vX, vY);
				break;

				case 0x1:
					if (p_Computer->getChip8Type() == CHIP8X)
					{
						tempStr1.Printf("%01X", (valueX>>4)&0x7);   
						tempStr1.ToLong(&tempLong1, 8); 
						tempStr1.Printf("%01X", (valueY>>4)&0x7);   
						tempStr1.ToLong(&tempLong2, 8); 
						resultValue = (int)((tempLong1 + tempLong2) & 0x7) << 4;
						tempStr1.Printf("%01X", valueX&0x7);   
						tempStr1.ToLong(&tempLong1, 8); 
						tempStr1.Printf("%01X", valueY&0x7);   
						tempStr1.ToLong(&tempLong2, 8); 
						resultValue = resultValue + ((tempLong1 + tempLong2)&0x7);

						buffer.Printf(" ADD8  V%01X, V%01X", vX, vY);
						detailsBuffer.Printf("V%01X=%02X", vX, (resultValue)&0xff);
					}
					else
						buffer.Printf(" Illegal instruction");
				break;

				default:
					buffer.Printf(" Illegal instruction");
				break;
			}
		break;

		case 0x60:
            buffer.Printf(" LD    V%01X, %02X", vX, value);
		break;

		case 0x70:
			buffer.Printf(" ADD   V%01X, %02X", vX, value);
			detailsBuffer.Printf("V%01X=%02X", vX, (valueX + value)&0xff);
		break;

		case 0x80:
			switch (chip8_opcode2 & 0xf)
			{
				case 0x0:
					buffer.Printf(" LD    V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, valueY);
				break;

				case 0x1:
					buffer.Printf(" OR    V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, valueX | valueY);
				break;

				case 0x2:
					buffer.Printf(" AND   V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, valueX & valueY);
				break;

				case 0x3:
					buffer.Printf(" XOR   V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, valueX ^ valueY);
				break;

				case 0x4:
					buffer.Printf(" ADD   V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, (valueX + valueY)&0xff);
				break;

				case 0x5:
					buffer.Printf(" SUB   V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, (valueX - valueY)&0xff);
				break;

				case 0x6:
					if (vX == vY)
						buffer.Printf(" SHR   V%01X", vX);
					else
						buffer.Printf(" SHR   V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, valueY>>1);
				break;

				case 0x7:
					buffer.Printf(" SUBN  V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, (valueY - valueX)&0xff);
				break;

				case 0xE:
					if (vX == vY)
						buffer.Printf(" SHL   V%01X", vX);
					else
						buffer.Printf(" SHL   V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, (valueY<<1)&0xff);
				break;

				default:
					buffer.Printf(" Illegal instruction");
				break;
			}
		break;

		case 0x90:
			if ((chip8_opcode2 & 0xf) == 0)
				buffer.Printf(" SNE   V%01X, V%01X", vX, vY);
			else
				buffer.Printf(" Illegal instruction");
		break;

		case 0xA0:
            address = address ^ ((address & 0x800) >> 1);
            buffer.Printf(" LD    I, %03X", address );
		break;

		case 0xB0:
			if (p_Computer->getChip8Type() == CHIP8X)
			{
				if ((chip8_opcode2 & 0xf) == 0)
					buffer.Printf(" CLRL  V%01X/V%01X, V%01X", vX, (vX+1)&0xf, vY);
				else
					buffer.Printf(" CLRH  V%01X/V%01X, V%01X, %01X", vX, (vX+1)&0xf, vY, nibble);
			}
			else
			{
                address = address | ((address & 0x800) >> 1);
				buffer.Printf(" JP    V0, %03X", address);
                address = (address + p_Computer->readMem(p_Computer->getChip8baseVar()))&0xfff;
                address = address | ((address & 0x800) >> 1);
				detailsBuffer.Printf("JP %03X", address);
			}
		break;

		case 0xC0:
			if (additionalChip8Details_)
			{
				addressStr = "";
				detailsBuffer.Printf("V%01X=%02X", vX, valueX);
				additionalChip8Details_ = false;
			}
			else
			{
				buffer.Printf(" RND   V%01X, %02X", vX, value);
				if (includeDetails)
					additionalChip8Details_ = true;
			}
		break;

		case 0xD0:
			buffer.Printf(" DRW   V%01X, V%01X, %01X", vX, vY, nibble);
		break;

		case 0xE0:
			switch (chip8_opcode2)
			{
				case 0x9E:
					buffer.Printf(" SKP   V%01X", vX);
				break;

				case 0xA1:
					buffer.Printf(" SKNP  V%01X", vX);
				break;

				case 0xF2:
					if (p_Computer->getChip8Type() == CHIP8X)
						buffer.Printf(" SKP2  V%01X", vX);
					else
						buffer.Printf(" Illegal instruction");
				break;

				case 0xF5:
					if (p_Computer->getChip8Type() == CHIP8X)
						buffer.Printf(" SKNP2 V%01X", vX);
					else
						buffer.Printf(" Illegal instruction");
				break;

				default:
					buffer.Printf(" Illegal instruction");
				break;
			}
		break;

		case 0xF0:
			switch (chip8_opcode2)
			{
				case 0x0:
					if (runningComputer_ == ETI)
					{
						buffer.Printf(" LD    P, V%01X", vX);
						detailsBuffer.Printf("P=%02X", valueX);
					}
					else
						buffer.Printf(" Illegal instruction");
				break;

				case 0x07:
					if (additionalChip8Details_)
					{
						addressStr = "";
						detailsBuffer.Printf("V%01X=%02X", vX, valueX);
						additionalChip8Details_ = false;
					}
					else
					{
						buffer.Printf(" LD    V%01X, DT", vX);
						if (includeDetails)
							additionalChip8Details_ = true;
					}
				break;

				case 0x0A:
					if (additionalChip8Details_)
					{
						addressStr = "";
						detailsBuffer.Printf("V%01X=%02X", vX, valueX);
						additionalChip8Details_ = false;
					}
					else
					{
						buffer.Printf(" LD    V%01X, K", vX);
						if (includeDetails)
							additionalChip8Details_ = true;
					}
				break;

				case 0x15:
					buffer.Printf(" LD    DT, V%01X", vX);
					detailsBuffer.Printf("DT=%02X", valueX);
				break;

				case 0x18:
					buffer.Printf(" LD    ST, V%01X", vX);
					detailsBuffer.Printf("ST=%d ms", valueX*20);
				break;

				case 0x1E:
					buffer.Printf(" ADD   I, V%01X", vX);
					detailsBuffer.Printf("I=%03X", (valueI+valueX)&0xfff);
				break;

				case 0x29:
					buffer.Printf(" LD    F, V%01X", vX);
					detailsBuffer.Printf("I=%03X", chip8Sprite[valueX&0xf]);
				break;

				case 0x33:
					buffer.Printf(" LD    B, V%01X", vX);
					detailsBuffer.Printf("[%03X->%03X]='%03d'", valueI, (valueY+2)&0xfff, valueX);
				break;

				case 0x55:
					buffer.Printf(" LD    [I], V%01X", vX);
                    detailsBuffer.Printf("[%04X]=%02X", valueI, vX);
				break;

				case 0x65:
					buffer.Printf(" LD    V%01X, [I]", vX);
                    detailsBuffer.Printf("V%01X=%02X", vX, p_Computer->readMem(valueI));
				break;

				case 0xF8:
					if (p_Computer->getChip8Type() == CHIP8X)
						buffer.Printf(" OUT3  V%01X", vX);
					else
						buffer.Printf(" Illegal instruction");
				break;

				case 0xFB:
					if (p_Computer->getChip8Type() == CHIP8X)
					{
						if (additionalChip8Details_)
						{
							addressStr = "";
							detailsBuffer.Printf("V%01X=%02X", vX, p_Computer->getInValue(1));
							additionalChip8Details_ = false;
						}
						else
						{
							buffer.Printf(" INP1  V%01X", vX);
							if (includeDetails)
								additionalChip8Details_ = true;
						}
					}
					else
						buffer.Printf(" Illegal instruction");
				break;

				default:
					buffer.Printf(" Illegal instruction");
				break;
			}
		break;
	}	
	while (buffer.Len() < 21 && addressStr != "")
		buffer += " ";
	buffer = addressStr + buffer;
	if (includeDetails)
		buffer = buffer + detailsBuffer;
	return buffer;
}

void DebugWindow::defineFelCommands_(int chip8Type)
{
    chip8Type_ = chip8Type;
    
    for (int command = 0; command <16; command++)
        dissassembleCommand_[command] = 0;
    
    for (int command = 0; command < LAST_COMMAND; command++)
        assembleCommand_[command] = -1;

    switch(chip8Type)
    {
        case CHIPFEL1:
            defineFelCommand(0, SYS_MMM);
            defineFelCommand(1, CALL_MMM);
            defineFelCommand(2, LD_VX_KK);
            defineFelCommand(3, SNE_VX_KK);
            defineFelCommand(4, RND_VX_KK);
            defineFelCommand(5, ADD_VX_KK);
            defineFelCommand(6, TAPE_KK);
            defineFelCommand(7, FEL1_COMMAND_7);
            defineFelCommand(8, FEL3_COMMAND_8);
            defineFelCommand(9, DRW_VX_VY_N);
            defineFelCommand(0xa, LD_RA_MMM);
            defineFelCommand(0xb, LD_RB_MMM);
            defineFelCommand(0xc, SNE_VX_VY);
            defineFelCommand(0xd, TONE_VX_VY);
            defineFelCommand(0xe, SYS1_AA);
            defineFelCommand(0xf, JP_MMM);
        break;

        case CHIPFEL2:
            defineFelCommand(0, SYS_MMM);
            defineFelCommand(1, CALL_MMM);
            defineFelCommand(2, SNE_VX_KK);
            defineFelCommand(3, SNE_VX_M8AA);
            defineFelCommand(4, LD_VX_KK);
            defineFelCommand(5, LD_B_VX_VY);
            defineFelCommand(6, ADD_VX_VY_VZ);
            defineFelCommand(7, SUB_VX_VY_VZ);
            defineFelCommand(8, RND_VX_KK);
            defineFelCommand(9, BEEP_F_KK_N);
            defineFelCommand(0xa, DRW_VX_VY_N);
            defineFelCommand(0xb, ADD8_VX_VY_N);
            defineFelCommand(0xc, FEL2_COMMAND_C);
            defineFelCommand(0xd, LD_M8AA_VX);
            defineFelCommand(0xe, FEL2_COMMAND_E);
            defineFelCommand(0xf, JP_MMM);
        break;

        case CHIPFEL3:
            defineFelCommand(0, SYS_MMM);
            defineFelCommand(1, CALL_MMM);
            defineFelCommand(2, LD_VX_KK);
            defineFelCommand(3, SNE_VX_KK);
            defineFelCommand(4, FEL3_COMMAND_4);
            defineFelCommand(5, ADD_VX_KK);
            defineFelCommand(6, FEL3_COMMAND_6);
            defineFelCommand(7, RND_VX_KK);
            defineFelCommand(8, FEL3_COMMAND_8);
            defineFelCommand(9, DRW_VX_L_N);
            defineFelCommand(0xa, LD_RA_MMM);
            defineFelCommand(0xb, LD_RB_MMM);
            defineFelCommand(0xc, FEL3_COMMAND_C);
            defineFelCommand(0xd, BEEP_F_KK_N);
            defineFelCommand(0xe, FEL3_COMMAND_E);
            defineFelCommand(0xf, JP_MMM);
        break;
        
        case CHIPSTIV:
            defineFelCommand(0, LD_I_MMMM);
            defineFelCommand(1, LD_I_MMMM);
            defineFelCommand(2, LD_I_MMMM);
            defineFelCommand(3, LD_I_MMMM);
            defineFelCommand(4, STIV_COMMAND_4);
            defineFelCommand(5, STIV_COMMAND_5);
            defineFelCommand(6, STIV_COMMAND_6);
            defineFelCommand(7, ADD_VX_KK);
            defineFelCommand(8, JZ_VX_KK);
            defineFelCommand(9, JNZ_VX_KK);
            defineFelCommand(0xa, JE_I_VX_KK);
            defineFelCommand(0xb, JNE_I_VX_KK);
            defineFelCommand(0xc, RND_VX_KK);
            defineFelCommand(0xd, LD_27KK_VX);
            defineFelCommand(0xe, LD_VX_27KK);
            defineFelCommand(0xf, LD_VX_KK);
        break;
    }
}

void DebugWindow::defineFelCommand(int command, int type)
{
    dissassembleCommand_[command] = type;
    assembleCommand_[type] = command << 4;
}

wxString DebugWindow::fel2Disassemble(Word dis_address, bool includeDetails, bool showOpcode)
{
    Byte chip8_opcode1 = p_Computer->readMem(dis_address);
    Byte chip8_opcode2 = p_Computer->readMem(dis_address + 1);
    wxString buffer, detailsBuffer, addressStr;
    
    buffer = "";
    detailsBuffer = "";
    
    //Instruction value
    int vX = chip8_opcode1&0xf;
    int vY = (chip8_opcode2>>4)&0xf;
  
    Word address = ((chip8_opcode1 & 0xf) << 8) + chip8_opcode2;
    Word address16bit = (chip8_opcode1 << 8) + chip8_opcode2;
    
    Byte nibble = chip8_opcode2&0xf;
    Byte value = chip8_opcode2;

    if (chip8Type_ == CHIPSTIV)
    {
        vY = (chip8_opcode2)&0xf;
        nibble = (chip8_opcode2>>4)&0xf;
    }

    //Real time value
    Word addressX = p_Computer->getChip8baseVar() + vX;
    Word addressY = p_Computer->getChip8baseVar() + vY;
    Byte valueX = p_Computer->readMem(addressX);
    Byte valueY = p_Computer->readMem(addressY);
    Word valueI = (p_Computer->readMem(0x27f6)<<8)+p_Computer->readMem(0x27f7);
    
    //Calculation variables
    wxString tempStr1;
    
    if (showOpcode)
        addressStr.Printf("%04X: %02X%02X  ", dis_address, chip8_opcode1, chip8_opcode2);
    else
    {
        if (chip8Type_ == CHIPSTIV)
            addressStr.Printf("%04X", dis_address);
        else
            addressStr.Printf("%03X", dis_address&0xfff);
    }
    
    Byte command = chip8_opcode1 >> 4;
    
    if (dissassembleCommand_[command] == SYS_MMM)
        buffer.Printf(" SYS   %03X", address);
    
    if (dissassembleCommand_[command] == SYS1_AA)
        buffer.Printf(" SYS1  %02X", chip8_opcode2);
    
    if (dissassembleCommand_[command] == CALL_MMM)
        buffer.Printf(" CALL  %03X", address );

    if (dissassembleCommand_[command] == TAPE_KK)
        buffer.Printf(" TAPE  %02X", value);
    
    if (dissassembleCommand_[command] == SNE_VX_KK)
        buffer.Printf(" SNE   V%01X, %02X", vX, value);
    
    if (dissassembleCommand_[command] == SE_VX_KK)
        buffer.Printf(" SE    V%01X, %02X", vX, value);
    
    if (dissassembleCommand_[command] == SNE_VX_VY)
        buffer.Printf(" SNE   V%01X, V%01X", vX, vY);
    
    if (dissassembleCommand_[command] == TONE_VX_VY)
        buffer.Printf(" TONE  V%01X, V%01X", vX, vY);
    
    if (dissassembleCommand_[command] == SNE_VX_M8AA)
    {
        buffer.Printf(" SNE   V%01X, [8%02X]", vX, value);
        detailsBuffer.Printf("[8%02X]=%02X", value, valueX);
    }
    
    if (dissassembleCommand_[command] == LD_VX_KK)
        buffer.Printf(" LD    V%01X, %02X", vX, value);
    
    if (dissassembleCommand_[command] == LD_VX_27KK)
    {
		if (nibble == 0xE)
			buffer.Printf(" LD    V%01X, V%01X", vX, vY);
		else
			buffer.Printf(" LD    V%01X, [27%02X]", vX, value);
    }
    
    if (dissassembleCommand_[command] == LD_27KK_VX)
    {
		if (nibble == 0xE)
			buffer.Printf(" LD    V%01X, V%01X", vY, vX);
		else
			buffer.Printf(" LD    [27%02X], V%01X", value, vX);
    }
    
    if (dissassembleCommand_[command] == LD_B_VX_VY)
    {
        buffer.Printf(" LD    B, V%01X, V%01X", vY, vX);
        detailsBuffer.Printf("V%01X->V%01X='%03d'", vY, vY+2, valueX);
    }
    
    if (dissassembleCommand_[command] == LD_I_MMMM)
        buffer.Printf(" LD    I, %04X", address16bit);
    
    if (dissassembleCommand_[command] == LD_RA_MMM)
        buffer.Printf(" LD    RA, %03X", address );
    
    if (dissassembleCommand_[command] == LD_RB_MMM)
        buffer.Printf(" LD    RB, %03X", address );
    
    if (dissassembleCommand_[command] == ADD_VX_KK)
        buffer.Printf(" ADD   V%01X, %02X", vX, value);
    
    if (dissassembleCommand_[command] == ADD_VX_VY_VZ)
    {
        if (nibble == vX)
            buffer.Printf(" ADD   V%01X, V%01X", vX, vY);
        else
            buffer.Printf(" ADD   V%01X, V%01X+V%01X", nibble, vX, vY);
        detailsBuffer.Printf("V%01X=%02X", nibble, (valueX + valueY)&0xff);
    }

    if (dissassembleCommand_[command] == SUB_VX_VY_VZ)
    {
        if (nibble == vX)
            buffer.Printf(" SUB   V%01X, V%01X", vX, vY);
        else
            buffer.Printf(" SUB   V%01X, V%01X-V%01X", nibble, vX, vY);
        detailsBuffer.Printf("V%01X=%02X", nibble, (valueX - valueY)&0xff);
    }

    if (dissassembleCommand_[command] == RND_VX_KK)
        buffer.Printf(" RND   V%01X, %02X", vX, value);
    
    if (dissassembleCommand_[command] == BEEP_F_KK_N)
        buffer.Printf(" BEEP  F=%02X, %01X", value, vX);

    if (dissassembleCommand_[command] == DRW_VX_VY_N)
        buffer.Printf(" DRW   V%01X, V%01X, %01X", vX, vY, nibble);

    if (dissassembleCommand_[command] == DRW_VX_L_N)
        buffer.Printf(" DRW   V%01X, %01X, %01X", vX, vY, nibble);
    
    if (dissassembleCommand_[command] == ADD8_VX_VY_N)
    {
        buffer.Printf(" ADD8  V%01X, V%01X, V%01X", vX, vY, nibble);
        if ((valueX + valueY) & 0x8)
            detailsBuffer.Printf("V%01X=%02X", nibble, 0xFF);
        else
            detailsBuffer.Printf("V%01X=%02X", vX, valueX + valueY);
    }

    if (dissassembleCommand_[command] == LD_M8AA_VX)
    {
        buffer.Printf(" LD    [8%02X], V%01X", value, vX);
        detailsBuffer.Printf("[8%02X]=%02X", value, valueX);
    }

    if (dissassembleCommand_[command] == JP_MMM)
    {
        address = address | ((address & 0x800) >> 1);
        buffer.Printf(" JP    %03X", address );
    }
    
    if (dissassembleCommand_[command] == JNZ_VX_KK)
        buffer.Printf(" JNZ   V%01X, %02X", vX, value);
    
    if (dissassembleCommand_[command] == JZ_VX_KK)
        buffer.Printf(" JZ    V%01X, %02X", vX, value);

    if (dissassembleCommand_[command] == JNE_I_VX_KK)
        buffer.Printf(" JNE   I, V%01X, %02X", vX, value);
    
    if (dissassembleCommand_[command] == JE_I_VX_KK)
        buffer.Printf(" JE    I, V%01X, %02X", vX, value);
    
    if (dissassembleCommand_[command] == FEL1_COMMAND_7)
    {
        buffer.Printf(" Illegal instruction");
        if (chip8_opcode2 == 0x1E)
            buffer.Printf(" LD    V%01X, [RA]", vX);
        if (chip8_opcode2 == 0x21)
            buffer.Printf(" LD    V%01X, [RB]", vX);
        if (chip8_opcode2 == 0x24)
            buffer.Printf(" LD    [RA], V%01X", vX);
        if (chip8_opcode2 == 0x27)
            buffer.Printf(" LD    [RB], V%01X", vX);
        if (chip8_opcode2 == 0x2A)
            buffer.Printf(" LD    RA.0, V%01X", vX);
        if (chip8_opcode2 == 0x30)
            buffer.Printf(" LD    RA.1, V%01X", vX);
        if (chip8_opcode2 == 0x34)
            buffer.Printf(" LD    RB.0, V%01X", vX);
        if (chip8_opcode2 == 0x38)
            buffer.Printf(" LD    V%01X, RA.0", vX);
        if (chip8_opcode2 == 0x3B)
            buffer.Printf(" LD    V%01X, RA.1", vX);
        if (chip8_opcode2 == 0x3E)
            buffer.Printf(" SHL4  V%01X", vX);
        if (chip8_opcode2 == 0x41)
            buffer.Printf(" SHR4  V%01X", vX);
        if (chip8_opcode2 == 0x4B)
            buffer.Printf(" DELAY V%01X", vX);
        if (chip8_opcode2 == 0x52)
            buffer.Printf(" LD    B, [RA], V%01X", vX);
        if (chip8_opcode1 == 0x7F && chip8_opcode2 == 0x6F)
            buffer.Printf(" INCA");
    }
    
    if (dissassembleCommand_[command] == FEL2_COMMAND_C)
    {
        if (chip8_opcode1 == 0xc0)
        {
            if (showOpcode)
                addressStr.Printf("%04X: %02X    ", dis_address, chip8_opcode1);
            buffer.Printf(" RETURN");
        }
        if (chip8_opcode1&0x1)
            buffer.Printf(" TONE  F=%02X", value);
        if (chip8_opcode1&0x2)
            buffer.Printf(" TONE  OFF");
        if (chip8_opcode1&0x4)
        {
            if (showOpcode)
                addressStr.Printf("%04X: %02X    ", dis_address, chip8_opcode1);
            buffer.Printf(" NO    OP");
        }
        if (chip8_opcode1&0x8)
            buffer.Printf(" LD    V%01X, PAR", vY);
    }

    if (dissassembleCommand_[command] == FEL2_COMMAND_E)
    {
        if ((chip8_opcode2&0xf) == 0)
            buffer.Printf(" LD    V%01X, COIN", vX);
        if (chip8_opcode2&0x1)
            buffer.Printf(" LD    V%01X, FIREA", vX);
        if (chip8_opcode2&0x2)
            buffer.Printf(" LD    V%01X, FIREB", vX);
        if (chip8_opcode2&0x4)
            buffer.Printf(" LD    V%01X, JOYA", vX);
        if (chip8_opcode2&0x8)
            buffer.Printf(" LD    V%01X, JOYB", vX);
    }

    if (dissassembleCommand_[command] == FEL3_COMMAND_4)
    {
        if (chip8_opcode2&0x1)
            buffer.Printf(" LD    V%01X, V%01X", vX, vY);
        else
            buffer.Printf(" SNE   V%01X, V%01X", vX, vY);
    }

    if (dissassembleCommand_[command] == FEL3_COMMAND_6)
    {
        buffer.Printf(" Illegal instruction");
        if (chip8_opcode2 == 0x5d)
            buffer.Printf(" LD    V%01X, COIN", vX);
        if (chip8_opcode2 == 0x63)
            buffer.Printf(" LD    V%01X, FIREA", vX);
        if (chip8_opcode2 == 0x67)
            buffer.Printf(" LD    V%01X, FIREB", vX);
        if (chip8_opcode2 == 0x6b)
            buffer.Printf(" LD    V%01X, JOYA", vX);
        if (chip8_opcode2 == 0x74)
            buffer.Printf(" LD    V%01X, JOYB", vX);
        if (chip8_opcode2 == 0x7b)
            buffer.Printf(" LD    V%01X, PAR", vX);
    }
    
    if (dissassembleCommand_[command] == FEL3_COMMAND_8)
    {
        buffer.Printf(" Illegal instruction");
        if (nibble == 1)
            buffer.Printf(" OR    V%01X, V%01X", vX, vY);
        if (nibble == 2)
            buffer.Printf(" AND   V%01X, V%01X", vX, vY);
        if (nibble == 3)
            buffer.Printf(" XOR   V%01X, V%01X", vX, vY);
        if (nibble == 4)
            buffer.Printf(" ADD   V%01X, V%01X", vX, vY);
        if (nibble == 5)
            buffer.Printf(" SUB   V%01X, V%01X", vX, vY);
        if (nibble == 6)
            buffer.Printf(" SHR   V%01X, V%01X", vX, vY);
        if (nibble == 7)
            buffer.Printf(" SUBN  V%01X, V%01X", vX, vY);
    }

    if (dissassembleCommand_[command] == FEL3_COMMAND_C)
    {
        buffer.Printf(" Illegal instruction");
        if (chip8_opcode2 == 0x35)
            buffer.Printf(" LD    V%01X, [RA]", vX);
        if (chip8_opcode2 == 0x38)
            buffer.Printf(" LD    V%01X, [RB]", vX);
        if (chip8_opcode2 == 0x3B)
            buffer.Printf(" LD    [RA], V%01X", vX);
        if (chip8_opcode2 == 0x3E)
            buffer.Printf(" LD    [RB], V%01X", vX);
        if (chip8_opcode2 == 0x41)
            buffer.Printf(" LD    RB.0, V%01X", vX);
        if (chip8_opcode2 == 0x44)
            buffer.Printf(" LD    RB.1, V%01X", vX);
        if (chip8_opcode2 == 0x47)
            buffer.Printf(" LD    V%01X, RB.0", vX);
        if (chip8_opcode2 == 0x4A)
            buffer.Printf(" LD    V%01X, RB.1", vX);
        if (chip8_opcode2 == 0x4D)
            buffer.Printf(" LD    RA.0, V%01X", vX);
        if (chip8_opcode2 == 0x50)
            buffer.Printf(" INCA");
        if (chip8_opcode2 == 0x51)
            buffer.Printf(" INCB");
        if (chip8_opcode2 == 0x54)
            buffer.Printf(" DECB");
        if (chip8_opcode2 == 0x56)
            buffer.Printf(" LD    V%01X, RA.0", vX);
    }

    if (dissassembleCommand_[command] == STIV_COMMAND_4)
    {
        if (nibble == 0)
		{
            buffer.Printf(" LD    B, [V%01X], V%01X", vY, vX);
			detailsBuffer.Printf("[V%01X]->[V%01X]='%03d'", valueY+0x2700, valueY+0x2702, valueX);
		}
		if (nibble == 1)
        {
            buffer.Printf(" OR    V%01X, V%01X", vX, vY);
            detailsBuffer.Printf("V%01X=%02X", vX, valueX | valueY);
        }
        if (nibble == 2)
        {
            buffer.Printf(" AND   V%01X, V%01X", vX, vY);
            detailsBuffer.Printf("V%01X=%02X", vX, valueX & valueY);
        }
        if (nibble == 3)
        {
            buffer.Printf(" XOR   V%01X, V%01X", vX, vY);
            detailsBuffer.Printf("V%01X=%02X", vX, valueX ^ valueY);
        }
        if (nibble == 4)
        {
            buffer.Printf(" ADD   V%01X, V%01X", vX, vY);
            detailsBuffer.Printf("V%01X=%02X", vX, (valueX + valueY)&0xff);
        }
        if (nibble == 5)
        {
            buffer.Printf(" SUB   V%01X, V%01X", vX, vY);
            detailsBuffer.Printf("V%01X=%02X", vX, (valueX - valueY)&0xff);
        }
        if (nibble == 6)
        {
            buffer.Printf(" SHL   V%01X, %01X", vX, vY);
            detailsBuffer.Printf("V%01X=%02X, VB=%02X", vX, (valueX<<vY)&0xff, ((valueX<<vY)&0xff00)>>8);
        }
        if (nibble == 7)
            buffer.Printf(" KEYP   V%01X", vY);
        if (nibble == 8)
            buffer.Printf(" KEYR   V%01X", vY);
        if (nibble == 9)
        {
            buffer.Printf(" SHR   V%01X, %01X", vX, vY);
            detailsBuffer.Printf("V%01X=%02X, VB=%02X", vX, (valueX>>vY)&0xff, ((valueX &(0xff >> (8-vY)))<<(8-vY)));
        }
        if (nibble == 0xA)
        {
            buffer.Printf(" ADDN  V%01X, V%01X", vX, vY);
            detailsBuffer.Printf("V%01X=%02X", vX, ((valueX + valueY)&0xf) | ((valueX&0xf0 + valueY&0xf0)));
        }
        if (nibble == 0xB)
            buffer.Printf(" JP    I");
        if (nibble == 0xC)
        {
            buffer.Printf(" SHR   V%01X, V%01X", vX, vY);
            detailsBuffer.Printf("V%01X=%02X, V%01X=%02X", vX, (valueX>>3)&0xf, vY, (valueY>>2)&0xf);
        }
        if (nibble == 0xD)
            buffer.Printf(" STOP");
        if (nibble == 0xE)
            buffer.Printf(" DRW   I, V%01X, %01X", vX, vY);
        if (nibble == 0xF)
            buffer.Printf(" KEY   V%01X", vY);
    }
    
    if (dissassembleCommand_[command] == STIV_COMMAND_5)
    {
        if (nibble == 0)
            buffer.Printf(" SYS   I");
        if (nibble == 1)
            buffer.Printf(" SWITCH V%01X, V%01X, [I]", vX, vY);
        if (nibble == 2)
            buffer.Printf(" DRW   I, V%01X", vX);
        if (nibble == 3)
            buffer.Printf(" JE    I, V%01X, V%01X", vX, vY);
        if (nibble == 4)
            buffer.Printf(" JU    I, V%01X, V%01X", vX, vY);
        if (nibble == 5)
            buffer.Printf(" CLR   V%01X, V%01X", vX, vY);
        if (nibble == 6)
            buffer.Printf(" CLR   V%01X, %01X", vX, nibble);
        if (nibble == 7)
            buffer.Printf(" DRWR  I, V%01X", vX);
        if (nibble == 8)
            buffer.Printf(" JK    I, V%01X", vY);
        if (nibble == 9)
            buffer.Printf(" JNK   I, V%01X", vY);
        if (nibble == 0xA)
            buffer.Printf(" JG    I, V%01X, V%01X", vX, vY);
        if (nibble == 0xB)
            buffer.Printf(" JS    I, V%01X, V%01X", vX, vY);
        if (nibble == 0xC)
            buffer.Printf(" CP    V%01X, V%01X, [I]", vX, vY);
        if (nibble == 0xD)
            buffer.Printf(" CP    [I], V%01X, V%01X", vX, vY);
        if (nibble == 0xE)
            buffer.Printf(" LD    [V%01X], V%01X", vY, vX);
        if (nibble == 0xF)
            buffer.Printf(" LD    V%01X, [V%01X]", vX, vY);
    }
    
    if (dissassembleCommand_[command] == STIV_COMMAND_6)
    {
        if (chip8_opcode1 == 0x60)
            buffer.Printf(" CALL  10%02X", value);
        if (chip8_opcode1 == 0x61)
            buffer.Printf(" CALL  11%02X", value);
        if (chip8_opcode1 == 0x62)
        {
            buffer.Printf(" ADD   I, %02X", value);
            detailsBuffer.Printf("I=%04X", (valueI&0xff00)+(((valueI&0xff)+value)&0xff));
        }
        if (chip8_opcode1 == 0x63)
		{
			if (nibble == 0xE)
				buffer.Printf(" LD    I, V%01X, V%01X", vY, vY+1);
			else
				buffer.Printf(" LD    I, [27%02X]", value);
		}
        if (chip8_opcode1 == 0x64)
		{
			if (nibble == 0xE)
				buffer.Printf(" LD    V%01X, V%01X, I", vY, vY+1);
			else
				buffer.Printf(" LD    [27%02X], I", value);
		}
        if (chip8_opcode1 == 0x65)
            buffer.Printf(" JP    %02X", value);
        if (chip8_opcode1 == 0x66)
		{
            buffer.Printf(" CALL  6%02X", value);
			switch (chip8_opcode2)
			{
				case 0x0:
					buffer.Printf(" PUSH  V0-V9");
				break;

				case 0xa:
					buffer.Printf(" POP   V0-V9");
				break;

				case 0x12:
					buffer.Printf(" SCR   CLS");
				break;

				case 0x26:
					buffer.Printf(" SCR   FILL");
				break;

				case 0x2c:
					buffer.Printf(" CHAR  [I], V0, V1");
				break;
				
				case 0x2e:                
					buffer.Printf(" CHAR  [V2V3],V0,V1");
				break;
				
				case 0x48:
					buffer.Printf(" PRINT");
				break;

				case 0x56:
					buffer.Printf(" PRINT [I]");
				break;

				case 0x60:
					buffer.Printf(" PRINT D, 3");
				break;

				case 0x7c:
					buffer.Printf(" PRINT D, 2");
				break;

				case 0x82:
					buffer.Printf(" PRINT D, 1");
				break;

				case 0x88:
					buffer.Printf(" CLR");
				break;

				case 0x90:
					buffer.Printf(" CP    [I]");
				break;
			}
		}
        if (chip8_opcode1 == 0x67)
		{
            buffer.Printf(" CALL  7%02X", value);
			switch (chip8_opcode2)
			{
				case 0x0:
					buffer.Printf(" RESET RAM");
				break;

				case 0xb:
					buffer.Printf(" SCR   XOR");
				break;

				case 0x1c:
					buffer.Printf(" KEY   SWITCH");
				break;

				case 0x54:
					buffer.Printf(" ADD   [V0V1][V2V3]");
				break;

				case 0x5a:
					buffer.Printf(" SUB   [V0V1][V2V3]");
				break;
				
				case 0x2e:                
					buffer.Printf(" CHAR  [V2V3],V0,V1");
				break;
				
				case 0x8e:
					buffer.Printf(" ADD   I, V9");
				break;

				case 0x9c:
					buffer.Printf(" LD    I, [I+V9]");
				break;

				case 0x9e:
					buffer.Printf(" LD    I, [I]");
				break;

				case 0xaa:
					buffer.Printf(" KEY   WAIT");
				break;

				case 0xb6:
					buffer.Printf(" RND   [270B], V9");
				break;

				case 0xbc:
					buffer.Printf(" RND   [270B],V8,V9");
				break;
			}
		}
        if (chip8_opcode1 == 0x68)
            buffer.Printf(" CALL  8%02X", value);
        if (chip8_opcode1 == 0x69)
            buffer.Printf(" CALL  I, %02X", value);
        if (chip8_opcode1 == 0x6A)
            buffer.Printf(" PUSH  I");
        if (chip8_opcode1 == 0x6B)
            buffer.Printf(" RETURN");
        if (chip8_opcode1 == 0x6C)
            buffer.Printf(" POP   I");
        if (chip8_opcode1 == 0x6D)
            buffer.Printf(" WAIT  [I], %02X", value);
        if (chip8_opcode1 == 0x6E)
            buffer.Printf(" OUT4  V%01X", vY);
        if (chip8_opcode1 == 0x6F)
            buffer.Printf(" OUT4  %01X", value);
    }

    if (dissassembleCommand_[command] == FEL3_COMMAND_E)
        buffer.Printf(" Illegal instruction");
    
    while (buffer.Len() < 21 && addressStr != "")
        buffer += " ";
    buffer = addressStr + buffer;
    if (includeDetails)
        buffer = buffer + detailsBuffer;
    return buffer;
}

void DebugWindow::st2Trace(Word address)
{	
	chip8DebugTrace(st2Disassemble(address, true, false));
}

wxString DebugWindow::st2Disassemble(Word dis_address, bool includeDetails, bool showOpcode)
{	
	Byte chip8_opcode1 = p_Computer->readMem(dis_address);
	Byte chip8_opcode2 = p_Computer->readMem(dis_address + 1);
	wxString buffer, detailsBuffer, addressStr;

	buffer = "";
	detailsBuffer = "";

	//Instruction value
	int vX = chip8_opcode1&0xf;
	int vY = (chip8_opcode2>>4)&0xf;
	Word address = ((chip8_opcode1 & 0xf) << 8) + chip8_opcode2;
//	Byte nibble = chip8_opcode2&0xf;
	Byte value = chip8_opcode2;

	//Real time value
	Word addressX = p_Computer->getChip8baseVar() + vX;
	Word addressY = p_Computer->getChip8baseVar() + vY;
	Byte valueX = p_Computer->readMem(addressX);
	Byte valueY = p_Computer->readMem(addressY);
	Word valueI = p_Computer->getScratchpadRegister(CHIP8_I);

	//Calculation variables
//	int resultValue;
	wxString tempStr1;
//	long tempLong1, tempLong2;

	if (showOpcode)
		addressStr.Printf("%04X: %02X%02X  ", dis_address, chip8_opcode1, chip8_opcode2);
	else
		addressStr.Printf("%03X", dis_address&0xfff);

	switch (chip8_opcode1 & 0xf0)
	{
		case 0:
			switch (chip8_opcode2)
			{
				case 0x66:
					buffer.Printf(" VIDEO ON");
				break;

				default:
					if (chip8_opcode1 == 0x2 && chip8_opcode2 == 0xF2)
					{
						buffer.Printf(" LD    [>I], 0");
						detailsBuffer.Printf("[%03X->%03X]=0", valueI&0xf00, valueI);
					}
					else
					{
						buffer.Printf(" SYS   %03X", address );
					}
				break;
			}
		break;

		case 0x10:
			buffer.Printf(" JP    %03X", address );
		break;

		case 0x20:
			buffer.Printf(" CALL  %03X", address );
		break;

		case 0x30:
			buffer.Printf(" JNZ   V%01X, %02X", vX, value);
		break;

		case 0x40:
			buffer.Printf(" JZ    V%01X, %02X", vX, value);
		break;

		case 0x50:
			buffer.Printf(" SNE   V%01X, %02X", vX, value);
		break;

		case 0x60:
			buffer.Printf(" LD    V%01X, %02X", vX, value);
		break;

		case 0x70:
			if (chip8_opcode1 == 0x70)
				buffer.Printf(" DJNZ  V0, %02X", value);
			else
			{
				buffer.Printf(" ADD   V%01X, %02X", vX, value);
				detailsBuffer.Printf("V%01X=%02X", vX, (valueX + value)&0xff);
			}
		break;

		case 0x80:
			switch (chip8_opcode2 & 0xf)
			{
				case 0x0:
					buffer.Printf(" LD    V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, valueY);
				break;

				case 0x1:
					buffer.Printf(" OR    V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, valueX | valueY);
				break;

				case 0x2:
					buffer.Printf(" AND   V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, valueX & valueY);
				break;

				case 0x3:
					buffer.Printf(" XOR   V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, valueX ^ valueY);
				break;

				case 0x4:
					buffer.Printf(" ADD   V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, (valueX + valueY)&0xff);
				break;

				case 0x5:
					buffer.Printf(" SUB   V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, (valueX - valueY)&0xff);
				break;

				case 0x6:
					if (vX == vY)
						buffer.Printf(" SHR   V%01X", vX);
					else
						buffer.Printf(" SHR   V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, valueY>>1);
				break;

				case 0x7:
					buffer.Printf(" SUBN  V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, (valueY - valueX)&0xff);
				break;

				case 0xE:
					if (vX == vY)
						buffer.Printf(" SHL   V%01X", vX);
					else
						buffer.Printf(" SHL   V%01X, V%01X", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, (valueY<<1)&0xff);
				break;

				default:
					buffer.Printf(" Illegal instruction");
				break;
			}
		break;

		case 0x90:
			switch (chip8_opcode2 & 0xf)
			{
				case 0x0:
					buffer.Printf(" SNE   V%01X, V%01X", vX, vY);
				break;

				case 0x1:
					buffer.Printf(" LD    V%01X, V%01X", vY, vX);
					detailsBuffer.Printf("V%01X=%02X", vY, valueX);
				break;

				case 0x2:
					buffer.Printf(" LD    V%01X, [V%01X]", vX, vY);
					detailsBuffer.Printf("V%01X=%02X", vX, p_Computer->readMem(0x800+valueY));
				break;

				case 0x4:
					buffer.Printf(" LD    [V%01X], V%01X", vY, vX);
					detailsBuffer.Printf("[8%02X]=%02X", valueY, valueX);
				break;

				case 0x8:
					buffer.Printf(" LD    B, [V%01X], V%01X", vY, vX);
					detailsBuffer.Printf("[%03X->%03X]='%03d'", (valueY+0x800)&0xfff, (valueY+0x802)&0xfff, valueX);
				break;

				default:
					buffer.Printf(" Illegal instruction");
				break;
			}
		break;

		case 0xA0:
			buffer.Printf(" LD    I, %03X", address);
		break;

		case 0xB0:
			buffer.Printf(" LD    [I], %02X, +%01X", value, vX);
			detailsBuffer.Printf("[%03X]=%02X, I=%03X", valueI, value, (valueI+vX)&0xfff);
		break;

		case 0xC0:
			if (chip8_opcode1 == 0xc0)
			{
				if (showOpcode)
					addressStr.Printf("%04X: %02X    ", dis_address, chip8_opcode1);
				buffer.Printf(" RETURN");
			}
			else
			{
				if (additionalChip8Details_)
				{
					addressStr = "";
					detailsBuffer.Printf("V%01X=%02X", vX, valueX);
					additionalChip8Details_ = false;
				}
				else
				{
					buffer.Printf(" RND   V%01X, %02X", vX, value);
					if (includeDetails)
						additionalChip8Details_ = true;
				}
			}
		break;

		case 0xD0:
			if (chip8_opcode1 == 0xDF)
				buffer.Printf(" JKP   VB, %02X", value);
			else
				buffer.Printf(" JKP   %01X, %02X", vX, value);
		break;

		case 0xE0:
			if (showOpcode && (chip8_opcode1 != 0xe8))
				addressStr.Printf("%04X: %02X    ", dis_address, chip8_opcode1);
			switch (chip8_opcode1)
			{
				case 0xE0:
					buffer.Printf(" SP    CLEAR");
				break;

				case 0xE1:
					buffer.Printf(" SP    MOVE");
				break;

				case 0xE2:
					buffer.Printf(" SP    VC, MOVE");
				break;

				case 0xE4:
					buffer.Printf(" SP    XOR, [I]");
				break;

				case 0xE8:
					buffer.Printf(" SP    DRW, JC, %02X", value);
				break;

				default:
					buffer.Printf(" Illegal instruction");
				break;
			}
		break;

		case 0xF0:
			if (vX == 0)
			{
				buffer.Printf(" SYS   2%02X", chip8_opcode2);
			}
			else
			{
				switch (chip8_opcode2)
				{
					case 0xa6:
						buffer.Printf(" LDM   V%01X, [I]", vX);
						detailsBuffer.Printf("V%01X=%02X", vX, p_Computer->readMem(valueI));
					break;

					case 0xa9:
						buffer.Printf(" LDM   [I], V%01X", vX);
						detailsBuffer.Printf("[%03X]=%02X", valueI, valueX);
					break;

					case 0xac:
						buffer.Printf(" LDMA   V%01X, [I]", vX);
						detailsBuffer.Printf("V%01X=%02X, I=%03X", vX, p_Computer->readMem(valueI), (valueI+1)&0xfff);
					break;

					case 0xaf:
						buffer.Printf(" LDMA  [I], V%01X", vX);
						detailsBuffer.Printf("[%03X]=%02X, I=%03X", valueI, value, (valueI+1)&0xff);
					break;

					case 0xb3:
						buffer.Printf(" SLSB  V%01X", vX);
						detailsBuffer.Printf("I=%03X", (valueI&0xf00) | valueX);
					break;

					case 0xb6:
						buffer.Printf(" AFOR  V%01X", vX);
						detailsBuffer.Printf("V%01X=%02X", vX, (valueX&0xf) | (valueI&0xff));
					break;

					default:
						buffer.Printf(" Illegal instruction");
					break;
				}
			}
		break;
	}	
	while (buffer.Len() < 21 && addressStr != "")
		buffer += " ";
	buffer = addressStr + buffer;
	if (includeDetails)
		buffer = buffer + detailsBuffer;
	return buffer;
}

void DebugWindow::onChip8Trace(wxCommandEvent& WXUNUSED(event))
{
	additionalChip8Details_ = false;
	chip8Trace_ = !chip8Trace_;
	SetChip8DebugMode();
	enableChip8DebugGui(true);
}

void DebugWindow::onChip8ProtectedMode(wxCommandEvent&event)
{
	chip8ProtectedMode_= event.IsChecked();
	enableChip8DebugGui(true);
}

void DebugWindow::onChip8Log(wxCommandEvent& WXUNUSED(event))
{
//	wxSetWorkingDirectory (workingDir_);
	int num = 0;
	wxString fileName, number;

	fileName = wxFileSelector( "Select the log file to save",
                               debugDir_, "chip8_trace.log",
                               "log",
                               wxString::Format
                              (
                                   "Dump File (*.log)|*.log|All files (%s)|%s",
                                   wxFileSelectorDefaultWildcardStr,
                                   wxFileSelectorDefaultWildcardStr
                               ),
                               wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                               this
                              );

	if (!fileName || fileName.empty())
		return;

	wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
	wxString name = FullPath.GetName();
	wxString path = FullPath.GetPath();
	wxString ext = FullPath.GetExt();

	while(wxFile::Exists(fileName))
	{
		num++;
		number.Printf("%d", num);
		fileName = path + pathSeparator_ + name + "." + number + "." + ext;
	}
	chip8TraceWindowPointer->SaveFile(fileName);
}

void DebugWindow::deleteChip8BreakPoint(wxListEvent&event)
{
	long selectedItem = event.GetIndex();

	if (selectedItem > -1)
	{
		for (long i=selectedItem; i<numberOfChip8BreakPoints_-1; i++)
			chip8BreakPoints_[i] = chip8BreakPoints_[i+1];
		numberOfChip8BreakPoints_--;
		SetChip8DebugMode();
	}
}

void DebugWindow::editChip8BreakPoint(wxListEvent&event)
{
	wxString bpStr = event.GetText();
	long selectedItem = event.GetIndex();

	if (bpStr == "")
	{
		chip8BreakPointWindowPointer->DeleteItem(selectedItem);
		event.Veto();
		return;
	}

	wxString strValue = extractWord (&bpStr);
	long value;

	if (!strValue.ToLong(&value, 16))
	{
		(void)wxMessageBox( "Please specify value in hexadecimal\n",
									"Emma 02", wxICON_ERROR | wxOK );
		event.Veto();
		return;
	}

	if (value > 0xfff)
	{
		(void)wxMessageBox( "Please specify value of 12 bit max\n",
									"Emma 02", wxICON_ERROR | wxOK );
		event.Veto();
		return;
	}

	chip8BreakPoints_[selectedItem] = value;
}

void DebugWindow::addChip8BreakPoint()
{
	wxString printBuffer;

	printBuffer.Printf("%03X", chip8BreakPoints_[numberOfChip8BreakPoints_]);
	chip8BreakPointWindowPointer->InsertItem(numberOfChip8BreakPoints_, printBuffer);
	numberOfChip8BreakPoints_++;
}

void DebugWindow::onChip8BreakPointAddress(wxCommandEvent&WXUNUSED(event))
{
	get12BitValue("Chip8BreakPointAddress");
}

void DebugWindow::onChip8BreakPointSet(wxCommandEvent&WXUNUSED(event))
{
	if (numberOfChip8BreakPoints_ == 64)
	{
		(void)wxMessageBox( "Maximum number of Break Points set\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return;
	}

	long chip8BreakPointAddress = get12BitValue("Chip8BreakPointAddress");
	if (chip8BreakPointAddress == -1)
	{
		(void)wxMessageBox( "No Break Point value specified\n",
									"Emma 02", wxICON_ERROR | wxOK );
		return;
	}

	chip8BreakPoints_[numberOfChip8BreakPoints_] = chip8BreakPointAddress;
	chip8BreakPointsSelected_[numberOfChip8BreakPoints_] = true;
	addChip8BreakPoint();
	SetChip8DebugMode();
}

void DebugWindow::onPercentageClock(wxScrollEvent&event)
{
	double percentage = event.GetPosition();
	XRCCTRL(*this,"Chip8PercentageClock", wxSlider)->SetValue((int)percentage);

	if (percentage < 100)
		percentage /= 50;

	percentage = pow (percentage, 2);
	percentageClock_ = (double)percentage / pow ((double)100,2);

	if (computerRunning_)
		p_Computer->setPercentageClock(percentageClock_);
}

void DebugWindow::onChip8PercentageClock(wxScrollEvent&event)
{
	double percentage = event.GetPosition();
	XRCCTRL(*this,"PercentageClock", wxSlider)->SetValue((int)percentage);

	if (percentage < 100)
		percentage /= 5;

	percentage = pow (percentage, 2);
	percentageClock_ = (double)percentage / pow ((double)100,2);

	if (computerRunning_)
		p_Computer->setPercentageClock(percentageClock_);
}


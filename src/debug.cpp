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
#include "wx/numformatter.h"

#include "main.h"
#include "debug.h"
#include "computerconfig.h"

#define CHIP8_I 10
#define CHIP8_PC 5
#define CARDTRAN_PC 0xf
#if defined (__linux__)
#define EDIT_ROW 16
//#define LINE_SPACE 13
#define EDIT_LINE 221
#define ASS_WIDTH 268
#define PROFILER_WIDTH 460
#define PROFILER_OFFSET 7
//#define CHAR_WIDTH 8
#endif
#if defined (__WXMSW__)
#define EDIT_ROW 16
//#define LINE_SPACE 11
#define EDIT_LINE 187
#define ASS_WIDTH 268
#define PROFILER_WIDTH 468
#define PROFILER_OFFSET 9
//#define CHAR_WIDTH 8
#endif
#if defined (__WXMAC__)
#define EDIT_ROW 15
//#define LINE_SPACE 11
#define EDIT_LINE 176
#define ASS_WIDTH 268
#define PROFILER_WIDTH 460
#define PROFILER_OFFSET 5
//#define CHAR_WIDTH 8
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
    CHIP8_VX_MEM_1,     //8
    AM_REG_MEM,
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
    ERROR_SP,
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
    ERROR_MINUS,
    ERROR_AMP,
    ERROR_PLUS,
    ERROR_EQUAL,
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
    ERROR_CPU_SYSTEM00,
    ERROR_CPU_ONLY_SYSTEM00,
    ERROR_STUDIO_CHIP_ADDRESS,
    ERROR_FEL2_CHIP_ADDRESS,
    ERROR_FPL_CHIP_ADDRESS,
    ERROR_RAM_CHIP_ADDRESS,
    ERROR_STUDIO_CHIP_ADDRESS_I,
    ERROR_STUDIOIV_ADDRESS_I,
    ERROR_CARDTRAN_ADDRESS,
    ERROR_CARDTRAN_DR,
    ERROR_COMX_NOT_RUNNING,
    WARNING_MISSING_SLOT_ADDRESS,
    ERROR_HEX,
    ERROR_HEX_MEM,
    ERROR_HEX_TO_HIGH,
    ERROR_NO_COMPUTER_RUNNING,
    ERROR_INCORR_ADDRESS,
    ERROR_MISSING_PAR,
    ERROR_INCORRECT_REG,
    ERROR_SYNTAX_FILE,
    ERROR_TEMP_PAR,
    ERROR_TEMP_CPU_1801,
    ERROR_COMX_SB_NOT_RUNNING,
    ERROR_LAST,
};

wxString DirAssErrorCodes[] =
{
    "4 bit value expected",
    "8 bit value expected",
    "16 bit value expected",
    "12 bit value expected",
    "sprite number 0 to 7 expected",
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
    "Minus expected after parameter",
    "Ampersand expected after parameter",
    "Plus expected after parameter",
    "Equal sign expected after parameter",
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
    "Not supported on CDP1801 or SYSTEM 00",
    "Not supported on SYSTEM 00",
    "Only supported on SYSTEM 00",
    "Specify address > 2FC and <= FFF",
    "Specify address > 1FF and <= FFF",
    "Specify address >= 600 and <= 6FF",
    "Specify address >= 800 and <= 8FF",
    "Specify address >= 100 and <= FFF",
    "Specify address >= 2700 and <= 27FF",
    "Specify address >= 200 and <= 2C6",
    "Parameter DR missing",
    "COMX-35 not running",
    "Warning: no slot specified",
    "Hexadecimal value expected",
    "Hexadecimal address expected",
    "Hexadecimal value to high",
    "No computer running",
    "Incorrect address value",
    "Missing parameter",
    "Incorrect register value",
    "Error in syntax file",
    "Too many parameters",
    "Not supported on CDP1801 or SYSTEM 00",
    "COMX SuperBoard not running",
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
    0xb0, 0xa0, 0xc0, // P
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
    0x30, 0x30, 0xc0, // P
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
    0, 0, 0, // P
    0, 0, 0, 0, 0, MEM_TYPE_OPCODE_RSHL, MEM_TYPE_OPCODE_RSHR, 0, // R
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MEM_TYPE_OPCODE_SKP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // S
    0, 0, 0, 0 // X
};

int minCpuType[] =
{
    CPU1802, CPU1802, SYSTEM00, CPU1801, SYSTEM00, CPU1801, // A
    SYSTEM00, SYSTEM00, SYSTEM00, SYSTEM00, CPU1804, SYSTEM00, SYSTEM00, CPU1801, CPU1801, CPU1801, CPU1801, CPU1801, CPU1801, CPU1801, CPU1802, SYSTEM00, SYSTEM00, CPU1802, SYSTEM00, CPU1804, SYSTEM00, // B
    CPU1804, CPU1804, // C
    CPU1805, CPU1805, CPU1805, CPU1805, CPU1805, SYSTEM00, CPU1801, CPU1805, CPU1805, CPU1805, CPU1805, CPU1805, CPU1804, // D
    CPU1804,
    CPU1804, SYSTEM00, SYSTEM00, // G
    SYSTEM00, SYSTEM00, SYSTEM00, SYSTEM00, // I
    CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, SYSTEM00, CPU1804, CPU1801, SYSTEM00, SYSTEM00, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, // L
    CPU1802, // M
    CPU1801, CPU1802, CPU1802, // N
    SYSTEM00, CPU1801, SYSTEM00, // O
    SYSTEM00, SYSTEM00, SYSTEM00, // P
    CPU1802, SYSTEM00, CPU1804, CPU1804, CPU1804, CPU1802, CPU1802, CPU1804, // R
    SYSTEM00, CPU1804, CPU1804, CPU1804, SYSTEM00, CPU1802, CPU1802, CPU1801, SYSTEM00, CPU1802, SYSTEM00, CPU1802, CPU1802, SYSTEM00, CPU1802, CPU1801, SYSTEM00, CPU1802, CPU1802, CPU1801, CPU1804, CPU1804, CPU1804, CPU1804, CPU1804, SYSTEM00, CPU1802, // S
    CPU1804, CPU1804, SYSTEM00, CPU1801 // X
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
    1, 1, 1, // P
    1, 1, 4, 2, 2, 1, 1, 2,// R
    1, 4, 2, 2, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, // S
    2, 2, 1, 2
};

int numberOfBytesSystem00[] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 1x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 2x
    2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0,  // 3x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 4x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 5x
    0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,  // 6x
    1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,  // 7x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 8x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 9x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Ax
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Bx
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Cx
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Dx
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Ex
    1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Fx
};

int numberOfBytes1801[] =
{
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 1x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 2x
    2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2,  // 3x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 4x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 5x
    0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,  // 6x
    1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,  // 7x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 8x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 9x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Ax
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Bx
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 7x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Dx
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Ex
    1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,  // Fx
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
    1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,  // Fx
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
    true, true, true,
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

int locationCorrection[]=
{
#if defined(__WXMAC__)
    0, 0, 0,
    -1, -1, -1, -1,
    -2, -2, -2, -2,
    -3, -3, -3, -3,
    -4, -4, -4, -4,
    -5, -5, -5, -5,
    -5, -5, -6, -6,
#else
    -6, -6, -6,
    -6, -6, -6, -6,
    -6, -6, -6, -6,
    -6, -6, -6, -6,
    -6, -6, -6, -6,
    -6, -6, -6, -6,
    -6, -6, -6, -6,
#endif
};

BEGIN_EVENT_TABLE(DebugWindow, GuiComx)

    EVT_CHECKBOX(XRCID("DebugMode"), Main::onDebugMode)

    EVT_BUTTON(XRCID("TraceLog"), DebugWindow::onLog)
    EVT_TOGGLEBUTTON(XRCID("TraceButton"), DebugWindow::onTrace)
    EVT_TOGGLEBUTTON(XRCID("DmaButton"), DebugWindow::onTraceDma)
    EVT_TOGGLEBUTTON(XRCID("IntButton"), DebugWindow::onTraceInt)
    EVT_TOGGLEBUTTON(XRCID("Chip8IntButton"), DebugWindow::onChip8TraceInt)
    EVT_TOGGLEBUTTON(XRCID("TraceTrapButton"), DebugWindow::onTraceTrap)

    EVT_BUTTON(XRCID("DebugInterrupt"), DebugWindow::onInt)
    EVT_BUTTON(XRCID("ClearButton"), DebugWindow::onClear)
    EVT_BUTTON(XRCID("DebugReset"), DebugWindow::onReset)

    EVT_BUTTON(XRCID("DebugPauseButton"), DebugWindow::onDegubPauseButton)
    EVT_BUTTON(XRCID("DebugStepButton"), DebugWindow::onDegugStepButton)
    EVT_BUTTON(XRCID("DebugRunButton"), DebugWindow::onDebugRunButton)
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
    EVT_TEXT_ENTER(XRCID("B"), DebugWindow::B)
    EVT_TEXT_ENTER(XRCID("CH"), DebugWindow::CH)
    EVT_TEXT_ENTER(XRCID("CNTR"), DebugWindow::CNTR)
    EVT_TEXT_ENTER(XRCID("DF"), DebugWindow::DF)
    EVT_TEXT_ENTER(XRCID("Q"), DebugWindow::Q)
    EVT_TEXT_ENTER(XRCID("IE"), DebugWindow::IE)
    EVT_TEXT_ENTER(XRCID("CIE"), DebugWindow::CIE)
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
    EVT_TEXT(XRCID("ProfilerAddress"), DebugWindow::onProfilerAddress)
//    EVT_SPIN_UP(XRCID("AssSpin"), DebugWindow::onAssSpinUp)
//    EVT_SPIN_DOWN(XRCID("AssSpin"), DebugWindow::onAssSpinDown)
//    EVT_SPIN_UP(XRCID("AssSpinPage"), DebugWindow::onAssSpinPageUp)
//    EVT_SPIN_DOWN(XRCID("AssSpinPage"), DebugWindow::onAssSpinPageDown)
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
//    EVT_BUTTON(XRCID("AssSave"), DebugWindow::onAssSave)
    EVT_BUTTON(XRCID("AssLoadAll"), DebugWindow::onAssLoadAll)
    EVT_BUTTON(XRCID("AssSaveAll"), DebugWindow::onAssSaveAll)
    EVT_BUTTON(XRCID("AssNew"), DebugWindow::onAssNew)
    EVT_BUTTON(XRCID("AssStore"), DebugWindow::onAssStore)
    EVT_BUTTON(XRCID("AssDir"), DebugWindow::onAssDir)
    EVT_BUTTON(XRCID("AssSaveSb"), DebugWindow::onAssSaveSb)
    EVT_SPIN_UP(XRCID("AssRangeSpin"), DebugWindow::onAssRangeSpinUp)
    EVT_SPIN_DOWN(XRCID("AssRangeSpin"), DebugWindow::onAssRangeSpinDown)
    EVT_CHOICE(XRCID("AssDataView"), DebugWindow::onAssDataView)
    EVT_CHOICE(XRCID("ProfilerDataView"), DebugWindow::onProfilerDataView)
    EVT_BUTTON(XRCID("AssCopy"), DebugWindow::onAssCopy)
    EVT_BUTTON(XRCID("AssDis"), DebugWindow::onAssDis)
    EVT_TEXT(XRCID("AssFileName"), DebugWindow::onAssTextChange)
    EVT_TEXT(XRCID("AssProgramStart"), DebugWindow::onAssTextChange)
    EVT_TEXT(XRCID("AssCodeEnd"), DebugWindow::onAssTextChange)
    EVT_TEXT(XRCID("AssProgramEnd"), DebugWindow::onAssTextChange)
    EVT_TEXT(XRCID("AssProgramSlot"), DebugWindow::onAssTextChange)

    EVT_COMMAND_SCROLL_PAGEDOWN(XRCID("ProfilerScrollbar"), DebugWindow::onProfilerSpinPageDown)
    EVT_COMMAND_SCROLL_PAGEUP(XRCID("ProfilerScrollbar"), DebugWindow::onProfilerSpinPageUp)
    EVT_COMMAND_SCROLL_LINEDOWN(XRCID("ProfilerScrollbar"), DebugWindow::onAssSpinDown)
    EVT_COMMAND_SCROLL_LINEUP(XRCID("ProfilerScrollbar"), DebugWindow::onAssSpinUp)
    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("ProfilerScrollbar"), DebugWindow::onProfilerThumbTrack)

    EVT_COMMAND_SCROLL_PAGEDOWN(XRCID("AssScrollbar"), DebugWindow::onAssSpinPageDown)
    EVT_COMMAND_SCROLL_PAGEUP(XRCID("AssScrollbar"), DebugWindow::onAssSpinPageUp)
    EVT_COMMAND_SCROLL_LINEDOWN(XRCID("AssScrollbar"), DebugWindow::onAssSpinDown)
    EVT_COMMAND_SCROLL_LINEUP(XRCID("AssScrollbar"), DebugWindow::onAssSpinUp)
    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("AssScrollbar"), DebugWindow::onAssThumbTrack)

    EVT_CHOICE(XRCID("LapTimeTrigger"), DebugWindow::onLaptimeTrigger)

    EVT_TEXT(XRCID("DebugDisplayPage"), DebugWindow::onDebugDisplayPage)
//#ifdef __WXMAC__
//    EVT_SPIN_UP(XRCID("DebugDisplayPageSpinButton"), DebugWindow::onDebugDisplayPageSpinDown)
//    EVT_SPIN_DOWN(XRCID("DebugDisplayPageSpinButton"), DebugWindow::onDebugDisplayPageSpinUp)
//#else
    EVT_SPIN_UP(XRCID("DebugDisplayPageSpinButton"), DebugWindow::onDebugDisplayPageSpinUp)
    EVT_SPIN_DOWN(XRCID("DebugDisplayPageSpinButton"), DebugWindow::onDebugDisplayPageSpinDown)
//#endif
    EVT_CHOICE(XRCID("DebugMemType"), DebugWindow::onDebugMemType)
    EVT_TEXT(XRCID("DebugExpansionSlot"), DebugWindow::onDebugExpansionSlot)
    EVT_TEXT(XRCID("DebugExpansionBank"), DebugWindow::onDebugExpansionRam)
    EVT_TEXT(XRCID("DebugEmsNumber"), DebugWindow::onDebugEmsNumber)
    EVT_TEXT(XRCID("DebugEmsPage"), DebugWindow::onDebugEmsPage)
    EVT_TEXT(XRCID("DebugPager"), DebugWindow::onDebugPager)
    EVT_TEXT(XRCID("DebugPortExtender"), DebugWindow::onDebugPortExtender)
    EVT_BUTTON(XRCID("DebugSave"), DebugWindow::onDebugSaveDump)
    EVT_BUTTON(XRCID("DebugCopy"), DebugWindow::onDebugCopy)
    EVT_TEXT(XRCID("DebugCopyStart"), DebugWindow::onDebugCopyStart)
    EVT_TEXT(XRCID("DebugCopyEnd"), DebugWindow::onDebugCopyEnd)
    EVT_TEXT(XRCID("DebugCopyTo"), DebugWindow::onDebugCopyTo)

    EVT_CHECKBOX(XRCID("DebugSCRT"), DebugWindow::onDebugScrt)
    EVT_TEXT(XRCID("DebugCallReg"), DebugWindow::onDebugCallReg)
    EVT_TEXT(XRCID("DebugCallAddress"), DebugWindow::onDebugCallAddress)
    EVT_TEXT(XRCID("DebugRetReg"), DebugWindow::onDebugRetReg)
    EVT_TEXT(XRCID("DebugRetAddress"), DebugWindow::onDebugRetAddress)

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

    EVT_CHOICE(XRCID("ProfilerType"), DebugWindow::onProfilerType)
    EVT_CHOICE(XRCID("ProfilerCounter"), DebugWindow::onProfilerCounter)
    EVT_BUTTON(XRCID("ProfilerClear"), DebugWindow::onProfilerClear)

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
    dataViewProfiler = true;
    selectedBreakPoint_ = -1;
    selectedChip8BreakPoint_ = -1;
    selectedTreg_ = -1;
    selectedTrap_ = -1;
    emsNumber_ = 0;

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
    fontSize_ = (int)configPointer->Read("/Main/FontSize", 11);

    dirAssStart_ = 0;
    dirAssEnd_ = 0;
    lastRange_ = 0;
    workingRange_ = -1;
    shownRange_ = -1;
    lastAssError_ = "";

    numberOfDebugLines_ = 35;

#if defined(__WXMAC__)
    wxFont exactFont(fontSize_+2, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#else
    wxFont exactFont(fontSize_+1, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#endif
    wxSize charSize = exactFont.GetPixelSize();
    lineSpace_ = charSize.y - 2;

    wxScreenDC dc;
    dc.SetFont(exactFont);
    charWidth_ = dc.GetCharWidth();

    assWidth_ = charWidth_ * 33 + charWidth_/2;
    profilerWidth_ = charWidth_ * 57 + charWidth_/2;

    assBmp = new wxBitmap(assWidth_, numberOfDebugLines_*lineSpace_+4, 24);
    profilerBmp = new wxBitmap(profilerWidth_, (numberOfDebugLines_-PROFILER_OFFSET)*lineSpace_+4, 24);
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
    delete profilerBmp;
}

void DebugWindow::readDebugConfig()
{
    dirAssConfigFile_ = configPointer->Read("/DebugConfigFile", "debug.config");
    profilerType_ = (int)configPointer->Read("/Main/ProfilerType", 0l);
    profilerCounter_ = (int)configPointer->Read("/Main/ProfilerCounter", 1l);

    dirAssConfigFileDir_ = readConfigDir("/Dir/Main/DebugConfig", dataDir_);
    debugDir_ = readConfigDir("/Dir/Main/Debug", dataDir_);

    if (!mode_.gui)
        return;
#if defined (__WXMSW__)
    breakPointWindowPointer->SetImageList(imageList_, wxIMAGE_LIST_SMALL);
    chip8BreakPointWindowPointer->SetImageList(imageList_, wxIMAGE_LIST_SMALL);
    tregWindowPointer->SetImageList(imageList_, wxIMAGE_LIST_SMALL);
    trapWindowPointer->SetImageList(imageList_, wxIMAGE_LIST_SMALL);
#endif    
    XRCCTRL(*this, "ProfilerBitmap", wxStaticBitmap)->SetSize(profilerWidth_, (numberOfDebugLines_-PROFILER_OFFSET)*lineSpace_+4);
    XRCCTRL(*this, "AssBitmap", wxStaticBitmap)->SetSize(assWidth_, numberOfDebugLines_*lineSpace_+4);

    XRCCTRL(*this, "ProfilerType", wxChoice)->SetSelection(profilerType_);
    XRCCTRL(*this, "ProfilerCounter", wxChoice)->SetSelection(profilerCounter_);
//    XRCCTRL(*this, "DebugPortExtender", HexEdit)->setStart(1);

    int lineWidth = charWidth_ * 16 + charWidth_/2;

    for (int i=0; i<16; i++)
        lineBmp[i] = new wxBitmap(lineWidth, 16, 24);
    paintDebugBackground();
}

void DebugWindow::writeDebugConfig()
{
    configPointer->Write("/DebugConfigFile", dirAssConfigFile_);
    configPointer->Write("/Main/ProfilerType", profilerType_);
    configPointer->Write("/Main/ProfilerCounter", profilerCounter_);

    writeConfigDir("/Dir/Main/DebugConfig", dirAssConfigFileDir_);
    writeConfigDir("/Dir/Main/Debug", debugDir_);
}

void DebugWindow::enableDebugGuiMemory ()
{
    if (!mode_.gui)
        return;
    wxString value;

#ifndef __WXMAC__
    XRCCTRL(*this, "DebugExpansionSlotText", wxStaticText)->Enable(computerConfiguration.slotConfiguration.maxSlotNumber_ > 0);
    XRCCTRL(*this, "DebugExpansionBankText", wxStaticText)->Enable(computerConfiguration.slotConfiguration.banksInUse_);
    XRCCTRL(*this, "DebugEmsPageText", wxStaticText)->Enable(computerConfiguration.emsMemoryConfiguration.size() != 0);
    XRCCTRL(*this, "DebugPagerText", wxStaticText)->Enable(computerConfiguration.memoryMapperConfiguration.defined);
#endif
    XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->Enable(computerConfiguration.slotConfiguration.maxSlotNumber_ > 0);
    XRCCTRL(*this, "DebugExpansionBank", SlotEdit)->Enable(computerConfiguration.slotConfiguration.banksInUse_);
    XRCCTRL(*this, "DebugEmsNumber", HexEdit)->Enable(computerConfiguration.emsMemoryConfiguration.size() != 0);
    XRCCTRL(*this, "DebugEmsPage", HexEdit)->Enable(computerConfiguration.emsMemoryConfiguration.size() != 0);
    XRCCTRL(*this, "DebugPager", HexEdit)->Enable(computerConfiguration.memoryMapperConfiguration.defined);
    XRCCTRL(*this, "DebugPortExtender", HexEdit)->Enable(computerConfiguration.memoryMapperConfiguration.defined);
    p_Main->updateSlotInfo();

    p_Main->setMemDumpColours();
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
    XRCCTRL(*this,"Chip8I", wxTextCtrl)->Enable(status&&!chip8ProtectedMode_);
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
    if (cpuType_ >= CPU1804)
    {
        cieTextPointer->Enable(status&&!protectedMode_);
        chTextPointer->Enable(status&&!protectedMode_);
        counterTextPointer->Enable(status&&!protectedMode_);
    }
    XRCCTRL(*this,"CNTRButton", wxStaticText)->Enable(cpuType_ >= CPU1804);
    XRCCTRL(*this,"CHButton", wxStaticText)->Enable(cpuType_ >= CPU1804);
    XRCCTRL(*this,"CIEButton", wxStaticText)->Enable(cpuType_ >= CPU1804);
    efTextPointer[1]->Enable(status&&!protectedMode_);
    efTextPointer[2]->Enable(status&&!protectedMode_);
    efTextPointer[3]->Enable(status&&!protectedMode_);
    efTextPointer[4]->Enable(status&&!protectedMode_);
    dTextPointer->Enable(status&&!protectedMode_);
    pTextPointer->Enable(status&&!protectedMode_);
    xTextPointer->Enable(status&&!protectedMode_);
    tTextPointer->Enable(status&&!protectedMode_);
    bTextPointer->Enable(status&&!protectedMode_);
//    XRCCTRL(*this,"InputWindow", wxTextCtrl)->Enable(status);
//    XRCCTRL(*this,"DebugAssemblerAddress", wxTextCtrl)->Enable(status);
    XRCCTRL(*this,"ProtectedMode", wxCheckBox)->Enable(status);
    XRCCTRL(*this,"DebugRunButton", wxButton)->Enable(status);
    XRCCTRL(*this,"DebugInterrupt", wxButton)->Enable(status);
    XRCCTRL(*this,"DebugReset", wxButton)->Enable(status);
//    XRCCTRL(*this,"DebugDis", wxButton)->Enable(status);
//    XRCCTRL(*this,"DebugDisLog", wxButton)->Enable(status);
    XRCCTRL(*this,"DebugCopy", wxButton)->Enable(status);
    XRCCTRL(*this, "DebugSave", wxButton)->Enable(status);
    XRCCTRL(*this,"DebugPauseButton", wxBitmapButton)->Enable(status);
    XRCCTRL(*this,"DebugStepButton", wxBitmapButton)->Enable(false);

    if (computerRunning_)
        p_Main->eventPauseState();
    else
        XRCCTRL(*this, "DebugPauseButton", wxBitmapButton)->SetBitmapLabel(pauseOffBitmap);

    XRCCTRL(*this, "DebugDisplayPage", wxTextCtrl)->Enable(memoryDisplay_ != CPU_TYPE);
#if defined(__WXMSW__) || defined(__WXMAC__)
    XRCCTRL(*this, "DebugDisplayPageSpinButton", wxSpinButton)->Enable(memoryDisplay_ != CPU_TYPE);
#endif
    XRCCTRL(*this, "DebugSave", wxButton)->Enable(memoryDisplay_ != CPU_TYPE && memoryDisplay_ != CPU_PROFILER);
    XRCCTRL(*this, "DebugCopy", wxButton)->Enable(memoryDisplay_ != CPU_TYPE && memoryDisplay_ != CPU_PROFILER && memoryDisplay_ != RTCRAM);

#ifndef __WXMAC__
    if (!status)
    {
        XRCCTRL(*this, "DebugPortExtender", HexEdit)->Enable(false);
        XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->Enable(false);
        XRCCTRL(*this, "DebugExpansionBank", SlotEdit)->Enable(false);
        XRCCTRL(*this, "DebugEmsPage", HexEdit)->Enable(false);
        XRCCTRL(*this, "DebugEmsNumber", HexEdit)->Enable(false);
        XRCCTRL(*this, "DebugPager", HexEdit)->Enable(false);
        XRCCTRL(*this, "DebugSave", wxButton)->Enable(false);
        XRCCTRL(*this, "DebugExpansionSlotText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "DebugPagerText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "DebugEmsPageText", wxStaticText)->Enable(false);
        XRCCTRL(*this, "DebugExpansionBankText", wxStaticText)->Enable(false);
        p_Main->setMemDumpColours();
    }
#endif
}

void DebugWindow::updateAssTabCheck(Word address)
{
    if ((dirAssStart_ & 0x8000) == 0x8000 && (dirAssEnd_ & 0x8000) == 0)
    {
        int offset = 0x10000 - dirAssStart_;
        address = (address + offset) & 0xffff;
        if (address>= 0 && address< (dirAssEnd_+ offset))
            p_Main->updateAssTab();
        return;
    }
    if (address>= dirAssStart_ && address<dirAssEnd_)
        p_Main->updateAssTab();
}

void DebugWindow::cyclePseudoDebug()
{
    if (!pseudoLoaded_)
        return;
    
    Byte programCounter = p_Computer->getProgramCounter();
    Word programCounterAddress = p_Computer->getScratchpadRegister(programCounter);
    
    if (selectedTab_ == DEBUGGERTAB && debuggerChoice_ == CHIP8TAB)
    {
        if (programCounterAddress == p_Computer->getChip8MainLoop())
            updateChip8Window();
    }
    
    Word chip8PC;
    
    if (pseudoType_ == "CARDTRAN")
        chip8PC = p_Computer->getScratchpadRegister(CARDTRAN_PC);
    else
        chip8PC = p_Computer->getScratchpadRegister(CHIP8_PC);
    
    if (programCounterAddress == p_Computer->getChip8MainLoop())
    {
        p_Computer->writeMemDataType(chip8PC, MEM_TYPE_PSEUDO_1);

        Byte command = p_Computer->readMemDebug(chip8PC);

        if (!checkSingleCommand(command))
            p_Computer->writeMemDataType((chip8PC+1)&0xffff, MEM_TYPE_PSEUDO_2);

        if (checkTrippleCommand(command))
            p_Computer->writeMemDataType((chip8PC+2)&0xffff, MEM_TYPE_PSEUDO_2);

        if (checkQuadrupleCommand(command))
        {
            p_Computer->writeMemDataType((chip8PC+2)&0xffff, MEM_TYPE_PSEUDO_2);
            p_Computer->writeMemDataType((chip8PC+3)&0xffff, MEM_TYPE_PSEUDO_2);
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
                    chip8DebugTrace(addDetails());
                if (chip8Steps_ == 1)
                {
                    p_Computer->setSteps(0);
                    chip8Steps_--;
                    setChip8PauseState();
                }
                if (chip8Steps_ != 0)
                {
                    p_Computer->setSteps(-1);
//                    if (chip8BreakPointCheck())  return;
                    if (chip8Trace_)
                        pseudoTrace(chip8PC);
                    chip8Steps_--;
                }
                if (chip8Steps_ == 0)
                    p_Computer->setSteps(0);
            }
        }
        else
        {
            if (programCounterAddress == p_Computer->getChip8MainLoop())
            {
                if (additionalChip8Details_)
                    chip8DebugTrace(addDetails());
                if (chip8BreakPointCheck())  return;
                if (chip8Trace_)
                    pseudoTrace(chip8PC);
            }
        }
    }
}

bool DebugWindow::checkSingleCommand(Byte command)
{
    for (size_t i=0; i<singleByteCommandNumber_; i++)
    {
        if (command == singleByteCommand_[i])
            return true;
    }
    return false;
}

bool DebugWindow::checkTrippleCommand(Byte command)
{
    for (size_t i=0; i<trippleByteCommandNumber_; i++)
    {
/*        if ((trippleByteCommand_[i] & 0xf) == 0xf)
        {
            if ((command&0xf0) == (trippleByteCommand_[i]&0xf0))
                return true;
        }
        else
        {*/
            if (command == trippleByteCommand_[i])
                return true;
   //     }
    }
    return false;
}

bool DebugWindow::checkQuadrupleCommand(Byte command)
{
    for (size_t i=0; i<quadrupleByteCommandNumber_; i++)
    {
        if (command == quadrupleByteCommand_[i])
            return true;
    }
    return false;
}

bool DebugWindow::chip8BreakPointCheck()
{
    Word chip8PC;
    if (pseudoType_ == "CARDTRAN")
        chip8PC = p_Computer->getScratchpadRegister(CARDTRAN_PC);
    else
        chip8PC = p_Computer->getScratchpadRegister(CHIP8_PC) & 0xfff;
    
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
        if (selectedTab_ == DEBUGGERTAB && debuggerChoice_ == TRACETAB)
            p_Main->updateWindow();

    if (p_Computer->getSteps() != 0 && numberOfBreakPoints_ > 0)
    {
        for (int i=0; i<numberOfBreakPoints_; i++)
        {
            if (breakPoints_[i] == programCounterAddress && breakPointsSelected_[i])
            {
                p_Computer->setSteps(0);
                debugTrace("Hit Breakpoint");
                p_Main->eventPauseState();
                i = numberOfBreakPoints_;
            }
        }
    }
    if (p_Computer->getSteps() != 0 && numberOfTraps_ > 0)
    {
        for (int i=0; i<numberOfTraps_; i++)
        {
            mask = traps_[i][7];
            instruction = p_Computer->readMemDebug(programCounterAddress);
            if (instruction == 0x60)
                mask = 0xff;
            if (instruction == 0x68)
            {
                if ((traps_[i][0] == 1 && traps_[i][1] == instruction && trapsSelected_[i]) ||
                    (traps_[i][0] == 2 && traps_[i][1] == instruction &&
                                         (traps_[i][2]&mask) == (p_Computer->readMemDebug(programCounterAddress+1)&mask) && trapsSelected_[i]) ||
                    (traps_[i][0] == 3 && traps_[i][1] == instruction &&
                                         (traps_[i][2]&mask) == (p_Computer->readMemDebug(programCounterAddress+1)&mask) &&
                                         traps_[i][3] == p_Computer->readMemDebug(programCounterAddress+2) && trapsSelected_[i]) ||
                    (traps_[i][0] == 4 && traps_[i][1] == instruction &&
                                         (traps_[i][2]&mask) == (p_Computer->readMemDebug(programCounterAddress+1)&mask) &&
                                         traps_[i][3] == p_Computer->readMemDebug(programCounterAddress+2) &&
                                         traps_[i][4] == p_Computer->readMemDebug(programCounterAddress+3) && trapsSelected_[i]))
                {
                    trapFound = true;
                }
            }
            else
            {
                if ((traps_[i][0] == 1 && (traps_[i][1]&mask) == (instruction&mask) && trapsSelected_[i]) ||
                    (traps_[i][0] == 2 && traps_[i][1] == instruction &&
                                         traps_[i][2] == p_Computer->readMemDebug(programCounterAddress+1) && trapsSelected_[i]) ||
                    (traps_[i][0] == 3 && traps_[i][1] == instruction &&
                                         traps_[i][2] == p_Computer->readMemDebug(programCounterAddress+1) &&
                                         traps_[i][3] == p_Computer->readMemDebug(programCounterAddress+2) && trapsSelected_[i]))
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
                    p_Main->eventPauseState();
                    i = numberOfTraps_;
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
                    printBuffer.Printf("T=%02X",p_Computer->getRegisterT());
                    j = i;
                }
                if (tregs_[i][0] == TREG_B && tregs_[i][1] == p_Computer->getRegisterB())
                {
                    printBuffer.Printf("B=%02X",p_Computer->getRegisterB());
                    j = i;
                }
                if (tregs_[i][0] == TREG_CH && tregs_[i][1] == p_Computer->getCounterJamValue())
                {
                    printBuffer.Printf("CH=%02X",p_Computer->getCounterJamValue());
                    j = i;
                }
                if (tregs_[i][0] == TREG_CNTR && tregs_[i][1] == p_Computer->getCounterTimer())
                {
                    printBuffer.Printf("CNTR=%02X",p_Computer->getCounterTimer());
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
            p_Main->eventPauseState();
        }
    }

    if (p_Computer->getSteps() > 0)
    {
        if (!p_Computer->getSkipTraceMode())
            p_Computer->setSteps(p_Computer->getSteps()-1);

        if (p_Computer->getSteps() == 0)
            p_Main->eventPauseState();
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
                    instruction = p_Computer->readMemDebug(programCounterAddress);
                    if (instruction == 0x60)
                        mask = 0xff;
                    if (instruction == 0x68)
                    {
                        if ((traps_[i][0] == 1 && traps_[i][1] == instruction && trapsSelected_[i]) ||
                            (traps_[i][0] == 2 && traps_[i][1] == instruction &&
                                                 (traps_[i][2]&mask) == (p_Computer->readMemDebug(programCounterAddress+1)&mask) && trapsSelected_[i]) ||
                            (traps_[i][0] == 3 && traps_[i][1] == instruction &&
                                                 (traps_[i][2]&mask) == (p_Computer->readMemDebug(programCounterAddress+1)&mask) &&
                                                 traps_[i][3] == p_Computer->readMemDebug(programCounterAddress+2) && trapsSelected_[i]) ||
                            (traps_[i][0] == 4 && traps_[i][1] == instruction &&
                                                 (traps_[i][2]&mask) == (p_Computer->readMemDebug(programCounterAddress+1)&mask) &&
                                                 traps_[i][3] == p_Computer->readMemDebug(programCounterAddress+2) &&
                                                 traps_[i][4] == p_Computer->readMemDebug(programCounterAddress+3) && trapsSelected_[i]))
                        {
                            trapFound = true;
                        }
                    }
                    else
                    {
                        if ((traps_[i][0] == 1 && (traps_[i][1]&mask) == (instruction&mask) && trapsSelected_[i]) ||
                            (traps_[i][0] == 2 && traps_[i][1] == instruction &&
                                                 traps_[i][2] == p_Computer->readMemDebug(programCounterAddress+1) && trapsSelected_[i]) ||
                            (traps_[i][0] == 3 && traps_[i][1] == instruction &&
                                                 traps_[i][2] == p_Computer->readMemDebug(programCounterAddress+1) &&
                                                 traps_[i][3] == p_Computer->readMemDebug(programCounterAddress+2) && trapsSelected_[i]))
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
        p_Main->eventPauseState();
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
    lastB_ = p_Computer->getRegisterB() + 1;
    lastDf_ = p_Computer->getDataFlag() ^ 1;
    lastQ_ = p_Computer->getFlipFlopQ() ^ 1;
    lastIe_ = p_Computer->getInterruptEnable() ^ 1;
    lastCie_ = p_Computer->getCounterInterruptEnable() ^ 1;
    lastCh_ = p_Computer->getCounterJamValue() ^ 1;
    lastCounter_ = p_Computer->getCounterTimer() ^ 1;
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
    if (pseudoType_ == "CARDTRAN")
        scratchpadRegister = p_Computer->getScratchpadRegister(CARDTRAN_PC);
    else
        scratchpadRegister = p_Computer->getScratchpadRegister(CHIP8_PC);
    if (scratchpadRegister != lastPC_)
    {
        if (pseudoType_ == "STIV")
            buffer.Printf("%04X", scratchpadRegister);
        else
        {
            if (pseudoType_ == "SUPERCHIP")
                buffer.Printf("%04X", scratchpadRegister&0xffff);
            else
                buffer.Printf("%03X", scratchpadRegister&0xfff);
        }
        p_Main->eventSetTextValue("Chip8PC", buffer);
        lastPC_ = scratchpadRegister;
    }
    if (pseudoType_ != "CARDTRAN")
    {
        if (pseudoType_ == "STIV")
            scratchpadRegister = (p_Computer->readMemDebug(0x27f6)<<8)+p_Computer->readMemDebug(0x27f7);
        else
            scratchpadRegister = p_Computer->getScratchpadRegister(CHIP8_I);
        if (scratchpadRegister != lastI_)
        {
            if (pseudoType_ == "STIV")
                buffer.Printf("%04X", scratchpadRegister);
            else
            {
                if (pseudoType_ == "SUPERCHIP")
                    buffer.Printf("%04X", scratchpadRegister&0xffff);
                else
                    buffer.Printf("%03X", scratchpadRegister&0xfff);
            }
            p_Main->eventSetTextValue("Chip8I", buffer);
            lastI_ = scratchpadRegister;
        }
        p_Computer->showChip8Registers();
    }
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
            if (computerConfiguration.vis1870Configuration.defined)
            {
                if (computerConfiguration.vis1870Configuration.outputWrite.portNumber[0] != -1)
                {
                    if (i==5 && (p_Computer->getOutValue(7) != 0x20) && (p_Computer->getOutValue(7) != 0x30))
                        buffer.Printf("%04X",p_Computer->getOutValue(i));
                    else
                        buffer.Printf("%02X",p_Computer->getOutValue(i));
                }
                else
                {
                    if (i>3)
                        buffer.Printf("%04X",p_Computer->getOutValue(i));
                    else
                        buffer.Printf("%02X",p_Computer->getOutValue(i));
                }
            }
            else
                buffer.Printf("%02X",p_Computer->getOutValue(i));

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
    cpucpuRegister = p_Computer->getRegisterB();
    if (cpucpuRegister != lastB_)
    {
        buffer.Printf("%02X", cpucpuRegister);
        bTextPointer->ChangeValue(buffer);
        lastB_ = cpucpuRegister;
    }
    cpucpuRegister = p_Computer->getCounterJamValue();
    if (cpucpuRegister != lastCh_)
    {
        buffer.Printf("%02X", cpucpuRegister);
        if (cpuType_ < CPU1804)
            buffer = "--";
        chTextPointer->ChangeValue(buffer);
        lastCh_ = cpucpuRegister;
    }
    cpucpuRegister = p_Computer->getCounterTimer();
    if (cpucpuRegister != lastCounter_)
    {
        buffer.Printf("%02X", cpucpuRegister);
        if (cpuType_ < CPU1804)
            buffer = "--";
        counterTextPointer->ChangeValue(buffer);
        lastCounter_ = cpucpuRegister;
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
    cpuFlag = p_Computer->getCounterInterruptEnable();
    if (cpuFlag != lastCie_)
    {
        buffer.Printf("%01X", cpuFlag);
        if (cpuType_ < CPU1804)
            buffer = "-";
        cieTextPointer->ChangeValue(buffer);
        lastCie_ = cpuFlag;
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
//    if (buffer.Len() < 17)
  //      return;
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
            case MEM_TYPE_OPCODE_LDRL:
            case MEM_TYPE_OPCODE_LDRL_SLOT:
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
                    if (cpuType_ == SYSTEM00)
                        printBuffer.Printf("BNZ  ");
                    else
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
                    if (cpuType_ <= CPU1801)
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
            if (cpuType_ == SYSTEM00)
            {
                if (traps_[i][7] == 0xf0)
                    printBuffer.Printf("PNI  Rx");
                else
                    printBuffer.Printf("PNI  R%X",n);
                break;
            }
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

        case TREG_B: printBuffer.Printf("B  %02X", tregs_[numberOfTregs_][1]);
        break;
            
        case TREG_CH: printBuffer.Printf("CH %02X", tregs_[numberOfTregs_][1]);
        break;

        case TREG_CNTR: printBuffer.Printf("CN %02X", tregs_[numberOfTregs_][1]);
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
    *buffer = buffer->Mid(end, buffer->Len()-end);
    buffer->Trim(false);
    
    if (buffer->Left(1) != "/" && buffer->Left(1) != "," && buffer->Left(1) != "+" && buffer->Left(1) != "-" && buffer->Left(1) != "=")
        *seperator = " ";
    else
    {
        *seperator = buffer->Left(1);
        *buffer = buffer->Right(buffer->Len()-1);
    }
    return ret;
}

wxString DebugWindow::cdp1802disassemble(Word* address, bool showDetails, bool showOpcode, bool textAssembler, Word start, Word end)
{
    wxString printBufferOpcode, printBufferAssembler, printBufferTemp, printBufferAddress, printBufferDetails;
    int i, n, i1805, n1805;
    Word instructionAddress = *address;
    uint64_t executed;
    Byte memType;
    
    i = p_Computer->readMemDebug(*address);

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
            if (cpuType_ == SYSTEM00)
                printBufferAssembler.Printf("IDL  R%X", n);
            else
            {
                switch(n)
                {
                    case 0x0:
                        printBufferAssembler.operator += ("IDL");
                    break;

                    default:
                        if (cpuType_ == CPU1801)
                            printBufferAssembler.Printf("Illegal code");
                        else
                        {
                            printBufferAssembler.Printf("LDN  R%X",n);
                            printBufferDetails.Printf("D=%02X", accumulator);
                        }
                    break;
                }
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
            if (n > 7 && cpuType_ == SYSTEM00)
            {
                printBufferAssembler.Printf("Illegal code");
                break;
            }
            switch(n)
            {
                case 0x0:
                    printBufferAssembler = "BR   " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X ", p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 1;
                break;
                case 0x1:
                    if (cpuType_ <= CPU1801)
                    {
                        if (cpuType_ == CPU1801)
                            printBufferAssembler.Printf("Illegal code");
                        else
                        {
                            printBufferAssembler = "BNZ  " + getShortAddressOrLabel(*address, textAssembler, start, end);
                            printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                            printBufferOpcode.operator += (printBufferTemp);
                        }
                    }
                    else
                    {
                        printBufferAssembler = "BQ   " + getShortAddressOrLabel(*address, textAssembler, start, end);
                        printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                        printBufferOpcode.operator += (printBufferTemp);
                    }
                    *address = *address + 1;
                break;
                case 0x2:
                    printBufferAssembler = "BZ   " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 1;
                break;
                case 0x3:
                    if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_BPZ)
                        printBufferAssembler = "BPZ  " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    else
                    {
                        if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_BGE)
                            printBufferAssembler = "BGE  " + getShortAddressOrLabel(*address, textAssembler, start, end);
                        else
                            printBufferAssembler = "BDF  " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    }
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 1;
                break;
                case 0x4:
                    printBufferAssembler = "B1   " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 1;
                break;
                case 0x5:
                    printBufferAssembler = "B2   " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 1;
                break;
                case 0x6:
                    printBufferAssembler = "B3   " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 1;
                break;
                case 0x7:
                    printBufferAssembler = "B4   " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 1;
                break;
                case 0x8:
                    if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_SKP || textAssembler)
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
                        printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                        printBufferOpcode.operator += (printBufferTemp);
                    }
                    *address = *address + 1;
                break;
                case 0xa:
                    printBufferAssembler = "BNZ  " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 1;
                break;
                case 0xb:
                    if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_BM)
                        printBufferAssembler = "BM   " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    else
                    {
                        if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_BL)
                            printBufferAssembler = "BL   " + getShortAddressOrLabel(*address, textAssembler, start, end);
                        else
                            printBufferAssembler = "BNF  " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    }
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 1;
                break;
                case 0xc:
                    printBufferAssembler = "BN1  " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 1;
                break;
                case 0xd:
                    printBufferAssembler = "BN2  " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 1;
                break;
                case 0xe:
                    printBufferAssembler = "BN3  " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 1;
                break;
                case 0xf:
                    printBufferAssembler = "BN4  " + getShortAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
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
            if (cpuType_ == SYSTEM00)
            {
                switch (n)
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        printBufferAssembler.Printf("OUT  %X",n);
                        printBufferDetails.Printf("[%02X]", p_Computer->readMemDebug(p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1));
                    break;
                        
                    case 8:
                        printBufferAssembler.Printf("INP");
                        printBufferDetails.Printf("M(%04X)=%02X", p_Computer->getScratchpadRegister(p_Computer->getDataPointer()), p_Computer->readMemDebug(p_Computer->getScratchpadRegister(p_Computer->getDataPointer())));
                    break;
                        
                    default:
                        printBufferAssembler.Printf("Illegal code");
                    break;
                }
                break;
            }
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
                        printBufferDetails.Printf("[%02X]", p_Computer->readMemDebug(p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1));
                    }
                break;
                case 0x1:
                case 0x2:
                case 0x3:
                case 0x4:
                case 0x5:
                case 0x6:
                case 0x7:
                //    if (n > 2 || (n == 1 && p_Computer->getOutValue(n) > 3))
                //    {
                    printBufferAssembler.Printf("OUT  %X",n);
                    if (computerConfiguration.vis1870Configuration.defined)
                    {
                        if (computerConfiguration.vis1870Configuration.outputWrite.portNumber[0] != -1)
                        {
                            if (n==5 && (p_Computer->getOutValue(7) != 0x20) && (p_Computer->getOutValue(7) != 0x30))
                                printBufferDetails.Printf("[%04X]", p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1);
                            else
                                printBufferDetails.Printf("[%02X]", p_Computer->readMemDebug(p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1));
                        }
                        else
                        {
                            if (n>3)
                                printBufferDetails.Printf("[%04X]", p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1);
                            else
                                printBufferDetails.Printf("[%02X]", p_Computer->readMemDebug(p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1));
                        }
                    }
                    else
                        printBufferDetails.Printf("[%02X]", p_Computer->readMemDebug(p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1));
                break;
                case 0x8:
                    if (cpuType_ == CPU1804 || cpuType_ == CPU1805)
                    {
                        i1805 = p_Computer->readMemDebug(*address);

                        printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
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
                                    printBufferTemp.Printf("%02X %02X ", p_Computer->readMemDebug(*address),p_Computer->readMemDebug((*address)+1));
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
                                        printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                                        printBufferOpcode.operator += (printBufferTemp);
                                        *address = *address + 1;
                                    break;
                                    case 0xf: // 1804
                                        printBufferAssembler = "BXI  " + getShortAddressOrLabel(*address, textAssembler, start, end);
                                        printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
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
                                            printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                                            printBufferOpcode.operator += (printBufferTemp);
                                            *address = *address + 1;
                                        break;
                                        case 0xf:
                                            printBufferAssembler = "DSBI " + getHexByte(*address, textAssembler);
                                            printBufferDetails.Printf("D=%02X", accumulator);
                                            printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
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
                                printBufferTemp.Printf("%02X %02X ", p_Computer->readMemDebug(*address),p_Computer->readMemDebug((*address)+1));
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
                                if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_RLDL)
                                {
                                    printBufferAssembler.Printf("RLDL R%X,",n1805);
                                    printBufferAssembler = printBufferAssembler + getSubAddressOrLabel(*address, textAssembler, start, end);
                                }
                                else
                                    printBufferAssembler.Printf("RLDI R%X,%04X",n1805, p_Computer->readMemDebug(*address)<<8|p_Computer->readMemDebug((*address)+1));
                                printBufferTemp.Printf("%02X %02X ", p_Computer->readMemDebug(*address),p_Computer->readMemDebug((*address)+1));
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
                                            printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                                            printBufferOpcode.operator += (printBufferTemp);
                                            *address = *address + 1;
                                        break;
                                        case 0xf:
                                            printBufferAssembler = "DSMI " + getHexByte(*address, textAssembler);
                                            printBufferDetails.Printf("D=%02X", accumulator);
                                            printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
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
                    if (cpuType_ == SYSTEM00)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler.operator += ("DIS");
                        printBufferDetails.Printf("P=R%X, X=R%X", p_Computer->getProgramCounter(), p_Computer->getDataPointer());
                    }
                break;
                case 0x2:
                    if (cpuType_ <= CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler.operator += ("LDXA");
                        printBufferDetails.Printf("D=M(%04X)=%02X", p_Computer->getScratchpadRegister(p_Computer->getDataPointer())-1, accumulator);
                    }
                break;
                case 0x3:
                    if (cpuType_ <= CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler.operator += ("STXD");
                        printBufferDetails.Printf("M(%04X)=%02X", p_Computer->getScratchpadRegister(p_Computer->getDataPointer())+1, accumulator);
                    }
                break;
                case 0x4:
                    if (cpuType_ <= CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler.operator += ("ADC");
                        printBufferDetails.Printf("D=%02X", accumulator);
                    }
                break;
                case 0x5:
                    if (cpuType_ <= CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler.operator += ("SDB");
                        printBufferDetails.Printf("D=%02X", accumulator);
                    }
                break;
                case 0x6:
                    if (cpuType_ <= CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_RSHR)
                            printBufferAssembler.operator += ("RSHR");
                        else
                            printBufferAssembler.operator += ("SHRC");
                        printBufferDetails.Printf("D=%02X", accumulator);
                    }
                break;
                case 0x7:
                    if (cpuType_ <= CPU1801)
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
                    if (cpuType_ <= CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                        printBufferAssembler.operator += ("MARK");
                break;
                case 0xa:
                    if (cpuType_ <= CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                        printBufferAssembler.operator += ("REQ");
                break;
                case 0xb:
                    if (cpuType_ <= CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                        printBufferAssembler.operator += ("SEQ");
                break;
                case 0xc:
                    if (cpuType_ <= CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler = "ADCI " + getHexByte(*address, textAssembler);
                        printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                        printBufferOpcode.operator += (printBufferTemp);
                        printBufferDetails.Printf("D=%02X", accumulator);
                        *address = *address + 1;
                    }
                break;
                case 0xd:
                    if (cpuType_ <= CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler = "SDBI " + getHexByte(*address, textAssembler);
                        printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                        printBufferOpcode.operator += (printBufferTemp);
                        printBufferDetails.Printf("D=%02X", accumulator);
                        *address = *address + 1;
                    }
                break;
                case 0xe:
                    if (cpuType_ <= CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_RSHL)
                            printBufferAssembler.operator += ("RSHL");
                        else
                            printBufferAssembler.operator += ("SHLC");
                        printBufferDetails.Printf("D=%02X", accumulator);
                    }
                break;
                case 0xf:
                    if (cpuType_ <= CPU1801)
                        printBufferAssembler.Printf("Illegal code");
                    else
                    {
                        printBufferAssembler = "SMBI " + getHexByte(*address, textAssembler);
                        printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
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
            if (cpuType_ <= CPU1801)
            {
                if (cpuType_ == SYSTEM00)
                {
                    printBufferAssembler.Printf("PNI  R%X",n);
                    printBufferDetails.Printf("R%X=%04X", n, p_Computer->getScratchpadRegister(n));
                }
                else
                    printBufferAssembler.Printf("Illegal code");
            }
            else
            {
            switch(n)
            {
                case 0x0:
                    if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_LBR_SLOT)
                        printBufferAssembler.Printf("LBR  S%02X,",p_Computer->readMemDataType(*address, &executed));
                    else
                            printBufferAssembler = "LBR  ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X %02X ",p_Computer->readMemDebug(*address),p_Computer->readMemDebug((*address)+1));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 2;
                break;
                case 0x1:
                    if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_LBR_SLOT)
                        printBufferAssembler.Printf("LBQ  S%02X",p_Computer->readMemDataType(*address, &executed));
                    else
                        printBufferAssembler = "LBQ  ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X %02X ",p_Computer->readMemDebug(*address),p_Computer->readMemDebug((*address)+1));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 2;
                break;
                case 0x2:
                    if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_LBR_SLOT)
                        printBufferAssembler.Printf("LBZ  S%02X,",p_Computer->readMemDataType(*address, &executed));
                    else
                        printBufferAssembler = "LBZ  ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X %02X ",p_Computer->readMemDebug(*address),p_Computer->readMemDebug((*address)+1));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 2;
                break;
                case 0x3:
                    if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_LBR_SLOT)
                        printBufferAssembler.Printf("LBDF S%02X,",p_Computer->readMemDataType(*address, &executed));
                    else
                        printBufferAssembler = "LBDF ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X %02X ",p_Computer->readMemDebug(*address),p_Computer->readMemDebug((*address)+1));
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
                    if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_LSKP || textAssembler)
                        printBufferAssembler.operator += ("LSKP");
                    else
                        printBufferAssembler.operator += ("NLBR");
                break;
                case 0x9:
                    if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_LBR_SLOT)
                        printBufferAssembler.Printf("LBNQ S%02X,",p_Computer->readMemDataType(*address, &executed));
                    else
                        printBufferAssembler = "LBNQ ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X %02X ",p_Computer->readMemDebug(*address),p_Computer->readMemDebug((*address)+1));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 2;
                break;
                case 0xa:
                    if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_LBR_SLOT)
                        printBufferAssembler.Printf("LBNZ S%02X,",p_Computer->readMemDataType(*address, &executed));
                    else
                        printBufferAssembler = "LBNZ ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X %02X ",p_Computer->readMemDebug(*address),p_Computer->readMemDebug((*address)+1));
                    printBufferOpcode.operator += (printBufferTemp);
                    *address = *address + 2;
                break;
                case 0xb:
                    if (p_Computer->readMemDataType(instructionAddress, &executed) == MEM_TYPE_OPCODE_LBR_SLOT)
                        printBufferAssembler.Printf("LBNF S%02X,",p_Computer->readMemDataType(*address, &executed));
                    else
                        printBufferAssembler = "LBNF ";
                    printBufferAssembler = printBufferAssembler + getLongAddressOrLabel(*address, textAssembler, start, end);
                    printBufferTemp.Printf("%02X %02X ",p_Computer->readMemDebug(*address),p_Computer->readMemDebug((*address)+1));
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
            if (n > 6 && cpuType_ == SYSTEM00)
            {
                printBufferAssembler.Printf("Illegal code");
                break;
            }
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
                    memType = p_Computer->readMemDataType(instructionAddress, &executed);
                    if (memType == MEM_TYPE_OPCODE_LDV || memType == MEM_TYPE_OPCODE_LDL || memType == MEM_TYPE_OPCODE_LDL_SLOT || memType == MEM_TYPE_OPCODE_LDRL || memType == MEM_TYPE_OPCODE_LDRL_SLOT)
                    {
                        switch (memType)
                        {
                            case MEM_TYPE_OPCODE_LDV:
                                if (textAssembler)
                                {
                                    if (p_Computer->readMemDebug(*address) >= 0xA0)
                                        printBufferAssembler.Printf("LDV  R%X,%03X%02XH",p_Computer->readMemDebug(*address+1)&0xf, p_Computer->readMemDebug(*address), p_Computer->readMemDebug(*address+3));
                                    else
                                        printBufferAssembler.Printf("LDV  R%X,%02X%02XH",p_Computer->readMemDebug(*address+1)&0xf, p_Computer->readMemDebug(*address), p_Computer->readMemDebug(*address+3));
                                }
                                else
                                    printBufferAssembler.Printf("LDV  R%X,%02X%02X",p_Computer->readMemDebug(*address+1)&0xf, p_Computer->readMemDebug(*address), p_Computer->readMemDebug(*address+3));
                            break;
                                
                            case MEM_TYPE_OPCODE_LDL_SLOT:
                                printBufferAssembler.Printf("LDL  S%02X,R%X,", p_Computer->readMemDataType(*address, &executed),p_Computer->readMemDebug(*address+1)&0xf);
                            break;
                                
                            case MEM_TYPE_OPCODE_LDL:
                                if (textAssembler)
                                {
                                    if (p_Computer->readMemDebug(*address) >= 0xA0)
                                        printBufferAssembler.Printf("LDL  R%X,%03X%02XH",p_Computer->readMemDebug(*address+1)&0xf, p_Computer->readMemDebug(*address), p_Computer->readMemDebug(*address+3));
                                    else
                                        printBufferAssembler.Printf("LDL  R%X,%02X%02XH",p_Computer->readMemDebug(*address+1)&0xf, p_Computer->readMemDebug(*address), p_Computer->readMemDebug(*address+3));
                                }
                                else
                                    printBufferAssembler.Printf("LDL  R%X,%02X%02X",p_Computer->readMemDebug(*address+1)&0xf, p_Computer->readMemDebug(*address), p_Computer->readMemDebug(*address+3));
                            break;
                                
                            case MEM_TYPE_OPCODE_LDRL_SLOT:
                                printBufferAssembler.Printf("LDRL S%02X,R%X,", p_Computer->readMemDataType(*address, &executed),p_Computer->readMemDebug(*address+1)&0xf);
                            break;
                                
                            case MEM_TYPE_OPCODE_LDRL:
                                if (textAssembler)
                                {
                                    if (p_Computer->readMemDebug(*address) >= 0xA0)
                                        printBufferAssembler.Printf("LDRL R%X,%03X%02XH",p_Computer->readMemDebug(*address+1)&0xf, p_Computer->readMemDebug(*address+3), p_Computer->readMemDebug(*address));
                                    else
                                        printBufferAssembler.Printf("LDRL R%X,%02X%02XH",p_Computer->readMemDebug(*address+1)&0xf, p_Computer->readMemDebug(*address+3), p_Computer->readMemDebug(*address));
                                }
                                else
                                    printBufferAssembler.Printf("LDRL R%X,%02X%02X",p_Computer->readMemDebug(*address+1)&0xf, p_Computer->readMemDebug(*address+3), p_Computer->readMemDebug(*address));
                            break;
                        }

                        printBufferTemp.Printf("%02X %02X",p_Computer->readMemDebug(*address), p_Computer->readMemDebug(*address+1));
                        printBufferOpcode.operator += (printBufferTemp);
                        *address = *address + 5;
                    }
                    else
                    {
                        printBufferAssembler = "LDI  " + getHexByte(*address, textAssembler);
                        printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                        printBufferOpcode.operator += (printBufferTemp);
                        printBufferDetails.Printf("D=%02X", accumulator);
                        *address = *address + 1;
                    }
                break;
                case 0x9:
                    printBufferAssembler = "ORI  " + getHexByte(*address, textAssembler);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    printBufferDetails.Printf("D=%02X", accumulator);
                    *address = *address + 1;
                break;
                case 0xa:
                    printBufferAssembler = "ANI  " + getHexByte(*address, textAssembler);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    printBufferDetails.Printf("D=%02X", accumulator);
                    *address = *address + 1;
                break;
                case 0xb:
                    printBufferAssembler = "XRI  " + getHexByte(*address, textAssembler);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    printBufferDetails.Printf("D=%02X", accumulator);
                    *address = *address + 1;
                break;
                case 0xc:
                    printBufferAssembler = "ADI  " + getHexByte(*address, textAssembler);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
                    printBufferOpcode.operator += (printBufferTemp);
                    printBufferDetails.Printf("D=%02X", accumulator);
                    *address = *address + 1;
                break;
                case 0xd:
                    printBufferAssembler = "SDI  " + getHexByte(*address, textAssembler);
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
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
                    printBufferTemp.Printf("%02X ",p_Computer->readMemDebug(*address));
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
        while (printBufferTemp.Len()<= 24)
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
    
    Word branchAddress = (address&0xff00) + p_Computer->readMemDebug(address);
    
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
        branchAddressString.Printf("%02X", p_Computer->readMemDebug(address));
    
   return branchAddressString;
}

wxString DebugWindow::getLongAddressOrLabel(Word address, bool textAssembler, Word start, Word end)
{
    wxString branchAddressString;
    
    Word branchAddress = p_Computer->readMemDebug(address)<<8|p_Computer->readMemDebug(address+1);

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
    
    Word branchAddress = p_Computer->readMemDebug(address)<<8|p_Computer->readMemDebug(address+1);

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
    
    Word branchAddress = p_Computer->readMemDebug(address)<<8|p_Computer->readMemDebug(address+3);
    
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

wxString DebugWindow::getReversedLoadAddressOrLabel(Word address, Word start, Word end)
{
    wxString branchAddressString;
    
    Word branchAddress = p_Computer->readMemDebug(address+3)<<8|p_Computer->readMemDebug(address);
    
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
    
    Word branchAddress = p_Computer->readMemDebug(address)<<8|p_Computer->readMemDebug(address+3);
    
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
        if (p_Computer->readMemDebug(address) >= 0xA0)
            branchAddressString.Printf("%03XH", p_Computer->readMemDebug(address));
        else
            branchAddressString.Printf("%02XH", p_Computer->readMemDebug(address));
    }
    else
        branchAddressString.Printf("%02X",p_Computer->readMemDebug(address));
    
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

int DebugWindow::assemblePseudo(wxString *buffer, Byte* b1, Byte* b2, Byte* b3, Byte* b4)
{
    int32_t pseudoCode;

    AssInput assInput = getAssInput(*buffer);
    if (assInput.errorCode != 0)  return assInput.errorCode;
    if (assInput.commandSeperator != " ") return ERROR_COMMAND_SEP;

    wxString pseudoLine;

    if (inFile.Open(commandSyntaxFile_))
    {
        for (pseudoLine=inFile.GetFirstLine(); !inFile.Eof(); pseudoLine=inFile.GetNextLine())
        {
            pseudoLine.Trim(false);
            pseudoLine.Trim(true);

            if (assInput.command == pseudoLine)
            {
                int returnValue = checkParameterPseudo(assInput, &pseudoCode);
                if (returnValue > ERROR_START)
                    return returnValue;

                switch (returnValue)
                {
                    case 1:
                        *b1 = pseudoCode & 0xff;
                    break;

                    case 2:
                        *b1 = (pseudoCode & 0xff00) >> 8;
                        *b2 = pseudoCode & 0xff;
                    break;

                    case 3:
                        *b1 = (pseudoCode & 0xff0000) >> 16;
                        *b2 = (pseudoCode & 0xff00) >> 8;
                        *b3 = pseudoCode & 0xff;
                    break;

                    case 4:
                        *b1 = (pseudoCode & 0xff000000) >> 24;
                        *b2 = (pseudoCode & 0xff0000) >> 16;
                        *b3 = (pseudoCode & 0xff00) >> 8;
                        *b4 = pseudoCode & 0xff;
                    break;
                }
                
                return returnValue;
            }
        }
    }
    inFile.Close();

    return ERROR_INST;
}

int DebugWindow::checkParameterPseudo(AssInput assInput, int32_t* pseudoCode)
{
    int parameterNumber;
    bool parameterFound=false;
    wxString pseudoLine, parameter, lowStr, highStr, regNumberStr;
    long high, low, regNumber;
    Word hexValue = 0, registerX = 0, registerY = 0, registerZ = 0, registerR = 0, nValue = 0, lValue = 0, oValue = 0, pValue = 0, qValue = 0, kkValue = 0, vvValue = 0, wwValue = 0, ddValue = 0;
    int errorValue = ERROR_MISSING_PAR;
    int32_t tempValue;

    parameterNumber = 0;
    for (pseudoLine=inFile.GetNextLine(); !inFile.Eof(); pseudoLine=inFile.GetNextLine())
    {
        pseudoLine.Trim(false);
        pseudoLine.Trim(true);

        while (pseudoLine.Len() > 0)
        {
            parameterFound=false;
            parameter = extractWord(&pseudoLine);
            if (parameter == "n")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE)
                {
                    if (assInput.parameterValue[parameterNumber] < 0 || assInput.parameterValue[parameterNumber] > 0xf)
                        errorValue = ERROR_4BIT;
                    else
                    {
                        parameterFound = true;
                        nValue = assInput.parameterValue[parameterNumber];
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_HEX;
            }
            if (parameter == "l")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE)
                {
                    if (assInput.parameterValue[parameterNumber] < 0 || assInput.parameterValue[parameterNumber] > 0xf)
                        errorValue = ERROR_4BIT;
                    else
                    {
                        parameterFound = true;
                        lValue = assInput.parameterValue[parameterNumber];
                        parameterNumber++;
                    }
                }
                else
                errorValue = ERROR_HEX;
            }
            if (parameter == "q")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE)
                {
                    if (assInput.parameterValue[parameterNumber] < 0 || assInput.parameterValue[parameterNumber] > 7)
                        errorValue = ERROR_SP;
                    else
                    {
                        parameterFound = true;
                        qValue = assInput.parameterValue[parameterNumber];
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_HEX;
            }
            if (parameter == "p")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE)
                {
                    if (assInput.parameterValue[parameterNumber] < 0 || assInput.parameterValue[parameterNumber] > 7)
                        errorValue = ERROR_SP;
                    else
                    {
                        parameterFound = true;
                        pValue = assInput.parameterValue[parameterNumber];
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_HEX;
            }
            if (parameter == "o")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE)
                {
                    if (assInput.parameterValue[parameterNumber] < 0 || assInput.parameterValue[parameterNumber] > 7)
                    errorValue = ERROR_SP;
                    else
                    {
                        parameterFound = true;
                        oValue = assInput.parameterValue[parameterNumber];
                        parameterNumber++;
                    }
                }
                else
                errorValue = ERROR_HEX;
            }
            if (parameter == "kk")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE)
                {
                    if (assInput.parameterValue[parameterNumber] < 0 || assInput.parameterValue[parameterNumber] > 0xff)
                        errorValue = ERROR_8BIT;
                    else
                    {
                        parameterFound = true;
                        kkValue = assInput.parameterValue[parameterNumber];
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_HEX;
            }
            if (parameter == "vv")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE)
                {
                    if (assInput.parameterValue[parameterNumber] < 0 || assInput.parameterValue[parameterNumber] > 0xff)
                        errorValue = ERROR_8BIT;
                    else
                    {
                        parameterFound = true;
                        vvValue = assInput.parameterValue[parameterNumber];
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_HEX;
            }
            if (parameter == "ww")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE)
                {
                    if (assInput.parameterValue[parameterNumber] < 0 || assInput.parameterValue[parameterNumber] > 0xff)
                        errorValue = ERROR_8BIT;
                    else
                    {
                        parameterFound = true;
                        wwValue = assInput.parameterValue[parameterNumber];
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_HEX;
            }
            if (parameter.Left(5) == "c-hex")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE)
                {
                    lowStr = parameter.Mid(6,4);
                    highStr = parameter.Mid(11.4);
                    
                    lowStr.ToLong(&low, 16);
                    highStr.ToLong(&high, 16);
                    
                    if (assInput.parameterValue[parameterNumber] < low || assInput.parameterValue[parameterNumber] > high)
                        errorValue = ERROR_CARDTRAN_ADDRESS;
                    else
                    {
                        parameterFound = true;
                        ddValue = getCardtranAddress(assInput.parameterValue[parameterNumber]);
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_HEX;
            }
            if (parameter.Left(3) == "hex")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE)
                {
                    lowStr = parameter.Mid(4,4);
                    highStr = parameter.Mid(9.4);

                    lowStr.ToLong(&low, 16);
                    highStr.ToLong(&high, 16);

                    if (pseudoType_ == "AMVBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020")
                    {
                        if (high == 0xff)
                        {
                            if ( ((dirAssAddress_ & 0xff00) == (assInput.parameterValue[parameterNumber] & 0xff00))
                             || (assInput.parameterValue[parameterNumber] <=0xff) )
                            {
                                parameterFound = true;
                                hexValue = assInput.parameterValue[parameterNumber];
                                parameterNumber++;
                            }
                        }
                        else
                        {
                            if (assInput.parameterValue[parameterNumber] < low || assInput.parameterValue[parameterNumber] > high)
                                errorValue = ERROR_INCORR_ADDRESS;
                            else
                            {
                                parameterFound = true;
                                hexValue = assInput.parameterValue[parameterNumber];
                                parameterNumber++;
                            }
                        }
                    }
                    else
                    {
                        if (assInput.parameterValue[parameterNumber] < low || assInput.parameterValue[parameterNumber] > high)
                            errorValue = ERROR_INCORR_ADDRESS;
                        else
                        {
                            parameterFound = true;
                            hexValue = assInput.parameterValue[parameterNumber];
                            parameterNumber++;
                        }
                    }
                }
                else
                    errorValue = ERROR_HEX;
            }
            if (parameter.Left(5) == "s-hex")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE)
                {
                    lowStr = parameter.Mid(6,4);
                    highStr = parameter.Mid(11.4);
                    
                    lowStr.ToLong(&low, 16);
                    highStr.ToLong(&high, 16);
                    
                    if (pseudoType_ == "SUPERCHIP")
                        high += 0x600;

                    if (assInput.parameterValue[parameterNumber] < low || assInput.parameterValue[parameterNumber] > high)
                        errorValue = ERROR_INCORR_ADDRESS;
                    else
                    {
                        parameterFound = true;
                        hexValue = assInput.parameterValue[parameterNumber];
                        if (pseudoType_ == "SUPERCHIP")
                        {
                            if (hexValue >= 0x200)
                            {
                                hexValue -= 0x600;
                                if (hexValue < 0)
                                    hexValue = 0;
                            }
                        }
                        else
                        {
                            if (hexValue >= 0xc00)
                                hexValue = hexValue & 0xfbff;
                            if (hexValue >= 0x300)
                                hexValue -= 0x100;
                        }
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_HEX;
            }
            if (parameter.Left(5) == "c-mem")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE_MEM)
                {
                    lowStr = parameter.Mid(6,4);
                    highStr = parameter.Mid(11.4);
                    
                    lowStr.ToLong(&low, 16);
                    highStr.ToLong(&high, 16);
                    
                    if (assInput.parameterValue[parameterNumber] < low || assInput.parameterValue[parameterNumber] > high)
                        errorValue = ERROR_CARDTRAN_ADDRESS;
                    else
                    {
                        parameterFound = true;
                        ddValue = getCardtranAddress(assInput.parameterValue[parameterNumber]);
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_HEX_MEM;
            }
            if (parameter.Left(3) == "mem")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE_MEM)
                {
                    lowStr = parameter.Mid(4,4);
                    highStr = parameter.Mid(9.4);

                    lowStr.ToLong(&low, 16);
                    highStr.ToLong(&high, 16);

                    if (assInput.parameterValue[parameterNumber] < low || assInput.parameterValue[parameterNumber] > high)
                        errorValue = ERROR_INCORR_ADDRESS;
                    else
                    {
                        hexValue = assInput.parameterValue[parameterNumber];
                        parameterFound = true;
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_HEX_MEM;
            }
            if (parameter.Left(3) == "mkk")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE_MEM)
                {
                    if (assInput.parameterValue[parameterNumber] > 0xff)
                        errorValue = ERROR_INCORR_ADDRESS;
                    else
                    {
                        kkValue = assInput.parameterValue[parameterNumber];
                        parameterFound = true;
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_HEX_MEM;
            }
            if (parameter.Left(3) == "mvv")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE_MEM)
                {
                    if (assInput.parameterValue[parameterNumber] > 0xff)
                        errorValue = ERROR_INCORR_ADDRESS;
                    else
                    {
                        vvValue = assInput.parameterValue[parameterNumber];
                        parameterFound = true;
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_HEX_MEM;
            }
            if (parameter.Left(3) == "mww")
            {
                if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE_MEM)
                {
                    if (assInput.parameterValue[parameterNumber] > 0xff)
                        errorValue = ERROR_INCORR_ADDRESS;
                    else
                    {
                        wwValue = assInput.parameterValue[parameterNumber];
                        parameterFound = true;
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_HEX_MEM;
            }
            if (parameter == "Vx")
            {
                if (assInput.parameterType[parameterNumber] == CHIP8_VX)
                {
                    parameterFound = true;
                    registerX = assInput.parameterValue[parameterNumber];
                    parameterNumber++;
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
            if (parameter == "Vy")
            {
                if (assInput.parameterType[parameterNumber] == CHIP8_VX)
                {
                    parameterFound = true;
                    registerY = assInput.parameterValue[parameterNumber];
                    parameterNumber++;
                }
                else
                errorValue = ERROR_REG_EXP;
            }
            if (parameter == "Vz")
            {
                if (assInput.parameterType[parameterNumber] == CHIP8_VX)
                {
                    parameterFound = true;
                    registerZ = assInput.parameterValue[parameterNumber];
                    parameterNumber++;
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
            if (parameter == "[Vx]")
            {
                if (assInput.parameterType[parameterNumber] == CHIP8_VX_MEM)
                {
                    parameterFound = true;
                    registerX = assInput.parameterValue[parameterNumber];
                    parameterNumber++;
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
            if (parameter == "[Vx.1]")
            {
                if (assInput.parameterType[parameterNumber] == CHIP8_VX_MEM_1)
                {
                    parameterFound = true;
                    registerX = assInput.parameterValue[parameterNumber];
                    parameterNumber++;
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
            if (parameter == "[Vy]")
            {
                if (assInput.parameterType[parameterNumber] == CHIP8_VX_MEM)
                {
                    parameterFound = true;
                    registerY = assInput.parameterValue[parameterNumber];
                    parameterNumber++;
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
            if (parameter == "[Vy.1]")
            {
                if (assInput.parameterType[parameterNumber] == CHIP8_VX_MEM_1)
                {
                    parameterFound = true;
                    registerY = assInput.parameterValue[parameterNumber];
                    parameterNumber++;
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
            if (parameter == "Vx1")
            {
                if (assInput.parameterType[parameterNumber] == CHIP8_VX)
                {
                    if (assInput.parameterValue[parameterNumber] == registerX+1)
                    {
                        parameterFound = true;
                        parameterNumber++;
                    }
                    else
                        errorValue = ERROR_INCORRECT_REG;
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
            if (parameter == "Vy1")
            {
                if (assInput.parameterType[parameterNumber] == CHIP8_VX)
                {
                    if (assInput.parameterValue[parameterNumber] == registerY+1)
                    {
                        parameterFound = true;
                        parameterNumber++;
                    }
                    else
                        errorValue = ERROR_INCORRECT_REG;
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
            if (parameter.Left(1) == "V" && parameter.Len() == 2)
            {
                regNumberStr = parameter.Right(1);
                if (regNumberStr.ToLong(&regNumber, 16))
                {
                    if (assInput.parameterType[parameterNumber] == CHIP8_VX)
                    {
                        if (assInput.parameterValue[parameterNumber] != regNumber)
                            errorValue = ERROR_INCORRECT_REG;
                        else
                        {
                            parameterFound = true;
                            parameterNumber++;
                        }
                    }
                    else
                        errorValue = ERROR_REG_EXP;
                }
            }
            if (parameter.Left(1) == "R" && parameter.Len() == 2)
            {
                regNumberStr = parameter.Right(1);
                if (regNumberStr.ToLong(&regNumber, 16))
                {
                    if (assInput.parameterType[parameterNumber] == ASS_REG)
                    {
                        if (assInput.parameterValue[parameterNumber] != regNumber || assInput.parameterValue[parameterNumber] > 0x7)
                            errorValue = ERROR_INCORRECT_REG;
                        else
                        {
                            parameterFound = true;
                            parameterNumber++;
                        }
                    }
                    else
                        errorValue = ERROR_REG_EXP;
                }
            }
            if (parameter.Len() == 1)
            {
                regNumberStr = parameter.Right(1);
                if (regNumberStr.ToLong(&regNumber, 16))
                {
                    if (assInput.parameterType[parameterNumber] == ASS_HEX_VALUE)
                    {
                        if (assInput.parameterValue[parameterNumber] != regNumber)
                            errorValue = ERROR_SYNTAX;
                        else
                        {
                            parameterFound = true;
                            parameterNumber++;
                        }
                    }
                    else
                        errorValue = ERROR_SYNTAX;
                }
            }
            if (parameter == "Rx")
            {
                if (assInput.parameterType[parameterNumber] == ASS_REG)
                {
                    if (assInput.parameterValue[parameterNumber] > 0x7)
                        errorValue = ERROR_INCORRECT_REG;
                    else
                    {
                        parameterFound = true;
                        registerX = assInput.parameterValue[parameterNumber]*2;
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
            if (parameter == "Ry")
            {
                if (assInput.parameterType[parameterNumber] == ASS_REG)
                {
                    if (assInput.parameterValue[parameterNumber] > 0x7)
                        errorValue = ERROR_INCORRECT_REG;
                    else
                    {
                        parameterFound = true;
                        registerY = assInput.parameterValue[parameterNumber]*2;
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
            if (parameter == "Rz")
            {
                if (assInput.parameterType[parameterNumber] == ASS_REG)
                {
                    if (assInput.parameterValue[parameterNumber] > 0x7)
                        errorValue = ERROR_INCORRECT_REG;
                    else
                    {
                        parameterFound = true;
                        registerZ = assInput.parameterValue[parameterNumber]*2;
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
            if (parameter == "Rr")
            {
                if (assInput.parameterType[parameterNumber] == ASS_REG)
                {
                    if (assInput.parameterValue[parameterNumber] > 0x7)
                        errorValue = ERROR_INCORRECT_REG;
                    else
                    {
                        parameterFound = true;
                        registerR = assInput.parameterValue[parameterNumber]*2;
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
            if (parameter == "[Ry]")
            {
                if (assInput.parameterType[parameterNumber] == AM_REG_MEM)
                {
                    if (assInput.parameterValue[parameterNumber] > 0x7)
                        errorValue = ERROR_SYNTAX;
                    else
                    {
                        parameterFound = true;
                        registerY = assInput.parameterValue[parameterNumber]*2;
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
            if (parameter == "[Rz]")
            {
                if (assInput.parameterType[parameterNumber] == AM_REG_MEM)
                {
                    if (assInput.parameterValue[parameterNumber] > 0x7)
                        errorValue = ERROR_SYNTAX;
                    else
                    {
                        parameterFound = true;
                        registerZ = assInput.parameterValue[parameterNumber]*2;
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_REG_EXP;
            }
           if (parameter == "RA")
            {
                if (assInput.parameterType[parameterNumber] == ASS_REG)
                {
                    if (assInput.parameterValue[parameterNumber] != 0xA)
                        errorValue = ERROR_SYNTAX;
                    else
                    {
                        parameterFound = true;
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_SYNTAX;
            }
            if (parameter == "RB")
            {
                if (assInput.parameterType[parameterNumber] == ASS_REG)
                {
                    if (assInput.parameterValue[parameterNumber] != 0xB)
                        errorValue = ERROR_SYNTAX;
                    else
                    {
                        parameterFound = true;
                        parameterNumber++;
                    }
                }
                else
                    errorValue = ERROR_SYNTAX;
            }
            if (parameter == assInput.parameterString[parameterNumber] && assInput.parameterType[parameterNumber] == ASS_STRING)
            {
                parameterFound = true;
                parameterNumber++;
            }
            if (parameter == "empty")
            {
                if (assInput.parameterString[parameterNumber] == "" && assInput.parameterType[parameterNumber] == ASS_STRING)
                {
                    parameterFound = true;
                    parameterNumber++;
                }
            }
            if (parameter == ',')
            {
                if (assInput.seperator[parameterNumber-1] == ",")
                    parameterFound = true;
                else
                    errorValue = ERROR_COMMA;
            }
            if (parameter == '+')
            {
                if (assInput.seperator[parameterNumber-1] == "+")
                    parameterFound = true;
                else
                    errorValue = ERROR_PLUS;
            }
            if (parameter == '-')
            {
                if (assInput.seperator[parameterNumber-1] == "-")
                    parameterFound = true;
                else
                    errorValue = ERROR_MINUS;
            }
            if (parameter == '&')
            {
                if (assInput.seperator[parameterNumber-1] == "&")
                    parameterFound = true;
                else
                    errorValue = ERROR_AMP;
            }
            if (parameter == '=')
            {
                if (assInput.seperator[parameterNumber-1] == "=")
                    parameterFound = true;
                else
                    errorValue = ERROR_EQUAL;
            }
            if (parameter == '/')
            {
                if (assInput.seperator[parameterNumber-1] == "/")
                    parameterFound = true;
                else
                    errorValue = ERROR_SLASH;
            }
            if (parameter.Left(3) == "det")
                parameterFound = true;
            if (parameter.Left(4) == "code")
            {
                if (parameterNumber == assInput.numberOfParameters)
                {
                    long command;
                    wxString tempStr;
                    wxString commandStr = parameter.Mid(5,1);
                    commandStr.ToLong(&command, 16);
                    
                    commandStr = parameter.Mid(6,7);
                    if (commandStr.Len() < 1)
                        return ERROR_SYNTAX_FILE;
                    
                    int pseudoLength = (int)(commandStr.Len() + 1)/2;
                    
                    *pseudoCode = (int32_t)command << (12+((pseudoLength-2)*8));
                    for (int i=0; i<((pseudoLength*2)-1); i++)
                    {
                        if (commandStr.GetChar(i) == 'a' || commandStr.GetChar(i) == 'j' || commandStr.GetChar(i) == 'b')
                        {
                            if (pseudoType_ == "AMVBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020")
                            {
                                if (i == 1)
                                {
                                    if (pseudoLength == 3)
                                        *pseudoCode |= hexValue & 0xffff;
                                    else
                                        *pseudoCode |= hexValue & 0xff;
                                }
                                else
                                    *pseudoCode |= hexValue & 0xff;
                            }
                            else
                            {
                                if (i == 1)
                                    *pseudoCode |= hexValue & 0xff;
                                else
                                    *pseudoCode |= hexValue & 0xfff;
                            }
                            return pseudoLength;
                        }
                        if (commandStr.GetChar(i) == 's')
                        {
                            if (i == 1)
                                *pseudoCode |= hexValue & 0xff;
                            else
                                *pseudoCode |= hexValue & 0xfff;
                            return pseudoLength;
                        }
                        if (commandStr.GetChar(i) == 'k')
                        {
                            if (pseudoType_ == "AMVBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020")
                            {
                                i++;
                                *pseudoCode |= ( (kkValue & 0xff) << ((pseudoLength-2)*8) );
                            }
                            else
                            {
                                *pseudoCode |= kkValue & 0xff;
                                return pseudoLength;
                            }
                        }
                        if (commandStr.GetChar(i) == 'v')
                        {
                            i++;
                            *pseudoCode |= ( (vvValue & 0xff) << ((pseudoLength-3)*8) );
                        }
                        if (commandStr.GetChar(i) == 'w')
                        {
                            i++;
                            *pseudoCode |=  wwValue & 0xff;
                        }
                        if (commandStr.GetChar(i) == 'd')
                        {
                            *pseudoCode |= ddValue & 0xff;
                            return pseudoLength;
                        }
                        if (commandStr.GetChar(i) == 'n')
                        {
                            tempValue = nValue << ((2-i)*4);
                            *pseudoCode |= tempValue;
                        }
                        if (commandStr.GetChar(i) == 'l')
                        {
                            tempValue = lValue << ((2-i)*4);
                            *pseudoCode |= tempValue;
                        }
                        if (commandStr.GetChar(i) == 'q')
                        {
                            tempValue = qValue << ((2-i)*4);
                            *pseudoCode |= tempValue;
                        }
                        if (commandStr.GetChar(i) == 'p')
                        {
                            tempValue = pValue << ((2-i)*4);
                            *pseudoCode |= tempValue;
                        }
                        if (commandStr.GetChar(i) == 'o')
                        {
                            tempValue = (oValue+8) << ((2-i)*4);
                            *pseudoCode |= tempValue;
                        }
                        if (commandStr.GetChar(i) == 'r')
                        {
                            *pseudoCode |= registerR;
                        }
                        if (commandStr.GetChar(i) == 'x')
                        {
                            tempValue = registerX << ((2-i)*4+((pseudoLength-2)*8));
                            *pseudoCode |= tempValue;
                        }
                        if (commandStr.GetChar(i) == 'y')
                        {
                            tempValue = registerY << ((2-i)*4+((pseudoLength-2)*8));
                            *pseudoCode |= tempValue;
                        }
                        if (commandStr.GetChar(i) == 'z')
                        {
                            tempValue = registerZ << ((2-i)*4+((pseudoLength-2)*8));
                            *pseudoCode |= tempValue;
                        }
                        if (commandStr.GetChar(i) >= '0' && commandStr.GetChar(i) <= '9')
                        {
                            tempValue = commandStr.GetChar(i) - '0';
                            tempValue = tempValue << ((2-i)*4+((pseudoLength-2)*8));
                            *pseudoCode |= tempValue;
                        }
                        if (commandStr.GetChar(i) >= 'A' && commandStr.GetChar(i) <= 'F')
                        {
                            tempValue = commandStr.GetChar(i) - 'A' + 10;
                            tempValue = tempValue << ((2-i)*4+((pseudoLength-2)*8));
                            *pseudoCode |= tempValue;
                        }
                    }
                    return pseudoLength;
                }
                else
                    errorValue = ERROR_PAR;
            }
            if (!parameterFound)
            {
                if (!inFile.Eof())
                {
                    pseudoLine=inFile.GetNextLine();
                    pseudoLine.Trim(false);
                    pseudoLine.Trim(true);

                    if (pseudoLine.GetChar(0) != '/')
                    {
                        parameterNumber = 0;
                    }
                    else
                        return errorValue;
                }
                else
                    return errorValue;
            }
        }
    }
    return ERROR_SYNTAX;
}

Byte DebugWindow::getCardtranAddress(long address)
{
    int value = (int)(address-0x200)/2;
    int highNibble = value / 10;
    int lowNibble = value - (highNibble * 10);
    return highNibble * 16 + lowNibble;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0x74; 
        return 1; 
    }
    if (assInput.command == "ADCI")
    {
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        {
            if (cpuType_ == SYSTEM00)
            {
                ret = getRegisterNumber(assInput, &registerNumber, b7, allowX);
                *b1 = registerNumber;
                return 1;
            }
            else
                return ERROR_PAR;
        }
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
        if (cpuType_ == SYSTEM00)
        {
            *b1 = 0x68;
            return 1;
        }
        
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0x60; 
        return 1; 
    }
    if (assInput.command == "LBDF")
    {
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
    if (assInput.command == "LDRL")
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

            ret = getWordPar2(assInput, b5, b2, allowX);
            if (ret != 4)
                return ret;

            *b1 = 0xF8;
            *b3 = 0xA0 | registerNumber;
            *b6 = 0xB0 | registerNumber;
            return MEM_TYPE_OPCODE_LDRL_SLOT;
        }
        else
        {
            if (assInput.numberOfParameters != 2)
                return ERROR_REGAND16;

            ret = getRegisterNumber(assInput, &registerNumber, b7, allowX);
            if (ret != 1)
                return ret;

            ret = getWordPar2(assInput, b5, b2, allowX);
            if (ret != 4)
                return ret;

            *b1 = 0xF8;
            *b3 = 0xA0 | registerNumber;
            *b4 = 0xF8;
            *b6 = 0xB0 | registerNumber;
            return MEM_TYPE_OPCODE_LDRL;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0x72; 
        return 1; 
    }
    if (assInput.command == "LSDF") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0xcf; 
        return 1; 
    }
    if (assInput.command == "LSIE") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0xcc; 
        return 1; 
    }
    if (assInput.command == "LSKP") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0xc8; 
        return MEM_TYPE_OPCODE_LSKP; 
    }
    if (assInput.command == "NLBR") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0xc8; 
        return 1; 
    }
    if (assInput.command == "LSNF") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0xc7; 
        return 1; 
    }
    if (assInput.command == "LSNQ") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0xc5; 
        return 1; 
    }
    if (assInput.command == "LSNZ") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0xc6; 
        return 1; 
    }
    if (assInput.command == "LSQ") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0xcd; 
        return 1; 
    }
    if (assInput.command == "LSZ") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0xce; 
        return 1; 
    }
    if (assInput.command == "MARK") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0x79; 
        return 1; 
    }
    if (assInput.command == "NOP") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)
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
    if (assInput.command == "PNI")
    {
        if (cpuType_ != SYSTEM00)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 1)
            return ERROR_PAR;

        ret =  getRegisterNumber(assInput, &registerNumber, b7, allowX);
        *b1 = 0xc0 | registerNumber;
        return ret;
    }
    if (assInput.command == "REQ") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0x75; 
        return 1; 
    }
    if (assInput.command == "SDBI")
    {
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_TEMP_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_TEMP_PAR;
        *b1 = 0xfe; 
        return 1; 
    }
    if (assInput.command == "SHLC") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0x7e; 
        return 1; 
    }
    if (assInput.command == "RSHL") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0x76; 
        return 1; 
    }
    if (assInput.command == "RSHR") 
    { 
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (assInput.numberOfParameters > 0)
            return ERROR_PAR;
        *b1 = 0x77; 
        return 1; 
    }
    if (assInput.command == "SMBI")
    {
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "DTC")
    {
        *b1 = 0x68;
        *b2 = 0x1;
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "SPM2")
    {
        *b1 = 0x68;
        *b2 = 0x2;
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "SCM2")
    {
        *b1 = 0x68;
        *b2 = 0x3;
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "SPM1")
    {
        *b1 = 0x68;
        *b2 = 0x4;
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "SCM1")
    {
        *b1 = 0x68;
        *b2 = 0x5;
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "LDC")
    {
        *b1 = 0x68;
        *b2 = 0x6;
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "STM")
    {
        *b1 = 0x68;
        *b2 = 0x7;
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "GEC")
    {
        *b1 = 0x68;
        *b2 = 0x8;
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "ETQ")
    {
        *b1 = 0x68;
        *b2 = 0x9;
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "XIE")
    {
        *b1 = 0x68;
        *b2 = 0xa;
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "XID")
    {
        *b1 = 0x68;
        *b2 = 0xb;
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "CIE")
    {
        *b1 = 0x68;
        *b2 = 0xc;
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "CID")
    {
        *b1 = 0x68;
        *b2 = 0xd;
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        return 2;
    }
    if (assInput.command == "DBNZ")
    {
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;

        ret = getByte(assInput, b3, allowX);
        if (ret > ERROR_START)  return ret;

        *b1 = 0x68;
        *b2 = 0x3e;
        return 3;
    }
    if (assInput.command == "BXI")
    {
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;

        ret = getByte(assInput, b3, allowX);
        if (ret > ERROR_START)  return ret;

        *b1 = 0x68;
        *b2 = 0x3f;
        return 3;
    }
    if (assInput.command == "RLXA")
    {
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        if (cpuType_ == CPU1804)  return ERROR_CPU_1804;
        *b1 = 0x68;
        *b2 = 0x74;
        return 2;
    }
    if (assInput.command == "DSAV")
    {
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        if (cpuType_ == CPU1804)  return ERROR_CPU_1804;
        *b1 = 0x68;
        *b2 = 0x76;
        return 2;
    }
    if (assInput.command == "DSMB")
    {
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
        if (cpuType_ == CPU1802)  return ERROR_CPU_1802;
        if (cpuType_ == CPU1804)  return ERROR_CPU_1804;
        *b1 = 0x68;
        *b2 = 0x77;
        return 2;
    }
    if (assInput.command == "DACI")
    {
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
        if (cpuType_ <= CPU1801)  return ERROR_CPU_1801;
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
    if (buffer.Left(4)== "[RA]" || buffer.Left(4)== "[RB]" || buffer.Left(3)== "[A]" || buffer.Left(3)== "[B]" ||
        buffer.Left(4)== "RB.0" || buffer.Left(4)== "RB.1" || buffer.Left(4)== "RA.0" || buffer.Left(4)== "RA.1" ||
        buffer.Left(6)== "[V0V1]" || buffer.Left(4)== "V0V1" || buffer.Left(6)== "[V2V3]" || buffer.Left(4)== "V2V3" ||
        buffer.Left(2)== "[I" || buffer.Left(3)== "V9]"|| buffer.Left(3)== "[I]" || buffer.Left(4)== "[>I]" ||
        buffer.Left(6)== "SWITCH" || buffer.Left(4)== "SWAP" || buffer.Left(2)== "ST" || buffer.Left(4)== "READ" ||
        buffer.Left(2)== "DR" || buffer.Left(3)== "RAM" || buffer.Left(6)== "SETCOL" || buffer.Left(4)== "SKSP" || buffer.Left(4)== "SYNC")
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
    if (buffer.Left(2) == "[V" && buffer.Right(3)== ".1]")
    {
        buffer = buffer.Mid(2, 1);
        if (!buffer.ToLong(value, 16))
        return ERROR_REG;
        if (*value < 0 || *value > 15)
        return ERROR_REG;
        *type = CHIP8_VX_MEM_1;
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
    if (buffer.Left(2) == "[R" && buffer.Right(1)== "]")
    {
        buffer = buffer.Mid(2, 1);
        if (!buffer.ToLong(value, 16))
            return ERROR_REG;
        if (*value < 0 || *value > 15)
            return ERROR_REG;
        *type = AM_REG_MEM;
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
    if (buffer == "B")  return TREG_B;
    if (buffer == "CH")  return TREG_CH;
    if (buffer == "CN")  return TREG_CNTR;
    if (buffer == "CNTR")  return TREG_CNTR;
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
//    int num = 0;
    long addr;
    Byte value;
    Word startAddress = 0, endAddress;
    wxFile outputFile;
    wxString fileName, memoryStr, number, strValue, line;

    endAddress = getAddressMask();
    
    switch (memoryDisplay_)
    {
        case CPU_MEMORY:
            memoryStr = "CPU Memory";
            fileName = "memorydump";
            
            addr = get16BitValue("DebugCopyStart");
            if (addr != -1)
                startAddress = addr;
            
            addr = (Word) get16BitValue("DebugCopyEnd");
            if (addr != -1)
                endAddress = addr;
        break;

        case CDP_1870_C:
            memoryStr = "CDP 1870 Character Ram";
            fileName = "cdp1870charramdump";
        break;
            
        case CDP_1870_COLOUR:
            memoryStr = "CDP 1870 Colour Ram";
            fileName = "cdp1870colourramdump";
        break;
            
        case CDP_1870_GRAPHIC:
            memoryStr = "CDP 1870 Graphic Ram";
            fileName = "cdp1870graphicramdump";
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

        case CDP_1862:
            memoryStr = "CDP 1862 Color Ram";
            fileName = "cdp1862colorramdump";
        break;

        case CDP_1864:
            memoryStr = "CDP 1864 Color Ram";
            fileName = "cdp1864colorramdump";
        break;

        case STUDIO_IV_COLOR:
            memoryStr = "Studio IV Color Ram";
            fileName = "studioivcolorramdump";
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
              
        case VIP2KSEQUENCER:
            memoryStr = "VIP2K Sequencer ROM";
            fileName = "vip2ksequencer";
        break;

        case RTCRAM:
            memoryStr = "RTC Ram";
            fileName = "rtcramdump";
        break;
    }

    fileName = wxFileSelector( "Select the " + memoryStr + " dump file to save",
                               debugDir_, fileName,
                               "",
                               wxString::Format
                              (
                                   "Binary File|*.bin;*.rom;*.ram;*.cos;*.c8;*.ch8;*.c8x;*.ch10;*.sc8|Intel Hex File|*.hex|Text File (*.txt)|*.txt"
                               ),
                               wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_OVERWRITE_PROMPT|wxFD_PREVIEW,
                               this
                              );

    if (!fileName || fileName.empty())
        return;

    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
//    wxString name = FullPath.GetName();
//    wxString path = FullPath.GetPath();
    wxString ext = FullPath.GetExt();

/*    while(wxFile::Exists(fileName))
    {
        num++;
        number.Printf("%d", num);
        fileName = path + pathSeparator_ + name + "." + number + "." + ext;
    }*/

    if (ext == "bin" || ext == "rom" || ext == "ram" || ext == "cos" || ext == "c8" || ext == "ch8" || ext == "c8x" || ext == "ch10")
    {
        outputFile.Create(fileName, true);
        if (memoryDisplay_ == VIP2KSEQUENCER)
        {
            for (long address = 0; address <= 0x7ff; address++)
            {
                value = p_Computer->readSequencerRom(address);
                outputFile.Write(&value, 1);
            }
        }
        else 
        {
            for (long address = startAddress; address <= endAddress; address++)
            {
                value = debugReadMem(address);
                outputFile.Write(&value, 1);
            }
        }
        outputFile.Close();
    }
    else
    {
        if (ext == "hex")
        {
            wxString byteStr;
            int checkSum;

            if (wxFile::Exists(fileName))
            {
                outputTextFile.Open(fileName);
                outputTextFile.Clear();
            }
            else
                outputTextFile.Create(fileName);
            addr = startAddress;

            while (addr <= endAddress)
            {
                line.Printf(":%02X%04X%02X", 0x10, (int)addr, 0x00);
                checkSum = 0x10+((addr>>8)&0xff)+(addr&0xff);
                for (int i = 0; i<16; i++)
                {
                    if (memoryDisplay_ == VIP2KSEQUENCER)
                    {
                        checkSum += p_Computer->readSequencerRom(addr);
                        byteStr.Printf("%02X", p_Computer->readSequencerRom(addr));
                    }
                    else
                    {
                        checkSum += debugReadMem(addr);
                        byteStr.Printf("%02X", debugReadMem(addr));
                    }
                    line += byteStr;
                    addr++;
                }
                checkSum = ((checkSum ^ 0xff) + 1) & 0xff;
                byteStr.Printf("%02X", checkSum);
                line += byteStr;
                outputTextFile.AddLine(line);
            }
            outputTextFile.AddLine(":00000001FF");
            outputTextFile.Write();
        }
        else
        {
            if (wxFile::Exists(fileName))
            {
                outputTextFile.Open(fileName);
                outputTextFile.Clear();
            }
            else
                outputTextFile.Create(fileName);
            addr = startAddress;
            if (memoryDisplay_ == VIP2KSEQUENCER)
            {
                while(addr <= 0x7ff)
                {
                    line.Printf("%04X:", (unsigned int)addr);
                    for (int i=0; i<16; i++)
                    {
                        strValue.Printf(" %02X", p_Computer->readSequencerRom(addr+i));
                        line = line + strValue;
                    }
                    outputTextFile.AddLine(line);
                    addr += 16;
                }
            }
            else
            {
                while(addr <= endAddress)
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
            }
            outputTextFile.Write();
            outputTextFile.Close();
        }
    }
}

void DebugWindow::onDegubPauseButton(wxCommandEvent&WXUNUSED(event))
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
        XRCCTRL(*this, "DebugStepButton", wxBitmapButton)->Enable(true);
    }
    else
    {
        if (p_Computer->getSteps() > 0)
        {
            XRCCTRL(*this, "DebugPauseButton", wxBitmapButton)->SetBitmapLabel(pauseOffBitmap);
            XRCCTRL(*this, "DebugStepButton", wxBitmapButton)->Enable(true);
        }
        else
        {
            XRCCTRL(*this, "DebugPauseButton", wxBitmapButton)->SetBitmapLabel(pauseOffBitmap);
            XRCCTRL(*this, "DebugStepButton", wxBitmapButton)->Enable(false);
        }
    }
    p_Main->eventUpdateTitle();
}

void DebugWindow::onDegugStepButton(wxCommandEvent&WXUNUSED(event))
{
    performStep_ = true;
}

void DebugWindow::onDebugRunButton(wxCommandEvent&WXUNUSED(event))
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
    if (trace_ || traceTrap_ || traceDma_ || traceInt_ || numberOfBreakPoints_ > 0 || numberOfTraps_ > 0 || numberOfTregs_ > 0)
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
//    wxSetWorkingDirectory (workingDir_);
//    int num = 0;
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
                               wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_OVERWRITE_PROMPT|wxFD_PREVIEW,
                               this
                              );

    if (!fileName || fileName.empty())
        return;

/*    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    wxString name = FullPath.GetName();
    wxString path = FullPath.GetPath();
    wxString ext = FullPath.GetExt();

    while(wxFile::Exists(fileName))
    {
        num++;
        number.Printf("%d", num);
        fileName = path + pathSeparator_ + name + "." + number + "." + ext;
    }*/
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
    {
        XRCCTRL(*this, "TrapWindowText", wxStaticText)->SetLabel("Instr. Trace:");
        XRCCTRL(*this, "TrapSet", wxButton)->SetLabel("Instr. Trace:");
    }
    else
    {
        XRCCTRL(*this, "TrapWindowText", wxStaticText)->SetLabel("Instr. Traps:");
        XRCCTRL(*this, "TrapSet", wxButton)->SetLabel("Instr. Trap:");
    }
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
                if (cpuType_ <= CPU1801)
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
            if (opCode[command] == 0x61 || opCode[command] == 0x69)
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

void DebugWindow::B(wxCommandEvent&WXUNUSED(event))
{
    long value = get8BitValue("B");
    if (value == -1)  return;
    
    p_Computer->setRegisterB(value);
}

void DebugWindow::CH(wxCommandEvent&WXUNUSED(event))
{
    long value = get8BitValue("CH");
    if (value == -1)  return;
    
    p_Computer->setCounterJamValue(value);
}

void DebugWindow::CNTR(wxCommandEvent&WXUNUSED(event))
{
    long value = get8BitValue("CNTR");
    if (value == -1)  return;
    
    p_Computer->setCounterTimer(value);
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

void DebugWindow::CIE(wxCommandEvent&WXUNUSED(event))
{
    long value = getBitValue("CIE");
    if (value == -1)  return;
    
    p_Computer->setCounterInterruptEnable(value);
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

    if (pseudoType_ == "STIV")
    {
        p_Computer->writeMemDebug(0x27f6, (value&0xff00)>>8, false);
        p_Computer->writeMemDebug(0x27f7, value&0xff, false);
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
        p_Computer->writeMemDebug(p_Computer->getChip8baseVar() + number, value, false);
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

    if (computerRunning_ && computerConfiguration.vis1870Configuration.defined)
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

    if (computerRunning_ && computerConfiguration.vis1870Configuration.defined)
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

    if (computerRunning_ && computerConfiguration.vis1870Configuration.defined)
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

    if (computerRunning_ && computerConfiguration.vis1870Configuration.defined)
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
    if (!computerRunning_ || (profilerCounter_ == PROFILER_OFF && selectedTab_ == PROFILERTAB))
        return;

    int bitmapWidth = assWidth_;
    uint64_t executed;
    
#if defined(__WXMAC__)
    wxFont exactFont(fontSize_+2, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont exactFontBold(fontSize_+2, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
#else
    wxFont exactFont(fontSize_+1, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont exactFontBold(fontSize_+1, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
#endif
    
    int numberOfDebugLines = numberOfDebugLines_;
    
    switch (selectedTab_)
    {
        case DIRECTASSTAB:
            bitmapWidth = assWidth_;
            dcAss.SelectObject(*assBmp);
        break;
            
        case PROFILERTAB:
            bitmapWidth = profilerWidth_;
            dcAss.SelectObject(*profilerBmp);
            numberOfDebugLines -= PROFILER_OFFSET;
        break;

        default:
            bitmapWidth = assWidth_;
            dcAss.SelectObject(*assBmp);
        break;
    }

//    dcAss.SetPen(wxPen(wxColour(windowInfo.red, windowInfo.green, windowInfo.blue)));
//    dcAss.SetBrush(wxBrush(wxColour(windowInfo.red, windowInfo.green, windowInfo.blue)));
//    dcAss.SetTextBackground(wxColour(windowInfo.red, windowInfo.green, windowInfo.blue));
    dcAss.SetPen(wxPen(guiBackGround_));
    dcAss.SetBrush(wxBrush(guiBackGround_));
    dcAss.SetTextBackground(guiBackGround_);
    dcAss.DrawRectangle(0, 0, bitmapWidth, numberOfDebugLines*lineSpace_+4);

    if (selectedTab_ == DIRECTASSTAB)
    {
        if (dirAssStart_ == dirAssEnd_)
        {
            for (int i=0; i<EDIT_ROW; i++)
                assSpinUp();
        }
    }

    Word address = dirAssStart_;

    wxString code, text;
    int count;
    wxString line2;
    wxString printBufferAddress, printBufferOpcode;

    for (int line=0; line <numberOfDebugLines; line ++)
    {
        wxColourDatabase colour;
        if (line == EDIT_ROW && selectedTab_ == DIRECTASSTAB)
        {
            dcAss.SetFont(exactFontBold);
            dirAssAddress_ = address;
            dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
            dcAss.DrawText(">", 1, 1+line*lineSpace_);
            dcAss.DrawText("<", bitmapWidth-9, 1+EDIT_ROW*lineSpace_);
        }
        else
            dcAss.SetFont(exactFont);
            
        dcAss.SetTextForeground(guiTextColour[GUI_COL_GREY]);
        for (int i=0; i<lastRange_; i++)
        {
            if (address >= dirAssProgramStartVector[i] && address <= dirAssProgramEndVector[i])
            {
                if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
                {
                    if (address >= computerConfiguration.slotConfiguration.start && address <= computerConfiguration.slotConfiguration.end)
                    {
                        if (p_Computer->getOutValue(computerConfiguration.slotConfiguration.output) ==  dirAssSlotVector[i])
                            dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
                    }
                    else
                        dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
                }
                else
                {
                    if (computerConfiguration.emsMemoryConfiguration.size() != 0)
                    {
                        for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = computerConfiguration.emsMemoryConfiguration.begin (); emsConfig != computerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
                        {
                            for (std::vector<WordRange>::iterator range = emsConfig->range.begin (); range != emsConfig->range.end (); ++range)
                            {
                                if (address >= range->start && address <= range->end)
                                {
                                    if (p_Computer->getEmsPage(emsConfig) == dirAssSlotVector[i])
                                        dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
                                }
                                else
                                    dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
                            }
                        }
                    }
                    else
                        dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
                }
            }
        }

        switch(p_Computer->readMemLabelType(address))
        {
            case LABEL_TYPE_BRANCH:
                dcAss.SetTextForeground(guiTextColour[GUI_COL_ORANGE]);
            break;
            case LABEL_TYPE_JUMP:
                dcAss.SetTextForeground(guiTextColour[GUI_COL_ORANGE]);
            break;
            case LABEL_TYPE_SUB:
                dcAss.SetTextForeground(guiTextColour[GUI_COL_PINK]);
            break;
        }
        
        Byte memType = p_Computer->readMemDataType(address, &executed);
        int numberOfSpaces = 0;
        Byte executedColor = (Byte)(log((double)executed)*5);
        
        wxString executedStr = "";
        
        if (profilerType_ == PROFILERTYPELOG)
        {
            if (executedColor > 0)
                executedStr.Printf("%3u", executedColor);
        }
        else
        {
            if (executed > 0)
                executedStr = wxNumberFormatter::ToString((double)executed, 0);
            numberOfSpaces = 26 - (int)executedStr.Len();
        }

        Byte tempByte;
        Byte command;
        
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
                switch (jumpCorrection[p_Computer->readMemDebug(address)])
                {
                    case 1:
                        text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
                        dcAss.DrawText(text.Left(5), 1+charWidth_, 1+line*lineSpace_);
                        setProfileColor(executedColor);
                        dcAss.DrawText(text.Mid(6,2), 1+charWidth_*7, 1+line*lineSpace_);
                        dcAss.DrawText(text.Mid(18,5), 1+charWidth_*19, 1+line*lineSpace_);
                        dcAss.SetTextForeground(guiTextColour[GUI_COL_ORANGE]);
                        dcAss.DrawText(text.Mid(9,2), 1+charWidth_*10, 1+line*lineSpace_);
                        dcAss.DrawText(text.Right(text.Len()-23), 1+charWidth_*24, 1+line*lineSpace_);
                    break;
                                        
                    case 2:
                        text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
                        dcAss.DrawText(text.Left(5), 1+charWidth_, 1+line*lineSpace_);
                        setProfileColor(executedColor);
                        dcAss.DrawText(text.Mid(6,2), 1+charWidth_*7, 1+line*lineSpace_);
                        dcAss.DrawText(text.Mid(18,5), 1+charWidth_*19, 1+line*lineSpace_);
                        dcAss.SetTextForeground(guiTextColour[GUI_COL_ORANGE]);
                        dcAss.DrawText(text.Mid(9,2), 1+charWidth_*10, 1+line*lineSpace_);
                        dcAss.DrawText(text.Mid(12,2), 1+charWidth_*13, 1+line*lineSpace_);
                        dcAss.DrawText(text.Right(text.Len()-23), 1+charWidth_*24, 1+line*lineSpace_);
                    break;
                
                    case 3:
                        tempByte = p_Computer->readMemDebug(address+1);
                        if ((tempByte&0xf0) == 0x80 || (tempByte&0xf0) == 0x20)
                        {
                            text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
                            dcAss.DrawText(text.Left(5), 1+charWidth_, 1+line*lineSpace_);
                            setProfileColor(executedColor);
                            dcAss.DrawText(text.Mid(6,2), 1+charWidth_*7, 1+line*lineSpace_);
                            dcAss.DrawText(text.Mid(9,2), 1+charWidth_*10, 1+line*lineSpace_);
                            dcAss.DrawText(text.Mid(18,5), 1+charWidth_*19, 1+line*lineSpace_);
                            dcAss.DrawText(text.Mid(23,3), 1+charWidth_*24, 1+line*lineSpace_);
                            if ((tempByte&0xf0) == 0x80)
                                dcAss.SetTextForeground(guiTextColour[GUI_COL_PINK]);
                            else
                                dcAss.SetTextForeground(guiTextColour[GUI_COL_ORANGE]);
                            dcAss.DrawText(text.Mid(12,2), 1+charWidth_*13, 1+line*lineSpace_);
                            dcAss.DrawText(text.Mid(15,2), 1+charWidth_*16, 1+line*lineSpace_);
                            dcAss.DrawText(text.Right(4), 1+charWidth_*27, 1+line*lineSpace_);
                        }
                        else
                        {
                            if (tempByte == 0x3e || tempByte == 0x3f)
                            {
                                text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
                                dcAss.DrawText(text.Left(5), 1+charWidth_, 1+line*lineSpace_);
                                setProfileColor(executedColor);
                                dcAss.DrawText(text.Mid(6,2), 1+charWidth_*7, 1+line*lineSpace_);
                                dcAss.DrawText(text.Mid(9,2), 1+charWidth_*10, 1+line*lineSpace_);
                                dcAss.DrawText(text.Mid(15,2), 1+charWidth_*16, 1+line*lineSpace_);
                                dcAss.DrawText(text.Mid(18,5), 1+charWidth_*19, 1+line*lineSpace_);
                                dcAss.SetTextForeground(guiTextColour[GUI_COL_ORANGE]);
                                dcAss.DrawText(text.Mid(12,2), 1+charWidth_*13, 1+line*lineSpace_);
                                dcAss.DrawText(text.Right(5), 1+charWidth_*24, 1+line*lineSpace_);
                            }
                            else
                            {
                                text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
                                dcAss.DrawText(text.Left(5), 1+charWidth_, 1+line*lineSpace_);
                                setProfileColor(executedColor);
                                dcAss.DrawText(text.Mid(6,2), 1+charWidth_*7, 1+line*lineSpace_);
                                dcAss.DrawText(text.Mid(9,2), 1+charWidth_*10, 1+line*lineSpace_);
                                dcAss.DrawText(text.Mid(12,2), 1+charWidth_*13, 1+line*lineSpace_);
                                dcAss.DrawText(text.Mid(15,2), 1+charWidth_*16, 1+line*lineSpace_);
                                if (text.Len() >= 23)
                                {
                                    dcAss.DrawText(text.Mid(18,5), 1+charWidth_*19, 1+line*lineSpace_);
                                    dcAss.DrawText(text.Right(text.Len()-23), 1+charWidth_*24, 1+line*lineSpace_);
                                    
                                }
                                else
                                    dcAss.DrawText(text.Right(text.Len()-18), 1+charWidth_*19, 1+line*lineSpace_);
                            }
                        }
                    break;

                    default:
                        text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
                        dcAss.DrawText(text.Left(5), 1+charWidth_, 1+line*lineSpace_);
                        setProfileColor(executedColor);
                        dcAss.DrawText(text.Mid(6,2), 1+charWidth_*7, 1+line*lineSpace_);
                        dcAss.DrawText(text.Mid(9,2), 1+charWidth_*10, 1+line*lineSpace_);
                        if (text.Len() >= 23)
                        {
                            dcAss.DrawText(text.Mid(18,5), 1+charWidth_*19, 1+line*lineSpace_);
                            dcAss.DrawText(text.Right(text.Len()-23), 1+charWidth_*24, 1+line*lineSpace_);
                            
                        }
                        else
                            dcAss.DrawText(text.Right(text.Len()-18), 1+charWidth_*19, 1+line*lineSpace_);
                    break;
                }
                dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
                if (selectedTab_ == PROFILERTAB)
                    dcAss.DrawText(executedStr, 1+charWidth_*(32+numberOfSpaces)+locationCorrection[numberOfSpaces], 1+line*lineSpace_);
            break;

            case MEM_TYPE_OPCODE_LBR_SLOT:
                text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
                dcAss.DrawText(text.Left(5), 1+charWidth_, 1+line*lineSpace_);
                setProfileColor(executedColor);
                dcAss.DrawText(text.Mid(6,2), 1+charWidth_*7, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(18,5), 1+charWidth_*19, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(23,4), 1+charWidth_*24, 1+line*lineSpace_);
                dcAss.SetTextForeground(guiTextColour[GUI_COL_ORANGE]);
                dcAss.DrawText(text.Mid(9,2), 1+charWidth_*10, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(12,2), 1+charWidth_*13, 1+line*lineSpace_);
                dcAss.DrawText(text.Right(4), 1+charWidth_*28, 1+line*lineSpace_);
                dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
                if (selectedTab_ == PROFILERTAB)
                    dcAss.DrawText(executedStr, 1+charWidth_*(32+numberOfSpaces)+locationCorrection[numberOfSpaces], 1+line*lineSpace_);
            break;

            case MEM_TYPE_OPCODE_RLDL:
                text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
                dcAss.DrawText(text.Left(5), 1+charWidth_, 1+line*lineSpace_);
                setProfileColor(executedColor);
                dcAss.DrawText(text.Mid(6,2), 1+charWidth_*7, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(9,2), 1+charWidth_*10, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(18,4), 1+charWidth_*19, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(23,3), 1+charWidth_*24, 1+line*lineSpace_);
                dcAss.SetTextForeground(guiTextColour[GUI_COL_PINK]);
                dcAss.DrawText(text.Mid(12,2), 1+charWidth_*13, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(15,2), 1+charWidth_*16, 1+line*lineSpace_);
                dcAss.DrawText(text.Right(4), 1+charWidth_*27, 1+line*lineSpace_);
                dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
                if (selectedTab_ == PROFILERTAB)
                    dcAss.DrawText(executedStr, 1+charWidth_*(32+numberOfSpaces)+locationCorrection[numberOfSpaces], 1+line*lineSpace_);
            break;

            case MEM_TYPE_OPCODE_LDV:
                text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
                dcAss.DrawText(text.Left(5), 1+charWidth_, 1+line*lineSpace_);
                setProfileColor(executedColor);
                dcAss.DrawText(text.Mid(6,2), 1+charWidth_*7, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(9,2), 1+charWidth_*10, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(12,2), 1+charWidth_*13, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(18,4), 1+charWidth_*19, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(23,7), 1+charWidth_*24, 1+line*lineSpace_);
                dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
                if (selectedTab_ == PROFILERTAB)
                    dcAss.DrawText(executedStr, 1+charWidth_*(32+numberOfSpaces)+locationCorrection[numberOfSpaces], 1+line*lineSpace_);
                line += 1;
                if (line == EDIT_ROW && selectedTab_ == DIRECTASSTAB)
                {
                    dcAss.SetFont(exactFontBold);
                    dirAssAddress_ = address - 3;
                    dcAss.DrawText(">", 1, 1+line*lineSpace_);
                    dcAss.DrawText("<", bitmapWidth-9, 1+EDIT_ROW*lineSpace_);
                }
                else
                    dcAss.SetFont(exactFont);
                if (line < numberOfDebugLines)
                {
                    setProfileColor(executedColor);
                    line2.Printf("%02X", p_Computer->readMemDebug(address-3));
                    dcAss.DrawText(line2, 1+charWidth_*7, 1+line*lineSpace_);
                    line2.Printf("%02X", p_Computer->readMemDebug(address-2));
                    dcAss.DrawText(line2, 1+charWidth_*10, 1+line*lineSpace_);
                    line2.Printf("%02X", p_Computer->readMemDebug(address-1));
                    dcAss.DrawText(line2, 1+charWidth_*13, 1+line*lineSpace_);
                }
            break;

            case MEM_TYPE_OPCODE_LDL:
            case MEM_TYPE_OPCODE_LDRL:
                text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
                dcAss.DrawText(text.Left(5), 1+charWidth_, 1+line*lineSpace_);
                setProfileColor(executedColor);
                dcAss.DrawText(text.Mid(6,2), 1+charWidth_*7, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(12,2), 1+charWidth_*13, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(18,4), 1+charWidth_*19, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(23,3), 1+charWidth_*24, 1+line*lineSpace_);
                dcAss.SetTextForeground(guiTextColour[GUI_COL_PINK]);
                dcAss.DrawText(text.Mid(9,2), 1+charWidth_*10, 1+line*lineSpace_);
                dcAss.DrawText(text.Right(4), 1+charWidth_*27, 1+line*lineSpace_);
                dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
                if (selectedTab_ == PROFILERTAB)
                    dcAss.DrawText(executedStr, 1+charWidth_*(32+numberOfSpaces)+locationCorrection[numberOfSpaces], 1+line*lineSpace_);
                line += 1;
                if (line == EDIT_ROW && selectedTab_ == DIRECTASSTAB)
                {
                    dcAss.SetFont(exactFontBold);
                    dirAssAddress_ = address - 3;
                    dcAss.DrawText(">", 1, 1+line*lineSpace_);
                    dcAss.DrawText("<", bitmapWidth-9, 1+EDIT_ROW*lineSpace_);
                }
                else
                    dcAss.SetFont(exactFont);
                if (line < numberOfDebugLines)
                {
                    setProfileColor(executedColor);
                    line2.Printf("%02X", p_Computer->readMemDebug(address-3));
                    dcAss.DrawText(line2, 1+charWidth_*7, 1+line*lineSpace_);
                    line2.Printf("%02X", p_Computer->readMemDebug(address-1));
                    dcAss.DrawText(line2, 1+charWidth_*13, 1+line*lineSpace_);
                    dcAss.SetTextForeground(guiTextColour[GUI_COL_PINK]);
                    line2.Printf("%02X", p_Computer->readMemDebug(address-2));
                    dcAss.DrawText(line2, 1+charWidth_*10, 1+line*lineSpace_);
                }
            break;

            case MEM_TYPE_OPCODE_LDL_SLOT:
                text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
                dcAss.DrawText(text.Left(5), 1+charWidth_, 1+line*lineSpace_);
                setProfileColor(executedColor);
                dcAss.DrawText(text.Mid(6,2), 1+charWidth_*7, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(12,2), 1+charWidth_*13, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(18,4), 1+charWidth_*19, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(23,7), 1+charWidth_*24, 1+line*lineSpace_);
                dcAss.SetTextForeground(guiTextColour[GUI_COL_PINK]);
                dcAss.DrawText(text.Mid(9,2), 1+charWidth_*10, 1+line*lineSpace_);
                dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
                if (selectedTab_ == PROFILERTAB)
                    dcAss.DrawText(executedStr, 1+charWidth_*(32+numberOfSpaces)+locationCorrection[numberOfSpaces], 1+line*lineSpace_);
                line += 1;
                if (line == EDIT_ROW && selectedTab_ == DIRECTASSTAB)
                {
                    dcAss.SetFont(exactFontBold);
                    dirAssAddress_ = address - 3;
                    dcAss.DrawText(">", 1, 1+line*lineSpace_);
                    dcAss.DrawText("<", bitmapWidth-9, 1+EDIT_ROW*lineSpace_);
                }
                else
                    dcAss.SetFont(exactFont);
                if (line < numberOfDebugLines)
                {
                    setProfileColor(executedColor);
                    line2.Printf("%02X", p_Computer->readMemDebug(address-3));
                    dcAss.DrawText(line2, 1+charWidth_*7, 1+line*lineSpace_);
                    line2.Printf("%02X", p_Computer->readMemDebug(address-1));
                    dcAss.DrawText(line2, 1+charWidth_*13, 1+line*lineSpace_);
                    dcAss.SetTextForeground(guiTextColour[GUI_COL_PINK]);
                    line2.Printf("%02X", p_Computer->readMemDebug(address-2));
                    dcAss.DrawText(line2, 1+charWidth_*10, 1+line*lineSpace_);
                    line2.Printf("%02X%02X", p_Computer->readMemDebug(address-5),p_Computer->readMemDebug(address-2));
                    dcAss.DrawText(line2, 1+charWidth_*24, 1+line*lineSpace_);
                }
            break;

            case MEM_TYPE_OPCODE_LDRL_SLOT:
                text = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, 0, 0xFFFF);
                dcAss.DrawText(text.Left(5), 1+charWidth_, 1+line*lineSpace_);
                setProfileColor(executedColor);
                dcAss.DrawText(text.Mid(6,2), 1+charWidth_*7, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(12,2), 1+charWidth_*13, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(18,4), 1+charWidth_*19, 1+line*lineSpace_);
                dcAss.DrawText(text.Mid(23,7), 1+charWidth_*24, 1+line*lineSpace_);
                dcAss.SetTextForeground(guiTextColour[GUI_COL_PINK]);
                dcAss.DrawText(text.Mid(9,2), 1+charWidth_*10, 1+line*lineSpace_);
                dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
                if (selectedTab_ == PROFILERTAB)
                    dcAss.DrawText(executedStr, 1+charWidth_*(32+numberOfSpaces)+locationCorrection[numberOfSpaces], 1+line*lineSpace_);
                line += 1;
                if (line == EDIT_ROW && selectedTab_ == DIRECTASSTAB)
                {
                    dcAss.SetFont(exactFontBold);
                    dirAssAddress_ = address - 3;
                    dcAss.DrawText(">", 1, 1+line*lineSpace_);
                    dcAss.DrawText("<", bitmapWidth-9, 1+EDIT_ROW*lineSpace_);
                }
                else
                    dcAss.SetFont(exactFont);
                if (line < numberOfDebugLines)
                {
                    setProfileColor(executedColor);
                    line2.Printf("%02X", p_Computer->readMemDebug(address-3));
                    dcAss.DrawText(line2, 1+charWidth_*7, 1+line*lineSpace_);
                    line2.Printf("%02X", p_Computer->readMemDebug(address-1));
                    dcAss.DrawText(line2, 1+charWidth_*13, 1+line*lineSpace_);
                    dcAss.SetTextForeground(guiTextColour[GUI_COL_PINK]);
                    line2.Printf("%02X", p_Computer->readMemDebug(address-2));
                    dcAss.DrawText(line2, 1+charWidth_*10, 1+line*lineSpace_);
                    line2.Printf("%02X%02X", p_Computer->readMemDebug(address-2),p_Computer->readMemDebug(address-5));
                    dcAss.DrawText(line2, 1+charWidth_*24, 1+line*lineSpace_);
                }
            break;
                
            case MEM_TYPE_OPERAND_LD_3:
                setProfileColor(executedColor);
                if (p_Computer->readMemDataType((address-3)&0xffff, &executed) == MEM_TYPE_OPCODE_LDL_SLOT)
                {
                    line2.Printf("%02X%02X", p_Computer->readMemDebug((address-2)&0xffff),p_Computer->readMemDebug(address+1));
                    dcAss.DrawText(line2, 1+charWidth_*24, 1+line*lineSpace_);
                }
                
                line2.Printf("%02X", p_Computer->readMemDebug(address));
                dcAss.DrawText(line2, 1+charWidth_*7, 1+line*lineSpace_);
                line2.Printf("%02X", p_Computer->readMemDebug(address+2));
                dcAss.DrawText(line2, 1+charWidth_*13, 1+line*lineSpace_);
                line2.Printf("%02X", p_Computer->readMemDebug(address+1));

                if (p_Computer->readMemDataType((address-3)&0xffff, &executed) != MEM_TYPE_OPCODE_LDV)
                    dcAss.SetTextForeground(guiTextColour[GUI_COL_PINK]);
                dcAss.DrawText(line2, 1+charWidth_*10, 1+line*lineSpace_);
                address+=3;
                address&=0xffff;
            break;

            case MEM_TYPE_OPERAND_LDR_3:
                setProfileColor(executedColor);
                if (p_Computer->readMemDataType((address-3)&0xffff, &executed) == MEM_TYPE_OPCODE_LDRL_SLOT)
                {
                    line2.Printf("%02X%02X", p_Computer->readMemDebug((address+1)&0xffff),p_Computer->readMemDebug(address-2));
                    dcAss.DrawText(line2, 1+charWidth_*24, 1+line*lineSpace_);
                }
                
                line2.Printf("%02X", p_Computer->readMemDebug(address));
                dcAss.DrawText(line2, 1+charWidth_*7, 1+line*lineSpace_);
                line2.Printf("%02X", p_Computer->readMemDebug(address+2));
                dcAss.DrawText(line2, 1+charWidth_*13, 1+line*lineSpace_);
                line2.Printf("%02X", p_Computer->readMemDebug(address+1));

                if (p_Computer->readMemDataType((address-3)&0xffff, &executed) != MEM_TYPE_OPCODE_LDV)
                    dcAss.SetTextForeground(guiTextColour[GUI_COL_PINK]);
                dcAss.DrawText(line2, 1+charWidth_*10, 1+line*lineSpace_);
                address+=3;
                address&=0xffff;
            break;

            case MEM_TYPE_PSEUDO_1:
                code = pseudoDisassemble(address, false, true);
                dcAss.DrawText(code.Left(5), 1+charWidth_, 1+line*lineSpace_);
                dcAss.SetTextForeground(guiTextColour[GUI_COL_STEEL]);
                dcAss.DrawText(code.Right(code.Len()-6), 57, 1+line*lineSpace_);

                command = p_Computer->readMemDebug(address);

                if (!checkSingleCommand(command))
                    address+=2;
                else
                    address+=1;
                
                if (checkTrippleCommand(command))
                    address++;
                
                if (checkQuadrupleCommand(command))
                    address+=2;

                address&=0xffff;
            break;
                
            case MEM_TYPE_OPCODE_JUMP_SLOT:
                printBufferAddress.Printf("%04X: ", address);
                dcAss.DrawText(printBufferAddress, 1+charWidth_, 1+line*lineSpace_);

                dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
                printBufferOpcode.Printf("S%02X,", p_Computer->readMemDataType(address+1, &executed));
                dcAss.DrawText(printBufferOpcode, 1+charWidth_*19, 1+line*lineSpace_);

                dcAss.SetTextForeground(guiTextColour[GUI_COL_PINK]);
                printBufferOpcode.Printf("%02X", p_Computer->readMemDebug(address));
                dcAss.DrawText(printBufferOpcode, 1+charWidth_*7, 1+line*lineSpace_);
                printBufferOpcode.Printf("%02X", p_Computer->readMemDebug(address+1));
                dcAss.DrawText(printBufferOpcode, 1+charWidth_*10, 1+line*lineSpace_);
                printBufferOpcode.Printf("%04X", (p_Computer->readMemDebug(address)<<8) + p_Computer->readMemDebug(address+1));
                dcAss.DrawText(printBufferOpcode, 1+charWidth_*23, 1+line*lineSpace_);
                address+=2;
                address&=0xffff;
            break;

            case MEM_TYPE_JUMP:
                printBufferAddress.Printf("%04X: ", address);
                dcAss.DrawText(printBufferAddress, 1+charWidth_, 1+line*lineSpace_);

                dcAss.SetTextForeground(guiTextColour[GUI_COL_PINK]);
                printBufferOpcode.Printf("%02X", p_Computer->readMemDebug(address));
                dcAss.DrawText(printBufferOpcode, 1+charWidth_*7, 1+line*lineSpace_);
                printBufferOpcode.Printf("%02X", p_Computer->readMemDebug(address+1));
                dcAss.DrawText(printBufferOpcode, 1+charWidth_*10, 1+line*lineSpace_);
                printBufferOpcode.Printf("%04X", (p_Computer->readMemDebug(address)<<8) + p_Computer->readMemDebug(address+1));
                dcAss.DrawText(printBufferOpcode, 1+charWidth_*19, 1+line*lineSpace_);
                address+=2;
                address&=0xffff;
            break;

            case MEM_TYPE_JUMP_REV:
                printBufferAddress.Printf("%04X: ", address);
                dcAss.DrawText(printBufferAddress, 1+charWidth_, 1+line*lineSpace_);

                dcAss.SetTextForeground(guiTextColour[GUI_COL_RED]);
                printBufferOpcode.Printf("%02X", p_Computer->readMemDebug(address));
                dcAss.DrawText(printBufferOpcode, 1+charWidth_*7, 1+line*lineSpace_);
                printBufferOpcode.Printf("%02X", p_Computer->readMemDebug(address+1));
                dcAss.DrawText(printBufferOpcode, 1+charWidth_*10, 1+line*lineSpace_);
                printBufferOpcode.Printf("%04X", (p_Computer->readMemDebug(address+1)<<8) + p_Computer->readMemDebug(address));
                dcAss.DrawText(printBufferOpcode, 1+charWidth_*19, 1+line*lineSpace_);
                address+=2;
                address&=0xffff;
            break;

/*            case MEM_TYPE_TEXT:
                printBufferAddress.Printf("%04X: ", address);
                dcAss.DrawText(printBufferAddress, 1+charWidth_, 1+line*lineSpace_);
                
                printBufferOpcode.Printf("%02X", p_Computer->readMemDebug(address));
                dcAss.SetTextForeground(guiTextColour[GUI_COL_PURPLE]);
                dcAss.DrawText(printBufferOpcode, 1+charWidth_*7, 1+line*lineSpace_);
                drawAssCharacter(address, line, 0);
                address++;
                address&=0xffff;
            break;*/
                
            default:
                if ((dataViewDump && selectedTab_ == DIRECTASSTAB) || (dataViewProfiler && selectedTab_ == PROFILERTAB))
                {
                    printBufferAddress.Printf("%04X: ", address);
                    dcAss.DrawText(printBufferAddress, 1+charWidth_, 1+line*lineSpace_);
                    count = 0;
                    memType = p_Computer->readMemDataType(address, &executed);
                    while (count < 4 && (memType == MEM_TYPE_UNDEFINED || memType == MEM_TYPE_DATA ||  memType == MEM_TYPE_TEXT || memType == MEM_TYPE_PSEUDO_2 || memType == MEM_TYPE_OPERAND))
                    {
                        printBufferOpcode.Printf("%02X", p_Computer->readMemDebug(address));
                        switch (memType)
                        {
                            case MEM_TYPE_UNDEFINED:
                                dcAss.SetTextForeground(guiTextColour[GUI_COL_RED]);
                            break;
                            case MEM_TYPE_TEXT:
                                dcAss.SetTextForeground(guiTextColour[GUI_COL_PURPLE]);
                            break;
                            default:
                                dcAss.SetTextForeground(guiTextColour[GUI_COL_BLUE]);
                            break;
                        }
                        dcAss.DrawText(printBufferOpcode, 1+charWidth_*(7+count*3), 1+line*lineSpace_);
                        drawAssCharacter(address, line, count);
                        address++;
                        address&=0xffff;
                        count++;
                        memType = p_Computer->readMemDataType(address, &executed);
                    }
                }
                else
                {
                    printBufferAddress.Printf("%04X: ", address);
                    dcAss.DrawText(printBufferAddress, 1+charWidth_, 1+line*lineSpace_);

                    printBufferOpcode.Printf("%02X", p_Computer->readMemDebug(address));
                    switch (memType)
                    {
                        case MEM_TYPE_UNDEFINED:
                            dcAss.SetTextForeground(guiTextColour[GUI_COL_RED]);
                            break;
                        case MEM_TYPE_TEXT:
                            dcAss.SetTextForeground(guiTextColour[GUI_COL_PURPLE]);
                            break;
                        default:
                            dcAss.SetTextForeground(guiTextColour[GUI_COL_BLUE]);
                            break;
                    }
                    dcAss.DrawText(printBufferOpcode, 1+charWidth_*7, 1+line*lineSpace_);
                    drawAssCharacter(address, line, 0);
                    address++;
                    address&=0xffff;
                }
            break;
        }
//        if (line == EDIT_ROW)
//            dcAss.SetFont(exactFont);
    }
    dirAssEnd_ = address;
    dcAss.SelectObject(wxNullBitmap);

    switch (selectedTab_)
    {
        case DIRECTASSTAB:
            XRCCTRL(*this, "AssBitmap", wxStaticBitmap)->SetBitmap(*assBmp);
        break;

        case PROFILERTAB:
            XRCCTRL(*this, "ProfilerBitmap", wxStaticBitmap)->SetBitmap(*profilerBmp);
        break;
    }
    int range = XRCCTRL(*this,"AssRangeType",wxChoice)->GetCurrentSelection();
    if (range == -1)
    {
        range = 0;
        XRCCTRL(*this,"AssRangeType",wxChoice)->SetSelection(range);
    }
    if (range == 0)
    {
        printBufferAddress.Printf("%04X", dirAssAddress_);
        XRCCTRL(*this, "AssStartType", wxTextCtrl)->SetValue(printBufferAddress);
    }
    if (XRCCTRL(*this,"AssDataView",wxChoice)->GetCurrentSelection() == -1)
        XRCCTRL(*this,"AssDataView",wxChoice)->SetSelection(0);
    if (XRCCTRL(*this,"ProfilerDataView",wxChoice)->GetCurrentSelection() == -1)
        XRCCTRL(*this,"ProfilerDataView",wxChoice)->SetSelection(0);
    if (XRCCTRL(*this,"AssType",wxChoice)->GetCurrentSelection() == -1)
        XRCCTRL(*this,"AssType",wxChoice)->SetSelection(0);

}

void DebugWindow::setProfileColor(Byte executedColor)
{
    if (executedColor > 0 && selectedTab_ == PROFILERTAB)
        dcAss.SetTextForeground(wxColour(executedColor+34,221-executedColor,0));
    else
        dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
}

void DebugWindow::onProfilerType(wxCommandEvent&event)
{
    profilerType_ = event.GetSelection();
    directAss();
}

void DebugWindow::onProfilerCounter(wxCommandEvent&event)
{
    profilerCounter_ = event.GetSelection();
}

void DebugWindow::onProfilerClear(wxCommandEvent&WXUNUSED(event))
{
    p_Computer->clearProfiler();
    directAss();
}

void DebugWindow::drawAssCharacter(Word address, int line, int count)
{
    int t;
    char bits [9];
    
//    dcAss.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
    dcAss.SetTextBackground(guiBackGround_);
    if (computerRunning_ && computerConfiguration.vis1870Configuration.defined)
    {
        for (int i=0; i<9; i++)
        {
            t = p_Computer->readCramDirect((p_Computer->readMemDebug(address)&0x7f)*p_Computer->getMaxLinesPerChar()+i);

            if (darkMode_)
                t = t ^ 0xff;
            bits[i] = (t & 0x1) << 5;
            bits[i] |= (t & 0x2) << 3;
            bits[i] |= (t & 0x4) << 1;
            bits[i] |= (t & 0x8) >> 1;
            bits[i] |= (t & 0x10) >> 3;
            bits[i] |= (t & 0x20) >> 5;
        }
        wxBitmap character(bits, 6, 9, 1);
        dcAss.DrawBitmap(character, 1+charWidth_*(19+count), 5+line*lineSpace_, false);
    }
    else
    {
        wxString character;
        Byte byteValue = p_Computer->readMemDebug(address)&0x7f;
        
        if (computerConfiguration.studio4VideoConfiguration.defined && pseudoType_ != "AM4KBAS2020")
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
            dcAss.DrawText(character, 1+charWidth_*(19+count), 1+line*lineSpace_);
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
    int    dataViewCount = 4;
    uint64_t executed;

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
            if (computerConfiguration.studio4VideoConfiguration.defined && pseudoType_ != "AM4KBAS2020")
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
            p_Computer->writeMemDebug(addressValue, character, true);
            p_Computer->writeMemDataType(addressValue++, MEM_TYPE_TEXT);
            assInputWindowPointer->Clear();
            if ((dataViewDump && selectedTab_ == DIRECTASSTAB) || (dataViewProfiler && selectedTab_ == PROFILERTAB))
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
                    p_Computer->writeMemDebug(addressValue, byteValue >> shift, true);
                    p_Computer->writeMemDataType(addressValue++, MEM_TYPE_DATA);
                    nextByte = nextByte - byteValue;
                    shift -= 8;
                    maskByte = maskByte >> 8;

                    assInputWindowPointer->Clear();
                    if ((dataViewDump && selectedTab_ == DIRECTASSTAB) || (dataViewProfiler && selectedTab_ == PROFILERTAB))
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
            case MEM_TYPE_OPCODE_LDRL:
                typeOpcode = MEM_TYPE_OPCODE_LDRL;
                typeOperand2 = MEM_TYPE_OPERAND_LDR_2;
                typeOperand3 = MEM_TYPE_OPERAND_LDR_3;
                typeOperand5 = MEM_TYPE_OPERAND_LDR_5;
                count = 6;
            break;
            case MEM_TYPE_OPCODE_LDRL_SLOT:
                typeOpcode = MEM_TYPE_OPCODE_LDRL_SLOT;
                typeOperand1 = b4;
                b4 = 0xf8;
                typeOperand2 = MEM_TYPE_OPERAND_LDR_2;
                typeOperand3 = MEM_TYPE_OPERAND_LDR_3;
                typeOperand5 = MEM_TYPE_OPERAND_LDR_5;
                count = 6;
            break;
       }
    }

    if (pseudoLoaded_ && (count == ERROR_INST || count == ERROR_TEMP_PAR || count == ERROR_TEMP_CPU_1801))
    {
        typeOpcode = MEM_TYPE_PSEUDO_1;
        typeOperand1 = MEM_TYPE_PSEUDO_2;
        typeOperand2 = MEM_TYPE_PSEUDO_2;
        typeOperand3 = MEM_TYPE_PSEUDO_2;
        count = assemblePseudo(&debugIn, &b1, &b2, &b3, &b4);
    }

    if (count > 0 && count < 7)
    {
        if (p_Computer->readMemDataType(addressValue, &executed) == MEM_TYPE_OPERAND_LD_3 || p_Computer->readMemDataType(addressValue, &executed) == MEM_TYPE_OPERAND_LDR_3)
            addressValue -= 3;

        for (int i=addressValue; i<addressValue+count; i++)
        {
            if (p_Computer->readMemDataType(i, &executed) == MEM_TYPE_OPCODE_LBR_SLOT)
            {
                for (int j=0; j<3; j++)
                {
                    p_Computer->writeMemDebug(i+j, 0xc4, true);
                    p_Computer->writeMemDataType(i+j, MEM_TYPE_OPCODE);
                }
            }
            if (p_Computer->readMemDataType(i, &executed) == MEM_TYPE_JUMP_REV || p_Computer->readMemDataType(i, &executed) == MEM_TYPE_JUMP || p_Computer->readMemDataType(i, &executed) == MEM_TYPE_OPCODE_JUMP_SLOT)
            {
                for (int j=0; j<2; j++)
                {
                    p_Computer->writeMemDebug(i+j, 0xc4, true);
                    p_Computer->writeMemDataType(i+j, MEM_TYPE_OPCODE);
                }
            }
        }

        int checkAddres = setMemLabel(dirAssAddress_, true);
        
        p_Computer->writeMemDebug(addressValue, b1, true);
        p_Computer->writeMemDataType(addressValue++, typeOpcode);

        if (count > 1)
        {
            p_Computer->writeMemDebug(addressValue, b2, true);
            p_Computer->writeMemDataType(addressValue++, typeOperand1);
        }
        if (count > 2) 
        {
            p_Computer->writeMemDebug(addressValue, b3, true);
            p_Computer->writeMemDataType(addressValue++, typeOperand2);
        }
        if (count > 3) 
        {
            p_Computer->writeMemDebug(addressValue, b4, true);
            p_Computer->writeMemDataType(addressValue++, typeOperand3);
        }
        if (count > 4) 
        {
            p_Computer->writeMemDebug(addressValue, b5, true);
            p_Computer->writeMemDataType(addressValue++, typeOperand4);
        }
        if (count > 5) 
        {
            p_Computer->writeMemDebug(addressValue, b6, true);
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
        
/*        while (p_Computer->readMemDataType(addressValue) == MEM_TYPE_OPERAND || p_Computer->readMemDataType(addressValue) == MEM_TYPE_OPERAND_LD_2  || p_Computer->readMemDataType(addressValue) == MEM_TYPE_OPERAND_LD_3  || p_Computer->readMemDataType(addressValue) == MEM_TYPE_OPERAND_LD_5 || p_Computer->readMemDataType(addressValue) == MEM_TYPE_ST2_2 || p_Computer->readMemDataType(addressValue) == MEM_TYPE_FEL2_2 || p_Computer->readMemDataType(addressValue) == MEM_TYPE_CHIP_8_2)
        {
            p_Computer->writeMemDebug(addressValue, 0xc4, true);
            p_Computer->writeMemDataType(addressValue++, MEM_TYPE_OPCODE);
        }*/

        while (p_Computer->readMemDataType(addressValue, &executed) == MEM_TYPE_OPERAND || p_Computer->readMemDataType(addressValue, &executed) == MEM_TYPE_OPERAND_LD_2  || p_Computer->readMemDataType(addressValue, &executed) == MEM_TYPE_OPERAND_LD_3  || p_Computer->readMemDataType(addressValue, &executed) == MEM_TYPE_OPERAND_LD_5  || p_Computer->readMemDataType(addressValue, &executed) == MEM_TYPE_OPERAND_LDR_2  || p_Computer->readMemDataType(addressValue, &executed) == MEM_TYPE_OPERAND_LDR_3  || p_Computer->readMemDataType(addressValue, &executed) == MEM_TYPE_OPERAND_LDR_5 || p_Computer->readMemDataType(addressValue, &executed) == MEM_TYPE_PSEUDO_2)
        {
            p_Computer->writeMemDebug(addressValue, 0, true);
            p_Computer->writeMemDataType(addressValue++, MEM_TYPE_DATA);
        }

        assInputWindowPointer->Clear();
        if ((dataViewDump && selectedTab_ == DIRECTASSTAB) || (dataViewProfiler && selectedTab_ == PROFILERTAB) || typeOpcode == MEM_TYPE_OPCODE || typeOpcode == MEM_TYPE_JUMP   || typeOpcode == MEM_TYPE_JUMP_REV || typeOpcode == MEM_TYPE_OPCODE_LDL_SLOT || typeOpcode == MEM_TYPE_OPCODE_LDRL_SLOT || typeOpcode == MEM_TYPE_OPCODE_LDRL || (typeOpcode >= MEM_TYPE_OPCODE_RSHR && typeOpcode <= MEM_TYPE_OPCODE_LDL))
        {
            assSpinDown();
            if (typeOpcode == MEM_TYPE_OPCODE_LDV || typeOpcode == MEM_TYPE_OPCODE_LDL || typeOpcode == MEM_TYPE_OPCODE_LDL_SLOT || typeOpcode == MEM_TYPE_OPCODE_LDRL || typeOpcode == MEM_TYPE_OPCODE_LDRL_SLOT)
                assSpinDown();
        }
        else
            for (int i=0; i<count; i++)
                assSpinDown();
        directAss();
//        assErrorDisplay("");
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
    uint64_t executed;
    
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
    switch(p_Computer->readMemDataType(labelAddress, &executed))
    {
        case MEM_TYPE_OPCODE:
        case MEM_TYPE_OPCODE_BPZ:
        case MEM_TYPE_OPCODE_BGE:
        case MEM_TYPE_OPCODE_BM:
        case MEM_TYPE_OPCODE_BL:
            switch(jumpCorrection[p_Computer->readMemDebug(labelAddress)])
            {
                case 1:
                    branchAddress = ((labelAddress+1)&0xff00) + p_Computer->readMemDebug(labelAddress+1);
                    p_Computer->writeMemLabelType(branchAddress, newBranchMemLabel);
                break;
                    
                case 2:
                    branchAddress = (p_Computer->readMemDebug(labelAddress+1)<<8)+p_Computer->readMemDebug(labelAddress+2);
                    p_Computer->writeMemLabelType(branchAddress, newJumpMemLabel);
                    checkSlotAddressWarning(branchAddress);
                break;

                case 3:
                    if (p_Computer->readMemDebug(labelAddress+1) == 0x3E || p_Computer->readMemDebug(labelAddress+1) == 0x3F)
                    {
                        branchAddress = ((labelAddress+2)&0xff00) + p_Computer->readMemDebug(labelAddress+2);
                        p_Computer->writeMemLabelType(branchAddress, newBranchMemLabel);
                    }
                    if ((p_Computer->readMemDebug(labelAddress+1)&0xf0) == 0x20)
                    {
                        branchAddress = (p_Computer->readMemDebug(labelAddress+2)<<8)+p_Computer->readMemDebug(labelAddress+3);
                        p_Computer->writeMemLabelType(branchAddress, newJumpMemLabel);
                        checkSlotAddressWarning(branchAddress);
                    }
                    if ((p_Computer->readMemDebug(labelAddress+1)&0xf0) == 0x80)
                    {
                        branchAddress = (p_Computer->readMemDebug(labelAddress+2)<<8)+p_Computer->readMemDebug(labelAddress+3);
                        p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
                        checkSlotAddressWarning(branchAddress);
                    }
                break;
            }
        break;
    
        case MEM_TYPE_OPCODE_RLDL:
            branchAddress = (p_Computer->readMemDebug(labelAddress+2)<<8)+p_Computer->readMemDebug(labelAddress+3);
            p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
            checkSlotAddressWarning(branchAddress);
        break;
    
        case MEM_TYPE_OPCODE_LDL:
            branchAddress = (p_Computer->readMemDebug(labelAddress+1)<<8)+p_Computer->readMemDebug(labelAddress+4);
            p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
            checkSlotAddressWarning(branchAddress);
        break;
    
        case MEM_TYPE_OPCODE_LDRL:
            branchAddress = (p_Computer->readMemDebug(labelAddress+4)<<8)+p_Computer->readMemDebug(labelAddress+1);
            p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
            checkSlotAddressWarning(branchAddress);
        break;
    
        case MEM_TYPE_JUMP:
            branchAddress = (p_Computer->readMemDebug(labelAddress)<<8)+p_Computer->readMemDebug(labelAddress+1);
            p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
            checkSlotAddressWarning(branchAddress);
        break;
    
        case MEM_TYPE_JUMP_REV:
            branchAddress = (p_Computer->readMemDebug(labelAddress+1)<<8)+p_Computer->readMemDebug(labelAddress);
            p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
            checkSlotAddressWarning(branchAddress);
        break;
    
        case MEM_TYPE_OPCODE_JUMP_SLOT:
            out1 = getOut1();
            setOut1(p_Computer->readMemDataType(labelAddress+1, &executed));
            branchAddress = (p_Computer->readMemDebug(labelAddress)<<8)+p_Computer->readMemDebug(labelAddress+1);
            p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
            setOut1(out1);
        break;
    
        case MEM_TYPE_OPCODE_LBR_SLOT:
            out1 = getOut1();
            setOut1(p_Computer->readMemDataType(labelAddress+1, &executed));
            branchAddress = (p_Computer->readMemDebug(labelAddress+1)<<8)+p_Computer->readMemDebug(labelAddress+2);
            p_Computer->writeMemLabelType(branchAddress, newJumpMemLabel);
            setOut1(out1);
        break;
    
        case MEM_TYPE_OPCODE_LDL_SLOT:
            out1 = getOut1();
            setOut1(p_Computer->readMemDataType(labelAddress+1, &executed));
            branchAddress = (p_Computer->readMemDebug(labelAddress+1)<<8)+p_Computer->readMemDebug(labelAddress+4);
            p_Computer->writeMemLabelType(branchAddress, newSubMemLabel);
            setOut1(out1);
        break;

        case MEM_TYPE_OPCODE_LDRL_SLOT:
            out1 = getOut1();
            setOut1(p_Computer->readMemDataType(labelAddress+1, &executed));
            branchAddress = (p_Computer->readMemDebug(labelAddress+4)<<8)+p_Computer->readMemDebug(labelAddress+1);
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
    if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
    {
        if (branchAddress >= computerConfiguration.slotConfiguration.start && branchAddress <= computerConfiguration.slotConfiguration.end)
            return true;
    }
    else
    {
        for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = computerConfiguration.emsMemoryConfiguration.begin (); emsConfig != computerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
        {
            for (std::vector<WordRange>::iterator range = emsConfig->range.begin (); range != emsConfig->range.end (); ++range)
            {
                if (branchAddress >= range->start && branchAddress <= range->end)
                    return true;
            }
        }
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

void DebugWindow::onProfilerAddress(wxCommandEvent&WXUNUSED(event))
{
    long address = get16BitValue("ProfilerAddress");
    if (address == -1)  return;

    dirAssStart_ = address;

    if (xmlLoaded_)
        p_Main->updateAssTab();
}

void DebugWindow::onAssThumbTrack(wxScrollEvent&event)
{
    long address = event.GetPosition();
    if (address == -1)  return;

    dirAssStart_ = address;

    for (int i=0; i<EDIT_ROW; i++)
        assSpinUp();

    directAss();
    assErrorDisplay("");
}

void DebugWindow::onProfilerThumbTrack(wxScrollEvent&event)
{
    long address = event.GetPosition();
    if (address == -1)  return;

    dirAssStart_ = address;

    directAss();
    assErrorDisplay("");
}

void DebugWindow::onAssSpinDown(wxSpinEvent&WXUNUSED(event))
{
    if (!computerRunning_)
        return;

    assSpinDown();

    directAss();
    assErrorDisplay("");
}

void DebugWindow::onAssSpinDown(wxScrollEvent&WXUNUSED(event))
{
    if (!computerRunning_)
        return;

    assSpinDown();

    XRCCTRL(*this,"AssScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);

    directAss();
    assErrorDisplay("");
}

void DebugWindow::onProfilerSpinDown(wxScrollEvent&WXUNUSED(event))
{
    if (!computerRunning_)
        return;

    assSpinDown();

    XRCCTRL(*this,"ProfilerScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);

    directAss();
    assErrorDisplay("");
}

void DebugWindow::assSpinDownScroll()
{
    assSpinDown();
    if (selectedTab_ == PROFILERTAB)
        XRCCTRL(*this,"ProfilerScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);
    else
        XRCCTRL(*this,"AssScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);
}

void DebugWindow::assSpinDown()
{
    uint64_t executed;
    int count;
    switch(p_Computer->readMemDataType(dirAssStart_, &executed))
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
            while (p_Computer->readMemDataType(dirAssStart_, &executed) == MEM_TYPE_OPERAND)
            {
                dirAssStart_++;
                dirAssStart_&=0xffff;
            }
        break;

        case MEM_TYPE_OPCODE_LDV:
        case MEM_TYPE_OPCODE_LDL:
        case MEM_TYPE_OPCODE_LDL_SLOT:
        case MEM_TYPE_OPCODE_LDRL:
        case MEM_TYPE_OPCODE_LDRL_SLOT:
        case MEM_TYPE_OPERAND_LD_3:
        case MEM_TYPE_OPERAND_LDR_3:
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
            dirAssStart_+=2;
            dirAssStart_&=0xffff;
        break;

        case MEM_TYPE_PSEUDO_1:
            dirAssStart_++;
            dirAssStart_&=0xffff;
            while (p_Computer->readMemDataType(dirAssStart_, &executed) == MEM_TYPE_PSEUDO_2)
            {
                dirAssStart_++;
                dirAssStart_&=0xffff;
            }
        break;

        case MEM_TYPE_DATA:
        case MEM_TYPE_TEXT:
        case MEM_TYPE_UNDEFINED:
            if ((dataViewDump && selectedTab_ == DIRECTASSTAB) || (dataViewProfiler && selectedTab_ == PROFILERTAB))
            {
                count = 0;
                while (count < 4 && (p_Computer->readMemDataType(dirAssStart_, &executed) == MEM_TYPE_DATA || p_Computer->readMemDataType(dirAssStart_, &executed) == MEM_TYPE_TEXT || p_Computer->readMemDataType(dirAssStart_, &executed) == MEM_TYPE_UNDEFINED))
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

void DebugWindow::onAssSpinUp(wxScrollEvent&WXUNUSED(event))
{
    if (!computerRunning_)
        return;

    assSpinUp();

    XRCCTRL(*this,"AssScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);

    directAss();
    assErrorDisplay("");
}

void DebugWindow::onProfilerSpinUp(wxScrollEvent&WXUNUSED(event))
{
    if (!computerRunning_)
        return;

    assSpinUp();

    XRCCTRL(*this,"ProfilerScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);

    directAss();
    assErrorDisplay("");
}

void DebugWindow::assSpinUpScroll()
{
    assSpinUp();
    if (selectedTab_ == PROFILERTAB)
        XRCCTRL(*this,"ProfilerScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);
    else
        XRCCTRL(*this,"AssScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);
}

void DebugWindow::assSpinUp()
{
    if (!computerRunning_)
        return;

    uint64_t executed;
    int count;
    dirAssStart_--;
    dirAssStart_&=0xffff;

    if (p_Computer->readMemDataType(dirAssStart_-1, &executed) == MEM_TYPE_JUMP_REV || p_Computer->readMemDataType(dirAssStart_-1, &executed) == MEM_TYPE_JUMP || p_Computer->readMemDataType(dirAssStart_-1, &executed) == MEM_TYPE_OPCODE_JUMP_SLOT)
    {
        dirAssStart_--;
        dirAssStart_&=0xffff;
        return;
    }
    switch(p_Computer->readMemDataType(dirAssStart_, &executed))
    {
        case MEM_TYPE_OPERAND:
            while (p_Computer->readMemDataType(dirAssStart_, &executed) == MEM_TYPE_OPERAND)
            {
                dirAssStart_--;
                dirAssStart_&=0xffff;
            }
        break;

        case MEM_TYPE_OPERAND_LD_5:
        case MEM_TYPE_OPERAND_LD_2:
        case MEM_TYPE_OPERAND_LDR_5:
        case MEM_TYPE_OPERAND_LDR_2:
        case MEM_TYPE_OPCODE_LBR_SLOT:
            dirAssStart_ -= 2;
            dirAssStart_&=0xffff;
        break;

        case MEM_TYPE_PSEUDO_2:
            while (p_Computer->readMemDataType(dirAssStart_, &executed) == MEM_TYPE_PSEUDO_2)
            {
                dirAssStart_--;
                dirAssStart_&=0xffff;
            }
        break;

        case MEM_TYPE_DATA:
        case MEM_TYPE_TEXT:
        case MEM_TYPE_UNDEFINED:
            if ((dataViewDump && selectedTab_ == DIRECTASSTAB) || (dataViewProfiler && selectedTab_ == PROFILERTAB))
            {
                count = 0;
                while (count < 3 && (p_Computer->readMemDataType((dirAssStart_-1)&0xFFFF, &executed) == MEM_TYPE_DATA || p_Computer->readMemDataType((dirAssStart_-1)&0xFFFF, &executed) == MEM_TYPE_TEXT || p_Computer->readMemDataType((dirAssStart_-1)&0xFFFF, &executed) == MEM_TYPE_UNDEFINED))
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

void DebugWindow::onAssSpinPageDown(wxScrollEvent&WXUNUSED(event))
{
    if (!computerRunning_)
        return;

    dirAssStart_ = dirAssEnd_;
    dirAssEnd_++;

    XRCCTRL(*this,"AssScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);

    directAss();
    assErrorDisplay("");
}

void DebugWindow::onProfilerSpinPageDown(wxScrollEvent&WXUNUSED(event))
{
    if (!computerRunning_)
        return;

    dirAssStart_ = dirAssEnd_;
    dirAssEnd_++;

    XRCCTRL(*this,"ProfilerScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);

    directAss();
    assErrorDisplay("");
}

void DebugWindow::onAssSpinPageDown()
{
    dirAssStart_ = dirAssEnd_;
    dirAssEnd_++;

    if (selectedTab_ == PROFILERTAB)
        XRCCTRL(*this,"ProfilerScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);
    else
        XRCCTRL(*this,"AssScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);

    directAss();
    assErrorDisplay("");
}

void DebugWindow::onAssSpinPageUp(wxSpinEvent&WXUNUSED(event))
{
    if (!computerRunning_)
        return;

    int numberOfDebugLines;
    if (selectedTab_ == PROFILERTAB)
        numberOfDebugLines = numberOfDebugLines_-PROFILER_OFFSET;
    else
        numberOfDebugLines = numberOfDebugLines_;

    for (int i=0; i<numberOfDebugLines; i++)
        assSpinUp();

    directAss();
    assErrorDisplay("");
}

void DebugWindow::onAssSpinPageUp(wxScrollEvent&WXUNUSED(event))
{
    if (!computerRunning_)
        return;

    int numberOfDebugLines;
    if (selectedTab_ == PROFILERTAB)
        numberOfDebugLines = numberOfDebugLines_-PROFILER_OFFSET;
    else
        numberOfDebugLines = numberOfDebugLines_;

    for (int i=0; i<numberOfDebugLines; i++)
        assSpinUp();

    XRCCTRL(*this,"AssScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);

    directAss();
    assErrorDisplay("");
}

void DebugWindow::onProfilerSpinPageUp(wxScrollEvent&WXUNUSED(event))
{
    if (!computerRunning_)
        return;

    int numberOfDebugLines;
    if (selectedTab_ == PROFILERTAB)
        numberOfDebugLines = numberOfDebugLines_-PROFILER_OFFSET;
    else
        numberOfDebugLines = numberOfDebugLines_;

    for (int i=0; i<numberOfDebugLines; i++)
        assSpinUp();

    XRCCTRL(*this,"ProfilerScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);

    directAss();
    assErrorDisplay("");
}

void DebugWindow::onAssSpinPageUp()
{
    int numberOfDebugLines;
    if (selectedTab_ == PROFILERTAB)
        numberOfDebugLines = numberOfDebugLines_-PROFILER_OFFSET;
    else
        numberOfDebugLines = numberOfDebugLines_;

    for (int i=0; i<numberOfDebugLines; i++)
        assSpinUp();

    if (selectedTab_ == PROFILERTAB)
        XRCCTRL(*this,"ProfilerScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);
    else
        XRCCTRL(*this,"AssScrollbar",wxScrollBar)->SetThumbPosition((int)dirAssStart_);

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
    XRCCTRL(*this, "AssStore", wxButton)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);
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

void DebugWindow::onProfilerDataView(wxCommandEvent&event)
{
    if (event.GetSelection() == 0)
        dataViewProfiler = true;
    else
        dataViewProfiler = false;

    if (!computerRunning_)
        return;

//    dirAssStart_ = dirAssAddress_;

//    for (int i=0; i<EDIT_ROW; i++)
//        assSpinUp();

    directAss();
}

int DebugWindow::markType(long *addrLong, int type)
{
    Word address = (Word)*addrLong;
    uint64_t executed;
    Byte command;
    int bytes;
    switch (type)
    {
        case 0:
            p_Computer->writeMemDataType(address, MEM_TYPE_OPCODE);
            switch (cpuType_)
            {
                case SYSTEM00:
                    bytes =    numberOfBytesSystem00[p_Computer->readMemDebug(address)];
                break;

                case CPU1801:
                    bytes =    numberOfBytes1801[p_Computer->readMemDebug(address)];
                break;

                case CPU1802:
                    bytes =    numberOfBytes1802[p_Computer->readMemDebug(address)];
                break;

                default:
                    bytes =    numberOfBytes1802[p_Computer->readMemDebug(address)];
                    if (bytes == 0)
                        bytes =    numberOfBytes1806[p_Computer->readMemDebug(address+1)];
                break;
            }
            if (bytes != 0)
            {
                p_Computer->writeMemDataType(address, MEM_TYPE_OPCODE);
                address++;
            }
            else
            {    assErrorDisplay(DirAssErrorCodes[ERROR_TEMP_CPU_1801-ERROR_START-1]);
                return 1;
            }
            for (int i=1; i<bytes; i++)
            {
                p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND);
            }
        break;
        case 1:
            if (p_Computer->readMemDataType(address, &executed) == MEM_TYPE_JUMP_REV || p_Computer->readMemDataType(address, &executed) == MEM_TYPE_JUMP || p_Computer->readMemDataType(address, &executed) == MEM_TYPE_OPCODE_JUMP_SLOT)
                p_Computer->writeMemDataType(address++, MEM_TYPE_DATA);
            p_Computer->writeMemDataType(address++, MEM_TYPE_DATA);
        break;
        case 2:
            if (p_Computer->readMemDebug(address) < 0x20)
                p_Computer->writeMemDataType(address, MEM_TYPE_DATA);
            else
                p_Computer->writeMemDataType(address, MEM_TYPE_TEXT);

            if (p_Computer->readMemDataType(address, &executed) == MEM_TYPE_JUMP_REV || p_Computer->readMemDataType(address, &executed) == MEM_TYPE_JUMP || p_Computer->readMemDataType(address, &executed) == MEM_TYPE_OPCODE_JUMP_SLOT)
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
            if (p_Computer->readMemDebug(address) != 0xF8 || (p_Computer->readMemDebug(address+2)&0xF0) != 0xB0 || p_Computer->readMemDebug(address+3) != 0xF8 || (p_Computer->readMemDebug(address+5)&0xF0) != 0xA0)
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
            if (p_Computer->readMemDebug(address) != 0xF8 || (p_Computer->readMemDebug(address+5)&0xF0) != 0xB0 || p_Computer->readMemDebug(address+3) != 0xF8 || (p_Computer->readMemDebug(address+2)&0xF0) != 0xA0)
            {
                assErrorDisplay(DirAssErrorCodes[ERROR_MACRO_NOT_FOUND-ERROR_START-1]);
                return 1;
            }
            p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_LDRL);
            p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND);
            p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND_LDR_2);
            p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND_LDR_3);
            p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND);
            p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND_LDR_5);
        break;
        case 7:
            if (p_Computer->readMemDebug(address) != 0xF8 || (p_Computer->readMemDebug(address+2)&0xF0) != 0xB0 || p_Computer->readMemDebug(address+3) != 0xF8 || (p_Computer->readMemDebug(address+5)&0xF0) != 0xA0)
            {    assErrorDisplay(DirAssErrorCodes[ERROR_MACRO_NOT_FOUND-ERROR_START-1]);
                return 1;
            }
            p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_LDV);
            p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND);
            p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND_LD_2);
            p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND_LD_3);
            p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND);
            p_Computer->writeMemDataType(address++, MEM_TYPE_OPERAND_LD_5);
        break;
        case 8:
            switch (p_Computer->readMemDebug(address))
            {
                case 0x76:
                    if (p_Computer->readMemDataType(address, &executed)== MEM_TYPE_OPCODE_RSHR)
                        p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE);
                    else
                        p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_RSHR);
                break;

                case 0x7e:
                    if (p_Computer->readMemDataType(address, &executed)== MEM_TYPE_OPCODE_RSHL)
                        p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE);
                    else
                        p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_RSHL);
                break;

                case 0x33:
                    if (p_Computer->readMemDataType(address, &executed)== MEM_TYPE_OPCODE_BPZ)
                        p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_BGE);
                    else
                    {
                        if (p_Computer->readMemDataType(address, &executed)== MEM_TYPE_OPCODE_BGE)
                            p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE);
                        else
                            p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_BPZ);
                    }
                break;

                case 0x3b:
                    if (p_Computer->readMemDataType(address, &executed)== MEM_TYPE_OPCODE_BM)
                        p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_BL);
                    else
                    {
                        if (p_Computer->readMemDataType(address, &executed)== MEM_TYPE_OPCODE_BL)
                            p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE);
                        else
                            p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_BM);
                    }
                break;

                case 0xc8:
                    if (p_Computer->readMemDataType(address, &executed)== MEM_TYPE_OPCODE_LSKP)
                        p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE);
                    else
                        p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_LSKP);
                break;

                case 0x38:
                    if (p_Computer->readMemDataType(address, &executed)== MEM_TYPE_OPCODE_SKP)
                        p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE);
                    else
                        p_Computer->writeMemDataType(address++, MEM_TYPE_OPCODE_SKP);
                break;

                case 0x68:
                    if ((p_Computer->readMemDebug(address+1)&0xf0) == 0xc0)
                    {
                        if (p_Computer->readMemDataType(address, &executed)== MEM_TYPE_OPCODE_RLDL)
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
        case 9:
            command = p_Computer->readMemDebug(address);
            p_Computer->writeMemDataType(address++, MEM_TYPE_PSEUDO_1);

            if (!checkSingleCommand(command))
                p_Computer->writeMemDataType(address++, MEM_TYPE_PSEUDO_2);

            if (checkTrippleCommand(command))
                p_Computer->writeMemDataType(address++, MEM_TYPE_PSEUDO_2);
            
            if (checkQuadrupleCommand(command))
            {
                p_Computer->writeMemDataType(address++, MEM_TYPE_PSEUDO_2);
                p_Computer->writeMemDataType(address++, MEM_TYPE_PSEUDO_2);
            }
        break;
    }
    Word clearAddress = address;
    while (p_Computer->readMemDataType(clearAddress, &executed) == MEM_TYPE_OPERAND || p_Computer->readMemDataType(clearAddress, &executed) == MEM_TYPE_PSEUDO_2 || p_Computer->readMemDataType(clearAddress, &executed) == MEM_TYPE_OPERAND_LD_2 || p_Computer->readMemDataType(clearAddress, &executed) == MEM_TYPE_OPERAND_LD_3 || p_Computer->readMemDataType(clearAddress, &executed) == MEM_TYPE_OPERAND_LD_5 || p_Computer->readMemDataType(clearAddress, &executed) == MEM_TYPE_OPERAND_LDR_2 || p_Computer->readMemDataType(clearAddress, &executed) == MEM_TYPE_OPERAND_LDR_3 || p_Computer->readMemDataType(clearAddress, &executed) == MEM_TYPE_OPERAND_LDR_5)
        p_Computer->writeMemDataType(clearAddress++, MEM_TYPE_DATA);
    setMemLabel((Word)*addrLong, false);
    return 0;
}

void DebugWindow::onSaveDebugFile(wxCommandEvent&event)
{
    saveDebugFile_ = event.IsChecked();
}

void DebugWindow::onLaptimeTrigger(wxCommandEvent&event)
{
    lapTimeTrigger_ = event.GetSelection();
}

void DebugWindow::onClearErrorLog(wxCommandEvent&WXUNUSED(event))
{
    assErrorWindowPointer->Clear();
}

Byte DebugWindow::getOut1()
{
    Byte out1 = 0x10;

    if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
        out1 = p_Computer->getOutValue(computerConfiguration.slotConfiguration.output);
    else
    {
        if (computerConfiguration.emsMemoryConfiguration.size() != 0)
            out1 = p_Computer->getEmsPage(emsNumber_);
    }
    return out1;
}

void DebugWindow::setOut1(Byte out1)
{
    if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
        p_Computer->slotOut(out1);
    else
    {
        if (computerConfiguration.emsMemoryConfiguration.size() != 0)
            p_Computer->setEmsPage(emsNumber_, out1);
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
    uint64_t executed;
    Byte branchType=LABEL_TYPE_NONE;
        
    for (int i=0; i<lastRange_; i++)
    {
        setOut1(dirAssSlotVector[i]);
        for (int addr=dirAssProgramStartVector[i]; addr<=dirAssDataEndVector[i]; addr++)
        {
            branchAddr = -1;
            switch (p_Computer->readMemDataType(addr, &executed))
            {
                case MEM_TYPE_OPCODE:
                case MEM_TYPE_OPCODE_BPZ:
                case MEM_TYPE_OPCODE_BGE:
                case MEM_TYPE_OPCODE_BM:
                case MEM_TYPE_OPCODE_BL:
                    switch(jumpCorrection[p_Computer->readMemDebug(addr)])
                    {
                        case 1: // short branch
                            branchAddr = ((addr+1)&0xff00)+p_Computer->readMemDebug(addr+1);
                            foundAddr = addr;
                            addr++;
                            branchType = LABEL_TYPE_BRANCH;
                        break;
                        
                        case 2: // long branch
                            branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+2);
                            foundAddr = addr;
                            addr+=2;
                            branchType = LABEL_TYPE_JUMP;
                        break;

                        case 3:
                            if ((p_Computer->readMemDebug(addr+1)&0xf0) == 0x20)
                            {   // SCAL RN,xxxx + DBNZ
                                branchAddr = (p_Computer->readMemDebug(addr+2) << 8) +  p_Computer->readMemDebug(addr+3);
                                foundAddr = addr;
                                addr+=3;
                                branchType = LABEL_TYPE_JUMP;
                            }
                            if ((p_Computer->readMemDebug(addr+1)&0xf0) == 0x80)
                            {   // SCAL RN,xxxx + DBNZ
                                branchAddr = (p_Computer->readMemDebug(addr+2) << 8) +  p_Computer->readMemDebug(addr+3);
                                foundAddr = addr;
                                addr+=3;
                                branchType = LABEL_TYPE_SUB;
                            }
                            if (p_Computer->readMemDebug(addr+1) == 0x3e || p_Computer->readMemDebug(addr+1) == 0x3f)
                            {
                                branchAddr = ((addr+2)&0xff00)+p_Computer->readMemDebug(addr+2);
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
                    branchAddr = (p_Computer->readMemDebug(addr) << 8) +  p_Computer->readMemDebug(addr+1);
                    if (!branchChangeNeeded(i, addr, branchAddr))
                        branchAddr = -1;
                    foundAddr = addr;
                    addr++;
                    branchType = LABEL_TYPE_SUB;
                break;

                case MEM_TYPE_JUMP_REV:
                    branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr);
                    if (!branchChangeNeeded(i, addr, branchAddr))
                        branchAddr = -1;
                    foundAddr = addr;
                    addr++;
                    branchType = LABEL_TYPE_SUB;
                break;

                case MEM_TYPE_OPCODE_JUMP_SLOT:
                    branchAddr = (p_Computer->readMemDebug(addr) << 8) +  p_Computer->readMemDebug(addr+1);
                    if (!branchChangeNeeded(i, addr, branchAddr))
                        branchAddr = -1;
                    foundAddr = addr;
                    addr++;
                    branchType = LABEL_TYPE_SUB;
                break;

                case MEM_TYPE_OPCODE_LBR_SLOT:
                    branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+2);
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

                    if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
                    {
                        if (addr >= computerConfiguration.slotConfiguration.start && addr <= computerConfiguration.slotConfiguration.end)
                            text.Printf("%04X (Slot: %02X)", foundAddr, dirAssSlotVector[i]);
                        else
                            text.Printf("%04X", foundAddr);
                    }
                    else
                    {
                        if (computerConfiguration.emsMemoryConfiguration.size() != 0)
                        {
                            for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = computerConfiguration.emsMemoryConfiguration.begin (); emsConfig != computerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
                            {
                                for (std::vector<WordRange>::iterator range = emsConfig->range.begin (); range != emsConfig->range.end (); ++range)
                                {
                                    if (addr >= range->start && addr <= range->end)
                                        text.Printf("%04X (Page: %02X)", foundAddr, dirAssSlotVector[i]);
                                    else
                                        text.Printf("%04X", foundAddr);
                                }
                            }
                        }
                        else
                            text.Printf("%04X", foundAddr);
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
    uint64_t executed;
    
    for (int i=0; i<lastRange_; i++)
    {
        setOut1(dirAssSlotVector[i]);
        for (int addr=dirAssProgramStartVector[i]; addr<=dirAssDataEndVector[i]; addr++)
        {
            loadAddr = -1;
            foundAddr = -1;
            leader = "";

            switch (p_Computer->readMemDataType(addr, &executed))
            {
                case MEM_TYPE_OPCODE_LDL:
                    loadAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+4);
                    if (!branchChangeNeeded(i, addr, loadAddr))
                        loadAddr = -1;
                    leader ="LDL: ";
                    if (loadAddr == dirAssAddress_)
                        foundAddr = addr;
                    addr+=5;
                    branchType = LABEL_TYPE_SUB;
                break;

                case MEM_TYPE_OPCODE_LDL_SLOT:
                    loadAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+4);
                    if (!branchChangeNeeded(i, addr, loadAddr))
                        loadAddr = -1;
                    leader ="LDL: ";
                    if (loadAddr == dirAssAddress_)
                        foundAddr = addr;
                    addr+=5;
                    branchType = LABEL_TYPE_SUB;
                break;

                case MEM_TYPE_OPCODE_LDRL:
                    loadAddr = (p_Computer->readMemDebug(addr+4) << 8) +  p_Computer->readMemDebug(addr+1);
                    if (!branchChangeNeeded(i, addr, loadAddr))
                        loadAddr = -1;
                    leader ="LDRL: ";
                    if (loadAddr == dirAssAddress_)
                        foundAddr = addr;
                    addr+=5;
                    branchType = LABEL_TYPE_SUB;
                break;

                case MEM_TYPE_OPCODE_LDRL_SLOT:
                    loadAddr = (p_Computer->readMemDebug(addr+4) << 8) +  p_Computer->readMemDebug(addr+1);
                    if (!branchChangeNeeded(i, addr, loadAddr))
                        loadAddr = -1;
                    leader ="LDRL: ";
                    if (loadAddr == dirAssAddress_)
                        foundAddr = addr;
                    addr+=5;
                    branchType = LABEL_TYPE_SUB;
                break;

                case MEM_TYPE_OPCODE_RLDL:
                    loadAddr = (p_Computer->readMemDebug(addr+2) << 8) +  p_Computer->readMemDebug(addr+3);
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

                    if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
                    {
                        if (addr >= computerConfiguration.slotConfiguration.start && addr <= computerConfiguration.slotConfiguration.end)
                            text.Printf(leader+"%04X (Slot: %02X)", foundAddr, dirAssSlotVector[i]);
                        else
                            text.Printf(leader+"%04X", foundAddr);
                    }
                    else
                    {
                        if (computerConfiguration.emsMemoryConfiguration.size() != 0)
                        {
                            for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = computerConfiguration.emsMemoryConfiguration.begin (); emsConfig != computerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
                            {
                                for (std::vector<WordRange>::iterator range = emsConfig->range.begin (); range != emsConfig->range.end (); ++range)
                                {
                                    if (addr >= range->start && addr <= range->end)
                                        text.Printf(leader+"%04X (Page: %02X)", foundAddr, dirAssSlotVector[i]);
                                    else
                                        text.Printf(leader+"%04X", foundAddr);
                                }
                            }
                        }
                        else
                            text.Printf(leader+"%04X", foundAddr);
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
    uint64_t executed;

    for (int i=0; i<lastRange_; i++)
    {
        setOut1(dirAssSlotVector[i]);
        for (int addr=dirAssProgramStartVector[i]; addr<=dirAssDataEndVector[i]; addr++)
        {
            loadAddr = -1;
            foundAddr = -1;
            leader = "";

            switch (p_Computer->readMemDataType(addr, &executed))
            {
                case MEM_TYPE_OPCODE:
                    if (p_Computer->readMemDebug(addr) == 0xf8 && (p_Computer->readMemDebug(addr+2)&0xf0) == 0xb0 && p_Computer->readMemDebug(addr+3) == 0xf8 && (p_Computer->readMemDebug(addr+5)&0xf0) == 0xa0 && (p_Computer->readMemDebug(addr+5)&0xf) == (p_Computer->readMemDebug(addr+2)&0xf))
                    {
                        loadAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+4);
                        if (!branchChangeNeeded(i, addr, loadAddr))
                            loadAddr = -1;
                        leader ="LD H-L: ";
                        if (loadAddr == dirAssAddress_)
                        {
                            foundAddr = addr;
                            addr+=5;
                        }
                    }

                    if (p_Computer->readMemDebug(addr) == 0xf8 && (p_Computer->readMemDebug(addr+2)&0xf0) == 0xa0 && p_Computer->readMemDebug(addr+3) == 0xf8 && (p_Computer->readMemDebug(addr+5)&0xf0) == 0xb0 && (p_Computer->readMemDebug(addr+5)&0xf) == (p_Computer->readMemDebug(addr+2)&0xf) && foundAddr == -1)
                    {
                        loadAddr = (p_Computer->readMemDebug(addr+4) << 8) +  p_Computer->readMemDebug(addr+1);
                        if (!branchChangeNeeded(i, addr, loadAddr))
                            loadAddr = -1;
                        leader ="LD L-H: ";
                        if (loadAddr == dirAssAddress_)
                        {
                            foundAddr = addr;
                            addr+=5;
                        }
                    }

                    if (p_Computer->readMemDebug(addr) == 0x68 && (p_Computer->readMemDebug(addr+1)&0xf0) == 0xc0 && foundAddr == -1)
                    {
                        loadAddr = (p_Computer->readMemDebug(addr+2) << 8) +  p_Computer->readMemDebug(addr+3);
                        if (!branchChangeNeeded(i, addr, loadAddr))
                            loadAddr = -1;
                        leader ="RLDI: ";
                        if (loadAddr == dirAssAddress_)
                        {
                            foundAddr = addr;
                            addr+=3;
                        }
                    }

                    if (p_Computer->readMemDebug(addr) == 0xf8 && (p_Computer->readMemDebug(addr+2)&0xf0) == 0xa0 && foundAddr == -1)
                    {
                        loadAddr = p_Computer->readMemDebug(addr+1);
                        if (!branchChangeNeeded(i, addr, loadAddr))
                            loadAddr = -1;
                        leader ="LD L: ";
                        if (loadAddr == (dirAssAddress_ & 0xff))
                        {
                            foundAddr = addr;
                            addr+=1;
                        }
                    }

                    if (p_Computer->readMemDebug(addr) == 0xf8 && (p_Computer->readMemDebug(addr+2)&0xf0) == 0xb0 && foundAddr == -1)
                    {
                        loadAddr = p_Computer->readMemDebug(addr+1);
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
                    loadAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+4);
                    if (!branchChangeNeeded(i, addr, loadAddr))
                        loadAddr = -1;
                    leader ="LDV: ";
                    if (loadAddr == dirAssAddress_)
                        foundAddr = addr;
                    addr+=5;
                break;

                case MEM_TYPE_OPCODE_LDL_SLOT:
                    loadAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+4);
                    if (!branchChangeNeeded(i, addr, loadAddr))
                        loadAddr = -1;
                    leader ="LDL: ";
                    if (loadAddr == dirAssAddress_)
                        foundAddr = addr;
                    addr+=5;
                break;

                case MEM_TYPE_OPCODE_LDL:
                    loadAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+4);
                    if (!branchChangeNeeded(i, addr, loadAddr))
                        loadAddr = -1;
                    leader ="LDL: ";
                    if (loadAddr == dirAssAddress_)
                        foundAddr = addr;
                    addr+=5;
                break;

                case MEM_TYPE_OPCODE_LDRL_SLOT:
                    loadAddr = (p_Computer->readMemDebug(addr+4) << 8) +  p_Computer->readMemDebug(addr+1);
                    if (!branchChangeNeeded(i, addr, loadAddr))
                        loadAddr = -1;
                    leader ="LDRL: ";
                    if (loadAddr == dirAssAddress_)
                        foundAddr = addr;
                    addr+=5;
                break;

                case MEM_TYPE_OPCODE_LDRL:
                    loadAddr = (p_Computer->readMemDebug(addr+4) << 8) +  p_Computer->readMemDebug(addr+1);
                    if (!branchChangeNeeded(i, addr, loadAddr))
                        loadAddr = -1;
                    leader ="LDRL: ";
                    if (loadAddr == dirAssAddress_)
                        foundAddr = addr;
                    addr+=5;
                break;
                    
                case MEM_TYPE_OPCODE_RLDL:
                    loadAddr = (p_Computer->readMemDebug(addr+2) << 8) +  p_Computer->readMemDebug(addr+3);
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

                if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
                {
                    if (addr >= computerConfiguration.slotConfiguration.start && addr <= computerConfiguration.slotConfiguration.end)
                        text.Printf(leader+"%04X (Slot: %02X)", foundAddr, dirAssSlotVector[i]);
                    else
                        text.Printf(leader+"%04X", foundAddr);
                }
                else
                {
                    if (computerConfiguration.emsMemoryConfiguration.size() != 0)
                    {
                        for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = computerConfiguration.emsMemoryConfiguration.begin (); emsConfig != computerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
                        {
                            for (std::vector<WordRange>::iterator range = emsConfig->range.begin (); range != emsConfig->range.end (); ++range)
                            {
                                if (addr >= range->start && addr <= range->end)
                                    text.Printf(leader+"%04X (Page: %02X)", foundAddr, dirAssSlotVector[i]);
                                else
                                    text.Printf(leader+"%04X", foundAddr);
                            }
                        }
                    }
                    else
                        text.Printf(leader+"%04X", foundAddr);
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
            if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
            {
                if (dirAssAddress_ >= computerConfiguration.slotConfiguration.start && dirAssAddress_ <= computerConfiguration.slotConfiguration.end)
                {
                    if (p_Computer->getOutValue(computerConfiguration.slotConfiguration.output) ==  dirAssSlotVector[i])
                        workingRange_ = i;
                }
                else
                    workingRange_ = i;
            }
            else
            {
                if (computerConfiguration.emsMemoryConfiguration.size() != 0)
                {
                    for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = computerConfiguration.emsMemoryConfiguration.begin (); emsConfig != computerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
                    {
                        for (std::vector<WordRange>::iterator range = emsConfig->range.begin (); range != emsConfig->range.end (); ++range)
                        {
                            if (dirAssAddress_ >= range->start && dirAssAddress_ <= range->end)
                            {
                                if (p_Computer->getEmsPage(emsConfig) ==  dirAssSlotVector[i])
                                    workingRange_ = i;
                            }
                            else
                                workingRange_ = i;
                        }
                    }
                }
                else
                    workingRange_ = i;
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
    uint64_t executed;
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

    if (p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_DATA || p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_TEXT || p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_PSEUDO_1)
    {
        insertByte(dirAssAddress_, 0, -1, false);
        if (pseudoType_ == "CARDTRAN")
        {
            if (dirAssAddress_ <= 0x2c7 && dirAssAddress_ >= 0x200)
                insertByte(dirAssAddress_, 0, -1, true);
        }
    }
    else
        insertByte(dirAssAddress_, 0xc4, -1, false);
    shortLongBranch();
    correctionList();
    directAss();
    XRCCTRL(*this, "AssInputWindow", wxTextCtrl)->SetFocus();
}

void DebugWindow::insertByte(Word insertAddress, Byte instruction, int branchAddress, bool secondCardtranInsert)
{
    uint64_t executed;
//    if (insertAddress > dirAssDataEndVector[workingRange_] || insertAddress < dirAssProgramStartVector[workingRange_])
//        assErrorDisplay(DirAssErrorCodes[ERROR_DEBUG_ADDRESS-ERROR_START-1]);
    Byte chip8_instruction;
    Word endAddr;

    if (insertAddress > dirAssProgramEndVector[workingRange_])
        endAddr = dirAssDataEndVector[workingRange_];
    else
        endAddr = dirAssProgramEndVector[workingRange_];

    Word lastUsedAddr = endAddr;
    for (int addr=endAddr; addr>insertAddress;addr--)
    {
        if (p_Computer->readMemDebug(addr) == 0 && addr == lastUsedAddr)
            lastUsedAddr = addr-1;

        p_Computer->writeMemDebug(addr, p_Computer->readMemDebug(addr-1), true);
        p_Computer->writeMemDataType(addr, p_Computer->readMemDataType(addr - 1, &executed));
        if (addr == insertAddress + 1)
            p_Computer->writeMemLabelType(addr, LABEL_TYPE_NONE);
        else
            p_Computer->writeMemLabelType(addr, p_Computer->readMemLabelType(addr - 1));
        branchAddressTable[addr] = branchAddressTable[addr-1];
        branchAddressTableCorrection[addr] = branchAddressTableCorrection[addr-1];
    }

    if (lastUsedAddr >= endAddr - 17)
        assErrorDisplay(DirAssErrorCodes[ERROR_MEMORY_WARNING-ERROR_START-1]);

    p_Computer->writeMemDebug(insertAddress, instruction, true);
    if (instruction != 0)
        p_Computer->writeMemDataType(insertAddress, MEM_TYPE_OPCODE);
    else
        p_Computer->writeMemDataType(insertAddress, MEM_TYPE_DATA);

    if (branchAddress >= 0)
    {
        p_Computer->writeMemDebug(insertAddress+1, (branchAddress&0xff00)>>8, true);
        p_Computer->writeMemDataType(insertAddress+1, MEM_TYPE_OPERAND);
        p_Computer->writeMemDebug(insertAddress+2, branchAddress&0xff, true);
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
        switch (p_Computer->readMemDataType(addr, &executed))
        {
            case MEM_TYPE_OPCODE:
            case MEM_TYPE_OPCODE_BPZ:
            case MEM_TYPE_OPCODE_BGE:
            case MEM_TYPE_OPCODE_BM:
            case MEM_TYPE_OPCODE_BL:
                switch(jumpCorrection[p_Computer->readMemDebug(addr)])
                {
                    case 1: // short branch
                        if (insertAddress>addr)
                            branchAddr = ((addr+1)&0xff00)+p_Computer->readMemDebug(addr+1);
                        else
                            branchAddr = (addr&0xff00)+p_Computer->readMemDebug(addr+1);
                        if (branchAddr<=insertAddress)
                        {
                            if ((addr&0xff)==0xff && addr >= insertAddress)
                            {
                                changeBranch(addr, branchAddr);
                            }
                        }
                        else
                        {
                            p_Computer->writeMemDebug(addr+1, p_Computer->readMemDebug(addr+1)+1, true);
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
                        branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+2);
                        if (branchAddr>insertAddress && branchAddr < endAddr)
                        {
                            branchAddr++;
                            p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                            p_Computer->writeMemDebug(addr+2, branchAddr&0xff, true);
                        }
                    break;

                    case 3: 
                        if ((p_Computer->readMemDebug(addr+1)&0xf0) == 0x80 || (p_Computer->readMemDebug(addr+1)&0xf0) == 0x20)
                        {   // SCAL RN,xxxx + DBNZ
                            branchAddr = (p_Computer->readMemDebug(addr+2) << 8) +  p_Computer->readMemDebug(addr+3);
                            if (branchAddr>insertAddress && branchAddr < endAddr)
                            {
                                branchAddr++;
                                p_Computer->writeMemDebug(addr+2, (branchAddr&0xff00)>>8, true);
                                p_Computer->writeMemDebug(addr+3, branchAddr&0xff, true);
                            }
                        }
                        if (p_Computer->readMemDebug(addr+1) == 0x3e || p_Computer->readMemDebug(addr+1) == 0x3f)
                        {
                            if (insertAddress>addr)
                                branchAddr = ((addr+2)&0xff00)+p_Computer->readMemDebug(addr+2);
                            else
                                branchAddr = ((addr+1)&0xff00)+p_Computer->readMemDebug(addr+2);
                            if (branchAddr<=insertAddress)
                            {
                                if (((addr+1)&0xff)==0xff && addr >= insertAddress)
                                {
                                    changeBranch(addr, branchAddr);
                                }
                            }
                            else
                            {
                                p_Computer->writeMemDebug(addr+2, p_Computer->readMemDebug(addr+2)+1, true);
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
                branchAddr = (p_Computer->readMemDebug(addr) << 8) +  p_Computer->readMemDebug(addr+1);
                if (branchAddr>insertAddress && branchAddr < endAddr)
                {
                    branchAddr++;
                    p_Computer->writeMemDebug(addr, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                }
                addr++;
            break;
        
            case MEM_TYPE_JUMP_REV:
                branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr);
                if (branchAddr>insertAddress && branchAddr < endAddr)
                {
                    branchAddr++;
                    p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr, branchAddr&0xff, true);
                }
                addr++;
            break;
        
            case MEM_TYPE_OPCODE_JUMP_SLOT:
                branchAddr = (p_Computer->readMemDebug(addr) << 8) +  p_Computer->readMemDebug(addr+1);
                if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
                {
                    branchAddr++;
                    p_Computer->writeMemDebug(addr, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                }
                addr++;
            break;

            case MEM_TYPE_OPCODE_LBR_SLOT:
                branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+2);
                if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
                {
                    branchAddr++;
                    p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+2, branchAddr&0xff, true);
                }
                addr+=2;
            break;
        
            case MEM_TYPE_OPCODE_LDL:
                branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+4);
                if (branchAddr>insertAddress && branchAddr < endAddr)
                {
                    branchAddr++;
                    p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+4, branchAddr&0xff, true);
                }
                addr+=5;
            break;

            case MEM_TYPE_OPCODE_LDL_SLOT:
                branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+4);
                if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
                {
                    branchAddr++;
                    p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+4, branchAddr&0xff, true);
                }
                addr+=5;
            break;

            case MEM_TYPE_OPCODE_LDRL:
                branchAddr = (p_Computer->readMemDebug(addr+4) << 8) +  p_Computer->readMemDebug(addr+1);
                if (branchAddr>insertAddress && branchAddr < endAddr)
                {
                    branchAddr++;
                    p_Computer->writeMemDebug(addr+4, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                }
                addr+=5;
            break;

            case MEM_TYPE_OPCODE_LDRL_SLOT:
                branchAddr = (p_Computer->readMemDebug(addr+4) << 8) +  p_Computer->readMemDebug(addr+1);
                if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
                {
                    branchAddr++;
                    p_Computer->writeMemDebug(addr+4, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                }
                addr+=5;
            break;

            case  MEM_TYPE_OPCODE_RLDL:
                branchAddr = (p_Computer->readMemDebug(addr+2) << 8) +  p_Computer->readMemDebug(addr+3);
                if (branchAddr>insertAddress && branchAddr < endAddr)
                {
                    branchAddr++;
                    p_Computer->writeMemDebug(addr+2, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+3, branchAddr&0xff, true);
                }
                addr+=3;
            break;

            case MEM_TYPE_PSEUDO_1:
                chip8_instruction = p_Computer->readMemDebug(addr);

                for (size_t jumpCommandNum=0; jumpCommandNum<jumpCommandNumber_; jumpCommandNum++)
                {
                    if (pseudoType_ == "AMVBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020")
                    {
                        if (jumpOffset_[jumpCommandNum] == 10)
                        {
                            if (chip8_instruction == jumpCommand_[jumpCommandNum])
                            {
                                branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+2);
                                if (branchAddr>insertAddress && branchAddr < endAddr)
                                {
                                    branchAddr++;
                                    p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                                    p_Computer->writeMemDebug(addr+2, branchAddr&0xff, true);
                                }
                                addr+=2;
                                break;
                            }
                        }
                        else
                        {
                            if (chip8_instruction == jumpCommand_[jumpCommandNum])
                            {
                                if (insertAddress>addr)
                                    branchAddr = ((addr+1+jumpOffset_[jumpCommandNum])&0xff00)+p_Computer->readMemDebug(addr+1+jumpOffset_[jumpCommandNum]);
                                else
                                    branchAddr = ((addr+jumpOffset_[jumpCommandNum])&0xff00)+p_Computer->readMemDebug(addr+1+jumpOffset_[jumpCommandNum]);
                                if (branchAddr<=insertAddress)
                                {
                                    if (((addr+jumpOffset_[jumpCommandNum])&0xff)==0xff && (addr) >= insertAddress)
                                        branchAddressTableCorrection[addr] = true;
                                }
                                else
                                {
                                    p_Computer->writeMemDebug(addr+1+jumpOffset_[jumpCommandNum], p_Computer->readMemDebug(addr+1+jumpOffset_[jumpCommandNum])+1, true);
                                    if (((addr+jumpOffset_[jumpCommandNum])&0xff)==0xff)
                                        branchAddressTableCorrection[addr] = true;
                                    else
                                    {
                                        if ((branchAddr&0xff)==0xff)
                                            branchAddressTableCorrection[addr] = true;
                                    }
                                }
                                break;
                            }
                        }
                    }
                    else
                    {
                        if ((chip8_instruction & jumpMask_[jumpCommandNum]) == jumpCommand_[jumpCommandNum])
                        {
                            branchAddr = ((p_Computer->readMemDebug(addr)&0xf) << 8) +  p_Computer->readMemDebug(addr+1);
                            if ((branchAddr+jumpOffset_[jumpCommandNum])>insertAddress && (branchAddr+jumpOffset_[jumpCommandNum]) < endAddr)
                            {
                                branchAddr++;
                                p_Computer->writeMemDebug(addr, (chip8_instruction & 0xf0) | ((branchAddr&0xf00)>>8), true);
                                p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                            }
                            addr++;
                            break;
                        }
                    }
                }
                for (size_t jumpCommandNum=0; jumpCommandNum<branchCommandNumber_; jumpCommandNum++)
                {
                    if ((chip8_instruction & branchMask_[jumpCommandNum]) == branchCommand_[jumpCommandNum])
                    {
                        if (insertAddress>addr)
                            branchAddr = ((addr+1)&0xff00)+p_Computer->readMemDebug(addr+1);
                        else
                            branchAddr = (addr&0xff00)+p_Computer->readMemDebug(addr+1);
                        if (branchAddr<=insertAddress)
                        {
                            if ((addr&0xff)==0xff && addr >= insertAddress)
                                branchAddressTableCorrection[addr] = true;
                        }
                        else
                        {
                            p_Computer->writeMemDebug(addr+1, p_Computer->readMemDebug(addr+1)+1, true);
                            if ((addr&0xff)==0xff)
                                branchAddressTableCorrection[addr] = true;
                            else
                            {
                                if ((branchAddr&0xff)==0xff)
                                    branchAddressTableCorrection[addr] = true;
                            }
                        }
                        break;
                    }
                }
                if (secondCardtranInsert)
                {
                    for (size_t jumpCommandNum=0; jumpCommandNum<decimalBranchCommandNumber_; jumpCommandNum++)
                    {
                        if (chip8_instruction == decimalBranchCommand_[jumpCommandNum])
                        {
                            branchAddr = 0x201 + (((p_Computer->readMemDebug(addr+1) & 0xf0) >> 4) * 10 + (p_Computer->readMemDebug(addr+1) & 0xf)) * 2;
                            if (branchAddr > insertAddress && branchAddr < endAddr)
                            {
                                branchAddr++;
                                p_Computer->writeMemDebug(addr+1, getCardtranAddress(branchAddr), true);
                            }
                            addr++;
                            break;
                        }
                    }
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
            switch (p_Computer->readMemDataType(addr, &executed))
            {
                case MEM_TYPE_OPCODE:
                case MEM_TYPE_OPCODE_LBR_SLOT:
                    switch(jumpCorrection[p_Computer->readMemDebug(addr)])
                    {
                        case 2: // long branch
                            branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+2);
                            if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                            {
                                branchAddr++;
                                p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                                p_Computer->writeMemDebug(addr+2, branchAddr&0xff, true);
                            }
                            addr +=2;
                        break;

                        case 3: // SCAL RN,xxxx + DBNZ
                            if ((p_Computer->readMemDebug(addr+1)&0xf0) == 0x80 || (p_Computer->readMemDebug(addr+1)&0xf0) == 0x20)
                            {
                                branchAddr = (p_Computer->readMemDebug(addr+2) << 8) +  p_Computer->readMemDebug(addr+3);
                                if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                                {
                                    branchAddr++;
                                    p_Computer->writeMemDebug(addr+2, (branchAddr&0xff00)>>8, true);
                                    p_Computer->writeMemDebug(addr+3, branchAddr&0xff, true);
                                }
                            }
                            addr +=3;
                        break;
                    }
                break;
            
                case MEM_TYPE_JUMP:
                case MEM_TYPE_OPCODE_JUMP_SLOT:
                    branchAddr = (p_Computer->readMemDebug(addr) << 8) +  p_Computer->readMemDebug(addr+1);
                    if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                    {
                        branchAddr++;
                        p_Computer->writeMemDebug(addr, (branchAddr&0xff00)>>8, true);
                        p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                    }
                    addr++;
                break;
            
                case MEM_TYPE_JUMP_REV:
                    branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr);
                    if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                    {
                        branchAddr++;
                        p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                        p_Computer->writeMemDebug(addr, branchAddr&0xff, true);
                    }
                    addr++;
                break;
            
                case MEM_TYPE_OPCODE_LDL:
                case MEM_TYPE_OPCODE_LDL_SLOT:
                    branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+4);
                    if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                    {
                        branchAddr++;
                        p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                        p_Computer->writeMemDebug(addr+4, branchAddr&0xff, true);
                    }
                    addr+=5;
                break;
            
                case MEM_TYPE_OPCODE_LDRL:
                case MEM_TYPE_OPCODE_LDRL_SLOT:
                    branchAddr = (p_Computer->readMemDebug(addr+4) << 8) +  p_Computer->readMemDebug(addr+1);
                    if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                    {
                        branchAddr++;
                        p_Computer->writeMemDebug(addr+4, (branchAddr&0xff00)>>8, true);
                        p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                    }
                    addr+=5;
                break;
            
                case MEM_TYPE_OPCODE_RLDL:
                    branchAddr = (p_Computer->readMemDebug(addr+2) << 8) +  p_Computer->readMemDebug(addr+3);
                    if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                    {
                        branchAddr++;
                        p_Computer->writeMemDebug(addr+2, (branchAddr&0xff00)>>8, true);
                        p_Computer->writeMemDebug(addr+3, branchAddr&0xff, true);
                    }
                    addr+=3;
                break;

                case MEM_TYPE_PSEUDO_1:
                    chip8_instruction = p_Computer->readMemDebug(addr);
                    for (size_t jumpCommandNum=0; jumpCommandNum<jumpCommandNumber_; jumpCommandNum++)
                    {
                        if (jumpMask_[jumpCommandNum] == 0xffff)
                        {
                            if (chip8_instruction == jumpCommand_[jumpCommandNum])
                            {
                                branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+2);
                                if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                                {
                                    branchAddr++;
                                    p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                                    p_Computer->writeMemDebug(addr+2, branchAddr&0xff, true);
                                }
                                addr+=2;
                                break;
                            }
                        }
                        else
                        {
                            if ((chip8_instruction & jumpMask_[jumpCommandNum]) == jumpCommand_[jumpCommandNum])
                            {
                                branchAddr = ((p_Computer->readMemDebug(addr)&0xf) << 8) +  p_Computer->readMemDebug(addr+1);
                                if ((branchAddr+jumpOffset_[jumpCommandNum])>insertAddress && (branchAddr+jumpOffset_[jumpCommandNum]) < endAddr && branchChangeNeeded(i, addr, (branchAddr+jumpOffset_[jumpCommandNum])))
                                {
                                    branchAddr++;
                                    p_Computer->writeMemDebug(addr, (chip8_instruction & 0xf0) | ((branchAddr&0xf00)>>8), true);
                                    p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                                }
                                addr++;
                                break;
                            }
                        }
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
            insertByte(i, (p_Computer->readMemDebug(i) & 0x0f) | 0xc0, branchAddress, false);
        }
    }
}

bool DebugWindow::branchChangeNeeded(int rangeIndex, Word address, Word branchAddr)
{
    uint64_t executed;

    if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
    {
        if (p_Computer->readMemDataType(address, &executed) == MEM_TYPE_OPCODE_LBR_SLOT || p_Computer->readMemDataType(address, &executed) == MEM_TYPE_OPCODE_JUMP_SLOT || p_Computer->readMemDataType(address, &executed) == MEM_TYPE_OPCODE_LDL_SLOT || p_Computer->readMemDataType(address, &executed) == MEM_TYPE_OPCODE_LDRL_SLOT)
        {
            if (p_Computer->readMemDataType(address+1, &executed) == dirAssSlotVector[workingRange_] || branchAddr < computerConfiguration.slotConfiguration.start || branchAddr > computerConfiguration.slotConfiguration.end)
                return true;
        }
        else
        {
            if (dirAssSlotVector[rangeIndex] == dirAssSlotVector[workingRange_] || branchAddr < computerConfiguration.slotConfiguration.start || branchAddr > computerConfiguration.slotConfiguration.end)
                return true;
        }
    }
    else
    {
        if (computerConfiguration.emsMemoryConfiguration.size() != 0)
        {
            for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = computerConfiguration.emsMemoryConfiguration.begin (); emsConfig != computerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
            {
                if (p_Computer->readMemDataType(address, &executed) == MEM_TYPE_OPCODE_LBR_SLOT || p_Computer->readMemDataType(address, &executed) == MEM_TYPE_OPCODE_JUMP_SLOT || p_Computer->readMemDataType(address, &executed) == MEM_TYPE_OPCODE_LDL_SLOT || p_Computer->readMemDataType(address, &executed) == MEM_TYPE_OPCODE_LDRL_SLOT)
                {
                    for (std::vector<WordRange>::iterator range = emsConfig->range.begin (); range != emsConfig->range.end (); ++range)
                    {
                        if (p_Computer->readMemDataType(address+1, &executed) == dirAssSlotVector[workingRange_] || branchAddr < range->start || branchAddr >= range->end)
                            return true;
                    }
                }
                else
                {
                    for (std::vector<WordRange>::iterator range = emsConfig->range.begin (); range != emsConfig->range.end (); ++range)
                    {
                        if (dirAssSlotVector[rangeIndex] == dirAssSlotVector[workingRange_] || branchAddr < range->start || branchAddr >= range->end)
                            return true;
                    }
                }
            }
        }
        else
            return true;
    }
    return false;
}

void DebugWindow::onDelete(wxCommandEvent&WXUNUSED(event))
{
    uint64_t executed;
    
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

    if (p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPERAND_LD_3 || p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPERAND_LDR_3)
        dirAssAddress_ -= 3;

    int checkAddres = setMemLabel(dirAssAddress_, true);
    if (p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPCODE_LBR_SLOT || p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPCODE_JUMP_SLOT || p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_JUMP_REV || p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_JUMP)
    {
        deleteByte(dirAssAddress_, false);
        p_Computer->writeMemDataType(dirAssAddress_, MEM_TYPE_DATA);
        deleteByte(dirAssAddress_, false);
        if (checkAddres != -1)
            checkAddres-=2;
        if (p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPCODE_LBR_SLOT)
        {
            deleteByte(dirAssAddress_, false);
            if (checkAddres != -1)
                checkAddres--;
        }
    }
    else
    {
        if (p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPCODE || p_Computer->readMemDataType(dirAssAddress_, &executed) >= MEM_TYPE_OPCODE_RSHR)
        {
            if (p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPCODE_LDL_SLOT || p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPCODE_LDRL_SLOT )
                deleteByte(dirAssAddress_, false);
            deleteByte(dirAssAddress_, false);
            while (p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPERAND || p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPERAND_LD_2 || p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPERAND_LD_3 || p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPERAND_LD_5 || p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPERAND_LDR_2 || p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPERAND_LDR_3 || p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_OPERAND_LDR_5)
            {
                deleteByte(dirAssAddress_, false);
                if (checkAddres != -1)
                    checkAddres--;
            }
        }
        else
        {
            if (p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_PSEUDO_1)
            {
                deleteByte(dirAssAddress_, false);
                while (p_Computer->readMemDataType(dirAssAddress_, &executed) == MEM_TYPE_PSEUDO_2)
                {
                    if (pseudoType_ == "CARDTRAN")
                    {
                        if (dirAssAddress_ <= 0x2c7 && dirAssAddress_ >= 0x200)
                            deleteByte(dirAssAddress_, true);
                    }
                    else
                        deleteByte(dirAssAddress_, false);
                    if (checkAddres != -1)
                        checkAddres--;
                }
            }
            else
            {
                deleteByte(dirAssAddress_, false);
                if (pseudoType_ == "CARDTRAN")
                {
                    if (dirAssAddress_ <= 0x2c7 && dirAssAddress_ >= 0x200)
                        deleteByte(dirAssAddress_, true);
                }
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
//    if (p_Computer->readMemDataType(dirAssAddress_) == MEM_TYPE_JUMP)
//        deleteByte(dirAssAddress_);

    shortLongBranch();
    correctionList();
    directAss();
    XRCCTRL(*this, "AssInputWindow", wxTextCtrl)->SetFocus();
}

void DebugWindow::deleteByte(Word insertAddress, bool secondCardtranDelete)
{
//    if (insertAddress > dirAssDataEndVector[workingRange_] || insertAddress < dirAssProgramStartVector[workingRange_])
//        assErrorDisplay(DirAssErrorCodes[ERROR_DEBUG_ADDRESS-ERROR_START-1]);
    Byte chip8_instruction;
    Word endAddr;
    uint64_t executed;

    if (insertAddress > dirAssProgramEndVector[workingRange_])
        endAddr = dirAssDataEndVector[workingRange_];
    else
        endAddr = dirAssProgramEndVector[workingRange_];

    for (int addr=insertAddress; addr<=endAddr;addr++)
    {
        p_Computer->writeMemDebug(addr, p_Computer->readMemDebug(addr+1), true);
        p_Computer->writeMemDataType(addr, p_Computer->readMemDataType(addr+1, &executed));
        if (addr == insertAddress)
            p_Computer->writeMemLabelType(addr, p_Computer->readMemLabelType(addr) | p_Computer->readMemLabelType(addr+1));
        else
            p_Computer->writeMemLabelType(addr, p_Computer->readMemLabelType(addr+1));
        branchAddressTable[addr] = branchAddressTable[addr+1];
        branchAddressTableCorrection[addr] = branchAddressTableCorrection[addr+1];
    }
    p_Computer->writeMemDebug(endAddr, 0, true);
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
        switch (p_Computer->readMemDataType(addr, &executed))
        {
            case MEM_TYPE_OPCODE:
            case MEM_TYPE_OPCODE_BPZ:
            case MEM_TYPE_OPCODE_BGE:
            case MEM_TYPE_OPCODE_BM:
            case MEM_TYPE_OPCODE_BL:
                switch(jumpCorrection[p_Computer->readMemDebug(addr)])
                {
                    case 1: // short branch
                        if (insertAddress>addr)
                            branchAddr = ((addr+1)&0xff00)+p_Computer->readMemDebug(addr+1);
                        else
                            branchAddr = ((addr+2)&0xff00)+p_Computer->readMemDebug(addr+1);
                        if (branchAddr>insertAddress)
                        {
                            p_Computer->writeMemDebug(addr+1, p_Computer->readMemDebug(addr+1)-1, true);
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
                        branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+2);
                        if (branchAddr>insertAddress && branchAddr < endAddr)
                        {
                            branchAddr--;
                            p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                            p_Computer->writeMemDebug(addr+2, branchAddr&0xff, true);
                        }
                    break;

                    case 3: // SCAL RN,xxxx + DBNZ
                        if ((p_Computer->readMemDebug(addr+1)&0xf0) == 0x80 || (p_Computer->readMemDebug(addr+1)&0xf0) == 0x20)
                        {
                            branchAddr = (p_Computer->readMemDebug(addr+2) << 8) +  p_Computer->readMemDebug(addr+3);
                            if (branchAddr>insertAddress && branchAddr < endAddr)
                            {
                                branchAddr--;
                                p_Computer->writeMemDebug(addr+2, (branchAddr&0xff00)>>8, true);
                                p_Computer->writeMemDebug(addr+3, branchAddr&0xff, true);
                            }
                        }
                        if (p_Computer->readMemDebug(addr+1) == 0x3e || p_Computer->readMemDebug(addr+1) == 0x3f)
                        {
                            if (insertAddress>addr)
                                branchAddr = ((addr+2)&0xff00)+p_Computer->readMemDebug(addr+2);
                            else
                                branchAddr = ((addr+3)&0xff00)+p_Computer->readMemDebug(addr+2);
                            if (branchAddr>insertAddress)
                            {
                                p_Computer->writeMemDebug(addr+2, p_Computer->readMemDebug(addr+2)-1, true);
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
                branchAddr = (p_Computer->readMemDebug(addr) << 8) +  p_Computer->readMemDebug(addr+1);
                if (branchAddr>insertAddress && branchAddr < endAddr)
                {
                    branchAddr--;
                    p_Computer->writeMemDebug(addr, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                }
                addr++;
            break;

            case MEM_TYPE_JUMP_REV:
                branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr);
                if (branchAddr>insertAddress && branchAddr < endAddr)
                {
                    branchAddr--;
                    p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr, branchAddr&0xff, true);
                }
                addr++;
            break;

            case MEM_TYPE_OPCODE_JUMP_SLOT:
                branchAddr = (p_Computer->readMemDebug(addr) << 8) +  p_Computer->readMemDebug(addr+1);
                if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
                {
                    branchAddr--;
                    p_Computer->writeMemDebug(addr, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                }
                addr++;
            break;
        
            case MEM_TYPE_OPCODE_LBR_SLOT:
                branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+2);
                if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
                {
                    branchAddr--;
                    p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+2, branchAddr&0xff, true);
                }
                addr+=2;
            break;

            case MEM_TYPE_OPCODE_LDL:
                branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+4);
                if (branchAddr>insertAddress && branchAddr < endAddr)
                {
                    branchAddr--;
                    p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+4, branchAddr&0xff, true);
                }
                addr+=5;
            break;

            case MEM_TYPE_OPCODE_LDL_SLOT:
                branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+4);
                if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
                {
                    branchAddr--;
                    p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+4, branchAddr&0xff, true);
                }
                addr+=5;
            break;

            case MEM_TYPE_OPCODE_LDRL:
                branchAddr = (p_Computer->readMemDebug(addr+4) << 8) +  p_Computer->readMemDebug(addr+1);
                if (branchAddr>insertAddress && branchAddr < endAddr)
                {
                    branchAddr--;
                    p_Computer->writeMemDebug(addr+4, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                }
                addr+=5;
            break;

            case MEM_TYPE_OPCODE_LDRL_SLOT:
                branchAddr = (p_Computer->readMemDebug(addr+4) << 8) +  p_Computer->readMemDebug(addr+1);
                if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(workingRange_, addr, branchAddr))
                {
                    branchAddr--;
                    p_Computer->writeMemDebug(addr+4, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                }
                addr+=5;
            break;

            case MEM_TYPE_OPCODE_RLDL:
                branchAddr = (p_Computer->readMemDebug(addr+2) << 8) +  p_Computer->readMemDebug(addr+3);
                if (branchAddr>insertAddress && branchAddr < endAddr)
                {
                    branchAddr--;
                    p_Computer->writeMemDebug(addr+2, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(addr+3, branchAddr&0xff, true);
                }
                addr+=3;
            break;

            case MEM_TYPE_PSEUDO_1:
                chip8_instruction = p_Computer->readMemDebug(addr);
                for (size_t jumpCommandNum=0; jumpCommandNum<jumpCommandNumber_; jumpCommandNum++)
                {
                    if (pseudoType_ == "AMVBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020")
                    {
                        if (jumpOffset_[jumpCommandNum] == 10)
                        {
                            if (chip8_instruction == jumpCommand_[jumpCommandNum])
                            {
                                branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+2);
                                if (branchAddr>insertAddress && branchAddr < endAddr)
                                {
                                    branchAddr--;
                                    p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                                    p_Computer->writeMemDebug(addr+2, branchAddr&0xff, true);
                                }
                                addr+=2;
                                break;
                            }
                        }
                        else
                        {
                            if (chip8_instruction == jumpCommand_[jumpCommandNum])
                            {
                                if (insertAddress>addr)
                                    branchAddr = ((addr+1+jumpOffset_[jumpCommandNum])&0xff00)+p_Computer->readMemDebug(addr+1+jumpOffset_[jumpCommandNum]);
                                else
                                    branchAddr = ((addr+2+jumpOffset_[jumpCommandNum])&0xff00)+p_Computer->readMemDebug(addr+1+jumpOffset_[jumpCommandNum]);
                                if (branchAddr>insertAddress)
                                {
                                    p_Computer->writeMemDebug(addr+1+jumpOffset_[jumpCommandNum], p_Computer->readMemDebug(addr+1+jumpOffset_[jumpCommandNum])-1, true);
                                    if (((addr+jumpOffset_[jumpCommandNum])&0xff)==0xfe)
                                        branchAddressTableCorrection[addr] = true;
                                    else
                                    {
                                        if ((branchAddr&0xff)==0)
                                            branchAddressTableCorrection[addr] = true;
                                    }
                                }
                                break;
                            }
                        }
                    }
                    else
                    {
                        if ((chip8_instruction & jumpMask_[jumpCommandNum]) == jumpCommand_[jumpCommandNum])
                        {
                            branchAddr = ((p_Computer->readMemDebug(addr)&0xf) << 8) +  p_Computer->readMemDebug(addr+1);
                            if ((branchAddr+jumpOffset_[jumpCommandNum])>insertAddress && (branchAddr+jumpOffset_[jumpCommandNum]) < endAddr)
                            {
                                branchAddr--;
                                p_Computer->writeMemDebug(addr, (chip8_instruction & 0xf0) | ((branchAddr&0xf00)>>8), true);
                                p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                            }
                            addr++;
                            break;
                        }
                    }
                }
                for (size_t i=0; i<branchCommandNumber_; i++)
                {
                    if ((chip8_instruction & branchMask_[i]) == branchCommand_[i])
                    {
                        if (insertAddress>addr)
                            branchAddr = ((addr+1)&0xff00)+p_Computer->readMemDebug(addr+1);
                        else
                            branchAddr = ((addr+2)&0xff00)+p_Computer->readMemDebug(addr+1);
                        if (branchAddr>insertAddress)
                        {
                            p_Computer->writeMemDebug(addr+1, p_Computer->readMemDebug(addr+1)-1, true);
                            if ((addr&0xff)==0xfe)
                                branchAddressTableCorrection[addr] = true;
                            else
                            {
                                if ((branchAddr&0xff)==0)
                                    branchAddressTableCorrection[addr] = true;
                            }
                        }
                        break;
                    }
                }
                if (secondCardtranDelete)
                {
                    for (size_t jumpCommandNum=0; jumpCommandNum<decimalBranchCommandNumber_; jumpCommandNum++)
                    {
                        if (chip8_instruction == decimalBranchCommand_[jumpCommandNum])
                        {
                            branchAddr = 0x200 + (((p_Computer->readMemDebug(addr+1) & 0xf0) >> 4) * 10 + (p_Computer->readMemDebug(addr+1) & 0xf)) * 2;
                            if (branchAddr > insertAddress && branchAddr < endAddr)
                            {
                                branchAddr--;
                                p_Computer->writeMemDebug(addr+1, getCardtranAddress(branchAddr), true);
                            }
                            addr++;
                            break;
                        }
                    }
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
            switch (p_Computer->readMemDataType(addr, &executed))
            {
                case MEM_TYPE_OPCODE:
                case MEM_TYPE_OPCODE_LBR_SLOT:
                    switch(jumpCorrection[p_Computer->readMemDebug(addr)])
                    {
                        case 2: // long branch
                            branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+2);
                            if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                            {
                                branchAddr--;
                                p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                                p_Computer->writeMemDebug(addr+2, branchAddr&0xff, true);
                            }
                            addr+=2;
                        break;

                        case 3: // SCAL RN,xxxx + DBNZ
                            if ((p_Computer->readMemDebug(addr+1)&0xf0) == 0x80 || (p_Computer->readMemDebug(addr+1)&0xf0) == 0x20)
                            {
                                branchAddr = (p_Computer->readMemDebug(addr+2) << 8) +  p_Computer->readMemDebug(addr+3);
                                if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                                {
                                    branchAddr--;
                                    p_Computer->writeMemDebug(addr+2, (branchAddr&0xff00)>>8, true);
                                    p_Computer->writeMemDebug(addr+3, branchAddr&0xff, true);
                                }
                            }
                            addr+=3;
                        break;
                    }
                break;

                case MEM_TYPE_JUMP:
                case MEM_TYPE_OPCODE_JUMP_SLOT:
                    branchAddr = (p_Computer->readMemDebug(addr) << 8) +  p_Computer->readMemDebug(addr+1);
                    if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                    {
                        branchAddr--;
                        p_Computer->writeMemDebug(addr, (branchAddr&0xff00)>>8, true);
                        p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                    }
                    addr++;
                break;

                case MEM_TYPE_JUMP_REV:
                    branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr);
                    if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                    {
                        branchAddr--;
                        p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                        p_Computer->writeMemDebug(addr, branchAddr&0xff, true);
                    }
                    addr++;
                break;

                case MEM_TYPE_OPCODE_LDL:
                case MEM_TYPE_OPCODE_LDL_SLOT:
                    branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+4);
                    if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                    {
                        branchAddr--;
                        p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                        p_Computer->writeMemDebug(addr+4, branchAddr&0xff, true);
                    }
                    addr+=5;
                break;

                case MEM_TYPE_OPCODE_LDRL:
                case MEM_TYPE_OPCODE_LDRL_SLOT:
                    branchAddr = (p_Computer->readMemDebug(addr+4) << 8) +  p_Computer->readMemDebug(addr+1);
                    if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                    {
                        branchAddr--;
                        p_Computer->writeMemDebug(addr+4, (branchAddr&0xff00)>>8, true);
                        p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                    }
                    addr+=5;
                break;

                case MEM_TYPE_OPCODE_RLDL:
                    branchAddr = (p_Computer->readMemDebug(addr+2) << 8) +  p_Computer->readMemDebug(addr+3);
                    if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                    {
                        branchAddr--;
                        p_Computer->writeMemDebug(addr+2, (branchAddr&0xff00)>>8, true);
                        p_Computer->writeMemDebug(addr+3, branchAddr&0xff, true);
                    }
                    addr+=3;
                break;

                case MEM_TYPE_PSEUDO_1:
                    chip8_instruction = p_Computer->readMemDebug(addr);
                    for (size_t jumpCommandNum=0; jumpCommandNum<jumpCommandNumber_; jumpCommandNum++)
                    {
                        if (jumpMask_[jumpCommandNum] == 0xffff)
                        {
                            if (chip8_instruction == jumpCommand_[jumpCommandNum])
                            {
                                branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+2);
                                if (branchAddr>insertAddress && branchAddr < endAddr && branchChangeNeeded(i, addr, branchAddr))
                                {
                                    branchAddr--;
                                    p_Computer->writeMemDebug(addr+1, (branchAddr&0xff00)>>8, true);
                                    p_Computer->writeMemDebug(addr+2, branchAddr&0xff, true);
                                }
                                addr+=2;
                                break;
                            }
                        }
                        else
                        {
                            if ((chip8_instruction & jumpMask_[jumpCommandNum]) == jumpCommand_[jumpCommandNum])
                            {
                                branchAddr = ((p_Computer->readMemDebug(addr)&0xf) << 8) +  p_Computer->readMemDebug(addr+1);
                                if ((branchAddr+jumpOffset_[jumpCommandNum])>insertAddress && (branchAddr+jumpOffset_[jumpCommandNum]) < endAddr && branchChangeNeeded(i, addr, (branchAddr+jumpOffset_[jumpCommandNum])))
                                {
                                    branchAddr--;
                                    p_Computer->writeMemDebug(addr, (chip8_instruction & 0xf0) | ((branchAddr&0xf00)>>8), true);
                                    p_Computer->writeMemDebug(addr+1, branchAddr&0xff, true);
                                }
                                addr++;
                                break;
                            }
                        }
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
            insertByte(i, (p_Computer->readMemDebug(i) & 0x0f) | 0xc0, branchAddress, false);
        }
    }
}

void DebugWindow::shortLongBranch()
{
    Word branchAddr;
    uint64_t executed;

    for (int addr=dirAssProgramStartVector[workingRange_]; addr<=dirAssProgramEndVector[workingRange_]; addr++)
    {
        if (p_Computer->readMemDataType(addr, &executed) == MEM_TYPE_OPCODE)
        {
            switch(jumpCorrection[p_Computer->readMemDebug(addr)])
            {
                case 2: // long branch
                    branchAddr = (p_Computer->readMemDebug(addr+1) << 8) +  p_Computer->readMemDebug(addr+2);
                    if ((branchAddr&0xff00) == (addr&0xff00) && ((addr&0xff) != 0xff))
                    {
                        p_Computer->writeMemDebug(addr, (p_Computer->readMemDebug(addr) & 0x0f) | 0x30, true);
                        p_Computer->writeMemDebug(addr+1, p_Computer->readMemDebug(addr+2), true);
                        p_Computer->writeMemDebug(addr+2, 0xc4, true);
                        p_Computer->writeMemDataType(addr+2, MEM_TYPE_OPCODE);
                        deleteByte(addr+2, false);
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
    Byte instruction=p_Computer->readMemDebug(addr);

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
    if (!computerConfiguration.superBoardConfiguration.defined)
    {
        assErrorDisplay(DirAssErrorCodes[ERROR_COMX_SB_NOT_RUNNING-ERROR_START-1]);
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

    Byte out1 = p_Computer->getOutValue(1);

    Byte value;
    wxFile outputFile, outputFileBank;
    wxString fileName =  dirAssDirNameVector[0] + pathSeparator_ + "system_rom.bin";
    outputFile.Create(fileName, true);
    for (long address = 0; address <= 0x3fff; address++)
    {
        value = p_Computer->readMemDebug(address);
        outputFile.Write(&value, 1);
    }
    for (long address = 0xe000; address <= 0xe7ff; address++)
    {
        value = p_Computer->readMemDebug(address);
        outputFile.Write(&value, 1);
    }
    for (long address = 0x4800; address <= 0x67ff; address++)
    {
        value = 0xff;
        outputFile.Write(&value, 1);
    }
    for (long address = 0xe800; address <= 0xefff; address++)
    {
        value = p_Computer->readMemDebug(address);
        outputFile.Write(&value, 1);
    }
    for (long address = 0x7000; address <= 0x77ff; address++)
    {
        value = 0xff;
        outputFile.Write(&value, 1);
    }
    for (long address = 0xf800; address <= 0xffff; address++)
    {
        value = p_Computer->readMemDebug(address);
        outputFile.Write(&value, 1);
    }
    outputFile.Close();
    fileName =  dirAssDirNameVector[0] + pathSeparator_ + "usb_rom.bin";
    outputFile.Create(fileName, true);
    p_Computer->slotOut(0x10);
    for (long address = 0xc000; address <= 0xdfff; address++)
    {
        value = p_Computer->readMemDebug(address);
        outputFile.Write(&value, 1);
    }
    p_Computer->slotOut(0x30);
    for (long address = 0xc000; address <= 0xdfff; address++)
    {
        value = p_Computer->readMemDebug(address);
        outputFile.Write(&value, 1);
    }
    p_Computer->slotOut(0x50);
    for (long address = 0xc000; address <= 0xdfff; address++)
    {
        value = p_Computer->readMemDebug(address);
        outputFile.Write(&value, 1);
    }
    p_Computer->slotOut(0x70);
    for (long address = 0xc000; address <= 0xdfff; address++)
    {
        value = p_Computer->readMemDebug(address);
        outputFile.Write(&value, 1);
    }
    outputFile.Close();
    fileName =  dirAssDirNameVector[0] + pathSeparator_ + "f&m_rom_2.bin";
    outputFile.Create(fileName, true);
    p_Computer->slotOut(0x90); // 1001
    for (long address = 0xc000; address <= 0xdfff; address++)
    {
        value = p_Computer->readMemDebug(address);
        outputFile.Write(&value, 1);
    }

    p_Computer->slotOut(0xb0); // 1011
    wxString bankFileName =  dirAssDirNameVector[0] + pathSeparator_ + "sb.c000-5.bin";
    outputFileBank.Create(bankFileName, true);
    for (long address = 0xc000; address <= 0xdfff; address++)
    {
        value = p_Computer->readMemDebug(address);
        outputFile.Write(&value, 1);
        outputFileBank.Write(&value, 1);
    }
    outputFileBank.Close();

    p_Computer->slotOut(0xd0); // 1101
    for (long address = 0xc000; address <= 0xdfff; address++)
    {
        value = p_Computer->readMemDebug(address);
        outputFile.Write(&value, 1);
    }
    p_Computer->slotOut(0xf0); // 1111
    for (long address = 0xc000; address <= 0xdfff; address++)
    {
        value = p_Computer->readMemDebug(address);
        outputFile.Write(&value, 1);
    }
    outputFile.Close();

    wxString systemRomName = dirAssDirNameVector[0] + pathSeparator_ + "sb.0000.bin";
    outputFile.Create(systemRomName, true);
    for (long address = 0x0000; address <= 0x3fff; address++)
    {
        value = p_Computer->readMemDebug(address);
        outputFile.Write(&value, 1);
    }
    outputFile.Close();

    p_Computer->slotOut(out1);
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
        XRCCTRL(*this, "AssStore", wxButton)->SetBackgroundColour(guiTextColour[GUI_COL_RED]);
#else
        XRCCTRL(*this, "AssStore", wxButton)->SetForegroundColour(guiTextColour[GUI_COL_RED]);
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
    XRCCTRL(*this, "AssStore", wxButton)->SetBackgroundColour(guiTextColour[GUI_COL_RED]);
#else
    XRCCTRL(*this, "AssStore", wxButton)->SetForegroundColour(guiTextColour[GUI_COL_RED]);
#endif
/*        XRCCTRL(*this,"AssProgramSlot",wxTextCtrl)->ChangeValue("");
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
    dirAssNewDir_ = computerConfiguration.mainDir_;
    dirAssConfigFileDir_ = computerConfiguration.mainDir_;
    dirAssConfigFile_ = fileName + ".config";
    onAssStore();
}

void DebugWindow::assDefault(wxString dirName, wxString fileName, int code_start, int code_end, int end)
{
    if (!mode_.gui)
        return;

    wxString valueString;
    
    XRCCTRL(*this,"AssProgramSlot",wxTextCtrl)->ChangeValue("");
    valueString = "";
    if (code_start >= 0)
        valueString.Printf("%04X", code_start);
    XRCCTRL(*this,"AssProgramStart",wxTextCtrl)->ChangeValue(valueString);
    valueString = "";
    if (code_start >= 0)
        valueString.Printf("%04X", code_end);
    XRCCTRL(*this,"AssCodeEnd",wxTextCtrl)->ChangeValue(valueString);
    valueString = "";
    if (code_start >= 0)
        valueString.Printf("%04X", end);
    XRCCTRL(*this,"AssProgramEnd",wxTextCtrl)->ChangeValue(valueString);
    wxString value;
    value.Printf("%0d:",lastRange_+1);
    XRCCTRL(*this,"AssRangeNumber",wxStaticText)->SetLabel(value);
    XRCCTRL(*this,"AssFileName",wxTextCtrl)->ChangeValue(fileName + ".bin");
    shownRange_ = lastRange_;
    dirAssNewDir_ = dirName;
    dirAssConfigFileDir_ = dirName;
    dirAssConfigFile_ = fileName + ".config";
    onAssStore();
}

void DebugWindow::scrtValues(bool status, bool Scrt, long CallReg, long CallAddress, long RetReg, long RetAddress)
{
    if (status)
        saveScrtValues("");
    else
        setScrtValues(Scrt, CallReg, CallAddress, RetReg, RetAddress, "");

}

void DebugWindow::setScrtValues(bool Scrt, long CallReg, long CallAddress, long RetReg, long RetAddress, wxString Game)
{
    if (!mode_.gui)
        return;

    configPointer->Read("/Xml/DebugScrt", &computerConfiguration.debuggerConfiguration.mode, Scrt);
    XRCCTRL(*this,"DebugSCRT", wxCheckBox)->SetValue(computerConfiguration.debuggerConfiguration.mode);

    XRCCTRL(*this,"DebugSCRT",wxCheckBox)->Enable(true);
    if (computerConfiguration.debuggerConfiguration.mode)
    {
        XRCCTRL(*this,"DebugCallText",wxStaticText)->Enable(true);
        XRCCTRL(*this,"DebugCallReg",wxTextCtrl)->Enable(true);
        XRCCTRL(*this,"DebugCallAddress",wxTextCtrl)->Enable(true);
        XRCCTRL(*this,"DebugRetText",wxStaticText)->Enable(true);
        XRCCTRL(*this,"DebugRetReg",wxTextCtrl)->Enable(true);
        XRCCTRL(*this,"DebugRetAddress",wxTextCtrl)->Enable(true);
    }
    
    wxString valueString;
    
    computerConfiguration.debuggerConfiguration.callRegister = configPointer->Read("/Xml/DebugCallReg" + Game, CallReg);
    if (computerConfiguration.debuggerConfiguration.callRegister == -1)
        valueString = "";
    else
        valueString.Printf("%01X", (int)computerConfiguration.debuggerConfiguration.callRegister);
    XRCCTRL(*this,"DebugCallReg",wxTextCtrl)->ChangeValue(valueString);
    
    computerConfiguration.debuggerConfiguration.callAddress = configPointer->Read("/Xml/DebugCallAddress" + Game, CallAddress);
    if (computerConfiguration.debuggerConfiguration.callAddress == -1)
        valueString = "";
    else
        valueString.Printf("%04X", (int)computerConfiguration.debuggerConfiguration.callAddress);
    XRCCTRL(*this,"DebugCallAddress",wxTextCtrl)->ChangeValue(valueString);
    
    computerConfiguration.debuggerConfiguration.returnRegister = configPointer->Read("/Xml/DebugRetReg" + Game, RetReg);
    if (computerConfiguration.debuggerConfiguration.returnRegister == -1)
        valueString = "";
    else
        valueString.Printf("%01X", (int)computerConfiguration.debuggerConfiguration.returnRegister);
    
    XRCCTRL(*this,"DebugRetReg",wxTextCtrl)->ChangeValue(valueString);
    computerConfiguration.debuggerConfiguration.returnAddress = configPointer->Read("/Xml/DebugRetAddress" + Game, RetAddress);
    if (computerConfiguration.debuggerConfiguration.returnAddress == -1)
        valueString = "";
    else
        valueString.Printf("%04X", (int)computerConfiguration.debuggerConfiguration.returnAddress);
    XRCCTRL(*this,"DebugRetAddress",wxTextCtrl)->ChangeValue(valueString);
}

void DebugWindow::saveScrtValues(wxString Game)
{
    if (!mode_.gui)
        return;
    
    configPointer->Write("/Xml/DebugScrt", computerConfiguration.debuggerConfiguration.mode);
    configPointer->Write("/Xml/DebugCallReg" + Game, computerConfiguration.debuggerConfiguration.callRegister);
    configPointer->Write("/Xml/DebugCallAddress" + Game, computerConfiguration.debuggerConfiguration.callAddress);
    configPointer->Write("/Xml/DebugRetReg" + Game, computerConfiguration.debuggerConfiguration.returnRegister);
    configPointer->Write("/Xml/DebugRetAddress" + Game, computerConfiguration.debuggerConfiguration.returnAddress);

    XRCCTRL(*this,"DebugSCRT",wxCheckBox)->Enable(false);
    XRCCTRL(*this,"DebugCallText",wxStaticText)->Enable(false);
    XRCCTRL(*this,"DebugCallReg",wxTextCtrl)->Enable(false);
    XRCCTRL(*this,"DebugCallAddress",wxTextCtrl)->Enable(false);
    XRCCTRL(*this,"DebugRetText",wxStaticText)->Enable(false);
    XRCCTRL(*this,"DebugRetReg",wxTextCtrl)->Enable(false);
    XRCCTRL(*this,"DebugRetAddress",wxTextCtrl)->Enable(false);
}

void DebugWindow::onDebugScrt(wxCommandEvent&event)
{
    computerConfiguration.debuggerConfiguration.mode = event.IsChecked();
    
    if (computerRunning_)
    {
        XRCCTRL(*this,"DebugCallText",wxStaticText)->Enable(computerConfiguration.debuggerConfiguration.mode);
        XRCCTRL(*this,"DebugCallReg",wxTextCtrl)->Enable(computerConfiguration.debuggerConfiguration.mode);
        XRCCTRL(*this,"DebugCallAddress",wxTextCtrl)->Enable(computerConfiguration.debuggerConfiguration.mode);
        XRCCTRL(*this,"DebugRetText",wxStaticText)->Enable(computerConfiguration.debuggerConfiguration.mode);
        XRCCTRL(*this,"DebugRetReg",wxTextCtrl)->Enable(computerConfiguration.debuggerConfiguration.mode);
        XRCCTRL(*this,"DebugRetAddress",wxTextCtrl)->Enable(computerConfiguration.debuggerConfiguration.mode);
    }
}

void DebugWindow::onDebugCallReg(wxCommandEvent& WXUNUSED(event))
{
    long value = get8BitValue("DebugCallReg");
    
    computerConfiguration.debuggerConfiguration.callRegister = value;
}

void DebugWindow::onDebugCallAddress(wxCommandEvent& WXUNUSED(event))
{
    long value = get16BitValue("DebugCallAddress");
    
    computerConfiguration.debuggerConfiguration.callAddress = value;
}

void DebugWindow::onDebugRetReg(wxCommandEvent& WXUNUSED(event))
{
    long value = get8BitValue("DebugRetReg");
    
    computerConfiguration.debuggerConfiguration.returnRegister = value;
}

void DebugWindow::onDebugRetAddress(wxCommandEvent& WXUNUSED(event))
{
    long value = get16BitValue("DebugRetAddress");
    
    computerConfiguration.debuggerConfiguration.returnAddress = value;
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
        if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
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

    if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
    {
        if (start >= computerConfiguration.slotConfiguration.start && start <= computerConfiguration.slotConfiguration.end)
        {
            if (codeEnd > computerConfiguration.slotConfiguration.end || end > computerConfiguration.slotConfiguration.end)
            {
                assErrorDisplay(DirAssErrorCodes[ERROR_SLOT_RANGE-ERROR_START-1]);
                return;
            }
        }
        if (codeEnd >= computerConfiguration.slotConfiguration.start && codeEnd <= computerConfiguration.slotConfiguration.end)
        {
            if (start < computerConfiguration.slotConfiguration.start || end > computerConfiguration.slotConfiguration.end)
            {
                assErrorDisplay(DirAssErrorCodes[ERROR_SLOT_RANGE-ERROR_START-1]);
                return;
            }
        }
        if (end >= computerConfiguration.slotConfiguration.start && end <= computerConfiguration.slotConfiguration.end)
        {
            if (start < computerConfiguration.slotConfiguration.start || codeEnd < computerConfiguration.slotConfiguration.start)
            {
                assErrorDisplay(DirAssErrorCodes[ERROR_SLOT_RANGE-ERROR_START-1]);
                return;
            }
        }
    }
    else
    {
        if (computerConfiguration.emsMemoryConfiguration.size() != 0)
        {
            for (std::vector<EmsMemoryConfiguration>::iterator emsConfig = computerConfiguration.emsMemoryConfiguration.begin (); emsConfig != computerConfiguration.emsMemoryConfiguration.end (); ++emsConfig)
            {
                for (std::vector<WordRange>::iterator range = emsConfig->range.begin (); range != emsConfig->range.end (); ++range)
                {
                    if (start >= range->start && start <= range->end)
                    {
                        if (codeEnd >= range->end || end >= range->end)
                        {
                            assErrorDisplay(DirAssErrorCodes[ERROR_SLOT_RANGE-ERROR_START-1]);
                            return;
                        }
                    }
                    if (codeEnd >= range->start && codeEnd < range->end)
                    {
                        if (start < range->start || end >= range->end)
                        {
                            assErrorDisplay(DirAssErrorCodes[ERROR_SLOT_RANGE-ERROR_START-1]);
                            return;
                        }
                    }
                    if (end >= range->start && end < range->end)
                    {
                        if (start < range->start || codeEnd < range->start)
                        {
                            assErrorDisplay(DirAssErrorCodes[ERROR_SLOT_RANGE-ERROR_START-1]);
                            return;
                        }
                    }
                }
            }
        }
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
    XRCCTRL(*this, "AssStore", wxButton)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);
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
        value = p_Computer->readMemDebug(address);
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
//        if (dirAssFileNameVector[range] == "sb.0000.bin")
//            number+=2;
//        if (dirAssFileNameVector[range] == "sb.e000.bin")
//            number-=3;
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
    if (computerRunning_ && computerConfiguration.debuggerConfiguration.videoLog_defined)
    {
        computerConfiguration.debuggerConfiguration.videoLog_active = true;
        wxString fileName = debugDir_ + "debug.log";
        
        int num = 0;
        wxString number;
        while(wxFile::Exists(fileName))
        {
            num++;
            number.Printf("%d", num);
            fileName = debugDir_ + "debug." + number + ".log";
        }
        dirAssLogFile_.Create(fileName);
        writingToLog_ = false;
    }
    else
        computerConfiguration.debuggerConfiguration.videoLog_active = false;
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
    uint64_t executed;

    if ((moveCorrection &0xff) == 0)
    {
        if (destination <= start)
        {
            while(address <= end)
            {
                p_Computer->writeMemDebug(destination, p_Computer->readMemDebug(address), true);
                p_Computer->writeMemDataType(destination++, p_Computer->readMemDataType(address++, &executed));
            }
        }
        else
        {
            address = end;
            destination += length;
            while(address >= start)
            {
                p_Computer->writeMemDebug(destination, p_Computer->readMemDebug(address), true);
                p_Computer->writeMemDataType(destination--, p_Computer->readMemDataType(address--, &executed));
            }
            destination += length;
        }

        Word branchAddr;
        while (correctAddress <= destination)
        {
            if (p_Computer->readMemDataType(correctAddress, &executed) == MEM_TYPE_OPCODE || p_Computer->readMemDataType(correctAddress, &executed) == MEM_TYPE_OPCODE_LBR_SLOT)
            {
                if (jumpCorrection[p_Computer->readMemDebug(correctAddress)] == 2) // long branch
                {
                    branchAddr = (p_Computer->readMemDebug(correctAddress+1) << 8) +  p_Computer->readMemDebug(correctAddress+2);
                    if (branchAddr >= start && branchAddr <= end)
                    {
                        branchAddr += moveCorrection;
                        p_Computer->writeMemDebug(correctAddress+1, (branchAddr&0xff00)>>8, true);
                        p_Computer->writeMemDebug(correctAddress+2, branchAddr&0xff, true);
                    }
                }
                if (jumpCorrection[p_Computer->readMemDebug(correctAddress)] == 3) // SCAL RN,xxxx + DBNZ
                {
                    if ((p_Computer->readMemDebug(correctAddress+1)&0xf0) == 0x80 || (p_Computer->readMemDebug(correctAddress+1)&0xf0) == 0x20)
                    {
                        branchAddr = (p_Computer->readMemDebug(correctAddress+2) << 8) +  p_Computer->readMemDebug(correctAddress+3);
                        if (branchAddr >= start && branchAddr <= end)
                        {
                            branchAddr += moveCorrection;
                            p_Computer->writeMemDebug(correctAddress+2, (branchAddr&0xff00)>>8, true);
                            p_Computer->writeMemDebug(correctAddress+3, branchAddr&0xff, true);
                        }
                    }
                }
            }    
            if (p_Computer->readMemDataType(correctAddress, &executed) == MEM_TYPE_JUMP || p_Computer->readMemDataType(correctAddress, &executed) == MEM_TYPE_OPCODE_JUMP_SLOT)
            {
                branchAddr = (p_Computer->readMemDebug(correctAddress) << 8) +  p_Computer->readMemDebug(correctAddress+1);
                if (branchAddr >= start && branchAddr <= end)
                {
                    branchAddr += moveCorrection;
                    p_Computer->writeMemDebug(correctAddress, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(correctAddress+1, branchAddr&0xff, true);
                }
            }
            if (p_Computer->readMemDataType(correctAddress, &executed) == MEM_TYPE_JUMP_REV)
            {
                branchAddr = (p_Computer->readMemDebug(correctAddress+1) << 8) +  p_Computer->readMemDebug(correctAddress);
                if (branchAddr >= start && branchAddr <= end)
                {
                    branchAddr += moveCorrection;
                    p_Computer->writeMemDebug(correctAddress+1, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(correctAddress, branchAddr&0xff, true);
                }
            }
            if (p_Computer->readMemDataType(correctAddress, &executed) == MEM_TYPE_OPCODE_LDL || p_Computer->readMemDataType(correctAddress, &executed) == MEM_TYPE_OPCODE_LDL_SLOT)
            {
                branchAddr = (p_Computer->readMemDebug(correctAddress+1) << 8) +  p_Computer->readMemDebug(correctAddress+4);
                if (branchAddr >= start && branchAddr <= end)
                {
                    branchAddr += moveCorrection;
                    p_Computer->writeMemDebug(correctAddress+1, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(correctAddress+4, branchAddr&0xff, true);
                }
            }
            if (p_Computer->readMemDataType(correctAddress, &executed) == MEM_TYPE_OPCODE_LDRL || p_Computer->readMemDataType(correctAddress, &executed) == MEM_TYPE_OPCODE_LDRL_SLOT)
            {
                branchAddr = (p_Computer->readMemDebug(correctAddress+4) << 8) +  p_Computer->readMemDebug(correctAddress+1);
                if (branchAddr >= start && branchAddr <= end)
                {
                    branchAddr += moveCorrection;
                    p_Computer->writeMemDebug(correctAddress+4, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(correctAddress+1, branchAddr&0xff, true);
                }
            }
            if (p_Computer->readMemDataType(correctAddress, &executed) == MEM_TYPE_OPCODE_RLDL)
            {
                branchAddr = (p_Computer->readMemDebug(correctAddress+2) << 8) +  p_Computer->readMemDebug(correctAddress+3);
                if (branchAddr >= start && branchAddr <= end)
                {
                    branchAddr += moveCorrection;
                    p_Computer->writeMemDebug(correctAddress+2, (branchAddr&0xff00)>>8, true);
                    p_Computer->writeMemDebug(correctAddress+3, branchAddr&0xff, true);
                }
            }
            if (p_Computer->readMemDataType(correctAddress, &executed) == MEM_TYPE_PSEUDO_1)
            {
                chip8_instruction = p_Computer->readMemDebug(correctAddress);
                for (size_t jumpCommandNum=0; jumpCommandNum<jumpCommandNumber_; jumpCommandNum++)
                {
                    if (jumpMask_[jumpCommandNum] == 0xffff)
                    {
                        if (chip8_instruction == jumpCommand_[jumpCommandNum])
                        {
                            branchAddr = (p_Computer->readMemDebug(correctAddress+1) << 8) +  p_Computer->readMemDebug(correctAddress+2);
                            if ((branchAddr+jumpOffset_[jumpCommandNum])>start && (branchAddr+jumpOffset_[jumpCommandNum]) < end)
                            {
                                branchAddr += moveCorrection;
                                p_Computer->writeMemDebug(correctAddress+1, (branchAddr&0xff00)>>8, true);
                                p_Computer->writeMemDebug(correctAddress+2, branchAddr&0xff, true);
                            }
                            break;
                        }
                    }
                    else
                    {
                        if ((chip8_instruction & jumpMask_[jumpCommandNum]) == jumpCommand_[jumpCommandNum])
                        {
                            branchAddr = ((p_Computer->readMemDebug(correctAddress)&0xf) << 8) +  p_Computer->readMemDebug(correctAddress+1);
                            if ((branchAddr+jumpOffset_[jumpCommandNum])>start && (branchAddr+jumpOffset_[jumpCommandNum]) < end)
                            {
                                branchAddr += moveCorrection;
                                p_Computer->writeMemDebug(correctAddress, chip8_instruction | ((branchAddr&0xf00)>>8), true);
                                p_Computer->writeMemDebug(correctAddress+1, branchAddr&0xff, true);
                            }
                            break;
                        }
                    }
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
                p_Computer->writeMemDebug(destination++, p_Computer->readMemDebug(address++), true);
            }
        }
        else
        {
            address = end;
            destination += length;
            while(address >= start)
            {
                p_Computer->writeMemDebug(destination--, p_Computer->readMemDebug(address--), true);
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
                               wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_OVERWRITE_PROMPT,
                               this
                              );

    if (!fileName || fileName.empty())
        return;

    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    wxString name = FullPath.GetName();
    wxString path = FullPath.GetPath();
    wxString ext = FullPath.GetExt();

    if (ext == "txt")
    {
        assDirOld(fileName, start, end);
        return;
    }
 
    disassembleAgain_ = true;
    disassemblePass_ = 1;
    
    outputTextFile.Create(fileName);
 
    for (int i=0; i<=0xffff;i++)
    {
        labelInfo_[i].labelDefined = false;
        labelInfo_[i].branchToFound = false;
        labelInfo_[i].outOfRange = false;
    }
    
    uint64_t executed;
    
    while (disassembleAgain_ && disassemblePass_ <= 2)
    {
        disassembleAgain_ = false;
        outputTextFile.Clear();
    
        Word address = (Word)start;
    
        wxString line, tempLine, characters, newChar, text, label, addressAndOpcode;
        Byte value, command;
        Word textStart, branchAddress;
        
        line.Printf("; Origin set to %05XH, EOF = %05XH", (Word)start, (Word)end);
        outputTextFile.AddLine(line);
        line.Printf("        ORG  %05XH", (Word)start);
        outputTextFile.AddLine(line);

        outputTextFile.AddLine("");
        outputTextFile.AddLine("; CPU Type:");
        if (getCpuType() <= CPU1802)
            line.Printf("        CPU 1802");
        else
            line.Printf("        CPU 1805");
        outputTextFile.AddLine(line);
        
        outputTextFile.AddLine("");
        outputTextFile.AddLine("; Register Definitions:");
        
        for (int i=0; i<16; i++)
        {
            line.Printf("R%01X        EQU %d", i, i);
            outputTextFile.AddLine(line);
        }
        
        outputTextFile.AddLine("");
        outputTextFile.AddLine("; Start code segment");
        
        while(address <= end && address >= start)
        {
            if (start == 0 && address >= 0xfff8)
                start += 8;

            Byte memType = p_Computer->readMemDataType(address, &executed);

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
                    text.Printf(" - Slot %02X", p_Computer->readMemDataType(address+1, &executed));
                    line = cdp1802disassemble(&address, false, true, TEXT_ASSEMBLER, start, end);
                    line = line.Left(5)+line.Mid(9, 12) + "    "+line.Mid(21);
                    line.Trim();
                    line += text;
                break;
                    
                case MEM_TYPE_OPCODE_LDV:
                    addressAndOpcode.Printf(";%04X: %02X %02X %02X", address, p_Computer->readMemDebug(address), p_Computer->readMemDebug(address+1), p_Computer->readMemDebug(address+2));
                    line.Printf("LOAD R%01X,", p_Computer->readMemDebug(address+2)&0xf);
                    line += getLoadAddress(address+1);
                    while (line.Len()<= 24)
                        line += " ";
                    line += addressAndOpcode;
                    address+=3;
                    address&=0xffff;
                break;
                    
                case MEM_TYPE_OPCODE_LDL:
                    addressAndOpcode.Printf(";%04X: %02X %02X %02X", address, p_Computer->readMemDebug(address), p_Computer->readMemDebug(address+1), p_Computer->readMemDebug(address+2));
                    line.Printf("LOAD R%01X,", p_Computer->readMemDebug(address+2)&0xf);
                    line += getLoadAddressOrLabel(address+1, start, end);
                    while (line.Len()<= 24)
                        line += " ";
                    line += addressAndOpcode;
                    address+=3;
                    address&=0xffff;
                break;
                    
                case MEM_TYPE_OPCODE_LDL_SLOT:
                    addressAndOpcode.Printf(";%04X: %02X %02X %02X - Slot %02X", address, p_Computer->readMemDebug(address), p_Computer->readMemDebug(address+1), p_Computer->readMemDebug(address+2), p_Computer->readMemDataType(address+1, &executed));
                    line.Printf("LOAD R%01X,", p_Computer->readMemDebug(address+2)&0xf);
                    line = line + getLoadAddressOrLabel(address+1, start, end);
                    while (line.Len()<= 24)
                        line += " ";
                    line += addressAndOpcode;
                    address+=3;
                    address&=0xffff;
                break;
                                        
                case MEM_TYPE_OPERAND_LD_3:
                    addressAndOpcode.Printf(";      %02X %02X %02X", p_Computer->readMemDebug(address), p_Computer->readMemDebug(address+1), p_Computer->readMemDebug(address+2));
                    line = "";
                    while (line.Len()<= 24)
                        line += " ";
                    line += addressAndOpcode;
                    address+=3;
                    address&=0xffff;
                break;

                case MEM_TYPE_OPCODE_LDRL:
                case MEM_TYPE_OPCODE_LDRL_SLOT:
                case MEM_TYPE_OPERAND_LDR_3:
                    value = p_Computer->readMemDebug(address+1);
                    addressAndOpcode.Printf(";%04X: %02X %02X", address, p_Computer->readMemDebug(address), value);
                    if (value >= 0xa0)
                        text.Printf("%03XH", value);
                    else
                        text.Printf("%02XH", value);
                    line = "LDI  " + text;
                    while (line.Len()<= 24)
                        line += " ";
                    line += addressAndOpcode;
                    address+=2;
                    address&=0xffff;
                break;
                                                            
                case MEM_TYPE_OPERAND_LDR_2:
                    addressAndOpcode.Printf(";%04X: %02X", address, p_Computer->readMemDebug(address));
                    line.Printf("PLO  R%01X", p_Computer->readMemDebug(address)&0xf);
                    while (line.Len()<= 24)
                        line += " ";
                    line += addressAndOpcode;
                    address++;
                    address&=0xffff;
                break;
                    
                case MEM_TYPE_OPERAND_LDR_5:
                    addressAndOpcode.Printf(";%04X: %02X", address, p_Computer->readMemDebug(address));
                    line.Printf("PHI  R%01X", p_Computer->readMemDebug(address)&0xf);
                    while (line.Len()<= 24)
                        line += " ";
                    line += addressAndOpcode;
                    address++;
                    address&=0xffff;
                break;

                case MEM_TYPE_PSEUDO_1:
                    tempLine = pseudoDisassemble(address, false, true);
                    
                    command = p_Computer->readMemDebug(address++);
                    if (command >= 0xa0)
                        line.Printf("DB   %03XH", command);
                    else
                        line.Printf("DB   %02XH", command);

                    if (!checkSingleCommand(command))
                    {
                        value = p_Computer->readMemDebug(address++);
                        if (value >= 0xa0)
                            text.Printf(", %03XH", value);
                        else
                            text.Printf(", %02XH", value);
                        line = line + text;
                    }

                    if (checkTrippleCommand(command))
                    {
                        value = p_Computer->readMemDebug(address++);
                        if (value >= 0xa0)
                            text.Printf(", %03XH", value);
                        else
                            text.Printf(", %02XH", value);
                        line = line + text;
                    }
                    
                    if (checkQuadrupleCommand(command))
                    {
                        value = p_Computer->readMemDebug(address++);
                        if (value >= 0xa0)
                            text.Printf(", %03XH", value);
                        else
                            text.Printf(", %02XH", value);
                        line = line + text;
                        value = p_Computer->readMemDebug(address++);
                        if (value >= 0xa0)
                            text.Printf(", %03XH", value);
                        else
                            text.Printf(", %02XH", value);
                        line = line + text;
                    }

                    while (line.Len() <= 24)
                        line += " ";
                    if (pseudoType_ == "AMVBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020")
                        line = line + ";"+ tempLine.Left(6)+tempLine.Right(tempLine.Len()-15);
                    else
                        line = line + ";"+ tempLine.Left(6)+tempLine.Right(tempLine.Len()-11);

                    address&=0xffff;
                break;
                    
                case MEM_TYPE_OPCODE_JUMP_SLOT:
                    addressAndOpcode.Printf(";%04X: %02X %02X - Slot %02X", address, p_Computer->readMemDebug(address), p_Computer->readMemDebug(address + 1), p_Computer->readMemDataType(address+1, &executed));
                    line = "DW   " + getSubAddressOrLabel(address, TEXT_ASSEMBLER, start, end);
                    while (line.Len()<= 24)
                        line += " ";
                    line += addressAndOpcode;
                    address+=2;
                    address&=0xffff;
                break;
                    
                case MEM_TYPE_JUMP:
                    addressAndOpcode.Printf(";%04X: %02X %02X", address, p_Computer->readMemDebug(address), p_Computer->readMemDebug(address + 1));
                    line = "DW   " + getSubAddressOrLabel(address, TEXT_ASSEMBLER, start, end);
                    while (line.Len()<= 24)
                        line += " ";
                    line += addressAndOpcode;
                    address+=2;
                    address&=0xffff;
                break;
                    
                case MEM_TYPE_JUMP_REV:
                    branchAddress = p_Computer->readMemDebug(address+1)<<8|p_Computer->readMemDebug(address);
                    if (p_Computer->readMemLabelType(branchAddress) >= LABEL_TYPE_SUB)
                    {
                        addressAndOpcode.Printf(";%04X: %02X", address, p_Computer->readMemDebug(address));
                        line.Printf("DW   LOW S%02X", branchAddress);
                        while (line.Len()<= 24)
                            line += " ";
                        line += addressAndOpcode;
                        outputTextFile.AddLine("        " + line);
                        addressAndOpcode.Printf(";%04X: %02X", address, p_Computer->readMemDebug(address+1));
                        line.Printf("DW   HIGH S%02X", branchAddress);
                    }
                    else
                    {
                        if (disassemblePass_ == 1)
                        {
                            p_Computer->writeMemLabelType(branchAddress, LABEL_TYPE_SUB);
                            disassembleAgain_ = true;
                            addressAndOpcode.Printf(";%04X: %02X", address, p_Computer->readMemDebug(address));
                            line.Printf("DW   LOW S%02X", branchAddress);
                            while (line.Len()<= 24)
                                line += " ";
                            line += addressAndOpcode;
                            outputTextFile.AddLine("        " + line);
                            addressAndOpcode.Printf(";%04X: %02X", address, p_Computer->readMemDebug(address+1));
                            line.Printf("DW   HIGH S%02X", branchAddress);
                        }
                        else
                        {
                            addressAndOpcode.Printf(";%04X: %02X %02X", address, p_Computer->readMemDebug(address), p_Computer->readMemDebug(address + 1));
                            if (branchAddress >= 0xA000)
                                line.Printf("DW   %05XH", branchAddress);
                            else
                                line.Printf("DW   %04XH", branchAddress);
                        }
                    }
                    while (line.Len()<= 24)
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
                        value = p_Computer->readMemDebug(address);
                        characters = " ";
                        if (value > 0x20)
                            characters.SetChar(0, value&0x7f);
                        text = text + characters;
                        address++;
                        address&=0xffff;
                        memType = p_Computer->readMemDataType(address, &executed);
                    }
                    addressAndOpcode.Printf(";%04X: %02X", textStart, p_Computer->readMemDebug(textStart));
                    line = "DB   '"+text+"'";
                    while (line.Len()<= 24)
                        line += " ";
                    line += addressAndOpcode;
                    
                    textStart++;
                    memType = p_Computer->readMemDataType(textStart, &executed);
                    
                    if (memType == MEM_TYPE_TEXT)
                        outputTextFile.AddLine("        " + line);
                    
                    while (memType == MEM_TYPE_TEXT)
                    {
                        value = p_Computer->readMemDebug(textStart);
                        addressAndOpcode.Printf(";%04X: %02X", textStart, value);
                        line = "";
                        while (line.Len()<= 24)
                            line += " ";
                        line += addressAndOpcode;
                        textStart++;
                        textStart&=0xffff;
                        memType = p_Computer->readMemDataType(textStart, &executed);
                        if (memType == MEM_TYPE_TEXT)
                            outputTextFile.AddLine("        " + line);
                    }
                break;
                    
                default:
                    value = p_Computer->readMemDebug(address);
                    addressAndOpcode.Printf(";%04X: %02X", address, value);
                    if (value >= 0xa0)
                        line.Printf("DB   %03XH", value);
                    else
                        line.Printf("DB   %02XH", value);
                    while (line.Len()<= 24)
                        line += " ";
                    line += addressAndOpcode;
                    address++;
                    address&=0xffff;
                break;
            }
            outputTextFile.AddLine("        " + line);
        }
        outputTextFile.AddLine("        END");
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
                
                outputTextFile.InsertLine(label + "    EQU " + AddressString, lineNumber++);
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

void DebugWindow::assDirOld(wxString fileName, long start, long end)
{
    outputTextFile.Create(fileName);
    wxString printBufferOpcode;
    Word address = (Word)start;
    uint64_t executed;

    wxString line, characters, newChar;
    Byte value;
    
    while(address <= end && address >= start)
    {
        if (start == 0 && address >= 0xfff8)
            start += 8;
        
        Byte memType = p_Computer->readMemDataType(address, &executed);
        
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
            case MEM_TYPE_OPCODE_LBR_SLOT:
            case MEM_TYPE_OPCODE_RLDL:
                line = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, start, end);
            break;
                
            case MEM_TYPE_OPCODE_LDV:
            case MEM_TYPE_OPCODE_LDL:
            case MEM_TYPE_OPCODE_LDRL:
                line = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, start, end);
                outputTextFile.AddLine(line);
                line.Printf("      %02X %02X %02X", p_Computer->readMem(address-3), p_Computer->readMem(address-2), p_Computer->readMem(address-1));
            break;
                
            case MEM_TYPE_OPCODE_LDL_SLOT:
                line = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, start, end);
                outputTextFile.AddLine(line);
                line.Printf("      %02X %02X %02X         %02X%02X", p_Computer->readMem(address-3), p_Computer->readMem(address-2), p_Computer->readMem(address-1), p_Computer->readMem(address-5),p_Computer->readMem(address-2));
            break;
                
            case MEM_TYPE_OPCODE_LDRL_SLOT:
                line = cdp1802disassemble(&address, false, true, DIRECT_ASSEMBLER, start, end);
                outputTextFile.AddLine(line);
                line.Printf("      %02X %02X %02X         %02X%02X", p_Computer->readMem(address-3), p_Computer->readMem(address-2), p_Computer->readMem(address-1), p_Computer->readMem(address-2),p_Computer->readMem(address-5));
            break;

            case MEM_TYPE_OPERAND_LD_3:
                if (p_Computer->readMemDataType((address-3)&0xffff, &executed) == MEM_TYPE_OPCODE_LDL_SLOT)
                    line.Printf("      %02X %02X %02X         %02X%02X", p_Computer->readMem(address), p_Computer->readMem(address+1), p_Computer->readMem(address+2), p_Computer->readMem(address-2),p_Computer->readMem(address+1));
                else
                    line.Printf("      %02X %02X %02X", p_Computer->readMem(address), p_Computer->readMem(address+1), p_Computer->readMem(address+2));
                address+=3;
                address&=0xffff;
            break;
                
            case MEM_TYPE_OPERAND_LDR_3:
                if (p_Computer->readMemDataType((address-3)&0xffff, &executed) == MEM_TYPE_OPCODE_LDRL_SLOT)
                    line.Printf("      %02X %02X %02X         %02X%02X", p_Computer->readMem(address), p_Computer->readMem(address+1), p_Computer->readMem(address+2), p_Computer->readMem(address+1),p_Computer->readMem(address-2));
                else
                    line.Printf("      %02X %02X %02X", p_Computer->readMem(address), p_Computer->readMem(address+1), p_Computer->readMem(address+2));
                address+=3;
                address&=0xffff;
            break;
                
            case MEM_TYPE_PSEUDO_1:
                line = pseudoDisassemble(address, false, true);
                value = p_Computer->readMemDebug(address++);
                
                if (!checkSingleCommand(value))
                    address++;

                if (checkTrippleCommand(value))
                    address++;

                if (checkQuadrupleCommand(value))
                    address++;

                address&=0xffff;
            break;
                
            case MEM_TYPE_OPCODE_JUMP_SLOT:
                line.Printf("%04X: %02X %02X       S%02X,%04X", address, p_Computer->readMem(address), p_Computer->readMem(address+1), p_Computer->readMemDataType(address+1, &executed), (p_Computer->readMem(address)<<8) + p_Computer->readMem(address+1));
                address+=2;
                address&=0xffff;
            break;
                
            case MEM_TYPE_JUMP:
                line.Printf("%04X: %02X %02X       %04X", address, p_Computer->readMem(address), p_Computer->readMem(address+1), (p_Computer->readMem(address)<<8) + p_Computer->readMem(address+1));
                address+=2;
                address&=0xffff;
            break;
                
            case MEM_TYPE_JUMP_REV:
                line.Printf("%04X: %02X %02X       %04X", address, p_Computer->readMem(address), p_Computer->readMem(address+1), (p_Computer->readMem(address+1)<<8) + p_Computer->readMem(address));
                address+=2;
                address&=0xffff;
            break;
                
            default:
                characters = "";
                if ((dataViewDump && selectedTab_ == DIRECTASSTAB) || (dataViewProfiler && selectedTab_ == PROFILERTAB))
                {
                    line.Printf("%04X: ", address);
                    int count = 0;
                    memType = p_Computer->readMemDataType(address, &executed);
                    while (count < 4 && (memType == MEM_TYPE_UNDEFINED || memType == MEM_TYPE_DATA || memType == MEM_TYPE_PSEUDO_2 || memType == MEM_TYPE_OPERAND))
                    {
                        value = p_Computer->readMem(address);
                        printBufferOpcode.Printf("%02X ", value);
                        line = line + printBufferOpcode;
                        newChar = " ";
                        if (value > 0x20)
                            newChar.SetChar(0, value&0x7f);
                        characters = characters + newChar;
                        address++;
                        address&=0xffff;
                        count++;
                        memType = p_Computer->readMemDataType(address, &executed);
                    }
                    if (count == 0)
                    {
                        value = p_Computer->readMem(address);
                        line.Printf("%04X: %02X", address, value);
                        address++;
                        address&=0xffff;
                        characters = " ";
                        if (value > 0x20)
                            characters.SetChar(0, value&0x7f);
                    }
                }
                else
                {
                    value = p_Computer->readMem(address);
                    line.Printf("%04X: %02X", address, value);
                    address++;
                    address&=0xffff;
                    characters = " ";
                    if (value > 0x20)
                        characters.SetChar(0, value&0x7f);
                }
                line = line + "            ";
                line = line.Left(18) + characters;
                break;
        }
        
        outputTextFile.AddLine(line);
    }
    outputTextFile.Write();
    outputTextFile.Close();
}

void DebugWindow::stopAssLog()
{
    if (computerRunning_ && computerConfiguration.debuggerConfiguration.videoLog_active)
    {
        computerConfiguration.debuggerConfiguration.videoLog_active = false;
        if (!writingToLog_)
            dirAssLogFile_.Close();
    }
    writingToLog_ = false;
}

void DebugWindow::assLog(Byte value)
{
    if (!(computerRunning_ && computerConfiguration.debuggerConfiguration.videoLog_active))
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

    if (computerRunning_ && computerConfiguration.debuggerConfiguration.videoLog_active)
    {
        if (value != 0xff && value != 0 && lastLogValue_!= -1)
            dirAssLogFile_.Write(&converted, 1);
    }
}

void DebugWindow::addressLog(Word value)
{
    if (!(computerRunning_ && computerConfiguration.debuggerConfiguration.videoLog_active))
        return;
    
    writingToLog_ = true;
    wxString buffer;
    buffer.Printf("%04X ",value);
    dirAssLogFile_.Write(buffer, 5);
    writingToLog_ = false;
    
    if (!computerConfiguration.debuggerConfiguration.videoLog_active)
        dirAssLogFile_.Close();
}

void DebugWindow::paintDebugBackground()
{
    wxString idReference;

    dcLine.SelectObject(*lineBmp[0]);
//    dcLine.SetPen(wxPen(wxColour(windowInfo.red, windowInfo.green, windowInfo.blue)));
//    dcLine.SetBrush(wxBrush(wxColour(windowInfo.red, windowInfo.green, windowInfo.blue)));
//    dcLine.SetTextBackground(wxColour(windowInfo.red, windowInfo.green, windowInfo.blue));
    dcLine.SetPen(wxPen(guiBackGround_));
    dcLine.SetBrush(wxBrush(guiBackGround_));
    dcLine.SetTextBackground(guiBackGround_);
    dcLine.DrawRectangle(0, 0, charWidth_ * 16 + charWidth_/2, 16);

    for (int y=0; y<16; y++)
    {
        idReference.Printf("CHAR%01X", y);
        XRCCTRL(*this, idReference, wxStaticBitmap)->SetBitmap(*lineBmp[0]);
    }

    wxMemoryDC dcDebugBackground;
    
    dcDebugBackground.SelectObject(*assBmp);
//    dcDebugBackground.SetPen(wxPen(wxColour(windowInfo.red, windowInfo.green, windowInfo.blue)));
//    dcDebugBackground.SetBrush(wxBrush(wxColour(windowInfo.red, windowInfo.green, windowInfo.blue)));
    dcDebugBackground.SetPen(wxPen(guiBackGround_));
    dcDebugBackground.SetBrush(wxBrush(guiBackGround_));

    dcDebugBackground.DrawRectangle(0, 0, assWidth_, numberOfDebugLines_*lineSpace_+4);
    XRCCTRL(*this,"AssScrollbar",wxScrollBar)->SetSize(-1, numberOfDebugLines_*lineSpace_+4);
    
    dcDebugBackground.SelectObject(*profilerBmp);
//    dcDebugBackground.SetPen(wxPen(wxColour(windowInfo.red, windowInfo.green, windowInfo.blue)));
//    dcDebugBackground.SetBrush(wxBrush(wxColour(windowInfo.red, windowInfo.green, windowInfo.blue)));
    dcDebugBackground.SetPen(wxPen(guiBackGround_));
    dcDebugBackground.SetBrush(wxBrush(guiBackGround_));
    
    dcDebugBackground.DrawRectangle(0, 0, profilerWidth_, (numberOfDebugLines_-PROFILER_OFFSET)*lineSpace_+4);
    XRCCTRL(*this,"ProfilerScrollbar",wxScrollBar)->SetSize(-1, (numberOfDebugLines_-PROFILER_OFFSET)*lineSpace_+4);

    
    dcDebugBackground.SelectObject(wxNullBitmap);
    if (xmlLoaded_)
    {
        XRCCTRL(*this, "AssBitmap", wxStaticBitmap)->SetBitmap(*assBmp);
        XRCCTRL(*this, "ProfilerBitmap", wxStaticBitmap)->SetBitmap(*profilerBmp);
    }
}

void DebugWindow::changeNumberOfDebugLines(int height)
{
    wxMemoryDC dcDebugBackground;
    dcDebugBackground.SelectObject(wxNullBitmap);
    delete assBmp;
    delete profilerBmp;

    numberOfDebugLines_ = (int) (height / lineSpace_);
    
    assBmp = new wxBitmap(assWidth_, numberOfDebugLines_*lineSpace_+4, 24);
    XRCCTRL(*this,"AssScrollbar",wxScrollBar)->SetSize(-1, numberOfDebugLines_*lineSpace_+4);

    profilerBmp = new wxBitmap(profilerWidth_, (numberOfDebugLines_-PROFILER_OFFSET)*lineSpace_+4, 24);
    XRCCTRL(*this,"ProfilerScrollbar",wxScrollBar)->SetSize(-1, (numberOfDebugLines_-PROFILER_OFFSET)*lineSpace_+4);

    paintDebugBackground();
    directAss();
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
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("No emulation running");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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
    int slot;
    
    if (computerConfiguration.emsMemoryConfiguration.size() != 0)
    {
        XRCCTRL(*this, "DebugEmsNumber", HexEdit)->changeNumber((int)emsNumber_);
        XRCCTRL(*this, "DebugEmsPage", HexEdit)->changeNumber(p_Computer->getEmsPage(emsNumber_));
    }
    slot = p_Computer->getSelectedSlot();
    if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
        XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->changeNumber(slot);
    if (computerConfiguration.slotConfiguration.banksInUse_)
    {
        XRCCTRL(*this, "DebugExpansionBank", SlotEdit)->changeNumber(p_Computer->getSelectedBank());
        XRCCTRL(*this, "DebugExpansionBank", SlotEdit)->setRange(0, computerConfiguration.slotConfiguration.slotInfo[p_Computer->getSelectedSlot()].maxBankNumber_);
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
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

        if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
        {
            if (start >= computerConfiguration.slotConfiguration.start && start <= computerConfiguration.slotConfiguration.end)
                XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_ORANGE]);
        }
        switch (p_Computer->getMemoryType((int)start/256))
        {
            case EMSMEMORY:
                XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLUE]);
            break;
                
            case PAGER:
                if ((start/(computerConfiguration.memoryMapperConfiguration.mask+1)) == portExtender_)
                    XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_PURPLE]);
            break;
        }

        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            value.Printf("%02X", p_Computer->readMemDebug(start));

            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);
            
            if (computerConfiguration.slotConfiguration.banksInUse_)
            {
                if (computerConfiguration.slotConfiguration.slotInfo[p_Computer->getSelectedSlot()].maxBankNumber_ > 1)
                {
                    if (start >= computerConfiguration.slotConfiguration.start && start <= computerConfiguration.slotConfiguration.end)
                        XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_GREEN]);
                }
            }

            XRCCTRL(*this, idReference, MemEdit)->ChangeValue("");
            XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);

            start++;
            while (start > ramMask)
                start -=  (ramMask + 1);
        }
    }
}

void DebugWindow::DebugDisplayProfiler()
{
    if (!computerRunning_)
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("No emulation running");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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

    if (computerConfiguration.emsMemoryConfiguration.size() != 0)
    {
        XRCCTRL(*this, "DebugEmsNumber", HexEdit)->changeNumber((int)emsNumber_);
        XRCCTRL(*this, "DebugEmsPage", HexEdit)->changeNumber(p_Computer->getEmsPage(emsNumber_));
    }
    if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
        XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->changeNumber(p_Computer->getSelectedSlot());
    if (computerConfiguration.slotConfiguration.banksInUse_)
        XRCCTRL(*this, "DebugExpansionBank", SlotEdit)->changeNumber(p_Computer->getSelectedBank());

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
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

        if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
        {
            if (start >= computerConfiguration.slotConfiguration.start && start <= computerConfiguration.slotConfiguration.end)
                XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_ORANGE]);
        }
        if (computerConfiguration.slotConfiguration.banksInUse_)
        {
            if (computerConfiguration.slotConfiguration.slotInfo[p_Computer->getSelectedSlot()].maxBankNumber_ > 1)
            {
                if (start >= computerConfiguration.slotConfiguration.start && start <= computerConfiguration.slotConfiguration.end)
                    XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_GREEN]);
            }
        }
        switch (p_Computer->getMemoryType((int)start/256))
        {
            case EMSMEMORY:
                XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLUE]);
            break;
                
            case PAGER:
                if ((start/(computerConfiguration.memoryMapperConfiguration.mask+1)) == portExtender_)
                    XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_PURPLE]);
            break;
        }

        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

        uint64_t executed;
        
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            p_Computer->readMemDataType(start, &executed);
      
            Byte executedColor = (Byte)(log((double)executed)*5);

            XRCCTRL(*this, idReference, MemEdit)->ChangeValue("");
            if (executedColor != 0)
            {
                value.Printf("%d", executedColor);
                XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(wxColour(executedColor+34,221-executedColor,0));
                XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);
            }

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
    dcLine.SetTextForeground(*wxWHITE);
    int lineWidth = charWidth_ * 16 + charWidth_/2;
    dcLine.DrawRectangle(0, 0, lineWidth, 16);

    dcLine.SetTextForeground(guiTextColour[GUI_COL_BLACK]);
    dcLine.SetTextBackground(guiBackGround_);

    if (computerConfiguration.vis1870Configuration.defined)
    {
        for (int j=0; j<16; j++)
        {
            int lines =9;
            if (p_Computer->getMaxLinesPerChar() == 8)
                lines = 8;
            for (int i=0; i<lines; i++)
            {
                t = p_Computer->readCramDirect((debugReadMem(address+j)&p_Computer->getPcbMask())*p_Computer->getMaxLinesPerChar()+i);

                if (darkMode_)
                    t = t ^ 0xff;
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
    }
    else
        ShowStandardCharacter(address);

    dcLine.SelectObject(wxNullBitmap);
    XRCCTRL(*this, idReference, wxStaticBitmap)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);
    XRCCTRL(*this, idReference, wxStaticBitmap)->SetBitmap(*lineBmp[y]);
}

void DebugWindow::ShowStandardCharacter(Word address)
{
#if defined(__WXMAC__)
    wxFont exactFont(fontSize_+2, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#else
    wxFont exactFont(fontSize_+1, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
#endif

    for (int j=0; j<16; j++)
    {
        wxString character;
        Byte byteValue;
        if (memoryDisplay_ == RTCRAM && (address+j) > 0x7f)
            byteValue = 0x20;
        else
            byteValue = debugReadMem(address+j)&0x7f;
        character.Printf("%c", byteValue);
        dcLine.SetFont(exactFont);
    /*            int offset = 1;
        if (byteValue == 0x4d || byteValue == 0x57)
            offset = 0;
        if (byteValue == 0x49)
            offset = 3;
        if (byteValue == 0x54)
            offset = 2;*/
    #if defined(__WXMAC__)
        if (fontSize_ == 11)
            dcLine.DrawText(character, (j*charWidth_), 1);
        else
            dcLine.DrawText(character, (j*charWidth_), -1);
    #else
        if (fontSize_ == 9)
            dcLine.DrawText(character, (j*charWidth_), 1);
        else
            dcLine.DrawText(character, (j*charWidth_), -2);
    #endif
    }
}

void DebugWindow::DebugDisplayMap()
{
    if (!computerRunning_)
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("No emulation running");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
        return;
    }

    wxString idReference, idReference2, value;
    enum
    {
        COL_BLACK,
        COL_ORANGE,
        COL_GREEN,
        COL_PURPLE
    };
    int textColor;

    XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("");

    if (computerConfiguration.emsMemoryConfiguration.size() != 0)
    {
        XRCCTRL(*this, "DebugEmsNumber", HexEdit)->changeNumber((int)emsNumber_);
        XRCCTRL(*this, "DebugEmsPage", HexEdit)->changeNumber(p_Computer->getEmsPage(emsNumber_));
    }
    if (computerConfiguration.slotConfiguration.maxSlotNumber_ > 0)
        XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->changeNumber(p_Computer->getSelectedSlot());
    if (computerConfiguration.slotConfiguration.banksInUse_)
        XRCCTRL(*this, "DebugExpansionBank", SlotEdit)->changeNumber(p_Computer->getSelectedBank());

    for (int x=0; x<16; x++)
    {
        idReference.Printf("TOP_HEADER%01X", x);
        value.Printf("  %01X", x);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
    }
    
    Word mainAddressRange, pagerEmsAddressRange, bankSlotAddressRange, mask;
    Byte memType;
    bool groupFound;

    for (int y=0; y<16; y++)
    {
        idReference.Printf("MEM_HEADER%01X", y);
        value.Printf("%04X", y*4096);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

        for (int x=0; x<16; x++)
        {
            textColor = COL_BLACK;
            
            mask = computerConfiguration.memoryMask >> 8;
            mainAddressRange = ((y<<4)+x) & mask;
            pagerEmsAddressRange = (y*16+x) & mask;
            bankSlotAddressRange = ((y&1)*16+x) & mask;
            
            switch (p_Computer->getMemoryType(mainAddressRange) & 0xff)
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
                    
                case NVRAM:
                    value.Printf ("N.");
                break;

                case PARTRAM:
                    value.Printf ("P.");
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
                                    
                case PRAM1870:
                    value.Printf ("PR");
                break;

                case CPURAM:
                    value.Printf ("CP");
                break;
                    
                case SLOTMEM:
                    XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_ORANGE]);
                    
                    if (computerConfiguration.slotConfiguration.slotInfo[p_Computer->getSelectedSlot()].maxBankNumber_ > 1)
                    {
                        textColor = COL_GREEN;
                        memType = p_Computer->getXmlBankMemoryType(p_Computer->getSelectedSlot(), p_Computer->getSelectedBank(), bankSlotAddressRange);
                    }
                    else
                    {
                        textColor = COL_ORANGE;
                        memType = p_Computer->getXmlSlotMemoryType(p_Computer->getSelectedSlot(), bankSlotAddressRange);
                    }

                    groupFound = false;
                    
                    if (computerConfiguration.mc6845Configuration.ioGroupVector.size() == 0)
                        groupFound = true;
                    else
                    {
                        for (std::vector<int>::iterator ioGroupIterator = computerConfiguration.mc6845Configuration.ioGroupVector.begin (); ioGroupIterator != computerConfiguration.mc6845Configuration.ioGroupVector.end (); ++ioGroupIterator)
                        {
                            if (*ioGroupIterator == p_Computer->getIoGroup())
                                groupFound = true;
                        }
                    }
                    if (groupFound)
                    {
                        if (((mainAddressRange*256)&computerConfiguration.mc6845Configuration.dataMask) == computerConfiguration.mc6845Configuration.data)
                            memType = MC6845REGISTERS;

                        if (((mainAddressRange*256)&computerConfiguration.mc6845Configuration.addressMask) == computerConfiguration.mc6845Configuration.address)
                            memType = MC6845REGISTERS;

                        if ((mainAddressRange*256) >=computerConfiguration.mc6845Configuration.startRam && (mainAddressRange*256) <= computerConfiguration.mc6845Configuration.endRam)
                            memType = MC6845RAM;
                    }

                    switch (memType)
                    {
                        case MC6845RAM:
                            value.Printf ("M5");
                        break;

                        case MC6845REGISTERS:
                            value.Printf ("MR");
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

                case EMSMEMORY:
                    XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLUE]);

                    switch (p_Computer->getEmsMemoryType((pagerEmsAddressRange)*256, p_Computer->getMemoryType(mainAddressRange)>>8))
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
                    if (((y*0x1000+x*256)/(computerConfiguration.memoryMapperConfiguration.mask+1)) == portExtender_)
                    {
                        textColor = COL_PURPLE;
                        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_PURPLE]);
                    }
                    switch (p_Computer->getPagerMemoryType(pagerEmsAddressRange))
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
            switch (textColor)
            {
                case COL_BLACK:
                    XRCCTRL(*this, idReference2, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);
                break;
                    
                case COL_ORANGE:
                    XRCCTRL(*this, idReference2, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_ORANGE]);
                break;

                case COL_GREEN:
                    XRCCTRL(*this, idReference2, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_GREEN]);
                break;

                case COL_PURPLE:
                    XRCCTRL(*this, idReference2, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_PURPLE]);
                break;
            }
        }

        idReference.Printf("CHAR%01X", y);
        wxBitmap line(128, 16, 24);
        wxMemoryDC dcMapLine;

        dcMapLine.SelectObject(line);

        dcMapLine.SetPen(wxPen(guiBackGround_));
        dcMapLine.SetBrush(wxBrush(guiBackGround_));
        dcMapLine.SetTextBackground(guiBackGround_);
/*#if defined(__linux__)
        dcMapLine.SetPen(wxPen(wxColour(0xfb, 0xf8, 0xf1)));
        dcMapLine.SetBrush(wxBrush(wxColour(0xfb, 0xf8, 0xf1)));
        dcMapLine.SetTextBackground(wxColour(0xfb, 0xf8, 0xf1));
#else
#if defined(__WXMAC__)
        dcMapLine.SetPen(wxPen(wxColour(214, 214, 214)));
        dcMapLine.SetBrush(wxBrush(wxColour(214, 214, 214)));
        dcMapLine.SetTextBackground(wxColour(214, 214, 214));
#else
        dcMapLine.SetPen(*wxWHITE_PEN);
        dcMapLine.SetBrush(*wxWHITE_BRUSH);
        dcMapLine.SetTextBackground(wxColour(255,255,255));
#endif
#endif*/
        dcMapLine.DrawRectangle(0, 0, 128, 16);

        dcMapLine.SelectObject(wxNullBitmap);
        XRCCTRL(*this, idReference, wxStaticBitmap)->SetBitmap(line);
    }
}
  
void DebugWindow::DebugDisplayVip2kSequencer()
{
    if (!computerConfiguration.vip2KVideoConfiguration.defined )
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("VIP2K Video not running");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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

    for (int x=0; x<16; x++)
    {
        idReference.Printf("TOP_HEADER%01X", x);
        value.Printf("  %01X", (unsigned int)(start+x)&0xf);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
    }
    for (int y=0; y<16; y++)
    {
        idReference.Printf("MEM_HEADER%01X", y);
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            value.Printf("%02X", debugReadMem(start));

            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

            XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue("");
            XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);

            start++;
            while (start > ramMask)  
            start -=  (ramMask + 1);
        }
    }
}
 
void DebugWindow::DebugDisplayRtcRam()
{    
    if (!computerConfiguration.rtcDs12887Configuration.defined)
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("RTC RAM not used");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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

    for (int x=0; x<16; x++)
    {
        idReference.Printf("TOP_HEADER%01X", x);
        value.Printf("  %01X", (unsigned int)(start+x)&0xf);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
    }
    for (int y=0; y<16; y++)
    {
        idReference.Printf("MEM_HEADER%01X", y);
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            if (start >0x7f)
                value = ".";
            else
                value.Printf("%02X", debugReadMem(start));

            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

            XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue("");
            XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);

            start++;
            while (start > 0xff)
                start = 0;
        }
    }
}
 
void DebugWindow::DebugDisplay1870VideoRam()
{
    if (!(computerRunning_ && computerConfiguration.vis1870Configuration.defined) )
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("CDP 1870 not running");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            value.Printf("%02X", debugReadMem(start));

            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

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
    if (computerConfiguration.vis1870Configuration.colorRamType == CR_NONE)
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Colour RAM not used");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);
        
        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
        
        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            value.Printf("%01X", debugReadMem(start));
            
            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);
            
            XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue("");
            XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);
            
            start++;
            while (start > ramMask)  
                start -=  (ramMask + 1);
        }
    }
}

void DebugWindow::DebugDisplay1870GraphicRam()
{
    if (computerConfiguration.vis1870Configuration.graphicMemSize == 0)
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Graphic RAM not used");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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
    
    for (int x=0; x<16; x++)
    {
        idReference.Printf("TOP_HEADER%01X", x);
        value.Printf("  %01X", (unsigned int)(start+x)&0xf);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
    }
    for (int y=0; y<16; y++)
    {
        idReference.Printf("MEM_HEADER%01X", y);
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);
        
        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
        
        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            value.Printf("%02X", debugReadMem(start));
            
            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);
            
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
    if (!(computerRunning_ && (computerConfiguration.cdp1862Configuration.defined || computerConfiguration.cdp1864Configuration.defined || computerConfiguration.studio4VideoConfiguration.defined || computerConfiguration.vip2KVideoConfiguration.defined)))
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Color RAM not used");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            value.Printf("%02X", debugReadMem(start));

            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

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
    if (!(computerConfiguration.mc6845Configuration.defined))
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("MC6845 not running");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            value.Printf("%02X", debugReadMem(start));

            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

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
    if (!computerConfiguration.i8275Configuration.defined)
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Intel 8275 not running");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            value.Printf("%02X", debugReadMem(start));

            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

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
    if (!computerConfiguration.i8275Configuration.defined)
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("Intel 8275 not running");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);
        
        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);
        
        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            value.Printf("%02X", debugReadMem(start));
            
            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);
            
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
    if (!computerConfiguration.mc6847Configuration.defined)
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("MC6847 not running");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            value.Printf("%02X", debugReadMem(start));

            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

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
    if (!computerConfiguration.mc6847Configuration.defined)
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("MC6847 not running");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
        return;
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
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);

            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

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
    if (!computerConfiguration.tmsConfiguration.defined)
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("TMS 9918 not running");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);

            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);
            XRCCTRL(*this, idReference, wxTextCtrl)->ChangeValue("");

            value.Printf("%02X", p_Computer->getTmsMemory((int)start));

            XRCCTRL(*this, idReference, MemEdit)->ChangeValue(value);

            start++;
            while (start > ramMask)  
            start -=  (ramMask + 1);
        }
    }
}

void DebugWindow::DebugDisplayVtRam()
{
    if (computerConfiguration.videoTerminalConfiguration.type == VTNONE)
    {
        if (xmlLoaded_)
        {
            XRCCTRL(*this, "MEM_Message", wxStaticText)->SetLabel("VT not running");
            XRCCTRL(*this, "DebugMemType", wxChoice)->SetSelection(0);
        }
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
        XRCCTRL(*this, idReference, wxStaticText)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

        value.Printf("%04X", (unsigned int)start);
        XRCCTRL(*this, idReference, wxStaticText)->SetLabel(value);

        ShowCharacters(start, y);
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);

            value.Printf("%02X", p_Vt100[UART1]->getVtMemory((int)start));

            XRCCTRL(*this, idReference, MemEdit)->SetForegroundColour(guiTextColour[GUI_COL_BLACK]);

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
            else if (strValue == "N.")
                setMemoryType((int)id, NVRAM);
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
            else if (strValue == "CE")
                setMemoryType((int)id, CPURAM);
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
            else if (strValue == "P.")
                setMemoryType((int)id, PARTRAM);
            else
            {
                (void)wxMessageBox(     "Please use one of the following codes:\n"
                                        "space = UNDEFINED\n"
                                        ". = RAM\n"
                                        "R = ROM\n"
                                        "M. = Mapped RAM\n"
                                        "E. = VP570 Expansion RAM\n"
                                        "N. = NVRAM\n"
                                        "CP = CDP1805 CPU RAM\n"
                                        "PR = 1870 Page RAM\n"
                                        "CR = 1870 Character RAM or\n"
                                        "     Cartridge ROM\n"
                                        "M7 = MC6847 Video RAM\n"
                                        "M5 = MC6845 Video RAM\n"
                                        "MR = MC6845 Register or Mapped ROM\n"
                                        "TC = Test Cartridge ROM\n"
                                        "C. = Victory or Vip Colour RAM access\n"
                                        "P.  = Partly RAM\n"
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
        case CDP_1870_GRAPHIC:
        case CDP_1870_P:
        case TMS_MEMORY:
        case VT_RAM:
        case CDP_1862:
        case CDP_1864:
        case STUDIO_IV_COLOR:
        case V_6845:
        case I_8275:
        case I_8275_RAM:
        case V_6847:
        case V_6847_RAM:
        case VIP2KSEQUENCER:
        case RTCRAM:
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
    if (setType == RAM || setType == ROM || setType == UNDEFINED || setType == CRAM1870 || setType == PRAM1870 || setType == MAPPEDRAM || setType == MAPPEDROM || setType == MC6847RAM || setType == MC6845RAM || setType == MC6845REGISTERS || setType == CRAM1870 || setType == PRAM1870 || setType == COLOURRAM || setType == PARTRAM || setType == VP570RAM)
    {
        if (!computerConfiguration.mc6845Configuration.defined && (setType == MC6845RAM || setType == MC6845REGISTERS))
        {
            (void)wxMessageBox( "No MC6845 configured\n",
                                        "Emma 02", wxICON_ERROR | wxOK );
            return;
        }
        if (!computerConfiguration.mc6847Configuration.defined && (setType == MC6847RAM))
        {
            (void)wxMessageBox( "No MC6847 configured\n",
                                        "Emma 02", wxICON_ERROR | wxOK );
            return;
        }
        if (!computerConfiguration.vis1870Configuration.defined && (setType == CRAM1870 ||setType == PRAM1870))
        {
            (void)wxMessageBox( "No VIS 1870 configured\n",
                                        "Emma 02", wxICON_ERROR | wxOK );
            return;
        }
        if ((p_Computer->getMemoryType(id) & 0xff) == EMSMEMORY)
        {
            if (setType == RAM || setType == ROM || setType == UNDEFINED)
                p_Computer->defineEmsMemoryType(p_Computer->getMemoryType(id)>>8, id*256, setType);
            else
            {
                (void)wxMessageBox( "Only RAM (.), ROM (R), or UNDEFINED (space) allowed in EMS Memory bank\n",
                                   "Emma 02", wxICON_ERROR | wxOK );
            }
        }
        else if (p_Computer->getMemoryType(id) == PAGER)
        {
            if (setType == RAM || setType == ROM || setType == UNDEFINED)
                p_Computer->definePagerMemoryType(id*256, setType);
            else
            {
                (void)wxMessageBox( "Only RAM (.), ROM (R), or UNDEFINED (space) allowed in Pager Memory\n",
                                            "Emma 02", wxICON_ERROR | wxOK );
            }
        }
        else if (!computerConfiguration.vis1870Configuration.defined && (setType == CRAM1870 || setType == PRAM1870))
        {
            (void)wxMessageBox( "No VIS 1870 configured\n",
                               "Emma 02", wxICON_ERROR | wxOK );
            return;
        }
        else if (!(computerConfiguration.cdp1862Configuration.defined || computerConfiguration.cdp1864Configuration.defined || computerConfiguration.studio4VideoConfiguration.defined) && (setType == COLOURRAM))
        {
            (void)wxMessageBox( "No colour RAM configured\n",
                               "Emma 02", wxICON_ERROR | wxOK );
            return;
        }
        else

            p_Computer->defineMemoryType(id*256, setType);
    }
    else
    {
        (void)wxMessageBox( "Only RAM (.), ROM (R), MAPPED RAM (M.), MAPPED ROM (MR), CDP1870 (PR/CR), MC6845 (M5/MR), MC6847 (M7) or UNDEFINED (space) allowed in Xml emulation\n",
                                    "Emma 02", wxICON_ERROR | wxOK );
    }
}

void DebugWindow::memoryDisplaySetGuiSize(int offset)
{
    wxPoint originalPosition;
    wxSize originalSize;
    wxString idReference;
    int positionOffset;
    
    XRCCTRL(*this, "MEM00", MemEdit)->GetSize(&originalSize.x, &originalSize.y);

    XRCCTRL(*this, "MEM_Message", wxStaticText)->Show(offset < 0);

    positionOffset = 0;
    for (int x=0; x<16; x++)
    {
        idReference.Printf("TOP_HEADER%01X", x);
        XRCCTRL(*this, idReference, wxStaticText)->SetSize(originalSize.x+offset, -1);
        XRCCTRL(*this, idReference, wxStaticText)->GetPosition(&originalPosition.x, &originalPosition.y);
        originalPosition.x += positionOffset;
        positionOffset += offset;
        XRCCTRL(*this, idReference, wxStaticText)->Move(originalPosition.x, originalPosition.y);
    }
    
    for (int y=0; y<16; y++)
    {
        idReference.Printf("CHAR%01X", y);
        XRCCTRL(*this, idReference, wxStaticBitmap)->Show(offset < 0);

        positionOffset = 0;
        for (int x=0; x<16; x++)
        {
            idReference.Printf("MEM%01X%01X", y, x);
            XRCCTRL(*this, idReference, MemEdit)->SetSize(originalSize.x+offset, -1);
            XRCCTRL(*this, idReference, MemEdit)->GetPosition(&originalPosition.x, &originalPosition.y);
            originalPosition.x += positionOffset;
            positionOffset += offset;
            XRCCTRL(*this, idReference, MemEdit)->Move(originalPosition.x, originalPosition.y);
        }
    }
}

void DebugWindow::memoryDisplay()
{
    switch (memoryDisplay_)
    {
        case CPU_MEMORY:
            DebugDisplayPage();
        break;

        case CPU_TYPE:
            DebugDisplayMap();
        break;

        case CPU_PROFILER:
            DebugDisplayProfiler();
        break;

        case CDP_1870_C:
            DebugDisplay1870VideoRam();
        break;
            
        case CDP_1870_COLOUR:
            DebugDisplay1870ColourRam();
        break;
            
        case CDP_1870_GRAPHIC:
            DebugDisplay1870GraphicRam();
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

        case CDP_1862:
            DebugDisplay1864ColorRam();
        break;

        case CDP_1864:
            DebugDisplay1864ColorRam();
        break;

        case STUDIO_IV_COLOR:
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
        
        case VIP2KSEQUENCER:
            DebugDisplayVip2kSequencer();
        break;

        case RTCRAM:
            DebugDisplayRtcRam();
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
            return p_Computer->getCharMemorySize();
        break;

        case CDP_1870_COLOUR:
            return p_Computer->getCharMemorySize();
        break;
            
        case CDP_1870_GRAPHIC:
            return p_Computer->getGraphicMemorySize();
        break;
            
        case CDP_1870_P:
            return p_Computer->getPageMemorySize();
        break;

        case TMS_MEMORY:
            return 0x3fff;
        break;

        case CDP_1862:
            return 0xff;
        break;
            
        case CDP_1864:
            return computerConfiguration.cdp1864Configuration.ramMask;
        break;

        case STUDIO_IV_COLOR:
            return 0xff;
        break;

        case I_8275:
            return 0x1fff;
        break;
            
        case VIP2KSEQUENCER:
            return 0x7ff;
        break;
            
        case RTCRAM:
            return 0x7f;
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
            return p_Computer->get6847RamMask();
        break;

        default:
            return 0xffff;
        break;
    }
}

void DebugWindow::onDebugMemType(wxCommandEvent&event)
{
    if (memoryDisplay_ == CPU_PROFILER)
#if defined(__WXMAC__)
        memoryDisplaySetGuiSize(-9);
#else
        memoryDisplaySetGuiSize(-7);
#endif
    memoryDisplay_ = event.GetSelection();

    if (memoryDisplay_ == CPU_PROFILER)
#if defined(__WXMAC__)
        memoryDisplaySetGuiSize(9);
#else
        memoryDisplaySetGuiSize(7);
#endif

    memoryDisplay();

    XRCCTRL(*this, "DebugDisplayPage", wxTextCtrl)->Enable(memoryDisplay_ != CPU_TYPE);
#if defined(__WXMSW__) || defined(__WXMAC__)
    XRCCTRL(*this, "DebugDisplayPageSpinButton", wxSpinButton)->Enable(memoryDisplay_ != CPU_TYPE);
#endif
    XRCCTRL(*this, "DebugSave", wxButton)->Enable(memoryDisplay_ != CPU_TYPE && memoryDisplay_ != CPU_PROFILER);
    XRCCTRL(*this, "DebugCopy", wxButton)->Enable(memoryDisplay_ != CPU_TYPE && memoryDisplay_ != CPU_PROFILER && memoryDisplay_ != RTCRAM);
}

void DebugWindow::onDebugExpansionSlot(wxCommandEvent&WXUNUSED(event))
{
    wxString value = XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->GetValue();

    if (!computerRunning_) return;

    long selection;
    if (!value.ToLong(&selection))
        return;

    p_Computer->setSelectedSlot((int)selection);
    p_Main->updateSlotInfo();

/*    if (p_Computer->getSelectedSlot() == selection)
        return;
    
    XRCCTRL(*this, "DebugExpansionSlot", SlotEdit)->saveNumber((int)selection);

    int slot = 1 << selection;
    int bank = p_Computer->getOutValue(1) & 0xe0;
    slot = bank | slot;

    p_Computer->out(1, 0, slot);*/
}

void DebugWindow::onDebugExpansionRam(wxCommandEvent&WXUNUSED(event))
{
    wxString value = XRCCTRL(*this, "DebugExpansionBank", SlotEdit)->GetValue();

    if (!computerRunning_) return;

    long selection;
    if (!value.ToLong(&selection))
        return;

    p_Computer->setSelectedBank((int)selection);
    
    if (p_Computer->getSelectedBank() != selection)
        XRCCTRL(*this, "DebugExpansionBank", SlotEdit)->changeNumber(p_Computer->getSelectedBank());

    p_Main->updateSlotInfo();

 /*   if (p_Computer->getSelectedBank() == selection)
        return;
    
    XRCCTRL(*this, "DebugExpansionBank", SlotEdit)->saveNumber((int)selection);

    int bank = (int)selection << 5;
    int slot = p_Computer->getOutValue(1) & 0x1f;
    slot = bank | slot;

    p_Computer->out(1, 0, slot);*/
}

void DebugWindow::onDebugEmsPage(wxCommandEvent&WXUNUSED(event))
{
    if (computerConfiguration.emsMemoryConfiguration.size() == 0 || !computerRunning_)
        return;

    wxString value;
    long page;

    value = XRCCTRL(*this, "DebugEmsPage", HexEdit)->GetValue();

    if (!value.ToLong(&page, 16))
        return;

    value = XRCCTRL(*this, "DebugEmsNumber", HexEdit)->GetValue();
    if (!value.ToLong(&emsNumber_, 16))
        return;

    XRCCTRL(*this, "DebugEmsNumber", HexEdit)->saveNumber((int)emsNumber_);
    XRCCTRL(*this, "DebugEmsPage", HexEdit)->saveNumber((int)page);

    if (emsNumber_ >= computerConfiguration.emsMemoryConfiguration.size())
        emsNumber_ = computerConfiguration.emsMemoryConfiguration.size() - 1;

    if (page > computerConfiguration.emsMemoryConfiguration[emsNumber_].output.mask)
        page = computerConfiguration.emsMemoryConfiguration[emsNumber_].output.mask;

    p_Computer->setEmsPage(emsNumber_, page);
}

void DebugWindow::onDebugEmsNumber(wxCommandEvent&WXUNUSED(event))
{
    if (computerConfiguration.emsMemoryConfiguration.size() == 0 || !computerRunning_)
        return;

    wxString value;
    long page;

    value = XRCCTRL(*this, "DebugEmsNumber", HexEdit)->GetValue();
    if (!value.ToLong(&emsNumber_, 16))
        return;
    
    value = XRCCTRL(*this, "DebugEmsPage", HexEdit)->GetValue();
    if (!value.ToLong(&page, 16))
        return;

    XRCCTRL(*this, "DebugEmsNumber", HexEdit)->saveNumber((int)emsNumber_);
    XRCCTRL(*this, "DebugEmsPage", HexEdit)->saveNumber((int)page);

    if (emsNumber_ >= computerConfiguration.emsMemoryConfiguration.size())
        emsNumber_ = computerConfiguration.emsMemoryConfiguration.size() - 1;

    if (page > computerConfiguration.emsMemoryConfiguration[emsNumber_].output.mask)
        page = computerConfiguration.emsMemoryConfiguration[emsNumber_].output.mask;

    p_Computer->setEmsPage(emsNumber_, page);
}

void DebugWindow::onDebugPager(wxCommandEvent&WXUNUSED(event))
{
    if (!computerConfiguration.memoryMapperConfiguration.defined)
        return;

    wxString value = XRCCTRL(*this, "DebugPager", HexEdit)->GetValue();

    if (!computerRunning_) return;

    long page;
    if (!value.ToLong(&page, 16))
        return;

    XRCCTRL(*this, "DebugPager", HexEdit)->saveNumber((int)page);

//    int selectOutput = getConfigItem(computerInfo.gui+"/PortExtenderSelectOutput", 5l);
    p_Computer->out(computerConfiguration.memoryMapperConfiguration.selectOutput.portNumber[0], 0, portExtender_);

//    int writeOutput = getConfigItem(computerInfo.gui+"PortExtenderWriteOutput", 6l);
    p_Computer->out(computerConfiguration.memoryMapperConfiguration.writeOutput.portNumber[0], 0, page);
}

void DebugWindow::onDebugPortExtender(wxCommandEvent&WXUNUSED(event))
{
    if (!computerConfiguration.memoryMapperConfiguration.defined)
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

Byte DebugWindow::debugReadMem(Word address)
{
    switch (memoryDisplay_)
    {
        case CPU_MEMORY:
            return p_Computer->readMemDebug(address);
        break;

        case CDP_1870_C:
            if (computerConfiguration.vis1870Configuration.defined)
                return p_Computer->readCramDirect(address);
            else
                return 0;
        break;

        case CDP_1870_COLOUR:
            if (computerConfiguration.vis1870Configuration.defined)
                return p_Computer->readColourRamDirect(address);
            else
                return 0;
        break;

        case CDP_1870_GRAPHIC:
            if (computerConfiguration.vis1870Configuration.defined)
                return p_Computer->readGraphicRamDirect(address);
            else
                return 0;
        break;

        case CDP_1870_P:
            if (computerConfiguration.vis1870Configuration.defined)
                return p_Computer->readPramDirect(address);
            else
                return 0;
        break;

        case TMS_MEMORY:
            if (computerConfiguration.tmsConfiguration.defined)
                return p_Computer->getTmsMemory(address);
            else
                return 0;
        break;

        case VT_RAM:
            if ((computerRunning_ && (p_Vt100[UART1] != NULL)))
                return p_Vt100[UART1]->getVtMemory(address);
            else
                return 0;
        break;

        case CDP_1862:
            if (computerConfiguration.cdp1862Configuration.defined)
                return p_Computer->read1862ColorDirect(address);
            else
                return 0;
        break;

        case CDP_1864:
            if (computerConfiguration.cdp1864Configuration.defined)
                return p_Computer->read1864ColorDirect(address);
            else
                return 0;
        break;

        case STUDIO_IV_COLOR:
            if (computerConfiguration.studio4VideoConfiguration.defined)
                return p_Computer->readSt4ColorDirect(address);
            else
                return 0;
        break;

        case I_8275:
            if (computerConfiguration.i8275Configuration.defined)
                return p_Computer->read8275CharRom(address);
            else
                return 0;
        break;

        case I_8275_RAM:
            if (computerConfiguration.i8275Configuration.defined)
                return p_Computer->read8275VideoRam(address);
            else
                return 0;
        break;
            
        case V_6845:
            if (computerConfiguration.mc6845Configuration.defined)
                return p_Computer->read6845CharRom(address);
            else
                return 0;
        break;

        case V_6847:
            if (computerConfiguration.mc6847Configuration.defined)
                return p_Computer->read6847CharRom(address);
            else
                return 0;
        break;

        case V_6847_RAM:
            if (computerConfiguration.mc6847Configuration.defined)
                return p_Computer->readDirect6847(address);
            else
                return 0;
        break;

        case VIP2KSEQUENCER:
            if (computerConfiguration.vip2KVideoConfiguration.defined)
                return p_Computer->readSequencerRom(address);
            else
                return 0;
        break;
            
        case RTCRAM:
            if (computerConfiguration.rtcDs12887Configuration.defined)
                return p_Computer->readDirectRtc(address);
            else
                return 0;
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
            p_Computer->writeMemDebug(address, value, true);
        break;

        case CDP_1870_C:
            if (computerConfiguration.vis1870Configuration.defined)
                p_Computer->writeCramDirect(address, value);
        break;

        case CDP_1870_COLOUR:
            if (computerConfiguration.vis1870Configuration.defined)
                p_Computer->writeColourRamDirect(address, value);
        break;
            
        case CDP_1870_GRAPHIC:
            if (computerConfiguration.vis1870Configuration.defined)
                p_Computer->writeGraphicRamDirect(address, value);
        break;
            
        case CDP_1870_P:
            if (computerConfiguration.vis1870Configuration.defined)
                p_Computer->writePramDirect(address, value);
        break;

        case TMS_MEMORY:
            if (computerConfiguration.tmsConfiguration.defined)
                p_Computer->setTmsMemory(address, value);
        break;

        case VT_RAM:
            if (computerRunning_ && (p_Vt100[UART1] != NULL))
                p_Vt100[UART1]->setVtMemory(address, value);
        break;

        case CDP_1862:
            if (computerConfiguration.cdp1862Configuration.defined)
                p_Computer->write1862ColorDirect(address, value);
        break;

        case CDP_1864:
            if (computerConfiguration.cdp1864Configuration.defined)
                p_Computer->write1864ColorDirect(address, value);
        break;

        case STUDIO_IV_COLOR:
            if (computerConfiguration.studio4VideoConfiguration.defined)
                p_Computer->writeSt4ColorDirect(address, value);
        break;

        case I_8275:
            if (computerConfiguration.i8275Configuration.defined)
                p_Computer->write8275CharRom(address, value);
        break;

        case I_8275_RAM:
            if (computerConfiguration.i8275Configuration.defined)
                p_Computer->write8275VideoRam(address, value);
        break;
            
        case V_6845:
            if (computerConfiguration.mc6845Configuration.defined)
                p_Computer->write6845CharRom(address, value);
        break;

        case V_6847:
            if (computerConfiguration.mc6847Configuration.defined)
                p_Computer->write6847CharRom(address, value);
        break;

        case V_6847_RAM:
            if (computerConfiguration.mc6847Configuration.defined)
                p_Computer->writeDirect6847(address, value);
        break;
        
        case VIP2KSEQUENCER:
            if (computerConfiguration.vip2KVideoConfiguration.defined)
                p_Computer->writeSequencerRom(address, value);
        break;

        case RTCRAM:
            if (computerConfiguration.rtcDs12887Configuration.defined)
                p_Computer->writeDirectRtc(address&0x7f, value);
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
            title = swName_ + ", Pseudo and 1802 Debug Mode";
    }
    else
    {
        if (chip8DebugMode_)
            title = swName_ + ", Pseudo Debug Mode";
    }

    if (!computerRunning_)
        return;
    
    if (p_Computer->getSteps()==0)
        title = title + " ** PAUSED **";
    if (p_Computer->getClear()==0)
        title = title + " ** CPU STOPPED **";
    p_Computer->SetTitle(getRunningComputerText() + title);
    p_Computer->updateTitle(title);
    p_Computer->setDebugMode(debugMode_, chip8DebugMode_, trace_, traceDma_, traceInt_, traceChip8Int_);
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
        p_Main->eventUpdateTitle();
    }
    XRCCTRL(*this,"PercentageClock", wxSlider)->Enable(debugMode_);
    XRCCTRL(*this,"PercentageClockText", wxStaticText)->Enable(debugMode_);
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
        XRCCTRL(*this, "Chip8PauseButton", wxBitmapButton)->SetBitmapLabel(pauseOnBitmap);
//        XRCCTRL(*this, "Chip8PauseButton", wxBitmapButton)->Enable(false);
        XRCCTRL(*this, "Chip8StepButton", wxBitmapButton)->Enable(true);
        chip8Steps_ = 1;
    }
    else
    {
        chip8Steps_ = -1;
        p_Computer->setSteps(-1);
    }
    updateChip8DebugMenu(true);
    setChip8PauseState();
}

void DebugWindow::setChip8PauseState()
{
    if (chip8Steps_ == 0)
    {
        XRCCTRL(*this, "Chip8PauseButton", wxBitmapButton)->SetBitmapLabel(pauseOnBitmap);
        XRCCTRL(*this, "Chip8StepButton", wxBitmapButton)->Enable(true);
    }
    else
    {
        if (chip8Steps_ > 0)
        {
            XRCCTRL(*this, "Chip8PauseButton", wxBitmapButton)->SetBitmapLabel(pauseOnBitmap);
            XRCCTRL(*this, "Chip8StepButton", wxBitmapButton)->Enable(true);
        }
        else
        {
            XRCCTRL(*this, "Chip8PauseButton", wxBitmapButton)->SetBitmapLabel(pauseOffBitmap);
            XRCCTRL(*this, "Chip8StepButton", wxBitmapButton)->Enable(false);
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
//    p_Computer->setSteps(-1);
    performChip8Step_ = true;
}

void DebugWindow::onChip8Clear(wxCommandEvent& WXUNUSED(event))
{
    chip8TraceWindowPointer->Clear();
}

void DebugWindow::pseudoTrace(Word address)
{
    chip8DebugTrace(pseudoDisassemble(address, true, false));
}

wxString DebugWindow::getPseudoDefinition(Word* pseudoBaseVar, Word* pseudoMainLoop, bool* chip8register12bit, bool* pseudoLoaded)
{
    wxTextFile defFile;

    wxString definition, addressStr, valueStr;
    long address, value, base, loop;

    *pseudoLoaded = false;
    *pseudoBaseVar = 0;
    *pseudoMainLoop = 0;
    *chip8register12bit = false;
    commandSyntaxFile_ = applicationDirectory_ + "Chip8.syntax";

    if (defFile.Open(applicationDirectory_ + "pseudo.def"))
    {
        for (wxString pseudoLine=defFile.GetFirstLine(); !defFile.Eof(); pseudoLine=defFile.GetNextLine())
        {
            pseudoLine.Trim(false);
            pseudoLine.Trim(true);

            pseudoType_ = pseudoLine;
            if (!defFile.Eof())
                pseudoLine=defFile.GetNextLine();

            pseudoLine.Trim(false);
            pseudoLine.Trim(true);

            while (pseudoLine.GetChar(0) != '/' && !defFile.Eof())
            {
                definition = extractWord(&pseudoLine);
                if (definition == "MEM")
                {
                    *pseudoLoaded = true;
                    while (pseudoLine.Len() > 0)
                    {
                        addressStr = extractWord(&pseudoLine);
                        valueStr = extractWord(&pseudoLine);

                        if (addressStr.ToLong(&address, 16) && valueStr.ToLong(&value, 16))
                        {
                            if (p_Computer->readMemDebug(address) != value)
                                *pseudoLoaded = false;
                        }
                    }
                }
                if (definition == "BASE" && *pseudoLoaded)
                {
                    addressStr = extractWord(&pseudoLine);
                    if (addressStr.ToLong(&base, 16))
                        *pseudoBaseVar = base;
                }
                if (definition == "LOOP" && *pseudoLoaded)
                {
                    addressStr = extractWord(&pseudoLine);
                    if (addressStr.ToLong(&loop, 16))
                        *pseudoMainLoop = loop;
                }
                if (definition == "FILE" && *pseudoLoaded)
                {
                    pseudoLine.Trim(false);
                    pseudoLine.Trim(true);
                    commandSyntaxFile_ = applicationDirectory_ + pseudoLine;
                }
                if (definition == "12BIT" && *pseudoLoaded)
                    *chip8register12bit = true;
                if (definition == "DISPLAY" && *pseudoLoaded)
                {
                    pseudoLine.Trim(false);
                    pseudoLine.Trim(true);
                    if (mode_.gui)
                        XRCCTRL(*this, "Chip8Type", wxStaticText)->SetLabel(pseudoLine);
                }
                pseudoLine=defFile.GetNextLine();
                pseudoLine.Trim(false);
                pseudoLine.Trim(true);
            }
            if (*pseudoLoaded)
                break;
        }
    }
    defFile.Close();

    if (*pseudoLoaded)
        p_Main->definePseudoCommands();
    else
        pseudoType_ = "";

    pseudoLoaded_ = *pseudoLoaded;
    return pseudoType_;
}

void DebugWindow::forcePseudoDefinition(wxString pseudoType, wxString filename, wxString pseudoName)
{
    pseudoType_ = pseudoType;
    pseudoLoaded_ = true;
    commandSyntaxFile_ = applicationDirectory_ + filename;
//    XRCCTRL(*this, "Chip8Type", wxStaticText)->SetLabel(pseudoName);
    p_Main->eventSetStaticTextValue("Chip8Type", pseudoName);
    p_Main->definePseudoCommands();
}

void DebugWindow::definePseudoCommands()
{
    wxString pseudoLine, commandText, subCommand, command;
    long commandLong;
    int command_offset;
    bool commandFound;

    psuedoNumber_ = 0;
    singleByteCommandNumber_ = 0;
    trippleByteCommandNumber_ = 0;
    quadrupleByteCommandNumber_ = 0;
    jumpCommandNumber_ = 0;
    branchCommandNumber_ = 0;

    if (inFile.Open(commandSyntaxFile_))
    {
        for (pseudoLine=inFile.GetFirstLine(); !inFile.Eof(); pseudoLine=inFile.GetNextLine())
        {
            pseudoLine.Trim(false);
            pseudoLine.Trim(true);
            command_offset = 0;
            
            if (pseudoLine.Len() > 0)
            {
                commandText = " " + pseudoLine;
                while (commandText.Len() <= 6)
                    commandText = commandText + " ";
                
                pseudoLine=inFile.GetNextLine();
                while (pseudoLine.Left(1) != "/")
                {
                    psuedoNumber_++;
                    pseudoCodeDetails_.resize(psuedoNumber_);
                    
                    pseudoCodeDetails_[psuedoNumber_-1].commandText = commandText;
                    
                    pseudoCodeDetails_[psuedoNumber_-1].length = 2;
                    if (pseudoLine.Mid(pseudoLine.Len()-7,1) == "=")
                    {
                        command_offset = 2;
                        pseudoCodeDetails_[psuedoNumber_-1].length = 3;

                        subCommand = pseudoLine.Mid(pseudoLine.Len()-6,2);
                        
                        if (subCommand.ToLong(&commandLong, 16))
                        {
                            commandFound = false;
                            for (size_t i=0; i<trippleByteCommandNumber_; i++)
                            {
                                if (trippleByteCommand_[i] == (Byte)commandLong)
                                    commandFound = true;
                            }
                            if (!commandFound)
                            {
                                trippleByteCommandNumber_++;
                                trippleByteCommand_.resize(trippleByteCommandNumber_);
                                
                                trippleByteCommand_[trippleByteCommandNumber_-1] = (Byte)commandLong;
                            }
                        }
                    }

                    if (pseudoLine.Mid(pseudoLine.Len()-9,1) == "=")
                    {
                        command_offset = 4;
                        pseudoCodeDetails_[psuedoNumber_-1].length = 4;

                        subCommand = pseudoLine.Mid(pseudoLine.Len()-8,2);

                        if (subCommand.ToLong(&commandLong, 16))
                        {
                            quadrupleByteCommandNumber_++;
                            quadrupleByteCommand_.resize(quadrupleByteCommandNumber_);
                            
                            quadrupleByteCommand_[quadrupleByteCommandNumber_-1] = (Byte)commandLong;
                        }
                    }

                    subCommand = pseudoLine.Right(4+command_offset);
                    if (subCommand.Mid(1,1) == "=")
                    {
                        pseudoCodeDetails_[psuedoNumber_-1].length = 1;
                        subCommand = pseudoLine.Right(2);

                        if (subCommand.ToLong(&commandLong, 16))
                        {
                            singleByteCommandNumber_++;
                            singleByteCommand_.resize(singleByteCommandNumber_);

                            singleByteCommand_[singleByteCommandNumber_-1] = (Byte)commandLong;
                        }
                        command = subCommand.Left(1);
                        subCommand = pseudoLine.Right(1);
                        pseudoCodeDetails_[psuedoNumber_-1].parameterText = pseudoLine.Left(pseudoLine.Len()-7);
                    }
                    else
                    {
                        if (subCommand.GetChar(2+command_offset) == 'j' || subCommand.GetChar(2+command_offset) == 's')
                        {
                            command = subCommand.Left(2);
        
                            jumpCommandNumber_++;
                            jumpCommand_.resize(jumpCommandNumber_);
                            jumpMask_.resize(jumpCommandNumber_);
                            jumpOffset_.resize(jumpCommandNumber_);
                            
                            jumpOffset_[jumpCommandNumber_-1] = 0;
                            
                            if (pseudoLine.Left(6) == "hex=08")
                                jumpOffset_[jumpCommandNumber_-1] = 0x0800;
                            if (pseudoLine.Left(6) == "hex=10")
                                jumpOffset_[jumpCommandNumber_-1] = 0x1000;
                            if (pseudoLine.Left(6) == "hex=11")
                                jumpOffset_[jumpCommandNumber_-1] = 0x1100;
                            
                            if (pseudoLine.Left(10) == "I , hex=10")
                                jumpOffset_[jumpCommandNumber_-1] = 0x1000;
                            if (pseudoLine.Left(10) == "I , hex=20")
                                jumpOffset_[jumpCommandNumber_-1] = 0x2000;
                            if (pseudoLine.Left(10) == "I , hex=30")
                                jumpOffset_[jumpCommandNumber_-1] = 0x3000;
                            if (subCommand.GetChar(2) == 's')
                            {
                                if (pseudoType_ == "SUPERCHIP")
                                    jumpOffset_[jumpCommandNumber_-1] = 0x600;
                                else
                                    jumpOffset_[jumpCommandNumber_-1] = 0x100;
                            }

                            if (pseudoType_ == "AMVBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020")
                            {
                                jumpOffset_[jumpCommandNumber_-1] = 10;
                                if (subCommand.Right(4) != "jjjj")
                                {
                                    if (subCommand.Len() == 4)
                                        jumpOffset_[jumpCommandNumber_-1] = 0;
                                    if (subCommand.Len() == 6)
                                        jumpOffset_[jumpCommandNumber_-1] = 1;
                                    if (subCommand.Len() == 8)
                                        jumpOffset_[jumpCommandNumber_-1] = 2;
                                }
                            }

                            if (command.ToLong(&commandLong, 16))
                            {
                                jumpCommand_[jumpCommandNumber_-1] = (Byte)commandLong;
                                
                                if (subCommand.Right(4) == "jjjj")
                                    jumpMask_[jumpCommandNumber_-1] = 0xFFFF;
                                else
                                    jumpMask_[jumpCommandNumber_-1] = 0xFF;
                            }
                            else
                            {
                                command = subCommand.Left(1);
                                
                                if (command.ToLong(&commandLong, 16))
                                {
                                    jumpCommand_[jumpCommandNumber_-1] = (Byte)commandLong*16;
                                    jumpMask_[jumpCommandNumber_-1] = 0xF0;
                                }
                                else
                                {
                                    jumpMask_[jumpCommandNumber_-1] = 0;
                                }
                            }
                        }
                        if (subCommand.GetChar(2) == 'b')
                        {
                            command = subCommand.Left(2);
                            
                            branchCommandNumber_++;
                            branchCommand_.resize(branchCommandNumber_);
                            branchMask_.resize(branchCommandNumber_);
                            
                            if (command.ToLong(&commandLong, 16))
                            {
                                branchCommand_[branchCommandNumber_-1] = (Byte)commandLong;
                                branchMask_[branchCommandNumber_-1] = 0xFF;
                            }
                            else
                            {
                                command = subCommand.Left(1);
                                
                                if (command.ToLong(&commandLong, 16))
                                {
                                    branchCommand_[branchCommandNumber_-1] = (Byte)commandLong*16;
                                    branchMask_[branchCommandNumber_-1] = 0xF0;
                                }
                                else
                                {
                                    branchMask_[branchCommandNumber_-1] = 0;
                                }
                            }
                        
                        }
                        if (subCommand.GetChar(2) == 'd')
                        {
                            command = subCommand.Left(2);
                            
                            if (command.ToLong(&commandLong, 16))
                            {
                                commandFound = false;
                                for (size_t i=0; i<decimalBranchCommandNumber_; i++)
                                {
                                    if (decimalBranchCommand_[i] == (Byte)commandLong)
                                    {
                                        commandFound = true;
                                        break;
                                    }
                                    
                                }
                                if (!commandFound)
                                {
                                    decimalBranchCommandNumber_++;
                                    decimalBranchCommand_.resize(decimalBranchCommandNumber_);
                                    
                                    decimalBranchCommand_[decimalBranchCommandNumber_-1] = (Byte)commandLong;
                                }
                            }
                        }

                        command = subCommand.Left(1);
                        subCommand = subCommand.Right(3+command_offset);
                        pseudoCodeDetails_[psuedoNumber_-1].parameterText = pseudoLine.Left(pseudoLine.Len()-9-command_offset);
                        
                    }
                    pseudoCodeDetails_[psuedoNumber_-1].parameterText.Trim(true);
                    pseudoCodeDetails_[psuedoNumber_-1].parameterText.Trim(false);
                    
                    command.ToLong(&pseudoCodeDetails_[psuedoNumber_-1].command, 16);
                    pseudoCodeDetails_[psuedoNumber_-1].subCommand = subCommand;
                    
                    pseudoLine=inFile.GetNextLine();
                }
            }
        }
    }
    inFile.Close();
}

wxString DebugWindow::pseudoDisassemble(Word dis_address, bool includeDetails, bool showOpcode)
{
    Byte chip8_opcode1 = p_Computer->readMemDebug(dis_address);
    Byte chip8_opcode2 = p_Computer->readMemDebug(dis_address + 1);
    Byte chip8_opcode3 = 0;
    Byte chip8_opcode4 = 0;

    wxString buffer, detailsBuffer, addressStr, charBufferStr;
    buffer = "";
    detailsBuffer = "";

    Byte command = chip8_opcode1 >> 4;
    Word address = ((chip8_opcode1 & 0xf) << 8) + chip8_opcode2;
    
    Byte nibble[5];
    nibble[0] = chip8_opcode1&0xf;
    nibble[1] = (chip8_opcode2>>4)&0xf;
    nibble[2] = chip8_opcode2&0xf;

    int vX = chip8_opcode1&0xf;
    int vY;
    int vZ = chip8_opcode2&0xf;

    int rX = (chip8_opcode1&0xf)/2;
    int rY = ((chip8_opcode2>>4)&0xf)/2;
    int rZ = (chip8_opcode2&0xf)/2;
    

    Word valueI, RegisterA, RegisterB;
    
    if (pseudoType_ == "AMVBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020")
    {
        chip8_opcode3 = p_Computer->readMemDebug(dis_address + 2);
        chip8_opcode4 = p_Computer->readMemDebug(dis_address + 3);
        nibble[3] = (chip8_opcode3>>4)&0xf;
        nibble[4] = chip8_opcode3&0xf;
    }
    
    if (pseudoType_ == "STIV")
    {
        valueI = (p_Computer->readMemDebug(0x27f6)<<8)+p_Computer->readMemDebug(0x27f7);
        vY = (chip8_opcode2)&0xf;
    }
    else
    {
        valueI = p_Computer->getScratchpadRegister(CHIP8_I) & 0xfff;
        vY = (chip8_opcode2>>4)&0xf;
    }
    
    if (pseudoType_ == "FEL")
    {
        RegisterA = (p_Computer->readMemDebug(0x110)<<8)+p_Computer->readMemDebug(0x111);
        RegisterB = (p_Computer->readMemDebug(0x112)<<8)+p_Computer->readMemDebug(0x113);
    }
    else
    {
        RegisterA = p_Computer->getScratchpadRegister(0xA);
        RegisterB = p_Computer->getScratchpadRegister(0xB);
    }
    
    //Real time value
    Word addressX = p_Computer->getChip8baseVar() + vX;
    Word addressY = p_Computer->getChip8baseVar() + vY;
    Word addressZ = p_Computer->getChip8baseVar() + vZ;
    
    Word addressRX = p_Computer->getChip8baseVar() + rX*2;
    Word addressRY = p_Computer->getChip8baseVar() + rY*2;
    Word addressRZ = p_Computer->getChip8baseVar() + rZ*2;
//    Byte valueX = p_Computer->readMemDebug(addressX);
    Byte valueY = p_Computer->readMemDebug(addressY);


    //Calculation variables
    size_t hexValueDigits;
    Byte kkValue = 0, ddValue = 0, lValue = 0, nValue = 0, oValue = 0, pValue = 0, qValue = 0, vvValue = 0, wwValue = 0, registerR = 0, registerX = 0, registerY = 0, registerZ = 0;
    char currentChar;
    bool commandFound, parameterFound;
    wxString tempStr1, tempStr2;
    wxString parameterStr, parameter, firstParameter, secondParameter, regNumberStr, detailsStr;
    
    buffer = " Illegal instruction";

    if (showOpcode)
    {
        if (pseudoType_ == "AMVBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020")
            addressStr.Printf("%04X: %02X%02X    ", dis_address, chip8_opcode1, chip8_opcode2);
        else
            addressStr.Printf("%04X: %02X%02X", dis_address, chip8_opcode1, chip8_opcode2);
    }
    else
    {
        if (pseudoType_ == "STIV")
            addressStr.Printf("%04X", dis_address);
        else
            addressStr.Printf("%03X", dis_address&0xfff);
    }

    for (size_t pseudoNr=0; pseudoNr<psuedoNumber_; pseudoNr++)
    {
        if (pseudoCodeDetails_[pseudoNr].command == command)
        {
            hexValueDigits = 0;
            commandFound = true;
            registerX = 0xff;
            
    //      for (int i=0; i<((pseudoCodeDetails_[pseudoNr].length*2)-1); i++)
            for (size_t i=0; i<pseudoCodeDetails_[pseudoNr].subCommand.Len(); i++)
            {
                currentChar = pseudoCodeDetails_[pseudoNr].subCommand.GetChar(i);
                
                if (currentChar == 'a' || currentChar == 'j' || currentChar == 'b') //&& hexValueDigits == 0)
                {
            //      if (i == 1)
                    hexValueDigits++;
            //      else
            //            hexValueDigits = pseudoCodeDetails_[pseudoNr].subCommand.length();
                }
                if (currentChar == 's' && i == 0)
                {
                    if (pseudoType_ == "SUPERCHIP")
                    {
                        if (address >= 0x200)
                            address += 0x600;
                    }
                    else
                    {
                        if (address >= 0x200)
                            address += 0x100;
                        address = address | ((address & 0x800) >> 1);
                    }
                }
                if (currentChar == 'd')
                {
                    ddValue = chip8_opcode2;
                }
                if (currentChar == 'k')
                {
                    kkValue = chip8_opcode2;
                }
                if (currentChar == 'l')
                {
                    lValue = nibble[i];
                }
                if (currentChar == 'n')
                {
                    nValue = nibble[i];
                }
                if (currentChar == 'o')
                {
                    oValue = nibble[i];
                    if (oValue <=7)
                        commandFound = false;
                }
                if (currentChar == 'p')
                {
                    pValue = nibble[i];
                    if (pValue >=8)
                        commandFound = false;
                }
                if (currentChar == 'q')
                {
                    oValue = nibble[i];
                    if (qValue >=8)
                        commandFound = false;
                }
                if (currentChar == 'r')
                {
                    registerR = nibble[i];
                }
                if (currentChar == 'v')
                {
                    vvValue = chip8_opcode3;
                }
                if (currentChar == 'w')
                {
                    wwValue = chip8_opcode4;
                }
                if (currentChar == 'x')
                {
                    if (registerX == 0xff)
                        registerX = nibble[i];
                    else
                    {
                        if (registerX != nibble[i])
                            commandFound = false;
                    }
                }
                if (currentChar == 'y')
                {
                    registerY = nibble[i];
                }
                if (currentChar == 'z')
                {
                    registerZ = nibble[i];
                }
                if (currentChar >= '0' && currentChar <= '9')
                {
                    if ((currentChar - '0') != nibble[i])
                        commandFound = false;
                }
                if (currentChar >= 'A' && currentChar <= 'F')
                {
                    if ((currentChar - 'A' + 10) != nibble[i])
                        commandFound = false;
                }
            }
            if (commandFound)
            {
                buffer = pseudoCodeDetails_[pseudoNr].commandText;
                
                if (showOpcode)
                {
                    switch (pseudoCodeDetails_[pseudoNr].length)
                    {
                        case 1:
                            if (pseudoType_ == "AMVBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020")
                                addressStr.Printf("%04X: %02X      ", dis_address, chip8_opcode1);
                            else
                                addressStr.Printf("%04X: %02X  ", dis_address, chip8_opcode1);
                        break;

                        case 3:
                            addressStr.Printf("%04X: %02X%02X%02X  ", dis_address, chip8_opcode1, chip8_opcode2, chip8_opcode3);
                        break;

                        case 4:
                            addressStr.Printf("%04X: %02X%02X%02X%02X", dis_address, chip8_opcode1, chip8_opcode2, chip8_opcode3, chip8_opcode4);
                        break;
                    }
                }

                wxString pseudoLine = pseudoCodeDetails_[pseudoNr].parameterText;
                wxString pseudoLineCopy = pseudoLine;
                firstParameter = extractWord(&pseudoLineCopy);
                pseudoLine = pseudoCodeDetails_[pseudoNr].parameterText;
                
                while (pseudoLine.Len() > 0)
                {
                    parameter = extractWord(&pseudoLine);
                    parameterFound = false;
                    if (parameter == "l")
                    {
                        parameterStr.Printf("%01X", lValue);
                        parameterFound = true;
                    }
                    if (parameter == "n")
                    {
                        parameterStr.Printf("%01X", nValue);
                        parameterFound = true;
                    }
                    if (parameter == "q" && qValue <= 7)
                    {
                        parameterStr.Printf("%01X", qValue);
                        parameterFound = true;
                    }
                    if (parameter == "p" && pValue <= 7)
                    {
                        parameterStr.Printf("%01X", pValue);
                        parameterFound = true;
                    }
                    if (parameter == "o" && oValue >= 8)
                    {
                        parameterStr.Printf("%01X", oValue-8);
                        parameterFound = true;
                    }
                    if (parameter == "kk")
                    {
                        parameterStr.Printf("%02X", kkValue);
                        parameterFound = true;
                    }
                    if (parameter == "vv")
                    {
                        parameterStr.Printf("%02X", vvValue);
                        parameterFound = true;
                    }
                    if (parameter == "ww")
                    {
                        parameterStr.Printf("%02X", wwValue);
                        parameterFound = true;
                    }
                    if (parameter.Left(5) == "c-hex")
                    {
                        address = 0x200 + (((ddValue & 0xf0) >> 4) * 10 + (ddValue & 0xf)) * 2;
                        parameterStr.Printf("%03X", address);
                        parameterFound = true;
                    }
                    if (parameter.Left(3) == "hex")
                    {
                        switch (hexValueDigits)
                        {
                            case 4:
                                address = (chip8_opcode2 << 8) + chip8_opcode3;
                                parameterStr.Printf("%04X", address);
                            break;
                                
                            case 3:
                                if (parameter.Mid(4,1) != "0")
                                {
                                    parameterStr.Printf("%03X", address);
                                    parameterStr = parameter.Mid(4,1) + parameterStr;
                                }
                                else
                                    parameterStr.Printf("%03X", address);
                            break;
                                
                            default:
                                switch (pseudoCodeDetails_[pseudoNr].length)
                                {
                                    case 4:
                                        parameterStr.Printf("%02X", chip8_opcode4);
                                    break;

                                    case 3:
                                        parameterStr.Printf("%02X", chip8_opcode3);
                                    break;
                                        
                                    default:
                                        parameterStr.Printf("%02X", address&0xff);
                                    break;
                                }
                                if (parameter.Mid(4,2) != "00")
                                {
                                    if (parameter.Mid(4,1) == "0")
                                    {
                                        if (pseudoType_ == "AMVBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020")
                                        {
                                            charBufferStr.Printf("%01X", (dis_address&0xF00)>>8);
                                            parameterStr = charBufferStr + parameterStr;
                                        }
                                        else
                                            parameterStr = parameter.Mid(5,1) + parameterStr;
                                    }
                                    else
                                        parameterStr = parameter.Mid(4,2) + parameterStr;
                                }
                            break;
                        }
                        parameterFound = true;
                    }
                    if (parameter.Left(5) == "s-hex")
                    {
                        parameterStr.Printf("%03X", address);
                        parameterFound = true;
                    }
                    if (parameter.Left(5) == "c-mem")
                    {
                        address = 0x200 + (((ddValue & 0xf0) >> 4) * 10 + (ddValue & 0xf)) * 2;
                         parameterStr.Printf("[%03X]", address);
                        parameterFound = true;
                    }
                    if (parameter.Left(3) == "mem")
                    {
                        if (hexValueDigits == 3)
                        {
                            parameterStr.Printf("%03X", address);
                            parameterStr = "["+ parameterStr + "]";
                        }
                        else
                        {
                            parameterStr.Printf("%02X", address&0xff);
                            if (parameter.Mid(5,1) == "0")
                                parameterStr = "["+ parameterStr + "]";
                            else
                            {
                                if (parameter.Mid(4,1) == "0")
                                    parameterStr = "["+ parameter.Mid(5,1) + parameterStr + "]";
                                else
                                    parameterStr = "["+ parameter.Mid(4,2) + parameterStr + "]";
                            }
                        }
                        parameterFound = true;
                    }
                    if (parameter.Left(3) == "mkk")
                    {
                        parameterStr.Printf("[%02X]", kkValue);
                        parameterFound = true;
                   }
                    if (parameter.Left(3) == "mvv")
                    {
                        parameterStr.Printf("[%02X]", vvValue);
                        parameterFound = true;
                    }
                    if (parameter.Left(3) == "mww")
                    {
                        parameterStr.Printf("[%02X]", wwValue);
                        parameterFound = true;
                    }
                    if (parameter == "Rr")
                    {
                        parameterStr.Printf("R%01X", registerR/2);
                        parameterFound = true;
                    }
                    if (parameter == "Rx")
                    {
                        parameterStr.Printf("R%01X", registerX/2);
                        parameterFound = true;
                    }
                    if (parameter == "Ry")
                    {
                        parameterStr.Printf("R%01X", registerY/2);
                        parameterFound = true;
                    }
                    if (parameter == "Rz")
                    {
                        parameterStr.Printf("R%01X", registerZ/2);
                        parameterFound = true;
                    }
                    if (parameter == "[Rx]")
                    {
                        parameterStr.Printf("[R%01X]", registerX/2);
                        parameterFound = true;
                    }
                    if (parameter == "[Ry]")
                    {
                        parameterStr.Printf("[R%01X]", registerY/2);
                        parameterFound = true;
                    }
                    if (parameter == "[Rz]")
                    {
                        parameterStr.Printf("[R%01X]", registerZ/2);
                        parameterFound = true;
                    }
                    if (parameter == "Vx")
                    {
                        parameterStr.Printf("V%01X", registerX);
                        parameterFound = true;
                    }
                    if (parameter == "Vy")
                    {
                        parameterStr.Printf("V%01X", registerY);
                        parameterFound = true;
                    }
                    if (parameter == "Vz")
                    {
                        parameterStr.Printf("V%01X", registerZ);
                        parameterFound = true;
                    }
                    if (parameter == "[Vx]")
                    {
                        parameterStr.Printf("[V%01X]", registerX);
                        parameterFound = true;
                    }
                    if (parameter == "[Vx.1]")
                    {
                        parameterStr.Printf("[V%01X.1]", registerX);
                        parameterFound = true;
                    }
                    if (parameter == "[Vy]")
                    {
                        parameterStr.Printf("[V%01X]", registerY);
                        parameterFound = true;
                    }
                    if (parameter == "[Vy.1]")
                    {
                        parameterStr.Printf("[V%01X.1]", registerY);
                        parameterFound = true;
                    }
                    if (parameter == "Vx1")
                    {
                        parameterStr.Printf("V%01X", registerX+1);
                        parameterFound = true;
                    }
                    if (parameter == "Vy1")
                    {
                        parameterStr.Printf("V%01X", registerY+1);
                        parameterFound = true;
                    }
                    if (parameter == "empty")
                    {
                        parameterFound = true;
                    }
                    if (parameter == ',')
                    {
                        parameterStr = ", ";
                        parameterFound = true;
                    }
                    if (parameter == '+')
                    {
                        parameterStr = "+";
                        parameterFound = true;
                    }
                    if (parameter == '-')
                    {
                        parameterStr = "-";
                        parameterFound = true;
                    }
                    if (parameter == '&')
                    {
                        parameterStr = "&";
                        parameterFound = true;
                    }
                    if (parameter == '=')
                    {
                        parameterStr = "=";
                        parameterFound = true;
                    }
                    if (parameter == '/')
                    {
                        parameterStr = "/";
                        parameterFound = true;
                    }
                    if (parameter.Left(3) == "str")
                    {
                        parameterFound = true;
                        parameterStr = "";
                        if (includeDetails)
                            additionalDetailsPrintStr_ = parameter.Right(parameter.Len()-4);
                    }
                    if (parameter.Left(3) == "det")
                    {
                        parameterFound = true;
                        parameterStr = "";
                        additionalChip8DetailsType_ = PSEUDO_DETAILS_X;
                        additionalDetailsAddressV2_ = 0;
                        if (includeDetails)
                        {
                            if (parameter.Right(2) == "VB")
                            {
                                additionalDetailsAddressV2_ = p_Computer->getChip8baseVar() + 0xB;
                                additionalDetailsPrintStrV2_ = ", VB=%02X";
                            }
                            if (firstParameter == "Rx")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressRX;
                                additionalDetailsPrintStr_.Printf("R%01X=", rX);
                                additionalDetailsPrintStr_ += "%04X";
                                additionalChip8DetailsType_ = PSEUDO_DETAILS_R;
                            }
                            if (firstParameter == "Ry")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressRY;
                                additionalDetailsPrintStr_.Printf("R%01X=", rY);
                                additionalDetailsPrintStr_ += "%04X";
                                additionalChip8DetailsType_ = PSEUDO_DETAILS_R;
                            }
                            if (firstParameter == "Rz")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressRZ;
                                additionalDetailsPrintStr_.Printf("R%01X=", rZ);
                                additionalDetailsPrintStr_ += "%04X";
                                additionalChip8DetailsType_ = PSEUDO_DETAILS_R;
                            }
                            if (firstParameter == "[Rx]")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressRX;
                                additionalDetailsPrintStr_.Printf("[%04X]=", (p_Computer->readMemDebug(addressRX)<<8) + p_Computer->readMemDebug(addressRX+1));
                                additionalDetailsPrintStr_ += "%02X";
                                additionalChip8DetailsType_ = PSEUDO_DETAILS_MR;
                            }
                            if (firstParameter == "[Ry]")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressRY;
                                additionalDetailsPrintStr_.Printf("[%04X]=", (p_Computer->readMemDebug(addressRY)<<8) + p_Computer->readMemDebug(addressRY+1));
                                additionalDetailsPrintStr_ += "%02X";
                                additionalChip8DetailsType_ = PSEUDO_DETAILS_MR;
                            }
                            if (firstParameter == "[Rz]")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressRZ;
                                additionalDetailsPrintStr_.Printf("[%04X]=", (p_Computer->readMemDebug(addressRZ)<<8) + p_Computer->readMemDebug(addressRZ+1));
                                additionalDetailsPrintStr_ += "%02X";
                                additionalChip8DetailsType_ = PSEUDO_DETAILS_MR;
                            }
                            if (firstParameter == "Vx")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressX;
                                additionalDetailsPrintStr_.Printf("V%01X=", vX);
                                additionalDetailsPrintStr_ += "%02X";
                            }
                            if (firstParameter == "Vy")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressY;
                                additionalDetailsPrintStr_.Printf("V%01X=", vY);
                                additionalDetailsPrintStr_ += "%02X";
                            }
                            if (firstParameter == "Vz")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressZ;
                                additionalDetailsPrintStr_.Printf("V%01X=", vZ);
                                additionalDetailsPrintStr_ += "%02X";
                            }
                            if (firstParameter == "[RA]")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressX;
                                additionalDetailsPrintStr_.Printf("[%03X]=", RegisterA);
                                additionalDetailsPrintStr_ += "%02X";
                            }
                            if (firstParameter == "[RB]")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressX;
                                additionalDetailsPrintStr_.Printf("[%03X]=", RegisterB);
                                additionalDetailsPrintStr_ += "%02X";
                            }
                            if (firstParameter == "[I]")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressX;
                                additionalDetailsPrintStr_.Printf("[%03X]=", valueI);
                                additionalDetailsPrintStr_ += "%02X";
                            }
                            if (firstParameter == "mem=0600-06FF")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressX;
                                additionalDetailsPrintStr_.Printf("[6%02X]=", chip8_opcode2);
                                additionalDetailsPrintStr_ += "%02X";
                            }
                            if (firstParameter == "mem=0800-08FF")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressX;
                                additionalDetailsPrintStr_.Printf("[8%02X]=", chip8_opcode2);
                                additionalDetailsPrintStr_ += "%02X";
                            }
                            if (firstParameter == "[Vy]")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressX;
                                if (parameter.Right(3) == "ST2")
                                    additionalDetailsPrintStr_.Printf("[%03X]=", 0x800+p_Computer->readMemDebug(addressY));
                                else
                                    additionalDetailsPrintStr_.Printf("[%04X]=", (p_Computer->readMemDebug(addressY)<<8) + p_Computer->readMemDebug(addressY+1));
                                additionalDetailsPrintStr_ += "%02X";
                            }
                            if (firstParameter == "ST")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressX;
                                additionalDetailsPrintStr_ = "ST=%d*20 ms";
                            }
                            if (firstParameter == "B")
                            {
                                secondParameter = extractWord(&pseudoLineCopy); // read ,
                                secondParameter = extractWord(&pseudoLineCopy); // read second parameter
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressX;
                                if (secondParameter == "Vx")
                                    additionalDetailsPrintStr_.Printf("[%03X->%03X]=", valueI, (valueI+2)&0xfff);
                                if (secondParameter == "Vy")
                                    additionalDetailsPrintStr_.Printf("V%01X->V%01X=", vY, vY+2);
                                if (secondParameter == "[RA]")
                                    additionalDetailsPrintStr_.Printf("[%03X->%03X]=", RegisterA, RegisterA+2);
                                if (secondParameter == "[Vy]")
                                {
                                    if (parameter.Right(3) == "ST2")
                                        additionalDetailsPrintStr_.Printf("[%03X->%03X]=", (valueY+0x800)&0xfff, (valueY+0x802)&0xfff);
                                    else
                                        additionalDetailsPrintStr_.Printf("[%04X->%04X]=", valueY+0x2700, valueY+0x2702);
                                }
                                additionalDetailsPrintStr_ += "'%03d'";
                            }
                            if (firstParameter == "I")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsPrintStr_ = "I=";
                                if (pseudoType_ == "STIV")
                                    additionalDetailsPrintStr_ += "%04X";
                                else
                                    additionalDetailsPrintStr_ += "%03X";
                                additionalChip8DetailsType_ = PSEUDO_DETAILS_I;
                            }
                            if (firstParameter == "[I]")
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = valueI;
                                if (pseudoType_ == "STIV")
                                    additionalDetailsPrintStr_.Printf("[%04X]=", valueI);
                                else
                                    additionalDetailsPrintStr_.Printf("[%03X]=", valueI);
                                additionalDetailsPrintStr_ += "%02X";
                                additionalChip8DetailsType_ = PSEUDO_DETAILS_MI;
                            }
                            if (!additionalChip8Details_)
                            {
                                additionalChip8Details_ = true;
                                additionalDetailsAddress_ = addressX;
                                additionalDetailsPrintStr_ = firstParameter + "=";
                                additionalDetailsPrintStr_ += "%02X";
                            }
                        }
                    }
                  
                    if (!parameterFound)
                        parameterStr = parameter;
                    if (pseudoType_ == "AMVBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020")
                        parameterStr.Replace(" ","",true);
                    buffer += parameterStr;
                }
                while (buffer.Len() < 21 && addressStr != "")
                    buffer += " ";
                buffer = addressStr + buffer;
                if (includeDetails)
                    buffer = buffer + detailsBuffer;
                return buffer;
            }
        }
    }
    
    while (buffer.Len() < 21 && addressStr != "")
        buffer += " ";
    buffer = addressStr + buffer;
    if (includeDetails)
        buffer = buffer + detailsBuffer;
    return buffer;
}

wxString DebugWindow::addDetails()
{
    wxString buffer, v2String;
    Word valueI;

    switch (additionalChip8DetailsType_)
    {
        case PSEUDO_DETAILS_X:
            buffer.Printf(additionalDetailsPrintStr_, p_Computer->readMemDebug(additionalDetailsAddress_));
            if (additionalDetailsAddressV2_ != 0)
            {
                v2String.Printf(additionalDetailsPrintStrV2_, p_Computer->readMemDebug(additionalDetailsAddressV2_));
                buffer += v2String;
            }
        break;
      
        case PSEUDO_DETAILS_R:
            buffer.Printf(additionalDetailsPrintStr_, (p_Computer->readMemDebug(additionalDetailsAddress_) << 8) + p_Computer->readMemDebug(additionalDetailsAddress_+1));
        break;
            
        case PSEUDO_DETAILS_MR:
            buffer.Printf(additionalDetailsPrintStr_, p_Computer->readMemDebug((p_Computer->readMemDebug(additionalDetailsAddress_) << 8) + p_Computer->readMemDebug(additionalDetailsAddress_+1)));
        break;
            
        case PSEUDO_DETAILS_I:
            if (pseudoType_ == "STIV")
                valueI = (p_Computer->readMemDebug(0x27f6)<<8)+p_Computer->readMemDebug(0x27f7);
            else
                valueI = p_Computer->getScratchpadRegister(CHIP8_I) & 0xfff;
            buffer.Printf(additionalDetailsPrintStr_, valueI);
        break;

        case PSEUDO_DETAILS_MI:
            if (pseudoType_ == "STIV")
                valueI = (p_Computer->readMemDebug(0x27f6)<<8)+p_Computer->readMemDebug(0x27f7);
            else
                valueI = p_Computer->getScratchpadRegister(CHIP8_I) & 0xfff;
            buffer.Printf(additionalDetailsPrintStr_, p_Computer->readMemDebug(additionalDetailsAddress_));
        break;
    }
    additionalChip8Details_ = false;
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
//    wxSetWorkingDirectory (workingDir_);
//    int num = 0;
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
                               wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_OVERWRITE_PROMPT,
                               this
                              );

    if (!fileName || fileName.empty())
        return;

    wxFileName FullPath = wxFileName(fileName, wxPATH_NATIVE);
    wxString name = FullPath.GetName();
    wxString path = FullPath.GetPath();
    wxString ext = FullPath.GetExt();

/*    while(wxFile::Exists(fileName))
    {
        num++;
        number.Printf("%d", num);
        fileName = path + pathSeparator_ + name + "." + number + "." + ext;
    }*/
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

    long chip8BreakPointAddress = get16BitValue("Chip8BreakPointAddress");
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


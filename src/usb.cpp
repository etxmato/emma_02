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

#include "main.h"
#include "usb.h"
#include "encrypt.h"
#include "mac.h"
//#include "serial.h"
#include "wx/sstream.h"
#include "wx/uilocale.h"
#include "wx/uri.h"
#include "http.h"

int dummyHeader_ = 0;

wxString stringConversion[] =
{
    "#",
    "~",
    ";",
    ":",
    "select",
    "describe",
    "insert",
    "update",
    "delete",
    "drop",
    "stop"
};

#define USB_IDLE 0
#define USB_HEADER 17
#define USB_SAVE_HEADER 11
#define USB_LOAD_HEADER 12
#define USB_CD_HEADER 13
#define USB_MKDIR_HEADER 14
#define USB_CAT_HEADER 15
#define USB_SAVE 1
#define USB_SAVE_OVERWRITE 0x81
#define USB_LOAD 2
#define USB_LOAD_DATA 0x42
#define USB_LOAD_RUN 0xc2
#define USB_LOAD_CONTINUE 0xa
#define USB_LOAD_CONTINUE_RUN 0xca
#define USB_CAT 3
#define USB_CAT_NEXT 4
#define USB_CD 5
#define USB_MKDIR 6
#define USB_RMDIR 7
#define USB_DEL 8
#define USB_CLOCK 9
#define USB_GET_TRACK 0x12
#define USB_GET_TRACK_HEADER 0x13
#define USB_GET_TRACK_CONTINUE 0x1a
#define USB_PUT_TRACK 0x14
#define USB_PUT_TRACK_HEADER 0x15
#define USB_IMG_TEST 0x16
#define USB_IMG_TEST_HEADER 0x17
#define USB_COMP_FILE 0x18
#define USB_URL 0x19
#define USB_URL_HEADER 0x1b
#define USB_URL_CONTINUE 0x1c
#define USB_URL_LINK 0x1d
#define USB_EMAIL 0x1e
#define USB_EMAIL_SEND 0x1f
#define USB_CANCEL 0xff
#define HEADER_LENGTH 64
#define HEADER_LONG_LENGTH 128
#define USB_SAVE_NVALL 0x20
#define USB_SAVE_NVDAT 0x21
#define USB_SAVE_NVOPT 0x22
#define USB_SAVE_NVSYNC 0x23
#define USB_LOAD_URL 0x24
#define USB_SAVE_URL_INPUT 0x25
#define USB_SAVE_BANK 0x30
#define USB_SAVE_BANK_ALL 0x31
#define USB_SAVE_BANK_ALL_CONT 0x32

#define HEADER_START 0
#define HEADER_MODE 0
#define HEADER_NUMBER 0
#define HEADER_FILENAME 1 // 1-18
#define HEADER_RESULT 19
#define HEADER_LENGTH_H 20
#define HEADER_LENGTH_L 21
#define HEADER_TYPE 22
#define HEADER_START_H 23
#define HEADER_START_L 24
#define HEADER_END_H 25
#define HEADER_END_L 26
#define HEADER_EXEC_H 27
#define HEADER_EXEC_L 28
#define HEADER_STRING_H 23
#define HEADER_STRING_L 24
#define HEADER_ARRAY_H 25
#define HEADER_ARRAY_L 26
#define HEADER_EOD_H 27
#define HEADER_EOD_L 28
#define HEADER_FILENAME2 29 // 29-46
#define HEADER_CATEGORY 47
#define HEADER_CHAR 48
#define HEADER_START_HIGH_BYTE 49
#define HEADER_BLOAD_TYPE 50        // B2h
#define HEADER_BLOAD_LENGTH_H 51    // B3h
#define HEADER_BLOAD_LENGTH_L 52    // B4h
#define HEADER_BUILD_NUMBER 53    // B5h + B6h + B7h
#define HEADER_URL_COMMAND 1
#define HEADER_URL_SCREEN_H 2
#define HEADER_URL_BOOKMARK 2
#define HEADER_URL_LINK 3
#define HEADER_URL_SCREEN_L 3
//#define HEADER_COLOUR_INFO 0x34
//#define HEADER_RESHAPE 0x35
//#define HEADER_RESHAPE_INFO 0x36

#define URL_COMMAND_LINK 0
#define URL_COMMAND_BACK 1
#define URL_COMMAND_HOME 2
#define URL_COMMAND_SET_HOME 3
#define URL_COMMAND_BOOKMARK 4
#define URL_COMMAND_SET_BOOKMARK 5
#define URL_COMMAND_HELP 6

#define HEADER_DENSITY 20
#define HEADER_SIDES 21
#define HEADER_SIDE 23
#define HEADER_TRACK 24
#define HEADER_SIDE_NUMBER 25

#define IMG_SIDE 0x800
#define IMG_TRACK 0x801
#define IMG_SIDE_NUMBER 0x802
#define IMG_TYPE 0x803

#define USB_FILE_EXISTS 0
#define USB_SEARCHING 1
#define USB_SS_SD 0xad
#define USB_SS_DD 0xae
#define USB_DS_SD 0xaf
#define USB_FILE_NOT_FOUND 0xff
#define USB_ADDRESS_ERROR 0xfe
#define USB_TYPE_ERROR 0xfd
#define USB_FILE_TOO_SMALL_64 0xfc
#define USB_FILE_TOO_SMALL_8 0xfb
#define USB_FILE_TOO_LARGE_64 0xfa
#define USB_FILE_TOO_LARGE_8 0xf9
#define USB_FILE_ERROR 0xf8
#define USB_FILE_OPEN 0xf7
#define USB_NETWORK_ERROR 0xf6
#define USB_SERVER_ERROR 0xf5
#define USB_NO_DATA 0xf4
#define USB_URL_NOTFOUND 0xf3

#define TYPE_BIN 0x10
#define TYPE_TXT 0x11
#define TYPE_IMG 0x12
#define TYPE_RAM 0x13
#define TYPE_ALL 0x14
#define TYPE_COMX_BAK 0x20
#define TYPE_BIN_BAK 0x30
#define TYPE_TXT_BAK 0x31
#define TYPE_IMG_BAK 0x32
#define TYPE_RAM_BAK 0x33
#define TYPE_ALL_BAK 0x34

#define MAKELOWER 0
#define MAKEUPPER 1
#define CAPITALIZE 2

#define SW_ID_HAP1 1
#define SW_ID_HAP2 2
#define SW_ID_HIT 3
#define SW_ID_SPACE 4
#define SW_ID_WORM 5
#define SW_ID_GYG 6
#define SW_ID_GATE 7
#define SW_ID_DRAGON 8
#define SW_ID_CROSS 9
#define SW_ID_LOOK 10
#define SW_ID_TENNIS 11
#define SW_ID_MUSIC 12

#define TABLE_CR 0
#define TABLE_A 127
#define TABLE_HIGH_COLOUR 31

int dummyHeaderValues[8][64] =
{
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA},
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
    {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F},
    {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F},
    {0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F},
    {0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF},
    {0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}
};

char bullets[11] =
{
    '-', '+', '*', '-', '+', '*', '-', '+', '*', '-', '+'
};

#define HTML_SHAPE 103 // location of shpae 1 to 31
#define HTML_UTF 134 // location of UTF-8 characters
#define HTML_LAST 224 // Number of htmlCodes
#define HTML_CODE_LAST 94 // Number of htmlNumbers
#define HTML_FREE_SHAPE 26 // Number of 'free' shapes + 1
#define HTML_START 161 // Start number of the first special html character

#define COLOR_NONE 0
#define COLOR_ON_PREVIOUS_PAGE 1
#define COLOR_ON_THIS_PAGE 2
#define COLOR_MULTIPLE 3
#define COLOR_RESHAPE 4

#define TAG_OL_TYPE1 0
#define TAG_OL_TYPEA 1
#define TAG_OL_TYPEa 2
#define TAG_OL_TYPEI 3
#define TAG_OL_TYPEi 4
#define TAG_IL_DASH 5
#define TAG_IL_DISC 6
#define TAG_IL_CIRCLE 7
#define TAG_IL_SQUARE 8

#define CHAR_SET_STD 0
#define CHAR_SET_UTF_8 1

#define INPUT_TYPE_NONE 0
#define INPUT_TYPE_TEXT 1
#define INPUT_TYPE_IMAGE 2
#define INPUT_TYPE_BUTTON 3
#define INPUT_TYPE_SUBMIT 4
#define INPUT_TYPE_CHECKBOX 5
#define INPUT_TYPE_RADIO 6
#define INPUT_TYPE_PASSWD 7
#define INPUT_TYPE_MESSAGE 8
#define INPUT_TYPE_HIDDEN 9

#define URL_RADIO_OFF 0x1b
#define URL_RADIO_ON 0x1c
#define URL_BOX 0x1d
#define URL_TICK 0x1e

#define URL_LINK true
#define URL_NO_LINK false
#define URL_HIGH_COLOR true
#define URL_NORMAL_COLOR false
#define URL_RADIO_INPUT true
#define URL_NO_RADIO_INPUT false
#define URL_NEW_LINE true
#define URL_NO_NEW_LINE false
#define URL_BREAK_LINE true
#define URL_DO_NOT_BREAK_LINE false

wxString htmlCodes[] =
{
    "&iexcl;",
    "&cent;",
    "&pound;",
    "&curren;",
    "&yen;",
    "&brvbar;",
    "&sect;",
    "&uml;",
    "&copy;",
    "&ordf;",
    "&laquo;",
    "&not;",
    "&shy;",
    "&reg;",
    "&macr;",
    "&deg;",
    "&plusmn;",
    "&sup2;",
    "&sup3;",
    "&acute;",
    "&micro;",
    "&para;",
    "&middot;",
    "&cedil;",
    "&sup1;",
    "&ordm;",
    "&raquo;",
    "&frac14;",
    "&frac12;",
    "&frac34;",
    "&iquest;",
    "&Agrave;",
    "&Aacute;",
    "&Acirc;",
    "&Atilde;",
    "&Auml;",
    "&Aring;",
    "&AElig;",
    "&Ccedil;",
    "&Egrave;",
    "&Eacute;",
    "&Ecirc;",
    "&Euml;",
    "&Igrave;",
    "&Iacute;",
    "&Icirc;",
    "&Iuml;",
    "&ETH;",
    "&Ntilde;",
    "&Ograve;",
    "&Oacute;",
    "&Ocirc;",
    "&Otilde;",
    "&Ouml;",
    "&times;",
    "&Oslash;",
    "&Ugrave;",
    "&Uacute;",
    "&Ucirc;",
    "&Uuml;",
    "&Yacute;",
    "&THORN;",
    "&szlig;",
    "&agrave;",
    "&aacute;",
    "&acirc;",
    "&atilde;",
    "&auml;",
    "&aring;",
    "&aelig;",
    "&ccedil;",
    "&egrave;",
    "&eacute;",
    "&ecirc;",
    "&euml;",
    "&igrave;",
    "&iacute;",
    "&icirc;",
    "&iuml;",
    "&eth;",
    "&ntilde;",
    "&ograve;",
    "&oacute;",
    "&ocirc;",
    "&otilde;",
    "&ouml;",
    "&divide;",
    "&oslash;",
    "&ugrave;",
    "&uacute;",
    "&ucirc;",
    "&uuml;",
    "&yacute;",
    "&thorn;",
    "&yuml;",
    "&hr;",
    "&disc;",
    "&circle;",
    "&square;",
    "&radio_off;",
    "&radio_on;",
    "&box;",
    "&tick;",
    "&shape1;",
    "&shape2;",
    "&shape3;",
    "&shape4;",
    "&shape5;",
    "&shape6;",
    "&shape7;",
    "&shape8;",
    "&shape9;",
    "&shape10;",
    "&shape11;",
    "&shape12;",
    "&shape13;",
    "&shape14;",
    "&shape15;",
    "&shape16;",
    "&shape17;",
    "&shape18;",
    "&shape19;",
    "&shape20;",
    "&shape21;",
    "&shape22;",
    "&shape23;",
    "&shape24;",
    "&shape25;",
    "&shape26;",
    "&shape27;",
    "&shape28;",
    "&shape29;",
    "&shape30;",
    "&shape31;",
    "&OElig;",    //338
    "&oelig;",    //339
    "&Scaron;",    //352
    "&scaron;",    //353
    "&Yuml;",    //376
    "&fnof;",    //402
    "&circ;",    //710
    "&tilde;",    //732
    "&ndash;",    //8211
    "&mdash;",    //8212
    "&lsquo;",    //8216
    "&rsquo;",    //8217
    "&sbquo;",    //8218
    "&ldquo;",    //8220
    "&rdquo;",    //8221
    "&bdquo;",    //8222
    "&dagger;",    //8224
    "&Dagger;",    //8225
    "&bull;",    //8226
    "&hellip;",    //8230
    "&permil;",    //8240
    "&prime;",    //8242
    "&Prime;",    //8243
    "&lsaquo;",    //8249
    "&rsaquo;",    //8250
    "&oline;",    //8254
    "&frasl;",    //8260
    "&euro;",    //8364
    "&image;",    //8465
    "&weierp;",    //8472
    "&real;",    //8476
    "&trade;",    //8482
    "&alefsym;",//8501
    "&larr;",    //8592
    "&uarr;",    //8593
    "&rarr;",    //8594
    "&darr;",    //8595
    "&harr;",    //8596
    "&crarr;",    //8629
    "&lArr;",    //8656
    "&uArr;",    //8657
    "&rArr;",    //8658
    "&dArr;",    //8659
    "&hArr;",    //8660
    "&forall;",
    "&part;",
    "&exist;",
    "&empty;",
    "&nabla;",
    "&isin;",
    "&notin;",
    "&ni;",
    "&prod;",
    "&sum;",
    "&minus;",
    "&lowast;",
    "&radic;",
    "&prop;",
    "&infin;",
    "&ang;",
    "&and;",
    "&or;",
    "&cap;",
    "&cup;",
    "&int;",
    "&there4;",
    "&sim;",
    "&cong;",
    "&asymp;",
    "&ne;",
    "&equiv;",
    "&le;",
    "&ge;",
    "&sub;",
    "&sup;",
    "&nsub;",
    "&sube;",
    "&supe;",
    "&oplus;",
    "&otimes;",
    "&perp;",
    "&sdot;",
    "&vellip;",
    "&lceil;",
    "&rceil;",
    "&lfloor;",
    "&rfloor;",
    "&lang;",
    "&rang;",
    "&loz;",
    "&spades;",
    "&clubs;",
    "&hearts;",
    "&diams;",
};


wxString htmlNumbers[] =
{
    "&#338;",
    "&#339;",
    "&#352;",
    "&#353;",
    "&#376;",
    "&#402;",
    "&#710;",
    "&#732;",
    "&#8211;",
    "&#8212;",
    "&#8216;",
    "&#8217;",
    "&#8218;",
    "&#8220;",
    "&#8221;",
    "&#8222;",
    "&#8224;",
    "&#8225;",
    "&#8226;",
    "&#8230;",
    "&#8240;",
    "&#8242;",
    "&#8243;",
    "&#8249;",
    "&#8250;",
    "&#8254;",
    "&#8260;",
    "&#8364;",
    "&#8465;",
    "&#8472;",
    "&#8476;",
    "&#8482;",
    "&#8501;",
    "&#8592;",
    "&#8593;",
    "&#8594;",
    "&#8595;",
    "&#8596;",
    "&#8629;",
    "&#8656;",
    "&#8657;",
    "&#8658;",
    "&#8659;",
    "&#8660;",
    "&#8704;",
    "&#8706;",
    "&#8707;",
    "&#8709;",
    "&#8711;",
    "&#8712;",
    "&#8713;",
    "&#8715;",
    "&#8719;",
    "&#8721;",
    "&#8722;",
    "&#8727;",
    "&#8730;",
    "&#8733;",
    "&#8734;",
    "&#8736;",
    "&#8743;",
    "&#8744;",
    "&#8745;",
    "&#8746;",
    "&#8747;",
    "&#8756;",
    "&#8764;",
    "&#8773;",
    "&#8776;",
    "&#8800;",
    "&#8801;",
    "&#8804;",
    "&#8805;",
    "&#8834;",
    "&#8835;",
    "&#8836;",
    "&#8838;",
    "&#8839;",
    "&#8853;",
    "&#8855;",
    "&#8869;",
    "&#8901;",
    "&#8942;",
    "&#8968;",
    "&#8969;",
    "&#8970;",
    "&#8971;",
    "&#9001;",
    "&#9002;",
    "&#9674;",
    "&#9824;",
    "&#9827;",
    "&#9829;",
    "&#9830;",
};

Byte shapeValues[][9] =
{
    {0xc8, 0x00, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0x00, 0x00}, // &iexcl;
    {0x00, 0xc8, 0xde, 0xe8, 0xe8, 0xe8, 0xde, 0xc8, 0x00}, // &cent;
    {0xcc, 0xd2, 0xd0, 0xf8, 0xd0, 0xd2, 0xec, 0x00, 0x00}, // &pound;
    {0x00, 0x00, 0xe2, 0xdc, 0xd4, 0xdc, 0xe2, 0x00, 0x00}, // &curren;
    {0xe2, 0xe2, 0xd4, 0xc8, 0xfe, 0xc8, 0xc8, 0xc8, 0x00}, // &yen;
    {0xc8, 0xc8, 0xc8, 0x00, 0xc8, 0xc8, 0xc8, 0x00, 0x00}, // &brvbar;
    {0xdc, 0xe0, 0xf8, 0xe4, 0xd2, 0xce, 0xc2, 0xdc, 0x00}, // &sect;
    {0xd4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &uml;
    {0xcc, 0xd2, 0xed, 0xe9, 0xed, 0xd2, 0xcc, 0x00, 0x00}, // &copy;
    {0xce, 0xd2, 0xd6, 0xca, 0x00, 0xde, 0x00, 0x00, 0x00}, // &ordf;
    {0x00, 0x00, 0xc9, 0xd2, 0xe4, 0xd2, 0xc9, 0x00, 0x00}, // &laquo;
    {0x00, 0x00, 0x00, 0xde, 0xc2, 0x00, 0x00, 0x00, 0x00}, // &not;
    {0x00, 0x00, 0x00, 0xde, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shy;
    {0xcc, 0xd2, 0xed, 0xe9, 0xe9, 0xd2, 0xcc, 0x00, 0x00}, // &reg;
    {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &macr;
    {0x00, 0xc8, 0xd4, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00}, // &deg;
    {0x00, 0x00, 0xc8, 0xc8, 0xfe, 0xc8, 0xc8, 0xfe, 0x00}, // &plusmn;
    {0xcc, 0xd2, 0xc4, 0xc8, 0xde, 0x00, 0x00, 0x00, 0x00}, // &sup2;
    {0xdc, 0xc2, 0xcc, 0xc2, 0xdc, 0x00, 0x00, 0x00, 0x00}, // &sup3;
    {0xc2, 0xc4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &acute;
    {0x00, 0x00, 0x00, 0xe2, 0xe2, 0xe2, 0xf2, 0xec, 0xe0}, // &micro;
    {0x00, 0xde, 0xfa, 0xfa, 0xda, 0xca, 0xca, 0xca, 0x00}, // &para;
    {0x00, 0x00, 0x00, 0x00, 0xc8, 0x00, 0x00, 0x00, 0x00}, // &middot;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0xc4}, // &cedil;
    {0xc4, 0xcc, 0xc4, 0xc4, 0xce, 0x00, 0x00, 0x00, 0x00}, // &sup1;
    {0x00, 0xcc, 0xd2, 0xd2, 0xcc, 0x00, 0xde, 0x00, 0x00}, // &ordm;
    {0x00, 0x00, 0xe4, 0xd2, 0xc9, 0xd2, 0xe4, 0x00, 0x00}, // &raquo;
    {0xd0, 0xf0, 0xd0, 0xf8, 0xc2, 0xc6, 0xca, 0xde, 0xc2}, // &frac14;
    {0xd0, 0xf0, 0xd0, 0xf8, 0xc4, 0xca, 0xc2, 0xc4, 0xce}, // &frac12;
    {0xf0, 0xc8, 0xd0, 0xc8, 0xf2, 0xc6, 0xca, 0xde, 0xc2}, // &frac34;
    {0xc8, 0x00, 0xc8, 0xc8, 0xd0, 0xe2, 0xdc, 0x00, 0x00}, // &iquest;
    {0xd0, 0xc8, 0xdc, 0xe2, 0xfe, 0xe2, 0xe2, 0x00, 0x00}, // &Agrave;
    {0xc4, 0xc8, 0xdc, 0xe2, 0xfe, 0xe2, 0xe2, 0x00, 0x00}, // &Aacute;
    {0xc8, 0xd4, 0xdc, 0xe2, 0xfe, 0xe2, 0xe2, 0x00, 0x00}, // &Acirc;
    {0xca, 0xd4, 0xdc, 0xe2, 0xfe, 0xe2, 0xe2, 0x00, 0x00}, // &Atilde;
    {0xd4, 0x00, 0xdc, 0xe2, 0xfe, 0xe2, 0xe2, 0x00, 0x00}, // &Auml;
    {0xc8, 0x00, 0xdc, 0xe2, 0xfe, 0xe2, 0xe2, 0x00, 0x00}, // &Aring;
    {0xce, 0xd4, 0xe4, 0xe6, 0xfc, 0xe4, 0xe6, 0x00, 0x00}, // &AElig;
    {0xdc, 0xe2, 0xe0, 0xe0, 0xe0, 0xe2, 0xdc, 0xc8, 0xd0}, // &Ccedil;
    {0xd0, 0xc8, 0xfe, 0xe0, 0xfc, 0xe0, 0xfe, 0x00, 0x00}, // &Egrave;
    {0xc4, 0xc8, 0xfe, 0xe0, 0xfc, 0xe0, 0xfe, 0x00, 0x00}, // &Eacute;
    {0xc8, 0xd4, 0xfe, 0xe0, 0xfc, 0xe0, 0xfe, 0x00, 0x00}, // &Ecirc;
    {0xd4, 0x00, 0xfe, 0xe0, 0xfc, 0xe0, 0xfe, 0x00, 0x00}, // &Euml;
    {0xd0, 0xc8, 0xdc, 0xc8, 0xc8, 0xc8, 0xdc, 0x00, 0x00}, // &Igrave;
    {0xc4, 0xc8, 0xdc, 0xc8, 0xc8, 0xc8, 0xdc, 0x00, 0x00}, // &Iacute;
    {0xc8, 0xd4, 0xdc, 0xc8, 0xc8, 0xc8, 0xdc, 0x00, 0x00}, // &Icirc;
    {0xd4, 0x00, 0xdc, 0xc8, 0xc8, 0xc8, 0xdc, 0x00, 0x00}, // &Iuml;
    {0xfc, 0xd2, 0xd2, 0xfa, 0xd2, 0xd2, 0xfc, 0x00, 0x00}, // &ETH;
    {0xca, 0xd4, 0xe2, 0xf2, 0xea, 0xe6, 0xe2, 0x00, 0x00}, // &Ntilde;
    {0xd0, 0xc8, 0xdc, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &Ograve;
    {0xc4, 0xc8, 0xdc, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &Oacute;
    {0xc8, 0xd4, 0xdc, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &Ocirc;
    {0xca, 0xd4, 0xdc, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &Otilde;
    {0xd4, 0x00, 0xdc, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &Ouml;
    {0x00, 0x00, 0xe2, 0xd4, 0xc8, 0xd4, 0xe2, 0x00, 0x00}, // &times;
    {0xdc, 0xe6, 0xe6, 0xea, 0xf2, 0xf2, 0xdc, 0x00, 0x00}, // &Oslash;
    {0xd0, 0xc8, 0xe2, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &Ugrave;
    {0xc4, 0xc8, 0xe2, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &Uacute;
    {0xc8, 0xd4, 0x00, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &Ucirc;
    {0xd4, 0x00, 0xe2, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &Uuml;
    {0xc4, 0xc8, 0xe2, 0xe2, 0xd4, 0xc8, 0xc8, 0x00, 0x00}, // &Yacute;
    {0xe0, 0xfc, 0xe2, 0xfc, 0xe0, 0xe0, 0xe0, 0x00, 0x00}, // &THORN;
    {0xdc, 0xe2, 0xe4, 0xe8, 0xe4, 0xe2, 0xec, 0x00, 0x00}, // &szlig;
    {0xd0, 0xc8, 0xdc, 0xc2, 0xde, 0xe2, 0xde, 0x00, 0x00}, // &agrave;
    {0xc4, 0xc8, 0xdc, 0xc2, 0xde, 0xe2, 0xde, 0x00, 0x00}, // &aacute;
    {0xc8, 0xd4, 0xdc, 0xc2, 0xde, 0xe2, 0xde, 0x00, 0x00}, // &acirc;
    {0xca, 0xd4, 0xdc, 0xc2, 0xde, 0xe2, 0xde, 0x00, 0x00}, // &atilde;
    {0xd4, 0x00, 0xdc, 0xc2, 0xde, 0xe2, 0xde, 0x00, 0x00}, // &auml;
    {0xc8, 0x00, 0xdc, 0xc2, 0xde, 0xe2, 0xde, 0x00, 0x00}, // &aring;
    {0x00, 0x00, 0xdc, 0xca, 0xde, 0xe8, 0xde, 0x00, 0x00}, // &aelig;
    {0x00, 0x00, 0xdc, 0xe2, 0xe0, 0xe2, 0xdc, 0xc8, 0xd0}, // &ccedil;
    {0xd0, 0xc8, 0xdc, 0xe2, 0xfe, 0xe0, 0xdc, 0x00, 0x00}, // &egrave;
    {0xc4, 0xc8, 0xdc, 0xe2, 0xfe, 0xe0, 0xdc, 0x00, 0x00}, // &eacute;
    {0xc8, 0xd4, 0xdc, 0xe2, 0xfe, 0xe0, 0xdc, 0x00, 0x00}, // &ecirc;
    {0xd4, 0x00, 0xdc, 0xe2, 0xfe, 0xe0, 0xdc, 0x00, 0x00}, // &euml;
    {0xd0, 0xc8, 0xd8, 0xc8, 0xc8, 0xc8, 0xdc, 0x00, 0x00}, // &igrave;
    {0xc8, 0xd0, 0xd8, 0xc8, 0xc8, 0xc8, 0xdc, 0x00, 0x00}, // &iacute;
    {0xc8, 0xd4, 0xd8, 0xc8, 0xc8, 0xc8, 0xdc, 0x00, 0x00}, // &icirc;
    {0xd4, 0x00, 0xd8, 0xc8, 0xc8, 0xc8, 0xdc, 0x00, 0x00}, // &iuml;
    {0xf0, 0xcc, 0xdc, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &eth;
    {0xca, 0xd4, 0xec, 0xf2, 0xe2, 0xe2, 0xe2, 0x00, 0x00}, // &ntilde;
    {0xd0, 0xc8, 0xdc, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &ograve;
    {0xc4, 0xc8, 0xdc, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &oacute;
    {0xc8, 0xd4, 0xdc, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &ocirc;
    {0xca, 0xd4, 0xdc, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &otilde;
    {0xd4, 0x00, 0xdc, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &ouml;
    {0x00, 0xc8, 0x00, 0xfe, 0x00, 0xc8, 0x00, 0x00, 0x00}, // &divide;
    {0x00, 0x00, 0xde, 0xe6, 0xea, 0xf2, 0xfc, 0x00, 0x00}, // &oslash;
    {0xd0, 0xc8, 0xe2, 0xe2, 0xe2, 0xe6, 0xda, 0x00, 0x00}, // &ugrave;
    {0xc4, 0xc8, 0xe2, 0xe2, 0xe2, 0xe6, 0xda, 0x00, 0x00}, // &uacute;
    {0xc8, 0xd4, 0x00, 0xe2, 0xe2, 0xe6, 0xda, 0x00, 0x00}, // &ucirc;
    {0xd4, 0x00, 0xe2, 0xe2, 0xe2, 0xe6, 0xda, 0x00, 0x00}, // &uuml;
    {0xc4, 0xc8, 0xe2, 0xe2, 0xe6, 0xda, 0xc2, 0xdc, 0x00}, // &yacute;
    {0x00, 0xe0, 0xfc, 0xe2, 0xe2, 0xfc, 0xe0, 0xe0, 0x00}, // &thorn;
    {0xd4, 0x00, 0xe2, 0xe2, 0xe6, 0xda, 0xc2, 0xdc, 0x00}, // &yuml;
    {0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00}, // &hr;
    {0x00, 0xdc, 0xfe, 0xfe, 0xfe, 0xdc, 0x00, 0x00, 0x00}, // &disc;
    {0x00, 0xdc, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00, 0x00}, // &circle;
    {0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x00, 0x00, 0x00}, // &square;
    {0x00, 0xdc, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00, 0x00}, // &radio_off;
    {0x00, 0xdc, 0xe2, 0xea, 0xe2, 0xdc, 0x00, 0x00, 0x00}, // &radio_on;
    {0x00, 0xfe, 0xe2, 0xe2, 0xe2, 0xfe, 0x00, 0x00, 0x00}, // &box;
    {0x00, 0xfe, 0xe2, 0xea, 0xe2, 0xfe, 0x00, 0x00, 0x00}, // &tick;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape1;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape2;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape3;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape4;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape5;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape6;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape7;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape8;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape9;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape10;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape11;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape12;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape13;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape14;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape15;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape16;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape17;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape18;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape19;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape20;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape21;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape22;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape23;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape24;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape25;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape26:
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape27;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape28;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape29;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape30;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &shape31;
    {0xde, 0xe4, 0xe4, 0xe6, 0xe4, 0xe4, 0xde, 0x00, 0x00}, // &OElig;
    {0x00, 0x00, 0xd6, 0xea, 0xee, 0xe8, 0xd6, 0x00, 0x00}, // &oelig;
    {0xd4, 0xc8, 0xde, 0xe0, 0xdc, 0xc2, 0xfc, 0x00, 0x00}, // &Scaron;
    {0xd4, 0xc8, 0xdc, 0xe0, 0xdc, 0xc2, 0xdc, 0x00, 0x00}, // &scaron;
    {0xd4, 0x00, 0xe2, 0xe2, 0xd4, 0xc8, 0xc8, 0x00, 0x00}, // &Yuml;
    {0xc2, 0xc4, 0xce, 0xc4, 0xc4, 0xc8, 0xc8, 0xf0, 0x00}, // &fnof;
    {0xc8, 0xd4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &circ;
    {0xd0, 0xea, 0xc4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &tilde;
    {0x00, 0x00, 0x00, 0x00, 0xde, 0x00, 0x00, 0x00, 0x00}, // &ndash; en dash
    {0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00}, // &mdash; em dash
    {0xc2, 0xc4, 0xc4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &lsquo; left single quotation mark
    {0xc8, 0xc8, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &rsquo; right single quotation mark
    {0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0xc8, 0xd0, 0x00}, // &sbquo; single low-9 quotation mark
    {0xc9, 0xd2, 0xd2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &ldquo; left doouble quotation mark
    {0xd2, 0xd2, 0xe4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &rdquo; right doouble quotation mark
    {0x00, 0x00, 0x00, 0x00, 0x00, 0xd2, 0xd2, 0xe4, 0x00}, // &bdquo; double low-9 quotation mark
    {0xc8, 0xfe, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0x00}, // &dagger;
    {0xc8, 0xfe, 0xc8, 0xc8, 0xc8, 0xc8, 0xfe, 0xc8, 0x00}, // &Dagger;
    {0x00, 0xdc, 0xfe, 0xfe, 0xfe, 0xdc, 0x00, 0x00, 0x00}, // &bull;
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xea, 0x00, 0x00}, // &hellip;
    {0xe2, 0xc2, 0xc4, 0xc8, 0xd0, 0xe0, 0xea, 0x00, 0x00}, // &permil;
    {0xd0, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &prime;
    {0xd2, 0xe4, 0xe4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &Prime;
    {0x00, 0xc4, 0xc8, 0xd0, 0xc8, 0xc4, 0x00, 0x00, 0x00}, // &lsaquo;
    {0x00, 0xd0, 0xc8, 0xc4, 0xc8, 0xd0, 0x00, 0x00, 0x00}, // &rsaquo;
    {0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // &oline;
    {0x00, 0xc2, 0xc4, 0xc8, 0xd0, 0xe0, 0x00, 0x00, 0x00}, // &frasl;
    {0xcc, 0xd2, 0xf8, 0xd0, 0xf8, 0xd2, 0xcc, 0x00, 0x00}, // &euro;
    {0xde, 0xe4, 0xd4, 0xc2, 0xc2, 0xd2, 0xce, 0x00, 0x00}, // &image;
    {0xec, 0xd2, 0xd2, 0xdc, 0xd0, 0xd0, 0xd0, 0x00, 0x00}, // &weierp;
    {0xec, 0xd2, 0xd2, 0xdc, 0xd8, 0xd4, 0xd2, 0x00, 0x00}, // &real;
    {0xfd, 0xd7, 0xd5, 0xd5, 0x00, 0x00, 0x00, 0x00, 0x00}, // &trade;
    {0xe4, 0xe2, 0xd2, 0xda, 0xd4, 0xd2, 0xe2, 0x00, 0x00}, // &alefsym;
    {0x00, 0xc8, 0xd0, 0xff, 0xd0, 0xc8, 0x00, 0x00, 0x00}, // &larr;
    {0xc8, 0xdc, 0xea, 0xc8, 0xc8, 0xc8, 0xc8, 0x00, 0x00}, // &uarr;
    {0x00, 0xc4, 0xc2, 0xff, 0xc2, 0xc4, 0x00, 0x00, 0x00}, // &rarr;
    {0xc8, 0xc8, 0xc8, 0xc8, 0xea, 0xdc, 0xc8, 0x00, 0x00}, // &darr;
    {0x00, 0xd2, 0xe1, 0xff, 0xe1, 0xd2, 0x00, 0x00, 0x00}, // &harr;
    {0xc1, 0xc1, 0xc9, 0xd1, 0xff, 0xd0, 0xc8, 0x00, 0x00}, // &crarr;
    {0xc4, 0xc8, 0xdf, 0xe0, 0xdf, 0xc8, 0xc4, 0x00, 0x00}, // &lArr;
    {0xc8, 0xd4, 0xf6, 0xd4, 0xd4, 0xd4, 0xd4, 0x00, 0x00}, // &uArr;
    {0xc8, 0xc4, 0xfe, 0xc1, 0xfe, 0xc4, 0xc8, 0x00, 0x00}, // &rArr;
    {0xd4, 0xd4, 0xd4, 0xd4, 0xf6, 0xd4, 0xc8, 0x00, 0x00}, // &dArr;
    {0xd2, 0xe1, 0xff, 0xe1, 0xff, 0xe1, 0xd2, 0x00, 0x00}, // &hArr;
    {0xe2, 0xe2, 0xfe, 0xe2, 0xe2, 0xdc, 0xc8, 0x00, 0x00}, // &forall; &#8704;
    {0xd8, 0xc4, 0xc2, 0xde, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &part; &#8706;
    {0xfe, 0xc2, 0xc2, 0xde, 0xc2, 0xc2, 0xfe, 0x00, 0x00}, // &exist; &#8707
    {0xdc, 0xe2, 0xe6, 0xea, 0xf2, 0xe2, 0xdc, 0x00, 0x00}, // &empty; &#8709;
    {0xfe, 0xe2, 0xe2, 0xd4, 0xd4, 0xd4, 0xc8, 0x00, 0x00}, // &nabla; &#8711;
    {0xde, 0xe0, 0xe0, 0xfe, 0xe0, 0xe0, 0xde, 0x00, 0x00}, // &isin; &#8712
    {0xde, 0xe2, 0xe4, 0xfe, 0xf0, 0xe0, 0xde, 0x00, 0x00}, // &notin; &#8713
    {0xfc, 0xc2, 0xc2, 0xfe, 0xc2, 0xc2, 0xfc, 0x00, 0x00}, // &ni; &#8715
    {0xfe, 0xe2, 0xe2, 0xe2, 0xe2, 0xe2, 0xe2, 0x00, 0x00}, // &prod; &#8719
    {0xfe, 0xe0, 0xd0, 0xc8, 0xd0, 0xe0, 0xfe, 0x00, 0x00}, // &sum; &#8721
    {0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00}, // &minus; &#8722
    {0xc8, 0xea, 0xdc, 0xc8, 0xdc, 0xea, 0xc8, 0x00, 0x00}, // &lowast; &#8727
    {0xc2, 0xc2, 0xc2, 0xe4, 0xe4, 0xd8, 0xc8, 0x00, 0x00}, // &radic; &#8730
    {0x00, 0xd6, 0xe8, 0xe8, 0xd6, 0x00, 0x00, 0x00, 0x00}, // &prop; &#8733
    {0x00, 0xd4, 0xea, 0xea, 0xd4, 0x00, 0x00, 0x00, 0x00}, // &infin; &#8734
    {0x00, 0xc2, 0xc4, 0xc8, 0xd0, 0xe0, 0xfe, 0x00, 0x00}, // &ang; &#8736
    {0x00, 0x00, 0xc8, 0xd4, 0xe2, 0x00, 0x00, 0x00, 0x00}, // &and; &#8743
    {0x00, 0x00, 0xe2, 0xd4, 0xc8, 0x00, 0x00, 0x00, 0x00}, // &or; &#8744
    {0xdc, 0xe2, 0xe2, 0xe2, 0xe2, 0xe2, 0xe2, 0x00, 0x00}, // &cap; &#8745
    {0xe2, 0xe2, 0xe2, 0xe2, 0xe2, 0xe2, 0xdc, 0x00, 0x00}, // &cup; &#8746
    {0xc4, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xd0, 0x00, 0x00}, // &int; &#8747
    {0x00, 0x00, 0xc8, 0x00, 0xe2, 0x00, 0x00, 0x00, 0x00}, // &ther4; &#8756
    {0x00, 0x00, 0xd0, 0xea, 0xc4, 0x00, 0x00, 0x00, 0x00}, // &sim; &#8764
    {0xd0, 0xea, 0xc4, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0x00}, // &cong; &#8773
    {0xd0, 0xea, 0xc4, 0xd0, 0xea, 0xc4, 0x00, 0x00, 0x00}, // &asymp; &#8776
    {0x00, 0xc4, 0xfe, 0xc8, 0xfe, 0xd0, 0x00, 0x00, 0x00}, // &ne; &#8800
    {0x00, 0xfe, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0x00, 0x00}, // &equiv; &#8801
    {0xc6, 0xd8, 0xe0, 0xd8, 0xc6, 0x00, 0xfe, 0x00, 0x00}, // &le; &#8804
    {0xf0, 0xcc, 0xc2, 0xcc, 0xf0, 0x00, 0xfe, 0x00, 0x00}, // &ge; &#8805
    {0x00, 0xde, 0xe0, 0xe0, 0xe0, 0xde, 0x00, 0x00, 0x00}, // &sub; &#8834
    {0x00, 0xfc, 0xc2, 0xc2, 0xc2, 0xfc, 0x00, 0x00, 0x00}, // &sup; &#8835
    {0x00, 0xde, 0xe4, 0xe8, 0xf0, 0xfe, 0x00, 0x00, 0x00}, // &nsub; &8836
    {0x00, 0xde, 0xe0, 0xe0, 0xde, 0x00, 0xfe, 0x00, 0x00}, // &sube; &#8838
    {0x00, 0xfc, 0xc2, 0xc2, 0xfc, 0x00, 0xfe, 0x00, 0x00}, // &supe; &#8839
    {0xdc, 0xea, 0xea, 0xfe, 0xea, 0xea, 0xdc, 0x00, 0x00}, // &oplus; &#8853
    {0xdc, 0xe2, 0xf6, 0xea, 0xf6, 0xe2, 0xdc, 0x00, 0x00}, // &otimes; &#8855
    {0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xfe, 0x00, 0x00}, // &perp; &#8869
    {0x00, 0x00, 0x00, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00}, // &sdot; &8901
    {0x00, 0xc8, 0x00, 0xc8, 0x00, 0xc8, 0x00, 0x00, 0x00}, // &vellip; &#8942
    {0xfe, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0x00}, // &lceil; &#8968
    {0xfe, 0xc2, 0xc2, 0xc2, 0xc2, 0xc2, 0xc2, 0xc2, 0x00}, // &rceil; &#8969
    {0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xfe, 0x00}, // &lfloor; &#8970
    {0xc2, 0xc2, 0xc2, 0xc2, 0xc2, 0xc2, 0xc2, 0xfe, 0x00}, // &rfloor; &8971
    {0xc8, 0xc8, 0xc8, 0xd0, 0xc8, 0xc8, 0xc8, 0x00, 0x00}, // &lang; &#9001
    {0xc8, 0xc8, 0xc8, 0xc4, 0xc8, 0xc8, 0xc8, 0x00, 0x00}, // &rang; &#9002
    {0xc8, 0xd4, 0xd4, 0xe2, 0xd4, 0xd4, 0xc8, 0x00, 0x00}, // &loz; &#9674
    {0xc8, 0xdc, 0xdc, 0xfe, 0xea, 0xc8, 0xfe, 0x00, 0x00}, // &spades; &#9824
    {0x00, 0xdc, 0xdc, 0xfe, 0xfe, 0xc8, 0xfe, 0x00, 0x00}, // &clubs; &#9827
    {0x00, 0xd4, 0xf6, 0xfe, 0xdc, 0xdc, 0xc8, 0x00, 0x00}, // &hearts; &#9829
    {0xc8, 0xdc, 0xdc, 0xfe, 0xdc, 0xdc, 0xc8, 0x00, 0x00}, // &diams; &#9830
    //      {0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x00, 0x00}, // &;
};

// CSerialComm oSerialComm;

Usbcard::Usbcard()
{
    mode_ = USB_IDLE;
    ef_ = 1;
    catDir_ = NULL;
    bufferPos_ = 0;
    
    rootDirectory_ = p_Main->getSbRootDirectory();
    fwVersion_ = p_Main->getSbFwVersion();
    additionalDir_ = wxFileName(rootDirectory_, wxPATH_NATIVE);
    
    //char cPortName[10];
    //strcpy_s(cPortName,"\\\\.\\COM7");
    //strcpy_s(cPortName,"COM7");
    //oSerialComm.OpenSerialPort(cPortName);
    
    currentUrlPos_ = 0;
    lastLink_ = 0;
    continueLink_ = false;
    numberOfUrlScreens_ = 0;
    urlHistorySize_ = 0;
    urlServer_ = "";
    reshape_ = COLOR_ON_THIS_PAGE;
    reshapeInfo_ = 0x00;
}

Usbcard::~Usbcard()
{
//    oSerialComm.CloseSerialPort();
    if (catDir_ != NULL)
        delete catDir_;
}

Byte Usbcard::usbInVspe()
{ // Only used to test real SB communication
//    char buffer;
    
//    oSerialComm.Read(&buffer, 1);
//    return (Byte)buffer;
    return 0;
}

void Usbcard::usbOutVspe(Byte value)
{
    if (p_Computer->getFlipFlopQ())
    {
        return;
    }
    
    char buffer;
    
    buffer = (char) value;
//    oSerialComm.Write(&buffer, 1);
}

Byte Usbcard::usbIn6()
{
    ef_ = 0;
    return 0;
}

Byte Usbcard::usbIn()
{
    Byte returnValue;
    
    if (bufferFlush_ > 0)
    {
        bufferFlush_--;
        return 0;
    }
    
    switch (mode_)
    {
        case USB_LOAD_HEADER:
            returnValue = header[bufferPos_++];
            if (bufferPos_ == HEADER_LENGTH)
            {
                if ((header[HEADER_RESULT] == USB_FILE_EXISTS || header[HEADER_RESULT] == USB_FILE_TOO_SMALL_64 || header[HEADER_RESULT] == USB_FILE_TOO_SMALL_8 || header[HEADER_RESULT] == USB_FILE_TOO_LARGE_64 || header[HEADER_RESULT] == USB_FILE_TOO_LARGE_8) && header[HEADER_MODE] != USB_CANCEL)
                {
                    mode_ = USB_LOAD_CONTINUE;
                    bufferPos_ = HEADER_START;
                }
                else
                {
                    mode_ = USB_IDLE;
                }
                ef_ = 1;
            }
            return returnValue;
            break;
            
        case USB_URL_HEADER:
            returnValue = header[bufferPos_++];
            if (bufferPos_ == HEADER_LENGTH)
            {
                if (header[HEADER_RESULT] == USB_FILE_EXISTS && header[HEADER_MODE] != USB_CANCEL)
                {
                    mode_ = USB_URL_CONTINUE;
                    bufferPos_ = HEADER_START;
                }
                else
                {
                    mode_ = USB_IDLE;
                }
                ef_ = 1;
            }
            return returnValue;
            break;
            
        case USB_GET_TRACK_HEADER:
            returnValue = header[bufferPos_++];
            if (bufferPos_ == HEADER_LENGTH)
            {
                if (header[HEADER_RESULT] == USB_FILE_EXISTS && header[HEADER_MODE] != USB_CANCEL)
                {
                    mode_ = USB_GET_TRACK_CONTINUE;
                    bufferPos_ = HEADER_START;
                }
                else
                {
                    mode_ = USB_IDLE;
                }
                ef_ = 1;
            }
            return returnValue;
            break;
            
        case USB_IMG_TEST_HEADER:
            returnValue = header[bufferPos_++];
            if (bufferPos_ == HEADER_LENGTH)
            {
                mode_ = USB_IDLE;
                ef_ = 1;
            }
            return returnValue;
            break;
            
        case USB_SAVE_HEADER:
            returnValue = header[bufferPos_++];
            if (bufferPos_ == HEADER_LENGTH)
            {
                switch (header[HEADER_RESULT])
                {
                    case USB_ADDRESS_ERROR:
                        mode_ = USB_IDLE;
                        break;
                        
                    case USB_FILE_EXISTS:
                        if ((header[HEADER_MODE] & 0x80) == 0x80)
                            mode_ = USB_SAVE;
                        else
                        {
                            mode_ = USB_IDLE;
                        }
                        break;
                        
                    case USB_FILE_OPEN:
                        mode_ = USB_IDLE;
                        break;
                        
                    default:
                        mode_ = USB_SAVE;
                        break;
                }
                ef_ = 1;
                switch (header[HEADER_TYPE])
                {
                    case 1: /* Machine code SAVE */
                        bufferPos_ = 11;
                        break;
                        
                    case 2: /* Regular SAVE - old format */
                    case 3: /* F&M BASIC SAVE - old format */
                    case 6: /* Regular SAVE - current format */
                        bufferPos_ = 15;
                        break;
                        
                    case 5: /* DSAVE */
                        bufferPos_ = 7;
                        break;
                        
                    case 0x10: /* binary */
                        bufferPos_ = 0;
                        break;
                        
                    case 0x11: /* text */
                        bufferPos_ = 0;
                        break;
                        
                    case USB_SAVE_NVALL:
                    case USB_SAVE_NVDAT:
                    case USB_SAVE_NVOPT:
                    case USB_SAVE_NVSYNC:
                    case USB_SAVE_URL_INPUT:
                        if (mode_ == USB_SAVE)
                            mode_ = header[HEADER_TYPE]&0x7f;
                        bufferPos_ = 0;
                        break;
                        
                    default:
                        if ((header[HEADER_TYPE]&0x80) == 0x80)
                        {
                            switch (header[HEADER_BLOAD_TYPE])
                            {
                                case USB_SAVE_BANK:
                                    bufferPos_ = 0;
                                    break;
                                    
                                case USB_SAVE_BANK_ALL:
                                case USB_SAVE_BANK_ALL_CONT:
                                    bufferPos_ = bank_ * 0x2000;
                                    if (mode_ == USB_SAVE)
                                        mode_ = header[HEADER_BLOAD_TYPE]&0x7f;
                                    break;
                            }
                        }
                        else
                        {
                            mode_ = USB_IDLE;
                            ef_ = 1;
                        }
                        break;
                }
            }
            return returnValue;
            break;
            
        case USB_PUT_TRACK_HEADER:
            returnValue = header[bufferPos_++];
            if (bufferPos_ == HEADER_LENGTH)
            {
                switch (header[HEADER_RESULT])
                {
                    case USB_FILE_EXISTS:
                        if ((header[HEADER_MODE] & 0x80) == 0x80)
                            mode_ = USB_PUT_TRACK;
                        else
                        {
                            mode_ = USB_IDLE;
                        }
                        break;
                        
                    case USB_FILE_OPEN:
                        mode_ = USB_IDLE;
                        break;
                        
                    default:
                        mode_ = USB_PUT_TRACK;
                        break;
                }
                bufferPos_ = 0;
                ef_ = 1;
            }
            return returnValue;
            break;
            
        case USB_CD_HEADER:
            returnValue = header[bufferPos_++];
            if (bufferPos_ == HEADER_LONG_LENGTH)
            {
                mode_ = USB_IDLE;
                ef_ = 1;
            }
            return returnValue;
            break;
            
        case USB_CAT_HEADER:
            returnValue = header[bufferPos_++];
            if (bufferPos_ == HEADER_LONG_LENGTH)
            {
                mode_ = USB_IDLE;
                ef_ = 1;
            }
            return returnValue;
            break;
            
        case USB_MKDIR_HEADER:
            if (dummyHeader_ == 0)
                returnValue = header[bufferPos_++];
            else
                returnValue = dummyHeaderValues[dummyHeader_-1][bufferPos_++];
            if (bufferPos_ == HEADER_LENGTH)
            {
                mode_ = USB_IDLE;
                ef_ = 1;
            }
            return returnValue;
            break;
            
        case USB_LOAD:
            returnValue = buffer_[bufferPos_++];
            length_--;
            if (length_ == 0)
            {
                mode_ = USB_IDLE;
                ef_ = 1;
            }
            return returnValue;
            break;
            
        case USB_URL:
            returnValue = urlScreens[currentUrlScreen_].screenData[currentUrlPos_++];
            
            /*            if (currentUrlPos_ >= 960)
             {
             currentUrlPos_ = 0;
             currentUrlScreen_++;
             }*/
            
            length_--;
            if (length_ == 0)
            {
                mode_ = USB_IDLE;
                ef_ = 1;
            }
            return returnValue;
            break;
            
        case USB_GET_TRACK:
            returnValue = buffer_[bufferPos_++];
            length_--;
            if (length_ == 0)
            {
                mode_ = USB_GET_TRACK_CONTINUE;
                bufferPos_ = HEADER_START;
                
                if (buffer_[IMG_TRACK] == (tracks_ - 1) && buffer_[IMG_SIDE_NUMBER] == sides_)
                {
                    mode_ = USB_IDLE;
                }
                ef_ = 1;
            }
            return returnValue;
            break;
            
        default:
            return 0;
            break;
    }
}

void Usbcard::usbOut(Byte value)
{
    if (p_Computer->getFlipFlopQ())
    {
        return;
    }
    usbOutNoEfCheck(value);
}

void Usbcard::usbOutNoEfCheck(Byte value)
{
    wxFile outputFile;
    wxTextFile outputTextFile;
    wxString directory, rootDirectory, newDirectory, name, name2;
    wxFileName rootDir, fileTest;
    size_t pos;
    wxDateTime systemNow;
    wxString dateFormat, timeFormat;
    off_t track;
    wxFFile inputFile, inputFile2;
    int posName2;
    char byteFile1, byteFile2;
    int comparePos;
    size_t length1, length2;
    wxDir rmDir;
    int length;
    int urlLink, newScreen;
    wxLocale systemFormat;
    
    switch (mode_)
    {
        case USB_IDLE:
        case USB_LOAD:
        case USB_URL:
        case USB_URL_LINK:
        case USB_URL_HEADER:
        case USB_EMAIL:
        case USB_EMAIL_SEND:
        case USB_LOAD_DATA:
        case USB_GET_TRACK:
        case USB_GET_TRACK_HEADER:
        case USB_IMG_TEST:
        case USB_IMG_TEST_HEADER:
        case USB_PUT_TRACK_HEADER:
        case USB_SAVE_HEADER:
        case USB_LOAD_HEADER:
            //            buffer_[bufferPos_++] = value;
            //            if (bufferPos_ == 0x1680)
            //            {
            //                bufferPos_ = 0;
            //            }
            mode_ = USB_HEADER;
            bufferPos_ = HEADER_START;
            header[bufferPos_++] = value;
            break;
            
        case USB_HEADER:
            header[bufferPos_++] = value;
            if ((bufferPos_ == HEADER_LENGTH && header[HEADER_MODE] != USB_URL_LINK && header[HEADER_MODE] != USB_EMAIL && header[HEADER_MODE] != USB_EMAIL_SEND) || bufferPos_ == HEADER_LONG_LENGTH)
            {
                if (bufferPos_ == HEADER_LENGTH)
                {
                    if (header[HEADER_BUILD_NUMBER] != 0)
                    {
                        int newVersion = ((header[HEADER_BUILD_NUMBER] - 0x30) * 100) + ((header[HEADER_BUILD_NUMBER+1] - 0x30) * 10) + (header[HEADER_BUILD_NUMBER+2] - 0x30);
                        
                        if (newVersion != fwVersion_)
                        {
                            fwVersion_ = newVersion;
                            p_Main->setSbFwVersion(fwVersion_);
                        }
                    }
                }
                
                mode_ = header[HEADER_MODE]&0x3f;
                fileName_ = "";
                bufferPos_ = HEADER_FILENAME;
                
                if (header[HEADER_MODE] == USB_URL_LINK || header[HEADER_MODE] == USB_EMAIL || header[HEADER_MODE] == USB_EMAIL_SEND)
                    length = 127;
                else
                    length = 18;
                
                while (header[bufferPos_] != 0xd && header[bufferPos_] != 0 && bufferPos_ <= length)
                {
                    if (header[bufferPos_] >= 32 && header[bufferPos_] < 128)
                        fileName_ += wxString::FromAscii(header[bufferPos_]);
                    bufferPos_++;
                }
                
                if (length == 127)
                    fileName_ = convertCase(fileName_, 3, p_Main->getSbCaseFile());
                else
                    fileName_ = convertCase(fileName_, header[HEADER_CHAR], p_Main->getSbCaseFile());
                
                fileNameOnly_ = fileName_;
                name = fileName_;
                
                if (mode_ == USB_GET_TRACK || mode_ == USB_PUT_TRACK || mode_ == USB_IMG_TEST)
                    extensionOnly_ = ".img";
                else
                {
                    if (header[HEADER_TYPE] == 0x10)
                        extensionOnly_ = ".bin";
                    else
                    {
                        if (header[HEADER_TYPE] == 0x11)
                            extensionOnly_ = ".txt";
                        else
                            if ((header[HEADER_TYPE]&0x80) == 0x80 && (header[HEADER_BLOAD_TYPE]&0xf0) == 0x30)
                                extensionOnly_ = ".ram";
                            else
                                extensionOnly_ = ".comx";
                    }
                }
                
                fileName_ = additionalDir_.GetLongPath() + fileName_ + extensionOnly_;
                
                switch (mode_)
                {
                    case 0x3f:
                        mode_ = USB_IDLE;
                        ef_ = 1;
                        break;
                        
                    case USB_SAVE:
                        save();
                        break;
                        
                    case USB_PUT_TRACK:
                        putTrack();
                        break;
                        
                    case USB_URL_LINK:
                        fileNameOnly_ = fileNameOnly_.Trim(true);
                        fileNameOnly_ = fileNameOnly_.Trim(false);
                        if (header[HEADER_URL_COMMAND] < 32)
                        {
                            switch (header[HEADER_URL_COMMAND])
                            {
                                case URL_COMMAND_BACK:
                                    if (urlHistorySize_ > 1)
                                        urlHistorySize_--;
                                    fileNameOnly_ = urlHistory[urlHistorySize_-1];
                                    break;
                                    
                                case URL_COMMAND_HOME:
                                    fileNameOnly_ = p_Main->getSbUrlHome();
                                    break;
                                    
                                case URL_COMMAND_BOOKMARK:
                                    fileNameOnly_ = p_Main->getSbUrlBookMark(header[HEADER_URL_BOOKMARK]);
                                    break;
                                    
                                case URL_COMMAND_SET_HOME:
                                    p_Main->setSbUrlHome(urlHistory[urlHistorySize_-1]);
                                    break;
                                    
                                case URL_COMMAND_SET_BOOKMARK:
                                    p_Main->setSbUrlBookMark(header[HEADER_URL_BOOKMARK], urlHistory[urlHistorySize_-1]);
                                    break;
                                    
                                case URL_COMMAND_HELP:
                                    getHelp();
                                    break;
                                    
                                default:
                                    urlLink = header[HEADER_URL_LINK];
                                    if (urlLink == 0)
                                        fileNameOnly_ = p_Main->getSbUrlHome();
                                    else
                                    {
                                        if (currentUrlScreen_ >= numberOfUrlScreens_)
                                            fileNameOnly_ = "";
                                        else
                                        {
                                            if (urlLink > urlScreens[currentUrlScreen_].numberOfLinks)
                                                fileNameOnly_ = "";
                                            else
                                            {
                                                fileNameOnly_ = urlScreens[currentUrlScreen_].urlLinks[urlLink-1];
                                                if (urlScreens[currentUrlScreen_].inputInfo[urlLink-1].inputType == INPUT_TYPE_SUBMIT)
                                                {
                                                    fileNameOnly_ = fileNameOnly_ + getForm();
                                                }
                                            }
                                        }
                                    }
                                    break;
                            }
                        }
                        if (header[HEADER_URL_COMMAND] != URL_COMMAND_HELP)
                        {
                            if (fileNameOnly_.Len()>7)
                            {
                                if (fileNameOnly_.Left(7) == "http://")
                                {
                                    fileNameOnly_ = fileNameOnly_.Right(fileNameOnly_.Len()-7);
                                }
                            }
                            if (fileNameOnly_.Len()>2)
                            {
                                if (fileNameOnly_.Left(2) == "//")
                                {
                                    fileNameOnly_ = fileNameOnly_.Right(fileNameOnly_.Len()-2);
                                }
                            }
                            if (!(header[HEADER_URL_COMMAND] == URL_COMMAND_SET_HOME || header[HEADER_URL_COMMAND] == URL_COMMAND_SET_BOOKMARK))
                            {
                                previousServerName_ = urlServer_;
                                
                                wxURI uri;
                                uri.Create("http://"+fileNameOnly_);
                                urlServer_ = uri.GetServer();
                                urlFullPath_ = uri.GetPath();
                                urlFragment_ = uri.GetFragment();
                                urlFragment_.Replace("%20", " ");
                                
                                if (urlFullPath_ == "/")
                                    urlFullPath_ = "";
                                
                                if (urlFullPath_.Find('#') != wxNOT_FOUND)
                                    urlFullPath_ = urlFullPath_.BeforeFirst('#');
                                
                                if (urlFullPath_.Find('.') != wxNOT_FOUND)
                                    urlPath_ = urlFullPath_.BeforeLast('/');
                                else
                                    urlPath_ = urlFullPath_;
                                
                                urlRedirectCounter_ = 0;
                                getPage();
                                if (header[HEADER_URL_COMMAND] != URL_COMMAND_BACK)
                                {
                                    urlHistorySize_++;
                                    urlHistory.resize(urlHistorySize_);
                                    urlHistory[urlHistorySize_-1] = fileNameOnly_;
                                }
                                if (networkError_ == USB_URL_NOTFOUND)
                                    urlServer_ = previousServerName_;
                            }
                        }
                        mode_ = USB_IDLE;
                        ef_ = 1;
                        bufferFlush_ = 2;
                        break;
                        
                    case USB_EMAIL:
                        email("");
                        mode_ = USB_IDLE;
                        ef_ = 1;
                        bufferFlush_ = 2;
                        break;
                        
                    case USB_EMAIL_SEND:
                    {
                        wxString emailForm = "", error = "";
                        if (header[HEADER_URL_COMMAND] < 32)
                        {
                            switch (header[HEADER_URL_COMMAND])
                            {
                                case URL_COMMAND_HELP:
                                    getEmailHelp();
                                    break;
                                    
                                default:
                                    urlLink = header[HEADER_URL_LINK];
                                    if (currentUrlScreen_ >= numberOfUrlScreens_)
                                        fileNameOnly_ = "";
                                    else
                                    {
                                        if (urlLink > urlScreens[currentUrlScreen_].numberOfLinks)
                                            fileNameOnly_ = "";
                                        else
                                        {
                                            fileNameOnly_ = urlScreens[currentUrlScreen_].urlLinks[urlLink-1];
                                            if (urlScreens[currentUrlScreen_].inputInfo[urlLink-1].inputType == INPUT_TYPE_SUBMIT)
                                                emailForm = getEmailForm(&error);
                                        }
                                    }
                                    break;
                            }
                        }
                        if (header[HEADER_URL_COMMAND] != URL_COMMAND_HELP)
                        {
                            if (fileNameOnly_ == "email")
                            {
                                email("");
                            }
                            else
                            {
                                if (error != "")
                                    email_error(error);
                                else
                                {
                                    previousServerName_ = urlServer_;
                                    fileNameOnly_ = fileNameOnly_ + emailForm;
                                    
                                    wxURI uri;
                                    uri.Create("http://"+fileNameOnly_);
                                    urlServer_ = uri.GetServer();
                                    urlFullPath_ = uri.GetPath();
                                    
                                    if (urlFullPath_ == "/")
                                        urlFullPath_ = "";
                                    
                                    if (urlFullPath_.Find('#') != wxNOT_FOUND)
                                        urlFullPath_ = urlFullPath_.BeforeFirst('#');
                                    
                                    if (urlFullPath_.Find('.') != wxNOT_FOUND)
                                        urlPath_ = urlFullPath_.BeforeLast('/');
                                    else
                                        urlPath_ = urlFullPath_;
                                    
                                    urlRedirectCounter_ = 0;
                                    sendEmail();
                                }
                            }
                            mode_ = USB_IDLE;
                        }
                        ef_ = 1;
                        bufferFlush_ = 2;
                    }
                        break;
                        
                    case USB_URL:
                        newScreen = (header[HEADER_URL_SCREEN_H] << 8) + header[HEADER_URL_SCREEN_L];
                        
                        if (networkError_ == USB_URL_NOTFOUND)
                            header[HEADER_RESULT] = USB_URL_NOTFOUND;
                        else
                        {
                            if (newScreen >= numberOfUrlScreens_)
                            {
                                header[HEADER_RESULT] = USB_NO_DATA;
                                newScreen = currentUrlScreen_;
                            }
                            else
                                header[HEADER_RESULT] = USB_FILE_EXISTS;
                        }
                        
                        currentUrlScreen_ = newScreen;
                        
                        //                        header[HEADER_COLOUR_INFO] = 0xe0;
                        //                        header[HEADER_RESHAPE] = reshape_;
                        //                        header[HEADER_RESHAPE_INFO] = reshapeInfo_;
                        
                        header[HEADER_MODE] = USB_LOAD;
                        header[HEADER_TYPE] = USB_LOAD_URL;
                        switch (currentUrlScreen_)
                    {
                        case 0:
                            header[HEADER_LENGTH_H] = 0;
                            header[HEADER_LENGTH_L] = 0x6c;
                            length_ = 0x6c;
                            break;
                        default:
                            if (currentUrlScreen_ >= numberOfUrlScreens_)
                            {
                                header[HEADER_LENGTH_H] = 0x00;
                                header[HEADER_LENGTH_L] = 0x1;
                                length_ = 0x1;
                            }
                            else
                            {
                                urlScreens[currentUrlScreen_].screenData[urlScreens[currentUrlScreen_].shapeLocation_ + 0x900] = 0;
                                header[HEADER_LENGTH_H] = 0x09 + ((urlScreens[currentUrlScreen_].shapeLocation_+1) >> 8);
                                header[HEADER_LENGTH_L] = (urlScreens[currentUrlScreen_].shapeLocation_+1) & 0xff;
                                length_ = urlScreens[currentUrlScreen_].shapeLocation_ + 0x901;
                            }
                            break;
                    }
                        header[HEADER_START_L] = 0;
                        mode_ = USB_URL_HEADER;
                        bufferPos_ = HEADER_START;
                        currentUrlPos_ = 0;
                        lastLink_ = 0;
                        continueLink_ = false;
                        bufferFlush_ = 2;
                        break;
                        
                    case USB_LOAD:
                    case USB_LOAD_DATA:
                        //                        if (!wxFile::Exists(fileName_))
                        //                        {
                        //                            fileName_ = name + ".bin";
                        //
                        //                            fileName_ = fileName_.MakeLower();
                        //                            fileName_ = additionalDir_.GetLongPath() + fileName_;
                        //                        }
                        if ((header[HEADER_TYPE]&0xf0) == 0x20)
                        {
                            switch (header[HEADER_TYPE]&0x7f)
                            {
                                case USB_SAVE_NVDAT:
                                    fileName_ = rootDirectory_+"nvram.bin";
                                    break;
                                    
                                case USB_SAVE_NVSYNC:
                                    fileName_ = rootDirectory_+"highscores.bin";
                                    break;
                                    
                                case USB_SAVE_NVALL:
                                case USB_SAVE_NVOPT:
                                    fileName_ = rootDirectory_+"system.bin";
                                    break;
                            }
                        }
                        load();
                        break;
                        
                    case USB_GET_TRACK:
                        getTrack();
                        break;
                        
                    case USB_IMG_TEST:
                        testImg();
                        break;
                        
                    case USB_CLOCK:
                        bufferPos_ = HEADER_FILENAME;
                        systemNow = wxDateTime::Now();
                        
                        storeClockValue(systemNow.GetSecond());
                        storeClockValue(systemNow.GetMinute());
                        storeClockValue(systemNow.GetHour());
                        storeClockValue(systemNow.GetWeekDay());
                        storeClockValue(systemNow.GetDay());
                        storeClockValue(systemNow.GetMonth()+1);
                        storeClockValue(systemNow.GetYear() % 100);
                                                
                        dateFormat = systemFormat.GetInfo(wxLOCALE_SHORT_DATE_FMT);
                        timeFormat = systemFormat.GetInfo(wxLOCALE_TIME_FMT);
                        
                        // %d/%m/%Y - %Y-%m-%d
                        dateFormat = dateFormat.MakeLower();
                        header[bufferPos_++] = dateFormat.GetChar(2);
                        storeDateFormat(dateFormat.Left(2));
                        storeDateFormat(dateFormat.Mid(3,2));
                        storeDateFormat(dateFormat.Mid(6,2));
                        
                        // %H:%M:%S
                        storeTimeFormat(timeFormat.Left(2));
                        
                        mode_ = USB_MKDIR_HEADER;
                        bufferPos_ = HEADER_START;
                        bufferFlush_ = 2;
                    break;
                        
                    case USB_CD:
                        directory = "";
                        bufferPos_ = HEADER_FILENAME;
                        while (header[bufferPos_] != 0 && bufferPos_ <= 18)
                        {
                            if (header[bufferPos_] >= 32 && header[bufferPos_] < 128)
                                directory += wxString::FromAscii(header[bufferPos_++]);
                        }
                        
                        if (bufferPos_ != HEADER_FILENAME)
                        {
                            if (directory != "..")
                            {
                                additionalDir_.AppendDir(directory);
                                if (!additionalDir_.DirExists())
                                {
                                    additionalDir_.RemoveLastDir();
                                    header[HEADER_START] = 0xff;
                                }
                            }
                            else
                            {
                                rootDir = wxFileName(rootDirectory_, wxPATH_NATIVE);
                                rootDirectory = rootDir.GetLongPath();
                                newDirectory = additionalDir_.GetLongPath();
                                
                                if (rootDirectory != newDirectory)
                                    additionalDir_.RemoveLastDir();
                            }
                        }
                        
                        directory = additionalDir_.GetLongPath();
                        
                        rootDir = wxFileName(rootDirectory_, wxPATH_NATIVE);
                        rootDir.RemoveLastDir();
                        rootDirectory = rootDir.GetLongPath();
                        
                        if (directory.Len() >= rootDirectory.Len())
                        {
                            pos = 0;
                            while (pos < rootDirectory.Len() && pos < directory.Len() && directory.GetChar(pos) == rootDirectory.GetChar(pos))
                                pos++;
                            
                            if (pos < directory.Len())
                                directory = directory.Right(directory.Len()-pos);
                        }
                        
                        if (p_Main->getSbCdRoot() == 1)
                        {
                            while (directory.Left(1) != rootDir.GetPathSeparator())
                                directory = directory.Right(directory.Len()-1);
                        }
                        
                        if (directory.Len() > 126)
                            directory = directory.Right(126);
                        
                        directory = convertCase(directory, header[HEADER_CHAR], MAKEUPPER);
                        for (size_t i=0; i<directory.Len(); i++)
                            header[i+1] = directory.GetChar(i);
                        header[directory.Len()+1] = 0;
                        
                        mode_ = USB_CD_HEADER;
                        bufferPos_ = HEADER_START;
                        bufferFlush_ = 2;
                        break;
                        
                    case USB_MKDIR:
                        directory = "";
                        bufferPos_ = HEADER_FILENAME;
                        while (header[bufferPos_] != 0 && bufferPos_ <= 18)
                        {
                            if (header[bufferPos_] >= 32 && header[bufferPos_] < 128)
                                directory += wxString::FromAscii(header[bufferPos_++]);
                        }
                        
                        if (bufferPos_ != HEADER_FILENAME)
                        {
                            directory = convertCase(directory, header[HEADER_CHAR], p_Main->getSbCaseDir());
                            
                            if (wxFile::Exists(additionalDir_.GetLongPath() + directory) || wxFileName::DirExists(additionalDir_.GetLongPath() + directory))
                            {
                                header[HEADER_START] = 0xff;
                            }
                            else
                            {
                                if (!wxFileName::Mkdir(additionalDir_.GetLongPath() + directory))
                                    header[HEADER_START] = 0xff;
                            }
                        }
                        
                        mode_ = USB_MKDIR_HEADER;
                        bufferPos_ = HEADER_START;
                        bufferFlush_ = 2;
                        break;
                        
                    case USB_RMDIR:
                        directory = "";
                        bufferPos_ = HEADER_FILENAME;
                        while (header[bufferPos_] != 0 && bufferPos_ <= 18)
                        {
                            if (header[bufferPos_] >= 32 && header[bufferPos_] < 128)
                                directory += wxString::FromAscii(header[bufferPos_++]);
                        }
                        
                        if (bufferPos_ != HEADER_FILENAME)
                        {
                            directory = convertCase(directory, header[HEADER_CHAR], p_Main->getSbCaseDir());
                            
                            if (!wxFileName::DirExists(additionalDir_.GetLongPath() + directory))
                            {
                                header[HEADER_START] = 0xff;
                            }
                            else
                            {
                                directory = additionalDir_.GetLongPath() + directory;
                                
                                rmDir.Open(directory);
                                if (rmDir.HasFiles())
                                    header[HEADER_START] = 0xff;
                                else
                                    if (rmDir.HasSubDirs())
                                        header[HEADER_START] = 0xff;
                                    else
                                    {
                                        if (!wxFileName::Rmdir(directory))
                                            header[HEADER_START] = 0xff;
                                    }
                            }
                        }
                        
                        mode_ = USB_MKDIR_HEADER;
                        bufferPos_ = HEADER_START;
                        bufferFlush_ = 2;
                        break;
                        
                    case USB_DEL:
                        name = "";
                        bufferPos_ = HEADER_FILENAME;
                        while (header[bufferPos_] != 0 && bufferPos_ <= 18)
                        {
                            if (header[bufferPos_] >= 32 && header[bufferPos_] < 128)
                                name += wxString::FromAscii(header[bufferPos_++]);
                        }
                        
                        if (bufferPos_ != HEADER_FILENAME)
                        {
                            name = convertCase(name, header[HEADER_CHAR], p_Main->getSbCaseFile());
                            
                            switch (header[HEADER_TYPE])
                            {
                                case TYPE_RAM:
                                    name += ".ram";
                                    header[HEADER_START] = 0xff;
                                    if (wxFile::Exists(additionalDir_.GetLongPath() + name))
                                    {
                                        if (wxFileName::IsFileWritable(additionalDir_.GetLongPath() + name))
                                            if (wxRemoveFile(additionalDir_.GetLongPath() + name))
                                                header[HEADER_START] = 8;
                                    }
                                    break;
                                    
                                case TYPE_TXT:
                                    name += ".txt";
                                    header[HEADER_START] = 0xff;
                                    if (wxFile::Exists(additionalDir_.GetLongPath() + name))
                                    {
                                        if (wxFileName::IsFileWritable(additionalDir_.GetLongPath() + name))
                                            if (wxRemoveFile(additionalDir_.GetLongPath() + name))
                                                header[HEADER_START] = 8;
                                    }
                                    break;
                                    
                                case TYPE_IMG:
                                    name += ".img";
                                    header[HEADER_START] = 0xff;
                                    if (wxFile::Exists(additionalDir_.GetLongPath() + name))
                                    {
                                        if (wxFileName::IsFileWritable(additionalDir_.GetLongPath() + name))
                                            if (wxRemoveFile(additionalDir_.GetLongPath() + name))
                                                header[HEADER_START] = 8;
                                    }
                                    break;
                                    
                                case TYPE_BIN:
                                    name += ".bin";
                                    header[HEADER_START] = 0xff;
                                    if (wxFile::Exists(additionalDir_.GetLongPath() + name))
                                    {
                                        if (wxFileName::IsFileWritable(additionalDir_.GetLongPath() + name))
                                            if (wxRemoveFile(additionalDir_.GetLongPath() + name))
                                                header[HEADER_START] = 8;
                                    }
                                    break;
                                    
                                case TYPE_COMX_BAK:
                                case TYPE_BIN_BAK:
                                case TYPE_RAM_BAK:
                                case TYPE_TXT_BAK:
                                case TYPE_IMG_BAK:
                                case TYPE_ALL_BAK:
                                    header[HEADER_START] = 0xff;
                                    switch (header[HEADER_TYPE] & 0x1f)
                                {
                                    case 0:
                                        deleteBackup(additionalDir_, name, ".comx", wxDIR_FILES);
                                        break;
                                    case TYPE_BIN:
                                        deleteBackup(additionalDir_, name, ".bin", wxDIR_FILES);
                                        break;
                                    case TYPE_RAM:
                                        deleteBackup(additionalDir_, name, ".ram", wxDIR_FILES);
                                        break;
                                    case TYPE_TXT:
                                        deleteBackup(additionalDir_, name, ".txt", wxDIR_FILES);
                                        break;
                                    case TYPE_IMG:
                                        deleteBackup(additionalDir_, name, ".img", wxDIR_FILES);
                                        break;
                                    case TYPE_ALL:
                                        deleteBackup(additionalDir_, name, ".comx", wxDIR_FILES);
                                        deleteBackup(additionalDir_, name, ".bin", wxDIR_FILES);
                                        deleteBackup(additionalDir_, name, ".ram", wxDIR_FILES);
                                        deleteBackup(additionalDir_, name, ".txt", wxDIR_FILES);
                                        deleteBackup(additionalDir_, name, ".img", wxDIR_FILES);
                                        break;
                                }
                                    break;
                                    
                                case TYPE_ALL:
                                    header[HEADER_START] = 1;
                                    if (wxFile::Exists(additionalDir_.GetLongPath() + name + ".ram"))
                                    {
                                        if (wxFileName::IsFileWritable(additionalDir_.GetLongPath() + name + ".ram"))
                                        {
                                            if (!wxRemoveFile(additionalDir_.GetLongPath() + name + ".ram"))
                                                header[HEADER_START] = 0xff;
                                        }
                                        else
                                            header[HEADER_START] = 0xff;
                                    }
                                    if (wxFile::Exists(additionalDir_.GetLongPath() + name + ".txt"))
                                    {
                                        if (wxFileName::IsFileWritable(additionalDir_.GetLongPath() + name + ".txt"))
                                        {
                                            if (!wxRemoveFile(additionalDir_.GetLongPath() + name + ".txt"))
                                                header[HEADER_START] = 0xff;
                                        }
                                        else
                                            header[HEADER_START] = 0xff;
                                    }
                                    if (wxFile::Exists(additionalDir_.GetLongPath() + name + ".bin"))
                                    {
                                        if (wxFileName::IsFileWritable(additionalDir_.GetLongPath() + name + ".bin"))
                                        {
                                            if (!wxRemoveFile(additionalDir_.GetLongPath() + name + ".bin"))
                                                header[HEADER_START] = 0xff;
                                        }
                                        else
                                            header[HEADER_START] = 0xff;
                                    }
                                    if (wxFile::Exists(additionalDir_.GetLongPath() + name + ".img"))
                                    {
                                        if (wxFileName::IsFileWritable(additionalDir_.GetLongPath() + name + ".img"))
                                        {
                                            if (!wxRemoveFile(additionalDir_.GetLongPath() + name + ".img"))
                                                header[HEADER_START] = 0xff;
                                        }
                                        else
                                            header[HEADER_START] = 0xff;
                                    }
                                    if (wxFile::Exists(additionalDir_.GetLongPath() + name + ".comx"))
                                    {
                                        if (wxFileName::IsFileWritable(additionalDir_.GetLongPath() + name + ".comx"))
                                        {
                                            if (!wxRemoveFile(additionalDir_.GetLongPath() + name + ".comx"))
                                                header[HEADER_START] = 0xff;
                                        }
                                        else
                                            header[HEADER_START] = 0xff;
                                    }
                                    if (header[HEADER_START] == 1)
                                        header[HEADER_START] = 8;
                                    break;
                                    
                                default:
                                    name += ".comx";
                                    header[HEADER_START] = 0xff;
                                    if (wxFile::Exists(additionalDir_.GetLongPath() + name))
                                    {
                                        if (wxFileName::IsFileWritable(additionalDir_.GetLongPath() + name))
                                            if (wxRemoveFile(additionalDir_.GetLongPath() + name))
                                                header[HEADER_START] = 8;
                                    }
                                    break;
                            }
                        }
                        
                        mode_ = USB_MKDIR_HEADER;
                        bufferPos_ = HEADER_START;
                        bufferFlush_ = 2;
                        break;
                        
                    case USB_COMP_FILE:
                        name = "";
                        name2 = "";
                        bufferPos_ = HEADER_FILENAME;
                        while (header[bufferPos_] != 0 && bufferPos_ <= 18)
                        {
                            if (header[bufferPos_] >= 32 && header[bufferPos_] < 128)
                                name += wxString::FromAscii(header[bufferPos_++]);
                        }
                        posName2 = HEADER_FILENAME2;
                        while (header[posName2] != 0 && posName2 <= HEADER_FILENAME2+18)
                        {
                            if (header[posName2] >= 32 && header[posName2] < 128)
                                name2 += wxString::FromAscii(header[posName2++]);
                        }
                        
                        if (bufferPos_ != HEADER_FILENAME && posName2 != HEADER_FILENAME2)
                        {
                            name = convertCase(name, header[HEADER_CHAR], p_Main->getSbCaseFile());
                            name2 = convertCase(name2, header[HEADER_CHAR], p_Main->getSbCaseFile());
                        }
                        
                        header[HEADER_START] = 0xff;
                        
                        switch (header[HEADER_TYPE])
                    {
                        case TYPE_RAM:
                            name += ".ram";
                            name2 += ".ram";
                            break;
                            
                        case TYPE_TXT:
                            name += ".txt";
                            name2 += ".txt";
                            break;
                            
                        case TYPE_IMG:
                            name += ".img";
                            name2 += ".img";
                            break;
                            
                        case TYPE_BIN:
                            name += ".bin";
                            name2 += ".bin";
                            break;
                            
                        default:
                            name += ".comx";
                            name2 += ".comx";
                            break;
                    }
                        
                        if (wxFile::Exists(additionalDir_.GetLongPath() + name) && wxFile::Exists(additionalDir_.GetLongPath() + name2))
                        {
                            if (inputFile.Open(additionalDir_.GetLongPath() + name, _("rb")) && inputFile2.Open(additionalDir_.GetLongPath() + name2, _("rb")))
                            {
                                header[HEADER_START] = 0x18;
                                
                                byteFile1 = 0;
                                byteFile2 = 0;
                                length1 = 1;
                                length2 = 1;
                                comparePos = 0;
                                
                                while (byteFile1 == byteFile2 && length1 != 0 && length2 != 0)
                                {
                                    length1 = inputFile.Read(&byteFile1, 1);
                                    length2 = inputFile2.Read(&byteFile2, 1);
                                    comparePos++;
                                }
                                
                                if (length1 == 0 && length2 == 0)
                                    comparePos = 0;
                                
                                header[HEADER_START_H] = (comparePos>>8) & 0xff;
                                header[HEADER_START_L] = comparePos & 0xff;
                            }
                            if (inputFile.IsOpened())
                                inputFile.Close();
                            if (inputFile2.IsOpened())
                                inputFile2.Close();
                        }
                        
                        mode_ = USB_MKDIR_HEADER;
                        bufferPos_ = HEADER_START;
                        bufferFlush_ = 2;
                        break;
                        
                    case USB_CAT:
                        charType_ = header[HEADER_CHAR];
                        bufferPos_ = HEADER_START;
                        for (int i=0; i<128; i++)
                            header[i] = 0;
                        
                        if (catDir_ != NULL)
                            delete catDir_;
                        
                        catDir_ = new wxDir (additionalDir_.GetLongPath());
                        contCat_ = catDir_->GetFirst(&catFileName_, wxEmptyString, wxDIR_DIRS);
                        if (!contCat_)
                        {
                            dirSearch_ = false;
                            contCat_ = catDir_->GetFirst(&catFileName_, wxEmptyString, wxDIR_FILES | wxDIR_HIDDEN);
                        }
                        else
                            dirSearch_ = true;
                        
                        directory = catDir_->GetName();
                        
                        directory = directory.Right(directory.Len()-directory.Find(additionalDir_.GetPathSeparator(), true)-1);
                        if (directory.Len() > 18)
                            directory = directory.Left(18);
                        directory = convertCase(directory, charType_, MAKEUPPER);
                        for (size_t i=0; i<directory.Len(); i++)
                            header[i+1] = directory.GetChar(i);
                        header[directory.Len()+1] = 0;
                        header[0] = 0xff;
                        
                        number_ = 1;
                        
                        bufferPos_ = HEADER_START + 0x20;
                        dir();
                        
                        mode_ = USB_CAT_HEADER;
                        bufferPos_ = HEADER_START;
                        bufferFlush_ = 2;
                        break;
                        
                    case USB_CAT_NEXT:
                        bufferPos_ = HEADER_START;
                        for (int i=0; i<128; i++)
                            header[i] = 0;
                        
                        if (number_ != 0)
                        {
                            dir();
                            mode_ = USB_CAT_HEADER;
                            bufferPos_ = HEADER_START;
                            bufferFlush_ = 2;
                        }
                        else
                        {
                            if (catDir_ != NULL)
                                delete catDir_;
                            catDir_ = NULL;
                            mode_ = USB_IDLE;
                            ef_ = 1;
                        }
                        
                        break;
                }
            }
            break;
            
        case USB_LOAD_CONTINUE:
            header[bufferPos_++] = value;
            if (bufferPos_ == HEADER_LENGTH)
            {
                if (header[HEADER_MODE] == USB_CANCEL)
                {
                    mode_= USB_IDLE;
                    ef_ = 1;
                }
                else
                {
                    mode_ = USB_LOAD;
                    switch (header[HEADER_TYPE])
                    {
                        case 1: // Machine code LOAD
                            bufferPos_ = 11;
                            break;
                            
                        case 2: // Regular LOAD - old format
                        case 3: // F&M BASIC LOAD - old format
                        case 6: // Regular LOAD - current format
                            bufferPos_ = 15;
                            break;
                            
                        case 5: // DLOAD
                            bufferPos_ = 7;
                            break;
                            
                        case 0x10: // Binary load
                        case USB_SAVE_NVALL:
                        case USB_SAVE_NVDAT:
                        case USB_SAVE_NVSYNC:
                        case USB_SAVE_NVOPT:
                        case USB_LOAD_URL:
                            bufferPos_ = 0;
                            break;
                            
                        default:
                            if ((header[HEADER_TYPE]&0x80) == 0x80)
                            {
                                switch (header[HEADER_BLOAD_TYPE])
                                {
                                    case USB_SAVE_BANK:
                                    case USB_SAVE_BANK_ALL:
                                        bufferPos_ = 0;
                                        break;
                                        
                                    case USB_SAVE_BANK_ALL_CONT:
                                        bufferPos_ = bank_*0x2000;
                                        break;
                                }
                            }
                            else
                            {
                                mode_ = USB_IDLE;
                                ef_ = 1;
                            }
                            break;
                    }
                }
                bufferFlush_ = 2;
            }
            break;
            
        case USB_URL_CONTINUE:
            header[bufferPos_++] = value;
            if (bufferPos_ == HEADER_LENGTH)
            {
                if (header[HEADER_MODE] == USB_CANCEL)
                {
                    mode_= USB_IDLE;
                    ef_ = 1;
                }
                else
                {
                    mode_ = USB_URL;
                }
                bufferFlush_ = 2;
            }
            break;
            
        case USB_GET_TRACK_CONTINUE:
            header[bufferPos_++] = value;
            if (bufferPos_ == HEADER_LENGTH)
            {
                mode_ = USB_GET_TRACK;
                
                if (buffer_[IMG_TYPE] == 0)
                {
                    buffer_[IMG_TYPE] = 1;
                }
                else
                {
                    buffer_[IMG_TRACK] += 1;
                    if (buffer_[IMG_TRACK] == tracks_ && buffer_[IMG_SIDE_NUMBER] == 0)
                    {
                        buffer_[IMG_SIDE] = (buffer_[IMG_SIDE] & 0xf) | 0xB0;
                        buffer_[IMG_SIDE_NUMBER] = 1;
                        buffer_[IMG_TRACK] = 0;
                    }
                    if (buffer_[IMG_TRACK] == (tracks_ - 1) && buffer_[IMG_SIDE_NUMBER] == sides_)
                        buffer_[IMG_TYPE] = 2;
                }
                
                bufferPos_ = 0;
                bufferFlush_ = 2;
                if (wxFile::Exists(fileName_))
                {
                    if (inputFile.Open(fileName_, _("rb")))
                    {
                        track = (buffer_[IMG_TRACK]*(sides_+1) + buffer_[IMG_SIDE_NUMBER])*0x800;
                        inputFile.Seek(track);
                        length_ = inputFile.Read(buffer_, 0x800);
                        length_ += 4;
                        inputFile.Close();
                    }
                }
            }
            break;
            
        case USB_PUT_TRACK:
            buffer_[bufferPos_++] = value;
            length_--;
            if (length_ == 0)
            {
                if (!fileName_.empty())
                {
                    if (wxFile::Exists(fileName_))
                    {
                        outputFile.Open(fileName_, wxFile::read_write);
                        track = (header[HEADER_TRACK]*(sides_+1) + header[HEADER_SIDE_NUMBER])*0x800;
                        outputFile.Seek(track);
                        outputFile.Write(buffer_, saveLength_);
                        outputFile.Close();
                    }
                    else
                    {
                        if (outputFile.Create(fileName_))
                        {
                            outputFile.Write(buffer_, saveLength_);
                            outputFile.Close();
                        }
                        else
                        {
                            mode_ = USB_IDLE;
                            ef_ = 1;
                            (void)wxMessageBox( "Error writing " + fileName_,
                                               "Emma 02", wxICON_ERROR | wxOK );
                        }
                    }
                }
                mode_ = USB_IDLE;
                ef_ = 1;
            }
            break;
            
        case USB_SAVE_URL_INPUT:
            buffer_[bufferPos_++] = value;
            length_--;
            if (length_ == 0)
            {
                for (int i=0; i<0x3c0; i++)
                {
                    urlScreens[currentUrlScreen_].screenData[i] = buffer_[i];
                }
                
                wxString linkName;
                Byte inputNumber, characterValue;
                
                for (int i=0x400; i<0x7c0; i++)
                {
                    inputNumber = urlScreens[currentUrlScreen_].screenData[i];
                    linkName = "";
                    if (inputNumber != 0)
                    {
                        if (urlScreens[currentUrlScreen_].inputInfo[inputNumber-1].inputType == INPUT_TYPE_RADIO)
                        {
                            characterValue = urlScreens[currentUrlScreen_].screenData[i-0x400] & 0x7f;
                            if (characterValue == URL_RADIO_ON)
                                linkName = urlScreens[currentUrlScreen_].urlLinks[inputNumber-1];
                            if (linkName != "")
                            {
                                for (int screen=1; screen<numberOfUrlScreens_; screen++)
                                {
                                    if (screen != currentUrlScreen_)
                                    {
                                        for (int j=0x400; j<0x7c0; j++)
                                        {
                                            inputNumber = urlScreens[screen].screenData[j];
                                            if (inputNumber != 0 && inputNumber <= urlScreens[screen].numberOfLinks)
                                            {
                                                if (urlScreens[screen].inputInfo[inputNumber-1].inputType == INPUT_TYPE_RADIO)
                                                {
                                                    if (urlScreens[screen].urlLinks[inputNumber-1] == linkName)
                                                    {
                                                        urlScreens[screen].screenData[j-0x400] = URL_RADIO_OFF;
                                                    }
                                                }
                                                
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                mode_ = USB_IDLE;
                ef_ = 1;
            }
            break;
            
        case USB_SAVE:
        case USB_SAVE_NVDAT:
        case USB_SAVE_NVSYNC:
        case USB_SAVE_NVOPT:
            buffer_[bufferPos_++] = value;
            length_--;
            if (length_ == 0)
            {
                if (!fileName_.empty())
                {
                    if (header[HEADER_TYPE] == 0x11)
                    {
                        if (wxFile::Exists(fileName_))
                        {
                            outputTextFile.Open(fileName_);
                            writeDumpFile(&outputTextFile);
                        }
                        else
                        {
                            if (outputTextFile.Create(fileName_))
                            {
                                writeDumpFile(&outputTextFile);
                            }
                            else
                            {
                                mode_ = USB_IDLE;
                                ef_ = 1;
                                (void)wxMessageBox( "Error writing " + fileName_,
                                                   "Emma 02", wxICON_ERROR | wxOK );
                            }
                        }
                    }
                    else
                    {
                        networkError_ = USB_FILE_EXISTS;
                        if (mode_ == USB_SAVE_NVSYNC)
                        {
                            convertSyncToScores();
                        }
                        if (networkError_ == USB_FILE_EXISTS)
                        {
                            if (outputFile.Create(fileName_, true))
                            {
                                outputFile.Write(buffer_, saveLength_);
                                outputFile.Close();
                            }
                            else
                            {
                                mode_ = USB_IDLE;
                                ef_ = 1;
                                (void)wxMessageBox( "Error writing " + fileName_,
                                                   "Emma 02", wxICON_ERROR | wxOK );
                            }
                        }
                    }
                }
                mode_ = USB_IDLE;
                ef_ = 1;
            }
            break;
            
        case USB_SAVE_BANK_ALL:
        case USB_SAVE_BANK_ALL_CONT:
            buffer_[bufferPos_++] = value;
            length_--;
            if (length_ == 0)
            {
                if (bank_ == 7)
                {
                    if (!fileName_.empty())
                    {
                        if (outputFile.Create(fileName_, true))
                        {
                            outputFile.Write(buffer_, 0x10000);
                            outputFile.Close();
                        }
                        else
                        {
                            mode_ = USB_IDLE;
                            ef_ = 1;
                            (void)wxMessageBox( "Error writing " + fileName_,
                                               "Emma 02", wxICON_ERROR | wxOK );
                        }
                    }
                }
                mode_ = USB_IDLE;
                ef_ = 1;
            }
            break;
            
        case USB_SAVE_NVALL:
            buffer_[bufferPos_++] = value;
            length_--;
            if (length_ == 0)
            {
                if (!fileName_.empty())
                {
                    if (outputFile.Create(fileName_, true))
                    {
                        outputFile.Write(buffer_, 1000);
                        outputFile.Close();
                    }
                    else
                    {
                        mode_ = USB_IDLE;
                        ef_ = 1;
                        (void)wxMessageBox( "Error writing " + fileName_,
                                           "Emma 02", wxICON_ERROR | wxOK );
                    }
                    fileName_ = rootDirectory_+"system.bin";
                    for (int i=0; i<16; i++)
                        buffer_[i] = buffer_[i+1000];
                    if (outputFile.Create(fileName_, true))
                    {
                        outputFile.Write(buffer_, 16);
                        outputFile.Close();
                    }
                    else
                    {
                        mode_ = USB_IDLE;
                        ef_ = 1;
                        (void)wxMessageBox( "Error writing " + fileName_,
                                           "Emma 02", wxICON_ERROR | wxOK );
                    }
                }
                mode_ = USB_IDLE;
                ef_ = 1;
            }
            break;
    }
    if (mode_ == USB_IDLE)
        ef_ = 1;
    else
        ef_ = 0;
}

void Usbcard::storeClockValue(Byte value)
{
    Byte high, low;
    
    high = (int)(value/10)*16;
    low = value - (int)(value/10)*10;
    header[bufferPos_++] = high + low;
}

void Usbcard::storeDateFormat(wxString value)
{
    if (value == "%d")
        header[bufferPos_++] = 0xfc;
    
    if (value == "%m")
        header[bufferPos_++] = 0xfd;
    
    if (value == "%y")
        header[bufferPos_++] = 0xfe;
}

void Usbcard::storeTimeFormat(wxString value)
{
    if (value == "%l" || value == "%h")
        header[bufferPos_++] = 0x12;
    else
        header[bufferPos_++] = 0x24;
}

wxString Usbcard::convertCase(wxString strValue, int type, int convertType)
{
    char convert;
    switch (type)
    {
        case 1:
            switch (convertType)
        {
            case MAKELOWER:
                strValue = strValue.MakeLower();
                break;
            case MAKEUPPER:
                strValue = strValue.MakeUpper();
                break;
            case CAPITALIZE:
                strValue = strValue.Capitalize();
                break;
        }
            break;
            
        case 2:
            break;
            
        case 3:
            for (size_t i=0; i<strValue.Len(); i++)
            {
                convert = strValue.GetChar(i);
                if (convert >=0x41 && convert <=0x5a)
                {
                    convert += 0x20;
                    strValue.SetChar(i, convert);
                }
                else
                {
                    if (convert >=0x61 && convert <=0x7a)
                    {
                        convert -= 0x20;
                        strValue.SetChar(i, convert);
                    }
                }
            }
            break;
            
        default:
            switch (convertType)
        {
            case MAKELOWER:
                strValue = strValue.MakeLower();
                break;
            case MAKEUPPER:
                strValue = strValue.MakeUpper();
                break;
            case CAPITALIZE:
                strValue = strValue.Capitalize();
                break;
        }
            break;
            
    }
    return strValue;
}

bool Usbcard::usb_ef()
{
    return ef_;
}

void Usbcard::dir()
{
    wxString directory, extension, name;
    wxFFile inputFile;
    wxULongLong length;
    
    directory = additionalDir_.GetLongPath();
    while ( contCat_ && bufferPos_ < 128)
    {
        wxFileName swFullPath = wxFileName(directory + catFileName_, wxPATH_NATIVE);
        extension = swFullPath.GetExt();
        name = swFullPath.GetName();
        
        if (name.Len() > 18)
            name = name.Left(18);
        
        name = convertCase(name, charType_, MAKEUPPER);
        for (size_t i=0; i<name.Len(); i++)
            header[i+bufferPos_+2] = name.GetChar(i);
        
        length = swFullPath.GetSize();
        
        header[HEADER_NUMBER+bufferPos_+1] = (number_ >> 8) & 0xff;
        header[HEADER_NUMBER+bufferPos_] = number_&0xff;
        number_++;
        if ((number_&0xf) == 0xa)
            number_ += 6;
        if ((number_&0xff) == 0xa0)
            number_ += 0x60;
        if (number_ == 0xa00)
            number_ = 1;
        
        if (dirSearch_)
            header[HEADER_TYPE+bufferPos_] = 7;
        else
        {
            if ((length % 1024) == 0)
                length = (length / 1024);
            else
                length = (length / 1024) + 1;
            if (length <= 255)
            {
                length_ = (int) length.ToULong();
                header[HEADER_LENGTH_H+bufferPos_] = (int) (length_);
                //                header[HEADER_LENGTH_H+bufferPos_] = (int) (length_ / 10) + 0x30;
                //                header[HEADER_LENGTH_L+bufferPos_] = (int) (length_ - ((int) (length_ / 10) * 10)) + 0x30;
            }
        }
        
        if (extension == "txt")
            header[HEADER_TYPE+bufferPos_] = 0x11;
        
        if (extension == "bak")
            header[HEADER_TYPE+bufferPos_] = 0x20;
        
        if (extension == "img")
            header[HEADER_TYPE+bufferPos_] = 0x12;
        
        if (extension == "ram")
            header[HEADER_TYPE+bufferPos_] = 0x13;
        
        if (extension == "comx")
        {
            if (inputFile.Open(directory + catFileName_, _("rb")))
            {
                inputFile.Read(buffer_, 15);
                header[HEADER_TYPE+bufferPos_] = buffer_[0];
                switch (buffer_[0])
                {
                    case 1: /* Machine code LOAD */
                        length_ -= 11;
                        header[HEADER_START_H+bufferPos_] = buffer_[5];
                        header[HEADER_START_L+bufferPos_] = buffer_[6];
                        header[HEADER_END_H+bufferPos_] = buffer_[7];
                        header[HEADER_END_L+bufferPos_] = buffer_[8];
                        header[HEADER_EXEC_H+bufferPos_] = buffer_[9];
                        header[HEADER_EXEC_L+bufferPos_] = buffer_[10];
                        break;
                        
                    case 2: /* Regular LOAD - old format */
                        length_ -= 15;
                        header[HEADER_TYPE+bufferPos_] = 6;
                        header[HEADER_START_H+bufferPos_] = buffer_[5];
                        header[HEADER_START_L+bufferPos_] = buffer_[6];
                        header[HEADER_END_H+bufferPos_] = buffer_[7];
                        header[HEADER_END_L+bufferPos_] = buffer_[8];
                        header[HEADER_EXEC_H+bufferPos_] = buffer_[9];
                        header[HEADER_EXEC_L+bufferPos_] = buffer_[10];
                        break;
                        
                    case 3: /* F&M BASIC LOAD */
                        length_ -= 15;
                        header[HEADER_START_H+bufferPos_] = buffer_[5];
                        header[HEADER_START_L+bufferPos_] = buffer_[6];
                        header[HEADER_END_H+bufferPos_] = buffer_[7];
                        header[HEADER_END_L+bufferPos_] = buffer_[8];
                        header[HEADER_EXEC_H+bufferPos_] = buffer_[9];
                        header[HEADER_EXEC_L+bufferPos_] = buffer_[10];
                        break;
                        
                    case 5: /* Data only LOAD */
                        length_ -= 7;
                        break;
                        
                    case 6: /* Regular LOAD - current format */
                        length_ -= 15;
                        header[HEADER_START_H+bufferPos_] = buffer_[5]+0x44;
                        header[HEADER_START_L+bufferPos_] = buffer_[6];
                        header[HEADER_END_H+bufferPos_] = buffer_[7]+0x44;
                        header[HEADER_END_L+bufferPos_] = buffer_[8];
                        header[HEADER_EXEC_H+bufferPos_] = buffer_[9]+0x44;
                        header[HEADER_EXEC_L+bufferPos_] = buffer_[10];
                        break;
                }
                inputFile.Close();
            }
        }
        
        bufferPos_ += 0x20;
        contCat_ = catDir_->GetNext(&catFileName_);
        
        if (!contCat_ && dirSearch_)
        {
            dirSearch_ = false;
            contCat_ = catDir_->GetFirst(&catFileName_, wxEmptyString, wxDIR_FILES | wxDIR_HIDDEN);
        }
    }
    if (!contCat_ && bufferPos_ < 128)
    {
        number_ = 0;
        delete catDir_;
        catDir_ = NULL;
    }
}

void Usbcard::save()
{
    Word arrayLength, dataEnd, stringStart, arrayStart;
    wxString directory;
    
    header[HEADER_RESULT] = USB_FILE_NOT_FOUND;
    directory = additionalDir_.GetLongPath();
    
    if ((header[HEADER_TYPE]&0xf0) == 0x20)
    {
        switch (header[HEADER_TYPE]&0x7f)
        {
            case USB_SAVE_NVDAT:
                fileName_ = rootDirectory_+"nvram.bin";
                fileNameOnly_ = "nvram";
                extensionOnly_ = ".bin";
                directory = rootDirectory_;
                break;
                
            case USB_SAVE_NVSYNC:
                fileName_ = rootDirectory_+"highscores.bin";
                fileNameOnly_ = "highscores";
                extensionOnly_ = ".bin";
                directory = rootDirectory_;
                if (wxFile::Exists(fileName_))
                    wxRemoveFile(fileName_);
                break;
                
            case USB_SAVE_NVOPT:
                fileName_ = rootDirectory_+"system.bin";
                fileNameOnly_ = "system";
                extensionOnly_ = ".bin";
                directory = rootDirectory_;
                break;
        }
    }
    if (wxFile::Exists(fileName_))
    {
        if (!wxFileName::IsFileWritable(fileName_))
            header[HEADER_RESULT] = USB_FILE_OPEN;
        else
        {
            if ((p_Main->getSbBackup() == 0 && (header[HEADER_TYPE]&0xf0) != 0x20) || (p_Main->getSbBackupSys() == 0 && (header[HEADER_TYPE]&0xf0) == 0x20))
            {
                int number=1;
                wxString numberString;
                
                numberString.Printf("%d",number);
                while (wxFile::Exists(directory + fileNameOnly_ + numberString + extensionOnly_ + ".bak"))
                {
                    number++;
                    numberString.Printf("%d",number);
                }
                wxRenameFile(fileName_, directory + fileNameOnly_ + numberString + extensionOnly_ +".bak",true);
            }
            else
                header[HEADER_RESULT] = USB_FILE_EXISTS;
        }
    }
    
    switch (header[HEADER_TYPE])
    {
        case 1:
            end_ = (header[HEADER_END_H]<<8) + header[HEADER_END_L];
            start_ = (header[HEADER_START_H]<<8) + header[HEADER_START_L];
            if (start_ >= end_)
                header[HEADER_RESULT] = USB_ADDRESS_ERROR;
            length_ = end_ - start_;
            buffer_[0] = header[HEADER_TYPE]&0x7f;
            buffer_[1] = 'C';
            buffer_[2] = 'O';
            buffer_[3] = 'M';
            buffer_[4] = 'X';
            buffer_[5] = header[HEADER_START_H];
            buffer_[6] = header[HEADER_START_L];
            buffer_[7] = ((end_-1) & 0xff00) >> 8;
            buffer_[8] = (end_-1) & 0xff;
            buffer_[9] = header[HEADER_EXEC_H];
            buffer_[10] = header[HEADER_EXEC_L];
            saveLength_ = length_ + 11;
            break;
            
        case 3:
            end_ = (header[HEADER_END_H]<<8) + header[HEADER_END_L];
            length_ = end_ - (header[HEADER_START_HIGH_BYTE] << 8);
            
            buffer_[0] = header[HEADER_TYPE]&0x7f;
            buffer_[1] = 'C';
            buffer_[2] = 'O';
            buffer_[3] = 'M';
            buffer_[4] = 'X';
            buffer_[5] = header[HEADER_START_H];
            buffer_[6] = header[HEADER_START_L];
            buffer_[7] = header[HEADER_END_H];
            buffer_[8] = header[HEADER_END_L];
            buffer_[9] = header[HEADER_EXEC_H];
            buffer_[10] = header[HEADER_EXEC_L];
            buffer_[11] = header[HEADER_EXEC_H];
            buffer_[12] = header[HEADER_EXEC_L];
            buffer_[13] = header[HEADER_EXEC_H];
            buffer_[14] = header[HEADER_EXEC_L];
            saveLength_ = length_ + 15;
            break;
            
        case 5:
            arrayStart = header[HEADER_ARRAY_H] << 8;
            arrayStart += header[HEADER_ARRAY_L];
            stringStart = header[HEADER_STRING_H] << 8;
            stringStart += header[HEADER_STRING_L];
            dataEnd = header[HEADER_EOD_H] << 8;
            dataEnd += header[HEADER_EOD_L];
            length_ = dataEnd-arrayStart;
            arrayLength = stringStart-arrayStart;
            
            buffer_[0] = header[HEADER_TYPE]&0x7f;
            buffer_[1] = 'C';
            buffer_[2] = 'O';
            buffer_[3] = 'M';
            buffer_[4] = 'X';
            buffer_[5] = (arrayLength >> 8) &0xff;
            buffer_[6] = arrayLength &0xff;
            saveLength_ = length_ + 7;
            break;
            
        case 6:
            end_ = (header[HEADER_END_H]<<8) + header[HEADER_END_L];
            length_ = end_ - (header[HEADER_START_HIGH_BYTE] << 8);
            
            buffer_[0] = header[HEADER_TYPE]&0x7f;
            buffer_[1] = 'C';
            buffer_[2] = 'O';
            buffer_[3] = 'M';
            buffer_[4] = 'X';
            buffer_[5] = header[HEADER_START_H]-header[HEADER_START_HIGH_BYTE];
            buffer_[6] = header[HEADER_START_L];
            buffer_[7] = header[HEADER_END_H]-header[HEADER_START_HIGH_BYTE];
            buffer_[8] = header[HEADER_END_L];
            buffer_[9] = header[HEADER_EXEC_H]-header[HEADER_START_HIGH_BYTE];
            buffer_[10] = header[HEADER_EXEC_L];
            buffer_[11] = header[HEADER_EXEC_H]-header[HEADER_START_HIGH_BYTE];
            buffer_[12] = header[HEADER_EXEC_L];
            buffer_[13] = header[HEADER_EXEC_H]-header[HEADER_START_HIGH_BYTE];
            buffer_[14] = header[HEADER_EXEC_L];
            saveLength_ = length_ + 15;
            break;
            
        case 0x10:
        case 0x11:
            end_ = (header[HEADER_END_H]<<8) + header[HEADER_END_L];
            start_ = (header[HEADER_START_H]<<8) + header[HEADER_START_L];
            length_ = end_ - start_;
            saveLength_ = length_;
            break;
            
        case USB_SAVE_URL_INPUT:
            length_ = 0x3c0;
            saveLength_ = 0x3c0;
            break;
            
        case USB_SAVE_NVALL:
            length_ = 0x3f8;
            saveLength_ = 0x3f8;
            break;
            
        case USB_SAVE_NVSYNC:
            length_ = 1001;
            saveLength_ = 1000;
            break;
            
        case USB_SAVE_NVDAT:
            length_ = 1000;
            saveLength_ = 1000;
            break;
            
        case USB_SAVE_NVOPT:
            length_ = 16;
            saveLength_ = 16;
            break;
            
        default:
            if ((header[HEADER_TYPE]&0x80) == 0x80)
            {
                switch (header[HEADER_BLOAD_TYPE])
                {
                    case USB_SAVE_BANK:
                        length_ = 0x2000;
                        saveLength_ = 0x2000;
                        break;
                        
                    case USB_SAVE_BANK_ALL:
                        length_ = 0x2000;
                        saveLength_ = 0xffff;
                        bank_ = 0;
                        break;
                        
                    case USB_SAVE_BANK_ALL_CONT:
                        length_ = 0x2000;
                        saveLength_ = 0xffff;
                        bank_++;
                        break;
                }
            }
            break;
    }
    mode_ = USB_SAVE_HEADER;
    bufferPos_ = HEADER_START;
    bufferFlush_ = 2;
}

void Usbcard::load()
{
    Word arrayLength, dataEnd, stringStart;
    wxFFile inputFile;
    size_t real_length;
    
    if (wxFile::Exists(fileName_))
    {
        if (inputFile.Open(fileName_, _("rb")))
        {
            real_length = inputFile.Read(buffer_, 65536);
            length_ = real_length;
            real_length = (size_t)inputFile.Length();
            if ((buffer_[0] == 1 || buffer_[0] == 2 || buffer_[0] == 3 || buffer_[0] == 4 || buffer_[0] == 5 || buffer_[0] == 6) && (buffer_[1] == 'C') && (buffer_[2] == 'O') && (buffer_[3] == 'M') && (buffer_[4] == 'X'))
            {
                switch (buffer_[0])
                {
                    case 1: /* Machine code LOAD */
                        length_ -= 11;
                        header[HEADER_TYPE] = buffer_[0];
                        header[HEADER_RESULT] = USB_FILE_EXISTS;
                        header[HEADER_LENGTH_H] = (length_&0xff00)>>8;
                        header[HEADER_LENGTH_L] = length_&0xff;
                        header[HEADER_START_H] = buffer_[5];
                        header[HEADER_START_L] = buffer_[6];
                        header[HEADER_END_H] = buffer_[7];
                        header[HEADER_END_L] = buffer_[8];
                        header[HEADER_EXEC_H] = buffer_[9];
                        header[HEADER_EXEC_L] = buffer_[10];
                        break;
                        
                    case 2: /* Regular LOAD - old format */
                        length_ -= 15;
                        header[HEADER_TYPE] = 6;
                        header[HEADER_RESULT] = USB_FILE_EXISTS;
                        header[HEADER_LENGTH_H] = (length_&0xff00)>>8;
                        header[HEADER_LENGTH_L] = length_&0xff;
                        header[HEADER_START_HIGH_BYTE] = header[HEADER_START_H];
                        header[HEADER_START_H] = buffer_[5] - 0x44 + header[HEADER_START_HIGH_BYTE];
                        header[HEADER_START_L] = buffer_[6];
                        header[HEADER_END_H] = buffer_[7] - 0x44 + header[HEADER_START_HIGH_BYTE];
                        header[HEADER_END_L] = buffer_[8];
                        header[HEADER_EXEC_H] = buffer_[9] - 0x44 + header[HEADER_START_HIGH_BYTE];
                        header[HEADER_EXEC_L] = buffer_[10];
                        break;
                        
                    case 3: /* F&M BASIC LOAD - old format */
                        length_ -= 15;
                        header[HEADER_TYPE] = buffer_[0];
                        header[HEADER_RESULT] = USB_FILE_EXISTS;
                        header[HEADER_LENGTH_H] = (length_&0xff00)>>8;
                        header[HEADER_LENGTH_L] = length_&0xff;
                        header[HEADER_START_HIGH_BYTE] = header[HEADER_START_H];
                        header[HEADER_START_H] = buffer_[5] - 0x67 + header[HEADER_START_HIGH_BYTE];
                        header[HEADER_START_L] = buffer_[6];
                        header[HEADER_END_H] = buffer_[7] - 0x67 + header[HEADER_START_HIGH_BYTE];
                        header[HEADER_END_L] = buffer_[8];
                        header[HEADER_EXEC_H] = buffer_[9] - 0x67 + header[HEADER_START_HIGH_BYTE];
                        header[HEADER_EXEC_L] = buffer_[10];
                        break;
                        
                    case 5: /* Data only LOAD */
                        start_ = header[HEADER_ARRAY_H] << 8;
                        start_ += header[HEADER_ARRAY_L];
                        length_ -= 7;
                        arrayLength = buffer_[5] << 8;
                        arrayLength += buffer_[6];
                        dataEnd = start_ + length_;
                        header[HEADER_TYPE] = buffer_[0];
                        header[HEADER_RESULT] = USB_FILE_EXISTS;
                        header[HEADER_LENGTH_H] = (length_&0xff00)>>8;
                        header[HEADER_LENGTH_L] = length_&0xff;
                        header[HEADER_EOD_H] = (dataEnd >> 8) & 0xff;
                        header[HEADER_EOD_L] = dataEnd & 0xff;
                        stringStart = start_ + arrayLength;
                        header[HEADER_STRING_H] = (stringStart >> 8) & 0xff;
                        header[HEADER_STRING_L] = stringStart & 0xff;
                        break;
                        
                    case 6: /* Regular LOAD - current format */
                        length_ -= 15;
                        header[HEADER_TYPE] = buffer_[0];
                        header[HEADER_RESULT] = USB_FILE_EXISTS;
                        header[HEADER_LENGTH_H] = (length_&0xff00)>>8;
                        header[HEADER_LENGTH_L] = length_&0xff;
                        header[HEADER_START_HIGH_BYTE] = header[HEADER_START_H];
                        header[HEADER_START_H] = buffer_[5] + header[HEADER_START_HIGH_BYTE];
                        header[HEADER_START_L] = buffer_[6];
                        header[HEADER_END_H] = buffer_[7] + header[HEADER_START_HIGH_BYTE];
                        header[HEADER_END_L] = buffer_[8];
                        header[HEADER_EXEC_H] = buffer_[9] + header[HEADER_START_HIGH_BYTE];
                        header[HEADER_EXEC_L] = buffer_[10];
                        break;
                }
                if (buffer_[0] == 5)
                {
                    if (header[HEADER_MODE] != USB_LOAD_DATA)
                        header[HEADER_RESULT] = USB_TYPE_ERROR;
                }
                else
                {
                    if (header[HEADER_MODE] != USB_LOAD)
                        header[HEADER_RESULT] = USB_TYPE_ERROR;
                }
            }
            else
            {
                if ((header[HEADER_TYPE]&0xf0) >= 0x20)
                {
                    switch (header[HEADER_TYPE]&0x7f)
                    {
                        case USB_SAVE_NVALL:
                            for (int i=0; i<16; i++)
                                buffer_[i+1000] = buffer_[i];
                            fileName_ = rootDirectory_+"nvram.bin";
                            inputFile.Close();
                            
                            if (wxFile::Exists(fileName_))
                            {
                                if (inputFile.Open(fileName_, _("rb")))
                                {
                                    length_ = inputFile.Read(buffer_, 1000);
                                    length_ += 16;
                                    header[HEADER_RESULT] = USB_FILE_EXISTS;
                                    header[HEADER_LENGTH_H] = 0x03;
                                    header[HEADER_LENGTH_L] = 0xf8;
                                    header[HEADER_START_H] = 0xf0;
                                    header[HEADER_START_L] = 0;
                                }
                            }
                            break;
                            
                        case USB_SAVE_NVDAT:
                            header[HEADER_RESULT] = USB_FILE_EXISTS;
                            header[HEADER_LENGTH_H] = 0x03;
                            header[HEADER_LENGTH_L] = 0xe8;
                            header[HEADER_START_H] = 0xf0;
                            header[HEADER_START_L] = 0;
                            break;
                            
                        case USB_SAVE_NVSYNC:
                            header[HEADER_RESULT] = networkError_;
                            header[HEADER_LENGTH_H] = 0x03;
                            header[HEADER_LENGTH_L] = 0xe8;
                            header[HEADER_START_H] = 0xf0;
                            header[HEADER_START_L] = 0;
                            break;
                            
                        case USB_SAVE_NVOPT:
                            header[HEADER_RESULT] = USB_FILE_EXISTS;
                            header[HEADER_LENGTH_H] = 0;
                            header[HEADER_LENGTH_L] = 16;
                            header[HEADER_START_H] = 0xf3;
                            header[HEADER_START_L] = 0xe8;
                            break;
                    }
                    if ((header[HEADER_TYPE]&0x80) == 0x80)
                    {
                        switch (header[HEADER_BLOAD_TYPE])
                        {
                            case USB_SAVE_BANK:
                                if (real_length < 0x2000)
                                {
                                    header[HEADER_RESULT] = USB_FILE_TOO_SMALL_8;
                                }
                                else
                                {
                                    if (real_length > 0x2000)
                                    {
                                        header[HEADER_RESULT] = USB_FILE_TOO_LARGE_8;
                                        length_ = 0x2000;
                                    }
                                    else
                                        header[HEADER_RESULT] = USB_FILE_EXISTS;
                                }
                                header[HEADER_BLOAD_LENGTH_H] = (length_ & 0xff00) >> 8;
                                header[HEADER_BLOAD_LENGTH_L] = length_ & 0xff;
                                break;
                                
                            case USB_SAVE_BANK_ALL:
                                if (real_length < 0x10000)
                                {
                                    header[HEADER_RESULT] = USB_FILE_TOO_SMALL_64;
                                }
                                else
                                {
                                    if (real_length > 0x10000)
                                        header[HEADER_RESULT] = USB_FILE_TOO_LARGE_64;
                                    else
                                        header[HEADER_RESULT] = USB_FILE_EXISTS;
                                }
                                if (real_length > 0x2000)
                                    length_ = 0x2000;
                                header[HEADER_BLOAD_LENGTH_H] = (length_ & 0xff00) >> 8;
                                header[HEADER_BLOAD_LENGTH_L] = length_ & 0xff;
                                bank_ = 0;
                                break;
                                
                            case USB_SAVE_BANK_ALL_CONT:
                                bank_++;
                                real_length = real_length - bank_ * 0x2000;
                                length_ = real_length;
                                if (real_length > 0x2000)
                                    length_ = 0x2000;

                                header[HEADER_BLOAD_LENGTH_H] = (length_ & 0xff00) >> 8;
                                header[HEADER_BLOAD_LENGTH_L] = length_ & 0xff;
                                break;
                        }
                    }
                }
                else
                {
                    header[HEADER_TYPE] = 0x10;
                    header[HEADER_RESULT] = USB_FILE_EXISTS;
                    header[HEADER_LENGTH_H] = (length_&0xff00)>>8;
                    header[HEADER_LENGTH_L] = length_&0xff;
                }
            }
            inputFile.Close();
        }
    }
    else
    {
        if ((header[HEADER_TYPE]&0x7f) == USB_SAVE_NVSYNC)
            if (networkError_ == USB_FILE_EXISTS)
                header[HEADER_RESULT] = USB_FILE_NOT_FOUND;
            else
                header[HEADER_RESULT] = networkError_;
            else
                header[HEADER_RESULT] = USB_FILE_NOT_FOUND;
    }
    mode_ = USB_LOAD_HEADER;
    bufferPos_ = HEADER_START;
    bufferFlush_ = 2;
}

void Usbcard::putTrack()
{
    header[HEADER_RESULT] = USB_FILE_NOT_FOUND;
    
    if (wxFile::Exists(fileName_))
    {
        if (header[HEADER_RESULT] != USB_FILE_EXISTS)
        {
            if (!wxFileName::IsFileWritable(fileName_))
                header[HEADER_RESULT] = USB_FILE_OPEN;
        }
        
        if (header[HEADER_RESULT] != USB_FILE_OPEN)
        {
            if (p_Main->getSbBackup() == 0)
            {
                if (header[HEADER_TYPE] == 0)
                {
                    int number=1;
                    wxString numberString;
                    
                    numberString.Printf("%d",number);
                    while (wxFile::Exists(additionalDir_.GetLongPath() + fileNameOnly_ + numberString + extensionOnly_ + ".bak"))
                    {
                        number++;
                        numberString.Printf("%d",number);
                    }
                    wxRenameFile(fileName_, additionalDir_.GetLongPath() + fileNameOnly_ + numberString + extensionOnly_ +".bak",true);
                }
            }
            else
            {
                header[HEADER_RESULT] = USB_FILE_EXISTS;
                if (header[HEADER_TYPE] != 0)
                {
                    header[HEADER_MODE] |= 0x80;
                }
            }
        }
    }
    
    if ((header[HEADER_MODE] & 0x80) != 0 || header[HEADER_RESULT] == USB_FILE_NOT_FOUND)
    {
        tracks_ = header[HEADER_DENSITY];
        tracks_ = (tracks_ + 1) * 35;
        
        sides_ = header[HEADER_SIDES];
        
        if (header[HEADER_TYPE] == 0)
        {
            //            header[HEADER_SIDE]= (Byte)0x14;
            header[HEADER_SIDE_NUMBER]=0;
            header[HEADER_TRACK]=0;
            header[HEADER_TYPE]=1;
        }
        else
        {
            if (header[HEADER_SIDE_NUMBER] == 0)
            {
                if (sides_ == 1)
                {
                    //                    header[HEADER_SIDE] = (Byte)0x34;
                    header[HEADER_SIDE] = (header[HEADER_SIDE] & 0xf) | 0x30;
                    header[HEADER_SIDE_NUMBER] = 1;
                }
                else
                    header[HEADER_TRACK] += 1;
            }
            else
            {
                //                header[HEADER_SIDE]= (Byte)0x14;
                header[HEADER_SIDE]= (header[HEADER_SIDE] & 0xf) | 0x10;
                header[HEADER_SIDE_NUMBER] = 0;
                header[HEADER_TRACK] += 1;
            }
            if (header[HEADER_TRACK] == (tracks_ - 1) && header[HEADER_SIDE_NUMBER] == sides_)
                header[HEADER_TYPE] = 2;
        }
        
        length_ = 0x800;
        saveLength_ = 0x800;
    }
    mode_ = USB_PUT_TRACK_HEADER;
    bufferPos_ = HEADER_START;
    
    bufferFlush_ = 2;
}

void Usbcard::getTrack()
{
    wxFFile inputFile;
    
    if (!wxFile::Exists(fileName_))
        header[HEADER_RESULT] = USB_FILE_NOT_FOUND;
    else
    {
        if (inputFile.Open(fileName_, _("rb")))
            inputFile.Read(buffer_, 0x20);
        
        tracks_ = buffer_ [0x12];
        sides_ = buffer_ [0x13];
        
        if (tracks_ != header[HEADER_DENSITY] || sides_ != header[HEADER_SIDES])
        {
            header[HEADER_RESULT] = USB_SS_SD;
            if (tracks_ == 1 && sides_ == 0)
                header[HEADER_RESULT] = USB_SS_DD;
            if (tracks_ == 0 && sides_ == 1)
                header[HEADER_RESULT] = USB_DS_SD;
        }
        
        tracks_ = (tracks_ + 1) * 35;
    }
    buffer_[IMG_SIDE] = (header[HEADER_SIDE] & 0xf) | 0x90;
    buffer_[IMG_SIDE_NUMBER]=0;
    buffer_[IMG_TRACK]=0;
    buffer_[IMG_TYPE]=0;
    
    mode_ = USB_GET_TRACK_HEADER;
    bufferPos_ = HEADER_START;
    bufferFlush_ = 2;
}

void Usbcard::testImg()
{
    wxFFile inputFile;
    
    if (!wxFile::Exists(fileName_))
        header[HEADER_RESULT] = USB_FILE_NOT_FOUND;
    else
    {
        if (inputFile.Open(fileName_, _("rb")))
            inputFile.Read(buffer_, 0x20);
        
        header[HEADER_DENSITY] = buffer_ [0x12];
        header[HEADER_SIDES] = buffer_ [0x13];
    }
    mode_ = USB_IMG_TEST_HEADER;
    bufferPos_ = HEADER_START;
    bufferFlush_ = 2;
}

void Usbcard::writeDumpFile(wxTextFile* outputTextFile)
{
    wxString strValue, tempStr;
    outputTextFile->Clear();
    outputTextFile->AddLine(fileNameOnly_+".TXT");
    
    wxDateTime systemTime = wxDateTime::Now();
    tempStr = systemTime.FormatISODate();
    outputTextFile->AddLine(tempStr);
    
    outputTextFile->AddLine("");
    strValue.Printf("Start: %04X",start_);
    outputTextFile->AddLine(strValue);
    strValue.Printf("End:   %04X",end_-1);
    outputTextFile->AddLine(strValue);
    outputTextFile->AddLine("");
    
    Word address = start_;
    
    while (address < end_)
    {
        strValue.Printf("%04X  ",address);
        tempStr.Printf("%02X ", buffer_[address-start_]&0xff);
        address++;
        strValue += tempStr;
        while (((address & 0xf) != 0) && (address < end_))
        {
            tempStr.Printf("%02X ", buffer_[address-start_]&0xff);
            address++;
            strValue += tempStr;
        }
        outputTextFile->AddLine(strValue);
        if ((address & 0xff) == 0)
            outputTextFile->AddLine("");
    }
    
    outputTextFile->Write();
    outputTextFile->Close();
}

void Usbcard::deleteBackup(wxFileName directory, wxString name, wxString ext, int flags)
{
    wxString fileName;
    bool fileFound;
    
    wxDir *dir;
    
    dir = new wxDir (directory.GetLongPath());
    fileFound = dir->GetFirst(&fileName, name+"*" + ext + ".bak", flags);
    
    while (fileFound)
    {
        if (wxFileName::IsFileWritable(directory.GetLongPath()+fileName))
            if (wxRemoveFile(directory.GetLongPath()+fileName))
                header[HEADER_START] = 8;
        
        fileFound = dir->GetNext(&fileName);
    }
    
    delete dir;
}

void Usbcard::deleteAllBackup(wxString directory, bool sub)
{
    wxString dirName;
    bool dirFound;
    wxFileName directoryName;
    
    wxDir *dir;
    
    directoryName = wxFileName(directory);
    dir = new wxDir (directoryName.GetLongPath());
    
    deleteBackup(directoryName, "", ".*", wxDIR_FILES);
    
    if (sub)
    {
        dirFound = dir->GetFirst(&dirName, "", wxDIR_DIRS);
        
        while (dirFound)
        {
            
            deleteAllBackup(directory+dirName+p_Main->getPathSep(), sub);
            dirFound = dir->GetNext(&dirName);
        }
    }
    
    delete dir;
}

void Usbcard::onBackupYes(wxString dir, bool sub)
{
    deleteAllBackup(dir, sub);
}

void Usbcard::convertSyncToScores()
{
    int pos;
    wxString swIdStr, highScoreStr, playerName, playerLocation, dataCopy, gameNumberStr, locationStr;
    long swId, highScore, gameNumber = 1, gameNumberCopy;
    Byte nibble, nibbleValue;
    wxString outputData = "", returnData = "", line;
    wxTextFile outputTextFile;
    
    wxString data;
    wxStringOutputStream html_stream(&data);
    
    wxCurlHTTP http("http://www.comx35.com/comx35sbhs/raw.php");
    
    wxString filename = p_Main->getRomDir(COMX, 0)+"errorfile.txt";
    
    if (!http.Get(html_stream))
    {
        if (!wxFile::Exists(filename))
            outputTextFile.Create(filename);
        
        outputTextFile.Open(filename);
        outputTextFile.AddLine("Open error");
        outputTextFile.AddLine("Header: "+http.GetResponseHeader());
        outputTextFile.AddLine("Body: "+http.GetResponseBody());
        outputTextFile.AddLine("Error: "+http.GetErrorString());
        outputTextFile.AddLine("Detailed error: "+http.GetDetailedErrorString());
        outputTextFile.AddLine("");
        outputTextFile.Write();
        outputTextFile.Close();
        
        networkError_ = USB_NETWORK_ERROR;
        return;
    }
    
    while (data.Left(1) == '#')
    {
        data = data.Right(data.Len()-1);
        
        swIdStr = data.BeforeFirst(';', &dataCopy);
        playerName = dataCopy.BeforeFirst(';', &data);
        highScoreStr = data.BeforeFirst(';', &dataCopy);
        gameNumberStr = dataCopy.BeforeFirst(';', &data);
        locationStr = data.BeforeFirst(';', &dataCopy);
        
        data = dataCopy.AfterFirst('~');
        
        int position =    data.Find('#');
        
        if (position != wxNOT_FOUND)
        {
            data = data.Mid(position);
        }
        
        swIdStr.ToCLong(&swId);
        highScoreStr.ToCLong(&highScore);
        gameNumberStr.ToCLong(&gameNumber);
        
        pos = getSwId((int)swId);
        if (pos != -1)
        {
            switch (swId)
            {
                case SW_ID_HAP1:
                case SW_ID_HAP2:
                    if (highScore >= getScore1(pos+10, 5, 10))
                    {
                        setName(playerName, pos, 8);
                        setScore1(highScore, pos+10, 5, -10);
                    }
                    else
                    {
                        playerName = convertString(getName(pos, 8));
                        playerLocation = p_Main->getSbLocation();
                        highScore = getScore1(pos+10, 5, 10);
                        line.Printf("\\%i;"+playerName+";%i;%i;"+playerLocation+";2;~",swId, highScore, 1);
                        outputData += line;
                        line.Printf("%i,%i;",swId, 1);
                        returnData += line;
                    }
                    break;
                    
                case SW_ID_HIT:
                    if (highScore >= getScore1(pos+1, 4, 0))
                        setScore1(highScore, pos+1, 4, 0);
                    else
                    {
                        playerName = p_Main->getSbPlayer();
                        playerLocation = p_Main->getSbLocation();
                        highScore = getScore1(pos+1, 4, 0);
                        line.Printf("\\%i;"+playerName+";%i;%i;"+playerLocation+";2;~",swId, highScore, 1);
                        outputData += line;
                        line.Printf("%i,%i;",swId, 1);
                        returnData += line;
                    }
                    break;
                    
                case SW_ID_SPACE:
                    if (highScore >= getScore2(pos+1, 2, 10))
                        setScore2(highScore, pos+1, 2, 10);
                    else
                    {
                        playerName = p_Main->getSbPlayer();
                        playerLocation = p_Main->getSbLocation();
                        highScore = getScore2(pos+1, 2, 10);
                        line.Printf("\\%i;"+playerName+";%i;%i;"+playerLocation+";2;~",swId, highScore, 1);
                        outputData += line;
                        line.Printf("%i,%i;",swId, 1);
                        returnData += line;
                    }
                    break;
                    
                case SW_ID_WORM:
                    if (highScore >= getScore3(pos))
                        setScore3(highScore, pos);
                    else
                    {
                        playerName = p_Main->getSbPlayer();
                        playerLocation = p_Main->getSbLocation();
                        highScore = getScore3(pos);
                        line.Printf("\\%i;"+playerName+";%i;%i;"+playerLocation+";2;~",swId, highScore, 1);
                        outputData += line;
                        line.Printf("%i,%i;",swId, 1);
                        returnData += line;
                    }
                    break;
                    
                case SW_ID_GYG:
                    if (highScore >= getScore1(pos, 3, 32))
                    {
                        setScore1(highScore, pos, 3, -32);
                        setName(playerName, pos+3, 8);
                    }
                    else
                    {
                        playerLocation = p_Main->getSbLocation();
                        highScore = getScore1(pos, 3, 32);
                        playerName = convertString(getName(pos+3, 8));
                        line.Printf("\\%i;"+playerName+";%i;%i;"+playerLocation+";2;~",swId, highScore, 1);
                        outputData += line;
                        line.Printf("%i,%i;",swId, 1);
                        returnData += line;
                    }
                    break;
                    
                case SW_ID_GATE:
                    if (highScore >= getScore2(pos, 2, 10))
                        setScore2(highScore, pos, 2, 10);
                    else
                    {
                        playerName = p_Main->getSbPlayer();
                        playerLocation = p_Main->getSbLocation();
                        highScore = getScore2(pos, 2, 10);
                        line.Printf("\\%i;"+playerName+";%i;%i;"+playerLocation+";2;~",swId, highScore, 1);
                        outputData += line;
                        line.Printf("%i,%i;",swId, 1);
                        returnData += line;
                    }
                    break;
                    
                case SW_ID_DRAGON:
                    if (highScore >= getScore2(pos, 4, 1))
                        setScore2(highScore, pos, 4, 1);
                    else
                    {
                        playerName = p_Main->getSbPlayer();
                        playerLocation = p_Main->getSbLocation();
                        highScore = getScore2(pos, 4, 1);
                        line.Printf("\\%i;"+playerName+";%i;%i;"+playerLocation+";2;~",swId, highScore, 1);
                        outputData += line;
                        line.Printf("%i,%i;",swId, 1);
                        returnData += line;
                    }
                    break;
                    
                case SW_ID_CROSS:
                    if (highScore >= getScore2(pos, 4, 1))
                        setScore2(highScore, pos, 4, 1);
                    else
                    {
                        playerName = p_Main->getSbPlayer();
                        playerLocation = p_Main->getSbLocation();
                        highScore = getScore2(pos, 4, 1);
                        line.Printf("\\%i;"+playerName+";%i;%i;"+playerLocation+";2;~",swId, highScore, 1);
                        outputData += line;
                        line.Printf("%i,%i;",swId, 1);
                        returnData += line;
                    }
                    break;
                    
                case SW_ID_LOOK:
                    if (highScore >= getScore2(pos, 4, 1))
                        setScore2(highScore, pos, 4, 1);
                    else
                    {
                        playerName = p_Main->getSbPlayer();
                        playerLocation = p_Main->getSbLocation();
                        highScore = getScore2(pos, 4, 1);
                        line.Printf("\\%i;"+playerName+";%i;%i;"+playerLocation+";2;~",swId, highScore, 1);
                        outputData += line;
                        line.Printf("%i,%i;",swId, 1);
                        returnData += line;
                    }
                    break;
                    
                case SW_ID_TENNIS:
                    nibble = 0x0f;
                    gameNumberCopy = gameNumber;
                    while (gameNumberCopy != 1)
                    {
                        nibbleValue = getNibble(&pos, &nibble);
                        if (nibbleValue != 0)
                            pos += 2;
                        gameNumberCopy--;
                    }
                    if (highScore >= getScore4(pos, nibble))
                        setScore4(highScore, pos, nibble);
                    else
                    {
                        if (getScore4(pos, nibble) > 1)
                        {
                            playerName = p_Main->getSbPlayer();
                            playerLocation = p_Main->getSbLocation();
                            highScore = getScore4(pos, nibble);
                            line.Printf("\\%i;"+playerName+";%i;%i;"+playerLocation+";2;~",swId, highScore, gameNumber);
                            outputData += line;
                            line.Printf("%i,%i;",swId, gameNumber);
                            returnData += line;
                        }
                    }
                    break;
                    
                case SW_ID_MUSIC:
                    if (highScore >= getScore2(pos, 2, 1))
                        setScore2(highScore, pos, 2, 1);
                    else
                    {
                        playerName = p_Main->getSbPlayer();
                        playerLocation = p_Main->getSbLocation();
                        highScore = getScore2(pos, 2, 1);
                        line.Printf("\\%i;"+playerName+";%i;%i;"+playerLocation+";2;~",swId, highScore, 1);
                        outputData += line;
                        line.Printf("%i,%i;",swId, 1);
                        returnData += line;
                    }
                    break;
            }
        }
    }
    
    if (outputData.Len() > 0)
    {
        Encrypt encrypt;
        wxString newData = encrypt.encryptUrlData(outputData);
        
        wxString url="http://www.comx35.com/comx35sbhs/readfeed.php?data="+newData;
        
        wxCurlHTTP post(url);
        
        post.SetOpt(CURLOPT_FOLLOWLOCATION);
        
        if(!post.Get(html_stream))
        {
            if (!wxFile::Exists(filename))
                outputTextFile.Create(filename);
            
            outputTextFile.Open(filename);
            
            outputTextFile.Open("errorfile.txt");
            outputTextFile.AddLine("Post error");
            outputTextFile.AddLine("Header: "+http.GetResponseHeader());
            outputTextFile.AddLine("Body: "+http.GetResponseBody());
            outputTextFile.AddLine("Error: "+http.GetErrorString());
            outputTextFile.AddLine("Detailed error: "+http.GetDetailedErrorString());
            outputTextFile.AddLine("");
            outputTextFile.Write();
            outputTextFile.Close();
            
            networkError_ = USB_NETWORK_ERROR;
        }
        else
        {
            data.Replace("<BR />","");
            if (data.GetChar(0) == 10)
                data = data.Right(data.Len()-1);
            if (data != returnData)
            {
                if (!wxFile::Exists(filename))
                    outputTextFile.Create(filename);
                
                outputTextFile.Open(filename);
                outputTextFile.AddLine("Server error");
                outputTextFile.AddLine("Header: "+http.GetResponseHeader());
                outputTextFile.AddLine("Body: "+http.GetResponseBody());
                outputTextFile.AddLine("Error: "+http.GetErrorString());
                outputTextFile.AddLine("Detailed error: "+http.GetDetailedErrorString());
                outputTextFile.AddLine("");
                outputTextFile.Write();
                outputTextFile.Close();
                
                networkError_ = USB_SERVER_ERROR;
            }
        }
    }
}

wxString Usbcard::convertString(wxString string)
{
    int i=0;
    
    while (stringConversion[i] != "stop")
        string.Replace(stringConversion[i++],"");
    return string;
}

int Usbcard::getSwId(int find_sw_id)
{
    int pos = (Word)(buffer_[0x3e8] << 2);
    
    Byte sw_id = buffer_[pos++];
    int length = buffer_[pos++];
    int next = pos + length;
    
    while (sw_id != 0)
    {
        if (sw_id == find_sw_id)
            return pos;
        
        pos = next;
        sw_id = buffer_[pos++];
        length = buffer_[pos++];
        next = pos + length;
    }
    return -1;
}

wxString Usbcard::getName(int pos, int length)
{
    wxString playerName = "";
    for (int i=0; i<length; i++)
    {
        if ((buffer_[pos]&0x7f) >= 32)
            playerName += wxString::FromAscii(buffer_[pos++]&0x7f);
    }
    return playerName;
}

void Usbcard::setName(wxString name, int pos, int length)
{
    for (int i=0; i<length; i++)
    {
        if (i < (int)name.Len())
        {
            buffer_[pos] = name.GetChar(i);
            buffer_[pos++] |= 0x80;
        }
        else
            buffer_[pos++] = 0x20;
    }
}

int Usbcard::getScore1(int pos, int length, char addFactor)
{ // Happiehap 1 & 2, USB HIT, GYG
    int highScore = 0;
    int factor = pow((double)10,length-1);
    for (int i=0; i<length; i++)
    {
        highScore += (buffer_[pos++]+addFactor)*factor;
        factor /= 10;
    }
    return highScore;
}

void Usbcard::setScore1(long highScore, int pos, int length, char addFactor)
{ // Happiehap 1 & 2, USB HIT, GYG
    int factor = pow((double)10,length-1);
    for (int i=0; i<length; i++)
    {
        buffer_[pos++] = (int)(highScore / factor)+addFactor;
        highScore = highScore % factor;
        factor /= 10;
    }
}

int Usbcard::getScore2(int pos, int length, int factor)
{ // USB SPACE, GateMaster, Dragon, crossfire, Look, Music Ball
    int highScore = 0;
    int shiftFactor = (length - 1)*8;
    
    for (int i=0; i<length; i++)
    {
        highScore += ((Byte)buffer_[pos++] << shiftFactor);
        shiftFactor -= 8;
    }
    
    highScore *= factor;
    
    return highScore;
}

void Usbcard::setScore2(long highScore, int pos, int length, int factor)
{ // USB SPACE, GateMaster, Dragon, crossfire, Look, Music Ball
    int shiftFactor = (length - 1)*8;
    
    highScore /= factor;
    
    for (int i=0; i<length; i++)
    {
        buffer_[pos++] = (highScore >> shiftFactor) & 0xff;
        shiftFactor -= 8;
    }
}

int Usbcard::getScore3(int pos)
{ // USB WORM
    int highScore = 0;
    int factor = 1;
    
    for (int i=0; i<5; i++)
    {
        highScore += (factor * (buffer_[pos+i]-0x30));
        factor *= 10;
    }
    
    return highScore;
}

void Usbcard::setScore3(long highScore, int pos)
{ // USB WORM
    for (int i=0; i<5; i++)
    {
        buffer_[pos++] = (highScore % 10) + 0x30;
        highScore = (int) (highScore / 10);
    }
}

int Usbcard::getScore4(int pos, Byte nibbleMask)
{
    int highScore = 0;
    int factor;
    
    Byte nibble = getNibble(&pos, &nibbleMask);
    
    if (nibble != 0)
    {
        factor = 10000;
        
        for (int i=0; i<5; i++)
        {
            highScore += (nibble-1)*factor;
            factor /= 10;
            if (i != 4)
                nibble = getNibble(&pos, &nibbleMask);
        }
    }
    
    return highScore;
}

void Usbcard::setScore4(long highScore, int pos, Byte nibbleMask)
{
    int factor;
    
    Byte nibble = getNibble(&pos, &nibbleMask, false);
    
    if (nibble != 0)
    {
        factor = 10000;
        
        for (int i=0; i<5; i++)
        {
            nibble = (int)(highScore / factor)+1;
            highScore = highScore % factor;
            factor /= 10;
            setNibble(nibble, &pos, &nibbleMask);
        }
    }
    else
        getNibble(&pos, &nibbleMask);
}

Byte Usbcard::getNibble(int *pos, Byte *nibbleMask, bool increase)
{
    Byte nibble;
    
    nibble = buffer_[*pos] & *nibbleMask;
    if (*nibbleMask == 0xf0)
    {
        nibble = nibble >> 4;
        if (increase)
            *pos = *pos + 1;
    }
    if (increase)
        *nibbleMask = *nibbleMask ^ 0xff;
    
    return nibble;
}

void Usbcard::setNibble(Byte nibble, int *pos, Byte *nibbleMask)
{
    *nibbleMask = *nibbleMask ^ 0xff;
    if (*nibbleMask == 0x0f)
    {
        nibble = nibble << 4;
        buffer_[*pos] = (buffer_[*pos] & *nibbleMask) | nibble;
        *pos = *pos + 1;
    }
    else
        buffer_[*pos] = (buffer_[*pos] & *nibbleMask) | nibble;
}

void Usbcard::sendEmail()
{
    wxString headerStr, error, response;
    baseName_ = "";
    
    wxCurlHTTP http(fileNameOnly_);
    
    wxMemoryOutputStream stream;
    
    if (!http.Get(stream))
    {
        headerStr = http.GetResponseHeader();
        error = http.GetErrorString();
        
        email_error("Server error");
        return;
    }

    headerStr = http.GetResponseHeader();
    response = http.GetResponseBody();

    networkError_ = USB_FILE_EXISTS;
    urlLink_ = 0;
    shapeColour_ = 0;
    urlInput_ = false;
    
    wxStreamBuffer* outputStreamBuffer = stream.GetOutputStreamBuffer();
    rawUrlDataSize = outputStreamBuffer->GetBufferSize();
    outputStreamBuffer->Seek(0, wxFromStart);
    
    if (rawUrlDataSize == 0)
    {
        email("Email send");
    }
    else
    {
        email_error("Server error");
    }
}

void Usbcard::getPage()
{
    wxString headerStr, error, response;
    baseName_ = "";
    
    wxCurlHTTP http(fileNameOnly_);
    
    wxMemoryOutputStream stream;
    
    if (!http.Get(stream))
    {
        headerStr = http.GetResponseHeader();
        error = http.GetErrorString();
        
        if (numberOfUrlScreens_ > 0)
        {
            urlScreens[0].screenData[0x7fe] = reshape_ ;
            urlScreens[0].screenData[0x7ff] = reshapeInfo_;
            urlScreens[0].screenData[0x900] = 0;
            urlScreens[0].shapeLocation_ = 0;
            urlScreens[0].shapeNumber_ = 1;
            
            urlScreens[0].numberOfLinks = 0;
            urlScreens[0].numberOfRadios_ = 0;
        }
        
        networkError_ = USB_URL_NOTFOUND;
        return;
    }
    
    networkError_ = USB_FILE_EXISTS;
    urlLink_ = 0;
    shapeColour_ = 0;
    urlInput_ = false;
    
    wxStreamBuffer* outputStreamBuffer = stream.GetOutputStreamBuffer();
    rawUrlDataSize = outputStreamBuffer->GetBufferSize();
    outputStreamBuffer->Seek(0, wxFromStart);
    
    rawUrlData = (Byte*)malloc(rawUrlDataSize);
    
    numberOfUrlScreens_ = 2;
    urlScreens.resize(numberOfUrlScreens_);
    
    outputStreamBuffer->Read(rawUrlData, rawUrlDataSize);
    
    liNum_ = 0;
    emptyLines_ = 0;
    centerTag_ = false;
    rawPos = 0;
    currentUrlPos_ = 0;
    lastLink_ = 0;
    continueLink_ = false;
    currentUrlLinePos_ = 0;
    
    tableTag_ = 0;
    tdTag_ = false;
    preTag_ = false;
    charSet_ = CHAR_SET_STD;
    
    currentUrlScreen_ = 0;
    while (currentUrlPos_ < 0x7fd)
        urlScreens[currentUrlScreen_].screenData[currentUrlPos_++] = 0;
    
    for (int i=0x800; i<=0x900; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    wxString goText = "Go to URL:   " + fileNameOnly_;
    if (goText.Len() > 108)
        goText = goText.Left(108);
    
    Byte charByte;
    for (size_t i = 0; i < 108; i++)
    {
        if (i < goText.Len())
            charByte = goText.GetChar(i);
        else
            charByte = 32;
        
        if (charByte >='a' && charByte <='z')
            charByte -= 0x20;
        else
        {
            if (charByte >='A' && charByte <='Z')
                charByte += 0x20;
        }
        urlScreens[0].screenData[i] = charByte;
    }
    urlScreens[0].screenData[0xa] = 0;
    urlScreens[0].screenData[0xb] = 1;
    urlScreens[0].screenData[0xc] = 0;
    
    currentUrlPos_ = 0;
    currentUrlScreen_ = 1;
    numberOfForms_ = 0;
    
    for (int i=960; i<0x7fd; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (int i=0x800; i<=0x900; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (int i=0; i< HTML_LAST; i++)
        urlScreens[currentUrlScreen_].shape_[i] = 0;
    
    urlScreens[currentUrlScreen_].invalidCharFound_ = false;
    urlScreens[currentUrlScreen_].textInputFound_ = false;
    urlScreens[currentUrlScreen_].numberOfInvalidChar_ = 0;
    urlScreens[currentUrlScreen_].invalidCharValue_.resize(urlScreens[currentUrlScreen_].numberOfInvalidChar_);
    
    colourInfo_ = 0x00;
    reshapeInfo_ = 0x00;
    urlScreens[currentUrlScreen_].screenData[0x7fd] = colourInfo_;
    
    switch (reshape_)
    {
        case COLOR_ON_PREVIOUS_PAGE:
            reshape_ = COLOR_NONE;
            break;
            
        case COLOR_RESHAPE:
        case COLOR_ON_THIS_PAGE:
        case COLOR_MULTIPLE:
            reshape_ = COLOR_ON_PREVIOUS_PAGE;
            break;
    }
    urlScreens[currentUrlScreen_].screenData[0x7fe] = reshape_ ;
    urlScreens[currentUrlScreen_].screenData[0x7ff] = reshapeInfo_;
    urlScreens[currentUrlScreen_].shapeLocation_ = 0;
    urlScreens[currentUrlScreen_].shapeNumber_ = 1;
    
    urlScreens[currentUrlScreen_].numberOfLinks = 0;
    urlScreens[currentUrlScreen_].numberOfRadios_ = 0;
    
    getText();
    
    while (currentUrlPos_ < 960)
        urlScreens[currentUrlScreen_].screenData[currentUrlPos_++] = 0;
    
    convertRemainingSpecialChar();
    
    currentUrlPos_ = 0;
    lastLink_ = 0;
    continueLink_ = false;
    currentUrlScreen_ = 1;
    
    free(rawUrlData);
}

void Usbcard::findHtmlCloseTag(wxString tag)
{
    bool found = false;
    while (!found && rawPos < rawUrlDataSize)
    {
        while ((rawPos+1) < rawUrlDataSize && !(rawUrlData[rawPos] == '<' && rawUrlData[rawPos+1] == '/'))
            rawPos++;
        
        getHtmlTag();
        
        //        if (tag.Left(1) == "v")
        //        {
        //            if ("/v" == htmlTag.element.Left(2))
        //                found = true;
        //        }
        //        else
        //        {
        if ("/"+tag == htmlTag.element)
            found = true;
        //        }
    }
}

void Usbcard::findHtmlTag(wxString tag)
{
    bool found = false;
    while (!found && rawPos < rawUrlDataSize)
    {
        while (rawUrlData[rawPos] != '<' && rawPos < rawUrlDataSize)
            rawPos++;
        
        getHtmlTag();
        
        if (tag == htmlTag.element)
            found = true;
    }
}

void Usbcard::getHtmlTag()
{
    if (rawPos < rawUrlDataSize)
    {
        rawPos++;
        
        htmlTag.attributeNumber = 0;
        htmlTag.attribute.resize(htmlTag.attributeNumber);
        htmlTag.attributeValue.resize(htmlTag.attributeNumber);
        
        htmlTag.element = getTag(&rawPos);
        htmlTag.element.MakeLower();
        
        if (htmlTag.element == "!--")
        {
            while ((rawPos +2 ) < rawUrlDataSize && !(rawUrlData[rawPos] == '-' && rawUrlData[rawPos+1] == '-' && rawUrlData[rawPos+2] == '>') )
                rawPos++;
            rawPos += 3;
        }
        else
        {
            while (rawUrlData[rawPos] != '>' && rawPos < rawUrlDataSize)
            {
                htmlTag.attributeNumber++;
                htmlTag.attribute.resize(htmlTag.attributeNumber);
                htmlTag.attributeValue.resize(htmlTag.attributeNumber);
                htmlTag.attribute[htmlTag.attributeNumber-1] = getTag(&rawPos);
                htmlTag.attribute[htmlTag.attributeNumber-1].MakeLower();
                htmlTag.attributeValue[htmlTag.attributeNumber-1] = getTagString();
            }
            rawPos++;
        }
    }
}

wxString Usbcard::getTag(size_t *pos)
{
    wxString returnString = "";
    
    while (*pos < rawUrlDataSize && rawUrlData[*pos] <= 0x20)
        *pos = *pos + 1;
    
    while (*pos < rawUrlDataSize && rawUrlData[*pos] != ' ' && rawUrlData[*pos] != '=' && rawUrlData[*pos] != '>' && rawUrlData[*pos] > 0x1f)
    {
        returnString.Append(rawUrlData[*pos]);
        *pos = *pos + 1;
    }
    
    if (returnString.Left(1) == '<')
        returnString = returnString.Right(returnString.Len()-1);
    
    return returnString;
}

wxString Usbcard::getTagString()
{
    wxString returnString = "";
    char identifier1 = ' ';
    Byte identifier2 = 0x0a;
    
    while (rawPos < rawUrlDataSize && rawUrlData[rawPos] <= 0x20)
        rawPos++;
    
    if (rawUrlData[rawPos] != '=')
        return "";
    
    rawPos++;
    
    while (rawPos < rawUrlDataSize && rawUrlData[rawPos] <= 0x20)
        rawPos++;
    
    if (rawUrlData[rawPos] == '\"')
    {
        rawPos++;
        identifier1 = '\"';
        identifier2 = 0;
    }
    else
    {
        if (rawUrlData[rawPos] == '\'')
        {
            rawPos++;
            identifier1 = '\'';
            identifier2 = 0;
        }
    }
    
    while (rawPos < rawUrlDataSize && rawUrlData[rawPos] != identifier1 && rawUrlData[rawPos] != identifier2 && rawUrlData[rawPos] != '>') // && rawUrlData[rawPos] > 0x1f)
    {
        if (rawUrlData[rawPos] > 0x1f)
            returnString.Append(rawUrlData[rawPos]);
        rawPos++;
    }
    
    if (rawUrlData[rawPos] == '\"' || rawUrlData[rawPos] == '\'')
        rawPos++;
    
    while (rawPos < rawUrlDataSize && rawUrlData[rawPos] <= 0x20)
        rawPos++;
    
    return returnString;
}

wxString Usbcard::getNextWord(size_t pos)
{
    wxString returnString = "";
    
    while (pos < rawUrlDataSize && rawUrlData[pos] != 9 && rawUrlData[pos] != ' ' && rawUrlData[pos] != '<' && rawUrlData[pos] != 0xa && rawUrlData[pos] != 0xd && rawUrlData[pos] <= 123)
    {
        returnString.Append(rawUrlData[pos]);
        pos++;
    }
    
    if (rawUrlData[pos] != '<')
        return returnString;
    
    Byte nextChar;
    if ((pos+1) < rawUrlDataSize)
        nextChar = rawUrlData[pos+1];
    else
        nextChar = 0;
    
    if (nextChar >= 65 || nextChar == '!' || nextChar == '/')
        return returnString;
    else
        return returnString+"<"+getNextWord(pos+1);
}

void Usbcard::writeText(wxString text, bool link, bool highColour, bool newline)
{
    if (tableTag_ != 0)
    {
        if (tdTag_)
        {
            text = replaceHtmlCharacter(text);
            urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].cellText += text;
        }
        if (captionTag_)
        {
            text = replaceHtmlCharacter(text);
            urlTable.caption.cellText += text;
        }
        return;
    }
    text = convertUtf8(text);
    text = shapeHtmlCharacter(text);
    
    if ((currentUrlLinePos_ + text.Len()) > 40)
    {
        writeLine();
        while (text.Len() >= 40)
        {
            wxString temp = text.Left(40);
            text = text.Right(text.Len() - 40);
            writeText(temp, link, highColour, newline);
            writeLine();
        }
        writeText(text, link, highColour, newline);
        return;
    }
    wxString tempText = text;
    
    if (currentUrlLinePos_ == 0 && !preTag_)
    {
        if (tempText.Len() > 0)
        {
            if (tempText.GetChar(tempText.Len()-1) > 31)
                tempText = tempText.Trim();
        }
    }
    
    tab();
    
    if (!(currentUrlLinePos_ == 0 && tempText.Len() == 0))
    {
        if (tempText.Len() == 1)
        {
            if (tempText.GetChar(0) != 160)
                emptyLines_ = 0;
        }
        else
            emptyLines_ = 0;
        
        textLine_ = textLine_ + text;
        
        if (link)
        {
            for (size_t i=0; i<text.Len(); i++)
            {
                if (currentUrlLinePos_ < 40 && text.GetChar(i) != 160)
                {
                    checkLinkNumber();
                    linkLine_[currentUrlLinePos_] = urlScreens[currentUrlScreen_].numberOfLinks;
                }
                currentUrlLinePos_++;
            }
        }
        else
        {
            if (highColour)
            {
                for (size_t i=0; i<text.Len(); i++)
                {
                    if (currentUrlLinePos_ < 40)
                        shapeLine_[currentUrlLinePos_] = 0x80;
                    currentUrlLinePos_++;
                }
            }
            else
                currentUrlLinePos_ += text.Len();
        }
    }
    if (newline)
        writeLine();
}

void Usbcard::writeInputText(wxString text, bool highColour, bool radioInput, int radioNumber, bool newline, bool breakline)
{
    if (tableTag_ != 0)
    {
        if (tdTag_)
        {
            text = replaceHtmlCharacter(text);
            urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].cellText += text;
        }
        if (captionTag_)
        {
            text = replaceHtmlCharacter(text);
            urlTable.caption.cellText += text;
        }
        return;
    }
    text = convertUtf8(text);
    text = shapeHtmlCharacter(text);
    
    if (breakline)
    {
        if ((currentUrlLinePos_ + text.Len()) > 40)
        {
            writeLine();
            while (text.Len() >= 40)
            {
                wxString temp = text.Left(40);
                text = text.Right(text.Len() - 40);
                writeInputText(temp, highColour, radioInput, radioNumber, newline, breakline);
                writeLine();
            }
            writeInputText(text, highColour, radioInput, radioNumber, newline, true);
            return;
        }
    }
    else
    {
        if ((currentUrlLinePos_ + text.Len())> 40)
        {
            while (text.Len() >= (size_t)(40 - currentUrlLinePos_))
            {
                wxString temp = text.Left(40 - currentUrlLinePos_);
                text = text.Right(text.Len() - 40 + currentUrlLinePos_);
                writeInputText(temp, highColour, radioInput, radioNumber, newline, breakline);
                writeLine();
            }
            writeInputText(text, highColour, radioInput, radioNumber, newline, true);
            return;
        }
    }
    
    wxString tempText = text;
    
    if (currentUrlLinePos_ == 0 && !preTag_)
    {
        if (tempText.Len() > 0)
        {
            if (tempText.GetChar(tempText.Len()-1) > 31)
                tempText = tempText.Trim();
        }
    }
    
    tab();
    
    if (!(currentUrlLinePos_ == 0 && tempText.Len() == 0))
    {
        if (tempText.Len() == 1)
        {
            if (tempText.GetChar(0) != 160)
                emptyLines_ = 0;
        }
        else
            emptyLines_ = 0;
        
        textLine_ = textLine_ + text;
        
        for (size_t i=0; i<text.Len(); i++)
        {
            if (radioInput)
            {
                if (currentUrlLinePos_ < 40)
                    linkLine_[currentUrlLinePos_] = radioNumber;
                if (lastLink_ < radioNumber)
                    lastLink_ = radioNumber;
            }
            else
            {
                if (currentUrlLinePos_ < 40)
                {
                    checkLinkNumber();
                    linkLine_[currentUrlLinePos_] = urlScreens[currentUrlScreen_].numberOfLinks;
                }
            }
            
            currentUrlLinePos_++;
        }
    }
    if (newline)
        writeLine();
}

void Usbcard::writeChar(wchar_t character, bool link, bool highColour)
{
    wxString tableaStr = " ";
    tableaStr.SetChar(0, TABLE_A);
    if (tableTag_ != 0)
    {
        if (tdTag_)
        {
            if (character == ' ')
            {
                if (urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].cellText.Len() == 0)
                    return;
                if (urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].cellText.Right(1) == " ")
                    return;
                if (urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].cellText.Right(1) == tableaStr && urlLink_ == 0x80)
                    return;
            }
            urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].cellText.Append(character);
        }
        if (captionTag_)
        {
            if (character == ' ')
            {
                if (urlTable.caption.cellText.Len() == 0)
                    return;
                if (urlTable.caption.cellText.Right(1) == " ")
                    return;
                if (urlTable.caption.cellText.Right(1) == tableaStr && urlLink_ == 0x80)
                    return;
            }
            urlTable.caption.cellText.Append(character);
        }
        return;
    }
    if (character == ' ' && !preTag_)
    {
        if (textLine_.Len() == 0)
            return;
        if (textLine_.Right(1) == " ")
            return;
        if (textLine_.Right(1) == tableaStr && urlLink_ == 0x80)
            return;
    }
    if ((currentUrlLinePos_ + 1) >= 40)
    {
        writeLine();
    }
    tab();
    if (!(currentUrlLinePos_ == 0 && (character == ' ' && !preTag_)))
    {
        emptyLines_ = 0;
        textLine_.Append(character);
        if (link)
        {
            checkLinkNumber();
            linkLine_[currentUrlLinePos_++] = urlScreens[currentUrlScreen_].numberOfLinks;
        }
        else
        {
            if (highColour)
            {
                shapeLine_[currentUrlLinePos_++] = 0x80;
            }
            else
                currentUrlLinePos_++;
        }
    }
}

void Usbcard::writeLine(bool trim)
{
    if (tableTag_ != 0)
        return;
    
    Byte charByte;
    
    size_t diff = 0;
    
    if (textLine_.Len() != 0)
    {
        if (centerTag_)
        {
            if (trim)
                textLine_.Trim();
            if (textLine_.Len() < 40)
                diff = (40 - textLine_.Len()) / 2;
        }
        
        if (linkLine_[0] == 0 && continueLink_)
        {
            urlScreens[currentUrlScreen_].numberOfLinks--;
        }
        continueLink_ = false;
        
        for (size_t i=0; i<40; i++)
        {
            if ((i+1)>diff && i<(textLine_.Len()+diff))
            {
                charByte = textLine_[i-diff].GetValue();
                int linkNumber = linkLine_[i-diff];
                
                if (charByte == 32)
                {
                    if (linkNumber != 0 && linkNumber <= urlScreens[currentUrlScreen_].numberOfLinks)
                    {
                        if (urlScreens[currentUrlScreen_].inputInfo[linkNumber-1].inputType != INPUT_TYPE_RADIO && urlScreens[currentUrlScreen_].inputInfo[linkNumber-1].inputType != INPUT_TYPE_CHECKBOX)
                            charByte = 0;
                    }
                }
                
                if (charByte >='a' && charByte <='z')
                    charByte -= 0x20;
                else
                {
                    if (charByte >='A' && charByte <='Z')
                        charByte += 0x20;
                }
                
                if (linkNumber != 0)
                {
                    urlScreens[currentUrlScreen_].screenData[currentUrlPos_] = (int)charByte | 0x80;
                    urlScreens[currentUrlScreen_].screenData[currentUrlPos_+0x400] = linkNumber;
                    
                    if (linkNumber <= urlScreens[currentUrlScreen_].numberOfLinks)
                    {
                        if (urlScreens[currentUrlScreen_].inputInfo[linkNumber-1].inputType != INPUT_TYPE_NONE && urlScreens[currentUrlScreen_].inputInfo[linkNumber-1].inputType != INPUT_TYPE_SUBMIT)
                            urlScreens[currentUrlScreen_].screenData[linkNumber+0x800] = urlScreens[currentUrlScreen_].inputInfo[linkNumber-1].inputType;
                    }
                }
                else
                    urlScreens[currentUrlScreen_].screenData[currentUrlPos_] = (int)charByte | shapeLine_[i-diff];
            }
            else
                urlScreens[currentUrlScreen_].screenData[currentUrlPos_] = 0;
            currentUrlPos_++;
        }
        
        checkUrlScreenPos();
        currentUrlLinePos_ = 0;
        
        textLine_ = "";
        for (size_t i=0; i<40; i++)
        {
            linkLine_[i] = 0;
            shapeLine_[i] = 0;
        }
    }
    currentUrlLinePos_ = 0;
}

wxString Usbcard::convertUtf8(wxString text)
{
    if (charSet_ != CHAR_SET_UTF_8)
        return text;
    
    int charValue;
    
    for (size_t i=0; i<text.Len(); i++)
    {
        charValue = text[i].GetValue();
        if ((text[i].GetValue()&0xe0) == 0xC0 && (i+1)<text.Len())
        {
            charValue = (text[i].GetValue()&0x1f)<<6;
            charValue = charValue + (text[i+1].GetValue()&0x3f);
            text.Remove(i+1,1);
        }
        
        if ((text[i].GetValue()&0xf0) == 0xE0 && (i+2)<text.Len())
        {
            charValue = (text[i].GetValue()&0x0f)<<12;
            charValue = charValue + ((text[i+1].GetValue()&0x3f)<<6);
            charValue = charValue + (text[i+2].GetValue()&0x3f);
            text.Remove(i+1,2);
        }
        
        if ((text[i].GetValue()&0xf1) == 0xf0 && (i+3)<text.Len())
        {
            charValue = (text[i].GetValue()&0x07)<<18;
            charValue = charValue + ((text[i+1].GetValue()&0x3f)<<12);
            charValue = charValue + ((text[i+2].GetValue()&0x3f)<<6);
            charValue = charValue + (text[i+3].GetValue()&0x3f);
            text.Remove(i+1,3);
        }
        if (charValue <= 0x80 || charValue == 160)
            text[i] = charValue;
        else
        {
            if (charValue > 160 && charValue < 256)
            {
                size_t codeNumber = charValue - HTML_START;
                if (urlScreens[currentUrlScreen_].shape_[codeNumber] == 0 && urlScreens[currentUrlScreen_].shapeNumber_ < HTML_FREE_SHAPE)
                {
                    urlScreens[currentUrlScreen_].shape_[codeNumber] = urlScreens[currentUrlScreen_].shapeNumber_;
                    urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = urlScreens[currentUrlScreen_].shapeNumber_;
                    urlScreens[currentUrlScreen_].shapeNumber_++;
                    for (int line=0; line<9; line++)
                        urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = shapeValues[codeNumber][line];
                }
                if (urlScreens[currentUrlScreen_].shape_[codeNumber] != 0)
                {
                    text[i] = urlScreens[currentUrlScreen_].shape_[codeNumber];
                }
                else
                {
                    text[i] = 31;
                    urlScreens[currentUrlScreen_].invalidCharFound_ = true;
                    urlScreens[currentUrlScreen_].numberOfInvalidChar_++;
                    urlScreens[currentUrlScreen_].invalidCharValue_.resize(urlScreens[currentUrlScreen_].numberOfInvalidChar_);
                    urlScreens[currentUrlScreen_].invalidCharValue_[urlScreens[currentUrlScreen_].numberOfInvalidChar_-1] = (int)codeNumber;
                }
            }
            else
            {
                if (charValue >= 256)
                {
                    wxString codeString;
                    codeString.Printf("&#%d;", charValue);
                    text = text.Left(i) + codeString + text.Right(text.Len()-i-1);
                }
                else
                    text[i] = 31;
            }
        }
    }
    return text;
}

void Usbcard::newLine()
{
    if (tableTag_ != 0)
    {
        writeChar((char)TABLE_CR, false, false);
        return;
    }
    
    if (emptyLines_ == 0 && currentUrlPos_ != 0)
    {
        for (size_t i=0; i<40; i++)
        {
            urlScreens[currentUrlScreen_].screenData[currentUrlPos_] = 32;
            currentUrlPos_++;
        }
        linkLine_[39] = 0;
        checkUrlScreenPos();
        
        emptyLines_++;
    }
}

void Usbcard::hr()
{
    writeLine();
    wxString line="&hr;";
    
    line = shapeHtmlCharacter(line);
    for (int i=0; i<40; i++)
    {
        if (i < 2 || i > 37)
            urlScreens[currentUrlScreen_].screenData[currentUrlPos_] = 32;
        else
        {
            urlScreens[currentUrlScreen_].screenData[currentUrlPos_] = line.GetChar(0);
        }
        currentUrlPos_++;
    }
    linkLine_[39] = 0;
    checkUrlScreenPos();
    
    newLine();
}

void Usbcard::checkUrlScreenPos()
{
    if (currentUrlPos_ == 960)
    {
        convertRemainingSpecialChar();
        currentUrlPos_ = 0;
        lastLink_ = 0;
        
        numberOfUrlScreens_++;
        currentUrlScreen_++;
        
        urlScreens.resize(numberOfUrlScreens_);
        urlScreens[currentUrlScreen_].numberOfRadios_ = 0;
        for (int i=960; i<0x7fd; i++)
            urlScreens[currentUrlScreen_].screenData[i]=0;
        for (int i=0x800; i<=0x900; i++)
            urlScreens[currentUrlScreen_].screenData[i]=0;
        for (int i=0; i< HTML_LAST; i++)
            urlScreens[currentUrlScreen_].shape_[i] = 0;
        urlScreens[currentUrlScreen_].screenData[0x7fd] = colourInfo_;
        urlScreens[currentUrlScreen_].screenData[0x7fe] = reshape_;
        urlScreens[currentUrlScreen_].screenData[0x7ff] = reshapeInfo_;
        urlScreens[currentUrlScreen_].shapeLocation_ = 0;
        urlScreens[currentUrlScreen_].shapeNumber_ = 1;
        
        urlScreens[currentUrlScreen_].invalidCharFound_ = false;
        urlScreens[currentUrlScreen_].textInputFound_ = false;
        urlScreens[currentUrlScreen_].numberOfInvalidChar_ = 0;
        urlScreens[currentUrlScreen_].invalidCharValue_.resize(urlScreens[currentUrlScreen_].numberOfInvalidChar_);
        
        if (linkLine_[39] != 0)
            continueLink_ = true;
        else
            continueLink_ = false;
        
        urlScreens[currentUrlScreen_].numberOfLinks = urlScreens[currentUrlScreen_-1].numberOfLinks;
        urlScreens[currentUrlScreen_].urlLinks.resize(urlScreens[currentUrlScreen_].numberOfLinks);
        urlScreens[currentUrlScreen_].inputInfo.resize(urlScreens[currentUrlScreen_].numberOfLinks);
        
        for (int linkNumber=0; linkNumber<urlScreens[currentUrlScreen_-1].numberOfLinks; linkNumber++)
        {
            urlScreens[currentUrlScreen_].urlLinks[linkNumber] = urlScreens[currentUrlScreen_-1].urlLinks[linkNumber];
            urlScreens[currentUrlScreen_].inputInfo[linkNumber].inputType = urlScreens[currentUrlScreen_-1].inputInfo[linkNumber].inputType;
            urlScreens[currentUrlScreen_].inputInfo[linkNumber].formNumber = urlScreens[currentUrlScreen_-1].inputInfo[linkNumber].formNumber;
        }
    }
}

void Usbcard::convertRemainingSpecialChar()
{
    if (!urlScreens[currentUrlScreen_].invalidCharFound_ || urlScreens[currentUrlScreen_].textInputFound_)
    {
        for (int i=0; i<960; i++)
        {
            if ((urlScreens[currentUrlScreen_].screenData[i] & 0x7f) == 31)
                urlScreens[currentUrlScreen_].screenData[i] = urlScreens[currentUrlScreen_].screenData[i] | 0x80;
        }
        return;
    }
    
    currentUrlPos_ = 0;
    bool charUsed[128];
    Byte charValue;
    Byte newCharacter;
    size_t invalidCharNumber = 0;
    
    for (int i=HTML_FREE_SHAPE; i<128; i++)
        charUsed[i] = false;
    
    charUsed[31] = true;
    
    for (int i=0; i<960; i++)
    {
        charValue = urlScreens[currentUrlScreen_].screenData[i] & 0x7f;
        charUsed[charValue] = true;
    }
    
    if (charUsed[URL_BOX] == true || charUsed[URL_TICK] == true)
    {
        charUsed[URL_BOX] = true;
        charUsed[URL_TICK] = true;
    }
    
    if (charUsed[URL_RADIO_OFF] == true || charUsed[URL_RADIO_ON] == true)
    {
        charUsed[URL_RADIO_OFF] = true;
        charUsed[URL_RADIO_ON] = true;
    }
    
    newCharacter = HTML_FREE_SHAPE;
    for (int i=0; i<960; i++)
    {
        charValue = urlScreens[currentUrlScreen_].screenData[i] & 0x7f;
        if (charValue == 31)
        {
            if (urlScreens[currentUrlScreen_].shape_[urlScreens[currentUrlScreen_].invalidCharValue_[invalidCharNumber]] == 0)
            {
                while (charUsed[newCharacter] && newCharacter < 127)
                    newCharacter++;
                
                if (newCharacter < 127)
                {
                    reshape_ = COLOR_RESHAPE;
                    urlScreens[currentUrlScreen_].screenData[0x7fe] = reshape_;
                    
                    charUsed[newCharacter] = true;
                    urlScreens[currentUrlScreen_].shape_[urlScreens[currentUrlScreen_].invalidCharValue_[invalidCharNumber]] = newCharacter;
                    urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = newCharacter;
                    for (int line=0; line<9; line++)
                        urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = shapeValues[urlScreens[currentUrlScreen_].invalidCharValue_[invalidCharNumber]][line];
                }
                if (urlScreens[currentUrlScreen_].shape_[urlScreens[currentUrlScreen_].invalidCharValue_[invalidCharNumber]] != 0)
                {
                    urlScreens[currentUrlScreen_].screenData[i] = (urlScreens[currentUrlScreen_].screenData[i] & 0x80) | newCharacter;
                }
                else
                    urlScreens[currentUrlScreen_].screenData[i] = urlScreens[currentUrlScreen_].screenData[i] | 0x80;
                
                invalidCharNumber++;
                if (invalidCharNumber == urlScreens[currentUrlScreen_].numberOfInvalidChar_)
                    i=960;
            }
        }
    }
}

void Usbcard::tab()
{
    if (liNum_ != 0 && currentUrlLinePos_ == 0)
    {
        for (int i=0; i<liNum_; i++)
        {
            textLine_ = textLine_ + " ";
            
            Byte lastUrlLink;
            if (currentUrlPos_ != 0)
                lastUrlLink = urlScreens[currentUrlScreen_].screenData[currentUrlPos_ - 1] & 0x80;
            else
                lastUrlLink = 0;
            
            if (urlLink_ == 0x80 && lastUrlLink == 0x80)
            {
                checkLinkNumber();
                linkLine_[currentUrlLinePos_++] = urlScreens[currentUrlScreen_].numberOfLinks;
            }
            else
            {
                if (shapeColour_ == 0x80)
                {
                    shapeLine_[currentUrlLinePos_++] = 0x80;
                }
                else
                    currentUrlLinePos_++;
            }
        }
    }
}

void Usbcard::getText()
{
    textLine_ = "";
    for (size_t i=0; i<40; i++)
    {
        linkLine_[i] = 0;
        shapeLine_[i] = 0;
    }
    wxString closeTag = "/";
    Byte nextChar;
    
    if (urlFragment_ != "")
    {
        nameTag_ = "";
        while (rawPos < rawUrlDataSize && nameTag_ != urlFragment_)
        {
            findHtmlTag("a");
            if (htmlTag.element == "a")
            {
                while (htmlTag.attributeNumber >= 1)
                {
                    if (htmlTag.attribute[htmlTag.attributeNumber-1] == "name")
                        nameTag_ = htmlTag.attributeValue[htmlTag.attributeNumber-1];
                    htmlTag.attributeNumber--;
                }
            }
        }
        if (nameTag_ != urlFragment_)
        {
            rawPos = 0;
            nameTag_ = "";
            while (rawPos < rawUrlDataSize && nameTag_ != urlFragment_)
            {
                if ((rawPos+1) < rawUrlDataSize)
                    nextChar = rawUrlData[rawPos+1];
                else
                    nextChar = 0;
                
                if (rawUrlData[rawPos] == '<' && (nextChar >= 65 || nextChar == '!' || nextChar == '?' || nextChar == '/'))
                {
                    getHtmlTag();
                    while (htmlTag.attributeNumber >= 1)
                    {
                        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "id")
                            nameTag_ = htmlTag.attributeValue[htmlTag.attributeNumber-1];
                        htmlTag.attributeNumber--;
                    }
                }
                else
                    rawPos++;
            }
            if (nameTag_ != urlFragment_)
                rawPos = 0;
        }
    }
    
    if (rawUrlData[0] == 0xef && rawUrlData[1] == 0xbb && rawUrlData[2] == 0xbf)
    {
        if (rawPos == 0)
            rawPos+=3;
        charSet_ = CHAR_SET_UTF_8;
    }
    
    while (rawPos < rawUrlDataSize)
    {
        if ((rawPos+1) < rawUrlDataSize)
            nextChar = rawUrlData[rawPos+1];
        else
            nextChar = 0;
        
        if (rawUrlData[rawPos] == '<' && (nextChar >= 65 || nextChar == '!' || nextChar == '?' || nextChar == '/'))
        {
            getHtmlTag();
            
            if (htmlTag.element.Len() != 0)
            {
                switch (htmlTag.element[0].GetValue())
                {
                    case 'a':
                        if (htmlTag.element == "a")
                        {
                            if (!(rawUrlData[rawPos] == '<' && rawUrlData[rawPos+1] == '/' && rawUrlData[rawPos+2] == 'a' && rawUrlData[rawPos+3] == '>'))
                                aTag();
                        }
                        break;
                        
                    case 'b':
                        if (htmlTag.element == "base")
                        {
                            while (htmlTag.attributeNumber >= 1)
                            {
                                if (htmlTag.attribute[htmlTag.attributeNumber-1] == "href")
                                    baseName_ = htmlTag.attributeValue[htmlTag.attributeNumber-1];
                                htmlTag.attributeNumber--;
                            }
                        }
                        
                        if (htmlTag.element == "br" || htmlTag.element == "br/")
                        {
                            if (tableTag_ !=0 )
                                writeChar((char)TABLE_CR, false, false);
                            else
                            {
                                textLine_ += " ";
                                writeLine();
                            }
                        }
                        
                        if (htmlTag.element == "blockquote")
                        {
                            newLine();
                            liNum_ = 5;
                            tab();
                        }
                        break;
                        
                    case 'c':
                        if (htmlTag.element == "caption")
                            captionTag_ = true;
                        
                        if (htmlTag.element == "center")
                            centerTag_ = true;
                        
                        if (htmlTag.element == "comx")
                            comxTag();
                        break;
                        
                    case 'd':
                        if (htmlTag.element == "del")
                            findHtmlCloseTag(htmlTag.element);
                        
                        if (htmlTag.element == "dir")
                        {
                            liNum_ += 3;
                            if (liNum_ > 30)
                                liNum_ = 30;
                            olTag_[liNum_] = false;
                            writeLine();
                        }
                        
                        if (htmlTag.element == "div")
                        {
                            writeLine();
                            newLine();
                        }
                        
                        if (htmlTag.element == "dl")
                            writeLine();
                        
                        if (htmlTag.element == "dt")
                            writeLine();
                        
                        if (htmlTag.element == "dd")
                        {
                            liNum_ = 3;
                            tab();
                        }
                        break;
                        
                    case 'e':
                        if (htmlTag.element == "embed")
                            showAttribute("src");
                        break;
                        
                    case 'f':
                        if (htmlTag.element == "form")
                            formTag();
                        break;
                        
                    case 'h':
                        if (htmlTag.element.Len() > 1) // hX
                        {
                            unsigned long number;
                            if (htmlTag.element.Mid(1,1).ToCULong(&number))
                            {
                                writeLine();
                                newLine();
                            }
                        }
                        
                        if (htmlTag.element == "hr")
                            hr();
                        break;
                        
                    case 'i':
                        if (htmlTag.element == "img")
                            imgTag();
                        
                        if (htmlTag.element == "input")
                            inputTag();
                        
                        if (htmlTag.element == "iframe")
                            iframeTag();
                        break;
                        
                    case 'l':
                        if (htmlTag.element == "li")
                            liTag();
                        break;
                        
                    case 'm':
                        if (htmlTag.element == "meta")
                        {
                            while (htmlTag.attributeNumber >= 1)
                            {
                                if (htmlTag.attribute[htmlTag.attributeNumber-1] == "charset")
                                {
                                    if ( htmlTag.attributeValue[htmlTag.attributeNumber-1] == "UTF-8")
                                        charSet_ = CHAR_SET_UTF_8;
                                }
                                if (htmlTag.attribute[htmlTag.attributeNumber-1] == "content")
                                {
                                    if ( htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("charset=UTF-8"))
                                        charSet_ = CHAR_SET_UTF_8;
                                }
                                htmlTag.attributeNumber--;
                            }
                        }
                        break;
                        
                    case 'o':
                        if (htmlTag.element == "object")
                            showAttribute("data");
                        
                        if (htmlTag.element == "ol")
                            olTag();
                        break;
                        
                    case 'p':
                        if (htmlTag.element == "p")
                        {
                            writeLine();
                            newLine();
                        }
                        
                        if (htmlTag.element == "pre")
                            preTag_ = true;
                        break;
                        
                    case 'q':
                        if (htmlTag.element == "q")
                            writeChar(0x22, urlLink_ == 0x80, shapeColour_ == 0x80);
                        break;
                        
                    case 's':
                        if (htmlTag.element == "s")
                            findHtmlCloseTag(htmlTag.element);
                        
                        if (htmlTag.element == "strike")
                            findHtmlCloseTag(htmlTag.element);
                        
                        if (htmlTag.element == "style")
                            findHtmlCloseTag(htmlTag.element);
                        
                        if (htmlTag.element == "script")
                            findHtmlCloseTag(htmlTag.element);
                        
                        if (htmlTag.element == "source")
                            sourceTag();
                        break;
                        
                    case 't':
                        if (htmlTag.element == "title")
                            findHtmlCloseTag(htmlTag.element);
                        
                        if (htmlTag.element == "table")
                        {
                            if (tableTag_ != 0)
                                tableTag_++;
                            else
                                startTable();
                        }
                        
                        if (htmlTag.element == "textarea")
                            preTag_ = true;
                        
                        if (htmlTag.element == "tr")
                        {
                            writeLine();
                            tdTag_ = false;
                            urlTable.numberOfRows++;
                            urlTable.column = 0;
                            urlTable.cellContent.push_back(urlTable.row);
                            urlTable.numberOfColumnsCurrentRow.resize(urlTable.numberOfRows);
                            urlTable.numberOfColumnsCurrentRow[urlTable.numberOfRows-1] = 0;
                        }
                        
                        if (htmlTag.element == "td" || htmlTag.element == "th")
                        {
                            if (urlTable.numberOfRows == 0)
                            {
                                urlTable.numberOfRows++;
                                urlTable.column = 0;
                                urlTable.cellContent.push_back(urlTable.row);
                                urlTable.numberOfColumnsCurrentRow.resize(urlTable.numberOfRows);
                                urlTable.numberOfColumnsCurrentRow[urlTable.numberOfRows-1] = 0;
                            }
                            regularCell();
                        }
                        
                        if (htmlTag.element == "tfoot")
                            urlTable.footStartRow = urlTable.numberOfRows + 1;
                        break;
                        
                    case 'u':
                        if (htmlTag.element == "ul")
                        {
                            liNum_ += 3;
                            if (liNum_ > 30)
                                liNum_ = 30;
                            olTag_[liNum_] = false;
                            writeLine();
                        }
                        break;
                        
                    case 'v':
                        if (htmlTag.element.Left(1) == "v")
                        {
                            if (htmlTag.attributeNumber == 0)
                                findHtmlCloseTag(htmlTag.element);
                            else
                            {
                                if (htmlTag.attribute[htmlTag.attributeNumber-1] != "/")
                                    findHtmlCloseTag(htmlTag.element);
                            }
                        }
                        break;
                        
                    case '?':
                        if (htmlTag.element == "?xml")
                        {
                            while (htmlTag.attributeNumber >= 1)
                            {
                                if (htmlTag.attribute[htmlTag.attributeNumber-1] == "encoding")
                                {
                                    if ( htmlTag.attributeValue[htmlTag.attributeNumber-1] == "UTF-8")
                                        charSet_ = CHAR_SET_UTF_8;
                                }
                                htmlTag.attributeNumber--;
                            }
                        }
                        break;
                        
                    default:
                        if (htmlTag.element == "/a" && hrefTag_)
                        {
                            if (tableTag_ != 0 && urlLink_ == 0x80)
                                writeChar(TABLE_A, false, false);
                            urlLink_ = 0;
                        }
                        
                        if (htmlTag.element == "/blockquote")
                        {
                            liNum_ = 0;
                            writeLine();
                            newLine();
                        }
                        
                        if (htmlTag.element == "/caption")
                            captionTag_ = false;
                        
                        if (htmlTag.element == "/center")
                        {
                            writeLine();
                            centerTag_ = false;
                        }
                        
                        if (htmlTag.element == "/dd")
                        {
                            liNum_ = 0;
                            writeLine();
                        }
                        
                        if (htmlTag.element == "/dl")
                            newLine();
                        
                        if (htmlTag.element.Len() > 2) // /hX
                        {
                            if (htmlTag.element.Left(2) == "/h")
                            {
                                unsigned long number;
                                if (htmlTag.element.Mid(2,1).ToCULong(&number))
                                {
                                    writeLine();
                                    newLine();
                                }
                            }
                        }
                        
                        if (htmlTag.element == "/li")
                            writeLine();
                        
                        if (htmlTag.element == "/ol")
                        {
                            if (olType_[liNum_] == TAG_OL_TYPEi || olType_[liNum_] == TAG_OL_TYPEI)
                                liNum_ -= 5;
                            else
                                liNum_ -= 3;
                            
                            if (liNum_ < 0)
                                liNum_ = 0;
                            if (liNum_ == 0)
                                newLine();
                        }
                        
                        if (htmlTag.element == "/p")
                            writeLine();
                        
                        if (htmlTag.element == "/pre")
                            preTag_ = false;
                        
                        if (htmlTag.element == "/q")
                            writeChar(0x22, urlLink_ == 0x80, shapeColour_ == 0x80);
                        
                        if (htmlTag.element == "/table")
                        {
                            tableTag_--;
                            
                            if (tableTag_ < 0)
                                tableTag_ = 0;
                            else
                            {
                                int keepTableTag = tableTag_;
                                tableTag_ = 0;
                                writeTable();
                                tableTag_ = keepTableTag;
                                
                                if (tableTag_ != 0)
                                {
                                    bool keepTdTag = tdTag_;
                                    tableTag_--;
                                    startTable();
                                    
                                    if (keepTdTag)
                                    {
                                        if (urlTable.numberOfRows == 0)
                                        {
                                            urlTable.numberOfRows++;
                                            urlTable.column = 0;
                                            urlTable.cellContent.push_back(urlTable.row);
                                            urlTable.numberOfColumnsCurrentRow.resize(urlTable.numberOfRows);
                                            urlTable.numberOfColumnsCurrentRow[urlTable.numberOfRows-1] = 0;
                                        }
                                        regularCell();
                                    }
                                }
                            }
                        }
                        
                        if (htmlTag.element == "/td")
                            tdTag_ = false;
                        
                        if (htmlTag.element == "/textarea")
                            preTag_ = false;
                        
                        if (htmlTag.element == "/tfoot")
                            urlTable.footEndRow = urlTable.numberOfRows;
                        
                        if (htmlTag.element == "/ul" || htmlTag.element == "/dir")
                        {
                            liNum_ -= 3;
                            if (liNum_ < 0)
                                liNum_ = 0;
                            if (liNum_ == 0)
                                newLine();
                        }
                        break;
                }
            }
        }
        else
        {
            if (rawUrlData[rawPos] == '<')
            {
                writeChar('<', urlLink_ == 0x80, shapeColour_ == 0x80);
                rawPos++;
            }
            
            else
            {
                while (rawPos < rawUrlDataSize && rawUrlData[rawPos] != '<')
                {
                    wxString nextWord = getNextWord(rawPos);
                    if (nextWord.Len() > 0)
                    {
                        writeText(nextWord, urlLink_ == 0x80, shapeColour_ == 0x80, false);
                        rawPos += nextWord.Len();
                    }
                    else
                    {
                        switch (rawUrlData[rawPos])
                        {
                            case 0xa:
                                writeChar(' ', urlLink_ == 0x80, shapeColour_ == 0x80);
                                while (rawUrlData[rawPos] == 0xd || rawUrlData[rawPos] == 0xa || (rawUrlData[rawPos] == 32 && !preTag_))
                                    rawPos++;
                                if (preTag_)
                                    writeLine();
                                break;
                                
                            case 0xd:
                                writeChar(' ', urlLink_ == 0x80, shapeColour_ == 0x80);
                                while (rawUrlData[rawPos] == 0xd || rawUrlData[rawPos] == 0xa || (rawUrlData[rawPos] == 32 && !preTag_))
                                    rawPos++;
                                if (preTag_)
                                    writeLine();
                                break;
                                
                            case 9:  // TAB
                                writeChar(' ', urlLink_ == 0x80, shapeColour_ == 0x80);
                                rawPos++;
                                break;
                                
                            case 32:
                                writeChar(' ', urlLink_ == 0x80, shapeColour_ == 0x80);
                                rawPos++;
                                break;
                                
                            case 0x91: //�
                            case 0x92: //`
                                writeChar(0x27, urlLink_ == 0x80, shapeColour_ == 0x80);
                                rawPos++;
                                break;
                                
                            case 0x93: //"
                            case 0x94: //"
                                writeChar('\"', urlLink_ == 0x80, shapeColour_ == 0x80);
                                rawPos++;
                                break;
                                
                            case 0x96: //"
                                writeChar('-', urlLink_ == 0x80, shapeColour_ == 0x80);
                                rawPos++;
                                break;
                                
                            case 169: //copy
                                writeChar(126, urlLink_ == 0x80, shapeColour_ == 0x80);
                                rawPos++;
                                break;
                                
                            default:
                                int charValue = rawUrlData[rawPos];
                                Byte shapeCol = shapeColour_;
                                if (charSet_ == CHAR_SET_UTF_8)
                                {
                                    if ((rawUrlData[rawPos]&0xe0) == 0xC0)
                                    {
                                        charValue = (rawUrlData[rawPos++]&0x1f)<<6;
                                        charValue = charValue + (rawUrlData[rawPos]&0x3f);
                                    }
                                    
                                    if ((rawUrlData[rawPos]&0xf0) == 0xE0)
                                    {
                                        charValue = (rawUrlData[rawPos++]&0x0f)<<12;
                                        charValue = charValue + ((rawUrlData[rawPos++]&0x3f)<<6);
                                        charValue = charValue + (rawUrlData[rawPos]&0x3f);
                                    }
                                    
                                    if ((rawUrlData[rawPos]&0xf1) == 0xf0)
                                    {
                                        charValue = (rawUrlData[rawPos++]&0x07)<<18;
                                        charValue = charValue + ((rawUrlData[rawPos++]&0x3f)<<12);
                                        charValue = charValue + ((rawUrlData[rawPos++]&0x3f)<<6);
                                        charValue = charValue + (rawUrlData[rawPos]&0x3f);
                                    }
                                }
                                if (tableTag_ != 0)
                                    writeChar(charValue, urlLink_ == 0x80, shapeCol == 0x80);
                                else
                                {
                                    if (charValue < 0x80 || charValue == 160)
                                        writeChar((Byte)charValue, urlLink_ ==  0x80, shapeCol == 0x80);
                                    else
                                    {
                                        if (charValue > 160 && charValue < 256)
                                        {
                                            size_t codeNumber = charValue - HTML_START;
                                            if (urlScreens[currentUrlScreen_].shape_[codeNumber] == 0 && urlScreens[currentUrlScreen_].shapeNumber_ < HTML_FREE_SHAPE)
                                            {
                                                urlScreens[currentUrlScreen_].shape_[codeNumber] = urlScreens[currentUrlScreen_].shapeNumber_;
                                                urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = urlScreens[currentUrlScreen_].shapeNumber_;
                                                urlScreens[currentUrlScreen_].shapeNumber_++;
                                                for (int i=0; i<9; i++)
                                                    urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = shapeValues[codeNumber][i];
                                            }
                                            if (urlScreens[currentUrlScreen_].shape_[codeNumber] != 0)
                                            {
                                                writeChar((wchar_t)urlScreens[currentUrlScreen_].shape_[(int)codeNumber], urlLink_ ==  0x80, shapeCol == 0x80);
                                            }
                                            else
                                            {
                                                writeChar(31, urlLink_ ==  0x80, shapeCol == 0x80);
                                                urlScreens[currentUrlScreen_].invalidCharFound_ = true;
                                                urlScreens[currentUrlScreen_].numberOfInvalidChar_++;
                                                urlScreens[currentUrlScreen_].invalidCharValue_.resize(urlScreens[currentUrlScreen_].numberOfInvalidChar_);
                                                urlScreens[currentUrlScreen_].invalidCharValue_[urlScreens[currentUrlScreen_].numberOfInvalidChar_-1] = (int)codeNumber;
                                            }
                                        }
                                        else
                                        {
                                            if (charValue >= 256)
                                            {
                                                wxString codeString;
                                                codeString.Printf("&#%d;", charValue);
                                                writeText(codeString, urlLink_ ==  0x80, shapeCol == 0x80, false);
                                            }
                                            else
                                                writeChar(31, urlLink_ ==  0x80, shapeCol == 0x80);
                                        }
                                    }
                                }
                                rawPos++;
                                break;
                        }
                    }
                }
            }
            if (htmlTag.element == "li")
                writeLine();
        }
    }
    if (tableTag_ != 0)
    {
        tableTag_ = 0;
        writeTable();
    }
    writeLine();
}

void Usbcard::startTable()
{
    urlTable.column = 0;
    
    urlTable.numberOfColumns = 0;
    urlTable.numberOfRows = 0;
    
    urlTable.columnWidth.resize(0);
    urlTable.numberOfColumnsCurrentRow.resize(urlTable.numberOfRows);
    
    urlTable.caption.cellText = "";
    urlTable.caption.currentLink = 0;
    urlTable.caption.screenLinkNumber = 0;
    urlTable.caption.numberOfLinks = 0;
    urlTable.footEndRow = 0;
    urlTable.footStartRow = 0;
    
    tableTag_++;
    tdTag_ = false;
    captionTag_ = false;
}

void Usbcard::writeTable()
{
    adjustColumnWidth();
    
    writeLine();
    
    wxString tempStr;
    bool continueLoop = true;
    
    if (urlTable.caption.cellText != "")
    {
        Byte urlLinkCaption = 0, shapeColourCaption = 0;
        size_t remove = 0;
        
        while (continueLoop)
        {
            continueLoop = false;
            if (urlTable.caption.cellText.Len() > 0)
            {
                if (urlTable.caption.cellText.GetChar(0) == TABLE_CR)
                    urlTable.caption.cellText = urlTable.caption.cellText.Right(urlTable.caption.cellText.Len()-1);
            }
            remove = 0;
            writeFirstCellLine(0, 0, urlTable.tableWidth, &remove, &urlLinkCaption, &shapeColourCaption, true, true);
            urlTable.caption.cellText = urlTable.caption.cellText.Right(urlTable.caption.cellText.Len()-remove);
            
            if  (urlTable.caption.cellText.Len() > 0)
                continueLoop = true;
            
            writeLine(false);
        }
    }
    
    if (urlTable.footStartRow != 0)
    {
        for (size_t row=0; row<urlTable.numberOfRows; row++)
        {
            if (row < (urlTable.footStartRow-1) || row > (urlTable.footEndRow-1))
                writeRow(row);
        }
        for (size_t row=urlTable.footStartRow-1; row<=(urlTable.footEndRow-1); row++)
            writeRow(row);
    }
    else
    {
        for (size_t row=0; row<urlTable.numberOfRows; row++)
            writeRow(row);
    }
}

void Usbcard::writeRow(size_t row)
{
    wxString tempStr;
    bool continueLoop = true;
    bool doNewLine = false;
    
    for (size_t col=0; col<urlTable.numberOfColumnsCurrentRow[row]; col++)
    {
        urlTable.urlLinkCell[col] = 0;
        urlTable.shapeColourCell[col] = 0;
    }
    while (continueLoop)
    {
        continueLoop = false;
        for (size_t col=0; col<urlTable.numberOfColumnsCurrentRow[row]; col++)
        {
            doNewLine = false;
            if (urlTable.cellContent[row][col].cellText.Len() > 0)
            {
                if (urlTable.cellContent[row][col].cellText.GetChar(0) == TABLE_CR && col == 0 && urlTable.numberOfColumnsCurrentRow[row] == 1)
                {
                    doNewLine = true;
                    urlTable.cellContent[row][col].cellText = urlTable.cellContent[row][col].cellText.Right(urlTable.cellContent[row][col].cellText.Len()-1);
                }
            }
            size_t remove = 0;
            if (urlTable.numberOfColumnsCurrentRow[row] == 1)
                writeFirstCellLine(row, col, urlTable.tableWidth, &remove, &urlTable.urlLinkCell[col], &urlTable.shapeColourCell[col], (col+1) == urlTable.numberOfColumnsCurrentRow[row], false);
            else
                writeFirstCellLine(row, col, urlTable.columnWidth[col], &remove, &urlTable.urlLinkCell[col], &urlTable.shapeColourCell[col], (col+1) == urlTable.numberOfColumnsCurrentRow[row], false);
            urlTable.cellContent[row][col].cellText = urlTable.cellContent[row][col].cellText.Right(urlTable.cellContent[row][col].cellText.Len()-remove);
            
            if  (urlTable.cellContent[row][col].cellText.Len() > 0)
                continueLoop = true;
        }
        tempStr = textLine_;
        tempStr.Trim();
        if (tempStr == "")
            textLine_ = "";
        writeLine(false);
    }
    if (doNewLine)
        newLine();
}

void Usbcard::adjustColumnWidth()
{
    int tableWidth = 0, columns =0;
    wxString tempString, tableaStr = " ", highColorStr = " ";
    
    tableaStr.SetChar(0, TABLE_A);
    highColorStr.SetChar(0, TABLE_HIGH_COLOUR);
    
    for (size_t row=0; row<urlTable.numberOfRows; row++)
    {
        if (urlTable.numberOfColumnsCurrentRow[row] != 1)
        {
            for (size_t col=0; col<urlTable.numberOfColumnsCurrentRow[row]; col++)
            {
                tempString = urlTable.cellContent[row][col].cellText;
                tempString.Replace(tableaStr, "");
                tempString.Replace(highColorStr, "");
                
                if (tempString.Len() > urlTable.columnWidth[col])
                    urlTable.columnWidth[col] = tempString.Len();
            }
        }
    }
    
    urlTable.tableWidth = 0;
    
    if (urlTable.numberOfColumns > 0)
    {
        for (size_t i=0; i<urlTable.numberOfColumns; i++)
            urlTable.tableWidth += (urlTable.columnWidth[i]+1);
        
        columns = (int)urlTable.numberOfColumns;
    }
    
    tableWidth = (int)urlTable.tableWidth;
    
    if (urlTable.tableWidth > 40)
        urlTable.tableWidth = 40;
    
    if (urlTable.tableWidth <= 1)
    {
        urlTable.tableWidth = 40;
        tableWidth = (int)urlTable.tableWidth;
    }
    
    urlTable.urlLinkCell.resize(columns);
    urlTable.shapeColourCell.resize(columns);
    
    for (int i=0; i<columns; i++)
    {
        urlTable.urlLinkCell[i] = 0;
        urlTable.shapeColourCell[i] = 0;
    }
    
    tableWidth -= 41;
    size_t tableCut = tableWidth;
    int finalSub = 0;
    int sub;
    bool subtract;
    int col = 0;
    
    float columnSizePercent;
    
    while (tableWidth > 0)
    {
        if (finalSub == 1)
            sub = 1;
        else
        {
            columnSizePercent = (float)urlTable.columnWidth[col] / (float)(tableCut + 41);
            sub = (float)tableCut * columnSizePercent;
        }
        
        if (tableWidth < sub)
            sub = tableWidth;
        
        subtract = false;
        if (col < (int)urlTable.numberOfColumns)
        {
            if ((urlTable.columnWidth[col]-sub) > 0)
            {
                urlTable.columnWidth[col]-=sub;
                subtract = true;
            }
        }
        col++;
        if (subtract)
            tableWidth-=sub;
        if (col >= columns)
        {
            col = 0;
            finalSub = 1;
        }
    }
}

wxString Usbcard::shapeHtmlCharacter(wxString str)
{
    wxString comxChar = " ", codeChar = " ", invalidChar = " ";
    comxChar.SetChar(0, 32+128);
    str.Replace("&nbsp;",comxChar);
    str.Replace("&ensp;",comxChar);
    str.Replace("&emsp;",comxChar);
    str.Replace("&thinsp;",comxChar);
    str.Replace("&#160;",comxChar);
    str.Replace("&amp;","&");
    str.Replace("&quot;","\"");
    str.Replace("&gt;",">");
    str.Replace("&lt;","<");
    invalidChar[0]=31;
    wxString codeString;
    
    if (str.Find("&") != wxNOT_FOUND)
    {
        for (size_t codeNumber = 0; codeNumber<HTML_LAST; codeNumber++)
        {
            if (str.Find(htmlCodes[codeNumber]) != wxNOT_FOUND)
            {
                if (urlScreens[currentUrlScreen_].shape_[codeNumber] == 0 && urlScreens[currentUrlScreen_].shapeNumber_ < HTML_FREE_SHAPE)
                {
                    urlScreens[currentUrlScreen_].shape_[codeNumber] = urlScreens[currentUrlScreen_].shapeNumber_;
                    if (codeNumber >= HTML_SHAPE)
                        urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = urlScreens[currentUrlScreen_].shapeNumber_ | 0x80;
                    else
                        urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = urlScreens[currentUrlScreen_].shapeNumber_;
                    urlScreens[currentUrlScreen_].shapeNumber_++;
                    for (int i=0; i<9; i++)
                        urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = shapeValues[codeNumber][i];
                }
                if (urlScreens[currentUrlScreen_].shape_[codeNumber] != 0)
                {
                    comxChar.SetChar(0, urlScreens[currentUrlScreen_].shape_[codeNumber]);
                    str.Replace(htmlCodes[codeNumber],comxChar);
                }
                else
                {
                    str.Replace(htmlCodes[codeNumber],invalidChar);
                    urlScreens[currentUrlScreen_].invalidCharFound_  = true;
                    urlScreens[currentUrlScreen_].numberOfInvalidChar_++;
                    urlScreens[currentUrlScreen_].invalidCharValue_.resize(urlScreens[currentUrlScreen_].numberOfInvalidChar_);
                    urlScreens[currentUrlScreen_].invalidCharValue_[urlScreens[currentUrlScreen_].numberOfInvalidChar_-1] = (int)codeNumber;
                }
            }
        }
    }
    
    for (size_t codeNumber = 0; codeNumber<95; codeNumber++)
    {
        codeChar.SetChar(0, codeNumber+HTML_START);
        if (str.Find(codeChar) != wxNOT_FOUND)
        {
            if (urlScreens[currentUrlScreen_].shape_[codeNumber] == 0 && urlScreens[currentUrlScreen_].shapeNumber_ < HTML_FREE_SHAPE)
            {
                urlScreens[currentUrlScreen_].shape_[codeNumber] = urlScreens[currentUrlScreen_].shapeNumber_;
                if (codeNumber >= HTML_SHAPE)
                    urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = urlScreens[currentUrlScreen_].shapeNumber_ | 0x80;
                else
                    urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = urlScreens[currentUrlScreen_].shapeNumber_;
                urlScreens[currentUrlScreen_].shapeNumber_++;
                for (int i=0; i<9; i++)
                    urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = shapeValues[codeNumber][i];
            }
            if (urlScreens[currentUrlScreen_].shape_[codeNumber] != 0)
            {
                comxChar.SetChar(0, urlScreens[currentUrlScreen_].shape_[codeNumber]);
                str.Replace(codeChar,comxChar);
            }
            else
            {
                str.Replace(codeChar,invalidChar);
                urlScreens[currentUrlScreen_].invalidCharFound_  = true;
                urlScreens[currentUrlScreen_].numberOfInvalidChar_++;
                urlScreens[currentUrlScreen_].invalidCharValue_.resize(urlScreens[currentUrlScreen_].numberOfInvalidChar_);
                urlScreens[currentUrlScreen_].invalidCharValue_[urlScreens[currentUrlScreen_].numberOfInvalidChar_-1] = (int)codeNumber;
            }
        }
    }
    
    if (str.Find("&#") != wxNOT_FOUND)
    {
        for (size_t codeNumber = 0; codeNumber<HTML_SHAPE; codeNumber++)
        {
            codeString.Printf("&#%d;", (unsigned char)codeNumber+HTML_START);
            if (str.Find(codeString) != wxNOT_FOUND)
            {
                if (urlScreens[currentUrlScreen_].shape_[codeNumber] == 0 && urlScreens[currentUrlScreen_].shapeNumber_ < HTML_FREE_SHAPE)
                {
                    urlScreens[currentUrlScreen_].shape_[codeNumber] = urlScreens[currentUrlScreen_].shapeNumber_;
                    if (codeNumber >= HTML_SHAPE)
                        urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = urlScreens[currentUrlScreen_].shapeNumber_ | 0x80;
                    else
                        urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = urlScreens[currentUrlScreen_].shapeNumber_;
                    urlScreens[currentUrlScreen_].shapeNumber_++;
                    for (int i=0; i<9; i++)
                        urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = shapeValues[codeNumber][i];
                }
                if (urlScreens[currentUrlScreen_].shape_[codeNumber] != 0)
                {
                    comxChar.SetChar(0, urlScreens[currentUrlScreen_].shape_[codeNumber]);
                    str.Replace(codeString,comxChar);
                }
                else
                {
                    str.Replace(codeString,invalidChar);
                    urlScreens[currentUrlScreen_].invalidCharFound_  = true;
                    urlScreens[currentUrlScreen_].numberOfInvalidChar_++;
                    urlScreens[currentUrlScreen_].invalidCharValue_.resize(urlScreens[currentUrlScreen_].numberOfInvalidChar_);
                    urlScreens[currentUrlScreen_].invalidCharValue_[urlScreens[currentUrlScreen_].numberOfInvalidChar_-1] = (int)codeNumber;
                }
            }
        }
        
        for (size_t codeNumber = 0; codeNumber<HTML_CODE_LAST; codeNumber++)
        {
            if (str.Find(htmlNumbers[codeNumber]) != wxNOT_FOUND)
            {
                if (urlScreens[currentUrlScreen_].shape_[codeNumber+HTML_UTF] == 0 && urlScreens[currentUrlScreen_].shapeNumber_ < HTML_FREE_SHAPE)
                {
                    urlScreens[currentUrlScreen_].shape_[codeNumber+HTML_UTF] = urlScreens[currentUrlScreen_].shapeNumber_;
                    urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = urlScreens[currentUrlScreen_].shapeNumber_;
                    urlScreens[currentUrlScreen_].shapeNumber_++;
                    for (int i=0; i<9; i++)
                        urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = shapeValues[codeNumber+HTML_UTF][i];
                }
                if (urlScreens[currentUrlScreen_].shape_[codeNumber+HTML_UTF] != 0)
                {
                    comxChar.SetChar(0, urlScreens[currentUrlScreen_].shape_[codeNumber+HTML_UTF]);
                    str.Replace(htmlNumbers[codeNumber],comxChar);
                }
                else
                {
                    str.Replace(htmlNumbers[codeNumber],invalidChar);
                    urlScreens[currentUrlScreen_].invalidCharFound_  = true;
                    urlScreens[currentUrlScreen_].numberOfInvalidChar_++;
                    urlScreens[currentUrlScreen_].invalidCharValue_.resize(urlScreens[currentUrlScreen_].numberOfInvalidChar_);
                    urlScreens[currentUrlScreen_].invalidCharValue_[urlScreens[currentUrlScreen_].numberOfInvalidChar_-1] = (int)codeNumber+HTML_UTF;
                }
            }
        }
        
        wxString temp = str;
        while (temp.Find("&#") != wxNOT_FOUND)
        {
            wxString number = "";
            size_t start = temp.Find("&#") + 2;
            while (start < temp.Len() && temp.GetChar(start) != ';')
            {
                number += temp.GetChar(start);
                start++;
            }
            
            if ((start+1) >= temp.Len())
                temp = "";
            else
                temp = temp.Right(temp.Len()-start-1);
            
            long numberLong;
            
            if (number.ToLong(&numberLong))
            {
                codeChar.SetChar(0,(char)numberLong);
                str.Replace("&#"+number+";",codeChar);
            }
        }
    }
    return str;
}

wxString Usbcard::replaceHtmlCharacter(wxString str)
{
    wxString comxChar = " ";
    comxChar.SetChar(0, 32+128);
    str.Replace("&nbsp;",comxChar);
    str.Replace("&ensp;",comxChar);
    str.Replace("&emsp;",comxChar);
    str.Replace("&thinsp;",comxChar);
    str.Replace("&#160;",comxChar);
    str.Replace("&amp;","&");
    str.Replace("&quot;","\"");
    str.Replace("&gt;",">");
    str.Replace("&lt;","<");
    wxString codeString;
    
    for (size_t codeNumber = 0; codeNumber<95; codeNumber++)
    {
        if (htmlCodes[codeNumber].Len() > 0)
        {
            if (str.Find(htmlCodes[codeNumber]) != wxNOT_FOUND)
            {
                comxChar.SetChar(0, codeNumber+HTML_START);
                str.Replace(htmlCodes[codeNumber],comxChar);
            }
            codeString.Printf("&#%d;", (unsigned char)codeNumber+HTML_START);
            if (str.Find(codeString) != wxNOT_FOUND)
            {
                comxChar.SetChar(0, codeNumber+HTML_START);
                str.Replace(codeString,comxChar);
            }
        }
    }
    
    return str;
}

void Usbcard::writeFirstCellLine(size_t row, size_t col, size_t cellWidth, size_t *remove, Byte *urlLink, Byte *shapeColour, bool lastCol, bool caption)
{
    wxString tempString, tempString2, str;
    size_t orgCellWidth, padding = 0;
    bool continueLoop = true;
    
    if (caption)
        str = urlTable.caption.cellText;
    else
        str = urlTable.cellContent[row][col].cellText;
    
    orgCellWidth = cellWidth;
    
    while (continueLoop)
    {
        if (str.Len() > 0)
        {
            if (str.GetChar(0) == ' ' || str.GetChar(0) == (char)TABLE_CR)
            {
                str = str.Right(str.Len()-1);
                padding++;
            }
            else
                continueLoop = false;
        }
        else
            continueLoop = false;
    }
    
    if (str.Find((char)TABLE_CR) != wxNOT_FOUND)
    {
        tempString = str.BeforeFirst((char)TABLE_CR);
        str = tempString;
        
        //emptyLines_ = 0;
    }
    
    tempString = str;
    
    if (tempString.Len()>0)
    {
        tempString = str.Left(cellWidth);
        while (tempString.Find((char)TABLE_A) != wxNOT_FOUND)
        {
            tempString2 = tempString.BeforeFirst((char)TABLE_A);
            cellWidth++;
            tempString = tempString2 + str.Right(str.Len()-tempString2.Len()-cellWidth+orgCellWidth);
            tempString = tempString.Left(orgCellWidth);
        }
        while (tempString.Find((char)TABLE_HIGH_COLOUR) != wxNOT_FOUND)
        {
            tempString2 = tempString.BeforeFirst((char)TABLE_HIGH_COLOUR);
            cellWidth++;
            tempString = tempString2 + str.Right(str.Len()-tempString2.Len()-cellWidth+orgCellWidth);
            tempString = tempString.Left(orgCellWidth);
        }
    }
    
    if (str.Len()>cellWidth)
    {
        tempString = str.Left(cellWidth);
        if (str.GetChar(cellWidth-1) != ' ' && str.GetChar(cellWidth) != ' ')
        {
            if (tempString.Find(' ') != wxNOT_FOUND)
                str = tempString.BeforeLast(' ');
            else
                str = tempString;
        }
        else
            str = tempString;
    }
    
    *remove = padding + str.Len();
    
    urlLink_ = *urlLink;
    shapeColour_ = *shapeColour;
    size_t stringLen = 0;
    
    int linkNumber, linkOffSet;
    if (str.Len()>0)
    {
        while (!(str.Find((char)TABLE_A) == wxNOT_FOUND && str.Find((char)TABLE_HIGH_COLOUR) == wxNOT_FOUND))
        {
            tempString = str.BeforeFirst((char)TABLE_A);
            tempString2 = str.BeforeFirst((char)TABLE_HIGH_COLOUR);
            
            if (tempString.Len() < tempString2.Len())
            {
                stringLen += tempString.Len();
                
                linkNumber = urlScreens[currentUrlScreen_].numberOfLinks;
                linkOffSet = 0;
                if (*urlLink == 0x80)
                {
                    if (caption)
                    {
                        urlScreens[currentUrlScreen_].numberOfLinks = urlTable.caption.screenLinkNumber;
                        linkOffSet = checkLinkNumber();
                        urlTable.caption.screenLinkNumber = urlScreens[currentUrlScreen_].numberOfLinks;
                    }
                    else
                    {
                        urlScreens[currentUrlScreen_].numberOfLinks = urlTable.cellContent[row][col].screenLinkNumber;
                        linkOffSet = checkLinkNumber();
                        urlTable.cellContent[row][col].screenLinkNumber = urlScreens[currentUrlScreen_].numberOfLinks;
                    }
                }
                writeText(tempString, *urlLink == 0x80, *shapeColour == 0x80, false);
                if (linkOffSet > linkNumber)
                    linkOffSet = 0;
                urlScreens[currentUrlScreen_].numberOfLinks = linkNumber - linkOffSet;
                
                *urlLink = *urlLink ^ 0x80;
                urlLink_ = *urlLink;
                str = str.Right(str.Len()-tempString.Len()-1);
                
                if (urlLink_ == 0x80)
                {
                    urlScreens[currentUrlScreen_].numberOfLinks++;
                    urlScreens[currentUrlScreen_].urlLinks.resize(urlScreens[currentUrlScreen_].numberOfLinks);    
                    urlScreens[currentUrlScreen_].inputInfo.resize(urlScreens[currentUrlScreen_].numberOfLinks);    
                    
                    if (caption)
                    {
                        urlScreens[currentUrlScreen_].urlLinks[urlScreens[currentUrlScreen_].numberOfLinks - 1] = urlTable.caption.urlLinks[urlTable.caption.currentLink];    
                        urlScreens[currentUrlScreen_].inputInfo[urlScreens[currentUrlScreen_].numberOfLinks - 1].inputType = urlTable.caption.inputInfo[urlTable.caption.currentLink].inputType;    
                        urlScreens[currentUrlScreen_].inputInfo[urlScreens[currentUrlScreen_].numberOfLinks - 1].formNumber = urlTable.caption.inputInfo[urlTable.caption.currentLink].formNumber;    
                        urlTable.caption.currentLink++;
                        urlTable.caption.screenLinkNumber = urlScreens[currentUrlScreen_].numberOfLinks;
                    }
                    else
                    {
                        urlScreens[currentUrlScreen_].urlLinks[urlScreens[currentUrlScreen_].numberOfLinks - 1] = urlTable.cellContent[row][col].urlLinks[urlTable.cellContent[row][col].currentLink];    
                        urlScreens[currentUrlScreen_].inputInfo[urlScreens[currentUrlScreen_].numberOfLinks - 1].inputType = urlTable.cellContent[row][col].inputInfo[urlTable.cellContent[row][col].currentLink].inputType;    
                        urlScreens[currentUrlScreen_].inputInfo[urlScreens[currentUrlScreen_].numberOfLinks - 1].formNumber = urlTable.cellContent[row][col].inputInfo[urlTable.cellContent[row][col].currentLink].formNumber;    
                        urlTable.cellContent[row][col].currentLink++;
                        urlTable.cellContent[row][col].screenLinkNumber = urlScreens[currentUrlScreen_].numberOfLinks;
                    }
                }
            }
            else
            {
                stringLen += tempString2.Len();
                
                linkNumber = urlScreens[currentUrlScreen_].numberOfLinks;
                linkOffSet = 0;
                if (*urlLink == 0x80)
                {
                    if (caption)
                    {
                        urlScreens[currentUrlScreen_].numberOfLinks = urlTable.caption.screenLinkNumber;
                        linkOffSet = checkLinkNumber();
                        urlTable.caption.screenLinkNumber = urlScreens[currentUrlScreen_].numberOfLinks;
                    }
                    else
                    {
                        urlScreens[currentUrlScreen_].numberOfLinks = urlTable.cellContent[row][col].screenLinkNumber;
                        linkOffSet = checkLinkNumber();
                        urlTable.cellContent[row][col].screenLinkNumber = urlScreens[currentUrlScreen_].numberOfLinks;
                    }
                }
                writeText(tempString2, *urlLink == 0x80, *shapeColour == 0x80, false);
                if (linkOffSet > linkNumber)
                    linkOffSet = 0;
                urlScreens[currentUrlScreen_].numberOfLinks = linkNumber - linkOffSet;
                
                *shapeColour = *shapeColour ^ 0x80;
                shapeColour_ = *shapeColour;
                str = str.Right(str.Len()-tempString2.Len()-1);
            }
        }
    }
    str = str.Trim();
    stringLen += str.Len();
    wxString comxChar = " ";
    comxChar.SetChar(0, 160);

    if (stringLen != 0)
    {
        for (size_t i = stringLen; i < orgCellWidth; i++)
        {
            str = str + comxChar;
        }
    }
    
    linkNumber = urlScreens[currentUrlScreen_].numberOfLinks;
    linkOffSet = 0;
    if (*urlLink == 0x80)
    {
        if (caption)
        {
            urlScreens[currentUrlScreen_].numberOfLinks = urlTable.caption.screenLinkNumber;
            linkOffSet = checkLinkNumber();
            urlTable.caption.screenLinkNumber = urlScreens[currentUrlScreen_].numberOfLinks;
        }
        else
        {
            urlScreens[currentUrlScreen_].numberOfLinks = urlTable.cellContent[row][col].screenLinkNumber;
            linkOffSet = checkLinkNumber();
            urlTable.cellContent[row][col].screenLinkNumber = urlScreens[currentUrlScreen_].numberOfLinks;
        }
    }
    writeText(str, *urlLink == 0x80, *shapeColour == 0x80, false);
    if (linkOffSet > linkNumber)
        linkOffSet = 0;
    urlScreens[currentUrlScreen_].numberOfLinks = linkNumber - linkOffSet;
    
    if (!lastCol)
        writeText(comxChar, false, false, false);
}

void Usbcard::regularCell()
{
    tdTag_ = true;
    urlTable.column++;
    
    if (urlTable.column > urlTable.numberOfColumns)
        urlTable.numberOfColumns = urlTable.column;
    
    if (urlTable.column > urlTable.numberOfColumnsCurrentRow[urlTable.numberOfRows-1])
        urlTable.numberOfColumnsCurrentRow[urlTable.numberOfRows-1] = urlTable.column;
    
    urlTable.cellContent[urlTable.numberOfRows-1].resize(urlTable.column);
    
    urlTable.cellContent[urlTable.numberOfRows-1][urlTable.column-1].cellText = "";
    urlTable.cellContent[urlTable.numberOfRows-1][urlTable.column-1].numberOfLinks = 0;
    urlTable.cellContent[urlTable.numberOfRows-1][urlTable.column-1].currentLink = 0;
    urlTable.cellContent[urlTable.numberOfRows-1][urlTable.column-1].screenLinkNumber = 0;
    
    if (urlTable.column > urlTable.columnWidth.size())
    {
        urlTable.columnWidth.resize(urlTable.column);
        urlTable.columnWidth[urlTable.column - 1] = 0; 
    }
}

void Usbcard::getHelp()
{
    Byte charByte;
    networkError_ = USB_FILE_EXISTS;
    urlLink_ = 0;
    shapeColour_ = 0;
    
    numberOfUrlScreens_ = 3;
    urlScreens.resize(numberOfUrlScreens_);
    // 1234567890123456789012345678901234567890
    wxString helpText1= "USB BROWSER - Key commands browse mode: "
    " Valid with magenta (or no) cursor      "
    "                                        "
    "MOVEMENT                                "
    " Joy down:     Cursor to next topic     "
    " Joy up:       Cursor to previous topic "
    " Joy right/CR: Follow selected link     "
    " Joy left:     Return to previous topic "
    "                                        "
    "SCROLLING                               "
    " + or space:   Scroll down (next page)  "
    " -:            Scroll up (previous page)"
    "                                        "
    "OTHER                                   "
    " CNTL B:       Return back to BASIC     "
    " G:            Go to specific URL       "
    " H:            Go to home page          "
    " SHIFT H:      Set current page as home "
    " 0-9:          Go to bookmark 0 to 9    "
    " SHIFT 0-9:    Bookmark current page    "
    " ?:            Show help page           "
    "                                        "
    "         Back to previous topic         "
    "          (+ to next help page)         ";
    
    wxString helpText2= "USB BROWSER - Key commands edit mode:   "
    " Valid with yellow or diamond cursor    "
    "                                        "
    "MOVEMENT                                "
    " Joy down:     Cursor to next topic or  "
    "               line                     "
    " Joy up:       Cursor to previous topic "
    "               or line                  "
    "                                        "
    "OTHER                                   "
    " CNTL B:       Return back to BASIC     "
    " CR or CNTL T: Store form text or URL   "
    "                                        "
    " Valid with yellow cursor               "
    "                                        "
    "MOVEMENT                                "
    " Joy right:    Cursor to right          "
    " Joy left:     Cursor to left           "
    "                                        "
    "OTHER                                   "
    " CNTL Q:       Quit 'go to' URL function"
    "                                        "
    "         Back to previous topic         "
    "        (- to previous help page)       ";
    
    currentUrlPos_ = 0;
    lastLink_ = 0;
    continueLink_ = false;
    currentUrlLinePos_ = 0;
    currentUrlScreen_ = 2;
    
    for (int i=960; i<0x7fd; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (int i=0x800; i<=0x900; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (size_t i = 0; i < 960; i++)
    {
        if (i < helpText2.Len())
            charByte = helpText2.GetChar(i);
        else
            charByte = 32;
        
        if (charByte >='a' && charByte <='z')
            charByte -= 0x20;
        else
        {
            if (charByte >='A' && charByte <='Z')
                charByte += 0x20;
        }
        urlScreens[currentUrlScreen_].screenData[i] = charByte;
        
        if (i>888 && i< 912)
        {
            urlScreens[currentUrlScreen_].screenData[i] = urlScreens[currentUrlScreen_].screenData[i] | 0x80;
            urlScreens[currentUrlScreen_].screenData[i+0x400] = 1;
        }
        
    }
    
    currentUrlScreen_ = 1;
    
    for (int i=960; i<0x7fd; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (int i=0x800; i<=0x900; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (size_t i = 0; i < 960; i++)
    {
        if (i < helpText1.Len())
            charByte = helpText1.GetChar(i);
        else
            charByte = 32;
        
        if (charByte >='a' && charByte <='z')
            charByte -= 0x20;
        else
        {
            if (charByte >='A' && charByte <='Z')
                charByte += 0x20;
        }
        urlScreens[currentUrlScreen_].screenData[i] = charByte;
        
        if (i>888 && i< 912)
        {
            urlScreens[currentUrlScreen_].screenData[i] = urlScreens[currentUrlScreen_].screenData[i] | 0x80;
            urlScreens[currentUrlScreen_].screenData[i+0x400] = 1;
        }
        
    }
    
    colourInfo_ = 0x00;
    reshapeInfo_ = 0x00;
    
    switch (reshape_)
    {
        case COLOR_ON_PREVIOUS_PAGE:
            reshape_ = COLOR_NONE;
            break;
            
        case COLOR_RESHAPE:
        case COLOR_ON_THIS_PAGE:
        case COLOR_MULTIPLE:
            reshape_ = COLOR_ON_PREVIOUS_PAGE;
            break;
    }
    for (int screen = 1; screen < numberOfUrlScreens_; screen++)
    {
        urlScreens[screen].screenData[0x7fd] = colourInfo_;
        urlScreens[screen].screenData[0x7fe] = reshape_ ;
        urlScreens[screen].screenData[0x7ff] = reshapeInfo_;
        urlScreens[screen].shapeLocation_ = 0;
        urlScreens[screen].shapeNumber_ = 1;
        
        urlScreens[screen].numberOfLinks = 1;
        urlScreens[screen].urlLinks.resize(urlScreens[screen].numberOfLinks);    
        urlScreens[screen].urlLinks[urlScreens[screen].numberOfLinks - 1] = urlHistory[urlHistorySize_-1];    
    }
}

void Usbcard::getEmailHelp()
{
    Byte charByte;
    networkError_ = USB_FILE_EXISTS;
    urlLink_ = 0;
    shapeColour_ = 0;
    
    numberOfUrlScreens_ = 3;
    urlScreens.resize(numberOfUrlScreens_);
    // 1234567890123456789012345678901234567890
    wxString helpText1= "USB EMAIL - Key commands:               "
    " Valid with magenta (or no) cursor      "
    "                                        "
    "MOVEMENT                                "
    " Joy down:     Cursor to next topic     "
    " Joy up:       Cursor to previous topic "
    " Joy right/CR: Follow selected link     "
    "                                        "
    "SCROLLING                               "
    " + or space:   Scroll down (next page)  "
    " -:            Scroll up (previous page)"
    "                                        "
    "OTHER                                   "
    " CNTL B:       Return back to BASIC     "
    " ?:            Show help page           "
    "                                        "
    "                                        "
    "                                        "
    "                                        "
    "                                        "
    "                                        "
    "                                        "
    "       Back to email input screen       "
    "          (+ to next help page)         ";
    
    wxString helpText2= "USB EMAIL - Key commands edit mode:     "
    " Valid with yellow cursor               "
    "                                        "
    "MOVEMENT                                "
    " Joy down:     Cursor to next topic or  "
    "               line                     "
    " Joy up:       Cursor to previous topic "
    "               or line                  "
    " Joy right:    Cursor to right          "
    " Joy left:     Cursor to left           "
    "                                        "
    "OTHER                                   "
    " CNTL B:       Return back to BASIC     "
    " CNTL T or CR: Store text               "
    " CR in message:Go to next line          "
    "                                        "
    "                                        "
    "                                        "
    "                                        "
    "                                        "
    "                                        "
    "                                        "
    "       Back to email input screen       "
    "        (- to previous help page)       ";
    
    currentUrlPos_ = 0;
    lastLink_ = 0;
    continueLink_ = false;
    currentUrlLinePos_ = 0;
    currentUrlScreen_ = 2;
    
    for (int i=960; i<0x7fd; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (int i=0x800; i<=0x900; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (size_t i = 0; i < 960; i++)
    {
        if (i < helpText2.Len())
            charByte = helpText2.GetChar(i);
        else
            charByte = 32;
        
        if (charByte >='a' && charByte <='z')
            charByte -= 0x20;
        else
        {
            if (charByte >='A' && charByte <='Z')
                charByte += 0x20;
        }
        urlScreens[currentUrlScreen_].screenData[i] = charByte;
        
        if (i>886 && i< 913)
        {
            urlScreens[currentUrlScreen_].screenData[i] = urlScreens[currentUrlScreen_].screenData[i] | 0x80;
            urlScreens[currentUrlScreen_].screenData[i+0x400] = 1;
        }
        
    }
    
    currentUrlScreen_ = 1;
    
    for (int i=960; i<0x7fd; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (int i=0x800; i<=0x900; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (size_t i = 0; i < 960; i++)
    {
        if (i < helpText1.Len())
            charByte = helpText1.GetChar(i);
        else
            charByte = 32;
        
        if (charByte >='a' && charByte <='z')
            charByte -= 0x20;
        else
        {
            if (charByte >='A' && charByte <='Z')
                charByte += 0x20;
        }
        urlScreens[currentUrlScreen_].screenData[i] = charByte;
        
        if (i>886 && i< 913)
        {
            urlScreens[currentUrlScreen_].screenData[i] = urlScreens[currentUrlScreen_].screenData[i] | 0x80;
            urlScreens[currentUrlScreen_].screenData[i+0x400] = 1;
        }
        
    }
    
    colourInfo_ = 0x00;
    reshapeInfo_ = 0x00;
    
    switch (reshape_)
    {
        case COLOR_ON_PREVIOUS_PAGE:
            reshape_ = COLOR_NONE;
            break;
            
        case COLOR_RESHAPE:
        case COLOR_ON_THIS_PAGE:
        case COLOR_MULTIPLE:
            reshape_ = COLOR_ON_PREVIOUS_PAGE;
            break;
    }
    for (int screen = 1; screen < numberOfUrlScreens_; screen++)
    {
        urlScreens[screen].screenData[0x7fd] = colourInfo_;
        urlScreens[screen].screenData[0x7fe] = reshape_ ;
        urlScreens[screen].screenData[0x7ff] = reshapeInfo_;
        urlScreens[screen].shapeLocation_ = 0;
        urlScreens[screen].shapeNumber_ = 1;
        
        urlScreens[screen].numberOfLinks = 1;
        urlScreens[screen].urlLinks.resize(urlScreens[screen].numberOfLinks);    
        urlScreens[screen].urlLinks[urlScreens[screen].numberOfLinks - 1] = "email";    
    }
}

void Usbcard::email(wxString message)
{
    Byte charByte;
    networkError_ = USB_FILE_EXISTS;
    urlLink_ = 0;
    shapeColour_ = 0;
    
    message = "Send email - " + message;
    
    numberOfUrlScreens_ = 3;
    urlScreens.resize(numberOfUrlScreens_);
    // 1234567890123456789012345678901234567890
    wxString emailText1= "To: 111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "Subject: 2222222222222222222222222222222"
    "2222222222222222222222222222222222222222"
    "Message:                                "
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333"
    "3333333333333333333333333333333333333333";
    
    wxString emailText2= "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111";
    
    emailText1 = emailText1 + message;
    emailText2 = emailText2 + message;
    
    currentUrlPos_ = 0;
    lastLink_ = 0;
    continueLink_ = false;
    currentUrlLinePos_ = 0;
    currentUrlScreen_ = 2;
    
    for (int i=960; i<0x7fd; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (int i=0x800; i<=0x900; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (size_t i = 0; i < 960; i++)
    {
        if (i < emailText2.Len())
            charByte = emailText2.GetChar(i);
        else
            charByte = 0;
        
        if (charByte >='a' && charByte <='z')
            charByte -= 0x20;
        else
        {
            if (charByte >='A' && charByte <='Z')
                charByte += 0x20;
        }
        if (charByte == 32)
            charByte = 0;
        if (charByte == '1' || charByte == '2' || charByte == '3')
        {
            urlScreens[currentUrlScreen_].screenData[i] = 0;
            urlScreens[currentUrlScreen_].screenData[i+0x400] = charByte - '1' + 1;
        }
        else
            urlScreens[currentUrlScreen_].screenData[i] = charByte;
        
        if (i>919 && i< 931)
        {
            urlScreens[currentUrlScreen_].screenData[i] = urlScreens[currentUrlScreen_].screenData[i] | 0x80;
            urlScreens[currentUrlScreen_].screenData[i+0x400] = 2;
        }
        
    }
    
    currentUrlScreen_ = 1;
    
    for (int i=960; i<0x7fd; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (int i=0x800; i<=0x900; i++)
        urlScreens[currentUrlScreen_].screenData[i]=0;
    
    for (size_t i = 0; i < 960; i++)
    {
        if (i < emailText1.Len())
            charByte = emailText1.GetChar(i);
        else
            charByte = 0;
        
        if (charByte >='a' && charByte <='z')
            charByte -= 0x20;
        else
        {
            if (charByte >='A' && charByte <='Z')
                charByte += 0x20;
        }
        if (charByte == 32)
            charByte = 0;
        if (charByte == '1' || charByte == '2' || charByte == '3')
        {
            urlScreens[currentUrlScreen_].screenData[i] = 0;
            urlScreens[currentUrlScreen_].screenData[i+0x400] = charByte - '1' + 1;
        }
        else
            urlScreens[currentUrlScreen_].screenData[i] = charByte;
        
        if (i>919 && i< 931)
        {
            urlScreens[currentUrlScreen_].screenData[i] = urlScreens[currentUrlScreen_].screenData[i] | 0x80;
            urlScreens[currentUrlScreen_].screenData[i+0x400] = 4;
        }
        
    }
    
    colourInfo_ = 0x00;
    reshapeInfo_ = 0x00;
    
    switch (reshape_)
    {
        case COLOR_ON_PREVIOUS_PAGE:
            reshape_ = COLOR_NONE;
            break;
            
        case COLOR_RESHAPE:
        case COLOR_ON_THIS_PAGE:
        case COLOR_MULTIPLE:
            reshape_ = COLOR_ON_PREVIOUS_PAGE;
            break;
    }
    
    for (int screen = 1; screen < numberOfUrlScreens_; screen++)
    {
        urlScreens[screen].screenData[0x7fd] = colourInfo_;
        urlScreens[screen].screenData[0x7fe] = reshape_ ;
        urlScreens[screen].screenData[0x7ff] = reshapeInfo_;
        urlScreens[screen].shapeLocation_ = 0;
        urlScreens[screen].shapeNumber_ = 1;
        
        if (screen == 1)
            urlScreens[screen].numberOfLinks = 4;
        else
            urlScreens[screen].numberOfLinks = 2;
        
        urlScreens[screen].urlLinks.resize(urlScreens[screen].numberOfLinks);    
        urlScreens[screen].inputInfo.resize(urlScreens[screen].numberOfLinks);    
        
        if (screen == 1)
        {
            urlScreens[screen].urlLinks[0] = "to";    
            urlScreens[screen].inputInfo[0].inputType = INPUT_TYPE_TEXT;    
            urlScreens[screen].inputInfo[0].formNumber = 0;    
            
            urlScreens[screen].urlLinks[1] = "subject";    
            urlScreens[screen].inputInfo[1].inputType = INPUT_TYPE_TEXT;    
            urlScreens[screen].inputInfo[1].formNumber = 0;    
            
            urlScreens[screen].urlLinks[2] = "message";    
            urlScreens[screen].inputInfo[2].inputType = INPUT_TYPE_TEXT;    
            urlScreens[screen].inputInfo[2].formNumber = 0;    
            
            urlScreens[screen].urlLinks[3] = "http://www.comx35.com/comx35sbhs/mailer.php";    
            urlScreens[screen].inputInfo[3].inputType = INPUT_TYPE_SUBMIT;    
            urlScreens[screen].inputInfo[3].formNumber = 0;
            
            urlScreens[screen].screenData[0x801] = INPUT_TYPE_TEXT;
            urlScreens[screen].screenData[0x802] = INPUT_TYPE_TEXT;
            urlScreens[screen].screenData[0x803] = INPUT_TYPE_MESSAGE;
        }
        else
        {
            urlScreens[screen].urlLinks[0] = "message";    
            urlScreens[screen].inputInfo[0].inputType = INPUT_TYPE_TEXT;    
            urlScreens[screen].inputInfo[0].formNumber = 0;    
            
            urlScreens[screen].urlLinks[1] = "http://www.comx35.com/comx35sbhs/mailer.php";    
            urlScreens[screen].inputInfo[1].inputType = INPUT_TYPE_SUBMIT;    
            urlScreens[screen].inputInfo[1].formNumber = 0;
            
            urlScreens[screen].screenData[0x801] = INPUT_TYPE_MESSAGE;
        }
    }
}


void Usbcard::email_error(wxString message)
{
    Byte charByte;
    
    currentUrlPos_ = 0;
    lastLink_ = 0;
    continueLink_ = false;
    currentUrlLinePos_ = 0;
    currentUrlScreen_ = 1;
    
    for (int screen = 1; screen < numberOfUrlScreens_; screen++)
    {
        for (size_t i=933; i < 960; i++)
        {
            if (i-933 < message.Len())
                charByte = message.GetChar(i-933);
            else
                charByte = 0;
            
            if (charByte >='a' && charByte <='z')
                charByte -= 0x20;
            else
            {
                if (charByte >='A' && charByte <='Z')
                    charByte += 0x20;
            }
            if (charByte == 32)
                charByte = 0;
            
            urlScreens[screen].screenData[i] = charByte;
        }
    }
}

void Usbcard::setLinkInfo(bool increaseLinkSize, wxString urlLink, int inputType, int formNumber)
{
    if (tableTag_ != 0)
    {
        if (tdTag_)
        {
            if (increaseLinkSize)
            {
                urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].numberOfLinks++;
                urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].urlLinks.resize(urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].numberOfLinks);
                urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].inputInfo.resize(urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].numberOfLinks);
            }
            urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].inputInfo[urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].numberOfLinks - 1].inputType = inputType;
            urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].inputInfo[urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].numberOfLinks - 1].formNumber = formNumber;
            urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].urlLinks[urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].numberOfLinks - 1] = urlLink;
        }
        if (captionTag_)
        {
            urlTable.caption.numberOfLinks++;
            urlTable.caption.urlLinks.resize(urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].numberOfLinks);
            urlTable.caption.inputInfo.resize(urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].numberOfLinks);
            
            urlTable.caption.inputInfo[urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].numberOfLinks - 1].inputType = inputType;
            urlTable.caption.inputInfo[urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].numberOfLinks - 1].formNumber = formNumber;
            urlTable.caption.urlLinks[urlTable.cellContent[urlTable.numberOfRows - 1][urlTable.column - 1].numberOfLinks - 1] = urlLink;
        }
        return;
    }
    
    if (increaseLinkSize)
    {
        urlScreens[currentUrlScreen_].numberOfLinks++;
        urlScreens[currentUrlScreen_].urlLinks.resize(urlScreens[currentUrlScreen_].numberOfLinks);    
        urlScreens[currentUrlScreen_].inputInfo.resize(urlScreens[currentUrlScreen_].numberOfLinks);
    }
    urlScreens[currentUrlScreen_].inputInfo[urlScreens[currentUrlScreen_].numberOfLinks - 1].inputType = inputType;    
    urlScreens[currentUrlScreen_].inputInfo[urlScreens[currentUrlScreen_].numberOfLinks - 1].formNumber = formNumber;    
    urlScreens[currentUrlScreen_].urlLinks[urlScreens[currentUrlScreen_].numberOfLinks - 1] = urlLink;    
}

void Usbcard::aTag()
{
    hrefTag_ = false;
    nameTag_ = "";
    
    while (htmlTag.attributeNumber >= 1)
    {
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "name")
        {
            nameTag_ = htmlTag.attributeValue[htmlTag.attributeNumber-1];
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "href")
        {
            wxString hrefTemp = htmlTag.attributeValue[htmlTag.attributeNumber-1].AfterLast('.');
            if (!(hrefTemp == "svg" || hrefTemp == "png" || hrefTemp == "gif" || hrefTemp == "jpg" || hrefTemp == "pdf" || hrefTemp == "mp3" || hrefTemp == "comx"))
            {  
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("mailto:") == wxNOT_FOUND && htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("ftp:") == wxNOT_FOUND)
                    hrefTag_ = true;
            }
            
            if (hrefTag_)
            {
                htmlTag.attributeValue[htmlTag.attributeNumber-1] = shapeHtmlCharacter(htmlTag.attributeValue[htmlTag.attributeNumber-1]);
                
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Len()>=2)
                {
                    if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(2) == "//")
                        htmlTag.attributeValue[htmlTag.attributeNumber-1] = "http:"+htmlTag.attributeValue[htmlTag.attributeNumber-1];
                }
                
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("http://") == wxNOT_FOUND && htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("https://") == wxNOT_FOUND )
                {
                    wxString url;
                    if (baseName_ == "")
                        url = urlServer_;
                    else
                        url = baseName_;
                    
                    if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(1) == "#")
                    {
                        if (urlFullPath_ == "")
                            htmlTag.attributeValue[htmlTag.attributeNumber-1] = url + "/" + htmlTag.attributeValue[htmlTag.attributeNumber-1];
                        else
                            htmlTag.attributeValue[htmlTag.attributeNumber-1] = url + urlFullPath_ + htmlTag.attributeValue[htmlTag.attributeNumber-1];
                    }
                    else
                    {
                        if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(1) == "/")
                            htmlTag.attributeValue[htmlTag.attributeNumber-1] = url + htmlTag.attributeValue[htmlTag.attributeNumber-1];
                        else            
                            htmlTag.attributeValue[htmlTag.attributeNumber-1] = url + urlPath_ + "/" +  htmlTag.attributeValue[htmlTag.attributeNumber-1];
                    }
                }
                
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("/url?q=") != wxNOT_FOUND)
                {
                    if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(7) == "/url?q=")
                    {
                        htmlTag.attributeValue[htmlTag.attributeNumber-1] = htmlTag.attributeValue[htmlTag.attributeNumber-1].Right(htmlTag.attributeValue[htmlTag.attributeNumber-1].Len()-7);
                        if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Find('&') != wxNOT_FOUND)
                            htmlTag.attributeValue[htmlTag.attributeNumber-1] = htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(htmlTag.attributeValue[htmlTag.attributeNumber-1].Find('&'));
                    }
                }
                
                setLinkInfo(urlLink_ == 0, htmlTag.attributeValue[htmlTag.attributeNumber-1], INPUT_TYPE_NONE, 0);
            }
        }
        htmlTag.attributeNumber--;
    }            
    if (hrefTag_)
    {
        if (tableTag_ != 0 && urlLink_ == 0)
            writeChar(TABLE_A, false, false);
        urlLink_ = 0x80;
    }
}

void Usbcard::formTag()
{
    hrefTag_ = false;
    nameTag_ = "";
    
    wxString formAction = "", formName = "";
    
    while (htmlTag.attributeNumber >= 1)
    {
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "action")
        {
            wxString hrefTemp = htmlTag.attributeValue[htmlTag.attributeNumber-1].AfterLast('.');
            if (!(hrefTemp == "svg" || hrefTemp == "png" || hrefTemp == "gif" || hrefTemp == "jpg" || hrefTemp == "pdf" || hrefTemp == "mp3" || hrefTemp == "comx"))
            {
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("mailto:") == wxNOT_FOUND && htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("ftp:") == wxNOT_FOUND)
                    hrefTag_ = true;
            }
            
            if (hrefTag_)
            {
                htmlTag.attributeValue[htmlTag.attributeNumber-1] = shapeHtmlCharacter(htmlTag.attributeValue[htmlTag.attributeNumber-1]);
                
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Len()>=2)
                {
                    if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(2) == "//")
                        htmlTag.attributeValue[htmlTag.attributeNumber-1] = "http:"+htmlTag.attributeValue[htmlTag.attributeNumber-1];
                }
                
                
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("http://") == wxNOT_FOUND && htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("https://") == wxNOT_FOUND )
                {
                    wxString url;
                    if (baseName_ == "")
                        url = urlServer_;
                    else
                        url = baseName_;
                    
                    if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(1) == "#")
                    {
                        if (urlFullPath_ == "")
                            htmlTag.attributeValue[htmlTag.attributeNumber-1] = url + "/" + htmlTag.attributeValue[htmlTag.attributeNumber-1];
                        else
                            htmlTag.attributeValue[htmlTag.attributeNumber-1] = url + urlFullPath_ + htmlTag.attributeValue[htmlTag.attributeNumber-1];
                    }
                    else
                    {
                        if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(1) == "/")
                            htmlTag.attributeValue[htmlTag.attributeNumber-1] = url + htmlTag.attributeValue[htmlTag.attributeNumber-1];
                        else            
                            htmlTag.attributeValue[htmlTag.attributeNumber-1] = url + urlPath_ + "/" +  htmlTag.attributeValue[htmlTag.attributeNumber-1];
                    }
                }
                
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("/url?q=") != wxNOT_FOUND)
                {
                    if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(7) == "/url?q=")
                    {
                        htmlTag.attributeValue[htmlTag.attributeNumber-1] = htmlTag.attributeValue[htmlTag.attributeNumber-1].Right(htmlTag.attributeValue[htmlTag.attributeNumber-1].Len()-7);
                        if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Find('&') != wxNOT_FOUND)
                            htmlTag.attributeValue[htmlTag.attributeNumber-1] = htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(htmlTag.attributeValue[htmlTag.attributeNumber-1].Find('&'));
                    }
                }
                
                formAction = htmlTag.attributeValue[htmlTag.attributeNumber-1];    
            }
        }
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "name")
        {
            formName = htmlTag.attributeValue[htmlTag.attributeNumber-1];
        }
        htmlTag.attributeNumber--;
    }    
    
    bool notFound = true;
    for (size_t number=0; number<numberOfForms_; number++)
    {
        if (htmlFormInfo[number].formName == formName)
        {
            notFound = false;
            if (formAction != "")
                htmlFormInfo[number].formAction = formAction;
        }
    }
    
    if (notFound)
    {
        numberOfForms_++;
        
        htmlFormInfo.resize(numberOfForms_);
        htmlFormInfo[numberOfForms_-1].formName = formName;
        htmlFormInfo[numberOfForms_-1].formAction = formAction;
    }
}

void Usbcard::olTag()
{
    liNum_ += 3;
    if (liNum_ > 30)
        liNum_ = 30;
    writeLine();
    
    olTag_[liNum_] = true;
    olReversed_[liNum_] = false;
    olType_[liNum_] = TAG_OL_TYPE1;
    olNumber_[liNum_] = 1;
    
    long olNumber;
    
    while (htmlTag.attributeNumber >= 1)
    {
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "start")
        {
            htmlTag.attributeValue[htmlTag.attributeNumber-1].ToLong(&olNumber);
            olNumber_[liNum_] = (int)olNumber;
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "type")
        {
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "a")
                olType_[liNum_] = TAG_OL_TYPEa;
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "A")
                olType_[liNum_] = TAG_OL_TYPEA;
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "i")
                olType_[liNum_] = TAG_OL_TYPEi;
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "I")
                olType_[liNum_] = TAG_OL_TYPEI;
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "reversed")
        {
            olReversed_[liNum_] = true;
        }
        htmlTag.attributeNumber--;
    }
    if (olType_[liNum_] == TAG_OL_TYPEi || olType_[liNum_] == TAG_OL_TYPEI)
    {
        int oldNum = liNum_;
        liNum_ += 2;
        if (liNum_ > 30)
            liNum_ = 30;
        olTag_[liNum_] = olTag_[oldNum];
        olReversed_[liNum_] = olReversed_[oldNum];
        olType_[liNum_] = olType_[oldNum];
        olNumber_[liNum_] = olNumber_[oldNum];
    }
}

void Usbcard::liTag()
{
    if (olTag_[liNum_])
    {
        long olNumber;
        int liType = olType_[liNum_];
        
        while (htmlTag.attributeNumber >= 1)
        {
            if (htmlTag.attribute[htmlTag.attributeNumber-1] == "value")
            {
                htmlTag.attributeValue[htmlTag.attributeNumber-1].ToLong(&olNumber);
                olNumber_[liNum_] = (int)olNumber;
            }
            if (htmlTag.attribute[htmlTag.attributeNumber-1] == "type")
            {
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "1")
                    liType = TAG_OL_TYPE1;
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "a")
                    liType = TAG_OL_TYPEa;
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "A")
                    liType = TAG_OL_TYPEA;
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "i")
                    liType = TAG_OL_TYPEi;
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "I")
                    liType = TAG_OL_TYPEI;
            }
            htmlTag.attributeNumber--;
        }
        
        if (currentUrlLinePos_ == 0)
            tab();
        if (currentUrlLinePos_ >= 2)
            textLine_ = textLine_.Left(textLine_.Len()-3);
        
        wxString numberStr;
        int ten, one; 
        
        switch(liType)
        {
            case TAG_OL_TYPE1:
                numberStr.Printf("%2d.",olNumber_[liNum_]);
                break;
                
            case TAG_OL_TYPEa:
            case TAG_OL_TYPEA:
                ten = ((olNumber_[liNum_]-1)/26)%27; 
                one = ((olNumber_[liNum_]-1)/1)%26; 
                
                if (olNumber_[liNum_] > 702)
                    numberStr = "**.";
                else
                {
                    numberStr = "  .";
                    numberStr.SetChar(1,one + 0x61);
                    if (ten != 0)
                        numberStr.SetChar(0,ten + 0x60);
                }
                if (olType_[liNum_] == TAG_OL_TYPEA)
                    numberStr = numberStr.MakeUpper();
                break;
                
            case TAG_OL_TYPEi:
            case TAG_OL_TYPEI:
                switch (olNumber_[liNum_])
            {
                case 1:
                    numberStr.Printf("   i.");
                    break;
                    
                case 2:
                    numberStr.Printf("  ii.");
                    break;
                    
                case 3:
                    numberStr.Printf(" iii.");
                    break;
                    
                case 4:
                    numberStr.Printf("  iv.");
                    break;
                    
                case 5:
                    numberStr.Printf("   v.");
                    break;
                    
                case 6:
                    numberStr.Printf("  vi.");
                    break;
                    
                case 7:
                    numberStr.Printf(" vii.");
                    break;
                    
                case 8:
                    numberStr.Printf("viii.");
                    break;
                    
                case 9:
                    numberStr.Printf("  ix.");
                    break;
                    
                case 10:
                    numberStr.Printf("   x.");
                    break;
                    
                case 11:
                    numberStr.Printf("  xi.");
                    break;
                    
                case 12:
                    numberStr.Printf(" xii.");
                    break;
                    
                case 13:
                    numberStr.Printf("xiii.");
                    break;
                    
                case 14:
                    numberStr.Printf(" xiv.");
                    break;
                    
                case 15:
                    numberStr.Printf("  xv.");
                    break;
                    
                case 16:
                    numberStr.Printf(" xvi.");
                    break;
                    
                case 17:
                    numberStr.Printf("xvii.");
                    break;
                    
                default:
                    numberStr.Printf("****.");
                    break;
            }
                if (olType_[liNum_] == TAG_OL_TYPEI)
                    numberStr = numberStr.MakeUpper();
                break;
                
        }
        textLine_ += numberStr;
        if (olReversed_[liNum_])
            olNumber_[liNum_]--;
        else
            olNumber_[liNum_]++;
    }
    else
    {
        int liType = TAG_IL_DASH;
        wxString bullet;
        
        while (htmlTag.attributeNumber >= 1)
        {
            if (htmlTag.attribute[htmlTag.attributeNumber-1] == "type")
            {
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "disc")
                {
                    bullet="&disc;";
                    liType = TAG_IL_DISC;
                    bullet = shapeHtmlCharacter(bullet);
                }
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "circle")
                {
                    bullet="&circle;";
                    liType = TAG_IL_CIRCLE;
                    bullet = shapeHtmlCharacter(bullet);
                }
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "square")
                {
                    bullet="&square;";
                    liType = TAG_IL_SQUARE;
                    bullet = shapeHtmlCharacter(bullet);
                }
            }
            htmlTag.attributeNumber--;
        }
        
        if (currentUrlLinePos_ == 0)
            tab();
        if (liType == TAG_IL_DASH)
        {
            if (currentUrlLinePos_ < 2)
                writeText("- ", urlLink_ == 0x80, shapeColour_ == 0x80, false);
            else
                textLine_.SetChar(currentUrlLinePos_-2, bullets[(liNum_/3)-1]);
        }
        else
        {
            if (currentUrlLinePos_ < 2)
                writeText(bullet+" ", urlLink_ == 0x80, shapeColour_ == 0x80, false);
            else
                textLine_.SetChar(currentUrlLinePos_-2, bullet[0].GetValue());
        }
    }
}

void Usbcard::sourceTag()
{
    wxString usemap = "";
    wxString temp = "";
    while (htmlTag.attributeNumber >= 1)
    {
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "src")
        {
            temp = htmlTag.attributeValue[htmlTag.attributeNumber-1].AfterLast('/');
            htmlTag.attributeValue[htmlTag.attributeNumber-1] = temp.BeforeFirst('.');
            temp = htmlTag.attributeValue[htmlTag.attributeNumber-1];
            temp.Replace("%20", " ");
        }
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "usemap")
        {
        }
        htmlTag.attributeNumber--;
    }
    if (temp != "")
    {
        if (usemap != "")
            writeText("[USEMAP:"+temp+"] ", true, false, false);
        else
            writeText("["+temp+"] ", urlLink_ == 0x80, false, false);
    }
}

void Usbcard::imgTag()
{
    wxString temp = "";
    bool alt=false;
    
    while (htmlTag.attributeNumber >= 1)
    {
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "src" && temp == "")
        {
            temp = htmlTag.attributeValue[htmlTag.attributeNumber-1].AfterLast('/');
            htmlTag.attributeValue[htmlTag.attributeNumber-1] = temp.BeforeFirst('.');
            temp = htmlTag.attributeValue[htmlTag.attributeNumber-1];
            temp.Replace("%20", " ");
        }
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "alt")
        {
            if (htmlTag.attributeValue[htmlTag.attributeNumber - 1] != "")
            {
                temp = htmlTag.attributeValue[htmlTag.attributeNumber - 1];
                temp.Replace("%20", " ");
                temp.Replace("&#13;", " ");
                while (temp.Find("  ") != wxNOT_FOUND)
                    temp.Replace("  ", " ");
                alt = true;
            }
        }
        htmlTag.attributeNumber--;
    }
    if (temp != "")
    {
        if (alt)
            writeText(temp, urlLink_ == 0x80, false, false);
        else
            writeText("["+temp+"] ", urlLink_ == 0x80, false, false);
    }
}

void Usbcard::inputTag()
{
    wxString temp="", alt="", value="", src="image", name="input", formAction = "";
    wxString comxChar = " ", box = " ", tick = " ";
    comxChar.SetChar(0, 128);
    int inputType = INPUT_TYPE_TEXT;
    size_t size=20, maxLength = 0, formNumber;
    bool checked = false;
    
    formNumber = numberOfForms_;
    
    while (htmlTag.attributeNumber >= 1)
    {
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "src")
        {
            src = htmlTag.attributeValue[htmlTag.attributeNumber-1].AfterLast('/');
            htmlTag.attributeValue[htmlTag.attributeNumber-1] = src.BeforeFirst('.');
            src = htmlTag.attributeValue[htmlTag.attributeNumber-1];
            src.Replace("%20", " ");
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "value")
        {
            value = htmlTag.attributeValue[htmlTag.attributeNumber-1];
            value.Replace("%20", " ");
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "alt")
        {
            if (htmlTag.attributeValue[htmlTag.attributeNumber - 1] != "")
            {
                alt = htmlTag.attributeValue[htmlTag.attributeNumber-1];
                alt.Replace("%20", " ");
                temp.Replace("&#13;", " ");
            }
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "name")
            name = htmlTag.attributeValue[htmlTag.attributeNumber-1];
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "checked")
            checked = true;
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "size")
        {
            long longSize;
            htmlTag.attributeValue[htmlTag.attributeNumber-1].ToLong(&longSize);
            size = (size_t)longSize;
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "maxlength")
        {
            long longSize;
            htmlTag.attributeValue[htmlTag.attributeNumber-1].ToLong(&longSize);
            maxLength = (size_t)longSize;
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "type")
        {
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "text")
                inputType = INPUT_TYPE_TEXT;
            
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "image")
                inputType = INPUT_TYPE_IMAGE;
            
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "button")
                inputType = INPUT_TYPE_BUTTON;
            
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "submit")
                inputType = INPUT_TYPE_SUBMIT;
            
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "checkbox")
                inputType = INPUT_TYPE_CHECKBOX;
            
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "radio")
                inputType = INPUT_TYPE_RADIO;
            
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "password")
                inputType = INPUT_TYPE_PASSWD;
            
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "hidden")
                inputType = INPUT_TYPE_HIDDEN;
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "form")
        {
            bool notFound = true;
            for (size_t number=0; number<numberOfForms_; number++)
            {
                if (htmlFormInfo[number].formName == htmlTag.attributeValue[htmlTag.attributeNumber-1])
                {
                    notFound = false;
                    formNumber = number + 1;
                }
            }
            
            if (notFound)
            {
                numberOfForms_++;
                
                htmlFormInfo.resize(numberOfForms_);
                htmlFormInfo[numberOfForms_-1].formName = htmlTag.attributeValue[htmlTag.attributeNumber-1];
                htmlFormInfo[numberOfForms_-1].formAction = "";
                formNumber = numberOfForms_;
            }
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "formaction")
        {
            formAction = htmlTag.attributeValue[htmlTag.attributeNumber-1];
        }
        
        htmlTag.attributeNumber--;
    }
    urlInput_ = true;
    
    if (maxLength != 0)
        size = maxLength;
    
    switch (inputType)
    {
        case INPUT_TYPE_TEXT:
            urlScreens[currentUrlScreen_].textInputFound_ = true;
            while (value.Len()<size)
                value = value + comxChar;
            
            setLinkInfo(true, name, INPUT_TYPE_TEXT, (int)formNumber);
            writeInputText(value, URL_NORMAL_COLOR, URL_NO_RADIO_INPUT, 0, URL_NO_NEW_LINE, URL_DO_NOT_BREAK_LINE);
            break;
            
        case INPUT_TYPE_PASSWD:
            urlScreens[currentUrlScreen_].textInputFound_ = true;
            while (value.Len()<size+1)
                value = value + comxChar;
            
            setLinkInfo(true, name, INPUT_TYPE_PASSWD, (int)formNumber);
            writeInputText(value, URL_NORMAL_COLOR, URL_NO_RADIO_INPUT, 0, URL_NO_NEW_LINE, URL_DO_NOT_BREAK_LINE);
            break;
            
        case INPUT_TYPE_CHECKBOX:
            setLinkInfo(true, name + "=" + value, INPUT_TYPE_CHECKBOX, (int)formNumber);
            
            box[0] = URL_BOX;
            tick[0] = URL_TICK;
            
            if (checked)
                writeInputText(tick, URL_NORMAL_COLOR, URL_NO_RADIO_INPUT, 0, URL_NO_NEW_LINE, URL_DO_NOT_BREAK_LINE);
            else
                writeInputText(box, URL_NORMAL_COLOR, URL_NO_RADIO_INPUT, 0, URL_NO_NEW_LINE, URL_DO_NOT_BREAK_LINE);
            break;
            
        case INPUT_TYPE_RADIO:
        {
            bool radioFound = false;
            size_t radioLink = 0;
            size_t radioNumber_ = 0;
            
            for (size_t i=0; i<urlScreens[currentUrlScreen_].numberOfRadios_; i++)
            {
                if (urlScreens[currentUrlScreen_].radio_[i].name_ == name)
                {
                    radioFound = true;
                    radioLink = urlScreens[currentUrlScreen_].radio_[i].linkNumber_;
                    radioNumber_ = i;
                }
            }
            
            if (!radioFound)
            {
                setLinkInfo(true, name, INPUT_TYPE_RADIO, (int)formNumber);
                radioLink = urlScreens[currentUrlScreen_].numberOfLinks;
                
                urlScreens[currentUrlScreen_].numberOfRadios_++;
                radioNumber_ = urlScreens[currentUrlScreen_].numberOfRadios_ - 1;
                urlScreens[currentUrlScreen_].radio_.resize(urlScreens[currentUrlScreen_].numberOfRadios_);
                urlScreens[currentUrlScreen_].radio_[radioNumber_].linkNumber_ = urlScreens[currentUrlScreen_].numberOfLinks;
                urlScreens[currentUrlScreen_].radio_[radioNumber_].name_ = name;
                urlScreens[currentUrlScreen_].radio_[radioNumber_].numberOfRadioValues_ = 0;
                urlScreens[currentUrlScreen_].radio_[radioNumber_].radioValueNumber_ = 0;
            }
            
            urlScreens[currentUrlScreen_].radio_[radioNumber_].numberOfRadioValues_++;
            urlScreens[currentUrlScreen_].radio_[radioNumber_].value_.resize(urlScreens[currentUrlScreen_].radio_[radioNumber_].numberOfRadioValues_);
            urlScreens[currentUrlScreen_].radio_[radioNumber_].value_[urlScreens[currentUrlScreen_].radio_[radioNumber_].numberOfRadioValues_-1] = value;
            
            box[0] = URL_RADIO_OFF;
            tick[0] = URL_RADIO_ON;
            
            if (checked)
                writeInputText(tick, URL_NORMAL_COLOR, URL_RADIO_INPUT, (int)radioLink, URL_NO_NEW_LINE, URL_DO_NOT_BREAK_LINE);
            else
                writeInputText(box, URL_NORMAL_COLOR, URL_RADIO_INPUT, (int)radioLink, URL_NO_NEW_LINE, URL_DO_NOT_BREAK_LINE);
        }
            break;
            
        case INPUT_TYPE_IMAGE:
            if (formAction == "")
                setLinkInfo(true, htmlFormInfo[formNumber-1].formAction, INPUT_TYPE_SUBMIT, (int)formNumber);
            else
                setLinkInfo(true, formAction, INPUT_TYPE_SUBMIT, (int)formNumber);
            if (tableTag_ != 0 && urlLink_ == 0)
                writeChar(TABLE_A, URL_NO_LINK, URL_NORMAL_COLOR);
            urlLink_ = 0x80;
            if (alt != "")
                writeInputText("["+alt+"] ", URL_NORMAL_COLOR, URL_NO_RADIO_INPUT, 0,URL_NO_NEW_LINE, URL_BREAK_LINE);
            else
            {
                if (value != "")
                    writeInputText("["+value+"] ", URL_NORMAL_COLOR, URL_NO_RADIO_INPUT, 0, URL_NO_NEW_LINE, URL_BREAK_LINE);
                else
                    writeInputText("["+src+"] ", URL_NORMAL_COLOR, URL_NO_RADIO_INPUT, 0, URL_NO_NEW_LINE, URL_BREAK_LINE);
            }
            if (tableTag_ != 0)
                writeChar(TABLE_A, URL_NO_LINK, URL_NORMAL_COLOR);
            urlLink_ = 0;
            break;
            
        case INPUT_TYPE_BUTTON:
        case INPUT_TYPE_SUBMIT:
            if (formAction == "")
                setLinkInfo(true, htmlFormInfo[formNumber-1].formAction, INPUT_TYPE_SUBMIT, (int)formNumber);
            else
                setLinkInfo(true, formAction, INPUT_TYPE_SUBMIT, (int)formNumber);
            if (tableTag_ != 0 && urlLink_ == 0)
                writeChar(TABLE_A, URL_NO_LINK, URL_NORMAL_COLOR);
            urlLink_ = 0x80;
            writeText("["+value+"] ", urlLink_ == 0x80, URL_NORMAL_COLOR, URL_NO_NEW_LINE);
            if (tableTag_ != 0)
                writeChar(TABLE_A, URL_NO_LINK, URL_NORMAL_COLOR);
            urlLink_ = 0;
            break;
            
        case INPUT_TYPE_HIDDEN:
            break;
            
            //        default:
            //            writeText("["+value+"] ", urlLink_ == 0x80, URL_NORMAL_COLOR, urlScreens[currentUrlScreen_].numberOfLinks, URL_NO_NEW_LINE, URL_BREAK_LINE);
            //        break;
            
    }
    urlInput_ = false;
}

void Usbcard::iframeTag()
{
    wxString name = "", link = "";
    
    while (htmlTag.attributeNumber >= 1)
    {
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "src")
        {
            htmlTag.attributeValue[htmlTag.attributeNumber-1] = shapeHtmlCharacter(htmlTag.attributeValue[htmlTag.attributeNumber-1]);
            
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Len()>=2)
            {
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(2) == "//")
                    htmlTag.attributeValue[htmlTag.attributeNumber-1] = "http:"+htmlTag.attributeValue[htmlTag.attributeNumber-1];
            }
            
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("http://") == wxNOT_FOUND && htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("https://") == wxNOT_FOUND )
            {
                wxString url;
                if (baseName_ == "")
                    url = urlServer_;
                else
                    url = baseName_;
                
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(1) == "#")
                {
                    if (urlFullPath_ == "")
                        htmlTag.attributeValue[htmlTag.attributeNumber-1] = url + "/" + htmlTag.attributeValue[htmlTag.attributeNumber-1];
                    else
                        htmlTag.attributeValue[htmlTag.attributeNumber-1] = url + urlFullPath_ + htmlTag.attributeValue[htmlTag.attributeNumber-1];
                }
                else
                {
                    if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(1) == "/")
                        htmlTag.attributeValue[htmlTag.attributeNumber-1] = url + htmlTag.attributeValue[htmlTag.attributeNumber-1];
                    else            
                        htmlTag.attributeValue[htmlTag.attributeNumber-1] = url + urlPath_ + "/" +  htmlTag.attributeValue[htmlTag.attributeNumber-1];
                }
            }
            
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Find("/url?q=") != wxNOT_FOUND)
            {
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(7) == "/url?q=")
                {
                    htmlTag.attributeValue[htmlTag.attributeNumber-1] = htmlTag.attributeValue[htmlTag.attributeNumber-1].Right(htmlTag.attributeValue[htmlTag.attributeNumber-1].Len()-7);
                    if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Find('&') != wxNOT_FOUND)
                        htmlTag.attributeValue[htmlTag.attributeNumber-1] = htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(htmlTag.attributeValue[htmlTag.attributeNumber-1].Find('&'));
                }
            }
            
            setLinkInfo(true, htmlTag.attributeValue[htmlTag.attributeNumber-1], INPUT_TYPE_NONE, 0);
            link = htmlTag.attributeValue[htmlTag.attributeNumber-1];
        }
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "name")
        {
            name = htmlTag.attributeValue[htmlTag.attributeNumber-1];
        }
        htmlTag.attributeNumber--;
    }
    writeText("IFRAME: ", false, false, false);
    if (name != "")
        writeText(name, true, false, true);
    else
        writeText(link, true, false, true);
}

void Usbcard::comxTag()
{
    while (htmlTag.attributeNumber >= 1)
    {
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "screen")
        {
            long screenColor;
            
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1].ToLong(&screenColor))
                screenColor--;
            else
            {
                htmlTag.attributeValue[htmlTag.attributeNumber-1].MakeLower();
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "black")
                    screenColor = 0;
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "green")
                    screenColor = 1;
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "blue")
                    screenColor = 2;
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "cyan")
                    screenColor = 3;
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "red")
                    screenColor = 4;
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "yellow")
                    screenColor = 5;
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "magenta")
                    screenColor = 6;
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "white")
                    screenColor = 7;
            }
            
            if (colourInfo_ == 0)
            {
                reshapeInfo_ = 0xff;
                colourInfo_ = 0xe0;
            }
            
            colourInfo_ = (colourInfo_ & 0xf8) | (screenColor & 0x7);
            urlScreens[currentUrlScreen_].screenData[0x7fd] = colourInfo_;
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "color")
        {
            long color;
            
            htmlTag.attributeValue[htmlTag.attributeNumber-1].ToLong(&color);
            
            color--;
            color &= 0xf;
            
            if (colourInfo_ == 0)
            {
                reshapeInfo_ = 0xff;
                colourInfo_ = 0xe0;
            }
            
            colourInfo_ = (colourInfo_ & 0x9f) | ((color & 0xc) << 3);
            urlScreens[currentUrlScreen_].screenData[0x7fd] = colourInfo_;
            
            reshapeInfo_  = 0x3f | ((color & 0x3) << 6);
            switch (reshape_)
            {
                case COLOR_NONE:
                case COLOR_ON_PREVIOUS_PAGE:
                    reshape_ = COLOR_ON_THIS_PAGE;
                    break;
                    
                case COLOR_RESHAPE:
                case COLOR_ON_THIS_PAGE:
                case COLOR_MULTIPLE:
                    reshape_ = COLOR_MULTIPLE;
                    break;
            }
            urlScreens[currentUrlScreen_].screenData[0x7fe] = reshape_;
            urlScreens[currentUrlScreen_].screenData[0x7ff] = reshapeInfo_;
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "ctone")
        {
            long ctone;
            
            if (!htmlTag.attributeValue[htmlTag.attributeNumber-1].ToLong(&ctone))
            {
                htmlTag.attributeValue[htmlTag.attributeNumber-1].MakeLower();
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "color")
                    ctone = 0;
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "gray")
                    ctone = 1;
            }
            
            if (colourInfo_ == 0)
            {
                reshapeInfo_ = 0xff;
                colourInfo_ = 0xe0;
            }
            
            colourInfo_ = (colourInfo_ & 0xf7) | ((ctone & 0x1) << 3);
            urlScreens[currentUrlScreen_].screenData[0x7fd] = colourInfo_;
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == "char")
        {
            htmlTag.attributeValue[htmlTag.attributeNumber-1].MakeLower();
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "high" || htmlTag.attributeValue[htmlTag.attributeNumber-1] == "white")
            {
                if (tableTag_ != 0)
                    writeChar(TABLE_HIGH_COLOUR, false, false);
                else
                    shapeColour_ = 0x80;
            }
            
            if (htmlTag.attributeValue[htmlTag.attributeNumber-1] == "low" || htmlTag.attributeValue[htmlTag.attributeNumber-1] == "cyan")
            {
                if (tableTag_ != 0)
                    writeChar(TABLE_HIGH_COLOUR, false, false);
                else
                    shapeColour_ = 0;
            }
        }
        
        if (htmlTag.attribute[htmlTag.attributeNumber-1].Len() >= 5)
        {
            if (htmlTag.attribute[htmlTag.attributeNumber-1].Left(5) == "shape")
            {
                long shapeNumber, nibble, shapeLine, line=0;
                wxString nibbleString;
                htmlTag.attribute[htmlTag.attributeNumber-1].Right(htmlTag.attribute[htmlTag.attributeNumber-1].Len()-5).ToLong(&shapeNumber);
                
                if (htmlTag.attributeValue[htmlTag.attributeNumber-1].Len() > 18)
                    htmlTag.attributeValue[htmlTag.attributeNumber-1] = htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(18);
                
                while (htmlTag.attributeValue[htmlTag.attributeNumber-1].Len() < 18)
                    htmlTag.attributeValue[htmlTag.attributeNumber-1] += "0";
                
                shapeNumber &= 0x7f;
                if (shapeNumber > 30)
                    urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = shapeNumber | 0x80;
                while (htmlTag.attributeValue[htmlTag.attributeNumber-1].Len() > 1)
                {
                    nibbleString = htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(1);
                    htmlTag.attributeValue[htmlTag.attributeNumber-1] = htmlTag.attributeValue[htmlTag.attributeNumber-1].Right(htmlTag.attributeValue[htmlTag.attributeNumber-1].Len()-1);
                    nibbleString.ToLong(&nibble, 16);
                    shapeLine = nibble * 16;
                    nibbleString = htmlTag.attributeValue[htmlTag.attributeNumber-1].Left(1);
                    htmlTag.attributeValue[htmlTag.attributeNumber-1] = htmlTag.attributeValue[htmlTag.attributeNumber-1].Right(htmlTag.attributeValue[htmlTag.attributeNumber-1].Len()-1);
                    nibbleString.ToLong(&nibble, 16);
                    shapeLine += nibble;
                    if (shapeNumber > 30)
                        urlScreens[currentUrlScreen_].screenData[0x900+urlScreens[currentUrlScreen_].shapeLocation_++] = shapeLine;
                    else
                        shapeValues[shapeNumber+HTML_SHAPE-1][line] = shapeLine;
                    line++;
                }
            }
        }
        
        htmlTag.attributeNumber--;
    }
}

int Usbcard::checkLinkNumber()
{
    if (urlScreens[currentUrlScreen_].numberOfLinks <= lastLink_)
        return 0;
    
    if (urlScreens[currentUrlScreen_].numberOfLinks == lastLink_ + 1)
    {
        lastLink_ = urlScreens[currentUrlScreen_].numberOfLinks;
        return 0;
    }
    
    int returnValue = 0;
    int endNumber = urlScreens[currentUrlScreen_].numberOfLinks;
    for (int linkNumber=lastLink_ + 1; linkNumber<endNumber; linkNumber++)
    {
        urlScreens[currentUrlScreen_].urlLinks[linkNumber - 1] = urlScreens[currentUrlScreen_].urlLinks[urlScreens[currentUrlScreen_].numberOfLinks - 1];    
        urlScreens[currentUrlScreen_].inputInfo[linkNumber - 1].inputType = urlScreens[currentUrlScreen_].inputInfo[urlScreens[currentUrlScreen_].numberOfLinks - 1].inputType;    
        urlScreens[currentUrlScreen_].inputInfo[linkNumber - 1].formNumber = urlScreens[currentUrlScreen_].inputInfo[urlScreens[currentUrlScreen_].numberOfLinks - 1].formNumber;    
        
        urlScreens[currentUrlScreen_].numberOfLinks--;
        returnValue++;
    }
    lastLink_ = urlScreens[currentUrlScreen_].numberOfLinks;
    return returnValue;
}

void Usbcard::showAttribute(wxString attribute)
{
    wxString temp;
    while (htmlTag.attributeNumber >= 1)
    {
        if (htmlTag.attribute[htmlTag.attributeNumber-1] == attribute)
        {
            temp = htmlTag.attributeValue[htmlTag.attributeNumber-1].AfterLast('/');
            htmlTag.attributeValue[htmlTag.attributeNumber-1] = temp.BeforeFirst('.');
            temp = htmlTag.attributeValue[htmlTag.attributeNumber-1];
            writeText("["+temp+"] ", urlLink_ == 0x80, false, false);
        }
        htmlTag.attributeNumber--;
    }
}

wxString Usbcard::getForm()
{
    int inputNumber;
    wxString returnData = "?", nextInput;
    char characterValue;
    wxString returnCode = " ";
    returnCode[0] = 0x1a;
    
    for (int screen=1; screen<numberOfUrlScreens_; screen++)
    {
        for (int i=0x400; i<0x7c0; i++)
        {
            inputNumber = urlScreens[screen].screenData[i];
            if (inputNumber != 0 && inputNumber <= urlScreens[screen].numberOfLinks)
            {
                if (urlScreens[screen].inputInfo[inputNumber-1].inputType != INPUT_TYPE_NONE && urlScreens[screen].inputInfo[inputNumber-1].inputType != INPUT_TYPE_IMAGE && urlScreens[screen].inputInfo[inputNumber-1].inputType != INPUT_TYPE_BUTTON && urlScreens[screen].inputInfo[inputNumber-1].inputType != INPUT_TYPE_SUBMIT)
                {
                    if (urlScreens[currentUrlScreen_].inputInfo[header[HEADER_URL_LINK]-1].formNumber == urlScreens[screen].inputInfo[inputNumber-1].formNumber)
                    {
                        nextInput = "";
                        switch (urlScreens[screen].inputInfo[inputNumber-1].inputType)
                        {
                            case INPUT_TYPE_TEXT:
                            case INPUT_TYPE_PASSWD:
                                nextInput = urlScreens[screen].urlLinks[inputNumber-1] + "=";
                                while ((urlScreens[screen].screenData[i]) == inputNumber)
                                {
                                    characterValue = urlScreens[screen].screenData[i-0x400] & 0x7f;
                                    if (characterValue == 0)
                                        characterValue = 32;
                                    if (characterValue >='a' && characterValue <='z')
                                        characterValue -= 0x20;
                                    else
                                    {
                                        if (characterValue >='A' && characterValue <='Z')
                                            characterValue += 0x20;
                                    }
                                    nextInput.Append(characterValue);
                                    if (characterValue == 0x1a)
                                    {
                                        characterValue = 0;
                                        
                                        while (characterValue == 0 && (urlScreens[screen].screenData[i]) == inputNumber)
                                        {
                                            i++;
                                            characterValue = urlScreens[screen].screenData[i-0x400] & 0x7f;
                                        }
                                    }
                                    else
                                        i++;
                                    if (i == 0x7c0 && screen<(numberOfUrlScreens_-1))
                                    {
                                        i = 0x400;
                                        screen++;
                                        inputNumber = urlScreens[screen].screenData[i];
                                    }
                                }
                                nextInput = nextInput.Trim();
                                nextInput.Replace(" ", "%20");
                                nextInput.Replace(returnCode, "%0d%0a");
                                nextInput = nextInput + ";";
                                break;
                                
                            case INPUT_TYPE_CHECKBOX:
                                characterValue = urlScreens[screen].screenData[i-0x400] & 0x7f;
                                if (characterValue == URL_TICK)
                                    nextInput = urlScreens[screen].urlLinks[inputNumber-1] + ";";
                                break;
                                
                            case INPUT_TYPE_RADIO:
                                characterValue = urlScreens[screen].screenData[i-0x400] & 0x7f;
                                if (characterValue == URL_RADIO_ON)
                                    nextInput = urlScreens[screen].urlLinks[inputNumber-1];
                                for (size_t line=0; line< urlScreens[screen].numberOfRadios_; line++)
                                {
                                    if (urlScreens[screen].urlLinks[inputNumber-1] == urlScreens[screen].radio_[line].name_ && urlScreens[screen].radio_[line].radioValueNumber_ < urlScreens[screen].radio_[line].numberOfRadioValues_)
                                    {
                                        if (nextInput != "")
                                            nextInput = nextInput + "=" + urlScreens[screen].radio_[line].value_[urlScreens[screen].radio_[line].radioValueNumber_] + ";";
                                        urlScreens[screen].radio_[line].radioValueNumber_++;
                                    }
                                }
                                break;
                        }
                        returnData = returnData + nextInput;
                    }
                }
            }
        }
    }
    return returnData;
}

wxString Usbcard::getEmailForm(wxString *error)
{
    int inputNumber;
    wxString returnData = "", nextInput;
    char characterValue;
    
    wxString returnCode = " ";
    returnCode[0] = 0x1a;
    
    *error = "";
    for (int screen=1; screen<numberOfUrlScreens_; screen++)
    {
        for (int i=0x400; i<0x7c0; i++)
        {
            inputNumber = urlScreens[screen].screenData[i];
            if (inputNumber != 0)
            {
                if (urlScreens[screen].inputInfo[inputNumber-1].inputType != INPUT_TYPE_NONE && urlScreens[screen].inputInfo[inputNumber-1].inputType != INPUT_TYPE_IMAGE && urlScreens[screen].inputInfo[inputNumber-1].inputType != INPUT_TYPE_BUTTON && urlScreens[screen].inputInfo[inputNumber-1].inputType != INPUT_TYPE_SUBMIT)
                {
                    nextInput = "";
                    
                    while ((urlScreens[screen].screenData[i]) == inputNumber)
                    {
                        characterValue = urlScreens[screen].screenData[i-0x400] & 0x7f;
                        if (characterValue == 0)
                            characterValue = 32;
                        if (characterValue >='a' && characterValue <='z')
                            characterValue -= 0x20;
                        else
                        {
                            if (characterValue >='A' && characterValue <='Z')
                                characterValue += 0x20;
                        }
                        nextInput.Append(characterValue);
                        
                        if (characterValue == 0x1a)
                        {
                            characterValue = 0;
                            
                            while (characterValue == 0 && (urlScreens[screen].screenData[i]) == inputNumber)
                            {
                                i++;
                                characterValue = urlScreens[screen].screenData[i-0x400] & 0x7f;
                            }
                        }
                        else
                            i++;
                    }
                    nextInput = nextInput.Trim();
                    nextInput.Replace(returnCode, "%0d%0a");
                    
                    if (screen == 1 && inputNumber == 1)
                    {
                        wxString aliasCheck = nextInput;
                        aliasCheck.Replace("%0d%0a", "");
                        size_t numberOfAlias = p_Main->getNumberOfAlias();
                        for (size_t number=0; number<numberOfAlias; number++)
                        {
                            if (aliasCheck == p_Main->getAlias(number))
                                nextInput = p_Main->getAliasEmail(number);
                        }
                        if (aliasCheck == "")
                            *error = "No email address found";
                        if (nextInput.Find('@') == wxNOT_FOUND)
                            *error = "Invalid email address";
                    }
                    
                    if (screen == 1 && inputNumber != 3)
                        nextInput = nextInput + "[~]";
                    
                    returnData = returnData + nextInput;
                }
            }
        }
    }
    
    returnData = returnData + "%0d%0a%0d%0aThis message was sent from the COMX-35 emulated in Emma 02.";
    wxString m_MACAddress = wxMACAddressUtility::GetMACAddress();
    
    returnData = returnData + "[~]" + p_Main->getSbEmail() + "[~]" + m_MACAddress;
    
    Encrypt encrypt;
    wxString newData = encrypt.encryptUrlData(returnData);
    return "?data=" + newData;
}


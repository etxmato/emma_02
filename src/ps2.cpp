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

#include "main.h"
#include "ps2.h"

#define PS2_CYCLES    20

Byte scancodes[] = {   0,       /* 0 */
                       0,       /* 1 */
                       0,       /* 2 */
                       0,       /* 3 */
                       0,       /* 4 */
                       0,       /* 5 */
                       0,       /* 6 */
                       0,       /* 7 */
                       0x66,    /* 8 */
                       0x0d,    /* 9 */
                       0,       /* 10 */
                       0,       /* 11 */
                       0,       /* 12 */
                       0x5a,    /* 13 */
                       0,       /* 14 */
                       0,       /* 15 */
                       0,       /* 16 */
                       0,       /* 17 */
                       0,       /* 18 */
                       0,       /* 19 */
                       0,       /* 20 */
                       0,       /* 21 */
                       0,       /* 22 */
                       0,       /* 23 */
                       0,       /* 24 */
                       0,       /* 25 */
                       0,       /* 26 */
                       0x76,    /* 27 */
                       0,       /* 28 */
                       0,       /* 29 */
                       0,       /* 30 */
                       0,       /* 31 */
                       0x29,    /* 32 */
                       0x16,    /* 33 */
                       0x52,    /* 34 */
                       0x26,    /* 35 */
                       0x25,    /* 36 */
                       0x2e,    /* 37 */
                       0x3d,    /* 38 */
                       0x52,    /* 39 */
                       0x46,    /* 40 */
                       0x45,    /* 41 */
                       0x3e,    /* 42 */
                       0x55,    /* 43 */
                       0x41,    /* 44 */
                       0x4e,    /* 45 */
                       0x49,    /* 46 */
                       0x4a,    /* 47 */
                       0x45,    /* 48 */
                       0x16,    /* 49 */
                       0x1e,    /* 50 */
                       0x26,    /* 51 */
                       0x25,    /* 52 */
                       0x2e,    /* 53 */
                       0x36,    /* 54 */
                       0x3d,    /* 55 */
                       0x3e,    /* 56 */
                       0x46,    /* 57 */
                       0x4c,    /* 58 */
                       0x4c,    /* 59 */
                       0x41,    /* 60 */
                       0x55,    /* 61 */
                       0x49,    /* 62 */
                       0x4a,    /* 63 */
                       0x1e,    /* 64 */
                       0x1c,    /* 65 */
                       0x32,    /* 66 */
                       0x21,    /* 67 */
                       0x23,    /* 68 */
                       0x24,    /* 69 */
                       0x2b,    /* 70 */
                       0x34,    /* 71 */
                       0x33,    /* 72 */
                       0x43,    /* 73 */
                       0x3b,    /* 74 */
                       0x42,    /* 75 */
                       0x4b,    /* 76 */
                       0x3a,    /* 77 */
                       0x31,    /* 78 */
                       0x44,    /* 79 */
                       0x4d,    /* 80 */
                       0x15,    /* 81 */
                       0x2d,    /* 82 */
                       0x1b,    /* 83 */
                       0x2c,    /* 84 */
                       0x3c,    /* 85 */
                       0x2a,    /* 86 */
                       0x1d,    /* 87 */
                       0x22,    /* 88 */
                       0x35,    /* 89 */
                       0x1a,    /* 90 */
                       0x54,    /* 91 */
                       0x5d,    /* 92 */
                       0x5b,    /* 93 */
                       0x36,    /* 94 */
                       0x4e,    /* 95 */
                       0x0e,    /* 96 */
                       0x1c,    /* 97 */
                       0x32,    /* 98 */
                       0x21,    /* 99 */
                       0x23,    /* 100 */
                       0x24,    /* 101 */
                       0x2b,    /* 102 */
                       0x34,    /* 103 */
                       0x33,    /* 104 */
                       0x43,    /* 105 */
                       0x3b,    /* 106 */
                       0x42,    /* 107 */
                       0x4b,    /* 108 */
                       0x3a,    /* 109 */
                       0x31,    /* 110 */
                       0x44,    /* 111 */
                       0x4d,    /* 112 */
                       0x15,    /* 113 */
                       0x2d,    /* 114 */
                       0x1b,    /* 115 */
                       0x2c,    /* 116 */
                       0x3c,    /* 117 */
                       0x2a,    /* 118 */
                       0x1d,    /* 119 */
                       0x22,    /* 120 */
                       0x35,    /* 121 */
                       0x1a,    /* 122 */
                       0x54,    /* 123 */
                       0x5d,    /* 124 */
                       0x5b,    /* 125 */
                       0x0e,    /* 126 */
                       0        /* 127 */
                 };

Ps2::Ps2()
{
	usePS2_ = false;
}

bool Ps2::keyDownPs2(int keycode)
{
	if (!usePS2_)  return false;

	Word scancode = getScanCode(keycode);
    if ((scancode >> 8 ) > 0) addToBuffer(scancode>>8);
    addToBuffer(scancode & 0xff);
	return true;
}

bool Ps2::keyUpPs2(int keycode)
{
	if (!usePS2_)  return false;

	Word scancode = getScanCode(keycode);
    if ((scancode >> 8 ) > 0) addToBuffer(scancode>>8);
    addToBuffer(0xf0);
    addToBuffer(scancode & 0xff);
	return true;
}

void  Ps2::configurePs2(bool ps2Interrupt, ElfPortConfiguration portConf) 
{
//	int input, output, efPort;

	ps2Interrupt_ = ps2Interrupt;

	usePS2_ = true;

	wxString runningComp = p_Main->getRunningComputerStr();

//	input = p_Main->getConfigItem(runningComp +"/Ps2KeyboardInput", 7l);
//	output = p_Main->getConfigItem(runningComp +"/Ps2KeyboardOutput", 7l);
//	efPort = p_Main->getConfigItem(runningComp +"/Ps2KeyboardEf", 3l);

	p_Computer->setInType(portConf.ps2KeyboardInput, PS2IN);
	p_Computer->setOutType(portConf.ps2KeyboardOutput, PS2OUT);
	p_Computer->setEfType(portConf.ps2KeyboardEf, PS2EF); 
	p_Computer->setCycleType(KEYCYCLE, PS2CYCLE);
	ps2KeyCycle_ = -1;
	ps2KeyStart_ = 0;
	ps2KeyEnd_ = 0;
	ps2KeyboardMode_ = 'X';
	ps2Cycles_ = 0;
	ps2Port_ = 1;
 	ps2CValue_ = 3;
	ps2KValue_ = 3;

	wxString printBuffer;
	p_Main->message("Configuring PS2 Keyboard");

	printBuffer.Printf("	Output %d: write data, input %d: read data, EF %d: clock signal\n", portConf.ps2KeyboardOutput, portConf.ps2KeyboardInput , portConf.ps2KeyboardEf);
	p_Main->message(printBuffer);
}

Byte Ps2::efPs2() 
{
	return(ps2CValue_ & 2) ?((ps2KValue_ & 2) ? 1 : 0) : 0;
}

Byte Ps2::inPs2() 
{
	Byte ret;
	ret = (ps2CValue_ & 1) ?(ps2KValue_ & 1) : 0;
	ret |= (ps2CValue_ & 2) ?(ps2KValue_ & 2) : 0;
	return ret;
}

void Ps2::outPs2(Byte value) 
{
	ps2CValue_ = value;
	if ((ps2CValue_ & 2) == 0) 
	{
		ps2KeyboardMode_ = 'X';
		ps2KValue_ = 3;
		ps2Cycles_ = p_Computer->getCpuCycles();
		ps2Port_ = 1;
	}
}

void Ps2::cyclePs2() 
{
	if (ps2KeyboardMode_ == 'X' &&(ps2CValue_ & 2)) 
	{
		if (ps2KeyStart_ == ps2KeyEnd_) 
			return;
		ps2Value_ = ps2Buffer_[ps2KeyStart_++];
		if (ps2KeyStart_ == 256) 
			ps2KeyStart_ = 0;
		ps2KeyboardMode_ = 'I';
		ps2Cycles_ = PS2_CYCLES;
		ps2KeyCycle_ = 0;
		ps2KValue_ = 2;
		ps2Parity_ = 1;
	}
	if (ps2KeyboardMode_ == 'I') 
	{
		if ((ps2Cycles_--) == 0) 
		{
			ps2KValue_ ^= 2;
			if (ps2KValue_ & 2) 
				p_Computer->debugTrace("PS2 Clock High");
			else 
				p_Computer->debugTrace("PS2 Clock Low");
			if (ps2Interrupt_ &&(ps2KValue_ & 2) == 0) 
				p_Computer->interrupt();
			ps2Cycles_ = PS2_CYCLES;
			if (ps2KeyCycle_ >= 11) 
				ps2KValue_ |= 2;
			if ((ps2KValue_ & 2) == 2) 
			{
				ps2KValue_ &= 2;
				if (ps2KeyCycle_ < 8) 
				{
					ps2Parity_ += (ps2Value_ & 1) ? 1 : 0;
					ps2KValue_ |= (ps2Value_ & 1) ? 1 : 0;
					ps2Value_ >>= 1;
				}
				if (ps2KeyCycle_ == 8) 
				{
					ps2KValue_ |= (ps2Parity_ & 1) ? 1 : 0;
				}
				if (ps2KeyCycle_ == 9) 
				{
					ps2KValue_ |= 1;
				}
				if (ps2KeyCycle_ == 10) 
				{
					ps2KeyboardMode_ = 'R';
					ps2Cycles_ = PS2_CYCLES;
					ps2KValue_ = 3;
				}
				ps2KeyCycle_++;
			}
		}
	}
	if (ps2KeyboardMode_ == 'R') 
	{
		if (ps2Cycles_ == 0)
			ps2KeyboardMode_ = 'X';
		else
			ps2Cycles_--;
	}
	if (ps2KeyboardMode_ == 'O') 
	{
    }
}

Word Ps2::getScanCode(Word value) 
{
	switch(value) 
	{
		case WXK_F1:return 0x05;
		case WXK_F2:return 0x06;
		case WXK_F3:return 0x04;
		case WXK_F4:return 0x0c;
		case WXK_F5:return 0x03;
		case WXK_F6:return 0x0b;
		case WXK_F7:return 0x83;
		case WXK_F8:return 0x0a;
		case WXK_F9:return 0x01;
		case WXK_F10:return 0x09;
		case WXK_F11:return 0x78;
		case WXK_F12:return 0x07;
		case WXK_BACK:return 0x66;
		case WXK_SHIFT: return 0x12;
//		case KEY_SHIFT_R: return 0x59;
		case WXK_CONTROL: return 0x14;
//		case KEY_CTRL_R: return 0xe014;
		case WXK_ALT: return 0x11;
//		case KEY_ALT_R: return 0xe011;
		case WXK_TAB:return 0x0d;
		case WXK_CAPITAL:return 0x58;
		case WXK_UP: return 0xe075;
		case WXK_DOWN: return 0xe072;
		case WXK_LEFT: return 0xe06b;
		case WXK_RIGHT: return 0xe074;
		case WXK_NUMPAD_UP: return 0x75;
		case WXK_NUMPAD_DOWN: return 0x72;
		case WXK_NUMPAD_LEFT: return 0x6b;
		case WXK_NUMPAD_RIGHT: return 0x74;
		case WXK_INSERT: return 0xe070;
		case WXK_DELETE: return 0xe071;
		case WXK_HOME: return 0xe06c;
		case WXK_END: return 0xe069;
		case WXK_PAGEUP: return 0xe07d;
		case WXK_PAGEDOWN: return 0xe07a;
		case WXK_NUMPAD0: return 0x70;
		case WXK_NUMPAD1: return 0x69;
		case WXK_NUMPAD2: return 0x72;
		case WXK_NUMPAD3: return 0x7a;
		case WXK_NUMPAD4: return 0x6b;
		case WXK_NUMPAD5: return 0x73;
		case WXK_NUMPAD6: return 0x74;
		case WXK_NUMPAD7: return 0x6c;
		case WXK_NUMPAD8: return 0x75;
		case WXK_NUMPAD9: return 0x7d;
		case WXK_NUMPAD_HOME: return 0x6c;
		case WXK_NUMPAD_END: return 0x69;
		case WXK_NUMPAD_PAGEUP: return 0x7d;
		case WXK_NUMPAD_PAGEDOWN: return 0x7a;
		case WXK_NUMPAD_ENTER: return 0xe05a;
		case WXK_NUMPAD_ADD: return 0x79;
		case WXK_NUMPAD_SUBTRACT: return 0x7b;
		case WXK_NUMPAD_MULTIPLY: return 0x7c;
		case WXK_NUMPAD_DIVIDE: return 0xe04a;
		case WXK_NUMLOCK: return 0x77;
	}
	if (value < 128) return scancodes[value];
	return 0;
}

void Ps2::addToBuffer(Byte value) 
{
	if (ps2KeyEnd_ != ps2KeyStart_-1 ||(ps2KeyEnd_ ==255 && ps2KeyStart_ != 0)) 
	{
		ps2Buffer_[ps2KeyEnd_++] = value;
		if (ps2KeyEnd_ == 256) ps2KeyEnd_ = 0;
	}
}

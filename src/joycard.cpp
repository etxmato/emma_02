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
#include "joycard.h"

Joycard::Joycard()
{
}

Byte Joycard::stick1() 
{
	Byte joystick;

	joystick = 0;

	if (wxGetKeyState(WXK_CAPITAL))
	{
		if (wxGetKeyState((wxKeyCode)81))  joystick = 5;
		if (wxGetKeyState((wxKeyCode)87))  joystick = 1;
		if (wxGetKeyState((wxKeyCode)65))  joystick = 4;
		if (wxGetKeyState((wxKeyCode)68))  joystick = 8;
		if (wxGetKeyState((wxKeyCode)88))  joystick = 2;
		if (wxGetKeyState((wxKeyCode)69))  joystick = 9;
		if (wxGetKeyState((wxKeyCode)67))  joystick = 10;
		if (wxGetKeyState((wxKeyCode)90)) joystick = 6;
	}
	joystick |= 0xf0;
	return joystick;
}

Byte Joycard::stick2() 
{
	Byte joystick;

	joystick = 0;
	if (wxGetKeyState(WXK_NUMPAD_HOME) || wxGetKeyState(WXK_NUMPAD7))  joystick = 5;
	if (wxGetKeyState(WXK_NUMPAD_UP) || wxGetKeyState(WXK_NUMPAD8))  joystick = 1;
	if (wxGetKeyState(WXK_NUMPAD_LEFT) || wxGetKeyState(WXK_NUMPAD4))  joystick = 4;
	if (wxGetKeyState(WXK_NUMPAD_RIGHT) || wxGetKeyState(WXK_NUMPAD6))  joystick = 8;
	if (wxGetKeyState(WXK_NUMPAD_DOWN) || wxGetKeyState(WXK_NUMPAD2))  joystick = 2;
	if (wxGetKeyState(WXK_NUMPAD_PAGEUP) || wxGetKeyState(WXK_NUMPAD9))  joystick = 9;
	if (wxGetKeyState(WXK_NUMPAD_PAGEDOWN) || wxGetKeyState(WXK_NUMPAD3))  joystick = 10;
	if (wxGetKeyState(WXK_NUMPAD_END) || wxGetKeyState(WXK_NUMPAD1))  joystick = 6;
	if (wxGetKeyState(WXK_NUMPAD_INSERT) || wxGetKeyState(WXK_NUMPAD0))  joystick += 16;
	if (wxGetKeyState(WXK_CAPITAL))
		if (wxGetKeyState((wxKeyCode)32))  joystick += 32;
	joystick |= 0xc0;
	return joystick;
}
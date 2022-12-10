/*
 *******************************************************************
 *** This software is copyright 2022 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 ***                                                             ***
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
#include "bitkeypad.h"

BitKeypad::BitKeypad()
{
}

void BitKeypad::configure(IoConfiguration ioConf, int pad)
{
    ioConfiguration_ = ioConf;
    pad_ = pad;
    wxString printBuffer;
 
    wxString ioGroup = "";
    if (ioConfiguration_.bitKeypad[pad_].ioGroup != -1)
    {
        ioGroup.Printf(" on group %X", ioConfiguration_.bitKeypad[pad_].ioGroup);
    }
    
    p_Main->message("Configuring keypad" + ioConfiguration_.bitKeypad[pad_].number + ioGroup);
    printBuffer.Printf("    Input %d: read data\n", ioConfiguration_.bitKeypad[pad_].inp);
    p_Main->message(printBuffer);
 
    inputValue_ = 0;
}

void BitKeypad::keyDown(int keycode)
{
    for (int key=0; key<ioConfiguration_.bitKeypad[pad_].numberOfKeys; key++)
    {
        if (ioConfiguration_.bitKeypad[pad_].bitkey[key].value == keycode)
            inputValue_ |= ioConfiguration_.bitKeypad[pad_].bitkey[key].bitMaskPressed;
    }
}

void BitKeypad::keyUp(int keycode)
{
    for (int key=0; key<ioConfiguration_.bitKeypad[pad_].numberOfKeys; key++)
    {
        if (ioConfiguration_.bitKeypad[pad_].bitkey[key].value == keycode)
            inputValue_ &= ioConfiguration_.bitKeypad[pad_].bitkey[key].bitMaskReleased;
    }
}

Byte BitKeypad::in()
{
    if (ioConfiguration_.bitKeyPressed == 1)
        return inputValue_;
    else
        return inputValue_^0xff;
}

void BitKeypad::cid1Bit8(bool set)
{
    if (set)
        inputValue_ |= 128;
    else
        inputValue_ &= 0x7f;
}

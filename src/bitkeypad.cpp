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

void BitKeypad::configure(BitKeypadConfiguration bitKeypadConfiguration)
{
    bitKeypadConfiguration_ = bitKeypadConfiguration;
    
    p_Computer->setInType(&bitKeypadConfiguration_.ioGroupVector, bitKeypadConfiguration_.input , "read data keypad / row" + p_Main->getGroupMessage(&bitKeypadConfiguration_.ioGroupVector));
 
    inputValue_ = 0;
    lastKey_ = 0;
}

void BitKeypad::keyDown(int keycode)
{
    if (keycode != lastKey_ || bitKeypadConfiguration_.repeat)
    {
        for (int key=0; key<bitKeypadConfiguration_.numberOfKeys; key++)
        {
            if (bitKeypadConfiguration_.bitkey[key].value == keycode)
                inputValue_ |= bitKeypadConfiguration_.bitkey[key].bitMaskPressed;
        }
    }
    lastKey_ = keycode;
}

bool BitKeypad::keyDownCtrlV(int keycode)
{
    bool keyFound = false;
    
    for (int key=0; key<bitKeypadConfiguration_.numberOfKeys; key++)
    {
        if (bitKeypadConfiguration_.bitkey[key].value == keycode)
        {
            inputValue_ |= bitKeypadConfiguration_.bitkey[key].bitMaskPressed;
            keyFound = true;
        }
    }
    for (int key=0; key<bitKeypadConfiguration_.numberOfModKeys; key++)
    {
        if (bitKeypadConfiguration_.bitModkey[key].value == keycode)
        {
            p_Computer->setEfKeyValue(bitKeypadConfiguration_.bitModkey[key].ef, 0);
            inputValue_ |= bitKeypadConfiguration_.bitModkey[key].bitMaskPressed;
            keyFound = true;
        }
    }
    return keyFound;
}

void BitKeypad::keyUp(int keycode)
{
    lastKey_ = 0;
    for (int key=0; key<bitKeypadConfiguration_.numberOfKeys; key++)
    {
        if (bitKeypadConfiguration_.bitkey[key].value == keycode)
            inputValue_ &= bitKeypadConfiguration_.bitkey[key].bitMaskReleased;
    }
}

void BitKeypad::keyUpCtrlV(int keycode)
{
    for (int key=0; key<bitKeypadConfiguration_.numberOfKeys; key++)
    {
        if (bitKeypadConfiguration_.bitkey[key].value == keycode)
            inputValue_ &= bitKeypadConfiguration_.bitkey[key].bitMaskReleased;
    }
}

void BitKeypad::keysUp()
{
    inputValue_ = 0;
}

Byte BitKeypad::in()
{
    Byte returnValue;
    
    if (bitKeypadConfiguration_.bitKeyPressed == 1)
        returnValue = inputValue_;
    else
        returnValue = inputValue_^0xff;
  
//    if ((p_Computer->getScratchpadRegister(3) & 0xf000) == 0)
 //       inputValue_ = 0; // VIP2K specific...
    return returnValue;
}

void BitKeypad::cid1Bit8(bool set)
{
    if (set)
        inputValue_ |= 128;
    else
        inputValue_ &= 0x7f;
}

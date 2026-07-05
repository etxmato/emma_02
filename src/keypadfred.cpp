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
#include "keypadfred.h"

KeypadFred::KeypadFred()
{
    keyCode_ = WXK_NONE;
    keyValue_ = 0;
    shiftPressed_ = 0;
    nextNybble_ = 'H';
    inpMode_ = INP_MODE_NONE;
    
    directionKey_ = 0;
    fireKeyA_ = 1;
    fireKeyB_ = 1;
    coinKey_ = 0;
    efState_ = 1;

    keyCycles_ = 20000;
}

void KeypadFred::configure(FredKeypadConfiguration fredKeypadConfiguration, int keyDefA1[], int keyDefA2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefA1_[i] = keyDefA1[i];
        keyDefA2_[i] = keyDefA2[i];
    }
    fredKeypadConfiguration_ = fredKeypadConfiguration;
    wxString printBuffer;
 
    p_Main->configureMessage(&fredKeypadConfiguration.ioGroupVector, "hex keypad & card");
    switch(fredKeypadConfiguration.keypad_mode)
    {
        case FRED_HEX_MODE:
            p_Main->message("	Hex mode");
        break;

        case FRED_BYTE_MODE:
            p_Main->message("	Byte mode");
        break;

        case FRED_HEX_PULSE_MODE:
            p_Main->message("	Hex pulse mode");
        break;

    }
    p_Computer->setOutType(&fredKeypadConfiguration.ioGroupVector, fredKeypadConfiguration.output, "1 = program mode, 2 = direct mode");
    printBuffer.Printf("read data keypad (if mode = %d)", fredKeypadConfiguration.input_mode);
    p_Computer->setInType(&fredKeypadConfiguration.ioGroupVector, fredKeypadConfiguration.inputKey, printBuffer);

    printBuffer.Printf("data ready (if mode = %d)\n", fredKeypadConfiguration.ef_mode);
    p_Computer->setEfType(&fredKeypadConfiguration.ioGroupVector, fredKeypadConfiguration.efKey, printBuffer);

    if (fredKeypadConfiguration.coinArcadeControl)
    {
        coinKey_ = 1;
        p_Main->configureMessage(&fredKeypadConfiguration.ioGroupVector, "RCA Video Coin Arcade controls");
        printBuffer.Printf("fire player A (if mode != %d)", fredKeypadConfiguration.ef_mode);
        p_Computer->setEfType(&fredKeypadConfiguration.ioGroupVector, fredKeypadConfiguration.efKey, printBuffer);
        p_Computer->setEfType(&fredKeypadConfiguration.ioGroupVector, fredKeypadConfiguration.efFireB, "fire player B");
        p_Computer->setEfType(&fredKeypadConfiguration.ioGroupVector, fredKeypadConfiguration.efCoin, "coin");
        p_Computer->setInType(&fredKeypadConfiguration.ioGroupVector, fredKeypadConfiguration.inputCoin, "direction keys & coin reset");

        p_Main->message("");

        keyDefCoin_ = p_Main->getDefaultCoinArcadeKeys(keyDefA_, keyDefB_);
    }

    p_Computer->setCycleType(CYCLE_TYPE_KEYBOARD, KEY_FRED_CYCLE);
}

void KeypadFred::keyDown(int keycode)
{
    if (keycode == WXK_SHIFT)
        shiftPressed_ = 0x10;
    
    for (int i=0; i<16; i++)
    {
        if (keycode == keyDefA1_[i])
        {
            keyCode_ = (wxKeyCode)keycode;
            keyFound(i);
        }
        if (keycode == keyDefA2_[i])
        {
            keyCode_ = (wxKeyCode)keycode;
            keyFound(i);
        }
    }
    if (efState_ == 1)
        keyCode_ = WXK_NONE;

    if (fredKeypadConfiguration_.coinArcadeControl)
    {
        if (keycode == keyDefCoin_)
            coinKey_ = 0;
        
        if (keycode == keyDefA_[KEY_UP])
            directionKey_ |= 0x02;
        if (keycode == keyDefA_[KEY_LEFT])
            directionKey_ |= 0x01;
        if (keycode == keyDefA_[KEY_RIGHT])
            directionKey_ |= 0x04;
        if (keycode == keyDefA_[KEY_DOWN])
            directionKey_ |= 0x08;
        if (keycode == keyDefA_[KEY_FIRE])
            fireKeyA_ = 0;
        
        if (keycode == keyDefB_[KEY_UP])
            directionKey_ |= 0x20;
        if (keycode == keyDefB_[KEY_LEFT])
            directionKey_ |= 0x10;
        if (keycode == keyDefB_[KEY_RIGHT])
            directionKey_ |= 0x40;
        if (keycode == keyDefB_[KEY_DOWN])
            directionKey_ |= 0x80;
        if (keycode == keyDefB_[KEY_FIRE])
            fireKeyB_ = 0;

        if (keycode == '1')
            directionKey_ |= 0x01;
        if (keycode == '2')
            directionKey_ |= 0x02;
        if (keycode == '3')
            directionKey_ |= 0x04;
        if (keycode == '4')
            directionKey_ |= 0x08;
    }
}

void KeypadFred::keyUp(int keycode)
{
    if (keycode == WXK_SHIFT)
        shiftPressed_ = 0;
    else
        efState_ = 1;

    if (fredKeypadConfiguration_.coinArcadeControl)
    {
        if (keycode == keyDefA_[KEY_UP])
            directionKey_ &= 0xFD;
        if (keycode == keyDefA_[KEY_LEFT])
            directionKey_ &= 0xFE;
        if (keycode == keyDefA_[KEY_RIGHT])
            directionKey_ &= 0xFB;
        if (keycode == keyDefA_[KEY_DOWN])
            directionKey_ &= 0xF7;
        if (keycode == keyDefA_[KEY_FIRE])
            fireKeyA_ = 1;
        
        if (keycode == keyDefB_[KEY_UP])
            directionKey_ &= 0xDF;
        if (keycode == keyDefB_[KEY_LEFT])
            directionKey_ &= 0xEF;
        if (keycode == keyDefB_[KEY_RIGHT])
            directionKey_ &= 0xBF;
        if (keycode == keyDefB_[KEY_DOWN])
            directionKey_ &= 0x7F;
        if (keycode == keyDefB_[KEY_FIRE])
            fireKeyB_ = 1;

        if (keycode == '1')
            directionKey_ &= 0xFE;
        if (keycode == '2')
            directionKey_ &= 0xFD;
        if (keycode == '3')
            directionKey_ &= 0xFB;
        if (keycode == '4')
            directionKey_ &= 0xF7;
    }
}

void KeypadFred::keyFound(int key)
{
    if (fredKeypadConfiguration_.keypad_mode == FRED_HEX_PULSE_MODE)
        keyCycles_ = 2000;
    else
        keyCycles_ = 20000;

    if (fredKeypadConfiguration_.keypad_mode == FRED_HEX_MODE || fredKeypadConfiguration_.keypad_mode == FRED_HEX_PULSE_MODE )
    {
        if (inpMode_ == INP_MODE_KEY_DIRECT)
            p_Computer->dmaIn(key | shiftPressed_);
        else
        {
            keyValue_ = key | shiftPressed_;
            efState_ = 0;
        }
    }
    else
    {
        if (nextNybble_ == 'H')
        {
            nextNybble_ = 'L';
            keyValue_ = key << 4;
        }
        else
        {
            nextNybble_ = 'H';
            if (inpMode_ == INP_MODE_KEY_DIRECT)
                p_Computer->dmaIn(keyValue_ | key);
            else
            {
                keyValue_ = keyValue_ | key;
                efState_ = 0;
            }
        }
    }

}

void KeypadFred::cycle()
{
    if (keyCycles_ > 0)
    {
        keyCycles_--;
        return;
    }
    if (fredKeypadConfiguration_.keypad_mode == FRED_HEX_PULSE_MODE)
    {
        efState_ = 1;
        keyCycles_ = 2000;
    }
    else
    {
        if (keyCode_ != WXK_NONE)
        {
            if (wxGetKeyState(keyCode_))
            {
                efState_ = 0;
                keyCycles_ = 20000;
            }
            else
            {
                efState_ = 1;
                keyCycles_ = 20000;
            }
        }
    }
}

void KeypadFred::setInputMode(int inpMode)
{
    inpMode_ = inpMode;
}

Byte KeypadFred::efHexFireA()
{
    if (inpMode_ == fredKeypadConfiguration_.ef_mode)
        return efState_^fredKeypadConfiguration_.efKey.reverse;
    else
    {
        if (fredKeypadConfiguration_.coinArcadeControl)
            return fireKeyA_^fredKeypadConfiguration_.efKey.reverse;
        else
            return 1;
    }
}

Byte KeypadFred::efFireB()
{
    if (fredKeypadConfiguration_.coinArcadeControl)
        return fireKeyB_^fredKeypadConfiguration_.efFireB.reverse;
    else
        return 1;
}

Byte KeypadFred::efCoin()
{
    return coinKey_^fredKeypadConfiguration_.efCoin.reverse;
}

Byte KeypadFred::inHex()
{
    efState_ = 1;
    return keyValue_;
}

Byte KeypadFred::inCoin()
{
    coinKey_ = 1;
    return directionKey_;
}

void KeypadFred::setKeyPress(Byte value)
{
    keyValue_ = value;
    efState_ = 0;
}

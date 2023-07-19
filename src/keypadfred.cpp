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
    coinKey_ = 1;
}

void KeypadFred::configure(IoConfiguration ioConf, int keyDefA1[], int keyDefA2[])
{
    for (int i=0; i<16; i++)
    {
        keyDefA1_[i] = keyDefA1[i];
        keyDefA2_[i] = keyDefA2[i];
    }
    ioConfiguration_ = ioConf;
    wxString printBuffer;
 
    wxString ioGroup = "";
    if (ioConfiguration_.fredKeypad.ioGroup != -1)
    {
        ioGroup.Printf(" on group %d", ioConfiguration_.fredKeypad.ioGroup);
    }

    p_Main->message("Configuring hex keypad & card" + ioGroup);
    switch(ioConfiguration_.fredKeypad.keypad_mode)
    {
        case FRED_HEX_MODE:
            p_Main->message("    Hex mode");
        break;

        case FRED_BYTE_MODE:
            p_Main->message("    Byte mode");
        break;

        case FRED_HEX_PULSE_MODE:
            p_Main->message("    Hex pulse mode");
        break;

    }
    printBuffer.Printf("    Output %d: 1 = program mode, 2 = direct mode", ioConfiguration_.fredKeypad.output);
    p_Main->message(printBuffer);
    p_Computer->setOutType(ioConfiguration_.fredKeypad.ioGroup + 1, ioConfiguration_.fredKeypad.output, FREDKEYMODE);

    printBuffer.Printf("    Input %d: read data keypad (if mode = %d)", ioConfiguration_.fredKeypad.inpKey, ioConfiguration_.fredKeypad.input_mode);
    p_Main->message(printBuffer);
    p_Computer->setInType(ioConfiguration_.fredKeypad.inpKey, FREDINP0);

    printBuffer.Printf("    EF %d: data ready (if mode = %d)\n", ioConfiguration_.fredKeypad.efKey, ioConfiguration_.fredKeypad.ef_mode);
    p_Main->message(printBuffer);
    p_Computer->setEfType(ioConfiguration_.fredKeypad.efKey, FREDEF1);

    if (ioConfiguration_.fredKeypad.coinArcadeControl)
    {
        p_Main->message("Configuring RCA Video Coin Arcade controls");
        
        printBuffer.Printf("    EF %d: fire player A, EF %d: fire player B, EF %d: coin", ioConfiguration_.fredKeypad.efKey, ioConfiguration_.fredKeypad.efFireB, ioConfiguration_.fredKeypad.efCoin);
        p_Main->message(printBuffer);
        p_Computer->setEfType(ioConfiguration_.fredKeypad.efFireB, COINARCADEEF3);

        printBuffer.Printf("    Input %d: direction keys & coin reset", ioConfiguration_.fredKeypad.inpCoin);
        p_Main->message(printBuffer);
        p_Computer->setInType(ioConfiguration_.fredKeypad.inpCoin, COINARCADEINPKEY6);

        keyDefCoin_ = p_Main->getDefaultCoinArcadeKeys(keyDefA_, keyDefB_);
    }

    p_Computer->setCycleType(KEYCYCLE, FREDKEYCYCLE);
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

    if (ioConfiguration_.fredKeypad.coinArcadeControl)
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

    if (ioConfiguration_.fredKeypad.coinArcadeControl)
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
    if (ioConfiguration_.fredKeypad.keypad_mode == FRED_HEX_PULSE_MODE)
        keyCycles_ = 2000;
    else
        keyCycles_ = 20000;

    if (ioConfiguration_.fredKeypad.keypad_mode == FRED_HEX_MODE || ioConfiguration_.fredKeypad.keypad_mode == FRED_HEX_PULSE_MODE )
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
    if (ioConfiguration_.fredKeypad.keypad_mode == FRED_HEX_PULSE_MODE)
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
    if (inpMode_ == ioConfiguration_.fredKeypad.ef_mode)
        return efState_;
    else
    {
        if (ioConfiguration_.fredKeypad.coinArcadeControl)
            return fireKeyA_;
        else
            return 1;
    }
}

Byte KeypadFred::efFireB()
{
    if (ioConfiguration_.fredKeypad.coinArcadeControl)
        return fireKeyB_;
    else
        return 1;
}

Byte KeypadFred::efCoin()
{
    return coinKey_;
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

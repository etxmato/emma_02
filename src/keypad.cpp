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

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#include "main.h"
#include "pushbutton.h"
#include "keypad.h"

EtiKeypad::EtiKeypad()
{
    keyClear();
    stepEf_ = 1;
}

void EtiKeypad::configure(EtiKeypadConfiguration etiKeypadConfiguration, int keyDefA1[], int keyDefA2[])
{
    etiKeypadConfiguration_ = etiKeypadConfiguration;
    for (int i=0; i<16; i++)
    {
        keyDefA1_[i] = keyDefA1[i];
        keyDefA2_[i] = keyDefA2[i];
    }
     
    p_Main->configureMessage(&etiKeypadConfiguration.ioGroupVector, "HUG1802 keypad");
    p_Computer->setOutType(&etiKeypadConfiguration.ioGroupVector, etiKeypadConfiguration.output, "write PIA");
    p_Computer->setInType(&etiKeypadConfiguration.ioGroupVector, etiKeypadConfiguration.input, "read PIA");
    p_Computer->setEfType(&etiKeypadConfiguration.ioGroupVector, etiKeypadConfiguration.ef, "step key");

    p_Main->message("");

    DataDirection_ = true;
}

void EtiKeypad::keyDown(int keycode,  wxKeyEvent& event)
{
    for (int i=0; i<16; i++)
    {
        if (keycode == keyDefA1_[i])
            eti660KeyState_[i] = true;
        if (keycode == keyDefA2_[i])
            eti660KeyState_[i] = true;
    }
}

void EtiKeypad::keyUp(int keycode, wxKeyEvent& WXUNUSED(event))
{
    for (int i=0; i<16; i++)
    {
        if (keycode == keyDefA1_[i])
            eti660KeyState_[i] = false;
        if (keycode == keyDefA2_[i])
            eti660KeyState_[i] = false;
    }
}

void EtiKeypad::onInButtonPress()
{
    stepEf_ = 0;
}

void EtiKeypad::onInButtonRelease()
{
    stepEf_ = 1;
}

Byte EtiKeypad::ef()
{
    return stepEf_^etiKeypadConfiguration_.ef.reverse;
}

Byte EtiKeypad::in(Word address)
{
    Byte ret = 0xff;

    if ((address&0x3) == 0)
    {
        if (DataDirection_)    // Data Direction A
        {
//            p_Main->message("Data Direction Register: Input");
        }
        else                // Output Register
        {
            if ((outputKeyValue_&0x1) == 0)
            {
                if (eti660KeyState_[3])
                    ret &= 0x7f;
                if (eti660KeyState_[2])
                    ret &= 0xbf;
                if (eti660KeyState_[1])
                    ret &= 0xdf;
                if (eti660KeyState_[0])
                    ret &= 0xef;
            }

            if ((outputKeyValue_&0x2) == 0)
            {
                if (eti660KeyState_[7])
                    ret &= 0x7f;
                if (eti660KeyState_[6])
                    ret &= 0xbf;
                if (eti660KeyState_[5])
                    ret &= 0xdf;
                if (eti660KeyState_[4])
                    ret &= 0xef;
            }

            if ((outputKeyValue_&0x4) == 0)
            {
                if (eti660KeyState_[0xb])
                    ret &= 0x7f;
                if (eti660KeyState_[0xa])
                    ret &= 0xbf;
                if (eti660KeyState_[9])
                    ret &= 0xdf;
                if (eti660KeyState_[8])
                    ret &= 0xef;
            }

            if ((outputKeyValue_&0x8) == 0)
            {
                if (eti660KeyState_[0xf])
                    ret &= 0x7f;
                if (eti660KeyState_[0xe])
                    ret &= 0xbf;
                if (eti660KeyState_[0xd])
                    ret &= 0xdf;
                if (eti660KeyState_[0xc])
                    ret &= 0xef;
            }

            if ((outputKeyValue_&0x10) == 0)
            {
                if (eti660KeyState_[0xc])
                    ret &= 0xf7;
                if (eti660KeyState_[8])
                    ret &= 0xfb;
                if (eti660KeyState_[4])
                    ret &= 0xfd;
                if (eti660KeyState_[0])
                    ret &= 0xfe;
            }

            if ((outputKeyValue_&0x20) == 0)
            {
                if (eti660KeyState_[0xd])
                    ret &= 0xf7;
                if (eti660KeyState_[9])
                    ret &= 0xfb;
                if (eti660KeyState_[5])
                    ret &= 0xfd;
                if (eti660KeyState_[1])
                    ret &= 0xfe;
            }

            if ((outputKeyValue_&0x40) == 0)
            {
                if (eti660KeyState_[0xe])
                    ret &= 0xf7;
                if (eti660KeyState_[0xa])
                    ret &= 0xfb;
                if (eti660KeyState_[6])
                    ret &= 0xfd;
                if (eti660KeyState_[2])
                    ret &= 0xfe;
            }

            if ((outputKeyValue_&0x80) == 0)
            {
                if (eti660KeyState_[0xf])
                    ret &= 0xf7;
                if (eti660KeyState_[0xb])
                    ret &= 0xfb;
                if (eti660KeyState_[7])
                    ret &= 0xfd;
                if (eti660KeyState_[3])
                    ret &= 0xfe;
            }

            ret &= inputKeyLatch_;
            ret |= (outputKeyValue_&outputKeyLatch_);
//            p_Main->eventMessageHex(ret);
        }
    }
//    else
//        p_Main->message("Other Input");
    return ret;
}

bool EtiKeypad::out(Word address, Byte value)
{
    bool returnValue = false;
    
    if ((address&0x3) == 1)    // Control register A
    {
        if ((value&0x4) == 0x4)
        {
//            p_Main->message("Data Direction Register Selected");
            returnValue = true;
            DataDirection_ = true;
        }
        else
        {
//            p_Main->message("Output Register Selected");
            DataDirection_ = false;
        }
//        p_Main->eventMessageHex(value);
    }

    if ((address&0x3) == 0)
    {
        if (DataDirection_)    // Data Direction A
        {
//            p_Main->message("Data Direction Register");
//            p_Main->eventMessageHex(value);
            outputKeyLatch_ = value;
            inputKeyLatch_ = value^0xff;
        }
        else                // Output Register A
        {
//            p_Main->message("Output Register");
            outputKeyValue_ = value&outputKeyLatch_;
            outputKeyValue_ |= inputKeyLatch_;
//            p_Main->eventMessageHex(outputKeyValue_);
        }
    }
    return returnValue;
}

void EtiKeypad::keyClear()
{
    for (int i=0; i<16; i++)
        eti660KeyState_[i] = false;
}

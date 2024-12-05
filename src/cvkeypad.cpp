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
#include "cvkeypad.h"

CvKeypad::CvKeypad()
{
    keyPressed_ = 1;
    unreadInput_ = 1;
}

void CvKeypad::configure(CvKeypadConfiguration cvKeypadConfiguration)
{
    cvKeypadConfiguration_ = cvKeypadConfiguration;
         
    p_Main->configureMessage(&cvKeypadConfiguration.ioGroupVector, "CyberVision keypad");
    p_Computer->setInType(&cvKeypadConfiguration.ioGroupVector, cvKeypadConfiguration.input , "read data");
    p_Computer->setEfType(&cvKeypadConfiguration.ioGroupVector, cvKeypadConfiguration.ef , "data ready");

    p_Main->message("");

    keyboardCode_ = 0xff;
}

void CvKeypad::keyDown(int keycode,  wxKeyEvent& event)
{
    if (keycode == cvKeypadConfiguration_.modKeyIgnore)
        return;
    
    int input = -1;

    int keyPad = 0;
    if (event.GetModifiers() == cvKeypadConfiguration_.modKey)
        keyPad = cvKeypadConfiguration_.padMask;

    if (cvKeypadConfiguration_.modKey == WXK_CAPITAL)
    {
        if (wxGetKeyState (WXK_CAPITAL))
            keyPad = cvKeypadConfiguration_.padMask;
    }

    if (keyPressed_ == 0)
    {
        if (keyboardCode_ != keycode)
        {
            switch (keycode)
            {
                case WXK_LEFT:
                case WXK_UP:
                case WXK_RIGHT:
                case WXK_DOWN:
                    secondKeyboardCodes[keycode-WXK_LEFT] = keycode;
                    secondKeyboardCodes[5] = keyPad;
                break;
                    
                case WXK_SPACE:
                    secondKeyboardCodes[4] = keycode;
                    secondKeyboardCodes[5] = keyPad;
                break;
            }
        }
        return;
    }
        
    switch (keycode)
    {
        case WXK_NUMPAD_ENTER:
            input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_NUMPAD_ENTER_KEY];
        break;

        case WXK_BACK:
            input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_BACK_KEY];
        break;

        case WXK_ESCAPE:
            input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_ESC_KEY];
        break;

        case WXK_PAUSE:
            input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_PAUSE_KEY];
        break;

        case WXK_MENU:
            input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_MENU_KEY];
        break;

        case WXK_TAB:
            input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_TAB_KEY];
        break;

        case WXK_RETURN:
            input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_RETURN_KEY];
        break;

        case WXK_END:
            input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_END_KEY];
        break;

        case WXK_HOME:
            input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_HOME_KEY];
        break;

        case WXK_DOWN:
            input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_DOWN_KEY];
        break;

        case WXK_LEFT:
            input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_LEFT_KEY];
        break;

        case WXK_RIGHT:
            input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_RIGHT_KEY];
        break;

        case WXK_UP:
            input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_UP_KEY];
        break;
            
        case WXK_SHIFT:
        break;

        default:
            input = cvKeypadConfiguration_.keydef[keycode];
        break;
    }

    if (input == -1)
        return;

    keyboardCode_ = input | keyPad;
    keyPressed_ = 0;
    unreadInput_ = 0;
}

void CvKeypad::keyUp(int keycode, wxKeyEvent& WXUNUSED(event))
{
    if (keycode == cvKeypadConfiguration_.modKeyIgnore)
        return;

    switch(keycode)
    {
        case WXK_LEFT:
        case WXK_UP:
        case WXK_RIGHT:
        case WXK_DOWN:
            secondKeyboardCodes[keycode-WXK_LEFT] = 0;
        break;
            
        case WXK_SPACE:
            secondKeyboardCodes[4] = 0;
        break;
    }
    
    keyPressed_ = 1;

    int keyNumber = 0, newKey = 0;
    while (keyNumber != 5 && newKey == 0)
    {
        if (secondKeyboardCodes[keyNumber] != 0)
            newKey = secondKeyboardCodes[keyNumber];
        keyNumber++;
    }
    if (newKey != 0)
    {
        int input = -1;

        switch (newKey)
        {
            case WXK_SPACE:
                input = cvKeypadConfiguration_.keydef[newKey];
            break;

            case WXK_DOWN:
                input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_DOWN_KEY];
            break;
                
            case WXK_LEFT:
                input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_LEFT_KEY];
            break;
                
            case WXK_RIGHT:
                input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_RIGHT_KEY];
            break;
                
            case WXK_UP:
                input = cvKeypadConfiguration_.textKey[MATRIX_TEXT_UP_KEY];
            break;
        }
        switch (newKey)
        {
            case WXK_SPACE:
            case WXK_DOWN:
            case WXK_LEFT:
            case WXK_RIGHT:
            case WXK_UP:
                if (input == -1)
                    return;

                keyboardCode_ = newKey | secondKeyboardCodes[5];
                keyPressed_ = 0;
                unreadInput_ = 0;
            break;
        }
    }
}

Byte CvKeypad::in()
{
    Byte ret = keyboardCode_;
    
    unreadInput_ = 1;
    return ret;
}

Byte CvKeypad::ef()
{
    return unreadInput_^cvKeypadConfiguration_.ef.reverse;
}

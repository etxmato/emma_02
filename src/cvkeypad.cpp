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
}

void CvKeypad::configure(IoConfiguration ioConf)
{
    ioConfiguration_ = ioConf;
    wxString printBuffer;
 
    wxString ioGroup = "";
    if (ioConfiguration_.cvKeypad.ioGroup != -1)
    {
        ioGroup.Printf(" on group %d", ioConfiguration_.cvKeypad.ioGroup);
    }
    
    p_Main->message("Configuring CyberVision keypad" + ioGroup);

    printBuffer.Printf("    Input %d: read data", ioConfiguration_.cvKeypad.inp);
    p_Main->message(printBuffer);
 
    printBuffer.Printf("    Ef %d: data ready\n", ioConfiguration_.cvKeypad.ef);
    p_Main->message(printBuffer);

    keyboardCode_ = 0xff;
}

void CvKeypad::keyDown(int keycode,  wxKeyEvent& event)
{
    int input = -1;

    int shiftPressed = 0;
    switch (event.GetModifiers())
    {
        case wxMOD_SHIFT:
            shiftPressed = 0x80;
        break;
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
                    secondKeyboardCodes[5] = shiftPressed;
                break;
                    
                case WXK_SPACE:
                    secondKeyboardCodes[4] = keycode;
                    secondKeyboardCodes[5] = shiftPressed;
                break;
            }
        }
        return;
    }
        
    switch (keycode)
    {
        case WXK_CAPITAL:
            input = ioConfiguration_.CvKeypadTextKey[MATRIX_CAPS_KEY];
        break;

        case WXK_ESCAPE:
            input = ioConfiguration_.CvKeypadTextKey[MATRIX_ESC_KEY];
        break;

        case WXK_RETURN:
            input = ioConfiguration_.CvKeypadTextKey[MATRIX_RETURN_KEY];
        break;

        case WXK_END:
            input = ioConfiguration_.CvKeypadTextKey[MATRIX_END_KEY];
        break;

        case WXK_HOME:
            input = ioConfiguration_.CvKeypadTextKey[MATRIX_HOME_KEY];
        break;

        case WXK_DOWN:
            input = ioConfiguration_.CvKeypadTextKey[MATRIX_DOWN_KEY];
        break;

        case WXK_LEFT:
            input = ioConfiguration_.CvKeypadTextKey[MATRIX_LEFT_KEY];
        break;

        case WXK_RIGHT:
            input = ioConfiguration_.CvKeypadTextKey[MATRIX_RIGHT_KEY];
        break;

        case WXK_UP:
            input = ioConfiguration_.CvKeypadTextKey[MATRIX_UP_KEY];
        break;
            
        case WXK_SHIFT:
        break;

        default:
            input = ioConfiguration_.cvKeypad.keydef[keycode];
        break;
    }

    if (input == -1)
        return;

    keyboardCode_ = input | shiftPressed;
    keyPressed_ = 0;
}

void CvKeypad::keyUp(int keycode, wxKeyEvent& WXUNUSED(event))
{
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
                input = ioConfiguration_.cvKeypad.keydef[newKey];
            break;

            case WXK_DOWN:
                input = ioConfiguration_.CvKeypadTextKey[MATRIX_DOWN_KEY];
            break;
                
            case WXK_LEFT:
                input = ioConfiguration_.CvKeypadTextKey[MATRIX_LEFT_KEY];
            break;
                
            case WXK_RIGHT:
                input = ioConfiguration_.CvKeypadTextKey[MATRIX_RIGHT_KEY];
            break;
                
            case WXK_UP:
                input = ioConfiguration_.CvKeypadTextKey[MATRIX_UP_KEY];
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
            break;
        }
    }
}

Byte CvKeypad::in()
{
    Byte ret = keyboardCode_;
    
    keyboardCode_ = 0xff;
    return ret;
}

Byte CvKeypad::ef()
{
    if (ioConfiguration_.cvKeypad.reversed)
        return keyPressed_^0x1;
    else
        return keyPressed_;
}

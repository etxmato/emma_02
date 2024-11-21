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
#include "ps2gpio.h"

Ps2gpio::Ps2gpio()
{
}

void Ps2gpio::configurePs2gpio(bool forceUpperCase, GpioPs2KeyboardConfiguration gpioPs2KeyboardConfiguration)
{
    wxString message;
    
    gpioPs2KeyboardConfiguration_ = gpioPs2KeyboardConfiguration;
    if (gpioPs2KeyboardConfiguration.input.portNumber[0] != -1 && gpioPs2KeyboardConfiguration.ef.flagNumber != -1)
    {
        message = "PS2 Keyboard (connected via 89C4051)";
        if (gpioPs2KeyboardConfiguration.output.portNumber[0] != -1)
            message += " and Sound";
        p_Computer->setCycleType(CYCLE_TYPE_KEYBOARD, PS2_GPIO_CYCLE);
    }
    else
        message = "Sound";

    p_Main->configureMessage(&gpioPs2KeyboardConfiguration.ioGroupVector, message);

    p_Computer->setOutType(&gpioPs2KeyboardConfiguration.ioGroupVector, gpioPs2KeyboardConfiguration.output, "Write Sound Register");
    p_Computer->setInType(&gpioPs2KeyboardConfiguration.ioGroupVector, gpioPs2KeyboardConfiguration.input, "read data");

    message = "data ready flag";
    if (gpioPs2KeyboardConfiguration_.interrupt)
        message += " and using interrupt";

    p_Computer->setEfType(&gpioPs2KeyboardConfiguration.ioGroupVector, gpioPs2KeyboardConfiguration.ef, message);

    p_Main->message("");

    forceUpperCase_ = forceUpperCase;
    keyboardEf_ = 1;
    keyboardValue_ = 0;
    rawKeyCode_ = 0;
    
    escKey_ = 0;
    escKey2_ = 0;
    keyCycles_ = 500000;
    startUp_ = true;

    ps2KeyStart_ = 0;
    ps2KeyEnd_ = 0;
}

void Ps2gpio::charEventPs2gpio(int keycode)
{
    switch(keycode)
    {
        case '?':
            addKeyToBuffer(27);
            addKeyToBuffer('A');
        break;

        case 27:
            addKeyToBuffer(0x80);
        break;

        case 0xA7:
            addKeyToBuffer(0x95);
        break;

        default:
            addKeyToBuffer(keycode);
        break;
    }
    rawKeyCode_ = keycode;
}

void Ps2gpio::addKeyToBuffer(Byte value)
{
    if (ps2KeyEnd_ != ps2KeyStart_-1 || (ps2KeyEnd_ == 255 && ps2KeyStart_ != 0))
    {
        ps2Buffer_[ps2KeyEnd_++] = value;
        if (ps2KeyEnd_ == 256) ps2KeyEnd_ = 0;
    }
}

Byte Ps2gpio::efPs2gpio()
{
    return keyboardEf_^gpioPs2KeyboardConfiguration_.ef.reverse;
}

Byte Ps2gpio::inPs2gpio() 
{
    keyboardEf_ = 1;
    if (gpioPs2KeyboardConfiguration_.interrupt)
        p_Computer->requestInterrupt(INTERRUPT_TYPE_KEYBOARD, false);
    return keyboardValue_;
}

void Ps2gpio::cyclePs2gpio() 
{
    if (gpioPs2KeyboardConfiguration_.startupString.Len() > 0 && (keyboardEf_ == 1))
    {
        keyboardEf_ = 0;
        keyboardValue_ = gpioPs2KeyboardConfiguration_.startupString.GetChar(0);
        
        gpioPs2KeyboardConfiguration_.startupString = gpioPs2KeyboardConfiguration_.startupString.Right(gpioPs2KeyboardConfiguration_.startupString.Len()-1);
        if (startUp_)
            keyboardValue_ |= 0x80;
        
        startUp_ = false;
        if (gpioPs2KeyboardConfiguration_.interrupt)
            p_Computer->requestInterrupt(INTERRUPT_TYPE_KEYBOARD, true);
        return;
    }
    if (p_Computer->getCtrlvCharNum() != 0  && keyboardEf_ == 1)
    {
        keyboardValue_ = p_Computer->getCtrlvChar();
        if (keyboardValue_ != 0)
            keyboardEf_ = 0;
        return;
    }
    if (elfKeyFileOpen_ && keyboardEf_ == 1 && keyCycles_ == 0)
    {
        if (elfKeyFile_.Read(&rawKeyCode_, 1) == 0)
        {
            elfKeyFileOpen_ = false;
            elfKeyFile_.Close();
        }
        else
        {
            keyCycles_ = 1000;
            if (rawKeyCode_ == 10) rawKeyCode_ = 13;
            if (rawKeyCode_ == 13 && lastKeyCode_ == 13) rawKeyCode_ = 0;
            lastKeyCode_ = rawKeyCode_;
            if (rawKeyCode_ == 13) keyCycles_ = 130000;
        }
    }
    if (keyCycles_ > 0 && keyboardEf_ == 1)
        keyCycles_--;
//    if (rawKeyCode_<0)
//        rawKeyCode_ = 0;
//    if (keyCycles_ == 1 && escKey_ != 0)
//    {
//        rawKeyCode_ = escKey_;
//        escKey_ = 0;
//    }
    if (ps2KeyStart_ != ps2KeyEnd_ && keyCycles_ == 0 && keyboardEf_ == 1)
    {
        keyboardValue_ = ps2Buffer_[ps2KeyStart_++];
        if (ps2KeyStart_ == 256)
            ps2KeyStart_ = 0;

        if (forceUpperCase_ && keyboardValue_ >= 'a' && keyboardValue_ <= 'z')
            keyboardValue_ -= 32;
/*         switch(rawKeyCode_)
        {
            case '?':
                if (escKey2_ != 0)
                {
                    keyCycles_ = 1000;
                    escKey_ = escKey2_;
                    escKey2_ = 0;
                }
                else
                    keyboardValue_ = (Byte)rawKeyCode_;
            break;
            case WXK_NUMPAD0:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = '?';
                escKey2_ = 'p';
            break;
            case WXK_NUMPAD1:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = '?';
                escKey2_ = 'q';
            break;
            case WXK_NUMPAD2:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = '?';
                escKey2_ = 'r';
            break;
            case WXK_NUMPAD3:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = '?';
                escKey2_ = 's';
            break;
            case WXK_NUMPAD4:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = '?';
                escKey2_ = 't';
            break;
            case WXK_NUMPAD5:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = '?';
                escKey2_ = 'u';
            break;
            case WXK_NUMPAD6:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = '?';
                escKey2_ = 'v';
            break;
            case WXK_NUMPAD7:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = '?';
                escKey2_ = 'w';
            break;
            case WXK_NUMPAD8:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = '?';
                escKey2_ = 'x';
            break;
            case WXK_NUMPAD9:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = '?';
                escKey2_ = 'y';
            break;       
            case WXK_NUMPAD_DECIMAL:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = '?';
                escKey2_ = 'z';
            break;
            case WXK_NUMPAD_ADD:
                keyboardValue_ = 0;
            break;
            case WXK_NUMPAD_DIVIDE:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = 'P';
            break;
            case WXK_NUMPAD_MULTIPLY:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = 'Q';
            break;
            case WXK_NUMPAD_SUBTRACT:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = 'R';
            break;
            case WXK_NUMPAD_ENTER:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = '?';
                escKey2_ = 'M';
            break;
            case WXK_UP:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
  //              escKey_ = 'b';
//                escKey2_ = '+';
                escKey_ = 'A';
            break;
            case WXK_DOWN:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = 'B';
            break;
            case WXK_LEFT:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = 'D';
            break;
            case WXK_RIGHT:
                keyboardValue_ = 27;
                keyCycles_ = 1000;
                escKey_ = 'C';
            break;
            default:
                keyboardValue_ = (Byte)rawKeyCode_;
            break;
        }*/
        rawKeyCode_ = 0;
        keyboardEf_ = 0;
        if (gpioPs2KeyboardConfiguration_.interrupt)
            p_Computer->requestInterrupt(INTERRUPT_TYPE_KEYBOARD, true);
    }
}

void Ps2gpio::startPs2gpioKeyFile()
{
    elfKeyFileOpen_ = false;

    wxString fileName = p_Main->getKeyFile();
    if (fileName.Len() != 0)
    {
        fileName = p_Main->getKeyFileDir() + fileName;
        if (wxFile::Exists(fileName)) 
        {
            if (elfKeyFile_.Open(fileName))
            {
                elfKeyFileOpen_ = true;
            }
        }
    }
}

void Ps2gpio::closePs2gpioKeyFile()
{
    if (elfKeyFileOpen_)
    {
        elfKeyFileOpen_ = false;
        elfKeyFile_.Close();
    }
}

void Ps2gpio::setForceUpperCasePs2gpio(bool status)
{
    forceUpperCase_ = status;
}

void Ps2gpio::resetPs2gpio() 
{
    startUp_ = true;
    keyboardEf_ = 1;
    keyboardValue_ = 0;
}

void Ps2gpio::writeGpioControlRegister(Byte value)
{
    if ((value & 0x20) == 0x20) // SPEN
    {
        p_Computer->setSoundFollowQ (false);
        switch (value & 0x18)
        {
            case 0:
                p_Computer->toneElf2KOff();
            break;

            case 0x8:
                p_Computer->beepOn();
            break;

            case 0x10:
                p_Computer->setSoundFollowQ (true);
            break;

            case 0x18:
                p_Computer->toneElf2KOn();
            break;
        }
    }
    if ((value & 0x4) == 0x4) // PPIEN
    {
    }
}

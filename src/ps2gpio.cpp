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

void Ps2gpio::configurePs2gpioElf2K(int computerType) 
{
    forceUpperCase_ = p_Main->getUpperCase(computerType);
    keyboardEf_ = 1;
    keyboardValue_ = 0;
    rawKeyCode_ = 0;

    p_Computer->setInType(7, PS2GPIOIN);
    p_Computer->setEfType(2, PS2GPIOEF); 
    p_Computer->setCycleType(KEYCYCLE, PS2GPIOCYCLE);

    wxString printBuffer;
    p_Main->message("Configuring GPIO PS2 Keyboard");

    printBuffer.Printf("    Input 7: read data, EF 2: data ready flag\n");
    p_Main->message(printBuffer);

    startUp_ = 3;
    escKey_ = 0;
    escKey2_ = 0;
    keyCycles_ = 500000;
}

void Ps2gpio::configurePs2gpio(bool forceUpperCase, IoConfiguration portConf)
{
    forceUpperCase_ = forceUpperCase;
    keyboardEf_ = 1;
    keyboardValue_ = 0;
    rawKeyCode_ = 0;

    wxString printBuffer = "";
    
    if (portConf.gpioOutput != -1)
        printBuffer = "Sound";
    if (portConf.gpioInput != -1 && portConf.gpioEf != -1)
    {
        if (portConf.gpioOutput != -1)
            printBuffer = "PS2 Keyboard and Sound";
        else
            printBuffer = "PS2 Keyboard";
    }
    p_Main->message("Configuring GPIO " + printBuffer);

    if (portConf.gpioOutput != -1)
    {
        p_Computer->setOutType(portConf.gpioIoGroup+1, portConf.gpioOutput, ELF2KGPIO);
        printBuffer.Printf("    Output %d: Write GPIO Control Register", portConf.gpioOutput);
        p_Main->message(printBuffer);
    }
    if (portConf.gpioInput != -1 && portConf.gpioEf != -1)
    {
        p_Computer->setInType(portConf.gpioIoGroup+1, portConf.gpioInput, PS2GPIOIN);
        p_Computer->setEfType(portConf.gpioIoGroup+1, portConf.gpioEf, PS2GPIOEF);
        p_Computer->setCycleType(KEYCYCLE, PS2GPIOCYCLE);

        printBuffer.Printf("    Input %d: read data, EF %d: data ready flag", portConf.gpioInput, portConf.gpioEf);
        p_Main->message(printBuffer);
    }

    p_Main->message("");

    startUp_ = 3;
    escKey_ = 0;
    escKey2_ = 0;
    keyCycles_ = 500000;
}

void Ps2gpio::charEventPs2gpio(int keycode)
{
    rawKeyCode_ = keycode;
}

Byte Ps2gpio::efPs2gpio() 
{
    return keyboardEf_;
}

Byte Ps2gpio::inPs2gpio() 
{
    keyboardEf_ = 1;
    return keyboardValue_;
}

void Ps2gpio::cyclePs2gpio() 
{
    if ((startUp_ != 0) && (keyboardEf_ == 1))
    {
        keyboardEf_ = 0;
        switch (startUp_)
        {
            case 3:
                keyboardValue_ = 'K' | 0x80;
            break;
            case 2:
                keyboardValue_ = 'B';
            break;
            case 1:
                keyboardValue_ = 1;
            break;
        }
        startUp_--;
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
    if (rawKeyCode_<0) 
        rawKeyCode_ = 0;
    if (keyCycles_ == 1 && escKey_ != 0)
    {
        rawKeyCode_ = escKey_;
        escKey_ = 0;
    }
    if (rawKeyCode_ != 0 && keyCycles_ == 0) 
    {
        if (forceUpperCase_ && rawKeyCode_ >= 'a' && rawKeyCode_ <= 'z')
             rawKeyCode_ -= 32;
         switch(rawKeyCode_)
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
        }
        rawKeyCode_ = 0;
        keyboardEf_ = 0;
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
    startUp_ = 3;
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

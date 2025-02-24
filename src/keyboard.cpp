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

#include "main.h"
#include "keyboard.h"

Keyboard::Keyboard()
{
    fileToBeLoaded_ = false;
    commandText_ = "";
}

void Keyboard::configureKeyboard(AsciiKeyboardConfiguration asciiKeyboardConfiguration, AddressLocationConfiguration addressLocationConfiguration, wxString saveCommand)
{
    asciiKeyboardConfiguration_ = asciiKeyboardConfiguration;
    
    p_Main->configureMessage(&asciiKeyboardConfiguration.ioGroupVector, "Ascii Keyboard");
    p_Computer->setInType(&asciiKeyboardConfiguration.ioGroupVector, asciiKeyboardConfiguration.input, "read data");
    p_Computer->setEfType(&asciiKeyboardConfiguration.ioGroupVector, asciiKeyboardConfiguration.ef, "data ready flag");
    p_Computer->setCycleType(CYCLE_TYPE_KEYBOARD, ASCII_KEYBOARD_CYCLE);

    p_Main->message("");

    addressLocations_ = addressLocationConfiguration;
    saveCommand_ = saveCommand;

    forceUpperCase_ = p_Main->getUpperCase();

    keyboardEf_ = 1;
    keyboardValue_ = 0;
    rawKeyCode_ = 0;
    elfRunCommand_ = 0;
    elfKeyFileOpen_ = false;

    keyCycles_ = 500000;
}

void Keyboard::charEventKeyboard(int keycode)
{
    rawKeyCode_ = keycode;
}

void Keyboard::keyboardUp() 
{
    keyboardValue_ = 0;
    keyboardEf_ = 1;
}

Byte Keyboard::efKeyboard() 
{
    return keyboardEf_^asciiKeyboardConfiguration_.ef.reverse;
}

Byte Keyboard::inKeyboard() 
{
    keyboardEf_ = 1;
    return keyboardValue_;
}

void Keyboard::cycleKeyboard()
{
    if ((fileToBeLoaded_ || commandText_ != "") && (keyboardEf_ == 1))
    {
        if (p_Computer->getScratchpadRegister(p_Computer->getProgramCounter()) == 0xf1c0)
        {
            rawKeyCode_ = 13;
            keyboardEf_ = 0;
        }
        if (p_Computer->checkKeyInputAddress())
        {
            if (commandText_ != "")
            {
                rawKeyCode_ = commandText_.GetChar(0);
                commandText_ = commandText_.Right(commandText_.Len()-1);
                keyboardEf_ = 0;
            }
            else
            {
                int saveExec = p_Main->pload();
                fileToBeLoaded_ = false;
                if (saveExec != wxNOT_FOUND)
                {
                    if (p_Main->isBatchConvertActive())
                        commandText_ = saveCommand_ + "\r";
                    else
                    {
                        if (!load_)
                        {
                            if (saveExec == 0)
                                commandText_ = "run\r";
                            else
                            {
                                wxString buffer;
                                buffer.Printf("%04x", saveExec);
                                commandText_ = "call(@" + buffer + ")\r";
                            }
                        }
                    }
                    rawKeyCode_ = 0;
                }
            }
        }
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
    if (rawKeyCode_ != 0 &&(Byte)rawKeyCode_ <= 127)
    {
        if (forceUpperCase_ && rawKeyCode_ >= 'a' && rawKeyCode_ <= 'z')
             rawKeyCode_ -= 32;
        keyboardValue_ = rawKeyCode_;
        rawKeyCode_ = 0;
        keyboardEf_ = 0;
    }
}

void Keyboard::startElfKeyFile(wxString elfTypeStr)
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

void Keyboard::closeElfKeyFile()
{
    if (elfKeyFileOpen_)
    {
        elfKeyFileOpen_ = false;
        elfKeyFile_.Close();
    }
}

void Keyboard::startElfRun(bool load, wxString command)
{
    load_ = load;
    commandText_ = command;
    fileToBeLoaded_ = true;
}

void Keyboard::startCtrlV(wxString command)
{
    commandText_ = command;
    fileToBeLoaded_ = false;
}

void Keyboard::setForceUpperCaseKeyboard(bool status)
{
    forceUpperCase_ = status;
}

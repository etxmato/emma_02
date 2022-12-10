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
#include "keyb1871.h"

Keyb1871::Keyb1871()
{
}

void Keyb1871::configureKeyb1871(int WXUNUSED(computerType), IoConfiguration portConf, Locations addressLocations, wxString saveCommand)
{
    addressLocations_ = addressLocations;
    saveCommand_ = saveCommand;
    
    keyboardEf_ = 1;
    keyboardRepeatEf_ = 1;
    
    keyCycles_ = 500000;
    rawKeyCode_ = 0;

    debounceCounter_ = 0;
    keyboardCode_ = 0;
    for (int i=0; i<5; i++)
        secondKeyboardCodes[i] = 0;
    previousKeyCode_ = (wxKeyCode) 0;

    runCommand1871_ = 0;
    keyFileOpen_ = false;

    wxString runningComp = p_Main->getRunningComputerStr();

    p_Computer->setInType(portConf.keyboardInput, COMXIN);
    p_Computer->setEfType(portConf.keyboardEf, COMXEF3);
    p_Computer->setEfType(portConf.keyboardRepeatEf, COMXEF2);
    p_Computer->setCycleType(KEYCYCLE, COMXCYCLE);

    wxString printBuffer;
    p_Main->message("Configuring CDP1871 Keyboard");
    printBuffer.Printf("    Input %d: read data, EF %d: keyboard RPT, EF %d: keyboard DA\n", portConf.keyboardInput , portConf.keyboardRepeatEf, portConf.keyboardEf);
    p_Main->message(printBuffer);
}

void Keyb1871::charEventKeyb1871(int keycode)
{
    if (keyboardRepeatEf_ == 0)
        return;

    keyboardCode_ = keycode;
    keyboardEf_ = 0;
}

bool Keyb1871::keyDownExtended1871(int keycode, wxKeyEvent& event)
{
    if (keyboardRepeatEf_ == 0)
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
                break;

                case WXK_SPACE:
                    secondKeyboardCodes[4] = keycode;
                break;
            }
        }
        return false;
    }

    previousKeyCode_ = (wxKeyCode) keycode;

    return keyCheck(keycode, event.GetModifiers());
}

bool Keyb1871::keyCheck(int keycode, int modifiers)
{
    switch(keycode)
    {
        case WXK_RETURN:
            keyboardCode_ = 0x80;
            keyboardEf_ = 0;
            return true;
        break;
            
        case WXK_NUMPAD_ENTER:
            keyboardCode_ = 0x80;
            keyboardEf_ = 0;
            return true;
        break;
            
        case WXK_ESCAPE:
            keyboardCode_ = 0x81;
            keyboardEf_ = 0;
            return true;
        break;
            
        case WXK_BACK:
            keyboardCode_ = 0x86;
            keyboardEf_ = 0;
            return true;
        break;
            
        case WXK_DELETE:
            keyboardCode_ = 0x86;
            keyboardEf_ = 0;
            return true;
        break;
            
        case WXK_LEFT:
            keyboardCode_ = 0x84;
            keyboardEf_ = 0;
            return true;
        break;
            
        case WXK_RIGHT:
            keyboardCode_ = 0x83;
            keyboardEf_ = 0;
            return true;
        break;
            
        case WXK_UP:
            keyboardCode_ = 0x82;
            keyboardEf_ = 0;
            return true;
        break;
            
        case WXK_DOWN:
            keyboardCode_ = 0x85;
            keyboardEf_ = 0;
            return true;
        break;
            
        case WXK_NUMPAD_ADD:
            if (modifiers == wxMOD_SHIFT)
                keyboardCode_ = 0xfb;
            else
                keyboardCode_ = 0xdb;
            keyboardEf_ = 0;
            return true;
        break;
            
        case WXK_NUMPAD_SUBTRACT:
            if (modifiers == wxMOD_SHIFT)
                keyboardCode_ = 0xfc;
            else
                keyboardCode_ = 0xdc;
            keyboardEf_ = 0;
            return true;
        break;
    }
    return false;
}

void Keyb1871::keyUp1871(int keycode)
{
    if (p_Main->isDiagOn(XML) == 1)
    {
        if (p_Computer->getOutValue(1) != 0)
            return;
    }

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
    
    int keyNumber = 0, newKey = 0;
    while (keyNumber != 5 && newKey == 0)
    {
        if (secondKeyboardCodes[keyNumber] != 0)
            newKey = secondKeyboardCodes[keyNumber];
        keyNumber++;
    }
    if (newKey != 0)
    {
        if (!keyCheck(newKey, 0))
        {
            keyboardEf_ = 0;
            keyboardCode_ = newKey;
        }
        previousKeyCode_ = (wxKeyCode) keyboardCode_;
        keyboardRepeatEf_ = 0;
        return;
    }
 
    keyboardRepeatEf_ = 1;
//  keyboardEf_ = 1;
//  keyboardCode_ = 0;
    previousKeyCode_ = (wxKeyCode) 0;

}

Byte Keyb1871::efKeyb1871()
{
    return keyboardEf_;
}

Byte Keyb1871::efKeybRepeat1871()
{
    return keyboardRepeatEf_;
}

void Keyb1871::keyClear()
{
    keyboardRepeatEf_ = 1;
    keyboardEf_ = 1;
    keyboardCode_ = 0;
    for (int i=0; i<5; i++)
        secondKeyboardCodes[i] = 0;
    previousKeyCode_ = (wxKeyCode) 0;
}

Byte Keyb1871::inKeyb1871()
{
    Byte ret;

    if (p_Main->isDiagOn(XML) == 1)
    {
        switch (keyboardCode_)
        {
            case 'a':
            case 'r':
            case 's':
                return 0;
            break;
        }
        if (p_Computer->getOutValue(1) != 0)
            keyboardRepeatEf_ = 0;
    }
    keyboardEf_ = 1;
    ret = keyboardCode_;

    if (wxGetKeyState(previousKeyCode_))
    {
        keyboardRepeatEf_ = 0;
    }

    switch(ret)
    {
        case '@':ret = 0x20; break;
        case '#':ret = 0x23; break;
        case '\'': ret = 0x27; break;
        case '[':ret = 0x28; break;
        case ']':ret = 0x29; break;
        case ':':ret = 0x2a; break;
        case ';':ret = 0x2b; break;
        case '<':ret = 0x2c; break;
        case '=':ret = 0x2d; break;
        case '>':ret = 0x2e; break;
        case '\\':ret = 0x2f; break;
        case '.':ret = 0x3a; break;
        case ',':ret = 0x3b; break;
        case '(':ret = 0x3c; break;
        case '^':ret = 0x3d; break;
        case ')':ret = 0x3e; break;
        case '_':ret = 0x3f; break;
        case '?':ret = 0x40; break;
        case '+':ret = 0x5b; break;
        case '-':ret = 0x5c; break;
        case '*':ret = 0x5d; break;
        case '/':ret = 0x5e; break;
        case ' ':ret = 0x5f; break;
    }
    if (ret >= 0x90)  ret &= 0x7f;

    return ret;
}

void Keyb1871::cycleKeyb1871()
{
    if (debounceCounter_ > 0)
        debounceCounter_--;

    if ((runCommand1871_ != 0) && (keyboardEf_ == 1))
    {
        if (checkKeyInputAddress(p_Computer->getScratchpadRegister(p_Computer->getProgramCounter())))
        {
            if (runCommand1871_ == 1)
            {
                keyboardCode_ = 3;
                keyboardEf_ = 0;
                runCommand1871_++;
            }
            else if (runCommand1871_ == 2)
            {
                int saveExec = p_Main->pload();
                if (saveExec == 1)
                    runCommand1871_ = 0;
                else
                {
                    if (p_Main->isBatchConvertActive())
                    {
                        commandText_ = saveCommand_;
                    }
                    else
                    {
                        if (saveExec == 0)
                            commandText_ = "run";
                        else
                        {
                            wxString buffer;
                            buffer.Printf("%04x", saveExec);
                            commandText_ = "call(@" + buffer + ")";
                        }
                    }
                    keyboardCode_ = 0;
                    keyboardEf_ = 0;
                    runCommand1871_++;
                }
            }
            else
            {
                if (load_)
                    runCommand1871_ = 0;
                else
                {
                    if ((runCommand1871_ - 2) <= commandText_.Len())
                    {
                        keyboardCode_ = commandText_.GetChar(runCommand1871_ - 3);
                        keyboardEf_ = 0;
                        runCommand1871_++;
                    }
                    else
                    {
                        keyboardCode_ = 128;
                        keyboardEf_ = 0;
                        runCommand1871_ = 0;
                    }
                }
            }
        }
    }

    if (p_Computer->getCtrlvCharNum() != 0 && keyboardEf_ == 1)
    {
        if (checkKeyInputAddress(p_Computer->getScratchpadRegister(p_Computer->getProgramCounter())))
        {
            keyboardCode_ = p_Computer->getCtrlvChar();
            
            if (keyboardCode_ == 13)
                keyboardCode_ = 128;
            
            if (keyboardCode_ >= 'a' && keyboardCode_ <= 'z')
                keyboardCode_ -= 32;
            else
            {
                if (keyboardCode_ >= 'A' && keyboardCode_ <= 'Z')
                    keyboardCode_ += 32;
            }

            if (keyboardCode_ != 0)
                keyboardEf_ = 0;
        }
    }

    if ((keyFileOpen_) && (keyboardEf_ == 1))
    {
        if (checkKeyInputAddress(p_Computer->getScratchpadRegister(p_Computer->getProgramCounter())))
        {
            if (keyFile_.Read(&keyboardCode_, 1) == 0)
            {
                keyFileOpen_ = false;
                keyFile_.Close();
            }
            else
            {
                if (keyboardCode_ == 13) keyboardCode_ = 128;
                if (keyboardCode_ == 10 && lastKeyCode_ == 128) keyboardCode_ = 0;
                if (keyboardCode_ == 0xb6)  keyboardCode_ = 1;
                if (keyboardCode_ >= 'A' && keyboardCode_ <= 'Z')
                    keyboardCode_ += 32;
                else
                    if (keyboardCode_ >= 'a' && keyboardCode_ <= 'z')
                        keyboardCode_ -= 32;

                lastKeyCode_ = keyboardCode_;
                keyboardEf_ = 0;
            }
        }
    }
}

bool Keyb1871::checkKeyInputAddress(Word address)
{
// ***   for (int i : addressLocations_.keyInputAddress)
	for (std::vector<Word>::iterator i = addressLocations_.keyInputAddress.begin (); i != addressLocations_.keyInputAddress.end (); ++i)
    {
        if (address == *i)
            return true;
    }
    return false;
}

void Keyb1871::start1871KeyFile()
{
    keyFileOpen_ = false;

    wxString fileName = p_Main->getKeyFile();
    if (fileName.Len() != 0)
    {
        fileName = p_Main->getKeyFileDir() + fileName;
        if (wxFile::Exists(fileName)) 
        {
            if (keyFile_.Open(fileName))
            {
                keyFileOpen_ = true;
                keyboardEf_ = 0;
            }
        }
    }
}

void Keyb1871::close1871KeyFile()
{
    if (keyFileOpen_)
    {
        keyFileOpen_ = false;
        keyFile_.Close();
    }
}

void Keyb1871::start1871Run(bool load)
{
    load_ = load;
    if (p_Computer->getRunState() == RESETSTATE)
        runCommand1871_ = 1;
    else
        runCommand1871_ = 2;
}

Byte Keyb1871::getDiagInput()
{
    Byte ret = 0xff;
    if (keyboardEf_ == 0)
    {
        switch(keyboardCode_)
        {
            case 's':
                ret = 0xfb;
            break;
            case 'a':
                ret = 0xbf;
            break;
            case 'r':
                ret = 0x7f;
            break;
        }
    }
    if (debounceCounter_ > 0)
        ret = ret & 0xfd;
    return ret;
}

void Keyb1871::diagOut(Byte value)
{
    switch (value)
    {
        case 0:
            debounceCounter_ = 4000;
            p_Main->eventDebounceTimer();
        break;
            
        case 0x10:
        case 0x11: //set breakpoint onn ce8f / CEE8
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
            if (keyboardEf_ == 1)
            {
                keyboardCode_ = (value & 0xf)+ 0x30;
                keyboardRepeatEf_ = 1;
                keyboardEf_ = 0;
            }
        break;

        case 0x09:
            if (keyboardEf_ == 1)
            {
                keyboardCode_ = '8';
                keyboardRepeatEf_ = 1;
                keyboardEf_ = 0;
            }
        break;
            
        case 0x0A:
            if (keyboardEf_ == 1)
            {
                keyboardCode_ = '?';
                keyboardRepeatEf_ = 1;
                keyboardEf_ = 0;
            }
        break;

        case 0x0B:
            if (keyboardEf_ == 1)
            {
                keyboardCode_ = 'h';
                keyboardRepeatEf_ = 1;
                keyboardEf_ = 0;
            }
        break;

        case 0x0C:
            if (keyboardEf_ == 1)
            {
                keyboardCode_ = 'p';
                keyboardRepeatEf_ = 1;
                keyboardEf_ = 0;
            }
            break;

        case 0x0D:
            if (keyboardEf_ == 1)
            {
                keyboardCode_ = 'x';
                keyboardRepeatEf_ = 1;
                keyboardEf_ = 0;
            }
            break;

        case 0x0E:
            if (keyboardEf_ == 1)
            {
                keyboardCode_ = 0x80;
                keyboardRepeatEf_ = 1;
                keyboardEf_ = 0;
            }
        break;

        case 0x2D:
            if (keyboardEf_ == 1)
            {
                keyboardCode_ = 0x58;
                keyboardRepeatEf_ = 1;
                keyboardEf_ = 0;
            }
        break;
            
        case 0x4D:
            if (keyboardEf_ == 1)
            {
                keyboardCode_ = 0x18;
                keyboardRepeatEf_ = 1;
                keyboardEf_ = 0;
            }
        break;
            
        default:
        break;
    }
}

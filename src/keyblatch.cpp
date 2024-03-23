/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
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
#include "keyblatch.h"


KeybLatch::KeybLatch()
{
    fileToBeLoaded_ = false;
    commandText_ = "";
}

void KeybLatch::configure(IoConfiguration ioConf, Locations addressLocations, wxString type, wxString saveCommand, int pad)
{
    addressLocations_ = addressLocations;
    ioConfiguration_ = ioConf;
    saveCommand_ = saveCommand;
    pad_ = pad;
    wxString printBuffer, efText;
 
    wxString ioGroup = "";
    if (ioConfiguration_.keyLatchDetails[pad_].IoGroup != -1)
    {
        ioGroup.Printf(" on group %d", ioConfiguration_.keyLatchDetails[pad_].IoGroup);
    }
    
    p_Main->message("Configuring " + type + ioGroup);
    if (ioConfiguration_.keyLatchDetails[pad_].pressed == 1)
        efText = ": latched key pressed\n";
    else
        efText = " (reversed): latched key pressed\n";

    printBuffer.Printf("	Output %d: " + type + " latch, EF %d", ioConfiguration_.keyLatchDetails[pad_].outPort, ioConfiguration_.keyLatchDetails[pad_].ef);
    p_Main->message(printBuffer + efText);

    p_Computer->setCycleType(KEYCYCLE, LATCHKEYBCYCLE);

    for (int i=0; i<256; i++)
        keyState_[i] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
    
    ctrlAltLeft_ = true;
    forceShiftActive_ = false;
    forceNoShiftActive_ = false;
    
    resetKeybLatch();
}

void KeybLatch::reDefineHexKeys(int hexKeyDef1[], int hexKeyDef2[], bool simDef2)
{
    for (int i=0; i<512; i++)
    {
        ioConfiguration_.keyLatchDetails[pad_].pc[i] = -1;
    }
    for (int i=0; i<16; i++)
    {
        keyDef1_[i] = hexKeyDef1[i];
        if (hexKeyDef1[i] != 0)
            ioConfiguration_.keyLatchDetails[pad_].pc[keyDef1_[i]] = i;

        keyDef2_[i] = hexKeyDef2[i];
        if (hexKeyDef2[i] != 0)
            ioConfiguration_.keyLatchDetails[pad_].pc[keyDef2_[i]] = i;
    }
    simDef2_ = simDef2;
}

void KeybLatch::keyDown(int keycode, wxKeyEvent& event)
{
    int modifier = event.GetModifiers();

    switch (modifier)
    {
        case wxMOD_SHIFT:
            if (ioConfiguration_.keyLatchDetails[pad_].shift != -1)
            {
                shiftPressed_ = true;
                keyState_[ioConfiguration_.keyLatchDetails[pad_].shift] = ioConfiguration_.keyLatchDetails[pad_].pressed;
            }
        break;
            
        case wxMOD_CONTROL:
            if (ctrlAltLeft_)
            {
                if (ioConfiguration_.keyLatchDetails[pad_].ctrlLeft != -1 && commandText_ == "")
                {
                    ctrlPressed_ = true;
                    keyState_[ioConfiguration_.keyLatchDetails[pad_].ctrlLeft] = ioConfiguration_.keyLatchDetails[pad_].pressed;
                }
            }
            else
            {
                if (ioConfiguration_.keyLatchDetails[pad_].ctrlRight != -1 && commandText_ == "")
                {
                    ctrlPressed_ = true;
                    keyState_[ioConfiguration_.keyLatchDetails[pad_].ctrlRight] = ioConfiguration_.keyLatchDetails[pad_].pressed;
                }
            }
        break;
            
        case wxMOD_ALT:
            if (ctrlAltLeft_)
            {
                if (ioConfiguration_.keyLatchDetails[pad_].altLeft != -1)
                {
                    altPressed_ = true;
                    keyState_[ioConfiguration_.keyLatchDetails[pad_].altLeft] = ioConfiguration_.keyLatchDetails[pad_].pressed;
                }
            }
            else
            {
                if (ioConfiguration_.keyLatchDetails[pad_].altRight != -1)
                {
                    altPressed_ = true;
                    keyState_[ioConfiguration_.keyLatchDetails[pad_].altRight] = ioConfiguration_.keyLatchDetails[pad_].pressed;
                }
            }
        break;
    }

    if (keycode == ioConfiguration_.keyLatchDetails[pad_].switchAltCtrl)
        ctrlAltLeft_ = !ctrlAltLeft_;
    
    if (keycode == WXK_CAPITAL)
        switchCaps();

    if (!(keycode == p_Main->getCtrlvKey() && modifier == CTRL_V))
        keyDown(keycode);
}

void KeybLatch::keyDown(int keycode)
{
    if (keycode == 306)
        keycode = keycode; //>for testing key values on shift...

    if (ioConfiguration_.keyLatchDetails[pad_].pcForceShift[keycode&0x1ff] != -1 && !shiftPressed_)
    {
        if (ioConfiguration_.keyLatchDetails[pad_].shift != -1)
            keyState_[ioConfiguration_.keyLatchDetails[pad_].shift] =  ioConfiguration_.keyLatchDetails[pad_].pressed;
        shiftPressed_ = true;
        forceShiftActive_ = true;
        shiftKey_ = keycode;
        keyboardCode_ = -3;
        cycleValue_ = 3000;
        return;
    }

    if (ioConfiguration_.keyLatchDetails[pad_].pcForceNoShift[keycode&0x1ff] != -1 && shiftPressed_)
    {
        if (ioConfiguration_.keyLatchDetails[pad_].shift != -1)
            keyState_[ioConfiguration_.keyLatchDetails[pad_].shift] =  ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
        shiftPressed_ = false;
        forceNoShiftActive_ = true;
        shiftKey_ = keycode;
        keyboardCode_ = -4;
        cycleValue_ = 3000;
        return;
    }

    if (ioConfiguration_.keyLatchDetails[pad_].pcShift[keycode&0x1ff] != -1 && shiftPressed_)
    {
        keyState_[ioConfiguration_.keyLatchDetails[pad_].pcShift[keycode&0x1ff]] = ioConfiguration_.keyLatchDetails[pad_].pressed;
        return;
    }

    keyDownNoShift(keycode);
}

void KeybLatch::keyDownNoShift(int keycode)
{
    if (ioConfiguration_.keyLatchDetails[pad_].pc[keycode&0x1ff] != -1 && !shiftPressed_)
    {
        if (simDef2_)
        {
            for (std::vector<DiagonalKeys>::iterator diagonalKeys = ioConfiguration_.diagonalKeys.begin (); diagonalKeys != ioConfiguration_.diagonalKeys.end (); ++diagonalKeys)
            {
                if (keycode == keyDef2_[diagonalKeys->key1])
                {
                    if (::wxGetKeyState((wxKeyCode)keyDef2_[diagonalKeys->key2]) == true)
                    {
                        keyState_[diagonalKeys->mainKey] = ioConfiguration_.keyLatchDetails[pad_].pressed;
                        keyState_[diagonalKeys->key1] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
                        keyState_[diagonalKeys->key2] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
                        return;
                    }
                }
                if (keycode == keyDef2_[diagonalKeys->key2])
                {
                    if (::wxGetKeyState((wxKeyCode)keyDef2_[diagonalKeys->key1]) == true)
                    {
                        keyState_[diagonalKeys->mainKey] = ioConfiguration_.keyLatchDetails[pad_].pressed;
                        keyState_[diagonalKeys->key1] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
                        keyState_[diagonalKeys->key2] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
                        return;
                    }
                }
            }
        }
        
        keyState_[ioConfiguration_.keyLatchDetails[pad_].pc[keycode&0x1ff]] = ioConfiguration_.keyLatchDetails[pad_].pressed;
        ctrlvFound_ = true;
    }
}

void KeybLatch::keyUp(int keycode, wxKeyEvent& event)
{
    keyUp(keycode);
    
    if (ctrlPressed_ || altPressed_ || shiftPressed_)
    {
        int modifier = event.GetModifiers();

        if (shiftPressed_ && (modifier != wxMOD_SHIFT))
        {
            shiftPressed_ = false;
            keyState_[ioConfiguration_.keyLatchDetails[pad_].shift] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
        }
        
        if (ctrlPressed_ && (modifier != wxMOD_CONTROL))
        {
            ctrlPressed_ = false;
            if (ctrlAltLeft_)
                keyState_[ioConfiguration_.keyLatchDetails[pad_].ctrlLeft] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
            else
                keyState_[ioConfiguration_.keyLatchDetails[pad_].ctrlRight] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
        }

        if (altPressed_ && (modifier != wxMOD_ALT))
        {
            altPressed_ = false;
            if (ctrlAltLeft_)
                keyState_[ioConfiguration_.keyLatchDetails[pad_].altLeft] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
            else
                keyState_[ioConfiguration_.keyLatchDetails[pad_].altRight] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
        }
    }
}

void KeybLatch::keyUp(int keycode)
{
    if (ioConfiguration_.keyLatchDetails[pad_].pcForceShift[keycode&0x1ff] != -1 && forceShiftActive_)
    {
        keyState_[ioConfiguration_.keyLatchDetails[pad_].pcForceShift[keycode&0x1ff]] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
        if (ioConfiguration_.keyLatchDetails[pad_].shift != -1)
            keyState_[ioConfiguration_.keyLatchDetails[pad_].shift] =  ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
        cycleValue_ = -1;
        forceShiftActive_ = false;
    }

    if (ioConfiguration_.keyLatchDetails[pad_].pcForceNoShift[keycode&0x1ff] != -1 && forceNoShiftActive_)
    {
        keyState_[ioConfiguration_.keyLatchDetails[pad_].pcForceNoShift[keycode&0x1ff]] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
        if (ioConfiguration_.keyLatchDetails[pad_].shift != -1)
            keyState_[ioConfiguration_.keyLatchDetails[pad_].shift] =  ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
        cycleValue_ = -1;
        forceNoShiftActive_ = false;
    }

    if (simDef2_)
    {
        for (std::vector<DiagonalKeys>::iterator diagonalKeys = ioConfiguration_.diagonalKeys.begin (); diagonalKeys != ioConfiguration_.diagonalKeys.end (); ++diagonalKeys)
        {
            if (keycode == keyDef2_[diagonalKeys->key1] || keycode == keyDef2_[diagonalKeys->key2])
            {
                keyState_[diagonalKeys->mainKey] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
                if (::wxGetKeyState((wxKeyCode)keyDef2_[diagonalKeys->key1]) == true)
                    keyState_[diagonalKeys->key1] = ioConfiguration_.keyLatchDetails[pad_].pressed;
                if (::wxGetKeyState((wxKeyCode)keyDef2_[diagonalKeys->key2]) == true)
                    keyState_[diagonalKeys->key2] = ioConfiguration_.keyLatchDetails[pad_].pressed;
            }
        }
    }
    
    if (ioConfiguration_.keyLatchDetails[pad_].pc[keycode&0x1ff] != -1)
        keyState_[ioConfiguration_.keyLatchDetails[pad_].pc[keycode&0x1ff]] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
    if (ioConfiguration_.keyLatchDetails[pad_].pcShift[keycode&0x1ff] != -1)
        keyState_[ioConfiguration_.keyLatchDetails[pad_].pcShift[keycode&0x1ff]] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
}

Byte KeybLatch::ef()
{
    if (keyLatch_ == -1)
        return ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
    
    return keyState_[keyLatch_];
}

void KeybLatch::out(Byte value)
{
    keyLatch_ = value&ioConfiguration_.keyLatchDetails[pad_].mask;
}

void KeybLatch::resetKeybLatch()
{
    fileToBeLoaded_ = false;
    commandText_ = "";
    keyLatch_ = -1;
    shiftPressed_ = false;
    ctrlPressed_ = false;
    altPressed_ = false;
    cycleValue_ = -1;
    keyboardCode_ = 0;
    
    capsPressed_ = wxGetKeyState (WXK_CAPITAL);
}

void KeybLatch::cycleKeybLatch()
{
    if (cycleValue_ >= 0 && keyboardCode_ != 0)
    {
        cycleValue_--;
        if (cycleValue_ == 0)
        {
            if (keyboardCode_ > 0)
                keyUp(keyboardCode_);
            if (keyboardCode_ == -2)
                keyState_[ioConfiguration_.keyLatchDetails[pad_].caps] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
            if (keyboardCode_ == -3)
                keyState_[ioConfiguration_.keyLatchDetails[pad_].pcForceShift[shiftKey_&0x1ff]] = ioConfiguration_.keyLatchDetails[pad_].pressed;
            if (keyboardCode_ == -4)
                keyState_[ioConfiguration_.keyLatchDetails[pad_].pcForceNoShift[shiftKey_&0x1ff]] = ioConfiguration_.keyLatchDetails[pad_].pressed;
            keyboardCode_ = 0;
            if (p_Computer->getCtrlvCharNum() != 0)
            {
                keyState_[0xf] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
                if (ctrlvFound_)
                    p_Computer->ctrlvTextCharNumPlusOne();
            }
        }
        return;
    }

    if (fileToBeLoaded_ || commandText_ != "")
    {
        if (keyLatch_ == ioConfiguration_.keyLatchDetails[pad_].mask)
        {
            if (commandText_ != "")
            {
                keyboardCode_ = commandText_.GetChar(0);
                commandText_ = commandText_.Right(commandText_.Len()-1);
                cycleValue_ = 50000;
                keyDownNoShift(keyboardCode_);
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
                                commandText_ = "RUN\r";
                            else
                            {
                                wxString buffer;
                                buffer.Printf("%d", saveExec);
                                commandText_ = "CALL(" + buffer + ")\r";
                            }
                        }
                    }
                    keyboardCode_ = 0;
                    cycleValue_ = 50000;
                    keyDownNoShift(keyboardCode_);
                }
            }
        }
    }

    if (p_Computer->getCtrlvCharNum() != 0)
    {
        if (addressLocations_.keyInputAddress.size() != 0)
        {
            if (keyboardCode_ == 0)
            {
                if (p_Computer->checkKeyInputAddress())
                {
                    keyboardCode_ = translateKey(p_Computer->getCtrlvChar(false));

                    if (keyboardCode_ != 0)
                    {
                        ctrlvFound_ = false;
                        cycleValue_ = 50000;
                        keyDownNoShift(keyboardCode_);
                    }
                }
            }
        }
        else
        {
            if (keyLatch_ == ioConfiguration_.keyLatchDetails[pad_].mask)
            {
                if (p_Computer->getCtrlvCharNum() <= 3)
                {
                    p_Computer->ctrlvTextCharNumPlusOne();
                    keyboardCode_ = -1;
                    cycleValue_ = 50000;
                }
                else
                {
                    keyboardCode_ = p_Computer->getCtrlvCharTmc();
                
                    if (keyboardCode_ != 0)
                    {
                        ctrlvFound_ = false;
                        cycleValue_ = 50000;
                        keyDownNoShift(keyboardCode_);
                    }
                }
            }
        }
    }

    if (keyFileOpened_)
    {
        if (keyLatch_ == ioConfiguration_.keyLatchDetails[pad_].mask)
        {
            if (keyFile_.Read(&keyboardCode_, 1) == 0)
            {
                keyFileOpened_ = false;
                keyFile_.Close();
            }
            else
            {
                if (keyboardCode_ != 0)
                {
                    cycleValue_ = 50000;
                    keyDown(keyboardCode_);
                }
            }
        }
    }
}

int KeybLatch::translateKey(int key)
{
    int returnKey = 0;
    key &= 0x1ff;

    if (ioConfiguration_.keyLatchDetails[pad_].pcMap[key] != -1)
        returnKey = ioConfiguration_.keyLatchDetails[pad_].pcMap[key];

    if (ioConfiguration_.keyLatchDetails[pad_].pcMapShift[key] != -1)
    {
        keyState_[0xf] = ioConfiguration_.keyLatchDetails[pad_].pressed;
        returnKey = ioConfiguration_.keyLatchDetails[pad_].pcMapShift[key];
    }
    return returnKey;
}

void KeybLatch::startKeyFile()
{
    keyFileOpened_ = false;

    wxString fileName = p_Main->getKeyFile();
    if (fileName.Len() != 0)
    {
        fileName = p_Main->getKeyFileDir() + fileName;
        if (wxFile::Exists(fileName))
        {
            if (keyFile_.Open(fileName))
                keyFileOpened_ = true;
        }
    }
}

void KeybLatch::closeKeyFile()
{
    if (keyFileOpened_)
    {
        keyFileOpened_ = false;
        keyFile_.Close();
        if (keyboardCode_ != 0)
            keyUp(keyboardCode_);
    }
    keyboardCode_ = 0;
}

bool KeybLatch::keyDown()
{
    bool keyPressed = false;
    
    Byte latch = 0;
    while (!keyPressed && latch <= ioConfiguration_.keyLatchDetails[pad_].mask)
    {
        keyPressed = (keyState_[latch] == ioConfiguration_.keyLatchDetails[pad_].pressed);
        latch++;
    }
    return keyPressed;
}

void KeybLatch::startLatchRun(bool load, wxString command)
{
    load_ = load;
    commandText_ = command;
    fileToBeLoaded_ = true;
}

void KeybLatch::startCtrlV(wxString command)
{
    if (addressLocations_.keyInputAddress.size() != 0)
        p_Computer->ctrlvText(command);
    else
    {
        ctrlPressed_ = false;
        if (ctrlAltLeft_)
            keyState_[ioConfiguration_.keyLatchDetails[pad_].ctrlLeft] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
        else
            keyState_[ioConfiguration_.keyLatchDetails[pad_].ctrlRight] = ioConfiguration_.keyLatchDetails[pad_].pressed ^ 1;
        commandText_ = command;
        fileToBeLoaded_ = false;
    }
}

void KeybLatch::checkCaps()
{
    if (capsPressed_ == false)
        switchCaps();
}

void KeybLatch::switchCaps()
{
    cycleValue_ = 50000;
    keyboardCode_ = -2;
    capsPressed_ = !capsPressed_;
    keyState_[ioConfiguration_.keyLatchDetails[pad_].caps] = ioConfiguration_.keyLatchDetails[pad_].pressed;
}

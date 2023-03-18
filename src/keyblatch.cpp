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
}

void KeybLatch::configure(IoConfiguration ioConf, wxString saveCommand)
{
    ioConfiguration_ = ioConf;
    saveCommand_ = saveCommand;
    wxString printBuffer, efText;
 
    wxString ioGroup = "";
    if (ioConfiguration_.keybLatchIoGroup != -1)
    {
        ioGroup.Printf(" on group %d", ioConfiguration_.keybLatchIoGroup);
    }
    
    p_Main->message("Configuring keyboard" + ioGroup);
    if (ioConfiguration_.keybLatchPressed == 1)
        efText = "latched key pressed\n";
    else
        efText = "(reversed): latched key pressed\n";

    printBuffer.Printf("    Output %d: keyboard latch, EF %d: ", ioConfiguration_.keybLatchOut, ioConfiguration_.keybLatchEf);
    p_Main->message(printBuffer + efText);

    p_Computer->setCycleType(KEYCYCLE, LATCHKEYBCYCLE);

    for (int i=0; i<256; i++)
        keyState_[i] = ioConfiguration_.keybLatchPressed ^ 1;
    
    ctrlAltLeft_ = true;
    
    resetKeybLatch();
}

void KeybLatch::keyDown(int keycode, wxKeyEvent& event)
{
    int modifier = event.GetModifiers();

    switch (modifier)
    {
        case wxMOD_SHIFT:
            if (ioConfiguration_.keybLatchShift != -1)
            {
                shiftPressed_ = true;
                keyState_[ioConfiguration_.keybLatchShift] = ioConfiguration_.keybLatchPressed;
            }
        break;
            
        case wxMOD_CONTROL:
            if (ctrlAltLeft_)
            {
                if (ioConfiguration_.keybLatchCtrlLeft != -1)
                {
                    ctrlPressed_ = true;
                    keyState_[ioConfiguration_.keybLatchCtrlLeft] = ioConfiguration_.keybLatchPressed;
                }
            }
            else
            {
                if (ioConfiguration_.keybLatchCtrlRight != -1)
                {
                    ctrlPressed_ = true;
                    keyState_[ioConfiguration_.keybLatchCtrlRight] = ioConfiguration_.keybLatchPressed;
                }
            }
        break;
            
        case wxMOD_ALT:
            if (ctrlAltLeft_)
            {
                if (ioConfiguration_.keybLatchAltLeft != -1)
                {
                    altPressed_ = true;
                    keyState_[ioConfiguration_.keybLatchAltLeft] = ioConfiguration_.keybLatchPressed;
                }
            }
            else
            {
                if (ioConfiguration_.keybLatchAltRight != -1)
                {
                    altPressed_ = true;
                    keyState_[ioConfiguration_.keybLatchAltRight] = ioConfiguration_.keybLatchPressed;
                }
            }
        break;
    }

    if (keycode == ioConfiguration_.keybLatchSwitch)
        ctrlAltLeft_ = !ctrlAltLeft_;
    
    if (keycode == WXK_CAPITAL)
        switchCaps();

    keyDown(keycode);
}

void KeybLatch::keyDown(int keycode)
{
    if (shiftPressed_ && ioConfiguration_.useKeyDefShift)
    {
        if (ioConfiguration_.keybLatchPcShift[keycode&0x1ff] != -1)
            keyState_[ioConfiguration_.keybLatchPcShift[keycode&0x1ff]] =  ioConfiguration_.keybLatchPressed;
    }
    else
    {
        if (ioConfiguration_.keybLatchPc[keycode&0x1ff] != -1)
            keyState_[ioConfiguration_.keybLatchPc[keycode&0x1ff]] = ioConfiguration_.keybLatchPressed;
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
            keyState_[ioConfiguration_.keybLatchShift] = ioConfiguration_.keybLatchPressed ^ 1;
        }
        
        if (ctrlPressed_ && (modifier != wxMOD_CONTROL))
        {
            ctrlPressed_ = false;
            if (ctrlAltLeft_)
                keyState_[ioConfiguration_.keybLatchCtrlLeft] = ioConfiguration_.keybLatchPressed ^ 1;
            else
                keyState_[ioConfiguration_.keybLatchCtrlRight] = ioConfiguration_.keybLatchPressed ^ 1;
        }

        if (altPressed_ && (modifier != wxMOD_ALT))
        {
            altPressed_ = false;
            if (ctrlAltLeft_)
                keyState_[ioConfiguration_.keybLatchAltLeft] = ioConfiguration_.keybLatchPressed ^ 1;
            else
                keyState_[ioConfiguration_.keybLatchAltRight] = ioConfiguration_.keybLatchPressed ^ 1;
        }
    }
}

void KeybLatch::keyUp(int keycode)
{
    if (shiftPressed_ && ioConfiguration_.useKeyDefShift)
    {
        if (ioConfiguration_.keybLatchPcShift[keycode&0x1ff] != -1)
            keyState_[ioConfiguration_.keybLatchPcShift[keycode&0x1ff]] = ioConfiguration_.keybLatchPressed ^ 1;
    }
    else
    {
        if (ioConfiguration_.keybLatchPc[keycode&0x1ff] != -1)
            keyState_[ioConfiguration_.keybLatchPc[keycode&0x1ff]] = ioConfiguration_.keybLatchPressed ^ 1;
    }
}

Byte KeybLatch::ef()
{
    if (keyLatch_ == -1)
        return ioConfiguration_.keybLatchPressed ^ 1;
    
    return keyState_[keyLatch_];
}

void KeybLatch::out(Byte value)
{
    keyLatch_ = value&ioConfiguration_.keybLatchOutMask;
}

void KeybLatch::resetKeybLatch()
{
    runCommand_ = 0;
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
                keyState_[ioConfiguration_.keybLatchCaps] = ioConfiguration_.keybLatchPressed ^ 1;
            keyboardCode_ = 0;
        }
        return;
    }

    if (runCommand_ != 0)
    {
        if (keyLatch_ == ioConfiguration_.keybLatchOutMask)
        {
            if (runCommand_ == 1)
            {
                keyboardCode_ = 'C';
                cycleValue_ = 50000;
                keyDown(keyboardCode_);
                runCommand_++;
            }
            else if (runCommand_ == 2)
            {
                int saveExec = p_Main->pload();
                if (saveExec == 1)
                    runCommand_ = 0;
                else
                {
                    if (p_Main->isBatchConvertActive())
                    {
                        commandText_ = saveCommand_;
                    }
                    else
                    {
                        if (saveExec == 0)
                            commandText_ = "RUN";
                        else
                        {
                            wxString buffer;
                            buffer.Printf("%d", saveExec);
                            commandText_ = "CALL(" + buffer + ")";
                        }
                    }
                    keyboardCode_ = 0;
                    cycleValue_ = 50000;
                    keyDown(keyboardCode_);
                    runCommand_++;
                }
            }
            else
            {
                if (load_)
                    runCommand_ = 0;
                else
                {
                    if ((runCommand_-2) <= commandText_.Len())
                    {
                        keyboardCode_ = commandText_.GetChar(runCommand_-3);
                        cycleValue_ = 50000;
                        keyDown(keyboardCode_);
                        runCommand_++;
                    }
                    else
                    {
                        keyboardCode_ = 13;
                        cycleValue_ = 50000;
                        keyDown(keyboardCode_);
                        runCommand_ = 0;
                    }
                }
            }
        }
    }

    if (p_Computer->getCtrlvCharNum() != 0)
    {
        if (keyLatch_ == ioConfiguration_.keybLatchOutMask)
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
                    cycleValue_ = 50000;
                    keyDown(keyboardCode_);
                }
            }
        }
    }

    if (keyFileOpened_)
    {
        if (keyLatch_ == ioConfiguration_.keybLatchOutMask)
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
    while (!keyPressed && latch <= ioConfiguration_.keybLatchOutMask)
    {
        keyPressed = (keyState_[latch] == ioConfiguration_.keybLatchPressed);
        latch++;
    }
    return keyPressed;
}

void KeybLatch::startLatchRun(bool load)
{
    load_ = load;
    if (p_Computer->getRunState() == RESETSTATE)
        runCommand_ = 1;
    else
        runCommand_ = 2;
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
    keyState_[ioConfiguration_.keybLatchCaps] = ioConfiguration_.keybLatchPressed;
}

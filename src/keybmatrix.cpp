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
#include "keybmatrix.h"


KeybMatrix::KeybMatrix()
{
}

void KeybMatrix::configure(IoConfiguration ioConf, wxString saveCommand)
{
    ioConfiguration_ = ioConf;
    saveCommand_ = saveCommand;
    wxString printBuffer, efText;
 
    wxString ioGroup = "";
    if (ioConfiguration_.keybMatrixIoGroup != -1)
    {
        ioGroup.Printf(" on group %d", ioConfiguration_.keybMatrixIoGroup);
    }
    
    p_Main->message("Configuring keyboard" + ioGroup);
    
    printBuffer.Printf("    Input %d: key input ", ioConfiguration_.keybMatrixIn);
    p_Main->message(printBuffer);
    printBuffer.Printf("    ");
    
    if (ioConfiguration_.keybMatrixEfKey[MATRIX_CTRL_KEY] != 0)
    {
        printBuffer.Printf("    EF %d: CTRL", ioConfiguration_.keybMatrixEfKey[MATRIX_CTRL_KEY]);
        p_Main->message(printBuffer + efText);
    }
    if (ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY] != 0)
    {
        printBuffer.Printf("    EF %d: SHIFT", ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]);
        p_Main->message(printBuffer + efText);
    }
    if (ioConfiguration_.keybMatrixEfKey[MATRIX_CAPS_KEY] != 0)
    {
        printBuffer.Printf("    EF %d: CAPS", ioConfiguration_.keybMatrixEfKey[MATRIX_CAPS_KEY]);
        p_Main->message(printBuffer + efText);
    }
    if (ioConfiguration_.keybMatrixEfKey[MATRIX_ESC_KEY] != 0)
    {
        printBuffer.Printf("    EF %d: ESC", ioConfiguration_.keybMatrixEfKey[MATRIX_ESC_KEY]);
        p_Main->message(printBuffer + efText);
    }
    
    resetKeyboard();
}

void KeybMatrix::charEvent(int keycode)
{
    if (keyDown_) return;
    if (keycode < 256)
    {
        if (ioConfiguration_.keybMatrixKeyValue[keycode] != -1)
        {
            keyValue_[ioConfiguration_.keybMatrixKeyValue[keycode]] |= (ioConfiguration_.keybMatrixBitValue[keycode]&0xff);
            keyDown_ = true;
            keyboardCode_ = keycode;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_CTRL_KEY]] = ioConfiguration_.keybMatrixCtrlValue[keycode];
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = ioConfiguration_.keybMatrixShiftValue[keycode];
        }
    }
}

bool KeybMatrix::keyDownExtended(int keycode, wxKeyEvent& event)
{
    if (runCommand_ != 0)
    {
        if (keyDown_)
        {
            keyDown_ = false;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_CTRL_KEY]] = 1;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
            if (keyboardCode_ == 13)
                clearReturn();

            if (keyboardCode_ > 26)
                keyValue_[ioConfiguration_.keybMatrixKeyValue[(unsigned char)keyboardCode_]]  &= ((ioConfiguration_.keybMatrixBitValue[(unsigned char)keyboardCode_]&0xff) ^ 0xff);
            keyboardCode_ = 0;
        }
        runCommand_ = 0;
    }

    if (keyDown_)
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
    
    keyboardCode_ = keycode;
    
    Byte shiftPressed = 1;
    switch (event.GetModifiers())
    {
        case wxMOD_SHIFT :
            shiftPressed = 0;
        break;
    }

    switch (keycode)
    {
        case WXK_CAPITAL:
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_CAPS_KEY]] = efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_CAPS_KEY]] ^ 1;
        break;

        case WXK_ESCAPE:
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_ESC_KEY]] = 0;
            return true;
        break;

        case WXK_RETURN:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RETURN_KEY].keyValue] |= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RETURN_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;

        case WXK_END:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_END_KEY].keyValue] |= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_END_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;

        case WXK_HOME:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_HOME_KEY].keyValue] |= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_HOME_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;

        case WXK_DOWN:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_DOWN_KEY].keyValue] |= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_DOWN_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;

        case WXK_LEFT:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_LEFT_KEY].keyValue] |= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_LEFT_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;

        case WXK_RIGHT:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RIGHT_KEY].keyValue] |= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RIGHT_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;

        case WXK_UP:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_UP_KEY].keyValue] |= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_UP_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;
    }
    return false;
}

void KeybMatrix::keyUpExtended(int keycode, wxKeyEvent& WXUNUSED(event))
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
    
    switch (keycode)
    {
        case WXK_ESCAPE:
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_ESC_KEY]] = 1;
            return;
        break;

        case WXK_RETURN:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RETURN_KEY].keyValue] &= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RETURN_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
        break;

        case WXK_END:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_END_KEY].keyValue] &= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_END_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
        break;

        case WXK_HOME:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_HOME_KEY].keyValue] &= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_HOME_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
        break;

        case WXK_SPACE:
            keyValue_[ioConfiguration_.keybMatrixKeyValue[32]]  &= ((ioConfiguration_.keybMatrixBitValue[32]&0xff) ^ 0xff);
            keyDown_ = false;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
        break;
            
        case WXK_DOWN:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_DOWN_KEY].keyValue] &= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_DOWN_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
        break;

        case WXK_LEFT:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_LEFT_KEY].keyValue] &= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_LEFT_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
        break;

        case WXK_RIGHT:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RIGHT_KEY].keyValue] &= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RIGHT_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
        break;

        case WXK_UP:
            keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_UP_KEY].keyValue] &= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_UP_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
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
        switch (newKey)
        {
            case WXK_SPACE:
                keyboardCode_ = newKey;
                keyValue_[ioConfiguration_.keybMatrixKeyValue[32]] |= (ioConfiguration_.keybMatrixBitValue[32]&0xff);
                keyDown_ = true;
                efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
                return;
            break;

            case WXK_DOWN:
                keyboardCode_ = newKey;
                keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_DOWN_KEY].keyValue] |= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_DOWN_KEY].bitMaskPressed;
                keyDown_ = true;
                efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
                return;
            break;
                
            case WXK_LEFT:
                keyboardCode_ = newKey;
                keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_LEFT_KEY].keyValue] |= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_LEFT_KEY].bitMaskPressed;
                keyDown_ = true;
                efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
                return;
            break;
                
            case WXK_RIGHT:
                keyboardCode_ = newKey;
                keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RIGHT_KEY].keyValue] |= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RIGHT_KEY].bitMaskPressed;
                keyDown_ = true;
                efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
                return;
            break;
                
            case WXK_UP:
                keyboardCode_ = newKey;
                keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_UP_KEY].keyValue] |= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_UP_KEY].bitMaskPressed;
                keyDown_ = true;
                efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
                return;
            break;
        }
    }
    
    if (keyDown_)
    {
        keyDown_ = false;
        efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_CTRL_KEY]] = 1;
        efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
        efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_ESC_KEY]] = 1;
        keyValue_[ioConfiguration_.keybMatrixKeyValue[(unsigned char)keyboardCode_]]  &= ((ioConfiguration_.keybMatrixBitValue[(unsigned char)keyboardCode_]&0xff) ^ 0xff);
        keyboardCode_ = 0;
    }
}

void KeybMatrix::keyDownFile()
{
    if (keyboardCode_ == 13)
    {
        keyDown_ = true;
        keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RETURN_KEY].keyValue] |= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RETURN_KEY].bitMaskPressed;
    }

    if (keyboardCode_ > 26)
    {
        if (ioConfiguration_.keybMatrixKeyValue[(unsigned char)keyboardCode_] != -1)
        {
            keyValue_[ioConfiguration_.keybMatrixKeyValue[(unsigned char)keyboardCode_]] |= (ioConfiguration_.keybMatrixBitValue[(unsigned char)keyboardCode_]&0xff);

            keyDown_ = true;
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_CTRL_KEY]] = ioConfiguration_.keybMatrixCtrlValue[(unsigned char)keyboardCode_];
            efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = ioConfiguration_.keybMatrixShiftValue[(unsigned char)keyboardCode_];
        }
        
    }
}

void KeybMatrix::keyUpFile()
{
    if (keyDown_)
    {
        keyDown_ = false;
        efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_CTRL_KEY]] = 1;
        efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
        if (keyboardCode_ == 13)
            clearReturn();

        if (keyboardCode_ > 26)
            keyValue_[ioConfiguration_.keybMatrixKeyValue[(unsigned char)keyboardCode_]]  &= ((ioConfiguration_.keybMatrixBitValue[(unsigned char)keyboardCode_]&0xff)  ^ 0xff);
        keyboardCode_ = 0;
    }
}

void KeybMatrix::clearReturn()
{
    keyValue_[ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RETURN_KEY].keyValue] &= ioConfiguration_.keybMatrixTextKey[MATRIX_TEXT_RETURN_KEY].bitMaskNotPressed;
}

Byte KeybMatrix::efKey(Byte efNumber)
{
    Byte ret = 1;
    
    int flag = 0;
    bool continueLoop = true;
    
    while (continueLoop && flag<LAST_MATRIX_EF_KEY)
    {
        if (ioConfiguration_.keybMatrixEfKey[flag] != 0 && ioConfiguration_.keybMatrixEfKey[flag] == efNumber)
        {
            continueLoop = false;
            if (ioConfiguration_.keybMatrixEfKeyRev[flag])
                ret = efKeyValue_[efNumber] ^1;
            else
                ret = efKeyValue_[efNumber];
        }
        flag++;
    }
    return ret;
}

Byte KeybMatrix::in(Word address)
{
    Byte ret = keyValue_[address&ioConfiguration_.keybMatrixInMask] ^ (ioConfiguration_.keybMatrixPressed ^ 0xff);
    
    if (runCommand_ != 0)
    {
        cycleValue_--;
        if (cycleValue_ <= 0)
        {
            cycleValue_ = 52;
            if (keyDown_)
            {
                keyUpFile();
                if (runCommand_ >= 255)
                    runCommand_ = 0;
            }
            else
            {
                if (runCommand_ == 2)
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
                                commandText_ = "run";
                            else
                            {
                                wxString buffer;
                                buffer.Printf("%d", saveExec);
                                commandText_ = "call(" + buffer + ")";
                            }
                        }
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
                            keyDownFile();
                            runCommand_++;
                        }
                        else
                        {
                            keyboardCode_ = 13;
                            keyDownFile();
                            runCommand_ = 255;
                        }
                    }
                }
            }
        }
    }
    
    if (keyFileOpened_)
    {
        cycleValue_--;
        if (cycleValue_ <= 0)
        {
            cycleValue_ = 52;
            if (keyDown_)
                keyUpFile();
            else
            {
                if (keyFile_.Read(&keyboardCode_, 1) == 0)
                {
                    keyFileOpened_ = false;
                    keyFile_.Close();
                }
                else
                    keyDownFile();
            }
        }
    }
    
    if (p_Computer->getCtrlvCharNum() != 0)
    {
        cycleValue_--;
        if (cycleValue_ <= 0)
        {
            cycleValue_ = 52;
            if (keyDown_)
                keyUpFile();
            else
            {
                keyboardCode_ = p_Computer->getCtrlvChar();
            
                 if (keyboardCode_ != 0)
                     keyDownFile();
            }
        }
    }

    return ret;
}

void KeybMatrix::resetKeyboard()
{
    for (int i=0; i<256; i++)
        keyValue_[i] = ioConfiguration_.keybMatrixPressed ^ 0xff;
    
    for (int i=0; i<5; i++)
        secondKeyboardCodes[i] = 0;
    
    efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_CAPS_KEY]] = 1;
    efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_SHIFT_KEY]] = 1;
    efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_CTRL_KEY]] = 1;
    efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_ESC_KEY]] = 1;

    keyboardCode_ = 0;
    keyDown_ = false;

    runCommand_ = 0;
    
    cycleValue_ = 1;

    capsPressed_ = wxGetKeyState (WXK_CAPITAL);
}

void KeybMatrix::startKeyFile()
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

void KeybMatrix::closeKeyFile()
{
    if (keyFileOpened_)
    {
        keyFileOpened_ = false;
        keyFile_.Close();
        if (keyboardCode_ != 0)
            keyUpFile();
    }
    keyboardCode_ = 0;
}

void KeybMatrix::startRun(bool load)
{
    load_ = load;
    if (p_Computer->getRunState() == RESETSTATE)
        runCommand_ = 1;
    else
        runCommand_ = 2;
}

void KeybMatrix::checkCaps()
{
    if (capsPressed_ == false)
        efKeyValue_[ioConfiguration_.keybMatrixEfKey[MATRIX_CAPS_KEY]] = 0;
}


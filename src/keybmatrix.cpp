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
    fileToBeLoaded_ = false;
    commandText_ = "";
}

void KeybMatrix::configure(MatrixKeyboardConfiguration matrixKeyboardConfiguration, wxString saveCommand)
{
    matrixKeyboardConfiguration_ = matrixKeyboardConfiguration;
    saveCommand_ = saveCommand;
    wxString printBuffer;
     
    p_Main->configureMessage(&matrixKeyboardConfiguration_.ioGroupVector, "Matrix Keyboard");
    
    if (matrixKeyboardConfiguration_.input.addressMode)
    {
        printBuffer.Printf("	Read address %04X: key input", matrixKeyboardConfiguration_.input.portNumber[0]);
        p_Main->message(printBuffer);
    }
    else
    {
        p_Computer->setInType(&matrixKeyboardConfiguration_.ioGroupVector, matrixKeyboardConfiguration_.input, "key input");
    }
    
    if (matrixKeyboardConfiguration_.output.addressMode)
    {
        printBuffer.Printf("	Write address %04X: set row", matrixKeyboardConfiguration_.output.portNumber[0]);
        p_Main->message(printBuffer);
    }
    else
    {
        p_Computer->setOutType(&matrixKeyboardConfiguration_.ioGroupVector, matrixKeyboardConfiguration_.output, "set row");
    }

    if (matrixKeyboardConfiguration_.efKey[MATRIX_CTRL_KEY] != 0)
    {
        printBuffer.Printf("	EF %d: CTRL", matrixKeyboardConfiguration_.efKey[MATRIX_CTRL_KEY]);
        p_Main->message(printBuffer);
    }
    if (matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY] != 0)
    {
        printBuffer.Printf("	EF %d: SHIFT", matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]);
        p_Main->message(printBuffer);
    }
    if (matrixKeyboardConfiguration_.efKey[MATRIX_CAPS_KEY] != 0)
    {
        printBuffer.Printf("	EF %d: CAPS", matrixKeyboardConfiguration_.efKey[MATRIX_CAPS_KEY]);
        p_Main->message(printBuffer);
    }
    if (matrixKeyboardConfiguration_.efKey[MATRIX_ESC_KEY] != 0)
    {
        printBuffer.Printf("	EF %d: ESC", matrixKeyboardConfiguration_.efKey[MATRIX_ESC_KEY]);
        p_Main->message(printBuffer);
    }
    
    p_Main->message("");
    resetKeyboard();
}

void KeybMatrix::charEvent(int keycode)
{
    if (keyDown_) return;
    if (keycode < 256)
    {
        if (matrixKeyboardConfiguration_.keyValue[keycode] != -1)
        {
            keyValue_[matrixKeyboardConfiguration_.keyValue[keycode]] |= (matrixKeyboardConfiguration_.bitValue[keycode]&0xff);
            keyDown_ = true;
            keyboardCode_ = keycode;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_CTRL_KEY]] = matrixKeyboardConfiguration_.ctrlValue[keycode];
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = matrixKeyboardConfiguration_.shiftValue[keycode];
        }
    }
}

bool KeybMatrix::keyDownExtended(int keycode, wxKeyEvent& event)
{
    if (fileToBeLoaded_ || commandText_ != "")
    {
        if (keyDown_)
        {
            keyDown_ = false;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_CTRL_KEY]] = 1;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
            if (keyboardCode_ == 13)
                clearReturn();

            if (keyboardCode_ > 26)
                keyValue_[matrixKeyboardConfiguration_.keyValue[(unsigned char)keyboardCode_]]  &= ((matrixKeyboardConfiguration_.bitValue[(unsigned char)keyboardCode_]&0xff) ^ 0xff);
            keyboardCode_ = 0;
        }
        fileToBeLoaded_ = false;
        commandText_ = "";
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
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_CAPS_KEY]] = efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_CAPS_KEY]] ^ 1;
        break;

        case WXK_ESCAPE:
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_ESC_KEY]] = 0;
            return true;
        break;

        case WXK_RETURN:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RETURN_KEY].keyValue] |= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RETURN_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;

        case WXK_END:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_END_KEY].keyValue] |= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_END_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;

        case WXK_HOME:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_HOME_KEY].keyValue] |= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_HOME_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;

        case WXK_DOWN:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_DOWN_KEY].keyValue] |= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_DOWN_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;

        case WXK_LEFT:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_LEFT_KEY].keyValue] |= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_LEFT_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;

        case WXK_RIGHT:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RIGHT_KEY].keyValue] |= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RIGHT_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;

        case WXK_UP:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_UP_KEY].keyValue] |= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_UP_KEY].bitMaskPressed;
            keyDown_ = true;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = shiftPressed;
            return true;
        break;
    }
    return false;
}

void KeybMatrix::keyUpExtended(int keycode)
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
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_ESC_KEY]] = 1;
            return;
        break;

        case WXK_RETURN:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RETURN_KEY].keyValue] &= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RETURN_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
        break;

        case WXK_END:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_END_KEY].keyValue] &= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_END_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
        break;

        case WXK_HOME:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_HOME_KEY].keyValue] &= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_HOME_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
        break;

        case WXK_SPACE:
            keyValue_[matrixKeyboardConfiguration_.keyValue[32]]  &= ((matrixKeyboardConfiguration_.bitValue[32]&0xff) ^ 0xff);
            keyDown_ = false;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
        break;
            
        case WXK_DOWN:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_DOWN_KEY].keyValue] &= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_DOWN_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
        break;

        case WXK_LEFT:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_LEFT_KEY].keyValue] &= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_LEFT_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
        break;

        case WXK_RIGHT:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RIGHT_KEY].keyValue] &= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RIGHT_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
        break;

        case WXK_UP:
            keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_UP_KEY].keyValue] &= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_UP_KEY].bitMaskNotPressed;
            keyDown_ = false;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
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
                keyValue_[matrixKeyboardConfiguration_.keyValue[32]] |= (matrixKeyboardConfiguration_.bitValue[32]&0xff);
                keyDown_ = true;
                efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
                return;
            break;

            case WXK_DOWN:
                keyboardCode_ = newKey;
                keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_DOWN_KEY].keyValue] |= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_DOWN_KEY].bitMaskPressed;
                keyDown_ = true;
                efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
                return;
            break;
                
            case WXK_LEFT:
                keyboardCode_ = newKey;
                keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_LEFT_KEY].keyValue] |= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_LEFT_KEY].bitMaskPressed;
                keyDown_ = true;
                efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
                return;
            break;
                
            case WXK_RIGHT:
                keyboardCode_ = newKey;
                keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RIGHT_KEY].keyValue] |= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RIGHT_KEY].bitMaskPressed;
                keyDown_ = true;
                efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
                return;
            break;
                
            case WXK_UP:
                keyboardCode_ = newKey;
                keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_UP_KEY].keyValue] |= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_UP_KEY].bitMaskPressed;
                keyDown_ = true;
                efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
                return;
            break;
        }
    }
    
    if (keyDown_)
    {
        keyDown_ = false;
        efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_CTRL_KEY]] = 1;
        efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
        efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_ESC_KEY]] = 1;
        keyValue_[matrixKeyboardConfiguration_.keyValue[(unsigned char)keyboardCode_]]  &= ((matrixKeyboardConfiguration_.bitValue[(unsigned char)keyboardCode_]&0xff) ^ 0xff);
        keyboardCode_ = 0;
    }
}

void KeybMatrix::keyDownFile()
{
    if (keyboardCode_ == 13)
    {
        keyDown_ = true;
        keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RETURN_KEY].keyValue] |= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RETURN_KEY].bitMaskPressed;
    }

    if (keyboardCode_ > 26)
    {
        if (matrixKeyboardConfiguration_.keyValue[(unsigned char)keyboardCode_] != -1)
        {
            keyValue_[matrixKeyboardConfiguration_.keyValue[(unsigned char)keyboardCode_]] |= (matrixKeyboardConfiguration_.bitValue[(unsigned char)keyboardCode_]&0xff);

            keyDown_ = true;
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_CTRL_KEY]] = matrixKeyboardConfiguration_.ctrlValue[(unsigned char)keyboardCode_];
            efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = matrixKeyboardConfiguration_.shiftValue[(unsigned char)keyboardCode_];
        }
        
    }
}

void KeybMatrix::keyUpFile()
{
    if (keyDown_)
    {
        keyDown_ = false;
        efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_CTRL_KEY]] = 1;
        efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
        if (keyboardCode_ == 13)
            clearReturn();

        if (keyboardCode_ > 26)
            keyValue_[matrixKeyboardConfiguration_.keyValue[(unsigned char)keyboardCode_]]  &= ((matrixKeyboardConfiguration_.bitValue[(unsigned char)keyboardCode_]&0xff)  ^ 0xff);
        keyboardCode_ = 0;
    }
}

void KeybMatrix::clearReturn()
{
    keyValue_[matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RETURN_KEY].keyValue] &= matrixKeyboardConfiguration_.textKey[MATRIX_TEXT_RETURN_KEY].bitMaskNotPressed;
}

int KeybMatrix::efKey(Byte efNumber)
{
    int ret = -1;
    
    int flag = 0;
    bool continueLoop = true;
    
    while (continueLoop && flag<LAST_MATRIX_EF_KEY)
    {
        if (matrixKeyboardConfiguration_.efKey[flag] != 0 && matrixKeyboardConfiguration_.efKey[flag] == efNumber)
        {
            continueLoop = false;
            if (matrixKeyboardConfiguration_.efKeyRev[flag])
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
    Byte ret = keyValue_[address&matrixKeyboardConfiguration_.input.addressMask] ^ (matrixKeyboardConfiguration_.keyPressed ^ 0xff);
    
    if (fileToBeLoaded_ || commandText_ != "")
    {
        cycleValue_--;
        if (cycleValue_ <= 0)
        {
            cycleValue_ = 52;
            if (keyDown_)
                keyUpFile();
            else
            {
                if (commandText_ != "")
                {
                    keyboardCode_ = commandText_.GetChar(0);
                    commandText_ = commandText_.Right(commandText_.Len()-1);
                    keyDownFile();
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
                                    buffer.Printf("%d", saveExec);
                                    commandText_ = "call(" + buffer + ")\r";
                                }
                            }
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

Byte KeybMatrix::in()
{
    return in(row_);
}

void KeybMatrix::setRow(Byte row)
{
    row_ = row;
}

void KeybMatrix::resetKeyboard()
{
    for (int i=0; i<256; i++)
        keyValue_[i] = 0;
    
    for (int i=0; i<5; i++)
        secondKeyboardCodes[i] = 0;
    
    efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_CAPS_KEY]] = 1;
    efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_SHIFT_KEY]] = 1;
    efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_CTRL_KEY]] = 1;
    efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_ESC_KEY]] = 1;

    keyboardCode_ = 0;
    keyDown_ = false;

    row_ = 0;
        
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

void KeybMatrix::startRun(bool load, wxString command)
{
    load_ = load;
    commandText_ = command;
    fileToBeLoaded_ = true;
}

void KeybMatrix::startCtrlV(wxString command)
{
    commandText_ = command;
    fileToBeLoaded_ = false;
}

void KeybMatrix::checkCaps()
{
    if (capsPressed_ == false)
        efKeyValue_[matrixKeyboardConfiguration_.efKey[MATRIX_CAPS_KEY]] = 0;
}


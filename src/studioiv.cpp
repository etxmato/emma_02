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
#include "studioiv.h"

StudioIV::StudioIV(const wxString& title, const wxPoint& pos, const wxSize& size, double zoom, double zoomfactor, int computerType, Conf computerConf)
:PixieStudioIV(title, pos, size, zoom, zoomfactor, computerType, 0)
{
    computerConfiguration = computerConf;
}

StudioIV::~StudioIV()
{
    p_Main->setMainPos(STUDIOIV, GetPosition());
}

void StudioIV::configureComputer()
{
    outType_[0][0][1] = VIPOUT4;
    outType_[0][0][2] = STUDIOOUT;
    outType_[0][0][7] = VIPIIOUT7;
    victoryKeyPort_ = 0;
    efType_[0][0][2] = VIPEF2;
    efType_[0][0][3] = STUDIOEF3;
    efType_[0][0][4] = STUDIOEF4;

    cycleType_[COMPUTERCYCLE] = VIPIIKEYCYCLE;

    for (int j=0; j<2; j++) for (int i=0; i<16; i++)
        victoryKeyState_[j][i] = 0;

    p_Main->message("Configuring Studio IV");
    p_Main->message("    Output 1: tone latch");
    p_Main->message("    Output 2: select port, EF 3: read selected port 1, EF4: read selected port 2");
    p_Main->message("    output 7: cassette on/off, EF 2: cassette in\n");

    p_Main->getDefaultHexKeys(STUDIOIV, "StudioIV", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
    p_Main->getDefaultHexKeys(STUDIOIV, "StudioIV", "B", keyDefB1_, keyDefB2_, keyDefGameHexB_);

    gameAuto_ = p_Main->getConfigBool("/StudioIV/GameAuto", true);

    simDefA2_ = p_Main->getConfigBool("/StudioIV/DiagonalA2", false);
    simDefB2_ = p_Main->getConfigBool("/StudioIV/DiagonalB2", false);
    
    keyboardValue_ = 0;
    shiftKey_ = 0;
    addressLatch_ = 0x8000;

    resetCpu();
}

void StudioIV::reDefineKeysA(int hexKeyDefA1[], int hexKeyDefA2[])
{
    for (int i=0; i<512; i++)
    {
        keyDefinition[i].defined = false;
    }
    for (int i=0; i<16; i++)
    {
        keyDefA1_[i] = hexKeyDefA1[i];
        if (hexKeyDefA1[i] != 0)
        {
            keyDefinition[keyDefA1_[i]].defined = true;
            keyDefinition[keyDefA1_[i]].player = 0;
            keyDefinition[keyDefA1_[i]].key = i;
            keyDefinition[keyDefA1_[i]].shift = false;
        }
        keyDefA2_[i] = hexKeyDefA2[i];
        if (hexKeyDefA2[i] != 0)
        {
            keyDefinition[keyDefA2_[i]].defined = true;
            keyDefinition[keyDefA2_[i]].player = 0;
            keyDefinition[keyDefA2_[i]].key = i;
            keyDefinition[keyDefA2_[i]].shift = true;
        }
    }
}

void StudioIV::reDefineKeysB(int hexKeyDefB1[], int hexKeyDefB2[])
{
    for (int i = 0; i<16; i++)
    {
        keyDefB1_[i] = hexKeyDefB1[i];
        if (hexKeyDefB1[i] != 0)
        {
            keyDefinition[keyDefB1_[i]].defined = true;
            keyDefinition[keyDefB1_[i]].player = 1;
            keyDefinition[keyDefB1_[i]].key = i;
            keyDefinition[keyDefB1_[i]].shift = false;
        }
        keyDefB2_[i] = hexKeyDefB2[i];
        if (hexKeyDefB2[i] != 0)
        {
            keyDefinition[keyDefB2_[i]].defined = true;
            keyDefinition[keyDefB2_[i]].player = 1;
            keyDefinition[keyDefB2_[i]].key = i;
            keyDefinition[keyDefB2_[i]].shift = true;
        }
    }
}

void StudioIV::keyDown(int keycode)
{
    if (keycode == WXK_SHIFT && (pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020"))
    {
        victoryKeyState_[0][0xf] = 1;
        victoryKeyState_[1][0xf] = 1;
    }
    
    if (keyDefinition[keycode].defined)
    {
        if (simDefA2_)
        {
            if (keycode == keyDefA2_[2])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[4]) == true)
                {
                    victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key - 1] = 1;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[6]) == true)
                {
                    victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key + 1] = 1;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
                    return;
                }
            }
            if (keycode == keyDefA2_[4])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[2]) == true)
                {
                    victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key - 1] = 1;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[8]) == true)
                {
                    victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key - 1] = 1;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
                    return;
                }
            }
            if (keycode == keyDefA2_[6])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[2]) == true)
                {
                    victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key + 1] = 1;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 0;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[8]) == true)
                {
                    victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key + 1] = 1;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
                    return;
                }
            }
            if (keycode == keyDefA2_[8])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[4]) == true)
                {
                    victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key - 1] = 1;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefA2_[6]) == true)
                {
                    victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key + 1] = 1;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 0;
                    victoryKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 0;
                    return;
                }
            }
        }

        if (simDefB2_)
        {
            if (keycode == keyDefB2_[2])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[4]) == true)
                {
                    victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key - 1] = 1;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[6]) == true)
                {
                    victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key + 1] = 1;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
                    return;
                }
            }
            if (keycode == keyDefB2_[4])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[2]) == true)
                {
                    victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key - 1] = 1;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[8]) == true)
                {
                    victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key - 1] = 1;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
                    return;
                }
            }
            if (keycode == keyDefB2_[6])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[2]) == true)
                {
                    victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key + 1] = 1;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 0;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[8]) == true)
                {
                    victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key + 1] = 1;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
                    return;
                }
            }
            if (keycode == keyDefB2_[8])
            {
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[4]) == true)
                {
                    victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key - 1] = 1;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 0;
                    return;
                }
                if (::wxGetKeyState((wxKeyCode)keyDefB2_[6]) == true)
                {
                    victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key + 1] = 1;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 0;
                    victoryKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 0;
                    return;
                }
            }
        }
        
        if (keyDefinition[keycode].shift && (pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020"))
        {
            victoryKeyState_[0][0xf] = 1;
            victoryKeyState_[1][0xf] = 1;
            keyboardValue_ = keycode;
            shiftKey_ = 5000;
        }
        else
            victoryKeyState_[keyDefinition[keycode].player][keyDefinition[keycode].key] = 1;
    }
//    keyboardValue_ = 0;
}

void StudioIV::keyUp(int keycode)
{
    if (keycode == WXK_SHIFT && (pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020"))
    {
        victoryKeyState_[0][0xf] = 0;
        victoryKeyState_[1][0xf] = 0;
    }

    if (simDefA2_)
    {
        if (keycode == keyDefA2_[2] || keycode == keyDefA2_[4] || keycode == keyDefA2_[6] || keycode == keyDefA2_[8])
        {
            victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key-1] = 0;
            victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key+1] = 0;
            victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key-1] = 0;
            victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key+1] = 0;
            if (::wxGetKeyState((wxKeyCode)keyDefA2_[2]) == true)
                victoryKeyState_[0][keyDefinition[keyDefA2_[2]].key] = 1;
            if (::wxGetKeyState((wxKeyCode)keyDefA2_[4]) == true)
                victoryKeyState_[0][keyDefinition[keyDefA2_[4]].key] = 1;
            if (::wxGetKeyState((wxKeyCode)keyDefA2_[6]) == true)
                victoryKeyState_[0][keyDefinition[keyDefA2_[6]].key] = 1;
            if (::wxGetKeyState((wxKeyCode)keyDefA2_[8]) == true)
                victoryKeyState_[0][keyDefinition[keyDefA2_[8]].key] = 1;
        }
    }
    if (simDefB2_)
    {
        if (keycode == keyDefB2_[2] || keycode == keyDefB2_[4] || keycode == keyDefB2_[6] || keycode == keyDefB2_[8])
        {
            victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key - 1] = 0;
            victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key + 1] = 0;
            victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key - 1] = 0;
            victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key + 1] = 0;
            if (::wxGetKeyState((wxKeyCode)keyDefB2_[2]) == true)
                victoryKeyState_[1][keyDefinition[keyDefB2_[2]].key] = 1;
            if (::wxGetKeyState((wxKeyCode)keyDefB2_[4]) == true)
                victoryKeyState_[1][keyDefinition[keyDefB2_[4]].key] = 1;
            if (::wxGetKeyState((wxKeyCode)keyDefB2_[6]) == true)
                victoryKeyState_[1][keyDefinition[keyDefB2_[6]].key] = 1;
            if (::wxGetKeyState((wxKeyCode)keyDefB2_[8]) == true)
                victoryKeyState_[1][keyDefinition[keyDefB2_[8]].key] = 1;
        }
    }
    if (keyDefinition[keycode].defined)
        victoryKeyState_[keyDefinition[keycode].player][keyDefinition[keycode].key] = 0;
    if (keyDefinition[keycode].shift && (pseudoType_ == "AM4KBAS" || pseudoType_ == "AM4KBAS1978" || pseudoType_ == "AM4KBASPLUS" || pseudoType_ == "AM4KBAS2020"))
    {
        victoryKeyState_[0][0xf] = 0;
        victoryKeyState_[1][0xf] = 0;
    }
}

Byte StudioIV::ef(int flag)
{
    switch(efType_[0][0][flag])
    {
        case 0:
            return 1;
        break;

        case PIXIEEF:
            return efPixie();
        break;

        case VIPEF2:
            if (tapeFinished_ > 0)
            {
                if ((tapeFinished_ & 0xff) == 0)
                    cassetteEf_ = !cassetteEf_;
                tapeFinished_--;
                if (tapeFinished_ == 0)
                    cassetteEf_ = 0;
            }
            return cassetteEf_;
        break;

        case STUDIOEF3:
            return ef3();
        break;

        case STUDIOEF4:
            return ef4();
        break;

        default:
            return 1;
    }
}

Byte StudioIV::ef3()
{
    if (victoryKeyPort_<0 || victoryKeyPort_>16)
        return 1;
    return(victoryKeyState_[0][victoryKeyPort_]) ? 0 : 1;
}

Byte StudioIV::ef4()
{
    if (victoryKeyPort_<0 || victoryKeyPort_>16)
        return 1;
    return(victoryKeyState_[1][victoryKeyPort_]) ? 0 : 1;
}

Byte StudioIV::in(Byte port, Word WXUNUSED(address))
{
    Byte ret;

    switch(inType_[0][0][port])
    {
        case 0:
            ret = 255;
        break;

        case PIXIEIN:
            ret = inPixie();
        break;

        default:
            ret = 255;
    }
    inValues_[port] = ret;
    return ret;
}

void StudioIV::out(Byte port, Word WXUNUSED(address), Byte value)
{
    outValues_[port] = value;

    switch(outType_[0][0][port])
    {
        case 0:
            return;
        break;

        case STUDIOOUT:
            outStudioIV(value);
        break;

        case PIXIEOUT:
            outPixieStudioIV(value);
        break;
            
        case VIPOUT4:
            tone1864Latch(value);
        break;

        case STUDIOIVDMA:
            dmaEnable();
        break;

        case VIPIIOUT7:
            if (value == 1)
            {
                p_Main->startCassetteLoad(0);
                return;
            }
            if (value == 2)
            {
                p_Main->startCassetteSave(0);
                return;
            }
            p_Main->stopCassette();
        break;
    }
}

void StudioIV::finishStopTape()
{
    tapeFinished_ = 10000;
}

void StudioIV::outStudioIV(Byte value)
{
    victoryKeyPort_ = value & 0xf;
}

void StudioIV::cycle(int type)
{
    int address = 0x3F0;
    
    switch(cycleType_[type])
    {
        case 0:
            return;
        break;

        case PIXIECYCLE:
            cyclePixie();
        break;
            
        case VIPIIKEYCYCLE:
            if (ctrlvTextCharNum_ != 0)
            {
                if (keyboardValue_ == 0)
                {
                    if (scratchpadRegister_[programCounter_] == 0x5CE)
                    {
                        keyboardValue_ = translateKey(getCtrlvChar());

                        if (keyboardValue_ != 0)
                            keyDown(keyboardValue_);
                    }
                }
                else
                {
                    if (pseudoType_ == "AM4KBAS1978")
                        address = 0x600;
                    
                    if (scratchpadRegister_[programCounter_] == address)
                    {
                        keyUp(keyboardValue_);
                        keyboardValue_ = 0;
                        victoryKeyState_[0][0xf] = 0;
                        victoryKeyState_[1][0xf] = 0;
                    }
                }
            }
            if (shiftKey_ > 0)
            {
                shiftKey_--;
                
                if (shiftKey_ == 0)
                    victoryKeyState_[keyDefinition[keyboardValue_].player][keyDefinition[keyboardValue_].key] = 1;
            }
        break;
    }
}

int StudioIV::translateKey(int key)
{
    int returnKey = 0;
    
    if (key >= 'a' && key <= 'z')
        returnKey =  key - 32;
    if (key >= 'A' && key <= 'Z')
        returnKey =  key;
    
    switch(key)
    {
        case '1': victoryKeyState_[0][0xf] = 1; returnKey = 'A'; break;
        case '2': victoryKeyState_[0][0xf] = 1; returnKey = 'B'; break;
        case '3': victoryKeyState_[0][0xf] = 1; returnKey = 'C'; break;
        case '<': victoryKeyState_[0][0xf] = 1; returnKey = 'D'; break;
        case '4': victoryKeyState_[0][0xf] = 1; returnKey = 'E'; break;
        case '5': victoryKeyState_[0][0xf] = 1; returnKey = 'F'; break;
        case '6': victoryKeyState_[0][0xf] = 1; returnKey = 'G'; break;
        case '=': victoryKeyState_[0][0xf] = 1; returnKey = 'H'; break;
        case '7': victoryKeyState_[0][0xf] = 1; returnKey = 'I'; break;
        case '8': victoryKeyState_[0][0xf] = 1; returnKey = 'J'; break;
        case '9': victoryKeyState_[0][0xf] = 1; returnKey = 'K'; break;
        case '>': victoryKeyState_[0][0xf] = 1; returnKey = 'L'; break;
        case ',': victoryKeyState_[0][0xf] = 1; returnKey = 'M'; break;
        case '0': victoryKeyState_[0][0xf] = 1; returnKey = 'N'; break;
        case ';': victoryKeyState_[0][0xf] = 1; returnKey = 'O'; break;
        case '!': victoryKeyState_[1][0xf] = 1; returnKey = 'P'; break;
        case '"': victoryKeyState_[1][0xf] = 1; returnKey = 'Q'; break;
        case '#': victoryKeyState_[1][0xf] = 1; returnKey = 'R'; break;
        case '$': victoryKeyState_[1][0xf] = 1; returnKey = 'S'; break;
        case '%': victoryKeyState_[1][0xf] = 1; returnKey = 'T'; break;
        case '(': victoryKeyState_[1][0xf] = 1; returnKey = 'U'; break;
        case ')': victoryKeyState_[1][0xf] = 1; returnKey = 'V'; break;
        case '*': victoryKeyState_[1][0xf] = 1; returnKey = 'W'; break;
        case '/': victoryKeyState_[1][0xf] = 1; returnKey = 'X'; break;
        case '-': victoryKeyState_[1][0xf] = 1; returnKey = 'Y'; break;
        case ':': victoryKeyState_[1][0xf] = 1; returnKey = 'Z'; break;
        case '?': victoryKeyState_[1][0xf] = 1; returnKey = 13; break;
        case '+': victoryKeyState_[1][0xf] = 1; returnKey = '.'; break;
        case 13: returnKey = 13; break;
        case ' ': returnKey = ' '; break;
        case 0x201c: victoryKeyState_[1][0xf] = 1; returnKey = 'Q'; break;
        case 0x201d: victoryKeyState_[1][0xf] = 1; returnKey = 'Q'; break;
    }
    return returnKey;
}

void StudioIV::startComputer()
{
    resetPressed_ = false;

    p_Main->setSwName("");

    p_Main->checkAndReInstallMainRom(STUDIOIV);
    
    st2020Active_ = p_Main->is2020Active();
    if (st2020Active_)
    {
        startComputer2020();
        return;
    }
    readProgram(p_Main->getRomDir(STUDIOIV, MAINROM1), p_Main->getRomFile(STUDIOIV, MAINROM1), ROM, 0, NONAME);

    p_Main->assDefault("studioivrom", 0x000, 0x7FF);
    p_Main->assDefault("studioivcart_1", 0x800, 0xFFF);
    p_Main->assDefault("studioivcart_2", 0x1000, 0x17FF);

    defineMemoryType(0x1800, 0x27FF, RAM);
    initRam(0x1800, 0x27FF);
    
    double zoom = p_Main->getZoom(VIDEOMAIN);

    configurePixie();
    initPixie();
    setZoom(zoom);
    Show(true);
    setWait(1);
    setClear(0);
    setWait(1);
    setClear(1);

    cassetteEf_ = 0;

    if (gameAuto_)
        p_Main->loadKeyDefinition(p_Main->getRomFile(STUDIOIV, MAINROM1), p_Main->getRomFile(STUDIOIV, CARTROM), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition.txt");

    pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);

    if (pseudoType_ == "SUPERCHIP")
    {
        if (p_Main->getLoadromModeStudio() == RAM)
        {
            defineMemoryType(0x800, 0x17FF, RAM);
            initRam(0x800, 0x17FF);
            readProgram(p_Main->getRomDir(STUDIOIV, CARTROM), p_Main->getRomFile(STUDIOIV, CARTROM), RAM, 0x800, SHOWNAME);
        }
        else
        {
            defineMemoryType(0x800, 0x17FF, ROM);
            readProgram(p_Main->getRomDir(STUDIOIV, CARTROM), p_Main->getRomFile(STUDIOIV, CARTROM), ROM, 0x800, SHOWNAME);
        }
    }
    else
    {
        if (p_Main->getLoadromModeStudio() == RAM)
            readSt2Program(STUDIOIV, RAM);
        else
            readSt2Program(STUDIOIV, CARTRIDGEROM);
    }

    if (pseudoType_ == "AM4KBASPLUS")
    {
        defineMemoryType(0x1000, 0x13FF, COLOURRAM);
        initRam(0x1000, 0x13FF);
        defineMemoryType(0x2800, 0x97FF, RAM);
        initRam(0x2800, 0x97FF);
    }
    else
    {
        defineMemoryType(0x2800, 0x2BFF, COLOURRAM);
        initRam(0x2800, 0x2BFF);
    }

    reDefineKeysA(keyDefA1_, keyDefA2_);
    reDefineKeysB(keyDefB1_, keyDefB2_);

    p_Main->updateTitle();

    cpuCycles_ = 0;
    instructionCounter_= 0;
    p_Main->startTime();

    threadPointer->Run();
}

void StudioIV::startComputer2020()
{
    readProgram(p_Main->getRomDir(STUDIOIV, MAINROM1), p_Main->getRomFile(STUDIOIV, MAINROM1), ROM, 0x8000, NONAME);

    defineMemoryType(0, 0x7bFF, RAM);
    initRam(0, 0x7bFF);
    defineMemoryType(0x7C00, 0x7FFF, COLOURRAM);
//    initRam(0xBC00, 0xBFFF);

    double zoom = p_Main->getZoom(VIDEOMAIN);
    
    configurePixie();
    initPixie();
    setZoom(zoom);
    Show(true);
    setWait(1);
    setClear(0);
    setWait(1);
    setClear(1);
    
    cassetteEf_ = 0;
        
    reDefineKeysA(keyDefA1_, keyDefA2_);
    reDefineKeysB(keyDefB1_, keyDefB2_);
    
    p_Main->updateTitle();
    
    cpuCycles_ = 0;
    instructionCounter_= 0;
    p_Main->startTime();
 
    pseudoType_ = "AM4KBAS2020";
    chip8baseVar_ = 0x7b00;
    chip8mainLoop_ = 0xA0AB;
    chip8register12bit_ = false;
    pseudoLoaded_ = true;
    p_Main->forcePseudoDefinition(pseudoType_, "am2020bas.syntax", "AM4KBAS 2020");

    threadPointer->Run();
}

void StudioIV::writeMemDataType(Word address, Byte type)
{
    switch (memoryType_[address/256]&0xff)
    {
        case RAM:
        case ROM:
            if (mainMemoryDataType_[address] != type)
            {
                p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
                mainMemoryDataType_[address] = type;
            }
            increaseExecutedMainMemory(address, type);
        break;
    }
}

Byte StudioIV::readMemDataType(Word address, uint64_t* executed)
{
    switch (memoryType_[address/256]&0xff)
    {
        case RAM:
        case ROM:
            if (profilerCounter_ != PROFILER_OFF)
                *executed = mainMemoryExecuted_[address];
            return mainMemoryDataType_[address];
        break;
    }
    return MEM_TYPE_UNDEFINED;
}

Byte StudioIV::readMem(Word address)
{
    if (address >= 0x8000)
        addressLatch_ = 0;
    
    if (st2020Active_)
        address |= addressLatch_;
    
    switch (memoryType_[address/256]&0xff)
    {
        case UNDEFINED:
            return 255;
        break;

        case COLOURRAM:
            address = (address&0xf) +  ((address&0x3c0) >> 2);
            return colorMemory1864_[address] & 0xf;
        break;
            
    }

//    if ((address < 0x1000 && address > 0x8a) || address < 0x2c)
//        p_Main->eventMessageHex(scratchpadRegister_[programCounter_]);
    return mainMemory_[address];
}

Byte StudioIV::readMemDebug(Word address)
{
/*    if (address >= 0x8000)
        addressLatch_ = 0;
    
    if (st2020Active_)
        address |= addressLatch_;
    
    switch (memoryType_[address/256]&0xff)
    {
        case UNDEFINED:
            return 255;
            break;
            
        case COLOURRAM:
            if (st2020Active_)
                return mainMemory_[address];
            else
            {
                address = (address&0xf) +  ((address&0x3c0) >> 2);
                return colorMemory1864_[address] & 0xf;
            }
            break;
            
    }
    
    return mainMemory_[address];*/
    return readMem(address);
}

void StudioIV::writeMem(Word address, Byte value, bool writeRom)
{
    switch (memoryType_[address/256]&0xff)
    {
        case RAM:
            if (mainMemory_[address]==value)
                return;
            mainMemory_[address]=value;
            if (address>= memoryStart_ && address<(memoryStart_+256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
        break;

        case COLOURRAM:
            address = (address&0xf) +  ((address&0x3c0) >> 2);
            colorMemory1864_[address] = value & 0xf;
            if ((address) >= memoryStart_ && (address) < (memoryStart_ + 256))
                p_Main->updateDebugMemory(address);
            if (address >= memoryStart_ && address < (memoryStart_ + 256))
                p_Main->updateDebugMemory(address);
            p_Main->updateAssTabCheck(address);
            useColour(7);
        break;
            
        default:
            if (writeRom)
                mainMemory_[address]=value;
        break;
    }
}

void StudioIV::writeMemDebug(Word address, Byte value, bool writeRom)
{
    writeMem(address, value, writeRom);
}

Byte StudioIV::read1864ColorDirect(Word address)
{
    return colorMemory1864_[address] & 0xf;
}

void StudioIV::write1864ColorDirect(Word address, Byte value)
{
    colorMemory1864_[address] = value & 0xf;
}

void StudioIV::cpuInstruction()
{
    if (cpuMode_ == RUN)
    {
        cpuCycleStep();
    }
    else
    {
        initPixie();
        cpuCycles_ = 0;
        instructionCounter_= 0;
        p_Main->startTime();
    }
}

void StudioIV::resetPressed()
{
    resetCpu();
    resetPressed_ = false;
    
    p_Main->getDefaultHexKeys(STUDIOIV, "StudioIV", "A", keyDefA1_, keyDefA2_, keyDefGameHexA_);
    p_Main->getDefaultHexKeys(STUDIOIV, "StudioIV", "B", keyDefB1_, keyDefB2_, keyDefGameHexB_);
    
    simDefA2_ = p_Main->getConfigBool("/StudioIV/DiagonalA2", true);
    simDefB2_ = p_Main->getConfigBool("/StudioIV/DiagonalB2", true);
    
    if (gameAuto_)
        p_Main->loadKeyDefinition(p_Main->getRomFile(STUDIOIV, MAINROM1), p_Main->getRomFile(STUDIOIV, CARTROM), keyDefA1_, keyDefB1_, keyDefA2_, &simDefA2_, keyDefB2_, &simDefB2_, &inKey1_, &inKey2_, keyDefGameHexA_, keyDefGameHexB_, "keydefinition_studio.txt");
    
    reDefineKeysA(keyDefA1_, keyDefA2_);
    reDefineKeysB(keyDefB1_, keyDefB2_);
   
    addressLatch_ = 0x8000;

    setWait(1);
    setClear(0);
    setWait(1);
    setClear(1);
    initPixie();
}

void StudioIV::onReset()
{
    resetPressed_ = true;
}

void StudioIV::startComputerRun(bool WXUNUSED(load))
{
    p_Main->pload();
}

void StudioIV::sleepComputer(long ms)
{
    threadPointer->Sleep(ms);
}

void StudioIV::checkComputerFunction()
{
    switch (scratchpadRegister_[programCounter_])
    {
        case 0:
 //           pseudoType_ = p_Main->getPseudoDefinition(&chip8baseVar_, &chip8mainLoop_, &chip8register12bit_, &pseudoLoaded_);
        break;
         
        case 0xa00a:
            pseudoType_ = "AM4KBAS2020";
            chip8baseVar_ = 0x7b00;
            chip8mainLoop_ = 0xA0AB;
            chip8register12bit_ = false;
            pseudoLoaded_ = true;
            p_Main->forcePseudoDefinition(pseudoType_, "am2020bas.syntax", "AM4KBAS 2020");
        break;

        case 0xb00a:
            pseudoType_ = "AM4KBAS2020";
            chip8baseVar_ = 0x7b00;
            chip8mainLoop_ = 0xB0A3;
            chip8register12bit_ = false;
            pseudoLoaded_ = true;
            p_Main->forcePseudoDefinition(pseudoType_, "am2020bas.syntax", "AM4KBAS 2020");
        break;
    }
}

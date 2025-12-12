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

#if defined(__linux__)
#include "app_icon.xpm"
#endif

#include "main.h"
#include "scn2671.h"

// COMMUNICATION STATUS BITS
#define RECEIVER_READY 0
#define TRANSMITTER_READY 1
#define TRANSMITTER_EMPTY 2
#define TRANSMIT_BREAK 3
#define RECEIVER_BREAK 4
#define OVERRUN_ERROR 5
#define FRAMING_ERROR 6
#define PARITY_ERROR 7

// COMMUNICATION MODE BITS
#define WORD_LENGTH_SELECT_1 0
#define WORD_LENGTH_SELECT_2 1
#define STOP_BIT_SELECT 2
#define PARITY_MODE_1 3
#define PARITY_MODE_2 4
#define PARITY 5
#define OPERATING_MODE_1 6
#define OPERATING_MODE_2 7

// MISC COMMAND BITS
#define TRANSMIT_CHAR_BREAK 0
#define TRANSMIT_TIMED_BREAK 1
#define RING_TONE_SHORT 2
#define RING_TONE_LONG 3
#define SET_SHIFT_LOCK 4
#define CLEAR_SHIFT_LOCK 5
#define SET_KEYBOARD_ENABLE 6
#define CLEAR_KEYBOARD_ENABLE 7

// INTERRUPT MASK/STATUS BITS
#define INTERRUPT_RXRDY 0
#define INTERRUPT_KOVR 1
#define INTERRUPT_KRDY 2
#define INTERRUPT_KERR 3
#define INTERRUPT_XINT 4
#define INTERRUPT_BREAK_CHANGE 5
#define INTERRUPT_TXEMT 6
#define INTERRUPT_TXRDY 7

// KEYBOARD STATUS BITS
#define KEYBOARD_KOVR  0
#define KEYBOARD_KERR 1
#define KEYBOARD_KRDY 2
#define KEYBOARD_ENABLED 3
#define KEYBOARD_REPEAT 4
#define KEYBOARD_SHIFT_LOCK 5
#define KEYBOARD_SHIFT 6
#define KEYBOARD_CONTROL 7

// KEYBOARD MODE BITS
#define KEYBOARD_MODE_TONE_SELECT 0
#define KEYBOARD_MODE_KMR1 1
#define KEYBOARD_MODE_KMR2 2
#define KEYBOARD_MODE_REPEAT 3
#define KEYBOARD_MODE_ENCODE 4
#define KEYBOARD_MODE_ROLLOVER_1 5
#define KEYBOARD_MODE_ROLLOVER_2 6
#define KEYBOARD_MODE_TEST 7

#define REGISTER_FULL 0;
#define REGISTER_EMPTY 1;

int scn2671BaudRateValue_[] =
{
    50, 110, 134, 150, 200, 300, 600, 1050, 1200, 1800, 2000, 2400, 4800, 9600, 19200, 38400
};

#define KEYBOARD_CODE 0
#define KEYBOARD_MOD_CTRL 1
#define KEYBOARD_MOD_SHIFT 2

Byte scn2671KeyCodes[128][3] =
{
    /*   0 */    {0x36, 1, 0 }, {0x0C, 1, 0 }, {0x25, 1, 0 }, {0x15, 1, 0 }, {0x1C, 1, 0 }, // CTRL A to CTRL D
    /*   5 */    {0x1B, 1, 0 }, {0x24, 1, 0 }, {0x2C, 1, 0 }, {0x34, 1, 0 }, {0x43, 1, 0 }, // CTRL E to I
    /*  10 */    {0x3C, 1, 0 }, {0x44, 1, 0 }, {0x4C, 1, 0 }, {0x35, 1, 0 }, {0x2D, 1, 0 }, // CTRL J to N
    /*  15 */    {0x4B, 1, 0 }, {0x2E, 1, 0 }, {0x0B, 1, 0 }, {0x23, 1, 0 }, {0x14, 1, 0 }, // CTRL O to S
    /*  20 */    {0x2B, 1, 0 }, {0x3B, 1, 0 }, {0x1D, 1, 0 }, {0x13, 1, 0 }, {0x0D, 1, 0 }, // CTRL T to X
    /*  25 */    {0x33, 1, 0 }, {0x05, 1, 0 }, {0x1F, 1, 0 }, {0x1C, 1, 0 }, {0x27, 1, 0 }, // CTRL Y, Z, ESC, FS , GS
    /*  30 */    {0x16, 1, 0 }, {0x17, 1, 0 }, {0x3F, 0, 0 }, {0x0A, 0, 1 }, {0x12, 0, 1 }, // RS, US, Space, !, "
    /*  35 */    {0x1A, 0, 1 }, {0x22, 0, 1 }, {0x2A, 0, 1 }, {0x32, 0, 1 }, {0x3A, 0, 1 }, // #, $, %, &, '
    /*  40 */    {0x42, 0, 1 }, {0x4A, 0, 1 }, {0x0F, 0, 1 }, {0x07, 0, 1 }, {0x3D, 0, 0 }, // (, ), *, +, ,
    /*  45 */    {0x0E, 0, 0 }, {0x45, 0, 0 }, {0x4D, 0, 0 }, {0x30, 0, 0 }, {0x0A, 0, 0 }, // -, ., /, 0, 1
    /*  50 */    {0x12, 0, 0 }, {0x1A, 0, 0 }, {0x22, 0, 0 }, {0x2A, 0, 0 }, {0x32, 0, 0 }, // 2, 3, 4, 5, 6
    /*  55 */    {0x3A, 0, 0 }, {0x42, 0, 0 }, {0x4A, 0, 0 }, {0x0F, 0, 0 }, {0x07, 0, 0 }, // 7, 8, 9, :, ;
    /*  60 */    {0x3D, 0, 1 }, {0x0E, 0, 1 }, {0x45, 0, 1 }, {0x4D, 0, 1 }, {0x36, 0, 0 }, // <, =, >, ?, @
    /*  65 */    {0x0C, 0, 1 }, {0x25, 0, 1 }, {0x15, 0, 1 }, {0x1C, 0, 1 },                // A to D
    /*  69 */    {0x1B, 0, 1 }, {0x24, 0, 1 }, {0x2C, 0, 1 }, {0x34, 0, 1 }, {0x43, 0, 1 }, // E to I
    /*  74 */    {0x3C, 0, 1 }, {0x44, 0, 1 }, {0x4C, 0, 1 }, {0x35, 0, 1 }, {0x2D, 0, 1 }, // J to N
    /*  79 */    {0x4B, 0, 1 }, {0x2E, 0, 1 }, {0x0B, 0, 1 }, {0x23, 0, 1 }, {0x14, 0, 1 }, // O to S
    /*  84 */    {0x2B, 0, 1 }, {0x3B, 0, 1 }, {0x1D, 0, 1 }, {0x13, 0, 1 }, {0x0D, 0, 1 }, // T to X
    /*  89 */    {0x33, 0, 1 }, {0x05, 0, 1 }, {0x1F, 0, 0 }, {0x5C, 0, 0 }, {0x27, 0, 0 }, // Y, Z, [, \ , ]
    /*  94 */    {0x16, 0, 0 }, {0x17, 0, 0 }, {0x36, 0, 1 },                               // ^, _, `
    /*  97 */    {0x0C, 0, 0 }, {0x25, 0, 0 }, {0x15, 0, 0 }, {0x1C, 0, 0 },                // a to d
    /* 101 */    {0x1B, 0, 0 }, {0x24, 0, 0 }, {0x2C, 0, 0 }, {0x34, 0, 0 }, {0x43, 0, 0 }, // e to i
    /* 106 */    {0x3C, 0, 0 }, {0x44, 0, 0 }, {0x4C, 0, 0 }, {0x35, 0, 0 }, {0x2D, 0, 0 }, // j to n
    /* 111 */    {0x4B, 0, 0 }, {0x2E, 0, 0 }, {0x0B, 0, 0 }, {0x23, 0, 0 }, {0x14, 0, 0 }, // o to s
    /* 116 */    {0x2B, 0, 0 }, {0x3B, 0, 0 }, {0x1D, 0, 0 }, {0x13, 0, 0 }, {0x0D, 0, 0 }, // t to x
    /* 121 */    {0x33, 0, 0 }, {0x05, 0, 0 }, {0x1F, 0, 1 }, {0x00, 0, 0 }, {0x27, 0, 1 }, // y, z, {, |, }
    /* 126 */    {0x16, 0, 1 }, {0x17, 0, 1 },                                              // ~, DEL
};

Byte scn2671Vt4801KeyCodes[128][3] =
{
    /*   0 */    {0x00, 1, 0 },
    /*   1 */    {0x0C, 1, 0 }, {0x2A, 1, 0 }, {0x32, 1, 0 }, {0x34, 1, 0 },                // CTRL A to CTRL D
    /*   5 */    {0x31, 1, 0 }, {0x14, 1, 0 }, {0x2C, 1, 0 }, {0x24, 1, 0 }, {0x1e, 1, 0 }, // CTRL E to I
    /*  10 */    {0x1c, 1, 0 }, {0x1d, 1, 0 }, {0x25, 1, 0 }, {0x1A, 1, 0 }, {0x22, 1, 0 }, // CTRL J to N
    /*  15 */    {0x26, 1, 0 }, {0x2E, 1, 0 }, {0x09, 1, 0 }, {0x11, 1, 0 }, {0x3C, 1, 0 }, // CTRL O to S
    /*  20 */    {0x29, 1, 0 }, {0x19, 1, 0 }, {0x12, 1, 0 }, {0x39, 1, 0 }, {0x3A, 1, 0 }, // CTRL T to X
    /*  25 */    {0x21, 1, 0 }, {0x0A, 1, 0 }, {0x01, 0, 0 }, {0x1B, 1, 0 }, {0x17, 1, 0 }, // CTRL Y, Z, ESC, FS , GS
    /*  30 */    {0x07, 1, 0 }, {0x2B, 1, 0 }, {0x13, 0, 0 }, {0x08, 0, 1 }, {0x38, 0, 1 }, // RS, US, Space, !, "
    /*  35 */    {0x30, 0, 1 }, {0x10, 0, 1 }, {0x28, 0, 1 }, {0x20, 0, 1 }, {0x18, 0, 1 }, // #, $, %, &, '
    /*  40 */    {0x1F, 0, 1 }, {0x27, 0, 1 }, {0x15, 0, 1 }, {0x2D, 0, 1 }, {0x1B, 0, 0 }, // (, ), *, +, ,
    /*  45 */    {0x3D, 0, 0 }, {0x07, 0, 0 }, {0x2B, 0, 0 }, {0x2F, 0, 0 }, {0x08, 0, 0 }, // -, ., /, 0, 1
    /*  50 */    {0x38, 0, 0 }, {0x30, 0, 0 }, {0x10, 0, 0 }, {0x28, 0, 0 }, {0x20, 0, 0 }, // 2, 3, 4, 5, 6
    /*  55 */    {0x18, 0, 0 }, {0x1f, 0, 0 }, {0x27, 0, 0 }, {0x15, 0, 0 }, {0x2D, 0, 0 }, // 7, 8, 9, :, ;
    /*  60 */    {0x1B, 0, 1 }, {0x3D, 0, 1 }, {0x07, 0, 1 }, {0x2B, 0, 1 }, {0x16, 0, 0 }, // <, =, >, ?, @
    /*  65 */    {0x0C, 0, 1 }, {0x2A, 0, 1 }, {0x32, 0, 1 }, {0x34, 0, 1 },                // A to D
    /*  69 */    {0x31, 0, 1 }, {0x14, 0, 1 }, {0x2C, 0, 1 }, {0x24, 0, 1 }, {0x1e, 0, 1 }, // E to I
    /*  74 */    {0x1c, 0, 1 }, {0x1d, 0, 1 }, {0x25, 0, 1 }, {0x1A, 0, 1 }, {0x22, 0, 1 }, // J to N
    /*  79 */    {0x26, 0, 1 }, {0x2E, 0, 1 }, {0x09, 0, 1 }, {0x11, 0, 1 }, {0x3C, 0, 1 }, // O to S
    /*  84 */    {0x29, 0, 1 }, {0x19, 0, 1 }, {0x12, 0, 1 }, {0x39, 0, 1 }, {0x3A, 0, 1 }, // T to X
    /*  89 */    {0x21, 0, 1 }, {0x0A, 0, 1 }, {0x36, 0, 0 }, {0x3F, 0, 0 }, {0x3E, 0, 0 }, // Y, Z, [, \ , ]
    /*  94 */    {0x37, 0, 0 }, {0x0E, 0, 1 }, {0x16, 0, 1 },                               // ^, _, `
    /*  97 */    {0x0C, 0, 0 }, {0x2A, 0, 0 }, {0x32, 0, 0 }, {0x34, 0, 0 },                // a to d
    /* 101 */    {0x31, 0, 0 }, {0x14, 0, 0 }, {0x2C, 0, 0 }, {0x24, 0, 0 }, {0x1e, 0, 0 }, // e to i
    /* 106 */    {0x1c, 0, 0 }, {0x1d, 0, 0 }, {0x25, 0, 0 }, {0x1A, 0, 0 }, {0x22, 0, 0 }, // j to n
    /* 111 */    {0x26, 0, 0 }, {0x2E, 0, 0 }, {0x09, 0, 0 }, {0x11, 0, 0 }, {0x3C, 0, 0 }, // o to s
    /* 116 */    {0x29, 0, 0 }, {0x19, 0, 0 }, {0x12, 0, 0 }, {0x39, 0, 0 }, {0x3A, 0, 0 }, // t to x
    /* 121 */    {0x21, 0, 0 }, {0x0A, 0, 0 }, {0x36, 0, 1 }, {0x3F, 0, 1 }, {0x3E, 0, 1 }, // y, z, {, |, }
    /* 126 */    {0x37, 0, 1 }, {0x04, 1, 0 },                                              // ~, DEL
};

Scn2671Instance::Scn2671Instance(int scn2671Number)
{
    scn2671Number_ = scn2671Number;
    
    communicationModeRegister_ = 0;
    communicationsStatusRegister_ = 0;
    
    communicationsStatusRegister_[TRANSMITTER_EMPTY] = REGISTER_EMPTY;
    communicationsStatusRegister_[TRANSMITTER_READY] = REGISTER_EMPTY;
    
    interruptMaskRegister_ = 0;
    interruptStatusRegister_ = 0;
    keyboardStatusRegister_ = 0;
    keyboardModeRegister_ = 0;
    toneFrequency_ = 1000;

    clearToSend_ = true;
    
    terminalLoad_ = false;
    terminalSave_ = false;
    sendPacket_ = false;
    
    interruptEf_ = 1;
}

void Scn2671Instance::configureScn2671(Scn2671Configuration scn2671Configuration, double clock, long int numberOfScn2671s)
{
    scn2671Configuration_ = scn2671Configuration;
    wxString scn2671NumberString = "";
    if (numberOfScn2671s > 1)
        scn2671NumberString.Printf(" %d", scn2671Number_);
    
    p_Main->configureMessage(&scn2671Configuration.ioGroupVector, "SCN2671 Programmable Keyboard and Communication Controller" + scn2671NumberString);
    
    p_Computer->setCycleType(CYCLE_TYPE_UART, SCN2671_CYCLE);

    baudRateT_ = (int) ((((clock * 1000000) / 8) / scn2671BaudRateValue_[13])+scn2671Configuration.baudCorrectionR);
    baudRateR_ = (int) ((((clock * 1000000) / 8) / scn2671BaudRateValue_[13])+scn2671Configuration.baudCorrectionR);

    transmitterHoldingRegister_ = 0;
    receiverHoldingRegister_ = 0;

    serialDataOutputCount_ = -1;
    serialDataInputCount_ = -1;
    serialDataInputEf_ = 1;
    
    p_Computer->sendSerialBreakComputer(scn2671Configuration_.connection, false);
}

bool Scn2671Instance::ioGroupScn2671(int ioGroup)
{
    bool groupFound = false;
    
    if (scn2671Configuration_.ioGroupVector.size() == 0)
        groupFound = true;
    else
    {
        for (std::vector<int>::iterator ioGroupIterator = scn2671Configuration_.ioGroupVector.begin (); ioGroupIterator != scn2671Configuration_.ioGroupVector.end (); ++ioGroupIterator)
        {
            if (*ioGroupIterator == ioGroup)
                groupFound = true;
        }
    }
    return groupFound;
}

void Scn2671Instance::resetCommand(Byte value)
{
    int command = value >> 5;
    switch (command)
    {
        case 0: // No effect
        case 1: // No effect
        break;

        case 2: // Set RxE
        break;
            
        case 3: // Reset RxE
        break;

        case 4: // Set TxE
        break;
            
        case 5: // Reset TxE
        break;

        case 6: // Set TxE and RxE
        break;

        case 7: // Communication reset
            communicationModeRegister_ = 0;
            communicationsStatusRegister_ = 0;
            interruptMaskRegister_ &= 0x1e;
            if ((value & 0x1) == 0x1) // master reset
            {
                keyboardModeRegister_ = 0;
                toneFrequency_ = 1000;
                keyboardStatusRegister_ &= 0xd0;
                interruptMaskRegister_ = 0;
                return;
            }
        break;
    }
    if ((value & 0x1) == 0x1) // keyboard reset
    {
        keyboardModeRegister_ = 0;
        toneFrequency_ = 1000;
        keyboardStatusRegister_ &= 0xd0;
        interruptMaskRegister_ &= 0xf1;
    }
    if ((value & 0x2) == 0x2)   // KERR reset
        keyboardStatusRegister_[KEYBOARD_KERR] = 0;
    if ((value & 0x4) == 0x4)   // KOVR reset
        keyboardStatusRegister_[KEYBOARD_KOVR] = 0;
    if ((value & 0x8) == 0x8)   // Communication error reset
        communicationsStatusRegister_ &= 0x1f;
    if ((value & 0x10) == 0x10) // Break detect change reset
        interruptStatusRegister_[INTERRUPT_BREAK_CHANGE] = 0;
}

void Scn2671Instance::commandMisc(Byte value)
{
    miscCommandRegister_ = value;
    
    if (miscCommandRegister_[TRANSMIT_CHAR_BREAK] || miscCommandRegister_[TRANSMIT_TIMED_BREAK])
        p_Computer->sendSerialBreakComputer(scn2671Configuration_.connection, true);
    else
        p_Computer->sendSerialBreakComputer(scn2671Configuration_.connection, false);
    
    if (miscCommandRegister_[CLEAR_KEYBOARD_ENABLE])
        keyboardStatusRegister_[KEYBOARD_ENABLED] = 0;
    
    if (miscCommandRegister_[SET_KEYBOARD_ENABLE])
        keyboardStatusRegister_[KEYBOARD_ENABLED] = 1;
    
    if (miscCommandRegister_[RING_TONE_SHORT])
        p_Main->eventBeepTimer(toneFrequency_, 25);
    
    if (miscCommandRegister_[RING_TONE_LONG])
        p_Main->eventBeepTimer(toneFrequency_, 100);
}

void Scn2671Instance::uartCts(Byte value)
{
    switch (value)
    {
        case 1:
            if (terminalLoad_)
                dataAvailableUart(0);
            clearToSend_ = false;
        break;

        case 2:
            if (terminalLoad_ && sendPacket_)
                dataAvailableUart(1);
            clearToSend_ = true;
        break;
    }
}

void Scn2671Instance::writeCommunicationModeRegister(Byte value)
{
    communicationModeRegister_ = value;
    
    Byte charLengh = communicationModeRegister_.to_ulong() & 3;
    charLengh -= 1;
    vtOutBits_ = charLengh + 9;
    vtOutBits_ -= communicationModeRegister_[STOP_BIT_SELECT];
    vtOutBits_ -= communicationModeRegister_[PARITY_MODE_2];
    
    if (terminalLoad_ && communicationsStatusRegister_[RECEIVER_READY] && clearToSend_)
        dataAvailableUart(1);
}

Byte Scn2671Instance::readCommunicationModeRegister()
{
    return communicationModeRegister_.to_ulong();
}

void Scn2671Instance::writeBaudRateControlRegister(Byte value, double clock)
{
    baudRateT_ = (int) ((((clock * 1000000) / 8) / scn2671BaudRateValue_[value&0xf])+scn2671Configuration_.baudCorrectionR);
    baudRateR_ = (int) ((((clock * 1000000) / 8) / scn2671BaudRateValue_[value&0xf])+scn2671Configuration_.baudCorrectionR);
}
    
Byte Scn2671Instance::readCommunicationsStatusRegister()
{
    return communicationsStatusRegister_.to_ulong();
}

void Scn2671Instance::writeInterruptMaskRegister(Byte value)
{
    interruptMaskRegister_ = value;
	for (int maskNumber=0; maskNumber<8; maskNumber++)
		p_Computer->maskInterrupt(maskNumber+INTERRUPT_TYPE_SCN2671_RXRDY, interruptMaskRegister_[maskNumber]);
}

Byte Scn2671Instance::readInterruptMaskRegister()
{
    return interruptMaskRegister_.to_ulong();
}

Byte Scn2671Instance::readInterruptStatusRegister()
{
    return interruptStatusRegister_.to_ulong();
}

void Scn2671Instance::writeKeyboardModeRegister(Byte value)
{
    keyboardModeRegister_ = value;
    if (keyboardModeRegister_[KEYBOARD_MODE_TONE_SELECT])
        toneFrequency_ = 2000;
    else
        toneFrequency_ = 1000;
}

Byte Scn2671Instance::readKeyboardModeRegister()
{
    return keyboardModeRegister_.to_ulong();
}

Byte Scn2671Instance::readKeyboardStatusRegister()
{
    return keyboardStatusRegister_.to_ulong();
}

void Scn2671Instance::writeTransmitterHoldingRegister(Byte value)
{
    transmitterHoldingRegister_ = value;
    communicationsStatusRegister_[TRANSMITTER_READY] = REGISTER_FULL;
    communicationsStatusRegister_[TRANSMITTER_EMPTY] = REGISTER_FULL;
}

Byte Scn2671Instance::readReceiverHoldingRegister()
{
    framingError(0);
    dataAvailableUart(0);
    clearInterrupt(INTERRUPT_RXRDY);

    return p_Computer->readReceiverHoldingRegister(scn2671Number_);
}

void Scn2671Instance::charEventKeyboardScn2671(wxKeyEvent& event, int keycode)
{
    if (keyboardModeRegister_[KEYBOARD_MODE_ENCODE])
    {
        if (scn2671Configuration_.vt4801coding)
            KeyboardHoldingRegister_ = scn2671Vt4801KeyCodes[keycode&0x7f][KEYBOARD_CODE];
        else
            KeyboardHoldingRegister_ = scn2671KeyCodes[keycode&0x7f][KEYBOARD_CODE];
    }
    else
        KeyboardHoldingRegister_ = keycode;
    
    switch (keycode)
    {
        case WXK_F1:
        case WXK_F2:
        case WXK_F3:
        case WXK_F4:
        case WXK_F5:
        case WXK_F6:
        case WXK_F7:
        case WXK_F8:
        case WXK_F9:
        case WXK_F10:
        case WXK_F11:
        case WXK_F12:
            if (event.GetModifiers() == wxMOD_CONTROL)
            {
                KeyboardHoldingRegister_ = scn2671Configuration_.textKey[keycode-WXK_F1+MATRIX_TEXT_CTRL_F1_KEY].keyValue;
                keyboardStatusRegister_[KEYBOARD_CONTROL] = scn2671Configuration_.textKey[keycode-WXK_F1+MATRIX_TEXT_CTRL_F1_KEY].ctrlValue & 1;
                keyboardStatusRegister_[KEYBOARD_SHIFT] = scn2671Configuration_.textKey[keycode-WXK_F1+MATRIX_TEXT_CTRL_F1_KEY].shiftValue & 1;
            }
            else
            {
                KeyboardHoldingRegister_ = scn2671Configuration_.textKey[keycode-WXK_F1+MATRIX_TEXT_F1_KEY].keyValue;
                keyboardStatusRegister_[KEYBOARD_CONTROL] = scn2671Configuration_.textKey[keycode-WXK_F1+MATRIX_TEXT_F1_KEY].ctrlValue & 1;
                keyboardStatusRegister_[KEYBOARD_SHIFT] = scn2671Configuration_.textKey[keycode-WXK_F1+MATRIX_TEXT_F1_KEY].shiftValue & 1;
            }
        break;

        default:
            if (keycode <= 128)
            {
                keyboardStatusRegister_[KEYBOARD_CONTROL] = scn2671Vt4801KeyCodes[keycode][KEYBOARD_MOD_CTRL] & 1;
                keyboardStatusRegister_[KEYBOARD_SHIFT] = scn2671Vt4801KeyCodes[keycode][KEYBOARD_MOD_SHIFT] & 1;
            }
            else
            {
                keyboardStatusRegister_[KEYBOARD_CONTROL] = 0;
                keyboardStatusRegister_[KEYBOARD_SHIFT] = 0;
            }
        break;
    }

    keyboardStatusRegister_[KEYBOARD_KRDY] = 1;
    interrupt(INTERRUPT_KRDY);
}

Byte Scn2671Instance::readKeyboardHoldingRegister()
{
    clearInterrupt(INTERRUPT_KRDY);
    keyboardStatusRegister_[KEYBOARD_KRDY] = 0;
    return KeyboardHoldingRegister_;
}

void Scn2671Instance::cycle()
{
    if (serialDataInputCount_ > 0)
        serialDataInput();

    serialDataOutputCount_--;
    if (serialDataOutputCount_ <= 0)
        writeTransmitterShiftRegister_();
}

void Scn2671Instance::interrupt(int type)
{
    interruptStatusRegister_[type] = 1;
    p_Computer->requestInterrupt(INTERRUPT_TYPE_SCN2671_RXRDY+type, true, scn2671Configuration_.picInterrupt);
}

void Scn2671Instance::clearInterrupt(int type)
{
    interruptStatusRegister_[type] = 0;
    p_Computer->requestInterrupt(INTERRUPT_TYPE_SCN2671_RXRDY+type, false, scn2671Configuration_.picInterrupt);
}

void Scn2671Instance::serialDataInput()
{
    serialDataInputCount_--;
    if (serialDataInputCount_ == 0)
    {
        serialDataInputCount_ = -1;
//           dataAvailableUart(1);
    }
}

void Scn2671Instance::writeTransmitterShiftRegister_()
{
    if (communicationsStatusRegister_[TRANSMITTER_READY] == 0)
        p_Computer->serialDataOutput(scn2671Configuration_.connection, transmitterHoldingRegister_, scn2671Number_);
    else
        communicationsStatusRegister_[TRANSMITTER_EMPTY] = REGISTER_EMPTY;

    transmitterHoldingRegister_ = 0;
    communicationsStatusRegister_[TRANSMITTER_READY] = REGISTER_EMPTY;
    
    if (terminalSave_)
        serialDataOutputCount_ = baudRateR_ * 4;
    else
        serialDataOutputCount_ = baudRateR_ * 9;
}

Byte Scn2671Instance::uartThreStatus()
{
    return communicationsStatusRegister_[TRANSMITTER_READY];
}

void Scn2671Instance::framingError(bool data)
{
    communicationsStatusRegister_[FRAMING_ERROR] = data;
}

void Scn2671Instance::dataAvailable()
{
    serialDataInputCount_ = baudRateT_;
}

void Scn2671Instance::dataAvailable(Byte value)
{
    receiverHoldingRegister_ = value;
    serialDataInputCount_ = baudRateT_;

    communicationsStatusRegister_[RECEIVER_READY] = 1;
    interrupt(INTERRUPT_RXRDY);
}

void Scn2671Instance::dataAvailableUart(bool data)
{
    communicationsStatusRegister_[RECEIVER_READY] = data;
    if (data)
    {
        interrupt(INTERRUPT_RXRDY);
        serialDataInputCount_ = baudRateT_;
    }
}

void Scn2671Instance::setSendPacket(bool status)
{
    sendPacket_ = status;
}

void Scn2671Instance::setTerminalLoad(bool status)
{
    terminalLoad_ = status;
}

void Scn2671Instance::setTerminalSave(bool status)
{
    terminalSave_ = status;
}


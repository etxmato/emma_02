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
#include "cdp1854.h"

// UART STATUS BITS
#define DATA_AVAILABLE 0
#define OVERRUN_ERROR 1
#define PARITY_ERROR 2
#define FRAMING_ERROR 3
#define EXTERNAL_STATUS 4
#define PERIPHERAL_STATUS_INTERRUPT 5
#define TRANSMITTER_SHIFT_REGISTER 6
#define TRANSMITTER_HOLDING_REGISTER 7

// UART CONTROL BITS
#define PARITY_INHIBIT 0
#define EVEN_PARITY_ENABLE 1
#define STOP_BIT_SELECT 2
#define WORD_LENGTH_SELECT_1 3
#define WORD_LENGTH_SELECT_2 4
#define INTERRUPT_ENABLE 5
#define TRANSMIT_BREAK 6
#define TRANSMIT_REQUEST 7

#define REGISTER_FULL 0;
#define REGISTER_EMPTY 1;

int uartBaudRateValue_[] =
{
    38400, 19200, 9600, 4800, 3600, 2400, 2000, 1800, 1200, 600, 300, 200, 150, 134, 110, 75, 50
};

Cdp1854Instance::Cdp1854Instance(int cdp1854Number)
{
    cdp1854Number_ = cdp1854Number;
    
    controlRegister_ = 0;
    statusRegister_ = 0;

    statusRegister_[EXTERNAL_STATUS] = 1;
    statusRegister_[TRANSMITTER_SHIFT_REGISTER] = REGISTER_EMPTY;
    statusRegister_[TRANSMITTER_HOLDING_REGISTER] = REGISTER_EMPTY;

    useSdi_ = false;
    clearToSend_ = true;
    
    terminalLoad_ = false;
    terminalSave_ = false;
    sendPacket_ = false;
}

void Cdp1854Instance::configureCdp1854(Cdp1854Configuration cdp1854Configuration, double clock, long int numberOfCdp1854s)
{
    cdp1854Configuration_ = cdp1854Configuration;
    wxString cdp1854NumberString = "";
    if (numberOfCdp1854s > 1)
        cdp1854NumberString.Printf(" %d", cdp1854Number_);
    
    p_Main->configureMessage(&cdp1854Configuration.ioGroupVector, "CDP1854 UART" + cdp1854NumberString);
    
    p_Computer->setOutType(&cdp1854Configuration.ioGroupVector, cdp1854Configuration.out, "load transmitter", cdp1854Number_);
    p_Computer->setInType(&cdp1854Configuration.ioGroupVector, cdp1854Configuration.in, "read receiver", cdp1854Number_);
    p_Computer->setOutType(&cdp1854Configuration.ioGroupVector, cdp1854Configuration.control, "load control", cdp1854Number_);
    p_Computer->setInType(&cdp1854Configuration.ioGroupVector, cdp1854Configuration.status, "read status", cdp1854Number_);
    if (cdp1854Configuration_.efInterrupt.flagNumber != -1)
        p_Computer->setEfType(&cdp1854Configuration.ioGroupVector, cdp1854Configuration.efInterrupt, "UART interrupt", cdp1854Number_);
    if (cdp1854Configuration_.ef.flagNumber != -1)
    {
        p_Computer->setEfType(&cdp1854Configuration.ioGroupVector, cdp1854Configuration.ef, "serial data input", cdp1854Number_);
        useSdi_ = true;
    }
    p_Computer->setCycleType(CYCLE_TYPE_UART, UART1854_CYCLE);

    baudRateT_ = (int) ((((clock * 1000000) / 8) / uartBaudRateValue_[cdp1854Configuration.baudT])+cdp1854Configuration.baudCorrectionT);
    baudRateR_ = (int) ((((clock * 1000000) / 8) / uartBaudRateValue_[cdp1854Configuration.baudR])+cdp1854Configuration.baudCorrectionR);

    p_Main->message("");

    transmitterHoldingRegister_ = 0;
    receiverHoldingRegister_ = 0;

    serialDataOutputCount_ = -1;
    serialDataInputCount_ = -1;
    serialDataInputEf_ = 1;
    
    p_Computer->sendSerialBreakComputer(cdp1854Configuration_.connection, false);
}

bool Cdp1854Instance::ioGroupCdp1854(int ioGroup)
{
    bool groupFound = false;
    
    if (cdp1854Configuration_.ioGroupVector.size() == 0)
        groupFound = true;
    else
    {
        for (std::vector<int>::iterator ioGroupIterator = cdp1854Configuration_.ioGroupVector.begin (); ioGroupIterator != cdp1854Configuration_.ioGroupVector.end (); ++ioGroupIterator)
        {
            if (*ioGroupIterator == ioGroup)
                groupFound = true;
        }
    }
    return groupFound;
}

void Cdp1854Instance::uartCts(Byte value)
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

void Cdp1854Instance::writeControlRegister(Byte value)
{
    if ((value & 0x80) == 0x80) // TRANSMIT REQUEST
    {
        interrupt();
    }
    else
    {
        controlRegister_ = value;
        clearInterrupt();
        
        if (controlRegister_[TRANSMIT_BREAK])
            p_Computer->sendSerialBreakComputer(cdp1854Configuration_.connection, true);
        else
            p_Computer->sendSerialBreakComputer(cdp1854Configuration_.connection, false);
        
        vtOutBits_ = ((controlRegister_.to_ulong() & 0x18) >> 3) + 7;
        vtOutBits_ += controlRegister_[STOP_BIT_SELECT];
        vtOutBits_ -= controlRegister_[PARITY_INHIBIT];
    }
    
    statusRegister_[TRANSMITTER_HOLDING_REGISTER] = REGISTER_EMPTY;

    if (terminalLoad_ && statusRegister_[DATA_AVAILABLE] && clearToSend_)
        dataAvailableUart(1);
}

Byte Cdp1854Instance::readStatusRegister()
{
    clearInterrupt();
    return statusRegister_.to_ulong();
}

void Cdp1854Instance::writeTransmitterHoldingRegister(Byte value)
{
    transmitterHoldingRegister_ = value;
    statusRegister_[TRANSMITTER_HOLDING_REGISTER] = REGISTER_FULL;
    statusRegister_[TRANSMITTER_SHIFT_REGISTER] = REGISTER_FULL;
}

Byte Cdp1854Instance::readReceiverHoldingRegister_()
{
    framingError(0);
    dataAvailableUart(0);

    return p_Computer->readReceiverHoldingRegister(cdp1854Number_);
}

Byte Cdp1854Instance::efSerialDataInput()
{
    return (cdp1854Configuration_.ef.reverse^serialDataInputEf_);
}

Byte Cdp1854Instance::efInterrupt()
{
    return (cdp1854Configuration_.efInterrupt.reverse^interruptEf_);
}

void Cdp1854Instance::cycle()
{
    if (serialDataInputCount_ > 0)
        serialDataInput();

    serialDataOutputCount_--;
    if (serialDataOutputCount_ <= 0)
        writeTransmitterShiftRegister_();
}

void Cdp1854Instance::interrupt()
{    
    if (controlRegister_[INTERRUPT_ENABLE] && cdp1854Configuration_.interrupt)
        p_Computer->requestInterrupt(INTERRUPT_TYPE_UART, true, cdp1854Configuration_.picInterrupt);
    if (cdp1854Configuration_.efInterrupt.flagNumber != -1)
        interruptEf_ = 0;
}

void Cdp1854Instance::clearInterrupt()
{
    p_Computer->requestInterrupt(INTERRUPT_TYPE_UART, false, cdp1854Configuration_.picInterrupt);
    if (cdp1854Configuration_.efInterrupt.flagNumber != -1)
        interruptEf_ = 1;
}

void Cdp1854Instance::serialDataInput()
{
    serialDataInputCount_--;
    if (useSdi_)
    {
        if (serialDataInputCount_ <= 0)
        {
            if (vtOutBits_ == 10)
                serialDataInputEf_ = 0;
            else
            {
                serialDataInputEf_ = (receiverHoldingRegister_ & 1) ? 1 : 0;
                if (vtOutBits_ > 10)
                    receiverHoldingRegister_ = 0;
                else
                    receiverHoldingRegister_ = (receiverHoldingRegister_ >> 1) | 128;
            }
            serialDataInputCount_ = baudRateT_;
            if (vtOutBits_ == 2)
                serialDataInputEf_ = 1;
            if (--vtOutBits_ == 0)
            {
                receiverHoldingRegister_ = 0;
                serialDataInputCount_ = -1;
                vtOutBits_=10;
            }
            if (vtOutBits_ == 11)
            {
                serialDataInputEf_ = 1;
                serialDataInputCount_ = -1;
                vtOutBits_=10;
            }
        }
    }
    else
    {
        if (serialDataInputCount_ == 0)
        {
            serialDataInputCount_ = -1;
        }
    }
}

void Cdp1854Instance::writeTransmitterShiftRegister_()
{
    if (statusRegister_[TRANSMITTER_HOLDING_REGISTER] == 0)
        p_Computer->serialDataOutput(cdp1854Configuration_.connection, transmitterHoldingRegister_, cdp1854Number_);
    else
        statusRegister_[TRANSMITTER_SHIFT_REGISTER] = REGISTER_EMPTY;

    transmitterHoldingRegister_ = 0;
    statusRegister_[TRANSMITTER_HOLDING_REGISTER] = REGISTER_EMPTY;
    
    if (terminalSave_)
        serialDataOutputCount_ = baudRateR_ * 4;
    else
        serialDataOutputCount_ = baudRateR_ * 9;
}

Byte Cdp1854Instance::uartThreStatus()
{
    return statusRegister_[TRANSMITTER_HOLDING_REGISTER];
}

void Cdp1854Instance::framingError(bool data)
{
    statusRegister_[FRAMING_ERROR] = data;
}

void Cdp1854Instance::dataAvailable()
{
    if (useSdi_)
        serialDataInputCount_ = baudRateT_ * 9;
    else
        serialDataInputCount_ = baudRateT_;
}

void Cdp1854Instance::dataAvailable(Byte value)
{
    receiverHoldingRegister_ = value;
    if (useSdi_)
        serialDataInputCount_ = baudRateT_ * 9;
    else
        serialDataInputCount_ = baudRateT_;

    statusRegister_[DATA_AVAILABLE] = 1;
}

void Cdp1854Instance::dataAvailableUart(bool data)
{
    statusRegister_[DATA_AVAILABLE] = data;
    if (data)
    {
        interrupt();
        if (useSdi_)
            serialDataInputCount_ = baudRateT_ * 9;
        else
            serialDataInputCount_ = baudRateT_;
    }
}

void Cdp1854Instance::setSendPacket(bool status)
{
    sendPacket_ = status;
}

void Cdp1854Instance::setTerminalLoad(bool status)
{
    terminalLoad_ = status;
}

void Cdp1854Instance::setTerminalSave(bool status)
{
    terminalSave_ = status;
}


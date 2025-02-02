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

Cdp1854Instance::Cdp1854Instance(int cdp1854Number)
{
    cdp1854Number_ = cdp1854Number;
    
}

void Cdp1854Instance::configureCdp1854(Cdp1854Configuration cdp1854Configuration)
{
    cdp1854Configuration_ = cdp1854Configuration;
    wxString cdp1854NumberString = "";
    if (cdp1854Number_ > 0)
        cdp1854NumberString.Printf(" %d", cdp1854Number_);
    
    p_Main->configureMessage(&cdp1854Configuration.ioGroupVector, "CDP1854 UART" + cdp1854NumberString);
    
    p_Computer->setOutType(&cdp1854Configuration.ioGroupVector, cdp1854Configuration.out, UART1854_LOAD_TRANSMITTER_OUT, "load transmitter");
    p_Computer->setInType(&cdp1854Configuration.ioGroupVector, cdp1854Configuration.in, UART1854_READ_RECEIVER_IN, "read receiver");
    p_Computer->setOutType(&cdp1854Configuration.ioGroupVector, cdp1854Configuration.control, UART1854_LOAD_CONTROL_OUT, "load control");
    p_Computer->setInType(&cdp1854Configuration.ioGroupVector, cdp1854Configuration.status, UART1854_READ_STATUS_IN, "read status");
    if (currentComputerConfiguration.cdp1854Configuration.efInterrupt.flagNumber != -1)
        p_Computer->setEfType(&cdp1854Configuration.ioGroupVector, cdp1854Configuration.efInterrupt, VIDEO_TERMINAL_EF_INTERRUPT, "UART interrupt");
    if (currentComputerConfiguration.cdp1854Configuration.ef.flagNumber != -1)
    {
        p_Computer->setEfType(&cdp1854Configuration.ioGroupVector, cdp1854Configuration.ef, VIDEO_TERMINAL_EF, "serial input");
        uartEf_ = true;
    }
    p_Computer->setCycleType(CYCLE_TYPE_VIDEO_TERMINAL, VIDEO_TERMINAL_CYCLE);

    p_Main->message("");

    rs232_ = 0;
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
            cts_ = false;
        break;

        case 2:
            if (terminalLoad_ && sendPacket_)
                dataAvailableUart(1);
            cts_ = true;
        break;
    }
}

void Cdp1854Instance::uartOut(Byte value)
{
    rs232_ = value;
    p_Computer->thrStatusVt100(1);
    uartStatus_[uart_thre_bit_] = 0;
    uartStatus_[uart_tsre_bit_] = 0;
    receivePacket_ = true;
}

void Cdp1854Instance::uartControl(Byte value)
{
    if ((value & 0x80) == 0x80)
    {
        uartInterrupt();
    }
    else
    {
        uartControl_ = value;
        clearUartInterrupt();
    }
    
    uartStatus_[uart_thre_bit_] = 1;

    if (terminalLoad_ && uartStatus_[uart_da_bit_] && cts_)
        dataAvailableUart(1);
}

Byte Cdp1854Instance::uartIn()
{
    framingError(0);
    dataAvailableUart(0);

    if (ctrlvText_ != 0)
    {
        videoScreenPointer->getKey(0);
        return checkCtrlvTextUart();
    }
    else
    {
        Byte loadByte = 0;
        if (terminalLoad_ || (terminalSave_ && (protocol_ == TERM_XMODEM_SAVE || protocol_ == TERM_YMODEM_SAVE)))
        {
            getTerminalLoadByte(&loadByte);
            return loadByte;
        }
        else
            return videoScreenPointer->getKey(0);
    }
}

Byte Cdp1854Instance::uartStatus()
{
    clearUartInterrupt();
    return uartStatus_.to_ulong();
}

Byte Cdp1854Instance::uartThreStatus()
{
    return uartStatus_[uart_thre_bit_];
}

void Cdp1854Instance::uartInterrupt()
{    
    if ((uartControl_ & 0x20) == 0x20 && currentComputerConfiguration.videoTerminalConfiguration.interrupt)
        p_Computer->requestInterrupt(INTERRUPT_TYPE_UART, true, currentComputerConfiguration.videoTerminalConfiguration.picInterrupt);
    if (currentComputerConfiguration.videoTerminalConfiguration.efInterrupt.flagNumber != -1)
        vt100EfInterrupt_ = 0;
}

void Cdp1854Instance::clearUartInterrupt()
{
    p_Computer->requestInterrupt(INTERRUPT_TYPE_UART, false, currentComputerConfiguration.videoTerminalConfiguration.picInterrupt);
    if (currentComputerConfiguration.videoTerminalConfiguration.efInterrupt.flagNumber != -1)
        vt100EfInterrupt_ = 1;
}
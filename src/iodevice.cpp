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

#include "main.h"
#include "iodevice.h"

IoDevice::IoDevice()
{
}

void IoDevice::initIo()
{
    for (int group=0; group<257; group++)
    {
        for (int io=0; io<5; io++)
        {
            for (int q=0; q<2; q++)
            {
                efType_[q][group][io] = 0;
            }
        }
        for (int io=0; io<8; io++)
        {
            for (int q=0; q<2; q++)
            {
                inType_[q][group][io] = 0;
                outType_[q][group][io] = 0;
            }
        }
    }
    for (int i=0; i<MAXCYCLE; i++)
    {
        cycleType_ [i] = 0;
    }
}

void IoDevice::setEfType(int number, int efType)
{
    for (int q=0; q<2; q++)
        for (int iogroup=0; iogroup<257; iogroup++)
            efType_[q][iogroup][number] = efType;
}

void IoDevice::setEfType(int iogroup, int number, int efType)
{
    if (iogroup == 0)
        setEfType(number, efType);
    else
    {
        for (int q=0; q<2; q++)
            efType_[q][iogroup][number] = efType;
    }
}

void IoDevice::setEfType(int q, int iogroup, int number, int efType)
{
    if (q == -1)
        setEfType(iogroup, number, efType);
    else
    {
        if (iogroup == 0)
            for (int group=0; group<257; group++)
                efType_[q][group][number] = efType;
        else
        {
            efType_[q][iogroup][number] = efType;
        }
    }
}

void IoDevice::setInType(int number, int inType)
{
    for (int q=0; q<2; q++)
        for (int iogroup=0; iogroup<257; iogroup++)
            inType_[q][iogroup][number] = inType;
}

void IoDevice::setInType(int iogroup, int number, int inType)
{
    if (iogroup == 0)
        setInType(number, inType);
    else
    {
        for (int q=0; q<2; q++)
            inType_[q][iogroup][number] = inType;
    }
}

void IoDevice::setInType(int q, int iogroup, int number, int inType)
{
    if (q == -1)
        setInType(iogroup, number, inType);
    else
    {
        if (iogroup == 0)
            for (int group=0; group<257; group++)
                inType_[q][group][number] = inType;
        else
        {
            inType_[q][iogroup][number] = inType;
        }
    }
}

wxString IoDevice::setInType(int q, int iogroup, IoPort port, int inType)
{
    wxString inputPorts = "";
    if (port.mask == 0xff)
    {
        setOutType(q, iogroup, port.portNumber, inType);
        inputPorts.Printf("%d", port.portNumber);
        return inputPorts;
    }
    for (int number = 0; number < 8; number++)
    {
        if ((number & port.mask) == port.mask)
        {
            setInType(q, iogroup, number, inType);
            if (inputPorts != "")
                inputPorts += ", ";
            inputPorts.Printf(inputPorts + "%d", number);
        }
    }
    
    return inputPorts;
}

void IoDevice::setOutType(int number, int outType)
{
    for (int q=0; q<2; q++)
        for (int iogroup=0; iogroup<257; iogroup++)
            outType_[q][iogroup][number] = outType;
}

void IoDevice::setOutType(int iogroup, int number, int outType)
{
    if (iogroup == 0)
        setOutType(number, outType);
    else
    {
        for (int q=0; q<2; q++)
            outType_[q][iogroup][number] = outType;
    }
}

void IoDevice::setOutType(int q, int iogroup, int number, int outType)
{
    if (q == -1)
        setOutType(iogroup, number, outType);
    else
    {
        if (iogroup == 0)
            for (int group=0; group<257; group++)
                outType_[q][group][number] = outType;
        else
        {
            outType_[q][iogroup][number] = outType;
        }
    }
}

wxString IoDevice::setOutType(int q, int iogroup, IoPort port, int outType)
{
    wxString outputPorts = "";
    if (port.mask == 0xff)
    {
        setOutType(q, iogroup, port.portNumber, outType);
        outputPorts.Printf("%d", port.portNumber);
        return outputPorts;
    }
    for (int number = 0; number < 8; number++)
    {
        if ((number & port.mask) == port.mask)
        {
            setOutType(q, iogroup, number, outType);
            if (outputPorts != "")
                outputPorts += ", ";
            outputPorts.Printf(outputPorts + "%d", number);
        }
    }
    
    return outputPorts;
}

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
                efItemNumber_[q][group][io] = 0;
            }
        }
        for (int io=0; io<8; io++)
        {
            for (int q=0; q<2; q++)
            {
                inType_[q][group][io] = 0;
                inItemNumber_[q][group][io] = 0;
                outType_[q][group][io] = 0;
                outItemNumber_[q][group][io] = 0;
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

void IoDevice::setEfTypeAndNumber(int number, int efType, int itemNumber)
{
    for (int q=0; q<2; q++)
        for (int iogroup=0; iogroup<257; iogroup++)
        {
            efType_[q][iogroup][number] = efType;
            efItemNumber_[q][iogroup][number] = itemNumber;
        }
}

void IoDevice::setEfTypeAndNumber(int iogroup, int number, int efType, int itemNumber)
{
    if (iogroup == 0)
        setEfTypeAndNumber(number, efType, itemNumber);
    else
    {
        for (int q=0; q<2; q++)
        {
            efType_[q][iogroup][number] = efType;
            efItemNumber_[q][iogroup][number] = itemNumber;
        }
    }
}

void IoDevice::setEfTypeAndNumber(int q, int iogroup, int number, int efType, int itemNumber)
{
    if (q == -1)
        setEfTypeAndNumber(iogroup, number, efType, itemNumber);
    else
    {
        if (iogroup == 0)
            for (int group=0; group<257; group++)
            {
                efType_[q][group][number] = efType;
                efItemNumber_[q][iogroup][number] = itemNumber;
            }
        else
        {
            efType_[q][iogroup][number] = efType;
            efItemNumber_[q][iogroup][number] = itemNumber;
        }
    }
}

void IoDevice::setEfTypeAndNumber(int q, int iogroup, int number, int efType, int itemNumber, wxString message)
{
    if (number == -1)
        return;
    
    wxString efPorts = "";
    wxString efQtext;
    
    if (q == -1)
        efQtext = "	EF ";
    else
        efQtext.Printf("	Q = %d & EF ", q);

    if (q == -1)
        setEfTypeAndNumber(iogroup, number, efType, itemNumber);
    else
    {
        if (iogroup == 0)
            for (int group=0; group<257; group++)
            {
                efType_[q][group][number] = efType;
                efItemNumber_[q][iogroup][number] = itemNumber;
            }
        else
        {
            efType_[q][iogroup][number] = efType;
            efItemNumber_[q][iogroup][number] = itemNumber;
        }
    }
    
    efPorts.Printf("%d", number);
    p_Main->message(efQtext + efPorts + ": " + message);
}

void IoDevice::setInTypeAndNumber(int number, int inType, int itemNumber)
{
    for (int q=0; q<2; q++)
        for (int iogroup=0; iogroup<257; iogroup++)
        {
            inType_[q][iogroup][number] = inType;
            inItemNumber_[q][iogroup][number] = itemNumber;
        }
}

void IoDevice::setInTypeAndNumber(int iogroup, int number, int inType, int itemNumber)
{
    if (iogroup == 0)
        setInTypeAndNumber(number, inType, itemNumber);
    else
    {
        for (int q=0; q<2; q++)
        {
            inType_[q][iogroup][number] = inType;
            inItemNumber_[q][iogroup][number] = itemNumber;
        }
    }
}

void IoDevice::setInTypeAndNumber(int q, int iogroup, int number, int inType, int itemNumber)
{
    if (q == -1)
        setInTypeAndNumber(iogroup, number, inType, itemNumber);
    else
    {
        if (iogroup == 0)
            for (int group=0; group<257; group++)
            {
                inType_[q][group][number] = inType;
                inItemNumber_[q][iogroup][number] = itemNumber;
            }
        else
        {
            inType_[q][iogroup][number] = inType;
            inItemNumber_[q][iogroup][number] = itemNumber;
        }
    }
}

void IoDevice::setInTypeAndNumber(int iogroup, IoPort port, int inType, int itemNumber, wxString message)
{
    if (port.portNumber == -1)
        return;
    
    wxString inputPorts = "";
    wxString inputQtext;
    
    if (port.qValue == -1)
        inputQtext = "	Input ";
    else
        inputQtext.Printf("	Q = %d & input ", port.qValue);

    if (port.mask == 0xff)
    {
        setInTypeAndNumber(port.qValue, iogroup, port.portNumber, inType, itemNumber);
        inputPorts.Printf("%d", port.portNumber);
    }
    else
    {
        for (int number = 0; number < 8; number++)
        {
            if ((number & port.mask) == (port.mask ^ port.xorMask))
            {
                setInTypeAndNumber(port.qValue, iogroup, number, inType, itemNumber);
                if (inputPorts != "")
                    inputPorts += ", ";
                inputPorts.Printf(inputPorts + "%d", number);
            }
        }
    }
    
    p_Main->message(inputQtext + inputPorts + ": " + message);
}

void IoDevice::setOutTypeAndNumber(int number, int outType, int itemNumber)
{
    for (int q=0; q<2; q++)
        for (int iogroup=0; iogroup<257; iogroup++)
        {
            outType_[q][iogroup][number] = outType;
            outItemNumber_[q][iogroup][number] = itemNumber;
        }
}

void IoDevice::setOutTypeAndNumber(int iogroup, int number, int outType, int itemNumber)
{
    if (iogroup == 0)
        setOutTypeAndNumber(number, outType, itemNumber);
    else
    {
        for (int q=0; q<2; q++)
        {
            outType_[q][iogroup][number] = outType;
            outItemNumber_[q][iogroup][number] = itemNumber;
        }
    }
}

void IoDevice::setOutTypeAndNumber(int q, int iogroup, int number, int outType, int itemNumber)
{
    if (q == -1)
        setOutTypeAndNumber(iogroup, number, outType, itemNumber);
    else
    {
        if (iogroup == 0)
            for (int group=0; group<257; group++)
            {
                outType_[q][group][number] = outType;
                outItemNumber_[q][iogroup][number] = itemNumber;
            }
        else
        {
            outType_[q][iogroup][number] = outType;
            outItemNumber_[q][iogroup][number] = itemNumber;
        }
    }
}

void IoDevice::setOutTypeAndNumber(int iogroup, IoPort port, int outType, int itemNumber, wxString message)
{
    if (port.portNumber == -1)
        return;
    
    wxString outputPorts = "";
    wxString outputQtext;
    
    if (port.qValue == -1)
        outputQtext = "	Output ";
    else
        outputQtext.Printf("	Q = %d & output ", port.qValue);

    if (port.mask == 0xff)
    {
        setOutTypeAndNumber(port.qValue, iogroup, port.portNumber, outType, itemNumber);
        outputPorts.Printf("%d", port.portNumber);
    }
    else
    {
        for (int number = 0; number < 8; number++)
        {
            if ((number & port.mask) == (port.mask ^ port.xorMask))
            {
                setOutTypeAndNumber(port.qValue, iogroup, number, outType, itemNumber);
                if (outputPorts != "")
                    outputPorts += ", ";
                outputPorts.Printf(outputPorts + "%d", number);
            }
        }
    }
    
    p_Main->message(outputQtext + outputPorts + ": " + message);
}


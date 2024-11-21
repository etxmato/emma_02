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
    for (int i=0; i<CYCLE_TYPE_MAX; i++)
    {
        cycleType_ [i] = 0;
    }
}

void IoDevice::setCycleType(int number, int outCycleType)
{
    cycleType_[number] = outCycleType;
}

void IoDevice::setEfType(int number, int efType, int itemNumber)
{
    for (int q=0; q<2; q++)
        for (int iogroup=0; iogroup<257; iogroup++)
        {
            efType_[q][iogroup][number] = efType;
            efItemNumber_[q][iogroup][number] = itemNumber;
        }
}

void IoDevice::setEfType(vector<int>* ioGroup, int number, int efType, bool excludeIoGroup, int itemNumber)
{
    if (ioGroup->size() == 0 || excludeIoGroup)
        setEfType(number, efType, itemNumber);
    else
    {
        for (int q=0; q<2; q++)
        {
            for (std::vector<int>::iterator ioGroupIterator = ioGroup->begin (); ioGroupIterator != ioGroup->end (); ++ioGroupIterator)
            {
                efType_[q][*ioGroupIterator+1][number] = efType;
                efItemNumber_[q][*ioGroupIterator+1][number] = itemNumber;
            }
        }
    }
}

void IoDevice::setEfType(vector<int>* ioGroup, EfFlag efFlag, int efType, wxString message, int itemNumber)
{
    efFlag.ioDefinition = efType;
    setEfType(ioGroup, efFlag, message, itemNumber);
}

void IoDevice::setEfType(vector<int>* ioGroup, EfFlag efFlag, wxString message, int itemNumber)
{
    if (efFlag.flagNumber == -1)
        return;
    
    wxString efPorts, efQtext, reversedText = "";
    
    if (efFlag.qValue == -1)
        efQtext = "	EF ";
    else
        efQtext.Printf("	Q = %d & EF ", efFlag.qValue);

    if (efFlag.reverse == 1)
        reversedText = " (reversed)";

    if (efFlag.qValue == -1)
        setEfType(ioGroup, efFlag.flagNumber, efFlag.ioDefinition, efFlag.excludeIoGroup, itemNumber);
    else
    {
        if (ioGroup->size() == 0 || efFlag.excludeIoGroup)
        {
            for (int group=0; group<257; group++)
            {
                efType_[efFlag.qValue][group][efFlag.flagNumber] = efFlag.ioDefinition;
                efItemNumber_[efFlag.qValue][group][efFlag.flagNumber] = itemNumber;
            }
        }
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = ioGroup->begin (); ioGroupIterator != ioGroup->end (); ++ioGroupIterator)
            {
                efType_[efFlag.qValue][*ioGroupIterator+1][efFlag.flagNumber] = efFlag.ioDefinition;
                efItemNumber_[efFlag.qValue][*ioGroupIterator+1][efFlag.flagNumber] = itemNumber;
            }
        }
    }
    
    efPorts.Printf("%d", efFlag.flagNumber);
    p_Main->message(efQtext + efPorts + ": " + message + reversedText);
}

void IoDevice::setInType(int number, int inType, int itemNumber)
{
    if (inType == 0)
        return;

    for (int q=0; q<2; q++)
        for (int iogroup=0; iogroup<257; iogroup++)
        {
            inType_[q][iogroup][number] = inType;
            inItemNumber_[q][iogroup][number] = itemNumber;
        }
}

void IoDevice::setInType(vector<int>* ioGroup, int number, int inType, int itemNumber)
{
    if (inType == 0)
        return;
    
    if (ioGroup->size() == 0)
        setInType(number, inType, itemNumber);
    else
    {
        for (std::vector<int>::iterator ioGroupIterator = ioGroup->begin (); ioGroupIterator != ioGroup->end (); ++ioGroupIterator)
        {
            for (int q=0; q<2; q++)
            {
                inType_[q][*ioGroupIterator+1][number] = inType;
                inItemNumber_[q][*ioGroupIterator+1][number] = itemNumber;
            }
        }
    }
}

void IoDevice::setInType(int q, vector<int>* ioGroup, int number, int inType, int itemNumber)
{
    if (inType == 0)
        return;
    
    if (q == -1)
        setInType(ioGroup, number, inType, itemNumber);
    else
    {
        if (ioGroup->size() == 0)
        {
            for (int group=0; group<257; group++)
            {
                inType_[q][group][number] = inType;
                inItemNumber_[q][group][number] = itemNumber;
            }
        }
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = ioGroup->begin (); ioGroupIterator != ioGroup->end (); ++ioGroupIterator)
            {
                inType_[q][*ioGroupIterator+1][number] = inType;
                inItemNumber_[q][*ioGroupIterator+1][number] = itemNumber;
            }
        }
    }
}

void IoDevice::setInType(vector<int>* ioGroup, IoPort port, int inType, wxString message, int itemNumber)
{
    port.ioDefinition = inType;
    setInType(ioGroup, port, message, itemNumber);
}

void IoDevice::setInType(vector<int>* ioGroup, IoPort port, wxString message, int itemNumber)
{
    if (port.portNumber[0] == -1)
        return;
    
    wxString inputPorts = "";
    wxString inputQtext;
    
    if (port.qValue == -1)
        inputQtext = "	Input ";
    else
        inputQtext.Printf("	Q = %d & input ", port.qValue);

    for (std::vector<int>::iterator portNumber = port.portNumber.begin (); portNumber != port.portNumber.end (); ++portNumber)
    {
        setInType(port.qValue, ioGroup, *portNumber, port.ioDefinition, itemNumber);
        if (inputPorts != "")
            inputPorts += ", ";
        inputPorts.Printf(inputPorts + "%d", *portNumber);
    }
    
    p_Main->message(inputQtext + inputPorts + ": " + message);
}

void IoDevice::setOutType(int number, int outType, int itemNumber)
{
    if (outType == 0)
        return;

    for (int q=0; q<2; q++)
        for (int iogroup=0; iogroup<257; iogroup++)
        {
            outType_[q][iogroup][number] = outType;
            outItemNumber_[q][iogroup][number] = itemNumber;
        }
}

void IoDevice::setOutType(vector<int>* ioGroup, int number, int outType, int itemNumber)
{
    if (outType == 0)
        return;

    if (ioGroup->size() == 0)
        setOutType(number, outType, itemNumber);
    else
    {
        for (int q=0; q<2; q++)
        {
            for (std::vector<int>::iterator ioGroupIterator = ioGroup->begin (); ioGroupIterator != ioGroup->end (); ++ioGroupIterator)
            {
                outType_[q][*ioGroupIterator+1][number] = outType;
                outItemNumber_[q][*ioGroupIterator+1][number] = itemNumber;
            }
        }
    }
}

void IoDevice::setOutType(int q, vector<int>* ioGroup, int number, int outType, int itemNumber)
{
    if (outType == 0)
        return;

    if (q == -1)
        setOutType(ioGroup, number, outType, itemNumber);
    else
    {
        if (ioGroup->size() == 0)
        {
            for (int group=0; group<257; group++)
            {
                outType_[q][group][number] = outType;
                outItemNumber_[q][group][number] = itemNumber;
            }
        }
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = ioGroup->begin (); ioGroupIterator != ioGroup->end (); ++ioGroupIterator)
            {
                outType_[q][*ioGroupIterator+1][number] = outType;
                outItemNumber_[q][*ioGroupIterator+1][number] = itemNumber;
            }
        }
    }
}

void IoDevice::setOutType(vector<int>* ioGroup, IoPort port, int outType, wxString message, int itemNumber)
{
    port.ioDefinition = outType;
    setOutType(ioGroup, port, message, itemNumber);
}

void IoDevice::setOutType(vector<int>* ioGroup, IoPort port, wxString message, int itemNumber)
{
    if (port.portNumber[0] == -1)
        return;
    
    wxString outputPorts = "";
    wxString outputQtext;
    
    if (port.qValue == -1)
        outputQtext = "	Output ";
    else
        outputQtext.Printf("	Q = %d & output ", port.qValue);

    for (std::vector<int>::iterator portNumber = port.portNumber.begin (); portNumber != port.portNumber.end (); ++portNumber)
    {
        setOutType(port.qValue, ioGroup, *portNumber, port.ioDefinition, itemNumber);
        if (outputPorts != "")
            outputPorts += ", ";
        outputPorts.Printf(outputPorts + "%d", *portNumber);
    }
    
    p_Main->message(outputQtext + outputPorts + ": " + message);
}


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
    outputConfiguration.resize(1);
    inputConfiguration.resize(2);

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
                for (int confNumber=0; confNumber<2; confNumber++)
                {
                    inputConfiguration[confNumber].type[q][group][io] = 0;
                    inputConfiguration[confNumber].itemNumber[q][group][io] = 0;
                }
                outputConfiguration[0].type[q][group][io] = 0;
                outputConfiguration[0].itemNumber[q][group][io] = 0;
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

    size_t inputConfigurationNumber;
    for (int q=0; q<2; q++)
    {
        for (int iogroup=0; iogroup<257; iogroup++)
        {
            inputConfigurationNumber = getInputConfiguration(q, iogroup, number);
            inputConfiguration[inputConfigurationNumber].type[q][iogroup][number] = inType;
            inputConfiguration[inputConfigurationNumber].itemNumber[q][iogroup][number] = itemNumber;
        }
    }
}

void IoDevice::setInType(vector<int>* ioGroup, int number, int inType, int itemNumber)
{
    if (inType == 0)
        return;
    
    size_t inputConfigurationNumber;
    if (ioGroup->size() == 0)
        setInType(number, inType, itemNumber);
    else
    {
        for (std::vector<int>::iterator ioGroupIterator = ioGroup->begin (); ioGroupIterator != ioGroup->end (); ++ioGroupIterator)
        {
            for (int q=0; q<2; q++)
            {
                inputConfigurationNumber = getInputConfiguration(q, *ioGroupIterator+1, number);
                inputConfiguration[inputConfigurationNumber].type[q][*ioGroupIterator+1][number] = inType;
                inputConfiguration[inputConfigurationNumber].itemNumber[q][*ioGroupIterator+1][number] = itemNumber;
            }
        }
    }
}

void IoDevice::setInType(int q, vector<int>* ioGroup, int number, int inType, int itemNumber)
{
    if (inType == 0)
        return;
    
    size_t inputConfigurationNumber;
    if (q == -1)
        setInType(ioGroup, number, inType, itemNumber);
    else
    {
        if (ioGroup->size() == 0)
        {
            for (int group=0; group<257; group++)
            {
                inputConfigurationNumber = getInputConfiguration(q, group, number);
                inputConfiguration[inputConfigurationNumber].type[q][group][number] = inType;
                inputConfiguration[inputConfigurationNumber].itemNumber[q][group][number] = itemNumber;
            }
        }
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = ioGroup->begin (); ioGroupIterator != ioGroup->end (); ++ioGroupIterator)
            {
                inputConfigurationNumber = getInputConfiguration(q, *ioGroupIterator+1, number);
                inputConfiguration[inputConfigurationNumber].type[q][*ioGroupIterator+1][number] = inType;
                inputConfiguration[inputConfigurationNumber].itemNumber[q][*ioGroupIterator+1][number] = itemNumber;
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
    {
        if (port.addressMode)
            inputQtext = "	Read ";
        else
            inputQtext = "	Input ";
    }
    else
        inputQtext.Printf("	Q = %d & input ", port.qValue);

    for (std::vector<int>::iterator portNumber = port.portNumber.begin (); portNumber != port.portNumber.end (); ++portNumber)
    {
        if (!port.addressMode)
            setInType(port.qValue, ioGroup, *portNumber, port.ioDefinition, itemNumber);
        if (inputPorts != "")
            inputPorts += ", ";
        if (port.addressMode)
            inputPorts.Printf(inputPorts + "address %04X", *portNumber);
        else
            inputPorts.Printf(inputPorts + "%d", *portNumber);
    }
    
    p_Main->message(inputQtext + inputPorts + ": " + message);
}

void IoDevice::setOutType(int number, int outType, int itemNumber)
{
    if (outType == 0)
        return;

    size_t outputConfigurationNumber;
    for (int q=0; q<2; q++)
    {
        for (int iogroup=0; iogroup<257; iogroup++)
        {
            outputConfigurationNumber = getOutputConfiguration(q, iogroup, number);
            outputConfiguration[outputConfigurationNumber].type[q][iogroup][number] = outType;
            outputConfiguration[outputConfigurationNumber].itemNumber[q][iogroup][number] = itemNumber;
        }
    }
}

void IoDevice::setOutType(vector<int>* ioGroup, int number, int outType, int itemNumber)
{
    if (outType == 0)
        return;

    size_t outputConfigurationNumber;
    if (ioGroup->size() == 0)
        setOutType(number, outType, itemNumber);
    else
    {
        for (int q=0; q<2; q++)
        {
            for (std::vector<int>::iterator ioGroupIterator = ioGroup->begin (); ioGroupIterator != ioGroup->end (); ++ioGroupIterator)
            {
                outputConfigurationNumber = getOutputConfiguration(q, *ioGroupIterator+1, number);
                outputConfiguration[outputConfigurationNumber].type[q][*ioGroupIterator+1][number] = outType;
                outputConfiguration[outputConfigurationNumber].itemNumber[q][*ioGroupIterator+1][number] = itemNumber;
            }
        }
    }
}

void IoDevice::setOutType(int q, vector<int>* ioGroup, int number, int outType, int itemNumber)
{
    if (outType == 0)
        return;

    size_t outputConfigurationNumber;
    if (q == -1)
        setOutType(ioGroup, number, outType, itemNumber);
    else
    {
        if (ioGroup->size() == 0)
        {
            for (int group=0; group<257; group++)
            {
                outputConfigurationNumber = getOutputConfiguration(q, group, number);
                outputConfiguration[outputConfigurationNumber].type[q][group][number] = outType;
                outputConfiguration[outputConfigurationNumber].itemNumber[q][group][number] = itemNumber;
            }
        }
        else
        {
            for (std::vector<int>::iterator ioGroupIterator = ioGroup->begin (); ioGroupIterator != ioGroup->end (); ++ioGroupIterator)
            {
                outputConfigurationNumber = getOutputConfiguration(q, *ioGroupIterator+1, number);
                outputConfiguration[outputConfigurationNumber].type[q][*ioGroupIterator+1][number] = outType;
                outputConfiguration[outputConfigurationNumber].itemNumber[q][*ioGroupIterator+1][number] = itemNumber;
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
    {
        if (port.addressMode)
            outputQtext = "	Write ";
        else
            outputQtext = "	Output ";
    }
    else
        outputQtext.Printf("	Q = %d & output ", port.qValue);

    for (std::vector<int>::iterator portNumber = port.portNumber.begin (); portNumber != port.portNumber.end (); ++portNumber)
    {
        if (!port.addressMode)
            setOutType(port.qValue, ioGroup, *portNumber, port.ioDefinition, itemNumber);
        if (outputPorts != "")
            outputPorts += ", ";
        if (port.addressMode)
            outputPorts.Printf(outputPorts + "address %04X", *portNumber);
        else
            outputPorts.Printf(outputPorts + "%d", *portNumber);
    }
    
    p_Main->message(outputQtext + outputPorts + ": " + message);
}

size_t IoDevice::getInputConfiguration(int q, int group, int number)
{
    if (number > 7)
        return 0;
    
    size_t inputConfigurationNumber = 0;
    while (inputConfiguration[inputConfigurationNumber].type[q][group][number] != 0)
    {
        inputConfigurationNumber++;
        if (inputConfigurationNumber >= inputConfiguration.size())
        {
            inputConfiguration.resize(inputConfigurationNumber+1);
            for (int localGroup=0; localGroup<257; localGroup++)
            {
                for (int io=0; io<8; io++)
                {
                    for (int loaclQ=0; loaclQ<2; loaclQ++)
                    {
                        inputConfiguration[inputConfigurationNumber].type[loaclQ][localGroup][io] = 0;
                        inputConfiguration[inputConfigurationNumber].itemNumber[loaclQ][localGroup][io] = 0;
                    }
                }
            }
        }
    }
    return inputConfigurationNumber;
}

size_t IoDevice::getOutputConfiguration(int q, int group, int number)
{
    if (number > 7)
        return 0;
    
    size_t outputConfigurationNumber = 0;
    while (outputConfiguration[outputConfigurationNumber].type[q][group][number] != 0)
    {
        outputConfigurationNumber++;
        if (outputConfigurationNumber >= outputConfiguration.size())
        {
            outputConfiguration.resize(outputConfigurationNumber+1);
            for (int localGroup=0; localGroup<257; localGroup++)
            {
                for (int io=0; io<8; io++)
                {
                    for (int loaclQ=0; loaclQ<2; loaclQ++)
                    {
                        outputConfiguration[outputConfigurationNumber].type[loaclQ][localGroup][io] = 0;
                        outputConfiguration[outputConfigurationNumber].itemNumber[loaclQ][localGroup][io] = 0;
                    }
                }
            }
        }
    }
    return outputConfigurationNumber;
}

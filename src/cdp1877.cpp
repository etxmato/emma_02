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
#include "cdp1877.h"

Cdp1877Instance::Cdp1877Instance(int cdp1877Number)
{
    cdp1877Number_ = cdp1877Number;
    
    status_ = 0xff;
    mask_ = 0;
    page_ = 0;
    interval_ = 0;
    
    vectorAddressLow_ = 0;
    vectorByteCounter_ = 0;
    
    for (int i=0; i<8; i++)
        interruptType[i] = 0;
}

void Cdp1877Instance::configureCdp1877(Cdp1877Configuration cdp1877Configuration)
{
    cdp1877Configuration_ = cdp1877Configuration;
    wxString cdp1877NumberString = "";
    if (cdp1877Number_ > 0)
        cdp1877NumberString.Printf(" %d", cdp1877Number_);
    
    p_Main->configureMessage(&cdp1877Configuration.ioGroupVector, "CDP1877 Programmable Interrupt Controller" + cdp1877NumberString);
    
    wxString printBuffer;
    printBuffer.Printf("	Write main interrupt enable address: %04X with bit mask: %02X", cdp1877Configuration.writeMien.portNumber[0], cdp1877Configuration.writeMien.mask);
    p_Main->message(printBuffer);
    printBuffer.Printf("	Write mask register address: %04X with address mask: %04X", cdp1877Configuration.writeMask.portNumber[0], cdp1877Configuration.writeMask.addressMask);
    p_Main->message(printBuffer);
    printBuffer.Printf("	Write control register address: %04X with address mask: %04X", cdp1877Configuration.writeControl.portNumber[0], cdp1877Configuration.writeControl.addressMask);
    p_Main->message(printBuffer);
    printBuffer.Printf("	Write page register address: %04X with address mask: %04X", cdp1877Configuration.writePage.portNumber[0], cdp1877Configuration.writePage.addressMask);
    p_Main->message(printBuffer);
    printBuffer.Printf("	Read interrupt request address: %04X with bit mask: %02X", cdp1877Configuration.readIrq.portNumber[0], cdp1877Configuration.readIrq.mask);
    p_Main->message(printBuffer);
    printBuffer.Printf("	Read status register address: %04X with address mask: %04X", cdp1877Configuration.readStatus.portNumber[0], cdp1877Configuration.readStatus.addressMask);
    p_Main->message(printBuffer);
    printBuffer.Printf("	Read polling register address: %04X with address mask: %04X", cdp1877Configuration.readPolling.portNumber[0], cdp1877Configuration.readPolling.addressMask);
    p_Main->message(printBuffer);
    printBuffer.Printf("	Read vector / long branch address: %04X with address mask: %04X", cdp1877Configuration.readVector.portNumber[0], cdp1877Configuration.readVector.addressMask);
    p_Main->message(printBuffer);

    p_Main->message("");
}

bool Cdp1877Instance::ioGroupCdp1877(int ioGroup)
{
    bool groupFound = false;
    
    if (cdp1877Configuration_.ioGroupVector.size() == 0)
        groupFound = true;
    else
    {
        for (std::vector<int>::iterator ioGroupIterator = cdp1877Configuration_.ioGroupVector.begin (); ioGroupIterator != cdp1877Configuration_.ioGroupVector.end (); ++ioGroupIterator)
        {
            if (*ioGroupIterator == ioGroup)
                groupFound = true;
        }
    }
    return groupFound;
}

void Cdp1877Instance::requestInterrupt(int type, bool state, int picNumber)
{
    status_[picNumber] = state;
    interruptType[picNumber] = type;
}

int Cdp1877Instance::findInterrupt()
{
    for (int picNumber=0; picNumber<8; picNumber++)
    {
        if (status_[picNumber] && !mask_[picNumber])
            return picNumber;
    }
    return -1;
}

void Cdp1877Instance::devWrite(Word address, Byte value)
{
    if ((address &  cdp1877Configuration_.writeMien.addressMask) == cdp1877Configuration_.writeMien.portNumber[0])
    {
        mien_ = ((value & cdp1877Configuration_.writeMien.mask) == cdp1877Configuration_.writeMien.mask);
        if (mien_)
            p_Computer->setMainMemory(address, p_Computer->getMainMemory(address) | cdp1877Configuration_.writeMien.mask);
        else
            p_Computer->setMainMemory(address, p_Computer->getMainMemory(address) &  (cdp1877Configuration_.writeMien.mask ^ 0xff));
    }
    if ((address &  cdp1877Configuration_.writeMask.addressMask) == cdp1877Configuration_.writeMask.portNumber[0])
    {
        writeMask(value);
    }
    if ((address &  cdp1877Configuration_.writePage .addressMask) == cdp1877Configuration_.writePage.portNumber[0])
    {
        writePage(value);
    }
    if ((address &  cdp1877Configuration_.writeControl .addressMask) == cdp1877Configuration_.writeControl.portNumber[0])
    {
        writeControl(value);
    }
}

void Cdp1877Instance::writeMask(Byte value)
{
    mask_ = value;
}

void Cdp1877Instance::writePage(Byte value)
{
    page_ = value;
}

void Cdp1877Instance::writeControl(Byte value)
{
    interval_ = value & 0x3;
    vectorAddressLow_ = value & 0xf0;
    
    if ((value & 0x8) == 0)
        status_ = 0;
    if ((value & 0x4) == 0)
        mask_ = 0;
}

int Cdp1877Instance::devRead(Word address, int function)
{
    if ((address &  cdp1877Configuration_.readIrq.addressMask) == cdp1877Configuration_.readIrq.portNumber[0])
    {
        if (status_ == 0)
            p_Computer->setMainMemory(address, p_Computer->getMainMemory(address) | cdp1877Configuration_.readIrq.mask);
        else
            p_Computer->setMainMemory(address, p_Computer->getMainMemory(address) ^  cdp1877Configuration_.readIrq.mask);
    }
    if ((address &  cdp1877Configuration_.readStatus.addressMask) == cdp1877Configuration_.readStatus.portNumber[0])
    {
        vectorByteCounter_ = 0;
        return readStatus();
    }
    if ((address &  cdp1877Configuration_.readVector .addressMask) == cdp1877Configuration_.readVector.portNumber[0])
    {
        return readVector(address, function);
    }
    if ((address &  cdp1877Configuration_.readPolling .addressMask) == cdp1877Configuration_.readPolling.portNumber[0])
    {
        vectorByteCounter_ = 0;
        return readPolling();
    }
    return -1;
}

Byte Cdp1877Instance::readStatus()
{
    return (Byte)status_.to_ulong();
}

Byte Cdp1877Instance::readPolling()
{
    int polling = findInterrupt();
    if (polling == -1)
        polling = 0;
    else
        status_[polling] = false;
    
    polling = polling << (interval_ + 1);
    
    Byte mask = 0xf0;
    mask = mask << interval_;

    polling |= (vectorAddressLow_ & mask);
    return polling;
}

Byte Cdp1877Instance::readVector(Word address, int function)
{
    if ((function & READ_FUNCTION_NOT_DEBUG) == READ_FUNCTION_NOT_DEBUG)
    {
        switch (vectorByteCounter_ & 0x3)
        {
            case 0:
                if ((function & READ_FUNCTION_FREEZE_PIC_VECTOR) != READ_FUNCTION_FREEZE_PIC_VECTOR)
                    vectorByteCounter_++;
                return 0xC0;
            break;
            case 1:
                if ((function & READ_FUNCTION_FREEZE_PIC_VECTOR) != READ_FUNCTION_FREEZE_PIC_VECTOR)
                    vectorByteCounter_++;
                return page_;
            break;
            case 2:
                if ((function & READ_FUNCTION_FREEZE_PIC_VECTOR) != READ_FUNCTION_FREEZE_PIC_VECTOR)
                    vectorByteCounter_++;
                return readPolling();
            break;
        }
    }
    else
    {
        switch (address & 0x3)
        {
            case 0:
                return 0xC0;
            break;
            case 1:
                return page_;
            break;
            case 2:
                return readPolling();
            break;
        }
    }
    return 0;
}

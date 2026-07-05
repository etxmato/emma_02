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
#include "dip.h"

DipInstance::DipInstance(int dipNumber)
{
    dipNumber_ = dipNumber;
}

void DipInstance::configureDip(DipConfiguration dipConfiguration)
{
    dipConfiguration_ = dipConfiguration;
    wxString dipNumberString = "";
    if (dipNumber_ > 0)
        dipNumberString.Printf(" %d", dipNumber_);
    
    p_Main->configureMessage(&dipConfiguration.ioGroupVector, "DIP switch" + dipNumberString);
    
    wxString printBuffer;

    printBuffer.Printf("hex value: %02X", dipConfiguration.value);
    p_Computer->setInType(&dipConfiguration.ioGroupVector, dipConfiguration.input, printBuffer);
    
    p_Main->message("");
}

bool DipInstance::ioGroupDip(int ioGroup)
{
    bool groupFound = false;
    
    if (dipConfiguration_.ioGroupVector.size() == 0)
        groupFound = true;
    else
    {
        for (std::vector<int>::iterator ioGroupIterator = dipConfiguration_.ioGroupVector.begin (); ioGroupIterator != dipConfiguration_.ioGroupVector.end (); ++ioGroupIterator)
        {
            if (*ioGroupIterator == ioGroup)
                groupFound = true;
        }
    }
    return groupFound;
}

Byte DipInstance::getValue()
{
    return dipConfiguration_.value;
}

bool DipInstance::isAddressValid(Word address)
{
    return (address == dipConfiguration_.input.portNumber[0]);
}

bool DipInstance::isAddressMode()
{
    return dipConfiguration_.input.addressMode;
}

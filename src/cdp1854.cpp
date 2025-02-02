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
    
    status_ = 0xff;
    mask_ = 0;
    page_ = 0;
    interval_ = 0;
    
    vectorAddressLow_ = 0;
    vectorByteCounter_ = 0;
    
    for (int i=0; i<8; i++)
        interruptType[i] = 0;
}

void Cdp1854Instance::configureCdp1854(Cdp1854Configuration cdp1854Configuration)
{
    cdp1854Configuration_ = cdp1854Configuration;
    wxString cdp1854NumberString = "";
    if (cdp1854Number_ > 0)
        cdp1854NumberString.Printf(" %d", cdp1854Number_);
    
    p_Main->configureMessage(&cdp1854Configuration.ioGroupVector, "CDP1854 Programmable Interrupt Controller" + cdp1854NumberString);
    
    p_Main->message("");
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
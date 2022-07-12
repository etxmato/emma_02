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
    for (int i=0; i<5; i++)
    {
        efType_ [i] = 0;
    }
    for (int i=0; i<8; i++)
    {
        inType_ [i] = 0;
        outType_ [i] = 0;
    }
    for (int i=0; i<MAXCYCLE; i++)
    {
        cycleType_ [i] = 0;
    }
}


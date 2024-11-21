/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 ***                                                             ***
 *** 1802 Code based on elf emulator by Michael H Riley with     ***
 *** copyright as below                                          ***
 *******************************************************************
*/

/*
 *******************************************************************
 *** This software is copyright 2006 by Michael H Riley          ***
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

#include "main.h"
#include "portext.h"

PortExt::PortExt() 
{
}

void PortExt::configurePortExt(MemoryMapperConfiguration memoryMapperConfiguration)
{
    p_Main->configureMessage(&memoryMapperConfiguration.ioGroupVector, "Port Extender");
    p_Computer->setOutType(&memoryMapperConfiguration.ioGroupVector, memoryMapperConfiguration.selectOutput, "port selector");
    p_Computer->setOutType(&memoryMapperConfiguration.ioGroupVector, memoryMapperConfiguration.writeOutput, "write selected");
    p_Computer->setInType(&memoryMapperConfiguration.ioGroupVector, memoryMapperConfiguration.input, "read selected");

    p_Main->message("");

    for (int port=0; port<256; port++)
    {
        extPortsOut_[port] = 0;
        extPortsIn_[port] = 0;
    }
    extPort_ = 0;
}

void PortExt::definePortExtForPager() 
{
    for (int port=1; port<16; port++)
    {
        extPortsOut_[port] = PAGEROUT;
    }
}

void PortExt::definePortExtForPager(Byte startPort, Byte endPort)
{
    for (int port=startPort; port<=endPort; port++)
    {
        extPortsOut_[port] = PAGEROUT;
    }
}

Byte PortExt::inPortExtender()
{
    switch(extPortsIn_[extPort_])
    {
        case 0:
            return 255;
        break;

        default:
            return 255;
    }
}

void PortExt::outPortExtender(Byte value)
{
    switch(extPortsOut_[extPort_])
    {
        case 0:
            return;
        break;

        case PAGEROUT:
            p_Computer->setPager(extPort_, value);
        break;
    }
}

void PortExt::selectPortExtender(Byte value) 
{
    extPort_ = value;
    p_Main->updateSlotInfo();
}


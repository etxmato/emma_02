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

void PortExt::configurePortExt(ElfPortConfiguration portConf)
{
//    int selectOutput, writeOutput, input;

    wxString runningComp = p_Main->getRunningComputerStr();

//    selectOutput = p_Main->getConfigItem(runningComp+"PortExtenderSelectOutput", 5l);
//    writeOutput = p_Main->getConfigItem(runningComp+"PortExtenderWriteOutput", 6l);
//    input = p_Main->getConfigItem(runningComp+"PortExtenderInput", 6l);

    p_Computer->setInType(portConf.portExtenderInput, PORTEXTIN);
    p_Computer->setOutType(portConf.portExtenderSelectOutput, PORTEXTSELECTOUT);
    p_Computer->setOutType(portConf.portExtenderWriteOutput, PORTEXTWRITEOUT);
    for (int port=0; port<256; port++) 
    {
        extPortsOut_[port] = 0;
        extPortsIn_[port] = 0;
    }
    extPort_ = 0;

    wxString printBuffer;
    p_Main->message("Configuring Port Extender");

    printBuffer.Printf("    Output %d: port selector, output %d: write selected, input %d: read selected\n", portConf.portExtenderSelectOutput, portConf.portExtenderWriteOutput, portConf.portExtenderInput);
    p_Main->message(printBuffer);
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


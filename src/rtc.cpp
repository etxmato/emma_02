/*
 *******************************************************************
 *** This software is copyright 2023 by Marcel van Tongeren      ***
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
#include "rtc.h"

Rtc::Rtc()
{
}

void Rtc::configureRtc(IoConfiguration ioConf)
{
    wxString ioGroup1 = "", ioGroup2;
    if (ioConf.rtcIoGroup1 != -1)
        ioGroup1.Printf(" on group %d", ioConf.rtcIoGroup1);

    p_Computer->setInType(ioConf.rtcIoGroup1+1, ioConf.rtcIn, RTCIN);
    p_Computer->setOutType(ioConf.rtcIoGroup1+1, ioConf.rtcOut, RTCOUT);
    p_Computer->setOutType(ioConf.rtcIoGroup1+1, ioConf.rtcSelect, RTCSELECT);

    if (ioConf.rtcIoGroup2 != -1)
    {
        ioGroup2.Printf(" and %d", ioConf.rtcIoGroup2);
        p_Computer->setInType(ioConf.rtcIoGroup2+1, ioConf.rtcIn, RTCIN);
        p_Computer->setOutType(ioConf.rtcIoGroup2+1, ioConf.rtcOut, RTCOUT);
        p_Computer->setOutType(ioConf.rtcIoGroup2+1, ioConf.rtcSelect, RTCSELECT);
    }

    wxString printBuffer;
    p_Main->message("Configuring RTC DS12788" + ioGroup1 + ioGroup2);

    printBuffer.Printf("	Output %d: select port, output %d: write selected", ioConf.rtcSelect, ioConf.rtcOut);
    p_Main->message(printBuffer);

    printBuffer.Printf("	Input %d: read port\n", ioConf.rtcIn);
    p_Main->message(printBuffer);
}

Byte Rtc::inRtc()
{
    Byte ret = rtcRam_[registerSelect_];
    
    if (registerSelect_ == 0xc)
    {
        rtcRam_[registerSelect_] = 0;
        p_Main->updateDebugMemory(registerSelect_);
    }
    
    return ret;
}

void Rtc::outRtc(Byte value)
{
    writeRtc(registerSelect_, value);
    if (registerSelect_ == 0xa)
        p_Computer->setDivider(value&0xf);
}

void Rtc::selectRtcRegister(Byte value)
{
    registerSelect_ = value &0x7f;
}

void Rtc::writeRtc(int address, Byte value)
{
    if (address == 0xc || address == 0xd)
        return;
    
    if (address == 0xa)
        rtcRam_[address] = (rtcRam_[address] & 0x80) | (value & 0x7f);
    else
        rtcRam_[address] = value;
    p_Main->updateDebugMemory(address);
}

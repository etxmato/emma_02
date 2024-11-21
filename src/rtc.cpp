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

void Rtc::configureRtc(RtcDs12887Configuration rtcDs12887Configuration)
{
    p_Main->configureMessage(&rtcDs12887Configuration.ioGroupVector, "RTC DS12788");
    
    p_Computer->setOutType(&rtcDs12887Configuration.ioGroupVector, rtcDs12887Configuration.outputSelectPort, "select port");
    p_Computer->setOutType(&rtcDs12887Configuration.ioGroupVector, rtcDs12887Configuration.outputWritePort, "write port");
    p_Computer->setInType(&rtcDs12887Configuration.ioGroupVector, rtcDs12887Configuration.input, "read port");

    p_Main->message("");
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

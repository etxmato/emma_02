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
#include "wx/datetime.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "main.h"
#include "rtc.h"

RtcDs12788::RtcDs12788()
{
}

void RtcDs12788::configureRtcDs12788(RtcDs12887Configuration rtcDs12887Configuration)
{
    p_Main->configureMessage(&rtcDs12887Configuration.ioGroupVector, "RTC DS12788");
    
    p_Computer->setOutType(&rtcDs12887Configuration.ioGroupVector, rtcDs12887Configuration.outputSelectPort, "select port");
    p_Computer->setOutType(&rtcDs12887Configuration.ioGroupVector, rtcDs12887Configuration.outputWritePort, "write port");
    p_Computer->setInType(&rtcDs12887Configuration.ioGroupVector, rtcDs12887Configuration.input, "read port");

    p_Main->message("");
}

Byte RtcDs12788::inRtcDs12788()
{
    Byte ret = rtcRam_[registerSelect_];
    
    if (registerSelect_ == 0xc)
    {
        rtcRam_[registerSelect_] = 0;
        p_Main->updateDebugMemory(registerSelect_);
    }
    
    return ret;
}

void RtcDs12788::outRtcDs12788(Byte value)
{
    writeRtc(registerSelect_, value);
    if (registerSelect_ == 0xa)
        p_Computer->setDivider(value&0xf);
}

void RtcDs12788::selectRtcRegisterDs12788(Byte value)
{
    registerSelect_ = value &0x7f;
}

void RtcDs12788::writeRtc(int address, Byte value)
{
    if (address == 0xc || address == 0xd)
        return;
    
    if (address == 0xa)
        rtcRam_[address] = (rtcRam_[address] & 0x80) | (value & 0x7f);
    else
        rtcRam_[address] = value;
    p_Main->updateDebugMemory(address);
}

int cycleValues[] =
{
    -1, 
    2048,
    1024,
    512,
    256,
    128,
    64,
    32,
    16,
    8,
    4,      // 250 ms
    2,      // 500 ms
    1,      // 1 second
    60,     // 1 minute
    3600,   // 1 hour
    86400   // 1 day
};

RtcCDP1879::RtcCDP1879()
{
}

void RtcCDP1879::configureRtcCdp1879(RtcCdp1879Configuration rtcCdp1879Configuration, double clockSpeed)
{
    rtcCdp1879Configuration_ = rtcCdp1879Configuration;
    clockSpeed_ =  clockSpeed;
    wxString message;
    
    p_Main->configureMessage(&rtcCdp1879Configuration.ioGroupVector, "RTC CDP1879");
    p_Computer->setEfType(&rtcCdp1879Configuration.ioGroupVector, rtcCdp1879Configuration.ef, "IRQ");
    message.Printf("	I/O at address %04X-%04X\n", rtcCdp1879Configuration.base, rtcCdp1879Configuration.control);
    p_Main->message(message);

    p_Computer->setCycleType(CYCLE_TYPE_RTC, RTC_CYCLE);

    status_ = 0;
    control_ = 0;
    
    alarmSecond_ = 0;
    alarmMinute_ = 0;
    alarmHour_ = 0;
    
    timeFrozen_ = false;
    
    cycleValue_ = -1;
    
    cycleSizeAlarm_ = (int) (((clockSpeed_ * 1000000) / 8) / cycleValues [11]);
    cycleValueAlarm_ = cycleSizeAlarm_;
    
    cycleSizeFrozenTime_ = (int) (((clockSpeed_ * 1000000) / 8) / cycleValues [10]);

    efState_ = 1;
}

bool RtcCDP1879::ioGroupCdp1879(int ioGroup)
{
    bool groupFound = false;
    
    if (rtcCdp1879Configuration_.ioGroupVector.size() == 0)
        groupFound = true;
    else
    {
        for (std::vector<int>::iterator ioGroupIterator = rtcCdp1879Configuration_.ioGroupVector.begin (); ioGroupIterator != rtcCdp1879Configuration_.ioGroupVector.end (); ++ioGroupIterator)
        {
            if (*ioGroupIterator == ioGroup)
                groupFound = true;
        }
    }
    return groupFound;
}

Byte RtcCDP1879::efRtcCdp1879()
{
    return efState_^rtcCdp1879Configuration_.ef.reverse;
}

Byte RtcCDP1879::readRtcCdp1879(Word address, Byte addressValue, wxDateTime systemTime, wxDateTime xmlComputerTime)
{
    wxDateTime systemNow;
    wxTimeSpan timeDiff;
    Byte value;
    Byte high, low;

    Byte pm = 0, leap = 0;
    
    if (!timeFrozen_)
    {
        timeFrozen_ = true;
        cycleValueFrozenTime_ = cycleSizeFrozenTime_;

        systemNow = wxDateTime::UNow();
        timeDiff = systemNow.Subtract(systemTime);

        frozenTime_ = xmlComputerTime;
        frozenTime_.Add(timeDiff);
    }
    
    value = 0;
    if (address == rtcCdp1879Configuration_.second)
        value = frozenTime_.GetSecond();
    if (address == rtcCdp1879Configuration_.minute)
        value = frozenTime_.GetMinute();
    if (address == rtcCdp1879Configuration_.hour)
    {
        value = frozenTime_.GetHour();
        if (value > 12)
            pm |= 0x80;
        if ((addressValue & 0x40) == 0x40)
        {
            pm |= 0x40;
            if (value > 12)
                value -= 12;
        }
    }
    if (address == rtcCdp1879Configuration_.date)
        value = frozenTime_.GetDay();
    if (address == rtcCdp1879Configuration_.month)
    {
        value = frozenTime_.GetMonth()+1;
        if (wxDateTime::IsLeapYear())
            leap = 0x80;
    }

    high = (int)(value/10)*16;
    low = value - (int)(value/10)*10;
    value = high + low | pm | leap;
    return value;
}

wxDateTime RtcCDP1879::writeRtcTimeCdp1879(Word address, Byte value, wxDateTime systemTime, wxDateTime xmlComputerTime)
{
    wxDateTime systemNow;
    wxDateTime now;
    wxTimeSpan timeDiff;
    Byte high, low;
    
    if ((control_&0x8) == 0)
    {
    
        if (address == rtcCdp1879Configuration_.second)
            value &= 0x7f;
        if (address == rtcCdp1879Configuration_.minute)
            value &= 0x7f;
        if (address == rtcCdp1879Configuration_.hour)
        {
            if ((value & 0xc0) == 0xc0)
                value += 12;
            value &= 0x3f;
        }

        high = ((value&0xf0)/16)*10;
        low = value&0xf;
        value = high + low;

        systemNow = wxDateTime::UNow();
        timeDiff = systemNow.Subtract(systemTime);

        now = xmlComputerTime;
        now.Add(timeDiff);

        if (address == rtcCdp1879Configuration_.second)
            now.SetSecond(value);
        if (address == rtcCdp1879Configuration_.minute)
            now.SetMinute(value);
        if (address == rtcCdp1879Configuration_.hour)
            now.SetHour(value);

        return now.Subtract(timeDiff);
    }
    else
    {
        high = ((value&0xf0)/16)*10;
        low = value&0xf;
        value = high + low;

        if (address == rtcCdp1879Configuration_.second)
            alarmSecond_ = value;
        if (address == rtcCdp1879Configuration_.minute)
            alarmMinute_ = value;
        if (address == rtcCdp1879Configuration_.hour)
            alarmHour_ = value;
    }
    return xmlComputerTime;
}

wxDateTime RtcCDP1879::writeRtcDateCdp1879(Word address, Byte value, wxDateTime systemTime, wxDateTime xmlComputerTime)
{
    wxDateTime systemNow;
    wxDateTime now;
    wxTimeSpan timeDiff;
    Byte high, low;
    
    if (address == rtcCdp1879Configuration_.date)
        value &= 0x3f;
    if (address == rtcCdp1879Configuration_.month)
        value &= 0x1f;

    high = ((value&0xf0)/16)*10;
    low = value&0xf;
    value = high + low;

    systemNow = wxDateTime::UNow();
    timeDiff = systemNow.Subtract(systemTime);

    now = xmlComputerTime;
    now.Add(timeDiff);

    if (address == rtcCdp1879Configuration_.date)
        now.SetDay(value);
    if (address == rtcCdp1879Configuration_.month)
        now.SetMonth(wxDateTime::Month(value));
    
    return now.Subtract(timeDiff);
}

void RtcCDP1879::writeRtcControlCdp1879(Byte value)
{
    control_ = value;
    status_ = 0;
    efState_ = 1;
    p_Computer->requestInterrupt(INTERRUPT_TYPE_RTC, false, rtcCdp1879Configuration_.picInterrupt);

    int freq = value >> 4;
    if (freq == 0)
        cycleSize_ = -1;
    if (freq >= 1 && freq <= 12)
        cycleSize_ = (int) (((clockSpeed_ * 1000000) / 8) / cycleValues [freq]);
    if (freq >= 13)
        cycleSize_ = (int) (((clockSpeed_ * 1000000) / 8) / (1/cycleValues [freq]));

    if ((value & 4) == 4)
        cycleValue_ = cycleSize_;
    else
        cycleValue_ = -1;
}

Byte RtcCDP1879::readRtcStatusCdp1879()
{
    return status_;
}

void RtcCDP1879::cycleRtcCdp1879(wxDateTime systemTime, wxDateTime xmlComputerTime)
{
    wxDateTime systemNow;
    wxDateTime now;
    wxTimeSpan timeDiff;

    if (cycleValue_ > 0)
    {
        cycleValue_ --;
        if (cycleValue_ <= 0)
        {
            cycleValue_ = cycleSize_;
            status_ = 0x40;
            efState_ = 0;
            if (rtcCdp1879Configuration_.interrupt)
                p_Computer->requestInterrupt(INTERRUPT_TYPE_RTC, true, rtcCdp1879Configuration_.picInterrupt);
        }
    }
    if (cycleValue_ > 0 && timeFrozen_)
    {
        cycleValueFrozenTime_ --;
        if (cycleValueFrozenTime_ <= 0)
            unfreezeTimeCdp1879();
    }
    if (cycleValueAlarm_ > 0 && (control_&0x8) == 0x8)
    {
        cycleValueAlarm_ --;
        if (cycleValueAlarm_ <= 0)
        {
            cycleValueAlarm_ = cycleSizeAlarm_;
            
            systemNow = wxDateTime::UNow();
            timeDiff = systemNow.Subtract(systemTime);
            now = xmlComputerTime;
            now.Add(timeDiff);
            
            if (now.GetHour() == alarmHour_)
            {
                if (now.GetMinute() == alarmMinute_)
                {
                    if (now.GetSecond() == alarmSecond_)
                    {
                        status_ = 0x80;
                        efState_ = 0;
                        if (rtcCdp1879Configuration_.interrupt)
                            p_Computer->requestInterrupt(INTERRUPT_TYPE_RTC, true, rtcCdp1879Configuration_.picInterrupt);
                    }
                }
            }
        }
    }
}

void RtcCDP1879::unfreezeTimeCdp1879()
{
    timeFrozen_ = false;
}

// MM58174AN Real-Time Clock (National Semiconductor)
// 4-bit bus, 16 nibble registers, memory-mapped

RtcMM58174::RtcMM58174()
{
    for (int i = 0; i < 16; i++)
        registers_[i] = 0;
    running_ = false;
}

void RtcMM58174::configureRtcMm58174(RtcMm58174Configuration rtcMm58174Configuration)
{
    rtcMm58174Configuration_ = rtcMm58174Configuration;
    wxString message;

    p_Main->configureMessage(&rtcMm58174Configuration.ioGroupVector, "RTC MM58174AN");
    message.Printf("	I/O at address %04X-%04X\n", rtcMm58174Configuration.base, rtcMm58174Configuration.base + 0xF);
    p_Main->message(message);

    for (int i = 0; i < 16; i++)
        registers_[i] = 0;
    running_ = false;
}

bool RtcMM58174::ioGroupMm58174(int ioGroup)
{
    bool groupFound = false;
    
    if (rtcMm58174Configuration_.ioGroupVector.size() == 0)
        groupFound = true;
    else
    {
        for (std::vector<int>::iterator ioGroupIterator = rtcMm58174Configuration_.ioGroupVector.begin (); ioGroupIterator != rtcMm58174Configuration_.ioGroupVector.end (); ++ioGroupIterator)
        {
            if (*ioGroupIterator == ioGroup)
                groupFound = true;
        }
    }
    return groupFound;
}

Byte RtcMM58174::readRtcMm58174(Word address, wxDateTime systemTime, wxDateTime xmlComputerTime)
{
    int reg = (address - rtcMm58174Configuration_.base) & 0xF;

    // Registers 0 (test) and D (leap year) are write-only, return 0
    if (reg == 0x0 || reg == 0xD)
        return 0;

    // Register E (stop/start) is write-only
    if (reg == 0xE)
        return 0;

    // Register F (interrupt control) - return stored value
    if (reg == 0xF)
        return registers_[0xF] & 0x0F;

    // Time registers: compute current time
    wxDateTime systemNow = wxDateTime::UNow();
    wxTimeSpan timeDiff = systemNow.Subtract(systemTime);
    wxDateTime now = xmlComputerTime;
    now.Add(timeDiff);

    Byte value = 0;
    switch (reg)
    {
        case 0x1:   // tenths of seconds
            value = (now.GetMillisecond() / 100) & 0x0F;
        break;

        case 0x2:   // seconds units
            value = now.GetSecond() % 10;
        break;

        case 0x3:   // seconds tens
            value = now.GetSecond() / 10;
        break;

        case 0x4:   // minutes units
            value = now.GetMinute() % 10;
        break;

        case 0x5:   // minutes tens
            value = now.GetMinute() / 10;
        break;

        case 0x6:   // hours units
            value = now.GetHour() % 10;
        break;

        case 0x7:   // hours tens
            value = now.GetHour() / 10;
        break;

        case 0x8:   // days units
            value = now.GetDay() % 10;
        break;

        case 0x9:   // days tens
            value = now.GetDay() / 10;
        break;

        case 0xA:   // day of week (1-7)
            value = now.GetWeekDay();
            if (value == 0) value = 7;  // Sunday: wxDateTime=0, MM58174=7
        break;

        case 0xB:   // months units
            value = (now.GetMonth() + 1) % 10;
        break;

        case 0xC:   // months tens
            value = (now.GetMonth() + 1) / 10;
        break;
    }

    return value & 0x0F;
}

wxDateTime RtcMM58174::writeRtcMm58174(Word address, Byte value, wxDateTime systemTime, wxDateTime xmlComputerTime)
{
    int reg = (address - rtcMm58174Configuration_.base) & 0xF;
    value &= 0x0F;     // Only low nibble is significant

    registers_[reg] = value;

    // Register E: stop/start control
    if (reg == 0xE)
    {
        running_ = (value & 0x01) != 0;
        return xmlComputerTime;
    }

    // Register F: interrupt control (store but no action needed for emulation)
    if (reg == 0xF)
        return xmlComputerTime;

    // Register 0: test mode (ignore)
    if (reg == 0x0)
        return xmlComputerTime;

    // Register D: leap year counter (store but no action needed — wxDateTime handles leap years)
    if (reg == 0xD)
        return xmlComputerTime;

    // Time registers: only update the computer time if clock is stopped (standard RTC set procedure)
    if (!running_)
    {
        wxDateTime systemNow = wxDateTime::UNow();
        wxTimeSpan timeDiff = systemNow.Subtract(systemTime);
        wxDateTime now = xmlComputerTime;
        now.Add(timeDiff);

        int second = registers_[0x2] + registers_[0x3] * 10;
        int minute = registers_[0x4] + registers_[0x5] * 10;
        int hour = registers_[0x6] + registers_[0x7] * 10;
        int day = registers_[0x8] + registers_[0x9] * 10;
        int month = registers_[0xB] + registers_[0xC] * 10;

        if (second >= 0 && second <= 59)
            now.SetSecond(second);
        if (minute >= 0 && minute <= 59)
            now.SetMinute(minute);
        if (hour >= 0 && hour <= 23)
            now.SetHour(hour);
        if (day >= 1 && day <= 31)
            now.SetDay(day);
        if (month >= 1 && month <= 12)
            now.SetMonth(wxDateTime::Month(month - 1));

        return now.Subtract(timeDiff);
    }

    return xmlComputerTime;
}

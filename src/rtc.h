#ifndef RTC_H
#define RTC_H

#include "computerconfig.h"

class RtcDs12788
{
public:
    RtcDs12788();
    ~RtcDs12788() {};

    void configureRtcDs12788(RtcDs12887Configuration rtcDs12887Configuration);
    Byte inRtcDs12788();
    void outRtcDs12788(Byte value);
    void selectRtcRegisterDs12788(Byte value);

protected:
    Byte rtcRam_[128];

private:
    void writeRtc(int address, Byte value);

    int registerSelect_;
};

class RtcCDP1879
{
public:
    RtcCDP1879();
    ~RtcCDP1879() {};

    void configureRtcCdp1879(RtcCdp1879Configuration rtcCdp1879Configuration, double clockSpeed);
    bool ioGroupCdp1879(int ioGroup);
    Byte efRtcCdp1879();
    
    Byte readRtcCdp1879(Word address, Byte addressValue, wxDateTime systemTime, wxDateTime xmlComputerTime);
    wxDateTime writeRtcTimeCdp1879(Word address, Byte value, wxDateTime systemTime, wxDateTime xmlComputerTime);
    wxDateTime writeRtcDateCdp1879(Word address, Byte value, wxDateTime systemTime, wxDateTime xmlComputerTime);

    void writeRtcControlCdp1879(Byte value);
    Byte readRtcStatusCdp1879();

    void cycleRtcCdp1879(wxDateTime systemTime, wxDateTime xmlComputerTime);
    void unfreezeTimeCdp1879();

private:
    RtcCdp1879Configuration rtcCdp1879Configuration_;
    
    Byte efState_;
    Byte status_;
    Byte control_;

    Byte alarmSecond_;
    Byte alarmMinute_;
    Byte alarmHour_;

    int cycleValue_;
    int cycleSize_;

    int cycleValueAlarm_;
    int cycleSizeAlarm_;
    
    int cycleValueFrozenTime_;
    int cycleSizeFrozenTime_;

    bool timeFrozen_;
    wxDateTime frozenTime_;
    
    double clockSpeed_;
};

#endif  // RTC_H

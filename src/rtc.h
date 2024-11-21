#ifndef RTC_H
#define RTC_H

#include "computerconfig.h"

class Rtc
{
public:
    Rtc();
    ~Rtc() {};

    void configureRtc(RtcDs12887Configuration rtcDs12887Configuration);
    Byte inRtc();
    void outRtc(Byte value);
    void selectRtcRegister(Byte value);

protected:
    Byte rtcRam_[128];

private:
    void writeRtc(int address, Byte value);

    int registerSelect_;
};

#endif  // RTC_H

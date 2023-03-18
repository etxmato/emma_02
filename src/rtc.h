#ifndef RTC_H
#define RTC_H

#include "elfconfiguration.h"

class Rtc
{
public:
    Rtc();
    ~Rtc() {};

    void configureRtc(IoConfiguration ioConf);
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

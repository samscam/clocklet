#pragma once

#include <RTClib.h>
#include "ESP32Rtc.h"

#include "../UpdateScheduler.h"

class TimeSync: public UpdateJob {
public:
    TimeSync(RTC_DS3231 *ds3231, RTC_ESP32 *esp32);
    ~TimeSync(){};

    bool performUpdate();
private:
    void esptods3231();
    void ds3231toesp();
    void variationCheck();

    RTC_DS3231 *_ds3231;
    RTC_ESP32 *_esp32;

    int syncCount = 0;
};

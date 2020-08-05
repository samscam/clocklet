#pragma once

#include <RTClib.h>

class RTC_ESP32 {
public:
    boolean begin(void);

    // newTimeZone needs to be a char* of a posix tz definition like
    void setTimeZone(const char* newTimeZone);
    void adjust(const DateTime& dt);
    DateTime now();
    DateTime localTime();
};

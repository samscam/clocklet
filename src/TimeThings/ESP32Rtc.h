#ifndef PRISCILLA_ESP32RTC
#define PRISCILLA_ESP32RTC

#include <RTClib.h>

class RTC_ESP32 {
public:
    boolean begin(void);
    void adjust(const DateTime& dt);
    DateTime now();
    DateTime localTime();
};

#endif
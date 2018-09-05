#ifndef PRISCILLA_DISPLAY
#define PRISCILLA_DISPLAY

#include <Arduino.h>
#include <RTClib.h>
#include "../weather/weather.h"

class Display {
public:
  Display();

  void setWeather(Weather weather);
  void displayTime(const DateTime& time);
  void scrollText(const char *stringy);

};

#endif

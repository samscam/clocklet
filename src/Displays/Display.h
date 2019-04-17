#ifndef PRISCILLA_DISPLAY
#define PRISCILLA_DISPLAY

#include <Arduino.h>
#include <RTClib.h>
#include "../weather/weather.h"

// General purpose protocol for clock displays
class Display {
public:

  // return true if it worked - otherwise false
  virtual boolean setup();

  // notifies the display to do another frame if it wants to do that kind of thing
  virtual void frameLoop();

  // It's a clock of some sort... you have to implement this
  // Time is passed by reference - the display should update on the next frame loop
  virtual void setTime(DateTime time);

  // Implementation is optional
  virtual void setWeather(Weather weather);

  // Show a message - but what kind of message?
  virtual void displayMessage(const char *stringy);

  // Brightness is a float from 0 (barely visible) to 1 (really bright)
  virtual void setBrightness(float brightness);

};

#endif

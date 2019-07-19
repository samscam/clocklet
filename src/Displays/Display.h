#ifndef PRISCILLA_DISPLAY
#define PRISCILLA_DISPLAY

#include <Arduino.h>
#include <RTClib.h>
#include "../weather/weather.h"

enum MessageType {
  good, bad, rando
};

// General purpose protocol for clock displays
class Display {
public:

  // return true if it worked - otherwise false
  virtual boolean setup();

  // notifies the display to do another frame if it wants to do that kind of thing
  virtual void frameLoop();

  // It's a clock of some sort... we have to implement this
  // Time is passed in here - the display should update on the next frame loop
  virtual void setTime(DateTime time);

  // Show a message
  // - but what kind of message?
  virtual void displayMessage(const char *stringy, MessageType messageType = good);

  // OPTIONAL:

  // Set a secondary time for like india
  virtual void setSecondaryTime(DateTime time, const char *identifier) {}

  // Set the weather, make it sunny please
  virtual void setWeather(Weather weather) {}
  
  virtual void displayTemperatures() {}

  // Show a status message (which should be sticky on epd)
  virtual void setStatusMessage(const char *stringy) {}

  // Brightness is a float based on ambient light levels
  // from 0 (dark) to 1 (bright)
  virtual void setBrightness(float brightness) {}

  // Battery level is a float between 0 and 1
  virtual void setBatteryLevel(float level) {}

  // Shows something if we are connected to mains
  virtual void setOnMainsPower(bool mains) {}

};

#endif

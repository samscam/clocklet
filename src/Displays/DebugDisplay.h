#ifndef PRISCILLA_DEBUGDISPLAY
#define PRISCILLA_DEBUGDISPLAY

#include "Display.h"

class DebugDisplay : public Display {
public:
  DebugDisplay();
  // return true if it worked - otherwise false
  boolean setup();

  // notifies the display to do another frame if it wants to do that kind of thing
  void frameLoop();

  // It's a clock of some sort... you have to implement this
  // Time is passed by reference - the display should update on the next frame loop
  void setTime(DateTime time);

  // Implementation is optional
  void setWeather(Weather weather);

  // Show a message - but what kind of message?
  void displayMessage(const char *stringy, MessageType messageType = good);

  // Brightness is a float from 0 (barely visible) to 1 (really bright)
  void setBrightness(float brightness);

  void setBatteryVoltage(float newVoltage);

};
#endif

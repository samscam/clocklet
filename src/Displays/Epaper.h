#ifndef PRISCILLA_EPAPERDISPLAY
#define PRISCILLA_EPAPERDISPLAY

#include "Display.h"


#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>


class EpaperDisplay : public Display {
public:
  EpaperDisplay();
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
  void displayMessage(const char *stringy);

  // Brightness is a float from 0 (barely visible) to 1 (really bright)
  void setBrightness(float brightness);

  // For fun
  void setBatteryVoltage(float voltage);

private:
  void updateDisplay();
  void scrollString(const char *string);
  void pageString(const char *string);
  void clear();

  GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display;

  char time_string[6];
  const char* weather_string = "";
  float voltage = 0;
  bool needsDisplay = false;
};
#endif

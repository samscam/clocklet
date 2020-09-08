#ifndef PRISCILLA_EPAPERDISPLAY
#define PRISCILLA_EPAPERDISPLAY

#include "Display.h"


#define ENABLE_GxEPD2_GFX 0
#define DISABLE_DIAGNOSTIC_OUTPUT

#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>


class EpaperDisplay : public Display {
public:
  EpaperDisplay();
  // return true if it worked - otherwise false
  boolean setup();

  void frameLoop();
  
  void setTime(DateTime time);

  void setSecondaryTime(DateTime time, const char *identifier);

  void setWeather(Weather weather);

  void displayMessage(const char *stringy, MessageType messageType = good);

  void setBatteryLevel(float level);

  bool showSecondaryTime = true;

private:
  void updateDisplay();
  void scrollString(const char *string);
  void pageString(const char *string);
  void displayOctogram();
  void displayDigital();
  void clear();
  void fillArcBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, float arc);
  GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display;

  char time_string[6];
  char secondary_time_string[6];
  const char *secondary_identifier = "";

  DateTime time;

  const char *weather_string = "";
  float batteryLevel = 0;
  bool needsDisplay = false;

  const char *statusMessage = "";
};
#endif

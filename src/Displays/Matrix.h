#ifndef PRISCILLA_MATRIX
#define PRISCILLA_MATRIX

#include <Arduino.h>
#include <FastLED.h>
#include <RTClib.h>
#include "weather/weather.h"
#include "Display.h"

// ----------- Display


#include "pgmspace.h"

#define DATA_PIN    18
#define CLOCK_PIN   19
#define LED_TYPE    SK9822
#define COLOR_ORDER BGR

#define COLUMNS  17
#define ROWS 5

#define NUM_LEDS    COLUMNS * ROWS

#define MAX_BRIGHTNESS 140
#define MIN_BRIGHTNESS 10

enum DeviceState {
  ok,
  weatherFail,
  syncFail,
  noLocation,
  noNetwork,
  bluetooth
};

class Matrix: public Display {
public:
  Matrix();
  // return true if it worked - otherwise false
  boolean setup();

  // notifies the display to do another frame if it wants to do that kind of thing
  void frameLoop();

  // It's a clock of some sort... you have to implement this
  // the display should update on the next frame loop
  void setTime(DateTime time);

  // Set the weather
  void setWeather(Weather weather);
  
  void displayTemperatures();

  // Show a message - but what kind of message?
  void displayMessage(const char *stringy, MessageType messageType);
  
  void setStatusMessage(const char * string);
  
  // Brightness is a float from 0 (barely visible) to 1 (really bright)
  void setBrightness(float brightness);

  void setRainbows(bool rainbows);

  CRGB colourFromTemperature(float temperature);
  void setDigits(int number, CRGB colour = CRGB::White);

  void setDeviceState(DeviceState state);

  void graphicsTest();

private:

  void displayTime(const DateTime& time, Weather weather);
  void maskTime(const DateTime& time);

  void updateBrightness();

  void scrollText(const char *stringy);
  void scrollText_fail(const char *stringy);
  void scrollText(const char *stringy, CRGB colour);
  void scrollText(const char *stringy, CRGB startColour, CRGB endColour, bool rainbow = false);
  void scrollText_randomColour(const char *stringy);
  void scrollText_rainbow(const char *stringy);

  int drawChar(bool imageBuffer[255][5], char character, int xpos, int ypos, const byte* font);

  void setDigit(int number, int digit);
  void setDigit(char character, int digit);

  void displayString(const char *string);

  void setDigits(float number, CRGB colour = CRGB::White);

  void setDigitMask(uint16_t mask, int digit);

  void setDot(bool state, CRGB colour = CRGB::Black);
  
  void advanceWindCycle(float speed);

  void fillDigits_rainbow();
  void fillDigits_heat(); //temps in •c
  void fillDigits_gradient(CRGB startColour, CRGB endColour, uint16_t startPos = 0, double direction = 0);

  void regenerateHeatPalette(float minTemp, float maxTemp);

  void initRain();
  void addRain( fract8 chanceOfRain, CRGB colour);
  void addSnow( fract8 chanceOfSnow );
  void addLightening();
  void addFrost();

  const bool    kMatrixSerpentineLayout = true;
  uint16_t XY( uint8_t x, uint8_t y);
  uint16_t XYsafe( uint8_t x, uint8_t y);

  bool _blinkColon = false;
  uint8_t _brightness;

  CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
  CRGB* leds;

  DateTime _time;
  Weather _weather;
  DeviceState _deviceState = ok;

  bool rainbows = false;

  uint8_t rainFrame = 0;
  bool rainDrops[ROWS][COLUMNS] = {{0}} ;
  CRGB rainLayer[NUM_LEDS];
  CRGB lighteningLayer[NUM_LEDS];

  CRGBPalette16 scaledHeatPalette;
  CRGBPalette256 temperaturePalette;
  float cycle = 0; // this is a float because 255 is not high enough resolution

};

#endif
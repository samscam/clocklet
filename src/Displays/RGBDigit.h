#ifndef PRISCILLA_RGBDIGIT
#define PRISCILLA_RGBDIGIT

#include <Arduino.h>
#include <FastLED.h>
#include <RTClib.h>
#include "weather/weather.h"
#include "Display.h"

// ----------- Display

#if defined(ESP32)
#define DATA_PIN    18
#include "pgmspace.h"
#else
#define DATA_PIN    11
#endif

#define CLOCK_PIN   19
#define LED_TYPE    SK9822
#define COLOR_ORDER GRB

#define NUM_DIGITS  4
#define DIGIT_SEGS  8
#define NUM_LEDS    DIGIT_SEGS * NUM_DIGITS


#define DIGIT_COUNT 4

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

class RGBDigit: public Display {
public:
  RGBDigit();
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

private:

  void displayTime(const DateTime& time, Weather weather);
  void maskTime(const DateTime& time);

  void updateBrightness();

  void scrollText(const char *stringy);
  void scrollText_fail(const char *stringy);
  void scrollText(const char *stringy, CRGB colour);
  void scrollText(const char *stringy, CRGB startColour, CRGB endColour);
  void scrollText_randomColour(const char *stringy);

  void setDigit(int number, int digit);
  void setDigit(char character, int digit);

  void setDigits(const char *string);

  void setDigits(float number, CRGB colour = CRGB::White);


  void setDigitMask(byte mask, int digit);

  void setDot(bool state, int digit, CRGB colour = CRGB::White);
  
  void advanceWindCycle(float speed);

  void fillDigits_rainbow(bool includePoints);
  void fillDigits_heat(); //temps in •c
  void fillDigits_gradient(CRGB startColour, CRGB endColour);


  void regenerateHeatPalette(float minTemp, float maxTemp);

  void initRain();
  void addRain( fract8 chanceOfRain, CRGB colour);
  void addSnow( fract8 chanceOfSnow );
  void addLightening();
  void addFrost();

  bool _blinkColon = false;
  uint8_t _brightness;
  CRGB leds[NUM_LEDS];
  DateTime _time;
  Weather _weather;
  DeviceState _deviceState = ok;

  bool rainbows = false;

  CRGB rainLayer[NUM_LEDS];
  int vsegs[4] = {1,2,4,5};
  int allvsegs[ 4 * NUM_DIGITS ] = {0};

  CRGB lighteningLayer[NUM_LEDS];
  CRGBPalette16 scaledHeatPalette;
  CRGBPalette256 temperaturePalette;
  float cycle = 0;

  //   0
  // 5   1
  //   6
  // 4   2
  //   3   7

  // ASCII offset 32
  const byte _symbolsLow[16] = {
    B00000000, // space - 32
    B01100001, // ! - 33
    B01000100, // " - 34
    B01101110, // # - 35 erm
    B10110110, // $ - 36 like S
    B10010010, // % - 37 three lines
    B10111110, // & - 38
    B00000100, // ' - 39
    B10011100, // ( - 40
    B11110000, // ) - 41
    B11000110, // * - 42
    B01100010, // + - 43
    B00001000, // , - 44
    B00000010, // - - 45
    B00000001, // . - 46
    B01001010, // / - 47
  };

  // ASCII offsets 65 and 97
  const byte _charMasks[36] = {
    B11101110, // a
    B00111110, // b
    B00011010, // c
    B01111010, // d
    B10011110, // e
    B10001110, // f
    B11110110, // g
    B00101110, // h
    B01100000, // i
    B01110000, // j
    B01101111, // k
    B00011100, // l
    B00101011, // m crap
    B00101010, // n
    B00111010, // o
    B11001110, // p
    B11100110, // q
    B00001010, // r
    B10110110, // s
    B00011110, // t
    B00111000, // u
    B00110000, // v crap
    B00111001, // w crap
    B00110011, // x crap
    B01110110, // y
    B11011010, // z
  };



  const byte _numberMasks[10] = {
    B11111100,//0
    B01100000,//1
    B11011010,//2
    B11110010,//3
    B01100110,//4
    B10110110,//5
    B10111110,//6
    B11100000,//7
    B11111110,//8
    B11110110,//9
  };
};

#endif
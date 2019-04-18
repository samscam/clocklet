#ifndef PRISCILLA_RGBDIGIT
#define PRISCILLA_RGBDIGIT

#include <Arduino.h>
#include <FastLED.h>
#include <RTClib.h>
#include "weather/weather.h"
#include "Display.h"

// ----------- Display

#if defined(ESP32)
#define DATA_PIN    22
#include "pgmspace.h"
#else
#define DATA_PIN    11
#endif

//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

#define NUM_DIGITS  4
#define DIGIT_SEGS  8
#define NUM_LEDS    DIGIT_SEGS * NUM_DIGITS


#define DIGIT_COUNT 4



DEFINE_GRADIENT_PALETTE( heatmap_pal ) {
0,     0,  255,  255,
51,   255,  255, 255,
102,   0,255,  0,
153, 255,255,0,
204,   255,0,0,
255,  255,0,255
};
CRGBPalette16  tempPalette = heatmap_pal;

class RGBDigit: public Display {
public:
  RGBDigit();
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
  void setDigits(int number);

  void setDigitMask(byte mask, int digit);

  void setDot(bool state, int digit);
  void fillDigits_rainbow(bool includePoints, float speed);
  CRGB colourFromTemperature(float temperature);
  void fillDigits_heat( float speed, float minTemp, float maxTemp); //temps in •c
  void fillDigits_gradient(CRGB startColour, CRGB endColour);

  void addRain( fract8 chanceOfRain, CRGB colour);
  void initRain();
  void addSnow( fract8 chanceOfSnow );
  void addLightening();
  void addFrost();

  bool _blinkColon = false;
  uint8_t _brightness;
  CRGB leds[NUM_LEDS];
  DateTime _time;
  Weather _weather;

  CRGB rainLayer[NUM_LEDS];
  int vsegs[4] = {1,2,4,5};
  int allvsegs[ 4 * NUM_DIGITS ] = {0};

  CRGB lighteningLayer[NUM_LEDS];

  // CRGBPalette16 tempPalette = CRGBPalette16(0x00FFFF, 0xFFFFFF, 0x00FF00, 0xFFFF00, 0xFF0000, 0xFF00FF}

  // Convert the temps in °c to uint8_t
  // -10 = 0
  // 0 = 51
  // 10 = 102
  // 20 = 153
  // 30 = 204
  // 40 = 255




  float cycle = 0;

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

  //   0
  // 5   1
  //   6
  // 4   2
  //   3   7

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

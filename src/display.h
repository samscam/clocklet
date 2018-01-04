#ifndef PRISCILLA_DISPLAY
#define PRISCILLA_DISPLAY

#include <Arduino.h>
#include <FastLED.h>
#include <RTClib.h>
#include "weather/weather.h"

// ----------- Display

#define DATA_PIN    11
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

#define NUM_DIGITS  4
#define DIGIT_SEGS  8
#define NUM_LEDS    DIGIT_SEGS * NUM_DIGITS


#define DIGIT_COUNT 4


// Function declarations
void initDisplay();
void displayTime(const DateTime& time, weather weather);
void maskTime(const DateTime& time);

void updateBrightness();

void randoMessage();
void scrollText(const char *stringy);
void scrollText_fail(const char *stringy);
void scrollText(const char *stringy, CRGB colour);
void setDigit(int number, int digit);
void setDigit(char character, int digit);
void setDigits(const char *string);

void setDigitMask(byte mask, int digit);

void setDot(bool state, int digit);
void fillDigits_rainbow(bool includePoints);

void addRain( fract8 chanceOfRain, CRGB colour);
void initRain();
void addSnow( fract8 chanceOfSnow );
void addLightening();
void addFrost();

#endif

#ifndef PRISCILLA_DISPLAY
#define PRISCILLA_DISPLAY

#include <Arduino.h>
#include <FastLED.h>
#include <RTClib.h>

#include "colours.h"

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
void displayTime(const DateTime& time, Colour colours[5]);

void updateBrightness();

void randoMessage();
void scrollText(const char *stringy);
void scrollText_fail(const char *stringy);
void scrollText(const char *stringy, Colour colour);
void setDigit(int number, int digit);

#endif

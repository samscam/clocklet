#ifndef PRISCILLA_DISPLAY
#define PRISCILLA_DISPLAY

#include <Arduino.h>
#include <RGBDigit.h>
#include <RTClib.h>

#include "colours.h"

// ----------- Display


#define DIGIT_COUNT 4


// Function declarations
void initDisplay();
void displayTime(const DateTime& time, Colour colours[5]);

void updateBrightness(int hours);

void randoMessage();
void scrollText(const char *stringy);
void scrollText_fail(const char *stringy);
void scrollText(const char *stringy, Colour colour);

#endif

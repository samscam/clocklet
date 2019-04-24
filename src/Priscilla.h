#ifndef PRISCILLA
#define PRISCILLA

#include <Arduino.h>

#include <SPI.h>

#include <Wire.h>
#include <RTClib.h>

// Weather type definitions
#include "weather/weather.h"


//#include "weather/darksky.h"
#include "weather/met-office.h"
#include "network.h"



#include "colours.h"

#if defined(ESP32)
#define BATTERY_MONITORING
#define BATTERY_PIN A13
#endif

// Function declarations

void updatesDaily();
void updatesHourly();
uint16_t dstAdjust(DateTime time);
void generateDSTTimes(uint16_t year);

void showTime();

void updateBrightness();

#if defined(BATTERY_MONITORING)
float batteryVoltage();
#endif
void espSleep(int seconds);

#endif

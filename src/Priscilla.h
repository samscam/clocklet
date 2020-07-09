#pragma once

#include <Arduino.h>

#include <SPI.h>

#include <Wire.h>
#include <RTClib.h>

// Weather type definitions
#include "weather/weather.h"

#include "network.h"

#if defined(ESP32)
// #define BATTERY_MONITORING
#define BATTERY_PIN A13
#endif

#if defined(CLOCKBRAIN)
#define TOUCH_PIN T7
#define LIGHT_PIN 36
#else
#define TOUCH_PIN T6
#define LIGHT_PIN A2
#endif



// Function declarations

float currentBrightness();
long detectTouchPeriod();

void sensibleDelay(int milliseconds);

#if defined(BATTERY_MONITORING)
float batteryVoltage();
float batteryLevel(float voltage);

static float cutoffVoltage = 3.3f;
static float maxVoltage = 4.2f;

#if defined(ESP32)
void espSleep(int seconds);
void espShutdown();
#endif

#endif


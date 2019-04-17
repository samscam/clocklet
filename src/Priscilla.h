#ifndef PRISCILLA
#define PRISCILLA

#include <Arduino.h>

#include <SPI.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <RTClib.h>


#include "weather/weather.h"
//#include "weather/darksky.h"
#include "weather/met-office.h"
#include "network.h"

#include "Displays/RGBDigit.h"

#include "colours.h"






// Function declarations

void updatesDaily();
void updatesHourly();
uint16_t dstAdjust(DateTime time);
void generateDSTTimes(uint16_t year);

void showTime();
void updateRTCTimeFromNTP();


void sendNTPpacket(IPAddress& address);
void updateBrightness();



#endif

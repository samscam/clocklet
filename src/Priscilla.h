#ifndef PRISCILLA
#define PRISCILLA

#include <Arduino.h>

#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <RTClib.h>


#include "weather.h"
#include "network.h"
#include "display.h"
#include "colours.h"


// Function declarations

void performUpdates(bool forceAll);

void showTime();
void updateRTCTimeFromNTP();

void sendNTPpacket(IPAddress& address);




#endif

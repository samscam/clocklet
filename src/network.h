#ifndef PRISCILLA_NETWORK
#define PRISCILLA_NETWORK

#include <Arduino.h>
#include <WiFi101.h>


// ----------- WIFI

const char ssid[] = "***REMOVED***";  //  your network SSID (name)
const char pass[] = "***REMOVED***";       // your network password

bool setupWifi();
bool connectWifi();
bool connectWifi();
void printWiFiStatus();


#endif

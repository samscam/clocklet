#ifndef PRISCILLA_NETWORK
#define PRISCILLA_NETWORK

#include <Arduino.h>

#if defined(ESP32)
#include <WiFi.h>
#include <WiFiClientSecure.h>
#else
#include <WiFi101.h>
#endif

// ----------- WIFI


bool setupWifi();
bool connectWifi();
void printWiFiStatus();


#endif

#ifndef PRISCILLA_NETWORK
#define PRISCILLA_NETWORK

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>


// ----------- WIFI

bool setupWifi();
bool connectWifi();
void printWiFiStatus();
void printWiFiConnectionStatus();
bool reconnect();
void stopWifi();
bool waitForWifi(uint32_t milliseconds);

#endif

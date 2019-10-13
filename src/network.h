#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>


// ----------- Network helpers

void printWiFiStatus();

bool reconnect();
void stopWifi();
bool waitForWifi(uint32_t milliseconds);

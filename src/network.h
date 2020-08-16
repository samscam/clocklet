#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>


// ----------- Network helpers

void printWiFiStatus();

bool reconnect();
void stopWifi();
bool waitForWifi(uint32_t milliseconds);
const char* wl_status_toString(wl_status_t status);
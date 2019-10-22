#pragma once

#include <Arduino.h>
#include <WiFi.h>

#define CONFIG_SOFTAP_SSID "Clocklet"
#define CONFIG_SOFTAP_PASS "boogaloo"

// #ifdef __cplusplus
// extern "C" {
// #endif

bool isAlreadyProvisioned();
bool provisioningActive();
void startProvisioning();
void wifi_init_sta();

void doBackgroundThings(void * parameter);

// #ifdef __cplusplus
// }
// #endif
#pragma once

#include <Arduino.h>
#include <WiFi.h>

#define CONFIG_SOFTAP_SSID "O'Clocky"
#define CONFIG_SOFTAP_PASS "boogaloo"

// #ifdef __cplusplus
// extern "C" {
// #endif

bool isAlreadyProvisioned();
bool provisioningActive();
void startProvisioning();
void wifi_init_sta();

// #ifdef __cplusplus
// }
// #endif
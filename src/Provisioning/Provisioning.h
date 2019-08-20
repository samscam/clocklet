#pragma once

#include <Arduino.h>

#define CONFIG_SOFTAP_SSID "O'Clocky"
#define CONFIG_SOFTAP_PASS "boogaloo"

#ifdef __cplusplus
extern "C" {
#endif

bool isAlreadyProvisioned();
void startProvisioning();

#ifdef __cplusplus
}
#endif
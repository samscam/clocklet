#pragma once

#include <Arduino.h>
#include <WiFi.h>

bool isAlreadyProvisioned();
bool isProvisioningActive();
void startProvisioning();
void wifi_init_sta();

void doBackgroundThings(void * parameter);

#pragma once

#include <Arduino.h>
#include <WiFi.h>

bool isAlreadyProvisioned();
bool isProvisioningActive();
void startProvisioning(QueueHandle_t prefsChangedQueue);
void wifi_init_sta();

void doBackgroundThings(void * parameter);

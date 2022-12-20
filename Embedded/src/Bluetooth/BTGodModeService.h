#pragma once

#include <NimBLEDevice.h>
#include "weather/weather.h"

struct GodModeSettings {
    bool enabled;
    Weather weather;
};

class BTGodModeService: public NimBLECharacteristicCallbacks {

public:
    BTGodModeService(NimBLEServer *server, QueueHandle_t godModeQueue);
    void onWrite(NimBLECharacteristic* pCharacteristic);
    void onRead(NimBLECharacteristic* pCharacteristic);
    
private:
    
    BLEService *_sv_godmode;
    BLECharacteristic *_ch_enabled;
    BLECharacteristic *_ch_weather;

    QueueHandle_t _godModeQueue;

    GodModeSettings _godModeSettings = {};
};
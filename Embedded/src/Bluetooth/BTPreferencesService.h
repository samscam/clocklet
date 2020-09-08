#pragma once

#include <NimBLEDevice.h>

#include "BTPreferencesGlue.h"


class BTPreferencesService {
public:
    BTPreferencesService(NimBLEServer *server, QueueHandle_t prefsChangedQueue);

private:
    NimBLEService *pservice;

    NimBLECharacteristic *availableSeparatorAnimationsCharacteristic;
    PreferencesGlue<std::string> *separatorAnimationsGlue;

    NimBLECharacteristic *availableTimeStyles;
    PreferencesGlue<std::string> *timeStyleGlue;

    PreferencesGlue<float_t> *brightnessGlue;
    PreferencesGlue<bool> *autoBrightnessGlue;
};




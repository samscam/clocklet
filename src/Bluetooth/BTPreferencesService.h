#pragma once

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEService.h>

#include "BTPreferencesGlue.h"


class BTPreferencesService {
public:
    BLEService *pservice;


    BTPreferencesService(BLEServer *server, QueueHandle_t prefsChangedQueue);

    BLECharacteristic *availableSeparatorAnimationsCharacteristic;
    PreferencesGlue<std::string> *separatorAnimationsGlue;

    BLECharacteristic *availableTimeStyles;
    PreferencesGlue<std::string> *timeStyleGlue;

};




#pragma once

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEService.h>
#include <vector>

#include <Preferences.h>


class PreferencesGlueString: public BLECharacteristicCallbacks {
    public:
    PreferencesGlueString(const char *uuid, const char *prefsKey, BLEService *pservice,QueueHandle_t prefsChangedQueue, Preferences *preferences);
    void onWrite(BLECharacteristic* pCharacteristic);
    void onRead(BLECharacteristic* pCharacteristic);

    private:
    const char *_prefsKey;
    BLECharacteristic *_characteristic;
    Preferences *_preferences;
    QueueHandle_t _prefsChangedQueue;
};


class BTPreferencesService {
    public:
    BLEService *pservice;


    BTPreferencesService(BLEServer *server, QueueHandle_t prefsChangedQueue);

    BLECharacteristic *availableSeparatorAnimationsCharacteristic;
    PreferencesGlueString *separatorAnimationsGlue;
};




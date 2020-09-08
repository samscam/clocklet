#pragma once

#include <NimBLEDevice.h>

#include "BTPreferencesGlue.h"

class ResetHandler: public NimBLECharacteristicCallbacks {
public:
    ResetHandler(NimBLEService *pService);
    void onWrite(NimBLECharacteristic* pCharacteristic);
private:
    NimBLECharacteristic *_characteristic;
};



class BTTechnicalService {
public:
    BTTechnicalService(NimBLEServer *server);

private:
    NimBLEService *deviceInfoService;
    NimBLEService *pservice;

    NimBLECharacteristic *cManufacturerName;
    NimBLECharacteristic *cModelNumber;
    NimBLECharacteristic *cSerialNumber;
    NimBLECharacteristic *cFirmwareVersion;

    ResetHandler *resetHandler;

    PreferencesGlue<bool> *stagingGlue;
    PreferencesGlue<bool> *autoUpdatesGlue;
};




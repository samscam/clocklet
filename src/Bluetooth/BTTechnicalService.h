#pragma once

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEService.h>


#include "BTPreferencesGlue.h"

class ResetHandler: public BLECharacteristicCallbacks {
public:
    ResetHandler(BLEService *pService);
    void onWrite(BLECharacteristic* pCharacteristic);
private:
    BLECharacteristic *_characteristic;
};



class BTTechnicalService {
public:
    BTTechnicalService(BLEServer *server);

private:
    BLEService *deviceInfoService;
    BLEService *pservice;

    BLECharacteristic *cManufacturerName;
    BLECharacteristic *cModelNumber;
    BLECharacteristic *cSerialNumber;
    BLECharacteristic *cFirmwareVersion;

    ResetHandler *resetHandler;

    PreferencesGlue<bool> *stagingGlue;
};




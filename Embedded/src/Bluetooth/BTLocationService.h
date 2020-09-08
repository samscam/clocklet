#pragma once

#include <NimBLEDevice.h>

#include "../Location/LocationManager.h"

class BTLocationService: public NimBLECharacteristicCallbacks {
public:
    BTLocationService(LocationManager *locationManager, BLEServer *pServer);
    void onWrite(NimBLECharacteristic* pCharacteristic);
    void onRead(NimBLECharacteristic* pCharacteristic);

private:
    BLEService *_sv_location;
    BLECharacteristic *_ch_currentLocation;
    LocationManager *_locationManager;
};
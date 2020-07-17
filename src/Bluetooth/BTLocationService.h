#pragma once

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "../Location/LocationManager.h"

class BTLocationService: public BLECharacteristicCallbacks {
public:
    BTLocationService(LocationManager *locationManager, BLEServer *pServer);
    void onWrite(BLECharacteristic* pCharacteristic);
    void onRead(BLECharacteristic* pCharacteristic);

private:
    BLEService *_sv_location;
    BLECharacteristic *_ch_currentLocation;
    LocationManager *_locationManager;
};
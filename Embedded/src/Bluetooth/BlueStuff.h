#pragma once

#include <NimBLEDevice.h>
#include <vector>

#include <WiFi.h>
#include <WiFiMulti.h>
#include "Utilities/Task.h"


#include "BTPreferencesService.h"
#include "BTNetworkService.h"
#include "BTLocationService.h"
#include "BTTechnicalService.h"

#include "../Location/LocationManager.h"
#include "../Utilities/Task.h"


class BlueStuff: public NimBLEServerCallbacks {

public:
    BlueStuff(QueueHandle_t bluetoothConnectedQueue,
                QueueHandle_t preferencesChangedQueue,
                QueueHandle_t networkChangedQueue,
                QueueHandle_t networkStatusQueue,
                LocationManager *locationManager);
    void startBlueStuff();
    void stopBlueStuff();

    // BLE callbacks
    void onConnect(NimBLEServer* server);
    void onDisconnect(NimBLEServer* server);

    void wifiEvent(WiFiEvent_t event);

    
private:


    char _deviceName[15];
    char _mfrData[10];

    void _setupAdvertising();
    bool _bluetoothRunning = false;
    
    QueueHandle_t _bluetoothConnectedQueue;
    NimBLEServer *pServer;

    BTTechnicalService* _technicalService;

    BTPreferencesService* _preferencesService;
    QueueHandle_t _preferencesChangedQueue;

    BTNetworkService* _networkService;
    QueueHandle_t _networkChangedQueue;
    QueueHandle_t _networkStatusQueue;

    BTLocationService *_locationService;
    LocationManager *_locationManager;
    BLEAdvertisementData _advertisementData;
    BLEAdvertisementData _scanResponseData;
    std::string _mfrdataString;
};

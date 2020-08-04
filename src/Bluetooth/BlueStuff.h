#pragma once

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
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


class BlueStuff: public BLEServerCallbacks {

public:
    BlueStuff(QueueHandle_t preferencesChangedQueue,
                QueueHandle_t networkChangedQueue,
                QueueHandle_t networkStatusQueue,
                LocationManager *locationManager);
    void startBlueStuff();
    void stopBlueStuff();

    // BLE callbacks
    void onConnect(BLEServer* server);
    void onDisconnect(BLEServer* server);

    void wifiEvent(WiFiEvent_t event);

    
private:
    void _setupAdvertising();
    bool _keepRunning = true;
    
    BLEServer *pServer;

    BTTechnicalService* _technicalService;

    BTPreferencesService* _preferencesService;
    QueueHandle_t _preferencesChangedQueue;

    BTNetworkService* _networkService;
    QueueHandle_t _networkChangedQueue;
    QueueHandle_t _networkStatusQueue;

    BTLocationService *_locationService;
    LocationManager *_locationManager;
};

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


class BlueStuff: public BLEServerCallbacks, public Task {

public:
    BlueStuff(QueueHandle_t preferencesChangedQueue,
                QueueHandle_t networkChangedQueue,
                QueueHandle_t networkStatusQueue,
                LocationManager *locationManager);
    void run(void *data);
    void startBlueStuff();
    void stopBlueStuff();

    // BLE callbacks
    void onConnect(BLEServer* server);
    void onDisconnect(BLEServer* server);

    void wifiEvent(WiFiEvent_t event);

    // WiFi callbacks
    bool isAlreadyProvisioned();
    
private:

    bool _keepRunning = true;

    esp_err_t _app_prov_is_provisioned(bool &provisioned);
    
    BLEServer *pServer;

    BLEService *sv_GAS;
    BLECharacteristic *ch_ServiceChanged;
    
    BTTechnicalService* _technicalService;

    BTPreferencesService* _preferencesService;
    QueueHandle_t _preferencesChangedQueue;

    BTNetworkService* _networkService;
    QueueHandle_t _networkChangedQueue;
    QueueHandle_t _networkStatusQueue;

    BTLocationService *_locationService;
    LocationManager *_locationManager;
};


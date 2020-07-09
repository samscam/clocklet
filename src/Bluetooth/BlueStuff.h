#pragma once

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <vector>

#include <WiFi.h>
#include <WiFiMulti.h>



#include "BTPreferencesService.h"
#include "BTNetworkService.h"



class BlueStuff: public BLEServerCallbacks {

public:
    BlueStuff(QueueHandle_t preferencesChangedQueue, QueueHandle_t networkChangedQueue);

    void startBlueStuff();
    void stopBlueStuff();

    // BLE callbacks
    void onConnect(BLEServer* server);
    void onDisconnect(BLEServer* server);

    void wifiEvent(WiFiEvent_t event);

    // WiFi callbacks

    
private:

    bool _keepRunning = true;

    void _startLocationService();
    

    BLEServer *pServer;

    BLEService *sv_GAS;
    BLECharacteristic *ch_ServiceChanged;
    
    BLEService *sv_location;
    BLECharacteristic *ch_currentLocation; // read/write current location {"lng":lng,"lat":lat}

    // Other clocklet prefs
    BLEService *sv_preferences;
    BLECharacteristic *ch_location; // read/write current location lat,lng pair
    BLECharacteristic *ch_brightness; // read/write min/max brightness
    
    // std::vector<NetworkInfo> networks;
    BTPreferencesService* _preferencesService;
    QueueHandle_t _preferencesChangedQueue;

    BTNetworkService* _networkService;
    QueueHandle_t _networkChangedQueue;

};


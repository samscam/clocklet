#pragma once

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <vector>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <ArduinoJson.h>
#define MAX_NETS 128

#include "BTPreferencesService.h"


struct NetworkInfo {
    uint8_t index;
    uint8_t enctype;
    int32_t rssi;
    uint8_t * bssid;
    int32_t channel;
    String ssid;
};

void wifiEventCb(WiFiEvent_t event);

class BlueStuff: public BLEServerCallbacks {

public:
    BlueStuff(QueueHandle_t preferencesChangedQueue);

    void startBlueStuff();
    void stopBlueStuff();

    // BLE callbacks
    void onConnect(BLEServer* server);
    void onDisconnect(BLEServer* server);

    void wifiEvent(WiFiEvent_t event);

    // WiFi callbacks

    bool _shouldScan = false;
    
private:
    QueueHandle_t _preferencesChangedQueue;
    bool _keepRunning = true;

    void _startNetworkService();
    void _startLocationService();
    
    void _updateCurrentNetwork();

    void _startWifiScan();
    void _encodeNetInfo(JsonDocument &doc, NetworkInfo netInfo);

    BLEServer *pServer;

    BLEService *sv_GAS;
    BLECharacteristic *ch_ServiceChanged;
    

    // Network provisioning
    BLEService *sv_network;
    BLECharacteristic *ch_currentNetwork; // returns info on current network connection
    BLECharacteristic *ch_availableNetworks; // returns list of known and available networks
    BLECharacteristic *ch_removeNetwork; // takes an SSID of one of the known networks - removes it from the list preventing future connection
    BLECharacteristic *ch_joinNetwork; // Joins a network

    BLEService *sv_location;
    BLECharacteristic *ch_currentLocation; // read/write current location {"lng":lng,"lat":lat}

    // Other clocklet prefs
    BLEService *sv_preferences;
    BLECharacteristic *ch_location; // read/write current location lat,lng pair
    BLECharacteristic *ch_brightness; // read/write min/max brightness
    
    // std::vector<NetworkInfo> networks;
    BTPreferencesService* _preferencesService;

};


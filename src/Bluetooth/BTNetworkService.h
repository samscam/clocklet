#pragma once

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEService.h>
#include <WiFi.h>
#include <ArduinoJson.h>

struct NetworkInfo {
    uint8_t index;
    uint8_t enctype;
    int32_t rssi;
    uint8_t * bssid;
    int32_t channel;
    String ssid;
};


class BTNetworkService: public BLECharacteristicCallbacks  {
public:
    BTNetworkService(BLEServer *server, QueueHandle_t networkChangedQueue, QueueHandle_t networkStatusQueue);
    ~BTNetworkService() {};

    void wifiEvent(WiFiEvent_t event);
    void onWrite(BLECharacteristic* pCharacteristic);
private:

    void _updateCurrentNetwork();
    void _startWifiScan();
    void _encodeNetInfo(JsonDocument &doc, NetworkInfo netInfo);


    bool _shouldScan = false;

    String _mac2String(uint8_t * bytes);

    QueueHandle_t _networkChangedQueue;
    QueueHandle_t _networkStatusQueue;

    // Network provisioning
    BLEService *sv_network;
    BLECharacteristic *ch_currentNetwork; // returns info on current network connection
    BLECharacteristic *ch_availableNetworks; // returns list of known and available networks
    BLECharacteristic *ch_removeNetwork; // takes an SSID of one of the known networks - removes it from the list preventing future connection
    BLECharacteristic *ch_joinNetwork; // Joins a network
};



void wifiEventCb(WiFiEvent_t event);
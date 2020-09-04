#pragma once

#include <NimBLEDevice.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "Utilities/Task.h"

#define SV_NETWORK_UUID     "68D924A1-C1B2-497B-AC16-FD1D98EDB41F"

enum WifiConfigStatus {
    notConfigured,
    configured
};

struct NetworkInfo {
    uint8_t index;
    uint8_t enctype;
    int32_t rssi;
    uint8_t * bssid;
    int32_t channel;
    String ssid;
};

class NetworkScanTask: public Task {
public:
    NetworkScanTask(NimBLECharacteristic *availableNetworks);
    void run(void *data);
private:
    NimBLECharacteristic *ch_availableNetworks;
    void _performWiFiScan();
    void _encodeNetInfo(JsonDocument &doc, NetworkInfo netInfo);
};

class BTNetworkService: public NimBLECharacteristicCallbacks  {
public:
    BTNetworkService(NimBLEServer *server, QueueHandle_t networkChangedQueue, QueueHandle_t networkStatusQueue);
    ~BTNetworkService() {};

    void wifiEvent(WiFiEvent_t event);
    void onWrite(NimBLECharacteristic* pCharacteristic);

    void onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue);
private:

    void _updateCurrentNetwork();

    void _encodeNetInfo(JsonDocument &doc, NetworkInfo netInfo);

    QueueHandle_t _networkChangedQueue;
    QueueHandle_t _networkStatusQueue;

    NetworkScanTask *_networkScanTask;

    // Network provisioning
    NimBLEService *sv_network;
    NimBLECharacteristic *ch_currentNetwork; // returns info on current network connection
    NimBLECharacteristic *ch_availableNetworks; // returns list of known and available networks

    NimBLECharacteristic *ch_removeNetwork; // takes an SSID of one of the known networks - removes it from the list preventing future connection
    NimBLECharacteristic *ch_joinNetwork; // Joins a network

    wifi_event_id_t _wifiEvent;
};

String _mac2String(uint8_t * bytes);

void wifiEventCb(WiFiEvent_t event);

#include "BlueStuff.h"


#include <Preferences.h>

#include <WiFi.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <esp_err.h>
#include <nvs.h>

#include <esp_log.h>
#include "Loggery.h"

#include "../ClockletSystem.h"

#include "nimble/nimble/host/services/gatt/include/services/gatt/ble_svc_gatt.h"

#define TAG "BLUESTUFF"

const char * shortName = "Clocklet";



BlueStuff::BlueStuff(QueueHandle_t bluetoothConnectedQueue,
            QueueHandle_t preferencesChangedQueue,
            QueueHandle_t networkChangedQueue,
            QueueHandle_t networkStatusQueue,
            QueueHandle_t godModeQueue,
            LocationManager *locationManager) {
    _bluetoothConnectedQueue = bluetoothConnectedQueue;
    _preferencesChangedQueue =  preferencesChangedQueue;
    _networkChangedQueue = networkChangedQueue;
    _networkStatusQueue = networkStatusQueue;
    _godModeQueue = godModeQueue;
    _locationManager = locationManager;


    uint32_t serial = clocklet_serial();
    sprintf(_deviceName,"%s #%d",shortName,serial);
}

void BlueStuff::startBlueStuff(){
    LOGMEM;
    if (_bluetoothRunning){
        return;
    }

    _bluetoothRunning = true;

    ESP_LOGI(TAG,"Starting BLE work!");
    
    LOGMEM;

    NimBLEDevice::init(_deviceName);
    
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
    NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM | BLE_SM_PAIR_AUTHREQ_SC);

    pServer = NimBLEDevice::createServer();

    pServer->setCallbacks(this);

    _technicalService = new BTTechnicalService(pServer);
    _locationService = new BTLocationService(_locationManager,pServer);
    _networkService = new BTNetworkService(pServer, _networkChangedQueue, _networkStatusQueue);
    _preferencesService = new BTPreferencesService(pServer, _preferencesChangedQueue);
    _godModeService = new BTGodModeService(pServer, _godModeQueue);

    // BLE Advertising
    _setupAdvertising();
    
    // Force update to overcome attribute caching on iOS
    // when a service or characteristic has changed
    ble_svc_gatt_changed(0x0001, 0xffff);

    pServer->start();

    LOGMEM;
}

void BlueStuff::_setupAdvertising(){
    

    uint16_t hwrev = clocklet_hwrev();
    uint16_t caseColour = clocklet_caseColour();
    uint32_t serial = clocklet_serial();

    if (isnan(serial)){
        serial = 0;
    }

    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    // pAdvertising->addServiceUUID(SV_NETWORK_UUID);

    // MAIN ADVERTISING PACKET
    _advertisementData.setPartialServices(BLEUUID(SV_NETWORK_UUID));

    char mfrdataBuffer[10];

    const char vendorID[2] = { 0x02, 0xE5 };
    memcpy(mfrdataBuffer, vendorID, 2);

    mfrdataBuffer[2] = (hwrev >> 0) & 0xFF;
    mfrdataBuffer[3] = (hwrev >> 8) & 0xFF;

    mfrdataBuffer[4] = (caseColour >> 0) & 0xFF;
    mfrdataBuffer[5] = (caseColour >> 8) & 0xFF;

    mfrdataBuffer[6] = (serial >> 0) & 0xFF;
    mfrdataBuffer[7] = (serial >> 8) & 0xFF;
    mfrdataBuffer[8] = (serial >> 16) & 0xFF;
    mfrdataBuffer[9] = (serial >> 24) & 0xFF;

    _mfrdataString = std::string(mfrdataBuffer,sizeof(mfrdataBuffer));
    _advertisementData.setManufacturerData(_mfrdataString);
    
    pAdvertising->setAdvertisementData(_advertisementData);
    // Manufacturer Data: 10 bytes
    // Service uuid: 16 bytes
    // = 26 bytes - WILL NOT CHANGE

    // SCAN RESPONSE PACKET
    _scanResponseData.setName(_deviceName);
    pAdvertising->setScanResponseData(_scanResponseData);

    pAdvertising->start();

}
void BlueStuff::stopBlueStuff(){
    if (!_bluetoothRunning){
        return;
    }
    _bluetoothRunning = false;

    delete(_technicalService);
    delete(_locationService);
    delete(_networkService);
    delete(_preferencesService);
    delete(_godModeService);

    BLEDevice::deinit(false);

}

void BlueStuff::onConnect(NimBLEServer* server) {
    ESP_LOGI(TAG,"Bluetooth client connected");
    bool change = true;
    xQueueSend(_bluetoothConnectedQueue, &change, (TickType_t) 0);
}

void BlueStuff::onDisconnect(NimBLEServer* server) {
    ESP_LOGI(TAG,"Bluetooth client disconnected");
    bool change = false;
    xQueueSend(_bluetoothConnectedQueue, &change, (TickType_t) 0);
}



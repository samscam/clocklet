
#include "BlueStuff.h"
#include "esp_bt_device.h"

#include <Preferences.h>

#include "BLE2902.h"
#include <WiFi.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <esp_err.h>
#include <nvs.h>

#include <esp_log.h>
#include "Loggery.h"

#include "../ClockletSystem.h"


#define TAG "BLUESTUFF"

const char * shortName = "Clocklet";

BlueStuff::BlueStuff(QueueHandle_t bluetoothConnectedQueue,
            QueueHandle_t preferencesChangedQueue,
            QueueHandle_t networkChangedQueue,
            QueueHandle_t networkStatusQueue, LocationManager *locationManager) {
    _bluetoothConnectedQueue = bluetoothConnectedQueue;
    _preferencesChangedQueue =  preferencesChangedQueue;
    _networkChangedQueue = networkChangedQueue;
    _networkStatusQueue = networkStatusQueue;
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

    BLEDevice::init(_deviceName);
    
    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
    
    pServer = BLEDevice::createServer();

    // BLE Advertising
    _setupAdvertising();

    pServer->setCallbacks(this);

    _technicalService = new BTTechnicalService(pServer);
    _locationService = new BTLocationService(_locationManager,pServer);
    _networkService = new BTNetworkService(pServer, _networkChangedQueue, _networkStatusQueue);
    _preferencesService = new BTPreferencesService(pServer, _preferencesChangedQueue);



    LOGMEM;
}

void BlueStuff::_setupAdvertising(){

    uint16_t hwrev = clocklet_hwrev();
    uint16_t caseColour = clocklet_caseColour();
    uint32_t serial = clocklet_serial();

    if (isnan(serial)){
        serial = 0;
    }

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    
    BLEAdvertisementData advertisementData;
    BLEAdvertisementData scanResponseData;

    scanResponseData.setName(_deviceName);
    advertisementData.setPartialServices(BLEUUID(SV_NETWORK_UUID));

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

    auto s = std::string(mfrdataBuffer,sizeof(mfrdataBuffer));
    advertisementData.setManufacturerData(s);
    // Manufacturer Data: 10 bytes
    // Service uuid: 16 bytes
    // = 26 bytes - WILL NOT CHANGE - putting those in the advertising packet

    // Name: 13 bytes - will eventually get longer - putting it in the scan response
    pAdvertising->setAdvertisementData(advertisementData);
    pAdvertising->setScanResponseData(scanResponseData);


    // Mythical settings that help with iPhone connections issue - don't seem to make any odds
    pAdvertising->setMinPreferred(0x06);  
    pAdvertising->setMaxPreferred(0x12);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();
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
    BLEDevice::deinit(false);


}

void BlueStuff::onConnect(BLEServer* server) {
    ESP_LOGI(TAG,"Bluetooth client connected");
    bool change = true;
    xQueueSend(_bluetoothConnectedQueue, &change, (TickType_t) 0);
    delay(500);

    _networkService->onConnect();
}

void BlueStuff::onDisconnect(BLEServer* server) {
    ESP_LOGI(TAG,"Bluetooth client disconnected");
    _networkService->onDisconnect();
    bool change = false;
    xQueueSend(_bluetoothConnectedQueue, &change, (TickType_t) 0);
}

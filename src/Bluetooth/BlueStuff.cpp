
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

BlueStuff::BlueStuff(QueueHandle_t preferencesChangedQueue,
            QueueHandle_t networkChangedQueue,
            QueueHandle_t networkStatusQueue, LocationManager *locationManager) {
    _preferencesChangedQueue =  preferencesChangedQueue;
    _networkChangedQueue = networkChangedQueue;
    _networkStatusQueue = networkStatusQueue;
    _locationManager = locationManager;
}

void BlueStuff::startBlueStuff(){
    LOGMEM;

    ESP_LOGI(TAG,"Starting BLE work!");

    uint32_t serial = clocklet_serial();
    
    LOGMEM;

    char *deviceName;
    asprintf(&deviceName,"%s #%d",shortName,serial);
    BLEDevice::init(deviceName);
    
    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
    
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    
    _locationService = new BTLocationService(_locationManager,pServer);
    _networkService = new BTNetworkService(pServer, _networkChangedQueue, _networkStatusQueue);
    _preferencesService = new BTPreferencesService(pServer, _preferencesChangedQueue);
    _technicalService = new BTTechnicalService(pServer);

    // BLE Advertising

    _setupAdvertising();

    LOGMEM;
}

void BlueStuff::_setupAdvertising(){

    uint16_t hwrev = clocklet_hwrev();
    uint16_t caseColour = clocklet_caseColour();
    uint32_t serial = clocklet_serial();
    char *deviceName;
    asprintf(&deviceName,"%s #%d",shortName,serial);

    if (isnan(serial)){
        serial = 0;
    }

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    
    BLEAdvertisementData advertisementData;
    BLEAdvertisementData scanResponseData;

    scanResponseData.setName(deviceName);
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

    pAdvertising->setAdvertisementData(advertisementData);
    pAdvertising->setScanResponseData(scanResponseData);


    // Mythical settings that help with iPhone connections issue - don't seem to make any odds
    pAdvertising->setMinPreferred(0x06);  
    pAdvertising->setMaxPreferred(0x12);
    pAdvertising->setScanResponse(true);
    pAdvertising->start();
}
void BlueStuff::stopBlueStuff(){
    
}

void BlueStuff::onConnect(BLEServer* server) {
    ESP_LOGI(TAG,"Bluetooth client connected");

    delay(500);

    _networkService->onConnect();
}

void BlueStuff::onDisconnect(BLEServer* server) {
    ESP_LOGI(TAG,"Bluetooth client disconnected");
    _networkService->onDisconnect();

    // Advertising has a habit of getting trashed on disconnect...
    // Force it to work...
    _setupAdvertising();
}

#include "BTLocationService.h"

#include <esp_log.h>
#include "Loggery.h"

#include <ArduinoJson.h>

#define SV_LOCATION_UUID "87888F3E-C1BF-4832-9823-F19C73328D30"
#define CH_CURRENTLOCATION_UUID "C8C7FF91-531A-4306-A68A-435374CB12A9"

BTLocationService::BTLocationService(LocationManager *locationManager, BLEServer *pServer){
    _locationManager = locationManager;
    ESP_LOGI(TAG, "Starting location service %s",SV_LOCATION_UUID);
    _sv_location= pServer->createService(SV_LOCATION_UUID);

    _ch_currentLocation = _sv_location->createCharacteristic(
                                            CH_CURRENTLOCATION_UUID,
                                            BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE
                                        );
    _ch_currentLocation->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

    _ch_currentLocation->setCallbacks(this);

    _sv_location->start();
}

void BTLocationService::onWrite(BLECharacteristic* pCharacteristic) {
    LOGMEM;
    std::string msg = pCharacteristic->getValue();
    ESP_LOGI(TAG,"BLE Location Characteristic Write: %s\n", msg.c_str());

    StaticJsonDocument<512> doc;
    deserializeJson(doc,msg);
    
    const double lat = doc["lat"];
    if (isnan(lat)) {
        ESP_LOGI(TAG,"Invalid latitude");
        return;
    }

    const double lng = doc["lng"];
    if (isnan(lng)) {
        ESP_LOGI(TAG,"Invalid longitude");
        return;
    }
    Location newLocation = {};
    newLocation.lat = lat;
    newLocation.lng = lng;
    _locationManager->setLocation(newLocation);

}

void BTLocationService::onRead(BLECharacteristic* pCharacteristic) {
    LOGMEM;
    std::string msg = pCharacteristic->getValue();
    ESP_LOGI(TAG, "BLE Location Characteristic Read: %s, %i", msg.c_str(), msg.length());
    // esp_log_buffer_char(LOG_TAG, msg.c_str(), msg.length());
    // esp_log_buffer_hex(LOG_TAG, msg.c_str(), msg.length());
    Location location = _locationManager->getLocation();

    StaticJsonDocument<80> locDoc;
    
    locDoc["lat"]=location.lat;
    locDoc["lng"]=location.lng;

    String outputStr = "";
    serializeJson(locDoc,outputStr);
    uint len = outputStr.length()+1;
    char json[len];
    outputStr.toCharArray(json,len);
    ESP_LOGD(TAG,json);

    pCharacteristic->setValue(json);
}
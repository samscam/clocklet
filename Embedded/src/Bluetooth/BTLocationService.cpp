#include "BTLocationService.h"

#include <esp_log.h>
#include "Loggery.h"

#include <ArduinoJson.h>

#define SV_LOCATION_UUID "87888F3E-C1BF-4832-9823-F19C73328D30"
#define CH_CURRENTLOCATION_UUID "C8C7FF91-531A-4306-A68A-435374CB12A9"

#define TAG "BTLocationService"

BTLocationService::BTLocationService(LocationManager *locationManager, NimBLEServer *pServer){
    _locationManager = locationManager;
    ESP_LOGI(TAG, "Starting location service %s",SV_LOCATION_UUID);
    _sv_location= pServer->createService(SV_LOCATION_UUID);

    _ch_currentLocation = _sv_location->createCharacteristic(
                                            CH_CURRENTLOCATION_UUID,
                                            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | 
                                            NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC |
                                            NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
                                        );

    _ch_currentLocation->setCallbacks(this);

    _sv_location->start();
}

void BTLocationService::onWrite(NimBLECharacteristic* pCharacteristic) {
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

    const char* placeName = doc["placeName"];
    if (!strlen(placeName) || strlen(placeName)>255) {
        ESP_LOGI(TAG,"No place name");
        return;
    }

    const char* timeZone = doc["timeZone"];
    if (!strlen(timeZone) || strlen(timeZone)>63) {
        ESP_LOGI(TAG,"No timezone");
        return;
    }

    ClockLocation newLocation = {};
    newLocation.lat = lat;
    newLocation.lng = lng;
    strcpy(newLocation.placeName,placeName);
    strcpy(newLocation.timeZone,timeZone);
    
    _locationManager->setLocation(newLocation);

}

void BTLocationService::onRead(NimBLECharacteristic* pCharacteristic) {
    
    ClockLocation location = _locationManager->getLocation();

    StaticJsonDocument<500> locDoc;
    locDoc["configured"]=_locationManager->hasSavedLocation();
    locDoc["lat"]=location.lat;
    locDoc["lng"]=location.lng;
    locDoc["placeName"] = location.placeName;
    locDoc["timeZone"] = location.timeZone;

    String outputStr = "";
    serializeJson(locDoc,outputStr);
    uint len = outputStr.length()+1;
    char json[len];
    outputStr.toCharArray(json,len);
    ESP_LOGD(TAG,"%s",json);

    pCharacteristic->setValue(json);

}
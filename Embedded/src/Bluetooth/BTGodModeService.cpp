#include "BTGodModeService.h"
#include <ArduinoJson.h>

#define SV_GODMODE_UUID "03205794-6A59-42E5-9B8D-BB3879716FD5"

#define CH_GODMODE_ENABLED_UUID "603B79B2-568A-4AFF-8B9A-08DF689C9D49"
#define CH_GODMODE_WEATHER_UUID "CAA822A1-4CFB-4F7B-8169-3EB56DEADA13"

BTGodModeService::BTGodModeService(NimBLEServer *pServer, QueueHandle_t godModeQueue){
    _godModeQueue = godModeQueue;
    _sv_godmode = pServer->createService(SV_GODMODE_UUID);

    _ch_enabled = _sv_godmode->createCharacteristic(
        CH_GODMODE_ENABLED_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC);
    _ch_enabled->setCallbacks(this);

    _ch_weather = _sv_godmode->createCharacteristic(
        CH_GODMODE_WEATHER_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC);
    _ch_weather->setCallbacks(this);

    _sv_godmode->start();
}

void BTGodModeService::onRead(BLECharacteristic* pCharacteristic) {
    if (pCharacteristic == _ch_enabled){
        pCharacteristic->setValue(_godModeSettings.enabled);
        return;
    }
    if (pCharacteristic == _ch_weather){
        StaticJsonDocument<512> doc;
        
        doc["precipChance"] = _godModeSettings.weather.precipChance;
        doc["precipIntensity"] = _godModeSettings.weather.precipIntensity;
        doc["precipType"] = _godModeSettings.weather.precipType;
        doc["maxTmp"] = _godModeSettings.weather.maxTmp;
        doc["minTmp"] = _godModeSettings.weather.minTmp;
        doc["windSpeed"] = _godModeSettings.weather.windSpeed;
        doc["thunder"] = _godModeSettings.weather.thunder;
        doc["rainbows"] = _godModeSettings.weather.rainbows;
        
        std::string outputStr;
        serializeJson(doc,outputStr);
        ESP_LOGI("GOD","Reading weather %s", outputStr.c_str() );
        
        pCharacteristic->setValue(outputStr);
        return;
    }
}

void BTGodModeService::onWrite(BLECharacteristic* pCharacteristic) {

    if (pCharacteristic == _ch_enabled){
        bool newVal = pCharacteristic->getValue<bool>();
        _godModeSettings.enabled = newVal;
        ESP_LOGI("GOD","God mode %s",newVal ? "TRUE" : "FALSE");
    }
    if (pCharacteristic == _ch_weather){
        std::string msg = pCharacteristic->getValue();
        ESP_LOGI("GOD","God mode %s",msg.c_str());
        StaticJsonDocument<512> doc;
        deserializeJson(doc,msg);
        
        _godModeSettings.weather.precipChance = doc["precipChance"];
        _godModeSettings.weather.precipIntensity = doc["precipIntensity"];
        _godModeSettings.weather.precipType = doc["precipType"];
        _godModeSettings.weather.maxTmp = doc["maxTmp"];
        _godModeSettings.weather.minTmp = doc["minTmp"];
        _godModeSettings.weather.windSpeed = doc["windSpeed"];
        _godModeSettings.weather.thunder = doc["thunder"];
        _godModeSettings.weather.rainbows = doc["rainbows"];
    }
    xQueueSend(_godModeQueue, &_godModeSettings, (TickType_t) 0);

}

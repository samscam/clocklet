#include "BTGodModeService.h"
#include <ArduinoJson.h>

#define SV_GODMODE_UUID "03205794-6A59-42E5-9B8D-BB3879716FD5"

#define CH_GODMODE_UUID "603B79B2-568A-4AFF-8B9A-08DF689C9D49"


BTGodModeService::BTGodModeService(NimBLEServer *pServer, QueueHandle_t godModeQueue){
    _godModeQueue = godModeQueue;
    _sv_godmode = pServer->createService(SV_GODMODE_UUID);
    _ch_godmode = _sv_godmode->createCharacteristic(
        CH_GODMODE_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC);
    
    _ch_godmode->setCallbacks(this);
    _sv_godmode->start();
}

void BTGodModeService::onRead(BLECharacteristic* pCharacteristic) {

}

void BTGodModeService::onWrite(BLECharacteristic* pCharacteristic) {
    std::string msg = pCharacteristic->getValue();

    StaticJsonDocument<512> doc;
    deserializeJson(doc,msg);
    
    _godModeSettings.enabled = doc["enabled"];
    _godModeSettings.weather.type = doc["weather"]["type"];
    _godModeSettings.weather.precipChance = doc["weather"]["precipChance"];
    _godModeSettings.weather.precipIntensity = doc["weather"]["precipIntensity"];
    _godModeSettings.weather.precipType = doc["weather"]["precipType"];
    _godModeSettings.weather.maxTmp = doc["weather"]["maxTmp"];
    _godModeSettings.weather.maxTmp = doc["weather"]["maxTmp"];
    _godModeSettings.weather.minTmp = doc["weather"]["minTmp"];
    _godModeSettings.weather.currentTmp = doc["weather"]["currentTmp"];
    _godModeSettings.weather.thunder = doc["weather"]["thunder"];
    _godModeSettings.weather.windSpeed = doc["weather"]["windSpeed"];
    _godModeSettings.weather.cloudCover = doc["weather"]["cloudCover"];
    _godModeSettings.weather.pressure = doc["weather"]["pressure"];
    _godModeSettings.weather.rainbows = doc["weather"]["rainbows"];

    xQueueSend(_godModeQueue, &_godModeSettings, (TickType_t) 0);

}

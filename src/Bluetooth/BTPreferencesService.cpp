#include "BTPreferencesService.h"

BTPreferencesService::BTPreferencesService(BLEServer *server, QueueHandle_t prefsChangedQueue){

    pservice = server->createService("28C65464-311E-4ABF-B6A0-D03B0BAA2815");

    Preferences *preferences = new Preferences();
    preferences->begin("clocklet", false);


    // The available separator animations
    // JSON fragment
    availableSeparatorAnimationsCharacteristic = pservice->createCharacteristic(
        "9982B160-23EF-42FF-9848-31D9FF21F490",
        BLECharacteristic::PROPERTY_READ);
    availableSeparatorAnimationsCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED);

    availableSeparatorAnimationsCharacteristic->setValue("[\"Static\",\"Blink\",\"Fade\"]");


    separatorAnimationsGlue = new PreferencesGlueString("2371E298-DCE5-4E1C-9CB2-5542213CE81C",
    "sep_anim",
    pservice,
    prefsChangedQueue,
    preferences, "blink");


    availableTimeStyles = pservice->createCharacteristic(
        "698D2B57-5B54-48D7-A483-1AB4660FBAF9",
        BLECharacteristic::PROPERTY_READ);
    availableTimeStyles->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED);
    availableTimeStyles->setValue("[\"24 Hour\",\"12 Hour\",\"Decimal\"]");

    timeStyleGlue = new PreferencesGlueString("AE35C2DE-7D36-4699-A5CE-A0FA6A0A5483",
    "time_style",
    pservice,
    prefsChangedQueue,
    preferences,"24 Hour");
    
    pservice->start();
}



PreferencesGlueString::PreferencesGlueString(const char *uuid, const char *prefsKey, BLEService *pservice,QueueHandle_t prefsChangedQueue, Preferences *preferences, const char *defaultValue){
    _prefsKey = prefsKey;
    _preferences = preferences;
    _prefsChangedQueue = prefsChangedQueue;

    _characteristic = pservice->createCharacteristic(
        uuid,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    _characteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    _characteristic->setCallbacks(this);
    String value = _preferences->getString(_prefsKey,defaultValue);
    _characteristic->setValue(value.c_str());
}

void PreferencesGlueString::onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    _preferences->putString(_prefsKey, value.c_str());
    bool change = true;
    xQueueSend(_prefsChangedQueue, &change, (TickType_t) 0);
}

void PreferencesGlueString::onRead(BLECharacteristic* pCharacteristic) {
    // String value = _preferences->getString(_prefsKey);
    // pCharacteristic->setValue(value.c_str());
}



#pragma once

#include <NimBLEDevice.h>

#include <Preferences.h>

#define TAG_GLUE "GLUE"

template <class T>
class PreferencesGlue: public NimBLECharacteristicCallbacks {
public:
    PreferencesGlue(const char *uuid,
        const char *prefsKey,
        BLEService *pservice,
        QueueHandle_t prefsChangedQueue,
        const char *prefsNamespace,
        T defaultValue);
    ~PreferencesGlue();

    void onWrite(NimBLECharacteristic* pCharacteristic);
    void onRead(NimBLECharacteristic* pCharacteristic);

private:
    Preferences *preferences;
    const char* _prefsNamespace;
    const char *_prefsKey;
    T _defaultValue;
    NimBLECharacteristic *_characteristic;
    QueueHandle_t _prefsChangedQueue;
};


template <class T> inline
PreferencesGlue<T>::PreferencesGlue(const char *uuid, const char *prefsKey, BLEService *pservice,QueueHandle_t prefsChangedQueue, const char *prefsNamespace, T defaultValue){
    _prefsKey = prefsKey;
    _prefsNamespace = prefsNamespace;
    _prefsChangedQueue = prefsChangedQueue;
    _defaultValue = defaultValue;
    preferences = new Preferences();
    _characteristic = pservice->createCharacteristic(
        uuid,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC |  NIMBLE_PROPERTY::WRITE_NR | NIMBLE_PROPERTY::WRITE_ENC);
    _characteristic->setCallbacks(this);
}

template <class T> inline
PreferencesGlue<T>::~PreferencesGlue(){

    delete(preferences);
}

template <class T>  inline
void PreferencesGlue<T>::onWrite(NimBLECharacteristic* pCharacteristic) {
    ESP_LOGE(TAG_GLUE,"Unimplemented Bluetooth Preferences Glue");
}

template <class T> inline
void PreferencesGlue<T>::onRead(NimBLECharacteristic* pCharacteristic) {
    ESP_LOGE(TAG_GLUE,"Unimplemented Bluetooth Preferences Glue");
}

template <>  inline
void PreferencesGlue<std::string>::onWrite(NimBLECharacteristic* pCharacteristic) {
    String value = String(pCharacteristic->getValue().c_str());
    ESP_LOGI(TAG_GLUE,"Preferences change: %s IS NOW %s",_prefsKey,value.c_str());

    preferences->begin(_prefsNamespace, false);
    preferences->putString(_prefsKey, value);
    preferences->end();

    bool change = true;
    if (_prefsChangedQueue){
        xQueueSend(_prefsChangedQueue, &change, (TickType_t) 0);
    }
}

template <> inline
void PreferencesGlue<std::string>::onRead(NimBLECharacteristic* pCharacteristic) {

    preferences->begin(_prefsNamespace, false);
    String value = preferences->getString(_prefsKey,_defaultValue.c_str());
    std::string sValue = std::string(value.c_str());
    ESP_LOGI(TAG_GLUE,"Preferences: %s IS \"%s\"",_prefsKey,sValue.c_str());
    pCharacteristic->setValue(sValue);
    preferences->end();
}

// BOOL

template <>  inline
void PreferencesGlue<bool>::onWrite(NimBLECharacteristic* pCharacteristic) {
    bool value = pCharacteristic->getValue<bool>();
    ESP_LOGI(TAG_GLUE,"Preferences change: %s IS NOW %d",_prefsKey,value);
    
    preferences->begin("clocklet", false);
    preferences->putBool(_prefsKey, value);
    preferences->end();

    bool change = true;
    if (_prefsChangedQueue){
        xQueueSend(_prefsChangedQueue, &change, (TickType_t) 0);
    }
}

template <> inline
void PreferencesGlue<bool>::onRead(NimBLECharacteristic* pCharacteristic) {
    preferences->begin("clocklet", false);
    bool prefsValue = preferences->getBool(_prefsKey,_defaultValue);
    uint8_t data[1];
    data[0] = prefsValue;
    pCharacteristic->setValue(data,1);
    preferences->end();
    ESP_LOGI(TAG_GLUE,"Preferences bool read: %s IS %d",_prefsKey,prefsValue);
    
}

// UINT8

template <>  inline
void PreferencesGlue<uint8_t>::onWrite(NimBLECharacteristic* pCharacteristic) {
    uint8_t value = pCharacteristic->getValue<uint8_t>();

    ESP_LOGI(TAG_GLUE,"Preferences change: %s IS NOW %d",_prefsKey,value);

    preferences->begin("clocklet", false);
    preferences->putUChar(_prefsKey, value);
    preferences->end();
    

    bool change = true;
    if (_prefsChangedQueue){
        xQueueSend(_prefsChangedQueue, &change, (TickType_t) 0);
    }
}

template <> inline
void PreferencesGlue<uint8_t>::onRead(NimBLECharacteristic* pCharacteristic) {

    preferences->begin("clocklet", false);
    uint8_t prefsValue = preferences->getUChar(_prefsKey,_defaultValue);
    uint8_t data[1] = {prefsValue};
    pCharacteristic->setValue(data,1);
    preferences->end();
}

template <>  inline
void PreferencesGlue<float_t>::onWrite(NimBLECharacteristic* pCharacteristic) {

    float_t value = pCharacteristic->getValue<float_t>();

    // ESP_LOGI(TAG_GLUE,"Preferences change: %s IS NOW %g",_prefsKey,value);

    preferences->begin("clocklet", false);
    preferences->putFloat(_prefsKey, value);
    preferences->end();

    bool change = true;
    if (_prefsChangedQueue){
        xQueueSend(_prefsChangedQueue, &change, (TickType_t) 0);
    }
}

template <> inline
void PreferencesGlue<float_t>::onRead(NimBLECharacteristic* pCharacteristic) {
    size_t size = sizeof(float_t);
    preferences->begin("clocklet", false);
    float_t prefsValue = preferences->getFloat(_prefsKey,_defaultValue);
    uint8_t data[size];
    memcpy(data, (uint8_t*) (&prefsValue), size );
    pCharacteristic->setValue(data,size);
    preferences->end();
}


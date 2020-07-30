#pragma once

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEService.h>

#include <Preferences.h>


template <class T>
class PreferencesGlue: public BLECharacteristicCallbacks {
public:
    PreferencesGlue(const char *uuid,
        const char *prefsKey,
        BLEService *pservice,
        QueueHandle_t prefsChangedQueue,
        const char *prefsNamespace,
        T defaultValue);

    void onWrite(BLECharacteristic* pCharacteristic);
    void onRead(BLECharacteristic* pCharacteristic);

private:
    const char* _prefsNamespace;
    const char *_prefsKey;
    T _defaultValue;
    BLECharacteristic *_characteristic;
    QueueHandle_t _prefsChangedQueue;
};


template <class T> inline
PreferencesGlue<T>::PreferencesGlue(const char *uuid, const char *prefsKey, BLEService *pservice,QueueHandle_t prefsChangedQueue, const char *prefsNamespace, T defaultValue){
    _prefsKey = prefsKey;
    _prefsNamespace = prefsNamespace;
    _prefsChangedQueue = prefsChangedQueue;
    _defaultValue = defaultValue;

    _characteristic = pservice->createCharacteristic(
        uuid,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    _characteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    _characteristic->setCallbacks(this);
}

template <class T>  inline
void PreferencesGlue<T>::onWrite(BLECharacteristic* pCharacteristic) {
    ESP_LOGE(TAG,"Unimplemented Bluetooth Preferences Glue");
}

template <class T> inline
void PreferencesGlue<T>::onRead(BLECharacteristic* pCharacteristic) {
    ESP_LOGE(TAG,"Unimplemented Bluetooth Preferences Glue");
}

template <>  inline
void PreferencesGlue<std::string>::onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    Preferences *preferences = new Preferences();
    preferences->begin(_prefsNamespace, false);
    preferences->putString(_prefsKey, value.c_str());
    preferences->end();
    delete(preferences);
    bool change = true;
    if (_prefsChangedQueue){
        xQueueSend(_prefsChangedQueue, &change, (TickType_t) 0);
    }
}

template <> inline
void PreferencesGlue<std::string>::onRead(BLECharacteristic* pCharacteristic) {
    Preferences *preferences = new Preferences();
    preferences->begin(_prefsNamespace, false);
    String value = preferences->getString(_prefsKey,_defaultValue.c_str());
    pCharacteristic->setValue(value.c_str());
    preferences->end();
    delete(preferences);
}


template <>  inline
void PreferencesGlue<bool>::onWrite(BLECharacteristic* pCharacteristic) {
    uint8_t *data = pCharacteristic->getData();

    bool value = (bool) data[0];
    ESP_LOGE(TAG,"STAGING IS NOW ****** %d",value);
    Preferences *preferences = new Preferences();
    preferences->begin("clocklet", false);
    preferences->putBool(_prefsKey, value);
    preferences->end();
    delete(preferences);

    bool change = true;
    if (_prefsChangedQueue){
        xQueueSend(_prefsChangedQueue, &change, (TickType_t) 0);
    }
}

template <> inline
void PreferencesGlue<bool>::onRead(BLECharacteristic* pCharacteristic) {
    Preferences *preferences = new Preferences();
    preferences->begin("clocklet", false);
    bool prefsValue = preferences->getBool(_prefsKey,_defaultValue);
    uint8_t data[1];
    data[0] = prefsValue;
    pCharacteristic->setValue(data,1);
    preferences->end();
    delete(preferences);

}

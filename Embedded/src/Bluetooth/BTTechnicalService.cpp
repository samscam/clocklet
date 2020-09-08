#include "BTTechnicalService.h"
#include "../ClockletSystem.h"
#include <esp_log.h>

#define TAG "BTTechnicalService"

BTTechnicalService::BTTechnicalService(NimBLEServer *server){

    // For stuff that fits in the device info service
    deviceInfoService = server->createService(BLEUUID((uint16_t)0x180a));

    cManufacturerName = deviceInfoService->createCharacteristic(BLEUUID((uint16_t)0x2A29), NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC);
    cManufacturerName->setValue("Spotlight Kid Ltd");

    cModelNumber = deviceInfoService->createCharacteristic(BLEUUID((uint16_t)0x2A24), NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC);
    uint16_t hwrev = clocklet_hwrev();
    char buf[11];
    sprintf(buf,"%d",hwrev);
    std::string modelNumber = std::string(buf);
    cModelNumber->setValue(modelNumber);

    cSerialNumber = deviceInfoService->createCharacteristic(BLEUUID((uint16_t)0x2A25), NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC);
    uint32_t serial = clocklet_serial();
    sprintf(buf,"%d",serial);
    std::string serialNumber = std::string(buf);
    cSerialNumber->setValue(serialNumber);
    
    char *cFirmwareVersionString;
    asprintf(&cFirmwareVersionString,"%s (%s)",VERSION,GIT_HASH);
    cFirmwareVersion = deviceInfoService->createCharacteristic(BLEUUID((uint16_t)0x2A26), NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC);
    std::string firmwareVersionString = std::string(cFirmwareVersionString);
    cFirmwareVersion->setValue(firmwareVersionString);
    deviceInfoService->start();

    // For other more custom stuff we have another service
    pservice = server->createService("417BD398-B942-4FF1-A759-02409F17D994");

    resetHandler = new ResetHandler(pservice);

    void *noQueue{ nullptr };
    stagingGlue = new PreferencesGlue<bool>("78FEC95A-61A9-4C1C-BDA1-F79245E118C0",
    "staging",
    pservice,
    noQueue,
    "clocklet",false);

    autoUpdatesGlue = new PreferencesGlue<bool>("3499658F-6794-4690-8728-EB96BFFD01FA",
    "autoUpdates",
    pservice,
    noQueue,
    "clocklet",true);

    pservice->start();
}



ResetHandler::ResetHandler(NimBLEService *pService){
    _characteristic = pService->createCharacteristic(
        "DD3FB44B-A925-4FC3-8047-77B1B6028B25",
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC);

    _characteristic->setCallbacks(this);
}


enum ResetType {
    nothing, reboot, partialReset, factoryReset
};

void ResetHandler::onWrite(NimBLECharacteristic* pCharacteristic) {
    ResetType resetType = pCharacteristic->getValue<ResetType>();
    ESP_LOGI(TAG,"Reset handler triggered: %d",resetType);
    switch (resetType) {
        case reboot:
            clocklet_reboot();
            break;
        case partialReset:
            clocklet_partialReset();
            break;
        case factoryReset:
            clocklet_factoryReset();
            break;
        case nothing:
            break;
    }
}

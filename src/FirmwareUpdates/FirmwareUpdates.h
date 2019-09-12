#pragma once

#include <WiFiClientSecure.h>

class FirmwareUpdates {
    public:
    FirmwareUpdates();
    void checkForUpdates();
    void startUpdate();
    bool updateAvailable;
    private:
    void processOTAUpdate(const char* url);
    char downloadURL[2048];
    WiFiClientSecure *client;
};
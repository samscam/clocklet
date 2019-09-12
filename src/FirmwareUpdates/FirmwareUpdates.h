#pragma once

class FirmwareUpdates {
    public:
    void checkForUpdates();
    private:
    void processOTAUpdate(const char* url);
};
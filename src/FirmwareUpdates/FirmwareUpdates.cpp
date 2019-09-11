#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

#include <ArduinoJson.h>

#include "FirmwareUpdates.h"
#include <functional>
#include <map>

void FirmwareUpdates::checkForUpdates() {
    WiFiClientSecure *client = new WiFiClientSecure;
    if (!client) {
        return;
    }

    // Extra scoping block
    {
        HTTPClient https;
        Serial.print("[HTTPS] begin...\n");

        // Fetch latest release data from github
        // GET /repos/:owner/:repo/releases/latest

        if (https.begin(*client, "https://api.github.com/repos/samscam/priscilla/releases/latest")) {  // HTTPS
            Serial.print("[HTTPS] GET...\n");
            // start connection and send HTTP header
            int httpCode = https.GET();
    
            // httpCode will be negative on error
            if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
    
            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                String payload = https.getString();
                Serial.println(payload);
                // Parse the json

                // Fish out verion string

                // Compare that to the local version

                // if local < latest

                // Fish out the binary url

                // Perform the update

            }
            } else {
            Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
            }
    
            https.end();
        } else {
            Serial.printf("[HTTPS] Unable to connect\n");
        }





    }

    delete client;
}
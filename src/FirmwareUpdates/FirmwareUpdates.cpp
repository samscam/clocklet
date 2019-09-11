#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

#include <ArduinoJson.h>

#include "FirmwareUpdates.h"
#include <semver.h>
#include "settings.h"

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

                // JSON parsing capacity with 10k overhead - should be more than plenty
                const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(13) + 3*JSON_OBJECT_SIZE(18) + 10240;
                DynamicJsonDocument doc(capacity);

                // Parse JSON object
                DeserializationError error = deserializeJson(doc, *client);
                if (error) {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.c_str());
                    return;
                }

                // Fish out verion string
                const char* tag_name = doc["tag_name"];
                
                // Strip the v from the start of the tag - if it doesn't have one, bail out
                int taglen = strlen(tag_name);
                char latestbuf[taglen];
                memcpy(latestbuf, &tag_name[1],taglen);
                Serial.println(latestbuf);

                semver_t latest = {};
                if (semver_parse(latestbuf, &latest)){
                    Serial.print("[FIRMWARE UPDATES] Failed to parse latest version");
                    return;
                }
                Serial.println("latest did parse");

                semver_t local = {};
                if ( semver_parse(VERSION, &local)){
                    Serial.print("[FIRMWARE UPDATES] Failed to parse local version");
                    return;
                }

                // Compare that to the local version
                // Bail unless an update is needed
                int comparison = semver_compare(local,latest);
                if (comparison < 0){
                    Serial.println("[FIRMWARE UPDATES] Latest is newer. Firmware update would be triggered");
                } else if (comparison == 0){
                    Serial.println("[FIRMWARE UPDATES] Local firmware is up to date");
                    return;
                } else {
                    Serial.println("[FIRMWARE UPDATES] Local firmware is newer than latest - must be developing");
                    return;
                }
                
                
                // Fish out the binary url
                JsonObject assets_0 = doc["assets"][0];
                const char* downloadURL = assets_0["browser_download_url"];
                Serial.printf("[FIRMWARE UPDATES] Download URL is %s\n",downloadURL);

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
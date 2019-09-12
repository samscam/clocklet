#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

#include <ArduinoJson.h>

#include "FirmwareUpdates.h"
#include <semver.h>
#include "settings.h"

/** 
 * Checks for firmware updates
 * If a valid firmware image is found it will apply the update
 * Ensure that the network is connected before running this
 * */

void FirmwareUpdates::checkForUpdates() {
    WiFiClientSecure *client = new WiFiClientSecure;
    if (!client) {
        Serial.println("[FIRMWARE UPDATES] failed to create network client...");
        return;
    }

    // Extra scoping block
    {
        HTTPClient https;
        Serial.print("[FIRMWARE UPDATES] begin...\n");

        // Fetch latest release data from github
        // GET /repos/:owner/:repo/releases/latest

        if (https.begin(*client, "https://api.github.com/repos/samscam/priscilla/releases/latest")) {  // HTTPS
            Serial.print("[FIRMWARE UPDATES] Fetching latest version from github...\n");
            // start connection and send HTTP header
            int httpCode = https.GET();
    
            // httpCode will be negative on error
            if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[FIRMWARE UPDATES] GET... code: %d\n", httpCode);
    
            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {

                // JSON parsing capacity with 10k overhead - should be more than plenty
                const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(13) + 3*JSON_OBJECT_SIZE(18) + 10240;
                DynamicJsonDocument doc(capacity);

                // Parse JSON object
                DeserializationError error = deserializeJson(doc, *client);
                if (error) {
                    Serial.print(F("[FIRMWARE UPDATES] deserializeJson() failed: "));
                    Serial.println(error.c_str());
                    return;
                }

                https.end();

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
                if (comparison == 0) {
                    Serial.println("[FIRMWARE UPDATES] Local firmware is up to date");
                    return;
                } else if (comparison > 0) {
                    Serial.println("[FIRMWARE UPDATES] Local firmware is newer than latest - must be developing");
                    return;
                } else {
                    Serial.println("[FIRMWARE UPDATES] Latest is newer. Firmware update will be triggered");
                }

                // Fish out the binary url
                JsonObject assets_0 = doc["assets"][0];
                const char* downloadURL = assets_0["browser_download_url"];
                Serial.printf("[FIRMWARE UPDATES] Download URL is %s\n",downloadURL);

                 // Perform the update
                processOTAUpdate(downloadURL);
                
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

void FirmwareUpdates::processOTAUpdate(const char* url)
{
    WiFiClientSecure client;
    if (!client) {
        Serial.println("[FIRMWARE UPDATES] failed to create network client...");
        return;
    }

    HTTPClient https;

    String firmwareUrlString = String(url);
    if (!firmwareUrlString.endsWith(".bin")) {
        Serial.println("Unsupported binary format. OTA update cannot be performed!");
        return;
    }

    if (https.begin(client, url)) {  // HTTPS
        Serial.println("[FIRMWARE UPDATES] Starting firmware load");
        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode <= 0) {
            Serial.println("[FIRMWARE UPDATES] Error from httpclient");
            return;
        }

        // HTTP header has been sent and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // https.collectHeaders(["Location"],1); << is it needed?

        // Handle redirects
        if (httpCode >= 300 && httpCode < 400 ){
            if (https.hasHeader("Location")) {
                String redirectLocation = https.header("Location");
                Serial.println("[FIRMWARE UPDATES] Redirect: " + redirectLocation);
                processOTAUpdate(redirectLocation.c_str()); // should we limit redirects? we should not recreate another wificlient?
                return;
            } else {
                Serial.println("[FIRMWARE UPDATES] 3xx status code but no redirect location");
                return;
            }
        }

        // Handle other non-200 status codes
        if (httpCode != 200){
            Serial.println("[FIRMWARE UPDATES] Bailing out due to non-200 status code");
            return;
        }
        
        volatile int contentLength = 0;

        if (https.hasHeader("Content-Length")){
            contentLength = atoi(https.header("Content-Length").c_str());
            Serial.printf("[FIRMWARE UPDATES] Content length is %i",contentLength);
        } else {
            Serial.println("[FIRMWARE UPDATES] No content length found");
            return;
        }

        volatile bool isValidContentType = false;
        if (https.hasHeader("Content-Type")){
            String contentType = https.header("Content-Type");
            if ( contentType == "application/octet-stream"){
                isValidContentType = true;
            }
            Serial.println("[FIRMWARE UPDATES] Content type is " + contentType);
        } else {
            Serial.println("[FIRMWARE UPDATES] No content type found");
            return;
        }

        // WE ARE GETTING SOMEWHERE

        // check whether we have everything for OTA update
        if (contentLength && isValidContentType)
        {
            if (Update.begin(contentLength))
            {
            Serial.println("Starting Over-The-Air update. This may take some time to complete ...");
            size_t written = Update.writeStream(client);

            if (written == contentLength)
            {
                Serial.println("Written : " + String(written) + " successfully");
            }
            else
            {
                Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
                // Retry??
            }

            if (Update.end())
            {
                if (Update.isFinished())
                {
                Serial.println("OTA update has successfully completed. Rebooting ...");
                ESP.restart();
                }
                else
                {
                Serial.println("Something went wrong! OTA update hasn't been finished properly.");
                }
            }
            else
            {
                Serial.println("An error Occurred. Error #: " + String(Update.getError()));
            }
            }
            else
            {
            Serial.println("There isn't enough space to start OTA update");
            client.flush();
            }
        }
        else
        {
            Serial.println("There was no valid content in the response from the OTA server!");
            client.flush();
        }

            
    }

}
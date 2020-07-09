#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

#include <ArduinoJson.h>

#include "FirmwareUpdates.h"
#include <semver.h>
#include "settings.h"

#include "esp_log.h"
#include <SpiRamJsonAllocator.h>
#include "../Loggery.h"

#include <Preferences.h>

#define TAG "FIRMWARE";

#define MAX_REDIRECT_DEPTH 5

FirmwareUpdates::FirmwareUpdates(QueueHandle_t firmwareUpdateQueue) {
    _firmwareUpdateQueue = firmwareUpdateQueue;
};


  // Firmware updates

bool FirmwareUpdates::performUpdate(){
    LOGMEM;
    
    FirmwareUpdateStatus fwUpdateStatus = idle;

    Preferences preferences = Preferences();
    preferences.begin("clocklet", true);
    bool staging = preferences.getBool("staging",false);
    if (checkForUpdates(staging)){
        if (updateAvailable){
            fwUpdateStatus = updating;
            xQueueSend(_firmwareUpdateQueue, &fwUpdateStatus,(TickType_t)0 );
            if (!startUpdate()){
                fwUpdateStatus = failed;
                xQueueSend(_firmwareUpdateQueue, &fwUpdateStatus,(TickType_t)0 );
            }
        }
        preferences.end();
        ESP_LOGI(TAG,"Firmware update check complete");
        return true;
    } else {
        preferences.end();
        ESP_LOGE(TAG,"Update check failed");
        return false;
    }

}


// Pass it a url... it will perform a get, process redirects
bool FirmwareUpdates::_getWithRedirects(HTTPClient ** httpsptr, WiFiClientSecure ** clientptr, const char* url, int depth){

    if (depth > MAX_REDIRECT_DEPTH){
        ESP_LOGE(TAG, "Maximum redirects exceeded");
        return false;
    }
    
    ESP_LOGV(TAG, "IN: httpsptr %p ... clientptr %p",httpsptr,clientptr);
    ESP_LOGV(TAG, "deref: httpsptr %p ... clientptr %p",*httpsptr,*clientptr);

    WiFiClientSecure *client = new WiFiClientSecure();
    HTTPClient *https = new HTTPClient();

    *clientptr = client;
    *httpsptr = https;

    ESP_LOGV(TAG, "created: https %p ... client %p",https,client);
    ESP_LOGV(TAG, "assigned: httpsptr %p ... clientptr %p",httpsptr,clientptr);
    ESP_LOGV(TAG, "deref: httpsptr %p ... clientptr %p",*httpsptr,*clientptr);

    ESP_LOGV(TAG, "BEGIN");
    // WiFiClient& upcast = &*client;
    if (!https->begin(*client, url)) {
        ESP_LOGE(TAG, "Could not begin HTTPS request to: %s",url);
        delete client;
        delete https;
        return false;
    }

    const char * headerKeys[] = {"Location","Content-Length","Content-Type"};
    https->collectHeaders(headerKeys,3);
    ESP_LOGV(TAG, "Get");
    int httpCode = https->GET();


    // httpCode will be negative on error
    if (httpCode < 0) {
        ESP_LOGE(TAG, "HTTP error: %s\n-- URL: %s", https->errorToString(httpCode).c_str(), url);
        delete client;
        delete https;
        return false;
    }

    ESP_LOGV(TAG, "nonNegative status");

    ESP_LOGV(TAG, "Redirects");
    // Handle redirects
    if (httpCode >= 300 && httpCode < 400 ){
        if (https->hasHeader("Location")) {
            String redirectLocation = https->header("Location");
            
            ESP_LOGV(TAG, "Flushola");
            https->end();
            client->flush();

            delete client;
            delete https;
        
            ESP_LOGI(TAG, "Redirecting: %s", redirectLocation.c_str());
            return _getWithRedirects(&*httpsptr, &*clientptr, redirectLocation.c_str(), depth++);
        } else {
            ESP_LOGE(TAG, "%d status code but no redirect location", httpCode);
            delete client;
            delete https;
            return false;
        }
    }
    ESP_LOGV(TAG, "TWO HUNDRED");
    // Handle other non-200 status codes
    if (httpCode != 200){
        ESP_LOGE(TAG, "Bailing out due to %d status code", httpCode);
        delete client;
        delete https;
        return false;
    }

    return true;

}

bool FirmwareUpdates::checkForUpdates(bool useStaging) {
    
    ESP_LOGI(TAG, "Checking for firmware updates");

    updateAvailable = false;

    // Fetch latest release data from github
    // GET /repos/:owner/:repo/releases/latest
    HTTPClient* https = nullptr;
    WiFiClientSecure* client = nullptr;
    ESP_LOGV(TAG, "About to GET! %p ... %p",https,client);

    const char* url;
    size_t capacity;

    if (useStaging){
        url = "https://api.github.com/repos/samscam/priscilla/releases";
        capacity = 5*JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(5) + 5*JSON_OBJECT_SIZE(13) + 15*JSON_OBJECT_SIZE(18) + 30720; // 30k overhead
    } else {
        url = "https://api.github.com/repos/samscam/priscilla/releases/latest";
        capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(13) + 3*JSON_OBJECT_SIZE(18) + 10240; // 10k overhead
    }

    if (!_getWithRedirects(&https, &client, url)){
        return false;
    }

    // Parse JSON object
    #if defined(CLOCKBRAIN)
    SpiRamJsonDocument doc(capacity);
    #else
    DynamicJsonDocument doc(capacity);
    #endif
    
    DeserializationError error = deserializeJson(doc, *client);

    ESP_LOGV(TAG, "TRASHING");
    delete client;
    delete https;

    if (error) {
        ESP_LOGE(TAG, "deserializeJson() failed: %s", error.c_str());
        return false;
    }
    ESP_LOGV(TAG, "FISHING");

    JsonObject rel;

    if (useStaging){
        // take the first staging release
        rel = doc[0];
    } else {
        rel = doc.as<JsonObject>();
    }

    // Fish out verion string
    const char* tag_name = rel["tag_name"];
    
    // Strip the v from the start of the tag - if it doesn't have one, bail out
    int taglen = strlen(tag_name);
    char latestbuf[taglen];
    memcpy(latestbuf, &tag_name[1],taglen);

    semver_t latest = {};
    if (semver_parse(latestbuf, &latest)){
        ESP_LOGE(TAG, "Failed to parse latest version");
        return false;
    }
    ESP_LOGI(TAG, "Latest firmware is %s", latestbuf);

    semver_t local = {};
    if ( semver_parse(VERSION, &local)){
        ESP_LOGE(TAG, "Failed to parse local version");
        return false;
    }
    ESP_LOGI(TAG, "Local firmware is %s", VERSION);

    // Compare that to the local version
    // Bail unless an update is needed
    int comparison = semver_compare(local,latest);
    if (comparison == 0) {
        ESP_LOGI(TAG, "Local firmware is up to date");
        return true;
    } else if (comparison > 0) {
        ESP_LOGI(TAG, "Local firmware is newer than latest - must be developing");
        return true;
    } else {
        ESP_LOGI(TAG, "Latest is newer. Firmware update will be triggered");
    }

    // Fish out the binary url
    if (rel["assets"][0].isNull()){
        ESP_LOGE(TAG, "Assets section not found in payload");
        return false;
    }
    
    // Iterate through the assets until we find something with the right name
    // For clockbrains, that's "clockbrain.bin"
    // For feathers, that's "feather.bin"

    JsonArray assets = rel["assets"];
    JsonObject selectedAsset;
    bool assetFound = false;

    #if defined(CLOCKBRAIN)
    const char* assetName = "clockbrain.bin";
    #else
    const char* assetName = "feather.bin";
    #endif
    
    for(JsonVariant vasset : assets){
        selectedAsset = vasset.as<JsonObject>();
        if (strcmp(selectedAsset["name"],assetName) == 0) {
            assetFound = true;
            break;
        }
    }

    if (!assetFound){
        ESP_LOGE(TAG, "Correct asset for this device not found");
        return false;
    }

    if (selectedAsset["browser_download_url"].isNull()){
        ESP_LOGE(TAG, "Download URL not found in payload");
        return false;
    }
    const char * __downloadURL = selectedAsset["browser_download_url"];

    strcpy(_downloadURL,__downloadURL);
    ESP_LOGI(TAG, "Download URL is %s\n",_downloadURL);
    updateAvailable = true;

    return true;

}

bool FirmwareUpdates::startUpdate(){
    if (updateAvailable && _downloadURL){
        return _processOTAUpdate(_downloadURL);
    }
    return false;
}

bool FirmwareUpdates::_processOTAUpdate(const char * url)
{
    ESP_LOGI(TAG, "About to download from %s",url);
    
    HTTPClient* https = nullptr;
    WiFiClientSecure* client = nullptr;
    
    if (!_getWithRedirects(&https, &client, url)){
        return false;
    }

            
    volatile int contentLength = 0;

    if (https->hasHeader("Content-Length")){
        contentLength = atoi(https->header("Content-Length").c_str());
        ESP_LOGD(TAG, "Content length is %i",contentLength);
    } else {
        ESP_LOGE(TAG, "No content length found");
        delete client;
        delete https;
        return false;
    }

    if (!https->hasHeader("Content-Type")){
        ESP_LOGE(TAG, "No content type found");
        delete client;
        delete https;
        return false;
    }

    String contentType = https->header("Content-Type");
    if (contentType != "application/octet-stream") {
        ESP_LOGE(TAG, "Invalid content type: %s", contentType);
        delete client;
        delete https;
        return false;
    }
    ESP_LOGD(TAG, "Content type is %s", contentType);


    if (!Update.begin(contentLength)) {
        ESP_LOGE(TAG, "OTA could not start update - probably not enough free space");
        delete client;
        delete https;
        return false;
    }

    ESP_LOGI(TAG, "Starting Over-The-Air update. This may take some time to complete ...");
    size_t written = Update.writeStream(*client);

    if (written == contentLength) {
        ESP_LOGI(TAG, "Written : %d successfully", written);
    } else {
        ESP_LOGE(TAG, "Written only : %d/%d " ,written, contentLength);
        delete client;
        delete https;
        return false;
    }

    if (!Update.end()) {
        ESP_LOGE(TAG, "An error Occurred. Error #: %d", Update.getError());
        delete client;
        delete https;
        return false;
    }

    if (!Update.isFinished()) {
        ESP_LOGE(TAG, "Something went wrong! OTA update hasn't been finished properly.");
        delete client;
        delete https;
        return false;
    }

    ESP_LOGI(TAG, "OTA update has successfully completed. Rebooting ...");
    return true; // But this will never ever happen :)
}

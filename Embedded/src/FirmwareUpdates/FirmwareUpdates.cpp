#include <Update.h>
#include <ArduinoJson.h>

#include "FirmwareUpdates.h"
#include <semver.h>
#include "settings.h"

#include "esp_log.h"
#include <SpiRamJsonAllocator.h>
#include "../Loggery.h"

#include <Preferences.h>

#include "../Utilities/HTTPnihClient.h"

#include "../network.h"

#define TAG "FIRMWARE"

#define MAX_REDIRECT_DEPTH 5

FirmwareUpdates::FirmwareUpdates(QueueHandle_t firmwareUpdateQueue) {
    _firmwareUpdateQueue = firmwareUpdateQueue;
};


  // Firmware updates

bool FirmwareUpdates::performUpdate(){
    LOGMEM;
    
    FirmwareUpdateStatus fwUpdateStatus = idle;

    Preferences *preferences = new Preferences();
    preferences->begin("clocklet", true);
    bool staging = preferences->getBool("staging",false);
    bool enabled = preferences->getBool("autoUpdates",true);
    preferences->end();
    delete preferences;

    if (!enabled){
        ESP_LOGI(TAG,"Firmware updates are disabled");
        return true;
    }

    if (checkForUpdates(staging)){
        if (updateAvailable){
            fwUpdateStatus = updating;
            xQueueSend(_firmwareUpdateQueue, &fwUpdateStatus,(TickType_t)0 );
            if (startUpdate()){
                fwUpdateStatus = complete;
                xQueueSend(_firmwareUpdateQueue, &fwUpdateStatus,(TickType_t)0 );
            } else {
                fwUpdateStatus = failed;
                xQueueSend(_firmwareUpdateQueue, &fwUpdateStatus,(TickType_t)0 );
            }
        }
        
        ESP_LOGI(TAG,"Firmware update check complete");
        return true;
    } else {
        ESP_LOGE(TAG,"Update check failed, should retry");
        return false;
    }

}

// should return TRUE if we do not want to retry...
// otherwise false...
bool FirmwareUpdates::checkForUpdates(bool useStaging) {
    
    if (!reconnect()){
        return false;
    }

    ESP_LOGI(TAG, "Checking for firmware updates");

    updateAvailable = false;

    // Fetch latest release data from github
    // GET /repos/:owner/:repo/releases/latest

    const char *url;
    if (useStaging){
        url = "https://api.github.com/repos/samscam/clocklet/releases?page=1&per_page=1";
    } else {
        url = "https://api.github.com/repos/samscam/clocklet/releases/latest";
    }

    HTTPnihClient nihClient;
    Stream* stream = nullptr;

    bool parseResult = false;

    int result = nihClient.get(url,&stream);
    if (result == 200){
        if (stream){
            parseResult = _parseGithubReleases(stream,useStaging);
        }
    }

    return parseResult;
}


// should return TRUE if we do not want to retry...
// otherwise false...
bool FirmwareUpdates::_parseGithubReleases(Stream *stream, bool useStaging){
    // Parse JSON object

    #if defined(SPI_RAM)
    SpiRamJsonDocument doc(64*1024);
    #else
    DynamicJsonDocument doc(8*1024);
    #endif
    
    DeserializationError error = deserializeJson(doc, *stream);

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
        return true;
    }

    ESP_LOGI(TAG, "Local firmware is %s", VERSION);

    // Compare that to the local version
    // Bail unless an update is needed
    #ifndef TEST_FORCE_UPDATE
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
    #endif

    // Fish out the binary url
    if (rel["assets"][0].isNull()){
        ESP_LOGE(TAG, "Assets section not found in payload");
        return true;
    }
    
    // Iterate through the assets until we find something with the right name
    // For clockbrains, that's "clockbrain.bin"
    // For feathers, that's "feather.bin"

    JsonArray assets = rel["assets"];
    JsonObject selectedAsset;
    bool assetFound = false;

    #if defined(CLOCKBRAIN)
    const char* assetName = "clockbrain.bin";
    #elif defined(FEATHER)
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
        return true;
    }

    if (selectedAsset["browser_download_url"].isNull()){
        ESP_LOGE(TAG, "Download URL not found in payload");
        return true;
    }
    const char * __downloadURL = selectedAsset["browser_download_url"];

    strcpy(_downloadURL,__downloadURL);
    ESP_LOGI(TAG, "Download URL is %s\n",_downloadURL);
    updateAvailable = true;

    return true;

}

bool FirmwareUpdates::startUpdate(){
    if (updateAvailable && _downloadURL){
        return _downloadOTAUpdate(_downloadURL);
    }
    return false;
}

bool FirmwareUpdates::_downloadOTAUpdate(const char * url)
{
    ESP_LOGI(TAG, "About to download from %s",url);
    
    HTTPnihClient nihClient;
    Stream* stream = nullptr;
    
    
    int result = nihClient.get(url,&stream);

    if (result != 200) {
        return false;
        
    }


    HTTPClient *httpClient = nihClient.getHttpClient();

            
    volatile int contentLength = 0;

    if (httpClient->hasHeader("Content-Length")){
        contentLength = atoi(httpClient->header("Content-Length").c_str());
        ESP_LOGD(TAG, "Content length is %i",contentLength);
    } else {
        ESP_LOGE(TAG, "No content length found");
        return false;
    }

    if (!httpClient->hasHeader("Content-Type")){
        ESP_LOGE(TAG, "No content type found");
        return false;
    }

    String contentType = httpClient->header("Content-Type");
    if (contentType != "application/octet-stream") {
        ESP_LOGE(TAG, "Invalid content type: %s", contentType.c_str());
        return false;
    }
    ESP_LOGD(TAG, "Content type is %s", contentType.c_str());


    if (!Update.begin(contentLength)) {
        ESP_LOGE(TAG, "OTA could not start update - probably not enough free space");
        return false;
    }

    ESP_LOGI(TAG, "Starting Over-The-Air update. This may take some time to complete ...");

    disableCore0WDT();
    size_t written = Update.writeStream(*stream);
    enableCore0WDT();

    if (written == contentLength) {
        ESP_LOGI(TAG, "Written : %d successfully", written);
    } else {
        ESP_LOGE(TAG, "Written only : %d/%d " ,written, contentLength);
        return false;
    }

    if (!Update.end()) {
        ESP_LOGE(TAG, "An error Occurred. Error #: %d", Update.getError());
        return false;
    }

    if (!Update.isFinished()) {
        ESP_LOGE(TAG, "Something went wrong! OTA update hasn't been finished properly.");
        return false;
    }

    ESP_LOGI(TAG, "OTA update has successfully completed. Rebooting ...");
    return true;
}

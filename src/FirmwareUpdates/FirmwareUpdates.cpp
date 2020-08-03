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

#define TAG "FIRMWARE";

#define MAX_REDIRECT_DEPTH 5

const char* digicert_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n" \
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n" \
"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n" \
"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n" \
"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n" \
"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n" \
"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n" \
"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n" \
"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n" \
"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n" \
"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n" \
"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n" \
"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n" \
"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n" \
"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n" \
"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n" \
"+OkuE6N36B9K\n" \
"-----END CERTIFICATE-----";

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
    bool enabled = preferences.getBool("autoUpdates",true);
    preferences.end();

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
        ESP_LOGE(TAG,"Update check failed");
        return false;
    }

}


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
        url = "https://api.github.com/repos/samscam/clocklet/releases";
    } else {
        url = "https://api.github.com/repos/samscam/clocklet/releases/latest";
    }

    HTTPnihClient nihClient = HTTPnihClient();
    Stream* stream = nullptr;

    bool parseResult = false;

    int result = nihClient.get(url,NULL,&stream);
    if (result == 200){
        if (stream){
            parseResult = _parseGithubReleases(stream,useStaging);
        }
    }

    return parseResult;
}



bool FirmwareUpdates::_parseGithubReleases(Stream *stream, bool useStaging){
    // Parse JSON object



    #if defined(CLOCKBRAIN)
    SpiRamJsonDocument doc(512*1024);
    #else
    size_t capacity = 64*1024;
    DynamicJsonDocument doc(capacity);
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
        return _downloadOTAUpdate(_downloadURL);
    }
    return false;
}

bool FirmwareUpdates::_downloadOTAUpdate(const char * url)
{
    ESP_LOGI(TAG, "About to download from %s",url);
    
    HTTPnihClient nihClient = HTTPnihClient();
    Stream* stream = nullptr;
    
    
    int result = nihClient.get(url,NULL,&stream);

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

#include "HTTPnihClient.h"
#include "esp_log.h"

#define TAG "HTTPNIH"
#define MAX_REDIRECT_DEPTH 10

extern const uint8_t rootca_crt_bundle_start[] asm("_binary_data_cert_x509_crt_bundle_bin_start");

HTTPnihClient::HTTPnihClient(){
    ESP_LOGV(TAG,"Starting HTTPnihClient");
    _httpClient = new HTTPClient();
    _httpClient->setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    _httpClient->setRedirectLimit(MAX_REDIRECT_DEPTH);
    _wifiClient = new WiFiClientSecure();
    _wifiClient->setCACertBundle(rootca_crt_bundle_start);


}

HTTPnihClient::~HTTPnihClient(){
    ESP_LOGV(TAG,"Removing HTTPnihClient");
    delete _httpClient;
    delete _wifiClient;
}

HTTPClient* HTTPnihClient::getHttpClient(){
    return _httpClient;
}

// Pass it a url... it will perform a get, process redirects, return the eventual status code or an error
// Pass it an empty stream pointer to process the stream when it has been through the headers
// Delete the client when done with the stream or if it errors

int HTTPnihClient::get(const char *url, const char *certificate, Stream **stream, int depth){

    if (depth > MAX_REDIRECT_DEPTH){
        ESP_LOGE(TAG, "Maximum redirects exceeded");
        return HTTPNIH_TOO_MANY_REDIRECTS;
    }

    if (certificate != NULL){
        _wifiClient->setCACert(certificate);
    }
    
    ESP_LOGV(TAG, "BEGIN");

    if (!_httpClient->begin(*_wifiClient, url)) {
        ESP_LOGE(TAG, "Could not begin HTTPS request to: %s",url);
        return HTTPNIH_COULD_NOT_BEGIN;
    }

    const char * headerKeys[] = {"Location","Content-Length","Content-Type"};
    _httpClient->collectHeaders(headerKeys,3);
    ESP_LOGV(TAG, "Get");
    int httpCode = _httpClient->GET();


    // httpCode will be negative on error
    if (httpCode < 0) {
        ESP_LOGE(TAG, "HTTP error: %s\n-- URL: %s", _httpClient->errorToString(httpCode).c_str(), url);
        return httpCode;
    }

    ESP_LOGV(TAG, "nonNegative status");

    ESP_LOGV(TAG, "Redirects");
    // Handle redirects
    if (httpCode >= 300 && httpCode < 400 ){
        if (_httpClient->hasHeader("Location")) {
            String redirectLocation = _httpClient->header("Location");
            _httpClient->end();
            delete _wifiClient;
            _wifiClient = new WiFiClientSecure();
            ESP_LOGI(TAG, "Redirecting %d: %s",httpCode, redirectLocation.c_str());
            return get(redirectLocation.c_str(), certificate, &*stream, depth++);
        } else {
            ESP_LOGE(TAG, "%d status code but no redirect location", httpCode);

            return HTTPNIH_NO_REDIRECT_DESTINATION;
        }
    }
    ESP_LOGV(TAG, "TWO HUNDRED");
    // Handle other non-200 status codes
    if (httpCode != 200){
        ESP_LOGE(TAG, "Bailing out due to %d status code", httpCode);
        return httpCode;
    }

    *stream = _wifiClient;
    return 200;

}
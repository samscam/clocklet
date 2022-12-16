#pragma once

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#define HTTPNIH_TOO_MANY_REDIRECTS -100
#define HTTPNIH_COULD_NOT_BEGIN -101
#define HTTPNIH_NO_REDIRECT_DESTINATION -102

class HTTPnihClient {
    public:
        HTTPnihClient();
        ~HTTPnihClient();

        int get(const char* url, Stream **stream, int depth = 0);
        HTTPClient* getHttpClient();
    private:

        HTTPClient *_httpClient;
        WiFiClientSecure *_wifiClient;
};
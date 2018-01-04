#ifndef PRISCILLA_DARKSKY
#define PRISCILLA_DARKSKY

#include <Arduino.h>
#include <WiFi101.h>
#include <ArduinoJson.h>
#include "weather.h"

#define DARKSKY_APIKEY "***REMOVED***"
#define DARKSKY_SERVER "api.darksky.net"
#define DARKSKY_PATH "/forecast/***REMOVED***/***REMOVED***?exclude=currently,minutely,hourly,alerts,flags&units=si"

#define DARKSKY_HTTP_TIMEOUT 10000  // max respone time from server
#define DARKSKY_MAX_CONTENT_SIZE 16384       // max size of the HTTP response


class DarkSky{

private:
  bool connect(const char* hostName);

  void disconnect();


  WiFiClient *client;

  bool sendRequest(const char* host, const char* resource);
  bool skipResponseHeaders();
  weather readReponseContent();

public:
  DarkSky(WiFiClient &client);
  void fetchWeather();
  weather latestWeather;

};


#endif

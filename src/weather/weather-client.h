#ifndef PRISCILLA_WEATHERCLIENT
#define PRISCILLA_WEATHERCLIENT

#include <Arduino.h>
#include <WiFi101.h>
#include <ArduinoJson.h>
#include "weather.h"

#define WEATHER_HTTP_TIMEOUT 10000  // max respone time from server
#define WEATHER_MAX_CONTENT_SIZE 16384       // max size of the HTTP response

class WeatherClient {

public:

  WeatherClient(WiFiClient &client);
  void fetchWeather();
  Weather latestWeather;

  bool connect(char* host, bool ssl);
  void disconnect();

  char* server;
  char* resource;
  bool ssl;

  WiFiClient *client;

  bool sendRequest(char* host, char* resource);
  bool skipResponseHeaders();
  virtual Weather readReponseContent();

};


#endif

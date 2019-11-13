#ifndef PRISCILLA_WEATHERCLIENT
#define PRISCILLA_WEATHERCLIENT

#include <Arduino.h>
#include "network.h"
#include <ArduinoJson.h>
#include "weather.h"
#include "../Location/LocationSource.h"
#include <RTClib.h>
#define WEATHER_HTTP_TIMEOUT 20  // max respone time from server
#define WEATHER_MAX_CONTENT_SIZE 16384       // max size of the HTTP response

class WeatherClient {

public:

  WeatherClient(WiFiClient &client);

  // This tells the client to go and update the weather from the remote source
  bool fetchWeather();

  Weather horizonWeather;
  Weather rainbowWeather;

  // Setters - subclasses can override
  virtual void setLocation(Location location) {};
  virtual void setTimeHorizon(uint8_t hours) {};

  bool connect(char* host, bool ssl);
  void disconnect();

  char* server;
  char* resource;
  bool ssl;

  WiFiClient *client;

  bool sendRequest(char* host, char* resource);
  bool skipResponseHeaders();
  
  virtual bool readReponseContent() { return false; };
  
};


#endif

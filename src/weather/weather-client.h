#ifndef PRISCILLA_WEATHERCLIENT
#define PRISCILLA_WEATHERCLIENT

#include <Arduino.h>
#include "network.h"
#include <ArduinoJson.h>
#include "weather.h"
#include "../Location/LocationSource.h"
#include "../UpdateScheduler.h"

#include <RTClib.h>
#define WEATHER_HTTP_TIMEOUT 20  // max respone time from server
#define WEATHER_MAX_CONTENT_SIZE 16384       // max size of the HTTP response

class WeatherClient: public UpdateJob {

public:
  
  QueueHandle_t weatherChangedQueue;

  WeatherClient(WiFiClient &client);
  virtual ~WeatherClient() {};

  // Conformance to UpdateJob
  bool performUpdate();

  // This tells the client to go and update the weather from the remote source
  bool fetchWeather();

  // HorizonWeather is the forecast for the time up to the set timeHorizon
  Weather horizonWeather;

  // RainbowWeather is the CURRENT weather (or for the next hour)
  Weather rainbowWeather;

  // Setters - subclasses can override
  virtual void setLocation(Location location) {};
  virtual void setTimeHorizon(uint8_t hours) {};

  Location _currentLocation;
  
  bool connect(char* host, bool ssl);
  void disconnect();

  char* server;
  char* resource;
  bool ssl;


  WiFiClient *client;

  bool sendRequest(char* host, char* resource);
  bool skipResponseHeaders();
  
  virtual bool readReponseContent() { return false; };
  
private:

};


#endif

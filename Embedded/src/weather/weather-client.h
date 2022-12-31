#pragma once

#include <ArduinoJson.h>

#include "network.h"
#include "weather.h"
#include "../Utilities/HTTPnihClient.h"
#include "../Location/LocationSource.h"
#include "../UpdateScheduler.h"

#include <RTClib.h>

class WeatherClient: public UpdateJob {

public:
  
  QueueHandle_t weatherChangedQueue;

  WeatherClient();
  ~WeatherClient();

  // Conformance to UpdateJob
  virtual bool performUpdate();

  // This tells the client to go and update the weather from the remote source
  bool fetchWeather();

  // HorizonWeather is the forecast for the time up to the set timeHorizon
  Weather horizonWeather;

  // RainbowWeather is the CURRENT weather (or for the next hour)
  Weather rainbowWeather;

  // Setters - subclasses can override
  virtual void setLocation(ClockLocation location) {};
  virtual void setTimeHorizon(uint8_t hours) {};

  ClockLocation _currentLocation;

  
  virtual char* constructURL() { return nullptr; };

  virtual bool readReponseContent(Stream *stream) { return false; };
  
private:


};

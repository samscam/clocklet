#ifndef PRISCILLA_METOFFICE
#define PRISCILLA_METOFFICE

#include <Arduino.h>

#include <WiFi101.h>
#include <ArduinoJson.h>

struct weather {
  int type;
  int precip;
  int maxTmp;
  int minTmp;
};

// ---------- WEATHER

// function declarations
weather fetchWeather();
bool connect(const char* hostName);
bool sendRequest(const char* host, const char* resource);
bool skipResponseHeaders();
weather readReponseContent();
void disconnect();


#endif

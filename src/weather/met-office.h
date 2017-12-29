#ifndef PRISCILLA_METOFFICE
#define PRISCILLA_METOFFICE

#include <Arduino.h>

#include <WiFi101.h>
#include "colours.h"
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

// constants

const char server[] = "datapoint.metoffice.gov.uk";
const char resource[] = "/public/data/val/wxfcs/all/json/351207?res=daily&key=***REMOVED***"; // http resource
const unsigned long HTTP_TIMEOUT = 10000;  // max respone time from server
const size_t MAX_CONTENT_SIZE = 4096;       // max size of the HTTP response


#endif

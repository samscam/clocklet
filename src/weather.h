#ifndef PRISCILLA_WEATHER
#define PRISCILLA_WEATHER

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


// ---------- WEATHER TYPE COLOUR PATTERNS

const Colour weatherTypeColours[][5] = {
  {DARK_BLUE,DARK_BLUE,DARK_BLUE,DARK_BLUE,DARK_GRAY}, // 0 - clear night
  {YELLOW,YELLOW,YELLOW,YELLOW,WHITE},             // 1 - sunny day
  {DARK_BLUE,DARK_GRAY,DARK_BLUE,DARK_GRAY,DARK_GRAY},           // 2 - Partly cloudy night
  {YELLOW,GRAY,YELLOW,GRAY,WHITE},                 // 3 - Partly cloudy day
  {WHITE,WHITE,WHITE,WHITE,WHITE},                 // 4 - unused
  {WHITE,GRAY,WHITE,GRAY,WHITE},                 // 5 - mist
  {BROWN,BROWN,BROWN,BROWN,WHITE},                 // 6 - fog
  {GRAY,BROWN,GRAY,BROWN,WHITE},                 // 7 - cloudy
  {BROWN,BROWN,BROWN,BROWN,WHITE},                 // 8 - overcast
  {BLUE,DARK_BLUE,BLUE,DARK_BLUE,DARK_GRAY},                 // 9 - light rain shower (night)
  {RED,ORANGE,GREEN,BLUE,WHITE},        // 10 - light rain shower (day)
  {LIGHT_BLUE,LIGHT_BLUE,LIGHT_BLUE,LIGHT_BLUE,WHITE},  // 11 - Drizzle
  {LIGHT_BLUE,LIGHT_BLUE,LIGHT_BLUE,LIGHT_BLUE,WHITE}, // 12 - Light rain
  {DARK_BLUE,BLUE,DARK_BLUE,BLUE,DARK_GRAY}, // 13 - Heavy rain shower (night)
  {YELLOW,BLUE,YELLOW,BLUE,WHITE},// 14 - Heavy rain shower (day)
  {BLUE,BLUE,BLUE,BLUE,WHITE},// 15 - Heavy rain
  {DARK_BLUE,WHITE,DARK_BLUE,WHITE,DARK_GRAY},// 16 - Sleet shower (night)
  {YELLOW,WHITE,YELLOW,WHITE,WHITE},// 17 - Sleet shower (day)
  {WHITE,WHITE,WHITE,WHITE,WHITE},// 18 - Sleet
  {DARK_BLUE,WHITE,DARK_BLUE,WHITE,DARK_GRAY},// 19 - Hail shower (night)
  {YELLOW,WHITE,YELLOW,WHITE,WHITE},// 20 - Hail shower (day)
  {WHITE,WHITE,WHITE,WHITE,WHITE},// 21 - Hail
  {DARK_BLUE,WHITE,DARK_BLUE,WHITE,DARK_GRAY},// 22 - Light snow shower (night)
  {YELLOW,WHITE,YELLOW,WHITE,WHITE},// 23 - Light snow shower (day)
  {WHITE,WHITE,WHITE,WHITE,WHITE},// 24 - Light snow
  {DARK_BLUE,WHITE,DARK_BLUE,WHITE,DARK_GRAY},// 25 - Heavy snow shower (night)
  {WHITE,YELLOW,WHITE,WHITE,WHITE},// 26 - Heavy snow shower (day)
  {WHITE,WHITE,WHITE,WHITE,WHITE},// 27 - Heavy snow
  {DARK_BLUE,ORANGE,BLUE,DARK_BLUE,DARK_GRAY},// 28 - Thunder shower (night)
  {BLUE,DARK_GRAY,BLUE,ORANGE,WHITE},// 29 - Thunder shower (day)
  {DARK_GRAY,BLUE,ORANGE,YELLOW,WHITE},// 30 - Thunder
  {RED,ORANGE,GREEN,BLUE,WHITE}, // 31 - rainbow
};



#endif

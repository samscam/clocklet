#include "met-office.h"
#include <SpiRamJsonAllocator.h>

MetOffice::MetOffice(WiFiClient &client) : WeatherClient(client)  {
  this->client = &client;
  this->server = (char *)METOFFICE_SERVER;
  char * resource;
  asprintf(&resource, (char *)METOFFICE_PATH, (char *)METOFFICE_SITE, (char *)METOFFICE_APIKEY);
  this->resource = resource;
  this->ssl = false;
  this->timeThreshold = 0;
};


const char* weatherTypes[] = {
  "Clear night", // 0 - clear night
  "Sunny day",             // 1 - sunny day
  "Partly cloudy night",           // 2 - Partly cloudy night
  "Partly cloudy",                 // 3 - Partly cloudy day
  "Random",                 // 4 - unused
  "Misty",                 // 5 - mist
  "Foggy",                 // 6 - fog
  "Cloudy",                // 7 - cloudy
  "Overcast",                 // 8 - overcast
  "Light showers",                 // 9 - light rain shower (night)
  "Light showers",        // 10 - light rain shower (day)
  "Drizzle",  // 11 - Drizzle
  "Light rain", // 12 - Light rain
  "Heavy showers", // 13 - Heavy rain shower (night)
  "Heavy showers",// 14 - Heavy rain shower (day)
  "Heavy rain",// 15 - Heavy rain
  "Sleet showers",// 16 - Sleet shower (night)
  "Sleet showers",// 17 - Sleet shower (day)
  "Sleet",// 18 - Sleet
  "Hail showers",// 19 - Hail shower (night)
  "Hail showers",// 20 - Hail shower (day)
  "Hail",// 21 - Hail
  "Light snow showers",// 22 - Light snow shower (night)
  "Light snow showers",// 23 - Light snow shower (day)
  "Light snow",// 24 - Light snow
  "Heavy snow shower",// 25 - Heavy snow shower (night)
  "Heavy snow shower",// 26 - Heavy snow shower (day)
  "Heavy snow",// 27 - Heavy snow
  "Thunder shower",// 28 - Thunder shower (night)
  "Thunder shower",// 29 - Thunder shower (day)
  "Thunder",// 30 - Thunder
  "Rainbows",
};


bool MetOffice::readReponseContent() {
  Serial.println("Reading metoffice content");

  // Allocate a temporary memory pool
  SpiRamJsonDocument root(METOFFICE_MAX_CONTENT_SIZE);
  auto error = deserializeJson(root,*client);


  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return false;
  }

  Weather result = {0};
  result.minTmp = 100.0; // Max out the minimum temperature

  // This for 3hourly time periods

  // We fish out the next n reps from the payload
  static int repsToMunge = 4; // Should be 12 hour window starting a bit in the past...

  // We may be interested in overlapping days - unwrap them into a big long array
  JsonArray periods = root["SiteRep"]["DV"]["Location"]["Period"];
  Serial.println("metoffice MUNGE");

  SpiRamJsonDocument flatRepsDoc = SpiRamJsonDocument(2048);
  JsonArray flatReps = flatRepsDoc.createNestedArray();
  int i=0;
  bool stop = false;
  bool firstDay = true;

  for (JsonObject period : periods) {
    JsonArray reps = period["Rep"];
    for (JsonObject rep : reps){
      if (firstDay && rep["$"] >= timeThreshold) {
        flatReps.add(rep);
        i++;
      }
      if (i == repsToMunge){
        stop = true;
        break;
      }

    }
    if (stop == true) {
      break;
    }
    firstDay = false;
  }

  Serial.println("metoffice MERGE");

  // Then we merge them down picking the worst case weather...
  for (JsonObject rep : flatReps){
    // serializeJsonPretty(rep,Serial);
    int type = rep["W"];
    if (type > result.type) {
      result.type = type;
      result.summary = weatherTypes[type];
    }

    float precipChance = rep["Pp"].as<float>() / 100.0 ; // as a float between 0 and 1
    result.precipChance =  precipChance > result.precipChance ? precipChance : result.precipChance;

    float temp = rep["T"];
    result.maxTmp = temp > result.maxTmp ? temp : result.maxTmp;
    result.minTmp = temp < result.minTmp ? temp : result.minTmp;

    float windSpeed = rep["S"].as<float>() * 0.44704; // in m/s
    result.windSpeed = windSpeed > result.windSpeed ? windSpeed : result.windSpeed;
  }

  result.precipType = Rain;
  if (result.type >= 16 && result.type <= 21) {
    result.precipType = Sleet;
  } else if (result.type >= 22 && result.type <= 27) {
    result.precipType = Snow;
  }

  if (result.type >= 28 && result.type <= 30) {
    result.thunder = true;
  }
  horizonWeather = result;
  return true;
}

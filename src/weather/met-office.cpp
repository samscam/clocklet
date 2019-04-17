#include "met-office.h"

MetOffice::MetOffice(WiFiClient &client) : WeatherClient(client)  {
  this->client = &client;
  this->server = (char *)METOFFICE_SERVER;
  this->resource = (char *)METOFFICE_PATH;
  this->ssl = false;
  this->timeThreshold = 0;
};


const char* weatherTypes[] = {
  "clear night", // 0 - clear night
  "sunny day",             // 1 - sunny day
  "partly cloudy night",           // 2 - Partly cloudy night
  "partly cloudy",                 // 3 - Partly cloudy day
  "random",                 // 4 - unused
  "misty",                 // 5 - mist
  "foggy",                 // 6 - fog
  "cloudy",                // 7 - cloudy
  "overcast",                 // 8 - overcast
  "light showers",                 // 9 - light rain shower (night)
  "light showers",        // 10 - light rain shower (day)
  "drizzle",  // 11 - Drizzle
  "light rain", // 12 - Light rain
  "heavy showers", // 13 - Heavy rain shower (night)
  "heavy showers",// 14 - Heavy rain shower (day)
  "heavy rain",// 15 - Heavy rain
  "sleet showers",// 16 - Sleet shower (night)
  "sleet showers",// 17 - Sleet shower (day)
  "sleet",// 18 - Sleet
  "hail shower",// 19 - Hail shower (night)
  "hail shower",// 20 - Hail shower (day)
  "hail",// 21 - Hail
  "light snow shower",// 22 - Light snow shower (night)
  "light snow shower",// 23 - Light snow shower (day)
  "light snow",// 24 - Light snow
  "heavy snow shower",// 25 - Heavy snow shower (night)
  "heavy snow shower",// 26 - Heavy snow shower (day)
  "heavy snow",// 27 - Heavy snow
  "thunder shower",// 28 - Thunder shower (night)
  "thunder shower",// 29 - Thunder shower (day)
  "thunder",// 30 - Thunder
  "rainbows",
};


Weather MetOffice::readReponseContent() {

  // Allocate a temporary memory pool
  DynamicJsonDocument root(METOFFICE_MAX_CONTENT_SIZE);
  auto error = deserializeJson(root,*client);

  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return latestWeather;
  }

  Weather result = {0};
  result.minTmp = 100.0; // Max out the minimum temperature

  // This for 3hourly time periods

  // We fish out the next n reps from the payload
  static int repsToMunge = 4; // Should be 12 hour window starting a bit in the past...

  // We may be interested in overlapping days - unwrap them into a big long array
  JsonArray periods = root["SiteRep"]["DV"]["Location"]["Period"];

  JsonArray flatReps = root.createNestedArray();
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

  // Then we merge them down picking the worst case weather...
  for (JsonObject rep : flatReps){
    serializeJsonPretty(rep,Serial);
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

  return result;
}

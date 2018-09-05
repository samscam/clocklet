#include "met-office.h"

MetOffice::MetOffice(WiFiClient &client) : WeatherClient(client)  {
  this->client = &client;
  this->server = METOFFICE_SERVER;
  this->resource = METOFFICE_PATH;
  this->ssl = false;
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


weather MetOffice::readReponseContent() {

  // Allocate a temporary memory pool
  DynamicJsonBuffer jsonBuffer(METOFFICE_MAX_CONTENT_SIZE);

  JsonObject& root = jsonBuffer.parseObject(*client);

  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return latestWeather;
  }
  weather result;
  int rawType = root["SiteRep"]["DV"]["Location"]["Period"][0]["Rep"][0]["W"];
  result.type = weatherTypes[rawType];
  result.summary = weatherTypes[rawType];
  result.precipChance = root["SiteRep"]["DV"]["Location"]["Period"][0]["Rep"][0]["PPd"];
  result.precipChance = result.precipChance / 100.0;
  result.maxTmp = root["SiteRep"]["DV"]["Location"]["Period"][0]["Rep"][0]["Dm"];
  result.minTmp = root["SiteRep"]["DV"]["Location"]["Period"][0]["Rep"][1]["Nm"];
  result.windSpeed = root["SiteRep"]["DV"]["Location"]["Period"][0]["Rep"][0]["S"];

  result.windSpeed = result.windSpeed * 0.44704; // convert to m/s
  result.precipType = Rain;

  if (rawType >= 16 && rawType <= 21) {
    result.precipType = Sleet;
  } else if (rawType >= 22 && rawType <= 27) {
    result.precipType = Snow;
  }

  if (rawType >= 28 && rawType <= 30) {
    result.thunder = true;
  } else {
    result.thunder = false;
  }

  return result;
}

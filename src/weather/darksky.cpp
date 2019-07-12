#include "darksky.h"

DarkSky::DarkSky(WiFiClient &client) : WeatherClient(client) {
  this->client = &client;
  this->server = (char *)DARKSKY_SERVER;
  this->resource = (char *)DARKSKY_PATH;
  this->ssl = true;
  
  Serial.print("Setting Default Weather");
  this->latestWeather = defaultWeather;
};

Weather DarkSky::readReponseContent() {

  // Allocate a temporary memory pool
  DynamicJsonDocument root(WEATHER_MAX_CONTENT_SIZE);
  auto error = deserializeJson(root,*client);

  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return latestWeather;
  }
  Weather result;

  result.type = 0;// root["daily"]["data"][0]["icon"];
  result.summary = root["daily"]["data"][0]["summary"];
  result.precipChance = root["daily"]["data"][0]["precipProbability"];
  result.precipIntensity = root["daily"]["data"][0]["precipIntensity"];

  const char* precipType = root["daily"]["data"][0]["precipType"];
  if (strcmp(precipType, "rain") == 0 ) {
    result.precipType = Rain;
  } else if (strcmp(precipType, "snow") == 0 ) {
    result.precipType = Snow;
  } else if (strcmp(precipType, "sleet") == 0 ) {
    result.precipType = Sleet;
  }

  result.maxTmp = root["daily"]["data"][0]["temperatureHigh"];
  result.minTmp = root["daily"]["data"][0]["apparentTemperatureLow"];

  result.windSpeed = root["daily"]["data"][0]["windSpeed"];

  result.cloudCover = root["daily"]["data"][0]["cloudCover"];

  result.thunder = false;
  return result;
}

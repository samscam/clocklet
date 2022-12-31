#include "openweathermap.h"
#include <SpiRamJsonAllocator.h>
#include "Loggery.h"

#define TAG "OWM"


OpenWeatherMap::OpenWeatherMap() : WeatherClient() {
  ESP_LOGV(TAG,"Starting OWM");
  horizonWeather = defaultWeather;
  rainbowWeather = defaultWeather;
};

char* OpenWeatherMap::constructURL(){
  char *buffer;
  asprintf(&buffer,OWM_PATH,_currentLocation.lat,_currentLocation.lng,OPENWEATHERMAP_APIKEY);
  ESP_LOGI(TAG, "%s", buffer);
  return buffer;
}


void OpenWeatherMap::setTimeHorizon(uint8_t hours){
  _timeHorizon = hours;
}

void OpenWeatherMap::setLocation(ClockLocation location){
  _currentLocation = location;
  horizonWeather = {};
  rainbowWeather = {};
  bool change = true;
  xQueueSend(weatherChangedQueue, &change, (TickType_t) 0);
  this->setNeedsUpdate();
}

bool OpenWeatherMap::readReponseContent(Stream *stream, size_t contentLength) {
  ESP_LOGI(TAG,"Content length is %d",contentLength);
  // Allocate a temporary memory pool
  #if defined(CLOCKBRAIN)
  SpiRamJsonDocument root(64*1024); // woo a whole 64k
  #else
  DynamicJsonDocument root(32*1024);
  #endif
  
  auto error = deserializeJson(root,*stream);

  if (error) {
    ESP_LOGE(TAG,"deserializeJson() failed because: %s",error.c_str());
    return false;
  }

  ESP_LOGI(TAG,"Weather deserialised... parsing...");

  rainbowWeather = _parseWeatherBlock(root["current"]);

  horizonWeather = defaultWeather;
  for (int i=0 ; i < _timeHorizon ; i++){
    Weather block = _parseWeatherBlock(root["hourly"][i]);
    horizonWeather += block;
  }

  ESP_LOGI(TAG,"Weather parsing done");
  return true;
}

Weather OpenWeatherMap::_parseWeatherBlock(JsonObject block){
  Weather result = defaultWeather;

  result.type = block["weather"][0]["id"];
  // result.synopsis = block["weather"]["description"];


  result.precipChance = block["pop"];

  float rainIntensity = block["rain"]["1h"];
  float snowIntensity = block["snow"]["1h"];

  result.precipIntensity = max(rainIntensity,snowIntensity);
  result.precipType = Rain;

  // THUNDER!!
  if (result.type >= 200 & result.type < 300) {
    result.thunder = true;
    if (result.type >= 230 & result.type < 240) {
      result.precipType = Drizzle;
    }
  }

  // DRIZZLE!!
  if (result.type >= 300 & result.type < 400) {
    result.precipType = Drizzle;
  }

  // SNOW!! (and sleet)
  if (result.type >= 600 & result.type < 700) {
    result.precipType = Snow;
    if (result.type >= 611 & result.type < 620) {
      result.precipType = Sleet;
    }
  }


  // Highs and lows are given on the daily blocks
  // result.maxTmp = block["temperatureHigh"];
  // result.minTmp = block["temperatureLow"];

  result.currentTmp = block["temp"];
  result.windSpeed = block["wind_speed"];
  result.cloudCover = block["clouds"];
  result.pressure = block["pressure"];
  
  return result;
}


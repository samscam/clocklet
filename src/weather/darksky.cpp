#include "darksky.h"

#define TAG "DARKSKY"

DarkSky::DarkSky(WiFiClient &client) : WeatherClient(client) {
  this->client = &client;
  this->server = (char *)DARKSKY_SERVER;
  this->ssl = true;
  
  Serial.print("Setting Default Weather");
  this->horizonWeather = defaultWeather;
  this->rainbowWeather = defaultWeather;
};


void DarkSky::setTimeHorizon(uint8_t hours){
  _timeHorizon = hours;
}

bool DarkSky::readReponseContent() {

  // Allocate a temporary memory pool
  DynamicJsonDocument root(30720);
  auto error = deserializeJson(root,*client);

  if (error) {
    ESP_LOGE(TAG,"deserializeJson() failed with code %s",error);
    return false;
  }

  ESP_LOGD(TAG,"Weather deserialised... parsing...");

  rainbowWeather = _parseWeatherBlock(root["hourly"]["data"][0]);

  horizonWeather = defaultWeather;
  for (int i=0 ; i < _timeHorizon ; i++){
    Weather block = _parseWeatherBlock(root["hourly"]["data"][i]);
    horizonWeather += block;
  }

  ESP_LOGD(TAG,"Weather parsing done");
  return true;
}

void DarkSky::setLocation(Location location){
  this->_currentLocation = location;
  int len = strlen(DARKSKY_PATH) + strlen(DARKSKY_APIKEY) + 15;
  char buffer[len];
  len = sprintf(buffer,DARKSKY_PATH,DARKSKY_APIKEY,location.lat,location.lng);

  this->resource = (char*)malloc( (strlen(buffer)+1) * sizeof(char));
  strcpy(this->resource,buffer);
  ESP_LOGD(TAG,"Weather fetch path %s",this->resource);
  
}

Weather DarkSky::_parseWeatherBlock(JsonObject block){
  Weather result = defaultWeather;

  result.type = 0;// root["daily"]["data"][0]["icon"];
  result.summary = block["summary"];
  result.precipChance = block["precipProbability"];
  result.precipIntensity = block["precipIntensity"];

  const char* precipType = block["precipType"];
  if (precipType){ // it doesn't always include it...
    if (strcmp(precipType, "rain") == 0 ) {
      result.precipType = Rain;
    } else if (strcmp(precipType, "snow") == 0 ) {
      result.precipType = Snow;
    } else if (strcmp(precipType, "sleet") == 0 ) {
      result.precipType = Sleet;
    }
  } else {
    result.precipType = Rain;
  }

  // Highs and lows are given on the daily blocks
  // result.maxTmp = block["temperatureHigh"];
  // result.minTmp = block["temperatureLow"];

  result.currentTmp = block["temperature"];
  result.windSpeed = block["windSpeed"];
  result.cloudCover = block["cloudCover"];
  result.pressure = block["pressure"];

  if (strstr(result.summary,"thunder")){
    result.thunder = true;
  } else {
    result.thunder = false;
  }
  
  
  return result;
}


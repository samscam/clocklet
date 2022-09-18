#include "darksky.h"
#include <SpiRamJsonAllocator.h>

#define TAG "DARKSKY"

const char* amazon_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
"-----END CERTIFICATE-----";



DarkSky::DarkSky() : WeatherClient() {
  ESP_LOGV(TAG,"Starting Darksky");
  horizonWeather = defaultWeather;
  rainbowWeather = defaultWeather;
};

char* DarkSky::constructURL(){
  char *buffer;
  asprintf(&buffer,DARKSKY_PATH,DARKSKY_APIKEY,_currentLocation.lat,_currentLocation.lng);
  return buffer;
}

const char* DarkSky::certificate(){
  return amazon_ca;
}

void DarkSky::setTimeHorizon(uint8_t hours){
  _timeHorizon = hours;
}

void DarkSky::setLocation(ClockLocation location){
  _currentLocation = location;
  horizonWeather = {};
  rainbowWeather = {};
  bool change = true;
  xQueueSend(weatherChangedQueue, &change, (TickType_t) 0);
  this->setNeedsUpdate();
}

bool DarkSky::readReponseContent(Stream *stream) {

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

  rainbowWeather = _parseWeatherBlock(root["hourly"]["data"][0]);

  horizonWeather = defaultWeather;
  for (int i=0 ; i < _timeHorizon ; i++){
    Weather block = _parseWeatherBlock(root["hourly"]["data"][i]);
    horizonWeather += block;
  }

  ESP_LOGI(TAG,"Weather parsing done");
  return true;
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

  if (stristr(result.summary,"thunder")){
    result.thunder = true;
  } else {
    result.thunder = false;
  }
  
  
  return result;
}

char* stristr(const char* haystack, const char* needle){
  do {
    const char* h = haystack;
    const char* n = needle;
    while (tolower((unsigned char) *h) == tolower((unsigned char ) *n) && *n) {
      h++;
      n++;
    }
    if (*n == 0) {
      return (char *) haystack;
    }
  } while (*haystack++);
  return 0;
}
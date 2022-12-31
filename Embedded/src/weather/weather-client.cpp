#include "weather-client.h"
#include <esp_log.h>
#include "../network.h"


#define TAG "WEATHER"



WeatherClient::WeatherClient() : UpdateJob() {

  ESP_LOGD(TAG,"Setting Default Weather");
  this->horizonWeather = defaultWeather;
  this->rainbowWeather = defaultWeather;

};

WeatherClient::~WeatherClient(){
  ESP_LOGD(TAG,"Deallocating WeatherClient");
  // delete nihClient;
}

bool WeatherClient::performUpdate(){
    return fetchWeather();
}

bool WeatherClient::fetchWeather(){

    // Requires location
    if (!_currentLocation){
        ESP_LOGE(TAG, "No location");
        return false;
    }

    // Requires network
    if (!reconnect()){
      return false;
    }

    
    
    HTTPnihClient nihClient;

    Stream *stream = nullptr;
    char* url = constructURL();
    int result = nihClient.get(url, &stream);
    free(url);

    bool parseResult = false;
    if (result == 200){
      if (stream){
        size_t contentLength = nihClient.getHttpClient()->getSize();
        parseResult = readReponseContent(stream, contentLength);
      }
    } 

    if (parseResult){
      bool change = true;
      xQueueSend(weatherChangedQueue, &change, (TickType_t) 0);
    } else {
      ESP_LOGE(TAG, "Weather Parsing failed");
    }

    return parseResult;

}


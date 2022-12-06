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

    
    HTTPnihClient *nihClient = new HTTPnihClient();

    Stream *stream = nullptr;
    char* url = constructURL();
    int result = nihClient->get(url,NULL,&stream);
    free(url);

    bool parseResult = false;
    if (result == 200){
      if (stream){
        parseResult = readReponseContent(stream);
      }
    } 

    if (parseResult){
      bool change = true;
      xQueueSend(weatherChangedQueue, &change, (TickType_t) 0);
    } else {
      ESP_LOGE(TAG, "Weather Parsing failed");
    }

    delete nihClient;

    return parseResult;

}


#include "weather-client.h"
#include <esp_log.h>
#include "../network.h"
#include "../Utilities/HTTPnihClient.h"

#define TAG "WEATHER"

WeatherClient::WeatherClient() : UpdateJob() {
  ESP_LOGD(TAG,"Setting Default Weather");
  this->horizonWeather = defaultWeather;
  this->rainbowWeather = defaultWeather;
};

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

    
    HTTPnihClient nihClient = HTTPnihClient();

    Stream *stream = nullptr;
    char* url = constructURL();
    int result = nihClient.get(url,certificate(),&stream);
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
    return parseResult;

}


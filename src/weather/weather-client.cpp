#include "weather-client.h"
#include <esp_log.h>
#include "../network.h"

#define TAG "WEATHER"

WeatherClient::WeatherClient(WiFiClient &client) : UpdateJob() {
  this->client = &client;
  Serial.print("Setting Default Weather");
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

    if (!connect(this->server, this->ssl)) {
        ESP_LOGE(TAG, "Failed to connect");
        return false;
    }

    if (!sendRequest(this->server, this->resource)){
        ESP_LOGE(TAG, "Request to server failed");
        return false;
    }
    
    if (!skipResponseHeaders()) {
        ESP_LOGE(TAG, "Failed to skip response headers");
        disconnect();
        return false;
    }

    ESP_LOGI(TAG, "Got weather response");
    if (readReponseContent()){
        disconnect();
        bool change = true;
        xQueueSend(weatherChangedQueue, &change, (TickType_t) 0);
        return true;
    }

    ESP_LOGE(TAG, "Weather Parsing failed");
    return false;
}

// Open connection to the HTTP server
bool WeatherClient::connect(char* host, bool ssl) {
  ESP_LOGD(TAG,"Connect to: %s",host);
  
  bool ok;

  if (ssl){
    #if defined(ESP32)
    ok = client -> connect(host, 443);
    #else // ATWINC
    ok = client -> connectSSL(host, 443);
    #endif
  } else {
    ok = client -> connect(host, 80);
  }
  ESP_LOGD(TAG,ok ? "Connected" : "Connection Failed!");


  return ok;
}

// Send the HTTP GET request to the server
bool WeatherClient::sendRequest(char* host, char* resource) {
    ESP_LOGD(TAG,"... sending request");
    ESP_LOGD(TAG,"Host: %s",host);
    ESP_LOGD(TAG,"Resource: %s",resource);
    
    // close any connection before send a new request.
    client -> print("GET ");
    client -> print(resource);
    client -> println(" HTTP/1.1");
    client -> print("Host: ");
    client -> println(host);
    client -> println("Connection: close");
    client -> println("User-Agent: Clocklet");
    client -> println();

    return true;
}


// Skip HTTP headers so that we are at the beginning of the response's body
bool WeatherClient::skipResponseHeaders() {
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";

  client -> setTimeout(WEATHER_HTTP_TIMEOUT);
  bool ok = client -> find(endOfHeaders);

  return ok;
}


// Close the connection with the HTTP server
void WeatherClient::disconnect() {
  ESP_LOGI(TAG, "Disconnecting from weather server");
  client -> stop();
}

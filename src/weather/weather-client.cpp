#include "weather-client.h"

WeatherClient::WeatherClient(WiFiClient &client) {
  this->client = &client;
};


bool WeatherClient::fetchWeather(){

  if (connect(this->server, this->ssl)) {
    if (sendRequest(this->server, this->resource) && skipResponseHeaders()) {
      Weather response = readReponseContent();
      Serial.print("Weather: ");
      Serial.println(response.summary);
      // CRGB minColour = colourFromTemperature(response.minTmp);
      // CRGB maxColour = colourFromTemperature(response.maxTmp);
      // scrollText(response.summary,minColour,maxColour);
      disconnect();
      latestWeather = response;
      return true;
    }
  }
  return false;
  // scrollText_fail("Weather fetch failed");
}

// Open connection to the HTTP server
bool WeatherClient::connect(char* host, bool ssl) {
  Serial.print("Connect to ");
  Serial.println(host);
  bool ok;

  if (ssl){
    #if defined(ESP32)
    ok = client -> connect(host, 443);
    #else
    ok = client -> connectSSL(host, 443);
    #endif
  } else {
    ok = client -> connect(host, 80);
  }
  Serial.println(ok ? "Connected" : "Connection Failed!");
  return ok;
}

// Send the HTTP GET request to the server
bool WeatherClient::sendRequest(char* host, char* resource) {
  Serial.print("Request ");
  Serial.println(host);
  Serial.println(resource);
  // close any connection before send a new request.

  client -> print("GET ");
  client -> print(resource);
  client -> println(" HTTP/1.1");
  client -> print("Host: ");
  client -> println(host);
  client -> println("Connection: close");
  client -> println();

  return true;
}


// Skip HTTP headers so that we are at the beginning of the response's body
bool WeatherClient::skipResponseHeaders() {
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";

  client -> setTimeout(WEATHER_HTTP_TIMEOUT);
  bool ok = client -> find(endOfHeaders);

  if (!ok) {
    Serial.println("No response or invalid response!");
  }

  return ok;
}

Weather WeatherClient::readReponseContent() {
  Serial.println("This is a stub!");

  Weather result;
  return result;
}

// Close the connection with the HTTP server
void WeatherClient::disconnect() {
  Serial.println("Disconnect from weather server");
  client -> stop();
}

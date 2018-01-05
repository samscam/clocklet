#include "darksky.h"
#include "../display.h"

DarkSky::DarkSky(WiFiClient &client) {
  this->client = &client;
};

void DarkSky::fetchWeather(){

  // if ( !client -> connected() ){
  //   scrollText_fail("nope");
  //   return;
  // }

  const char darkskyserver[] = DARKSKY_SERVER;
  // const char apiKey[] = DARKSKY_APIKEY;
  const char resource[] = DARKSKY_PATH;

  if (connect(darkskyserver)) {
    if (sendRequest(darkskyserver, resource) && skipResponseHeaders()) {
      weather response = readReponseContent();
      Serial.print("Weather: ");
      Serial.println(response.summary);
      scrollText(response.summary);
      disconnect();
      latestWeather = response;
      return;
    }
  }
  scrollText_fail("Weather fetch failed");
}

// Open connection to the HTTP server
bool DarkSky::connect(const char* hostName) {
  Serial.print("Connect to ");
  Serial.println(hostName);

  bool ok = client -> connectSSL(hostName, 443);

  Serial.println(ok ? "Connected" : "Connection Failed!");
  return ok;
}

// Send the HTTP GET request to the server
bool DarkSky::sendRequest(const char* host, const char* resource) {
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
bool DarkSky::skipResponseHeaders() {
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";

  client -> setTimeout(DARKSKY_HTTP_TIMEOUT);
  bool ok = client -> find(endOfHeaders);

  if (!ok) {
    Serial.println("No response or invalid response!");
  }

  return ok;
}

weather DarkSky::readReponseContent() {

  // Allocate a temporary memory pool
  DynamicJsonBuffer jsonBuffer(DARKSKY_MAX_CONTENT_SIZE);

  JsonObject& root = jsonBuffer.parseObject(*client);

  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return latestWeather;
  }
  weather result;

  result.type = root["daily"]["data"][0]["icon"];
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
  result.minTmp = root["daily"]["data"][0]["temperatureLow"];

  result.windSpeed = root["daily"]["data"][0]["windSpeed"];

  result.cloudCover = root["daily"]["data"][0]["cloudCover"];
  
  result.thunder = false;
  return result;
}

// Close the connection with the HTTP server
void DarkSky::disconnect() {
  Serial.println("Disconnect from weather server");
  client -> stop();
}

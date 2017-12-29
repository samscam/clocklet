#include "darksky.h"
#include "../network.h"
#include "../display.h"


DarkSky::DarkSky(Client &client){
  this->client = &client;
};


dsweather DarkSky::fetchWeather(){

  if ( !connectWifi() ){
    return latestWeather;
  }

  const char darkskyserver[] = DARKSKY_SERVER;
  const char apiKey[] = DARKSKY_APIKEY;

  const char resource[] = DARKSKY_PATH;

  if (connect(darkskyserver)) {
    if (sendRequest(darkskyserver, resource) && skipResponseHeaders()) {
      dsweather response = readReponseContent();
      Serial.print("Weather type: ");
      Serial.println(response.type);
    //  scrollText(weatherTypes[response.type]);
      disconnect();
      latestWeather = response;
      return(response);
    }
  }
  scrollText_fail("Weather fetch failed");
  return latestWeather;
}

// Open connection to the HTTP server
bool DarkSky::connect(const char* hostName) {
  Serial.print("Connect to ");
  Serial.println(hostName);

  bool ok = client -> connect(hostName, 80);

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
  client -> println(" HTTP/1.0");
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

dsweather DarkSky::readReponseContent() {

  // Allocate a temporary memory pool
  DynamicJsonBuffer jsonBuffer(DARKSKY_MAX_CONTENT_SIZE);

  JsonObject& root = jsonBuffer.parseObject(*client);

  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return latestWeather;
  }
  dsweather result;
  // Here were copy the strings we're interested in
  // if (hours >= 21) {
  //   weatherType = root["SiteRep"]["DV"]["Location"]["Period"][0]["Rep"][1]["W"];
  // } else {
  result.type = root["SiteRep"]["DV"]["Location"]["Period"][0]["Rep"][0]["W"];
  result.precip = root["SiteRep"]["DV"]["Location"]["Period"][0]["Rep"][0]["PPd"];
  result.maxTmp = root["SiteRep"]["DV"]["Location"]["Period"][0]["Rep"][0]["Dm"];
  result.minTmp = root["SiteRep"]["DV"]["Location"]["Period"][0]["Rep"][1]["Nm"];
  // }

  return result;
}

// Close the connection with the HTTP server
void DarkSky::disconnect() {
  Serial.println("Disconnect from weather server");
  client -> stop();
}

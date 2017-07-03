#include "weather.h"
#include "network.h"
#include "display.h"

WiFiClient client;

const char* weatherTypes[] = {
  "clear night", // 0 - clear night
  "sunny day",             // 1 - sunny day
  "partly cloudy night",           // 2 - Partly cloudy night
  "partly cloudy",                 // 3 - Partly cloudy day
  "random",                 // 4 - unused
  "misty",                 // 5 - mist
  "foggy",                 // 6 - fog
  "cloudy",                // 7 - cloudy
  "overcast",                 // 8 - overcast
  "light showers",                 // 9 - light rain shower (night)
  "light showers",        // 10 - light rain shower (day)
  "drizzle",  // 11 - Drizzle
  "light rain", // 12 - Light rain
  "heavy showers", // 13 - Heavy rain shower (night)
  "heavy showers",// 14 - Heavy rain shower (day)
  "heavy rain",// 15 - Heavy rain
  "sleet showers",// 16 - Sleet shower (night)
  "sleet showers",// 17 - Sleet shower (day)
  "sleet",// 18 - Sleet
  "hail shower",// 19 - Hail shower (night)
  "hail shower",// 20 - Hail shower (day)
  "hail",// 21 - Hail
  "light snow shower",// 22 - Light snow shower (night)
  "light snow shower",// 23 - Light snow shower (day)
  "light snow",// 24 - Light snow
  "heavy snow shower",// 25 - Heavy snow shower (night)
  "heavy snow shower",// 26 - Heavy snow shower (day)
  "heavy snow",// 27 - Heavy snow
  "thunder shower",// 28 - Thunder shower (night)
  "thunder shower",// 29 - Thunder shower (day)
  "thunder",// 30 - Thunder
  "rainbows",
};

// MARK: WEATHER FETCHING
weather latestWeather = { -1, 0, 0, 0 };

weather fetchWeather(){

  if ( !connectWifi() ){
    return latestWeather;
  }

  if (connect(server)) {
    if (sendRequest(server, resource) && skipResponseHeaders()) {
      weather response = readReponseContent();
      Serial.print("Weather type: ");
      Serial.println(response.type);
      scrollText(weatherTypes[response.type]);
      disconnect();
      latestWeather = response;
      return(response);
    }
  }
  scrollText_fail("Weather fetch failed");
  return latestWeather;
}

// Open connection to the HTTP server
bool connect(const char* hostName) {
  Serial.print("Connect to ");
  Serial.println(hostName);

  bool ok = client.connect(hostName, 80);

  Serial.println(ok ? "Connected" : "Connection Failed!");
  return ok;
}

// Send the HTTP GET request to the server
bool sendRequest(const char* host, const char* resource) {
  Serial.print("Request ");
  Serial.println(host);
  Serial.println(resource);
  // close any connection before send a new request.

  client.print("GET ");
  client.print(resource);
  client.println(" HTTP/1.0");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close");
  client.println();

  return true;
}


// Skip HTTP headers so that we are at the beginning of the response's body
bool skipResponseHeaders() {
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";

  client.setTimeout(HTTP_TIMEOUT);
  bool ok = client.find(endOfHeaders);

  if (!ok) {
    Serial.println("No response or invalid response!");
  }

  return ok;
}

weather readReponseContent() {

  // Allocate a temporary memory pool
  DynamicJsonBuffer jsonBuffer(MAX_CONTENT_SIZE);

  JsonObject& root = jsonBuffer.parseObject(client);

  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return latestWeather;
  }
  weather result;
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
void disconnect() {
  Serial.println("Disconnect");
  client.stop();
}

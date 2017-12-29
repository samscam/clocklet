#ifndef PRISCILLA_DARKSKY
#define PRISCILLA_DARKSKY

#include <Arduino.h>
#include <WiFi101.h>
#include <ArduinoJson.h>

#define DARKSKY_APIKEY "***REMOVED***"
#define DARKSKY_SERVER "api.darksky.net"
#define DARKSKY_PATH "forecast/***REMOVED***/***REMOVED***?exclude=currently,minutely,daily,alerts,flags"

#define DARKSKY_HTTP_TIMEOUT 10000  // max respone time from server
#define DARKSKY_MAX_CONTENT_SIZE 4096       // max size of the HTTP response


struct dsweather {
  int type;
  int precip;
  int maxTmp;
  int minTmp;
};


class DarkSky{

private:
  bool connect(const char* hostName);
  dsweather fetchWeather();
  void disconnect();

  Client *client;
  dsweather latestWeather;

  bool sendRequest(const char* host, const char* resource);
  bool skipResponseHeaders();
  dsweather readReponseContent();

public:
  DarkSky(Client &client);
  dsweather currentWeather();

};


#endif

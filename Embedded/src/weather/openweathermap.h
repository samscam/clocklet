#ifndef PRISCILLA_OPENWEATHERMAP
#define PRISCILLA_OPENWEATHERMAP

#include "weather-client.h"
#include "../Secrets/APIKeys.h"
#include "Rainbows.h"

#define OWM_SERVER "api.openweathermap.org"
#define OWM_PATH "https://api.openweathermap.org/data/3.0/onecall?lat=%f&lon=%f&exclude=minutely,daily,alerts&units=metric&appid=%s"

#define HTTP_TIMEOUT 20  // max respone time from server
#define MAX_CONTENT_SIZE 16384       // max size of the HTTP response

char* stristr(const char* haystack, const char* needle);

class OpenWeatherMap final : public WeatherClient {
public:
  OpenWeatherMap();
  virtual ~OpenWeatherMap() {};
  
  // Overrides
  bool readReponseContent(Stream *stream);
  void setTimeHorizon(uint8_t hours);
  void setLocation(ClockLocation location);
  char* constructURL();
  const char* certificate();

private:

  Weather _parseWeatherBlock(JsonObject block);
  uint8_t _timeHorizon;

};


#endif
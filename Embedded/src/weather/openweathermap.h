#ifndef PRISCILLA_OPENWEATHERMAP
#define PRISCILLA_OPENWEATHERMAP

#include "weather-client.h"
#include "../Secrets/APIKeys.h"
#include "Rainbows.h"

#define OWM_SERVER "api.openweathermap.org"
#define OWM_PATH "https://api.openweathermap.org/data/3.0/onecall?lat=%f&lon=%f&exclude=minutely,daily,alerts&units=metric&appid=%s"

char* stristr(const char* haystack, const char* needle);

class OpenWeatherMap final : public WeatherClient {
public:
  OpenWeatherMap();
  virtual ~OpenWeatherMap() {};
  
  // Overrides
  bool readReponseContent(Stream *stream, size_t contentLength);
  void setTimeHorizon(uint8_t hours);
  void setLocation(ClockLocation location);
  char* constructURL();

private:

  Weather _parseWeatherBlock(JsonObject block);
  uint8_t _timeHorizon;

};


#endif

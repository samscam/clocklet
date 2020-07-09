#ifndef PRISCILLA_DARKSKY
#define PRISCILLA_DARKSKY

#include "weather-client.h"
#include "../Secrets/APIKeys.h"
#include "Rainbows.h"

#define DARKSKY_SERVER "api.darksky.net"
#define DARKSKY_PATH "/forecast/%s/%f,%f?exclude=currently,minutely,daily,alerts,flags&units=si"

#define DARKSKY_HTTP_TIMEOUT 20  // max respone time from server
#define DARKSKY_MAX_CONTENT_SIZE 16384       // max size of the HTTP response


class DarkSky final : public WeatherClient {
public:
  DarkSky(WiFiClient &client);
  virtual ~DarkSky() {};
  
  // Overrides
  bool readReponseContent();
  void setTimeHorizon(uint8_t hours);
  void setLocation(Location location);

private:

  Weather _parseWeatherBlock(JsonObject block);
  uint8_t _timeHorizon;

};


#endif

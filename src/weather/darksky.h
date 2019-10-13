#ifndef PRISCILLA_DARKSKY
#define PRISCILLA_DARKSKY

#include "weather-client.h"
#include "../Secrets/APIKeys.h"

#define DARKSKY_SERVER "api.darksky.net"
#define DARKSKY_PATH "/forecast/%s/%f,%f?exclude=minutely,hourly,alerts,flags&units=si"

#define DARKSKY_HTTP_TIMEOUT 20  // max respone time from server
#define DARKSKY_MAX_CONTENT_SIZE 16384       // max size of the HTTP response


class DarkSky : public WeatherClient {
public:
  DarkSky(WiFiClient &client);
  Weather readReponseContent();

  void setLocation(Location location);
  Location currentLocation;
};


#endif

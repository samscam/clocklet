#ifndef PRISCILLA_DARKSKY
#define PRISCILLA_DARKSKY

#include "weather-client.h"

#define DARKSKY_APIKEY "***REMOVED***"
#define DARKSKY_SERVER "api.darksky.net"
#define DARKSKY_PATH "/forecast/***REMOVED***/43.5756,3.4908?exclude=currently,minutely,hourly,alerts,flags&units=si"
***REMOVED***
// Withington ***REMOVED***
#define DARKSKY_HTTP_TIMEOUT 20  // max respone time from server
#define DARKSKY_MAX_CONTENT_SIZE 16384       // max size of the HTTP response


class DarkSky : public WeatherClient {
public:
  DarkSky(WiFiClient &client);
  Weather readReponseContent();
};


#endif

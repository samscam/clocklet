#ifndef PRISCILLA_METOFFICE
#define PRISCILLA_METOFFICE

#include "weather-client.h"

#define METOFFICE_APIKEY "***REMOVED***"
#define METOFFICE_SERVER "datapoint.metoffice.gov.uk"
#define METOFFICE_PATH "/public/data/val/wxfcs/all/json/351207?res=3hourly&key=***REMOVED***"
#define METOFFICE_SITE "351207" // didsbury

#define METOFFICE_HTTP_TIMEOUT 20  // max respone time from server
#define METOFFICE_MAX_CONTENT_SIZE 20480       // max size of the HTTP response

class MetOffice : public WeatherClient {
public:
  MetOffice(WiFiClient &client);
  Weather readReponseContent();
  int timeThreshold;
};

#endif

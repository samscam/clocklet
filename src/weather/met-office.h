#ifndef PRISCILLA_METOFFICE
#define PRISCILLA_METOFFICE

#include "weather-client.h"

#define METOFFICE_APIKEY "***REMOVED***"
#define METOFFICE_SERVER "datapoint.metoffice.gov.uk"
#define METOFFICE_PATH "/public/data/val/wxfcs/all/json/351207?res=daily&key=***REMOVED***"

#define METOFFICE_HTTP_TIMEOUT 10000  // max respone time from server
#define METOFFICE_MAX_CONTENT_SIZE 16384       // max size of the HTTP response

class MetOffice : public WeatherClient {
public:
  MetOffice(WiFiClient &client);
  weather readReponseContent();
};

#endif

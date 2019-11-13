#ifndef PRISCILLA_METOFFICE
#define PRISCILLA_METOFFICE

#include "weather-client.h"
#include "../Secrets/APIKeys.h"

#define METOFFICE_SERVER "datapoint.metoffice.gov.uk"
#define METOFFICE_PATH "/public/data/val/wxfcs/all/json/%s?res=3hourly&key=%s"
#define METOFFICE_SITE "351207" // didsbury

#define METOFFICE_HTTP_TIMEOUT 20  // max respone time from server
#define METOFFICE_MAX_CONTENT_SIZE 20480       // max size of the HTTP response

class MetOffice : public WeatherClient {
public:
  MetOffice(WiFiClient &client);
  bool readReponseContent();
  int timeThreshold;
};

#endif

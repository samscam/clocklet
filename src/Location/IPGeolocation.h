#ifndef PRICILLA_IPGEOLOCATION
#define PRICILLA_IPGEOLOCATION

#include "LocationSource.h"
#include "network.h"

#define IPGEOLOCATION_ENDPOINT "https://api.ipgeolocation.io/ipgeo"
#define IPGEOLOCATION_API_KEY "***REMOVED***"

class IPGeolocation: public LocationSource {
    public:
    IPGeolocation(WiFiClient &client);

    private:
    WiFiClient *client;
};



#endif
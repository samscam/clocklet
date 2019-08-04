#ifndef PRICILLA_IPGEOLOCATION
#define PRICILLA_IPGEOLOCATION

#include "LocationSource.h"
#include "network.h"
#include "APIKeys.h"

#define IPGEOLOCATION_ENDPOINT "https://api.ipgeolocation.io/ipgeo"


class IPGeolocation: public LocationSource {
    public:
    IPGeolocation(WiFiClient &client);

    private:
    WiFiClient *client;
};



#endif
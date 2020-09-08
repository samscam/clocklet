#pragma once

#include "LocationSource.h"
#include "network.h"
#include "../Secrets/APIKeys.h"

#define IPGEOLOCATION_ENDPOINT "https://api.ipgeolocation.io/ipgeo"


class IPGeolocation: public LocationSource {
    public:
    IPGeolocation(WiFiClient &client);

    private:
    WiFiClient *client;
};


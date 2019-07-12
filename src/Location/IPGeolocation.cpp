#include "IPGeolocation.h"

IPGeolocation::IPGeolocation(WiFiClient &client) {
    this->client = &client;

}
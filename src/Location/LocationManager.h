#pragma once

#include "LocationSource.h"

bool isValidLocation(Location location);

class LocationManager: public LocationSource {
    public:
    LocationManager();

    bool hasSavedLocation();
    Location getLocation();
    bool setLocation(Location location);

    private:
    Location currentLocation;
};
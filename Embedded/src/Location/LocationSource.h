#pragma once

#include <cmath>
#include <math.h>

struct ClockLocation {
    double lat;
    double lng;
    char placeName[256];
    char timeZone[64];

    bool operator == (const ClockLocation& rhs) const {
        return (lat == rhs.lat && lng == rhs.lng);
    }

    explicit operator bool(){
        if ( std::isnan(lat) || std::isnan(lng)){
            return false;
        }
        if (lat == 0 && lng == 0){
            return false;
        }
        if (lat > 90 || lat < -90){
            return false;
        }
        if (lng > 180 || lng < -180){
            return false;
        }

        return true;
    }

};

class LocationSource {
    public:
    virtual ClockLocation getLocation();
};

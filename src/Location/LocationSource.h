#pragma once

#include <math.h>

struct Location {
    double lat;
    double lng;

    bool operator == (const Location& rhs) const {
        return (lat == rhs.lat && lng == rhs.lng);
    }

    explicit operator bool(){
        if (isnan(lat) || isnan(lng)){
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
    virtual Location getLocation();
};

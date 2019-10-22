#pragma once

struct Location {
    double lat;
    double lng;

};

class LocationSource {
    public:
    virtual Location getLocation();
};

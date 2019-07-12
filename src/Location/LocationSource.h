#ifndef PRISCILLA_LOCATIONSOURCE
#define PRISCILLA_LOCATIONSOURCE


struct Location {
    double lat;
    double lng;

};

class LocationSource
{
    virtual Location location();
};





#endif
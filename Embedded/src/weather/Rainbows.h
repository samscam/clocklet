#pragma once

#include "../Location/LocationSource.h"
#include <RTClib.h>
#include "../weather/weather.h"

class Rainbows {
public:
    
    bool rainbowProbability(DateTime currentTimeUTC);

    void setLocation(Location location);
    void setWeather(Weather weather);
    
    
private:
    DateTime _sunrise;
    DateTime _fortyTwoRise;
    DateTime _fortyTwoSet;
    DateTime _sunset;

    Location _location;
    Weather _weather;
    
    void _calculateSunTimes(DateTime currentTimeUTC, Location currentLocation);
    DateTime _rsTimeToDateTime(DateTime base, double rsTime);
    bool _sameDay(DateTime lhs, DateTime rhs);
};
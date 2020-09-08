#pragma once

#include "../Location/LocationSource.h"
#include <RTClib.h>
#include "../weather/weather.h"

class Rainbows {
public:
    
    bool rainbowProbability(DateTime currentTimeUTC);

    void setLocation(ClockLocation location);
    void setWeather(Weather weather);
    
    
private:
    DateTime _sunrise;
    DateTime _fortyTwoRise;
    DateTime _fortyTwoSet;
    DateTime _sunset;

    ClockLocation _location;
    Weather _weather;
    
    void _calculateSunTimes(DateTime currentTimeUTC, ClockLocation currentLocation);
    DateTime _rsTimeToDateTime(DateTime base, double rsTime);
    bool _sameDay(DateTime lhs, DateTime rhs);
    DateTime _startOfDay(DateTime time);
};
#include "Rainbows.h"
#include <esp_log.h>

extern "C" {
    #include <SolTrack.h>
}

#define TAG "RAINBOWS"

// The currentTime here must be UTC
bool Rainbows::rainbowProbability(DateTime currentTime){

    if (!_sameDay(currentTime,_sunrise)){
        _calculateSunTimes(currentTime, _location);
    }

    if (currentTime < _sunrise || currentTime > _sunset){
        return false;
    }
    if (currentTime > _fortyTwoRise && currentTime < _fortyTwoSet){
        return false;
    }
    if (_weather.cloudCover < 0.8 && _weather.cloudCover > 0.2 && _weather.precipChance >= 0.2 ){
        return true;
    }
    return false;

}

void Rainbows::setLocation(Location location){
    _location = location;
}

void Rainbows::setWeather(Weather weather){
    _weather = weather;
}

// Private
DateTime Rainbows::_rsTimeToDateTime(DateTime base, double rsTime){
    uint8_t hour = floor(rsTime);
    uint8_t min = floor((rsTime - hour) * 60);
    uint8_t sec = floor((((rsTime - hour) * 60) - min) * 60);
    return DateTime(base.year(),
                    base.month(),
                    base.day(),
                    hour,
                    min,
                    sec);
}

/// Calculates the rise and set times for the day
void Rainbows::_calculateSunTimes(DateTime currentTimeUTC, Location currentLocation){

    struct Time time;
    time.year = currentTimeUTC.year();
    time.month = currentTimeUTC.month();
    time.day = currentTimeUTC.day();
    
    struct STLocation location;
    location.longitude = currentLocation.lng;
    location.latitude = currentLocation.lat;
    
    struct Position rsPosition;
    struct RiseSet riseSet;
    
    int useDegrees = 1;
    int useNorthEqualsZero = 1;
    double rsAlt = 0.0;

    SolTrack_RiseSet(time, location, &rsPosition, &riseSet, rsAlt, useDegrees, useNorthEqualsZero);

    _sunrise = _rsTimeToDateTime(currentTimeUTC,riseSet.riseTime);
    _sunset = _rsTimeToDateTime(currentTimeUTC,riseSet.setTime);

    ESP_LOGD(TAG,"Sunrise: %d:%d:%d UTC\n",_sunrise.hour(),_sunrise.minute(),_sunrise.second());
    ESP_LOGD(TAG,"Sunset: %d:%d:%d UTC\n",_sunset.hour(),_sunset.minute(),_sunset.second());
    
    // When do we hit 42 degrees?
    rsAlt = 42.0 / R2D;
    SolTrack_RiseSet(time, location, &rsPosition, &riseSet, rsAlt, useDegrees, useNorthEqualsZero);

    _fortyTwoRise = _rsTimeToDateTime(currentTimeUTC,riseSet.riseTime);
    _fortyTwoSet = _rsTimeToDateTime(currentTimeUTC,riseSet.setTime);

    ESP_LOGD(TAG,"42 Rise: %d:%d:%d UTC\n",_fortyTwoRise.hour(),_fortyTwoRise.minute(),_fortyTwoRise.second());
    ESP_LOGD(TAG,"42 Set: %d:%d:%d UTC\n",_fortyTwoSet.hour(),_fortyTwoSet.minute(),_fortyTwoSet.second());
    
}

bool Rainbows::_sameDay(DateTime lhs, DateTime rhs){
    if (lhs.day() == rhs.day() &&
            lhs.month() == rhs.month() &&
            lhs.year() == rhs.year() ) {
        return true;
    }
    return false;
}
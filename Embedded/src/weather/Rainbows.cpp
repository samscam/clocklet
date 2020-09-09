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

    if (_circumPolar){
        if (_circumPolarWinter){
            return false; // the sun never rises
        } // otherwise, the sun never sets
        
    } else {
        // Normal places with a sunrise and sunset
        if ( _sunrise < _sunset){
            // Normal places...
            if (currentTime < _sunrise || currentTime > _sunset){
                return false; // it is night time
            }

        } else {
            // Places on the other side of the world
            if (currentTime > _sunset && currentTime < _sunrise){
                return false; // it is night time
            }
        }
    }

    if (_goesOver42){ // The sun gets over 42 degrees
        if (_fortyTwoRise < _fortyTwoSet){
            // Normal places...
            if (currentTime > _fortyTwoRise && currentTime < _fortyTwoSet){
                return false; // The sun is too high
            }
        } else {
            if (currentTime < _fortyTwoSet || currentTime > _fortyTwoRise){
                return false; // The sun is too high
            }
        }
    }

    if (_weather.cloudCover < 0.8 && _weather.cloudCover > 0.2 && _weather.precipChance >= 0.2 ){
        return true;
    }
    return false;

}

void Rainbows::setLocation(ClockLocation location){
    _location = location;
    _sunrise = DateTime(); // reset on change of location
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
/// This should be called once per UTC day for a given location
void Rainbows::_calculateSunTimes(DateTime currentTimeUTC, ClockLocation currentLocation){

    struct Time time;
    time.year = currentTimeUTC.year();
    time.month = currentTimeUTC.month();
    time.day = currentTimeUTC.day();
    
    struct Location location;
    location.longitude = currentLocation.lng;
    location.latitude = currentLocation.lat;
    
    struct Position rsPosition;
    struct RiseSet riseSet;
    
    int useDegrees = 1;
    int useNorthEqualsZero = 1;
    double rsAlt = 0.0;

    SolTrack_RiseSet(time, location, &rsPosition, &riseSet, rsAlt, useDegrees, useNorthEqualsZero);

    if (riseSet.riseTime <= 0.0 && riseSet.setTime <= 0.0){
        _circumPolar = true;
        _circumPolarWinter = (riseSet.transitAltitude <= 0.0);
        ESP_LOGD(TAG,"CircumPolar - winter=%d",_circumPolarWinter);
    } else {
        _circumPolar = false;
    }
    _sunrise = _rsTimeToDateTime(currentTimeUTC,riseSet.riseTime);
    _sunset = _rsTimeToDateTime(currentTimeUTC,riseSet.setTime);

    char timeBuf[20];
    strcpy(timeBuf,"YYYY-MM-DD hh:mm:ss");
    ESP_LOGD(TAG,"Sunrise: %s UTC ... %f",_sunrise.toString(timeBuf), riseSet.riseTime);


    strcpy(timeBuf,"YYYY-MM-DD hh:mm:ss");
    ESP_LOGD(TAG,"Sunset: %s UTC ... %f",_sunset.toString(timeBuf),riseSet.setTime);
    
    // When do we hit 42 degrees?
    rsAlt = 42.0 / R2D;
    SolTrack_RiseSet(time, location, &rsPosition, &riseSet, rsAlt, useDegrees, useNorthEqualsZero);

    if (riseSet.riseTime <= 0.0 && riseSet.setTime <= 0.0){
        ESP_LOGD(TAG,"Sun never goes over 42 degrees");
        _goesOver42 = false;
    } else {
        ESP_LOGD(TAG,"Sun does go over 42 degrees");
        _goesOver42 = true;
    }

    _fortyTwoRise = _rsTimeToDateTime(currentTimeUTC,riseSet.riseTime);
    _fortyTwoSet = _rsTimeToDateTime(currentTimeUTC,riseSet.setTime);

    strcpy(timeBuf,"YYYY-MM-DD hh:mm:ss");
    ESP_LOGD(TAG,"42 Rise: %s UTC ... %f",_fortyTwoRise.toString(timeBuf),riseSet.riseTime);

    strcpy(timeBuf,"YYYY-MM-DD hh:mm:ss");
    ESP_LOGD(TAG,"42 Set: %s UTC ... %f",_fortyTwoSet.toString(timeBuf),riseSet.setTime);
    
}

bool Rainbows::_sameDay(DateTime lhs, DateTime rhs){
    if (lhs.day() == rhs.day() &&
            lhs.month() == rhs.month() &&
            lhs.year() == rhs.year() ) {
        return true;
    }
    return false;
}

DateTime Rainbows::_startOfDay(DateTime time){
    return DateTime(time.year(),time.month(),time.day(),0,0,0);
}
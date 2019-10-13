#include "LocationManager.h"
#include <Preferences.h>

LocationManager::LocationManager(){
    // Initialise with null island
    Serial.println("LocationManager constructor ******");
    currentLocation = {0,0};

    Preferences preferences = Preferences();
    preferences.begin("clocklet", true);

    double lat = preferences.getDouble("lat");
    double lng = preferences.getDouble("lng");
    currentLocation.lat = lat;
    currentLocation.lng = lng;

    if (isValidLocation(currentLocation)){
        Serial.printf("Retrieved saved location: %g,%g\n",lat,lng);
    } else {
        Serial.println("No location set, welcome to Null Island!");
    }

    preferences.end();
}

bool LocationManager::hasSavedLocation(){
    return isValidLocation(currentLocation);
}

Location LocationManager::getLocation(){
    return currentLocation;
}

bool LocationManager::setLocation(Location newLocation){
    
    currentLocation = newLocation;

    Preferences preferences = Preferences();
    preferences.begin("clocklet", false);

    preferences.putDouble("lat",newLocation.lat);
    preferences.putDouble("lng",newLocation.lng);

    preferences.end();
    return true;
}

bool isValidLocation(Location location){
    if (location.lat == 0 && location.lng == 0){
        return false;
    }
    if (location.lat > 90 || location.lat < -90){
        return false;
    }
    if (location.lng > 180 || location.lng < -180){
        return false;
    }

    return true;
}
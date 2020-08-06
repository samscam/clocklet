#include "LocationManager.h"
#include <Preferences.h>

LocationManager::LocationManager(QueueHandle_t locationChangedQueue){
    _locationChangedQueue = locationChangedQueue;
    
    // Initialise with null island
    Serial.println("LocationManager constructor ******");
    currentLocation = {0,0,NULL,NULL};

    Preferences preferences = Preferences();
    preferences.begin("clocklet", true);

    double lat = preferences.getDouble("lat",0);
    double lng = preferences.getDouble("lng",0);
    String placeName = preferences.getString("placeName",String("Nowhere"));
    String timeZone = preferences.getString("timeZone",String("Europe/London"));
    currentLocation.lat = lat;
    currentLocation.lng = lng;
    strcpy(currentLocation.placeName, placeName.c_str());
    strcpy(currentLocation.timeZone, timeZone.c_str());

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
    if (!newLocation){
        return false;
    }
    currentLocation = newLocation;

    Preferences preferences = Preferences();
    if (preferences.begin("clocklet", false)) {
        Serial.printf("[LocationManager] Setting location to %g, %g\n",newLocation.lat, newLocation.lng);
        preferences.putDouble("lat",newLocation.lat);
        preferences.putDouble("lng",newLocation.lng);
        preferences.putString("placeName",newLocation.placeName);
        preferences.putString("timeZone",newLocation.timeZone);

        preferences.end();

        bool change = true;
        xQueueSend(_locationChangedQueue, &change, (TickType_t) 0);
        return true;

    } else {
        Serial.println("[LocationManager] Couldn't start preferences for some reason");
        return false;
    }


}

bool isValidLocation(Location location){
    if (location){
        return true;
    } else {
        return false;
    }
}
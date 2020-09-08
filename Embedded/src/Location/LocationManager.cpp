#include "LocationManager.h"
#include <Preferences.h>

#define TAG "LOCMAN"

LocationManager::LocationManager(QueueHandle_t locationChangedQueue){
    _locationChangedQueue = locationChangedQueue;
    
    // Initialise with null island
    currentLocation = {};

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
        ESP_LOGI(TAG,"Retrieved saved location: %g,%g\n",lat,lng);
    } else {
        ESP_LOGE(TAG,"No location set, welcome to Null Island!");
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
        // VV Oddly, uncommenting this makes it crash on changing location... WHY?
        // ESP_LOGI(TAG,"[LocationManager] Setting location to %f, %f",newLocation.lat, newLocation.lng);
        preferences.putDouble("lat",newLocation.lat);
        preferences.putDouble("lng",newLocation.lng);
        preferences.putString("placeName",newLocation.placeName);
        preferences.putString("timeZone",newLocation.timeZone);

        preferences.end();

        bool change = true;
        xQueueSend(_locationChangedQueue, &change, (TickType_t) 0);
        return true;

    } else {
        ESP_LOGE(TAG,"[LocationManager] Couldn't start preferences for some reason");
        return false;
    }


}

bool LocationManager::isValidLocation(Location location){
    if (location){
        return true;
    } else {
        return false;
    }
}
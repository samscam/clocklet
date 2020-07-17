#pragma once

#include "LocationSource.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

bool isValidLocation(Location location);

class LocationManager: public LocationSource {
    public:
    LocationManager(QueueHandle_t locationChangedQueue);

    bool hasSavedLocation();
    Location getLocation();
    bool setLocation(Location location);

    private:
    Location currentLocation;
    QueueHandle_t _locationChangedQueue;
};
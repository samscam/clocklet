#pragma once

#include "LocationSource.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

class LocationManager: public LocationSource {
    public:
    LocationManager(QueueHandle_t locationChangedQueue);

    bool hasSavedLocation();
    ClockLocation getLocation();
    bool setLocation(ClockLocation location);

    private:
    bool isValidLocation(ClockLocation location);
    ClockLocation currentLocation;
    QueueHandle_t _locationChangedQueue;
};
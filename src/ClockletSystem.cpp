#include "ClockletSystem.h"

#include <Esp.h>
#include <nvs_flash.h>

void resetClocklet(){
    nvs_flash_erase();
    ESP.restart();
}
#include "ClockletSystem.h"

#include <Esp.h>
#include <nvs_flash.h>

void doFactoryReset(){
    nvs_flash_erase();
    ESP.restart();
}

void doPartialReset(){

}

void doReboot(){
    ESP.restart();
}
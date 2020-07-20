#include "ClockletSystem.h"

#include <Esp.h>
#include <nvs_flash.h>
#include <soc/efuse_reg.h>

void clocklet_factoryReset(){
    nvs_flash_erase();
    ESP.restart();
}

void clocklet_partialReset(){
    // NOT IMPLEMENTED BOOOOO
}

void clocklet_reboot(){
    ESP.restart();
}

uint32_t clocklet_serial(){
    return REG_GET_FIELD(EFUSE_BLK3_RDATA7_REG, EFUSE_BLK3_DOUT7);
}

uint16_t clocklet_caseColour(){
    uint32_t hwcaseField = REG_GET_FIELD(EFUSE_BLK3_RDATA6_REG, EFUSE_BLK3_DOUT6);
    uint16_t caseColour = hwcaseField >> 16;
    return caseColour;
}

uint16_t clocklet_hwrev(){
    uint32_t hwcaseField = REG_GET_FIELD(EFUSE_BLK3_RDATA6_REG, EFUSE_BLK3_DOUT6);
    uint16_t hwrev = (hwcaseField << 16) >> 16;
    return hwrev;
}
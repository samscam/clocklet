#pragma once
#include <stdint.h>

// Reset and reboot
void clocklet_reboot();
void clocklet_partialReset();
void clocklet_factoryReset();

// System information
uint32_t clocklet_serial();
uint16_t clocklet_caseColour();
uint16_t clocklet_hwrev();

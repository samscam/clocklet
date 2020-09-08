#ifndef PRISCILLA_COLOURS
#define PRISCILLA_COLOURS

#include <Arduino.h>

// Type for Colour

struct Colour {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

// ----------- COLOUR
const Colour WHITE = {120,128,128};
const Colour RED = {128, 0, 0};
const Colour ORANGE = {128, 96, 0};
const Colour YELLOW = {96, 96, 0};
const Colour GREEN = {0, 128, 15};
const Colour LIGHT_BLUE = {80, 80, 128};
const Colour BLUE = {0, 40, 128};
const Colour DARK_BLUE = {20, 0, 50};
const Colour GRAY = {64,64,64};
const Colour DARK_GRAY = {32,32,32};
const Colour BROWN = {55,55,30};
const Colour OTHER_BROWN = {40,60,0};

// DEFAULT RAINBOW
const Colour RAINBOW[5] = {
    RED,ORANGE,GREEN,BLUE,WHITE
};

const Colour ALLWHITE[5] = {
    WHITE,WHITE,WHITE,WHITE,WHITE
};
#endif

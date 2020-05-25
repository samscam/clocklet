#include <FastLED.h>

void fill_matrix_radial_rainbow(CRGB *pFirstLed,int columns,int rows,int centrex,int centrey,uint8_t initialHue, uint8_t deltaHue);
uint16_t XY( uint8_t x, uint8_t y);
uint16_t XYsafe( uint8_t x, uint8_t y);
void fillMatrixWave(CRGB *leds, int columns, int rows, uint8_t position, uint8_t step, CRGBPalette256 palette);

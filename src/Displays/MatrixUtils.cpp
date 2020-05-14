#include "MatrixUtils.h"

#define COLUMNS  17
#define ROWS 5
const bool    kMatrixSerpentineLayout = true;

void fill_matrix_radial_rainbow(CRGB *leds,
    int columns,
    int rows,
    int centrex,
    int centrey,
    uint8_t initialHue,
    uint8_t deltaHue){
        double hyp;
        uint8_t hue;
        uint8_t saturation = 255;
        uint8_t brightness = 255;

        for (int x=0;x<columns;x++){
            for (int y=0;y<rows;y++){

                hyp = sqrt(pow(x-centrex,2) + pow(y-centrey,2));
                hue = initialHue + ( hyp * deltaHue);
                // Serial.printf("%d,%d hyp:%f hue:%d\n",x,y,hyp,hue);
                leds[XYsafe(x,y)] = CHSV(hue,saturation,brightness);
  
            }
        }


}

uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  
  if( kMatrixSerpentineLayout == false) {
    i = (y * COLUMNS) + x;
  }

  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (COLUMNS - 1) - x;
      i = (y * COLUMNS) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * COLUMNS) + x;
    }
  }
  
  return i;
}

uint16_t XYsafe( uint8_t x, uint8_t y)
{
  if( x >= COLUMNS) return -1;
  if( y >= ROWS) return -1;
  return XY(x,y);
}
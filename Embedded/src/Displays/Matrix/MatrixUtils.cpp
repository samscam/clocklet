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
    double deltaHue){
        double hyp;
        uint8_t hue;
        uint8_t saturation = 255;
        uint8_t brightness = 255;

        for (int x=0;x<columns;x++){
            for (int y=0;y<rows;y++){

                hyp = sqrt(pow(x-centrex,2) + pow(y-centrey,2));
                hue = initialHue - ( hyp * deltaHue);
                leds[XYsafe(x,y)] = CHSV(hue,saturation,brightness);
  
            }
        }


}

void fillMatrixWave(CRGB *leds, int columns, int rows, uint8_t position, uint8_t step, CRGBPalette256 palette){
  uint8_t yp;
  uint8_t palpos;
  uint8_t xchunk = 255/(columns*2);
  uint8_t ychunk = 255/(rows*2);

  for (int x=0;x<columns;x++){
    for (int y=0;y<rows;y++){
      yp = sin8((x * (xchunk)) + position);
      palpos = yp + (y*ychunk);
      leds[XYsafe(x,4-y)] = palette[palpos];
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
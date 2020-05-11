#if defined(MATRIX)

#include "Matrix.h"
#include <Fonts/MatrixFont.h>
#include "../settings.h"

FASTLED_USING_NAMESPACE

// This is a gradient palette for the temperature scale

DEFINE_GRADIENT_PALETTE (temperatureGPalette) {
0,   127, 127, 255, // -10
51,  255, 255, 255, // 0
61,  127, 255, 255, // 2
76,  0,   255, 255, // 5
102, 0,   255, 0,   // 10
127, 255, 255, 0,   // 15
153, 255, 128,   0,   // 20
177, 255, 0,   0,   // 25
204, 255, 0,   255,   // 30
255, 120, 0,   255  // 40
};



Matrix::Matrix() : Display() {
  leds = leds_plus_safety_pixel + 1;
}

// PUBLIC METHODS


boolean Matrix::setup() {
  FastLED.addLeds<LED_TYPE,DATA_PIN,CLOCK_PIN,COLOR_ORDER>(leds, NUM_LEDS);//.setCorrection(TypicalLEDStrip);
  temperaturePalette = temperatureGPalette;
  initRain();
  regenerateHeatPalette(0.0,0.0);

  return true;
}

void Matrix::frameLoop() {
  displayTime(_time,_weather);
}

void Matrix::setWeather(Weather weather) {
  _weather = weather;
  
  regenerateHeatPalette(_weather.minTmp,_weather.maxTmp);
}

void Matrix::displayTemperatures(){
  const char * message = "Currently";
  CRGB colour = colourFromTemperature(_weather.currentTmp);
  scrollText(message, colour);
  
  setDigits(_weather.currentTmp,colour);
  FastLED.delay(4000);
}

void Matrix::setTime(DateTime time) {
  _time = time;
}

void Matrix::displayMessage(const char *stringy, MessageType messageType = good) {
  switch (messageType){
    case good:
    scrollText(stringy);
    break;
    case bad:
    scrollText_fail(stringy);
    break;
    case rando:
    scrollText_randomColour(stringy);
    break;
  }
}

void Matrix::setStatusMessage(const char * string){
  fillDigits_rainbow(true);
  setDigits(string);
  FastLED.show();
}

void Matrix::setBrightness(float brightness){
  uint8_t scaledBrightness = MIN_BRIGHTNESS + (brightness * (MAX_BRIGHTNESS - MIN_BRIGHTNESS));
  FastLED.setBrightness(scaledBrightness);
}

void Matrix::setRainbows(bool newState){
  rainbows = newState;
}

void Matrix::setDeviceState(DeviceState newState){
  _deviceState = newState;
}

void Matrix::graphicsTest(){
  // Red
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
  delay(500);

  // Green
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  FastLED.show();
  delay(500);

  // Blue
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  FastLED.show();
  delay(500);

  // Rainbow
  fill_rainbow(leds,NUM_LEDS,0);
  FastLED.show();
  delay(500);

  // for (int f = -10; f<41; f++){
  //     CRGB colour = colourFromTemperature((float)f);
  //     setDigits(f,colour);
  //     frameLoop();
  // }
}

// PRIVATE


// MARK: SCROLLING TEXT

void Matrix::scrollText(const char *stringy){
  scrollText(stringy, CRGB::Green);
}

void Matrix::scrollText_fail(const char *stringy){
  scrollText(stringy, CRGB::Red);
}

void Matrix::scrollText_randomColour(const char *stringy){
  CHSV colour = CHSV(random8(), 255, 255);
  scrollText(stringy, colour);
}

void Matrix::scrollText(const char *stringy, CRGB colour){
  scrollText(stringy, colour, colour);
}

void Matrix::scrollText(const char *stringy, CRGB startColour, CRGB endColour) {
  // Serial.println(stringy);

  // char charbuffer[DIGIT_COUNT] = { 0 };
  // int origLen = strlen(stringy);
  // int extendedLen = origLen + DIGIT_COUNT;
  // char res[extendedLen];
  // memset(res, 0, extendedLen);
  // memcpy(res,stringy,origLen);

  // int i;
  // for ( i = 0; i < extendedLen ; i++ ) {
  //   fillDigits_gradient(startColour,endColour);

  //   for ( int d = 0; d < DIGIT_COUNT ; d++ ) {
  //     if (d == 3) {
  //       charbuffer[d] = res[i];
  //     } else {
  //       charbuffer[d] = charbuffer[d+1];
  //     }

  //     setDigit(charbuffer[d], d);

  //   }

  //   FastLED.show();
  //   FastLED.delay(200);

  // }
}


// MARK: DISPLAY TIME --------------------------------------


void Matrix::displayTime(const DateTime& time, Weather weather){

  // Advance the wind cycle
  advanceWindCycle(weather.windSpeed);

  if (rainbows){
    // Fill the digits entirely with rainbows
    fillDigits_rainbow(false);
  } else {
    // Or heat colours
    fillDigits_heat();
  }


  float precip = weather.precipChance * 100;
  float minTmp = weather.minTmp;

  // PRECIPITATION --------

  // We are shaving off anything under a 25% chance of rain and calling that zero
  precip = precip - 25;
  precip = precip < 0 ? 0 : precip;
  fract8 rainRate = (precip * 255) / 75.0;

  if (rainRate > 0) {
    switch (weather.precipType) {
      case Snow:
        addSnow(rainRate);
        break;
      case Rain:
        addRain(rainRate, CRGB::Blue);
        break;
      case Sleet:
        addRain(rainRate, CRGB::White);
        break;
    }
  }

  if (minTmp <= 0.0f){
    addFrost();
  }

  if (weather.thunder){
    addLightening();
  }

  maskTime(time);

  _blinkColon = (time.second() % 2) == 0;

  CRGB dotColour;
  switch(_deviceState){
    case ok:
      dotColour = CRGB::Black;
      break;
    case weatherFail:
      dotColour = CRGB::LightYellow;
      break;
    case syncFail:
      dotColour = CRGB::Violet;
      break;
    case noLocation:
      dotColour = CRGB::LimeGreen;
      break;
    case noNetwork:
      dotColour = CRGB::Red;
      break;
    case bluetooth:
      dotColour = CRGB::Blue;
      break;
  }

  setDot(_blinkColon,dotColour);

  FastLED.show();

}

void Matrix::maskTime(const DateTime& time){
  int digit[4];

  int h = time.hour();
  digit[0] = h/10;                      // left digit
  digit[1] = h - (h/10)*10;             // right digit

  int m = time.minute();
  digit[2] = m/10;
  digit[3] = m - (m/10)*10;

  for (int i = 0; i<4 ; i++){
    setDigit(digit[i], i);
  }

  // skip leading zeroes
  if (digit[0] == 0){
    setDigit(' ',0);
  }
}


void Matrix::setDigit(int number, int digit){
  uint16_t mask = MATRIX_FONT_NUMBERS[number];
  setDigitMask(mask,digit);
}



void Matrix::setDigit(char character, int digit){
  uint16_t mask = 0;

  if (character == 45){
    // minus
    mask = 0b0000001110000000;
  } else if (character < 48) {
    // It's a control char
  } else if (character >= 48 && character <= 57) {
    // Numbers
    mask = MATRIX_FONT_NUMBERS[character - 48];
  } else if (character >= 65 && character <= 90) {
    // Uppercase
    // mask = _charMasks[character-65];
  } else if (character >= 97 && character <= 122) {
    // Lowercase
    // mask = _charMasks[character-97];
  }

  setDigitMask(mask,digit);
}



void Matrix::setDigitMask(uint16_t mask, int digit){

  int bit = 0;

  int xpos = digit*4;

  if (digit >= 2){
    xpos += 2;
  }

  int x = 0;
  int y = 0;

  while (bit < 15) {

    if (!(mask & 0x01)){
      leds[ XYsafe(xpos+2-x,y) ] = CRGB::Black;
    }
    mask = mask >> 1;
    bit++;
    x++;
    if (x>2){
      x=0;
      y++;
    }

  }

  //black column after the digit
  for (int n=0;n<5;n++){
    leds[ XYsafe(xpos+3,n) ] = CRGB::Black;
  }

}

void Matrix::setDot(bool state, CRGB colour){
  leds[ XYsafe(8,0) ] = CRGB::Black ;
  leds[ XYsafe(8,2) ] = CRGB::Black ;
  leds[ XYsafe(8,4) ] = CRGB::Black ;

  if (!state){
    leds[ XYsafe(8,1) ] =  colour ;
    leds[ XYsafe(8,3) ] =  colour ;
  }
  
    //black column after the dots
  for (int n=0;n<5;n++){
    leds[ XYsafe(9,n) ] = CRGB::Black;
  }
}

/// Display a string (up to the length of the display)
void Matrix::setDigits(const char *string){
  // for (int i=0;i<DIGIT_COUNT;i++){
  //   setDigit(string[i],i);
  // }
}

/// Display an integer
void Matrix::setDigits(int number, CRGB colour){
  fillDigits_gradient(colour,colour);
  // bool negative = false;

  // if (number < 0) {
  //   negative = true;
  //   number = number * -1;
  // }

  // for (int i=0;i<DIGIT_COUNT-1;i++){
  //   int fromRight = DIGIT_COUNT - i - 1;
  //   int units = number % 10;
  //   setDigit(units,fromRight);
  //   number = number / 10;
  // }

  // if (negative){
  //   setDigit('-',0);
  // } else {
  //   setDigit(' ',0);
  // }

}

/// Display a float
/// This really doesn't work properly.
void Matrix::setDigits(float number, CRGB colour){
  fillDigits_gradient(colour,colour);

  // // Decompose the float
  // float integral, fractional;
  // fractional = modff(number, &integral);

  // int digit = 0;

  // bool negative = (fractional < 0) ;
  // if (negative){
  //   setDigit('-',digit);
  //   digit++;

  //   integral = fabsf(integral);
  //   fractional = fabsf(fractional);
  // }

  // int integralInt = integral;
  // bool doneDot = false;
  // for (;digit<DIGIT_COUNT;digit++){
    
  //   if (integralInt > 0){
  //     int first = integralInt;
  //     int exp = 0;
  //     while (first > 10){
  //       first = first / 10;
  //       exp ++;
  //     }
  //     setDigit(first,digit);
  //     integralInt -= first * pow(10,exp);
  //   } else {

  //     if (!doneDot){
  //       setDot(true,digit-1, colour);
  //       doneDot = true;
  //     }

  //     int firstFract = fractional * 10;

  //     setDigit(firstFract,digit);
  //     float dumper;
  //     fractional = modff(fractional*10,&dumper);

  //   }
  // }

}
// WIND

void Matrix::advanceWindCycle(float speed){
  /*
  Wind speed comes in here in m/s
  we are scaling it in a dumb way but in beaufort terms:

  0 Calm: 0.5 m/s = 2.5 steps/second = 0.041 steps/frame = 102 seconds/iteration
  1 Light air: 1m/s - 51 secs/iteration
  2 Light breeze: 2.5m/s - 20 secs/iteration
  ... you get the idea
  5 Fresh breeze: 10m/s - 5.1 s/iteration
  8 Gale: 20m/s - 2.55 s/iteration
  10 Storm: 25m/s - 2.04 s/iteration
  12 Hurricane: 32m/s - 1.59 s/iteration
  
  */
 
  cycle = cycle + ( (speed * 5.0f ) / (float)FPS );
  if (cycle > 255.0f) {
    cycle -= 255.0f;
  }

}

void Matrix::fillDigits_rainbow(bool includePoints){


  uint8_t hue = cycle;
  fill_rainbow(leds, NUM_LEDS, hue, 1);
  
}



CRGB Matrix::colourFromTemperature(float temperature){
  float min = -10.0;
  float max = 40.0;

  if (temperature < min) { temperature = min; }
  if (temperature > max) { temperature = max; }

  uint8_t scaled = (int)roundf(((temperature - min) / (max - min)) * 255.0);

  CRGB col = ColorFromPalette(temperaturePalette, scaled);
  return col;
}

void Matrix::fillDigits_heat(){


  fill_palette(leds, NUM_LEDS, cycle, 1, scaledHeatPalette, 255, LINEARBLEND);

}

void Matrix::regenerateHeatPalette(float minTemp, float maxTemp){
  scaledHeatPalette = CRGBPalette16();
  float tempChunk = (maxTemp - minTemp) / 16.0f;
  for (int i = 0; i < 16; i++) {
    float temp = minTemp + (tempChunk * i);
    scaledHeatPalette[i] = colourFromTemperature(temp);
  }
}

void Matrix::fillDigits_gradient(CRGB startColour, CRGB endColour){

  fill_gradient_RGB(leds , 0,  startColour, NUM_LEDS, endColour);

}

// Rain (also sleet)

/// Creates the mapping in `allvsegs` to the vertical segments in `leds`
/// This only needs to be done once on initialisation of the class
void Matrix::initRain(){
  // int s = 0;

  // for (int digit = 0; digit < NUM_DIGITS ; digit++) {
  //   for (int vseg = 0 ; vseg < 4 ; vseg++){
  //     int digStep = DIGIT_SEGS * digit;
  //     int val = vsegs[vseg] + digStep;
  //     allvsegs[s] = val;
  //     s++;
  //   }
  // }
}

void Matrix::addRain( fract8 chanceOfRain, CRGB colour)
{
  // for(int i = 0; i < NUM_LEDS; i++) {
  //   rainLayer[i].nscale8(230);
  // }
  // if( random8() < chanceOfRain) {
  //   int segnum = random8(4 * NUM_DIGITS);

  //   rainLayer[ allvsegs[segnum] ] = colour;
  // }
  // nscale8_video(leds, NUM_LEDS, 255 - (chanceOfRain * 0.7));

  // for(int i = 0; i < NUM_LEDS; i++) { leds[i] += rainLayer[i] ; }
}

// Snow

void Matrix::addSnow( fract8 chanceOfSnow ) {
  // for(int i = 0; i < NUM_LEDS; i++) {
  //   rainLayer[i].nscale8_video(252);
  // }
  // if( random8() < chanceOfSnow) {
  //   int segnum = random8(NUM_LEDS);
  //   rainLayer[ segnum ] = CRGB::White;
  // }
  // nscale8_video(leds, NUM_LEDS, 255 - chanceOfSnow);

  // for(int i = 0; i < NUM_LEDS; i++) { leds[i] += rainLayer[i] ; }
}

// Frost

void Matrix::addFrost(){
  // CRGB frostLayer[NUM_LEDS];
  // fill_solid(frostLayer, NUM_LEDS, CRGB::Black);
  // // We want the bottom row and the next one up...

  // for (int d = 0; d < NUM_DIGITS; d++){
  //   // Serial.println((d*8) + 3);

  //   frostLayer[ (d*8) + 3 ] = CHSV(0,0,200);
  //   frostLayer[ (d*8) + 2 ] = CHSV(0,0,90);
  //   frostLayer[ (d*8) + 4 ] = CHSV(0,0,90);
  // }

  // for(int i = 0; i < NUM_LEDS; i++) { leds[i] += frostLayer[i] ; }
}

// Lightening

void Matrix::addLightening(){
  for(int i = 0; i < NUM_LEDS; i++) {
    lighteningLayer[i].nscale8(150);
  }
  if(random8() == 1) {
    fill_solid(lighteningLayer, NUM_LEDS, CRGB::White);
  }
  for(int i = 0; i < NUM_LEDS; i++) { leds[i] += lighteningLayer[i] ; }
}

// Matrix helpers from fastled example

uint16_t Matrix::XY( uint8_t x, uint8_t y)
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

uint16_t Matrix::XYsafe( uint8_t x, uint8_t y)
{
  if( x >= COLUMNS) return -1;
  if( y >= ROWS) return -1;
  return XY(x,y);
}


#endif

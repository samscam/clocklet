#if defined(MATRIX)

#include "Matrix.h"
#include <Fonts/MatrixFont.h>
#include "../settings.h"
#include "MatrixUtils.h"


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
  FastLED.addLeds<LED_TYPE,DATA_PIN,CLOCK_PIN,COLOR_ORDER >(leds, NUM_LEDS);//.setCorrection(TypicalLEDStrip);
  temperaturePalette = temperatureGPalette;
  initRain();
  regenerateHeatPalette(0.0,0.0);

  return true;
}

void Matrix::frameLoop() {
  displayTime(_time,_weather);
}

void Matrix::setWeather(Weather weather) {

//   _weather = {
//   .summary = "No weather yet",
//   .type = 0,
//   .precipChance = 1.0f,
//   .precipIntensity = 1.0f,
//   .precipType = Rain,
//   .maxTmp = 01.0f,
//   .minTmp = 08.0f,
//   .currentTmp = 02.0f,
//   .thunder = false,
//   .windSpeed = 10.0f,
//   .cloudCover = 0.0f,
//   .pressure = 0.0f,
// };


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
    case rainbow:
    scrollText_rainbow(stringy);
    break;
  }
}

void Matrix::setStatusMessage(const char * string){
  fillDigits_rainbow();
  displayString(string);
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
  // // Red
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  // displayString("Red");
  FastLED.show();
  delay(2000);

  // // Green
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  // displayString("Green");
  FastLED.show();
  delay(2000);

  // // Blue
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  // displayString("Blue");
  FastLED.show();
  delay(2000 );

  // // Rainbow

  for (int i=0;i<100;i++){
    fill_matrix_radial_rainbow(leds,COLUMNS,ROWS,8,30,i,50);
    FastLED.show();
    delay(10);
  }



  // // Temperature gradients
  // for (float f = -10; f<41; f+=0.1){
  //     CRGB colour = colourFromTemperature((float)f);
  //     setDigits(f,colour);
  //     FastLED.show();
  //     delay(100);
  // }

  // Rain

  // SETUP FAKE FRAMERATE
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(1000/FPS);

  CRGBPalette256 pal;
  // pal = RainbowStripeColors_p;
  regenerateHeatPalette(0,20);

  pal = scaledHeatPalette;
  uint8_t position = 0;
  ulong startMillis = millis();
  while (millis()-startMillis < 10000){
    fillMatrixWave(leds,COLUMNS,ROWS,position,1,pal);
    position++;
    FastLED.show();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }

  // RAIN
  fract8 rate = 50;
  bool done = false;
  uint8_t hue = 0;
  startMillis = millis();
  while (millis()-startMillis < 10000){
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    addRain(rate,CHSV(hue,255,255));
    hue++;

    FastLED.show();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }

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

void Matrix::scrollText_rainbow(const char *stringy){
  scrollText(stringy, CRGB::Black, CRGB::Black, true);
}

void Matrix::scrollText(const char *stringy, CRGB startColour, CRGB endColour, bool rainbow) {
  
  Serial.printf("Scrolling: %s\n",stringy);

  int remainingCharacters = strlen(stringy);

  int charCount = 0;
  bool imageBuffer[255][5] = {};
  
  int lastGlyphWidth = 0;
  int shiftedSinceLastChar = 0;
  uint8_t step = 0;

  bool isScrolling = true;
  
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(1000/FPS);

  int millisPerStep = 100;
  ulong lastmillis = millis();

  while (isScrolling){
    if (rainbow){
      fill_matrix_radial_rainbow(leds,COLUMNS,ROWS,8,30,step,10);
    } else {
      fillDigits_gradient(startColour,endColour,step);
    }
    
    

    step++;
    // Render the buffer
    for (int y=0;y<5;y++){
      for (int x=0;x<17;x++){
        if (!imageBuffer[x][y]){
          leds[ XYsafe(x,4-y)] = CRGB::Black; 
        }
      }
    }
    FastLED.show();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    if (millis()-lastmillis < millisPerStep){
      continue;
    }

    lastmillis = millis();

    // Shift it one to the left
    for (int col = 0; col<255-1; col++){
      for (int row = 0; row<5; row++){
        imageBuffer[col][row] = imageBuffer[col+1][row];
      }
    }

    shiftedSinceLastChar ++;

    // Do we need another character adding?
    if (shiftedSinceLastChar > lastGlyphWidth && remainingCharacters > 0){
      char thisCharacter = stringy[charCount];

      lastGlyphWidth = drawChar(imageBuffer,thisCharacter,COLUMNS,0,MATRIX5_FONT);
      remainingCharacters--;
      charCount++;
      shiftedSinceLastChar = 0;
    }
    if (remainingCharacters == 0 && shiftedSinceLastChar > lastGlyphWidth + COLUMNS){
      isScrolling = false;
    }
    
  }
}

/// Display a string (up to the length of the display)
void Matrix::displayString(const char *string){

  bool imageBuffer[255][5] = {0};
  int xpos = 0;
  for (int ch=0;ch<strlen(string);ch++){  
    int width = drawChar(imageBuffer,string[ch],xpos,0,MATRIX5_FONT);
    
    xpos += width+1;
  }

  for (int y=0;y<5;y++){
    for (int x=0;x<17;x++){
      if (!imageBuffer[x][y]){
        leds[ XYsafe(x,4-y)] = CRGB::Black; 
      }
    }
  }
}


int Matrix::drawChar(bool imageBuffer[255][5], char character, int xpos, int ypos, const byte* font){

  int width = 0;
  bool charfound = false;
  
  int i = 0;

  // Each time we want to draw a char it will iterate through the font table until it finds the right glyph
  while (!charfound && font[i] != 0x00) {

    // The font table structure is... (one byte at a time)
    char ascii = font[i]; // Ascii code for the glyph
    width = font[i+1];    // Width of the glyph
    int height = font[i+2];// Height of the glyph
    int glyphBits = width*height;
    size_t glyphBytes = glyphBits/8 + (glyphBits % 8 != 0); // this many bytes of actual bitmap data
    // ... and then on to the next ascii code
    // Some kind of lookup (or not re-inventing the wheel here) would probably be preferable,
    // but it works and is easy to edit...

    i += 3;
    if (ascii == character) {

      charfound = true;
      int glyphx = 0;
      int glyphy = 0;
      int processedBits = 0;
      // render it into the buffer
      for (int b = 0; b < glyphBytes; b++ ){
        byte workingByte = font[i+b];
        int bit = 0;

        while (bit < 8 && processedBits<glyphBits){
          byte crunched = workingByte << bit;
          crunched = crunched >> 7;
          
          imageBuffer[ glyphx+xpos][ glyphy+ypos ] = crunched;
          bit++;
          processedBits++;
          glyphx++;
          if (glyphx==width){
            glyphx=0;
            glyphy++;
          }
        }

      }
    }
    
    i += glyphBytes;
  }
  return width;
}   

// MARK: DISPLAY TIME --------------------------------------


void Matrix::displayTime(const DateTime& time, Weather weather){

  // Advance the wind cycle
  advanceWindCycle(weather.windSpeed);

  if (rainbows){
    // Fill the digits entirely with rainbows
    fillDigits_rainbow();
  } else {
    // Or heat colours
    fillDigits_heat();
  }


  float precip = weather.precipChance * 100;
  float minTmp = weather.minTmp;

  // PRECIPITATION --------

  // We are shaving off anything under a 25% chance of rain and calling that zero
  precip = precip - 20;
  precip = precip < 0 ? 0 : precip;
  fract8 rainChance = (precip * 255) / 80.0;


  if (rainChance > 0) {
    switch (weather.precipType) {
      case Snow:
        addSnow(rainChance);
        break;
      case Rain:
        addRain(rainChance, CRGB::Blue);
        break;
      case Sleet:
        addRain(rainChance, CRGB::White);
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

  CRGB dotColour = CRGB::Black;
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



/// Display an integer
void Matrix::setDigits(int number, CRGB colour){
  fillDigits_gradient(colour,colour);
  bool negative = false;

  if (number < 0) {
    negative = true;
    number = number * -1;
  }

  for (int i=0;i<4-1;i++){
    int fromRight = 4 - i - 1;
    int units = number % 10;
    setDigit(units,fromRight);
    number = number / 10;
  }

  if (negative){
    setDigit('-',0);
  } else {
    setDigit(' ',0);
  }

}

/// Display a float
void Matrix::setDigits(float number, CRGB colour){
  fillDigits_gradient(colour,colour);
  char buf[10];
  sprintf(buf,"%3.1f",number);
  displayString(buf);
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
 
  cycle = cycle + ((speed * 5.0f ) / (float)FPS);
  
  // loop it round
  if (cycle > 255.0){
    cycle = cycle - 255.0;
  }

  // Catch accidental infinity
  if (cycle > 255.0 || cycle < -255.0) {
    cycle = 0;
  }
}

void Matrix::fillDigits_rainbow(){

  uint8_t hue = -cycle;
  fill_matrix_radial_rainbow(leds,COLUMNS,ROWS,8,30,hue,50);
}



CRGB Matrix::colourFromTemperature(float temperature){
  float min = -10.0;
  float max = 40.0;

  if (temperature < min) { temperature = min; }
  if (temperature > max) { temperature = max; }

  uint8_t scaled = roundf(((temperature - min) / (max - min)) * 255.0);

  CRGB col = ColorFromPalette(temperaturePalette, scaled);
  return col;
}

void Matrix::fillDigits_heat(){

  uint8_t startIndex = cycle;
  // fill_palette(leds, NUM_LEDS, startIndex, 1, scaledHeatPalette, 255, LINEARBLEND);
  fillMatrixWave(leds,COLUMNS,ROWS,startIndex,1,scaledHeatPalette);
}

void Matrix::regenerateHeatPalette(float minTemp, float maxTemp){
  CRGBPalette32 tempPalette = CRGBPalette32();
  float tempChunk = (maxTemp - minTemp) / 16.0f;
  int z=31;
  for (int i = 0; i < 16; i++) {
    float temp = minTemp + (tempChunk * i);
    CRGB colour = colourFromTemperature(temp);
    tempPalette[i] = colour;
    tempPalette[z] = colour;
    z--;
  }
  UpscalePalette(tempPalette,scaledHeatPalette);
}

void Matrix::fillDigits_gradient(CRGB startColour, CRGB endColour, uint16_t startPos, double direction){

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
  
  uint8_t framesPerDrop = 6;
  // Fade down previous drops
  nscale8(rainLayer,NUM_LEDS,230);
  
  // Shift the drops down one row
  if (rainFrame > framesPerDrop){
    rainFrame = 0;
    for (int row=ROWS-1;row>0;row--){
      for (int col=0;col<COLUMNS;col++){
        rainDrops[row][col]=rainDrops[row-1][col];
      }
    }

    // Populate the top row with new raindrops
    for (int col=0;col<COLUMNS;col++){
      rainDrops[0][col] = random8() < chanceOfRain/4;
    }
  }

  rainFrame++;
  // Render the leading droplets into the layer
  for (int col=0;col<COLUMNS;col++){
    for (int row=0;row<ROWS;row++){
      if (rainDrops[row][col]){
        rainLayer[ XY(col,row) ] = colour;
      }
    }
  }
  
  // Fade the background colours on the main layer down
  nscale8_video(leds, NUM_LEDS, 255 - (chanceOfRain * 0.5));

  // And composite on the raindrops
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(255-(rainLayer[i].b * 0.9));
    leds[i] += rainLayer[i] ; 
  }
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

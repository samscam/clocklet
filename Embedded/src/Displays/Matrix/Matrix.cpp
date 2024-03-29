#if defined(MATRIX)

#include "Matrix.h"
#include "Fonts/MatrixFont.h"
#include "settings.h"
#include "MatrixUtils.h"
#include "TimeThings/TimeUtils.h"

#include <esp_log.h>
#define TAG "MATRIX"

FASTLED_USING_NAMESPACE

// This is a gradient palette for the temperature scale

DEFINE_GRADIENT_PALETTE (temperatureGPalette) {
0,   127, 127, 255, // -10  very light blue
51,  255, 255, 255, // 0    white
61,  127, 255, 255, // 2    light cyan
76,  0,   255, 255, // 5    cyan
102, 0,   255, 0,   // 10   green
127, 255, 255, 0,   // 15   yellow
153, 255, 128, 0,   // 20   orange
177, 255, 0,   0,   // 25   red
204, 255, 0,   32,  // 30   magenta
229, 255, 0,   128, // 35   magenta
255, 120, 0,   255  // 40   purple
};



Matrix::Matrix() : Display() {
  leds = leds_plus_safety_pixel + 1;
}

// PUBLIC METHODS


boolean Matrix::setup() {
  FastLED.addLeds<LED_TYPE,DATA_PIN,CLOCK_PIN,COLOR_ORDER >(leds, NUM_LEDS).setCorrection(0xE9A9FF);
  temperaturePalette = temperatureGPalette;
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

void Matrix::setDecimalTime(double decimalTime) {
  _decimalTime = decimalTime;
}

void Matrix::setTimeStyle(TimeStyle timeStyle){
  _timeStyle = timeStyle;
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
  setBrightness(1);
  
  // Red
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  // displayString("Red");
  FastLED.show();
  delay(500);

  // Green
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  // displayString("Green");
  FastLED.show();
  delay(500);

  // Blue
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  // displayString("Blue");
  FastLED.show();
  delay(500);

  fill_solid(leds, NUM_LEDS, CRGB::White);
  // displayString("Blue");
  FastLED.show();
  delay(5000);

  // SETUP FAKE FRAMERATE
  TickType_t xLastWakeTime = xTaskGetTickCount();
  TickType_t xFrequency = pdMS_TO_TICKS(1000/FPS);

  // Rainbow

  ulong startMillis = millis();
  uint8_t position = 0;
  while (millis()-startMillis < 3000){
    fill_matrix_radial_rainbow(leds,COLUMNS,ROWS,8,30,position,30);
    position++;
    FastLED.show();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

  }

  // Temperature gradients
  for (float f = -10; f<41; f+=0.1){
      CRGB colour = colourFromTemperature((float)f);
      setDigits(f,colour);
      FastLED.show();
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }

  // Heatwave
  xFrequency = pdMS_TO_TICKS(1000/FPS);
  CRGBPalette256 pal;
  
  regenerateHeatPalette(0,20);

  pal = scaledHeatPalette;
  position = 0;
  startMillis = millis();
  while (millis()-startMillis < 10000){
    fillMatrixWave(leds,COLUMNS,ROWS,position,1,pal);
    position++;
    FastLED.show();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }

  // Rainbow RAIN
  fract8 chance = 0;
  fract8 intensity = 0;
  bool done = false;
  uint8_t hue = 0;
  startMillis = millis();
  while (millis()-startMillis < 10000){
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    addRain(chance,intensity,CHSV(hue,255,255));
    hue++;

    FastLED.show();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    chance++;
    intensity++;
  }

  // Drizzle
  chance = 0;
  startMillis = millis();
  while (millis()-startMillis < 20000){
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    addDrizzle(chance,CRGB::Blue);
    hue++;

    FastLED.show();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    chance++;
  }


  // Snow
  startMillis = millis();
  chance = 0;
  intensity = 0;
  while (millis()-startMillis < 10000){
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    addSnow(chance,intensity);
    hue++;

    FastLED.show();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    chance++;
    intensity++;
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
  
  ESP_LOGI(TAG,"Scrolling: %s",stringy);

  int remainingCharacters = strlen(stringy);

  int charCount = 0;
  bool imageBuffer[255][5] = {};
  
  int lastGlyphWidth = 0;
  int shiftedSinceLastChar = 0;
  uint8_t step = 0;

  bool isScrolling = true;
  
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(1000/FPS);

  int millisPerStep = 75;
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




  _blinkColon = BLINK_SEPARATOR ? (time.second() % 2) == 0 : true;

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

  switch(_timeStyle) {
    case twentyFourHour:
      maskTime(time);
      setDot(_blinkColon,dotColour,8,colon);
      break;
    case twelveHour:
      maskTime(time);
      setDot(_blinkColon,dotColour,8,time.isPM() ? colon : point);
      break;
    case decimal:
      char buf[10];
      sprintf(buf,"%.3f",_decimalTime);
      displayString(buf);
      int decimalSecond = floor(_decimalTime * 10000) - (floor(_decimalTime * 1000)*10);
      _blinkColon = BLINK_SEPARATOR ? (decimalSecond % 2) == 0 : true;
      setDot(_blinkColon,dotColour,4,point);
      break;
  }

  // PRECIPITATION --------

   // Intensity is float somewhere representing mm/hour
   // 0 ..  we are going to top out at 10
  fract8 precipIntensity = min((weather.precipIntensity / 10.0f ) * 255.0f , float(255));

  // Probablility is a float from 0 to 1...
  // We are shaving off anything under a 20% chance of rain and calling that zero
  float minChance = 0.2f;
  float precipChanceF = weather.precipChance;
  precipChanceF = max(precipChanceF - minChance, 0.0f);
  precipChanceF = precipChanceF / (1.0f - minChance);
  fract8 precipChance = min(precipChanceF * 255.0f, float(255));

  if (precipChance > 0) {
    switch (weather.precipType) {
      case Drizzle:
        addDrizzle(precipChance, CRGB::Blue);
        break;
      case Rain:
        addRain(precipChance, precipIntensity, CRGB::Blue);
        break;
      case Sleet:
        addRain(precipChance, precipIntensity, CRGB::Gray);
        break;
      case Snow:
        addSnow(precipChance, precipIntensity);
        break;
    }
  }

  // float minTmp = weather.minTmp;
  // if (minTmp <= 0.0f){
  //   addFrost();
  // }

  if (weather.thunder){
    addLightening();
  }


  FastLED.show();

}


void Matrix::maskTime(const DateTime& time){
  int digit[4];

  int h;
  if (_timeStyle == twelveHour){
    h = time.hour() % 12 ;
    if (h == 0) {
      h = 12;
    }
  } else {
    h = time.hour() ;
  }
  
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

void Matrix::maskDate(const DateTime& time){
  int digit[4];

  int day = time.day();
  digit[0] = day/10;                      // left digit
  digit[1] = day - (day/10)*10;             // right digit

  int month = time.month();
  digit[2] = month/10;
  digit[3] = month - (month/10)*10;

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

void Matrix::setDot(bool state, CRGB colour, uint8_t column, SeparatorStyle style){

  uint8_t separatorMask = style;

  for (int y = 0;y<5;y++){

    uint8_t thisPixelVal = (uint8_t)(separatorMask << (7-y)) >> 7;

    if (!thisPixelVal){
      leds[ XYsafe(column,4-y) ] = CRGB::Black;
    }
    if (thisPixelVal && !state){
      leds[ XYsafe(column,4-y) ] = colour;
    }
  }

    //black column after the dots
  for (int n=0;n<5;n++){
    leds[ XYsafe(column+1,n) ] = CRGB::Black;
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
  fill_matrix_radial_rainbow(leds,COLUMNS,ROWS,8,30,cycle,20);
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

// Drizzle

void Matrix::addDrizzle( fract8 precipChance, CRGB colour)
{
  colour.nscale8_video(192);
  // Fade down previous drops
  nscale8(rainLayer,NUM_LEDS,220);
  
  rainFrame++;
  // Render the leading droplets into the layer
  for (int col=0;col<COLUMNS;col++){
    for (int row=0;row<ROWS;row++){
      if (random8() <  scale8(precipChance,8) )
        rainLayer[ XYsafe(col,row) ] = colour;
    }
  }

  // And composite on the raindrops
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8( 255 - scale8(rainLayer[i].b, 140) );
    leds[i] += rainLayer[i] ; 
  }
}


// Rain (also sleet)

void Matrix::addRain( fract8 precipChance, fract8 precipIntensity, CRGB colour)
{
  colour.nscale8_video(192);
  fract8 product = scale8(precipChance,precipIntensity);

  uint8_t framesPerDrop = scale8(255 - precipIntensity, 4) + 3;
  uint8_t fadeDown = scale8(precipIntensity, 30) + 200;

  // Fade down previous drops
  nscale8(rainLayer,NUM_LEDS,fadeDown);
  
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
      rainDrops[0][col] = random8() < scale8(product,128);
    }
  }

  rainFrame++;
  // Render the leading droplets into the layer
  for (int col=0;col<COLUMNS;col++){
    for (int row=0;row<ROWS;row++){
      if (rainDrops[row][col]){
        rainLayer[ XYsafe(col,row) ] = colour;
      }
    }
  }

  // And composite on the raindrops
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8( 255 - scale8( rainLayer[i].b, 140 ));
    leds[i] += rainLayer[i] ; 
  }
}

// Snow

void Matrix::addSnow( fract8 precipChance, fract8 precipIntensity ) {

  fract8 product = scale8(precipChance,precipIntensity);

  uint8_t framesPerDrop = scale8(255 - precipIntensity, 8) + 8;
  uint8_t fadeDown = scale8(precipIntensity, 30) + 140;

  // Fade down previous drops
  nscale8(rainLayer,NUM_LEDS,fadeDown);
  
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
      rainDrops[0][col] = random8() < scale8(product,64);
    }
  }

  rainFrame++;
  // Render the leading droplets into the layer
  for (int col=0;col<COLUMNS;col++){
    for (int row=0;row<ROWS;row++){
      if (rainDrops[row][col]){
        rainLayer[ XYsafe(col,row) ] = CRGB::Grey;
      }
    }
  }

  // And composite on the raindrops
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(255 - scale8(rainLayer[i].b, 160 ));
    leds[i] += rainLayer[i] ; 
  }
}

// Frost

void Matrix::addFrost(){
  CRGB frostLayer[NUM_LEDS];
  fill_solid(frostLayer, NUM_LEDS, CRGB::Black);
  // We want the bottom row and the next one up...

  for (int col=0;col<COLUMNS;col++){
    frostLayer[ XYsafe(col,4) ] = CHSV(0,0,60);
    frostLayer[ XYsafe(col,3)] = CHSV(0,0,30);
  }

  for(int i = 0; i < NUM_LEDS; i++) { 
    leds[i] += frostLayer[i] ; 
  }
}

// Lightening

void Matrix::addLightening(){
  
  fill_solid(lighteningLayer, NUM_LEDS, CRGB::Black);
  for(int i = 0; i < NUM_LEDS; i++) {
    lighteningLayer[i].nscale8(120);
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

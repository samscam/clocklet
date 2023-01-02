#if defined(RAINBOWDISPLAY)

#include "RGBDigit.h"

#include "../settings.h"

FASTLED_USING_NAMESPACE

// This is a gradient palette for the temperature scale
// I would rather it was in the private scope of the class in the header file
// But this macro doesn't seem to like doing that...
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


RGBDigit::RGBDigit() : Display() {


}

// PUBLIC METHODS


boolean RGBDigit::setup() {
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);  temperaturePalette = temperatureGPalette;
  initRain();
  regenerateHeatPalette(0.0,0.0);

  return true;
}

void RGBDigit::frameLoop() {
  displayTime(_time,_weather);
}

/* SETTERS */

void RGBDigit::setWeather(Weather weather) {
  _weather = weather;
  
  regenerateHeatPalette(_weather.minTmp,_weather.maxTmp);
}

void RGBDigit::setDecimalTime(double decimalTime) {
  _decimalTime = decimalTime;
}

void RGBDigit::setTimeStyle(TimeStyle timeStyle){
  _timeStyle = timeStyle;
}

void RGBDigit::setTime(DateTime time) {
  _time = time;
}

/* HIGH LEVEL DISPLAY METHODS */

void RGBDigit::displayTemperatures(){
  const char * message = "Currently";
  CRGB colour = colourFromTemperature(_weather.currentTmp);
  scrollText(message, colour);
  
  setDigits(_weather.currentTmp,colour);
  FastLED.delay(4000);
}


void RGBDigit::displayMessage(const char *stringy, MessageType messageType = good) {
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
    scrollText_randomColour(stringy);
    break;
  }
}

void RGBDigit::setStatusMessage(const char * string){
  fillDigits_rainbow(true);
  setDigits(string);
  FastLED.show();
}

void RGBDigit::setBrightness(float brightness){
  uint8_t scaledBrightness = MIN_BRIGHTNESS + (brightness * (MAX_BRIGHTNESS - MIN_BRIGHTNESS));
  FastLED.setBrightness(scaledBrightness);
}

void RGBDigit::setRainbows(bool newState){
  rainbows = newState;
}

void RGBDigit::setDeviceState(DeviceState newState){
  _deviceState = newState;
}

// PRIVATE


// MARK: SCROLLING TEXT

void RGBDigit::scrollText(const char *stringy){
  scrollText(stringy, CRGB::Green);
}

void RGBDigit::scrollText_fail(const char *stringy){
  scrollText(stringy, CRGB::Red);
}

void RGBDigit::scrollText_randomColour(const char *stringy){
  CHSV colour = CHSV(random8(), 255, 255);
  scrollText(stringy, colour);
}

void RGBDigit::scrollText(const char *stringy, CRGB colour){
  scrollText(stringy, colour, colour);
}

void RGBDigit::scrollText(const char *stringy, CRGB startColour, CRGB endColour) {
  char charbuffer[DIGIT_COUNT] = { 0 };
  int origLen = strlen(stringy);
  int extendedLen = origLen + DIGIT_COUNT;
  char res[extendedLen];
  memset(res, 0, extendedLen);
  memcpy(res,stringy,origLen);

  int i;
  for ( i = 0; i < extendedLen ; i++ ) {
    fillDigits_gradient(startColour,endColour);

    for ( int d = 0; d < DIGIT_COUNT ; d++ ) {
      if (d == 3) {
        charbuffer[d] = res[i];
      } else {
        charbuffer[d] = charbuffer[d+1];
      }

      setDigit(charbuffer[d], d);

    }

    FastLED.show();
    FastLED.delay(200);

  }
}


// MARK: DISPLAY TIME --------------------------------------


void RGBDigit::displayTime(const DateTime& time, Weather weather){

  // Advance the wind cycle
  advanceWindCycle(weather.windSpeed);

  if (rainbows){
    // Fill the digits entirely with rainbows
    fillDigits_rainbow(true);
  } else {
    // Or heat colours
    fillDigits_heat();
  }


  float minTmp = weather.minTmp;

  // PRECIPITATION --------

  // Intensity is somewhere in the range of 0 to 80 mm/hour but we are going to top out at 20
  float precip = (weather.precipIntensity / 20.0f) * 255.0f;
  precip = max(precip,0.0f);
  precip = min(precip,255.0f);
  fract8 intensity = fract8(precip);

  if (intensity > 0) {
    switch (weather.precipType) {
      case Snow:
        addSnow(intensity);
        break;
      case Drizzle:
        addRain(intensity, CRGB::Blue);
        break;
      case Rain:
        addRain(intensity, CRGB::Blue);
        break;
      case Sleet:
        addRain(intensity, CRGB::White);
        break;
    }
  }

  if (minTmp <= 0.0f){
    addFrost();
  }

  if (weather.thunder){
    addLightening();
  }

  _blinkColon = BLINK_SEPARATOR ? (time.second() % 2) == 0 : true;

  CRGB dotColour = CRGB::White;
  switch(_deviceState){
    case ok:
      dotColour = CRGB::White;
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
      setDot(_blinkColon,1,dotColour);
      break;
    case twelveHour:
      maskTime(time);
      setDot(_blinkColon,1,dotColour);
      break;
    case decimal:
      char buf[10];
      sprintf(buf,"%.3f",_decimalTime);
      setDigit(buf[0],0);
      setDigit(buf[2],1);
      setDigit(buf[3],2);
      setDigit(buf[4],3);
      int decimalSecond = floor(_decimalTime * 10000) - (floor(_decimalTime * 1000)*10);
      _blinkColon = BLINK_SEPARATOR ? (decimalSecond % 2) == 0 : true;
      setDot(_blinkColon,0,dotColour);
      break;
  }


  FastLED.show();

}

void RGBDigit::maskTime(const DateTime& time){
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


void RGBDigit::setDigit(int number, int digit){
  byte mask = _numberMasks[number];
  setDigitMask(mask,digit);
}



void RGBDigit::setDigit(char character, int digit){
  byte mask;
  if (character == 45){
    // minus
    mask = B00000010;
  } else if (character < 48) {
    mask = B00000000;
    // It's a control char
  } else if (character >= 48 && character <= 57) {
    mask = _numberMasks[character - 48];
    // Numbers
  } else if (character >= 65 && character <= 90) {
    // Uppercase
    mask = _charMasks[character-65];
  } else if (character >= 97 && character <= 122) {
    // Lowercase
    mask = _charMasks[character-97];
  } else {
    // out of range
    mask = B00000000;
  }

  setDigitMask(mask,digit);
}



void RGBDigit::setDigitMask(byte mask, int digit){
  int bit = 0;

  while (bit < 8) {
    if (mask & 0x01){
      //leds[ 7 - bit + (digit * DIGIT_SEGS) ] = CRGB::Crimson;
    } else {
      leds[ 7 - bit + (digit * DIGIT_SEGS) ] = CRGB::Black;
    }
    bit++;
    mask = mask >> 1;
  }
}

void RGBDigit::setDot(bool state, int digit, CRGB colour){
  leds[ 7 + (digit * DIGIT_SEGS)] = state ? colour : CRGB::Black ;
}

/// Display a string (up to the length of the display)
void RGBDigit::setDigits(const char *string){
  for (int i=0;i<DIGIT_COUNT;i++){
    setDigit(string[i],i);
  }
}

/// Display an integer
void RGBDigit::setDigits(int number, CRGB colour){
  fillDigits_gradient(colour,colour);
  bool negative = false;

  if (number < 0) {
    negative = true;
    number = number * -1;
  }

  for (int i=0;i<DIGIT_COUNT-1;i++){
    int fromRight = DIGIT_COUNT - i - 1;
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
/// This really doesn't work properly.
void RGBDigit::setDigits(float number, CRGB colour){
  fillDigits_gradient(colour,colour);

  // Decompose the float
  float integral, fractional;
  fractional = modff(number, &integral);

  int digit = 0;

  bool negative = (fractional < 0) ;
  if (negative){
    setDigit('-',digit);
    digit++;

    integral = fabsf(integral);
    fractional = fabsf(fractional);
  }

  int integralInt = integral;
  bool doneDot = false;
  for (;digit<DIGIT_COUNT;digit++){
    
    if (integralInt > 0){
      int first = integralInt;
      int exp = 0;
      while (first > 10){
        first = first / 10;
        exp ++;
      }
      setDigit(first,digit);
      integralInt -= first * pow(10,exp);
    } else {

      if (!doneDot){
        setDot(true,digit-1, colour);
        doneDot = true;
      }

      int firstFract = fractional * 10;

      setDigit(firstFract,digit);
      float dumper;
      fractional = modff(fractional*10,&dumper);

    }
  }

}
// WIND

void RGBDigit::advanceWindCycle(float speed){
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
  // Catch accidental infinity
  if (cycle > 255.0 || cycle < -255.0) {
    cycle = 0;
  }
}

void RGBDigit::fillDigits_rainbow(bool includePoints){

  uint8_t hue = cycle;

  if (includePoints){
    // when we include the points, we treat each digit as 4 columns of segments
    // the decimal point is a column in its own right
    CHSV cols[4*NUM_DIGITS];
    fill_rainbow( cols, 4*NUM_DIGITS, hue, 6);

    int mapping[] = {
      1,2,2,1,0,0,1,3
    };

    for (int i = 0; i < NUM_LEDS; i++){
      leds[i] = cols[ mapping[i%8] + ((i/8)*4) ];
    }

  } else {
    // treat each digit as 3 columns of segments
    // the decimal point is given the same colour as the right-most vertical segments
    CHSV cols[3*NUM_DIGITS];

    fill_rainbow( cols, 3*NUM_DIGITS, hue, 8);

    int mapping[] = {
      1,2,2,1,0,0,1,2
    };

    for (int i = 0; i < NUM_LEDS; i++){
      leds[i] = cols[ mapping[i%8] + ((i/8)*3) ];
    }
  }
}



CRGB RGBDigit::colourFromTemperature(float temperature){
  float min = -10.0;
  float max = 40.0;

  if (temperature < min) { temperature = min; }
  if (temperature > max) { temperature = max; }

  uint8_t scaled = (int)roundf(((temperature - min) / (max - min)) * 255.0);

  CRGB col = ColorFromPalette(temperaturePalette, scaled);
  return col;
}

void RGBDigit::fillDigits_heat(){

  uint8_t numCols = 3 * NUM_DIGITS;
  CRGB cols[3 * NUM_DIGITS];


  fill_palette(cols, numCols, cycle, 6, scaledHeatPalette, 255, LINEARBLEND);

  int mapping[] = {
    1,2,2,1,0,0,1,2
  };

  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = cols[ mapping[i%8] + ((i/8)*3) ];
  }
}

void RGBDigit::regenerateHeatPalette(float minTemp, float maxTemp){
  scaledHeatPalette = CRGBPalette16();
  float tempChunk = (maxTemp - minTemp) / 16.0f;
  for (int i = 0; i < 16; i++) {
    float temp = minTemp + (tempChunk * i);
    scaledHeatPalette[i] = colourFromTemperature(temp);
  }
}

void RGBDigit::fillDigits_gradient(CRGB startColour, CRGB endColour){
  uint8_t numCols = 3 * NUM_DIGITS;
  CRGB cols[3 * NUM_DIGITS];

  fill_gradient_RGB(cols , 0,  startColour, numCols-1, endColour);

  int mapping[] = {
    1,2,2,1,0,0,1,2
  };

  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = cols[ mapping[i%8] + ((i/8)*3) ];
  }
}

// Rain (also sleet)

/// Creates the mapping in `allvsegs` to the vertical segments in `leds`
/// This only needs to be done once on initialisation of the class
void RGBDigit::initRain(){
  int s = 0;

  for (int digit = 0; digit < NUM_DIGITS ; digit++) {
    for (int vseg = 0 ; vseg < 4 ; vseg++){
      int digStep = DIGIT_SEGS * digit;
      int val = vsegs[vseg] + digStep;
      allvsegs[s] = val;
      s++;
    }
  }
}

void RGBDigit::addRain( fract8 chanceOfRain, CRGB colour)
{
  for(int i = 0; i < NUM_LEDS; i++) {
    rainLayer[i].nscale8(230);
  }
  if( random8() < chanceOfRain) {
    int segnum = random8(4 * NUM_DIGITS);

    rainLayer[ allvsegs[segnum] ] = colour;
  }
  nscale8_video(leds, NUM_LEDS, 255 - (chanceOfRain * 0.7));

  for(int i = 0; i < NUM_LEDS; i++) { leds[i] += rainLayer[i] ; }
}

// Snow

void RGBDigit::addSnow( fract8 chanceOfSnow ) {
  for(int i = 0; i < NUM_LEDS; i++) {
    rainLayer[i].nscale8_video(252);
  }
  if( random8() < chanceOfSnow) {
    int segnum = random8(NUM_LEDS);
    rainLayer[ segnum ] = CRGB::White;
  }
  nscale8_video(leds, NUM_LEDS, 255 - chanceOfSnow);

  for(int i = 0; i < NUM_LEDS; i++) { leds[i] += rainLayer[i] ; }
}

// Frost

void RGBDigit::addFrost(){
  CRGB frostLayer[NUM_LEDS];
  fill_solid(frostLayer, NUM_LEDS, CRGB::Black);
  // We want the bottom row and the next one up...

  for (int d = 0; d < NUM_DIGITS; d++){
    frostLayer[ (d*8) + 3 ] = CHSV(0,0,200);
    frostLayer[ (d*8) + 2 ] = CHSV(0,0,90);
    frostLayer[ (d*8) + 4 ] = CHSV(0,0,90);
  }

  for(int i = 0; i < NUM_LEDS; i++) { leds[i] += frostLayer[i] ; }
}

// Lightening

void RGBDigit::addLightening(){
  for(int i = 0; i < NUM_LEDS; i++) {
    lighteningLayer[i].nscale8(150);
  }
  if(random8() == 1) {
    fill_solid(lighteningLayer, NUM_LEDS, CRGB::White);
  }
  for(int i = 0; i < NUM_LEDS; i++) { leds[i] += lighteningLayer[i] ; }
}

#endif

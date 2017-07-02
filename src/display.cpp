#include "display.h"

FASTLED_USING_NAMESPACE

// LDR pins
int lightPin = 0;
static const float min_brightness = 3;
static const float max_brightness = 150;

CRGB leds[NUM_LEDS];
CRGB rainLayer[NUM_LEDS];

// ----------- RANDOM MESSAGES

const char* messages[] = {
  "commit",
  "jez we can",
  "best nest",
  "aline is a piglet",
  "all your base are belong to us",
  "what time is it",
  "sarah is very nice",
  "do the ham dance",
  "bobbins",
  "sam is really clever",
  "i am not a hoover",
  "i like our new house",
  "i want to be more glitchy",
  "striving to be less shite",
  "just a baby fart",
  "sam is a sausage",
  "this is an art project",
  "minus excremento nitimur",
};

#define numMessages (sizeof(messages)/sizeof(char *)) //array size


void initDisplay(){
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // rgbDigit.begin();
  // rgbDigit.clearAll();
}

void randoMessage(){
  int messageIndex = random(0,numMessages-1);
  const char* randoMessage = messages[messageIndex];

  // Do it three times
  scrollText(randoMessage);
  scrollText(randoMessage);
  scrollText(randoMessage);
}

void scrollText(const char *stringy){
  scrollText(stringy, GREEN);
}

void scrollText_fail(const char *stringy){
  scrollText(stringy, RED);
}

void scrollText(const char *stringy, Colour colour){
  Serial.println(stringy);

  //clockDisplay.drawColon(0);
  char charbuffer[DIGIT_COUNT] = { 0 };
  int origLen = strlen(stringy);
  int extendedLen = origLen + DIGIT_COUNT;
  char res[extendedLen];
  memset(res, 0, extendedLen);
  memcpy(res,stringy,origLen);

  int i;
  for ( i = 0; i < extendedLen ; i++ ) {
    fill_solid(leds,NUM_LEDS,CRGB::Green);

    for ( int d = 0; d < DIGIT_COUNT ; d++ ) {
      if (d == 3) {
        charbuffer[d] = res[i];
      } else {
        charbuffer[d] = charbuffer[d+1];
      }

      setDigit(charbuffer[d], d);

    }

    FastLED.show();
    delay(200);

  }


}

// MARK: DISPLAY THINGS --------------------------------------

uint8_t hue = 0;

// Remember if the colon was drawn on the display so it can be blinked
// on and off every second.
bool blinkColon = false;

void displayTime(const DateTime& time, Colour colours[5]){
  hue ++;
  //rainChance ++;
  fill_rainbow( leds, NUM_LEDS, hue, 4);

  int digit[4];

  int h = time.hour();
  digit[0] = h/10;                      // left digit
  digit[1] = h - (h/10)*10;             // right digit

  int m = time.minute();
  digit[2] = m/10;
  digit[3] = m - (m/10)*10;

  for (int i = 0; i<4 ; i++){
    setDigit(digit[i], i); // show on digit 0 (=first). Color is rgb(64,0,0).
  }

  blinkColon = (time.second() % 2) == 0;
  setDot(blinkColon,1);

  FastLED.show();

  // if (blinkColon) {
  //   rgbDigit.clearDot(1);               // clear dot on digit 3 (=fourth)
  // } else {
  //   rgbDigit.showDot(1, colours[4].red, colours[4].green, colours[4].blue);    // show dot on digit 1 (=second). Color is rgb(64,0,0).
  // }
}


uint8_t brightness;

void updateBrightness(){
  float lightReading = analogRead(lightPin);

  float bRange = max_brightness - min_brightness;

  brightness = (lightReading * bRange / 1024.0f) + min_brightness;
  // Serial.println(brightness);
  FastLED.setBrightness(brightness);
  // rgbDigit.setBrightness(brightness);
}

//   0
// 5   1
//   6
// 4   2
//   3   7

const byte _numberMasks[10] = {
  B11111100,//0
  B01100000,//1
  B11011010,//2
  B11110010,//3
  B01100110,//4
  B10110110,//5
  B10111110,//6
  B11100000,//7
  B11111110,//8
  B11110110,//9
};

void setDigit(int number, int digit){
  byte mask = _numberMasks[number];
  setDigitMask(mask,digit);
}

const byte _charMasks[36] = {
  B11101110, // a
  B00111110, // b
  B00011010, // c
  B01111010, // d
  B10011110, // e
  B10001110, // f
  B11110110, // g
  B00101110, // h
  B00100000, // i
  B01110000, // j
  B01101111, // k
  B01100000, // l
  B00101011, // m crap
  B00101010, // n
  B00111010, // o
  B11001110, // p
  B11100110, // q
  B00001010, // r
  B10110110, // s
  B00011110, // t
  B00111000, // u
  B00110000, // v crap
  B00111001, // w crap
  B00110011, // x crap
  B01110110, // y
  B11011010, // z
};

void setDigit(char character, int digit){
  byte mask;
  if (character < 48) {
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



void setDigitMask(byte mask, int digit){
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

void setDot(bool state, int digit){
  leds[ 7 + (digit * DIGIT_SEGS)] = state ? CRGB::White : CRGB::Black ;
}

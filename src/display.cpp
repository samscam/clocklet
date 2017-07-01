#include "display.h"

FASTLED_USING_NAMESPACE

// LDR pins
int lightPin = 0;
static const float min_brightness = 7;
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
/*
  //clockDisplay.drawColon(0);
  char charbuffer[DIGIT_COUNT] = { 0 };
  int origLen = strlen(stringy);
  int extendedLen = origLen + DIGIT_COUNT;
  char res[extendedLen];
  memset(res, 0, extendedLen);
  memcpy(res,stringy,origLen);

  Serial.print("SCROLLING: ");

  int i;
  for ( i = 0; i < extendedLen ; i++ ) {
    for ( int d = 0; d < DIGIT_COUNT ; d++ ) {
      if (d == 3) {
        charbuffer[d] = res[i];
      } else {
        charbuffer[d] = charbuffer[d+1];
      }
      if (charbuffer[d] == 0){
        rgbDigit.clearDigit( d );
      } else {
        rgbDigit.setDigit(charbuffer[d], d, colour.red, colour.green, colour.blue);
      }
    }

    delay(200);

  }
*/

}

// MARK: DISPLAY THINGS --------------------------------------

uint8_t hue = 0;

// Remember if the colon was drawn on the display so it can be blinked
// on and off every second.
bool blinkColon = false;

void displayTime(const DateTime& time, Colour colours[5]){
  Serial.println("displaying time");
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

  FastLED.show();
  // blinkColon = (time.second() % 2) == 0;
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

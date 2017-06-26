#include "display.h"

// LDR pins
 int lightPin = 0;


// ----------- RANDOM MESSAGES

const char* messages[] = {
  "commit",
  "best nest",
  "aline is a piglet",
  "all your base are belong to us",
  "what time is it",
  "sarah is very nice",
  "do the ham dance",
  "bobbins",
  "sam is really clever",
  "i am not a hoover",
  "i want all the things",
  "i like our new house",
  "i want to be more glitchy",
  "striving to be less shite every nanosecond",
  "it was just a baby fart",
  "sam is a sausage. he should wake up."
};

#define numMessages (sizeof(messages)/sizeof(char *)) //array size

RGBDigit rgbDigit = RGBDigit(DIGIT_COUNT, 11);

void initDisplay(){
  rgbDigit.begin();
  rgbDigit.clearAll();
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

  //clockDisplay.drawColon(0);
  char charbuffer[DIGIT_COUNT] = { 0 };
  int origLen = strlen(stringy);
  int extendedLen = origLen + DIGIT_COUNT;
  char res[extendedLen];
  memset(res, 0, extendedLen);
  memcpy(res,stringy,origLen);

  Serial.print("SCROLLING: ");
  Serial.println(stringy);
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


}

// MARK: DISPLAY THINGS --------------------------------------

// Remember if the colon was drawn on the display so it can be blinked
// on and off every second.
bool blinkColon = false;

void displayTime(const DateTime& time, Colour colours[5]){
  int digit[4];

  int h = time.hour();
  digit[0] = h/10;                      // left digit
  digit[1] = h - (h/10)*10;             // right digit

  int m = time.minute();
  digit[2] = m/10;
  digit[3] = m - (m/10)*10;

  for (int i = 0; i<4 ; i++){
    rgbDigit.setDigit(digit[i], i, colours[i].red, colours[i].green, colours[i].blue); // show on digit 0 (=first). Color is rgb(64,0,0).
  }


  blinkColon = (time.second() % 2) == 0;
  if (blinkColon) {
    rgbDigit.clearDot(1);               // clear dot on digit 3 (=fourth)
  } else {
    rgbDigit.showDot(1, colours[4].red, colours[4].green, colours[4].blue);    // show dot on digit 1 (=second). Color is rgb(64,0,0).
  }
}


uint8_t brightness;

void updateBrightness(){
  uint32_t lightReading = analogRead(lightPin);

  brightness = (lightReading / 6.0) + 3;
  // Serial.println(brightness);

  rgbDigit.setBrightness(brightness);
}

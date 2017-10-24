#include "tests.h"
#include "display.h"
#include <FastLED.h>

#define FPS 60

void runDemo(){
    // rain
    // fract8 chance = 255;

    for (int16_t chance = 0; chance < 255; chance = chance + 16){
      for (int f = 0; f<60; f++){
        updateBrightness();
        fillDigits_rainbow(true);
        addRain(chance,CRGB::Blue);
        setDigits("rain");
        FastLED.show();
        FastLED.delay(1000/FPS);
      }
      Serial.println(chance);
    }

    for (int16_t chance = 0; chance < 255; chance = chance + 16){
      for (uint8_t f = 0; f<120; f++){
        updateBrightness();
        fillDigits_rainbow(true);
        addRain(chance,CRGB::White);
        setDigits("hail");
        FastLED.show();
        FastLED.delay(1000/FPS);
      }
    }

    for (int16_t chance = 0; chance < 255; chance = chance + 16){
      for (uint8_t f = 0; f<120; f++){
        updateBrightness();
        fillDigits_rainbow(true);
        addRain(chance,CRGB::Brown);
        setDigits("slet");
        FastLED.show();
        FastLED.delay(1000/FPS);
      }
    }

    for (int16_t chance = 0; chance < 255; chance = chance + 16){
      for (uint8_t f = 0; f<120; f++){
        updateBrightness();
        fillDigits_rainbow(true);
        addSnow(chance);
        setDigits("snow");
        FastLED.show();
        FastLED.delay(1000/FPS);
      }
    }
}

#include "tests.h"

#include <RTClib.h>
#include <FastLED.h>

void displayTests(RGBDigit *display){

    display->setStatusMessage("TEMP");
    FastLED.delay(1000);
    showTemperatureRange(display);

    display->setStatusMessage("FAST");
    FastLED.delay(1000);
    runFast(display);

}


void runFast(RGBDigit *display){
    
    DateTime time = DateTime(1978,12,28,0,0,0);
    DateTime endTime = DateTime(1978,12,29,0,0,0);
    while (time.unixtime() < endTime.unixtime()){
        display->setTime(time);
        display->frameLoop();
        time = time + TimeSpan(60);
    }

}

void showTemperatureRange(RGBDigit *display){
    for (int f = -10; f<41; f++){
        CRGB colour = display->colourFromTemperature((float)f);
        display->setDigits(f,colour);
        FastLED.show();
        FastLED.delay(500);
    }
}

// #include <FastLED.h>
//
// #define FPS 60
//
// void runDemo(){
//
//     // Temperature Colours
//

//
//
//     runDemo();
//     // rain
//     // fract8 chance = 255;
//     // for (int16_t speed = 0; speed < 50; speed += 1){
//     //   for (int f = 0; f<60; f++){
//     //     updateBrightness();
//     //     fillDigits_rainbow(true, speed);
//     //
//     //     setDigits("wind");
//     //     FastLED.show();
//     //     FastLED.delay(1000/FPS);
//     //   }
//     //   Serial.println(speed);
//     // }
// /*
//     for (int16_t chance = 0; chance < 255; chance = chance + 16){
//       for (int f = 0; f<60; f++){
//         updateBrightness();
//         fillDigits_rainbow(true, 0.0);
//         addRain(chance,CRGB::Blue);
//         setDigits("rain");
//         FastLED.show();
//         FastLED.delay(1000/FPS);
//       }
//       Serial.println(chance);
//     }
//
//     for (int16_t chance = 0; chance < 255; chance = chance + 16){
//       for (uint8_t f = 0; f<120; f++){
//         updateBrightness();
//         fillDigits_rainbow(true,0.0);
//         addRain(chance,CRGB::White);
//         setDigits("hail");
//         FastLED.show();
//         FastLED.delay(1000/FPS);
//       }
//     }
//
//     for (int16_t chance = 0; chance < 255; chance = chance + 16){
//       for (uint8_t f = 0; f<120; f++){
//         updateBrightness();
//         fillDigits_rainbow(true,0.0);
//         addSnow(chance);
//         setDigits("snow");
//         FastLED.show();
//         FastLED.delay(1000/FPS);
//       }
//     }
//     */
// }

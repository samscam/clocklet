// #include "tests.h"
// #include "Displays/Display.h"
// #include <FastLED.h>
//
// #define FPS 60
//
// void runDemo(){
//
//     // Temperature Colours
//
//     for (int f = -30; f<60; f++){
//       updateBrightness();
//
//       fillDigits_heat(0, f, f );
//
//       setDigits(f);
//
//       FastLED.show();
//       FastLED.delay(500);
//     }
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

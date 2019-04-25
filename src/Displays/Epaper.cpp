#include "Epaper.h"
#include "Fonts/Transport_Medium14pt7b.h"
#include "Fonts/Transport_Heavy40pt7b.h"
#include "Fonts/Transport_Heavy24pt7b.h"
#include "Fonts/Transport_Medium10pt7b.h"
#include <Esp.h>
#include <stdio.h>

EpaperDisplay::EpaperDisplay() : Display(),
display(GxEPD2_290(/*CS=5*/ 27, /*DC=*/ 33, /*RST=*/ 15, /*BUSY=*/ 19)),
time_string{'0', '0', ':', '0', '0', '\0'}
 {

}

boolean EpaperDisplay::setup() {
  display.init(115200);
  display.setRotation(1);
  display.setFont(&Transport_Heavy40pt7b);
  display.setTextColor(GxEPD_BLACK);
  return true;
}



// It's a clock of some sort... you have to implement this
// Time is passed by reference - the display should update on the next frame loop
void EpaperDisplay::setTime(DateTime time) {

  // Turn the time into a string
  // time_string = ;
  time_string[0] = time.hour() / 10 + '0';
  time_string[1] = time.hour() % 10 + '0';
  time_string[3] = time.minute() / 10 + '0';
  time_string[4] = time.minute() % 10 + '0';

  Serial.println((String)"*** TIME: " + time.hour() + ":" + time.minute() + ":" + time.second());
  needsDisplay = true;
}

// Implementation is optional
void EpaperDisplay::setWeather(Weather weather) {
  weather_string = weather.summary;
  Serial.println((String)"*** WEATHER: " + weather_string);
  needsDisplay = true;
  pageString(weather.summary);
}

// Show a message - but what kind of message?
void EpaperDisplay::displayMessage(const char *stringy) {
  Serial.println((String)"*** MESSAGE: " + stringy);
  pageString(stringy);
}

// Brightness is a float from 0 (barely visible) to 1 (really bright) - should it be a char?
void EpaperDisplay::setBrightness(float brightness) {
  // Do nothing. It's epaper :)
}

void EpaperDisplay::setBatteryVoltage(float newVoltage){
  voltage=newVoltage;
  needsDisplay = true;
}

// DISPLAY MANAGEMENT

// notifies the display to do another frame if it wants to do that kind of thing
void EpaperDisplay::frameLoop() {
  if (needsDisplay){
    needsDisplay = false;
    updateDisplay();
  }
}

void EpaperDisplay::updateDisplay(){

    display.setTextWrap(false);
    display.setPartialWindow(0, 0, display.width(), display.height());
    display.setFont(&Transport_Heavy40pt7b);
  // do this outside of the loop
  int16_t tbx, tby; uint16_t tbw, tbh;
  // center update text
  display.getTextBounds(time_string, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = (display.width() - tbw) / 2;
  uint16_t y;
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    y = (display.height() + tbh) / 2; // y is base line!
    display.setCursor(x,y);
    display.print(time_string);

    display.setFont(&Transport_Medium10pt7b);
    display.setCursor(10,display.height()-5);
    char buf[10];
    display.print((String) gcvt(voltage,3,buf) + " volts");

    int16_t wbx, wby; uint16_t wbw, wbh;
    display.getTextBounds(weather_string, 0, 0, &wbx, &wby, &wbw, &wbh);
    display.setCursor(display.width()-5-wbw,display.height()-5);
    display.print(weather_string);

    // Free heap
    // uint32_t heapSize = ESP.getHeapSize();
    // uint32_t freeHeap = ESP.getFreeHeap();
    // float freeRam = (float)freeHeap/(float)heapSize * 100.0f;
    // display.setCursor(10,18);
    // char buftwi[10];
    // display.print((String) gcvt(freeRam,3,buftwi) + "% Ram free");

  }
  while (display.nextPage());
  display.hibernate();
}



void EpaperDisplay::scrollString(const char *string){
  clear();

  display.setTextWrap(false);
  display.setFont(&Transport_Heavy40pt7b);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(string, 0, 0, &tbx, &tby, &tbw, &tbh);

  tbw += 10; // to avoid clipping
  tbh += 10; // to avoid clipping

  // Let's try scrolling :)

  GFXcanvas1 *canvas = new GFXcanvas1(tbw, tbh);
  canvas->setTextWrap(false);
  canvas->setFont(&Transport_Heavy40pt7b);
  canvas->fillScreen(0);
  canvas->setTextColor(1);
  canvas->setCursor(0,tbh-10);
  canvas->print(string);

  uint16_t scrollpos = 0;

  int16_t x, y;
  y = 20;

  display.setPartialWindow(0, y, display.width(), tbh);
  while (scrollpos < display.width() + tbw + 20){
      x = display.width() - scrollpos;

      //Serial.println((String)"*** coords: " + x + "," + y + "--" + " tbw " + tbw + " tbh: " + tbh);
      do
      {


        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(x, y, canvas->getBuffer(), tbw, tbh, GxEPD_BLACK, GxEPD_WHITE);

      }
      while (display.nextPage());
      // display.writeScreenBuffer(); // use default for white
        //display.writeImage(canvas->getBuffer(), x, y, tbw, tbh);
        // display.refresh(true);
      scrollpos += 30;
  }

  delete canvas;
  clear();
  needsDisplay = true;
  // display.hibernate();
}

void EpaperDisplay::pageString(const char *string){

  clear();

  display.setTextWrap(false);
  display.setFont(&Transport_Heavy24pt7b);
  display.setPartialWindow(0, 0, display.width(), display.height());


  char stringCopy[127];
  snprintf(stringCopy, strlen(string)+1, "%s\n", string);

  char * pch;
  printf ("Splitting string \"%s\" into tokens:\n",stringCopy);
  pch = strtok (stringCopy," ,.-");

  int16_t tbx, tby; uint16_t tbw, tbh;

  while (pch != NULL)
  {
    printf ("%s\n",pch);


    display.getTextBounds(pch, 0, 0, &tbx, &tby, &tbw, &tbh);

    do
    {
      display.fillScreen(GxEPD_WHITE);
      display.setCursor((display.width()-tbw)/2,((display.height()-tbh)/2) + tbh);
      display.print(pch);
    }
    while (display.nextPage());


    pch = strtok (NULL, " ,.-");

    delay(1000);

  }


  needsDisplay = true;
  // display.hibernate();
}

void EpaperDisplay::clear(){
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
  }
  while (display.nextPage());
}

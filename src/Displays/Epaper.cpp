#if defined(EPAPER)

#include "Epaper.h"
#include "Fonts/Transport_Medium14pt7b.h"
#include "Fonts/Transport_Heavy40pt7b.h"
#include "Fonts/Transport_Heavy30pt7b.h"
#include "Fonts/Transport_Heavy24pt7b.h"
#include "Fonts/Transport_Medium10pt7b.h"
#include <Esp.h>
#include <stdio.h>
#include <math.h>

/*
PINS for the Huzzah32
Busy (purple) 19/MISO
RST (white) 15
DC (green) 33
CS (orange) 27
CLK (yellow) SCK/5
DIN (blue) 18/MOSI
Gnd (black) GND
VCC (red) 3v
*/

EpaperDisplay::EpaperDisplay() : Display(),
    display(GxEPD2_290(/*CS=5*/ 27, /*DC=*/ 33, /*RST=*/ 15, /*BUSY=*/ 19)),
    time_string{'0', '0', ':', '0', '0', '\0'},
    secondary_time_string{'0', '0', ':', '0', '0', '\0'}{}

boolean EpaperDisplay::setup() {
  display.init(115200);
  display.setRotation(1);
  display.mirror(false);
  display.setFont(&Transport_Heavy40pt7b);
  display.setTextColor(GxEPD_BLACK);
  return true;
}


// - the display should update on the next frame loop
void EpaperDisplay::setTime(DateTime time) {
  this->time = time;
  // Turn the time into a string
  // time_string = ;
  time_string[0] = time.hour() / 10 + '0';
  time_string[1] = time.hour() % 10 + '0';
  time_string[3] = time.minute() / 10 + '0';
  time_string[4] = time.minute() % 10 + '0';

  Serial.println((String)"*** TIME: " + time.hour() + ":" + time.minute() + ":" + time.second());
  needsDisplay = true;
}

void EpaperDisplay::setSecondaryTime(DateTime time, const char *identifier) {

  // Turn the time into a string
  // time_string = ;
  secondary_time_string[0] = time.hour() / 10 + '0';
  secondary_time_string[1] = time.hour() % 10 + '0';
  secondary_time_string[3] = time.minute() / 10 + '0';
  secondary_time_string[4] = time.minute() % 10 + '0';

  secondary_identifier = identifier;

  Serial.println(secondary_time_string);
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
void EpaperDisplay::displayMessage(const char *stringy, MessageType messageType){
  Serial.println((String)"*** MESSAGE: " + stringy);
  pageString(stringy);
}

void EpaperDisplay::setBatteryLevel(float newLevel){
  batteryLevel=newLevel;
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
  // displayOctogram();
  displayDigital();
  display.hibernate();
}

void EpaperDisplay::displayDigital(){
  display.setTextWrap(false);
  display.setPartialWindow(0, 0, display.width(), display.height());


  display.setFont(&Transport_Heavy40pt7b);
    
  int16_t itemX, itemY; uint16_t itemW, itemH;
  
  uint16_t x;
  uint16_t y;

  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);

    y = 5;

    // Battery level
    display.setFont(&Transport_Medium10pt7b);

    int level = (int)floor(batteryLevel * 100);
    String blev = ((String) level + "%");
    display.getTextBounds(blev, 0, 0, &itemX, &itemY, &itemW, &itemH);
    x = 10;
    // x = (display.width() - itemW) / 2;
    y += itemH;
    display.setCursor(x,y);
    display.print(blev);
 
    y += 5;

    // Weather status
    display.setFont(&Transport_Medium14pt7b);
    display.getTextBounds(weather_string, 0, 0, &itemX, &itemY, &itemW, &itemH);
    x = (display.width() - itemW) / 2;

    y = 5;
    y += itemH;
    display.setCursor(x,y);
    display.print(weather_string);

    y += 10;

    // Main Clock
    display.setFont(&Transport_Heavy40pt7b);
    display.getTextBounds(time_string, 0, 0, &itemX, &itemY, &itemW, &itemH);
    x = (display.width() - itemW) / 2;
    y += itemH;
    display.setCursor(x,y);
    display.print(time_string);
    
    // y += 10;
    // // Bottom half black
    // display.fillRect(0,y,display.width(),display.height()-y,GxEPD_BLACK);


    // y += 7;
    // // Secondary clock - inverted
    // display.setFont(&Transport_Medium14pt7b);
    // String secStr = (String) secondary_identifier + (String) " " + (String) secondary_time_string;
    // display.getTextBounds(secStr, 0, 0, &itemX, &itemY, &itemW, &itemH);
    // x = (display.width() - itemW) / 2;
    // y += itemH;
    // display.setCursor(x,y);
    // display.setTextColor(GxEPD_WHITE);
    // display.print(secStr);

    // display.setTextColor(GxEPD_BLACK);

    // Free heap
    // uint32_t heapSize = ESP.getHeapSize();
    // uint32_t freeHeap = ESP.getFreeHeap();
    // float freeRam = (float)freeHeap/(float)heapSize * 100.0f;
    // display.setCursor(10,18);
    // char buftwi[10];
    // display.print((String) gcvt(freeRam,3,buftwi) + "% Ram free");

  }
  while (display.nextPage());

}

void EpaperDisplay::displayOctogram(){
  display.setPartialWindow(0, 0, display.width(), display.height());

  display.firstPage();
  do
  {
    // double secondAngle = ((double) time.second() / 60.0f) * M_PI * 2;
    // fillArcBox(0,0,display.width(),display.height(),secondAngle);

    float minuteAngle = (((float) time.minute() / 60.0f) + ((float) time.second() / 3600.0f)) * M_PI * 2;
    fillArcBox(0,0,display.width(),display.height(),minuteAngle);

    float hourAngle = ((((float)(time.hour() % 12) + (float) time.minute() / 60.0f)) / 12.0f) * M_PI * 2;
    fillArcBox(30,30,display.width()-60,display.height()-60,hourAngle);

    // display.setFont(&Transport_Medium14pt7b);
    // display.setCursor(10,20);
    // display.print(time_string);
  }
  
  while (display.nextPage());
}

void EpaperDisplay::fillArcBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, float arc){

    uint16_t xc = width/2;
    uint16_t yc = height/2;
    
    uint16_t xcentre = xc + x;
    uint16_t ycentre = yc + y;

    float alphaAngle = atan2((float)yc,(float)xc);
    float betaAngle = M_PI_2 - alphaAngle;
    float sigmaAngle = betaAngle;

    display.fillRect(x,y,width,height,GxEPD_WHITE);

    
    int quadrant = 0;
    while (arc > M_PI_2) {
      switch (quadrant){
        case 0:
          display.fillRect(xcentre,y,xc,yc,GxEPD_BLACK);
          sigmaAngle = alphaAngle;
          break;
        case 1:
          display.fillRect(xcentre,ycentre,xc,yc,GxEPD_BLACK);
          sigmaAngle = betaAngle;
          break;
        case 2:
          display.fillRect(x,ycentre,xc,yc,GxEPD_BLACK);
          sigmaAngle = alphaAngle;
          break;
        case 3:
          // should never actually happen
          display.fillRect(x,y,xc,yc,GxEPD_BLACK);
          sigmaAngle = betaAngle;
          break;
      }
      
      arc -= M_PI_2;
      quadrant++;
    }
    
    uint16_t xphi = floor(tan(arc) * (float)yc);
    uint16_t yphi = floor(tan(arc) * (float)xc);
    uint16_t yoo = floor(tan(M_PI_2-arc)*(float)xc);
    uint16_t xoo = floor(tan(M_PI_2-arc)*(float)yc);
    
    if (arc <= sigmaAngle){
      switch (quadrant) {
        case 0:
          display.fillTriangle(xcentre,ycentre,xcentre,y,xcentre+xphi,y,GxEPD_BLACK);
          break;
        case 1:
          display.fillTriangle(xcentre,ycentre,width+x,ycentre,width+x,ycentre+yphi,GxEPD_BLACK);
          break;
        case 2:
          display.fillTriangle(xcentre,ycentre,xcentre,height+y,xcentre-xphi,height+y,GxEPD_BLACK);
          break;
        case 3:
          display.fillTriangle(xcentre,ycentre,x,ycentre,x,ycentre-yphi,GxEPD_BLACK);
          break;
      }
    } else {
      switch (quadrant){
        case 0:
          display.fillTriangle(xcentre,ycentre,xcentre,y,x+width,y,GxEPD_BLACK);
          display.fillTriangle(xcentre,ycentre,x+width,y,x+width,ycentre-yoo,GxEPD_BLACK);
          break;
        case 1:
          display.fillTriangle(xcentre,ycentre,x+width,ycentre,x+width,y+height,GxEPD_BLACK);
          display.fillTriangle(xcentre,ycentre,x+width,y+height,xcentre+xoo,y+height,GxEPD_BLACK);
          break;
        case 2:
          display.fillTriangle(xcentre,ycentre,xcentre,y+height,x,y+height,GxEPD_BLACK);
          display.fillTriangle(xcentre,ycentre,x,y+height,x,ycentre+yoo,GxEPD_BLACK);
          break;
        case 3:
          display.fillTriangle(xcentre,ycentre,x,ycentre,x,y,GxEPD_BLACK);
          display.fillTriangle(xcentre,ycentre,x,y,xcentre-xoo,y,GxEPD_BLACK);
          break;
      }
    }
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
}

void EpaperDisplay::clear(){
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
  }
  while (display.nextPage());
}

#endif
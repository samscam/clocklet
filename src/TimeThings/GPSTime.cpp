#if defined(TIME_GPS)

#include "GPSTime.h"
#include <TinyGPS++.h>

#define _ss Serial2

boolean RTC_GPS::begin(void){
  _ss.begin(9600);
  delay(2000);
  _ss.println(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  _ss.println(PMTK_SET_NMEA_UPDATE_1HZ);

  return true;
}

void RTC_GPS::loop(){
  if (needsWake){
    wake();
  }

  while (_ss.available() > 0) {
    if(_gps.encode(_ss.read())){
      displayInfo();
    }
  }

  // if (millis() > 5000 && _gps.charsProcessed() < 10)
  // {
  //   Serial.println(F("No GPS detected: check wiring."));
  //   while(true);
  // }
}

void RTC_GPS::displayInfo()
{
  Serial.print(F("Location: "));
  if (_gps.location.isValid())
  {
    Serial.print(_gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(_gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (_gps.date.isValid())
  {
    Serial.print(_gps.date.month());
    Serial.print(F("/"));
    Serial.print(_gps.date.day());
    Serial.print(F("/"));
    Serial.print(_gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (_gps.time.isValid())
  {
    if (_gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(_gps.time.hour());
    Serial.print(F(":"));
    if (_gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(_gps.time.minute());
    Serial.print(F(":"));
    if (_gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(_gps.time.second());
    Serial.print(F("."));
    if (_gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(_gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
// SIGNAL(TIMER0_COMPA_vect) {
//   char c = GPS.read();
//   // if you want to debug, this is a good time to do it!
// #ifdef UDR0
//   if (GPSECHO)
//     if (c) UDR0 = c;
//     // writing direct to UDR0 is much much faster than Serial.print
//     // but only one character can be written at a time.
// #endif
// }


void RTC_GPS::adjust(const DateTime& dt){
  // do nothing
}

DateTime RTC_GPS::now(){
  TinyGPSDate date = _gps.date;
  TinyGPSTime time = _gps.time;

  return DateTime(date.year(),date.month(),date.day(),time.hour(),time.minute(),time.second());
}

void RTC_GPS::sleep(){
  _ss.println("PMTK161,0*28");
  needsWake = true;
}
void RTC_GPS::wake(){
  _ss.println("w");
  needsWake = false;
}

#endif
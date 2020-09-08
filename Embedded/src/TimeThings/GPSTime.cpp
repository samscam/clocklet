#if defined(TIME_GPS)

#include "GPSTime.h"
#include <TinyGPS++.h>
#include <esp_log.h>

#define _ss Serial2
#define TAG "GPS"

boolean RTC_GPS::begin(void){
  _ss.begin(9600);
  delay(2000);
  _ss.println(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  _ss.println(PMTK_SET_NMEA_UPDATE_10HZ);

  return true;
}

void RTC_GPS::loop(){
  if (needsWake){
    ESP_LOGI(TAG,"Woke GPS");
    wake();
  }
  
  while (_ss.available() > 0) {
    if(_gps.encode(_ss.read())){
      
    }
  }

}

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


// MARK: Conformance to LocationSource protocol

Location RTC_GPS::location(){
  Location location = {};
  location.lat = _gps.location.lat();
  location.lng = _gps.location.lng();
  return location;
}

#endif
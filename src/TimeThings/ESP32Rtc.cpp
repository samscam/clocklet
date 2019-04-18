#include "ESP32Rtc.h"
#include "time.h"


boolean RTC_ESP32::begin(void){
  configTime(0*3600,1*3600,"pool.ntp.org");
  return true;
}
void RTC_ESP32::adjust(const DateTime& dt){
  // do nothing
}

DateTime RTC_ESP32::now(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("failed to obtain time");
    return DateTime(0);
  }

  return DateTime(timeinfo.tm_year,timeinfo.tm_mon,timeinfo.tm_mday,timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
}

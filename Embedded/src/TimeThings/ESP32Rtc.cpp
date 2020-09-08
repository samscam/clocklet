#if defined(TIME_ESP32)

#include "ESP32Rtc.h"
#include <stdlib.h>

#include "esp_system.h"

#include "time.h"
#include "lwip/apps/sntp.h"

#include "Zones.h"
#include "esp_log.h"

#define TAG "RTCESP32"

boolean RTC_ESP32::begin(void){
  if(sntp_enabled()){
      sntp_stop();
  }

  char *server0 = strdup("0.pool.ntp.org");
  char *server1 = strdup("1.pool.ntp.org");
  char *server2 = strdup("2.pool.ntp.org");

  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, server0);
  sntp_setservername(1, server1);
  sntp_setservername(2, server2);
  sntp_init();

  return true;
}

void RTC_ESP32::setTimeZone(const char* newTimeZone){
  ESP_LOGI(TAG, "Looking up %s",newTimeZone);
  int zonesLength = sizeof(zones)/sizeof(char*);
  const char* posixCode = nullptr;
  int x;
  for(x=0;x<zonesLength;x=x+2){
    ESP_LOGV(TAG, "... Matching %s",zones[x]);
    if (strcmp(zones[x],newTimeZone) == 0){
      posixCode = zones[x+1];
      break;
    }
  }
  if (posixCode != nullptr){
    ESP_LOGI(TAG, "New timezone code is %s",posixCode);
    setenv( "TZ", posixCode, 1 );
    tzset();
  }
}

void RTC_ESP32::adjust(const DateTime& dt){
  timeval tv = {(long)dt.unixtime(),0};
  timezone tz = {0,0};
  settimeofday(&tv,&tz);
}

DateTime RTC_ESP32::now(){
  timeval tv;
  timezone tz = {0,0};
  gettimeofday(&tv,&tz);
  return DateTime(tv.tv_sec);
}

DateTime RTC_ESP32::localTime(){
  time_t now;
  time(&now);
  tm timeStruct;
  localtime_r(&now,&timeStruct);
  return DateTime(timeStruct.tm_year, timeStruct.tm_mon, timeStruct.tm_mday, timeStruct.tm_hour,
                   timeStruct.tm_min, timeStruct.tm_sec);
}

// Returns the local time of day in decimal representation as a double
// 24 hours = 1.0
double RTC_ESP32::decimalTime(){
  timeval tv;
  gettimeofday(&tv,NULL);
  
  tm timeStruct;
  localtime_r(&tv.tv_sec,&timeStruct);

  
  int msSinceStartOfDay = timeStruct.tm_hour * 60 * 60 * 1000;
  msSinceStartOfDay += timeStruct.tm_min * 60 * 1000;
  msSinceStartOfDay += timeStruct.tm_sec * 1000;
  msSinceStartOfDay += tv.tv_usec / 1000;
  
  
  double val = msSinceStartOfDay / 8640000.0;
  return val;
    
}

#endif
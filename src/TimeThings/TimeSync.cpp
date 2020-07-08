#include "TimeSync.h"

TimeSync::TimeSync(RTC_DS3231 *ds3231, RTC_ESP32 *esp32) : UpdateJob(){
    _ds3231 = ds3231;
    _esp32 = esp32;
}

  // Conformance to UpdateJob
bool TimeSync::performUpdate(){
    if (syncCount == 0){
        ds3231toesp();
    } else {
        esptods3231();
    }
    syncCount++;
    return true;
}

void TimeSync::esptods3231(){
    
  // Hourly sync the system (ntp) time back to the ds3231
  
  // Wait until we hit the next second boundary
  Serial.println("Syncing time: esp32 >> ds3231");
  uint32_t u32 = _esp32->now().unixtime();
  while (_esp32->now().unixtime() == u32){
    delay(1);
  }

  DateTime timertc = _esp32->now();
  _ds3231->adjust(timertc);

  char ds3231_buf[64] = "DDD, DD MMM YYYY hh:mm:ss";
  char esp32_buf[64] =  "DDD, DD MMM YYYY hh:mm:ss";
  Serial.printf("Sync complete... time is:\n - ds3231: %s\n - esp32: %s\n",_ds3231->now().toString(ds3231_buf),_esp32->now().toString(esp32_buf));
}

void TimeSync::ds3231toesp(){
  // Sync the ESP time to the DS3231 time on the next second boundary...
  Serial.println("Syncing ds3231 >> esp32 time");
  DateTime time3231 = _ds3231->now();
  while (_ds3231->now() == time3231){
    delay(1);
  }
  time3231 = _ds3231->now();
  _esp32->adjust(time3231);

  char ds3231_buf[64] = "DDD, DD MMM YYYY hh:mm:ss";
  char esp32_buf[64] =  "DDD, DD MMM YYYY hh:mm:ss";
  ESP_LOGI(TAG,"Sync complete... time is:\n - ds3231: %s\n - esp32: %s\n",time3231.toString(ds3231_buf),_esp32->now().toString(esp32_buf));

}

void TimeSync::variationCheck(){
    // Check for major variations in the time > 1 minute
  // This often happens after a (delayed) NTP sync or when GPS gets a fix
  // TimeSpan timeDiff = time - lastTime;
  // if (timeDiff.totalseconds() > 60 || timeDiff.totalseconds() < -60 ) {
  //   needsDaily = true;
  // }

}
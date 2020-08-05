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

/* ESP32 calibration experiment
#include "soc/rtc.h"
#define CALIBRATE_ONE(cali_clk) calibrate_one(cali_clk, #cali_clk)
#include "soc/rtc.h"
static uint32_t calibrate_one(rtc_cal_sel_t cal_clk, const char *name)
{

    const uint32_t cal_count = 1000;
    const float factor = (1 << 19) * 1000.0f;
    uint32_t cali_val;
    printf("%s:\n", name);
    for (int i = 0; i < 5; ++i)
    {
        printf("calibrate (%d): ", i);
        cali_val = rtc_clk_cal(cal_clk, cal_count);
        printf("%.3f kHz\n", factor / (float)cali_val);
    }
    return cali_val;
}
*/

 // Serial.println((String) "Slow clock freq" + rtc_clk_slow_freq_get_hz());
  //ESP32 Oscillator fudge
  // rtc_clk_32k_bootstrap(512);
  // rtc_clk_32k_bootstrap(512);
  // rtc_clk_32k_enable(true);
  // delay(500);
  // uint32_t cal_32k = CALIBRATE_ONE(RTC_CAL_32K_XTAL);
  // rtc_clk_slow_freq_set(RTC_SLOW_FREQ_RTC);//RTC_SLOW_FREQ_32K_XTAL);
  // Serial.println((String) "Slow clock freq" + rtc_clk_slow_freq_get_hz());
  // delay(2000);
  // if (cal_32k == 0)
  // {
  //    printf("32K XTAL OSC has not started up");
  // }
  // else
  // {
  //    printf("done\n");
  // }
  //
  // if (rtc_clk_32k_enabled())
  // {
  //    Serial.println("OSC Enabled");
  // }

#endif
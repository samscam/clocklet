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
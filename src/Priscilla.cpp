#include "Priscilla.h"
#include "tests.h"
#include "settings.h"
#include "Messages.h"
#include "soc/rtc.h"


// CONFIGURATION  --------------------------------------

// Time zone adjust (in hours from utc)
int32_t tzAdjust = 0;

// Set to false to display time in 12 hour format, or true to use 24 hour:

// ----------- Display
// RGBDigit display = RGBDigit();
//Adafruit7 display = Adafruit7();

// #include "Displays/DebugDisplay.h"
// DebugDisplay display = DebugDisplay();

#include "Displays/Epaper.h"
EpaperDisplay display = EpaperDisplay();

// ----------- RTC

//RTC_DS3231 rtc = RTC_DS3231();

// #include "TimeThings/ESP32Rtc.h"
// RTC_ESP32 rtc = RTC_ESP32();

#include "TimeThings/GPSTime.h"
RTC_GPS rtc = RTC_GPS();


#define CALIBRATE_ONE(cali_clk) calibrate_one(cali_clk, #cali_clk)

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

// ---------- Networking

// #if defined(ESP32) // Oh dear - it works differently
// WiFiClientSecure client;
// #else
WiFiClient client;
// #endif

MetOffice *weatherClient = new MetOffice(client);




// SETUP  --------------------------------------

void setup() {
  delay(2000);
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
 while (!Serial) {
   ; // wait for serial port to connect. Needed for native USB port only
 }

  rtc.begin();

  delay(2000);
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

  analogReadResolution(12);
  uint16_t seed = analogRead(A0);
  randomSeed(seed);
  Serial.println((String)"Seed: " + seed);
  for (int i = 0; i<10 ; i++){
    updateBrightness();
  }

  Serial.println("Clock starting!");
  Wire.begin();

  display.setup();
  display.displayMessage("Everything is awesome");

  while (!setupWifi()){}
}

// LOOP  --------------------------------------
unsigned long lastUpdateTime = 0;
unsigned long nextUpdateDelay = 0;
unsigned long lastRandomMessageTime = millis();
unsigned long nextMessageDelay = 1000 * 60 * 2;
unsigned long lastDailyUpdate = 0;
unsigned int fuzz = random(5,300);

DateTime lastTime = 0;

void loop() {
  // updateBrightness();
  rtc.loop();

  // This should always be UTC
  DateTime time = rtc.now();

  // adjust for timezone and DST
  time = time + TimeSpan(dstAdjust(time) * 3600);
  time = time + TimeSpan(tzAdjust * 3600);

  boolean needsDaily = false;
  boolean needsHourly = false;
  boolean needsMinutely = false;

  // Check for major variations in the time > 1 minute
  TimeSpan timeDiff = time - lastTime;
  if (timeDiff.totalseconds() > 60 || timeDiff.totalseconds() < -60 ) {
    needsDaily = true;
  }

  //Daily update
  if ( time.unixtime() > lastDailyUpdate + (60 * 60 * 24) + fuzz ) {
    updatesDaily();
    lastDailyUpdate = time.unixtime();
  }

  // Hourly updates
  if (millis() > lastUpdateTime + nextUpdateDelay){
    updatesHourly();
    lastUpdateTime = millis();
    nextUpdateDelay = 1000 * 60 * 60; // 60 mins
  }

  if (millis() > lastRandomMessageTime + nextMessageDelay){
    const char* message = randoMessage();
    display.displayMessage(message);
    lastRandomMessageTime = millis();
    nextMessageDelay = 1000 * 60 * random(5,59);
  }

  // Seconds precision updates
  // if (time.unixtime() > lastTime.unixtime()){
  //   display.setTime(time);
  //   lastTime = time;
  // }

  // Minutes precision updates
  // Will fail when starting at zero :/
  if (time.minute() != lastTime.minute()){
    display.setTime(time);
    display.setBatteryVoltage(batteryVoltage());
    lastTime = time;

    display.frameLoop();
    espSleep(58 - time.second() );
  }

  // display.frameLoop();
  //
  delay(1000/FPS);

  // FastLED.delay(1000/FPS);

}

void espSleep(int seconds){
  rtc.sleep();
  esp_sleep_enable_timer_wakeup(seconds * 1000 * 1000 ); // 58 seconds sounds nice
  esp_light_sleep_start();

}

// MARK: UPDATE CYCLE ---------------------------------------

void updatesHourly(){
  Serial.println("Hourly update");

  if (connectWifi()) {
    weatherClient -> timeThreshold = (rtc.now().hour() * 60) - 180;
    if (weatherClient -> fetchWeather()){
      display.setWeather(weatherClient->latestWeather);
    }

  }
}

void updatesDaily(){
  Serial.println("Daily update");
  // if (connectWifi()) {
  //   updateRTCTimeFromNTP();
  // }
  generateDSTTimes(rtc.now().year());
}

DateTime dstStart;
DateTime dstEnd;

void generateDSTTimes(uint16_t year){
  // last sunday in march at 01:00 utc
  DateTime eom = DateTime(year, 3, 31);
  int lastSun = 31 - (eom.dayOfTheWeek() % 7);
  dstStart = DateTime(year, 3, lastSun , 1);

  // last sunday in october at 01:00 utc
  DateTime eoo = DateTime(year, 10, 31);
  lastSun = 31 - eoo.dayOfTheWeek();
  dstEnd = DateTime(year, 10, lastSun , 1);

  Serial.println(dstStart.unixtime());
  Serial.println(dstEnd.unixtime());
}

uint16_t dstAdjust(DateTime time){
  if (time.unixtime() >= dstStart.unixtime() && time.unixtime() < dstEnd.unixtime() ) {
    return 1;
  } else {
    return 0;
  }
}




// brightness sensing

int lightPin = 0;
const int readingWindow = 10;
float readings[readingWindow] = {4096.0f};
int readingIndex = 0;

static const float min_brightness = 10;
static const float max_brightness = 200;

void updateBrightness(){
  readings[readingIndex] = analogRead(lightPin);
  readingIndex++;
  if (readingIndex == readingWindow) { readingIndex = 0; }
  float sum = 0;
  for (int loop = 0 ; loop < readingWindow; loop++) {
    sum += readings[loop];
  }
  float lightReading = sum / readingWindow;

  float bRange = max_brightness - min_brightness;

  float brightness = (lightReading * bRange / 4096.0f) + min_brightness;
  display.setBrightness(brightness);
}



#if defined(BATTERY_MONITORING)
float batteryVoltage(){
  float reading = analogRead(BATTERY_PIN);
  return (reading / 4095.0f) * 2.0f * 3.3f * 1.1;
}
#endif

#include "Priscilla.h"
#include "tests.h"
#include "settings.h"
#include "Messages.h"
#include "Displays/Display.h"

// CONFIGURATION  --------------------------------------

// Time zone adjust (in MINUTES from utc)
int32_t tzAdjust = 0;
int32_t secondaryTimeZone = 330; // Mumbai is +5:30

// Set to false to display time in 12 hour format, or true to use 24 hour:

// ----------- Display
// RGBDigit display = RGBDigit();
//Adafruit7 display = Adafruit7();

// #include "Displays/DebugDisplay.h"
// DebugDisplay display = DebugDisplay();

#include "Displays/Epaper.h"
Display *display = new EpaperDisplay();

// ----------- RTC

//RTC_DS3231 rtc = RTC_DS3231();

// #include "TimeThings/ESP32Rtc.h"
// RTC_ESP32 rtc = RTC_ESP32();

#include "TimeThings/GPSTime.h"
RTC_GPS rtc = RTC_GPS();

// ---------- Networking

// WiFiClientSecure client; // << https on esp32
WiFiClient client; // <<  plain http, and https on atmelwinc

MetOffice *weatherClient = new MetOffice(client);

// WeatherClient *weatherClient = new 


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


  analogReadResolution(12);
  uint16_t seed = analogRead(A0);
  randomSeed(seed);
  Serial.println((String)"Seed: " + seed);
  for (int i = 0; i<10 ; i++){
    updateBrightness();
  }

  Serial.println("Clock starting!");
  Wire.begin();

  display->setup();
  display->displayMessage("Everything is awesome");
  setupWifi();
}

// LOOP  --------------------------------------


unsigned long lastRandomMessageTime = millis();
unsigned long nextMessageDelay = 1000 * 60 * 2;

unsigned long lastHourlyUpdate = 0;
unsigned long lastDailyUpdate = 0;

DateTime lastTime = 0;

void loop() {
  // updateBrightness();
  rtc.loop();

  // This should always be UTC
  DateTime time = rtc.now();

  bool needsDaily = false;

  // Check for major variations in the time > 1 minute
  // TimeSpan timeDiff = time - lastTime;
  // if (timeDiff.totalseconds() > 60 || timeDiff.totalseconds() < -60 ) {
  //   needsDaily = true;
  // }
  if ( time.unixtime() > lastDailyUpdate + (60 * 60 * 24)) {
    needsDaily = true;
  }

  //Daily update
  if (needsDaily){
    updatesDaily();
    time = rtc.now();
    lastDailyUpdate = time.unixtime();
  }

  // Hourly updates
  if ( time.unixtime() > lastHourlyUpdate + (60 * 60)){
    updatesHourly();
    time = rtc.now();
    lastHourlyUpdate = time.unixtime();
  }

  if (millis() > lastRandomMessageTime + nextMessageDelay){
    const char* message = randoMessage();
    display->displayMessage(message);
    lastRandomMessageTime = millis();
    nextMessageDelay = 1000 * 60 * random(5,59);
  }


  // Minutes precision updates
  // Will fail when starting at zero :/
  if (time.minute() != lastTime.minute()){
    DateTime displayTime;
    // adjust for timezone and DST
    displayTime = time + TimeSpan(dstAdjust(time) * 3600);
    displayTime = displayTime + TimeSpan(tzAdjust * 60);
    
    display->setTime(displayTime);

    // secondary time
    displayTime = time; //+ TimeSpan(dstAdjust(time) * 3600); -- no dst in india
    displayTime = displayTime + TimeSpan(secondaryTimeZone * 60);
    display->setSecondaryTime(displayTime,"Mumbai");

    float voltage = batteryVoltage();
    display->setBatteryLevel(batteryLevel(voltage));

    if (voltage < cutoffVoltage){
      espShutdown();
    }

    lastTime = time;
    display->frameLoop();
    
    espSleep(59 - time.second() );
  }

  // delay(50);
  // delay(1000/FPS);
  // FastLED.delay(1000/FPS);

}

void espSleep(int seconds){
  rtc.sleep();
  esp_sleep_enable_timer_wakeup(seconds * 1000 * 1000 ); // 58 seconds sounds nice
  esp_light_sleep_start();

}

void espShutdown(){
  display->setStatusMessage("LOW BATTERY");
  Serial.println("LOW BATTERY shutting down");
  esp_deep_sleep_start();
}

// MARK: UPDATE CYCLE ---------------------------------------

void updatesHourly(){
  Serial.println("Hourly update");

  if (connectWifi()) {
    weatherClient -> timeThreshold = (rtc.now().hour() * 60) - 180;
    if (weatherClient -> fetchWeather()){
      display->setWeather(weatherClient->latestWeather);
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

int lightPin = A2;
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
  display->setBrightness(brightness);
}



#if defined(BATTERY_MONITORING)

float batteryLevel(float voltage){
  float v = (voltage - cutoffVoltage) / (maxVoltage - cutoffVoltage);
  v = fmax(0.0f,v); // make sure it doesn't go negative
  return fmin(1.0f, v); // cap the level at 1
}

float batteryVoltage(){
  float reading = analogRead(BATTERY_PIN);
  return (reading / 4095.0f) * 2.0f * 3.3f * 1.1;
}

#endif

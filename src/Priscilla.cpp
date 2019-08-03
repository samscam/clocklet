#include "Priscilla.h"
#include "tests.h"
#include "settings.h"
#include "Messages.h"
#include "Displays/Display.h"
#include "Location/LocationSource.h"

#include "TimeThings/NTP.h"

// CONFIGURATION  --------------------------------------

// Time zone adjust (in MINUTES from utc)
int32_t tzAdjust = 0;
int32_t secondaryTimeZone = 330; // Mumbai is +5:30

// ----------- Display

#if defined(RAINBOWDISPLAY)

#include "Displays/RGBDigit.h"
Display *display = new RGBDigit();

#elif defined(EPAPER)

#include "Displays/Epaper.h"
Display *display = new EpaperDisplay();

#endif

// RGBDigit display = RGBDigit();
//Adafruit7 display = Adafruit7();

// #include "Displays/DebugDisplay.h"
// DebugDisplay display = DebugDisplay();



// ----------- RTC



#if defined(TIME_GPS)
#include "TimeThings/GPSTime.h"
RTC_GPS rtc = RTC_GPS();
LocationSource locationSource = rtc;

#elif defined(TIME_DS3231)
RTC_DS3231 rtc = RTC_DS3231();

#elif defined(TIME_ESP32)
#include "TimeThings/ESP32Rtc.h"
RTC_ESP32 rtc = RTC_ESP32();

#endif





// ---------- Networking

WiFiClientSecure client; // << https on esp32
// WiFiClient client; // <<  plain http, and https on atmelwinc


// ---------- WEATHER CLIENT

// MetOffice *weatherClient = new MetOffice(client);
WeatherClient *weatherClient = new DarkSky(client);

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

  // seed the brightness
  for (int i = 0; i<10 ; i++){
    currentBrightness();
    delay(100);
  }

  Serial.println("Clock starting!");
  Wire.begin();

  display->setup();
  display->setBrightness(currentBrightness());
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
  display->setBrightness(currentBrightness());

  // Check for touches...
  if (detectTouchPeriod() > 500){
    display->displayTemperatures();//displayMessage("That tickles",rando);
  }

#if defined(TIME_GPS)
  rtc.loop(); //<< needed on the GPS rtc to wake it :/
#endif
  // This should always be UTC
  DateTime time = rtc.now();

  bool needsDaily = false;

  // Check for major variations in the time > 1 minute
  // This often happens after a (delayed) NTP sync or when GPS gets a fix
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
    Serial.println("Random message");
    const char* message = randoMessage();
    display->displayMessage(message, rando);
    lastRandomMessageTime = millis();
    nextMessageDelay = 1000 * 60 * random(5,59);
  }


  // Minutes precision updates
  // Will fail when starting at zero :/
  // if (time.minute() != lastTime.minute()){

    DateTime displayTime;
    // adjust for timezone and DST
    displayTime = time + TimeSpan(dstAdjust(time) * 3600);
    displayTime = displayTime + TimeSpan(tzAdjust * 60);
    
    display->setTime(displayTime);

    // secondary time
    // displayTime = time; //+ TimeSpan(dstAdjust(time) * 3600); -- no dst in india
    // displayTime = displayTime + TimeSpan(secondaryTimeZone * 60);
    // display->setSecondaryTime(displayTime,"Mumbai");

#if defined(BATTERY_MONITORING)
    float voltage = batteryVoltage();
    display->setBatteryLevel(batteryLevel(voltage));

    if (voltage < cutoffVoltage){
      
      espShutdown();
    }

#endif


    lastTime = time;

    // display->frameLoop();
    
    // espSleep(59 - time.second() );
  // }

  // delay(50);
  // delay(1000/FPS);
  display->frameLoop();
  FastLED.delay(1000/FPS);

}


// MARK: UPDATE CYCLE ---------------------------------------

void updatesHourly(){
  Serial.println("Hourly update");

  if (reconnect()) {
    weatherClient -> timeThreshold = (rtc.now().hour() * 60) - 180;
    if (weatherClient -> fetchWeather()){
      display->setWeather(weatherClient->latestWeather);
    }
  }
}

void updatesDaily(){
  Serial.println("Daily update");
  #if defined(TIMESOURCE_NTP)
  if (reconnect()) {
    DateTime ntpTime;
    if (timeFromNTP(ntpTime)){
      rtc.adjust(ntpTime);
      // display->displayMessage("Synchronised with NTP", good);
    } else {
      // display->displayMessage("No sync", bad);
    }
  }
  #endif
  generateDSTTimes(rtc.now().year());
}

DateTime dstStart;
DateTime dstEnd;

void generateDSTTimes(uint16_t year){
  // European DST rules:
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

  // Making this work anywhere is going to be more complex.
}

uint16_t dstAdjust(DateTime time){
  if (time.unixtime() >= dstStart.unixtime() && time.unixtime() < dstEnd.unixtime() ) {
    return 1;
  } else {
    return 0;
  }
}


// MARK: BRIGHTNESS SENSING -------------------------

int lightPin = A2;
const int readingWindow = 10;
uint16_t readings[readingWindow] = {1024};
int readingIndex = 0;

float currentBrightness(){
  readings[readingIndex] = analogRead(lightPin);
  readingIndex++;
  if (readingIndex == readingWindow) { readingIndex = 0; }
  uint16_t sum = 0; // The sum can be a 16 bit integer because
  // 4096 goes into 2^16 - 16 times and we are doing 10 readings
  for (int loop = 0 ; loop < readingWindow; loop++) {
    sum += readings[loop];
  }

  uint16_t lightReading = sum / (uint16_t)readingWindow;
  return lightReading / 4096.0f;

}

// MARK: TOUCH SENSITIVITY ------------------------
long startTouchMillis = 0;

/// Returns the number of ms which the user has been touching the device
long detectTouchPeriod(){
  int touchValue = touchRead(T6);

  if (touchValue < 45){
    if (!startTouchMillis){
      startTouchMillis = millis();
    }
    return millis() - startTouchMillis;
  } else {
    startTouchMillis = 0;
    return 0;
  }

}

// MARK: POWER MANAGEMENT -------------------------

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


#if defined(ESP32)
void espSleep(int seconds){
  stopWifi();
  #if defined(TIME_GPS)
  rtc.sleep();
  #endif

  esp_sleep_enable_timer_wakeup(seconds * 1000 * 1000 ); // 58 seconds sounds nice
  esp_light_sleep_start();

}

void espShutdown(){
  display->setStatusMessage("LOW BATTERY");
  Serial.println("LOW BATTERY shutting down");
  esp_deep_sleep_start();
}
#endif
#include "Priscilla.h"
#include "Tests/Tests.h"
#include "settings.h"
#include "Messages.h"
#include "Displays/Display.h"
#include "Location/LocationSource.h"
#include "Location/LocationManager.h"
#include <WiFi.h>

#include "Bluetooth/BlueStuff.h"

#include "FirmwareUpdates/FirmwareUpdates.h"
#include <Preferences.h>
#include "Weather/Rainbows.h"

#include "Loggery.h"
#include "TimeThings/NTP.h"

#include "rom/uart.h"
#include <soc/efuse_reg.h>

#include "TimeThings/TimeSync.h"

#include "ClockletSystem.h"

#define TAG "PRISCILLA"


// ----------- Display

#if defined(RAINBOWDISPLAY)

#include "Displays/RGBDigit.h"
RGBDigit display = RGBDigit();

#elif defined(MATRIX)

#include "Displays/Matrix.h"
Matrix display = Matrix();

// #elif defined(EPAPER)

// #include "Displays/Epaper.h"
// Display *display = new EpaperDisplay();

#endif


//Adafruit7 display = Adafruit7();

// #include "Displays/DebugDisplay.h"
// Display *display = new DebugDisplay();

// ----------- RTC

// We will always use the internal ESP32 time now...
#include "TimeThings/ESP32Rtc.h"
RTC_ESP32 rtc = RTC_ESP32();


#if defined(TIME_GPS)
#include "TimeThings/GPSTime.h"
RTC_GPS gpsRTC = RTC_GPS();
#endif

#if defined(TIME_DS3231)
RTC_DS3231 ds3231 = RTC_DS3231();
#endif

#if defined(LOCATION_GPS)
LocationSource locationSource = rtc;
#else
LocationManager *locationManager;
#endif


// ---------- WEATHER CLIENT

WiFiClientSecure secureClient; // << https on esp32
// WiFiClient client; // <<  plain http, and https on atmelwinc

// #include "weather/met-office.h"
// WeatherClient *weatherClient = new MetOffice(client); // << It's plain HTTP

#include "weather/darksky.h"
DarkSky weatherClient = DarkSky();


// RAINBOWS

Rainbows rainbows;

// Global Notification queues

QueueHandle_t prefsChangedQueue;
QueueHandle_t weatherChangedQueue;
QueueHandle_t locationChangedQueue;
QueueHandle_t networkChangedQueue;
QueueHandle_t networkStatusQueue;
QueueHandle_t firmwareUpdateQueue;

UpdateScheduler updateScheduler = UpdateScheduler();

BlueStuff *blueStuff;

// SETUP  --------------------------------------
void setup() {


  delay(2000);
  
  Serial.begin(115200);
  LOGMEM;
  // Say hello on Serial port
  Serial.println("");
  Serial.println("   ------------    ");
  Serial.println("  /            \\   ");
  Serial.println("  --------------   ");
  Serial.println(" |              |   ");
  Serial.println(" | ! CLOCKLET ! |   ");
  Serial.println(" |              |   ");
  Serial.println("  --------------   \n");

  Serial.printf("Firmware Version: %s (%s)\n",VERSION,GIT_HASH);

  // Notification queues
  prefsChangedQueue = xQueueCreate(1, sizeof(bool));
  weatherChangedQueue = xQueueCreate(1, sizeof(bool));
  locationChangedQueue = xQueueCreate(1, sizeof(bool));
  networkChangedQueue = xQueueCreate(1, sizeof(bool));
  networkStatusQueue =  xQueueCreate(1, sizeof(wl_status_t));
  firmwareUpdateQueue = xQueueCreate(1, sizeof(FirmwareUpdateStatus));

  // Read things from eFuse

  uint16_t hwrev = clocklet_hwrev();
  uint16_t caseColour = clocklet_caseColour();
  uint32_t serial = clocklet_serial();

  Serial.printf("Serial number: %d\n",serial);
  Serial.printf("Hardware revision: %d\n",hwrev);
  Serial.printf("Case Colour: %d\n",caseColour);

  // Read things from preferences...
  Preferences preferences = Preferences();
  preferences.begin("clocklet", false);

  // Post-update migrations
  String swmigrev = preferences.getString("swmigrev","0.0.0");

  if (swmigrev != VERSION){
    Serial.printf("Previously running: %s\n",swmigrev.c_str());
    preferences.putString("swmigrev",VERSION);
    Serial.println("MIGRATED!");
  }

  String owner = preferences.getString("owner","");
  Serial.printf("Owner: %s\n",owner.c_str());

  preferences.end();

  Serial.println("");
  LOGMEM;

  analogReadResolution(12);
  analogSetPinAttenuation(LIGHT_PIN,ADC_0db);

  // Randomise the random seed - Not sure if this is random enough
  // We don't actually need to do this if the wireless subsystems are active
  uint16_t seed = analogRead(A0);
  randomSeed(seed);
  Serial.println((String)"Seed: " + seed);

  // seed the brightness
  for (int i = 0; i<10 ; i++){
    currentBrightness();
    delay(100);
  }

  ESP_LOGI(TAG,"Clock starting!");


  display.setup();
  display.setBrightness(currentBrightness());


  // Uncomment to run various display tests:
  // displayTests(display);  

  // DISPLAY A GREETING
  display.displayMessage("CLOCKLET",rainbow);

  updateDisplayPreferences();

  WiFi.begin();

  locationManager = new LocationManager(locationChangedQueue);


  #if defined(CLOCKBRAIN)
  blueStuff = new BlueStuff(prefsChangedQueue,networkChangedQueue,networkStatusQueue,locationManager);
  blueStuff->startBlueStuff();
  #endif


  Location currentLocation = locationManager->getLocation();
  weatherClient.weatherChangedQueue = weatherChangedQueue;
  rainbows.setLocation(currentLocation);
  weatherClient.setLocation(currentLocation);
  weatherClient.setTimeHorizon(12);


  rtc.setTimeZone(currentLocation.timeZone);

  // Setup DS3231
      // Initialise I2c stuff (DS3231)
  #if defined(TIME_DS3231)

  #if defined(CLOCKBRAIN)
  Wire.begin(22, 21); // Got the damn pins the wrong way round on clockbrain
  #else
  Wire.begin();
  #endif

  if (!ds3231.begin()){
    Serial.println("Could not connect to DS3231");
  }

  #endif
  TimeSync *timeSync = new TimeSync(&ds3231, &rtc); // << deliberately leaking these here - should really go smart pointers
  timeSync->performUpdate();

  // Start the internal RTC and NTP sync
  rtc.begin();

  FirmwareUpdates *firmwareUpdates = new FirmwareUpdates(firmwareUpdateQueue);

  // Start Update Scheduler
  updateScheduler.addJob(&weatherClient,hourly);
  updateScheduler.addJob(firmwareUpdates,daily);
  updateScheduler.addJob(timeSync,hourly);

  updateScheduler.start();

}

// LOOP  --------------------------------------


unsigned long lastRandomMessageTime = millis();
unsigned long nextMessageDelay = 1000 * 60 * 2;

unsigned long lastHourlyUpdate = 0;
unsigned long lastDailyUpdate = 0;


enum Precision {
  minutes, seconds, subseconds
};

Precision precision = subseconds;
// #if defined(RAINBOWDISPLAY)
// Precision precision = subseconds;
// #else
// Precision precision = minutes;
// #endif

bool didDisplay = false;

FirmwareUpdateStatus fwUpdateStatus = idle;
bool fwUpdateStarted = false;

void loop() {
  
  display.setBrightness(currentBrightness());

  // ---- MONITOR QUEUES -----

  // ... preferences
  bool prefsDidChange = false;
  xQueueReceive(prefsChangedQueue, &prefsDidChange, (TickType_t)0 );
  if (prefsDidChange){
    Serial.println("PREFS DID CHANGE");
    updateDisplayPreferences();

  }

  // ... weather
  bool weatherDidChange = false;
  xQueueReceive(weatherChangedQueue, &weatherDidChange, (TickType_t)0 );
  if (weatherDidChange){
    Serial.println("Weather did change");
    display.setWeather(weatherClient.horizonWeather);
    rainbows.setWeather(weatherClient.rainbowWeather);
  }

  // ... location
  bool locationDidChange = false;
  xQueueReceive(locationChangedQueue, &locationDidChange, (TickType_t)0 );
  if (locationDidChange){
    Serial.println("locationDidChange did change");
    Location location = locationManager->getLocation();
    weatherClient.setLocation(location);
    rainbows.setLocation(location);
    rtc.setTimeZone(location.timeZone);
  }


  // ... firmware updates
  
  xQueueReceive(firmwareUpdateQueue, &fwUpdateStatus, (TickType_t)0 );

  switch (fwUpdateStatus) {
    case idle:
      break;
    case updating:
      if (!fwUpdateStarted){
        display.displayMessage("Updating Firmware", rando);
        fwUpdateStarted = true;
      } else {
        display.setStatusMessage("WAIT");
      }
      
      delay(100);
      return;

      break;
    case failed:
      display.displayMessage("Update failed... sorry :(",bad);
      fwUpdateStatus = idle;
      break;
    case complete:
      display.displayMessage("Update complete! Restarting...", good);
      ESP.restart();
      break;
  }

  #if defined(FEATHER)
  // Check for touches... (This doesn't work on the clockbrain ... but it doesn't need to work!)
  if (detectTouchPeriod() > 500){
    display.displayMessage("That tickles",rando);
  }
  if (detectTouchPeriod() > 5000){
    blueStuff = new BlueStuff(prefsChangedQueue,networkChangedQueue,networkStatusQueue,locationManager);
    blueStuff->startBlueStuff();

    display.setDeviceState(bluetooth);
    display.displayMessage("Bluetooth is on",good);
  }
  if (detectTouchPeriod() > 10000){
    display.displayMessage("Keep holding for restart",bad);
  }
  if (detectTouchPeriod() > 15000){
    ESP.restart();
  }
  #endif

  // BATTERY MONITORING
  #if defined(BATTERY_MONITORING)
      float voltage = batteryVoltage();
      display.setBatteryLevel(batteryLevel(voltage));

      if (voltage < cutoffVoltage){
        
        espShutdown();
      }
  #endif

  #if defined(TIME_GPS)
    rtc.loop(); //<< needed on the GPS rtc to wake it :/
  #endif

  // This should always be UTC
  DateTime time = rtc.now();


  if (millis() > lastRandomMessageTime + nextMessageDelay){
    Serial.println("Random message");
    const char *message = randoMessage();
    display.displayMessage(message, rainbow);
    display.displayMessage(message, rainbow);

    lastRandomMessageTime = millis();
    nextMessageDelay = 1000 * 60 * random(5,59);
  }


  DateTime localTime = rtc.localTime();
  display.setRainbows(rainbows.rainbowProbability(time));
  display.setTime(localTime);
  display.frameLoop();

  sensibleDelay(1000/FPS);


}

void updateDisplayPreferences(){
  Preferences preferences = Preferences();
  preferences.begin("clocklet", false);
  
  String timeStyleString = preferences.getString("time_style");
  ESP_LOGI(TAG,"NEW TIME STYLE***** %s",timeStyleString);
  if (timeStyleString == "24 Hour"){
    display.setTimeStyle(twentyFourHour);
  } else if (timeStyleString == "12 Hour"){
    display.setTimeStyle(twelveHour);
  } else if (timeStyleString == "Decimal"){
    display.setTimeStyle(decimal);
  }

  preferences.end();

}

void sensibleDelay(int milliseconds){

    FastLED.delay(milliseconds);

}


// MARK: BRIGHTNESS SENSING -------------------------

const int readingWindow = 10;
uint16_t readings[readingWindow] = {1024};
int readingIndex = 0;

float currentBrightness(){
  readings[readingIndex] = analogRead(LIGHT_PIN);
  readingIndex++;
  if (readingIndex == readingWindow) { readingIndex = 0; }
  uint16_t sum = 0; // The sum can be a 16 bit integer because
  // 4096 goes into 2^16 - 16 times and we are doing 10 readings
  for (int loop = 0 ; loop < readingWindow; loop++) {
    sum += readings[loop];
  }

  float lightReading = sum / (float)readingWindow;

  lightReading = lightReading / 4096.0f;
  #if defined(CLOCKBRAIN) // actually this should be the display type
  lightReading = lightReading * 10.0f; // Boost the level somewhat
  #endif
  lightReading = lightReading > 1.0f ? 1.0f : lightReading;

  return lightReading;

}

// MARK: TOUCH SENSITIVITY ------------------------
long startTouchMillis = 0;
uint16_t touchThreshold = 43;

/// Returns the number of ms which the user has been touching the device
long detectTouchPeriod(){
  int touchValue = touchRead(TOUCH_PIN);


  if (touchValue < touchThreshold){ // touch threshold is a mess
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
// This stuff is for the one battery powered device I've built - mostly irrelevant at the moment

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
void espSleep(int milliseconds){
  
  stopWifi();

  #if defined(TIME_GPS)
  rtc.sleep();
  #endif
  Serial.println("SLEEP");
  uart_tx_wait_idle(0);
  uint64_t microseconds = milliseconds * 1000;

  esp_err_t err = esp_sleep_enable_timer_wakeup( microseconds );
  if (err == ESP_ERR_INVALID_ARG){
    Serial.println("Sleep timer wakeup invalid");
    return;
  }
  err = esp_light_sleep_start();

  if (err == ESP_ERR_INVALID_STATE){
    Serial.println("Trying to sleep: Invalid state error");
  }
  Serial.println("AWAKE");
}

void espShutdown(){
  // display.setStatusMessage("LOW BATTERY");
  // Serial.println("LOW BATTERY shutting down");
  // esp_deep_sleep_start();
}
#endif

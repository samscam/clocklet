#include "main.h"

#include "Tests/Tests.h"
#include "settings.h"
#include "Messages.h"
#include "Displays/Display.h"
#include "Location/LocationSource.h"
#include "Location/LocationManager.h"
#include <WiFi.h>
#include <esp_log.h>

#include "Bluetooth/BlueStuff.h"

#include "FirmwareUpdates/FirmwareUpdates.h"
#include <Preferences.h>


#include "Loggery.h"

#include <rom/uart.h>
#include <soc/efuse_reg.h>

#include "TimeThings/TimeSync.h"

#include "ClockletSystem.h"

#define TAG "PRISCILLA"

// ----------- Display

#if defined(RAINBOWDISPLAY)

#include "Displays/RGBDigit.h"
RGBDigit display = RGBDigit();

#elif defined(MATRIX)

#include "Displays/Matrix/Matrix.h"
Matrix display = Matrix();

#elif defined(EPAPER)

#include "Displays/Epaper.h"
Display *display = new EpaperDisplay();

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

// TimeSync
TimeSync *timeSync;

// Location

#if defined(LOCATION_GPS)
LocationSource locationSource = rtc;
#else
LocationManager *locationManager;
#endif


// ---------- WEATHER CLIENT

// // #include "weather/met-office.h"
// // WeatherClient *weatherClient = new MetOffice(client); // << It's plain HTTP

// #include "weather/darksky.h"
// DarkSky weatherClient = DarkSky();

#include "weather/openweathermap.h"
OpenWeatherMap *weatherClient;

// RAINBOWS
#include "Weather/Rainbows.h"
Rainbows rainbows;

// Global Notification queues
QueueHandle_t bluetoothConnectedQueue;
QueueHandle_t prefsChangedQueue;
QueueHandle_t weatherChangedQueue;
QueueHandle_t locationChangedQueue;
QueueHandle_t networkChangedQueue;
QueueHandle_t networkStatusQueue;
QueueHandle_t firmwareUpdateQueue;
QueueHandle_t godModeQueue;

// God Mode
GodModeSettings godModeSettings;

// Firmware updater
FirmwareUpdates *firmwareUpdates;

// Update scheduler
UpdateScheduler updateScheduler = UpdateScheduler();

// Bluetooth implementation
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
    ESP_LOGI(TAG,"Previously running: %s\n",swmigrev.c_str());
    preferences.putString("swmigrev",VERSION);
    ESP_LOGI(TAG,"MIGRATED!");
  }

  preferences.end();

  LOGMEM;

  // Notification queues
  bluetoothConnectedQueue = xQueueCreate(1, sizeof(bool));
  prefsChangedQueue = xQueueCreate(1, sizeof(bool));
  weatherChangedQueue = xQueueCreate(1, sizeof(bool));
  locationChangedQueue = xQueueCreate(1, sizeof(bool));
  networkChangedQueue = xQueueCreate(1, sizeof(bool));
  networkStatusQueue =  xQueueCreate(1, sizeof(wl_status_t));
  firmwareUpdateQueue = xQueueCreate(1, sizeof(FirmwareUpdateStatus));
  godModeQueue = xQueueCreate(1, sizeof(GodModeSettings));

  // Analog input for light pin
  analogReadResolution(12);
  analogSetPinAttenuation(LIGHT_PIN,ADC_0db);

  // seed the brightness
  for (int i = 0; i<10 ; i++){
    currentBrightness();
    delay(100);
  }

  ESP_LOGI(TAG,"Clock starting!");


  display.setup();
  display.setBrightness(currentBrightness());


  // Uncomment to run various display tests:
  // displayTests(&display);  

  // DISPLAY A GREETING
  display.displayMessage("CLOCKLET",rainbow);

  updateDisplayPreferences();

  WiFi.begin();


  locationManager = new LocationManager(locationChangedQueue);
  ClockLocation currentLocation = locationManager->getLocation();

  blueStuff = new BlueStuff(bluetoothConnectedQueue,prefsChangedQueue,networkChangedQueue,networkStatusQueue,godModeQueue,locationManager);
  blueStuff->startBlueStuff();


  weatherClient = new OpenWeatherMap();
  weatherClient->weatherChangedQueue = weatherChangedQueue;
  weatherClient->setLocation(currentLocation);
  weatherClient->setTimeHorizon(12);
  updateScheduler.addJob(weatherClient,"weather",hourly);

  rainbows.setLocation(currentLocation);

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
      ESP_LOGE(TAG,"Could not connect to DS3231");
    }

    timeSync = new TimeSync(&ds3231, &rtc); // << deliberately leaking these here - should really go smart pointers
    timeSync->performUpdate();
    updateScheduler.addJob(timeSync,"timesync",hourly);
    
  #endif




  // Start the internal RTC and NTP sync
  rtc.begin();

  // Firmware updatedr
  firmwareUpdates = new FirmwareUpdates(firmwareUpdateQueue);
  updateScheduler.addJob(firmwareUpdates,"firmware",daily);

  // Start Update Scheduler
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
bool bluetoothConnected = false;

float_t brightnessAdjust = 1;
bool autoBrightness = true;

void loop() {

  
  // ---- MONITOR QUEUES -----

  // ... preferences
  bool prefsDidChange = false;
  xQueueReceive(prefsChangedQueue, &prefsDidChange, (TickType_t)0 );
  if (prefsDidChange){
    ESP_LOGD(TAG,"PREFS DID CHANGE");
    updateDisplayPreferences();
  }
  
  display.setBrightness(currentBrightness());

  // ... weather
  bool weatherDidChange = false;
  xQueueReceive(weatherChangedQueue, &weatherDidChange, (TickType_t)0 );
  if (weatherDidChange && !godModeSettings.enabled){
    ESP_LOGI(TAG,"Weather did change");
    display.setWeather(weatherClient->horizonWeather);
    rainbows.setWeather(weatherClient->rainbowWeather);
  }

  // ... location
  bool locationDidChange = false;
  xQueueReceive(locationChangedQueue, &locationDidChange, (TickType_t)0 );
  if (locationDidChange){
    ESP_LOGI(TAG,"Location did change");
    ClockLocation location = locationManager->getLocation();
    weatherClient->setLocation(location);
    rainbows.setLocation(location);
    rtc.setTimeZone(location.timeZone);
  }

  // Bluetooth Connected
  bool bluetoothDidConnect = false;
  if (xQueueReceive(bluetoothConnectedQueue, &bluetoothDidConnect, (TickType_t)0 )){
    bluetoothConnected = bluetoothDidConnect;
    display.setDeviceState(bluetoothDidConnect ? bluetooth : ok);
  }

  // ... firmware updates
  
  xQueueReceive(firmwareUpdateQueue, &fwUpdateStatus, (TickType_t)0 );

  switch (fwUpdateStatus) {
    case idle:
      break;
    case updating:
      blueStuff->stopBlueStuff();
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
      blueStuff->startBlueStuff();
      display.displayMessage("Update failed... sorry :(",bad);
      fwUpdateStatus = idle;
      break;
    case complete:
      display.displayMessage("Update complete! Restarting...", good);
      ESP.restart();
      break;
  }

  // ... God Mode

  if (xQueueReceive(godModeQueue, &godModeSettings, (TickType_t)0 )){
    if (godModeSettings.enabled){
      display.setWeather(godModeSettings.weather);
    } else {
      display.setWeather(weatherClient->horizonWeather);
      rainbows.setWeather(weatherClient->rainbowWeather);
    }
  }
  


  // TOUCH SENSOR (Doesn't work)

  #if defined(TOUCH_SENSOR)
  // Check for touches... (This doesn't work on the clockbrain ... but it doesn't need to work!)
  long touchPeriod = detectTouchPeriod();
  if ( touchPeriod > 500 && touchPeriod < 10000){
    display.displayMessage("That tickles",rando);
  }
  touchPeriod = detectTouchPeriod();
  if (touchPeriod > 10000 && touchPeriod < 15000){
    display.displayMessage("Keep holding for restart",bad);
  }
  touchPeriod = detectTouchPeriod();
  if (touchPeriod > 15000){
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

  if (!bluetoothConnected){
    if (millis() > lastRandomMessageTime + nextMessageDelay){
      
      const char *message = randoMessage();
      ESP_LOGD(TAG,"Random message %s",message);
      display.displayMessage(message, rainbow);
      display.displayMessage(message, rainbow);

      lastRandomMessageTime = millis();
      nextMessageDelay = 1000 * 60 * random(5,59);
    }
  }


  DateTime time = rtc.now();
  DateTime localTime = rtc.localTime();
  double decimalTime = rtc.decimalTime();
  display.setRainbows(rainbows.rainbowProbability(time));
  display.setTime(localTime);
  display.setDecimalTime(decimalTime);
  display.frameLoop();

  sensibleDelay(1000/FPS);

  
}

void updateDisplayPreferences(){
  Preferences preferences = Preferences();
  preferences.begin("clocklet", false);
  
  String timeStyleString = preferences.getString("time_style");
  ESP_LOGD(TAG,"NEW TIME STYLE***** %s",timeStyleString);
  if (timeStyleString == "24 Hour"){
    display.setTimeStyle(twentyFourHour);
  } else if (timeStyleString == "12 Hour"){
    display.setTimeStyle(twelveHour);
  } else if (timeStyleString == "Decimal"){
    display.setTimeStyle(decimal);
  }

  brightnessAdjust = preferences.getFloat("brightness",0.5f);
  autoBrightness = preferences.getBool("autoBrightness",true);

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
  if (!autoBrightness){
    return brightnessAdjust;
  }

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

  lightReading = (lightReading + (brightnessAdjust / 3)) * (brightnessAdjust * 2);
  
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
  uart_tx_wait_idle(0);
  uint64_t microseconds = milliseconds * 1000;

  esp_err_t err = esp_sleep_enable_timer_wakeup( microseconds );
  if (err == ESP_ERR_INVALID_ARG){
    ESP_LOGE(TAG,"Sleep timer wakeup invalid");
    return;
  }
  err = esp_light_sleep_start();

  if (err == ESP_ERR_INVALID_STATE){
    ESP_LOGE(TAG,"Trying to sleep: Invalid state error");
  }
  ESP_LOGI(TAG,"AWAKE");
}

void espShutdown(){
  // display.setStatusMessage("LOW BATTERY");
  // ESP_LOGI(TAG,"LOW BATTERY shutting down");
  // esp_deep_sleep_start();
}
#endif

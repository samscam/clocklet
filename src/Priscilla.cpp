#include "Priscilla.h"
#include "tests.h"
#include "settings.h"
#include "Messages.h"

#include "Displays/DebugDisplay.h"
// CONFIGURATION  --------------------------------------

// Time zone adjust (in hours from utc)
int32_t tzAdjust = 0;

// Set to false to display time in 12 hour format, or true to use 24 hour:

// ----------- Display
// RGBDigit display = RGBDigit();
//Adafruit7 display = Adafruit7();
DebugDisplay display = DebugDisplay();

// ----------- RTC

//RTC_DS3231 rtc = RTC_DS3231();
#include "TimeThings/ESP32Rtc.h"
RTC_ESP32 rtc = RTC_ESP32();



// ---------- Networking

// #if defined(ESP32) // Oh dear - it works differently
// WiFiClientSecure client;
// #else
WiFiClient client;
// #endif

MetOffice *weatherClient = new MetOffice(client);




// SETUP  --------------------------------------

void setup() {
  delay(4000);
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
 while (!Serial) {
   ; // wait for serial port to connect. Needed for native USB port only
 }
  randomSeed(analogRead(0));

  analogReadResolution(12);

  for (int i = 0; i<10 ; i++){
    updateBrightness();
  }

  Serial.println("Clock starting!");
  Wire.begin();
  display.setup();

  // runDemo();

  display.displayMessage("Everything is awesome");

  while (!setupWifi()){}

  rtc.begin();
}

// LOOP  --------------------------------------
unsigned long lastUpdateTime = 0;
unsigned long nextUpdateDelay = 0;
unsigned long lastRandomMessageTime = millis();
unsigned long nextMessageDelay = 1000 * 60 * 30;
unsigned long lastDailyUpdate = 0;
unsigned int fuzz = random(5,300);

DateTime lastTime = 0;

void loop() {
  updateBrightness();

  DateTime time = rtc.now();
  time = time + TimeSpan(dstAdjust(time) * 3600);
  time = time + TimeSpan(tzAdjust * 3600);

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
  // Possible fail when starting at zero
  if (time.minute() != lastTime.minute()){
    display.setTime(time);
    lastTime = time;
  }

  display.frameLoop();

  delay(1000/FPS);

  // FastLED.delay(1000/FPS);

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

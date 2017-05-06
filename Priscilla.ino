
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <RTClib.h>
#include <RGBDigit.h>
#include <ArduinoJson.h>

// CONFIGURATION  --------------------------------------

// Time zone adjust
int utcAdjust = 1 * 3600;

// Set to false to display time in 12 hour format, or true to use 24 hour:
#define TIME_24_HOUR      true

// Keep track of the hours, minutes, seconds displayed by the clock.
// Start off at 0:00:00 as a signal that the time should be read from
// the DS1307 to initialize it.
int hours = 0;
int minutes = 0;
int seconds = 0;

// Remember if the colon was drawn on the display so it can be blinked
// on and off every second.
bool blinkColon = false;


// ----------- Display

#define DIGIT_COUNT 4
RGBDigit rgbDigit(DIGIT_COUNT, 11);
uint8_t brightness;

// ----------- RTC

// Create display and DS1307 objects.  These are global variables that
// can be accessed from both the setup and loop function below.

RTC_DS1307 rtc = RTC_DS1307();

// ----------- WIFI

int status = WL_IDLE_STATUS;
char ssid[] = "***REMOVED***";  //  your network SSID (name)
char pass[] = "***REMOVED***";       // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)

// ----------- TIME SERVER

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServer(130, 88, 202, 49);//ntp2a.mcc.ac.uk
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
//IPAddress timeServer(194,35,252,7); //chronos.csr.net

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

// ----------- RANDOM MESSAGES

static char* messages[] = {
  "commit",
  "best nest",
  "aline is a piglet",
  "all your base are belong to us",
  "what time is it      adventure time     ",
  "sarah is very nice",
  "do the ham dance",
  "nuts to brexit",
  "bobbins",
  "sam is really clever",
  "chris cant do this",
};

#define numMessages (sizeof(messages)/sizeof(char *)) //array size  

int randoMinute = random(0,59);

// ------------ TYPES

struct Colour {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

// ----------- COLOUR
const Colour WHITE = {128,128,128};
const Colour RED = {64, 0, 0};
const Colour ORANGE = {64, 53, 0};
const Colour YELLOW = {64, 64, 0};
const Colour GREEN = {0, 128, 15};
const Colour LIGHT_BLUE = {80, 80, 128};
const Colour BLUE = {0, 40, 128};
const Colour DARK_BLUE = {20, 0, 40};
const Colour GRAY = {64,64,64};
const Colour DARK_GRAY = {32,32,32};
const Colour BROWN = {55,55,30};
const Colour OTHER_BROWN = {40,60,0};

// DEFAULT RAINBOW
const Colour RAINBOW[5] = {
    RED,ORANGE,GREEN,BLUE,WHITE
};

Colour currentColours[5];

// ---------- WEATHER

char server[] = "datapoint.metoffice.gov.uk";
char resource[] = "/public/data/val/wxfcs/all/json/351207?res=daily&key=***REMOVED***"; // http resource
const unsigned long HTTP_TIMEOUT = 10000;  // max respone time from server
const size_t MAX_CONTENT_SIZE = 1024;       // max size of the HTTP response
WiFiClient client;

// ---------- WEATHER TYPE COLOUR PATTERNS

const Colour weatherTypeColours[][5] = {
  {DARK_BLUE,DARK_BLUE,DARK_BLUE,DARK_BLUE,WHITE}, // 0 - clear night
  {YELLOW,YELLOW,YELLOW,YELLOW,WHITE},             // 1 - sunny day
  {DARK_BLUE,DARK_GRAY,DARK_BLUE,DARK_GRAY,WHITE},           // 2 - Partly cloudy night
  {YELLOW,GRAY,YELLOW,GRAY,WHITE},                 // 3 - Partly cloudy day
  {WHITE,WHITE,WHITE,WHITE,WHITE},                 // 4 - unused
  {WHITE,GRAY,WHITE,GRAY,WHITE},                 // 5 - mist
  {BROWN,BROWN,BROWN,BROWN,WHITE},                 // 6 - fog
  {GRAY,BROWN,GRAY,BROWN,WHITE},                 // 7 - cloudy
  {BROWN,BROWN,BROWN,BROWN,WHITE},                 // 8 - overcast
  {BLUE,DARK_BLUE,BLUE,DARK_BLUE,WHITE},                 // 9 - light rain shower (night)
  {RED,ORANGE,GREEN,BLUE,WHITE},        // 10 - light rain shower (day)
  {LIGHT_BLUE,LIGHT_BLUE,LIGHT_BLUE,LIGHT_BLUE,WHITE},  // 11 - Drizzle
  {LIGHT_BLUE,LIGHT_BLUE,LIGHT_BLUE,LIGHT_BLUE,WHITE}, // 12 - Light rain
  {DARK_BLUE,BLUE,DARK_BLUE,BLUE,WHITE}, // 13 - Heavy rain shower (night)
  {YELLOW,BLUE,YELLOW,BLUE,WHITE},// 14 - Heavy rain shower (day)
  {BLUE,BLUE,BLUE,BLUE,WHITE},// 15 - Heavy rain
  {DARK_BLUE,WHITE,DARK_BLUE,WHITE,WHITE},// 16 - Sleet shower (night)
  {YELLOW,WHITE,YELLOW,WHITE,WHITE},// 17 - Sleet shower (day)
  {WHITE,WHITE,WHITE,WHITE,WHITE},// 18 - Sleet
  {DARK_BLUE,WHITE,DARK_BLUE,WHITE,WHITE},// 19 - Hail shower (night)
  {YELLOW,WHITE,YELLOW,WHITE,WHITE},// 20 - Hail shower (day)
  {WHITE,WHITE,WHITE,WHITE,WHITE},// 21 - Hail
  {DARK_BLUE,WHITE,DARK_BLUE,WHITE,WHITE},// 22 - Light snow shower (night)
  {YELLOW,WHITE,YELLOW,WHITE,WHITE},// 23 - Light snow shower (day)
  {WHITE,WHITE,WHITE,WHITE,WHITE},// 24 - Light snow
  {DARK_BLUE,WHITE,DARK_BLUE,WHITE,WHITE},// 25 - Heavy snow shower (night)
  {WHITE,YELLOW,WHITE,WHITE,WHITE},// 26 - Heavy snow shower (day)
  {WHITE,WHITE,WHITE,WHITE,WHITE},// 27 - Heavy snow
  {DARK_BLUE,ORANGE,BLUE,DARK_BLUE,WHITE},// 28 - Thunder shower (night)
  {BLUE,DARK_GRAY,BLUE,ORANGE,WHITE},// 29 - Thunder shower (day)
  {DARK_GRAY,BLUE,ORANGE,YELLOW,WHITE},// 30 - Thunder
};



// SETUP  --------------------------------------

void setup() {

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }
  randomSeed(analogRead(0));

  Serial.println("Clock starting!");

  rgbDigit.begin();
  rgbDigit.clearAll();

  memcpy(currentColours,RAINBOW,5*3);
  
  // Get the time from the DS1307.
  updateTimeFromRTC();
  
  // Set brightness
  updateBrightness();

  scrollText("everything is awesome");
  
  updateTimeFromRTC();
  displayTimeRGB();
  
  setupWifi();

  

  updateTimeFromRTC();
  displayTimeRGB();
  
  if (status == WL_CONNECTED){
  // Get the time from NTP.
  updateRTCTimeFromNTP();
  }
  updateTimeFromRTC();

  int currentWeather = fetchWeather();
  memcpy(currentColours,weatherTypeColours[currentWeather],5*3);
  updateTimeFromRTC();
  displayTimeRGB();
  
}

// LOOP  --------------------------------------
int fakeWeather = 0;

void loop() {
  // Loop function runs over and over again to implement the clock logic.
  
  // Check if it's the top of the hour and get a new time reading
  // from the DS1307.  This helps keep the clock accurate by fixing
  // any drift.
  

  if (seconds == 0) {
    if (minutes == 0) {
      randoMinute = random(0,59);
      updateBrightness();
      
      // Get the time from NTP.
      updateRTCTimeFromNTP();

      if (hours % 3 == 0){
        int currentWeather = fetchWeather();
        memcpy(currentColours,weatherTypeColours[currentWeather],5*3);
      }
    }
    
    if (minutes == randoMinute){
      randoMessage();
    }

    // Get the time from the DS1307.
    updateTimeFromRTC();
  }

  
  
  displayTimeRGB(currentColours);
  
  // Pause for a second for time to elapse.  This value is in milliseconds
  // so 1000 milliseconds = 1 second.
  delay(1000);

  //   Now increase the seconds by one.
  seconds += 1;
  // If the seconds go above 59 then the minutes should increase and
  // the seconds should wrap back to 0.
  if (seconds > 59) {
    seconds = 0;
    minutes += 1;
    // Again if the minutes go above 59 then the hour should increase and
    // the minutes should wrap back to 0.
    if (minutes > 59) {
      minutes = 0;
      hours += 1;
      // Note that when the minutes are 0 (i.e. it's the top of a new hour)
      // then the start of the loop will read the actual time from the DS1307
      // again.  Just to be safe though we'll also increment the hour and wrap
      // back to 0 if it goes above 23 (i.e. past midnight).
      if (hours > 23) {
        hours = 0;
      }
    }
  }

  // Loop code is finished, it will jump back to the start of the loop
  // function again!
}

// MARK: NETWORK STUFF --------------------------------------

void setupWifi(){
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8,7,4,2);
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    scrollText_fail("WiFi shield not present");
    return;
  }

  
  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  status = WiFi.begin(ssid, pass);
  
  // Wait for the connection to solidify
  while ( status == WL_IDLE_STATUS ) {
    status = WiFi.status();
    Serial.println(status);
    delay(20);
  }
  switch (status) {
    case WL_CONNECT_FAILED:
      scrollText_fail("connect failed");
      break;
    case WL_DISCONNECTED:
      scrollText_fail("disconnected");
      break;
    case WL_NO_SSID_AVAIL:
      scrollText_fail("no ssid");
      break;
    case WL_CONNECTED:
      scrollText("wifi good");
      break;
  }

  printWiFiStatus();

}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}



// MARK: TIME SYNC STUFF --------------------------------------

void updateTimeFromRTC(){
    DateTime now = rtc.now();
    // Print out the time for debug purposes:
    Serial.print("Read date & time from DS1307: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    // Now set the hours and minutes.
    hours = now.hour();
    minutes = now.minute();
    seconds = now.second();
}


void updateRTCTimeFromNTP(){
  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);
  
  unsigned long timeout = 2000;
  int maxRetries = 4;
  int retries = 0;
  
  unsigned long startMillis = millis();
  unsigned long timech = millis();
  sendNTPpacket(timeServer); // send an NTP packet to a time server
  // wait to see if a reply is available

  int packet = 0;
  
  while( packet == 0 ){
    packet = Udp.parsePacket();
    if (millis() - timech >= timeout) {
      retries ++;
      if (retries >= maxRetries ) {
        Serial.print("Didn't get a packet back... skipping sync...");
        scrollText_fail("failed to update ntp");
        return;
      }
      timech = millis();
      sendNTPpacket(timeServer);
    }
  }

  Serial.print("Roundtrip Time:");
  Serial.println(millis() - startMillis);
  
  Serial.println("packet received");
  // We've received a packet, read the data from it
  Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  Serial.print("Seconds since Jan 1 1900 = " );
  Serial.println(secsSince1900);

  // now convert NTP time into everyday time:
  Serial.print("Unix time = ");
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  unsigned long epoch = secsSince1900 - seventyYears;
  // print Unix time:
  Serial.println(epoch);


  // print the hour, minute and second:
  Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
  Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
  Serial.print(':');
  if ( ((epoch % 3600) / 60) < 10 ) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    Serial.print('0');
  }
  Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
  Serial.print(':');
  if ( (epoch % 60) < 10 ) {
    // In the first 10 seconds of each minute, we'll want a leading '0'
    Serial.print('0');
  }
  Serial.println(epoch % 60); // print the second

  
  rtc.adjust(DateTime(epoch) + utcAdjust);
  
  Serial.print("Time to adjust time:");
  Serial.println(millis() - startMillis);

  scrollText("synchronised");
}


// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("Sending NTP packet");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)

  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}


// MARK: DISPLAY THINGS --------------------------------------


void displayTimeRGB(){
  displayTimeRGB(currentColours);
}

void displayTimeRGB(Colour colours[5]){
  int digit[4];
  
  int h = hours;
  digit[0] = h/10;                      // left digit
  digit[1] = h - (h/10)*10;             // right digit
    int m = minutes;
  digit[2] = m/10;
  digit[3] = m - (m/10)*10;

  for (int i = 0; i<4 ; i++){
    rgbDigit.setDigit(digit[i], i, colours[i].red, colours[i].green, colours[i].blue); // show on digit 0 (=first). Color is rgb(64,0,0).
  }


  blinkColon = (seconds % 2) == 0;
  if (blinkColon) {
    rgbDigit.clearDot(1);               // clear dot on digit 3 (=fourth)
  } else {
    rgbDigit.showDot(1, colours[4].red, colours[4].green, colours[4].blue);    // show dot on digit 1 (=second). Color is rgb(64,0,0).
  }
}


void updateBrightness(){
  
  // Brightness adjust
  if ( hours >= 22 || hours <= 5 ) {
    brightness = 10; 
   } else if ( hours >= 21 || hours <= 6 ) {
    brightness = 20;
   } else if ( hours == 20 || hours == 7 ) {
    brightness = 55;
  } else {
    brightness = 128;
  }
  
  rgbDigit.setBrightness(brightness);
}

void randoMessage(){
  int messageIndex = random(0,numMessages-1);
  char* randoMessage = messages[messageIndex];

  // Do it three times
  scrollText(randoMessage);
  scrollText(randoMessage);
  scrollText(randoMessage);
}

void scrollText(char *stringy){
  scrollText(stringy, GREEN);
}

void scrollText_fail(char *stringy){
  scrollText(stringy, RED);
}

void scrollText(char *stringy, Colour colour){
  
  //clockDisplay.drawColon(0);
  char charbuffer[DIGIT_COUNT] = { 0 };
  int origLen = strlen(stringy);
  int extendedLen = origLen + DIGIT_COUNT;
  char res[extendedLen];
  memset(res, 0, extendedLen);
  memcpy(res,stringy,origLen);
  
  Serial.print("SCROLLING: ");
  Serial.println(stringy);
  int i;
  for ( i = 0; i < extendedLen ; i++ ) {
    for ( int d = 0; d < DIGIT_COUNT ; d++ ) {
      if (d == 3) {
        charbuffer[d] = res[i];
      } else {
        charbuffer[d] = charbuffer[d+1];
      }
      if (charbuffer[d] == 0){
        rgbDigit.clearDigit( d );
      } else {
        rgbDigit.setDigit(charbuffer[d], d, colour.red, colour.green, colour.blue);
      }
    }

    delay(200);
    
  }
  
  
}

// MARK: WEATHER FETCHING

int fetchWeather(){
  if (connect(server)) {
    if (sendRequest(server, resource) && skipResponseHeaders()) {
      int weatherType = readReponseContent();
      Serial.print("Weather type: ");
      Serial.println(weatherType);
      disconnect();
      return(weatherType);
    }
  }
  scrollText_fail("Weather fetch failed");
  return 0;
}

// Open connection to the HTTP server
bool connect(const char* hostName) {
  Serial.print("Connect to ");
  Serial.println(hostName);

  bool ok = client.connect(hostName, 80);

  Serial.println(ok ? "Connected" : "Connection Failed!");
  return ok;
}

// Send the HTTP GET request to the server
bool sendRequest(const char* host, const char* resource) {
  Serial.print("Request ");
  Serial.println(host);
  Serial.println(resource);
  // close any connection before send a new request.

  client.print("GET ");
  client.print(resource);
  client.println(" HTTP/1.0");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close");
  client.println();
  
  return true;
}


// Skip HTTP headers so that we are at the beginning of the response's body
bool skipResponseHeaders() {
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";

  client.setTimeout(HTTP_TIMEOUT);
  bool ok = client.find(endOfHeaders);

  if (!ok) {
    Serial.println("No response or invalid response!");
  }

  return ok;
}

int readReponseContent() {
  // Compute optimal size of the JSON buffer according to what we need to parse.
  // This is only required if you use StaticJsonBuffer.
  const size_t BUFFER_SIZE =
      JSON_OBJECT_SIZE(8)    // the root object has 8 elements
      + JSON_OBJECT_SIZE(5)  // the "address" object has 5 elements
      + JSON_OBJECT_SIZE(2)  // the "geo" object has 2 elements
      + JSON_OBJECT_SIZE(3)  // the "company" object has 3 elements
      + MAX_CONTENT_SIZE;    // additional space for strings

  // Allocate a temporary memory pool
  DynamicJsonBuffer jsonBuffer(BUFFER_SIZE);

  JsonObject& root = jsonBuffer.parseObject(client);

  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return false;
  }

  // Here were copy the strings we're interested in
  int weatherType = root["SiteRep"]["DV"]["Location"]["Period"][0]["Rep"][0]["W"];
  
  return weatherType;
}

// Close the connection with the HTTP server
void disconnect() {
  Serial.println("Disconnect");
  client.stop();
}



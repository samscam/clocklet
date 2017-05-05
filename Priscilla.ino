
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <RTClib.h>
#include <RGBDigit.h>


// CONFIGURATION  --------------------------------------

// Time zone adjust
int utcAdjust = 1 * 3600;

// Set to false to display time in 12 hour format, or true to use 24 hour:
#define TIME_24_HOUR      true

// I2C address of the display.  Stick with the default address of 0x70
// unless you've changed the address jumpers on the back of the display.
//#define DISPLAY_ADDRESS   0x70
//Adafruit_7segment clockDisplay = Adafruit_7segment();
#define DIGIT_COUNT 4
RGBDigit rgbDigit(DIGIT_COUNT, 11);       // uses default pin 12

// Create display and DS1307 objects.  These are global variables that
// can be accessed from both the setup and loop function below.

RTC_DS1307 rtc = RTC_DS1307();


// Keep track of the hours, minutes, seconds displayed by the clock.
// Start off at 0:00:00 as a signal that the time should be read from
// the DS1307 to initialize it.
int hours = 0;
int minutes = 0;
int seconds = 0;

// Remember if the colon was drawn on the display so it can be blinked
// on and off every second.
bool blinkColon = false;


int status = WL_IDLE_STATUS;
char ssid[] = "***REMOVED***";  //  your network SSID (name)
char pass[] = "***REMOVED***";       // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServer(130, 88, 202, 49);//ntp2a.mcc.ac.uk
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
//IPAddress timeServer(194,35,252,7); //chronos.csr.net

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

uint8_t brightness;

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

  
}

// LOOP  --------------------------------------

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
    }
    
    if (minutes == randoMinute){
      randoMessage();
    }

    // Get the time from the DS1307.
    updateTimeFromRTC();
  }

  displayTimeRGB();
  

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
  //Serial.println("2");
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  //Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  //Serial.println("4");
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  //Serial.println("5");
  Udp.endPacket();
  //Serial.println("6");
}


// MARK: DISPLAY THINGS --------------------------------------

void displayTimeRGB(){
  int h = hours;
  int h1 = h/10;                      // left digit
  int h2 = h - (h/10)*10;             // right digit
  rgbDigit.setDigit(h1, 0, 64, 0, 0); // show on digit 0 (=first). Color is rgb(64,0,0).
  rgbDigit.setDigit(h2, 1, 64, 53, 0);
  int m = minutes;
  int m1 = m/10;
  int m2 = m - (m/10)*10;
  rgbDigit.setDigit(m1, 2, 0, 64, 15);
  rgbDigit.setDigit(m2, 3, 0, 20, 64);
  rgbDigit.showDot(1, 64, 64, 64);    // show dot on digit 1 (=second). Color is rgb(64,0,0).

  blinkColon = (seconds % 2) == 0;
  if (blinkColon) {
    rgbDigit.clearDot(1);               // clear dot on digit 3 (=fourth)
  } else {
    rgbDigit.showDot(1, 64, 64, 64);
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
  scrollText(stringy, 129, 240, 12);
}

void scrollText_fail(char *stringy){
  scrollText(stringy, 255, 0, 0);
}

void scrollText(char *stringy, uint8_t red, uint8_t green, uint8_t blue){
  
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
        rgbDigit.setDigit(charbuffer[d], d, red, green, blue);
      }
    }

    delay(200);
    
  }
  
  
}



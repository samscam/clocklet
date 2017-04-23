
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <RTClib.h>
#include "Adafruit_LEDBackpack.h"

// ********* CONFIGURATION ************

// Time zone adjust
int utcAdjust = 1 * 3600;

// Set to false to display time in 12 hour format, or true to use 24 hour:
#define TIME_24_HOUR      true

// I2C address of the display.  Stick with the default address of 0x70
// unless you've changed the address jumpers on the back of the display.
#define DISPLAY_ADDRESS   0x70


// Create display and DS1307 objects.  These are global variables that
// can be accessed from both the setup and loop function below.
Adafruit_7segment clockDisplay = Adafruit_7segment();
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

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;


// ********* SETUP ************

void setup() {
  
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8,7,4,2);
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }

  Serial.println("Clock starting!");

  // Setup the display.
  clockDisplay.begin(DISPLAY_ADDRESS);
  
  scrollText("everything is awesome");
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  
  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    
    // Wait for the connection to solidify
    while ( status == WL_IDLE_STATUS ) {
      status = WiFi.status();
      delay(20);
    }
  }

  scrollText("wifi good");

  Serial.println("Connected to wifi");
  printWiFiStatus();

  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);

  
  // Get the time from NTP.
  updateRTCTimeFromNTP();
  
  // Get the time from the DS1307.
  updateTimeFromRTC();

  // Set brightness
  updateBrightness();

  randomSeed(analogRead(0));
}

int randoMinute = random(0,59);

void loop() {
  // Loop function runs over and over again to implement the clock logic.
  
  // Check if it's the top of the hour and get a new time reading
  // from the DS1307.  This helps keep the clock accurate by fixing
  // any drift.
  

  if (seconds == 0) {
    if (minutes == 0) {
      
      updateBrightness();
      
      // Get the time from NTP.
      updateRTCTimeFromNTP();
    }
    
    if (minutes == randoMinute){
      scrollText("aline is a piglet    ");
      randoMinute = random(0,59);
    }

    // Get the time from the DS1307.
    updateTimeFromRTC();
  }
  

  
  // Show the time on the display by turning it into a numeric
  // value, like 3:30 turns into 330, by multiplying the hour by
  // 100 and then adding the minutes.
  int displayValue = hours*100 + minutes;

  // Do 24 hour to 12 hour format conversion when required.
  if (!TIME_24_HOUR) {
    // Handle when hours are past 12 by subtracting 12 hours (1200 value).
    if (hours > 12) {
      displayValue -= 1200;
    }
    // Handle hour 0 (midnight) being shown as 12.
    else if (hours == 0) {
      displayValue += 1200;
    }
  }

  // Now print the time value to the display.
  clockDisplay.print(displayValue, DEC);

  // Add zero padding when in 24 hour mode and it's midnight.
  // In this case the print function above won't have leading 0's
  // which can look confusing.  Go in and explicitly add these zeros.
  if (TIME_24_HOUR && hours == 0) {
    // Pad hour 0.
    clockDisplay.writeDigitNum(1, 0);
    // Also pad when the 10's minute is 0 and should be padded.
    if (minutes < 10) {
      clockDisplay.writeDigitNum(3, 0);
    }
  }

  // Blink the colon by flipping its value every loop iteration
  // (which happens every second).
  blinkColon = (seconds % 2) == 0;
  clockDisplay.drawColon(blinkColon);

  // Now push out to the display the new values that were set above.
  clockDisplay.writeDisplay();

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
        scrollText("failed to update ntp");
        delay(1000);
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
  
  scrollText("synchronised");
  
  Serial.print("Time to adjust time:");
  Serial.println(millis() - startMillis);
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

void updateBrightness(){
  
  // Brightness adjust
  uint8_t brightness;
  if ( hours >= 22 || hours <= 5 ) {
    brightness = 0; 
   } else if ( hours >= 21 || hours <= 6 ) {
    brightness = 2;
   } else if ( hours == 20 || hours == 7 ) {
    brightness = 6;
  } else {
    brightness = 15;
  }
  
  clockDisplay.setBrightness(brightness);

}



static const uint8_t letters[] = {
  B00000000, // [space]
  B01110111, // a
  B01111100, // b
  B00111001, // c
  B01011110, // d
  B01111001, // e
  B01110001, // f
  B01101111, // g
  B01110100, // h
  B00110000, // i
  B00001110, // j
  B01110110, // k
  B00111000, // l
  B11010100, // m crap
  B01010100, // n
  B01011100, // o
  B01110011, // p
  B01100111, // q
  B01010000, // r
  B01101101, // s
  B01111000, // t
  B00011100, // u
  B00011000, // v crap
  B01001111, // w crap
  B01000110, // x crap
  B01101110, // y
  B01011011, // z
};


void scrollText(char *stringy){
  clockDisplay.drawColon(0);
  uint8_t charbuffer[4] = { 0,0,0,0 };
  int stringLength = strlen(stringy);
  Serial.print("SCROLLING: ");
  Serial.println(stringy);
  int i;
  for ( i = 0; i < stringLength; i++ ) {
    
    charbuffer[0] = charbuffer[1];
    charbuffer[1] = charbuffer[2];
    charbuffer[2] = charbuffer[3];
    charbuffer[3] = stringy[i];

    int let = 0;
    for (int a = 0; a < 4; a++) {
      uint8_t thisChar = charbuffer[a] - 96;
      if ( thisChar >= 26 ) { thisChar = 0; }
      clockDisplay.writeDigitRaw(let,letters[thisChar]); 
      let++;
      if (let == 2) { let++; }
    }
    clockDisplay.writeDisplay();
    delay(200);
    
  }
  
  
}



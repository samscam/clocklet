#include "Priscilla.h"

// CONFIGURATION  --------------------------------------

// Time zone adjust (in hours from utc)
int32_t tzAdjust = 0;

// Set to false to display time in 12 hour format, or true to use 24 hour:
#define TIME_24_HOUR      true



// ----------- RTC

// Create display and RTC_DS3231 objects.  These are global variables that
// can be accessed from both the setup and loop function below.

RTC_DS3231 rtc = RTC_DS3231();

// ----------- TIME SERVER

unsigned int localPort = 2390;      // local port to listen for UDP packets

// IPAddress timeServer(130, 88, 202, 49);//ntp2a.mcc.ac.uk
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
//IPAddress timeServer(194,35,252,7); //chronos.csr.net
IPAddress timeServer(129, 250, 35, 251); //some other one from the pool
// char timeServerPool[] = "pool.ntp.org";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;


// SETUP  --------------------------------------

void setup() {
  delay(4000);
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
 // while (!Serial) {
 //   ; // wait for serial port to connect. Needed for native USB port only
 // }
  randomSeed(analogRead(0));

  Serial.println("Clock starting!");
  Wire.begin();
  rtc.begin();

  initDisplay();
  updateBrightness();

  scrollText("everything is awesome");

  setupWifi();

}

// LOOP  --------------------------------------
unsigned long lastUpdateTime = 0;
unsigned long nextUpdateDelay = 0;
unsigned long lastRandomMessageTime = millis();
unsigned long nextMessageDelay = 1000 * 60 * 30;
unsigned long lastDailyUpdate = 0;
unsigned int fuzz = random(5,300);

weather currentWeather = { -1, 0, 1, 1};

void loop() {

  updateBrightness();

  DateTime time = rtc.now();

  if ( time.unixtime() > lastDailyUpdate + (60 * 60 * 24) + fuzz ) {
    updatesDaily();
    lastDailyUpdate = time.unixtime();
  }

  if (millis() > lastUpdateTime + nextUpdateDelay){

    updatesHourly();
    lastUpdateTime = millis();
    nextUpdateDelay = 1000 * 60 * 60; // 60 mins
  }

  if (millis() > lastRandomMessageTime + nextMessageDelay){

    randoMessage();
    lastRandomMessageTime = millis();
    nextMessageDelay = 1000 * 60 * random(5,59);
  }

  showTime();

  FastLED.delay(1000/FPS);

}

// MARK: UPDATE CYCLE ---------------------------------------

void updatesHourly(){
  Serial.println("Hourly update");
  currentWeather = fetchWeather();
}

void updatesDaily(){
  Serial.println("Daily update");
  updateRTCTimeFromNTP();
  generateDSTTimes(rtc.now().year());
}


void showTime(){
  DateTime time = rtc.now();
  time = time + TimeSpan(dstAdjust(time) * 3600);
  time = time + TimeSpan(tzAdjust * 3600);
  displayTime(time, currentWeather);
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



// MARK: TIME SYNC STUFF --------------------------------------


void updateRTCTimeFromNTP(){
  if ( !connectWifi() ){
    return;
  }

  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);

  unsigned long timeout = 5000;

  unsigned long startMillis = millis();
  unsigned long timech = millis();
   // send an NTP packet to a time server
  // wait to see if a reply is available

  int packet = 0;

  while( packet == 0 ){
    sendNTPpacket(timeServer);
    packet = Udp.parsePacket();
    if (millis() - timech >= timeout) {
      Serial.print("Didn't get a packet back... skipping sync...");
      scrollText_fail("failed to update ntp");
      return;
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

  rtc.adjust(DateTime(epoch));

  Serial.print("Time to adjust time:");
  Serial.println(millis() - startMillis);

  scrollText("synchronised");
}


// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress& address)
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

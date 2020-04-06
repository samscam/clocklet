#include "NTP.h"
#include "network.h"

// ----------- TIME SOURCE

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServer(130, 88, 202, 49);//ntp2a.mcc.ac.uk
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
//IPAddress timeServer(194,35,252,7); //chronos.csr.net
// IPAddress timeServer(129, 250, 35, 251); //some other one from the pool
// char timeServerPool[] = "pool.ntp.org";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

// MARK: TIME SYNC STUFF --------------------------------------


bool timeFromNTP(DateTime &time){
  if ( !reconnect() ){
    Serial.println("NTP: Wifi is actually down now");
    return false;
  }

  Serial.println("\nNTP: Starting connection to server...");
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
      // display.displayMessage("failed to update ntp");
      return false;
    }
  }

  Serial.print("NTP: Roundtrip Time:");
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

  // rtc.adjust(DateTime(epoch));
  time = DateTime(epoch);
  return true;

  // Serial.print("Time to adjust time (ms):");
  // Serial.println(millis() - startMillis);

}


// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress& address)
{
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

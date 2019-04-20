#include <WiFi.h>
#include <WiFiUdp.h>
#include <RTCLib.h>

void timeFromNTP(DateTime& time);
// private
// void updateRTCTimeFromNTP();
void sendNTPpacket(IPAddress& address);

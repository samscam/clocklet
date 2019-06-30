
#ifndef PRISCILLA_NTP
#define PRISCILLA_NTP

#include <WiFi.h>
#include <WiFiUdp.h>
#include <RTClib.h>

bool timeFromNTP(DateTime &time);
// private
// void updateRTCTimeFromNTP();
void sendNTPpacket(IPAddress &address);

#endif
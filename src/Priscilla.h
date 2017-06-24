#include <WiFi101.h>

// ------------ TYPES

struct Colour {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

// Function declarations

void performUpdates(bool forceAll);

bool setupWifi();
bool connectWifi();
bool connectWifi();
void printWiFiStatus();

void updateTimeFromRTC();
void updateRTCTimeFromNTP();

void sendNTPpacket(IPAddress& address);

void displayTimeRGB();
void displayTimeRGB(Colour colours[5]);

void updateBrightness();

void randoMessage();
void scrollText(const char *stringy);
void scrollText_fail(const char *stringy);
void scrollText(const char *stringy, Colour colour);

int fetchWeather();
bool connect(const char* hostName);
bool sendRequest(const char* host, const char* resource);
bool skipResponseHeaders();
int readReponseContent();
void disconnect();

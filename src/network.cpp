#include "network.h"
#include "display.h"
#include "settings.h"

// MARK: NETWORK STUFF --------------------------------------

bool setupWifi(){

  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8,7,4,2);

  return connectWifi();

}

uint32_t lastConnectAttempt = 0;

bool connectWifi(){
  int status = WiFi.status();

  switch (status) {
    case WL_CONNECTED:
      //scrollText("wifi good");
      return true;
      break;
    case WL_NO_SHIELD:
      //scrollText_fail("no wifi shield");
      return false;
      break;
    case WL_CONNECT_FAILED:
      //scrollText_fail("connect failed");
      break;
    case WL_CONNECTION_LOST:
      break;
    case WL_DISCONNECTED:
      //scrollText_fail("disconnected");
      break;
    case WL_NO_SSID_AVAIL:
      //scrollText_fail("network not found");
      break;

  }

  int64_t timedif = millis() - lastConnectAttempt;
  if (  lastConnectAttempt == 0 ) {
    // millis has rolled over or it's our first shot
  } else {

    // Avoid trying if the last attempt was within the last minute
    if ( timedif < (1000 * 60) ) {
      //scrollText_fail("not long enough");
      return false;
    }
  }

  lastConnectAttempt = millis();

  // If we have fallen through, try connecting

  static char ssid[] = NETWORK_SSID;
  static char pass[] = NETWORK_PASSWORD;


  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);

  // Wait for the connection to solidify
  while ( status == WL_IDLE_STATUS ) {
    status = WiFi.status();
    Serial.println(status);
    delay(1000);
  }

  printWiFiStatus();

  switch (status) {
    case WL_CONNECTED:
      scrollText("wife good");
      return true;
      break;
    case WL_CONNECT_FAILED:
      scrollText_fail("connect failed");
      break;
    case WL_DISCONNECTED:
      scrollText_fail("disconnected");
      break;
    case WL_NO_SSID_AVAIL:
      scrollText_fail("network not found");
      break;

  }

  return false;
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

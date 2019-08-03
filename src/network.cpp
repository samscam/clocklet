#include "network.h"
#include "settings.h"
#include <esp_wifi.h>
#include <esp_wifi_types.h>
// MARK: NETWORK STUFF --------------------------------------

bool setupWifi(){
#if defined(ARDUINO_ARCH_SAMD)
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8,7,4,2);
#endif
  #if defined(ESP32)
    wifi_country_t country = {"GB", 1, 13, 127, WIFI_COUNTRY_POLICY_AUTO};
    esp_wifi_set_country(&country);
    btStop();
  #endif

  return connectWifi();

}

uint32_t lastConnectAttempt = 0;

bool connectWifi(){

    int status = WiFi.status();

    switch (status) {
      // VVV ---- We are good :)
      case WL_CONNECTED:
        return true;
        break;
      
      case WL_NO_SHIELD:
#if defined(ESP32)
        // Initial state - needs to connect
        break;
#else
        // VVV ---- This is never going to work :(
        return false;
        break;
#endif
      // VVV ---- attempt a (re)connection on fallthrough
      case WL_CONNECT_FAILED:
        break;
      case WL_CONNECTION_LOST:
        break;
      case WL_DISCONNECTED:
        break;
      case WL_NO_SSID_AVAIL:
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

    #ifndef ESP32
    // clear out any existing session - presuming harmless only on the Atwinc
    WiFi.end();
    #endif

    lastConnectAttempt = millis();

    // If we have fallen through, try connecting

    static char ssid[] = NETWORK_SSID;
    static char pass[] = NETWORK_PASSWORD;


    // attempt to connect to WiFi network:
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);

    bool connected = waitForWifi(6000);

    bool didSetSleep = WiFi.setSleep(true);
    if (!didSetSleep){
      Serial.print("Failed to set wifi sleep mode");
    }
    printWiFiStatus();
    return connected;
}


bool reconnect(){
  // Restart wifi if we are power-saving
  esp_wifi_start();
  WiFi.reconnect();
  return waitForWifi(1000);
}

bool waitForWifi(uint32_t milliseconds){
  
  uint64_t timeout = millis() + milliseconds;
  while (millis() < timeout){
    int status = WiFi.status();

    switch (status) {
      case WL_NO_SHIELD:
        Serial.println("WL_NO_SHIELD");
        break;
      case WL_IDLE_STATUS:
        Serial.println("WL_IDLE_STATUS");
        break;
      case WL_NO_SSID_AVAIL:
        Serial.println("WL_NO_SSID_AVAIL");
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("WL_SCAN_COMPLETED");
        break;
      case WL_CONNECTED:
        Serial.println("WL_CONNECTED");
        return true;
        break;
      case WL_CONNECT_FAILED:
        Serial.println("WL_CONNECT_FAILED");
        break;
      case WL_CONNECTION_LOST:
        Serial.println("WL_CONNECT_FAILED");
        break;
      case WL_DISCONNECTED:
        Serial.println("WL_CONNECT_FAILED");
        break;
    }
  }
  Serial.println("Wifi timeout");
  return false;
}

void stopWifi(){
  esp_wifi_stop();
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

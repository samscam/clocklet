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
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
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

    #ifdef ESP32
    int64_t wifiTimeout = millis() + 6000;
    while (millis() < wifiTimeout){
      if ( status != WL_CONNECTED ) {
        status = WiFi.status();
        Serial.println(status);
        delay(200);
      } else {
        wifiTimeout = 0;
      }
    }
    #else
    // Wait for the connection to solidify
    while ( status == WL_IDLE_STATUS ) {
      status = WiFi.status();
      Serial.println(status);
      delay(1000);
    }
    #endif


    printWiFiStatus();

    switch (status) {
      case WL_CONNECTED:
        // scrollText("wife good");
        return true;
        break;
      case WL_CONNECT_FAILED:
        // scrollText_fail("connect failed");
        break;
      case WL_DISCONNECTED:
        // scrollText_fail("disconnected");
        break;
      case WL_NO_SSID_AVAIL:
        // scrollText_fail("network not found");
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

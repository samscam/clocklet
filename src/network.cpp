#include "network.h"
#include "Secrets/Credentials.h"
#include <esp_wifi.h>
#include <esp_wifi_types.h>
// MARK: NETWORK STUFF --------------------------------------

bool setupWifi(){

  wifi_country_t country = {"GB", 1, 13, 127, WIFI_COUNTRY_POLICY_AUTO};
  esp_wifi_set_country(&country);
  btStop();

  return connectWifi();

}

uint32_t lastConnectAttempt = 0;

bool connectWifi(){

    wl_status_t status = WiFi.status();
    if (status == WL_CONNECTED){
      return true;
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

    status = WiFi.begin();

    bool connected = waitForWifi(6000);

    bool didSetSleep = WiFi.setSleep(true);
    if (!didSetSleep){
      Serial.print("Failed to set wifi sleep mode");
    }
    printWiFiStatus();
    return connected;
}

bool wifiStopped = false;
bool reconnect(){
  // Restart wifi if we are power-saving
  if (wifiStopped){
    esp_wifi_start();
    WiFi.reconnect();
  }
  return waitForWifi(2000);
}

bool waitForWifi(uint32_t milliseconds){
  
  uint64_t timeout = millis() + milliseconds;
  wl_status_t lastStatus = WL_NO_SHIELD;
  while (millis() < timeout){
    wl_status_t status = WiFi.status();
    if (status != lastStatus){
      lastStatus = status;

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
    vTaskDelay(10);
  }
  Serial.println("Wifi timeout");
  return false;
}

void stopWifi(){
  ESP_LOGI(TAG, "Stopping wifi");
  esp_wifi_stop();
  wifiStopped = true;
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

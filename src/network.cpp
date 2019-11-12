#include "network.h"
#include "Secrets/Credentials.h"
#include <esp_wifi.h>
#include <esp_wifi_types.h>
// MARK: NETWORK STUFF --------------------------------------



uint32_t lastConnectAttempt = 0;
bool wifiStopped = false;

bool reconnect(){
  // Restart wifi if we are power-saving
  if (wifiStopped){
    esp_wifi_start();
  }
  if (!WiFi.isConnected()){
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
          printWiFiStatus();
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